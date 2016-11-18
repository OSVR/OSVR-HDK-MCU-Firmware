/*
 * Copyright 2016 OSVR and contributors.
 * Copyright 2016 Dennis Yeh.
 * Copyright 2016 Sensics, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#include "GlobalOptions.h"
#ifdef SVR_IS_HDK_20

#include "HDK2.h"

// From the Variant config directory.
#include "EDID_Data.h"

#include "my_hardware.h"

#include "DeviceDrivers/BNO070.h"
//#include "ui.h"
//#include "uart.h"
#include "Console.h"

// asf headers
#include <delay.h>
#include <nvm.h>

// standard headers
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <inttypes.h>  // for stdint.h-matching format specifier macros

#define AUO_INIT_DELAY_US 1000
#define EDID_ADDR_YEAR 0x11
#define EDID_ADDR_WEEK 0x10
#define EDID_ADDR_SN_0 0x0C
#define EDID_ADDR_SN_1 0x0D
#define EDID_ADDR_SN_2 0x0E
#define EDID_ADDR_SN_3 0x0F
#define EDID_ADDR_CS_0 0x7F
#define EDID_ADDR_CS_1 0xFF

#define CORE_KEY "Ctlh618#"
#define CORE_KEY_LENGTH 8
#define CORE_KEY_RETRY_TIME 3

static inline uint8_t ascii_to_dec_8(uint8_t *buf) { return (buf[0] - '0') * 10 + (buf[1] - '0'); }
static unsigned char s_sn[SN_LENGTH];
static bool s_triedSerial = false;
static bool s_serialValid = false;

/// Writes serial number data already placed in s_sn
static void eep_write_sn(void);
/// returns true if the serial number is valid
static bool eep_read_sn(void);
static inline void UpdateResolutionDetection(void)
{
	// dummy function to satisfy old code.
}
/// Converts four ascii decimal digits to a uint16_t
/// @note Assumes valid input!
static inline uint16_t ascii_to_dec_16(uint8_t *buf)
{
	return (uint16_t)(ascii_to_dec_8(buf) * UINT16_C(100)) + (uint16_t)(ascii_to_dec_8(buf + 2));
}

int TC358870_i2c_Read(uint16_t RegNum, uint8_t *data)
{
	static status_code_t nResult = 0;
	uint8_t data_received[2] = {0x00, 0x00};

	twi_package_t packet_read = {
	    .addr[0] = (uint8_t)((RegNum >> 8) & 0xFF),  // TWI slave memory address data
	    .addr[1] = (uint8_t)(RegNum & 0xFF),         // TWI slave memory address data
	    .addr_length = sizeof(uint16_t),             // TWI slave memory address data size
	    .chip = TC358870_ADDR,                       // TWI slave bus address
	    .buffer = data_received,                     // transfer data source buffer
	    .length = sizeof(data_received)              // transfer data size (bytes)
	};

	nResult = twi_master_read(TC358870_TWI_PORT, &packet_read);

	if (nResult == STATUS_OK)
	{
		*data = data_received[0];
		return TC358870_OK;
	}
	WriteLn("TC358870_i2c_Read err");
	return TC358870_ERROR;
}

int TC358870_i2c_Write(uint16_t RegNum, uint32_t nValue, int nLength)
{
	static status_code_t nResult = 0;
	uint8_t sendData[4] = {0x0};

	if (nLength == 1)
	{
		sendData[0] = (uint8_t)nValue;
	}
	else if (nLength == 2)
	{
		sendData[0] = (uint8_t)(nValue & 0xFF);
		sendData[1] = (uint8_t)((nValue >> 8) & 0xFF);
	}
	else if (nLength == 4)
	{
		sendData[0] = (uint8_t)(nValue & 0xFF);
		sendData[1] = (uint8_t)((nValue >> 8) & 0xFF);
		sendData[2] = (uint8_t)((nValue >> 16) & 0xFF);
		sendData[3] = (uint8_t)((nValue >> 24) & 0xFF);
	}
	else
		return TC358870_ERROR;

	twi_package_t packet = {
	    .addr[0] = (uint8_t)((RegNum >> 8) & 0xFF),  // TWI slave memory address data
	    .addr[1] = (uint8_t)(RegNum & 0xFF),         // TWI slave memory address data
	    .addr_length = sizeof(uint16_t),             // TWI slave memory address data size
	    .chip = TC358870_ADDR,                       // TWI slave bus address
	    .buffer = sendData,                          // transfer data source buffer
	    .length = nLength,                           // transfer data size (bytes)
	    .no_wait = true                              // return immediately if not available
	};

	nResult = twi_master_write(TC358870_TWI_PORT, &packet);
	if (nResult == STATUS_OK)
		return TC358870_OK;

	WriteLn("TC358870_i2c_Write err");
	return TC358870_ERROR;
}

static inline void debugPrintf(const char *format, ...)
{
	char buffer[256];

	va_list ap;
	va_start(ap, format);
	vsprintf(buffer, format, ap);
	WriteLn(buffer);
	va_end(ap);
}

void OSVR_HDK_EDID(void)
{
	uint8_t cs = 0;
	int i, j;
	unsigned short idx = 0;
	int sn_status;
	uint8_t edid_week;
	uint8_t edid_year;
	uint8_t edid_sn_hex[4] = {0};
	uint32_t edid_sn_dec = 0;
	char buf[20];

	if ((sn_status = eep_read_sn(sn)) != 0)
	{
		// WriteLn ("Read S/N NG.");

		edid_year = 26;  // 26 => 2016.
		edid_week = 1;
	}
	else
	{                                               // parsing s/n info
		edid_year = (ascii_to_dec_8(sn + 2) + 10);  // counting from 1990. year - 1990
		edid_week = ascii_to_dec_8(sn + 4);
		if (edid_week < 1 || edid_week > 53)
		{
			edid_week = 1;
			// WriteLn("Week Out Of Range.\n");
		}
		edid_sn_dec =
		    (uint32_t)(((uint32_t)(sn[10] - '0') * 10000) + ((uint32_t)(sn[11] - '0') * 1000) +
		               ((uint32_t)(sn[12] - '0') * 100) + ((uint32_t)(sn[13] - '0') * 10) + (uint32_t)(sn[14] - '0'));
		edid_sn_hex[0] = (uint8_t)(edid_sn_dec & 0x000000FF);
		edid_sn_hex[1] = (uint8_t)((edid_sn_dec & 0x0000FF00) >> 8);
		edid_sn_hex[2] = (uint8_t)((edid_sn_dec & 0x00FF0000) >> 16);
		edid_sn_hex[3] = (uint8_t)((edid_sn_dec & 0xFF000000) >> 24);
	}

	if (GetDebugLevel() == 0xAA)
	{  // debug message.
		WriteLn("S/N Info...\n");
		sprintf(buf, "Year: %d", edid_year);
		WriteLn(buf);
		sprintf(buf, "Week: %d", edid_week);
		WriteLn(buf);

		sprintf(buf, "int32=%d", sizeof(uint32_t));
		WriteLn(buf);

		sprintf(buf, "S/N: %" PRId32, (uint32_t)edid_sn_dec);
		WriteLn(buf);
		sprintf(buf, "%02X-%02X-%02X-%02X", edid_sn_hex[3], edid_sn_hex[2], edid_sn_hex[1], edid_sn_hex[0]);
		WriteLn(buf);
	}

	// feeding EDID data to TC358870
	for (i = 0; i < 2; i++)
	{
		for (j = 0, cs = 0; j < 0x80; j++, idx++)
		{
			switch (idx)
			{
			case EDID_ADDR_WEEK:
				cs += edid_week;
				TC358870_i2c_Write(0x8C00 + idx, edid_week, 1);
				break;

			case EDID_ADDR_YEAR:
				cs += edid_year;
				TC358870_i2c_Write(0x8C00 + idx, edid_year, 1);
				break;

			case EDID_ADDR_SN_0:
			case EDID_ADDR_SN_1:
			case EDID_ADDR_SN_2:
			case EDID_ADDR_SN_3:
				cs += edid_sn_hex[idx - EDID_ADDR_SN_0];
				TC358870_i2c_Write(0x8C00 + idx, edid_sn_hex[idx - EDID_ADDR_SN_0], 1);
				break;

			case EDID_ADDR_CS_0:
			case EDID_ADDR_CS_1:
				cs = (unsigned char)(0x100 - cs);  // Making EDID page check sum. The 1-byte sum of all 128 bytes in
				                                   // this EDID block shall equal zero.
				TC358870_i2c_Write(0x8C00 + idx, (unsigned char)cs, 1);
				break;

			default:
				cs += EDID_LUT[idx];
				TC358870_i2c_Write(0x8C00 + idx, EDID_LUT[idx], 1);
				break;
			}
		}
	}
}

/*    ==============================================================
        Implement for EDID date code and S/E function.
        1. Access S/N from MCU EEPROM.
        2. Create command to access S/N
        3. Write a function to parse date code and S/N from S/N, offers for EDID
        4. Write a function to calculate EDID check sum sinc date code and S/N are replace.
        5. Insert date code and S/N when feed EDID to TC358870.

        ==============================================================
*/

unsigned char sn[SN_LENGTH];

// If CRC 16 is fine for the performnace, replace it...
static unsigned char check_sum_gen(unsigned char *buf, int length)
{
	int i = 0;
	unsigned char cs = 0;

	for (i = 0; i < length; i++)
	{
		cs += buf[i];
	}

	return cs;
}

void eep_write_sn(void)
{
	sn[SN_LENGTH - 1] = check_sum_gen(sn, SN_LENGTH - 1);  // fill check sum.
	nvm_eeprom_erase_and_write_buffer(EEP_ADDR_SN, sn, SN_LENGTH);
}

int eep_read_sn(unsigned char *buf)
{
	int i = 0;

	for (i = 0; i < SN_LENGTH; i++)
	{
		buf[i] = nvm_eeprom_read_byte(EEP_ADDR_SN + i);
	}

	// check sum verify.
	if (check_sum_gen(buf, SN_LENGTH - 1) != buf[SN_LENGTH - 1])
		return -1;  // check sum verify fail.

	return 0;
}

extern char CommandToExecute[MaxCommandLength + 1];

void ProcessFactoryCommand(void)
{
	int i;
	char OutString[SN_LENGTH];
	static int core_key_retry_time = 0;

	switch (CommandToExecute[1])
	{
	case 's':  // 20160520, fctu, S/N read, write.
	case 'S':
		switch (CommandToExecute[2])
		{
		case 'w':  // Write S/N to eeprom.
		case 'W':
			if (core_key_pass == 0)
				break;
			// write...
			memcpy(sn, CommandToExecute + 3, SN_LENGTH - 1);
			eep_write_sn();

			// verify...
			i = eep_read_sn((unsigned char *)OutString);
			OutString[SN_LENGTH - 1] = '\0';
			sn[SN_LENGTH - 1] = '\0';

			if (i)
				WriteLn("NG");  // check sum error.
			else if (strcmp(OutString, (char *)sn) != 0)
				WriteLn("NG");  // s/n does not match.
			else
				WriteLn("OK");  // s/n match.

			break;

		case 'r':  // Read S/N from eeprom.
		case 'R':
			if (eep_read_sn((unsigned char *)OutString))
				WriteLn("NG");  // send NG to host.
			else
			{
				OutString[SN_LENGTH - 1] = '\0';
				WriteLn(OutString);
			}
			break;
		}
		break;

	case 't':  // for testing purpose.
	case 'T':
		OSVR_HDK_EDID();
		break;

	case 'k':
	case 'K':
		switch (CommandToExecute[2])
		{
		case 'r':  // get key.
		case 'R':
			// WriteLn(CORE_KEK);
			break;

		case 'c':  // match key.
		case 'C':
			if (core_key_retry_time >= CORE_KEY_RETRY_TIME)
			{
				// WriteLn("LOCK");  // for debug.
				WriteLn("NG");  // for someone whom unauthority.
				break;
			}

			memcpy(OutString, CommandToExecute + 3, CORE_KEY_LENGTH);

			if (memcmp(OutString, CORE_KEY, CORE_KEY_LENGTH) == 0)
			{
				core_key_pass = CORE_KEY_PASS_SUCCESS_VALUE;
			}
			else
			{
				core_key_pass = CORE_KEY_PASS_FAIL_VALUE;
			}
			if (CORE_KEY_PASS_SUCCESS_VALUE == core_key_pass)
			{
				core_key_retry_time = 0;
				WriteLn("OK");
			}
			else
			{
				core_key_retry_time++;
				WriteLn("NG");
			}

			break;
		}
		break;
	}
}

void nvm_eeprom_write_byte_(eeprom_addr_t address, uint8_t value)
{
	if (address >= EEP_ADDR_SN && address <= 0x1F)  // 0x08 ~0x1F
		if (core_key_pass == 0)                     // no right to access this area.
			return;

	nvm_eeprom_write_byte(address, value);
}
#endif  // SVR_IS_HDK_20

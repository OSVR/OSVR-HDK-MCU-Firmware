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
#include "DeviceDrivers/Toshiba_TC358870.h"
//#include "ui.h"
//#include "uart.h"
#include "Console.h"
#include "BitUtilsC.h"

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

#define EDID_ADDR_SNSTRING_0 0x4d
#define EDID_ADDR_SNSTRING_1 0x4e
#define EDID_ADDR_SNSTRING_2 0x4f
#define EDID_ADDR_SNSTRING_3 0x50
#define EDID_ADDR_SNSTRING_4 0x51
#define EDID_ADDR_SNSTRING_5 0x52
#define EDID_ADDR_SNSTRING_6 0x53
#define EDID_ADDR_SNSTRING_7 0x54
#define EDID_ADDR_SNSTRING_8 0x55
#define EDID_ADDR_SNSTRING_9 0x56
#define EDID_ADDR_SNSTRING_10 0x57
#define EDID_ADDR_SNSTRING_11 0x58
#define EDID_ADDR_SNSTRING_12 0x59

#ifndef SVR_DEFAULT_MODEL_YEAR
#define SVR_DEFAULT_MODEL_YEAR 16
#endif
#define EDID_20XX_YEAR_TRANSFORM(X) (X + 10)
#define EDID_WEEK_MODEL_YEAR_FLAG 0xFF

#define CORE_KEY "Ctlh618#"
#define CORE_KEY_LENGTH 8
#define CORE_KEY_RETRY_TIME 3

static unsigned char s_sn[SN_LENGTH];
static bool s_triedSerial = false;
static bool s_serialValid = false;

/// Writes serial number data already placed in s_sn
static void eep_write_sn(void);
/// returns true if the serial number is valid
static bool eep_read_sn(void);

/// Converts a single ascii decimal digit to a uint8_t.
/// @note Assumes valid input!
static inline uint8_t ascii_dec_byte(uint8_t c) { return c - '0'; }
/// Converts two ascii decimal digits to a uint8_t
/// @note Assumes valid input!
static inline uint8_t ascii_to_dec_8(uint8_t *buf)
{
	return ascii_dec_byte(buf[0]) * UINT8_C(10) + ascii_dec_byte(buf[1]);
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

#define TC_EDID_BASE UINT16_C(0x8c00)
void OSVR_HDK_EDID(void)
{
	uint8_t edid_week = EDID_WEEK_MODEL_YEAR_FLAG;                         // model year
	uint8_t edid_year = EDID_20XX_YEAR_TRANSFORM(SVR_DEFAULT_MODEL_YEAR);  // 26 => 2016.
	uint32_t edid_sn_numeric = 0;
	uint8_t edid_sn_numeric_bytes[4] = {0, 0, 0, 0};

	bool sn_ok = eep_read_sn();
	if (sn_ok)
	{
		// parsing s/n info
		// Format: CTyywwV001ssss
		// where yy signifies year of mfr 20yy
		// ww is week of mfr
		// and ssss is a serially-assigned number
		edid_year =
		    EDID_20XX_YEAR_TRANSFORM(ascii_to_dec_8(s_sn + 2));  // SN counts from 2000, EDID counts from 1990, so +10
		edid_week = ascii_to_dec_8(s_sn + 4);
		if (edid_week < 1 || edid_week > 53)
		{
			WriteLn("Week Out Of Range.");
			/// Reset date of manufacture to model year instead.
			edid_year = EDID_20XX_YEAR_TRANSFORM(SVR_DEFAULT_MODEL_YEAR);
			edid_week = EDID_WEEK_MODEL_YEAR_FLAG;
		}

		edid_sn_numeric = ascii_to_dec_16(s_sn + 11);
		edid_sn_numeric_bytes[0] = BITUTILS_GET_NTH_LEAST_SIG_BYTE(0, edid_sn_numeric);
		edid_sn_numeric_bytes[1] = BITUTILS_GET_NTH_LEAST_SIG_BYTE(1, edid_sn_numeric);
		edid_sn_numeric_bytes[2] = BITUTILS_GET_NTH_LEAST_SIG_BYTE(2, edid_sn_numeric);
		edid_sn_numeric_bytes[3] = BITUTILS_GET_NTH_LEAST_SIG_BYTE(3, edid_sn_numeric);
	}

#ifdef HDMI_VERBOSE
	if ((GetDebugLevel() & debugHDK2Mask) != 0)
	{  // debug message.
		char buf[50];
		dWriteLn("S/N Info...\n", debugHDK2Mask);
		sprintf(buf, "Year: %" PRId8, edid_year);
		dWriteLn(buf, debugHDK2Mask);
		sprintf(buf, "Week: %" PRId8, edid_week);
		dWriteLn(buf, debugHDK2Mask);

		sprintf(buf, "Numeric S/N: %" PRId32, edid_sn_numeric);
		dWriteLn(buf, debugHDK2Mask);
		sprintf(buf, "%02" PRIX8 "-%02" PRIX8 "-%02" PRIX8 "-%02" PRIX8, edid_sn_numeric_bytes[3],
		        edid_sn_numeric_bytes[2], edid_sn_numeric_bytes[1], edid_sn_numeric_bytes[0]);
		dWriteLn(buf, debugHDK2Mask);
	}
#endif

	// feeding EDID data to TC358870
	/// absolute index in EDID data
	uint16_t idx = 0;
	for (uint8_t i = 0; i < 2; i++)
	{
		// two blocks with separate checksums
		for (uint8_t j = 0, cs = 0; j < 0x80; j++, idx++)
		{
			uint8_t currentByte;
			switch (idx)
			{
			case EDID_ADDR_WEEK:
				currentByte = edid_week;
				break;

			case EDID_ADDR_YEAR:
				currentByte = edid_year;
				break;

			case EDID_ADDR_SN_0:
			case EDID_ADDR_SN_1:
			case EDID_ADDR_SN_2:
			case EDID_ADDR_SN_3:
				currentByte = edid_sn_numeric_bytes[idx - EDID_ADDR_SN_0];
				break;

			case EDID_ADDR_SNSTRING_0:
			case EDID_ADDR_SNSTRING_1:
			case EDID_ADDR_SNSTRING_2:
			case EDID_ADDR_SNSTRING_3:
			case EDID_ADDR_SNSTRING_4:
			case EDID_ADDR_SNSTRING_5:
			case EDID_ADDR_SNSTRING_6:
			case EDID_ADDR_SNSTRING_7:
			case EDID_ADDR_SNSTRING_8:
			case EDID_ADDR_SNSTRING_9:
			case EDID_ADDR_SNSTRING_10:
			case EDID_ADDR_SNSTRING_11:
			case EDID_ADDR_SNSTRING_12:
				if (sn_ok)
				{
					_Static_assert(SN_LENGTH == 16,
					               "If serial number length changes, need to change EDID filling to pad.");
					const uint8_t stringPos = idx - EDID_ADDR_SNSTRING_0;
					// the + 2 is to drop the serial number's constant "CT" prefix to get a 13-byte string.
					currentByte = s_sn[stringPos + 2];
				}
				else
				{
					/// if bad SN read, just use the default.
					currentByte = EDID_LUT[idx];
				}
				break;

			case EDID_ADDR_CS_0:
			case EDID_ADDR_CS_1:
				// Making EDID page check sum. The 1-byte sum of all 128 bytes in
				// this EDID block shall equal zero.
				// 0xFF - cs + 0x01 keeps everything in 8 bit variables, while being functionally equivalent to 0x100 -
				// cs.
				currentByte = UINT8_C(0xFF) - (cs) + UINT8_C(0x01);
				break;

			default:
				currentByte = EDID_LUT[idx];
				break;
			}
			cs += currentByte;
			Toshiba_TC358870_I2C_Write8(TC_EDID_BASE + idx, currentByte);
		}
	}
}

/*    ==============================================================
        Implement for EDID date code and S/E function.
        1. Access S/N from MCU EEPROM.
        2. Create command to access S/N
        3. Write a function to parse date code and S/N from S/N, offers for EDID
        4. Write a function to calculate EDID check sum since date code and S/N are replaced.
        5. Insert date code and S/N when feed EDID to TC358870.

        ==============================================================
*/


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
	s_sn[SN_LENGTH - 1] = check_sum_gen(s_sn, SN_LENGTH - 1);  // fill check sum.
	nvm_eeprom_erase_and_write_buffer(EEP_ADDR_SN, s_sn, SN_LENGTH);
	// Reset these flags so we can validate.
	s_triedSerial = false;
	s_serialValid = false;
}

bool eep_read_sn()
{
	if (s_triedSerial)
	{
		/// early out - only need to read this once per startup.
		return s_serialValid;
	}
	int i = 0;

	for (i = 0; i < SN_LENGTH; i++)
	{
		s_sn[i] = nvm_eeprom_read_byte(EEP_ADDR_SN + i);
	}

	// check sum verify.
	if (check_sum_gen(s_sn, SN_LENGTH - 1) == s_sn[SN_LENGTH - 1])
	{
		s_serialValid = true;
	}
	return s_serialValid;
}

bool eep_get_sn(uint8_t buf[SN_LENGTH])
{
	bool haveSn = eep_read_sn();
	if (!haveSn)
	{
		// null-terminate and get out.
		buf[0] = '\0';
		return false;
	}
	memcpy(buf, s_sn, SN_LENGTH - 1);
	// null-terminate
	buf[SN_LENGTH - 1] = '\0';
	return true;
}

extern char CommandToExecute[MaxCommandLength + 1];

void ProcessFactoryCommand(void)
{
	static int core_key_retry_time = 0;

	switch (CommandToExecute[1])
	{
	case 's':  // 20160520, fctu, S/N read, write.
	case 'S':
		switch (CommandToExecute[2])
		{
		case 'w':  // Write S/N to eeprom.
		case 'W':
		{
			if (core_key_pass == 0)
				break;
			// write...
			char *inputString = CommandToExecute + 3;
			if (strnlen(inputString, SN_LENGTH) != SN_LENGTH - 1)
			{
				WriteLn("NG");
				WriteLn("Serial number wrong length.");
			}
			memcpy(s_sn, inputString, SN_LENGTH - 1);
			eep_write_sn();
			{
				// verify...
				bool sn_success = eep_read_sn();
				if (!sn_success)
				{
					WriteLn("NG");  // check sum error.
				}
				else if (strncmp(inputString, (char *)s_sn, SN_LENGTH - 1) != 0)
				{
					// compare the input to the serial number read (raw) but don't look at the last byte (checksum) or
					// wait for terminator.
					WriteLn("NG");  // s/n does not match.
				}
				else
				{
					WriteLn("OK");  // s/n match.
				}
			}
			break;
		}
		case 'r':  // Read S/N from eeprom.
		case 'R':
		{
			uint8_t OutString[SN_LENGTH];
			if (!eep_get_sn(OutString))
			{
				WriteLn("NG");  // send NG to host.
			}
			else
			{
				WriteLn((char *)OutString);
			}
			break;
		}
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

			if (memcmp(CommandToExecute + 3, CORE_KEY, CORE_KEY_LENGTH) == 0)
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

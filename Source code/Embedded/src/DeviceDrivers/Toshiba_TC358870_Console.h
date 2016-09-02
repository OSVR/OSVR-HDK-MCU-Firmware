/** @file
    @brief Header

    @date 2016

    @author
    Sensics, Inc.
    <http://sensics.com/osvr>
*/

// Copyright 2016 Sensics, Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//        http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef INCLUDED_Toshiba_TC358870_Console_h_GUID_4BFE48BF_5992_4D80_E879_5D88FD604F9C
#define INCLUDED_Toshiba_TC358870_Console_h_GUID_4BFE48BF_5992_4D80_E879_5D88FD604F9C

#include "GlobalOptions.h"
#ifdef SVR_HAVE_TOSHIBA_TC358870
#include "SerialStateMachine.h"
#include "Console.h"
#include "Toshiba_TC358870.h"
#include "HDK2.h"
#include "VideoInput.h"
#include "SvrYield.h"

#include <inttypes.h>  // for the PRI__ format string specifiers matching stdint.h types.

static inline void Toshiba_TC358870_Print_Status(TC358870_Op_Status_t status)
{
	char message[50];
	switch (status)
	{
	case TOSHIBA_TC358770_OK:
		WriteLn("OK");
		break;
	default:
		sprintf(message, "Not OK - return code %d", status);
		WriteLn(message);
	}
}
static inline void Toshiba_TC358870_Console_S(BufWithStatus_t cmd)
{
	char message[50];
	switch (statusBufPeekFront(&cmd))
	{
	/// write
	case 'w':
	case 'W':
	{
		statusBufConsumeByte(&cmd);
		switch (statusBufPeekFront(&cmd))
		{
		case '1':
		{
			statusBufConsumeByte(&cmd);
			uint16_t addr = statusBufConsumeHexDigits4_16(&cmd);
			uint8_t data = statusBufConsumeHexDigits2_8(&cmd);
			if (!cmd.valid)
			{
				WriteLn("write one byte: w1xxxxyy");
				break;
			}
			sprintf(message, "Toshiba_TC358870: write reg [%#06" PRIX16 "] %#04" PRIX8, addr, data);
			WriteLn(message);
			TC358870_Op_Status_t status = Toshiba_TC358870_I2C_Write8(addr, data);
			Toshiba_TC358870_Print_Status(status);
			break;
		}

		case '2':
		{
			statusBufConsumeByte(&cmd);
			uint16_t addr = statusBufConsumeHexDigits4_16(&cmd);
			uint16_t data = statusBufConsumeHexDigits4_16(&cmd);
			if (!cmd.valid)
			{
				WriteLn("write two bytes: w2xxxxyyyy");
				break;
			}
			sprintf(message, "Toshiba_TC358870: write reg [%#06" PRIX16 "] %#06" PRIX16, addr, data);
			WriteLn(message);
			TC358870_Op_Status_t status = Toshiba_TC358870_I2C_Write16(addr, data);
			Toshiba_TC358870_Print_Status(status);
			break;
		}
		default:
			break;
		}
		break;
	}
	case 'r':
	case 'R':
	{
		statusBufConsumeByte(&cmd);
		switch (statusBufPeekFront(&cmd))
		{
		// read 1 byte
		case '1':
		{
			statusBufConsumeByte(&cmd);
			uint16_t addr = statusBufConsumeHexDigits4_16(&cmd);
			if (!cmd.valid)
			{
				WriteLn("read one byte: r1xxxx");
				break;
			}
			sprintf(message, "Toshiba_TC358870: read 8 bits from reg [%#06" PRIX16 "]", addr);
			WriteLn(message);
			uint8_t data = 0;
			TC358870_Op_Status_t status = Toshiba_TC358870_I2C_Read8(addr, &data);
			Toshiba_TC358870_Print_Status(status);
			sprintf(message, "Data: %#04" PRIX8, data);
			WriteLn(message);
			break;
		}

		case '2':
		{
			statusBufConsumeByte(&cmd);
			uint16_t addr = statusBufConsumeHexDigits4_16(&cmd);
			if (!cmd.valid)
			{
				WriteLn("read two bytes: r2xxxx");
				break;
			}

			sprintf(message, "Toshiba_TC358870: read 16 bits from reg [%#06" PRIX16 "]", addr);
			WriteLn(message);

			uint16_t data = 0;
			TC358870_Op_Status_t status = Toshiba_TC358870_I2C_Read16(addr, &data);
			Toshiba_TC358870_Print_Status(status);

			sprintf(message, "Data: %#06" PRIX16, data);
			WriteLn(message);
			break;
		}
		default:
			break;
		}
		break;
	}
	default:
		break;
	}
}

static inline void Toshiba_TC358870_Console_H(BufWithStatus_t cmd)
{
	switch (statusBufPeekFront(&cmd))
	{
	case 'e':
	case 'E':
	{
		WriteLn("Pulse HPD 1ms");
		ioport_set_pin_high(HDMI_HPD);
		barrier();
		svr_yield_ms(1);
		barrier();
		ioport_set_pin_low(HDMI_HPD);
		break;
	}
	case 'h':
	case 'H':
	{
		WriteLn("Pulse HPD 1sec");
		ioport_set_pin_high(HDMI_HPD);
		barrier();
		svr_yield_ms(1000);
		barrier();
		ioport_set_pin_low(HDMI_HPD);
		break;
	}

	default:
		break;
	}
}

#endif

#endif  // INCLUDED_Toshiba_TC358870_Console_h_GUID_4BFE48BF_5992_4D80_E879_5D88FD604F9C

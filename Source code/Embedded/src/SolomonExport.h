/** @file
    @brief Header

    @date 2017

    @author
    Sensics, Inc.
    <http://sensics.com/osvr>
*/

// Copyright 2017 Sensics, Inc.
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

#ifndef INCLUDED_SolomonExport_h_GUID_E2365C7E_6862_4EBB_C206_C5DB412B3F83
#define INCLUDED_SolomonExport_h_GUID_E2365C7E_6862_4EBB_C206_C5DB412B3F83

#include "SvrYield.h"
#include "Console.h"

#define HEXPRINTER_OUTPUT_LINE(x) WriteLn(x)
#include "HexPrinter.h"

static inline void solomon_export(uint8_t deviceId)
{
	Solomon_t *sol = solomon_get_channel(deviceId);
	solomon_select(sol);
	for (uint8_t addr = 0xB0; addr < 0xf8; ++addr)
	{
		if (addr == 0xbf)
		{
			// skip the packet drop register
			continue;
		}
		// Moving it up a nibble so that there's room for these 16-bit registers in output.
		uint16_t shiftedAddr = ((uint16_t)(addr)) << 4;
		uint16_t data = solomon_read_reg_2byte(sol, addr);
		uint8_t dataBytes[] = {(uint8_t)(data & 0xff), (uint8_t)(data >> 8)};
		hexprinter_output_data_record(shiftedAddr, sizeof(dataBytes), dataBytes);
		svr_yield_ms(200);
	}
	solomon_deselect(sol);
	hexprinter_output_eof_record();
}
#endif  // INCLUDED_SolomonExport_h_GUID_E2365C7E_6862_4EBB_C206_C5DB412B3F83

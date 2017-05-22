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

#ifndef INCLUDED_HexPrinter_h_GUID_F40A9597_8606_45AE_233E_C4388E207CC9
#define INCLUDED_HexPrinter_h_GUID_F40A9597_8606_45AE_233E_C4388E207CC9

// Internal Includes
// - none

// Library/third-party includes
// - none

// Standard includes
// - none

#ifndef HEXPRINTER_OUTPUT_LINE
#error "Must define HEXPRINTER_OUTPUT_LINE for this to work."
#endif
#ifndef HEXPRINTER_MAX_DATA_BYTES
#define HEXPRINTER_MAX_DATA_BYTES 16
#endif

// two chars per byte, null terminated
#define HEXPRINTER_DATA_CHARS_SIZE ((2 * HEXPRINTER_MAX_DATA_BYTES) + 1)
// colon (1), byte count(2), address (4), record type (2), data (2 * HEXPRINTER_MAX_DATA_BYTES), and checksum (2), null
// terminated (1)
#define HEXPRINTER_MAX_CHARS (1 + 2 + 4 + 2 + (2 * HEXPRINTER_MAX_DATA_BYTES) + 2 + 1)

/// Turns the lower nibble of the given byte into hex.
static inline char hexprinter_impl_lower_nibble_to_hex(uint8_t nibble)
{
	static const uint8_t smallestAlpha = 0x0a;
	/// make sure it's a nibble.
	nibble &= 0x0f;
	if (nibble < smallestAlpha)
	{
		// arabic numeral
		return (char)(nibble + (uint8_t)'0');
	}
	// a-f
	static const uint8_t aOffset = ((uint8_t)('A') - smallestAlpha);
	return (char)(nibble + aOffset);
}

static inline char hexprinter_impl_upper_nibble_to_hex(uint8_t b)
{
	return hexprinter_impl_lower_nibble_to_hex(b >> 4);
}

typedef struct Hexprinter_State_Struct
{
	char* buf;
	uint8_t sum;
} Hexprinter_State;

static inline void hexprinter_impl_append_char_unchecksummed(Hexprinter_State* state, char c)
{
	*(state->buf) = c;
	++(state->buf);
}

static inline void hexprinter_impl_append_byte(Hexprinter_State* state, uint8_t b)
{
	hexprinter_impl_append_char_unchecksummed(state, hexprinter_impl_upper_nibble_to_hex(b));
	hexprinter_impl_append_char_unchecksummed(state, hexprinter_impl_lower_nibble_to_hex(b));
	state->sum += b;
}

static inline void hexprinter_impl_do_output(uint8_t recordType, uint16_t addr, uint8_t len, const uint8_t* data)
{
	char dataBytes[HEXPRINTER_DATA_CHARS_SIZE] = {0};

	/// start buf at the beginning, and sum at 0
	Hexprinter_State state = {&(dataBytes[0]), 0};
	hexprinter_impl_append_char_unchecksummed(&state, ':');

	hexprinter_impl_append_byte(&state, len);

	{
		uint8_t highAddress = (uint8_t)((addr >> 8) & 0xff);
		hexprinter_impl_append_byte(&state, highAddress);
	}
	{
		uint8_t lowAddress = (uint8_t)(addr & 0xff);
		hexprinter_impl_append_byte(&state, lowAddress);
	}

	hexprinter_impl_append_byte(&state, recordType);
	if (len > 0)
	{
		for (uint8_t i = 0; i < len; ++i)
		{
			hexprinter_impl_append_byte(&state, data[i]);
		}
	}
	// Compute the checksum - two's complement of the sum so far.
	uint8_t checksum = (~(state.sum)) + 1;
	// no longer matters that this method affects the sum member
	hexprinter_impl_append_byte(&state, checksum);
	// null terminate
	hexprinter_impl_append_char_unchecksummed(&state, '\0');
	HEXPRINTER_OUTPUT_LINE(dataBytes);
}
static inline void hexprinter_output_data_record(uint16_t addr, uint8_t len, const uint8_t* data)
{
	static const uint8_t RECORD_TYPE = 0x00;
	hexprinter_impl_do_output(RECORD_TYPE, addr, len, data);
}
static inline void hexprinter_output_eof_record(void)
{
	static const uint8_t RECORD_TYPE = 0x01;
	/// little overkill since the record is effectively fixed (address of 0 is conventional) but shares implemenation
	/// for validation's sake.
	// hexprinter_impl_do_output(RECORD_TYPE, 0x0000, 0, NULL);
}

#endif  // INCLUDED_HexPrinter_h_GUID_F40A9597_8606_45AE_233E_C4388E207CC9

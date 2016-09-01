/** @file
    @brief Header defining some bit manipulation for the C language. Sadly, this
    means macros instead of function templates, and associated reduced type safety.
    Sorry about that.

    Must be c-safe!

    @date 2016

    @author
    Sensics, Inc.
    <http://sensics.com/osvr>
*/

/*
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
*/

#ifndef INCLUDED_BitUtilsC_h_GUID_FD9633A7_921B_4886_272B_1519C6D424B5
#define INCLUDED_BitUtilsC_h_GUID_FD9633A7_921B_4886_272B_1519C6D424B5

/* Internal Includes */
/* none */

/* Library/third-party includes */
/* none */

/* Standard includes */
#include <stdint.h>
#include <limits.h>

/// Given a uint8_t value and corresponding 8-bit mask with just one bit set, checks to see if that bit is set.
/// Note: assumes the mask you pass has only one bit set! For the general case, see bitUtils_checkMask
static bool bitUtils_checkBit(uint8_t value, uint8_t mask);
/// Checks that all the bits in the mask are set in the given value. (uint8_t)
static bool bitUtils_checkMask(uint8_t value, uint8_t mask);

static inline bool bitUtils_checkBit(uint8_t value, uint8_t mask) { return (value & mask) != 0 ? true : false; }
/// Generic macro-based version of bitUtils_checkBit
#define BITUTILS_CHECKBIT(VALUE, MASK) (((VALUE) & (MASK)) != 0x0)

/// Checks that all the bits in the mask are set in the given value. (uint8_t)
static inline bool bitUtils_checkMask(uint8_t value, uint8_t mask)
{
/// Factored out because I'm not sure what will give the best performance.
#if 0
	return (value & mask) == mask;
#else
	// xor should flip all masked bits from 1 to 0, if they were set, then the and should give us only any
	// previously-unset bits in the masked bits that remain. This lets us compare against 0. Who knows, the compiler may
	// be clever enough to turn the first approach into this.
	return ((value ^ mask) & mask) == 0x0;
#endif
}

/// Generic macro-based version of bitUtils_checkMask
#define BITUTILS_CHECKMASK(VALUE, MASK) ((((VALUE) ^ (MASK)) & (MASK)) == 0x0)

/// Get the nth least significant byte of a given value (the nth byte in little-endian format)
#define BITUTILS_GET_NTH_LEAST_SIG_BYTE(BYTENUM, VALUE) \
	((uint8_t)((((BYTENUM) == 0) ? (VALUE) : ((VALUE) >> ((BYTENUM)*CHAR_BIT))) & UINT8_MAX))

static inline __always_inline uint8_t bitUtils_lowByte(uint16_t val) { return (uint8_t)(val & UINT8_MAX); }
static inline __always_inline uint8_t bitUtils_highByte(uint16_t val)
{
	return (uint8_t)((val >> CHAR_BIT) & UINT8_MAX);
}

/// Get a value with the lowest BITS bits set, where bits < 8.
#define BITUTILS_GET_SET_LOW_BITS_MAX8(BITS) (UINT8_MAX >> (CHAR_BIT - (BITS)))
/// Get a value with the lowest BITS bits set, where BITS < 16.
#define BITUTILS_GET_SET_LOW_BITS_MAX16(BITS) (UINT16_MAX >> ((2 * CHAR_BIT) - (BITS)))

/// Mask off all but the BITS low bits of VAL, where BITS < 8.
#define BITUTILS_KEEP_LOW_BITS_MAX8(BITS, VAL) (BITUTILS_GET_SET_LOW_BITS_MAX8(BITS) & (VAL))
/// Mask off all but the BITS low bits of VAL, where BITS < 16.
#define BITUTILS_KEEP_LOW_BITS_MAX16(BITS, VAL) (BITUTILS_GET_SET_LOW_BITS_MAX16(BITS) & (VAL))

/// Portable equivalent of _BV
#define BITUTILS_BIT(B) (0x01 << (B))

#endif

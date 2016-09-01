/*
 * SerialStateMachine.h
 *
 * Created: 9/1/2013 6:57:04 PM
 *  Author: Sensics
 */

#ifndef SERIALSTATEMACHINE_H_
#define SERIALSTATEMACHINE_H_

#include <stdint.h>
#include <stdbool.h>

extern bool CommandReady;  // true if a command is ready to be executed

void InitSerialState(void);
void ProcessIncomingChar(char CharReceived);
void ProcessCommand(void);

/// Utility function for parsing the first character of a given C string as a
/// hex digit.
uint8_t ParseHexDigitNibble(const char *buf);

/// Utility function for parsing the first two characters of a given C string as
/// a pair of hex digits.
uint8_t ParseHexDigits2_8(const char *buf);

/// Utility function for parsing the first 4 characters of a given C string as
/// hex digits specifying a uint16
uint16_t ParseHexDigits4_16(const char *buf);

typedef struct BufWithStatus
{
	const char *buf;
	bool valid;
} BufWithStatus_t;

/// Create a BufWithStatus structure that can be used to avoid having to check for end of string with these routines.
static inline BufWithStatus_t statusBufCreate(const char *buf) { return (BufWithStatus_t){buf, (buf != NULL)}; }
/// For internal implementation use only (when the buffer has already been checked)
static inline char statusBufPeekFront_Unchecked(BufWithStatus_t const *b) { return *(b->buf); }
/// Returns the front character in the buffer, if valid. If the buffer isn't valid, it returns a \0.
static inline char statusBufPeekFront(BufWithStatus_t const *b)
{
	if (!b || !(b->valid))
	{
		return '\0';
	}
	return *(b->buf);
}

/// For internal implementation use only (when the buffer has already been checked)
static inline void statusBufConsumeByte_Unchecked(BufWithStatus_t *b)
{
	if ('\0' == *(b->buf))
	{
		b->valid = false;
	}
	else
	{
		b->buf++;
	}
}

/// Consumes one byte from the buffer, advancing the pointer if possible and updating the valid field.
static inline void statusBufConsumeByte(BufWithStatus_t *b)
{
	if (!(b->valid))
	{
		return;
	}
	statusBufConsumeByte_Unchecked(b);
}

/// Consume 2 bytes (if available) from the buf with status, parse them as hex, and return the uint8 to which they
/// correspond
uint8_t statusBufConsumeHexDigits2_8(BufWithStatus_t *b);

/// Consume 4 bytes (if available) from the buf with status, parse them as hex, and return the uint16 to which they
/// correspond
uint16_t statusBufConsumeHexDigits4_16(BufWithStatus_t *b);

#endif /* SERIALSTATEMACHINE_H_ */

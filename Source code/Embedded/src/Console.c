/*
 * Console.c
 *
 * Console output routines
 * Created: 11/15/2013 9:27:41 AM
 *  Author: Sensics
 */

#include "Console.h"
#include "USB.h"

// asf headers
#include <udi_cdc.h>

// standard headers
#include <string.h>

uint8_t DebugLevel = 0xff;  // start by opening all debug messages

static const uint8_t s_timeoutCountLimit = 100;

void Write(const char *const Data)
{
	if (!usb_cdc_is_active())
	{
		// Early out if no USB CDC connected.
		return;
	}
	if (!Data)
	{
		// Given a null pointer - that's not very polite.
		return;
	}
	if (!(*Data))
	{
		// Empty string: Just don't print it, nothing to print!
		// If this was from a WriteLn("") - consider WriteEndl instead...
		return;
	}
	{
		/// Though udi_cdc_[multi_]write_buf already waits on the tx-ready status, this timeout prevents us from hanging
		/// if we end up with a full buffer that's not being consumed.
		iram_size_t dataLen = strlen(Data);
		for (uint8_t i = 0; i < s_timeoutCountLimit; ++i)
		{
			if (usb_cdc_should_tx(dataLen))
			{
				// OK, we can transmit now - write the buffer and get out of here.
				udi_cdc_write_buf(Data, dataLen);
				return;
			}
		}
		/// Fallthrough means timed out with a full buffer.
	}
}

void dWrite(const char *const Data, uint8_t DebugMask)

{
	if ((DebugLevel & DebugMask) != 0)
		Write(Data);
}

static const char CR[] = "\n\r";

void WriteLn(const char *const Data)

{
	Write(Data);
	Write(CR);
	// This is what RETIF_SEM in tmdlHdmiRx_local.h and its consumers print. Presumably requires another newline for
	// some reason.
	if (strcmp(Data, "Release local") == 0)
	{
		Write(CR);
	}
}

void WriteEndl(void) { Write(CR); }
void dWriteLn(const char *const Data, uint8_t DebugMask)
{
	if ((DebugLevel & DebugMask) != 0)
		WriteLn(Data);
}
void dWriteEndl(uint8_t DebugMask)
{
	if ((DebugLevel & DebugMask) != 0)
		WriteEndl();
}

void SetDebugLevel(uint8_t NewLevel) { DebugLevel = NewLevel; }

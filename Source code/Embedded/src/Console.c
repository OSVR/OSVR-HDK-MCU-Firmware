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
/// @todo this seems redundant, the logic in udi_cdc_[multi_]write_buf already waits on the tx buffer being ready.
#if 0
		int TimeoutCounter = 0;
		// ATOMIC_BLOCK(ATOMIC_FORCEON)
		//{
		// CDC_Device_SendString(&VirtualSerial_CDC_Interface, Data, strlen(Data));
		//}
		do
		{
			TimeoutCounter++;
		} while (!udi_cdc_multi_is_tx_ready(0) && (TimeoutCounter < 100));
		if (TimeoutCounter > MaxTimerCounter)
			MaxTimerCounter = TimeoutCounter;
#endif
		CDCWriteInProgress = true;
		udi_cdc_write_buf(Data, strlen(Data));
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

/*
 * Console.c
 *
 * Console output routines
 * Created: 11/15/2013 9:27:41 AM
 *  Author: Sensics
 */

#include <asf.h>
#include "Console.h"
#include "string.h"

bool CDCWriteInProgress=false; // true if USB is being used to write console messages

uint8_t DebugLevel=0xff; // start by opening all debug messages

int MaxTimerCounter=0;
void Write(char *const Data)

{
    {
		int TimeoutCounter=0;
        //ATOMIC_BLOCK(ATOMIC_FORCEON)
        //{
        //CDC_Device_SendString(&VirtualSerial_CDC_Interface, Data, strlen(Data));
        //}
		do 
		{
			TimeoutCounter++;
		} while (!udi_cdc_multi_is_tx_ready(0) && (TimeoutCounter<100));
		if (TimeoutCounter>MaxTimerCounter)
			MaxTimerCounter=TimeoutCounter;
		CDCWriteInProgress=true;
        udi_cdc_write_buf(Data, strlen(Data));
    }
}

void dWrite(char *const Data, uint8_t DebugMask)

{
    if ((DebugLevel & DebugMask)!=0)
        Write(Data);
}


void WriteLn(char *const Data)

{
    char*       CR  = "\n\r";

    Write(Data);
    Write(CR);
    if (strcmp(Data, "Release local")==0)
    {
        Write(CR);
    }
}

void dWriteLn(char *const Data, uint8_t DebugMask)

{
    if ((DebugLevel & DebugMask)!=0)
        WriteLn(Data);
}


void SetDebugLevel(uint8_t NewLevel)

{
    DebugLevel=NewLevel;
}

void UpdateCDCBusyStatus(void)

{
	if (udi_cdc_multi_is_tx_ready(0))
		CDCWriteInProgress=false;
}
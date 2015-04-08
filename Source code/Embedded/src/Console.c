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

uint8_t DebugLevel=0xff; // start by opening all debug messages

void Write(char *const Data)

{
    {
        //ATOMIC_BLOCK(ATOMIC_FORCEON)
        //{
        //CDC_Device_SendString(&VirtualSerial_CDC_Interface, Data, strlen(Data));
        //}
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

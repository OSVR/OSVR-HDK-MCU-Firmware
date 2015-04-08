/*
 * Console.h
 *
 * Created: 11/15/2013 9:29:03 AM
 *  Author: Sensics
 */


#ifndef CONSOLE_H_
#define CONSOLE_H_

#include <asf.h>

#define debugI2CReadMask 1
#define debugNXPLineMask 2
#define debugSolomonMask 4

void Write(char *const Data);
void dWrite(char *const Data, uint8_t DebugMask);
void WriteLn(char *const Data);
void dWriteLn(char *const Data, uint8_t DebugMask);
void SetDebugLevel(uint8_t NewLevel);

#endif /* CONSOLE_H_ */
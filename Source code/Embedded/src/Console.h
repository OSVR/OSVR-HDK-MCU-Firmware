/*
 * Console.h
 *
 * Created: 11/15/2013 9:29:03 AM
 *  Author: Sensics
 */

#ifndef CONSOLE_H_
#define CONSOLE_H_

#include <stdbool.h>
#include <stdint.h>

#define debugI2CReadMask 1
#define debugNXPLineMask 2
#define debugSolomonMask 4

void Write(const char *const Data);
void dWrite(const char *const Data, uint8_t DebugMask);
void WriteLn(const char *const Data);
// Just write an end of line.
void WriteEndl(void);
void dWriteLn(const char *const Data, uint8_t DebugMask);
void dWriteEndl(uint8_t DebugMask);
void SetDebugLevel(uint8_t NewLevel);

#endif /* CONSOLE_H_ */
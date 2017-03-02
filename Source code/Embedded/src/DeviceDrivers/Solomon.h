/*
 * Solomon.h
 *
 * Created: 9/7/2013 11:35:02 PM
 *  Author: Sensics
 */

#ifndef SOLOMON_H_
#define SOLOMON_H_

#define SOLOMON_DEPRECATED __attribute__((deprecated))
#include "GlobalOptions.h"
#include "DeviceDrivers/Display.h"
#include "DeviceDrivers/Solomon_SSD2828.h"

/// For the little debug code here
#include "BitUtilsC.h"
#include "Console.h"
#include <stdio.h>  // for sprintf

static inline void solomon_debug_value(const char name[], uint16_t val)
{
	Write(name);
	char hexString[20];
	sprintf(hexString, "0x%04x - ", val);
	Write(hexString);
	const char msg[] = BITUTILS_CSTR_INIT_FROM_U16_TO_BIN(val);
	WriteLn(msg);
}

#include <stdbool.h>
#include <stdint.h>

#ifdef SVR_HAVE_SOLOMON1
#define Solomon1 0
#endif  // SVR_HAVE_SOLOMON1

#ifdef SVR_HAVE_SOLOMON2
#define Solomon2 1
#endif  // SVR_HAVE_SOLOMON2

extern Solomon_t g_solomons[SVR_HAVE_SOLOMON];

__always_inline static Solomon_t* solomon_get_channel(uint8_t channel) { return &(g_solomons[channel]); }
void init_solomon(void);
bool init_solomon_device(uint8_t deviceID);
uint16_t read_solomon(uint8_t channel, uint8_t address);
void write_solomon(uint8_t channel, uint8_t address, uint16_t data);
void write_solomon_pair(uint8_t channel, uint8_t address, uint16_t data1, uint16_t data2);
SOLOMON_DEPRECATED void raise_sdc(uint8_t channel);
SOLOMON_DEPRECATED void lower_sdc(uint8_t channel);
void Solomon_Reset(uint8_t SolomonNum);

/// Writes out the config register to the console.
void Solomon_Dump_All_Config_Debug(const char* loc);
/// Writes out the config register to the console for just one device.
void Solomon_Dump_Config_Debug(uint8_t deviceId, const char* loc);

// read the solomon ID
uint16_t read_Solomon_ID(uint8_t channel);

#endif /* SOLOMON_H_ */

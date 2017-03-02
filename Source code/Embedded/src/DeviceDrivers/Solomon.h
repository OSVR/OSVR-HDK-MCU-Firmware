/*
 * Solomon.h
 *
 * Created: 9/7/2013 11:35:02 PM
 *  Author: Sensics
 */

#ifndef SOLOMON_H_
#define SOLOMON_H_

/// Some functions we want to get rid of.
#define SOLOMON_DEPRECATED(MSG) __attribute__((deprecated(MSG)))

/// Some functions we just don't want to use in Solomon.c
#ifdef SOLOMON_IMPL
#define SOLOMON_INTERNALLY_DEPRECATED(MSG) SOLOMON_DEPRECATED(MSG)
#else
#define SOLOMON_INTERNALLY_DEPRECATED(MSG)
#endif

#include "GlobalOptions.h"
#include "Console.h"
#include "DeviceDrivers/Display.h"
#include "DeviceDrivers/Solomon_SSD2828.h"

/// For the little debug code here
#include "BitUtilsC.h"
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

/// Gets a Solomon instance by index.
/// @returns a pointer to the instance, or null if out of bounds.
__always_inline static Solomon_t* solomon_get_channel(uint8_t channel)
{
	if (channel >= SVR_HAVE_SOLOMON)
	{
		WriteLn("Tried to get out of bounds Solomon!");
		return NULL;
	}
	return &(g_solomons[channel]);
}
void init_solomon(void);

/// Resets and initializes a Solomon by index.
/// @return false if the given device cannot be properly set up (can't deduce read behavior or out of bounds)
///
/// @note Calls solomon_select and solomon_deselect on this device
bool init_solomon_device(uint8_t deviceID);

/// Reads a word from a register address.
///
/// @note Calls solomon_select and solomon_deselect on this device
uint16_t read_solomon(uint8_t channel, uint8_t address)
    SOLOMON_INTERNALLY_DEPRECATED("Use solomon_read_reg, etc instead");

/// Writes a word to a register address.
///
/// @note Calls solomon_select and solomon_deselect on this device
void write_solomon(uint8_t channel, uint8_t address, uint16_t data)
    SOLOMON_INTERNALLY_DEPRECATED("Use solomon_write_reg_2byte/solomon_write_reg_word instead");

/// Writes two words to a single register address
///
/// @note Calls solomon_select and solomon_deselect on this device
void write_solomon_pair(uint8_t channel, uint8_t address, uint16_t data1, uint16_t data2)
    SOLOMON_INTERNALLY_DEPRECATED("Use solomon_write_reg instead");
void raise_sdc(uint8_t channel) SOLOMON_DEPRECATED("Use solomon_write/solomon_read abstractions instead");
void lower_sdc(uint8_t channel) SOLOMON_DEPRECATED("Use solomon_write/solomon_read abstractions instead");
void Solomon_Reset(uint8_t SolomonNum);

/// Writes out the config register to the console for just one device.
///
/// @note Calls solomon_select and solomon_deselect on this device
void Solomon_Dump_Config_Debug(uint8_t deviceId, const char* loc);

/// Writes out the config register to the console for just one device.
/// @pre must call solomon_select on this device
void Solomon_Dump_Config_Debug_New(uint8_t deviceId, Solomon_t const* sol, const char* loc);

/// read the solomon ID - should be 0x2828
///
/// @note Calls solomon_select and solomon_deselect on this device
uint16_t read_Solomon_ID(uint8_t channel);

#endif /* SOLOMON_H_ */

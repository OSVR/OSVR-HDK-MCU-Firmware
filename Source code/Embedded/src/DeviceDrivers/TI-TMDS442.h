/*
 * TI_TMDS442.h
 *

 driver for TI TMDS442 4-TO-2 DVI/HDMI SWITCH
 * Created: 10/20/2013 9:08:04 AM
 *  Author: Sensics
 */

#ifndef TITMDS442_H_
#define TITMDS442_H_

#include "GlobalOptions.h"

#ifdef SVR_HAVE_TMDS442
#include <stdint.h>
#include <stdbool.h>

enum TMDS442_PLUG_SOURCE_BITS
{
	TMDS442_PLUG_SOURCE_A = 0x01 << 0,
	TMDS442_PLUG_SOURCE_B = 0x01 << 1
};

/// Initialize the driver and TDMS442 HDMI switch.
void TMDS442_Init(void);

/// Run the periodic task that checks the input status.
/// @return true if change in input status noticed.
bool TMDS442_Task(void);

/// Force polling of the input status (with console logging)
/// @return true if change in input status noticed.
bool TMDS442_ForcePoll(void);

/// Return a bitfield (with bits defined in TMDS442_PLUG_SOURCE_BITS) indicating which source ports on the switch
/// currently appear to have something connected to them.
uint8_t TMDS442_GetPlugSourceData(void);

/// @name Internal functions exposed for debugging
/// @{
bool TMDS442_ReadReg(uint8_t regNum, uint8_t *NewStatus);
bool TMDS442_ReadInputStatus(uint8_t *newStatus);
bool TMDS442_WriteReg(uint8_t RegNum, uint8_t Value);
void TMDS442_SetInputStatus(uint8_t NewStatus);
/// @}

/// @name Debugging-only functions
/// @{
void TMDS442_EnableVideoA(void);
void TMDS442_DisableVideoA(void);

void TMDS442_EnableVideoB(void);
void TMDS442_DisableVideoB(void);
/// @}

#endif  // SVR_HAVE_TMDS442
#endif  /* TI-TMDS442_H_ */
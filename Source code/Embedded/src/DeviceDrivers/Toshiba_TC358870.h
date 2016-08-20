/*
 * Toshiba_TC358870.h
 *
 * Created: 7/21/2016 8:29:17 AM
 *  Author: Coretronic, Sensics
 */

#ifndef TOSHIBA_TC358870_H_
#define TOSHIBA_TC358870_H_

/*
    HDMI IF Functions

    Coretronic
    FC Tu
*/

#include <stdbool.h>
#include <stdint.h>

/// Sets up the i2c bus, does an initial read, then calls the "black-box" PowerOnSeq libhdk20 function that, among other
/// things, eventually calls the other libhdk20 function TC358870_Init_Receive_HDMI_Signal
void Toshiba_TC358870_Init(void);

/// Wraps libhdk20 function TC358870_Init_Receive_HDMI_Signal
void Toshiba_TC358870_Init_Receiver(void);

/// Returns the number of times that Toshiba_TC358870_Init() has been called.
uint8_t Toshiba_TC358870_Get_Init_Count(void);

/// Triggers a chip reset of the TC358870 - does not do any initialization afterwards
/// (Can't call it just Toshiba_TC358870_Reset, libhdk20 exports a symbol by that name.)
void Toshiba_TC358870_Trigger_Reset(void);

/// Checks the status register to see if the toshiba chip has stable video sync.
bool Toshiba_TC358870_Have_Video_Sync(void);

/// Send a short DSI command with no parameter.
void Toshiba_TC358870_DSI_Write_Cmd_Short(uint8_t cmd);

/// Send a short DSI command with one parameter.
void Toshiba_TC358870_DSI_Write_Cmd_Short_Param(uint8_t cmd, uint8_t param);

/// Send a "long" DSI command with data (may be of length 0)
// void Toshiba_TC358870_DSI_Write_Cmd_Long(uint8_t cmd, uint16_t len, uint8_t * data);

#endif /* TOSHIBA_TC358870_H_ */
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

/// Returns the number of times that Toshiba_TC358870_Init() has been called.
uint8_t Toshiba_TC358870_Get_Init_Count(void);

/// Triggers a chip reset of the TC358870 - does not do any initialization afterwards
/// (Can't call it just Toshiba_TC358870_Reset, libhdk20 exports a symbol by that name.)
void Toshiba_TC358870_Trigger_Reset(void);

/// @todo currently disabled because it is instead referenced circularly by libhdk20 by a different name
#if 0
/*
    HDMI IF Functions

    Coretronic
    FC Tu
*/
/*
    Function : Toshiba_TC358870_HDMI_IsVideoExisting
    IN : void
    OUT: false: video does not exist
              true: video exist
*/
bool Toshiba_TC358870_HDMI_IsVideoExisting(void);
#endif

#endif /* TOSHIBA_TC358870_H_ */
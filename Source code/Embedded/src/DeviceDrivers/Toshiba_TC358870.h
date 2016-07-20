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

/*
    Function : Toshiba_TC358870_HDMI_IsVideoExisting
    IN : void
    OUT: false: video does not exist
              true: video exist
*/
bool Toshiba_TC358870_HDMI_IsVideoExisting(void);

#endif /* TOSHIBA_TC358870_H_ */
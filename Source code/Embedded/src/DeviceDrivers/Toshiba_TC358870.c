/*
 * Toshiba_TC358870.c
 *
 * Created: 7/21/2016 8:30:05 AM
 *  Author: Coretronic, Sensics
 */

#include "GlobalOptions.h"

#ifdef SVR_HAVE_TOSHIBA_TC358870

#include "Toshiba_TC358870.h"
#include <stdint.h>
#include <libhdk20.h>

/// @todo Despite this function being empty in the Coretronic fork (and the original contents since being renamed to be
/// namespaced in the mainline), we need to define it libhdk20.a can link against it circularly...
/// (IsVideoExistingPolling contains a reference to it)
void UpdateResolutionDetection(void);
void UpdateResolutionDetection() {}
/// @todo We actually need to export this function as bool HDMI_IsVideoExisting(void) so libhdk20.a can link against it
/// circularly... (IsVideoExistingPolling contains a reference to it)
bool HDMI_IsVideoExisting(void);
bool HDMI_IsVideoExisting()
{
	uint8_t tc_data;

	if (TC358870_i2c_Read(0x8520, &tc_data) != TC358870_OK)  // get SYS_STATUS
		return false;

	if (tc_data != 0x9F)
		return false;

	return true;
}
/*
    Function : Toshiba_TC358870_HDMI_IsVideoExisting
    IN : void
    OUT: 0: video does not exist
              1: video exist

     To read TC358870 register SYS_STATUS (0x8520) to check  video status.
     if the value is 0x9F, means the video is ready, otherwise it's not exist.

*/
bool Toshiba_TC358870_HDMI_IsVideoExisting(void)
{
	uint8_t tc_data;

	if (TC358870_i2c_Read(0x8520, &tc_data) != TC358870_OK)  // get SYS_STATUS
		return false;

	if (tc_data != 0x9F)
		return false;

	return true;
}

#endif  // SVR_HAVE_TOSHIBA_TC358870
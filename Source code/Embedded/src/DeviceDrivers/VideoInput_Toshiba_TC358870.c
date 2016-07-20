/*
 * VideoInput_Toshiba_TC358870.c
 *
 * Created: 7/21/2016 8:03:50 AM
 *  Author: Sensics - based on Coretronic modifications
 */

#include "GlobalOptions.h"

#ifdef SVR_HAVE_TOSHIBA_TC358870

#include <libhdk20.h>
#include "VideoInput_Protected.h"
#include "DeviceDrivers/Toshiba_TC358870.h"
#include "my_hardware.h"
#include "Console.h"
#include "SvrYield.h"

#define SVR_DEBUG_LIBHDK2_BEHAVIOR

/// @todo Despite this function being empty in the Coretronic fork (and the original contents since being renamed to be
/// namespaced in the mainline), we need to define it libhdk20.a can link against it circularly...
/// (IsVideoExistingPolling contains a reference to it)
void UpdateResolutionDetection(void);
void UpdateResolutionDetection()
{
#ifdef SVR_DEBUG_LIBHDK2_BEHAVIOR
	if (VideoInput_Get_Status())
	{
		WriteLn("libhdk20: called UpdateResolutionDetection() when video signal present.");
	}
	else
	{
		WriteLn("libhdk20: called UpdateResolutionDetection() when video signal absent.");
	}
#endif
}

/// @todo We actually need to export this function as bool HDMI_IsVideoExisting(void) so libhdk20.a can link against it
/// circularly... (IsVideoExistingPolling contains a reference to it)
///
/// Yes, this is effectively a duplicate of code in Toshiba_TC358870, except that we've interposed our signal report
/// methods in it.
///
/// IsVideoExistingPolling calls this method.
bool HDMI_IsVideoExisting(void);
bool HDMI_IsVideoExisting()
{
	uint8_t tc_data;

	if (TC358870_i2c_Read(0x8520, &tc_data) != TC358870_OK)
	{  // get SYS_STATUS
		VideoInput_Protected_Report_No_Signal();
		return false;
	}

	if (tc_data != 0x9F)
	{
		VideoInput_Protected_Report_No_Signal();
		return false;
	}

	VideoInput_Protected_Report_Signal();
	return true;
}

void VideoInput_Init(void)
{
	// No separate init required - all done in Display_System_Init() since it's just one chip.
	VideoInput_Protected_Init_Succeeded();
}
void VideoInput_Update_Resolution_Detection(void)
{
	// Using the latest received status (instead of polling the chip again) to determine the status.
	/// @todo Assuming that these devices only take in landscape.
	HDMIStatus = (VideoInput_Get_Status() ? VIDSTATUS_VIDEO_LANDSCAPE : VIDSTATUS_NOVIDEO);
}

void VideoInput_Task(void)
{
	// This method reportedly calls HDMI_IsVideoExisting, so we supply a version that contains calls to
	// VideoInput_Protected_Report_No_Signal() and VideoInput_Protected_Report_Signal()
	IsVideoExistingPolling();
}
void VideoInput_Reset(uint8_t inputId)
{
	if (inputId == 1)
	{
		WriteLn("reset HDMI1");
		ioport_set_pin_low(TC358870_Reset_Pin);
		svr_yield_ms(12);
		ioport_set_pin_high(TC358870_Reset_Pin);
	}
	else
		WriteLn("Wrong HDMI num");
}

static const char LIBHDK2_NOT_SUPPORTED[] = "TC358870 via libhdk20 does not support this feature.";

void VideoInput_Suspend(void) { WriteLn(LIBHDK2_NOT_SUPPORTED); }
void VideoInput_Resume(void) { WriteLn(LIBHDK2_NOT_SUPPORTED); }
void VideoInput_Poll_Status(void)
{
	// does not require separate polling outside the task.
}
static const char LIBHDK2_NO_STATUS[] = "TC358870 via libhdk20 does not report video input status or events!";

// to console
void VideoInput_Report_Status(void) { WriteLn(LIBHDK2_NO_STATUS); }
#endif
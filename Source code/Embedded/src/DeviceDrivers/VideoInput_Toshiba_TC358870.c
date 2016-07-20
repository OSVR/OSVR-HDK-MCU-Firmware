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
	IsVideoExistingPolling();
	// Extract events from the status.
	VideoInput_Protected_Report_Status(Toshiba_TC358870_HDMI_IsVideoExisting());
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
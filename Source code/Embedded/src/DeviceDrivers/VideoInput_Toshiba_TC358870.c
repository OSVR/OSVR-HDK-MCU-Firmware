/*
 * VideoInput_Toshiba_TC358870.c
 *
 * Created: 7/21/2016 8:03:50 AM
 *  Author: Sensics - based on Coretronic modifications
 */

#include "GlobalOptions.h"

#ifdef SVR_HAVE_TOSHIBA_TC358870

#include "VideoInput_Protected.h"
#include "DeviceDrivers/Display.h"
#include "DeviceDrivers/HDK2.h"
#include "DeviceDrivers/Toshiba_TC358870.h"
#include "my_hardware.h"
#include "Console.h"
#include "SvrYield.h"
#include "BitUtilsC.h"

#include <stdio.h>  // for sprintf

#define SVR_DEBUG_LIBHDK2_BEHAVIOR

static bool haveInitOnce = false;

static void VideoInput_Init_Impl(void)
{
	Toshiba_TC358870_Disable_Video_TX();
	// software reset HDMI
	Toshiba_TC358870_I2C_Write16(0x0002, BITUTILS_BIT(8));
	Toshiba_TC358870_I2C_Write16(0x0002, 0);
	Toshiba_TC358870_HDMI_Setup();
}

void VideoInput_Init(void)
{
	if (haveInitOnce)
	{
		// This is a repeat init - presumably from serial console - so we'll actually call over to the TC358870 driver
		// (used in Display_System_Init()) since that's where the meat of initializing the chip happens.
		// Toshiba_TC358870_Base_Init();
		bool haveVideo = VideoInput_Get_Status();
		VideoInput_Init_Impl();
		if (haveVideo)
		{
			Toshiba_TC358870_Enable_Video_TX();
		}
	}
	else
	{
		// start the chip, if it hasn't been started.
		Toshiba_TC358870_Init_Once();

		VideoInput_Init_Impl();
		haveInitOnce = true;
	}

	VideoInput_Protected_Init_Succeeded();
}

void VideoInput_Update_Resolution_Detection(void)
{
	// Using the latest received status (instead of polling the chip again) to determine the status.
	/// @todo Assuming that these devices only take in landscape.
	HDMIStatus = (VideoInput_Get_Status() ? VIDSTATUS_VIDEO_LANDSCAPE : VIDSTATUS_NOVIDEO);
}

void VideoInput_Task(void) {}
void VideoInput_Reset(uint8_t inputId)
{
	if (inputId != 1)
	{
		WriteLn("Wrong HDMI num");
		return;
	}

	WriteLn("reset HDMI1");
	Toshiba_TC358870_Trigger_Reset();
}

static const char LIBHDK2_NOT_SUPPORTED[] = "TC358870 via libhdk20 does not support this feature.";

void VideoInput_Suspend(void) { WriteLn(LIBHDK2_NOT_SUPPORTED); }
void VideoInput_Resume(void) { WriteLn(LIBHDK2_NOT_SUPPORTED); }
void VideoInput_Poll_Status(void) { VideoInput_Protected_Report_Status(Toshiba_TC358870_Have_Video_Sync()); }
// to console
void VideoInput_Report_Status(void)
{
	Write("Last time libhdk20:IsVideoExistingPolling() called HDMI_IsVideoExisting(), ");
	if (VideoInput_Get_Status())
	{
		WriteLn("video input was available.");
	}
	else
	{
		WriteLn("no video input was available.");
	}
	char myMessage[50];
	sprintf(myMessage, "TC358870_Init called %d times", Toshiba_TC358870_Get_Init_Count());
	WriteLn(myMessage);

	sprintf(myMessage, "VideoInput_Init called once: %d", (haveInitOnce ? 1 : 0));
	WriteLn(myMessage);
}
#endif

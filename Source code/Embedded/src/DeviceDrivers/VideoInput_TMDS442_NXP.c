/*
 * VideoInput_TMDS442_NXP.c
 * for video (HDMI) input using the TI TMDS442 HDMI switch and the NXP 19971 HDMI receivers
 * Created: 7/20/2016 10:24:03 AM
 *  Author: Sensics
 */

#include "GlobalOptions.h"

#if defined(SVR_HAVE_NXP) && defined(SVR_HAVE_TMDS442)
#include "VideoInput.h"
#include "VideoInput_Protected.h"
#include "main.h"  // for the task booleans
#include "NXP/AVRHDMI.h"
#include "DeviceDrivers/TI-TMDS442.h"
#include "Console.h"

void VideoInput_Init()
{
	TMDS442_Init();
	TMDS442_Task();
	NXP_Init_HDMI();
}
void VideoInput_Update_Resolution_Detection(void) { NXP_Update_Resolution_Detection(); }
void VideoInput_Task(void)
{
	if (HDMI_task)
	{
		/// @todo Do we need to poll the NXP every time through the mainloop, or can we depend on the interrupts?
		NXP_HDMI_Task();
	}
}
void VideoInput_Reset(uint8_t inputId) { NXP_HDMI_Reset(inputId); }
void VideoInput_Suspend(void) { NXP_Suspend(); }
void VideoInput_Resume(void) { NXP_Resume(); }
void VideoInput_Poll_Status(void)
{
	if (HDMISwitch_task)
	{
		// check status of HDMI switch
		TMDS442_Task();
	}
	if (HDMI_task)
	{
		/// @todo Do we need to poll the NXP every time through the mainloop, or can we depend on the interrupts?
		NXP_HDMI_Task();
	}
}
void VideoInput_Report_Status(void) { NXP_Report_HDMI_status(); }
#if 0
uint8_t VideoInput_Get_Detail_Status(void) { return Get_HDMI_Status(); }
#endif

#endif  // SVR_HAVE_NXP && SVR_HAVE_TMDS442

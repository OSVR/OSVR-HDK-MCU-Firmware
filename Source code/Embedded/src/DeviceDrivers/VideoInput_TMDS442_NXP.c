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
#include "SvrYield.h"

void VideoInput_Init()
{
	/// Init TI TMDS442 HDMI input switch.
	TMDS442_Init();
	TMDS442_ForcePoll();

	/// Pull NXP chips out of reset.
	NXP_HDMI_Reset(1);
	NXP_HDMI_Reset(2);

	/// Init NXP HDMI receivers
	NXP_Init_HDMI();
}

static inline void VideoInput_dSight_do_step(void)
{
	static bool gotTmdsChange = false;
	bool gotTmdsChangeLastTime = gotTmdsChange;
	gotTmdsChange = false;

	bool switchLostInput = false;
	if (HDMISwitch_task)
	{
		// check status of HDMI switch
		// If it reports a change, reset NXP, report "no signal!", and exit the handler for now.
		// This will let plug/switch state changes properly propagate thru.
		const uint8_t initialPlugSource = TMDS442_GetPlugSourceData();
		if (TMDS442_Task())
		{
			gotTmdsChange = true;
			svr_yield_ms(100);
			const uint8_t newPlugSource = TMDS442_GetPlugSourceData();
			if (initialPlugSource != 0 && newPlugSource != 0)
			{
				// OK, so plugs changed without initial gain or complete loss - we will simulate a complete loss.
				VideoInput_Protected_Report_No_Signal();
				NXP_HDMI_Reset(1);
				NXP_HDMI_Reset(2);
				/// Init NXP HDMI receivers
				NXP_Init_HDMI();
				return;
			}
			else if (newPlugSource == 0)
			{
				// No longer have any input.
				// Will give NXP a chance to deal with this, but if it doesn't, we'll report it at the end.
				switchLostInput = true;
			}
		}
	}
	if (HDMI_task)
	{
		NXP_HDMI_Task();
	}
#ifdef SVR_HAVE_SHARP_LCD
	if (VideoInput_Events.videoDetected)
	{
		WriteLn("VideoInput: Video detected event, running task a few more times.");
		for (uint8_t i = 0; i < 10; ++i)
		{
			svr_yield_ms(10);
			NXP_HDMI_Task();
		}
	}
#endif  // SVR_HAVE_SHARP_LCD
	if (switchLostInput && VideoInput_Get_Status())
	{
		// if we lost input, and videoinput still says we have video - report that we lost input here.
		WriteLn("VideoInput: TMDS detected full signal loss, but NXP did not. Reporting anyway.");
		VideoInput_Protected_Report_No_Signal();
	}
}
void VideoInput_Update_Resolution_Detection(void) { NXP_Update_Resolution_Detection(); }
void VideoInput_Task(void) { VideoInput_dSight_do_step(); }
void VideoInput_Reset(uint8_t inputId) { NXP_HDMI_Reset(inputId); }
void VideoInput_Suspend(void) { NXP_Suspend(); }
void VideoInput_Resume(void) { NXP_Resume(); }
void VideoInput_Poll_Status(void)
{
	// All work is done in _Task
}
void VideoInput_Report_Status(void) { NXP_Report_HDMI_status(); }
#if 0
uint8_t VideoInput_Get_Detail_Status(void) { return Get_HDMI_Status(); }
#endif

#endif  // SVR_HAVE_NXP && SVR_HAVE_TMDS442

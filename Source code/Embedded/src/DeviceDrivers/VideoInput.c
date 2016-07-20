/*
 * VideoInput.c
 *
 * Created: 7/20/2016 10:45:54 AM
 *  Author: Sensics
 */

#include "VideoInput.h"
#include "VideoInput_Protected.h"

#include "main.h"  // for HDMI_task

void VideoInput_Protected_Init_Succeeded() { HDMI_task = true; }
bool PortraitMode = false;  // true if incoming video is in portrait mode
uint8_t HDMIStatus = 0;     // shows whether we have video and also video mode

static bool s_everSignal = false;
static bool s_lastStatus = false;
VideoInputEvents_t VideoInput_Events = {false, false, false};

static void internal_report_detected_event(void);
static void internal_report_lost_event(void);
static inline void internal_report_detected_event()
{
	// report event
	VideoInput_Events.videoDetected = true;
	if (!s_everSignal)
	{
		// signal for the first time!
		s_everSignal = true;
		VideoInput_Events.firstVideoDetected = true;
	}
}
static inline void internal_report_lost_event()
{
	// report event
	VideoInput_Events.videoLost = true;
}
void VideoInput_Protected_Report_Status(bool signalStatus)
{
	if (s_lastStatus == signalStatus)
	{
		// no change, early exit
		return;
	}
	// OK, change, what kind is it?
	s_lastStatus = signalStatus;
	if (signalStatus)
	{
		internal_report_detected_event();
	}
	else
	{
		internal_report_lost_event();
	}
}

void VideoInput_Protected_Report_Signal()
{
	if (s_lastStatus == true)
	{
		// no change, get out early.
		return;
	}

	// OK, we have a change: got some video.

	// Update last status.
	s_lastStatus = true;
	// report event

	internal_report_detected_event();
}

void VideoInput_Protected_Report_No_Signal()
{
	if (s_lastStatus == false)
	{
		// no change, get out early.
		return;
	}
	// OK, we have a change: lost signal.
	// Update last status.
	s_lastStatus = false;
	// report event
	internal_report_lost_event();
}

bool VideoInput_Get_Status() { return s_lastStatus; }
/*
 * VideoInput.h
 *
 * Created: 7/20/2016 9:10:30 AM
 *  Author: Sensics
 */

#ifndef VIDEOINPUT_H_
#define VIDEOINPUT_H_

#include <stdint.h>
#include <stdbool.h>

typedef struct VideoInputEvents_
{
	bool videoLost;
	bool videoDetected;
	bool firstVideoDetected;
} VideoInputEvents_t;

/// Struct containing flags set by polling (below) or potentially by interrupts. It is your responsibility to clear the
/// flags when you handle events, and to check the values here frequently enough that events are not lost.
extern VideoInputEvents_t VideoInput_Events;

/// Must be executed after Display_System_Init()
void VideoInput_Init(void);

void VideoInput_Update_Resolution_Detection(void);

/// Task to run each mainloop, whether or not we are polling.
void VideoInput_Task(void);

void VideoInput_Reset(uint8_t inputId);

void VideoInput_Suspend(void);

void VideoInput_Resume(void);

/// Checks whether there is a video input, potentially updating VideoInput_Events (if not done via interrupts)
/// Only call if SVR_VIDEO_INPUT_POLL_INTERVAL is defined
void VideoInput_Poll_Status(void);

/// to console
void VideoInput_Report_Status(void);

/// Have we been notified of having a signal? (does not poll)
/// Note: Shared implementation - do not re-implement for each VideoInput.
bool VideoInput_Get_Status(void);

enum VideoInput_VideoDetailStatus
{
	VIDSTATUS_NOVIDEO = 0,
	VIDSTATUS_VIDEO_LANDSCAPE = 1,
	VIDSTATUS_VIDEO_PORTRAIT = 3
};

#if 0
/// this code is currently subsumed by a global HDMIStatus variable updated in Update_Resolution_Detection

/// Returns a value interpreted by VideoInput_VideoDetailStatus
uint8_t VideoInput_Get_Detail_Status(void);
#endif

extern bool PortraitMode;   // true if incoming video is in portrait mode
extern uint8_t HDMIStatus;  // shows whether we have video and also video mode

#endif /* VIDEOINPUT_H_ */

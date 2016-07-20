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

void VideoInput_Init(void);
void VideoInput_Update_Resolution_Detection(void);
void VideoInput_Task(void);
void VideoInput_Reset(uint8_t inputId);
void VideoInput_Suspend(void);
void VideoInput_Resume(void);

typedef struct VideoInputEvents_
{
	bool videoLost;
	bool videoDetected;
	bool firstVideoDetected;
} VideoInputEvents_t;

/// Struct containing flags set by polling (below) or potentially by interrupts. It is your responsibility to clear the
/// flags when you handle events.
extern VideoInputEvents_t VideoInput_Events;

/// Checks whether there is a video input, potentially updating VideoInput_Events (if not done via interrupts)
bool VideoInput_Poll_Status(void);

/// Have we been notified of having a signal? (does not poll)
bool VideoInput_Get_Status(void);

// to console
void VideoInput_Report_Status(void);

#if 0
/// this code is currently subsumed by a global HDMIStatus variable updated in Update_Resolution_Detection
enum VideoInput_VideoDetailStatus
{
	VIDSTATUS_NOVIDEO = 0,
	VIDSTATUS_VIDEO_LANDSCAPE = 1,
	VIDSTATUS_VIDEO_PORTRAIT = 3
};

/// Returns a valid interpreted by VideoInput_VideoDetailStatus
uint8_t VideoInput_Get_Detail_Status(void);
#endif
#endif /* VIDEOINPUT_H_ */

/*
 * VideoInput_Protected.h
 * Internal interfaces used by VideoInput implementations.
 * Created: 7/20/2016 11:41:15 AM
 *  Author: Sensics
 */

#ifndef VIDEOINPUT_PROTECTED_H_
#define VIDEOINPUT_PROTECTED_H_

#include "VideoInput.h"

/// Call at the end of VideoInput_Init - sets the HDMItask flag indicating that it's safe to run your task.
void VideoInput_Protected_Init_Succeeded(void);

/// This is equivalent to calling report_signal or report_no_signal in an if-else
void VideoInput_Protected_Report_Status(bool signalStatus);

/// Call from within a VideoInput implementation when you know you have a video signal.
/// Updates status and sets events accordingly.
void VideoInput_Protected_Report_Signal(void);
/// Call from within a VideoInput implementation when you know you do not have a video signal.
/// Updates status and sets events accordingly.
void VideoInput_Protected_Report_No_Signal(void);

#endif /* VIDEOINPUT_PROTECTED_H_ */
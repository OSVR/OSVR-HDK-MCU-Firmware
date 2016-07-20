/*
 * VideoInput_Protected.h
 * Internal interfaces used by VideoInput implementations.
 * Created: 7/20/2016 11:41:15 AM
 *  Author: Sensics
 */

#ifndef VIDEOINPUT_PROTECTED_H_
#define VIDEOINPUT_PROTECTED_H_

#include "VideoInput.h"

/// This is equivalent to calling report_signal or report_no_signal in an if-else
void VideoInput_Protected_Report_Status(bool signalStatus);
void VideoInput_Protected_Report_Signal(void);
void VideoInput_Protected_Report_No_Signal(void);

/// Have we been notified of having a signal?
bool VideoInput_Protected_Get_Status(void);

#endif /* VIDEOINPUT_PROTECTED_H_ */
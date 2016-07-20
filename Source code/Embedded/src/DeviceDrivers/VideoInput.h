/*
 * VideoInput.h
 *
 * Created: 7/20/2016 9:10:30 AM
 *  Author: Sensics
 */

#ifndef VIDEOINPUT_H_
#define VIDEOINPUT_H_

#include <stdint.h>

void VideoInput_Init(void);
void VideoInput_Update_Resolution_Detection(void);
void VideoInput_Task(void);
void VideoInput_Reset(uint8_t inputId);
void VideoInput_Suspend(void);
void VideoInput_Resume(void);

// to console
void VideoInput_Report_Status(void);

#endif /* VIDEOINPUT_H_ */

/*
 * VideoInput_NXP.c
 *
 * Created: 7/20/2016 9:10:58 AM
 *  Author: Sensics
 */

#include "VideoInput.h"
#include "GlobalOptions.h"

#ifdef SVR_HAVE_NXP
#include "NXP/AVRHDMI.h"

void VideoInput_Init() { NXP_Init_HDMI(); }
void VideoInput_Update_Resolution_Detection(void) { NXP_Update_Resolution_Detection(); }
void VideoInput_Task(void) { NXP_HDMI_Task(); }
void VideoInput_Reset(uint8_t inputId) { NXP_HDMI_Reset(inputId); }
void VideoInput_Suspend(void) { NXP_Suspend(); }
void VideoInput_Resume(void) { NXP_Resume(); }
#endif

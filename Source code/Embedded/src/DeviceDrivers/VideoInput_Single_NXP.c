/*
 * VideoInput_NXP.c
 *
 * Created: 7/20/2016 9:10:58 AM
 *  Author: Sensics
 */

#include "GlobalOptions.h"

#if defined(SVR_HAVE_NXP) && SVR_HAVE_NXP == 1

#include "VideoInput.h"
#include "main.h"  // for the task booleans
#include "NXP/AVRHDMI.h"

void VideoInput_Init() { NXP_Init_HDMI(); }
void VideoInput_Update_Resolution_Detection(void) { NXP_Update_Resolution_Detection(); }
void VideoInput_Task(void)
{
	if (HDMI_task)
	{
		NXP_HDMI_Task();
	}
}
void VideoInput_Reset(uint8_t inputId) { NXP_HDMI_Reset(inputId); }
void VideoInput_Suspend(void) { NXP_Suspend(); }
void VideoInput_Resume(void) { NXP_Resume(); }
#if 0
uint8_t VideoInput_Get_Detail_Status(void) { return Get_HDMI_Status(); }
#endif

#endif  // defined(SVR_HAVE_NXP) && SVR_HAVE_NXP == 1

#ifndef _HDMI_H_
#define _HDMI_H_

#include <stdint.h>
#include <stdbool.h>

/// performs initialization tasks for NXP
void NXP_Init_HDMI(void);


/// periodically called to address HDMI task
void NXP_HDMI_Task(void);

/// report HDMI status for debug
void NXP_Report_HDMI_status(void);

void NXP_Video_On(void);

void NXP_Video_Off(void);

void NXP_Suspend(void);

void NXP_Resume(void);

void NXP_Debug(bool bothSides);

void NXP_HDMI_Reset(uint8_t HDMINum);

void NXP_Program_MTP0(void);
void NXP_Program_MTP1(void);

void NXP_Update_Resolution_Detection(void);
void NXP_Test_Pattern(bool enabled);

#endif

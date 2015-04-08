#ifndef _HDMI_H_
#define _HDMI_H_

void Init_HDMI(void);
// performs initialization tasks for NXP


void HDMITask(void);
// periodically called to address HDMI task

void Report_HDMI_status(void);
// report HDMI status for debug

void PRINTIF(uint16_t errCode, uint16_t lineNumber);

void VideoOn(void);

void VideoOff(void);

void NXPSuspend(void);

void NXPResume(void);

void NXPDebug(bool bothSides);

void HDMI_Reset(uint8_t HDMINum);

void ProgramMTP0(void);
void ProgramMTP1(void);

#endif

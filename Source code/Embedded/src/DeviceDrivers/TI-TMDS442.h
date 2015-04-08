/*
 * TI_TMDS442.h
 *

 driver for TI TMDS422 4-TO-2 DVI/HDMI SWITCH
 * Created: 10/20/2013 9:08:04 AM
 *  Author: Sensics
 */


#ifndef TITMDS442_H_
#define TITMDS442_H_

#ifdef TMDS422
    void InitHDMISwitch(void);

    void EnableVideoA(void);
    void DisableVideoA(void);

    void EnableVideoB(void);
    void DisableVideoB(void);

    void SetInputStatus(uint8_t NewStatus);

    Bool ReadTMDS422Status(uint8_t regNum, uint8_t *NewStatus);

    void TMDS_422_Task(void);

    bool HDMI_config(uint8_t RegNum, uint8_t Value);

#endif
#endif /* TI-TMDS442_H_ */
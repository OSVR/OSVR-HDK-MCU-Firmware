/*
 * GlobalOptions.h
 *
 * Created: 9/1/2013 5:10:07 PM
 *  Author: Sensics
 */


#ifndef GLOBALOPTIONS_H_
#define GLOBALOPTIONS_H_

#define OSVRHDK // if defined, HMD only has one screen, no TI HDMI switch and BNO070 tracker

#define MajorVersion	1
#define MinorVersion	62

#define TMFL_TDA19971 // same reasons

//#define SkipNXP1 // ignore first NXP during HDMI task.
//#define BYPASS_FIRST_NXP

#ifdef OSVRHDK
    #define LS055T1SX01 // sharp 5.5"
    //#define H546DLT01  // AUO OLED
    #define BNO070
#else
    #define LS050T1SX01 // Sharp 5" LCD screen
#endif


#define HDMI_DEBUG

extern char Msg[];
#define TMFL_NO_RTOS
#define I2C_TIMEOUT			1 // msec

#define NXP_TWI_SPEED	50000 //!< TWI data transfer rate

#ifdef BNO070
    #define BNO_TWI_SPEED	150000 //!< TWI data transfer rate
#endif

#define USB_REPORT_SIZE		32

#define	MaxCommandLength	20

#ifndef OSVRHDK
    #define TMDS422 // true if TMDS HDMI switch is to be used
#endif

#endif /* GLOBALOPTIONS_H_ */
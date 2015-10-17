/*
 * GlobalOptions.h
 *
 * Created: 9/1/2013 5:10:07 PM
 *  Author: Sensics
 */


#ifndef GLOBALOPTIONS_H_
#define GLOBALOPTIONS_H_

#include "VariantOptions.h"

#define MajorVersion	1
#define MinorVersion	82

#define TMFL_TDA19971 // same reasons

//#define SkipNXP1 // ignore first NXP during HDMI task.
//#define BYPASS_FIRST_NXP


#ifndef SENSICS_DISPLAY_CONFIGURED
    #define LS050T1SX01 // Sharp 5" LCD screen
    #define SENSICS_DISPLAY_CONFIGURED
#endif


#define HDMI_DEBUG

extern char Msg[];
#define TMFL_NO_RTOS
#define I2C_TIMEOUT			1 // msec

#define NXP_TWI_SPEED	50000 //!< TWI data transfer rate

#ifdef BNO070
	#define MeasurePerformance // if defined, performance is being recorded for BNO work
	#define BNO_TWI_SPEED	400000 //!< TWI data transfer rate
#endif

#define USB_REPORT_SIZE		16

#define	MaxCommandLength	20

#ifndef OSVRHDK
    #define TMDS422 // true if TMDS HDMI switch is to be used
#endif

#endif /* GLOBALOPTIONS_H_ */
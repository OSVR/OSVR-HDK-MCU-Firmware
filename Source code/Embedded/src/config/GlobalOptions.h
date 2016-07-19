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
#define MinorVersion	94

//#define SkipNXP1 // ignore first NXP during HDMI task.
//#define BYPASS_FIRST_NXP

#ifndef SENSICS_DISPLAY_CONFIGURED
    #define LS050T1SX01 // Sharp 5" LCD screen
    #define SENSICS_DISPLAY_CONFIGURED
#endif

#if defined(LS050T1SX01) || defined(LS050T1SX01)
	#define SVR_HAVE_SHARP_LCD
#elif defined(H546DLT01)
	#define SVR_HAVE_AUO_OLED
#endif

//#define HDMI_DEBUG

#ifdef SVR_HAVE_NXP

#undef SVR_NO_NXP

// Define for NXP driver to choose the product in the NXP HDMI receiver family.
#define TMFL_TDA19971
#define TMFL_NO_RTOS

#define NXP_TWI_SPEED	50000 //!< TWI data transfer rate

#define SVR_HAVE_NXP1
#if SVR_HAVE_NXP>=2
#define SVR_HAVE_NXP2
#endif
#else // ^ SVR_HAVE_NXP / v !SVR_HAVE_NXP
#ifndef SVR_NO_NXP
#define SVR_NO_NXP
#endif
#endif // SVR_HAVE_NXP

#ifdef SVR_HAVE_SOLOMON
#undef SVR_NO_SOLOMON
#define SVR_HAVE_SOLOMON1
#if SVR_HAVE_SOLOMON>=2
#define SVR_HAVE_SOLOMON2
#endif

#else // ^ SVR_HAVE_SOLOMON / v !SVR_HAVE_SOLOMON
#ifndef SVR_NO_SOLOMON
#define SVR_NO_SOLOMON
#endif
#endif // SVR_HAVE_SOLOMON

#if !defined(SVR_HAVE_SIDEBYSIDE) && !defined(SVR_NO_SIDEBYSIDE)
#define SVR_NO_SIDEBYSIDE
#endif

extern char Msg[];
#define I2C_TIMEOUT			1 // msec

#ifdef BNO070
	//#define MeasurePerformance // if defined, performance is being recorded for BNO work, with some performance impact
	#define BNO_TWI_SPEED	400000 //!< TWI data transfer rate
#endif

//#define DISABLE_NXP // for debug - disables access to HDMI chips

#define USB_REPORT_SIZE		16

#define	MaxCommandLength	20

#if !defined(OSVRHDK) || defined(SVR_HAVE_TMDS422)
    #define TMDS422 // true if TMDS HDMI switch is to be used
#endif
#if defined(TMDS422) && !defined(SVR_HAVE_TMDS422)
	#define SVR_HAVE_TMDS422
#endif

#endif /* GLOBALOPTIONS_H_ */
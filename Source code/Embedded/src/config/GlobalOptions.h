/*
 * GlobalOptions.h
 *
 * Created: 9/1/2013 5:10:07 PM
 *  Author: Sensics
 */

#ifndef GLOBALOPTIONS_H_
#define GLOBALOPTIONS_H_

#include "VariantOptions.h"

#define MajorVersion 1
#define MinorVersion 94

/// @ingroup Build-time options
/// @{
#ifdef DOXYGEN
/// Creates a firmware that will perform a firmware upgrade on the BNO070 chip. It will not have display support - it is
/// a special-purpose firmware only.
#define PERFORM_BNO_DFU

/// Prints additional debug messages to the USB serial console related to HDMI state.
#define HDMI_DEBUG

/// if defined, performance is being recorded for BNO work, with some performance impact
#define MeasurePerformance

/// @todo unknown debug option?
#define BYPASS_FIRST_NXP

/// ignore first NXP during HDMI task.
/// @todo unknown debug option for dSight?
#define SkipNXP1

#endif  // DOXYGEN
/// @}

#ifndef SVR_DISABLE_VIDEO_INPUT
/// Automatically-defined opposite of SVR_DISABLE_VIDEO_INPUT (and the deprecated DISABLE_NXP)
#define SVR_ENABLE_VIDEO_INPUT
#else
#undef SVR_ENABLE_VIDEO_INPUT
#endif

#ifndef SENSICS_DISPLAY_CONFIGURED
#define LS050T1SX01  // Sharp 5" LCD screen
#define SENSICS_DISPLAY_CONFIGURED
#endif

#if defined(OSVRHDK) && !defined(HDK_20)
#define SVR_IS_HDK_1_x
#endif

#if SVR_NUM_LOGICAL_DISPLAYS == 1
#define SVR_HAVE_DISPLAY1
#elif SVR_NUM_LOGICAL_DISPLAYS == 2
#define SVR_HAVE_DISPLAY1
#define SVR_HAVE_DISPLAY2
#endif

#if defined(LS050T1SX01) || defined(LS050T1SX01)
#define SVR_HAVE_SHARP_LCD
#elif defined(H546DLT01)
#define SVR_HAVE_AUO_OLED
#endif

#ifdef SVR_HAVE_NXP

#undef SVR_NO_NXP

// Define for NXP driver to choose the product in the NXP HDMI receiver family.
#define TMFL_TDA19971
#define TMFL_NO_RTOS

#define NXP_TWI_SPEED 50000  //!< TWI data transfer rate

#define SVR_HAVE_NXP1
#if SVR_HAVE_NXP >= 2
#define SVR_HAVE_NXP2
#endif
#else  // ^ SVR_HAVE_NXP / v !SVR_HAVE_NXP
#ifndef SVR_NO_NXP
#define SVR_NO_NXP
#endif
#endif  // SVR_HAVE_NXP

// DISABLE_NXP is a flag passed to disable HDMI receiver functionality in some specialized firmware builds.
// It is being replaced by SVR_DISABLE_VIDEO_INPUT
#if defined(SVR_HAVE_NXP) && !defined(SVR_DISABLE_VIDEO_INPUT)
#define SVR_USING_NXP
#endif

#if defined(SVR_HAVE_TOSHIBA_TC358870) && !defined(SVR_DISABLE_VIDEO_INPUT)
#define SVR_USING_TOSHIBA_TC358870
#endif

#ifdef SVR_HAVE_SOLOMON
#undef SVR_NO_SOLOMON
#define SVR_HAVE_SOLOMON1
#if SVR_HAVE_SOLOMON >= 2
#define SVR_HAVE_SOLOMON2
#endif

#else  // ^ SVR_HAVE_SOLOMON / v !SVR_HAVE_SOLOMON
#ifndef SVR_NO_SOLOMON
#define SVR_NO_SOLOMON
#endif
#endif  // SVR_HAVE_SOLOMON

#if !defined(SVR_HAVE_SIDEBYSIDE) && !defined(SVR_NO_SIDEBYSIDE)
#define SVR_NO_SIDEBYSIDE
#endif

extern char Msg[];
#define I2C_TIMEOUT 1  // msec

#ifdef BNO070
//#define MeasurePerformance // if defined, performance is being recorded for BNO work, with some performance impact
#define BNO_TWI_SPEED 400000  //!< TWI data transfer rate
#endif

#define USB_REPORT_SIZE 16

#define MaxCommandLength 20

#if !defined(OSVRHDK) || defined(SVR_HAVE_TMDS422)
#define TMDS422  // true if TMDS HDMI switch is to be used
#endif
#if defined(TMDS422) && !defined(SVR_HAVE_TMDS422)
#define SVR_HAVE_TMDS422
#endif

#endif /* GLOBALOPTIONS_H_ */
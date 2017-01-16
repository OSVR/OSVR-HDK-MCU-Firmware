/*
 * VariantOptions.h
 * For the dual-AUO-OLED, single HDMI HDK 2.0 and related HMDs, customized by Sensics.
 * Created: 7/20/2016 5:51:12 PM
 *  Author: Sensics
 */

// Derived from the HDK2

#ifndef VARIANTOPTIONS_H_HDK20SVR
#define VARIANTOPTIONS_H_HDK20SVR

// Product: OSVR HDK 2.0 and derived custom devices
#define OSVRHDK
#define SVR_IS_HDK_20

#define SVR_HDK_DEFAULT_MAJOR_VER 2
#define SVR_HDK_DEFAULT_MINOR_VER 0
#define SVR_HDK_VARIANT_STRING "20"

// BNO070 IMU (reporting over USB through this MCU)
#define BNO070

// Dual 3.8" AUO OLED
#define AUO_H381DLN01_X2
#define SENSICS_DISPLAY_CONFIGURED

// Toshiba TC358870 HDMI to dual MIPI converter chip.
#define TC358870
#define SVR_HAVE_TOSHIBA_TC358870

// changes OLED configuration
//#define LOW_PERSISTENCE

// Single display path to control
#define SVR_NUM_LOGICAL_DISPLAYS 1

// HDK 1.3 boards and newer contain signal lines sufficient to permit BNO DFU.
#ifdef PERFORM_BNO_DFU
// Disable video input (NXP) code
/// @todo why?
#define SVR_DISABLE_VIDEO_INPUT
#endif

#define SVR_HAVE_EDID_INFO_STRING
extern const char svrEdidInfoString[];

#define SVR_VARIANT_STRING "HDK_20_SVR"

#define SVR_IS_HDK_20_SVR

#endif /* VARIANTOPTIONS_H_HDK20SVR */

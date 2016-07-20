/*
 * VariantOptions.h
 * for the AUO low-persistence-OLED-based OSVR HDK and derived HMDs
 *
 * Created: 5/26/2015 5:10:07 PM
 *  Author: Sensics
 */

#ifndef VARIANTOPTIONS_H_
#define VARIANTOPTIONS_H_

// Commonalities in the video signal path
#include "SingleDisplayNXPSolomonFPGA.h"

// Product: OSVR HDK (1.2, 1.3, 1.4) and derived custom devices
#define OSVRHDK
#define SVR_HDK_DEFAULT_MAJOR_VER 1
#define SVR_HDK_DEFAULT_MINOR_VER 2
#define SVR_HDK_VARIANT_STRING "1X-OLED"

// BNO070 IMU (reporting over USB through this MCU)
#define BNO070

// AUO OLED
#define H546DLT01
#define SENSICS_DISPLAY_CONFIGURED

// changes OLED configuration
#define LOW_PERSISTENCE

// HDK 1.3 boards and newer contain signal lines sufficient to permit BNO DFU.
#ifdef PERFORM_BNO_DFU
#define DISABLE_NXP
#endif

#endif /* VARIANTOPTIONS_H_ */
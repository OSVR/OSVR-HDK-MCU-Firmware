/*
 * VariantOptions.h
 * for the Sharp LCD-based OSVR HDK and derived HMDs
 *
 * Created: 5/26/2015 5:10:07 PM
 *  Author: Sensics
 */

#ifndef VARIANTOPTIONS_H_
#define VARIANTOPTIONS_H_

// Commonalities in the video signal path
#include "SingleDisplayNXPSolomonFPGA.h"

// Product: OSVR HDK (1.0, 1.1) and derived custom devices
#define OSVRHDK
#define SVR_HDK_DEFAULT_MAJOR_VER 1
#define SVR_HDK_DEFAULT_MINOR_VER 1

// BNO070 IMU (reporting over USB through this MCU)
#define BNO070

// sharp 5.5" LCD
#define LS055T1SX01
#define SENSICS_DISPLAY_CONFIGURED

#endif /* VARIANTOPTIONS_H_ */
/*
 * VariantOptions.h
 *
 * Created: 5/26/2015 5:10:07 PM
 *  Author: Sensics
 */

#ifndef VARIANTOPTIONS_H_
#define VARIANTOPTIONS_H_

#include "SingleDisplayNXPSolomonFPGA.h"

#define OSVRHDK
#define BNO070

#define SENSICS_DISPLAY_CONFIGURED
#define H546DLT01        // AUO OLED
#define LOW_PERSISTENCE  // changes OLED configuration

#ifdef PERFORM_BNO_DFU
#define DISABLE_NXP
#endif

#endif /* VARIANTOPTIONS_H_ */
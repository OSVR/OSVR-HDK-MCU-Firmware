/*
 * VariantOptions.h
 * For the dual-AUO-OLED, single HDMI HDK 2.0 and related HMDs, customized by Sensics.
 * Created: 7/20/2016 5:51:12 PM
 *  Author: Sensics
 */

// Derived from the HDK2_SVR
#include "../HDK_20_SVR/VariantOptions.h"

#ifndef VARIANTOPTIONS_H_HDK20SVR60
#define VARIANTOPTIONS_H_HDK20SVR60

#undef SVR_VARIANT_STRING
#define SVR_VARIANT_STRING "HDK_20_SVR_60"

// So the driver checks the incoming rate for compatibility
#define SVR_MULTIRATE_HDK2

#endif /* VARIANTOPTIONS_H_HDK20SVR60 */

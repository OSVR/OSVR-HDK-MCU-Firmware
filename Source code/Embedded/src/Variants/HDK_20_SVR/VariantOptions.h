/*
 * VariantOptions.h
 * For the dual-AUO-OLED, single HDMI HDK 2.0 and related HMDs, customized by Sensics.
 * Created: 7/20/2016 5:51:12 PM
 *  Author: Sensics
 */

// Derived from the HDK2
#include "../HDK_20/VariantOptions.h"

#ifndef VARIANTOPTIONS_H_HDK20SVR
#define VARIANTOPTIONS_H_HDK20SVR

#undef SVR_VARIANT_STRING
#define SVR_VARIANT_STRING "HDK_20_SVR"

#define SVR_IS_HDK_20_SVR

// Using the same hardware compatibility signature, but different variant signature than the default.
#define SVR_VARIANT_SIGNATURE 0x00040001

#endif /* VARIANTOPTIONS_H_HDK20SVR */

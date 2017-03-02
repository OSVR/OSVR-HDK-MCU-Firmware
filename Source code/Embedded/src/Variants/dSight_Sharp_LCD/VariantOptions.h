/*
 * VariantOptions.h
 * for the dual-display, dual-input dSight professional HMD
 *
 * Created: 5/26/2015 5:10:07 PM
 *  Author: Sensics
 */

#ifndef VARIANTOPTIONS_H_
#define VARIANTOPTIONS_H_

// Product: Sensics dSight professional HMD
#define DSIGHT
#define SVR_IS_DSIGHT

// sharp 5" LCD
#define LS050T1SX01
#define SENSICS_DISPLAY_CONFIGURED

// TI TMDS422 HDMI switch preceding the HDMI receivers in the signal path and providing connect/disconnect notification.
#define SVR_HAVE_TMDS422

// Dual NXP TDA-19971 HDMI receiver
#define SVR_HAVE_NXP 2

// Dual Solomon SSD2828 MIPI bridge chip
#define SVR_HAVE_SOLOMON 2

// Video-processing FPGA in the signal path between each HDMI receiver and MIPI bridge
#define SVR_HAVE_FPGA 2

// Side-by-side video transform functionality provided by FPGA
#define SVR_HAVE_SIDEBYSIDE

// Two displays independently controlled
#define SVR_NUM_LOGICAL_DISPLAYS 2

#define SVR_HAVE_PWM_OUTPUTS

#define SVR_VARIANT_STRING "dSight_Sharp_LCD"

#endif /* VARIANTOPTIONS_H_ */

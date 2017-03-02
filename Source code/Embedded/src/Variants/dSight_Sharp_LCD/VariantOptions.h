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
#define SVR_HAVE_TMDS442

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

#define Solomon_SPI_MOSI IOPORT_CREATE_PIN(PORTC, 5)
#define Solomon_SPI_MISO IOPORT_CREATE_PIN(PORTC, 6)
#define Solomon_SPI_SCK IOPORT_CREATE_PIN(PORTC, 7)

#define Solomon1_CSN IOPORT_CREATE_PIN(PORTC, 4)
#define Solomon1_AddrData IOPORT_CREATE_PIN(PORTB, 1)
#define Solomon1_VOUT_Shutdown IOPORT_CREATE_PIN(PORTA, 1)  // when low, Solomon is activated
#define Solomon1_Reset IOPORT_CREATE_PIN(PORTA, 2)

#define Solomon1_SPI SPIC

/// Solomon2 shares same SPI port and lines, using the mux below to select which to use (not [just] nSS lines)
#define Solomon2_VOUT_Shutdown IOPORT_CREATE_PIN(PORTA, 3)
#define Solomon2_Reset IOPORT_CREATE_PIN(PORTA, 4)

#define Solomon2_CSN Solomon1_CSN
#define Solomon2_AddrData Solomon1_AddrData
#define Solomon2_SPI Solomon1_SPI

/* When Mux_OE is low, Mux_select low is Solomon1, Mux_select high is Solomon2 */
#define SPI_Mux_OE IOPORT_CREATE_PIN(PORTC, 2)
#define SPI_Mux_Select IOPORT_CREATE_PIN(PORTC, 3)

#define Solomon_DCSPI                                                                                    \
	{                                                                                                    \
		.addrData = Solomon1_AddrData, .addrLevel = false, .muxPin = SPI_Mux_Select, .muxOe = SPI_Mux_OE \
	}

#define Solomon1_Struct                                                                                             \
	{                                                                                                               \
		.spi = &Solomon1_SPI, .dcSpi = Solomon_DCSPI,                                                               \
		.dcSpiDevice = {.spiDevice = {Solomon1_CSN}, .muxPinLevel = false}, .voutShutdown = Solomon1_VOUT_Shutdown, \
		.reset = Solomon1_Reset                                                                                     \
	}
#define Solomon2_Struct                                                                                            \
	{                                                                                                              \
		.spi = &Solomon2_SPI, .dcSpi = Solomon_DCSPI,                                                              \
		.dcSpiDevice = {.spiDevice = {Solomon2_CSN}, .muxPinLevel = true}, .voutShutdown = Solomon2_VOUT_Shutdown, \
		.reset = Solomon2_Reset                                                                                    \
	}
#endif /* VARIANTOPTIONS_H_ */

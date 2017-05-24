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

// Interval in number of mainloop cycles that should elapse between polling for video status.
// Quite a few, because polling this is fairly expensive.
#define SVR_VIDEO_INPUT_POLL_INTERVAL 10000

#define SVR_VARIANT_STRING "dSight_Sharp_LCD"

#define SVR_PRODUCT_NAME_STRING "Sensics dSight 1080"

#define Solomon_SPI_MOSI IOPORT_CREATE_PIN(PORTC, 5)
#define Solomon_SPI_MISO IOPORT_CREATE_PIN(PORTC, 6)
#define Solomon_SPI_SCK IOPORT_CREATE_PIN(PORTC, 7)

#define Solomon1_CSN IOPORT_CREATE_PIN(PORTC, 4)
#define Solomon1_AddrData IOPORT_CREATE_PIN(PORTB, 1)
#define Solomon1_VOUT_Shutdown IOPORT_CREATE_PIN(PORTA, 1)  // when low, Solomon is activated
#define Solomon1_Reset IOPORT_CREATE_PIN(PORTA, 2)

#define Solomon1_SPI SPIC

/// Solomon2 shares same SPI port and many lines, using the mux below to select which to use (not [just] nSS lines)
#define Solomon2_VOUT_Shutdown IOPORT_CREATE_PIN(PORTA, 3)
#define Solomon2_Reset IOPORT_CREATE_PIN(PORTA, 4)
#define Solomon2_AddrData IOPORT_CREATE_PIN(PORTB, 3)

#define Solomon2_CSN Solomon1_CSN
#define Solomon2_SPI Solomon1_SPI

/* When Mux_OE is low, Mux_select low is Solomon1, Mux_select high is Solomon2 */
#define SPI_Mux_OE IOPORT_CREATE_PIN(PORTC, 2)
#define SPI_Mux_Select IOPORT_CREATE_PIN(PORTC, 3)

#define Solomon1_Struct                                                                                              \
	{                                                                                                                \
		.spi = &Solomon1_SPI,                                                                                        \
		.dcSpi = {.addrData = Solomon1_AddrData, .addrLevel = false, .muxPin = SPI_Mux_Select, .muxOe = SPI_Mux_OE}, \
		.dcSpiDevice = {.spiDevice = {Solomon1_CSN}, .muxPinLevel = false}, .voutShutdown = Solomon1_VOUT_Shutdown,  \
		.reset = Solomon1_Reset                                                                                      \
	}
#define Solomon2_Struct                                                                                              \
	{                                                                                                                \
		.spi = &Solomon2_SPI,                                                                                        \
		.dcSpi = {.addrData = Solomon2_AddrData, .addrLevel = false, .muxPin = SPI_Mux_Select, .muxOe = SPI_Mux_OE}, \
		.dcSpiDevice = {.spiDevice = {Solomon2_CSN}, .muxPinLevel = true}, .voutShutdown = Solomon2_VOUT_Shutdown,   \
		.reset = Solomon2_Reset                                                                                      \
	}

// NXP and Panel control lines

#define NXP1_Reset_Pin IOPORT_CREATE_PIN(PORTD, 5)
#define Int_HDMI_A IOPORT_CREATE_PIN(PORTD, 2)

#define NXP2_Reset_Pin IOPORT_CREATE_PIN(PORTD, 4)
#define Int_HDMI_B IOPORT_CREATE_PIN(PORTD, 3)

/// Active low, only controllable when SVR_PANEL1_LEVELSHIFT_OE_PIN is high
/// connects to CH1_LCD1_X_RESET through the FPGA
/// f.k.a. USARTF0_XCK0
#define SVR_PANEL1_RESET_PIN IOPORT_CREATE_PIN(PORTF, 1)
/// Active low, only controllable when SVR_PANEL2_LEVELSHIFT_OE_PIN is high
/// connects to CH2_LCD1_X_RESET through the FPGA
/// f.k.a. GPIO_E_X1_CH-2
#define SVR_PANEL2_RESET_PIN IOPORT_CREATE_PIN(PORTE, 2)

/// That is, these are reset-low.
#define SVR_PANEL_RESET_VALUE false

#define SVR_PANEL_RESET_PINS                       \
	{                                              \
		SVR_PANEL1_RESET_PIN, SVR_PANEL2_RESET_PIN \
	}

/// Activates level shifter for panel reset lines, etc - active high, tristate low.
/// Connects to CH1_TXS0104_OE via the FPGA
/// f.k.a. USARTF0_RXD0
#define SVR_PANEL1_LEVELSHIFT_OE_PIN IOPORT_CREATE_PIN(PORTF, 2)
/// Activates level shifter for panel reset lines, etc - active high, tristate low.
/// Connects to CH2_TXS0104_OE via the FPGA
/// f.k.a. GPIO_E_X2_CH-2
#define SVR_PANEL2_LEVELSHIFT_OE_PIN IOPORT_CREATE_PIN(PORTE, 3)

/// Pin can pull down GLOBAL_RST net, which actually only resets FPGAs and their memory.
#define FPGA_Reset_Pin IOPORT_CREATE_PIN(PORTB, 2)

/// Goes high when the termination power supply for the FPGA DDR memory is ready and operating.
#define VTT_PGOOD_X_PIN IOPORT_CREATE_PIN(PORTB, 0)

/// Goes high when the 2.5V power supply is OK (which is the last of the main rails to come up)
#define PS_PGD_2V5_PIN IOPORT_CREATE_PIN(PORTD, 0)

/// EDID is in a separate eeprom, not supplied by the firmware.
#define SVR_EXTERNAL_EDID

#endif /* VARIANTOPTIONS_H_ */

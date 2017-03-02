/*
 * SingleDisplayNXPSolomonFPGA.h
 *
 * Created: 7/19/2016 10:12:25 AM
 *  Author: Sensics
 */

#ifndef SINGLEDISPLAYNXPSOLOMONFPGA_H_
#define SINGLEDISPLAYNXPSOLOMONFPGA_H_

// Shared configuration options for the family of devices
// with a single display, using a Solomon MIPI bridge, NXP receiver,
// and FPGA.

// Interval in number of mainloop cycles that should elapse between polling for video status.
#define SVR_VIDEO_INPUT_POLL_INTERVAL 1000

// Single NXP TDA-19971 HDMI receiver
#define SVR_HAVE_NXP 1

// Single Solomon SSD2828 MIPI bridge chip
#define SVR_HAVE_SOLOMON 1

// Video-processing FPGA in the signal path between HDMI receiver and MIPI bridge
#define SVR_HAVE_FPGA 1

// Input pin from FPGA indicating that it has a lock on a video signal
#define SVR_HAVE_FPGA_VIDEO_LOCK_PIN

// Side-by-side video transform functionality provided by FPGA
#define SVR_HAVE_SIDEBYSIDE

/// Side-by-side video transform config state is stored in eeprom.
#define SVR_HAVE_SIDEBYSIDE_CONFIG

// Single display to control
#define SVR_NUM_LOGICAL_DISPLAYS 1

#define Solomon_SPI_MOSI IOPORT_CREATE_PIN(PORTC, 5)
#define Solomon_SPI_MISO IOPORT_CREATE_PIN(PORTC, 6)
#define Solomon_SPI_SCK IOPORT_CREATE_PIN(PORTC, 7)

#define Solomon1_CSN IOPORT_CREATE_PIN(PORTC, 4)
#define Solomon1_AddrData IOPORT_CREATE_PIN(PORTB, 1)
#define Solomon1_VOUT_Shutdown IOPORT_CREATE_PIN(PORTA, 1)  // when low, Solomon is activated
#define Solomon1_Reset IOPORT_CREATE_PIN(PORTA, 2)

#define Solomon1_SPI SPIC

// turn off mux support in dcspi
#define DCSPI_MUX_SUPPORT 0
#define Solomon1_Struct                                                                                               \
	{                                                                                                                 \
		.spi = &Solomon1_SPI, .dcSpi = {.addrData = Solomon1_AddrData, .addrLevel = false},                           \
		.dcSpiDevice = {.spiDevice = {Solomon1_CSN}}, .voutShutdown = Solomon1_VOUT_Shutdown, .reset = Solomon1_Reset \
	}

#endif /* SINGLEDISPLAYNXPSOLOMONFPGA_H_ */
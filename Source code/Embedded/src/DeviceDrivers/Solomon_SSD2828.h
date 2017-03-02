/*
 * Solomon_SSD2828.h
 *
 *  Author: Ryan, Sensics, Inc.
 *  Copyright 2017 Sensics, Inc.
 */

#ifndef SOLOMON_SSD2828_H_
#define SOLOMON_SSD2828_H_

#include "DisplayControllerSPI.h"
#include <ioport.h>

/// Detail of what byte needs to be written to the LRR register for a read. Detected by solomon_init.
typedef enum Solomon_LRR_Behavior_Enum { SOLOMON_LRR_USE_CONSTANT, SOLOMON_LRR_USE_ADDRESS } Solomon_LRR_Behavior_t;

typedef struct Solomon
{
	SPI_t* spi;
	DisplayControlSPI_t dcSpi;
	DisplayControlSPI_Device_t dcSpiDevice;
	// when low, Solomon is activated. High, it shuts down the display.
	ioport_pin_t voutShutdown;
	/// active-low reset pin.
	ioport_pin_t reset;
	/// Apparently varies by unit? Automatically set by solomon_init, no need to initialize manually.
	Solomon_LRR_Behavior_t lrrBehavior;
} Solomon_t;

/// Sets up SPI, briefly resets the chip, and detects LRR behavior.
/// @return true if Solomon successfully initialized.
bool solomon_init(Solomon_t* sol);

/// Detects which of the two Solomon_LRR_Behavior_t values should be used and updates sol->lrrBehavior accordingly.
/// Can be used if the exact Solomon_t instance used for init is inaccessible from your bit of code.
/// @return false if neither value results in expected behavior
bool solomon_detect_lrr_behavior(Solomon_t* sol);

/// @pre solomon_select has been called for this unit
void solomon_write_reg(Solomon_t const* sol, uint8_t addr, const uint8_t* data, size_t len);
/// @pre solomon_select has been called for this unit
void solomon_write_reg_byte(Solomon_t const* sol, uint8_t addr, uint8_t data);
/// @pre solomon_select has been called for this unit
void solomon_write_reg_2byte(Solomon_t const* sol, uint8_t addr, uint8_t data1, uint8_t data2);
/// @pre solomon_select has been called for this unit
void solomon_write_reg_word(Solomon_t const* sol, uint8_t addr, uint16_t data);
/// @pre solomon_select has been called for this unit
void solomon_read_reg(Solomon_t const* sol, uint8_t addr, uint8_t* data, size_t len);
/// @pre solomon_select has been called for this unit
uint16_t solomon_read_reg_2byte(Solomon_t const* sol, uint8_t addr);

/// @post reset is normal (high) but voutShutdown still high.
void solomon_reset(Solomon_t const* sol);

__always_inline static void solomon_start_reset(Solomon_t const* sol) { ioport_set_pin_level(sol->reset, false); }
__always_inline static void solomon_end_reset(Solomon_t const* sol) { ioport_set_pin_level(sol->reset, true); }
__always_inline static void solomon_start_video_shutdown(Solomon_t const* sol)
{
	ioport_set_pin_level(sol->voutShutdown, true);
}
__always_inline static void solomon_end_video_shutdown(Solomon_t const* sol)
{
	ioport_set_pin_level(sol->voutShutdown, false);
}

/// @brief Call before any of the read or write operations.
__always_inline static void solomon_select(Solomon_t const* sol)
{
	dcspi_start_data(&(sol->dcSpi));
	dcspi_select_device(sol->spi, &(sol->dcSpi), &(sol->dcSpiDevice));
}

/// @brief Call when done with read/write operations on this device, prior to calling solomon_select on another device.
/// @pre solomon_select has been called on this device.
__always_inline static void solomon_deselect(Solomon_t const* sol)
{
	dcspi_deselect_device(sol->spi, &(sol->dcSpi), &(sol->dcSpiDevice));
}

#define SOLOMON_REG_CFGR UINT8_C(0xB7)
static const uint16_t SOLOMON_CFGR_TXD_bm = (UINT16_C(0x01) << 11);
static const uint16_t SOLOMON_CFGR_LPE_bm = (UINT16_C(0x01) << 10);
static const uint16_t SOLOMON_CFGR_REN_bm = (UINT16_C(0x01) << 7);
static const uint16_t SOLOMON_CFGR_DCS_bm = (UINT16_C(0x01) << 6);

/// Clock select - only touch when PEN is off.
static const uint16_t SOLOMON_CFGR_CSS_bm = (UINT16_C(0x01) << 5);
/// video enable
static const uint16_t SOLOMON_CFGR_VEN_bm = (UINT16_C(0x01) << 3);
/// Sleep/ULP mode
static const uint16_t SOLOMON_CFGR_SLP_bm = (UINT16_C(0x01) << 2);
/// clock lane enable high speed in all cases
static const uint16_t SOLOMON_CFGR_CKE_bm = (UINT16_C(0x01) << 1);
/// Send data using HS interface
static const uint16_t SOLOMON_CFGR_HS_bm = (UINT16_C(0x01));

/// VC Control Register
#define SOLOMON_REG_VCR UINT8_C(0xB8)
/// packet size control register 1
#define SOLOMON_REG_PSCR1 UINT8_C(0xBC)
/// Packet drop register
#define SOLOMON_REG_PDR UINT8_C(0xBF)
/// PLL control reg
#define SOLOMON_REG_PCR UINT8_C(0xBF)
/// PLL enable
static const uint16_t SOLOMON_PCR_PEN_bm = (UINT16_C(0x01));

#define SOLOMON_REG_DIR UINT8_C(0xb0)  // device ID register - should return value of 0x2828

#endif /* SOLOMON_SSD2828_H_ */
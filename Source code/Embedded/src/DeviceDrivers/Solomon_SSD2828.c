/*
 * Solomon_SSD2828.c
 *
 *  Author: Ryan, Sensics, Inc.
 *  Copyright 2017 Sensics, Inc.
 */

#include "Solomon_SSD2828.h"
#include "DisplayControllerSPI.h"

#include <spi_master.h>
#include <delay.h>

#include <limits.h>  // for char_bit

#ifndef SOLOMON_CLOCK_SPEED
#define SOLOMON_CLOCK_SPEED 1000000
#endif

// useful registers
#define SOLOMON_REG_LRR UINT8_C(0xd4)              //  SPI read register - default value is 0xFA
#define SOLOMON_REG_LRR_RESET_VALUE UINT8_C(0xFA)  // default value of LRR

#define SOLOMON_EXPECTED_DEVID 0x2828

bool solomon_init(Solomon_t* sol)
{
	/// start in reset
	solomon_start_reset(sol);
	ioport_set_pin_dir(sol->reset, IOPORT_DIR_OUTPUT);

	/// start with video shut off
	solomon_start_video_shutdown(sol);
	ioport_set_pin_dir(sol->voutShutdown, IOPORT_DIR_OUTPUT);

	/// Set chip select off (high)
	const ioport_pin_t cs = sol->dcSpiDevice.spiDevice.id;
#if 0
	ioport_configure_pin(cs, IOPORT_PULL_UP | IOPORT_DIR_INPUT);
#else
	ioport_set_pin_level(cs, true);
	ioport_set_pin_dir(cs, IOPORT_DIR_OUTPUT);
	ioport_enable_pin(cs);
#endif

	ioport_enable_pin(sol->reset);
	ioport_enable_pin(sol->voutShutdown);
	dcspi_init(sol->spi, &(sol->dcSpi));
	dcspi_setup_device(sol->spi, &(sol->dcSpiDevice), SPI_MODE_0, SOLOMON_CLOCK_SPEED);
	spi_enable(sol->spi);

	solomon_end_video_shutdown(sol);
	solomon_end_reset(sol);

	return solomon_detect_lrr_behavior(sol);
}

bool solomon_detect_lrr_behavior(Solomon_t* sol)
{
	solomon_select(sol);
	sol->lrrBehavior = SOLOMON_LRR_USE_ADDRESS;
	uint16_t id = solomon_read_reg_2byte(sol, SOLOMON_REG_DIR);
	if (id != SOLOMON_EXPECTED_DEVID)
	{
		sol->lrrBehavior = SOLOMON_LRR_USE_CONSTANT;
	}
	id = solomon_read_reg_2byte(sol, SOLOMON_REG_DIR);
	solomon_deselect(sol);
	if (id != SOLOMON_EXPECTED_DEVID)
	{
		return false;
	}
	return true;
}

void solomon_write_reg(Solomon_t const* sol, uint8_t addr, const uint8_t* data, size_t len)
{
	dcspi_write_packet_byteaddr(sol->spi, &(sol->dcSpi), addr, data, len);
}

void solomon_write_reg_byte(Solomon_t const* sol, uint8_t addr, uint8_t data)
{
	dcspi_write_packet_byteaddr(sol->spi, &(sol->dcSpi), addr, &data, 1);
}

void solomon_write_reg_2byte(Solomon_t const* sol, uint8_t addr, uint8_t data1, uint8_t data2)
{
	const uint8_t d[] = {data1, data2};
	dcspi_write_packet_byteaddr(sol->spi, &(sol->dcSpi), addr, d, 2);
}
void solomon_write_reg_word(Solomon_t const* sol, uint8_t addr, uint16_t data)
{
	const uint8_t d[] = {(uint8_t)(data & 0xFF), (uint8_t)((data >> CHAR_BIT) & 0xFF)};
	dcspi_write_packet_byteaddr(sol->spi, &(sol->dcSpi), addr, d, 2);
}

void solomon_read_reg(Solomon_t const* sol, uint8_t addr, uint8_t* data, size_t len)
{
	uint8_t lrrVal = SOLOMON_REG_LRR_RESET_VALUE;
	if (sol->lrrBehavior == SOLOMON_LRR_USE_ADDRESS)
	{
		lrrVal = addr;
	}
	solomon_write_reg_2byte(sol, SOLOMON_REG_LRR, lrrVal, 0x00);
	// Component switches to read mode if the second command byte
	// byte equals the contents of register
	// LRR (0xD4), which are, by default (and by setting above), 0xFA.
	// Oddly, no devices appear to work with lrrVal passed instead of the constant code.
	dcspi_read_packet_2byteaddr(sol->spi, &(sol->dcSpi), addr, SOLOMON_REG_LRR_RESET_VALUE, data, len);
}

uint16_t solomon_read_reg_2byte(Solomon_t const* sol, uint8_t addr)
{
	uint8_t tmp[2];
	solomon_read_reg(sol, addr, tmp, 2);
	return ((uint16_t)tmp[0]) | (((uint16_t)tmp[1]) << CHAR_BIT);
}

void solomon_reset(Solomon_t const* sol)
{
	solomon_start_video_shutdown(sol);
	delay_ms(10);
	solomon_start_reset(sol);
	delay_ms(10);
	solomon_end_reset(sol);
	delay_ms(10);
	solomon_end_video_shutdown(sol);
	delay_ms(10);
}
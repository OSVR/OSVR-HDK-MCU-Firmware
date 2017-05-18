/*
 * Solomon_SSD2828.c
 *
 *  Author: Ryan, Sensics, Inc.
 *  Copyright 2017 Sensics, Inc.
 */

#include "Solomon_SSD2828.h"
#include "DisplayControllerSPI.h"

#include "BitUtilsC.h"

#include "Console.h"

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
	ioport_set_pin_level(cs, true);
	ioport_set_pin_dir(cs, IOPORT_DIR_OUTPUT);
	ioport_enable_pin(cs);
#else
	ioport_configure_pin(cs, IOPORT_INIT_HIGH | IOPORT_DIR_OUTPUT);
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

#ifdef SVR_SOLOMON_VERBOSE
#define SVR_SOLOMON_VERBOSE_WRITE(X) Write(X)
#define SVR_SOLOMON_VERBOSE_WRITELN(X) WriteLn(X)
#else
#define SVR_SOLOMON_VERBOSE_WRITE(X) \
	do                               \
	{                                \
	} while (0)
#define SVR_SOLOMON_VERBOSE_WRITELN(X) \
	do                                 \
	{                                  \
	} while (0)
#endif
bool solomon_detect_lrr_behavior(Solomon_t* sol)
{
	SVR_SOLOMON_VERBOSE_WRITE("LRR detect: ");

	solomon_select(sol);
#if 0
	sol->lrrBehavior = SOLOMON_LRR_USE_ADDRESS;
	uint16_t id = solomon_read_reg_2byte(sol, SOLOMON_REG_DIR);
	if (id == SOLOMON_EXPECTED_DEVID)
	{
		SVR_SOLOMON_VERBOSE_WRITELN("Succeeded in address mode!");
		solomon_deselect(sol);
		return true;
	}

	SVR_SOLOMON_VERBOSE_WRITE("Failed address mode; Trying constant mode: ");
	sol->lrrBehavior = SOLOMON_LRR_USE_CONSTANT;
#else
	sol->lrrBehavior = SOLOMON_LRR_USE_CONSTANT;
	uint16_t id = solomon_read_reg_2byte(sol, SOLOMON_REG_DIR);
	if (id == SOLOMON_EXPECTED_DEVID)
	{
		SVR_SOLOMON_VERBOSE_WRITELN("Succeeded in constant mode!");
		solomon_deselect(sol);
		return true;
	}

	SVR_SOLOMON_VERBOSE_WRITE("Failed constant mode; Trying address mode: ");
	sol->lrrBehavior = SOLOMON_LRR_USE_ADDRESS;
#endif
	id = solomon_read_reg_2byte(sol, SOLOMON_REG_DIR);
	if (id != SOLOMON_EXPECTED_DEVID)
	{
		SVR_SOLOMON_VERBOSE_WRITELN("Failed overall!");
		solomon_deselect(sol);
		return false;
	}
	SVR_SOLOMON_VERBOSE_WRITELN("Succeeded!");
	solomon_deselect(sol);
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
	dcspi_write_packet_byteaddr(sol->spi, &(sol->dcSpi), addr, d, sizeof(d));
}
void solomon_write_reg_word(Solomon_t const* sol, uint8_t addr, uint16_t data)
{
	const uint8_t d[] = {BITUTILS_GET_NTH_LEAST_SIG_BYTE(0, data), BITUTILS_GET_NTH_LEAST_SIG_BYTE(1, data)};
	dcspi_write_packet_byteaddr(sol->spi, &(sol->dcSpi), addr, d, sizeof(d));
}

void solomon_read_reg(Solomon_t const* sol, uint8_t addr, uint8_t* data, size_t len)
{
	const uint8_t lrrVal = (sol->lrrBehavior == SOLOMON_LRR_USE_ADDRESS) ? addr : SOLOMON_REG_LRR_RESET_VALUE;

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

bool solomon_pll_is_locked(Solomon_t const* sol)
{
	return BITUTILS_CHECKBIT(solomon_read_reg_2byte(sol, SOLOMON_REG_ISR), SOLOMON_ISR_PLS_bm);
}

void solomon_pll_enable(Solomon_t const* sol)
{
	const uint16_t val = solomon_read_reg_2byte(sol, SOLOMON_REG_PCR);
	solomon_write_reg_word(sol, SOLOMON_REG_PCR, val | SOLOMON_PCR_PEN_bm);
}

void solomon_pll_disable(Solomon_t const* sol)
{
	const uint16_t val = solomon_read_reg_2byte(sol, SOLOMON_REG_PCR);
	solomon_write_reg_word(sol, SOLOMON_REG_PCR, val & ~SOLOMON_PCR_PEN_bm);
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

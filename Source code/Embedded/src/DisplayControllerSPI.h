/*
 * DisplayControllerSPI.h
 *
 *  Author: Ryan, Sensics, Inc.
 *  Copyright 2017 Sensics, Inc.
 */

#ifndef DISPLAYCONTROLLERSPI_H_
#define DISPLAYCONTROLLERSPI_H_

#include "GlobalOptions.h"

#include <stdbool.h>
#include <stdint.h>

#ifdef DCSPI_VERBOSE
#include <stdio.h>
#define DCSPI_VERBOSE_PRINTF(...) \
	do                            \
	{                             \
		printf(__VA_ARGS__);      \
	} while (0)
#else
#define DCSPI_VERBOSE_PRINTF(...) \
	do                            \
	{                             \
	} while (0)
#endif  // DCSPI_VERBOSE

#include <status_codes.h>
#include <ioport.h>
#include <spi_master.h>
#include <delay.h>

/** Configuration defines:

    - DCSPI_MUX_SUPPORT - compile time value, defaults to 1
    - DCSPI_MUX_OE_ENABLE_VALUE - bool, defaults to false
    - DCSPI_MUX_CHANGE_ON_DESELECT - compile time value, defaults to 0
    - DCSPI_MUX_OE_DISABLE_ON_DESELECT - compile time value, defaults to 0
*/

#ifndef DCSPI_MUX_SUPPORT
#define DCSPI_MUX_SUPPORT 1
#endif
#ifndef DCSPI_MUX_OE_ENABLE_VALUE
#define DCSPI_MUX_OE_ENABLE_VALUE false
#endif
#ifndef DCSPI_MUX_CHANGE_ON_DESELECT
#define DCSPI_MUX_CHANGE_ON_DESELECT 0
#endif
#ifndef DCSPI_MUX_OE_DISABLE_ON_DESELECT
#define DCSPI_MUX_OE_DISABLE_ON_DESELECT 0
#endif
#ifndef DCSPI_SELECT_DELAY_US
#define DCSPI_SELECT_DELAY_US 100
#endif
#ifndef DCSPI_START_ADDR_DELAY_US
#define DCSPI_START_ADDR_DELAY_US 100
#endif
#ifndef DCSPI_START_DATA_DELAY_US
#define DCSPI_START_DATA_DELAY_US 100
#endif
/// Configuration struct for a DCSPI channel.
typedef struct DisplayControlSPI_struct
{
	ioport_pin_t addrData;
	bool addrLevel;
#if DCSPI_MUX_SUPPORT
	/// if nonzero, will set level during select device
	ioport_pin_t muxPin;
	/// If non-zero, will be pulled to DCSPI_MUX_OE_ENABLE_VALUE at select device time.
	ioport_pin_t muxOe;
#endif  // DCSPI_MUX_SUPPORT
} DisplayControlSPI_t;

/// corresponds to (by containing) a `struct spi_device` with the extra capability of specifying mux pin level.
typedef struct DisplayControlSPI_Device_struct
{
	struct spi_device spiDevice;

#if DCSPI_MUX_SUPPORT
	/// If the muxPin is set, this is the muxLevel it will be set to in select device
	bool muxPinLevel;
#endif  // DCSPI_MUX_SUPPORT
} DisplayControlSPI_Device_t;

/// Includes call to spi_master_init
__always_inline static void dcspi_init(SPI_t *spi, DisplayControlSPI_t const *dcSPI)
{
	spi_master_init(spi);
	// Initial value for addrData should be data level.
	ioport_set_pin_level(dcSPI->addrData, !dcSPI->addrLevel);
	ioport_set_pin_dir(dcSPI->addrData, IOPORT_DIR_OUTPUT);

#if DCSPI_MUX_SUPPORT
	if (dcSPI->muxOe != 0)
	{
		ioport_set_pin_level(dcSPI->muxOe, !DCSPI_MUX_OE_ENABLE_VALUE);
		ioport_set_pin_dir(dcSPI->muxOe, IOPORT_DIR_OUTPUT);
		ioport_enable_pin(dcSPI->muxOe);
	}
	if (dcSPI->muxPin != 0)
	{
		ioport_set_pin_level(dcSPI->muxPin, true);
		ioport_set_pin_dir(dcSPI->muxPin, IOPORT_DIR_OUTPUT);
		ioport_enable_pin(dcSPI->muxPin);
	}
#endif  // DCSPI_MUX_SUPPORT
	ioport_enable_pin(dcSPI->addrData);
}
__always_inline static void dcspi_setup_device(SPI_t *spi, DisplayControlSPI_Device_t const *dcDev, spi_flags_t flags,
                                               unsigned long baud_rate)
{  // const_cast to remove constness - consumer doesn't actually use mutability, but isn't const-safe.
	spi_master_setup_device(spi, (struct spi_device *)&(dcDev->spiDevice), flags, baud_rate, 0);
}

/// Includes call to spi_select_device
__always_inline static void dcspi_select_device(SPI_t *spi, DisplayControlSPI_t const *dcSPI,
                                                DisplayControlSPI_Device_t const *dcDev)
{
#if DCSPI_MUX_SUPPORT
	if (dcSPI->muxOe != 0)
	{
		DCSPI_VERBOSE_PRINTF("enabling mux\r\n");
		ioport_set_pin_level(dcSPI->muxOe, DCSPI_MUX_OE_ENABLE_VALUE);
	}
	if (dcSPI->muxPin != 0)
	{
		DCSPI_VERBOSE_PRINTF("Setting mux level %d \r\n", ((dcDev->muxPinLevel) ? 1 : 0));
		ioport_set_pin_level(dcSPI->muxPin, dcDev->muxPinLevel);
	}
#endif  // DCSPI_MUX_SUPPORT
	// const_cast to remove constness - consumer doesn't actually use mutability, but isn't const-safe.
	spi_select_device(spi, (struct spi_device *)&(dcDev->spiDevice));
	delay_us(DCSPI_SELECT_DELAY_US);
}

/// Includes call to spi_deselect_device
__always_inline static void dcspi_deselect_device(SPI_t *spi, DisplayControlSPI_t const *dcSPI,
                                                  DisplayControlSPI_Device_t const *dcDev)
{
	// const_cast to remove constness - consumer doesn't actually use mutability, but isn't const-safe.
	spi_deselect_device(spi, (struct spi_device *)&(dcDev->spiDevice));
#if DCSPI_MUX_SUPPORT
#if DCSPI_MUX_OE_DISABLE_ON_DESELECT
	if (dcSPI->muxOe != 0)
	{
		DCSPI_VERBOSE_PRINTF("disabling mux\r\n");
		ioport_set_pin_level(dcSPI->muxOe, !DCSPI_MUX_OE_ENABLE_VALUE);
	}
#endif
#if DCSPI_MUX_CHANGE_ON_DESELECT
	if (dcSPI->muxPin != 0)
	{
		DCSPI_VERBOSE_PRINTF("Setting mux level %d \r\n", (!dcDev->muxPinLevel) ? 1 : 0);
		ioport_set_pin_level(dcSPI->muxPin, !dcDev->muxPinLevel);
	}
#endif
#endif  // DCSPI_MUX_SUPPORT
	delay_us(DCSPI_SELECT_DELAY_US);
}

__always_inline static void dcspi_start_addr(SPI_t *spi, DisplayControlSPI_t const *dcSPI)
{
	DCSPI_VERBOSE_PRINTF("Start addr\r\n");
	delay_us(DCSPI_START_ADDR_DELAY_US);
	ioport_set_pin_level(dcSPI->addrData, dcSPI->addrLevel);
	delay_us(DCSPI_START_ADDR_DELAY_US);
}

__always_inline static void dcspi_start_data(SPI_t *spi, DisplayControlSPI_t const *dcSPI)
{
	DCSPI_VERBOSE_PRINTF("Start data\r\n");
	delay_us(DCSPI_START_DATA_DELAY_US);
	ioport_set_pin_level(dcSPI->addrData, !(dcSPI->addrLevel));
	delay_us(DCSPI_START_DATA_DELAY_US);
}

__always_inline static void dcspi_wait(SPI_t *spi)
{
	while (!spi_is_rx_full(spi))
	{
		// spin loop
	}
}

/// must be followed by a `while (!spi_is_rx_full(spi))` loop!
__always_inline static void dcspi_internal_write_single(SPI_t *spi, uint8_t data)
{
	DCSPI_VERBOSE_PRINTF("[w:%02x]", data);
	spi_put(spi, data);
}

__always_inline static status_code_t dcspi_internal_write_packet(SPI_t *spi, const uint8_t *data, size_t len)
{
	DCSPI_VERBOSE_PRINTF("Write packet: ");
	while (len--)
	{
		dcspi_internal_write_single(spi, *data++);

		while (!spi_is_rx_full(spi))
		{
		}
		uint8_t tmp = spi_get(spi);
		DCSPI_VERBOSE_PRINTF("[r:%02x]", tmp);
	}
	DCSPI_VERBOSE_PRINTF("\r\n");

	return STATUS_OK;
}

__always_inline static status_code_t dcspi_internal_blocking_write_byte(SPI_t *spi, const uint8_t data)
{
	DCSPI_VERBOSE_PRINTF("Write byte: ");
	dcspi_internal_write_single(spi, data);

	while (!spi_is_rx_full(spi))
	{
	}
	uint8_t tmp = spi_get(spi);
	DCSPI_VERBOSE_PRINTF("[r:%02x]\r\n", tmp);
	return STATUS_OK;
}

__always_inline static status_code_t dcspi_internal_read_packet(SPI_t *spi, uint8_t *data, size_t len)
{
	DCSPI_VERBOSE_PRINTF("Read packet: ");
	while (len--)
	{
		dcspi_internal_write_single(spi, CONFIG_SPI_MASTER_DUMMY);  // Dummy write

		while (!spi_is_rx_full(spi))
		{
		}

		spi_read_single(spi, data);
		DCSPI_VERBOSE_PRINTF("[r:%02x]", *data);
		data++;
	}
	DCSPI_VERBOSE_PRINTF("\r\n");

	return STATUS_OK;
}

/// @brief Sends arbitrary length address/command and data packet.
/// @pre SPI device must be selected first with dcspi_select_device
__always_inline static status_code_t dcspi_write_packet(SPI_t *spi, DisplayControlSPI_t const *dcSPI,
                                                        const uint8_t *addr, size_t addrLen, const uint8_t *data,
                                                        size_t len)
{
	DCSPI_VERBOSE_PRINTF("dcspi_write_packet\r\n");
	dcspi_start_addr(spi, dcSPI);
	status_code_t ret = dcspi_internal_write_packet(spi, addr, addrLen);
	if (ret != STATUS_OK)
	{
		return ret;
	}
	dcspi_start_data(spi, dcSPI);
	return dcspi_internal_write_packet(spi, data, len);
}

/// @brief Sends 8 bit address and arbitrary length data packet.
/// @pre SPI device must be selected first with dcspi_select_device
__always_inline static status_code_t dcspi_write_packet_byteaddr(SPI_t *spi, DisplayControlSPI_t const *dcSPI,
                                                                 uint8_t addr, const uint8_t *data, size_t len)
{
	DCSPI_VERBOSE_PRINTF("dcspi_write_packet_byteaddr\r\n");
	dcspi_start_addr(spi, dcSPI);
	dcspi_internal_blocking_write_byte(spi, addr);
	dcspi_start_data(spi, dcSPI);
	return dcspi_internal_write_packet(spi, data, len);
}

/// @brief Sends 16 bit address and arbitrary length data packet.
/// @pre SPI device must be selected first with dcspi_select_device
__always_inline static status_code_t dcspi_write_packet_twobyteaddr(SPI_t *spi, DisplayControlSPI_t const *dcSPI,
                                                                    uint8_t addrFirst, uint8_t addrSecond,
                                                                    const uint8_t *data, size_t len)
{
	DCSPI_VERBOSE_PRINTF("dcspi_write_packet_twobyteaddr\r\n");
	dcspi_start_addr(spi, dcSPI);
	dcspi_internal_blocking_write_byte(spi, addrFirst);
	dcspi_internal_blocking_write_byte(spi, addrSecond);
	dcspi_start_data(spi, dcSPI);
	return dcspi_internal_write_packet(spi, data, len);
}

/// @brief Sends 8 bit address, then reads arbitrary length data packet.
/// @pre SPI device must be selected first with dcspi_select_device
__always_inline static status_code_t dcspi_read_packet_byteaddr(SPI_t *spi, DisplayControlSPI_t const *dcSPI,
                                                                uint8_t addr, uint8_t *data, size_t len)
{
	DCSPI_VERBOSE_PRINTF("dcspi_read_packet_byteaddr\r\n");
	dcspi_start_addr(spi, dcSPI);
	dcspi_internal_blocking_write_byte(spi, addr);
	dcspi_start_data(spi, dcSPI);
	return dcspi_internal_read_packet(spi, data, len);
}

/// @brief Sends 16 bit address, then reads arbitrary length data packet.
/// @pre SPI device must be selected first with dcspi_select_device
__always_inline static status_code_t dcspi_read_packet_2byteaddr(SPI_t *spi, DisplayControlSPI_t const *dcSPI,
                                                                 uint8_t addrFirst, uint8_t addrSecond, uint8_t *data,
                                                                 size_t len)
{
	DCSPI_VERBOSE_PRINTF("dcspi_read_packet_2byteaddr\r\n");
	dcspi_start_addr(spi, dcSPI);
	dcspi_internal_blocking_write_byte(spi, addrFirst);
	dcspi_internal_blocking_write_byte(spi, addrSecond);
	dcspi_start_data(spi, dcSPI);
	return dcspi_internal_read_packet(spi, data, len);
}
#endif /* DISPLAYCONTROLLERSPI_H_ */
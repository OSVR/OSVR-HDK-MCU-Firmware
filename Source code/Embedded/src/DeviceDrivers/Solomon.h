/*
 * Solomon.h
 *
 * Created: 9/7/2013 11:35:02 PM
 *  Author: Sensics
 */

#ifndef SOLOMON_H_
#define SOLOMON_H_

#include "GlobalOptions.h"
#include "DeviceDrivers/Display.h"
#include <stdbool.h>
#include <stdint.h>

#ifdef SVR_HAVE_SOLOMON1
#define Solomon1 0
#endif  // SVR_HAVE_SOLOMON1

#ifdef SVR_HAVE_SOLOMON2
#define Solomon2 1
#endif  // SVR_HAVE_SOLOMON2

void init_solomon(void);
bool init_solomon_device(uint8_t deviceID);
uint16_t read_solomon(uint8_t channel, uint8_t address);
void write_solomon(uint8_t channel, uint8_t address, uint16_t data);
void write_solomon_pair(uint8_t channel, uint8_t address, uint16_t data1, uint16_t data2);
void raise_sdc(uint8_t channel);
void lower_sdc(uint8_t channel);
void Solomon_Reset(uint8_t SolomonNum);

/// Writes out the config register to the console.
void Solomon_Dump_All_Config_Debug(const char* loc);
/// Writes out the config register to the console for just one device.
void Solomon_Dump_Config_Debug(uint8_t deviceId, const char* loc);

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

/// Packet drop register
#define SOLOMON_REG_PDR UINT8_C(0xBF)
/// PLL control reg
#define SOLOMON_REG_PCR UINT8_C(0xBF)
/// PLL enable
static const uint16_t SOLOMON_PCR_PEN_bm = (UINT16_C(0x01));

// read the solomon ID
uint16_t read_Solomon_ID(uint8_t channel);

#endif /* SOLOMON_H_ */

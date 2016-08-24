/*
 * Copyright 2016 OSVR and contributors.
 * Copyright 2016 Dennis Yeh.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#ifndef HDKV15_H_
#define HDKV15_H_

#include <asf.h>
#include "GlobalOptions.h"
#include "twi_master.h"
#include "status_codes.h"

#include <stdint.h>

#define TC358870 0

#define TC358870_OK 0
#define TC358870_ERROR -1

#define TC358870_NoSync 0
#define TC358870_Sync 1

#define TC358870_TWI_SPEED 100000  // or 400kHz or 2MHz
#define TC358870_ADDR 0x1F         // or 0x1F

#define VIDEO_POLLING_PERIOD 60000
#define VIDEO_POLLING_TIMER TCC2

#define EEP_ADDR_SN 0x008  // 20160605, fctu, change from 0xFF0 to 0x008 due to RAZER asked.
#define SN_LENGTH 16       // include 1 byte check sum.

inline uint8_t ascii_to_dec_8(uint8_t *buf) { return (buf[0] - '0') * 10 + (buf[1] - '0'); }
extern unsigned char sn[SN_LENGTH];

#if 1  // Dennis Yeh : for Toshiba I2C
#define i2c1_uh2d_write8(x, y) TC358870_i2c_Write(x, y, 1)
#define i2c1_uh2d_write16(x, y) TC358870_i2c_Write(x, y, 2)
#define i2c1_uh2d_write32(x, y) TC358870_i2c_Write(x, y, 4)
#endif

int TC358870_i2c_Init(void);

int TC358870_i2c_Read(ui16_t RegNum, ui8_t *data);
int TC358870_i2c_Write(ui16_t RegNum, ui32_t nValue, int nLength);

int AUO_H381DLN01_Init(int bDisplayON);
int TC358870_Init_Receive_HDMI_Signal(void);

bool IsVideoExistingPolling(void);
bool PowerOnSeq(void);
void AUO_H381DLN01_Reset(void);
void Toshiba_TC358870_Reset(void);

void Debug_LED_Reset(void);
int TC358870_VideoSyncSignalStatus(void);

int TC358870_Reset_MIPI(void);
int TC358870_CheckLANEStatus(void);
int TC358870_Check0x0294tatus(void);

void eep_write_sn(void);
int eep_read_sn(unsigned char *buf);
void OSVR_HDK_EDID(void);
void ProcessFactoryCommand(void);
void nvm_eeprom_write_byte_(eeprom_addr_t address, uint8_t value);

#endif /* HDKV15_H_ */

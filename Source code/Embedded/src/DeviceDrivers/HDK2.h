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
#include <stdbool.h>

#define TC358870_OK 0
#define TC358870_ERROR -1

#define TC358870_TWI_SPEED 100000  // or 400kHz or 2MHz
#define TC358870_ADDR 0x1F         // or 0x1F

#define EEP_ADDR_SN 0x008  // 20160605, fctu, change from 0xFF0 to 0x008 due to RAZER asked.
#define SN_LENGTH 16       // include 1 byte check sum.

int TC358870_i2c_Read(uint16_t RegNum, uint8_t *data);
int TC358870_i2c_Write(uint16_t RegNum, uint32_t nValue, int nLength);

/// Given a buffer of at least SN_LENGTH size, puts a null-terminated string in it. If the SN could be read and checksum
/// verified, then true is returned and a non-empty null-terminated string is in the buffer.
bool eep_get_sn(uint8_t buf[SN_LENGTH]);

void OSVR_HDK_EDID(void);
void ProcessFactoryCommand(void);
void nvm_eeprom_write_byte_(eeprom_addr_t address, uint8_t value);

#ifdef HDK_20

#define VIDEO_POLLING_PERIOD     60000
#define VIDEO_POLLING_TIMER      TCC2
#define DEBUG_LED_ON    0
#define DEBUG_LED_OFF   1
#define TC358870_NoSync	0
#define TC358870_Sync	1

typedef enum {
    VIDEO_NOT_EXIST,
    VIDEO_EXIST
} VIDEO_EXISTING;

void IsVideoExistingPolling (void);
int TC358870_Init_Receive_HDMI_Signal(void);
int TC358870_i2c_Init(void);
bool PowerOnSeq(void);

#endif  // HDK_20

#endif /* HDKV15_H_ */

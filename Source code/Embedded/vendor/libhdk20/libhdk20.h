/*
 * HDKv15.h
 *
 * Created: 2016/3/14 下午 03:33:26
 *  Author: dennis.yeh
 */



/*
This has been stripped down to be just the header required to use the libhdk20.a
binary blob without duplicating code found or better located elsewhere in the
firmware. It has also been re-organized to be in alphabetical symbol order and
roughly grouped.

Note that the libhdk20.a library is not self-sufficient: it contains undefined
symbols that it expects to be able to resolve from this project, some of which
are implementation details, not just ASF methods. In particular, these symbols
are undefined and are not obviously either standard library/runtime symbols or
ASF symbols:

WriteLn (part of Console)
DebugLevel (part of Console)
HDMI_IsVideoExisting (was added in Coretronic fork of firmware, referenced by library export IsVideoExistingPolling)
UpdateResolutionDetection (Coretronic fork has an empty implementation: referenced twice by library export IsVideoExistingPolling)
Update_BNO_Report_Header (normally called by main() upon a display event)
CommandToExecute (part of SerialStateMachine)

-- Ryan A. Pavlik, Ph.D.
   Sensics, Inc.
*/

#ifndef HDKV15_H_
#define HDKV15_H_

#if 0
#include <asf.h>
#include "VariantOptions.h"
#include "GlobalOptions.h"
#include "twi_master.h"
#include "status_codes.h"
#endif

#include <nvm.h>

#define TC358870_OK		0
#define TC358870_ERROR	-1

#define TC358870_NoSync	0
#define TC358870_Sync	1

#define TC358870_TWI_SPEED	100000	// or 400kHz or 2MHz
#define TC358870_ADDR		0x1F	// or 0x1F

#define VIDEO_POLLING_PERIOD     60000
#define VIDEO_POLLING_TIMER      TCC2

/// Inline function included in this header but not needed.
#if 0
inline uint8_t ascii_to_dec_8 (uint8_t *buf) { return (buf[0]-'0')*10 + (buf[1]-'0'); }
#endif

/// @todo Replace these with the proper stdint.h types.
typedef unsigned char  ui8_t;
typedef unsigned short ui16_t;
typedef unsigned long  ui32_t;

int AUO_H381DLN01_Init(int bDisplayON);
void AUO_H381DLN01_Reset(void);

// debugPrintf text symbol exported by library but no match in header
// EDID_LUT read-only data symbol exported by library but no match in header

void eep_write_sn (void);
int eep_read_sn (unsigned char *buf);

bool IsVideoExistingPolling (void);

/// Unknown how this differs from the original version in the standard Atmel ASF
void nvm_eeprom_write_byte_(eeprom_addr_t address, uint8_t value);

void OSVR_HDK_EDID (void);
bool PowerOnSeq(void);
void ProcessFactoryCommand(void);

#define EEP_ADDR_SN     0x008   // 20160605, fctu, change from 0xFF0 to 0x008 due to RAZER asked.
#define SN_LENGTH       16  // include 1 byte check sum.
extern unsigned char sn[SN_LENGTH];

int TC358870_Check0x0294tatus(void);
int TC358870_CheckLANEStatus(void);
int TC358870_i2c_Init(void);
int TC358870_i2c_Read(ui16_t RegNum, ui8_t *data);
int TC358870_i2c_Write(ui16_t RegNum, ui32_t nValue, int nLength);
int TC358870_Init_Receive_HDMI_Signal(void);
int TC358870_Reset_MIPI(void);
int TC358870_VideoSyncSignalStatus(void);
void Toshiba_TC358870_Reset(void);

/// Made definition of these optional since they pollute the namespace.
#ifdef DEFINE_TOSHIBA_I2C_MACROS // Dennis Yeh : for Toshiba I2C
	#define i2c1_uh2d_write8(x,y)  TC358870_i2c_Write(x,y,1)
	#define i2c1_uh2d_write16(x,y)  TC358870_i2c_Write(x,y,2)
	#define i2c1_uh2d_write32(x,y)  TC358870_i2c_Write(x,y,4)
#endif

// This was declared in this header but not included in the library.
//void Debug_LED_Reset(void);

#endif /* HDKV15_H_ */

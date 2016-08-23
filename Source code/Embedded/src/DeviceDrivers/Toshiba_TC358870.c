/*
 * Toshiba_TC358870.c
 *
 * Created: 7/21/2016 8:30:05 AM
 *  Author: Coretronic, Sensics
 */
/*
 * Copyright 2016 Sensics, Inc.
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

// Options header
#include "GlobalOptions.h"

#ifdef SVR_HAVE_TOSHIBA_TC358870

#include "Toshiba_TC358870.h"

// Application headers
#include "Console.h"
#include "my_hardware.h"
#include "SvrYield.h"

// Vendor library header
#include <libhdk20.h>

// asf header
#include <ioport.h>
#include <twi_master.h>

// standard header
#include <limits.h>

static TC358870_Op_Status_t Toshiba_TC358870_I2C_Write_Impl(TC358870_Reg_t reg, uint8_t* buf, uint8_t len);

static uint8_t s_tc358870_init_count = 0;

void Toshiba_TC358870_Init(void)
{
#ifdef HDMI_VERBOSE
	WriteLn("Toshiba_TC358870_Init: Start");
#endif
	// Dennis Yeh 2016/03/14 : for TC358870
	uint8_t tc_data;
	TC358870_i2c_Init();
	TC358870_i2c_Read(0x0000, &tc_data);

	PowerOnSeq();
	s_tc358870_init_count++;
#ifdef HDMI_VERBOSE
	WriteLn("Toshiba_TC358870_Init: End");
#endif
}

void Toshiba_TC358870_Init_Receiver() { TC358870_Init_Receive_HDMI_Signal(); }
uint8_t Toshiba_TC358870_Get_Init_Count() { return s_tc358870_init_count; }
void Toshiba_TC358870_Trigger_Reset()
{
	WriteLn("Toshiba_TC358870 Resetting");
	ioport_set_pin_low(TC358870_Reset_Pin);
	svr_yield_ms(12);
	ioport_set_pin_high(TC358870_Reset_Pin);
}

#define TC_MAKE_SINGLE_BIT_MASK(BIT) (0x01 << BIT)

enum
{
	TC_REG_SYS_CONTROL = 0x0002,
	TC_REG_CONFIG_CONTROL_0 = 0x0004,
	TC_REG_CONFIG_CONTROL_1 = 0x0006,
	TC_REG_DCSCMD_Q = 0x0504,
	TC_REG_SYS_STATUS = 0x8520,
#if 0
TC_REG_SYS_STATUS_HAVE_VIDEO_MASK = TC_MAKE_SINGLE_BIT_MASK(7) | TC_MAKE_SINGLE_BIT_MASK(3) /* PHY DE detect */ | TC_MAKE_SINGLE_BIT_MASK(2) /* PHY PLL lock */ | TC_MAKE_SINGLE_BIT_MASK(1) /* TMDS input amplitude */ | TC_MAKE_SINGLE_BIT_MASK(0) /* DDC_Power input */
#endif
	TC_REG_SYS_STATUS_HAVE_VIDEO_BIT_MASK = TC_MAKE_SINGLE_BIT_MASK(7)
};

/// Checks that all the bits in the mask are set. Factored out because I'm not sure what will give the best performance.
static inline bool checkMask(uint8_t value, uint8_t mask)
{
#if 0
	return (value & mask) == mask;
#else
	// xor should flip all masked bits from 1 to 0, if they were set, then the and should give us only any
	// previously-unset bits in the masked bits that remain. This lets us compare against 0. Who knows, the compiler may
	// be clever enough to turn the first approach into this.
	return ((value ^ mask) & mask) == 0x0;
#endif
}

/// General implementation of the I2C write, used by all sizes of writes.
static inline TC358870_Op_Status_t Toshiba_TC358870_I2C_Write_Impl(TC358870_Reg_t reg, uint8_t* buf, uint8_t len)
{
	twi_package_t packet_write = {
	    .addr = reg,                            //< register to write to
	    .addr_length = sizeof(TC358870_Reg_t),  //< length of register address
	    .chip = TC358870_ADDR,                  //< bus address
	    .buffer = (void*)buf,                   //< buffer to write
	    .length = len,                          //< buffer length
	    .no_wait = false                        //< block until the bus is available to send this.
	};
	return (TC358870_Op_Status_t)twi_master_write(TC358870_TWI_PORT, &packet_write);
}

TC358870_Op_Status_t Toshiba_TC358870_I2C_Write8(TC358870_Reg_t reg, uint8_t val)
{
	return Toshiba_TC358870_I2C_Write_Impl(reg, &val, sizeof(val));
}

TC358870_Op_Status_t Toshiba_TC358870_I2C_Write16(TC358870_Reg_t reg, uint16_t val)
{
	uint8_t buf[sizeof(val)];
	buf[0] = (uint8_t)(val & 0xff);
	buf[1] = (uint8_t)((val >> CHAR_BIT) & 0xff);
	return Toshiba_TC358870_I2C_Write_Impl(reg, buf, sizeof(buf));
}

TC358870_Op_Status_t Toshiba_TC358870_I2C_Write32(TC358870_Reg_t reg, uint32_t val)
{
	uint8_t buf[sizeof(val)];
	buf[0] = (uint8_t)(val & 0xff);
	buf[1] = (uint8_t)((val >> CHAR_BIT) & 0xff);
	buf[2] = (uint8_t)((val >> (2 * CHAR_BIT)) & 0xff);
	buf[3] = (uint8_t)((val >> (3 * CHAR_BIT)) & 0xff);
	return Toshiba_TC358870_I2C_Write_Impl(reg, buf, sizeof(buf));
}

/// assumes the mask you pass has only one bit set!
static inline bool checkBit(uint8_t value, uint8_t mask) { return (value & mask) != 0; }
/* Documentation from old Coretronic-authored predecessor to this function follows: note that the comparison to 0x9f is
   faulty.
    Function : Toshiba_TC358870_HDMI_IsVideoExisting
    IN : void
    OUT: 0: video does not exist
              1: video exist

     To read TC358870 register SYS_STATUS (0x8520) to check  video status.
     if the value is 0x9F, means the video is ready, otherwise it's not exist.

*/
bool Toshiba_TC358870_Have_Video_Sync(void)
{
	uint8_t tc_data;

	if (TC358870_i2c_Read(TC_REG_SYS_STATUS, &tc_data) != TC358870_OK)  // get SYS_STATUS
		return false;

	/// @todo - should we check the lower nybble too (PHY DE detect, PHY PLL, TMDS input amplitude, and DDC power
	/// input)?
	/// Bit 7 is input video sync - bits 6, 5, and 4 are unimportant to the task at hand, so equality to 0x9f is not
	/// quite right.
	return checkBit(tc_data, TC_REG_SYS_STATUS_HAVE_VIDEO_BIT_MASK);
}

/// Send a short DSI command with no parameter.
void Toshiba_TC358870_DSI_Write_Cmd_Short(uint8_t cmd)
{
	TC358870_i2c_Write(TC_REG_DCSCMD_Q, 0x0005, 2);
	TC358870_i2c_Write(TC_REG_DCSCMD_Q, (uint32_t)cmd, 2);
}

/// Send a short DSI command with one parameter.
void Toshiba_TC358870_DSI_Write_Cmd_Short_Param(uint8_t cmd, uint8_t param)
{
	TC358870_i2c_Write(TC_REG_DCSCMD_Q, 0x0015, 2);
	// uint8_t yyCmd, uint8_t zzArg
	// want to send 0xzzyy
	TC358870_i2c_Write(TC_REG_DCSCMD_Q, (((uint16_t)param) << sizeof(cmd)) | ((uint16_t)cmd), 2);
}

#if 0
void Toshiba_TC358870_Set_Address_AutoIncrement(bool value)
{
	static const uint8_t AutoIndexBit = 2;
	uint8_t data;
	if (TC358870_i2c_Read(TC_REG_CONFIG_CONTROL_0, &data) != TC358870_OK) {
		// don't set if we couldn't read.
		return;
	}
	if (value) {
	data |= (0x01 << AutoIndexBit);
	} else {
	data &= !(0x01<<AutoIndexBit);
	}
	TC358870_i2c_Write
}
#endif

void Toshiba_TC358870_Set_MIPI_PLL_Config(uint8_t output, Toshiba_TC358870_MIPI_PLL_Conf_t conf)
{
	uint32_t val = 0;
	// load in MP_LBW
	val |= (uint32_t)(0x03 & ((uint8_t)conf.low_bandwidth_setting)) << 16;
	// now MP_PRD
	val |= (uint32_t)(0x0f & (conf.input_divider - 1)) << 12;
	// MP_FRS
	val |= (uint32_t)(0x03 & ((uint8_t)conf.hsck_freq_range_post_divider)) << 10;
	// LFBREN
	if (conf.lower_freq_bound_removal)
	{
		val |= (uint32_t)(0x01) << 9;
	}
	// Finally, MP_FBD
	val |= (uint32_t)(0x01ff & (conf.feedback_divider_value - 1));
}

/// Send a "long" DSI command with data (may be of length 0)
// void Toshiba_TC358870_DSI_Write_Cmd_Long(uint8_t cmd, uint16_t len, uint8_t * data);

#endif  // SVR_HAVE_TOSHIBA_TC358870
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
#include "Toshiba_TC358870_ISR.h"

// Application headers
#include "Console.h"
#include "my_hardware.h"
#include "SvrYield.h"
#include "BitUtilsC.h"

// Vendor library header
#include "DeviceDrivers/HDK2.h"

// asf headers
#include <ioport.h>
#include <twi_master.h>
#include <delay.h>

// standard header
#include <limits.h>

#include "Toshiba_TC358870_Setup_Impl.h"

// -- Basic support functionality -- //

/// General implementation of the I2C write, used by all sizes of writes.
static TC358870_Op_Status_t Toshiba_TC358870_I2C_Write_Impl(TC358870_Reg_t reg, uint8_t* buf, uint8_t len);

/// General implementation of the I2C read, used by all sizes of reads.
static TC358870_Op_Status_t Toshiba_TC358870_I2C_Read_Impl(TC358870_Reg_t reg, uint8_t* buf, uint8_t len);

/// The toshiba chip's registers have two-byte addresses. We need to split that up to fit into the address array.
#define TC_REG_CONVERSION(REG)                        \
	{                                                 \
		bitUtils_highByte(REG), bitUtils_lowByte(REG) \
	}

static inline TC358870_Op_Status_t Toshiba_TC358870_I2C_Write_Impl(TC358870_Reg_t reg, uint8_t* buf, uint8_t len)
{
	twi_package_t packet_write = {
	    .addr = TC_REG_CONVERSION(reg),         //< register to write to, disassembled into bytes.
	    .addr_length = sizeof(TC358870_Reg_t),  //< length of register address
	    .chip = TC358870_ADDR,                  //< bus address
	    .buffer = (void*)buf,                   //< buffer to write
	    .length = len,                          //< buffer length
	    .no_wait = false                        //< block until the bus is available to send this.
	};
	TC358870_Op_Status_t status = (TC358870_Op_Status_t)twi_master_write(TC358870_TWI_PORT, &packet_write);
	while (TOSHIBA_TC358770_OK != status && TOSHIBA_TC358770_ERR_INVALID_ARG != status)
	{
		/// We think we can beat io, buffer, and bus state errors.
		status = (TC358870_Op_Status_t)twi_master_write(TC358870_TWI_PORT, &packet_write);
	}
	return status;
}

static inline TC358870_Op_Status_t Toshiba_TC358870_I2C_Read_Impl(TC358870_Reg_t reg, uint8_t* buf, uint8_t len)
{
	twi_package_t packet_read = {
	    .addr = TC_REG_CONVERSION(reg),         //< register to write to, disassembled into bytes.
	    .addr_length = sizeof(TC358870_Reg_t),  //< length of register address
	    .chip = TC358870_ADDR,                  //< bus address
	    .buffer = (void*)buf,                   //< buffer to read
	    .length = len,                          //< buffer length
	    .no_wait = false                        //< block until the bus is available to send this.
	};
	return (TC358870_Op_Status_t)twi_master_read(TC358870_TWI_PORT, &packet_read);
}

TC358870_Op_Status_t Toshiba_TC358870_I2C_Write8(TC358870_Reg_t reg, uint8_t val)
{
	return Toshiba_TC358870_I2C_Write_Impl(reg, &val, sizeof(val));
}

TC358870_Op_Status_t Toshiba_TC358870_I2C_Write16(TC358870_Reg_t reg, uint16_t val)
{
	uint8_t buf[sizeof(val)];
	buf[0] = BITUTILS_GET_NTH_LEAST_SIG_BYTE(0, val);
	buf[1] = BITUTILS_GET_NTH_LEAST_SIG_BYTE(1, val);
	return Toshiba_TC358870_I2C_Write_Impl(reg, buf, sizeof(buf));
}

TC358870_Op_Status_t Toshiba_TC358870_I2C_Write32(TC358870_Reg_t reg, uint32_t val)
{
	uint8_t buf[sizeof(val)];
	buf[0] = BITUTILS_GET_NTH_LEAST_SIG_BYTE(0, val);
	buf[1] = BITUTILS_GET_NTH_LEAST_SIG_BYTE(1, val);
	buf[2] = BITUTILS_GET_NTH_LEAST_SIG_BYTE(2, val);
	buf[3] = BITUTILS_GET_NTH_LEAST_SIG_BYTE(3, val);
	return Toshiba_TC358870_I2C_Write_Impl(reg, buf, sizeof(buf));
}

TC358870_Op_Status_t Toshiba_TC358870_I2C_Read8(TC358870_Reg_t reg, uint8_t* val)
{
	return Toshiba_TC358870_I2C_Read_Impl(reg, val, sizeof(uint8_t));
}

TC358870_Op_Status_t Toshiba_TC358870_I2C_Read16(TC358870_Reg_t reg, uint16_t* val)
{
	uint8_t buf[2];
	TC358870_Op_Status_t status = Toshiba_TC358870_I2C_Read_Impl(reg, buf, sizeof(uint16_t));
	*val = (((uint16_t)(buf[1])) << CHAR_BIT) | ((uint16_t)(buf[0]));
	return status;
}

// Select register addresses

static const TC358870_Reg_t TC_REG_DSITX1_OFFSET = 0x0200;  //< add to any DSITX0 register address
static const TC358870_Reg_t TC_REG_SYS_CONTROL = 0x0002;
static const TC358870_Reg_t TC_REG_CONFIG_CONTROL_0 = 0x0004;
static const TC358870_Reg_t TC_REG_CONFIG_CONTROL_1 = 0x0006;
static const TC358870_Reg_t TC_REG_INT_STATUS = 0x0014;
static const TC358870_Reg_t TC_REG_INT_MASK = 0x0016;
static const TC358870_Reg_t TC_REG_DCSCMD_Q = 0x0504;
static const TC358870_Reg_t TC_REG_MISC_INT = 0x850B;
static const TC358870_Reg_t TC_REG_MISC_INT_MASK = 0x851B;
static const TC358870_Reg_t TC_REG_SYS_STATUS = 0x8520;
#if 0
	TC_REG_SYS_STATUS_HAVE_VIDEO_MASK = BITUTILS_BIT(7) | BITUTILS_BIT(3) /* PHY DE detect */ | BITUTILS_BIT(2) /* PHY PLL lock */ | BITUTILS_BIT(1) /* TMDS input amplitude */ | BITUTILS_BIT(0) /* DDC_Power input */
#endif

#define TC_REG_INT_STATUS_HDMI_INT_BITMASK BITUTILS_BIT(9)
#define TC_REG_INT_STATUS_NONRESERVED_BITS (BITUTILS_GET_SET_LOW_BITS_MAX16(12) & (~BITUTILS_BIT(6)))
#define TC_REG_MISC_INT_SYNC_CHG_BITMASK BITUTILS_BIT(1)
static const uint8_t TC_REG_SYS_STATUS_HAVE_VIDEO_BIT_MASK = BITUTILS_BIT(7);

static uint8_t s_tc358870_init_count = 0;

void Toshiba_TC358870_Base_Init(void)
{
	static bool repeat = false;
	bool firstTime = false;
	if (!repeat)
	{
		// this is the first time we've been in here!
		firstTime = true;
		repeat = true;
	}

#ifdef HDMI_VERBOSE
	WriteLn("Toshiba_TC358870_Init: Start");
#endif

	// disable interrupts
	Toshiba_TC358870_MCU_Ints_Suspend();
	if (firstTime)
	{
		twi_master_options_t opt = {
		    .speed = TC358870_TWI_SPEED,  //< used in twi_master_setup with the TWI_BAUD macro and
		                                  // the system clock to compute the .speed_reg member.
		    .chip = TC358870_ADDR};
		/// twi_master_setup sets .speed_reg for you and starts appropriate clocks before calling twi_master_init.
		twi_master_setup(TC358870_TWI_PORT, &opt);
	}

	ioport_set_pin_low(TC358870_Reset_Pin);
	ioport_set_pin_low(PANEL_RESET);

	WriteLn("Toshiba_TC358870_Base_Init: Waiting for 5V power rail");
	while (!ioport_get_value(ANA_PWR_IN))
	{
		delay_us(50);
	}
	svr_yield_ms(10);
	WriteLn("Toshiba_TC358870_Base_Init: Waiting for low-voltage power rail");
	while (!ioport_get_value(TC358870_PWR_GOOD))
	{
		delay_us(50);
	}
	svr_yield_ms(50);
	ioport_set_pin_high(TC358870_Reset_Pin);
	ioport_set_pin_high(PANEL_RESET);
	svr_yield_ms(5);

#if 0
	// Dennis Yeh 2016/03/14 : for TC358870
	uint8_t tc_data;
	/// dummy read?
	Toshiba_TC358870_I2C_Read8(0x0000, &tc_data);
#endif

	// Turn on auto-increment.
	// Toshiba_TC358870_I2C_Write8(TC_REG_CONFIG_CONTROL_0, BITUTILS_BIT(2));
	Toshiba_TC358870_SW_Reset();
	Toshiba_TC358870_Prepare_TX();
	Toshiba_TC358870_Configure_Splitter();
	Toshiba_TC358870_HDMI_Setup();

	// Toshiba_TC358870_Init_Receiver();
	s_tc358870_init_count++;

	if (firstTime)
	{
		Toshiba_TC358870_MCU_Ints_Init();
	}
	else
	{
		// not our first go-round, we'll just resume ints here.
		Toshiba_TC358870_MCU_Ints_Resume();
	}

#ifdef HDMI_VERBOSE
	WriteLn("Toshiba_TC358870_Init: End");
#endif
}

bool Toshiba_TC358870_Init_Once(void)
{
	if (0 == s_tc358870_init_count)
	{
		Toshiba_TC358870_Base_Init();
		return true;
	}
	return false;
}

// void Toshiba_TC358870_Init_Receiver() { TC358870_Init_Receive_HDMI_Signal(); }
uint8_t Toshiba_TC358870_Get_Init_Count() { return s_tc358870_init_count; }
void Toshiba_TC358870_Trigger_Reset()
{
	WriteLn("Toshiba_TC358870 Resetting");
	ioport_set_pin_low(TC358870_Reset_Pin);
	svr_yield_ms(50);
	ioport_set_pin_high(TC358870_Reset_Pin);
}

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
#if 0
	if (TC358870_i2c_Read(TC_REG_SYS_STATUS, &tc_data) != TC358870_OK)  // get SYS_STATUS
		return false;
#endif
	if (TOSHIBA_TC358770_OK != Toshiba_TC358870_I2C_Read8(TC_REG_SYS_STATUS, &tc_data))
	{
		return false;
	}
	/// @todo - should we check the lower nybble too (PHY DE detect, PHY PLL, TMDS input amplitude, and DDC power
	/// input)?
	/// Bit 7 is input video sync - bits 6, 5, and 4 are unimportant to the task at hand, so equality to 0x9f is not
	/// quite right.
	return bitUtils_checkBit(tc_data, TC_REG_SYS_STATUS_HAVE_VIDEO_BIT_MASK);
}

/// Send a short DSI command with no parameter.
void Toshiba_TC358870_DSI_Write_Cmd_Short(uint8_t cmd)
{
	Toshiba_TC358870_I2C_Write16(TC_REG_DCSCMD_Q, 0x0005);
	Toshiba_TC358870_I2C_Write16(TC_REG_DCSCMD_Q, (uint16_t)cmd);
}

/// Send a short DSI command with one parameter.
void Toshiba_TC358870_DSI_Write_Cmd_Short_Param(uint8_t cmd, uint8_t param)
{
	Toshiba_TC358870_I2C_Write16(TC_REG_DCSCMD_Q, 0x0015);
	// uint8_t yyCmd, uint8_t zzArg
	// want to send 0xzzyy
	Toshiba_TC358870_I2C_Write16(TC_REG_DCSCMD_Q, (((uint16_t)param) << sizeof(cmd)) | ((uint16_t)cmd));
}

void Toshiba_TC358870_Set_MIPI_PLL_Config(uint8_t output, Toshiba_TC358870_MIPI_PLL_Conf_t conf)
{
	uint32_t val = 0;
	// load in MP_LBW
	val |= (uint32_t)(BITUTILS_KEEP_LOW_BITS_MAX8(2, (uint8_t)conf.low_bandwidth_setting)) << 16;
	// now MP_PRD
	val |= (uint32_t)(BITUTILS_KEEP_LOW_BITS_MAX8(4, (conf.input_divider - 1))) << 12;
	// MP_FRS
	val |= (uint32_t)(BITUTILS_KEEP_LOW_BITS_MAX8(2, (uint8_t)conf.hsck_freq_range_post_divider)) << 10;
	// LFBREN
	if (conf.lower_freq_bound_removal)
	{
		val |= (uint32_t)(0x01) << 9;
	}
	// Finally, MP_FBD
	val |= (uint32_t)(BITUTILS_KEEP_LOW_BITS_MAX16(9, (conf.feedback_divider_value - 1)));
}

/// Send a "long" DSI command with data (may be of length 0)
// void Toshiba_TC358870_DSI_Write_Cmd_Long(uint8_t cmd, uint16_t len, uint8_t * data);

void Toshiba_TC358870_Clear_HDMI_Sync_Change_Int()
{
	Toshiba_TC358870_I2C_Write8(TC_REG_MISC_INT, TC_REG_MISC_INT_SYNC_CHG_BITMASK);
	Toshiba_TC358870_I2C_Write16(TC_REG_INT_STATUS, TC_REG_INT_STATUS_HDMI_INT_BITMASK);
}

void Toshiba_TC358870_Enable_Video_TX()
{
	uint8_t data;
	if (TOSHIBA_TC358770_OK != Toshiba_TC358870_I2C_Read8(TC_REG_CONFIG_CONTROL_0, &data))
	{
		WriteLn("TC358770: Could not read config control reg 0");
		return;
	}
	// Enable Video TX0 and TX1
	data |= BITUTILS_BIT(0) | BITUTILS_BIT(1);
	if (TOSHIBA_TC358770_OK != Toshiba_TC358870_I2C_Write8(TC_REG_CONFIG_CONTROL_0, data))
	{
		WriteLn("TC358770: Could not write config control reg 0");
		return;
	}
	// Switch clock source to HDMI pixel clock
	if (TOSHIBA_TC358770_OK != Toshiba_TC358870_I2C_Write8(TC_REG_CONFIG_CONTROL_1, 0x0))
	{
		WriteLn("TC358770: Could not write config control reg 1");
		return;
	}
}
void Toshiba_TC358870_Disable_Video_TX()
{
	uint8_t data;
	if (TOSHIBA_TC358770_OK != Toshiba_TC358870_I2C_Read8(TC_REG_CONFIG_CONTROL_0, &data))
	{
		WriteLn("TC358770: Could not read config control reg 0");
		return;
	}
	// Disable Video TX0 and TX1
	data &= ~((uint8_t)(BITUTILS_BIT(0) | BITUTILS_BIT(1)));
	if (TOSHIBA_TC358770_OK != Toshiba_TC358870_I2C_Write8(TC_REG_CONFIG_CONTROL_0, data))
	{
		WriteLn("TC358770: Could not write config control reg 0");
		return;
	}
	// Switch clock source back to reference clock
	if (TOSHIBA_TC358770_OK != Toshiba_TC358870_I2C_Write8(TC_REG_CONFIG_CONTROL_1, BITUTILS_BIT(3)))
	{
		WriteLn("TC358770: Could not write config control reg 1");
		return;
	}
}

void Toshiba_TC358870_Enable_HDMI_Sync_Status_Interrupts(void)
{
	// Clear out the interrupt flags.
	Toshiba_TC358870_I2C_Write8(TC_REG_MISC_INT, 0xFF);
	// Unmask only the sync status change bit.
	Toshiba_TC358870_I2C_Write8(TC_REG_MISC_INT_MASK, ~TC_REG_MISC_INT_SYNC_CHG_BITMASK);

	// Write 1 to clear all the non-reserved bits (0-11, excluding 6)
	Toshiba_TC358870_I2C_Write16(TC_REG_INT_STATUS, TC_REG_INT_STATUS_NONRESERVED_BITS);
	// Top level: mask all non-reserved bits, unmasking the HDMI bit.
	Toshiba_TC358870_I2C_Write16(TC_REG_INT_MASK,
	                             TC_REG_INT_STATUS_NONRESERVED_BITS & (~TC_REG_INT_STATUS_HDMI_INT_BITMASK));
}

void Toshiba_TC358870_Disable_All_Interrupts(void)
{
	// Top level: mask all non-reserved bits.
	Toshiba_TC358870_I2C_Write16(TC_REG_INT_MASK, TC_REG_INT_STATUS_NONRESERVED_BITS);
	// Mask all interrupts at the lower level as well.
	Toshiba_TC358870_I2C_Write8(TC_REG_MISC_INT_MASK, 0xff);

	// Clear out the interrupt flags.

	// low level
	Toshiba_TC358870_I2C_Write8(TC_REG_MISC_INT, 0xFF);
	// top level
	// Write 1 to clear all the non-reserved bits (0-11, excluding 6)
	Toshiba_TC358870_I2C_Write16(TC_REG_INT_STATUS, TC_REG_INT_STATUS_NONRESERVED_BITS);
}

#endif  // SVR_HAVE_TOSHIBA_TC358870

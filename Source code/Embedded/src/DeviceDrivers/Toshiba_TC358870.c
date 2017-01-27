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
#include <stdio.h>

#define HDMI_VERBOSE

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
	typedef uint16_t T;
	uint8_t buf[sizeof(T)];
	TC358870_Op_Status_t status = Toshiba_TC358870_I2C_Read_Impl(reg, buf, sizeof(T));
	*val = (((T)(buf[1])) << CHAR_BIT) | ((T)(buf[0]));
	return status;
}

TC358870_Op_Status_t Toshiba_TC358870_I2C_Read32(TC358870_Reg_t reg, uint32_t* val)
{
	typedef uint32_t T;
	uint8_t buf[sizeof(T)];
	TC358870_Op_Status_t status = Toshiba_TC358870_I2C_Read_Impl(reg, buf, sizeof(T));
	*val = (((T)buf[3]) << (3 * CHAR_BIT)) |  //< byte 3
	       (((T)buf[2]) << (2 * CHAR_BIT)) |  //< byte 2
	       (((T)buf[1]) << CHAR_BIT) |        //< byte 1
	       ((T)buf[0]);                       //< LSB
	return status;
}

static const TC358870_DSITX_Config_t* g_tc358870_active_config = NULL;

// Select register addresses

static const TC358870_Reg_t TC_REG_DSITX1_OFFSET = 0x0200;  //< add to any DSITX0 register address
static const TC358870_Reg_t TC_REG_SYS_CONTROL = 0x0002;
static const TC358870_Reg_t TC_REG_CONFIG_CONTROL_0 = 0x0004;
static const TC358870_Reg_t TC_REG_CONFIG_CONTROL_1 = 0x0006;
static const TC358870_Reg_t TC_REG_INT_STATUS = 0x0014;
static const TC358870_Reg_t TC_REG_INT_MASK = 0x0016;
static const TC358870_Reg_t TC_REG_MODE_CONFIG = 0x0110;
static const TC358870_Reg_t TC_REG_DSITX_START = 0x011C;
static const TC358870_Reg_t TC_REG_DCSCMD_Q = 0x0504;
static const TC358870_Reg_t TC_REG_MISC_INT = 0x850B;
static const TC358870_Reg_t TC_REG_MISC_INT_MASK = 0x851B;
static const TC358870_Reg_t TC_REG_SYS_STATUS = 0x8520;
static const TC358870_Reg_t TC_REG_HV_CLEAR = 0x8593;
static const TC358870_Reg_t TC_REG_IN_HSize = 0x858e;
static const TC358870_Reg_t TC_REG_DE_HSize = 0x8582;
static const TC358870_Reg_t TC_REG_IN_VSize = 0x8590;
static const TC358870_Reg_t TC_REG_DE_VSize = 0x858C;
#if 0
	TC_REG_SYS_STATUS_HAVE_VIDEO_MASK = BITUTILS_BIT(7) | BITUTILS_BIT(3) /* PHY DE detect */ | BITUTILS_BIT(2) /* PHY PLL lock */ | BITUTILS_BIT(1) /* TMDS input amplitude */ | BITUTILS_BIT(0) /* DDC_Power input */
#endif

#define TC_REG_INT_STATUS_HDMI_INT_BITMASK BITUTILS_BIT(9)
#define TC_REG_INT_STATUS_NONRESERVED_BITS (BITUTILS_GET_SET_LOW_BITS_MAX16(12) & (~BITUTILS_BIT(6)))
#define TC_REG_MISC_INT_SYNC_CHG_BITMASK BITUTILS_BIT(1)
static const uint8_t TC_REG_SYS_STATUS_HAVE_VIDEO_BIT_MASK = BITUTILS_BIT(7);

#define TC_IF_ERR_RETURN_VAL(STATUS, RETVAL) \
	do                                       \
	{                                        \
		if (STATUS != TOSHIBA_TC358770_OK)   \
		{                                    \
			return RETVAL;                   \
		}                                    \
	} while (0)

TC358870_Op_Status_t Toshiba_TC358870_I2C_Write8_BothDSITX(TC358870_Reg_t reg, uint8_t val)
{
	TC358870_Op_Status_t status1 = Toshiba_TC358870_I2C_Write8(reg, val);
	TC358870_Op_Status_t status2 = Toshiba_TC358870_I2C_Write8(reg + TC_REG_DSITX1_OFFSET, val);
	return min(status1, status2);
}

TC358870_Op_Status_t Toshiba_TC358870_I2C_Write16_BothDSITX(TC358870_Reg_t reg, uint16_t val)
{
	TC358870_Op_Status_t status1 = Toshiba_TC358870_I2C_Write16(reg, val);
	TC358870_Op_Status_t status2 = Toshiba_TC358870_I2C_Write16(reg + TC_REG_DSITX1_OFFSET, val);
	return min(status1, status2);
}

TC358870_Op_Status_t Toshiba_TC358870_I2C_Write32_BothDSITX(TC358870_Reg_t reg, uint32_t val)
{
	TC358870_Op_Status_t status1 = Toshiba_TC358870_I2C_Write32(reg, val);
	TC358870_Op_Status_t status2 = Toshiba_TC358870_I2C_Write32(reg + TC_REG_DSITX1_OFFSET, val);
	return min(status1, status2);
}

static inline void Toshiba_TC358770_Start_Reset(void) { ioport_set_pin_low(TC358870_Reset_Pin); }
static inline void Toshiba_TC358770_End_Reset(void) { ioport_set_pin_high(TC358870_Reset_Pin); }
static uint8_t s_tc358870_init_count = 0;

static void Toshiba_TC358870_Base_Init_Impl(bool firstTime, bool hardResetToshiba, bool fullSoftwareReset,
                                            bool shouldStartInterrupts)
{
#ifdef HDMI_VERBOSE
	WriteLn("Toshiba_TC358870_Init: Start");
#endif
#ifndef SVR_VIDEO_INPUT_POLL_INTERVAL
	// disable interrupts
	Toshiba_TC358870_MCU_Ints_Suspend();
#endif  // !SVR_VIDEO_INPUT_POLL_INTERVAL
	if (firstTime)
	{
		twi_master_options_t opt = {
		    .speed = TC358870_TWI_SPEED,  //< used in twi_master_setup with the TWI_BAUD macro and
		                                  // the system clock to compute the .speed_reg member.
		    .chip = TC358870_ADDR};
		/// twi_master_setup sets .speed_reg for you and starts appropriate clocks before calling twi_master_init.
		twi_master_setup(TC358870_TWI_PORT, &opt);
	}

	WriteLn("Toshiba_TC358870_Init: Waiting for 5V power rail");
	while (!ioport_get_value(ANA_PWR_IN))
	{
		delay_us(50);
	}
	svr_yield_ms(10);
	WriteLn("Toshiba_TC358870_Init: Waiting for low-voltage power rail");
	while (!ioport_get_value(TC358870_PWR_GOOD))
	{
		delay_us(50);
	}

	svr_yield_ms(100);
	if (hardResetToshiba)
	{
#ifdef HDMI_VERBOSE
		WriteLn("Toshiba_TC358870_Init: Resetting TC358770");
#endif
		Toshiba_TC358770_Start_Reset();
	}
#ifdef HDMI_VERBOSE
	WriteLn("Toshiba_TC358870_Init: Resetting panel");
#endif
	g_tc358870PanelFuncs.startReset();

	svr_yield_ms(50);

	if (hardResetToshiba)
	{
		Toshiba_TC358770_End_Reset();
	}

	svr_yield_ms(50);

	// Dennis Yeh 2016/03/14 : for TC358870
	uint16_t tc_data;
	/// dummy read?
	Toshiba_TC358870_I2C_Read16(0x0000, &tc_data);

	if (fullSoftwareReset)
	{
		WriteLn("Toshiba_TC358870_Init: Full software reset");
		Toshiba_TC358870_SW_Reset();
	}
	else
	{
		WriteLn("Toshiba_TC358870_Init: DSI-TX software reset");
		Toshiba_TC358870_DSITX_SW_Reset();
	}
	svr_yield_ms(10);
	g_tc358870PanelFuncs.endReset();
	svr_yield_ms(10);
	Toshiba_TC358770_Setup_TX_Parameterized(g_tc358870_active_config);
	svr_yield_ms(50);
	g_tc358870PanelFuncs.sendInitCommands();
	Toshiba_TC358870_Configure_Splitter();
	Toshiba_TC358870_HDMI_Setup();

	s_tc358870_init_count++;
// Only one of polling or interrupts
#ifndef SVR_VIDEO_INPUT_POLL_INTERVAL
	if (firstTime)
	{
		if (shouldStartInterrupts)
		{
#ifdef HDMI_VERBOSE
			WriteLn("Toshiba_TC358870_Init: Initializing MCU interrupt handler");
#endif
			Toshiba_TC358870_MCU_Ints_Init();
		}
	}
	else
	{
		Toshiba_TC358870_Clear_HDMI_Sync_Change_Int();
		Toshiba_TC358870_MCU_Ints_Clear_Flag();
		if (shouldStartInterrupts)
		{
// not our first go-round, we'll just resume ints here.
#ifdef HDMI_VERBOSE
			WriteLn("Toshiba_TC358870_Init: Re-enabling MCU interrupt handler");
#endif
			Toshiba_TC358870_MCU_Ints_Resume();
		}
	}
#endif  // !SVR_VIDEO_INPUT_POLL_INTERVAL
	WriteLn("Toshiba_TC358870_Init: End");
}

void Toshiba_TC358870_Base_Init(void)
{
	static bool repeat = false;
	bool firstTime = false;
	if (!repeat)
	{
		// this is the first time we've been in here!
		firstTime = true;
		repeat = true;
		// grab the default config.
		g_tc358870_active_config = g_TC358870_DSITX_Config_Default;
	}
	// hard reset, full SW reset, should start interrupts
	Toshiba_TC358870_Base_Init_Impl(firstTime, true, true, true);
}

static inline void tc_Turn_On_LD17(void) { /*ioport_set_pin_high(MCU_LED_R);*/}
static inline void tc_Turn_Off_LD17(void) { /*ioport_set_pin_low(MCU_LED_R);*/}
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

bool Toshiba_TC358870_Have_Video_Sync_Detailed(uint8_t* val)
{
	if (TOSHIBA_TC358770_OK != Toshiba_TC358870_I2C_Read8(TC_REG_SYS_STATUS, val))
	{
		return false;
	}
	/// @todo - should we check the lower nybble too (PHY DE detect, PHY PLL, TMDS input amplitude, and DDC power
	/// input)?
	/// Bit 7 is input video sync - bits 6, 5, and 4 are unimportant to the task at hand, so equality to 0x9f is not
	/// quite right.
	return bitUtils_checkBit(*val, TC_REG_SYS_STATUS_HAVE_VIDEO_BIT_MASK);
}

TC358870_InputMeasurements_t Toshiba_TC358770_Get_Input_Measurements()
{
	TC358870_InputMeasurements_t ret = {.opStatus = TOSHIBA_TC358770_OK,
	                                    .reg8405 = 0,
	                                    .reg8406 = 0,
	                                    .horizTotal = 0,
	                                    .horizActive = 0,
	                                    .vertTotal = 0,
	                                    .vertActive = 0};
	/// Clear the measurements
	ret.opStatus = Toshiba_TC358870_I2C_Write8(TC_REG_HV_CLEAR, 0x33);
	TC_IF_ERR_RETURN_VAL(ret.opStatus, ret);
	if (ret.opStatus != TOSHIBA_TC358770_OK)
	{
		return ret;
	}
	/// Wait 2 vertical period at a minimum - 11 is safer
	svr_yield_ms(11 * TC358870_VSYNC_PERIOD_MS);

	/// Retrieve undocumented data.
	ret.opStatus = Toshiba_TC358870_I2C_Read8(0x8405, &ret.reg8405);
	TC_IF_ERR_RETURN_VAL(ret.opStatus, ret);
	ret.opStatus = Toshiba_TC358870_I2C_Read8(0x8406, &ret.reg8406);
	TC_IF_ERR_RETURN_VAL(ret.opStatus, ret);

	/// Retrieve documented parameters
	ret.opStatus = Toshiba_TC358870_I2C_Read16(TC_REG_IN_HSize, &ret.horizTotal);
	TC_IF_ERR_RETURN_VAL(ret.opStatus, ret);
	ret.opStatus = Toshiba_TC358870_I2C_Read16(TC_REG_DE_HSize, &ret.horizActive);
	TC_IF_ERR_RETURN_VAL(ret.opStatus, ret);
	ret.opStatus = Toshiba_TC358870_I2C_Read16(TC_REG_IN_VSize, &ret.vertTotal);
	TC_IF_ERR_RETURN_VAL(ret.opStatus, ret);
	ret.opStatus = Toshiba_TC358870_I2C_Read16(TC_REG_DE_VSize, &ret.vertActive);
	return ret;
}

TC358870_InputMeasurements_t Toshiba_TC358770_Print_Input_Measurements()
{
	char msg[50];
	TC358870_InputMeasurements_t meas = Toshiba_TC358770_Get_Input_Measurements();
	if (meas.opStatus == TOSHIBA_TC358770_OK)
	{
		sprintf(msg, "[8405]: %04" PRIx8 "  [8406] %04" PRIx8, meas.reg8405, meas.reg8406);
		WriteLn(msg);
		sprintf(msg, "Horiz: %" PRId16 "  active %" PRId16, meas.horizTotal, meas.horizActive);
		WriteLn(msg);
		sprintf(msg, "Vert: %" PRId16 "  active %" PRId16, meas.vertTotal, meas.vertActive);
		WriteLn(msg);
	}
	else
	{
		sprintf(msg, "Failed to get measurements: %" PRId8, meas.opStatus);
		WriteLn(msg);
	}
	return meas;
}

bool Toshiba_TC358770_Update_DSITX_Config_And_Reinit(const TC358870_DSITX_Config_t* newConfig)
{
	if (newConfig == g_tc358870_active_config)
	{
#ifdef HDMI_VERBOSE
		WriteLn("Toshiba_TC358770_Update_DSITX_Config_And_Reinit: Given the already-active config, nothing to do");
#endif
		return false;
	}
	WriteLn(
	    "Toshiba_TC358770_Update_DSITX_Config_And_Reinit: Given a different config, setting as active and "
	    "re-initializing DSITX");
	g_tc358870_active_config = newConfig;
	// Not first time, don't hard-reset toshiba chip, don't do a full software reset, don't start interrupts.
	Toshiba_TC358870_Base_Init_Impl(false, false, false, false);
	return true;
}
const TC358870_DSITX_Config_t* Toshiba_TC358770_Get_DSITX_Config() { return g_tc358870_active_config; }
/// Perform a software reset of the HDMI receiver portion of the chip.
void Toshiba_TC358870_HDMI_SW_Reset()
{
	Toshiba_TC358870_I2C_Write16(TC_REG_SYS_CONTROL, BITUTILS_BIT(8));
	Toshiba_TC358870_I2C_Write16(TC_REG_SYS_CONTROL, 0);
	svr_yield_ms(150);
}

void Toshiba_TC358870_DSITX_SW_Reset()
{
	Toshiba_TC358870_I2C_Write16(TC_REG_SYS_CONTROL, BITUTILS_BIT(9));
	Toshiba_TC358870_I2C_Write16(TC_REG_SYS_CONTROL, 0);

	svr_yield_ms(150);
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
	Toshiba_TC358870_I2C_Write16(TC_REG_DCSCMD_Q, (((uint16_t)param) << (sizeof(cmd) * CHAR_BIT)) | ((uint16_t)cmd));
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
	Toshiba_TC358870_I2C_Write8(TC_REG_MISC_INT, 0xff /*TC_REG_MISC_INT_SYNC_CHG_BITMASK*/);
	Toshiba_TC358870_I2C_Write16(TC_REG_INT_STATUS, UINT16_C(0x0fbf) /*TC_REG_INT_STATUS_HDMI_INT_BITMASK*/);
}

void Toshiba_TC358870_Enable_Video_TX()
{
	tc_Turn_On_LD17();

#if 0
	uint16_t data;
	if (TOSHIBA_TC358770_OK != Toshiba_TC358870_I2C_Read16(TC_REG_CONFIG_CONTROL_0, &data))
	{
		WriteLn("TC358770: Could not read config control reg 0");
		return;
	}
	// Enable Video TX0 and TX1
	data |= BITUTILS_BIT(0) | BITUTILS_BIT(1);
#else
	const uint16_t data = 0x0C37;  // more than just bits 0 and 1 - fixed value used by libhdk2
#endif
	if (TOSHIBA_TC358770_OK != Toshiba_TC358870_I2C_Write16(TC_REG_CONFIG_CONTROL_0, data))
	{
		WriteLn("TC358770: Could not write config control reg 0");
		return;
	}
	// Switch clock source to HDMI pixel clock
	if (TOSHIBA_TC358770_OK != Toshiba_TC358870_I2C_Write16(TC_REG_CONFIG_CONTROL_1, 0x0))
	{
		WriteLn("TC358770: Could not write config control reg 1");
		return;
	}
	Toshiba_TC358870_I2C_Write32_BothDSITX(TC_REG_MODE_CONFIG, UINT32_C(0x00000006));
}
void Toshiba_TC358870_Disable_Video_TX()
{
	tc_Turn_Off_LD17();

#if 0
	Toshiba_TC358870_I2C_Write32_BothDSITX(TC_REG_DSITX_START, 0);  // DSITX_START goes to 0 now.
#endif

	uint16_t data;
#if 0
	if (TOSHIBA_TC358770_OK != Toshiba_TC358870_I2C_Read16(TC_REG_CONFIG_CONTROL_0, &data))
	{
		WriteLn("TC358770: Could not read config control reg 0");
		return;
	}
	// Disable Video TX0 and TX1
	data &= ~((uint16_t)(BITUTILS_BIT(0) | BITUTILS_BIT(1)));
#else
	data = BITUTILS_BIT(2) /*0x0004*/;
#endif
	if (TOSHIBA_TC358770_OK != Toshiba_TC358870_I2C_Write16(TC_REG_CONFIG_CONTROL_0, data))
	{
		WriteLn("TC358770: Could not write config control reg 0");
		return;
	}

	// Switch clock source back to reference clock
	if (TOSHIBA_TC358770_OK != Toshiba_TC358870_I2C_Write16(TC_REG_CONFIG_CONTROL_1, BITUTILS_BIT(3)))
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

static inline void tc358870_Setup_Single_DSITX(TC358870_Reg_t base, const TC358870_DSITX_Config_t* params)
{
	Toshiba_TC358870_I2C_Write32(base + 0x0108, 0x00000001);             // DSI_TX_CLKEN
	Toshiba_TC358870_I2C_Write32(base + 0x010C, 0x00000001);             // DSI_TX_CLKSEL
	Toshiba_TC358870_I2C_Write32(base + 0x02A0, 0x00000001);             // MIPI_PLL_CONTROL
	Toshiba_TC358870_I2C_Write32(base + 0x02AC, params->MIPI_PLL_CONF);  // MIPI_PLL_CNF
	delay_ms(2);

	Toshiba_TC358870_I2C_Write32(base + 0x02A0, 0x00000003);  // MIPI_PLL_CONTROL
	Toshiba_TC358870_I2C_Write32(base + 0x0118, 0x00000014);  // LANE_ENABLE
	delay_ms(1);
	Toshiba_TC358870_I2C_Write32(base + 0x0120, 0x00001770);         // LINE_INIT_COUNT
	Toshiba_TC358870_I2C_Write32(base + 0x0124, 0x00000000);         // HSTX_TO_COUNT
	Toshiba_TC358870_I2C_Write32(base + 0x0128, 0x00000101);         // FUNC_ENABLE
	Toshiba_TC358870_I2C_Write32(base + 0x0130, 0x00010000);         // DSI_TATO_COUNT
	Toshiba_TC358870_I2C_Write32(base + 0x0134, 0x00005000);         // DSI_PRESP_BTA_COUNT
	Toshiba_TC358870_I2C_Write32(base + 0x0138, 0x00010000);         // DSI_PRESP_LPR_COUNT
	Toshiba_TC358870_I2C_Write32(base + 0x013C, 0x00010000);         // DSI_PRESP_LPW_COUNT
	Toshiba_TC358870_I2C_Write32(base + 0x0140, 0x00010000);         // DSI_PRESP_HSR_COUNT
	Toshiba_TC358870_I2C_Write32(base + 0x0144, 0x00010000);         // DSI_PRESP_HSW_COUNT
	Toshiba_TC358870_I2C_Write32(base + 0x0148, 0x00001000);         // DSI_PR_TO_COUNT
	Toshiba_TC358870_I2C_Write32(base + 0x014C, 0x00010000);         // DSI_LRX-H_TO_COUNT
	Toshiba_TC358870_I2C_Write32(base + 0x0150, params->FUNC_MODE);  // FUNC_MODE

	Toshiba_TC358870_I2C_Write32(base + 0x0154, 0x00000001);                       // DSI_RX_VC_ENABLE
	Toshiba_TC358870_I2C_Write32(base + 0x0158, 0x000000C8);                       // IND_TO_COUNT
	Toshiba_TC358870_I2C_Write32(base + 0x0168, 0x0000002A);                       // DSI_HSYNC_STOP_COUNT
	Toshiba_TC358870_I2C_Write32(base + 0x0170, params->APF_VDELAYCNT);            // APF_VDELAYCNT
	Toshiba_TC358870_I2C_Write32(base + 0x017C, 0x00000081);                       // DSI_TX_MODE
	Toshiba_TC358870_I2C_Write32(base + 0x018C, 0x00000001);                       // DSI_HSYNC_WIDTH
	Toshiba_TC358870_I2C_Write32(base + 0x0190, params->DSI_HBPR);                 // DSI_HBPR
	Toshiba_TC358870_I2C_Write32(base + 0x01A4, 0x00000000);                       // DSI_RX_STATE_INT_MASK
	Toshiba_TC358870_I2C_Write32(base + 0x01C0, 0x00000015);                       // DSI_LPRX_THRESH_COUNT
	Toshiba_TC358870_I2C_Write32(base + 0x0214, 0x00000000);                       // APP_SIDE_ERR_INT_MASK
	Toshiba_TC358870_I2C_Write32(base + 0x021C, 0x00000080);                       // DSI_RX_ERR_INT_MASK
	Toshiba_TC358870_I2C_Write32(base + 0x0224, 0x00000000);                       // DSI_LPTX_INT_MASK
	Toshiba_TC358870_I2C_Write32(base + 0x0254, 0x00000006);                       // LPTXTIMECNT
	Toshiba_TC358870_I2C_Write32(base + 0x0258, params->PPI_DPHY_TCLK_HEADERCNT);  // TCLK_HEADERCNT
	Toshiba_TC358870_I2C_Write32(base + 0x025C, params->PPI_DPHY_TCLK_TRAILCNT);   // TCLK_TRAILCNT
	Toshiba_TC358870_I2C_Write32(base + 0x0260, params->PPI_DPHY_THS_HEADERCNT);   // THS_HEADERCNT
	Toshiba_TC358870_I2C_Write32(base + 0x0264, params->PPI_DPHY_TWAKEUPCNT);      // TWAKEUPCNT
	Toshiba_TC358870_I2C_Write32(base + 0x0268, params->PPI_DPHY_TCLK_POSTCNT);    // TCLK_POSTCNT
	Toshiba_TC358870_I2C_Write32(base + 0x026C, params->PPI_DPHY_THSTRAILCNT);     // THS_TRAILCNT
	Toshiba_TC358870_I2C_Write32(base + 0x0270, 0x00000020);                       // HSTXVREGCNT
	Toshiba_TC358870_I2C_Write32(base + 0x0274, 0x0000001F);                       // HSTXVREGEN
	Toshiba_TC358870_I2C_Write32(base + 0x0278, params->PPI_DSI_BTA_COUNT);        // BTA_COUNT
	Toshiba_TC358870_I2C_Write32(base + 0x027C, 0x00000002);                       // DPHY_TX ADJUST
	Toshiba_TC358870_I2C_Write32(base + 0x011C, 0x00000001);                       // DSITX_START
}

void Toshiba_TC358770_Setup_TX_Parameterized(const TC358870_DSITX_Config_t* params)
{
	tc358870_Setup_Single_DSITX(0x0000, params);
	tc358870_Setup_Single_DSITX(TC_REG_DSITX1_OFFSET, params);
	// Command Transmission Before Video Start
	Toshiba_TC358870_I2C_Write32_BothDSITX(0x0110,
	                                       0x00000016);  // MODE_CONFIG - high speed mode DSI commands, hsync-, vsync-
	Toshiba_TC358870_I2C_Write16(0x0500, 0x0004);        // CMD_SEL - send dcs cmds to both tx
}

#endif  // SVR_HAVE_TOSHIBA_TC358870

/*
 * Toshiba_TC358870.h
 *
 * Requires definitions of:
 * - TC358870_Reset_Pin
 * - TC358870_ADDR (I2C address, either 0x0f or 0x1f depending on reset state of INT)
 * - TC358870_TWI_PORT (to something like (&TWIE) )
 * - TC358870_TWI_SPEED (100000 or 400kHz or 2MHz)
 * - TC358870_ADDR_SEL_INT (an IOPORT_CREATE_PIN for where you connected INT/addr_select)
 * - TC358870_PWR_GOOD (an IOPORT_CREATE_PIN that goes high when the TC power rail is good and the chip power-on
 * sequence can continue)
 *
 * Created: 7/21/2016 8:29:17 AM
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

#ifndef TOSHIBA_TC358870_H_
#define TOSHIBA_TC358870_H_

/*
    HDMI IF Functions

    Coretronic
    FC Tu
*/

#include <stdbool.h>
#include <stdint.h>

typedef enum {
	TOSHIBA_TC358770_OK = 0,
	TOSHIBA_TC358770_ERR_IO = -1,
	TOSHIBA_TC358770_ERR_BUFFER = -7,
	TOSHIBA_TC358770_ERR_BUS_STATE = -5,
	TOSHIBA_TC358770_ERR_INVALID_ARG = -8
} TC358870_Op_Status_t;

typedef uint16_t TC358870_Reg_t;

typedef void (*TC358870_PanelFunction)(void);

typedef struct TC358870_PanelFunctions
{
	TC358870_PanelFunction startReset;
	TC358870_PanelFunction endReset;
	TC358870_PanelFunction sendInitCommands;
} TC358870_PanelFunctions_t;

/// Must define this in your panel-specific code.
extern const TC358870_PanelFunctions_t g_tc358870PanelFuncs;

#ifndef TC358870_VSYNC_PERIOD_MS
/// Default to 90Hz (ceil(1./90.))
#define TC358870_VSYNC_PERIOD_MS 12
#endif

/// Sets up the i2c bus, does an initial read, then resets the chip and the panel.
/// Formerly called the the "black-box" PowerOnSeq libhdk20 function that, among other
/// things, eventually calls the other libhdk20 function TC358870_Init_Receive_HDMI_Signal
void Toshiba_TC358870_Base_Init(void);

/// Calls Toshiba_TC358870_Base_Init() only if it hasn't been called before - returns false if it doesn't actually call
/// init.
bool Toshiba_TC358870_Init_Once(void);

/// Wraps libhdk20 function TC358870_Init_Receive_HDMI_Signal
void Toshiba_TC358870_Init_Receiver(void);

/// Returns the number of times that Toshiba_TC358870_Init() has been called.
uint8_t Toshiba_TC358870_Get_Init_Count(void);

/// Triggers a chip reset of the TC358870 - does not do any initialization afterwards
/// (Can't call it just Toshiba_TC358870_Reset, libhdk20 exports a symbol by that name.)
void Toshiba_TC358870_Trigger_Reset(void);

/// Checks the status register to see if the toshiba chip has stable video sync.
bool Toshiba_TC358870_Have_Video_Sync(void);

/// Checks the status register to see if the toshiba chip has stable video sync.
bool Toshiba_TC358870_Have_Video_Sync_Detailed(uint8_t* val);

typedef struct TC358870_InputMeasurements
{
	TC358870_Op_Status_t opStatus;
	/// Unknown status register
	uint8_t reg8405;
	/// Unknown status register
	uint8_t reg8406;
	/// Total dots per horizontal line (including blanking - sync and porches)
	uint16_t horizTotal;
	/// Active dots per horizontal line (horizontal resolution)
	uint16_t horizActive;
	/// Height in lines (total - including blanking - sync and porches)
	uint16_t vertTotal;
	/// Active height in lines (vertical resolution)
	uint16_t vertActive;
} TC358870_InputMeasurements_t;

/// Note: Output will only be valid if we have video sync.
TC358870_InputMeasurements_t Toshiba_TC358770_Get_Input_Measurements(void);

TC358870_InputMeasurements_t Toshiba_TC358770_Print_Input_Measurements(void);

/// Members of this struct are used to populate the registers of the same name on the two DSI-TX register sets.
typedef struct TC358870_DSITX_Config
{
	uint32_t MIPI_PLL_CONF;  //< DSI-TX0 register at 0x02ac

	// uint32_t LANE_ENABLE; //< DSI-TX0 register at 0x0118
	// uint32_t LINE_INIT_COUNT; //< DSI-TX0 register at 0x0120
	// uint32_t HSTX_TO_COUNT; //< DSI-TX0 register at 0x0124
	// uint32_t FUNC_ENABLE; //< DSI-TX0 register at 0x0128
	// uint32_t ***UNKNOWN***; //< DSI-TX0 register at 0x0130
	// uint32_t ***UNKNOWN***; //< DSI-TX0 register at 0x0134
	// uint32_t ***UNKNOWN***; //< DSI-TX0 register at 0x0138
	// uint32_t DSI_PRESP_LPW_COUNT; //< DSI-TX0 register at 0x013c
	// uint32_t DSI_PRESP_HSR_COUNT; //< DSI-TX0 register at 0x0140
	// uint32_t DSI_PRESP_HSW_COUNT; //< DSI-TX0 register at 0x0144
	// uint32_t ***UNKNOWN***; //< DSI-TX0 register at 0x0148
	// uint32_t ***UNKNOWN***; //< DSI-TX0 register at 0x014c

	uint32_t FUNC_MODE;  //< DSI-TX0 register at 0x0150

	// uint32_t DSIRX_VC_ENABLE; //< DSI-TX0 register at 0x0154
	// uint32_t IND_TO_COUNT; //< DSI-TX0 register at 0x0158
	// uint32_t ***UNKNOWN***; //< DSI-TX0 register at 0x0168

	uint32_t APF_VDELAYCNT;  //< DSI-TX0 register at 0x0170

	// uint32_t DSITX_MODE; //< DSI-TX0 register at 0x017c
	// uint32_t DSI_HSYNC_WIDTH; //< DSI-TX0 register at 0x018c
	uint32_t DSI_HBPR;  //< DSI-TX0 register at 0x0190

	// uint32_t DSI_RX_STATE_INT_MASK; //< DSI-TX0 register at 0x01a4
	// uint32_t DSI_LPRX_THRESH_COUNT; //< DSI-TX0 register at 0x01c0
	// uint32_t APP_SIDE_ERR_INT_MASK; //< DSI-TX0 register at 0x0214
	// uint32_t DSI_RX_ERR_INT_MASK; //< DSI-TX0 register at 0x021c
	// uint32_t DSI_LPTX_INT_MASK; //< DSI-TX0 register at 0x0224
	// uint32_t PPI_DPHY_LPTXTIMECNT; //< DSI-TX0 register at 0x0254

	uint32_t PPI_DPHY_TCLK_HEADERCNT;  //< DSI-TX0 register at 0x0258
	uint32_t PPI_DPHY_TCLK_TRAILCNT;   //< DSI-TX0 register at 0x025c
	uint32_t PPI_DPHY_THS_HEADERCNT;   //< DSI-TX0 register at 0x0260
	uint32_t PPI_DPHY_TWAKEUPCNT;      //< DSI-TX0 register at 0x0264
	uint32_t PPI_DPHY_TCLK_POSTCNT;    //< DSI-TX0 register at 0x0268
	uint32_t PPI_DPHY_THSTRAILCNT;     //< DSI-TX0 register at 0x026c

	// uint32_t PPI_DPHY_HSTXVREGCNT; //< DSI-TX0 register at 0x0270
	// uint32_t PPI_DPHY_HSTXVREGEN; //< DSI-TX0 register at 0x0274

	uint32_t PPI_DSI_BTA_COUNT;  //< DSI-TX0 register at 0x0278

	// uint32_t PPI_DPHYTX_ADJUST; //< DSI-TX0 register at 0x027c

	/// After both DSI-TX are setup...
	// uint32_t MODE_CONFIG; //< DSI-TX0 register at 0x0110
} TC358870_DSITX_Config_t;

extern const TC358870_DSITX_Config_t TC358870_DSITX_Config_60hz_2160_1200;
extern const TC358870_DSITX_Config_t TC358870_DSITX_Config_90hz_2160_1200;

/// returns true if the config was different and thus the chip was re-initialized.
bool Toshiba_TC358770_Update_DSITX_Config_And_Reinit(const TC358870_DSITX_Config_t* newConfig);
const TC358870_DSITX_Config_t* Toshiba_TC358770_Get_DSITX_Config(void);

/// Writes an 8-bit byte to the given register over I2C.
/// Does wait for the bus to become available, but returns other errors from I2C code as-is.
TC358870_Op_Status_t Toshiba_TC358870_I2C_Write8(TC358870_Reg_t reg, uint8_t val);

/// Writes 16 bits starting at the given register address over I2C.
/// Does wait for the bus to become available, but returns other errors from I2C code as-is.
TC358870_Op_Status_t Toshiba_TC358870_I2C_Write16(TC358870_Reg_t reg, uint16_t val);

/// Writes 32 bits starting at the given register address over I2C.
/// Does wait for the bus to become available, but returns other errors from I2C code as-is.
TC358870_Op_Status_t Toshiba_TC358870_I2C_Write32(TC358870_Reg_t reg, uint32_t val);

/// Like Write8, but expects a register in the DSI-TX0 range (0x0100-0x02ff) - it will write both to that register and
/// its DSI-TX1 counterpart in the range (0x0300-0x04FF)
/// Return value is the minimum of the two independent return values.
TC358870_Op_Status_t Toshiba_TC358870_I2C_Write8_BothDSITX(TC358870_Reg_t reg, uint8_t val);

/// Like Write16, but expects a register in the DSI-TX0 range (0x0100-0x02ff) - it will write both to that register and
/// its DSI-TX1 counterpart in the range (0x0300-0x04FF)
/// Return value is the minimum of the two independent return values.
TC358870_Op_Status_t Toshiba_TC358870_I2C_Write16_BothDSITX(TC358870_Reg_t reg, uint16_t val);

/// Like Write32, but expects a register in the DSI-TX0 range (0x0100-0x02ff) - it will write both to that register and
/// its DSI-TX1 counterpart in the range (0x0300-0x04FF)
/// Return value is the minimum of the two independent return values.
TC358870_Op_Status_t Toshiba_TC358870_I2C_Write32_BothDSITX(TC358870_Reg_t reg, uint32_t val);

/// Reads an 8-bit byte from the given register over I2C.
TC358870_Op_Status_t Toshiba_TC358870_I2C_Read8(TC358870_Reg_t reg, uint8_t* val);

/// Reads 16 bits starting at the given register address over I2C.
TC358870_Op_Status_t Toshiba_TC358870_I2C_Read16(TC358870_Reg_t reg, uint16_t* val);

/// Reads 32 bits starting at the given register address over I2C.
TC358870_Op_Status_t Toshiba_TC358870_I2C_Read32(TC358870_Reg_t reg, uint32_t* val);

/// Send a short DSI command with no parameter.
void Toshiba_TC358870_DSI_Write_Cmd_Short(uint8_t cmd);

/// Send a short DSI command with one parameter.
void Toshiba_TC358870_DSI_Write_Cmd_Short_Param(uint8_t cmd, uint8_t param);

/// Send a "long" DSI command with data (may be of length 0)
// void Toshiba_TC358870_DSI_Write_Cmd_Long(uint8_t cmd, uint16_t len, uint8_t * data);

typedef struct Toshiba_TC358870_MIPI_PLL_Conf
{
	enum
	{
		LBW_25PCT_OF_MAX = 0x00,
		LBW_33PCT_OF_MAX = 0x01,
		LBW_50PCT_OF_MAX = 0x02,
		LBW_MAX = 0x03
	} low_bandwidth_setting;

	/// raw input divider value: max value of 9 (will have 1 subtracted then be fit into 4 bits)
	uint8_t input_divider;
	enum
	{
		FRS_500_1G = 0x00,
		FRS_250_500 = 0x01,
		FRS_125_250 = 0x02,
		FRS_62_5_125 = 0x03
	} hsck_freq_range_post_divider;
	bool lower_freq_bound_removal;

	/// feedback divider value: max value 2049 (will have 1 subtracted then be fit into 9 bits.)
	uint16_t feedback_divider_value;
} Toshiba_TC358870_MIPI_PLL_Conf_t;

void Toshiba_TC358870_Set_MIPI_PLL_Config(uint8_t output, Toshiba_TC358870_MIPI_PLL_Conf_t conf);

/*
/// approximation of TC358870_Init_Receive_HDMI_Signal in factored-out functions
void TC358870_Init_Receive_HDMI_Signal() {
  Toshiba_TC358870_SW_Reset();
  Toshiba_TC358870_Prepare_TX();
  AUO_H381DLN01_Init(0);
  Toshiba_TC358870_Configure_Splitter();
  Toshiba_TC358870_HDMI_Setup();

  // strange waiting for video sync here, then...
  Toshiba_TC358870_Enable_Video_TX();

  if (this wasn't the first time in here) {
    // this is TC358870_Reset_MIPI
    AUO_H381DLN01_Reset();

    // approximately:
    Toshiba_TC358870_Disable_Video_TX();
    set then clear bit 9 (CTxRst) of 0x0002 - software reset of DSI-TX
    delay_ms(150);
    Toshiba_TC358870_Prepare_TX();
    AUO_H381DLN01_Init(1);
    Toshiba_TC358870_Enable_Video_TX();
  }
}
*/

void Toshiba_TC358870_Clear_HDMI_Sync_Change_Int(void);
void Toshiba_TC358870_Enable_Video_TX(void);
void Toshiba_TC358870_Disable_Video_TX(void);

/// Perform a software reset of the HDMI receiver portion of the chip.
void Toshiba_TC358870_HDMI_SW_Reset(void);

/// Perform a software reset of the DSI transmitters (both DSI-TX0 and DSI-TX1) portion of the chip.
void Toshiba_TC358870_DSITX_SW_Reset(void);

/// Original first step of TC358870_Init_Receive_HDMI_Signal
void Toshiba_TC358870_SW_Reset(void);
/// This is the portion of TC358870_Init_Receive_HDMI_Signal after the software reset that preceded
/// AUO_H381DLN01_Init(0) (which would send the setup commands to the panel and sleep out, but not display on)
void Toshiba_TC358870_Prepare_TX(void);
/// This is the portion of TC358870_Init_Receive_HDMI_Signal with DSI/panel related code following AUO_H381DLN01_Init
void Toshiba_TC358870_Configure_Splitter(void);
void Toshiba_TC358870_HDMI_Setup(void);

void Toshiba_TC358870_Enable_HDMI_Sync_Status_Interrupts(void);
void Toshiba_TC358870_Disable_All_Interrupts(void);

void Toshiba_TC358770_Setup_TX_Parameterized(const TC358870_DSITX_Config_t* params);

#endif /* TOSHIBA_TC358870_H_ */
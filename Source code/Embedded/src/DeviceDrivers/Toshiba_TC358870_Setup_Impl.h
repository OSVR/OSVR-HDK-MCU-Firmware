/** @file
    @brief Header with Toshiba TC358870 setup details.

    Based on code from libhdk20, used pursuant to the Apache License, Version 2.0.

    @date 2016

*/

// Copyright 2016 OSVR and contributors.
// Copyright 2016 Dennis Yeh.
// Copyright 2016 Sensics, Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//        http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef INCLUDED_Toshiba_TC358870_Setup_Impl_h_GUID_098B0B47_6470_4AA6_589E_4B32CCC542C8
#define INCLUDED_Toshiba_TC358870_Setup_Impl_h_GUID_098B0B47_6470_4AA6_589E_4B32CCC542C8

// Internal Includes
#include "Toshiba_TC358870.h"
#include "SvrYield.h"
#include "VariantOptions.h"

// Library/third-party includes
// - none

// Standard includes
// - none

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

void Toshiba_TC358870_SW_Reset()
{
	// Software Reset

	// disable everything except auto-increment
	TC358870_i2c_Write(0x0004, 0x0004, 2);  // ConfCtl0
	TC358870_i2c_Write(0x0002, 0x3F01, 2);  // SysCtl
	TC358870_i2c_Write(0x0002, 0x0000, 2);  // SysCtl
	TC358870_i2c_Write(0x0006, 0x0008, 2);  // ConfCtl1
}

/// As used the first time the 1.01 firmware initialized the chip
#define TC_FUNC_MODE_VALUE 0x00000161
/// As used the second time the 1.01 firmware initialized the chip
//#define TC_FUNC_MODE_VALUE 0x00000160
/// as used in TFN board
//#define TC_FUNC_MODE_VALUE 0x00000141

inline static void Toshiba_TC358770_Setup_TX_TFN(void)
{
	// DSI-TX0 Transition Timing
	TC358870_i2c_Write(0x0108, 0x00000001, 4);  // DSI_TX_CLKEN
	TC358870_i2c_Write(0x010C, 0x00000001, 4);  // DSI_TX_CLKSEL
	TC358870_i2c_Write(0x02A0, 0x00000001, 4);  // MIPI_PLL_CONTROL
	TC358870_i2c_Write(0x02AC, 0x00009095, 4);  // MIPI_PLL_CNF
	delay_ms(2);

	TC358870_i2c_Write(0x02A0, 0x00000003, 4);  // MIPI_PLL_CONTROL
	TC358870_i2c_Write(0x0118, 0x00000014, 4);  // LANE_ENABLE
	delay_ms(1);
	TC358870_i2c_Write(0x0120, 0x00001770, 4);  // LINE_INIT_COUNT
	TC358870_i2c_Write(0x0124, 0x00000000, 4);  // HSTX_TO_COUNT
	TC358870_i2c_Write(0x0128, 0x00000101, 4);  // FUNC_ENABLE
	TC358870_i2c_Write(0x0130, 0x00010000, 4);  // DSI_TATO_COUNT
	TC358870_i2c_Write(0x0134, 0x00005000, 4);  // DSI_PRESP_BTA_COUNT
	TC358870_i2c_Write(0x0138, 0x00010000, 4);  // DSI_PRESP_LPR_COUNT
	TC358870_i2c_Write(0x013C, 0x00010000, 4);  // DSI_PRESP_LPW_COUNT
	TC358870_i2c_Write(0x0140, 0x00010000, 4);  // DSI_PRESP_HSR_COUNT
	TC358870_i2c_Write(0x0144, 0x00010000, 4);  // DSI_PRESP_HSW_COUNT
	TC358870_i2c_Write(0x0148, 0x00001000, 4);  // DSI_PR_TO_COUNT
	TC358870_i2c_Write(0x014C, 0x00010000, 4);  // DSI_LRX-H_TO_COUNT
	// TC358870_i2c_Write(0x0150, 0x00000141, 4);  // FUNC_MODE
	TC358870_i2c_Write(0x0150, TC_FUNC_MODE_VALUE, 4);  // FUNC_MODE

	TC358870_i2c_Write(0x0154, 0x00000001, 4);  // DSI_RX_VC_ENABLE
	TC358870_i2c_Write(0x0158, 0x000000C8, 4);  // IND_TO_COUNT
	TC358870_i2c_Write(0x0168, 0x0000002A, 4);  // DSI_HSYNC_STOP_COUNT
	TC358870_i2c_Write(0x0170, 0x000004bf, 4);  // APF_VDELAYCNT
	TC358870_i2c_Write(0x017C, 0x00000081, 4);  // DSI_TX_MODE
	TC358870_i2c_Write(0x018C, 0x00000001, 4);  // DSI_HSYNC_WIDTH
	TC358870_i2c_Write(0x0190, 0x00000122, 4);  // DSI_HBPR
	TC358870_i2c_Write(0x01A4, 0x00000000, 4);  // DSI_RX_STATE_INT_MASK
	TC358870_i2c_Write(0x01C0, 0x00000015, 4);  // DSI_LPRX_THRESH_COUNT
	TC358870_i2c_Write(0x0214, 0x00000000, 4);  // APP_SIDE_ERR_INT_MASK
	TC358870_i2c_Write(0x021C, 0x00000080, 4);  // DSI_RX_ERR_INT_MASK
	TC358870_i2c_Write(0x0224, 0x00000000, 4);  // DSI_LPTX_INT_MASK
	TC358870_i2c_Write(0x0254, 0x00000006, 4);  // LPTXTIMECNT
	TC358870_i2c_Write(0x0258, 0x00260205, 4);  // TCLK_HEADERCNT
	TC358870_i2c_Write(0x025C, 0x000d0009, 4);  // TCLK_TRAILCNT
	TC358870_i2c_Write(0x0260, 0x00140007, 4);  // THS_HEADERCNT
	TC358870_i2c_Write(0x0264, 0x00004268, 4);  // TWAKEUPCNT
	TC358870_i2c_Write(0x0268, 0x0000000F, 4);  // TCLK_POSTCNT
	TC358870_i2c_Write(0x026C, 0x000d0009, 4);  // THS_TRAILCNT
	TC358870_i2c_Write(0x0270, 0x00000020, 4);  // HSTXVREGCNT
	TC358870_i2c_Write(0x0274, 0x0000001F, 4);  // HSTXVREGEN
	TC358870_i2c_Write(0x0278, 0x00060007, 4);  // BTA_COUNT
	TC358870_i2c_Write(0x027C, 0x00000002, 4);  // DPHY_TX ADJUST
	TC358870_i2c_Write(0x011C, 0x00000001, 4);  // DSITX_START

	// DSI-TX1 Transition Timing
	TC358870_i2c_Write(0x0308, 0x00000001, 4);  // DSI_TX_CLKEN
	TC358870_i2c_Write(0x030C, 0x00000001, 4);  // DSI_TX_CLKSEL
	TC358870_i2c_Write(0x04A0, 0x00000001, 4);  // MIPI_PLL_CONTROL
	TC358870_i2c_Write(0x04AC, 0x00009095, 4);  // MIPI_PLL_CNF
	delay_ms(2);

	TC358870_i2c_Write(0x04A0, 0x00000003, 4);  // MIPI_PLL_CONTROL
	TC358870_i2c_Write(0x0318, 0x00000014, 4);  // LANE_ENABLE
	delay_ms(1);
	TC358870_i2c_Write(0x0320, 0x00001770, 4);  // LINE_INIT_COUNT
	TC358870_i2c_Write(0x0324, 0x00000000, 4);  // HSTX_TO_COUNT
	TC358870_i2c_Write(0x0328, 0x00000101, 4);  // FUNC_ENABLE
	TC358870_i2c_Write(0x0330, 0x00010000, 4);  // DSI_TATO_COUNT
	TC358870_i2c_Write(0x0334, 0x00005000, 4);  // DSI_PRESP_BTA_COUNT
	TC358870_i2c_Write(0x0338, 0x00010000, 4);  // DSI_PRESP_LPR_COUNT
	TC358870_i2c_Write(0x033C, 0x00010000, 4);  // DSI_PRESP_LPW_COUNT
	TC358870_i2c_Write(0x0340, 0x00010000, 4);  // DSI_PRESP_HSR_COUNT
	TC358870_i2c_Write(0x0344, 0x00010000, 4);  // DSI_PRESP_HSW_COUNT
	TC358870_i2c_Write(0x0348, 0x00001000, 4);  // DSI_PR_TO_COUNT
	TC358870_i2c_Write(0x034C, 0x00010000, 4);  // DSI_LRX-H_TO_COUNT
	// TC358870_i2c_Write(0x0350, 0x00000141, 4);  // FUNC_MODE
	TC358870_i2c_Write(0x0350, TC_FUNC_MODE_VALUE, 4);  // FUNC_MODE

	TC358870_i2c_Write(0x0354, 0x00000001, 4);  // DSI_RX_VC_ENABLE
	TC358870_i2c_Write(0x0358, 0x000000C8, 4);  // IND_TO_COUNT
	TC358870_i2c_Write(0x0368, 0x0000002A, 4);  // DSI_HSYNC_STOP_COUNT
	TC358870_i2c_Write(0x0370, 0x000004bf, 4);  // APF_VDELAYCNT
	TC358870_i2c_Write(0x037C, 0x00000081, 4);  // DSI_TX_MODE
	TC358870_i2c_Write(0x038C, 0x00000001, 4);  // DSI_HSYNC_WIDTH
	TC358870_i2c_Write(0x0390, 0x00000122, 4);  // DSI_HBPR
	TC358870_i2c_Write(0x03A4, 0x00000000, 4);  // DSI_RX_STATE_INT_MASK
	TC358870_i2c_Write(0x03C0, 0x00000015, 4);  // DSI_LPRX_THRESH_COUNT
	TC358870_i2c_Write(0x0414, 0x00000000, 4);  // APP_SIDE_ERR_INT_MASK
	TC358870_i2c_Write(0x041C, 0x00000080, 4);  // DSI_RX_ERR_INT_MASK
	TC358870_i2c_Write(0x0424, 0x00000000, 4);  // DSI_LPTX_INT_MASK
	TC358870_i2c_Write(0x0454, 0x00000006, 4);  // LPTXTIMECNT
	TC358870_i2c_Write(0x0458, 0x00260205, 4);  // TCLK_HEADERCNT
	TC358870_i2c_Write(0x045C, 0x000d0009, 4);  // TCLK_TRAILCNT
	TC358870_i2c_Write(0x0460, 0x00140007, 4);  // THS_HEADERCNT
	TC358870_i2c_Write(0x0464, 0x00004268, 4);  // TWAKEUPCNT
	TC358870_i2c_Write(0x0468, 0x0000000F, 4);  // TCLK_POSTCNT
	TC358870_i2c_Write(0x046C, 0x000d0009, 4);  // THS_TRAILCNT
	TC358870_i2c_Write(0x0470, 0x00000020, 4);  // HSTXVREGCNT
	TC358870_i2c_Write(0x0474, 0x0000001F, 4);  // HSTXVREGEN
	TC358870_i2c_Write(0x0478, 0x00060007, 4);  // BTA_COUNT
	TC358870_i2c_Write(0x047C, 0x00000002, 4);  // DPHY_TX ADJUST
	TC358870_i2c_Write(0x031C, 0x00000001, 4);  // DSITX_START
}

void Toshiba_TC358870_Prepare_TX()
{
#if 0

	// version from init receive hdmi - identical registers and order, just different values.
	// Doesn't match TFN sniff as closely.

	// DSI-TX0 Transition Timing
	TC358870_i2c_Write(0x0108, 0x00000001, 4);  // DSI_TX_CLKEN
	TC358870_i2c_Write(0x010C, 0x00000001, 4);  // DSI_TX_CLKSEL
	TC358870_i2c_Write(0x02A0, 0x00000001, 4);  // MIPI_PLL_CONTROL
	TC358870_i2c_Write(0x02AC, 0x00003047, 4);  // MIPI_PLL_CNF
	delay_ms(1);

	TC358870_i2c_Write(0x02A0, 0x00000003, 4);  // MIPI_PLL_CONTROL
	TC358870_i2c_Write(0x0118, 0x00000014, 4);  // LANE_ENABLE
	TC358870_i2c_Write(0x0120, 0x00001B58, 4);  // LINE_INIT_COUNT
	TC358870_i2c_Write(0x0124, 0x00000000, 4);  // HSTX_TO_COUNT
	TC358870_i2c_Write(0x0128, 0x00000101, 4);  // FUNC_ENABLE
	TC358870_i2c_Write(0x0130, 0x00010000, 4);  // DSI_TATO_COUNT
	TC358870_i2c_Write(0x0134, 0x00005000, 4);  // DSI_PRESP_BTA_COUNT
	TC358870_i2c_Write(0x0138, 0x00010000, 4);  // DSI_PRESP_LPR_COUNT
	TC358870_i2c_Write(0x013C, 0x00010000, 4);  // DSI_PRESP_LPW_COUNT
	TC358870_i2c_Write(0x0140, 0x00010000, 4);  // DSI_PRESP_HSR_COUNT
	TC358870_i2c_Write(0x0144, 0x00010000, 4);  // DSI_PRESP_HSW_COUNT
	TC358870_i2c_Write(0x0148, 0x00001000, 4);  // DSI_PR_TO_COUNT
	TC358870_i2c_Write(0x014C, 0x00010000, 4);  // DSI_LRX-H_TO_COUNT
	//TC358870_i2c_Write(0x0150, 0x00000161, 4);  // FUNC_MODE
	TC358870_i2c_Write(0x0150, TC_FUNC_MODE_VALUE, 4);  // FUNC_MODE
	TC358870_i2c_Write(0x0154, 0x00000001, 4);  // DSI_RX_VC_ENABLE
	TC358870_i2c_Write(0x0158, 0x000000C8, 4);  // IND_TO_COUNT
	TC358870_i2c_Write(0x0168, 0x0000002A, 4);  // DSI_HSYNC_STOP_COUNT
	TC358870_i2c_Write(0x0170, 0x000003CC, 4);  // APF_VDELAYCNT
	TC358870_i2c_Write(0x017C, 0x00000081, 4);  // DSI_TX_MODE
	TC358870_i2c_Write(0x018C, 0x00000001, 4);  // DSI_HSYNC_WIDTH
	TC358870_i2c_Write(0x0190, 0x0000018C, 4);  // DSI_HBPR
	TC358870_i2c_Write(0x01A4, 0x00000000, 4);  // DSI_RX_STATE_INT_MASK
	TC358870_i2c_Write(0x01C0, 0x00000015, 4);  // DSI_LPRX_THRESH_COUNT
	TC358870_i2c_Write(0x0214, 0x00000000, 4);  // APP_SIDE_ERR_INT_MASK
	TC358870_i2c_Write(0x021C, 0x00000080, 4);  // DSI_RX_ERR_INT_MASK
	TC358870_i2c_Write(0x0224, 0x00000000, 4);  // DSI_LPTX_INT_MASK
	TC358870_i2c_Write(0x0254, 0x00000006, 4);  // LPTXTIMECNT
	TC358870_i2c_Write(0x0258, 0x001C0206, 4);  // TCLK_HEADERCNT
	TC358870_i2c_Write(0x025C, 0x000E0008, 4);  // TCLK_TRAILCNT
	TC358870_i2c_Write(0x0260, 0x000B0006, 4);  // THS_HEADERCNT
	TC358870_i2c_Write(0x0264, 0x00004650, 4);  // TWAKEUPCNT
	TC358870_i2c_Write(0x0268, 0x00000010, 4);  // TCLK_POSTCNT
	TC358870_i2c_Write(0x026C, 0x000C0008, 4);  // THS_TRAILCNT
	TC358870_i2c_Write(0x0270, 0x00000020, 4);  // HSTXVREGCNT
	TC358870_i2c_Write(0x0274, 0x0000001F, 4);  // HSTXVREGEN
	TC358870_i2c_Write(0x0278, 0x00060006, 4);  // BTA_COUNT
	TC358870_i2c_Write(0x027C, 0x00000002, 4);  // DPHY_TX ADJUST
	TC358870_i2c_Write(0x011C, 0x00000001, 4);  // DSITX_START

	// DSI-TX1 Transition Timing
	TC358870_i2c_Write(0x0308, 0x00000001, 4);  // DSI_TX_CLKEN
	TC358870_i2c_Write(0x030C, 0x00000001, 4);  // DSI_TX_CLKSEL
	TC358870_i2c_Write(0x04A0, 0x00000001, 4);  // MIPI_PLL_CONTROL
	TC358870_i2c_Write(0x04AC, 0x00003047, 4);  // MIPI_PLL_CNF
	delay_ms(1);

	TC358870_i2c_Write(0x04A0, 0x00000003, 4);  // MIPI_PLL_CONTROL
	TC358870_i2c_Write(0x0318, 0x00000014, 4);  // LANE_ENABLE
	TC358870_i2c_Write(0x0320, 0x00001B58, 4);  // LINE_INIT_COUNT
	TC358870_i2c_Write(0x0324, 0x00000000, 4);  // HSTX_TO_COUNT
	TC358870_i2c_Write(0x0328, 0x00000101, 4);  // FUNC_ENABLE
	TC358870_i2c_Write(0x0330, 0x00010000, 4);  // DSI_TATO_COUNT
	TC358870_i2c_Write(0x0334, 0x00005000, 4);  // DSI_PRESP_BTA_COUNT
	TC358870_i2c_Write(0x0338, 0x00010000, 4);  // DSI_PRESP_LPR_COUNT
	TC358870_i2c_Write(0x033C, 0x00010000, 4);  // DSI_PRESP_LPW_COUNT
	TC358870_i2c_Write(0x0340, 0x00010000, 4);  // DSI_PRESP_HSR_COUNT
	TC358870_i2c_Write(0x0344, 0x00010000, 4);  // DSI_PRESP_HSW_COUNT
	TC358870_i2c_Write(0x0348, 0x00001000, 4);  // DSI_PR_TO_COUNT
	TC358870_i2c_Write(0x034C, 0x00010000, 4);  // DSI_LRX-H_TO_COUNT
	TC358870_i2c_Write(0x0350, 0x00000161, 4);  // FUNC_MODE
	TC358870_i2c_Write(0x0354, 0x00000001, 4);  // DSI_RX_VC_ENABLE
	TC358870_i2c_Write(0x0358, 0x000000C8, 4);  // IND_TO_COUNT
	TC358870_i2c_Write(0x0368, 0x0000002A, 4);  // DSI_HSYNC_STOP_COUNT
	TC358870_i2c_Write(0x0370, 0x000003CC, 4);  // APF_VDELAYCNT
	TC358870_i2c_Write(0x037C, 0x00000081, 4);  // DSI_TX_MODE
	TC358870_i2c_Write(0x038C, 0x00000001, 4);  // DSI_HSYNC_WIDTH
	TC358870_i2c_Write(0x0390, 0x0000018C, 4);  // DSI_HBPR
	TC358870_i2c_Write(0x03A4, 0x00000000, 4);  // DSI_RX_STATE_INT_MASK
	TC358870_i2c_Write(0x03C0, 0x00000015, 4);  // DSI_LPRX_THRESH_COUNT
	TC358870_i2c_Write(0x0414, 0x00000000, 4);  // APP_SIDE_ERR_INT_MASK
	TC358870_i2c_Write(0x041C, 0x00000080, 4);  // DSI_RX_ERR_INT_MASK
	TC358870_i2c_Write(0x0424, 0x00000000, 4);  // DSI_LPTX_INT_MASK
	TC358870_i2c_Write(0x0454, 0x00000006, 4);  // LPTXTIMECNT
	TC358870_i2c_Write(0x0458, 0x001C0206, 4);  // TCLK_HEADERCNT
	TC358870_i2c_Write(0x045C, 0x000E0008, 4);  // TCLK_TRAILCNT
	TC358870_i2c_Write(0x0460, 0x000B0006, 4);  // THS_HEADERCNT
	TC358870_i2c_Write(0x0464, 0x00004650, 4);  // TWAKEUPCNT
	TC358870_i2c_Write(0x0468, 0x00000010, 4);  // TCLK_POSTCNT
	TC358870_i2c_Write(0x046C, 0x000C0008, 4);  // THS_TRAILCNT
	TC358870_i2c_Write(0x0470, 0x00000020, 4);  // HSTXVREGCNT
	TC358870_i2c_Write(0x0474, 0x0000001F, 4);  // HSTXVREGEN
	TC358870_i2c_Write(0x0478, 0x00060006, 4);  // BTA_COUNT
	TC358870_i2c_Write(0x047C, 0x00000002, 4);  // DPHY_TX ADJUST
	TC358870_i2c_Write(0x031C, 0x00000001, 4);  // DSITX_START

#endif
#if 0
	// DSI-TX0 Transition Timing
	TC358870_i2c_Write(0x0108, 0x00000001, 4);  // DSI_TX_CLKEN
	TC358870_i2c_Write(0x010C, 0x00000001, 4);  // DSI_TX_CLKSEL
	TC358870_i2c_Write(0x02A0, 0x00000001, 4);  // MIPI_PLL_CONTROL
	TC358870_i2c_Write(0x02AC, 0x000090B0, 4);  // MIPI_PLL_CNF
	delay_ms(1);

	TC358870_i2c_Write(0x02A0, 0x00000003, 4);  // MIPI_PLL_CONTROL
	TC358870_i2c_Write(0x0118, 0x00000014, 4);  // LANE_ENABLE
	TC358870_i2c_Write(0x0120, 0x00001770, 4);  // LINE_INIT_COUNT
	TC358870_i2c_Write(0x0124, 0x00000000, 4);  // HSTX_TO_COUNT
	TC358870_i2c_Write(0x0128, 0x00000101, 4);  // FUNC_ENABLE
	TC358870_i2c_Write(0x0130, 0x00010000, 4);  // DSI_TATO_COUNT
	TC358870_i2c_Write(0x0134, 0x00005000, 4);  // DSI_PRESP_BTA_COUNT
	TC358870_i2c_Write(0x0138, 0x00010000, 4);  // DSI_PRESP_LPR_COUNT
	TC358870_i2c_Write(0x013C, 0x00010000, 4);  // DSI_PRESP_LPW_COUNT
	TC358870_i2c_Write(0x0140, 0x00010000, 4);  // DSI_PRESP_HSR_COUNT
	TC358870_i2c_Write(0x0144, 0x00010000, 4);  // DSI_PRESP_HSW_COUNT
	TC358870_i2c_Write(0x0148, 0x00001000, 4);  // DSI_PR_TO_COUNT
	TC358870_i2c_Write(0x014C, 0x00010000, 4);  // DSI_LRX-H_TO_COUNT
	//TC358870_i2c_Write(0x0150, 0x00000160, 4);  // FUNC_MODE
	TC358870_i2c_Write(0x0150, TC_FUNC_MODE_VALUE, 4);  // FUNC_MODE
	TC358870_i2c_Write(0x0154, 0x00000001, 4);  // DSI_RX_VC_ENABLE
	TC358870_i2c_Write(0x0158, 0x000000C8, 4);  // IND_TO_COUNT
	TC358870_i2c_Write(0x0168, 0x0000002A, 4);  // DSI_HSYNC_STOP_COUNT
	TC358870_i2c_Write(0x0170, 0x000003CB, 4);  // APF_VDELAYCNT
	TC358870_i2c_Write(0x017C, 0x00000081, 4);  // DSI_TX_MODE
	TC358870_i2c_Write(0x018C, 0x00000001, 4);  // DSI_HSYNC_WIDTH
	TC358870_i2c_Write(0x0190, 0x0000016C, 4);  // DSI_HBPR
	TC358870_i2c_Write(0x01A4, 0x00000000, 4);  // DSI_RX_STATE_INT_MASK
	TC358870_i2c_Write(0x01C0, 0x00000015, 4);  // DSI_LPRX_THRESH_COUNT
	TC358870_i2c_Write(0x0214, 0x00000000, 4);  // APP_SIDE_ERR_INT_MASK
	TC358870_i2c_Write(0x021C, 0x00000080, 4);  // DSI_RX_ERR_INT_MASK
	TC358870_i2c_Write(0x0224, 0x00000000, 4);  // DSI_LPTX_INT_MASK
	TC358870_i2c_Write(0x0254, 0x00000006, 4);  // LPTXTIMECNT
	TC358870_i2c_Write(0x0258, 0x00240204, 4);  // TCLK_HEADERCNT
	TC358870_i2c_Write(0x025C, 0x000D0008, 4);  // TCLK_TRAILCNT
	TC358870_i2c_Write(0x0260, 0x00140006, 4);  // THS_HEADERCNT
	TC358870_i2c_Write(0x0264, 0x00004268, 4);  // TWAKEUPCNT
	TC358870_i2c_Write(0x0268, 0x0000000F, 4);  // TCLK_POSTCNT
	TC358870_i2c_Write(0x026C, 0x000D0008, 4);  // THS_TRAILCNT
	TC358870_i2c_Write(0x0270, 0x00000020, 4);  // HSTXVREGCNT
	TC358870_i2c_Write(0x0274, 0x0000001F, 4);  // HSTXVREGEN
	TC358870_i2c_Write(0x0278, 0x00060005, 4);  // BTA_COUNT
	TC358870_i2c_Write(0x027C, 0x00000002, 4);  // DPHY_TX ADJUST
	TC358870_i2c_Write(0x011C, 0x00000001, 4);  // DSITX_START

	// DSI-TX1 Transition Timing
	TC358870_i2c_Write(0x0308, 0x00000001, 4);  // DSI_TX_CLKEN
	TC358870_i2c_Write(0x030C, 0x00000001, 4);  // DSI_TX_CLKSEL
	TC358870_i2c_Write(0x04A0, 0x00000001, 4);  // MIPI_PLL_CONTROL
	TC358870_i2c_Write(0x04AC, 0x000090B0, 4);  // MIPI_PLL_CNF
	delay_ms(1);

	TC358870_i2c_Write(0x04A0, 0x00000003, 4);  // MIPI_PLL_CONTROL
	TC358870_i2c_Write(0x0318, 0x00000014, 4);  // LANE_ENABLE
	TC358870_i2c_Write(0x0320, 0x00001770, 4);  // LINE_INIT_COUNT
	TC358870_i2c_Write(0x0324, 0x00000000, 4);  // HSTX_TO_COUNT
	TC358870_i2c_Write(0x0328, 0x00000101, 4);  // FUNC_ENABLE
	TC358870_i2c_Write(0x0330, 0x00010000, 4);  // DSI_TATO_COUNT
	TC358870_i2c_Write(0x0334, 0x00005000, 4);  // DSI_PRESP_BTA_COUNT
	TC358870_i2c_Write(0x0338, 0x00010000, 4);  // DSI_PRESP_LPR_COUNT
	TC358870_i2c_Write(0x033C, 0x00010000, 4);  // DSI_PRESP_LPW_COUNT
	TC358870_i2c_Write(0x0340, 0x00010000, 4);  // DSI_PRESP_HSR_COUNT
	TC358870_i2c_Write(0x0344, 0x00010000, 4);  // DSI_PRESP_HSW_COUNT
	TC358870_i2c_Write(0x0348, 0x00001000, 4);  // DSI_PR_TO_COUNT
	TC358870_i2c_Write(0x034C, 0x00010000, 4);  // DSI_LRX-H_TO_COUNT
	//TC358870_i2c_Write(0x0350, 0x00000160, 4);  // FUNC_MODE
	TC358870_i2c_Write(0x0350, TC_FUNC_MODE_VALUE, 4);  // FUNC_MODE
	TC358870_i2c_Write(0x0354, 0x00000001, 4);  // DSI_RX_VC_ENABLE
	TC358870_i2c_Write(0x0358, 0x000000C8, 4);  // IND_TO_COUNT
	TC358870_i2c_Write(0x0368, 0x0000002A, 4);  // DSI_HSYNC_STOP_COUNT
	TC358870_i2c_Write(0x0370, 0x000003CB, 4);  // APF_VDELAYCNT
	TC358870_i2c_Write(0x037C, 0x00000081, 4);  // DSI_TX_MODE
	TC358870_i2c_Write(0x038C, 0x00000001, 4);  // DSI_HSYNC_WIDTH
	TC358870_i2c_Write(0x0390, 0x0000016C, 4);  // DSI_HBPR
	TC358870_i2c_Write(0x03A4, 0x00000000, 4);  // DSI_RX_STATE_INT_MASK
	TC358870_i2c_Write(0x03C0, 0x00000015, 4);  // DSI_LPRX_THRESH_COUNT
	TC358870_i2c_Write(0x0414, 0x00000000, 4);  // APP_SIDE_ERR_INT_MASK
	TC358870_i2c_Write(0x041C, 0x00000080, 4);  // DSI_RX_ERR_INT_MASK
	TC358870_i2c_Write(0x0424, 0x00000000, 4);  // DSI_LPTX_INT_MASK
	TC358870_i2c_Write(0x0454, 0x00000006, 4);  // LPTXTIMECNT
	TC358870_i2c_Write(0x0458, 0x00240204, 4);  // TCLK_HEADERCNT
	TC358870_i2c_Write(0x045C, 0x000D0008, 4);  // TCLK_TRAILCNT
	TC358870_i2c_Write(0x0460, 0x00140006, 4);  // THS_HEADERCNT
	TC358870_i2c_Write(0x0464, 0x00004268, 4);  // TWAKEUPCNT
	TC358870_i2c_Write(0x0468, 0x0000000F, 4);  // TCLK_POSTCNT
	TC358870_i2c_Write(0x046C, 0x000D0008, 4);  // THS_TRAILCNT
	TC358870_i2c_Write(0x0470, 0x00000020, 4);  // HSTXVREGCNT
	TC358870_i2c_Write(0x0474, 0x0000001F, 4);  // HSTXVREGEN
	TC358870_i2c_Write(0x0478, 0x00060005, 4);  // BTA_COUNT
	TC358870_i2c_Write(0x047C, 0x00000002, 4);  // DPHY_TX ADJUST
	TC358870_i2c_Write(0x031C, 0x00000001, 4);  // DSITX_START
#endif
	Toshiba_TC358770_Setup_TX_TFN();
	// Command Transmission Before Video Start
	TC358870_i2c_Write(0x0110, 0x00000016, 4);  // MODE_CONFIG - high speed mode DSI commands, hsync+, vsync+
	TC358870_i2c_Write(0x0310, 0x00000016, 4);  // MODE_CONFIG
	TC358870_i2c_Write(0x0500, 0x0004, 2);      // CMD_SEL - send dcs cmds to both tx
}

void Toshiba_TC358870_Configure_Splitter()
{
#if 1  // Dennis Yeh 2016/04/18
	// Split Control
	TC358870_i2c_Write(0x5000, 0x0000, 2);  // STX0_CTL
	/// @todo 0x8??? indicates "auto split line in half" and 0x?4?00 indicates tx0 gets first half
	/// why is 0x??E0 (first pixel for tx0) not zero? it's not used...
	TC358870_i2c_Write(0x500C, 0x84E0, 2);  // STX0_FPX
	TC358870_i2c_Write(0x5080, 0x0000, 2);  // STX1_CTL
#else                                       // shift 28 pixels.
	TC358870_i2c_Write(0x5000, 0x0000, 2);  // STX0_CTL
	TC358870_i2c_Write(0x500C, 0x0000, 2);  // STX0_FPX
	TC358870_i2c_Write(0x500E, 0x041B, 2);  // STX0_LPX
	TC358870_i2c_Write(0x5080, 0x0000, 2);  // STX1_CTL
	TC358870_i2c_Write(0x508C, 0x0454, 2);  // STX1_FPX
	TC358870_i2c_Write(0x508E, 0x086F, 2);  // STX1_LPX
	TC358870_i2c_Write(0x500A, 0x001C, 2);  // STX0_DPX
	// TC358870_i2c_Write(0x5010,0x0000, 2); // STX0_DRPX
	// TC358870_i2c_Write(0x5012,0x0000, 2); // STX0_DGPX
	// TC358870_i2c_Write(0x5014,0x0000, 2); // STX0_DBPX
	TC358870_i2c_Write(0x508A, 0x1C00, 2);  // STX1_DPX
                                            // TC358870_i2c_Write(0x5090,0x0000, 2); // STX1_DRPX
                                            // TC358870_i2c_Write(0x5092,0x0000, 2); // STX1_DGPX
                                            // TC358870_i2c_Write(0x5094,0x0000, 2); // STX1_DBPX
#endif
}

/// Sets up HDMI, including sending EDID data to receiver
void Toshiba_TC358870_HDMI_Setup(void)
{
	// HDMI PHY
	TC358870_i2c_Write(0x8410, 0x03, 1);  // PHY CTL
	TC358870_i2c_Write(0x8413, 0x3F, 1);  // PHY_ENB
	TC358870_i2c_Write(0x8420, 0x06, 1);  // EQ_BYPS
	TC358870_i2c_Write(0x84F0, 0x31, 1);  // APLL_CTL
	TC358870_i2c_Write(0x84F4, 0x01, 1);  // DDCIO_CTL

	// HDMI Clock
	TC358870_i2c_Write(0x8540, 0x12C0, 2);  // SYS_FREQ0_1
	TC358870_i2c_Write(0x8630, 0x00, 1);    // LOCKDET_FREQ0
	TC358870_i2c_Write(0x8631, 0x0753, 2);  // LOCKDET_REF1_2
	TC358870_i2c_Write(0x8670, 0x02, 1);    // NCO_F0_MOD
	TC358870_i2c_Write(0x8A0C, 0x12C0, 2);  // CSC_SCLK0_1

	// HDMI Interrupt Mask, Clears any pending video sync change interrupt flags and allows only those through.
	TC358870_i2c_Write(0x850B, 0xFF, 1);  // MISC_INT
#ifdef SVR_VIDEO_INPUT_POLL_INTERVAL
	TC358870_i2c_Write(0x851B, 0xFF, 1);  // MISC_INTM
#else
	TC358870_i2c_Write(0x851B, 0xFD, 1);    // MISC_INTM
#endif  // SVR_VIDEO_INPUT_POLL_INTERVAL

	// Interrupt Control (TOP level)
	TC358870_i2c_Write(0x0014, 0x0FBF, 2);  // IntStatus
#ifdef SVR_VIDEO_INPUT_POLL_INTERVAL
	TC358870_i2c_Write(0x0016, 0x0FBF, 2);  // IntMask
#else
	TC358870_i2c_Write(0x0016, 0x0DBF, 2);  // IntMask
#endif  // SVR_VIDEO_INPUT_POLL_INTERVAL

	// EDID
	TC358870_i2c_Write(0x85E0, 0x01, 1);    // EDID_MODE  Internal EDID-RAM & DDC2B mode
	TC358870_i2c_Write(0x85E3, 0x0100, 2);  // EDID_LEN1_2

	// EDID Data
	OSVR_HDK_EDID();

	// HDCP Setting
	// TC358870_i2c_Write(0x8840, 0xC0, 1); // HDCP Repeater enable
	TC358870_i2c_Write(0x85EC, 0x01, 1);  // key loading command

	// Video Color Format Setting
	TC358870_i2c_Write(0x8A02, 0x42, 1);  // VOUT_SYNC0

	// HDMI SYSTEM
	// DDC active only while hotplug is being output, no delay in DDC5V rising detection.
	TC358870_i2c_Write(0x8543, 0x02, 1);  // DDC_CTL
	// DDC5v detection interlock - HPDO will be asserted when INIT_END asserted
	TC358870_i2c_Write(0x8544, 0x10, 1);  // HPD_CTL

	// HDMI Audio Setting
	TC358870_i2c_Write(0x8600, 0x00, 1);        // AUD_Auto_Mute
	TC358870_i2c_Write(0x8602, 0xF3, 1);        // Auto_CMD0
	TC358870_i2c_Write(0x8603, 0x02, 1);        // Auto_CMD1
	TC358870_i2c_Write(0x8604, 0x0C, 1);        // Auto_CMD2
	TC358870_i2c_Write(0x8606, 0x05, 1);        // BUFINIT_START
	TC358870_i2c_Write(0x8607, 0x00, 1);        // FS_MUTE
	TC358870_i2c_Write(0x8652, 0x02, 1);        // SDO_MODE1
	TC358870_i2c_Write(0x8671, 0x020C49BA, 4);  // NCO_48F0A_D
	TC358870_i2c_Write(0x8675, 0x01E1B089, 4);  // NCO_44F0A_D
	TC358870_i2c_Write(0x8680, 0x00, 1);        // AUD_MODE

// Let HDMI Source start access
#if 1
	TC358870_i2c_Write(0x854A, 0x01, 1);  // INIT_END
#else
	TC358870_i2c_Write(0x854A, 0x8001, 2);  // INIT_END
#endif
}

#endif  // INCLUDED_Toshiba_TC358870_Setup_Impl_h_GUID_098B0B47_6470_4AA6_589E_4B32CCC542C8

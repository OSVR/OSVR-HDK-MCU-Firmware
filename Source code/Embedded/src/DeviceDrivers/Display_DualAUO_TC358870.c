/*
 * Display_DualAUO_TC358870.c
 *
 * Created: 7/19/2016 9:40:29 AM
 *  Author: Coretronic, Sensics
 */

#include "GlobalOptions.h"

#define HDMI_VERBOSE

#if defined(SVR_IS_HDK_20)
#include "my_hardware.h"

#include "Display.h"
#include "VideoInput.h"
#include "Console.h"
#include "SvrYield.h"
#include "BitUtilsC.h"
#include <ioport.h>

#include "DeviceDrivers/Toshiba_TC358870.h"
#include "DeviceDrivers/Toshiba_TC358870_ISR.h"
#include "DeviceDrivers/HDK2.h"

#include <stdio.h>
#include <inttypes.h>

// whether to send the sleep in/out and display on/off commands with a parameter of 0x00, or with no parameter. Appears
// to work both ways, docs are contradictory: AUO app note says use a parameter, standard says don't
#undef SLEEP_HAS_NULL_PARAMETER
static void AUO_Null_Param_DSI_Cmd(uint8_t cmd);

static void AUO_H381DLN01_Send_Panel_Init_Commands(void);
static void AUO_H381DLN01_Panel_Reset(void);

typedef struct CommandSequenceElt
{
	uint8_t addr;
	uint8_t param;
} CommandSequenceElt_t;

static const CommandSequenceElt_t AUO_H381DLN01_Init_Commands[] = {
    {0xFE, 0x07}, {0x00, 0xEC}, {0x0B, 0xEC}, {0x16, 0xEC}, {0x21, 0xEC}, {0x2D, 0xEC}, {0xA9, 0xBA}, {0xAB, 0x06},
    {0xBB, 0x84}, {0xBC, 0x1C}, {0xFE, 0x08}, {0x07, 0x1A}, {0xFE, 0x0A}, {0x2A, 0x1B}, {0xFE, 0x0D}, {0x02, 0x65},
    {0x4D, 0x41}, {0x4B, 0x0F}, {0x53, 0xFE}, {0xFE, 0x00}, {0xC2, 0x03}, {0x51, 0xFF}};

static void AUO_H381DLN01_Send_Panel_Init_Commands(void)
{
	/// Need at least 10ms from reset going back high before initial setting.
	svr_yield_ms(10);
	/// Run all display setup commands for these panels from the app note.
	unsigned int numCommands = sizeof(AUO_H381DLN01_Init_Commands) / sizeof(AUO_H381DLN01_Init_Commands[0]);
	for (unsigned int i = 0; i < numCommands; ++i)
	{
		Toshiba_TC358870_DSI_Write_Cmd_Short_Param(AUO_H381DLN01_Init_Commands[i].addr,
		                                           AUO_H381DLN01_Init_Commands[i].param);
		svr_yield_ms(16);
	}
}

static void AUO_H381DLN01_Panel_StartReset(void)
{
	ioport_set_pin_low(PANEL_RESET);
	// >3ms recommended to move from deep standby to sleep state.
	svr_yield_ms(10);
}
static void AUO_H381DLN01_Panel_EndReset(void)
{
	ioport_set_pin_high(PANEL_RESET);
	// Reset after this signal takes at most 5ms during sleep mode, 120ms during non-sleep mode (and can't "sleep out"
	// for 120ms)
	svr_yield_ms(150);
}

static void AUO_H381DLN01_Panel_Reset(void)
{
	AUO_H381DLN01_Panel_StartReset();
	AUO_H381DLN01_Panel_EndReset();
}
const TC358870_PanelFunctions_t g_tc358870PanelFuncs = {.startReset = &AUO_H381DLN01_Panel_StartReset,
                                                        .endReset = &AUO_H381DLN01_Panel_EndReset,
                                                        .sendInitCommands = &AUO_H381DLN01_Send_Panel_Init_Commands};

// Pre-defined timings
const TC358870_DSITX_Config_t TC358870_DSITX_Config_60hz_2160_1200 = {
    .MIPI_PLL_CONF = UINT32_C(0x00009095),
    .FUNC_MODE = UINT32_C(0x00000141),
    .APF_VDELAYCNT = UINT32_C(0x000004bf),
    .DSI_HBPR = UINT32_C(0x00000122),
    .PPI_DPHY_TCLK_HEADERCNT = UINT32_C(0x00260205),
    .PPI_DPHY_TCLK_TRAILCNT = UINT32_C(0x000d0009),
    .PPI_DPHY_THS_HEADERCNT = UINT32_C(0x00140007),
    .PPI_DPHY_TWAKEUPCNT = UINT32_C(0x00004268),
    .PPI_DPHY_TCLK_POSTCNT = UINT32_C(0x0000000f),
    .PPI_DPHY_THSTRAILCNT = UINT32_C(0x000d0009),
    .PPI_DSI_BTA_COUNT = UINT32_C(0x00060007),
};

const TC358870_DSITX_Config_t TC358870_DSITX_Config_90hz_2160_1200 = {
    .MIPI_PLL_CONF = UINT32_C(0x000090bd),
    .FUNC_MODE = UINT32_C(0x00000160),
    .APF_VDELAYCNT = UINT32_C(0x000003c4),
    .DSI_HBPR = UINT32_C(0x00000190),
    .PPI_DPHY_TCLK_HEADERCNT = UINT32_C(0x00240204),
    .PPI_DPHY_TCLK_TRAILCNT = UINT32_C(0x000d0008),
    .PPI_DPHY_THS_HEADERCNT = UINT32_C(0x00140006),
    .PPI_DPHY_TWAKEUPCNT = UINT32_C(0x00004268),
    .PPI_DPHY_TCLK_POSTCNT = UINT32_C(0x0000000f),
    .PPI_DPHY_THSTRAILCNT = UINT32_C(0x000d0008),
    .PPI_DSI_BTA_COUNT = UINT32_C(0x00060005),
};

const TC358870_DSITX_Config_t* g_TC358870_DSITX_Config_Default = &TC358870_DSITX_Config_90hz_2160_1200;

inline static void AUO_Null_Param_DSI_Cmd(uint8_t cmd)
{
#ifdef SLEEP_HAS_NULL_PARAMETER
	Toshiba_TC358870_DSI_Write_Cmd_Short_Param(cmd, 0x00);
#else
	Toshiba_TC358870_DSI_Write_Cmd_Short(cmd);
#endif
}
/// Note that must have 120ms since reset!
inline static void AUO_DSI_Sleep_Out(void) { AUO_Null_Param_DSI_Cmd(0x11); }
inline static void AUO_DSI_Display_On(void)
{
	// Spec says, >166ms. Best to be safe.
	svr_yield_ms(167);  //>10 frame
	AUO_Null_Param_DSI_Cmd(0x29);
}

inline static void AUO_DSI_Display_Off(void)
{
	AUO_Null_Param_DSI_Cmd(0x28);
	// Spec says, >166ms before bringing reset low. Best to be safe.
	svr_yield_ms(167);  //>10 frame
}
inline static void AUO_DSI_Sleep_In(void) { AUO_Null_Param_DSI_Cmd(0x10); }
void Display_System_Init()
{
	Toshiba_TC358870_Init_Once();
#if 0
	// start the chip, if it hasn't been started.
	if (!Toshiba_TC358870_Init_Once())
	{
		// if it has been started, do a mini reset
		Toshiba_TC358870_Disable_Video_TX();
		/// Software reset of TC358870's DSI-TX
		Toshiba_TC358870_DSITX_SW_Reset();
		Toshiba_TC358870_Prepare_TX();
		Toshiba_TC358870_Configure_Splitter();
	}
#endif
}
void Display_Init(uint8_t deviceID)
{
	// make sure we aren't held in reset mode.
	AUO_H381DLN01_Panel_EndReset();
#if 0
	static bool repeat = false;
	bool firstTime = false;
	if (!repeat)
	{
		repeat = true;
		firstTime = true;
	}

	bool shouldRestoreVideo = false;

	if (!firstTime)
	{
		// if it has been started, do a mini reset
		shouldRestoreVideo = VideoInput_Get_Status();
		Toshiba_TC358870_Disable_Video_TX();
		/// Software reset of TC358870's DSI-TX
		Toshiba_TC358870_DSITX_SW_Reset();
		Toshiba_TC358870_Prepare_TX();
		Toshiba_TC358870_Configure_Splitter();
	}

	AUO_H381DLN01_Send_Panel_Init_Commands();

	if (shouldRestoreVideo)
	{
		// this was a repeat init that started out with video.

		AUO_DSI_Sleep_Out();
		AUO_DSI_Display_On();

		// seemed like a good idea at the time
		svr_yield_ms(10);

		Toshiba_TC358870_Enable_Video_TX();
	}
#endif
}
void Display_On(uint8_t deviceID)
{
#ifdef HDMI_VERBOSE

#endif
	WriteLn("Turning display on");
	Debug_LED_Turn_On();

#if 0
	/// @todo ugly workaround for resetting things.
	// TC358870_Init_Receive_HDMI_Signal();
	// Toshiba_TC358870_Init();
	// Toshiba_TC358870_SW_Reset();
	AUO_H381DLN01_Panel_Reset();
	Toshiba_TC358870_DSITX_SW_Reset();
	Toshiba_TC358870_Prepare_TX();
	svr_yield_ms(16);
	AUO_H381DLN01_Send_Panel_Init_Commands();
	svr_yield_ms(120);
#endif

#if 0
	/// This one is at least a little bit less extreme
	AUO_H381DLN01_Panel_Reset();
	AUO_H381DLN01_Send_Panel_Init_Commands();
	svr_yield_ms(120);
	Toshiba_TC358870_Init_Receiver();
#endif

#if 0  // Not for SSD2848
	TC358870_i2c_Write(0x0504, 0x8029, 2); // DCSCMD Long Write
	TC358870_i2c_Write(0x0504, 0x0006, 2); // Number of data
	TC358870_i2c_Write(0x0504, 0xB700, 2); // 0xB7
	TC358870_i2c_Write(0x0504, 0x0000, 2); // DCSCMD_Q
	TC358870_i2c_Write(0x0504, 0x2903, 2); // DCSCMD_Q
#endif
	TC358870_InputMeasurements_t measurements = Toshiba_TC358770_Print_Input_Measurements();
	bool wasReset = false;
	if (measurements.reg8405 == 0x71 || measurements.reg8405 == 0x70)
	{
		WriteLn("Display_On: Apparent 90Hz input");
		wasReset = Toshiba_TC358770_Update_DSITX_Config_And_Reinit(&TC358870_DSITX_Config_90hz_2160_1200);
	}
	else
	{
		WriteLn("Display_On: Apparent non-90Hz input");
		wasReset = Toshiba_TC358770_Update_DSITX_Config_And_Reinit(&TC358870_DSITX_Config_60hz_2160_1200);
	}
	if (wasReset)
	{
		Toshiba_TC358770_Print_Input_Measurements();
		/// Wait 20 frames before doing anything with the panel.
		// svr_yield_ms(20 * TC358870_VSYNC_PERIOD_MS);
	}
	Toshiba_TC358870_Enable_Video_TX();
	/// Wait 20 frames before doing anything with the panel.
	svr_yield_ms(22 * TC358870_VSYNC_PERIOD_MS);
	AUO_DSI_Sleep_Out();
	/// Wait 25 frames between sleep out and display on
	svr_yield_ms(27 * TC358870_VSYNC_PERIOD_MS);
	AUO_DSI_Display_On();

	if (wasReset)
	{
		Toshiba_TC358870_Clear_HDMI_Sync_Change_Int();
		Toshiba_TC358870_MCU_Ints_Resume();
	}
}

void Display_Off(uint8_t deviceID)
{
#ifdef HDMI_VERBOSE
#endif

	WriteLn("Turning display off");
	/// Make the MCU start ignoring interrupts
	Toshiba_TC358870_MCU_Ints_Suspend();
	Debug_LED_Turn_Off();

	AUO_DSI_Display_Off();
	svr_yield_ms(3 * TC358870_VSYNC_PERIOD_MS);
	AUO_DSI_Sleep_In();

	svr_yield_ms(31 * TC358870_VSYNC_PERIOD_MS);
	Toshiba_TC358870_Disable_Video_TX();
	AUO_H381DLN01_Panel_StartReset();
	Toshiba_TC358870_Base_Init();
#if 0
	Toshiba_TC358870_Disable_Video_TX();
	Toshiba_TC358870_DSITX_SW_Reset();
	Toshiba_TC358870_Prepare_TX();
	AUO_H381DLN01_Send_Panel_Init_Commands();
#endif
/// @todo could power down the display completely here

/// @todo ugly workaround for resetting things.
/// This first version is uglier (more binary blob) but avoids a 1-2second period of a bright horizontal stripe on the
/// display during shutdown.
#if 0
	Toshiba_TC358870_Init();
#endif
#if 0
	Display_Reset(Display1);
	AUO_H381DLN01_Panel_Reset();
	AUO_H381DLN01_Send_Panel_Init_Commands();
#endif

#if 0
	TC358870_i2c_Write(0x0004, 0x0004, 2);  // ConfCtl0
	TC358870_i2c_Write(0x0002, 0x3F01, 2);  // SysCtl
	TC358870_i2c_Write(0x0002, 0x0000, 2);  // SysCtl
#endif
}

void Display_Reset(uint8_t deviceID)
{
	WriteLn("TC358870 DSI-TX soft power cycle");
	Toshiba_TC358870_Disable_Video_TX();
	AUO_DSI_Display_Off();
	AUO_DSI_Sleep_In();
	/// Software reset of TC358870's DSI-TX
	Toshiba_TC358870_DSITX_SW_Reset();
	Toshiba_TC358870_Prepare_TX();
	Toshiba_TC358870_Configure_Splitter();
	WriteLn("Panel setup");
	AUO_H381DLN01_Send_Panel_Init_Commands();

	if (VideoInput_Get_Status())
	{
		WriteLn("VideoInput says we have video, turning display on");
		AUO_DSI_Sleep_Out();
		AUO_DSI_Display_On();
		// seemed like a good idea.
		svr_yield_ms(10);
		Toshiba_TC358870_Enable_Video_TX();
	}
}

void Display_Powercycle(uint8_t deviceID)
{
	bool hadVideo = VideoInput_Get_Status();
	WriteLn("Display Power Cycle");
	Toshiba_TC358870_Disable_Video_TX();
	AUO_DSI_Display_Off();
	AUO_DSI_Sleep_In();
	// just to be sure we've made the timings.
	svr_yield_ms(10);

	// Pull reset low.
	ioport_set_pin_low(PANEL_RESET);

	svr_yield_ms(1000);

	// pull reset high again
	ioport_set_pin_high(PANEL_RESET);

	// Reset after this signal takes at most 5ms during sleep mode, 120ms during non-sleep mode (and can't "sleep out"
	// for 120ms)
	svr_yield_ms(120);

	AUO_H381DLN01_Send_Panel_Init_Commands();
	if (hadVideo)
	{
		AUO_DSI_Sleep_Out();
		AUO_DSI_Display_On();

		Toshiba_TC358870_Enable_Video_TX();
	}
}

#endif

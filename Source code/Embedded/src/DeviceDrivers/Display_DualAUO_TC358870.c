/*
 * Display_DualAUO_TC358870.c
 *
 * Created: 7/19/2016 9:40:29 AM
 *  Author: Coretronic, Sensics
 */

#include "GlobalOptions.h"

#if defined(SVR_IS_HDK_20)
#include "Display.h"
#include "Console.h"
#include "SvrYield.h"

#include "DeviceDrivers/Toshiba_TC358870.h"
#include <libhdk20.h>

// whether to send the sleep in/out and display on/off commands with a parameter of 0x00, or with no parameter. Appears
// to work both ways, docs are contradictory: AUO app note says use a parameter, standard says don't
#undef SLEEP_HAS_NULL_PARAMETER

static void tc358870_mystery_setup_commands(void);
static void AUO_H381DLN01_Send_Panel_Init_Commands(void);

typedef struct CommandSequenceElt
{
	uint8_t addr;
	uint8_t param;
} CommandSequenceElt_t;

static const CommandSequenceElt_t AUO_H381DLN01_Init_Commands[] = {
    {0xFE, 0x07}, {0x00, 0xEC}, {0x0B, 0xEC}, {0x16, 0xEC}, {0x21, 0xEC}, {0x2D, 0xEC}, {0xA9, 0xBA}, {0xAB, 0x06},
    {0xBB, 0x84}, {0xBC, 0x1C}, {0xFE, 0x08}, {0x07, 0x1A}, {0xFE, 0x0A}, {0x2A, 0x1B}, {0xFE, 0x0D}, {0x02, 0x65},
    {0x4D, 0x41}, {0x4B, 0x0F}, {0x53, 0xFE}, {0xFE, 0x00}, {0xC2, 0x03}, {0x51, 0xFF}};
inline static void AUO_H381DLN01_Send_Panel_Init_Commands()
{
	/// Run all display setup commands for these panels from the app note.
	unsigned int numCommands = sizeof(AUO_H381DLN01_Init_Commands) / sizeof(AUO_H381DLN01_Init_Commands[0]);
	for (unsigned int i = 0; i < numCommands; ++i)
	{
		Toshiba_TC358870_DSI_Write_Cmd_Short_Param(AUO_H381DLN01_Init_Commands[i].addr,
		                                           AUO_H381DLN01_Init_Commands[i].param);
		svr_yield_ms(16);
	}
}
inline static void tc358870_mystery_setup_commands()
{
	/// This code was originally in "write_solomon()" in the Coretronic fork of the firmware. It would have gotten
	/// triggered once: during init_solomon_device, the call to read_solomon_id starts with a write_solomon (which would
	/// call this). The subsequent read was replaced with a dummy function returning 0, so the init_solomon_device would
	/// always fail out at that point (not receiving the ID it expected), but these two writes would have taken place.
	/// @todo why was this implementation included? what does it do?
	/// Answer - it's the first step of the panel init commands.
	TC358870_i2c_Write(0x0504, 0x0015, 2);
	TC358870_i2c_Write(0x0504, 0x07FE, 2);
}

void Display_System_Init() { Toshiba_TC358870_Init(); }
void Display_Init(uint8_t deviceID)
{
#if 0
tc358870_mystery_setup_commands();
#endif
#if 0
AUO_H381DLN01_Send_Panel_Init_Commands();
#endif
}
void Display_On(uint8_t deviceID)

{
#ifdef HDMI_VERBOSE
	WriteLn("Turning display on");
#endif

	/// @todo ugly workaround for resetting things.
	Toshiba_TC358870_Init();

	AUO_H381DLN01_Init(true);
#if 0
	/// @todo ugly workaround for resetting things.
	Display_Init(Display1);
#endif
	// display power on - wait 120 ms in case this is after a reset.
	svr_yield_ms(120);

#if 0  // Not for SSD2848
	TC358870_i2c_Write(0x0504, 0x8029, 2); // DCSCMD Long Write
	TC358870_i2c_Write(0x0504, 0x0006, 2); // Number of data
	TC358870_i2c_Write(0x0504, 0xB700, 2); // 0xB7
	TC358870_i2c_Write(0x0504, 0x0000, 2); // DCSCMD_Q
	TC358870_i2c_Write(0x0504, 0x2903, 2); // DCSCMD_Q
#endif

// Sleep Out
#ifdef SLEEP_HAS_NULL_PARAMETER
	Toshiba_TC358870_DSI_Write_Cmd_Short_Param(0x11, 0x00);
#else
	Toshiba_TC358870_DSI_Write_Cmd_Short(0x11);
#endif
	svr_yield_ms(166);  //>10 frame

// Display On
#ifdef SLEEP_HAS_NULL_PARAMETER
	Toshiba_TC358870_DSI_Write_Cmd_Short_Param(0x29, 0x00);
#else
	Toshiba_TC358870_DSI_Write_Cmd_Short(0x29);
#endif
}

void Display_Off(uint8_t deviceID)
{
#ifdef HDMI_VERBOSE
	WriteLn("Turning display off");
#endif
#if 0  // Not for SSD2848
	// video mode off
    TC358870_i2c_Write(0x0504, 0x8029, 2); // DCSCMD Long Write
    TC358870_i2c_Write(0x0504, 0x0006, 2); // Number of data
    TC358870_i2c_Write(0x0504, 0xB700, 2); // 0xB7
    TC358870_i2c_Write(0x0504, 0x0000, 2); // DCSCMD_Q
    TC358870_i2c_Write(0x0504, 0x2103, 2); // DCSCMD_Q
	svr_yield_ms(16);
#endif

// Display Off
#ifdef SLEEP_HAS_NULL_PARAMETER
	Toshiba_TC358870_DSI_Write_Cmd_Short_Param(0x28, 0x00);
#else
	Toshiba_TC358870_DSI_Write_Cmd_Short(0x28);
#endif
	svr_yield_ms(16);

// Sleep in
#ifdef SLEEP_HAS_NULL_PARAMETER
	Toshiba_TC358870_DSI_Write_Cmd_Short_Param(0x10, 0x00);
#else
	Toshiba_TC358870_DSI_Write_Cmd_Short(0x10);
#endif
	svr_yield_ms(16);

	/// @todo ugly workaround for resetting things.
	Toshiba_TC358870_Init();

#if 0
	TC358870_i2c_Write(0x0004, 0x0004, 2);  // ConfCtl0
	TC358870_i2c_Write(0x0002, 0x3F01, 2);  // SysCtl
	TC358870_i2c_Write(0x0002, 0x0000, 2);  // SysCtl
#endif
}

void Display_Reset(uint8_t deviceID)
{
	/// Note: essentially the same as VideoInput_Reset - since one chip does both ends!
	Toshiba_TC358870_Trigger_Reset();

	/// @todo we could actually panel reset here...
}

void Display_Powercycle(uint8_t deviceID)
{
	WriteLn("Display Power Cycle");
	// Display Off
	TC358870_i2c_Write(0x0504, 0x0015, 2);
	TC358870_i2c_Write(0x0504, 0x0028, 2);

	svr_yield_ms(120);

	//  Sleep In
	TC358870_i2c_Write(0x0504, 0x0005, 2);
	TC358870_i2c_Write(0x0504, 0x0010, 2);

	svr_yield_ms(1000);

	// Exit Sleep
	TC358870_i2c_Write(0x0504, 0x0005, 2);
	TC358870_i2c_Write(0x0504, 0x0011, 2);

	svr_yield_ms(166);  //>10 frame

	// Display On
	TC358870_i2c_Write(0x0504, 0x0015, 2);
	TC358870_i2c_Write(0x0504, 0x0029, 2);
}

#endif

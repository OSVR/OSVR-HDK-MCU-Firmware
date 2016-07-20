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
static void tc358870_mystery_setup_commands();

inline static void tc358870_mystery_setup_commands()
{
	/// This code was originally in "write_solomon()" in the Coretronic fork of the firmware. It would have gotten
	/// triggered once: during init_solomon_device, the call to read_solomon_id starts with a write_solomon (which would
	/// call this). The subsequent read was replaced with a dummy function returning 0, so the init_solomon_device would
	/// always fail out at that point (not receiving the ID it expected), but these two writes would have taken place.
	/// @todo why was this implementation included? what does it do?
	TC358870_i2c_Write(0x0504, 0x0015, 2);
	TC358870_i2c_Write(0x0504, 0x07FE, 2);
}

void Display_System_Init() { Toshiba_TC358870_Init(); }
void Display_Init(uint8_t deviceID) { tc358870_mystery_setup_commands(); }
void Display_On(uint8_t deviceID)

{
	// delay_ms(500);
	WriteLn("Turning display on");

	// display power on
	svr_yield_ms(20);
#if 0
	// Exit Sleep
	TC358870_i2c_Write(0x0504, 0x0005, 2);
	TC358870_i2c_Write(0x0504, 0x0011, 2);
	svr_yield_ms(33);
#endif
#if 0  // Not for SSD2848
	TC358870_i2c_Write(0x0504, 0x8029, 2); // DCSCMD Long Write
	TC358870_i2c_Write(0x0504, 0x0006, 2); // Number of data
	TC358870_i2c_Write(0x0504, 0xB700, 2); // 0xB7
	TC358870_i2c_Write(0x0504, 0x0000, 2); // DCSCMD_Q
	TC358870_i2c_Write(0x0504, 0x2903, 2); // DCSCMD_Q
#endif
	svr_yield_ms(166);  //>10 frame

	// Display On
	TC358870_i2c_Write(0x0504, 0x0005 /*0x0015*/, 2);
	TC358870_i2c_Write(0x0504, 0x0029, 2);
}

void Display_Off(uint8_t deviceID)
{
	WriteLn("Turning display off");
#if 0  // Not for SSD2848
	// video mode off
    TC358870_i2c_Write(0x0504, 0x8029, 2); // DCSCMD Long Write
    TC358870_i2c_Write(0x0504, 0x0006, 2); // Number of data
    TC358870_i2c_Write(0x0504, 0xB700, 2); // 0xB7
    TC358870_i2c_Write(0x0504, 0x0000, 2); // DCSCMD_Q
    TC358870_i2c_Write(0x0504, 0x2103, 2); // DCSCMD_Q
	svr_yield_ms(16);
#endif

#if 1
	// Display Off
	TC358870_i2c_Write(0x0504, 0x0005 /*0x0015*/, 2);
	TC358870_i2c_Write(0x0504, 0x0028, 2);
	svr_yield_ms(16);

#if 0        
	//  Sleep In
	TC358870_i2c_Write(0x0504, 0x0005, 2);
	TC358870_i2c_Write(0x0504, 0x0010, 2);
	svr_yield_ms(20); // delay > 1 frames
#endif
#else
	TC358870_i2c_Write(0x0004, 0x0004, 2);  // ConfCtl0
	TC358870_i2c_Write(0x0002, 0x3F01, 2);  // SysCtl
	TC358870_i2c_Write(0x0002, 0x0000, 2);  // SysCtl
#endif
}

void Display_Reset(uint8_t deviceID)
{
	/// Note: essentially the same as VideoInput_Reset - since one chip does both ends!
	Toshiba_TC358870_Trigger_Reset();
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

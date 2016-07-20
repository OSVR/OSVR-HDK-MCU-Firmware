/*
 * Display_DualAUO_TC358870.c
 *
 * Created: 7/19/2016 9:40:29 AM
 *  Author: Sensics - based on Coretronic modifications
 */

#include "GlobalOptions.h"

#if defined(SVR_IS_HDK_20)
#include "Display.h"

#include <delay.h>
#include <libhdk20.h>

static uint8_t tc_data;

void Display_System_Init()
{
	// Dennis Yeh 2016/03/14 : for TC358870
	TC358870_i2c_Init();
	TC358870_i2c_Read(0x0000, &tc_data);

	PowerOnSeq();
}

void Display_Init(DisplayId id) {}
/// @todo why was this implementation included?
#if 0
void write_solomon(uint8_t channel, uint8_t address, uint16_t data)
{
	
	TC358870_i2c_Write(0x0504,0x0015, 2);
	TC358870_i2c_Write(0x0504,0x07FE, 2);
}
#endif

void Display_On(uint8_t deviceID)

{
	// delay_ms(500);
	// WriteLn("Turning display on");

	// display power on
	delay_ms(20);
#if 0
	// Exit Sleep
	TC358870_i2c_Write(0x0504, 0x0005, 2);
	TC358870_i2c_Write(0x0504, 0x0011, 2);
	delay_ms(33);
#endif
#if 0  // Not for SSD2848
	TC358870_i2c_Write(0x0504, 0x8029, 2); // DCSCMD Long Write
	TC358870_i2c_Write(0x0504, 0x0006, 2); // Number of data
	TC358870_i2c_Write(0x0504, 0xB700, 2); // 0xB7
	TC358870_i2c_Write(0x0504, 0x0000, 2); // DCSCMD_Q
	TC358870_i2c_Write(0x0504, 0x2903, 2); // DCSCMD_Q
#endif
	delay_ms(166);  //>10 frame

	// Display On
	TC358870_i2c_Write(0x0504, 0x0005 /*0x0015*/, 2);
	TC358870_i2c_Write(0x0504, 0x0029, 2);
}

void Display_Off(uint8_t deviceID)
{
#if 0  // Not for SSD2848
	// video mode off
    TC358870_i2c_Write(0x0504, 0x8029, 2); // DCSCMD Long Write
    TC358870_i2c_Write(0x0504, 0x0006, 2); // Number of data
    TC358870_i2c_Write(0x0504, 0xB700, 2); // 0xB7
    TC358870_i2c_Write(0x0504, 0x0000, 2); // DCSCMD_Q
    TC358870_i2c_Write(0x0504, 0x2103, 2); // DCSCMD_Q
	delay_ms(16);
#endif

#if 1
	// Display Off
	TC358870_i2c_Write(0x0504, 0x0005 /*0x0015*/, 2);
	TC358870_i2c_Write(0x0504, 0x0028, 2);
	delay_ms(16);

#if 0        
			//  Sleep In
			TC358870_i2c_Write(0x0504, 0x0005, 2);
			TC358870_i2c_Write(0x0504, 0x0010, 2);
			delay_ms(20); // delay > 1 frames
#endif
#else
	TC358870_i2c_Write(0x0004, 0x0004, 2);  // ConfCtl0
	TC358870_i2c_Write(0x0002, 0x3F01, 2);  // SysCtl
	TC358870_i2c_Write(0x0002, 0x0000, 2);  // SysCtl
#endif
}

void Display_Powercycle(uint8_t deviceID)
{
	// Display Off
	TC358870_i2c_Write(0x0504, 0x0015, 2);
	TC358870_i2c_Write(0x0504, 0x0028, 2);

	delay_ms(120);

	//  Sleep In
	TC358870_i2c_Write(0x0504, 0x0005, 2);
	TC358870_i2c_Write(0x0504, 0x0010, 2);

	delay_ms(1000);

	// Exit Sleep
	TC358870_i2c_Write(0x0504, 0x0005, 2);
	TC358870_i2c_Write(0x0504, 0x0011, 2);

	delay_ms(166);  //>10 frame

	// Display On
	TC358870_i2c_Write(0x0504, 0x0015, 2);
	TC358870_i2c_Write(0x0504, 0x0029, 2);
}

#endif

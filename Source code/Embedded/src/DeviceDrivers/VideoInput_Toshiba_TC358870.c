/*
 * VideoInput_Toshiba_TC358870.c
 *
 * Created: 7/21/2016 8:03:50 AM
 *  Author: Sensics - based on Coretronic modifications
 */

#include "GlobalOptions.h"

#ifdef SVR_HAVE_TOSHIBA_TC358870

#include "VideoInput_Protected.h"
#include "DeviceDrivers/Display.h"
#include "DeviceDrivers/HDK2.h"
#include "DeviceDrivers/Toshiba_TC358870.h"
#include "DeviceDrivers/Toshiba_TC358870_ISR.h"
#include "my_hardware.h"
#include "Console.h"
#include "SvrYield.h"
#include "BitUtilsC.h"

#include <stdio.h>     // for sprintf
#include <inttypes.h>  // for stdint.h-matching format specifier macros

#define SVR_DEBUG_LIBHDK2_BEHAVIOR

static void VideoInput_Init_Impl(void)
{
	/// Check for video input right away
	VideoInput_Protected_Report_Status(Toshiba_TC358870_Have_Video_Sync());
	/// Turn on interrupts.
	Toshiba_TC358870_Enable_HDMI_Sync_Status_Interrupts();
}
void VideoInput_Init(void)
{
	static bool haveInit = false;
	if (!haveInit)
	{
		// This is real-deal, first time initialization.
		haveInit = true;
		// start the chip, if it hasn't been started.
		Toshiba_TC358870_Init_Once();

		VideoInput_Init_Impl();
	}
	else
	{
		// This is a repeat init - presumably from serial console - so we'll actually call over to the TC358870 driver
		// (used in Display_System_Init()) since that's where the meat of initializing the chip happens.
		// Toshiba_TC358870_Base_Init();
		bool haveVideo = VideoInput_Get_Status();

		Toshiba_TC358870_Disable_Video_TX();
		// software reset HDMI
		Toshiba_TC358870_HDMI_SW_Reset();

		Toshiba_TC358870_HDMI_Setup();

		VideoInput_Init_Impl();
		if (haveVideo)
		{
			Toshiba_TC358870_Enable_Video_TX();
		}
	}

	VideoInput_Protected_Init_Succeeded();
}

void VideoInput_Update_Resolution_Detection(void)
{
	// Using the latest received status (instead of polling the chip again) to determine the status.
	/// @todo Assuming that these devices only take in landscape.
	HDMIStatus = (VideoInput_Get_Status() ? VIDSTATUS_VIDEO_LANDSCAPE : VIDSTATUS_NOVIDEO);
}

static volatile bool s_gotVideoInterrupt = false;
TC358870_ISR()
{
	s_gotVideoInterrupt = true;
	Toshiba_TC358870_MCU_Ints_Clear_Flag();
}
/// Get the state of the "got interrupt" flag atomically.
static bool gotVideoInterrupt(void)
{
	bool gotInterrupt;
	{
		Toshiba_TC358870_MCU_Ints_Suspend();
		gotInterrupt = s_gotVideoInterrupt;
		s_gotVideoInterrupt = false;
		barrier();
		Toshiba_TC358870_MCU_Ints_Resume();
	}
	return gotInterrupt;
}

void VideoInput_Task(void)
{
	/// Get the state of the "got interrupt" flag atomically.
	bool gotInterrupt = gotVideoInterrupt();
	if (gotInterrupt)
	{
		WriteLn("Got a video sync change interrupt!");
		char myMessage[50];
		uint8_t data = 0;
		Toshiba_TC358870_I2C_Read8(0x8520, &data);
		sprintf(myMessage, "System status reg 0x8520: %#04x", data);
		WriteLn(myMessage);

		// Retrieve the new video sync status.
		VideoInput_Protected_Report_Status(Toshiba_TC358870_Have_Video_Sync());
		// Clear the interrupt flag on the toshiba chip.
		Toshiba_TC358870_Clear_HDMI_Sync_Change_Int();
	}
}

void VideoInput_Reset(uint8_t inputId)
{
	if (inputId != 1)
	{
		WriteLn("Wrong HDMI num");
		return;
	}

	WriteLn("reset HDMI1");
	Toshiba_TC358870_Trigger_Reset();
	VideoInput_Poll_Status();
}

static const char LIBHDK2_NOT_SUPPORTED[] = "TC358870 via libhdk20 does not support this feature.";

void VideoInput_Suspend(void) { WriteLn(LIBHDK2_NOT_SUPPORTED); }
void VideoInput_Resume(void) { WriteLn(LIBHDK2_NOT_SUPPORTED); }
void VideoInput_Poll_Status(void) { VideoInput_Protected_Report_Status(Toshiba_TC358870_Have_Video_Sync()); }
#define VITC_GETBIT(VAL, BIT) ((uint8_t)(((VAL & BITUTILS_BIT(BIT)) >> (BIT))))

static inline void vi_tc_dump_reg_status(uint16_t reg, uint8_t bytes)
{
	char myMessage[50];
	sprintf(myMessage, "TC358870 reg %#06" PRIX16 ": ", reg);
	Write(myMessage);

	uint32_t data = 0;
	Toshiba_TC358870_I2C_Read32(reg, &data);

	// access through a character pointer is OK even with strict aliasing.
	unsigned char* buf = (unsigned char*)(&data);

	switch (bytes)
	{
	case 1:
	{
		sprintf(myMessage, " (as 8-bit hex %#04" PRIx8 ") ", buf[0]);
		Write(myMessage);
		break;
	}
	case 2:
	{
		// not violating strict aliasing...
		uint16_t justMyData;
		unsigned char* myBuf = (unsigned char*)(&justMyData);
		myBuf[0] = buf[0];
		myBuf[1] = buf[1];

		sprintf(myMessage, " (as 16-bit hex %#06" PRIx16 ") ", justMyData);
		Write(myMessage);
	}

	case 4:
	{
		sprintf(myMessage, " (as 32-bit hex %#010" PRIx32 ") ", data);
		Write(myMessage);
		break;
	}
	}

	for (uint8_t i = 0; i < bytes; ++i)
	{
		unsigned char val = buf[bytes - i - 1];
		sprintf(myMessage, "%d%d%d%d %d%d%d%d ", VITC_GETBIT(val, 7), VITC_GETBIT(val, 6), VITC_GETBIT(val, 5),
		        VITC_GETBIT(val, 4), VITC_GETBIT(val, 3), VITC_GETBIT(val, 2), VITC_GETBIT(val, 1),
		        VITC_GETBIT(val, 0));
		Write(myMessage);
	}
	WriteEndl();
}

// to console
void VideoInput_Report_Status(void)
{
	Write("Video input status: ");
	if (VideoInput_Get_Status())
	{
		WriteLn("have sync.");
	}
	else
	{
		WriteLn("no sync available.");
	}
	char myMessage[50];
	sprintf(myMessage, "TC358870_Init called %d times", Toshiba_TC358870_Get_Init_Count());
	WriteLn(myMessage);

	sprintf(myMessage, "Address select/interrupt pin: %d", ioport_get_value(TC358870_ADDR_SEL_INT));
	WriteLn(myMessage);
	{
		uint16_t data = 0;
		Toshiba_TC358870_I2C_Read16(0x0000, &data);
		uint8_t highByte = (uint8_t)((data >> 8) & 0xff);
		uint8_t lowByte = (uint8_t)(data & 0xff);
		sprintf(myMessage, "Chip ID: %#04" PRIx8 " (expected 0x47)", highByte);
		WriteLn(myMessage);
		sprintf(myMessage, "Rev ID: %#04" PRIx8 " (expected 0x00)", lowByte);
		WriteLn(myMessage);
	}
	Write("System Status ");
	vi_tc_dump_reg_status(0x8520, 1);

	Write("LANE_STATUS_HS (DSI-TX0) - want 8f - ");
	vi_tc_dump_reg_status(0x0290, 4);

	Write("LANE_STATUS_LS (DSI-TX0) - want 0 - ");
	vi_tc_dump_reg_status(0x0294, 4);

	Write("MIPI_PLL_CNF (DSI-TX0) ");
	vi_tc_dump_reg_status(0x02AC, 4);

	Write("FUNC_MODE (DSI-TX0) ");
	vi_tc_dump_reg_status(0x0150, 4);
}
#endif

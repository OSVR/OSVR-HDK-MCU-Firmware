/*
 * Display_Solomon.c
 *
 * Created: 7/19/2016 9:44:55 AM
 *  Author: Sensics
 */

#include "GlobalOptions.h"

#if defined(SVR_HAVE_SOLOMON)

#include "Display.h"

#include "Solomon.h"
#include "Console.h"
#include "my_hardware.h"
#include "SvrYield.h"
#include "FPGA.h"
#include "VideoInput.h"

#if defined(SVR_HAVE_SHARP_LCD)  // sharp 5" or 5.5"
//#define SVR_DISPLAY_SHOULD_TURN_OFF_AFTER_INIT
#endif  // defined(SVR_HAVE_SHARP_LCD)

#ifdef SVR_DISPLAY_SHOULD_TURN_OFF_AFTER_INIT
#include "VideoInput.h"
#endif  // SVR_DISPLAY_SHOULD_TURN_OFF_AFTER_INIT

void Display_System_Init()
{
#ifdef SVR_HAVE_SHARP_LCD
	if (!VideoInput_Get_Status())
	{
		FPGA_start_reset();
	}
	else
	{
		FPGA_reset();
	}
#endif  // SVR_HAVE_SHARP_LCD

	init_solomon();
}

/// Pulls panel reset line appropriately to start panel reset, if possible.
/// Also begins reset of corresponding solomon device.
/// @param deviceID 0-indexed panel ID
static void Display_Internal_Reset_Begin(uint8_t deviceID);
/// Pulls panel reset line appropriately to end panel reset, if possible.
/// Also ends reset of corresponding solomon device.
/// @param deviceID 0-indexed panel ID
/// @return true if device was actually in reset.
static bool Display_Internal_Reset_End(uint8_t deviceID);

static bool s_inReset[SVR_HAVE_SOLOMON] =
#ifdef SVR_HAVE_DISPLAY2
    {true, true};
#else
    {true};
#endif
#ifdef SVR_PANEL_RESET_PINS
static port_pin_t s_resetPins[] = SVR_PANEL_RESET_PINS;

static inline void Display_Internal_Reset_Begin(uint8_t deviceID)
{
	s_inReset[deviceID] = true;
	ioport_set_pin_level(s_resetPins[deviceID], SVR_PANEL_RESET_VALUE);
	solomon_start_reset(solomon_get_channel(deviceID));
}

static inline bool Display_Internal_Reset_End(uint8_t deviceID)
{
	const bool wasInReset = s_inReset[deviceID];
	ioport_set_pin_level(s_resetPins[deviceID], SVR_PANEL_RESET_VALUE);
	solomon_end_reset(solomon_get_channel(deviceID));
	s_inReset[deviceID] = false;
	return wasInReset;
}
#else
inline void Display_Internal_Reset_Begin(uint8_t deviceID)
{
	/* no-op - access to reset line not provided */
	s_inReset[deviceID] = true;
}
inline bool Display_Internal_Reset_End(uint8_t deviceID)
{
	/* no-op - access to reset line not provided */
	const bool wasInReset = s_inReset[deviceID];
	s_inReset[deviceID] = false;
	return wasInReset;
}
#endif  // SVR_PANEL_RESET_PINS

static inline void WriteDigitPlus1(uint8_t digit)
{
	const char str[] = {(digit > 8 ? '?' : (digit + '1')), '\0'};
	Write(str);
}

static inline void WriteDigit(uint8_t digit)
{
	const char str[] = {(digit > 9 ? '?' : (digit + '0')), '\0'};
	Write(str);
}

void Display_Init(uint8_t deviceID)
{
	Write("Init for display ");
	WriteDigitPlus1(deviceID);
	WriteEndl();

	/// Reset the solomon and the panel.
	Display_Internal_Reset_Begin(deviceID);
	svr_yield_ms(100);
	Display_Internal_Reset_End(deviceID);

	svr_yield_ms(100);
	init_solomon_device(deviceID);
	svr_yield_ms(100);
	Solomon_t *sol = solomon_get_channel(deviceID);
	solomon_end_video_shutdown(sol);
#ifdef SVR_HAVE_SHARP_LCD

	solomon_select(sol);
	solomon_cfgr_set_clear_bits(sol, SOLOMON_CFGR_DCS_bm, 0);  // Set DCS bit.

	solomon_write_reg_word(sol, SOLOMON_REG_PSCR1, 0x0001);     // no of bytes to send
	solomon_write_reg_word(sol, SOLOMON_REG_VCR, 0x0000);       // VC
	solomon_write_reg_2byte(sol, SOLOMON_REG_PDR, 0x28, 0x00);  // display off
	svr_yield_ms(20);
	solomon_write_reg_2byte(sol, SOLOMON_REG_PDR, 0x10, 0x00);  // sleep in
	svr_yield_ms(80);
	// Clear VEN and HS bits.
	solomon_cfgr_set_clear_bits(sol, 0x0, SOLOMON_CFGR_VEN_bm | SOLOMON_CFGR_HS_bm);  // Set VEN and HS bits.
	solomon_deselect(sol);
#endif
#ifdef SVR_DISPLAY_SHOULD_TURN_OFF_AFTER_INIT
	if (!VideoInput_Get_Status())
	{
		Display_Off(deviceID);
	}
#endif  // SVR_DISPLAY_SHOULD_TURN_OFF_AFTER_INIT
}

void Display_On(uint8_t deviceID)
{
	Write("Turning display ");
	WriteDigitPlus1(deviceID);
	WriteLn(" on");
#if defined(SVR_HAVE_SHARP_LCD)  // sharp 5" or 5.5"
	Display_Internal_Reset_Begin(deviceID);
	svr_yield_ms(10);  // at least 1
	FPGA_end_reset();
	svr_yield_ms(500);
	Display_Internal_Reset_End(deviceID);
	svr_yield_ms(100);  // at least 3

	WriteLn("Solomon re-initializing...");
	if (!init_solomon_device(deviceID))
	{
		// failed to re-init!
		return;
	}
#endif  // defined(SVR_HAVE_SHARP_LCD)

	Display_Internal_Reset_End(deviceID);
	svr_yield_ms(100);
	Solomon_Dump_Config_Debug(deviceID, "Display_On - before");
#ifdef SVR_HAVE_SHARP_LCD
	Solomon_t *sol = solomon_get_channel(deviceID);
	solomon_select(sol);

	/// @todo investigate if turning on VEN here improves reliability, done in some related drivers.
	solomon_cfgr_set_clear_bits(sol, SOLOMON_CFGR_DCS_bm, SOLOMON_CFGR_VEN_bm | SOLOMON_CFGR_HS_bm);  // Set DCS bit.

	solomon_write_reg_word(sol, SOLOMON_REG_PSCR1, 0x0001);     // no of bytes to send
	solomon_write_reg_word(sol, SOLOMON_REG_VCR, 0x0000);       // VC
	solomon_write_reg_2byte(sol, SOLOMON_REG_PDR, 0x29, 0x00);  // display on
	svr_yield_ms(120);
	solomon_write_reg_2byte(sol, SOLOMON_REG_PDR, 0x11, 0x00);  // sleep out
	svr_yield_ms(250);

	solomon_cfgr_set_clear_bits(sol, SOLOMON_CFGR_VEN_bm | SOLOMON_CFGR_HS_bm, 0x0);  // Set VEN and HS bits.
	solomon_deselect(sol);
#endif
#ifdef H546DLT01  // AUO 5.46" OLED
	// svr_yield_ms(500);

	// display power on
	svr_yield_ms(20);

	// initial setting
	/// @todo why are we setting bytes to 2 here? Shouldn't it be 1?
	write_solomon(deviceID, SOLOMON_REG_PSCR1, 0x0002);  // no of byte send

	write_solomon(deviceID, SOLOMON_REG_PDR, 0x0011);  // sleep out
	svr_yield_ms(33);
	write_solomon(deviceID, SOLOMON_REG_CFGR, 0x0329);  // video signal on // TX6
	svr_yield_ms(166);                                  //>10 frame
	write_solomon(deviceID, SOLOMON_REG_PDR, 0x0029);   // display on

#endif
#ifdef SVR_IS_DSIGHT
	/// @todo This is a bit of a bodge to make sure the second display reliably works.
	svr_yield_ms(10);
	FPGA_reset();
#endif
#if 0
	Solomon_Dump_Config_Debug(deviceID, "Display_On - after");
#endif
}

void Display_Off(uint8_t deviceID)
{
	Write("Turning display ");
	/// Single digit sprintf substitute.
	const char displayNum[] = {deviceID + '1', '\0'};
	Write(displayNum);
	WriteLn(" off");
#if 0
	Solomon_Dump_Config_Debug(deviceID, "Display_Off - before");
#endif
#ifdef SVR_HAVE_SHARP_LCD
	Solomon_t *sol = solomon_get_channel(deviceID);
	solomon_select(sol);
	solomon_cfgr_set_clear_bits(sol, SOLOMON_CFGR_DCS_bm, 0);  // Set DCS bit.

	solomon_write_reg_word(sol, SOLOMON_REG_PSCR1, 0x0001);     // no of bytes to send
	solomon_write_reg_word(sol, SOLOMON_REG_VCR, 0x0000);       // VC
	solomon_write_reg_2byte(sol, SOLOMON_REG_PDR, 0x28, 0x00);  // display off
	svr_yield_ms(20);
	solomon_write_reg_2byte(sol, SOLOMON_REG_PDR, 0x10, 0x00);  // sleep in
	svr_yield_ms(80);
	// Clear VEN and HS bits.
	solomon_cfgr_set_clear_bits(sol, 0x0, SOLOMON_CFGR_VEN_bm | SOLOMON_CFGR_HS_bm);  // Set VEN and HS bits.
	solomon_deselect(sol);
	svr_yield_ms(20);  // delay > 1 frames
	Display_Internal_Reset_Begin(deviceID);
	svr_yield_ms(500);  // give that some time to take effect
	FPGA_start_reset();
	svr_yield_ms(500);
// FPGA_end_reset();
#endif
#ifdef H546DLT01  // AUO 5.46" OLED

	write_solomon(deviceID, SOLOMON_REG_CFGR, 0x0321);  // video mode off // TX7

	write_solomon(deviceID, SOLOMON_REG_PSCR1, 0x0001);  // 1 byte commands
	write_solomon(deviceID, SOLOMON_REG_VCR, 0x0000);    // VC
	svr_yield_ms(16);
	write_solomon(deviceID, SOLOMON_REG_PDR, 0x0028);  // display off
	svr_yield_ms(16);
	write_solomon(deviceID, SOLOMON_REG_PDR, 0x0010);  // sleep in
	svr_yield_ms(20);                                  // delay > 1 frames

#endif
#if 0
	Solomon_Dump_Config_Debug(deviceID, "Display_Off - after");
#endif
}
void Display_Reset(uint8_t deviceID)
{
	Display_Internal_Reset_Begin(deviceID);
	Solomon_Reset(deviceID);
	Display_Internal_Reset_End(deviceID);
}
// power cycles display connected to the specific device
void Display_Powercycle(uint8_t deviceID)
{
	/// @todo set SOLOMON_REG_CFGR?
	write_solomon(deviceID, SOLOMON_REG_PSCR1, 0x0001);  // 1 byte commands
	write_solomon(deviceID, SOLOMON_REG_VCR, 0x0000);    // VC

	write_solomon(deviceID, SOLOMON_REG_PDR, 0x0028);  // display on
	svr_yield_ms(120);
	write_solomon(deviceID, SOLOMON_REG_PDR, 0x0010);  // sleep out

	svr_yield_ms(1000);

	write_solomon(deviceID, SOLOMON_REG_PDR, 0x0029);  // display on
	svr_yield_ms(120);
	write_solomon(deviceID, SOLOMON_REG_PDR, 0x0011);  // sleep out
}

void Display_Handle_Gain_Video()
{
#if 0
	for (uint8_t deviceID = 0; deviceID < SVR_HAVE_SOLOMON; ++deviceID)
	{
		Solomon_t *sol = solomon_get_channel(deviceID);
		solomon_end_video_shutdown(sol);
	}
#endif
}

void Display_Handle_Lose_Video()
{
	for (uint8_t deviceID = 0; deviceID < SVR_HAVE_SOLOMON; ++deviceID)
	{
#if 0
		Solomon_t *sol = solomon_get_channel(deviceID);
		solomon_start_video_shutdown(sol);
#endif
		Display_Internal_Reset_Begin(deviceID);
	}
}

void Display_Set_Strobing(uint8_t deviceID, uint8_t refresh, uint8_t percentage)
{
#ifdef H546DLT01  // limit to low-persistence AUO OLED panel
	Display_Strobing_Rate = refresh;
	SetConfigValue(PersistenceOffset, Display_Strobing_Rate);

	Display_Strobing_Percent = percentage;
	SetConfigValue(PersistencePercentOffset, Display_Strobing_Percent);

	/// @todo set SOLOMON_REG_CFGR? Looks like it would be an LP DCS write as found in init_solomon_device
	write_solomon(deviceID, SOLOMON_REG_PSCR1, 0x0002);  // two bytes at a time
	write_solomon(deviceID, SOLOMON_REG_VCR, 0x0000);    // VC

	// added commands to address strobing
	write_solomon(deviceID, SOLOMON_REG_PDR, 0x08fe);

	if (refresh == 60)
	{
		write_solomon(deviceID, SOLOMON_REG_PDR, 0x9889);
		switch (percentage)
		{
		case 0:
			write_solomon(deviceID, SOLOMON_REG_PDR, 0x078a);
			write_solomon(deviceID, SOLOMON_REG_PDR, 0x708b);
			break;
		case 10:
			write_solomon(deviceID, SOLOMON_REG_PDR, 0xbd8a);
			write_solomon(deviceID, SOLOMON_REG_PDR, 0x708b);
			break;
		case 20:
			write_solomon(deviceID, SOLOMON_REG_PDR, 0x808a);
			write_solomon(deviceID, SOLOMON_REG_PDR, 0x718b);
			break;
		case 30:
			write_solomon(deviceID, SOLOMON_REG_PDR, 0x438a);
			write_solomon(deviceID, SOLOMON_REG_PDR, 0x728b);
			break;
		case 40:
			write_solomon(deviceID, SOLOMON_REG_PDR, 0x068a);
			write_solomon(deviceID, SOLOMON_REG_PDR, 0x738b);
			break;
		case 50:
			write_solomon(deviceID, SOLOMON_REG_PDR, 0xcc8a);
			write_solomon(deviceID, SOLOMON_REG_PDR, 0x738b);
			break;
		case 60:
			write_solomon(deviceID, SOLOMON_REG_PDR, 0x8c8a);
			write_solomon(deviceID, SOLOMON_REG_PDR, 0x748b);
			break;
		case 70:
			write_solomon(deviceID, SOLOMON_REG_PDR, 0x8b8a);
			write_solomon(deviceID, SOLOMON_REG_PDR, 0x758b);
			break;
		case 80:
			write_solomon(deviceID, SOLOMON_REG_PDR, 0x128a);
			write_solomon(deviceID, SOLOMON_REG_PDR, 0x768b);
			break;
		case 82:
			write_solomon(deviceID, SOLOMON_REG_PDR, 0x3a8a);
			write_solomon(deviceID, SOLOMON_REG_PDR, 0x768b);
			break;
		case 90:
			write_solomon(deviceID, SOLOMON_REG_PDR, 0xd58a);
			write_solomon(deviceID, SOLOMON_REG_PDR, 0x768b);
			break;
		default:
			write_solomon(deviceID, SOLOMON_REG_PDR, 0xCC8a);
			write_solomon(deviceID, SOLOMON_REG_PDR, 0x738b);
			break;
		}
	}

	else  // refresh is 240
	{
		write_solomon(deviceID, SOLOMON_REG_PDR, 0xe689);
		switch (percentage)
		{
		case 50:
			write_solomon(deviceID, SOLOMON_REG_PDR, 0xf38a);
			write_solomon(deviceID, SOLOMON_REG_PDR, 0x108b);
			break;
		case 82:
			write_solomon(deviceID, SOLOMON_REG_PDR, 0x8e8a);
			write_solomon(deviceID, SOLOMON_REG_PDR, 0x118b);
			break;
		default:
			write_solomon(deviceID, SOLOMON_REG_PDR, 0x8e8a);
			write_solomon(deviceID, SOLOMON_REG_PDR, 0x118b);
			break;
		}
	}

	write_solomon(deviceID, SOLOMON_REG_PDR, 0x00fe);
#endif  // H546DLT01
}

#endif  // defined(SVR_HAVE_SOLOMON)

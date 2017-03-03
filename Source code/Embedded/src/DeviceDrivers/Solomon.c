/*
 * Solomon.c
 *
 * Created: 9/7/2013 11:34:25 PM
 *  Author: Sensics
 */

// driver for Solomon MIPI bridge chip
#include "GlobalOptions.h"

#ifdef SVR_HAVE_SOLOMON
/// To get internal deprecation warnings.
#define SOLOMON_IMPL

#include "Solomon.h"
#include "board.h"

#include "my_hardware.h"
#include "NXP/my_bit.h"  // for lo() and hi()

#include <spi_master.h>

#include "Console.h"
#include "SvrYield.h"

#include <stdio.h>

#include "BitUtilsC.h"

#define SOLOMON_MAX_PLL_ATTEMPTS 40

Solomon_t g_solomons[SVR_HAVE_SOLOMON] = {Solomon1_Struct
#ifdef SVR_HAVE_SOLOMON2
                                          ,
                                          Solomon2_Struct
#endif
};

void Solomon_Dump_Config_Debug_New(uint8_t deviceId, Solomon_t const *sol, const char *loc)
{
	uint16_t config = solomon_read_reg_2byte(sol, SOLOMON_REG_CFGR);
	{
		char msg[50];
		sprintf(msg, "Config %d: 0x%04x - ", deviceId, config);
		Write(msg);
	}
	{
		char confStr[] = BITUTILS_CSTR_INIT_FROM_U16_TO_BIN(config);
		Write(confStr);
	}
	{
		uint16_t status = solomon_read_reg_2byte(sol, SOLOMON_REG_ISR);
		char msg[50];
		sprintf(msg, " (ISR: 0x%04x)", status);
		Write(msg);
	}
	Write(" [");
	Write(loc);
	WriteLn("]");
}

void Solomon_Dump_Config_Debug(uint8_t deviceId, const char *loc)
{
	Solomon_t *sol = solomon_get_channel(deviceId);
	solomon_select(sol);
	Solomon_Dump_Config_Debug_New(deviceId, sol, loc);
	solomon_deselect(sol);
}

static inline void Solomon_Dump_Config_Debug_Bare(Solomon_t const *sol, const char *loc)
{
	uint16_t config = solomon_read_reg_2byte(sol, SOLOMON_REG_CFGR);
	{
		char msg[50];
		sprintf(msg, "Config  : 0x%04x - ", config);
		Write(msg);
	}
	{
		char confStr[] = BITUTILS_CSTR_INIT_FROM_U16_TO_BIN(config);
		Write(confStr);
	}
	{
		uint16_t status = solomon_read_reg_2byte(sol, SOLOMON_REG_ISR);
		char msg[50];
		sprintf(msg, " (ISR: 0x%04x)", status);
		Write(msg);
	}
	Write(" [");
	Write(loc);
	WriteLn("]");
}

static inline void Solomon_Dump_All_Config_Debug(const char *loc)
{
	for (uint8_t deviceId = 0; deviceId < SVR_HAVE_SOLOMON; ++deviceId)
	{
		Solomon_Dump_Config_Debug(deviceId, loc);
	}
}

bool init_solomon_device(uint8_t deviceID)
{
	/// 20 MHz crystal on xtal-in/xtal-io
	Solomon_t *sol = solomon_get_channel(deviceID);

	/// Reset the solomon.
	solomon_start_reset(sol);
	svr_yield_ms(10);
	solomon_end_reset(sol);
	svr_yield_ms(10);

	bool ok = solomon_detect_lrr_behavior(sol);
	if (!ok)
	{
		WriteLn("Failed to get correct readback operation for one of the solomons!");
		return false;
	}
	Solomon_Dump_All_Config_Debug("init_solomon_device - before");

	/// Select this device to communicate with.
	solomon_select(sol);

#ifndef H546DLT01
	solomon_write_reg_word(sol, 0xBA, 0xC030);  // lane speed=960Mbps
#else
	solomon_write_reg_word(sol, 0xBA, 0xC02D);  // lane speed=900Mbps
#endif
	solomon_write_reg_word(sol, 0xBB, 0x0008);  // LP clock

	Write("Trying to enable PLL...");
	solomon_pll_enable(sol);
	for (uint8_t i = 0; i < SOLOMON_MAX_PLL_ATTEMPTS && !solomon_pll_is_locked(sol); ++i)
	{
		Write(".");
		// no pll lock
		svr_yield_ms(1);
	}
	if (!solomon_pll_is_locked(sol))
	{
		WriteLn(" Failed!");
		return false;
	}
	WriteLn(" PLL locked");

#ifndef H546DLT01
	solomon_write_reg_word(sol, 0xB1, 0x0216);  // VSA=2, HSA=22
	solomon_write_reg_word(sol, 0xB2, 0x0630);  // VBP=4, HBP=26
	solomon_write_reg_word(sol, 0xB3, 0x288C);  // VFP=40, HFP=140
#else
	solomon_write_reg_word(sol, 0xB1, 0x0505);  // VSA=2, HSA=22
	solomon_write_reg_word(sol, 0xB2, 0x0760);  // VBP=7, HBP=11
	solomon_write_reg_word(sol, 0xB3, 0x0C20);  // VFP=24, HFP=32
#endif

	solomon_write_reg_word(sol, 0xB4, 0x0438);  // HACT=1080
	solomon_write_reg_word(sol, 0xB5, 0x0780);  // VACT=1920

	solomon_write_reg_word(sol, 0xB6, 0x2007);  // non burst with sync events, 24bpp

	solomon_write_reg_word(sol, 0xC9, 0x140A);  // HS prepare delay
	solomon_write_reg_word(sol, 0xDE, 0x0003);  // no of lane
	solomon_write_reg_word(sol, 0xD6, 0x0004);  // packet number in blanking period
#ifndef H546DLT01
	solomon_write_reg_word(sol, 0xC4, 0x0001);  // auto BTA
#endif

	svr_yield_ms(50);

	// module panel initialization
	/// This line sets EOT, ECD, and CKE.
	solomon_write_reg_word(sol, SOLOMON_REG_CFGR, 0x0302);  // LP generic write // TX1
	solomon_write_reg_word(sol, SOLOMON_REG_VCR, 0x0000);   // VC

#ifdef LS055T1SX01                                           // sharp 5.5"
	solomon_write_reg_word(sol, SOLOMON_REG_PSCR1, 0x0003);  // no of bytes to send
	/// @todo can we do this in a single write or should we do it in two?
	static const uint8_t brightnessStartupCmd[] = {0x51, 0x0f, 0xff, 0x00};  // cmd=51, data1 = 0F, data2=ff LEDPWM 100%
	solomon_write_reg(sol, SOLOMON_REG_PDR, brightnessStartupCmd, sizeof(brightnessStartupCmd));

	solomon_write_reg_word(sol, SOLOMON_REG_PSCR1, 0x0002);  // no of bytes to send
	solomon_write_reg_word(sol, SOLOMON_REG_PDR, 0x0453);    // cmd=53, data=04 LEDPWM ON
	solomon_write_reg_word(sol, SOLOMON_REG_PDR, 0x0055);    // cmd=55, data=00 CABC OFF
#if 1
	solomon_cfgr_set_clear_bits(sol, SOLOMON_CFGR_DCS_bm, 0x0);  // Set DCS bit.
#else
	solomon_write_reg_word(sol, SOLOMON_REG_CFGR, 0x0342);  // LP DCS write // TX2
#endif
	solomon_write_reg_word(sol, SOLOMON_REG_VCR, 0x0000);  // VC

	solomon_write_reg_word(sol, SOLOMON_REG_PSCR1, 0x0001);  //
	solomon_write_reg_word(sol, SOLOMON_REG_PDR, 0x0029);    // display on
	svr_yield_ms(120);
	solomon_write_reg_word(sol, SOLOMON_REG_PDR, 0x0011);  // sleep out
#endif

#ifdef LS050T1SX01  // sharp 5"
	// from LS050T1SX01 data sheet
	solomon_write_reg_word(sol, SOLOMON_REG_PSCR1, 0x0002);  // no of bytes to send
	solomon_write_reg_word(sol, SOLOMON_REG_PDR, 0x04B0);    // cmd=B0, data=04
	solomon_write_reg_word(sol, SOLOMON_REG_PDR, 0x01D6);    // cmd=D6, data=01

#if 1
	solomon_cfgr_set_clear_bits(sol, SOLOMON_CFGR_DCS_bm, 0x0);  // Set DCS bit.
#else
	solomon_write_reg_word(sol, SOLOMON_REG_CFGR, 0x0342);  // LP DCS write // TX3
#endif
	solomon_write_reg_word(sol, SOLOMON_REG_VCR, 0x0000);    // VC
	solomon_write_reg_word(sol, SOLOMON_REG_PSCR1, 0x0003);  // no of bytes to send

	/// @todo can we do this in a single write or should we do it in two?
	static const uint8_t brightnessStartupCmd[] = {0x51, 0x0f, 0xff, 0x00};
	solomon_write_reg(sol, SOLOMON_REG_PDR, brightnessStartupCmd, sizeof(brightnessStartupCmd));

	solomon_write_reg_word(sol, SOLOMON_REG_PSCR1, 0x0002);  // no of bytes to send
	// solomon_write_reg_word(sol,0xBB,0x0008); // LP clock BC 0002
	solomon_write_reg_word(sol, SOLOMON_REG_PDR, 0x0453);  // cmd=53, data=04

	solomon_write_reg_word(sol, SOLOMON_REG_PSCR1, 0x0001);  // 1 byte
	solomon_write_reg_word(sol, SOLOMON_REG_PDR, 0x0029);    // display on
	svr_yield_ms(120);
	solomon_write_reg_word(sol, SOLOMON_REG_PDR, 0x0011);  // sleep out

// end of LS050T1SX01 data sheet
#endif

#ifdef H546DLT01  // AUO 5.46" OLED
	// from LS050T1SX01 data sheet
	solomon_write_reg_word(sol, SOLOMON_REG_PSCR1, 0x0002);  // no of bytes to send
#if 1
	/// @todo also sets CSS!
	solomon_cfgr_set_clear_bits(sol, SOLOMON_CFGR_DCS_bm | SOLOMON_CFGR_HS_bm,
	                            SOLOMON_CFGR_CKE_bm);  // Set DCS, HS, clear CKE bit.
#else
	/// @todo not actually LP - setting HS flag!
	solomon_write_reg_word(sol, SOLOMON_REG_CFGR, 0x0321);  // LP DCS write // TX4
#endif
	solomon_write_reg_word(sol, SOLOMON_REG_VCR, 0x0000);  // VC

	solomon_write_reg_word(sol, SOLOMON_REG_PSCR1, 0x0002);  // number of bytes to write
	svr_yield_ms(100);

#ifdef LOW_PERSISTENCE
	solomon_write_reg_word(sol, SOLOMON_REG_PDR, 0x08FE);  // cmd=FE, data=08
	solomon_write_reg_word(sol, SOLOMON_REG_PDR, 0x4003);  // cmd=03, data=40
	solomon_write_reg_word(sol, SOLOMON_REG_PDR, 0x1A07);  // cmd=07, data=1A
	/*Add by AUO*/
	solomon_write_reg_word(sol, SOLOMON_REG_PDR, 0x0DFE);  // cmd=FE, data=0D
	solomon_write_reg_word(sol, SOLOMON_REG_PDR, 0xFE53);  // cmd=53, data=FE
	/* -----*/
	solomon_write_reg_word(sol, SOLOMON_REG_PDR, 0x00FE);  // cmd=FE, data=00
	solomon_write_reg_word(sol, SOLOMON_REG_PDR, 0x08C2);  // cmd=C2, data=08
	solomon_write_reg_word(sol, SOLOMON_REG_PDR, 0xFF51);  // cmd=51, data=FF

	// solomon_write_reg_word(sol,SOLOMON_REG_PDR,0x9889);
	// solomon_write_reg_word(sol,SOLOMON_REG_PDR,0x068a);
	// solomon_write_reg_word(sol,SOLOMON_REG_PDR,0x708b);

	// removed for test
	// solomon_write_reg_word(sol,SOLOMON_REG_PDR,0xE689);
	// solomon_write_reg_word(sol,SOLOMON_REG_PDR,0x8E8a);
	// solomon_write_reg_word(sol,SOLOMON_REG_PDR,0x118b);

	Display_Set_Strobing(deviceID, Display_Strobing_Rate, Display_Strobing_Percent);

	solomon_write_reg_word(sol, SOLOMON_REG_PDR, 0xFF51);  // cmd=FE, data=08

#else
	solomon_write_reg_word(sol, SOLOMON_REG_PDR, 0x04FE);   // cmd=FE, data=04
	svr_yield_ms(16);
	solomon_write_reg_word(sol, SOLOMON_REG_PDR, 0x005E);
	svr_yield_ms(16);
	solomon_write_reg_word(sol, SOLOMON_REG_PDR, 0x4744);
	svr_yield_ms(16);
	solomon_write_reg_word(sol, SOLOMON_REG_PDR, 0x07FE);
	svr_yield_ms(16);
	solomon_write_reg_word(sol, SOLOMON_REG_PDR, 0x6AA9);
	svr_yield_ms(16);
	solomon_write_reg_word(sol, SOLOMON_REG_PDR, 0x0AFE);
	svr_yield_ms(16);
	solomon_write_reg_word(sol, SOLOMON_REG_PDR, 0x5214);
	svr_yield_ms(16);
	solomon_write_reg_word(sol, SOLOMON_REG_PDR, 0x00FE);
	svr_yield_ms(16);
	solomon_write_reg_word(sol, SOLOMON_REG_PDR, 0x0135);
	svr_yield_ms(16);
	solomon_write_reg_word(sol, SOLOMON_REG_PDR, 0x0055);
	svr_yield_ms(16);
#endif
#if 0
	solomon_write_reg_word(sol, SOLOMON_REG_PSCR1, 0x0001);  //
	delay_ms(16);
	solomon_write_reg_word(sol, SOLOMON_REG_PDR, 0x0011);    // sleep out
	delay_ms(170);                                           // delay > 10 frames
	solomon_write_reg_word(sol, SOLOMON_REG_PDR, 0x0029);    // display on
	delay_ms(20);                                            // delay > 1 frames
	solomon_write_reg_word(sol, SOLOMON_REG_PSCR1, 0x0002);  // number of bytes to write
	solomon_write_reg_word(sol, SOLOMON_REG_PDR, 0x07FE);    // cmd=FE, data=BF
	delay_ms(100);                                           // delay > 5 frames
	solomon_write_reg_word(sol, SOLOMON_REG_PDR, 0xFAA9);
	delay_ms(16);
	solomon_write_reg_word(sol, SOLOMON_REG_PDR, 0x00FE);
	delay_ms(16);
	solomon_write_reg_word(sol, SOLOMON_REG_PSCR1, 0x0001);  //
	delay_ms(16);
#endif  // 0
#endif

#ifndef H546DLT01                                          // AUO 5.46" OLED
	                                                       // video mode on
	svr_yield_ms(250);

	solomon_write_reg_word(sol, SOLOMON_REG_CFGR, 0x034B);  // video mode on // TX5
	svr_yield_ms(100);
#endif

	Solomon_Dump_All_Config_Debug("init_solomon_device - after");
	solomon_deselect(sol);
	return true;
}

void init_solomon(void)
{
	for (uint8_t i = 0; i < SVR_HAVE_SOLOMON; ++i)
	{
		Solomon_t *sol = solomon_get_channel(i);
		solomon_start_reset(sol);
		svr_yield_ms(10);
		solomon_end_reset(sol);
		svr_yield_ms(10);
		solomon_init(sol);
	}
}

uint16_t read_solomon(uint8_t channel, uint8_t address)
{
	Solomon_t *sol = solomon_get_channel(channel);
	solomon_select(sol);
	uint16_t data = solomon_read_reg_2byte(sol, address);
	solomon_deselect(sol);
	return data;
}

void write_solomon(uint8_t channel, uint8_t address, uint16_t data)
{
	Solomon_t *sol = solomon_get_channel(channel);
	solomon_select(sol);
	solomon_write_reg_word(sol, address, data);
	solomon_deselect(sol);
}

void write_solomon_pair(uint8_t channel, uint8_t address, uint16_t data1, uint16_t data2)
{
	Solomon_t const *sol = solomon_get_channel(channel);
	solomon_select(sol);
	uint8_t data[] = {lo(data1), hi(data1), lo(data2), hi(data2)};
	solomon_write_reg(sol, address, data, sizeof(data));
	solomon_deselect(sol);
}

uint16_t read_Solomon_ID(uint8_t channel)
{
	Solomon_t *sol = solomon_get_channel(channel);
	solomon_select(sol);
	uint16_t id = solomon_read_reg_2byte(sol, SOLOMON_REG_DIR);
	solomon_deselect(sol);
	return id;
};

void Solomon_Reset(uint8_t SolomonNum)
{
	if (SolomonNum == 1)
	{
		WriteLn("reset Sol1");
	}
#ifdef SVR_HAVE_SOLOMON2
	else if (SolomonNum == 2)
	{
		WriteLn("reset Sol2");
	}
#endif
	else
	{
		WriteLn("Wrong Sol num");
		return;
	}
	Solomon_t *sol = solomon_get_channel(SolomonNum - 1);
	solomon_start_reset(sol);
	svr_yield_ms(10);
	solomon_end_reset(sol);
}

#endif  // SVR_HAVE_SOLOMON

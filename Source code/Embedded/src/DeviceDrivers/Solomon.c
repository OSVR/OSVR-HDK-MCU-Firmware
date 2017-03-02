/*
 * Solomon.c
 *
 * Created: 9/7/2013 11:34:25 PM
 *  Author: Sensics
 */

// driver for Solomon MIPI bridge chip
#include "GlobalOptions.h"

#ifdef SVR_HAVE_SOLOMON

#include "Solomon.h"
#include "board.h"

#include "my_hardware.h"
#include "NXP/my_bit.h"  // for lo() and hi()

#include <spi_master.h>

#include "Console.h"
#include "SvrYield.h"

#include <stdio.h>

#include "BitUtilsC.h"

Solomon_t g_solomons[SVR_HAVE_SOLOMON] = {Solomon1_Struct
#ifdef SVR_HAVE_SOLOMON2
                                          ,
                                          Solomon2_Struct
#endif
};

static struct spi_device devices[2];

static SPI_t *spi[2];

static bool SolomonInitialized = false;

// uint16_t Solomon_CSN[2]; todo: can we remove?

SOLOMON_DEPRECATED static bool init_solomon_spi(uint8_t deviceID);
SOLOMON_DEPRECATED static void select_solomon(uint8_t channel);
SOLOMON_DEPRECATED static void deselect_solomon(void);
static void solomon_wait_for_spi_rx_full(uint8_t channel);

bool init_solomon_spi(uint8_t deviceID)
{
	/// now done by ssd2828 driver.
	return true;
}

void Solomon_Dump_All_Config_Debug(const char *loc)
{
	for (uint8_t deviceId = 0; deviceId < SVR_HAVE_SOLOMON; ++deviceId)
	{
		Solomon_Dump_Config_Debug(deviceId, loc);
	}
}
void Solomon_Dump_Config_Debug(uint8_t deviceId, const char *loc)
{
	char msg[20];
	uint16_t config = read_solomon(deviceId, SOLOMON_REG_CFGR);
	char confStr[] = BITUTILS_CSTR_INIT_FROM_U16_TO_BIN(config);
	sprintf(msg, "Config %d: ", deviceId);
	Write(msg);

	Write(confStr);
	Write(" [");
	Write(loc);
	WriteLn("]");
}

bool init_solomon_device(uint8_t deviceID)
{
	/// 20 MHz crystal on xtal-in/xtal-io
	Solomon_t *sol = solomon_get_channel(deviceID);
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

	solomon_select(sol);
#ifndef H546DLT01
	solomon_write_reg_word(sol, 0xBA, 0xC030);  // lane speed=960Mbps
#else
	solomon_write_reg_word(sol, 0xBA, 0xC02D);  // lane speed=900Mbps
#endif
	solomon_write_reg_word(sol, 0xBB, 0x0008);  // LP clock
	WriteLn("Trying to enable PLL");
	solomon_write_reg_word(sol, 0xB9, 0x0001);  // enable PLL
	while (!BITUTILS_CHECKBIT(solomon_read_reg_2byte(sol, 0xc6), BITUTILS_BIT(7)))
	{
		// no pll lock
		svr_yield_ms(1);
	}
	WriteLn("PLL locked");

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
	write_solomon(deviceID, 0xB7, 0x0302);  // LP generic write
	write_solomon(deviceID, 0xB8, 0x0000);  // VC

#ifdef LS055T1SX01                                       // sharp 5.5"
	write_solomon(deviceID, 0xBC, 0x0003);               // no of bytes to send
	write_solomon_pair(deviceID, 0xBF, 0x0F51, 0x00FF);  // cmd=51, data1=0F

	write_solomon(deviceID, 0xBC, 0x0002);  // no of byte send
	write_solomon(deviceID, 0xBF, 0x0453);  // cmd=53, data=04
	write_solomon(deviceID, 0xBF, 0x0055);  // cmd=55, data=00

	write_solomon(deviceID, 0xB7, 0x0342);  // LP DCS write
	write_solomon(deviceID, 0xB8, 0x0000);  // VC

	write_solomon(deviceID, 0xBC, 0x0001);  //
	write_solomon(deviceID, 0xBF, 0x0029);  // display on
	svr_yield_ms(120);
	write_solomon(deviceID, 0xBF, 0x0011);  // sleep out
#endif

#ifdef LS050T1SX01  // sharp 5"
	// from LS050T1SX01 data sheet
	write_solomon(deviceID, 0xBC, 0x0002);  // no of byte send
	write_solomon(deviceID, 0xBF, 0x04B0);  // cmd=B0, data=04
	write_solomon(deviceID, 0xBF, 0x01D6);  // cmd=D6, data=01
	write_solomon(deviceID, 0xB7, 0x0342);  // LP DCS write
	write_solomon(deviceID, 0xB8, 0x0000);  // VC
	write_solomon(deviceID, 0xBC, 0x0003);  // no of byte send

	write_solomon_pair(deviceID, 0xBF, 0x0F51, 0x00FF);  // cmd=51, data1=0F

	write_solomon(deviceID, 0xBC, 0x0002);  //
	// write_solomon(deviceID,0xBB,0x0008); // LP clock BC 0002
	write_solomon(deviceID, 0xBF, 0x0453);  // cmd=53, data=04
	write_solomon(deviceID, 0xBC, 0x0001);  //
	write_solomon(deviceID, 0xBF, 0x0029);  // display on
	svr_yield_ms(120);
	write_solomon(deviceID, 0xBF, 0x0011);  // sleep out

// end of LS050T1SX01 data sheet
#endif

#ifdef H546DLT01  // AUO 5.46" OLED
	// from LS050T1SX01 data sheet
	write_solomon(deviceID, 0xBC, 0x0002);  // no of byte send
	write_solomon(deviceID, 0xB7, 0x0321);  // LP DCS write
	write_solomon(deviceID, 0xB8, 0x0000);  // VC

	// write_solomon(deviceID,0xBC,0x0002); // number of bytes to write
	svr_yield_ms(100);

#ifdef LOW_PERSISTENCE
	write_solomon(deviceID, 0xBF, 0x08FE);  // cmd=FE, data=08
	write_solomon(deviceID, 0xBF, 0x4003);  // cmd=FE, data=08
	write_solomon(deviceID, 0xBF, 0x1A07);  // cmd=FE, data=08
	/*Add by AUO*/
	write_solomon(deviceID, 0xBF, 0x0DFE);  // cmd=FE, data=0D
	write_solomon(deviceID, 0xBF, 0xFE53);  // cmd=53, data=FE
	/* -----*/
	write_solomon(deviceID, 0xBF, 0x00FE);  // cmd=FE, data=08
	write_solomon(deviceID, 0xBF, 0x08C2);  // cmd=FE, data=08
	write_solomon(deviceID, 0xBF, 0xFF51);  // cmd=FE, data=08

	// write_solomon(deviceID,0xBF,0x9889);
	// write_solomon(deviceID,0xBF,0x068a);
	// write_solomon(deviceID,0xBF,0x708b);

	// removed for test
	// write_solomon(deviceID,0xBF,0xE689);
	// write_solomon(deviceID,0xBF,0x8E8a);
	// write_solomon(deviceID,0xBF,0x118b);

	Display_Set_Strobing(deviceID, Display_Strobing_Rate, Display_Strobing_Percent);

	write_solomon(deviceID, 0xBF, 0xFF51);  // cmd=FE, data=08

#else
	write_solomon(deviceID, 0xBF, 0x04FE);  // cmd=FE, data=04
	svr_yield_ms(16);
	write_solomon(deviceID, 0xBF, 0x005E);
	svr_yield_ms(16);
	write_solomon(deviceID, 0xBF, 0x4744);
	svr_yield_ms(16);
	write_solomon(deviceID, 0xBF, 0x07FE);
	svr_yield_ms(16);
	write_solomon(deviceID, 0xBF, 0x6AA9);
	svr_yield_ms(16);
	write_solomon(deviceID, 0xBF, 0x0AFE);
	svr_yield_ms(16);
	write_solomon(deviceID, 0xBF, 0x5214);
	svr_yield_ms(16);
	write_solomon(deviceID, 0xBF, 0x00FE);
	svr_yield_ms(16);
	write_solomon(deviceID, 0xBF, 0x0135);
	svr_yield_ms(16);
	write_solomon(deviceID, 0xBF, 0x0055);
	svr_yield_ms(16);
#endif
/*
write_solomon(deviceID,0xBC,0x0001); //
delay_ms(16);
write_solomon(deviceID,0xBF,0x0011); // sleep out
delay_ms(170); // delay > 10 frames
write_solomon(deviceID,0xBF,0x0029); // display on
delay_ms(20); // delay > 1 frames
write_solomon(deviceID,0xBC,0x0002); // number of bytes to write
write_solomon(deviceID,0xBF,0x07FE); // cmd=FE, data=BF
delay_ms(100); // delay > 5 frames
write_solomon(deviceID,0xBF,0xFAA9);
delay_ms(16);
write_solomon(deviceID,0xBF,0x00FE);
delay_ms(16);
write_solomon(deviceID,0xBC,0x0001); //
delay_ms(16);*/

#endif

#ifndef H546DLT01  // AUO 5.46" OLED
	               // video mode on
	svr_yield_ms(250);
	write_solomon(deviceID, 0xB7, 0x034B);  // video mode on
	svr_yield_ms(100);
#endif

	Solomon_Dump_All_Config_Debug("init_solomon_device - after");
	solomon_deselect(sol);
	return true;
}

void init_solomon(void)
{
#ifdef Solomon1_SPI
	devices[Solomon1].id = Solomon1_CSN;
	devices[Solomon1].sdc = Solomon1_AddrData;
	spi[Solomon1] = &Solomon1_SPI;
#endif

#ifdef Solomon2_SPI
	devices[Solomon2].id = Solomon1_CSN;
	devices[Solomon2].sdc = Solomon2_AddrData;
	spi[Solomon2] = &Solomon1_SPI;
#endif

	// Solomon_CSN[Solomon1]=Solomon1_CSN; // todo: can we remove?
	// Solomon_CSN[Solomon2]=Solomon2_CSN;

	init_solomon_spi(Solomon1);  // no need to do repeat for Solomon2 because both
	                             // share the same SPI port

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

/* write data to solomon
    channel: solomon1 or solomon2
    address: address to read
    returns 16-bit data
*/

void select_solomon(uint8_t channel)
// enable the mux and select the correct output
{
#ifndef SVR_HAVE_SOLOMON2
	return;
#else
	ioport_set_pin_low(SPI_Mux_OE);
	if (channel == Solomon1)
		ioport_set_pin_low(SPI_Mux_Select);
	else
		ioport_set_pin_high(SPI_Mux_Select);
#endif
}

void deselect_solomon(void)

// deselect the mux

{
	// ioport_set_pin_high(SPI_Mux_OE); // leave permanently low per Zeev
}

inline void solomon_wait_for_spi_rx_full(uint8_t channel)
{
	while (!spi_is_rx_full(spi[channel]))
	{
	}
}

uint16_t read_solomon(uint8_t channel, uint8_t address)

{
	Solomon_t *sol = solomon_get_channel(channel);
	solomon_select(sol);
	uint16_t data = solomon_read_reg_2byte(sol, address);
	solomon_deselect(sol);
	return data;
};

/* write data to solomon
    channel: solomon1 or solomon2
    address: address to write
    data: data to write
*/

void write_solomon(uint8_t channel, uint8_t address, uint16_t data)

{
	select_solomon(channel);

	spi_select_device(spi[channel], &devices[channel]);
	lower_sdc(channel);  // lower sdc bit because this is command
	spi_write_single(spi[channel], address);
	solomon_wait_for_spi_rx_full(channel);
	raise_sdc(channel);                        // raise sdc bit because here comes the data
	spi_write_single(spi[channel], lo(data));  // low byte first. by default, MSB of each byte is
	                                           // sent first because DORD=0
	solomon_wait_for_spi_rx_full(channel);
	spi_write_single(spi[channel], hi(data));
	solomon_wait_for_spi_rx_full(channel);
	spi_deselect_device(spi[channel], &devices[channel]);

	deselect_solomon();
};

// writes a pair of data points
void write_solomon_pair(uint8_t channel, uint8_t address, uint16_t data1, uint16_t data2)

{
	select_solomon(channel);

	spi_select_device(spi[channel], &devices[channel]);
	lower_sdc(channel);  // lower sdc bit because this is command
	spi_write_single(spi[channel], address);

	solomon_wait_for_spi_rx_full(channel);
	raise_sdc(channel);  // raise sdc bit because here comes the data

	spi_write_single(spi[channel], lo(data1));  // low byte first. by default, MSB of each byte
	                                            // is sent first because DORD=0

	solomon_wait_for_spi_rx_full(channel);
	spi_write_single(spi[channel], hi(data1));
	solomon_wait_for_spi_rx_full(channel);

	spi_write_single(spi[channel], lo(data2));  // low byte first. by default, MSB of each byte
	                                            // is sent first because DORD=0
	solomon_wait_for_spi_rx_full(channel);
	spi_write_single(spi[channel], hi(data2));
	solomon_wait_for_spi_rx_full(channel);

	spi_deselect_device(spi[channel], &devices[channel]);

	deselect_solomon();
};

// read the solomon ID
uint16_t read_Solomon_ID(uint8_t channel)
{
	Solomon_t *sol = solomon_get_channel(channel);
	solomon_select(sol);
	uint16_t id = solomon_read_reg_2byte(sol, SOLOMON_REG_DIR);
	solomon_deselect(sol);
	return id;
};

void raise_sdc(uint8_t channel)
{
	ioport_set_pin_high(devices[channel].sdc);  // // raise sdc bit because here comes the data
}

void lower_sdc(uint8_t channel)
{
	ioport_set_pin_low(devices[channel].sdc);  // lower sdc bit because this is command
}

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
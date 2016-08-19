/*
 * Toshiba_TC358870.c
 *
 * Created: 7/21/2016 8:30:05 AM
 *  Author: Coretronic, Sensics
 */

// Options header
#include "GlobalOptions.h"

#ifdef SVR_HAVE_TOSHIBA_TC358870

#include "Toshiba_TC358870.h"

// Application headers
#include "Console.h"
#include "my_hardware.h"
#include "SvrYield.h"

// Vendor library header
#include <libhdk20.h>

// asf header
#include <ioport.h>

static uint8_t s_tc358870_init_count = 0;

void Toshiba_TC358870_Init(void)
{
	WriteLn("Toshiba_TC358870_Init: Start");
	// Dennis Yeh 2016/03/14 : for TC358870
	uint8_t tc_data;
	TC358870_i2c_Init();
	TC358870_i2c_Read(0x0000, &tc_data);

	PowerOnSeq();
	s_tc358870_init_count++;
	WriteLn("Toshiba_TC358870_Init: End");
}

void Toshiba_TC358870_Init_Receiver() { TC358870_Init_Receive_HDMI_Signal(); }
uint8_t Toshiba_TC358870_Get_Init_Count() { return s_tc358870_init_count; }
void Toshiba_TC358870_Trigger_Reset()
{
	WriteLn("Toshiba_TC358870 Resetting");
	ioport_set_pin_low(TC358870_Reset_Pin);
	svr_yield_ms(12);
	ioport_set_pin_high(TC358870_Reset_Pin);
}

#define TC_MAKE_SINGLE_BIT_MASK(BIT) (0x01 << BIT)

enum
{
	TC_REG_DCSCMD_Q = 0x0504,
	TC_REG_SYS_STATUS = 0x8520,
#if 0
TC_REG_SYS_STATUS_HAVE_VIDEO_MASK = TC_MAKE_SINGLE_BIT_MASK(7) | TC_MAKE_SINGLE_BIT_MASK(3) /* PHY DE detect */ | TC_MAKE_SINGLE_BIT_MASK(2) /* PHY PLL lock */ | TC_MAKE_SINGLE_BIT_MASK(1) /* TMDS input amplitude */ | TC_MAKE_SINGLE_BIT_MASK(0) /* DDC_Power input */
#endif
	TC_REG_SYS_STATUS_HAVE_VIDEO_BIT_MASK = TC_MAKE_SINGLE_BIT_MASK(7)
};

/// Checks that all the bits in the mask are set. Factored out because I'm not sure what will give the best performance.
static inline bool checkMask(uint8_t value, uint8_t mask)
{
#if 0
	return (value & mask) == mask;
#else
	// xor should flip all masked bits from 1 to 0, if they were set, then the and should give us only any
	// previously-unset bits in the masked bits that remain. This lets us compare against 0. Who knows, the compiler may
	// be clever enough to turn the first approach into this.
	return ((value ^ mask) & mask) == 0x0;
#endif
}

/// assumes the mask you pass has only one bit set!
static inline bool checkBit(uint8_t value, uint8_t mask) { return (value & mask) != 0; }
/* Documentation from old Coretronic-authored predecessor to this function follows: note that the comparison to 0x9f is
   faulty.
    Function : Toshiba_TC358870_HDMI_IsVideoExisting
    IN : void
    OUT: 0: video does not exist
              1: video exist

     To read TC358870 register SYS_STATUS (0x8520) to check  video status.
     if the value is 0x9F, means the video is ready, otherwise it's not exist.

*/
bool Toshiba_TC358870_Have_Video_Sync(void)
{
	uint8_t tc_data;

	if (TC358870_i2c_Read(TC_REG_SYS_STATUS, &tc_data) != TC358870_OK)  // get SYS_STATUS
		return false;

	/// @todo - should we check the lower nybble too (PHY DE detect, PHY PLL, TMDS input amplitude, and DDC power
	/// input)?
	/// Bit 7 is input video sync - bits 6, 5, and 4 are unimportant to the task at hand, so equality to 0x9f is not
	/// quite right.
	return checkBit(tc_data, TC_REG_SYS_STATUS_HAVE_VIDEO_BIT_MASK);
}

/// Send a short DSI command with no parameter.
void Toshiba_TC358870_DSI_Write_Cmd_Short(uint8_t cmd)
{
	TC358870_i2c_Write(TC_REG_DCSCMD_Q, 0x0005, 2);
	TC358870_i2c_Write(TC_REG_DCSCMD_Q, (uint32_t)cmd, 2);
}

/// Send a short DSI command with one parameter.
void Toshiba_TC358870_DSI_Write_Cmd_Short_Param(uint8_t cmd, uint8_t param)
{
	TC358870_i2c_Write(TC_REG_DCSCMD_Q, 0x0015, 2);
	// uint8_t yyCmd, uint8_t zzArg
	// want to send 0xzzyy
	TC358870_i2c_Write(TC_REG_DCSCMD_Q, (((uint16_t)param) << sizeof(cmd)) | ((uint16_t)cmd), 2);
}

/// Send a "long" DSI command with data (may be of length 0)
// void Toshiba_TC358870_DSI_Write_Cmd_Long(uint8_t cmd, uint16_t len, uint8_t * data);

#endif  // SVR_HAVE_TOSHIBA_TC358870
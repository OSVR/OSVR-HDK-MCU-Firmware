/*
 * Toshiba_TC358870.c
 *
 * Created: 7/21/2016 8:30:05 AM
 *  Author: Coretronic, Sensics
 */

#include "GlobalOptions.h"

#ifdef SVR_HAVE_TOSHIBA_TC358870

#include "Toshiba_TC358870.h"
#include "Console.h"
#include "my_hardware.h"
#include "SvrYield.h"
#include <libhdk20.h>

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

uint8_t Toshiba_TC358870_Get_Init_Count() { return s_tc358870_init_count; }
void Toshiba_TC358870_Trigger_Reset(void)
{
	WriteLn("Toshiba_TC358870 Resetting");
	ioport_set_pin_low(TC358870_Reset_Pin);
	svr_yield_ms(12);
	ioport_set_pin_high(TC358870_Reset_Pin);
}
/// @todo currently disabled because it is instead referenced circularly by libhdk20 by a different name
#if 0
/*
    Function : Toshiba_TC358870_HDMI_IsVideoExisting
    IN : void
    OUT: 0: video does not exist
              1: video exist

     To read TC358870 register SYS_STATUS (0x8520) to check  video status.
     if the value is 0x9F, means the video is ready, otherwise it's not exist.

*/
bool Toshiba_TC358870_HDMI_IsVideoExisting(void)
{
	uint8_t tc_data;

	if (TC358870_i2c_Read(0x8520, &tc_data) != TC358870_OK)  // get SYS_STATUS
		return false;

	if (tc_data != 0x9F)
		return false;

	return true;
}
#endif

#endif  // SVR_HAVE_TOSHIBA_TC358870
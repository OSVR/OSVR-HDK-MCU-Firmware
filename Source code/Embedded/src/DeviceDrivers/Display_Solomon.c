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

void Display_System_Init() { init_solomon(); }
void Display_Init(uint8_t id) { init_solomon_device(id); }
void Display_On(uint8_t deviceID)
{
#ifdef H546DLT01  // AUO 5.46" OLED
	// svr_yield_ms(500);
	WriteLn("Turning display on");

	// display power on
	svr_yield_ms(20);

	// initial setting
	write_solomon(deviceID, 0xBC, 0x0002);  // no of byte send

	write_solomon(deviceID, 0xBF, 0x0011);  // sleep out
	svr_yield_ms(33);
	write_solomon(deviceID, 0xB7, 0x0329);  // video signal on
	svr_yield_ms(166);                      //>10 frame
	write_solomon(deviceID, 0xBF, 0x0029);  // display on

#endif
}

void Display_Off(uint8_t deviceID)
{
#ifdef H546DLT01  // AUO 5.46" OLED

	WriteLn("Turning display off");

	write_solomon(deviceID, 0xB7, 0x0321);  // video mode off
	svr_yield_ms(16);
	write_solomon(deviceID, 0xBF, 0x0028);  // display off
	svr_yield_ms(16);
	write_solomon(deviceID, 0xBF, 0x0010);  // sleep in
	svr_yield_ms(20);                       // delay > 1 frames

#endif
}
void Display_Reset(uint8_t deviceID) { Solomon_Reset(deviceID); }
// power cycles display connected to the specific device
void Display_Powercycle(uint8_t deviceID)
{
	write_solomon(deviceID, 0xBF, 0x0028);  // display on
	svr_yield_ms(120);
	write_solomon(deviceID, 0xBF, 0x0010);  // sleep out

	svr_yield_ms(1000);

	write_solomon(deviceID, 0xBF, 0x0029);  // display on
	svr_yield_ms(120);
	write_solomon(deviceID, 0xBF, 0x0011);  // sleep out
}

void Display_Set_Strobing(uint8_t deviceID, uint8_t refresh, uint8_t percentage)
{
	/// @todo Do we need #ifdef H546DLT01 here to limit this to the low-persistence AUO OLED panel?
	Display_Strobing_Rate = refresh;
	SetConfigValue(PersistenceOffset, Display_Strobing_Rate);

	Display_Strobing_Percent = percentage;
	SetConfigValue(PersistencePercentOffset, Display_Strobing_Percent);

	// added commands to address strobing
	write_solomon(deviceID, 0xBF, 0x08fe);

	if (refresh == 60)
	{
		write_solomon(deviceID, 0xBF, 0x9889);
		switch (percentage)
		{
		case 0:
			write_solomon(deviceID, 0xBF, 0x078a);
			write_solomon(deviceID, 0xBF, 0x708b);
			break;
		case 10:
			write_solomon(deviceID, 0xBF, 0xbd8a);
			write_solomon(deviceID, 0xBF, 0x708b);
			break;
		case 20:
			write_solomon(deviceID, 0xBF, 0x808a);
			write_solomon(deviceID, 0xBF, 0x718b);
			break;
		case 30:
			write_solomon(deviceID, 0xBF, 0x438a);
			write_solomon(deviceID, 0xBF, 0x728b);
			break;
		case 40:
			write_solomon(deviceID, 0xBF, 0x068a);
			write_solomon(deviceID, 0xBF, 0x738b);
			break;
		case 50:
			write_solomon(deviceID, 0xBF, 0xcc8a);
			write_solomon(deviceID, 0xBF, 0x738b);
			break;
		case 60:
			write_solomon(deviceID, 0xBF, 0x8c8a);
			write_solomon(deviceID, 0xBF, 0x748b);
			break;
		case 70:
			write_solomon(deviceID, 0xBF, 0x8b8a);
			write_solomon(deviceID, 0xBF, 0x758b);
			break;
		case 80:
			write_solomon(deviceID, 0xBF, 0x128a);
			write_solomon(deviceID, 0xBF, 0x768b);
			break;
		case 82:
			write_solomon(deviceID, 0xBF, 0x3a8a);
			write_solomon(deviceID, 0xBF, 0x768b);
			break;
		case 90:
			write_solomon(deviceID, 0xBF, 0xd58a);
			write_solomon(deviceID, 0xBF, 0x768b);
			break;
		default:
			write_solomon(deviceID, 0xBF, 0xCC8a);
			write_solomon(deviceID, 0xBF, 0x738b);
			break;
		}
	}

	else  // refresh is 240
	{
		write_solomon(deviceID, 0xBF, 0xe689);
		switch (percentage)
		{
		case 50:
			write_solomon(deviceID, 0xBF, 0xf38a);
			write_solomon(deviceID, 0xBF, 0x108b);
			break;
		case 82:
			write_solomon(deviceID, 0xBF, 0x8e8a);
			write_solomon(deviceID, 0xBF, 0x118b);
			break;
		default:
			write_solomon(deviceID, 0xBF, 0x8e8a);
			write_solomon(deviceID, 0xBF, 0x118b);
			break;
		}
	}

	write_solomon(deviceID, 0xBF, 0x00fe);
}

#endif
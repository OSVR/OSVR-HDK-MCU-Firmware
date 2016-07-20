/*
 * Display.c
 *
 * Created: 7/20/2016 8:33:19 AM
 *  Author: Sensics
 */

#include "Display.h"

#include "GlobalOptions.h"

uint8_t Display_Strobing_Rate = 60;
uint8_t Display_Strobing_Percent = 80;

void Display_System_Full_Init()
{
	Display_System_Init();

#ifdef SVR_HAVE_DISPLAY1
	Display_Init(Display1);
#endif
#ifdef SVR_HAVE_DISPLAY2
	Display_Init(Display2);
#endif
}
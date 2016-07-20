/*
 * Display.h
 *
 * Created: 7/19/2016 9:40:13 AM
 *  Author: Sensics
 */

#ifndef DISPLAY_H_
#define DISPLAY_H_

#include <stdint.h>

typedef enum { Display1 = 0, Display2 = 1 } DisplayId;
void Display_System_Init(void);
void Display_Init(uint8_t id);
void Display_On(uint8_t deviceID);
void Display_Off(uint8_t deviceID);
void Display_Powercycle(uint8_t deviceID);

// strobing settings
void Display_Set_Strobing(uint8_t deviceID, uint8_t refresh, uint8_t percentage);
extern uint8_t Display_Strobing_Rate;
extern uint8_t Display_Strobing_Percent;

#endif /* DISPLAY_H_ */

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

/// Initializes the display system/controller, not including individual displays or per-display circuitry.
void Display_System_Init(void);

/// Initializes per-display/device controllers and panels.
void Display_Init(uint8_t id);

/// A convenience method that calls Display_System_Init(), then Display_Init() for each available display. Shared
/// implementation: Does not need to be implemented by custom implementations of this interface.
void Display_System_Full_Init(void);

void Display_On(uint8_t deviceID);

void Display_Off(uint8_t deviceID);

// Reset the display controller for the given device ID
void Display_Reset(uint8_t deviceID);

// Power-cycle the panel(s) connected to the display controller with given device ID
void Display_Powercycle(uint8_t deviceID);

// strobing settings
void Display_Set_Strobing(uint8_t deviceID, uint8_t refresh, uint8_t percentage);
extern uint8_t Display_Strobing_Rate;
extern uint8_t Display_Strobing_Percent;

#endif /* DISPLAY_H_ */

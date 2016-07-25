/*
 * SideBySide.h
 *
 *  Author: Sensics
 */

#include <stdbool.h>

// Initialize side-by-side functionality, setting up initial pin states, loading
// preferences, etc.
void SxS_Init(void);

// Toggle side-by-side state, writing updated preferences.
void SxS_Toggle(void);

// Enable side-by-side, writing updated preferences.
void SxS_Enable(void);

// Enable side-by-side, writing updated preferences.
void SxS_Disable(void);

// Get current side-by-side state.
bool SxS_IsEnabled(void);

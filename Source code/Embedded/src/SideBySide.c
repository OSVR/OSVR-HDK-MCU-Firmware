/*
 * SideBySide.c
 *
 *  Author: Sensics
 */

#include "SideBySide.h"
#include "GlobalOptions.h"

#ifdef SVR_HAVE_SIDEBYSIDE

#include "my_hardware.h"
#include <ioport.h>

static bool sxs_enabled = false;

static void SxS_Set(bool state);
static void SxS_Apply(void);
static void SxS_Save_Config(void);

static void SxS_Set(bool state)
{
	if (state)
	{
#ifdef DSIGHT
		ioport_set_pin_high(Side_by_side_A);
#endif
		ioport_set_pin_high(Side_by_side_B);
	}
	else
	{
#ifdef DSIGHT
		ioport_set_pin_low(Side_by_side_A);
#endif
		ioport_set_pin_low(Side_by_side_B);
	}
}

static inline void SxS_Apply(void) { SxS_Set(sxs_enabled); }
static inline void SxS_Save_Config()
{
#ifdef OSVRHDK
	SetConfigValue(SideBySideOffset, sxs_enabled ? 1 : 0);
#endif
}

void SxS_Init()
{
#if defined(OSVRHDK)
	ioport_configure_pin(Side_by_side_B, IOPORT_DIR_OUTPUT | IOPORT_INIT_LOW);
	uint8_t sideBySideConfig;
	if (GetValidConfigValueOrWriteDefault(SideBySideOffset, 0, &sideBySideConfig))
	{
		sxs_enabled = (sideBySideConfig != 0);
		SxS_Apply();
	}
#elif defined(DSIGHT)
	ioport_configure_pin(Side_by_side_A, IOPORT_DIR_OUTPUT | IOPORT_INIT_HIGH);
	ioport_configure_pin(Side_by_side_B, IOPORT_DIR_OUTPUT | IOPORT_INIT_HIGH);
	sxs_enabled = true;
#endif
}

void SxS_Toggle()
{
	sxs_enabled = !sxs_enabled;
	SxS_Apply();
	SxS_Save_Config();
}

void SxS_Enable()
{
	sxs_enabled = true;
	SxS_Apply();
	SxS_Save_Config();
}

void SxS_Disable()
{
	sxs_enabled = false;
	SxS_Apply();
	SxS_Save_Config();
}

bool SxS_IsEnabled() { return sxs_enabled; }
#else
void SxS_Init() {}
void SxS_Toggle() {}
void SxS_Enable() {}
void SxS_Disable() {}
bool SxS_IsEnabled() { return false; }

#endif  // SVR_HAVE_SIDEBYSIDE

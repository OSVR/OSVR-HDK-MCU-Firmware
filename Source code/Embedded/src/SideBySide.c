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

#if defined(DSIGHT)
// dSight defaults to enabled.
#define SXS_STARTUP_VALUE true

#else
#define SXS_STARTUP_VALUE false
#endif

#define SXS_STARTUP_PIN_STATE (SXS_STARTUP_VALUE ? IOPORT_INIT_LOW : IOPORT_INIT_HIGH)

static bool sxs_enabled = SXS_STARTUP_VALUE;

static void SxS_Set(bool state);
static void SxS_Apply(void);
static void SxS_Save_Config(void);

static void SxS_Set(bool state)
{
	if (state)
	{
#ifdef Side_by_side_A
		ioport_set_pin_high(Side_by_side_A);
#endif
#ifdef Side_by_side_B
		ioport_set_pin_high(Side_by_side_B);
#endif
	}
	else
	{
#ifdef Side_by_side_A
		ioport_set_pin_low(Side_by_side_A);
#endif
#ifdef Side_by_side_B
		ioport_set_pin_low(Side_by_side_B);
#endif
	}
}

static inline void SxS_Apply(void) { SxS_Set(sxs_enabled); }
static inline void SxS_Save_Config()
{
#ifdef SVR_HAVE_SIDEBYSIDE_CONFIG
	SetConfigValue(SideBySideOffset, sxs_enabled ? 1 : 0);
#endif
}

void SxS_Init()
{
#ifdef Side_by_side_A
	// Initialize this pin to FPGA, if defined.
	ioport_configure_pin(Side_by_side_A, IOPORT_DIR_OUTPUT | SXS_STARTUP_PIN_STATE);
#endif
#ifdef Side_by_side_B
	// Initialize this pin to FPGA, if defined.
	ioport_configure_pin(Side_by_side_B, IOPORT_DIR_OUTPUT | SXS_STARTUP_PIN_STATE);
#endif

#ifdef SVR_HAVE_SIDEBYSIDE_CONFIG
	// If we have config, read and apply it.
	uint8_t sideBySideConfig;
	if (GetValidConfigValueOrWriteDefault(SideBySideOffset, 0, &sideBySideConfig))
	{
		sxs_enabled = (sideBySideConfig != 0);
		SxS_Apply();
	}
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

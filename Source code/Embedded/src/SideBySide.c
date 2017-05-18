/*
 * SideBySide.c
 *
 *  Author: Sensics
 */

#include "GlobalOptions.h"

// needs to be above conditional since we provide dummy implementations if not defined
#include "SideBySide.h"

#ifdef SVR_HAVE_SIDEBYSIDE

#include "my_hardware.h"
#include "Console.h"
#include <ioport.h>

#if defined(SVR_IS_DSIGHT)
// dSight defaults to enabled.
#define SXS_STARTUP_VALUE true

#else
#define SXS_STARTUP_VALUE false
#endif

#define SXS_STARTUP_PIN_STATE (SXS_STARTUP_VALUE ? IOPORT_INIT_LOW : IOPORT_INIT_HIGH)

static bool sxs_enabled = SXS_STARTUP_VALUE;

static bool s_sxs_initialized = false;

static void SxS_Set(bool state);
static void SxS_Apply(void);
static void SxS_Save_Config(void);
static void SxS_Ensure_Init(void);

static void SxS_Set(bool state)
{
#ifdef Side_by_side_A
	ioport_set_pin_level(Side_by_side_A, state);
#endif  // Side_by_side_A
#ifdef Side_by_side_B
	ioport_set_pin_level(Side_by_side_B, state);
#endif
}

static inline void SxS_Apply(void)
{
	Write("SxS_Apply: sxs_enabled is ");
	if (sxs_enabled)
	{
		WriteLn("true");
	}
	else
	{
		WriteLn("false");
	}
	SxS_Set(sxs_enabled);
}
static inline void SxS_Save_Config()
{
#ifdef SVR_HAVE_SIDEBYSIDE_CONFIG
	SetConfigValue(SideBySideOffset, sxs_enabled ? 1 : 0);
#endif
}

static inline void SxS_Ensure_Init()
{
	if (!s_sxs_initialized)
	{
		SxS_Init();
	}
}

void SxS_Init()
{
	WriteLn("SxS Init");
	s_sxs_initialized = true;
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
	}
#endif
	SxS_Apply();
}

void SxS_Toggle()
{
	SxS_Ensure_Init();
	sxs_enabled = !sxs_enabled;
	SxS_Apply();
	SxS_Save_Config();
}

void SxS_Enable()
{
	SxS_Ensure_Init();
	sxs_enabled = true;
	SxS_Apply();
	SxS_Save_Config();
}

void SxS_Disable()
{
	SxS_Ensure_Init();
	sxs_enabled = false;
	SxS_Apply();
	SxS_Save_Config();
}


bool SxS_IsEnabled()
{
	SxS_Ensure_Init();
	return sxs_enabled;
}
#else
void SxS_Init() {}
void SxS_Toggle() {}
void SxS_Enable() {}
void SxS_Disable() {}
bool SxS_IsEnabled() { return false; }

#endif  // SVR_HAVE_SIDEBYSIDE

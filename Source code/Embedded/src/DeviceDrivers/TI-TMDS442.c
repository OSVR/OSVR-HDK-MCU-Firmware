/*
 * TI_TMDS442.c
 *
 * Created: 10/20/2013 9:23:25 AM
 *  Author: Sensics
 */

// Options header
#include "GlobalOptions.h"

#ifdef SVR_HAVE_TMDS442

#include "TI-TMDS442.h"

// application headers
#include "Console.h"
#include "Solomon.h"
#include "NXP/i2c.h"
#include "my_hardware.h"
#include "SideBySide.h"
#include "VideoInput_Protected.h"
#include "BitUtilsC.h"
#include "SvrYield.h"

// asf headers
#include <ioport.h>
#include <twi_master.h>

// standard headers
#include <stdio.h>

static uint8_t s_InputStatus = 0;     // bit field for the two channels. Shows how the TMDS is configured
static uint8_t s_LastStatusRead = 0;  // shows the last value read from TMDS config. Used to detect changes

#define TWI_TMDS442_ADDR 0x2c
#define TWI_SPEED 100000  //!< TWI data transfer rate

/// Source plug-in status register
#define TMDS442_SRC_PLUG_REG UINT8_C(0x03)
/// When clear: sink port 1/A is the main display when cloning source to both sinks
/// When set: sink port 2/B is.
static const uint8_t TMDS442_SRC_PLUG_SP_bm = BITUTILS_BIT(5);

/// When set, TMDS output status is controlled by the corresponding 5V power signal
static const uint8_t TMDS442_SRC_PLUG_5V_EN_bm = BITUTILS_BIT(4);

/// So-called "source A": 5V_PWR1 bit in TMDS442_SRC_PLUG_REG.
/// (Using A and B instead of numbers to avoid indexing confusion.)
static const uint8_t SRC_A_bm = BITUTILS_BIT(1);

/// So-called "source A": 5V_PWR2 bit in TMDS442_SRC_PLUG_REG.
/// (Using A and B instead of numbers to avoid indexing confusion.)
static const uint8_t SRC_B_bm = BITUTILS_BIT(2);

/// group mask for either source A or B.
#define SRC_AB_gm (SRC_A_bm | SRC_B_bm)

#if 0
#define TMDS442_SRC_PLUG_REG_value (TMDS442_SRC_PLUG_5V_EN_bm | SRC_AB_gm)
#define TMDS442_SRC_PLUG_REG_value (SRC_AB_gm)
#else
#define TMDS442_SRC_PLUG_REG_value (0)
#endif

/// Register for sink 1 (A)
#define SINK1_PORT_REG UINT8_C(0x01)
/// Register for sink 2 (B)
#define SINK2_PORT_REG UINT8_C(0x02)

/// Active low output enable. (Low: sink-side TMDS is on, high: sink-side TMDS off/hi-Z)
static const uint8_t SINK_PORT_nOE_bm = BITUTILS_BIT(2);

/// Sink-side I2C buffer enable (low: sink-side I2C buffer is disabled/hi-Z)
static const uint8_t SINK_PORT_I2CEN_bm = BITUTILS_BIT(3);

/// 3dB De-emphasis
static const uint8_t SINK_PORT_PRE_bm = BITUTILS_BIT(4);

static const uint8_t SINK_PORT_SOURCESEL_gm = BITUTILS_BIT(0) | BITUTILS_BIT(1);
static const uint8_t SINK_PORT_SOURCESEL_Source_1 = UINT8_C(0x00);
static const uint8_t SINK_PORT_SOURCESEL_Source_2 = UINT8_C(0x01);
static const uint8_t SINK_PORT_SOURCESEL_Source_3 = UINT8_C(0x02);
static const uint8_t SINK_PORT_SOURCESEL_Source_4 = UINT8_C(0x03);

/// A base value for SINK_PORT_REG values
#define TMDS442_SINK_PORT_VAL_BASE (UINT8_C(0))  // (SINK_PORT_I2CEN_bm)
/// Value assigned to SINK_PORT_REGs when they are shut down.
#define TMDS442_SINK_PORT_VAL_SHUTDOWN (TMDS442_SINK_PORT_VAL_BASE | SINK_PORT_nOE_bm)
/// Starting point for computing the value assigned to SINK_PORT_REGs when they are enabled.
#define TMDS442_SINK_PORT_VAL_ACTIVE_BASE (TMDS442_SINK_PORT_VAL_BASE | SINK_PORT_I2CEN_bm)

void TMDS442_ProgramHDMISwitch(void);

bool TMDS442_WriteReg(uint8_t RegNum, uint8_t Value)
{
	twi_package_t packet = {
	    .addr[0] = RegNum,               // TWI slave memory address data MSB
	    .addr_length = sizeof(uint8_t),  // TWI slave memory address data size
	    .chip = TWI_TMDS442_ADDR,        // TWI slave bus address
	    .buffer = (void *)&Value,        // transfer data source buffer
	    .length = 1                      // transfer data size (bytes)
	};
	return (twi_master_write(TWI_TMDS442_PORT, &packet) == TWI_SUCCESS);
}

bool TMDS442_ReadReg(uint8_t regNum, uint8_t *NewStatus)
{
	uint8_t Status;

	twi_package_t packet_received = {
	    .addr[0] = regNum,               // TWI slave memory address data
	    .addr_length = sizeof(uint8_t),  // TWI slave memory address data size
	    .chip = TWI_TMDS442_ADDR,        // TWI slave bus address
	    .buffer = &Status,               // transfer data destination buffer
	    .length = 1                      // transfer data size (bytes)
	};

	// Perform a read access then check the result.
	if (twi_master_read(TWI_TMDS442_PORT, &packet_received) == TWI_SUCCESS)
	{
		*NewStatus = Status;
		return true;
	}
	return false;
}

bool TMDS442_ReadInputStatus(uint8_t *newStatus)
{
	uint8_t status;
	bool ret = TMDS442_ReadReg(TMDS442_SRC_PLUG_REG, &status);
	if (ret)
	{
		*newStatus = status & SRC_AB_gm;
		return true;
	}
	return false;
}

void TMDS442_ProgramHDMISwitch(void)
{
	switch (s_InputStatus)
	{
	case 0:  // no inputs
	{
		WriteLn("TMDS442_ProgramHDMISwitch: No inputs - programming as straight-thru");
#if 0
		TMDS442_WriteReg(SINK1_PORT_REG, TMDS442_SINK_PORT_VAL_SHUTDOWN);
		TMDS442_WriteReg(SINK2_PORT_REG, TMDS442_SINK_PORT_VAL_SHUTDOWN);
#else

		TMDS442_WriteReg(SINK1_PORT_REG, TMDS442_SINK_PORT_VAL_ACTIVE_BASE | SINK_PORT_SOURCESEL_Source_1);
		TMDS442_WriteReg(SINK2_PORT_REG, TMDS442_SINK_PORT_VAL_ACTIVE_BASE | SINK_PORT_SOURCESEL_Source_2);		// switch to regular (not side-by-side) mode as there are two inputs
		SxS_Disable();
#endif
#ifdef SVR_TMDS_REPORT_STATUS
#ifndef SVR_TMDS442_ALWAYS_UPDATE_STATUS
		VideoInput_Protected_Report_No_Signal();
#endif  // !SVR_TMDS442_ALWAYS_UPDATE_STATUS
#endif // SVR_TMDS_REPORT_STATUS
		break;
	}
	case 1:  // just input A
	{
		WriteLn("TMDS442_ProgramHDMISwitch: Input A to both");
		TMDS442_WriteReg(SINK1_PORT_REG, TMDS442_SINK_PORT_VAL_ACTIVE_BASE | SINK_PORT_SOURCESEL_Source_1);
		TMDS442_WriteReg(SINK2_PORT_REG, TMDS442_SINK_PORT_VAL_ACTIVE_BASE | SINK_PORT_SOURCESEL_Source_1);
		// switch to side by side mode as there is one input
		SxS_Enable();
		break;
	}
	case 2:  // just input B
	{
		WriteLn("TMDS442_ProgramHDMISwitch: Input B to both");
		TMDS442_WriteReg(SINK1_PORT_REG, TMDS442_SINK_PORT_VAL_ACTIVE_BASE | SINK_PORT_SOURCESEL_Source_2);
		TMDS442_WriteReg(SINK2_PORT_REG, TMDS442_SINK_PORT_VAL_ACTIVE_BASE | SINK_PORT_SOURCESEL_Source_2);
		// switch to side by side mode as there is one input
		SxS_Enable();
		break;
	}
	case 3:  // both inputs
	{
		WriteLn("TMDS442_ProgramHDMISwitch: Two inputs");
		TMDS442_WriteReg(SINK1_PORT_REG, TMDS442_SINK_PORT_VAL_ACTIVE_BASE | SINK_PORT_SOURCESEL_Source_1);
		TMDS442_WriteReg(SINK2_PORT_REG, TMDS442_SINK_PORT_VAL_ACTIVE_BASE | SINK_PORT_SOURCESEL_Source_2);
		// switch to regular (not side-by-side) mode as there are two inputs
		SxS_Disable();
		break;
	}
	}
	svr_yield_ms(100);  // after programming, a few frames to allow signal to stabilize
#ifdef SVR_TMDS442_ALWAYS_UPDATE_STATUS
	VideoInput_Protected_Report_Status(s_InputStatus != 0);
#endif  // SVR_TMDS442_ALWAYS_UPDATE_STATUS
}

void TMDS442_Init(void)
{
	s_InputStatus = 0;
#if 0
	s_InputStatus = 3;  // both - for EDID testing
#endif
	if (!TWI_TMDS442_PORT_initialized)
	{
		// initialize if port has not been initialized yet
		twi_master_options_t opt_TMDS442 = {.speed = TWI_SPEED, .chip = TWI_TMDS442_ADDR};
		twi_master_setup(TWI_TMDS442_PORT, &opt_TMDS442);
		TWI_TMDS442_PORT_initialized = true;
	}
#if 1
	TMDS442_WriteReg(TMDS442_SRC_PLUG_REG, TMDS442_SRC_PLUG_REG_value);
#else
	uint8_t plugReg = 0;
	TMDS442_ReadReg(TMDS442_SRC_PLUG_REG, &plugReg);
	TMDS442_WriteReg(TMDS442_SRC_PLUG_REG, plugReg & ~(TMDS442_SRC_PLUG_5V_EN_bm));
#endif
	TMDS442_ProgramHDMISwitch();
}

void TMDS442_EnableVideoA(void)
{
	s_InputStatus |= SRC_A_bm;
	TMDS442_ProgramHDMISwitch();
}

void TMDS442_DisableVideoA(void)
{
	s_InputStatus &= ~SRC_A_bm;
	TMDS442_ProgramHDMISwitch();
}

void TMDS442_EnableVideoB(void)
{
	s_InputStatus |= SRC_B_bm;
	TMDS442_ProgramHDMISwitch();
}

void TMDS442_DisableVideoB(void)
{
	s_InputStatus &= ~SRC_B_bm;
	TMDS442_ProgramHDMISwitch();
}

void TMDS442_SetInputStatus(uint8_t NewStatus)
{
	/// Mask new status to keep it in the bounds of the case statements.
	s_InputStatus = NewStatus & SRC_AB_gm;
	TMDS442_ProgramHDMISwitch();
}

// read TMDS status and decide what to do with it
void TMDS442_Task(void)
{
	uint8_t NewStatus;
	if (!TMDS442_ReadInputStatus(&NewStatus))  // process only if successful
	{
		return;
	}

	if (NewStatus != s_LastStatusRead)
	{
		WriteLn("TMDS442: Detected change in input status.");
		s_LastStatusRead = NewStatus;
		TMDS442_SetInputStatus(NewStatus);
	}
}
#endif  // SVR_HAVE_TMDS442

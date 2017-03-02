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

// asf headers
#include <ioport.h>
#include <twi_master.h>
#include <delay.h>

// standard headers
#include <stdio.h>

static uint8_t InputStatus = 0;     // bit field for the two channels. Shows how the TMDS is configured
static uint8_t LastStatusRead = 0;  // shows the last value read from TMDS config. Used to detect changes

#define ChannelAMask 1
#define ChannelBMask 2
#define BothChannelMask (ChannelAMask | ChannelBMask)

#define TWI_TMDS442_ADDR 0x2c
#define TWI_SPEED 50000  //!< TWI data transfer rate

// internal registers
#define Sink1_port_config 0x01
#define Sink2_port_config 0x02
#define Source_plug_in_status 0x03

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

#define Fixed_mask 0  // 3db emphasis off, sink side I2C disabled
//#define Fixed_mask		0x0C // 3db emphasis off, sink side I2C enabled, output enable as well
#define Sink_TMDS_on 0x0
#define Sink_TMDS_off 0x04
#define Source_port_1 0
#define Source_port_2 1
#define Source_port_3 2
#define Source_port_4 3

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
	bool ret = TMDS442_ReadReg(Source_plug_in_status, &status);
	if (ret)
	{
		*newStatus = status & BothChannelMask;
		return true;
	}
	return false;
}
void TMDS442_ProgramHDMISwitch(void)
{
	switch (InputStatus)
	{
	case 0:  // no inputs
	{
		WriteLn("No inputs");
		TMDS442_WriteReg(Sink1_port_config, Fixed_mask); // | Sink_TMDS_off);
		TMDS442_WriteReg(Sink2_port_config, Fixed_mask); // | Sink_TMDS_off);
		break;
	}
	case 1:  // just input A
	{
		WriteLn("Input A to both");
		TMDS442_WriteReg(Sink1_port_config, Fixed_mask | Sink_TMDS_on | Source_port_1);
		TMDS442_WriteReg(Sink2_port_config, Fixed_mask | Sink_TMDS_on | Source_port_1);
		// switch to side by side mode as there is one input
		SxS_Enable();
		break;
	}
	case 2:  // just input B
	{
		WriteLn("Input B to both");
		TMDS442_WriteReg(Sink1_port_config, Fixed_mask | Sink_TMDS_on | Source_port_2);
		TMDS442_WriteReg(Sink2_port_config, Fixed_mask | Sink_TMDS_on | Source_port_2);
		// switch to side by side mode as there is one input
		SxS_Enable();
		break;
	}
	case 3:  // both inputs
	{
		WriteLn("Two inputs");
		TMDS442_WriteReg(Sink1_port_config, Fixed_mask | Sink_TMDS_on | Source_port_1);
		TMDS442_WriteReg(Sink2_port_config, Fixed_mask | Sink_TMDS_on | Source_port_2);
		// switch to regular (not side-by-side) mode as there are two inputs
		SxS_Disable();
		break;
	}
	}
	delay_ms(100);  // after programming, a few frames to allow signal to stabilize
	VideoInput_Protected_Report_Status(InputStatus != 0);
}

void TMDS442_Init(void)
{
	InputStatus = 0;
#if 0
	InputStatus = 3;  // both - for EDID testing
#endif
	if (!TWI_TMDS442_PORT_initialized)
	{
		// initialize if port has not been initialized yet
		twi_master_options_t opt_TMDS442 = {.speed = TWI_SPEED, .chip = TWI_TMDS442_ADDR};
		twi_master_setup(TWI_TMDS442_PORT, &opt_TMDS442);
		TWI_TMDS442_PORT_initialized = true;
	}
#if 1
	TMDS442_WriteReg(Source_plug_in_status, 0);  // done to change bit 4 which allows us to control the switch via I2c
#endif
#if 0
	TMDS442_WriteReg(Source_plug_in_status, 0x13); // done to change bit 4 which allows us to control the switch via I2c
	// and also to enable source-side I2C
#endif

	TMDS442_ProgramHDMISwitch();
}

void TMDS442_EnableVideoA(void)
{
	InputStatus |= ChannelAMask;
	TMDS442_ProgramHDMISwitch();
}

void TMDS442_DisableVideoA(void)
{
	InputStatus &= (ChannelAMask ^ 0xff);
	TMDS442_ProgramHDMISwitch();
}

void TMDS442_EnableVideoB(void)
{
	InputStatus |= ChannelBMask;
	TMDS442_ProgramHDMISwitch();
}

void TMDS442_DisableVideoB(void)
{
	InputStatus &= (ChannelBMask ^ 0xff);
	TMDS442_ProgramHDMISwitch();
}

void TMDS442_SetInputStatus(uint8_t NewStatus)
{
	/// Mask new status to keep it in the bounds of the case statements.
	InputStatus = NewStatus & BothChannelMask;
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

	if (NewStatus != LastStatusRead)
	{
		TMDS442_SetInputStatus(NewStatus);
		LastStatusRead = NewStatus;
	}
}
#endif  // SVR_HAVE_TMDS442

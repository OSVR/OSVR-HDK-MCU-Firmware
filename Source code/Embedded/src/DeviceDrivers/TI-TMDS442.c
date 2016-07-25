/*
 * TI_TMDS442.c
 *
 * Created: 10/20/2013 9:23:25 AM
 *  Author: Sensics
 */

// Options header
#include "GlobalOptions.h"

#ifdef TMDS422

#include "TI-TMDS442.h"

// application headers
#include "Console.h"
#include "Solomon.h"
#include "nxp/i2c.h"
#include "my_hardware.h"
#include "SideBySide.h"

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

#define TWI_TMDS422_ADDR 0x2c
#define TWI_SPEED 50000  //!< TWI data transfer rate

// internal registers
#define Sink1_port_config 0x01
#define Sink2_port_config 0x02
#define Source_plug_in_status 0x03

void ProgramHDMISwitch(void);

bool HDMI_config(uint8_t RegNum, uint8_t Value)
{
	twi_package_t packet = {
	    .addr[0] = RegNum,               // TWI slave memory address data MSB
	    .addr_length = sizeof(uint8_t),  // TWI slave memory address data size
	    .chip = TWI_TMDS422_ADDR,        // TWI slave bus address
	    .buffer = (void *)&Value,        // transfer data source buffer
	    .length = 1                      // transfer data size (bytes)
	};
	return (twi_master_write(TWI_TMDS422_PORT, &packet) == TWI_SUCCESS);
}

#define Fixed_mask 0  // 3db emphasis off, sink side I2C disabled
//#define Fixed_mask		0x0C // 3db emphasis off, sink side I2C enabled, output enable as well
#define Sink_TMDS_on 0x0
#define Sink_TMDS_off 0x04
#define Source_port_1 0
#define Source_port_2 1
#define Source_port_3 2
#define Source_port_4 3

void ProgramHDMISwitch(void)

{
	switch (InputStatus)
	{
	case 0:  // no inputs
	{
		WriteLn("No inputs");
		HDMI_config(Sink1_port_config, Fixed_mask);  //|Sink_TMDS_off);
		HDMI_config(Sink2_port_config, Fixed_mask);  //|Sink_TMDS_off);
		break;
	}
	case 1:  // just input A
	{
		WriteLn("Input A to both");
		HDMI_config(Sink1_port_config, Fixed_mask | Sink_TMDS_on | Source_port_1);
		HDMI_config(Sink2_port_config, Fixed_mask | Sink_TMDS_on | Source_port_1);
		// switch to side by side mode as there is one input
		SxS_Enable();
		init_solomon_device(Solomon1);
		init_solomon_device(Solomon2);
		delay_ms(1000);
		init_solomon_device(Solomon1);
		init_solomon_device(Solomon2);
		break;
	}
	case 2:  // just input B
	{
		WriteLn("Input B to both");
		HDMI_config(Sink1_port_config, Fixed_mask | Sink_TMDS_on | Source_port_2);
		HDMI_config(Sink2_port_config, Fixed_mask | Sink_TMDS_on | Source_port_2);
		// switch to side by side mode as there is one input
		SxS_Enable();
		init_solomon_device(Solomon1);
		init_solomon_device(Solomon2);
		delay_ms(1000);
		init_solomon_device(Solomon1);
		init_solomon_device(Solomon2);
		break;
	}
	case 3:  // both inputs
	{
		WriteLn("Two inputs");
		HDMI_config(Sink1_port_config, Fixed_mask | Sink_TMDS_on | Source_port_1);
		HDMI_config(Sink2_port_config, Fixed_mask | Sink_TMDS_on | Source_port_2);
		// switch to regular (not side-by-side) mode as there are two inputs
		SxS_Disable();
		init_solomon_device(Solomon1);
		init_solomon_device(Solomon2);
		delay_ms(1000);
		init_solomon_device(Solomon1);
		init_solomon_device(Solomon2);
		break;
	}
	}
	delay_ms(100);  // after programming, a few frames to allow signal to stabilize
}

void InitHDMISwitch(void)
{
	twi_master_options_t opt_TMDS422;

	InputStatus = 0;
	// InputStatus=3; 	 // both - for EDID testing
	opt_TMDS422.speed = TWI_SPEED;
	opt_TMDS422.chip = TWI_TMDS422_ADDR;
	if (!TWI_TMDS422_PORT_initialized)
	{
		// initialize if port has not been initialized yet
		twi_master_setup(TWI_TMDS422_PORT, &opt_TMDS422);
		TWI_TMDS422_PORT_initialized = true;
	}
	HDMI_config(Source_plug_in_status, 0);  // done to change bit 4 which allows us to control the switch via I2c
	// HDMI_config(Source_plug_in_status,0x13); // done to change bit 4 which allows us to control the switch via I2c
	// and also to enable source-side I2C
	ProgramHDMISwitch();
}

void EnableVideoA(void)
{
	InputStatus |= ChannelAMask;
	ProgramHDMISwitch();
}

void DisableVideoA(void)
{
	InputStatus &= (ChannelAMask ^ 0xff);
	ProgramHDMISwitch();
}

void EnableVideoB(void)
{
	InputStatus |= ChannelBMask;
	ProgramHDMISwitch();
}

void DisableVideoB(void)
{
	InputStatus &= (ChannelBMask ^ 0xff);
	ProgramHDMISwitch();
}

void SetInputStatus(uint8_t NewStatus)

{
	InputStatus = NewStatus;
	ProgramHDMISwitch();
}

bool ReadTMDS422Status(uint8_t regNum, uint8_t *NewStatus)
{
	uint8_t Status;

	twi_package_t packet_received = {
	    .addr[0] = regNum,               // TWI slave memory address data
	    .addr_length = sizeof(uint8_t),  // TWI slave memory address data size
	    .chip = TWI_TMDS422_ADDR,        // TWI slave bus address
	    .buffer = &Status,               // transfer data destination buffer
	    .length = 1                      // transfer data size (bytes)
	};

	// Perform a read access then check the result.
	if (twi_master_read(TWI_TMDS422_PORT, &packet_received) == TWI_SUCCESS)
	{
		*NewStatus = Status;
		return true;
	}
	else
		return false;
}

void TMDS_422_Task(void)

// read TMDS status and decide what to do with it
{
	uint8_t NewStatus;
	if (ReadTMDS422Status(Source_plug_in_status, &NewStatus))  // process only if successful
	{
		NewStatus &= BothChannelMask;
		if (NewStatus != LastStatusRead)
		{
			SetInputStatus(NewStatus);
			LastStatusRead = NewStatus;
		}
	}
}
#endif  // TMDS422
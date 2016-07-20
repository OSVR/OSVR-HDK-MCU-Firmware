/*
 * SerialStateMachine.c
 *
 * Created: 9/1/2013 5:07:35 PM
 *  Author: Sensics
 */

/*
             Main board software
*/

/*
  Copyright 2013  Sensics, Inc.

  This source code is copyrighted and can not be used without express
  written permission of Sensics, Inc.
*/

/** \file
 *
 *  Serial state machine file, The board has a control interface via a serial port (actually, virtual
    serial port implemented over USB). This file implements a state machine for receiving the commands
    from this port and analyzing them
 */

// global variable definition

/* implements state machine:
    0: USB not connected
    1: Awaiting beginning of command ("#")
    2: Receiving command

    Once command has been received it is executed.
    If command exceeds certain length, machine reverts to awaiting beginning state and sends error message
*/

#include "config/GlobalOptions.h"
#include <asf.h>
#include <stdbool.h>
#include "string.h"
#include "conf_board.h"
#include "conf_timeout.h"
#include "uart.h"
#include "SerialStateMachine.h"
#include "config/my_hardware.h"

#include "stdio.h"
#include "DeviceDrivers/Display.h"
#include "DeviceDrivers/VideoInput.h"
#include "boot.h"
#include "Console.h"
#include "main.h"
#include "TimingDebug.h"
#include <util/delay.h>
#include "my_hardware.h"

#ifdef SVR_HAVE_SOLOMON
#include "DeviceDrivers/Solomon.h"
#endif

#ifdef SVR_HAVE_FPGA
#include "FPGA.h"
#endif

#ifdef SVR_HAVE_TMDS422
#include "DeviceDrivers/TI-TMDS442.h"
#endif

#ifdef BNO070
#include "DeviceDrivers/BNO070.h"
#endif

#ifdef SVR_USING_NXP
#include "nxp/AVRHDMI.h"
#include "nxp/tmbslHdmiRx_types.h"
#include "nxp/tmdlHdmiRx.h"
#include "nxp/tmdlHdmiRx_cfg.h"
#include "nxp/tmbslTDA1997X_functions.h"
#include "nxp/tmbslTDA1997X_local.h"
#include "nxp/i2c.h"
#include "nxp/my_bit.h"
#endif

#ifdef SVR_IS_HDK_20
#include <libhdk20.h>
#endif

#define USBNotConnected 0
#define AwaitingCommand 1
#define ReceivingCommand 2
#define AwaitingCommandCompletion 3  // for commands that take a long time to complete such as contrast

static uint8_t SerialState = USBNotConnected;
static char CommandBuffer[MaxCommandLength + 1];

bool CommandReady = false;  // true if a command is ready to be executed

/// @todo Can't make this static because libhdk20 expects it to be exported: it links against it.
char CommandToExecute[MaxCommandLength + 1];

char Msg[20];  // todo: remove after debug

typedef struct
{
	union {
		struct
		{
			uint8_t Signature[8];
			uint8_t Pad[EEPROM_PAGE_SIZE - 8];
		};
		uint8_t Buffer[EEPROM_PAGE_SIZE];
	};
} EEPROM_type;

static EEPROM_type EEPROM;

// todo: is this required?
static uint8_t I2CAddress = 0;   // selected I2C address
static bool NXPLeftSide = true;  // selected eye (left or right)

// todo: move USBActive as well as TRUE and FALSE
static bool USBActive = false;  // true if USB is connected

static uint8_t BufferPos = 0;       /* position of character to be received in new buffer. When command is completed,
                              this also shows the length of the command */
static uint8_t ReadyBufferPos = 0;  // copy of BufferPos for command being executed

static uint8_t HexDigitToDecimal(uint8_t CommandBufferIndex);
static uint8_t HexPairToDecimal(uint8_t startIndex);
static void Display_software_version(void);

// called once to reset state machine
void InitSerialState(void)
{
	SerialState = AwaitingCommand;  // Ready to receive chars right now;
	CommandReady = false;
}

void ProcessIncomingChar(char CharReceived)

{
	switch (SerialState)
	{
	case USBNotConnected:  // USB not ready, so don't process char
		break;
	case AwaitingCommand:  // waiting for #
	{
		if (CharReceived == '#')
		{
			SerialState = ReceivingCommand;
			BufferPos = 0;
		}
		else if ((CharReceived == '\n') || (CharReceived == '\r'))  // show user that the program is ready
		{
			WriteEndl();
			Write(">");
		}
		break;
	}
	case ReceivingCommand:  // process normal character
	{
		if ((CharReceived == '\n') || (CharReceived == '\r'))
		{
			CommandBuffer[BufferPos] = '\0';  // terminate string
			// todo: add check whether CommandReady is already true and then decide what to do
			// move temp command to new buffer so that USB can start receiving next command
			memcpy(CommandToExecute, CommandBuffer, BufferPos + 1);
			CommandReady = true;
			ReadyBufferPos = BufferPos;
			SerialState = AwaitingCommand;  // wait for command to finish
			                                // todo: do we need PollStateMachine?
			                                // PollStateMachine=True;
		}
		else  // any other character
		{
			if (BufferPos >= MaxCommandLength)
			{
				WriteEndl();
				WriteLn(";Command too long");
				Write(">");
				SerialState = AwaitingCommand;
				BufferPos = 0;
			}
			else
			{
				CommandBuffer[BufferPos] = CharReceived;
				BufferPos++;
			}
		}
		break;
	}
	// todo: is this state really required?
	case AwaitingCommandCompletion:
	{
		Write(">");
		SerialState = AwaitingCommand;  // ready for new command
		                                // todo: do we need PollStateMachine?
		                                // PollStateMachine=False;
	}
	}
}

void ProcessInfoCommands(void);
void ProcessBNO070Commands(void);
void ProcessSPICommand(void);
void ProcessI2CCommand(void);
void ProcessFPGACommand(void);
void ProcessHDMICommand(void);
void ProcessTMDSCommand(void);

#ifdef SVR_IS_HDK_20
/// Used only by HDK 2.0 code right now.
static bool is_sensics_id_equal_to_buffer(uint8_t addr, uint8_t *buffer, int leng)
{
	uint8_t i;
	char Msg[10];

	for (i = 0; i < leng; i++)
	{
		if (nvm_eeprom_read_byte(addr + i) != buffer[i])
		{
			WriteLn("---");
			sprintf(Msg, "%d %d %d", nvm_eeprom_read_byte(addr + i), buffer[i], i);
			WriteLn(Msg);
			return false;
		}
	}

	return true;
}
#endif

// To do: move this to a util module

// converts hex digit to decimal equivalent. Works for upper and lower case. If not found, returns 0.
// accepts index of digit in command buffer as parameter

static uint8_t HexDigitToDecimal(uint8_t CommandBufferIndex)
{
	static const char Digits[] = "0123456789ABCDEF0000abcdef";
	uint8_t i;
	char CharToConvert = CommandToExecute[CommandBufferIndex];

	for (i = 0; i < 26; i++)
		if (Digits[i] == CharToConvert)
		{
			if (i < 16)
				return i;
			else
				return i - 10;
		}
	return 0;
}

static uint8_t HexPairToDecimal(uint8_t startIndex)
{
	return HexDigitToDecimal(startIndex) * 16 + HexDigitToDecimal(startIndex + 1);
}

static void Display_software_version(void)
{
	char OutString[12];

	Write("Version ");
	sprintf(OutString, "%d.%2.2d", MajorVersion, MinorVersion);
	Write(OutString);
	Write("  ");
	WriteLn(__DATE__);
#ifdef WITH_TRACKER
	Display_tracker_version();
#endif
#ifdef BNO070
	Write("Tracker:");
	sprintf(OutString, "%u.%u.%u.%lu", BNO070id.swVersionMajor, BNO070id.swVersionMinor, BNO070id.swVersionPatch,
	        BNO070id.swBuildNumber);
	WriteLn(OutString);
#endif
}

void ProcessCommand(void)
{
	const char SENSICS[] = "SENSICS\0";
	char OutString[12];
	eeprom_addr_t EEPROM_addr;

	if (ReadyBufferPos > 0)  // no need to process empty commands
	{
		switch (CommandToExecute[0])
		{
		case '?':
		{
			ProcessInfoCommands();
			break;
		}
#ifdef BNO070
		case 'B':
		case 'b':
		{
			ProcessBNO070Commands();
			break;
		}
#endif
		case 'S':
		case 's':
		{
			ProcessSPICommand();
			break;
		}

		case 'I':
		case 'i':
		{
			ProcessI2CCommand();
			break;
		}
#ifdef SVR_HAVE_FPGA
		case 'F':
		case 'f':
		{
			ProcessFPGACommand();
			break;
		}
#endif
#ifdef SVR_HAVE_TMDS422
		case 't':  // test commands for TMDS 422 switch
		case 'T':
		{
			ProcessTMDSCommand();
			break;
		}
#endif
#ifdef SVR_ENABLE_VIDEO_INPUT

		case 'H':  // HDMI commands
		case 'h':
		{
			ProcessHDMICommand();
			break;
		}
#endif

		case 'P':  // PWM settings
		case 'p':
		{
			set_pwm_values(HexPairToDecimal(2), HexPairToDecimal(2));
			break;
		}
		case 'E':  // Turn echo on/off. Also EEPROM functions
		case 'e':
		{
			switch (CommandToExecute[1])
			{
			case 'I':  // write Sensics ID
			case 'i':
			{
				set_buffer(EEPROM.Buffer, 0);
				memcpy(EEPROM.Signature, SENSICS, sizeof(SENSICS));
/// @todo HDK20RF sizeof(EEPROM.Buffer) was changed to sizeof(SENSICS) - 1 due to "write Sensics ID will overwrite S/N"
#ifndef SVR_IS_HDK_20
				nvm_eeprom_erase_and_write_buffer(SIGNATURE_PAGE * EEPROM_PAGE_SIZE, &EEPROM.Buffer[0],
				                                  sizeof(EEPROM.Buffer));
#else  // 20160605, fctu, fix write SENSICS ID will overwrite S/N.
				nvm_eeprom_erase_and_write_buffer(SIGNATURE_PAGE * EEPROM_PAGE_SIZE, &EEPROM.Buffer[0],
				                                  sizeof(SENSICS) - 1);
#endif
				break;
			}
			case 'W':  // write four bytes of data
			case 'w':
			{
				EEPROM_addr = HexPairToDecimal(2);
/// @todo HDK20RF uses the version of nvm_eeprom_write_byte in its binary-only libhdk20, called nvm_eeprom_write_byte_ -
/// differences unknown
#ifdef SVR_IS_HDK_20
				nvm_eeprom_write_byte_(EEPROM_addr, HexPairToDecimal(4));
				nvm_eeprom_write_byte_(EEPROM_addr + 1, HexPairToDecimal(6));
				nvm_eeprom_write_byte_(EEPROM_addr + 2, HexPairToDecimal(8));
				nvm_eeprom_write_byte_(EEPROM_addr + 3, HexPairToDecimal(10));
#else
				nvm_eeprom_write_byte(EEPROM_addr, HexPairToDecimal(4));
				nvm_eeprom_write_byte(EEPROM_addr + 1, HexPairToDecimal(6));
				nvm_eeprom_write_byte(EEPROM_addr + 2, HexPairToDecimal(8));
				nvm_eeprom_write_byte(EEPROM_addr + 3, HexPairToDecimal(10));
#endif
				break;
			}
			case 'V':  // verify Sensics ID
			case 'v':
			{
#ifndef SVR_IS_HDK_20
				set_buffer(EEPROM.Buffer, 0);
				memcpy(EEPROM.Buffer, SENSICS, sizeof(SENSICS));
				if (is_eeprom_page_equal_to_buffer(SIGNATURE_PAGE, EEPROM.Buffer))
					WriteLn("Verified");
				else
					WriteLn("Not verified");
				break;
#else  // 20160605, fctu, fix write SENSICS ID will overwrite S/N.
				/// @todo HDK20RF presumably related to the above
				memcpy(EEPROM.Buffer, SENSICS, sizeof(SENSICS) - 1);
				if (is_sensics_id_equal_to_buffer(SIGNATURE_PAGE, EEPROM.Buffer, sizeof(SENSICS) - 1))
					WriteLn("Verified");
				else
					WriteLn("Not verified");
				break;
#endif
			}
			case 'R':  // read four bytes of data
			case 'r':
			{
				EEPROM_addr = HexPairToDecimal(2);
				for (int i = 0; i < 4; i++)
				{
					sprintf(OutString, "%d ", nvm_eeprom_read_byte(EEPROM_addr + i));
					Write(OutString);
				}
				WriteEndl();
				break;
			}
			}
			break;
		}
		case 'd':
		case 'D':
		{
			{
				// debug commands
				WriteLn("Set debug level");
				SetDebugLevel(HexPairToDecimal(1));
			}
			break;
		}

#ifdef SVR_IS_HDK_20
		case 'f':  // 20160520, fctu, factory service command.
		case 'F':
		{
			ProcessFactoryCommand();
			break;
		}
#endif  // SVR_IS_HDK_20

		default:
			WriteLn(";Unrecognized command");
		}
	}
	SerialState = AwaitingCommand;  // todo: should this be here?
}

void ProcessInfoCommands(void)

// process commands that start with '?'

{
	char OutString[12];

	switch (CommandToExecute[1])
	{
	case 'V':  // version
	case 'v':
	{
		Display_software_version();
		break;
	}
	case 'C':  // clock
	case 'c':
	{
		// todo: add back clock
		// sprintf(OutString, "%2.2d:%2.2d:%2.2d:%2.2d", day,hour,minute,second);
		WriteLn(OutString);
		break;
	}
	case 'b':  // bootloader
	case 'B':
	{
		if (memcmp(CommandToExecute + 2, "1948", 4) == 0)
		{
			PrepareForSoftwareUpgrade();
		}
		break;
	}
	}
}

#ifdef BNO070
void ProcessBNO070Commands(void)
{
	char OutString[40];

	switch (CommandToExecute[1])
	{
	case 'D':
	case 'd':
	{
		switch (CommandToExecute[2])
		{
		case 'E':
		case 'e':
		{
			// #BDExx - BNO DCD Enable, set DCD enable flags
			int flags = HexPairToDecimal(3);
			if (SetDcdEn_BNO070(flags))
			{
				sprintf(OutString, "Calibration flags set (%02x)", flags);
				WriteLn(OutString);
			}
			else
			{
				WriteLn("Failed.");
			}
			break;
		}
		case 'S':
		case 's':
		{
			// #BDS - BNO DCD Save
			if (SaveDcd_BNO070())
			{
				WriteLn("DCD Saved.");
			}
			else
			{
				WriteLn("Failed.");
			}
			break;
		}
		case 'C':
		case 'c':
		{
			// #BDC = BNO DCD Clear
			if (ClearDcd_BNO070())
			{
				WriteLn("DCD Cleared.");
			}
			else
			{
				WriteLn("Failed.");
			}
			break;
		}
		}
		break;
	}
#ifdef MeasurePerformance
	case 'T':
	case 't':
	{
		TimingDebug_output();
		break;
	}
#endif
	case 'M':
	case 'm':
	{
		switch (CommandToExecute[2])
		{
		case 'E':
		case 'e':
		{
			// #BMExx - BNO Mag Enable
			bool enabled = HexPairToDecimal(3) > 0;
			if (MagSetEnable_BNO070(enabled))
			{
				WriteLn(enabled ? "Mag Enabled." : "Mag Disabled.");
			}
			else
			{
				WriteLn("Failed.");
			}
			break;
		}
		case 'Q':
		case 'q':
		{
			// #BMS - BNO Mag Status
			uint8_t status = MagStatus_BNO070();  // 0 - Unreliable, 1 - Low, 2 - Medium, 3 - High Accuracy.
			sprintf(OutString, "Mag Accuracy: %d", status);
			WriteLn(OutString);
			break;
		}
		}
		break;
	}
	case 'S':
	case 's':
	{
		switch (CommandToExecute[2])
		{
		case 'Q':
		case 'q':
		{
			// #BSQ - BNO Stats Query
			BNO070_Stats_t stats;
			GetStats_BNO070(&stats);  // 0 - Unreliable, 1 - Low, 2 - Medium, 3 - High Accuracy.
			sprintf(OutString, "Resets: %lu", stats.resets);
			WriteLn(OutString);
			sprintf(OutString, "I2C Events: %lu", stats.events);
			WriteLn(OutString);
			sprintf(OutString, "Empty events:%lu", stats.empty_events);
			WriteLn(OutString);
			break;
		}
		}
		break;
	}
	case 'V':
	case 'v':
	{
		switch (CommandToExecute[2])
		{
		case 'V':
		case 'v':
		{
			// #BVVxx log events to serial xx=0 turn off, all else = on
			bool enabled = HexPairToDecimal(3) > 0;
			SetDebugPrintEvents_BNO070(enabled);
			WriteLn(enabled ? "enabled\n" : "disabled\n");
			break;
		}
		}
	}
	case 'H':  // display BNO header
	case 'h':
	{
		sprintf(Msg, "header: %x ", Get_BNO_Report_Header());
		WriteLn(Msg);
		break;
	}
	case 'R':
	case 'r':
	{
		switch (CommandToExecute[2])
		{
		case 'I':
		case 'i':
		{
			// #BRI
			if (ReInit_BNO070())
			{
				WriteLn("Reinitialized");
			}
			else
			{
				WriteLn("Failed");
			}
			break;
		}
		case 'H':
		case 'h':
		{
			// #BRH
			if (Reset_BNO070())
			{
				WriteLn("Reset");
			}
			else
			{
				WriteLn("Failed");
			}
			break;
		}
		}
	}
	}
}
#endif

// send one or more bytes to the SPI interface and prints the received bytes
// (and/or interact with displays, conventionally attached to a Solomon controller on SPI)
void ProcessSPICommand(void)
{
	uint16_t SolID, SolRegister;
	char OutString[12];

	switch (CommandToExecute[1])
	{
	case 'I':  // init command
	case 'i':
	{
		if (CommandToExecute[2] == '1')
		{
			WriteLn("Init Display1");
			Display_Init(Display1);
		}
#ifdef SVR_HAVE_DISPLAY2
		else if (CommandToExecute[2] == '2')
		{
			WriteLn("Init Display2");
			Display_Init(Display2);
		}
#endif  // SVR_HAVE_DISPLAY2
		else
			WriteLn("Bad display ID");
		break;
	}

	case 'n':
	case 'N':
	{
		WriteLn("Display on");
		Display_On(Display1);
#ifdef SVR_ENABLE_VIDEO_INPUT
		VideoInput_Update_Resolution_Detection();
#endif
#ifdef BNO070
		Update_BNO_Report_Header();
#endif
		break;
	}
#ifdef BNO070
	case 'G':
	case 'g':
	{
		if (CommandToExecute[2] == '1')
		{
			SELECT_GRV = 1;
			WriteLn("Game rotation vector");
		}
		else if (CommandToExecute[2] == '0')
		{
			SELECT_GRV = 0;
			WriteLn("Rotation vector");
		}
		else
			WriteLn("Bad parameter");

		SetConfigValue(GRVOffset, SELECT_GRV);
		break;
	}
#endif
	case 'f':
	case 'F':
	{
		WriteLn("Display off");
		Display_Off(Display1);
#ifdef SVR_ENABLE_VIDEO_INPUT
		VideoInput_Update_Resolution_Detection();
#endif
#ifdef BNO070
		Update_BNO_Report_Header();
#endif
		break;
	}

	case '0':  // solomon reset
	{
		Display_Reset(HexDigitToDecimal(2));
		break;
	}
	case '1':  // display power cycle
	{
		if (HexDigitToDecimal(2) == 1)
		{
			Display_Powercycle(Display1);
			// init_solomon_device(Solomon1);
		}
#ifdef SVR_HAVE_DISPLAY2
		else if (HexDigitToDecimal(2) == 2)
		{
			Display_Powercycle(Display2);
			// init_solomon_device(Solomon2);
		}
#endif  // SVR_HAVE_DISPLAY2
		else
			WriteLn("Wrong ID");
		break;
	}
#ifdef SVR_HAVE_SOLOMON
	case 'w':  // Write Solomon register
	case 'W':
	{
		WriteLn("Write");
		uint16_t data;
		data = HexPairToDecimal(5);
		data = (data << 8) + HexPairToDecimal(7);
		if (CommandToExecute[2] == '1')
			write_solomon(Solomon1, HexPairToDecimal(3), data);
#ifdef SVR_HAVE_SOLOMON2
		else if (CommandToExecute[2] == '2')
			write_solomon(Solomon2, HexPairToDecimal(3), data);
#endif  // SVR_HAVE_SOLOMON2
		else
			WriteLn("Wrong Solomon ID");
		break;
	}
	case 'r':  // read Solomon ID or read register
	case 'R':
	{
		if (CommandToExecute[2] == '1')
		{
			if (ReadyBufferPos < 4)  // no register number, just read ID
			{
				WriteLn("Read ID");
				SolID = read_Solomon_ID(Solomon1);
				sprintf(OutString, "Id %x", SolID);
			}
			else
			{
				write_solomon(Solomon1, 0xd4, 0xfa);  // about to read register
				SolRegister = read_solomon(Solomon1, HexPairToDecimal(3));
				sprintf(OutString, "Read: %x", SolRegister);
			}
		}
#ifdef SVR_HAVE_SOLOMON2
		else if (CommandToExecute[2] == '2')
		{
			if (ReadyBufferPos < 4)  // no register number, just read ID
			{
				WriteLn("Read ID");
				SolID = read_Solomon_ID(Solomon2);
				sprintf(OutString, "Id %x", SolID);
			}
			else
			{
				write_solomon(Solomon2, 0xd4, 0xfa);  // about to read register
				SolRegister = read_solomon(Solomon2, HexPairToDecimal(3));
				sprintf(OutString, "Read: %x", SolRegister);
			}
		}
#endif  // SVR_HAVE_SOLOMON2
		else
		{
			WriteLn("Wrong Solomon ID");
			return;
		}
		WriteLn(OutString);
		break;
	}
	case 'u':
	case 'U':
	{
		if (CommandToExecute[2] == '1')
			raise_sdc(Solomon1);
#ifdef SVR_HAVE_SOLOMON2
		else if (CommandToExecute[2] == '2')
			raise_sdc(Solomon2);
#endif  // SVR_HAVE_SOLOMON2
		else
			WriteLn("Wrong Solomon ID");
		break;
	}
	case 'd':
	case 'D':
	{
		if (CommandToExecute[2] == '1')
			lower_sdc(Solomon1);
#ifndef OSVRHDK
		else if (CommandToExecute[2] == '2')
			lower_sdc(Solomon2);
		else if (CommandToExecute[2] == '3')
		{
			WriteLn("OE high");
			ioport_configure_pin(SPI_Mux_OE, IOPORT_DIR_OUTPUT | IOPORT_INIT_LOW);
			gpio_set_pin_high(SPI_Mux_OE);
		}
		else if (CommandToExecute[2] == '4')
			gpio_toggle_pin(SPI_Mux_OE);
#endif
		else
			WriteLn("Wrong Solomon ID");
		break;
	}
	case 'p':  // persistance settings
	case 'P':
	{
		Display_Set_Strobing(HexDigitToDecimal(2), HexPairToDecimal(3), HexPairToDecimal(5));
		break;
	}
#endif  // SVR_HAVE_SOLOMON
	}

	WriteLn("");
}

// send one or more bytes to the I2C interface and prints the received bytes

void ProcessI2CCommand(void)

{
	uint8_t TxByte, RxByte, Num, Page;
	bool Result = false;

	char OutString[14];

	switch (CommandToExecute[1])
	{
#ifdef BNO070
	case 'B':
	case 'b':  // BNO commands
	{
		switch (CommandToExecute[2])  // sub-commands for NXP
		{
		// to read a 1-byte Hex value after the subcommand use something like param=HexToDecimal(3)
		// to read a 2-byte Hex value after the subcommand use something like param=HexPairToDecimal(3)
		case 'T':  // tare
		case 't':
		{
			Tare_BNO070();
		}
		break;
		}
	}
	break;

#endif

#ifdef SVR_USING_NXP

	case 'D':  // Select I2C device
	case 'd':
	{
		I2CAddress = HexPairToDecimal(2);  // since command type is in index 1, start at 2
		WriteLn(";Address received");
		break;
	}

	case 'R':  // Receive Data Command
	case 'r':
	{
		Num = HexPairToDecimal(2);  // num of bytes to receive

		// todo: replace with real stuff
		RxByte = 0;
		// RxByte=I2CReadRegister(I2CAddress,Num);
		sprintf(OutString, "R %x ", RxByte);
		Write(OutString);
		break;
	}

	case 'f':
	case 'F':  // find available I2C addresses
	{
		for (int i = 0x80; i < 0xA0; i = i + 2)
		{
			sprintf(Msg, "Trying %x", i);
			WriteLn(Msg);
			if (I2CReadRegister(i, 0))
				WriteLn("Success");
			_delay_ms(100);
		}
		break;
	}

	case 'N':  // NXP commands
	case 'n':
	{
		switch (CommandToExecute[2])  // sub-commands for NXP
		{
		case 'W':  // write register
		case 'w':
		{
			Num = HexPairToDecimal(3);     // Register number
			TxByte = HexPairToDecimal(5);  // byte to send
			if (NXPLeftSide == false)
				Result = i2cWriteRegister(NXP_1_ADDR, Num, TxByte);
#ifdef SVR_HAVE_NXP2
			else
				Result = i2cWriteRegister(NXP_2_ADDR, Num, TxByte);
#endif
			if (!Result)
				Write(";Write failed");
			break;
		}

		case 'R':  // read register
		case 'r':
		{
			Num = HexPairToDecimal(3);  // Register number
			if (NXPLeftSide == false)
				RxByte = NXPReadRegister(NXP_1_ADDR, Num);
#ifdef SVR_HAVE_NXP2
			else
				RxByte = NXPReadRegister(NXP_2_ADDR, Num);
#endif
			sprintf(OutString, "%x ", RxByte);
			Write(OutString);
			break;
		}
		case 'X':  // read register through NXP library
		case 'x':
		{
			//#define ACC_REG(a, p) (UInt16)(((p) << 8) | (a))
			UInt32 errCode;

			Num = HexPairToDecimal(3);  // Register number
			Page = HexPairToDecimal(5);
			if (NXPLeftSide == false)
				errCode = tmbslTDA1997XReadI2C(0, ACC_REG(Num, Page), 1, &RxByte);
			// RxByte=NXPReadRegister(NXP_1_ADDR, Num);
			else
				errCode = tmbslTDA1997XReadI2C(1, ACC_REG(Num, Page), 1, &RxByte);  /// temp. Should be unit 1
			// RxByte=NXPReadRegister(NXP_2_ADDR, Num);
			if (errCode != TM_OK)
				sprintf(OutString, "Err %ld ", errCode);
			else
				sprintf(OutString, "%x ", RxByte);
			Write(OutString);
			delay_ms(1);
			if (NXPLeftSide == false)
				errCode = tmbslTDA1997XReadI2C(0, ACC_REG(Num, Page), 1, &RxByte);
			// RxByte=NXPReadRegister(NXP_1_ADDR, Num);
			else
				errCode = tmbslTDA1997XReadI2C(1, ACC_REG(Num, Page), 1, &RxByte);  /// temp. Should be unit 1
			// RxByte=NXPReadRegister(NXP_2_ADDR, Num);
			if (errCode != TM_OK)
				sprintf(OutString, "Err %ld ", errCode);
			else
				sprintf(OutString, "%x ", RxByte);
			Write(OutString);
			delay_ms(1);
			if (NXPLeftSide == false)
				errCode = tmbslTDA1997XReadI2C(0, ACC_REG(Num, Page), 1, &RxByte);
			// RxByte=NXPReadRegister(NXP_1_ADDR, Num);
			else
				errCode = tmbslTDA1997XReadI2C(1, ACC_REG(Num, Page), 1, &RxByte);  /// temp. Should be unit 1
			// RxByte=NXPReadRegister(NXP_2_ADDR, Num);
			if (errCode != TM_OK)
				sprintf(OutString, "Err %ld ", errCode);
			else
				sprintf(OutString, "%x ", RxByte);
			Write(OutString);
			break;
		}
		case 'S':  // side
		case 's':
		{
			NXPLeftSide = (CommandBuffer[3] == '0');
			break;
		}
		case '0':
		{
			NXP_Suspend();
			break;
		}
		case '1':
		{
			NXP_Resume();
			break;
		}
		}
		break;
	}

#endif
	}

	WriteEndl();
}

#ifdef SVR_HAVE_FPGA
void ProcessFPGACommand(void)

// #FnRxx reads address XX from FPGA n
// #FnWxxyy writes value YY to address XX in FPGA n

{
	uint8_t FPGANum, Addr, Data;

	if (CommandToExecute[1] == '2')
		FPGANum = 2;
	else if (CommandToExecute[1] == '1')
		FPGANum = 1;
	else
	{
		WriteLn("Bad command");
		return;
	}
	switch (CommandToExecute[2])
	{
	case 'r':
	case 'R':
	{
		WriteLn("Read");
		Addr = HexPairToDecimal(3);
		sprintf(Msg, "%x = %x", Addr, FPGA_read(FPGANum, Addr));
		WriteLn(Msg);
		WriteLn("After");
		break;
	}
	case 'w':
	case 'W':
	{
		WriteLn("Write");
		Addr = HexPairToDecimal(3);
		Data = HexPairToDecimal(5);
		FPGA_write(FPGANum, Addr, Data);
		break;
	}
	case 's':
	case 'S':  // toggle side by side
	{
		WriteLn("Toggle side by side");
#ifndef OSVRHDK
		ioport_toggle_pin_level(Side_by_side_A);
#endif
		ioport_toggle_pin_level(Side_by_side_B);
#ifdef OSVRHDK
		SideBySideMode = ~SideBySideMode;
		SetConfigValue(SideBySideOffset, SideBySideMode);
#endif
		break;
	}
	case '0':
	{
		WriteLn("FPGA Reset");
		FPGA_reset();
		break;
	}
	case 'l':
	case 'L':
	{
#ifdef SVR_HAVE_FPGA_VIDEO_LOCK_PIN
		if (ioport_get_pin_level(FPGA_unlocked))
			WriteLn("FPGA unlocked");
		else
			WriteLn("FPGA locked");
#endif  // SVR_HAVE_FPGA_VIDEO_LOCK_PIN
		break;
	}
	}
}
#endif  // SVR_HAVE_FPGA

#ifdef SVR_ENABLE_VIDEO_INPUT

void ProcessHDMICommand(void)

{
	switch (CommandToExecute[1])
	{
	// Vendor-independent commands - routed through VideoInput interface.
	case 'I':
	case 'i':
	{
		WriteLn(";Init HDMI");
		VideoInput_Init();
		WriteLn(";End init");
		break;
	}

	case 'R':
	case 'r':
	{
		VideoInput_Report_Status();
		break;
	}
	case 'T':
	case 't':
	{
		HDMI_task = true;
		break;
	}
	case '0':
	{
		VideoInput_Reset(HexDigitToDecimal(2));
		break;
	}
#ifdef SVR_HAVE_NXP
	case 'S':
	case 's':
	{
		NXP_HDMIShadow = (CommandToExecute[2] == '1');
		if (NXP_HDMIShadow)
			WriteLn("Shadow on");
		break;
	}
	case 'E':
	case 'e':
	{
		WriteLn("Pulse HPD");
		tmdlHdmiRxManualHPD(0, TMDL_HDMIRX_HPD_PULSE);
		break;
	}
	case 'H':
	case 'h':
	{
		WriteLn("Pulse HPD 1sec");
		tmdlHdmiRxManualHPD(0, TMDL_HDMIRX_HPD_HIGH);
		_delay_ms(1000);
		tmdlHdmiRxManualHPD(0, TMDL_HDMIRX_HPD_LOW);
		break;
	}
	case 'V':
	case 'v':
	{
		uint8_t i;
		// move both NXP to page 0
		i2cWriteRegister(NXP_1_ADDR, 0xff, 0);
#ifdef SVR_HAVE_NXP2
		i2cWriteRegister(NXP_2_ADDR, 0xff, 0);
#endif
		for (i = 0; i < 10; i++)
		{
			NXPReadRegister(NXP_1_ADDR, 0);
#ifdef SVR_HAVE_NXP2
			NXPReadRegister(NXP_2_ADDR, 0);
#endif
		}
		break;
	}
	case 'M':
	case 'm':
	{
		if (CommandBuffer[2] == '0')
		{
			WriteLn(";Program MTP");
			_delay_ms(50);
			NXP_Program_MTP0();
		}
#ifdef SVR_HAVE_NXP2
		if (CommandBuffer[2] == '1')
		{
			WriteLn(";Program MTP1");
			_delay_ms(50);
			NXP_Program_MTP1();
		}
#endif
		break;
	}
#endif  // SVR_HAVE_NXP
	}
}

#endif  // SVR_ENABLE_VIDEO_INPUT

#ifdef TMDS422
void ProcessTMDSCommand(void)

{
	uint8_t Temp;

	switch (CommandToExecute[1])
	{
	case 'r':
	case 'R':  // read status
	{
		if (ReadTMDS422Status(HexDigitToDecimal(2), &Temp))
		{
			sprintf(Msg, "Read:%x", Temp);
			WriteLn(Msg);
		}
		else
			WriteLn("Read failed");
		break;
	}
	case 'w':
	case 'W':  // write
	{
		HDMI_config(HexPairToDecimal(2), HexPairToDecimal(4));
		break;
	}
	case 's':  // set input status
	case 'S':
	{
		Temp = HexPairToDecimal(2);
		SetInputStatus(Temp);
		break;
	}
	case 't':  // start task
	case 'T':
	{
		HDMISwitch_task = true;
#ifdef TMDS422
// TMDS_422_Task();
// timeout_start_periodic(	TMDS_422_Timeout, 1);
#endif
		break;
	}
	case 'p':
	case 'P':
	{
		WriteLn("Toggle backlight");
		ioport_toggle_pin_level(PWM_A);
		ioport_toggle_pin_level(PWM_B);
		break;
	}
	case 'i':
	case 'I':
	{
		WriteLn("Init switch");
		InitHDMISwitch();
		break;
	}
	default:
	{
		WriteLn(";Unknown command");
		break;
	}
	}
}
#endif
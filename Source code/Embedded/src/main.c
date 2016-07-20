/**
 * \file
 *
 * \brief CDC Application Main functions
 *
 * Copyright (c) 2011-2012 Atmel Corporation. All rights reserved.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. The name of Atmel may not be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * 4. This software may only be redistributed and used in connection with an
 *    Atmel microcontroller product.
 *
 * THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * \asf_license_stop
 *
 */

#include <asf.h>
#include <pmic.h>
#include "conf_usb.h"
#include "conf_timeout.h"
#include "ui.h"
#include "uart.h"
#include "SerialStateMachine.h"
#include "DeviceDrivers/Solomon.h"
#include "my_hardware.h"
#include "bno_callbacks.h"

#ifndef DISABLE_NXP
#include "nxp/AVRHDMI.h"
#endif

#include "DeviceDrivers/Display.h"
#include "DeviceDrivers/VideoInput.h"
#include "DeviceDrivers/TI-TMDS442.h"
#include "DeviceDrivers/BNO070.h"
#include "Console.h"

#include "TimingDebug.h"

#ifdef OSVRHDK
#include "nxp\i2c.h"
#include "string.h"
#endif

#include "USB.h"

#include "main.h"

bool HDMI_task = false;
bool HDMISwitch_task = true;
bool NewVideoDetected = false;
bool VideoLost = false;
bool NXPEverLocked = false;  // true if HDMI receiver was ever locked on incoming video signal

#ifdef OSVRHDK
bool LastFPGALockStatus = 0;  // last state of FPGA_unlocked pin
#endif

void HandleHDMI(void);

void load_configuration(void)

{
	if (IsConfigOffsetValid(PersistenceOffset))
		Display_Strobing_Rate = GetConfigValue(PersistenceOffset);
	else
		SetConfigValue(PersistenceOffset, Display_Strobing_Rate);

	if (IsConfigOffsetValid(PersistencePercentOffset))
		Display_Strobing_Percent = GetConfigValue(PersistencePercentOffset);
	else
		SetConfigValue(PersistencePercentOffset, Display_Strobing_Percent);
}

/*! \brief Main function. Execution starts here.
 */
int main(void)
{
	//_StackPaint();

	irq_initialize_vectors();

	// Initialize the sleep manager
	sysclk_init();
	pmic_init();
	sleepmgr_init();

	board_init();

	custom_board_init();  // add initialization that is specific to Sensics board
	// timeout_init(); //- timeouts not working quite yet // todo: activate this
	cpu_irq_enable();

	ui_init();
	ui_powerdown();

	load_configuration();

	Display_System_Init();

	// init the incoming serial state machine
	InitSerialState();

	ioport_set_pin_high(USB_Hub_Reset_Pin);  // free hub from reset
	// disable this after removing the appropriate resistor
	ioport_set_pin_low(USB_Hub_Power_Pin);  // enable power on hub

// delay_ms(100);

// delay_s(1);
// to assist in debug
// WriteLn("Start");

// sleepmgr_lock_mode(SLEEPMGR_IDLE); // allow timers
// timeout_start_periodic(Debug_LED_Timeout, 1);
// timeout_start_periodic(	TMDS_422_Timeout, 1);

// The main loop manages only the power mode
// because the USB management is done by interrupt

#ifdef TMDS422
	InitHDMISwitch();
	TMDS_422_Task();
#endif  // TMDS422

#ifdef OSVRHDK
// ioport_set_pin_low(FPGA_Reset_Pin);	// hold FPGA reset
#ifdef MeasurePerformance
	TimingDebug_init();
#endif  // MeasurePerformance
#endif  // OSVRHDK

#ifdef SVR_USING_NXP

	VideoInput_Init();  // make sure Solomon is init before HDMI because HDMI init assumes that I2C port for NXP2 has
	                    // already been initialized

	if (NewVideoDetected)
	{
		WriteLn("Video at start");
#ifdef SVR_HAVE_DISPLAY1
		Display_On(Display1);
		VideoInput_Update_Resolution_Detection();
#ifdef BNO070
		Update_BNO_Report_Header();
#endif  // BNO070
#endif  // SVR_HAVE_DISPLAY1
#ifdef SVR_HAVE_DISPLAY2
		Display_On(Display2);
#endif  // SVR_HAVE_DISPLAY2
		NewVideoDetected = false;
	};
#ifdef OSVRHDK

	if (!(ioport_get_pin_level(FPGA_unlocked)))
		NXPEverLocked = true;
#endif  // OSVRHDK

	if (VideoLost)
	{
#ifdef SVR_HAVE_DISPLAY1
		Display_Off(Display1);
		VideoInput_Update_Resolution_Detection();
#ifdef BNO070
		Update_BNO_Report_Header();
#endif  // BNO070

#endif  // SVR_HAVE_DISPLAY1
#ifdef SVR_HAVE_DISPLAY2
		Display_Off(Display2);
#endif  // SVR_HAVE_DISPLAY1
		VideoLost = false;
	}

#ifdef DSIGHT
	/// @todo isn't this redundant with the NewVideoDetected check above? or is the waiting for 1 second important for
	/// dSight specifically?
	if (NewVideoDetected)
	{
		delay_ms(1000);
		Display_Init(Display1);
		Display_Init(Display2);
	}
#endif  // DSIGHT

	// ProgramMTP0();

	HDMI_task = true;

#endif

#ifdef BNO070
	BNO070Active = init_BNO070();
#if defined(OSVRHDK) && !defined(HDK_20)
	if (BNO070Active)
	{
		/// If tracker version is 1.8.4 or greater...
		if (1 < BNO070id.swVersionMajor || (1 == BNO070id.swVersionMajor && 8 < BNO070id.swVersionMinor) ||
		    (1 == BNO070id.swVersionMajor && 8 == BNO070id.swVersionMinor && 4 <= BNO070id.swVersionPatch))
		{
			HDK_Version_Major = 1;
			HDK_Version_Minor = 3;
			strcpy(ProductName, "OSVR HDK 1.3");  /// important! make sure did length of this product name is not longer
			                                      /// than original name defined in udc.h and in my_hardware.c
		}
	}
#endif  // OSVRHDK && !HDK_20
#endif

	// Start USB stack to authorize VBus monitoring
	udc_start();

	uint16_t slower = 0;

#ifdef OSVRHDK
// ioport_set_pin_high(FPGA_Reset_Pin);	// release FPGA reset
#endif

#ifdef OSVRHDK
	LastFPGALockStatus = ioport_get_pin_level(FPGA_unlocked);  // last state of FPGA_unlocked pin
#endif

	while (true)
	{
		// sleepmgr_enter_sleep(); // todo - probably remove this since the board has to work without USB
		if (CommandReady)
		{
			ProcessCommand();
			CommandReady = false;
		}

		delay_us(50);  // Some delay is required to allow USB interrupt to process

#ifdef BNO070
#ifdef OSVRHDK
		BNO_Yield();
#endif
#endif

#ifdef TMDS422
		if (HDMISwitch_task)  //(timeout_test_and_clear_expired(TMDS_422_Timeout))
		{
			// check status of HDMI switch
			TMDS_422_Task();
		}
#endif

//#ifndef OSVRHDK // disable checking the NXP here; video changes will be picked up by interrupts anyway
#ifndef DISABLE_NXP
		slower++;  // used to slow down the rate of checking HDMI
		if ((HDMI_task) && (slower > 1000))
		{
			slower = 0;
#ifdef OSVRHDK
			bool NewFPGALockStatus;
			NewFPGALockStatus = ioport_get_pin_level(FPGA_unlocked);
			if (NewFPGALockStatus)
			{
				if (LastFPGALockStatus != NewFPGALockStatus)
				{
					// WriteLn("Video signal lost");

					LastFPGALockStatus = NewFPGALockStatus;

#ifdef Solomon1_SPI
					Display_Off(Solomon1);
					NXP_Update_Resolution_Detection();
#ifdef BNO070
					Update_BNO_Report_Header();
#endif  // BNO070
#endif  // Solomon1_SPI
#ifdef Solomon2_SPI
					Display_Off(Solomon2);
#endif  // Solomon2_SPI
				}
			}
			else  // FPGA is locked
			{
				if (!NXPEverLocked)
				{
					// WriteLn("First lock");
					NXPEverLocked = true;
					// Init_HDMI();
					//#ifdef Solomon1_SPI
					// Display_On(Solomon1);
					//#endif
					//#ifdef Solomon2_SPI
					// Display_On(Solomon2);
					//#endif
				}
				if (LastFPGALockStatus != NewFPGALockStatus)
				{
					// WriteLn("Video signal detected");
					LastFPGALockStatus = NewFPGALockStatus;
#ifdef Solomon1_SPI
					Display_On(Solomon1);
					NXP_Update_Resolution_Detection();
#ifdef BNO070
					Update_BNO_Report_Header();
#endif  // BNO070
#endif  // Solomon1_SPI
#ifdef Solomon2_SPI
					Display_On(Solomon2);
#endif  // Solomon2_SPI
				}
			}
#else  // OSVR_HDK ^ / v dSight
			HandleHDMI();
#endif
		}
#endif
		//#endif // #ifndef OSVRHDK
	}
}

void HandleHDMI()

{
#ifndef DISABLE_NXP
	NXP_HDMI_Task();
	if (NewVideoDetected)
	{
		NewVideoDetected = false;
// WriteLn("New video detected");
#ifdef Solomon1_SPI
#ifndef H546DLT01
		init_solomon_device(Solomon1);  // todo: add back after debug of board
#endif
		Display_On(Solomon1);
		NXP_Update_Resolution_Detection();
#ifdef BNO070
		Update_BNO_Report_Header();
#endif

#ifndef H546DLT01
		init_solomon_device(Solomon1);  // todo: add back after debug of board
#endif
#endif
#ifdef Solomon2_SPI
#ifndef H546DLT01
		init_solomon_device(Solomon2);
#endif
		Display_On(Solomon2);  // added to make it same process as solomon 1
#ifndef H546DLT01
		init_solomon_device(Solomon2);
#endif
#endif
	}
	if (VideoLost)
	{
		VideoLost = false;
// WriteLn("Video lost");

#ifdef Solomon1_SPI
		Display_Off(Solomon1);
		NXP_Update_Resolution_Detection();
#ifdef BNO070
		Update_BNO_Report_Header();
#endif
#endif
#ifdef Solomon2_SPI
		Display_Off(Solomon2);
#endif
	}
#endif
}

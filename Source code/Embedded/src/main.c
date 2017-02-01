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

#include "main.h"
#include "Boot.h"

#include <asf.h>
#include <pmic.h>
#include "conf_usb.h"
#include "conf_timeout.h"
#include "ui.h"
#include "uart.h"
#include "SerialStateMachine.h"
#include "my_hardware.h"

#include "DeviceDrivers/Display.h"
#include "DeviceDrivers/VideoInput.h"
#include "DeviceDrivers/BNO070.h"
#include "Console.h"

#include "TimingDebug.h"

#include "USB.h"
#include "SvrYield.h"

/// The HDK 1.x OLED firmware works across lots of hardware versions, so we determine a product string at runtime based
/// on the BNO firmware version (loaded at the factory).
#if defined(SVR_IS_HDK_1_x) && defined(H546DLT01) && defined(BNO070)
#include "string.h"  // for strcpy
#endif

bool HDMI_task = false;
bool HDMISwitch_task = true;

void HandleHDMI(void);

static void load_configuration(void)
{
	GetValidConfigValueOrWriteDefault(PersistenceOffset, Display_Strobing_Rate, &Display_Strobing_Rate);
	GetValidConfigValueOrWriteDefault(PersistencePercentOffset, Display_Strobing_Percent, &Display_Strobing_Percent);
}

/*! \brief Main function. Execution starts here.
 */
int main(void)
{
	//_StackPaint();

	// In case the bootloader that's on board isn't working right or the board
	// isn't working right, give the switch a bit of a chance to be read.
	// Should be unnecessary, but extant hardware is reluctant to respond to
	// the bootloader switch.
	CheckForBootloaderSwitchOnStartup();

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

	// Sets up video display part of data path: MIPI bridge (Solomon) or other output device
	Display_System_Full_Init();

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

#ifdef OSVRHDK
// ioport_set_pin_low(FPGA_Reset_Pin);	// hold FPGA reset
#ifdef MeasurePerformance
	TimingDebug_init();
#endif  // MeasurePerformance
#endif  // OSVRHDK

#ifdef SVR_ENABLE_VIDEO_INPUT
#ifdef BNO070
	BNO070Active = init_BNO070();
#ifdef SVR_NEED_TO_COMPUTE_PRODUCT_FROM_BNO
	if (BNO070Active)
	{
		/// If tracker version is 1.8.4 or greater...
		if (1 < BNO070id.swVersionMajor || (1 == BNO070id.swVersionMajor && 8 < BNO070id.swVersionMinor) ||
		    (1 == BNO070id.swVersionMajor && 8 == BNO070id.swVersionMinor && 4 <= BNO070id.swVersionPatch))
		{
			HDK_Version_Major = 1;
			HDK_Version_Minor = 3;
			static const char HDK13_VER_STRING[] = "OSVR HDK 1.3+";
			_Static_assert(
			    sizeof(HDK13_VER_STRING) <= sizeof(USB_DEVICE_PRODUCT_NAME),
			    "HDK 1.3 product name string exceeds the length of USB_DEVICE_PRODUCT_NAME (in conf_usb.h)!");
			_Static_assert(sizeof(HDK13_VER_STRING) <= SVR_HDK_PRODUCT_NAME_STRING_LENGTH,
			               "HDK 1.3 product name string exceeds the length of ProductName (in my_hardware.c)!");
			strcpy(ProductName,
			       HDK13_VER_STRING);  /// important! make sure did length of this product name is not longer
			                           /// than original name defined in udc.h and in my_hardware.c
		}
	}
#endif  // SVR_NEED_TO_COMPUTE_PRODUCT_FROM_BNO
#endif  // BNO070

	// Start USB stack to authorize VBus monitoring
	udc_start();

	// Sets up video input part of data path: switch (if present), HDMI receiver.
	VideoInput_Init();  // make sure Solomon is init before HDMI because HDMI init assumes that I2C port for NXP2 has
	                    // already been initialized
	// Poll once on startup to see if we have video at start.
	VideoInput_Poll_Status();
	HandleHDMI();

#ifdef DSIGHT
	/// @todo isn't this redundant with the videoDetected check above? or is the waiting for 1 second important for
	/// dSight specifically?
	if (VideoInput_Events.videoDetected)
	{
		delay_ms(1000);
		Display_Init(Display1);
		Display_Init(Display2);
	}
#endif  // DSIGHT

// ProgramMTP0();
#endif  // SVR_ENABLE_VIDEO_INPUT

#ifdef SVR_VIDEO_INPUT_POLL_INTERVAL
	uint16_t videoPollCounter = 0;
#endif

	// Main loop
	while (true)
	{
		if (CommandReady)
		{
			ProcessCommand();
			CommandReady = false;
		}

		delay_us(50);  // Some delay is required to allow USB interrupt to process
		svr_yield();

#ifdef SVR_ENABLE_VIDEO_INPUT
#ifdef SVR_VIDEO_INPUT_POLL_INTERVAL
		if (HDMI_task)
		{
			videoPollCounter++;
			if (videoPollCounter > SVR_VIDEO_INPUT_POLL_INTERVAL)
			{
				videoPollCounter = 0;
				VideoInput_Poll_Status();
			}
		}
#endif  // SVR_VIDEO_INPUT_POLL_INTERVAL
		HandleHDMI();
#endif  // SVR_ENABLE_VIDEO_INPUT
	}
}

static void local_display_on(uint8_t id);
static void local_display_off(uint8_t id);
inline static void local_display_on(uint8_t id)
{
/// @todo why display init every time display goes on here? is this a panel quirk that should be handled in
/// panel-specific code?
#if !defined(H546DLT01) && !defined(OSVRHDK)
	Display_Init(id);  // todo: add back after debug of board
#endif
	Display_On(id);
	if (id == Display1)
	{
		VideoInput_Update_Resolution_Detection();
#ifdef BNO070
		Update_BNO_Report_Header();
#endif
	}
/// @todo and similarly, why display init again afterwards?
#if !defined(H546DLT01) && !defined(OSVRHDK)
	Display_Init(id);  // todo: add back after debug of board
#endif
}
inline static void local_display_off(uint8_t id)
{
	Display_Off(id);
	if (id == Display1)
	{
		VideoInput_Update_Resolution_Detection();
#ifdef BNO070
		Update_BNO_Report_Header();
#endif
	}
}
void HandleHDMI()

{
#ifdef SVR_ENABLE_VIDEO_INPUT
	VideoInput_Task();
	bool videoLost = VideoInput_Events.videoLost;
	VideoInput_Events.videoLost = false;
	bool videoDetected = VideoInput_Events.videoDetected;
	VideoInput_Events.videoDetected = false;
	if (videoDetected)
	{
		WriteLn("New video detected");
#ifdef SVR_HAVE_DISPLAY1
		local_display_on(Display1);
#endif
#ifdef SVR_HAVE_DISPLAY2
		local_display_on(Display2);
#endif
	}
	if (videoLost)
	{
		WriteLn("Video lost");
#ifdef SVR_HAVE_DISPLAY1
		local_display_off(Display1);
#endif
#ifdef SVR_HAVE_DISPLAY2
		local_display_off(Display2);
#endif
	}
#endif  // SVR_ENABLE_VIDEO_INPUT
}

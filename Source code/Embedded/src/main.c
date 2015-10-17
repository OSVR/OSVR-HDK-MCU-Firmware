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

#include "DeviceDrivers/TI-TMDS442.h"
#include "DeviceDrivers/BNO070.h"
#include "Console.h"

#include "TimingDebug.h"

#ifdef OSVRHDK
    #include "nxp\i2c.h"
#endif

#include "main.h"

static volatile bool main_b_cdc_enable = false;

bool HDMI_task=false;
bool HDMISwitch_task=true;
bool NewVideoDetected=false;
bool VideoLost=false;
bool NXPEverLocked=false; // true if HDMI receiver was ever locked on incoming video signal


#ifdef OSVRHDK
bool LastFPGALockStatus=0; // last state of FPGA_unlocked pin
#endif

void HandleHDMI(void);

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

    custom_board_init(); // add initialization that is specific to Sensics board
    //timeout_init(); //- timeouts not working quite yet // todo: activate this
    cpu_irq_enable();

    ui_init();
    ui_powerdown();


    init_solomon();


    // init the incoming serial state machine
    InitSerialState();

    ioport_set_pin_high(USB_Hub_Reset_Pin); // free hub from reset
    // disable this after removing the appropriate resistor
    ioport_set_pin_low(USB_Hub_Power_Pin); // enable power on hub




    // Start USB stack to authorize VBus monitoring
    udc_start();
    //delay_ms(100);
	
	//delay_s(1);
	// to assist in debug
	//WriteLn("Start");


    //sleepmgr_lock_mode(SLEEPMGR_IDLE); // allow timers
    //timeout_start_periodic(Debug_LED_Timeout, 1);
    //timeout_start_periodic(	TMDS_422_Timeout, 1);

    // The main loop manages only the power mode
    // because the USB management is done by interrupt

#ifdef TMDS422
    InitHDMISwitch();
    TMDS_422_Task();
#endif

#ifdef OSVRHDK
	//ioport_set_pin_low(FPGA_Reset_Pin);	// hold FPGA reset
	TimingDebug_init();
#endif


#ifndef DISABLE_NXP

    Init_HDMI(); // make sure Solomon is init before HDMI because HDMI init assumes that I2C port for NXP2 has already been initizliaed
	
	if (NewVideoDetected)
	{
		WriteLn("Video at start");
#ifdef Solomon1_SPI
		DisplayOn(Solomon1);
#endif
#ifdef Solomon2_SPI
		DisplayOn(Solomon2);
#endif
		NewVideoDetected=false;
	
	};
#ifdef OSVRHDK

	if (!(ioport_get_pin_level(FPGA_unlocked)))
		NXPEverLocked=true;
#endif
			
	if (VideoLost)
	{
#ifdef Solomon1_SPI
		DisplayOff(Solomon1);
#endif
#ifdef Solomon2_SPI
		DisplayOff(Solomon2);
#endif
		VideoLost=false;
	}
	
		
    //ProgramMTP0();

    HDMI_task=true;
	
#endif

#ifdef BNO070
	BNO070Active=init_BNO070();
#endif


	uint16_t slower = 0;

#ifdef OSVRHDK
	//ioport_set_pin_high(FPGA_Reset_Pin);	// release FPGA reset
#endif

#ifdef OSVRHDK
	LastFPGALockStatus=ioport_get_pin_level(FPGA_unlocked); // last state of FPGA_unlocked pin
#endif

	
    while (true) {
        //sleepmgr_enter_sleep(); // todo - probably remove this since the board has to work without USB
        if (CommandReady)
        {
            ProcessCommand();
            CommandReady=false;
        }

        delay_us(50); // Some delay is required to allow USB interrupt to process


#ifdef BNO070
		//if (CDCWriteInProgress)
			//UpdateCDCBusyStatus();
		//else // check tracker only if we can actually report its results
		{
	        if (BNO070Active)
			{
				if (bno_data_ready>0)
				{
					TimingIncreaseCounter();
					Check_BNO070();
				}
			}
		}
#endif

#ifdef TMDS422
        if (HDMISwitch_task) //(timeout_test_and_clear_expired(TMDS_422_Timeout))
        {
            // check status of HDMI switch
            TMDS_422_Task();
        }
#endif

#ifndef DISABLE_NXP
		slower++; // used to slow down the rate of checking HDMI
		if ((HDMI_task) && (slower>1000))
		{
			slower=0;
	#ifdef OSVRHDK
			bool NewFPGALockStatus;
			NewFPGALockStatus=ioport_get_pin_level(FPGA_unlocked);
			if (NewFPGALockStatus)
			{
				if (LastFPGALockStatus!=NewFPGALockStatus)
				{
					//WriteLn("Video signal lost");
#ifdef OSVRHDK
					LastFPGALockStatus=NewFPGALockStatus;
#endif

#ifdef Solomon1_SPI
					DisplayOff(Solomon1);
#endif
#ifdef Solomon2_SPI
					DisplayOff(Solomon2);
#endif
				}
			}
			else // FPGA is locked
			{
				if (!NXPEverLocked)
				{
					//WriteLn("First lock");
					NXPEverLocked=true;
					//Init_HDMI();
//#ifdef Solomon1_SPI
					//DisplayOn(Solomon1);
//#endif
//#ifdef Solomon2_SPI
					//DisplayOn(Solomon2);
//#endif
				}
				if (LastFPGALockStatus!=NewFPGALockStatus)
				{
					//WriteLn("Video signal detected");
					LastFPGALockStatus=NewFPGALockStatus;
#ifdef Solomon1_SPI
					DisplayOn(Solomon1);
#endif
#ifdef Solomon2_SPI
					DisplayOn(Solomon2);
#endif
				}
					
			}
	#else // dSight
			HandleHDMI();
	#endif
		}
#endif
	}
}

void HandleHDMI()

{
	HDMITask();
	if (NewVideoDetected)
	{
		NewVideoDetected=false;
		//WriteLn("New video detected");
#ifdef Solomon1_SPI
	#ifndef H546DLT01
		init_solomon_device(Solomon1); // todo: add back after debug of board
	#endif
		DisplayOn(Solomon1);
	#ifndef H546DLT01
		init_solomon_device(Solomon1); // todo: add back after debug of board
	#endif
#endif
#ifdef Solomon2_SPI
	#ifndef H546DLT01
		init_solomon_device(Solomon2);
	#endif
		DisplayOn(Solomon2); // added to make it same process as solomon 1
	#ifndef H546DLT01
		init_solomon_device(Solomon2);
	#endif
#endif
	}
	if (VideoLost)
	{

		VideoLost=false;
		//WriteLn("Video lost");
				
#ifdef Solomon1_SPI
		DisplayOff(Solomon1);
#endif
#ifdef Solomon2_SPI
		DisplayOff(Solomon2);
#endif
	}
}


void main_suspend_action(void)
{
    ui_powerdown();
}

void main_resume_action(void)
{
    ui_wakeup();
}

void main_sof_action(void)
{
    if (!main_b_cdc_enable)
        return;
    ui_process(udd_get_frame_number());
}

bool main_cdc_enable(uint8_t port)
{
    main_b_cdc_enable = true;
    // Open communication
    uart_open(port);
    return true;
}

void main_cdc_disable(uint8_t port)
{
    main_b_cdc_enable = false;
    // Close communication
    uart_close(port);
}

void main_cdc_set_dtr(uint8_t port, bool b_enable)
{
    if (b_enable) {
        // Host terminal has open COM
        ui_com_open(port);
    } else {
        // Host terminal has close COM
        ui_com_close(port);
    }
}

/**
 * \mainpage ASF USB Device CDC
 *
 * \section intro Introduction
 * This example shows how to implement a USB Device CDC
 * on Atmel MCU with USB module.
 * The application note AVR4907 provides more information
 * about this implementation.
 *
 * \section desc Description of the Communication Device Class (CDC)
 * The Communication Device Class (CDC) is a general-purpose way to enable all
 * types of communications on the Universal Serial Bus (USB).
 * This class makes it possible to connect communication devices such as
 * digital telephones or analog modems, as well as networking devices
 * like ADSL or Cable modems.
 * While a CDC device enables the implementation of quite complex devices,
 * it can also be used as a very simple method for communication on the USB.
 * For example, a CDC device can appear as a virtual COM port, which greatly
 * simplifies application development on the host side.
 *
 * \section startup Startup
 * The example is a bridge between a USART from the main MCU
 * and the USB CDC interface.
 *
 * In this example, we will use a PC as a USB host:
 * it connects to the USB and to the USART board connector.
 * - Connect the USART peripheral to the USART interface of the board.
 * - Connect the application to a USB host (e.g. a PC)
 *   with a mini-B (embedded side) to A (PC host side) cable.
 * The application will behave as a virtual COM (see Windows Device Manager).
 * - Open a HyperTerminal on both COM ports (RS232 and Virtual COM)
 * - Select the same configuration for both COM ports up to 115200 baud.
 * - Type a character in one HyperTerminal and it will echo in the other.
 *
 * \note
 * On the first connection of the board on the PC,
 * the operating system will detect a new peripheral:
 * - This will open a new hardware installation window.
 * - Choose "No, not this time" to connect to Windows Update for this installation
 * - click "Next"
 * - When requested by Windows for a driver INF file, select the
 *   atmel_devices_cdc.inf file in the directory indicated in the Atmel Studio
 *   "Solution Explorer" window.
 * - click "Next"
 *
 * \copydoc UI
 *
 * \section example About example
 *
 * The example uses the following module groups:
 * - Basic modules:
 *   Startup, board, clock, interrupt, power management
 * - USB Device stack and CDC modules:
 *   <br>services/usb/
 *   <br>services/usb/udc/
 *   <br>services/usb/class/cdc/
 * - Specific implementation:
 *    - main.c,
 *      <br>initializes clock
 *      <br>initializes interrupt
 *      <br>manages UI
 *      <br>
 *    - uart_xmega.c,
 *      <br>implementation of RS232 bridge for XMEGA parts
 *    - uart_uc3.c,
 *      <br>implementation of RS232 bridge for UC3 parts
 *    - uart_sam.c,
 *      <br>implementation of RS232 bridge for SAM parts
 *    - specific implementation for each target "./examples/product_board/":
 *       - conf_foo.h   configuration of each module
 *       - ui.c        implement of user's interface (leds,buttons...)
 */

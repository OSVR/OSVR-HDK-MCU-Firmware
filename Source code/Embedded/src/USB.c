/*
 * USB.c
 *
 * Created: 10/23/2015 5:33:42 PM
 *  Author: Sensics
 */

// usb events

#include <asf.h>
#include <pmic.h>
#include "conf_usb.h"
#include "conf_timeout.h"
#include "ui.h"
#include "uart.h"
#include "SerialStateMachine.h"
#include "DeviceDrivers/Display.h"
#include "my_hardware.h"
#include "bno_callbacks.h"

#include "DeviceDrivers/BNO070.h"
#include "Console.h"

#include "TimingDebug.h"

#if defined(OSVRHDK) && defined(HDK_ENABLE_HID_SXS)
#include "SideBySide.h"
#endif

#include "USB.h"

static volatile bool main_b_cdc_enable = false;
static volatile bool main_b_cdc_opened = false;
#undef USB_USE_UART

#ifdef USB_USE_UART
static const uint8_t s_port = 0;
#endif

void main_suspend_action(void) { ui_powerdown(); }
void main_resume_action(void) { ui_wakeup(); }
void main_sof_action(void)
{
	if (!main_b_cdc_enable)
		return;
	ui_process(udd_get_frame_number());
}

bool main_cdc_enable()
{
	main_b_cdc_enable = true;
// Open communication
#ifdef USB_USE_UART
	uart_open(s_port);
#endif
	return true;
}

void main_cdc_disable()
{
	main_b_cdc_enable = false;
	main_b_cdc_opened = false;
// Close communication
#ifdef USB_USE_UART
	uart_close(s_port);
#endif
}

bool usb_cdc_is_active(void) { return main_b_cdc_enable && main_b_cdc_opened; }
bool usb_cdc_should_tx(iram_size_t size)
{
	return main_b_cdc_enable && main_b_cdc_opened &&
	       (udi_cdc_get_free_tx_buffer() >= min(size, UDI_CDC_DATA_EPS_FS_SIZE));
}
void main_cdc_rx_notify()
{
	while (udi_cdc_is_rx_ready())
	{
		char ch = udi_cdc_getc();
		// echo on
		udi_cdc_putc(ch);
		ProcessIncomingChar(ch);
	}
}

void main_cdc_set_dtr(bool b_enable)
{
	if (b_enable)
	{
		main_b_cdc_opened = true;
#ifdef USB_USE_UART
		// Host terminal has open COM
		ui_com_open(s_port);
#endif
	}
	else
	{
		main_b_cdc_opened = false;
#ifdef USB_USE_UART
		// Host terminal has close COM
		ui_com_close(s_port);
#endif
	}
}

static bool my_flag_autorize_generic_events = false;
bool my_callback_generic_enable(void)
{
	my_flag_autorize_generic_events = true;
	return true;
}
void my_callback_generic_disable(void) { my_flag_autorize_generic_events = false; }
// void my_button_press_event(void)
//{
// if (!my_flag_autorize_generic_events) {
// return;
//}
// uint8_t report[] = {0x00,0x01,0x02};
// udi_hid_generic_send_report_in(report);
//}

void my_callback_generic_report_out(uint8_t *report)
{
	if ((report[0] == 0) && (report[1] == 1))
	{
		Display_Off(Display1);
		// The report is correct
	}
}
void my_callback_generic_set_feature(uint8_t *report_feature)

// 0x7125 is signature in first two bytes
// next byte is version number for report, defaults to 1
// next byte is "1" to set side-by-side mode, 0 to go to normal mode
{
	if ((report_feature[0] == 0x71) && (report_feature[1] == 0x25))
	{
		// The report feature is correct
		if (report_feature[2] == 1)
		{
#if defined(OSVRHDK) && defined(HDK_ENABLE_HID_SXS)
			if (report_feature[3] == 0)
			{
				SxS_Disable();  // normal mode
			}
			else
			{
				SxS_Enable();  // side by side
			}
#endif
		}
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

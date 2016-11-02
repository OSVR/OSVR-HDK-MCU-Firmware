/*
 * Boot.c
 *
 * Created: 10/2/2013 5:16:47 PM
 *  Author: Sensics
 */

#include "GlobalOptions.h"
#include "my_hardware.h"
#include <ioport.h>
#include <udc.h>

#include "Boot.h"
#include <stdint.h>
#include <avr/io.h>

#define ISP_PORT_DIR PORTE_DIR
#define ISP_PORT_PINCTRL PORTE_PIN5CTRL
#define ISP_PORT_IN PORTE_IN
#define ISP_PORT_PIN 5

void CheckForBootloaderSwitchOnStartup()
{
	// read the pin:
	// brute force the whole thing as input
	ISP_PORT_DIR = 0x0;
	// enable pull-up
	ISP_PORT_PINCTRL = 0x18;
	// delay equiv. to in the assembly
	for (uint8_t n = 0xff; n; --n)
	{
		barrier();
	}

	// If pin is low, start the bootloader.
	if (!(ISP_PORT_IN & _BV(ISP_PORT_PIN)))
	{
#ifdef Debug_LED
		// Turn the LED on to indicate that we didn't get into the bootloader the normal way.
		ioport_set_pin_dir(Debug_LED, IOPORT_DIR_OUTPUT);
		Debug_LED_Turn_On();
#endif
		PrepareForSoftwareUpgrade();
	}
	ISP_PORT_PINCTRL = 0x0;
}

void PrepareForSoftwareUpgrade(void)
{
	// code is taken from here: http://www.avrfreaks.net/index.php?name=PNphpBB2&file=printview&t=125724&start=0

	udc_stop(); /*Required to stop USB interrupts messing you up before the vectors have been moved */

#ifdef BNO070
	/* Disable interrupts from BNO070 */
	PORTD.INTCTRL &= ~PORT_INT0LVL0_bm;  // Disable PORT D Interrupt
#endif

	/* Jump to 0x401FC = BOOT_SECTION_START + 0x1FC which is
	 * the stated location of the bootloader entry (AVR1916).
	 * Note the address used is the word address = byte addr/2.
	 * My ASM fu isn't that strong, there are probably nicer
	 * ways to do this with, yennow, defined symbols .. */

	asm("ldi r30, 0xFE\n" /* Low byte to ZL */
	    "ldi r31, 0x00\n" /* mid byte to ZH */
	    "ldi r24, 0x02\n" /* high byte to EIND which lives */
	    "out 0x3c, r24\n" /* at addr 0x3c in I/O space */
	    "eijmp"
	    : ::"r24", "r30", "r31");
}

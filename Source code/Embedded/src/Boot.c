/*
 * Boot.c
 *
 * Created: 10/2/2013 5:16:47 PM
 *  Author: Sensics
 */

#include <udc.h>

#include "Boot.h"

void PrepareForSoftwareUpgrade(void)

{
    // code is taken from here: http://www.avrfreaks.net/index.php?name=PNphpBB2&file=printview&t=125724&start=0

    udc_stop(); /*Required to stop USB interrupts messing you up before the vectors have been moved */

    /* Jump to 0x401FC = BOOT_SECTION_START + 0x1FC which is
     * the stated location of the bootloader entry (AVR1916).
     * Note the address used is the word address = byte addr/2.
     * My ASM fu isn't that strong, there are probably nicer
     * ways to do this with, yennow, defined symbols .. */

    asm ("ldi r30, 0xFE\n"  /* Low byte to ZL */
         "ldi r31, 0x00\n" /* mid byte to ZH */
         "ldi r24, 0x02\n" /* high byte to EIND which lives */
         "out 0x3c, r24\n" /* at addr 0x3c in I/O space */
         "eijmp":  :: "r24", "r30", "r31");
}
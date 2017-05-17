/*
 * my_hardware.h
 *
 * Created: 9/1/2013 9:55:05 PM
 *  Author: Sensics
 */

#ifndef MY_HARDWARE_H_
#define MY_HARDWARE_H_

// Options header
#include "GlobalOptions.h"

// avr-libc header
#include <avr/io.h>

// asf header
#include <board.h>

#define Debug_LED IOPORT_CREATE_PIN(PORTF, 0)

#ifdef SVR_HAVE_SIDEBYSIDE
#ifdef DSIGHT
#define Side_by_side_A IOPORT_CREATE_PIN(PORTE, 2)  // low: regular; high: side by side
#endif
#define Side_by_side_B IOPORT_CREATE_PIN(PORTF, 1)  // low: regular; high: side by side
#endif

#ifdef SVR_HAVE_PWM_OUTPUTS
/// @todo Uncomment the following line to enable the PWM for display brightness.
//#define SVR_ENABLE_DISPLAY_PWM
#endif
//#define Backlight				IOPORT_CREATE_PIN(PORTE,4) //

#ifdef BNO070
#define BNO070_ADDR 0x48
// BNO070 will use INT0 vector of PORTD
#define BNO070_ISR() ISR(PORTD_INT0_vect)
#define BNO_070_Reset_Pin IOPORT_CREATE_PIN(PORTA, 5)
#define Int_BNO070 IOPORT_CREATE_PIN(PORTD, 3)
#define BNO_BOOTN IOPORT_CREATE_PIN(PORTE, 2)  // low: Entry boot mode; high: normal.

#endif  // BNO070

#ifdef SVR_IS_HDK_1_x
#define LCD_avdd_en IOPORT_CREATE_PIN(PORTC, 2)
#define LCD_avdd_en_sw IOPORT_CREATE_PIN(PORTC, 3)
#endif

#ifdef SVR_HAVE_FPGA_VIDEO_LOCK_PIN
#define FPGA_unlocked IOPORT_CREATE_PIN(PORTF, 2)  // low:locked; high:not locked

#endif  // SVR_HAVE_FPGA_VIDEO_LOCK_PIN

#ifdef SVR_HAVE_PWM_OUTPUTS
#define PWM_A IOPORT_CREATE_PIN(PORTE, 3)  // high:on; low:off GPIO_E_X2_CH-2
#define PWM_B IOPORT_CREATE_PIN(PORTF, 2)  // high:on; low:off USARTF0_RXD0

#endif  // SVR_HAVE_PWM_OUTPUTS

#define USB_Hub_Reset_Pin IOPORT_CREATE_PIN(PORTF, 6)
#define USB_Hub_Power_Pin IOPORT_CREATE_PIN(PORTF, 7)

#ifdef SVR_HAVE_NXP1
#define NXP_1_ADDR 0x48  // current comp - was 0x90 and 0x92
#define CEC_1_ADDR 0x34
extern uint8_t
    actualNXP_1_ADDR;  //< @todo extern added on a hunch that we want this to be global, not replicated everywhere.
extern uint8_t
    actualCEC_1_ADDR;  //< @todo extern added on a hunch that we want this to be global, not replicated everywhere.

#endif  // SVR_HAVE_NXP1

#ifdef SVR_HAVE_NXP2
#define NXP_2_ADDR 0x49  // this is fake. In reality, NXP2 and NXP1 are same
#define CEC_2_ADDR 0x36  // this is fake. In reality, CEC2 and CEC1 are same  $$$
extern uint8_t
    actualNXP_2_ADDR;  //< @todo extern added on a hunch that we want this to be global, not replicated everywhere.
extern uint8_t
    actualCEC_2_ADDR;  //< @todo extern added on a hunch that we want this to be global, not replicated everywhere.

#endif  // SVR_HAVE_NXP2

#ifdef SVR_HAVE_TOSHIBA_TC358870
#define TC358870_Reset_Pin IOPORT_CREATE_PIN(PORTD, 5)  // out, active low, >12ms.
#define TC358870_TWI_PORT (&TWIE)                       // Dennis Yeh 2016/03/14 : for TC358870

/// This pin is used as address select during reset, interrupt the rest of the time.
/// out, Active high, It's also I2C address selection: Slave address = 0x0F if INT = Low
/// during reset, Slave address = 0x1F if INT = High during reset
#define TC358870_ADDR_SEL_INT IOPORT_CREATE_PIN(PORTD, 2)

// Warning - because IOPORT_CREATE_PIN doesn't do expansion before token pasting, we can't use this as an input, so it
// has to be kept in sync manually... and you need the trailing underscore since PORTD is a define itself
#define TC358870_ADDR_SEL_INT_PORT PORTD_INT

// We'll use interrupt 1 on this port - interrupt 0 is used by BNO already.
#define TC358870_PORT_INT 1

// Display interrupts will be medium level
#define TC358870_INT_LEVEL MED

// To get the ISR definition, use DeviceDrivers/Toshiba_TC358870_ISR.h

/// in, 1.8v power good, asserts low if output voltage is low due to thermal shutdown,
/// overcurrent, over/under-voltage or EN shut down.
#define TC358870_PWR_GOOD IOPORT_CREATE_PIN(PORTD, 0)
#endif  // SVR_HAVE_TOSHIBA_TC358870

#ifdef SVR_IS_HDK_20
#define PANEL_RESET IOPORT_CREATE_PIN(PORTF, 4)  // out, panel reset. hw power reset.
#endif

#ifdef SVR_IS_HDK_20
#define MCU_LEVEL_SHIFT_OE IOPORT_CREATE_PIN(PORTA, 1)  // out, level shift enable, low enable. U55
#define HDMI_HPD IOPORT_CREATE_PIN(PORTF, 3)            //< @todo out, HDMI_HPD on schematic?
#define MCU_LED_R IOPORT_CREATE_PIN(PORTA, 3)           //< @todo LD17 - aka MCU_EDID_R - just an LED or something more?

#define Left_SWIRE IOPORT_CREATE_PIN(PORTF, 1)   // out, Left Panel SWIRE //< @todo NC?
#define Right_SWIRE IOPORT_CREATE_PIN(PORTF, 3)  // out, Right Panel SWIRE //< @todo PF3 is HDMI_HPD on schematic?

#define ANA_PWR_IN IOPORT_CREATE_PIN(PORTA, 6)   // in, 5v power good. "ANA_TMP1" - there is also PA4 HDMI_D5V
#define EDID_EEP_WP IOPORT_CREATE_PIN(PORTA, 7)  // out, edid eeprom write protection, 0: write protect

// port B
#define AUD_JACK_DETECT IOPORT_CREATE_PIN(PORTB, 0)  // in, Audio jack detection, 0: plug, 1: unplug
#define CPU_RST IOPORT_CREATE_PIN(PORTB, 2)          //
// out, dac, audio codec operation mode, LOW normal operating mode, MID low sampling
// frequency mode, HIGH Power-down or Sleep mode
#define AUD_PCS IOPORT_CREATE_PIN(PORTB, 3)

// port C
#define AUD_DEEM IOPORT_CREATE_PIN(PORTC, 2)
#define AUD_MUTE IOPORT_CREATE_PIN(PORTC, 3)

// port D
#define USB_SW_OC IOPORT_CREATE_PIN(PORTD, 1)  // out, HW NC, Don't care. //< @todo schematic says in, present

#endif  // SVR_IS_HDK_20

void custom_board_init(void);                             // Sensics-specific initialization of the hardware
void set_pwm_values(uint8_t Display1, uint8_t Display2);  // sets pwm values for each display

#ifdef OSVRHDK
// indicates version of HDK, e.g. 1.1, 1.2, 1.3
extern uint8_t HDK_Version_Major;
extern uint8_t HDK_Version_Minor;
extern unsigned char ProductName[];
// Must be manually kept in sync with the string in my_hardware.c (and conf_usb.h), but a static assert ensures it is
// so.
#if SVR_HDK_DEFAULT_MAJOR_VER == 1 && SVR_HDK_DEFAULT_MINOR_VER == 2
// these version have an extra space for a + since we can't determine what's a 1.3 vs 1.4
#define SVR_HDK_PRODUCT_NAME_STRING_LENGTH 14
#elif defined(SVR_IS_HDK_20)
// just HDK 2, no .0
#define SVR_HDK_PRODUCT_NAME_STRING_LENGTH 11
#else
#define SVR_HDK_PRODUCT_NAME_STRING_LENGTH 13
#endif

#endif  // OSVRHDK

#define SVR_EEP_SIGNATURE_PAGE 0  //< EEPROM page where Sensics signature is stored

#define SVR_EEP_CONFIGURATION_PAGE 1  //< EEPROM page where configuration is stored

/// Each single-byte config value uses four bytes of eeprom for validation.
#define SVR_CONFIG_BLOCK_SIZE 4

/// Structure to wrap eeprom offsets
typedef struct SvrEepromOffset_
{
	uint8_t offset;
} SvrEepromOffset_t;

#define PersistenceOffset ((SvrEepromOffset_t){4})         //< Persistence refresh rate
#define PersistencePercentOffset ((SvrEepromOffset_t){8})  //< Persistence percent
#define SideBySideOffset ((SvrEepromOffset_t){12})         //< Side-by-side settings
#define GRVOffset ((SvrEepromOffset_t){16})                //< BNO game rotation vector mode

/**
 * Set all values of a memory buffer of size EEPROM_PAGE_SIZE to a given value
 */
void set_buffer(uint8_t buffer[EEPROM_PAGE_SIZE], uint8_t value);
/**
 * Check if an EEPROM page is equal to a memory buffer, for a buffer of size EEPROM_PAGE_SIZE
 */
bool is_eeprom_page_equal_to_buffer(uint8_t page_addr, uint8_t buffer[EEPROM_PAGE_SIZE]);
void SetConfigValue(SvrEepromOffset_t oset, uint8_t value);
uint8_t GetConfigValue(SvrEepromOffset_t oset);
bool IsConfigOffsetValid(SvrEepromOffset_t oset);

// return value is true if it was valid and if we retrieved into outValue, false if it was not valid and we did nothing.
bool GetValidConfigValue(SvrEepromOffset_t oset, uint8_t *outValue);

// returns the stored config value if it's valid, otherwise it returns the given default value (which is not stored into
// eeprom, just returned to you.)
uint8_t GetValidConfigValueOrDefault(SvrEepromOffset_t oset, uint8_t defaultValue);

// return value is true if it was valid and if we retrieved into outValue, false if it was not valid and we just stored
// the default value.
bool GetValidConfigValueOrWriteDefault(SvrEepromOffset_t oset, uint8_t defaultValue, uint8_t *outValue);

void Debug_LED_Turn_On(void);
void Debug_LED_Turn_Off(void);

#endif /* MY_HARDWARE_H_ */

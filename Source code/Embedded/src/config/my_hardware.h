/*
 * my_hardware.h
 *
 * Created: 9/1/2013 9:55:05 PM
 *  Author: Sensics
 */

#ifndef MY_HARDWARE_H_
#define MY_HARDWARE_H_

#include "board.h"
#include "GlobalOptions.h"

//! \note Perform unit tests using SPI instance SPIC
#define Solomon1_SPI SPIC

#ifndef OSVRHDK
#define Solomon2_SPI SPID
#endif

#define Debug_LED IOPORT_CREATE_PIN(PORTF, 0)

#define FPGA_Reset_Pin IOPORT_CREATE_PIN(PORTB, 0)  // low holds the FPGA in reset, high releases it

#define NXP1_Reset_Pin IOPORT_CREATE_PIN(PORTD, 5)
#ifndef OSVRHDK
#define NXP2_Reset_Pin IOPORT_CREATE_PIN(PORTD, 4)
#endif

#define Int_HDMI_A IOPORT_CREATE_PIN(PORTD, 2)
#ifndef OSVRHDK
#define Int_HDMI_B IOPORT_CREATE_PIN(PORTD, 3)
#endif

#define Side_by_side_A IOPORT_CREATE_PIN(PORTE, 2)  // low: regular; high: side by side
#define Side_by_side_B IOPORT_CREATE_PIN(PORTF, 1)  // low: regular; high: side by side

//#define Backlight				IOPORT_CREATE_PIN(PORTE,4) //

#ifdef OSVRHDK
#define LCD_avdd_en IOPORT_CREATE_PIN(PORTC, 2)
#define LCD_avdd_en_sw IOPORT_CREATE_PIN(PORTC, 3)
#define BNO_070_Reset_Pin IOPORT_CREATE_PIN(PORTA, 5)
#define Int_BNO070 IOPORT_CREATE_PIN(PORTD, 3)
#define FPGA_unlocked IOPORT_CREATE_PIN(PORTF, 2)  // low:locked; high:not locked
#else
#define PWM_A IOPORT_CREATE_PIN(PORTE, 3)  // high:on; low:off
#define PWM_B IOPORT_CREATE_PIN(PORTF, 2)  // high:on; low:off
#endif

#define USB_Hub_Reset_Pin IOPORT_CREATE_PIN(PORTF, 6)
#define USB_Hub_Power_Pin IOPORT_CREATE_PIN(PORTF, 7)

#define NXP_1_ADDR 0x48  // current comp - was 0x90 and 0x92
#ifndef OSVRHDK
#define NXP_2_ADDR 0x49  // this is fake. In reality, NXP2 and NXP1 are same
#endif

#define CEC_1_ADDR 0x34
#ifndef OSVRHDK
#define CEC_2_ADDR 0x36  // this is fake. In reality, CEC2 and CEC1 are same  $$$
#endif

uint8_t actualNXP_1_ADDR;
uint8_t actualCEC_1_ADDR;

#ifndef OSVRHDK
uint8_t actualNXP_2_ADDR;
uint8_t actualCEC_2_ADDR;
#endif

#ifdef BNO070
#define BNO070_ADDR 0x48
#endif

void custom_board_init(void);                             // Sensics-specific initialization of the hardware
void set_pwm_values(uint8_t Display1, uint8_t Display2);  // sets pwm values for each display

// indicates version of HDK, e.g. 1.1, 1.2, 1.3
extern uint8_t HDK_Version_Major;
extern uint8_t HDK_Version_Minor;
extern char ProductName[];

// BNO070 will use INT0 vector of PORTD
#define BNO070_ISR() ISR(PORTD_INT0_vect)

#define SIGNATURE_PAGE 0  // EEPROM page where Sensics signature is stored

#define CONFIGURATION_PAGE 1  // EEPROM page where configuration is stored

#define SBSOffset 0                 // Side-by-side settings
#define PersistenceOffset 4         // Persistence refresh rate
#define PersistencePercentOffset 8  // Persistence percent
#define SideBySideOffset 12         // Persistence percent
#define GRVOffset 16                // BNO game rotation vector mode

void set_buffer(uint8_t *buffer, uint8_t value);
bool is_eeprom_page_equal_to_buffer(uint8_t page_addr, uint8_t *buffer);
void SetConfigValue(uint8_t offset, uint8_t value);
uint8_t GetConfigValue(uint8_t offset);
bool IsConfigOffsetValid(uint8_t offset);

#endif /* MY_HARDWARE_H_ */

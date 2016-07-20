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

#ifdef SVR_HAVE_SOLOMON1
//! \note Perform unit tests using SPI instance SPIC
#define Solomon1_SPI SPIC
#endif  // SVR_HAVE_SOLOMON1

#ifdef SVR_HAVE_SOLOMON2
#define Solomon2_SPI SPID
#endif  // SVR_HAVE_SOLOMON2

#define Debug_LED IOPORT_CREATE_PIN(PORTF, 0)

#ifdef SVR_HAVE_NXP1
#define NXP1_Reset_Pin IOPORT_CREATE_PIN(PORTD, 5)
#define Int_HDMI_A IOPORT_CREATE_PIN(PORTD, 2)
#endif  // SVR_HAVE_NXP1

#ifdef SVR_HAVE_NXP2
#define NXP2_Reset_Pin IOPORT_CREATE_PIN(PORTD, 4)
#define Int_HDMI_B IOPORT_CREATE_PIN(PORTD, 3)
#endif  // SVR_HAVE_NXP1

#ifdef SVR_HAVE_SIDEBYSIDE
#define Side_by_side_A IOPORT_CREATE_PIN(PORTE, 2)  // low: regular; high: side by side
#define Side_by_side_B IOPORT_CREATE_PIN(PORTF, 1)  // low: regular; high: side by side
#endif

//#define Backlight				IOPORT_CREATE_PIN(PORTE,4) //

#ifdef BNO070
#define BNO_070_Reset_Pin IOPORT_CREATE_PIN(PORTA, 5)
#define Int_BNO070 IOPORT_CREATE_PIN(PORTD, 3)
#endif  // BNO070

#ifdef SVR_IS_HDK_1_x
#define LCD_avdd_en IOPORT_CREATE_PIN(PORTC, 2)
#define LCD_avdd_en_sw IOPORT_CREATE_PIN(PORTC, 3)
#endif

#ifdef SVR_HAVE_FPGA_VIDEO_LOCK_PIN
#define FPGA_unlocked IOPORT_CREATE_PIN(PORTF, 2)  // low:locked; high:not locked
#endif                                             // SVR_HAVE_FPGA_VIDEO_LOCK_PIN

#ifdef SVR_HAVE_PWM_OUTPUTS
#define PWM_A IOPORT_CREATE_PIN(PORTE, 3)  // high:on; low:off
#define PWM_B IOPORT_CREATE_PIN(PORTF, 2)  // high:on; low:off
#endif                                     // SVR_HAVE_PWM_OUTPUTS

#ifdef SVR_HAVE_FPGA
#define FPGA_Reset_Pin IOPORT_CREATE_PIN(PORTB, 0)  // low holds the FPGA in reset, high releases it
#endif                                              // SVR_HAVE_FPGA

#define USB_Hub_Reset_Pin IOPORT_CREATE_PIN(PORTF, 6)
#define USB_Hub_Power_Pin IOPORT_CREATE_PIN(PORTF, 7)

#ifdef SVR_HAVE_NXP1
#define NXP_1_ADDR 0x48  // current comp - was 0x90 and 0x92
#define CEC_1_ADDR 0x34
uint8_t actualNXP_1_ADDR;  /// @todo should this be extern?
uint8_t actualCEC_1_ADDR;  /// @todo should this be extern?
#endif                     // SVR_HAVE_NXP1

#ifdef SVR_HAVE_NXP2
#define NXP_2_ADDR 0x49    // this is fake. In reality, NXP2 and NXP1 are same
#define CEC_2_ADDR 0x36    // this is fake. In reality, CEC2 and CEC1 are same  $$$
uint8_t actualNXP_2_ADDR;  /// @todo should this be extern?
uint8_t actualCEC_2_ADDR;  /// @todo should this be extern?
#endif                     // SVR_HAVE_NXP2

#ifdef BNO070
#define BNO070_ADDR 0x48
#endif  // BNO070

void custom_board_init(void);                             // Sensics-specific initialization of the hardware
void set_pwm_values(uint8_t Display1, uint8_t Display2);  // sets pwm values for each display

#ifdef OSVRHDK
// indicates version of HDK, e.g. 1.1, 1.2, 1.3
extern uint8_t HDK_Version_Major;
extern uint8_t HDK_Version_Minor;
extern char ProductName[];
#endif

#ifdef BNO070
// BNO070 will use INT0 vector of PORTD
#define BNO070_ISR() ISR(PORTD_INT0_vect)
#endif  // BNO070

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

// return value is true if it was valid and if we retrieved into outValue, false if it was not valid and we did nothing.
bool GetValidConfigValue(uint8_t offset, uint8_t *outValue);

// returns the stored config value if it's valid, otherwise it returns the given default value (which is not stored into
// eeprom, just returned to you.)
uint8_t GetValidConfigValueOrDefault(uint8_t offset, uint8_t defaultValue);

// return value is true if it was valid and if we retrieved into outValue, false if it was not valid and we just stored
// the default value.
bool GetValidConfigValueOrWriteDefault(uint8_t offset, uint8_t defaultValue, uint8_t *outValue);

#endif /* MY_HARDWARE_H_ */

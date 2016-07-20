/*
 * my_hardware.c
 *
 * Created: 10/2/2013 1:15:05 PM
 *  Author: Sensics
 */

#include "my_hardware.h"

#include "GlobalOptions.h"

#ifdef SVR_HAVE_SOLOMON
#include "DeviceDrivers/Solomon.h"
#endif

#include "Console.h"

#include <asf.h>
#include <ioport.h>
#include <delay.h>

#include <stdio.h>

uint8_t HDK_Version_Major = 1;
uint8_t HDK_Version_Minor = 2;  // set 1.2 as default version
char ProductName[] = "OSVR HDK 1.2";

#ifdef SVR_HAVE_NXP1
uint8_t actualNXP_1_ADDR = NXP_1_ADDR;
uint8_t actualCEC_1_ADDR = CEC_1_ADDR;
#endif

#ifdef SVR_HAVE_NXP2
uint8_t actualNXP_2_ADDR = NXP_2_ADDR;
uint8_t actualCEC_2_ADDR = CEC_2_ADDR;
#endif

struct pwm_config pwm_cfg[2];

void set_pwm_values(uint8_t Display1, uint8_t Display2)

// sets PWM of each display
{
	return;  // remove this line if you want to use the on-board PWM
	pwm_stop(&pwm_cfg[0]);
	pwm_start(&pwm_cfg[0], Display1);
	pwm_stop(&pwm_cfg[1]);
	pwm_start(&pwm_cfg[1], Display2);
}

void custom_board_init(void)

{
#ifdef SVR_HAVE_SOLOMON1
	ioport_configure_pin(AT86RFX_SPI_SCK, IOPORT_DIR_OUTPUT | IOPORT_INIT_HIGH);
	ioport_configure_pin(AT86RFX_SPI_MOSI, IOPORT_DIR_OUTPUT | IOPORT_INIT_HIGH);
	ioport_configure_pin(AT86RFX_SPI_MISO, IOPORT_DIR_INPUT);

	ioport_configure_pin(Solomon1_CSN, IOPORT_DIR_OUTPUT | IOPORT_INIT_HIGH);
	ioport_configure_pin(Solomon1_AddrData, IOPORT_DIR_OUTPUT | IOPORT_INIT_HIGH);
#endif  // SVR_HAVE_SOLOMON1

#if defined(SVR_HAVE_SOLOMON1) && defined(SVR_HAVE_SOLOMON2)
	ioport_configure_pin(SPI_Mux_OE, IOPORT_DIR_OUTPUT | IOPORT_INIT_LOW);
	ioport_configure_pin(SPI_Mux_Select, IOPORT_DIR_OUTPUT | IOPORT_INIT_LOW);
#endif  // SVR_HAVE_SOLOMON1 && SVR_HAVE_SOLOMON2

#ifdef SVR_HAVE_SOLOMON1
	ioport_configure_pin(Solomon1_VOUT_Shutdown, IOPORT_DIR_OUTPUT | IOPORT_INIT_LOW);
	ioport_configure_pin(Solomon1_Reset, IOPORT_DIR_OUTPUT | IOPORT_INIT_HIGH);
#endif

#ifdef SVR_HAVE_SOLOMON2
	ioport_configure_pin(Solomon2_VOUT_Shutdown, IOPORT_DIR_OUTPUT | IOPORT_INIT_LOW);
	ioport_configure_pin(Solomon2_Reset, IOPORT_DIR_OUTPUT | IOPORT_INIT_HIGH);
	ioport_configure_pin(Solomon2_AddrData, IOPORT_DIR_OUTPUT | IOPORT_INIT_HIGH);
#endif

	ioport_configure_pin(Debug_LED, IOPORT_DIR_OUTPUT | IOPORT_INIT_HIGH);

#ifdef SVR_HAVE_NXP1
	ioport_configure_pin(NXP1_Reset_Pin, IOPORT_DIR_OUTPUT | IOPORT_INIT_HIGH);
	ioport_configure_pin(Int_HDMI_A, IOPORT_DIR_INPUT);
#endif

#ifdef SVR_HAVE_SIDEBYSIDE
#ifdef OSVRHDK
	ioport_configure_pin(Side_by_side_A, IOPORT_DIR_OUTPUT | IOPORT_INIT_LOW);
	ioport_configure_pin(Side_by_side_B, IOPORT_DIR_OUTPUT | IOPORT_INIT_LOW);
	uint8_t sideBySideConfig;
	if (GetValidConfigValueOrWriteDefault(SideBySideOffset, 0, &sideBySideConfig))
	{
		if (sideBySideConfig == 0)
			ioport_set_pin_low(Side_by_side_B);  // normal mode
		else
			ioport_set_pin_high(Side_by_side_B);  // SBS mode
	}
#else
	ioport_configure_pin(Side_by_side_A, IOPORT_DIR_OUTPUT | IOPORT_INIT_HIGH);
	ioport_configure_pin(Side_by_side_B, IOPORT_DIR_OUTPUT | IOPORT_INIT_HIGH);
#endif

#endif  // SVR_HAVE_SIDEBYSIDE

#ifdef SVR_HAVE_NXP2
	ioport_configure_pin(NXP2_Reset_Pin, IOPORT_DIR_OUTPUT | IOPORT_INIT_HIGH);
	ioport_configure_pin(Int_HDMI_B, IOPORT_DIR_INPUT);
#endif

	ioport_configure_pin(USB_Hub_Reset_Pin, IOPORT_DIR_OUTPUT | IOPORT_INIT_LOW);
	ioport_configure_pin(USB_Hub_Power_Pin, IOPORT_DIR_OUTPUT | IOPORT_INIT_LOW);

#ifdef SVR_IS_HDK_1_x
	ioport_configure_pin(LCD_avdd_en, IOPORT_DIR_OUTPUT | IOPORT_INIT_HIGH);
	ioport_configure_pin(LCD_avdd_en_sw, IOPORT_DIR_OUTPUT | IOPORT_INIT_HIGH);
#endif

#ifdef SVR_HAVE_FPGA_VIDEO_LOCK_PIN
	ioport_configure_pin(FPGA_unlocked, IOPORT_DIR_INPUT | IOPORT_MODE_PULLUP);
#endif

#ifdef SVR_HAVE_PWM_OUTPUTS
	ioport_configure_pin(PWM_A, IOPORT_DIR_OUTPUT | IOPORT_INIT_HIGH);
	ioport_configure_pin(PWM_B, IOPORT_DIR_OUTPUT | IOPORT_INIT_HIGH);
#endif

// ioport_configure_pin(Backlight,IOPORT_DIR_OUTPUT |  IOPORT_INIT_HIGH);

#ifdef SVR_HAVE_FPGA
	// init both UARTs
	static usart_rs232_options_t usart_options = {.baudrate = FPGA_USART_BAUD_RATE,
	                                              .charlength = FPGA_USART_SERIAL_CHAR_LENGTH,
	                                              .paritytype = FPGA_USART_SERIAL_PARITY,
	                                              .stopbits = FPGA_USART_SERIAL_STOP_BIT};

	if (!usart_init_rs232(FPGA1_USART, &usart_options))
		WriteLn("FPGA1 init err");

#if SVR_HAVE_FPGA == 2
	if (!usart_init_rs232(FPGA2_USART, &usart_options))
		WriteLn("FPGA2 init err");
#endif  // SVR_HAVE_FPGA == 2

#ifdef OSVRHDK
	ioport_configure_pin(FPGA_Reset_Pin,
	                     IOPORT_DIR_OUTPUT | IOPORT_INIT_LOW);  // start FPGA in reset mode until there is video
#else
	ioport_configure_pin(FPGA_Reset_Pin, IOPORT_DIR_OUTPUT | IOPORT_INIT_HIGH);
#endif  // OSVRHDK

#endif  // SVR_HAVE_FPGA

// init PWM for display brightness and strobing

// pwm_init(&pwm_cfg[0], PWM_TCE0, PWM_CH_D, 11000); // PWM_A: PE3
// pwm_init(&pwm_cfg[1], PWM_TCF0, PWM_CH_C, 11000); //PWM_B: PF2

// pwm_init(&pwm_cfg[0], PWM_TCD1, PWM_CH_A, 60); // Debug_LED - D4
// pwm_init(&pwm_cfg[1], PWM_TCE1, PWM_CH_A, 10); //Backlight - E4

// Start both PWM channels
// set_pwm_values(30,30);
// ioport_set_pin_low(PWM_A);
// ioport_set_pin_low(PWM_B);

// init PF0 LED for debug purposes
#ifdef OSVRHDK
	ioport_configure_pin(Debug_LED, IOPORT_DIR_OUTPUT | IOPORT_INIT_HIGH);
#endif
};

/**
 * Set all values of a memory buffer to a given value
 */
void set_buffer(uint8_t *buffer, uint8_t value)
{
	uint8_t i;

	for (i = 0; i < EEPROM_PAGE_SIZE; i++)
	{
		buffer[i] = value;
	}
}

/**
 * Check if an EEPROM page is equal to a memory buffer
 */
bool is_eeprom_page_equal_to_buffer(uint8_t page_addr, uint8_t *buffer)
{
	uint8_t i;
	char Msg[10];

	for (i = 0; i < EEPROM_PAGE_SIZE; i++)
	{
		// WriteLn("+");
		if (nvm_eeprom_read_byte(page_addr * EEPROM_PAGE_SIZE + i) != buffer[i])
		{
			WriteLn("---");
			sprintf(Msg, "%d %d %d", nvm_eeprom_read_byte(page_addr * EEPROM_PAGE_SIZE + i), buffer[i], i);
			WriteLn(Msg);
			return false;
		}
	}

	return true;
}

void eeprom_write_byte(uint8_t page_addr, uint8_t offset, uint8_t value)
{
	nvm_eeprom_write_byte(EEPROM_PAGE_SIZE + offset, value);
}

uint8_t eeprom_read_byte(uint8_t page_addr, uint8_t offset)
{
	return nvm_eeprom_read_byte(page_addr * EEPROM_PAGE_SIZE + offset);
}

bool IsConfigOffsetValid(uint8_t offset)
// determines if value at particular offset in config page is valid
{
	// for each value, next byte needs to be value+37, next byte reverse bitwise of value and next byte 65

	bool Valid = true;
	if (((eeprom_read_byte(CONFIGURATION_PAGE, offset) + 37) & 0xff) !=
	    eeprom_read_byte(CONFIGURATION_PAGE, offset + 1))
		Valid = false;
	if (eeprom_read_byte(CONFIGURATION_PAGE, offset) != (eeprom_read_byte(CONFIGURATION_PAGE, offset + 2) ^ 0xff))
		Valid = false;
	if (eeprom_read_byte(CONFIGURATION_PAGE, offset + 3) != 65)
		Valid = false;
	return Valid;
}

uint8_t GetConfigValue(uint8_t offset) { return eeprom_read_byte(CONFIGURATION_PAGE, offset); }
void SetConfigValue(uint8_t offset, uint8_t value)

{
	eeprom_write_byte(CONFIGURATION_PAGE, offset, value);
	eeprom_write_byte(CONFIGURATION_PAGE, offset + 1, (value + 37) & 0xff);
	eeprom_write_byte(CONFIGURATION_PAGE, offset + 2, value ^ 0xff);
	eeprom_write_byte(CONFIGURATION_PAGE, offset + 3, 65);
}

bool GetValidConfigValue(uint8_t offset, uint8_t *outValue)
{
	if (IsConfigOffsetValid(offset))
	{
		// Value was valid, return it in outValue
		*outValue = GetConfigValue(offset);
		// and tell caller that they got a live one.
		return true;
	}
	// Otherwise, return false without touching outValue.
	return false;
}

uint8_t GetValidConfigValueOrDefault(uint8_t offset, uint8_t defaultValue)
{
	if (IsConfigOffsetValid(offset))
	{
		// Value was valid, return it
		return GetConfigValue(offset);
	}
	// Not a valid config value found, so return the default instead.
	return defaultValue;
}

bool GetValidConfigValueOrWriteDefault(uint8_t offset, uint8_t defaultValue, uint8_t *outValue)
{
	if (GetValidConfigValue(offset, outValue))
	{
		// Value was valid, return true right away
		return true;
	}
	// Otherwise, write the default value to eeprom and return false accordingly.
	SetConfigValue(offset, defaultValue);
	return false;
}

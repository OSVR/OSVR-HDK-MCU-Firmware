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
#include "MacroUtils.h"

#include <asf.h>
#include <ioport.h>
#include <delay.h>

#include <stdio.h>

#ifdef OSVRHDK

uint8_t HDK_Version_Major = SVR_HDK_DEFAULT_MAJOR_VER;
uint8_t HDK_Version_Minor = SVR_HDK_DEFAULT_MINOR_VER;  // set default version from VariantOptions.h

char ProductName[] = USB_DEVICE_PRODUCT_NAME;
_Static_assert(sizeof(ProductName) == SVR_HDK_PRODUCT_NAME_STRING_LENGTH,
               "SVR_HDK_PRODUCT_NAME_STRING_LENGTH needs to be updated to match the current length of ProductName");

#endif

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
/// @todo This is pin setup code for part of the block of pins that are referred to nowhere else in the source code for
/// the HDK_20. Unclear how much of this is actually required for proper operation and how much is just extraneous: the
/// system did work with just the level shifter output enable setup.
#ifdef SVR_IS_HDK_20
	/// @todo Is this an MCU feature that just the HDK20 needs, or is this effectively dead code?
	ioport_configure_pin(
	    MCU_LEVEL_SHIFT_OE,
	    IOPORT_DIR_OUTPUT | IOPORT_INIT_LOW);  // I/O level shift gate enable. (i2c, hdmi_rst, 2848_reset).
/// @todo the device was functional without these configured.
#if 0
		ioport_configure_pin(ANA_PWR_IN, IOPORT_DIR_INPUT);                       // 5v power good indicator.
		ioport_configure_pin(EDID_EEP_WP, IOPORT_DIR_OUTPUT | IOPORT_INIT_HIGH);  // EDID EEP write protect (Low protect)

		// audio block IO   (All of this block are reserved !! HW NC)
		ioport_configure_pin(AUD_JACK_DETECT, IOPORT_DIR_INPUT);               // audio phone jack detection.
		ioport_configure_pin(AUD_DEEM, IOPORT_DIR_OUTPUT | IOPORT_INIT_HIGH);  // AUD_DEEM,  de-emphasis , 0:off, 1:on
		ioport_configure_pin(AUD_MUTE,
		IOPORT_DIR_OUTPUT | IOPORT_INIT_LOW);  // audio soft mute, low: mute off, high: mute on.
		// AUD_PCS, it's DAC output for audio codec operation mode. (Reserve)

		ioport_configure_pin(PWR_GOOD_2V5, IOPORT_DIR_INPUT);  // TPS54478 (U16) 1.8v power good indicator.
#endif  // 0
#endif

// Solomon SSD2848 IO Init.
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

#ifdef SVR_HAVE_TOSHIBA_TC358870
	ioport_configure_pin(TC358870_Reset_Pin, IOPORT_DIR_OUTPUT | IOPORT_INIT_HIGH);  // HW power on reset, > 12ms.
	/// @todo why is this being initialized as input? The pin is used as Int_HDMI_A on other variants, but on HDK_20
	/// it's I2C_Addr_Sel
	ioport_configure_pin(I2C_Addr_Sel, IOPORT_DIR_INPUT);
#endif

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

	/// @todo HW NC on SVR_IS_HDK_20?
	ioport_configure_pin(USB_Hub_Power_Pin, IOPORT_DIR_OUTPUT | IOPORT_INIT_LOW);

/// @todo initialize USB_SW_OC except on SVR_IS_HDK_20 where it may be HW NC?

#ifdef SVR_IS_HDK_20
	ioport_configure_pin(PANEL_RESET, IOPORT_DIR_OUTPUT | IOPORT_INIT_HIGH);  // HW power on reset, low > 10us
#endif

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

static void eeprom_write_byte(uint8_t page_addr, uint8_t offset, uint8_t value);
static uint8_t eeprom_read_byte(uint8_t page_addr, uint8_t offset);

void set_buffer(uint8_t buffer[EEPROM_PAGE_SIZE], uint8_t value)
{
	for (uint8_t i = 0; i < EEPROM_PAGE_SIZE; i++)
	{
		buffer[i] = value;
	}
}

/**
 * Check if an EEPROM page is equal to a memory buffer
 */
bool is_eeprom_page_equal_to_buffer(uint8_t page_addr, uint8_t buffer[EEPROM_PAGE_SIZE])
{
	for (uint8_t i = 0; i < EEPROM_PAGE_SIZE; i++)
	{
		// WriteLn("+");
		uint8_t ebyte = eeprom_read_byte(page_addr, i);
		if (ebyte != buffer[i])
		{
			WriteLn("---");
			char Msg[10];
			sprintf(Msg, "%d %d %d", ebyte, buffer[i], i);
			WriteLn(Msg);
			return false;
		}
	}

	return true;
}

static inline void eeprom_write_byte(uint8_t page_addr, uint8_t offset, uint8_t value)
{
	nvm_eeprom_write_byte(EEPROM_PAGE_SIZE + offset, value);
}

static inline uint8_t eeprom_read_byte(uint8_t page_addr, uint8_t offset)
{
	return nvm_eeprom_read_byte(page_addr * EEPROM_PAGE_SIZE + offset);
}

/// Read a chunk of eeprom corresponding to a single config value.
static void read_eeprom_config_block(SvrEepromOffset_t oset, uint8_t block[SVR_CONFIG_BLOCK_SIZE]);
static inline void read_eeprom_config_block(SvrEepromOffset_t oset, uint8_t block[SVR_CONFIG_BLOCK_SIZE])
{
	for (uint8_t i = 0; i < SVR_CONFIG_BLOCK_SIZE; ++i)
	{
		block[i] = eeprom_read_byte(SVR_EEP_CONFIGURATION_PAGE, oset.offset + i);
	}
}

/// Internal implementation version of IsConfigOffsetValid that also returns the main configuration value in an
/// outparam.
static bool IsConfigOffsetValid_Impl(SvrEepromOffset_t oset, uint8_t *outVal);

static inline bool IsConfigOffsetValid_Impl(SvrEepromOffset_t oset, uint8_t *outVal)
{
	// for each value, next byte needs to be value+37, next byte reverse bitwise of value and next byte 65

	uint8_t confBlock[SVR_CONFIG_BLOCK_SIZE];
	read_eeprom_config_block(oset, confBlock);
	if (outVal)
	{
		*outVal = confBlock[0];
	}
	_Static_assert(SVR_CONFIG_BLOCK_SIZE == 4,
	               "IsConfigOffsetValid_Impl validates four bytes for each config value. If you change that block "
	               "size, you better change IsConfigOffsetValid_Impl too.");
	if (((confBlock[0] + 37) & 0xff) != confBlock[1])
	{
		return false;
	}
	if (confBlock[0] != (confBlock[2] ^ 0xff))
	{
		return false;
	}
	if (confBlock[3] != 65)
	{
		return false;
	}
	return true;
}

bool IsConfigOffsetValid(SvrEepromOffset_t oset)
// determines if value at particular offset in config page is valid
{
	return IsConfigOffsetValid_Impl(oset, NULL);
}

uint8_t GetConfigValue(SvrEepromOffset_t oset) { return eeprom_read_byte(SVR_EEP_CONFIGURATION_PAGE, oset.offset); }
void SetConfigValue(SvrEepromOffset_t oset, uint8_t value)

{
	// writes single byte configuration value. Additional bytes in this foursome act as verification for the read
	// operation that written value is valid
	_Static_assert(SVR_CONFIG_BLOCK_SIZE == 4,
	               "SetConfigValue writes four bytes for each config value. If you change that block size, you better "
	               "change SetConfigValue too.");
	eeprom_write_byte(SVR_EEP_CONFIGURATION_PAGE, oset.offset, value);
	eeprom_write_byte(SVR_EEP_CONFIGURATION_PAGE, oset.offset + 1, (value + 37) & 0xff);
	eeprom_write_byte(SVR_EEP_CONFIGURATION_PAGE, oset.offset + 2, value ^ 0xff);
	eeprom_write_byte(SVR_EEP_CONFIGURATION_PAGE, oset.offset + 3, 65);
}

bool GetValidConfigValue(SvrEepromOffset_t oset, uint8_t *outValue)
{
	uint8_t ret;
	if (IsConfigOffsetValid_Impl(oset, &ret))
	{
		// Value was valid, return it in outValue
		*outValue = ret;
		// and tell caller that they got a live one.
		return true;
	}
	// Otherwise, return false without touching outValue.
	return false;
}

uint8_t GetValidConfigValueOrDefault(SvrEepromOffset_t oset, uint8_t defaultValue)
{
	uint8_t ret;
	if (IsConfigOffsetValid_Impl(oset, &ret))
	{
		// Value was valid, return it
		return ret;
	}
	// Not a valid config value found, so return the default instead.
	return defaultValue;
}

bool GetValidConfigValueOrWriteDefault(SvrEepromOffset_t oset, uint8_t defaultValue, uint8_t *outValue)
{
	if (GetValidConfigValue(oset, outValue))
	{
		// Value was valid, return true right away
		return true;
	}
	// Otherwise, write the default value to eeprom and return false accordingly.
	SetConfigValue(oset, defaultValue);
	return false;
}

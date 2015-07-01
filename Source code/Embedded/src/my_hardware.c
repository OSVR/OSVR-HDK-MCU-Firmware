/*
 * my_hardware.c
 *
 * Created: 10/2/2013 1:15:05 PM
 *  Author: Sensics
 */

#include "my_hardware.h"
#include <ioport.h>
#include "DeviceDrivers/Solomon.h"
#include <asf.h>
#include "Console.h"
#include "GlobalOptions.h"
#include "delay.h"

uint8_t actualNXP_1_ADDR=NXP_1_ADDR;
uint8_t actualCEC_1_ADDR=CEC_1_ADDR;

#ifndef OSVRHDK
    uint8_t actualNXP_2_ADDR=NXP_2_ADDR;
    uint8_t actualCEC_2_ADDR=CEC_2_ADDR;
#endif


struct pwm_config pwm_cfg[2];

void set_pwm_values(uint8_t Display1, uint8_t Display2)

// sets PWM of each display
{
    return; // remove this line if you want to use the on-board PWM
    pwm_stop(&pwm_cfg[0]);
    pwm_start(&pwm_cfg[0], Display1);
    pwm_stop(&pwm_cfg[1]);
    pwm_start(&pwm_cfg[1], Display2);
}

void custom_board_init(void)

{
#ifdef Solomon1_SPI
    ioport_configure_pin(AT86RFX_SPI_SCK, IOPORT_DIR_OUTPUT
                         | IOPORT_INIT_HIGH);
    ioport_configure_pin(AT86RFX_SPI_MOSI, IOPORT_DIR_OUTPUT
                         | IOPORT_INIT_HIGH);
    ioport_configure_pin(AT86RFX_SPI_MISO, IOPORT_DIR_INPUT);

    ioport_configure_pin(Solomon1_CSN, IOPORT_DIR_OUTPUT | IOPORT_INIT_HIGH);
    ioport_configure_pin(Solomon1_AddrData,IOPORT_DIR_OUTPUT | IOPORT_INIT_HIGH);


#ifndef OSVRHDK
    ioport_configure_pin(SPI_Mux_OE,IOPORT_DIR_OUTPUT | IOPORT_INIT_LOW);
    ioport_configure_pin(SPI_Mux_Select,IOPORT_DIR_OUTPUT | IOPORT_INIT_LOW);
#endif

    ioport_configure_pin(Solomon1_VOUT_Shutdown,IOPORT_DIR_OUTPUT | IOPORT_INIT_LOW);
    ioport_configure_pin(Solomon1_Reset,IOPORT_DIR_OUTPUT | IOPORT_INIT_HIGH);
#endif


#ifdef Solomon2_SPI
    ioport_configure_pin(Solomon2_VOUT_Shutdown,IOPORT_DIR_OUTPUT | IOPORT_INIT_LOW);
    ioport_configure_pin(Solomon2_Reset,IOPORT_DIR_OUTPUT | IOPORT_INIT_HIGH);
    ioport_configure_pin(Solomon2_AddrData,IOPORT_DIR_OUTPUT | IOPORT_INIT_HIGH);
#endif

    ioport_configure_pin(Debug_LED,IOPORT_DIR_OUTPUT | IOPORT_INIT_HIGH);


    ioport_configure_pin(NXP1_Reset_Pin,IOPORT_DIR_OUTPUT | IOPORT_INIT_HIGH);
    ioport_configure_pin(Int_HDMI_A, IOPORT_DIR_INPUT);

#ifdef OSVRHDK

#ifdef H546DLT01
    ioport_configure_pin(Side_by_side_A ,IOPORT_DIR_OUTPUT |  IOPORT_INIT_LOW); 
    ioport_configure_pin(Side_by_side_B ,IOPORT_DIR_OUTPUT |  IOPORT_INIT_LOW);
#else // LCD
	ioport_configure_pin(Side_by_side_A ,IOPORT_DIR_OUTPUT |  IOPORT_INIT_LOW);
	ioport_configure_pin(Side_by_side_B ,IOPORT_DIR_OUTPUT |  IOPORT_INIT_LOW);
#endif

#else
    ioport_configure_pin(Side_by_side_A ,IOPORT_DIR_OUTPUT |  IOPORT_INIT_HIGH); 
    ioport_configure_pin(Side_by_side_B ,IOPORT_DIR_OUTPUT |  IOPORT_INIT_HIGH);
#endif

#ifndef OSVRHDK

    ioport_configure_pin(NXP2_Reset_Pin,IOPORT_DIR_OUTPUT | IOPORT_INIT_HIGH);
    ioport_configure_pin(Int_HDMI_B, IOPORT_DIR_INPUT);
#endif


    ioport_configure_pin(USB_Hub_Reset_Pin,IOPORT_DIR_OUTPUT | IOPORT_INIT_LOW);
    ioport_configure_pin(USB_Hub_Power_Pin,IOPORT_DIR_OUTPUT | IOPORT_INIT_LOW);


#ifdef OSVRHDK
    ioport_configure_pin(LCD_avdd_en ,IOPORT_DIR_OUTPUT |  IOPORT_INIT_HIGH);
    ioport_configure_pin(LCD_avdd_en_sw ,IOPORT_DIR_OUTPUT |  IOPORT_INIT_HIGH);
    ioport_configure_pin(FPGA_unlocked, IOPORT_DIR_INPUT|IOPORT_MODE_PULLUP);
#endif

#ifndef OSVRHDK
    ioport_configure_pin(PWM_A ,IOPORT_DIR_OUTPUT |  IOPORT_INIT_HIGH);
    ioport_configure_pin(PWM_B ,IOPORT_DIR_OUTPUT |  IOPORT_INIT_HIGH);
#endif

    //ioport_configure_pin(Backlight,IOPORT_DIR_OUTPUT |  IOPORT_INIT_HIGH);

    // init both UARTs
    static usart_rs232_options_t usart_options = {
        .baudrate = FPGA_USART_BAUD_RATE,
        .charlength = FPGA_USART_SERIAL_CHAR_LENGTH,
        .paritytype = FPGA_USART_SERIAL_PARITY,
        .stopbits = FPGA_USART_SERIAL_STOP_BIT
    };


    if (!usart_init_rs232(FPGA1_USART, &usart_options))
        WriteLn("FPGA1 init err");

#ifndef OSVRHDK
    if (!usart_init_rs232(FPGA2_USART, &usart_options))
        WriteLn("FPGA2 init err");
#endif

#ifdef OSVRHDK
    ioport_configure_pin(FPGA_Reset_Pin,IOPORT_DIR_OUTPUT | IOPORT_INIT_LOW); // start FPGA in reset mode until there is video
#else
    ioport_configure_pin(FPGA_Reset_Pin,IOPORT_DIR_OUTPUT | IOPORT_INIT_HIGH);
#endif

	
    // init PWM for display brightness and strobing

    //pwm_init(&pwm_cfg[0], PWM_TCE0, PWM_CH_D, 11000); // PWM_A: PE3
    //pwm_init(&pwm_cfg[1], PWM_TCF0, PWM_CH_C, 11000); //PWM_B: PF2

    //pwm_init(&pwm_cfg[0], PWM_TCD1, PWM_CH_A, 60); // Debug_LED - D4
    //pwm_init(&pwm_cfg[1], PWM_TCE1, PWM_CH_A, 10); //Backlight - E4

    // Start both PWM channels
    //set_pwm_values(30,30);
    //ioport_set_pin_low(PWM_A);
    //ioport_set_pin_low(PWM_B);

	// init PF0 LED for debug purposes
	#ifdef OSVRHDK
	    ioport_configure_pin(Debug_LED,IOPORT_DIR_OUTPUT | IOPORT_INIT_HIGH);
	#endif

};
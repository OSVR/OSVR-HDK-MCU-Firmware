/*
 * FPGA.c
 *
 * Created: 9/1/2013 10:30:01 PM
 *  Author: Sensics
 */

#include <asf.h>
#include "FPGA.h"
#include "my_hardware.h"
#include "Console.h"

void FPGA_reset(void)

{
    ioport_set_pin_low(FPGA_Reset_Pin);
    delay_ms(1);
    ioport_set_pin_high(FPGA_Reset_Pin);
};

void FPGA_write(uint8_t FPGANun, uint8_t Command, uint8_t Value)

{
    switch (FPGANun)
    {
    case 1:
    {
        usart_putchar(FPGA1_USART, Command);
        usart_putchar(FPGA1_USART, Value);
        break;
    }
#ifndef OSVRHDK
    case 2:
    {
        usart_putchar(FPGA2_USART, Command);
        usart_putchar(FPGA2_USART, Value);
        break;
    }
#endif
    default:
    {
        WriteLn("Case err");
        break;
    }
    }

};

uint8_t FPGA_read(uint8_t FPGANun, uint8_t Command)

{
    uint8_t Value=0;

    Command|=0x80; // add leftmost bit to indicate read command
    switch (FPGANun)
    {
    case 1:
    {
        usart_putchar(FPGA1_USART, Command);
        //delay_ms(20);
        Value=usart_getchar(FPGA1_USART);
        break;
    }
#ifndef OSVRHDK
    case 2:
    {
        usart_putchar(FPGA2_USART, Command);
        //delay_ms(20);
        Value=usart_getchar(FPGA2_USART);
        break;
    }
#endif
    default:
    {
        WriteLn("Case err");
        break;
    }
    }
    return Value;
};

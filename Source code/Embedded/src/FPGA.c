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
#include "SvrYield.h"

#ifdef SVR_HAVE_FPGA
bool SideBySideMode = false;

void FPGA_start_reset()
{
#ifdef FPGA_Reset_Pin
	ioport_set_pin_low(FPGA_Reset_Pin);
#endif
}
void FPGA_end_reset()
{
#ifdef FPGA_Reset_Pin
	ioport_set_pin_high(FPGA_Reset_Pin);
#endif
}

void FPGA_reset(void)
{
	FPGA_start_reset();
	// 1ms was too short for anything useful to happen on dSight,
	// 10ms was intermittently successful, 15ms is pretty reliable.
	svr_yield_ms(15);
	FPGA_end_reset();
}
#if 0
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
}

uint8_t FPGA_read(uint8_t FPGANun, uint8_t Command)
{
	uint8_t Value = 0;

	Command |= 0x80;  // add leftmost bit to indicate read command
	switch (FPGANun)
	{
	case 1:
	{
		usart_putchar(FPGA1_USART, Command);
		// delay_ms(20);
		Value = usart_getchar(FPGA1_USART);
		break;
	}
#ifndef OSVRHDK
	case 2:
	{
		usart_putchar(FPGA2_USART, Command);
		// delay_ms(20);
		Value = usart_getchar(FPGA2_USART);
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
}
#endif
#endif  // SVR_HAVE_FPGA
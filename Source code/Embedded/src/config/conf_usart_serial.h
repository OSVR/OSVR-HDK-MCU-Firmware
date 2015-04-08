/**
 * \file *********************************************************************
 *
 * \brief USART Serial configuration
 *
 * Copyright (c) 2011 Atmel Corporation. All rights reserved.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. The name of Atmel may not be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * 4. This software may only be redistributed and used in connection with an
 *    Atmel microcontroller product.
 *
 * THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * \asf_license_stop
 *
 */

#ifndef CONF_USART_SERIAL_H_INCLUDED
#define CONF_USART_SERIAL_H_INCLUDED

#include "GlobalOptions.h"

#define  FPGA1_USART               &USARTF0
//#define  FPGA1_USART_RX_Vect       USARTF0_RXC_vect
//#define  FPGA1_USART_DRE_Vect      USARTF0_DRE_vect
//#define  FPGA1_USART_SYSCLK        SYSCLK_USART0
//#define  FPGA1_USART_PORT          PORTF
//#define  FPGA1_USART_PORT_PIN_TX   0x08  // PF3 (TXC0)
//#define  FPGA1_USART_PORT_PIN_RX   0x04  // PF2 (RXC0)
//#define  FPGA1_USART_PORT_SYSCLK   SYSCLK_PORT_F

#ifndef OSVRHDK
    #define  FPGA2_USART               &USARTE0
    //#define  FPGA2_USART_RX_Vect       USARTE0_RXC_vect
    //#define  FPGA2_USART_DRE_Vect      USARTE0_DRE_vect
    //#define  FPGA2_USART_SYSCLK        SYSCLK_USART0
    //#define  FPGA2_USART_PORT          PORTE
    //#define  FPGA2_USART_PORT_PIN_TX   0x08  // PE3 (TXC0)
    //#define  FPGA2_USART_PORT_PIN_RX   0x04  // PE2 (RXC0)
    //#define  FPGA2_USART_PORT_SYSCLK   SYSCLK_PORT_E
#endif

#define FPGA_USART_BAUD_RATE				19200
#define FPGA_USART_SERIAL_CHAR_LENGTH    USART_CHSIZE_8BIT_gc
#define FPGA_USART_SERIAL_PARITY         USART_PMODE_DISABLED_gc
#define FPGA_USART_SERIAL_STOP_BIT       false

#endif /* CONF_USART_SERIAL_H_INCLUDED */

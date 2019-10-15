/**
 * \file
 *
 * \brief ATmegaRFA1 KITRFA1 board header file.
 *
 * This file contains definitions and services related to the features of the
 * ATmega256RFR2 Xplained Pro board.
 *
 * To use this board, define BOARD= ATMEGA256RFR2_XPLAINED_PRO.
 *
 * Copyright (c) 2013 Atmel Corporation. All rights reserved.
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
#ifndef _KITRFA1_H_
#define _KITRFA1_H_

#include "compiler.h"
#include "led.h"

#if AVR8_PART_IS_DEFINED(ATmega128RFA1)
	#define MCU_SOC_NAME				"ATmega128RFA1"
	#define BOARD_NAME					"KITRFA1"
#elif AVR8_PART_IS_DEFINED(ATmega256RFR2)
	#define MCU_SOC_NAME				"ATmega256RFR2"
	#define BOARD_NAME					"KITRFR2"
#endif

/*
 * PINs where buttons are connected
 */
#define LED0_GPIO						IOPORT_CREATE_PIN(PORTG, 0)
#define LED1_GPIO						IOPORT_CREATE_PIN(PORTG, 1)
#define LED2_GPIO						IOPORT_CREATE_PIN(PORTG, 5)
#define LED0							LED0_GPIO
#define LED1							LED1_GPIO
#define LED2							LED2_GPIO
#define LED_COUNT						3

#define GPIO_PUSH_BUTTON_0				IOPORT_CREATE_PIN(PORTG, 2)

//! \name Communication interfaces
//@{
#define TWID_SDA						IOPORT_CREATE_PIN(PORTD, 1)
#define TWID_SCL						IOPORT_CREATE_PIN(PORTD, 0)
#define USARTA0_RXD						IOPORT_CREATE_PIN(PORTE, 0)
#define USARTA0_TXD						IOPORT_CREATE_PIN(PORTE, 1)
#define USARTA1_RXD						IOPORT_CREATE_PIN(PORTD, 2)
#define USARTA1_TXD						IOPORT_CREATE_PIN(PORTD, 3)
#define SPIB_SS							IOPORT_CREATE_PIN(PORTB, 0)
#define SPIB_MOSI						IOPORT_CREATE_PIN(PORTB, 2)
#define SPIB_MISO						IOPORT_CREATE_PIN(PORTB, 3)
#define SPIB_SCK						IOPORT_CREATE_PIN(PORTB, 1)
//@}

/*Board has an External PA Connected to the Radio, Following are the constants related to the Ext PA */
#define ANTENNA_DIVERSITY				(0)
#define ANTENNA_DEFAULT					(0)

// Device List
#define VCNL40XX_DEVICE					(0x26)
#define MCP98243_TEMP_DEVICE			(0x18)
#define MCP98243_MEM_DEVICE				(0x50)

#endif  /* _KITRFA1_H_ */

/**
 * \file halGpio.h
 *
 * \brief ATSAMD20 GPIO interface
 *
 * Copyright (C) 2012-2013, Atmel Corporation. All rights reserved.
 *
 * \asf_license_start
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
 * $Id: halGpio.h 8410 2013-08-08 16:59:55Z ataradov $
 *
 */

#ifndef _HAL_GPIO_H_
#define _HAL_GPIO_H_

/*- Includes ---------------------------------------------------------------*/
#include <stdint.h>
#include "sysTypes.h"

/*- Definitions ------------------------------------------------------------*/
#define PORT_INSTANCE_ADDR		REG_PIOA_PER
#define PORT_INSTANCE_OFFSET	0x200
#define PORT_INSTANCE_REG(port, offset) MMIO_REG(PORT_INSTANCE_ADDR + (PORT_INSTANCE_OFFSET << RES(PORT##port##_INSTANCE)) + offset, uint32_t)
#define PORTA_INSTANCE 0
#define PORTB_INSTANCE 1
#define PORTC_INSTANCE 2

#define PERIPHERAL_FUNCTION_A		0x00
#define PERIPHERAL_FUNCTION_B		0x01
#define PERIPHERAL_FUNCTION_C		0x02
#define PERIPHERAL_FUNCTION_EXTRA	0x03
#define PERIPHERAL_FUNCTION_SYSTEM	0x04

#define PORT_PMUX_FUNCTION_OFFSET(bit)	((bit % 2) == 1 ? PORT_PMUX_PMUXO_Pos : PORT_PMUX_PMUXE_Pos)
#define PORT_PMUX_FUNCTION_MASK(bit)	((bit % 2) == 1 ? PORT_PMUX_PMUXO_Msk : PORT_PMUX_PMUXE_Msk)
#define PORT_PMUX_FUNCTION_NOTMASK(bit)	((bit % 2) == 1 ? PORT_PMUX_PMUXE_Msk : PORT_PMUX_PMUXO_Msk)

#define  CONCAT(a, b) XCAT(a, b)
#define  XCAT(a, b)   a ## b
#define  RES(R)       R
#define CONCAT2(port, sufix)			CONCAT(XCAT(REG_PIO, port), _##sufix)


#define HAL_GPIO_PIN(name, port, bit) \
	INLINE void    HAL_GPIO_##name##_set(void)      { CONCAT2(port, SODR)									= (1 << bit); }				\
	INLINE void    HAL_GPIO_##name##_clr(void)      { CONCAT2(port, CODR)									= (1 << bit); }				\
	INLINE void    HAL_GPIO_##name##_in(void)       { CONCAT2(port, ODR)									= (1 << bit); }				\
	INLINE void    HAL_GPIO_##name##_out(void)      { CONCAT2(port, PER)									= (1 << bit);				\
													  CONCAT2(port, OER)									= (1 << bit); }				\
	INLINE void    HAL_GPIO_##name##_pullup(void)   { CONCAT2(port, PPDDR)									= (1 << bit);				\
													  CONCAT2(port, PUER)									= (1 << bit); }				\
	INLINE uint8_t HAL_GPIO_##name##_read(void)		{ return((CONCAT2(port, PDSR) & (1 << bit)) != 0);					  }				\
	INLINE uint8_t HAL_GPIO_##name##_state(void)	{ return((CONCAT2(port, OSR) & (1 << bit)) != 0);					  }

#define HAL_GPIO_PMUX_FUNCTION(port, bit, function)	{																					\
	uint8_t			value												= PORT_INSTANCE_REG(port, PORT_PMUX_OFFSET + (bit / 2));		\
	value																&= PORT_PMUX_FUNCTION_NOTMASK(bit);								\
	PORT_INSTANCE_REG(port, PORT_PMUX_OFFSET + (bit / 2))				= (value | (function & 0x03)); }

#endif // _HAL_GPIO_H_

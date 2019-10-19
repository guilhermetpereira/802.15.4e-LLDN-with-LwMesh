/**
 * \file halPhy.c
 *
 * \brief ATSAMD20 PHY interface implementation
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
 * $Id: halPhy.c 8367 2013-07-25 17:18:50Z ataradov $
 *
 */

/*- Includes ---------------------------------------------------------------*/
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "hal.h"
#include "halPhy.h"
#include "halGpio.h"
#include "phy.h"
#include "conf_board.h"
#include <sysclk.h>
#include <pio.h>
#include <ioport.h>
#include <spi_master.h>

/*- Defines ----------------------------------------------------------------*/
#define				SPI_CLK_SPEED		4000000				// 4-MHz

/*- Implementations --------------------------------------------------------*/
struct spi_device SPI_AT86RFX_DEVICE	=
{
	.id									= RF_SPI_CHIP_SELECT	// Board specific select id
};

/*************************************************************************//**
*****************************************************************************/
uint8_t HAL_PhySpiWriteByte(uint8_t value)
{
	return(HAL_PhySpiWriteByteInline(value));
}

/*************************************************************************//**
*****************************************************************************/
void HAL_PhyReset(void)
{
	HAL_GPIO_PHY_RST_clr();
	HAL_Delay(10);
	HAL_GPIO_PHY_RST_set();
}

/*************************************************************************//**
*****************************************************************************/
void halPhyInit(void)
{
	spi_master_init(AT86RFX_SPI);
	spi_master_setup_device(AT86RFX_SPI, &SPI_AT86RFX_DEVICE, SPI_MODE_0, SPI_CLK_SPEED, 0);
	spi_set_peripheral_chip_select_value(AT86RFX_SPI, (0x0F & ~(1 << SPI_AT86RFX_DEVICE.id)));
	spi_enable(AT86RFX_SPI);

	ioport_set_pin_mode(AT86RFX_CS_PIN, AT86RFX_CS_FLAGS);
	ioport_set_pin_dir(AT86RFX_CS_PIN, IOPORT_DIR_OUTPUT);
	ioport_set_pin_level(AT86RFX_CS_PIN, IOPORT_PIN_LEVEL_HIGH);
	ioport_enable_pin(AT86RFX_CS_PIN);

	ioport_set_pin_mode(AT86RFX_IRQ0_PIN, AT86RFX_IRQ0_FLAGS);
	ioport_set_pin_dir(AT86RFX_IRQ0_PIN, IOPORT_DIR_INPUT);
	ioport_set_pin_sense_mode(AT86RFX_IRQ0_PIN, IOPORT_SENSE_RISING);
	ioport_enable_pin(AT86RFX_IRQ0_PIN);

	pio_configure_interrupt(AT86RFX_IRQ0_PIO, AT86RFX_IRQ0_MASK, AT86RFX_IRQ0_ATTR);

	ioport_set_pin_mode(AT86RFX_IRQ1_PIN, AT86RFX_IRQ1_FLAGS);
	ioport_set_pin_dir(AT86RFX_IRQ1_PIN, IOPORT_DIR_INPUT);
	ioport_set_pin_sense_mode(AT86RFX_IRQ1_PIN, IOPORT_SENSE_RISING);
	ioport_enable_pin(AT86RFX_IRQ1_PIN);

	pio_configure_interrupt(AT86RFX_IRQ1_PIO, AT86RFX_IRQ1_MASK, AT86RFX_IRQ1_ATTR);

	NVIC_DisableIRQ(RF_PHY_Vect_IRQn);
	NVIC_ClearPendingIRQ(RF_PHY_Vect_IRQn);
	NVIC_EnableIRQ(RF_PHY_Vect_IRQn);

	pio_enable_interrupt(AT86RFX_IRQ0_PIO, AT86RFX_IRQ0_MASK);
	pio_enable_interrupt(AT86RFX_IRQ1_PIO, AT86RFX_IRQ1_MASK);
}

/*************************************************************************//**
*****************************************************************************/
ISR(RF_PHY_Vect)
{
	if((pio_get_interrupt_status(RF_PHY_Vect_PIO) & AT86RFX_IRQ1_MASK) == AT86RFX_IRQ1_MASK)
	{
		phyInterruptHandler();
	}
}
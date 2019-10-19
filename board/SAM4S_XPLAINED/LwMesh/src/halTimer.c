/**
 * \file halTimer.c
 *
 * \brief ATSAMD20 timer implementation
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
 * $Id: halTimer.c 8367 2013-07-25 17:18:50Z ataradov $
 *
 */

/*- Includes ---------------------------------------------------------------*/
#include <stdint.h>
#include "hal.h"
#include "halTimer.h"
#include "conf_board.h"
#include <sysclk.h>
#include <tc.h>

/*- Definitions ------------------------------------------------------------*/
#define TIMER_PRESCALER  32
#define TIMER_TOP        (((F_CPU / 1000ul) / TIMER_PRESCALER) * HAL_TIMER_INTERVAL)

/*- Variables --------------------------------------------------------------*/
volatile uint8_t halTimerIrqCount;
static volatile uint8_t halTimerDelayInt;

/*- Implementations --------------------------------------------------------*/

/*************************************************************************//**
*****************************************************************************/
void HAL_TimerInit(void)
{
	halTimerIrqCount					= 0;

	// System Tick
	sysclk_enable_peripheral_clock(SYSTIMER_TICK_CHANNEL_ID);
	tc_init(SYSTIMER, SYSTIMER_TICK_CHANNEL, TC_CMR_TCCLKS_TIMER_CLOCK3 | TC_CMR_WAVE | TC_CMR_WAVSEL_UP_RC);

	NVIC_DisableIRQ(SYSTIMER_TICK_CHANNEL_IRQn);
	NVIC_ClearPendingIRQ(SYSTIMER_TICK_CHANNEL_IRQn);
	NVIC_EnableIRQ(SYSTIMER_TICK_CHANNEL_IRQn);
	tc_enable_interrupt(SYSTIMER, SYSTIMER_TICK_CHANNEL, TC_IER_CPCS);

	tc_write_rc(SYSTIMER, SYSTIMER_TICK_CHANNEL, TIMER_TOP);
	tc_start(SYSTIMER, SYSTIMER_TICK_CHANNEL);

	sysclk_enable_peripheral_clock(SYSTIMER_DELAY_CHANNEL_ID);
	tc_init(SYSTIMER, SYSTIMER_DELAY_CHANNEL, TC_CMR_TCCLKS_TIMER_CLOCK3 | TC_CMR_WAVE | TC_CMR_WAVSEL_UP_RC | TC_CMR_CPCDIS | TC_CMR_CPCSTOP);

	NVIC_DisableIRQ(SYSTIMER_DELAY_CHANNEL_IRQn);
	NVIC_ClearPendingIRQ(SYSTIMER_DELAY_CHANNEL_IRQn);
	NVIC_EnableIRQ(SYSTIMER_DELAY_CHANNEL_IRQn);
	
	tc_enable_interrupt(SYSTIMER, SYSTIMER_DELAY_CHANNEL, TC_IER_CPCS);
}

/*************************************************************************//**
*****************************************************************************/
void HAL_TimerDelay(uint16_t us)
{
	halTimerDelayInt					= 0;
	
	tc_write_rc(SYSTIMER, SYSTIMER_DELAY_CHANNEL, us);
	tc_start(SYSTIMER, SYSTIMER_DELAY_CHANNEL);	

	// Wait compare match interrupt...
	while(halTimerDelayInt == 0)
	{
		// Stop processor clock until next interrupt...
		__WFI();
	}
}

/*************************************************************************//**
*****************************************************************************/
ISR(SYSTIMER_OVF_Vect)
{
	tc_get_status(SYSTIMER, SYSTIMER_TICK_CHANNEL);
	++halTimerIrqCount;
}

ISR(SYSTIMER_CCB_Vect)
{
	tc_get_status(SYSTIMER, SYSTIMER_DELAY_CHANNEL);
	halTimerDelayInt					= 1;
}
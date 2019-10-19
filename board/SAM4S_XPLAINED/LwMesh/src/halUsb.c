/**
 * \file
 *
 * \brief PCWM functions for AVR Xmega
 *
 * Copyright (c) 2011 - 2012 Atmel Corporation. All rights reserved.
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

#include "config.h"
#include "conf_board.h"
#include "udc.h"
#include "halUsb.h"

static volatile bool driver_b_cdc_enable = false;

void driver_vbus_action(bool b_high)
{
	if(b_high)
	{
		// Attach USB Device
		udc_attach();
	}
	else
	{
		// VBUS not present
		udc_detach();
	}
}
void driver_sof_action(void)
{
	if(!driver_b_cdc_enable)
		return;
	
//	ui_process(udd_get_frame_number());
}
void driver_suspend_action(void)
{
//	ui_powerdown();
}
void driver_resume_action(void)
{
//	ui_wakeup();
}
bool driver_cdc_enable(uint8_t port)
{
	UNUSED(port);

	driver_b_cdc_enable				= true;

	return(true);
}
void driver_cdc_disable(uint8_t port)
{
	UNUSED(port);

	driver_b_cdc_enable				= false;
}
void driver_cdc_set_dtr(uint8_t port, bool b_enable)
{
	UNUSED(port);
	UNUSED(b_enable);
}
void driver_usb_cdc_init(void)
{
	// Start USB stack to authorize VBus monitoring
	udc_start();

	if(!udc_include_vbus_monitoring())
	{
		// VBUS monitoring is not available on this product
//#if (PCWM_USB_SELF_POWERED == 1)
		//// SO, VBUS is monitoring by I/O PD 5 IRQ
		//PORTD.INTCTRL				|= (PORT_INT0LVL0_bm | PORT_INT0LVL1_bm);	// High Level Int
		//PORTD.INT0MASK				= (1 << PIN5);								// PD5
		//PORTD.PIN5CTRL				= PORT_ISC_BOTHEDGES_gc;					// Enable Both Edge Int
		//PMIC.CTRL					|= PMIC_HILVLEN_bm;							// Enable HIGH Level Int
//#else
		//// activate always!
		driver_vbus_action(true);
//#endif
	}
}
void driver_usb_cdc_task(void)
{
	// today, do nothing!
}
void driver_cdc_rx_notify(uint8_t port)
{
	(void) port;
//#if (PCWM_PORT == PCWM_USB_PORT)
	//if(port == PCWM_USB_PORT_NUMBER)
	//{
		//LLC_ProcessData();
	//}
//#endif
#if (HAL_USB_ENABLED == 1)
	USB_ProcessData();
#endif
}
//ISR(PORTD_INT0_vect)
//{
	//uint8_t						input						= PORTD_IN;
	//input													&= (1 << PIN5);
	//bool						vbus						= (input == (1 << PIN5));
	//driver_vbus_action(vbus);
//}
//
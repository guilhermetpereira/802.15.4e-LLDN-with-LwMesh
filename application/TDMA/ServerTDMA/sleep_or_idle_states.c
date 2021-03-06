/*
 * sleep_n_idle_states.c
 *
 * Created: 17/02/2021 19:51:19
 *  Author: Guilherme
 */ 

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include "config.h"
#include "sys.h"
#include "phy.h"
#include "sys.h"
#include "nwk.h"
#include "sysclk.h"
#include "sysTimer.h"
#include "sleep_mgr.h"
#include "sleepmgr.h"
#include "ioport.h"
#include "conf_sleepmgr.h"
#include "board.h"
#include "platform.h"

#include "lldn.h"

#include "conf_sio2host.h"
#include "macsc_megarf.h"

AppState_t appState = APP_STATE_SLEEP_PREPARE; 
// AppState_t appState = APP_STATE_INITIAL;


static void APP_TaskHandler(void)
{
	switch (appState){
		case APP_STATE_INITIAL:
		{
			/* Init Radio Module */ 
			NWK_SetAddr(APP_ADDR);
			PHY_SetChannel(APP_CHANNEL);
			PHY_SetRxState(true);
			PHY_SetTdmaMode(true);
			PHY_SetPromiscuousMode(true);
			appState = APP_STATE_IDLE;
		}
		case APP_STATE_SLEEP_PREPARE:
		{
			if(!NWK_Busy())
			{
				irqflags_t flags = cpu_irq_save();
				NWK_SleepReq();
				appState		= APP_STATE_SLEEP;
				cpu_irq_restore(flags);
			}
			break;
		}
		case APP_STATE_SLEEP:
		{
			sleep_enable();
			sleep_enter();
			sleep_disable();
			break;
		}
		default:
		break;
	}
}


	/*****************************************************************************
	*****************************************************************************/
int main(void)
{
	sysclk_init();
	board_init();

	SYS_Init();
	/* Disable CSMA/CA
		* Disable auto ACK
		* Enable Rx of LLDN Frame Type as described in 802.15.4e - 2012 
		*/

	sm_init();

	// Initialize interrupt vector table support.
#if (SIO2HOST_CHANNEL == SIO_USB)
	irq_initialize_vectors();
#endif
	cpu_irq_enable();

#if 1
#if (SIO2HOST_CHANNEL == SIO_USB)
	stdio_usb_init();
#else
	const usart_serial_options_t usart_serial_options =
	{
		.baudrate     = USART_HOST_BAUDRATE,
		.charlength   = USART_HOST_CHAR_LENGTH,
		.paritytype   = USART_HOST_PARITY,
		.stopbits     = USART_HOST_STOP_BITS
	};

	stdio_serial_init(USART_HOST, &usart_serial_options);
	usart_double_baud_enable(USART_HOST);
	usart_set_baudrate_precalculated(USART_HOST, USART_HOST_BAUDRATE, sysclk_get_source_clock_hz());

#endif
#endif
	for(;;)
	{
		SYS_TaskHandler();
		APP_TaskHandler();
	}
}
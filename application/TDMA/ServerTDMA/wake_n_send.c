/*
 * wake_n_send.c
 *
 * Created: 17/02/2021 20:04:40
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

#define DELAY_INTERVAL 0.5 // (seconds)

AppState_t appState = APP_STATE_INITIAL; 



uint8_t data_payload = APP_ADDR;
static NWK_DataReq_t msgReqData = { .dstAddr =0,
									.dstEndpoint = APP_COMMAND_ENDPOINT,
									.srcEndpoint = APP_COMMAND_ENDPOINT,
									.options = NWK_OPT_LLDN_DATA,
									.data = (uint8_t*)&data_payload,
									.size = sizeof(data_payload)};

uint32_t cmp_value;
int a = 1;
static void online_time_hndlr(void)
{
	/* TOGGLE PIN HERE */
 	PORTD= 0xff;
	cmp_value = macsc_read_count();
	// appState = APP_STATE_WAKEUP_AND_SEND;
	macsc_use_cmp(0, cmp_value + DELAY_INTERVAL / SYMBOL_TIME , MACSC_CC1);
	if(a==1)
	{
		macsc_use_cmp(0,cmp_value + (DELAY_INTERVAL / 2 )/ SYMBOL_TIME , MACSC_CC2);
		a = 0;
	}
	else
	a = 1;
	printf("\nA");

}

static void online_time_hndlr_df(void)
{
	printf("\nB");

}
static void APP_TaskHandler(void)
{
	switch (appState){
		case APP_STATE_INITIAL:
		{
			NWK_SetAddr(APP_ADDR);
			PHY_SetChannel(APP_CHANNEL);
			PHY_SetRxState(true);
			PHY_SetTdmaMode(true);
			PHY_SetPromiscuousMode(true);
			
			macsc_set_cmp1_int_cb(online_time_hndlr);
			macsc_enable_cmp_int(MACSC_CC1);
				cmp_value = macsc_read_count();

			macsc_use_cmp(1, DELAY_INTERVAL / SYMBOL_TIME , MACSC_CC1);
			
			macsc_set_cmp2_int_cb(online_time_hndlr_df);
			macsc_enable_cmp_int(MACSC_CC2);
			
 			macsc_enable_manual_bts();
			appState = APP_STATE_SLEEP_PREPARE;
			break;
		}
		case APP_STATE_SLEEP_PREPARE:
		{		
			
			if(/*!NWK_Busy()*/0)
			{
				DDRD = 0x00;
				irqflags_t flags = cpu_irq_save();
				NWK_SleepReq();
				appState		= APP_STATE_SLEEP;
				cpu_irq_restore(flags);
			}
			else
				appState = APP_STATE_IDLE;
			break;
		}
		case APP_STATE_SLEEP:
		{
			
			sleep_enable();
			sleep_enter();
			sleep_disable();
			break;
		}
		case APP_STATE_WAKEUP_AND_SEND:
		{
			NWK_WakeupReq();
			NWK_DataReq(&msgReqData);
			macsc_enable_manual_bts();
			appState			= APP_STATE_SLEEP_PREPARE;
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
	
	
	/* TOGGLE PIN HERE */

	PORTD	= 0xFF;
	DDRD = 0xFF;
	
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
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
#include "led.h"
#include "ioport.h"
#include "conf_sleepmgr.h"
#include "board.h"
#include "platform.h"

#include "lldn.h"

#include "conf_sio2host.h"
#include "macsc_megarf.h"

#if APP_COORDINATOR
#define DELAY_INTERVAL 0.02 // (seconds)
#else
#define DELAY_INTERVAL 0.02 - 0.001 // (seconds)
#endif
uint32_t cmp_value;

uint8_t data_payload[4] = {APP_ADDR,APP_ADDR,APP_ADDR,APP_ADDR};
static NWK_DataReq_t msgReqData = { .dstAddr =0,
							.dstEndpoint = APP_COMMAND_ENDPOINT,
							.srcEndpoint = APP_COMMAND_ENDPOINT,
							.options = NWK_OPT_LLDN_DATA,
							.data = (uint8_t*)&data_payload,
							.size = sizeof(data_payload)};


AppState_t appState = APP_STATE_INITIAL;

static void online_time_hndlr(void)
{
	/* TOGGLE PIN HERE */
	#if APP_COORDINATOR
	NWK_DataReq(&msgReqData);
	macsc_enable_manual_bts();
	printf("s");
	#else
	PORTD = 0xFF;
	appState = APP_STATE_WAKEUP_AND_SEND;  /* FAKE NEWS � ENVIA NADA */
	
	uint32_t cmp_value_tmr = macsc_read_count();
	printf("\nTMR : %x",cmp_value_tmr);
	
	#endif
	return;
}

static bool appBeaconInd(NWK_DataInd_t *ind)
{
	(void)ind;
	cmp_value = macsc_read_count();
 	printf("\nMSG : %x",cmp_value);
	macsc_enable_manual_bts();

	
	appState = APP_STATE_PREP_TMR;
	return true;
}

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

			NWK_OpenEndpoint(APP_BEACON_ENDPOINT, appBeaconInd);
			NWK_OpenEndpoint(APP_DATA_ENDPOINT, appBeaconInd);


			#if APP_COORDINATOR
			macsc_set_cmp1_int_cb(online_time_hndlr);
			macsc_enable_cmp_int(MACSC_CC1);
			macsc_use_cmp(MACSC_RELATIVE_CMP, (DELAY_INTERVAL)/ SYMBOL_TIME , MACSC_CC1);
			
			macsc_enable_manual_bts();
			#endif
			

				
			appState = APP_STATE_IDLE;
			break;
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
		case APP_STATE_WAKEUP_AND_SEND: /* FAKE NEWS � ENVIA NADA */
		{
			NWK_WakeupReq();
			appState			= APP_STATE_IDLE;
			break;
		}
		case APP_STATE_PREP_TMR:
		{
			macsc_set_cmp1_int_cb(online_time_hndlr);
			macsc_enable_cmp_int(MACSC_CC1);
			macsc_use_cmp(0, cmp_value + (DELAY_INTERVAL)/ SYMBOL_TIME , MACSC_CC1);
			PORTD = 0x00;
			appState = APP_STATE_SLEEP_PREPARE;
			break;
		}
		case APP_STATE_IDLE:
		{
			break;
		}

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

		DDRD	= 0xFF;
		PORTD = 0x00;
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
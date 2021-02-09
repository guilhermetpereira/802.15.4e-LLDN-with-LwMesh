/*
	* ServerLLDN.c
	*
	* Created: 10/18/2019 5:15:37 PM
	*  Author: guilherme
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



#if 1
#if (SIO2HOST_CHANNEL == SIO_USB)
/* Only ARM */
#include "hw_timer_lldn.h"
#include "stdio_usb.h"
#define MASTER_MACSC	0
#else
/* Only megarf series */
#include "conf_sio2host.h"
#define MASTER_MACSC	1
#endif
#else
/* Only megarf series */
#define MASTER_MACSC		1
#endif


#if (MASTER_MACSC == 1)
#include "macsc_megarf.h"
#else
static SYS_Timer_t				tmrBeaconInterval;			// Beacon
static SYS_Timer_t				tmrComputeData;				// Compute data
#endif

static SYS_Timer_t tmrDelay;	
AppState_t	appState = APP_STATE_INITIAL;

uint8_t assTimeslot=1;
nodes_info_t nodes_info_arr[50];





static void send_info_serial(void)
{
	/*prints # nodes*/
	printf("S%hhx",assTimeslot);
	
	for (int i = 0; i < assTimeslot;i++)
	{
		/* prints N<assigned ts><energy> */ 
		printf("N%hhx%02X%02X%02X%02X"
		,nodes_info_arr[i].assigned_time_slot
		,(nodes_info_arr[i].energy>>24)&0xFF
		,(nodes_info_arr[i].energy>>16)&0xFF
		,(nodes_info_arr[i].energy>>8)&0xFF
		,nodes_info_arr[i].energy&0xFF);
		for (int j=0; j < (int)ceil(assTimeslot/8.0);j++)
		{
			printf("%01x", nodes_info_arr[i].neighbors[j]);
		}
		printf("F");
	}
	printf("T");
}

static void tmrDelayHandler(SYS_Timer_t *timer)
{
	send_info_serial();
	appState = APP_STATE_INITIAL;
}


static void APP_TaskHandler(void)
{
	switch (appState){
		case APP_STATE_INITIAL:
		{
			nodes_info_arr[0].assigned_time_slot = 0x03;
			nodes_info_arr[0].energy= 35;
			nodes_info_arr[0].neighbors[0] = 0x01;
			SYS_TimerStart(&tmrDelay);
			appState = APP_STATE_IDLE;
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

		tmrDelay.interval = 3*1000;
		tmrDelay.mode = SYS_TIMER_INTERVAL_MODE;
		tmrDelay.handler = tmrDelayHandler;

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

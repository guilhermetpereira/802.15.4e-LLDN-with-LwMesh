/*
 * Teste.c
 *
 * Created: 09/02/2020 15:58:34
 *  Author: Guilherme
 */ 

#include "hw_timer_lldn.h"
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

#if APP_COORDINATOR
#if (SIO2HOST_CHANNEL == SIO_USB)
/* Only ARM */
#include "stdio_usb.h"
#define MASTER_MACSC	0
#define TIMESLOT_TIMER	0
#else
/* Only megarf series */
#include "conf_sio2host.h" // necessary for prints
#define MASTER_MACSC	1
#define TIMESLOT_TIMER	0
#endif
#else
/* Only megarf series */
#include "conf_sio2host.h" // necessary for prints
#define MASTER_MACSC		1
#endif

typedef enum Estado_t
{
	INIT,
	IDLE,	
}Estado_t;

Estado_t state = IDLE;
static SYS_Timer_t	tmrInit;				// Feedback
static NWK_DataReq_t msgReq;
static NWK_ConfigStatus_t msgDiscResponse = { .id = LL_DISCOVER_RESPONSE,
	.macAddr = APP_ADDR,
	.ts_dir.tsDuration = 0x01,
.ts_dir.dirIndicator = 1 };

int tmr_cont = 0;

void hw_timer_handler(void)
{
	// timer_start();
		timer_stop();
		printf("\n hw_timer_handler");
		NWK_DataReq(&msgReq);	
		tmr_cont = 0;
}

static bool appBeaconInd(NWK_DataInd_t *ind)
{
	printf("\nBeacon Recebido");
	timer_start();
	return true;
}

void APP_TaskHandler(void)
{
	
	switch (state)
	{
		case INIT:
		{
			
			timer_init();
			timer_delay((uint16_t) 150 - 100);
			hw_timer_setup_handler(hw_timer_handler);
			/*
			 * Enable CSMA/CA
			 * Enable Random CSMA seed generator
			 */
			NWK_SetAddr(APP_ADDR);
			PHY_SetChannel(APP_CHANNEL);
			PHY_SetRxState(true);
			PHY_SetPromiscuousMode(true);
			PHY_SetTdmaMode(true);
			// PHY_SetOptimizedCSMAValues();
		
			NWK_OpenEndpoint(APP_BEACON_ENDPOINT, appBeaconInd);
			/*
			* Configure interrupts callback functions
			*/
			
			
			msgDiscResponse.id = LL_DISCOVER_RESPONSE;
			msgDiscResponse.s_macAddr = 0x11;
			msgDiscResponse.assTimeSlot = 0x22;
			msgDiscResponse.macAddr = 0x24;
			msgDiscResponse.ts_dir.tsDuration = (uint8_t)15;
			msgDiscResponse.ts_dir.dirIndicator = 1 << 0;
			
				
			msgReq.dstAddr				= 0;
			msgReq.dstEndpoint			= APP_COMMAND_ENDPOINT;
			msgReq.srcEndpoint			= APP_COMMAND_ENDPOINT;
			msgReq.options				= NWK_OPT_MAC_COMMAND;
			msgReq.data					= (uint8_t*)&msgDiscResponse;
			msgReq.size					= sizeof(msgDiscResponse);
			
			NWK_DataReq(&msgReq);
			
			state = IDLE;
			break;
		}
		case IDLE:
		break;
	}
	
}


static void tmrDelayHandler(SYS_Timer_t *timer)
{
	state = INIT;
	SYS_TimerStop(&tmrInit);
	return;
}


int main(void)
{
	sysclk_init();
	board_init();

	SYS_Init();
	// Disable CSMA/CA
	// Disable auto ACK
	sm_init();

	// Initialize interrupt vector table support.
	#if (SIO2HOST_CHANNEL == SIO_USB)
	irq_initialize_vectors();
	#endif
	cpu_irq_enable();

	#if APP_COORDINATOR
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
	
	
	 tmrInit.interval = 1000;
	 tmrInit.mode = SYS_TIMER_PERIODIC_MODE;
	 tmrInit.handler = tmrDelayHandler;
	 SYS_TimerStart(&tmrInit);


	for(;;)
	{
		SYS_TaskHandler();
		APP_TaskHandler();
	}
}
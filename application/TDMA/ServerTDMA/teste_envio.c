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

#define HUMAM_READABLE			1

#if (MASTER_MACSC == 1)
#include "macsc_megarf.h"
#define TIMESLOT_TIMER 0
#else
static SYS_Timer_t				tmrBeaconInterval;			// Beacon
static SYS_Timer_t				tmrComputeData;				// Compute data
#endif

#define PRINT 1

	
// equation for tTS gives time in seconds, the division by SYMBOL_TIME changes to symbols for counter usage
AppState_t	appState = APP_STATE_INITIAL;
static NWK_DataReq_t msgReq;
static uint8_t PanId;
static NWK_DataReq_t msgReq;

static NWK_ConfigStatus_t msgConfigStatus = { .id = LL_CONFIGURATION_STATUS,
												.macAddr = APP_ADDR,
												.s_macAddr = APP_ADDR,
												.ts_dir.tsDuration = 127,
												.ts_dir.dirIndicator = 1,
												.assTimeSlot = 0xff };

static bool appCommandInd(NWK_DataInd_t *ind)
{
	uint32_t cmp_value = macsc_read_bts();
	printf("\n%" PRIu32 "\n",cmp_value);
	macsc_disable();
}

static void APP_TaskHandler(void)
{
	switch (appState){
		case APP_STATE_INITIAL:
		{
			printf("\nINIT");

			NWK_SetAddr(APP_ADDR);
			PHY_SetChannel(APP_CHANNEL);
			PHY_SetRxState(true);
			PHY_SetTdmaMode(false);

			NWK_OpenEndpoint(APP_COMMAND_ENDPOINT, appCommandInd);
			PHY_SetPromiscuousMode(true);
	
			macsc_write_count(0x00000000);

			msgReq.dstAddr				= 0;
			msgReq.dstEndpoint			= APP_COMMAND_ENDPOINT;
			msgReq.srcEndpoint			= APP_COMMAND_ENDPOINT;
			msgReq.options				= NWK_OPT_MAC_COMMAND;
			msgReq.data					= (uint8_t*)&msgConfigStatus;
			msgReq.size					= sizeof(msgConfigStatus);
		
			NWK_DataReq(&msgReq);

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
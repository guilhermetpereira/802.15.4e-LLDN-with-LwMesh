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
	uint8_t data_payload = APP_ADDR;

	
// equation for tTS gives time in seconds, the division by SYMBOL_TIME changes to symbols for counter usage
AppState_t	appState = APP_STATE_INITIAL;
static uint8_t PanId;
static NWK_DataReq_t msgReq;										
static NWK_DiscoverResponse_t msgDiscResponse = { .id = LL_DISCOVER_RESPONSE,
	.macAddr = APP_ADDR,
	.ts_dir.tsDuration = 50,
.ts_dir.dirIndicator = 0b1 };
static NWK_ConfigStatus_t msgConfigStatus = { .id = LL_CONFIGURATION_STATUS,
	.macAddr = APP_ADDR,
	.s_macAddr = APP_ADDR,
	.ts_dir.tsDuration = 50,
	.ts_dir.dirIndicator = 1,
.assTimeSlot = 0xff };
											
static NWK_DataReq_t msgReqDiscResponse = { .dstAddr = 0,
	.dstEndpoint = APP_COMMAND_ENDPOINT,
	.srcEndpoint = APP_COMMAND_ENDPOINT,
	.options = NWK_OPT_MAC_COMMAND,
	.data = (uint8_t*)&msgDiscResponse,
.size = sizeof(msgDiscResponse)};
											
static NWK_DataReq_t msgReqConfigStatus = { .dstAddr =0,
	.dstEndpoint = APP_COMMAND_ENDPOINT,
	.srcEndpoint = APP_COMMAND_ENDPOINT,
	.options = NWK_OPT_MAC_COMMAND,
	.data = (uint8_t*)&msgConfigStatus,
.size = sizeof(msgConfigStatus)};
											
static NWK_DataReq_t msgReqData = { .dstAddr =0,
	.dstEndpoint = APP_COMMAND_ENDPOINT,
	.srcEndpoint = APP_COMMAND_ENDPOINT,
	.options = NWK_OPT_LLDN_DATA,
	.data = (uint8_t*)&data_payload,
.size = sizeof(data_payload)};



static bool appCommandInd(NWK_DataInd_t *ind)
{	
	uint32_t cmp_value = macsc_read_count();
	printf("\n%" PRIu32 "",cmp_value);
	printf("\nal");
	
}

static SYS_Timer_t	tmrInit;	
static void tmrDelayHandler(SYS_Timer_t *timer)
{
	uint32_t cmp_value = macsc_read_bts();
	printf("\n%" PRIu32 "",cmp_value);
	SYS_TimerStop(&tmrInit);
	return;
}



	static void disc_time_hndlr(void)
	{
		printf("\nDisc");
		NWK_DataReq(&msgReqDiscResponse);
		macsc_set_cmp1_int_cb(0);
		
	}
	
	static void config_time_hndlr(void)
	{
		if(1)
		{
			NWK_DataReq(&msgReqConfigStatus);
		}
		macsc_set_cmp1_int_cb(0);
		
	}
	
	
	static void online_time_hndlr(void)
	{
// 	uint32_t cmp_value = macsc_read_count();
// 	printf("\n%" PRIu32 "\n",cmp_value);
			NWK_DataReq(&msgReqData);
		//macsc_set_cmp1_int_cb(0);
	}
	
#if !APP_COORDINATOR
#endif	


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
				macsc_write_count(0);

			macsc_enable_manual_bts();


			#if APP_COORDINATOR
					
								msgReq.dstAddr				= 0;
								msgReq.dstEndpoint			= APP_COMMAND_ENDPOINT;
								msgReq.srcEndpoint			= APP_COMMAND_ENDPOINT;
								msgReq.options				= NWK_OPT_MAC_COMMAND;
								msgReq.data					= (uint8_t*)&msgConfigStatus;
								msgReq.size					= sizeof(msgConfigStatus);
								
	
			
			NWK_DataReq(&msgReq);
// 			macsc_enable_cmp_int(MACSC_CC1);
// 					macsc_set_cmp1_int_cb(online_time_hndlr);
// 
// 			macsc_use_cmp(MACSC_RELATIVE_CMP, 1000/*msg_wait_time - 150*/, MACSC_CC1);
// 				 tmrInit.interval = 1000;
// 				 tmrInit.mode = SYS_TIMER_PERIODIC_MODE;
// 				 tmrInit.handler = tmrDelayHandler;
// 				 // SYS_TimerStart(&tmrInit);
			macsc_write_count(0);

			macsc_enable_manual_bts();
			NWK_OpenEndpoint(APP_DATA_ENDPOINT, appCommandInd);

			#else
			NWK_OpenEndpoint(APP_DATA_ENDPOINT, appCommandInd);
		

			#endif
			
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
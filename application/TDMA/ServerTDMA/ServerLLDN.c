	/*
	 * ServerLLDN.c
	 *
	 * Created: 10/18/2019 5:15:37 PM
	 *  Author: guilh
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
	// #include "Solver.h"
	// #include "Energy.h"
	#include "platform.h"

typedef enum AppState_t {
	APP_STATE_INITIAL,
	APP_STATE_IDLE,
	APP_STATE_SEND,
	APP_STATE_DISC_INIT,
	APP_STATE_CONFIG_INIT,
} AppState_t;


#if APP_COORDINATOR
	#if (SIO2HOST_CHANNEL == SIO_USB)
		/* Only ARM */
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

#define seconds_3  3 / SYMBOL_TIME 
#define disc_mode	0b100
#define config_mode 0b110


#if (MASTER_MACSC == 1)
	#include "macsc_megarf.h"
#else
	static SYS_Timer_t				tmrBeaconInterval;			// Beacon
	static SYS_Timer_t				tmrComputeData;				// Compute data
#endif
	
// equation for tTS gives time in seconds, the division by SYMBOL_TIME changes to symbols for counter usage
static volatile AppState_t		appState					= APP_STATE_INITIAL;
static NWK_DataReq_t msgReq;
	
#if APP_COORDINATOR
	static float BeaconInterval;	
	

	static void tdma_server_beacon(void)
	{
		macsc_enable_manual_bts();
		appState = APP_STATE_SEND;
	}
#else
	static NwkFrameBeaconHeaderLLDN_t *rec_beacon;
	static NWK_DiscoverResponse_t msgDiscResponse;
	static int payloadSize;
	
	static void send_message_time(void)
	{
		appState = APP_STATE_SEND;
	}
#endif // APP_COORDINATOR

static void appSendData(void)
{
	NWK_DataReq(&msgReq);
	#if APP_COORDINATOR
		printf("\n Beacon Message Req");
	#endif
}

#if (!APP_COORDINATOR)
	static bool appBeaconInd(NWK_DataInd_t *ind)
	{
		macsc_enable_manual_bts();
		rec_beacon = (NwkFrameBeaconHeaderLLDN_t*)ind->data;
	
		if(rec_beacon->Flags.txState == disc_mode ||
			rec_beacon->Flags.txState == config_mode)
			{
				int msg_wait_time = rec_beacon->TimeSlotSize;
				macsc_use_cmp(MACSC_RELATIVE_CMP, msg_wait_time , MACSC_CC1);
				appState = (rec_beacon->Flags.txState == disc_mode) ? APP_STATE_DISC_INIT : APP_STATE_CONFIG_INIT;
			}
	}
	
	void appPrepareDiscoverResponse()
	{
		msgDiscResponse.id		= LL_DISCOVER_RESPONSE;
		msgDiscResponse.macAddr = APP_ADDR;
	}
#endif // !APP_COORDINATOR

static void appInit(void)
	{
		NWK_SetAddr(APP_ADDR);
		NWK_SetPanId(APP_PANID);
		PHY_SetChannel(APP_CHANNEL);
		PHY_SetRxState(true);
		
		#if APP_COORDINATOR
			printf("Iniciando...");
			PHY_SetTdmaMode(true);
		
			msgReq.dstAddr				= 0;
			msgReq.dstEndpoint			= APP_BEACON_ENDPOINT;
			msgReq.srcEndpoint			= APP_BEACON_ENDPOINT;
			msgReq.options				= NWK_OPT_LLDN_BEACON | NWK_OPT_DISCOVERY_STATE;
			msgReq.data					= NULL; // value for Expected Max Data Payload Size
			msgReq.size					= 0;
			tTS =  /*((p_var*sp + (m+n)*sm + macMinLIFSPeriod)/v_var)*/ 3;
			#if (MASTER_MACSC == 1)
				/*
				 * Configure interrupts callback functions
				 * overflow interrupt, compare 1,2,3 interrupts
				 */
				macsc_set_cmp1_int_cb(tdma_server_beacon);
				/*
				 * Configure MACSC to generate compare interrupts from channels 1,2,3
				 * Set compare mode to absolute, set compare value.
				 */
				macsc_enable_manual_bts();
				macsc_enable_cmp_int(MACSC_CC1);
				BeaconInterval = tTS / SYMBOL_TIME;
				macsc_use_cmp(MACSC_RELATIVE_CMP, BeaconInterval , MACSC_CC1);
			#endif
		#else
			  PHY_SetTdmaMode(false);
		
			  NWK_OpenEndpoint(APP_BEACON_ENDPOINT, appBeaconInd);
			  
			  macsc_set_cmp1_int_cb(send_message_time);
			  macsc_enable_cmp_int(MACSC_CC1);
			  
		#endif // APP_COORDENATOR

	}


	static void APP_TaskHandler(void)
	{
		switch (appState){
			case APP_STATE_INITIAL:
			{
				appInit();
				appState = APP_STATE_IDLE;
				break;
			}
			case APP_STATE_SEND:
			{
				appSendData();
				appState = APP_STATE_IDLE;
			}
			#if APP_COORDINATOR
			/* IMPLEMENT COORDINATOR STATE MACHINE */
			#else
				case APP_STATE_DISC_INIT:
				{
					if(rec_beacon->confSeqNumber == 0)
						appPrepareDiscoverResponse();
					else
					appState = APP_STATE_IDLE;
				}
			#endif
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
		PHY_SetPromiscuousMode(true);
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
		for(;;)
		{
			SYS_TaskHandler();
			APP_TaskHandler();
		}
	}
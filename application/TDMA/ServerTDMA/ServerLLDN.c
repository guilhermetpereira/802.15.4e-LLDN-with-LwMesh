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


typedef enum AppState_t {
	APP_STATE_INITIAL,
	APP_STATE_IDLE,
	APP_STATE_SEND,
	APP_STATE_ATT_PAN_STATE,
	APP_STATE_PREP_DISC_REPONSE,
	APP_STATE_CONFIG_INIT
} AppState_t;

#if (MASTER_MACSC == 1)
	#include "macsc_megarf.h"
#else
	static SYS_Timer_t				tmrBeaconInterval;			// Beacon
	static SYS_Timer_t				tmrComputeData;				// Compute data
#endif
	
// equation for tTS gives time in seconds, the division by SYMBOL_TIME changes to symbols for counter usage
static volatile AppState_t		appState					= APP_STATE_INITIAL;
static NWK_DataReq_t msgReq;

static void appSendData(void)
{
	if(msgReq.options)
	{
		printf("\nMessage Request in Queue");
		NWK_DataReq(&msgReq);
	}
}

#if APP_COORDINATOR

	typedef enum AppPanState_t {
		APP_PAN_STATE_IDLE,
		APP_PAN_STATE_DISC_INITIAL,
		APP_PAN_STATE_DISC_SECOND_BE,
		APP_PAN_STATE_DISC_PREPARE_ACK,
	} AppPanState_t;

	static float	beaconInterval;	
	static volatile AppPanState_t appPanState = APP_PAN_STATE_DISC_INITIAL;
	
	static void lldn_server_beacon(void)
	{
		macsc_enable_manual_bts();
		appState = APP_STATE_SEND;
	}

	static void appPanDiscInit(void)
	{
		/* 
		* Disable CSMA/CA
		* Disable auto ACK
		*/
		PHY_SetTdmaMode(true);
		/* Prepare Beacon Message as first beacon in discovery state */
		msgReq.dstAddr				= 0;
		msgReq.dstEndpoint			= APP_BEACON_ENDPOINT;
		msgReq.srcEndpoint			= APP_BEACON_ENDPOINT;
		msgReq.options				= NWK_OPT_LLDN_BEACON | NWK_OPT_DISCOVERY_STATE;
		msgReq.data					= NULL;
		msgReq.size					= 0;
		/* Calculates Beacon Intervals according to 802.15.4e - 2012 p. 70 */
		n = 255; // octets
		tTS =  1.5; //((p_var*sp + (m+n)*sm + macMinLIFSPeriod)/v_var); // 0.009088 seconds with n = 255 
		#if (MASTER_MACSC == 1)
			/* 
			* Beacon Interval values:
			* 2 x (0.009088 seconds) / (0.000016 seconds) = 1136 symbols
			* 1136 symbols = 0.0018176 seconds
			*/ 
			beaconInterval = numMgmtTs_Disc_Conf * (tTS) / (SYMBOL_TIME);
			/*
			* Configure interrupts callback functions
			* overflow interrupt, compare 1,2,3 interrupts
			*/
			macsc_set_cmp1_int_cb(lldn_server_beacon);
			/*
			* Configure MACSC to generate compare interrupts from channels 1,2,3
			* Set compare mode to absolute, set compare value.
			*/
			macsc_enable_manual_bts();
			macsc_enable_cmp_int(MACSC_CC1);
			macsc_use_cmp(MACSC_RELATIVE_CMP, beaconInterval , MACSC_CC1);
		#endif
	}

#else 

	static NwkFrameBeaconHeaderLLDN_t *rec_beacon;
	static NWK_DiscoverResponse_t msgDiscResponse;
	static int payloadSize;
	
	static void send_message_timeHandler(void)
	{
		appState = APP_STATE_SEND;
	}
	static bool appBeaconInd(NWK_DataInd_t *ind)
	{
		macsc_enable_manual_bts();
		rec_beacon = (NwkFrameBeaconHeaderLLDN_t*)ind->data;
	
		if(rec_beacon->Flags.txState == disc_mode ||
			rec_beacon->Flags.txState == config_mode)
		{
			int msg_wait_time = rec_beacon->TimeSlotSize;
			macsc_use_cmp(MACSC_RELATIVE_CMP, msg_wait_time , MACSC_CC1);
			appState = (rec_beacon->Flags.txState == disc_mode) ? APP_STATE_PREP_DISC_REPONSE : APP_STATE_CONFIG_INIT;
		}
	}
	void appPrepareDiscoverResponse()
	{
		/* CONTINUAR IMPLEMENTAÇÃO */
		msgDiscResponse.id					= LL_DISCOVER_RESPONSE;
		msgDiscResponse.macAddr				= APP_ADDR;
		msgDiscResponse.ts_dir.tsDuration	= payloadSize;
		msgDiscResponse.ts_dir.dirIndicator = 1;
		
		msgReq.dstAddr				= 0;
		msgReq.dstEndpoint			= APP_COMMAND_ENDPOINT;
		msgReq.srcEndpoint			= APP_COMMAND_ENDPOINT;
		msgReq.options				= NWK_OPT_MAC_COMMAND;
		msgReq.data					= (uint8_t*)&msgDiscResponse;
		msgReq.size					= sizeof(msgDiscResponse);
	}
#endif // APP_COORDINATOR

static void appInit(void)
{
	printf("\nIniciando....");
	NWK_SetAddr(APP_ADDR);
	NWK_SetPanId(APP_PANID);
	PHY_SetChannel(APP_CHANNEL);
	PHY_SetRxState(true);
		
	#if APP_COORDINATOR
	#else
			PHY_SetTdmaMode(false);
			payloadSize = 1;
			NWK_OpenEndpoint(APP_BEACON_ENDPOINT, appBeaconInd);
			/*
			* Configure interrupts callback functions
			*/
			macsc_set_cmp1_int_cb(send_message_timeHandler);
			macsc_enable_cmp_int(MACSC_CC1);	  
	#endif // APP_COORDENATOR

}

	static void APP_TaskHandler(void)
	{
		switch (appState){
			case APP_STATE_INITIAL:
			{
				appInit();
				#if APP_COORDINATOR
					appState = APP_STATE_ATT_PAN_STATE;
				#else
					appState = APP_STATE_IDLE;
				#endif
				break;
			}
			case APP_STATE_SEND:
			{
				appSendData();
				#if APP_COORDINATOR
					appState = APP_STATE_ATT_PAN_STATE;
				#else
					appState = APP_STATE_IDLE;
				#endif
				break;
			}
			#if APP_COORDINATOR // COORDINATOR SPECIFIC STATE MACHINE
			case APP_STATE_ATT_PAN_STATE:
			{
				switch(appPanState){
					case APP_PAN_STATE_DISC_INITIAL:
					{
						appPanDiscInit();
						appState	= APP_STATE_IDLE;
						appPanState = APP_PAN_STATE_DISC_SECOND_BE;
						break;
					}
					case APP_PAN_STATE_DISC_SECOND_BE:
					{
						msgReq.options = NWK_OPT_LLDN_BEACON | NWK_OPT_DISCOVERY_STATE | NWK_OPT_SECOND_BEACON ;
						appState	= APP_STATE_IDLE;
						appPanState = APP_PAN_STATE_DISC_PREPARE_ACK;
						break;
					}
					case APP_PAN_STATE_DISC_PREPARE_ACK:
					{
						/* CONTINUAR APÓS IMPLEMENTAR NODE DISCOVERY RESPONSE ANTES*/
						msgReq.options = 0;
						appState	= APP_STATE_IDLE;
						appPanState = APP_PAN_STATE_IDLE;
						break;
					}
					default:
					{
						appState = APP_STATE_IDLE;
					}
					break;
				}
				break;	
			}
			#else // NODES SPECIFIC STATE MACHINE
				case APP_STATE_PREP_DISC_REPONSE:
				{
					if(rec_beacon->confSeqNumber == 0)
						appPrepareDiscoverResponse();
					else
						appState = APP_STATE_IDLE;
					break;
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
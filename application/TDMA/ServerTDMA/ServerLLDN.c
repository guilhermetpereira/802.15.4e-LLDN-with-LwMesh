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
		#define TIMESLOT_TIMER	1
	#endif
#else
	/* Only megarf series */
	#include "conf_sio2host.h" // necessary for prints
	#define MASTER_MACSC		1
#endif


#if (MASTER_MACSC == 1)
	#include "macsc_megarf.h"
#else
	// static SYS_Timer_t				tmrBeaconInterval;			// Beacon
	// static SYS_Timer_t				tmrComputeData;				// Compute data
#endif
	
// equation for tTS gives time in seconds, the division by SYMBOL_TIME changes to symbols for counter usage
static volatile AppState_t	appState = APP_STATE_INITIAL;
static NWK_DataReq_t msgReq;
static uint8_t PanId;

static void appSendData(void)
{
	if(msgReq.options)
	{		
		NWK_DataReq(&msgReq);
		#if APP_COORDINATOR
		#endif
		printf("\noption = %d", msgReq.options);
	}
}

#if APP_COORDINATOR
	static float beaconInterval;	
	static volatile AppPanState_t appPanState = APP_PAN_STATE_RESET;
	static NWK_ACKFormat_t ACKFrame;
	static int ACKFrame_size = 0;
	static uint8_t cycles_counter = macLLDNdiscoveryModeTimeout;
	
	static SYS_Timer_t tmrDiscoveryMode;

	static void tmrDiscoveryModeHandler(SYS_Timer_t *timer)
	{
		appState = APP_STATE_INITIAL;
		appPanState = APP_PAN_STATE_RESET;
	}
	
	static void lldn_server_beacon(void)
	{
		macsc_enable_manual_bts();
		appState = APP_STATE_SEND;
	}
	
	static void downlink_delay_handler(void)
	{
		macsc_disable_cmp_int(MACSC_CC3);
		appState = APP_STATE_SEND;
	}
	
	#if TIMESLOT_TIMER
	static void teste_handler(void)
	{
		if(msgReq.options)
		printf("\n***TIMESLOT****");
	}
	#endif
	
	static void addToAckArray(uint16_t addres)
	{	
		int pos =(int) addres / 8;
		int bit_shift = 8 - (addres % 8);
		ACKFrame.ackFlags[pos] |= 1 << bit_shift;
		if (pos + 1 > ACKFrame_size)
		ACKFrame_size = pos + 1;
	}
	
	static bool appCommandInd(NWK_DataInd_t *ind)
	{
		if(ind->data[0] == LL_DISCOVER_RESPONSE)
		{
			NWK_DiscoverResponse_t *msg = (NWK_DiscoverResponse_t*)ind->data;
			addToAckArray(msg->macAddr);
			printf("\nDiscover Response %hhx", msg->macAddr);	
		}
		else if(ind->data[0] == LL_CONFIGURATION_STATUS)
		{
			NWK_ConfigStatus_t *msg = (NWK_ConfigStatus_t*)ind->data;
			printf("\nConfiguration Status %hhx", msg->macAddr);	
		}
		else return false;			
		return true;
	}
	
	static void appPanPrepareACK(void)
	{
		msgReq.dstAddr				= 0;
		msgReq.dstEndpoint			= APP_BEACON_ENDPOINT;
		msgReq.srcEndpoint			= APP_BEACON_ENDPOINT;
		msgReq.options				= NWK_OPT_LLDN_ACK;
		msgReq.data					= (uint8_t *)&ACKFrame;
		msgReq.size					= sizeof(uint8_t)*(ACKFrame_size + 1);
	}

	static void appPanDiscInit(void)
	{		
		/* Prepare Beacon Message as first beacon in discovery state */		
		msgReq.dstAddr				= 0;
		msgReq.dstEndpoint			= APP_BEACON_ENDPOINT;
		msgReq.srcEndpoint			= APP_BEACON_ENDPOINT;
		msgReq.options				= NWK_OPT_LLDN_BEACON | NWK_OPT_DISCOVERY_STATE;
		msgReq.data					= NULL;
		msgReq.size					= 0;
		
		
		
		/* Calculates Beacon Intervals according to 802.15.4e - 2012 p. 70 */
		n = 127; // 180 -safe octets
		tTS =  ((p_var*sp + (m+n)*sm + macMinLIFSPeriod)/v_var); // 0.009088 seconds with n = 255 
		#if (MASTER_MACSC == 1)
		
			beaconInterval = 2 * numBaseTimeSlotperMgmt * (tTS) / (SYMBOL_TIME);
			/*
			* Configure interrupts callback functions
			* overflow interrupt, compare 1,2,3 interrupts
			*/
			macsc_set_cmp1_int_cb(lldn_server_beacon);
			macsc_set_cmp2_int_cb(downlink_delay_handler);

			/*
			* Configure MACSC to generate compare interrupts from channels 1,2,3
			* Set compare mode to absolute, set compare value.
			*/
			macsc_enable_manual_bts();
			macsc_enable_cmp_int(MACSC_CC1);
			
			macsc_use_cmp(MACSC_RELATIVE_CMP, beaconInterval , MACSC_CC1);
			macsc_use_cmp(MACSC_RELATIVE_CMP, DELAY , MACSC_CC3);
			
			#if TIMESLOT_TIMER
			macsc_set_cmp2_int_cb(teste_handler);	
			macsc_enable_cmp_int(MACSC_CC2);
			macsc_use_cmp(MACSC_RELATIVE_CMP, beaconInterval / 2, MACSC_CC2);
			#endif
			
		#endif
	}

#else 
	static NwkFrameBeaconHeaderLLDN_t *rec_beacon;
	static NWK_ConfigStatus_t msgConfigStatus;
	static NWK_DiscoverResponse_t msgDiscResponse;

	static uint8_t payloadSize = 0x01;
	static uint8_t assTimeSlot = 0xFF;
	static bool ack_received;
	
	static void send_message_timeHandler(void)
	{
		appState = APP_STATE_SEND;	
	}
	
	static bool appBeaconInd(NWK_DataInd_t *ind)
	{
		macsc_enable_manual_bts();	
		rec_beacon = (NwkFrameBeaconHeaderLLDN_t*)ind->data;
		PanId = rec_beacon->PanId;
		if( (rec_beacon->Flags.txState == DISC_MODE && !ack_received) ||
			rec_beacon->Flags.txState == CONFIG_MODE && ack_received)
		{
			int msg_wait_time = rec_beacon->Flags.numBaseMgmtTimeslots * rec_beacon->TimeSlotSize* 2 ; // symbols 190 is a delay adjustment
			macsc_set_cmp1_int_cb(send_message_timeHandler);
			macsc_enable_cmp_int(MACSC_CC1);	  
			macsc_use_cmp(MACSC_RELATIVE_CMP, msg_wait_time , MACSC_CC1);
			appState = (rec_beacon->Flags.txState == DISC_MODE) ? APP_STATE_PREP_DISC_REPONSE : APP_STATE_PREP_CONFIG_STATUS;
			/*
			macsc_set_cmp2_int_cb(end_cap);	
			macsc_enable_cmp_int(MACSC_CC2);
			macsc_use_cmp(MACSC_RELATIVE_CMP, msg_wait_time + rec_beacon->TimeSlotSize * 2, MACSC_CC2);
			*/
		}
		else if (rec_beacon->Flags.txState == DISC_MODE)
		{
			ack_received = 0;
		}
			
		return true;
	}
	
	static bool appAckInd(NWK_DataInd_t *ind)
	{
		NWK_ACKFormat_t *ackframe = (NWK_ACKFormat_t*)ind->data;
		if(PanId == ackframe->sourceId)
		{
			int pos = APP_ADDR / 8;
			int bit_shift = 8 - APP_ADDR % 8;
			if( ackframe->ackFlags[pos] & 1 << bit_shift)	
				{
				printf("\n ack true");
				ack_received = true;
				}
		}
		return true;
	}
	void appPrepareDiscoverResponse()
	{
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
	NWK_SetAddr(APP_ADDR);
	PHY_SetChannel(APP_CHANNEL);
	PHY_SetRxState(true);
		
	#if APP_COORDINATOR
	  tmrDiscoveryMode.interval = 2500;
	  tmrDiscoveryMode.mode = SYS_TIMER_PERIODIC_MODE;
	  tmrDiscoveryMode.handler = tmrDiscoveryModeHandler;
	  SYS_TimerStart(&tmrDiscoveryMode);
		printf("\n---------\n");
		/* 
		* Disable CSMA/CA
		* Disable auto ACK
		*/
		NWK_SetPanId(APP_PANID);
		PanId = APP_PANID;
		ACKFrame.sourceId = APP_PANID;
		PHY_SetTdmaMode(true);
		NWK_OpenEndpoint(APP_COMMAND_ENDPOINT, appCommandInd);
	#else
		PHY_SetTdmaMode(false);
		PHY_SetOptimizedCSMAValues();
		
		payloadSize = 0x01;
		NWK_OpenEndpoint(APP_BEACON_ENDPOINT, appBeaconInd);
		NWK_OpenEndpoint(APP_ACK_ENDPOINT, appAckInd);
		/*
		* Configure interrupts callback functions
		*/
		
	#endif // APP_COORDENATOR
	PHY_SetPromiscuousMode(true);

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
			switch(appPanState)
			{
				case APP_PAN_STATE_RESET:
				{
					msgReq.options = NWK_OPT_LLDN_BEACON | NWK_OPT_RESET_STATE;
					appPanState = APP_PAN_STATE_DISC_INITIAL;
					appState	= APP_STATE_IDLE;
				}
				case APP_PAN_STATE_DISC_INITIAL:
				{
					printf("\n\nCICLO %d \n", cycles_counter); 
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
					macsc_enable_cmp_int(MACSC_CC3);
					appPanPrepareACK();
					if(cycles_counter == 0)
					{
						appPanState = APP_PAN_STATE_IDLE;
						cycles_counter = macLLDNdiscoveryModeTimeout;
					}
					else
					{
						appPanState = APP_PAN_STATE_DISC_INITIAL;
						cycles_counter--;
					}
					 appState = APP_STATE_IDLE;
					
					//printf("\nack");
					break;
				}
				case APP_PAN_STATE_IDLE:
				{
					msgReq.options = 0;
					appState = APP_STATE_IDLE;
					break;
				}
			}
			break;	
		}
		#else // NODES SPECIFIC STATE MACHINE
		case APP_STATE_PREP_DISC_REPONSE:
		{
			if(rec_beacon->confSeqNumber == 0)
			{
				appPrepareDiscoverResponse();
			}
			else msgReq.options = 0;
			appState = APP_STATE_IDLE;	
			break;
		}
		/*
		case APP_STATE_PREP_CONFIG_STATUS:
		{
			if(ack_received)
				appPrepareDiscoverResponse();
			appState = APP_STATE_IDLE;
			break;
		}*/
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
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
		#define TIMESLOT_TIMER	0
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
AppState_t	appState = APP_STATE_INITIAL;
static NWK_DataReq_t msgReq;
static uint8_t PanId;

static void appSendData(void)
{
	if(msgReq.options != 0)
	{
		printf("\nMSG REQ SENT %d",msgReq.options);
		NWK_DataReq(&msgReq);
	#if !APP_COORDINATOR
	#endif
	}
}

#if APP_COORDINATOR

	#define NODOS_ASSOCIADOS_ESPERADOS 3

	float beaconInterval;
	AppPanState_t appPanState = APP_PAN_STATE_RESET;		// Initial state of PAN node
	NWK_ACKFormat_t ACKFrame;								// ACK Frame Payload used in Discovery State
	int ACKFrame_size = 0;									// Bitmap size 
	uint8_t cycles_counter = macLLDNdiscoveryModeTimeout;
	int counter_associados = 0;								// Associated nodes counter
	NWK_ConfigRequest_t msgsConfRequest[254];				// Array for Configure Request messages, one position per node, 254 limited by star topology
	NWK_ConfigRequest_t ConfigRequest;
	int macLLDNnumUplinkTS = 0;								// Number of uplink timeslots, is also the control of associated nodes, further implementations must be done
	int biggest_timeslot_duration  = 0;						
	int index_ConfRequest = 0;
	SYS_Timer_t tmrDelay_Discovery;							// Timer for delay between messages
	SYS_Timer_t tmrDelay_Configuration;						// Timer for delay between messages
	

	
	static void tmrDelayHandler(SYS_Timer_t *timer)
	{
		printf("\nTimer_Software");
		appState = APP_STATE_SEND;
	}
	
	static void lldn_server_beacon(void)
	{
		printf("\nBeacon Timer");
		macsc_enable_manual_bts();
		appState = APP_STATE_SEND;
	}
	
	static void downlink_delay_handler(void)
	{
		printf("\ndelay timer");
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
	
	static void addConfRequestArray(NWK_ConfigStatus_t *node)
	{
		index_ConfRequest++;
		if(node->ts_dir.tsDuration > biggest_timeslot_duration)
			biggest_timeslot_duration = node->ts_dir.tsDuration;
		// tem que atualizar o tamanho final
		msgsConfRequest[index_ConfRequest].id = LL_CONFIGURATION_REQUEST;
 		msgsConfRequest[index_ConfRequest].macAddr = node->macAddr;
 		msgsConfRequest[index_ConfRequest].s_macAddr = APP_ADDR;
 		msgsConfRequest[index_ConfRequest].tx_channel = APP_CHANNEL;
		// PRECISA MUDAR O ASSTIMESLOT, VERIFIACR O NOME APROPRIADO NA NORMA E A IMPLEMENTAÇÃO TAMBÉM ESTÁ ERRADA
 		msgsConfRequest[index_ConfRequest].assTimeSlot = (uint8_t)index_ConfRequest;
 		msgsConfRequest[index_ConfRequest].conf.macLLDNmgmtTS = MacLLDNMgmtTS;
	}
		
	static void CopyToConfigRequest(int i)
	{
		ConfigRequest.id = msgsConfRequest[i].id;
		ConfigRequest.s_macAddr = msgsConfRequest[i].s_macAddr;
		ConfigRequest.tx_channel = msgsConfRequest[i].tx_channel;
		ConfigRequest.assTimeSlot = msgsConfRequest[i].assTimeSlot;
		ConfigRequest.macAddr = msgsConfRequest[i].macAddr;
		ConfigRequest.conf.tsDuration = msgsConfRequest[i].conf.tsDuration;
		ConfigRequest.conf.mgmtFrames = msgsConfRequest[i].conf.mgmtFrames;
	}
	
	static bool appCommandInd(NWK_DataInd_t *ind)
	{
		if(ind->data[0] == LL_DISCOVER_RESPONSE)
		{
			NWK_DiscoverResponse_t *msg = (NWK_DiscoverResponse_t*)ind->data;
			addToAckArray(msg->macAddr);
			printf("\nDISCOVER STATUS %hhx", msg->macAddr);	
		}
		else if(ind->data[0] == LL_CONFIGURATION_STATUS)
		{
			NWK_ConfigStatus_t *msg = (NWK_ConfigStatus_t*)ind->data;
			addConfRequestArray(msg);
			printf("\nCONFIGURATION STATUS %hhx", msg->macAddr);	
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

	static void appPanReset(void)
	{
		// prepare beacon reset message
		msgReq.dstAddr		= 0;
		msgReq.dstEndpoint	= APP_BEACON_ENDPOINT;
		msgReq.srcEndpoint	= APP_BEACON_ENDPOINT;
		msgReq.options		= NWK_OPT_LLDN_BEACON | NWK_OPT_RESET_STATE;
		msgReq.data			= NULL;
		msgReq.size			= 0;

		ACKFrame_size = 0;
		index_ConfRequest = 0;
		counter_associados = 0;
		biggest_timeslot_duration = 0;
	}

	static void appPanDiscInit(void)
	{	
		/* clear Ack array of previous discovery state */
		for(int i = 0; i < 32; i++)
			ACKFrame.ackFlags[i] = 0;
		
		/* Prepare Beacon Message as first beacon in discovery state */		
		msgReq.dstAddr				= 0;
		msgReq.dstEndpoint			= APP_BEACON_ENDPOINT;
		msgReq.srcEndpoint			= APP_BEACON_ENDPOINT;
		msgReq.options				= NWK_OPT_LLDN_BEACON | NWK_OPT_DISCOVERY_STATE;
		msgReq.data					= NULL;
		msgReq.size					= 0;
		
		
		/* Only start timers if it is the first association process */
		if(cycles_counter == 0) 
		{
			
		/* Calculates Beacon Intervals according to 802.15.4e - 2012 p. 70 */
		n = 127; // 180 -safe octets
		tTS =  ((p_var*sp + (m+n)*sm + macMinLIFSPeriod)/v_var);
		#if (MASTER_MACSC == 1)
		
			beaconInterval = 2 * numBaseTimeSlotperMgmt * (tTS) / (SYMBOL_TIME);
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
			//macsc_enable_cmp_int(MACSC_CC3);

			
			macsc_use_cmp(MACSC_RELATIVE_CMP, beaconInterval , MACSC_CC1);
			macsc_use_cmp(MACSC_RELATIVE_CMP,DELAY , MACSC_CC3);
			
			/* Timer used in testing */
			#if TIMESLOT_TIMER
			macsc_set_cmp2_int_cb(teste_handler);	
			macsc_enable_cmp_int(MACSC_CC2);
			macsc_use_cmp(MACSC_RELATIVE_CMP, beaconInterval / 2, MACSC_CC2);
			#endif
			
		#endif
		}
	}

#else 
	static NwkFrameBeaconHeaderLLDN_t *rec_beacon;
	static NWK_ConfigStatus_t msgConfigStatus;
	static NWK_DiscoverResponse_t msgDiscResponse;

	static uint8_t payloadSize = 0x01;
	static uint8_t assTimeSlot = 0xFF;
	static bool ack_received;
	bool associated = 0;
	
	static void send_message_timeHandler(void)
	{
		appState = APP_STATE_SEND;	
	}
	
	static void end_cap(void)
	{
		PHY_ResetRadio();
	}
	
	static bool appBeaconInd(NWK_DataInd_t *ind)
	{
		macsc_enable_manual_bts();	
		rec_beacon = (NwkFrameBeaconHeaderLLDN_t*)ind->data;
		PanId = rec_beacon->PanId; // só pode mudar se ele associar
			// mudar nome ack_Received
		if( (rec_beacon->Flags.txState == DISC_MODE && !ack_received) || 
			(rec_beacon->Flags.txState == CONFIG_MODE && ack_received))
		{
			int msg_wait_time = rec_beacon->Flags.numBaseMgmtTimeslots * rec_beacon->TimeSlotSize* 2 ; // symbols 190 is a delay adjustment
			macsc_set_cmp1_int_cb(send_message_timeHandler);
			
			macsc_enable_cmp_int(MACSC_CC1);	  
			macsc_use_cmp(MACSC_RELATIVE_CMP, msg_wait_time , MACSC_CC1);
			appState = (rec_beacon->Flags.txState == DISC_MODE) ? APP_STATE_PREP_DISC_REPONSE : APP_STATE_PREP_CONFIG_STATUS;
			/*
			macsc_set_cmp2_int_cb(end_cap);	
			macsc_enable_cmp_int(MACSC_CC2);
			macsc_use_cmp(MACSC_RELATIVE_CMP, 2 * msg_wait_time, MACSC_CC2);
			*/
		}
		else if (rec_beacon->Flags.txState == RESET_MODE)
		{
			
			ack_received = 0;
			associated = 0;
			printf("ack = %d" ,ack_received);
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
	
	static bool appCommandInd(NWK_DataInd_t *ind)
	{
		if(ind->data[0] == LL_CONFIGURATION_REQUEST)
		{
			NWK_ConfigRequest_t *msg = (NWK_ConfigRequest_t*)ind->data;
			if(msg->macAddr == APP_ADDR)
			{
				associated = 1;
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
	
	void appPrepareConfigurationStatus()
	{
		msgConfigStatus.id					  = LL_CONFIGURATION_STATUS;
		msgConfigStatus.macAddr				  = APP_ADDR;
		msgConfigStatus.s_macAddr			  = APP_ADDR;
		msgConfigStatus.ts_dir.tsDuration	  = payloadSize;
		msgConfigStatus.ts_dir.dirIndicator   = 1;
		// assTimeSlot precisa ser variado quando for implementar
		// no caso inicial a rede não está em andamento ainda portanto faz
		// sentido nenhum nodo ter timeslot assigned
		msgConfigStatus.assTimeSlot = 0;

		
		msgReq.dstAddr				= 0;
		msgReq.dstEndpoint			= APP_COMMAND_ENDPOINT;
		msgReq.srcEndpoint			= APP_COMMAND_ENDPOINT;
		msgReq.options				= NWK_OPT_MAC_COMMAND;
		msgReq.data					= (uint8_t*)&msgConfigStatus;
		msgReq.size					= sizeof(msgConfigStatus);
	}
	
#endif // APP_COORDINATOR

static void appInit(void)
{
	NWK_SetAddr(APP_ADDR);
	PHY_SetChannel(APP_CHANNEL);
	PHY_SetRxState(true);
		
	#if APP_COORDINATOR
	  /* Timer used for delay between messages */
	  tmrDelay_Discovery.interval = 2;
	  tmrDelay_Discovery.mode = SYS_TIMER_INTERVAL_MODE;
	  tmrDelay_Discovery.handler = tmrDelayHandler;
	  
	  /* Timer used for delay between messages */
	  tmrDelay_Configuration.interval = 3;
	  tmrDelay_Configuration.mode = SYS_TIMER_INTERVAL_MODE;
	  tmrDelay_Configuration.handler = tmrDelayHandler;
	  
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
		/*
		 * Enable CSMA/CA
		 * Enable Random CSMA seed generator
		 */
		PHY_SetTdmaMode(false);
		PHY_SetOptimizedCSMAValues();
		
		payloadSize = 0x01;
		NWK_OpenEndpoint(APP_BEACON_ENDPOINT, appBeaconInd);
		NWK_OpenEndpoint(APP_ACK_ENDPOINT, appAckInd);
		NWK_OpenEndpoint(APP_COMMAND_ENDPOINT, appCommandInd);
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
				/* Every time a message is send updates coordinator to prepare next message */
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
				/* Prepare beacon to desassociate all nodes */
				case APP_PAN_STATE_RESET:
				{
					appPanReset();
					appPanState = APP_PAN_STATE_DISC_INITIAL;
					appState	= APP_STATE_SEND;
					cycles_counter = 0;
					break;
				}
				/* Prepare first Beacon of Discovery */
				case APP_PAN_STATE_DISC_INITIAL:
				{
					printf("\n\n");
					index_ConfRequest = 0;
					/* if nodes associated is equal to expected number of associated nodes stop association process 
					 * this implementation was done as is to be used in tests, for real network functionality 
					 * the number of max association processes must be done through macLLDNdiscoveryModeTimeout
					 */
					if(counter_associados == NODOS_ASSOCIADOS_ESPERADOS || cycles_counter > NODOS_ASSOCIADOS_ESPERADOS)
					{	
						printf("\n%d", cycles_counter);
						counter_associados = 0;
						/* if all nodes expected where associated stop beacon generation interruptions */
						macsc_disable_cmp_int(MACSC_CC1);
						macsc_disable_cmp_int(MACSC_CC2);
						macsc_disable_cmp_int(MACSC_CC3);
						msgReq.options = 0;
						/* set coordinator node to idle further implementation of online state must be done */
						appState = APP_STATE_IDLE;
						appPanState = APP_PAN_STATE_IDLE;
						
					}
					/* if not all nodes expected where associated run through association process again */
					else 
					{
						/* prepare beacon message and start timers for beacon */
						appPanDiscInit();
						appState	= APP_STATE_IDLE;
						appPanState = APP_PAN_STATE_DISC_SECOND_BE;
					}
					break;
				}
				case APP_PAN_STATE_DISC_SECOND_BE:
				{
					/* Prepares message as: Discovery Beacon and Second Beacon */
					msgReq.options = NWK_OPT_LLDN_BEACON | NWK_OPT_DISCOVERY_STATE | NWK_OPT_SECOND_BEACON ;
					appState	= APP_STATE_IDLE;
					appPanState = APP_PAN_STATE_DISC_PREPARE_ACK;
					break;
				}
				case APP_PAN_STATE_DISC_PREPARE_ACK:
				{
					/* This timer implements a delay between messages, 
					 * if not used the nodes are not able to receive the message
					 */
					SYS_TimerStart(&tmrDelay_Discovery);
					appPanPrepareACK();
					appPanState = APP_PAN_STATE_CONFIG_INITIAL; 
					appState = APP_STATE_IDLE;
					break;
				}
				case APP_PAN_STATE_CONFIG_INITIAL:
				{
					/* Prepares the message as: Configuration Beacon and First State Beacon */
					msgReq.options = NWK_OPT_LLDN_BEACON | NWK_OPT_CONFIG_STATE;
					appState	= APP_STATE_IDLE;
					appPanState = APP_PAN_STATE_CONFIG_SECOND_BEACON;
					break;

				}
				case APP_PAN_STATE_CONFIG_SECOND_BEACON:
				{
					/* Prepares the message as: Configuration Beacon and Second State Beacon */
					msgReq.options = NWK_OPT_LLDN_BEACON | NWK_OPT_CONFIG_STATE | NWK_OPT_SECOND_BEACON;
					appState	= APP_STATE_IDLE;
					appPanState = APP_PAN_STATE_SEND_CONF_REQUEST;
					break;
				}
				case APP_PAN_STATE_SEND_CONF_REQUEST:
				{
					/* Send Configuration Requests frames 
					 * PRECISA SER MUDADO, NA IMPLEMENTAÇÃO ATUAL A LOCAÇÃO DE SLOTS NÃO ESTÁ CORRETA
					 */
					if(index_ConfRequest > 0)
					{
						CopyToConfigRequest(index_ConfRequest);
						msgReq.options		= NWK_OPT_MAC_COMMAND;
						msgReq.data			= (uint8_t*)&ConfigRequest;
						msgReq.size			= sizeof(NWK_ConfigRequest_t);
						appState	= APP_STATE_IDLE;
						appPanState = APP_PAN_STATE_SEND_CONF_REQUEST;
						index_ConfRequest--;
						counter_associados++;
						SYS_TimerStart(&tmrDelay_Configuration);
					}
					else
					{
						appPanState = APP_PAN_STATE_CONFIG_THIRD_BEACON;
					}
					break;
				}
				case APP_PAN_STATE_CONFIG_THIRD_BEACON:
				{
					printf("\nTHIRD BEACON");
					msgReq.options = NWK_OPT_LLDN_BEACON | NWK_OPT_CONFIG_STATE | NWK_OPT_THIRD_BEACON;
					appState	= APP_STATE_IDLE;
					appPanState = APP_PAN_STATE_DISC_INITIAL;
					cycles_counter++;	
					
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
			// desativar timer espera timeslot
			if(rec_beacon->confSeqNumber == 0)
			{
				appPrepareDiscoverResponse();
			}
			else msgReq.options = 0;
			appState = APP_STATE_IDLE;	
			break;
		}
		
		case APP_STATE_PREP_CONFIG_STATUS:
		{
			// se o nodo recebeu ack na fase do discovery prepara a mensagem de configuration status
			if(ack_received && rec_beacon->confSeqNumber == 0 && associated == 0) {
				appPrepareConfigurationStatus();
			}
			// se o nodo não recebeu desativa o timer e fica em idle
			else {
				macsc_disable_cmp_int(MACSC_CC1);
			}
			appState = APP_STATE_IDLE;
			break;
		}
		#endif
		default:
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
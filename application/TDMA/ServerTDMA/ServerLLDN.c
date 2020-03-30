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

 void appSendData(void)
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

	#define NODOS_ASSOCIADOS_ESPERADOS 12
	
	AppPanState_t appPanState = APP_PAN_STATE_RESET; // Initial state of PAN node
	
	/* Configuration Request Frames */
	/* Da pra mudar o envio do confrequest pra ja usar essa array com as informaçõs dos nodos */	
	nodes_info_t nodes_info_arr[50]; // Array for Configure Request messages, one position per node, 254 limited by star topology
	NWK_ConfigRequest_t config_request_frame = { .id = LL_CONFIGURATION_REQUEST,
												 .s_macAddr = APP_ADDR,
												 .tx_channel = APP_CHANNEL,
												 .conf.macLLDNmgmtTS = MacLLDNMgmtTS };
	nodes_info_list_t *conf_req_list = NULL;

	/* Acknowledge Frame and Array */
	NWK_ACKFormat_t ACKFrame;	// ACK Frame Payload used in Discovery State
	int ACKFrame_size = 0;

	float beaconInterval = 0; // não precisa ser global
	
	int macLLDNnumUplinkTS = 0;		// Number of uplink timeslots, is also the control of associated nodes, further implementations must be done
	
	/* This timer implements a delay between messages, 
	 * if not used the nodes are not able to receive the message
	 */
	static SYS_Timer_t tmrDelay;	
	
	/*  Control variables for testing */	
	int assTimeSlot = 0;
	uint8_t timeslot_counter = 0;

	int counter_associados = 0;		// Associated nodes counter
	uint8_t cycles_counter = macLLDNdiscoveryModeTimeout;

	/* data related variables */
	msg_info_t msg_info_array[50]; // size of array limited by hardware
	unsigned int size_msg_info = 0;
	bool data_received = false;
	float succes_rate = 0;
	
	static void tmrDelayHandler(SYS_Timer_t *timer)
	{
		appState = APP_STATE_SEND;
	}
	
	static void lldn_server_beacon(void)
	{
		macsc_enable_manual_bts();
		appState = APP_STATE_SEND;
	}
	
	static void time_slot_handler(void)
	{
		if (timeslot_counter > 0)
			macsc_use_cmp(MACSC_RELATIVE_CMP, tTS / (SYMBOL_TIME), MACSC_CC1);
		
		macsc_enable_manual_bts();
		appState = APP_STATE_ATT_PAN_STATE;
	}
	
	static void downlink_delay_handler(void)
	{
		if(msgReq.options == NWK_OPT_LLDN_ACK)
		{
		appState = APP_STATE_SEND;
		}
	}

	static void end_of_online_handler(void)
	{
		appState = APP_STATE_ATT_PAN_STATE;
		appPanState = APP_PAN_STATE_ONLINE_END_BE;
	}
	
	#if TIMESLOT_TIMER
	static void teste_handler(void)
	{
		if(msgReq.options)
			printf("\n***TIMESLOT****");
		macsc_disable_cmp_int(MACSC_CC3);
	}
	#endif
	
	static bool addToAckArray(uint8_t addres)
	{	
		int pos =(int) addres / 8;
		int bit_shift = 8 - (addres % 8);
		
		if(ACKFrame.ackFlags[pos] & 1 << bit_shift)
		{
			printf("\nAddr rep %d", addres);
			return false;
		}
		ACKFrame.ackFlags[pos] = 1 << bit_shift;
		if (pos + 1 > ACKFrame_size)
			ACKFrame_size = pos + 1;
		
		return true;
	}

	static void addConfRequestArray(NWK_ConfigStatus_t *node)
	{

		uint8_t i;
		for (i= 0;i < 256 && nodes_info_arr[i].mac_addr != 0; i++);
		
		assTimeSlot++;
	
		if(node->ts_dir.tsDuration > config_request_frame.conf.tsDuration)
			config_request_frame.conf.tsDuration =  node->ts_dir.tsDuration;
		
		nodes_info_arr[i].req_timeslot_duration = node->ts_dir.tsDuration;
		nodes_info_arr[i].mac_addr = node->macAddr;
		nodes_info_arr[i].assigned_time_slot = (uint8_t)i;
		
		if(conf_req_list != NULL)
		{
			nodes_info_list_t *tmp = (nodes_info_list_t*)malloc(sizeof(nodes_info_list_t));
			tmp->node = &nodes_info_arr[i];
			tmp->next = conf_req_list;
			conf_req_list = tmp;
		}
		else
		{
			conf_req_list = (nodes_info_list_t*)malloc(sizeof(nodes_info_list_t));
			conf_req_list->node = &nodes_info_arr[i];
			conf_req_list->next = NULL;
		}
	}

	static void CopyToConfigRequest()
	{
		config_request_frame.assTimeSlot = conf_req_list->node->assigned_time_slot;
		config_request_frame.macAddr = conf_req_list->node->mac_addr;
		nodes_info_list_t *tmp = conf_req_list;
		conf_req_list = conf_req_list->next;
		tmp->node = NULL;
		tmp->next = NULL;
		free(tmp);
	}


	static bool appCommandInd(NWK_DataInd_t *ind)
	{
		if(ind->data[0] == LL_DISCOVER_RESPONSE)
		{
			NWK_DiscoverResponse_t *msg = (NWK_DiscoverResponse_t*)ind->data;
			addToAckArray(msg->macAddr);	
					
			#if PRINT
			printf("\nDISC %hhx", msg->macAddr);	
			#endif
		}
		else if(ind->data[0] == LL_CONFIGURATION_STATUS)
		{
			NWK_ConfigStatus_t *msg = (NWK_ConfigStatus_t*)ind->data;
			addConfRequestArray(msg);
			#if PRINT
			printf("\nCONF %d", msg->macAddr);	
			#endif
		}
		else return false;			
		return true;
	}
	
	static bool appDataInd(NWK_DataInd_t *ind)
	{
		
		int curr_ts = timeslot_counter - 2*MacLLDNMgmtTS;
		
		nodes_info_arr[curr_ts].rssi = ind->rssi;
		nodes_info_arr[curr_ts].msg_rec++;
		
		printf("\n %d payload: ", curr_ts);
		for (int i = 0; i < ind->size; i++)
			printf("%hhx", ind->data[i]);
			
	}
	
	static void appPanPrepareACK(void)
	{
		msgReq.dstAddr		= 0;
		msgReq.dstEndpoint	= APP_BEACON_ENDPOINT;
		msgReq.srcEndpoint	= APP_BEACON_ENDPOINT;
		msgReq.options		= NWK_OPT_LLDN_ACK;
		msgReq.data	= (uint8_t *)&ACKFrame;
		msgReq.size	= sizeof(uint8_t)*(ACKFrame_size + 1);

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

		for(int i = 0; i < 32; i++)
			ACKFrame.ackFlags[i] = 0;
			
		for (int i = 0; i < 50; i++)
		{
			nodes_info_arr[i].mac_addr = 0;
			nodes_info_arr[i].msg_rec = 0;

			
			msg_info_array[i].mac_addr = 0;
			msg_info_array[i].coop_addr = 0;
			for (int j = 0; j < 50; j++)
				nodes_info_arr[i].neighbors[j] = 0;
		}
		assTimeSlot = MacLLDNMgmtTS * 2;	
		ACKFrame_size = 0;
		counter_associados = 0;
		n = 0;
	}

	static void appPanDiscInit(void)
	{	
		/* clear Ack array of previous discovery state */
		for(int i = 0; i < 32; i++)
			ACKFrame.ackFlags[i] = 0;
		ACKFrame_size = 0;
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
		n = 127; 
		tTS =  ((p_var*sp + (m+n)*sm + macMinLIFSPeriod)/v_var);
		#if (MASTER_MACSC == 1)
		
			beaconInterval = 2 * numBaseTimeSlotperMgmt_association * (tTS) / (SYMBOL_TIME);
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
			
			/* Timer used in testing */
			#if TIMESLOT_TIMER
			macsc_set_cmp2_int_cb(teste_handler);	
			macsc_enable_cmp_int(MACSC_CC2);
			macsc_use_cmp(MACSC_RELATIVE_CMP, beaconInterval / 2, MACSC_CC2);
			#endif
			
		#endif
		}
	}

	static void appPanOnlineInit()
	{
			timeslot_counter = 0;
			
			tTS =  ((p_var*sp + (m+ config_request_frame.conf.tsDuration )*sm + macMinLIFSPeriod)/v_var);
			
			n = config_request_frame.conf.tsDuration;
			
			msgReq.dstAddr				= 0;
			msgReq.dstEndpoint			= APP_BEACON_ENDPOINT;
			msgReq.srcEndpoint			= APP_BEACON_ENDPOINT;
			msgReq.options				= NWK_OPT_LLDN_BEACON | NWK_OPT_ONLINE_STATE;
			msgReq.data					= NULL;
			msgReq.size					= 0;
			
			// (number of time slots x mgmt time solts) x base timelosts
			beaconInterval = (assTimeSlot + 2*MacLLDNMgmtTS) * tTS / (SYMBOL_TIME);
			
			// Configure Timers
			macsc_set_cmp1_int_cb(time_slot_handler);
			
			macsc_enable_manual_bts();
			
			macsc_enable_cmp_int(MACSC_CC1);
			macsc_use_cmp(MACSC_RELATIVE_CMP, numBaseTimeSlotperMgmt_online * tTS / (SYMBOL_TIME), MACSC_CC1);
			
			NWK_OpenEndpoint(APP_DATA_ENDPOINT, appDataInd);

			
	}


#else 
	uint8_t payloadSize = 22;
	uint8_t assTimeSlot = 0xFF;
	uint8_t n = 0;
	
	static NwkFrameBeaconHeaderLLDN_t *rec_beacon;
	static NWK_DiscoverResponse_t msgDiscResponse = { .id = LL_DISCOVER_RESPONSE,
													 .macAddr = APP_ADDR,
													 .ts_dir.tsDuration = 22,
													 .ts_dir.dirIndicator = 0b1 };
	static NWK_ConfigStatus_t msgConfigStatus = { .id = LL_CONFIGURATION_STATUS,
												 .macAddr = APP_ADDR,
												 .s_macAddr = APP_ADDR,
												 .ts_dir.tsDuration = 22,
												 .ts_dir.dirIndicator = 1,
												 .assTimeSlot = 0xff };
	uint8_t data_payload = APP_ADDR;
	static bool ack_received;
	bool MacLLDNMgmtTS = 0; 
	bool associated = 0;
	
	static void send_message_timeHandler(void)
	{
		printf("\nmsg_hdlr");
		appState = APP_STATE_SEND;	
		#if MASTER_MACSC == 0
			timer_stop();
		#endif
	}

	static void start_timer(int delay)
	{
		#if MASTER_MACSC
		macsc_enable_manual_bts();
		macsc_set_cmp1_int_cb(send_message_timeHandler);
		macsc_enable_cmp_int(MACSC_CC1);
		macsc_use_cmp(MACSC_RELATIVE_CMP, delay - 195, MACSC_CC1);
		#else
		timer_init();
		timer_delay(delay/2);
		hw_timer_setup_handler(send_message_timeHandler);
		timer_start();
		#endif
	}
	
	static bool appBeaconInd(NWK_DataInd_t *ind)
	{
		rec_beacon = (NwkFrameBeaconHeaderLLDN_t*)ind->data;
		PanId = rec_beacon->PanId;
		// é bom implementar rotinas pra se o nodo estiver associado a um coordeandor e se não estiver
		if( ((rec_beacon->Flags.txState == DISC_MODE && !ack_received && rec_beacon->confSeqNumber == 0x00) || 
			(rec_beacon->Flags.txState == CONFIG_MODE && ack_received)) && associated == 0)
		{
	
			int ts_time =  ((p_var*sp + (m+ rec_beacon->TimeSlotSize  )*sm + macMinLIFSPeriod)/v_var)  / (SYMBOL_TIME);
			int msg_wait_time = rec_beacon->Flags.numBaseMgmtTimeslots * rec_beacon->TimeSlotSize * 2; // symbols 190 is a delay adjustment
			
			start_timer(msg_wait_time);
			
			appState = (rec_beacon->Flags.txState == DISC_MODE) ? APP_STATE_PREP_DISC_REPONSE : APP_STATE_PREP_CONFIG_STATUS;
		}
		else if (rec_beacon->Flags.txState == ONLINE_MODE && assTimeSlot != 0xFF && associated == 1)
		{
			int ts_time = ((p_var*sp + (m+ n)*sm + macMinLIFSPeriod)/v_var)  / (SYMBOL_TIME);
			int msg_wait_time = (2*rec_beacon->Flags.numBaseMgmtTimeslots + assTimeSlot) * ts_time;
			printf("msg_wait_time: %d", msg_wait_time);
			start_timer(msg_wait_time - 80);
			appState = APP_STATE_PREP_DATA_FRAME;
		}
		else if (rec_beacon->Flags.txState == RESET_MODE)
		{
			printf("\n Reset beacon");
			ack_received = 0;
			associated = 0;
		}
		return true;
	}
	
	static bool appAckInd(NWK_DataInd_t *ind)
	{
		#if !MASTER_MACSC
		ind->data = ind->data - (uint8_t) 1;
		#endif
		NWK_ACKFormat_t *ackframe = (NWK_ACKFormat_t*)ind->data;
		if(PanId == ackframe->sourceId)
		{
			int pos = (int) APP_ADDR / 8;
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
		#if !MASTER_MACSC
		ind->data = ind->data - (uint8_t) 1;
		#endif
		
		if(ind->data[0] == LL_CONFIGURATION_REQUEST)
		{
			NWK_ConfigRequest_t *msg = (NWK_ConfigRequest_t*)ind->data;
			if(msg->macAddr == APP_ADDR)
			{
				PHY_SetChannel(msg->tx_channel);
				NWK_SetPanId(msg->s_macAddr);
				PanId = msg->s_macAddr;
				assTimeSlot = msg->assTimeSlot;
				n = msg->conf.tsDuration;
				associated = 1;
				printf("\nAssociado!");
			}
		}
		return true;
	}

	void appPrepareDiscoverResponse()
	{
		msgReq.dstAddr				= 0;
		msgReq.dstEndpoint			= APP_COMMAND_ENDPOINT;
		msgReq.srcEndpoint			= APP_COMMAND_ENDPOINT;
		msgReq.options				= NWK_OPT_MAC_COMMAND;
		msgReq.data					= (uint8_t*)&msgDiscResponse;
		msgReq.size					= sizeof(msgDiscResponse);
	}
	
	void appPrepareConfigurationStatus()
	{		
		msgReq.dstAddr				= 0;
		msgReq.dstEndpoint			= APP_COMMAND_ENDPOINT;
		msgReq.srcEndpoint			= APP_COMMAND_ENDPOINT;
		msgReq.options				= NWK_OPT_MAC_COMMAND;
		msgReq.data					= (uint8_t*)&msgConfigStatus;
		msgReq.size					= sizeof(msgConfigStatus);
	}
	
	void appPrepareDataFrame(void)
	{
		
		PHY_SetTdmaMode(false);

	
		msgReq.dstAddr				= 0;
		msgReq.dstEndpoint			= APP_COMMAND_ENDPOINT;
		msgReq.srcEndpoint			= APP_COMMAND_ENDPOINT;
		msgReq.options				= NWK_OPT_LLDN_DATA;
		msgReq.data					= (uint8_t *)&data_payload;
		msgReq.size					= sizeof(data_payload);
	}
	
	
#endif // APP_COORDINATOR

static void appInit(void)
{
	NWK_SetAddr(APP_ADDR);
	PHY_SetChannel(APP_CHANNEL);
	PHY_SetRxState(true);
		
	#if APP_COORDINATOR	 
		/* Timer used for delay between messages */
		tmrDelay.interval = 2;
		tmrDelay.mode = SYS_TIMER_INTERVAL_MODE;
		tmrDelay.handler = tmrDelayHandler;
	  
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
		// PHY_SetOptimizedCSMAValues();
		
		payloadSize = 127;
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
					/* if nodes associated is equal to expected number of associated nodes stop association process 
					 * this implementation was done as is to be used in tests, for real network functionality 
					 * the number of max association processes must be done through macLLDNdiscoveryModeTimeout
					 */
					if(counter_associados == NODOS_ASSOCIADOS_ESPERADOS || cycles_counter >= 2)
					{	
						printf("\n%d, %d", cycles_counter, counter_associados);
						counter_associados = 0;
						/* if all nodes expected where associated stop beacon generation interruptions */
						// macsc_disable_cmp_int(MACSC_CC1);
						macsc_disable_cmp_int(MACSC_CC2);
						msgReq.options = 0;
						/* set coordinator node to idle further implementation of online state must be done */
						appState = APP_STATE_IDLE;
						appPanState = APP_PAN_STATE_ONLINE_INITIAL; // APP_PAN_STATE_ONLINE_INIT
						
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
					msgReq.data = NULL;
					msgReq.size = 0;
					
					appState	= APP_PAN_STATE_DISC_PREPARE_ACK;
					appPanState = APP_PAN_STATE_DISC_PREPARE_ACK;
					break;
				}
				case APP_PAN_STATE_DISC_PREPARE_ACK:
				{
					/* This timer implements a delay between messages, 
					 * if not used the nodes are not able to receive the message
					 */
					appPanPrepareACK();
					SYS_TimerStart(&tmrDelay);
					
					appPanState = APP_PAN_STATE_CONFIG_INITIAL; 
					appState = APP_STATE_IDLE;
					break;
				}
				case APP_PAN_STATE_CONFIG_INITIAL:
				{
					/* Prepares the message as: Configuration Beacon and First State Beacon */
					msgReq.options = NWK_OPT_LLDN_BEACON | NWK_OPT_CONFIG_STATE;
					msgReq.data = NULL;
					msgReq.size = 0;
					
					appState	= APP_STATE_IDLE;
					appPanState = APP_PAN_STATE_CONFIG_SECOND_BEACON;
					break;

				}
				case APP_PAN_STATE_CONFIG_SECOND_BEACON:
				{
					/* Prepares the message as: Configuration Beacon and Second State Beacon */
					msgReq.options = NWK_OPT_LLDN_BEACON | NWK_OPT_CONFIG_STATE | NWK_OPT_SECOND_BEACON;
					msgReq.data = NULL;
					msgReq.size = 0;
					
					appState	= APP_STATE_IDLE;
					appPanState = APP_PAN_STATE_SEND_CONF_REQUEST;
					break;
				}
				case APP_PAN_STATE_SEND_CONF_REQUEST:
				{

					if(conf_req_list != NULL)
					{
						CopyToConfigRequest();
						msgReq.options		= NWK_OPT_MAC_COMMAND;
						msgReq.data			= (uint8_t*)&config_request_frame;
						msgReq.size			= sizeof(NWK_ConfigRequest_t);
						appState	= APP_STATE_IDLE;
						appPanState = APP_PAN_STATE_SEND_CONF_REQUEST;
						
						counter_associados++;
						SYS_TimerStart(&tmrDelay);
					}
					else
					{
						appPanState = APP_PAN_STATE_CONFIG_THIRD_BEACON;
					}
					break;
				}
				case APP_PAN_STATE_CONFIG_THIRD_BEACON:
				{
					msgReq.options = NWK_OPT_LLDN_BEACON | NWK_OPT_CONFIG_STATE | NWK_OPT_THIRD_BEACON;
					msgReq.data = NULL;
					msgReq.size = 0;
					
					appState	= APP_STATE_IDLE;
					appPanState = APP_PAN_STATE_DISC_INITIAL;
					cycles_counter++;	
					
					break;
				}
				case APP_PAN_STATE_ONLINE_INITIAL:
				{
					appPanOnlineInit();
					appState = APP_STATE_SEND;
					appPanState = APP_PAN_STATE_CHECK_TS;

					break;
				}
				case APP_PAN_STATE_CHECK_TS:
				{
					if(timeslot_counter >= assTimeSlot)
					{
						printf("\nFim de um Período");
						macsc_disable_cmp_int(MACSC_CC1);
						macsc_disable_cmp_int(MACSC_CC2);
						appState = APP_STATE_IDLE;
						appPanState = APP_PAN_STATE_IDLE;
					}
					else
					{
						/* check if coordinator received any message in last time slot, used to calculate success rate */
						if(timeslot_counter >= 2*MacLLDNMgmtTS && !data_received)
							nodes_info_arr[timeslot_counter - 2*MacLLDNMgmtTS].msg_not_rec++;
						data_received = false;
						
						printf("\n------- slot %d --------", timeslot_counter);
						appState = APP_STATE_IDLE;
						timeslot_counter++;
					}
					break;
				}
				case APP_PAN_STATE_ONLINE_END_BE:
				{
					if(0)
					{
						// implementar as condições para entrar no processo de associação
					}
					else
					{
						// posso calcular esse valor no onlineinit, o tTS precisa ser recalculado, o seu valor muda no inicio do online, talvez definir este valor ? 
						// pode voltar pro state_online_initial porque precisa reconfigurar os timers
						// precisa revisar o macsc_enable_manual_bts()
						/*
						int idle_time =  2 * numBaseTimeSlotperMgmt * (tTS) * 5 / (SYMBOL_TIME); // 5 is the total of beacons in discovery + configuration
						macsc_set_cmp1_int_cb(lldn_server_beacon); // esta função pode só mandar o beacon do online, checar se não precisa atualizar em nada este beacon, seq number é atualizado sozinho
						macsc_enable_manual_bts();
						macsc_enable_cmp_int(MACSC_CC1);
						macsc_use_cmp(MACSC_RELATIVE_CMP, idle_time, MACSC_CC1);
						*/
					}
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
			appPrepareDiscoverResponse();
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
				#if MASTER_MACSC
				macsc_disable_cmp_int(MACSC_CC1);
				#else
				timer_stop();
				#endif
			}
			ack_received = 0;
			appState = APP_STATE_IDLE;
			break;
		}
		case APP_STATE_PREP_DATA_FRAME:
		{
			appPrepareDataFrame();
			appState = APP_STATE_IDLE;
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
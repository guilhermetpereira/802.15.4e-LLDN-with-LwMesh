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
		NWK_DataReq(&msgReq);
		#if PRINT
// 		if(msgReq.options & NWK_OPT_ONLINE_STATE)
// 			printf("\n  BEACON ");
		//printf("\nREQ %d",msgReq.options);
		#endif
	#if !APP_COORDINATOR
			macsc_set_cmp2_int_cb(0);
	#endif
	}
}

#if APP_COORDINATOR

	#define NODOS_ASSOCIADOS_ESPERADOS 12
	int accepting_requests = 1;
	AppPanState_t appPanState = APP_PAN_STATE_RESET; // Initial state of PAN node
	
	/* Configuration Request Frames */
	/* Da pra mudar o envio do confrequest pra ja usar essa array com as informaçõs dos nodos */	
	nodes_info_t nodes_info_arr[50]; // Array for Configure Request messages, one position per node, 254 limited by star topology
	NWK_ConfigRequest_t config_request_frame = { .id = LL_CONFIGURATION_REQUEST,
												 .s_macAddr = APP_ADDR,
												 .tx_channel = APP_CHANNEL,
												 .conf.macLLDNmgmtTS = MacLLDNMgmtTS };



	nodes_info_list_t *conf_req_list = NULL;
	uint8_t stack_conf_req[20];
	
	/* Acknowledge Frame and Array */
	NWK_ACKFormat_t ACKFrame;	
	NWK_ACKFormat_t ACKFrame_aux;
	
	int ACKFrame_size = 0;
	

	float beaconInterval = 0; // não precisa ser global
	float beaconInterval_association = 0;
	
	/* This timer implements a delay between messages, 
	 * if not used the nodes are not able to receive the message
	 */
	static SYS_Timer_t tmrDelay;	
	
	/*  Control variables for testing */	
	int assTimeSlot = 0;

	int counter_associados = 0;		// Associated nodes counter
	uint8_t cycles_counter = macLLDNdiscoveryModeTimeout;

	bool association_request = false;

	/* data related variables */
	msg_info_t msg_info_array[50]; // size of array limited by hardware
	unsigned int size_msg_info = 0;
	bool data_received = false;	
	float PLR = 0;
	float PER = 0;
	uint8_t retransmit_ts_array[32];
	int retransmit_ts_array_counter = 0;
	
	int counter_delay_msg = 0;
	uint32_t cmp_value_start_superframe = 0;
	float count_up = 0;
	float count_lost_superframe = 0;
	
	static NWK_DataReq_t msgReqGACK = { .dstAddr = 0,
										.dstEndpoint = APP_BEACON_ENDPOINT,
										.srcEndpoint = APP_BEACON_ENDPOINT,
										.options = NWK_OPT_LLDN_ACK,
										.data = (uint8_t*)&ACKFrame};

	
	static void lldn_server_beacon(void)
	{
		macsc_enable_manual_bts();
		appState = APP_STATE_SEND;
	}
	

	static void downlink_delay_handler(void)
	{
		if(msgReq.options == NWK_OPT_MAC_COMMAND)
		{
			counter_delay_msg++;
			appState = APP_STATE_SEND;
		}
	}


	#if TIMESLOT_TIMER
	static void teste_handler(void)
	{
		if(msgReq.options)
			// printf("\n***TIMESLOT****");
		macsc_disable_cmp_int(MACSC_CC3);
	}
	#endif
	
	static bool addToAckArray(uint8_t addres)
	{	
		int pos,bit_shift;
		if(addres == 8)
		{
			pos = 0;
			bit_shift = 0;
		}
		else
		{
			pos =  addres / 8;
			bit_shift = 8 - addres % 8;
		}
		if(ACKFrame.ackFlags[pos] & 1 << bit_shift)
		{
			// printf("\nAddr rep %d", addres);
			return false;
		}
		ACKFrame.ackFlags[pos] |= 1 << bit_shift;
		if (pos + 1 > ACKFrame_size)
			ACKFrame_size = pos + 1;
		// printf("ACK, %hhx", ACKFrame.ackFlags[pos]);
		return true;
	}

	static void addConfRequestArray(NWK_ConfigStatus_t *node)
	{

		uint8_t i;
		/*check if mac addres is already associated*/ 
		for (int j = 0; j <= assTimeSlot ; j++)
		{
			if (nodes_info_arr[j].mac_addr == node->macAddr)
			{	
				return;
			}
		}
		/*find first available timeslot*/
		for (i= 0;i < 50 && nodes_info_arr[i].mac_addr != 0; i++);
		
		assTimeSlot++;
	
		if(node->ts_dir.tsDuration > config_request_frame.conf.tsDuration)
			config_request_frame.conf.tsDuration =  node->ts_dir.tsDuration;
		
		nodes_info_arr[i].req_timeslot_duration = node->ts_dir.tsDuration;
		nodes_info_arr[i].mac_addr = node->macAddr;
		nodes_info_arr[i].assigned_time_slot = (uint8_t)i;
		
// 		stack_conf_req
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
		return;
	}

	static bool CopyToConfigRequest(void)
	{
		if(conf_req_list->node == NULL)
			return false;
		config_request_frame.assTimeSlot = conf_req_list->node->assigned_time_slot;
		config_request_frame.macAddr = conf_req_list->node->mac_addr;
		nodes_info_list_t *tmp = conf_req_list;
		conf_req_list = conf_req_list->next;
		tmp->node = NULL;
		tmp->next = NULL;
		free(tmp);
		return true;
	}


	static bool appCommandInd(NWK_DataInd_t *ind)
	{
		if(!accepting_requests) return false;
		// if(n <127) return false;
		if(ind->data[0] == LL_DISCOVER_RESPONSE)
		{
			NWK_DiscoverResponse_t *msg = (NWK_DiscoverResponse_t*)ind->data;
			if(msg->macAddr > 0x0F)
			return false;
			addToAckArray(msg->macAddr);	
					
			#if PRINT
			//uint32_t tmp = macsc_read_count();
// 			printf("\n%" PRIu32 " ", tmp);			
 			// printf(" DISC %hhx", msg->macAddr);	
			#endif
		}
		else if(ind->data[0] == LL_CONFIGURATION_STATUS)
		{
			NWK_ConfigStatus_t *msg = (NWK_ConfigStatus_t*)ind->data;
			if(msg->macAddr > 0x0F)
			return false;
			addConfRequestArray(msg);
			#if PRINT
			// printf("\nCONF %d", msg->macAddr);	
			#endif
		}
		else return false;			
		return true;
	}
	
	bool check_ack_pan(int addr)
	{
		int pos,bit_shift;
		if(addr == 8)
		{
			pos = 0;
			bit_shift = 0;
		}
		else
		{
			pos =  addr / 8;
			bit_shift = 8 - addr % 8;
		}
		if( ACKFrame.ackFlags[pos] & 1 << bit_shift)
		{
			return true;
		}
		else
			return false;
	}
	
	bool check_ack_aux(int addr)
	{
		int pos,bit_shift;
		if(addr == 8)
		{
			pos = 0;
			bit_shift = 0;
		}
		else
		{
			pos =  addr / 8;
			bit_shift = 8 - addr % 8;
		}
		if( ACKFrame_aux.ackFlags[pos] & 1 << bit_shift)
		{
			return true;
		}
		else
		return false;
	}
	
	static bool appDataInd(NWK_DataInd_t *ind)
	{
		uint32_t cmp_value = macsc_read_count();
		int relative_cmp =  cmp_value - cmp_value_start_superframe - 40;
		

		int curr_up_ts = (relative_cmp)/(tTS / (SYMBOL_TIME)) - 2*MacLLDNMgmtTS*numBaseTimeSlotperMgmt_online;
		if(curr_up_ts > assTimeSlot)
		{
			
			int i;
			curr_up_ts -= assTimeSlot+1;
			for (i = 0; i < assTimeSlot && curr_up_ts > 0; ++i)
			{
				if(!check_ack_aux(i+1))
				{
					curr_up_ts--;
				}
			}
			curr_up_ts = i-1*(i!=0);
		}
	
		if(nodes_info_arr[curr_up_ts].mac_addr != ind->data[0])
			return false;
		nodes_info_arr[curr_up_ts].rssi = ind->rssi;
		nodes_info_arr[curr_up_ts].average_rssi = (nodes_info_arr[curr_up_ts].rssi + nodes_info_arr[curr_up_ts].average_rssi*nodes_info_arr[curr_up_ts].msg_rec)
																				/(nodes_info_arr[curr_up_ts].msg_rec + 1);
		nodes_info_arr[curr_up_ts].msg_rec++;
	count_up++;
		addToAckArray(curr_up_ts+1);
		#if PRINT
//  		printf("\n[%d] Data: ", curr_up_ts);
		#endif
		for (int i = 0; i < ind->size; i++)
		{
			msg_info_array[curr_up_ts].data_payload[i] = ind->data[i];
			#if PRINT
//  			printf("%hhx", msg_info_array[curr_up_ts].data_payload[i]);
			#endif
		}		
		return true;
// 		printf(" cmp %d", relative_cmp);
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
		ACKFrame_size = 0;
		counter_associados = 0;
		macLLDNnumUplinkTS = 0;
		n = 0;
	}

	static void appPanDiscInit(void)
	{	
		/* clearray of previous discovery state */

		/* Prepare Beacon Message as first beacon in discovery state */		
		msgReq.dstAddr				= 0;
		msgReq.dstEndpoint			= APP_BEACON_ENDPOINT;
		msgReq.srcEndpoint			= APP_BEACON_ENDPOINT;
		msgReq.options				= NWK_OPT_LLDN_BEACON | NWK_OPT_DISCOVERY_STATE;
		msgReq.data					= NULL;
		msgReq.size					= 0;
		
		macLLDNnumTimeSlots = 2;
		accepting_requests = 1;
		/* Only start timers if it is the first association process */
		if(cycles_counter == 0) 
		{
		for(int i = 0; i < 32; i++)
		ACKFrame.ackFlags[i] = 0;
		ACKFrame_size = 0;
		/* Calculates Beacon Intervals according to 802.15.4e - 2012 p. 70 */
		n = 127; 
		tTS =  ((p_var*sp + (m+n)*sm + macMinLIFSPeriod)/v_var);
		#if (MASTER_MACSC == 1)
		
			beaconInterval_association = 2 * numBaseTimeSlotperMgmt_association * (tTS) / (SYMBOL_TIME);
			#if PRINT
// 			printf("\n Beacon interval %f", beaconInterval_association);
			#endif
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

			macsc_use_cmp(MACSC_RELATIVE_CMP, beaconInterval_association , MACSC_CC1);
			
			/* Timer used in testing */
			#if TIMESLOT_TIMER
			macsc_set_cmp2_int_cb(teste_handler);	
			macsc_enable_cmp_int(MACSC_CC2);
			macsc_use_cmp(MACSC_RELATIVE_CMP, beaconInterval_association / 2, MACSC_CC2);
			#endif
			
		#endif
		}
	}
	
	static void end_of_superframe_Online(void)
	{
		appPanState = APP_PAN_STATE_ONLINE_INITIAL;
		if(cycles_counter>0)
			appState = APP_STATE_SEND;
		else
		appState = APP_STATE_ATT_PAN_STATE;

	}
	
	static void send_gack(void)
	{
		msgReqGACK.size = sizeof(uint8_t)*(macLLDNRetransmitTS + 1);
		NWK_DataReq(&msgReqGACK);
		ACKFrame_aux = ACKFrame;
		#if PRINT
// 		printf("\n%hhx ", ACKFrame.ackFlags[0]);
		#endif
		if(cycles_counter<NUMERO_CICLOS_ONLINE-1)
		{
			for (int i  = 0; i < assTimeSlot;i++)
				if(!check_ack_pan(i+1))
				{
					nodes_info_arr[i].msg_not_rec++;
					count_lost_superframe++;
			}
		}
	}

	static void appPanOnlineInit()
	{
			
			macLLDNnumUplinkTS = (assTimeSlot) * 2 + 1;
			macLLDNRetransmitTS = assTimeSlot;
			macLLDNnumTimeSlots = macLLDNnumUplinkTS + 2 *MacLLDNMgmtTS;			
			
 			// printf("\nmacLLDNnumUplinkTS : %d\nmacLLDNRetransmitTS : %d\nmacLLDNnumTimeSlots : %d",macLLDNnumUplinkTS,macLLDNRetransmitTS,macLLDNnumTimeSlots );
			
			config_request_frame.conf.tsDuration = 75;

			tTS =  ((p_var*sp + (m+ config_request_frame.conf.tsDuration )*sm + macMinLIFSPeriod)/v_var);
				
			n = config_request_frame.conf.tsDuration;
			

			msgReq.dstAddr				= 0;
			msgReq.dstEndpoint			= APP_BEACON_ENDPOINT;
			msgReq.srcEndpoint			= APP_BEACON_ENDPOINT;
			msgReq.options				= NWK_OPT_LLDN_BEACON | NWK_OPT_ONLINE_STATE;
			msgReq.data					= NULL;
			msgReq.size					= 0;
			
			beaconInterval= (macLLDNnumUplinkTS + 2*MacLLDNMgmtTS*numBaseTimeSlotperMgmt_online) * tTS / (SYMBOL_TIME);
			float GACK_tTS = (assTimeSlot + 0.2 + 2*MacLLDNMgmtTS*numBaseTimeSlotperMgmt_online) * tTS / (SYMBOL_TIME);
			macsc_write_count(0);
			macsc_set_cmp1_int_cb(end_of_superframe_Online);
			macsc_enable_cmp_int(MACSC_CC1);

			macsc_set_cmp2_int_cb(send_gack);
			if(cycles_counter != NUMERO_CICLOS_ONLINE)
				macsc_enable_cmp_int(MACSC_CC2);

			macsc_use_cmp(MACSC_RELATIVE_CMP, beaconInterval, MACSC_CC1);
			macsc_use_cmp(MACSC_RELATIVE_CMP, GACK_tTS, MACSC_CC2);
			macsc_enable_manual_bts();
			
			cmp_value_start_superframe = macsc_read_count();


			for(int i = 0; i < 32; i++)
			{
				ACKFrame.ackFlags[i] = 0;
				retransmit_ts_array[i] = 0;
			}
			retransmit_ts_array_counter = 0;
			ACKFrame_size = 0;
			if (cycles_counter < NUMERO_CICLOS_ONLINE-1)
			{
				printf("\n%.3f %.3f",/*PER*/ count_lost_superframe/assTimeSlot , /*PLR*/1 - count_up/assTimeSlot);
				count_up = 0;
				count_lost_superframe = 0;
			}
			NWK_OpenEndpoint(APP_DATA_ENDPOINT, appDataInd);
	}

#else 
	uint8_t payloadSize = 50;
	uint8_t assTimeSlot = 0xFF;
	uint8_t n = 0;
	uint8_t data_payload = APP_ADDR;


	
	NwkFrameBeaconHeaderLLDN_t rec_beacon;
	NWK_ACKFormat_t *ackframe;
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

	static bool ack_received = false;
	bool MacLLDNMgmtTS = 0; 
	bool associated = 0;
	int ts_time;
	uint8_t STATE = DISC_MODE;

	static void send_message_timeHandler(void)
	{
		appSendData();
		#if MASTER_MACSC == 0
			timer_stop();
		#else 
		macsc_disable_cmp_int(MACSC_CC1);
		#endif
	
	}


	static void disc_time_hndlr(void)
	{
		printf("\nDisc");
			NWK_DataReq(&msgReqDiscResponse);
		macsc_set_cmp1_int_cb(0);
		
	}
	
	static void config_time_hndlr(void)
	{
		if(ack_received)
		{
			NWK_DataReq(&msgReqConfigStatus);
			ack_received = false;
		}
		macsc_set_cmp1_int_cb(0);
		
	}
			
	static void online_time_hndlr(void)
	{
		appState = APP_STATE_WAKEUP_AND_SEND;
		macsc_set_cmp1_int_cb(0);		
	}
	
	static void beacon_interval_time_hndlr(void)
	{
		appState  = APP_STATE_WAKEUP_AND_WAIT;
		macsc_set_cmp2_int_cb(0);		
	}
	
	static bool appBeaconInd(NWK_DataInd_t *ind)
	{
		NwkFrameBeaconHeaderLLDN_t *tmp_beacon = (NwkFrameBeaconHeaderLLDN_t*)ind->data;
		rec_beacon = *tmp_beacon;
		PanId = tmp_beacon->PanId;
		tTS =  ((p_var*sp + (m+ tmp_beacon->TimeSlotSize )*sm + macMinLIFSPeriod)/v_var)  / (SYMBOL_TIME);
		if(rec_beacon.Flags.txState == STATE && rec_beacon.confSeqNumber == 0)
		{
// 				beaconInterval= (macLLDNnumUplinkTS + 2*MacLLDNMgmtTS*numBaseTimeSlotperMgmt_online) * tTS / (SYMBOL_TIME);

			int msg_wait_time = tTS * rec_beacon.Flags.numBaseMgmtTimeslots;
			int ack_wait_time;
			if(STATE == DISC_MODE) 
				macsc_set_cmp1_int_cb(disc_time_hndlr);
			else if(STATE == CONFIG_MODE)
				macsc_set_cmp1_int_cb(config_time_hndlr);
			else if(STATE == ONLINE_MODE)
			{
				int beacon_interval = macLLDNnumTimeSlots*tTS;
							
				
				macsc_set_cmp1_int_cb(online_time_hndlr);
				
				macsc_set_cmp2_int_cb(beacon_interval_time_hndlr);
				macsc_enable_cmp_int(MACSC_CC2);
				macsc_use_cmp(MACSC_RELATIVE_CMP, beacon_interval, MACSC_CC2);

				msg_wait_time = (2*rec_beacon.Flags.numBaseMgmtTimeslots + assTimeSlot) * tTS /*+ 80 + 45*assTimeSlot*/;
				
				
				#if COOP_RT
				#else
				ack_wait_time = (rec_beacon.NumberOfBaseTimeslotsinSuperframe);
				appState = APP_STATE_SLEEP_PREPARE;
				#endif
			}

			macsc_enable_cmp_int(MACSC_CC1);
			macsc_use_cmp(MACSC_RELATIVE_CMP, msg_wait_time - 150, MACSC_CC1);
 			macsc_enable_manual_bts();
			
		}

		else if (rec_beacon.Flags.txState == RESET_MODE)
		{
			PHY_SetTdmaMode(false);
			ack_received = 0;
			STATE = DISC_MODE;
		}
		return true;
	}
	
	bool check_ack(int addr)
	{
		int pos,bit_shift;
		if(addr == 8)
		{
			pos = 0;
			bit_shift = 0;
		}
		else
		{
			pos =  addr / 8;
			bit_shift = 8 - addr % 8;
		}
		if( ackframe->ackFlags[pos] & 1 << bit_shift)
		{
			return true;
		}
		else
			return false;
	}
	
	static bool appAckInd(NWK_DataInd_t *ind)
	{
		#if !MASTER_MACSC
		ind->data = ind->data - (uint8_t) 1;
		#endif
		ackframe = (NWK_ACKFormat_t*)ind->data;

		if(PanId == ackframe->sourceId)
		{
			if(STATE == ONLINE_MODE && rec_beacon.Flags.txState == ONLINE_MODE)
			{
				ack_received = check_ack(assTimeSlot + 1);
				if(!ack_received)
				{
				
					int retransmition_slot = 0;
					for(int i = 0; i < assTimeSlot && i < (rec_beacon.NumberOfBaseTimeslotsinSuperframe - 2*rec_beacon.Flags.numBaseMgmtTimeslots - 1)/2; i++) // -3 because of 2 mgmt and 1 gack
						if( !check_ack(i+1) )
							retransmition_slot++;
					if(retransmition_slot == 0)
					{
						NWK_DataReq(&msgReqData);
					}
					else
					{
						#if MASTER_MACSC
						macsc_enable_manual_bts();
						macsc_set_cmp1_int_cb(online_time_hndlr);
						macsc_enable_cmp_int(MACSC_CC1);
						macsc_use_cmp(MACSC_RELATIVE_CMP, tTS * retransmition_slot - 40, MACSC_CC1);
						#endif
					}					

				}				
			}
			else
			{
				ack_received = check_ack(APP_ADDR);								
				if(STATE == DISC_MODE && ack_received)
					STATE = CONFIG_MODE;
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
				STATE = ONLINE_MODE;
				PHY_SetTdmaMode(true);
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
		
		PHY_SetTdmaMode(true);

	
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
// 		tmrDelay.interval = 1;
// 		tmrDelay.mode = SYS_TIMER_INTERVAL_MODE;
// 		tmrDelay.handler = tmrDelayHandler;
// 	  
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
		appPrepareDiscoverResponse();	
		/*
		 * Enable CSMA/CA
		 * Enable Random CSMA seed generator
		 */
		PHY_SetTdmaMode(false);
		PHY_SetOptimizedCSMAValues();
		
		
		
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
					if(counter_associados == NODOS_ASSOCIADOS_ESPERADOS || cycles_counter >= 10)
					{	
						printf("\n%d, %d", cycles_counter, counter_associados);
						counter_associados = 0;
						/* if all nodes expected where associated stop beacon generation interruptions */
						macsc_disable_cmp_int(MACSC_CC1);
						macsc_disable_cmp_int(MACSC_CC2);
						msgReq.options = 0;
						
						/* reseting ack bitmap */
						for(int i = 0; i < 32; i++)
						ACKFrame.ackFlags[i] = 0;
						ACKFrame_size = 0;
						accepting_requests = 0;
						appState = APP_STATE_ATT_PAN_STATE;
						appPanState = APP_PAN_STATE_ONLINE_INITIAL; // APP_PAN_STATE_ONLINE_INIT
						cycles_counter = NUMERO_CICLOS_ONLINE;
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
					
					appState	= APP_STATE_IDLE;
					appPanState = APP_PAN_STATE_DISC_PREPARE_ACK;
					break;
				}
				case APP_PAN_STATE_DISC_PREPARE_ACK:
				{
					/* This timer implements a delay between messages, 
					 * if not used the nodes are not able to receive the message
					 */
					appPanPrepareACK();
					appState = APP_STATE_SEND;
					// SYS_TimerStart(&tmrDelay);
					
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
					
					counter_delay_msg = 0;
					
					appState	= APP_STATE_IDLE;
					appPanState = APP_PAN_STATE_SEND_CONF_REQUEST;
					
					break;
				}
				case APP_PAN_STATE_SEND_CONF_REQUEST:
				{
					if(conf_req_list != NULL)
					{
						if(CopyToConfigRequest())
						{
						msgReq.options		= NWK_OPT_MAC_COMMAND;
						msgReq.data			= (uint8_t*)&config_request_frame;
						msgReq.size			= sizeof(NWK_ConfigRequest_t);
						appState	= APP_STATE_IDLE;
						appPanState = APP_PAN_STATE_SEND_CONF_REQUEST;
// 						printf("  assts %d %hhx",config_request_frame.assTimeSlot,  config_request_frame.macAddr);
						// Delay between messages
						}
						else
						{
							msgReq.options = 0;
						}
						macsc_set_cmp1_int_cb(downlink_delay_handler);
						macsc_disable_cmp_int(MACSC_CC1);
						macsc_enable_manual_bts();
						macsc_enable_cmp_int(MACSC_CC1);
						macsc_use_cmp(MACSC_RELATIVE_CMP, DELAY, MACSC_CC1);
						
						counter_associados++;
					}
					else
					{
						msgReq.options = 0;

						if(counter_delay_msg > 0)
						{
							macsc_set_cmp1_int_cb(lldn_server_beacon);
							macsc_disable_cmp_int(MACSC_CC1);
							macsc_enable_manual_bts();
							macsc_enable_cmp_int(MACSC_CC1);
							macsc_use_cmp(MACSC_RELATIVE_CMP,beaconInterval_association - counter_delay_msg * DELAY, MACSC_CC1);
						}
						
						appState	= APP_STATE_ATT_PAN_STATE;
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

					if(cycles_counter != 0)
					{
						appPanOnlineInit();
						
						appState = APP_STATE_IDLE;
						appPanState = APP_PAN_STATE_IDLE;
						
						cycles_counter--;
					}
					else
					{
						appState = APP_STATE_IDLE;
						appPanState = APP_PAN_STATE_IDLE;
						macsc_disable_cmp_int(MACSC_CC1);
						
						printf("\n\n Métricas (%d Ciclos):\n", NUMERO_CICLOS_ONLINE -1);
						int total_msg = 0;
						float uplink_lost_packets = 0;
						float expected_messages = assTimeSlot * (float)(NUMERO_CICLOS_ONLINE - 1);
						for(int i = 0; nodes_info_arr[i].mac_addr != 0 && assTimeSlot > i; i++)
						{
							printf("\nAddrs , %hhx", nodes_info_arr[i].mac_addr);
							printf("\nPLR ,  %.3f", 1 - nodes_info_arr[i].msg_rec / (float)(NUMERO_CICLOS_ONLINE - 1));
							printf("\nPER ,  %.3f", nodes_info_arr[i].msg_not_rec / (float)(NUMERO_CICLOS_ONLINE - 1));
							printf("\nRssi Médio , %f\n", nodes_info_arr[i].average_rssi);
							
							total_msg = total_msg + nodes_info_arr[i].msg_rec;
							uplink_lost_packets += nodes_info_arr[i].msg_not_rec;
						}
						if(assTimeSlot > 0 && total_msg > 0)
						{
							PLR = 1 - total_msg / expected_messages;
							PER = uplink_lost_packets / expected_messages;
							printf("\nPLR , %.3f\nPER , %.3f, nodos associados %d", PLR, PER, assTimeSlot);
							
						}
					}
					break;
				}
				case APP_PAN_STATE_ONLINE_PREPARE_ACK_GROUP:
				{
					for(int i = 0; i < 32; i++)
					{
						ACKFrame.ackFlags[i] = 0;
						retransmit_ts_array[i] = 0;
					}
					retransmit_ts_array_counter = 0;
					ACKFrame_size = 0;
					
					msgReq.dstAddr		= 0;
					msgReq.dstEndpoint	= APP_BEACON_ENDPOINT;
					msgReq.srcEndpoint	= APP_BEACON_ENDPOINT;
					msgReq.options		= NWK_OPT_LLDN_ACK;
					msgReq.data	= (uint8_t *)&ACKFrame;
					msgReq.size	= sizeof(uint8_t)*(macLLDNRetransmitTS + 1);

					appState = APP_STATE_IDLE;
					appPanState = APP_PAN_STATE_IDLE;
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
			if(ack_received && rec_beacon.confSeqNumber == 0 && associated == 0 && STATE != ONLINE_MODE) {
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
		case APP_STATE_WAKEUP_AND_SEND:
		{
			NWK_WakeupReq();
			NWK_DataReq(&msgReqData);
			#if COOP_RT
			#else
			appState			= APP_STATE_IDLE;
			#endif
			break;
		}
		case APP_STATE_WAKEUP_AND_WAIT:
		{
			NWK_WakeupReq();
			appState			= APP_STATE_SLEEP_PREPARE;
		}
		case APP_STATE_RETRANSMIT_DATA:
		{
			if(!ack_received)
			{
				appState = APP_STATE_IDLE;
				
				int retransmition_slot = 0;
				
				
				for(int i = 0; i < assTimeSlot - 1 && i < (rec_beacon.NumberOfBaseTimeslotsinSuperframe - 3)/2; i++) // -3 because of 2 mgmt and 1 gack
					if( !check_ack(i) )
						retransmition_slot++;
					
				printf("\nretransmition_slot %d", retransmition_slot);
				
				if(retransmition_slot == 0)
				{
					appSendData();
					// appState = APP_STATE_SEND;			
				}
				else
				{
					#if MASTER_MACSC
					macsc_enable_manual_bts();
					macsc_set_cmp1_int_cb(send_message_timeHandler);
					macsc_enable_cmp_int(MACSC_CC1);
					macsc_use_cmp(MACSC_RELATIVE_CMP, ts_time * retransmition_slot, MACSC_CC1);
					#endif
				}
				
			}
			else
			{
				appState = APP_STATE_IDLE;
			}
			
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
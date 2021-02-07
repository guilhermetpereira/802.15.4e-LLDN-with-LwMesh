/*
 * lldn.h
 *
 * Created: 11/4/2019 7:34:49 PM
 *  Author: guilh
 */ 


#ifndef LLDN_H_
#define LLDN_H_

typedef enum {
	APP_STATE_INITIAL,
	APP_STATE_IDLE,
	APP_STATE_SEND,
	APP_STATE_ATT_PAN_STATE,
	#if !APP_COORDINATOR
	APP_STATE_PREP_DISC_REPONSE,
	APP_STATE_PREP_CONFIG_STATUS,
	APP_STATE_SLEEP_PREPARE,
	APP_STATE_RETRANSMIT_DATA,
	APP_STATE_SLEEP,
	APP_STATE_WAKEUP_AND_SEND,
	APP_STATE_WAKEUP_AND_WAIT
	#endif
} AppState_t;

#define COOP_RT 0

#if APP_COORDINATOR
	#define DELAY 75 // symbols
	#define MacLLDNMgmtTS 0x01

	#define NUMERO_CICLOS_ONLINE 5
	#define GROUP_ACK 0


	typedef enum {
		APP_PAN_STATE_RESET,
		APP_PAN_STATE_DISC_INITIAL,
		APP_PAN_STATE_DISC_SECOND_BE,
		APP_PAN_STATE_DISC_PREPARE_ACK,
		APP_PAN_STATE_CONFIG_INITIAL,
		APP_PAN_STATE_SEND_CONF_REQUEST,
		APP_PAN_STATE_CONFIG_SECOND_BEACON,
		APP_PAN_STATE_CONFIG_THIRD_BEACON,
		APP_PAN_STATE_ONLINE_INITIAL,
		APP_PAN_STATE_ONLINE_END_BE,
		APP_PAN_STATE_ONLINE_PREPARE_ACK,
		APP_PAN_STATE_CHECK_TS,
		APP_PAN_STATE_ONLINE_PREPARE_ACK_GROUP,
		APP_PAN_STATE_IDLE,

	} AppPanState_t;
	
	typedef struct nodes_info_t{
		uint8_t assigned_time_slot;
		uint16_t mac_addr;
		uint8_t req_timeslot_duration;
		
		uint8_t rssi;
		double average_rssi;
		
		uint8_t neighbors[50]; // size limited by hardware
		unsigned int num_neighbors;
		
		double tx_success;
		double tx_success_neigh;
		
		unsigned int msg_rec;
		unsigned int msg_not_rec;
		// uint8_t DATA_PAYLOAD[127];
		bool coop;
		
	}nodes_info_t;

	typedef struct msg_info_t{
		uint16_t mac_addr;
		uint16_t coop_addr;
		uint8_t size;
		uint8_t data_payload[NWK_FRAME_MAX_PAYLOAD_SIZE];
	}msg_info_t;
	
	typedef struct nodes_info_list_t{
		nodes_info_t *node;
		struct nodes_info_list *next;
	}nodes_info_list_t;
	
#else
	#define DISC_MODE	0b100
	#define CONFIG_MODE 0b110
	#define RESET_MODE	0b111
	#define ONLINE_MODE 0b000
	#define SYNC_TIMER  0b1000
#endif



#define LL_DISCOVER_RESPONSE		0x0d
#define LL_CONFIGURATION_STATUS		0x0e
#define LL_CONFIGURATION_REQUEST	0x0f

// payload structure for Discovery Response Frame
typedef struct NWK_DiscoverResponse_t {
	uint8_t id;
	uint8_t macAddr;
	struct{
		uint8_t tsDuration	 : 7;
		uint8_t dirIndicator : 1;
	}ts_dir;
} NWK_DiscoverResponse_t;

// payload structure for Configuration Status Frame
	typedef struct NWK_ConfigStatus_t {
		uint8_t id;
		uint8_t s_macAddr;
		uint8_t assTimeSlot;
		uint8_t macAddr;
		struct{
			uint8_t tsDuration 		: 7;
			uint8_t dirIndicator 	: 1;
		}ts_dir;
	} NWK_ConfigStatus_t;

// payload structure for Configuration Request Frame
typedef struct NWK_ConfigRequest_t {
	uint8_t id;
	uint8_t s_macAddr;
	uint8_t tx_channel;
	uint8_t assTimeSlot;
	uint8_t macAddr;
	struct{
		uint8_t tsDuration	: 7;
		uint8_t macLLDNmgmtTS 	: 1;
	} conf;
} NWK_ConfigRequest_t;

typedef struct NWK_ACKFormat_t{
	uint8_t sourceId;
	// 127: maximum size avaible on buffer
	// 4: size of NwkFrameGeneralHeaderLLDN_t
	uint8_t ackFlags[32];
} NWK_ACKFormat_t;



#endif /* LLDN_H_ */
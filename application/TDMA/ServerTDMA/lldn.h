/*
 * lldn.h
 *
 * Created: 11/4/2019 7:34:49 PM
 *  Author: guilh
 */ 


#ifndef LLDN_H_
#define LLDN_H_

typedef enum AppState_t {
	APP_STATE_INITIAL,
	APP_STATE_IDLE,
	APP_STATE_SEND,
	APP_STATE_ATT_PAN_STATE,
	#if !APP_COORDINATOR
	APP_STATE_PREP_DISC_REPONSE,
	APP_STATE_PREP_CONFIG_STATUS,
	#endif
} AppState_t;

#define LL_DISCOVER_RESPONSE		0x0d
#define LL_CONFIGURATION_STATUS		0x0e
#define LL_CONFIGURATION_REQUEST	0x0f

// payload structure for Discovery Response Frame
typedef struct NWK_DiscoverResponse_t {
	uint8_t id;
	uint16_t macAddr;
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
		uint16_t macAddr;
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
	uint16_t macAddr;
	struct{
		uint8_t tsDuration	: 7;
		uint8_t mgmtFrames 	: 1;
	} conf;
} NWK_ConfigRequest_t;

typedef struct NWK_ACKFormat_t{
	uint8_t sourceId;
	// 127: maximum size avaible on buffer
	// 4: size of NwkFrameGeneralHeaderLLDN_t
	uint8_t ackFlags[32];
} NWK_ACKFormat_t;


#if APP_COORDINATOR
	#define DELAY 30 // symbols

	typedef enum AppPanState_t {
		APP_PAN_STATE_IDLE,
		APP_PAN_STATE_RESET,
		APP_PAN_STATE_DISC_INITIAL,
		APP_PAN_STATE_DISC_SECOND_BE,
		APP_PAN_STATE_DISC_PREPARE_ACK,
		
	} AppPanState_t;
	
	typedef struct infoNode{
		uint8_t s_macAddr;
		uint8_t assTimeSlot;
		uint16_t macAddr;
		uint8_t tsDuration;
	};
	
#else
	#define DISC_MODE	0b100
	#define CONFIG_MODE 0b110
	#define RESET_MODE	0b111
#endif



#endif /* LLDN_H_ */
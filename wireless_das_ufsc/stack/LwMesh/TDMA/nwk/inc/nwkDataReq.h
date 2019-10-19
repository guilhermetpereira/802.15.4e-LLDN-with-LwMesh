/**
 * \file nwkDataReq.h
 *
 * \brief NWK_DataReq() interface
 *
 * Copyright (C) 2014 Atmel Corporation. All rights reserved.
 *
 * \asf_license_start
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. The name of Atmel may not be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * 4. This software may only be redistributed and used in connection with an
 *    Atmel microcontroller product.
 *
 * THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * \asf_license_stop
 *
 *
 */

/*
 * Copyright (c) 2014, Atmel Corporation All rights reserved.
 *
 * Licensed under Atmel's Limited License Agreement --> EULA.txt
 */

#ifndef _NWK_DATA_REQ_H_
#define _NWK_DATA_REQ_H_

/*- Includes ---------------------------------------------------------------*/
#include <stdint.h>
#include <stdbool.h>
#include "sysConfig.h"

/*- Definitions ------------------------------------------------------------*/

#define LL_DISCOVERY_RESPONSE 		0x0d
#define LL_CONFIGURATION_STATUS 	0x0e
#define LL_CONFIGURATION_REQUEST 	0x0f

/*- Types ------------------------------------------------------------------*/
enum {
	NWK_OPT_ACK_REQUEST           = 1 << 0,
	NWK_OPT_ENABLE_SECURITY       = 1 << 1,
	NWK_OPT_BROADCAST_PAN_ID      = 1 << 2,
	NWK_OPT_LINK_LOCAL            = 1 << 3,
	NWK_OPT_MULTICAST             = 1 << 4,
	NWK_OPT_BEACON                = 1 << 5,
	NWK_OPT_LLDN_BEACON			  = 1 << 6,
	NWK_OPT_LLDN_BEACON_ONLINE	  = 1 << 7,
	NWK_OPT_LLDN_BEACON_DISCOVERY = 1 << 8,
	NWK_OPT_LLDN_BEACON_CONFIG	  = 1 << 9,
	NWK_OPT_LLDN_BEACON_RESET	  = 1 << 10,
	NWK_OPT_LLDN_BEACON_SECOND	  = 1 << 11,
	NWK_OPT_LLDN_BEACON_THIRD	  = 1 << 12,
	NWK_OPT_LLDN_DATA 			  = 1 << 13,
	NWK_OPT_LLDN_ACK 			  = 1 << 14,
	NWK_OPT_MAC_COMMAND			  = 1 << 15,
};

typedef struct NWK_DataReq_t {
	/* service fields */
	void *next;
	void *frame;
	uint8_t state;

	/* request parameters */
	uint16_t dstAddr;
	uint8_t dstEndpoint;
	uint8_t srcEndpoint;
	uint16_t options;
#ifdef NWK_ENABLE_MULTICAST
	uint8_t memberRadius;
	uint8_t nonMemberRadius;
#endif
	uint8_t *data;
	uint8_t size;
	void (*confirm)(struct NWK_DataReq_t *req);

	/* confirmation parameters */
	uint8_t status;
	uint8_t control;
} NWK_DataReq_t;


// payload structure for Discovery Response Frame
typedef struct NWK_DiscoveryResponse_t {
	uint8_t identifier;
	uint16_t macAddr;
	struct{
		uint8_t tsDuration	 : 7;
		uint8_t dirIndicator : 1;
	}ts_dir;
} NWK_DiscoveryResponse_t;

// payload structure for Configuration Status Frame
typedef struct NWK_ConfigStatus_t {
	uint8_t identifier;
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
	uint8_t identifier;
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
	uint8_t ackFlags[127 - 4];
} NWK_ACKFormat_t;


/*- Prototypes -------------------------------------------------------------*/
void NWK_DataReq(NWK_DataReq_t *req);

void nwkDataReqInit(void);
void nwkDataReqTaskHandler(void);

#endif /* _NWK_DATA_REQ_H_ */

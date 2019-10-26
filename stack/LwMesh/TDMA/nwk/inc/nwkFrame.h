/**
 * \file nwkFrame.h
 *
 * \brief Frame buffers management interface
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

#ifndef _NWK_FRAME_H_
#define _NWK_FRAME_H_

/*- Includes ---------------------------------------------------------------*/
#include <stdint.h>
#include "compiler.h"

/*- Definitions ------------------------------------------------------------*/
#define NWK_FRAME_MAX_PAYLOAD_SIZE   127

/*- Types ------------------------------------------------------------------*/
COMPILER_PACK_SET(1)
typedef struct  NwkFrameHeader_t {
	uint16_t macFcf;
	uint8_t macSeq;
	uint16_t macDstPanId;
	uint16_t macDstAddr;
	uint16_t macSrcAddr;

	struct {
		uint8_t ackRequest : 1;
		uint8_t security   : 1;
		uint8_t linkLocal  : 1;
		uint8_t multicast  : 1;
		uint8_t beacon     : 1;
		uint8_t reserved   : 3;
	} nwkFcf;
	uint8_t nwkSeq;
	uint16_t nwkSrcAddr;
	uint16_t nwkDstAddr;
	struct {
		uint8_t nwkSrcEndpoint : 4;
		uint8_t nwkDstEndpoint : 4;
	};
} NwkFrameHeader_t;

typedef struct  NwkFrameBeaconHeader_t {
	uint16_t macFcf;
	uint8_t macSeq;
	uint16_t macSrcPanId;
	uint16_t macSrcAddr;
	struct
	{
		uint16_t beaconOrder			: 4;
		uint16_t superframeOrder		: 4;
		uint16_t finalCAPslot			: 4;
		uint16_t BatteryLifeExtension	: 1;
		uint16_t Reserved				: 1;
		uint16_t PANCoordinator			: 1;
		uint16_t AssociationPermit		: 1;
	} macSFS;
	uint8_t macGTS;
	uint8_t macPending;
} NwkFrameBeaconHeader_t;


typedef struct  NwkFrameBeaconHeaderLLDN_t {
	/******* MHR *****/
	// struct
	// {
	// 	uint8_t FrameType 				: 3;
	// 	uint8_t SecurityEnabled 	: 1;
	// 	uint8_t FrameVersion			: 1;
	// 	uint8_t ackRequest				: 1;
	// 	uint8_t SubFrameType			: 2;
	// } macFcf;
	uint8_t macFcf;
	uint8_t macSeqNumber;
	struct
	{
		uint8_t secLevel 	: 3;
		uint8_t KeyId			: 2;
		uint8_t countSup 	: 1;
		uint8_t countSize	: 1;
		uint8_t reserved 	: 1;
	} macSecHeader;
	/****** MAC Payload *******/
	struct
	{
		uint8_t txState 				: 3;
		uint8_t txDir 					: 1;
		uint8_t reserved				: 1;
		uint8_t numMgmtTimeslots: 3; // number of managment time slots
		// uplink and downlink must be equal
	} Flags;

	uint8_t PanId;
	uint8_t confSeqNumber;
	uint8_t TimeSlotSize;


} NwkFrameBeaconHeaderLLDN_t;

typedef struct  NwkFrameGeneralHeaderLLDN_t {
	uint8_t macFcf;
	uint8_t macSeqNumber;
	struct
	{
		uint8_t secLevel 	: 3;
		uint8_t KeyId			: 2;
		uint8_t countSup 	: 1;
		uint8_t countSize	: 1;
		uint8_t reserved 	: 1;
	} macSecHeader;
} NwkFrameGeneralHeaderLLDN_t;


typedef struct  NwkFrameMulticastHeader_t {
	uint16_t nonMemberRadius    : 4;
	uint16_t maxNonMemberRadius : 4;
	uint16_t memberRadius       : 4;
	uint16_t maxMemberRadius    : 4;
} NwkFrameMulticastHeader_t;

typedef struct NwkFrame_t {
	uint8_t state;
	uint8_t size;

	union {
		NwkFrameHeader_t header;
		NwkFrameBeaconHeader_t beacon;
		NwkFrameBeaconHeaderLLDN_t LLbeacon;
		NwkFrameGeneralHeaderLLDN_t LLgeneral;
		uint8_t data[NWK_FRAME_MAX_PAYLOAD_SIZE];
	};

	uint8_t *payload;

	union {
		struct {
			uint8_t lqi;
			int8_t rssi;
		} rx;

		struct {
			uint8_t status;
			uint16_t timeout;
			uint8_t control;
			void (*confirm)(struct NwkFrame_t *frame);
		} tx;
	};
} NwkFrame_t;
COMPILER_PACK_RESET()
/*- Prototypes -------------------------------------------------------------*/
void nwkFrameInit(void);
NwkFrame_t *nwkFrameAlloc(void);
void nwkFrameFree(NwkFrame_t *frame);
NwkFrame_t *nwkFrameNext(NwkFrame_t *frame);
void nwkFrameCommandInit(NwkFrame_t *frame);

NwkFrame_t *nwkFrameAlloc_LLDN(bool beacon_frame);
/*- Implementations --------------------------------------------------------*/

/*************************************************************************//**
*****************************************************************************/
static inline uint8_t nwkFramePayloadSize(NwkFrame_t *frame)
{
	return frame->size - (frame->payload - frame->data);
}

#endif /* _NWK_FRAME_H_ */
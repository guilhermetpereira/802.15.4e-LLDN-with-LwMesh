/**
 * \file config.h
 *
 * \brief WSNDemo application and stack configuration
 *
 * Copyright (C) 2012-2013, Atmel Corporation. All rights reserved.
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
 * $Id: config.h 7863 2013-05-13 20:14:34Z ataradov $
 *
 */

#ifndef _CONFIG_H_
#define _CONFIG_H_

#include "board.h"

/*- Definitions ------------------------------------------------------------*/
#define APP_PAN_ADDR										0x0000
#define APP_PANID											0xCAFE
#define APP_SENDING_INTERVAL								2000
#define APP_BEACON_ENDPOINT									0
#define APP_DATA_ENDPOINT									1
#define APP_OTA_ENDPOINT									2
#define APP_COMMAND_ENDPOINT								3
#define APP_ACK_ENDPOINT									4	
#define APP_SECURITY_KEY									"TestSecurityKey0"

#ifdef PHY_AT86RF212
  #define APP_CHANNEL										0x01
  #define APP_BAND											0x00
  #define APP_MODULATION									0x24
#else
  #define APP_CHANNEL										0x0F
#endif

#define BI_COEF												(8)
#define SD_COEF												(6)
#define FINAL_CAP_SLOT										(1)
#define SYMBOL_TIME											0.000016						// (seconds per symbol)
#define ABASEFRAMEDURATION									(960ul)							// Symbols
#define BI_EXPO												(256)
#define SD_EXPO												(50)
#define BEACON_INTERVAL_BI									(ABASEFRAMEDURATION * BI_EXPO)	// 4s
#define SUPERFRAME_DURATION_SD								(ABASEFRAMEDURATION * SD_EXPO)	// 0.768s
#define TDMA_SLOT_PERIOD									0.05							// s
#define TDMA_FIRST_SLOT										(3125)							// Symbols
#define TDMA_BATTERY_EXTENSION								1

// Values of table 3e 802.15.4e-2012
#define p_var	6.0	// octets
#define m	3		// octets
#define sp  2		// symbols per octet
#define sm  2		// symbols per octet
#define v_var (62500)	// symbols per second
#define macMinSIFSPeriod	 (12) // symbols
#define macMinLIFSPeriod	 (40) // 40 symbols
#define aMaxSIFSFrameSize	 (18) // 18 symbols

uint8_t n; // Expected maximum number of octets of data payload
static float tTS;

#define numMgmtTs_Disc_Conf  2  // discovery and configuration states has 2 TimeSlots in Managment (one uplink, one downlink)
 
 /* MAC Command frames identifier defined by Table 5 : 802.15.4e - 2012 */
#define LL_DISCOVER_RESPONSE		0x0d
#define LL_CONFIGURATION_STATUS		0x0e
#define LL_CONFIGURATION_REQUEST	0x0f
// #define	LL_CTS_SHARED_GROUP			0x10
// #define	LL_RTS						0x11
// #define	LL_CTS						0x12

#define HAL_ENABLE_USB										0
#define HAL_ENABLE_UART										1
#define HAL_UART_CHANNEL									0
#define HAL_UART_RX_FIFO_SIZE								1
#define HAL_UART_TX_FIFO_SIZE								100

#define PHY_ENABLE_RANDOM_NUMBER_GENERATOR
//#define PHY_ENABLE_ENERGY_DETECTION
//#define PHY_ENABLE_AES_MODULE

#define SYS_SECURITY_MODE									0

#define NWK_BUFFERS_AMOUNT									20
#define NWK_DUPLICATE_REJECTION_TABLE_SIZE					50
#define NWK_DUPLICATE_REJECTION_TTL							2000				// ms
#define NWK_ROUTE_TABLE_SIZE								100
#define NWK_ROUTE_DEFAULT_SCORE								3
#define NWK_ACK_WAIT_TIME									1000				// ms
#define NWK_GROUPS_AMOUNT									3
#define NWK_ROUTE_DISCOVERY_TABLE_SIZE						5
#define NWK_ROUTE_DISCOVERY_TIMEOUT							1000				// ms
#define APP_RX_BUF_SIZE										20

//#define NWK_ENABLE_ROUTING
//#define NWK_ENABLE_SECURITY
#define NWK_ENABLE_MULTICAST
//#define NWK_ENABLE_ROUTE_DISCOVERY
//#define NWK_ENABLE_SECURE_COMMANDS

////////////////////////////////////////////////////////////////////////////////
// "You must put APP_ADDR=address into make call"
////////////////////////////////////////////////////////////////////////////////
#if APP_ADDR == 0
	#define APP_CAPTION     "Coordinator"
	#define APP_NODE_TYPE   0
	#define APP_COORDINATOR 1
	#define APP_ROUTER      0
	#define APP_ENDDEVICE   0
#else
	#define APP_CAPTION     "End Device"
	#define APP_NODE_TYPE   2
	#define APP_COORDINATOR 0
	#define APP_ROUTER      0
	#define APP_ENDDEVICE   1
#endif

#ifndef LED_COUNT
	#define LED_COUNT		0
#endif

#if LED_COUNT > 2
	#define LED_NETWORK						LED0_GPIO
	#define LED_DATA						LED1_GPIO
	#define LED_BLINK						LED2_GPIO
	#define LED_IDENTIFY					LED2_GPIO
#elif LED_COUNT == 2
	#define LED_NETWORK						LED0_GPIO
	#define LED_DATA						LED1_GPIO
	#define LED_BLINK						LED1_GPIO
	#define LED_IDENTIFY					LED0_GPIO
#elif LED_COUNT == 1
	#define LED_NETWORK						LED0_GPIO
	#define LED_DATA						LED0_GPIO
	#define LED_BLINK						LED0_GPIO
	#define LED_IDENTIFY					LED0_GPIO
#endif

#ifdef LED0_ACTIVE_LEVEL
	#define LED_NETWORK_GPIO				LED_NETWORK
	#define LED_DATA_GPIO					LED_DATA
	#define LED_BLINK_GPIO					LED_BLINK
	#define LED_IDENTIFY_GPIO				LED_IDENTIFY
	#define LED_IDENTIFY_ACTIVE_LEVEL		LED0_ACTIVE_LEVEL
	#define LED_IDENTIFY_INACTIVE_LEVEL		LED0_ACTIVE_LEVEL
	#define LED_NETWORK_ACTIVE_LEVEL		LED0_INACTIVE_LEVEL
	#define LED_NETWORK_INACTIVE_LEVEL		LED0_INACTIVE_LEVEL
	#define LED_DATA_ACTIVE_LEVEL			LED0_ACTIVE_LEVEL
	#define LED_DATA_INACTIVE_LEVEL			LED0_INACTIVE_LEVEL
	#define LED_BLINK_ACTIVE_LEVEL			LED0_ACTIVE_LEVEL
	#define LED_BLINK_INACTIVE_LEVEL		LED0_INACTIVE_LEVEL
#endif

#endif // _CONFIG_H_

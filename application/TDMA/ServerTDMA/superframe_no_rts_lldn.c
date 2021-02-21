/*
 * sleep_n_idle_states.c
 *
 * Created: 17/02/2021 19:51:19
 *  Author: Guilherme
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

#include "conf_sio2host.h"
#include "macsc_megarf.h"
#define MACSC_ABSOLUTE_CMP 0

#if APP_COORDINATOR
#define DELAY_INTERVAL 0.05 // (seconds)
#else
#define DELAY_INTERVAL 0.05 - 0.002 // (seconds)
#endif
uint32_t cmp_value;
static NWK_DataReq_t msgReq;


AppState_t appState = APP_STATE_INITIAL;

#if APP_COORDINATOR
static NWK_ACKFormat_t ACKFrame;
static NWK_DataReq_t msgReqGACK = { .dstAddr = 0,
									.dstEndpoint = APP_BEACON_ENDPOINT,
									.srcEndpoint = APP_BEACON_ENDPOINT,
									.options = NWK_OPT_LLDN_ACK,
									.data = (uint8_t*)&ACKFrame};
int assTimeSlot = 2;
int count = 10;


static void beacon_interval_hndlr(void)
{
	/* TOGGLE PIN HERE */
	if(count-- == 0)
		return;
	
	NWK_DataReq(&msgReq);
	macsc_enable_manual_bts();
	printf("\nb");
	return;
}
static void config_beacon(void)
{
	n = 75;
	tTS =  ((p_var*sp + (m + n)*sm + macMinLIFSPeriod)/v_var);
	
	macLLDNnumUplinkTS = (assTimeSlot) * 2 + 1;
	macLLDNRetransmitTS = assTimeSlot;
	macLLDNnumTimeSlots = macLLDNnumUplinkTS + 2 *MacLLDNMgmtTS;

	msgReq.dstAddr				= 0;
	msgReq.dstEndpoint			= APP_BEACON_ENDPOINT;
	msgReq.srcEndpoint			= APP_BEACON_ENDPOINT;
	msgReq.options				= NWK_OPT_LLDN_BEACON | NWK_OPT_ONLINE_STATE;
	msgReq.data					= NULL;
	msgReq.size					= 0;
	
	return;	
}

static void send_gack(void)
{
	if(count == 10)
		return;
	msgReqGACK.size = sizeof(uint8_t)*(macLLDNRetransmitTS + 1);
	NWK_DataReq(&msgReqGACK);
	printf("\ng");
	return;
}

static void config_tmrs(void)
{
	int beaconInterval= (macLLDNnumUplinkTS + 2*MacLLDNMgmtTS*numBaseTimeSlotperMgmt_online) * tTS / (SYMBOL_TIME);
	int GACK_tTS = (assTimeSlot + 0.2 + 2*MacLLDNMgmtTS*numBaseTimeSlotperMgmt_online) * tTS / (SYMBOL_TIME);
	
	
	macsc_set_cmp1_int_cb(beacon_interval_hndlr);
	macsc_set_cmp2_int_cb(send_gack);
	
	macsc_enable_cmp_int(MACSC_CC2);
	macsc_enable_cmp_int(MACSC_CC1);

	macsc_use_cmp(MACSC_RELATIVE_CMP, beaconInterval, MACSC_CC1);
	macsc_use_cmp(MACSC_RELATIVE_CMP, GACK_tTS, MACSC_CC2);
	macsc_enable_manual_bts();
}
#else
NwkFrameBeaconHeaderLLDN_t rec_beacon;
int assTimeSlot = 0;

uint8_t data_payload = APP_ADDR;
static NWK_DataReq_t msgReqData = { .dstAddr =0,
					.dstEndpoint = APP_COMMAND_ENDPOINT,
					.srcEndpoint = APP_COMMAND_ENDPOINT,
					.options = NWK_OPT_LLDN_DATA,
					.data = (uint8_t*)&data_payload,
					.size = sizeof(data_payload)};

	
static bool appBeaconInd(NWK_DataInd_t *ind)
{
	cmp_value = macsc_read_count();
	// 	printf("\nMSG : %x",cmp_value);
	
	rec_beacon = *(NwkFrameBeaconHeaderLLDN_t*)ind->data;
	printf("\nb");
	// 	PIND &= ~(1 << PIND7);
	appState = APP_STATE_PREP_TMR;
	return true;
}

static bool appAckInd(NWK_DataInd_t *ind)
{	
	// 	PIND &= ~(1 << PIND7);
	printf("\na");
	appState = APP_STATE_SLEEP_PREPARE;
	return true;
}

static void wake_up_hnldr(void)
{
	appState = APP_STATE_WAKEUP_AND_WAIT;
	return;
}

static void wake_n_send_hndlr(void)
{
	NWK_WakeupReq();
	NWK_DataReq(&msgReqData);
	printf("\ns");
	appState			= APP_STATE_SLEEP_PREPARE;
 	//appState = APP_STATE_WAKEUP_AND_SEND;
}
	

static void config_tmrs(void)
{
	
	tTS =  ((p_var*sp + (m+ rec_beacon.TimeSlotSize )*sm + macMinLIFSPeriod)/v_var)  / (SYMBOL_TIME);
	int beacon_interval = rec_beacon.NumberOfBaseTimeslotsinSuperframe*tTS;
	int send_msg = (2*rec_beacon.Flags.numBaseMgmtTimeslots + assTimeSlot) * tTS;
	int GACK_tTS = ((int)(rec_beacon.NumberOfBaseTimeslotsinSuperframe -2)/2 + 2*rec_beacon.Flags.numBaseMgmtTimeslots)*tTS;
	
	macsc_set_cmp1_int_cb(wake_up_hnldr);
	macsc_set_cmp2_int_cb(wake_n_send_hndlr);
	macsc_set_cmp3_int_cb(wake_up_hnldr);
	
	macsc_enable_cmp_int(MACSC_CC1);
	macsc_enable_cmp_int(MACSC_CC2);
	macsc_enable_cmp_int(MACSC_CC3);

	macsc_use_cmp(MACSC_ABSOLUTE_CMP, cmp_value + beacon_interval - 50, MACSC_CC1);
	macsc_use_cmp(MACSC_ABSOLUTE_CMP, cmp_value + send_msg - 50, MACSC_CC2);
	macsc_use_cmp(MACSC_ABSOLUTE_CMP, cmp_value + GACK_tTS - 80, MACSC_CC3);
	
	
	macsc_enable_manual_bts();

	return;
	
}

#endif



static void APP_TaskHandler(void)
{
	switch (appState){
		case APP_STATE_INITIAL:
		{
			/* Init Radio Module */ 
			NWK_SetAddr(APP_ADDR);
			PHY_SetChannel(APP_CHANNEL);
			PHY_SetRxState(true);
			PHY_SetTdmaMode(true);
			PHY_SetPromiscuousMode(true);

			#if APP_COORDINATOR
			config_beacon();
			config_tmrs();
			#else			
			NWK_OpenEndpoint(APP_BEACON_ENDPOINT, appBeaconInd);
			NWK_OpenEndpoint(APP_ACK_ENDPOINT, appAckInd);
			#endif
		
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
		#if !APP_COORDINATOR
		case APP_STATE_PREP_TMR:
		{
			config_tmrs();
			appState = APP_STATE_SLEEP_PREPARE;
			break;
		}	
		case APP_STATE_WAKEUP_AND_WAIT:
		{
			NWK_WakeupReq();
			appState			= APP_STATE_IDLE;
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

		sm_init();

		DDRD	= 0xFF;

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
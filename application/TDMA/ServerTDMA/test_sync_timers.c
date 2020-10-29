/*
 * test_sync_timers.c
 *
 * Created: 09/07/2020 22:48:19
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

#include "conf_sio2host.h"

#include "macsc_megarf.h"

typedef enum {
	APP_STATE_INITIAL,
	APP_STATE_IDLE,
} AppState_t;

AppState_t state = APP_STATE_INITIAL;
NwkFrameBeaconHeaderLLDN_t rec_beacon;
static NWK_DataReq_t msgReq;
static uint8_t PanId;
float beaconInterval_association = 0;
int counter = 0;
int media = 0;
uint32_t be_read = 0;
uint32_t anterior = 0;
uint32_t tmr_read= 0;
bool beacon_tmr = false;
int beacon_received = 0;
int error = 50;
int msg_wait_time;

static void lldn_server_beacon(void)
{
	counter++;
		NWK_DataReq(&msgReq);
	uint32_t tmp = macsc_read_count();
	printf("\nb, %" PRIu32 " ", tmp);
	if(counter < 20)
		macsc_enable_manual_bts();

}
static void time_slot(void)
{
	printf("\n-");
}


static void send(void)
{
	printf("S");
	macsc_set_cmp2_int_cb(0);

	NWK_DataReq(&msgReq);
}
static void node_time_handler(void)
{
	beacon_tmr = true;
	tmr_read = macsc_read_count();
	return;
}

static bool appBeaconInd(NWK_DataInd_t *ind)
{
	printf("\n%d ",counter);
	beacon_received = true;
	
	macsc_disable_cmp_int(MACSC_CC1);
	macsc_disable_cmp_int(MACSC_CC2);
	
		NWK_DataReq(&msgReq);

	
	NwkFrameBeaconHeaderLLDN_t *tmp_beacon = (NwkFrameBeaconHeaderLLDN_t*)ind->data;
	rec_beacon = *tmp_beacon;
	
	
	/*
	if(counter == 0 )
	{
		tTS =  ((p_var*sp + (m+ tmp_beacon->TimeSlotSize  )*sm + macMinLIFSPeriod)/v_var)  / (SYMBOL_TIME);
		msg_wait_time = tmp_beacon->Flags.numBaseMgmtTimeslots * tTS * 2;
		printf(" %d",  msg_wait_time );

 		macsc_set_cmp1_int_cb(node_time_handler);
		macsc_set_cmp2_int_cb(send);

		macsc_enable_cmp_int(MACSC_CC1);

		macsc_use_cmp(MACSC_RELATIVE_CMP, msg_wait_time, MACSC_CC1);
		
		
		macsc_enable_manual_bts();
	}
	else if(beacon_tmr)
	{
		beacon_tmr = false;
		be_read = macsc_read_count();
		int erro = be_read - tmr_read;
		if(erro > 35)
		{
			macsc_disable_cmp_int(MACSC_CC1);
			macsc_disable_cmp_int(MACSC_CC2);

			msg_wait_time+=10;
			macsc_enable_cmp_int(MACSC_CC1);
			macsc_use_cmp(MACSC_RELATIVE_CMP, msg_wait_time, MACSC_CC1);
			macsc_use_cmp(MACSC_RELATIVE_CMP, msg_wait_time/2, MACSC_CC2);

			macsc_enable_manual_bts();
		}
		else
		{
			printf(" OK");
			macsc_set_cmp2_int_cb(send);

			macsc_use_cmp(MACSC_RELATIVE_CMP, msg_wait_time/2, MACSC_CC2);
	
			macsc_enable_cmp_int(MACSC_CC1);			
			macsc_enable_cmp_int(MACSC_CC2);

			macsc_enable_manual_bts();
		}
	}
	else
	{
		macsc_disable_cmp_int(MACSC_CC1);
		macsc_disable_cmp_int(MACSC_CC2);
		
		msg_wait_time-=10;
		macsc_enable_cmp_int(MACSC_CC1);
		macsc_use_cmp(MACSC_RELATIVE_CMP, msg_wait_time, MACSC_CC1);
		macsc_enable_manual_bts();
	}
	counter++;*/
}

static bool appBeaconInd_2(NWK_DataInd_t *ind)
{

	uint32_t tmp = macsc_read_count();
	printf("\nd, %" PRIu32 " ", tmp);

}

static void appInit(void)
{
	NWK_SetAddr(APP_ADDR);
	PHY_SetChannel(APP_CHANNEL);
	PHY_SetRxState(true);
	n = 50;
			/* prepare beacon */
			msgReq.dstAddr				= 0;
			msgReq.dstEndpoint			= APP_BEACON_ENDPOINT;
			msgReq.srcEndpoint			= APP_BEACON_ENDPOINT;
			msgReq.options				= NWK_OPT_LLDN_BEACON | NWK_OPT_DISCOVERY_STATE;
			msgReq.data					= NULL;
			msgReq.size					= 0;
			
	#if APP_COORDINATOR	 
		/* 
		* Disable CSMA/CA
		* Disable auto ACK
		*/
		NWK_SetPanId(APP_PANID);
		PanId = APP_PANID;
		PHY_SetTdmaMode(true);
		
			
		tTS =  ((p_var*sp + (m+ n)*sm + macMinLIFSPeriod)/v_var);
		beaconInterval_association = 2 * numBaseTimeSlotperMgmt_association * (tTS) / (SYMBOL_TIME);
		macsc_set_cmp1_int_cb(lldn_server_beacon);
		macsc_set_cmp2_int_cb(time_slot);
		macsc_set_cmp3_int_cb(time_slot);

		
		macsc_enable_manual_bts();
		macsc_enable_cmp_int(MACSC_CC1);
// 		macsc_enable_cmp_int(MACSC_CC3);
// 
// 		macsc_enable_cmp_int(MACSC_CC2);

		macsc_use_cmp(MACSC_RELATIVE_CMP, beaconInterval_association , MACSC_CC1);
// 		macsc_use_cmp(MACSC_RELATIVE_CMP, beaconInterval_association/4 , MACSC_CC2);
// 		macsc_use_cmp(MACSC_RELATIVE_CMP, 2*beaconInterval_association/4 , MACSC_CC3);

		NWK_OpenEndpoint(APP_COMMAND_ENDPOINT, appBeaconInd_2);

		
		
	#else
		PHY_SetTdmaMode(false);
		
		NWK_OpenEndpoint(APP_BEACON_ENDPOINT, appBeaconInd);
		

	#endif // APP_COORDENATOR
	PHY_SetPromiscuousMode(true);

}


void APP_TaskHandler(void)
{
	switch(state)
	{
		case APP_STATE_INITIAL:
			appInit();
			state = APP_STATE_IDLE;
		break;
		default:
		break;
		
	}
}



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

	cpu_irq_enable();
	
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

	for(;;)
	{
		SYS_TaskHandler();
		APP_TaskHandler();
	}
}
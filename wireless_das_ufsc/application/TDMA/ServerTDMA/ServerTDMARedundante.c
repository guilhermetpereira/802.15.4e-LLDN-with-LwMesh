/*
 * ServerTDMA.c
 *
 * Created: 07/09/2014 16:50:52
 *  Author: nando
 */ 

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
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
#include "Solver.h"
#include "Energy.h"
#include "platform.h"

#if APP_COORDINATOR
	#if (SIO2HOST_CHANNEL == SIO_USB)
		/* Only ARM */
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

#if (MASTER_MACSC == 1)
	#include "macsc_megarf.h"
#else
static SYS_Timer_t				tmrBeaconInterval;			// Beacon
static SYS_Timer_t				tmrComputeData;				// Compute data
#endif

static volatile AppState_t		appState					= APP_STATE_INITIAL;
static SYS_Timer_t				tmrBlinkData;				// Feedback
static NWK_DataReq_t			msgReq;
static AppMessageFrame_t		msgFrame;

static NWK_DataReq_t			msgReqConnection;
static AppMessageFrame_t		msgFrameConnection;

#if (APP_COORDINATOR)
	static uint16_t				server_turn					= 1;
	static uint8_t				statistics_buffer[(MSG_SIZE_MAX + 1) * 2];
	static uint8_t				aux_buffer[(MSG_SIZE_MAX + 1)];
#endif
#if (APP_ENDDEVICE)
	static bool					connected;
#endif

static void tmrBlinkDataHandler(SYS_Timer_t *timer)
{
#if (LED_COUNT > 0)
	LED_Off(LED_DATA);
#endif

	(void)timer;
}
#if (APP_COORDINATOR)
static void toHexBuffer(uint8_t* out, uint8_t* in, uint16_t size)
{
	uint16_t		pos			= 0;
	for(uint16_t i = 0; i < size; ++i)
	{
		pos						+= sprintf(out + pos, "%02X", in[i]);
	}
	out[pos]					= NULL;
}
#if (MASTER_MACSC == 1)
static void tdma_server_beacon(void)
{
	macsc_enable_manual_bts();

	appState					= APP_STATE_SEND;
}
static void tdma_server_statistics(void)
{
	appState					= APP_STATE_SERVER_STATISTICS;
}
#else
static void tmr_tdma_server_beacon(SYS_Timer_t *timer)
{
	(void) timer;

	SYS_TimerStart(&tmrComputeData);
	
	appState					= APP_STATE_SEND;
}
static void tmr_tdma_server_statistics(SYS_Timer_t *timer)
{
	(void) timer;

	appState					= APP_STATE_SERVER_STATISTICS;
}
#endif
static void server_statistics(void)
{
	int	n_decoded				= solver_solve_system();
	int n_received				= solver_get_n_received();
	printf("R: %d, C: %d, S: %d\n", n_received, solver_get_n_colaborative(), n_decoded);
	
	for(uint8_t j = SOLVER_MSG_RECEIVED; j < SOLVER_MSG_MAX; ++j)
	{
		for(uint8_t i = 0; i < N_MOTES_COLLAB_MAX; ++i)
		{
			uint8_t* buffer			= solver_get_data(i, j);
			if(buffer)
			{
				--n_decoded;
				//toHexBuffer(statistics_buffer, buffer, MSG_SIZE_MAX);
				//printf("%s: Node[%02d]: %s\n", (j == SOLVER_MSG_RECEIVED ? "R" : "S"), i + 1, statistics_buffer);
				printf("%s: Node[%02d]: \n", (j == SOLVER_MSG_RECEIVED ? "R" : "S"), i + 1);
			}
		}
	}
	
	appState					= APP_STATE_IDLE;
}
#else
static void tdma_client_turn(void)
{
	appState					= APP_STATE_WAKEUP_AND_WAIT;
}
static void tdma_client_job(void)
{
	appState					= APP_STATE_WAKEUP_AND_SEND;
}
static void tdma_job_conf(NWK_DataReq_t *req)
{
	(void) req;

	appState					= APP_STATE_SLEEP_PREPARE;
}
static bool appBeaconInd(NWK_DataInd_t *ind)
{
	(void) ind;

#if (LED_COUNT > 0)
	//LED_Toggle(LED_BLINK);
	//LED_Off(LED_DATA);
#endif

	AppMessageFrame_t*	frame_struct= (AppMessageFrame_t*) ind->data;

	if(frame_struct->frameType == MSG_STATE_BEACON)
	{
		appState						= APP_STATE_SEND_PREPARE;
	}

	return true;
}
static void appSendPrepare(void)
{
	for(uint8_t i = 0; i < MSG_SIZE_MAX; ++i)
	{
		msgFrame.data.data_vector[i]	= rand();
	}
}
#endif
static void appSendData(void)
{
#if (LED_COUNT > 0)
	LED_On(LED_DATA);
	#if (APP_COORDINATOR)
		SYS_TimerStart(&tmrBlinkData);
	#endif
#endif

#if (APP_COORDINATOR)
	printf("\n\nTurn: %04d\n", server_turn++);
	energy_prepare_next_turn();
	solver_prepare_next_turn();
	energy_get_collab_vector(msgFrame.beacon.collab_vector);
	uint8_t aux_connected = energy_get_connected_vector(aux_buffer);
	toHexBuffer(statistics_buffer, aux_buffer, N_MOTES_COLLAB_MAX);
	printf("Connected: %02d, %s\n", aux_connected, statistics_buffer);

	toHexBuffer(statistics_buffer, msgFrame.beacon.collab_vector, N_COLLAB_VECTOR);
	printf("Collab Buffer: %s\n", statistics_buffer);
	
	NWK_DataReq(&msgReq);
#else
	if(connected)
	{
		// Utiliza o SLOT do TDMA para enviar dados somente apos conectar com o Coordenador
		NWK_DataReq(&msgReq);
	}
	else
	{
		NWK_DataReq(&msgReqConnection);
	}
#endif
}
static bool appDataInd(NWK_DataInd_t *ind)
{
#if (LED_COUNT > 0)
	LED_Toggle(LED_BLINK);
	//LED_Off(LED_DATA);
#endif
	energy_receive_statistics(ind);
	solver_received_data_frame(ind);

	AppMessageFrame_t*	frame_struct	= (AppMessageFrame_t*) ind->data;

	if(frame_struct->frameType == MSG_STATE_CONNECTION)
	{
#if (APP_COORDINATOR)
		// No coordenador, responder ao pedido de conexão.
		msgReqConnection.dstAddr = ind->srcAddr;
		NWK_DataReq(&msgReqConnection);
#else
		// No cliente, indicação de conexão aceita e que os dados podem ser enviados.
		connected				= true;
#endif
	}

	return true;
}
static void appInit(void)
{
	NWK_SetAddr(APP_ADDR);
	NWK_SetPanId(APP_PANID);
	PHY_SetChannel(APP_CHANNEL);
	PHY_SetRxState(true);

#if (LED_COUNT > 0)
	LED_On(LED_NETWORK);
#endif

#ifdef PHY_ENABLE_RANDOM_NUMBER_GENERATOR
	srand(PHY_RandomReq());
#endif

	tmrBlinkData.interval		= 50;
	tmrBlinkData.mode			= SYS_TIMER_INTERVAL_MODE;
	tmrBlinkData.handler		= tmrBlinkDataHandler;

	msgFrameConnection.frameType = MSG_STATE_CONNECTION;

	msgReqConnection.dstAddr	= BROADCAST;
	msgReqConnection.dstEndpoint = APP_DATA_ENDPOINT;
	msgReqConnection.srcEndpoint = APP_DATA_ENDPOINT;
	msgReqConnection.options	= NWK_OPT_LINK_LOCAL;
	msgReqConnection.data		= (uint8_t *)&msgFrameConnection;
	msgReqConnection.size		= sizeof(MsgState_t);
	msgReqConnection.confirm	= NULL;

#if (APP_COORDINATOR)
	server_turn					= 1;

	NWK_OpenEndpoint(APP_DATA_ENDPOINT, appDataInd);

	msgFrame.frameType			= MSG_STATE_BEACON;

	msgReq.dstAddr				= BROADCAST;
	msgReq.dstEndpoint			= APP_BEACON_ENDPOINT;
	msgReq.srcEndpoint			= APP_BEACON_ENDPOINT;
	msgReq.options				= NWK_OPT_BEACON;
	msgReq.data					= (uint8_t *)&msgFrame;
	msgReq.size					= sizeof(MsgState_t) + sizeof(AppMessageBeacon_t);
	msgReq.confirm				= NULL;

#if (MASTER_MACSC == 1)
	/*
	 * Configure interrupts callback functions
	 * overflow interrupt, compare 1,2,3 interrupts
	 */
	macsc_set_cmp1_int_cb(tdma_server_beacon);
	macsc_set_cmp2_int_cb(tdma_server_statistics);

	/*
	 * Configure MACSC to generate compare interrupts from channels 1,2,3
	 * Set compare mode to absolute, set compare value.
	 */
	macsc_enable_manual_bts();
	macsc_enable_cmp_int(MACSC_CC1);
	macsc_use_cmp(MACSC_RELATIVE_CMP, BEACON_INTERVAL_BI, MACSC_CC1);
	macsc_enable_cmp_int(MACSC_CC2);
	macsc_use_cmp(MACSC_RELATIVE_CMP, (SUPERFRAME_DURATION_SD * 3), MACSC_CC2);
#else
	tmrBeaconInterval.interval	= (BEACON_INTERVAL_BI * SYMBOL_TIME) * 1000;
	tmrBeaconInterval.mode		= SYS_TIMER_PERIODIC_MODE;
	tmrBeaconInterval.handler	= tmr_tdma_server_beacon;

	tmrComputeData.interval		= ((SUPERFRAME_DURATION_SD * 3) * SYMBOL_TIME) * 1000;
	tmrComputeData.mode			= SYS_TIMER_INTERVAL_MODE;
	tmrComputeData.handler		= tmr_tdma_server_statistics;
	
	SYS_TimerStart(&tmrBeaconInterval);
#endif
#else
	connected					= false;

	NWK_OpenEndpoint(APP_BEACON_ENDPOINT, appBeaconInd);
	NWK_OpenEndpoint(APP_DATA_ENDPOINT, appDataInd);

	msgFrame.frameType			= MSG_STATE_DATA;

	msgReq.dstAddr				= BROADCAST;
	msgReq.dstEndpoint			= APP_DATA_ENDPOINT;
	msgReq.srcEndpoint			= APP_DATA_ENDPOINT;
	msgReq.options				= NWK_OPT_LINK_LOCAL;
	msgReq.data					= (uint8_t *)&msgFrame;
	msgReq.size					= sizeof(MsgState_t) + sizeof(AppMessageData_t);
	msgReq.confirm				= tdma_job_conf;

	/*
	 * Configure interrupts callback functions
	 * overflow interrupt, compare 1,2,3 interrupts
	 */
	macsc_set_cmp1_int_cb(tdma_client_turn);		// Wake-up, wait beacon (synchronize)
	macsc_set_cmp2_int_cb(tdma_client_job);			// Do job & Sleep
	macsc_set_cmp3_int_cb(tdma_client_job);			// Do job & Sleep again

	/*
	 * Configure MACSC to generate compare interrupts from channels 1,2,3
	 * Set compare mode to absolute,set compare value.
	 */
	macsc_enable_auto_ts();
	macsc_enable_cmp_int(MACSC_CC1);
	macsc_use_cmp(MACSC_RELATIVE_CMP, BEACON_INTERVAL_BI - TDMA_FIRST_SLOT, MACSC_CC1);
	
	macsc_enable_cmp_int(MACSC_CC2);
	macsc_use_cmp(MACSC_RELATIVE_CMP, (TDMA_FIRST_SLOT * APP_ADDR), MACSC_CC2);

	macsc_enable_cmp_int(MACSC_CC3);
	macsc_use_cmp(MACSC_RELATIVE_CMP, (SUPERFRAME_DURATION_SD * 2) + (TDMA_FIRST_SLOT * APP_ADDR), MACSC_CC3);
#endif
}
/*************************************************************************//**
*****************************************************************************/
static void APP_TaskHandler(void)
{
	switch (appState)
	{
#if (APP_COORDINATOR)
		case APP_STATE_SERVER_STATISTICS:
		{
			server_statistics();
			break;
		}
#endif
		case APP_STATE_SEND:
		{
			appSendData();
			appState			= APP_STATE_SEND_BUSY_DATA;
			break;
		}
#if (APP_ENDDEVICE)
		case APP_STATE_WAKEUP_AND_SEND:
		{
			NWK_WakeupReq();
#if (LED_COUNT > 0)
			LED_On(LED_NETWORK);
#endif
			appState			= APP_STATE_SEND;
			break;
		}
		case APP_STATE_WAKEUP_AND_COLLAB:
		{
			NWK_WakeupReq();
#if (LED_COUNT > 0)
			LED_On(LED_NETWORK);
#endif
			appState			= APP_STATE_DO_COMPRESS;
			break;
		}
		case APP_STATE_WAKEUP_AND_SEND_COLLAB:
		{
			NWK_WakeupReq();
#if (LED_COUNT > 0)
			LED_On(LED_NETWORK);
#endif
			appState			= APP_STATE_SEND_COLLAB;
			break;
		}
		case APP_STATE_WAKEUP_AND_WAIT:
		{
			NWK_WakeupReq();
#if (LED_COUNT > 0)
	LED_On(LED_NETWORK);
#endif
			appState			= APP_STATE_IDLE;
			break;
		}
		case APP_STATE_SEND_PREPARE:
		{
			appSendPrepare();
			appState			= APP_STATE_SLEEP_PREPARE;
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
#if (LED_COUNT > 0)
	LED_Off(LED_NETWORK);
	LED_Off(LED_DATA);
	LED_Off(LED_BLINK);
#endif
			sleep_enable();
			sleep_enter();
			sleep_disable();
			break;
		}
#endif
		case APP_STATE_INITIAL:
		{
			energy_init();
			solver_init();
			appInit();
			appState			= APP_STATE_IDLE;
			break;
		}
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
	// Disable CSMA/CA
	// Disable auto ACK
	PHY_SetTdmaMode(true);
	sm_init();

	// Initialize interrupt vector table support.
#if 0
	irq_initialize_vectors();
#endif
	cpu_irq_enable();

#if APP_COORDINATOR
#if 0
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
#endif
#endif
	for(;;)
	{
		SYS_TaskHandler();
		APP_TaskHandler();
	}
}
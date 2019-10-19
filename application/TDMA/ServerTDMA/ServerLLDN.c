	/*
	 * ServerLLDN.c
	 *
	 * Created: 10/18/2019 5:15:37 PM
	 *  Author: guilh
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
	// #include "Solver.h"
	// #include "Energy.h"
	#include "platform.h"

typedef enum AppState_t {
	APP_STATE_INITIAL,
	APP_STATE_IDLE,
	APP_STATE_SEND_PREPARE,
	APP_STATE_SEND,
	APP_STATE_SEND_COLLAB,
	APP_STATE_SEND_BUSY_DATA,
	APP_STATE_SEND_BUSY_COLLAB,
	APP_STATE_SLEEP_PREPARE,
	APP_STATE_SLEEP,
	APP_STATE_WAKEUP_AND_WAIT,
	APP_STATE_WAKEUP_AND_COLLAB,
	APP_STATE_WAKEUP_AND_SEND,
	APP_STATE_WAKEUP_AND_SEND_COLLAB,
	APP_STATE_RECEIVE_COLLAB,
	APP_STATE_DO_COMPRESS,
	APP_STATE_SERVER_STATISTICS,
} AppState_t;


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

	#define HUMAM_READABLE			1
	#define seconds_3  3 / SYMBOL_TIME 


	#if (MASTER_MACSC == 1)
		#include "macsc_megarf.h"
	#else
	static SYS_Timer_t				tmrBeaconInterval;			// Beacon
	static SYS_Timer_t				tmrComputeData;				// Compute data
	#endif

	static volatile AppState_t		appState					= APP_STATE_INITIAL;
	// equation for tTS gives time in seconds, the division by SYMBOL_TIME changes to symbols for counter usage
	static float tTS =  ((p_var*sp + (m+n)*sm + macMinLIFSPeriod)/v_var) / SYMBOL_TIME ;
	static SYS_Timer_t tmrSendData;

	
	static void tmrSendDataHandler(SYS_Timer_t *timer)
	{
	  printf("\nSoftware Timer");
	}
  
  static void tdma_server_beacon(void)
	{
		macsc_enable_manual_bts();
		printf("\n Hardware Timer");
	}

	static void appInit(void)
	{
		tmrSendData.interval = 3000;
		tmrSendData.mode = SYS_TIMER_PERIODIC_MODE;
		tmrSendData.handler = tmrSendDataHandler;
	#if (MASTER_MACSC == 1)
	SYS_TimerStart(&tmrSendData);


	/*
	 * Configure interrupts callback functions
	 * overflow interrupt, compare 1,2,3 interrupts
	 */
	macsc_set_cmp1_int_cb(tdma_server_beacon);
	/*
	 * Configure MACSC to generate compare interrupts from channels 1,2,3
	 * Set compare mode to absolute, set compare value.
	 */
	macsc_enable_manual_bts();
	macsc_enable_cmp_int(MACSC_CC1);
	macsc_use_cmp(MACSC_RELATIVE_CMP, seconds_3, MACSC_CC1);
	#endif
	

	}


	static void APP_TaskHandler(void)
	{
		switch (appState){
			case APP_STATE_INITIAL:
			{
				appInit();
				appState = APP_STATE_IDLE;
				break;
			}
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
		// Disable CSMA/CA
		// Disable auto ACK
		PHY_SetTdmaMode(true);
		sm_init();

		// Initialize interrupt vector table support.
	#if (SIO2HOST_CHANNEL == SIO_USB)
		irq_initialize_vectors();
	#endif
		cpu_irq_enable();

	#if APP_COORDINATOR
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
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

	#define HUMAM_READABLE			1

	#if (MASTER_MACSC == 1)
		#include "macsc_megarf.h"
	#else
	static SYS_Timer_t				tmrBeaconInterval;			// Beacon
	static SYS_Timer_t				tmrComputeData;				// Compute data
	#endif

	static volatile AppState_t		appState					= APP_STATE_INITIAL;
	static SYS_Timer_t tmrSendData;

	static void tmrSendDataHandler(SYS_Timer_t *timer)
	{
	  printf("\nMESSAGE SENT");
	}
  

	static void appInit(void)
	{
		 // Set up Timer
	  tmrSendData.interval = 2500;
	  tmrSendData.mode = SYS_TIMER_PERIODIC_MODE;
	  tmrSendData.handler = tmrSendDataHandler;
	  SYS_TimerStart(&tmrSendData);

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
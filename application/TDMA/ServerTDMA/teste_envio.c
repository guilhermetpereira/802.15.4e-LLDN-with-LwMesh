/*
	* ServerLLDN.c
	*
	* Created: 10/18/2019 5:15:37 PM
	*  Author: guilherme
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



#if 1
#if (SIO2HOST_CHANNEL == SIO_USB)
/* Only ARM */
#include "hw_timer_lldn.h"
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

static SYS_Timer_t tmrDelay;	
AppState_t	appState = APP_STATE_INITIAL;

char scan = 'S';


static void tmrDelayHandler(SYS_Timer_t *timer)
{
	int B = 54;
	char hex_int[4];
	hex_int[0] = (B >> 24) & 0xFF;
	hex_int[1] = (B >> 16) & 0xFF;
	hex_int[2] = (B >> 8) & 0xFF;
	hex_int[3] = B & 0xFF;

	printf("S");
	uint8_t assigned_time_slot = 0x01;
	uint8_t assTimeSlot = 0x0a;
	uint8_t bitmap = 0x0f;
	printf("%hhx",assTimeSlot);
	for (int i = 0; i < assTimeSlot; i++)
	{
		printf("N%hhx%02x%02x%02x%02x",assigned_time_slot,(unsigned char)hex_int[3],
												(unsigned char)hex_int[2],
												(unsigned char)hex_int[1],
												(unsigned char)hex_int[0]);
		for (int j = 0; j <= assTimeSlot/8; j++)
		{
			printf("%02x",bitmap);
		}
	}
	printf("T");
	appState = APP_STATE_INITIAL;
}


static int readStrinig(char *receivedString, char finalChar, unsigned int maxNbBytes)
{
    // Number of characters read
    unsigned int    NbBytes=0;
    // Returned value from Read
    char            charRead;
	while (NbBytes<maxNbBytes)
	{
		// Read a character with the restant time
		scanf("%c",&receivedString[NbBytes]);


		// Check if this is the final char
		if (receivedString[NbBytes]==finalChar)
		{
			for (int i = 0; i < NbBytes; i++)
			{
				printf("%c",receivedString[i]);
			}
			// This is the final char, add zero (end of string)
			receivedString  [++NbBytes]=0;
			// Return the number of bytes read
			return NbBytes;
		}

		// The character is not the final char, increase the number of bytes read
		NbBytes++;

		// An error occured while reading, return the error number
		if (charRead<0) return charRead;
	}
	printf("f");
}
static void APP_TaskHandler(void)
{
	switch (appState){
		case APP_STATE_INITIAL:
		{
			SYS_TimerStart(&tmrDelay);
// 			char stringreceived[10];
// 			readStrinig(stringreceived,'d',10);
			appState = APP_STATE_IDLE;
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
		/* Disable CSMA/CA
		 * Disable auto ACK
		 * Enable Rx of LLDN Frame Type as described in 802.15.4e - 2012 
		 */

		sm_init();

		tmrDelay.interval = 3*1000;
		tmrDelay.mode = SYS_TIMER_INTERVAL_MODE;
		tmrDelay.handler = tmrDelayHandler;

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
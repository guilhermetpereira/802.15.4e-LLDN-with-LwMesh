/*
 * LLDN_APP.c
 *
 * Created: 10/16/2019 2:46:36 PM
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
//#include "board.h"
//#include "Solver.h"
//#include "Energy.h"
#include "platform.h"


#if (SIO2HOST_CHANNEL == SIO_USB)
		/* Only ARM */
		#include "stdio_usb.h"
		#define MASTER_MACSC	0
#else
		/* Only megarf series */
		#include "conf_sio2host.h"
		#define MASTER_MACSC	1
#endif


/*************************************************************************//**
*****************************************************************************/
static void APP_TaskHandler(void)
{

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
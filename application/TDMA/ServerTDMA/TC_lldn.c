/*
 * TC_lldn.c
 *
 * Created: 21/01/2020 17:18:49
 *  Author: guilh
 */ 

#include <compiler.h>
#include <parts.h>
#include "tc.h"
#include "genclk.h"
#include "TC_lldn_conf.h"
	
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

#if (SIO2HOST_CHANNEL == SIO_USB)
/* Only ARM */
#include "stdio_usb.h"
#define MASTER_MACSC	0
#else
/* Only megarf series */
#include "conf_sio2host.h"
#define MASTER_MACSC	1
#endif

static SYS_Timer_t tmrComputeData;	// Compute data		
tmr_callback_t tmr_callback_lldn = 0;
AppState_t	appState = APP_STATE_IDLE;
int first  = 1;

static void configure_NVIC(Tc *cmn_hw_timer, uint8_t cmn_hw_timer_ch);
static void tc_callback_lldn(void);

void timer_init(void);
void timer_enable_cc_interrupt(void);
void tc_compare_stop(void);


static void tmrComputeDataHandler(SYS_Timer_t *timer)
{
	printf("\nTimer_Software");
	if(first)
	{
	appState = APP_STATE_INITIAL;	
	first = 0;
	}
}

void hw_expiry_cb(void)
{
	printf("\nRC Compare Interrupt");
}

void hw_overflow_cb(void)
{
	printf("\nRC Ovf Interrupt");
}

void timer_stop(void)
{
	tc_stop(TMR, TMR_CHANNEL_ID);
}

void tc_delay (uint16_t compare_value)
{

	tc_write_rc(TMR, TMR_CHANNEL_ID, compare_value);
	timer_enable_cc_interrupt();
}

void timer_init(void)
{
	uint8_t tmr_mul;
	/* Configure clock service. */
	
	genclk_enable_config(8, GENCLK_SRC_RC32K, 1);
	sysclk_enable_peripheral_clock(TMR);

	tmr_mul = sysclk_get_peripheral_bus_hz(TMR) / 1000000;
	tmr_mul = tmr_mul >> 1;

	tc_init(TMR, TMR_CHANNEL_ID,
	TC_CMR_TCCLKS_TIMER_CLOCK1 | TC_CMR_WAVE |
	TC_CMR_WAVSEL_UP_NO_AUTO);

	/* Configure and enable interrupt on RC compare. */
	
	configure_NVIC(TMR, TMR_CHANNEL_ID);
	
	tc_get_status(TMR, TMR_CHANNEL_ID);
	// tc_enable_interrupt(TMR, TMR_CHANNEL_ID, TC_IER_COVFS);
	
	tc_compare_stop();
	
	tc_start(TMR, TMR_CHANNEL_ID);
	
	return;
}

void configure_NVIC(Tc *cmn_hw_timer, uint8_t cmn_hw_timer_ch)
{
	(void) cmn_hw_timer;
	
	switch (cmn_hw_timer_ch) 
	{
	case 0:
	NVIC_EnableIRQ(TC10_IRQn);
	break;

	case 1:
	NVIC_EnableIRQ(TC11_IRQn);
	break;

	case 2:
	NVIC_EnableIRQ(TC12_IRQn);
	break;
	default:
	break;
	}	
	tmr_callback_lldn = tc_callback_lldn;
	
}


void tc_callback_lldn(void)
{
	uint32_t ul_status;
	/* Read TC1 Status. */
	ul_status = tc_get_status(TMR, TMR_CHANNEL_ID);
	ul_status &= tc_get_interrupt_mask(TMR, TMR_CHANNEL_ID);
	if (TC_SR_CPCS == (ul_status & TC_SR_CPCS)) {
		hw_expiry_cb();
	}

	/* Overflow */
	if (TC_SR_COVFS == (ul_status & TC_SR_COVFS)) {
		hw_overflow_cb();
	}
}


void timer_enable_cc_interrupt(void)
{
	tc_get_status(TMR, TMR_CHANNEL_ID);
	tc_enable_interrupt(TMR, TMR_CHANNEL_ID, TC_IDR_CPCS);
}

void tc_compare_stop(void)
{
	tc_get_status(TMR, TMR_CHANNEL_ID);
	tc_disable_interrupt(TMR, TMR_CHANNEL_ID, TC_IDR_CPCS);
}

/**
 * \brief Interrupt handlers for TC10
 */
void TC10_Handler(void)
{
	if (tmr_callback_lldn) {
		printf("\nTC10_Handler");
		tmr_callback_lldn();
	}
}

void APP_TaskHandler(void)
{
		switch (appState)
		{
			case APP_STATE_INITIAL:
			{
				uint16_t delay = 1000;
				
				timer_init();
				tc_delay(delay);
				
				appState = APP_STATE_IDLE;
				break;
			}
			case APP_STATE_IDLE:
			{
				break;
			}
		}
}


int main (void)
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
	
	tmrComputeData.interval = 2000;
	tmrComputeData.mode = SYS_TIMER_PERIODIC_MODE;
	tmrComputeData.handler = tmrComputeDataHandler;
	SYS_TimerStart(&tmrComputeData);	
	
	
	for(;;)
	{
		SYS_TaskHandler();
		APP_TaskHandler();
	}
	
	return 0;
}
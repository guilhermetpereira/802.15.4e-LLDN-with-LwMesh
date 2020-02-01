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
tmr_callback_t tmr_callback;
AppState_t	appState = APP_STATE_IDLE;

static void configure_NVIC(Tc *cmn_hw_timer, uint8_t cmn_hw_timer_ch);
static void tc_callback(void);

void timer_init(void);
void timer_enable_cc_interrupt(void);
void tc_compare_stop(void);


static void tmrComputeDataHandler(SYS_Timer_t *timer)
{
	printf("\nTimer_Software");
	appState = APP_STATE_INITIAL;
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
	#if SAM4E
	tc_write_ra(TMR, TMR_CHANNEL_ID, compare_value);
	#else
	tc_write_rc(TMR, TMR_CHANNEL_ID, compare_value);
	#endif
	timer_enable_cc_interrupt();
}

void timer_init(void)
{
	uint8_t tmr_mul;
	/* Configure clock service. */
	#if SAM4L
	sysclk_enable_peripheral_clock(TMR);
	genclk_enable_config(8, GENCLK_SRC_RC1M, 30);
	#else
	sysclk_enable_peripheral_clock(ID_TC);
	#endif

	tmr_mul = sysclk_get_peripheral_bus_hz(TMR) / 1000000;
	tmr_mul = tmr_mul >> 1;

	#if SAM4L
		tc_init(TMR, TMR_CHANNEL_ID,
		TC_CMR_TCCLKS_TIMER_CLOCK2 | TC_CMR_WAVE |
		TC_CMR_WAVSEL_UP_NO_AUTO);
	#elif SAM4E
		tc_init(TMR, TMR_CHANNEL_ID,
		TC_CMR_TCCLKS_TIMER_CLOCK1 | TC_CMR_WAVE |
		TC_CMR_WAVSEL_UP_RC);
	#else
		tc_init(TMR, TMR_CHANNEL_ID,
		TC_CMR_TCCLKS_TIMER_CLOCK1 | TC_CMR_WAVE |
		TC_CMR_WAVSEL_UP);
	#endif

	/* Configure and enable interrupt on RC compare. */
	configure_NVIC(TMR, TMR_CHANNEL_ID);
	#if SAM4E
		tc_get_status(TMR, TMR_CHANNEL_ID);
		tc_enable_interrupt(TMR, TMR_CHANNEL_ID, TC_IER_CPCS);
		tc_write_rc(TMR, TMR_CHANNEL_ID, UINT16_MAX);
	#else
		tc_get_status(TMR, TMR_CHANNEL_ID);
		tc_enable_interrupt(TMR, TMR_CHANNEL_ID, TC_IER_COVFS);
	#endif
	tc_compare_stop();
	tc_start(TMR, TMR_CHANNEL_ID);
	return;
}

void configure_NVIC(Tc *cmn_hw_timer, uint8_t cmn_hw_timer_ch)
{
	if (TC0 == cmn_hw_timer) {
		switch (cmn_hw_timer_ch) {
			#if SAM4L
			case 0:
			NVIC_EnableIRQ(TC00_IRQn);
			break;

			case 1:
			NVIC_EnableIRQ(TC01_IRQn);
			break;

			case 2:
			NVIC_EnableIRQ(TC02_IRQn);
			break;

			#else
			case 0:
			NVIC_EnableIRQ(TC0_IRQn);
			break;

			case 1:
			NVIC_EnableIRQ(TC1_IRQn);
			break;

			case 2:
			NVIC_EnableIRQ(TC2_IRQn);
			break;
			#endif
			default:
			break;
		}
		#ifdef TC1
		} else if (TC1 == cmn_hw_timer) {
		switch (cmn_hw_timer_ch) {
			#if SAM4L
			case 0:
			NVIC_EnableIRQ(TC10_IRQn);
			break;				

			case 1:
			NVIC_EnableIRQ(TC11_IRQn);
			break;

			case 2:
			NVIC_EnableIRQ(TC12_IRQn);
			break;

			#else
			case 0:
			NVIC_EnableIRQ(TC3_IRQn);
			break;

			case 1:
			NVIC_EnableIRQ(TC4_IRQn);
			break;

			case 2:
			NVIC_EnableIRQ(TC5_IRQn);
			break;
			#endif
			default:
			break;
		}
		#endif
	}

	tmr_callback = tc_callback;
}


void tc_callback(void)
{
	uint32_t ul_status;
	/* Read TC0 Status. */
	ul_status = tc_get_status(TMR, TMR_CHANNEL_ID);
	ul_status &= tc_get_interrupt_mask(TMR, TMR_CHANNEL_ID);
	#if SAM4E
	if (TC_SR_CPAS == (ul_status & TC_SR_CPAS)) {
		hw_expiry_cb();
	}

	if (TC_SR_CPCS == (ul_status & TC_SR_CPCS)) {
		hw_overflow_cb();
	}

	#else
	if (TC_SR_CPCS == (ul_status & TC_SR_CPCS)) {
		hw_expiry_cb();
	}

	/* Overflow */
	if (TC_SR_COVFS == (ul_status & TC_SR_COVFS)) {
		hw_overflow_cb();
	}
	#endif
}


void timer_enable_cc_interrupt(void)
{
	tc_get_status(TMR, TMR_CHANNEL_ID);
	#if SAM4E
	tc_enable_interrupt(TMR, TMR_CHANNEL_ID, TC_IDR_CPAS);
	#else
	tc_enable_interrupt(TMR, TMR_CHANNEL_ID, TC_IDR_CPCS);
	#endif
}

void tc_compare_stop(void)
{
	tc_get_status(TMR, TMR_CHANNEL_ID);
	#if SAM4E
	tc_disable_interrupt(TMR, TMR_CHANNEL_ID, TC_IDR_CPAS);
	#else
	tc_disable_interrupt(TMR, TMR_CHANNEL_ID, TC_IDR_CPCS);
	#endif
}

/**
 * \brief Interrupt handlers for TC10
 */
#if SAM4L
void TC10_Handler(void)
#else
void TC3_Handler(void)
#endif
{
	if (tmr_callback) {
		tmr_callback();
	}
}

void APP_TaskHandler(void)
{
		switch (appState)
		{
			case APP_STATE_INITIAL:
			{
				printf("\nPRINT FUNCIONANDO");
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
	
	tmrComputeData.interval = 5000;
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
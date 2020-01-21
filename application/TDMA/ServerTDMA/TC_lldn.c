/*
 * TC_lldn.c
 *
 * Created: 21/01/2020 17:18:49
 *  Author: guilh
 */ 

#include <compiler.h>
#include <parts.h>
#include "tc.h"
#include "sysclk.h"
#include "genclk.h"
#include "TC_lldn_conf.h"
#include "stdio_usb.h"
		
tmr_callback_t tmr_callback;

static void configure_NVIC(Tc *cmn_hw_timer, uint8_t cmn_hw_timer_ch);
static void tc_callback(void);

void timer_init(void);
void timer_enable_cc_interrupt(void);
void tc_compare_stop(void);

void hw_expiry_cb(void)
{
	// printf("\nRC Compare Interrupt");
}

void hw_overflow_cb(void)
{
	// printf("\nRC Ovf Interrupt");
}

void timer_stop(void)
{
	tc_stop(TIMER, TIMER_CHANNEL_ID);
}

void tc_delay (uint16_t compare_value)
{
	#if SAM4E
	tc_write_ra(TIMER, TIMER_CHANNEL_ID, compare_value);
	#else
	tc_write_rc(TIMER, TIMER_CHANNEL_ID, compare_value);
	#endif
	timer_enable_cc_interrupt();
}

void timer_init(void)
{
	uint8_t tmr_mul;
	/* Configure clock service. */
	#if SAM4L
	genclk_enable_config(8, GENCLK_SRC_RC1M, 1);
	sysclk_enable_peripheral_clock(TIMER);
	#else
	sysclk_enable_peripheral_clock(ID_TC);
	#endif

	#if SAM4L
		tc_init(TIMER, TIMER_CHANNEL_ID,
		TC_CMR_TCCLKS_TIMER_CLOCK1 | TC_CMR_WAVE |
		TC_CMR_WAVSEL_UP_NO_AUTO);
	#elif SAM4E
		tc_init(TIMER, TIMER_CHANNEL_ID,
		TC_CMR_TCCLKS_TIMER_CLOCK1 | TC_CMR_WAVE |
		TC_CMR_WAVSEL_UP_RC);
	#else
		tc_init(TIMER, TIMER_CHANNEL_ID,
		TC_CMR_TCCLKS_TIMER_CLOCK1 | TC_CMR_WAVE |
		TC_CMR_WAVSEL_UP);
	#endif

	/* Configure and enable interrupt on RC compare. */
	configure_NVIC(TIMER, TIMER_CHANNEL_ID);
	#if SAM4E
		tc_get_status(TIMER, TIMER_CHANNEL_ID);
		tc_enable_interrupt(TIMER, TIMER_CHANNEL_ID, TC_IER_CPCS);
		tc_write_rc(TIMER, TIMER_CHANNEL_ID, UINT16_MAX);
	#else
		tc_get_status(TIMER, TIMER_CHANNEL_ID);
		tc_enable_interrupt(TIMER, TIMER_CHANNEL_ID, TC_IER_COVFS);
	#endif
	tc_compare_stop();
	tc_start(TIMER, TIMER_CHANNEL_ID);
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
	ul_status = tc_get_status(TIMER, TIMER_CHANNEL_ID);
	ul_status &= tc_get_interrupt_mask(TIMER, TIMER_CHANNEL_ID);
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
	tc_get_status(TIMER, TIMER_CHANNEL_ID);
	#if SAM4E
	tc_enable_interrupt(TIMER, TIMER_CHANNEL_ID, TC_IDR_CPAS);
	#else
	tc_enable_interrupt(TIMER, TIMER_CHANNEL_ID, TC_IDR_CPCS);
	#endif
}

void tc_compare_stop(void)
{
	tc_get_status(TIMER, TIMER_CHANNEL_ID);
	#if SAM4E
	tc_disable_interrupt(TIMER, TIMER_CHANNEL_ID, TC_IDR_CPAS);
	#else
	tc_disable_interrupt(TIMER, TIMER_CHANNEL_ID, TC_IDR_CPCS);
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

int main (void)
{
	return 0;
}
/*
 * TC_lldn.c
 *
 * Created: 21/01/2020 17:18:49
 *  Author: guilh
 */ 

#include "hw_timer_lldn.h"

static void configura_NVIC_lldn(Tc *cmn_hw_timer, uint8_t cmn_hw_timer_ch);
static void tc_callback_lldn(void);

void timer_enable_cc_interrupt(void);
void tc_compare_stop(void);

tmr_callback_t tmr_callback_lldn = 0;
tmr_callback_t callback = 0;
uint16_t delay = 0;

void setup_handler(tmr_callback_t callback_v)
{
	callback = callback_v;
}

void hw_expiry_cb(void)
{

	if(callback)
		callback();
}

void hw_overflow_cb(void)
{

	printf("\nRC Ovf Interrupt");
}

void timer_stop(void)
{
	tc_stop(TMR, TMR_CHANNEL_ID);
}

void tc_delay (uint16_t delay_v)
{
	delay = delay_v;
	tc_write_rc(TMR, TMR_CHANNEL_ID, delay);
	timer_enable_cc_interrupt();
}

void timer_start(void)
{
	tc_start(TMR, TMR_CHANNEL_ID);
}

void timer_init(void)
{	
	/* Configure clock service. */
	
	genclk_enable_config(8, GENCLK_SRC_RC1M, 7);
	sysclk_enable_peripheral_clock(TMR);

	tc_init(TMR, TMR_CHANNEL_ID,
	TC_CMR_TCCLKS_TIMER_CLOCK1 | TC_CMR_WAVE |
	TC_CMR_WAVSEL_UP_NO_AUTO);

	/* Configure and enable interrupt on RC compare. */
	
	configura_NVIC_lldn(TMR, TMR_CHANNEL_ID);
	
	tc_get_status(TMR, TMR_CHANNEL_ID);
	// tc_enable_interrupt(TMR, TMR_CHANNEL_ID, TC_IER_COVFS);
	
	tc_compare_stop();
	
	// tc_start(TMR, TMR_CHANNEL_ID);
	
	return;
}

void configura_NVIC_lldn(Tc *cmn_hw_timer, uint8_t cmn_hw_timer_ch)
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
		tmr_callback_lldn();
	}
}
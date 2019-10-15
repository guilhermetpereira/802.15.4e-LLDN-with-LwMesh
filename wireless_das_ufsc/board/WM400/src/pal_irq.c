/**
 * @file pal_irq.c
 *
 * @brief PAL IRQ functionality
 *
 * This file contains functions to initialize, enable, disable and install
 * handler for the transceiver interrupts.
 *
 * $Id: pal_irq.c 28470 2011-09-08 07:16:41Z mahendran.p $
 *
 */
/**
 * @author
 * Atmel Corporation: http://www.atmel.com
 * Support email: avr@atmel.com
 */
/*
 * Copyright (c) 2010, Atmel Corporation All rights reserved.
 *
 * Licensed under Atmel's Limited License Agreement --> EULA.txt
 */

/* === Includes ============================================================= */

#include <stdint.h>
#include "pal.h"
#include "pal_config.h"
#include "pal_boardtypes.h"


#if (BOARD_TYPE == WM400)

/* === Types ============================================================== */


/* === Globals ============================================================== */
/*
 * defining pins for trx interrupt
 */
static const pin_t pin_radio_irq		= TRX_INT_PIN;

/*
 * Function pointers to store the callback function of
 * the transceiver interrupt
 */
static irq_handler_t irq_hdl_trx;

/* === Prototypes =========================================================== */
static void trx_isr(void);

/* === Implementation ======================================================= */

/**
 * @brief Initializes the transceiver interrupts
 *
 * This function sets the microcontroller specific registers
 * responsible for handling the transceiver interrupts
 *
 * @param trx_irq_cb Callback function for the given transceiver
 * interrupt
 */
void pal_trx_irq_init(FUNC_PTR trx_irq_cb)
{
    /*
     * Set the handler function.
     * The handler is set before enabling the interrupt to prepare for
     * spurious interrupts, that can pop up the moment they are enabled.
     */
    irq_hdl_trx							= (irq_handler_t)trx_irq_cb;

    /* Configure the pin and interrupt */
    pal_pio_configure(&pin_radio_irq, PIO_LISTSIZE(pin_radio_irq));
    pal_pio_configure_interrupt(&pin_radio_irq, (void ( *)(const pin_t *))trx_isr);

    /* Configure the NVIC interrupt */
    NVIC_EnableIRQ(PIOB_IRQn);

    /*
     * The interrupt bit corresponding to the trx interrupt is cleared.
     */
    CLEAR_TRX_IRQ();
}


/**
 * @brief Handler for TRX interrupt
 *
 * This function is an handler for interrupt (transceiver interrupt) of the
 * microcontroller. It calls the user installed handler for transceiver
 * interrupt.
 */
void trx_isr(void)
{
    irq_hdl_trx();
}
#endif /* #if (BOARD_TYPE == WM400)*/
/* EOF */


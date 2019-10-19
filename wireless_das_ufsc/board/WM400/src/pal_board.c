/**
 * @file pal_board.c
 *
 * @brief PAL board specific functionality
 *
 * This file implements PAL board specific functionality.
 *
 * $Id: pal_board.c 30926 2012-02-20 13:29:28Z mahendran.p $
 *
 *  @author
 *      Atmel Corporation: http://www.atmel.com
 *      Support email: avr@atmel.com
 */
/*
 * Copyright (c) 2010, Atmel Corporation All rights reserved.
 *
 * Licensed under Atmel's Limited License Agreement --> EULA.txt
 */
#ifdef MAC802_15_4

/* === Includes ============================================================ */

#include <stdbool.h>
#include <stdlib.h>
#include "pal.h"
#include "pal_boardtypes.h"
#include "pal_config.h"
#include <sysclk.h>
#include <ioport.h>
#include <spi_master.h>

/*
 * 'sys_time' is a entity of timer module which is given to other modules
 * through the interface file below. 'sys_time' is required to obtain the
 * frame timestamp
 */
#include "pal_timer.h"

#if (BOARD_TYPE == WM400)
/* === Macros =============================================================== */

/* PLLA mutilplier and divider settings */
#if (F_CPU != 32000000 && F_CPU != 48000000)
#error "No settings for current F_CPU."
#endif

/* Timer Channel Macros*/
#define TIMER_CHANNEL_0					(0)
#define TIMER_CHANNEL_1					(1)
#define TIMER_CHANNEL_2					(2)

/* Define clock timeout */
#define CLOCK_TIMEOUT					(0xFFFFFFFF)

#define	SPI_CLK_SPEED					4000000				// 4-MHz
struct spi_device SPI_AT86RFX_DEVICE	=
{
	.id									= AT86RFX_SPI_CS_NUM	// Board specific select id
};

/* === Globals ============================================================== */

/* Indicates that the ADC conversion is finished. */
static volatile uint8_t conversionDone;
/* === Prototypes =========================================================== */

static void initialize_timer_channel(uint32_t mode, TcChannel *channel);
static uint8_t adc_is_channel_irq_status_set(uint32_t adc_sr, uint8_t channel);
static void adc_initialize (Adc *pAdc);
void trx_interface_init(void);

/* === Implementation ======================================================= */

/**
 * @brief Provides timestamp of the last received frame
 *
 * This function provides the timestamp (in microseconds)
 * of the last received frame.
 *
 * @param[out] Timestamp in microseconds
 */
void pal_trx_read_timestamp(uint32_t *timestamp)
{
    ENTER_CRITICAL_REGION();
    /*
     * 'TIME_STAMP_HIGH_REGISTER'    'TIME_STAMP_LOW_REGISTER register'
     *  |-----------------|-----------------| => 32 bit timestamp
     *        16 bits           16 bits
     */
    *timestamp							= (uint32_t)TIME_STAMP_HIGH_REGISTER << (uint32_t)16;
    *timestamp							|= TIME_STAMP_LOW_REGISTER;
    
	/* The timeout is pre scaled accroding to the clock period. */
    *timestamp							= (uint32_t)(*timestamp * CLOCK_PERIOD);

    LEAVE_CRITICAL_REGION();
}


#ifdef SNIFFER
/**
 * @brief Write the timestamp value
 *
 * This function writes the timestamp in the register
 *
 * @param[In] Timestamp in microseconds
 */
void pal_trx_write_timestamp(uint32_t *timestamp)
{
    /*
     * 'TIME_STAMP_HIGH_REGISTER'   'TIME_STAMP_LOW_REGISTER'
     *  ---------|--------- => 32 bit timestamp
     *   16 bits   16 bits
     */
    TIME_STAMP_LOW_REGISTER				= (uint16_t)(*timestamp);
    TIME_STAMP_HIGH_REGISTER			= (uint16_t)((*timestamp) >> (uint32_t)16);
}
#endif


/**
 * @brief Calibrates the internal RC oscillator
 *
 * This function calibrates the internal RC oscillator.
 *
 * @return True
 */
bool pal_calibrate_rc_osc(void)
{
    return (true);
}


/**
 * @brief Initializes the GPIO pins
 *
 * This function is used to initialize the port pins used to connect
 * the microcontroller to transceiver.
 */
void gpio_init(void)
{
	/**
	 * @brief Performs the low-level initialization of the chip.
	 * This includes EFC and master clock configuration.
	 * It also enable a low level on the pin NRST triggers a user reset.
	 */
	sysclk_init();

	board_init();
}

/*
 * This function is called by timer_init() to perform the non-generic portion
 * of the initialization of the timer module.
 */
void timer_init_non_generic(void)
{
#ifdef TIMESTAMP_UPPER_16BIT_SW

    initialize_timer_channel(TC_CLKS_MCK32, &PAL_TIMER_CH);

    /* The clock for pal timer is enabled here. */
    PERIPHERAL_CLOCK_ENABLE(PAL_TIMER_PERIPH_ID);

    /* The clock is enabled at the timer level. */
    PAL_TIMER_CCR						= PAL_TIMER_CCR_EN;

    /* timer overflow interrupts for the pal timer is enabled */
    PAL_TIMER_IER						= PAL_TIMER_SR_OVERFLOW_FLAG;

    /* The pal timer channel interrupt in NVIC is enabled. */
    NVIC_EnableIRQ(PAL_TIMER_IRQ_ID);

    /* pal timer channel is triggered. Reset counter and start clock */
    PAL_TIMER_CCR						= TC_CCR_SWTRG;
#else
    initialize_timer_channel(TC_CLKS_MCK32, &PAL_TIMER_CH);

    /* The clock for pal timer is enabled here. */
    PERIPHERAL_CLOCK_ENABLE(PAL_TIMER_PERIPH_ID);

    /* The clock is enabled at the timer level. */
    PAL_TIMER_CCR						= PAL_TIMER_CCR_EN;

    /* timer overflow interrupts for the pal timer is enabled */
    PAL_TIMER_IER						= PAL_TIMER_SR_OVERFLOW_FLAG;

    /* The pal timer channel interrupt in NVIC is enabled. */
    NVIC_EnableIRQ(PAL_TIMER_IRQ_ID);

    /* Initialize timer for Time stamp -32bit time */
    initialize_timer_channel((TC_CLKS_MCK32 | TC_CMR0_WAVE | TC_CMR0_ACPA_SET | (0x1 << 13) | TC_CMR0_ACPC_CLEAR), &TC0->TC_CHANNEL[TIMER_CHANNEL_0]);
    initialize_timer_channel((TC_CMR0_TCCLKS_XC1), &TC0->TC_CHANNEL[TIMER_CHANNEL_1]);

    /* RC is loaded with 0xFFFF so that whenever CV reached FFFF(RC) TIOA line is
     made as high */
    TC0->TC_CHANNEL[TIMER_CHANNEL_0].TC_RC = 0xFFFF;

    TC0->TC_BMR							= TC_BMR_TC1XC1S_TCLK1;
    TC0->TC_BCR							= TC_BCR_SYNC;

    /* The peripheral clock for pal timer is enabled */
    PERIPHERAL_CLOCK_ENABLE(ID_TC0);
    PERIPHERAL_CLOCK_ENABLE(ID_TC1);

    /* The clock is enabled at the timer level. */
    TC0->TC_CHANNEL[TIMER_CHANNEL_0].TC_CCR = TC_CCR0_CLKEN;
    TC0->TC_CHANNEL[TIMER_CHANNEL_1].TC_CCR = TC_CCR1_CLKEN;

    /* Reset counter and start the timer */
    PAL_TIMER_CCR = TC_CCR0_SWTRG;
    TC0->TC_CHANNEL[TIMER_CHANNEL_0].TC_CCR = TC_CCR0_SWTRG;
    TC0->TC_CHANNEL[TIMER_CHANNEL_1].TC_CCR = TC_CCR1_SWTRG;
#endif


#ifdef ENABLE_HIGH_PRIO_TMR
    initialize_timer_channel(TC_CLKS_MCK32, &PAL_HIGHPRI_TMR_CH);
    /*
     * The clock for high priority timer will be enabled only when a high
     * priority timer is requested to be started, as until that time, the timer
     * is not used at all. Also the overall power consumption
     * can be reduced by clocking a perpheral only when required.
     */
#endif  /* ENABLE_HIGH_PRIO_TMR */
}


/**
 * @brief Configures the timer channel
 *
 * This function configures the timer channel. It disables the clock to the
 * timer channel at the timer level, disables all the timer interrupts and
 * programs the prescaler for timer clock.
 *
 * @param mode Value to be written in the TCCLKS field
 *                                of the TC_CMR register, to select the
 *                                prescaler for the main clock which is
 *                                the timer clock source
 * @param channel - timer channel
 */
static void initialize_timer_channel(uint32_t mode,
                                     TcChannel *channel)
{
    uint32_t tc_status;
    TcChannel *pTcCh;

    pTcCh								= channel;

    /* The clock and the interrupts of the timer channel are disabled. */
    /*  Disable TC clock */
    pTcCh->TC_CCR						= TC_CCR_CLKDIS;

    /*  Disable interrupts */
    pTcCh->TC_IDR						= ALL_TIMER_INTERRUPTS_MASK;

    /* The status register is read to clear any pending interrupt. */
    tc_status							= pTcCh->TC_SR;

    /*  Set mode */
    /* The prescaler for the timer clock source (main clock) is selected. */
    pTcCh->TC_CMR						= mode;

    /*
     * Done to avoid compiler warning about variable being not used after
     * setting.
     */
    tc_status							= tc_status;
}


/**
 * @brief Initialize LEDs
 */
void pal_led_init(void)
{
    pal_led(LED_0, LED_OFF);
	pal_led(LED_1, LED_OFF);
	pal_led(LED_2, LED_OFF);
}


/**
 * @brief Control LED status
 *
 * param led_no LED ID
 * param led_setting LED_ON, LED_OFF, LED_TOGGLE
 */
void pal_led(led_id_t led_no, led_action_t led_setting)
{
    uint32_t led_pin;
    uint32_t pin_status;
    Pio* pio_select;

    switch (led_no)
    {
        case LED_0:
        {
            led_pin						= PIN_LED_0_MASK;
            pio_select					= PIN_LED_0_PIO;

                if (led_setting == LED_ON)
                {
	                led_setting = LED_OFF;
                }
                else if (led_setting == LED_OFF)
                {
	                led_setting = LED_ON;
                }
                else
                {
                }
		}
        break;
        case LED_1:
        {
            led_pin						= PIN_LED_1_MASK;
            pio_select					= PIN_LED_1_PIO;
        }
        break;
        case LED_2:
        {
	        led_pin						= PIN_LED_2_MASK;
	        pio_select					= PIN_LED_2_PIO;
        }
        break;
        default:
            return;
    }

    if (led_setting == LED_ON)
    {
        PIN_CLEAR(led_pin, pio_select);
    }
    else if (led_setting == LED_OFF)
    {
        PIN_SET(led_pin, pio_select);
    }
    else if (led_setting == LED_TOGGLE)
    {
        PIN_GET_STATUS(pin_status, led_pin, pio_select);

        if (pin_status)
        {
            PIN_CLEAR(led_pin, pio_select);
        }
        else
        {
            PIN_SET(led_pin, pio_select);
        }
    }
    else
    {
        return;
    }
}


/**
 * @brief Initialize the button
 */
void pal_button_init(void)
{
    const pin_t pinsPushButtons[]		= {PINS_PUSHBUTTONS};
    
	/* Enable peripheral clock of PIOB(button1) and PIOC(button2) before configuring the pins */
    PERIPHERAL_CLOCK_ENABLE(PIN_SW0_ID);
	PERIPHERAL_CLOCK_ENABLE(PIN_SW1_ID);
    
	/* Configure the pins for buttons */
    pal_pio_configure(pinsPushButtons, PIO_LISTSIZE(pinsPushButtons));
}


/**
 * @brief Read button
 *
 * @param button_no Button ID
 */
button_state_t pal_button_read(button_id_t button_no)
{
    Pio *pio_ptr;
    uint32_t button;
    if (button_no == BUTTON_0)
    {
		button							= PIN_SW0_MASK;
        pio_ptr							= PIN_SW0_PIO;
    }
    else if (button_no == BUTTON_1)
    {
	    button							= PIN_SW1_MASK;
	    pio_ptr							= PIN_SW1_PIO;
    }

    /* Read the status of the pin(button) */
    if (~pio_ptr->PIO_PDSR & button)
    {
        return BUTTON_PRESSED;
    }
    else
    {
        return BUTTON_OFF;
    }
}


/**
 * @brief Initializes the ADC controller
 *
 * @param pAdc Pointer to an Adc instance
 */
void adc_initialize(Adc *pAdc)
{
    /* Enable the peripheral clock. */
    PERIPHERAL_CLOCK_ENABLE(ID_ADC);

    /* Reset the controller. */
    pAdc->ADC_CR						= ADC_CR_SWRST;

    /* Clear the MR register. */
    pAdc->ADC_MR						= 0;

    /*
     * Write the required mode:
     * TRGEN: Hardware triggers are disabled
     * TRGSEL: 0
     * LOWRES: 12-bit resolution
     * SLEEP: Normal Mode
     */
    pAdc->ADC_MR						= ADC_MR_PRESCAL(ADC_PRESCAL)
										| ADC_MR_STARTUP_SUT512
										| ADC_MR_TRACKTIM(ADC_SHTIM);
}


/**
 * @brief Generation of random seed for function srand() in case this
 *        is not supported by the transceiver (e.g. AT86RF230)
 *
 * @return uint16_t Random number to be used as seed for function srand()
 * @ingroup apiPalApi
 */
uint16_t pal_generate_rand_seed(void)
{
    uint16_t		cur_random_seed		= 0;
    uint8_t			cur_random_bit		= 0;
    uint16_t		cur_adc_sample		= 0;
    uint8_t			no_of_conversion	= NO_OF_CONVERSIONS;   /* 16 times 1 channel to get 16 bit */

    adc_initialize(ADC);

    /* Enable ADC channel. */
    ADC->ADC_CHER						= (1 << ADC_NUM_1);

    /*Enable ADC interrupt*/
    NVIC_EnableIRQ(ADC_IRQn);

    for (no_of_conversion = 0; no_of_conversion < NO_OF_CONVERSIONS; no_of_conversion++)
    {

        /* Enable Interrupt for the ADC channel. */
        ADC->ADC_IER				= 1 << ADC_NUM_1;

        conversionDone				= 0;

        /* Start the conversion. */
        ADC->ADC_CR					= ADC_CR_START;

        while ( conversionDone != ((1 << ADC_NUM_1)) );

        cur_adc_sample				= ADC->ADC_CDR[ADC_NUM_1];
        cur_random_bit				= cur_adc_sample & 0x01;
        cur_random_seed				= cur_random_seed << 1;
        cur_random_seed				|= cur_random_bit;
    }

    /* Disable interrupt source. */
    NVIC_DisableIRQ(ADC_IRQn);

    /* Disable ADC channel. */
    ADC->ADC_CHDR					= (1 << ADC_NUM_1);

    return (cur_random_seed);
}


/**
 * @brief Interrupt handler for the ADC.
 *
 * This function is the interrupt handler for the ADC.
 * Signals that the conversion is finished by setting a flag variable.
 */
void ADC_Handler(void)
{
    uint32_t status					= ADC->ADC_ISR;

    if (adc_is_channel_irq_status_set(status, ADC_NUM_1))
    {
        /* Disable Interrupt for the ADC channel. */
        ADC->ADC_IDR				= 1 << ADC_NUM_1;

        conversionDone				|= (1 << ADC_NUM_1);
    }
}


/**
 * @brief Checks if ADC channel interrupt status is set
 *
 * @param adc_sr Value of SR register
 * @param channel Channel to be checked
 *
 * @return 1 if interrupt status is set, otherwise 0
 */
uint8_t adc_is_channel_irq_status_set(uint32_t adc_sr, uint8_t channel)
{
    if ((adc_sr & (1 << channel)) == (1 << channel))
    {
        return (1);
    }
    else
    {
        return (0);
    }
}


/**
 * @brief initializes the trx interface
 */
void trx_interface_init(void)
{
	spi_master_init(AT86RFX_SPI);
	spi_master_setup_device(AT86RFX_SPI, &SPI_AT86RFX_DEVICE, SPI_MODE_0, SPI_CLK_SPEED, 0);
	spi_set_peripheral_chip_select_value(AT86RFX_SPI, (0x0F & ~(1 << SPI_AT86RFX_DEVICE.id)));
	spi_enable(AT86RFX_SPI);

	ioport_set_pin_mode(AT86RFX_CS_PIN, AT86RFX_CS_FLAGS);
	ioport_set_pin_dir(AT86RFX_CS_PIN, IOPORT_DIR_OUTPUT);
	ioport_set_pin_level(AT86RFX_CS_PIN, IOPORT_PIN_LEVEL_HIGH);
	ioport_enable_pin(AT86RFX_CS_PIN);

	ioport_set_pin_mode(AT86RFX_IRQ0_PIN, AT86RFX_IRQ0_FLAGS);
	ioport_set_pin_dir(AT86RFX_IRQ0_PIN, IOPORT_DIR_INPUT);
	ioport_set_pin_sense_mode(AT86RFX_IRQ0_PIN, IOPORT_SENSE_RISING);
	ioport_enable_pin(AT86RFX_IRQ0_PIN);

	pio_configure_interrupt(AT86RFX_IRQ0_PIO, AT86RFX_IRQ0_MASK, AT86RFX_IRQ0_ATTR);

	ioport_set_pin_mode(AT86RFX_IRQ1_PIN, AT86RFX_IRQ1_FLAGS);
	ioport_set_pin_dir(AT86RFX_IRQ1_PIN, IOPORT_DIR_INPUT);
	ioport_set_pin_sense_mode(AT86RFX_IRQ1_PIN, IOPORT_SENSE_RISING);
	ioport_enable_pin(AT86RFX_IRQ1_PIN);

	pio_configure_interrupt(AT86RFX_IRQ1_PIO, AT86RFX_IRQ1_MASK, AT86RFX_IRQ1_ATTR);

	//NVIC_DisableIRQ(RF_PHY_Vect_IRQn);
	//NVIC_ClearPendingIRQ(RF_PHY_Vect_IRQn);
	//NVIC_EnableIRQ(RF_PHY_Vect_IRQn);
//
	pio_enable_interrupt(AT86RFX_IRQ0_PIO, AT86RFX_IRQ0_MASK);
	pio_enable_interrupt(AT86RFX_IRQ1_PIO, AT86RFX_IRQ1_MASK);
}

#endif /* #if (BOARD_TYPE == WM400) */

#endif /* #ifdef MAC802_15_4 */

/* EOF */


/**
 * @file pal_config.h
 *
 * @brief PAL configuration for WM400
 *
 * This header file contains configuration parameters for ATSAM4S16C.
 *
 * $Id: pal_config.h 29480 2011-11-23 12:04:21Z mahendran.p $
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

/* Prevent double inclusion */


#ifndef PAL_CONFIG_H
#define PAL_CONFIG_H

#include "pal_boardtypes.h"

#if (BOARD_TYPE == WM400)

#define	BAUD_RATE 115200

/* === Includes ==============================================================*/
/*
 * This header file is required since a function with
 * return type retval_t is declared
 */
#include "return_val.h"
#include "pal_internal.h"
#include <spi.h>

/* === Types =================================================================*/

/* Enumerations used to identify LEDs */
typedef enum led_id_tag
{
    LED_0,
    LED_1,
    LED_2
} led_id_t;

#define NO_OF_LEDS						(3)

/* Enumerations used to identify buttons */
typedef enum button_id_tag
{
    BUTTON_0,
    BUTTON_1
} button_id_t;

#define NO_OF_BUTTONS					(2)

/* === Externals =============================================================*/


/* === Macros ================================================================*/

/**
 * This board does not support antenna diversity.
 */
#define ANTENNA_DIVERSITY				(1)

/** SPI MISO pin definition. */
#define PIN_SPI_MISO					{PIO_PA12A_MISO, PIOA, ID_PIOA, PIO_PERIPH_A, PIO_DEFAULT}
/** SPI MOSI pin definition. */
#define PIN_SPI_MOSI					{PIO_PA13A_MOSI, PIOA, ID_PIOA, PIO_PERIPH_A, PIO_DEFAULT}
/** SPI SPCK pin definition. */
#define PIN_SPI_SPCK					{PIO_PA14A_SPCK, PIOA, ID_PIOA, PIO_PERIPH_A, PIO_DEFAULT}
/** SPI chip select pin definition. */
#define PIN_SPI_NPCS					{PIO_PB2B_NPCS2, PIOB, ID_PIOB, PIO_PERIPH_B, PIO_DEFAULT}
/** List of SPI pin definitions (MISO, MOSI & SPCK). */
#define PINS_SPI						{PIN_SPI_MISO, PIN_SPI_MOSI, PIN_SPI_SPCK}

#define TRX_INT_PIN						{AT86RFX_IRQ1_MASK, AT86RFX_IRQ1_PIO, AT86RFX_IRQ1_ID, PIO_INPUT, PIO_DEGLITCH | PIO_IT_RISE_EDGE}
#define TRX_RXTS_PIN					{AT86RFX_IRQ0_MASK, AT86RFX_IRQ0_PIO, AT86RFX_IRQ0_ID, PIO_INPUT, PIO_DEGLITCH | PIO_IT_RISE_EDGE}

/** Base address of SPI peripheral connected to the RF TRX. */
#define BOARD_TRX_SPI_BASE				SPI
/** Identifier of SPI peripheral connected to the transceiver. */
#define BOARD_TRX_SPI_ID				ID_SPI
/** Pins of the SPI peripheral connected to the transceiver. */
#define BOARD_TRX_SPI_PINS				PINS_SPI
/** Chip select connected to the transceiver. */
#define BOARD_TRX_NPCS					AT86RFX_SPI_CS_NUM
/** Chip select pin connected to the touchscreen controller. */
#define BOARD_TRX_NPCS_PIN				PIO_PB2B_NPCS2

#define BOARD_USB_BMATTRIBUTES			USBConfigurationDescriptor_SELFPOWERED_NORWAKEUP

/** UART0 pin RX */
#define PIN_UART0_RXD					{PIO_PA9A_URXD0, PIOA, ID_PIOA, PIO_PERIPH_A, PIO_DEFAULT}
/** UART0 pin TX */
#define PIN_UART0_TXD					{PIO_PA10A_UTXD0, PIOA, ID_PIOA, PIO_PERIPH_A, PIO_DEFAULT}

/** USART0 pin RX */
#define PIN_USART0_RXD					{PIO_PA5A_RXD0, PIOA, ID_PIOA, PIO_PERIPH_A, PIO_DEFAULT}
/** USART0 pin TX */
#define PIN_USART0_TXD					{PIO_PA6A_TXD0, PIOA, ID_PIOA, PIO_PERIPH_A, PIO_DEFAULT}
/** USART0 pin CTS */
#define PIN_USART0_CTS					{PIO_PA8A_CTS0, PIOA, ID_PIOA, PIO_PERIPH_A, PIO_DEFAULT}
/** USART0 pin RTS */
#define PIN_USART0_RTS					{PIO_PA7A_RTS0, PIOA, ID_PIOA, PIO_PERIPH_A, PIO_DEFAULT}
/** USART0 pin SCK */
#define PIN_USART0_SCK					{PIO_PA2B_SCK0, PIOA, ID_PIOA, PIO_PERIPH_A, PIO_DEFAULT}

/** USART1 pin RX */
#define PIN_USART1_RXD					{PIO_PA21A_RXD1, PIOA, ID_PIOA, PIO_PERIPH_A, PIO_DEFAULT}
/** USART1 pin TX */
#define PIN_USART1_TXD					{PIO_PA22A_TXD1, PIOA, ID_PIOA, PIO_PERIPH_A, PIO_DEFAULT}
/** USART1 pin CTS */
#define PIN_USART1_CTS					{PIO_PA25A_CTS1, PIOA, ID_PIOA, PIO_PERIPH_A, PIO_DEFAULT}
/** USART1 pin RTS */
#define PIN_USART1_RTS					{PIO_PA24A_RTS1, PIOA, ID_PIOA, PIO_PERIPH_A, PIO_DEFAULT}
/** USART1 pin ENABLE */
#define PIN_USART1_EN					{PIO_PA23A_SCK1, PIOA, ID_PIOA, PIO_OUTPUT_0, PIO_DEFAULT}

/** USB VBus monitoring pin definition. */
#define PIN_USB_VBUS					{PIO_PC9, PIOC, ID_PIOC, PIO_INPUT, PIO_PULLUP}


/** Number of USB endpoints */
#define CHIP_USB_NUMENDPOINTS			8

#define CHIP_USB_ENDPOINTS_MAXPACKETSIZE(i) \
    ((i == 0) ? 64 : \
     ((i == 1) ? 64 : \
      ((i == 2) ? 64 : \
       ((i == 3) ? 64 : \
        ((i == 4) ? 512 : \
         ((i == 5) ? 512 : \
          ((i == 6) ? 64 : \
           ((i == 7) ? 64 : 0 ))))))))

/** Endpoints Number of Bank */
#define CHIP_USB_ENDPOINTS_BANKS(i) \
    ((i == 0) ? 1 : \
     ((i == 1) ? 2 : \
      ((i == 2) ? 2 : \
       ((i == 3) ? 1 : \
        ((i == 4) ? 2 : \
         ((i == 5) ? 2 : \
          ((i == 6) ? 2 : \
           ((i == 7) ? 2 : 0 ))))))))


#if (F_CPU != 32000000UL && F_CPU != 48000000UL)
	#error "unsupported CPU clock"
#endif

/*
 * This board does NOT have an external eeprom available.
 */
#ifndef EXTERN_EEPROM_AVAILABLE
#define EXTERN_EEPROM_AVAILABLE			(0)
#endif

/*
 * Macros dealing with the PMC of ATSAM4S16CB
 */

/*
 * Enables the clock to the given peripheral id
 */
#define PERIPHERAL_CLOCK_ENABLE(id)		(pal_peripheral_clock_enable(id))

/*
 * Disables the clock to the given peripheral id
 */
#define PERIPHERAL_CLOCK_DISABLE(id)	(pal_peripheral_clock_disable(id))



/*
 * SPI Base Register:
 * SPI0 is used by WM400
 */
#define SPI_USED						(SPI)


/*
 * Macros dealing with the PIO of ATSAM4S16C
 */

/*
 * Enables the PIO control for the requested pin and sets it as output
 */

#define PIN_SET_AS_PIO_OUTPUT(pin, pio)	do {									\
        pio->PIO_OER = pin;														\
        pio->PIO_PER = pin;														\
    } while (0);

/*
 * Enables the PIO control for the requested pin and sets it as input
 */
#define PIN_SET_AS_PIO_INPUT(pin, pio)	do {									\
        pio->PIO_ODR = pin;														\
        pio->PIO_PER = pin;														\
    } while (0);

/*
 * Sets the pin of requested PIO
 */
#define PIN_SET(pin, pio)				do {									\
        pio->PIO_SODR = pin;													\
    } while (0);

/*
 * Clears the pin of requested PIO
 */
#define PIN_CLEAR(pin, pio)				do {									\
        pio->PIO_CODR = pin;													\
    } while (0);

/*
 * Gets the status of the requested pin of PIO
 */
#define PIN_GET_STATUS(status, pin, pio) do {									\
        status = (pio->PIO_PDSR & pin ? true : false);							\
    } while (0);

/* Enables the transceiver interrupts. */
#define ENABLE_TRX_IRQ()				do {									\
        (pal_pio_enable_interrupt(&(const pin_t) TRX_INT_PIN));					\
    } while(0);

/* Disables the transceiver interrupts. */
#define DISABLE_TRX_IRQ()				do {									\
        (pal_pio_disable_interrupt(&(const pin_t) TRX_INT_PIN));				\
    }while(0);

/* Clears the transceiver interrupts. */
#define CLEAR_TRX_IRQ()					do {									\
        (NVIC_ClearPendingIRQ((IRQn_Type) AT86RFX_IRQ1_ID));					\
    }while(0);

/* Enables the transceiver time stamp interrupt */
#define ENABLE_TRX_IRQ_TSTAMP()
/* Disables the transceiver time stamp interrupt */
#define DISABLE_TRX_IRQ_TSTAMP()
/* Clears the transceiver time stamp interrupt */
#define CLEAR_TRX_IRQ_TSTAMP()


/*
 * Macros dealing with the global interrupt (IRQ bit) of ATSAM4S16C.
 */

/* Enables the global interrupt. */
#define ENABLE_GLOBAL_IRQ()				cpu_irq_enable()

/* Disables the global interrupt. */
#define DISABLE_GLOBAL_IRQ()			cpu_irq_disable()

/*
 * This macro is used to mark the start of a critical region. It saves the
 * current status register and then disables the interrupt.
 */
#define ENTER_CRITICAL_REGION()													\
    {uint32_t sreg = __get_PRIMASK(); DISABLE_GLOBAL_IRQ()

/*
 * This macro is used to mark the end of a critical region. It restores the
 * current status register.
 */
#define LEAVE_CRITICAL_REGION()													\
	__set_PRIMASK(sreg);}

/*
 * This macro saves the trx interrupt status and disables the trx interrupt.
 */
#define ENTER_TRX_REGION() {uint32_t irq_mask = AT86RFX_IRQ1_PIO->PIO_IMR;		\
        DISABLE_TRX_IRQ() DISABLE_TRX_IRQ_TSTAMP()

/*
 *  This macro restores the transceiver interrupt status
 */
#define LEAVE_TRX_REGION()				AT86RFX_IRQ1_PIO->PIO_IER = irq_mask; }


/*
 * GPIO macros for ATSAM4S16C
 */

/*
 * This board uses an SPI-attached transceiver.
 */
#define PAL_USE_SPI_TRX					(1)

/*
 * Set TRX GPIO pins.
 */
#define RST_HIGH()						{PIN_SET(AT86RFX_RST_MASK, AT86RFX_RST_PIO)}
#define RST_LOW()						{PIN_CLEAR(AT86RFX_RST_MASK, AT86RFX_RST_PIO)}
#define SLP_TR_HIGH()					{PIN_SET(AT86RFX_SLP_MASK, AT86RFX_SLP_PIO)}
#define SLP_TR_LOW()					{PIN_CLEAR(AT86RFX_SLP_MASK, AT86RFX_SLP_PIO)}


/*
 * Timer macros for ATSAM4S16C
 */

/*
 * These macros are placeholders for delay functions for high speed processors.
 *
 * The following delay are not reasonably implemented via delay functions,
 * but rather via a certain number of NOP operations.
 * The actual number of NOPs for each delay is fully MCU and frequency
 * dependent, so it needs to be updated for each board configuration.
 *
 * ATSAM4S16C @ FPU
 */
#define WAIT_2_NOPS()					{nop(); nop();}
#define WAIT_4_NOPS()					{WAIT_2_NOPS(); WAIT_2_NOPS();}
#define WAIT_8_NOPS()					{WAIT_4_NOPS(); WAIT_4_NOPS();}
#define WAIT_16_NOPS()					{WAIT_8_NOPS(); WAIT_8_NOPS();}
#define WAIT_32_NOPS()					{WAIT_16_NOPS(); WAIT_16_NOPS();}

#if (F_CPU == 32000000UL)
/* 32 MHz */
/*
 * Wait for 65 ns.
 * time t7: SLP_TR time (see data sheet or SWPM).
 * In case the system clock is 32 MHz we need to have this value filled,
 * otherwise frame transmission may not be initiated properly.
 */
    #define PAL_WAIT_65_NS()			nop(); nop(); nop()
    /* Wait for 500 ns. */
    #define PAL_WAIT_500_NS()			WAIT_16_NOPS()
    /* Wait for 1 us. */
    #define PAL_WAIT_1_US()				WAIT_32_NOPS()

#elif (F_CPU == 48000000UL)
/* 48 MHz */
/*
 * Wait for 65 ns.
 * time t7: SLP_TR time (see data sheet or SWPM).
 * In case the system clock is 48 MHz we need to have this value filled,
 * otherwise frame transmission may not be initiated properly.
 */
    #define PAL_WAIT_65_NS()			WAIT_4_NOPS()
    /* Wait for 500 ns. */
    #define PAL_WAIT_500_NS()			WAIT_32_NOPS(); WAIT_16_NOPS()
    /* Wait for 1 us. */
    #define PAL_WAIT_1_US()				PAL_WAIT_500_NS(); PAL_WAIT_500_NS()

#endif

/*
 * The smallest timeout in microseconds
 */
#define MIN_TIMEOUT						(0x80)

/*
 * The largest timeout in microseconds
 */
#define MAX_TIMEOUT						(0x7FFFFFFF)

/*
 * Settings to give clocking to timer when radio is awake
 *
 */
#define TIMER_SRC_DURING_TRX_AWAKE()

/*
 * Settings to give clocking to timer when radio is sleeping
 *
 */
#define TIMER_SRC_DURING_TRX_SLEEP()

/*
 * Maximum numbers of software timers running at a time
 */
#define MAX_NO_OF_TIMERS				(25)
#if (MAX_NO_OF_TIMERS > 255)
#error "MAX_NO_OF_TIMERS must not be larger than 255"
#endif

/* If this macro is defined then the upper 16-bit of the timestamp value
is taken from sys_time which is a SW count incremented whenever lower 16bit
overflow happens */
#define TIMESTAMP_UPPER_16BIT_SW		(1)

/*
 * Hardware register that holds Rx timestamp
 */
#ifdef TIMESTAMP_UPPER_16BIT_SW
	#define TIME_STAMP_LOW_REGISTER		(PAL_TIMER_CH.TC_CV)
	#define TIMER_LOW_REGISTER			(PAL_TIMER_CH.TC_CV)
#else
	#define TIME_STAMP_LOW_REGISTER		(TC0->TC_CHANNEL[0].TC_CV)
	#define TIMER_LOW_REGISTER			(TC0->TC_CHANNEL[0].TC_CV)
	#define TIME_STAMP_HIGH_REGISTER	(TC0->TC_CHANNEL[1].TC_CV)
#endif

/* Macros for PAL Normal Timer Register */
#define PAL_TIMER_CH					(TC0->TC_CHANNEL[2])
#define PAL_TIMER_IRQ_ID				(TC2_IRQn)
#define PAL_TIMER_PERIPH_ID				(ID_TC2)
#define PAL_TIMER_CCR_EN				(TC_CCR_CLKEN)
#define PAL_TIMER_IER_FLAG				(TC_IER_CPCS)
#define PAL_TIMER_IDR_FLAG				(TC_IDR_CPCS)
#define PAL_TIMER_SR_FLAG				(TC_SR_CPCS)
#define PAL_TIMER_SR_OVERFLOW_FLAG		(TC_SR_COVFS)
#define PAL_TIMER_STATUS_REG			(PAL_TIMER_CH.TC_SR)
#define PAL_TIMER_IER					(PAL_TIMER_CH.TC_IER)
#define PAL_TIMER_IDR					(PAL_TIMER_CH.TC_IDR)
#define PAL_TIMER_COMP_REG				(PAL_TIMER_CH.TC_RC)
#define PAL_TIMER_CCR					(PAL_TIMER_CH.TC_CCR)
#define PAL_TIMER_IRQ					TC2_Handler


/* Macros for PAL High Priority Timer Register */
#define PAL_HIGHPRI_TMR_CH				(TC1->TC_CHANNEL[1])
#define PAL_HIGHPRIO_TIMER_IRQ_ID		(TC4_IRQn)
#define PAL_HIGHPRIO_TIMER_PERIPH_ID	(ID_TC4)
#define PAL_HIGHPRIO_TIMER_CCR_EN		(TC_CCR_CLKEN)
#define PAL_HIGHPRIO_TIMER_IER_FLAG		(TC_IER_CPCS)
#define PAL_HIGHPRIO_TIMER_IDR_FLAG		(TC_IDR_CPCS)
#define PAL_HIGHPRIO_TIMER_STATUS_REG	(PAL_HIGHPRI_TMR_CH.TC_SR)
#define PAL_HIGHPRIO_TIMER_IER			(PAL_HIGHPRI_TMR_CH.TC_IER)
#define PAL_HIGHPRIO_TIMER_IDR			(PAL_HIGHPRI_TMR_CH.TC_IDR)
#define PAL_HIGHPRIO_TIMER_COMP_REG		(PAL_HIGHPRI_TMR_CH.TC_RC)
#define PAL_HIGHPRIO_TIMER_CCR			(PAL_HIGHPRI_TMR_CH.TC_CCR)
#define PAL_HIGHPRIO_TIMER_IRQ			TC4_Handler

/*
 * TRX Access macros for ATSAM4S16C
 */

/*
 * Position of the PCS (peripheral chip select) field in the SPI_MR register.
 */
#define PCS_FIELD_IN_MR					(AT86RFX_SPI_CS_NUM)

/*
 * Value that needs to be written to in the PCS field of the SPI_MR to
 * activate the line to which the trx select line is connected.
 */
#define PCS_FIELD_VALUE					(1)

/*
 * Value of PCS field in SPI_MR (mode register) which indicates the controller
 * about the line to which the slave is connected.
 */
#define SS_ENABLE						(0x0F & ~(PCS_FIELD_VALUE << PCS_FIELD_IN_MR))

/*
 * Slave select made low
 */
#define SS_LOW()						PIN_CLEAR(AT86RFX_CS_MASK, AT86RFX_CS_PIO)

/*
 * Slave select made high
 */
#define SS_HIGH()						PIN_SET(AT86RFX_CS_MASK, AT86RFX_CS_PIO)

/*
 * Dummy value to be written in SPI transmit register to retrieve data form it
 */
#define SPI_DUMMY_VALUE					(0x00)

/* Reads the data from the SPI receive register. */
#define SPI_READ(register_value)		do {									\
        while ((SPI->SPI_SR & SPI_SR_RDRF) == 0);								\
        register_value = (SPI->SPI_RDR & 0xFFFF);								\
    } while (0);

/* Writes the data into the SPI transmit register. */
#define SPI_WRITE(data)					do {									\
        while ((SPI->SPI_SR & SPI_SR_TDRE) == 0);								\
        SPI->SPI_TDR = data & 0xFFFF;											\
        while ((SPI->SPI_SR & SPI_SR_TXEMPTY) == 0);							\
    } while (0);


/**
 * This board does not use the timestamp interrupt, since the DIG2 pin from the
 * the transceiver is not connected to the MCU.
 */
#ifndef DISABLE_TSTAMP_IRQ
#define DISABLE_TSTAMP_IRQ				(1)
#endif


/*
 * Value of an external PA gain
 * If no external PA is available, value is 0.
 */
#define EXTERN_PA_GAIN					(0)

/*
 * Alert initialization
 */
#define ALERT_INIT()					{										\
	PIN_SET_AS_PIO_OUTPUT(PIN_LED_0_MASK, PIN_LED_0_PIO);						\
	PIN_SET_AS_PIO_OUTPUT(PIN_LED_1_MASK, PIN_LED_1_PIO);						\
	PIN_SET_AS_PIO_OUTPUT(PIN_LED_2_MASK, PIN_LED_2_PIO);						\
	pal_led(LED_0, LED_OFF);													\
	pal_led(LED_1, LED_OFF);													\
	pal_led(LED_2, LED_OFF);			}


/*
 * Alert indication
 */
#define ALERT_INDICATE()				do										\
    {																			\
		pal_led(LED_0, LED_TOGGLE);												\
		pal_led(LED_1, LED_TOGGLE);												\
		pal_led(LED_2, LED_TOGGLE);												\
    } while (0);

/*
 * ADC Initialization values
 */
/*
 * Number of ADC conversions to be done during generation of random number.
 * Since a 16-bit value is needed and 4 ADC channels are used
 * to get 1 single bit (always the LSB), 4 subsequent conversions are required.
 */
#define BOARD_ADC_FREQ					(6000000)

#define NO_OF_CONVERSIONS				(16)

/* Value of the ADC clock in Hz */
#define USED_ADC_CLOCK_FREQ				(6000000)
/* Value of MCK in Hz */
#define USED_MCK_CLOCK_FREQ				(F_CPU)

#define ADC_STARTUP_TIME_NS				(10)        /* In us */
#define ADC_SAMPLE_AND_HOLD_TIME_US		(1200)      /* In ns */

/* Defines for the ADC Mode register */
#define ADC_PRESCAL						((USED_MCK_CLOCK_FREQ / (2 * USED_ADC_CLOCK_FREQ)) - 1)
#define ADC_STARTUP						(((USED_ADC_CLOCK_FREQ / 1000000) * ADC_STARTUP_TIME_NS / 8) - 1)
#define ADC_SHTIM						((((USED_ADC_CLOCK_FREQ / 1000000) * ADC_SAMPLE_AND_HOLD_TIME_US) / 1000) - 1)
#define ADC_NUM_1						(4)

/*
 * E2PROM Defines
 */
#define		EE_BOOTLOADER_OFFSET							(64)							// 64 - Bootloader offset
#define		EE_IEEE_ADDR									(EE_BOOTLOADER_OFFSET)			// 8  - IEEE address of board in EEPROM
#define		EE_RADIO_PAGE									(EE_IEEE_ADDR + 8)				// 1  - Radio channel page
#define		EE_RADIO_CHANNEL								(EE_RADIO_PAGE + 1)				// 1  - Radio virtual channel
#define		EE_RADIO_TXGAIN									(EE_RADIO_CHANNEL + 1)			// 1  - Radio TX gain
#define		EE_RADIO_RXGAIN									(EE_RADIO_TXGAIN + 1)			// 1  - Radio RX gain
#define		EE_RADIO_PANID									(EE_RADIO_RXGAIN + 1)			// 2  - Network Personal Area Network Identification
#define		EE_RADIO_SHORTSCAN								(EE_RADIO_PANID + 2)			// 1  - Network Short Scan Interval
#define		EE_RADIO_LONGSCAN								(EE_RADIO_SHORTSCAN + 1)		// 1  - Network Long Scan Interval
#define		EE_RADIO_VERBOSE								(EE_RADIO_LONGSCAN + 1)			// 1  - System Verbose
#define		EE_RADIO_SHORT_ADDR								(EE_RADIO_VERBOSE + 1)			// 2  - Network Short Address
#define		EE_RADIO_COORDINATOR							(EE_RADIO_SHORT_ADDR + 2)		// 1  - Network Device Coordinator
#define		EE_RADIO_BEACON_ORDER							(EE_RADIO_COORDINATOR + 1)		// 1  - Network Beacon Order
#define		EE_RADIO_SUPERFRAME_ORDER						(EE_RADIO_BEACON_ORDER + 1)		// 1  - Network Superframe Order
#define		EE_COMMUNICATION_TIMEOUT						(EE_RADIO_SUPERFRAME_ORDER + 1)	// 2  - Timeout for all communication interfaces
#define		EE_COMMUNICATION_PARENT							(EE_COMMUNICATION_TIMEOUT + 2)	// 2  - Network parent for Tree Route Service
#define		EE_COMMUNICATION_TYPE							(EE_COMMUNICATION_PARENT + 2)	// 1  - Communication interface type
#define		EE_USER_SPACE_OFFSET							(EE_COMMUNICATION_TYPE + 1)		//    - Start of EEPROM User Space

/* === Prototypes ===========================================================*/
void LowLevelInit(void);
#ifdef __cplusplus
extern "C" {
#endif


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* #if (BOARD_TYPE == WM400)*/

#endif  /* PAL_CONFIG_H */
/* EOF */

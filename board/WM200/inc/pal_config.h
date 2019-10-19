/**
 * @file pal_config.h
 *
 * @brief PAL configuration for ATxmega256A3
 *
 * This header file contains configuration parameters for ATxmega256A3.
 *
 * $Id: pal_config.h 22250 2010-06-16 13:51:39Z sschneid $
 *
 * @author    Atmel Corporation: http://www.atmel.com
 * @author    Support email: avr@atmel.com
 */
/*
 * Copyright (c) 2009, Atmel Corporation All rights reserved.
 *
 * Licensed under Atmel's Limited License Agreement --> EULA.txt
 */

/* Prevent double inclusion */
#ifndef PAL_CONFIG_H
#define PAL_CONFIG_H

/* === Includes =============================================================*/

#include "pal_boardtypes.h"
#include "avr_compiler.h"

#if (BOARD_TYPE == WM200)

#define	BAUD_RATE 115200

/*
 * This header file is required since a function with
 * return type retval_t is declared
 */
#include "return_val.h"

/* === Types ================================================================*/

/* Enumerations used to identify LEDs */
typedef enum led_id_tag
{
    LED_0,
    LED_1,
    LED_2,
    LED_3,
    LED_4,
    LED_5,
    LED_6,
    LED_7
} led_id_t;

#define NO_OF_LEDS                      (3)


/* Enumerations used to identify buttons */
typedef enum button_id_tag
{
    BUTTON_0,
    BUTTON_1,
    BUTTON_2,
    BUTTON_3,
    BUTTON_4,
    BUTTON_5,
    BUTTON_6,
    BUTTON_7
} button_id_t;

#define NO_OF_BUTTONS                   (1)

/* === Externals ============================================================*/


/* === Macros ===============================================================*/

/* Currently only the following system clock frequencies are supported */
#if ((F_CPU != (32000000UL)) && (F_CPU != (16000000UL)) && (F_CPU != (8000000UL)) && (F_CPU != (4000000UL)))
#error "Unsupported F_CPU value"
#endif

/**
 * This board NOT uses Antenna Diversity.
 * The antenna diversity support can be overwritten by Makefile setting.
 */
#ifndef ANTENNA_DIVERSITY
#define ANTENNA_DIVERSITY               (0)
#endif

///**
 //* This board uses a custom power table
 //*/
//#define CUSTOM_PWR_TABLE
//
///*
 //* Value of an external LNA gain.
 //* If no external LNA is available, the value is 0.
 //*/
//#define EXT_LNA_HIGH_GAIN    (14)
//#define EXT_LNA_LOW_GAIN     (-2)
//
///**
  //* RSSI BASE VAL based on External LNA Gain.
  //*/
//#define PAL_RSSI_BASE_VAL				(RSSI_BASE_VAL_DBM - EXT_LNA_HIGH_GAIN)
//
//#define CUSTOM_DEFAULT_TX_PWR
//
//#ifdef CUSTOM_DEFAULT_TX_PWR
///*
 //* Default value of transmit power of transceiver: Preset
 //*    - definition acct. IEEE802.15.4 PHY PIB attribute phyTransmitPower
 //*    - TX Pout init value based on validation
 //*/
//#define TAL_TRANSMIT_POWER_DEFAULT      (TX_PWR_TOLERANCE | 0x14)
//#endif

/*
 * Default tx power for Ch26 to meet FCC compliance
  */
#define DEFAULT_TX_POWER_CH26             (0x80 | 0x0d)

/*
 * IRQ macros for ATxmega256A3
 * Disable the timestamp IRQ/DIG2;
 * If following value is set to 0, the timestamp IRG/DIG2 is used.
 */
#ifndef DISABLE_TSTAMP_IRQ
#define DISABLE_TSTAMP_IRQ              (0)
#endif

/* Mapping of TRX interrupts to ISR vectors */
#define TRX_MAIN_ISR_VECTOR             (PORTC_INT0_vect)
#if ((defined BEACON_SUPPORT) || (defined ENABLE_TSTAMP)) && (DISABLE_TSTAMP_IRQ == 0)
#define TRX_TSTAMP_ISR_VECTOR           (PORTC_INT1_vect)
#endif

/** Enables the transceiver main interrupt. */
#define ENABLE_TRX_IRQ()                (PORTC.INTCTRL |= PORT_INT0LVL_gm)

/** Disables the transceiver main interrupt. */
#define DISABLE_TRX_IRQ()               (PORTC.INTCTRL &= ~PORT_INT0LVL_gm)

/** Clears the transceiver main interrupt. */
#define CLEAR_TRX_IRQ()                 (PORTC.INTFLAGS = PORT_INT0IF_bm)

#if ((defined BEACON_SUPPORT) || (defined ENABLE_TSTAMP)) && (DISABLE_TSTAMP_IRQ == 0)
/** Enables the transceiver timestamp interrupt. */
#define ENABLE_TRX_IRQ_TSTAMP()         (PORTC.INTCTRL |= PORT_INT1LVL_gm)

/** Disables the transceiver timestamp interrupt. */
#define DISABLE_TRX_IRQ_TSTAMP()        (PORTC.INTCTRL &= ~PORT_INT1LVL_gm)

/** Clears the transceiver timestamp interrupt. */
#define CLEAR_TRX_IRQ_TSTAMP()          (PORTC.INTFLAGS = PORT_INT1IF_bm)
#endif  /* #if ((defined BEACON_SUPPORT) || (defined ENABLE_TSTAMP)) && (DISABLE_TSTAMP_IRQ == 0) */

/** Enables the global interrupts. */
#define ENABLE_GLOBAL_IRQ()             sei()

/** Disables the global interrupts. */
#define DISABLE_GLOBAL_IRQ()            cli()

/*
 * This macro saves the global interrupt status
 */
#define ENTER_CRITICAL_REGION()         AVR_ENTER_CRITICAL_REGION()

/*
 *  This macro restores the global interrupt status
 */
#define LEAVE_CRITICAL_REGION()         AVR_LEAVE_CRITICAL_REGION()

/*
 * This macro saves the trx interrupt status and disables the trx interrupt.
 */
#define ENTER_TRX_REGION()				{ uint8_t irq_mask = PORTC.INTCTRL; PORTC.INTCTRL &= ~PORT_INT0LVL_gm

/*
 *  This macro restores the transceiver interrupt status
 */
#define LEAVE_TRX_REGION()				PORTC.INTCTRL = irq_mask; }


/*
 * GPIO macros for ATxmega256A3
 */

/*
 * This board uses an SPI-attached transceiver.
 */
#define PAL_USE_SPI_TRX                 (1)

/* Actual Ports */
/*
 * The data direction register for the transceiver
 */
#define TRX_PORT1_DDR                   (PORTC.DIR)

/*
 * The transceiver port
 */
#define TRX_PORT1                       (PORTC)

/*
 * RESET pin of transceiver
 */
#define TRX_RST                         (0)

/*
 * Sleep Transceiver pin
 */
#define SLP_TR                          (1)

/*
 * Slave select pin
 */
#define SEL                             (4)

/*
 * SPI Bus Master Output/Slave Input pin
 */
#define MOSI                            (5)

/*
 * SPI Bus Master Input/Slave Output pin
 */
#define MISO                            (6)

/*
 * SPI serial clock pin
 */
#define SCK                             (7)

/*
 * IRQ pin direct GPIO access
 *
 * use pin 2 directly to avoid additional macro "TRX_IRQ"
 *
 */
#define IRQ_PINGET()                    (TRX_PORT1.IN & _BV(2))

/*
 * Set TRX GPIO pins.
 */
#define RST_HIGH()                      (TRX_PORT1.OUTSET = _BV(TRX_RST))
#define RST_LOW()                       (TRX_PORT1.OUTCLR = _BV(TRX_RST))
#define SLP_TR_HIGH()                   (TRX_PORT1.OUTSET = _BV(SLP_TR))
#define SLP_TR_LOW()                    (TRX_PORT1.OUTCLR = _BV(SLP_TR))

/*
 * PORT where LEDs are connected
 */
#define LED_PORT                        (PORTA)

/*
 * PINs where LEDs are connected
 */
#define LED_PIN_0                       (6)
#define LED_PIN_1                       (3)
#define LED_PIN_2                       (2)
//#define LED_PIN_3                       (1)
//#define LED_PIN_4                       (1)
//#define LED_PIN_5                       (1)
//#define LED_PIN_6                       (1)
//#define LED_PIN_7                       (1)

/*
 * PORT where button is connected
 */
#define BUTTON_PORT                     (PORTE)

/*
 * PINs where buttons are connected
 */
#define BUTTON_PIN_0                    (5)
//#define BUTTON_PIN_1                    (2)
//#define BUTTON_PIN_2                    (2)
//#define BUTTON_PIN_3                    (2)
//#define BUTTON_PIN_4                    (2)
//#define BUTTON_PIN_5                    (2)
//#define BUTTON_PIN_6                    (2)
//#define BUTTON_PIN_7                    (2)

/*
 * UART 0 port and pin defines
 * here: UART 0 is located at port D, pin 2 and 3
 */
#ifdef UART0
#define UART0_PORT                      PORTD
#define UART0_TX_PIN                    PIN3_bm
#define UART0_RX_PIN                    PIN2_bm
#define UART0_REG                       USARTD0
#define UART0_RX_ISR_VECT               USARTD0_RXC_vect
#define UART0_TX_ISR_VECT               USARTD0_TXC_vect
#endif

/*
 * UART 1 port and pin defines
 * here: UART 1 is located at port E, pin 2 and 3
 */
#ifdef UART1
#define UART1_PORT                      PORTE
#define UART1_TX_PIN                    PIN3_bm
#define UART1_RX_PIN                    PIN2_bm
#define UART1_REG                       USARTE0
#define UART1_RX_ISR_VECT               USARTE0_RXC_vect
#define UART1_TX_ISR_VECT               USARTE0_TXC_vect
#endif

/*
 * Value of an external PA gain.
 * If no external PA is available, the value is 0.
 */
#define EXTERN_PA_GAIN                  (0)

/*
 * Timer macros for ATxmega256A3
 */
#define WAIT_2_NOPS()                   {nop(); nop();}
#define WAIT_4_NOPS()                   {WAIT_2_NOPS(); WAIT_2_NOPS();}
#define WAIT_8_NOPS()                   {WAIT_4_NOPS(); WAIT_4_NOPS();}
#define WAIT_16_NOPS()                  {WAIT_8_NOPS(); WAIT_8_NOPS();}
#define WAIT_32_NOPS()                  {WAIT_16_NOPS(); WAIT_16_NOPS();}

/*
 * These macros are placeholders for delay functions for high speed processors.
 *
 * The following delay are not reasonbly implemented via delay functions,
 * but rather via a certain number of NOP operations.
 * The actual number of NOPs for each delay is fully MCU and frequency
 * dependent, so it needs to be updated for each board configuration.
 */
#if (F_CPU == (32000000UL))
/*
 * ATxmega256A3 @ 32MHz
 */
    /*
     * Wait for 65 ns.
     * time t7: SLP_TR time (see data sheet or SWPM).
     * In case the system clock is 32 MHz we need to have this value filled,
     * otherwise frame transmission may not be initiated properly.
     */
    #define PAL_WAIT_65_NS()                nop(); nop(); nop()
    /* Wait for 500 ns. */
    #define PAL_WAIT_500_NS()               WAIT_16_NOPS()
    /* Wait for 1 us. */
    #define PAL_WAIT_1_US()                 WAIT_32_NOPS()

#elif (F_CPU == (16000000UL))
/*
 * ATxmega256A3 @ 16MHz
 */
    /*
     * Wait for 65 ns.
     * time t7: SLP_TR time (see data sheet or SWPM).
     */
    #define PAL_WAIT_65_NS()                nop(); nop()
    /* Wait for 500 ns. */
    #define PAL_WAIT_500_NS()               WAIT_8_NOPS()
    /* Wait for 1 us. */
    #define PAL_WAIT_1_US()                 WAIT_16_NOPS()

#elif (F_CPU == (8000000UL))
/*
 * ATxmega256A3 @ 8MHz
 */
    /*
     * Wait for 65 ns.
     * time t7: SLP_TR time (see data sheet or SWPM).
     */
    #define PAL_WAIT_65_NS()                nop()
    /* Wait for 500 ns. */
    #define PAL_WAIT_500_NS()               WAIT_4_NOPS()
    /* Wait for 1 us. */
    #define PAL_WAIT_1_US()                 WAIT_8_NOPS()

#elif (F_CPU == (4000000UL))
/*
 * ATxmega256A3 @ 4MHz
 */
    /*
     * Wait for 65 ns.
     * time t7: SLP_TR time (see data sheet or SWPM).
     * In case the system clock is slower than 32 MHz we do not need
     * to have this value filled.
     */
    #define PAL_WAIT_65_NS()                // empty
    /* Wait for 500 ns. */
    #define PAL_WAIT_500_NS()               WAIT_2_NOPS()
    /* Wait for 1 us. */
    #define PAL_WAIT_1_US()                 WAIT_4_NOPS()

#else
    #error Unknown system clock
#endif

/*
 * The smallest timeout in microseconds
 */
#define MIN_TIMEOUT                     (0x80)

/*
 * The largest timeout in microseconds
 */
#define MAX_TIMEOUT                     (0x7FFFFFFF)

/**
 * Minimum time in microseconds, accepted as a delay request
 */
#define MIN_DELAY_VAL                   (5)

/*
 * Timer clock source while radio is awake.
 *
 * T1 & T0:
 * clk source is event channel 0 triggered by system clock (16MHz) with prescaler 16
 * TRX's clock (CLKM) is never used with Xmega, keep macro empty.
 */
#define TIMER_SRC_DURING_TRX_AWAKE()

/*
 * Timer clock source while radio is sleeping.
 *
 * T1 & T0:
 * clk source is event channel 0 triggered by system clock (16MHz) with prescaler 16
 */
#define TIMER_SRC_DURING_TRX_SLEEP() {TCC0_CTRLA = TC0_CLKSEL3_bm; TCC1_CTRLA = TC1_CLKSEL3_bm;}

/*
 * Maximum numbers of software timers running at a time
 */
#define MAX_NO_OF_TIMERS                (25)
#if (MAX_NO_OF_TIMERS > 255)
#error "MAX_NO_OF_TIMERS must not be larger than 255"
#endif

/*
 * Hardware register that holds Rx timestamp
 */
#define TIME_STAMP_REGISTER             (TCC1_CCA)


/*
 * TRX Access macros for ATxmega256A3
 */

/*
 * Bit mask for slave select
 */
#define SS_BIT_MASK                     (1 << SEL)

/*
 * Slave select made low
 */
#define SS_LOW()                        (TRX_PORT1.OUTCLR = SS_BIT_MASK)

/*
 * Slave select made high
 */
#define SS_HIGH()                       (TRX_PORT1.OUTSET = SS_BIT_MASK)

/*
 * Mask for SPIF bit in status register
 */
#define SPIF_MASK                       (SPI_IF_bm)

/*
 * SPI Data Register
 */
#define SPI_DATA_REG                    (SPIC.DATA)

/*
 * Wait for SPI interrupt flag
 */
#define SPI_WAIT()                      do {                        \
    while ((SPIC.STATUS & SPIF_MASK) == 0) { ; }                    \
} while (0)

/*
 * Dummy value written in SPDR to retrieve data form it
 */
#define SPI_DUMMY_VALUE                 (0x00)

/*
 * TRX Initialization
 */
#if (F_CPU == (32000000UL))
    #define TRX_INIT()                      do {                        \
        /* Enable the SPI and make the microcontroller as SPI master */ \
        /* Set the SPI speed to 4MHz. */                                \
        /* CLK2X = 1; PRESCALER = 1 (clkPER/8) */                       \
        /* Resulting SPI speed: 4MHz */                                 \
        SPIC.CTRL = SPI_ENABLE_bm | SPI_MASTER_bm | (0 << SPI_MODE0_bp);\
        SPIC.CTRL |= (1 << SPI_CLK2X_bp) | (1 << SPI_PRESCALER0_bp);    \
        /* Set SEL pin to high */                                       \
        TRX_PORT1.OUTSET = _BV(SEL);                                    \
    } while (0)

#elif (F_CPU == (16000000UL))
    #define TRX_INIT()                      do {                        \
        /* Enable the SPI and make the microcontroller as SPI master */ \
        /* Set the SPI speed to 4MHz. */                                \
        /* CLK2X = 0; PRESCALER = 0 (clkPER/4) */                       \
        SPIC.CTRL = SPI_ENABLE_bm | SPI_MASTER_bm | (0 << SPI_MODE0_bp);\
        /* Set SEL pin to high */                                       \
        TRX_PORT1.OUTSET = _BV(SEL);                                    \
    } while (0)

#elif (F_CPU == (8000000UL))
    #define TRX_INIT()                      do {                        \
        /* Enable the SPI and make the microcontroller as SPI master */ \
        /* Set the SPI speed to 4MHz. */                                \
        /* CLK2X = 1; PRESCALER = 0 (clkPER/2) */                       \
        SPIC.CTRL = SPI_ENABLE_bm | SPI_MASTER_bm | (0 << SPI_MODE0_bp);\
        SPIC.CTRL |= (1 << SPI_CLK2X_bp);                               \
        /* Set SEL pin to high */                                       \
        TRX_PORT1.OUTSET = _BV(SEL);                                    \
    } while (0)

#elif (F_CPU == (4000000UL))
    #define TRX_INIT()                      do {                        \
        /* Enable the SPI and make the microcontroller as SPI master */ \
        /* Set the SPI speed to 2MHz. */                                \
        /* CLK2X = 1; PRESCALER = 0 (clkPER/2) */                       \
        SPIC.CTRL = SPI_ENABLE_bm | SPI_MASTER_bm | (0 << SPI_MODE0_bp);\
        SPIC.CTRL |= (1 << SPI_CLK2X_bp);                               \
        /* Set SEL pin to high */                                       \
        TRX_PORT1.OUTSET = _BV(SEL);                                    \
    } while (0)

#else
    #error Unknown system clock
#endif

/*
 * This board does NOT have an external eeprom available.
 */
#ifndef EXTERN_EEPROM_AVAILABLE
#define EXTERN_EEPROM_AVAILABLE            (0)
#endif

/*
 * Alert initialization
 */
#define ALERT_INIT()                    do {    \
        LED_PORT.OUT    = 0;                    \
        LED_PORT.DIRSET = 0x4C;                 \
} while (0)

/*
 * Alert indication
 */
#define ALERT_INDICATE()                (LED_PORT.OUTTGL = 0x4C)


/*
 * Macros for random seed generation by means of the ADC
 */
#define COMMON_MODE_CYCLES              (16)
#define SAMPLE_COUNT                    (16)
#define LSB_MASK_INT_16                 (0x0001)

/*
 * ADC used for random seed generation
 */
#define ADC_FOR_RANDOM_SEED             (ADCB)
#define ADC_FOR_TEMPERATURE             (ADCB)

/*
 * Channel used from the ADC for random seed generation
 * ! Do not put the define in "()"
 */
#define ADC_CHANNEL_RANDOM_SEED			CH1
#define ADC_CHANNEL_TEMPERATURE			CH0

/*
 * Positive and negative input multiplexer selection.
 *
 * The pin were selected based on test with this board, but might need to be
 * retested and updated for a new hardware platform.
 */
#define ADC_POS_MUX_SEL                 (2)
#define ADC_NEG_MUX_SEL                 (2)

/*
 * This macro returns the channel conversion complete flag.
 */
#define ADC_CH_CONVERSION_COMPLETEs(_adc_ch)    (((_adc_ch)->INTFLAGS & ADC_CH_CHIF_bm) != 0x00)


/* === Macros to freeRTOS ===================================================*/

#ifndef	XMEGA_TICK_PORT
	#define XMEGA_TIMER_TICK								TCC0
	#define	XMEGA_TIMER_TICK_VECTovf						TCC0_OVF_vect
//	#define XMEGA_TIMER_TICK								TCD0
//	#define	XMEGA_TIMER_TICK_VECTovf						TCD0_OVF_vect
//	#define XMEGA_TIMER_TICK								TCE0
//	#define	XMEGA_TIMER_TICK_VECTovf						TCE0_OVF_vect
//	#define XMEGA_TIMER_TICK								TCF0
//	#define	XMEGA_TIMER_TICK_VECTovf						TCF0_OVF_vect
#endif
#define	XMEGA_TIMER_TICK_PERIOD								( F_CPU / 64 / configTICK_RATE_HZ )

//#define	XMEGA_SOFTWARE_INT_PORT								PORTA
//#define	XMEGA_SOFTWARE_INT0_MASK							0x01
//#define	XMEGA_SOFTWARE_INT0_VECT							PORTD_INT0_vect
//#define	XMEGA_SOFTWARE_INT1_MASK							0x02
//#define	XMEGA_SOFTWARE_INT1_VECT							PORTA_INT1_vect

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
#ifdef __cplusplus
extern "C" {
#endif

void		pal_adctemp_init(void);
double		pal_read_temperature(void);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* WM200 */

#endif  /* PAL_CONFIG_H */
/* EOF */
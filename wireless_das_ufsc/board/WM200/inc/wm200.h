#ifndef _WM200_SHIELD_H_
#define _WM200_SHIELD_H_

#include <compiler.h>

#define MCU_SOC_NAME        "ATxmega256A3U"
#define BOARD_NAME          "WM-200"

/* Validate board support for the common sensor service. */
#define COMMON_SENSOR_PLATFORM

/**
 * \name LEDs
 *
 * LED0 and LED1 are single yellow LEDs that are active low..
 * LED2 and LED3 are inside one package (Led red and green close
 * to USB connector) but can be controlled individually.
 * LED2 has a red color and is active low. This LED can be
 * used for general purposes.
 * LED3 has a green color and is active high. By default this
 * LED is on since it shall indicate that power is applied to the
 * board. By pulling the gate of a N-FET low it is possible to
 * turn off the LED if needed.
 */

#define LED0_GPIO                       IOPORT_CREATE_PIN(PORTA, 2)
#define LED1_GPIO                       IOPORT_CREATE_PIN(PORTA, 3)
#define LED2_GPIO                       IOPORT_CREATE_PIN(PORTA, 6)
#define LED0                            LED0_GPIO
#define LED1                            LED1_GPIO
#define LED2                            LED2_GPIO
//! Number of LEDs.
#define LED_COUNT                       3

//! \name Push buttons
#define GPIO_PUSH_BUTTON_0              IOPORT_CREATE_PIN(PORTE, 5)

//! \name DataFlash memory (AT45DBX)
#define AT45DBX_SPI                     &USARTF0
#define AT45DBX_CS                      IOPORT_CREATE_PIN(PORTF, 0)
#define AT45DBX_MASTER_SCK              IOPORT_CREATE_PIN(PORTF, 1)
#define AT45DBX_MASTER_MOSI             IOPORT_CREATE_PIN(PORTF, 3)
#define AT45DBX_MASTER_MISO             IOPORT_CREATE_PIN(PORTF, 2)

/**
 * \name External oscillator
 *
 * \todo Need to measure the actual startup time on the hardware.
 */
#define BOARD_XOSC_HZ                   32768
#define BOARD_XOSC_TYPE                 XOSC_TYPE_32KHZ
#define BOARD_XOSC_STARTUP_US           1000000
//#define CONFIG_XOSC_RANGE				XOSC_RANGE_12TO16

/**
 * \name Reserved communication interface
 */
#define TWID_SDA						IOPORT_CREATE_PIN(PORTE, 0)
#define TWID_SCL						IOPORT_CREATE_PIN(PORTE, 1)
#define USARTE0_RXD						IOPORT_CREATE_PIN(PORTE, 2)
#define USARTE0_TXD						IOPORT_CREATE_PIN(PORTE, 3)

/**
 * \name Reserved communication interface
 */
#define USARTD0_RXD						IOPORT_CREATE_PIN(PORTD, 2)
#define USARTD0_TXD						IOPORT_CREATE_PIN(PORTD, 3)
#define USARTD1_RXD						IOPORT_CREATE_PIN(PORTD, 6)
#define USARTD1_TXD						IOPORT_CREATE_PIN(PORTD, 7)

/**
 * \name IEEE 802.15.4 Wireless Transceiver Configuration
 */
#define AT86RFX_SPI						&SPIC
#define AT86RFX_RST_PIN					IOPORT_CREATE_PIN(PORTC, 0)
#define AT86RFX_MISC_PIN				IOPORT_CREATE_PIN(PORTC, 3)
#define AT86RFX_IRQ_PIN					IOPORT_CREATE_PIN(PORTC, 2)
#define AT86RFX_SLP_PIN					IOPORT_CREATE_PIN(PORTC, 1)
#define AT86RFX_SPI_CS					IOPORT_CREATE_PIN(PORTC, 4)
#define AT86RFX_SPI_MOSI				IOPORT_CREATE_PIN(PORTC, 5)
#define AT86RFX_SPI_MISO				IOPORT_CREATE_PIN(PORTC, 6)
#define AT86RFX_SPI_SCK					IOPORT_CREATE_PIN(PORTC, 7)

#define AT86RFX_INTC_INIT()				ioport_configure_pin(AT86RFX_IRQ_PIN, IOPORT_DIR_INPUT); \
										PORTC.PIN2CTRL = PORT_ISC0_bm; \
										PORTC.INT0MASK = PIN2_bm; \
										PORTC.INTFLAGS = PORT_INT0IF_bm;

#define AT86RFX_ISR()					ISR(PORTC_INT0_vect)

/** Enables the transceiver main interrupt. */
#define ENABLE_TRX_IRQ()				(PORTC.INTCTRL |= PORT_INT0LVL_gm)

/** Disables the transceiver main interrupt. */
#define DISABLE_TRX_IRQ()				(PORTC.INTCTRL &= ~PORT_INT0LVL_gm)

/** Clears the transceiver main interrupt. */
#define CLEAR_TRX_IRQ()					(PORTC.INTFLAGS = PORT_INT0IF_bm)

/*
 * This macro saves the trx interrupt status and disables the trx interrupt.
 */
#define ENTER_TRX_REGION()				{ uint8_t irq_mask = PORTC.INTCTRL; PORTC.INTCTRL &= ~PORT_INT0LVL_gm

/*
 *  This macro restores the transceiver interrupt status
 */
#define LEAVE_TRX_REGION()				PORTC.INTCTRL = irq_mask; }

#if !defined(CONF_BOARD_ENABLE_USARTE0)
	#define CONF_BOARD_ENABLE_USARTE0
#endif


#endif /* _WM200_SHIELD_H_ */

/**
 * \file
 *
 * \brief SAM4S-XPLAINED Board Definition.
 *
 * Copyright (c) 2011 - 2013 Atmel Corporation. All rights reserved.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. The name of Atmel may not be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * 4. This software may only be redistributed and used in connection with an
 *    Atmel microcontroller product.
 *
 * THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * \asf_license_stop
 *
 */

#ifndef SAM4S_XPLAINED_H_INCLUDED
#define SAM4S_XPLAINED_H_INCLUDED

#include "compiler.h"
#include "system_sam4s.h"

#define BOARD_REV_B
/*----------------------------------------------------------------------------*/
/**
 *  \page sam4s_xplained_opfreq "SAM4S-XPLAINED - Operating frequencies"
 *  This page lists several definition related to the board operating frequency
 *
 *  \section Definitions
 *  - \ref BOARD_FREQ_*
 *  - \ref BOARD_MCK
 */

/** Board oscillator settings */
#define BOARD_FREQ_SLCK_XTAL        (32768U)
#define BOARD_FREQ_SLCK_BYPASS      (32768U)
#define BOARD_FREQ_MAINCK_XTAL      (12000000U)
#define BOARD_FREQ_MAINCK_BYPASS    (12000000U)

/** Master clock frequency */
#define BOARD_MCK                   CHIP_FREQ_CPU_MAX

/** board main clock xtal statup time */
#define BOARD_OSC_STARTUP_US        15625

/*----------------------------------------------------------------------------*/
/**
 * \page sam4s_xplained_board_info "SAM4S-XPLAINED - Board informations"
 * This page lists several definition related to the board description.
 *
 * \section Definitions
 * - \ref BOARD_NAME
 */

/** Name of the board */
#define MCU_SOC_NAME					"ATSAM4S16C"
#define BOARD_NAME						"SAM4S-XPLAINED"
/** Board definition */
#define sam4sxplained
/** Family definition (already defined) */
#define sam4s
/** Core definition */
#define cortexm4

/*----------------------------------------------------------------------------*/
//! \name SW0 definitions
//@{
#define SW0_PIN							IOPORT_CREATE_PIN(PIOA, 5)
#define SW0_ACTIVE						false
#define SW0_INACTIVE					!SW0_ACTIVE
#define SW0_SUPC_INPUT					2

/**
 * Wrapper macros for SW0, to ensure common naming across all Xplained Pro
 * boards.
 */
#define PIN_SW0							{PIO_PA5, PIOA, ID_PIOA, PIO_INPUT, PIO_PULLUP | PIO_DEBOUNCE | PIO_IT_RISE_EDGE}
#define PIN_SW0_MASK					PIO_PA5
#define PIN_SW0_PIO						PIOA
#define PIN_SW0_ID						ID_PIOA
#define PIN_SW0_TYPE					PIO_INPUT
#define PIN_SW0_ATTR					(PIO_PULLUP | PIO_DEBOUNCE | PIO_IT_RISE_EDGE)
//@}

//! \name LED0 definitions
//@{
#define LED0_GPIO						PIO_PC10_IDX
#define LED0_ACTIVE_LEVEL				false
#define LED0_INACTIVE_LEVEL				!LED0_ACTIVE_LEVEL
//@}

//! \name LED1 definitions
//@{
#define LED1_GPIO						PIO_PC17_IDX
#define LED1_ACTIVE_LEVEL				false
#define LED1_INACTIVE_LEVEL				!LED1_ACTIVE_LEVEL
//@}

/*----------------------------------------------------------------------------*/
/** Pins ADC */
#define PINS_ADC_TRIG      PIO_PA8_IDX
#define PINS_ADC_TRIG_FLAG (PIO_PERIPH_B | PIO_DEFAULT)

/** UART0 pins (UTXD0 and URXD0) definitions, PA9,10. */
#define PINS_UART0						(PIO_PA9A_URXD0 | PIO_PA10A_UTXD0)
#define PIN_UART0_RXD_IDX				(PIO_PA9_IDX)
#define PIN_UART0_TXD_IDX				(PIO_PA10_IDX)
#define PINS_UART0_FLAGS				(IOPORT_MODE_MUX_A)

#define PINS_UART0_PORT					IOPORT_PIOA
#define PINS_UART0_MASK					(PIO_PA9A_URXD0 | PIO_PA10A_UTXD0)
#define PINS_UART0_PIO					PIOA
#define PINS_UART0_ID					ID_PIOA
#define PINS_UART0_TYPE					PIO_PERIPH_A
#define PINS_UART0_ATTR					PIO_DEFAULT

/** UART1 pins (UTXD1 and URXD1) definitions, PB2,PB3. */
#define PINS_UART1						(PIO_PB2A_URXD1 | PIO_PB3A_UTXD1)
#define PINS_UART1_RXD_IDX				(PIO_PB2_IDX)
#define PINS_UART1_TXD_IDX				(PIO_PB3_IDX)
#define PINS_UART1_FLAGS				(IOPORT_MODE_MUX_A)

#define PINS_UART1_PORT					IOPORT_PIOB
#define PINS_UART1_MASK					(PIO_PB2A_URXD1 | PIO_PB3A_UTXD1)
#define PINS_UART1_PIO					PIOB
#define PINS_UART1_ID					ID_PIOB
#define PINS_UART1_TYPE					PIO_PERIPH_A
#define PINS_UART1_ATTR					PIO_DEFAULT

/** EBI Data Bus pins */
#define PIN_EBI_DATA_BUS_D0        PIO_PC0_IDX
#define PIN_EBI_DATA_BUS_D1        PIO_PC1_IDX
#define PIN_EBI_DATA_BUS_D2        PIO_PC2_IDX
#define PIN_EBI_DATA_BUS_D3        PIO_PC3_IDX
#define PIN_EBI_DATA_BUS_D4        PIO_PC4_IDX
#define PIN_EBI_DATA_BUS_D5        PIO_PC5_IDX
#define PIN_EBI_DATA_BUS_D6        PIO_PC6_IDX
#define PIN_EBI_DATA_BUS_D7        PIO_PC7_IDX
#define PIN_EBI_DATA_BUS_FLAGS     PIO_PERIPH_A | PIO_PULLUP
#define PIN_EBI_DATA_BUS_MASK      0xFF
#define PIN_EBI_DATA_BUS_PIO       PIOC
#define PIN_EBI_DATA_BUS_ID        ID_PIOC
#define PIN_EBI_DATA_BUS_TYPE      PIO_PERIPH_A
#define PIN_EBI_DATA_BUS_ATTR      PIO_PULLUP
/** EBI NRD pin */
#define PIN_EBI_NRD       PIO_PC11_IDX
#define PIN_EBI_NRD_FLAGS PIO_PERIPH_A | PIO_PULLUP
#define PIN_EBI_NRD_MASK  1 << 11
#define PIN_EBI_NRD_PIO   PIOC
#define PIN_EBI_NRD_ID    ID_PIOC
#define PIN_EBI_NRD_TYPE  PIO_PERIPH_A
#define PIN_EBI_NRD_ATTR  PIO_PULLUP
/** EBI NWE pin */
#define PIN_EBI_NWE       PIO_PC8_IDX
#define PIN_EBI_NWE_FLAGS PIO_PERIPH_A | PIO_PULLUP
#define PIN_EBI_NWE_MASK  1 << 8
#define PIN_EBI_NWE_PIO   PIOC
#define PIN_EBI_NWE_ID    ID_PIOC
#define PIN_EBI_NWE_TYPE  PIO_PERIPH_A
#define PIN_EBI_NWE_ATTR  PIO_PULLUP
/** EBI NCS0 pin */
#define PIN_EBI_NCS0       PIO_PC14_IDX
#define PIN_EBI_NCS0_FLAGS PIO_PERIPH_A | PIO_PULLUP
#define PIN_EBI_NCS0_MASK  1 << 14
#define PIN_EBI_NCS0_PIO   PIOC
#define PIN_EBI_NCS0_ID    ID_PIOC
#define PIN_EBI_NCS0_TYPE  PIO_PERIPH_A
#define PIN_EBI_NCS0_ATTR  PIO_PULLUP

/** EBI NLB pin */
#define PIN_EBI_NLB           PIO_PC16_IDX
#define PIN_EBI_NLB_FLAGS     PIO_OUTPUT_0

/** EBI address bus pins  */
#define PIN_EBI_ADDR_BUS_A0    PIO_PC18_IDX
#define PIN_EBI_ADDR_BUS_A1    PIO_PC19_IDX
#define PIN_EBI_ADDR_BUS_A2    PIO_PC20_IDX
#define PIN_EBI_ADDR_BUS_A3    PIO_PC21_IDX
#define PIN_EBI_ADDR_BUS_A4    PIO_PC22_IDX
#define PIN_EBI_ADDR_BUS_A5    PIO_PC23_IDX
#define PIN_EBI_ADDR_BUS_A6    PIO_PC24_IDX
#define PIN_EBI_ADDR_BUS_A7    PIO_PC25_IDX
#define PIN_EBI_ADDR_BUS_A8    PIO_PC26_IDX
#define PIN_EBI_ADDR_BUS_A9    PIO_PC27_IDX
#define PIN_EBI_ADDR_BUS_A10   PIO_PC28_IDX
#define PIN_EBI_ADDR_BUS_A11   PIO_PC29_IDX
#define PIN_EBI_ADDR_BUS_A12   PIO_PC30_IDX
#define PIN_EBI_ADDR_BUS_A13   PIO_PC31_IDX
#define PIN_EBI_ADDR_BUS_FLAG1 PIO_PERIPH_A | PIO_PULLUP
#define PIN_EBI_ADDR_BUS_A14   PIO_PA18_IDX
#define PIN_EBI_ADDR_BUS_A15   PIO_PA19_IDX
#define PIN_EBI_ADDR_BUS_A16   PIO_PA20_IDX
#define PIN_EBI_ADDR_BUS_A17   PIO_PA0_IDX
#define PIN_EBI_ADDR_BUS_A18   PIO_PA1_IDX
#define PIN_EBI_ADDR_BUS_FLAG2 PIO_PERIPH_C | PIO_PULLUP

/** LED #0 pin definition (BLUE). */
#define LED0							LED0_GPIO
#define LED_0_NAME						"blue LED D9"
#define LED_0_PIN						LED0_GPIO
#define LED_0_ACTIVE					LED0_ACTIVE_LEVEL
#define LED_0_INACTIVE					LED0_INACTIVE_LEVEL

#define PIN_LED_0						{PIO_PC10, PIOC, ID_PIOC, PIO_OUTPUT_1, PIO_DEFAULT}
#define PIN_LED_0_MASK					PIO_PC10
#define PIN_LED_0_PIO					PIOC
#define PIN_LED_0_ID					ID_PIOC
#define PIN_LED_0_TYPE					PIO_OUTPUT_1
#define PIN_LED_0_ATTR					PIO_DEFAULT

/** LED #1 pin definition (GREEN). */
#define LED1							LED1_GPIO
#define LED_1_NAME						"green LED D10"
#define LED_1_PIN						LED1_GPIO
#define LED_1_ACTIVE					LED1_ACTIVE_LEVEL
#define LED_1_INACTIVE					LED1_INACTIVE_LEVEL

#define PIN_LED_1						{PIO_PC17, PIOC, ID_PIOC, PIO_OUTPUT_1, PIO_DEFAULT}
#define PIN_LED_1_MASK					PIO_PC17
#define PIN_LED_1_PIO					PIOC
#define PIN_LED_1_ID					ID_PIOC
#define PIN_LED_1_TYPE					PIO_OUTPUT_1
#define PIN_LED_1_ATTR					PIO_DEFAULT

#define BOARD_NUM_OF_LED				2

/** Push button #0 definition. Attributes = pull-up + debounce + interrupt on rising edge. */
#define BUTTON_0_NAME					"BP2"
#define BUTTON_0_PIN					SW0_PIN
#define BUTTON_0_ACTIVE					SW0_ACTIVE
#define BUTTON_0_INACTIVE				SW0_INACTIVE
#define BUTTON_0_SUPC_INPUT				SW0_SUPC_INPUT

#define PUSHBUTTON_1_NAME				BUTTON_0_NAME
#define PIN_PUSHBUTTON_1				{PIO_PA5, PIOA, ID_PIOA, PIO_INPUT, PIO_PULLUP | PIO_DEBOUNCE | PIO_IT_RISE_EDGE}
#define PIN_PUSHBUTTON_1_MASK			PIO_PA5
#define PIN_PUSHBUTTON_1_PIO			PIOA
#define PIN_PUSHBUTTON_1_ID				ID_PIOA
#define PIN_PUSHBUTTON_1_TYPE			PIO_INPUT
#define PIN_PUSHBUTTON_1_ATTR			(PIO_PULLUP | PIO_DEBOUNCE | PIO_IT_RISE_EDGE)

#define PINS_PUSHBUTTONS				PIN_PUSHBUTTON_1

#define PIN_TC0_TIOA0       (PIO_PA0_IDX)
#define PIN_TC0_TIOA0_MUX   (IOPORT_MODE_MUX_B)
#define PIN_TC0_TIOA0_FLAGS (PIO_PERIPH_B | PIO_DEFAULT)

#define PIN_TC0_TIOA1       (PIO_PA15_IDX)
#define PIN_TC0_TIOA1_MUX   (IOPORT_MODE_MUX_B)
#define PIN_TC0_TIOA1_FLAGS (PIO_PERIPH_B | PIO_DEFAULT)

#define PIN_TC0_TIOA1_PIO   PIOA
#define PIN_TC0_TIOA1_MASK  PIO_PA15
#define PIN_TC0_TIOA1_ID    ID_PIOA
#define PIN_TC0_TIOA1_TYPE  PIO_PERIPH_B
#define PIN_TC0_TIOA1_ATTR  PIO_DEFAULT

#define PIN_TC0_TIOA2       (PIO_PA26_IDX)
#define PIN_TC0_TIOA2_MUX   (IOPORT_MODE_MUX_B)
#define PIN_TC0_TIOA2_FLAGS (PIO_PERIPH_B | PIO_DEFAULT)

#define PIN_TC0_TIOA2_PIO   PIOA
#define PIN_TC0_TIOA2_MASK  PIO_PA26
#define PIN_TC0_TIOA2_ID    ID_PIOA
#define PIN_TC0_TIOA2_TYPE  PIO_INPUT
#define PIN_TC0_TIOA2_ATTR  PIO_DEFAULT

/** SPI MISO pin definition. */
#define SPI_MISO_GPIO					(PIO_PA12_IDX)
#define SPI_MISO_FLAGS					(IOPORT_MODE_MUX_A)
/** SPI MOSI pin definition. */
#define SPI_MOSI_GPIO					(PIO_PA13_IDX)
#define SPI_MOSI_FLAGS					(IOPORT_MODE_MUX_A)
/** SPI SPCK pin definition. */
#define SPI_SPCK_GPIO					(PIO_PA14_IDX)
#define SPI_SPCK_FLAGS					(IOPORT_MODE_MUX_A)

/** SPI chip select 0 pin definition. (Only one configuration is possible) */
#define SPI_NPCS0_GPIO					(PIO_PA11_IDX)
#define SPI_NPCS0_FLAGS					(IOPORT_MODE_MUX_A)
/** SPI chip select 1 pin definition. (multiple configurations are possible) */
#define SPI_NPCS1_PA9_GPIO				(PIO_PA9_IDX)
#define SPI_NPCS1_PA9_FLAGS				(IOPORT_MODE_MUX_B)
#define SPI_NPCS1_PA31_GPIO				(PIO_PA31_IDX)
#define SPI_NPCS1_PA31_FLAGS			(IOPORT_MODE_MUX_A)
#define SPI_NPCS1_PB14_GPIO				(PIO_PB14_IDX)
#define SPI_NPCS1_PB14_FLAGS			(IOPORT_MODE_MUX_A)
#define SPI_NPCS1_PC4_GPIO				(PIO_PC4_IDX)
#define SPI_NPCS1_PC4_FLAGS				(IOPORT_MODE_MUX_A)
/** SPI chip select 2 pin definition. (multiple configurations are possible) */
#define SPI_NPCS2_PA10_GPIO				(PIO_PA10_IDX)
#define SPI_NPCS2_PA10_FLAGS			(IOPORT_MODE_MUX_B)
#define SPI_NPCS2_PA30_GPIO				(PIO_PA30_IDX)
#define SPI_NPCS2_PA30_FLAGS			(IOPORT_MODE_MUX_B)
#define SPI_NPCS2_PB2_GPIO				(PIO_PB2_IDX)
#define SPI_NPCS2_PB2_FLAGS				(IOPORT_MODE_MUX_B)
/** SPI chip select 3 pin definition. (multiple configurations are possible) */
#define SPI_NPCS3_PA3_GPIO				(PIO_PA3_IDX)
#define SPI_NPCS3_PA3_FLAGS				(IOPORT_MODE_MUX_B)
#define SPI_NPCS3_PA5_GPIO				(PIO_PA5_IDX)
#define SPI_NPCS3_PA5_FLAGS				(IOPORT_MODE_MUX_B)
#define SPI_NPCS3_PA22_GPIO				(PIO_PA22_IDX)
#define SPI_NPCS3_PA22_FLAGS			(IOPORT_MODE_MUX_B)

//! \name MXT143E Xplained top module
//@{
#define MXT143E_XPLAINED_TWI            TWI0
#define MXT143E_XPLAINED_SPI            SPI
#define MXT143E_XPLAINED_CS             (PIO_PA31_IDX)
#define MXT143E_XPLAINED_SCK            (PIO_PA14_IDX)
#define MXT143E_XPLAINED_MOSI           (PIO_PA13_IDX)
#define MXT143E_XPLAINED_MISO           (PIO_PA12_IDX)
#define MXT143E_XPLAINED_CHG            (PIO_PB2_IDX)
#define MXT143E_XPLAINED_DC             (PIO_PB3_IDX)
#define MXT143E_XPLAINED_BACKLIGHT      (PIO_PA17_IDX)
#define MXT143E_XPLAINED_LCD_RESET      (PIO_PC13_IDX)
#define MXT143E_XPLAINED_SDA            (PIO_PA3_IDX)
#define MXT143E_XPLAINED_SCL            (PIO_PA4_IDX)
//@}

/** SSC pin Transmitter Data (TD) */
#define PIN_SSC_TD        (PIO_PA17_IDX)
#define PIN_SSC_TD_FLAGS  (PIO_PERIPH_A | PIO_DEFAULT)
/** SSC pin Transmitter Clock (TK) */
#define PIN_SSC_TK        (PIO_PA16_IDX)
#define PIN_SSC_TK_FLAGS  (PIO_PERIPH_A | PIO_DEFAULT)
/** SSC pin Transmitter FrameSync (TF) */
#define PIN_SSC_TF        (PIO_PA15_IDX)
#define PIN_SSC_TF_FLAGS  (PIO_PERIPH_A | PIO_DEFAULT)

/** PCK0 */
#define PIN_PCK0        (PIO_PA6_IDX)
#define PIN_PCK0_MUX    (IOPORT_MODE_MUX_B)
#define PIN_PCK0_FLAGS  (PIO_PERIPH_B | PIO_DEFAULT)

#define PIN_PCK_0_MASK PIO_PA6
#define PIN_PCK_0_PIO  PIOA
#define PIN_PCK_0_ID   ID_PIOA
#define PIN_PCK_0_TYPE PIO_PERIPH_B
#define PIN_PCK_0_ATTR PIO_DEFAULT

#define PIN_PCK1        (PIO_PA17_IDX)
#define PIN_PCK1_FLAGS  (PIO_PERIPH_B | PIO_DEFAULT)

#define PIN_PCK_1_MASK PIO_PA17
#define PIN_PCK_1_PIO  PIOA
#define PIN_PCK_1_ID   ID_PIOA
#define PIN_PCK_1_TYPE PIO_PERIPH_B
#define PIN_PCK_1_ATTR PIO_DEFAULT

/** TWI0 pins definition */
#define TWI0_DATA_GPIO   PIO_PA3_IDX
#define TWI0_DATA_FLAGS  (PIO_PERIPH_A | PIO_DEFAULT)
#define TWI0_CLK_GPIO    PIO_PA4_IDX
#define TWI0_CLK_FLAGS   (PIO_PERIPH_A | PIO_DEFAULT)

/** TWI1 pins definition */
#define TWI1_DATA_GPIO   PIO_PB4_IDX
#define TWI1_DATA_FLAGS  (PIO_PERIPH_A | PIO_DEFAULT)
#define TWI1_CLK_GPIO    PIO_PB5_IDX
#define TWI1_CLK_FLAGS   (PIO_PERIPH_A | PIO_DEFAULT)

/** USART0 pin RX */
#define PIN_USART0_RXD					{PIO_PA5A_RXD0, PIOA, ID_PIOA, PIO_PERIPH_A, PIO_DEFAULT}
#define PIN_USART0_RXD_IDX				(PIO_PA5_IDX)
#define PIN_USART0_RXD_FLAGS			(IOPORT_MODE_MUX_A)
/** USART0 pin TX */
#define PIN_USART0_TXD					{PIO_PA6A_TXD0, PIOA, ID_PIOA, PIO_PERIPH_A, PIO_DEFAULT}
#define PIN_USART0_TXD_IDX				(PIO_PA6_IDX)
#define PIN_USART0_TXD_FLAGS			(IOPORT_MODE_MUX_A)
/** USART0 pin CTS */
#define PIN_USART0_CTS					{PIO_PA8A_CTS0, PIOA, ID_PIOA, PIO_PERIPH_A, PIO_DEFAULT}
#define PIN_USART0_CTS_IDX				(PIO_PA8_IDX)
#define PIN_USART0_CTS_FLAGS			(IOPORT_MODE_MUX_A)
/** USART0 pin RTS */
#define PIN_USART0_RTS					{PIO_PA7A_RTS0, PIOA, ID_PIOA, PIO_PERIPH_A, PIO_DEFAULT}
#define PIN_USART0_RTS_IDX				(PIO_PA7_IDX)
#define PIN_USART0_RTS_FLAGS			(IOPORT_MODE_MUX_A)
/** USART0 pin SCK */
#define PIN_USART0_SCK					{PIO_PA2B_SCK0, PIOA, ID_PIOA, PIO_PERIPH_B, PIO_DEFAULT}
#define PIN_USART0_SCK_IDX				(PIO_PA2_IDX)
#define PIN_USART0_SCK_FLAGS			(IOPORT_MODE_MUX_B)

/** USART1 pin RX */
#define PIN_USART1_RXD					{PIO_PA21A_RXD1, PIOA, ID_PIOA, PIO_PERIPH_A, PIO_DEFAULT}
#define PIN_USART1_RXD_IDX				(PIO_PA21_IDX)
#define PIN_USART1_RXD_FLAGS			(IOPORT_MODE_MUX_A)
/** USART1 pin TX */
#define PIN_USART1_TXD					{PIO_PA22A_TXD1, PIOA, ID_PIOA, PIO_PERIPH_A, PIO_DEFAULT}
#define PIN_USART1_TXD_IDX				(PIO_PA22_IDX)
#define PIN_USART1_TXD_FLAGS			(IOPORT_MODE_MUX_A)
/** USART1 pin CTS */
#define PIN_USART1_CTS					{PIO_PA25A_CTS1, PIOA, ID_PIOA, PIO_PERIPH_A, PIO_DEFAULT}
#define PIN_USART1_CTS_IDX				(PIO_PA25_IDX)
#define PIN_USART1_CTS_FLAGS			(IOPORT_MODE_MUX_A)
/** USART1 pin RTS */
#define PIN_USART1_RTS					{PIO_PA24A_RTS1, PIOA, ID_PIOA, PIO_PERIPH_A, PIO_DEFAULT}
#define PIN_USART1_RTS_IDX				(PIO_PA24_IDX)
#define PIN_USART1_RTS_FLAGS			(IOPORT_MODE_MUX_A)
/** USART1 pin ENABLE */
#define PIN_USART1_EN					{PIO_PA23A_SCK1, PIOA, ID_PIOA, PIO_OUTPUT_0, PIO_DEFAULT}
#define PIN_USART1_EN_IDX				(PIO_PA23_IDX)
#define PIN_USART1_EN_FLAGS				(PIO_OUTPUT_0 | PIO_DEFAULT)
/** USART1 pin SCK */
#define PIN_USART1_SCK					{PIO_PA23A_SCK1, PIOA, ID_PIOA, PIO_PERIPH_A, PIO_DEFAULT}
#define PIN_USART1_SCK_IDX				(PIO_PA23_IDX)
#define PIN_USART1_SCK_FLAGS			(IOPORT_MODE_MUX_A)

/** Definition of MMA7341L x,y,z axis channel number */
#define MMA7341L_ADC_CHANNEL_X  2
#define MMA7341L_ADC_CHANNEL_Y  6
#define MMA7341L_ADC_CHANNEL_Z  7

/** MMA7341L mode set pin definition. */
#define PIN_MMA7341L_MODE      PIO_PC13_IDX
#define PIN_MMA7341L_MODE_FLAG PIO_OUTPUT_1 | PIO_DEFAULT

/** MMA7341L X,Y,Z axis pin definition. */
#define PIN_MMA7341L_X_AXIS      PIO_PB3_IDX
#define PIN_MMA7341L_X_AXIS_FLAG PIO_INPUT | PIO_DEFAULT
#define PIN_MMA7341L_Y_AXIS      PIO_PC17_IDX
#define PIN_MMA7341L_Y_AXIS_FLAG PIO_INPUT | PIO_DEFAULT
#define PIN_MMA7341L_Z_AXIS      PIO_PC18_IDX
#define PIN_MMA7341L_Z_AXIS_FLAG PIO_INPUT | PIO_DEFAULT
/*----------------------------------------------------------------------------*/
#define CONSOLE_UART					UART0
#define CONSOLE_UART_ID					ID_UART0

/* RE pin. */
#define PIN_RE_IDX                 PIN_USART1_CTS_IDX
#define PIN_RE_FLAGS               (PIO_OUTPUT_0 | PIO_DEFAULT)

/* IRDA SD pin. */
#define PIN_IRDA_SD_IDX            PIN_USART1_CTS_IDX
#define PIN_IRDA_SD_FLAGS          (PIO_OUTPUT_1 | PIO_DEFAULT)

/* TXD pin configuration. */
#define PIN_USART_TXD_IDX          PIN_USART1_TXD_IDX
#define PIN_USART_TXD_FLAGS        (PIO_PERIPH_A | PIO_DEFAULT)
#define PIN_USART_TXD_IO_FLAGS     (PIO_OUTPUT_0 | PIO_DEFAULT)

/*----------------------------------------------------------------------------*/
/**
 * \page sam4s_ek_zigbee "SAM4S-EK - Zigbee / IEEE 802.15.4 MAC"
 * This page lists definitions related to internal & external on-board memories.
 *
 */

#define AT86RFX_SPI						SPI

#define AT86RFX_SLP						{PIO_PB3A_UTXD1, PIOB, ID_PIOB, PIO_OUTPUT_1, PIO_PULLUP}
#define AT86RFX_SLP_PIN					PIO_PB3_IDX
#define AT86RFX_SLP_FLAGS				(PIO_PULLUP | PIO_OUTPUT_1)
#define AT86RFX_SLP_MASK				PIO_PB3
#define AT86RFX_SLP_PIO					PIOB
#define AT86RFX_SLP_ID					ID_PIOB
#define AT86RFX_SLP_TYPE				PIO_PERIPH_A
#define AT86RFX_SLP_ATTR				PIO_DEFAULT

#define AT86RFX_IRQ0					{PIO_PA4A_TWCK0, PIOA, ID_PIOA, PIO_INPUT, PIO_PULLUP}
#define AT86RFX_IRQ0_PIN				PIO_PA4_IDX
#define AT86RFX_IRQ0_FLAGS				(PIO_PULLUP | PIO_IT_RISE_EDGE)
#define AT86RFX_IRQ0_MASK				PIO_PA4
#define AT86RFX_IRQ0_PIO				PIOA
#define AT86RFX_IRQ0_ID					ID_PIOA
#define AT86RFX_IRQ0_TYPE				PIO_PERIPH_A
#define AT86RFX_IRQ0_ATTR				(PIO_PULLUP | PIO_IT_RISE_EDGE)

#define AT86RFX_IRQ1					{PIO_PB2A_URXD1, PIOB, ID_PIOB, PIO_INPUT, PIO_PULLUP}
#define AT86RFX_IRQ1_PIN				PIO_PB2_IDX
#define AT86RFX_IRQ1_FLAGS				(PIO_PULLUP | PIO_IT_RISE_EDGE)
#define AT86RFX_IRQ1_MASK				PIO_PB2
#define AT86RFX_IRQ1_PIO				PIOB
#define AT86RFX_IRQ1_ID					ID_PIOB
#define AT86RFX_IRQ1_TYPE				PIO_PERIPH_A
#define AT86RFX_IRQ1_ATTR				(PIO_PULLUP | PIO_IT_RISE_EDGE)

#define AT86RFX_RST						{PIO_PA23A_SCK1, PIOA, ID_PIOA, PIO_OUTPUT_1, PIO_PULLUP}
#define AT86RFX_RST_PIN					PIO_PA3_IDX
#define AT86RFX_RST_FLAGS				(PIO_PULLUP | PIO_OUTPUT_1)
#define AT86RFX_RST_MASK				PIO_PA3
#define AT86RFX_RST_PIO					PIOA
#define AT86RFX_RST_ID					ID_PIOA
#define AT86RFX_RST_TYPE				PIO_PERIPH_A
#define AT86RFX_RST_ATTR				PIO_DEFAULT

#define AT86RFX_CSD		
#define AT86RFX_CSD_PIN
#define AT86RFX_CSD_FLAGS				
#define AT86RFX_CSD_MASK				
#define AT86RFX_CSD_PIO					
#define AT86RFX_CSD_ID					
#define AT86RFX_CSD_TYPE				
#define AT86RFX_CSD_ATTR				

#define AT86RFX_CPS
#define AT86RFX_CPS_PIN
#define AT86RFX_CPS_FLAGS				
#define AT86RFX_CPS_MASK				
#define AT86RFX_CPS_PIO					
#define AT86RFX_CPS_ID					
#define AT86RFX_CPS_TYPE				
#define AT86RFX_CPS_ATTR				

#define AT86RFX_CS						{PIO_PA31A_NPCS1, PIOA, ID_PIOA, PIO_OUTPUT_1, PIO_PULLUP}
#define AT86RFX_CS_PIN					PIO_PA31_IDX
#define AT86RFX_CS_FLAGS				(PIO_PULLUP | PIO_OUTPUT_1)
#define AT86RFX_CS_MASK					PIO_PA31
#define AT86RFX_CS_PIO					PIOA
#define AT86RFX_CS_ID					ID_PIOA
#define AT86RFX_CS_TYPE					IOPORT_MODE_MUX_A
#define AT86RFX_CS_ATTR					(PIO_PULLUP | PIO_OUTPUT_1)

#define AT86RFX_SPI_CS					1
#define AT86RFX_SPI_MOSI_GPIO			SPI_MOSI_GPIO
#define AT86RFX_SPI_MOSI_FLAGS			SPI_MOSI_FLAGS
#define AT86RFX_SPI_MISO_GPIO			SPI_MISO_GPIO
#define AT86RFX_SPI_MISO_FLAGS			SPI_MISO_FLAGS
#define AT86RFX_SPI_SPCK_GPIO			SPI_SPCK_GPIO
#define AT86RFX_SPI_SPCK_FLAGS			SPI_SPCK_FLAGS

#define AT86RFX_INTC_INIT()				ioport_set_pin_mode(AT86RFX_IRQ1_PIN, AT86RFX_IRQ1_FLAGS);\
										ioport_set_pin_dir(AT86RFX_IRQ1_PIN, IOPORT_DIR_INPUT);\
										ioport_set_pin_sense_mode(AT86RFX_IRQ1_PIN, IOPORT_SENSE_RISING);\
										ioport_enable_pin(AT86RFX_IRQ1_PIN);\
										pio_configure_interrupt(AT86RFX_IRQ1_PIO, AT86RFX_IRQ1_MASK, AT86RFX_IRQ1_ATTR);\
										NVIC_DisableIRQ(PIOB_IRQn);\
										NVIC_ClearPendingIRQ(PIOB_IRQn);\
										NVIC_EnableIRQ(PIOB_IRQn);\
										pio_enable_interrupt(AT86RFX_IRQ1_PIO, AT86RFX_IRQ1_MASK);

#define AT86RFX_ISR()					ISR(PIOB_Handler)

/** Enables the transceiver main interrupt. */
#define ENABLE_TRX_IRQ()				pio_enable_interrupt(AT86RFX_IRQ1_PIO, AT86RFX_IRQ1_MASK);

/** Disables the transceiver main interrupt. */
#define DISABLE_TRX_IRQ()				pio_disable_interrupt(AT86RFX_IRQ1_PIO, AT86RFX_IRQ1_MASK);

/** Clears the transceiver main interrupt. */
#define CLEAR_TRX_IRQ()					pio_get_interrupt_status(AT86RFX_IRQ1_PIO)

/*
 * This macro saves the trx interrupt status and disables the trx interrupt.
 */
#define ENTER_TRX_REGION()				NVIC_DisableIRQ(PIOB_IRQn)

/*
 *  This macro restores the transceiver interrupt status
 */
#define LEAVE_TRX_REGION()				NVIC_EnableIRQ(PIOB_IRQn)

/*Board has an External PA Connected to the Radio, Following are the constants related to the Ext PA */
#define ANTENNA_DIVERSITY				(0)
#define ANTENNA_DEFAULT					(0)

#endif  /* SAM4S_XPLAINED_H_INCLUDED */

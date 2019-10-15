/**
 * \file
 *
 * \brief WM400 board definition
 *
 * Copyright (c) 2014 Atmel Corporation. All rights reserved.
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

#ifndef _WM400_H_
#define _WM400_H_

#include <conf_board.h>

/**
 * \ingroup group_common_boards
 * \defgroup wm400_group WM400 board
 *
 * @{
 */

/**
 * \defgroup wm400_config_group Configuration
 *
 * Symbols to use for configuring the board and its initialization.
 *
 * @{
 */
#ifdef __DOXYGEN__

//! \name Initialization
//@{

/**
 * \def CONF_BOARD_KEEP_WATCHDOG_AT_INIT
 * \brief Let watchdog remain enabled
 *
 * If this symbol is defined, the watchdog is left running with its current
 * configuration. Otherwise, it gets disabled during board initialization.
 */
# ifndef CONF_BOARD_KEEP_WATCHDOG_AT_INIT
#  define CONF_BOARD_KEEP_WATCHDOG_AT_INIT
# endif

//@}

#endif // __DOXYGEN__
/** @} */

/**
 * \defgroup wm400_features_group Features
 *
 * Symbols that describe features and capabilities of the board.
 *
 * @{
 */

//! Name string macro
#define BOARD_NAME						"WM-400L-BBBS"
#define MCU_SOC_NAME					"ATSAM4LC4B"

//! \name Board oscillator definitions
//@{
//! Osc frequency (Hz.) and startup time (RCOsc periods)
#define FOSC0							(12000000UL)
//! Osc32 frequency (Hz.) and startup time (RCOsc periods)
#define FOSC32							(32768UL)
#define BOARD_OSC32_IS_XTAL				true
#define BOARD_OSC32_HZ					FOSC32
#define BOARD_OSC32_STARTUP_US			(750000UL)
#define BOARD_OSC32_SELCURR				BSCIF_OSCCTRL32_SELCURR(10)
#define BOARD_OSC0_IS_XTAL				true
#define BOARD_OSC0_HZ					FOSC0
#define BOARD_OSC0_STARTUP_US			(6100UL)
//@}

//! \name LED0 definitions
//@{
#define LED0							PIN_PA08
#define LED0_PIN						PIN_PA08 /* Wrapper definition */
#define LED0_GPIO						PIN_PA08 /* Wrapper definition */
#define LED0_ACTIVE						false
#define LED0_INACTIVE					!LED0_ACTIVE
#define LED1							PIN_PB06
#define LED1_PIN						PIN_PB06 /* Wrapper definition */
#define LED1_GPIO						PIN_PB06 /* Wrapper definition */
#define LED1_ACTIVE						false
#define LED1_INACTIVE					!LED1_ACTIVE
#define LED2							PIN_PB07
#define LED2_PIN						PIN_PB07 /* Wrapper definition */
#define LED2_GPIO						PIN_PB07 /* Wrapper definition */
#define LED2_ACTIVE						false
#define LED2_INACTIVE					!LED2_ACTIVE
//@}

//! \name SW0 definitions
//@{
#define GPIO_PUSH_BUTTON_0				PIN_PB03 /* Wrapper definition */
#define SW0_PIN							PIN_PB03
#define SW0_ACTIVE						false
#define SW0_INACTIVE					!SW0_ACTIVE
//#define SW0_EIC_PIN					PIN_PC24B_EIC_EXTINT1
//#define SW0_EIC_PIN_MUX				MUX_PC24B_EIC_EXTINT1
//#define SW0_EIC_LINE					1
////@}

//! \name TOUCH QTouch button definitions
//@{
#define QTOUCH_DISCHARGE_PIN			PIN_PC14
#define QTOUCH_DISCHARGE_MUX			MUX_PC14G_CATB_DIS
#define QTOUCH_SENSE_PIN				PIN_PC13
#define QTOUCH_SENSE_MUX				MUX_PC13G_CATB_SENSE15
#define QTOUCH_PINSEL_BUTTON			15
//@}

/**
 * \name USB pin definitions
 *
 * These are named according to the USBC driver's requirements.
 */
//@{
#define USB_ID_PIN						PIN_PB11
#define USB_ID_FLAGS					(IOPORT_MODE_PULLUP | IOPORT_MODE_GLITCH_FILTER)
#define USB_VBUS_PIN					PIN_PA18
#define USB_VBUS_FLAGS					IOPORT_MODE_GLITCH_FILTER
#define USB_VBOF_PIN					PIN_PB02
#define USB_VBOF_ACTIVE_LEVEL			1
#define USB_VBOF_INACTIVE_LEVEL			0
#define USB_OC_PIN						PIN_PA23
#define USB_OC_FLAGS					(IOPORT_MODE_PULLUP | IOPORT_MODE_GLITCH_FILTER)
//@}

//! \name USART connections to GPIO for Virtual Com Port
// @{
#define COM_PORT_USART					USART0
#define COM_PORT_USART_ID				ID_USART0
#define COM_PORT_RX_PIN					PIN_PB14A_USART0_RXD
#define COM_PORT_RX_GPIO				GPIO_PB14A_USART0_RXD
#define COM_PORT_RX_MUX					MUX_PB14A_USART0_RXD
#define COM_PORT_TX_PIN					PIN_PB15A_USART0_TXD
#define COM_PORT_TX_GPIO				GPIO_PB15A_USART0_TXD
#define COM_PORT_TX_MUX					MUX_PB15A_USART0_TXD
// @}

//! \name USART connections to the RS485
// @{
#define RS485_USART_USART				USART0
#define RS485_USART_USART_ID			ID_USART0
#define RS485_USART_RX_PIN				PIN_PB14A_USART0_RXD
#define RS485_USART_RX_GPIO				GPIO_PB14A_USART0_RXD
#define RS485_USART_RX_MUX				MUX_PB14A_USART0_RXD
#define RS485_USART_TX_PIN				PIN_PB15A_USART0_TXD
#define RS485_USART_TX_GPIO				GPIO_PB15A_USART0_TXD
#define RS485_USART_TX_MUX				MUX_PB15A_USART0_TXD
#define RS485_USART_RTS_PIN				PIN_PB12A_USART0_RTS
#define RS485_USART_RTS_GPIO			GPIO_PB12A_USART0_RTS
#define RS485_USART_RTS_MUX				MUX_PB12A_USART0_RTS
#define RS485_USART_CTS_PIN				PIN_PB11A_USART0_CTS
#define RS485_USART_CTS_GPIO			GPIO_PB11A_USART0_CTS
#define RS485_USART_CTS_MUX				MUX_PB11A_USART0_CTS
// @}

//! \name USART connections to GPIO for WM design USART
// @{
#define WM_PORT_USART					USART1
#define WM_PORT_USART_ID				ID_USART1
#define WM_PORT_RX_PIN					PIN_PA15A_USART1_RXD
#define WM_PORT_RX_GPIO					GPIO_PA15A_USART1_RXD
#define WM_PORT_RX_MUX					MUX_PA15A_USART1_RXD
#define WM_PORT_TX_PIN					PIN_PA16A_USART1_TXD
#define WM_PORT_TX_GPIO					GPIO_PA16A_USART1_TXD
#define WM_PORT_TX_MUX					MUX_PA16A_USART1_TXD
// @}

/**
 * \name LED #0 definitions
 *
 * Wrapper macros for LED0, to ensure common naming across all Xplained Pro
 * boards.
 */
//@{
#define LED_0_NAME						"LED0"
#define LED_0_PIN						LED0_PIN
#define LED_0_ACTIVE					LED0_ACTIVE
#define LED_0_INACTIVE					LED0_INACTIVE
#define LED_1_NAME						"LED1"
#define LED_1_PIN						LED1_PIN
#define LED_1_ACTIVE					LED1_ACTIVE
#define LED_1_INACTIVE					LED1_INACTIVE
#define LED_2_NAME						"LED2"
#define LED_2_PIN						LED2_PIN
#define LED_2_ACTIVE					LED2_ACTIVE
#define LED_2_INACTIVE					LED2_INACTIVE
//@}

//! Number of on-board LEDs
#define LED_COUNT						3

/**
 * \name Button #0 definitions
 *
 * Wrapper macros for SW0, to ensure common naming across all Xplained Pro
 * boards.
 */
//@{
#define BUTTON_0_NAME					"SW0"
#define BUTTON_0_PIN					SW0_PIN
#define BUTTON_0_ACTIVE					SW0_ACTIVE
#define BUTTON_0_INACTIVE				SW0_INACTIVE

//! Number of on-board buttons
#define BUTTON_COUNT					1

//! \name TWIMS1 pins
// @{
#define TWIMS1_TWI_SDA_PIN				PIN_PB00A_TWIMS1_TWD
#define TWIMS1_TWI_SDA_GPIO				GPIO_PB00A_TWIMS1_TWD
#define TWIMS1_TWI_SDA_MUX				MUX_PB00A_TWIMS1_TWD
#define TWIMS1_TWI_SCL_PIN				PIN_PB01A_TWIMS1_TWCK
#define TWIMS1_TWI_SCL_GPIO				GPIO_PB01A_TWIMS1_TWCK
#define TWIMS1_TWI_SCL_MUX				MUX_PB01A_TWIMS1_TWCK
// @}

//! \name ADCIFE pins
// @{
#define ADCIFE_AD2_PIN					PIN_PA07A_ADCIFE_AD2
#define ADCIFE_AD2_GPIO					GPIO_PA07A_ADCIFE_AD2
#define ADCIFE_AD2_MUX					MUX_PA07A_ADCIFE_AD2
#define ADCIFE_AD5_PIN					PIN_PB04A_ADCIFE_AD5
#define ADCIFE_AD5_GPIO					GPIO_PB04A_ADCIFE_AD5
#define ADCIFE_AD5_MUX					MUX_PB04A_ADCIFE_AD5
#define ADCIFE_AD6_PIN					PIN_PB05A_ADCIFE_AD6
#define ADCIFE_AD6_GPIO					GPIO_PB05A_ADCIFE_AD6
#define ADCIFE_AD6_MUX					MUX_PB05A_ADCIFE_AD6
// @}

//! \name DACC pin
// @{
#define DACC_VOUT_PIN					PIN_PA06A_DACC_VOUT
#define DACC_VOUT_GPIO					GPIO_PA06A_DACC_VOUT
#define DACC_VOUT_MUX					MUX_PA06A_DACC_VOUT
// @}

/*! \name Connections of the AT86RFX transceiver
 */
//! @{
#define AT86RFX_SPI						SPI
#define AT86RFX_SPI_BAUDRATE			(3000000)
#define AT86RFX_RST_PIN					PIN_PA10
#define AT86RFX_IRQ0_PIN				PIN_PA20
#define AT86RFX_IRQ_PIN					AT86RFX_IRQ0_PIN
#define AT86RFX_IRQ0_NUMBER				GPIO_2_IRQn
#define AT86RFX_IRQ0_HANDLER			GPIO_2_Handler
#define AT86RFX_IRQ1_PIN				PIN_PA08
#define AT86RFX_SLP_PIN					PIN_PA09
//#define AT86RFX_SPI_CS_PIN			PIN_PA13
// See SPI definitions NPCS_1
#define AT86RFX_SPI_CS					1
#define AT86RFX_SPI_MOSI				PIN_PA22
#define AT86RFX_SPI_MISO				PIN_PA21
#define AT86RFX_SPI_SCK					PIN_PA23
#define AT86RFX_CSD						PIN_PA11
//#define AT86RFX_CPS					PIN_PA11

#define EXT_RF_FRONT_END_CTRL			1
#define ANTENNA_DIVERSITY				0
#define ANTENNA_DEFAULT					1	// [1|2]
#define CUSTOM_PWR_TABLE				1
#define CUSTOM_DEFAULT_TX_PWR			1

#ifdef EXT_RF_FRONT_END_CTRL /*For External PA for 233FEM*/
#define EXT_PA_SE2431L
/*
 * Value of an external LNA gain.
 * If no external LNA is available, the value is 0.
 */
#define EXT_LNA_HIGH_GAIN				(14)
#endif

#ifdef CUSTOM_DEFAULT_TX_PWR /*For External PA for 233FEM*/
/*
 * Default value of transmit power of transceiver: Preset
 *    - definition acct. IEEE802.15.4 PHY PIB attribute phyTransmitPower
 *    - TX Pout init value based on validation
 */
#define TAL_TRANSMIT_POWER_DEFAULT		(TX_PWR_TOLERANCE | 0x14)
#endif

#define AT86RFX_INTC_INIT()				ioport_set_pin_dir(AT86RFX_IRQ0_PIN, IOPORT_DIR_INPUT);\
										ioport_set_pin_sense_mode(AT86RFX_IRQ0_PIN, IOPORT_SENSE_RISING);\
										arch_ioport_pin_to_base(AT86RFX_IRQ0_PIN)->GPIO_IERS = arch_ioport_pin_to_mask(AT86RFX_IRQ0_PIN);\
										arch_ioport_pin_to_base(AT86RFX_IRQ0_PIN)->GPIO_IMR0S = arch_ioport_pin_to_mask(AT86RFX_IRQ0_PIN);\
										NVIC_EnableIRQ(AT86RFX_IRQ0_NUMBER)

#define AT86RFX_ISR()					ISR(AT86RFX_IRQ0_HANDLER)

/** Enables the transceiver main interrupt. */
#define ENABLE_TRX_IRQ()				arch_ioport_pin_to_base(AT86RFX_IRQ0_PIN)->GPIO_IERS = arch_ioport_pin_to_mask(AT86RFX_IRQ0_PIN)

/** Disables the transceiver main interrupt. */
#define DISABLE_TRX_IRQ()				arch_ioport_pin_to_base(AT86RFX_IRQ0_PIN)->GPIO_IERC = arch_ioport_pin_to_mask(AT86RFX_IRQ0_PIN)

/** Clears the transceiver main interrupt. */
#define CLEAR_TRX_IRQ()					arch_ioport_pin_to_base(AT86RFX_IRQ0_PIN)->GPIO_IFRC = arch_ioport_pin_to_mask(AT86RFX_IRQ0_PIN)

/*
 * This macro saves the trx interrupt status and disables the trx interrupt.
 */
#define ENTER_TRX_REGION()				NVIC_DisableIRQ(AT86RFX_IRQ0_NUMBER)

/*
 *  This macro restores the transceiver interrupt status
 */
#define LEAVE_TRX_REGION()				NVIC_EnableIRQ(AT86RFX_IRQ0_NUMBER)

#define CONF_BOARD_SPI_NPCS1_GPIO		(PIN_PA13C_SPI_NPCS1)
#define CONF_BOARD_SPI_NPCS1_FLAGS		(MUX_PA13C_SPI_NPCS1)
#define SPI_MISO_GPIO					(PIN_PA21A_SPI_MISO)
#define SPI_MISO_FLAGS					(MUX_PA21A_SPI_MISO)
#define SPI_MOSI_GPIO					(PIN_PA22A_SPI_MOSI)
#define SPI_MOSI_FLAGS					(MUX_PA22A_SPI_MOSI)
#define SPI_SPCK_GPIO					(PIN_PA23A_SPI_SCK)
#define SPI_SPCK_FLAGS					(MUX_PA23A_SPI_SCK)

//! @}

/** @} */

// Device List
#define VCNL40XX_DEVICE					(0x13)
#define CC2D33S_DEVICE					(0x28)
#define AT30TSE752_TEMP_DEVICE			(0x4A)
#define AT30TSE752_MEM_DEVICE			(0x52)
#define AT24MAC602_MEM_DEVICE			(0x57)
#define AT24MAC602_EUI_DEVICE			(0x5F)
#define MPL115A2_DEVICE					(0x60)

#endif /* _WM400_H_ */
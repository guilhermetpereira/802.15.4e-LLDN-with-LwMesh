/**
 * \file
 *
 * \brief SAM4S-EK board init.
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

#include <board.h>
#include <ioport.h>
#include <wdt_sam4l.h>
#include "conf_board.h"

/**
 * \brief Set peripheral mode for one single IOPORT pin.
 * It will configure port mode and disable pin mode (but enable peripheral).
 * \param pin IOPORT pin to configure
 * \param mode Mode masks to configure for the specified pin (\ref ioport_modes)
 */
#define ioport_set_pin_peripheral_mode(pin, mode) \
	do {\
		ioport_set_pin_mode(pin, mode);\
		ioport_disable_pin(pin);\
	} while (0)

void board_init(void)
{
#ifndef CONF_BOARD_KEEP_WATCHDOG_AT_INIT
	struct wdt_dev_inst wdt_inst;
	struct wdt_config   wdt_cfg;

	wdt_get_config_defaults(&wdt_cfg);
	wdt_init(&wdt_inst, WDT, &wdt_cfg);
	wdt_disable(&wdt_inst);
#endif

	/* GPIO has been deprecated, the old code just keeps it for compatibility.
	 * In new designs IOPORT is used instead.
	 * Here IOPORT must be initialized for others to use before setting up IO.
	 */
	ioport_init();

	/* Configure LED pins */
	ioport_set_pin_dir(LED_0_PIN, IOPORT_DIR_OUTPUT);
	ioport_set_pin_level(LED_0_PIN, IOPORT_PIN_LEVEL_HIGH);
	ioport_set_pin_dir(LED_1_PIN, IOPORT_DIR_OUTPUT);
	ioport_set_pin_level(LED_1_PIN, IOPORT_PIN_LEVEL_HIGH);
	ioport_set_pin_dir(LED_2_PIN, IOPORT_DIR_OUTPUT);
	ioport_set_pin_level(LED_2_PIN, IOPORT_PIN_LEVEL_HIGH);

	/* Configure Push Button pins */
	ioport_set_pin_dir(BUTTON_0_PIN, IOPORT_DIR_INPUT);
	ioport_set_pin_mode(BUTTON_0_PIN, IOPORT_MODE_PULLUP);

#ifdef CONF_BOARD_ADC_AD2
	ioport_set_pin_peripheral_mode(ADCIFE_AD2_PIN, ADCIFE_AD2_MUX);
#endif
#ifdef CONF_BOARD_ADC_AD5
	ioport_set_pin_peripheral_mode(ADCIFE_AD5_PIN, ADCIFE_AD5_MUX);
#endif
#ifdef CONF_BOARD_ADC_AD6
	ioport_set_pin_peripheral_mode(ADCIFE_AD6_PIN, ADCIFE_AD6_MUX);
#endif

#ifdef CONF_BOARD_PWM_LED0
	/* Configure PWM LED0 pin */
	gpio_configure_pin(PIN_PWM_LED0_GPIO, PIN_PWM_LED0_FLAGS);
#endif

#ifdef CONF_BOARD_PWM_LED1
	/* Configure PWM LED1 pin */
	gpio_configure_pin(PIN_PWM_LED1_GPIO, PIN_PWM_LED1_FLAGS);
#endif

#ifdef CONF_BOARD_PWM_LED2
	/* Configure PWM LED2 pin */
	gpio_configure_pin(PIN_PWM_LED2_GPIO, PIN_PWM_LED2_FLAGS);
#endif

#if defined (CONF_BOARD_COM_PORT)
	ioport_set_pin_peripheral_mode(COM_PORT_RX_PIN, COM_PORT_RX_MUX);
	ioport_set_pin_peripheral_mode(COM_PORT_TX_PIN, COM_PORT_TX_MUX);
#endif

#ifdef CONF_BOARD_RS485
	ioport_set_pin_peripheral_mode(RS485_USART_RX_PIN, RS485_USART_RX_MUX);
	ioport_set_pin_peripheral_mode(RS485_USART_TX_PIN, RS485_USART_TX_MUX);
	ioport_set_pin_peripheral_mode(RS485_USART_RTS_PIN, RS485_USART_RTS_MUX);
	ioport_set_pin_dir(RS485_USART_CTS_PIN, IOPORT_DIR_OUTPUT);
	ioport_set_pin_level(RS485_USART_CTS_PIN, IOPORT_PIN_LEVEL_LOW);
#endif

#if defined (CONF_BOARD_WM_PORT)
ioport_set_pin_peripheral_mode(WM_PORT_RX_PIN, WM_PORT_RX_MUX);
ioport_set_pin_peripheral_mode(WM_PORT_TX_PIN, WM_PORT_TX_MUX);
#endif

#if defined (CONF_BOARD_TWIMS1)
	ioport_set_pin_peripheral_mode(TWIMS1_TWI_SCL_PIN, TWIMS1_TWI_SCL_MUX);
	ioport_set_pin_peripheral_mode(TWIMS1_TWI_SDA_PIN, TWIMS1_TWI_SDA_MUX);
#endif

#if defined (CONF_BOARD_USART0)
	ioport_set_pin_peripheral_mode(EXT1_PIN_UART_RX, EXT1_UART_RX_MUX);
	ioport_set_pin_peripheral_mode(EXT1_PIN_UART_TX, EXT1_UART_TX_MUX);
#endif

#if (defined CONF_BOARD_USB_PORT)
	ioport_set_pin_peripheral_mode(PIN_PA25A_USBC_DM, MUX_PA25A_USBC_DM);
	ioport_set_pin_peripheral_mode(PIN_PA26A_USBC_DP, MUX_PA26A_USBC_DP);

	#if defined(CONF_BOARD_USB_VBUS_DETECT)
		#if defined(USB_VBUS_EIC)
			ioport_set_pin_peripheral_mode(USB_VBUS_EIC, USB_VBUS_EIC_MUX | USB_VBUS_FLAGS);
		#elif defined(USB_VBUS_PIN)
			ioport_set_pin_dir(USB_VBUS_PIN, IOPORT_DIR_INPUT);
		#else
			#warning USB_VBUS pin not defined
		#endif		/* !(defined USB_VBUS_EIC) */
	#endif			/* !(defined CONF_BOARD_USB_VBUS_DETECT) */
	#if defined(CONF_BOARD_USB_ID_DETECT)
		#if defined(USB_ID_EIC)
			ioport_set_pin_peripheral_mode(USB_ID_EIC, USB_ID_EIC_MUX | USB_ID_FLAGS);
		#elif defined(USB_ID_PIN)
			ioport_set_pin_dir(USB_ID_PIN, IOPORT_DIR_INPUT);
		#else
			#warning USB_ID pin not defined
		#endif		/* !(defined USB_ID_EIC) */
	#endif			/* !(defined CONF_BOARD_USB_ID_DETECT) */
	#if defined(CONF_BOARD_USB_VBUS_CONTROL)
		#if defined(USB_VBOF_PIN)
			ioport_set_pin_dir(USB_VBOF_PIN, IOPORT_DIR_OUTPUT);
			ioport_set_pin_level(USB_VBOF_PIN, USB_VBOF_INACTIVE_LEVEL);
		#else
			#warning USB_VBOF pin not defined
		#endif		/* !(defined USB_VBOF_PIN) */
		#if defined(CONF_BOARD_USB_VBUS_ERR_DETECT)
			#if defined(USB_VBERR_EIC)
				ioport_set_pin_peripheral_mode(USB_VBERR_EIC, USB_VBERR_EIC_MUX | USB_VBERR_FLAGS);
			#elif defined(USB_VBERR_PIN)
				ioport_set_pin_dir(USB_VBERR_PIN, IOPORT_DIR_INPUT);
			#else
				#warning USB_VBERR pin not defined
			#endif
		#endif		/* !(defined CONF_BOARD_USB_VBUS_ERR_DETECT) */
	#endif			/* !(defined CONF_BOARD_USB_NO_VBUS_CONTROL) */
#endif				/* (defined CONF_BOARD_USB_PORT) */

#ifdef CONF_BOARD_AT86RFX
	#define CONF_BOARD_SPI
	#define CONF_BOARD_SPI_NPCS1
//	ioport_set_pin_peripheral_mode(AT86RFX_IRQ0_PIN, AT86RFX_IRQ0_FLAGS);
//	ioport_set_pin_peripheral_mode(AT86RFX_IRQ1_PIN, AT86RFX_IRQ1_FLAGS);

	/* Initialize TRX_RST and SLP_TR as GPIO. */
	ioport_set_pin_dir(AT86RFX_RST_PIN, IOPORT_DIR_OUTPUT);
	ioport_set_pin_level(AT86RFX_RST_PIN, IOPORT_PIN_LEVEL_HIGH);
	ioport_enable_pin(AT86RFX_RST_PIN);
	ioport_set_pin_dir(AT86RFX_SLP_PIN, IOPORT_DIR_OUTPUT);
	ioport_set_pin_level(AT86RFX_SLP_PIN, IOPORT_PIN_LEVEL_LOW);
	ioport_enable_pin(AT86RFX_SLP_PIN);

	#ifdef EXT_RF_FRONT_END_CTRL
//		ioport_set_pin_dir(AT86RFX_CPS_PIN, IOPORT_DIR_OUTPUT);
//		ioport_set_pin_level(AT86RFX_CPS_PIN, IOPORT_PIN_LEVEL_HIGH);
//		ioport_enable_pin(AT86RFX_CPS_PIN);
		ioport_set_pin_dir(AT86RFX_CSD, IOPORT_DIR_OUTPUT);
		ioport_set_pin_level(AT86RFX_CSD, IOPORT_PIN_LEVEL_LOW);
		ioport_enable_pin(AT86RFX_CSD);
	#endif
#endif

	/* Configure SPI pins */
#ifdef CONF_BOARD_SPI
	ioport_set_pin_peripheral_mode(SPI_MISO_GPIO, SPI_MISO_FLAGS);
	ioport_set_pin_peripheral_mode(SPI_MOSI_GPIO, SPI_MOSI_FLAGS);
	ioport_set_pin_peripheral_mode(SPI_SPCK_GPIO, SPI_SPCK_FLAGS);

	/**
	 * For NPCS 1, 2, and 3, different PINs can be used to access the same NPCS line.
	 * Depending on the application requirements, the default PIN may not be available.
	 * Hence a different PIN should be selected using the CONF_BOARD_SPI_NPCS_GPIO and
	 * CONF_BOARD_SPI_NPCS_FLAGS macros.
	 */

#  ifdef CONF_BOARD_SPI_NPCS0
	ioport_set_pin_peripheral_mode(SPI_NPCS0_GPIO, SPI_NPCS0_FLAGS);
#  endif

#  ifdef CONF_BOARD_SPI_NPCS1
#    if defined(CONF_BOARD_SPI_NPCS1_GPIO) && defined(CONF_BOARD_SPI_NPCS1_FLAGS)
	ioport_set_pin_peripheral_mode(CONF_BOARD_SPI_NPCS1_GPIO, CONF_BOARD_SPI_NPCS1_FLAGS);
#    else
	ioport_set_pin_peripheral_mode(SPI_NPCS1_PA31_GPIO, SPI_NPCS1_PA31_FLAGS);
#    endif
#  endif

#  ifdef CONF_BOARD_SPI_NPCS2
#    if defined(CONF_BOARD_SPI_NPCS2_GPIO) && defined(CONF_BOARD_SPI_NPCS2_FLAGS)
	ioport_set_pin_peripheral_mode(CONF_BOARD_SPI_NPCS2_GPIO, CONF_BOARD_SPI_NPCS2_FLAGS);
#    else
	ioport_set_pin_peripheral_mode(SPI_NPCS2_PA30_GPIO, SPI_NPCS2_PA30_FLAGS);
#    endif
#  endif

#  ifdef CONF_BOARD_SPI_NPCS3
#    if defined(CONF_BOARD_SPI_NPCS3_GPIO) && defined(CONF_BOARD_SPI_NPCS3_FLAGS)
	ioport_set_pin_peripheral_mode(CONF_BOARD_SPI_NPCS3_GPIO, CONF_BOARD_SPI_NPCS3_FLAGS);
#    else
	ioport_set_pin_peripheral_mode(SPI_NPCS3_PA22_GPIO, SPI_NPCS3_PA22_FLAGS);
#    endif
#  endif
#endif /* CONF_BOARD_SPI */


#ifdef CONF_BOARD_USART_SCK
	/* Configure USART synchronous communication SCK pin */
	gpio_configure_pin(PIN_USART1_SCK_IDX, PIN_USART1_SCK_FLAGS);
#endif

#if defined(CONF_BOARD_USB_PORT)
# if defined(CONF_BOARD_USB_VBUS_DETECT)
	gpio_configure_pin(USB_VBUS_PIN, USB_VBUS_FLAGS);
# endif
#endif
}

/*
 *  Tenda 3G300M board support
 *
 *  Copyright (C) 2013 Cezary Jackiewicz <cezary.jackiewicz@gmail.com>
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 */

#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/spi/spi.h>
#include <linux/spi/flash.h>
#include <linux/gpio.h>

#include <asm/mach-ralink/machine.h>
#include <asm/mach-ralink/dev-gpio-buttons.h>
#include <asm/mach-ralink/dev-gpio-leds.h>
#include <asm/mach-ralink/rt305x.h>
#include <asm/mach-ralink/rt305x_regs.h>

#include "devices.h"

#define TENDA_3G300M_GPIO_BUTTON_RESET		0
#define TENDA_3G300M_GPIO_BUTTON_MODE		10

#define TENDA_3G300M_GPIO_LED_3GROUTER		11
#define TENDA_3G300M_GPIO_LED_AP		12
#define TENDA_3G300M_GPIO_LED_WISPROUTER	9
#define TENDA_3G300M_GPIO_LED_WIRELESSROUTER	13
#define TENDA_3G300M_GPIO_LED_3G		7
#define TENDA_3G300M_GPIO_LED_WPSRESET		14

#define TENDA_3G300M_KEYS_POLL_INTERVAL	20
#define TENDA_3G300M_KEYS_DEBOUNCE_INTERVAL	(3 * TENDA_3G300M_KEYS_POLL_INTERVAL)

const struct flash_platform_data tenda_3g300m_flash = {
	.type		= "mx25l3205d",
};

struct spi_board_info tenda_3g300m_spi_slave_info[] __initdata = {
	{
		.modalias	= "m25p80",
		.platform_data	= &tenda_3g300m_flash,
		.irq		= -1,
		.max_speed_hz	= 10000000,
		.bus_num	= 0,
		.chip_select	= 0,
	}
};

static struct gpio_led tenda_3g300m_leds_gpio[] __initdata = {
        {
		.name		= "tenda:blue:3grouter",
		.gpio		= TENDA_3G300M_GPIO_LED_3GROUTER,
		.active_low	= 1,
	},{
		.name		= "tenda:blue:ap",
		.gpio		= TENDA_3G300M_GPIO_LED_AP,
		.active_low	= 1,
	},{
		.name		= "tenda:blue:wisprouter",
		.gpio		= TENDA_3G300M_GPIO_LED_WISPROUTER,
		.active_low	= 1,
	},{
		.name		= "tenda:blue:wirelessrouter",
		.gpio		= TENDA_3G300M_GPIO_LED_WIRELESSROUTER,
		.active_low	= 1,
	},{
		.name		= "tenda:blue:3g",
		.gpio		= TENDA_3G300M_GPIO_LED_3G,
		.active_low	= 1,
	},{
		.name		= "tenda:blue:wpsreset",
		.gpio		= TENDA_3G300M_GPIO_LED_WPSRESET,
		.active_low	= 1,
	}
};

static struct gpio_keys_button tenda_3g300m_gpio_buttons[] __initdata = {
	{
		.desc		= "reset",
		.type		= EV_KEY,
		.code		= KEY_RESTART,
		.debounce_interval = TENDA_3G300M_KEYS_DEBOUNCE_INTERVAL,
		.gpio		= TENDA_3G300M_GPIO_BUTTON_RESET,
		.active_low	= 1,
	},{
		.desc		= "mode",
		.type		= EV_KEY,
		.code		= BTN_0,
		.debounce_interval = TENDA_3G300M_KEYS_DEBOUNCE_INTERVAL,
		.gpio		= TENDA_3G300M_GPIO_BUTTON_MODE,
		.active_low	= 1,
	}
};

static void __init tenda_3g300m_init(void)
{
	rt305x_gpio_init((RT305X_GPIO_MODE_GPIO <<
			 RT305X_GPIO_MODE_UART0_SHIFT) |
			 RT305X_GPIO_MODE_JTAG);

	rt305x_register_spi(tenda_3g300m_spi_slave_info,
			ARRAY_SIZE(tenda_3g300m_spi_slave_info));

	ramips_register_gpio_leds(-1, ARRAY_SIZE(tenda_3g300m_leds_gpio),
			tenda_3g300m_leds_gpio);

	ramips_register_gpio_buttons(-1, TENDA_3G300M_KEYS_POLL_INTERVAL,
			ARRAY_SIZE(tenda_3g300m_gpio_buttons),
			tenda_3g300m_gpio_buttons);

	rt305x_esw_data.vlan_config = RT305X_ESW_VLAN_CONFIG_LLLLW;
	rt305x_register_ethernet();

	rt305x_register_wifi();

	rt305x_register_wdt();

	rt305x_register_usb();
}

MIPS_MACHINE(RAMIPS_MACH_3G300M, "3G300M", "Tenda 3G300M",
		tenda_3g300m_init);

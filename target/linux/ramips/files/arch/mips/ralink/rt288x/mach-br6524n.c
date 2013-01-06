/*
 *  Edimax BR6524N board support
 *
 *  Copyright (C) 2012 Florian Fainelli <florian@openwrt.org>
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 */

#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/ethtool.h>

#include <asm/mach-ralink/machine.h>
#include <asm/mach-ralink/dev-gpio-buttons.h>
#include <asm/mach-ralink/dev-gpio-leds.h>
#include <asm/mach-ralink/rt288x.h>
#include <asm/mach-ralink/rt288x_regs.h>
#include <asm/mach-ralink/ramips_eth_platform.h>

#include "devices.h"

#define BR6524N_GPIO_STATUS_LED	12
#define BR6524N_GPIO_BUTTON_WPS	0

#define BR6524N_KEYS_POLL_INTERVAL	20
#define BR6524N_KEYS_DEBOUNCE_INTERVAL	(3 * BR6524N_KEYS_POLL_INTERVAL)

static struct gpio_led br6524n_leds_gpio[] __initdata = {
	{
		.name		= "br6524n:green:status",
		.gpio		= BR6524N_GPIO_STATUS_LED,
		.active_low	= 1,
	}
};

static struct gpio_keys_button br6524n_gpio_buttons[] __initdata = {
	{
		.desc		= "wps",
		.type		= EV_KEY,
		.code		= KEY_WPS_BUTTON,
		.debounce_interval = BR6524N_KEYS_DEBOUNCE_INTERVAL,
		.gpio		= BR6524N_GPIO_BUTTON_WPS,
	}
};

static void __init br6524n_fe_init(void)
{
	rt288x_gpio_init(RT2880_GPIO_MODE_UART0);

	rt288x_register_flash(0);

	ramips_register_gpio_leds(-1, ARRAY_SIZE(br6524n_leds_gpio),
				  br6524n_leds_gpio);

	ramips_register_gpio_buttons(-1, BR6524N_KEYS_POLL_INTERVAL,
				     ARRAY_SIZE(br6524n_gpio_buttons),
				     br6524n_gpio_buttons);

	rt288x_register_wifi();

	/* Board is connected to an IC+ IP175C Fast Ethernet switch */
	rt288x_eth_data.speed = SPEED_100;
	rt288x_eth_data.duplex = DUPLEX_FULL;
	rt288x_eth_data.tx_fc = 1;
	rt288x_eth_data.rx_fc = 1;
	rt288x_eth_data.phy_mask = BIT(0);
	rt288x_register_ethernet();

	rt288x_register_wdt();
}

MIPS_MACHINE(RAMIPS_MACH_BR6524N, "BR6524N", "Edimax BR6524N", br6524n_fe_init);

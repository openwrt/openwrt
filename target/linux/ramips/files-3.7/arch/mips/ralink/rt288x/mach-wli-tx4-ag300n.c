/*
 *  Buffalo WLI-TX4-AG300N board support
 *
 *  Copyright (C) 2011 Gabor Juhos <juhosg@openwrt.org>
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 */

#include <linux/init.h>
#include <linux/ethtool.h>
#include <linux/platform_device.h>

#include <asm/mach-ralink/machine.h>
#include <asm/mach-ralink/dev-gpio-buttons.h>
#include <asm/mach-ralink/dev-gpio-leds.h>
#include <asm/mach-ralink/rt288x.h>
#include <asm/mach-ralink/rt288x_regs.h>
#include <asm/mach-ralink/ramips_eth_platform.h>

#include "devices.h"

#define WLI_TX4_AG300N_GPIO_LED_DIAG		10
#define WLI_TX4_AG300N_GPIO_LED_POWER		12
#define WLI_TX4_AG300N_GPIO_LED_SECURITY	13

#define WLI_TX4_AG300N_GPIO_BUTTON_AOSS		0
#define WLI_TX4_AG300N_GPIO_BUTTON_BW_SWITCH	8
#define WLI_TX4_AG300N_GPIO_BUTTON_RESET	9

#define WLI_TX4_AG300N_KEYS_POLL_INTERVAL	20
#define WLI_TX4_AG300N_KEYS_DEBOUNCE_INTERVAL	(3 * WLI_TX4_AG300N_KEYS_POLL_INTERVAL)

static struct gpio_led wli_tx4_ag300n_leds_gpio[] __initdata = {
	{
		.name		= "buffalo:blue:power",
		.gpio		= WLI_TX4_AG300N_GPIO_LED_POWER,
		.active_low	= 1,
	},
	{
		.name		= "buffalo:red:diag",
		.gpio		= WLI_TX4_AG300N_GPIO_LED_DIAG,
		.active_low	= 1,
	},
	{
		.name		= "buffalo:blue:security",
		.gpio		= WLI_TX4_AG300N_GPIO_LED_SECURITY,
		.active_low	= 0,
	},
};

static struct gpio_keys_button wli_tx4_ag300n_gpio_buttons[] __initdata = {
	{
		.desc		= "Reset button",
		.type		= EV_KEY,
		.code		= KEY_RESTART,
		.debounce_interval = WLI_TX4_AG300N_KEYS_DEBOUNCE_INTERVAL,
		.gpio		= WLI_TX4_AG300N_GPIO_BUTTON_RESET,
		.active_low	= 1,
	},
	{
		.desc		= "AOSS button",
		.type		= EV_KEY,
		.code		= KEY_WPS_BUTTON,
		.debounce_interval = WLI_TX4_AG300N_KEYS_DEBOUNCE_INTERVAL,
		.gpio		= WLI_TX4_AG300N_GPIO_BUTTON_AOSS,
		.active_low	= 1,
	},
	{
		.desc		= "Bandwidth switch",
		.type		= EV_KEY,
		.code		= BTN_0,
		.debounce_interval = WLI_TX4_AG300N_KEYS_DEBOUNCE_INTERVAL,
		.gpio		= WLI_TX4_AG300N_GPIO_BUTTON_BW_SWITCH,
		.active_low	= 0,
	},
};

static void __init wli_tx4_ag300n_init(void)
{
	rt288x_gpio_init(RT2880_GPIO_MODE_UART0);

	ramips_register_gpio_leds(-1, ARRAY_SIZE(wli_tx4_ag300n_leds_gpio),
				  wli_tx4_ag300n_leds_gpio);
	ramips_register_gpio_buttons(-1, WLI_TX4_AG300N_KEYS_POLL_INTERVAL,
				     ARRAY_SIZE(wli_tx4_ag300n_gpio_buttons),
				     wli_tx4_ag300n_gpio_buttons);

	rt288x_register_flash(0);
	rt288x_register_wifi();
	rt288x_register_wdt();

	rt288x_eth_data.speed = SPEED_100;
	rt288x_eth_data.duplex = DUPLEX_FULL;
	rt288x_eth_data.tx_fc = 1;
	rt288x_eth_data.rx_fc = 1;
	rt288x_register_ethernet();
}

MIPS_MACHINE(RAMIPS_MACH_WLI_TX4_AG300N, "WLI-TX4-AG300N",
	     "Buffalo WLI-TX4-AG300N", wli_tx4_ag300n_init);

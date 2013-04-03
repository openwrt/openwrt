/*
 *  Senao / EnGenius ESR-9753 board support
 *
 *  Copyright (C) 2011 Artur Wronowski <arteqw@gmail.com>
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 */

#include <linux/init.h>
#include <linux/platform_device.h>

#include <asm/mach-ralink/machine.h>
#include <asm/mach-ralink/dev-gpio-buttons.h>
#include <asm/mach-ralink/dev-gpio-leds.h>
#include <asm/mach-ralink/rt305x.h>
#include <asm/mach-ralink/rt305x_regs.h>

#include "devices.h"

#define ESR_9753_GPIO_LED_POWER		8
#define ESR_9753_GPIO_LED_WPS		14

#define ESR_9753_GPIO_BUTTON_WPS	0	/* active low */
#define ESR_9753_GPIO_BUTTON_RESET	10	/* active low */

#define ESR_9753_KEYS_POLL_INTERVAL	20
#define ESR_9753_KEYS_DEBOUNCE_INTERVAL	(3 * ESR_9753_KEYS_POLL_INTERVAL)

static struct gpio_led esr_9753_leds_gpio[] __initdata = {
	{
		.name		= "esr-9753:orange:power",
		.gpio		= ESR_9753_GPIO_LED_POWER,
		.active_low	= 1,
	}, {
		.name		= "esr-9753:orange:wps",
		.gpio		= ESR_9753_GPIO_LED_WPS,
		.active_low	= 1,
	}
};

static struct gpio_keys_button esr_9753_gpio_buttons[] __initdata = {
	{
		.desc		= "reset",
		.type		= EV_KEY,
		.code		= KEY_RESTART,
		.debounce_interval = ESR_9753_KEYS_DEBOUNCE_INTERVAL,
		.gpio		= ESR_9753_GPIO_BUTTON_RESET,
		.active_low	= 1,
	}, {
		.desc		= "wps",
		.type		= EV_KEY,
		.code		= KEY_WPS_BUTTON,
		.debounce_interval = ESR_9753_KEYS_DEBOUNCE_INTERVAL,
		.gpio		= ESR_9753_GPIO_BUTTON_WPS,
		.active_low	= 1,
	}
};

static void __init esr_9753_init(void)
{
	rt305x_gpio_init(RT305X_GPIO_MODE_GPIO << RT305X_GPIO_MODE_UART0_SHIFT);

	rt305x_register_flash(0);

	ramips_register_gpio_leds(-1, ARRAY_SIZE(esr_9753_leds_gpio),
				  esr_9753_leds_gpio);

	ramips_register_gpio_buttons(-1, ESR_9753_KEYS_POLL_INTERVAL,
				     ARRAY_SIZE(esr_9753_gpio_buttons),
				     esr_9753_gpio_buttons);

	rt305x_esw_data.vlan_config = RT305X_ESW_VLAN_CONFIG_WLLLL;
	rt305x_register_ethernet();
	rt305x_register_wifi();
	rt305x_register_wdt();
}

MIPS_MACHINE(RAMIPS_MACH_ESR_9753, "ESR-9753", "Senao / EnGenius ESR-9753",
	     esr_9753_init);

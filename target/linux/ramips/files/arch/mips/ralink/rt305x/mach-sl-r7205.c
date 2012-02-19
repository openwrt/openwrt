/*
 *  Skyline SL-R7205 Wireless 3G Router support
 *
 *  Copyright (C) 2012 Haipoh Teoh <hpteoh@ceedtec.com>
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

#define SL_R7205_GPIO_BUTTON_WPS	0
#define SL_R7205_GPIO_BUTTON_RESET	10	/* active low */

#define SL_R7205_GPIO_LED_WIFI		7

#define SL_R7205_KEYS_POLL_INTERVAL	20
#define SL_R7205_KEYS_DEBOUNCE_INTERVAL	(3 * SL_R7205_KEYS_POLL_INTERVAL)

static struct gpio_led sl_r7205_leds_gpio[] __initdata = {
	{
		.name		= "sl-r7205:green:wifi",
		.gpio		= SL_R7205_GPIO_LED_WIFI,
		.active_low	= 1,
	}
};

static struct gpio_keys_button sl_r7205_gpio_buttons[] __initdata = {
	{
		.desc		= "reset",
		.type		= EV_KEY,
		.code		= KEY_RESTART,
		.debounce_interval = SL_R7205_KEYS_DEBOUNCE_INTERVAL,
		.gpio		= SL_R7205_GPIO_BUTTON_RESET,
		.active_low	= 1,
	}, {
		.desc		= "wps",
		.type		= EV_KEY,
		.code		= KEY_WPS_BUTTON,
		.debounce_interval = SL_R7205_KEYS_DEBOUNCE_INTERVAL,
		.gpio		= SL_R7205_GPIO_BUTTON_WPS,
		.active_low	= 1,
	}
};

static void __init sl_r7205_init(void)
{
	rt305x_gpio_init(RT305X_GPIO_MODE_GPIO << RT305X_GPIO_MODE_UART0_SHIFT);

	rt305x_register_flash(0);

	rt305x_esw_data.vlan_config = RT305X_ESW_VLAN_CONFIG_WLLLL;
	rt305x_register_ethernet();
	ramips_register_gpio_leds(-1, ARRAY_SIZE(sl_r7205_leds_gpio),
				  sl_r7205_leds_gpio);
	ramips_register_gpio_buttons(-1, SL_R7205_KEYS_POLL_INTERVAL,
				     ARRAY_SIZE(sl_r7205_gpio_buttons),
				     sl_r7205_gpio_buttons);
	rt305x_register_wifi();
	rt305x_register_wdt();
	rt305x_register_usb();
}

MIPS_MACHINE(RAMIPS_MACH_SL_R7205, "SL-R7205",
	     "Skyline SL-R7205 Wireless 3G Router",
	     sl_r7205_init);

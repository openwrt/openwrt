/*
 *  Planex MZK-W300NH2 board support
 *
 *  Copyright (C) 2012 Samir Ibradžić <sibradzic@gmail.com>
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

#define MZKW300NH2_GPIO_BUTTON_RESET     0	/* active low */
#define MZKW300NH2_GPIO_BUTTON_WPS      12	/* active low */
#define MZKW300NH2_GPIO_SWITCH_MODE_RT	13	/* active low */

#define MZKW300NH2_GPIO_LED_POWER        9
#define MZKW300NH2_GPIO_LED_WLAN        14
#define MZKW300NH2_GPIO_LED_WPS         11

#define MZKW300NH2_KEYS_POLL_INTERVAL   20
#define MZKW300NH2_KEYS_DEBOUNCE_INTERVAL (3 * MZKW300NH2_KEYS_POLL_INTERVAL)

static struct gpio_led mzkw300nh2_leds_gpio[] __initdata = {
	{
		.name		= "mzkw300nh2:green:power",
		.gpio		= MZKW300NH2_GPIO_LED_POWER,
		.active_low	= 1,
	}, {
		.name		= "mzkw300nh2:amber:wlan",
		.gpio		= MZKW300NH2_GPIO_LED_WLAN,
		.active_low	= 1,
	}, {
		.name		= "mzkw300nh2:amber:wps",
		.gpio		= MZKW300NH2_GPIO_LED_WPS,
		.active_low	= 1,
	}
};

static struct gpio_keys_button mzkw300nh2_gpio_buttons[] __initdata = {
	{
		.desc		= "wps",
		.type		= EV_KEY,
		.code		= KEY_WPS_BUTTON,
		.debounce_interval = MZKW300NH2_KEYS_DEBOUNCE_INTERVAL,
		.gpio		= MZKW300NH2_GPIO_BUTTON_WPS,
		.active_low	= 1,
	}, {
		.desc		= "reset",
		.type		= EV_KEY,
		.code		= KEY_RESTART,
		.debounce_interval = MZKW300NH2_KEYS_DEBOUNCE_INTERVAL,
		.gpio		= MZKW300NH2_GPIO_BUTTON_RESET,
		.active_low	= 1,
	}, {
		.desc		= "rt",
		.type		= EV_KEY,
		.code		= BTN_0,
		.debounce_interval = MZKW300NH2_KEYS_DEBOUNCE_INTERVAL,
		.gpio		= MZKW300NH2_GPIO_SWITCH_MODE_RT,
		.active_low	= 1,
	}
};

static void __init mzkw300nh2_init(void)
{
	rt305x_gpio_init(RT305X_GPIO_MODE_UART0(RT305X_GPIO_MODE_GPIO) |
			 RT305X_GPIO_MODE_I2C |
			 RT305X_GPIO_MODE_SPI |
			 RT305X_GPIO_MODE_JTAG);

	rt305x_register_flash(0);
	rt305x_esw_data.vlan_config = RT305X_ESW_VLAN_CONFIG_LLLLW;
	rt305x_register_ethernet();
	ramips_register_gpio_leds(-1, ARRAY_SIZE(mzkw300nh2_leds_gpio),
				  mzkw300nh2_leds_gpio);
	ramips_register_gpio_buttons(-1, MZKW300NH2_KEYS_POLL_INTERVAL,
				     ARRAY_SIZE(mzkw300nh2_gpio_buttons),
				     mzkw300nh2_gpio_buttons);
	rt305x_register_wifi();
	rt305x_register_wdt();
}

MIPS_MACHINE(RAMIPS_MACH_MZKW300NH2, "MZK-W300NH2", "Planex MZK-W300NH2",
	      mzkw300nh2_init);

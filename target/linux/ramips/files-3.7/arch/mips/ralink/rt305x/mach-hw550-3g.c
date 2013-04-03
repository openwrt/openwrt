/*
 *  Aztech HW550-3G board support
 *
 *  Copyright (C) 2011 Layne Edwards <ledwards76@gmail.com>
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

#define HW550_3G_GPIO_LED_USB		8
#define HW550_3G_GPIO_LED_3G		11
#define HW550_3G_GPIO_LED_STATUS	12
#define HW550_3G_GPIO_LED_WPS		14

#define HW550_3G_GPIO_BUTTON_RESET	10
#define HW550_3G_GPIO_BUTTON_CONNECT	7
#define HW550_3G_GPIO_BUTTON_WPS	0

#define HW550_3G_KEYS_POLL_INTERVAL	20
#define HW550_3G_KEYS_DEBOUNCE_INTERVAL	(3 * HW550_3G_KEYS_POLL_INTERVAL)

static struct gpio_led hw550_3g_leds_gpio[] __initdata = {
	{
		.name		= "hw550-3g:green:usb",
		.gpio		= HW550_3G_GPIO_LED_USB,
		.active_low	= 1,
	}, {
		.name		= "hw550-3g:green:3g",
		.gpio		= HW550_3G_GPIO_LED_3G,
		.active_low	= 1,
	}, {
		.name		= "hw550-3g:green:status",
		.gpio		= HW550_3G_GPIO_LED_STATUS,
		.active_low	= 1,
	}, {
		.name		= "hw550-3g:green:wps",
		.gpio		= HW550_3G_GPIO_LED_WPS,
		.active_low	= 1,
	}
};

static struct gpio_keys_button hw550_3g_gpio_buttons[] __initdata = {
	{
		.desc		= "reset",
		.type		= EV_KEY,
		.code		= KEY_RESTART,
		.debounce_interval = HW550_3G_KEYS_DEBOUNCE_INTERVAL,
		.gpio		= HW550_3G_GPIO_BUTTON_RESET,
		.active_low	= 1,
	}, {
		.desc		= "connect",
		.type		= EV_KEY,
		.code		= KEY_CONNECT,
		.debounce_interval = HW550_3G_KEYS_DEBOUNCE_INTERVAL,
		.gpio		= HW550_3G_GPIO_BUTTON_CONNECT,
		.active_low	= 1,
	}, {
		.desc		= "wps",
		.type		= EV_KEY,
		.code		= KEY_WPS_BUTTON,
		.debounce_interval = HW550_3G_KEYS_DEBOUNCE_INTERVAL,
		.gpio		= HW550_3G_GPIO_BUTTON_WPS,
		.active_low	= 1,
	}
};

#define HW550_3G_GPIO_MODE \
	((RT305X_GPIO_MODE_GPIO << RT305X_GPIO_MODE_UART0_SHIFT) | \
	 RT305X_GPIO_MODE_MDIO)

static void __init hw550_3g_init(void)
{
	rt305x_gpio_init(HW550_3G_GPIO_MODE);

	rt305x_register_flash(0);

       rt305x_esw_data.vlan_config = RT305X_ESW_VLAN_CONFIG_WLLLL;
	rt305x_register_ethernet();
	ramips_register_gpio_leds(-1, ARRAY_SIZE(hw550_3g_leds_gpio),
				  hw550_3g_leds_gpio);
	ramips_register_gpio_buttons(-1, HW550_3G_KEYS_POLL_INTERVAL,
				     ARRAY_SIZE(hw550_3g_gpio_buttons),
				     hw550_3g_gpio_buttons);
	rt305x_register_wifi();
	rt305x_register_wdt();
	rt305x_register_usb();
}

MIPS_MACHINE(RAMIPS_MACH_HW550_3G, "HW550-3G", "Aztech HW550-3G",
	     hw550_3g_init);

MIPS_MACHINE(RAMIPS_MACH_ALL0239_3G, "ALL0239-3G", "Allnet ALL0239-3G",
	     hw550_3g_init);

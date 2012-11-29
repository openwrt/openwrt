/*
 *  ASUS RT-N13U board support
 *
 *  Copyright (C) 2012 lintel<lintel.huang@gmail.com>
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

#define RT_N13U_GPIO_BUTTON_RESET	10
#define RT_N13U_GPIO_BUTTON_WPS		0

#define RT_N13U_GPIO_LED_POWER		7
#define RT_N13U_GPIO_LED_WIFI		8


#define RT_N13U_BUTTONS_POLL_INTERVAL	10
#define RT_N13U_BUTTONS_DEBOUNCE_INTERVAL  (3 * RT_N13U_BUTTONS_POLL_INTERVAL)

static struct gpio_led rt_n13u_leds_gpio[] __initdata = {
	{
		.name		= "rt-n13u:power",
		.gpio		= RT_N13U_GPIO_LED_POWER,
		.active_low	= 1,
	}, {
		.name		= "rt-n13u:wifi",
		.gpio		= RT_N13U_GPIO_LED_WIFI,
		.active_low	= 1,
	}
};

static struct gpio_keys_button rt_n13u_gpio_buttons[] __initdata = {
	{
		.desc		= "reset",
		.type		= EV_KEY,
		.code		= KEY_RESTART,
		.debounce_interval	= RT_N13U_BUTTONS_DEBOUNCE_INTERVAL,
		.gpio		= RT_N13U_GPIO_BUTTON_RESET,
		.active_low	= 1,
	}, {
		.desc		= "wps",
		.type		= EV_KEY,
		.code		= KEY_WPS_BUTTON,
		.debounce_interval	= RT_N13U_BUTTONS_DEBOUNCE_INTERVAL,
		.gpio		= RT_N13U_GPIO_BUTTON_WPS,
		.active_low	= 1,
	},
};

static void __init rt_n13u_init(void)
{
	rt305x_gpio_init(RT305X_GPIO_MODE_GPIO << RT305X_GPIO_MODE_UART0_SHIFT);

	rt305x_register_flash(0);

	ramips_register_gpio_leds(-1, ARRAY_SIZE(rt_n13u_leds_gpio),
				  rt_n13u_leds_gpio);

	ramips_register_gpio_buttons(-1, RT_N13U_BUTTONS_POLL_INTERVAL,
				     ARRAY_SIZE(rt_n13u_gpio_buttons),
				     rt_n13u_gpio_buttons);

	rt305x_esw_data.vlan_config = RT305X_ESW_VLAN_CONFIG_LLLLW;
	rt305x_register_ethernet();
	rt305x_register_wifi();
	rt305x_register_wdt();
	rt305x_register_usb();
}

MIPS_MACHINE(RAMIPS_MACH_RT_N13U, "RT-N13U", "Asus RT-N13U",
	     rt_n13u_init);

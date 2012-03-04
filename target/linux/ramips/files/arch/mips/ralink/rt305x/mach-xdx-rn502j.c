/*
 *  Unknown router name/model, PCB marked with XDX-RN502J
 *
 *  Copyright (C) 2011 Bruno Schwander bruno@tinkerbox.org
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

#define XDXRN502J_GPIO_BUTTON_RESET	12
#define XDXRN502J_GPIO_LED_WIFI		7
#define XDXRN502J_GPIO_LED_POWER	9

#define XDXRN502J_BUTTONS_POLL_INTERVAL	20
#define XDXRN502J_BUTTONS_DEBOUNCE_INTERVAL (3 * XDXRN502J_BUTTONS_POLL_INTERVAL)


static struct gpio_led xdxrn502j_leds_gpio[] __initdata = {
	{
		.name		= "xdxrn502j:green:wifi",
		.gpio		= XDXRN502J_GPIO_LED_WIFI,
		.active_low	= 1,
	}, {
		.name		= "xdxrn502j:green:power",
		.gpio		= XDXRN502J_GPIO_LED_POWER,
		.active_low	= 1,
	}
};

static struct gpio_keys_button xdxrn502j_gpio_buttons[] __initdata = {
	{
		.desc		= "reset",
		.type		= EV_KEY,
		.code		= KEY_RESTART,
		.debounce_interval = XDXRN502J_BUTTONS_DEBOUNCE_INTERVAL,
		.gpio		= XDXRN502J_GPIO_BUTTON_RESET,
		.active_low	= 1,
	}
};

#define XDXRN502J_GPIO_MODE \
	((RT305X_GPIO_MODE_GPIO << RT305X_GPIO_MODE_UART0_SHIFT) | \
	 RT305X_GPIO_MODE_MDIO)

static void __init xdxrn502j_init(void)
{
	rt305x_gpio_init(XDXRN502J_GPIO_MODE);

	rt305x_register_flash(0);

	rt305x_esw_data.vlan_config = RT305X_ESW_VLAN_CONFIG_WLLLL;
	rt305x_register_ethernet();

	ramips_register_gpio_leds(-1, ARRAY_SIZE(xdxrn502j_leds_gpio),
				  xdxrn502j_leds_gpio);

	ramips_register_gpio_buttons(-1, XDXRN502J_BUTTONS_POLL_INTERVAL,
				     ARRAY_SIZE(xdxrn502j_gpio_buttons),
				     xdxrn502j_gpio_buttons);

	rt305x_register_wifi();
	rt305x_register_wdt();
	rt305x_register_usb();
}

MIPS_MACHINE(RAMIPS_MACH_XDXRN502J, "XDXRN502J", "XDX RN502J",
	     xdxrn502j_init);

/*
 *  AWB WR6202 board support
 *
 *  Copyright (C) 2012 Johnathan Boyce<jon.boyce@globalreach.eu.com>
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 */

#include<linux/init.h>
#include<linux/platform_device.h>
#include<linux/gpio.h>

#include<asm/mach-ralink/machine.h>
#include<asm/mach-ralink/dev-gpio-buttons.h>
#include<asm/mach-ralink/dev-gpio-leds.h>
#include<asm/mach-ralink/rt305x.h>
#include<asm/mach-ralink/rt305x_regs.h>

#include "devices.h"

#define WR6202_GPIO_BUTTON_RESET     10        /* active low */
#define WR6202_GPIO_BUTTON_WPS        0        /* active low */

#define WR6202_KEYS_POLL_INTERVAL 20
#define WR6202_KEYS_DEBOUNCE_INTERVAL	(3 * WR6202_KEYS_POLL_INTERVAL)

#define WR6202_GPIO_USB_POWER	     11

#define WR6202_GPIO_LED_3G	     13
#define WR6202_GPIO_LED_WPS	     14

static struct gpio_led wr6202_leds_gpio[] __initdata = {
	{
		.name           = "wr6202:blue:wps",
		.gpio           = WR6202_GPIO_LED_WPS,
		.active_low     = 1,
	}, {
		.name           = "wr6202:blue:3g",
		.gpio           = WR6202_GPIO_LED_3G,
		.active_low     = 1,
	}
};

static struct gpio_keys_button wr6202_gpio_buttons[] __initdata = {
	{
		.desc           = "reset",
		.type           = EV_KEY,
		.code           = KEY_RESTART,
		.debounce_interval = WR6202_KEYS_DEBOUNCE_INTERVAL,
		.gpio           = WR6202_GPIO_BUTTON_RESET,
		.active_low     = 1,
	}, {
		.desc           = "wps",
		.type           = EV_KEY,
		.code           = KEY_WPS_BUTTON,
		.debounce_interval = WR6202_KEYS_DEBOUNCE_INTERVAL,
		.gpio           = WR6202_GPIO_BUTTON_WPS,
		.active_low     = 1,
	}
};

static void __init wr6202_init(void)
{
	rt305x_esw_data.vlan_config = RT305X_ESW_VLAN_CONFIG_WLLLL;

	rt305x_gpio_init(RT305X_GPIO_MODE_GPIO << RT305X_GPIO_MODE_UART0_SHIFT);

	ramips_register_gpio_leds(-1, ARRAY_SIZE(wr6202_leds_gpio),
				wr6202_leds_gpio);
	ramips_register_gpio_buttons(-1, WR6202_KEYS_POLL_INTERVAL,
				ARRAY_SIZE(wr6202_gpio_buttons),
				wr6202_gpio_buttons);

	/* Power to the USB port is controlled by GPIO line */
	gpio_request(WR6202_GPIO_USB_POWER, "WR6202_GPIO_USB_POWER");
	gpio_direction_output(WR6202_GPIO_USB_POWER, 0);
	gpio_free(WR6202_GPIO_USB_POWER);

	rt305x_register_flash(0);

	rt305x_register_ethernet();
	rt305x_register_wifi();
	rt305x_register_wdt();
	rt305x_register_usb();
}

MIPS_MACHINE(RAMIPS_MACH_WR6202, "WR6202", "AWB WR6202",
	     wr6202_init);

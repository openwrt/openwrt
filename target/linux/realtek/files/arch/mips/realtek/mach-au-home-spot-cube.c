/*
 *  AU HOME SPOT CUBE board support
 *
 *  Copyright (C) 2017 Weijie Gao <hackpascal@gmail.com>
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 */

#include <linux/init.h>
#include <linux/platform_device.h>

#include <asm/mach-realtek/realtek.h>
#include <asm/mach-realtek/platform.h>

#include "machtypes.h"
#include "dev-gpio-buttons.h"
#include "dev-leds-gpio.h"
#include "dev-m25p80.h"
#include "dev-eth.h"
#include "gpio.h"

#define AU_GPIO_LED_WPS_GREEN		4
#define AU_GPIO_LED_WPS_RED		16
#define AU_GPIO_LED_STATUS_RED		6
#define AU_GPIO_LED_LAN			10
#define AU_GPIO_LED_LAN_LINK		13
#define AU_GPIO_LED_WAN			14
#define AU_GPIO_LED_WAN_LINK		12

#define AU_GPIO_BTN_WPS			3
#define AU_GPIO_BTN_RESET		5

#define AU_KEYS_POLL_INTERVAL		20	/* msecs */
#define AU_KEYS_DEBOUNCE_INTERVAL	(3 * AU_KEYS_POLL_INTERVAL)

static struct gpio_led au_leds_gpio[] __initdata = {
	{
		.name		= "au:red:wps",
		.gpio		= AU_GPIO_LED_WPS_RED,
		.active_low	= 1,
	}, {
		.name		= "au:green:wps",
		.gpio		= AU_GPIO_LED_WPS_GREEN,
		.active_low	= 1,
	}, {
		.name		= "au:red:status",
		.gpio		= AU_GPIO_LED_STATUS_RED,
		.active_low	= 1,
	}, {
		.name		= "au:green:lan-link",
		.gpio		= AU_GPIO_LED_LAN_LINK,
		.active_low	= 1,
	}, {
		.name		= "au:green:wan-link",
		.gpio		= AU_GPIO_LED_WAN_LINK,
		.active_low	= 1,
	},
};

static struct gpio_keys_button au_gpio_keys[] __initdata = {
	{
		.desc		= "reset",
		.type		= EV_KEY,
		.code		= KEY_RESTART,
		.debounce_interval = AU_KEYS_DEBOUNCE_INTERVAL,
		.gpio		= AU_GPIO_BTN_RESET,
		.active_low	= 1,
	} , {
		.desc		= "wps",
		.type		= EV_KEY,
		.code		= KEY_WPS_BUTTON,
		.debounce_interval = AU_KEYS_DEBOUNCE_INTERVAL,
		.gpio		= AU_GPIO_BTN_WPS,
		.active_low	= 1,
	}
};

static void __init au_home_spot_cube_init(void)
{
	realtek_register_leds_gpio(-1, ARRAY_SIZE(au_leds_gpio),
				 au_leds_gpio);
	realtek_register_gpio_keys_polled(-1, AU_KEYS_POLL_INTERVAL,
					ARRAY_SIZE(au_gpio_keys),
					au_gpio_keys);

	realtek_register_m25p80(NULL);
	realtek_register_eth();

	realtek_set_gpio_control(AU_GPIO_LED_WPS_GREEN, true);
	realtek_set_gpio_control(AU_GPIO_LED_WPS_RED, true);
	realtek_set_gpio_control(AU_GPIO_LED_STATUS_RED, true);
	realtek_set_gpio_control(AU_GPIO_LED_LAN_LINK, true);
	realtek_set_gpio_control(AU_GPIO_LED_WAN_LINK, true);
	realtek_set_gpio_control(AU_GPIO_BTN_WPS, true);
	realtek_set_gpio_control(AU_GPIO_BTN_RESET, true);
	realtek_set_gpio_control(AU_GPIO_LED_LAN, false);
	realtek_set_gpio_control(AU_GPIO_LED_WAN, false);

	realtek_set_gpio_mux(
		RTL8196C_GPIO_MUX_PCIE_RST |
		(RTL8196C_GPIO_MUX_PORT0_MASK << RTL8196C_GPIO_MUX_PORT0_SHIFT) |
		(RTL8196C_GPIO_MUX_PORT4_MASK << RTL8196C_GPIO_MUX_PORT4_SHIFT),
		(RTL8196C_GPIO_MUX_PORT2_MASK << RTL8196C_GPIO_MUX_PORT2_SHIFT) |
		(RTL8196C_GPIO_MUX_PORT3_MASK << RTL8196C_GPIO_MUX_PORT3_SHIFT) |
		(RTL8196C_GPIO_MUX_RESET_MASK << RTL8196C_GPIO_MUX_RESET_SHIFT) |
		(RTL8196C_GPIO_MUX_GPIOC0_MASK << RTL8196C_GPIO_MUX_GPIOC0_SHIFT) |
		(RTL8196C_GPIO_MUX_JTAG_MASK << RTL8196C_GPIO_MUX_JTAG_SHIFT));
}

MIPS_MACHINE(REALTEK_MACH_AU_HOME_SPOT_CUBE, "AU-HOME-SPOT-CUBE", "AU HOME SPOT CUBE",
	     au_home_spot_cube_init);

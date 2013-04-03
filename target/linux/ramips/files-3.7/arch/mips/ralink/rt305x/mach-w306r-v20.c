/*
 *  Tenda W306R V2.0 board support
 *
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
#include <asm/mach-ralink/rt305x.h>
#include <asm/mach-ralink/rt305x_regs.h>
#include <asm/mach-ralink/ramips_eth_platform.h>

#include "devices.h"

#define W306R_V20_GPIO_LED_SYS		9
#define W306R_V20_GPIO_LED_WPS		13

#define W306R_V20_GPIO_BUTTON_RESET	10

#define W306R_V20_KEYS_POLL_INTERVAL	20
#define W306R_V20_KEYS_DEBOUNCE_INTERVAL (3 * W306R_V20_KEYS_POLL_INTERVAL)

static struct gpio_led w306r_v20_leds_gpio[] __initdata = {
	{
		.name		= "w306r-v20:green:sys",
		.gpio		= W306R_V20_GPIO_LED_SYS,
		.active_low	= 1,
	}, {
		.name		= "w306r-v20:green:wps",
		.gpio		= W306R_V20_GPIO_LED_WPS,
		.active_low	= 1,
	}
};

static struct gpio_keys_button w306r_v20_gpio_buttons[] __initdata = {
	{
		.desc		= "RESET/WPS",
		.type		= EV_KEY,
		.code		= KEY_RESTART,
		.debounce_interval = W306R_V20_KEYS_DEBOUNCE_INTERVAL,
		.gpio		= W306R_V20_GPIO_BUTTON_RESET,
		.active_low	= 1,
	}
};

static void __init w306r_v20_init(void)
{
	rt305x_register_flash(0);

	rt305x_gpio_init(RT305X_GPIO_MODE_GPIO << RT305X_GPIO_MODE_UART0_SHIFT);

	rt305x_esw_data.vlan_config = RT305X_ESW_VLAN_CONFIG_WLLLL;
	rt305x_register_ethernet();
	ramips_register_gpio_leds(-1, ARRAY_SIZE(w306r_v20_leds_gpio),
				  w306r_v20_leds_gpio);
	ramips_register_gpio_buttons(-1, W306R_V20_KEYS_POLL_INTERVAL,
				     ARRAY_SIZE(w306r_v20_gpio_buttons),
				     w306r_v20_gpio_buttons);
	rt305x_register_wifi();
	rt305x_register_wdt();
}

MIPS_MACHINE(RAMIPS_MACH_W306R_V20, "W306R_V20", "Tenda W306R V2.0",
	     w306r_v20_init);

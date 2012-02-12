/*
 *  Buffalo WHR-G300N board support
 *
 *  Copyright (C) 2009-2010 Gabor Juhos <juhosg@openwrt.org>
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 */

#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/partitions.h>
#include <linux/mtd/physmap.h>

#include <asm/mach-ralink/machine.h>
#include <asm/mach-ralink/dev-gpio-buttons.h>
#include <asm/mach-ralink/dev-gpio-leds.h>
#include <asm/mach-ralink/rt305x.h>
#include <asm/mach-ralink/rt305x_regs.h>

#include "devices.h"

#define WHR_G300N_GPIO_LED_DIAG		7
#define WHR_G300N_GPIO_LED_ROUTER	9
#define WHR_G300N_GPIO_LED_SECURITY	14

#define WHR_G300N_GPIO_BUTTON_AOSS		0	/* active low */
#define WHR_G300N_GPIO_BUTTON_RESET		10	/* active low */
#define WHR_G300N_GPIO_BUTTON_ROUTER_ON		8	/* active low */
#define WHR_G300N_GPIO_BUTTON_ROUTER_OFF	11	/* active low */

#define WHR_G300N_KEYS_POLL_INTERVAL		20
#define WHR_G300N_KEYS_DEBOUNCE_INTERVAL	(3 * WHR_G300N_KEYS_POLL_INTERVAL)

static struct mtd_partition whr_g300n_partitions[] = {
	{
		.name	= "u-boot",
		.offset	= 0,
		.size	= 0x030000,
		.mask_flags = MTD_WRITEABLE,
	}, {
		.name	= "u-boot-env",
		.offset	= 0x030000,
		.size	= 0x010000,
		.mask_flags = MTD_WRITEABLE,
	}, {
		.name	= "factory",
		.offset	= 0x040000,
		.size	= 0x010000,
		.mask_flags = MTD_WRITEABLE,
	}, {
		.name	= "kernel",
		.offset	= 0x050000,
		.size	= 0x090000,
	}, {
		.name	= "rootfs",
		.offset	= 0x140000,
		.size	= 0x2B0000,
	}, {
		.name	= "user",
		.offset	= 0x3f0000,
		.size	= 0x010000,
	}, {
		.name	= "firmware",
		.offset	= 0x050000,
		.size	= 0x3a0000,
	}
};

static struct gpio_led whr_g300n_leds_gpio[] __initdata = {
	{
		.name		= "whr-g300n:red:diag",
		.gpio		= WHR_G300N_GPIO_LED_DIAG,
		.active_low	= 1,
	}, {
		.name		= "whr-g300n:green:router",
		.gpio		= WHR_G300N_GPIO_LED_ROUTER,
		.active_low	= 1,
	}, {
		.name		= "whr-g300n:amber:security",
		.gpio		= WHR_G300N_GPIO_LED_SECURITY,
		.active_low	= 1,
	}
};

static struct gpio_keys_button whr_g300n_gpio_buttons[] __initdata = {
	{
		.desc		= "reset",
		.type		= EV_KEY,
		.code		= KEY_RESTART,
		.debounce_interval = WHR_G300N_KEYS_DEBOUNCE_INTERVAL,
		.gpio		= WHR_G300N_GPIO_BUTTON_RESET,
		.active_low	= 1,
	}, {
		.desc		= "aoss",
		.type		= EV_KEY,
		.code		= KEY_WPS_BUTTON,
		.debounce_interval = WHR_G300N_KEYS_DEBOUNCE_INTERVAL,
		.gpio		= WHR_G300N_GPIO_BUTTON_AOSS,
		.active_low	= 1,
	}, {
		.desc		= "router-off",
		.type		= EV_KEY,
		.code		= BTN_2,
		.debounce_interval = WHR_G300N_KEYS_DEBOUNCE_INTERVAL,
		.gpio		= WHR_G300N_GPIO_BUTTON_ROUTER_OFF,
		.active_low	= 1,
	}, {
		.desc		= "router-on",
		.type		= EV_KEY,
		.code		= BTN_3,
		.debounce_interval = WHR_G300N_KEYS_DEBOUNCE_INTERVAL,
		.gpio		= WHR_G300N_GPIO_BUTTON_ROUTER_ON,
		.active_low	= 1,
	}
};

static void __init whr_g300n_init(void)
{
	rt305x_gpio_init(RT305X_GPIO_MODE_GPIO << RT305X_GPIO_MODE_UART0_SHIFT);

	rt305x_flash0_data.nr_parts = ARRAY_SIZE(whr_g300n_partitions);
	rt305x_flash0_data.parts = whr_g300n_partitions;
	rt305x_register_flash(0);

	rt305x_esw_data.vlan_config = RT305X_ESW_VLAN_CONFIG_LLLLW;
	rt305x_register_ethernet();
	ramips_register_gpio_leds(-1, ARRAY_SIZE(whr_g300n_leds_gpio),
				  whr_g300n_leds_gpio);
	ramips_register_gpio_buttons(-1, WHR_G300N_KEYS_POLL_INTERVAL,
				     ARRAY_SIZE(whr_g300n_gpio_buttons),
				     whr_g300n_gpio_buttons);
	rt305x_register_wifi();
	rt305x_register_wdt();
}

MIPS_MACHINE(RAMIPS_MACH_WHR_G300N, "WHR-G300N", "Buffalo WHR-G300N",
	     whr_g300n_init);

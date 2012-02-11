/*
 *  ALFA Networks W502U board support
 *
 *  Copyright (C) 2011 Gabor Juhos <juhosg@openwrt.org>
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

#define W502U_GPIO_LED_USB		13
#define W502U_GPIO_LED_WPS		14

#define W502U_GPIO_BUTTON_WPS		0
#define W502U_GPIO_BUTTON_RESET		10

#define W502U_BUTTONS_POLL_INTERVAL	20

static struct mtd_partition w502u_partitions[] = {
	{
		.name	= "u-boot",
		.offset	= 0,
		.size	= 0x030000,
		.mask_flags = MTD_WRITEABLE,
	}, {
		.name	= "config",
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
		.size	= 0x0d0000,
	}, {
		.name	= "rootfs",
		.offset	= 0x120000,
		.size	= 0x6d0000,
	}, {
		.name	= "data3g",
		.offset	= 0x7f0000,
		.size	= 0x010000,
		.mask_flags = MTD_WRITEABLE,
	}, {
		.name	= "firmware",
		.offset	= 0x050000,
		.size	= 0x7a0000,
	}
};

static struct physmap_flash_data w502u_flash_data = {
	.nr_parts	= ARRAY_SIZE(w502u_partitions),
	.parts		= w502u_partitions,
};

static struct gpio_led w502u_leds_gpio[] __initdata = {
	{
		.name		= "alfa:blue:usb",
		.gpio		= W502U_GPIO_LED_USB,
		.active_low	= 1,
	},
	{
		.name		= "alfa:blue:wps",
		.gpio		= W502U_GPIO_LED_WPS,
		.active_low	= 1,
	},
};

static struct gpio_button w502u_gpio_buttons[] __initdata = {
	{
		.desc		= "Reset button",
		.type		= EV_KEY,
		.code		= KEY_RESTART,
		.threshold	= 3,
		.gpio		= W502U_GPIO_BUTTON_RESET,
		.active_low	= 1,
	},
	{
		.desc		= "WPS button",
		.type		= EV_KEY,
		.code		= KEY_WPS_BUTTON,
		.threshold	= 3,
		.gpio		= W502U_GPIO_BUTTON_WPS,
		.active_low	= 1,
	},
};

static void __init w502u_init(void)
{
	rt305x_gpio_init((RT305X_GPIO_MODE_GPIO <<
			  RT305X_GPIO_MODE_UART0_SHIFT));

	rt305x_register_flash(0, &w502u_flash_data);
	rt305x_esw_data.vlan_config = RT305X_ESW_VLAN_CONFIG_WLLLL;
	rt305x_register_ethernet();
	ramips_register_gpio_leds(-1, ARRAY_SIZE(w502u_leds_gpio),
				  w502u_leds_gpio);
	ramips_register_gpio_buttons(-1, W502U_BUTTONS_POLL_INTERVAL,
				     ARRAY_SIZE(w502u_gpio_buttons),
				     w502u_gpio_buttons);
	rt305x_register_wifi();
	rt305x_register_wdt();
	rt305x_register_usb();
}

MIPS_MACHINE(RAMIPS_MACH_W502U, "W502U", "ALFA Networks W502U",
	      w502u_init);

/*
 *  La Fonera20N board support
 *
 *  Copyright (C) 2009 John Crispin <blogic@openwrt.org>
 *  Copyright (C) 2010 Gabor Juhos <juhosg@openwrt.org>
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

#define FONERA20N_GPIO_BUTTON_RESET	12
#define FONERA20N_GPIO_SWITCH		13
#define FONERA20N_GPIO_LED_WIFI		7
#define FONERA20N_GPIO_LED_POWER	9
#define FONERA20N_GPIO_LED_USB		14

#define FONERA20N_KEYS_POLL_INTERVAL	20
#define FONERA20N_KEYS_DEBOUNCE_INTERVAL (3 * FONERA20N_KEYS_POLL_INTERVAL)

static struct mtd_partition fonera20n_partitions[] = {
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
		.size	= 0x0a0000,
	}, {
		.name	= "rootfs",
		.offset	= 0x150000,
		.size	= 0x6b0000,
	}, {
		.name	= "firmware",
		.offset	= 0x050000,
		.size	= 0x7b0000,
	}
};

static struct gpio_led fonera20n_leds_gpio[] __initdata = {
	{
		.name		= "fonera20n:orange:wifi",
		.gpio		= FONERA20N_GPIO_LED_WIFI,
		.active_low	= 1,
	}, {
		.name		= "fonera20n:green:power",
		.gpio		= FONERA20N_GPIO_LED_POWER,
		.active_low	= 1,
	}, {
		.name		= "fonera20n:orange:usb",
		.gpio		= FONERA20N_GPIO_LED_USB,
		.active_low	= 1,
	}
};

static struct gpio_keys_button fonera20n_gpio_buttons[] __initdata = {
	{
		.desc		= "reset",
		.type		= EV_KEY,
		.code		= KEY_RESTART,
		.debounce_interval = FONERA20N_KEYS_DEBOUNCE_INTERVAL,
		.gpio		= FONERA20N_GPIO_BUTTON_RESET,
		.active_low	= 1,
	}, {
		.desc		= "switch",
		.type		= EV_KEY,
		.code		= BTN_1,
		.debounce_interval = FONERA20N_KEYS_DEBOUNCE_INTERVAL,
		.gpio		= FONERA20N_GPIO_SWITCH,
		.active_low	= 1,
	}
};

static void __init fonera20n_init(void)
{
	rt305x_gpio_init(RT305X_GPIO_MODE_GPIO << RT305X_GPIO_MODE_UART0_SHIFT);

	rt305x_flash0_data.nr_parts = ARRAY_SIZE(fonera20n_partitions);
	rt305x_flash0_data.parts = fonera20n_partitions;
	rt305x_register_flash(0);

	ramips_register_gpio_leds(-1, ARRAY_SIZE(fonera20n_leds_gpio),
				  fonera20n_leds_gpio);

	ramips_register_gpio_buttons(-1, FONERA20N_KEYS_POLL_INTERVAL,
				     ARRAY_SIZE(fonera20n_gpio_buttons),
				     fonera20n_gpio_buttons);

	rt305x_esw_data.vlan_config = RT305X_ESW_VLAN_CONFIG_LLLLW;
	rt305x_register_ethernet();
	rt305x_register_wifi();
	rt305x_register_wdt();
	rt305x_register_usb();
}

MIPS_MACHINE(RAMIPS_MACH_FONERA20N, "FONERA20N", "La Fonera 2.0N",
	     fonera20n_init);

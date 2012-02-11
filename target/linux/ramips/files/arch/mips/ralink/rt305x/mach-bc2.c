/*
 *  NexAira BC2 board support
 *
 *  Copyright (C) 2011 Adam J. Porter <porter.adam@gmail.com>
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

#define BC2_GPIO_BUTTON_RESET		17
#define BC2_GPIO_LED_USB		20

#define BC2_BUTTONS_POLL_INTERVAL	20

static struct mtd_partition bc2_partitions[] = {
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

static struct physmap_flash_data bc2_flash_data = {
	.nr_parts	= ARRAY_SIZE(bc2_partitions),
	.parts		= bc2_partitions,
};

static struct gpio_led bc2_leds_gpio[] __initdata = {
	{
		.name		= "bc2:blue:usb",
		.gpio		= BC2_GPIO_LED_USB,
		.active_low	= 1,
	}
};

static struct gpio_button bc2_gpio_buttons[] __initdata = {
	{
		.desc		= "reset",
		.type		= EV_KEY,
		.code		= KEY_RESTART,
		.threshold	= 3,
		.gpio		= BC2_GPIO_BUTTON_RESET,
		.active_low	= 1,
	}
};

static void __init bc2_init(void)
{
	rt305x_gpio_init((RT305X_GPIO_MODE_GPIO <<
			  RT305X_GPIO_MODE_UART0_SHIFT) |
			  RT305X_GPIO_MODE_JTAG);

	rt305x_register_flash(0, &bc2_flash_data);

	ramips_register_gpio_leds(-1, ARRAY_SIZE(bc2_leds_gpio),
				  bc2_leds_gpio);

	ramips_register_gpio_buttons(-1, BC2_BUTTONS_POLL_INTERVAL,
				     ARRAY_SIZE(bc2_gpio_buttons),
				     bc2_gpio_buttons);

	rt305x_esw_data.vlan_config = RT305X_ESW_VLAN_CONFIG_LLLLW;
	rt305x_register_ethernet();
	rt305x_register_wifi();
	rt305x_register_wdt();
	rt305x_register_usb();
}

MIPS_MACHINE(RAMIPS_MACH_BC2, "BC2", "NexAira BC2",
	     bc2_init);

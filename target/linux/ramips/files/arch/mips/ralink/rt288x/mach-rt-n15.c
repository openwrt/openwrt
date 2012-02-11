/*
 *  Asus RT-N15 board support
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
#include <linux/rtl8366.h>
#include <linux/ethtool.h>

#include <asm/mach-ralink/machine.h>
#include <asm/mach-ralink/dev-gpio-buttons.h>
#include <asm/mach-ralink/dev-gpio-leds.h>
#include <asm/mach-ralink/rt288x.h>
#include <asm/mach-ralink/rt288x_regs.h>
#include <asm/mach-ralink/ramips_eth_platform.h>

#include "devices.h"

#define RT_N15_GPIO_LED_POWER		11
#define RT_N15_GPIO_BUTTON_WPS		0
#define RT_N15_GPIO_BUTTON_RESET	12

#define RT_N15_GPIO_RTL8366_SCK		2
#define RT_N15_GPIO_RTL8366_SDA		1

#define RT_N15_KEYS_POLL_INTERVAL	20
#define RT_N15_KEYS_DEBOUNCE_INTERVAL	(3 * RT_N15_KEYS_POLL_INTERVAL)

static struct mtd_partition rt_n15_partitions[] = {
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
		.size   = 0x0d0000,
	}, {
		.name	= "rootfs",
		.offset = 0x120000,
		.size   = 0x2e0000,
	}, {
		.name	= "firmware",
		.offset	= 0x050000,
		.size	= 0x3b0000,
	}
};

static struct physmap_flash_data rt_n15_flash_data = {
	.nr_parts	= ARRAY_SIZE(rt_n15_partitions),
	.parts		= rt_n15_partitions,
};

static struct gpio_led rt_n15_leds_gpio[] __initdata = {
	{
		.name		= "rt-n15:blue:power",
		.gpio		= RT_N15_GPIO_LED_POWER,
		.active_low	= 1,
	}
};

static struct gpio_keys_button rt_n15_gpio_buttons[] __initdata = {
	{
		.desc		= "reset",
		.type		= EV_KEY,
		.code		= KEY_RESTART,
		.debounce_interval = RT_N15_KEYS_DEBOUNCE_INTERVAL,
		.gpio		= RT_N15_GPIO_BUTTON_RESET,
		.active_low	= 1,
	}, {
		.desc		= "wps",
		.type		= EV_KEY,
		.code		= KEY_WPS_BUTTON,
		.debounce_interval = RT_N15_KEYS_DEBOUNCE_INTERVAL,
		.gpio		= RT_N15_GPIO_BUTTON_WPS,
		.active_low	= 1,
	}
};

static struct rtl8366_platform_data rt_n15_rtl8366s_data = {
	.gpio_sda        = RT_N15_GPIO_RTL8366_SDA,
	.gpio_sck        = RT_N15_GPIO_RTL8366_SCK,
};

static struct platform_device rt_n15_rtl8366s_device = {
	.name		= RTL8366S_DRIVER_NAME,
	.id		= -1,
	.dev = {
		.platform_data	= &rt_n15_rtl8366s_data,
	}
};

static void __init rt_n15_init(void)
{
	rt288x_gpio_init(RT2880_GPIO_MODE_UART0 | RT2880_GPIO_MODE_I2C);

	rt288x_register_flash(0, &rt_n15_flash_data);

	ramips_register_gpio_leds(-1, ARRAY_SIZE(rt_n15_leds_gpio),
				  rt_n15_leds_gpio);

	ramips_register_gpio_buttons(-1, RT_N15_KEYS_POLL_INTERVAL,
				     ARRAY_SIZE(rt_n15_gpio_buttons),
				     rt_n15_gpio_buttons);

	platform_device_register(&rt_n15_rtl8366s_device);

	rt288x_register_wifi();

	rt288x_eth_data.speed = SPEED_1000;
	rt288x_eth_data.duplex = DUPLEX_FULL;
	rt288x_eth_data.tx_fc = 1;
	rt288x_eth_data.rx_fc = 1;
	rt288x_register_ethernet();
	rt288x_register_wdt();
}

MIPS_MACHINE(RAMIPS_MACH_RT_N15, "RT-N15", "Asus RT-N15", rt_n15_init);

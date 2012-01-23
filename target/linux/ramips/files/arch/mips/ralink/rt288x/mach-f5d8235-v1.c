/*
 *  Belkin F5D825 V1 board support
 *
 *  Copyright (C) 2011 Cezary Jackiewicz <cezary.jackiewicz@gmail.com>
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
#include <asm/mach-ralink/rt288x.h>
#include <asm/mach-ralink/rt288x_regs.h>

#include "devices.h"

#include <linux/rtl8366.h>
#include <linux/ethtool.h>

#include <asm/mach-ralink/ramips_eth_platform.h>

#define F5D8235_GPIO_RTL8366_SCK	2
#define F5D8235_GPIO_RTL8366_SDA	1

#define F5D8235_GPIO_LED_USB_BLUE	7
#define F5D8235_GPIO_LED_USB_ORANGE	8
#define F5D8235_GPIO_BUTTON_WPS		0
#define F5D8235_GPIO_BUTTON_RESET	9

#define F5D8235_BUTTONS_POLL_INTERVAL	20

#ifdef CONFIG_MTD_PARTITIONS
static struct mtd_partition f5d8235_partitions[] = {
	{
		.name	= "uboot",
		.offset	= 0,
		.size	= 0x050000,
		.mask_flags = MTD_WRITEABLE,
	}, {
		.name	= "kernel and rootfs",
		.offset	= 0x050000,
		.size	= 0x790000,
	}, {
		.name	= "rootfs",
		.offset	= 0x1D0000,
		.size	= 0x610000,
	}, {
		.name	= "nvram",
		.offset	= 0x7E0000,
		.size	= 0x010000,
	}, {
		.name	= "factory",
		.offset	= 0x7F0000,
		.size	= 0x010000,
	}
};
#endif /* CONFIG_MTD_PARTITIONS */

static struct physmap_flash_data f5d8235_flash_data = {
#ifdef CONFIG_MTD_PARTITIONS
	.nr_parts	= ARRAY_SIZE(f5d8235_partitions),
	.parts		= f5d8235_partitions,
#endif
};

static struct rtl8366_platform_data f5d8235_rtl8366s_data = {
	.gpio_sda	= F5D8235_GPIO_RTL8366_SDA,
	.gpio_sck	= F5D8235_GPIO_RTL8366_SCK,
};

static struct platform_device f5d8235_rtl8366s_device = {
	.name		= RTL8366S_DRIVER_NAME,
	.id		= -1,
	.dev = {
		.platform_data	= &f5d8235_rtl8366s_data,
	}
};

static struct gpio_led f5d8235_leds_gpio[] __initdata = {
	{
		.name		= "f5d8235-v1:blue:storage",
		.gpio		= F5D8235_GPIO_LED_USB_BLUE,
		.active_low	= 1,
	},{
		.name		= "f5d8235-v1:orange:storage",
		.gpio		= F5D8235_GPIO_LED_USB_ORANGE,
		.active_low	= 1,
	}
};

static struct gpio_button f5d8235_gpio_buttons[] __initdata = {
	{
		.desc		= "reset",
		.type		= EV_KEY,
		.code		= KEY_RESTART,
		.threshold	= 3,
		.gpio		= F5D8235_GPIO_BUTTON_RESET,
		.active_low	= 1,
	}, {
		.desc		= "wps",
		.type		= EV_KEY,
		.code		= KEY_WPS_BUTTON,
		.threshold	= 3,
		.gpio		= F5D8235_GPIO_BUTTON_WPS,
		.active_low	= 1,
	}
};

static void __init f5d8235_init(void)
{
	rt288x_gpio_init(RT2880_GPIO_MODE_UART0 | RT2880_GPIO_MODE_I2C);

	rt288x_register_flash(0, &f5d8235_flash_data);
	rt288x_register_wifi();
	rt288x_register_wdt();

	ramips_register_gpio_leds(-1, ARRAY_SIZE(f5d8235_leds_gpio),
				  f5d8235_leds_gpio);

	ramips_register_gpio_buttons(-1, F5D8235_BUTTONS_POLL_INTERVAL,
				     ARRAY_SIZE(f5d8235_gpio_buttons),
				     f5d8235_gpio_buttons);

	platform_device_register(&f5d8235_rtl8366s_device);

	rt288x_eth_data.speed = SPEED_1000;
	rt288x_eth_data.duplex = DUPLEX_FULL;
	rt288x_eth_data.tx_fc = 1;
	rt288x_eth_data.rx_fc = 1;
	rt288x_register_ethernet();
}

MIPS_MACHINE(RAMIPS_MACH_F5D8235_V1, "F5D8235_V1",
	     "Belkin F5D8235 v1", f5d8235_init);

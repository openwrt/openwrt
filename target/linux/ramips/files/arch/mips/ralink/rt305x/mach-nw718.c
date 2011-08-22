/*
 *  Netcore NW718 board support
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
#include <linux/spi/spi.h>
#include <linux/spi/flash.h>

#include <asm/mach-ralink/machine.h>
#include <asm/mach-ralink/dev-gpio-buttons.h>
#include <asm/mach-ralink/dev-gpio-leds.h>
#include <asm/mach-ralink/rt305x.h>
#include <asm/mach-ralink/rt305x_regs.h>

#include "devices.h"

#define NW718_GPIO_LED_USB		8
#define NW718_GPIO_LED_CPU		13
#define NW718_GPIO_LED_WPS		14

#define NW718_GPIO_BUTTON_WPS		0
#define NW718_GPIO_BUTTON_RESET		10

#define NW718_GPIO_SPI_CS0		3

#define NW718_BUTTONS_POLL_INTERVAL	20

#ifdef CONFIG_MTD_PARTITIONS
static struct mtd_partition nw718_partitions[] = {
	{
		.name	= "u-boot",
		.offset	= 0,
		.size	= 0x030000,
		.mask_flags = MTD_WRITEABLE,
	}, {
		.name	= "config",
		.offset	= 0x030000,
		.size	= 0x020000,
		.mask_flags = MTD_WRITEABLE,
	}, {
		.name	= "factory",
		.offset	= 0x050000,
		.size	= 0x010000,
		.mask_flags = MTD_WRITEABLE,
	}, {
		.name	= "kernel",
		.offset	= 0x060000,
		.size	= 0x090000,
	}, {
		.name	= "rootfs",
		.offset	= 0x150000,
		.size	= 0x2b0000,
	}, {
		.name	= "firmware",
		.offset	= 0x060000,
		.size	= 0x3a0000,
	}
};
#define nw718_nr_parts		ARRAY_SIZE(nw718_partitions)
#else
#define nw718_nr_parts		0
#define nw718_partitions	NULL
#endif /* CONFIG_MTD_PARTITIONS */

static struct flash_platform_data nw718_flash_data = {
	.nr_parts	= nw718_nr_parts,
	.parts		= nw718_partitions,
};

static struct gpio_led nw718_leds_gpio[] __initdata = {
	{
		.name		= "nw718:amber:cpu",
		.gpio		= NW718_GPIO_LED_CPU,
		.active_low	= 1,
	}, {
		.name		= "nw718:amber:usb",
		.gpio		= NW718_GPIO_LED_USB,
		.active_low	= 1,
	}, {
		.name		= "nw718:amber:wps",
		.gpio		= NW718_GPIO_LED_WPS,
		.active_low	= 1,
	}
};

static struct gpio_button nw718_gpio_buttons[] __initdata = {
	{
		.desc		= "reset",
		.type		= EV_KEY,
		.code		= KEY_RESTART,
		.threshold	= 3,
		.gpio		= NW718_GPIO_BUTTON_RESET,
		.active_low	= 1,
	}, {
		.desc		= "wps",
		.type		= EV_KEY,
		.code		= KEY_WPS_BUTTON,
		.threshold	= 3,
		.gpio		= NW718_GPIO_BUTTON_WPS,
		.active_low	= 1,
	}
};

static struct spi_board_info nw718_spi_info[] = {
	{
		.bus_num	= 0,
		.chip_select	= 0,
		.max_speed_hz	= 25000000,
		.modalias	= "m25p80",
		.platform_data	= &nw718_flash_data,
		.controller_data = (void *) NW718_GPIO_SPI_CS0,
	}
};

static void __init nw718_init(void)
{
	rt305x_gpio_init(RT305X_GPIO_MODE_I2C |
			 RT305X_GPIO_MODE_GPIO << RT305X_GPIO_MODE_UART0_SHIFT);

	rt305x_esw_data.vlan_config = RT305X_ESW_VLAN_CONFIG_LLLLW;
	rt305x_register_ethernet();
	ramips_register_gpio_leds(-1, ARRAY_SIZE(nw718_leds_gpio),
				  nw718_leds_gpio);
	ramips_register_gpio_buttons(-1, NW718_BUTTONS_POLL_INTERVAL,
				     ARRAY_SIZE(nw718_gpio_buttons),
				     nw718_gpio_buttons);
	rt305x_register_wifi();
	rt305x_register_wdt();
	rt305x_register_spi(nw718_spi_info, ARRAY_SIZE(nw718_spi_info));
	rt305x_register_usb();
}

MIPS_MACHINE(RAMIPS_MACH_WHR_G300N, "NW718", "Netcore NW718", nw718_init);

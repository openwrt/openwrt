/*
 * linux/arch/mips/jz4740/board-516.c
 *
 * JZ4740 n516 board setup routines.
 *
 * Copyright (c) 2009, Yauhen Kharuzhy <jekhor@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#include <linux/init.h>
#include <linux/sched.h>
#include <linux/ioport.h>
#include <linux/mm.h>
#include <linux/console.h>
#include <linux/delay.h>
#include <linux/i2c.h>
#include <linux/platform_device.h>
#include <linux/mtd/mtd.h>
#include <linux/mmc/jz4740_mmc.h>
#include <linux/mtd/jz4740_nand.h>
#include <linux/leds.h>

#include <linux/power_supply.h>
#include <linux/power/gpio-charger.h>

#include <linux/i2c.h>
#include <linux/i2c-gpio.h>

#include <asm/mach-jz4740/board-n516.h>
#include <asm/mach-jz4740/platform.h>

#include "clock.h"

static long n516_panic_blink(long time)
{
	gpio_set_value(GPIO_LED_ENABLE, 1);
	mdelay(200);
	gpio_set_value(GPIO_LED_ENABLE, 0);
	mdelay(200);

	return 400;
}

static void __init board_gpio_setup(void)
{
/*	jz_gpio_enable_pullup(JZ_GPIO_PORTD(23));
	jz_gpio_enable_pullup(JZ_GPIO_PORTD(24));*/
}

static struct i2c_gpio_platform_data n516_i2c_pdata = {
	.sda_pin = JZ_GPIO_PORTD(23),
	.scl_pin = JZ_GPIO_PORTD(24),
	.udelay = 2,
	.timeout = 3 * HZ,
};

static struct platform_device n516_i2c_device = {
	.name = "i2c-gpio",
	.id = -1,
	.dev = {
		.platform_data = &n516_i2c_pdata,
	},
};

static const struct i2c_board_info n516_i2c_board_info[] = {
	{
		.type		= "LPC524",
		.addr		= 0x54,
	},
	{
		.type		= "lm75a",
		.addr		= 0x48,
	}
};

static struct jz4740_mmc_platform_data n516_mmc_pdata = {
	.gpio_card_detect	= GPIO_SD_CD_N,
	.card_detect_active_low = 1,
	.gpio_read_only		= -1,
	.gpio_power		= GPIO_SD_VCC_EN_N,
	.power_active_low = 1,
};

static struct gpio_led n516_leds[] = {
	{
		.name = "n516:blue:power",
		.gpio = GPIO_LED_ENABLE,
		.default_state = LEDS_GPIO_DEFSTATE_ON,
		.default_trigger = "nand-disk",
	}
};

static struct gpio_led_platform_data n516_leds_pdata = {
	.leds = n516_leds,
	.num_leds = ARRAY_SIZE(n516_leds),
};

static struct platform_device n516_leds_device = {
	.name = "leds-gpio",
	.id = -1,
	.dev = {
		.platform_data = &n516_leds_pdata,
	},
};

static struct mtd_partition n516_partitions[] = {
	{ .name = "NAND BOOT partition",
	  .offset = 0 * 0x100000,
	  .size = 4 * 0x100000,
 	},
	{ .name = "NAND KERNEL partition",
	  .offset = 4 * 0x100000,
	  .size = 4 * 0x100000,
 	},
	{ .name = "NAND ROOTFS partition",
	  .offset = 8 * 0x100000,
	  .size = 504 * 0x100000,
 	},
};

static struct nand_ecclayout n516_ecclayout = {
	.eccbytes = 36,
	.eccpos = {
		 6,  7,  8,  9, 10, 11, 12, 13, 14,
		15, 16, 17, 18, 19, 20, 21, 22, 23,
		24, 25, 26, 27, 28, 29, 30, 31, 32,
		33, 34, 35, 36, 37, 38, 39, 40, 41,
	},
	.oobfree = {
		{.offset = 2,
		 .length = 4},
		{.offset = 42,
		 .length = 22}}
};

static struct jz_nand_platform_data n516_nand_pdata = {
	.ecc_layout = &n516_ecclayout,
	.partitions = n516_partitions,
	.num_partitions = ARRAY_SIZE(n516_partitions),
	.busy_gpio = 94,
};

static char *n516_batteries[] = {
	"n516_battery",
};

static struct gpio_charger_platform_data n516_charger_pdata = {
	.name = "usb",
	.type = POWER_SUPPLY_TYPE_USB,
	.gpio = GPIO_USB_DETECT,
	.gpio_active_low = 1,
	.batteries = n516_batteries,
	.num_batteries = ARRAY_SIZE(n516_batteries),
};

static struct platform_device n516_charger_device = {
	.name = "gpio-charger",
	.dev = {
		.platform_data = &n516_charger_pdata,
	},
};

static struct platform_device *n516_devices[] __initdata = {
	&jz4740_nand_device,
	&n516_leds_device,
	&jz4740_mmc_device,
	&jz4740_i2s_device,
	&jz4740_codec_device,
	&jz4740_rtc_device,
	&jz4740_usb_gdt_device,
	&n516_i2c_device,
	&n516_charger_device,
};

struct jz4740_clock_board_data jz4740_clock_bdata = {
	.ext_rate = 12000000,
	.rtc_rate = 32768,
};

extern int jz_gpiolib_init(void);

static int n516_setup_platform(void)
{
	if (jz_gpiolib_init())
		panic("Failed to initalize jz gpio\n");

	jz4740_clock_init();
	board_gpio_setup();

	panic_blink = n516_panic_blink;
	i2c_register_board_info(0, n516_i2c_board_info, ARRAY_SIZE(n516_i2c_board_info));
	jz4740_mmc_device.dev.platform_data = &n516_mmc_pdata;
	jz4740_nand_device.dev.platform_data = &n516_nand_pdata;

	return platform_add_devices(n516_devices, ARRAY_SIZE(n516_devices));
}
arch_initcall(n516_setup_platform);

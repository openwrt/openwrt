/*
 *  TP-LINK TL-WR703N board support
 *
 *  Copyright (C) 2011 dongyuqi <729650915@qq.com>
 *  Copyright (C) 2011 Gabor Juhos <juhosg@openwrt.org>
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 */

#include <linux/mtd/mtd.h>
#include <linux/mtd/partitions.h>
#include <linux/spi/flash.h>
#include <linux/gpio.h>

#include <asm/mach-ar71xx/ar71xx.h>

#include "machtype.h"
#include "devices.h"
#include "dev-ar9xxx-wmac.h"
#include "dev-gpio-buttons.h"
#include "dev-leds-gpio.h"
#include "dev-m25p80.h"
#include "dev-usb.h"

#define TL_WR703N_GPIO_LED_SYSTEM	27
#define TL_WR703N_GPIO_BTN_RESET	11

#define TL_WR703N_GPIO_USB_POWER	8

#define TL_WR703N_KEYS_POLL_INTERVAL	20	/* msecs */
#define TL_WR703N_KEYS_DEBOUNCE_INTERVAL	(3 * TL_WR703N_KEYS_POLL_INTERVAL)

#ifdef CONFIG_MTD_PARTITIONS
static struct mtd_partition tl_wr703n_parts[] = {
	{
		.name		= "u-boot",
		.offset		= 0,
		.size		= 0x020000,
		.mask_flags	= MTD_WRITEABLE,
	}, {
		.name		= "kernel",
		.offset		= 0x020000,
		.size		= 0x100000,
	}, {
		.name		= "rootfs",
		.offset		= 0x120000,
		.size		= 0x2d0000,
	}, {
		.name		= "art",
		.offset		= 0x3f0000,
		.size		= 0x010000,
		.mask_flags	= MTD_WRITEABLE,
	}, {
		.name		= "firmware",
		.offset		= 0x020000,
		.size		= 0x3d0000,
	}
};
#define tl_wr703n_nr_parts	ARRAY_SIZE(tl_wr703n_parts)
#else
#define tl_wr703n_parts		NULL
#define tl_wr703n_nr_parts	0
#endif /* CONFIG_MTD_PARTITIONS */

static struct flash_platform_data tl_wr703n_flash_data = {
	.parts		= tl_wr703n_parts,
	.nr_parts	= tl_wr703n_nr_parts,
};

static struct gpio_led tl_wr703n_leds_gpio[] __initdata = {
	{
		.name		= "tp-link:blue:system",
		.gpio		= TL_WR703N_GPIO_LED_SYSTEM,
		.active_low	= 1,
	},
};

static struct gpio_keys_button tl_wr703n_gpio_keys[] __initdata = {
	{
		.desc		= "reset",
		.type		= EV_KEY,
		.code		= KEY_RESTART,
		.debounce_interval = TL_WR703N_KEYS_DEBOUNCE_INTERVAL,
		.gpio		= TL_WR703N_GPIO_BTN_RESET,
		.active_low	= 1,
	}
};

static void __init tl_wr703n_setup(void)
{
	u8 *mac = (u8 *) KSEG1ADDR(0x1f01fc00);
	u8 *ee = (u8 *) KSEG1ADDR(0x1fff1000);

	ar71xx_add_device_m25p80(&tl_wr703n_flash_data);
	ar71xx_add_device_leds_gpio(-1, ARRAY_SIZE(tl_wr703n_leds_gpio),
					tl_wr703n_leds_gpio);
	ar71xx_register_gpio_keys_polled(-1, TL_WR703N_KEYS_POLL_INTERVAL,
					 ARRAY_SIZE(tl_wr703n_gpio_keys),
					 tl_wr703n_gpio_keys);

	gpio_request(TL_WR703N_GPIO_USB_POWER, "USB power");
	gpio_direction_output(TL_WR703N_GPIO_USB_POWER, 1);
	ar71xx_add_device_usb();

	ar71xx_init_mac(ar71xx_eth0_data.mac_addr, mac, 0);

	ar71xx_add_device_mdio(0, 0x0);
	ar71xx_add_device_eth(0);

	ar9xxx_add_device_wmac(ee, mac);
}

MIPS_MACHINE(AR71XX_MACH_TL_WR703N, "TL-WR703N", "TP-LINK TL-WR703N v1",
	     tl_wr703n_setup);

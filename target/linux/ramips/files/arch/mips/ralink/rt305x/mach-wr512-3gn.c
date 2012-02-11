/*
 *  Unbranded router from DX board support
 *  Also known as *WR512*-3GN by local vendors
 *  e.g. WS-WR512N1, Sin Hon SH-WR512NU, and etc
 *  (http://www.dealextreme.com/p/portable-wireless-n-3g-router-cdma2000-evdo-td-scdma-hspa-wcdma-45639)
 *  This router is also known to be rebranded and sold by a number of local
 *  vendors in several countries.
 *
 *  Copyright (C) 2011 Andrew Andrianov <necromant@necromant.ath.cx>
 *  Based on MOFI3500-3N code by
 *  Copyright (C) 2011 Layne Edwards <ledwards76@gmail.com>
 *
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


#define WR512_3GN_GPIO_LED_3G		9
#define WR512_3GN_GPIO_LED_GATEWAY	11
#define WR512_3GN_GPIO_LED_AP		12
#define WR512_3GN_GPIO_LED_STATION	13
#define WR512_3GN_GPIO_LED_WPS		14

#define WR512_3GN_GPIO_BUTTON_RESET	10
#define WR512_3GN_GPIO_BUTTON_CONNECT	7
#define WR512_3GN_GPIO_BUTTON_WPS	0
#define WR512_3GN_GPIO_BUTTON_WPS2	8

#define WR512_3GN_KEYS_POLL_INTERVAL	20
#define WR512_3GN_KEYS_DEBOUNCE_INTERVAL (3 * WR512_3GN_KEYS_POLL_INTERVAL)

static struct mtd_partition wr512_3gn_partitions[] = {
	{
		.name	= "u-boot",
		.offset	= 0,
		.size	= 0x030000,
		.mask_flags = MTD_WRITEABLE,
	}, {
		.name	= "devdata",
		.offset	= 0x030000,
		.size	= 0x010000,
		.mask_flags = MTD_WRITEABLE,
	}, {
		.name	= "devconf",
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
		.size	= 0x2e0000,
	}, {
		.name	= "firmware",
		.offset	= 0x050000,
		.size	= 0x3b0000,
	}
};

static struct physmap_flash_data wr512_3gn_flash_data = {
	.nr_parts	= ARRAY_SIZE(wr512_3gn_partitions),
	.parts		= wr512_3gn_partitions,
};

static struct gpio_led wr512_3gn_leds_gpio[] __initdata = {
	{
		.name		= "wr512:green:3g",
		.gpio		= WR512_3GN_GPIO_LED_3G,
		.active_low	= 1,
	}, {
		.name		= "wr512:green:gateway",
		.gpio		= WR512_3GN_GPIO_LED_GATEWAY,
		.active_low	= 1,
	}, {
		.name		= "wr512:green:ap",
		.gpio		= WR512_3GN_GPIO_LED_AP,
		.active_low	= 1,
	}, {
		.name		= "wr512:green:wps",
		.gpio		= WR512_3GN_GPIO_LED_WPS,
		.active_low	= 1,
	}, {
		.name		= "wr512:green:station",
		.gpio		= WR512_3GN_GPIO_LED_STATION,
		.active_low	= 1,
	}
};

static struct gpio_keys_button wr512_3gn_gpio_buttons[] __initdata = {
	{
		.desc		= "reset_wps",
		.type		= EV_KEY,
		.code		= KEY_RESTART,
		.debounce_interval = WR512_3GN_KEYS_DEBOUNCE_INTERVAL,
		.gpio		= WR512_3GN_GPIO_BUTTON_RESET,
		.active_low	= 1,
	}, {
		.desc		= "mode",
		.type		= EV_KEY,
		.code		= KEY_M,
		.debounce_interval = WR512_3GN_KEYS_DEBOUNCE_INTERVAL,
		.gpio		= WR512_3GN_GPIO_BUTTON_CONNECT,
		.active_low	= 1,
	}
};

#define WR512_3GN_GPIO_MODE \
	((RT305X_GPIO_MODE_GPIO << RT305X_GPIO_MODE_UART0_SHIFT) | \
	 RT305X_GPIO_MODE_MDIO)

static void __init wr512_3gn_init(void)
{
	rt305x_gpio_init(WR512_3GN_GPIO_MODE);

	rt305x_register_flash(0, &wr512_3gn_flash_data);
	rt305x_esw_data.vlan_config = RT305X_ESW_VLAN_CONFIG_LLLLW;
	rt305x_register_ethernet();
	ramips_register_gpio_leds(-1, ARRAY_SIZE(wr512_3gn_leds_gpio),
				  wr512_3gn_leds_gpio);
	ramips_register_gpio_buttons(-1, WR512_3GN_KEYS_POLL_INTERVAL,
				     ARRAY_SIZE(wr512_3gn_gpio_buttons),
				     wr512_3gn_gpio_buttons);
	rt305x_register_wifi();
	rt305x_register_wdt();
	rt305x_register_usb();
}

MIPS_MACHINE(RAMIPS_MACH_WR512_3GN, "WR512-3GN", "WR512-3GN-like router",
	      wr512_3gn_init);

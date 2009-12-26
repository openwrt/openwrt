/*
 *  TP-LINK TL-WR941ND board support
 *
 *  Copyright (C) 2009 Gabor Juhos <juhosg@openwrt.org>
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 */

#include <linux/mtd/mtd.h>
#include <linux/mtd/partitions.h>

#include <asm/mips_machine.h>
#include <asm/mach-ar71xx/ar71xx.h>

#include "devices.h"
#include "dev-m25p80.h"
#include "dev-ar913x-wmac.h"
#include "dev-gpio-buttons.h"
#include "dev-leds-gpio.h"

#define TL_WR941ND_GPIO_LED_SYSTEM	2
#define TL_WR941ND_GPIO_LED_QSS		5

#define TL_WR941ND_GPIO_BTN_RESET	3
#define TL_WR941ND_GPIO_BTN_QSS		7

#define TL_WR941ND_BUTTONS_POLL_INTERVAL	20

#ifdef CONFIG_MTD_PARTITIONS
static struct mtd_partition tl_wr941nd_partitions[] = {
	{
		.name		= "u-boot",
		.offset		= 0,
		.size		= 0x020000,
		.mask_flags	= MTD_WRITEABLE,
	} , {
		.name		= "kernel",
		.offset		= 0x020000,
		.size		= 0x140000,
	} , {
		.name		= "rootfs",
		.offset		= 0x160000,
		.size		= 0x290000,
	} , {
		.name		= "art",
		.offset		= 0x3f0000,
		.size		= 0x010000,
		.mask_flags	= MTD_WRITEABLE,
	} , {
		.name		= "firmware",
		.offset		= 0x020000,
		.size		= 0x3d0000,
	}
};
#endif /* CONFIG_MTD_PARTITIONS */

static struct flash_platform_data tl_wr941nd_flash_data = {
#ifdef CONFIG_MTD_PARTITIONS
        .parts          = tl_wr941nd_partitions,
        .nr_parts       = ARRAY_SIZE(tl_wr941nd_partitions),
#endif
};

static struct gpio_led tl_wr941nd_leds_gpio[] __initdata = {
	{
		.name		= "tl-wr941nd:green:system",
		.gpio		= TL_WR941ND_GPIO_LED_SYSTEM,
		.active_low	= 1,
	}, {
		.name		= "tl-wr941nd:red:qss",
		.gpio		= TL_WR941ND_GPIO_LED_QSS,
		.active_low	= 1,
	}
};

static struct gpio_button tl_wr941nd_gpio_buttons[] __initdata = {
	{
		.desc		= "reset",
		.type		= EV_KEY,
		.code		= BTN_0,
		.threshold	= 5,
		.gpio		= TL_WR941ND_GPIO_BTN_RESET,
		.active_low	= 1,
	}, {
		.desc		= "qss",
		.type		= EV_KEY,
		.code		= BTN_1,
		.threshold	= 5,
		.gpio		= TL_WR941ND_GPIO_BTN_QSS,
		.active_low	= 1,
	}
};

static struct dsa_chip_data tl_wr941nd_dsa_chip = {
	.port_names[0]  = "wan",
	.port_names[1]  = "lan1",
	.port_names[2]  = "lan2",
	.port_names[3]  = "lan3",
	.port_names[4]  = "lan4",
	.port_names[5]  = "cpu",
};

static struct dsa_platform_data tl_wr941nd_dsa_data = {
	.nr_chips	= 1,
	.chip		= &tl_wr941nd_dsa_chip,
};

static void __init tl_wr941nd_setup(void)
{
	u8 *mac = (u8 *) KSEG1ADDR(0x1f01fc00);

	ar71xx_set_mac_base(mac);

	ar71xx_add_device_mdio(0x0);

	ar71xx_eth0_data.phy_if_mode = PHY_INTERFACE_MODE_RMII;
	ar71xx_eth0_data.phy_mask = 0x0;
	ar71xx_eth0_data.speed = SPEED_100;
	ar71xx_eth0_data.duplex = DUPLEX_FULL;

	ar71xx_add_device_eth(0);
	ar71xx_add_device_dsa(0, &tl_wr941nd_dsa_data);

	ar71xx_add_device_m25p80(&tl_wr941nd_flash_data);

	ar71xx_add_device_leds_gpio(-1, ARRAY_SIZE(tl_wr941nd_leds_gpio),
					tl_wr941nd_leds_gpio);

	ar71xx_add_device_gpio_buttons(-1, TL_WR941ND_BUTTONS_POLL_INTERVAL,
					ARRAY_SIZE(tl_wr941nd_gpio_buttons),
					tl_wr941nd_gpio_buttons);
	ar913x_add_device_wmac();
}

MIPS_MACHINE(AR71XX_MACH_TL_WR941ND, "TP-LINK TL-WR941ND", tl_wr941nd_setup);

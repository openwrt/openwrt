/*
 *  Buffalo WZR-HP-AG300H board support
 *
 *  Copyright (C) 2011 Felix Fietkau <nbd@openwrt.org>
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 */

#include <linux/platform_device.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/partitions.h>
#include <linux/mtd/concat.h>

#include <asm/mips_machine.h>
#include <asm/mach-ar71xx/ar71xx.h>
#include <asm/mach-ar71xx/gpio.h>

#include "machtype.h"
#include "devices.h"
#include "dev-ap94-pci.h"
#include "dev-gpio-buttons.h"
#include "dev-leds-gpio.h"
#include "dev-m25p80.h"
#include "dev-usb.h"

#define WZRHPAG300H_MAC_OFFSET		0x20c
#define WZRHPAG300H_KEYS_POLL_INTERVAL     20      /* msecs */
#define WZRHPAG300H_KEYS_DEBOUNCE_INTERVAL (3 * WZRHPAG300H_KEYS_POLL_INTERVAL)

#ifdef CONFIG_MTD_PARTITIONS
static struct mtd_partition wzrhpag300h_flash_partitions[] = {
	{
		.name		= "u-boot",
		.offset		= 0,
		.size		= 0x0040000,
		.mask_flags	= MTD_WRITEABLE,
	}, {
		.name		= "u-boot-env",
		.offset		= 0x0040000,
		.size		= 0x0010000,
		.mask_flags	= MTD_WRITEABLE,
	}, {
		.name		= "art",
		.offset		= 0x0050000,
		.size		= 0x0010000,
		.mask_flags	= MTD_WRITEABLE,
	}, {
		.name		= "kernel",
		.offset		= 0x0060000,
		.size		= 0x0100000,
	}, {
		.name		= "rootfs",
		.offset		= 0x0160000,
		.size		= 0x1e90000,
	}, {
		.name		= "user_property",
		.offset		= 0x1ff0000,
		.size		= 0x0010000,
		.mask_flags	= MTD_WRITEABLE,
	}, {
		.name		= "firmware",
		.offset		= 0x0060000,
		.size		= 0x1f90000,
	}
};

#endif /* CONFIG_MTD_PARTITIONS */

static struct mtd_info *concat_devs[2] = { NULL, NULL };
static struct work_struct mtd_concat_work;

static void mtd_concat_add_work(struct work_struct *work)
{
	struct mtd_info *mtd;

	mtd = mtd_concat_create(concat_devs, ARRAY_SIZE(concat_devs), "flash");

#ifdef CONFIG_MTD_PARTITIONS
	add_mtd_partitions(mtd, wzrhpag300h_flash_partitions,
			   ARRAY_SIZE(wzrhpag300h_flash_partitions));
#else
	add_mtd_device(mtd);
#endif
}

static void mtd_concat_add(struct mtd_info *mtd)
{
	static bool registered = false;

	if (registered)
		return;

	if (!strcmp(mtd->name, "spi0.0"))
		concat_devs[0] = mtd;
	else if (!strcmp(mtd->name, "spi0.1"))
		concat_devs[1] = mtd;
	else
		return;

	if (!concat_devs[0] || !concat_devs[1])
		return;

	registered = true;
	INIT_WORK(&mtd_concat_work, mtd_concat_add_work);
	schedule_work(&mtd_concat_work);
}

static void mtd_concat_remove(struct mtd_info *mtd)
{
}

static void add_mtd_concat_notifier(void)
{
	static struct mtd_notifier not = {
		.add = mtd_concat_add,
		.remove = mtd_concat_remove,
	};

	register_mtd_user(&not);
}

static struct gpio_led wzrhpag300h_leds_gpio[] __initdata = {
	{
		.name		= "buffalo:red:diag",
		.gpio		= 1,
		.active_low	= 1,
	},
};


static struct gpio_keys_button wzrhpag300h_gpio_keys[] __initdata = {
	{
		.desc		= "reset",
		.type		= EV_KEY,
		.code		= KEY_RESTART,
		.debounce_interval = WZRHPAG300H_KEYS_DEBOUNCE_INTERVAL,
		.gpio		= 11,
		.active_low	= 1,
	}, {
		.desc		= "usb",
		.type		= EV_KEY,
		.code		= BTN_2,
		.debounce_interval = WZRHPAG300H_KEYS_DEBOUNCE_INTERVAL,
		.gpio		= 3,
		.active_low	= 1,
	}, {
		.desc		= "aoss",
		.type		= EV_KEY,
		.code		= KEY_WPS_BUTTON,
		.debounce_interval = WZRHPAG300H_KEYS_DEBOUNCE_INTERVAL,
		.gpio		= 5,
		.active_low	= 1,
	}, {
		.desc		= "router_auto",
		.type		= EV_KEY,
		.code		= BTN_6,
		.debounce_interval = WZRHPAG300H_KEYS_DEBOUNCE_INTERVAL,
		.gpio		= 6,
		.active_low	= 1,
	}, {
		.desc		= "router_off",
		.type		= EV_KEY,
		.code		= BTN_5,
		.debounce_interval = WZRHPAG300H_KEYS_DEBOUNCE_INTERVAL,
		.gpio		= 7,
		.active_low	= 1,
	}
};

static struct spi_board_info ar71xx_spi_info[] = {
	{
		.bus_num	= 0,
		.chip_select	= 0,
		.max_speed_hz   = 25000000,
		.modalias   = "m25p80",
	},
	{
		.bus_num	= 0,
		.chip_select	= 1,
		.max_speed_hz   = 25000000,
		.modalias   = "m25p80",
	}
};

static void __init wzrhpag300h_setup(void)
{
	u8 *eeprom1 = (u8 *) KSEG1ADDR(0x1f051000);
	u8 *eeprom2 = (u8 *) KSEG1ADDR(0x1f055000);
	u8 *mac1 = eeprom1 + WZRHPAG300H_MAC_OFFSET;
	u8 *mac2 = eeprom2 + WZRHPAG300H_MAC_OFFSET;

	ar71xx_init_mac(ar71xx_eth0_data.mac_addr, mac1, 0);
	ar71xx_init_mac(ar71xx_eth1_data.mac_addr, mac2, 1);

	ar71xx_add_device_mdio(0, ~(BIT(0) | BIT(4)));

	ar71xx_eth0_data.phy_if_mode = PHY_INTERFACE_MODE_RGMII;
	ar71xx_eth0_data.speed = SPEED_1000;
	ar71xx_eth0_data.duplex = DUPLEX_FULL;
	ar71xx_eth0_data.phy_mask = BIT(0);

	ar71xx_eth1_data.phy_if_mode = PHY_INTERFACE_MODE_RGMII;
	ar71xx_eth1_data.phy_mask = BIT(4);

	ar71xx_add_device_eth(0);
	ar71xx_add_device_eth(1);

	ar71xx_add_device_usb();
	gpio_request(2, "usb");
	gpio_direction_output(2, 1);

	ar71xx_add_device_leds_gpio(-1, ARRAY_SIZE(wzrhpag300h_leds_gpio),
					wzrhpag300h_leds_gpio);

	ar71xx_register_gpio_keys_polled(-1, WZRHPAG300H_KEYS_POLL_INTERVAL,
					 ARRAY_SIZE(wzrhpag300h_gpio_keys),
					 wzrhpag300h_gpio_keys);

	ar71xx_add_device_spi(NULL, ar71xx_spi_info,
			      ARRAY_SIZE(ar71xx_spi_info));

	add_mtd_concat_notifier();

	ap94_pci_init(eeprom1, mac1, eeprom2, mac2);
}

MIPS_MACHINE(AR71XX_MACH_WZR_HP_AG300H, "WZR-HP-AG300H",
	     "Buffalo WZR-HP-AG300H", wzrhpag300h_setup);


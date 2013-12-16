/*
 *  MikroTik RouterBOARD 91X support
 *
 *  Copyright (C) 2013 Gabor Juhos <juhosg@openwrt.org>
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 */

#define pr_fmt(fmt) "rb91x: " fmt

#include <linux/phy.h>
#include <linux/delay.h>
#include <linux/platform_device.h>
#include <linux/ath9k_platform.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/nand.h>
#include <linux/mtd/partitions.h>
#include <linux/spi/spi.h>
#include <linux/spi/74x164.h>
#include <linux/spi/flash.h>
#include <linux/routerboot.h>
#include <linux/gpio.h>

#include <asm/prom.h>
#include <asm/mach-ath79/ath79.h>
#include <asm/mach-ath79/ath79_spi_platform.h>
#include <asm/mach-ath79/ar71xx_regs.h>

#include "common.h"
#include "dev-eth.h"
#include "dev-leds-gpio.h"
#include "dev-m25p80.h"
#include "dev-nfc.h"
#include "dev-usb.h"
#include "dev-wmac.h"
#include "machtypes.h"
#include "pci.h"
#include "routerboot.h"

#define RB_ROUTERBOOT_OFFSET	0x0000
#define RB_ROUTERBOOT_MIN_SIZE	0xb000
#define RB_HARD_CFG_SIZE	0x1000
#define RB_BIOS_OFFSET		0xd000
#define RB_BIOS_SIZE		0x1000
#define RB_SOFT_CFG_OFFSET	0xf000
#define RB_SOFT_CFG_SIZE	0x1000

#define RB91X_FLAG_USB		BIT(0)
#define RB91X_FLAG_PCIE		BIT(1)

struct rb_board_info {
	const char *name;
	u32 flags;
};

static struct mtd_partition rb711gr100_spi_partitions[] = {
	{
		.name		= "routerboot",
		.offset		= RB_ROUTERBOOT_OFFSET,
		.mask_flags	= MTD_WRITEABLE,
	}, {
		.name		= "hard_config",
		.size		= RB_HARD_CFG_SIZE,
		.mask_flags	= MTD_WRITEABLE,
	}, {
		.name		= "bios",
		.offset		= RB_BIOS_OFFSET,
		.size		= RB_BIOS_SIZE,
		.mask_flags	= MTD_WRITEABLE,
	}, {
		.name		= "soft_config",
		.size		= RB_SOFT_CFG_SIZE,
	}
};

static struct flash_platform_data rb711gr100_spi_flash_data = {
	.parts		= rb711gr100_spi_partitions,
	.nr_parts	= ARRAY_SIZE(rb711gr100_spi_partitions),
};

static void __init rb711gr100_init_partitions(const struct rb_info *info)
{
	rb711gr100_spi_partitions[0].size = info->hard_cfg_offs;
	rb711gr100_spi_partitions[1].offset = info->hard_cfg_offs;

	rb711gr100_spi_partitions[3].offset = info->soft_cfg_offs;
}

void __init rb711gr100_wlan_init(void)
{
	char *caldata;
	u8 wlan_mac[ETH_ALEN];

	caldata = rb_get_wlan_data();
	if (caldata == NULL)
		return;

	ath79_init_mac(wlan_mac, ath79_mac_base, 1);
	ath79_register_wmac(caldata + 0x1000, wlan_mac);

	kfree(caldata);
}

#define RB_BOARD_INFO(_name, _flags)	\
	{				\
		.name = (_name),	\
		.flags = (_flags),	\
	}

static const struct rb_board_info rb711gr100_boards[] __initconst = {
	RB_BOARD_INFO("911G-2HPnD", 0),
	RB_BOARD_INFO("911G-5HPnD", 0),
	RB_BOARD_INFO("912UAG-2HPnD", RB91X_FLAG_USB | RB91X_FLAG_PCIE),
	RB_BOARD_INFO("912UAG-5HPnD", RB91X_FLAG_USB | RB91X_FLAG_PCIE),
};

static u32 rb711gr100_get_flags(const struct rb_info *info)
{
	int i;

	for (i = 0; i < ARRAY_SIZE(rb711gr100_boards); i++) {
		const struct rb_board_info *bi;

		bi = &rb711gr100_boards[i];
		if (strcmp(info->board_name, bi->name) == 0)
			return bi->flags;
	}

	return 0;
}

static void __init rb711gr100_setup(void)
{
	const struct rb_info *info;
	char buf[64];
	u32 flags;

	info = rb_init_info((void *) KSEG1ADDR(0x1f000000), 0x10000);
	if (!info)
		return;

	scnprintf(buf, sizeof(buf), "Mikrotik RouterBOARD %s",
		  (info->board_name) ? info->board_name : "");
	mips_set_machine_name(buf);

	rb711gr100_init_partitions(info);
	ath79_register_m25p80(&rb711gr100_spi_flash_data);

	ath79_setup_ar934x_eth_cfg(AR934X_ETH_CFG_RGMII_GMAC0 |
				   AR934X_ETH_CFG_SW_ONLY_MODE);

	ath79_register_mdio(0, 0x0);

	ath79_init_mac(ath79_eth0_data.mac_addr, ath79_mac_base, 0);
	ath79_eth0_data.phy_if_mode = PHY_INTERFACE_MODE_RGMII;
	ath79_eth0_data.phy_mask = BIT(0);

	ath79_register_eth(0);

	rb711gr100_wlan_init();

	platform_device_register_simple("rb91x-nand", -1, NULL, 0);

	flags = rb711gr100_get_flags(info);

	if (flags & RB91X_FLAG_USB)
		ath79_register_usb();

	if (flags & RB91X_FLAG_PCIE)
		ath79_register_pci();

}

MIPS_MACHINE_NONAME(ATH79_MACH_RB_711GR100, "711Gr100", rb711gr100_setup);

/*
 *  Atheros G450 board support
 *
 *  Copyright (C) 2010 Felix Fietkau <nbd@openwrt.org>
 *  Copyright (C) 2008-2009 Gabor Juhos <juhosg@openwrt.org>
 *  Copyright (C) 2008 Imre Kaloz <kaloz@openwrt.org>
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 */

#include <linux/mtd/mtd.h>
#include <linux/mtd/partitions.h>
#include <asm/mach-ar71xx/ar71xx.h>

#include "machtype.h"
#include "devices.h"
#include "dev-m25p80.h"
#include "dev-ap91-pci.h"

#ifdef CONFIG_MTD_PARTITIONS
static struct mtd_partition wzrhpg450h_partitions[] = {
	{
		.name		= "u-boot",
		.offset		= 0,
		.size		= 0x0040000,
		.mask_flags	= MTD_WRITEABLE,
	}, {
		.name		= "u-boot-env",
		.offset		= 0x0040000,
		.size		= 0x0010000,
	}, {
		.name		= "ART",
		.offset		= 0x0050000,
		.size		= 0x0010000,
		.mask_flags	= MTD_WRITEABLE,
	}, {
		.name		= "uImage",
		.offset		= 0x0060000,
		.size		= 0x0100000,
	}, {
		.name		= "rootfs",
		.offset		= 0x0160000,
		.size		= 0x1e80000,
	}, {
		.name		= "user_property",
		.offset		= 0x1fe0000,
		.size		= 0x0020000,
	}, {
		.name		= "firmware",
		.offset		= 0x0060000,
		.size		= 0x1f80000,
	}
};
#endif /* CONFIG_MTD_PARTITIONS */

static struct flash_platform_data wzrhpg450h_flash_data = {
#ifdef CONFIG_MTD_PARTITIONS
	.parts		= wzrhpg450h_partitions,
	.nr_parts	= ARRAY_SIZE(wzrhpg450h_partitions),
#endif
};

static void __init wzrhpg450h_init(void)
{
	u8 *ee = (u8 *) KSEG1ADDR(0x1f051000);
	u8 *mac = (u8 *) ee + 2;

	ar71xx_add_device_m25p80_multi(&wzrhpg450h_flash_data);

	ar71xx_add_device_mdio(0, ~BIT(0));
	ar71xx_init_mac(ar71xx_eth0_data.mac_addr, mac, 0);
	ar71xx_eth0_data.phy_if_mode = PHY_INTERFACE_MODE_RGMII;
	ar71xx_eth0_data.speed = SPEED_1000;
	ar71xx_eth0_data.duplex = DUPLEX_FULL;
	ar71xx_eth0_data.phy_mask = BIT(0);

	ar71xx_add_device_eth(0);

	ap91_pci_init(ee, NULL);
}

MIPS_MACHINE(AR71XX_MACH_WZR_HP_G450H, "WZR-HP-G450H", "Buffalo WZR-HP-G450H",
	     wzrhpg450h_init);

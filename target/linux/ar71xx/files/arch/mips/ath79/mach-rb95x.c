/*
 *  MikroTik RouterBOARD 95X support
 *
 *  Copyright (C) 2012 Stijn Tintel <stijn@linux-ipv6.be>
 *  Copyright (C) 2012 Gabor Juhos <juhosg@openwrt.org>
 *  Copyright (C) 2013 Kamil Trzcinski <ayufan@ayufan.eu>
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 */

#define pr_fmt(fmt) "rb95x: " fmt

#include <linux/phy.h>
#include <linux/delay.h>
#include <linux/platform_device.h>
#include <linux/ath9k_platform.h>
#include <linux/ar8216_platform.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/nand.h>
#include <linux/mtd/partitions.h>
#include <linux/spi/spi.h>
#include <linux/spi/flash.h>
#include <linux/routerboot.h>
#include <linux/gpio.h>

#include <asm/mach-ath79/ath79.h>
#include <asm/mach-ath79/ar71xx_regs.h>

#include "common.h"
#include "dev-eth.h"
#include "dev-m25p80.h"
#include "dev-nfc.h"
#include "dev-usb.h"
#include "dev-wmac.h"
#include "machtypes.h"
#include "routerboot.h"

#define RB95X_GPIO_NAND_NCE	14

static struct mtd_partition rb95x_nand_partitions[] = {
	{
		.name	= "booter",
		.offset	= 0,
		.size	= (256 * 1024),
		.mask_flags = MTD_WRITEABLE,
	},
	{
		.name	= "kernel",
		.offset	= (256 * 1024),
		.size	= (4 * 1024 * 1024) - (256 * 1024),
	},
	{
		.name	= "rootfs",
		.offset	= MTDPART_OFS_NXTBLK,
		.size	= MTDPART_SIZ_FULL,
	},
};

static struct ar8327_pad_cfg rb95x_ar8327_pad0_cfg = {
	.mode = AR8327_PAD_MAC_RGMII,
	.txclk_delay_en = true,
	.rxclk_delay_en = true,
	.txclk_delay_sel = AR8327_CLK_DELAY_SEL1,
	.rxclk_delay_sel = AR8327_CLK_DELAY_SEL2,
};

static struct ar8327_platform_data rb95x_ar8327_data = {
	.pad0_cfg = &rb95x_ar8327_pad0_cfg,
	.port0_cfg = {
		.force_link = 1,
		.speed = AR8327_PORT_SPEED_1000,
		.duplex = 1,
		.txpause = 1,
		.rxpause = 1,
	}
};

static struct mdio_board_info rb95x_mdio0_info[] = {
	{
		.bus_id = "ag71xx-mdio.0",
		.phy_addr = 0,
		.platform_data = &rb95x_ar8327_data,
	},
};

void __init rb95x_wlan_init(void)
{
	char *art_buf;
	u8 wlan_mac[ETH_ALEN];

	art_buf = rb_get_wlan_data();
	if (art_buf == NULL)
		return;

	ath79_init_mac(wlan_mac, ath79_mac_base, 11);
	ath79_register_wmac(art_buf + 0x1000, wlan_mac);

	kfree(art_buf);
}

static void rb95x_nand_select_chip(int chip_no)
{
	switch (chip_no) {
	case 0:
		gpio_set_value(RB95X_GPIO_NAND_NCE, 0);
		break;
	default:
		gpio_set_value(RB95X_GPIO_NAND_NCE, 1);
		break;
	}
	ndelay(500);
}

static struct nand_ecclayout rb95x_nand_ecclayout = {
	.eccbytes	= 6,
	.eccpos		= { 8, 9, 10, 13, 14, 15 },
	.oobavail	= 9,
	.oobfree	= { { 0, 4 }, { 6, 2 }, { 11, 2 }, { 4, 1 } }
};

static int rb95x_nand_scan_fixup(struct mtd_info *mtd)
{
	struct nand_chip *chip = mtd->priv;

	if (mtd->writesize == 512) {
		/*
		 * Use the OLD Yaffs-1 OOB layout, otherwise RouterBoot
		 * will not be able to find the kernel that we load.
		 */
		chip->ecc.layout = &rb95x_nand_ecclayout;
	}

	return 0;
}

void __init rb95x_nand_init(void)
{
	gpio_request_one(RB95X_GPIO_NAND_NCE, GPIOF_OUT_INIT_HIGH, "NAND nCE");

	ath79_nfc_set_scan_fixup(rb95x_nand_scan_fixup);
	ath79_nfc_set_parts(rb95x_nand_partitions,
			    ARRAY_SIZE(rb95x_nand_partitions));
	ath79_nfc_set_select_chip(rb95x_nand_select_chip);
	ath79_nfc_set_swap_dma(true);
	ath79_register_nfc();
}

static int __init rb95x_setup(void)
{
	const struct rb_info *info;

	info = rb_init_info((void *)(KSEG1ADDR(0x1f00000)), 0x10000);
	if (!info)
		return -EINVAL;

	rb95x_nand_init();

	ath79_setup_ar934x_eth_cfg(AR934X_ETH_CFG_RGMII_GMAC0 |
				   AR934X_ETH_CFG_SW_ONLY_MODE);

	ath79_register_mdio(0, 0x0);

	mdiobus_register_board_info(rb95x_mdio0_info,
				    ARRAY_SIZE(rb95x_mdio0_info));

	ath79_init_mac(ath79_eth0_data.mac_addr, ath79_mac_base, 0);
	ath79_eth0_data.phy_if_mode = PHY_INTERFACE_MODE_RGMII;
	ath79_eth0_data.phy_mask = BIT(0);

	ath79_register_eth(0);

	return 0;
}

static void __init rb951g_setup(void)
{
	if (rb95x_setup())
		return;

	rb95x_wlan_init();
	ath79_register_usb();
}

MIPS_MACHINE(ATH79_MACH_RB_951G, "951G", "MikroTik RouterBOARD 951G-2HnD",
	     rb951g_setup);

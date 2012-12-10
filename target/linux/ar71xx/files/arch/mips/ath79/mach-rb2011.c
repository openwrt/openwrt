/*
 *  MikroTik RouterBOARD 2011 support
 *
 *  Copyright (C) 2012 Stijn Tintel <stijn@linux-ipv6.be>
 *  Copyright (C) 2012 Gabor Juhos <juhosg@openwrt.org>
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 */

#define pr_fmt(fmt) "rb2011: " fmt

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
#include <linux/rle.h>
#include <linux/routerboot.h>
#include <linux/gpio.h>

#include <asm/mach-ath79/ath79.h>
#include <asm/mach-ath79/ar71xx_regs.h>

#include "common.h"
#include "dev-eth.h"
#include "dev-m25p80.h"
#include "dev-nfc.h"
#include "dev-wmac.h"
#include "machtypes.h"
#include "routerboot.h"

#define RB2011_GPIO_NAND_NCE	14

#define RB_ROUTERBOOT_OFFSET	0x0000
#define RB_ROUTERBOOT_SIZE	0xb000
#define RB_HARD_CFG_OFFSET	0xb000
#define RB_HARD_CFG_SIZE	0x1000
#define RB_BIOS_OFFSET		0xd000
#define RB_BIOS_SIZE		0x2000
#define RB_SOFT_CFG_OFFSET	0xf000
#define RB_SOFT_CFG_SIZE	0x1000

#define RB_ART_SIZE		0x10000

static struct mtd_partition rb2011_spi_partitions[] = {
	{
		.name		= "routerboot",
		.offset		= RB_ROUTERBOOT_OFFSET,
		.size		= RB_ROUTERBOOT_SIZE,
		.mask_flags	= MTD_WRITEABLE,
	}, {
		.name		= "hard_config",
		.offset		= RB_HARD_CFG_OFFSET,
		.size		= RB_HARD_CFG_SIZE,
		.mask_flags	= MTD_WRITEABLE,
	}, {
		.name		= "bios",
		.offset		= RB_BIOS_OFFSET,
		.size		= RB_BIOS_SIZE,
		.mask_flags	= MTD_WRITEABLE,
	}, {
		.name		= "soft_config",
		.offset		= RB_SOFT_CFG_OFFSET,
		.size		= RB_SOFT_CFG_SIZE,
	}
};

static struct mtd_partition rb2011_nand_partitions[] = {
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

static struct flash_platform_data rb2011_spi_flash_data = {
	.parts		= rb2011_spi_partitions,
	.nr_parts	= ARRAY_SIZE(rb2011_spi_partitions),
};

static struct ar8327_pad_cfg rb2011_ar8327_pad0_cfg = {
	.mode = AR8327_PAD_MAC_RGMII,
	.txclk_delay_en = true,
	.rxclk_delay_en = true,
	.txclk_delay_sel = AR8327_CLK_DELAY_SEL1,
	.rxclk_delay_sel = AR8327_CLK_DELAY_SEL2,
};

static struct ar8327_platform_data rb2011_ar8327_data = {
	.pad0_cfg = &rb2011_ar8327_pad0_cfg,
	.cpuport_cfg = {
		.force_link = 1,
		.speed = AR8327_PORT_SPEED_1000,
		.duplex = 1,
		.txpause = 1,
		.rxpause = 1,
	}
};

static struct mdio_board_info rb2011_mdio0_info[] = {
	{
		.bus_id = "ag71xx-mdio.0",
		.phy_addr = 0,
		.platform_data = &rb2011_ar8327_data,
	},
};

static void __init rb2011_wlan_init(void)
{
	u8 *hard_cfg = (u8 *) KSEG1ADDR(0x1f000000 + RB_HARD_CFG_OFFSET);
	u16 tag_len;
	u8 *tag;
	char *art_buf;
	u8 wlan_mac[ETH_ALEN];
	int err;

	err = routerboot_find_tag(hard_cfg, RB_HARD_CFG_SIZE, RB_ID_WLAN_DATA,
				  &tag, &tag_len);
	if (err) {
		pr_err("no calibration data found\n");
		return;
	}

	art_buf = kmalloc(RB_ART_SIZE, GFP_KERNEL);
	if (art_buf == NULL) {
		pr_err("no memory for calibration data\n");
		return;
	}

	err = rle_decode((char *) tag, tag_len, art_buf, RB_ART_SIZE,
			 NULL, NULL);
	if (err) {
		pr_err("unable to decode calibration data\n");
		goto free;
	}

	ath79_init_mac(wlan_mac, ath79_mac_base, 11);
	ath79_register_wmac(art_buf + 0x1000, wlan_mac);

free:
	kfree(art_buf);
}

static void rb2011_nand_select_chip(int chip_no)
{
	switch (chip_no) {
	case 0:
		gpio_set_value(RB2011_GPIO_NAND_NCE, 0);
		break;
	default:
		gpio_set_value(RB2011_GPIO_NAND_NCE, 1);
		break;
	}
	ndelay(500);
}

static struct nand_ecclayout rb2011_nand_ecclayout = {
	.eccbytes	= 6,
	.eccpos		= { 8, 9, 10, 13, 14, 15 },
	.oobavail	= 9,
	.oobfree	= { { 0, 4 }, { 6, 2 }, { 11, 2 }, { 4, 1 } }
};

static int rb2011_nand_scan_fixup(struct mtd_info *mtd)
{
	struct nand_chip *chip = mtd->priv;

	if (mtd->writesize == 512) {
		/*
		 * Use the OLD Yaffs-1 OOB layout, otherwise RouterBoot
		 * will not be able to find the kernel that we load.
		 */
		chip->ecc.layout = &rb2011_nand_ecclayout;
	}

	return 0;
}

static void __init rb2011_nand_init(void)
{
	ath79_nfc_set_scan_fixup(rb2011_nand_scan_fixup);
	ath79_nfc_set_parts(rb2011_nand_partitions,
			    ARRAY_SIZE(rb2011_nand_partitions));
	ath79_nfc_set_select_chip(rb2011_nand_select_chip);
	ath79_nfc_set_swap_dma(true);
	ath79_register_nfc();
}

static void __init rb2011_gpio_init(void)
{
	gpio_request_one(RB2011_GPIO_NAND_NCE, GPIOF_OUT_INIT_HIGH, "NAND nCE");
}

static void __init rb2011_setup(void)
{
	rb2011_gpio_init();

	ath79_register_m25p80(&rb2011_spi_flash_data);
	rb2011_nand_init();

	ath79_setup_ar934x_eth_cfg(AR934X_ETH_CFG_RGMII_GMAC0 |
				   AR934X_ETH_CFG_SW_ONLY_MODE);

	ath79_register_mdio(1, 0x0);
	ath79_register_mdio(0, 0x0);

	mdiobus_register_board_info(rb2011_mdio0_info,
				    ARRAY_SIZE(rb2011_mdio0_info));

	/* GMAC0 is connected to an ar8327 switch */
	ath79_init_mac(ath79_eth0_data.mac_addr, ath79_mac_base, 0);
	ath79_eth0_data.phy_if_mode = PHY_INTERFACE_MODE_RGMII;
	ath79_eth0_data.phy_mask = BIT(0);
	ath79_eth0_data.mii_bus_dev = &ath79_mdio0_device.dev;
	ath79_eth0_pll_data.pll_1000 = 0x06000000;

	ath79_register_eth(0);

	/* GMAC1 is connected to the internal switch */
	ath79_init_mac(ath79_eth1_data.mac_addr, ath79_mac_base, 5);
	ath79_eth1_data.phy_if_mode = PHY_INTERFACE_MODE_GMII;
	ath79_eth1_data.speed = SPEED_1000;
	ath79_eth1_data.duplex = DUPLEX_FULL;

	ath79_register_eth(1);
}

MIPS_MACHINE(ATH79_MACH_RB_2011L, "2011L", "MikroTik RouterBOARD 2011L",
	     rb2011_setup);

static void __init rb2011g_setup(void)
{
	rb2011_setup();
	rb2011_wlan_init();
}

MIPS_MACHINE(ATH79_MACH_RB_2011G, "2011G", "MikroTik RouterBOARD 2011UAS-2HnD",
	     rb2011g_setup);

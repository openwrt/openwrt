/*
 *  MikroTik RouterBOARD 2011 support
 *
 *  Copyright (C) 2012 Stijn Tintel <stijn@linux-ipv6.be>
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 */

#include <linux/phy.h>
#include <linux/platform_device.h>
#include <linux/ar8216_platform.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/partitions.h>
#include <linux/spi/spi.h>
#include <linux/spi/flash.h>

#include <asm/mach-ath79/ar71xx_regs.h>

#include "common.h"
#include "dev-eth.h"
#include "dev-m25p80.h"
#include "machtypes.h"

#define RB_ROUTERBOOT_OFFSET	0x0000
#define RB_ROUTERBOOT_SIZE	0xb000
#define RB_HARD_CFG_OFFSET	0xb000
#define RB_HARD_CFG_SIZE	0x1000
#define RB_BIOS_OFFSET		0xd000
#define RB_BIOS_SIZE		0x2000
#define RB_SOFT_CFG_OFFSET	0xf000
#define RB_SOFT_CFG_SIZE	0x1000

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

static void __init rb2011_gmac_setup(void)
{
	void __iomem *base;
	u32 t;

	base = ioremap(AR934X_GMAC_BASE, AR934X_GMAC_SIZE);

	t = __raw_readl(base + AR934X_GMAC_REG_ETH_CFG);
	t &= ~(AR934X_ETH_CFG_RGMII_GMAC0 | AR934X_ETH_CFG_MII_GMAC0 |
		AR934X_ETH_CFG_GMII_GMAC0 | AR934X_ETH_CFG_SW_ONLY_MODE);
	t |= AR934X_ETH_CFG_RGMII_GMAC0 | AR934X_ETH_CFG_SW_ONLY_MODE;

	__raw_writel(t, base + AR934X_GMAC_REG_ETH_CFG);

	iounmap(base);
}

static void __init rb2011_setup(void)
{
	ath79_register_m25p80(&rb2011_spi_flash_data);

	rb2011_gmac_setup();

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

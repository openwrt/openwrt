/*
 *  Atheros DB120 board (WASP SoC) support
 *
 *  Copyright (C) 2010-2011 Jaiganesh Narayanan <jnarayanan@atheros.com>
 *  Copyright (C) 2011 Gabor Juhos <juhosg@openwrt.org>
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 */

#include <linux/mtd/mtd.h>
#include <linux/mtd/partitions.h>
#include <linux/platform_device.h>

#include <asm/mach-ar71xx/ar71xx.h>

#include "machtype.h"
#include "devices.h"
#include "dev-m25p80.h"
#include "dev-gpio-buttons.h"
#include "dev-leds-gpio.h"
#include "dev-usb.h"
#include "dev-ar9xxx-wmac.h"
#include "dev-db120-pci.h"

#define DB120_GPIO_LED_USB	11
#define DB120_GPIO_LED_WLAN_5G	12
#define DB120_GPIO_LED_WLAN_2G	13
#define DB120_GPIO_LED_STATUS	14
#define DB120_GPIO_LED_WPS	15

#define DB120_GPIO_BTN_WPS	16

#define DB120_MAC0_OFFSET	0
#define DB120_MAC1_OFFSET	6
#define DB120_WMAC_CALDATA_OFFSET 0x1000
#define DB120_PCIE_CALDATA_OFFSET 0x5000

#define DB120_KEYS_POLL_INTERVAL	20	/* msecs */
#define DB120_KEYS_DEBOUNCE_INTERVAL	(3 * DB120_KEYS_POLL_INTERVAL)

#ifdef CONFIG_MTD_PARTITIONS
static struct mtd_partition db120_partitions[] = {
	{
		.name		= "u-boot",
		.offset		= 0,
		.size		= 0x040000,
		.mask_flags	= MTD_WRITEABLE,
	}, {
		.name		= "u-boot-env",
		.offset		= 0x040000,
		.size		= 0x010000,
	}, {
		.name		= "rootfs",
		.offset		= 0x050000,
		.size		= 0x630000,
	}, {
		.name		= "uImage",
		.offset		= 0x680000,
		.size		= 0x160000,
	}, {
		.name		= "NVRAM",
		.offset		= 0x7E0000,
		.size		= 0x010000,
	}, {
		.name		= "ART",
		.offset		= 0x7F0000,
		.size		= 0x010000,
		.mask_flags	= MTD_WRITEABLE,
	}
};
#endif /* CONFIG_MTD_PARTITIONS */

static struct flash_platform_data db120_flash_data = {
#ifdef CONFIG_MTD_PARTITIONS
	.parts		= db120_partitions,
	.nr_parts	= ARRAY_SIZE(db120_partitions),
#endif
};

static struct gpio_led db120_leds_gpio[] __initdata = {
	{
		.name		= "db120:green:status",
		.gpio		= DB120_GPIO_LED_STATUS,
		.active_low	= 1,
	}, {
		.name		= "db120:green:wps",
		.gpio		= DB120_GPIO_LED_WPS,
		.active_low	= 1,
	}, {
		.name		= "db120:green:wlan-5g",
		.gpio		= DB120_GPIO_LED_WLAN_5G,
		.active_low	= 1,
	}, {
		.name		= "db120:green:wlan-2g",
		.gpio		= DB120_GPIO_LED_WLAN_2G,
		.active_low	= 1,
	}, {
		.name		= "db120:green:usb",
		.gpio		= DB120_GPIO_LED_USB,
		.active_low	= 1,
	}
};

static struct gpio_keys_button db120_gpio_keys[] __initdata = {
	{
		.desc		= "WPS button",
		.type		= EV_KEY,
		.code		= KEY_WPS_BUTTON,
		.debounce_interval = DB120_KEYS_DEBOUNCE_INTERVAL,
		.gpio		= DB120_GPIO_BTN_WPS,
		.active_low	= 1,
	}
};

static void __init db120_gmac_setup(void)
{
	void __iomem *base;
	u32 t;

	base = ioremap(AR934X_GMAC_BASE, AR934X_GMAC_SIZE);

	t = __raw_readl(base + AR934X_GMAC_REG_ETH_CFG);
	t &= ~(AR934X_ETH_CFG_RGMII_GMAC0 | AR934X_ETH_CFG_MII_GMAC0 |
	       AR934X_ETH_CFG_MII_GMAC0 | AR934X_ETH_CFG_SW_ONLY_MODE);
	__raw_writel(t, base + AR934X_GMAC_REG_ETH_CFG);

	iounmap(base);
}

static void __init db120_setup(void)
{
	u8 *art = (u8 *) KSEG1ADDR(0x1fff0000);

	ar71xx_gpio_output_select(DB120_GPIO_LED_USB, AR934X_GPIO_OUT_GPIO);

	ar71xx_add_device_usb();

	ar71xx_add_device_m25p80(&db120_flash_data);

	ar71xx_add_device_leds_gpio(-1, ARRAY_SIZE(db120_leds_gpio),
					db120_leds_gpio);

	ar71xx_register_gpio_keys_polled(-1, DB120_KEYS_POLL_INTERVAL,
					 ARRAY_SIZE(db120_gpio_keys),
					 db120_gpio_keys);

	db120_gmac_setup();

	ar71xx_add_device_mdio(0, 0x0);
	ar71xx_add_device_mdio(1, 0x0);

	ar71xx_init_mac(ar71xx_eth0_data.mac_addr, art + DB120_MAC0_OFFSET, 0);
#if 0
	/* GMAC0 is connected to an AR8327 switch */
	ar71xx_eth0_data.phy_if_mode = PHY_INTERFACE_MODE_RGMII;
	ar71xx_eth0_data.speed = SPEED_1000;
	ar71xx_eth0_data.duplex = DUPLEX_FULL;
#else
	/* GMAC0 is connected to PHY4 of the internal switch */
	ar71xx_switch_data.phy4_mii_en = 1;

	ar71xx_eth0_data.phy_if_mode = PHY_INTERFACE_MODE_MII;
	ar71xx_eth0_data.phy_mask = BIT(4);
	ar71xx_eth0_data.mii_bus_dev = &ar71xx_mdio1_device.dev;
#endif

	ar71xx_add_device_eth(0);

	/* GMAC1 is connected to the internal switch */
	ar71xx_init_mac(ar71xx_eth1_data.mac_addr, art + DB120_MAC1_OFFSET, 0);
	ar71xx_eth1_data.phy_if_mode = PHY_INTERFACE_MODE_GMII;
	ar71xx_eth1_data.speed = SPEED_1000;
	ar71xx_eth1_data.duplex = DUPLEX_FULL;

	ar71xx_add_device_eth(1);

	ar9xxx_add_device_wmac(art + DB120_WMAC_CALDATA_OFFSET, NULL);

	db120_pci_init(art + DB120_PCIE_CALDATA_OFFSET, NULL);
}

MIPS_MACHINE(AR71XX_MACH_DB120, "DB120", "Atheros DB120", db120_setup);

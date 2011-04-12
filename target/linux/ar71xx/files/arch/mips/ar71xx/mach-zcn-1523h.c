/*
 *  Zcomax ZCN-1523H-2-8/5-16 board support
 *
 *  Copyright (C) 2010 Gabor Juhos <juhosg@openwrt.org>
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
#include "dev-gpio-buttons.h"
#include "dev-leds-gpio.h"

#define ZCN_1523H_GPIO_BTN_RESET	0
#define ZCN_1523H_GPIO_LED_INIT		11
#define ZCN_1523H_GPIO_LED_LAN1		17

#define ZCN_1523H_2_GPIO_LED_WEAK	13
#define ZCN_1523H_2_GPIO_LED_MEDIUM	14
#define ZCN_1523H_2_GPIO_LED_STRONG	15

#define ZCN_1523H_5_GPIO_LED_UNKNOWN	1
#define ZCN_1523H_5_GPIO_LED_LAN2	13
#define ZCN_1523H_5_GPIO_LED_WEAK	14
#define ZCN_1523H_5_GPIO_LED_MEDIUM	15
#define ZCN_1523H_5_GPIO_LED_STRONG	16

#define ZCN_1523H_KEYS_POLL_INTERVAL	20	/* msecs */
#define ZCN_1523H_KEYS_DEBOUNCE_INTERVAL (3 * ZCN_1523H_KEYS_POLL_INTERVAL)

#ifdef CONFIG_MTD_PARTITIONS
static struct mtd_partition zcn_1523h_partitions[] = {
	{
		.name		= "u-boot",
		.offset		= 0,
		.size		= 0x040000,
		.mask_flags	= MTD_WRITEABLE,
	}, {
		.name		= "u-boot-env",
		.offset		= 0x040000,
		.size		= 0x010000,
		.mask_flags	= MTD_WRITEABLE,
	}, {
		.name		= "rootfs",
		.offset		= 0x050000,
		.size		= 0x610000,
	}, {
		.name		= "kernel",
		.offset		= 0x660000,
		.size		= 0x170000,
	}, {
		.name		= "configure",
		.offset		= 0x7d0000,
		.size		= 0x010000,
		.mask_flags	= MTD_WRITEABLE,
	}, {
		.name		= "mfg",
		.offset		= 0x7e0000,
		.size		= 0x010000,
		.mask_flags	= MTD_WRITEABLE,
	}, {
		.name		= "eeprom",
		.offset		= 0x7f0000,
		.size		= 0x010000,
		.mask_flags	= MTD_WRITEABLE,
	}, {
		.name		= "firmware",
		.offset		= 0x050000,
		.size		= 0x780000,
	}
};
#endif /* CONFIG_MTD_PARTITIONS */

static struct flash_platform_data zcn_1523h_flash_data = {
#ifdef CONFIG_MTD_PARTITIONS
	.parts		= zcn_1523h_partitions,
	.nr_parts	= ARRAY_SIZE(zcn_1523h_partitions),
#endif
};

static struct gpio_keys_button zcn_1523h_gpio_keys[] __initdata = {
	{
		.desc		= "reset",
		.type		= EV_KEY,
		.code		= KEY_RESTART,
		.debounce_interval = ZCN_1523H_KEYS_DEBOUNCE_INTERVAL,
		.gpio		= ZCN_1523H_GPIO_BTN_RESET,
		.active_low	= 1,
	}
};

static struct gpio_led zcn_1523h_leds_gpio[] __initdata = {
	{
		.name		= "zcn-1523h:amber:init",
		.gpio		= ZCN_1523H_GPIO_LED_INIT,
		.active_low	= 1,
	}, {
		.name		= "zcn-1523h:green:lan1",
		.gpio		= ZCN_1523H_GPIO_LED_LAN1,
		.active_low	= 1,
	}
};

static struct gpio_led zcn_1523h_2_leds_gpio[] __initdata = {
	{
		.name		= "zcn-1523h:red:weak",
		.gpio		= ZCN_1523H_2_GPIO_LED_WEAK,
		.active_low	= 1,
	}, {
		.name		= "zcn-1523h:amber:medium",
		.gpio		= ZCN_1523H_2_GPIO_LED_MEDIUM,
		.active_low	= 1,
	}, {
		.name		= "zcn-1523h:green:strong",
		.gpio		= ZCN_1523H_2_GPIO_LED_STRONG,
		.active_low	= 1,
	}
};

static struct gpio_led zcn_1523h_5_leds_gpio[] __initdata = {
	{
		.name		= "zcn-1523h:red:weak",
		.gpio		= ZCN_1523H_5_GPIO_LED_WEAK,
		.active_low	= 1,
	}, {
		.name		= "zcn-1523h:amber:medium",
		.gpio		= ZCN_1523H_5_GPIO_LED_MEDIUM,
		.active_low	= 1,
	}, {
		.name		= "zcn-1523h:green:strong",
		.gpio		= ZCN_1523H_5_GPIO_LED_STRONG,
		.active_low	= 1,
	}, {
		.name		= "zcn-1523h:green:lan2",
		.gpio		= ZCN_1523H_5_GPIO_LED_LAN2,
		.active_low	= 1,
	}, {
		.name		= "zcn-1523h:amber:unknown",
		.gpio		= ZCN_1523H_5_GPIO_LED_UNKNOWN,
	}
};

static void __init zcn_1523h_generic_setup(void)
{
	u8 *mac = (u8 *) KSEG1ADDR(0x1f7e0004);
	u8 *ee = (u8 *) KSEG1ADDR(0x1fff1000);

	ar71xx_gpio_function_disable(AR724X_GPIO_FUNC_ETH_SWITCH_LED0_EN |
				     AR724X_GPIO_FUNC_ETH_SWITCH_LED1_EN |
				     AR724X_GPIO_FUNC_ETH_SWITCH_LED2_EN |
				     AR724X_GPIO_FUNC_ETH_SWITCH_LED3_EN |
				     AR724X_GPIO_FUNC_ETH_SWITCH_LED4_EN);

	ar71xx_add_device_m25p80(&zcn_1523h_flash_data);

	ar71xx_add_device_leds_gpio(0, ARRAY_SIZE(zcn_1523h_leds_gpio),
					zcn_1523h_leds_gpio);

	ar71xx_register_gpio_keys_polled(-1, ZCN_1523H_KEYS_POLL_INTERVAL,
					 ARRAY_SIZE(zcn_1523h_gpio_keys),
					 zcn_1523h_gpio_keys);

	ap91_pci_init(ee, mac);

	ar71xx_init_mac(ar71xx_eth0_data.mac_addr, mac, 0);
	ar71xx_init_mac(ar71xx_eth1_data.mac_addr, mac, 1);

	/* LAN1 port */
	ar71xx_eth0_data.phy_if_mode = PHY_INTERFACE_MODE_RMII;
	ar71xx_eth0_data.speed = SPEED_100;
	ar71xx_eth0_data.duplex = DUPLEX_FULL;

	/* LAN2 port */
	ar71xx_eth1_data.phy_if_mode = PHY_INTERFACE_MODE_RMII;
	ar71xx_eth1_data.speed = SPEED_1000;
	ar71xx_eth1_data.duplex = DUPLEX_FULL;

	ar71xx_add_device_mdio(0x0);
	ar71xx_add_device_eth(0);
}

static void __init zcn_1523h_2_setup(void)
{
	zcn_1523h_generic_setup();

	ar71xx_add_device_leds_gpio(1, ARRAY_SIZE(zcn_1523h_2_leds_gpio),
					zcn_1523h_2_leds_gpio);
}

MIPS_MACHINE(AR71XX_MACH_ZCN_1523H_2, "ZCN-1523H-2", "Zcomax ZCN-1523H-2",
	     zcn_1523h_2_setup);

static void __init zcn_1523h_5_setup(void)
{
	zcn_1523h_generic_setup();

	ar71xx_add_device_leds_gpio(1, ARRAY_SIZE(zcn_1523h_5_leds_gpio),
					zcn_1523h_5_leds_gpio);
	ar71xx_add_device_eth(1);
}

MIPS_MACHINE(AR71XX_MACH_ZCN_1523H_5, "ZCN-1523H-5", "Zcomax ZCN-1523H-5",
	     zcn_1523h_5_setup);

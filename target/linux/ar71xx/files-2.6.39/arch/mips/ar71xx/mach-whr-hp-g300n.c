/*
 *  Buffalo WHR-HP-G300N board support
 *
 *  based on ...
 *
 *  TP-LINK TL-WR741ND board support
 *
 *  Copyright (C) 2009-2010 Gabor Juhos <juhosg@openwrt.org>
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

#define	WHRHPG300N_GPIO_LED_SECURITY		0
#define	WHRHPG300N_GPIO_LED_DIAG		1
#define	WHRHPG300N_GPIO_LED_ROUTER		6

#define	WHRHPG300N_GPIO_BTN_ROUTER_ON		7
#define	WHRHPG300N_GPIO_BTN_ROUTER_AUTO		8
#define	WHRHPG300N_GPIO_BTN_RESET		11
#define	WHRHPG300N_GPIO_BTN_AOSS		12

#define	WHRHPG300N_KEYS_POLL_INTERVAL	20	/* msecs */
#define WHRHPG300N_KEYS_DEBOUNCE_INTERVAL (3 * WHRHPG300N_KEYS_POLL_INTERVAL)

#define WHRHPG300N_MAC_OFFSET		0x20c

#ifdef CONFIG_MTD_PARTITIONS
static struct mtd_partition whrhpg300n_partitions[] = {
	{
		.name		= "u-boot",
		.offset		= 0,
		.size		= 0x03e000,
		.mask_flags	= MTD_WRITEABLE,
	}, {
		.name		= "u-boot-env",
		.offset		= 0x03e000,
		.size		= 0x002000,
		.mask_flags	= MTD_WRITEABLE,
	}, {
		.name		= "kernel",
		.offset		= 0x040000,
		.size		= 0x0e0000,
	}, {
		.name		= "rootfs",
		.offset		= 0x120000,
		.size		= 0x2c0000,
	}, {
		.name		= "user_property",
		.offset		= 0x3e0000,
		.size		= 0x010000,
	}, {
		.name		= "ART",
		.offset		= 0x3f0000,
		.size		= 0x010000,
		.mask_flags	= MTD_WRITEABLE,
	}, {
		.name		= "firmware",
		.offset		= 0x040000,
		.size		= 0x3a0000,
	}
};
#endif /* CONFIG_MTD_PARTITIONS */

static struct flash_platform_data whrhpg300n_flash_data = {
#ifdef CONFIG_MTD_PARTITIONS
	.parts		= whrhpg300n_partitions,
	.nr_parts	= ARRAY_SIZE(whrhpg300n_partitions),
#endif
};

static struct gpio_led whrhpg300n_leds_gpio[] __initdata = {
	{
		.name		= "buffalo:orange:security",
		.gpio		= WHRHPG300N_GPIO_LED_SECURITY,
		.active_low	= 1,
	}, {
		.name		= "buffalo:red:diag",
		.gpio		= WHRHPG300N_GPIO_LED_DIAG,
		.active_low	= 1,
	}, {
		.name		= "buffalo:green:router",
		.gpio		= WHRHPG300N_GPIO_LED_ROUTER,
		.active_low	= 1,
	}
};

static struct gpio_keys_button whrhpg300n_gpio_keys[] __initdata = {
	{
		.desc		= "reset",
		.type		= EV_KEY,
		.code		= KEY_RESTART,
		.debounce_interval = WHRHPG300N_KEYS_DEBOUNCE_INTERVAL,
		.gpio		= WHRHPG300N_GPIO_BTN_RESET,
		.active_low	= 1,
	}, {
		.desc		= "aoss/wps",
		.type		= EV_KEY,
		.code		= KEY_WPS_BUTTON,
		.gpio		= WHRHPG300N_GPIO_BTN_AOSS,
		.debounce_interval = WHRHPG300N_KEYS_DEBOUNCE_INTERVAL,
		.active_low	= 1,
	}, {
		.desc		= "router_on",
		.type		= EV_KEY,
		.code		= BTN_2,
		.gpio		= WHRHPG300N_GPIO_BTN_ROUTER_ON,
		.debounce_interval = WHRHPG300N_KEYS_DEBOUNCE_INTERVAL,
		.active_low	= 1,
	}, {
		.desc		= "router_auto",
		.type		= EV_KEY,
		.code		= BTN_3,
		.gpio		= WHRHPG300N_GPIO_BTN_ROUTER_AUTO,
		.debounce_interval = WHRHPG300N_KEYS_DEBOUNCE_INTERVAL,
		.active_low	= 1,
	}
};

static void __init whrhpg300n_setup(void)
{
	u8 *ee = (u8 *) KSEG1ADDR(0x1fff1000);
	u8 *mac = (u8 *) KSEG1ADDR(ee + WHRHPG300N_MAC_OFFSET);

	ar71xx_add_device_m25p80(&whrhpg300n_flash_data);

	ar71xx_add_device_leds_gpio(-1, ARRAY_SIZE(whrhpg300n_leds_gpio),
					whrhpg300n_leds_gpio);

	ar71xx_register_gpio_keys_polled(-1, WHRHPG300N_KEYS_POLL_INTERVAL,
					 ARRAY_SIZE(whrhpg300n_gpio_keys),
					 whrhpg300n_gpio_keys);

	ar71xx_init_mac(ar71xx_eth0_data.mac_addr, mac, 0);
	ar71xx_init_mac(ar71xx_eth1_data.mac_addr, mac, 1);

	ar71xx_add_device_mdio(0, 0x0);

	/* LAN ports */
	ar71xx_add_device_eth(1);
	/* WAN port */
	ar71xx_add_device_eth(0);

	ap91_pci_setup_wmac_led_pin(1);

	ap91_pci_init(ee, mac);
}

MIPS_MACHINE(AR71XX_MACH_WHR_HP_G300N, "WHR-HP-G300N", "Buffalo WHR-HP-G300N",
	     whrhpg300n_setup);

MIPS_MACHINE(AR71XX_MACH_WHR_G301N, "WHR-G301N", "Buffalo WHR-G301N",
	     whrhpg300n_setup);

MIPS_MACHINE(AR71XX_MACH_WHR_HP_GN, "WHR-HP-GN", "Buffalo WHR-HP-GN",
	     whrhpg300n_setup);

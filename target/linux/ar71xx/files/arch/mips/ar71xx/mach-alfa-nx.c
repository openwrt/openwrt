/*
 *  ALFA Network N2/N5 board support
 *
 *  Copyright (C) 2011 Gabor Juhos <juhosg@openwrt.org>
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

#define ALFA_NX_GPIO_LED_2		17
#define ALFA_NX_GPIO_LED_3		16
#define ALFA_NX_GPIO_LED_5		12
#define ALFA_NX_GPIO_LED_6		8
#define ALFA_NX_GPIO_LED_7		6
#define ALFA_NX_GPIO_LED_8		7

#define ALFA_NX_GPIO_BTN_RESET		11

#define ALFA_NX_KEYS_POLL_INTERVAL	20	/* msecs */
#define ALFA_NX_KEYS_DEBOUNCE_INTERVAL (3 * ALFA_NX_KEYS_POLL_INTERVAL)

#define ALFA_NX_MAC0_OFFSET		0
#define ALFA_NX_MAC1_OFFSET		6
#define ALFA_NX_CALDATA_OFFSET		0x1000

#ifdef CONFIG_MTD_PARTITIONS
static struct mtd_partition alfa_nx_partitions[] = {
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
		.size		= 0x600000,
	}, {
		.name		= "kernel",
		.offset		= 0x650000,
		.size		= 0x190000,
	}, {
		.name		= "nvram",
		.offset		= 0x7e0000,
		.size		= 0x010000,
		.mask_flags	= MTD_WRITEABLE,
	}, {
		.name		= "art",
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

static struct flash_platform_data alfa_nx_flash_data = {
#ifdef CONFIG_MTD_PARTITIONS
	.parts		= alfa_nx_partitions,
	.nr_parts	= ARRAY_SIZE(alfa_nx_partitions),
#endif
};

static struct gpio_keys_button alfa_nx_gpio_keys[] __initdata = {
	{
		.desc		= "Reset button",
		.type		= EV_KEY,
		.code		= KEY_RESTART,
		.debounce_interval = ALFA_NX_KEYS_DEBOUNCE_INTERVAL,
		.gpio		= ALFA_NX_GPIO_BTN_RESET,
		.active_low	= 1,
	}
};

static struct gpio_led alfa_nx_leds_gpio[] __initdata = {
	{
		.name		= "alfa:green:led_2",
		.gpio		= ALFA_NX_GPIO_LED_2,
		.active_low	= 1,
	}, {
		.name		= "alfa:green:led_3",
		.gpio		= ALFA_NX_GPIO_LED_3,
		.active_low	= 1,
	}, {
		.name		= "alfa:red:led_5",
		.gpio		= ALFA_NX_GPIO_LED_5,
		.active_low	= 1,
	}, {
		.name		= "alfa:amber:led_6",
		.gpio		= ALFA_NX_GPIO_LED_6,
		.active_low	= 1,
	}, {
		.name		= "alfa:green:led_7",
		.gpio		= ALFA_NX_GPIO_LED_7,
		.active_low	= 1,
	}, {
		.name		= "alfa:green:led_8",
		.gpio		= ALFA_NX_GPIO_LED_8,
		.active_low	= 1,
	}
};

static void __init alfa_nx_setup(void)
{
	u8 *art = (u8 *) KSEG1ADDR(0x1fff0000);

	ar71xx_gpio_function_setup(AR724X_GPIO_FUNC_JTAG_DISABLE,
				   AR724X_GPIO_FUNC_ETH_SWITCH_LED0_EN |
				   AR724X_GPIO_FUNC_ETH_SWITCH_LED1_EN |
				   AR724X_GPIO_FUNC_ETH_SWITCH_LED2_EN |
				   AR724X_GPIO_FUNC_ETH_SWITCH_LED3_EN |
				   AR724X_GPIO_FUNC_ETH_SWITCH_LED4_EN);

	ar71xx_add_device_m25p80(&alfa_nx_flash_data);

	ar71xx_add_device_leds_gpio(0, ARRAY_SIZE(alfa_nx_leds_gpio),
					alfa_nx_leds_gpio);

	ar71xx_register_gpio_keys_polled(-1, ALFA_NX_KEYS_POLL_INTERVAL,
					 ARRAY_SIZE(alfa_nx_gpio_keys),
					 alfa_nx_gpio_keys);

	ar71xx_add_device_mdio(0, 0x0);

	ar71xx_init_mac(ar71xx_eth0_data.mac_addr,
			art + ALFA_NX_MAC0_OFFSET, 0);
	ar71xx_init_mac(ar71xx_eth1_data.mac_addr,
			art + ALFA_NX_MAC1_OFFSET, 0);

	/* WAN port */
	ar71xx_add_device_eth(0);
	/* LAN port */
	ar71xx_add_device_eth(1);

	ap91_pci_init(art + ALFA_NX_CALDATA_OFFSET, NULL);
}

MIPS_MACHINE(AR71XX_MACH_ALFA_NX, "ALFA-NX", "ALFA Network N2/N5",
	     alfa_nx_setup);

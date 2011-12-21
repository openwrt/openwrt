/*
 *  Buffalo WZR-HP-G300NH2 board support
 *
 *  Copyright (C) 2011 Felix Fietkau <nbd@openwrt.org>
 *  Copyright (C) 2011 Mark Deneen <mdeneen@gmail.com>
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 */

#include <linux/platform_device.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/partitions.h>

#include <asm/mips_machine.h>
#include <asm/mach-ar71xx/ar71xx.h>
#include <asm/mach-ar71xx/gpio.h>

#include "machtype.h"
#include "devices.h"
#include "dev-ap91-pci.h"
#include "dev-gpio-buttons.h"
#include "dev-leds-gpio.h"
#include "dev-m25p80.h"
#include "dev-usb.h"


#define WZRHPG300NH2_MAC_OFFSET		0x20c
#define WZRHPG300NH2_KEYS_POLL_INTERVAL     20      /* msecs */
#define WZRHPG300NH2_KEYS_DEBOUNCE_INTERVAL (3 * WZRHPG300NH2_KEYS_POLL_INTERVAL)

#ifdef CONFIG_MTD_PARTITIONS
static struct mtd_partition wzrhpg300nh2_flash_partitions[] = {
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



static struct flash_platform_data wzrhpg300nh2_flash_data = {
#ifdef CONFIG_MTD_PARTITIONS
	.parts          = wzrhpg300nh2_flash_partitions,
	.nr_parts       = ARRAY_SIZE(wzrhpg300nh2_flash_partitions),
#endif
};


static struct gpio_led wzrhpg300nh2_leds_gpio[] __initdata = {
	{
		.name		= "buffalo:red:diag",
		.gpio		= 16,
		.active_low	= 1,
	},
};

static struct gpio_led wzrhpg300nh2_wmac_leds_gpio[] = {
	{
		.name           = "buffalo:blue:usb",
		.gpio           = 4,
		.active_low     = 1,
	},
	{
		.name           = "buffalo:orange:security",
		.gpio           = 6,
		.active_low     = 1,
	},
	{
		.name           = "buffalo:green:router",
		.gpio           = 7,
		.active_low     = 1,
	},
	{
		.name           = "buffalo:blue:movie_engine_on",
		.gpio           = 8,
		.active_low     = 1,
	},
	{
		.name           = "buffalo:blue:movie_engine_off",
		.gpio           = 9,
		.active_low     = 1,
	},
};

/* The AOSS button is wmac gpio 12 */
static struct gpio_keys_button wzrhpg300nh2_gpio_keys[] __initdata = {
	{
		.desc		= "reset",
		.type		= EV_KEY,
		.code		= KEY_RESTART,
		.debounce_interval = WZRHPG300NH2_KEYS_DEBOUNCE_INTERVAL,
		.gpio		= 1,
		.active_low	= 1,
	}, {
		.desc		= "usb",
		.type		= EV_KEY,
		.code		= BTN_2,
		.debounce_interval = WZRHPG300NH2_KEYS_DEBOUNCE_INTERVAL,
		.gpio		= 7,
		.active_low	= 1,
	}, {
		.desc		= "qos",
		.type		= EV_KEY,
		.code		= BTN_3,
		.debounce_interval = WZRHPG300NH2_KEYS_DEBOUNCE_INTERVAL,
		.gpio		= 11,
		.active_low	= 0,
	}, {
		.desc		= "router_on",
		.type		= EV_KEY,
		.code		= BTN_5,
		.debounce_interval = WZRHPG300NH2_KEYS_DEBOUNCE_INTERVAL,
		.gpio		= 8,
		.active_low	= 0,
	},
};


static void __init wzrhpg300nh2_setup(void)
{

	u8 *eeprom = (u8 *)   KSEG1ADDR(0x1f051000);
	u8 *mac0   = eeprom + WZRHPG300NH2_MAC_OFFSET;
	/* There is an eth1 but it is not connected to the switch */

	ar71xx_add_device_m25p80_multi(&wzrhpg300nh2_flash_data);

	ar71xx_init_mac(ar71xx_eth0_data.mac_addr, mac0, 0);
	ar71xx_add_device_mdio(0, ~(BIT(0)));

	ar71xx_init_mac(ar71xx_eth0_data.mac_addr, mac0, 0);
	ar71xx_eth0_data.phy_if_mode = PHY_INTERFACE_MODE_RGMII;
	ar71xx_eth0_data.speed = SPEED_1000;
	ar71xx_eth0_data.duplex = DUPLEX_FULL;
	ar71xx_eth0_data.phy_mask = BIT(0);

	ar71xx_add_device_eth(0);
	ar71xx_add_device_usb();
	/* gpio13 is usb power.  Turn it on. */
	gpio_request(13, "usb");
	gpio_direction_output(13, 1);

	ar71xx_add_device_leds_gpio(-1, ARRAY_SIZE(wzrhpg300nh2_leds_gpio),
					wzrhpg300nh2_leds_gpio);
	ar71xx_register_gpio_keys_polled(-1, WZRHPG300NH2_KEYS_POLL_INTERVAL,
					ARRAY_SIZE(wzrhpg300nh2_gpio_keys),
					wzrhpg300nh2_gpio_keys);
	ap91_pci_setup_wmac_led_pin(5);
	ap91_pci_setup_wmac_leds(wzrhpg300nh2_wmac_leds_gpio,
				ARRAY_SIZE(wzrhpg300nh2_wmac_leds_gpio));

	ap91_pci_init(eeprom, mac0);
}

MIPS_MACHINE(AR71XX_MACH_WZR_HP_G300NH2, "WZR-HP-G300NH2",
	     "Buffalo WZR-HP-G300NH2", wzrhpg300nh2_setup);


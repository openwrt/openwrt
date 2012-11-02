/*
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 *
 *  Copyright (C) 2010 John Crispin <blogic@openwrt.org>
 */

#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/leds.h>
#include <linux/gpio.h>
#include <linux/gpio_buttons.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/partitions.h>
#include <linux/mtd/physmap.h>
#include <linux/input.h>
#include <linux/etherdevice.h>
#include <linux/ath5k_platform.h>
#include <linux/ath9k_platform.h>
#include <linux/pci.h>

#include <lantiq_soc.h>
#include <lantiq_platform.h>
#include <dev-gpio-leds.h>
#include <dev-gpio-buttons.h>

#include "../machtypes.h"
#include "dev-wifi-rt2x00.h"
#include "dev-wifi-athxk.h"
#include "devices.h"
#include "dev-dwc_otg.h"
#include "pci-ath-fixup.h"

static struct mtd_partition arv45xx_brnboot_partitions[] =
{
	{
		.name	= "brn-boot",
		.offset	= 0x0,
		.size	= 0x20000,
	},
	{
		.name	= "config",
		.offset	= 0x20000,
		.size	= 0x30000,
	},
	{
		.name	= "linux",
		.offset	= 0x50000,
		.size	= 0x390000,
	},
	{
		.name	= "reserved", /* 12-byte signature at 0x3efff4 :/ */
		.offset	= 0x3e0000,
		.size	= 0x010000,
	},
	{
		.name	= "eeprom",
		.offset	= 0x3f0000,
		.size	= 0x10000,
	},
};

static struct mtd_partition arv75xx_brnboot_partitions[] =
{
	{
		.name	= "brn-boot",
		.offset	= 0x0,
		.size	= 0x20000,
	},
	{
		.name	= "config",
		.offset	= 0x20000,
		.size	= 0x40000,
	},
	{
		.name	= "linux",
		.offset	= 0x440000,
		.size	= 0x3a0000,
	},
	{
		.name	= "reserved", /* 12-byte signature at 0x7efff4 :/ */
		.offset	= 0x7e0000,
		.size	= 0x010000,
	},
	{
		.name	= "board_config",
		.offset	= 0x7f0000,
		.size	= 0x10000,
	},
};

/*
 * this is generic configuration for all arv based boards, note that it can be
 * rewriten in arv_load_nor()
 */
static struct mtd_partition arv_partitions[] =
{
	{
		.name	= "uboot",
		.offset	= 0x0,
		.size	= 0x20000,
	},
	{
		.name	= "uboot_env",
		.offset	= 0x20000,
		.size	= 0x10000,
	},
	{
		.name	= "linux",
		.offset	= 0x30000,
		.size	= 0x3c0000,
	},
	{
		.name	= "board_config",
		.offset	= 0x3f0000,
		.size	= 0x10000,
	},
};

static struct physmap_flash_data arv45xx_brnboot_flash_data = {
	.nr_parts	= ARRAY_SIZE(arv45xx_brnboot_partitions),
	.parts		= arv45xx_brnboot_partitions,
};

static struct physmap_flash_data arv75xx_brnboot_flash_data = {
	.nr_parts	= ARRAY_SIZE(arv75xx_brnboot_partitions),
	.parts		= arv75xx_brnboot_partitions,
};

static struct physmap_flash_data arv_flash_data = {
	.nr_parts	= ARRAY_SIZE(arv_partitions),
	.parts		= arv_partitions,
};

static struct ltq_pci_data ltq_pci_data = {
	.clock	= PCI_CLOCK_EXT,
	.gpio	= PCI_GNT1 | PCI_REQ1,
	.irq	= {
		[14] = INT_NUM_IM0_IRL0 + 22,
	},
};

static struct ltq_eth_data ltq_eth_data = {
	.mii_mode	= PHY_INTERFACE_MODE_RMII,
};

static struct gpio_led
arv4510pw_gpio_leds[] __initdata = {
	{ .name = "soc:green:foo", .gpio = 4, .active_low = 1, },
};

static struct gpio_led
arv4518pw_gpio_leds[] __initdata = {
	{ .name = "soc:green:power", .gpio = 3, .active_low = 1, .default_trigger = "default-on" },
	{ .name = "soc:green:adsl", .gpio = 4, .active_low = 1, .default_trigger = "default-on" },
	{ .name = "soc:green:internet", .gpio = 5, .active_low = 1, .default_trigger = "default-on" },
	{ .name = "soc:green:wifi", .gpio = 6, .active_low = 1, .default_trigger = "default-on" },
	{ .name = "soc:yellow:wps", .gpio = 7, .active_low = 1, .default_trigger = "default-on" },
	{ .name = "soc:red:fail", .gpio = 8, .active_low = 1, .default_trigger = "default-on" },
	{ .name = "soc:green:usb", .gpio = 19, .active_low = 1, .default_trigger = "default-on" },
	{ .name = "soc:green:voip", .gpio = 100, .active_low = 1, .default_trigger = "default-on" },
	{ .name = "soc:green:fxs1", .gpio = 101, .active_low = 1, .default_trigger = "default-on" },
	{ .name = "soc:green:fxs2", .gpio = 102, .active_low = 1, .default_trigger = "default-on" },
	{ .name = "soc:green:fxo", .gpio = 103, .active_low = 1, .default_trigger = "default-on" },
};

static struct gpio_keys_button
arv4518pw_gpio_keys[] __initdata = {
	{
		.desc		= "wifi",
		.type		= EV_KEY,
		.code		= BTN_0,
		.debounce_interval = LTQ_KEYS_DEBOUNCE_INTERVAL,
		.gpio		= 28,
		.active_low	= 1,
	},
	{
		.desc		= "reset",
		.type		= EV_KEY,
		.code		= BTN_1,
		.debounce_interval = LTQ_KEYS_DEBOUNCE_INTERVAL,
		.gpio		= 30,
		.active_low	= 1,
	},
	{
		.desc		= "wps",
		.type		= EV_KEY,
		.code		= BTN_2,
		.debounce_interval = LTQ_KEYS_DEBOUNCE_INTERVAL,
		.gpio		= 29,
		.active_low	= 1,
	},
};

static struct gpio_led
arv4519pw_gpio_leds[] __initdata = {
	{ .name = "soc:red:power", .gpio = 7, .active_low = 1, },
	{ .name = "soc:green:power", .gpio = 2, .active_low = 1, .default_trigger = "default-on" },
	{ .name = "soc:green:wifi", .gpio = 6, .active_low = 1, },
	{ .name = "soc:green:adsl", .gpio = 4, .active_low = 1, },
	{ .name = "soc:green:internet", .gpio = 5, .active_low = 1, },
	{ .name = "soc:red:internet", .gpio = 8, .active_low = 1, },
	{ .name = "soc:green:voip", .gpio = 100, .active_low = 1, },
	{ .name = "soc:green:phone1", .gpio = 101, .active_low = 1, },
	{ .name = "soc:green:phone2", .gpio = 102, .active_low = 1, },
	{ .name = "soc:green:fxo", .gpio = 103, .active_low = 1, },
	{ .name = "soc:green:usb", .gpio = 19, .active_low = 1, },
	{ .name = "soc:orange:wps", .gpio = 104, .active_low = 1, },
	{ .name = "soc:green:wps", .gpio = 105, .active_low = 1, },
	{ .name = "soc:red:wps", .gpio = 106, .active_low = 1, },

};

static struct gpio_keys_button
arv4519pw_gpio_keys[] __initdata = {
	{
		.desc		= "reset",
		.type		= EV_KEY,
		.code		= BTN_1,
		.debounce_interval = LTQ_KEYS_DEBOUNCE_INTERVAL,
		.gpio		= 30,
		.active_low	= 1,
	},
	{
		.desc		= "wlan",
		.type		= EV_KEY,
		.code		= BTN_2,
		.debounce_interval = LTQ_KEYS_DEBOUNCE_INTERVAL,
		.gpio		= 28,
		.active_low	= 1,
	},
};

static struct gpio_led
arv4520pw_gpio_leds[] __initdata = {
	{ .name = "soc:blue:power", .gpio = 3, .active_low = 1, },
	{ .name = "soc:blue:adsl", .gpio = 4, .active_low = 1, },
	{ .name = "soc:blue:internet", .gpio = 5, .active_low = 1, },
	{ .name = "soc:red:power", .gpio = 6, .active_low = 1, },
	{ .name = "soc:yellow:wps", .gpio = 7, .active_low = 1, },
	{ .name = "soc:red:wps", .gpio = 9, .active_low = 1, },
	{ .name = "soc:blue:voip", .gpio = 100, .active_low = 1, },
	{ .name = "soc:blue:fxs1", .gpio = 101, .active_low = 1, },
	{ .name = "soc:blue:fxs2", .gpio = 102, .active_low = 1, },
	{ .name = "soc:blue:fxo", .gpio = 103, .active_low = 1, },
	{ .name = "soc:blue:voice", .gpio = 104, .active_low = 1, },
	{ .name = "soc:blue:usb", .gpio = 105, .active_low = 1, },
	{ .name = "soc:blue:wifi", .gpio = 106, .active_low = 1, },
};

static struct gpio_led
arv452cpw_gpio_leds[] __initdata = {
	{ .name = "soc:blue:power", .gpio = 3, .active_low = 1, .default_trigger = "default-on" },
	{ .name = "soc:blue:adsl", .gpio = 4, .active_low = 1, .default_trigger = "default-on" },
	{ .name = "soc:blue:isdn", .gpio = 5, .active_low = 1, .default_trigger = "default-on" },
	{ .name = "soc:red:power", .gpio = 6, .active_low = 1, .default_trigger = "default-on" },
	{ .name = "soc:yellow:wps", .gpio = 7, .active_low = 1, .default_trigger = "default-on" },
	{ .name = "soc:red:wps", .gpio = 9, .active_low = 1, .default_trigger = "default-on" },
	{ .name = "soc:blue:fxs1", .gpio = 100, .active_low = 1, .default_trigger = "default-on" },
	{ .name = "soc:blue:fxs2", .gpio = 101, .active_low = 1, .default_trigger = "default-on" },
	{ .name = "soc:blue:wps", .gpio = 102, .active_low = 1, .default_trigger = "default-on" },
	{ .name = "soc:blue:fxo", .gpio = 103, .active_low = 1, .default_trigger = "default-on" },
	{ .name = "soc:blue:voice", .gpio = 104, .active_low = 1, .default_trigger = "default-on" },
	{ .name = "soc:blue:usb", .gpio = 105, .active_low = 1, .default_trigger = "default-on" },
	{ .name = "soc:blue:wifi", .gpio = 106, .active_low = 1, .default_trigger = "default-on" },
	{ .name = "soc:blue:internet", .gpio = 108, .active_low = 1, .default_trigger = "default-on" },
	{ .name = "soc:red:internet", .gpio = 109, .active_low = 1, .default_trigger = "default-on" },
};

static struct gpio_led
arv4525pw_gpio_leds[] __initdata = {
	{ .name = "soc:green:dsl", .gpio = 6, .active_low = 1, .default_trigger = "default-on" },
	{ .name = "soc:green:wifi", .gpio = 8, .active_low = 1, .default_trigger = "default-on" },
	{ .name = "soc:green:online", .gpio = 9, .active_low = 1, .default_trigger = "default-on" },
	{ .name = "soc:green:fxs-internet", .gpio = 5, .active_low = 1, .default_trigger = "default-on" },
	{ .name = "soc:green:fxs-festnetz", .gpio = 4, .active_low = 1, .default_trigger = "default-on" },
};

#define ARV4525PW_PHYRESET	13
#define ARV4525PW_RELAY		31

static struct gpio
arv4525pw_gpios[] __initdata = {
	{ ARV4525PW_PHYRESET,	GPIOF_OUT_INIT_HIGH, "phyreset"	},
	{ ARV4525PW_RELAY,	GPIOF_OUT_INIT_HIGH, "relay"	},
};


static struct gpio_led
arv752dpw22_gpio_leds[] __initdata = {
	{ .name = "soc:blue:power", .gpio = 3, .active_low = 1, .default_trigger = "default-on" },
	{ .name = "soc:red:internet", .gpio = 5, .active_low = 1, .default_trigger = "default-on" },
	{ .name = "soc:red:power", .gpio = 6, .active_low = 1, .default_trigger = "default-on" },
	{ .name = "soc:red:wps", .gpio = 8, .active_low = 1, .default_trigger = "default-on" },
	{ .name = "soc:red:fxo", .gpio = 103, .active_low = 1, .default_trigger = "default-on" },
	{ .name = "soc:red:voice", .gpio = 104, .active_low = 1, .default_trigger = "default-on" },
	{ .name = "soc:green:usb", .gpio = 105, .active_low = 1, .default_trigger = "default-on" },
	{ .name = "soc:green:wifi", .gpio = 106, .active_low = 1, .default_trigger = "default-on" },
	{ .name = "soc:green:wifi1", .gpio = 107, .active_low = 1, .default_trigger = "default-on" },
	{ .name = "soc:blue:wifi", .gpio = 108, .active_low = 1, .default_trigger = "default-on" },
	{ .name = "soc:blue:wifi1", .gpio = 109, .active_low = 1, .default_trigger = "default-on" },
	{ .name = "soc:green:eth1", .gpio = 111, .active_low = 1, .default_trigger = "default-on" },
	{ .name = "soc:green:eth2", .gpio = 112, .active_low = 1, .default_trigger = "default-on" },
	{ .name = "soc:green:eth3", .gpio = 113, .active_low = 1, .default_trigger = "default-on" },
	{ .name = "soc:green:eth4", .gpio = 114, .active_low = 1, .default_trigger = "default-on", },
};

static struct gpio_keys_button
arv752dpw22_gpio_keys[] __initdata = {
	{
		.desc		= "btn0",
		.type		= EV_KEY,
		.code		= BTN_0,
		.debounce_interval = LTQ_KEYS_DEBOUNCE_INTERVAL,
		.gpio		= 12,
		.active_low	= 1,
	},
	{
		.desc		= "btn1",
		.type		= EV_KEY,
		.code		= BTN_1,
		.debounce_interval = LTQ_KEYS_DEBOUNCE_INTERVAL,
		.gpio		= 13,
		.active_low	= 1,
	},
	{
		.desc		= "btn2",
		.type		= EV_KEY,
		.code		= BTN_2,
		.debounce_interval = LTQ_KEYS_DEBOUNCE_INTERVAL,
		.gpio		= 28,
		.active_low	= 1,
	},
};

static struct gpio_led
arv7518pw_gpio_leds[] __initdata = {
	{ .name = "soc:red:power", .gpio = 7, .active_low = 1, },
	{ .name = "soc:green:power", .gpio = 2, .active_low = 1, .default_trigger = "default-on" },
	{ .name = "soc:green:wifi", .gpio = 6, .active_low = 1, },
	{ .name = "soc:green:adsl", .gpio = 4, .active_low = 1, },
	{ .name = "soc:green:internet", .gpio = 5, .active_low = 1, },
	{ .name = "soc:red:internet", .gpio = 8, .active_low = 1, },
	{ .name = "soc:green:voip", .gpio = 100, .active_low = 1, },
	{ .name = "soc:green:phone1", .gpio = 101, .active_low = 1, },
	{ .name = "soc:green:phone2", .gpio = 102, .active_low = 1, },
	{ .name = "soc:orange:fail", .gpio = 103, .active_low = 1, },
	{ .name = "soc:green:usb", .gpio = 19, .active_low = 1, },
	{ .name = "soc:orange:wps", .gpio = 104, .active_low = 1, },
	{ .name = "soc:green:wps", .gpio = 105, .active_low = 1, },
	{ .name = "soc:red:wps", .gpio = 106, .active_low = 1, },

};

static struct gpio_keys_button
arv7518pw_gpio_keys[] __initdata = {
	/*{
		.desc		= "reset",
		.type		= EV_KEY,
		.code		= BTN_1,
		.debounce_interval = LTQ_KEYS_DEBOUNCE_INTERVAL,
		.gpio		= 23,
		.active_low	= 1,
	},*/
	{
		.desc		= "wifi",
		.type		= EV_KEY,
		.code		= BTN_2,
		.debounce_interval = LTQ_KEYS_DEBOUNCE_INTERVAL,
		.gpio		= 25,
		.active_low	= 1,
	},
};

static struct gpio_keys_button
arv7525pw_gpio_keys[] __initdata = {
	{
		.desc		= "restart",
		.type		= EV_KEY,
		.code		= BTN_0,
		.debounce_interval = LTQ_KEYS_DEBOUNCE_INTERVAL,
		.gpio		= 29,
		.active_low	= 1,
	},
};

static void __init
arv_load_nor(unsigned int max)
{
#define UBOOT_MAGIC	0x27051956

	int i;
	int sector = -1;

	if (ltq_brn_boot) {
		if (max == 0x800000)
			ltq_register_nor(&arv75xx_brnboot_flash_data);
		else
			ltq_register_nor(&arv45xx_brnboot_flash_data);
		return;
	}

	for (i = 1; i < 4 && sector < 0; i++) {
		unsigned int uboot_magic;
		memcpy_fromio(&uboot_magic, (void *)KSEG1ADDR(LTQ_FLASH_START) + (i * 0x10000), 4);
		if (uboot_magic == UBOOT_MAGIC)
			sector = i;
	}

	if (sector < 0)
		return;

	arv_partitions[0].size = arv_partitions[1].offset = (sector - 1) * 0x10000;
	arv_partitions[2].offset = arv_partitions[0].size + 0x10000;
	arv_partitions[2].size = max - arv_partitions[2].offset - 0x10000;
	arv_partitions[3].offset = max - 0x10000;
	ltq_register_nor(&arv_flash_data);
}

static void __init
arv_register_ethernet(unsigned int mac_addr)
{
	memcpy_fromio(&ltq_eth_data.mac.sa_data,
		(void *)KSEG1ADDR(LTQ_FLASH_START + mac_addr), 6);
	ltq_register_etop(&ltq_eth_data);
}

static u16 arv_ath5k_eeprom_data[ATH5K_PLAT_EEP_MAX_WORDS];
static u16 arv_ath9k_eeprom_data[ATH9K_PLAT_EEP_MAX_WORDS];
static u8 arv_athxk_eeprom_mac[6];

static void __init
arv_register_ath5k(unsigned int ath_addr, unsigned int mac_addr)
{
	int i;

	memcpy_fromio(arv_athxk_eeprom_mac,
		(void *)KSEG1ADDR(LTQ_FLASH_START + mac_addr), 6);
	arv_athxk_eeprom_mac[5]++;
	memcpy_fromio(arv_ath5k_eeprom_data,
		(void *)KSEG1ADDR(LTQ_FLASH_START + ath_addr), ATH5K_PLAT_EEP_MAX_WORDS);
	// swap eeprom bytes
	for (i = 0; i < ATH5K_PLAT_EEP_MAX_WORDS>>1; i++) {
		arv_ath5k_eeprom_data[i] = swab16(arv_ath5k_eeprom_data[i]);
		if (i == 0x17e>>1) {
			/*
			 * regdomain is invalid. it's unknown how did original
			 * fw convered value to 0x82d4 so for now force to 0x67
			 */
			arv_ath5k_eeprom_data[i] &= 0x0000;
			arv_ath5k_eeprom_data[i] |= 0x67;
		}
	}
}

static void __init
arv_register_ath9k(unsigned int ath_addr, unsigned int mac_addr)
{
	int i;
	u16 *eepdata, sum, el;

	memcpy_fromio(arv_athxk_eeprom_mac,
		(void *)KSEG1ADDR(LTQ_FLASH_START + mac_addr), 6);
	arv_athxk_eeprom_mac[5]++;
	memcpy_fromio(arv_ath9k_eeprom_data,
		(void *)KSEG1ADDR(LTQ_FLASH_START + ath_addr), ATH9K_PLAT_EEP_MAX_WORDS);

	// force regdomain to 0x67
	arv_ath9k_eeprom_data[0x208>>1] = 0x67;

	// calculate new checksum
	sum = arv_ath9k_eeprom_data[0x200>>1];
	el = sum / sizeof(u16) - 2;  /* skip length and (old) checksum */
	eepdata = (u16 *) (&arv_ath9k_eeprom_data[0x204>>1]); /* after checksum */
	for (i = 0; i < el; i++)
		sum ^= *eepdata++;
	sum ^= 0xffff;
	arv_ath9k_eeprom_data[0x202>>1] = sum;
}

static void __init
arv3527p_init(void)
{
#define ARV3527P_MAC_ADDR		0x3f0016

	ltq_register_gpio_stp();
	// ltq_add_device_gpio_leds(arv3527p_gpio_leds, ARRAY_SIZE(arv3527p_gpio_leds));
	arv_load_nor(0x400000);
	arv_register_ethernet(ARV3527P_MAC_ADDR);
}

MIPS_MACHINE(LANTIQ_MACH_ARV3527P,
			"ARV3527P",
			"ARV3527P - Arcor Easybox 401",
			arv3527p_init);

static void __init
arv4510pw_init(void)
{
#define ARV4510PW_MAC_ADDR		0x3f0014

	ltq_register_gpio_stp();
	ltq_add_device_gpio_leds(-1, ARRAY_SIZE(arv4510pw_gpio_leds), arv4510pw_gpio_leds);
	arv_load_nor(0x400000);
	ltq_pci_data.irq[12] = (INT_NUM_IM2_IRL0 + 31);
	ltq_pci_data.irq[15] = (INT_NUM_IM0_IRL0 + 26);
	ltq_pci_data.gpio |= PCI_EXIN2 | PCI_REQ2;
	ltq_register_pci(&ltq_pci_data);
	arv_register_ethernet(ARV4510PW_MAC_ADDR);
}

MIPS_MACHINE(LANTIQ_MACH_ARV4510PW,
			"ARV4510PW",
			"ARV4510PW - Wippies Homebox",
			arv4510pw_init);

static void __init
arv4518pw_init(void)
{
#define ARV4518PW_EBU			0
#define ARV4518PW_USB			14
#define ARV4518PW_SWITCH_RESET		13
#define ARV4518PW_ATH_ADDR		0x3f0400
#define ARV4518PW_MAC_ADDR		0x3f0016

	ltq_register_gpio_ebu(ARV4518PW_EBU);
	ltq_add_device_gpio_leds(-1, ARRAY_SIZE(arv4518pw_gpio_leds), arv4518pw_gpio_leds);
	ltq_register_gpio_keys_polled(-1, LTQ_KEYS_POLL_INTERVAL,
				ARRAY_SIZE(arv4518pw_gpio_keys), arv4518pw_gpio_keys);
	arv_load_nor(0x400000);
	ltq_pci_data.gpio = PCI_GNT2 | PCI_REQ2;
	ltq_register_pci(&ltq_pci_data);
	xway_register_dwc(ARV4518PW_USB);
	arv_register_ethernet(ARV4518PW_MAC_ADDR);
	arv_register_ath5k(ARV4518PW_ATH_ADDR, ARV4518PW_MAC_ADDR);
	ltq_register_ath5k(arv_ath5k_eeprom_data, arv_athxk_eeprom_mac);
	ltq_register_tapi();

	gpio_request(ARV4518PW_SWITCH_RESET, "switch");
	gpio_direction_output(ARV4518PW_SWITCH_RESET, 1);
	gpio_export(ARV4518PW_SWITCH_RESET, 0);
}

MIPS_MACHINE(LANTIQ_MACH_ARV4518PW,
			"ARV4518PW",
			"ARV4518PW - SMC7908A-ISP, Airties WAV-221",
			arv4518pw_init);

static void __init
arv4519pw_init(void)
{
#define ARV4519PW_EBU			0
#define ARV4519PW_USB			14
#define ARV4519PW_RELAY			31
#define ARV4519PW_SWITCH_RESET		13
#define ARV4519PW_ATH_ADDR		0x3f0400
#define ARV4519PW_MAC_ADDR		0x3f0016

	arv_load_nor(0x400000);
	ltq_register_gpio_ebu(ARV4519PW_EBU);
	ltq_add_device_gpio_leds(-1, ARRAY_SIZE(arv4519pw_gpio_leds), arv4519pw_gpio_leds);
	ltq_register_gpio_keys_polled(-1, LTQ_KEYS_POLL_INTERVAL,
				ARRAY_SIZE(arv4519pw_gpio_keys), arv4519pw_gpio_keys);
	ltq_pci_data.gpio = PCI_GNT2 | PCI_REQ1;
	ltq_register_pci(&ltq_pci_data);
	xway_register_dwc(ARV4519PW_USB);
	arv_register_ethernet(ARV4519PW_MAC_ADDR);
	arv_register_ath5k(ARV4519PW_ATH_ADDR, ARV4519PW_MAC_ADDR);
	ltq_register_ath5k(arv_ath5k_eeprom_data, arv_athxk_eeprom_mac);
	ltq_register_tapi();

	gpio_request(ARV4519PW_RELAY, "relay");
	gpio_direction_output(ARV4519PW_RELAY, 1);
	gpio_export(ARV4519PW_RELAY, 0);

	gpio_request(ARV4519PW_SWITCH_RESET, "switch");
	gpio_set_value(ARV4519PW_SWITCH_RESET, 1);
	gpio_export(ARV4519PW_SWITCH_RESET, 0);
}

MIPS_MACHINE(LANTIQ_MACH_ARV4519PW,
			"ARV4519PW",
			"ARV4519PW - Vodafone, Pirelli",
			arv4519pw_init);

static void __init
arv4520pw_init(void)
{
#define ARV4520PW_EBU			0x400
#define ARV4520PW_USB			28
#define ARV4520PW_SWITCH_RESET		110
#define ARV4520PW_MAC_ADDR		0x3f0016

	ltq_register_gpio_ebu(ARV4520PW_EBU);
	ltq_add_device_gpio_leds(-1, ARRAY_SIZE(arv4520pw_gpio_leds), arv4520pw_gpio_leds);
	arv_load_nor(0x400000);
	ltq_register_pci(&ltq_pci_data);
	ltq_register_tapi();
	arv_register_ethernet(ARV4520PW_MAC_ADDR);
	ltq_register_rt2x00(NULL, (const u8 *) ltq_eth_data.mac.sa_data);
	xway_register_dwc(ARV4520PW_USB);
	ltq_register_tapi();

	gpio_request(ARV4520PW_SWITCH_RESET, "switch");
	gpio_set_value(ARV4520PW_SWITCH_RESET, 1);
}

MIPS_MACHINE(LANTIQ_MACH_ARV4520PW,
			"ARV4520PW",
			"ARV4520PW - Airties WAV-281, Arcor A800",
			arv4520pw_init);

static void __init
arv452Cpw_init(void)
{
#define ARV452CPW_EBU			0x77f
#define ARV452CPW_USB			28
#define ARV452CPW_RELAY1		31
#define ARV452CPW_RELAY2		107
#define ARV452CPW_SWITCH_RESET		110
#define ARV452CPW_ATH_ADDR		0x3f0400
#define ARV452CPW_MAC_ADDR		0x3f0016

	ltq_register_gpio_ebu(ARV452CPW_EBU);
	ltq_add_device_gpio_leds(-1, ARRAY_SIZE(arv452cpw_gpio_leds), arv452cpw_gpio_leds);
	arv_load_nor(0x400000);
	ltq_register_pci(&ltq_pci_data);
	xway_register_dwc(ARV452CPW_USB);
	arv_register_ethernet(ARV452CPW_MAC_ADDR);
	arv_register_ath5k(ARV452CPW_ATH_ADDR, ARV452CPW_MAC_ADDR);
	ltq_register_ath5k(arv_ath5k_eeprom_data, arv_athxk_eeprom_mac);
	ltq_register_tapi();

	gpio_request(ARV452CPW_SWITCH_RESET, "switch");
	gpio_set_value(ARV452CPW_SWITCH_RESET, 1);
	gpio_export(ARV452CPW_SWITCH_RESET, 0);

	gpio_request(ARV452CPW_RELAY1, "relay1");
	gpio_direction_output(ARV452CPW_RELAY1, 1);
	gpio_export(ARV452CPW_RELAY1, 0);

	gpio_request(ARV452CPW_RELAY2, "relay2");
	gpio_set_value(ARV452CPW_RELAY2, 1);
	gpio_export(ARV452CPW_RELAY2, 0);
}

MIPS_MACHINE(LANTIQ_MACH_ARV452CPW,
			"ARV452CPW",
			"ARV452CPW - Arcor A801",
			arv452Cpw_init);

static void __init
arv4525pw_init(void)
{
#define ARV4525PW_ATH_ADDR		0x3f0400
#define ARV4525PW_MAC_ADDR		0x3f0016

	arv_load_nor(0x400000);
	ltq_add_device_gpio_leds(-1, ARRAY_SIZE(arv4525pw_gpio_leds), arv4525pw_gpio_leds);
	gpio_request_array(arv4525pw_gpios, ARRAY_SIZE(arv4525pw_gpios));
	gpio_export(ARV4525PW_RELAY, false);
	gpio_export(ARV4525PW_PHYRESET, false);
	ltq_pci_data.clock = PCI_CLOCK_INT;
	ltq_register_pci(&ltq_pci_data);
	arv_register_ath5k(ARV4525PW_ATH_ADDR, ARV4525PW_MAC_ADDR);
	ltq_register_ath5k(arv_ath5k_eeprom_data, arv_athxk_eeprom_mac);
	ltq_eth_data.mii_mode = PHY_INTERFACE_MODE_MII;
	arv_register_ethernet(ARV4525PW_MAC_ADDR);
	ltq_register_tapi();
}

MIPS_MACHINE(LANTIQ_MACH_ARV4525PW,
			"ARV4525PW",
			"ARV4525PW - Speedport W502V",
			arv4525pw_init);

static void __init
arv7525pw_init(void)
{
#define ARV7525P_MAC_ADDR	0x3f0016

	arv_load_nor(0x400000);
	ltq_add_device_gpio_leds(-1, ARRAY_SIZE(arv4525pw_gpio_leds), arv4525pw_gpio_leds);
	ltq_register_gpio_keys_polled(-1, LTQ_KEYS_POLL_INTERVAL,
				ARRAY_SIZE(arv7525pw_gpio_keys), arv7525pw_gpio_keys);
	ltq_pci_data.clock = PCI_CLOCK_INT;
	ltq_pci_data.gpio = PCI_GNT1 | PCI_EXIN1;
	ltq_pci_data.irq[14] = (INT_NUM_IM3_IRL0 + 31);
	ltq_register_pci(&ltq_pci_data);
	ltq_eth_data.mii_mode = PHY_INTERFACE_MODE_MII;
	ltq_register_rt2x00("RT2860.eeprom", NULL);
	ltq_register_tapi();
	arv_register_ethernet(ARV7525P_MAC_ADDR);
}

MIPS_MACHINE(LANTIQ_MACH_ARV7525PW,
			"ARV7525PW",
			"ARV7525PW - Speedport W303V",
			arv7525pw_init);

static void __init
arv7518pw_init(void)
{
#define ARV7518PW_EBU			0x2
#define ARV7518PW_USB			14
#define ARV7518PW_SWITCH_RESET		13
#define ARV7518PW_ATH_ADDR		0x7f0400
#define ARV7518PW_MAC_ADDR		0x7f0016

	arv_load_nor(0x800000);
	ltq_register_gpio_ebu(ARV7518PW_EBU);
	ltq_add_device_gpio_leds(-1, ARRAY_SIZE(arv7518pw_gpio_leds), arv7518pw_gpio_leds);
	ltq_register_gpio_keys_polled(-1, LTQ_KEYS_POLL_INTERVAL,
				ARRAY_SIZE(arv7518pw_gpio_keys), arv7518pw_gpio_keys);
	ltq_register_pci(&ltq_pci_data);
	ltq_register_tapi();
	xway_register_dwc(ARV7518PW_USB);
	arv_register_ethernet(ARV7518PW_MAC_ADDR);
	arv_register_ath9k(ARV7518PW_ATH_ADDR, ARV7518PW_MAC_ADDR);
	ltq_register_ath9k(arv_ath9k_eeprom_data, arv_athxk_eeprom_mac);
	ltq_pci_ath_fixup(14, arv_ath9k_eeprom_data);
	ltq_register_tapi();

	gpio_request(ARV7518PW_SWITCH_RESET, "switch");
	gpio_direction_output(ARV7518PW_SWITCH_RESET, 1);
	gpio_export(ARV7518PW_SWITCH_RESET, 0);
}

MIPS_MACHINE(LANTIQ_MACH_ARV7518PW,
			"ARV7518PW",
			"ARV7518PW - ASTORIA",
			arv7518pw_init);

static void __init
arv752dpw22_init(void)
{
#define ARV752DPW22_EBU			0x2
#define ARV752DPW22_USB			100
#define ARV752DPW22_RELAY		101
#define ARV752DPW22_MAC_ADDR		0x7f0016

	arv_load_nor(0x800000);
	ltq_register_gpio_ebu(ARV752DPW22_EBU);
	ltq_add_device_gpio_leds(-1, ARRAY_SIZE(arv752dpw22_gpio_leds), arv752dpw22_gpio_leds);
	ltq_register_gpio_keys_polled(-1, LTQ_KEYS_POLL_INTERVAL,
				ARRAY_SIZE(arv752dpw22_gpio_keys), arv752dpw22_gpio_keys);
	ltq_pci_data.irq[15] = (INT_NUM_IM3_IRL0 + 31);
	ltq_pci_data.gpio |= PCI_EXIN1 | PCI_REQ2;
	ltq_register_pci(&ltq_pci_data);
	xway_register_dwc(ARV752DPW22_USB);
	arv_register_ethernet(ARV752DPW22_MAC_ADDR);
	ltq_register_tapi();

	gpio_request(ARV752DPW22_RELAY, "relay");
	gpio_set_value(ARV752DPW22_RELAY, 1);
	gpio_export(ARV752DPW22_RELAY, 0);
}

MIPS_MACHINE(LANTIQ_MACH_ARV752DPW22,
			"ARV752DPW22",
			"ARV752DPW22 - Arcor A803",
			arv752dpw22_init);

static void __init
arv752dpw_init(void)
{
#define ARV752DPW22_EBU			0x2
#define ARV752DPW22_USB			100
#define ARV752DPW22_RELAY		101
#define ARV752DPW22_MAC_ADDR		0x7f0016

	arv_load_nor(0x800000);
	ltq_register_gpio_ebu(ARV752DPW22_EBU);
	ltq_add_device_gpio_leds(-1, ARRAY_SIZE(arv752dpw22_gpio_leds), arv752dpw22_gpio_leds);
	ltq_register_gpio_keys_polled(-1, LTQ_KEYS_POLL_INTERVAL, ARRAY_SIZE(arv752dpw22_gpio_keys), arv752dpw22_gpio_keys);
	ltq_pci_data.irq[14] = (INT_NUM_IM3_IRL0 + 31);
	ltq_pci_data.gpio |= PCI_EXIN1 | PCI_REQ2;
	ltq_register_pci(&ltq_pci_data);
	ltq_register_tapi();
	xway_register_dwc(ARV752DPW22_USB);
	ltq_register_rt2x00("RT2860.eeprom", NULL);
	arv_register_ethernet(ARV752DPW22_MAC_ADDR);
	gpio_request(ARV752DPW22_RELAY, "relay");
	gpio_set_value(ARV752DPW22_RELAY, 1);
	gpio_export(ARV752DPW22_RELAY, 0);

}

MIPS_MACHINE(LANTIQ_MACH_ARV752DPW,
			"ARV752DPW",
			"ARV752DPW - Arcor A802",
			arv752dpw_init);

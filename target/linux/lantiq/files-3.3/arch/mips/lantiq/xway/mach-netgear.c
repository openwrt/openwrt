/*
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 *
 *  Copyright (C) 2010 John Crispin <blogic@openwrt.org>
 *  Copyright (C) 2012 Pieter Voorthuijsen <p.voorthuijsen@gmail.com>
 */

#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/partitions.h>
#include <linux/mtd/physmap.h>
#include <linux/input.h>
#include <linux/phy.h>
#include <linux/spi/spi.h>
#include <linux/spi/flash.h>
#include <linux/spi/spi_gpio.h>
#include <linux/ath9k_platform.h>
#include <linux/if_ether.h>
#include <linux/etherdevice.h>
#include <linux/kobject.h>
#include <linux/sysfs.h>
#include <linux/rtl8366.h>

#include <lantiq_soc.h>
#include <irq.h>
#include <dev-gpio-leds.h>
#include <dev-gpio-buttons.h>
#include "dev-wifi-athxk.h"

#include "../machtypes.h"
#include "devices.h"
#include "dev-dwc_otg.h"
#include "pci-ath-fixup.h"
#include <mtd/mtd-abi.h>
#include <asm-generic/sizes.h>

static struct mtd_partition dgn3500_partitions[] = {
	{
		.name = "u-boot",
		.offset = 0,
		.size = 0x10000,
		.mask_flags = MTD_WRITEABLE,
	},
	{
		.name = "environment",
		.offset = 0x10000,
		.size = 0x10000,
		.mask_flags = MTD_WRITEABLE,
	},
	{
		.name = "calibration",
		.offset = 0x20000,
		.size = 0x10000,
		.mask_flags = MTD_WRITEABLE,
	},
	{
		.name = "linux",
		.offset = 0x50000,
		.size = 0xfa0000,
	},
};

static struct ltq_pci_data ltq_pci_data = {
	.clock	= PCI_CLOCK_INT,
	.gpio	= PCI_GNT1 | PCI_REQ1,
	.irq	= {
		[14] = INT_NUM_IM0_IRL0 + 22,
	},
};

static struct ltq_eth_data ltq_eth_data = {
	.mii_mode = PHY_INTERFACE_MODE_MII,
};

static struct gpio_led
dgn3500_gpio_leds[] __initdata = {
	{ .name = "soc:green:power", .gpio = 34, .active_low = 1, },
	{ .name = "soc:red:power", .gpio = 39, .active_low = 1, },
	{ .name = "soc:orange:wlan", .gpio = 51, .active_low = 1, },
	{ .name = "soc:green:wps", .gpio = 52, .active_low = 1, },
	{ .name = "soc:green:usb", .gpio = 22, .active_low = 1, },
	{ .name = "soc:green:dsl", .gpio = 4, .active_low = 1, },
	{ .name = "soc:green:internet", .gpio = 2, .active_low = 1, },
};

static struct gpio_keys_button
dgn3500_gpio_keys[] __initdata = {
	{
		.desc = "wps",
		.type = EV_KEY,
		.code = BTN_0,
		.debounce_interval = LTQ_KEYS_DEBOUNCE_INTERVAL,
		.gpio = 54,
		.active_low = 1,
	},
	{
		.desc = "reset",
		.type = EV_KEY,
		.code = BTN_1,
		.debounce_interval = LTQ_KEYS_DEBOUNCE_INTERVAL,
		.gpio = 36,
		.active_low = 1,
	},
};

#define SPI_GPIO_MRST   16
#define SPI_GPIO_MTSR   17
#define SPI_GPIO_CLK    18
#define SPI_GPIO_CS0    10

static struct spi_gpio_platform_data spi_gpio_data = {
	.sck            = SPI_GPIO_CLK,
	.mosi           = SPI_GPIO_MTSR,
	.miso           = SPI_GPIO_MRST,
	.num_chipselect = 2,
};

static struct platform_device spi_gpio_device = {
	.name                   = "spi_gpio",
	.dev.platform_data      = &spi_gpio_data,
};

static struct flash_platform_data spi_flash_data = {
	.name           = "sflash",
	.parts          = dgn3500_partitions,
	.nr_parts       = ARRAY_SIZE(dgn3500_partitions),
};

static struct spi_board_info spi_flash __initdata = {
	.modalias               = "m25p80",
	.bus_num                = 0,
	.chip_select            = 0,
	.max_speed_hz           = 10 * 1000 * 1000,
	.mode                   = SPI_MODE_3,
	.chip_select            = 0,
	.controller_data        = (void *) SPI_GPIO_CS0,
	.platform_data          = &spi_flash_data
};

static u8 ltq_ethaddr[6] = { 0 };

static int __init setup_ethaddr(char *str)
{
	if (!mac_pton(str, ltq_ethaddr))
		memset(ltq_ethaddr, 0, 6);
	return 0;
}
__setup("ethaddr=", setup_ethaddr);

#define smi_SCK		37
#define smi_SDA		35

static struct rtl8366_platform_data rtl8366rb_data = {
	.gpio_sda = smi_SDA,
	.gpio_sck = smi_SCK,
};

static struct platform_device rtl8366rb_device = {
	.name = RTL8366RB_DRIVER_NAME,
	.id = -1,
	.dev = {
		.platform_data  = &rtl8366rb_data,
	}
};

static u16 dgn3500_eeprom_data[ATH9K_PLAT_EEP_MAX_WORDS] = {0};

static ssize_t ath_eeprom_read(struct file *filp, struct kobject *kobj,
		struct bin_attribute *attr, char *buf,
		loff_t offset, size_t count)
{
	if (unlikely(offset >= sizeof(dgn3500_eeprom_data)))
		return 0;
	if ((offset + count) > sizeof(dgn3500_eeprom_data))
		count = sizeof(dgn3500_eeprom_data) - offset;
	if (unlikely(!count))
		return count;

	memcpy(buf, (char *)(dgn3500_eeprom_data) + offset, count);

	return count;
}

extern struct ath9k_platform_data ath9k_pdata;

static ssize_t ath_eeprom_write(struct file *filp, struct kobject *kobj,
		struct bin_attribute *attr, char *buf,
		loff_t offset, size_t count)
{
	int i;
	char *eeprom_bytes = (char *)dgn3500_eeprom_data;

	if (unlikely(offset >= sizeof(dgn3500_eeprom_data)))
		return -EFBIG;
	if ((offset + count) > sizeof(dgn3500_eeprom_data))
		count = sizeof(dgn3500_eeprom_data) - offset;
	if (unlikely(!count))
		return count;
	if (count % 2)
		return 0;

	/* The PCI fixup routine requires an endian swap of the calibartion data
	 * stored in flash */
	for (i = 0; i < count; i += 2) {
		eeprom_bytes[offset + i + 1] = buf[i];
		eeprom_bytes[offset + i] = buf[i+1];
	}

	/* The original data does not contain a checksum. Set the country and
	 * calculate new checksum when all data is received */
	if ((count + offset) == sizeof(dgn3500_eeprom_data))
		memcpy(ath9k_pdata.eeprom_data, dgn3500_eeprom_data,
				sizeof(ath9k_pdata.eeprom_data));

	return count;
}

static struct bin_attribute dev_attr_ath_eeprom = {
	.attr = {
		.name = "ath_eeprom",
		.mode = S_IRUGO|S_IWUSR,
	},
	.read = ath_eeprom_read,
	.write = ath_eeprom_write,
};

static void __init dgn3500_init(void)
{
	if (sysfs_create_bin_file(firmware_kobj, &dev_attr_ath_eeprom))
		printk(KERN_INFO "Failed to create ath eeprom sysfs entry\n");
	ltq_add_device_gpio_leds(-1, ARRAY_SIZE(dgn3500_gpio_leds),
			dgn3500_gpio_leds);
	ltq_register_gpio_keys_polled(-1, LTQ_KEYS_POLL_INTERVAL,
			ARRAY_SIZE(dgn3500_gpio_keys), dgn3500_gpio_keys);
	platform_device_register(&spi_gpio_device);
	ltq_register_pci(&ltq_pci_data);
	spi_register_board_info(&spi_flash, 1);
	if (!is_valid_ether_addr(ltq_ethaddr)) {
		printk(KERN_INFO "MAC invalid using random\n");
		random_ether_addr(ltq_ethaddr);
	}
	memcpy(&ltq_eth_data.mac.sa_data, ltq_ethaddr, 6);
	ltq_register_etop(&ltq_eth_data);
	ltq_register_ath9k(dgn3500_eeprom_data, ltq_ethaddr);
	ltq_pci_ath_fixup(14, dgn3500_eeprom_data);
	/* The usb power is always enabled, protected by a fuse */
	xway_register_dwc(-1);
	platform_device_register(&rtl8366rb_device);
}

MIPS_MACHINE(LANTIQ_MACH_DGN3500B,
	     "DGN3500B",
	     "Netgear DGN3500B",
	      dgn3500_init);

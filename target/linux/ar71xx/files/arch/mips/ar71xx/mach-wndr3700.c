/*
 *  Netgear WNDR3700 board support
 *
 *  Copyright (C) 2009 Marco Porsch
 *  Copyright (C) 2009 Gabor Juhos <juhosg@openwrt.org>
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 */

#include <linux/platform_device.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/partitions.h>
#include <linux/spi/spi.h>
#include <linux/spi/flash.h>
#include <linux/input.h>
#include <linux/pci.h>
#include <linux/ath9k_platform.h>
#include <linux/delay.h>
#include <linux/rtl8366_smi.h>

#include <asm/mips_machine.h>
#include <asm/mach-ar71xx/ar71xx.h>
#include <asm/mach-ar71xx/pci.h>

#include "devices.h"

#define WNDR3700_GPIO_LED_WPS_ORANGE	0
#define WNDR3700_GPIO_LED_POWER_ORANGE	1
#define WNDR3700_GPIO_LED_POWER_GREEN	2
#define WNDR3700_GPIO_LED_WPS_GREEN	4

#define WNDR3700_GPIO_BTN_WPS		3
#define WNDR3700_GPIO_BTN_RESET		8
#define WNDR3700_GPIO_BTN_WIFI		11

#define WNDR3700_GPIO_RTL8366_SDA	5
#define WNDR3700_GPIO_RTL8366_SCK	7

#define WNDR3700_BUTTONS_POLL_INTERVAL    20

#ifdef CONFIG_MTD_PARTITIONS
static struct mtd_partition wndr3700_partitions[] = {
	{
		.name		= "uboot",
		.offset		= 0,
		.size		= 0x050000,
		.mask_flags	= MTD_WRITEABLE,
	} , {
		.name		= "env",
		.offset		= 0x050000,
		.size		= 0x020000,
		.mask_flags	= MTD_WRITEABLE,
	} , {
		.name		= "rootfs",
		.offset		= 0x070000,
		.size		= 0x720000,
	} , {
		.name		= "config",
		.offset		= 0x790000,
		.size		= 0x010000,
		.mask_flags	= MTD_WRITEABLE,
	} , {
		.name		= "config_bak",
		.offset		= 0x7a0000,
		.size		= 0x010000,
		.mask_flags	= MTD_WRITEABLE,
	} , {
		.name		= "pot",
		.offset		= 0x7b0000,
		.size		= 0x010000,
		.mask_flags	= MTD_WRITEABLE,
	} , {
		.name		= "traffic_meter",
		.offset		= 0x7c0000,
		.size		= 0x010000,
		.mask_flags	= MTD_WRITEABLE,
	} , {
		.name		= "language",
		.offset		= 0x7d0000,
		.size		= 0x020000,
		.mask_flags	= MTD_WRITEABLE,
	} , {
		.name		= "caldata",
		.offset		= 0x7f0000,
		.size		= 0x010000,
		.mask_flags	= MTD_WRITEABLE,
	}
};
#endif /* CONFIG_MTD_PARTITIONS */

static struct flash_platform_data wndr3700_flash_data = {
#ifdef CONFIG_MTD_PARTITIONS
        .parts          = wndr3700_partitions,
        .nr_parts       = ARRAY_SIZE(wndr3700_partitions),
#endif
};

#ifdef CONFIG_PCI
static struct ar71xx_pci_irq wndr3700_pci_irqs[] __initdata = {
	{
		.slot	= 0,
		.pin	= 1,
		.irq	= AR71XX_PCI_IRQ_DEV0,
	}, {
		.slot	= 1,
		.pin	= 1,
		.irq	= AR71XX_PCI_IRQ_DEV1,
	}
};

static struct ath9k_platform_data wndr3700_wmac0_data;
static u8 wndr3700_wmac0_macaddr[6];
static struct ath9k_platform_data wndr3700_wmac1_data;
static u8 wndr3700_wmac1_macaddr[6];

static void wndr3700_pci_fixup(struct pci_dev *dev)
{
	void __iomem *mem;
	u16 *cal_data;
	u16 cmd;
	u32 bar0;
	u32 val;

	if (ar71xx_mach != AR71XX_MACH_WNDR3700)
		return;

	switch (PCI_SLOT(dev->devfn)) {
	case 17:
		cal_data = wndr3700_wmac0_data.eeprom_data;
		break;
	case 18:
		cal_data = wndr3700_wmac1_data.eeprom_data;
		break;
	default:
		return;
	}

	if (*cal_data != 0xa55a) {
		printk(KERN_ERR "PCI: no calibration data found for %s\n",
		       pci_name(dev));
		return;
	}

	mem = ioremap(AR71XX_PCI_MEM_BASE, 0x10000);
	if (!mem) {
		printk(KERN_ERR "PCI: ioremap error for device %s\n",
		       pci_name(dev));
		return;
	}

	printk(KERN_INFO "PCI: fixup device %s\n", pci_name(dev));

	pci_read_config_dword(dev, PCI_BASE_ADDRESS_0, &bar0);

	/* Setup the PCI device to allow access to the internal registers */
	pci_write_config_dword(dev, PCI_BASE_ADDRESS_0, AR71XX_PCI_MEM_BASE);
	pci_read_config_word(dev, PCI_COMMAND, &cmd);
	cmd |= PCI_COMMAND_MASTER | PCI_COMMAND_MEMORY;
	pci_write_config_word(dev, PCI_COMMAND, cmd);

        /* set pointer to first reg address */
	cal_data += 3;
	while (*cal_data != 0xffff) {
		u32 reg;
		reg = *cal_data++;
		val = *cal_data++;
		val |= (*cal_data++) << 16;

		__raw_writel(val, mem + reg);
		udelay(100);
	}

	pci_read_config_dword(dev, PCI_VENDOR_ID, &val);
	dev->vendor = val & 0xffff;
	dev->device = (val >> 16) & 0xffff;

	pci_read_config_dword(dev, PCI_CLASS_REVISION, &val);
	dev->revision = val & 0xff;
	dev->class = val >> 8; /* upper 3 bytes */

	pci_read_config_word(dev, PCI_COMMAND, &cmd);
	cmd &= ~(PCI_COMMAND_MASTER | PCI_COMMAND_MEMORY);
	pci_write_config_word(dev, PCI_COMMAND, cmd);

	pci_write_config_dword(dev, PCI_BASE_ADDRESS_0, bar0);

	iounmap(mem);
}
DECLARE_PCI_FIXUP_EARLY(PCI_VENDOR_ID_ATHEROS, PCI_ANY_ID,
			wndr3700_pci_fixup);

static int wndr3700_pci_plat_dev_init(struct pci_dev *dev)
{
	switch (PCI_SLOT(dev->devfn)) {
	case 17:
		dev->dev.platform_data = &wndr3700_wmac0_data;
		break;
	case 18:
		dev->dev.platform_data = &wndr3700_wmac1_data;
		break;
	}

	return 0;
}

static void __init wndr3700_pci_init(void)
{
	u8 *ee = (u8 *) KSEG1ADDR(0x1fff0000);

	memcpy(wndr3700_wmac0_data.eeprom_data, ee + 0x1000,
	       sizeof(wndr3700_wmac0_data.eeprom_data));
	memcpy(wndr3700_wmac0_macaddr, ee, sizeof(wndr3700_wmac0_macaddr));
	wndr3700_wmac0_data.macaddr = wndr3700_wmac0_macaddr;

	memcpy(wndr3700_wmac1_data.eeprom_data, ee + 0x5000,
	       sizeof(wndr3700_wmac1_data.eeprom_data));
	memcpy(wndr3700_wmac1_macaddr, ee + 12, sizeof(wndr3700_wmac1_macaddr));
	wndr3700_wmac1_data.macaddr = wndr3700_wmac1_macaddr;

	ar71xx_pci_plat_dev_init = wndr3700_pci_plat_dev_init;
	ar71xx_pci_init(ARRAY_SIZE(wndr3700_pci_irqs), wndr3700_pci_irqs);
}
#else
static inline void wndr3700_pci_init(void) { };
#endif /* CONFIG_PCI */

static struct spi_board_info wndr3700_spi_info[] = {
	{
		.bus_num	= 0,
		.chip_select	= 0,
		.max_speed_hz	= 25000000,
		.modalias	= "m25p80",
		.platform_data  = &wndr3700_flash_data,
	}
};

static struct gpio_led wndr3700_leds_gpio[] __initdata = {
	{
		.name		= "wndr3700:green:power",
		.gpio		= WNDR3700_GPIO_LED_POWER_GREEN,
		.active_low	= 1,
	}, {
		.name		= "wndr3700:orange:power",
		.gpio		= WNDR3700_GPIO_LED_POWER_ORANGE,
		.active_low	= 1,
	}, {
		.name		= "wndr3700:green:wps",
		.gpio		= WNDR3700_GPIO_LED_WPS_GREEN,
		.active_low	= 1,
	}, {
		.name		= "wndr3700:orange:wps",
		.gpio		= WNDR3700_GPIO_LED_WPS_ORANGE,
		.active_low	= 1,
	}
};

static struct gpio_button wndr3700_gpio_buttons[] __initdata = {
	{
		.desc		= "reset",
		.type		= EV_KEY,
		.code		= BTN_0,
		.threshold	= 5,
		.gpio		= WNDR3700_GPIO_BTN_RESET,
	}, {
		.desc		= "wps",
		.type		= EV_KEY,
		.code		= BTN_1,
		.threshold	= 5,
		.gpio		= WNDR3700_GPIO_BTN_WPS,
	} , {
		.desc		= "wifi",
		.type		= EV_KEY,
		.code		= BTN_2,
		.threshold	= 5,
		.gpio		= WNDR3700_GPIO_BTN_WIFI,
	}
};

static struct rtl8366_smi_platform_data wndr3700_rtl8366_smi_data = {
	.gpio_sda        = WNDR3700_GPIO_RTL8366_SDA,
	.gpio_sck        = WNDR3700_GPIO_RTL8366_SCK,
};

static struct platform_device wndr3700_rtl8366_smi_device = {
	.name		= "rtl8366-smi",
	.id		= -1,
	.dev = {
		.platform_data	= &wndr3700_rtl8366_smi_data,
	}
};

static void __init wndr3700_setup(void)
{
	u8 *mac = (u8 *) KSEG1ADDR(0x1fff0000);

	ar71xx_set_mac_base(mac);

	ar71xx_eth0_pll_data.pll_1000 = 0x11110000;
	ar71xx_eth0_data.mii_bus_dev = &wndr3700_rtl8366_smi_device.dev;
	ar71xx_eth0_data.phy_if_mode = PHY_INTERFACE_MODE_RGMII;
	ar71xx_eth0_data.phy_mask = 0xf;
	ar71xx_eth0_data.speed = SPEED_1000;
	ar71xx_eth0_data.duplex = DUPLEX_FULL;

	ar71xx_eth1_pll_data.pll_1000 = 0x11110000;
	ar71xx_eth1_data.mii_bus_dev = &wndr3700_rtl8366_smi_device.dev;
	ar71xx_eth1_data.phy_if_mode = PHY_INTERFACE_MODE_RGMII;
	ar71xx_eth1_data.phy_mask = 0x10;

	ar71xx_add_device_eth(0);
	ar71xx_add_device_eth(1);

	ar71xx_add_device_usb();

	ar71xx_add_device_spi(NULL, wndr3700_spi_info,
			      ARRAY_SIZE(wndr3700_spi_info));

        ar71xx_add_device_leds_gpio(-1, ARRAY_SIZE(wndr3700_leds_gpio),
				    wndr3700_leds_gpio);

	ar71xx_add_device_gpio_buttons(-1, WNDR3700_BUTTONS_POLL_INTERVAL,
				      ARRAY_SIZE(wndr3700_gpio_buttons),
				      wndr3700_gpio_buttons);

	platform_device_register(&wndr3700_rtl8366_smi_device);
	platform_device_register_simple("wndr3700-led-usb", -1, NULL, 0);
	wndr3700_pci_init();
}

MIPS_MACHINE(AR71XX_MACH_WNDR3700, "NETGEAR WNDR3700", wndr3700_setup);

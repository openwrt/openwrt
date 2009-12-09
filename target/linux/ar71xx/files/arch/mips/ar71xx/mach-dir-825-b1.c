/*
 *  D-Link DIR-825 rev. B1 board support
 *
 *  Copyright (C) 2009 Lukas Kuna, Evkanet, s.r.o.
 *
 *  based on mach-wndr3700.c
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

#define DIR825B1_GPIO_LED_BLUE_USB		0
#define DIR825B1_GPIO_LED_ORANGE_POWER		1
#define DIR825B1_GPIO_LED_BLUE_POWER		2
#define DIR825B1_GPIO_LED_BLUE_POWERSAVE	4
#define DIR825B1_GPIO_LED_ORANGE_PLANET		6
#define DIR825B1_GPIO_LED_BLUE_PLANET		11

#define DIR825B1_GPIO_BTN_RESET			3
#define DIR825B1_GPIO_BTN_POWERSAVE		8

#define DIR825B1_GPIO_RTL8366_SDA		5
#define DIR825B1_GPIO_RTL8366_SCK		7

#define DIR825B1_BUTTONS_POLL_INTERVAL		20

#define DIR825B1_CAL_LOCATION_0			0x1f661000
#define DIR825B1_CAL_LOCATION_1			0x1f665000

#define DIR825B1_MAC_LOCATION_0			0x2ffa81b8
#define DIR825B1_MAC_LOCATION_1			0x2ffa8370

static struct ath9k_platform_data dir825b1_wmac0_data;
static struct ath9k_platform_data dir825b1_wmac1_data;
static char dir825b1_wmac0_mac[6];
static char dir825b1_wmac1_mac[6];

#ifdef CONFIG_MTD_PARTITIONS
static struct mtd_partition dir825b1_partitions[] = {
	{
		.name		= "uboot",
		.offset		= 0,
		.size		= 0x040000,
		.mask_flags	= MTD_WRITEABLE,
	} , {
		.name		= "config",
		.offset		= 0x040000,
		.size		= 0x010000,
		.mask_flags	= MTD_WRITEABLE,
	} , {
		.name		= "firmware",
		.offset		= 0x050000,
		.size		= 0x610000,
	} , {
		.name		= "caldata",
		.offset		= 0x660000,
		.size		= 0x010000,
		.mask_flags	= MTD_WRITEABLE,
	}
};
#endif /* CONFIG_MTD_PARTITIONS */

static struct flash_platform_data dir825b1_flash_data = {
#ifdef CONFIG_MTD_PARTITIONS
        .parts          = dir825b1_partitions,
        .nr_parts       = ARRAY_SIZE(dir825b1_partitions),
#endif
};

static struct spi_board_info dir825b1_spi_info[] = {
	{
		.bus_num	= 0,
		.chip_select	= 0,
		.max_speed_hz	= 25000000,
		.modalias	= "m25p80",
		.platform_data  = &dir825b1_flash_data,
	}
};

static struct gpio_led dir825b1_leds_gpio[] __initdata = {
	{
		.name		= "dir825b1:blue:usb",
		.gpio		= DIR825B1_GPIO_LED_BLUE_USB,
		.active_low	= 1,
	}, {
		.name		= "dir825b1:orange:power",
		.gpio		= DIR825B1_GPIO_LED_ORANGE_POWER,
		.active_low	= 1,
	}, {
		.name		= "dir825b1:blue:power",
		.gpio		= DIR825B1_GPIO_LED_BLUE_POWER,
		.active_low	= 1,
	}, {
		.name		= "dir825b1:blue:powersave",
		.gpio		= DIR825B1_GPIO_LED_BLUE_POWERSAVE,
		.active_low	= 1,
	}, {
		.name		= "dir825b1:orange:planet",
		.gpio		= DIR825B1_GPIO_LED_ORANGE_PLANET,
		.active_low	= 1,
	}, {
		.name		= "dir825b1:blue:planet",
		.gpio		= DIR825B1_GPIO_LED_BLUE_PLANET,
		.active_low	= 1,
	}
};

static struct gpio_button dir825b1_gpio_buttons[] __initdata = {
	{
		.desc		= "reset",
		.type		= EV_KEY,
		.code		= BTN_0,
		.threshold	= 5,
		.gpio		= DIR825B1_GPIO_BTN_RESET,
		.active_low	= 1,
	} , {
		.desc		= "powersave",
		.type		= EV_KEY,
		.code		= BTN_1,
		.threshold	= 5,
		.gpio		= DIR825B1_GPIO_BTN_POWERSAVE,
		.active_low	= 1,
	}
};

static struct rtl8366_smi_platform_data dir825b1_rtl8366_smi_data = {
	.gpio_sda        = DIR825B1_GPIO_RTL8366_SDA,
	.gpio_sck        = DIR825B1_GPIO_RTL8366_SCK,
};

static struct platform_device dir825b1_rtl8366_smi_device = {
	.name		= "rtl8366-smi",
	.id		= -1,
	.dev = {
		.platform_data	= &dir825b1_rtl8366_smi_data,
	}
};

#ifdef CONFIG_PCI
static struct ar71xx_pci_irq dir825b1_pci_irqs[] __initdata = {
        {
                .slot   = 0,
                .pin    = 1,
                .irq    = AR71XX_PCI_IRQ_DEV0,
        }, {
                .slot   = 1,
                .pin    = 1,
                .irq    = AR71XX_PCI_IRQ_DEV1,
        }
};

static int dir825b1_pci_plat_dev_init(struct pci_dev *dev)
{
	switch(PCI_SLOT(dev->devfn)) {
	case 17:
		dev->dev.platform_data = &dir825b1_wmac0_data;
		break;

	case 18:
		dev->dev.platform_data = &dir825b1_wmac1_data;
		break;
	}

	return 0;
}

static void dir825b1_pci_fixup(struct pci_dev *dev)
{
	void __iomem *mem;
	u16 *cal_data;
	u16 cmd;
	u32 bar0;
	u32 val;

	if (ar71xx_mach != AR71XX_MACH_DIR_825_B1)
		return;

	dir825b1_pci_plat_dev_init(dev);
	cal_data = dev->dev.platform_data;

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
			dir825b1_pci_fixup);

static void __init dir825b1_pci_init(void)
{
	memcpy(dir825b1_wmac0_data.eeprom_data,
	       (u8 *) KSEG1ADDR(DIR825B1_CAL_LOCATION_0),
	       sizeof(dir825b1_wmac0_data.eeprom_data));

	memcpy(dir825b1_wmac1_data.eeprom_data,
	       (u8 *) KSEG1ADDR(DIR825B1_CAL_LOCATION_1),
	       sizeof(dir825b1_wmac1_data.eeprom_data));

	memcpy(dir825b1_wmac0_mac, (u8 *)KSEG1ADDR(DIR825B1_MAC_LOCATION_0), 6);
	dir825b1_wmac0_data.macaddr = dir825b1_wmac0_mac;
	memcpy(dir825b1_wmac1_mac, (u8 *)KSEG1ADDR(DIR825B1_MAC_LOCATION_1), 6);
	dir825b1_wmac1_data.macaddr = dir825b1_wmac1_mac;

	ar71xx_pci_plat_dev_init = dir825b1_pci_plat_dev_init;
	ar71xx_pci_init(ARRAY_SIZE(dir825b1_pci_irqs), dir825b1_pci_irqs);
}
#else
static void __init dir825b1_pci_init(void) { }
#endif /* CONFIG_PCI */

static void __init dir825b1_setup(void)
{
        u8 mac[6], i;

	memcpy(mac, (u8*)KSEG1ADDR(DIR825B1_MAC_LOCATION_1), 6);
	for(i = 5; i >= 3; i--)
		if(++mac[i] != 0x00) break;

	ar71xx_set_mac_base(mac);

	ar71xx_add_device_mdio(0x0);

	ar71xx_eth0_data.mii_bus_dev = &dir825b1_rtl8366_smi_device.dev;
	ar71xx_eth0_data.phy_if_mode = PHY_INTERFACE_MODE_RGMII;
	ar71xx_eth0_data.speed = SPEED_1000;
	ar71xx_eth0_data.duplex = DUPLEX_FULL;
	ar71xx_eth0_pll_data.pll_1000 = 0x11110000;

	ar71xx_eth1_data.mii_bus_dev = &dir825b1_rtl8366_smi_device.dev;
	ar71xx_eth1_data.phy_if_mode = PHY_INTERFACE_MODE_RGMII;
	ar71xx_eth1_data.phy_mask = 0x10;
	ar71xx_eth1_pll_data.pll_1000 = 0x11110000;

	ar71xx_add_device_eth(0);
	ar71xx_add_device_eth(1);

	ar71xx_add_device_spi(NULL, dir825b1_spi_info,
			      ARRAY_SIZE(dir825b1_spi_info));

	ar71xx_add_device_leds_gpio(-1, ARRAY_SIZE(dir825b1_leds_gpio),
					dir825b1_leds_gpio);

	ar71xx_add_device_gpio_buttons(-1, DIR825B1_BUTTONS_POLL_INTERVAL,
					ARRAY_SIZE(dir825b1_gpio_buttons),
					dir825b1_gpio_buttons);

	ar71xx_add_device_usb();

	platform_device_register(&dir825b1_rtl8366_smi_device);
	dir825b1_pci_init();
}

MIPS_MACHINE(AR71XX_MACH_DIR_825_B1, "D-Link DIR-825 rev. B1", dir825b1_setup);

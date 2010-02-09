/*
 *  Atheros AP94 reference board PCI initialization
 *
 *  Copyright (C) 2009-2010 Gabor Juhos <juhosg@openwrt.org>
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 */

#include <linux/pci.h>
#include <linux/ath9k_platform.h>
#include <linux/delay.h>

#include <asm/mach-ar71xx/ar71xx.h>
#include <asm/mach-ar71xx/pci.h>

#include "dev-ap94-pci.h"

static struct ath9k_platform_data ap94_wmac0_data;
static struct ath9k_platform_data ap94_wmac1_data;
static char ap94_wmac0_mac[6];
static char ap94_wmac1_mac[6];
static int ap94_pci_fixup_enabled;

static struct ar71xx_pci_irq ap94_pci_irqs[] __initdata = {
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

static int ap94_pci_plat_dev_init(struct pci_dev *dev)
{
	switch(PCI_SLOT(dev->devfn)) {
	case 17:
		dev->dev.platform_data = &ap94_wmac0_data;
		break;

	case 18:
		dev->dev.platform_data = &ap94_wmac1_data;
		break;
	}

	return 0;
}

static void ap94_pci_fixup(struct pci_dev *dev)
{
	void __iomem *mem;
	u16 *cal_data;
	u16 cmd;
	u32 bar0;
	u32 val;

	if (!ap94_pci_fixup_enabled)
		return;

	switch (PCI_SLOT(dev->devfn)) {
	case 17:
		cal_data = ap94_wmac0_data.eeprom_data;
		break;
	case 18:
		cal_data = ap94_wmac1_data.eeprom_data;
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
DECLARE_PCI_FIXUP_EARLY(PCI_VENDOR_ID_ATHEROS, PCI_ANY_ID, ap94_pci_fixup);

void __init ap94_pci_enable_quirk_wndr3700(void)
{
	ap94_wmac0_data.quirk_wndr3700 = 1;
	ap94_wmac1_data.quirk_wndr3700 = 1;
}

void __init ap94_pci_init(u8 *cal_data0, u8 *mac_addr0,
			  u8 *cal_data1, u8 *mac_addr1)
{
	if (cal_data0)
		memcpy(ap94_wmac0_data.eeprom_data, cal_data0,
		       sizeof(ap94_wmac0_data.eeprom_data));

	if (cal_data1)
		memcpy(ap94_wmac1_data.eeprom_data, cal_data1,
		       sizeof(ap94_wmac1_data.eeprom_data));

	if (mac_addr0) {
		memcpy(ap94_wmac0_mac, mac_addr0, sizeof(ap94_wmac0_mac));
		ap94_wmac0_data.macaddr = ap94_wmac0_mac;
	}

	if (mac_addr1) {
		memcpy(ap94_wmac1_mac, mac_addr1, sizeof(ap94_wmac1_mac));
		ap94_wmac1_data.macaddr = ap94_wmac1_mac;
	}

	ar71xx_pci_plat_dev_init = ap94_pci_plat_dev_init;
	ar71xx_pci_init(ARRAY_SIZE(ap94_pci_irqs), ap94_pci_irqs);

	ap94_pci_fixup_enabled = 1;
}

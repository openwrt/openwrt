/*
 *  Atheros AP91 reference board PCI initialization
 *
 *  Copyright (C) 2009 Gabor Juhos <juhosg@openwrt.org>
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

#include "dev-ap91-pci.h"

static struct ath9k_platform_data ap91_wmac_data;
static char ap91_wmac_mac[6];
static int ap91_pci_fixup_enabled;

static struct ar71xx_pci_irq ap91_pci_irqs[] __initdata = {
	{
		.slot	= 0,
		.pin	= 1,
		.irq	= AR71XX_PCI_IRQ_DEV0,
	}
};

static int ap91_pci_plat_dev_init(struct pci_dev *dev)
{
	switch(PCI_SLOT(dev->devfn)) {
	case 0:
		dev->dev.platform_data = &ap91_wmac_data;
		break;
	}

	return 0;
}

static void ap91_pci_fixup(struct pci_dev *dev)
{
	void __iomem *mem;
	u16 *cal_data;
	u16 cmd;
	u32 val;

	if (!ap91_pci_fixup_enabled)
		return;

	printk(KERN_INFO "PCI: fixup device %s\n", pci_name(dev));

	cal_data = ap91_wmac_data.eeprom_data;
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

	/* Setup the PCI device to allow access to the internal registers */
	pci_write_config_dword(dev, PCI_BASE_ADDRESS_0, 0xffff);
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

	iounmap(mem);
}
DECLARE_PCI_FIXUP_EARLY(PCI_VENDOR_ID_ATHEROS, PCI_ANY_ID, ap91_pci_fixup);

void __init ap91_pci_init(u8 *cal_data, u8 *mac_addr)
{
	if (cal_data)
		memcpy(ap91_wmac_data.eeprom_data, cal_data,
		       sizeof(ap91_wmac_data.eeprom_data));

	if (mac_addr) {
		memcpy(ap91_wmac_mac, mac_addr, sizeof(ap91_wmac_mac));
		ap91_wmac_data.macaddr = ap91_wmac_mac;
	}

	ar71xx_pci_plat_dev_init = ap91_pci_plat_dev_init;
	ar71xx_pci_init(ARRAY_SIZE(ap91_pci_irqs), ap91_pci_irqs);

	ap91_pci_fixup_enabled = 1;
}

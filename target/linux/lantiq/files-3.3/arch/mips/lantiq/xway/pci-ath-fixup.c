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
#include <linux/init.h>
#include <linux/delay.h>
#include <lantiq_soc.h>

#define LTQ_PCI_MEM_BASE		0x18000000

struct ath_fixup {
	u16		*cal_data;
	unsigned	slot;
};

static int ath_num_fixups;
static struct ath_fixup ath_fixups[2];

static void ath_pci_fixup(struct pci_dev *dev)
{
	void __iomem *mem;
	u16 *cal_data = NULL;
	u16 cmd;
	u32 bar0;
	u32 val;
	unsigned i;

	for (i = 0; i < ath_num_fixups; i++) {
		if (ath_fixups[i].cal_data == NULL)
			continue;

		if (ath_fixups[i].slot != PCI_SLOT(dev->devfn))
			continue;

		cal_data = ath_fixups[i].cal_data;
		break;
	}

	if (cal_data == NULL)
		return;

	if (*cal_data != 0xa55a) {
		pr_err("pci %s: invalid calibration data\n", pci_name(dev));
		return;
	}

	pr_info("pci %s: fixup device configuration\n", pci_name(dev));

	mem = ioremap(LTQ_PCI_MEM_BASE, 0x10000);
	if (!mem) {
		pr_err("pci %s: ioremap error\n", pci_name(dev));
		return;
	}

	pci_read_config_dword(dev, PCI_BASE_ADDRESS_0, &bar0);
	pci_write_config_dword(dev, PCI_BASE_ADDRESS_0, LTQ_PCI_MEM_BASE);
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

		ltq_w32(swab32(val), mem + reg);
		udelay(100);
	}

	pci_read_config_dword(dev, PCI_VENDOR_ID, &val);
	dev->vendor = val & 0xffff;
	dev->device = (val >> 16) & 0xffff;

	pci_read_config_dword(dev, PCI_CLASS_REVISION, &val);
	dev->revision = val & 0xff;
	dev->class = val >> 8; /* upper 3 bytes */

	pr_info("pci %s: fixup info: [%04x:%04x] revision %02x class %#08x\n", 
		pci_name(dev), dev->vendor, dev->device, dev->revision, dev->class);

	pci_read_config_word(dev, PCI_COMMAND, &cmd);
	cmd &= ~(PCI_COMMAND_MASTER | PCI_COMMAND_MEMORY);
	pci_write_config_word(dev, PCI_COMMAND, cmd);

	pci_write_config_dword(dev, PCI_BASE_ADDRESS_0, bar0);

	iounmap(mem);
}
DECLARE_PCI_FIXUP_EARLY(PCI_VENDOR_ID_ATHEROS, PCI_ANY_ID, ath_pci_fixup);

void __init ltq_pci_ath_fixup(unsigned slot, u16 *cal_data)
{
	if (ath_num_fixups >= ARRAY_SIZE(ath_fixups))
		return;

	ath_fixups[ath_num_fixups].slot = slot;
	ath_fixups[ath_num_fixups].cal_data = cal_data;
	ath_num_fixups++;
}

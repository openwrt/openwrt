/*
 *  Atheros db120 reference board PCI initialization
 *
 *  Copyright (C) 2010-2011 Jaiganesh Narayanan <jnarayanan@atheros.com>
 *  Copyright (C) 2011 Gabor Juhos <juhosg@openwrt.org>
 *
 *  Parts of this file are based on Atheros linux 2.6.31 BSP
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 */

#include <linux/pci.h>
#include <linux/ath9k_platform.h>

#include <asm/mach-ar71xx/ar71xx.h>
#include <asm/mach-ar71xx/pci.h>

#include "dev-db120-pci.h"

static struct ath9k_platform_data db120_wmac_data = {
	.led_pin = -1,
};
static char db120_wmac_mac[6];

static struct ar71xx_pci_irq db120_pci_irqs[] __initdata = {
	{
		.slot	= 0,
		.pin	= 1,
		.irq	= AR71XX_PCI_IRQ_DEV0,
	}
};

static int db120_pci_plat_dev_init(struct pci_dev *dev)
{
	switch (PCI_SLOT(dev->devfn)) {
	case 0:
		dev->dev.platform_data = &db120_wmac_data;
		break;
	}

	return 0;
}

void __init db120_pci_init(u8 *cal_data, u8 *mac_addr)
{
	if (cal_data)
		memcpy(db120_wmac_data.eeprom_data, cal_data,
		       sizeof(db120_wmac_data.eeprom_data));

	if (mac_addr) {
		memcpy(db120_wmac_mac, mac_addr, sizeof(db120_wmac_mac));
		db120_wmac_data.macaddr = db120_wmac_mac;
	}

	ar71xx_pci_plat_dev_init = db120_pci_plat_dev_init;
	ar71xx_pci_init(ARRAY_SIZE(db120_pci_irqs), db120_pci_irqs);
}

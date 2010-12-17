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
#include "pci-ath9k-fixup.h"

static struct ath9k_platform_data ap94_wmac0_data = {
	.led_pin = -1,
};
static struct ath9k_platform_data ap94_wmac1_data = {
	.led_pin = -1,
};
static char ap94_wmac0_mac[6];
static char ap94_wmac1_mac[6];

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
	switch (PCI_SLOT(dev->devfn)) {
	case 17:
		dev->dev.platform_data = &ap94_wmac0_data;
		break;

	case 18:
		dev->dev.platform_data = &ap94_wmac1_data;
		break;
	}

	return 0;
}

__init void ap94_pci_setup_wmac_led_pin(unsigned wmac, int pin)
{
	switch (wmac) {
	case 0:
		ap94_wmac0_data.led_pin = pin;
		break;
	case 1:
		ap94_wmac1_data.led_pin = pin;
		break;
	}
}

__init void ap94_pci_setup_wmac_gpio(unsigned wmac, u32 mask, u32 val)
{
	switch (wmac) {
	case 0:
		ap94_wmac0_data.gpio_mask = mask;
		ap94_wmac0_data.gpio_val = val;
		break;
	case 1:
		ap94_wmac1_data.gpio_mask = mask;
		ap94_wmac1_data.gpio_val = val;
		break;
	}
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

	pci_enable_ath9k_fixup(17, ap94_wmac0_data.eeprom_data);
	pci_enable_ath9k_fixup(18, ap94_wmac1_data.eeprom_data);
}

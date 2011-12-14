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
#include "pci-ath9k-fixup.h"

static struct ath9k_platform_data ap91_wmac_data = {
	.led_pin = -1,
};
static char ap91_wmac_mac[6];

static struct ar71xx_pci_irq ap91_pci_irqs[] __initdata = {
	{
		.slot	= 0,
		.pin	= 1,
		.irq	= AR71XX_PCI_IRQ_DEV0,
	}
};

static int ap91_pci_plat_dev_init(struct pci_dev *dev)
{
	switch (PCI_SLOT(dev->devfn)) {
	case 0:
		dev->dev.platform_data = &ap91_wmac_data;
		break;
	}

	return 0;
}

__init void ap91_pci_setup_wmac_leds(struct gpio_led *leds, int num_leds)
{
	ap91_wmac_data.leds = leds;
	ap91_wmac_data.num_leds = num_leds;
}

__init void ap91_pci_setup_wmac_led_pin(int pin)
{
	ap91_wmac_data.led_pin = pin;
}

__init void ap91_pci_setup_wmac_gpio(u32 mask, u32 val)
{
	ap91_wmac_data.gpio_mask = mask;
	ap91_wmac_data.gpio_val = val;
}

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

	pci_enable_ath9k_fixup(0, ap91_wmac_data.eeprom_data);
}

/*
 *  Senao EAP7660D board support
 *
 *  Copyright (C) 2010 Daniel Golle <daniel.golle@gmail.com>
 *  Copyright (C) 2008 Gabor Juhos <juhosg@openwrt.org>
 *  Copyright (C) 2008 Imre Kaloz <kaloz@openwrt.org>
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 */

#include <linux/pci.h>
#include <linux/ath5k_platform.h>
#include <linux/delay.h>
#include <asm/mach-ar71xx/ar71xx.h>
#include <asm/mach-ar71xx/pci.h>

#include "machtype.h"
#include "devices.h"
#include "dev-gpio-buttons.h"
#include "dev-leds-gpio.h"
#include "dev-m25p80.h"

#define EAP7660D_KEYS_POLL_INTERVAL	20	/* msecs */
#define EAP7660D_KEYS_DEBOUNCE_INTERVAL	(3 * EAP7660D_KEYS_POLL_INTERVAL)

#define EAP7660D_GPIO_DS4		7
#define EAP7660D_GPIO_DS5		2
#define EAP7660D_GPIO_DS7		0
#define EAP7660D_GPIO_DS8		4
#define EAP7660D_GPIO_SW1		3
#define EAP7660D_GPIO_SW3		8
#define EAP7660D_PHYMASK		BIT(20)
#define EAP7660D_BOARDCONFIG		0x1F7F0000
#define EAP7660D_GBIC_MAC_OFFSET	0x1000
#define EAP7660D_WMAC0_MAC_OFFSET	0x1010
#define EAP7660D_WMAC1_MAC_OFFSET	0x1016
#define EAP7660D_WMAC0_CALDATA_OFFSET	0x2000
#define EAP7660D_WMAC1_CALDATA_OFFSET	0x3000

static struct ath5k_platform_data eap7660d_wmac0_data;
static struct ath5k_platform_data eap7660d_wmac1_data;
static char eap7660d_wmac0_mac[6];
static char eap7660d_wmac1_mac[6];
static u16 eap7660d_wmac0_eeprom[ATH5K_PLAT_EEP_MAX_WORDS];
static u16 eap7660d_wmac1_eeprom[ATH5K_PLAT_EEP_MAX_WORDS];

#ifdef CONFIG_PCI
static struct ar71xx_pci_irq eap7660d_pci_irqs[] __initdata = {
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

static int eap7660d_pci_plat_dev_init(struct pci_dev *dev)
{
	switch (PCI_SLOT(dev->devfn)) {
	case 17:
		dev->dev.platform_data = &eap7660d_wmac0_data;
		break;

	case 18:
		dev->dev.platform_data = &eap7660d_wmac1_data;
		break;
	}

	return 0;
}

void __init eap7660d_pci_init(u8 *cal_data0, u8 *mac_addr0,
			      u8 *cal_data1, u8 *mac_addr1)
{
	if (cal_data0 && *cal_data0 == 0xa55a) {
		memcpy(eap7660d_wmac0_eeprom, cal_data0,
			ATH5K_PLAT_EEP_MAX_WORDS);
		eap7660d_wmac0_data.eeprom_data = eap7660d_wmac0_eeprom;
	}

	if (cal_data1 && *cal_data1 == 0xa55a) {
		memcpy(eap7660d_wmac1_eeprom, cal_data1,
			ATH5K_PLAT_EEP_MAX_WORDS);
		eap7660d_wmac1_data.eeprom_data = eap7660d_wmac1_eeprom;
	}

	if (mac_addr0) {
		memcpy(eap7660d_wmac0_mac, mac_addr0,
			sizeof(eap7660d_wmac0_mac));
		eap7660d_wmac0_data.macaddr = eap7660d_wmac0_mac;
	}

	if (mac_addr1) {
		memcpy(eap7660d_wmac1_mac, mac_addr1,
			sizeof(eap7660d_wmac1_mac));
		eap7660d_wmac1_data.macaddr = eap7660d_wmac1_mac;
	}

	ar71xx_pci_plat_dev_init = eap7660d_pci_plat_dev_init;
	ar71xx_pci_init(ARRAY_SIZE(eap7660d_pci_irqs), eap7660d_pci_irqs);
}
#else
static inline void eap7660d_pci_init(u8 *cal_data0, u8 *mac_addr0,
				     u8 *cal_data1, u8 *mac_addr1)
{
}
#endif /* CONFIG_PCI */

static struct gpio_led eap7660d_leds_gpio[] __initdata = {
	{
		.name		= "eap7660d:green:ds8",
		.gpio		= EAP7660D_GPIO_DS8,
		.active_low	= 0,
	},
	{
		.name		= "eap7660d:green:ds5",
		.gpio		= EAP7660D_GPIO_DS5,
		.active_low	= 0,
	},
	{
		.name		= "eap7660d:green:ds7",
		.gpio		= EAP7660D_GPIO_DS7,
		.active_low	= 0,
	},
	{
		.name		= "eap7660d:green:ds4",
		.gpio		= EAP7660D_GPIO_DS4,
		.active_low	= 0,
	}
};

static struct gpio_keys_button eap7660d_gpio_keys[] __initdata = {
	{
		.desc		= "reset",
		.type		= EV_KEY,
		.code		= KEY_RESTART,
		.debounce_interval = EAP7660D_KEYS_DEBOUNCE_INTERVAL,
		.gpio		= EAP7660D_GPIO_SW1,
		.active_low	= 1,
	},
	{
		.desc		= "wps",
		.type		= EV_KEY,
		.code		= KEY_WPS_BUTTON,
		.debounce_interval = EAP7660D_KEYS_DEBOUNCE_INTERVAL,
		.gpio		= EAP7660D_GPIO_SW3,
		.active_low	= 1,
	}
};

static const char *eap7660d_part_probes[] = {
	"RedBoot",
	NULL,
};

static struct flash_platform_data eap7660d_flash_data = {
	.part_probes	= eap7660d_part_probes,
};

static void __init eap7660d_setup(void)
{
	u8 *boardconfig = (u8 *) KSEG1ADDR(EAP7660D_BOARDCONFIG);

	ar71xx_add_device_mdio(0, ~EAP7660D_PHYMASK);

	ar71xx_init_mac(ar71xx_eth0_data.mac_addr,
			boardconfig + EAP7660D_GBIC_MAC_OFFSET, 0);
	ar71xx_eth0_data.phy_if_mode = PHY_INTERFACE_MODE_RGMII;
	ar71xx_eth0_data.phy_mask = EAP7660D_PHYMASK;
	ar71xx_add_device_eth(0);
	ar71xx_add_device_m25p80(&eap7660d_flash_data);
	ar71xx_add_device_leds_gpio(-1, ARRAY_SIZE(eap7660d_leds_gpio),
					eap7660d_leds_gpio);
	ar71xx_register_gpio_keys_polled(-1, EAP7660D_KEYS_POLL_INTERVAL,
					 ARRAY_SIZE(eap7660d_gpio_keys),
					 eap7660d_gpio_keys);
	eap7660d_pci_init(boardconfig + EAP7660D_WMAC0_CALDATA_OFFSET,
			boardconfig + EAP7660D_WMAC0_MAC_OFFSET,
			boardconfig + EAP7660D_WMAC1_CALDATA_OFFSET,
			boardconfig + EAP7660D_WMAC1_MAC_OFFSET);
};

MIPS_MACHINE(AR71XX_MACH_EAP7660D, "EAP7660D", "Senao EAP7660D",
	     eap7660d_setup);

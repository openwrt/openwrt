/*
 *  Atheros PB92 board support
 *
 *  Copyright (C) 2010 Felix Fietkau <nbd@openwrt.org>
 *  Copyright (C) 2008-2009 Gabor Juhos <juhosg@openwrt.org>
 *  Copyright (C) 2008 Imre Kaloz <kaloz@openwrt.org>
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 */

#include <asm/mach-ath79/ath79.h>

#include "dev-eth.h"
#include "dev-gpio-buttons.h"
#include "dev-m25p80.h"
#include "dev-usb.h"
#include "machtypes.h"
#include "pci.h"

#define PB92_KEYS_POLL_INTERVAL		20	/* msecs */
#define PB92_KEYS_DEBOUNCE_INTERVAL	(3 * PB92_KEYS_POLL_INTERVAL)

#define PB92_GPIO_BTN_SW4	8
#define PB92_GPIO_BTN_SW5	3

static struct gpio_keys_button pb92_gpio_keys[] __initdata = {
	{
		.desc		= "sw4",
		.type		= EV_KEY,
		.code		= BTN_0,
		.debounce_interval = PB92_KEYS_DEBOUNCE_INTERVAL,
		.gpio		= PB92_GPIO_BTN_SW4,
		.active_low	= 1,
	}, {
		.desc		= "sw5",
		.type		= EV_KEY,
		.code		= BTN_1,
		.debounce_interval = PB92_KEYS_DEBOUNCE_INTERVAL,
		.gpio		= PB92_GPIO_BTN_SW5,
		.active_low	= 1,
	}
};

static void __init pb92_init(void)
{
	u8 *mac = (u8 *) KSEG1ADDR(0x1fff0000);

	ath79_register_m25p80(NULL);

	ath79_register_mdio(0, ~BIT(0));
	ath79_init_mac(ath79_eth0_data.mac_addr, mac, 0);
	ath79_eth0_data.phy_if_mode = PHY_INTERFACE_MODE_RGMII;
	ath79_eth0_data.speed = SPEED_1000;
	ath79_eth0_data.duplex = DUPLEX_FULL;
	ath79_eth0_data.phy_mask = BIT(0);

	ath79_register_eth(0);

	ath79_register_gpio_keys_polled(-1, PB92_KEYS_POLL_INTERVAL,
					 ARRAY_SIZE(pb92_gpio_keys),
					 pb92_gpio_keys);

	ath79_register_usb();

	ath79_register_pci();
}

MIPS_MACHINE(ATH79_MACH_PB92, "PB92", "Atheros PB92", pb92_init);

/*
 *  Yuncore XD6700 (from Atheros AP147) reference board support
 *
 *  Copyright (C) 2014 Matthias Schiffer <mschiffer@universe-factory.net>
 *  Copyright (C) 2015 Sven Eckelmann <sven@open-mesh.com>
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 */

#include <linux/phy.h>
#include <linux/gpio.h>
#include <linux/ar8216_platform.h>

#include <asm/mach-ath79/ar71xx_regs.h>
#include <asm/mach-ath79/ath79.h>

#include "common.h"
#include "dev-ap9x-pci.h"
#include "dev-eth.h"
#include "dev-gpio-buttons.h"
#include "dev-leds-gpio.h"
#include "dev-m25p80.h"
#include "dev-usb.h"
#include "dev-wmac.h"
#include "machtypes.h"
#include "pci.h"

#define XD6700_GPIO_LED_SYSTEM      1

#define XD6700_GPIO_LED_GREEN      12
#define XD6700_GPIO_LED_RED        13

#define XD6700_GPIO_BTN_RESET      17

#define XD6700_KEYS_POLL_INTERVAL  20     /* msecs */
#define XD6700_KEYS_DEBOUNCE_INTERVAL     (3 * XD6700_KEYS_POLL_INTERVAL)

#define XD6700_WMAC0_OFFSET         0x1000

static struct gpio_led xd6700_leds_gpio[] __initdata = {
        {
                .name           = "xd6700:green:system",
                .gpio           = XD6700_GPIO_LED_SYSTEM,
                .active_low     = 1,
        }, { 
                .name           = "xd6700:green:wlan2g",
                .gpio           = XD6700_GPIO_LED_GREEN,
                .active_low     = 1,
	}, {
		.name		= "xd6700:red:wlan5g",
		.gpio		= XD6700_GPIO_LED_RED,
		.active_low	= 1,
	}
};


static struct gpio_keys_button xd6700_gpio_keys[] __initdata = {
	{
		.desc		= "reset",
		.type		= EV_KEY,
		.code		= KEY_RESTART,
		.debounce_interval = XD6700_KEYS_DEBOUNCE_INTERVAL,
		.gpio		= XD6700_GPIO_BTN_RESET,
		.active_low	= 1,
	}
};

static void __init xd6700_setup(void)
{
	u8 *art = (u8 *)KSEG1ADDR(0x1fff0000);

	ath79_register_m25p80(NULL);
	ath79_register_leds_gpio(-1, ARRAY_SIZE(xd6700_leds_gpio),
				    xd6700_leds_gpio);
	ath79_register_gpio_keys_polled(-1, XD6700_KEYS_POLL_INTERVAL,
					    ARRAY_SIZE(xd6700_gpio_keys),
					    xd6700_gpio_keys);

	ath79_register_usb();

	ath79_register_pci();

	ath79_register_wmac(art + XD6700_WMAC0_OFFSET, NULL);

	ath79_setup_ar933x_phy4_switch(false, false);

	ath79_register_mdio(0, 0x0);

	/* WAN */
	ath79_switch_data.phy4_mii_en = 1;
	ath79_eth0_data.phy_if_mode = PHY_INTERFACE_MODE_MII;
	ath79_eth0_data.duplex = DUPLEX_FULL;
	ath79_eth0_data.speed = SPEED_1000;
	ath79_eth0_data.phy_mask = BIT(4);
	ath79_init_mac(ath79_eth0_data.mac_addr, art, 1);
	ath79_register_eth(0);

	/* LAN */
	ath79_eth1_data.phy_if_mode = PHY_INTERFACE_MODE_GMII;
	ath79_eth1_data.duplex = DUPLEX_FULL;
	ath79_eth0_data.speed = SPEED_1000;
	ath79_switch_data.phy_poll_mask |= BIT(4);
	ath79_init_mac(ath79_eth1_data.mac_addr, art, 0);
	ath79_register_eth(1);
}

MIPS_MACHINE(ATH79_MACH_XD6700, "XD6700", "YunCore XD6700", xd6700_setup);

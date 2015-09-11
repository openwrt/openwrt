/*
 *  COMFAST CF-E316N v2
 *  by Shenzhen Four Seas Global Link Network Technology Co., Ltd
 *
 *  aka CF-E316V2, CF-E316N-V2 and CF-E316Nv2.0 (no FCC ID)
 *
 *  Copyright (C) 2015 Paul Fertser <fercerpav@gmail.com>
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 */

#include <linux/gpio.h>
#include <linux/platform_device.h>
#include <linux/timer.h>

#include <asm/mach-ath79/ath79.h>
#include <asm/mach-ath79/ar71xx_regs.h>

#include "common.h"
#include "dev-eth.h"
#include "dev-gpio-buttons.h"
#include "dev-leds-gpio.h"
#include "dev-m25p80.h"
#include "dev-wmac.h"
#include "dev-usb.h"
#include "machtypes.h"

static struct gpio_led cf_e316n_v2_leds_gpio[] __initdata = {
	{
		.name		= "cf-e316n-v2:blue:diag",
		.gpio		= 0,
		.active_low	= 0,
	}, {
		.name		= "cf-e316n-v2:red:diag",
		.gpio		= 2,
		.active_low	= 0,
	}, {
		.name		= "cf-e316n-v2:green:diag",
		.gpio		= 3,
		.active_low	= 0,
	}, {
		.name		= "cf-e316n-v2:blue:wlan",
		.gpio		= 12,
		.active_low	= 1,
	}, {
		.name		= "cf-e316n-v2:blue:wan",
		.gpio		= 17,
		.active_low	= 1,
	}, {
		.name		= "cf-e316n-v2:blue:lan",
		.gpio		= 19,
		.active_low	= 1,
	},
};

static struct gpio_keys_button cf_e316n_v2_gpio_keys[] __initdata = {
	{
		.desc		= "Reset button",
		.type		= EV_KEY,
		.code		= KEY_RESTART,
		.debounce_interval = 60,
		.gpio		= 20,
		.active_low	= 1,
	},
};

/* There's a Pericon Technology PT7A7514 connected to GPIO 16 */
#define EXT_WATCHDOG_GPIO	16
static struct timer_list gpio_wdt_timer;

static void gpio_wdt_toggle(unsigned long period)
{
	static int state;
	state = !state;
	gpio_set_value(EXT_WATCHDOG_GPIO, state);
	mod_timer(&gpio_wdt_timer, jiffies + period);
}

static void __init cf_e316n_v2_setup(void)
{
	u8 *maclan = (u8 *) KSEG1ADDR(0x1f010000);
	u8 *macwlan = (u8 *) KSEG1ADDR(0x1f011002);
	u8 *ee = (u8 *) KSEG1ADDR(0x1f011000);
	u8 tmpmac[ETH_ALEN];

	gpio_request(EXT_WATCHDOG_GPIO, "PT7A7514 watchdog");
	gpio_direction_output(EXT_WATCHDOG_GPIO, 0);
	setup_timer(&gpio_wdt_timer, gpio_wdt_toggle, msecs_to_jiffies(500));
	gpio_wdt_toggle(msecs_to_jiffies(1));

	ath79_register_m25p80(NULL);

	ath79_setup_ar934x_eth_cfg(AR934X_ETH_CFG_SW_PHY_SWAP);
	ath79_register_mdio(1, 0x0);

	/* GMAC0 is connected to the PHY0 of the internal switch */
	ath79_switch_data.phy4_mii_en = 1;
	ath79_switch_data.phy_poll_mask = BIT(0);
	ath79_eth0_data.phy_if_mode = PHY_INTERFACE_MODE_MII;
	ath79_eth0_data.phy_mask = BIT(0);
	ath79_eth0_data.mii_bus_dev = &ath79_mdio1_device.dev;
	ath79_init_mac(ath79_eth0_data.mac_addr, maclan, 0);
	ath79_register_eth(0);

	/* GMAC1 is connected to the internal switch */
	ath79_eth1_data.phy_if_mode = PHY_INTERFACE_MODE_GMII;
	ath79_init_mac(ath79_eth1_data.mac_addr, maclan, 2);
	ath79_register_eth(1);

	/* Enable 2x Skyworks SE2576L WLAN power amplifiers */
	gpio_request(13, "RF Amp 1");
	gpio_direction_output(13, 1);
	gpio_request(14, "RF Amp 2");
	gpio_direction_output(14, 1);
	ath79_init_mac(tmpmac, macwlan, 0);
	ath79_register_wmac(ee, tmpmac);

	ath79_register_leds_gpio(-1, ARRAY_SIZE(cf_e316n_v2_leds_gpio),
				 cf_e316n_v2_leds_gpio);

	ath79_register_gpio_keys_polled(1, 20,
					ARRAY_SIZE(cf_e316n_v2_gpio_keys),
					cf_e316n_v2_gpio_keys);

	/* J1 is a High-Speed USB port, pin 1 is Vcc */
	ath79_register_usb();
}

MIPS_MACHINE(ATH79_MACH_CF_E316N_V2, "CF-E316N-V2", "COMFAST CF-E316N v2",
	     cf_e316n_v2_setup);

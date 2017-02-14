/*
 *  COMFAST CF-E214N v2
 *  by Shenzhen Four Seas Global Link Network Technology Co., Ltd
 *
 *  aka CF-E214V2, CF-E214N-V2 and CF-E214Nv2.0 (no FCC ID)
 *
 *  Copyright (C) 2017 Robert Budde <rl.budde@gmail.com>
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
#include "dev-leds-gpio.h"
#include "dev-m25p80.h"
#include "dev-usb.h"
#include "dev-wmac.h"
#include "machtypes.h"

static struct gpio_led cf_e214n_v2_leds_gpio[] __initdata = {
	{
		.name		= "cf-e214n-v2:wlan",
		.gpio		= 0,
		.active_low	= 1,
	}, {
		.name		= "cf-e214n-v2:lan",
		.gpio		= 2,
		.active_low	= 1,
	}, {
		.name		= "cf-e214n-v2:wan",
		.gpio		= 3,
		.active_low	= 1,
	}, {
		.name		= "cf-e214n-v2:link1",
		.gpio		= 11,
		.active_low	= 1,
	}, {
		.name		= "cf-e214n-v2:link2",
		.gpio		= 12,
		.active_low	= 1,
	}, {
		.name		= "cf-e214n-v2:link3",
		.gpio		= 14,
		.active_low	= 1,
	}, {
		.name		= "cf-e214n-v2:link4",
		.gpio		= 16,
		.active_low	= 1,
	},
};

/* There's a Pericon Technology PT7A7514 connected to GPIO 13 */
#define EXT_WATCHDOG_GPIO	13
static struct timer_list gpio_wdt_timer;

static void gpio_wdt_toggle(unsigned long period)
{
	static int state;
	state = !state;
	gpio_set_value(EXT_WATCHDOG_GPIO, state);
	mod_timer(&gpio_wdt_timer, jiffies + period);
}

static void __init cf_e214n_v2_setup(void)
{
	u8 *maclan = (u8 *) KSEG1ADDR(0x1f010000);
	u8 *macwlan = (u8 *) KSEG1ADDR(0x1f011002);
	u8 *ee = (u8 *) KSEG1ADDR(0x1f011000);
	u8 tmpmac[ETH_ALEN];

	int i;

	for (i = 0; i < ARRAY_SIZE(cf_e214n_v2_leds_gpio); i++)
		ath79_gpio_output_select(cf_e214n_v2_leds_gpio[i].gpio,
					 AR934X_GPIO_OUT_GPIO);

	gpio_request(EXT_WATCHDOG_GPIO, "PT7A7514 watchdog");
	gpio_direction_output(EXT_WATCHDOG_GPIO, 0);
	setup_timer(&gpio_wdt_timer, gpio_wdt_toggle, msecs_to_jiffies(500));
	gpio_wdt_toggle(msecs_to_jiffies(1));

	ath79_register_m25p80(NULL);

	ath79_register_mdio(0, 0x0);
	ath79_register_mdio(1, 0x0);

	ath79_init_mac(ath79_eth0_data.mac_addr, maclan, 0);
	ath79_init_mac(ath79_eth1_data.mac_addr, maclan, 1);

	/* "LAN" - GMAC0 is connected to the PHY4 of the internal switch */
	ath79_eth0_data.phy_if_mode = PHY_INTERFACE_MODE_MII;
	ath79_eth0_data.speed = SPEED_100;
	ath79_eth0_data.duplex = DUPLEX_FULL;
	ath79_eth0_data.phy_mask = BIT(4);
	ath79_register_eth(0);

	/* "WAN" - GMAC1 is connected to the internal switch */
	ath79_eth1_data.phy_if_mode = PHY_INTERFACE_MODE_GMII;
	ath79_eth1_data.speed = SPEED_1000;
	ath79_eth1_data.duplex = DUPLEX_FULL;
	ath79_switch_data.phy_poll_mask |= BIT(4);
	ath79_switch_data.phy4_mii_en = 1;
	ath79_register_eth(1);

	ath79_init_mac(tmpmac, macwlan, 0);
	ath79_register_wmac(ee, tmpmac);

	ath79_register_leds_gpio(-1, ARRAY_SIZE(cf_e214n_v2_leds_gpio),
				 cf_e214n_v2_leds_gpio);

	ath79_register_usb();
}

MIPS_MACHINE(ATH79_MACH_CF_E214N_V2, "CF-E214N-V2", "COMFAST CF-E214N v2",
	     cf_e214n_v2_setup);

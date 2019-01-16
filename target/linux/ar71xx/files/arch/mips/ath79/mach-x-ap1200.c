/*
 *  X AP1200 board
 *
 *  Copyright (C) 2019 Developer X <dev@x-wrt.com>
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 */

#include <linux/platform_device.h>
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

#define X_AP1200_GPIO_LED_WAN	0
#define X_AP1200_GPIO_LED_LAN1	1

#define X_AP1200_GPIO_LED_STATUS	4
#define X_AP1200_GPIO_LED_WLAN	12

#define X_AP1200_GPIO_BTN_WPS	17

#define X_AP1200_KEYS_POLL_INTERVAL	20	/* msecs */
#define X_AP1200_KEYS_DEBOUNCE_INTERVAL	(3 * X_AP1200_KEYS_POLL_INTERVAL)

#define X_AP1200_MAC0_OFFSET	0x1000

static struct gpio_led x_ap1200_leds_gpio[] __initdata = {
	{
		.name		= "x-ap1200:green:wan",
		.gpio		= X_AP1200_GPIO_LED_WAN,
		.active_low	= 1,
	}, {
		.name		= "x-ap1200:green:lan1",
		.gpio		= X_AP1200_GPIO_LED_LAN1,
		.active_low	= 1,
	}, {
		.name		= "x-ap1200:green:status",
		.gpio		= X_AP1200_GPIO_LED_STATUS,
		.active_low	= 1,
	}, {
		.name		= "x-ap1200:blue:wlan",
		.gpio		= X_AP1200_GPIO_LED_WLAN,
		.active_low	= 1,
	},
};

static struct gpio_keys_button x_ap1200_gpio_keys[] __initdata = {
	{
		.desc		= "RESET button",
		.type		= EV_KEY,
		.code		= KEY_RESTART,
		.debounce_interval = X_AP1200_KEYS_DEBOUNCE_INTERVAL,
		.gpio		= X_AP1200_GPIO_BTN_WPS,
		.active_low	= 1,
	},
};

static void __init x_ap1200_setup(void)
{
	u8 *art = (u8 *)KSEG1ADDR(0x1fff0000);

	ath79_register_m25p80(NULL);
	ath79_gpio_function_enable(AR934X_GPIO_FUNC_JTAG_DISABLE);
	ath79_register_leds_gpio(-1, ARRAY_SIZE(x_ap1200_leds_gpio),
				 x_ap1200_leds_gpio);
	ath79_register_gpio_keys_polled(-1, X_AP1200_KEYS_POLL_INTERVAL,
					ARRAY_SIZE(x_ap1200_gpio_keys),
					x_ap1200_gpio_keys);

	ath79_register_pci();

	ath79_register_wmac(art + X_AP1200_MAC0_OFFSET, NULL);

	ath79_setup_ar933x_phy4_switch(false, false);

	ath79_register_mdio(0, 0x0);

	ath79_eth1_data.phy_if_mode = PHY_INTERFACE_MODE_GMII;
	ath79_eth1_data.duplex = DUPLEX_FULL;
	ath79_switch_data.phy_poll_mask |= BIT(4);
	ath79_init_mac(ath79_eth1_data.mac_addr, art, 0);
	ath79_register_eth(1);
}

MIPS_MACHINE(ATH79_MACH_X_AP1200, "X-AP1200", "X AP1200 board", x_ap1200_setup);

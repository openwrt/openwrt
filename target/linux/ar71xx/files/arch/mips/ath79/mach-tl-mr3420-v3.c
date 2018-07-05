/*
 *  TP-LINK TL-MR3420 v3 board support
 *
 *  Copyright (C) 2018 Lutfa Ibtihaji Ilham <lutfailham96@gmail.com>
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 */

#include <linux/gpio.h>
#include <linux/platform_device.h>

#include <asm/mach-ath79/ath79.h>
#include <asm/mach-ath79/ar71xx_regs.h>

#include "common.h"
#include "dev-eth.h"
#include "dev-gpio-buttons.h"
#include "dev-leds-gpio.h"
#include "dev-m25p80.h"
#include "dev-usb.h"
#include "dev-wmac.h"
#include "machtypes.h"

#define DRIVER_NAME "leds-tl-mr3420-v3"
#define TL_MR3420V3_GPIO_USB_POWER	0
#define TL_MR3420V3_GPIO_LED_SYSTEM	1
#define TL_MR3420V3_GPIO_LED_QSS	3

#define TL_MR3420V3_GPIO_BTN_RESET	12
#define TL_MR3420V3_GPIO_BTN_WIFI	17

#define TL_MR3420V3_KEYS_POLL_INTERVAL	20	/* msecs */
#define TL_MR3420V3_KEYS_DEBOUNCE_INTERVAL (3 * TL_MR3420V3_KEYS_POLL_INTERVAL)

static const char *tl_mr3420_v3_part_probes[] = {
	"tp-link",
	NULL,
};

static struct flash_platform_data tl_mr3420_v3_flash_data = {
	.part_probes	= tl_mr3420_v3_part_probes,
};

static struct gpio_led tl_mr3420_v3_leds_gpio[] __initdata = {
	{
		.name		= "tp-link:green:system",
		.gpio		= TL_MR3420V3_GPIO_LED_SYSTEM,
		.active_low	= 1,
	}, {
		.name		= "tp-link:green:qss",
		.gpio		= TL_MR3420V3_GPIO_LED_QSS,
		.active_low	= 1,
	}
};

static struct gpio_keys_button tl_mr3420_v3_gpio_keys[] __initdata = {
	{
		.desc		= "Reset button",
		.type		= EV_KEY,
		.code		= KEY_RESTART,
		.debounce_interval = TL_MR3420V3_KEYS_DEBOUNCE_INTERVAL,
		.gpio		= TL_MR3420V3_GPIO_BTN_RESET,
		.active_low	= 1,
	}, {
		.desc		= "WIFI button",
		.type		= EV_KEY,
		.code		= KEY_RFKILL,
		.debounce_interval = TL_MR3420V3_KEYS_DEBOUNCE_INTERVAL,
		.gpio		= TL_MR3420V3_GPIO_BTN_WIFI,
		.active_low	= 1,
	}
};

static void __init tl_ap143_setup(void)
{
	u8 *mac = (u8 *) KSEG1ADDR(0x1f01fc00);
	u8 *ee = (u8 *) KSEG1ADDR(0x1fff1000);
	u8 tmpmac[ETH_ALEN];

	ath79_register_m25p80(&tl_mr3420_v3_flash_data);

	ath79_setup_ar933x_phy4_switch(false, false);

	ath79_register_mdio(0, 0x0);

	/* LAN */
	ath79_eth1_data.phy_if_mode = PHY_INTERFACE_MODE_GMII;
	ath79_eth1_data.duplex = DUPLEX_FULL;
	ath79_switch_data.phy_poll_mask |= BIT(4);
	ath79_init_mac(ath79_eth1_data.mac_addr, mac, 0);
	ath79_register_eth(1);

	/* WAN */
	ath79_switch_data.phy4_mii_en = 1;
	ath79_eth0_data.phy_if_mode = PHY_INTERFACE_MODE_MII;
	ath79_eth0_data.duplex = DUPLEX_FULL;
	ath79_eth0_data.speed = SPEED_100;
	ath79_eth0_data.phy_mask = BIT(4);
	ath79_init_mac(ath79_eth0_data.mac_addr, mac, 1);
	ath79_register_eth(0);

	ath79_init_mac(tmpmac, mac, 0);
	ath79_register_wmac(ee, tmpmac);
}

static void __init tl_mr3420_v3_usb_setup(void)
{
	/* enable power for the USB port */
	gpio_request_one(TL_MR3420V3_GPIO_USB_POWER,
			 GPIOF_OUT_INIT_HIGH | GPIOF_EXPORT_DIR_FIXED,
			 "USB power");
	ath79_register_usb();
}

static void __init tl_mr3420_v3_setup(void)
{
	tl_ap143_setup();

	ath79_register_leds_gpio(-1, ARRAY_SIZE(tl_mr3420_v3_leds_gpio),
				 tl_mr3420_v3_leds_gpio);

	ath79_register_gpio_keys_polled(1, TL_MR3420V3_KEYS_POLL_INTERVAL,
					ARRAY_SIZE(tl_mr3420_v3_gpio_keys),
					tl_mr3420_v3_gpio_keys);
	tl_mr3420_v3_usb_setup();
	platform_device_register_simple(DRIVER_NAME, -1, NULL, 0);
}

MIPS_MACHINE(ATH79_MACH_TL_MR3420_V3, "TL-MR3420-v3", "TP-LINK TL-MR3420 v3",
	     tl_mr3420_v3_setup);

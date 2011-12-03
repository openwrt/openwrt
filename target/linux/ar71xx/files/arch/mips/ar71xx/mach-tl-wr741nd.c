/*
 *  TP-LINK TL-WR741ND board support
 *
 *  Copyright (C) 2009-2010 Gabor Juhos <juhosg@openwrt.org>
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 */

#include <asm/mach-ar71xx/ar71xx.h>

#include "machtype.h"
#include "devices.h"
#include "dev-m25p80.h"
#include "dev-ap91-pci.h"
#include "dev-gpio-buttons.h"
#include "dev-leds-gpio.h"

#define TL_WR741ND_GPIO_LED_QSS		0
#define TL_WR741ND_GPIO_LED_SYSTEM	1

#define TL_WR741ND_GPIO_BTN_RESET	11
#define TL_WR741ND_GPIO_BTN_QSS		12

#define TL_WR741ND_KEYS_POLL_INTERVAL	20	/* msecs */
#define TL_WR741ND_KEYS_DEBOUNCE_INTERVAL (3 * TL_WR741ND_KEYS_POLL_INTERVAL)

static const char *tl_wr741nd_part_probes[] = {
	"tp-link",
	NULL,
};

static struct flash_platform_data tl_wr741nd_flash_data = {
	.part_probes	= tl_wr741nd_part_probes,
};

static struct gpio_led tl_wr741nd_leds_gpio[] __initdata = {
	{
		.name		= "tp-link:green:system",
		.gpio		= TL_WR741ND_GPIO_LED_SYSTEM,
		.active_low	= 1,
	}, {
		.name		= "tp-link:green:qss",
		.gpio		= TL_WR741ND_GPIO_LED_QSS,
		.active_low	= 1,
	}
};

static struct gpio_keys_button tl_wr741nd_gpio_keys[] __initdata = {
	{
		.desc		= "reset",
		.type		= EV_KEY,
		.code		= KEY_RESTART,
		.debounce_interval = TL_WR741ND_KEYS_DEBOUNCE_INTERVAL,
		.gpio		= TL_WR741ND_GPIO_BTN_RESET,
		.active_low	= 1,
	}, {
		.desc		= "qss",
		.type		= EV_KEY,
		.code		= KEY_WPS_BUTTON,
		.debounce_interval = TL_WR741ND_KEYS_DEBOUNCE_INTERVAL,
		.gpio		= TL_WR741ND_GPIO_BTN_QSS,
		.active_low	= 1,
	}
};

static void __init tl_wr741nd_setup(void)
{
	u8 *mac = (u8 *) KSEG1ADDR(0x1f01fc00);
	u8 *ee = (u8 *) KSEG1ADDR(0x1fff1000);

	ar71xx_add_device_m25p80(&tl_wr741nd_flash_data);

	ar71xx_add_device_leds_gpio(-1, ARRAY_SIZE(tl_wr741nd_leds_gpio),
					tl_wr741nd_leds_gpio);

	ar71xx_register_gpio_keys_polled(-1, TL_WR741ND_KEYS_POLL_INTERVAL,
					 ARRAY_SIZE(tl_wr741nd_gpio_keys),
					 tl_wr741nd_gpio_keys);

	ar71xx_init_mac(ar71xx_eth0_data.mac_addr, mac, 0);
	ar71xx_init_mac(ar71xx_eth1_data.mac_addr, mac, 1);

	ar71xx_add_device_mdio(0, 0x0);

	/* LAN ports */
	ar71xx_add_device_eth(1);

	/* WAN port */
	ar71xx_add_device_eth(0);

	ap91_pci_setup_wmac_led_pin(1);

	ap91_pci_init(ee, mac);
}
MIPS_MACHINE(AR71XX_MACH_TL_WR741ND, "TL-WR741ND", "TP-LINK TL-WR741ND",
	     tl_wr741nd_setup);

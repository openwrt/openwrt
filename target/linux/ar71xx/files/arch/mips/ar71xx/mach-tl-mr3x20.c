/*
 *  TP-LINK TL-MR3220/3420 board support
 *
 *  Copyright (C) 2010 Gabor Juhos <juhosg@openwrt.org>
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 */

#include <linux/gpio.h>

#include <asm/mach-ar71xx/ar71xx.h>

#include "machtype.h"
#include "devices.h"
#include "dev-m25p80.h"
#include "dev-ap91-pci.h"
#include "dev-gpio-buttons.h"
#include "dev-leds-gpio.h"
#include "dev-usb.h"

#define TL_MR3X20_GPIO_LED_QSS		0
#define TL_MR3X20_GPIO_LED_SYSTEM	1
#define TL_MR3X20_GPIO_LED_3G		8

#define TL_MR3X20_GPIO_BTN_RESET	11
#define TL_MR3X20_GPIO_BTN_QSS		12

#define TL_MR3X20_GPIO_USB_POWER	6

#define TL_MR3X20_KEYS_POLL_INTERVAL	20	/* msecs */
#define TL_MR3X20_KEYS_DEBOUNCE_INTERVAL (3 * TL_MR3X20_KEYS_POLL_INTERVAL)

static const char *tl_mr3x20_part_probes[] = {
	"tp-link",
	NULL,
};

static struct flash_platform_data tl_mr3x20_flash_data = {
	.part_probes	= tl_mr3x20_part_probes,
};

static struct gpio_led tl_mr3x20_leds_gpio[] __initdata = {
	{
		.name		= "tp-link:green:system",
		.gpio		= TL_MR3X20_GPIO_LED_SYSTEM,
		.active_low	= 1,
	}, {
		.name		= "tp-link:green:qss",
		.gpio		= TL_MR3X20_GPIO_LED_QSS,
		.active_low	= 1,
	}, {
		.name		= "tp-link:green:3g",
		.gpio		= TL_MR3X20_GPIO_LED_3G,
		.active_low	= 1,
	}
};

static struct gpio_keys_button tl_mr3x20_gpio_keys[] __initdata = {
	{
		.desc		= "reset",
		.type		= EV_KEY,
		.code		= KEY_RESTART,
		.debounce_interval = TL_MR3X20_KEYS_DEBOUNCE_INTERVAL,
		.gpio		= TL_MR3X20_GPIO_BTN_RESET,
		.active_low	= 1,
	}, {
		.desc		= "qss",
		.type		= EV_KEY,
		.code		= KEY_WPS_BUTTON,
		.debounce_interval = TL_MR3X20_KEYS_DEBOUNCE_INTERVAL,
		.gpio		= TL_MR3X20_GPIO_BTN_QSS,
		.active_low	= 1,
	}
};

static void __init tl_mr3x20_setup(void)
{
	u8 *mac = (u8 *) KSEG1ADDR(0x1f01fc00);
	u8 *ee = (u8 *) KSEG1ADDR(0x1fff1000);

	/* enable power for the USB port */
	gpio_request(TL_MR3X20_GPIO_USB_POWER, "USB power");
	gpio_direction_output(TL_MR3X20_GPIO_USB_POWER, 1);

	ar71xx_add_device_m25p80(&tl_mr3x20_flash_data);

	ar71xx_add_device_leds_gpio(-1, ARRAY_SIZE(tl_mr3x20_leds_gpio),
					tl_mr3x20_leds_gpio);

	ar71xx_register_gpio_keys_polled(-1, TL_MR3X20_KEYS_POLL_INTERVAL,
					 ARRAY_SIZE(tl_mr3x20_gpio_keys),
					 tl_mr3x20_gpio_keys);

	ar71xx_init_mac(ar71xx_eth0_data.mac_addr, mac, 1);
	ar71xx_init_mac(ar71xx_eth1_data.mac_addr, mac, -1);

	ar71xx_add_device_mdio(0, 0x0);

	/* LAN ports */
	ar71xx_add_device_eth(1);
	/* WAN port */
	ar71xx_add_device_eth(0);

	ar71xx_add_device_usb();

	ap91_pci_init(ee, mac);
}

static void __init tl_mr3220_setup(void)
{
	tl_mr3x20_setup();
	ap91_pci_setup_wmac_led_pin(1);
}

MIPS_MACHINE(AR71XX_MACH_TL_MR3220, "TL-MR3220", "TP-LINK TL-MR3220",
	     tl_mr3220_setup);

static void __init tl_mr3420_setup(void)
{
	tl_mr3x20_setup();
	ap91_pci_setup_wmac_led_pin(0);
}

MIPS_MACHINE(AR71XX_MACH_TL_MR3420, "TL-MR3420", "TP-LINK TL-MR3420",
	     tl_mr3420_setup);

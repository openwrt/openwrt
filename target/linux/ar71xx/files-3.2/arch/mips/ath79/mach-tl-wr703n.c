/*
 *  TP-LINK TL-WR703N board support
 *
 *  Copyright (C) 2011 dongyuqi <729650915@qq.com>
 *  Copyright (C) 2011-2012 Gabor Juhos <juhosg@openwrt.org>
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 */

#include <linux/gpio.h>

#include <asm/mach-ath79/ath79.h>

#include "dev-eth.h"
#include "dev-gpio-buttons.h"
#include "dev-leds-gpio.h"
#include "dev-m25p80.h"
#include "dev-usb.h"
#include "dev-wmac.h"
#include "machtypes.h"

#define TL_WR703N_GPIO_LED_SYSTEM	27
#define TL_WR703N_GPIO_BTN_RESET	11

#define TL_WR703N_GPIO_USB_POWER	8

#define TL_WR703N_KEYS_POLL_INTERVAL	20	/* msecs */
#define TL_WR703N_KEYS_DEBOUNCE_INTERVAL	(3 * TL_WR703N_KEYS_POLL_INTERVAL)

static const char *tl_wr703n_part_probes[] = {
	"tp-link",
	NULL,
};

static struct flash_platform_data tl_wr703n_flash_data = {
	.part_probes	= tl_wr703n_part_probes,
};

static struct gpio_led tl_wr703n_leds_gpio[] __initdata = {
	{
		.name		= "tp-link:blue:system",
		.gpio		= TL_WR703N_GPIO_LED_SYSTEM,
		.active_low	= 1,
	},
};

static struct gpio_keys_button tl_wr703n_gpio_keys[] __initdata = {
	{
		.desc		= "reset",
		.type		= EV_KEY,
		.code		= KEY_RESTART,
		.debounce_interval = TL_WR703N_KEYS_DEBOUNCE_INTERVAL,
		.gpio		= TL_WR703N_GPIO_BTN_RESET,
		.active_low	= 1,
	}
};

static void __init tl_wr703n_setup(void)
{
	u8 *mac = (u8 *) KSEG1ADDR(0x1f01fc00);
	u8 *ee = (u8 *) KSEG1ADDR(0x1fff1000);

	ath79_register_m25p80(&tl_wr703n_flash_data);
	ath79_register_leds_gpio(-1, ARRAY_SIZE(tl_wr703n_leds_gpio),
				 tl_wr703n_leds_gpio);
	ath79_register_gpio_keys_polled(-1, TL_WR703N_KEYS_POLL_INTERVAL,
					ARRAY_SIZE(tl_wr703n_gpio_keys),
					tl_wr703n_gpio_keys);

	gpio_request(TL_WR703N_GPIO_USB_POWER, "USB power");
	gpio_direction_output(TL_WR703N_GPIO_USB_POWER, 1);
	ath79_register_usb();

	ath79_init_mac(ath79_eth0_data.mac_addr, mac, 0);

	ath79_register_mdio(0, 0x0);
	ath79_register_eth(0);

	ath79_register_wmac(ee, mac);
}

MIPS_MACHINE(ATH79_MACH_TL_WR703N, "TL-WR703N", "TP-LINK TL-WR703N v1",
	     tl_wr703n_setup);

/*
 *  Pisen Cloud Easy Power board support
 *
 *  Based on mach-tl-wr703n.c, which is:
 *    Copyright (C) 2011 dongyuqi <729650915@qq.com>
 *    Copyright (C) 2011-2012 Gabor Juhos <juhosg@openwrt.org>
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

#define CLOUD_EASY_POWER_GPIO_LED_SYSTEM	0
#define CLOUD_EASY_POWER_GPIO_BTN_RESET		12

#define CLOUD_EASY_POWER_GPIO_USB_POWER		8

#define CLOUD_EASY_POWER_KEYS_POLL_INTERVAL	20	/* msecs */
#define CLOUD_EASY_POWER_KEYS_DEBOUNCE_INTERVAL	(3 * CLOUD_EASY_POWER_KEYS_POLL_INTERVAL)

static const char *cloud_easy_power_part_probes[] = {
	"tp-link",
	NULL,
};

static struct flash_platform_data cloud_easy_power_flash_data = {
	.part_probes	= cloud_easy_power_part_probes,
};

static struct gpio_led cloud_easy_power_leds_gpio[] __initdata = {
	{
		.name		= "pisen:blue:system",
		.gpio		= CLOUD_EASY_POWER_GPIO_LED_SYSTEM,
		.active_low	= 1,
	},
};

static struct gpio_keys_button cloud_easy_power_gpio_keys[] __initdata = {
	{
		.desc		= "reset",
		.type		= EV_KEY,
		.code		= KEY_RESTART,
		.debounce_interval = CLOUD_EASY_POWER_KEYS_DEBOUNCE_INTERVAL,
		.gpio		= CLOUD_EASY_POWER_GPIO_BTN_RESET,
		.active_low	= 0,
	}
};

static void __init cloud_easy_power_setup(void)
{
	u8 *mac = (u8 *) KSEG1ADDR(0x1f01fc00);
	u8 *ee = (u8 *) KSEG1ADDR(0x1fff1000);

	/* disable PHY_SWAP and PHY_ADDR_SWAP bits */
	ath79_setup_ar933x_phy4_switch(false, false);

	ath79_register_m25p80(&cloud_easy_power_flash_data);
	ath79_register_leds_gpio(-1, ARRAY_SIZE(cloud_easy_power_leds_gpio),
				 cloud_easy_power_leds_gpio);
	ath79_register_gpio_keys_polled(-1, CLOUD_EASY_POWER_KEYS_POLL_INTERVAL,
					ARRAY_SIZE(cloud_easy_power_gpio_keys),
					cloud_easy_power_gpio_keys);

	gpio_request_one(CLOUD_EASY_POWER_GPIO_USB_POWER,
			 GPIOF_OUT_INIT_HIGH | GPIOF_EXPORT_DIR_FIXED,
			 "USB power");
	ath79_register_usb();

	ath79_init_mac(ath79_eth0_data.mac_addr, mac, 0);

	ath79_register_mdio(0, 0x0);
	ath79_register_eth(0);

	ath79_register_wmac(ee, mac);
}

MIPS_MACHINE(ATH79_MACH_CLOUD_EASY_POWER, "CLOUD-EASY-POWER", "Pisen Cloud Easy Power",
	     cloud_easy_power_setup);

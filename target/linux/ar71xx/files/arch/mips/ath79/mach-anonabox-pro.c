/*
 * Atheros ANONABOX_PRO reference board support
 *
 * Copyright (c) 2013 The Linux Foundation. All rights reserved.
 * Copyright (c) 2012 Gabor Juhos <juhosg@openwrt.org>
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 */

#include <linux/platform_device.h>
#include <linux/ath9k_platform.h>
#include <linux/ar8216_platform.h>

#include <asm/mach-ath79/ar71xx_regs.h>

#include "common.h"
#include "dev-eth.h"
#include "dev-gpio-buttons.h"
#include "dev-leds-gpio.h"
#include "dev-m25p80.h"
#include "dev-spi.h"
#include "dev-usb.h"
#include "dev-wmac.h"
#include "machtypes.h"

#define ANONABOX_PRO_GPIO_LED_STATUS              14

#define ANONABOX_PRO_GPIO_BTN_RST                 17

#define ANONABOX_PRO_KEYS_POLL_INTERVAL           20	/* msecs */
#define ANONABOX_PRO_KEYS_DEBOUNCE_INTERVAL       (3 * ANONABOX_PRO_KEYS_POLL_INTERVAL)

#define ANONABOX_PRO_MAC0_OFFSET                  0
#define ANONABOX_PRO_MAC1_OFFSET                  6
#define ANONABOX_PRO_WMAC_CALDATA_OFFSET          0x1000

static struct gpio_led anonabox_pro_leds_gpio[] __initdata = {
	{
		.name		= "anonabox_pro:green:status",
		.gpio		= ANONABOX_PRO_GPIO_LED_STATUS,
		.active_low	= 1,
	},
};

static struct gpio_keys_button anonabox_pro_gpio_keys[] __initdata = {
	{
		.desc		= "reset button",
		.type		= EV_KEY,
		.code		= KEY_RESTART,
		.debounce_interval = ANONABOX_PRO_KEYS_DEBOUNCE_INTERVAL,
		.gpio		= ANONABOX_PRO_GPIO_BTN_RST,
		.active_low	= 1,
	},
};

static void __init anonabox_pro_gpio_led_setup(void)
{
	ath79_register_leds_gpio(-1, ARRAY_SIZE(anonabox_pro_leds_gpio),
			anonabox_pro_leds_gpio);
	ath79_register_gpio_keys_polled(-1, ANONABOX_PRO_KEYS_POLL_INTERVAL,
			ARRAY_SIZE(anonabox_pro_gpio_keys),
			anonabox_pro_gpio_keys);
}

static void __init anonabox_pro_setup(void)
{
	u8 *art = (u8 *) KSEG1ADDR(0x1fff0000);

	ath79_register_m25p80(NULL);

	anonabox_pro_gpio_led_setup();

	ath79_register_usb();

	ath79_register_wmac(art + ANONABOX_PRO_WMAC_CALDATA_OFFSET, NULL);

	ath79_register_mdio(0, 0x0);
	ath79_register_mdio(1, 0x0);

	ath79_init_mac(ath79_eth0_data.mac_addr, art + ANONABOX_PRO_MAC0_OFFSET, 0);
	ath79_init_mac(ath79_eth1_data.mac_addr, art + ANONABOX_PRO_MAC1_OFFSET, 0);

	/* WAN port */
	ath79_eth0_data.phy_if_mode = PHY_INTERFACE_MODE_MII;
	ath79_eth0_data.speed = SPEED_100;
	ath79_eth0_data.duplex = DUPLEX_FULL;
	ath79_eth0_data.phy_mask = BIT(4);
	ath79_register_eth(0);

	/* LAN ports */
	ath79_eth1_data.phy_if_mode = PHY_INTERFACE_MODE_GMII;
	ath79_eth1_data.speed = SPEED_1000;
	ath79_eth1_data.duplex = DUPLEX_FULL;
	ath79_switch_data.phy_poll_mask |= BIT(4);
	ath79_switch_data.phy4_mii_en = 1;
	ath79_register_eth(1);
}

MIPS_MACHINE(ATH79_MACH_ANONABOX_PRO, "ANONABOX_PRO", "Anonabox Pro board",
	     anonabox_pro_setup);

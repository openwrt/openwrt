/*
 *  NETGEAR WNR2000v5 board support
 *
 * Copyright (c) 2017 Raphael Catolino <raphael.catolino@gmail.com>
 * Copyright (c) 2013-2015 The Linux Foundation. All rights reserved.
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

#include <linux/gpio.h>
#include <linux/platform_device.h>
#include <linux/ath9k_platform.h>
#include <linux/spi/74x164.h>
#include <linux/spi/spi.h>
#include <linux/spi/spi_gpio.h>

#include <asm/mach-ath79/ath79.h>
#include <asm/mach-ath79/ath79_spi_platform.h>
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

#define WNR2000V5_GPIO_LED_STATUS_GREEN 21
#define WNR2000V5_GPIO_LED_STATUS_AMBER 20
#define WNR2000V5_GPIO_LED_DATA_GREEN   25
#define WNR2000V5_GPIO_LED_DATA_AMBER   19
#define WNR2000V5_GPIO_LED_WLAN         18

#define WNR2000V5_GPIO_BTN_RFKILL 0
#define WNR2000V5_GPIO_BTN_RESET  1
#define WNR2000V5_GPIO_BTN_WPS    2

#define WNR2000V5_KEYS_POLL_INTERVAL	20	/* msecs */
#define WNR2000V5_KEYS_DEBOUNCE_INTERVAL	(4 * WNR2000V5_KEYS_POLL_INTERVAL)

#define WNR2000V5_SPI_GPIO_RESET 14
#define WNR2000V5_SPI_GPIO_MOSI 15
#define WNR2000V5_SPI_GPIO_CLK 16
/* The QCA953X platform has QCA953X_GPIO_COUNT GPIOs, the 74x164 muxer adds 8
 * additional GPIOs on this board. */
#define WNR2000V5_74X164_GPIO_BASE QCA953X_GPIO_COUNT


static struct gpio_led wnr2000v5_leds_gpio[] __initdata = {
	{
		.name		= "netgear:green:status",
		.gpio		= WNR2000V5_GPIO_LED_STATUS_GREEN,
		.active_low	= 1,
		.default_trigger = "default-on",
	},
	{
		.name		= "netgear:amber:status",
		.gpio		= WNR2000V5_GPIO_LED_STATUS_AMBER,
		.active_low	= 1,
	},
	{
		.name		= "netgear:green:wlan",
		.gpio		= WNR2000V5_GPIO_LED_WLAN,
		.active_low	= 1,
	},
	{
		.name		= "netgear:green:wan",
		.gpio		= WNR2000V5_GPIO_LED_DATA_GREEN,
		.active_low	= 1,
	},
	{
		.name		= "netgear:amber:wan",
		.gpio		= WNR2000V5_GPIO_LED_DATA_AMBER,
		.active_low	= 1,
	},
};

static struct gpio_keys_button wnr2000v5_gpio_keys[] __initdata = {
	{
		.desc		= "WPS button",
		.type		= EV_KEY,
		.code		= KEY_WPS_BUTTON,
		.debounce_interval = WNR2000V5_KEYS_DEBOUNCE_INTERVAL,
		.gpio		= WNR2000V5_GPIO_BTN_WPS,
		.active_low	= 1,
	},
	{
		.desc		= "rfkill",
		.type		= EV_KEY,
		.code		= KEY_RFKILL,
		.debounce_interval = WNR2000V5_KEYS_DEBOUNCE_INTERVAL,
		.gpio		= WNR2000V5_GPIO_BTN_RFKILL,
		.active_low	= 1,
	},
	{
		.desc		= "reset",
		.type		= EV_KEY,
		.code		= KEY_RESTART,
		.debounce_interval = WNR2000V5_KEYS_DEBOUNCE_INTERVAL,
		.gpio		= WNR2000V5_GPIO_BTN_RESET,
		.active_low	= 1,
	},
};

static void __init wnr2000v5_gpio_led_setup(void)
{
	ath79_gpio_direction_select(WNR2000V5_GPIO_LED_STATUS_GREEN, true);
	ath79_gpio_direction_select(WNR2000V5_GPIO_LED_STATUS_AMBER, true);
	ath79_gpio_direction_select(WNR2000V5_GPIO_LED_WLAN, true);
	ath79_gpio_direction_select(WNR2000V5_GPIO_LED_DATA_GREEN, true);
	ath79_gpio_direction_select(WNR2000V5_GPIO_LED_DATA_AMBER, true);

	ath79_register_leds_gpio(-1, ARRAY_SIZE(wnr2000v5_leds_gpio),
			wnr2000v5_leds_gpio);

	ath79_register_gpio_keys_polled(-1, WNR2000V5_KEYS_POLL_INTERVAL,
			ARRAY_SIZE(wnr2000v5_gpio_keys),
			wnr2000v5_gpio_keys);
}

static u8 wnr2000v5_ssr_initdata[] __initdata = {
	BIT(7) | BIT(6) | BIT(4) | BIT(3) | BIT(2) | BIT(1) | BIT(0),
};

static struct spi_gpio_platform_data wnr2000v5_spi_gpio_data = {
	.sck		= WNR2000V5_SPI_GPIO_CLK,
	.mosi		= WNR2000V5_SPI_GPIO_MOSI,
	.miso		= SPI_GPIO_NO_MISO,
	.num_chipselect = 1,
};

static struct platform_device wnr2000v5_spi_gpio_device = {
	.name		= "spi_gpio",
	.id		= 1,
	.dev = {
		.platform_data = &wnr2000v5_spi_gpio_data,
	},
};

static struct gen_74x164_chip_platform_data wnr2000v5_ssr_data = {
	.base = WNR2000V5_74X164_GPIO_BASE,
	.num_registers = ARRAY_SIZE(wnr2000v5_ssr_initdata),
	.init_data = wnr2000v5_ssr_initdata,
};

static struct spi_board_info wnr2000v5_spi_gpio_info[] = {
	{
		.bus_num	= 1,
		.chip_select	= 0,
		.max_speed_hz	= 400000,
		.modalias	= "74x164",
		.controller_data = (void *) SPI_GPIO_NO_CHIPSELECT,
		.platform_data = &wnr2000v5_ssr_data,
	},
};

static void __init WNR2000V5_setup(void)
{
	u8 *art = (u8 *) KSEG1ADDR(0x1fff0000);

	/* SPI NOR initialization */
	ath79_register_m25p80(NULL);

	/* GPIOS and LEDS */
	spi_register_board_info(wnr2000v5_spi_gpio_info, ARRAY_SIZE(wnr2000v5_spi_gpio_info));
	platform_device_register(&wnr2000v5_spi_gpio_device);
	wnr2000v5_gpio_led_setup();
	ath79_register_wmac(art + 0x1000, NULL);
	ath79_register_mdio(0, 0x0);

	/* Ethernet and switch */
	ath79_init_mac(ath79_eth0_data.mac_addr, art + 0, 0);
	ath79_init_mac(ath79_eth1_data.mac_addr, art + 6, 0);

	ath79_eth0_data.phy_if_mode = PHY_INTERFACE_MODE_MII;
	ath79_eth0_data.speed = SPEED_100;
	ath79_eth0_data.duplex = DUPLEX_FULL;
	ath79_eth0_data.phy_mask = BIT(4);
	ath79_register_eth(0);

	ath79_eth1_data.phy_if_mode = PHY_INTERFACE_MODE_GMII;
	ath79_eth1_data.speed = SPEED_1000;
	ath79_eth1_data.duplex = DUPLEX_FULL;
	ath79_switch_data.phy_poll_mask |= BIT(4);
	ath79_switch_data.phy4_mii_en = 1;
	ath79_register_eth(1);
}

MIPS_MACHINE(ATH79_MACH_WNR2000_V5, "WNR2000V5", "Netgear WNR2000 V5", WNR2000V5_setup);

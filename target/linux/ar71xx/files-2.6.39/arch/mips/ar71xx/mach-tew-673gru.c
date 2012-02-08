/*
 *  TRENDnet TEW-673GRU board support
 *
 *  Copyright (C) 2012 Gabor Juhos <juhosg@openwrt.org>
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 */

#include <linux/platform_device.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/partitions.h>
#include <linux/delay.h>
#include <linux/rtl8366.h>
#include <linux/spi/spi.h>
#include <linux/spi/spi_gpio.h>

#include <asm/mach-ar71xx/ar71xx.h>

#include "machtype.h"
#include "devices.h"
#include "dev-m25p80.h"
#include "dev-ap94-pci.h"
#include "dev-gpio-buttons.h"
#include "dev-leds-gpio.h"
#include "dev-usb.h"

#define TEW673GRU_GPIO_LCD_SCK		0
#define TEW673GRU_GPIO_LCD_MOSI		1
#define TEW673GRU_GPIO_LCD_MISO		2
#define TEW673GRU_GPIO_LCD_CS		6

#define TEW673GRU_GPIO_LED_WPS		9

#define TEW673GRU_GPIO_BTN_RESET	3
#define TEW673GRU_GPIO_BTN_WPS		8

#define TEW673GRU_GPIO_RTL8366_SDA	5
#define TEW673GRU_GPIO_RTL8366_SCK	7

#define TEW673GRU_KEYS_POLL_INTERVAL	20 /* msecs */
#define TEW673GRU_KEYS_DEBOUNCE_INTERVAL (3 * TEW673GRU_KEYS_POLL_INTERVAL)

#define TEW673GRU_CAL_LOCATION_0	0x1f661000
#define TEW673GRU_CAL_LOCATION_1	0x1f665000
#define TEW673GRU_MAC_LOCATION_0	0x1f66ffa0
#define TEW673GRU_MAC_LOCATION_1	0x1f66ffb4

#ifdef CONFIG_MTD_PARTITIONS
static struct mtd_partition tew673gru_partitions[] = {
	{
		.name		= "uboot",
		.offset		= 0,
		.size		= 0x040000,
		.mask_flags	= MTD_WRITEABLE,
	}, {
		.name		= "config",
		.offset		= 0x040000,
		.size		= 0x010000,
		.mask_flags	= MTD_WRITEABLE,
	}, {
		.name		= "firmware",
		.offset		= 0x050000,
		.size		= 0x610000,
	}, {
		.name		= "caldata",
		.offset		= 0x660000,
		.size		= 0x010000,
		.mask_flags	= MTD_WRITEABLE,
	}, {
		.name		= "unknown",
		.offset		= 0x670000,
		.size		= 0x190000,
		.mask_flags	= MTD_WRITEABLE,
	}
};
#endif /* CONFIG_MTD_PARTITIONS */

static struct flash_platform_data tew673gru_flash_data = {
#ifdef CONFIG_MTD_PARTITIONS
	.parts          = tew673gru_partitions,
	.nr_parts       = ARRAY_SIZE(tew673gru_partitions),
#endif
};

static struct gpio_led tew673gru_leds_gpio[] __initdata = {
	{
		.name		= "trendnet:blue:wps",
		.gpio		= TEW673GRU_GPIO_LED_WPS,
		.active_low	= 1,
	}
};

static struct gpio_keys_button tew673gru_gpio_keys[] __initdata = {
	{
		.desc		= "reset",
		.type		= EV_KEY,
		.code		= KEY_RESTART,
		.debounce_interval = TEW673GRU_KEYS_DEBOUNCE_INTERVAL,
		.gpio		= TEW673GRU_GPIO_BTN_RESET,
		.active_low	= 1,
	}, {
		.desc		= "wps",
		.type		= EV_KEY,
		.code		= KEY_WPS_BUTTON,
		.debounce_interval = TEW673GRU_KEYS_DEBOUNCE_INTERVAL,
		.gpio		= TEW673GRU_GPIO_BTN_WPS,
		.active_low	= 1,
	}
};

static struct rtl8366_initval tew673gru_rtl8366s_initvals[] = {
	{ .reg = 0x06, .val = 0x0108 },
};

static struct rtl8366_platform_data tew673gru_rtl8366s_data = {
	.gpio_sda	= TEW673GRU_GPIO_RTL8366_SDA,
	.gpio_sck	= TEW673GRU_GPIO_RTL8366_SCK,
	.num_initvals	= ARRAY_SIZE(tew673gru_rtl8366s_initvals),
	.initvals	= tew673gru_rtl8366s_initvals,
};

static struct platform_device tew673gru_rtl8366s_device = {
	.name		= RTL8366S_DRIVER_NAME,
	.id		= -1,
	.dev = {
		.platform_data	= &tew673gru_rtl8366s_data,
	}
};

static struct spi_board_info tew673gru_spi_info[] = {
	{
		.bus_num	= 1,
		.chip_select	= 0,
		.max_speed_hz	= 400000,
		.modalias	= "spidev",
		.mode		= SPI_MODE_2,
		.controller_data = (void *) TEW673GRU_GPIO_LCD_CS,
	},
};

static struct spi_gpio_platform_data tew673gru_spi_data = {
	.sck		= TEW673GRU_GPIO_LCD_SCK,
	.miso		= TEW673GRU_GPIO_LCD_MISO,
	.mosi		= TEW673GRU_GPIO_LCD_MOSI,
	.num_chipselect = 1,
};

static struct platform_device tew673gru_spi_device = {
	.name		= "spi_gpio",
	.id		= 1,
	.dev = {
		.platform_data = &tew673gru_spi_data,
	},
};

static void tew673gru_read_ascii_mac(u8 *dest, unsigned int src_addr)
{
	int ret;
	u8 *src = (u8 *)KSEG1ADDR(src_addr);

	ret = sscanf(src, "%02hhx:%02hhx:%02hhx:%02hhx:%02hhx:%02hhx",
		     &dest[0], &dest[1], &dest[2],
		     &dest[3], &dest[4], &dest[5]);

	if (ret != ETH_ALEN) memset(dest, 0, ETH_ALEN);
}

static void __init tew673gru_setup(void)
{
	u8 mac1[ETH_ALEN], mac2[ETH_ALEN];

	tew673gru_read_ascii_mac(mac1, TEW673GRU_MAC_LOCATION_0);
	tew673gru_read_ascii_mac(mac2, TEW673GRU_MAC_LOCATION_1);

	ar71xx_add_device_mdio(0, 0x0);

	ar71xx_init_mac(ar71xx_eth0_data.mac_addr, mac1, 2);
	ar71xx_eth0_data.mii_bus_dev = &tew673gru_rtl8366s_device.dev;
	ar71xx_eth0_data.phy_if_mode = PHY_INTERFACE_MODE_RGMII;
	ar71xx_eth0_data.speed = SPEED_1000;
	ar71xx_eth0_data.duplex = DUPLEX_FULL;
	ar71xx_eth0_pll_data.pll_1000 = 0x11110000;

	ar71xx_init_mac(ar71xx_eth1_data.mac_addr, mac1, 3);
	ar71xx_eth1_data.mii_bus_dev = &tew673gru_rtl8366s_device.dev;
	ar71xx_eth1_data.phy_if_mode = PHY_INTERFACE_MODE_RGMII;
	ar71xx_eth1_data.phy_mask = 0x10;
	ar71xx_eth1_pll_data.pll_1000 = 0x11110000;

	ar71xx_add_device_eth(0);
	ar71xx_add_device_eth(1);

	ar71xx_add_device_m25p80(&tew673gru_flash_data);

	ar71xx_add_device_leds_gpio(-1, ARRAY_SIZE(tew673gru_leds_gpio),
					tew673gru_leds_gpio);

	ar71xx_register_gpio_keys_polled(-1, TEW673GRU_KEYS_POLL_INTERVAL,
					 ARRAY_SIZE(tew673gru_gpio_keys),
					 tew673gru_gpio_keys);

	ar71xx_add_device_usb();

	platform_device_register(&tew673gru_rtl8366s_device);

	ap94_pci_setup_wmac_led_pin(0, 5);
	ap94_pci_setup_wmac_led_pin(1, 5);

	ap94_pci_init((u8 *) KSEG1ADDR(TEW673GRU_CAL_LOCATION_0), mac1,
		      (u8 *) KSEG1ADDR(TEW673GRU_CAL_LOCATION_1), mac2);

	spi_register_board_info(tew673gru_spi_info,
				ARRAY_SIZE(tew673gru_spi_info));
	platform_device_register(&tew673gru_spi_device);
}

MIPS_MACHINE(AR71XX_MACH_TEW_673GRU, "TEW-673GRU", "TRENDnet TEW-673GRU",
	     tew673gru_setup);

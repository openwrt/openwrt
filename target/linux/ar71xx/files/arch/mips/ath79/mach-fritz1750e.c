/*
 *  AVM FRITZ!WLAN Repeater 1750E board support
 *
 *  Copyright (C) 2018 David Bauer <mail@david-bauer.net>
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 */

#include <linux/init.h>
#include <linux/ath9k_platform.h>
#include <linux/etherdevice.h>
#include <linux/gpio.h>
#include <linux/platform_device.h>

#include <linux/mtd/mtd.h>
#include <linux/mtd/partitions.h>
#include <linux/platform_data/mdio-gpio.h>
#include <linux/platform_data/phy-at803x.h>
#include <linux/spi/spi_gpio.h>
#include <linux/spi/74x164.h>

#include <asm/mach-ath79/ath79.h>
#include <asm/mach-ath79/ar71xx_regs.h>

#include "common.h"
#include "dev-eth.h"
#include "dev-gpio-buttons.h"
#include "dev-leds-gpio.h"
#include "dev-m25p80.h"
#include "dev-spi.h"
#include "dev-wmac.h"
#include "machtypes.h"
#include "pci.h"


#define FRITZ1750E_GPIO_SHIFT_SER		15   /* DS,   Data Serial Input */
#define FRITZ1750E_GPIO_SHIFT_SRCLK		14 /* SHCP, Shift Reg Clock Input */

#define FRITZ1750E_SSR_BIT_0			0
#define FRITZ1750E_SSR_BIT_1			1
#define FRITZ1750E_SSR_BIT_2			2
#define FRITZ1750E_SSR_BIT_3			3
#define FRITZ1750E_SSR_BIT_4			4
#define FRITZ1750E_SSR_BIT_5			5
#define FRITZ1750E_SSR_BIT_6			6
#define FRITZ1750E_SSR_BIT_7			7

#define FRITZ1750E_74HC_GPIO_BASE		32
#define FRITZ1750E_74HC_GPIO_LED_RSSI0		(FRITZ1750E_74HC_GPIO_BASE + 0)
#define FRITZ1750E_74HC_GPIO_LED_RSSI1		(FRITZ1750E_74HC_GPIO_BASE + 1)
#define FRITZ1750E_74HC_GPIO_LED_RSSI2		(FRITZ1750E_74HC_GPIO_BASE + 2)
#define FRITZ1750E_74HC_GPIO_LED_RSSI3		(FRITZ1750E_74HC_GPIO_BASE + 3)
#define FRITZ1750E_74HC_GPIO_LED_RSSI4		(FRITZ1750E_74HC_GPIO_BASE + 4)
#define FRITZ1750E_74HC_GPIO_LED_WLAN		(FRITZ1750E_74HC_GPIO_BASE + 5)
#define FRITZ1750E_74HC_GPIO_LED_POWER		(FRITZ1750E_74HC_GPIO_BASE + 6)

#define FRITZ1750E_GPIO_LED_LAN			13

#define FRITZ1750E_GPIO_BTN_WPS			4
#define FRITZ1750E_KEYS_POLL_INTERVAL		20 /* msecs */
#define FRITZ1750E_KEYS_DEBOUNCE_INTERVAL	(3 * FRITZ1750E_KEYS_POLL_INTERVAL)

#define FRITZ1750E_PHY_ADDRESS			0
#define FRITZ1750E_GPIO_PHY_RESET		11
#define FRITZ1750E_GPIO_MDIO_CLK		12
#define FRITZ1750E_GPIO_MDIO_DATA		19

#define FRITZ1750E_GPIO_PEREGRINE_RESET		17
#define FRITZ1750E_GPIO_PCIE_RESET		18



static struct spi_gpio_platform_data fritz1750e_spi_data = {
	.sck		= FRITZ1750E_GPIO_SHIFT_SRCLK,
	.miso		= SPI_GPIO_NO_MISO,
	.mosi		= FRITZ1750E_GPIO_SHIFT_SER,
	.num_chipselect	= 1,
};

static u8 fritz1750e_ssr_initdata[] = {
	BIT(FRITZ1750E_SSR_BIT_7) |
	BIT(FRITZ1750E_SSR_BIT_6) |
	BIT(FRITZ1750E_SSR_BIT_5) |
	BIT(FRITZ1750E_SSR_BIT_4) |
	BIT(FRITZ1750E_SSR_BIT_3) |
	BIT(FRITZ1750E_SSR_BIT_2) |
	BIT(FRITZ1750E_SSR_BIT_1)
};

static struct gen_74x164_chip_platform_data fritz1750e_ssr_data = {
	.base		= FRITZ1750E_74HC_GPIO_BASE,
	.num_registers	= ARRAY_SIZE(fritz1750e_ssr_initdata),
	.init_data	= fritz1750e_ssr_initdata,
};

static struct platform_device fritz1750e_spi_device = {
	.name		= "spi_gpio",
	.id		= 1,
	.dev = {
		.platform_data = &fritz1750e_spi_data,
	},
};

static struct spi_board_info fritz1750e_spi_info[] = {
	{
		.bus_num		= 1,
		.chip_select		= 0,
		.max_speed_hz		= 10000000,
		.modalias		= "74x164",
		.platform_data		= &fritz1750e_ssr_data,
		.controller_data	= (void *) 0x0,
	},
};

static struct mtd_partition fritz1750e_flash_partitions[] = {
	{
		.name		= "urlader",
		.offset		= 0,
		.size		= 0x0020000,
		.mask_flags	= MTD_WRITEABLE,
	}, {
		.name		= "firmware",
		.offset		= 0x0020000,
		.size		= 0x0EE0000,
	}, {
		.name		= "tffs (1)",
		.offset		= 0x0f00000,
		.size		= 0x0080000,
		.mask_flags	= MTD_WRITEABLE,
	}, {
		.name		= "tffs (2)",
		.offset		= 0x0f80000,
		.size		= 0x0080000,
		.mask_flags	= MTD_WRITEABLE,
	}
};

static struct flash_platform_data fritz1750e_flash_data = {
	.parts		= fritz1750e_flash_partitions,
	.nr_parts	= ARRAY_SIZE(fritz1750e_flash_partitions),
};

static struct gpio_led fritz1750e_leds_gpio[] __initdata = {
	{
		.name		= "fritz1750e:green:lan",
		.gpio		= FRITZ1750E_GPIO_LED_LAN,
		.active_low	= 0,
	}, {
		.name		= "fritz1750e:green:rssi0",
		.gpio		= FRITZ1750E_74HC_GPIO_LED_RSSI0,
		.active_low	= 0,
	}, {
		.name		= "fritz1750e:green:rssi1",
		.gpio		= FRITZ1750E_74HC_GPIO_LED_RSSI1,
		.active_low	= 0,
	}, {
		.name		= "fritz1750e:green:rssi2",
		.gpio		= FRITZ1750E_74HC_GPIO_LED_RSSI2,
		.active_low	= 0,
	}, {
		.name		= "fritz1750e:green:rssi3",
		.gpio		= FRITZ1750E_74HC_GPIO_LED_RSSI3,
		.active_low	= 0,
	}, {
		.name		= "fritz1750e:green:rssi4",
		.gpio		= FRITZ1750E_74HC_GPIO_LED_RSSI4,
		.active_low	= 0,
	}, {
		.name		= "fritz1750e:green:wlan",
		.gpio		= FRITZ1750E_74HC_GPIO_LED_WLAN,
		.active_low	= 0,
	}, {
		.name		= "fritz1750e:green:power",
		.gpio		= FRITZ1750E_74HC_GPIO_LED_POWER,
		.active_low	= 0,
	},
};

static struct gpio_keys_button fritz1750e_gpio_keys[] __initdata = {
	{
		.desc			= "WPS Button",
		.type			= EV_KEY,
		.code			= KEY_WPS_BUTTON,
		.debounce_interval	= FRITZ1750E_KEYS_DEBOUNCE_INTERVAL,
		.gpio			= FRITZ1750E_GPIO_BTN_WPS,
		.active_low		= 1,
	}
};

static struct at803x_platform_data fritz1750e_at803x_data = {
	.disable_smarteee = 1,
	.has_reset_gpio = 1,
	.override_sgmii_aneg = 1,
	.reset_gpio = FRITZ1750E_GPIO_PHY_RESET,
};

static struct mdio_board_info fritz1750e_mdio_info[] = {
	{
		.bus_id = "ag71xx-mdio.1",
		.phy_addr = FRITZ1750E_PHY_ADDRESS,
		.platform_data = &fritz1750e_at803x_data,
	},
};

static void __init fritz1750e_setup(void) {
	ath79_register_m25p80(&fritz1750e_flash_data);

	gpio_request_one(FRITZ1750E_GPIO_MDIO_CLK, GPIOF_OUT_INIT_HIGH, "MDC Pull-UP");
	gpio_request_one(FRITZ1750E_GPIO_MDIO_DATA, GPIOF_OUT_INIT_HIGH, "MDIO Pull-UP");
	gpio_request_one(FRITZ1750E_GPIO_PHY_RESET, GPIOF_OUT_INIT_HIGH, "PHY reset");
	gpio_request_one(FRITZ1750E_GPIO_PEREGRINE_RESET, GPIOF_OUT_INIT_HIGH, "PEREGRINE reset");
	gpio_request_one(FRITZ1750E_GPIO_PCIE_RESET, GPIOF_OUT_INIT_HIGH, "PCIE reset");

	/* Register PHY device */
	mdiobus_register_board_info(fritz1750e_mdio_info,
				    ARRAY_SIZE(fritz1750e_mdio_info));

	ath79_register_mdio(1, ~BIT(FRITZ1750E_PHY_ADDRESS));
	ath79_eth0_data.mii_bus_dev = &ath79_mdio1_device.dev;
	ath79_eth0_data.phy_if_mode = PHY_INTERFACE_MODE_SGMII;
	ath79_eth0_data.phy_mask = BIT(FRITZ1750E_PHY_ADDRESS);
	ath79_eth0_pll_data.pll_1000 = 0x03000000;
	ath79_eth0_pll_data.pll_100 = 0x00000101;
	ath79_eth0_pll_data.pll_10 = 0x00001313;
	ath79_register_eth(0);

	/* Initialize 2.4GHz WiFi */
	ath79_register_wmac_simple();

	/* Initialize 5GHz WiFi */
	ath79_register_pci();

	/* Register LED shift-register */
	spi_register_board_info(fritz1750e_spi_info,
				ARRAY_SIZE(fritz1750e_spi_info));
	platform_device_register(&fritz1750e_spi_device);

	/* Register GPIO buttons */
	ath79_register_gpio_keys_polled(-1, FRITZ1750E_KEYS_POLL_INTERVAL,
					ARRAY_SIZE(fritz1750e_gpio_keys),
					fritz1750e_gpio_keys);

	/* Register LEDs */
	ath79_register_leds_gpio(-1, ARRAY_SIZE(fritz1750e_leds_gpio),
				 fritz1750e_leds_gpio);
}

MIPS_MACHINE(ATH79_MACH_FRITZ1750E, "FRITZ1750E",
	     "AVM FRITZ!WLAN Repeater 1750E", fritz1750e_setup);

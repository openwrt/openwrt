/*
 *  TELTONIKA TLT-RUT900 board support
 *
 *  Copyright (C) 2014 Teltonika
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 */

#include <linux/phy.h>
#include <linux/gpio.h>
#include <linux/platform_device.h>
#include <linux/ath9k_platform.h>
#include <linux/ar8216_platform.h>
#include <linux/mmc/host.h>
#include <linux/spi/spi.h>
#include <linux/spi/mmc_spi.h>

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

#include <linux/i2c.h>
#include <linux/i2c-algo-bit.h>
#include <linux/i2c-gpio.h>

/* I2C interface */
#define rut900_GPIO_SDA			17
#define rut900_GPIO_SCK			16

#define RUT900_GPIO_LED_BAR0		48
#define RUT900_GPIO_LED_BAR1		49
#define RUT900_GPIO_LED_BAR2		50
#define RUT900_GPIO_LED_BAR3		51
#define RUT900_GPIO_LED_BAR4		52
#define RUT900_GPIO_LED_ST_GRE		54
#define RUT900_GPIO_LED_ST_RED		53
//#define RUT900_GPIO_LED_WAN		18
#define RUT900_GPIO_LED_LAN1		14
#define RUT900_GPIO_LED_LAN2		13
#define RUT900_GPIO_LED_LAN3		22
#define RUT900_GPIO_LED_WAN		1

#define RUT900_GPIO_EXTERNAL_LNA0	2
//#define RUT900_GPIO_EXTERNAL_LNA1	3

#define RUT900_GPIO_BTN_RESET		15
#define RUT900_GPIO_BTN_EXPANDER	4
#define RUT900_GPIO_BTN_TEST		58
#define RUT900_GPIO_MMC_CS		3

//#define RUT900_GPIO_EXPANDER_POWER	11

#define RUT900_KEYS_POLL_INTERVAL	20	/* msecs */
#define RUT900_KEYS_DEBOUNCE_INTERVAL	(3 * RUT900_KEYS_POLL_INTERVAL)
#define RUT900_KEYS_DEBOUNCE_INTERVAL_DIN 20

#define RUT900_MAC0_OFFSET		0
#define RUT900_MAC1_OFFSET		6
#define RUT900_WMAC_CALDATA_OFFSET	0x1000
#define RUT900_PCIE_CALDATA_OFFSET	0x5000

static struct i2c_gpio_platform_data rut900_i2c_device_platdata = {
	.sda_pin	= rut900_GPIO_SDA,
	.scl_pin	= rut900_GPIO_SCK,
	.udelay		= 10,
};

static struct platform_device rut900_i2c_device = {
	.name		= "i2c-gpio",
	.id		= 0,
	.num_resources	= 0,
	.resource	= NULL,
	.dev		= {
		.platform_data	= &rut900_i2c_device_platdata,
	},
};

static struct i2c_board_info rut900_i2c_devs[] __initdata = {
	{
		I2C_BOARD_INFO("pca9539", 0x74), 
	},
	{
		I2C_BOARD_INFO("mcp3221", 0x4D), 
	},
};

static void rut900_i2c_init(void)
{
	/* The gpio interface */
	platform_device_register(&rut900_i2c_device);
	/* I2C devices */
	i2c_register_board_info(0, rut900_i2c_devs, ARRAY_SIZE(rut900_i2c_devs));
}

//Flash partitions are calcutated in drivers/mtd/tltpart.c
static const char *rut900_part_probes[] = {
	"tlt",
	NULL,
};

static struct flash_platform_data rut900_flash_data = {
	.part_probes	= rut900_part_probes,
};

static struct gpio_led rut900_leds_gpio[] __initdata = {
	{
		.name		= "tlt:green:signal_bar0",
		.gpio		= RUT900_GPIO_LED_BAR0,
		.active_low	= 0,
	},
	{
		.name		= "tlt:green:signal_bar1",
		.gpio		= RUT900_GPIO_LED_BAR1,
		.active_low	= 0,
	},
	{
		.name		= "tlt:green:signal_bar2",
		.gpio		= RUT900_GPIO_LED_BAR2,
		.active_low	= 0,
	},
	{
		.name		= "tlt:green:signal_bar3",
		.gpio		= RUT900_GPIO_LED_BAR3,
		.active_low	= 0,
	},
	{
		.name		= "tlt:green:signal_bar4",
		.gpio		= RUT900_GPIO_LED_BAR4,
		.active_low	= 0,
	},
	{
		.name		= "tlt:green:status",
		.gpio		= RUT900_GPIO_LED_ST_GRE,
		.active_low	= 0,
	},
	{
		.name		= "tlt:red:status",
		.gpio		= RUT900_GPIO_LED_ST_RED,
		.active_low	= 0,
	},

};

static struct gpio_keys_button rut900_gpio_keys[] __initdata = {
	{
		.desc		= "Reset button",
		.type		= EV_KEY,
		.code		= KEY_RESTART,
		.debounce_interval = RUT900_KEYS_DEBOUNCE_INTERVAL,
		.gpio		= RUT900_GPIO_BTN_RESET,
		.active_low	= 1,
	},
	{
		.desc		= "Expander interrupt",
		.type		= EV_KEY,
		.code		= KEY_RFKILL,
		.debounce_interval = RUT900_KEYS_DEBOUNCE_INTERVAL_DIN,
		.gpio		= RUT900_GPIO_BTN_EXPANDER,
	},
};

static struct spi_board_info ath79_spi_info[] = {
	{
		.bus_num	= 0,
		.chip_select	= 0,
		.max_speed_hz	= 25000000,
		.modalias	= "m25p80",
		.platform_data	= &rut900_flash_data,
	},
	{
		.bus_num	= 0,
		.chip_select	= 1,
		.max_speed_hz	= 25000000,
		.modalias	= "mmc_spi",
	}
};

static struct ath79_spi_platform_data ath79_spi_data = {
	.bus_num 	= 0,
	.num_chipselect	= 2,
};

static void __init rut900_setup(void)
{
	u8 *mac = (u8 *) KSEG1ADDR(0x1f020000);
	u8 *art = (u8 *) KSEG1ADDR(0x1f030000);
	u8 tmpmac[ETH_ALEN];

	ath79_register_spi(&ath79_spi_data, ath79_spi_info, 2);
	ath79_register_leds_gpio(-1, ARRAY_SIZE(rut900_leds_gpio),
				 rut900_leds_gpio);
	ath79_register_gpio_keys_polled(-1, RUT900_KEYS_POLL_INTERVAL,
					ARRAY_SIZE(rut900_gpio_keys),
					rut900_gpio_keys);
					
	ath79_wmac_set_ext_lna_gpio(0, RUT900_GPIO_EXTERNAL_LNA0);

	ath79_init_mac(tmpmac, mac, 2);
	ath79_register_wmac(art + RUT900_WMAC_CALDATA_OFFSET, tmpmac);


	ath79_setup_ar934x_eth_cfg(AR934X_ETH_CFG_SW_ONLY_MODE);

	ath79_register_mdio(1, 0x0);

	/* LAN */
	ath79_init_mac(ath79_eth1_data.mac_addr, mac, 0);

	/* GMAC1 is connected to the internal switch */
	ath79_eth1_data.phy_if_mode = PHY_INTERFACE_MODE_GMII;

	ath79_register_eth(1);

	/* WAN */
	ath79_init_mac(ath79_eth0_data.mac_addr, mac, 1);

	/* GMAC0 is connected to the PHY4 of the internal switch */
	ath79_switch_data.phy4_mii_en = 1;
	ath79_switch_data.phy_poll_mask = BIT(4);
	ath79_eth0_data.phy_if_mode = PHY_INTERFACE_MODE_MII;
	ath79_eth0_data.phy_mask = BIT(4);
	ath79_eth0_data.mii_bus_dev = &ath79_mdio1_device.dev;

	ath79_register_eth(0);

	ath79_register_usb();

	ath79_gpio_output_select(RUT900_GPIO_LED_LAN1,
				 AR934X_GPIO_OUT_LED_LINK3);
	ath79_gpio_output_select(RUT900_GPIO_LED_LAN2,
				 AR934X_GPIO_OUT_LED_LINK2);
	ath79_gpio_output_select(RUT900_GPIO_LED_LAN3,
				 AR934X_GPIO_OUT_LED_LINK1);
	ath79_gpio_output_select(RUT900_GPIO_LED_WAN,
				 AR934X_GPIO_OUT_LED_LINK4);
	
	//SPI CS1
	ath79_gpio_output_select(RUT900_GPIO_MMC_CS,
				 7);
	
	//HS UART
	ath79_gpio_output_select(18, 79);
	
	/* Init I2C interface */
	rut900_i2c_init();
}

MIPS_MACHINE(ATH79_MACH_TLT_RUT900, "TLT-RUT900",
		"TELTONIKA TLT-RUT900",
		rut900_setup);

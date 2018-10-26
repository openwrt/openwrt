/*
 * GL.iNet GL-X1200 board support
 *
 * Copyright (C) 2018 wellnw <guilin.wang@gl-inet.com>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published
 * by the Free Software Foundation.
 */
#include <linux/pci.h>
#include <linux/phy.h>
#include <linux/gpio.h>
#include <linux/platform_device.h>
#include <linux/ath9k_platform.h>
#include <linux/ar8216_platform.h>
#include <linux/etherdevice.h>

#include <asm/mach-ath79/ath79.h>
#include <asm/mach-ath79/irq.h>
#include <asm/mach-ath79/ar71xx_regs.h>

#include <linux/platform_data/phy-at803x.h>

#include "common.h"
#include "dev-ap9x-pci.h"
#include "dev-eth.h"
#include "dev-gpio-buttons.h"
#include "dev-leds-gpio.h"
#include "dev-spi.h"
#include "dev-m25p80.h"
#include "dev-wmac.h"
#include "dev-usb.h"
#include "machtypes.h"
#include "pci.h"

#define GL_X1200_KEYS_POLL_INTERVAL	20
#define GL_X1200_KEYS_DEBOUNCE_INTERVAL	(3 * GL_X1200_KEYS_POLL_INTERVAL)

#define GL_X1200_GPIO_LED_WLAN2G        19
#define GL_X1200_GPIO_LED_WLAN5G        20
#define GL_X1200_GPIO_LED_POWER			8
#define GL_X1200_GPIO_USB_POWER			7

#define GL_X1200_GPIO_BTN_RESET			2

#define GL_X1200_MAC0_OFFSET             0x0000
#define GL_X1200_WMAC_CALDATA_OFFSET     0x1000
#define GL_X1200_PCI_CALDATA_OFFSET      0x5000

static struct spi_board_info gl_x1200_spi_info[] = {
	{
		.bus_num		= 0,
		.chip_select    = 0,
		.max_speed_hz   = 25000000,
		.modalias		= "m25p80",
		.platform_data  = NULL,
	},
    {
        .bus_num    = 0,
        .chip_select    = 1,
        .max_speed_hz   = 25000000,
        .modalias   = "generic-spinand-controller",
        .platform_data  = NULL,
    }
};

static struct ath79_spi_platform_data gl_x1200_spi_data = {
	.bus_num			= 0,
	.num_chipselect     = 2,
};

static struct gpio_led gl_x1200_leds_gpio[] __initdata = {
	{
		.name			= "gl-x1200:green:power",
		.gpio			= GL_X1200_GPIO_LED_POWER,
		.default_state	= LEDS_GPIO_DEFSTATE_KEEP,
		.active_low		= 1,
	},{
		.name		= "gl-x1200:green:usbpower",
		.gpio		= GL_X1200_GPIO_USB_POWER,
		.active_low	= 1,
	},{
		.name		= "gl-x1200:green:wlan2g",
		.gpio		= GL_X1200_GPIO_LED_WLAN2G,
		.active_low	= 1,
	},{
		.name		= "gl-x1200:green:wlan5g",
		.gpio		= GL_X1200_GPIO_LED_WLAN5G,
		.active_low	= 0,
	}
};

static struct gpio_keys_button gl_x1200_gpio_keys[] __initdata = {
	{
		.desc                   = "reset",
		.type                   = EV_KEY,
		.code                   = KEY_RESTART,
		.debounce_interval      = GL_X1200_KEYS_DEBOUNCE_INTERVAL,
		.gpio                   = GL_X1200_GPIO_BTN_RESET,
		.active_low             = 1,
	},
};

static struct ar8327_pad_cfg gl_x1200_ar8327_pad0_cfg = {
	.mode = AR8327_PAD_MAC_SGMII,
	.sgmii_delay_en = true,
};

static struct ar8327_platform_data gl_x1200_ar8327_data = {
	.pad0_cfg = &gl_x1200_ar8327_pad0_cfg,
	.port0_cfg = {
		.force_link = 1,
		.speed = AR8327_PORT_SPEED_1000,
		.duplex = 1,
		.txpause = 1,
		.rxpause = 1,
	},
};


static struct mdio_board_info gl_x1200_mdio0_info[] = {
	{
		.bus_id = "ag71xx-mdio.0",
		.phy_addr = 0,
		.platform_data = &gl_x1200_ar8327_data,
	},
};

static void __init gl_x1200_setup(void)
{
	u8 *eeprom = (u8 *) KSEG1ADDR(0x1f050000);

	ath79_register_spi(&gl_x1200_spi_data, gl_x1200_spi_info, 2);

	ath79_init_mac(ath79_eth0_data.mac_addr,
			eeprom + GL_X1200_MAC0_OFFSET, 0);

	ath79_eth0_data.phy_if_mode = PHY_INTERFACE_MODE_SGMII;
	ath79_eth0_data.mii_bus_dev = &ath79_mdio0_device.dev;
	ath79_eth0_data.phy_mask = BIT(0);

	mdiobus_register_board_info(gl_x1200_mdio0_info,
			ARRAY_SIZE(gl_x1200_mdio0_info));
	ath79_register_mdio(0, 0x00);
	ath79_register_eth(0);
	ath79_register_usb();

	//disable JTAG Function	
	ath79_gpio_function_enable(AR934X_GPIO_FUNC_JTAG_DISABLE);

	ath79_register_wmac(eeprom + GL_X1200_WMAC_CALDATA_OFFSET, NULL);
	ap91_pci_init(eeprom + GL_X1200_PCI_CALDATA_OFFSET, NULL);

	ath79_register_leds_gpio(-1, ARRAY_SIZE(gl_x1200_leds_gpio),
			gl_x1200_leds_gpio);

	ath79_register_gpio_keys_polled(-1, GL_X1200_KEYS_POLL_INTERVAL,
			ARRAY_SIZE(gl_x1200_gpio_keys),
			gl_x1200_gpio_keys);
}

MIPS_MACHINE(ATH79_MACH_GL_X1200, "GL-X1200", "GL-X1200",
		gl_x1200_setup);

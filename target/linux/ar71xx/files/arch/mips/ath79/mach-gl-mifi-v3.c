/*
 * GL.iNet GL-MIFI-V3 board support
 *
 * Copyright (C) 2018 Piotr Dymacz <pepe2k@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published
 * by the Free Software Foundation.
 */

#include <linux/gpio.h>
#include <linux/i2c.h>
#include <linux/i2c-gpio.h>
#include <linux/platform_device.h>

#include <asm/mach-ath79/ath79.h>
#include <asm/mach-ath79/ar71xx_regs.h>

#include "common.h"
#include "dev-ap9x-pci.h"
#include "dev-eth.h"
#include "dev-gpio-buttons.h"
#include "dev-leds-gpio.h"
#include "dev-spi.h"
#include "dev-usb.h"
#include "dev-wmac.h"
#include "machtypes.h"

#define GL_MIFI_V3_GPIO_BTN_RESET		3

#define GL_MIFI_V3_KEYS_POLL_INTERVAL	20
#define GL_MIFI_V3_KEYS_DEBOUNCE_INTERVAL	(3 * GL_MIFI_V3_KEYS_POLL_INTERVAL)

#define GL_MIFI_V3_MAC0_OFFSET		0
#define GL_MIFI_V3_WMAC2G_CALDATA_OFFSET	0x1000
#define GL_MIFI_V3_WMAC5G_CALDATA_OFFSET	0x5000

static struct gpio_keys_button gl_mifi_v3_gpio_keys[] __initdata = {
	{
		.desc			= "reset",
		.type			= EV_KEY,
		.code			= KEY_RESTART,
		.debounce_interval	= GL_MIFI_V3_KEYS_DEBOUNCE_INTERVAL,
		.gpio			= GL_MIFI_V3_GPIO_BTN_RESET,
		.active_low		= 1,
	},
};

static struct spi_board_info gl_mifi_v3_spi_info[] = {
    {
        .bus_num    = 0,
        .chip_select    = 0,
        .max_speed_hz   = 25000000,
        .modalias   = "m25p80",
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

static struct ath79_spi_platform_data gl_mifi_v3_spi_data = {
    .bus_num        = 0,
    .num_chipselect     = 2,
};

static void __init gl_mifi_v3_setup(void)
{
	u8 *art = (u8 *) KSEG1ADDR(0x1f050000);

	ath79_register_spi(&gl_mifi_v3_spi_data, gl_mifi_v3_spi_info, 2);

	ath79_register_mdio(0, 0x0);

	ath79_switch_data.phy4_mii_en = 1;
	ath79_switch_data.phy_poll_mask |= BIT(4);

	/* WAN */
	ath79_eth0_data.duplex = DUPLEX_FULL;
	ath79_eth0_data.phy_if_mode = PHY_INTERFACE_MODE_MII;
	ath79_eth0_data.phy_mask = BIT(4);
	ath79_eth0_data.speed = SPEED_100;
	ath79_init_mac(ath79_eth0_data.mac_addr, art + GL_MIFI_V3_MAC0_OFFSET, 0);
	ath79_register_eth(0);

	/* Disable JTAG (enables GPIO0-3) */
	ath79_gpio_function_enable(AR934X_GPIO_FUNC_JTAG_DISABLE);

	ath79_register_gpio_keys_polled(-1, GL_MIFI_V3_KEYS_POLL_INTERVAL,
					ARRAY_SIZE(gl_mifi_v3_gpio_keys),
					gl_mifi_v3_gpio_keys);

	ath79_register_usb();

	ath79_register_wmac(art + GL_MIFI_V3_WMAC2G_CALDATA_OFFSET, NULL);

	ap91_pci_init(art + GL_MIFI_V3_WMAC5G_CALDATA_OFFSET, NULL);
}

MIPS_MACHINE(ATH79_MACH_GL_MIFI_V3, "GL-MIFI-V3", "GL.iNet GL-MIFI-V3",
	     gl_mifi_v3_setup);

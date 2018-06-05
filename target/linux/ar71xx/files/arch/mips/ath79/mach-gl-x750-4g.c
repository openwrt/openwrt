/*
 * GL.iNet GL-X750-4G board support
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
#include "dev-m25p80.h"
#include "dev-usb.h"
#include "dev-wmac.h"
#include "machtypes.h"

#define GL_X750_4G_GPIO_LED_POWER		12
#define GL_X750_4G_GPIO_LED_WLAN2G	4
#define GL_X750_4G_GPIO_LED_WLAN5G	13
#define GL_X750_4G_GPIO_LED_4G	15
#define GL_X750_4G_GPIO_LED_WAN		14

#define GL_X750_4G_GPIO_BTN_RESET		3

#define GL_X750_4G_GPIO_I2C_SCL		16
#define GL_X750_4G_GPIO_I2C_SDA		1

#define GL_X750_4G_GPIO_USB_POWER		2

#define GL_X750_4G_KEYS_POLL_INTERVAL	20
#define GL_X750_4G_KEYS_DEBOUNCE_INTERVAL	(3 * GL_X750_4G_KEYS_POLL_INTERVAL)

#define GL_X750_4G_MAC0_OFFSET		0
#define GL_X750_4G_WMAC2G_CALDATA_OFFSET	0x1000
#define GL_X750_4G_WMAC5G_CALDATA_OFFSET	0x5000

static struct gpio_led gl_x750_4g_leds_gpio[] __initdata = {
	{
		.name		= "gl-x750-4g:white:power",
		.gpio		= GL_X750_4G_GPIO_LED_POWER,
		.default_state	= LEDS_GPIO_DEFSTATE_KEEP,
		.active_low	= 1,
	}, {
		.name		= "gl-x750-4g:white:wlan2g",
		.gpio		= GL_X750_4G_GPIO_LED_WLAN2G,
		.active_low	= 1,
	}, {
		.name		= "gl-x750-4g:white:wlan5g",
		.gpio		= GL_X750_4G_GPIO_LED_WLAN5G,
		.active_low	= 1,
	},{
		.name		= "gl-x750-4g:white:wan",
		.gpio		= GL_X750_4G_GPIO_LED_WAN,
		.active_low	= 1,
	}, {
		.name		= "gl-x750-4g:white:4g",
		.gpio		= GL_X750_4G_GPIO_LED_4G,
		.active_low	= 1,
	},
};

static struct gpio_keys_button gl_x750_4g_gpio_keys[] __initdata = {
	{
		.desc			= "reset",
		.type			= EV_KEY,
		.code			= KEY_RESTART,
		.debounce_interval	= GL_X750_4G_KEYS_DEBOUNCE_INTERVAL,
		.gpio			= GL_X750_4G_GPIO_BTN_RESET,
		.active_low		= 1,
	},
};

/*static struct i2c_gpio_platform_data gl_x750_4g_i2c_gpio_data = {
	.sda_pin = GL_X750_4G_GPIO_I2C_SDA,
	.scl_pin = GL_X750_4G_GPIO_I2C_SCL,
};

static struct platform_device gl_x750_4g_i2c_gpio = {
	.name	= "i2c-gpio",
	.id	= 0,
	.dev	= {
		.platform_data = &gl_x750_4g_i2c_gpio_data,
	},
};*/

static void __init gl_x750_4g_setup(void)
{
	u8 *art = (u8 *) KSEG1ADDR(0x1f050000);

	ath79_register_m25p80(NULL);

	ath79_register_mdio(0, 0x0);
	//ath79_register_mdio(1, 0x0);

	ath79_switch_data.phy4_mii_en = 1;
	ath79_switch_data.phy_poll_mask |= BIT(4);

	/* WAN */
	ath79_eth0_data.duplex = DUPLEX_FULL;
	ath79_eth0_data.phy_if_mode = PHY_INTERFACE_MODE_MII;
	ath79_eth0_data.phy_mask = BIT(4);
	ath79_eth0_data.speed = SPEED_100;
	ath79_init_mac(ath79_eth0_data.mac_addr, art + GL_X750_4G_MAC0_OFFSET, 0);
	ath79_register_eth(0);

	/* LAN */
	ath79_eth1_data.duplex = DUPLEX_FULL;
	ath79_eth1_data.phy_if_mode = PHY_INTERFACE_MODE_GMII;
	ath79_eth1_data.speed = SPEED_1000;
	ath79_init_mac(ath79_eth1_data.mac_addr, art + GL_X750_4G_MAC0_OFFSET, 1);
	ath79_register_eth(1);

	/* Disable JTAG (enables GPIO0-3) */
	ath79_gpio_function_enable(AR934X_GPIO_FUNC_JTAG_DISABLE);

	ath79_register_leds_gpio(-1, ARRAY_SIZE(gl_x750_4g_leds_gpio),
				 gl_x750_4g_leds_gpio);

	ath79_register_gpio_keys_polled(-1, GL_X750_4G_KEYS_POLL_INTERVAL,
					ARRAY_SIZE(gl_x750_4g_gpio_keys),
					gl_x750_4g_gpio_keys);

	gpio_request_one(GL_X750_4G_GPIO_USB_POWER,
			 GPIOF_OUT_INIT_HIGH | GPIOF_EXPORT_DIR_FIXED,
			 "USB power");

//	platform_device_register(&gl_x750_4g_i2c_gpio);

	ath79_register_usb();

	ath79_register_wmac(art + GL_X750_4G_WMAC2G_CALDATA_OFFSET, NULL);

	ap91_pci_init(art + GL_X750_4G_WMAC5G_CALDATA_OFFSET, NULL);
}

MIPS_MACHINE(ATH79_MACH_GL_X750_4G, "GL-X750-4G", "GL.iNet GL-X750-4G",
	     gl_x750_4g_setup);

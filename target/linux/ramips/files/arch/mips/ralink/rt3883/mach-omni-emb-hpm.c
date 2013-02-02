/*
 *  Omnima EMB HPM board support
 *
 *  Copyright (C) 2012 Gabor Juhos <juhosg@openwrt.org>
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 */

#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/ethtool.h>
#include <linux/gpio.h>
#include <linux/rt2x00_platform.h>
#include <linux/spi/spi.h>

#include <asm/mach-ralink/machine.h>
#include <asm/mach-ralink/dev-gpio-buttons.h>
#include <asm/mach-ralink/dev-gpio-leds.h>
#include <asm/mach-ralink/rt3883.h>
#include <asm/mach-ralink/rt3883_regs.h>
#include <asm/mach-ralink/ramips_eth_platform.h>

#include "devices.h"

#define OMNI_EMB_HPM_GPIO_LED_POWER		7
#define OMNI_EMB_HPM_GPIO_LED_ETH_GREEN		20
#define OMNI_EMB_HPM_GPIO_LED_ETH_RED		18
#define OMNI_EMB_HPM_GPIO_LED_STATUS		21
#define OMNI_EMB_HPM_GPIO_LED_WIFI_GREEN	17
#define OMNI_EMB_HPM_GPIO_LED_WIFI_RED		19

#define OMNI_EMB_HPM_GPIO_BUTTON_RESET		14

#define OMNI_EMB_HPM_GPIO_USB0_ENABLE		2
#define OMNI_EMB_HPM_GPIO_USB1_ENABLE		1
#define OMNI_EMB_HPM_GPIO_USB0_OC		12
#define OMNI_EMB_HPM_GPIO_USB1_OC		13

#define OMNI_EMB_HPM_KEYS_POLL_INTERVAL	20
#define OMNI_EMB_HPM_KEYS_DEBOUNCE_INTERVAL (3 * OMNI_EMB_HPM_KEYS_POLL_INTERVAL)

static struct gpio_led omni_emb_hpm_leds_gpio[] __initdata = {
	{
		.name		= "emb:orange:power",
		.gpio		= OMNI_EMB_HPM_GPIO_LED_POWER,
		.active_low	= 1,
	},
	{
		.name		= "emb:green:status",
		.gpio		= OMNI_EMB_HPM_GPIO_LED_STATUS,
	},
	{
		.name		= "emb:green:eth",
		.gpio		= OMNI_EMB_HPM_GPIO_LED_ETH_GREEN,
		.active_low	= 1,
	},
	{
		.name		= "emb:red:eth",
		.gpio		= OMNI_EMB_HPM_GPIO_LED_ETH_RED,
		.active_low	= 1,
	},
	{
		.name		= "emb:green:wifi",
		.gpio		= OMNI_EMB_HPM_GPIO_LED_WIFI_GREEN,
		.active_low	= 1,
	},
	{
		.name		= "emb:red:wifi",
		.gpio		= OMNI_EMB_HPM_GPIO_LED_WIFI_RED,
		.active_low	= 1,
	},
};

static struct gpio_keys_button omni_emb_hpm_gpio_buttons[] __initdata = {
	{
		.desc		= "reset",
		.type		= EV_KEY,
		.code		= KEY_RESTART,
		.debounce_interval = OMNI_EMB_HPM_KEYS_DEBOUNCE_INTERVAL,
		.gpio		= OMNI_EMB_HPM_GPIO_BUTTON_RESET,
		.active_low	= 1,
	},
};

static struct spi_board_info omni_emb_hpm_spi_info[] = {
	{
		.bus_num	= 0,
		.chip_select	= 0,
		.max_speed_hz	= 25000000,
		.modalias	= "m25p80",
	}
};


static void __init omni_emb_hpm_gpio_request(unsigned int gpio,
					     unsigned long flags,
					     const char *label,
					     bool free)
{
	int err;

	err = gpio_request_one(gpio, flags, label);
	if (err) {
		pr_err("EMB_HPM: can't setup GPIO%u (%s), err=%d\n",
			gpio, label, err);
		return;
	}

	if (free)
		gpio_free(gpio);
}

static void __init omni_emb_hpm_gpio_init(void)
{
	rt3883_gpio_init(RT3883_GPIO_MODE_I2C |
		         RT3883_GPIO_MODE_UART0(RT3883_GPIO_MODE_GPIO) |
			 RT3883_GPIO_MODE_JTAG);

	omni_emb_hpm_gpio_request(OMNI_EMB_HPM_GPIO_USB0_ENABLE,
				  GPIOF_OUT_INIT_HIGH,
				  "USB0 power", true);
	omni_emb_hpm_gpio_request(OMNI_EMB_HPM_GPIO_USB1_ENABLE,
				  GPIOF_OUT_INIT_HIGH,
				  "USB1 power", true);
	omni_emb_hpm_gpio_request(OMNI_EMB_HPM_GPIO_USB0_OC,
				  GPIOF_IN, "USB0 OC", false);
	omni_emb_hpm_gpio_request(OMNI_EMB_HPM_GPIO_USB1_OC,
				  GPIOF_IN, "USB1 OC", false);
}

static void __init omni_emb_hpm_init(void)
{
	omni_emb_hpm_gpio_init();

	ramips_register_gpio_leds(-1, ARRAY_SIZE(omni_emb_hpm_leds_gpio),
				  omni_emb_hpm_leds_gpio);

	ramips_register_gpio_buttons(-1, OMNI_EMB_HPM_KEYS_POLL_INTERVAL,
				     ARRAY_SIZE(omni_emb_hpm_gpio_buttons),
				     omni_emb_hpm_gpio_buttons);

	rt3883_register_spi(omni_emb_hpm_spi_info,
			    ARRAY_SIZE(omni_emb_hpm_spi_info));

	rt3883_register_wlan();

	rt3883_eth_data.phy_mask = BIT(4);
	rt3883_register_ethernet();

	rt3883_register_wdt(false);
	rt3883_register_usbhost();
}

MIPS_MACHINE(RAMIPS_MACH_OMNI_EMB_HPM, "OMNI-EMB-HPM", "Omnima EMB HPM",
	     omni_emb_hpm_init);

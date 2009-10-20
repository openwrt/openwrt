/*
 *  Compex WP543/WPJ543 board support
 *
 *  Copyright (C) 2008-2009 Gabor Juhos <juhosg@openwrt.org>
 *  Copyright (C) 2008 Imre Kaloz <kaloz@openwrt.org>
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 */

#include <linux/platform_device.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/partitions.h>
#include <linux/spi/spi.h>
#include <linux/spi/flash.h>
#include <linux/input.h>

#include <asm/mips_machine.h>
#include <asm/mach-ar71xx/ar71xx.h>
#include <asm/mach-ar71xx/pci.h>

#include "devices.h"

#define WP543_GPIO_SW6		2
#define WP543_GPIO_LED_1	3
#define WP543_GPIO_LED_2	4
#define WP543_GPIO_LED_WLAN	5
#define WP543_GPIO_LED_CONN	6
#define WP543_GPIO_LED_DIAG	7
#define WP543_GPIO_SW4		8

#define WP543_BUTTONS_POLL_INTERVAL	20

static struct spi_board_info wp543_spi_info[] = {
	{
		.bus_num	= 0,
		.chip_select	= 0,
		.max_speed_hz	= 25000000,
		.modalias	= "m25p80",
	}
};

static struct ar71xx_pci_irq wp543_pci_irqs[] __initdata = {
	{
		.slot	= 0,
		.pin	= 1,
		.irq	= AR71XX_PCI_IRQ_DEV0,
	}, {
		.slot	= 1,
		.pin	= 1,
		.irq	= AR71XX_PCI_IRQ_DEV1,
	}, {
		.slot	= 2,
		.pin	= 1,
		.irq	= AR71XX_PCI_IRQ_DEV2,
	}
};

static struct gpio_led wp543_leds_gpio[] __initdata = {
	{
		.name		= "wp543:green:led1",
		.gpio		= WP543_GPIO_LED_1,
		.active_low	= 1,
	}, {
		.name		= "wp543:green:led2",
		.gpio		= WP543_GPIO_LED_2,
		.active_low	= 1,
	}, {
		.name		= "wp543:green:wlan",
		.gpio		= WP543_GPIO_LED_WLAN,
		.active_low	= 1,
	}, {
		.name		= "wp543:green:conn",
		.gpio		= WP543_GPIO_LED_CONN,
		.active_low	= 1,
	}, {
		.name		= "wp543:green:diag",
		.gpio		= WP543_GPIO_LED_DIAG,
		.active_low	= 1,
	}
};

static struct gpio_button wp543_gpio_buttons[] __initdata = {
	{
		.desc		= "sw6",
		.type		= EV_KEY,
		.code		= BTN_0,
		.threshold	= 5,
		.gpio		= WP543_GPIO_SW6,
	}, {
		.desc		= "sw4",
		.type		= EV_KEY,
		.code		= BTN_1,
		.threshold	= 5,
		.gpio		= WP543_GPIO_SW4,
	}
};

static void __init wp543_setup(void)
{
	ar71xx_add_device_spi(NULL, wp543_spi_info, ARRAY_SIZE(wp543_spi_info));

	ar71xx_add_device_mdio(0xfffffff7);

	ar71xx_eth0_data.phy_if_mode = PHY_INTERFACE_MODE_MII;
	ar71xx_eth0_data.phy_mask = 0x00000008;
	ar71xx_eth0_data.reset_bit = RESET_MODULE_GE0_MAC |
				     RESET_MODULE_GE0_PHY;
	ar71xx_add_device_eth(0);

	ar71xx_add_device_usb();

	ar71xx_pci_init(ARRAY_SIZE(wp543_pci_irqs), wp543_pci_irqs);

	ar71xx_add_device_leds_gpio(-1, ARRAY_SIZE(wp543_leds_gpio),
					wp543_leds_gpio);

	ar71xx_add_device_gpio_buttons(-1, WP543_BUTTONS_POLL_INTERVAL,
					ARRAY_SIZE(wp543_gpio_buttons),
					wp543_gpio_buttons);
}

MIPS_MACHINE(AR71XX_MACH_WP543, "Compex WP543", wp543_setup);

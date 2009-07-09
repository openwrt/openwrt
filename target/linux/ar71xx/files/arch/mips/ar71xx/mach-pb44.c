/*
 *  Atheros PB44 board support
 *
 *  Copyright (C) 2009 Gabor Juhos <juhosg@openwrt.org>
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 */

#include <linux/init.h>
#include <linux/bitops.h>
#include <linux/input.h>
#include <linux/platform_device.h>
#include <linux/spi/spi.h>
#include <linux/spi/flash.h>
#include <linux/i2c.h>
#include <linux/i2c-gpio.h>
#include <linux/i2c/pcf857x.h>

#include <asm/mips_machine.h>
#include <asm/mach-ar71xx/ar71xx.h>
#include <asm/mach-ar71xx/pci.h>

#include "devices.h"

#define PB44_PCF8757_VSC7385_CS	0
#define PB44_PCF8757_STEREO_CS	1
#define PB44_PCF8757_SLIC_CS0	2
#define PB44_PCF8757_SLIC_TEST	3
#define PB44_PCF8757_SLIC_INT0	4
#define PB44_PCF8757_SLIC_INT1	5
#define PB44_PCF8757_SW_RESET	6
#define PB44_PCF8757_SW_JUMP	8
#define PB44_PCF8757_LED_JUMP1	9
#define PB44_PCF8757_LED_JUMP2	10
#define PB44_PCF8757_TP24	11
#define PB44_PCF8757_TP25	12
#define PB44_PCF8757_TP26	13
#define PB44_PCF8757_TP27	14
#define PB44_PCF8757_TP28	15

#define PB44_GPIO_I2C_SCL	0
#define PB44_GPIO_I2C_SDA	1

#define PB44_GPIO_EXP_BASE	16
#define PB44_GPIO_SW_RESET	(PB44_GPIO_EXP_BASE + PB44_PCF8757_SW_RESET)
#define PB44_GPIO_SW_JUMP	(PB44_GPIO_EXP_BASE + PB44_PCF8757_SW_JUMP)
#define PB44_GPIO_LED_JUMP1	(PB44_GPIO_EXP_BASE + PB44_PCF8757_LED_JUMP1)
#define PB44_GPIO_LED_JUMP2	(PB44_GPIO_EXP_BASE + PB44_PCF8757_LED_JUMP2)

static struct spi_board_info pb44_spi_info[] = {
	{
		.bus_num	= 0,
		.chip_select	= 0,
		.max_speed_hz	= 25000000,
		.modalias	= "m25p80",
	}
};

static struct ar71xx_pci_irq pb44_pci_irqs[] __initdata = {
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

static struct i2c_gpio_platform_data pb44_i2c_gpio_data = {
	.sda_pin        = PB44_GPIO_I2C_SDA,
	.scl_pin        = PB44_GPIO_I2C_SCL,
};

static struct platform_device pb44_i2c_gpio_device = {
	.name		= "i2c-gpio",
	.id		= 0,
	.dev = {
		.platform_data	= &pb44_i2c_gpio_data,
	}
};

static struct pcf857x_platform_data pb44_pcf857x_data = {
	.gpio_base	= PB44_GPIO_EXP_BASE,
};

static struct i2c_board_info pb44_i2c_board_info[] __initdata = {
	{
		I2C_BOARD_INFO("pcf8575", 0x20),
		.platform_data  = &pb44_pcf857x_data,
	},
};

static struct gpio_led pb44_leds_gpio[] __initdata = {
	{
		.name		= "pb44:amber:jump1",
		.gpio		= PB44_GPIO_LED_JUMP1,
		.active_low	= 1,
	}, {
		.name		= "pb44:green:jump2",
		.gpio		= PB44_GPIO_LED_JUMP2,
		.active_low	= 1,
	},
};

static struct gpio_button pb44_gpio_buttons[] __initdata = {
	{
		.desc		= "soft_reset",
		.type		= EV_KEY,
		.code		= BTN_0,
		.threshold	= 5,
		.gpio		= PB44_GPIO_SW_RESET,
		.active_low	= 1,
	} , {
		.desc		= "jumpstart",
		.type		= EV_KEY,
		.code		= BTN_1,
		.threshold	= 5,
		.gpio		= PB44_GPIO_SW_JUMP,
		.active_low	= 1,
	}
};

#define PB44_WAN_PHYMASK	BIT(0)
#define PB44_LAN_PHYMASK	0
#define PB44_MDIO_PHYMASK	(PB44_LAN_PHYMASK | PB44_WAN_PHYMASK)

static void __init pb44_init(void)
{
	ar71xx_add_device_spi(NULL, pb44_spi_info,
				ARRAY_SIZE(pb44_spi_info));

	ar71xx_add_device_mdio(~PB44_MDIO_PHYMASK);

	ar71xx_eth0_data.phy_if_mode = PHY_INTERFACE_MODE_RGMII;
	ar71xx_eth0_data.phy_mask = PB44_WAN_PHYMASK;

	ar71xx_add_device_eth(0);

	ar71xx_eth1_data.phy_if_mode = PHY_INTERFACE_MODE_RGMII;
	ar71xx_eth1_data.phy_mask = PB44_LAN_PHYMASK;
	ar71xx_eth1_data.speed = SPEED_1000;
	ar71xx_eth1_data.duplex = DUPLEX_FULL;

	ar71xx_add_device_eth(1);

	ar71xx_add_device_usb();

	ar71xx_pci_init(ARRAY_SIZE(pb44_pci_irqs), pb44_pci_irqs);

	i2c_register_board_info(0, pb44_i2c_board_info,
 				ARRAY_SIZE(pb44_i2c_board_info));

	platform_device_register(&pb44_i2c_gpio_device);

	ar71xx_add_device_leds_gpio(-1, ARRAY_SIZE(pb44_leds_gpio),
				    pb44_leds_gpio);

	ar71xx_add_device_gpio_buttons(-1, 20, ARRAY_SIZE(pb44_gpio_buttons),
				       pb44_gpio_buttons);
}

MIPS_MACHINE(AR71XX_MACH_PB44, "Atheros PB44", pb44_init);

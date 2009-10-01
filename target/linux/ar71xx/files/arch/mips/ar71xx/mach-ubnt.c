/*
 *  Ubiquiti RouterStation support
 *
 *  Copyright (C) 2008-2009 Gabor Juhos <juhosg@openwrt.org>
 *  Copyright (C) 2008 Imre Kaloz <kaloz@openwrt.org>
 *  Copyright (C) 2008 Ubiquiti <support@ubnt.com>
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 */

#include <linux/platform_device.h>
#include <linux/spi/spi.h>
#include <linux/spi/flash.h>
#include <linux/input.h>

#include <asm/mips_machine.h>
#include <asm/mach-ar71xx/ar71xx.h>
#include <asm/mach-ar71xx/pci.h>

#include "devices.h"

#define UBNT_RS_GPIO_LED_RF	2
#define UBNT_RS_GPIO_SW4	8

#define UBNT_LS_SR71_GPIO_LED_D25	0
#define UBNT_LS_SR71_GPIO_LED_D26	1
#define UBNT_LS_SR71_GPIO_LED_D24	2
#define UBNT_LS_SR71_GPIO_LED_D23	4
#define UBNT_LS_SR71_GPIO_LED_D22	5
#define UBNT_LS_SR71_GPIO_LED_D27	6
#define UBNT_LS_SR71_GPIO_LED_D28	7

#define UBNT_BULLET_M_GPIO_LED_L1	0
#define UBNT_BULLET_M_GPIO_LED_L2	1
#define UBNT_BULLET_M_GPIO_LED_L3	11
#define UBNT_BULLET_M_GPIO_LED_L4	7
#define UBNT_BULLET_M_GPIO_BTN_RESET	12

#define UBNT_BUTTONS_POLL_INTERVAL	20

static struct spi_board_info ubnt_spi_info[] = {
	{
		.bus_num	= 0,
		.chip_select	= 0,
		.max_speed_hz	= 25000000,
		.modalias	= "m25p80",
	}
};

static struct ar71xx_pci_irq ubnt_pci_irqs[] __initdata = {
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

static struct gpio_led ubnt_rs_leds_gpio[] __initdata = {
	{
		.name		= "ubnt:green:rf",
		.gpio		= UBNT_RS_GPIO_LED_RF,
		.active_low	= 0,
	}
};

static struct gpio_led ubnt_ls_sr71_leds_gpio[] __initdata = {
	{
		.name		= "ubnt:green:d22",
		.gpio		= UBNT_LS_SR71_GPIO_LED_D22,
		.active_low	= 0,
	}, {
		.name		= "ubnt:green:d23",
		.gpio		= UBNT_LS_SR71_GPIO_LED_D23,
		.active_low	= 0,
	}, {
		.name		= "ubnt:green:d24",
		.gpio		= UBNT_LS_SR71_GPIO_LED_D24,
		.active_low	= 0,
	}, {
		.name		= "ubnt:red:d25",
		.gpio		= UBNT_LS_SR71_GPIO_LED_D25,
		.active_low	= 0,
	}, {
		.name		= "ubnt:red:d26",
		.gpio		= UBNT_LS_SR71_GPIO_LED_D26,
		.active_low	= 0,
	}, {
		.name		= "ubnt:green:d27",
		.gpio		= UBNT_LS_SR71_GPIO_LED_D27,
		.active_low	= 0,
	}, {
		.name		= "ubnt:green:d28",
		.gpio		= UBNT_LS_SR71_GPIO_LED_D28,
		.active_low	= 0,
	}
};

static struct gpio_led ubnt_bullet_m_leds_gpio[] __initdata = {
	{
		.name		= "ubnt:red:link1",
		.gpio		= UBNT_BULLET_M_GPIO_LED_L1,
		.active_low	= 0,
	}, {
		.name		= "ubnt:orange:link2",
		.gpio		= UBNT_BULLET_M_GPIO_LED_L2,
		.active_low	= 0,
	}, {
		.name		= "ubnt:green:link3",
		.gpio		= UBNT_BULLET_M_GPIO_LED_L3,
		.active_low	= 0,
	}, {
		.name		= "ubnt:green:link4",
		.gpio		= UBNT_BULLET_M_GPIO_LED_L4,
		.active_low	= 0,
	}
};

static struct gpio_button ubnt_gpio_buttons[] __initdata = {
	{
		.desc		= "sw4",
		.type		= EV_KEY,
		.code		= BTN_0,
		.threshold	= 5,
		.gpio		= UBNT_RS_GPIO_SW4,
		.active_low	= 1,
	}
};

static struct gpio_button ubnt_bullet_m_gpio_buttons[] __initdata = {
	{
		.desc		= "reset",
		.type		= EV_KEY,
		.code		= BTN_0,
		.threshold	= 5,
		.gpio		= UBNT_BULLET_M_GPIO_BTN_RESET,
		.active_low	= 1,
	}
};

static void __init ubnt_generic_setup(void)
{
	ar71xx_add_device_spi(NULL, ubnt_spi_info,
				    ARRAY_SIZE(ubnt_spi_info));

	ar71xx_add_device_gpio_buttons(-1, UBNT_BUTTONS_POLL_INTERVAL,
					ARRAY_SIZE(ubnt_gpio_buttons),
					ubnt_gpio_buttons);

	ar71xx_pci_init(ARRAY_SIZE(ubnt_pci_irqs), ubnt_pci_irqs);
}

#define UBNT_RS_WAN_PHYMASK	(1 << 20)
#define UBNT_RS_LAN_PHYMASK	((1 << 16) | (1 << 17) | (1 << 18) | (1 << 19))

#define UBNT_RSPRO_WAN_PHYMASK	(1 << 4)
#define UBNT_RSPRO_LAN_PHYMASK	((1 << 0) | (1 << 1) | (1 << 2) | (1 << 3))

static void __init ubnt_rs_setup(void)
{
	ubnt_generic_setup();

	ar71xx_add_device_mdio(~(UBNT_RS_WAN_PHYMASK | UBNT_RS_LAN_PHYMASK));

	ar71xx_eth0_data.phy_if_mode = PHY_INTERFACE_MODE_MII;
	ar71xx_eth0_data.phy_mask = UBNT_RS_WAN_PHYMASK;

	ar71xx_eth1_data.phy_if_mode = PHY_INTERFACE_MODE_RMII;
	ar71xx_eth1_data.phy_mask = UBNT_RS_LAN_PHYMASK;

	ar71xx_eth1_data.speed = SPEED_100;
	ar71xx_eth1_data.duplex = DUPLEX_FULL;

	ar71xx_add_device_eth(0);
	ar71xx_add_device_eth(1);

	ar71xx_add_device_usb();

	ar71xx_add_device_leds_gpio(-1, ARRAY_SIZE(ubnt_rs_leds_gpio),
					ubnt_rs_leds_gpio);
}

MIPS_MACHINE(AR71XX_MACH_UBNT_RS, "Ubiquiti RouterStation", ubnt_rs_setup);

static void __init ubnt_rspro_setup(void)
{
	ubnt_generic_setup();

	ar71xx_add_device_mdio(~(UBNT_RSPRO_WAN_PHYMASK | UBNT_RSPRO_LAN_PHYMASK));

	ar71xx_eth0_data.phy_if_mode = PHY_INTERFACE_MODE_RGMII;
	ar71xx_eth0_data.phy_mask = UBNT_RSPRO_WAN_PHYMASK;

	ar71xx_eth1_data.phy_if_mode = PHY_INTERFACE_MODE_RGMII;
	ar71xx_eth1_data.phy_mask = UBNT_RSPRO_LAN_PHYMASK;

	ar71xx_eth1_data.speed = SPEED_1000;
	ar71xx_eth1_data.duplex = DUPLEX_FULL;

	ar71xx_add_device_eth(0);
	ar71xx_add_device_eth(1);

	ar71xx_add_device_usb();

	ar71xx_add_device_leds_gpio(-1, ARRAY_SIZE(ubnt_rs_leds_gpio),
					ubnt_rs_leds_gpio);
}

MIPS_MACHINE(AR71XX_MACH_UBNT_RSPRO, "Ubiquiti RouterStation Pro",
	     ubnt_rspro_setup);

static void __init ubnt_lsx_setup(void)
{
	ubnt_generic_setup();
}

MIPS_MACHINE(AR71XX_MACH_UBNT_LSX, "Ubiquiti LSX", ubnt_lsx_setup);

#define UBNT_LSSR71_PHY_MASK	(1 << 1)

static void __init ubnt_lssr71_setup(void)
{
	ubnt_generic_setup();

	ar71xx_add_device_mdio(~UBNT_LSSR71_PHY_MASK);

	ar71xx_eth0_data.phy_if_mode = PHY_INTERFACE_MODE_MII;
	ar71xx_eth0_data.phy_mask = UBNT_LSSR71_PHY_MASK;

	ar71xx_add_device_eth(0);

	ar71xx_add_device_leds_gpio(-1, ARRAY_SIZE(ubnt_ls_sr71_leds_gpio),
					ubnt_ls_sr71_leds_gpio);
}

MIPS_MACHINE(AR71XX_MACH_UBNT_LSSR71, "Ubiquiti LS-SR71", ubnt_lssr71_setup);

static struct ar71xx_pci_irq ubnt_bullet_m_pci_irqs[] __initdata = {
	{
		.slot	= 0,
		.pin	= 1,
		.irq	= AR71XX_PCI_IRQ_DEV0,
	}
};

static void __init ubnt_bullet_m_setup(void)
{
	u8 *mac = (u8 *) KSEG1ADDR(0x1fff0000);

	ar71xx_set_mac_base(mac);
	ar71xx_add_device_spi(NULL, ubnt_spi_info,
				    ARRAY_SIZE(ubnt_spi_info));

	ar71xx_add_device_mdio(~0);

	ar71xx_eth0_data.phy_if_mode = PHY_INTERFACE_MODE_MII;
	ar71xx_eth0_data.phy_mask = 0;

	ar71xx_eth0_data.speed = SPEED_100;
	ar71xx_eth0_data.duplex = DUPLEX_FULL;

	ar71xx_add_device_eth(0);

	ar71xx_add_device_leds_gpio(-1, ARRAY_SIZE(ubnt_bullet_m_leds_gpio),
					ubnt_bullet_m_leds_gpio);

	ar71xx_add_device_gpio_buttons(-1, UBNT_BUTTONS_POLL_INTERVAL,
					ARRAY_SIZE(ubnt_bullet_m_gpio_buttons),
					ubnt_bullet_m_gpio_buttons);

	ar71xx_pci_init(ARRAY_SIZE(ubnt_bullet_m_pci_irqs),
			ubnt_bullet_m_pci_irqs);
}

MIPS_MACHINE(AR71XX_MACH_UBNT_BULLET_M, "Ubiquiti Bullet M", ubnt_bullet_m_setup);

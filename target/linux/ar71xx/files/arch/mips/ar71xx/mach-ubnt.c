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

#include <asm/mach-ar71xx/ar71xx.h>

#include "machtype.h"
#include "devices.h"
#include "dev-m25p80.h"
#include "dev-ap91-pci.h"
#include "dev-gpio-buttons.h"
#include "dev-pb42-pci.h"
#include "dev-leds-gpio.h"
#include "dev-usb.h"

#define UBNT_RS_GPIO_LED_RF	2
#define UBNT_RS_GPIO_SW4	8

#define UBNT_LS_SR71_GPIO_LED_D25	0
#define UBNT_LS_SR71_GPIO_LED_D26	1
#define UBNT_LS_SR71_GPIO_LED_D24	2
#define UBNT_LS_SR71_GPIO_LED_D23	4
#define UBNT_LS_SR71_GPIO_LED_D22	5
#define UBNT_LS_SR71_GPIO_LED_D27	6
#define UBNT_LS_SR71_GPIO_LED_D28	7

#define UBNT_M_GPIO_LED_L1	0
#define UBNT_M_GPIO_LED_L2	1
#define UBNT_M_GPIO_LED_L3	11
#define UBNT_M_GPIO_LED_L4	7
#define UBNT_M_GPIO_BTN_RESET	12

#define UBNT_KEYS_POLL_INTERVAL		20	/* msecs */
#define UBNT_KEYS_DEBOUNCE_INTERVAL	(3 * UBNT_KEYS_POLL_INTERVAL)

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

static struct gpio_led ubnt_m_leds_gpio[] __initdata = {
	{
		.name		= "ubnt:red:link1",
		.gpio		= UBNT_M_GPIO_LED_L1,
		.active_low	= 0,
	}, {
		.name		= "ubnt:orange:link2",
		.gpio		= UBNT_M_GPIO_LED_L2,
		.active_low	= 0,
	}, {
		.name		= "ubnt:green:link3",
		.gpio		= UBNT_M_GPIO_LED_L3,
		.active_low	= 0,
	}, {
		.name		= "ubnt:green:link4",
		.gpio		= UBNT_M_GPIO_LED_L4,
		.active_low	= 0,
	}
};

static struct gpio_keys_button ubnt_gpio_keys[] __initdata = {
	{
		.desc		= "sw4",
		.type		= EV_KEY,
		.code		= KEY_RESTART,
		.debounce_interval = UBNT_KEYS_DEBOUNCE_INTERVAL,
		.gpio		= UBNT_RS_GPIO_SW4,
		.active_low	= 1,
	}
};

static struct gpio_keys_button ubnt_m_gpio_keys[] __initdata = {
	{
		.desc		= "reset",
		.type		= EV_KEY,
		.code		= KEY_RESTART,
		.debounce_interval = UBNT_KEYS_DEBOUNCE_INTERVAL,
		.gpio		= UBNT_M_GPIO_BTN_RESET,
		.active_low	= 1,
	}
};

static void __init ubnt_generic_setup(void)
{
	ar71xx_add_device_m25p80(NULL);

	ar71xx_register_gpio_keys_polled(-1, UBNT_KEYS_POLL_INTERVAL,
					 ARRAY_SIZE(ubnt_gpio_keys),
					 ubnt_gpio_keys);

	pb42_pci_init();
}

/*
 * There is Secondary MAC address duplicate problem with some UBNT HW batches.
 * Do not increase Secondary MAC address by 1 but do workaround
 * with 'Locally Administrated' bit.
 */
static void __init ubnt_init_secondary_mac(unsigned char *mac_base)
{
	ar71xx_init_mac(ar71xx_eth1_data.mac_addr, mac_base, 0);
	ar71xx_eth1_data.mac_addr[0] |= 0x02;
}

#define UBNT_RS_WAN_PHYMASK	BIT(20)
#define UBNT_RS_LAN_PHYMASK	(BIT(16) | BIT(17) | BIT(18) | BIT(19))

static void __init ubnt_rs_setup(void)
{
	ubnt_generic_setup();

	ar71xx_add_device_mdio(0, ~(UBNT_RS_WAN_PHYMASK | UBNT_RS_LAN_PHYMASK));

	ar71xx_init_mac(ar71xx_eth0_data.mac_addr, ar71xx_mac_base, 0);
	ar71xx_eth0_data.phy_if_mode = PHY_INTERFACE_MODE_MII;
	ar71xx_eth0_data.phy_mask = UBNT_RS_WAN_PHYMASK;

	ubnt_init_secondary_mac(ar71xx_mac_base);
	ar71xx_eth1_data.phy_if_mode = PHY_INTERFACE_MODE_RMII;
	ar71xx_eth1_data.speed = SPEED_100;
	ar71xx_eth1_data.duplex = DUPLEX_FULL;

	ar71xx_add_device_eth(0);
	ar71xx_add_device_eth(1);

	ar71xx_add_device_usb();

	ar71xx_add_device_leds_gpio(-1, ARRAY_SIZE(ubnt_rs_leds_gpio),
					ubnt_rs_leds_gpio);
}

MIPS_MACHINE(AR71XX_MACH_UBNT_RS, "UBNT-RS", "Ubiquiti RouterStation",
	     ubnt_rs_setup);

#define UBNT_RSPRO_WAN_PHYMASK	BIT(4)
#define UBNT_RSPRO_LAN_PHYMASK	(BIT(0) | BIT(1) | BIT(2) | BIT(3))

static void __init ubnt_rspro_setup(void)
{
	ubnt_generic_setup();

	ar71xx_add_device_mdio(0, ~(UBNT_RSPRO_WAN_PHYMASK |
				 UBNT_RSPRO_LAN_PHYMASK));

	ar71xx_init_mac(ar71xx_eth0_data.mac_addr, ar71xx_mac_base, 0);
	ar71xx_eth0_data.phy_if_mode = PHY_INTERFACE_MODE_RGMII;
	ar71xx_eth0_data.phy_mask = UBNT_RSPRO_WAN_PHYMASK;

	ubnt_init_secondary_mac(ar71xx_mac_base);
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

MIPS_MACHINE(AR71XX_MACH_UBNT_RSPRO, "UBNT-RSPRO", "Ubiquiti RouterStation Pro",
	     ubnt_rspro_setup);

static void __init ubnt_lsx_setup(void)
{
	ubnt_generic_setup();
}

MIPS_MACHINE(AR71XX_MACH_UBNT_LSX, "UBNT-LSX", "Ubiquiti LSX", ubnt_lsx_setup);

#define UBNT_LSSR71_PHY_MASK	BIT(1)

static void __init ubnt_lssr71_setup(void)
{
	ubnt_generic_setup();

	ar71xx_add_device_mdio(0, ~UBNT_LSSR71_PHY_MASK);

	ar71xx_init_mac(ar71xx_eth0_data.mac_addr, ar71xx_mac_base, 0);
	ar71xx_eth0_data.phy_if_mode = PHY_INTERFACE_MODE_MII;
	ar71xx_eth0_data.phy_mask = UBNT_LSSR71_PHY_MASK;

	ar71xx_add_device_eth(0);

	ar71xx_add_device_leds_gpio(-1, ARRAY_SIZE(ubnt_ls_sr71_leds_gpio),
					ubnt_ls_sr71_leds_gpio);
}

MIPS_MACHINE(AR71XX_MACH_UBNT_LSSR71, "UBNT-LS-SR71", "Ubiquiti LS-SR71",
	     ubnt_lssr71_setup);

#define UBNT_M_WAN_PHYMASK	BIT(4)

static void __init ubnt_m_setup(void)
{
	u8 *mac1 = (u8 *) KSEG1ADDR(0x1fff0000);
	u8 *mac2 = (u8 *) KSEG1ADDR(0x1fff0000 + ETH_ALEN);
	u8 *ee = (u8 *) KSEG1ADDR(0x1fff1000);

	ar71xx_add_device_m25p80(NULL);

	ar71xx_add_device_mdio(0, ~UBNT_M_WAN_PHYMASK);

	ar71xx_init_mac(ar71xx_eth0_data.mac_addr, mac1, 0);
	ar71xx_init_mac(ar71xx_eth1_data.mac_addr, mac2, 0);
	ar71xx_eth0_data.phy_if_mode = PHY_INTERFACE_MODE_RMII;
	ar71xx_eth0_data.speed = SPEED_100;
	ar71xx_eth0_data.duplex = DUPLEX_FULL;
	ar71xx_eth0_data.phy_mask = UBNT_M_WAN_PHYMASK;

	ar71xx_add_device_eth(0);

	ap91_pci_init(ee, NULL);

	ar71xx_add_device_leds_gpio(-1, ARRAY_SIZE(ubnt_m_leds_gpio),
					ubnt_m_leds_gpio);

	ar71xx_register_gpio_keys_polled(-1, UBNT_KEYS_POLL_INTERVAL,
					 ARRAY_SIZE(ubnt_m_gpio_keys),
					 ubnt_m_gpio_keys);
}

static void __init ubnt_rocket_m_setup(void)
{
	ubnt_m_setup();
	ar71xx_add_device_usb();
}

MIPS_MACHINE(AR71XX_MACH_UBNT_BULLET_M, "UBNT-BM", "Ubiquiti Bullet M",
	     ubnt_m_setup);
MIPS_MACHINE(AR71XX_MACH_UBNT_ROCKET_M, "UBNT-RM", "Ubiquiti Rocket M",
	     ubnt_rocket_m_setup);

/* TODO detect the second ethernet port and use one
   init function for all Ubiquiti MIMO series products */
static void __init ubnt_nano_m_setup(void)
{
	ubnt_m_setup();

	ar71xx_eth1_data.has_ar7240_switch = 1;
	ar71xx_eth1_data.phy_if_mode = PHY_INTERFACE_MODE_RMII;
	ar71xx_eth1_data.speed = SPEED_1000;
	ar71xx_eth1_data.duplex = DUPLEX_FULL;

	ar71xx_add_device_eth(1);
}

MIPS_MACHINE(AR71XX_MACH_UBNT_NANO_M, "UBNT-NM", "Ubiquiti Nanostation M",
	     ubnt_nano_m_setup);

static struct gpio_led ubnt_unifi_leds_gpio[] __initdata = {
	{
		.name		= "ubnt:orange:dome",
		.gpio		= 1,
		.active_low	= 0,
	}, {
		.name		= "ubnt:green:dome",
		.gpio		= 0,
		.active_low	= 0,
	}
};

static void __init ubnt_unifi_setup(void)
{
	u8 *mac = (u8 *) KSEG1ADDR(0x1fff0000);
	u8 *ee = (u8 *) KSEG1ADDR(0x1fff1000);

	ar71xx_add_device_m25p80(NULL);

	ar71xx_add_device_mdio(0, ~UBNT_M_WAN_PHYMASK);

	ar71xx_init_mac(ar71xx_eth0_data.mac_addr, mac, 0);
	ar71xx_eth0_data.phy_if_mode = PHY_INTERFACE_MODE_RMII;
	ar71xx_eth0_data.speed = SPEED_100;
	ar71xx_eth0_data.duplex = DUPLEX_FULL;
	ar71xx_eth0_data.phy_mask = UBNT_M_WAN_PHYMASK;

	ar71xx_add_device_eth(0);

	ap91_pci_init(ee, NULL);

	ar71xx_add_device_leds_gpio(-1, ARRAY_SIZE(ubnt_unifi_leds_gpio),
					ubnt_unifi_leds_gpio);
}

MIPS_MACHINE(AR71XX_MACH_UBNT_UNIFI, "UBNT-XM", "Ubiquiti UniFi",
	     ubnt_unifi_setup);

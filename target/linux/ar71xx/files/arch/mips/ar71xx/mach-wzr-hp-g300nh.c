/*
 *  Buffalo WZR-HP-G300NH board support
 *
 *  Copyright (C) 2010 Gabor Juhos <juhosg@openwrt.org>
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 */

#include <linux/platform_device.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/partitions.h>
#include <linux/nxp_74hc153.h>
#include <linux/rtl8366_smi.h>

#include <asm/mips_machine.h>
#include <asm/mach-ar71xx/ar71xx.h>
#include <asm/mach-ar71xx/ar91xx_flash.h>

#include "machtype.h"
#include "devices.h"
#include "dev-ar913x-wmac.h"
#include "dev-gpio-buttons.h"
#include "dev-leds-gpio.h"
#include "dev-usb.h"

#define WZRHPG300NH_GPIO_LED_USB	0
#define WZRHPG300NH_GPIO_LED_DIAG	1
#define WZRHPG300NH_GPIO_LED_WIRELESS	6
#define WZRHPG300NH_GPIO_LED_SECURITY	17
#define WZRHPG300NH_GPIO_LED_ROUTER	18

#define WZRHPG300NH_GPIO_RTL8366_SDA	19
#define WZRHPG300NH_GPIO_RTL8366_SCK	20

#define WZRHPG300NH_GPIO_74HC153_S0	9
#define WZRHPG300NH_GPIO_74HC153_S1	11
#define WZRHPG300NH_GPIO_74HC153_1Y	12
#define WZRHPG300NH_GPIO_74HC153_2Y	14

#define WZRHPG300NH_GPIO_EXP_BASE	32
#define WZRHPG300NH_GPIO_BTN_AOSS	(WZRHPG300NH_GPIO_EXP_BASE + 0)
#define WZRHPG300NH_GPIO_BTN_RESET	(WZRHPG300NH_GPIO_EXP_BASE + 1)
#define WZRHPG300NH_GPIO_BTN_ROUTER_ON	(WZRHPG300NH_GPIO_EXP_BASE + 2)
#define WZRHPG300NH_GPIO_BTN_QOS_ON	(WZRHPG300NH_GPIO_EXP_BASE + 3)
#define WZRHPG300NH_GPIO_BTN_USB	(WZRHPG300NH_GPIO_EXP_BASE + 5)
#define WZRHPG300NH_GPIO_BTN_ROUTER_AUTO (WZRHPG300NH_GPIO_EXP_BASE + 6)
#define WZRHPG300NH_GPIO_BTN_QOS_OFF	(WZRHPG300NH_GPIO_EXP_BASE + 7)

#define WZRHPG300NH_BUTTONS_POLL_INTERVAL	20

#define WZRHPG300NH_MAC_OFFSET		0x20c

#ifdef CONFIG_MTD_PARTITIONS
static struct mtd_partition wzrhpg300nh_flash_partitions[] = {
	{
		.name		= "u-boot",
		.offset		= 0,
		.size		= 0x0040000,
		.mask_flags	= MTD_WRITEABLE,
	}, {
		.name		= "u-boot-env",
		.offset		= 0x0040000,
		.size		= 0x0020000,
		.mask_flags	= MTD_WRITEABLE,
	}, {
		.name		= "kernel",
		.offset		= 0x0060000,
		.size		= 0x0100000,
	}, {
		.name		= "rootfs",
		.offset		= 0x0160000,
		.size		= 0x1e60000,
	}, {
		.name		= "user_property",
		.offset		= 0x1fc0000,
		.size		= 0x0020000,
		.mask_flags	= MTD_WRITEABLE,
	}, {
		.name		= "art",
		.offset		= 0x1fe0000,
		.size		= 0x0020000,
		.mask_flags	= MTD_WRITEABLE,
	}, {
		.name		= "firmware",
		.offset		= 0x0060000,
		.size		= 0x1f60000,
	}
};
#endif /* CONFIG_MTD_PARTITIONS */

static struct ar91xx_flash_platform_data wzrhpg300nh_flash_data = {
	.width		= 2,
#ifdef CONFIG_MTD_PARTITIONS
        .parts          = wzrhpg300nh_flash_partitions,
        .nr_parts       = ARRAY_SIZE(wzrhpg300nh_flash_partitions),
#endif
};

#define WZRHPG300NH_FLASH_BASE	0x1e000000
#define WZRHPG300NH_FLASH_SIZE	(32 * 1024 * 1024)

static struct resource wzrhpg300nh_flash_resources[] = {
	[0] = {
		.start	= WZRHPG300NH_FLASH_BASE,
		.end	= WZRHPG300NH_FLASH_BASE + WZRHPG300NH_FLASH_SIZE - 1,
		.flags	= IORESOURCE_MEM,
	},
};

static struct platform_device wzrhpg300nh_flash_device = {
	.name		= "ar91xx-flash",
	.id		= -1,
	.resource	= wzrhpg300nh_flash_resources,
	.num_resources	= ARRAY_SIZE(wzrhpg300nh_flash_resources),
	.dev		= {
		.platform_data = &wzrhpg300nh_flash_data,
	}
};

static struct gpio_led wzrhpg300nh_leds_gpio[] __initdata = {
	{
		.name		= "wzr-hp-g300nh:orange:security",
		.gpio		= WZRHPG300NH_GPIO_LED_SECURITY,
		.active_low	= 1,
	}, {
		.name		= "wzr-hp-g300nh:green:wireless",
		.gpio		= WZRHPG300NH_GPIO_LED_WIRELESS,
		.active_low	= 1,
	}, {
		.name		= "wzr-hp-g300nh:green:router",
		.gpio		= WZRHPG300NH_GPIO_LED_ROUTER,
		.active_low	= 1,
	}, {
		.name		= "wzr-hp-g300nh:red:diag",
		.gpio		= WZRHPG300NH_GPIO_LED_DIAG,
		.active_low	= 1,
	}, {
		.name		= "wzr-hp-g300nh:blue:usb",
		.gpio		= WZRHPG300NH_GPIO_LED_USB,
		.active_low	= 1,
	}
};

static struct gpio_button wzrhpg300nh_gpio_buttons[] __initdata = {
	{
		.desc		= "reset",
		.type		= EV_KEY,
		.code		= BTN_0,
		.threshold	= 3,
		.gpio		= WZRHPG300NH_GPIO_BTN_RESET,
		.active_low	= 1,
	}, {
		.desc		= "aoss",
		.type		= EV_KEY,
		.code		= BTN_1,
		.threshold	= 3,
		.gpio		= WZRHPG300NH_GPIO_BTN_AOSS,
		.active_low	= 1,
	}, {
		.desc		= "usb",
		.type		= EV_KEY,
		.code		= BTN_2,
		.threshold	= 3,
		.gpio		= WZRHPG300NH_GPIO_BTN_USB,
		.active_low	= 1,
	}, {
		.desc		= "qos_on",
		.type		= EV_KEY,
		.code		= BTN_3,
		.threshold	= 3,
		.gpio		= WZRHPG300NH_GPIO_BTN_QOS_ON,
		.active_low	= 0,
	}, {
		.desc		= "qos_off",
		.type		= EV_KEY,
		.code		= BTN_4,
		.threshold	= 3,
		.gpio		= WZRHPG300NH_GPIO_BTN_QOS_OFF,
		.active_low	= 0,
	}, {
		.desc		= "router_on",
		.type		= EV_KEY,
		.code		= BTN_5,
		.threshold	= 3,
		.gpio		= WZRHPG300NH_GPIO_BTN_ROUTER_ON,
		.active_low	= 0,
	}, {
		.desc		= "router_auto",
		.type		= EV_KEY,
		.code		= BTN_6,
		.threshold	= 3,
		.gpio		= WZRHPG300NH_GPIO_BTN_ROUTER_AUTO,
		.active_low	= 0,
	}
};

static struct nxp_74hc153_platform_data wzrhpg300nh_74hc153_data = {
	.gpio_base	= WZRHPG300NH_GPIO_EXP_BASE,
	.gpio_pin_s0	= WZRHPG300NH_GPIO_74HC153_S0,
	.gpio_pin_s1	= WZRHPG300NH_GPIO_74HC153_S1,
	.gpio_pin_1y	= WZRHPG300NH_GPIO_74HC153_1Y,
	.gpio_pin_2y	= WZRHPG300NH_GPIO_74HC153_2Y,
};

static struct platform_device wzrhpg300nh_74hc153_device = {
	.name		= NXP_74HC153_DRIVER_NAME,
	.id		= -1,
	.dev = {
		.platform_data	= &wzrhpg300nh_74hc153_data,
	}
};

static struct rtl8366_smi_platform_data wzrhpg300nh_rtl8366_smi_data = {
	.gpio_sda        = WZRHPG300NH_GPIO_RTL8366_SDA,
	.gpio_sck        = WZRHPG300NH_GPIO_RTL8366_SCK,
};

static struct platform_device wzrhpg300nh_rtl8366_smi_device = {
	.name		= "rtl8366-smi",
	.id		= -1,
	.dev = {
		.platform_data	= &wzrhpg300nh_rtl8366_smi_data,
	}
};

static void __init wzrhpg300nh_setup(void)
{
	u8 *eeprom = (u8 *) KSEG1ADDR(0x1fff1000);

	ar71xx_set_mac_base(eeprom + WZRHPG300NH_MAC_OFFSET);

	ar71xx_eth0_pll_data.pll_1000 = 0x1e000100;
	ar71xx_eth0_data.mii_bus_dev = &wzrhpg300nh_rtl8366_smi_device.dev;
	ar71xx_eth0_data.phy_if_mode = PHY_INTERFACE_MODE_RGMII;
	ar71xx_eth0_data.phy_mask = 0xf;
	ar71xx_eth0_data.speed = SPEED_1000;
	ar71xx_eth0_data.duplex = DUPLEX_FULL;

	ar71xx_eth1_pll_data.pll_1000 = 0x1e000100;
	ar71xx_eth1_data.mii_bus_dev = &wzrhpg300nh_rtl8366_smi_device.dev;
	ar71xx_eth1_data.phy_if_mode = PHY_INTERFACE_MODE_RGMII;
	ar71xx_eth1_data.phy_mask = 0x10;

	ar71xx_add_device_eth(0);
	ar71xx_add_device_eth(1);

	ar71xx_add_device_usb();
	ar913x_add_device_wmac(eeprom, NULL);

	platform_device_register(&wzrhpg300nh_74hc153_device);
	platform_device_register(&wzrhpg300nh_flash_device);
	platform_device_register(&wzrhpg300nh_rtl8366_smi_device);

	ar71xx_add_device_leds_gpio(-1, ARRAY_SIZE(wzrhpg300nh_leds_gpio),
				    wzrhpg300nh_leds_gpio);

	ar71xx_add_device_gpio_buttons(-1, WZRHPG300NH_BUTTONS_POLL_INTERVAL,
				       ARRAY_SIZE(wzrhpg300nh_gpio_buttons),
				       wzrhpg300nh_gpio_buttons);

}

MIPS_MACHINE(AR71XX_MACH_WZR_HP_G300NH, "WZR-HP-G300NH",
	     "Buffalo WZR-HP-G300NH", wzrhpg300nh_setup);

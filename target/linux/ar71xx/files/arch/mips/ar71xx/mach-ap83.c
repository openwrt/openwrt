/*
 *  Atheros AP83 board support
 *
 *  Copyright (C) 2008-2009 Gabor Juhos <juhosg@openwrt.org>
 *  Copyright (C) 2008 Imre Kaloz <kaloz@openwrt.org>
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 */

#include <linux/delay.h>
#include <linux/platform_device.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/partitions.h>
#include <linux/spi/spi.h>
#include <linux/spi/spi_gpio.h>
#include <linux/spi/vsc7385.h>

#include <asm/mach-ar71xx/ar71xx.h>
#include <asm/mach-ar71xx/ar91xx_flash.h>

#include "machtype.h"
#include "devices.h"
#include "dev-ar913x-wmac.h"
#include "dev-gpio-buttons.h"
#include "dev-leds-gpio.h"
#include "dev-usb.h"

#define AP83_GPIO_LED_WLAN	6
#define AP83_GPIO_LED_POWER	14
#define AP83_GPIO_LED_JUMPSTART	15
#define AP83_GPIO_BTN_JUMPSTART	12
#define AP83_GPIO_BTN_RESET	21

#define AP83_050_GPIO_VSC7385_CS	1
#define AP83_050_GPIO_VSC7385_MISO	3
#define AP83_050_GPIO_VSC7385_MOSI	16
#define AP83_050_GPIO_VSC7385_SCK	17

#define AP83_BUTTONS_POLL_INTERVAL	20

#ifdef CONFIG_MTD_PARTITIONS
static struct mtd_partition ap83_flash_partitions[] = {
	{
		.name		= "u-boot",
		.offset		= 0,
		.size		= 0x040000,
		.mask_flags	= MTD_WRITEABLE,
	} , {
		.name		= "u-boot-env",
		.offset		= 0x040000,
		.size		= 0x020000,
		.mask_flags	= MTD_WRITEABLE,
	} , {
		.name		= "kernel",
		.offset		= 0x060000,
		.size		= 0x140000,
	} , {
		.name		= "rootfs",
		.offset		= 0x1a0000,
		.size		= 0x650000,
	} , {
		.name		= "art",
		.offset		= 0x7f0000,
		.size		= 0x010000,
		.mask_flags	= MTD_WRITEABLE,
	} , {
		.name		= "firmware",
		.offset		= 0x060000,
		.size		= 0x790000,
	}
};
#endif /* CONFIG_MTD_PARTITIONS */

static struct ar91xx_flash_platform_data ap83_flash_data = {
	.width		= 2,
#ifdef CONFIG_MTD_PARTITIONS
        .parts          = ap83_flash_partitions,
        .nr_parts       = ARRAY_SIZE(ap83_flash_partitions),
#endif
};

static struct resource ap83_flash_resources[] = {
	[0] = {
		.start	= AR71XX_SPI_BASE,
		.end	= AR71XX_SPI_BASE + AR71XX_SPI_SIZE - 1,
		.flags	= IORESOURCE_MEM,
	},
};

static struct platform_device ap83_flash_device = {
	.name		= "ar91xx-flash",
	.id		= -1,
	.resource	= ap83_flash_resources,
	.num_resources	= ARRAY_SIZE(ap83_flash_resources),
	.dev		= {
		.platform_data = &ap83_flash_data,
	}
};

static struct gpio_led ap83_leds_gpio[] __initdata = {
	{
		.name		= "ap83:green:jumpstart",
		.gpio		= AP83_GPIO_LED_JUMPSTART,
		.active_low	= 0,
	}, {
		.name		= "ap83:green:power",
		.gpio		= AP83_GPIO_LED_POWER,
		.active_low	= 0,
	}, {
		.name		= "ap83:green:wlan",
		.gpio		= AP83_GPIO_LED_WLAN,
		.active_low	= 0,
	},
};

static struct gpio_button ap83_gpio_buttons[] __initdata = {
	{
		.desc		= "soft_reset",
		.type		= EV_KEY,
		.code		= BTN_0,
		.threshold	= 5,
		.gpio		= AP83_GPIO_BTN_RESET,
		.active_low	= 1,
	} , {
		.desc		= "jumpstart",
		.type		= EV_KEY,
		.code		= BTN_1,
		.threshold	= 5,
		.gpio		= AP83_GPIO_BTN_JUMPSTART,
		.active_low	= 1,
	}
};

static struct resource ap83_040_spi_resources[] = {
	[0] = {
		.start	= AR71XX_SPI_BASE,
		.end	= AR71XX_SPI_BASE + AR71XX_SPI_SIZE - 1,
		.flags	= IORESOURCE_MEM,
	},
};

static struct platform_device ap83_040_spi_device = {
	.name		= "ap83-spi",
	.id		= 0,
	.resource	= ap83_040_spi_resources,
	.num_resources	= ARRAY_SIZE(ap83_040_spi_resources),
};

static struct spi_gpio_platform_data ap83_050_spi_data = {
	.miso	= AP83_050_GPIO_VSC7385_MISO,
	.mosi	= AP83_050_GPIO_VSC7385_MOSI,
	.sck	= AP83_050_GPIO_VSC7385_SCK,
	.num_chipselect = 1,
};

static struct platform_device ap83_050_spi_device = {
	.name		= "spi_gpio",
	.id		= 0,
	.dev		= {
		.platform_data = &ap83_050_spi_data,
	}
};

static void ap83_vsc7385_reset(void)
{
	ar71xx_device_stop(RESET_MODULE_GE1_PHY);
	udelay(10);
	ar71xx_device_start(RESET_MODULE_GE1_PHY);
	mdelay(50);
}

static struct vsc7385_platform_data ap83_vsc7385_data = {
	.reset		= ap83_vsc7385_reset,
	.ucode_name	= "vsc7385_ucode_ap83.bin",
	.mac_cfg = {
		.tx_ipg		= 6,
		.bit2		= 0,
		.clk_sel	= 3,
	},
};

static struct spi_board_info ap83_spi_info[] = {
	{
		.bus_num	= 0,
		.chip_select	= 0,
		.max_speed_hz	= 25000000,
		.modalias	= "spi-vsc7385",
		.platform_data	= &ap83_vsc7385_data,
		.controller_data = (void *) AP83_050_GPIO_VSC7385_CS,
	}
};

static void __init ap83_generic_setup(void)
{
	u8 *eeprom = (u8 *) KSEG1ADDR(0x1fff1000);

	ar71xx_set_mac_base(eeprom);

	ar71xx_add_device_mdio(0xfffffffe);

	ar71xx_eth0_data.phy_if_mode = PHY_INTERFACE_MODE_RGMII;
	ar71xx_eth0_data.phy_mask = 0x1;

	ar71xx_add_device_eth(0);

	ar71xx_eth1_data.phy_if_mode = PHY_INTERFACE_MODE_RGMII;
	ar71xx_eth1_data.phy_mask = 0x0;
	ar71xx_eth1_data.speed = SPEED_1000;
	ar71xx_eth1_data.duplex = DUPLEX_FULL;

	ar71xx_eth1_pll_data.pll_1000 = 0x1f000000;

	ar71xx_add_device_eth(1);

	ar71xx_add_device_leds_gpio(-1, ARRAY_SIZE(ap83_leds_gpio),
					ap83_leds_gpio);

	ar71xx_add_device_gpio_buttons(-1, AP83_BUTTONS_POLL_INTERVAL,
					ARRAY_SIZE(ap83_gpio_buttons),
					ap83_gpio_buttons);

	ar71xx_add_device_usb();

	ar913x_add_device_wmac(eeprom, NULL);

	platform_device_register(&ap83_flash_device);

	spi_register_board_info(ap83_spi_info, ARRAY_SIZE(ap83_spi_info));
}

static void __init ap83_040_setup(void)
{
	ap83_flash_data.is_shared=1;
	ap83_generic_setup();
	platform_device_register(&ap83_040_spi_device);
}

static void __init ap83_050_setup(void)
{
	ap83_generic_setup();
	platform_device_register(&ap83_050_spi_device);
}

static void __init ap83_setup(void)
{
	u8 *board_id = (u8 *) KSEG1ADDR(0x1fff1244);
	unsigned int board_version;

	board_version = (unsigned int)(board_id[0] - '0');
	board_version += ((unsigned int)(board_id[1] - '0')) * 10;

	switch (board_version) {
	case 40:
		ap83_040_setup();
		break;
	case 50:
		ap83_050_setup();
		break;
	default:
		printk(KERN_WARNING "AP83-%03u board is not yet supported\n",
		       board_version);
	}
}

MIPS_MACHINE(AR71XX_MACH_AP83, "Atheros AP83", ap83_setup);

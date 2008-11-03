/*
 *  MikroTik RouterBOARD 4xx series support
 *
 *  Copyright (C) 2008 Gabor Juhos <juhosg@openwrt.org>
 *  Copyright (C) 2008 Imre Kaloz <kaloz@openwrt.org>
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 */

#include <linux/platform_device.h>
#include <linux/irq.h>
#include <linux/mmc/host.h>
#include <linux/spi/spi.h>
#include <linux/spi/flash.h>
#include <linux/spi/mmc_spi.h>
#include <linux/input.h>

#include <asm/mips_machine.h>
#include <asm/mach-ar71xx/ar71xx.h>
#include <asm/mach-ar71xx/pci.h>
#include <asm/mach-ar71xx/platform.h>

#define RB4XX_GPIO_USER_LED	4
#define RB4XX_GPIO_RESET_SWITCH	7

#define RB4XX_BUTTONS_POLL_INTERVAL	20

static struct gpio_led rb4xx_leds_gpio[] __initdata = {
	{
		.name		= "rb4xx:yellow:user",
		.gpio		= RB4XX_GPIO_USER_LED,
		.active_low	= 0,
	},
};

static struct gpio_button rb4xx_gpio_buttons[] __initdata = {
	{
		.desc		= "reset_switch",
		.type		= EV_KEY,
		.code		= BTN_0,
		.threshold	= 5,
		.gpio		= RB4XX_GPIO_RESET_SWITCH,
		.active_low	= 1,
	}
};

static struct platform_device rb4xx_nand_device = {
	.name	= "rb4xx-nand",
	.id	= -1,
};

static struct ar71xx_pci_irq rb4xx_pci_irqs[] __initdata = {
	{
		.slot	= 1,
		.pin	= 1,
		.irq	= AR71XX_PCI_IRQ_DEV0,
	}, {
		.slot	= 1,
		.pin	= 2,
		.irq	= AR71XX_PCI_IRQ_DEV1,
	}, {
		.slot	= 2,
		.pin	= 1,
		.irq	= AR71XX_PCI_IRQ_DEV1,
	}, {
		.slot	= 3,
		.pin	= 1,
		.irq	= AR71XX_PCI_IRQ_DEV2,
	}
};

#if 0
/*
 * SPI device support is experimental
 */
static struct flash_platform_data rb4xx_flash_data = {
	.type	= "pm25lv512",
};

static struct spi_board_info rb4xx_spi_info[] = {
	{
		.bus_num	= 0,
		.chip_select	= 0,
		.max_speed_hz	= 25000000,
		.modalias	= "m25p80",
		.platform_data	= &rb4xx_flash_data,
	}
};

static struct mmc_spi_platform_data rb433_mmc_data = {
	.ocr_mask	= MMC_VDD_32_33 | MMC_VDD_33_34,
};

static struct spi_board_info rb433_spi_info[] = {
	{
		.bus_num	= 0,
		.chip_select	= 0,
		.max_speed_hz	= 25000000,
		.modalias	= "m25p80",
		.platform_data	= &rb433_flash_data,
	}, {
		.bus_num	= 0,
		.chip_select	= 2,
		.max_speed_hz	= 25000000,
		.modalias	= "mmc_spi",
		.platform_data	= &rb433_mmc_data,
	}
};

static u32 rb433_spi_get_ioc_base(u8 chip_select, int cs_high, int is_on)
{
	u32 ret;

	if (is_on == AR71XX_SPI_CS_INACTIVE) {
		ret = SPI_IOC_CS0 | SPI_IOC_CS1;
	} else {
		if (cs_high) {
			ret = SPI_IOC_CS0 | SPI_IOC_CS1;
		} else {
			if ((chip_select ^ 2) == 0)
				ret = SPI_IOC_CS1 ^ (SPI_IOC_CS0 | SPI_IOC_CS1);
			else
				ret = SPI_IOC_CS0 ^ (SPI_IOC_CS0 | SPI_IOC_CS1);
		}
	}

	return ret;
}

struct ar71xx_spi_platform_data rb433_spi_data = {
	.bus_num		= 0,
	.num_chipselect		= 3,
	.get_ioc_base		= rb433_spi_get_ioc_base,
};

static void rb4xx_add_device_spi(void)
{
	ar71xx_add_device_spi(NULL, rb4xx_spi_info, ARRAY_SIZE(rb4xx_spi_info));
}

static void rb433_add_device_spi(void)
{
	ar71xx_add_device_spi(&rb433_spi_data, rb433_spi_info,
				ARRAY_SIZE(rb433_spi_info));
}
#else
static inline void rb4xx_add_device_spi(void) {}
static inline void rb433_add_device_spi(void) {}
#endif

static void __init rb411_setup(void)
{
	rb4xx_add_device_spi();

	ar71xx_add_device_mdio(0xfffffffe);

	ar71xx_eth0_data.phy_if_mode = PHY_INTERFACE_MODE_MII;
	ar71xx_eth0_data.phy_mask = 0x00000001;

	ar71xx_add_device_eth(0);

	ar71xx_add_device_leds_gpio(-1, ARRAY_SIZE(rb4xx_leds_gpio),
					rb4xx_leds_gpio);

	ar71xx_add_device_gpio_buttons(-1, RB4XX_BUTTONS_POLL_INTERVAL,
					ARRAY_SIZE(rb4xx_gpio_buttons),
					rb4xx_gpio_buttons);

	platform_device_register(&rb4xx_nand_device);

	ar71xx_pci_init(ARRAY_SIZE(rb4xx_pci_irqs), rb4xx_pci_irqs);
}

MIPS_MACHINE(MACH_AR71XX_RB_411, "MikroTik RouterBOARD 411/A/AH", rb411_setup);

static void __init rb433_setup(void)
{
	rb433_add_device_spi();

	ar71xx_add_device_mdio(0xffffffec);

	ar71xx_eth0_data.phy_if_mode = PHY_INTERFACE_MODE_MII;
	ar71xx_eth0_data.phy_mask = 0x00000003;
	ar71xx_eth0_data.speed = SPEED_100;
	ar71xx_eth0_data.duplex = DUPLEX_FULL;

	ar71xx_eth1_data.phy_if_mode = PHY_INTERFACE_MODE_RMII;
	ar71xx_eth1_data.phy_mask = 0x00000010;

	ar71xx_add_device_eth(1);
	ar71xx_add_device_eth(0);

	ar71xx_add_device_leds_gpio(-1, ARRAY_SIZE(rb4xx_leds_gpio),
					rb4xx_leds_gpio);

	ar71xx_add_device_gpio_buttons(-1, RB4XX_BUTTONS_POLL_INTERVAL,
					ARRAY_SIZE(rb4xx_gpio_buttons),
					rb4xx_gpio_buttons);

	platform_device_register(&rb4xx_nand_device);

	ar71xx_pci_init(ARRAY_SIZE(rb4xx_pci_irqs), rb4xx_pci_irqs);
}

MIPS_MACHINE(MACH_AR71XX_RB_433, "MikroTik RouterBOARD 433/AH", rb433_setup);

static void __init rb450_setup(void)
{
	rb4xx_add_device_spi();

	ar71xx_add_device_mdio(0xffffffe0);

	ar71xx_eth0_data.phy_if_mode = PHY_INTERFACE_MODE_MII;
	ar71xx_eth0_data.phy_mask = 0x0000000f;
	ar71xx_eth0_data.speed = SPEED_100;
	ar71xx_eth0_data.duplex = DUPLEX_FULL;

	ar71xx_eth1_data.phy_if_mode = PHY_INTERFACE_MODE_RMII;
	ar71xx_eth1_data.phy_mask = 0x00000010;

	ar71xx_add_device_eth(1);
	ar71xx_add_device_eth(0);

	ar71xx_add_device_leds_gpio(-1, ARRAY_SIZE(rb4xx_leds_gpio),
					rb4xx_leds_gpio);

	ar71xx_add_device_gpio_buttons(-1, RB4XX_BUTTONS_POLL_INTERVAL,
					ARRAY_SIZE(rb4xx_gpio_buttons),
					rb4xx_gpio_buttons);

	platform_device_register(&rb4xx_nand_device);
}

MIPS_MACHINE(MACH_AR71XX_RB_450, "MikroTik RouterBOARD 450", rb450_setup);

static void __init rb493_setup(void)
{
	rb4xx_add_device_spi();

	ar71xx_add_device_mdio(0x3fffff00);

	ar71xx_eth0_data.phy_if_mode = PHY_INTERFACE_MODE_MII;
	ar71xx_eth0_data.phy_mask = 0;
	ar71xx_eth0_data.speed = SPEED_100;
	ar71xx_eth0_data.duplex = DUPLEX_FULL;

	ar71xx_eth1_data.phy_if_mode = PHY_INTERFACE_MODE_RMII;
	ar71xx_eth1_data.phy_mask = 0x00000001;

	ar71xx_add_device_eth(0);
	ar71xx_add_device_eth(1);

	ar71xx_add_device_leds_gpio(-1, ARRAY_SIZE(rb4xx_leds_gpio),
					rb4xx_leds_gpio);

	ar71xx_add_device_gpio_buttons(-1, RB4XX_BUTTONS_POLL_INTERVAL,
					ARRAY_SIZE(rb4xx_gpio_buttons),
					rb4xx_gpio_buttons);

	platform_device_register(&rb4xx_nand_device);

	ar71xx_pci_init(ARRAY_SIZE(rb4xx_pci_irqs), rb4xx_pci_irqs);
}

MIPS_MACHINE(MACH_AR71XX_RB_493, "MikroTik RouterBOARD 493/AH", rb493_setup);


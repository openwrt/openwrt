/*
 * arch/arm/mach-kirkwood/nas6210-setup.c
 *
 * Raidsonic ICYBOX NAS6210 Board Setup
 *
 * This file is licensed under the terms of the GNU General Public
 * License version 2.  This program is licensed "as is" without any
 * warranty of any kind, whether express or implied.
 */

#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/ata_platform.h>
#include <linux/mtd/partitions.h>
#include <linux/mv643xx_eth.h>
#include <linux/gpio.h>
#include <linux/gpio_keys.h>
#include <linux/input.h>
#include <linux/leds.h>
#include <asm/mach-types.h>
#include <asm/mach/arch.h>
#include <mach/kirkwood.h>
#include "common.h"
#include "mpp.h"

#define NAS6210_GPIO_POWER_OFF	24

static struct mtd_partition nas6210_nand_parts[] = {
	{
		.name = "uboot",
		.offset = 0,
		.size = SZ_512K
	}, {
		.name = "uboot_env",
		.offset = MTDPART_OFS_NXTBLK,
		.size = SZ_128K
	}, {
		.name = "kernel",
		.offset = MTDPART_OFS_NXTBLK,
		.size = 3 * SZ_1M
	}, {
		.name = "rootfs",
		.offset = MTDPART_OFS_NXTBLK,
		.size = MTDPART_SIZ_FULL
	},
};

static struct mv643xx_eth_platform_data nas6210_ge00_data = {
	.phy_addr	= MV643XX_ETH_PHY_ADDR(8),
};

static struct mv_sata_platform_data nas6210_sata_data = {
	.n_ports	= 2,
};

static struct gpio_led nas6210_led_pins[] = {
	{
		.name			= "status:green:power",
		.default_trigger	= "default-on",
		.gpio			= 25,
		.active_low		= 0,
	},
	{
		.name			= "status:red:power",
		.default_trigger	= "none",
		.gpio			= 22,
		.active_low		= 0,
	},
	{
		.name			= "status:red:usb_copy",
		.default_trigger	= "none",
		.gpio			= 27,
		.active_low		= 0,
	},
};

static struct gpio_led_platform_data nas6210_led_data = {
	.leds		= nas6210_led_pins,
	.num_leds	= ARRAY_SIZE(nas6210_led_pins),
};

static struct platform_device nas6210_leds = {
	.name	= "leds-gpio",
	.id	= -1,
	.dev	= {
		.platform_data	= &nas6210_led_data,
	}
};

static struct gpio_keys_button nas6210_buttons[] = {
	{
		.code		= KEY_COPY,
		.gpio		= 29,
		.desc		= "USB Copy",
		.active_low	= 1,
	},
	{
		.code		= KEY_RESTART,
		.gpio		= 28,
		.desc		= "Reset",
		.active_low	= 1,
	},
};

static struct gpio_keys_platform_data nas6210_button_data = {
	.buttons	= nas6210_buttons,
	.nbuttons	= ARRAY_SIZE(nas6210_buttons),
};

static struct platform_device nas6210_button_device = {
	.name		= "gpio-keys",
	.id		= -1,
	.num_resources	= 0,
	.dev		= {
		.platform_data	= &nas6210_button_data,
	}
};

static unsigned int nas6210_mpp_config[] __initdata = {
	MPP0_NF_IO2,
	MPP1_NF_IO3,
	MPP2_NF_IO4,
	MPP3_NF_IO5,
	MPP4_NF_IO6,
	MPP5_NF_IO7,
	MPP18_NF_IO0,
	MPP19_NF_IO1,
 	MPP22_GPIO,	/* Power LED red */
	MPP24_GPIO,	/* Power off device */
	MPP25_GPIO,	/* Power LED green */
	MPP27_GPIO,	/* USB transfer LED */
	MPP28_GPIO,	/* Reset button */
	MPP29_GPIO,	/* USB Copy button */
	0
};

static void nas6210_power_off(void)
{
	gpio_set_value(NAS6210_GPIO_POWER_OFF, 1);
}

static void __init nas6210_init(void)
{
	/*
	 * Basic setup. Needs to be called early.
	 */
	kirkwood_init();
	kirkwood_mpp_conf(nas6210_mpp_config);

	kirkwood_nand_init(ARRAY_AND_SIZE(nas6210_nand_parts), 25);
	kirkwood_ehci_init();
	kirkwood_ge00_init(&nas6210_ge00_data);
	kirkwood_sata_init(&nas6210_sata_data);
	kirkwood_uart0_init();
	platform_device_register(&nas6210_leds);
	platform_device_register(&nas6210_button_device);
	if (gpio_request(NAS6210_GPIO_POWER_OFF, "power-off") == 0 &&
	    gpio_direction_output(NAS6210_GPIO_POWER_OFF, 0) == 0)
		pm_power_off = nas6210_power_off;
	else
		pr_err("nas6210: failed to configure power-off GPIO\n");
}

static int __init nas6210_pci_init(void)
{
	if (machine_is_nas6210()) {
		u32 dev, rev;

		kirkwood_pcie_id(&dev, &rev);
		if (dev == MV88F6282_DEV_ID)
			kirkwood_pcie_init(KW_PCIE1 | KW_PCIE0);
		else
			kirkwood_pcie_init(KW_PCIE0);
	}

	return 0;
}
subsys_initcall(nas6210_pci_init);

MACHINE_START(NAS6210, "RaidSonic ICY BOX IB-NAS6210")
	/* Maintainer: <gmbnomis at gmail dot com> */
	.atag_offset	= 0x100,
	.init_machine	= nas6210_init,
	.map_io		= kirkwood_map_io,
	.init_early	= kirkwood_init_early,
	.init_irq	= kirkwood_init_irq,
	.timer		= &kirkwood_timer,
	.restart	= kirkwood_restart,
MACHINE_END

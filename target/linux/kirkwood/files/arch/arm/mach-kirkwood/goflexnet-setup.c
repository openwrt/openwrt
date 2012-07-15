/*
 * arch/arm/mach-kirkwood/goflexnet-setup.c
 *
 * Seagate GoFlex Net Setup
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
#include <linux/leds.h>
#include <asm/mach-types.h>
#include <asm/mach/arch.h>
#include <mach/kirkwood.h>
#include "common.h"
#include "mpp.h"

static struct mtd_partition goflexnet_nand_parts[] = {
	{
		.name = "u-boot",
		.offset = 0,
		.size = SZ_1M
	}, {
		.name = "uImage",
		.offset = MTDPART_OFS_NXTBLK,
		.size = SZ_4M
	}, {
		.name = "rootfs",
		.offset = MTDPART_OFS_NXTBLK,
		.size = SZ_32M
	}, {
		.name = "data",
		.offset = MTDPART_OFS_NXTBLK,
		.size = MTDPART_SIZ_FULL
	},
};

static struct mv643xx_eth_platform_data goflexnet_ge00_data = {
	.phy_addr	= MV643XX_ETH_PHY_ADDR(0),
};

static struct mv_sata_platform_data goflexnet_sata_data = {
	.n_ports	= 2,
};

static struct gpio_led goflexnet_led_pins[] = {
	{
		.name			= "status:green:health",
		.default_trigger	= "default-on",
		.gpio			= 46, // 0x4000
		.active_low		= 1,
	},
	{
		.name			= "status:orange:fault",
		.default_trigger	= "none",
		.gpio			= 47, // 0x8000
		.active_low		= 1,
	},
	{
		.name			= "status:white:left0",
		.default_trigger	= "none",
		.gpio			= 42, // 0x0400
		.active_low		= 0,
	},
	{
		.name			= "status:white:left1",
		.default_trigger	= "none",
		.gpio			= 43, // 0x0800
		.active_low		= 0,
	},
	{
		.name			= "status:white:left2",
		.default_trigger	= "none",
		.gpio			= 44, // 0x1000
		.active_low		= 0,
	},
	{
		.name			= "status:white:left3",
		.default_trigger	= "none",
		.gpio			= 45, // 0x2000
		.active_low		= 0,
	},
	{
		.name			= "status:white:right0",
		.default_trigger	= "none",
		.gpio			= 38, // 0x0040
		.active_low		= 0,
	},
	{
		.name			= "status:white:right1",
		.default_trigger	= "none",
		.gpio			= 39, // 0x0080
		.active_low		= 0,
	},
	{
		.name			= "status:white:right2",
		.default_trigger	= "none",
		.gpio			= 40, // 0x0100
		.active_low		= 0,
	},
	{
		.name			= "status:white:right3",
		.default_trigger	= "none",
		.gpio			= 41, // 0x0200
		.active_low		= 0,
	}
};

static struct gpio_led_platform_data goflexnet_led_data = {
	.leds		= goflexnet_led_pins,
	.num_leds	= ARRAY_SIZE(goflexnet_led_pins),
};

static struct platform_device goflexnet_leds = {
	.name	= "leds-gpio",
	.id	= -1,
	.dev	= {
		.platform_data	= &goflexnet_led_data,
	}
};

static unsigned int goflexnet_mpp_config[] __initdata = {
	MPP29_GPIO,	/* USB Power Enable */
	MPP47_GPIO,	/* LED Orange */
	MPP46_GPIO,	/* LED Green */
	MPP45_GPIO,	/* LED Left Capacity 3 */
	MPP44_GPIO,	/* LED Left Capacity 2 */
	MPP43_GPIO,	/* LED Left Capacity 1 */
	MPP42_GPIO,	/* LED Left Capacity 0 */
	MPP41_GPIO,	/* LED Right Capacity 3 */
	MPP40_GPIO,	/* LED Right Capacity 2 */
	MPP39_GPIO,	/* LED Right Capacity 1 */
	MPP38_GPIO,	/* LED Right Capacity 0 */
	0
};

static void __init goflexnet_init(void)
{
	/*
	 * Basic setup. Needs to be called early.
	 */
	kirkwood_init();

	/* setup gpio pin select */
	kirkwood_mpp_conf(goflexnet_mpp_config);

	kirkwood_uart0_init();
	kirkwood_nand_init(ARRAY_AND_SIZE(goflexnet_nand_parts), 40);

	if (gpio_request(29, "USB Power Enable") != 0 ||
	    gpio_direction_output(29, 1) != 0)
		printk(KERN_ERR "can't set up GPIO 29 (USB Power Enable)\n");
	kirkwood_ehci_init();
	kirkwood_ge00_init(&goflexnet_ge00_data);
	kirkwood_sata_init(&goflexnet_sata_data);

	platform_device_register(&goflexnet_leds);
}

MACHINE_START(GOFLEXNET, "Seagate GoFlex Net")
	/* Maintainer: Peter Carmichael <peterjncarm@ovi.com> */
	.atag_offset	= 0x100,
	.init_machine	= goflexnet_init,
	.map_io		= kirkwood_map_io,
	.init_early	= kirkwood_init_early,
	.init_irq	= kirkwood_init_irq,
	.timer		= &kirkwood_timer,
	.restart	= kirkwood_restart,
MACHINE_END

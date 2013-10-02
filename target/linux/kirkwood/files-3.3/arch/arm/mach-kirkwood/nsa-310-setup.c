/*
 * arch/arm/mach-kirkwood/nsa-310-setup.c
 *
 * ZyXEL NSA-310 Setup
 *
 * This file is licensed under the terms of the GNU General Public
 * License version 2.  This program is licensed "as is" without any
 * warranty of any kind, whether express or implied.
 */

#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/ata_platform.h>
#include <linux/i2c.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/partitions.h>
#include <linux/gpio.h>
#include <linux/gpio_keys.h>
#include <linux/input.h>
#include <linux/leds.h>
#include <linux/delay.h>

#include <asm/mach-types.h>
#include <asm/mach/arch.h>
#include <mach/kirkwood.h>
#include "common.h"
#include "mpp.h"

#define NSA310_GPIO_LED_ESATA_GREEN	12
#define NSA310_GPIO_LED_ESATA_RED	13
#define NSA310_GPIO_LED_USB_GREEN	15
#define NSA310_GPIO_LED_USB_RED		16
#define NSA310_GPIO_USB_POWER_OFF	21
#define NSA310_GPIO_LED_SYS_GREEN	28
#define NSA310_GPIO_LED_SYS_RED		29
#define NSA310_GPIO_KEY_RESTART		36
#define NSA310_GPIO_KEY_COPY		37
#define NSA310_GPIO_LED_COPY_GREEN	39
#define NSA310_GPIO_LED_COPY_RED	40
#define NSA310_GPIO_LED_HDD_GREEN	41
#define NSA310_GPIO_LED_HDD_RED		42
#define NSA310_GPIO_BUZZER		44
#define NSA310_GPIO_KEY_POWER		46
#define NSA310_GPIO_POWER_OFF		48


static unsigned int nsa310_mpp_config[] __initdata = {
	MPP12_GPIO,
	MPP13_GPIO,
	MPP15_GPIO,
	MPP16_GPIO,
	MPP21_GPIO,
	MPP28_GPIO,
	MPP29_GPIO,
	MPP36_GPIO,
	MPP37_GPIO,
	MPP39_GPIO,
	MPP40_GPIO,
	MPP41_GPIO,
	MPP42_GPIO,
	MPP44_GPIO,
	MPP46_GPIO,
	MPP48_GPIO,
	0
};

static struct mtd_partition nsa310_mtd_parts[] = {
	{
		.name	= "uboot",
		.offset	= 0,
		.size	= 0x100000,
		.mask_flags = MTD_WRITEABLE,
	}, {
		.name	= "uboot_env",
		.offset	= MTDPART_OFS_NXTBLK,
		.size	= 0x80000,
	}, {
		.name	= "key_store",
		.offset	= MTDPART_OFS_NXTBLK,
		.size	= 0x80000,
	}, {
		.name	= "info",
		.offset	= MTDPART_OFS_NXTBLK,
		.size	= 0x80000,
	}, {
		.name	= "etc",
		.offset	= MTDPART_OFS_NXTBLK,
		.size	= 0xa00000,
	}, {
		.name	= "kernel_1",
		.offset	= MTDPART_OFS_NXTBLK,
		.size	= 0xa00000,
	}, {
		.name	= "rootfs1",
		.offset	= MTDPART_OFS_NXTBLK,
		.size	= 0x2fc0000,
	}, {
		.name	= "kernel_2",
		.offset	= MTDPART_OFS_NXTBLK,
		.size	= 0xa00000,
	}, {
		.name	= "rootfs2",
		.offset	= MTDPART_OFS_NXTBLK,
		.size	= 0x2fc0000,
	},
};

static struct gpio_led nsa310_leds[] = {
	{
		.name	= "nsa310:green:sys",
		.gpio	= NSA310_GPIO_LED_SYS_GREEN,
	}, {
		.name	= "nsa310:red:sys",
		.gpio	= NSA310_GPIO_LED_SYS_RED,
	}, {
		.name	= "nsa310:green:hdd",
		.gpio	= NSA310_GPIO_LED_HDD_GREEN,
	}, {
		.name	= "nsa310:red:hdd",
		.gpio	= NSA310_GPIO_LED_HDD_RED,
	}, {
		.name	= "nsa310:green:esata",
		.gpio	= NSA310_GPIO_LED_ESATA_GREEN,
	}, {
		.name	= "nsa310:red:esata",
		.gpio	= NSA310_GPIO_LED_ESATA_RED,
	}, {
		.name	= "nsa310:green:usb",
		.gpio	= NSA310_GPIO_LED_USB_GREEN,
	}, {
		.name	= "nsa310:red:usb",
		.gpio	= NSA310_GPIO_LED_USB_RED,
	}, {
		.name	= "nsa310:green:copy",
		.gpio	= NSA310_GPIO_LED_COPY_GREEN,
	}, {
		.name	= "nsa310:red:copy",
		.gpio	= NSA310_GPIO_LED_COPY_RED,
	},
};

static struct gpio_led_platform_data nsa310_leds_data = {
	.leds		= nsa310_leds,
	.num_leds	= ARRAY_SIZE(nsa310_leds),
};

static struct platform_device nsa310_leds_device = {
	.name	= "leds-gpio",
	.id	= -1,
	.dev	= {
		.platform_data	= &nsa310_leds_data,
	}
};

static struct gpio_keys_button nsa310_buttons[] = {
	{
		.desc		= "Power Button",
		.code		= KEY_POWER,
		.type		= EV_KEY,
		.gpio		= NSA310_GPIO_KEY_POWER,
		.debounce_interval = 1000,
	}, {
		.desc		= "Copy Button",
		.code		= KEY_COPY,
		.type		= EV_KEY,
		.gpio		= NSA310_GPIO_KEY_COPY,
		.active_low	= 1,
		.debounce_interval = 1000,
	}, {
		.desc		= "Reset Button",
		.code		= KEY_RESTART,
		.type		= EV_KEY,
		.gpio		= NSA310_GPIO_KEY_RESTART,
		.active_low	= 1,
		.debounce_interval = 1000,
	},
};

static struct gpio_keys_platform_data nsa310_keys_data = {
	.buttons	= nsa310_buttons,
	.nbuttons	= ARRAY_SIZE(nsa310_buttons),
};

static struct platform_device nsa310_keys_device = {
	.name	= "gpio-keys",
	.id	= -1,
	.dev = {
		.platform_data  = &nsa310_keys_data,
	}
};

static struct i2c_board_info __initdata nsa310_i2c_info[] = {
        { I2C_BOARD_INFO("adt7476", 0x2e) },
};

static struct mv_sata_platform_data nsa310_sata_data = {
	.n_ports	= 2,
};

static void nsa310_power_off(void)
{
	gpio_set_value(NSA310_GPIO_POWER_OFF, 1);
}

static int __init nsa310_gpio_request(unsigned int gpio, unsigned long flags,
				       const char *label)
{
	int err;

	err = gpio_request_one(gpio, flags, label);
	if (err)
		pr_err("NSA-310: can't setup GPIO%u (%s), err=%d\n",
			gpio, label, err);

	return err;
}

static void __init nsa310_gpio_init(void)
{
	int err;

	err = nsa310_gpio_request(NSA310_GPIO_POWER_OFF, GPIOF_OUT_INIT_LOW,
				  "Power Off");
	if (!err)
		pm_power_off = nsa310_power_off;

	nsa310_gpio_request(NSA310_GPIO_USB_POWER_OFF, GPIOF_OUT_INIT_LOW,
			    "USB Power Off");
}

static void __init nsa310_init(void)
{
	u32 dev, rev;

	kirkwood_init();
	kirkwood_mpp_conf(nsa310_mpp_config);

	nsa310_gpio_init();

	kirkwood_nand_init(ARRAY_AND_SIZE(nsa310_mtd_parts), 35);
	kirkwood_ehci_init();

	kirkwood_pcie_id(&dev, &rev);

	kirkwood_sata_init(&nsa310_sata_data);
	kirkwood_uart0_init();

	i2c_register_board_info(0, ARRAY_AND_SIZE(nsa310_i2c_info));
	kirkwood_i2c_init();

	platform_device_register(&nsa310_leds_device);
	platform_device_register(&nsa310_keys_device);
}

static int __init nsa310_pci_init(void)
{
	if (machine_is_nsa310())
		kirkwood_pcie_init(KW_PCIE0);

	return 0;
}
subsys_initcall(nsa310_pci_init);

MACHINE_START(NSA310, "ZyXEL NSA-310")
	.atag_offset	= 0x100,
	.init_machine	= nsa310_init,
	.map_io		= kirkwood_map_io,
	.init_early	= kirkwood_init_early,
	.init_irq	= kirkwood_init_irq,
	.timer		= &kirkwood_timer,
	.restart	= kirkwood_restart,
MACHINE_END

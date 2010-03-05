/*
 *  Copyright (C) 2009, Lars-Peter Clausen <lars@metafoo.de>
 *  	N526 eBook reader support
 *
 *  This program is free software; you can redistribute	 it and/or modify it
 *  under  the terms of	 the GNU General  Public License as published by the
 *  Free Software Foundation;  either version 2 of the	License, or (at your
 *  option) any later version.
 *
 *  You should have received a copy of the  GNU General Public License along
 *  with this program; if not, write  to the Free Software Foundation, Inc.,
 *  675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */

#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/gpio.h>

#include <asm/mach-jz4740/platform.h>

#include <linux/mtd/jz4740_nand.h>
#include <linux/jz4740_fb.h>
#include <linux/power_supply.h>
#include <linux/mmc/jz4740_mmc.h>

#include <video/broadsheetfb.h>
#include <linux/delay.h>
#include <linux/interrupt.h>

#include <linux/input.h>
#include <linux/gpio_keys.h>

#include <linux/i2c.h>
#include <linux/i2c-gpio.h>

#include "clock.h"

/* NAND */
static struct nand_ecclayout n526_ecclayout = {
	.eccbytes = 36,
	.eccpos = {
		6,  7,  8,  9,  10, 11, 12, 13,
		14, 15, 16, 17, 18, 19, 20, 21,
		22, 23, 24, 25, 26, 27, 28, 29,
		30, 31, 32, 33, 34, 35, 36, 37,
		38, 39, 40, 41},
	.oobfree = {
		{.offset = 2,
		 .length = 4},
		{.offset = 42,
		 .length = 22}}
};

static struct mtd_partition n526_partitions[] = {
	{ .name = "NAND BOOT partition",
	  .offset = 0 * 0x100000,
	  .size = 4 * 0x100000,
 	},
	{ .name = "NAND KERNEL partition",
	  .offset = 4 * 0x100000,
	  .size = 4 * 0x100000,
 	},
	{ .name = "NAND ROOTFS partition",
	  .offset = 16 * 0x100000,
	  .size = 498 * 0x100000,
 	},
};

static struct jz_nand_platform_data n526_nand_pdata = {
	.ecc_layout = &n526_ecclayout,
	.partitions = n526_partitions,
	.num_partitions = ARRAY_SIZE(n526_partitions),
	.busy_gpio = 94,
};

static struct jz4740_mmc_platform_data n526_mmc_pdata = {
	.gpio_card_detect	= JZ_GPIO_PORTD(7),
	.card_detect_active_low = 1,
	.gpio_read_only		= -1,
	.gpio_power		= JZ_GPIO_PORTD(17),
	.power_active_low = 1,
};

static struct gpio_led n526_leds[] = {
	{
		.name = "n526:blue:power",
		.gpio = JZ_GPIO_PORTD(28),
		.default_state = LEDS_GPIO_DEFSTATE_ON,
	}
};

static struct gpio_led_platform_data n526_leds_pdata = {
	.leds = n526_leds,
	.num_leds = ARRAY_SIZE(n526_leds),
};

static struct platform_device n526_leds_device = {
	.name = "leds-gpio",
	.id = -1,
	.dev = {
		.platform_data = &n526_leds_pdata,
	},
};

static void __init board_gpio_setup(void)
{
	/* We only need to enable/disable pullup here for pins used in generic
	 * drivers. Everything else is done by the drivers themselfs. */
	jz_gpio_disable_pullup(JZ_GPIO_PORTD(17));
	jz_gpio_enable_pullup(JZ_GPIO_PORTD(7));
	jz_gpio_disable_pullup(JZ_GPIO_PORTC(19));
	jz_gpio_disable_pullup(JZ_GPIO_PORTC(20));
	jz_gpio_disable_pullup(JZ_GPIO_PORTC(21));
	jz_gpio_disable_pullup(JZ_GPIO_PORTC(23));
}


static const int n526_eink_ctrl_gpios[] = {
	0,
	JZ_GPIO_PORTC(23),
	JZ_GPIO_PORTC(19),
	JZ_GPIO_PORTC(20),
};

static void n526_eink_set_ctl(struct broadsheetfb_par * par, unsigned char ctrl, u8
value)
{
	gpio_set_value(n526_eink_ctrl_gpios[ctrl], value);
}


static int n526_eink_wait(struct broadsheetfb_par *par)
{
	wait_event(par->waitq, gpio_get_value(JZ_GPIO_PORTB(17)));

	return 0;
}

static u16 n526_eink_get_hdb(struct broadsheetfb_par *par)
{
	u16 value = 0;
	jz_gpio_port_direction_input(JZ_GPIO_PORTC(0), 0xffff);
	gpio_set_value(JZ_GPIO_PORTC(21), 0);
	mdelay(100);

	value = jz_gpio_port_get_value(JZ_GPIO_PORTC(0), 0xffff);

	gpio_set_value(JZ_GPIO_PORTC(21), 1);
	jz_gpio_port_direction_output(JZ_GPIO_PORTC(0), 0xffff);
	return value;
}

static void n526_eink_set_hdb(struct broadsheetfb_par *par, u16 value)
{
	jz_gpio_port_set_value(JZ_GPIO_PORTC(0), value, 0xffff);
}

static int n526_eink_init(struct broadsheetfb_par *par)
{
	int i;

	gpio_request(JZ_GPIO_PORTD(1), "display reset");
	gpio_direction_output(JZ_GPIO_PORTD(1), 1);
	mdelay(10);
	gpio_set_value(JZ_GPIO_PORTD(1), 0);

	gpio_request(JZ_GPIO_PORTB(18), "eink enable");
	gpio_direction_output(JZ_GPIO_PORTB(18), 0);

	gpio_request(JZ_GPIO_PORTB(29), "foobar");
	gpio_direction_output(JZ_GPIO_PORTB(29), 1);

	for(i = 1; i < ARRAY_SIZE(n526_eink_ctrl_gpios); ++i) {
		gpio_request(n526_eink_ctrl_gpios[i], "eink display ctrl");
		gpio_direction_output(n526_eink_ctrl_gpios[i], 0);
	}

	gpio_request(JZ_GPIO_PORTC(22), "foobar");
	gpio_direction_input(JZ_GPIO_PORTC(22));
	gpio_request(JZ_GPIO_PORTC(21), "eink nRD");
	gpio_direction_output(JZ_GPIO_PORTC(21), 1);

	for(i = 0; i < 16; ++i) {
		gpio_request(JZ_GPIO_PORTC(i), "eink display data");
	}
	jz_gpio_port_direction_output(JZ_GPIO_PORTC(0), 0xffff);

	gpio_set_value(JZ_GPIO_PORTB(18), 1);

	return 0;
}

static irqreturn_t n526_eink_busy_irq(int irq, void *devid)
{
	struct broadsheetfb_par *par = devid;
	wake_up(&par->waitq);

	return IRQ_HANDLED;
}

static int n526_eink_setup_irq(struct fb_info *info)
{
	int ret;
	struct broadsheetfb_par *par = info->par;

	gpio_request(JZ_GPIO_PORTB(17), "eink busy");
	gpio_direction_input(JZ_GPIO_PORTB(17));

	ret = request_irq(gpio_to_irq(JZ_GPIO_PORTB(17)), n526_eink_busy_irq,
				IRQF_DISABLED | IRQF_TRIGGER_RISING,
				"eink busyline", par);
	if (ret)
		printk("n526 display: Failed to request busyline irq: %d\n", ret);
	return 0;
}

static void n526_eink_cleanup(struct broadsheetfb_par *par)
{
}

static struct broadsheet_board broadsheet_pdata = {
	.owner = THIS_MODULE,
	.init = n526_eink_init,
	.wait_for_rdy = n526_eink_wait,
	.set_ctl = n526_eink_set_ctl,
	.set_hdb = n526_eink_set_hdb,
	.get_hdb = n526_eink_get_hdb,
	.cleanup = n526_eink_cleanup,
	.setup_irq = n526_eink_setup_irq,
};

static struct platform_device n526_broadsheet_device = {
	.name = "broadsheetfb",
	.id = -1,
	.dev = {
		.platform_data = &broadsheet_pdata,
	},
};

static struct gpio_keys_button qi_lb60_gpio_keys_buttons[] = {
	[0] = {
		.code		= KEY_ENTER,
		.gpio		= 0,
		.active_low	= 1,
		.desc		= "Power",
	},
};

static struct gpio_keys_platform_data qi_lb60_gpio_keys_data = {
	.nbuttons = ARRAY_SIZE(qi_lb60_gpio_keys_buttons),
	.buttons = qi_lb60_gpio_keys_buttons,
};

static struct platform_device qi_lb60_gpio_keys = {
	.name =	"gpio-keys",
	.id =	-1,
	.dev = {
		.platform_data = &qi_lb60_gpio_keys_data,
	}
};

static struct i2c_gpio_platform_data n526_i2c_pdata = {
	.sda_pin = JZ_GPIO_PORTD(23),
	.scl_pin = JZ_GPIO_PORTD(24),
	.udelay = 2,
	.timeout = 3 * HZ,
};

static struct platform_device n526_i2c_device = {
	.name = "i2c-gpio",
	.id = -1,
	.dev = {
		.platform_data = &n526_i2c_pdata,
	},
};

static struct i2c_board_info n526_i2c_board_info = {
	.type = "n526-lpc",
	.addr = 0x54,
};

static struct platform_device *jz_platform_devices[] __initdata = {
	&jz4740_usb_ohci_device,
	&jz4740_usb_gdt_device,
	&jz4740_mmc_device,
	&jz4740_nand_device,
	&jz4740_i2s_device,
	&jz4740_codec_device,
	&jz4740_rtc_device,
	&n526_leds_device,
	&n526_broadsheet_device,
	&qi_lb60_gpio_keys,
	&n526_i2c_device,
};


static int __init n526_init_platform_devices(void)
{

	jz4740_nand_device.dev.platform_data = &n526_nand_pdata;
	jz4740_mmc_device.dev.platform_data = &n526_mmc_pdata;

	n526_i2c_board_info.irq = gpio_to_irq(JZ_GPIO_PORTD(14)),
	i2c_register_board_info(0, &n526_i2c_board_info, 1);

	return platform_add_devices(jz_platform_devices,
					ARRAY_SIZE(jz_platform_devices));

}

struct jz4740_clock_board_data jz4740_clock_bdata = {
	.ext_rate = 12000000,
	.rtc_rate = 32768,
};

extern int jz_gpiolib_init(void);

static int __init n526_board_setup(void)
{
	if (jz_gpiolib_init())
		panic("Failed to initalize jz gpio\n");

	jz4740_clock_init();
	board_gpio_setup();

	if (n526_init_platform_devices())
		panic("Failed to initalize platform devices\n");

	return 0;
}

arch_initcall(n526_board_setup);

/*
 * arch/ubicom32/mach-ip7k/board-ip7500av.c
 *   Support for IP7500 Audio Video Board + CPU module board.
 *
 * This file supports the IP7500 Audio Video Board:
 *	8007-0810  Rev 1.0
 * with one of the following CPU module boards:
 *	8007-0510  Rev 1.0
 *	8007-0510A Rev 1.0 (with ethernet)
 *
 * DIP Switch SW2 configuration: (*) default
 *	POS 1: on(*) = PCI enabled, off = PCI disabled
 *	POS 2: on(*) = TTYX => PA6, off = TTYX => PF12
 *	POS 3: on(*) = TTYY => PA7, off = TTYY => PF15
 *	POS 4: unused
 *
 * (C) Copyright 2009, Ubicom, Inc.
 *
 * This file is part of the Ubicom32 Linux Kernel Port.
 *
 * The Ubicom32 Linux Kernel Port is free software: you can redistribute
 * it and/or modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation, either version 2 of the
 * License, or (at your option) any later version.
 *
 * The Ubicom32 Linux Kernel Port is distributed in the hope that it
 * will be useful, but WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See
 * the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with the Ubicom32 Linux Kernel Port.  If not,
 * see <http://www.gnu.org/licenses/>.
 */

#include <linux/platform_device.h>
#include <linux/device.h>
#include <linux/gpio.h>
#include <linux/i2c.h>
#include <linux/i2c-gpio.h>
#include <linux/delay.h>
#include <asm/board.h>
#include <asm/machdep.h>
#include <asm/ring_tio.h>
#include <asm/vdc_tio.h>
#include <asm/audio.h>
#include <asm/ubi32-pcm.h>
#include <asm/ubi32-cs4384.h>

/******************************************************************************
 * Devices on the I2C bus
 *
 * BEWARE of changing the order of things in this array as we depend on
 * certain things to be in certain places.
 */
static struct i2c_board_info __initdata ip7500av_i2c_board_info[] = {
	/*
	 * U6, CS4384 DAC, address 0x19
	 */
	{
		.type		= "cs4384",
		.addr		= 0x19,
	},
};

/*
 * I2C bus on the board, SDA PD1, SCL PD2
 */
static struct i2c_gpio_platform_data ip7500av_i2c_data = {
	.sda_pin		= GPIO_RD_6,
	.scl_pin		= GPIO_RD_3,
	.sda_is_open_drain	= 0,
	.scl_is_open_drain	= 0,
	.udelay			= 50,
};

static struct platform_device ip7500av_i2c_device = {
	.name	= "i2c-gpio",
	.id	= 0,
	.dev	= {
		.platform_data = &ip7500av_i2c_data,
	},
};

/*
 * List of possible mclks we can generate.  This depends on the CPU frequency.
 */
static struct ubi32_cs4384_mclk_entry ip7500av_cs4384_mclk_entries[] = {
	{
		.rate	=	12288000,
		.div	=	44,
	},
	{
		.rate	=	11289600,
		.div	=	48,
	},
};

/*
 * List of all devices in our system
 */
static struct platform_device *ip7500av_devices[] __initdata = {
	&ip7500av_i2c_device,
};

/*
 * ip7500av_vdac_write
 */
static int __init ip7500av_vdac_write(int reg, int val)
{
	struct i2c_adapter *adap;
	struct i2c_msg msg[1];
	unsigned char data[2];
	int err;

	adap = i2c_get_adapter(0);
	if (!adap) {
		printk(KERN_WARNING "%s: failed to get i2c adapter\n", __FUNCTION__);
		return -ENODEV;
	}
	msg->addr = 0x2B;
	msg->flags = 0;
	msg->len = 2;
	msg->buf = data;
	data[0] = reg;
	data[1] = val;
	err = i2c_transfer(adap, msg, 1);
	i2c_put_adapter(adap);
	if (err >= 0) {
		return 0;
	}
	return err;
}

/*
 * ip7500av_vdac_init
 *	Initializes the video DAC via I2C
 *
 * Equivalent mode line: 720x480p = 27 Mhz, 720 736 800 858 480 484 492 525
 */
static int __init ip7500av_vdac_init(void)
{
	int err;

	printk(KERN_INFO "Initializing ADV7393 DAC\n");

	/*
	 * Reset the VDAC
	 */
	if (gpio_request(GPIO_RF_6, "VDAC Reset")) {
		printk(KERN_WARNING "%s: failed to allocate VDAC Reset\n", __FUNCTION__);
		return -EBUSY;
	}
	gpio_direction_output(GPIO_RF_6, 0);
	udelay(1);
	gpio_set_value(GPIO_RF_6, 1);

	/*
	 * See table 100 of ADV7393 data sheet: 16-bit 525p YCrCb In, YPbPr Out
	 */
	err = ip7500av_vdac_write(0x17, 0x02);
	if (err) {
		printk(KERN_WARNING "%s: failed to write VDAC\n", __FUNCTION__);
		return err;
	}
	err = ip7500av_vdac_write(0x00, 0x1c);
	if (err) {
		printk(KERN_WARNING "%s: failed to write VDAC\n", __FUNCTION__);
		return err;
	}
	err = ip7500av_vdac_write(0x01, 0x10);
	if (err) {
		printk(KERN_WARNING "%s: failed to write VDAC\n", __FUNCTION__);
		return err;
	}
	err = ip7500av_vdac_write(0x31, 0x01);
	if (err) {
		printk(KERN_WARNING "%s: failed to write VDAC\n", __FUNCTION__);
		return err;
	}
#ifdef IP7500AV_VDAC_SWAP_PBPR
	err = ip7500av_vdac_write(0x35, 0x08);
	if (err) {
		printk(KERN_WARNING "%s: failed to write VDAC\n", __FUNCTION__);
		return err;
	}
#endif
#ifdef IP7500AV_VDAC_FULL_RANGE
	err = ip7500av_vdac_write(0x30, 0x02);
	if (err) {
		printk(KERN_WARNING "%s: failed to write VDAC\n", __FUNCTION__);
		return err;
	}
#endif
	return 0;
}
late_initcall(ip7500av_vdac_init);

/*
 * ip7500av_init
 *	Called to add the devices which we have on this board
 */
static int __init ip7500av_init(void)
{
	struct platform_device *audio_dev;
	struct platform_device *audio_dev2;
	struct ubi32_cs4384_platform_data *cs4384_pd;

	board_init();

	ubi_gpio_init();

	vdc_tio_init();

	printk(KERN_INFO "%s: registering device resources\n", __FUNCTION__);
	platform_add_devices(ip7500av_devices, ARRAY_SIZE(ip7500av_devices));

	/*
	 * CS4384 DAC
	 */
	audio_dev = audio_device_alloc("snd-ubi32-cs4384", "audio", "audio-i2sout",
			sizeof(struct ubi32_cs4384_platform_data));
	if (audio_dev) {
		/*
		 * Attempt to figure out a good divisor.  This will only work
		 * assuming the core frequency is compatible.
		 */
		int i;
		unsigned int freq = processor_frequency();
		for (i = 0; i < ARRAY_SIZE(ip7500av_cs4384_mclk_entries); i++) {
			unsigned int div;
			unsigned int rate = ip7500av_cs4384_mclk_entries[i].rate / 1000;
			div = ((freq / rate) + 500) / 1000;
			ip7500av_cs4384_mclk_entries[i].div = div;
			printk("CS4384 mclk %d rate %u000Hz div %u act %u\n", i, rate, div, freq / div);
		}

		cs4384_pd = audio_device_priv(audio_dev);
		cs4384_pd->mclk_src = UBI32_CS4384_MCLK_PWM_0;
		cs4384_pd->n_mclk = ARRAY_SIZE(ip7500av_cs4384_mclk_entries);
		cs4384_pd->mclk_entries = ip7500av_cs4384_mclk_entries;
		ip7500av_i2c_board_info[0].platform_data = audio_dev;

		/*
		 * Reset the DAC
		 */
		if (gpio_request(GPIO_RF_4, "DAC Reset") == 0) {
			gpio_direction_output(GPIO_RF_4, 0);
			udelay(1);
			gpio_direction_output(GPIO_RF_4, 1);
		} else {
			printk("Unable to request DAC reset GPIO\n");
		}
	}

	/*
	 * SPDIF port
	 */
	audio_dev2 = audio_device_alloc("snd-ubi32-generic", "audio", "audio-spdifout", 0);
	if (audio_dev2) {
		platform_device_register(audio_dev2);
	}

	/*
	 * Register all of the devices which sit on the I2C bus
	 */
	printk(KERN_INFO "%s: registering i2c resources\n", __FUNCTION__);
	i2c_register_board_info(0, ip7500av_i2c_board_info, ARRAY_SIZE(ip7500av_i2c_board_info));

	printk(KERN_INFO "IP7500 Audio/Video Board\n");
	return 0;
}
arch_initcall(ip7500av_init);

/*
 * arch/ubicom32/mach-ip7k/board-ip7160dpf.c
 *   Platform initialization for ip7160dpf board.
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
 *
 * Ubicom32 implementation derived from (with many thanks):
 *   arch/m68knommu
 *   arch/blackfin
 *   arch/parisc
 */
#include <linux/device.h>
#include <linux/platform_device.h>
#include <linux/delay.h>
#include <linux/gpio.h>

#include <linux/i2c.h>
#include <linux/i2c-gpio.h>

#include <linux/input.h>

#include <asm/board.h>
#include <asm/machdep.h>
#include <asm/ubicom32hid.h>
#include <asm/vdc_tio.h>
#include <asm/audio.h>

/*
 * Backlight on the board PD0, hardware PWM
 */
static const struct ubicom32hid_button ip7160dpf_ubicom32hid_buttons[] = {
	{
		.type	= EV_KEY,
		.code	= KEY_UP,
		.bit	= 0,
	},
	{
		.type	= EV_KEY,
		.code	= KEY_LEFT,
		.bit	= 1,
	},
	{
		.type	= EV_KEY,
		.code	= KEY_RIGHT,
		.bit	= 2,
	},
	{
		.type	= EV_KEY,
		.code	= KEY_DOWN,
		.bit	= 3,
	},
	{
		.type	= EV_KEY,
		.code	= KEY_ENTER,
		.bit	= 4,
	},
	{
		.type	= EV_KEY,
		.code	= KEY_MENU,
		.bit	= 5,
	},
	{
		.type	= EV_KEY,
		.code	= KEY_ESC,
		.bit	= 7,
	},
};

static const struct ubicom32hid_ir ip7160dpf_ubicom32hid_ircodes[] = {
	{
		.type		= EV_KEY,
		.code		= KEY_UP,
		.ir_code	= 0xF807916E
	},
	{
		.type		= EV_KEY,
		.code		= KEY_DOWN,
		.ir_code	= 0xF20D916E
	},
	{
		.type		= EV_KEY,
		.code		= KEY_LEFT,
		.ir_code	= 0xF609916E
	},
	{
		.type		= EV_KEY,
		.code		= KEY_RIGHT,
		.ir_code	= 0xF40B916E
	},
	{
		.type		= EV_KEY,
		.code		= KEY_ENTER,
		.ir_code	= 0xF50A916E
	},
	{	/* rotate */
		.type		= EV_KEY,
		.code		= KEY_FN_F1,
		.ir_code	= 0xF906916E
	},
	{
		.type		= EV_KEY,
		.code		= KEY_MENU,
		.ir_code	= 0xF708916E
	},
	{	/* font size */
		.type		= EV_KEY,
		.code		= KEY_FN_F2,
		.ir_code	= 0xF30C916E
	},
	{
		.type		= EV_KEY,
		.code		= KEY_ESC,
		.ir_code	= 0xF10E916E
	},
	{
		.type		= EV_KEY,
		.code		= KEY_VOLUMEUP,
		.ir_code	= 0xF00F916E
	},
	{
		.type		= EV_KEY,
		.code		= KEY_VOLUMEDOWN,
		.ir_code	= 0xED12916E
	},
	{
		.type		= EV_KEY,
		.code		= KEY_MUTE,
		.ir_code	= 0xEA15916E
	},
	{
		.type		= EV_KEY,
		.code		= KEY_INFO,
		.ir_code	= 0xEF10916E
	},
	{	/* Like */
		.type		= EV_KEY,
		.code		= KEY_FN_F3,
		.ir_code	= 0xEE11916E
	},
	{	/* Dislike */
		.type		= EV_KEY,
		.code		= KEY_FN_F4,
		.ir_code	= 0xEB14916E
	},
	{
		.type		= EV_KEY,
		.code		= KEY_POWER,
		.ir_code	= 0xFD02916E
	},
};

static struct ubicom32hid_platform_data ip7160dpf_ubicom32hid_platform_data = {
	.gpio_reset		= GPIO_RI_5,
	.gpio_reset_polarity	= 0,
	.type			= UBICOM32HID_BL_TYPE_PWM,
	.invert			= 0,
	.default_intensity	= 128,
	.buttons		= ip7160dpf_ubicom32hid_buttons,
	.nbuttons		= ARRAY_SIZE(ip7160dpf_ubicom32hid_buttons),
	.ircodes		= ip7160dpf_ubicom32hid_ircodes,
	.nircodes		= ARRAY_SIZE(ip7160dpf_ubicom32hid_ircodes),
};

/*
 * Devices on the I2C bus
 *	This board has a "bus 2" which is isolated from the main bus by U47
 *	and pin RI0.  It should be safe to always enable bus 2 by setting
 *	RI0 to low, however, it should be noted that on all existing configurations
 *	of this board, U49 and U51 are not populated.
 */
static struct i2c_board_info __initdata ip7160dpf_i2c_board_info[] = {
	/*
	 * U37, CS4350 DAC, address 0x4B, bus 2
	 *	THIS ENTRY MUST BE FIRST
	 */
	{
		.type		= "cs4350",
		.addr		= 0x4B,
	}

	/*
	 * U24, ubicom32hid
	 */
	{
		.type		= "ubicom32hid",
		.addr		= 0x08,
		.platform_data	= &ip7160dpf_ubicom32hid_platform_data,
	},

	/*
	 * U49, ISL29001 Ambient Light Sensor, address 0x44, bus 2 (may not be stuffed)
	 */

	/*
	 * U51, S35390A RTC, address 0x30, bus 2 (may not be stuffed)
	 */
#ifdef CONFIG_RTC_DRV_S35390A
	{
		.type           = "s35390a",
		.addr           = 0x30,
	},
#endif
};

/*
 * I2C bus on the board, SDA PI1, SCL PI2
 */
static struct i2c_gpio_platform_data ip7160dpf_i2c_data = {
	.sda_pin		= GPIO_RI_1,
	.scl_pin		= GPIO_RI_2,
	.sda_is_open_drain	= 0,
	.scl_is_open_drain	= 0,
	.scl_is_output_only	= 1,
	.udelay			= 6,
};

static struct platform_device ip7160dpf_i2c_device = {
	.name	= "i2c-gpio",
	.id	= 0,
	.dev	= {
		.platform_data = &ip7160dpf_i2c_data,
	},
};

/*
 * List of all devices in our system
 */
static struct platform_device *ip7160dpf_devices[] __initdata = {
	&ip7160dpf_i2c_device,
};

/*
 * ip7160dpf_power_off
 *	Called to turn the power off for this board
 */
static void ip7160dpf_power_off(void)
{
	gpio_set_value(GPIO_RF_14, 0);
}

/*
 * ip7160dpf_init
 *	Called to add the devices which we have on this board
 */
static int __init ip7160dpf_init(void)
{
	int ret;
	struct platform_device *audio_dev;

	ubi_gpio_init();

	/*
	 * Hold the POWER_HOLD line
	 */
	ret = gpio_request(GPIO_RF_14, "POWER_HOLD");
	if (ret) {
		printk(KERN_ERR "%s: could not request POWER_HOLD GPIO\n", __FUNCTION__);
	}
	gpio_direction_output(GPIO_RF_14, 1);
	mach_power_off = ip7160dpf_power_off;

	/*
	 * USB SEL_HOST_USB line
	 */
	ret = gpio_request(GPIO_RI_13, "SEL_HOST_USB");
	if (ret) {
		printk(KERN_ERR "%s: could not request SEL_HOST_USB GPIO\n", __FUNCTION__);
	}
	gpio_direction_output(GPIO_RI_13, 0);

	/*
	 * USB/DAC nRESET line
	 */
	ret = gpio_request(GPIO_RI_3, "USB_DAC_nRESET");
	if (ret) {
		printk(KERN_ERR "%s: could not request USB_DAC_nRESET GPIO\n", __FUNCTION__);
	}
	gpio_direction_output(GPIO_RI_3, 0);
	udelay(1);
	gpio_direction_output(GPIO_RI_3, 1);

	/*
	 * I2C BUS2 Disable line
	 */
	ret = gpio_request(GPIO_RI_0, "DISABLE_BUS2");
	if (ret) {
		printk(KERN_ERR "%s: could not request DISABLE_BUS2 GPIO\n", __FUNCTION__);
	}
	gpio_direction_output(GPIO_RI_0, 0);

	vdc_tio_init();

	printk(KERN_INFO "%s: registering device resources\n", __FUNCTION__);
	platform_add_devices(ip7160dpf_devices, ARRAY_SIZE(ip7160dpf_devices));

	/*
	 * Allocate the audio driver if we can
	 */
	audio_dev = audio_device_alloc("snd-ubi32-cs4350", "audio-i2sout", 0);
	if (audio_dev) {
		ip7160dpf_i2c_board_info[0].platform_data = audio_dev;
	}

	printk(KERN_INFO "%s: registering i2c resources\n", __FUNCTION__);
	i2c_register_board_info(0, ip7160dpf_i2c_board_info, ARRAY_SIZE(ip7160dpf_i2c_board_info));

	return 0;
}

arch_initcall(ip7160dpf_init);

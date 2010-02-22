/*
 * arch/ubicom32/mach-ip5k/board-ip5170dpf.c
 *   Platform initialization for ip5160dpf board.
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
#include <linux/gpio.h>
#include <linux/leds.h>

#include <linux/i2c.h>
#include <linux/i2c-gpio.h>

#include <linux/input.h>
#include <asm/board.h>
#include <asm/machdep.h>
#include <asm/ubicom32hid.h>
#include <asm/vdc_tio.h>

/*
 * LEDs
 *
 * WLAN			PD9	(Note this is shared with MISO, but we don't use it)
 * WPS			PD8
 *
 * TODO: check triggers, are they generic?
 */
static struct gpio_led ip5170dpf_gpio_leds[] = {
	{
		.name			= "d31:green:WLAN1",
		.default_trigger	= "WLAN1",
		.gpio			= GPIO_RD_9,
		.active_low		= 1,
	},
	{
		.name			= "d30:green:WPS",
		.default_trigger	= "WPS",
		.gpio			= GPIO_RD_8,
		.active_low		= 1,
	},
};

static struct gpio_led_platform_data ip5170dpf_gpio_led_platform_data = {
	.num_leds	= 2,
	.leds		= ip5170dpf_gpio_leds,
};

static struct platform_device ip5170dpf_gpio_leds_device = {
	.name		= "leds-gpio",
	.id		= -1,
	.dev = {
		.platform_data = &ip5170dpf_gpio_led_platform_data,
	},
};

/*
 * Backlight on the board PD0, hardware PWM
 */
static const struct ubicom32hid_button ip5170dpf_ubicom32hid_buttons[] = {
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

static const struct ubicom32hid_ir ip5170dpf_ubicom32hid_ircodes[] = {
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

static struct ubicom32hid_platform_data ip5170dpf_ubicom32hid_platform_data = {
	.gpio_reset		= GPIO_RA_4,
	.gpio_reset_polarity	= 0,
	.type			= UBICOM32HID_BL_TYPE_BINARY,
	.invert			= 0,
	.default_intensity	= 1,
	.buttons		= ip5170dpf_ubicom32hid_buttons,
	.nbuttons		= ARRAY_SIZE(ip5170dpf_ubicom32hid_buttons),
	.ircodes		= ip5170dpf_ubicom32hid_ircodes,
	.nircodes		= ARRAY_SIZE(ip5170dpf_ubicom32hid_ircodes),
};

/*
 * Devices on the I2C bus
 */
static struct i2c_board_info __initdata ip5170dpf_i2c_board_info[] = {
	/*
	 * U24, ubicom32hid
	 */
	{
		.type		= "ubicom32hid",
		.addr		= 0x08,
		.platform_data	= &ip5170dpf_ubicom32hid_platform_data,
	},

	/*
	 * U14, CS4350 DAC, address 0x4B
	 */
};

/*
 * I2C bus on the board, SDA PF13, SCL PF14
 */
static struct i2c_gpio_platform_data ip5170dpf_i2c_data = {
	.sda_pin		= GPIO_RF_13,
	.scl_pin		= GPIO_RF_14,
	.sda_is_open_drain	= 0,
	.scl_is_open_drain	= 0,
	.scl_is_output_only	= 1,
	.udelay			= 5,
};

static struct platform_device ip5170dpf_i2c_device = {
	.name	= "i2c-gpio",
	.id	= 0,
	.dev	= {
		.platform_data = &ip5170dpf_i2c_data,
	},
};

/*
 * List of all devices in our system
 */
static struct platform_device *ip5170dpf_devices[] __initdata = {
	&ip5170dpf_i2c_device,
	&ip5170dpf_gpio_leds_device,
};

/*
 * ip5170dpf_init
 *	Called to add the devices which we have on this board
 */
static int __init ip5170dpf_init(void)
{
	ubi_gpio_init();

	vdc_tio_init();

	printk(KERN_INFO "%s: registering device resources\n", __FUNCTION__);
	platform_add_devices(ip5170dpf_devices, ARRAY_SIZE(ip5170dpf_devices));

	printk(KERN_INFO "%s: registering i2c resources\n", __FUNCTION__);
	i2c_register_board_info(0, ip5170dpf_i2c_board_info, ARRAY_SIZE(ip5170dpf_i2c_board_info));

	return 0;
}

arch_initcall(ip5170dpf_init);

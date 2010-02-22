/*
 * arch/ubicom32/mach-ip7k/board-ip7160rgw.c
 *   Platform initialization for ip7160rgw board.
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
#include <linux/delay.h>
#include <linux/input.h>
#include <linux/spi/spi.h>

#include <asm/board.h>
#include <asm/machdep.h>
#include <asm/ubicom32input.h>

#ifdef CONFIG_SERIAL_UBI32_SERDES
#include <asm/ubicom32suart.h>
#endif

#include <asm/ubicom32-spi-gpio.h>
#include <asm/switch-dev.h>

#ifdef CONFIG_IP7160RGWLCD
#include <linux/i2c.h>
#include <linux/i2c-gpio.h>
/*
 * LCD Adapter board 8007-092x support
 *
 * Touch controller
 *
 * Connected via I2C bus, interrupt on PA6
 */
#include <linux/i2c/tsc2007.h>

/*
 * ip7160rgwlcd_tsc2007_exit_platform_hw
 */
static void ip7160rgwlcd_tsc2007_exit_platform_hw(void)
{
	UBICOM32_IO_PORT(RA)->ctl0 &= ~(0x03 << 17);
	gpio_free(GPIO_RA_5);
}

/*
 * ip7160rgwlcd_tsc2007_init_platform_hw
 */
static int ip7160rgwlcd_tsc2007_init_platform_hw(void)
{
	int res = gpio_request(GPIO_RA_5, "TSC2007_IRQ");
	if (res) {
		return res;
	}

	UBICOM32_IO_PORT(RA)->ctl0 &= ~(0x03 << 17);
	UBICOM32_IO_PORT(RA)->ctl0 |= (0x02 << 17);
	return 0;
}

/*
 * ip7160rgwlcd_tsc2007_get_pendown_state
 */
static int ip7160rgwlcd_tsc2007_get_pendown_state(void)
{
	return !gpio_get_value(GPIO_RA_5);
}

static struct tsc2007_platform_data ip7160rgwlcd_tsc2007_data = {
	.model			= 2007,
	.x_plate_ohms		= 350,
	.get_pendown_state	= ip7160rgwlcd_tsc2007_get_pendown_state,
	.init_platform_hw	= ip7160rgwlcd_tsc2007_init_platform_hw,
	.exit_platform_hw	= ip7160rgwlcd_tsc2007_exit_platform_hw,
};

/******************************************************************************
 * I2C bus on the board, SDA PI14, SCL PI13
 */
static struct i2c_gpio_platform_data ip7160rgwlcd_i2c_data = {
	.sda_pin		= GPIO_RI_14,
	.scl_pin		= GPIO_RI_13,
	.sda_is_open_drain	= 0,
	.scl_is_open_drain	= 0,
	.udelay			= 50,
};

static struct platform_device ip7160rgwlcd_i2c_device = {
	.name	= "i2c-gpio",
	.id	= 0,
	.dev	= {
		.platform_data = &ip7160rgwlcd_i2c_data,
	},
};

static struct i2c_board_info __initdata ip7160rgwlcd_i2c_board_info[] = {
	{
		.type = "tsc2007",
		.addr = 0x48,
		.irq = 45, // RA5
		.platform_data = &ip7160rgwlcd_tsc2007_data,
	},
};

#endif

/*
 * SPI bus over GPIO for Gigabit Ethernet Switch
 *	U58:
 *		MOSI	PE0
 *		MISO	PE1
 *		CLK	PE3
 *		CS	PE2
 */
static struct ubicom32_spi_gpio_platform_data ip7160rgw_spi_gpio_data = {
	.pin_mosi	= GPIO_RE_0,
	.pin_miso	= GPIO_RE_1,
	.pin_clk	= GPIO_RE_3,
	.bus_num	= 0,		// We'll call this SPI bus 0
	.num_chipselect	= 1,		// only one device on this SPI bus
	.clk_default	= 1,
};

static struct platform_device ip7160rgw_spi_gpio_device = {
	.name	= "ubicom32-spi-gpio",
	.id	= 0,
	.dev	= {
		.platform_data = &ip7160rgw_spi_gpio_data,
	},
};

static struct ubicom32_spi_gpio_controller_data ip7160rgw_bcm539x_controller_data = {
	.pin_cs = GPIO_RE_2,
};

static struct switch_core_platform_data ip7160rgw_bcm539x_platform_data = {
	.flags		= SWITCH_DEV_FLAG_HW_RESET,
	.pin_reset	= GPIO_RE_4,
	.name		= "bcm539x",
};

static struct spi_board_info ip7160rgw_spi_board_info[] = {
	{
		.modalias		= "bcm539x-spi",
		.bus_num		= 0,
		.chip_select		= 0,
		.max_speed_hz		= 2000000,
		.platform_data		= &ip7160rgw_bcm539x_platform_data,
		.controller_data	= &ip7160rgw_bcm539x_controller_data,
		.mode			= SPI_MODE_3,
	}
};

/*
 * LEDs
 *
 * WLAN1		PD0	(PWM capable)
 * WLAN2		PD1
 * USB2.0		PD2
 * Status		PD3
 * WPS			PD4
 *
 * TODO: check triggers, are they generic?
 */
static struct gpio_led ip7160rgw_gpio_leds[] = {
	{
		.name			= "d53:green:WLAN1",
		.default_trigger	= "WLAN1",
		.gpio			= GPIO_RD_0,
		.active_low		= 1,
	},
	{
		.name			= "d54:green:WLAN2",
		.default_trigger	= "WLAN2",
		.gpio			= GPIO_RD_1,
		.active_low		= 1,
	},
	{
		.name			= "d55:green:USB",
		.default_trigger	= "USB",
		.gpio			= GPIO_RD_2,
		.active_low		= 1,
	},
	{
		.name			= "d56:green:Status",
		.default_trigger	= "Status",
		.gpio			= GPIO_RD_3,
		.active_low		= 1,
	},
	{
		.name			= "d57:green:WPS",
		.default_trigger	= "WPS",
		.gpio			= GPIO_RD_4,
		.active_low		= 1,
	},
};

static struct gpio_led_platform_data ip7160rgw_gpio_led_platform_data = {
	.num_leds	= 5,
	.leds		= ip7160rgw_gpio_leds,
};

static struct platform_device ip7160rgw_gpio_leds_device = {
	.name		= "leds-gpio",
	.id		= -1,
	.dev = {
		.platform_data = &ip7160rgw_gpio_led_platform_data,
	},
};

/*
 * Use ubicom32input driver to monitor the various pushbuttons on this board.
 *
 * WPS			PD5
 * FACT_DEFAULT		PD6
 *
 * TODO: pick some ubicom understood EV_xxx define for WPS and Fact Default
 */
static struct ubicom32input_button ip7160rgw_ubicom32input_buttons[] = {
	{
		.type		= EV_KEY,
		.code		= KEY_FN_F1,
		.gpio		= GPIO_RD_5,
		.desc		= "WPS",
		.active_low	= 1,
	},
	{
		.type		= EV_KEY,
		.code		= KEY_FN_F2,
		.gpio		= GPIO_RD_6,
		.desc		= "Factory Default",
		.active_low	= 1,
	},
};

static struct ubicom32input_platform_data ip7160rgw_ubicom32input_data = {
	.buttons	= ip7160rgw_ubicom32input_buttons,
	.nbuttons	= ARRAY_SIZE(ip7160rgw_ubicom32input_buttons),
};

static struct platform_device ip7160rgw_ubicom32input_device = {
	.name	= "ubicom32input",
	.id	= -1,
	.dev	= {
		.platform_data = &ip7160rgw_ubicom32input_data,
	},
};

#ifdef CONFIG_SERIAL_UBI32_SERDES
static struct resource ip7160rgw_ubicom32_suart_resources[] = {
	{
		.start	= RE,
		.end	= RE,
		.flags	= IORESOURCE_MEM,
	},
	{
		.start	= PORT_OTHER_INT(RE),
		.end	= PORT_OTHER_INT(RE),
		.flags	= IORESOURCE_IRQ,
	},
	{
		.start	= 250000000,
		.end	= 250000000,
		.flags	= UBICOM32_SUART_IORESOURCE_CLOCK,
	},
};

static struct platform_device ip7160rgw_ubicom32_suart_device = {
	.name		= "ubicom32suart",
	.id		= -1,
	.num_resources	= ARRAY_SIZE(ip7160rgw_ubicom32_suart_resources),
	.resource	= ip7160rgw_ubicom32_suart_resources,
};
#endif

/*
 * List of all devices in our system
 */
static struct platform_device *ip7160rgw_devices[] __initdata = {
#ifdef CONFIG_SERIAL_UBI32_SERDES
	&ip7160rgw_ubicom32_suart_device,
#endif
	&ip7160rgw_ubicom32input_device,
	&ip7160rgw_gpio_leds_device,
	&ip7160rgw_spi_gpio_device,
#ifdef CONFIG_IP7160RGWLCD
	&ip7160rgwlcd_i2c_device,
#endif
};

/*
 * ip7160rgw_init
 *	Called to add the devices which we have on this board
 */
static int __init ip7160rgw_init(void)
{
	board_init();

	/*
	 * Rev 1.2 boards have spi in a different place than 1.1/1.0
	 */
	if (strcmp(board_get_revision(), "1.2") == 0) {
		ip7160rgw_spi_gpio_data.pin_mosi = GPIO_RD_7;
	}

	ubi_gpio_init();

	/*
	 * Reserve switch SPI CS on behalf on switch driver
	 */
	if (gpio_request(ip7160rgw_bcm539x_controller_data.pin_cs, "switch-bcm539x-cs")) {
		printk(KERN_WARNING "Could not request cs of switch SPI I/F\n");
		return -EIO;
	}
	gpio_direction_output(ip7160rgw_bcm539x_controller_data.pin_cs, 1);

	printk(KERN_INFO "%s: registering device resources\n", __FUNCTION__);
	platform_add_devices(ip7160rgw_devices, ARRAY_SIZE(ip7160rgw_devices));

	printk(KERN_INFO "%s: registering SPI resources\n", __FUNCTION__);
	spi_register_board_info(ip7160rgw_spi_board_info, ARRAY_SIZE(ip7160rgw_spi_board_info));

#ifdef CONFIG_IP7160RGWLCD
	printk(KERN_INFO "%s: registering i2c resources\n", __FUNCTION__);
	i2c_register_board_info(0, ip7160rgwlcd_i2c_board_info, ARRAY_SIZE(ip7160rgwlcd_i2c_board_info));
	printk(KERN_INFO "IP7160 RGW + LCD\n");
#else
	printk(KERN_INFO "IP7160 RGW\n");
#endif
	return 0;
}

arch_initcall(ip7160rgw_init);

/*
 * arch/ubicom32/mach-ip7k/board-ip7500iap.c
 *   Support for IP7500 Internet Audio Player
 *
 * This file supports the IP7500 Internet Audio Player:
 *	8007-1110  Rev 1.0
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
#include <linux/device.h>
#include <linux/gpio.h>
#include <asm/board.h>

#include <linux/delay.h>

#include <linux/platform_device.h>
#include <asm/audio.h>
#include <linux/i2c.h>
#include <linux/i2c-gpio.h>

#include <asm/ubicom32sd.h>
#include <asm/sd_tio.h>

#include <asm/ubicom32bl.h>

#include <asm/machdep.h>

/******************************************************************************
 * SD/IO Port F (Slot 1) platform data
 */
static struct resource ip7500iap_portf_sd_resources[] = {
	/*
	 * Send IRQ
	 */
	[0] = {
		/*
		 * The init routine will query the devtree and fill this in
		 */
		.flags	= IORESOURCE_IRQ,
	},

	/*
	 * Receive IRQ
	 */
	[1] = {
		/*
		 * The init routine will query the devtree and fill this in
		 */
		.flags	= IORESOURCE_IRQ,
	},

	/*
	 * Memory Mapped Registers
	 */
	[2] = {
		/*
		 * The init routine will query the devtree and fill this in
		 */
		.flags	= IORESOURCE_MEM,
	},
};

static struct ubicom32sd_card ip7500iap_portf_sd_cards[] = {
	[0] = {
		.pin_wp		= GPIO_RF_7,
		.wp_polarity	= 1,
		.pin_pwr	= GPIO_RF_8,
		.pin_cd		= GPIO_RF_6,
	},
};

static struct ubicom32sd_platform_data ip7500iap_portf_sd_platform_data = {
	.ncards		= 1,
	.cards		= ip7500iap_portf_sd_cards,
};

static struct platform_device ip7500iap_portf_sd_device = {
	.name		= "ubicom32sd",
	.id		= 0,
	.resource	= ip7500iap_portf_sd_resources,
	.num_resources	= ARRAY_SIZE(ip7500iap_portf_sd_resources),
	.dev		= {
			.platform_data = &ip7500iap_portf_sd_platform_data,
	},

};

/*
 * ip7500iap_portf_sd_init
 */
static void ip7500iap_portf_sd_init(void)
{
	/*
	 * Check the device tree for the sd_tio
	 */
	struct sd_tio_node *sd_node = (struct sd_tio_node *)devtree_find_node("portf_sd");
	if (!sd_node) {
		printk(KERN_INFO "PortF SDTIO not found\n");
		return;
	}

	/*
	 * Fill in the resources and platform data from devtree information
	 */
	ip7500iap_portf_sd_resources[0].start = sd_node->dn.sendirq;
	ip7500iap_portf_sd_resources[1].start = sd_node->dn.recvirq;
	ip7500iap_portf_sd_resources[2].start = (u32_t)&(sd_node->regs);
	ip7500iap_portf_sd_resources[2].end = (u32_t)&(sd_node->regs) + sizeof(sd_node->regs);

	platform_device_register(&ip7500iap_portf_sd_device);
}

/******************************************************************************
 * SD/IO Port B (Slot 2) platform data
 */
static struct resource ip7500iap_portb_sd_resources[] = {
	/*
	 * Send IRQ
	 */
	[0] = {
		/*
		 * The init routine will query the devtree and fill this in
		 */
		.flags	= IORESOURCE_IRQ,
	},

	/*
	 * Receive IRQ
	 */
	[1] = {
		/*
		 * The init routine will query the devtree and fill this in
		 */
		.flags	= IORESOURCE_IRQ,
	},

	/*
	 * Memory Mapped Registers
	 */
	[2] = {
		/*
		 * The init routine will query the devtree and fill this in
		 */
		.flags	= IORESOURCE_MEM,
	},
};

static struct ubicom32sd_card ip7500iap_portb_sd_cards[] = {
	[0] = {
		.pin_wp		= GPIO_RB_13,
		.wp_polarity	= 1,
		.pin_pwr	= GPIO_RB_11,
		.pin_cd		= GPIO_RB_12,
	},
};

static struct ubicom32sd_platform_data ip7500iap_portb_sd_platform_data = {
	.ncards		= 1,
	.cards		= ip7500iap_portb_sd_cards,
};

static struct platform_device ip7500iap_portb_sd_device = {
	.name		= "ubicom32sd",
	.id		= 1,
	.resource	= ip7500iap_portb_sd_resources,
	.num_resources	= ARRAY_SIZE(ip7500iap_portb_sd_resources),
	.dev		= {
			.platform_data = &ip7500iap_portb_sd_platform_data,
	},

};

/*
 * ip7500iap_portb_sd_init
 */
static void ip7500iap_portb_sd_init(void)
{
	/*
	 * Check the device tree for the sd_tio
	 */
	struct sd_tio_node *sd_node = (struct sd_tio_node *)devtree_find_node("portb_sd");
	if (!sd_node) {
		printk(KERN_INFO "PortB SDTIO not found\n");
		return;
	}

	/*
	 * Fill in the resources and platform data from devtree information
	 */
	ip7500iap_portb_sd_resources[0].start = sd_node->dn.sendirq;
	ip7500iap_portb_sd_resources[1].start = sd_node->dn.recvirq;
	ip7500iap_portb_sd_resources[2].start = (u32_t)&(sd_node->regs);
	ip7500iap_portb_sd_resources[2].end = (u32_t)&(sd_node->regs) + sizeof(sd_node->regs);

	platform_device_register(&ip7500iap_portb_sd_device);
}

/******************************************************************************
 * Touch controller
 *
 * Connected via I2C bus, interrupt on PA6
 */
#include <linux/i2c/tsc2007.h>

/*
 * ip7500iap_tsc2007_exit_platform_hw
 */
static void ip7500iap_tsc2007_exit_platform_hw(void)
{
	UBICOM32_IO_PORT(RA)->ctl0 &= ~(0x03 << 19);
	gpio_free(GPIO_RA_6);
}

/*
 * ip7500iap_tsc2007_init_platform_hw
 */
static int ip7500iap_tsc2007_init_platform_hw(void)
{
	int res = gpio_request(GPIO_RA_6, "TSC2007_IRQ");
	if (res) {
		return res;
	}

	UBICOM32_IO_PORT(RA)->ctl0 &= ~(0x03 << 19);
	UBICOM32_IO_PORT(RA)->ctl0 |= (0x02 << 19);
	return 0;
}

/*
 * ip7500iap_tsc2007_get_pendown_state
 */
static int ip7500iap_tsc2007_get_pendown_state(void)
{
	return !gpio_get_value(GPIO_RA_6);
}

static struct tsc2007_platform_data ip7500iap_tsc2007_data = {
	.model			= 2007,
	.x_plate_ohms		= 350,
	.get_pendown_state	= ip7500iap_tsc2007_get_pendown_state,
	.init_platform_hw	= ip7500iap_tsc2007_init_platform_hw,
	.exit_platform_hw	= ip7500iap_tsc2007_exit_platform_hw,
};

/******************************************************************************
 * i2c devices
 *
 * DO NOT CHANGE THE ORDER HERE unless you know how this works.  There
 * are hardcoded indicies which refer to the order of drivers listed here.
 */
static struct i2c_board_info __initdata ip7500iap_i2c_board_info[] = {
	/*
	 * U6, CS4350 DAC, address 0x4B
	 */
	{
		.type		= "cs4350",
		.addr		= 0x4B,
	},

	/*
	 * U20, S35390A RTC, address 0x30
	 */
	{
		.type           = "s35390a",
		.addr           = 0x30,
	},

	/*
	 * U9, TSC2007 Touch screen controller, address 0x49, irq RA6
	 */
	{
		.type		= "tsc2007",
		.addr		= 0x49,
		.irq		= 46,
		.platform_data	= &ip7500iap_tsc2007_data,
	},
};

/*
 * I2C bus on the board, SDA PE4, SCL PE5
 */
static struct i2c_gpio_platform_data ip7500iap_i2c_data = {
	.sda_pin		= GPIO_RF_14,
	.scl_pin		= GPIO_RF_13,
	.sda_is_open_drain	= 0,
	.scl_is_open_drain	= 0,
	.udelay			= 50,
};

static struct platform_device ip7500iap_i2c_device = {
	.name	= "i2c-gpio",
	.id	= 0,
	.dev	= {
		.platform_data = &ip7500iap_i2c_data,
	},
};

/******************************************************************************
 * Backlight on the board PD0, hardware PWM
 */
static struct ubicom32bl_platform_data ip7500iap_backlight_data = {
	.type			= UBICOM32BL_TYPE_PWM,
	.pwm_channel		= 2,
	.pwm_prescale		= 15,
	.pwm_period		= 60,
	.default_intensity	= 0x80,
};

static struct platform_device ip7500iap_backlight_device = {
	.name	= "ubicom32bl",
	.id	= -1,
	.dev	= {
		.platform_data = &ip7500iap_backlight_data,
	},
};

/******************************************************************************
 * Devices on this board
 */
static struct platform_device *ip7500iap_devices[] __initdata = {
	&ip7500iap_i2c_device,
	&ip7500iap_backlight_device,
};

/*
 * ip7500iap_power_off
 *	Called to turn the power off for this board
 */
static void ip7500iap_power_off(void)
{
	gpio_set_value(GPIO_RF_11, 0);
}

/*
 * ip7500iap_init
 *	Called to add the devices which we have on this board
 */
static int __init ip7500iap_init(void)
{
	struct platform_device *audio_dev;
	struct platform_device *audio_dev2;
	int ret;

	board_init();

	ubi_gpio_init();

	/*
	 * Hold the POWER_HOLD line
	 */
	ret = gpio_request(GPIO_RF_11, "POWER_HOLD");
	if (ret) {
		printk(KERN_ERR "%s: could not request POWER_HOLD GPIO\n", __FUNCTION__);
	}
	gpio_direction_output(GPIO_RF_11, 1);
	mach_power_off = ip7500iap_power_off;

	/*
	 * DAC nRESET line
	 */
	ret = gpio_request(GPIO_RE_7, "DAC_nRESET");
	if (ret) {
		printk(KERN_ERR "%s: could not request DAC_nRESET GPIO\n", __FUNCTION__);
	}
	gpio_direction_output(GPIO_RE_7, 0);
	udelay(1);
	gpio_set_value(GPIO_RE_7, 1);

	/*
	 * Bring up any SDIO slots
	 */
	ip7500iap_portb_sd_init();
	ip7500iap_portf_sd_init();

	/*
	 * Bring up audio devices
	 */
	platform_add_devices(ip7500iap_devices, ARRAY_SIZE(ip7500iap_devices));

	audio_dev = audio_device_alloc("snd-ubi32-cs4350", "audio", "audio-i2sout", 0);
	if (audio_dev) {
		ip7500iap_i2c_board_info[0].platform_data = audio_dev;
	}

	audio_dev2 = audio_device_alloc("snd-ubi32-generic", "audio", "audio-spdifout", 0);
	if (audio_dev2) {
		platform_device_register(audio_dev2);
	}

	printk(KERN_INFO "%s: registering i2c resources\n", __FUNCTION__);
	i2c_register_board_info(0, ip7500iap_i2c_board_info, ARRAY_SIZE(ip7500iap_i2c_board_info));

	printk(KERN_INFO "IP7500 Internet Audio Player\n");

	return 0;
}

arch_initcall(ip7500iap_init);

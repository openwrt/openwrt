/*
 * arch/ubicom32/mach-ip7k/board-ip7500media.c
 *   Board file for IP7500 media board.
 *
 * Supports the following configuration
 *	CPU Module:
 *		P/N 8007-0510 rev 1.0 NOPHY
 *		P/N 8007-0511 rev 1.1 NOPHY
 *			DIP Switch SW2 configuration:
 *				POS 1: on  = PCI enabled
 *				POS 2: off = TTYX => PF12
 *				POS 3: off = TTYY => PF15
 *				POS 4: unused
 *	Media Board:
 *		P/N 8007-0610 rev 1.0
 *
 *	LCD Adapter Board: (optional)
 *		P/N 8007-0920 rev 2.0
 *		P/N 8007-0921 rev 2.1
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

#include <linux/input.h>

#include <linux/i2c.h>
#include <linux/i2c-gpio.h>
#include <linux/i2c/pca953x.h>

#include <asm/board.h>
#include <asm/machdep.h>
#include <asm/ubicom32input_i2c.h>
#include <asm/ubicom32bl.h>
#include <asm/ubicom32lcdpower.h>
#include <asm/vdc_tio.h>

#include <asm/ubicom32sd.h>
#include <asm/sd_tio.h>
#include <asm/devtree.h>
#include <asm/audio.h>

#include <asm/ring_tio.h>

/******************************************************************************
 * SD/IO Port F (Slot 1) platform data
 */
static struct resource ip7500media_portf_sd_resources[] = {
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

static struct ubicom32sd_card ip7500media_portf_sd_cards[] = {
	[0] = {
		.pin_wp		= IP7500MEDIA_IO16,
		.wp_polarity	= 1,
		.pin_pwr	= IP7500MEDIA_IO20,
		.pin_cd		= IP7500MEDIA_IO23,
	},
	[1] = {
		.pin_wp		= IP7500MEDIA_IO17,
		.wp_polarity	= 1,
		.pin_pwr	= IP7500MEDIA_IO21,
		.pin_cd		= IP7500MEDIA_IO24,
	},
};

static struct ubicom32sd_platform_data ip7500media_portf_sd_platform_data = {
	.ncards		= 2,
	.cards		= ip7500media_portf_sd_cards,
};

static struct platform_device ip7500media_portf_sd_device = {
	.name		= "ubicom32sd",
	.id		= 0,
	.resource	= ip7500media_portf_sd_resources,
	.num_resources	= ARRAY_SIZE(ip7500media_portf_sd_resources),
	.dev		= {
			.platform_data = &ip7500media_portf_sd_platform_data,
	},

};

/*
 * ip7500media_portf_sd_init
 */
static void ip7500media_portf_sd_init(void)
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
	ip7500media_portf_sd_resources[0].start = sd_node->dn.sendirq;
	ip7500media_portf_sd_resources[1].start = sd_node->dn.recvirq;
	ip7500media_portf_sd_resources[2].start = (u32_t)&(sd_node->regs);
	ip7500media_portf_sd_resources[2].end = (u32_t)&(sd_node->regs) + sizeof(sd_node->regs);

	platform_device_register(&ip7500media_portf_sd_device);
}

/******************************************************************************
 * SD/IO Port B (Slot 2) platform data
 */
static struct resource ip7500media_portb_sd_resources[] = {
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

static struct ubicom32sd_card ip7500media_portb_sd_cards[] = {
	[0] = {
		.pin_wp		= IP7500MEDIA_IO19,
		.wp_polarity	= 1,
		.pin_pwr	= IP7500MEDIA_IO22,
		.pin_cd		= IP7500MEDIA_IO18,
	},
};

static struct ubicom32sd_platform_data ip7500media_portb_sd_platform_data = {
	.ncards		= 1,
	.cards		= ip7500media_portb_sd_cards,
};

static struct platform_device ip7500media_portb_sd_device = {
	.name		= "ubicom32sd",
	.id		= 1,
	.resource	= ip7500media_portb_sd_resources,
	.num_resources	= ARRAY_SIZE(ip7500media_portb_sd_resources),
	.dev		= {
			.platform_data = &ip7500media_portb_sd_platform_data,
	},

};

/*
 * ip7500media_portb_sd_init
 */
static void ip7500media_portb_sd_init(void)
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
	ip7500media_portb_sd_resources[0].start = sd_node->dn.sendirq;
	ip7500media_portb_sd_resources[1].start = sd_node->dn.recvirq;
	ip7500media_portb_sd_resources[2].start = (u32_t)&(sd_node->regs);
	ip7500media_portb_sd_resources[2].end = (u32_t)&(sd_node->regs) + sizeof(sd_node->regs);

	platform_device_register(&ip7500media_portb_sd_device);
}

/*
 * ip7500media_u17_setup
 *	Called by I2C to tell us that u17 is setup.
 *
 * This function is called by I2C to tell us that u17 has been setup.  All
 * devices which rely on this chip being initialized (or even present) need to
 * be initialized in this function otherwise they may get initialized too early.
 *
 * Currently the only device depending on u17 is the SDIO
 */
static int __init ip7500media_u17_setup(struct i2c_client *client, unsigned gpio, unsigned ngpio, void *context)
{
	/*
	 * Initialize the Port F/Port B SD slots (only the enabled ports will init)
	 */
	ip7500media_portf_sd_init();
	ip7500media_portb_sd_init();

	return 0;
}

/******************************************************************************
 * LCD VGH on the board at PE6
 */
static struct ubicom32lcdpower_platform_data ip7500media_lcdpower_data = {
	.vgh_gpio		= GPIO_RE_7,
	.vgh_polarity		= true,
};

static struct platform_device ip7500media_lcdpower_device = {
	.name	= "ubicom32lcdpower",
	.id	= -1,
	.dev	= {
		.platform_data = &ip7500media_lcdpower_data,
	},
};

/******************************************************************************
 * Backlight on the board PD0, hardware PWM
 */
static struct ubicom32bl_platform_data ip7500media_backlight_data = {
	.type			= UBICOM32BL_TYPE_PWM,
	.pwm_channel		= 2,
	.pwm_prescale		= 15,
	.pwm_period		= 60,
	.default_intensity	= 0x80,
};

static struct platform_device ip7500media_backlight_device = {
	.name	= "ubicom32bl",
	.id	= -1,
	.dev	= {
		.platform_data = &ip7500media_backlight_data,
	},
};

/******************************************************************************
 * Ubicom32Input on I2C, U15 MAX7310, address 0x18, 8 bits
 */
static struct ubicom32input_i2c_button ip7500media_ubicom32input_i2c_u15_buttons[] = {
	{
		.type		= EV_KEY,
		.code		= KEY_LEFT,
		.bit		= 0,
		.active_low	= 1,
	},
	{
		.type		= EV_KEY,
		.code		= KEY_RIGHT,
		.bit		= 1,
		.active_low	= 1,
	},
	{
		.type		= EV_KEY,
		.code		= KEY_UP,
		.bit		= 2,
		.active_low	= 1,
	},
	{
		.type		= EV_KEY,
		.code		= KEY_DOWN,
		.bit		= 3,
		.active_low	= 1,
	},
	{
		.type		= EV_KEY,
		.code		= KEY_ENTER,
		.bit		= 4,
		.active_low	= 1,
	},
	{
		.type		= EV_KEY,
		.code		= KEY_MENU,
		.bit		= 5,
		.active_low	= 1,
	},
	{
		.type		= EV_KEY,
		.code		= KEY_ESC,
		.bit		= 6,
		.active_low	= 1,
	},
};

static struct ubicom32input_i2c_platform_data ip7500media_ubicom32input_i2c_u15_platform_data = {
	.buttons	= ip7500media_ubicom32input_i2c_u15_buttons,
	.nbuttons	= ARRAY_SIZE(ip7500media_ubicom32input_i2c_u15_buttons),
	.name		= "Ubicom32 Input I2C U15",
};

/******************************************************************************
 * Additional GPIO chips
 */
static struct pca953x_platform_data ip7500media_gpio_u16_platform_data = {
	.gpio_base = IP7500MEDIA_U16_BASE,
};

static struct pca953x_platform_data ip7500media_gpio_u17_platform_data = {
	.gpio_base = IP7500MEDIA_U17_BASE,
	.setup = ip7500media_u17_setup,
};

static struct pca953x_platform_data ip7500media_gpio_u18_platform_data = {
	.gpio_base = IP7500MEDIA_U18_BASE,
};


/******************************************************************************
 * Touch controller present on LCD Adapter board
 *
 * Connected via I2C bus, interrupt on PD1
 */
#include <linux/i2c/tsc2007.h>

/*
 * ip7500media_tsc2007_exit_platform_hw
 */
static void ip7500media_tsc2007_exit_platform_hw(void)
{
	UBICOM32_IO_PORT(RD)->int_mask &= ~(1 << 11);
	UBICOM32_IO_PORT(RD)->ctl2 &= ~(0x03 << 16);
	gpio_free(GPIO_RD_1);
}

/*
 * ip7500media_tsc2007_init_platform_hw
 */
static int ip7500media_tsc2007_init_platform_hw(void)
{
	int res = gpio_request(GPIO_RD_1, "TSC2007_IRQ");
	if (res) {
		return res;
	}
	UBICOM32_IO_PORT(RD)->function = 0;
	UBICOM32_IO_PORT(RD)->int_mask = (1 << 11);
	UBICOM32_IO_PORT(RD)->ctl2 &= ~(0x03 << 16);
	UBICOM32_IO_PORT(RD)->ctl2 |= (0x02 << 16);

	return 0;
}

/*
 * ip7500media_tsc2007_clear_penirq
 */
static void ip7500media_tsc2007_clear_penirq(void)
{
	UBICOM32_IO_PORT(RD)->int_clr = (1 << 11);
}

/*
 * ip7500media_tsc2007_get_pendown_state
 */
static int ip7500media_tsc2007_get_pendown_state(void)
{
	return !gpio_get_value(GPIO_RD_1);
}

static struct tsc2007_platform_data ip7500media_tsc2007_data = {
	.model			= 2007,
	.x_plate_ohms		= 350,
	.get_pendown_state	= ip7500media_tsc2007_get_pendown_state,
	.init_platform_hw	= ip7500media_tsc2007_init_platform_hw,
	.exit_platform_hw	= ip7500media_tsc2007_exit_platform_hw,
	.clear_penirq		= ip7500media_tsc2007_clear_penirq,
};

/******************************************************************************
 * Devices on the I2C bus
 *
 * BEWARE of changing the order of things in this array as we depend on
 * certain things to be in certain places.
 */
static struct i2c_board_info __initdata ip7500media_i2c_board_info[] = {
	/*
	 * U6, CS4350 DAC, address 0x4B
	 */
	{
		.type		= "cs4350",
		.addr		= 0x4B,
	},

	/*
	 * U14, S35390A RTC, address 0x30
	 */
	{
		.type		= "s35390a",
		.addr		= 0x30,
	},

	/*
	 * U15, MAX7310 IO expander, 8 bits, address 0x18
	 *	IO0: User I/O (J16-1) (Left)	IO4: User I/O (J16-5) (Enter)
	 *	IO1: User I/O (J16-2) (Right)	IO5: User I/O (J16-6) (Menu)
	 *	IO2: User I/O (J16-3) (Up)	IO6: User I/O (J16-7) (Back)
	 *	IO3: User I/O (J16-4) (Down)	IO7: User I/O (J16-8)
	 */
	{
		.type		= "ubicom32in_max7310",
		.addr		= 0x18,
		.platform_data	= &ip7500media_ubicom32input_i2c_u15_platform_data,
	},

	/*
	 * U16, MAX7310 IO expander, 8 bits, address 0x1C
	 *	IO8 : User I/O (J16-9)		IO12: User I/O (J16-17)
	 *	IO9 : User I/O (J16-10)		IO13: User I/O (J16-18)
	 *	IO10: User I/O (J16-15)		IO14: User I/O (J16-19)
	 *	IO11: User I/O (J16-16)		IO15: User I/O (J16-20)
	 */
	{
		.type		= "max7310",
		.addr		= 0x1C,
		.platform_data	= &ip7500media_gpio_u16_platform_data,
	},

	/*
	 * U17, MAX7310 IO expander, 8 bits, address 0x1A
	 *	IO16: SDIO1A_WP			IO20: SD1A_PWREN
	 *	IO17: SDIO1B_WP			IO21: SD1B_PWREN
	 *	IO18: SDIO2_CD			IO22: SD2_PWREN
	 *	IO19: SDIO2_WP			IO23: SDIO1A_CD
	 *
	 */
	{
		.type		= "max7310",
		.addr		= 0x1A,
		.platform_data	= &ip7500media_gpio_u17_platform_data,
	},

	/*
	 * U18, MAX7310 IOB expander, 8 bits, address 0x1E
	 *	IO24: SDIO1B_CD			IO28: User I/O TP6
	 *	IO25: User I/O TP9		IO29: User I/O TP5
	 *	IO26: User I/O TP8		IO30: User I/O TP4
	 *	IO27: User I/O TP7		IO31: User I/O TP3
	 */
	{
		.type		= "max7310",
		.addr		= 0x1E,
		.platform_data	= &ip7500media_gpio_u18_platform_data,
	},
};

/*
 * Additional I2C devices to add when a LCD adapter board is present
 */
static struct i2c_board_info __initdata ip7500media_lcd_adapter_i2c_board_info[] = {
	{
		I2C_BOARD_INFO("tsc2007", 0x48),
		.irq = PORT_OTHER_INT(RD),
		.platform_data = &ip7500media_tsc2007_data,
	},
};

/*
 * I2C bus on the board, SDA PE4, SCL PE5
 */
static struct i2c_gpio_platform_data ip7500media_i2c_data = {
	.sda_pin		= GPIO_RE_4,
	.scl_pin		= GPIO_RE_5,
	.sda_is_open_drain	= 0,
	.scl_is_open_drain	= 0,
	.udelay			= 50,
};

static struct platform_device ip7500media_i2c_device = {
	.name	= "i2c-gpio",
	.id	= 0,
	.dev	= {
		.platform_data = &ip7500media_i2c_data,
	},
};

/*
 * Virtual Frame Buffer device for use with LCD Adapter
 */
static struct platform_device ip7500media_vfb_device = {
	.name	= "ubicom32vfb",
	.id	= -1,
};

/*
 * vdc_override:
 *	0: no override (auto-detect)
 *	1: force vdc usage
 *	2: force lcd adapter usage
 */
static int __initdata vdc_override = 0;

/*
 * ip7500media_set_forcevdc
 *	Called when forcevdc is present on the kernel boot line
 */
static int __init ip7500media_set_forcevdc(char *str)
{
	if (str[0] == '1') {
		vdc_override = 1;
	} else {
		vdc_override = 2;
	}
	return 1;
}

/*
 * ip7500media_video_init
 *	Called late to determine what kind of video we have on this board
 */
static int __init ip7500media_video_init(void)
{
	struct i2c_adapter *adap;
	struct i2c_msg msg[1];
	unsigned char *data;
	unsigned char checksum;
	int err;
	int i;

	if (vdc_override == 1) {
		printk(KERN_INFO "Force VDCTIO mode\n");
		goto no_adapter;
	}
	if (vdc_override == 2) {
		printk(KERN_INFO "Force LCD Adapter Board mode\n");
		return 0;
	}

	/*
	 * Check to see if there is an EEPROM out there.  If we see an
	 * EEPROM then we will assume a LCD Adapter Board (8007-092x)
	 * exists.
	 */
	data = kmalloc(256, GFP_KERNEL);
	if (!data) {
		printk(KERN_WARNING "%s: Failed to allocate memory\n", __FUNCTION__);
		return -ENOMEM;
	}

	adap = i2c_get_adapter(0);
	if (!adap) {
		printk(KERN_WARNING "%s: Failed to get i2c adapter\n", __FUNCTION__);
		kfree(data);
		return -ENODEV;
	}
	data[0] = 0;
	msg->addr = 0x50;
	msg->flags = 0;
	msg->len = 1;
	msg->buf = data;
	err = i2c_transfer(adap, msg, 1);
	if (err < 0) {
		goto no_adapter;
	}

	msg->addr = 0x50;
	msg->flags = I2C_M_RD;
	msg->len = 256;
	msg->buf = data;
	err = i2c_transfer(adap, msg, 1);
	if (err < 0) {
		goto no_adapter;
	}

	i2c_put_adapter(adap);

	/*
	 * Verify the checksum
	 */
	checksum = 0xff;
	for (i = 0; i < 255; i++) {
		checksum ^= data[i];
	}
	if (checksum != data[255]) {
		printk(KERN_WARNING "%s: Checksum mismatch\n", __FUNCTION__);
	}

	kfree(data);

	/*
	 * Bring up VFB
	 */
	platform_device_register(&ip7500media_vfb_device);

	/*
	 * Add the i2c devices on the LCD Adapter board.  (We have to use i2c_new_device
	 * since it's late in the boot process.)
	 */
	printk(KERN_INFO "%s: registering LCD Adapter board i2c resources\n", __FUNCTION__);
	for (i = 0; i < ARRAY_SIZE(ip7500media_lcd_adapter_i2c_board_info); i++) {
		i2c_new_device(adap, &ip7500media_lcd_adapter_i2c_board_info[i]);
	}

	i2c_put_adapter(adap);

	return 0;

	/*
	 * No LCD Adapter board, bring up VDC
	 */
no_adapter:
	vdc_tio_init();
	return 0;
}
late_initcall(ip7500media_video_init);
__setup("forcevdc=", ip7500media_set_forcevdc);

/*
 * ip7500media_init
 *	Called to add the devices which we have on this board
 */
static int __init ip7500media_init(void)
{
	struct platform_device *audio_dev;
	int have_ethernet = (devtree_find_node("eth_lan") != 0);

	board_init();

	ubi_gpio_init();

#ifdef CONFIG_UIO_UBICOM32RING
	ring_tio_init("decoder_ring");
#endif

	/*
	 * Register all of the devices we have on this board
	 */
	printk(KERN_INFO "%s: registering device resources\n", __FUNCTION__);
	platform_device_register(&ip7500media_i2c_device);
	platform_device_register(&ip7500media_backlight_device);

	/*
	 * If ethernet doesn't exist then we can init the lcdpower
	 */
	if (!have_ethernet) {
		platform_device_register(&ip7500media_lcdpower_device);
	}

	/*
	 * Allocate the audio drivers.  SPDIF not supported on boards with ethernet.
	 */
	audio_dev = audio_device_alloc("snd-ubi32-cs4350", "audio", "audio-i2sout", 0);
	if (audio_dev) {
		ip7500media_i2c_board_info[0].platform_data = audio_dev;
	}

	if (!have_ethernet) {
		struct platform_device *audio_dev2;

		audio_dev2 = audio_device_alloc("snd-ubi32-generic", "audio", "audio-spdifout", 0);
		if (audio_dev2) {
			platform_device_register(audio_dev2);
		}
	}

	/*
	 * Register all of the devices which sit on the I2C bus
	 */
	printk(KERN_INFO "%s: registering i2c resources\n", __FUNCTION__);
	i2c_register_board_info(0, ip7500media_i2c_board_info, ARRAY_SIZE(ip7500media_i2c_board_info));

	/*
	 * We have to initialize the SDIO after the I2C IOB gets setup.  SDIO is initialized in
	 * ip7500media_u17_setup
	 */

	printk("IP7500 Media Board\n");

	return 0;
}

arch_initcall(ip7500media_init);

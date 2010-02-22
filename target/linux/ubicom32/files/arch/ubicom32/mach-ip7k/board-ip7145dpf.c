/*
 * arch/ubicom32/mach-ip7k/board-ip7145dpf.c
 *   Board file for IP7145DPF, rev 1.0, P/N 8007-0410
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
#include <asm/ubicom32input.h>
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
static struct resource ip7145dpf_portf_sd_resources[] = {
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

static struct ubicom32sd_card ip7145dpf_portf_sd_cards[] = {
	[0] = {
		.pin_wp		= IP7145DPF_IOB0,
		.wp_polarity	= 1,
		.pin_pwr	= IP7145DPF_IOB4,
		.pin_cd		= GPIO_RA_4,
	},
	[1] = {
		.pin_wp		= IP7145DPF_IOB1,
		.wp_polarity	= 1,
		.pin_pwr	= IP7145DPF_IOB5,
		.pin_cd		= GPIO_RA_6,
	},
};

static struct ubicom32sd_platform_data ip7145dpf_portf_sd_platform_data = {
	.ncards		= 2,
	.cards		= ip7145dpf_portf_sd_cards,
};

static struct platform_device ip7145dpf_portf_sd_device = {
	.name		= "ubicom32sd",
	.id		= 0,
	.resource	= ip7145dpf_portf_sd_resources,
	.num_resources	= ARRAY_SIZE(ip7145dpf_portf_sd_resources),
	.dev		= {
			.platform_data = &ip7145dpf_portf_sd_platform_data,
	},

};

/*
 * ip7145dpf_portf_sd_init
 */
static void ip7145dpf_portf_sd_init(void)
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
	ip7145dpf_portf_sd_resources[0].start = sd_node->dn.sendirq;
	ip7145dpf_portf_sd_resources[1].start = sd_node->dn.recvirq;
	ip7145dpf_portf_sd_resources[2].start = (u32_t)&(sd_node->regs);
	ip7145dpf_portf_sd_resources[2].end = (u32_t)&(sd_node->regs) + sizeof(sd_node->regs);

	platform_device_register(&ip7145dpf_portf_sd_device);
}

/******************************************************************************
 * SD/IO Port B (Slot 2) platform data
 */
static struct resource ip7145dpf_portb_sd_resources[] = {
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

static struct ubicom32sd_card ip7145dpf_portb_sd_cards[] = {
	[0] = {
		.pin_wp		= IP7145DPF_IOB2,
		.wp_polarity	= 1,
		.pin_pwr	= IP7145DPF_IOB6,
		.pin_cd		= IP7145DPF_IOB3,
	},
};

static struct ubicom32sd_platform_data ip7145dpf_portb_sd_platform_data = {
	.ncards		= 1,
	.cards		= ip7145dpf_portb_sd_cards,
};

static struct platform_device ip7145dpf_portb_sd_device = {
	.name		= "ubicom32sd",
	.id		= 1,
	.resource	= ip7145dpf_portb_sd_resources,
	.num_resources	= ARRAY_SIZE(ip7145dpf_portb_sd_resources),
	.dev		= {
			.platform_data = &ip7145dpf_portb_sd_platform_data,
	},

};

/*
 * ip7145dpf_portb_sd_init
 */
static void ip7145dpf_portb_sd_init(void)
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
	ip7145dpf_portb_sd_resources[0].start = sd_node->dn.sendirq;
	ip7145dpf_portb_sd_resources[1].start = sd_node->dn.recvirq;
	ip7145dpf_portb_sd_resources[2].start = (u32_t)&(sd_node->regs);
	ip7145dpf_portb_sd_resources[2].end = (u32_t)&(sd_node->regs) + sizeof(sd_node->regs);

	platform_device_register(&ip7145dpf_portb_sd_device);
}


#ifdef IP7145DPF_USE_MMC_SPI
/******************************************************************************
 * SPI over GPIO (MMC_SPI)
 */
#include <linux/spi/spi.h>
#include <linux/spi/mmc_spi.h>
#include <linux/mmc/host.h>
#include <asm/ubicom32-spi-gpio.h>

#define MMC_CS	GPIO_RF_5	// PF5 D3
#define MMC_CD	GPIO_RA_4	// PA4 CD
#define MMC_WP	IP7145DPF_IOB0	// IOB0 WP
#define MMC_PWR	IP7145DPF_IOB4	// IOB4 PWR

/*
 * SPI bus over GPIO (for SD card)
 */
static struct ubicom32_spi_gpio_platform_data ip7145dpf_spi_gpio_data = {
	.pin_mosi	= GPIO_RF_0,	// PF0 CMD
	.pin_miso	= GPIO_RF_2,	// PF2 D0
	.pin_clk	= GPIO_RF_1,	// PF1 CLK
	.bus_num	= 0,		// We'll call this SPI bus 0
	.num_chipselect	= 1,		// only one device on this SPI bus
};

static struct platform_device ip7145dpf_spi_gpio_device = {
	.name	= "ubicom32-spi-gpio",
	.id	= 0,
	.dev	= {
		.platform_data = &ip7145dpf_spi_gpio_data,
	},
};

/*
 * ip7145dpf_mmc_spi_setpower_slot_a
 *	Set the power state for slot A
 */
static void ip7145dpf_mmc_spi_setpower_slot_a(struct device *dev, unsigned int vdd)
{
	struct mmc_spi_platform_data *pd = dev->platform_data;

	/*
	 * Power is inverted, we could tell the IOB to do it, but it's cleaner this way.
	 */
	if ((1 << vdd) & pd->ocr_mask) {
		gpio_set_value(MMC_PWR, 0);
		return;
	}
	gpio_set_value(MMC_PWR, 1);
}

/*
 * ip7145dpf_mmc_spi_get_cd_slot_a
 *	Get the CD bit for slot A
 */
static int ip7145dpf_mmc_spi_get_cd_slot_a(struct device *dev)
{
	/*
	 * Note that the sense of the GPIO is inverted
	 */
	return !gpio_get_value(MMC_CD);
}

/*
 * ip7145dpf_mmc_spi_get_ro_slot_a
 *	Get the WP bit for slot A
 */
static int ip7145dpf_mmc_spi_get_ro_slot_a(struct device *dev)
{
	/*
	 * Note that the sense of the GPIO is inverted, we could tell the IOB to do it, but
	 * it's clearer this way.
	 */
	return !gpio_get_value(MMC_WP);
}

/*
 * ip7145dpf_mmc_spi_exit_slot_a
 *	Free the appropriate GPIOs for slot A SD slot.
 */
static void ip7145dpf_mmc_spi_exit_slot_a(struct device *dev, void *appdata)
{
	gpio_free(MMC_CD);
	gpio_free(MMC_CS);
	gpio_free(MMC_WP);
	gpio_free(MMC_PWR);
	platform_device_unregister(&ip7145dpf_spi_gpio_device);
}

/*
 * ip7145dpf_mmc_spi_init_slot_a
 *	Allocate the appropriate GPIOs for slot A SD slot.
 *	WP is on IOB0, CD is PA4, CS is on PF5
 *	TODO: make CD an interrupt
 */
static int ip7145dpf_mmc_spi_init_slot_a(void)
{
	int ret = gpio_request(MMC_CD, "mmc-a-cd");
	if (ret) {
		printk(KERN_ERR "%s: could not request mmc-a-cd pin\n", __FUNCTION__);
		return -ENOSYS;
	}
	gpio_direction_input(MMC_CD);

	ret = gpio_request(MMC_CS, "mmc-a-cs");
	if (ret) {
		printk(KERN_ERR "%s: could not request mmc-a-cs pin\n", __FUNCTION__);
		goto no_cs;
	}
	gpio_direction_output(MMC_CS, 0);

	ret = gpio_request(MMC_WP, "mmc-a-wp");
	if (ret) {
		printk(KERN_ERR "%s: could not request mmc-a-wp pin\n", __FUNCTION__);
		goto no_wp;
	}
	gpio_direction_input(MMC_WP);

	/*
	 * Start off with power off
	 */
	ret = gpio_request(MMC_PWR, "mmc-a-pwr");
	if (ret) {
		printk(KERN_ERR "%s: could not request mmc-a-pwr pin\n", __FUNCTION__);
		goto no_pwr;
	}
	ret = gpio_direction_output(MMC_PWR, 1);

	return 0;

no_pwr:
	gpio_free(MMC_WP);

no_wp:
	gpio_free(MMC_CS);

no_cs:
	gpio_free(MMC_CD);
	return -ENOSYS;
}

/*
 * MMC_SPI driver (currently bitbang)
 */
static struct mmc_spi_platform_data ip7145dpf_mmc_platform_data = {
	.ocr_mask	= MMC_VDD_33_34,
	.exit		= ip7145dpf_mmc_spi_exit_slot_a,
	.get_ro		= ip7145dpf_mmc_spi_get_ro_slot_a,
	.get_cd		= ip7145dpf_mmc_spi_get_cd_slot_a,

	.setpower	= ip7145dpf_mmc_spi_setpower_slot_a,
	.powerup_msecs	= 500,

	.detect_delay	= 100,

	.caps		= MMC_CAP_NEEDS_POLL,
};

static struct ubicom32_spi_gpio_controller_data ip7145dpf_mmc_controller_data = {
	.pin_cs =  MMC_CS,
};

static struct spi_board_info ip7145dpf_spi_board_info[] = {
	{
		.modalias = "mmc_spi",
		.bus_num = 0,
		.chip_select = 0,
		.max_speed_hz = 2000000,
		.platform_data = &ip7145dpf_mmc_platform_data,
		.controller_data = &ip7145dpf_mmc_controller_data,
	}
};
#endif /* IP7145DPF_USE_MMC_SPI */

/*
 * ip7145dpf_u72_setup
 *	Called by I2C to tell us that u72 is setup.
 *
 * This function is called by I2C to tell us that u72 has been setup.  All
 * devices which rely on this chip being initialized (or even present) need to
 * be initialized in this function otherwise they may get initialized too early.
 *
 * Currently the only device depending on u72 is the SPI
 */
static int __init ip7145dpf_u72_setup(struct i2c_client *client, unsigned gpio, unsigned ngpio, void *context)
{
#ifdef IP7145DPF_USE_MMC_SPI
	if (ip7145dpf_mmc_spi_init_slot_a()) {
		printk(KERN_ERR "%s: could not request mmc resources\n", __FUNCTION__);
	} else {
		printk(KERN_INFO "%s: registering SPI resources\n", __FUNCTION__);
		spi_register_board_info(ip7145dpf_spi_board_info, ARRAY_SIZE(ip7145dpf_spi_board_info));
		platform_device_register(&ip7145dpf_spi_gpio_device);
	}
#else
	/*
	 * Initialize the Port F/Port B SD slots
	 */
	ip7145dpf_portf_sd_init();
	ip7145dpf_portb_sd_init();
#endif
	return 0;
}

/******************************************************************************
 * LCD VGH on the board at PE6
 */
static struct ubicom32lcdpower_platform_data ip7145dpf_lcdpower_data = {
	.vgh_gpio		= GPIO_RE_6,
	.vgh_polarity		= true,
};

static struct platform_device ip7145dpf_lcdpower_device = {
	.name	= "ubicom32lcdpower",
	.id	= -1,
	.dev	= {
		.platform_data = &ip7145dpf_lcdpower_data,
	},
};

/******************************************************************************
 * Backlight on the board PD0, hardware PWM
 */
static struct ubicom32bl_platform_data ip7145dpf_backlight_data = {
	.type			= UBICOM32BL_TYPE_PWM,
	.pwm_channel		= 2,
	.pwm_prescale		= 15,
	.pwm_period		= 60,
	.default_intensity	= 0x80,
};

static struct platform_device ip7145dpf_backlight_device = {
	.name	= "ubicom32bl",
	.id	= -1,
	.dev	= {
		.platform_data = &ip7145dpf_backlight_data,
	},
};

/******************************************************************************
 * Ubicom32Input on I2C, U48 MAX7310, address 0x18, 8 bits
 */
static struct ubicom32input_i2c_button ip7145dpf_ubicom32input_i2c_u48_buttons[] = {
	{
		.type		= EV_KEY,
		.code		= KEY_UP,
		.bit		= 0,
		.active_low	= 1,
	},
	{
		.type		= EV_KEY,
		.code		= KEY_LEFT,
		.bit		= 1,
		.active_low	= 1,
	},
	{
		.type		= EV_KEY,
		.code		= KEY_RIGHT,
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

static struct ubicom32input_i2c_platform_data ip7145dpf_ubicom32input_i2c_u48_platform_data = {
	.buttons	= ip7145dpf_ubicom32input_i2c_u48_buttons,
	.nbuttons	= ARRAY_SIZE(ip7145dpf_ubicom32input_i2c_u48_buttons),
	.name		= "Ubicom32 Input I2C U48",
};

/******************************************************************************
 * Additional GPIO chips
 */
static struct pca953x_platform_data ip7145dpf_gpio_u72_platform_data = {
	.gpio_base = IP7145DPF_U72_BASE,
	.setup = ip7145dpf_u72_setup,
};

/******************************************************************************
 * Devices on the I2C bus
 */
static struct i2c_board_info __initdata ip7145dpf_i2c_board_info[] = {
	/*
	 * U51, S35390A RTC, address 0x30
	 */
	{
		.type		= "s35390a",
		.addr		= 0x30,
	},

	/*
	 * U48, MAX7310 IO expander, 8 bits, address 0x18
	 */
	{
		.type		= "ubicom32in_max7310",
		.addr		= 0x18,
		.platform_data	= &ip7145dpf_ubicom32input_i2c_u48_platform_data,
	},

	/*
	 * U72, MAX7310 IOB expander, 8 bits, address 0x19
	 */
	{
		.type		= "max7310",
		.addr		= 0x19,
		.platform_data	= &ip7145dpf_gpio_u72_platform_data,
	},
};

/*
 * I2C bus on the board, SDA PE1, SCL PE2
 */
static struct i2c_gpio_platform_data ip7145dpf_i2c_data = {
	.sda_pin		= GPIO_RE_1,
	.scl_pin		= GPIO_RE_2,
	.sda_is_open_drain	= 0,
	.scl_is_open_drain	= 0,
};

static struct platform_device ip7145dpf_i2c_device = {
	.name	= "i2c-gpio",
	.id	= 0,
	.dev	= {
		.platform_data = &ip7145dpf_i2c_data,
	},
};

/******************************************************************************
 * Use ubicom32input driver to monitor the various pushbuttons on this board.
 *
 * WPS			PF12
 * FACT_DEFAULT		PF13
 * POWER		PE4
 *
 * Not sutable for the keypad buttons since those run on I2C GPIO.  The polling
 * of ubicom32input would seem to be excessive for this.
 *
 * TODO: pick some ubicom understood EV_xxx define for WPS and Fact Default
 */
static struct ubicom32input_button ip7145dpf_ubicom32input_buttons[] = {
	{
		.type		= EV_KEY,
		.code		= KEY_FN_F1,
		.gpio		= GPIO_RF_12,
		.desc		= "WPS",
		.active_low	= 1,
	},
	{
		.type		= EV_KEY,
		.code		= KEY_FN_F2,
		.gpio		= GPIO_RF_13,
		.desc		= "Factory Default",
		.active_low	= 1,
	},
	{
		.type		= EV_KEY,
		.code		= KEY_POWER,
		.gpio		= GPIO_RE_4,
		.desc		= "Power",
		.active_low	= 1,
	},
};

static struct ubicom32input_platform_data ip7145dpf_ubicom32input_data = {
	.buttons	= ip7145dpf_ubicom32input_buttons,
	.nbuttons	= ARRAY_SIZE(ip7145dpf_ubicom32input_buttons),
};

static struct platform_device ip7145dpf_ubicom32input_device = {
	.name	= "ubicom32input",
	.id	= -1,
	.dev	= {
		.platform_data = &ip7145dpf_ubicom32input_data,
	},
};

/*
 * List of all devices in our system
 */
static struct platform_device *ip7145dpf_devices[] __initdata = {
	&ip7145dpf_i2c_device,
	&ip7145dpf_lcdpower_device,
	&ip7145dpf_backlight_device,
	&ip7145dpf_ubicom32input_device,
};

/*
 * ip7145dpf_power_off
 *	Called to turn the power off for this board
 */
static void ip7145dpf_power_off(void)
{
	gpio_set_value(GPIO_RE_5, 0);
}

/*
 * ip7145dpf_init
 *	Called to add the devices which we have on this board
 */
static int __init ip7145dpf_init(void)
{
	int ret;
	struct platform_device *audio_dev;

	ubi_gpio_init();

#ifdef CONFIG_UIO_UBICOM32RING
	ring_tio_init("decoder_ring");
#endif

	/*
	 * Start up the video driver first
	 */
	vdc_tio_init();

	/*
	 * Take over holding of the power from the system
	 */
	ret = gpio_request(GPIO_RE_5, "power_hold");
	if (ret) {
		printk(KERN_ERR "%s: could not request power hold GPIO\n", __FUNCTION__);
	}
	gpio_direction_output(GPIO_RE_5, 1);
	mach_power_off = ip7145dpf_power_off;

	/*
	 * USB SEL_HOST_USB line
	 */
	ret = gpio_request(GPIO_RF_11, "SEL_HOST_USB");
	if (ret) {
		printk(KERN_ERR "%s: could not request SEL_HOST_USB GPIO\n", __FUNCTION__);
	}
	gpio_direction_output(GPIO_RF_11, 0);

	/*
	 * Setup audio
	 */
	audio_dev = audio_device_alloc("snd-ubi32-generic", "audio", "audio-i2sout", 0);
	if (audio_dev) {
		platform_device_register(audio_dev);
	}

	/*
	 * Register all of the devices we have on this board
	 */
	printk(KERN_INFO "%s: registering device resources\n", __FUNCTION__);
	platform_add_devices(ip7145dpf_devices, ARRAY_SIZE(ip7145dpf_devices));

	/*
	 * Register all of the devices which sit on the I2C bus
	 */
	printk(KERN_INFO "%s: registering i2c resources\n", __FUNCTION__);
	i2c_register_board_info(0, ip7145dpf_i2c_board_info, ARRAY_SIZE(ip7145dpf_i2c_board_info));

	/*
	 * We have to initialize the SPI after the I2C IOB gets setup. SPI is initialized in
	 * ip7145dpf_u72_setup
	 */

	return 0;
}

arch_initcall(ip7145dpf_init);

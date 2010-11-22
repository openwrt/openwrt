/*
 * arch/m68k/coldfire/m547x/devices.c
 *
 * Coldfire M547x/M548x Platform Device Configuration
 *
 * Copyright 2008-2009 Freescale Semiconductor, Inc. All Rights Reserved.
 *
 * Kurt Mahan <kmahan@freescale.com>
 */
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/fsl_devices.h>
#include <linux/spi/spi.h>
#include <linux/i2c.h>

#include <asm/coldfire.h>
#include <asm/mcfsim.h>

#ifdef CONFIG_SPI

#include <asm/mcfqspi.h>
/*
 *
 * DSPI
 *
 */

/* number of supported SPI selects */
#define SPI_NUM_CHIPSELECTS	8

void coldfire_spi_cs_control(u8 cs, u8 command)
{
	/* nothing special required */
}

#if defined(CONFIG_SPI_SPIDEV) || defined(CONFIG_SPI_SPIDEV_MODULE)
static struct coldfire_spi_chip spidev_chip_info = {
	.bits_per_word = 8,
};
#endif

static struct spi_board_info spi_board_info[] = {
#if defined(CONFIG_SPI_SPIDEV) || defined(CONFIG_SPI_SPIDEV_MODULE)
	{
		.modalias = "spidev",
		.max_speed_hz = 16000000,  	/* max clk (SCK) speed in HZ */
		.bus_num = 1,
		.chip_select = 0,		/* CS0 */
		.controller_data = &spidev_chip_info,
	}
#endif
};

static int spi_irq_list[] = {
	/* IRQ, 	     ICR Offset, 	ICR Val,Mask */
	64 + ISC_DSPI_OVRFW, ISC_DSPI_OVRFW,	0x18, 	0,
	64 + ISC_DSPI_RFOF,  ISC_DSPI_RFOF,	0x18, 	0,
	64 + ISC_DSPI_RFDF,  ISC_DSPI_RFDF,	0x18, 	0,
	64 + ISC_DSPI_TFUF,  ISC_DSPI_TFUF,	0x18, 	0,
	64 + ISC_DSPI_TCF,   ISC_DSPI_TCF,	0x18, 	0,
	64 + ISC_DSPI_TFFF,  ISC_DSPI_TFFF,	0x18, 	0,
	64 + ISC_DSPI_EOQF,  ISC_DSPI_EOQF,	0x18, 	0,
	0,0,0,0,
};

static struct coldfire_spi_master coldfire_master_info = {
	.bus_num = 1,
	.num_chipselect = SPI_NUM_CHIPSELECTS,
	.irq_list = spi_irq_list,
	.irq_source = 0,	/* not used */
	.irq_vector = 0,	/* not used */
	.irq_mask = 0,		/* not used */
	.irq_lp = 0,		/* not used */
	.par_val = 0,		/* not used */
	.cs_control = coldfire_spi_cs_control,
};

static struct resource coldfire_spi_resources[] = {
	[0] = {
		.name = "spi-par",
		.start = MCF_MBAR + 0x00000a50,	/* PAR_DSPI */
		.end = MCF_MBAR + 0x00000a50,	/* PAR_DSPI */
		.flags = IORESOURCE_MEM
	},

	[1] = {
		.name = "spi-module",
		.start = MCF_MBAR + 0x00008a00,	/* DSPI MCR Base */
		.end = MCF_MBAR + 0x00008ab8,	/* DSPI mem map end */
		.flags = IORESOURCE_MEM
	},

	[2] = {
		.name = "spi-int-level",
		.start = MCF_MBAR + 0x740,		 /* ICR start */
		.end = MCF_MBAR + 0x740 + ISC_DSPI_EOQF, /* ICR end */
		.flags = IORESOURCE_MEM
	},

	[3] = {
		.name = "spi-int-mask",
		.start = MCF_MBAR + 0x70c,	/* IMRL */
		.end = MCF_MBAR + 0x70c,	/* IMRL */
		.flags = IORESOURCE_MEM
	}
};

static struct platform_device coldfire_spi = {
	.name = "spi_coldfire",
	.id = -1,
	.resource = coldfire_spi_resources,
	.num_resources = ARRAY_SIZE(coldfire_spi_resources),
	.dev = {
		.platform_data = &coldfire_master_info,
	}
};

/**
 * m547x_8x_spi_init - Initialize SPI
 */
static int __init m547x_8x_spi_init(void)
{
	int retval;

	/* initialize the DSPI PAR */
	MCF_GPIO_PAR_DSPI = (MCF_GPIO_PAR_DSPI_PAR_CS5 |
			     MCF_GPIO_PAR_DSPI_PAR_CS3_DSPICS |
			     MCF_GPIO_PAR_DSPI_PAR_CS2_DSPICS |
			     MCF_GPIO_PAR_DSPI_PAR_CS0_DSPICS |
			     MCF_GPIO_PAR_DSPI_PAR_SCK_SCK |
			     MCF_GPIO_PAR_DSPI_PAR_SIN_SIN |
			     MCF_GPIO_PAR_DSPI_PAR_SOUT_SOUT);

	/* register device */
	retval = platform_device_register(&coldfire_spi);
	if (retval < 0) {
		goto out;
	}

	/* register board info */
	if (ARRAY_SIZE(spi_board_info))
		retval = spi_register_board_info(spi_board_info, ARRAY_SIZE(spi_board_info));

out:
	return retval;
}
#endif

#ifdef CONFIG_I2C_BOARDINFO
static struct i2c_board_info mcf_i2c_devices[] = {
	{
		I2C_BOARD_INFO("rv5c387a", 0x32),
	},
};
#endif

/**
 * m547x_8x_init_devices - Initialize M547X_8X devices
 *
 * Returns 0 on success.
 */
static int __init m547x_8x_init_devices(void)
{
#ifdef CONFIG_SPI
	m547x_8x_spi_init();
#endif
#ifdef CONFIG_I2C_BOARDINFO
	i2c_register_board_info(0, mcf_i2c_devices,
				ARRAY_SIZE(mcf_i2c_devices));
#endif

	return 0;
}
arch_initcall(m547x_8x_init_devices);

/*
 * Copyright (C) 2007 Openmoko, Inc.
 * Author: Harald Welte <laforge@openmoko.org>
 *
 * Smedia Glamo GPIO based SPI driver
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This driver currently only implements a minimum subset of the hardware
 * features, esp. those features that are required to drive the jbt6k74
 * LCM controller asic in the TD028TTEC1 LCM.
 *
*/

#define DEBUG

#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <linux/device.h>
#include <linux/spinlock.h>
#include <linux/platform_device.h>

#include <linux/spi/spi.h>
#include <linux/spi/spi_bitbang.h>
#include <linux/spi/glamo.h>

#include <linux/glamofb.h>

#include <mach/hardware.h>

#include "glamo-core.h"
#include "glamo-regs.h"

struct glamo_spigpio {
	struct spi_bitbang		bitbang;
	struct spi_master		*master;
	struct glamo_spigpio_platform_data	*info;
};

static inline struct glamo_spigpio *to_sg(struct spi_device *spi)
{
	return dev_get_drvdata(&spi->master->dev);
}

static inline void setsck(struct spi_device *dev, int on)
{
	struct glamo_spigpio *sg = to_sg(dev);
	glamo_gpio_setpin(sg->info->core, sg->info->pin_clk, on ? 1 : 0);
}

static inline void setmosi(struct spi_device *dev, int on)
{
	struct glamo_spigpio *sg = to_sg(dev);
	glamo_gpio_setpin(sg->info->core, sg->info->pin_mosi, on ? 1 : 0);
}

static inline u32 getmiso(struct spi_device *dev)
{
	struct glamo_spigpio *sg = to_sg(dev);
	if (sg->info->pin_miso)
		return glamo_gpio_getpin(sg->info->core, sg->info->pin_miso) ? 1 : 0;
	else
		return 0;
}

#define spidelay(x) ndelay(x)

#define EXPAND_BITBANG_TXRX
#include <linux/spi/spi_bitbang.h>

static u32 glamo_spigpio_txrx_mode0(struct spi_device *spi,
				    unsigned nsecs, u32 word, u8 bits)
{
	return bitbang_txrx_be_cpha0(spi, nsecs, 0, word, bits);
}

static u32 glamo_spigpio_txrx_mode1(struct spi_device *spi,
				    unsigned nsecs, u32 word, u8 bits)
{
	return bitbang_txrx_be_cpha1(spi, nsecs, 0, word, bits);
}

static u32 glamo_spigpio_txrx_mode2(struct spi_device *spi,
				    unsigned nsecs, u32 word, u8 bits)
{
	return bitbang_txrx_be_cpha0(spi, nsecs, 1, word, bits);
}

static u32 glamo_spigpio_txrx_mode3(struct spi_device *spi,
				    unsigned nsecs, u32 word, u8 bits)
{
	return bitbang_txrx_be_cpha1(spi, nsecs, 1, word, bits);
}


#if 0
static int glamo_spigpio_setupxfer(struct spi_device *spi,
				   struct spi_transfer *t)
{
	struct glamo_spi *gs = to_sg(spi);
	unsigned int bpw;

	bpw = t ? t->bits_per_word : spi->bits_per_word;

	if (bpw != 9 && bpw != 8) {
		dev_err(&spi->dev, "invalid bits-per-word (%d)\n", bpw);
		return -EINVAL;
	}

	return 0;
}
#endif

static void glamo_spigpio_chipsel(struct spi_device *spi, int value)
{
	struct glamo_spigpio *gs = to_sg(spi);
#if 0
	dev_dbg(&spi->dev, "chipsel %d: spi=%p, gs=%p, info=%p, handle=%p\n",
		value, spi, gs, gs->info, gs->info->glamo);
#endif
	glamo_gpio_setpin(gs->info->core, gs->info->pin_cs, value ? 0 : 1);
}


static int glamo_spigpio_probe(struct platform_device *pdev)
{
	struct spi_master *master;
	struct glamo_spigpio *sp;
	int ret;

	master = spi_alloc_master(&pdev->dev, sizeof(struct glamo_spigpio));
	if (master == NULL) {
		dev_err(&pdev->dev, "failed to allocate spi master\n");
		ret = -ENOMEM;
		goto err;
	}

	sp = spi_master_get_devdata(master);
	platform_set_drvdata(pdev, sp);
	sp->info = pdev->dev.platform_data;
	if (!sp->info) {
		dev_err(&pdev->dev, "can't operate without platform data\n");
		ret = -EIO;
		goto err_no_pdev;
	}

	master->num_chipselect = 1;
	master->bus_num = 2; /* FIXME: use dynamic number */

	sp->master = spi_master_get(master);

	sp->bitbang.master = sp->master;
	sp->bitbang.chipselect = glamo_spigpio_chipsel;
	sp->bitbang.txrx_word[SPI_MODE_0] = glamo_spigpio_txrx_mode0;
	sp->bitbang.txrx_word[SPI_MODE_1] = glamo_spigpio_txrx_mode1;
	sp->bitbang.txrx_word[SPI_MODE_2] = glamo_spigpio_txrx_mode2;
	sp->bitbang.txrx_word[SPI_MODE_3] = glamo_spigpio_txrx_mode3;

	/* set state of spi pins */
	glamo_gpio_setpin(sp->info->core, sp->info->pin_clk, 0);
	glamo_gpio_setpin(sp->info->core, sp->info->pin_mosi, 0);
	glamo_gpio_setpin(sp->info->core, sp->info->pin_cs, 1);

	glamo_gpio_cfgpin(sp->info->core, sp->info->pin_clk);
	glamo_gpio_cfgpin(sp->info->core, sp->info->pin_mosi);
	glamo_gpio_cfgpin(sp->info->core, sp->info->pin_cs);
	if (sp->info->pin_miso)
		glamo_gpio_cfgpin(sp->info->core, sp->info->pin_miso);

	/* bring the LCM panel out of reset if it isn't already */

	glamo_gpio_setpin(sp->info->core, GLAMO_GPIO4, 1);
	glamo_gpio_cfgpin(sp->info->core, GLAMO_GPIO4_OUTPUT);
	msleep(90);

#if 0
	sp->dev = &pdev->dev;

	sp->bitbang.setup_transfer = glamo_spi_setupxfer;
	sp->bitbang.txrx_bufs = glamo_spi_txrx;
	sp->bitbang.master->setup = glamo_spi_setup;
#endif

	dev_set_drvdata(&sp->master->dev, sp);

	ret = spi_bitbang_start(&sp->bitbang);
	if (ret)
		goto err_no_bitbang;

	return 0;

err_no_bitbang:
	platform_set_drvdata(pdev, NULL);
err_no_pdev:
	spi_master_put(sp->bitbang.master);
err:
	return ret;

}

static int glamo_spigpio_remove(struct platform_device *pdev)
{
	struct glamo_spigpio *sp = platform_get_drvdata(pdev);

	spi_bitbang_stop(&sp->bitbang);
	spi_master_put(sp->bitbang.master);

	return 0;
}


#ifdef CONFIG_PM
/*static int glamo_spigpio_suspend(struct device *dev)
{
	return 0;
}*/

static int glamo_spigpio_resume(struct device *dev)
{
	struct glamo_spigpio *sp = dev_get_drvdata(dev);

	if (!sp)
		return 0;

	/* set state of spi pins */
	glamo_gpio_setpin(sp->info->core, sp->info->pin_clk, 0);
	glamo_gpio_setpin(sp->info->core, sp->info->pin_mosi, 0);
	glamo_gpio_setpin(sp->info->core, sp->info->pin_cs, 1);

	glamo_gpio_cfgpin(sp->info->core, sp->info->pin_clk);
	glamo_gpio_cfgpin(sp->info->core, sp->info->pin_mosi);
	glamo_gpio_cfgpin(sp->info->core, sp->info->pin_cs);
	if (sp->info->pin_miso)
		glamo_gpio_cfgpin(sp->info->core, sp->info->pin_miso);

	return 0;
}

static struct dev_pm_ops glamo_spigpio_pm_ops = {
/*	.suspend = glamo_spiogpio_suspend,*/
	.resume_noirq = glamo_spigpio_resume,
};

#define GLAMO_SPIGPIO_PM_OPS (&glamo_spigpio_pm_ops)

#else
#define GLAMO_SPIGPIO_PM_OPS NULL
#endif

static struct platform_driver glamo_spi_drv = {
	.probe		= glamo_spigpio_probe,
	.remove		= glamo_spigpio_remove,
	.driver		= {
		.name	= "glamo-spi-gpio",
		.owner	= THIS_MODULE,
		.pm     = GLAMO_SPIGPIO_PM_OPS,
	},
};

static int __init glamo_spi_init(void)
{
	return platform_driver_register(&glamo_spi_drv);
}

static void __exit glamo_spi_exit(void)
{
	platform_driver_unregister(&glamo_spi_drv);
}

module_init(glamo_spi_init);
module_exit(glamo_spi_exit);

MODULE_DESCRIPTION("Smedia Glamo 336x/337x LCM serial command SPI Driver");
MODULE_AUTHOR("Harald Welte <laforge@openmoko.org>")
MODULE_LICENSE("GPL");

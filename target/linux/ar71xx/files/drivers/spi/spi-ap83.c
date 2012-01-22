/*
 * Atheros AP83 board specific SPI Controller driver
 *
 * Copyright (C) 2009 Gabor Juhos <juhosg@openwrt.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 */

#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <linux/spinlock.h>
#include <linux/workqueue.h>
#include <linux/platform_device.h>
#include <linux/io.h>
#include <linux/spi/spi.h>
#include <linux/spi/spi_bitbang.h>
#include <linux/bitops.h>
#include <linux/gpio.h>

#include <asm/mach-ar71xx/ar71xx.h>
#include <asm/mach-ar71xx/platform.h>

#define DRV_DESC	"Atheros AP83 board SPI Controller driver"
#define DRV_VERSION	"0.1.0"
#define DRV_NAME	"ap83-spi"

#define AP83_SPI_CLK_HIGH	(1 << 23)
#define AP83_SPI_CLK_LOW	0
#define AP83_SPI_MOSI_HIGH	(1 << 22)
#define AP83_SPI_MOSI_LOW	0

#define AP83_SPI_GPIO_CS	1
#define AP83_SPI_GPIO_MISO	3

struct ap83_spi {
	struct	spi_bitbang	bitbang;
	void __iomem		*base;
	u32			addr;

	struct platform_device	*pdev;
};

static inline u32 ap83_spi_rr(struct ap83_spi *sp, u32 reg)
{
	return __raw_readl(sp->base + reg);
}

static inline struct ap83_spi *spidev_to_sp(struct spi_device *spi)
{
	return spi_master_get_devdata(spi->master);
}

static inline void setsck(struct spi_device *spi, int val)
{
	struct ap83_spi *sp = spidev_to_sp(spi);

	if (val)
		sp->addr |= AP83_SPI_CLK_HIGH;
	else
		sp->addr &= ~AP83_SPI_CLK_HIGH;

	dev_dbg(&spi->dev, "addr=%08x,  SCK set to %s\n",
		sp->addr, (val) ? "HIGH" : "LOW");

	ap83_spi_rr(sp, sp->addr);
}

static inline void setmosi(struct spi_device *spi, int val)
{
	struct ap83_spi *sp = spidev_to_sp(spi);

	if (val)
		sp->addr |= AP83_SPI_MOSI_HIGH;
	else
		sp->addr &= ~AP83_SPI_MOSI_HIGH;

	dev_dbg(&spi->dev, "addr=%08x, MOSI set to %s\n",
		sp->addr, (val) ? "HIGH" : "LOW");

	ap83_spi_rr(sp, sp->addr);
}

static inline u32 getmiso(struct spi_device *spi)
{
	u32 ret;

	ret = gpio_get_value(AP83_SPI_GPIO_MISO) ? 1 : 0;
	dev_dbg(&spi->dev, "get MISO: %d\n", ret);

	return ret;
}

static inline void do_spidelay(struct spi_device *spi, unsigned nsecs)
{
	ndelay(nsecs);
}

static void ap83_spi_chipselect(struct spi_device *spi, int on)
{
	struct ap83_spi *sp = spidev_to_sp(spi);

	dev_dbg(&spi->dev, "set CS to %d\n", (on) ? 0 : 1);

	if (on) {
		ar71xx_flash_acquire();

		sp->addr = 0;
		ap83_spi_rr(sp, sp->addr);

		gpio_set_value(AP83_SPI_GPIO_CS, 0);
	} else {
		gpio_set_value(AP83_SPI_GPIO_CS, 1);
		ar71xx_flash_release();
	}
}

#define spidelay(nsecs)							\
	do {								\
		/* Steal the spi_device pointer from our caller.	\
		 * The bitbang-API should probably get fixed here... */	\
		do_spidelay(spi, nsecs);				\
	} while (0)

#define EXPAND_BITBANG_TXRX
#include <linux/spi/spi_bitbang.h>
#include "spi_bitbang_txrx.h"

static u32 ap83_spi_txrx_mode0(struct spi_device *spi,
			       unsigned nsecs, u32 word, u8 bits)
{
	dev_dbg(&spi->dev, "TXRX0 word=%08x, bits=%u\n", word, bits);
	return bitbang_txrx_be_cpha0(spi, nsecs, 0, 0, word, bits);
}

static u32 ap83_spi_txrx_mode1(struct spi_device *spi,
			       unsigned nsecs, u32 word, u8 bits)
{
	dev_dbg(&spi->dev, "TXRX1 word=%08x, bits=%u\n", word, bits);
	return bitbang_txrx_be_cpha1(spi, nsecs, 0, 0, word, bits);
}

static u32 ap83_spi_txrx_mode2(struct spi_device *spi,
			       unsigned nsecs, u32 word, u8 bits)
{
	dev_dbg(&spi->dev, "TXRX2 word=%08x, bits=%u\n", word, bits);
	return bitbang_txrx_be_cpha0(spi, nsecs, 1, 0, word, bits);
}

static u32 ap83_spi_txrx_mode3(struct spi_device *spi,
			       unsigned nsecs, u32 word, u8 bits)
{
	dev_dbg(&spi->dev, "TXRX3 word=%08x, bits=%u\n", word, bits);
	return bitbang_txrx_be_cpha1(spi, nsecs, 1, 0, word, bits);
}

static int ap83_spi_probe(struct platform_device *pdev)
{
	struct spi_master *master;
	struct ap83_spi *sp;
	struct ap83_spi_platform_data *pdata;
	struct resource	*r;
	int ret;

	ret = gpio_request(AP83_SPI_GPIO_MISO, "spi-miso");
	if (ret) {
		dev_err(&pdev->dev, "gpio request failed for MISO\n");
		return ret;
	}

	ret = gpio_request(AP83_SPI_GPIO_CS, "spi-cs");
	if (ret) {
		dev_err(&pdev->dev, "gpio request failed for CS\n");
		goto err_free_miso;
	}

	ret = gpio_direction_input(AP83_SPI_GPIO_MISO);
	if (ret) {
		dev_err(&pdev->dev, "unable to set direction of MISO\n");
		goto err_free_cs;
	}

	ret = gpio_direction_output(AP83_SPI_GPIO_CS, 0);
	if (ret) {
		dev_err(&pdev->dev, "unable to set direction of CS\n");
		goto err_free_cs;
	}

	master = spi_alloc_master(&pdev->dev, sizeof(*sp));
	if (master == NULL) {
		dev_err(&pdev->dev, "failed to allocate spi master\n");
		return -ENOMEM;
	}

	sp = spi_master_get_devdata(master);
	platform_set_drvdata(pdev, sp);

	pdata = pdev->dev.platform_data;

	sp->bitbang.master = spi_master_get(master);
	sp->bitbang.chipselect = ap83_spi_chipselect;
	sp->bitbang.txrx_word[SPI_MODE_0] = ap83_spi_txrx_mode0;
	sp->bitbang.txrx_word[SPI_MODE_1] = ap83_spi_txrx_mode1;
	sp->bitbang.txrx_word[SPI_MODE_2] = ap83_spi_txrx_mode2;
	sp->bitbang.txrx_word[SPI_MODE_3] = ap83_spi_txrx_mode3;

	sp->bitbang.master->bus_num = pdev->id;
	sp->bitbang.master->num_chipselect = 1;

	r = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if (r == NULL) {
		ret = -ENOENT;
		goto err_spi_put;
	}

	sp->base = ioremap_nocache(r->start, r->end - r->start + 1);
	if (!sp->base) {
		ret = -ENXIO;
		goto err_spi_put;
	}

	ret = spi_bitbang_start(&sp->bitbang);
	if (!ret)
		goto err_unmap;

	dev_info(&pdev->dev, "AP83 SPI adapter at %08x\n", r->start);

	return 0;

err_unmap:
	iounmap(sp->base);
err_spi_put:
	platform_set_drvdata(pdev, NULL);
	spi_master_put(sp->bitbang.master);

err_free_cs:
	gpio_free(AP83_SPI_GPIO_CS);
err_free_miso:
	gpio_free(AP83_SPI_GPIO_MISO);
	return ret;
}

static int ap83_spi_remove(struct platform_device *pdev)
{
	struct ap83_spi *sp = platform_get_drvdata(pdev);

	spi_bitbang_stop(&sp->bitbang);
	iounmap(sp->base);
	platform_set_drvdata(pdev, NULL);
	spi_master_put(sp->bitbang.master);

	return 0;
}

static struct platform_driver ap83_spi_drv = {
	.probe		= ap83_spi_probe,
	.remove		= ap83_spi_remove,
	.driver		= {
		.name	= DRV_NAME,
		.owner	= THIS_MODULE,
	},
};

static int __init ap83_spi_init(void)
{
	return platform_driver_register(&ap83_spi_drv);
}
module_init(ap83_spi_init);

static void __exit ap83_spi_exit(void)
{
	platform_driver_unregister(&ap83_spi_drv);
}
module_exit(ap83_spi_exit);

MODULE_ALIAS("platform:" DRV_NAME);
MODULE_DESCRIPTION(DRV_DESC);
MODULE_VERSION(DRV_VERSION);
MODULE_AUTHOR("Gabor Juhos <juhosg@openwrt.org>");
MODULE_LICENSE("GPL v2");

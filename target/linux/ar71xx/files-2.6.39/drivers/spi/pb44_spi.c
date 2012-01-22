/*
 * Atheros PB44 board SPI controller driver
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

#define DRV_DESC	"Atheros PB44 SPI Controller driver"
#define DRV_VERSION	"0.1.0"
#define DRV_NAME	"pb44-spi"

#undef PER_BIT_READ

struct ar71xx_spi {
	struct	spi_bitbang	bitbang;
	u32			ioc_base;
	u32			reg_ctrl;

	void __iomem		*base;

	struct platform_device	*pdev;
};

static inline u32 pb44_spi_rr(struct ar71xx_spi *sp, unsigned reg)
{
	return __raw_readl(sp->base + reg);
}

static inline void pb44_spi_wr(struct ar71xx_spi *sp, unsigned reg, u32 val)
{
	__raw_writel(val, sp->base + reg);
}

static inline struct ar71xx_spi *spidev_to_sp(struct spi_device *spi)
{
	return spi_master_get_devdata(spi->master);
}

static void pb44_spi_chipselect(struct spi_device *spi, int is_active)
{
	struct ar71xx_spi *sp = spidev_to_sp(spi);
	int cs_high = (spi->mode & SPI_CS_HIGH) ? is_active : !is_active;

	if (is_active) {
		/* set initial clock polarity */
		if (spi->mode & SPI_CPOL)
			sp->ioc_base |= SPI_IOC_CLK;
		else
			sp->ioc_base &= ~SPI_IOC_CLK;

		pb44_spi_wr(sp, SPI_REG_IOC, sp->ioc_base);
	}

	if (spi->chip_select) {
		unsigned long gpio = (unsigned long) spi->controller_data;

		/* SPI is normally active-low */
		gpio_set_value(gpio, cs_high);
	} else {
		if (cs_high)
			sp->ioc_base |= SPI_IOC_CS0;
		else
			sp->ioc_base &= ~SPI_IOC_CS0;

		pb44_spi_wr(sp, SPI_REG_IOC, sp->ioc_base);
	}

}

static void pb44_spi_enable(struct ar71xx_spi *sp)
{
	/* enable GPIO mode */
	pb44_spi_wr(sp, SPI_REG_FS, SPI_FS_GPIO);

	/* save CTRL register */
	sp->reg_ctrl = pb44_spi_rr(sp, SPI_REG_CTRL);
	sp->ioc_base = pb44_spi_rr(sp, SPI_REG_IOC);

	pb44_spi_wr(sp, SPI_REG_CTRL, 0x43);
}

static void pb44_spi_disable(struct ar71xx_spi *sp)
{
	/* restore CTRL register */
	pb44_spi_wr(sp, SPI_REG_CTRL, sp->reg_ctrl);
	/* disable GPIO mode */
	pb44_spi_wr(sp, SPI_REG_FS, 0);
}

static int pb44_spi_setup_cs(struct spi_device *spi)
{
	struct ar71xx_spi *sp = spidev_to_sp(spi);

	if (spi->chip_select) {
		unsigned long gpio = (unsigned long) spi->controller_data;
		int status = 0;

		status = gpio_request(gpio, dev_name(&spi->dev));
		if (status)
			return status;

		status = gpio_direction_output(gpio, spi->mode & SPI_CS_HIGH);
		if (status) {
			gpio_free(gpio);
			return status;
		}
	} else {
		if (spi->mode & SPI_CS_HIGH)
			sp->ioc_base |= SPI_IOC_CS0;
		else
			sp->ioc_base &= ~SPI_IOC_CS0;
		pb44_spi_wr(sp, SPI_REG_IOC, sp->ioc_base);
	}

	return 0;
}

static void pb44_spi_cleanup_cs(struct spi_device *spi)
{
	if (spi->chip_select) {
		unsigned long gpio = (unsigned long) spi->controller_data;
		gpio_free(gpio);
	}
}

static int pb44_spi_setup(struct spi_device *spi)
{
	int status = 0;

	if (spi->bits_per_word > 32)
		return -EINVAL;

	if (!spi->controller_state) {
		status = pb44_spi_setup_cs(spi);
		if (status)
			return status;
	}

	status = spi_bitbang_setup(spi);
	if (status && !spi->controller_state)
		pb44_spi_cleanup_cs(spi);

	return status;
}

static void pb44_spi_cleanup(struct spi_device *spi)
{
	pb44_spi_cleanup_cs(spi);
	spi_bitbang_cleanup(spi);
}

static u32 pb44_spi_txrx_mode0(struct spi_device *spi, unsigned nsecs,
			       u32 word, u8 bits)
{
	struct ar71xx_spi *sp = spidev_to_sp(spi);
	u32 ioc = sp->ioc_base;
	u32 ret;

	/* clock starts at inactive polarity */
	for (word <<= (32 - bits); likely(bits); bits--) {
		u32 out;

		if (word & (1 << 31))
			out = ioc | SPI_IOC_DO;
		else
			out = ioc & ~SPI_IOC_DO;

		/* setup MSB (to slave) on trailing edge */
		pb44_spi_wr(sp, SPI_REG_IOC, out);
		pb44_spi_wr(sp, SPI_REG_IOC, out | SPI_IOC_CLK);

		word <<= 1;

#ifdef PER_BIT_READ
		/* sample MSB (from slave) on leading edge */
		ret = pb44_spi_rr(sp, SPI_REG_RDS);
		pb44_spi_wr(sp, SPI_REG_IOC, out);
#endif
	}

#ifndef PER_BIT_READ
	ret = pb44_spi_rr(sp, SPI_REG_RDS);
#endif
	return ret;
}

static int pb44_spi_probe(struct platform_device *pdev)
{
	struct spi_master *master;
	struct ar71xx_spi *sp;
	struct ar71xx_spi_platform_data *pdata;
	struct resource	*r;
	int ret;

	master = spi_alloc_master(&pdev->dev, sizeof(*sp));
	if (master == NULL) {
		dev_err(&pdev->dev, "failed to allocate spi master\n");
		return -ENOMEM;
	}

	sp = spi_master_get_devdata(master);
	platform_set_drvdata(pdev, sp);

	pdata = pdev->dev.platform_data;

	master->setup = pb44_spi_setup;
	master->cleanup = pb44_spi_cleanup;
	if (pdata) {
		master->bus_num = pdata->bus_num;
		master->num_chipselect = pdata->num_chipselect;
	} else {
		master->bus_num = 0;
		master->num_chipselect = 1;
	}

	sp->bitbang.master = spi_master_get(master);
	sp->bitbang.chipselect = pb44_spi_chipselect;
	sp->bitbang.txrx_word[SPI_MODE_0] = pb44_spi_txrx_mode0;
	sp->bitbang.setup_transfer = spi_bitbang_setup_transfer;
	sp->bitbang.flags = SPI_CS_HIGH;

	r = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if (r == NULL) {
		ret = -ENOENT;
		goto err1;
	}

	sp->base = ioremap_nocache(r->start, r->end - r->start + 1);
	if (!sp->base) {
		ret = -ENXIO;
		goto err1;
	}

	pb44_spi_enable(sp);

	ret = spi_bitbang_start(&sp->bitbang);
	if (!ret)
		return 0;

	pb44_spi_disable(sp);
	iounmap(sp->base);
err1:
	platform_set_drvdata(pdev, NULL);
	spi_master_put(sp->bitbang.master);

	return ret;
}

static int pb44_spi_remove(struct platform_device *pdev)
{
	struct ar71xx_spi *sp = platform_get_drvdata(pdev);

	spi_bitbang_stop(&sp->bitbang);
	pb44_spi_disable(sp);
	iounmap(sp->base);
	platform_set_drvdata(pdev, NULL);
	spi_master_put(sp->bitbang.master);

	return 0;
}

static void pb44_spi_shutdown(struct platform_device *pdev)
{
	int ret;

	ret = pb44_spi_remove(pdev);
	if (ret)
		dev_err(&pdev->dev, "shutdown failed with %d\n", ret);
}

static struct platform_driver pb44_spi_drv = {
	.probe		= pb44_spi_probe,
	.remove		= pb44_spi_remove,
	.shutdown	= pb44_spi_shutdown,
	.driver		= {
		.name	= DRV_NAME,
		.owner	= THIS_MODULE,
	},
};

static int __init pb44_spi_init(void)
{
	return platform_driver_register(&pb44_spi_drv);
}
module_init(pb44_spi_init);

static void __exit pb44_spi_exit(void)
{
	platform_driver_unregister(&pb44_spi_drv);
}
module_exit(pb44_spi_exit);

MODULE_ALIAS("platform:" DRV_NAME);
MODULE_DESCRIPTION(DRV_DESC);
MODULE_VERSION(DRV_VERSION);
MODULE_AUTHOR("Gabor Juhos <juhosg@openwrt.org>");
MODULE_LICENSE("GPL v2");

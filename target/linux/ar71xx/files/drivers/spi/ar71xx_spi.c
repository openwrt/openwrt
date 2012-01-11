/*
 * Atheros AR71xx SPI Controller driver
 *
 * Copyright (C) 2008-2009 Gabor Juhos <juhosg@openwrt.org>
 * Copyright (C) 2008 Imre Kaloz <kaloz@openwrt.org>
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

#include <asm/mach-ar71xx/ar71xx.h>
#include <asm/mach-ar71xx/platform.h>

#define DRV_DESC	"Atheros AR71xx SPI Controller driver"
#define DRV_VERSION	"0.2.4"
#define DRV_NAME	"ar71xx-spi"

#undef PER_BIT_READ

struct ar71xx_spi {
	struct	spi_bitbang	bitbang;
	u32			ioc_base;
	u32			reg_ctrl;

	void __iomem		*base;

	struct platform_device	*pdev;
	u32			(*get_ioc_base)(u8 chip_select, int cs_high,
						int is_on);
};

static inline u32 ar71xx_spi_rr(struct ar71xx_spi *sp, unsigned reg)
{
	return __raw_readl(sp->base + reg);
}

static inline void ar71xx_spi_wr(struct ar71xx_spi *sp, unsigned reg, u32 val)
{
	__raw_writel(val, sp->base + reg);
}

static inline struct ar71xx_spi *spidev_to_sp(struct spi_device *spi)
{
	return spi_master_get_devdata(spi->master);
}

static u32 ar71xx_spi_get_ioc_base(u8 chip_select, int cs_high, int is_on)
{
	u32 ret;

	if (is_on == AR71XX_SPI_CS_INACTIVE)
		ret = SPI_IOC_CS_ALL;
	else
		ret = SPI_IOC_CS_ALL & ~SPI_IOC_CS(chip_select);

	return ret;
}

static void ar71xx_spi_chipselect(struct spi_device *spi, int value)
{
	struct ar71xx_spi *sp = spidev_to_sp(spi);
	void __iomem *base = sp->base;
	u32 ioc_base;

	switch (value) {
	case BITBANG_CS_INACTIVE:
		ioc_base = sp->get_ioc_base(spi->chip_select,
					(spi->mode & SPI_CS_HIGH) != 0,
					AR71XX_SPI_CS_INACTIVE);
		__raw_writel(ioc_base, base + SPI_REG_IOC);
		break;

	case BITBANG_CS_ACTIVE:
		ioc_base = sp->get_ioc_base(spi->chip_select,
					(spi->mode & SPI_CS_HIGH) != 0,
					AR71XX_SPI_CS_ACTIVE);

		__raw_writel(ioc_base, base + SPI_REG_IOC);
		sp->ioc_base = ioc_base;
		break;
	}
}

static void ar71xx_spi_setup_regs(struct ar71xx_spi *sp)
{
	/* enable GPIO mode */
	ar71xx_spi_wr(sp, SPI_REG_FS, SPI_FS_GPIO);

	/* save CTRL register */
	sp->reg_ctrl = ar71xx_spi_rr(sp, SPI_REG_CTRL);

	/* TODO: setup speed? */
	ar71xx_spi_wr(sp, SPI_REG_CTRL, 0x43);
}

static void ar71xx_spi_restore_regs(struct ar71xx_spi *sp)
{
	/* restore CTRL register */
	ar71xx_spi_wr(sp, SPI_REG_CTRL, sp->reg_ctrl);
	/* disable GPIO mode */
	ar71xx_spi_wr(sp, SPI_REG_FS, 0);
}

static int ar71xx_spi_setup(struct spi_device *spi)
{
	if (spi->bits_per_word > 32)
		return -EINVAL;

	return spi_bitbang_setup(spi);
}

static void ar71xx_spi_cleanup(struct spi_device *spi)
{
	spi_bitbang_cleanup(spi);
}

static u32 ar71xx_spi_txrx_mode0(struct spi_device *spi, unsigned nsecs,
					u32 word, u8 bits)
{
	struct ar71xx_spi *sp = spidev_to_sp(spi);
	void __iomem *base = sp->base;
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
		__raw_writel(out, base + SPI_REG_IOC);

		__raw_writel(out | SPI_IOC_CLK, base + SPI_REG_IOC);

		word <<= 1;

#ifdef PER_BIT_READ
		/* sample MSB (from slave) on leading edge */
		ret = __raw_readl(base + SPI_REG_RDS);
		__raw_writel(out, base + SPI_REG_IOC);
#endif

	}

#ifndef PER_BIT_READ
	ret = __raw_readl(base + SPI_REG_RDS);
#endif
	return ret;
}

static int ar71xx_spi_probe(struct platform_device *pdev)
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

	master->setup = ar71xx_spi_setup;
	master->cleanup = ar71xx_spi_cleanup;

	sp->bitbang.master = spi_master_get(master);
	sp->bitbang.chipselect = ar71xx_spi_chipselect;
	sp->bitbang.txrx_word[SPI_MODE_0] = ar71xx_spi_txrx_mode0;
	sp->bitbang.setup_transfer = spi_bitbang_setup_transfer;

	sp->get_ioc_base = ar71xx_spi_get_ioc_base;
	if (pdata) {
		sp->bitbang.master->bus_num = pdata->bus_num;
		sp->bitbang.master->num_chipselect = pdata->num_chipselect;
		if (pdata->get_ioc_base)
			sp->get_ioc_base = pdata->get_ioc_base;
	} else {
		sp->bitbang.master->bus_num = 0;
		sp->bitbang.master->num_chipselect = 3;
	}

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

	ar71xx_spi_setup_regs(sp);

	ret = spi_bitbang_start(&sp->bitbang);
	if (!ret)
		return 0;

	ar71xx_spi_restore_regs(sp);
	iounmap(sp->base);
err1:
	platform_set_drvdata(pdev, NULL);
	spi_master_put(sp->bitbang.master);

	return ret;
}

static int ar71xx_spi_remove(struct platform_device *pdev)
{
	struct ar71xx_spi *sp = platform_get_drvdata(pdev);

	spi_bitbang_stop(&sp->bitbang);
	ar71xx_spi_restore_regs(sp);
	iounmap(sp->base);
	platform_set_drvdata(pdev, NULL);
	spi_master_put(sp->bitbang.master);

	return 0;
}

static struct platform_driver ar71xx_spi_drv = {
	.probe		= ar71xx_spi_probe,
	.remove		= ar71xx_spi_remove,
	.driver		= {
		.name	= DRV_NAME,
		.owner	= THIS_MODULE,
	},
};

static int __init ar71xx_spi_init(void)
{
	printk(KERN_INFO DRV_DESC " version " DRV_VERSION "\n");
	return platform_driver_register(&ar71xx_spi_drv);
}
module_init(ar71xx_spi_init);

static void __exit ar71xx_spi_exit(void)
{
	platform_driver_unregister(&ar71xx_spi_drv);
}
module_exit(ar71xx_spi_exit);

MODULE_ALIAS("platform:" DRV_NAME);
MODULE_DESCRIPTION(DRV_DESC);
MODULE_VERSION(DRV_VERSION);
MODULE_AUTHOR("Gabor Juhos <juhosg@openwrt.org>");
MODULE_AUTHOR("Imre Kaloz <kaloz@openwrt.org>");
MODULE_LICENSE("GPL v2");

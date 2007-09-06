/*
 * Simple bitbanged-GPIO SPI driver for ETRAX FS et al.
 *
 * Copyright (c) 2007 Axis Communications AB
 *
 * Author: Hans-Peter Nilsson, inspired by earlier work by
 * Andre Spanberg but mostly by copying large parts of
 * spi_s3c24xx_gpio.c, hence also:
 * Copyright (c) 2006 Ben Dooks
 * Copyright (c) 2006 Simtec Electronics
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 */

#include <linux/types.h>
#include <linux/device.h>
#include <linux/spi/spi.h>
#include <linux/spi/spi_bitbang.h>
#include <linux/delay.h>
#include <linux/platform_device.h>
#include <asm/io.h>
#include <asm/arch/board.h>

/* Our main driver state.  */

struct crisv32_spi_hw_info {
	struct crisv32_iopin sclk;
	struct crisv32_iopin mosi;
	struct crisv32_iopin miso;
	struct crisv32_iopin cs;
};

/*
 * The driver state hides behind the spi_bitbang state.  We're
 * responsible for allocating that, so we can get a little something
 * for ourselves.
 */

struct crisv32_spi_gpio_devdata {
	struct spi_bitbang bitbang;
	struct crisv32_spi_hw_info pins;
};

/* Helper function getting the driver state from a spi_device.  */

static inline struct crisv32_spi_hw_info *spidev_to_hw(struct spi_device *spi)
{
	struct crisv32_spi_gpio_devdata *dd = spi_master_get_devdata(spi->master);
	return &dd->pins;
}

/* The SPI-bitbang functions: see spi_bitbang.h at EXPAND_BITBANG_TXRX.  */

static inline void setsck(struct spi_device *spi, int is_on)
{
	crisv32_io_set(&spidev_to_hw(spi)->sclk, is_on != 0);
}

static inline void setmosi(struct spi_device *spi, int is_on)
{
	crisv32_io_set(&spidev_to_hw(spi)->mosi, is_on != 0);
}

static inline u32 getmiso(struct spi_device *spi)
{
	return crisv32_io_rd(&spidev_to_hw(spi)->miso) != 0 ? 1 : 0;
}

#define spidelay(x) ndelay(x)

#define	EXPAND_BITBANG_TXRX
#include <linux/spi/spi_bitbang.h>

/*
 * SPI-bitbang word transmit-functions for the four SPI modes,
 * dispatching to the inlined functions we just included.
 */

static u32 crisv32_spi_gpio_txrx_mode0(struct spi_device *spi,
				      unsigned nsecs, u32 word, u8 bits)
{
	return bitbang_txrx_be_cpha0(spi, nsecs, 0, word, bits);
}

static u32 crisv32_spi_gpio_txrx_mode1(struct spi_device *spi,
				       unsigned nsecs, u32 word, u8 bits)
{
	return bitbang_txrx_be_cpha1(spi, nsecs, 0, word, bits);
}

static u32 crisv32_spi_gpio_txrx_mode2(struct spi_device *spi,
				      unsigned nsecs, u32 word, u8 bits)
{
	return bitbang_txrx_be_cpha0(spi, nsecs, 1, word, bits);
}

static u32 crisv32_spi_gpio_txrx_mode3(struct spi_device *spi,
				       unsigned nsecs, u32 word, u8 bits)
{
	return bitbang_txrx_be_cpha1(spi, nsecs, 1, word, bits);
}

/* SPI-bitbang chip-select function.  */

static void crisv32_spi_gpio_chipselect(struct spi_device *spi, int value)
{
	if (spi->mode & SPI_CS_HIGH)
		crisv32_io_set(&spidev_to_hw(spi)->cs,
			       value == BITBANG_CS_ACTIVE ? 1 : 0);
	else
		crisv32_io_set(&spidev_to_hw(spi)->cs,
			       value == BITBANG_CS_ACTIVE ? 0 : 1);
}

/* Platform-device probe function.  */

static int __devinit crisv32_spi_gpio_probe(struct platform_device *dev)
{
	struct spi_master *master;
	struct crisv32_spi_gpio_devdata  *dd;
	struct resource *res;
	struct crisv32_spi_gpio_controller_data *gc;
	int ret = 0;

	/*
	 * We need to get the controller data as a hardware resource,
	 * or else it wouldn't be available until *after* the
	 * spi_bitbang_start call!
	 */
	res = platform_get_resource_byname(dev, 0, "controller_data_ptr");
	if (res == NULL) {
		dev_err(&dev->dev, "can't get controller_data resource\n");
		return -EIO;
	}

	gc = (struct crisv32_spi_gpio_controller_data *) res->start;

	master = spi_alloc_master(&dev->dev, sizeof *dd);
	if (master == NULL) {
		dev_err(&dev->dev, "failed to allocate spi master\n");
		ret = -ENOMEM;
		goto err;
	}

	dd = spi_master_get_devdata(master);
	platform_set_drvdata(dev, dd);

	/*
	 * The device data asks for this driver, and holds the id
	 * number, which must be unique among the same-type devices.
	 * We use this as the number of this SPI bus.
	 */
	master->bus_num = dev->id;

	/*
	 * Allocate pins.  Note that thus being allocated as GPIO, we
	 * don't have to deconfigure them at the end or if something
	 * fails.
	 */
	if ((ret = crisv32_io_get_name(&dd->pins.cs, gc->cs)) != 0
	    || (ret = crisv32_io_get_name(&dd->pins.miso, gc->miso)) != 0
	    || (ret = crisv32_io_get_name(&dd->pins.mosi, gc->mosi)) != 0
	    || (ret = crisv32_io_get_name(&dd->pins.sclk, gc->sclk)) != 0)
		goto err_no_pins;

	/* Set directions of the SPI pins.  */
	crisv32_io_set_dir(&dd->pins.cs, crisv32_io_dir_out);
	crisv32_io_set_dir(&dd->pins.sclk, crisv32_io_dir_out);
	crisv32_io_set_dir(&dd->pins.miso, crisv32_io_dir_in);
	crisv32_io_set_dir(&dd->pins.mosi, crisv32_io_dir_out);

	/* Set state of the SPI pins.  */
	dev_dbg(&dev->dev, "cs.port 0x%x, pin: %d\n"
		dd->pins.cs.port, dd->pins.cs.bit);

	/*
	 * Can't use crisv32_spi_gpio_chipselect(spi, 1) here; we
	 * don't have a proper "spi" until after spi_bitbang_start.
	 */
	crisv32_io_set(&dd->pins.cs, 1);
	crisv32_io_set(&dd->pins.sclk, 0);
	crisv32_io_set(&dd->pins.mosi, 0);

	/* Setup SPI bitbang adapter hooks.  */
	dd->bitbang.master = spi_master_get(master);
	dd->bitbang.chipselect = crisv32_spi_gpio_chipselect;

	dd->bitbang.txrx_word[SPI_MODE_0] = crisv32_spi_gpio_txrx_mode0;
	dd->bitbang.txrx_word[SPI_MODE_1] = crisv32_spi_gpio_txrx_mode1;
	dd->bitbang.txrx_word[SPI_MODE_2] = crisv32_spi_gpio_txrx_mode2;
	dd->bitbang.txrx_word[SPI_MODE_3] = crisv32_spi_gpio_txrx_mode3;

	ret = spi_bitbang_start(&dd->bitbang);
	if (ret)
		goto err_no_bitbang;

	printk (KERN_INFO "CRIS v32 SPI driver for GPIO"
		" (cs: %s, miso: %s, mosi: %s, sclk: %s)\n",
		gc->cs, gc->miso, gc->mosi, gc->sclk);

	return 0;

 err_no_bitbang:
	spi_master_put(dd->bitbang.master);
 err_no_pins:
	platform_set_drvdata(dev, NULL);
 err:
	return ret;
}

/* Platform-device remove-function.  */

static int __devexit crisv32_spi_gpio_remove(struct platform_device *dev)
{
	struct crisv32_spi_gpio_devdata *dd = platform_get_drvdata(dev);
	int ret;

	ret = spi_bitbang_stop(&dd->bitbang);
	if (ret != 0)
		return ret;

	spi_master_put(dd->bitbang.master);
	platform_set_drvdata(dev, NULL);
	return 0;
}

/*
 * For the time being, there's no suspend/resume support to care
 * about, so we let those handlers default to NULL.
 */
static struct platform_driver crisv32_spi_gpio_drv = {
	.probe		= crisv32_spi_gpio_probe,
	.remove		= __devexit_p(crisv32_spi_gpio_remove),
	.driver		= {
		.name	= "spi_crisv32_gpio",
		.owner	= THIS_MODULE,
	},
};

/* Module init function.  */

static int __devinit crisv32_spi_gpio_init(void)
{
	return platform_driver_register(&crisv32_spi_gpio_drv);
}

/* Module exit function.  */

static void __devexit crisv32_spi_gpio_exit(void)
{
	platform_driver_unregister(&crisv32_spi_gpio_drv);
}

module_init(crisv32_spi_gpio_init);
module_exit(crisv32_spi_gpio_exit);

MODULE_DESCRIPTION("CRIS v32 SPI-GPIO Driver");
MODULE_AUTHOR("Hans-Peter Nilsson, <hp@axis.com>");
MODULE_LICENSE("GPL");

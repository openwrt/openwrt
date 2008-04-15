/*
 * Bitbanging SPI bus driver using GPIO API
 *
 * Copyright (c) 2008 Piotr Skamruk
 * Copyright (c) 2008 Michael Buesch
 *
 * based on spi_s3c2410_gpio.c
 *   Copyright (c) 2006 Ben Dooks
 *   Copyright (c) 2006 Simtec Electronics
 * and on i2c-gpio.c
 *   Copyright (C) 2007 Atmel Corporation
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <linux/spinlock.h>
#include <linux/workqueue.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/spi/spi.h>
#include <linux/spi/spi_bitbang.h>
#include "linux/spi/spi_gpio.h" //XXX
#include <asm/gpio.h>


struct spi_gpio {
	struct spi_bitbang bitbang;
	struct spi_gpio_platform_data *info;
	struct platform_device *pdev;
	struct spi_board_info bi;
};


static inline struct spi_gpio *spidev_to_sg(struct spi_device *dev)
{
	return dev->controller_data;
}

static inline void setsck(struct spi_device *dev, int val)
{
	struct spi_gpio *sp = spidev_to_sg(dev);
	gpio_set_value(sp->info->pin_clk, val ? 1 : 0);
}

static inline void setmosi(struct spi_device *dev, int val )
{
	struct spi_gpio *sp = spidev_to_sg(dev);
	gpio_set_value(sp->info->pin_mosi, val ? 1 : 0);
}

static inline u32 getmiso(struct spi_device *dev)
{
	struct spi_gpio *sp = spidev_to_sg(dev);
	return gpio_get_value(sp->info->pin_miso) ? 1 : 0;
}

static inline void do_spidelay(struct spi_device *dev, unsigned nsecs)
{
	struct spi_gpio *sp = spidev_to_sg(dev);

	if (!sp->info->no_spi_delay)
		ndelay(nsecs);
}

#define spidelay(nsecs) do {					\
	/* Steal the spi_device pointer from our caller.	\
	 * The bitbang-API should probably get fixed here... */	\
	do_spidelay(spi, nsecs);				\
  } while (0)

#define EXPAND_BITBANG_TXRX
#include <linux/spi/spi_bitbang.h>

static u32 spi_gpio_txrx_mode0(struct spi_device *spi,
			       unsigned nsecs, u32 word, u8 bits)
{
        return bitbang_txrx_be_cpha0(spi, nsecs, 0, word, bits);
}

static u32 spi_gpio_txrx_mode1(struct spi_device *spi,
			       unsigned nsecs, u32 word, u8 bits)
{
        return bitbang_txrx_be_cpha1(spi, nsecs, 0, word, bits);
}

static u32 spi_gpio_txrx_mode2(struct spi_device *spi,
			       unsigned nsecs, u32 word, u8 bits)
{
        return bitbang_txrx_be_cpha0(spi, nsecs, 1, word, bits);
}

static u32 spi_gpio_txrx_mode3(struct spi_device *spi,
			       unsigned nsecs, u32 word, u8 bits)
{
        return bitbang_txrx_be_cpha1(spi, nsecs, 1, word, bits);
}

static void spi_gpio_chipselect(struct spi_device *dev, int on)
{
	struct spi_gpio *sp = spidev_to_sg(dev);

	if (sp->info->cs_activelow)
		on = !on;
	gpio_set_value(sp->info->pin_cs, on ? 1 : 0);
}

static int spi_gpio_probe(struct platform_device *pdev)
{
	struct spi_master *master;
	struct spi_gpio_platform_data *pdata;
	struct spi_gpio *sp;
	struct spi_device *spidev;
	int err;

	pdata = pdev->dev.platform_data;
	if (!pdata)
		return -ENXIO;

	err = -ENOMEM;
	master = spi_alloc_master(&pdev->dev, sizeof(struct spi_gpio));
	if (!master)
		goto err_alloc_master;

	sp = spi_master_get_devdata(master);
	platform_set_drvdata(pdev, sp);
	sp->info = pdata;

	err = gpio_request(pdata->pin_clk, "spi_clock");
	if (err)
		goto err_request_clk;
	err = gpio_request(pdata->pin_mosi, "spi_mosi");
	if (err)
		goto err_request_mosi;
	err = gpio_request(pdata->pin_miso, "spi_miso");
	if (err)
		goto err_request_miso;
	err = gpio_request(pdata->pin_cs, "spi_cs");
	if (err)
		goto err_request_cs;

	sp->bitbang.master = spi_master_get(master);
	sp->bitbang.master->bus_num = -1;
	sp->bitbang.master->num_chipselect = 1;
	sp->bitbang.chipselect = spi_gpio_chipselect;
	sp->bitbang.txrx_word[SPI_MODE_0] = spi_gpio_txrx_mode0;
	sp->bitbang.txrx_word[SPI_MODE_1] = spi_gpio_txrx_mode1;
	sp->bitbang.txrx_word[SPI_MODE_2] = spi_gpio_txrx_mode2;
	sp->bitbang.txrx_word[SPI_MODE_3] = spi_gpio_txrx_mode3;

	gpio_direction_output(pdata->pin_clk, 0);
	gpio_direction_output(pdata->pin_mosi, 0);
	gpio_direction_output(pdata->pin_cs,
			      pdata->cs_activelow ? 1 : 0);
	gpio_direction_input(pdata->pin_miso);

	err = spi_bitbang_start(&sp->bitbang);
	if (err)
		goto err_no_bitbang;
	err = pdata->boardinfo_setup(&sp->bi, master,
				     pdata->boardinfo_setup_data);
	if (err)
		goto err_bi_setup;
	sp->bi.controller_data = sp;
	spidev = spi_new_device(master, &sp->bi);
	if (!spidev)
		goto err_new_dev;

	return 0;

err_new_dev:
err_bi_setup:
	spi_bitbang_stop(&sp->bitbang);
err_no_bitbang:
	spi_master_put(sp->bitbang.master);
	gpio_free(pdata->pin_cs);
err_request_cs:
	gpio_free(pdata->pin_miso);
err_request_miso:
	gpio_free(pdata->pin_mosi);
err_request_mosi:
	gpio_free(pdata->pin_clk);
err_request_clk:
	kfree(master);

err_alloc_master:
	return err;
}

static int __devexit spi_gpio_remove(struct platform_device *pdev)
{
	struct spi_gpio *sp;
	struct spi_gpio_platform_data *pdata;

	pdata = pdev->dev.platform_data;
	sp = platform_get_drvdata(pdev);

	gpio_free(pdata->pin_clk);
	gpio_free(pdata->pin_mosi);
	gpio_free(pdata->pin_miso);
	gpio_free(pdata->pin_cs);
	spi_bitbang_stop(&sp->bitbang);
	spi_master_put(sp->bitbang.master);

	return 0;
}

static struct platform_driver spi_gpio_driver = {
	.driver		= {
		.name	= "spi-gpio",
		.owner	= THIS_MODULE,
	},
	.probe		= spi_gpio_probe,
	.remove		= __devexit_p(spi_gpio_remove),
};

static int __init spi_gpio_init(void)
{
	int err;

	err = platform_driver_register(&spi_gpio_driver);
	if (err)
		printk(KERN_ERR "spi-gpio: register failed: %d\n", err);

	return err;
}
module_init(spi_gpio_init);

static void __exit spi_gpio_exit(void)
{
	platform_driver_unregister(&spi_gpio_driver);
}
module_exit(spi_gpio_exit);

MODULE_AUTHOR("Piot Skamruk <piotr.skamruk at gmail.com>");
MODULE_AUTHOR("Michael Buesch");
MODULE_DESCRIPTION("Platform independent GPIO bitbangling SPI driver");
MODULE_LICENSE("GPL v2");

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
#if 0
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <linux/device.h>
#include <linux/spinlock.h>
#include <linux/workqueue.h>
#include <linux/platform_device.h>

#include <linux/spi/spi.h>
#include <linux/spi/spi_bitbang.h>
#include <linux/spi/glamo.h>

#include <linux/glamofb.h>

#include <mach/hardware.h>

#include "glamo-core.h"
#include "glamo-regs.h"

struct glamo_spi {
	struct spi_bitbang	bitbang;
	struct spi_master	*master;
	struct glamo_spi_info	*info;
	struct device		*dev;
};

static inline struct glamo_spi *to_gs(struct spi_device *spi)
{
	return spi->controller_data;
}

static int glamo_spi_setupxfer(struct spi_device *spi, struct spi_transfer *t)
{
	unsigned int bpw;

	bpw = t ? t->bits_per_word : spi->bits_per_word;

	if (bpw != 9 && bpw != 8) {
		dev_err(&spi->dev, "invalid bits-per-word (%d)\n", bpw);
		return -EINVAL;
	}

	return 0;
}

static void glamo_spi_chipsel(struct spi_device *spi, int value)
{
#if 0
	struct glamo_spi *gs = to_gs(spi);

	dev_dbg(&spi->dev, "chipsel %d: spi=%p, gs=%p, info=%p, handle=%p\n",
		value, spi, gs, gs->info, gs->info->glamofb_handle);

	glamofb_cmd_mode(gs->info->glamofb_handle, value);
#endif
}

static int glamo_spi_txrx(struct spi_device *spi, struct spi_transfer *t)
{
	struct glamo_spi *gs = to_gs(spi);
	const u_int16_t *ui16 = (const u_int16_t *) t->tx_buf;
	u_int16_t nine_bits;
	int i;

	dev_dbg(&spi->dev, "txrx: tx %p, rx %p, bpw %d, len %d\n",
		t->tx_buf, t->rx_buf, t->bits_per_word, t->len);

	if (spi->bits_per_word == 9)
		nine_bits = (1 << 9);
	else
		nine_bits = 0;

	if (t->len > 3 * sizeof(u_int16_t)) {
		dev_err(&spi->dev, "this driver doesn't support "
			"%u sized xfers\n", t->len);
		return -EINVAL;
	}

	for (i = 0; i < t->len/sizeof(u_int16_t); i++) {
		/* actually transfer the data */
#if 1
		glamofb_cmd_write(gs->info->glamofb_handle,
				  GLAMO_LCD_CMD_TYPE_SERIAL | nine_bits |
				  (1 << 10) | (1 << 11) | (ui16[i] & 0x1ff));
#endif
		/* FIXME: fire ?!? */
		if (i == 0 && (ui16[i] & 0x1ff) == 0x29) {
			dev_dbg(&spi->dev, "leaving command mode\n");
			glamofb_cmd_mode(gs->info->glamofb_handle, 0);
		}
	}

	return t->len;
}

static int glamo_spi_setup(struct spi_device *spi)
{
	int ret;

	if (!spi->bits_per_word)
		spi->bits_per_word = 9;

	/* FIXME: hardware can do this */
	if (spi->mode & SPI_LSB_FIRST)
		return -EINVAL;

	ret = glamo_spi_setupxfer(spi, NULL);
	if (ret < 0) {
		dev_err(&spi->dev, "setupxfer returned %d\n", ret);
		return ret;
	}

	dev_dbg(&spi->dev, "%s: mode %d, %u bpw\n",
		__FUNCTION__, spi->mode, spi->bits_per_word);

	return 0;
}

static int glamo_spi_probe(struct platform_device *pdev)
{
	struct spi_master *master;
	struct glamo_spi *sp;
	int ret;
	int i;

	master = spi_alloc_master(&pdev->dev, sizeof(struct glamo_spi));
	if (master == NULL) {
		dev_err(&pdev->dev, "failed to allocate spi master\n");
		ret = -ENOMEM;
		goto err;
	}

	sp = spi_master_get_devdata(master);
	memset(sp, 0, sizeof(struct glamo_spi));

	sp->master = spi_master_get(master);
	sp->info = pdev->dev.platform_data;
	if (!sp->info) {
		dev_err(&pdev->dev, "can't operate without platform data\n");
		ret = -EIO;
		goto err_no_pdev;
	}
	dev_dbg(&pdev->dev, "sp->info(pdata) = %p\n", sp->info);

	sp->dev = &pdev->dev;

	platform_set_drvdata(pdev, sp);

	sp->bitbang.master = sp->master;
	sp->bitbang.setup_transfer = glamo_spi_setupxfer;
	sp->bitbang.chipselect = glamo_spi_chipsel;
	sp->bitbang.txrx_bufs = glamo_spi_txrx;
	sp->bitbang.master->setup = glamo_spi_setup;

	ret = spi_bitbang_start(&sp->bitbang);
	if (ret)
		goto err_no_bitbang;

	/* register the chips to go with the board */

	glamofb_cmd_mode(sp->info->glamofb_handle, 1);

	for (i = 0; i < sp->info->board_size; i++) {
		dev_info(&pdev->dev, "registering %p: %s\n",
			 &sp->info->board_info[i],
			 sp->info->board_info[i].modalias);

		sp->info->board_info[i].controller_data = sp;
		spi_new_device(master, sp->info->board_info + i);
	}

	return 0;

err_no_bitbang:
	platform_set_drvdata(pdev, NULL);
err_no_pdev:
	spi_master_put(sp->bitbang.master);
err:
	return ret;

}

static int glamo_spi_remove(struct platform_device *pdev)
{
	struct glamo_spi *sp = platform_get_drvdata(pdev);

	spi_bitbang_stop(&sp->bitbang);
	spi_master_put(sp->bitbang.master);

	return 0;
}

#define glamo_spi_suspend NULL
#define glamo_spi_resume NULL

static struct platform_driver glamo_spi_drv = {
	.probe		= glamo_spi_probe,
	.remove		= glamo_spi_remove,
	.suspend	= glamo_spi_suspend,
	.resume		= glamo_spi_resume,
	.driver		= {
		.name	= "glamo-lcm-spi",
		.owner	= THIS_MODULE,
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
#endif

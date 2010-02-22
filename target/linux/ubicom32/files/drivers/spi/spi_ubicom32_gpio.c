/*
 * drivers/spi_spi_ubicom32_gpio.c
 *	Ubicom32 GPIO based SPI driver
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
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <linux/spinlock.h>
#include <linux/workqueue.h>
#include <linux/platform_device.h>

#include <linux/spi/spi.h>
#include <linux/spi/spi_bitbang.h>

#include <linux/gpio.h>

#include <asm/ubicom32-spi-gpio.h>

#define DRIVER_NAME "ubicom32-spi-gpio"

struct ubicom32_spi_gpio {
	struct spi_bitbang			 bitbang;

	struct ubicom32_spi_gpio_platform_data	*pdata;

	struct platform_device			*dev;
};

/*
 * The following 4 functions are used by EXPAND_BITBANG_TXRX to bitbang the data out.
 */
static inline void setsck(struct spi_device *dev, int on)
{
	struct ubicom32_spi_gpio *usg = (struct ubicom32_spi_gpio *)spi_master_get_devdata(dev->master);
	gpio_set_value(usg->pdata->pin_clk, on ? 1 : 0);
}

static inline void setmosi(struct spi_device *dev, int on)
{
	struct ubicom32_spi_gpio *usg = (struct ubicom32_spi_gpio *)spi_master_get_devdata(dev->master);
	gpio_set_value(usg->pdata->pin_mosi, on ? 1 : 0);
}

static inline u32 getmiso(struct spi_device *dev)
{
	struct ubicom32_spi_gpio *usg = (struct ubicom32_spi_gpio *)spi_master_get_devdata(dev->master);
	return gpio_get_value(usg->pdata->pin_miso) ? 1 : 0;
}

#define spidelay(x) ndelay(x)

#define	EXPAND_BITBANG_TXRX
#include <linux/spi/spi_bitbang.h>

/*
 * ubicom32_spi_gpio_txrx_mode0
 */
static u32 ubicom32_spi_gpio_txrx_mode0(struct spi_device *spi, unsigned nsecs, u32 word, u8 bits)
{
	return bitbang_txrx_be_cpha0(spi, nsecs, 0, word, bits);
}

/*
 * ubicom32_spi_gpio_txrx_mode1
 */
static u32 ubicom32_spi_gpio_txrx_mode1(struct spi_device *spi, unsigned nsecs, u32 word, u8 bits)
{
	return bitbang_txrx_be_cpha1(spi, nsecs, 0, word, bits);
}

/*
 * ubicom32_spi_gpio_txrx_mode2
 */
static u32 ubicom32_spi_gpio_txrx_mode2(struct spi_device *spi, unsigned nsecs, u32 word, u8 bits)
{
	return bitbang_txrx_be_cpha0(spi, nsecs, 1, word, bits);
}

/*
 * ubicom32_spi_gpio_txrx_mode3
 */
static u32 ubicom32_spi_gpio_txrx_mode3(struct spi_device *spi, unsigned nsecs, u32 word, u8 bits)
{
	return bitbang_txrx_be_cpha1(spi, nsecs, 1, word, bits);
}

/*
 * ubicom32_spi_gpio_chipselect
 */
static void ubicom32_spi_gpio_chipselect(struct spi_device *dev, int value)
{
	struct ubicom32_spi_gpio_controller_data *cd = (struct ubicom32_spi_gpio_controller_data *)dev->controller_data;
	unsigned int cs_polarity = dev->mode & SPI_CS_HIGH ? 1 : 0;

	if (value == BITBANG_CS_ACTIVE) {
		gpio_set_value(cd->pin_cs, cs_polarity);
		return;
	}
	gpio_set_value(cd->pin_cs, !cs_polarity);
}

/*
 * ubicom32_spi_gpio_probe
 */
static int ubicom32_spi_gpio_probe(struct platform_device *dev)
{
	struct ubicom32_spi_gpio_platform_data *pdata;
	struct spi_master *master;
	struct ubicom32_spi_gpio *usg;
	int ret;

	master = spi_alloc_master(&dev->dev, sizeof(struct ubicom32_spi_gpio));
	if (master == NULL) {
		dev_err(&dev->dev, "failed to allocate spi master\n");
		ret = -ENOMEM;
		goto err;
	}

	usg = (struct ubicom32_spi_gpio *)spi_master_get_devdata(master);

	platform_set_drvdata(dev, usg);

	/*
	 * Copy in the platform data
	 */
	pdata = dev->dev.platform_data;
	usg->pdata = dev->dev.platform_data;

	/*
	 * Request the GPIO lines
	 */
	ret = gpio_request(pdata->pin_mosi, "spi-mosi");
	if (ret) {
		dev_err(&dev->dev, "Failed to allocate spi-mosi GPIO\n");
		goto err;
	}

	ret = gpio_request(pdata->pin_miso, "spi-miso");
	if (ret) {
		dev_err(&dev->dev, "Failed to allocate spi-miso GPIO\n");
		goto err_nomiso;
	}

	ret = gpio_request(pdata->pin_clk, "spi-clk");
	if (ret) {
		dev_err(&dev->dev, "Failed to allocate spi-clk GPIO\n");
		goto err_noclk;
	}

	/*
	 * Setup spi-bitbang adaptor
	 */
	usg->bitbang.flags |= SPI_CS_HIGH;
	usg->bitbang.master = spi_master_get(master);
	usg->bitbang.master->bus_num = pdata->bus_num;
	usg->bitbang.master->num_chipselect = pdata->num_chipselect;
	usg->bitbang.chipselect = ubicom32_spi_gpio_chipselect;

	usg->bitbang.txrx_word[SPI_MODE_0] = ubicom32_spi_gpio_txrx_mode0;
	usg->bitbang.txrx_word[SPI_MODE_1] = ubicom32_spi_gpio_txrx_mode1;
	usg->bitbang.txrx_word[SPI_MODE_2] = ubicom32_spi_gpio_txrx_mode2;
	usg->bitbang.txrx_word[SPI_MODE_3] = ubicom32_spi_gpio_txrx_mode3;

	/*
	 * Setup the GPIO pins
	 */
	gpio_direction_output(pdata->pin_clk, pdata->clk_default);
	gpio_direction_output(pdata->pin_mosi, 0);
	gpio_direction_input(pdata->pin_miso);

	/*
	 * Ready to go
	 */
	ret = spi_bitbang_start(&usg->bitbang);
	if (ret) {
		goto err_no_bitbang;
	}

	return 0;

err_no_bitbang:
	spi_master_put(usg->bitbang.master);

	gpio_free(pdata->pin_clk);

err_noclk:
	gpio_free(pdata->pin_miso);

err_nomiso:
	gpio_free(pdata->pin_mosi);

err:
	return ret;
}

/*
 * ubicom32_spi_gpio_remove
 */
static int ubicom32_spi_gpio_remove(struct platform_device *dev)
{
	struct ubicom32_spi_gpio *sp = platform_get_drvdata(dev);

	spi_bitbang_stop(&sp->bitbang);
	spi_master_put(sp->bitbang.master);

	return 0;
}

/*
 * Work with hotplug and coldplug
 */
MODULE_ALIAS("platform:ubicom32_spi_gpio");

static struct platform_driver ubicom32_spi_gpio_drv = {
	.probe		= ubicom32_spi_gpio_probe,
        .remove		= ubicom32_spi_gpio_remove,
        .driver		= {
		.name	= DRIVER_NAME,
		.owner	= THIS_MODULE,
        },
};

/*
 * ubicom32_spi_gpio_init
 */
static int __init ubicom32_spi_gpio_init(void)
{
        return platform_driver_register(&ubicom32_spi_gpio_drv);
}

/*
 * ubicom32_spi_gpio_exit
 */
static void __exit ubicom32_spi_gpio_exit(void)
{
        platform_driver_unregister(&ubicom32_spi_gpio_drv);
}

module_init(ubicom32_spi_gpio_init);
module_exit(ubicom32_spi_gpio_exit);

MODULE_DESCRIPTION("Ubicom32 SPI-GPIO Driver");
MODULE_AUTHOR("Pat Tjin, <@ubicom.com>");
MODULE_LICENSE("GPL");

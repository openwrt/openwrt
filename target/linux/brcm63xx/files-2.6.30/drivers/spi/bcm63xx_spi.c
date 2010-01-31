/*
 * Broadcom BCM63xx SPI controller support
 *
 * Copyright (C) 2009 Florian Fainelli <florian@openwrt.org>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 */

#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/clk.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/delay.h>
#include <linux/interrupt.h>
#include <linux/spi/spi.h>
#include <linux/spi/spi_bitbang.h>
#include <linux/gpio.h>
#include <linux/completion.h>
#include <linux/err.h>

#include <bcm63xx_io.h>
#include <bcm63xx_regs.h>
#include <bcm63xx_dev_spi.h>

#define PFX 		KBUILD_MODNAME
#define DRV_VER		"0.1.2"

struct bcm63xx_spi {
	/* bitbang has to be first */
        struct spi_bitbang	bitbang;
        struct completion	done;

        void __iomem		*regs;
        int			irq;

	/* Platform data */
        u32			speed_hz;
	unsigned		fifo_size;

	/* Data buffers */
	const unsigned char	*tx_ptr;
	unsigned char		*rx_ptr;
	int			remaining_bytes;

	struct clk		*clk;
	struct resource		*ioarea;
	struct platform_device	*pdev;
};

static void bcm63xx_spi_chipselect(struct spi_device *spi, int is_on)
{
	struct bcm63xx_spi *bs = spi_master_get_devdata(spi->master);
	u16 val;

	val = bcm_spi_readw(bs->regs, SPI_CMD);
	if (is_on == BITBANG_CS_INACTIVE)
		val |= SPI_CMD_NOOP;
	else if (is_on == BITBANG_CS_ACTIVE)
		val |= (1 << spi->chip_select << SPI_CMD_DEVICE_ID_SHIFT);
		
	bcm_spi_writew(val, bs->regs, SPI_CMD);
}

static int bcm63xx_spi_setup_transfer(struct spi_device *spi,
					struct spi_transfer *t)
{
	u8 bits_per_word;
	u8 clk_cfg;
	u32 hz;
	unsigned int div;

	struct bcm63xx_spi *bs = spi_master_get_devdata(spi->master);

	bits_per_word = (t) ? t->bits_per_word : spi->bits_per_word;
	hz = (t) ? t->speed_hz : spi->max_speed_hz;
	if (bits_per_word != 8) {
		dev_err(&spi->dev, "%s, unsupported bits_per_word=%d\n",
			__func__, bits_per_word);
		return -EINVAL;
        }

	if (spi->chip_select > spi->master->num_chipselect) {
		dev_err(&spi->dev, "%s, unsupported slave %d\n",
			__func__, spi->chip_select);
		return -EINVAL;
	}

	/* Check clock setting */
	div = (bs->speed_hz / hz);
	switch (div) {
	case 2:
		clk_cfg = SPI_CLK_25MHZ;
		break;
	case 4:
		clk_cfg = SPI_CLK_12_50MHZ;
		break;
	case 8:
		clk_cfg = SPI_CLK_6_250MHZ;
		break;
	case 16:
		clk_cfg = SPI_CLK_3_125MHZ;
		break;
	case 32:
		clk_cfg = SPI_CLK_1_563MHZ;
		break;
	case 128:
		clk_cfg = SPI_CLK_0_781MHZ;
		break;
	case 64:
	default:
		/* Set to slowest mode for compatibility */
		clk_cfg = SPI_CLK_0_781MHZ;
		break;
	}

	bcm_spi_writeb(clk_cfg, bs->regs, SPI_CLK_CFG);
	dev_dbg(&spi->dev, "Setting clock register to %d (hz %d, cmd %02x)\n",
								div, hz, clk_cfg);
	
	return 0;
}

/* the spi->mode bits understood by this driver: */
#define MODEBITS (SPI_CPOL | SPI_CPHA)

static int bcm63xx_spi_setup(struct spi_device *spi)
{
	struct spi_bitbang *bitbang;
	struct bcm63xx_spi *bs;
	int retval;

	bs = spi_master_get_devdata(spi->master);
	bitbang = &bs->bitbang;

	if (!spi->bits_per_word)
		spi->bits_per_word = 8;

	if (spi->mode & ~MODEBITS) {
		dev_err(&spi->dev, "%s, unsupported mode bits %x\n",
			__func__, spi->mode & ~MODEBITS);
		return -EINVAL;
	}

	retval = bcm63xx_spi_setup_transfer(spi, NULL);
	if (retval < 0) {
		dev_err(&spi->dev, "setup: unsupported mode bits %x\n",
			spi->mode & ~MODEBITS);
		return retval;
	}

	dev_dbg(&spi->dev, "%s, mode %d, %u bits/w, %u nsec/bit\n",
		__func__, spi->mode & MODEBITS, spi->bits_per_word, 0);

	return 0;
}

/* Fill the TX FIFO with as many bytes as possible */
static void bcm63xx_spi_fill_tx_fifo(struct bcm63xx_spi *bs)
{
        u8 tail;

        /* Fill the Tx FIFO with as many bytes as possible */
	tail = bcm_spi_readb(bs->regs, SPI_MSG_TAIL);
        while ((tail < bs->fifo_size) && (bs->remaining_bytes > 0)) {
                if (bs->tx_ptr)
                        bcm_spi_writeb(*bs->tx_ptr++, bs->regs, SPI_MSG_DATA);
		else
			bcm_spi_writeb(0, bs->regs, SPI_MSG_DATA); 
                bs->remaining_bytes--;
		tail = bcm_spi_readb(bs->regs, SPI_MSG_TAIL);
        }
}

static int bcm63xx_txrx_bufs(struct spi_device *spi, struct spi_transfer *t)
{
	struct bcm63xx_spi *bs = spi_master_get_devdata(spi->master);
	u8 msg_ctl;
	u16 cmd;

	dev_dbg(&spi->dev, "txrx: tx %p, rx %p, len %d\n",
				t->tx_buf, t->rx_buf, t->len);

	/* Transmitter is inhibited */
	bs->tx_ptr = t->tx_buf;
	bs->rx_ptr = t->rx_buf;
	bs->remaining_bytes = t->len;
	init_completion(&bs->done);

	bcm63xx_spi_fill_tx_fifo(bs);

	/* Enable the command done interrupt which
	 * we use to determine completion of a command */
	bcm_spi_writeb(SPI_INTR_CMD_DONE, bs->regs, SPI_INT_MASK);
	
	/* Fill in the Message control register */
	msg_ctl = bcm_spi_readb(bs->regs, SPI_MSG_CTL);
	msg_ctl |= (t->len << SPI_BYTE_CNT_SHIFT);
	msg_ctl |= (SPI_FD_RW << SPI_MSG_TYPE_SHIFT);
	bcm_spi_writeb(msg_ctl, bs->regs, SPI_MSG_CTL);
	
	/* Issue the transfer */
	cmd = bcm_spi_readb(bs->regs, SPI_CMD);
	cmd |= SPI_CMD_START_IMMEDIATE;
	cmd |= (0 << SPI_CMD_PREPEND_BYTE_CNT_SHIFT);
	bcm_spi_writeb(cmd, bs->regs, SPI_CMD);

	wait_for_completion(&bs->done);	

	/* Disable the CMD_DONE interrupt */
	bcm_spi_writeb(~(SPI_INTR_CMD_DONE), bs->regs, SPI_INT_MASK);

	return t->len - bs->remaining_bytes;
}

/* This driver supports single master mode only. Hence 
 * CMD_DONE is the only interrupt we care about
 */
static irqreturn_t bcm63xx_spi_interrupt(int irq, void *dev_id)
{
	struct spi_master *master = (struct spi_master *)dev_id;
	struct bcm63xx_spi *bs = spi_master_get_devdata(master);
	u8 intr;
	u16 cmd;

	/* Read interupts and clear them immediately */
	intr = bcm_spi_readb(bs->regs, SPI_INT_STATUS);
	bcm_spi_writeb(SPI_INTR_CLEAR_ALL, bs->regs, SPI_INT_MASK);

	/* A tansfer completed */
	if (intr & SPI_INTR_CMD_DONE) {
		u8 rx_empty;
	
		rx_empty = bcm_spi_readb(bs->regs, SPI_ST);
		/* Read out all the data */
		while ((rx_empty & SPI_RX_EMPTY) == 0) {
			u8 data;
		
			data = bcm_spi_readb(bs->regs, SPI_RX_DATA);
			if (bs->rx_ptr)
				*bs->rx_ptr++ = data;

			rx_empty = bcm_spi_readb(bs->regs, SPI_RX_EMPTY);
		}

		/* See if there is more data to send */
		if (bs->remaining_bytes > 0) {
			bcm63xx_spi_fill_tx_fifo(bs);

			/* Start the transfer */
			cmd = bcm_spi_readb(bs->regs, SPI_CMD);
			cmd |= SPI_CMD_START_IMMEDIATE;
			cmd |= (0 << SPI_CMD_PREPEND_BYTE_CNT_SHIFT);
			bcm_spi_writeb(cmd, bs->regs, SPI_CMD);
		} else
			complete(&bs->done);
	}

	return IRQ_HANDLED;
}


static int __init bcm63xx_spi_probe(struct platform_device *pdev)
{
	struct resource *r;
	struct bcm63xx_spi_pdata *pdata = pdev->dev.platform_data;
	int irq;
	struct spi_master *master;
	struct clk *clk;
	struct bcm63xx_spi *bs;
	int ret;

	r = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if (!r) {
		ret = -ENXIO;
		goto out;
	}

	irq = platform_get_irq(pdev, 0);
	if (irq < 0) {
		ret = -ENXIO;
		goto out;
	}

	clk = clk_get(&pdev->dev, "spi");
	if (IS_ERR(clk)) {
		dev_err(&pdev->dev, "No clock for device\n");
		ret = -ENODEV;
		goto out;
	}
	
	master = spi_alloc_master(&pdev->dev, sizeof(struct bcm63xx_spi));
	if (!master) {
		ret = -ENOMEM;
		goto out_free;
	}

	bs = spi_master_get_devdata(master);
	bs->bitbang.master = spi_master_get(master);
	bs->bitbang.chipselect = bcm63xx_spi_chipselect;
	bs->bitbang.setup_transfer = bcm63xx_spi_setup_transfer;
	bs->bitbang.txrx_bufs = bcm63xx_txrx_bufs;
	bs->bitbang.master->setup = bcm63xx_spi_setup;
	init_completion(&bs->done);
	
	platform_set_drvdata(pdev, master);
        bs->pdev = pdev;

	if (!request_mem_region(r->start,
			r->end - r->start, PFX)) {
		ret = -ENXIO;
		goto out_free;
	}

        bs->regs = ioremap_nocache(r->start, r->end - r->start);
	if (!bs->regs) {
		printk(KERN_ERR PFX " unable to ioremap regs\n");
		ret = -ENOMEM;
		goto out_free;
	}
	bs->irq = irq;
	bs->clk = clk;
	bs->fifo_size = pdata->fifo_size;

	ret = request_irq(irq, bcm63xx_spi_interrupt, 0,
				pdev->name, master);
	if (ret) {
		printk(KERN_ERR PFX " unable to request irq\n");
		goto out_unmap;
	}

	master->bus_num = pdata->bus_num;
	master->num_chipselect = pdata->num_chipselect;
	bs->speed_hz = pdata->speed_hz;
	
	/* Initialize hardware */
	clk_enable(bs->clk);
	bcm_spi_writeb(SPI_INTR_CLEAR_ALL, bs->regs, SPI_INT_MASK);
	
	dev_info(&pdev->dev, " at 0x%08x (irq %d, FIFOs size %d) v%s\n",
				r->start, irq, bs->fifo_size, DRV_VER);

	ret = spi_bitbang_start(&bs->bitbang);
	if (ret) {
		dev_err(&pdev->dev, "spi_bitbang_start FAILED\n");
		goto out_reset_hw;
	}

	return ret;

out_reset_hw:
	clk_disable(clk);
	free_irq(irq, master);
out_unmap:
	iounmap(bs->regs);
out_free:
	clk_put(clk);
	spi_master_put(master);
out:
	return ret;
}

static int __exit bcm63xx_spi_remove(struct platform_device *pdev)
{
	struct spi_master	*master = platform_get_drvdata(pdev);
	struct bcm63xx_spi	*bs = spi_master_get_devdata(master);

	spi_bitbang_stop(&bs->bitbang);
	clk_disable(bs->clk);
	clk_put(bs->clk);
	free_irq(bs->irq, master);
	iounmap(bs->regs);
	platform_set_drvdata(pdev, 0);
	spi_master_put(bs->bitbang.master);

	return 0;
}

#ifdef CONFIG_PM
static int bcm63xx_spi_suspend(struct platform_device *pdev, pm_message_t mesg)
{
	struct spi_master	*master = platform_get_drvdata(pdev);
	struct bcm63xx_spi	*bs = spi_master_get_devdata(master);

        clk_disable(bs->clk);
        
	return 0;
}

static int bcm63xx_spi_resume(struct platform_device *pdev)
{
	struct bcm63xx_spi	*bs = spi_master_get_devdata(master);
	struct bcm63xx_spi	*bs = spi_master_get_devdata(master);

	clk_enable(bs->clk);

	return 0;
}
#else
#define bcm63xx_spi_suspend	NULL
#define bcm63xx_spi_resume	NULL
#endif

static struct platform_driver bcm63xx_spi_driver = {
	.driver = {
		.name	= "bcm63xx-spi",
		.owner	= THIS_MODULE,
	},
	.probe		= bcm63xx_spi_probe,
	.remove		= bcm63xx_spi_remove,
	.suspend	= bcm63xx_spi_suspend,
	.resume		= bcm63xx_spi_resume,
};


static int __init bcm63xx_spi_init(void)
{
	return platform_driver_register(&bcm63xx_spi_driver);
}

static void __exit bcm63xx_spi_exit(void)
{
	platform_driver_unregister(&bcm63xx_spi_driver);
}

module_init(bcm63xx_spi_init);
module_exit(bcm63xx_spi_exit);

MODULE_ALIAS("platform:bcm63xx_spi");
MODULE_AUTHOR("Florian Fainelli <florian@openwrt.org>");
MODULE_DESCRIPTION("Broadcom BCM63xx SPI Controller driver");
MODULE_LICENSE("GPL");
MODULE_VERSION(DRV_VER);

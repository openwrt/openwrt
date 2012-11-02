/************************************************************************
 *
 * Copyright (c) 2008
 * Infineon Technologies AG
 * St. Martin Strasse 53; 81669 Muenchen; Germany
 *
 * Inspired by Atmel AT32/AT91 SPI Controller driver
 * Copyright (c) 2006 Atmel Corporation
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 *
 ************************************************************************/
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/delay.h>
#include <linux/interrupt.h>
#include <linux/slab.h>
#include <linux/platform_device.h>
#include <linux/spi/spi.h>

#include <asm/io.h>

#include <status_reg.h>
#include <base_reg.h>
#include <ssc_reg.h>
#include <sys0_reg.h>
#include <sys1_reg.h>

#define SFRAME_SIZE 512 /* bytes */
#define FIFO_HEADROOM 2 /* words */

#define SVIP_SSC_RFIFO_WORDS    8

enum svip_ssc_dir {
	SSC_RXTX,
	SSC_RX,
	SSC_TX,
	SSC_UNDEF
};

/*
 * The core SPI transfer engine just talks to a register bank to set up
 * DMA transfers; transfer queue progress is driven by IRQs.  The clock
 * framework provides the base clock, subdivided for each spi_device.
 */
struct svip_ssc_device {
	struct svip_reg_ssc *regs;
	enum svip_ssc_dir bus_dir;
	struct spi_device *stay;

	u8 stopping;
	struct list_head queue;
	struct spi_transfer *current_transfer;
	int remaining_bytes;
	int rx_bytes;
	int tx_bytes;

	char intname[4][16];

	spinlock_t lock;
};

static int svip_ssc_setup(struct spi_device *spi);

extern unsigned int ltq_get_fbs0_hz(void);

static void cs_activate(struct svip_ssc_device *ssc_dev, struct spi_device *spi)
{
	ssc_dev->regs->whbgpostat = 0x0001 << spi->chip_select; /* activate the chip select */
}

static void cs_deactivate(struct svip_ssc_device *ssc_dev, struct spi_device *spi)
{
	ssc_dev->regs->whbgpostat = 0x0100 << spi->chip_select; /* deactivate the chip select */
}

/*
 * "Normally" returns Byte Valid = 4.
 * If the unaligned remainder of the packet is 3 bytes, these have to be
 * transferred as a combination of a 16-bit and a 8-bit FPI transfer. For
 * 2 or 1 remaining bytes a single 16-bit or 8-bit transfer will do.
 */
static int inline _estimate_bv(int byte_pos, int bytelen)
{
	int remainder = bytelen % 4;

	if (byte_pos < (bytelen - remainder))
		return 4;

	if (remainder == 3)
	{
		if (byte_pos == (bytelen - remainder))
			return 2;
		else
			return 1;
	}
	return remainder;
}

/*
 * Submit next transfer.
 * lock is held, spi irq is blocked
 */
static void svip_ssc_next_xfer(struct spi_master *master,
			       struct spi_message *msg)
{
	struct svip_ssc_device *ssc_dev  = spi_master_get_devdata(master);
	struct spi_transfer   *xfer;
	unsigned char         *buf_ptr;

	xfer = ssc_dev->current_transfer;
	if (!xfer || ssc_dev->remaining_bytes == 0) {
		if (xfer)
			xfer = list_entry(xfer->transfer_list.next,
					  struct spi_transfer, transfer_list);
		else
			xfer = list_entry(msg->transfers.next,
					  struct spi_transfer, transfer_list);
		ssc_dev->remaining_bytes = xfer->len;
		ssc_dev->rx_bytes = 0;
		ssc_dev->tx_bytes = 0;
		ssc_dev->current_transfer = xfer;
		ssc_dev->regs->sfcon = 0; /* reset Serial Framing */

		/* enable and flush RX/TX FIFO */
		ssc_dev->regs->rxfcon =
			SSC_RXFCON_RXFITL_VAL(SVIP_SSC_RFIFO_WORDS-FIFO_HEADROOM) |
			SSC_RXFCON_RXFLU | /* Receive FIFO Flush */
			SSC_RXFCON_RXFEN;  /* Receive FIFO Enable */

		ssc_dev->regs->txfcon =
			SSC_TXFCON_TXFITL_VAL(FIFO_HEADROOM) |
			SSC_TXFCON_TXFLU | /* Transmit FIFO Flush */
			SSC_TXFCON_TXFEN;  /* Transmit FIFO Enable */

		asm("sync");

		/* select mode RXTX, RX or TX */
		if (xfer->rx_buf && xfer->tx_buf) /* RX and TX */
		{
			if (ssc_dev->bus_dir != SSC_RXTX)
			{
				ssc_dev->regs->mcon &= ~(SSC_MCON_RXOFF | SSC_MCON_TXOFF);
				ssc_dev->bus_dir = SSC_RXTX;
				ssc_dev->regs->irnen = SSC_IRNEN_T | SSC_IRNEN_F | SSC_IRNEN_E;
			}
			ssc_dev->regs->sfcon =
				SSC_SFCON_PLEN_VAL(0) |
				SSC_SFCON_DLEN_VAL(((xfer->len-1)%SFRAME_SIZE)*8+7) |
				SSC_SFCON_STOP |
				SSC_SFCON_ICLK_VAL(2) |
				SSC_SFCON_IDAT_VAL(2) |
				SSC_SFCON_IAEN |
				SSC_SFCON_SFEN;

		}
		else if (xfer->rx_buf) /* RX only */
		{
			if (ssc_dev->bus_dir != SSC_RX)
			{
				ssc_dev->regs->mcon =
					(ssc_dev->regs->mcon | SSC_MCON_TXOFF) & ~SSC_MCON_RXOFF;

				ssc_dev->bus_dir = SSC_RX;

				ssc_dev->regs->irnen = SSC_IRNEN_R | SSC_IRNEN_E;
			}
			/* Initiate clock generation for Rx-Only Transfer. In case of RX-only transfer,
			 * rx_bytes represents the number of already requested bytes.
			 */
			ssc_dev->rx_bytes = min(xfer->len, (unsigned)(SVIP_SSC_RFIFO_WORDS*4));
			ssc_dev->regs->rxreq = ssc_dev->rx_bytes;
		}
		else /* TX only */
		{
			if (ssc_dev->bus_dir != SSC_TX)
			{
				ssc_dev->regs->mcon =
					(ssc_dev->regs->mcon | SSC_MCON_RXOFF) & ~SSC_MCON_TXOFF;

				ssc_dev->bus_dir = SSC_TX;

				ssc_dev->regs->irnen =
					SSC_IRNEN_T | SSC_IRNEN_F | SSC_IRNEN_E;
			}
			ssc_dev->regs->sfcon =
				SSC_SFCON_PLEN_VAL(0) |
				SSC_SFCON_DLEN_VAL(((xfer->len-1)%SFRAME_SIZE)*8+7) |
				SSC_SFCON_STOP |
				SSC_SFCON_ICLK_VAL(2) |
				SSC_SFCON_IDAT_VAL(2) |
				SSC_SFCON_IAEN |
				SSC_SFCON_SFEN;
		}
	}

	if (xfer->tx_buf)
	{
		int outstanding;
		int i;
		int fstat = ssc_dev->regs->fstat;
		int txffl = SSC_FSTAT_TXFFL_GET(fstat);
		int rxffl = SSC_FSTAT_RXFFL_GET(fstat);

		outstanding = txffl;

		if (xfer->rx_buf)
		{
			outstanding += rxffl;
			if (SSC_STATE_BSY_GET(ssc_dev->regs->state))
				outstanding++;

			while (rxffl) /* is 0 in TX-Only mode */
			{
				unsigned int rb;
				int rxbv = _estimate_bv(ssc_dev->rx_bytes, xfer->len);
				rb = ssc_dev->regs->rb;
				for (i=0; i<rxbv; i++)
				{
					((unsigned char*)xfer->rx_buf)[ssc_dev->rx_bytes] =
						(rb >> ((rxbv-i-1)*8)) & 0xFF;

					ssc_dev->rx_bytes++;
				}
				rxffl--;
				outstanding--;
			}
			ssc_dev->remaining_bytes = xfer->len - ssc_dev->rx_bytes;
		}

		/* for last Tx cycle set TxFifo threshold to 0 */
		if ((xfer->len - ssc_dev->tx_bytes) <=
		    (4*(SVIP_SSC_RFIFO_WORDS-1-outstanding)))
		{
			ssc_dev->regs->txfcon = SSC_TXFCON_TXFITL_VAL(0) |
				SSC_TXFCON_TXFEN;
		}

		while ((ssc_dev->tx_bytes < xfer->len) &&
		       (outstanding < (SVIP_SSC_RFIFO_WORDS-1)))
		{
			unsigned int tb = 0;
			int txbv = _estimate_bv(ssc_dev->tx_bytes, xfer->len);

			for (i=0; i<txbv; i++)
			{
				tb |= ((unsigned char*)xfer->tx_buf)[ssc_dev->tx_bytes] <<
					((txbv-i-1)*8);

				ssc_dev->tx_bytes++;
			}
			switch(txbv)
			{
#ifdef __BIG_ENDIAN
			case 1:
				*((unsigned char *)(&(ssc_dev->regs->tb))+3) = tb & 0xFF;
				break;
			case 2:
				*((unsigned short *)(&(ssc_dev->regs->tb))+1) = tb & 0xFFFF;
				break;
#else /* __LITTLE_ENDIAN */
			case 1:
				*((unsigned char *)(&(ssc_dev->regs->tb))) = tb & 0xFF;
				break;
			case 2:
				*((unsigned short *)(&(ssc_dev->regs->tb))) = tb & 0xFFFF;
				break;
#endif
			default:
				ssc_dev->regs->tb = tb;
			}
			outstanding++;
		}
	}
	else /* xfer->tx_buf == NULL -> RX only! */
	{
		int j;
		int rxffl = SSC_FSTAT_RXFFL_GET(ssc_dev->regs->fstat);
		int rxbv = 0;
		unsigned int rbuf;

		buf_ptr = (unsigned char*)xfer->rx_buf +
			(xfer->len - ssc_dev->remaining_bytes);

		for (j = 0; j < rxffl; j++)
		{
			rxbv = SSC_STATE_RXBV_GET(ssc_dev->regs->state);
			rbuf = ssc_dev->regs->rb;

			if (rxbv == 4)
			{
				*((unsigned int*)buf_ptr+j) = ntohl(rbuf);
			}
			else
			{
				int b;
#ifdef __BIG_ENDIAN
				for (b = 0; b < rxbv; b++)
				{
					buf_ptr[4*j+b] = ((unsigned char*)(&rbuf))[4-rxbv+b];
				}
#else /* __LITTLE_ENDIAN */
				for (b = 0; b < rxbv; b++)
				{
					buf_ptr[4*j+b] = ((unsigned char*)(&rbuf))[rxbv-1-b];
				}
#endif
			}
			ssc_dev->remaining_bytes -= rxbv;
		}
		if ((ssc_dev->rx_bytes < xfer->len) &&
		    !SSC_STATE_BSY_GET(ssc_dev->regs->state))
		{
			int rxreq = min(xfer->len - ssc_dev->rx_bytes,
					(unsigned)(SVIP_SSC_RFIFO_WORDS*4));

			ssc_dev->rx_bytes += rxreq;
			ssc_dev->regs->rxreq = rxreq;
		}

		if (ssc_dev->remaining_bytes < 0)
		{
			printk("ssc_dev->remaining_bytes = %d! xfer->len = %d, "
			       "rxffl=%d, rxbv=%d\n", ssc_dev->remaining_bytes, xfer->len,
			       rxffl, rxbv);

			ssc_dev->remaining_bytes = 0;
		}
	}
}

/*
 * Submit next message.
 * lock is held
 */
static void svip_ssc_next_message(struct spi_master *master)
{
	struct svip_ssc_device *ssc_dev  = spi_master_get_devdata(master);
	struct spi_message    *msg;
	struct spi_device     *spi;

	BUG_ON(ssc_dev->current_transfer);

	msg = list_entry(ssc_dev->queue.next, struct spi_message, queue);
	spi = msg->spi;

	dev_dbg(master->dev.parent, "start message %p on %p\n", msg, spi);

	/* select chip if it's not still active */
	if (ssc_dev->stay) {
		if (ssc_dev->stay != spi) {
			cs_deactivate(ssc_dev, ssc_dev->stay);
			svip_ssc_setup(spi);
			cs_activate(ssc_dev, spi);
		}
		ssc_dev->stay = NULL;
	}
	else {
		svip_ssc_setup(spi);
		cs_activate(ssc_dev, spi);
	}

	svip_ssc_next_xfer(master, msg);
}

/*
 * Report message completion.
 * lock is held
 */
static void
svip_ssc_msg_done(struct spi_master *master, struct svip_ssc_device *ssc_dev,
		  struct spi_message *msg, int status, int stay)
{
	if (!stay || status < 0)
		cs_deactivate(ssc_dev, msg->spi);
	else
		ssc_dev->stay = msg->spi;

	list_del(&msg->queue);
	msg->status = status;

	dev_dbg(master->dev.parent,
		"xfer complete: %u bytes transferred\n",
		msg->actual_length);

	spin_unlock(&ssc_dev->lock);
	msg->complete(msg->context);
	spin_lock(&ssc_dev->lock);

	ssc_dev->current_transfer = NULL;

	/* continue if needed */
	if (list_empty(&ssc_dev->queue) || ssc_dev->stopping)
		; /* TODO: disable hardware */
	else
		svip_ssc_next_message(master);
}

static irqreturn_t svip_ssc_eir_handler(int irq, void *dev_id)
{
	struct platform_device *pdev     = (struct platform_device*)dev_id;
	struct spi_master      *master   = platform_get_drvdata(pdev);
	struct svip_ssc_device  *ssc_dev  = spi_master_get_devdata(master);

	dev_err (&pdev->dev, "ERROR: errirq. STATE = 0x%0lx\n",
		 ssc_dev->regs->state);
	return IRQ_HANDLED;
}

static irqreturn_t svip_ssc_rir_handler(int irq, void *dev_id)
{
	struct platform_device *pdev     = (struct platform_device*)dev_id;
	struct spi_master      *master   = platform_get_drvdata(pdev);
	struct svip_ssc_device  *ssc_dev  = spi_master_get_devdata(master);
	struct spi_message     *msg;
	struct spi_transfer    *xfer;

	xfer = ssc_dev->current_transfer;
	msg = list_entry(ssc_dev->queue.next, struct spi_message, queue);

	/* Tx and Rx Interrupts are fairly unpredictable. Just leave interrupt
	 * handler for spurious Interrupts!
	 */
	if (!xfer) {
		dev_dbg(master->dev.parent,
			"%s(%d): xfer = NULL\n", __FUNCTION__, irq);
		goto out;
	}
	if ( !(xfer->rx_buf) ) {
		dev_dbg(master->dev.parent,
			"%s(%d): xfer->rx_buf = NULL\n", __FUNCTION__, irq);
		goto out;
	}
	if (ssc_dev->remaining_bytes > 0)
	{
		/*
		 * Keep going, we still have data to send in
		 * the current transfer.
		 */
		svip_ssc_next_xfer(master, msg);
	}

	if (ssc_dev->remaining_bytes == 0)
	{
		msg->actual_length += xfer->len;

		if (msg->transfers.prev == &xfer->transfer_list) {
			/* report completed message */
			svip_ssc_msg_done(master, ssc_dev, msg, 0,
					  xfer->cs_change);
		}
		else {
			if (xfer->cs_change) {
				cs_deactivate(ssc_dev, msg->spi);
				udelay(1); /* not nice in interrupt context */
				cs_activate(ssc_dev, msg->spi);
			}

			/* Not done yet. Submit the next transfer. */
			svip_ssc_next_xfer(master, msg);
		}
	}
out:
	return IRQ_HANDLED;
}

static irqreturn_t svip_ssc_tir_handler(int irq, void *dev_id)
{
	struct platform_device *pdev     = (struct platform_device*)dev_id;
	struct spi_master      *master   = platform_get_drvdata(pdev);
	struct svip_ssc_device  *ssc_dev  = spi_master_get_devdata(master);
	struct spi_message     *msg;
	struct spi_transfer    *xfer;
	int tx_remain;

	xfer = ssc_dev->current_transfer;
	msg = list_entry(ssc_dev->queue.next, struct spi_message, queue);

	/* Tx and Rx Interrupts are fairly unpredictable. Just leave interrupt
	 * handler for spurious Interrupts!
	 */
	if (!xfer) {
		dev_dbg(master->dev.parent,
			"%s(%d): xfer = NULL\n", __FUNCTION__, irq);
		goto out;
	}
	if ( !(xfer->tx_buf) ) {
		dev_dbg(master->dev.parent,
			"%s(%d): xfer->tx_buf = NULL\n", __FUNCTION__, irq);
		goto out;
	}

	if (ssc_dev->remaining_bytes > 0)
	{
		tx_remain = xfer->len - ssc_dev->tx_bytes;
		if ( tx_remain == 0 )
		{
			dev_dbg(master->dev.parent,
				"%s(%d): tx_remain = 0\n", __FUNCTION__, irq);
		}
		else
			/*
			 * Keep going, we still have data to send in
			 * the current transfer.
			 */
			svip_ssc_next_xfer(master, msg);
	}
out:
	return IRQ_HANDLED;
}

static irqreturn_t svip_ssc_fir_handler(int irq, void *dev_id)
{
	struct platform_device *pdev     = (struct platform_device*)dev_id;
	struct spi_master      *master   = platform_get_drvdata(pdev);
	struct svip_ssc_device  *ssc_dev  = spi_master_get_devdata(master);
	struct spi_message     *msg;
	struct spi_transfer    *xfer;

	xfer = ssc_dev->current_transfer;
	msg = list_entry(ssc_dev->queue.next, struct spi_message, queue);

	/* Tx and Rx Interrupts are fairly unpredictable. Just leave interrupt
	 * handler for spurious Interrupts!
	 */
	if (!xfer) {
		dev_dbg(master->dev.parent,
			"%s(%d): xfer = NULL\n", __FUNCTION__, irq);
		goto out;
	}
	if ( !(xfer->tx_buf) ) {
		dev_dbg(master->dev.parent,
			"%s(%d): xfer->tx_buf = NULL\n", __FUNCTION__, irq);
		goto out;
	}

	if (ssc_dev->remaining_bytes > 0)
	{
		int tx_remain = xfer->len - ssc_dev->tx_bytes;

		if (tx_remain == 0)
		{
			/* Frame interrupt gets raised _before_ last Rx interrupt */
			if (xfer->rx_buf)
			{
				svip_ssc_next_xfer(master, msg);
				if (ssc_dev->remaining_bytes)
					printk("expected RXTX transfer to be complete!\n");
			}
			ssc_dev->remaining_bytes = 0;
		}
		else
		{
			ssc_dev->regs->sfcon = SSC_SFCON_PLEN_VAL(0) |
				SSC_SFCON_DLEN_VAL(SFRAME_SIZE*8-1) |
				SSC_SFCON_STOP |
				SSC_SFCON_ICLK_VAL(2) |
				SSC_SFCON_IDAT_VAL(2) |
				SSC_SFCON_IAEN |
				SSC_SFCON_SFEN;
		}
	}

	if (ssc_dev->remaining_bytes == 0)
	{
		msg->actual_length += xfer->len;

		if (msg->transfers.prev == &xfer->transfer_list) {
			/* report completed message */
			svip_ssc_msg_done(master, ssc_dev, msg, 0,
					  xfer->cs_change);
		}
		else {
			if (xfer->cs_change) {
				cs_deactivate(ssc_dev, msg->spi);
				udelay(1); /* not nice in interrupt context */
				cs_activate(ssc_dev, msg->spi);
			}

			/* Not done yet. Submit the next transfer. */
			svip_ssc_next_xfer(master, msg);
		}
	}

out:
	return IRQ_HANDLED;
}

/* the spi->mode bits understood by this driver: */
#define MODEBITS (SPI_CPOL | SPI_CPHA | SPI_LSB_FIRST | SPI_LOOP)

static int svip_ssc_setup(struct spi_device *spi)
{
	struct spi_master       *master = spi->master;
	struct svip_ssc_device   *ssc_dev = spi_master_get_devdata(master);
	unsigned int            bits = spi->bits_per_word;
	unsigned int            br, sck_hz = spi->max_speed_hz;
	unsigned long           flags;

	if (ssc_dev->stopping)
		return -ESHUTDOWN;

	if (spi->chip_select >= master->num_chipselect) {
		dev_dbg(&spi->dev,
			"setup: invalid chipselect %u (%u defined)\n",
			spi->chip_select, master->num_chipselect);
		return -EINVAL;
	}

	if (bits == 0)
		bits = 8;
	if (bits != 8) {
		dev_dbg(&spi->dev,
			"setup: invalid bits_per_word %u (expect 8)\n",
			bits);
		return -EINVAL;
	}

	if (spi->mode & ~MODEBITS) {
		dev_dbg(&spi->dev, "setup: unsupported mode bits %x\n",
			spi->mode & ~MODEBITS);
		return -EINVAL;
	}

	/* Disable SSC */
	ssc_dev->regs->whbstate = SSC_WHBSTATE_CLREN;

	if (sck_hz == 0)
		sck_hz = 10000;

	br = ltq_get_fbs0_hz()/(2 *sck_hz);
	if (ltq_get_fbs0_hz()%(2 *sck_hz) == 0)
		br = br -1;
	ssc_dev->regs->br = br;

	/* set Control Register */
	ssc_dev->regs->mcon = SSC_MCON_ENBV |
		SSC_MCON_RUEN |
		SSC_MCON_TUEN |
		SSC_MCON_AEN |
		SSC_MCON_REN |
		SSC_MCON_TEN |
		(spi->mode & SPI_CPOL ? SSC_MCON_PO : 0) |      /* Clock Polarity */
		(spi->mode & SPI_CPHA ? 0 : SSC_MCON_PH) |      /* Tx on trailing edge */
		(spi->mode & SPI_LOOP ? SSC_MCON_LB : 0) |      /* Loopback */
		(spi->mode & SPI_LSB_FIRST ? 0 : SSC_MCON_HB);  /* MSB first */
	ssc_dev->bus_dir = SSC_UNDEF;

	/* Enable SSC */
	ssc_dev->regs->whbstate = SSC_WHBSTATE_SETEN;
	asm("sync");

	spin_lock_irqsave(&ssc_dev->lock, flags);
	if (ssc_dev->stay == spi)
		ssc_dev->stay = NULL;
	cs_deactivate(ssc_dev, spi);
	spin_unlock_irqrestore(&ssc_dev->lock, flags);

	dev_dbg(&spi->dev,
		"setup: %u Hz bpw %u mode 0x%02x cs %u\n",
		sck_hz, bits, spi->mode, spi->chip_select);

	return 0;
}

static int svip_ssc_transfer(struct spi_device *spi, struct spi_message *msg)
{
	struct spi_master       *master = spi->master;
	struct svip_ssc_device   *ssc_dev = spi_master_get_devdata(master);
	struct spi_transfer     *xfer;
	unsigned long           flags;

	dev_dbg(&spi->dev, "new message %p submitted\n", msg);

	if (unlikely(list_empty(&msg->transfers)
		     || !spi->max_speed_hz)) {
		return -EINVAL;
	}

	if (ssc_dev->stopping)
		return -ESHUTDOWN;

	list_for_each_entry(xfer, &msg->transfers, transfer_list) {
		if (!(xfer->tx_buf || xfer->rx_buf) || (xfer->len == 0)) {
			dev_dbg(&spi->dev, "missing rx or tx buf\n");
			return -EINVAL;
		}

		/* FIXME implement these protocol options!! */
		if (xfer->bits_per_word || xfer->speed_hz) {
			dev_dbg(&spi->dev, "no protocol options yet\n");
			return -ENOPROTOOPT;
		}

#ifdef VERBOSE
		dev_dbg(spi->dev,
			"  xfer %p: len %u tx %p/%08x rx %p/%08x\n",
			xfer, xfer->len,
			xfer->tx_buf, xfer->tx_dma,
			xfer->rx_buf, xfer->rx_dma);
#endif
	}

	msg->status = -EINPROGRESS;
	msg->actual_length = 0;

	spin_lock_irqsave(&ssc_dev->lock, flags);
	list_add_tail(&msg->queue, &ssc_dev->queue);
	if (!ssc_dev->current_transfer)
	{
		/* start transmission machine, if not started yet */
		svip_ssc_next_message(master);
	}
	spin_unlock_irqrestore(&ssc_dev->lock, flags);

	return 0;
}

static void svip_ssc_cleanup(struct spi_device *spi)
{
	struct svip_ssc_device *ssc_dev = spi_master_get_devdata(spi->master);
	unsigned long   flags;

	if (!spi->controller_state)
		return;

	spin_lock_irqsave(&ssc_dev->lock, flags);
	if (ssc_dev->stay == spi) {
		ssc_dev->stay = NULL;
		cs_deactivate(ssc_dev, spi);
	}
	spin_unlock_irqrestore(&ssc_dev->lock, flags);
}

/*-------------------------------------------------------------------------*/

static int __init svip_ssc_probe(struct platform_device *pdev)
{
	int                  ret;
	struct spi_master    *master;
	struct svip_ssc_device *ssc_dev;
	struct resource      *res_regs;
	int                  irq;

	ret = -ENOMEM;

	/* setup spi core then atmel-specific driver state */
	master = spi_alloc_master(&pdev->dev, sizeof (*ssc_dev));
	if (!master)
	{
		dev_err (&pdev->dev, "ERROR: no memory for master spi\n");
		goto errout;
	}

	ssc_dev = spi_master_get_devdata(master);
	platform_set_drvdata(pdev, master);

	master->bus_num = pdev->id;
	master->num_chipselect = 8;
	master->mode_bits = MODEBITS;
	master->setup = svip_ssc_setup;
	master->transfer = svip_ssc_transfer;
	master->cleanup = svip_ssc_cleanup;

	spin_lock_init(&ssc_dev->lock);
	INIT_LIST_HEAD(&ssc_dev->queue);

	/* retrive register configration */
	res_regs = platform_get_resource_byname (pdev, IORESOURCE_MEM, "regs");
	if (NULL == res_regs)
	{
		dev_err (&pdev->dev, "ERROR: missed 'regs' resource\n");
		goto spierr;
	}

	ssc_dev->regs = (struct svip_reg_ssc*)KSEG1ADDR(res_regs->start);

	irq = platform_get_irq_byname (pdev, "tx");
	if (irq < 0)
		goto irqerr;
	sprintf(ssc_dev->intname[0], "%s_tx", pdev->name);
	ret = devm_request_irq(&pdev->dev, irq, svip_ssc_tir_handler,
			       IRQF_DISABLED, ssc_dev->intname[0], pdev);
	if (ret != 0)
		goto irqerr;

	irq = platform_get_irq_byname (pdev, "rx");
	if (irq < 0)
		goto irqerr;
	sprintf(ssc_dev->intname[1], "%s_rx", pdev->name);
	ret = devm_request_irq(&pdev->dev, irq, svip_ssc_rir_handler,
			       IRQF_DISABLED, ssc_dev->intname[1], pdev);
	if (ret != 0)
		goto irqerr;

	irq = platform_get_irq_byname (pdev, "err");
	if (irq < 0)
		goto irqerr;
	sprintf(ssc_dev->intname[2], "%s_err", pdev->name);
	ret = devm_request_irq(&pdev->dev, irq, svip_ssc_eir_handler,
			       IRQF_DISABLED, ssc_dev->intname[2], pdev);
	if (ret != 0)
		goto irqerr;

	irq = platform_get_irq_byname (pdev, "frm");
	if (irq < 0)
		goto irqerr;
	sprintf(ssc_dev->intname[3], "%s_frm", pdev->name);
	ret = devm_request_irq(&pdev->dev, irq, svip_ssc_fir_handler,
			       IRQF_DISABLED, ssc_dev->intname[3], pdev);
	if (ret != 0)
		goto irqerr;

	/*
	 * Initialize the Hardware
	 */

	/* Clear enable bit, i.e. put SSC into configuration mode */
	ssc_dev->regs->whbstate = SSC_WHBSTATE_CLREN;
	/* enable SSC core to run at fpi clock */
	ssc_dev->regs->clc = SSC_CLC_RMC_VAL(1);
	asm("sync");

	/* GPIO CS */
	ssc_dev->regs->gpocon     = SSC_GPOCON_ISCSBN_VAL(0xFF);
	ssc_dev->regs->whbgpostat = SSC_WHBGPOSTAT_SETOUTN_VAL(0xFF); /* CS to high */

	/* Set Master mode */
	ssc_dev->regs->whbstate = SSC_WHBSTATE_SETMS;

	/* enable and flush RX/TX FIFO */
	ssc_dev->regs->rxfcon = SSC_RXFCON_RXFITL_VAL(SVIP_SSC_RFIFO_WORDS-FIFO_HEADROOM) |
		SSC_RXFCON_RXFLU | /* Receive FIFO Flush */
		SSC_RXFCON_RXFEN;  /* Receive FIFO Enable */

	ssc_dev->regs->txfcon = SSC_TXFCON_TXFITL_VAL(FIFO_HEADROOM) |
		SSC_TXFCON_TXFLU | /* Transmit FIFO Flush */
		SSC_TXFCON_TXFEN;  /* Transmit FIFO Enable */
	asm("sync");

	/* enable IRQ */
	ssc_dev->regs->irnen = SSC_IRNEN_E;

	dev_info(&pdev->dev, "controller at 0x%08lx (irq %d)\n",
		 (unsigned long)ssc_dev->regs, platform_get_irq_byname (pdev, "rx"));

	ret = spi_register_master(master);
	if (ret)
		goto out_reset_hw;

	return 0;

out_reset_hw:

irqerr:
	devm_free_irq (&pdev->dev, platform_get_irq_byname (pdev, "tx"), pdev);
	devm_free_irq (&pdev->dev, platform_get_irq_byname (pdev, "rx"), pdev);
	devm_free_irq (&pdev->dev, platform_get_irq_byname (pdev, "err"), pdev);
	devm_free_irq (&pdev->dev, platform_get_irq_byname (pdev, "frm"), pdev);

spierr:

	spi_master_put(master);

errout:
	return ret;
}

static int __exit svip_ssc_remove(struct platform_device *pdev)
{
	struct spi_master       *master = platform_get_drvdata(pdev);
	struct svip_ssc_device   *ssc_dev = spi_master_get_devdata(master);
	struct spi_message      *msg;

	/* reset the hardware and block queue progress */
	spin_lock_irq(&ssc_dev->lock);
	ssc_dev->stopping = 1;
	/* TODO: shutdown hardware */
	spin_unlock_irq(&ssc_dev->lock);

	/* Terminate remaining queued transfers */
	list_for_each_entry(msg, &ssc_dev->queue, queue) {
		/* REVISIT unmapping the dma is a NOP on ARM and AVR32
		 * but we shouldn't depend on that...
		 */
		msg->status = -ESHUTDOWN;
		msg->complete(msg->context);
	}

	devm_free_irq (&pdev->dev, platform_get_irq_byname (pdev, "tx"), pdev);
	devm_free_irq (&pdev->dev, platform_get_irq_byname (pdev, "rx"), pdev);
	devm_free_irq (&pdev->dev, platform_get_irq_byname (pdev, "err"), pdev);
	devm_free_irq (&pdev->dev, platform_get_irq_byname (pdev, "frm"), pdev);

	spi_unregister_master(master);
	platform_set_drvdata(pdev, NULL);
	spi_master_put(master);
	return 0;
}

#ifdef CONFIG_PM
static int svip_ssc_suspend(struct platform_device *pdev, pm_message_t mesg)
{
	struct spi_master        *master = platform_get_drvdata(pdev);
	struct svip_ssc_device    *ssc_dev = spi_master_get_devdata(master);

	clk_disable(ssc_dev->clk);
	return 0;
}

static int svip_ssc_resume(struct platform_device *pdev)
{
	struct spi_master        *master = platform_get_drvdata(pdev);
	struct svip_ssc_device    *ssc_dev = spi_master_get_devdata(master);

	clk_enable(ssc_dev->clk);
	return 0;
}
#endif

static struct platform_driver svip_ssc_driver = {
	.driver		= {
		.name	= "ifx_ssc",
		.owner	= THIS_MODULE,
	},
	.probe		= svip_ssc_probe,
#ifdef CONFIG_PM
	.suspend	= svip_ssc_suspend,
	.resume		= svip_ssc_resume,
#endif
	.remove		= __exit_p(svip_ssc_remove)
};

int __init svip_ssc_init(void)
{
	return platform_driver_register(&svip_ssc_driver);
}

void __exit svip_ssc_exit(void)
{
	platform_driver_unregister(&svip_ssc_driver);
}

module_init(svip_ssc_init);
module_exit(svip_ssc_exit);

MODULE_ALIAS("platform:ifx_ssc");
MODULE_DESCRIPTION("Lantiq SSC Controller driver");
MODULE_AUTHOR("Andreas Schmidt <andreas.schmidt@infineon.com>");
MODULE_AUTHOR("Jevgenijs Grigorjevs <Jevgenijs.Grigorjevs@lantiq.com>");
MODULE_LICENSE("GPL");

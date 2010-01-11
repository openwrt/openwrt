/*
 *  Copyright (C) 2009, Lars-Peter Clausen <lars@metafoo.de>
 *  	JZ7420/JZ4740 GPIO SD/MMC controller driver
 *
 *  This program is free software; you can redistribute	 it and/or modify it
 *  under  the terms of	 the GNU General  Public License as published by the
 *  Free Software Foundation;  either version 2 of the	License, or (at your
 *  option) any later version.
 *
 *  You should have received a copy of the  GNU General Public License along
 *  with this program; if not, write  to the Free Software Foundation, Inc.,
 *  675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */

#include <linux/mmc/host.h>
#include <linux/io.h>
#include <linux/irq.h>
#include <linux/interrupt.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/delay.h>
#include <linux/scatterlist.h>
#include <linux/clk.h>
#include <linux/mmc/jz4740_mmc.h>

#include <linux/gpio.h>
#include <asm/mach-jz4740/gpio.h>
#include <asm/cacheflush.h>
#include <linux/dma-mapping.h>

#define JZ_REG_MMC_STRPCL	0x00
#define JZ_REG_MMC_STATUS	0x04
#define JZ_REG_MMC_CLKRT	0x08
#define JZ_REG_MMC_CMDAT	0x0C
#define JZ_REG_MMC_RESTO	0x10
#define JZ_REG_MMC_RDTO		0x14
#define JZ_REG_MMC_BLKLEN	0x18
#define JZ_REG_MMC_NOB		0x1C
#define JZ_REG_MMC_SNOB		0x20
#define JZ_REG_MMC_IMASK	0x24
#define JZ_REG_MMC_IREG		0x28
#define JZ_REG_MMC_CMD		0x2C
#define JZ_REG_MMC_ARG		0x30
#define JZ_REG_MMC_RESP_FIFO	0x34
#define JZ_REG_MMC_RXFIFO	0x38
#define JZ_REG_MMC_TXFIFO	0x3C

#define JZ_MMC_STRPCL_EXIT_MULTIPLE BIT(7)
#define JZ_MMC_STRPCL_EXIT_TRANSFER BIT(6)
#define JZ_MMC_STRPCL_START_READWAIT BIT(5)
#define JZ_MMC_STRPCL_STOP_READWAIT BIT(4)
#define JZ_MMC_STRPCL_RESET BIT(3)
#define JZ_MMC_STRPCL_START_OP BIT(2)
#define JZ_MMC_STRPCL_CLOCK_CONTROL BIT(1) | BIT(0)
#define JZ_MMC_STRPCL_CLOCK_STOP BIT(0)
#define JZ_MMC_STRPCL_CLOCK_START BIT(1)


#define JZ_MMC_STATUS_IS_RESETTING BIT(15)
#define JZ_MMC_STATUS_SDIO_INT_ACTIVE BIT(14)
#define JZ_MMC_STATUS_PRG_DONE BIT(13)
#define JZ_MMC_STATUS_DATA_TRAN_DONE BIT(12)
#define JZ_MMC_STATUS_END_CMD_RES BIT(11)
#define JZ_MMC_STATUS_DATA_FIFO_AFULL BIT(10)
#define JZ_MMC_STATUS_IS_READWAIT BIT(9)
#define JZ_MMC_STATUS_CLK_EN BIT(8)
#define JZ_MMC_STATUS_DATA_FIFO_FULL BIT(7)
#define JZ_MMC_STATUS_DATA_FIFO_EMPTY BIT(6)
#define JZ_MMC_STATUS_CRC_RES_ERR BIT(5)
#define JZ_MMC_STATUS_CRC_READ_ERROR BIT(4)
#define JZ_MMC_STATUS_TIMEOUT_WRITE BIT(3)
#define JZ_MMC_STATUS_CRC_WRITE_ERROR BIT(2)
#define JZ_MMC_STATUS_TIMEOUT_RES BIT(1)
#define JZ_MMC_STATUS_TIMEOUT_READ BIT(0)

#define JZ_MMC_STATUS_READ_ERROR_MASK (BIT(4) | BIT(0))
#define JZ_MMC_STATUS_WRITE_ERROR_MASK (BIT(3) | BIT(2))


#define JZ_MMC_CMDAT_IO_ABORT BIT(11)
#define JZ_MMC_CMDAT_BUS_WIDTH_4BIT BIT(10)
#define JZ_MMC_CMDAT_DMA_EN BIT(8)
#define JZ_MMC_CMDAT_INIT BIT(7)
#define JZ_MMC_CMDAT_BUSY BIT(6)
#define JZ_MMC_CMDAT_STREAM BIT(5)
#define JZ_MMC_CMDAT_WRITE BIT(4)
#define JZ_MMC_CMDAT_DATA_EN BIT(3)
#define JZ_MMC_CMDAT_RESPONSE_FORMAT BIT(2) | BIT(1) | BIT(0)
#define JZ_MMC_CMDAT_RSP_R1 1
#define JZ_MMC_CMDAT_RSP_R2 2
#define JZ_MMC_CMDAT_RSP_R3 3

#define JZ_MMC_IRQ_SDIO BIT(7)
#define JZ_MMC_IRQ_TXFIFO_WR_REQ BIT(6)
#define JZ_MMC_IRQ_RXFIFO_RD_REQ BIT(5)
#define JZ_MMC_IRQ_END_CMD_RES BIT(2)
#define JZ_MMC_IRQ_PRG_DONE BIT(1)
#define JZ_MMC_IRQ_DATA_TRAN_DONE BIT(0)


#define JZ_MMC_CLK_RATE 24000000

struct jz4740_mmc_host {
	struct mmc_host *mmc;
	struct platform_device *pdev;
	struct jz4740_mmc_platform_data *pdata;
	struct clk *clk;

	int irq;
	int card_detect_irq;

	struct resource *mem;
	void __iomem *base;
	struct mmc_request *req;
	struct mmc_command *cmd;

	int max_clock;
	uint32_t cmdat;

	uint16_t irq_mask;

	spinlock_t lock;
	struct timer_list clock_timer;
	struct timer_list timeout_timer;
	unsigned waiting:1;
};

static void jz4740_mmc_cmd_done(struct jz4740_mmc_host *host);

static void jz4740_mmc_enable_irq(struct jz4740_mmc_host *host, unsigned int irq)
{
	unsigned long flags;
	spin_lock_irqsave(&host->lock, flags);

	host->irq_mask &= ~irq;
	writew(host->irq_mask, host->base + JZ_REG_MMC_IMASK);

	spin_unlock_irqrestore(&host->lock, flags);
}

static void jz4740_mmc_disable_irq(struct jz4740_mmc_host *host, unsigned int irq)
{
	unsigned long flags;
	spin_lock_irqsave(&host->lock, flags);

	host->irq_mask |= irq;
	writew(host->irq_mask, host->base + JZ_REG_MMC_IMASK);

	spin_unlock_irqrestore(&host->lock, flags);
}

static void jz4740_mmc_clock_enable(struct jz4740_mmc_host *host, bool start_transfer)
{
	uint16_t val = JZ_MMC_STRPCL_CLOCK_START;

	if (start_transfer)
		val |= JZ_MMC_STRPCL_START_OP;

	writew(val, host->base + JZ_REG_MMC_STRPCL);
}

static void jz4740_mmc_clock_disable(struct jz4740_mmc_host *host)
{
	uint16_t status;
	writew(JZ_MMC_STRPCL_CLOCK_STOP, host->base + JZ_REG_MMC_STRPCL);
	do {
		status = readl(host->base + JZ_REG_MMC_STATUS);
	} while (status & JZ_MMC_STATUS_CLK_EN);

}

static void jz4740_mmc_reset(struct jz4740_mmc_host *host)
{
	writew(JZ_MMC_STRPCL_RESET, host->base + JZ_REG_MMC_STRPCL);
	udelay(10);
	while(readw(host->base + JZ_REG_MMC_STATUS) & JZ_MMC_STATUS_IS_RESETTING);
}

static void jz4740_mmc_request_done(struct jz4740_mmc_host *host)
{
	struct mmc_request *req;
	unsigned long flags;

	spin_lock_irqsave(&host->lock, flags);
	req = host->req;
	host->req = NULL;
	host->waiting = 0;
	spin_unlock_irqrestore(&host->lock, flags);

	if (!unlikely(req))
		return;

/*	if (req->cmd->error != 0) {
		printk("error\n");
		jz4740_mmc_reset(host);
	}*/

	mmc_request_done(host->mmc, req);
}

static void jz4740_mmc_write_data(struct jz4740_mmc_host *host, struct mmc_data *data) {
	struct scatterlist *sg;
	uint32_t *sg_pointer;
	int status;
	size_t i, j;

	for (sg = data->sg; sg; sg = sg_next(sg)) {
		sg_pointer = sg_virt(sg);
		i = sg->length / 4;
		j = i >> 3;
		i = i & 0x7;
		while (j) {
			do {
				status = readw(host->base + JZ_REG_MMC_IREG);
			} while (!(status & JZ_MMC_IRQ_TXFIFO_WR_REQ));
			writew(JZ_MMC_IRQ_TXFIFO_WR_REQ, host->base + JZ_REG_MMC_IREG);

			writel(sg_pointer[0], host->base + JZ_REG_MMC_TXFIFO);
			writel(sg_pointer[1], host->base + JZ_REG_MMC_TXFIFO);
			writel(sg_pointer[2], host->base + JZ_REG_MMC_TXFIFO);
			writel(sg_pointer[3], host->base + JZ_REG_MMC_TXFIFO);
			writel(sg_pointer[4], host->base + JZ_REG_MMC_TXFIFO);
			writel(sg_pointer[5], host->base + JZ_REG_MMC_TXFIFO);
			writel(sg_pointer[6], host->base + JZ_REG_MMC_TXFIFO);
			writel(sg_pointer[7], host->base + JZ_REG_MMC_TXFIFO);
			sg_pointer += 8;
			--j;
		}
		if (i) {
			do {
				status = readw(host->base + JZ_REG_MMC_IREG);
			} while (!(status & JZ_MMC_IRQ_TXFIFO_WR_REQ));
			writew(JZ_MMC_IRQ_TXFIFO_WR_REQ, host->base + JZ_REG_MMC_IREG);

			while (i) {
				writel(*sg_pointer, host->base + JZ_REG_MMC_TXFIFO);
				++sg_pointer;
				--i;
			}
		}
		data->bytes_xfered += sg->length;
	}

	status = readl(host->base + JZ_REG_MMC_STATUS);
	if (status & JZ_MMC_STATUS_WRITE_ERROR_MASK)
		goto err;

	writew(JZ_MMC_IRQ_TXFIFO_WR_REQ, host->base + JZ_REG_MMC_IREG);
	do {
		status = readl(host->base + JZ_REG_MMC_STATUS);
	} while ((status & JZ_MMC_STATUS_DATA_TRAN_DONE) == 0);
	writew(JZ_MMC_IRQ_DATA_TRAN_DONE, host->base + JZ_REG_MMC_IREG);

	return;
err:
	if(status & (JZ_MMC_STATUS_TIMEOUT_WRITE)) {
		host->req->cmd->error = -ETIMEDOUT;
		data->error = -ETIMEDOUT;
	} else {
		host->req->cmd->error = -EILSEQ;
		data->error = -EILSEQ;
	}
}

static void jz4740_mmc_timeout(unsigned long data)
{
	struct jz4740_mmc_host *host = (struct jz4740_mmc_host*)data;
	unsigned long flags;

	spin_lock_irqsave(&host->lock, flags);
	if (!host->waiting) {
		spin_unlock_irqrestore(&host->lock, flags);
		return;
	}

	host->waiting = 0;

	spin_unlock_irqrestore(&host->lock, flags);

	host->req->cmd->error = -ETIMEDOUT;
	jz4740_mmc_request_done(host);
}

static void jz4740_mmc_read_data(struct jz4740_mmc_host *host, struct mmc_data *data) {
	struct scatterlist *sg;
	uint32_t *sg_pointer;
	uint32_t d;
	uint16_t status = 0;
	size_t i, j;

	for (sg = data->sg; sg; sg = sg_next(sg)) {
		sg_pointer = sg_virt(sg);
		i = sg->length;
		j = i >> 5;
		i = i & 0x1f;
		while (j) {
			do {
				status = readw(host->base + JZ_REG_MMC_IREG);
			} while (!(status & JZ_MMC_IRQ_RXFIFO_RD_REQ));
			writew(JZ_MMC_IRQ_RXFIFO_RD_REQ, host->base + JZ_REG_MMC_IREG);

			sg_pointer[0] = readl(host->base + JZ_REG_MMC_RXFIFO);
			sg_pointer[1] = readl(host->base + JZ_REG_MMC_RXFIFO);
			sg_pointer[2] = readl(host->base + JZ_REG_MMC_RXFIFO);
			sg_pointer[3] = readl(host->base + JZ_REG_MMC_RXFIFO);
			sg_pointer[4] = readl(host->base + JZ_REG_MMC_RXFIFO);
			sg_pointer[5] = readl(host->base + JZ_REG_MMC_RXFIFO);
			sg_pointer[6] = readl(host->base + JZ_REG_MMC_RXFIFO);
			sg_pointer[7] = readl(host->base + JZ_REG_MMC_RXFIFO);

			sg_pointer += 8;
			--j;
		}

		while (i >= 4) {
			do {
				status = readl(host->base + JZ_REG_MMC_STATUS);
			} while ((status & JZ_MMC_STATUS_DATA_FIFO_EMPTY));

			*sg_pointer = readl(host->base + JZ_REG_MMC_RXFIFO);
			++sg_pointer;
			i -= 4;
		}
		if (i > 0) {
			d = readl(host->base + JZ_REG_MMC_RXFIFO);
			memcpy(sg_pointer, &d, i);
		}
		data->bytes_xfered += sg->length;

		flush_dcache_page(sg_page(sg));
	}

	status = readl(host->base + JZ_REG_MMC_STATUS);
	if (status & JZ_MMC_STATUS_READ_ERROR_MASK)
		goto err;

	/* For whatever reason there is sometime one word more in the fifo then
	 * requested */
	while ((status & JZ_MMC_STATUS_DATA_FIFO_EMPTY) == 0) {
		d = readl(host->base + JZ_REG_MMC_RXFIFO);
		status = readl(host->base + JZ_REG_MMC_STATUS);
	}
	return;

err:
	if(status & JZ_MMC_STATUS_TIMEOUT_READ) {
		host->req->cmd->error = -ETIMEDOUT;
		data->error = -ETIMEDOUT;
	} else {
		host->req->cmd->error = -EILSEQ;
		data->error = -EILSEQ;
	}
}

static irqreturn_t jz_mmc_irq_worker(int irq, void *devid)
{
	struct jz4740_mmc_host *host = (struct jz4740_mmc_host*)devid;

	if (host->cmd->error)
		jz4740_mmc_request_done(host);
	else
		jz4740_mmc_cmd_done(host);

	return IRQ_HANDLED;
}

static irqreturn_t jz_mmc_irq(int irq, void *devid)
{
	struct jz4740_mmc_host *host = devid;
	uint16_t irq_reg, status, tmp;
	unsigned long flags;
	irqreturn_t ret = IRQ_HANDLED;

	irq_reg = readw(host->base + JZ_REG_MMC_IREG);

	tmp = irq_reg;
	spin_lock(&host->lock);
	irq_reg &= ~host->irq_mask;
	spin_unlock(&host->lock);

	if (irq_reg & JZ_MMC_IRQ_SDIO) {
		writew(JZ_MMC_IRQ_SDIO, host->base + JZ_REG_MMC_IREG);
		mmc_signal_sdio_irq(host->mmc);
	}

	if (!host->req || !host->cmd) {
		goto handled;
	}


	spin_lock_irqsave(&host->lock, flags);
	if (!host->waiting) {
		spin_unlock_irqrestore(&host->lock, flags);
		goto handled;
	}
	host->waiting = 0;
	spin_unlock_irqrestore(&host->lock, flags);

	del_timer(&host->timeout_timer);

	status = readl(host->base + JZ_REG_MMC_STATUS);

	if (status & JZ_MMC_STATUS_TIMEOUT_RES) {
		host->cmd->error = -ETIMEDOUT;
	} else if (status & JZ_MMC_STATUS_CRC_RES_ERR) {
		host->cmd->error = -EIO;
	} else if(status & (JZ_MMC_STATUS_CRC_READ_ERROR |
						JZ_MMC_STATUS_CRC_WRITE_ERROR)) {
		host->cmd->data->error = -EIO;
	} else if(status & (JZ_MMC_STATUS_CRC_READ_ERROR |
						JZ_MMC_STATUS_CRC_WRITE_ERROR)) {
		host->cmd->data->error = -EIO;
	}

	if (irq_reg & JZ_MMC_IRQ_END_CMD_RES) {
		jz4740_mmc_disable_irq(host, JZ_MMC_IRQ_END_CMD_RES);
		writew(JZ_MMC_IRQ_END_CMD_RES, host->base + JZ_REG_MMC_IREG);
		ret = IRQ_WAKE_THREAD;
	}

	return ret;
handled:

	writew(0xff, host->base + JZ_REG_MMC_IREG);
	return IRQ_HANDLED;
}

static int jz4740_mmc_set_clock_rate(struct jz4740_mmc_host *host, int rate) {
	int div = 0;
	int real_rate = host->max_clock;
	jz4740_mmc_clock_disable(host);

	while ((real_rate >> 1) >= rate && div < 7) {
		++div;
		real_rate >>= 1;
	}
	clk_set_rate(host->clk, JZ_MMC_CLK_RATE);

	writew(div, host->base + JZ_REG_MMC_CLKRT);
	return real_rate;
}


static void jz4740_mmc_read_response(struct jz4740_mmc_host *host, struct mmc_command *cmd)
{
	int i;
	uint16_t tmp;
	if (cmd->flags & MMC_RSP_136) {
		tmp = readw(host->base + JZ_REG_MMC_RESP_FIFO);
		for (i = 0; i < 4; ++i) {
			cmd->resp[i] = tmp << 24;
			cmd->resp[i] |= readw(host->base + JZ_REG_MMC_RESP_FIFO) << 8;
			tmp = readw(host->base + JZ_REG_MMC_RESP_FIFO);
			cmd->resp[i] |= tmp >> 8;
		}
	} else {
		cmd->resp[0] = readw(host->base + JZ_REG_MMC_RESP_FIFO) << 24;
		cmd->resp[0] |= readw(host->base + JZ_REG_MMC_RESP_FIFO) << 8;
		cmd->resp[0] |= readw(host->base + JZ_REG_MMC_RESP_FIFO) & 0xff;
	}
}

static void jz4740_mmc_send_command(struct jz4740_mmc_host *host, struct mmc_command *cmd)
{
	uint32_t cmdat = host->cmdat;

	host->cmdat &= ~JZ_MMC_CMDAT_INIT;
	jz4740_mmc_clock_disable(host);

	host->cmd = cmd;

	if (cmd->flags & MMC_RSP_BUSY)
		cmdat |= JZ_MMC_CMDAT_BUSY;

	switch (mmc_resp_type(cmd)) {
	case MMC_RSP_R1B:
	case MMC_RSP_R1:
		cmdat |= JZ_MMC_CMDAT_RSP_R1;
		break;
	case MMC_RSP_R2:
		cmdat |= JZ_MMC_CMDAT_RSP_R2;
		break;
	case MMC_RSP_R3:
		cmdat |= JZ_MMC_CMDAT_RSP_R3;
		break;
	default:
		break;
	}

	if (cmd->data) {
		cmdat |= JZ_MMC_CMDAT_DATA_EN;
		if (cmd->data->flags & MMC_DATA_WRITE)
			cmdat |= JZ_MMC_CMDAT_WRITE;
		if (cmd->data->flags & MMC_DATA_STREAM)
			cmdat |= JZ_MMC_CMDAT_STREAM;

		writew(cmd->data->blksz, host->base + JZ_REG_MMC_BLKLEN);
		writew(cmd->data->blocks, host->base + JZ_REG_MMC_NOB);
	}

	writeb(cmd->opcode, host->base + JZ_REG_MMC_CMD);
	writel(cmd->arg, host->base + JZ_REG_MMC_ARG);
	writel(cmdat, host->base + JZ_REG_MMC_CMDAT);

	host->waiting = 1;
	jz4740_mmc_clock_enable(host, 1);
	mod_timer(&host->timeout_timer, 4*HZ);
}

static void jz4740_mmc_cmd_done(struct jz4740_mmc_host *host)
{
	uint32_t status;
	struct mmc_command *cmd = host->req->cmd;
	struct mmc_request *req = host->req;
	status = readl(host->base + JZ_REG_MMC_STATUS);

	if (cmd->flags & MMC_RSP_PRESENT)
		jz4740_mmc_read_response(host, cmd);

	if (cmd->data) {
		if (cmd->data->flags & MMC_DATA_READ)
			jz4740_mmc_read_data(host, cmd->data);
		else
			jz4740_mmc_write_data(host, cmd->data);
	}

	if (req->stop) {
		jz4740_mmc_send_command(host, req->stop);
		do {
			status = readl(host->base + JZ_REG_MMC_STATUS);
		} while ((status & JZ_MMC_STATUS_PRG_DONE) == 0);
		writew(JZ_MMC_IRQ_PRG_DONE, host->base + JZ_REG_MMC_IREG);
	}

	jz4740_mmc_request_done(host);
}

static void jz4740_mmc_request(struct mmc_host *mmc, struct mmc_request *req)
{
	struct jz4740_mmc_host *host = mmc_priv(mmc);

	host->req = req;

	writew(0xffff, host->base + JZ_REG_MMC_IREG);

	writew(JZ_MMC_IRQ_END_CMD_RES, host->base + JZ_REG_MMC_IREG);
	jz4740_mmc_enable_irq(host, JZ_MMC_IRQ_END_CMD_RES);
	jz4740_mmc_send_command(host, req->cmd);
}


static void jz4740_mmc_set_ios(struct mmc_host *mmc, struct mmc_ios *ios)
{
	struct jz4740_mmc_host *host = mmc_priv(mmc);
	if (ios->clock)
		jz4740_mmc_set_clock_rate(host, ios->clock);

	switch(ios->power_mode) {
	case MMC_POWER_UP:
		if (gpio_is_valid(host->pdata->gpio_power))
			gpio_set_value(host->pdata->gpio_power,
					!host->pdata->power_active_low);
		host->cmdat |= JZ_MMC_CMDAT_INIT;
		clk_enable(host->clk);
		break;
	case MMC_POWER_ON:
		break;
	default:
		if (gpio_is_valid(host->pdata->gpio_power))
			gpio_set_value(host->pdata->gpio_power,
					host->pdata->power_active_low);
		clk_disable(host->clk);
		break;
	}

	switch(ios->bus_width) {
	case MMC_BUS_WIDTH_1:
		host->cmdat &= ~JZ_MMC_CMDAT_BUS_WIDTH_4BIT;
		break;
	case MMC_BUS_WIDTH_4:
		host->cmdat |= JZ_MMC_CMDAT_BUS_WIDTH_4BIT;
		break;
	default:
		dev_err(&host->pdev->dev, "Invalid bus width: %d\n", ios->bus_width);
	}
}

static int jz4740_mmc_get_ro(struct mmc_host *mmc)
{
	struct jz4740_mmc_host *host = mmc_priv(mmc);
	if (!gpio_is_valid(host->pdata->gpio_read_only))
		return -ENOSYS;

	return gpio_get_value(host->pdata->gpio_read_only) ^
		host->pdata->read_only_active_low;
}

static int jz4740_mmc_get_cd(struct mmc_host *mmc)
{
	struct jz4740_mmc_host *host = mmc_priv(mmc);
	if (!gpio_is_valid(host->pdata->gpio_card_detect))
		return -ENOSYS;

	return gpio_get_value(host->pdata->gpio_card_detect) ^
			host->pdata->card_detect_active_low;
}

static irqreturn_t jz4740_mmc_card_detect_irq(int irq, void *devid)
{
	struct jz4740_mmc_host *host = devid;

	mmc_detect_change(host->mmc, HZ / 3);

	return IRQ_HANDLED;
}

static void jz4740_mmc_enable_sdio_irq(struct mmc_host *mmc, int enable)
{
	struct jz4740_mmc_host *host = mmc_priv(mmc);
	if (enable)
		jz4740_mmc_enable_irq(host, JZ_MMC_IRQ_SDIO);
	else
		jz4740_mmc_disable_irq(host, JZ_MMC_IRQ_SDIO);
}

static const struct mmc_host_ops jz4740_mmc_ops = {
	.request	= jz4740_mmc_request,
	.set_ios	= jz4740_mmc_set_ios,
	.get_ro		= jz4740_mmc_get_ro,
	.get_cd		= jz4740_mmc_get_cd,
	.enable_sdio_irq = jz4740_mmc_enable_sdio_irq,
};

static const struct jz_gpio_bulk_request jz4740_mmc_pins[] = {
	JZ_GPIO_BULK_PIN(MSC_CMD),
	JZ_GPIO_BULK_PIN(MSC_CLK),
	JZ_GPIO_BULK_PIN(MSC_DATA0),
	JZ_GPIO_BULK_PIN(MSC_DATA1),
	JZ_GPIO_BULK_PIN(MSC_DATA2),
	JZ_GPIO_BULK_PIN(MSC_DATA3),
};

static int __devinit jz4740_mmc_request_gpios(struct platform_device *pdev)
{
	int ret;
	struct jz4740_mmc_platform_data *pdata = pdev->dev.platform_data;

	if (!pdata)
		return 0;

	if (gpio_is_valid(pdata->gpio_card_detect)) {
		ret = gpio_request(pdata->gpio_card_detect, "MMC detect change");
		if (ret) {
			dev_err(&pdev->dev, "Failed to request detect change gpio\n");
			goto err;
		}
		gpio_direction_input(pdata->gpio_card_detect);
	}

	if (gpio_is_valid(pdata->gpio_read_only)) {
		ret = gpio_request(pdata->gpio_read_only, "MMC read only");
		if (ret) {
			dev_err(&pdev->dev, "Failed to request read only gpio: %d\n", ret);
			goto err_free_gpio_card_detect;
		}
		gpio_direction_input(pdata->gpio_read_only);
	}

	if (gpio_is_valid(pdata->gpio_power)) {
		ret = gpio_request(pdata->gpio_power, "MMC power");
		if (ret) {
			dev_err(&pdev->dev, "Failed to request power gpio: %d\n", ret);
			goto err_free_gpio_read_only;
		}
		gpio_direction_output(pdata->gpio_power, pdata->power_active_low);
	}

	return 0;

err_free_gpio_read_only:
	if (gpio_is_valid(pdata->gpio_read_only))
		gpio_free(pdata->gpio_read_only);
err_free_gpio_card_detect:
	if (gpio_is_valid(pdata->gpio_card_detect))
		gpio_free(pdata->gpio_card_detect);
err:
	return ret;
}

static void jz4740_mmc_free_gpios(struct platform_device *pdev)
{
	struct jz4740_mmc_platform_data *pdata = pdev->dev.platform_data;

	if (!pdata)
		return;

	if (gpio_is_valid(pdata->gpio_power))
		gpio_free(pdata->gpio_power);
	if (gpio_is_valid(pdata->gpio_read_only))
		gpio_free(pdata->gpio_read_only);
	if (gpio_is_valid(pdata->gpio_card_detect))
		gpio_free(pdata->gpio_card_detect);
}

static int __devinit jz4740_mmc_probe(struct platform_device* pdev)
{
	int ret;
	struct mmc_host *mmc;
	struct jz4740_mmc_host *host;
	struct jz4740_mmc_platform_data *pdata;

	pdata = pdev->dev.platform_data;

	mmc = mmc_alloc_host(sizeof(struct jz4740_mmc_host), &pdev->dev);

	if (!mmc) {
		dev_err(&pdev->dev, "Failed to alloc mmc host structure\n");
		return -ENOMEM;
	}

	host = mmc_priv(mmc);

	host->irq = platform_get_irq(pdev, 0);

	if (host->irq < 0) {
		ret = host->irq;
		dev_err(&pdev->dev, "Failed to get platform irq: %d\n", ret);
		goto err_free_host;
	}

	host->clk = clk_get(&pdev->dev, "mmc");
	if (!host->clk) {
		ret = -ENOENT;
		dev_err(&pdev->dev, "Failed to get mmc clock\n");
		goto err_free_host;
	}

	host->mem = platform_get_resource(pdev, IORESOURCE_MEM, 0);

	if (!host->mem) {
		ret = -ENOENT;
		dev_err(&pdev->dev, "Failed to get base platform memory\n");
		goto err_clk_put;
	}

	host->mem = request_mem_region(host->mem->start, resource_size(host->mem),
					pdev->name);

	if (!host->mem) {
		ret = -EBUSY;
		dev_err(&pdev->dev, "Failed to request base memory region\n");
		goto err_clk_put;
	}

	host->base = ioremap_nocache(host->mem->start, resource_size(host->mem));

	if (!host->base) {
		ret = -EBUSY;
		dev_err(&pdev->dev, "Failed to ioremap base memory\n");
		goto err_release_mem_region;
	}

	if (pdata && pdata->data_1bit)
		ret = jz_gpio_bulk_request(jz4740_mmc_pins, ARRAY_SIZE(jz4740_mmc_pins) - 3);
	else
		ret = jz_gpio_bulk_request(jz4740_mmc_pins, ARRAY_SIZE(jz4740_mmc_pins));

	if (ret) {
		dev_err(&pdev->dev, "Failed to request function pins: %d\n", ret);
		goto err_iounmap;
	}

	ret = jz4740_mmc_request_gpios(pdev);
	if (ret)
		goto err_gpio_bulk_free;

	mmc->ops = &jz4740_mmc_ops;
	mmc->f_min = JZ_MMC_CLK_RATE / 128;
	mmc->f_max = JZ_MMC_CLK_RATE;
	mmc->ocr_avail = MMC_VDD_32_33 | MMC_VDD_33_34;
	mmc->caps = (pdata && pdata->data_1bit) ? 0 : MMC_CAP_4_BIT_DATA;
	mmc->caps |= MMC_CAP_SDIO_IRQ;
	mmc->max_seg_size = 4096;
	mmc->max_phys_segs = 128;

	mmc->max_blk_size = (1 << 10) - 1;
	mmc->max_blk_count = (1 << 15) - 1;
	mmc->max_req_size = mmc->max_blk_size * mmc->max_blk_count;

	host->mmc = mmc;
	host->pdev = pdev;
	host->pdata = pdata;
	host->max_clock = JZ_MMC_CLK_RATE;
	spin_lock_init(&host->lock);
	host->irq_mask = 0xffff;

	host->card_detect_irq = gpio_to_irq(pdata->gpio_card_detect);

	if (host->card_detect_irq < 0) {
		dev_warn(&pdev->dev, "Failed to get irq for card detect gpio\n");
	} else {
		ret = request_irq(host->card_detect_irq,
				jz4740_mmc_card_detect_irq, IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING, "MMC/SD detect changed", host);

		if (ret) {
			dev_err(&pdev->dev, "Failed to request card detect irq");
			goto err_free_gpios;
		}
	}

	ret = request_threaded_irq(host->irq, jz_mmc_irq, jz_mmc_irq_worker, IRQF_DISABLED, "MMC/SD", host);
	if (ret) {
		dev_err(&pdev->dev, "Failed to request irq: %d\n", ret);
		goto err_free_card_detect_irq;
	}

	jz4740_mmc_reset(host);
	jz4740_mmc_clock_disable(host);
	setup_timer(&host->timeout_timer, jz4740_mmc_timeout, (unsigned long)host);

	platform_set_drvdata(pdev, host);
	ret = mmc_add_host(mmc);

	if (ret) {
		dev_err(&pdev->dev, "Failed to add mmc host: %d\n", ret);
		goto err_free_irq;
	}
	printk("JZ SD/MMC card driver registered\n");

	return 0;

err_free_irq:
	free_irq(host->irq, host);
err_free_card_detect_irq:
	if (host->card_detect_irq >= 0)
		free_irq(host->card_detect_irq, host);
err_free_gpios:
	jz4740_mmc_free_gpios(pdev);
err_gpio_bulk_free:
	if (pdata && pdata->data_1bit)
		jz_gpio_bulk_free(jz4740_mmc_pins, ARRAY_SIZE(jz4740_mmc_pins) - 3);
	else
		jz_gpio_bulk_free(jz4740_mmc_pins, ARRAY_SIZE(jz4740_mmc_pins));
err_iounmap:
	iounmap(host->base);
err_release_mem_region:
	release_mem_region(host->mem->start, resource_size(host->mem));
err_clk_put:
	clk_put(host->clk);
err_free_host:
	platform_set_drvdata(pdev, NULL);
	mmc_free_host(mmc);

	return ret;
}

static int jz4740_mmc_remove(struct platform_device *pdev)
{
	struct jz4740_mmc_host *host = platform_get_drvdata(pdev);
	struct jz4740_mmc_platform_data *pdata = host->pdata;

	del_timer_sync(&host->timeout_timer);
	jz4740_mmc_disable_irq(host, 0xff);
	jz4740_mmc_reset(host);

	mmc_remove_host(host->mmc);

	free_irq(host->irq, host);
	if (host->card_detect_irq >= 0)
		free_irq(host->card_detect_irq, host);

	jz4740_mmc_free_gpios(pdev);
	if (pdata && pdata->data_1bit)
		jz_gpio_bulk_free(jz4740_mmc_pins, ARRAY_SIZE(jz4740_mmc_pins) - 3);
	else
		jz_gpio_bulk_free(jz4740_mmc_pins, ARRAY_SIZE(jz4740_mmc_pins));

	iounmap(host->base);
	release_mem_region(host->mem->start, resource_size(host->mem));

	clk_put(host->clk);

	platform_set_drvdata(pdev, NULL);
	mmc_free_host(host->mmc);

	return 0;
}

#ifdef CONFIG_PM
static int jz4740_mmc_suspend(struct device *dev)
{
	struct jz4740_mmc_host *host = dev_get_drvdata(dev);
	struct jz4740_mmc_platform_data *pdata = host->pdata;

	mmc_suspend_host(host->mmc, PMSG_SUSPEND);

	if (pdata && pdata->data_1bit)
		jz_gpio_bulk_suspend(jz4740_mmc_pins, ARRAY_SIZE(jz4740_mmc_pins) - 3);
	else
		jz_gpio_bulk_suspend(jz4740_mmc_pins, ARRAY_SIZE(jz4740_mmc_pins));

	return 0;
}

static int jz4740_mmc_resume(struct device *dev)
{
	struct jz4740_mmc_host *host = dev_get_drvdata(dev);
	struct jz4740_mmc_platform_data *pdata = host->pdata;

	if (pdata && pdata->data_1bit)
		jz_gpio_bulk_resume(jz4740_mmc_pins, ARRAY_SIZE(jz4740_mmc_pins) - 3);
	else
		jz_gpio_bulk_resume(jz4740_mmc_pins, ARRAY_SIZE(jz4740_mmc_pins));

	mmc_resume_host(host->mmc);

	return 0;
}

struct dev_pm_ops jz4740_mmc_pm_ops = {
	.suspend	= jz4740_mmc_suspend,
	.resume		= jz4740_mmc_resume,
	.poweroff	= jz4740_mmc_suspend,
	.restore	= jz4740_mmc_resume,
};

#define jz4740_mmc_PM_OPS (&jz4740_mmc_pm_ops)
#else
#define jz4740_mmc_PM_OPS NULL
#endif

static struct platform_driver jz4740_mmc_driver = {
	.probe = jz4740_mmc_probe,
	.remove = jz4740_mmc_remove,
	.driver = {
		.name = "jz4740-mmc",
		.owner = THIS_MODULE,
		.pm = jz4740_mmc_PM_OPS,
	},
};

static int __init jz4740_mmc_init(void) {
	return platform_driver_register(&jz4740_mmc_driver);
}
module_init(jz4740_mmc_init);

static void __exit jz4740_mmc_exit(void) {
	platform_driver_unregister(&jz4740_mmc_driver);
}
module_exit(jz4740_mmc_exit);

MODULE_DESCRIPTION("JZ4720/JZ4740 SD/MMC controller driver");
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Lars-Peter Clausen <lars@metafoo.de>");

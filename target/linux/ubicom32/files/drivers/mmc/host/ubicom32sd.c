/*
 * drivers/mmc/host/ubicom32sd.c
 *	Ubicom32 Secure Digital Host Controller Interface driver
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
 */

#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/delay.h>
#include <linux/scatterlist.h>
#include <linux/leds.h>
#include <linux/gpio.h>
#include <linux/mmc/host.h>

#include <asm/ubicom32sd.h>

#define DRIVER_NAME "ubicom32sd"

#define sd_printk(...)
//#define sd_printk printk

#define SDTIO_VP_VERSION	3

#define SDTIO_MAX_SG_BLOCKS	16

enum sdtio_commands {
	SDTIO_COMMAND_NOP,
	SDTIO_COMMAND_SETUP,
	SDTIO_COMMAND_SETUP_SDIO,
	SDTIO_COMMAND_EXECUTE,
	SDTIO_COMMAND_RESET,
};

#define SDTIO_COMMAND_SHIFT			24
#define SDTIO_COMMAND_FLAG_STOP_RSP_CRC		(1 << 10)
#define SDTIO_COMMAND_FLAG_STOP_RSP_136		(1 << 9)
#define SDTIO_COMMAND_FLAG_STOP_RSP		(1 << 8)
#define SDTIO_COMMAND_FLAG_STOP_CMD		(1 << 7)
#define SDTIO_COMMAND_FLAG_DATA_STREAM		(1 << 6)
#define SDTIO_COMMAND_FLAG_DATA_RD		(1 << 5)
#define SDTIO_COMMAND_FLAG_DATA_WR		(1 << 4)
#define SDTIO_COMMAND_FLAG_CMD_RSP_CRC		(1 << 3)
#define SDTIO_COMMAND_FLAG_CMD_RSP_136		(1 << 2)
#define SDTIO_COMMAND_FLAG_CMD_RSP		(1 << 1)
#define SDTIO_COMMAND_FLAG_CMD			(1 << 0)

/*
 * SDTIO_COMMAND_SETUP_SDIO
 */
#define SDTIO_COMMAND_FLAG_SDIO_INT_EN		(1 << 0)

/*
 * SDTIO_COMMAND_SETUP
 *      clock speed in arg
 */
#define SDTIO_COMMAND_FLAG_4BIT                 (1 << 3)
#define SDTIO_COMMAND_FLAG_1BIT                 (1 << 2)
#define SDTIO_COMMAND_FLAG_SET_CLOCK            (1 << 1)
#define SDTIO_COMMAND_FLAG_SET_WIDTH            (1 << 0)

#define SDTIO_COMMAND_FLAG_CMD_RSP_MASK		(SDTIO_COMMAND_FLAG_CMD_RSP | SDTIO_COMMAND_FLAG_CMD_RSP_136)
#define SDTIO_COMMAND_FLAG_STOP_RSP_MASK	(SDTIO_COMMAND_FLAG_STOP_RSP | SDTIO_COMMAND_FLAG_STOP_RSP_136)
#define SDTIO_COMMAND_FLAG_RSP_MASK		(SDTIO_COMMAND_FLAG_CMD_RSP_MASK | SDTIO_COMMAND_FLAG_STOP_RSP_MASK)

struct sdtio_vp_sg {
	volatile void		*addr;
	volatile u32_t		len;
};

#define SDTIO_VP_INT_STATUS_DONE		(1 << 31)
#define SDTIO_VP_INT_STATUS_SDIO_INT		(1 << 10)
#define SDTIO_VP_INT_STATUS_DATA_CRC_ERR	(1 << 9)
#define SDTIO_VP_INT_STATUS_DATA_PROG_ERR	(1 << 8)
#define SDTIO_VP_INT_STATUS_DATA_TIMEOUT	(1 << 7)
#define SDTIO_VP_INT_STATUS_STOP_RSP_CRC	(1 << 6)
#define SDTIO_VP_INT_STATUS_STOP_RSP_TIMEOUT	(1 << 5)
#define SDTIO_VP_INT_STATUS_CMD_RSP_CRC		(1 << 4)
#define SDTIO_VP_INT_STATUS_CMD_RSP_TIMEOUT	(1 << 3)
#define SDTIO_VP_INT_STATUS_CMD_TIMEOUT		(1 << 2)
#define SDTIO_VP_INT_STATUS_CARD1_INSERT	(1 << 1)
#define SDTIO_VP_INT_STATUS_CARD0_INSERT	(1 << 0)

struct sdtio_vp_regs {
	u32_t				version;
	u32_t				f_max;
	u32_t				f_min;

	volatile u32_t			int_status;

	volatile u32_t			command;
	volatile u32_t			arg;

	volatile u32_t			cmd_opcode;
	volatile u32_t			cmd_arg;
	volatile u32_t			cmd_rsp0;
	volatile u32_t			cmd_rsp1;
	volatile u32_t			cmd_rsp2;
	volatile u32_t			cmd_rsp3;

	volatile u32_t			stop_opcode;
	volatile u32_t			stop_arg;
	volatile u32_t			stop_rsp0;
	volatile u32_t			stop_rsp1;
	volatile u32_t			stop_rsp2;
	volatile u32_t			stop_rsp3;

	volatile u32_t			data_timeout_ns;
	volatile u16_t			data_blksz;
	volatile u16_t			data_blkct;
	volatile u32_t			data_bytes_transferred;
	volatile u32_t			sg_len;
	struct sdtio_vp_sg		sg[SDTIO_MAX_SG_BLOCKS];
};

struct ubicom32sd_data {
	const struct ubicom32sd_platform_data	*pdata;

	struct mmc_host				*mmc;

	/*
	 * Lock used to protect the data structure
	spinlock_t				lock;
	 */
	int	int_en;
	int	int_pend;

	/*
	 * Receive and transmit interrupts used for communicating
	 * with hardware
	 */
	int					irq_tx;
	int					irq_rx;

	/*
	 * Current outstanding mmc request
	 */
	struct mmc_request			*mrq;

	/*
	 * Hardware registers
	 */
	struct sdtio_vp_regs			*regs;
};

/*****************************************************************************\
 *                                                                           *
 * Suspend/resume                                                            *
 *                                                                           *
\*****************************************************************************/

#if 0//def CONFIG_PM

int ubicom32sd_suspend_host(struct ubicom32sd_host *host, pm_message_t state)
{
	int ret;

	ret = mmc_suspend_host(host->mmc, state);
	if (ret)
		return ret;

	free_irq(host->irq, host);

	return 0;
}

EXPORT_SYMBOL_GPL(ubicom32sd_suspend_host);

int ubicom32sd_resume_host(struct ubicom32sd_host *host)
{
	int ret;

	if (host->flags & UBICOM32SD_USE_DMA) {
		if (host->ops->enable_dma)
			host->ops->enable_dma(host);
	}

	ret = request_irq(host->irq, ubicom32sd_irq, IRQF_SHARED,
			  mmc_hostname(host->mmc), host);
	if (ret)
		return ret;

	ubicom32sd_init(host);
	mmiowb();

	ret = mmc_resume_host(host->mmc);
	if (ret)
		return ret;

	return 0;
}

EXPORT_SYMBOL_GPL(ubicom32sd_resume_host);

#endif /* CONFIG_PM */

/*
 * ubicom32sd_send_command_sync
 */
static void ubicom32sd_send_command_sync(struct ubicom32sd_data *ud, u32_t command, u32_t arg)
{
	ud->regs->command = command;
	ud->regs->arg = arg;
	ubicom32_set_interrupt(ud->irq_tx);
	while (ud->regs->command) {
		ndelay(100);
	}
}

/*
 * ubicom32sd_send_command
 */
static void ubicom32sd_send_command(struct ubicom32sd_data *ud, u32_t command, u32_t arg)
{
	ud->regs->command = command;
	ud->regs->arg = arg;
	ubicom32_set_interrupt(ud->irq_tx);
}

/*
 * ubicom32sd_reset
 */
static void ubicom32sd_reset(struct ubicom32sd_data *ud)
{
	ubicom32sd_send_command_sync(ud, SDTIO_COMMAND_RESET << SDTIO_COMMAND_SHIFT, 0);
	ud->regs->int_status = 0;
}

/*
 * ubicom32sd_mmc_request
 */
static void ubicom32sd_mmc_request(struct mmc_host *mmc, struct mmc_request *mrq)
{
	struct ubicom32sd_data *ud = (struct ubicom32sd_data *)mmc_priv(mmc);
	u32_t command = SDTIO_COMMAND_EXECUTE << SDTIO_COMMAND_SHIFT;
	int ret = 0;

	WARN(ud->mrq != NULL, "ud->mrq still set to %p\n", ud->mrq);
	//pr_debug("send cmd %08x arg %08x flags %08x\n", cmd->opcode, cmd->arg, cmd->flags);

	if (mrq->cmd) {
		struct mmc_command *cmd = mrq->cmd;

		sd_printk("%s:\t\t\tsetup cmd %02d arg %08x flags %08x\n", mmc_hostname(mmc), cmd->opcode, cmd->arg, cmd->flags);

		ud->regs->cmd_opcode = cmd->opcode;
		ud->regs->cmd_arg = cmd->arg;

		command |= SDTIO_COMMAND_FLAG_CMD;

		if (cmd->flags & MMC_RSP_PRESENT) {
			command |= SDTIO_COMMAND_FLAG_CMD_RSP;
		}

		if (cmd->flags & MMC_RSP_136) {
			command |= SDTIO_COMMAND_FLAG_CMD_RSP_136;
		}

		if (cmd->flags & MMC_RSP_CRC) {
			command |= SDTIO_COMMAND_FLAG_CMD_RSP_CRC;
		}
	}

	if (mrq->data) {
		struct mmc_data *data = mrq->data;
		struct scatterlist *sg = data->sg;
		int i;

printk("%s:\t\t\tsetup data blksz %d num %d sglen=%d fl=%08x Tns=%u\n", mmc_hostname(mmc), data->blksz, data->blocks, data->sg_len, data->flags, data->timeout_ns);

		sd_printk("%s:\t\t\tsetup data blksz %d num %d sglen=%d fl=%08x Tns=%u\n",
			  mmc_hostname(mmc), data->blksz, data->blocks, data->sg_len,
			  data->flags, data->timeout_ns);

		if (data->sg_len > SDTIO_MAX_SG_BLOCKS) {
			ret = -EINVAL;
			data->error = -EINVAL;
			goto fail;
		}

		ud->regs->data_timeout_ns = data->timeout_ns;
		ud->regs->data_blksz = data->blksz;
		ud->regs->data_blkct = data->blocks;
		ud->regs->sg_len = data->sg_len;

		/*
		 * Load all of our sg list into the driver sg buffer
		 */
		for (i = 0; i < data->sg_len; i++) {
			sd_printk("%s: sg %d = %p %d\n", mmc_hostname(mmc), i, sg_virt(sg), sg->length);
			ud->regs->sg[i].addr = sg_virt(sg);
			ud->regs->sg[i].len = sg->length;
			if (((u32_t)ud->regs->sg[i].addr & 0x03) || (sg->length & 0x03)) {
				sd_printk("%s: Need aligned buffers\n", mmc_hostname(mmc));
				ret = -EINVAL;
				data->error = -EINVAL;
				goto fail;
			}
			sg++;
		}
		if (data->flags & MMC_DATA_READ) {
			command |= SDTIO_COMMAND_FLAG_DATA_RD;
		} else if (data->flags & MMC_DATA_WRITE) {
			command |= SDTIO_COMMAND_FLAG_DATA_WR;
		} else if (data->flags & MMC_DATA_STREAM) {
			command |= SDTIO_COMMAND_FLAG_DATA_STREAM;
		}
	}

	if (mrq->stop) {
		struct mmc_command *stop = mrq->stop;
		sd_printk("%s: \t\t\tsetup stop %02d arg %08x flags %08x\n", mmc_hostname(mmc), stop->opcode, stop->arg, stop->flags);

		ud->regs->stop_opcode = stop->opcode;
		ud->regs->stop_arg = stop->arg;

		command |= SDTIO_COMMAND_FLAG_STOP_CMD;

		if (stop->flags & MMC_RSP_PRESENT) {
			command |= SDTIO_COMMAND_FLAG_STOP_RSP;
		}

		if (stop->flags & MMC_RSP_136) {
			command |= SDTIO_COMMAND_FLAG_STOP_RSP_136;
		}

		if (stop->flags & MMC_RSP_CRC) {
			command |= SDTIO_COMMAND_FLAG_STOP_RSP_CRC;
		}
	}

	ud->mrq = mrq;

	sd_printk("%s: Sending command %08x\n", mmc_hostname(mmc), command);

	ubicom32sd_send_command(ud, command, 0);

	return;
fail:
	sd_printk("%s: mmcreq ret = %d\n", mmc_hostname(mmc), ret);
	mrq->cmd->error = ret;
	mmc_request_done(mmc, mrq);
}

/*
 * ubicom32sd_mmc_set_ios
 */
static void ubicom32sd_mmc_set_ios(struct mmc_host *mmc, struct mmc_ios *ios)
{
	struct ubicom32sd_data *ud = (struct ubicom32sd_data *)mmc_priv(mmc);
	u32_t command = SDTIO_COMMAND_SETUP << SDTIO_COMMAND_SHIFT;
	u32_t arg = 0;
	sd_printk("%s: ios call bw:%u pm:%u clk:%u\n", mmc_hostname(mmc), 1 << ios->bus_width, ios->power_mode, ios->clock);

	switch (ios->bus_width) {
	case MMC_BUS_WIDTH_1:
		command |= SDTIO_COMMAND_FLAG_SET_WIDTH | SDTIO_COMMAND_FLAG_1BIT;
		break;

	case MMC_BUS_WIDTH_4:
		command |= SDTIO_COMMAND_FLAG_SET_WIDTH | SDTIO_COMMAND_FLAG_4BIT;
		break;
	}

	if (ios->clock) {
		arg = ios->clock;
		command |= SDTIO_COMMAND_FLAG_SET_CLOCK;
	}

	switch (ios->power_mode) {

	/*
	 * Turn off the SD bus (power + clock)
	 */
	case MMC_POWER_OFF:
		gpio_set_value(ud->pdata->cards[0].pin_pwr, !ud->pdata->cards[0].pwr_polarity);
		command |= SDTIO_COMMAND_FLAG_SET_CLOCK;
		break;

	/*
	 * Turn on the power to the SD bus
	 */
	case MMC_POWER_ON:
		gpio_set_value(ud->pdata->cards[0].pin_pwr, ud->pdata->cards[0].pwr_polarity);
		break;

	/*
	 * Turn on the clock to the SD bus
	 */
	case MMC_POWER_UP:
		/*
		 * Done above
		 */
		break;
	}

	ubicom32sd_send_command_sync(ud, command, arg);

	/*
	 * Let the power settle down
	 */
	udelay(500);
}

/*
 * ubicom32sd_mmc_get_cd
 */
static int ubicom32sd_mmc_get_cd(struct mmc_host *mmc)
{
	struct ubicom32sd_data *ud = (struct ubicom32sd_data *)mmc_priv(mmc);
	sd_printk("%s: get cd %u %u\n", mmc_hostname(mmc), ud->pdata->cards[0].pin_cd, gpio_get_value(ud->pdata->cards[0].pin_cd));

	return gpio_get_value(ud->pdata->cards[0].pin_cd) ?
				ud->pdata->cards[0].cd_polarity :
				!ud->pdata->cards[0].cd_polarity;
}

/*
 * ubicom32sd_mmc_get_ro
 */
static int ubicom32sd_mmc_get_ro(struct mmc_host *mmc)
{
	struct ubicom32sd_data *ud = (struct ubicom32sd_data *)mmc_priv(mmc);
	sd_printk("%s: get ro %u %u\n", mmc_hostname(mmc), ud->pdata->cards[0].pin_wp, gpio_get_value(ud->pdata->cards[0].pin_wp));

	return gpio_get_value(ud->pdata->cards[0].pin_wp) ?
				ud->pdata->cards[0].wp_polarity :
				!ud->pdata->cards[0].wp_polarity;
}

/*
 * ubicom32sd_mmc_enable_sdio_irq
 */
static void ubicom32sd_mmc_enable_sdio_irq(struct mmc_host *mmc, int enable)
{
	struct ubicom32sd_data *ud = (struct ubicom32sd_data *)mmc_priv(mmc);

	ud->int_en = enable;
	if (enable && ud->int_pend) {
		ud->int_pend = 0;
		mmc_signal_sdio_irq(mmc);
	}
}

/*
 * ubicom32sd_interrupt
 */
static irqreturn_t ubicom32sd_interrupt(int irq, void *dev)
{
	struct mmc_host *mmc = (struct mmc_host *)dev;
	struct mmc_request *mrq;
	struct ubicom32sd_data *ud;
	u32_t int_status;

	if (!mmc) {
		return IRQ_HANDLED;
	}

	ud = (struct ubicom32sd_data *)mmc_priv(mmc);
	if (!ud) {
		return IRQ_HANDLED;
	}

	int_status = ud->regs->int_status;
	ud->regs->int_status &= ~int_status;

	if (int_status & SDTIO_VP_INT_STATUS_SDIO_INT) {
		if (ud->int_en) {
			ud->int_pend = 0;
			mmc_signal_sdio_irq(mmc);
		} else {
			ud->int_pend++;
		}
	}

	if (!(int_status & SDTIO_VP_INT_STATUS_DONE)) {
		return IRQ_HANDLED;
	}

	mrq = ud->mrq;
	if (!mrq) {
		sd_printk("%s: Spurious interrupt", mmc_hostname(mmc));
		return IRQ_HANDLED;
	}
	ud->mrq = NULL;

	/*
	 * SDTIO_VP_INT_DONE
	 */
	if (mrq->cmd->flags & MMC_RSP_PRESENT) {
		struct mmc_command *cmd = mrq->cmd;
		cmd->error = 0;

		if ((cmd->flags & MMC_RSP_CRC) && (int_status & SDTIO_VP_INT_STATUS_CMD_RSP_CRC)) {
			cmd->error = -EILSEQ;
		} else if (int_status & SDTIO_VP_INT_STATUS_CMD_RSP_TIMEOUT) {
			cmd->error = -ETIMEDOUT;
			goto done;
		} else if (cmd->flags & MMC_RSP_136) {
			cmd->resp[0] = ud->regs->cmd_rsp0;
			cmd->resp[1] = ud->regs->cmd_rsp1;
			cmd->resp[2] = ud->regs->cmd_rsp2;
			cmd->resp[3] = ud->regs->cmd_rsp3;
		} else {
			cmd->resp[0] = ud->regs->cmd_rsp0;
		}
		sd_printk("%s:\t\t\tResponse %08x %08x %08x %08x err=%d\n", mmc_hostname(mmc), cmd->resp[0], cmd->resp[1], cmd->resp[2], cmd->resp[3], cmd->error);
	}

	if (mrq->data) {
		struct mmc_data *data = mrq->data;

		if (int_status & SDTIO_VP_INT_STATUS_DATA_TIMEOUT) {
			data->error = -ETIMEDOUT;
			sd_printk("%s:\t\t\tData Timeout\n", mmc_hostname(mmc));
			goto done;
		} else if (int_status & SDTIO_VP_INT_STATUS_DATA_CRC_ERR) {
			data->error = -EILSEQ;
			sd_printk("%s:\t\t\tData CRC\n", mmc_hostname(mmc));
			goto done;
		} else if (int_status & SDTIO_VP_INT_STATUS_DATA_PROG_ERR) {
			data->error = -EILSEQ;
			sd_printk("%s:\t\t\tData Program Error\n", mmc_hostname(mmc));
			goto done;
		} else {
			data->error = 0;
			data->bytes_xfered = ud->regs->data_bytes_transferred;
		}
	}

	if (mrq->stop && (mrq->stop->flags & MMC_RSP_PRESENT)) {
		struct mmc_command *stop = mrq->stop;
		stop->error = 0;

		if ((stop->flags & MMC_RSP_CRC) && (int_status & SDTIO_VP_INT_STATUS_STOP_RSP_CRC)) {
			stop->error = -EILSEQ;
		} else if (int_status & SDTIO_VP_INT_STATUS_STOP_RSP_TIMEOUT) {
			stop->error = -ETIMEDOUT;
			goto done;
		} else if (stop->flags & MMC_RSP_136) {
			stop->resp[0] = ud->regs->stop_rsp0;
			stop->resp[1] = ud->regs->stop_rsp1;
			stop->resp[2] = ud->regs->stop_rsp2;
			stop->resp[3] = ud->regs->stop_rsp3;
		} else {
			stop->resp[0] = ud->regs->stop_rsp0;
		}
		sd_printk("%s:\t\t\tStop Response %08x %08x %08x %08x err=%d\n", mmc_hostname(mmc), stop->resp[0], stop->resp[1], stop->resp[2], stop->resp[3], stop->error);
	}

done:
	mmc_request_done(mmc, mrq);

	return IRQ_HANDLED;
}

static struct mmc_host_ops ubicom32sd_ops = {
	.request		= ubicom32sd_mmc_request,
	.set_ios		= ubicom32sd_mmc_set_ios,
	.get_ro			= ubicom32sd_mmc_get_ro,
	.get_cd			= ubicom32sd_mmc_get_cd,
	.enable_sdio_irq	= ubicom32sd_mmc_enable_sdio_irq,
};

/*
 * ubicom32sd_probe
 */
static int __devinit ubicom32sd_probe(struct platform_device *pdev)
{
	struct ubicom32sd_platform_data *pdata = (struct ubicom32sd_platform_data *)pdev->dev.platform_data;
	struct mmc_host *mmc;
	struct ubicom32sd_data *ud;
	struct resource *res_regs;
	struct resource *res_irq_tx;
	struct resource *res_irq_rx;
	int ret;

	/*
	 * Get our resources, regs is the hardware driver base address
	 * and the tx and rx irqs are used to communicate with the
	 * hardware driver.
	 */
	res_regs = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	res_irq_tx = platform_get_resource(pdev, IORESOURCE_IRQ, 0);
	res_irq_rx = platform_get_resource(pdev, IORESOURCE_IRQ, 1);
	if (!res_regs || !res_irq_tx || !res_irq_rx) {
		ret = -EINVAL;
		goto fail;
	}

	/*
	 * Reserve any gpios we need
	 */
	ret = gpio_request(pdata->cards[0].pin_wp, "sd-wp");
	if (ret) {
		goto fail;
	}
	gpio_direction_input(pdata->cards[0].pin_wp);

	ret = gpio_request(pdata->cards[0].pin_cd, "sd-cd");
	if (ret) {
		goto fail_cd;
	}
	gpio_direction_input(pdata->cards[0].pin_cd);

	/*
	 * HACK: for the dual port controller on port F, we don't support the second port right now
	 */
	if (pdata->ncards > 1) {
		ret = gpio_request(pdata->cards[1].pin_pwr, "sd-pwr");
		gpio_direction_output(pdata->cards[1].pin_pwr, !pdata->cards[1].pwr_polarity);
		gpio_direction_output(pdata->cards[1].pin_pwr, pdata->cards[1].pwr_polarity);
	}

	ret = gpio_request(pdata->cards[0].pin_pwr, "sd-pwr");
	if (ret) {
		goto fail_pwr;
	}
	gpio_direction_output(pdata->cards[0].pin_pwr, !pdata->cards[0].pwr_polarity);

	/*
	 * Allocate the MMC driver, it includes memory for our data.
	 */
	mmc = mmc_alloc_host(sizeof(struct ubicom32sd_data), &pdev->dev);
	if (!mmc) {
		ret = -ENOMEM;
		goto fail_mmc;
	}
	ud = (struct ubicom32sd_data *)mmc_priv(mmc);
	ud->mmc = mmc;
	ud->pdata = pdata;
	ud->regs = (struct sdtio_vp_regs *)res_regs->start;
	ud->irq_tx = res_irq_tx->start;
	ud->irq_rx = res_irq_rx->start;
	platform_set_drvdata(pdev, mmc);

	ret = request_irq(ud->irq_rx, ubicom32sd_interrupt, IRQF_DISABLED, mmc_hostname(mmc), mmc);
	if (ret) {
		goto fail_mmc;
	}

	/*
	 * Fill in the mmc structure
	 */
	mmc->ops = &ubicom32sd_ops;
	mmc->caps = MMC_CAP_4_BIT_DATA | MMC_CAP_NEEDS_POLL | MMC_CAP_SDIO_IRQ |
		    MMC_CAP_MMC_HIGHSPEED | MMC_CAP_SD_HIGHSPEED;

	mmc->f_min = ud->regs->f_min;
	mmc->f_max = ud->regs->f_max;
	mmc->ocr_avail = MMC_VDD_32_33 | MMC_VDD_33_34;

	/*
	 * Setup some restrictions on transfers
	 *
	 * We allow up to SDTIO_MAX_SG_BLOCKS of data to DMA into, there are
	 * not really any "max_seg_size", "max_req_size", or "max_blk_count"
	 * restrictions (must be less than U32_MAX though), pick
	 * something large?!...
	 *
	 * The hardware can do up to 4095 bytes per block, since the spec
	 * only requires 2048, we'll set it to that and not worry about
	 * potential weird blk lengths.
	 */
	mmc->max_hw_segs = SDTIO_MAX_SG_BLOCKS;
	mmc->max_phys_segs = SDTIO_MAX_SG_BLOCKS;
	mmc->max_seg_size = 1024 * 1024;
	mmc->max_req_size = 1024 * 1024;
	mmc->max_blk_count = 1024;

	mmc->max_blk_size = 2048;

	ubicom32sd_reset(ud);

	/*
	 * enable interrupts
	 */
	ud->int_en = 0;
	ubicom32sd_send_command_sync(ud, SDTIO_COMMAND_SETUP_SDIO << SDTIO_COMMAND_SHIFT | SDTIO_COMMAND_FLAG_SDIO_INT_EN, 0);

	mmc_add_host(mmc);

	printk(KERN_INFO "%s at %p, irq %d/%d\n", mmc_hostname(mmc),
			ud->regs, ud->irq_tx, ud->irq_rx);
	return 0;

fail_mmc:
	gpio_free(pdata->cards[0].pin_pwr);
fail_pwr:
	gpio_free(pdata->cards[0].pin_cd);
fail_cd:
	gpio_free(pdata->cards[0].pin_wp);
fail:
	return ret;
}

/*
 * ubicom32sd_remove
 */
static int __devexit ubicom32sd_remove(struct platform_device *pdev)
{
	struct mmc_host *mmc = platform_get_drvdata(pdev);

	platform_set_drvdata(pdev, NULL);

	if (mmc) {
		struct ubicom32sd_data *ud = (struct ubicom32sd_data *)mmc_priv(mmc);

		gpio_free(ud->pdata->cards[0].pin_pwr);
		gpio_free(ud->pdata->cards[0].pin_cd);
		gpio_free(ud->pdata->cards[0].pin_wp);

		mmc_remove_host(mmc);
		mmc_free_host(mmc);
	}

	/*
	 * Note that our data is allocated as part of the mmc structure
	 * so we don't need to free it.
	 */
	return 0;
}

static struct platform_driver ubicom32sd_driver = {
	.driver = {
		.name = DRIVER_NAME,
		.owner = THIS_MODULE,
	},
	.probe = ubicom32sd_probe,
	.remove = __devexit_p(ubicom32sd_remove),
#if 0
	.suspend = ubicom32sd_suspend,
	.resume = ubicom32sd_resume,
#endif
};

/*
 * ubicom32sd_init
 */
static int __init ubicom32sd_init(void)
{
	return platform_driver_register(&ubicom32sd_driver);
}
module_init(ubicom32sd_init);

/*
 * ubicom32sd_exit
 */
static void __exit ubicom32sd_exit(void)
{
    platform_driver_unregister(&ubicom32sd_driver);
}
module_exit(ubicom32sd_exit);

MODULE_AUTHOR("Patrick Tjin");
MODULE_DESCRIPTION("Ubicom32 Secure Digital Host Controller Interface driver");
MODULE_LICENSE("GPL");

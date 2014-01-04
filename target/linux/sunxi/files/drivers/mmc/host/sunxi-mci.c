/*
 * Driver for sunxi SD/MMC host controllers
 * (C) Copyright 2007-2011 Reuuimlla Technology Co., Ltd.
 * (C) Copyright 2007-2011 Aaron Maoye <leafy.myeh@reuuimllatech.com>
 * (C) Copyright 2013-2013 O2S GmbH <www.o2s.ch>
 * (C) Copyright 2013-2013 David Lanzendörfer <david.lanzendoerfer@o2s.ch>
 * (C) Copyright 2013-2013 Hans de Goede <hdegoede@redhat.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/io.h>
#include <linux/device.h>
#include <linux/interrupt.h>
#include <linux/delay.h>
#include <linux/err.h>

#include <linux/clk.h>
#include <linux/clk-private.h>
#include <linux/clk/sunxi.h>

#include <linux/gpio.h>
#include <linux/platform_device.h>
#include <linux/spinlock.h>
#include <linux/scatterlist.h>
#include <linux/dma-mapping.h>
#include <linux/slab.h>
#include <linux/regulator/consumer.h>

#include <linux/of_address.h>
#include <linux/of_gpio.h>
#include <linux/of_platform.h>

#include <linux/mmc/host.h>
#include <linux/mmc/sd.h>
#include <linux/mmc/mmc.h>
#include <linux/mmc/core.h>
#include <linux/mmc/card.h>

#include "sunxi-mci.h"

static void sunxi_mmc_init_host(struct mmc_host *mmc)
{
	u32 rval;
	struct sunxi_mmc_host *smc_host = mmc_priv(mmc);

	/* reset controller */
	rval = mci_readl(smc_host, REG_GCTRL) | SDXC_HWReset;
	mci_writel(smc_host, REG_GCTRL, rval);

	mci_writel(smc_host, REG_FTRGL, 0x20070008);
	mci_writel(smc_host, REG_TMOUT, 0xffffffff);
	mci_writel(smc_host, REG_IMASK, smc_host->sdio_imask);
	mci_writel(smc_host, REG_RINTR, 0xffffffff);
	mci_writel(smc_host, REG_DBGC, 0xdeb);
	mci_writel(smc_host, REG_FUNS, 0xceaa0000);
	mci_writel(smc_host, REG_DLBA, smc_host->sg_dma);
	rval = mci_readl(smc_host, REG_GCTRL)|SDXC_INTEnb;
	rval &= ~SDXC_AccessDoneDirect;
	mci_writel(smc_host, REG_GCTRL, rval);
}

static void sunxi_mmc_exit_host(struct sunxi_mmc_host *smc_host)
{
	mci_writel(smc_host, REG_GCTRL, SDXC_HWReset);
}

/* /\* UHS-I Operation Modes */
/*  * DS		25MHz	12.5MB/s	3.3V */
/*  * HS		50MHz	25MB/s		3.3V */
/*  * SDR12	25MHz	12.5MB/s	1.8V */
/*  * SDR25	50MHz	25MB/s		1.8V */
/*  * SDR50	100MHz	50MB/s		1.8V */
/*  * SDR104	208MHz	104MB/s		1.8V */
/*  * DDR50	50MHz	50MB/s		1.8V */
/*  * MMC Operation Modes */
/*  * DS		26MHz	26MB/s		3/1.8/1.2V */
/*  * HS		52MHz	52MB/s		3/1.8/1.2V */
/*  * HSDDR	52MHz	104MB/s		3/1.8/1.2V */
/*  * HS200	200MHz	200MB/s		1.8/1.2V */
/*  * */
/*  * Spec. Timing */
/*  * SD3.0 */
/*  * Fcclk    Tcclk   Fsclk   Tsclk   Tis     Tih     odly  RTis     RTih */
/*  * 400K     2.5us   24M     41ns    5ns     5ns     1     2209ns   41ns */
/*  * 25M      40ns    600M    1.67ns  5ns     5ns     3     14.99ns  5.01ns */
/*  * 50M      20ns    600M    1.67ns  6ns     2ns     3     14.99ns  5.01ns */
/*  * 50MDDR   20ns    600M    1.67ns  6ns     0.8ns   2     6.67ns   3.33ns */
/*  * 104M     9.6ns   600M    1.67ns  3ns     0.8ns   1     7.93ns   1.67ns */
/*  * 208M     4.8ns   600M    1.67ns  1.4ns   0.8ns   1     3.33ns   1.67ns */

/*  * 25M      40ns    300M    3.33ns  5ns     5ns     2     13.34ns   6.66ns */
/*  * 50M      20ns    300M    3.33ns  6ns     2ns     2     13.34ns   6.66ns */
/*  * 50MDDR   20ns    300M    3.33ns  6ns     0.8ns   1     6.67ns    3.33ns */
/*  * 104M     9.6ns   300M    3.33ns  3ns     0.8ns   0     7.93ns    1.67ns */
/*  * 208M     4.8ns   300M    3.33ns  1.4ns   0.8ns   0     3.13ns    1.67ns */

/*  * eMMC4.5 */
/*  * 400K     2.5us   24M     41ns    3ns     3ns     1     2209ns    41ns */
/*  * 25M      40ns    600M    1.67ns  3ns     3ns     3     14.99ns   5.01ns */
/*  * 50M      20ns    600M    1.67ns  3ns     3ns     3     14.99ns   5.01ns */
/*  * 50MDDR   20ns    600M    1.67ns  2.5ns   2.5ns   2     6.67ns    3.33ns */
/*  * 200M     5ns     600M    1.67ns  1.4ns   0.8ns   1     3.33ns    1.67ns */
/*  *\/ */

static void sunxi_mmc_init_idma_des(struct sunxi_mmc_host *host,
				    struct mmc_data *data)
{
	struct sunxi_idma_des *pdes = (struct sunxi_idma_des *)host->sg_cpu;
	struct sunxi_idma_des *pdes_pa = (struct sunxi_idma_des *)host->sg_dma;
	int i, max_len = (1 << host->idma_des_size_bits);

	for (i = 0; i < data->sg_len; i++) {
		pdes[i].config = SDXC_IDMAC_DES0_CH | SDXC_IDMAC_DES0_OWN |
				 SDXC_IDMAC_DES0_DIC;

		if (data->sg[i].length == max_len)
			pdes[i].buf_size = 0; /* 0 == max_len */
		else
			pdes[i].buf_size = data->sg[i].length;

		pdes[i].buf_addr_ptr1 = sg_dma_address(&data->sg[i]);
		pdes[i].buf_addr_ptr2 = (u32)&pdes_pa[i + 1];
	}
	pdes[0].config |= SDXC_IDMAC_DES0_FD;
	pdes[i - 1].config = SDXC_IDMAC_DES0_OWN | SDXC_IDMAC_DES0_LD;

	wmb(); /* Ensure idma_des hit main mem before we start the idmac */
}

static enum dma_data_direction sunxi_mmc_get_dma_dir(struct mmc_data *data)
{
	if (data->flags & MMC_DATA_WRITE)
		return DMA_TO_DEVICE;
	else
		return DMA_FROM_DEVICE;
}

static int sunxi_mmc_prepare_dma(struct sunxi_mmc_host *smc_host,
				 struct mmc_data *data)
{
	u32 dma_len;
	u32 i;
	u32 temp;
	struct scatterlist *sg;

	dma_len = dma_map_sg(mmc_dev(smc_host->mmc), data->sg, data->sg_len,
			     sunxi_mmc_get_dma_dir(data));
	if (dma_len == 0) {
		dev_err(mmc_dev(smc_host->mmc), "dma_map_sg failed\n");
		return -ENOMEM;
	}

	for_each_sg(data->sg, sg, data->sg_len, i) {
		if (sg->offset & 3 || sg->length & 3) {
			dev_err(mmc_dev(smc_host->mmc),
				"unaligned scatterlist: os %x length %d\n",
				sg->offset, sg->length);
			return -EINVAL;
		}
	}

	sunxi_mmc_init_idma_des(smc_host, data);

	temp = mci_readl(smc_host, REG_GCTRL);
	temp |= SDXC_DMAEnb;
	mci_writel(smc_host, REG_GCTRL, temp);
	temp |= SDXC_DMAReset;
	mci_writel(smc_host, REG_GCTRL, temp);

	mci_writel(smc_host, REG_DMAC, SDXC_IDMACSoftRST);

	if (!(data->flags & MMC_DATA_WRITE))
		mci_writel(smc_host, REG_IDIE, SDXC_IDMACReceiveInt);

	mci_writel(smc_host, REG_DMAC, SDXC_IDMACFixBurst | SDXC_IDMACIDMAOn);

	return 0;
}

static void sunxi_mmc_send_manual_stop(struct sunxi_mmc_host *host,
				       struct mmc_request *req)
{
	u32 cmd_val = SDXC_Start | SDXC_RspExp | SDXC_StopAbortCMD
			| SDXC_CheckRspCRC | MMC_STOP_TRANSMISSION;
	u32 ri = 0;
	unsigned long expire = jiffies + msecs_to_jiffies(1000);

	mci_writel(host, REG_CARG, 0);
	mci_writel(host, REG_CMDR, cmd_val);
	do {
		ri = mci_readl(host, REG_RINTR);
	} while (!(ri & (SDXC_CmdDone | SDXC_IntErrBit)) &&
		 time_before(jiffies, expire));

	if (ri & SDXC_IntErrBit) {
		dev_err(mmc_dev(host->mmc), "send stop command failed\n");
		if (req->stop)
			req->stop->resp[0] = -ETIMEDOUT;
	} else {
		if (req->stop)
			req->stop->resp[0] = mci_readl(host, REG_RESP0);
	}

	mci_writel(host, REG_RINTR, 0xffff);
}

static void sunxi_mmc_dump_errinfo(struct sunxi_mmc_host *smc_host)
{
	struct mmc_command *cmd = smc_host->mrq->cmd;
	struct mmc_data *data = smc_host->mrq->data;

	/* For some cmds timeout is normal with sd/mmc cards */
	if ((smc_host->int_sum & SDXC_IntErrBit) == SDXC_RespTimeout &&
			(cmd->opcode == 5 || cmd->opcode == 52))
		return;

	dev_err(mmc_dev(smc_host->mmc),
		"smc %d err, cmd %d,%s%s%s%s%s%s%s%s%s%s !!\n",
		smc_host->mmc->index, cmd->opcode,
		data ? (data->flags & MMC_DATA_WRITE ? " WR" : " RD") : "",
		smc_host->int_sum & SDXC_RespErr     ? " RE"     : "",
		smc_host->int_sum & SDXC_RespCRCErr  ? " RCE"    : "",
		smc_host->int_sum & SDXC_DataCRCErr  ? " DCE"    : "",
		smc_host->int_sum & SDXC_RespTimeout ? " RTO"    : "",
		smc_host->int_sum & SDXC_DataTimeout ? " DTO"    : "",
		smc_host->int_sum & SDXC_FIFORunErr  ? " FE"     : "",
		smc_host->int_sum & SDXC_HardWLocked ? " HL"     : "",
		smc_host->int_sum & SDXC_StartBitErr ? " SBE"    : "",
		smc_host->int_sum & SDXC_EndBitErr   ? " EBE"    : ""
		);
}

static void sunxi_mmc_finalize_request(struct sunxi_mmc_host *host)
{
	struct mmc_request *mrq;
	unsigned long iflags;

	spin_lock_irqsave(&host->lock, iflags);

	mrq = host->mrq;
	if (!mrq) {
		spin_unlock_irqrestore(&host->lock, iflags);
		dev_err(mmc_dev(host->mmc), "no request to finalize\n");
		return;
	}

	if (host->int_sum & SDXC_IntErrBit) {
		sunxi_mmc_dump_errinfo(host);
		mrq->cmd->error = -ETIMEDOUT;
		if (mrq->data)
			mrq->data->error = -ETIMEDOUT;
		if (mrq->stop)
			mrq->stop->error = -ETIMEDOUT;
	} else {
		if (mrq->cmd->flags & MMC_RSP_136) {
			mrq->cmd->resp[0] = mci_readl(host, REG_RESP3);
			mrq->cmd->resp[1] = mci_readl(host, REG_RESP2);
			mrq->cmd->resp[2] = mci_readl(host, REG_RESP1);
			mrq->cmd->resp[3] = mci_readl(host, REG_RESP0);
		} else {
			mrq->cmd->resp[0] = mci_readl(host, REG_RESP0);
		}
		if (mrq->data)
			mrq->data->bytes_xfered =
				mrq->data->blocks * mrq->data->blksz;
	}

	if (mrq->data) {
		struct mmc_data *data = mrq->data;
		u32 temp;

		mci_writel(host, REG_IDST, 0x337);
		mci_writel(host, REG_DMAC, 0);
		temp = mci_readl(host, REG_GCTRL);
		mci_writel(host, REG_GCTRL, temp|SDXC_DMAReset);
		temp &= ~SDXC_DMAEnb;
		mci_writel(host, REG_GCTRL, temp);
		temp |= SDXC_FIFOReset;
		mci_writel(host, REG_GCTRL, temp);
		dma_unmap_sg(mmc_dev(host->mmc), data->sg, data->sg_len,
				     sunxi_mmc_get_dma_dir(data));
	}

	mci_writel(host, REG_RINTR, 0xffff);

	dev_dbg(mmc_dev(host->mmc), "req done, resp %08x %08x %08x %08x\n",
		mrq->cmd->resp[0], mrq->cmd->resp[1],
		mrq->cmd->resp[2], mrq->cmd->resp[3]);

	host->mrq = NULL;
	host->int_sum = 0;
	host->wait_dma = 0;

	spin_unlock_irqrestore(&host->lock, iflags);

	if (mrq->data && mrq->data->error) {
		dev_err(mmc_dev(host->mmc),
			"data error, sending stop command\n");
		sunxi_mmc_send_manual_stop(host, mrq);
	}

	mmc_request_done(host->mmc, mrq);
}

static s32 sunxi_mmc_get_ro(struct mmc_host *mmc)
{
	struct sunxi_mmc_host *host = mmc_priv(mmc);

	int read_only = 0;

	if (gpio_is_valid(host->wp_pin)) {
		pinctrl_request_gpio(host->wp_pin);
		read_only = gpio_get_value(host->wp_pin);
	}

	return read_only;
}

static irqreturn_t sunxi_mmc_irq(int irq, void *dev_id)
{
	struct sunxi_mmc_host *host = dev_id;
	u32 finalize = 0;
	u32 sdio_int = 0;
	u32 msk_int;
	u32 idma_int;

	spin_lock(&host->lock);

	idma_int  = mci_readl(host, REG_IDST);
	msk_int   = mci_readl(host, REG_MISTA);

	dev_dbg(mmc_dev(host->mmc), "irq: rq %p mi %08x idi %08x\n",
		host->mrq, msk_int, idma_int);

	if (host->mrq) {
		if (idma_int & SDXC_IDMACReceiveInt)
			host->wait_dma = 0;

		host->int_sum |= msk_int;

		/* Wait for CmdDone on RespTimeout before finishing the req */
		if ((host->int_sum & SDXC_RespTimeout) &&
				!(host->int_sum & SDXC_CmdDone))
			mci_writel(host, REG_IMASK,
				   host->sdio_imask | SDXC_CmdDone);
		else if (host->int_sum & SDXC_IntErrBit)
			finalize = 1; /* Don't wait for dma on error */
		else if (host->int_sum & SDXC_IntDoneBit && !host->wait_dma)
			finalize = 1; /* Done */

		if (finalize) {
			mci_writel(host, REG_IMASK, host->sdio_imask);
			mci_writel(host, REG_IDIE, 0);
		}
	}

	if (msk_int & SDXC_SDIOInt)
		sdio_int = 1;

	mci_writel(host, REG_RINTR, msk_int);
	mci_writel(host, REG_IDST, idma_int);

	spin_unlock(&host->lock);

	if (finalize)
		tasklet_schedule(&host->tasklet);

	if (sdio_int)
		mmc_signal_sdio_irq(host->mmc);

	return IRQ_HANDLED;
}

static void sunxi_mmc_tasklet(unsigned long data)
{
	struct sunxi_mmc_host *smc_host = (struct sunxi_mmc_host *) data;
	sunxi_mmc_finalize_request(smc_host);
}

static void sunxi_mmc_oclk_onoff(struct sunxi_mmc_host *host, u32 oclk_en)
{
	unsigned long expire = jiffies + msecs_to_jiffies(2000);
	u32 rval;

	rval = mci_readl(host, REG_CLKCR);
	rval &= ~(SDXC_CardClkOn | SDXC_LowPowerOn);
	if (oclk_en)
		rval |= SDXC_CardClkOn;
	if (!host->io_flag)
		rval |= SDXC_LowPowerOn;
	mci_writel(host, REG_CLKCR, rval);

	rval = SDXC_Start | SDXC_UPCLKOnly | SDXC_WaitPreOver;
	if (host->voltage_switching)
		rval |= SDXC_VolSwitch;
	mci_writel(host, REG_CMDR, rval);
	do {
		rval = mci_readl(host, REG_CMDR);
	} while (time_before(jiffies, expire) && (rval & SDXC_Start));

	if (rval & SDXC_Start) {
		dev_err(mmc_dev(host->mmc), "fatal err update clk timeout\n");
		host->ferror = 1;
	}
}

static void sunxi_mmc_set_clk_dly(struct sunxi_mmc_host *smc_host,
				  u32 oclk_dly, u32 sclk_dly)
{
	unsigned long iflags;
	struct clk_hw *hw = __clk_get_hw(smc_host->clk_mod);

	spin_lock_irqsave(&smc_host->lock, iflags);
	clk_sunxi_mmc_phase_control(hw, sclk_dly, oclk_dly);
	spin_unlock_irqrestore(&smc_host->lock, iflags);
}

struct sunxi_mmc_clk_dly mmc_clk_dly[MMC_CLK_MOD_NUM] = {
	{ MMC_CLK_400K, 0, 7 },
	{ MMC_CLK_25M, 0, 5 },
	{ MMC_CLK_50M, 3, 5 },
	{ MMC_CLK_50MDDR, 2, 4 },
	{ MMC_CLK_50MDDR_8BIT, 2, 4 },
	{ MMC_CLK_100M, 1, 4 },
	{ MMC_CLK_200M, 1, 4 },
};

static void sunxi_mmc_clk_set_rate(struct sunxi_mmc_host *smc_host,
				   unsigned int rate)
{
	u32 newrate;
	u32 src_clk;
	u32 oclk_dly;
	u32 sclk_dly;
	u32 temp;
	struct sunxi_mmc_clk_dly *dly = NULL;

	newrate = clk_round_rate(smc_host->clk_mod, rate);
	if (smc_host->clk_mod_rate == newrate) {
		dev_dbg(mmc_dev(smc_host->mmc), "clk already %d, rounded %d\n",
			rate, newrate);
		return;
	}

	dev_dbg(mmc_dev(smc_host->mmc), "setting clk to %d, rounded %d\n",
		rate, newrate);

	/* setting clock rate */
	clk_disable(smc_host->clk_mod);
	clk_set_rate(smc_host->clk_mod, newrate);
	clk_enable(smc_host->clk_mod);
	smc_host->clk_mod_rate = newrate = clk_get_rate(smc_host->clk_mod);
	dev_dbg(mmc_dev(smc_host->mmc), "clk is now %d\n", newrate);

	sunxi_mmc_oclk_onoff(smc_host, 0);
	/* clear internal divider */
	temp = mci_readl(smc_host, REG_CLKCR);
	temp &= ~0xff;
	mci_writel(smc_host, REG_CLKCR, temp);

	/* determine delays */
	if (rate <= 400000) {
		dly = &mmc_clk_dly[MMC_CLK_400K];
	} else if (rate <= 25000000) {
		dly = &mmc_clk_dly[MMC_CLK_25M];
	} else if (rate <= 50000000) {
		if (smc_host->ddr) {
			if (smc_host->bus_width == 8)
				dly = &mmc_clk_dly[MMC_CLK_50MDDR_8BIT];
			else
				dly = &mmc_clk_dly[MMC_CLK_50MDDR];
		} else {
			dly = &mmc_clk_dly[MMC_CLK_50M];
		}
	} else if (rate <= 104000000) {
		dly = &mmc_clk_dly[MMC_CLK_100M];
	} else if (rate <= 208000000) {
		dly = &mmc_clk_dly[MMC_CLK_200M];
	} else
		dly = &mmc_clk_dly[MMC_CLK_50M];

	oclk_dly = dly->oclk_dly;
	sclk_dly = dly->sclk_dly;

	src_clk = clk_get_rate(clk_get_parent(smc_host->clk_mod));
	if (src_clk >= 300000000 && src_clk <= 400000000) {
		if (oclk_dly)
			oclk_dly--;
		if (sclk_dly)
			sclk_dly--;
	}

	sunxi_mmc_set_clk_dly(smc_host, oclk_dly, sclk_dly);
	sunxi_mmc_oclk_onoff(smc_host, 1);

	/* oclk_onoff sets various irq status bits, clear these */
	mci_writel(smc_host, REG_RINTR,
		   mci_readl(smc_host, REG_RINTR) & ~SDXC_SDIOInt);
}

static void sunxi_mmc_set_ios(struct mmc_host *mmc, struct mmc_ios *ios)
{
	struct sunxi_mmc_host *host = mmc_priv(mmc);
	u32 temp;
	s32 err;

	/* Set the power state */
	switch (ios->power_mode) {
	case MMC_POWER_ON:
		break;

	case MMC_POWER_UP:
		if (!IS_ERR(host->vmmc)) {
			mmc_regulator_set_ocr(host->mmc, host->vmmc, ios->vdd);
			udelay(200);
		}

		err =  clk_prepare_enable(host->clk_ahb);
		if (err) {
			dev_err(mmc_dev(host->mmc), "AHB clk err %d\n", err);
			host->ferror = 1;
			return;
		}
		err =  clk_prepare_enable(host->clk_mod);
		if (err) {
			dev_err(mmc_dev(host->mmc), "MOD clk err %d\n", err);
			host->ferror = 1;
			return;
		}

		sunxi_mmc_init_host(mmc);
		enable_irq(host->irq);

		dev_dbg(mmc_dev(host->mmc), "power on!\n");
		host->ferror = 0;
		break;

	case MMC_POWER_OFF:
		dev_dbg(mmc_dev(host->mmc), "power off!\n");
		disable_irq(host->irq);
		sunxi_mmc_exit_host(host);
		clk_disable_unprepare(host->clk_ahb);
		clk_disable_unprepare(host->clk_mod);
		if (!IS_ERR(host->vmmc))
			mmc_regulator_set_ocr(host->mmc, host->vmmc, 0);
		host->ferror = 0;
		break;
	}

	/* set bus width */
	switch (ios->bus_width) {
	case MMC_BUS_WIDTH_1:
		mci_writel(host, REG_WIDTH, SDXC_WIDTH1);
		host->bus_width = 1;
		break;
	case MMC_BUS_WIDTH_4:
		mci_writel(host, REG_WIDTH, SDXC_WIDTH4);
		host->bus_width = 4;
		break;
	case MMC_BUS_WIDTH_8:
		mci_writel(host, REG_WIDTH, SDXC_WIDTH8);
		host->bus_width = 8;
		break;
	}

	/* set ddr mode */
	temp = mci_readl(host, REG_GCTRL);
	if (ios->timing == MMC_TIMING_UHS_DDR50) {
		temp |= SDXC_DDR_MODE;
		host->ddr = 1;
	} else {
		temp &= ~SDXC_DDR_MODE;
		host->ddr = 0;
	}
	mci_writel(host, REG_GCTRL, temp);

	/* set up clock */
	if (ios->clock && ios->power_mode) {
		dev_dbg(mmc_dev(host->mmc), "ios->clock: %d\n", ios->clock);
		sunxi_mmc_clk_set_rate(host, ios->clock);
		usleep_range(50000, 55000);
	}
}

static void sunxi_mmc_enable_sdio_irq(struct mmc_host *mmc, int enable)
{
	struct sunxi_mmc_host *smc_host = mmc_priv(mmc);
	unsigned long flags;
	u32 imask;

	spin_lock_irqsave(&smc_host->lock, flags);
	imask = mci_readl(smc_host, REG_IMASK);
	if (enable) {
		smc_host->sdio_imask = SDXC_SDIOInt;
		imask |= SDXC_SDIOInt;
	} else {
		smc_host->sdio_imask = 0;
		imask &= ~SDXC_SDIOInt;
	}
	mci_writel(smc_host, REG_IMASK, imask);
	spin_unlock_irqrestore(&smc_host->lock, flags);
}

static void sunxi_mmc_hw_reset(struct mmc_host *mmc)
{
	struct sunxi_mmc_host *smc_host = mmc_priv(mmc);
	mci_writel(smc_host, REG_HWRST, 0);
	udelay(10);
	mci_writel(smc_host, REG_HWRST, 1);
	udelay(300);
}

static int sunxi_mmc_card_present(struct mmc_host *mmc)
{
	struct sunxi_mmc_host *host = mmc_priv(mmc);

	switch (host->cd_mode) {
	case CARD_DETECT_BY_GPIO_POLL:
		return !gpio_get_value(host->cd_pin); /* Signal inverted */
	case CARD_ALWAYS_PRESENT:
		return 1;
	}
	return 0; /* Never reached */
}

static void sunxi_mmc_request(struct mmc_host *mmc, struct mmc_request *mrq)
{
	struct sunxi_mmc_host *host = mmc_priv(mmc);
	struct mmc_command *cmd = mrq->cmd;
	struct mmc_data *data = mrq->data;
	unsigned long iflags;
	u32 imask = SDXC_IntErrBit;
	u32 cmd_val = SDXC_Start | (cmd->opcode & 0x3f);
	u32 byte_cnt = 0;
	int ret;

	if (!sunxi_mmc_card_present(mmc) || host->ferror) {
		dev_dbg(mmc_dev(host->mmc), "no medium present\n");
		mrq->cmd->error = -ENOMEDIUM;
		mmc_request_done(mmc, mrq);
		return;
	}

	if (data) {
		byte_cnt = data->blksz * data->blocks;
		mci_writel(host, REG_BLKSZ, data->blksz);
		mci_writel(host, REG_BCNTR, byte_cnt);
		ret = sunxi_mmc_prepare_dma(host, data);
		if (ret < 0) {
			dev_err(mmc_dev(host->mmc), "prepare DMA failed\n");
			cmd->error = ret;
			cmd->data->error = ret;
			mmc_request_done(host->mmc, mrq);
			return;
		}
	}

	if (cmd->opcode == MMC_GO_IDLE_STATE) {
		cmd_val |= SDXC_SendInitSeq;
		imask |= SDXC_CmdDone;
	}

	if (cmd->opcode == SD_SWITCH_VOLTAGE) {
		cmd_val |= SDXC_VolSwitch;
		imask |= SDXC_VolChgDone;
		host->voltage_switching = 1;
		sunxi_mmc_oclk_onoff(host, 1);
	}

	if (cmd->flags & MMC_RSP_PRESENT) {
		cmd_val |= SDXC_RspExp;
		if (cmd->flags & MMC_RSP_136)
			cmd_val |= SDXC_LongRsp;
		if (cmd->flags & MMC_RSP_CRC)
			cmd_val |= SDXC_CheckRspCRC;

		if ((cmd->flags & MMC_CMD_MASK) == MMC_CMD_ADTC) {
			cmd_val |= SDXC_DataExp | SDXC_WaitPreOver;
			if (cmd->data->flags & MMC_DATA_STREAM) {
				imask |= SDXC_AutoCMDDone;
				cmd_val |= SDXC_Seqmod | SDXC_SendAutoStop;
			}
			if (cmd->data->stop) {
				imask |= SDXC_AutoCMDDone;
				cmd_val |= SDXC_SendAutoStop;
			} else
				imask |= SDXC_DataOver;

			if (cmd->data->flags & MMC_DATA_WRITE)
				cmd_val |= SDXC_Write;
			else
				host->wait_dma = 1;
		} else
			imask |= SDXC_CmdDone;
	} else
		imask |= SDXC_CmdDone;

	dev_dbg(mmc_dev(host->mmc), "cmd %d(%08x) arg %x ie 0x%08x len %d\n",
		cmd_val & 0x3f, cmd_val, cmd->arg, imask,
		mrq->data ? mrq->data->blksz * mrq->data->blocks : 0);

	spin_lock_irqsave(&host->lock, iflags);
	host->mrq = mrq;
	mci_writel(host, REG_IMASK, host->sdio_imask | imask);
	spin_unlock_irqrestore(&host->lock, iflags);

	mci_writel(host, REG_CARG, cmd->arg);
	mci_writel(host, REG_CMDR, cmd_val);
}

static const struct of_device_id sunxi_mmc_of_match[] = {
	{ .compatible = "allwinner,sun4i-mmc", },
	{ .compatible = "allwinner,sun5i-mmc", },
	{ /* sentinel */ }
};
MODULE_DEVICE_TABLE(of, sunxi_mmc_of_match);

static struct mmc_host_ops sunxi_mmc_ops = {
	.request	 = sunxi_mmc_request,
	.set_ios	 = sunxi_mmc_set_ios,
	.get_ro		 = sunxi_mmc_get_ro,
	.get_cd		 = sunxi_mmc_card_present,
	.enable_sdio_irq = sunxi_mmc_enable_sdio_irq,
	.hw_reset	 = sunxi_mmc_hw_reset,
};

static int sunxi_mmc_resource_request(struct sunxi_mmc_host *host,
				      struct platform_device *pdev)
{
	struct device_node *np = pdev->dev.of_node;
	int ret;

	if (of_device_is_compatible(np, "allwinner,sun4i-mmc"))
		host->idma_des_size_bits = 13;
	else
		host->idma_des_size_bits = 16;

	host->vmmc = devm_regulator_get_optional(&pdev->dev, "vmmc");
	if (IS_ERR(host->vmmc) && PTR_ERR(host->vmmc) == -EPROBE_DEFER)
		return -EPROBE_DEFER;

	host->reg_base = devm_ioremap_resource(&pdev->dev,
			      platform_get_resource(pdev, IORESOURCE_MEM, 0));
	if (IS_ERR(host->reg_base))
		return PTR_ERR(host->reg_base);

	host->irq = platform_get_irq(pdev, 0);
	ret = devm_request_irq(&pdev->dev, host->irq, sunxi_mmc_irq, 0,
			       "sunxi-mci", host);
	if (ret)
		return ret;
	disable_irq(host->irq);

	host->clk_ahb = devm_clk_get(&pdev->dev, "ahb");
	if (IS_ERR(host->clk_ahb)) {
		dev_err(&pdev->dev, "Could not get ahb clock\n");
		return PTR_ERR(host->clk_ahb);
	}

	host->clk_mod = devm_clk_get(&pdev->dev, "mod");
	if (IS_ERR(host->clk_mod)) {
		dev_err(&pdev->dev, "Could not get mod clock\n");
		return PTR_ERR(host->clk_mod);
	}

	of_property_read_u32(np, "bus-width", &host->bus_width);
	if (host->bus_width != 1 && host->bus_width != 4) {
		dev_err(&pdev->dev, "Invalid bus-width %d\n", host->bus_width);
		return -EINVAL;
	}

	of_property_read_u32(np, "cd-mode", &host->cd_mode);
	switch (host->cd_mode) {
	case CARD_DETECT_BY_GPIO_POLL:
		host->cd_pin = of_get_named_gpio(np, "cd-gpios", 0);
		if (!gpio_is_valid(host->cd_pin)) {
			dev_err(&pdev->dev, "Invalid cd-gpios\n");
			return -EINVAL;
		}
		ret = devm_gpio_request(&pdev->dev, host->cd_pin, "mmc_cd");
		if (ret) {
			dev_err(&pdev->dev, "Could not get cd-gpios\n");
			return ret;
		}
		gpio_direction_input(host->cd_pin);
		break;
	case CARD_ALWAYS_PRESENT:
		break;
	default:
		dev_err(&pdev->dev, "Invalid cd-mode %d\n", host->cd_mode);
		return -EINVAL;
	}

	host->wp_pin = of_get_named_gpio(np, "wp-gpios", 0);
	if (gpio_is_valid(host->wp_pin)) {
		ret = devm_gpio_request(&pdev->dev, host->wp_pin, "mmc_wp");
		if (ret) {
			dev_err(&pdev->dev, "Could not get wp-gpios\n");
			return ret;
		}
		gpio_direction_input(host->wp_pin);
	}

	return 0;
}

static int sunxi_mmc_probe(struct platform_device *pdev)
{
	struct sunxi_mmc_host *host;
	struct mmc_host *mmc;
	int ret;

	mmc = mmc_alloc_host(sizeof(struct sunxi_mmc_host), &pdev->dev);
	if (!mmc) {
		dev_err(&pdev->dev, "mmc alloc host failed\n");
		return -ENOMEM;
	}

	host = mmc_priv(mmc);
	host->mmc = mmc;
	spin_lock_init(&host->lock);
	tasklet_init(&host->tasklet, sunxi_mmc_tasklet, (unsigned long)host);

	ret = sunxi_mmc_resource_request(host, pdev);
	if (ret)
		goto error_free_host;

	host->sg_cpu = dma_alloc_coherent(&pdev->dev, PAGE_SIZE,
					  &host->sg_dma, GFP_KERNEL);
	if (!host->sg_cpu) {
		dev_err(&pdev->dev, "Failed to allocate DMA descriptor mem\n");
		ret = -ENOMEM;
		goto error_free_host;
	}

	mmc->ops		= &sunxi_mmc_ops;
	mmc->max_blk_count	= 8192;
	mmc->max_blk_size	= 4096;
	mmc->max_segs		= PAGE_SIZE / sizeof(struct sunxi_idma_des);
	mmc->max_seg_size	= (1 << host->idma_des_size_bits);
	mmc->max_req_size	= mmc->max_seg_size * mmc->max_segs;
	/* 400kHz ~ 50MHz */
	mmc->f_min		=   400000;
	mmc->f_max		= 50000000;
	/* available voltages */
	if (!IS_ERR(host->vmmc))
		mmc->ocr_avail = mmc_regulator_get_ocrmask(host->vmmc);
	else
		mmc->ocr_avail = MMC_VDD_32_33 | MMC_VDD_33_34;

	mmc->caps = MMC_CAP_MMC_HIGHSPEED | MMC_CAP_SD_HIGHSPEED |
		MMC_CAP_UHS_SDR12 | MMC_CAP_UHS_SDR25 | MMC_CAP_UHS_SDR50 |
		MMC_CAP_UHS_DDR50 | MMC_CAP_SDIO_IRQ | MMC_CAP_NEEDS_POLL |
		MMC_CAP_DRIVER_TYPE_A;
	if (host->bus_width == 4)
		mmc->caps |= MMC_CAP_4_BIT_DATA;
	mmc->caps2 = MMC_CAP2_NO_PRESCAN_POWERUP;

	ret = mmc_add_host(mmc);
	if (ret)
		goto error_free_dma;

	dev_info(&pdev->dev, "base:0x%p irq:%u\n", host->reg_base, host->irq);
	platform_set_drvdata(pdev, mmc);
	return 0;

error_free_dma:
	dma_free_coherent(&pdev->dev, PAGE_SIZE, host->sg_cpu, host->sg_dma);
error_free_host:
	mmc_free_host(mmc);
	return ret;
}

static int sunxi_mmc_remove(struct platform_device *pdev)
{
	struct mmc_host	*mmc = platform_get_drvdata(pdev);
	struct sunxi_mmc_host *host = mmc_priv(mmc);

	mmc_remove_host(mmc);
	sunxi_mmc_exit_host(host);
	tasklet_disable(&host->tasklet);
	dma_free_coherent(&pdev->dev, PAGE_SIZE, host->sg_cpu, host->sg_dma);
	mmc_free_host(mmc);

	return 0;
}

static struct platform_driver sunxi_mmc_driver = {
	.driver = {
		.name	= "sunxi-mci",
		.owner	= THIS_MODULE,
		.of_match_table = of_match_ptr(sunxi_mmc_of_match),
	},
	.probe		= sunxi_mmc_probe,
	.remove		= sunxi_mmc_remove,
};
module_platform_driver(sunxi_mmc_driver);

MODULE_DESCRIPTION("Allwinner's SD/MMC Card Controller Driver");
MODULE_LICENSE("GPL v2");
MODULE_AUTHOR("David Lanzendörfer <david.lanzendoerfer@o2s.ch>");
MODULE_ALIAS("platform:sunxi-mmc");

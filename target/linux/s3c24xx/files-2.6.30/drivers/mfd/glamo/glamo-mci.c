/*
 *  linux/drivers/mmc/host/glamo-mmc.c - Glamo MMC driver
 *
 *  Copyright (C) 2007 Openmoko, Inc,  Andy Green <andy@openmoko.com>
 *  Based on S3C MMC driver that was:
 *  Copyright (C) 2004-2006 maintech GmbH, Thomas Kleffel <tk@maintech.de>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <linux/module.h>
#include <linux/dma-mapping.h>
#include <linux/clk.h>
#include <linux/mmc/mmc.h>
#include <linux/mmc/host.h>
#include <linux/platform_device.h>
#include <linux/irq.h>
#include <linux/delay.h>
#include <linux/interrupt.h>
#include <linux/spinlock.h>
#include <linux/workqueue.h>

#include <asm/dma.h>
#include <asm/dma-mapping.h>
#include <asm/io.h>

#include "glamo-mci.h"
#include "glamo-core.h"
#include "glamo-regs.h"

/* from glamo-core.c */
extern struct glamo_mci_pdata glamo_mci_def_pdata;

static spinlock_t clock_lock;

#define DRIVER_NAME "glamo-mci"
#define RESSIZE(ressource) (((ressource)->end - (ressource)->start) + 1)

static void glamo_mci_send_request(struct mmc_host *mmc);

/*
 * Max SD clock rate
 *
 * held at /(3 + 1) due to concerns of 100R recommended series resistor
 * allows 16MHz @ 4-bit --> 8MBytes/sec raw
 *
 * you can override this on kernel commandline using
 *
 *   glamo_mci.sd_max_clk=10000000
 *
 * for example
 */

static int sd_max_clk = 50000000 / 3;
module_param(sd_max_clk, int, 0644);

/*
 * Slow SD clock rate
 *
 * you can override this on kernel commandline using
 *
 *   glamo_mci.sd_slow_ratio=8
 *
 * for example
 *
 * platform callback is used to decide effective clock rate, if not
 * defined then max is used, if defined and returns nonzero, rate is
 * divided by this factor
 */

static int sd_slow_ratio = 8;
module_param(sd_slow_ratio, int, 0644);

/*
 * Post-power SD clock rate
 *
 * you can override this on kernel commandline using
 *
 *   glamo_mci.sd_post_power_clock=1000000
 *
 * for example
 *
 * After changing power to card, clock is held at this rate until first bulk
 * transfer completes
 */

static int sd_post_power_clock = 1000000;
module_param(sd_post_power_clock, int, 0644);


/*
 * SD Signal drive strength
 *
 * you can override this on kernel commandline using
 *
 *   glamo_mci.sd_drive=0
 *
 * for example
 */

static int sd_drive;
module_param(sd_drive, int, 0644);

/*
 * SD allow SD clock to run while idle
 *
 * you can override this on kernel commandline using
 *
 *   glamo_mci.sd_idleclk=0
 *
 * for example
 */

static int sd_idleclk = 0; /* disallow idle clock by default */
module_param(sd_idleclk, int, 0644);

/* used to stash real idleclk state in suspend: we force it to run in there */
static int suspend_sd_idleclk;


unsigned char CRC7(u8 * pu8, int cnt)
{
	u8 crc = 0;

	while (cnt--) {
		int n;
		u8 d = *pu8++;
		for (n = 0; n < 8; n++) {
			crc <<= 1;
			if ((d & 0x80) ^ (crc & 0x80))
				crc ^= 0x09;
			d <<= 1;
		}
	}
	return (crc << 1) | 1;
}

static int get_data_buffer(struct glamo_mci_host *host,
			   volatile u32 *words, volatile u16 **pointer)
{
	struct scatterlist *sg;

	*words = 0;
	*pointer = NULL;

	if (host->pio_active == XFER_NONE)
		return -EINVAL;

	if ((!host->mrq) || (!host->mrq->data))
		return -EINVAL;

	if (host->pio_sgptr >= host->mrq->data->sg_len) {
		dev_dbg(&host->pdev->dev, "no more buffers (%i/%i)\n",
		      host->pio_sgptr, host->mrq->data->sg_len);
		return -EBUSY;
	}
	sg = &host->mrq->data->sg[host->pio_sgptr];

	*words = sg->length >> 1; /* we are working with a 16-bit data bus */
	*pointer = page_address(sg_page(sg)) + sg->offset;

	BUG_ON(((long)(*pointer)) & 1);

	host->pio_sgptr++;

	/* dev_info(&host->pdev->dev, "new buffer (%i/%i)\n",
	      host->pio_sgptr, host->mrq->data->sg_len); */
	return 0;
}

static void do_pio_read(struct glamo_mci_host *host)
{
	int res;
	u16 __iomem *from_ptr = host->base_data + (RESSIZE(host->mem_data) /
							      sizeof(u16) / 2);
#ifdef DEBUG
	u16 * block;
#endif

	while (1) {
		res = get_data_buffer(host, &host->pio_words, &host->pio_ptr);
		if (res) {
			host->pio_active = XFER_NONE;
			host->complete_what = COMPLETION_FINALIZE;

			dev_dbg(&host->pdev->dev, "pio_read(): "
				"complete (no more data).\n");
			return;
		}

		dev_dbg(&host->pdev->dev, "pio_read(): host->pio_words: %d\n",
				host->pio_words);

		host->pio_count += host->pio_words << 1;

#ifdef DEBUG
		block = (u16 *)host->pio_ptr;
		res = host->pio_words << 1;
#endif
#if 0
		/* u16-centric memcpy */
		while (host->pio_words--)
			*host->pio_ptr++ = *from_ptr++;
#else
		/* memcpy can be faster? */
		memcpy((void *)host->pio_ptr, from_ptr, host->pio_words << 1);
		host->pio_ptr += host->pio_words;
#endif

#ifdef DEBUG
		print_hex_dump(KERN_DEBUG, "", DUMP_PREFIX_OFFSET, 16, 1,
			       (void *)block, res, 1);
#endif
	}
}

static int do_pio_write(struct glamo_mci_host *host)
{
	int res = 0;
	volatile u16 __iomem *to_ptr = host->base_data;
	int err = 0;

	dev_dbg(&host->pdev->dev, "pio_write():\n");
	while (!res) {
		res = get_data_buffer(host, &host->pio_words, &host->pio_ptr);
		if (res)
			continue;

		dev_dbg(&host->pdev->dev, "pio_write():new source: [%i]@[%p]\n",
			host->pio_words, host->pio_ptr);

		host->pio_count += host->pio_words << 1;
		while (host->pio_words--)
			writew(*host->pio_ptr++, to_ptr++);
	}

	dev_dbg(&host->pdev->dev, "pio_write(): complete\n");
	host->pio_active = XFER_NONE;
	return err;
}

static void __glamo_mci_fix_card_div(struct glamo_mci_host *host, int div)
{
	unsigned long flags;

	spin_lock_irqsave(&clock_lock, flags);

	if (div < 0) {
		/* stop clock - remove clock from divider input */
		writew(readw(glamo_mci_def_pdata.pglamo->base +
		     GLAMO_REG_CLOCK_GEN5_1) & (~GLAMO_CLOCK_GEN51_EN_DIV_TCLK),
		     glamo_mci_def_pdata.pglamo->base + GLAMO_REG_CLOCK_GEN5_1);

		goto done;
	} else {
		/* set the nearest prescaler factor
		*
		* register shared with SCLK divisor -- no chance of race because
		* we don't use sensor interface
		*/
		writew((readw(glamo_mci_def_pdata.pglamo->base +
				GLAMO_REG_CLOCK_GEN8) & 0xff00) | div,
		       glamo_mci_def_pdata.pglamo->base + GLAMO_REG_CLOCK_GEN8);
		/* enable clock to divider input */
		writew(readw(glamo_mci_def_pdata.pglamo->base +
			GLAMO_REG_CLOCK_GEN5_1) | GLAMO_CLOCK_GEN51_EN_DIV_TCLK,
		     glamo_mci_def_pdata.pglamo->base + GLAMO_REG_CLOCK_GEN5_1);
	}

	if (host->force_slow_during_powerup)
		div = host->clk_rate / sd_post_power_clock;
	else
		if (host->pdata->glamo_mci_use_slow)
			if ((host->pdata->glamo_mci_use_slow)())
				div = div * sd_slow_ratio;

	if (div > 255)
		div = 255;

	/*
	 * set the nearest prescaler factor
	 *
	 * register shared with SCLK divisor -- no chance of race because
	 * we don't use sensor interface
	 */
	writew((readw(glamo_mci_def_pdata.pglamo->base +
			GLAMO_REG_CLOCK_GEN8) & 0xff00) | div,
		glamo_mci_def_pdata.pglamo->base + GLAMO_REG_CLOCK_GEN8);
	/* enable clock to divider input */
	writew(readw(glamo_mci_def_pdata.pglamo->base +
		GLAMO_REG_CLOCK_GEN5_1) | GLAMO_CLOCK_GEN51_EN_DIV_TCLK,
		glamo_mci_def_pdata.pglamo->base + GLAMO_REG_CLOCK_GEN5_1);

done:
	spin_unlock_irqrestore(&clock_lock, flags);
}

static int __glamo_mci_set_card_clock(struct glamo_mci_host *host, int freq,
								  int *division)
{
	int div = 0;
	int real_rate = 0;

	if (freq) {
		/* Set clock */
		for (div = 0; div < 256; div++) {
			real_rate = host->clk_rate / (div + 1);
			if (real_rate <= freq)
				break;
		}
		if (div > 255)
			div = 255;

		if (division)
			*division = div;

		__glamo_mci_fix_card_div(host, div);

	} else {
		/* stop clock */
		if (division)
			*division = 0xff;

		if (!sd_idleclk && !host->force_slow_during_powerup)
			/* clock off */
			__glamo_mci_fix_card_div(host, -1);
	}

	return real_rate;
}


static void glamo_mci_irq_worker(struct work_struct *work)
{
	struct glamo_mci_host *host =
			    container_of(work, struct glamo_mci_host, irq_work);
	struct mmc_command *cmd = host->mrq->cmd;

	if (host->pio_active == XFER_READ)
		do_pio_read(host);

	host->mrq->data->bytes_xfered = host->pio_count;
	dev_dbg(&host->pdev->dev, "count=%d\n", host->pio_count);

	/* issue STOP if we have been given one to use */
	if (host->mrq->stop) {
		host->cmd_is_stop = 1;
		glamo_mci_send_request(host->mmc);
		host->cmd_is_stop = 0;
	}

	if (!sd_idleclk && !host->force_slow_during_powerup)
		/* clock off */
		__glamo_mci_fix_card_div(host, -1);

	host->complete_what = COMPLETION_NONE;
	host->mrq = NULL;
	mmc_request_done(host->mmc, cmd->mrq);
}

static void glamo_mci_irq_host(struct glamo_mci_host *host)
{
	u16 status;
	struct mmc_command *cmd;
	unsigned long iflags;

	if (host->suspending) { /* bad news, dangerous time */
		dev_err(&host->pdev->dev, "****glamo_mci_irq before resumed\n");
		return;
	}

	if (!host->mrq)
		return;
	cmd = host->mrq->cmd;
	if (!cmd)
		return;

	spin_lock_irqsave(&host->complete_lock, iflags);

	status = readw(host->base + GLAMO_REG_MMC_RB_STAT1);
	dev_dbg(&host->pdev->dev, "status = 0x%04x\n", status);

	/* ack this interrupt source */
	writew(GLAMO_IRQ_MMC,
	       glamo_mci_def_pdata.pglamo->base + GLAMO_REG_IRQ_CLEAR);

	/* we ignore a data timeout report if we are also told the data came */
	if (status & GLAMO_STAT1_MMC_RB_DRDY)
		status &= ~GLAMO_STAT1_MMC_DTOUT;

	if (status & (GLAMO_STAT1_MMC_RTOUT |
		      GLAMO_STAT1_MMC_DTOUT))
		cmd->error = -ETIMEDOUT;
	if (status & (GLAMO_STAT1_MMC_BWERR |
		      GLAMO_STAT1_MMC_BRERR))
		cmd->error = -EILSEQ;
	if (cmd->error) {
		dev_info(&host->pdev->dev, "Error after cmd: 0x%x\n", status);
		goto done;
	}

	/*
	 * disable the initial slow start after first bulk transfer
	 */
	if (host->force_slow_during_powerup)
		host->force_slow_during_powerup--;

	/*
	 * we perform the memcpy out of Glamo memory outside of IRQ context
	 * so we don't block other interrupts
	 */
	schedule_work(&host->irq_work);

	goto leave;

done:
	host->complete_what = COMPLETION_NONE;
	host->mrq = NULL;
	mmc_request_done(host->mmc, cmd->mrq);
leave:
	spin_unlock_irqrestore(&host->complete_lock, iflags);
}

static void glamo_mci_irq(unsigned int irq, struct irq_desc *desc)
{
	struct glamo_mci_host *host = (struct glamo_mci_host *)
				      desc->handler_data;

	if (host)
		glamo_mci_irq_host(host);

}

static int glamo_mci_send_command(struct glamo_mci_host *host,
				  struct mmc_command *cmd)
{
	u8 u8a[6];
	u16 fire = 0;

	/* if we can't do it, reject as busy */
	if (!readw(host->base + GLAMO_REG_MMC_RB_STAT1) &
	     GLAMO_STAT1_MMC_IDLE) {
		host->mrq = NULL;
		cmd->error = -EBUSY;
		mmc_request_done(host->mmc, host->mrq);
		return -EBUSY;
	}

	/* create an array in wire order for CRC computation */
	u8a[0] = 0x40 | (cmd->opcode & 0x3f);
	u8a[1] = (u8)(cmd->arg >> 24);
	u8a[2] = (u8)(cmd->arg >> 16);
	u8a[3] = (u8)(cmd->arg >> 8);
	u8a[4] = (u8)cmd->arg;
	u8a[5] = CRC7(&u8a[0], 5); /* CRC7 on first 5 bytes of packet */

	/* issue the wire-order array including CRC in register order */
	writew((u8a[4] << 8) | u8a[5], host->base + GLAMO_REG_MMC_CMD_REG1);
	writew((u8a[2] << 8) | u8a[3], host->base + GLAMO_REG_MMC_CMD_REG2);
	writew((u8a[0] << 8) | u8a[1], host->base + GLAMO_REG_MMC_CMD_REG3);

	/* command index toggle */
	fire |= (host->ccnt & 1) << 12;

	/* set type of command */
	switch (mmc_cmd_type(cmd)) {
	case MMC_CMD_BC:
		fire |= GLAMO_FIRE_MMC_CMDT_BNR;
		break;
	case MMC_CMD_BCR:
		fire |= GLAMO_FIRE_MMC_CMDT_BR;
		break;
	case MMC_CMD_AC:
		fire |= GLAMO_FIRE_MMC_CMDT_AND;
		break;
	case MMC_CMD_ADTC:
		fire |= GLAMO_FIRE_MMC_CMDT_AD;
		break;
	}
	/*
	 * if it expects a response, set the type expected
	 *
	 * R1, Length  : 48bit, Normal response
	 * R1b, Length : 48bit, same R1, but added card busy status
	 * R2, Length  : 136bit (really 128 bits with CRC snipped)
	 * R3, Length  : 48bit (OCR register value)
	 * R4, Length  : 48bit, SDIO_OP_CONDITION, Reverse SDIO Card
	 * R5, Length  : 48bit, IO_RW_DIRECTION, Reverse SDIO Card
	 * R6, Length  : 48bit (RCA register)
	 * R7, Length  : 48bit (interface condition, VHS(voltage supplied),
	 *                     check pattern, CRC7)
	 */
	switch (mmc_resp_type(cmd)) {
	case MMC_RSP_R6: /* same index as R7 and R1 */
		fire |= GLAMO_FIRE_MMC_RSPT_R1;
		break;
	case MMC_RSP_R1B:
		fire |= GLAMO_FIRE_MMC_RSPT_R1b;
		break;
	case MMC_RSP_R2:
		fire |= GLAMO_FIRE_MMC_RSPT_R2;
		break;
	case MMC_RSP_R3:
		fire |= GLAMO_FIRE_MMC_RSPT_R3;
		break;
	/* R4 and R5 supported by chip not defined in linux/mmc/core.h (sdio) */
	}
	/*
	 * From the command index, set up the command class in the host ctrllr
	 *
	 * missing guys present on chip but couldn't figure out how to use yet:
	 *     0x0 "stream read"
	 *     0x9 "cancel running command"
	 */
	switch (cmd->opcode) {
	case MMC_READ_SINGLE_BLOCK:
		fire |= GLAMO_FIRE_MMC_CC_SBR; /* single block read */
		break;
	case MMC_SWITCH: /* 64 byte payload */
	case 0x33: /* observed issued by MCI */
	case MMC_READ_MULTIPLE_BLOCK:
		/* we will get an interrupt off this */
		if (!cmd->mrq->stop)
			/* multiblock no stop */
			fire |= GLAMO_FIRE_MMC_CC_MBRNS;
		else
			 /* multiblock with stop */
			fire |= GLAMO_FIRE_MMC_CC_MBRS;
		break;
	case MMC_WRITE_BLOCK:
		fire |= GLAMO_FIRE_MMC_CC_SBW; /* single block write */
		break;
	case MMC_WRITE_MULTIPLE_BLOCK:
		if (cmd->mrq->stop)
			 /* multiblock with stop */
			fire |= GLAMO_FIRE_MMC_CC_MBWS;
		else
// 			 /* multiblock NO stop-- 'RESERVED'? */
			fire |= GLAMO_FIRE_MMC_CC_MBWNS;
		break;
	case MMC_STOP_TRANSMISSION:
		fire |= GLAMO_FIRE_MMC_CC_STOP; /* STOP */
		break;
	default:
		fire |= GLAMO_FIRE_MMC_CC_BASIC; /* "basic command" */
		break;
	}

	/* always largest timeout */
	writew(0xfff, host->base + GLAMO_REG_MMC_TIMEOUT);

	/* Generate interrupt on txfer */
	writew((readw(host->base + GLAMO_REG_MMC_BASIC) & 0x3e) |
		   0x0800 | GLAMO_BASIC_MMC_NO_CLK_RD_WAIT |
		   GLAMO_BASIC_MMC_EN_COMPL_INT | (sd_drive << 6),
		   host->base + GLAMO_REG_MMC_BASIC);

	/* send the command out on the wire */
	/* dev_info(&host->pdev->dev, "Using FIRE %04X\n", fire); */
	writew(fire, host->base + GLAMO_REG_MMC_CMD_FIRE);
	cmd->error = 0;
	return 0;
}

static int glamo_mci_prepare_pio(struct glamo_mci_host *host,
				 struct mmc_data *data)
{
	/*
	 * the S-Media-internal RAM offset for our MMC buffer
	 * Read is halfway up the buffer and write is at the start
	 */
	if (data->flags & MMC_DATA_READ) {
		writew((u16)(GLAMO_FB_SIZE + (RESSIZE(host->mem_data) / 2)),
			   host->base + GLAMO_REG_MMC_WDATADS1);
		writew((u16)((GLAMO_FB_SIZE +
					(RESSIZE(host->mem_data) / 2)) >> 16),
			   host->base + GLAMO_REG_MMC_WDATADS2);
	} else {
		writew((u16)GLAMO_FB_SIZE, host->base +
					       GLAMO_REG_MMC_RDATADS1);
		writew((u16)(GLAMO_FB_SIZE >> 16), host->base +
						       GLAMO_REG_MMC_RDATADS2);
	}

	/* set up the block info */
	writew(data->blksz, host->base + GLAMO_REG_MMC_DATBLKLEN);
	writew(data->blocks, host->base + GLAMO_REG_MMC_DATBLKCNT);
	dev_dbg(&host->pdev->dev, "(blksz=%d, count=%d)\n",
				   data->blksz, data->blocks);
	host->pio_sgptr = 0;
	host->pio_words = 0;
	host->pio_count = 0;
	host->pio_active = 0;
	/* if write, prep the write into the shared RAM before the command */
	if (data->flags & MMC_DATA_WRITE) {
		host->pio_active = XFER_WRITE;
		return do_pio_write(host);
	}
	host->pio_active = XFER_READ;
	return 0;
}

static void glamo_mci_send_request(struct mmc_host *mmc)
{
	struct glamo_mci_host *host = mmc_priv(mmc);
	struct mmc_request *mrq = host->mrq;
	struct mmc_command *cmd = host->cmd_is_stop ? mrq->stop : mrq->cmd;
	u16 * pu16 = (u16 *)&cmd->resp[0];
	u16 * reg_resp = (u16 *)(host->base + GLAMO_REG_MMC_CMD_RSP1);
	u16 status;
	int n;
	int timeout = 1000000;
	int insanity_timeout = 1000000;

	if (host->suspending) {
		dev_err(&host->pdev->dev, "IGNORING glamo_mci_send_request while "
								 "suspended\n");
		cmd->error = -EIO;
		if (cmd->data)
			cmd->data->error = -EIO;
		mmc_request_done(mmc, mrq);
		return;
	}

	host->ccnt++;
	/*
	 * somehow 2.6.24 MCI manages to issue MMC_WRITE_BLOCK *without* the
	 * MMC_DATA_WRITE flag, WTF?  Work around the madness.
	 */
	if (cmd->opcode == MMC_WRITE_BLOCK)
		if (mrq->data)
			mrq->data->flags |= MMC_DATA_WRITE;

	 /* this guy has data to read/write? */
	if ((!host->cmd_is_stop) && cmd->data) {
		int res;
		host->dcnt++;
		res = glamo_mci_prepare_pio(host, cmd->data);
		if (res) {
			cmd->error = -EIO;
			cmd->data->error = -EIO;
			mmc_request_done(mmc, mrq);
			return;
		}
	}

	dev_dbg(&host->pdev->dev,"cmd 0x%x, "
		 "arg 0x%x data=%p mrq->stop=%p flags 0x%x\n",
		 cmd->opcode, cmd->arg, cmd->data, cmd->mrq->stop,
		 cmd->flags);

	/* resume requested clock rate
	 * scale it down by sd_slow_ratio if platform requests it
	 */
	__glamo_mci_fix_card_div(host, host->clk_div);

	if (glamo_mci_send_command(host, cmd))
		goto bail;

	/* we are deselecting card?  because it isn't going to ack then... */
	if ((cmd->opcode == 7) && (cmd->arg == 0))
		goto done;

	/*
	 * we must spin until response is ready or timed out
	 * -- we don't get interrupts unless there is a bulk rx
	 */
	do
		status = readw(host->base + GLAMO_REG_MMC_RB_STAT1);
	while (((((status >> 15) & 1) != (host->ccnt & 1)) ||
		(!(status & (GLAMO_STAT1_MMC_RB_RRDY |
			     GLAMO_STAT1_MMC_RTOUT |
			     GLAMO_STAT1_MMC_DTOUT |
			     GLAMO_STAT1_MMC_BWERR |
			     GLAMO_STAT1_MMC_BRERR)))) && (insanity_timeout--));

	if (insanity_timeout < 0)
		dev_info(&host->pdev->dev, "command timeout, continuing\n");

	if (status & (GLAMO_STAT1_MMC_RTOUT |
		      GLAMO_STAT1_MMC_DTOUT))
		cmd->error = -ETIMEDOUT;
	if (status & (GLAMO_STAT1_MMC_BWERR |
		      GLAMO_STAT1_MMC_BRERR))
		cmd->error = -EILSEQ;

	if (host->cmd_is_stop)
		goto bail;

	if (cmd->error) {
		dev_info(&host->pdev->dev, "Error after cmd: 0x%x\n", status);
		goto done;
	}
	/*
	 * mangle the response registers in two different exciting
	 * undocumented ways discovered by trial and error
	 */
	if (mmc_resp_type(cmd) == MMC_RSP_R2)
		/* grab the response */
		for (n = 0; n < 8; n++) /* super mangle power 1 */
			pu16[n ^ 6] = readw(&reg_resp[n]);
	else
		for (n = 0; n < 3; n++) /* super mangle power 2 */
			pu16[n] = (readw(&reg_resp[n]) >> 8) |
				  (readw(&reg_resp[n + 1]) << 8);
	/*
	 * if we don't have bulk data to take care of, we're done
	 */
	if (!cmd->data)
		goto done;
	if (!(cmd->data->flags & (MMC_DATA_READ | MMC_DATA_WRITE)))
		goto done;

	/*
	 * Otherwise can can use the interrupt as async completion --
	 * if there is read data coming, or we wait for write data to complete,
	 * exit without mmc_request_done() as the payload interrupt
	 * will service it
	 */
	dev_dbg(&host->pdev->dev, "Waiting for payload data\n");
	/*
	 * if the glamo INT# line isn't wired (*cough* it can happen)
	 * I'm afraid we have to spin on the IRQ status bit and "be
	 * our own INT# line"
	 */
	if (!glamo_mci_def_pdata.pglamo->irq_works) {
		/*
		 * we have faith we will get an "interrupt"...
		 * but something insane like suspend problems can mean
		 * we spin here forever, so we timeout after a LONG time
		 */
		while ((!(readw(glamo_mci_def_pdata.pglamo->base +
			 GLAMO_REG_IRQ_STATUS) & GLAMO_IRQ_MMC)) &&
		       (timeout--))
			;

		if (timeout < 0) {
			if (cmd->data->error)
				cmd->data->error = -ETIMEDOUT;
			dev_err(&host->pdev->dev, "Payload timeout\n");
			goto bail;
		}

		/* yay we are an interrupt controller! -- call the ISR
		 * it will stop clock to card
		 */
		glamo_mci_irq_host(host);
	}
	return;

done:
	host->complete_what = COMPLETION_NONE;
	host->mrq = NULL;
	mmc_request_done(host->mmc, cmd->mrq);
bail:
	if (!sd_idleclk && !host->force_slow_during_powerup)
		/* stop the clock to card */
		__glamo_mci_fix_card_div(host, -1);
}

static void glamo_mci_request(struct mmc_host *mmc, struct mmc_request *mrq)
{
	struct glamo_mci_host *host = mmc_priv(mmc);

	host->cmd_is_stop = 0;
	host->mrq = mrq;
	glamo_mci_send_request(mmc);
}

#if 1
static void glamo_mci_reset(struct glamo_mci_host *host)
{
	if (host->suspending) {
		dev_err(&host->pdev->dev, "IGNORING glamo_mci_reset while "
								 "suspended\n");
		return;
	}
	dev_dbg(&host->pdev->dev, "******* glamo_mci_reset\n");
	/* reset MMC controller */
	writew(GLAMO_CLOCK_MMC_RESET | GLAMO_CLOCK_MMC_DG_TCLK |
		   GLAMO_CLOCK_MMC_EN_TCLK | GLAMO_CLOCK_MMC_DG_M9CLK |
		   GLAMO_CLOCK_MMC_EN_M9CLK,
		   glamo_mci_def_pdata.pglamo->base + GLAMO_REG_CLOCK_MMC);
	udelay(10);
	/* and disable reset */
	writew(GLAMO_CLOCK_MMC_DG_TCLK |
		   GLAMO_CLOCK_MMC_EN_TCLK | GLAMO_CLOCK_MMC_DG_M9CLK |
		   GLAMO_CLOCK_MMC_EN_M9CLK,
		   glamo_mci_def_pdata.pglamo->base + GLAMO_REG_CLOCK_MMC);
}
#endif
static inline int glamo_mci_get_mv(int vdd)
{
	int mv = 1650;

	if (vdd > 7)
		mv += 350 + 100 * (vdd - 8);

	return mv;
}

static void glamo_mci_set_ios(struct mmc_host *mmc, struct mmc_ios *ios)
{
	struct glamo_mci_host *host = mmc_priv(mmc);
	struct regulator *regulator;
	int n = 0;
	int div;
	int powering = 0;
	int mv;

	if (host->suspending) {
		dev_err(&host->pdev->dev, "IGNORING glamo_mci_set_ios while "
								 "suspended\n");
		return;
	}

	regulator = host->regulator;

	/* Set power */
	switch(ios->power_mode) {
	case MMC_POWER_UP:
		if (host->pdata->glamo_can_set_mci_power()) {
			mv = glamo_mci_get_mv(ios->vdd);
			regulator_set_voltage(regulator, mv * 1000, mv * 1000);
			regulator_enable(regulator);
		}
		break;
	case MMC_POWER_ON:
		/*
		 * we should use very slow clock until first bulk
		 * transfer completes OK
		 */
		host->force_slow_during_powerup = 1;

		if (host->vdd_current != ios->vdd) {
			if (host->pdata->glamo_can_set_mci_power()) {
				mv = glamo_mci_get_mv(ios->vdd);
				regulator_set_voltage(regulator, mv * 1000, mv * 1000);
				printk(KERN_INFO "SD power -> %dmV\n", mv);
			}
			host->vdd_current = ios->vdd;
		}
		if (host->power_mode_current == MMC_POWER_OFF) {
			glamo_engine_enable(glamo_mci_def_pdata.pglamo,
							      GLAMO_ENGINE_MMC);
			powering = 1;
		}
		break;

	case MMC_POWER_OFF:
	default:
		if (host->power_mode_current == MMC_POWER_OFF)
			break;
		/* never want clocking with dead card */
		__glamo_mci_fix_card_div(host, -1);

		glamo_engine_disable(glamo_mci_def_pdata.pglamo,
				     GLAMO_ENGINE_MMC);
		regulator_disable(regulator);
		host->vdd_current = -1;
		break;
	}
	host->power_mode_current = ios->power_mode;

	host->real_rate = __glamo_mci_set_card_clock(host, ios->clock, &div);
	host->clk_div = div;

	/* after power-up, we are meant to give it >= 74 clocks so it can
	 * initialize itself.  Doubt any modern cards need it but anyway...
	 */
	if (powering)
		mdelay(1);

	if (!sd_idleclk && !host->force_slow_during_powerup)
		/* stop the clock to card, because we are idle until transfer */
		__glamo_mci_fix_card_div(host, -1);

	if ((ios->power_mode == MMC_POWER_ON) ||
	    (ios->power_mode == MMC_POWER_UP)) {
		dev_info(&host->pdev->dev,
			"powered (vdd = %d) clk: %lukHz div=%d (req: %ukHz). "
			"Bus width=%d\n",(int)ios->vdd,
			host->real_rate / 1000, (int)host->clk_div,
			ios->clock / 1000, (int)ios->bus_width);
	} else
		dev_info(&host->pdev->dev, "glamo_mci_set_ios: power down.\n");

	/* set bus width */
	host->bus_width = ios->bus_width;
	if (host->bus_width == MMC_BUS_WIDTH_4)
		n = GLAMO_BASIC_MMC_EN_4BIT_DATA;
	writew((readw(host->base + GLAMO_REG_MMC_BASIC) &
					  (~(GLAMO_BASIC_MMC_EN_4BIT_DATA |
					     GLAMO_BASIC_MMC_EN_DR_STR0 |
					     GLAMO_BASIC_MMC_EN_DR_STR1))) | n |
			       sd_drive << 6, host->base + GLAMO_REG_MMC_BASIC);
}


/*
 * no physical write protect supported by us
 */
static int glamo_mci_get_ro(struct mmc_host *mmc)
{
	return 0;
}

static struct mmc_host_ops glamo_mci_ops = {
	.request	= glamo_mci_request,
	.set_ios	= glamo_mci_set_ios,
	.get_ro		= glamo_mci_get_ro,
};

static int glamo_mci_probe(struct platform_device *pdev)
{
	struct mmc_host 	*mmc;
	struct glamo_mci_host 	*host;
	int ret;

	dev_info(&pdev->dev, "glamo_mci driver (C)2007 Openmoko, Inc\n");

	mmc = mmc_alloc_host(sizeof(struct glamo_mci_host), &pdev->dev);
	if (!mmc) {
		ret = -ENOMEM;
		goto probe_out;
	}

	host = mmc_priv(mmc);
	host->mmc = mmc;
	host->pdev = pdev;
	host->pdata = &glamo_mci_def_pdata;
	host->power_mode_current = MMC_POWER_OFF;

	host->complete_what = COMPLETION_NONE;
	host->pio_active = XFER_NONE;

	spin_lock_init(&host->complete_lock);
	INIT_WORK(&host->irq_work, glamo_mci_irq_worker);

	host->mem = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if (!host->mem) {
		dev_err(&pdev->dev,
			"failed to get io memory region resouce.\n");

		ret = -ENOENT;
		goto probe_free_host;
	}

	host->mem = request_mem_region(host->mem->start,
		RESSIZE(host->mem), pdev->name);

	if (!host->mem) {
		dev_err(&pdev->dev, "failed to request io memory region.\n");
		ret = -ENOENT;
		goto probe_free_host;
	}

	host->base = ioremap(host->mem->start, RESSIZE(host->mem));
	if (!host->base) {
		dev_err(&pdev->dev, "failed to ioremap() io memory region.\n");
		ret = -EINVAL;
		goto probe_free_mem_region;
	}

	host->regulator = regulator_get(&pdev->dev, "SD_3V3");
	if (!host->regulator) {
		dev_err(&pdev->dev, "Cannot proceed without regulator.\n");
		return -ENODEV;
	}

	/* set the handler for our bit of the shared chip irq register */
	set_irq_handler(IRQ_GLAMO(GLAMO_IRQIDX_MMC), glamo_mci_irq);
	/* stash host as our handler's private data */
	set_irq_data(IRQ_GLAMO(GLAMO_IRQIDX_MMC), host);

	/* Get ahold of our data buffer we use for data in and out on MMC */
	host->mem_data = platform_get_resource(pdev, IORESOURCE_MEM, 1);
	if (!host->mem_data) {
		dev_err(&pdev->dev,
			"failed to get io memory region resource.\n");
		ret = -ENOENT;
		goto probe_iounmap;
	}

	host->mem_data = request_mem_region(host->mem_data->start,
		RESSIZE(host->mem_data), pdev->name);

	if (!host->mem_data) {
		dev_err(&pdev->dev, "failed to request io memory region.\n");
		ret = -ENOENT;
		goto probe_iounmap;
	}
	host->base_data = ioremap(host->mem_data->start,
					  RESSIZE(host->mem_data));
	host->data_max_size = RESSIZE(host->mem_data);

	if (host->base_data == 0) {
		dev_err(&pdev->dev, "failed to ioremap() io memory region.\n");
		ret = -EINVAL;
		goto probe_free_mem_region_data;
	}

	host->vdd_current = 0;
	host->clk_rate = 50000000; /* really it's 49152000 */
	host->clk_div = 16;

	/* explain our host controller capabilities */
	mmc->ops 	= &glamo_mci_ops;
	mmc->ocr_avail	= host->pdata->ocr_avail;
	mmc->caps	= MMC_CAP_4_BIT_DATA |
			  MMC_CAP_MMC_HIGHSPEED |
			  MMC_CAP_SD_HIGHSPEED;
	mmc->f_min 	= host->clk_rate / 256;
	mmc->f_max 	= sd_max_clk;

	mmc->max_blk_count	= (1 << 16) - 1; /* GLAMO_REG_MMC_RB_BLKCNT */
	mmc->max_blk_size	= (1 << 12) - 1; /* GLAMO_REG_MMC_RB_BLKLEN */
	mmc->max_req_size	= RESSIZE(host->mem_data) / 2;
	mmc->max_seg_size	= mmc->max_req_size;
	mmc->max_phys_segs	= 1; /* hw doesn't talk about segs??? */
	mmc->max_hw_segs	= 1;

	dev_info(&host->pdev->dev, "probe: mapped mci_base:%p irq:%u.\n",
		host->base, host->irq);

	platform_set_drvdata(pdev, mmc);

	glamo_engine_enable(glamo_mci_def_pdata.pglamo, GLAMO_ENGINE_MMC);
	glamo_mci_reset(host);

	if ((ret = mmc_add_host(mmc))) {
		dev_err(&pdev->dev, "failed to add mmc host.\n");
		goto probe_free_mem_region_data;
	}

	dev_info(&pdev->dev,"initialisation done.\n");
	return 0;

 probe_free_mem_region_data:
	release_mem_region(host->mem_data->start, RESSIZE(host->mem_data));

 probe_iounmap:
	iounmap(host->base);

 probe_free_mem_region:
	release_mem_region(host->mem->start, RESSIZE(host->mem));

 probe_free_host:
	mmc_free_host(mmc);
 probe_out:
	return ret;
}

static int glamo_mci_remove(struct platform_device *pdev)
{
	struct mmc_host 	*mmc  = platform_get_drvdata(pdev);
	struct glamo_mci_host 	*host = mmc_priv(mmc);
	struct regulator *regulator;

	mmc_remove_host(mmc);
	/* stop using our handler, revert it to default */
	set_irq_handler(IRQ_GLAMO(GLAMO_IRQIDX_MMC), handle_level_irq);
	iounmap(host->base);
	iounmap(host->base_data);
	release_mem_region(host->mem->start, RESSIZE(host->mem));
	release_mem_region(host->mem_data->start, RESSIZE(host->mem_data));

	regulator = host->regulator;
	regulator_put(regulator);
	
	mmc_free_host(mmc);

	glamo_engine_disable(glamo_mci_def_pdata.pglamo, GLAMO_ENGINE_MMC);
	return 0;
}


#ifdef CONFIG_PM

static int glamo_mci_suspend(struct platform_device *dev, pm_message_t state)
{
	struct mmc_host *mmc = platform_get_drvdata(dev);
	struct glamo_mci_host 	*host = mmc_priv(mmc);
	int ret;

	cancel_work_sync(&host->irq_work);

	/*
	 * possible workaround for SD corruption during suspend - resume
	 * make sure the clock was running during suspend and consequently
	 * resume
	 */
	__glamo_mci_fix_card_div(host, host->clk_div);

	/* we are going to do more commands to override this in
	 * mmc_suspend_host(), so we need to change sd_idleclk for the
	 * duration as well
	 */
	suspend_sd_idleclk = sd_idleclk;
	sd_idleclk = 1;

	ret = mmc_suspend_host(mmc, state);

	host->suspending++;
	/* so that when we resume, we use any modified max rate */
	mmc->f_max = sd_max_clk;

	return ret;
}

int glamo_mci_resume(struct platform_device *dev)
{
	struct mmc_host *mmc = platform_get_drvdata(dev);
	struct glamo_mci_host 	*host = mmc_priv(mmc);
	int ret;

	sd_idleclk = 1;

	glamo_engine_enable(host->pdata->pglamo, GLAMO_ENGINE_MMC);
	glamo_mci_reset(host);

	host->suspending--;

	ret = mmc_resume_host(mmc);

	/* put sd_idleclk back to pre-suspend state */
	sd_idleclk = suspend_sd_idleclk;

	return ret;
}
EXPORT_SYMBOL_GPL(glamo_mci_resume);

#else /* CONFIG_PM */
#define glamo_mci_suspend NULL
#define glamo_mci_resume NULL
#endif /* CONFIG_PM */


static struct platform_driver glamo_mci_driver =
{
	.driver.name	= "glamo-mci",
	.probe		= glamo_mci_probe,
	.remove		= glamo_mci_remove,
	.suspend	= glamo_mci_suspend,
	.resume		= glamo_mci_resume,
};

static int __init glamo_mci_init(void)
{
	spin_lock_init(&clock_lock);
	platform_driver_register(&glamo_mci_driver);
	return 0;
}

static void __exit glamo_mci_exit(void)
{
	platform_driver_unregister(&glamo_mci_driver);
}

module_init(glamo_mci_init);
module_exit(glamo_mci_exit);

MODULE_DESCRIPTION("Glamo MMC/SD Card Interface driver");
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Andy Green <andy@openmoko.com>");

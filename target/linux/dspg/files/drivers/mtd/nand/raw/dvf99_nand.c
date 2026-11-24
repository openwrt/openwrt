// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (C) 2026 John Crispin <john@phrozen.org>
 */

#include <linux/bitfield.h>
#include <linux/bits.h>
#include <linux/clk.h>
#include <linux/completion.h>
#include <linux/dma-mapping.h>
#include <linux/interrupt.h>
#include <linux/io.h>
#include <linux/iopoll.h>
#include <linux/module.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/rawnand.h>
#include <linux/of.h>
#include <linux/platform_device.h>
#include <linux/reset.h>
#include <linux/sizes.h>
#include <linux/spinlock.h>

#define DVF99_NFC_CTL			0x00
#define DVF99_NFC_STATUS		0x04
#define DVF99_NFC_STS_CLR		0x08
#define DVF99_NFC_INT_EN		0x0c
#define DVF99_NFC_INT_CAUSE		0x10
#define DVF99_NFC_SEQUENCE		0x14
#define DVF99_NFC_ADDR_COL		0x18
#define DVF99_NFC_ADDR_ROW		0x1c
#define DVF99_NFC_CMD1			0x20
#define DVF99_NFC_CMD2			0x24
#define DVF99_NFC_WAIT1			0x28
#define DVF99_NFC_WAIT2			0x2c
#define DVF99_NFC_WAIT3			0x30
#define DVF99_NFC_WAIT4			0x34
#define DVF99_NFC_PULSETIME		0x38
#define DVF99_NFC_DCOUNT		0x3c
#define DVF99_NFC_FTSR			0x40
#define DVF99_NFC_DPR			0x44
#define DVF99_NFC_RDPR			0x48
#define DVF99_NFC_TIMEOUT		0x4c
#define DVF99_NFC_BCH			0x50
#define DVF99_NFC_START			0x58
#define DVF99_NFC_FBYP_DATA		0x60
#define DVF99_NFC_CHIEN(n)		(0x68 + (n) * 4)

#define DVF99_NFC_CTL_ECC_MODE		GENMASK(3, 0)
#define DVF99_NFC_CTL_CHIEN_START	GENMASK(28, 16)

#define DVF99_NFC_ST_TRANS_DONE		BIT(0)
#define DVF99_NFC_ST_ECC_DONE		BIT(1)
#define DVF99_NFC_ST_ERR_FOUND		BIT(2)
#define DVF99_NFC_ST_TRANS_BUSY		BIT(6)
#define DVF99_NFC_ST_ECC_BUSY		BIT(7)
#define DVF99_NFC_ST_RDY(n)		BIT(8 + (n))
#define DVF99_NFC_ST_ECC_NC_ERR		BIT(11)
#define DVF99_NFC_ST_AHB_ERR		BIT(16)
#define DVF99_NFC_ST_AHB_DONE		BIT(17)

#define DVF99_NFC_SEQ_CMD1		BIT(0)
#define DVF99_NFC_SEQ_WAIT0		BIT(1)
#define DVF99_NFC_SEQ_ADDR(n)		BIT(2 + (n))
#define DVF99_NFC_SEQ_WAIT1		BIT(7)
#define DVF99_NFC_SEQ_CMD2		BIT(8)
#define DVF99_NFC_SEQ_WAIT2		BIT(9)
#define DVF99_NFC_SEQ_RW		BIT(10)
#define DVF99_NFC_SEQ_RW_WRITE		BIT(11)
#define DVF99_NFC_SEQ_ECC		BIT(12)
#define DVF99_NFC_SEQ_CMD3		BIT(13)
#define DVF99_NFC_SEQ_WAIT3		BIT(14)
#define DVF99_NFC_SEQ_CMD4		BIT(15)
#define DVF99_NFC_SEQ_WAIT4		BIT(16)
#define DVF99_NFC_SEQ_READ_ONCE		BIT(17)
#define DVF99_NFC_SEQ_CS		GENMASK(19, 18)
#define DVF99_NFC_SEQ_RDY_EN		BIT(22)
#define DVF99_NFC_SEQ_RDY_SEL		BIT(23)
#define DVF99_NFC_SEQ_CMD5		BIT(24)
#define DVF99_NFC_SEQ_WAIT5		BIT(25)
#define DVF99_NFC_SEQ_CMD6		BIT(26)
#define DVF99_NFC_SEQ_WAIT6		BIT(27)

#define DVF99_NFC_CMD_FIRST		GENMASK(7, 0)
#define DVF99_NFC_CMD_SECOND		GENMASK(15, 8)
#define DVF99_NFC_CMD_THIRD		GENMASK(23, 16)
#define DVF99_NFC_CMD_FOURTH		GENMASK(31, 24)

#define DVF99_NFC_WAIT_LO_A		GENMASK(5, 0)
#define DVF99_NFC_WAIT_LO_B		GENMASK(13, 8)
#define DVF99_NFC_WAIT_HI_A		GENMASK(21, 16)
#define DVF99_NFC_WAIT_HI_B		GENMASK(29, 24)

#define DVF99_NFC_PT_RD_LOW		GENMASK(3, 0)
#define DVF99_NFC_PT_RD_HIGH		GENMASK(7, 4)
#define DVF99_NFC_PT_WR_LOW		GENMASK(11, 8)
#define DVF99_NFC_PT_WR_HIGH		GENMASK(15, 12)
#define DVF99_NFC_PT_CLE_SETUP		GENMASK(19, 16)
#define DVF99_NFC_PT_ALE_SETUP		GENMASK(23, 20)
#define DVF99_NFC_PT_SPARE_WORDS	GENMASK(29, 24)

#define DVF99_NFC_DCOUNT_VP_SIZE	GENMASK(12, 0)
#define DVF99_NFC_DCOUNT_N3		GENMASK(15, 13)
#define DVF99_NFC_DCOUNT_N2		GENMASK(23, 16)
#define DVF99_NFC_DCOUNT_N1		GENMASK(27, 24)
#define DVF99_NFC_DCOUNT_PAGE_CNT	GENMASK(31, 28)

#define DVF99_NFC_FTSR_SIZE		GENMASK(12, 0)

#define DVF99_NFC_BCH_LOC		GENMASK(12, 0)
#define DVF99_NFC_BCH_STEP		GENMASK(16, 13)

#define DVF99_NFC_INT_MASK		(DVF99_NFC_ST_TRANS_DONE | \
					 DVF99_NFC_ST_ECC_DONE | \
					 DVF99_NFC_ST_ERR_FOUND | \
					 DVF99_NFC_ST_ECC_NC_ERR | \
					 DVF99_NFC_ST_AHB_ERR | \
					 DVF99_NFC_ST_AHB_DONE)

#define DVF99_NFC_WAIT_CYCLES		4
#define DVF99_NFC_PULSE_CYCLES		4
#define DVF99_NFC_CHIEN_COUNT_START	0x0e6c
#define DVF99_NFC_STEP_SIZE		512
#define DVF99_NFC_MAX_STEPS		16
#define DVF99_NFC_MAX_XFER		DVF99_NFC_FTSR_SIZE
#define DVF99_NFC_BUF_SIZE		SZ_8K
#define DVF99_NFC_TIMEOUT_MS		1000
#define DVF99_NFC_IDLE_TIMEOUT_US	100000
#define DVF99_NFC_HAMMING_BYTES		3
#define DVF99_NFC_BCH_MIN_OOBSIZE		64

#define DVF99_ROM_REV1_OFS		0x5464
#define DVF99_ROM_REV1_TAG		0x0a302e31
#define DVF99_ROM_REV2_OFS		0x112e4
#define DVF99_ROM_REV2_TAG		0x0a302e32

#define DVF99_MTD_NAME			"dmw_nand"

enum dvf99_nfc_ecc_mode {
	DVF99_NFC_ECC_HAMMING,
	DVF99_NFC_ECC_BCH4,
	DVF99_NFC_ECC_BCH8,
	DVF99_NFC_ECC_BCH16,
};

static const unsigned int dvf99_nfc_ecc_bits[] = {
	[DVF99_NFC_ECC_HAMMING] = 1,
	[DVF99_NFC_ECC_BCH4] = 4,
	[DVF99_NFC_ECC_BCH8] = 8,
	[DVF99_NFC_ECC_BCH16] = 16,
};

static const u16 dvf99_nfc_chien_codes[] = {
	0x0b19, 0x14e2, 0x0d8d, 0x01e8, 0x0d7d, 0x1b24, 0x1351, 0x14ae,
	0x1ea6, 0x0e12, 0x1512, 0x09e9, 0x1365, 0x0ae3, 0x1e4a, 0x11b8,
	0x14a1, 0x11ed, 0x1c13, 0x0ce5, 0x0d18, 0x02c4, 0x11b9, 0x0fbc,
	0x0d62, 0x1a03, 0x0772, 0x1346, 0x1980, 0x1df5, 0x1140, 0x1d45,
};

struct dvf99_nfc {
	struct nand_controller controller;
	struct nand_chip chip;
	struct device *dev;
	void __iomem *regs;
	struct reset_control *rst;
	struct completion done;
	spinlock_t lock;
	u8 *buf;
	dma_addr_t buf_dma;
	unsigned int cs;
	unsigned int rb;
	bool legacy_layout;

	u32 irq_wait;
	u32 irq_seen;
	bool armed;
	bool ahb_err;
	bool dead;

	enum dvf99_nfc_ecc_mode ecc_mode;
	unsigned int dirty_pos;
	unsigned int n1;
	unsigned int n2;
	unsigned int n3;
	unsigned int spare_words;
	u8 *raw_buf;
	u32 *ecc_codes;
	unsigned int ecc_ncodes;
	unsigned int ecc_maxcodes;
	bool ecc_uncorrectable;
};

static struct dvf99_nfc *dvf99_nfc_from_chip(struct nand_chip *chip)
{
	return container_of(chip->controller, struct dvf99_nfc, controller);
}

static void dvf99_nfc_ecc_drain(struct dvf99_nfc *nfc)
{
	u32 code;

	lockdep_assert_held(&nfc->lock);

	if (readl(nfc->regs + DVF99_NFC_STATUS) & DVF99_NFC_ST_ECC_NC_ERR) {
		nfc->ecc_uncorrectable = true;
		writel(DVF99_NFC_ST_ECC_NC_ERR, nfc->regs + DVF99_NFC_STS_CLR);
	}

	while (readl(nfc->regs + DVF99_NFC_STATUS) & DVF99_NFC_ST_ERR_FOUND) {
		code = readl(nfc->regs + DVF99_NFC_BCH);
		if (nfc->ecc_ncodes < nfc->ecc_maxcodes)
			nfc->ecc_codes[nfc->ecc_ncodes++] = code;
		else
			nfc->ecc_uncorrectable = true;
	}
}

static irqreturn_t dvf99_nfc_irq(int irq, void *data)
{
	struct dvf99_nfc *nfc = data;
	u32 pending;

	spin_lock(&nfc->lock);

	pending = readl(nfc->regs + DVF99_NFC_INT_CAUSE);
	if (!pending) {
		spin_unlock(&nfc->lock);
		return IRQ_NONE;
	}

	if (pending & (DVF99_NFC_ST_ERR_FOUND | DVF99_NFC_ST_ECC_NC_ERR))
		dvf99_nfc_ecc_drain(nfc);

	writel(pending, nfc->regs + DVF99_NFC_STS_CLR);

	if (nfc->armed) {
		if (pending & DVF99_NFC_ST_AHB_ERR)
			nfc->ahb_err = true;

		nfc->irq_seen |= pending;
		if (nfc->ahb_err ||
		    (nfc->irq_seen & nfc->irq_wait) == nfc->irq_wait) {
			nfc->armed = false;
			complete(&nfc->done);
		}
	}

	spin_unlock(&nfc->lock);

	return IRQ_HANDLED;
}

static u32 dvf99_nfc_seq_base(struct dvf99_nfc *nfc)
{
	return FIELD_PREP(DVF99_NFC_SEQ_CS, nfc->cs) | DVF99_NFC_SEQ_RDY_EN |
	       (nfc->rb ? DVF99_NFC_SEQ_RDY_SEL : 0) | DVF99_NFC_SEQ_WAIT1;
}

static void dvf99_nfc_spare_words_set(struct dvf99_nfc *nfc)
{
	u32 val;

	val = readl(nfc->regs + DVF99_NFC_PULSETIME);
	val &= ~DVF99_NFC_PT_SPARE_WORDS;
	val |= FIELD_PREP(DVF99_NFC_PT_SPARE_WORDS, nfc->spare_words);
	writel(val, nfc->regs + DVF99_NFC_PULSETIME);
}

static void dvf99_nfc_hw_init(struct dvf99_nfc *nfc)
{
	u32 wait;
	int i;

	writel(0, nfc->regs + DVF99_NFC_INT_EN);
	writel(~0U, nfc->regs + DVF99_NFC_STS_CLR);

	wait = FIELD_PREP(DVF99_NFC_WAIT_LO_A, DVF99_NFC_WAIT_CYCLES) |
	       FIELD_PREP(DVF99_NFC_WAIT_LO_B, DVF99_NFC_WAIT_CYCLES) |
	       FIELD_PREP(DVF99_NFC_WAIT_HI_A, DVF99_NFC_WAIT_CYCLES) |
	       FIELD_PREP(DVF99_NFC_WAIT_HI_B, DVF99_NFC_WAIT_CYCLES);
	writel(wait, nfc->regs + DVF99_NFC_WAIT1);
	writel(wait, nfc->regs + DVF99_NFC_WAIT2);
	writel(wait, nfc->regs + DVF99_NFC_WAIT3);
	writel(FIELD_PREP(DVF99_NFC_WAIT_LO_A, DVF99_NFC_WAIT_CYCLES) |
	       FIELD_PREP(DVF99_NFC_WAIT_LO_B, DVF99_NFC_WAIT_CYCLES),
	       nfc->regs + DVF99_NFC_WAIT4);

	writel(FIELD_PREP(DVF99_NFC_PT_RD_LOW, DVF99_NFC_PULSE_CYCLES) |
	       FIELD_PREP(DVF99_NFC_PT_RD_HIGH, DVF99_NFC_PULSE_CYCLES) |
	       FIELD_PREP(DVF99_NFC_PT_WR_LOW, DVF99_NFC_PULSE_CYCLES) |
	       FIELD_PREP(DVF99_NFC_PT_WR_HIGH, DVF99_NFC_PULSE_CYCLES) |
	       FIELD_PREP(DVF99_NFC_PT_CLE_SETUP, DVF99_NFC_PULSE_CYCLES) |
	       FIELD_PREP(DVF99_NFC_PT_ALE_SETUP, DVF99_NFC_PULSE_CYCLES),
	       nfc->regs + DVF99_NFC_PULSETIME);

	for (i = 0; i < ARRAY_SIZE(dvf99_nfc_chien_codes); i++)
		writel(dvf99_nfc_chien_codes[i],
		       nfc->regs + DVF99_NFC_CHIEN(i));
}

static void dvf99_nfc_recover(struct dvf99_nfc *nfc)
{
	u32 status;
	int ret;

	if (nfc->rst) {
		reset_control_assert(nfc->rst);
		reset_control_deassert(nfc->rst);
		dvf99_nfc_hw_init(nfc);
		dvf99_nfc_spare_words_set(nfc);
		writel(DVF99_NFC_INT_MASK, nfc->regs + DVF99_NFC_INT_EN);
		return;
	}

	ret = readl_poll_timeout(nfc->regs + DVF99_NFC_STATUS, status,
				 !(status & (DVF99_NFC_ST_TRANS_BUSY |
					     DVF99_NFC_ST_ECC_BUSY)),
				 10, DVF99_NFC_IDLE_TIMEOUT_US);
	if (ret) {
		dev_err(nfc->dev, "controller stuck, disabling it\n");
		nfc->dead = true;
	}

	spin_lock_irq(&nfc->lock);
	dvf99_nfc_ecc_drain(nfc);
	nfc->ecc_ncodes = 0;
	writel(~0U, nfc->regs + DVF99_NFC_STS_CLR);
	spin_unlock_irq(&nfc->lock);
}

static int dvf99_nfc_run(struct dvf99_nfc *nfc, u32 seq,
			 unsigned int timeout_ms)
{
	u32 wait = DVF99_NFC_ST_TRANS_DONE;

	if (seq & DVF99_NFC_SEQ_RW)
		wait |= DVF99_NFC_ST_AHB_DONE;
	if (seq & DVF99_NFC_SEQ_ECC)
		wait |= DVF99_NFC_ST_ECC_DONE;

	if (nfc->dead)
		return -EIO;

	spin_lock_irq(&nfc->lock);
	nfc->irq_wait = wait;
	nfc->irq_seen = 0;
	nfc->ahb_err = false;
	nfc->armed = true;
	reinit_completion(&nfc->done);
	spin_unlock_irq(&nfc->lock);

	writel(seq, nfc->regs + DVF99_NFC_SEQUENCE);
	writel(~0U, nfc->regs + DVF99_NFC_STS_CLR);
	writel(1, nfc->regs + DVF99_NFC_START);

	if (!wait_for_completion_timeout(&nfc->done,
					 msecs_to_jiffies(timeout_ms))) {
		dev_err(nfc->dev, "transfer timeout, status 0x%08x\n",
			readl(nfc->regs + DVF99_NFC_STATUS));
		spin_lock_irq(&nfc->lock);
		nfc->armed = false;
		spin_unlock_irq(&nfc->lock);
		dvf99_nfc_recover(nfc);
		return -ETIMEDOUT;
	}

	if (nfc->ahb_err) {
		dev_err(nfc->dev, "AHB error\n");
		return -EIO;
	}

	return 0;
}

static int dvf99_nfc_exec_seq(struct nand_chip *chip,
			      const struct nand_subop *subop)
{
	struct dvf99_nfc *nfc = dvf99_nfc_from_chip(chip);
	struct mtd_info *mtd = nand_to_mtd(chip);
	unsigned int timeout_ms = DVF99_NFC_TIMEOUT_MS;
	unsigned int i, j, off, naddrs, ncmds = 0, len = 0, xfer;
	u32 seq, wait = DVF99_NFC_SEQ_WAIT0;
	u32 cmds = 0, col = 0, row = 0, dcount = 0;
	const struct nand_op_instr *instr;
	u8 *in = NULL;
	int ret;

	seq = dvf99_nfc_seq_base(nfc);

	for (i = 0; i < subop->ninstrs; i++) {
		instr = &subop->instrs[i];

		switch (instr->type) {
		case NAND_OP_CMD_INSTR:
			if (!ncmds) {
				cmds |= FIELD_PREP(DVF99_NFC_CMD_FIRST,
						   instr->ctx.cmd.opcode);
				seq |= DVF99_NFC_SEQ_CMD1;
				wait = DVF99_NFC_SEQ_WAIT0;
			} else {
				cmds |= FIELD_PREP(DVF99_NFC_CMD_SECOND,
						   instr->ctx.cmd.opcode);
				seq |= DVF99_NFC_SEQ_CMD2;
				wait = DVF99_NFC_SEQ_WAIT2;
			}
			ncmds++;
			break;

		case NAND_OP_ADDR_INSTR:
			off = nand_subop_get_addr_start_off(subop, i);
			naddrs = nand_subop_get_num_addr_cyc(subop, i);
			for (j = 0; j < naddrs; j++) {
				u32 addr = instr->ctx.addr.addrs[off + j];

				seq |= DVF99_NFC_SEQ_ADDR(j);
				if (j < 2)
					col |= addr << (8 * j);
				else
					row |= addr << (8 * (j - 2));
			}
			wait = DVF99_NFC_SEQ_WAIT1;
			break;

		case NAND_OP_DATA_IN_INSTR:
			off = nand_subop_get_data_start_off(subop, i);
			len = nand_subop_get_data_len(subop, i);
			in = instr->ctx.data.buf.in + off;
			seq |= DVF99_NFC_SEQ_RW;
			break;

		case NAND_OP_WAITRDY_INSTR:
			seq |= wait;
			timeout_ms += instr->ctx.waitrdy.timeout_ms;
			break;

		default:
			return -EOPNOTSUPP;
		}
	}

	xfer = len;
	if (ncmds > 1 && mtd->writesize && len > mtd->writesize) {
		xfer = mtd->writesize;
		dcount = FIELD_PREP(DVF99_NFC_DCOUNT_VP_SIZE, xfer) |
			 FIELD_PREP(DVF99_NFC_DCOUNT_N2, len - xfer);
	} else if (ncmds > 1) {
		dcount = FIELD_PREP(DVF99_NFC_DCOUNT_VP_SIZE, len);
	}

	writel(col, nfc->regs + DVF99_NFC_ADDR_COL);
	writel(row, nfc->regs + DVF99_NFC_ADDR_ROW);
	writel(cmds, nfc->regs + DVF99_NFC_CMD1);
	writel(0, nfc->regs + DVF99_NFC_CMD2);
	writel(dcount, nfc->regs + DVF99_NFC_DCOUNT);
	writel(FIELD_PREP(DVF99_NFC_FTSR_SIZE, xfer), nfc->regs + DVF99_NFC_FTSR);
	writel(0, nfc->regs + DVF99_NFC_TIMEOUT);
	writel(nfc->buf_dma, nfc->regs + DVF99_NFC_DPR);
	writel(nfc->buf_dma + xfer, nfc->regs + DVF99_NFC_RDPR);

	ret = dvf99_nfc_run(nfc, seq, timeout_ms);
	if (ret)
		return ret;

	if (len)
		memcpy(in, nfc->buf, len);

	return 0;
}

static int dvf99_nfc_exec_waitrdy(struct nand_chip *chip,
				  const struct nand_subop *subop)
{
	struct dvf99_nfc *nfc = dvf99_nfc_from_chip(chip);
	const struct nand_op_instr *instr = &subop->instrs[0];
	u32 status;

	return readl_poll_timeout(nfc->regs + DVF99_NFC_STATUS, status,
				  status & DVF99_NFC_ST_RDY(nfc->rb), 10,
				  instr->ctx.waitrdy.timeout_ms * USEC_PER_MSEC);
}

static const struct nand_op_parser dvf99_nfc_op_parser = NAND_OP_PARSER(
	NAND_OP_PARSER_PATTERN(dvf99_nfc_exec_seq,
		NAND_OP_PARSER_PAT_CMD_ELEM(false),
		NAND_OP_PARSER_PAT_WAITRDY_ELEM(true),
		NAND_OP_PARSER_PAT_ADDR_ELEM(true, 5),
		NAND_OP_PARSER_PAT_WAITRDY_ELEM(true),
		NAND_OP_PARSER_PAT_CMD_ELEM(true),
		NAND_OP_PARSER_PAT_WAITRDY_ELEM(true),
		NAND_OP_PARSER_PAT_DATA_IN_ELEM(true, DVF99_NFC_MAX_XFER)),
	NAND_OP_PARSER_PATTERN(dvf99_nfc_exec_waitrdy,
		NAND_OP_PARSER_PAT_WAITRDY_ELEM(false)),
);

static bool dvf99_nfc_opcode_is_read_only(u8 opcode)
{
	switch (opcode) {
	case NAND_CMD_RESET:
	case NAND_CMD_READID:
	case NAND_CMD_PARAM:
	case NAND_CMD_STATUS:
	case NAND_CMD_READ0:
	case NAND_CMD_READSTART:
	case NAND_CMD_RNDOUT:
	case NAND_CMD_RNDOUTSTART:
	case NAND_CMD_GET_FEATURES:
		return true;
	default:
		return false;
	}
}

static bool dvf99_nfc_op_is_read_only(const struct nand_operation *op)
{
	const struct nand_op_instr *instr;
	unsigned int i;

	for (i = 0; i < op->ninstrs; i++) {
		instr = &op->instrs[i];

		if (instr->type == NAND_OP_DATA_OUT_INSTR)
			return false;

		if (instr->type == NAND_OP_CMD_INSTR &&
		    !dvf99_nfc_opcode_is_read_only(instr->ctx.cmd.opcode))
			return false;
	}

	return true;
}

static u32 dvf99_nfc_row_seq(struct nand_chip *chip)
{
	u32 seq = DVF99_NFC_SEQ_ADDR(2) | DVF99_NFC_SEQ_ADDR(3);

	if (chip->options & NAND_ROW_ADDR_3)
		seq |= DVF99_NFC_SEQ_ADDR(4);

	return seq;
}

static int dvf99_nfc_status_check(struct dvf99_nfc *nfc)
{
	u8 status = readl(nfc->regs + DVF99_NFC_FBYP_DATA);

	return status & NAND_STATUS_FAIL ? -EIO : 0;
}

static int dvf99_nfc_erase(struct nand_chip *chip, unsigned int page)
{
	struct dvf99_nfc *nfc = dvf99_nfc_from_chip(chip);
	u32 seq;
	int ret;

	seq = FIELD_PREP(DVF99_NFC_SEQ_CS, nfc->cs) | DVF99_NFC_SEQ_RDY_EN |
	      (nfc->rb ? DVF99_NFC_SEQ_RDY_SEL : 0) |
	      DVF99_NFC_SEQ_CMD1 | DVF99_NFC_SEQ_WAIT0 |
	      dvf99_nfc_row_seq(chip) |
	      DVF99_NFC_SEQ_CMD2 | DVF99_NFC_SEQ_WAIT2 |
	      DVF99_NFC_SEQ_CMD4 | DVF99_NFC_SEQ_WAIT4 |
	      DVF99_NFC_SEQ_READ_ONCE;

	writel(0, nfc->regs + DVF99_NFC_ADDR_COL);
	writel(page, nfc->regs + DVF99_NFC_ADDR_ROW);
	writel(FIELD_PREP(DVF99_NFC_CMD_FIRST, NAND_CMD_ERASE1) |
	       FIELD_PREP(DVF99_NFC_CMD_SECOND, NAND_CMD_ERASE2) |
	       FIELD_PREP(DVF99_NFC_CMD_FOURTH, NAND_CMD_STATUS),
	       nfc->regs + DVF99_NFC_CMD1);
	writel(0, nfc->regs + DVF99_NFC_CMD2);
	writel(0, nfc->regs + DVF99_NFC_DCOUNT);
	writel(0, nfc->regs + DVF99_NFC_FTSR);
	writel(0, nfc->regs + DVF99_NFC_TIMEOUT);

	ret = dvf99_nfc_run(nfc, seq, DVF99_NFC_TIMEOUT_MS);
	if (ret)
		return ret;

	return dvf99_nfc_status_check(nfc);
}

static bool dvf99_nfc_op_is_erase(const struct nand_operation *op,
				  unsigned int *page)
{
	const struct nand_op_instr *instr = op->instrs;
	unsigned int i;

	if (op->ninstrs < 3 ||
	    instr[0].type != NAND_OP_CMD_INSTR ||
	    instr[0].ctx.cmd.opcode != NAND_CMD_ERASE1 ||
	    instr[1].type != NAND_OP_ADDR_INSTR ||
	    instr[2].type != NAND_OP_CMD_INSTR ||
	    instr[2].ctx.cmd.opcode != NAND_CMD_ERASE2)
		return false;

	for (i = 3; i < op->ninstrs; i++)
		if (instr[i].type != NAND_OP_WAITRDY_INSTR)
			return false;

	if (instr[1].ctx.addr.naddrs > sizeof(*page))
		return false;

	*page = 0;
	for (i = 0; i < instr[1].ctx.addr.naddrs; i++)
		*page |= instr[1].ctx.addr.addrs[i] << (8 * i);

	return true;
}

static int dvf99_nfc_exec_op(struct nand_chip *chip,
			     const struct nand_operation *op, bool check_only)
{
	unsigned int page;

	if (dvf99_nfc_op_is_erase(op, &page))
		return check_only ? 0 : dvf99_nfc_erase(chip, page);

	if (!dvf99_nfc_op_is_read_only(op))
		return -EOPNOTSUPP;

	return nand_op_parser_exec_op(chip, &dvf99_nfc_op_parser, op,
				      check_only);
}

static unsigned int dvf99_nfc_zero_bits(const u8 *buf, unsigned int len)
{
	unsigned int i, count = 0;

	for (i = 0; i < len; i++)
		count += hweight8(~buf[i]);

	return count;
}

static bool dvf99_nfc_page_is_erased(struct nand_chip *chip)
{
	struct mtd_info *mtd = nand_to_mtd(chip);
	struct dvf99_nfc *nfc = dvf99_nfc_from_chip(chip);
	const u8 *oob = nfc->buf + mtd->writesize;
	u8 dirty = oob[nfc->dirty_pos];

	if (dirty == 0xff)
		return true;
	if (dirty == 0x00)
		return false;

	return dvf99_nfc_zero_bits(oob + nfc->n1, chip->ecc.bytes) <=
	       chip->ecc.strength;
}

static int dvf99_nfc_erased_check(struct nand_chip *chip)
{
	struct mtd_info *mtd = nand_to_mtd(chip);
	struct dvf99_nfc *nfc = dvf99_nfc_from_chip(chip);
	unsigned int chunk = nfc->n1 + nfc->n2;
	u8 *oob = nfc->buf + mtd->writesize;
	unsigned int max_bitflips = 0;
	int step, ret;

	for (step = 0; step < chip->ecc.steps; step++) {
		ret = nand_check_erased_ecc_chunk(nfc->buf + step * chip->ecc.size,
						  chip->ecc.size,
						  oob + step * chunk,
						  nfc->n1 + chip->ecc.bytes,
						  NULL, 0, chip->ecc.strength);
		if (ret < 0) {
			mtd->ecc_stats.failed++;
			continue;
		}

		mtd->ecc_stats.corrected += ret;
		max_bitflips = max_t(unsigned int, max_bitflips, ret);
	}

	return max_bitflips;
}

static int dvf99_nfc_ecc_correct(struct nand_chip *chip)
{
	struct mtd_info *mtd = nand_to_mtd(chip);
	struct dvf99_nfc *nfc = dvf99_nfc_from_chip(chip);
	unsigned int flips[DVF99_NFC_MAX_STEPS] = { };
	unsigned int chunk = nfc->n1 + nfc->n2;
	u8 *oob = nfc->buf + mtd->writesize;
	unsigned int max_bitflips = 0;
	unsigned int i, step, byte;
	int vector_bits, loc;

	if (dvf99_nfc_page_is_erased(chip))
		return dvf99_nfc_erased_check(chip);

	if (nfc->ecc_uncorrectable)
		goto uncorrectable;

	vector_bits = chip->ecc.size * BITS_PER_BYTE + nfc->n1 * BITS_PER_BYTE +
		      chip->ecc.strength * 13 - 1;

	for (i = 0; i < nfc->ecc_ncodes; i++) {
		step = FIELD_GET(DVF99_NFC_BCH_STEP, nfc->ecc_codes[i]);
		loc = vector_bits -
		      (int)FIELD_GET(DVF99_NFC_BCH_LOC, nfc->ecc_codes[i]);
		if (loc < 0 || step >= chip->ecc.steps)
			goto uncorrectable;

		byte = loc / BITS_PER_BYTE;
		if (byte < chip->ecc.size)
			nfc->buf[step * chip->ecc.size + byte] ^= BIT(loc % BITS_PER_BYTE);
		else if (byte - chip->ecc.size < chunk)
			oob[step * chunk + byte - chip->ecc.size] ^= BIT(loc % BITS_PER_BYTE);
		else
			goto uncorrectable;

		flips[step]++;
	}

	for (step = 0; step < chip->ecc.steps; step++) {
		mtd->ecc_stats.corrected += flips[step];
		max_bitflips = max(max_bitflips, flips[step]);
	}

	return max_bitflips;

uncorrectable:
	mtd->ecc_stats.failed++;
	return 0;
}

static int dvf99_nfc_read_page(struct nand_chip *chip, u8 *buf,
			       int oob_required, int page)
{
	struct mtd_info *mtd = nand_to_mtd(chip);
	struct dvf99_nfc *nfc = dvf99_nfc_from_chip(chip);
	unsigned int steps = chip->ecc.steps;
	u32 seq;
	int ret;

	seq = dvf99_nfc_seq_base(nfc) | DVF99_NFC_SEQ_CMD1 |
	      DVF99_NFC_SEQ_ADDR(0) | DVF99_NFC_SEQ_ADDR(1) |
	      DVF99_NFC_SEQ_ADDR(2) | DVF99_NFC_SEQ_ADDR(3) |
	      DVF99_NFC_SEQ_CMD2 | DVF99_NFC_SEQ_WAIT2 |
	      DVF99_NFC_SEQ_RW | DVF99_NFC_SEQ_ECC |
	      DVF99_NFC_SEQ_CMD5 | DVF99_NFC_SEQ_WAIT5 |
	      DVF99_NFC_SEQ_CMD6 | DVF99_NFC_SEQ_WAIT6;
	if (chip->options & NAND_ROW_ADDR_3)
		seq |= DVF99_NFC_SEQ_ADDR(4);

	writel(FIELD_PREP(DVF99_NFC_CTL_ECC_MODE, nfc->ecc_mode) |
	       FIELD_PREP(DVF99_NFC_CTL_CHIEN_START, DVF99_NFC_CHIEN_COUNT_START),
	       nfc->regs + DVF99_NFC_CTL);
	writel(0, nfc->regs + DVF99_NFC_ADDR_COL);
	writel(page, nfc->regs + DVF99_NFC_ADDR_ROW);
	writel(FIELD_PREP(DVF99_NFC_CMD_FIRST, NAND_CMD_READ0) |
	       FIELD_PREP(DVF99_NFC_CMD_SECOND, NAND_CMD_READSTART),
	       nfc->regs + DVF99_NFC_CMD1);
	writel(FIELD_PREP(DVF99_NFC_CMD_FIRST, NAND_CMD_RNDOUT) |
	       FIELD_PREP(DVF99_NFC_CMD_SECOND, NAND_CMD_RNDOUTSTART),
	       nfc->regs + DVF99_NFC_CMD2);
	writel(FIELD_PREP(DVF99_NFC_DCOUNT_VP_SIZE, chip->ecc.size) |
	       FIELD_PREP(DVF99_NFC_DCOUNT_N3, nfc->n3) |
	       FIELD_PREP(DVF99_NFC_DCOUNT_N2, nfc->n2) |
	       FIELD_PREP(DVF99_NFC_DCOUNT_N1, nfc->n1) |
	       FIELD_PREP(DVF99_NFC_DCOUNT_PAGE_CNT, steps - 1),
	       nfc->regs + DVF99_NFC_DCOUNT);
	writel(FIELD_PREP(DVF99_NFC_FTSR_SIZE, steps), nfc->regs + DVF99_NFC_FTSR);
	writel(0, nfc->regs + DVF99_NFC_TIMEOUT);
	writel(nfc->buf_dma, nfc->regs + DVF99_NFC_DPR);
	writel(nfc->buf_dma + mtd->writesize, nfc->regs + DVF99_NFC_RDPR);

	spin_lock_irq(&nfc->lock);
	nfc->ecc_ncodes = 0;
	nfc->ecc_uncorrectable = false;
	spin_unlock_irq(&nfc->lock);

	ret = dvf99_nfc_run(nfc, seq, DVF99_NFC_TIMEOUT_MS);
	if (ret)
		return ret;

	spin_lock_irq(&nfc->lock);
	dvf99_nfc_ecc_drain(nfc);
	spin_unlock_irq(&nfc->lock);

	ret = dvf99_nfc_ecc_correct(chip);

	memcpy(buf, nfc->buf, mtd->writesize);
	if (oob_required)
		memcpy(chip->oob_poi, nfc->buf + mtd->writesize, mtd->oobsize);

	return ret;
}

static int dvf99_nfc_read_page_raw(struct nand_chip *chip, u8 *buf,
				   int oob_required, int page)
{
	struct mtd_info *mtd = nand_to_mtd(chip);
	struct dvf99_nfc *nfc = dvf99_nfc_from_chip(chip);
	int ret;

	ret = nand_read_page_op(chip, page, 0, nfc->raw_buf,
				mtd->writesize + mtd->oobsize);
	if (ret)
		return ret;

	memcpy(buf, nfc->raw_buf, mtd->writesize);
	if (oob_required)
		memcpy(chip->oob_poi, nfc->raw_buf + mtd->writesize,
		       mtd->oobsize);

	return 0;
}

static bool dvf99_nfc_buf_is_blank(const u8 *buf, unsigned int len)
{
	return memchr_inv(buf, 0xff, len) == NULL;
}

static int dvf99_nfc_write_page(struct nand_chip *chip, const u8 *buf,
				int oob_required, int page)
{
	struct mtd_info *mtd = nand_to_mtd(chip);
	struct dvf99_nfc *nfc = dvf99_nfc_from_chip(chip);
	u8 *oob = nfc->buf + mtd->writesize;
	unsigned int steps = chip->ecc.steps;
	u32 seq;
	int ret;

	memcpy(nfc->buf, buf, mtd->writesize);
	if (oob_required)
		memcpy(oob, chip->oob_poi, mtd->oobsize);
	else
		memset(oob, 0xff, mtd->oobsize);

	oob[nfc->dirty_pos] = 0xff;
	if (dvf99_nfc_buf_is_blank(nfc->buf, mtd->writesize + mtd->oobsize))
		return 0;
	oob[nfc->dirty_pos] = 0;

	seq = FIELD_PREP(DVF99_NFC_SEQ_CS, nfc->cs) | DVF99_NFC_SEQ_RDY_EN |
	      (nfc->rb ? DVF99_NFC_SEQ_RDY_SEL : 0) |
	      DVF99_NFC_SEQ_CMD1 | DVF99_NFC_SEQ_ADDR(0) | DVF99_NFC_SEQ_ADDR(1) |
	      dvf99_nfc_row_seq(chip) |
	      DVF99_NFC_SEQ_RW | DVF99_NFC_SEQ_RW_WRITE | DVF99_NFC_SEQ_ECC |
	      DVF99_NFC_SEQ_CMD3 | DVF99_NFC_SEQ_WAIT3 |
	      DVF99_NFC_SEQ_CMD4 | DVF99_NFC_SEQ_WAIT4 |
	      DVF99_NFC_SEQ_CMD5 | DVF99_NFC_SEQ_WAIT5 |
	      DVF99_NFC_SEQ_WAIT6 | DVF99_NFC_SEQ_READ_ONCE;

	writel(FIELD_PREP(DVF99_NFC_CTL_ECC_MODE, nfc->ecc_mode) |
	       FIELD_PREP(DVF99_NFC_CTL_CHIEN_START, DVF99_NFC_CHIEN_COUNT_START),
	       nfc->regs + DVF99_NFC_CTL);
	writel(0, nfc->regs + DVF99_NFC_ADDR_COL);
	writel(page, nfc->regs + DVF99_NFC_ADDR_ROW);
	writel(FIELD_PREP(DVF99_NFC_CMD_FIRST, NAND_CMD_SEQIN) |
	       FIELD_PREP(DVF99_NFC_CMD_THIRD, NAND_CMD_PAGEPROG) |
	       FIELD_PREP(DVF99_NFC_CMD_FOURTH, NAND_CMD_STATUS),
	       nfc->regs + DVF99_NFC_CMD1);
	writel(FIELD_PREP(DVF99_NFC_CMD_FIRST, NAND_CMD_RNDIN),
	       nfc->regs + DVF99_NFC_CMD2);
	writel(FIELD_PREP(DVF99_NFC_DCOUNT_VP_SIZE, chip->ecc.size) |
	       FIELD_PREP(DVF99_NFC_DCOUNT_N3, nfc->n3) |
	       FIELD_PREP(DVF99_NFC_DCOUNT_N2, nfc->n2) |
	       FIELD_PREP(DVF99_NFC_DCOUNT_N1, nfc->n1) |
	       FIELD_PREP(DVF99_NFC_DCOUNT_PAGE_CNT, steps - 1),
	       nfc->regs + DVF99_NFC_DCOUNT);
	writel(FIELD_PREP(DVF99_NFC_FTSR_SIZE, steps), nfc->regs + DVF99_NFC_FTSR);
	writel(0, nfc->regs + DVF99_NFC_TIMEOUT);
	writel(nfc->buf_dma, nfc->regs + DVF99_NFC_DPR);
	writel(nfc->buf_dma + mtd->writesize, nfc->regs + DVF99_NFC_RDPR);

	ret = dvf99_nfc_run(nfc, seq, DVF99_NFC_TIMEOUT_MS);
	if (ret)
		return ret;

	return dvf99_nfc_status_check(nfc);
}

static int dvf99_nfc_write_page_raw(struct nand_chip *chip, const u8 *buf,
				    int oob_required, int page)
{
	return -EROFS;
}

static int dvf99_nfc_write_oob(struct nand_chip *chip, int page)
{
	return -EROFS;
}

static int dvf99_nfc_ooblayout_ecc(struct mtd_info *mtd, int section,
				   struct mtd_oob_region *oobregion)
{
	struct nand_chip *chip = mtd_to_nand(mtd);
	struct dvf99_nfc *nfc = dvf99_nfc_from_chip(chip);

	if (section >= chip->ecc.steps)
		return -ERANGE;

	oobregion->offset = section * (nfc->n1 + nfc->n2) + nfc->n1;
	oobregion->length = chip->ecc.bytes;

	return 0;
}

static int dvf99_nfc_ooblayout_free(struct mtd_info *mtd, int section,
				    struct mtd_oob_region *oobregion)
{
	struct nand_chip *chip = mtd_to_nand(mtd);
	struct dvf99_nfc *nfc = dvf99_nfc_from_chip(chip);
	unsigned int len = nfc->n2 - chip->ecc.bytes;

	if (section || !len)
		return -ERANGE;

	oobregion->offset = nfc->n1 + chip->ecc.bytes;
	oobregion->length = len;

	return 0;
}

static const struct mtd_ooblayout_ops dvf99_nfc_ooblayout_ops = {
	.ecc = dvf99_nfc_ooblayout_ecc,
	.free = dvf99_nfc_ooblayout_free,
};

static int dvf99_nfc_ecc_mode_get(struct nand_chip *chip)
{
	const struct nand_ecc_props *req;
	struct mtd_info *mtd = nand_to_mtd(chip);
	struct dvf99_nfc *nfc = dvf99_nfc_from_chip(chip);
	int mode;

	if (!chip->parameters.onfi)
		return mtd->oobsize >= DVF99_NFC_BCH_MIN_OOBSIZE ?
		       DVF99_NFC_ECC_BCH4 : DVF99_NFC_ECC_HAMMING;

	req = nanddev_get_ecc_requirements(&chip->base);
	for (mode = DVF99_NFC_ECC_HAMMING; mode <= DVF99_NFC_ECC_BCH16; mode++)
		if (dvf99_nfc_ecc_bits[mode] >= req->strength)
			break;

	if (mode > DVF99_NFC_ECC_BCH16)
		return -EINVAL;

	if (mode == DVF99_NFC_ECC_HAMMING && !nfc->legacy_layout &&
	    mtd->oobsize >= DVF99_NFC_BCH_MIN_OOBSIZE)
		mode = DVF99_NFC_ECC_BCH4;

	return mode;
}

static int dvf99_nfc_attach_chip(struct nand_chip *chip)
{
	struct mtd_info *mtd = nand_to_mtd(chip);
	struct dvf99_nfc *nfc = dvf99_nfc_from_chip(chip);
	unsigned int steps, chunk, eccbytes, bbm;
	int mode;

	if (chip->ecc.engine_type != NAND_ECC_ENGINE_TYPE_ON_HOST) {
		dev_err(nfc->dev, "only on-host ECC is supported\n");
		return -EINVAL;
	}

	if (chip->options & NAND_BUSWIDTH_16) {
		dev_err(nfc->dev, "16-bit bus is not supported\n");
		return -EINVAL;
	}

	if (mtd->writesize < SZ_2K ||
	    mtd->writesize + mtd->oobsize > DVF99_NFC_BUF_SIZE ||
	    mtd->oobsize > FIELD_MAX(DVF99_NFC_DCOUNT_N2)) {
		dev_err(nfc->dev, "unsupported page size %u+%u\n",
			mtd->writesize, mtd->oobsize);
		return -EINVAL;
	}

	mode = dvf99_nfc_ecc_mode_get(chip);
	if (mode < 0) {
		dev_err(nfc->dev, "unsupported ECC requirement\n");
		return mode;
	}

	bbm = chip->badblockpos & ~1;
	if (nfc->legacy_layout) {
		nfc->dirty_pos = bbm + 2;
		nfc->n1 = nfc->dirty_pos + 2;
		nfc->n3 = nfc->n1 - 2;
	} else {
		nfc->dirty_pos = 1;
		nfc->n1 = bbm + 2;
		nfc->n3 = 1;
	}

	steps = mtd->writesize / DVF99_NFC_STEP_SIZE;
	chunk = mtd->oobsize / steps;
	if (mode == DVF99_NFC_ECC_HAMMING)
		eccbytes = DVF99_NFC_HAMMING_BYTES;
	else
		eccbytes = DIV_ROUND_UP(dvf99_nfc_ecc_bits[mode] * 13,
					BITS_PER_BYTE);
	if (chunk < nfc->n1 + eccbytes) {
		dev_err(nfc->dev, "OOB too small for %u-bit ECC\n",
			dvf99_nfc_ecc_bits[mode]);
		return -EINVAL;
	}

	nfc->ecc_mode = mode;
	nfc->n2 = chunk - nfc->n1;
	nfc->spare_words = DIV_ROUND_UP(chunk, 4);
	dvf99_nfc_spare_words_set(nfc);

	nfc->raw_buf = devm_kmalloc(nfc->dev, mtd->writesize + mtd->oobsize,
				    GFP_KERNEL);
	if (!nfc->raw_buf)
		return -ENOMEM;

	chip->ecc.size = DVF99_NFC_STEP_SIZE;
	chip->ecc.steps = steps;
	chip->ecc.bytes = eccbytes;
	chip->ecc.strength = dvf99_nfc_ecc_bits[mode];
	chip->ecc.read_page_raw = dvf99_nfc_read_page_raw;
	chip->ecc.write_page = dvf99_nfc_write_page_raw;
	chip->ecc.write_page_raw = dvf99_nfc_write_page_raw;
	chip->ecc.write_oob = dvf99_nfc_write_oob;
	chip->ecc.write_oob_raw = dvf99_nfc_write_oob;
	mtd_set_ooblayout(mtd, &dvf99_nfc_ooblayout_ops);

	if (mode == DVF99_NFC_ECC_HAMMING) {
		chip->ecc.engine_type = NAND_ECC_ENGINE_TYPE_SOFT;
		chip->ecc.algo = NAND_ECC_ALGO_HAMMING;
		dev_info(nfc->dev, "1-bit software Hamming, %s layout\n",
			 nfc->legacy_layout ? "legacy" : "current");
		return 0;
	}

	nfc->ecc_maxcodes = steps * dvf99_nfc_ecc_bits[mode];
	nfc->ecc_codes = devm_kcalloc(nfc->dev, nfc->ecc_maxcodes,
				      sizeof(*nfc->ecc_codes), GFP_KERNEL);
	if (!nfc->ecc_codes)
		return -ENOMEM;

	chip->ecc.read_page = dvf99_nfc_read_page;
	chip->ecc.write_page = dvf99_nfc_write_page;

	dev_info(nfc->dev, "%u-bit BCH, %s layout\n", chip->ecc.strength,
		 nfc->legacy_layout ? "legacy" : "current");

	return 0;
}

static const struct nand_controller_ops dvf99_nfc_controller_ops = {
	.attach_chip = dvf99_nfc_attach_chip,
	.exec_op = dvf99_nfc_exec_op,
};

static int dvf99_nfc_rom_layout_get(struct platform_device *pdev,
				    struct dvf99_nfc *nfc)
{
	void __iomem *rom;

	rom = devm_platform_ioremap_resource(pdev, 1);
	if (IS_ERR(rom))
		return PTR_ERR(rom);

	nfc->legacy_layout =
		readl(rom + DVF99_ROM_REV1_OFS) == DVF99_ROM_REV1_TAG ||
		readl(rom + DVF99_ROM_REV2_OFS) == DVF99_ROM_REV2_TAG;

	devm_iounmap(&pdev->dev, rom);

	return 0;
}

static int dvf99_nfc_chip_init(struct dvf99_nfc *nfc)
{
	struct device_node *np __free(device_node) =
		of_get_next_available_child(nfc->dev->of_node, NULL);
	struct nand_chip *chip = &nfc->chip;
	struct mtd_info *mtd = nand_to_mtd(chip);
	int ret;

	if (!np)
		return dev_err_probe(nfc->dev, -ENODEV, "no NAND chip node\n");

	ret = of_property_read_u32(np, "reg", &nfc->cs);
	if (ret || nfc->cs > 1)
		return dev_err_probe(nfc->dev, -EINVAL, "invalid chip select\n");

	nfc->rb = nfc->cs;
	of_property_read_u32(np, "nand-rb", &nfc->rb);
	if (nfc->rb > 1)
		return dev_err_probe(nfc->dev, -EINVAL, "invalid ready/busy line\n");

	chip->controller = &nfc->controller;
	chip->options |= NAND_NO_SUBPAGE_WRITE;
	chip->ecc.engine_type = NAND_ECC_ENGINE_TYPE_ON_HOST;
	nand_set_flash_node(chip, np);

	mtd->dev.parent = nfc->dev;
	mtd->owner = THIS_MODULE;
	mtd->name = DVF99_MTD_NAME;

	ret = nand_scan(chip, 1);
	if (ret)
		return ret;

	ret = mtd_device_register(mtd, NULL, 0);
	if (ret) {
		nand_cleanup(chip);
		return ret;
	}

	return 0;
}

static int dvf99_nfc_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	struct dvf99_nfc *nfc;
	struct clk *clk;
	int irq, ret;

	nfc = devm_kzalloc(dev, sizeof(*nfc), GFP_KERNEL);
	if (!nfc)
		return -ENOMEM;

	nfc->dev = dev;
	init_completion(&nfc->done);
	spin_lock_init(&nfc->lock);
	nand_controller_init(&nfc->controller);
	nfc->controller.ops = &dvf99_nfc_controller_ops;

	nfc->regs = devm_platform_ioremap_resource(pdev, 0);
	if (IS_ERR(nfc->regs))
		return PTR_ERR(nfc->regs);

	ret = dvf99_nfc_rom_layout_get(pdev, nfc);
	if (ret)
		return ret;

	irq = platform_get_irq(pdev, 0);
	if (irq < 0)
		return irq;

	clk = devm_clk_get_enabled(dev, NULL);
	if (IS_ERR(clk))
		return dev_err_probe(dev, PTR_ERR(clk), "failed to get clock\n");

	nfc->rst = devm_reset_control_get_optional_exclusive_deasserted(dev, NULL);
	if (IS_ERR(nfc->rst))
		return dev_err_probe(dev, PTR_ERR(nfc->rst),
				     "failed to get reset\n");

	nfc->buf = dmam_alloc_coherent(dev, DVF99_NFC_BUF_SIZE, &nfc->buf_dma,
				       GFP_KERNEL);
	if (!nfc->buf)
		return -ENOMEM;

	dvf99_nfc_hw_init(nfc);

	ret = devm_request_irq(dev, irq, dvf99_nfc_irq, 0, dev_name(dev), nfc);
	if (ret)
		return ret;

	writel(DVF99_NFC_INT_MASK, nfc->regs + DVF99_NFC_INT_EN);

	platform_set_drvdata(pdev, nfc);

	return dvf99_nfc_chip_init(nfc);
}

static void dvf99_nfc_remove(struct platform_device *pdev)
{
	struct dvf99_nfc *nfc = platform_get_drvdata(pdev);
	struct nand_chip *chip = &nfc->chip;

	WARN_ON(mtd_device_unregister(nand_to_mtd(chip)));
	nand_cleanup(chip);
	writel(0, nfc->regs + DVF99_NFC_INT_EN);
}

static const struct of_device_id dvf99_nfc_of_match[] = {
	{ .compatible = "dspg,dvf99-nfc" },
	{ }
};
MODULE_DEVICE_TABLE(of, dvf99_nfc_of_match);

static struct platform_driver dvf99_nfc_driver = {
	.probe = dvf99_nfc_probe,
	.remove = dvf99_nfc_remove,
	.driver = {
		.name = "dvf99-nfc",
		.of_match_table = dvf99_nfc_of_match,
	},
};
module_platform_driver(dvf99_nfc_driver);

MODULE_AUTHOR("John Crispin <john@phrozen.org>");
MODULE_DESCRIPTION("DSPG DVF99 NAND flash controller driver");
MODULE_LICENSE("GPL");

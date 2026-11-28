// SPDX-License-Identifier: GPL-2.0
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/module.h>
#include <linux/sched.h>
#include <linux/platform_device.h>
#include <linux/reset.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/rawnand.h>
#include <linux/mtd/partitions.h>
#include <linux/bitops.h>
#include <linux/err.h>
#include <linux/unaligned.h>
#include <linux/io.h>
#include <linux/iopoll.h>
#include <linux/clk.h>
#include <linux/delay.h>
#include <linux/jiffies.h>
#include <linux/of.h>

#define CS75XX_OWN_BUF_SIZE	(SZ_4K + 512)

#define FLASH_STATUS		0x08
#define FLASH_TYPE		0x0c
#define FLASH_NF_ACCESS		0x28
#define FLASH_NF_COUNT		0x2c
#define FLASH_NF_COMMAND	0x30
#define FLASH_NF_ADDRESS_1	0x34
#define FLASH_NF_ADDRESS_2	0x38
#define FLASH_NF_DATA		0x3c
#define FLASH_NF_ECC_STATUS	0x44
#define FLASH_NF_ECC_CONTROL	0x48
#define FLASH_NF_ECC_OOB	0x4c
#define FLASH_NF_ECC_GEN0	0x50
#define FLASH_FLASH_ACCESS_START 0xa4
#define FLASH_NF_ECC_RESET	0xa8
#define FLASH_FLASH_INTERRUPT	0xac
#define FLASH_NF_BCH_CONTROL	0xd0

#define NF_ACCESS_EXTADDR(x)	((x) & 0xff)
#define NF_ACCESS_WIDTH8	(0x0 << 10)
#define NF_ACCESS_WIDTH32	(0x2 << 10)
#define NF_ACCESS_CE_ALT	BIT(15)

#define NF_CNT_CMD(x)		((x) & 0x3)
#define NF_CNT_ADDR(x)		(((x) & 0x7) << 4)
#define NF_CNT_DATA(x)		(((x) & 0x3fff) << 8)
#define NF_CNT_OOB(x)		(((x) & 0x3ffU) << 22)
#define NCNT_EMPTY_OOB		0x3ff
#define NCNT_EMPTY_DATA		0x3fff
#define NCNT_EMPTY_ADDR		0x7
#define NCNT_EMPTY_CMD		0x3

#define FLASH_START_REGREQ	BIT(0)
#define FLASH_START_CMD(x)	(((x) & 0x3) << 12)
#define FLASH_RD		0x2
#define FLASH_WT		0x3

#define ECC_RESET_ECC_CLR	BIT(0)
#define ECC_RESET_FIFO_CLR	BIT(1)
#define ECC_RESET_NF_RESET	BIT(2)

#define FLASH_INT_REGIRQ	BIT(0)

#define ECC_CTL_CODESEL(x)	(((x) & 0xf) << 4)
#define ECC_CTL_EN		BIT(8)

#define ECC_STS_STATUS(v)	((v) & 0x3)
#define ECC_STS_ERRBIT(v)	(((v) >> 3) & 0xf)
#define ECC_STS_ERRBYTE(v)	(((v) >> 7) & 0x1ff)
#define ECC_STS_DONE		BIT(31)
#define ECC_NO_ERR		0x0
#define ECC_1BIT_DATA_ERR	0x1
#define ECC_1BIT_ECC_ERR	0x2
#define ECC_UNCORRECTABLE	0x3

#define FLASH_STATUS_NSTATE_MASK	(0x0f << 8)

struct cs752x_nand_host {
	struct nand_controller	controller;
	struct nand_chip	chip;
	struct device		*dev;

	void __iomem		*iobase_fl;
	u8			own_buf[CS75XX_OWN_BUF_SIZE];
	unsigned int		buf_offs;
	unsigned int		buf_data_len;
	unsigned int		cache_column;
	u8			read_mode;
	u8			cs;
	bool			use_hwecc;

	u8			*ecc_calc;
	u8			*ecc_code;
};

/* The controller sequences complete transactions; do not split CMD/ADDR. */
struct cs752x_nand_op {
	u32 command;
	u64 address;
	unsigned int ncmds;
	unsigned int naddrs;
	const struct nand_op_instr *data;
	unsigned int timeout_ms;
	unsigned int delay_ns;
	bool wait_ready;
};

static inline struct cs752x_nand_host *to_host(struct nand_chip *chip)
{
	return container_of(chip, struct cs752x_nand_host, chip);
}

static inline u32 fl_readl(struct cs752x_nand_host *h, u32 reg)
{
	return readl(h->iobase_fl + reg);
}

static inline void fl_writel(struct cs752x_nand_host *h, u32 reg, u32 val)
{
	writel(val, h->iobase_fl + reg);
}

static int check_flash_ctrl_status(struct cs752x_nand_host *h)
{
	u32 val;
	int ret;

	ret = readl_poll_timeout(h->iobase_fl + FLASH_STATUS, val,
				 !(val & FLASH_STATUS_NSTATE_MASK), 1, 1000000);
	if (ret)
		dev_err(h->dev, "FLASH_STATUS busy: 0x%x\n", val);
	return ret;
}

static int wait_reg(struct cs752x_nand_host *h, u32 reg, u32 bit, const char *what)
{
	u32 val;
	int ret;

	ret = readl_poll_timeout(h->iobase_fl + reg, val, val & bit, 1, 2000000);
	if (ret)
		dev_err(h->dev, "%s timeout\n", what);
	return ret;
}

static void reset_ecc_bch_registers(struct cs752x_nand_host *h)
{
	fl_writel(h, FLASH_NF_ECC_RESET, ECC_RESET_ECC_CLR | ECC_RESET_FIFO_CLR);
	fl_writel(h, FLASH_FLASH_INTERRUPT, FLASH_INT_REGIRQ);
	fl_writel(h, FLASH_NF_ECC_RESET, ECC_RESET_ECC_CLR);
	fl_writel(h, FLASH_NF_BCH_CONTROL, 0);
	fl_writel(h, FLASH_NF_ECC_CONTROL, 0);
}

static int cs752x_page_op_begin(struct cs752x_nand_host *h)
{
	int ret = check_flash_ctrl_status(h);

	if (ret) {
		fl_writel(h, FLASH_NF_ECC_RESET,
			  ECC_RESET_ECC_CLR | ECC_RESET_FIFO_CLR | ECC_RESET_NF_RESET);
		return ret;
	}
	reset_ecc_bch_registers(h);
	return 0;
}

static int cs752x_nand_words(struct cs752x_nand_host *h, u8 *buf,
			     unsigned int len, bool write)
{
	u32 value;
	unsigned int i;
	int ret;

	if (len % sizeof(u32))
		return -EINVAL;
	for (i = 0; i < len; i += sizeof(u32)) {
		if (write)
			fl_writel(h, FLASH_NF_DATA, get_unaligned_le32(buf + i));
		fl_writel(h, FLASH_FLASH_ACCESS_START,
			  FLASH_START_REGREQ | FLASH_START_CMD(write ? FLASH_WT : FLASH_RD));
		ret = readl_poll_timeout_atomic(h->iobase_fl + FLASH_FLASH_ACCESS_START,
						value, !(value & FLASH_START_REGREQ), 1, 10000);
		if (ret) {
			fl_writel(h, FLASH_NF_ECC_RESET,
				  ECC_RESET_ECC_CLR | ECC_RESET_FIFO_CLR | ECC_RESET_NF_RESET);
			return ret;
		}
		if (!write)
			put_unaligned_le32(fl_readl(h, FLASH_NF_DATA), buf + i);
	}
	return 0;
}

static int fill_hamming_oob_data(struct cs752x_nand_host *h)
{
	struct nand_chip *chip = &h->chip;
	struct mtd_info *mtd = nand_to_mtd(chip);
	u8 *ecc_calc = h->ecc_calc;
	int eccsteps = chip->ecc.steps, eccbytes = chip->ecc.bytes;
	int i, j, ret;

	ret = wait_reg(h, FLASH_NF_ECC_STATUS, ECC_STS_DONE, "ecc gen");
	if (ret)
		return ret;
	fl_writel(h, FLASH_NF_ECC_CONTROL,
		  fl_readl(h, FLASH_NF_ECC_CONTROL) & ~ECC_CTL_EN);

	for (i = 0, j = 0; eccsteps; eccsteps--, i++, j += eccbytes) {
		u32 v = fl_readl(h, FLASH_NF_ECC_GEN0 + 4 * i);

		ecc_calc[j]     = v & 0xff;
		ecc_calc[j + 1] = (v >> 8) & 0xff;
		ecc_calc[j + 2] = (v >> 16) & 0xff;
	}
	return mtd_ooblayout_set_eccbytes(mtd, ecc_calc, chip->oob_poi, 0,
					  chip->ecc.total);
}

static int hamming_correct(struct cs752x_nand_host *h, u8 *buf, int page)
{
	struct nand_chip *chip = &h->chip;
	struct mtd_info *mtd = nand_to_mtd(chip);
	u8 *ecc_code = h->ecc_code;
	int eccsteps = chip->ecc.steps, eccbytes = chip->ecc.bytes;
	int eccsize = chip->ecc.size;
	int i, max_bitflips = 0, ret;
	u8 *p = buf;

	ret = wait_reg(h, FLASH_NF_ECC_STATUS, ECC_STS_DONE, "ecc gen");
	if (ret)
		return ret;
	fl_writel(h, FLASH_NF_ECC_CONTROL,
		  fl_readl(h, FLASH_NF_ECC_CONTROL) & ~ECC_CTL_EN);

	ret = mtd_ooblayout_get_eccbytes(mtd, ecc_code, chip->oob_poi, 0, chip->ecc.total);
	if (ret)
		return ret;

	for (i = 0; eccsteps; eccsteps--, i += eccbytes, p += eccsize) {
		u32 sts, ctl;
		int step = i / eccbytes;

		fl_writel(h, FLASH_NF_ECC_OOB,
			  ecc_code[i] | (ecc_code[i + 1] << 8) | (ecc_code[i + 2] << 16));
		ctl = fl_readl(h, FLASH_NF_ECC_CONTROL) & ~ECC_CTL_CODESEL(0xf);
		fl_writel(h, FLASH_NF_ECC_CONTROL, ctl | ECC_CTL_CODESEL(step));

		sts = fl_readl(h, FLASH_NF_ECC_STATUS);
		switch (ECC_STS_STATUS(sts)) {
		case ECC_NO_ERR:
			break;
		case ECC_1BIT_DATA_ERR:
			if (ECC_STS_ERRBYTE(sts) >= eccsize || ECC_STS_ERRBIT(sts) >= 8) {
				mtd->ecc_stats.failed++;
				break;
			}
			p[ECC_STS_ERRBYTE(sts)] ^= BIT(ECC_STS_ERRBIT(sts));
			mtd->ecc_stats.corrected++;
			max_bitflips = max(max_bitflips, 1);
			dev_dbg(h->dev, "page %d step %d: 1-bit data error corrected\n",
				page, step);
			break;
		case ECC_1BIT_ECC_ERR:
			mtd->ecc_stats.corrected++;
			max_bitflips = max(max_bitflips, 1);
			dev_dbg(h->dev, "page %d step %d: 1-bit ecc error\n", page, step);
			break;
		case ECC_UNCORRECTABLE:
			mtd->ecc_stats.failed++;
			dev_dbg(h->dev, "page %d step %d: uncorrectable\n", page, step);
			break;
		}
	}
	return max_bitflips;
}

static int cs752x_nand_supports_op(const struct nand_operation *op,
				   struct cs752x_nand_op *p)
{
	unsigned int i, state = 0;

	memset(p, 0, sizeof(*p));
	p->timeout_ms = 100;
	if (op->cs || !op->ninstrs || op->ninstrs > 8)
		return -EOPNOTSUPP;
	for (i = 0; i < op->ninstrs; i++) {
		const struct nand_op_instr *in = &op->instrs[i];
		unsigned int j;

		p->delay_ns = max(p->delay_ns, in->delay_ns);
		switch (in->type) {
		case NAND_OP_CMD_INSTR:
			/* Two prefix commands and one confirm cover small-page NAND. */
			if (p->ncmds == 3 || state == 2 || state == 4 || state == 5 ||
			    p->wait_ready ||
			    (state == 0 && p->ncmds == 2))
				return -EOPNOTSUPP;
			p->command |= (u32)in->ctx.cmd.opcode << (p->ncmds++ * 8);
			if (state == 3)
				state = 5;
			else if (state == 1)
				state = 2;
			break;
		case NAND_OP_ADDR_INSTR:
			if (state || p->wait_ready || !p->ncmds || p->naddrs ||
			    !in->ctx.addr.naddrs || in->ctx.addr.naddrs > 5)
				return -EOPNOTSUPP;
			p->naddrs = in->ctx.addr.naddrs;
			for (j = 0; j < p->naddrs; j++)
				p->address |= (u64)in->ctx.addr.addrs[j] << (j * 8);
			state = 1;
			break;
		case NAND_OP_DATA_IN_INSTR:
		case NAND_OP_DATA_OUT_INSTR:
			if (p->data || !in->ctx.data.len ||
			    in->ctx.data.len > CS75XX_OWN_BUF_SIZE || state == 5)
				return -EOPNOTSUPP;
			if (in->type == NAND_OP_DATA_OUT_INSTR &&
			    (state == 2 || p->wait_ready ||
			     in->ctx.data.len % sizeof(u32)))
				return -EOPNOTSUPP;
			p->data = in;
			state = in->type == NAND_OP_DATA_IN_INSTR ? 4 : 3;
			break;
		case NAND_OP_WAITRDY_INSTR:
			if (p->wait_ready || !in->ctx.waitrdy.timeout_ms ||
			    state == 3 || state == 4)
				return -EOPNOTSUPP;
			p->wait_ready = true;
			p->timeout_ms = in->ctx.waitrdy.timeout_ms;
			break;
		default:
			return -EOPNOTSUPP;
		}
	}
	if (!p->ncmds)
		return op->ninstrs == 1 && (p->wait_ready || p->data) ?
		       0 : -EOPNOTSUPP;
	return 0;
}

static int cs752x_nand_wait_ready(struct cs752x_nand_host *h,
				  unsigned int timeout_ms)
{
	const struct nand_interface_config *conf;
	u32 value;

	/* R/B can remain high for tWB after a command has been issued. */
	conf = nand_get_interface_config(&h->chip);
	ndelay(NAND_COMMON_TIMING_NS(conf, tWB_max));

	/* nflash_rbIn_d is the sampled NAND R/B input, not the FSM state. */
	return readl_poll_timeout(h->iobase_fl + FLASH_STATUS, value,
				 value & BIT(26), 1, (u64)timeout_ms * 1000);
}

static int cs752x_nand_hwecc_op(struct cs752x_nand_host *h,
				const struct cs752x_nand_op *p)
{
	struct nand_chip *chip = &h->chip;
	struct mtd_info *mtd = nand_to_mtd(chip);
	bool write = p->data->type == NAND_OP_DATA_OUT_INSTR;
	u32 count, access;
	int ret;

	ret = cs752x_page_op_begin(h);
	if (ret)
		return ret;
	h->buf_data_len = 0;
	h->buf_offs = 0;
	fl_writel(h, FLASH_NF_ECC_CONTROL, ECC_CTL_EN);
	count = NF_CNT_DATA(mtd->writesize - 1) | NF_CNT_OOB(mtd->oobsize - 1) |
		NF_CNT_ADDR(p->naddrs - 1) | NF_CNT_CMD(p->ncmds - 1);
	access = NF_ACCESS_WIDTH32 |
		 NF_ACCESS_EXTADDR(((p->address >> 16) << chip->page_shift) >> 27);
	if (h->cs)
		access |= NF_ACCESS_CE_ALT;
	fl_writel(h, FLASH_NF_COUNT, count);
	fl_writel(h, FLASH_NF_COMMAND, p->command);
	fl_writel(h, FLASH_NF_ADDRESS_1, lower_32_bits(p->address));
	fl_writel(h, FLASH_NF_ADDRESS_2, upper_32_bits(p->address));
	fl_writel(h, FLASH_NF_ACCESS, access);
	ret = cs752x_nand_words(h, write ? (u8 *)p->data->ctx.data.buf.out :
				       p->data->ctx.data.buf.in,
			       mtd->writesize, write);
	if (!ret && write)
		ret = fill_hamming_oob_data(h);
	if (!ret)
		ret = cs752x_nand_words(h, chip->oob_poi, mtd->oobsize, write);
	if (!ret && p->wait_ready)
		ret = cs752x_nand_wait_ready(h, p->timeout_ms);
	if (p->delay_ns)
		ndelay(p->delay_ns);
	if (ret)
		fl_writel(h, FLASH_NF_ECC_RESET,
			  ECC_RESET_ECC_CLR | ECC_RESET_FIFO_CLR | ECC_RESET_NF_RESET);
	/* Read ECC registers remain live until the caller corrects the data. */
	return ret;
}

static int cs752x_nand_pio(struct cs752x_nand_host *h,
			   const struct cs752x_nand_op *p, u8 *buf,
			   unsigned int len, bool write)
{
	u32 value;
	unsigned int i, step = write && len ? sizeof(u32) : 1;
	int ret;

	ret = check_flash_ctrl_status(h);
	if (ret)
		return ret;
	reset_ecc_bch_registers(h);
	fl_writel(h, FLASH_NF_COUNT,
		  NF_CNT_OOB(NCNT_EMPTY_OOB) |
		  NF_CNT_DATA(len ? len - 1 : NCNT_EMPTY_DATA) |
		  NF_CNT_ADDR(p->naddrs ? p->naddrs - 1 : NCNT_EMPTY_ADDR) |
		  NF_CNT_CMD(p->ncmds ? p->ncmds - 1 : NCNT_EMPTY_CMD));
	fl_writel(h, FLASH_NF_COMMAND, p->command);
	fl_writel(h, FLASH_NF_ADDRESS_1, lower_32_bits(p->address));
	fl_writel(h, FLASH_NF_ADDRESS_2, upper_32_bits(p->address));
	/*
	 * Explicit row cycles are in ADDRESS_1/_2. Match the vendor erase
	 * and OOB register-PIO paths, which leave aperture EXTADDR zero.
	 */
	fl_writel(h, FLASH_NF_ACCESS,
		  (write && len ? NF_ACCESS_WIDTH32 : NF_ACCESS_WIDTH8) |
		  (h->cs ? NF_ACCESS_CE_ALT : 0));

	for (i = 0; i < max(len, 1U); i += step) {
		if (write && len)
			fl_writel(h, FLASH_NF_DATA, get_unaligned_le32(buf + i));
		fl_writel(h, FLASH_FLASH_ACCESS_START,
			  FLASH_START_REGREQ | FLASH_START_CMD(write ? FLASH_WT : FLASH_RD));
		/* Byte PIO completes in microseconds, below scheduler granularity. */
		ret = readl_poll_timeout_atomic(h->iobase_fl + FLASH_FLASH_ACCESS_START,
						value, !(value & FLASH_START_REGREQ), 1,
					(u64)p->timeout_ms * 1000);
		if (ret)
			goto out;
		if (!write && len)
			buf[i] = fl_readl(h, FLASH_NF_DATA) >> ((i % 4) * 8);
	}
	ret = p->wait_ready ? cs752x_nand_wait_ready(h, p->timeout_ms) : 0;
	if (p->delay_ns)
		ndelay(p->delay_ns);
out:
	fl_writel(h, FLASH_NF_ECC_RESET,
		  ECC_RESET_ECC_CLR | ECC_RESET_FIFO_CLR | ECC_RESET_NF_RESET);
	return ret;
}

static int cs752x_nand_exec_op(struct nand_chip *chip,
			       const struct nand_operation *op, bool check_only)
{
	struct cs752x_nand_host *h = to_host(chip);
	struct mtd_info *mtd = nand_to_mtd(chip);
	struct cs752x_nand_op p;
	unsigned int len, requested, col;
	int ret;

	ret = cs752x_nand_supports_op(op, &p);
	if (ret || check_only)
		return ret;
	requested = p.data ? p.data->ctx.data.len : 0;
	if (!p.ncmds && p.wait_ready) {
		ret = cs752x_nand_wait_ready(h, p.timeout_ms);
		if (p.delay_ns)
			ndelay(p.delay_ns);
		return ret;
	}
	if (h->use_hwecc && p.data && p.ncmds == 2 &&
	    p.naddrs >= 4 && !(p.address & 0xffff) &&
	    requested == mtd->writesize)
		return cs752x_nand_hwecc_op(h, &p);
	if (!p.ncmds && p.data->type == NAND_OP_DATA_OUT_INSTR) {
		h->buf_data_len = 0;
		h->buf_offs = 0;
		return cs752x_nand_pio(h, &p, (u8 *)p.data->ctx.data.buf.out,
				      requested, true);
	}
	if (!p.ncmds) {
		if (h->read_mode != NAND_CMD_STATUS)
			goto cached;
		if (requested != 1)
			return -EOPNOTSUPP;
		p.command = NAND_CMD_STATUS;
		p.ncmds = 1;
	}
	if (p.command == (NAND_CMD_RNDOUT | (NAND_CMD_RNDOUTSTART << 8))) {
		col = p.address;
		if (h->read_mode != NAND_CMD_PARAM && h->read_mode != NAND_CMD_READ0)
			return -EINVAL;
		if (col < h->cache_column || col - h->cache_column > h->buf_data_len)
			return -EINVAL;
		h->buf_offs = col - h->cache_column;
		goto cached;
	}
	h->buf_offs = 0;
	h->buf_data_len = 0;
	h->read_mode = p.command & 0xff;
	h->cache_column = 0;
	len = p.data && p.data->type == NAND_OP_DATA_IN_INSTR ? requested : 0;
	switch (p.command) {
	case NAND_CMD_READID:
		len = max(requested, 8U);
		break;
	case NAND_CMD_PARAM:
		/* Keep all standard ONFI copies plus room for extended parameters. */
		len = sizeof(h->own_buf);
		break;
	case NAND_CMD_STATUS:
		len = 1;
		break;
	case NAND_CMD_READ0 | (NAND_CMD_READSTART << 8):
		col = p.address & 0xffff;
		if (col >= mtd->writesize + mtd->oobsize)
			return -EINVAL;
		h->cache_column = col;
		len = mtd->writesize + mtd->oobsize - col;
		break;
	default:
		break;
	}
	if (len > sizeof(h->own_buf) || (requested > len &&
					 (!p.data || p.data->type == NAND_OP_DATA_IN_INSTR)))
		return -EOPNOTSUPP;
	if (p.data && p.data->type == NAND_OP_DATA_OUT_INSTR)
		return cs752x_nand_pio(h, &p, (u8 *)p.data->ctx.data.buf.out,
				      requested, true);

	ret = cs752x_nand_pio(h, &p, h->own_buf, len,
			      p.command == NAND_CMD_RESET);
	if (ret)
		return ret;
	h->buf_data_len = len;
cached:
	if (!requested)
		return 0;
	if (h->buf_offs > h->buf_data_len ||
	    requested > h->buf_data_len - h->buf_offs)
		return -EOPNOTSUPP;
	memcpy(p.data->ctx.data.buf.in, h->own_buf + h->buf_offs, requested);
	h->buf_offs += requested;
	return 0;
}

static int cs752x_nand_read_page_hwecc(struct nand_chip *chip, u8 *buf,
				       int oob_required, int page)
{
	struct cs752x_nand_host *h = to_host(chip);
	struct mtd_info *mtd = nand_to_mtd(chip);
	int ret;

	h->use_hwecc = true;
	ret = nand_read_page_op(chip, page, 0, buf, mtd->writesize);
	h->use_hwecc = false;
	if (ret)
		return ret;
	return hamming_correct(h, buf, page);
}

static int cs752x_nand_write_page_hwecc(struct nand_chip *chip, const u8 *buf,
					int oob_required, int page)
{
	struct cs752x_nand_host *h = to_host(chip);
	struct mtd_info *mtd = nand_to_mtd(chip);
	int ret;

	h->use_hwecc = true;
	ret = nand_prog_page_op(chip, page, 0, buf, mtd->writesize);
	h->use_hwecc = false;
	return ret;
}

static int cs752x_nand_attach_chip(struct nand_chip *chip)
{
	struct mtd_info *mtd = nand_to_mtd(chip);
	struct cs752x_nand_host *h = to_host(chip);
	int ecc_total;

	/* Byte-bus transfer handling and the staging buffer are driver limits. */
	if (chip->options & NAND_BUSWIDTH_16 ||
	    mtd->writesize + mtd->oobsize > sizeof(h->own_buf) ||
	    mtd->writesize % sizeof(u32) || mtd->oobsize % sizeof(u32))
		return -EOPNOTSUPP;
	chip->ecc.read_page_raw = nand_monolithic_read_page_raw;
	chip->ecc.write_page_raw = nand_monolithic_write_page_raw;
	/* The core overrides raw callbacks for NONE with split program ops. */
	if (chip->ecc.engine_type == NAND_ECC_ENGINE_TYPE_SOFT)
		return 0;
	if ((mtd->writesize != SZ_2K && mtd->writesize != SZ_4K) ||
	    (mtd->oobsize != 64 && mtd->oobsize != 128))
		return -EOPNOTSUPP;
	if (chip->ecc.engine_type != NAND_ECC_ENGINE_TYPE_ON_HOST ||
	    (chip->ecc.placement != NAND_ECC_PLACEMENT_UNKNOWN &&
	     chip->ecc.placement != NAND_ECC_PLACEMENT_OOB) ||
	    (chip->ecc.algo != NAND_ECC_ALGO_UNKNOWN &&
	     chip->ecc.algo != NAND_ECC_ALGO_HAMMING) ||
	    (chip->ecc.size && chip->ecc.size != 256) ||
	    (chip->ecc.strength && chip->ecc.strength != 1))
		return -EOPNOTSUPP;

	ecc_total = mtd->writesize / 256 * 3;
	/* The standard Hamming layout reserves the last 24 or 48 OOB bytes. */
	if (ecc_total > (mtd->oobsize == 64 ? 24 : 48))
		return -EINVAL;
	chip->ecc.placement = NAND_ECC_PLACEMENT_OOB;
	chip->ecc.algo = NAND_ECC_ALGO_HAMMING;
	chip->ecc.size = 256;
	chip->ecc.bytes = 3;
	chip->ecc.strength = 1;

	h->ecc_calc = devm_kzalloc(h->dev, mtd->oobsize, GFP_KERNEL);
	h->ecc_code = devm_kzalloc(h->dev, mtd->oobsize, GFP_KERNEL);
	if (!h->ecc_calc || !h->ecc_code)
		return -ENOMEM;

	mtd_set_ooblayout(mtd, nand_get_large_page_hamming_ooblayout());

	chip->ecc.read_page	 = cs752x_nand_read_page_hwecc;
	chip->ecc.write_page	 = cs752x_nand_write_page_hwecc;

	dev_info(h->dev, "HW Hamming ECC, %d bytes per %d-byte step, page %u, oob %u\n",
		 chip->ecc.bytes, chip->ecc.size, mtd->writesize, mtd->oobsize);
	return 0;
}

static const struct nand_controller_ops cs752x_nand_controller_ops = {
	.attach_chip = cs752x_nand_attach_chip,
	.exec_op = cs752x_nand_exec_op,
};

static int cs752x_nand_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	struct cs752x_nand_host *h;
	struct nand_chip *chip;
	struct mtd_info *mtd;
	struct device_node *flash;
	struct reset_control *reset;
	struct clk *clk;
	u32 cs;
	int err;

	h = devm_kzalloc(dev, sizeof(*h), GFP_KERNEL);
	if (!h)
		return -ENOMEM;
	h->dev = dev;

	clk = devm_clk_get_enabled(dev, NULL);
	if (IS_ERR(clk))
		return dev_err_probe(dev, PTR_ERR(clk), "AXI clock\n");
	err = devm_clk_rate_exclusive_get(dev, clk);
	if (err)
		return dev_err_probe(dev, err, "retain firmware bus timing\n");
	reset = devm_reset_control_get_optional_exclusive(dev, NULL);
	if (IS_ERR(reset))
		return dev_err_probe(dev, PTR_ERR(reset), "flash reset\n");
	/* Preserve the bootloader timing and data: never pulse flash reset. */
	err = reset_control_deassert(reset);
	if (err)
		return dev_err_probe(dev, err, "release flash reset\n");

	h->iobase_fl = devm_platform_ioremap_resource(pdev, 0);
	if (IS_ERR(h->iobase_fl))
		return PTR_ERR(h->iobase_fl);

	nand_controller_init(&h->controller);
	h->controller.ops = &cs752x_nand_controller_ops;

	chip = &h->chip;
	mtd = nand_to_mtd(chip);
	chip->controller = &h->controller;
	if (of_get_available_child_count(dev->of_node) != 1)
		return dev_err_probe(dev, -EOPNOTSUPP,
				     "one connected NAND target is supported\n");
	flash = of_get_next_available_child(dev->of_node, NULL);
	if (!flash || !of_node_name_eq(flash, "nand")) {
		of_node_put(flash);
		return dev_err_probe(dev, -ENODEV, "missing NAND chip node\n");
	}
	err = of_property_read_u32(flash, "reg", &cs);
	if (err || cs > 1) {
		of_node_put(flash);
		return dev_err_probe(dev, -EINVAL, "invalid chip select\n");
	}
	h->cs = cs;
	nand_set_flash_node(chip, flash);
	of_node_put(flash);
	nand_set_controller_data(chip, h);
	mtd->dev.parent = dev;
	if (!mtd->name)
		mtd->name = "cs752x_nand_flash";

	chip->options |= NAND_NO_SUBPAGE_WRITE;

	dev_info(dev, "flash type 0x%08x\n", fl_readl(h, FLASH_TYPE));

	err = nand_scan(chip, 1);
	if (err)
		return dev_err_probe(dev, err, "NAND scan failed\n");

	err = mtd_device_register(mtd, NULL, 0);
	if (err) {
		nand_cleanup(chip);
		return err;
	}

	platform_set_drvdata(pdev, h);
	return 0;
}

static void cs752x_nand_remove(struct platform_device *pdev)
{
	struct cs752x_nand_host *h = platform_get_drvdata(pdev);
	struct nand_chip *chip = &h->chip;

	WARN_ON(mtd_device_unregister(nand_to_mtd(chip)));
	nand_cleanup(chip);
}

static const struct of_device_id cs752x_nand_dt_ids[] = {
	{ .compatible = "cortina,cs7542-nand" },
	{}
};
MODULE_DEVICE_TABLE(of, cs752x_nand_dt_ids);

static struct platform_driver cs752x_nand_driver = {
	.driver = {
		.name	= "cs752x_nand",
		.of_match_table = cs752x_nand_dt_ids,
		/* Fixed flash has no safe manual detach with active MTD users. */
		.suppress_bind_attrs = true,
	},
	.probe		= cs752x_nand_probe,
	.remove		= cs752x_nand_remove,
};
module_platform_driver(cs752x_nand_driver);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("NAND flash driver for Cortina CS7542");
MODULE_ALIAS("platform:cs752x_nand");

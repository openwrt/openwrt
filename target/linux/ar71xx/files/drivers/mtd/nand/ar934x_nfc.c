/*
 * Driver for the built-in NAND controller of the Atheros AR934x SoCs
 *
 * Copyright (C) 2011-2012 Gabor Juhos <juhosg@openwrt.org>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published
 * by the Free Software Foundation.
 */

#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/module.h>
#include <linux/dma-mapping.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/nand.h>
#include <linux/mtd/partitions.h>
#include <linux/platform_device.h>
#include <linux/delay.h>
#include <linux/slab.h>

#include <linux/platform/ar934x_nfc.h>

#define AR934X_NFC_REG_CMD		0x00
#define AR934X_NFC_REG_CTRL		0x04
#define AR934X_NFC_REG_STATUS		0x08
#define AR934X_NFC_REG_INT_MASK		0x0c
#define AR934X_NFC_REG_INT_STATUS	0x10
#define AR934X_NFC_REG_ECC_CTRL		0x14
#define AR934X_NFC_REG_ECC_OFFSET	0x18
#define AR934X_NFC_REG_ADDR0_0		0x1c
#define AR934X_NFC_REG_ADDR0_1		0x24
#define AR934X_NFC_REG_ADDR1_0		0x20
#define AR934X_NFC_REG_ADDR1_1		0x28
#define AR934X_NFC_REG_SPARE_SIZE	0x30
#define AR934X_NFC_REG_PROTECT		0x38
#define AR934X_NFC_REG_LOOKUP_EN	0x40
#define AR934X_NFC_REG_LOOKUP(_x)	(0x44 + (_i) * 4)
#define AR934X_NFC_REG_DMA_ADDR		0x64
#define AR934X_NFC_REG_DMA_COUNT	0x68
#define AR934X_NFC_REG_DMA_CTRL		0x6c
#define AR934X_NFC_REG_MEM_CTRL		0x80
#define AR934X_NFC_REG_DATA_SIZE	0x84
#define AR934X_NFC_REG_READ_STATUS	0x88
#define AR934X_NFC_REG_TIME_SEQ		0x8c
#define AR934X_NFC_REG_TIMINGS_ASYN	0x90
#define AR934X_NFC_REG_TIMINGS_SYN	0x94
#define AR934X_NFC_REG_FIFO_DATA	0x98
#define AR934X_NFC_REG_TIME_MODE	0x9c
#define AR934X_NFC_REG_DMA_ADDR_OFFS	0xa0
#define AR934X_NFC_REG_FIFO_INIT	0xb0
#define AR934X_NFC_REG_GEN_SEQ_CTRL	0xb4

#define AR934X_NFC_CMD_CMD_SEQ_S		0
#define AR934X_NFC_CMD_CMD_SEQ_M		0x3f
#define   AR934X_NFC_CMD_SEQ_1C			0x00
#define   AR934X_NFC_CMD_SEQ_ERASE		0x0e
#define   AR934X_NFC_CMD_SEQ_12			0x0c
#define   AR934X_NFC_CMD_SEQ_1C1AXR		0x21
#define   AR934X_NFC_CMD_SEQ_S			0x24
#define   AR934X_NFC_CMD_SEQ_1C3AXR		0x27
#define   AR934X_NFC_CMD_SEQ_1C5A1CXR		0x2a
#define   AR934X_NFC_CMD_SEQ_18			0x32
#define AR934X_NFC_CMD_INPUT_SEL_SIU		0
#define AR934X_NFC_CMD_INPUT_SEL_DMA		BIT(6)
#define AR934X_NFC_CMD_ADDR_SEL_0		0
#define AR934X_NFC_CMD_ADDR_SEL_1		BIT(7)
#define AR934X_NFC_CMD_CMD0_S			8
#define AR934X_NFC_CMD_CMD0_M			0xff
#define AR934X_NFC_CMD_CMD1_S			16
#define AR934X_NFC_CMD_CMD1_M			0xff
#define AR934X_NFC_CMD_CMD2_S			24
#define AR934X_NFC_CMD_CMD2_M			0xff

#define AR934X_NFC_CTRL_ADDR_CYCLE0_M		0x7
#define AR934X_NFC_CTRL_ADDR_CYCLE0_S		0
#define AR934X_NFC_CTRL_SPARE_EN		BIT(3)
#define AR934X_NFC_CTRL_INT_EN			BIT(4)
#define AR934X_NFC_CTRL_ECC_EN			BIT(5)
#define AR934X_NFC_CTRL_BLOCK_SIZE_S		6
#define AR934X_NFC_CTRL_BLOCK_SIZE_M		0x3
#define   AR934X_NFC_CTRL_BLOCK_SIZE_32		0
#define   AR934X_NFC_CTRL_BLOCK_SIZE_64		1
#define   AR934X_NFC_CTRL_BLOCK_SIZE_128	2
#define   AR934X_NFC_CTRL_BLOCK_SIZE_256	3
#define AR934X_NFC_CTRL_PAGE_SIZE_S		8
#define AR934X_NFC_CTRL_PAGE_SIZE_M		0x7
#define   AR934X_NFC_CTRL_PAGE_SIZE_256		0
#define   AR934X_NFC_CTRL_PAGE_SIZE_512		1
#define   AR934X_NFC_CTRL_PAGE_SIZE_1024	2
#define   AR934X_NFC_CTRL_PAGE_SIZE_2048	3
#define   AR934X_NFC_CTRL_PAGE_SIZE_4096	4
#define   AR934X_NFC_CTRL_PAGE_SIZE_8192	5
#define   AR934X_NFC_CTRL_PAGE_SIZE_16384	6
#define AR934X_NFC_CTRL_CUSTOM_SIZE_EN		BIT(11)
#define AR934X_NFC_CTRL_IO_WIDTH_8BITS		0
#define AR934X_NFC_CTRL_IO_WIDTH_16BITS		BIT(12)
#define AR934X_NFC_CTRL_LOOKUP_EN		BIT(13)
#define AR934X_NFC_CTRL_PROT_EN			BIT(14)
#define AR934X_NFC_CTRL_WORK_MODE_ASYNC		0
#define AR934X_NFC_CTRL_WORK_MODE_SYNC		BIT(15)
#define AR934X_NFC_CTRL_ADDR0_AUTO_INC		BIT(16)
#define AR934X_NFC_CTRL_ADDR1_AUTO_INC		BIT(17)
#define AR934X_NFC_CTRL_ADDR_CYCLE1_M		0x7
#define AR934X_NFC_CTRL_ADDR_CYCLE1_S		18
#define AR934X_NFC_CTRL_SMALL_PAGE		BIT(21)

#define AR934X_NFC_DMA_CTRL_DMA_START		BIT(7)
#define AR934X_NFC_DMA_CTRL_DMA_DIR_WRITE	0
#define AR934X_NFC_DMA_CTRL_DMA_DIR_READ	BIT(6)
#define AR934X_NFC_DMA_CTRL_DMA_MODE_SG		BIT(5)
#define AR934X_NFC_DMA_CTRL_DMA_BURST_S		2
#define AR934X_NFC_DMA_CTRL_DMA_BURST_0		0
#define AR934X_NFC_DMA_CTRL_DMA_BURST_1		1
#define AR934X_NFC_DMA_CTRL_DMA_BURST_2		2
#define AR934X_NFC_DMA_CTRL_DMA_BURST_3		3
#define AR934X_NFC_DMA_CTRL_DMA_BURST_4		4
#define AR934X_NFC_DMA_CTRL_DMA_BURST_5		5
#define AR934X_NFC_DMA_CTRL_ERR_FLAG		BIT(1)
#define AR934X_NFC_DMA_CTRL_DMA_READY		BIT(0)

#define AR934X_NFC_INT_DEV_RDY(_x)		BIT(4 + (_x))
#define AR934X_NFC_INT_CMD_END			BIT(1)

/* default timing values */
#define AR934X_NFC_TIME_SEQ_DEFAULT	0x7fff
#define AR934X_NFC_TIMINGS_ASYN_DEFAULT	0x22
#define AR934X_NFC_TIMINGS_SYN_DEFAULT	0xf

#define AR934X_NFC_ID_BUF_SIZE		8
#define AR934X_NFC_DEV_READY_TIMEOUT	25 /* msecs */
#define AR934X_NFC_DMA_READY_TIMEOUT	25 /* msecs */
#define AR934X_NFC_DONE_TIMEOUT		1000
#define AR934X_NFC_DMA_RETRIES		20

#define AR934X_NFC_USE_IRQ		true
#define AR934X_NFC_IRQ_MASK		AR934X_NFC_INT_DEV_RDY(0)

#define  AR934X_NFC_GENSEQ_SMALL_PAGE_READ	0x30043

#undef AR934X_NFC_DEBUG_DATA
#undef AR934X_NFC_DEBUG

struct ar934x_nfc;

static inline  __attribute__ ((format (printf, 2, 3)))
void _nfc_dbg(struct ar934x_nfc *nfc, const char *fmt, ...)
{
}

#ifdef AR934X_NFC_DEBUG
#define nfc_dbg(_nfc, fmt, ...) \
	dev_info((_nfc)->parent, fmt, ##__VA_ARGS__)
#else
#define nfc_dbg(_nfc, fmt, ...) \
	_nfc_dbg((_nfc), fmt, ##__VA_ARGS__)
#endif /* AR934X_NFC_DEBUG */

#ifdef AR934X_NFC_DEBUG_DATA
static void
nfc_debug_data(const char *label, void *data, int len)
{
	print_hex_dump(KERN_WARNING, label, DUMP_PREFIX_OFFSET, 16, 1,
		       data, len, 0);
}
#else
static inline void
nfc_debug_data(const char *label, void *data, int len) {}
#endif /* AR934X_NFC_DEBUG_DATA */

struct ar934x_nfc {
	struct mtd_info mtd;
	struct nand_chip nand_chip;
	struct device *parent;
	void __iomem *base;
	void (*select_chip)(int chip_no);
	int irq;
	wait_queue_head_t irq_waitq;

	bool spurious_irq_expected;
	u32 irq_status;

	u32 ctrl_reg;
	bool small_page;
	unsigned int addr_count0;
	unsigned int addr_count1;

	u8 *buf;
	dma_addr_t buf_dma;
	unsigned int buf_size;
	int buf_index;

	int erase1_page_addr;

	int rndout_page_addr;
	int rndout_read_cmd;

	int seqin_page_addr;
	int seqin_column;
	int seqin_read_cmd;
};

static void ar934x_nfc_restart(struct ar934x_nfc *nfc);

static inline void
ar934x_nfc_wr(struct ar934x_nfc *nfc, unsigned reg, u32 val)
{
	__raw_writel(val, nfc->base + reg);
}

static inline u32
ar934x_nfc_rr(struct ar934x_nfc *nfc, unsigned reg)
{
	return __raw_readl(nfc->base + reg);
}

static inline struct ar934x_nfc_platform_data *
ar934x_nfc_get_platform_data(struct ar934x_nfc *nfc)
{
	return nfc->parent->platform_data;
}

static inline struct
ar934x_nfc *mtd_to_ar934x_nfc(struct mtd_info *mtd)
{
	return container_of(mtd, struct ar934x_nfc, mtd);
}

static inline bool ar934x_nfc_use_irq(struct ar934x_nfc *nfc)
{
	return AR934X_NFC_USE_IRQ;
}

static inline void ar934x_nfc_write_cmd_reg(struct ar934x_nfc *nfc, u32 cmd_reg)
{
	wmb();

	ar934x_nfc_wr(nfc, AR934X_NFC_REG_CMD, cmd_reg);
	/* flush write */
	ar934x_nfc_rr(nfc, AR934X_NFC_REG_CMD);
}

static bool
__ar934x_nfc_dev_ready(struct ar934x_nfc *nfc)
{
	u32 status;

	status = ar934x_nfc_rr(nfc, AR934X_NFC_REG_STATUS);
	return (status & 0xff) == 0xff;
}

static inline bool
__ar934x_nfc_is_dma_ready(struct ar934x_nfc *nfc)
{
	u32 status;

	status = ar934x_nfc_rr(nfc, AR934X_NFC_REG_DMA_CTRL);
	return (status & AR934X_NFC_DMA_CTRL_DMA_READY) != 0;
}

static int
ar934x_nfc_wait_dev_ready(struct ar934x_nfc *nfc)
{
	unsigned long timeout;

	timeout = jiffies + msecs_to_jiffies(AR934X_NFC_DEV_READY_TIMEOUT);
	do {
		if (__ar934x_nfc_dev_ready(nfc))
			return 0;
	} while time_before(jiffies, timeout);

	nfc_dbg(nfc, "timeout waiting for device ready, status:%08x int:%08x\n",
		ar934x_nfc_rr(nfc, AR934X_NFC_REG_STATUS),
		ar934x_nfc_rr(nfc, AR934X_NFC_REG_INT_STATUS));
	return -ETIMEDOUT;
}

static int
ar934x_nfc_wait_dma_ready(struct ar934x_nfc *nfc)
{
	unsigned long timeout;

	timeout = jiffies + msecs_to_jiffies(AR934X_NFC_DMA_READY_TIMEOUT);
	do {
		if (__ar934x_nfc_is_dma_ready(nfc))
			return 0;
	} while time_before(jiffies, timeout);

	nfc_dbg(nfc, "timeout waiting for DMA ready, dma_ctrl:%08x\n",
		ar934x_nfc_rr(nfc, AR934X_NFC_REG_DMA_CTRL));
	return -ETIMEDOUT;
}

static int
ar934x_nfc_wait_irq(struct ar934x_nfc *nfc)
{
	long timeout;
	int ret;

	timeout = wait_event_timeout(nfc->irq_waitq,
				(nfc->irq_status & AR934X_NFC_IRQ_MASK) != 0,
				msecs_to_jiffies(AR934X_NFC_DEV_READY_TIMEOUT));

	ret = 0;
	if (!timeout) {
		ar934x_nfc_wr(nfc, AR934X_NFC_REG_INT_MASK, 0);
		ar934x_nfc_wr(nfc, AR934X_NFC_REG_INT_STATUS, 0);
		/* flush write */
		ar934x_nfc_rr(nfc, AR934X_NFC_REG_INT_STATUS);

		nfc_dbg(nfc,
			"timeout waiting for interrupt, status:%08x\n",
			nfc->irq_status);
		ret = -ETIMEDOUT;
	}

	nfc->irq_status = 0;
	return ret;
}

static int
ar934x_nfc_wait_done(struct ar934x_nfc *nfc)
{
	int ret;

	if (ar934x_nfc_use_irq(nfc))
		ret = ar934x_nfc_wait_irq(nfc);
	else
		ret = ar934x_nfc_wait_dev_ready(nfc);

	if (ret)
		return ret;

	return ar934x_nfc_wait_dma_ready(nfc);
}

static int
ar934x_nfc_alloc_buf(struct ar934x_nfc *nfc, unsigned size)
{
	nfc->buf = dma_alloc_coherent(nfc->parent, size,
				      &nfc->buf_dma, GFP_KERNEL);
	if (nfc->buf == NULL) {
		dev_err(nfc->parent, "no memory for DMA buffer\n");
		return -ENOMEM;
	}

	nfc->buf_size = size;
	nfc_dbg(nfc, "buf:%p size:%u\n", nfc->buf, nfc->buf_size);

	return 0;
}

static void
ar934x_nfc_free_buf(struct ar934x_nfc *nfc)
{
	dma_free_coherent(nfc->parent, nfc->buf_size, nfc->buf, nfc->buf_dma);
}

static void
ar934x_nfc_get_addr(struct ar934x_nfc *nfc, int column, int page_addr,
		    u32 *addr0, u32 *addr1)
{
	u32 a0, a1;

	a0 = 0;
	a1 = 0;

	if (column == -1) {
		/* ERASE1 */
		a0 = (page_addr & 0xffff) << 16;
		a1 = (page_addr >> 16) & 0xf;
	} else if (page_addr != -1) {
		/* SEQIN, READ0, etc.. */

		/* TODO: handle 16bit bus width */
		if (nfc->small_page) {
			a0 = column & 0xff;
			a0 |= (page_addr & 0xff) << 8;
			a0 |= ((page_addr >> 8) & 0xff) << 16;
			a0 |= ((page_addr >> 16) & 0xff) << 24;
		} else {
			a0 = column & 0x0FFF;
			a0 |= (page_addr & 0xffff) << 16;

			if (nfc->addr_count0 > 4)
				a1 = (page_addr >> 16) & 0xf;
		}
	}

	*addr0 = a0;
	*addr1 = a1;
}

static void
ar934x_nfc_send_cmd(struct ar934x_nfc *nfc, unsigned command)
{
	u32 cmd_reg;

	cmd_reg = AR934X_NFC_CMD_INPUT_SEL_SIU | AR934X_NFC_CMD_ADDR_SEL_0 |
		  AR934X_NFC_CMD_SEQ_1C;
	cmd_reg |= (command & AR934X_NFC_CMD_CMD0_M) << AR934X_NFC_CMD_CMD0_S;

	ar934x_nfc_wr(nfc, AR934X_NFC_REG_INT_STATUS, 0);
	ar934x_nfc_wr(nfc, AR934X_NFC_REG_CTRL, nfc->ctrl_reg);

	ar934x_nfc_write_cmd_reg(nfc, cmd_reg);
	ar934x_nfc_wait_dev_ready(nfc);
}

static void
ar934x_nfc_do_rw_command(struct ar934x_nfc *nfc, int column, int page_addr,
			 int len, u32 cmd_reg, u32 ctrl_reg, bool write)
{
	u32 addr0, addr1;
	u32 dma_ctrl;
	int dir;
	int err;
	int retries = 0;

	WARN_ON(len & 3);

	if (WARN_ON(len > nfc->buf_size))
		dev_err(nfc->parent, "len=%d > buf_size=%d", len, nfc->buf_size);

	if (write) {
		dma_ctrl = AR934X_NFC_DMA_CTRL_DMA_DIR_WRITE;
		dir = DMA_TO_DEVICE;
	} else {
		dma_ctrl = AR934X_NFC_DMA_CTRL_DMA_DIR_READ;
		dir = DMA_FROM_DEVICE;
	}

	ar934x_nfc_get_addr(nfc, column, page_addr, &addr0, &addr1);

	dma_ctrl |= AR934X_NFC_DMA_CTRL_DMA_START |
		    (AR934X_NFC_DMA_CTRL_DMA_BURST_3 <<
		     AR934X_NFC_DMA_CTRL_DMA_BURST_S);

	cmd_reg |= AR934X_NFC_CMD_INPUT_SEL_DMA | AR934X_NFC_CMD_ADDR_SEL_0;
	ctrl_reg |= AR934X_NFC_CTRL_INT_EN;

	nfc_dbg(nfc, "%s a0:%08x a1:%08x len:%x cmd:%08x dma:%08x ctrl:%08x\n",
		(write) ? "write" : "read",
		addr0, addr1, len, cmd_reg, dma_ctrl, ctrl_reg);

retry:
	ar934x_nfc_wr(nfc, AR934X_NFC_REG_INT_STATUS, 0);
	ar934x_nfc_wr(nfc, AR934X_NFC_REG_ADDR0_0, addr0);
	ar934x_nfc_wr(nfc, AR934X_NFC_REG_ADDR0_1, addr1);
	ar934x_nfc_wr(nfc, AR934X_NFC_REG_DMA_ADDR, nfc->buf_dma);
	ar934x_nfc_wr(nfc, AR934X_NFC_REG_DMA_COUNT, len);
	ar934x_nfc_wr(nfc, AR934X_NFC_REG_DATA_SIZE, len);
	ar934x_nfc_wr(nfc, AR934X_NFC_REG_CTRL, ctrl_reg);
	ar934x_nfc_wr(nfc, AR934X_NFC_REG_DMA_CTRL, dma_ctrl);

	if (ar934x_nfc_use_irq(nfc)) {
		ar934x_nfc_wr(nfc, AR934X_NFC_REG_INT_MASK, AR934X_NFC_IRQ_MASK);
		/* flush write */
		ar934x_nfc_rr(nfc, AR934X_NFC_REG_INT_MASK);
	}

	ar934x_nfc_write_cmd_reg(nfc, cmd_reg);
	err = ar934x_nfc_wait_done(nfc);
	if (err) {
		dev_dbg(nfc->parent, "%s operation stuck at page %d\n",
			(write) ? "write" : "read", page_addr);

		ar934x_nfc_restart(nfc);
		if (retries++ < AR934X_NFC_DMA_RETRIES)
			goto retry;

		dev_err(nfc->parent, "%s operation failed on page %d\n",
			(write) ? "write" : "read", page_addr);
	}
}

static void
ar934x_nfc_send_readid(struct ar934x_nfc *nfc, unsigned command)
{
	u32 cmd_reg;

	nfc_dbg(nfc, "readid, cmd:%02x\n", command);

	cmd_reg = AR934X_NFC_CMD_SEQ_1C1AXR;
	cmd_reg |= (command & AR934X_NFC_CMD_CMD0_M) << AR934X_NFC_CMD_CMD0_S;

	ar934x_nfc_do_rw_command(nfc, -1, -1, AR934X_NFC_ID_BUF_SIZE, cmd_reg,
				 nfc->ctrl_reg, false);

	nfc_debug_data("[id] ", nfc->buf, AR934X_NFC_ID_BUF_SIZE);
}

static void
ar934x_nfc_send_read(struct ar934x_nfc *nfc, unsigned command, int column,
		     int page_addr, int len)
{
	u32 cmd_reg;

	nfc_dbg(nfc, "read, column=%d page=%d len=%d\n",
		column, page_addr, len);

	cmd_reg = (command & AR934X_NFC_CMD_CMD0_M) << AR934X_NFC_CMD_CMD0_S;

	if (nfc->small_page) {
		cmd_reg |= AR934X_NFC_CMD_SEQ_18;
	} else {
		cmd_reg |= NAND_CMD_READSTART << AR934X_NFC_CMD_CMD1_S;
		cmd_reg |= AR934X_NFC_CMD_SEQ_1C5A1CXR;
	}

	ar934x_nfc_do_rw_command(nfc, column, page_addr, len,
				 cmd_reg, nfc->ctrl_reg, false);

	nfc_debug_data("[data] ", nfc->buf, len);
}

static void
ar934x_nfc_send_erase(struct ar934x_nfc *nfc, unsigned command, int column,
		      int page_addr)
{
	u32 addr0, addr1;
	u32 ctrl_reg;
	u32 cmd_reg;

	ar934x_nfc_get_addr(nfc, column, page_addr, &addr0, &addr1);

	ctrl_reg = nfc->ctrl_reg;
	if (nfc->small_page) {
		/* override number of address cycles for the erase command */
		ctrl_reg &= ~(AR934X_NFC_CTRL_ADDR_CYCLE0_M <<
			      AR934X_NFC_CTRL_ADDR_CYCLE0_S);
		ctrl_reg &= ~(AR934X_NFC_CTRL_ADDR_CYCLE1_M <<
			      AR934X_NFC_CTRL_ADDR_CYCLE1_S);
		ctrl_reg &= ~(AR934X_NFC_CTRL_SMALL_PAGE);
		ctrl_reg |= (nfc->addr_count0 + 1) <<
			    AR934X_NFC_CTRL_ADDR_CYCLE0_S;
	}

	cmd_reg = NAND_CMD_ERASE1 << AR934X_NFC_CMD_CMD0_S;
	cmd_reg |= command << AR934X_NFC_CMD_CMD1_S;
	cmd_reg |= AR934X_NFC_CMD_SEQ_ERASE;

	nfc_dbg(nfc, "erase page %d, a0:%08x a1:%08x cmd:%08x ctrl:%08x\n",
		page_addr, addr0, addr1, cmd_reg, ctrl_reg);

	ar934x_nfc_wr(nfc, AR934X_NFC_REG_INT_STATUS, 0);
	ar934x_nfc_wr(nfc, AR934X_NFC_REG_CTRL, ctrl_reg);
	ar934x_nfc_wr(nfc, AR934X_NFC_REG_ADDR0_0, addr0);
	ar934x_nfc_wr(nfc, AR934X_NFC_REG_ADDR0_1, addr1);

	ar934x_nfc_write_cmd_reg(nfc, cmd_reg);
	ar934x_nfc_wait_dev_ready(nfc);
}

static void
ar934x_nfc_send_write(struct ar934x_nfc *nfc, unsigned command, int column,
		     int page_addr, int len)
{
	u32 cmd_reg;

	nfc_dbg(nfc, "write, column=%d page=%d len=%d\n",
		column, page_addr, len);

	nfc_debug_data("[data] ", nfc->buf, len);

	cmd_reg = NAND_CMD_SEQIN << AR934X_NFC_CMD_CMD0_S;
	cmd_reg |= command << AR934X_NFC_CMD_CMD1_S;
	cmd_reg |= AR934X_NFC_CMD_SEQ_12;

	ar934x_nfc_do_rw_command(nfc, column, page_addr, len,
				 cmd_reg, nfc->ctrl_reg, true);
}

static void
ar934x_nfc_read_status(struct ar934x_nfc *nfc)
{
	u32 cmd_reg;
	u32 status;

	cmd_reg = NAND_CMD_STATUS << AR934X_NFC_CMD_CMD0_S;
	cmd_reg |= AR934X_NFC_CMD_SEQ_S;

	ar934x_nfc_wr(nfc, AR934X_NFC_REG_INT_STATUS, 0);
	ar934x_nfc_wr(nfc, AR934X_NFC_REG_CTRL, nfc->ctrl_reg);

	ar934x_nfc_write_cmd_reg(nfc, cmd_reg);
	ar934x_nfc_wait_dev_ready(nfc);

	status = ar934x_nfc_rr(nfc, AR934X_NFC_REG_READ_STATUS);

	nfc_dbg(nfc, "read status, cmd:%08x status:%02x\n",
		cmd_reg, (status & 0xff));

	nfc->buf[0 ^ 3] = status;
}

static void
ar934x_nfc_cmdfunc(struct mtd_info *mtd, unsigned int command, int column,
		   int page_addr)
{
	struct ar934x_nfc *nfc = mtd_to_ar934x_nfc(mtd);

	if (command != NAND_CMD_PAGEPROG)
		nfc->buf_index = 0;

	switch (command) {
	case NAND_CMD_RESET:
		ar934x_nfc_send_cmd(nfc, command);
		break;

	case NAND_CMD_READID:
		ar934x_nfc_send_readid(nfc, command);
		break;

	case NAND_CMD_READ0:
	case NAND_CMD_READ1:
		if (nfc->small_page) {
			ar934x_nfc_send_read(nfc, command, column, page_addr,
					     mtd->writesize + mtd->oobsize);
		} else {
			ar934x_nfc_send_read(nfc, command, 0, page_addr,
					     mtd->writesize + mtd->oobsize);
			nfc->buf_index = column;
			nfc->rndout_page_addr = page_addr;
			nfc->rndout_read_cmd = command;
		}
		break;

	case NAND_CMD_READOOB:
		if (nfc->small_page)
			ar934x_nfc_send_read(nfc, NAND_CMD_READOOB,
					     column, page_addr,
					     mtd->oobsize);
		else
			ar934x_nfc_send_read(nfc, NAND_CMD_READ0,
					     mtd->writesize, page_addr,
					     mtd->oobsize);
		break;

	case NAND_CMD_RNDOUT:
		if (WARN_ON(nfc->small_page))
			break;

		/* emulate subpage read */
		ar934x_nfc_send_read(nfc, nfc->rndout_read_cmd, 0,
				     nfc->rndout_page_addr,
				     mtd->writesize + mtd->oobsize);
		nfc->buf_index = column;
		break;

	case NAND_CMD_ERASE1:
		nfc->erase1_page_addr = page_addr;
		break;

	case NAND_CMD_ERASE2:
		ar934x_nfc_send_erase(nfc, command, -1, nfc->erase1_page_addr);
		break;

	case NAND_CMD_STATUS:
		ar934x_nfc_read_status(nfc);
		break;

	case NAND_CMD_SEQIN:
		if (nfc->small_page) {
			/* output read command */
			if (column >= mtd->writesize) {
				column -= mtd->writesize;
				nfc->seqin_read_cmd = NAND_CMD_READOOB;
			} else if (column < 256) {
				nfc->seqin_read_cmd = NAND_CMD_READ0;
			} else {
				column -= 256;
				nfc->seqin_read_cmd = NAND_CMD_READ1;
			}
		} else {
			nfc->seqin_read_cmd = NAND_CMD_READ0;
		}
		nfc->seqin_column = column;
		nfc->seqin_page_addr = page_addr;
		break;

	case NAND_CMD_PAGEPROG:
		if (nfc->small_page)
			ar934x_nfc_send_cmd(nfc, nfc->seqin_read_cmd);

		ar934x_nfc_send_write(nfc, command, nfc->seqin_column,
				      nfc->seqin_page_addr,
				      nfc->buf_index);
		break;

	default:
		dev_err(nfc->parent,
			"unsupported command: %x, column:%d page_addr=%d\n",
			command, column, page_addr);
		break;
	}
}

static int
ar934x_nfc_dev_ready(struct mtd_info *mtd)
{
	struct ar934x_nfc *nfc = mtd_to_ar934x_nfc(mtd);

	return __ar934x_nfc_dev_ready(nfc);
}

static void
ar934x_nfc_select_chip(struct mtd_info *mtd, int chip_no)
{
	struct ar934x_nfc *nfc = mtd_to_ar934x_nfc(mtd);

	if (nfc->select_chip)
		nfc->select_chip(chip_no);
}

static u8
ar934x_nfc_read_byte(struct mtd_info *mtd)
{
	struct ar934x_nfc *nfc = mtd_to_ar934x_nfc(mtd);
	unsigned int buf_index;
	u8 data;

	WARN_ON(nfc->buf_index >= nfc->buf_size);

	buf_index = nfc->buf_index ^ 3;
	data = nfc->buf[buf_index];
	nfc->buf_index++;

	return data;
}

static void
ar934x_nfc_write_buf(struct mtd_info *mtd, const u8 *buf, int len)
{
	struct ar934x_nfc *nfc = mtd_to_ar934x_nfc(mtd);
	int i;

	WARN_ON(nfc->buf_index + len > nfc->buf_size);

	for (i = 0; i < len; i++) {
		nfc->buf[nfc->buf_index ^ 3] = buf[i];
		nfc->buf_index++;
	}
}

static void
ar934x_nfc_read_buf(struct mtd_info *mtd, u8 *buf, int len)
{
	struct ar934x_nfc *nfc = mtd_to_ar934x_nfc(mtd);
	int buf_index;
	int i;

	WARN_ON(nfc->buf_index + len > nfc->buf_size);

	buf_index = nfc->buf_index;

	for (i = 0; i < len; i++) {
		buf[i] = nfc->buf[buf_index ^ 3];
		buf_index++;
	}

	nfc->buf_index = buf_index;
}

static void
ar934x_nfc_hw_init(struct ar934x_nfc *nfc)
{
	struct ar934x_nfc_platform_data *pdata;

	pdata = ar934x_nfc_get_platform_data(nfc);
	if (pdata->hw_reset) {
		pdata->hw_reset(true);
		pdata->hw_reset(false);
	}

	/*
	 * setup timings
	 * TODO: make it configurable via platform data
	 */
	ar934x_nfc_wr(nfc, AR934X_NFC_REG_TIME_SEQ,
		      AR934X_NFC_TIME_SEQ_DEFAULT);
	ar934x_nfc_wr(nfc, AR934X_NFC_REG_TIMINGS_ASYN,
		      AR934X_NFC_TIMINGS_ASYN_DEFAULT);
	ar934x_nfc_wr(nfc, AR934X_NFC_REG_TIMINGS_SYN,
		      AR934X_NFC_TIMINGS_SYN_DEFAULT);

	/* disable WP on all chips, and select chip 0 */
	ar934x_nfc_wr(nfc, AR934X_NFC_REG_MEM_CTRL, 0xff00);

	ar934x_nfc_wr(nfc, AR934X_NFC_REG_DMA_ADDR_OFFS, 0);

	/* initialize Control register */
	nfc->ctrl_reg = AR934X_NFC_CTRL_CUSTOM_SIZE_EN;
	ar934x_nfc_wr(nfc, AR934X_NFC_REG_CTRL, nfc->ctrl_reg);

	if (nfc->small_page) {
		/*  Setup generic sequence register for small page reads. */
		ar934x_nfc_wr(nfc, AR934X_NFC_REG_GEN_SEQ_CTRL,
			      AR934X_NFC_GENSEQ_SMALL_PAGE_READ);
	}
}

static void
ar934x_nfc_restart(struct ar934x_nfc *nfc)
{
	u32 ctrl_reg;

	if (nfc->select_chip)
		nfc->select_chip(-1);

	ctrl_reg = nfc->ctrl_reg;
	ar934x_nfc_hw_init(nfc);
	nfc->ctrl_reg = ctrl_reg;

	if (nfc->select_chip)
		nfc->select_chip(0);

	ar934x_nfc_send_cmd(nfc, NAND_CMD_RESET);
}

static irqreturn_t
ar934x_nfc_irq_handler(int irq, void *data)
{
	struct ar934x_nfc *nfc = data;
	u32 status;

	status = ar934x_nfc_rr(nfc, AR934X_NFC_REG_INT_STATUS);

	ar934x_nfc_wr(nfc, AR934X_NFC_REG_INT_STATUS, 0);
	/* flush write */
	ar934x_nfc_rr(nfc, AR934X_NFC_REG_INT_STATUS);

	status &= ar934x_nfc_rr(nfc, AR934X_NFC_REG_INT_MASK);
	if (status) {
		nfc_dbg(nfc, "got IRQ, status:%08x\n", status);

		nfc->irq_status = status;
		nfc->spurious_irq_expected = true;
		wake_up(&nfc->irq_waitq);
	} else {
		if (nfc->spurious_irq_expected) {
			nfc->spurious_irq_expected = false;
		} else {
			dev_warn(nfc->parent, "spurious interrupt\n");
		}
	}

	return IRQ_HANDLED;
}

static int __devinit
ar934x_nfc_init_tail(struct mtd_info *mtd)
{
	struct ar934x_nfc *nfc = mtd_to_ar934x_nfc(mtd);
	struct nand_chip *chip = &nfc->nand_chip;
	u32 ctrl;
	u32 t;
	int err;

	switch (mtd->oobsize) {
	case 16:
	case 64:
	case 128:
		ar934x_nfc_wr(nfc, AR934X_NFC_REG_SPARE_SIZE, mtd->oobsize);
		break;

	default:
		dev_err(nfc->parent, "unsupported OOB size: %d bytes\n",
			mtd->oobsize);
		return -ENXIO;
	}

	ctrl = AR934X_NFC_CTRL_CUSTOM_SIZE_EN;

	switch (mtd->erasesize / mtd->writesize) {
	case 32:
		t = AR934X_NFC_CTRL_BLOCK_SIZE_32;
		break;

	case 64:
		t = AR934X_NFC_CTRL_BLOCK_SIZE_64;
		break;

	case 128:
		t = AR934X_NFC_CTRL_BLOCK_SIZE_128;
		break;

	case 256:
		t = AR934X_NFC_CTRL_BLOCK_SIZE_256;
		break;

	default:
		dev_err(nfc->parent, "unsupported block size: %u\n",
			mtd->erasesize / mtd->writesize);
		return -ENXIO;
	}

	ctrl |= t << AR934X_NFC_CTRL_BLOCK_SIZE_S;

	switch (mtd->writesize) {
	case 256:
		nfc->small_page = 1;
		t = AR934X_NFC_CTRL_PAGE_SIZE_256;
		break;

	case 512:
		nfc->small_page = 1;
		t = AR934X_NFC_CTRL_PAGE_SIZE_512;
		break;

	case 1024:
		t = AR934X_NFC_CTRL_PAGE_SIZE_1024;
		break;

	case 2048:
		t = AR934X_NFC_CTRL_PAGE_SIZE_2048;
		break;

	case 4096:
		t = AR934X_NFC_CTRL_PAGE_SIZE_4096;
		break;

	case 8192:
		t = AR934X_NFC_CTRL_PAGE_SIZE_8192;
		break;

	case 16384:
		t = AR934X_NFC_CTRL_PAGE_SIZE_16384;
		break;

	default:
		dev_err(nfc->parent, "unsupported write size: %d bytes\n",
			mtd->writesize);
		return -ENXIO;
	}

	ctrl |= t << AR934X_NFC_CTRL_PAGE_SIZE_S;

	if (nfc->small_page) {
		ctrl |= AR934X_NFC_CTRL_SMALL_PAGE;

		if (chip->chipsize > (32 << 20)) {
			nfc->addr_count0 = 4;
			nfc->addr_count1 = 3;
		} else if (chip->chipsize > (2 << 16)) {
			nfc->addr_count0 = 3;
			nfc->addr_count1 = 2;
		} else {
			nfc->addr_count0 = 2;
			nfc->addr_count1 = 1;
		}
	} else {
		if (chip->chipsize > (128 << 20)) {
			nfc->addr_count0 = 5;
			nfc->addr_count1 = 3;
		} else if (chip->chipsize > (8 << 16)) {
			nfc->addr_count0 = 4;
			nfc->addr_count1 = 2;
		} else {
			nfc->addr_count0 = 3;
			nfc->addr_count1 = 1;
		}
	}

	ctrl |= nfc->addr_count0 << AR934X_NFC_CTRL_ADDR_CYCLE0_S;
	ctrl |= nfc->addr_count1 << AR934X_NFC_CTRL_ADDR_CYCLE1_S;

	nfc->ctrl_reg = ctrl;
	ar934x_nfc_wr(nfc, AR934X_NFC_REG_CTRL, nfc->ctrl_reg);

	ar934x_nfc_free_buf(nfc);
	err = ar934x_nfc_alloc_buf(nfc, mtd->writesize + mtd->oobsize);

	return err;
}

static int __devinit
ar934x_nfc_probe(struct platform_device *pdev)
{
	static const char *part_probes[] = { "cmdlinepart", NULL, };
	struct ar934x_nfc_platform_data *pdata;
	struct ar934x_nfc *nfc;
	struct resource *res;
	struct mtd_info *mtd;
	struct nand_chip *nand;
	struct mtd_part_parser_data ppdata;
	int ret;

	pdata = pdev->dev.platform_data;
	if (pdata == NULL) {
		dev_err(&pdev->dev, "no platform data defined\n");
		return -EINVAL;
	}

	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if (!res) {
		dev_err(&pdev->dev, "failed to get I/O memory\n");
		return -EINVAL;
	}

	nfc = kzalloc(sizeof(struct ar934x_nfc), GFP_KERNEL);
	if (!nfc) {
		dev_err(&pdev->dev, "failed to allocate driver data\n");
		return -ENOMEM;
	}

	nfc->base = ioremap(res->start, resource_size(res));
	if (nfc->base == NULL) {
		dev_err(&pdev->dev, "failed to remap I/O memory\n");
		ret = -ENXIO;
		goto err_free_nand;
	}

	nfc->irq = platform_get_irq(pdev, 0);
	if (nfc->irq < 0) {
		dev_err(&pdev->dev, "no IRQ resource specified\n");
		ret = -EINVAL;
		goto err_unmap;
	}

	init_waitqueue_head(&nfc->irq_waitq);
	ret = request_irq(nfc->irq, ar934x_nfc_irq_handler, IRQF_DISABLED,
			  dev_name(&pdev->dev), nfc);
	if (ret) {
		dev_err(&pdev->dev, "requast_irq failed, err:%d\n", ret);
		goto err_unmap;
	}

	nfc->parent = &pdev->dev;
	nfc->select_chip = pdata->select_chip;

	nand = &nfc->nand_chip;
	mtd = &nfc->mtd;

	mtd->priv = nand;
	mtd->owner = THIS_MODULE;
	if (pdata->name)
		mtd->name = pdata->name;
	else
		mtd->name = dev_name(&pdev->dev);

	nand->chip_delay = 25;
	nand->ecc.mode = NAND_ECC_SOFT;

	nand->dev_ready = ar934x_nfc_dev_ready;
	nand->cmdfunc = ar934x_nfc_cmdfunc;
	nand->read_byte = ar934x_nfc_read_byte;
	nand->write_buf = ar934x_nfc_write_buf;
	nand->read_buf = ar934x_nfc_read_buf;
	nand->select_chip = ar934x_nfc_select_chip;

	ret = ar934x_nfc_alloc_buf(nfc, AR934X_NFC_ID_BUF_SIZE);
	if (ret)
		goto err_free_irq;

	platform_set_drvdata(pdev, nfc);

	ar934x_nfc_hw_init(nfc);

	ret = nand_scan_ident(mtd, 1, NULL);
	if (ret) {
		dev_err(&pdev->dev, "nand_scan_ident failed, err:%d\n", ret);
		goto err_free_buf;
	}

	ret = ar934x_nfc_init_tail(mtd);
	if (ret) {
		dev_err(&pdev->dev, "init tail failed, err:%d\n", ret);
		goto err_free_buf;
	}

	if (pdata->scan_fixup) {
		ret = pdata->scan_fixup(mtd);
		if (ret)
			goto err_free_buf;
	}

	ret = nand_scan_tail(mtd);
	if (ret) {
		dev_err(&pdev->dev, "scan tail failed, err:%d\n", ret);
		goto err_free_buf;
	}

	memset(&ppdata, '\0', sizeof(ppdata));
	ret = mtd_device_parse_register(mtd, part_probes, &ppdata,
					pdata->parts, pdata->nr_parts);
	if (ret) {
		dev_err(&pdev->dev, "unable to register mtd, err:%d\n", ret);
		goto err_free_buf;
	}

	return 0;

err_free_buf:
	ar934x_nfc_free_buf(nfc);
err_free_irq:
	free_irq(nfc->irq, nfc);
err_unmap:
	iounmap(nfc->base);
err_free_nand:
	kfree(nfc);
	platform_set_drvdata(pdev, NULL);
	return ret;
}

static int __devexit
ar934x_nfc_remove(struct platform_device *pdev)
{
	struct ar934x_nfc *nfc;

	nfc = platform_get_drvdata(pdev);
	if (nfc) {
		nand_release(&nfc->mtd);
		ar934x_nfc_free_buf(nfc);
		free_irq(nfc->irq, nfc);
		iounmap(nfc->base);
		kfree(nfc);
	}

	return 0;
}

static struct platform_driver ar934x_nfc_driver = {
	.probe		= ar934x_nfc_probe,
	.remove		= __devexit_p(ar934x_nfc_remove),
	.driver = {
		.name	= AR934X_NFC_DRIVER_NAME,
		.owner	= THIS_MODULE,
	},
};

module_platform_driver(ar934x_nfc_driver);

MODULE_LICENSE("GPL v2");
MODULE_AUTHOR("Gabor Juhos <juhosg@openwrt.org>");
MODULE_DESCRIPTION("Atheros AR934x NAND Flash Controller driver");
MODULE_ALIAS("platform:" AR934X_NFC_DRIVER_NAME);

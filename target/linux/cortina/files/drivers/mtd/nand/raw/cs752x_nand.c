// SPDX-License-Identifier: GPL-2.0
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/module.h>
#include <linux/sched.h>
#include <linux/platform_device.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/rawnand.h>
#include <linux/mtd/partitions.h>
#include <linux/bitops.h>
#include <linux/err.h>
#include <linux/dma-mapping.h>
#include <linux/io.h>
#include <linux/delay.h>
#include <linux/jiffies.h>
#include <linux/of.h>

#if defined(CONFIG_CS752X_NAND_ECC_HW_BCH_8_512) || defined(CONFIG_CS752X_NAND_ECC_HW_BCH_12_512)
#define CS752X_ECC_BCH
#else
#define CS752X_ECC_HAMMING
#endif

#define CS75XX_CMD_MAX_NUM	3
#define CS75XX_OWN_BUF_SIZE	(SZ_4K + 512)

#define FLASH_ID		0x00
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
#define FLASH_NF_FIFO_CONTROL	0x90
#define FLASH_FLASH_ACCESS_START 0xa4
#define FLASH_NF_ECC_RESET	0xa8
#define FLASH_FLASH_INTERRUPT	0xac
#define FLASH_NF_BCH_STATUS	0xb4
#define FLASH_NF_BCH_ERROR_LOC01 0xb8
#define FLASH_NF_BCH_CONTROL	0xd0
#define FLASH_NF_BCH_OOB0	0xd4
#define FLASH_NF_BCH_GEN0_0	0xe8
#define FLASH_NF_BCH_GEN0_1	0xec
#define FLASH_NF_BCH_GEN1_0	0xfc

#define DMA_SSP_RXDMA_CONTROL	0x00
#define DMA_SSP_TXDMA_CONTROL	0x04
#define DMA_SSP_TXQ5_CONTROL	0x14
#define DMA_SSP_RXQ5_BASE_DEPTH	0x38
#define DMA_SSP_RXQ5_WPTR	0x44
#define DMA_SSP_RXQ5_RPTR	0x48
#define DMA_SSP_TXQ5_BASE_DEPTH	0x5c
#define DMA_SSP_TXQ5_WPTR	0x68
#define DMA_SSP_TXQ5_RPTR	0x6c
#define DMA_SSP_RXQ5_INTERRUPT	0xc8
#define DMA_SSP_TXQ5_INTERRUPT	0xe0

#define NF_ACCESS_EXTADDR(x)	((x) & 0xff)
#define NF_ACCESS_WIDTH8	(0x0 << 10)
#define NF_ACCESS_WIDTH16	(0x1 << 10)
#define NF_ACCESS_WIDTH32	(0x2 << 10)
#define NF_ACCESS_CEALT(x)	(((x) & 1) << 15)

#define NF_CNT_CMD(x)		((x) & 0x3)
#define NF_CNT_ADDR(x)		(((x) & 0x7) << 4)
#define NF_CNT_DATA(x)		(((x) & 0x3fff) << 8)
#define NF_CNT_OOB(x)		(((x) & 0x3ff) << 22)
#define NCNT_EMPTY_OOB		0x3ff
#define NCNT_EMPTY_DATA		0x3fff
#define NCNT_EMPTY_ADDR		0x7
#define NCNT_DATA_1		0x0
#define NCNT_ADDR_1		0x0
#define NCNT_ADDR_2		0x1
#define NCNT_ADDR_3		0x2
#define NCNT_ADDR_4		0x3
#define NCNT_ADDR_5		0x4
#define NCNT_CMD_1		0x0
#define NCNT_CMD_2		0x1
#define NCNT_CMD_3		0x2
#define NCNT_512P_DATA		0x1ff

#define FLASH_START_REGREQ	BIT(0)
#define FLASH_START_FIFOREQ	BIT(2)
#define FLASH_START_CMD(x)	(((x) & 0x3) << 12)
#define FLASH_RD		0x2
#define FLASH_WT		0x3

#define FIFO_CMD(x)		((x) & 0x3)

#define ECC_RESET_ECC_CLR	BIT(0)
#define ECC_RESET_FIFO_CLR	BIT(1)
#define ECC_RESET_NF_RESET	BIT(2)

#define FLASH_INT_REGIRQ	BIT(0)

#define ECC_CTL_GEN_MODE_512	BIT(1)
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

#define BCH_CTL_COMPARE		BIT(0)
#define BCH_CTL_DECODE		BIT(1)
#define BCH_CTL_CODESEL(x)	(((x) & 0xf) << 4)
#define BCH_CTL_EN		BIT(8)
#define BCH_CTL_ERRCAP_12	BIT(9)

#define BCH_STS_DECSTATUS(v)	((v) & 0x3)
#define BCH_STS_ERRNUM(v)	(((v) >> 4) & 0xf)
#define BCH_STS_DECDONE		BIT(30)
#define BCH_STS_GENDONE		BIT(31)
#define BCH_NO_ERR		0x1
#define BCH_CORRECTABLE_ERR	0x2
#define BCH_UNCORRECTABLE	0x3

#define FLASH_STATUS_MASK_nState	(0x0f << 8)

#define DMA_CTRL_ENABLE		BIT(0)
#define DMA_CTRL_CHECK_OWN	BIT(1)
#define DMA_TXQ5_EN		BIT(0)
#define DMA_Q_IDX(v)		((v) & 0x1fff)
#define DMA_RXQ5_EOF		BIT(0)
#define DMA_TXQ5_EOF		BIT(0)

#define FDMA_DEPTH		3
#define FDMA_DESC_NUM		(1 << FDMA_DEPTH)

#define OWN_DMA			0
#define OWN_SW			1

#define DESC_BUF_SIZE(x)	((x) & 0xffff)
#define DESC_SGM		(0x15 << 22)
#define DESC_OWN(x)		(((x) & 1) << 31)

struct cs752x_dma_desc {
	u32 word0;
	u32 buf_adr;
	u32 word2;
	u32 word3;
};

struct cs752x_nand_host {
	struct nand_controller	controller;
	struct nand_chip	chip;
	struct device		*dev;

	void __iomem		*iobase_fl;
	void __iomem		*iobase_dma;
	u32			dma_phy_base;

	struct cs752x_dma_desc	*tx_desc;
	struct cs752x_dma_desc	*rx_desc;
	dma_addr_t		tx_desc_dma;
	dma_addr_t		rx_desc_dma;

	unsigned int		cmd_array[CS75XX_CMD_MAX_NUM];
	unsigned int		cmd_cnt;
	int			cmd_page;

	unsigned int		buf_offs;
	unsigned int		buf_data_len;
	unsigned char		*buf_top;
	unsigned char		own_buf[CS75XX_OWN_BUF_SIZE];

	u8			*ecc_calc;
	u8			*ecc_code;
	u8			*page_buf;
	bool			flag_status_req;
	u32			chip_en;
	int			cur_page;
	int			cur_col;
};

#define BCH_ERASE_TAG_LEN	1
#define BCH_ERASE_TAG_SECTION	0xFF

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

static inline u32 dma_readl(struct cs752x_nand_host *h, u32 reg)
{
	return readl(h->iobase_dma + reg);
}

static inline void dma_writel(struct cs752x_nand_host *h, u32 reg, u32 val)
{
	writel(val, h->iobase_dma + reg);
}

#ifdef CS752X_ECC_BCH
static int cs752x_ooblayout_ecc_bch16(struct mtd_info *mtd, int section,
				      struct mtd_oob_region *oobregion)
{
	struct nand_chip *chip = mtd_to_nand(mtd);

	if (section > 1)
		return -ERANGE;
	if (section == 0) {
		oobregion->offset = 0;
		oobregion->length = 4;
	} else {
		oobregion->offset = 6;
		oobregion->length = chip->ecc.total - 4;
	}
	return 0;
}

static int cs752x_ooblayout_free_bch16(struct mtd_info *mtd, int section,
				       struct mtd_oob_region *oobregion)
{
	if (section != BCH_ERASE_TAG_SECTION)
		return -ERANGE;
	oobregion->offset = 15;
	oobregion->length = 1 - BCH_ERASE_TAG_LEN;
	return 0;
}

static const struct mtd_ooblayout_ops cs752x_ooblayout_ops_bch16 = {
	.ecc = cs752x_ooblayout_ecc_bch16,
	.free = cs752x_ooblayout_free_bch16,
};

static int cs752x_ooblayout_ecc_bch_lp(struct mtd_info *mtd, int section,
				       struct mtd_oob_region *oobregion)
{
	struct nand_chip *chip = mtd_to_nand(mtd);

	if (section)
		return -ERANGE;
	oobregion->length = chip->ecc.total;
	oobregion->offset = mtd->oobsize - chip->ecc.total;
	return 0;
}

static int cs752x_ooblayout_free_bch_lp(struct mtd_info *mtd, int section,
					struct mtd_oob_region *oobregion)
{
	struct nand_chip *chip = mtd_to_nand(mtd);

	if (section != 0 && section != BCH_ERASE_TAG_SECTION)
		return -ERANGE;
	oobregion->length = mtd->oobsize - chip->ecc.total - 2 - BCH_ERASE_TAG_LEN;
	oobregion->offset = 2;
	return 0;
}

static const struct mtd_ooblayout_ops cs752x_ooblayout_ops_bch_lp = {
	.ecc = cs752x_ooblayout_ecc_bch_lp,
	.free = cs752x_ooblayout_free_bch_lp,
};
#endif

static int check_flash_ctrl_status(struct cs752x_nand_host *h)
{
	unsigned long timeo = jiffies + HZ;
	u32 val;

	do {
		val = fl_readl(h, FLASH_STATUS) & FLASH_STATUS_MASK_nState;
		if (val == 0)
			return 0;
		cpu_relax();
	} while (time_before(jiffies, timeo));

	dev_err(h->dev, "FLASH_STATUS busy: 0x%x\n", val);
	return -ETIMEDOUT;
}

static inline u32 mk_nf_command(u8 cmd0, u8 cmd1, u8 cmd2)
{
	return ((u32)cmd2 << 16) | ((u32)cmd1 << 8) | cmd0;
}

static u32 mk_nf_addr(u64 chipsize, u32 page, u32 *paddr1, u32 *paddr2, u32 oob_offs)
{
	if (chipsize < SZ_32M) {
		*paddr1 = (page & 0x00ffffff) << 8;
		*paddr2 = (page & 0xff000000) >> 24;
		return NCNT_ADDR_3;
	}
	*paddr1 = ((page & 0xffff) << 16) + (oob_offs & 0xffff);
	*paddr2 = (page & 0xffff0000) >> 16;
	return chipsize <= SZ_128M ? NCNT_ADDR_4 : NCNT_ADDR_5;
}

static int wait_flash_start_clear(struct cs752x_nand_host *h, u32 bit)
{
	unsigned long timeo = jiffies + 2 * HZ;

	while (fl_readl(h, FLASH_FLASH_ACCESS_START) & bit) {
		if (time_after(jiffies, timeo)) {
			dev_err(h->dev, "flash access timeout (bit 0x%x)\n", bit);
			return -ETIMEDOUT;
		}
		udelay(1);
		cond_resched();
	}
	return 0;
}

static int wait_dma_eof(struct cs752x_nand_host *h)
{
	unsigned long timeo = jiffies + 2 * HZ;

	while (!(dma_readl(h, DMA_SSP_RXQ5_INTERRUPT) & DMA_RXQ5_EOF)) {
		if (time_after(jiffies, timeo)) {
			dev_err(h->dev, "rxq5 eof timeout\n");
			return -ETIMEDOUT;
		}
		udelay(1);
		cond_resched();
	}
	timeo = jiffies + 2 * HZ;
	while (!(dma_readl(h, DMA_SSP_TXQ5_INTERRUPT) & DMA_TXQ5_EOF)) {
		if (time_after(jiffies, timeo)) {
			dev_err(h->dev, "txq5 eof timeout\n");
			return -ETIMEDOUT;
		}
		udelay(1);
		cond_resched();
	}
	return 0;
}

static void clear_dma_eof(struct cs752x_nand_host *h)
{
	dma_writel(h, DMA_SSP_TXQ5_INTERRUPT, dma_readl(h, DMA_SSP_TXQ5_INTERRUPT));
	dma_writel(h, DMA_SSP_RXQ5_INTERRUPT, dma_readl(h, DMA_SSP_RXQ5_INTERRUPT));
}

static int wait_reg(struct cs752x_nand_host *h, u32 reg, u32 bit, const char *what)
{
	unsigned long timeo = jiffies + 2 * HZ;

	while (!(fl_readl(h, reg) & bit)) {
		if (time_after(jiffies, timeo)) {
			dev_err(h->dev, "%s timeout\n", what);
			return -ETIMEDOUT;
		}
		udelay(1);
		cond_resched();
	}
	return 0;
}

static void reset_ecc_bch_registers(struct cs752x_nand_host *h)
{
	fl_writel(h, FLASH_NF_ECC_RESET, ECC_RESET_ECC_CLR | ECC_RESET_FIFO_CLR);
	fl_writel(h, FLASH_FLASH_INTERRUPT, FLASH_INT_REGIRQ);
	fl_writel(h, FLASH_NF_ECC_RESET, ECC_RESET_ECC_CLR);
	fl_writel(h, FLASH_NF_BCH_CONTROL, 0);
	fl_writel(h, FLASH_NF_ECC_CONTROL, 0);
}

static void configure_hwecc_reg(struct cs752x_nand_host *h, bool is_write)
{
#if defined(CONFIG_CS752X_NAND_ECC_HW_BCH_8_512)
	fl_writel(h, FLASH_NF_BCH_CONTROL,
		  BCH_CTL_EN | (is_write ? 0 : BCH_CTL_DECODE));
#elif defined(CONFIG_CS752X_NAND_ECC_HW_BCH_12_512)
	fl_writel(h, FLASH_NF_BCH_CONTROL,
		  BCH_CTL_EN | BCH_CTL_ERRCAP_12 | (is_write ? 0 : BCH_CTL_DECODE));
#elif defined(CONFIG_CS752X_NAND_ECC_HW_HAMMING_512)
	fl_writel(h, FLASH_NF_ECC_CONTROL, ECC_CTL_EN | ECC_CTL_GEN_MODE_512);
#else
	fl_writel(h, FLASH_NF_ECC_CONTROL, ECC_CTL_EN);
#endif
}

static int do_pio_write_buf(struct cs752x_nand_host *h, const void *buf, int bytes)
{
	const u32 *ulp = buf;
	int n = bytes / sizeof(u32);
	int i, ret;

	for (i = 0; i < n; i++) {
		fl_writel(h, FLASH_NF_ACCESS,
			  NF_ACCESS_CEALT(h->chip_en) | NF_ACCESS_WIDTH32);
		fl_writel(h, FLASH_NF_DATA, *ulp++);
		fl_writel(h, FLASH_FLASH_ACCESS_START,
			  FLASH_START_REGREQ | FLASH_START_CMD(FLASH_WT));
		ret = wait_flash_start_clear(h, FLASH_START_REGREQ);
		if (ret)
			return ret;
	}
	return 0;
}

static int do_pio_read_buf(struct cs752x_nand_host *h, void *buf, int bytes)
{
	u32 *ulp = buf;
	int n = bytes / sizeof(u32);
	int i, ret;

	for (i = 0; i < n; i++) {
		fl_writel(h, FLASH_NF_ACCESS,
			  NF_ACCESS_CEALT(h->chip_en) | NF_ACCESS_WIDTH32);
		fl_writel(h, FLASH_FLASH_ACCESS_START,
			  FLASH_START_REGREQ | FLASH_START_CMD(FLASH_RD));
		ret = wait_flash_start_clear(h, FLASH_START_REGREQ);
		if (ret)
			return ret;
		*ulp++ = fl_readl(h, FLASH_NF_DATA);
	}
	return 0;
}

static int cs752x_nand_erase_block(struct cs752x_nand_host *h, int page)
{
	struct nand_chip *chip = &h->chip;
	struct mtd_info *mtd = nand_to_mtd(chip);
	u64 chipsize = nanddev_target_size(&chip->base);
	u32 cnt;

	check_flash_ctrl_status(h);

	fl_writel(h, FLASH_NF_ECC_CONTROL, 0);

	cnt = NF_CNT_OOB(NCNT_EMPTY_OOB) | NF_CNT_DATA(NCNT_EMPTY_DATA) |
	      NF_CNT_CMD(NCNT_CMD_2);
	if (chipsize > (u64)0x10000 * mtd->writesize)
		cnt |= NF_CNT_ADDR(NCNT_ADDR_3);
	else
		cnt |= NF_CNT_ADDR(NCNT_ADDR_2);

	fl_writel(h, FLASH_NF_COUNT, cnt);
	fl_writel(h, FLASH_NF_COMMAND, mk_nf_command(NAND_CMD_ERASE1, NAND_CMD_ERASE2, 0));
	fl_writel(h, FLASH_NF_ADDRESS_1, page);
	fl_writel(h, FLASH_NF_ADDRESS_2, 0);
	fl_writel(h, FLASH_NF_ACCESS, NF_ACCESS_CEALT(h->chip_en) | NF_ACCESS_WIDTH8);
	fl_writel(h, FLASH_FLASH_ACCESS_START,
		  FLASH_START_REGREQ | FLASH_START_CMD(FLASH_RD));

	return wait_flash_start_clear(h, FLASH_START_REGREQ);
}

static int cs752x_nand_write_oob_std(struct nand_chip *chip, int page)
{
	struct cs752x_nand_host *h = to_host(chip);
	struct mtd_info *mtd = nand_to_mtd(chip);
	u64 chipsize = nanddev_target_size(&chip->base);
	u32 cmd, addr1, addr2, cnt;
	int status;

	check_flash_ctrl_status(h);

	fl_writel(h, FLASH_NF_ECC_CONTROL, 0);

	cnt = NF_CNT_OOB(mtd->oobsize - 1) | NF_CNT_DATA(NCNT_EMPTY_DATA);
	if (chipsize < SZ_32M && mtd->writesize <= NCNT_512P_DATA) {
		cnt |= NF_CNT_CMD(NCNT_CMD_3);
		cmd = mk_nf_command(NAND_CMD_READOOB, NAND_CMD_SEQIN, NAND_CMD_PAGEPROG);
	} else {
		cnt |= NF_CNT_CMD(NCNT_CMD_2);
		cmd = mk_nf_command(NAND_CMD_SEQIN, NAND_CMD_PAGEPROG, 0);
	}
	cnt |= NF_CNT_ADDR(mk_nf_addr(chipsize, page, &addr1, &addr2, mtd->writesize));

	fl_writel(h, FLASH_NF_COUNT, cnt);
	fl_writel(h, FLASH_NF_COMMAND, cmd);
	fl_writel(h, FLASH_NF_ADDRESS_1, addr1);
	fl_writel(h, FLASH_NF_ADDRESS_2, addr2);

	status = do_pio_write_buf(h, chip->oob_poi, mtd->oobsize);
	if (status)
		return status;

	status = chip->legacy.waitfunc(chip);
	if (status < 0)
		return status;
	return status & NAND_STATUS_FAIL ? -EIO : 0;
}

static int cs752x_do_read_oob(struct cs752x_nand_host *h, int page)
{
	struct nand_chip *chip = &h->chip;
	struct mtd_info *mtd = nand_to_mtd(chip);
	u64 chipsize = nanddev_target_size(&chip->base);
	u32 cmd, addr1, addr2, cnt;

	check_flash_ctrl_status(h);

	fl_writel(h, FLASH_NF_ECC_CONTROL, 0);

	cnt = NF_CNT_OOB(mtd->oobsize - 1) | NF_CNT_DATA(NCNT_EMPTY_DATA);
	if (chipsize < SZ_32M) {
		cnt |= NF_CNT_CMD(NCNT_CMD_1);
		cmd = mk_nf_command(mtd->writesize > NCNT_512P_DATA ?
				    NAND_CMD_READ0 : NAND_CMD_READOOB, 0, 0);
	} else if (chipsize <= SZ_128M && mtd->writesize <= NCNT_512P_DATA) {
		cnt |= NF_CNT_CMD(NCNT_CMD_1);
		cmd = mk_nf_command(NAND_CMD_READ0, 0, 0);
	} else {
		cnt |= NF_CNT_CMD(NCNT_CMD_2);
		cmd = mk_nf_command(NAND_CMD_READ0, NAND_CMD_READSTART, 0);
	}
	cnt |= NF_CNT_ADDR(mk_nf_addr(chipsize, page, &addr1, &addr2, mtd->writesize));

	fl_writel(h, FLASH_NF_COUNT, cnt);
	fl_writel(h, FLASH_NF_COMMAND, cmd);
	fl_writel(h, FLASH_NF_ADDRESS_1, addr1);
	fl_writel(h, FLASH_NF_ADDRESS_2, addr2);

	return do_pio_read_buf(h, h->own_buf, mtd->oobsize);
}

static void cs752x_setup_page_regs(struct cs752x_nand_host *h, int page,
				   bool write, bool oob_required)
{
	struct nand_chip *chip = &h->chip;
	struct mtd_info *mtd = nand_to_mtd(chip);
	u64 chipsize = nanddev_target_size(&chip->base);
	u32 cmd, addr1, addr2, cnt;

	cnt = NF_CNT_DATA(mtd->writesize - 1);
	cnt |= NF_CNT_OOB(oob_required ? mtd->oobsize - 1 : NCNT_EMPTY_OOB);

	if (write) {
		cnt |= NF_CNT_CMD(NCNT_CMD_2);
		cmd = mk_nf_command(NAND_CMD_SEQIN, NAND_CMD_PAGEPROG, 0);
	} else if (chipsize < SZ_32M ||
		   (chipsize <= SZ_128M && mtd->writesize <= NCNT_512P_DATA)) {
		cnt |= NF_CNT_CMD(NCNT_CMD_1);
		cmd = mk_nf_command(NAND_CMD_READ0, 0, 0);
	} else {
		cnt |= NF_CNT_CMD(NCNT_CMD_2);
		cmd = mk_nf_command(NAND_CMD_READ0, NAND_CMD_READSTART, 0);
	}
	cnt |= NF_CNT_ADDR(mk_nf_addr(chipsize, page, &addr1, &addr2, 0));

	fl_writel(h, FLASH_NF_COUNT, cnt);
	fl_writel(h, FLASH_NF_COMMAND, cmd);
	fl_writel(h, FLASH_NF_ADDRESS_1, addr1);
	fl_writel(h, FLASH_NF_ADDRESS_2, addr2);
	fl_writel(h, FLASH_NF_ACCESS,
		  NF_ACCESS_CEALT(h->chip_en) | NF_ACCESS_WIDTH8 |
		  NF_ACCESS_EXTADDR(((u32)page << chip->page_shift) / SZ_128M));
}

static int cs752x_dma_xfer(struct cs752x_nand_host *h, dma_addr_t src,
			   dma_addr_t dst, u32 len, bool kick_fifo, bool write)
{
	u32 txw = DMA_Q_IDX(dma_readl(h, DMA_SSP_TXQ5_WPTR));
	u32 rxr = DMA_Q_IDX(dma_readl(h, DMA_SSP_RXQ5_RPTR));
	int ret;

	h->tx_desc[txw].word0 = DESC_OWN(OWN_DMA) | DESC_SGM | DESC_BUF_SIZE(len);
	h->tx_desc[txw].buf_adr = src;
	h->rx_desc[rxr].word0 = DESC_OWN(OWN_DMA) | DESC_BUF_SIZE(len);
	h->rx_desc[rxr].buf_adr = dst;
	wmb();

	txw = (txw + 1) % FDMA_DESC_NUM;
	if (write)
		dma_writel(h, DMA_SSP_TXQ5_WPTR, txw);

	if (kick_fifo) {
		fl_writel(h, FLASH_NF_FIFO_CONTROL, FIFO_CMD(write ? FLASH_WT : FLASH_RD));
		fl_writel(h, FLASH_FLASH_ACCESS_START, FLASH_START_FIFOREQ);
	}

	if (!write)
		dma_writel(h, DMA_SSP_TXQ5_WPTR, txw);
	dma_writel(h, DMA_SSP_TXQ5_CONTROL, DMA_TXQ5_EN);

	ret = wait_dma_eof(h);

	rxr = (rxr + 1) % FDMA_DESC_NUM;
	dma_writel(h, DMA_SSP_RXQ5_RPTR, rxr);
	clear_dma_eof(h);

	return ret;
}

static int cs752x_dma_prepare(struct cs752x_nand_host *h)
{
	dma_writel(h, DMA_SSP_TXQ5_CONTROL, 0);
	clear_dma_eof(h);
	return 0;
}

static int cs752x_nand_read_page_dma(struct cs752x_nand_host *h, u8 *buf,
				     bool oob_required, int page)
{
	struct nand_chip *chip = &h->chip;
	struct mtd_info *mtd = nand_to_mtd(chip);
	dma_addr_t src = h->dma_phy_base + (((u32)page << chip->page_shift) % SZ_128M);
	dma_addr_t dst;
	int ret;

	cs752x_dma_prepare(h);
	cs752x_setup_page_regs(h, page, false, oob_required);

	dst = dma_map_single(h->dev, h->page_buf, mtd->writesize, DMA_FROM_DEVICE);
	if (dma_mapping_error(h->dev, dst))
		return -ENOMEM;
	ret = cs752x_dma_xfer(h, src, dst, mtd->writesize, true, false);
	dma_unmap_single(h->dev, dst, mtd->writesize, DMA_FROM_DEVICE);
	if (ret)
		return ret;

	if (oob_required) {
		dst = dma_map_single(h->dev, chip->oob_poi, mtd->oobsize, DMA_FROM_DEVICE);
		if (dma_mapping_error(h->dev, dst))
			return -ENOMEM;
		ret = cs752x_dma_xfer(h, src + mtd->writesize, dst, mtd->oobsize,
				      false, false);
		dma_unmap_single(h->dev, dst, mtd->oobsize, DMA_FROM_DEVICE);
		if (ret)
			return ret;
	}

	ret = wait_flash_start_clear(h, FLASH_START_FIFOREQ);
	clear_dma_eof(h);

	memcpy(buf, h->page_buf, mtd->writesize);

	return ret;
}

static int cs752x_nand_write_page_dma(struct cs752x_nand_host *h, const u8 *buf,
				      bool oob_required, int page,
				      int (*fill_oob)(struct cs752x_nand_host *))
{
	struct nand_chip *chip = &h->chip;
	struct mtd_info *mtd = nand_to_mtd(chip);
	dma_addr_t dst = h->dma_phy_base + (((u32)page << chip->page_shift) % SZ_128M);
	dma_addr_t src;
	int ret;

	cs752x_dma_prepare(h);
	cs752x_setup_page_regs(h, page, true, oob_required);

	memcpy(h->page_buf, buf, mtd->writesize);

	src = dma_map_single(h->dev, h->page_buf, mtd->writesize, DMA_TO_DEVICE);
	if (dma_mapping_error(h->dev, src))
		return -ENOMEM;
	ret = cs752x_dma_xfer(h, src, dst, mtd->writesize, true, true);
	dma_unmap_single(h->dev, src, mtd->writesize, DMA_TO_DEVICE);
	if (ret)
		return ret;

	if (oob_required) {
		if (fill_oob) {
			ret = fill_oob(h);
			if (ret)
				return ret;
		}
		src = dma_map_single(h->dev, chip->oob_poi, mtd->oobsize, DMA_TO_DEVICE);
		if (dma_mapping_error(h->dev, src))
			return -ENOMEM;
		ret = cs752x_dma_xfer(h, src, dst + mtd->writesize, mtd->oobsize,
				      false, true);
		dma_unmap_single(h->dev, src, mtd->oobsize, DMA_TO_DEVICE);
		if (ret)
			return ret;
	}

	ret = wait_flash_start_clear(h, FLASH_START_FIFOREQ);
	clear_dma_eof(h);
	return ret;
}

#ifdef CS752X_ECC_BCH
static int fill_bch_oob_data(struct cs752x_nand_host *h)
{
	struct nand_chip *chip = &h->chip;
	struct mtd_info *mtd = nand_to_mtd(chip);
	u8 *ecc_calc = h->ecc_calc;
	const u32 reg_offset = FLASH_NF_BCH_GEN0_1 - FLASH_NF_BCH_GEN0_0;
	const u32 group_offset = FLASH_NF_BCH_GEN1_0 - FLASH_NF_BCH_GEN0_0;
	struct mtd_oob_region region;
	u32 addr = FLASH_NF_BCH_GEN0_0;
	int eccsteps, i, j, k = 0, tail_offs, ret;

	ret = wait_reg(h, FLASH_NF_BCH_STATUS, BCH_STS_GENDONE, "bch gen");
	if (ret)
		return ret;
	fl_writel(h, FLASH_NF_BCH_CONTROL,
		  fl_readl(h, FLASH_NF_BCH_CONTROL) & ~BCH_CTL_EN);

	for (eccsteps = chip->ecc.steps; eccsteps; --eccsteps, addr += group_offset) {
		tail_offs = k + chip->ecc.bytes;
		for (i = 0; k < tail_offs; ++i) {
			u32 v = fl_readl(h, addr + reg_offset * i);

			for (j = 0; j < 4 && k < tail_offs; ++j, ++k)
				ecc_calc[k] = (v >> (j * 8)) & 0xff;
		}
	}
	mtd_ooblayout_set_eccbytes(mtd, ecc_calc, chip->oob_poi, 0, chip->ecc.total);

	mtd_ooblayout_free(mtd, BCH_ERASE_TAG_SECTION, &region);
	chip->oob_poi[region.offset + region.length] = 0;
	return 0;
}

static int bch_correct(struct cs752x_nand_host *h, u8 *buf)
{
	struct nand_chip *chip = &h->chip;
	struct mtd_info *mtd = nand_to_mtd(chip);
	u8 *ecc_code = h->ecc_code;
	int eccsteps = chip->ecc.steps, eccbytes = chip->ecc.bytes;
	int eccsize = chip->ecc.size;
	int i, j, k, m = 0, tail_offs, max_bitflips = 0;
	u8 *p = buf;

	mtd_ooblayout_get_eccbytes(mtd, ecc_code, chip->oob_poi, 0, chip->ecc.total);

	for (i = 0; eccsteps; eccsteps--, i += eccbytes, p += eccsize) {
		u32 sts, ctl;

		tail_offs = m + eccbytes;
		for (j = 0; j < eccbytes; j += 4) {
			u32 v = 0;

			for (k = 0; k < 4 && m < tail_offs; ++k, ++m)
				v |= ecc_code[m] << (8 * k);
			fl_writel(h, FLASH_NF_BCH_OOB0 + j, v);
		}

		ctl = fl_readl(h, FLASH_NF_BCH_CONTROL) & ~BCH_CTL_CODESEL(0xf);
		ctl |= BCH_CTL_CODESEL(i / eccbytes) | BCH_CTL_COMPARE;
		fl_writel(h, FLASH_NF_BCH_CONTROL, ctl);

		if (wait_reg(h, FLASH_NF_BCH_STATUS, BCH_STS_DECDONE, "bch dec"))
			return -ETIMEDOUT;
		sts = fl_readl(h, FLASH_NF_BCH_STATUS);

		switch (BCH_STS_DECSTATUS(sts)) {
		case BCH_CORRECTABLE_ERR: {
			int nerr = BCH_STS_ERRNUM(sts);

			for (j = 0; j < (nerr + 1) / 2; j++) {
				u32 loc = fl_readl(h, FLASH_NF_BCH_ERROR_LOC01 + j * 4);
				u32 l0 = loc & 0x1fff, l1 = (loc >> 16) & 0x1fff;

				if ((j + 1) * 2 <= nerr && (l1 >> 3) < eccsize)
					p[l1 >> 3] ^= BIT(l1 & 7);
				if ((l0 >> 3) < eccsize)
					p[l0 >> 3] ^= BIT(l0 & 7);
			}
			mtd->ecc_stats.corrected += nerr;
			max_bitflips = max(max_bitflips, nerr);
			break;
		}
		case BCH_UNCORRECTABLE:
			mtd->ecc_stats.failed++;
			break;
		}

		fl_writel(h, FLASH_NF_BCH_CONTROL,
			  fl_readl(h, FLASH_NF_BCH_CONTROL) & ~BCH_CTL_COMPARE);
	}
	return max_bitflips;
}
#else
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

	mtd_ooblayout_get_eccbytes(mtd, ecc_code, chip->oob_poi, 0, chip->ecc.total);

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
#endif

static int cs752x_nand_read_page_raw(struct nand_chip *chip, u8 *buf,
				     int oob_required, int page)
{
	struct cs752x_nand_host *h = to_host(chip);

	nand_read_page_op(chip, page, 0, NULL, 0);
	check_flash_ctrl_status(h);
	reset_ecc_bch_registers(h);
	return cs752x_nand_read_page_dma(h, buf, oob_required, page);
}

static int cs752x_nand_write_page_raw(struct nand_chip *chip, const u8 *buf,
				      int oob_required, int page)
{
	struct cs752x_nand_host *h = to_host(chip);
	int ret;

	nand_prog_page_begin_op(chip, page, 0, NULL, 0);
	check_flash_ctrl_status(h);
	reset_ecc_bch_registers(h);
	ret = cs752x_nand_write_page_dma(h, buf, oob_required, page, NULL);
	if (ret)
		return ret;
	return nand_prog_page_end_op(chip);
}

static int cs752x_nand_read_page_hwecc(struct nand_chip *chip, u8 *buf,
				       int oob_required, int page)
{
	struct cs752x_nand_host *h = to_host(chip);
	int ret;

	nand_read_page_op(chip, page, 0, NULL, 0);
	check_flash_ctrl_status(h);
	reset_ecc_bch_registers(h);
	configure_hwecc_reg(h, false);

	ret = cs752x_nand_read_page_dma(h, buf, true, page);
	if (ret)
		return ret;

#ifdef CS752X_ECC_BCH
	{
		struct mtd_oob_region region;

		if (wait_reg(h, FLASH_NF_BCH_STATUS, BCH_STS_GENDONE, "bch gen"))
			return -ETIMEDOUT;
		mtd_ooblayout_free(nand_to_mtd(chip), BCH_ERASE_TAG_SECTION, &region);
		if (chip->oob_poi[region.offset + region.length] == 0xFF)
			ret = 0;
		else
			ret = bch_correct(h, buf);
	}
	fl_writel(h, FLASH_NF_BCH_CONTROL, 0);
#else
	ret = hamming_correct(h, buf, page);
#endif
	return ret;
}

static int cs752x_nand_write_page_hwecc(struct nand_chip *chip, const u8 *buf,
					int oob_required, int page)
{
	struct cs752x_nand_host *h = to_host(chip);
	int ret;

	nand_prog_page_begin_op(chip, page, 0, NULL, 0);
	check_flash_ctrl_status(h);
	reset_ecc_bch_registers(h);
	configure_hwecc_reg(h, true);

#ifdef CS752X_ECC_BCH
	ret = cs752x_nand_write_page_dma(h, buf, true, page, fill_bch_oob_data);
#else
	ret = cs752x_nand_write_page_dma(h, buf, true, page, fill_hamming_oob_data);
#endif
	if (ret)
		return ret;
	return nand_prog_page_end_op(chip);
}

static void cs752x_nand_read_buf(struct nand_chip *chip, u8 *buf, int len)
{
	struct cs752x_nand_host *h = to_host(chip);

	if (h->buf_data_len > h->buf_offs) {
		int len2 = min_t(int, h->buf_data_len - h->buf_offs, len);

		memcpy(buf, h->buf_top + h->buf_offs, len2);
		if (len2 < len)
			memset(buf + len2, 0xff, len - len2);
		h->buf_offs += len2;
		return;
	}
	memset(buf, 0xff, len);
}

static void cs752x_nand_write_buf(struct nand_chip *chip, const u8 *buf, int len)
{
	dev_warn_once(to_host(chip)->dev, "write_buf is not supported\n");
}

static u8 cs752x_nand_read_byte(struct nand_chip *chip)
{
	struct cs752x_nand_host *h = to_host(chip);

	if (h->flag_status_req)
		return fl_readl(h, FLASH_NF_DATA) & 0xff;

	if (h->buf_offs < h->buf_data_len)
		return h->buf_top[h->buf_offs++];

	return 0xff;
}

static void clear_command_cache(struct cs752x_nand_host *h)
{
	memset(h->cmd_array, 0, sizeof(h->cmd_array));
	h->cmd_cnt = 0;
	h->cmd_page = -1;
}

static void add_to_command_cache(struct cs752x_nand_host *h, unsigned int cmd)
{
	if (h->cmd_cnt < CS75XX_CMD_MAX_NUM)
		h->cmd_array[h->cmd_cnt++] = cmd;
}

static void do_readid_param(struct cs752x_nand_host *h, int col, u8 cmd, int bytes)
{
	int i;

	check_flash_ctrl_status(h);

	fl_writel(h, FLASH_NF_ECC_CONTROL, 0);
	fl_writel(h, FLASH_NF_COUNT,
		  NF_CNT_OOB(NCNT_EMPTY_OOB) | NF_CNT_DATA(bytes - 1) |
		  NF_CNT_ADDR(NCNT_ADDR_1) | NF_CNT_CMD(NCNT_CMD_1));
	fl_writel(h, FLASH_NF_COMMAND, mk_nf_command(cmd, 0, 0));
	fl_writel(h, FLASH_NF_ADDRESS_1, col);
	fl_writel(h, FLASH_NF_ADDRESS_2, 0);
	fl_writel(h, FLASH_NF_ACCESS, NF_ACCESS_CEALT(h->chip_en) | NF_ACCESS_WIDTH8);

	for (i = 0; i < bytes; i++) {
		u32 v;

		fl_writel(h, FLASH_FLASH_ACCESS_START,
			  FLASH_START_REGREQ | FLASH_START_CMD(FLASH_RD));
		if (wait_flash_start_clear(h, FLASH_START_REGREQ))
			break;
		v = fl_readl(h, FLASH_NF_DATA);
		h->own_buf[i] = (v >> ((i << 3) % 32)) & 0xff;
	}
	h->buf_top = h->own_buf;
	h->buf_offs = 0;
	h->buf_data_len = bytes;

	fl_writel(h, FLASH_NF_ECC_RESET,
		  ECC_RESET_ECC_CLR | ECC_RESET_FIFO_CLR | ECC_RESET_NF_RESET);
}

#define READID_DATA_LEN	8
#define PARAM_DATA_LEN	sizeof(struct nand_onfi_params)

static void cs752x_nand_command(struct nand_chip *chip, unsigned int command,
				int column, int page_addr)
{
	struct cs752x_nand_host *h = to_host(chip);
	struct mtd_info *mtd = nand_to_mtd(chip);

	h->flag_status_req = false;
	h->buf_offs = 0;
	h->buf_data_len = 0;
	h->buf_top = NULL;

	if (column != -1) {
		if (chip->options & NAND_BUSWIDTH_16)
			column >>= 1;
		h->cur_col = column;
	}
	if (page_addr != -1)
		h->cur_page = page_addr;

	switch (command) {
	case NAND_CMD_STATUS:
		h->flag_status_req = true;
		fl_writel(h, FLASH_NF_ECC_CONTROL, 0);
		fl_writel(h, FLASH_NF_COUNT,
			  NF_CNT_OOB(NCNT_EMPTY_OOB) | NF_CNT_DATA(NCNT_DATA_1) |
			  NF_CNT_ADDR(NCNT_EMPTY_ADDR) | NF_CNT_CMD(NCNT_CMD_1));
		fl_writel(h, FLASH_NF_COMMAND, mk_nf_command(NAND_CMD_STATUS, 0, 0));
		fl_writel(h, FLASH_NF_ACCESS,
			  NF_ACCESS_CEALT(h->chip_en) | NF_ACCESS_WIDTH8);
		fl_writel(h, FLASH_FLASH_ACCESS_START,
			  FLASH_START_REGREQ | FLASH_START_CMD(FLASH_RD));
		wait_flash_start_clear(h, FLASH_START_REGREQ);
		clear_command_cache(h);
		return;

	case NAND_CMD_READID:
		do_readid_param(h, column, NAND_CMD_READID, READID_DATA_LEN);
		clear_command_cache(h);
		return;

	case NAND_CMD_PARAM:
		do_readid_param(h, column, NAND_CMD_PARAM, PARAM_DATA_LEN);
		clear_command_cache(h);
		return;

	case NAND_CMD_ERASE1:
		add_to_command_cache(h, NAND_CMD_ERASE1);
		h->cmd_page = page_addr;
		return;

	case NAND_CMD_ERASE2:
		if (h->cmd_cnt == 1 && h->cmd_array[0] == NAND_CMD_ERASE1)
			cs752x_nand_erase_block(h, h->cmd_page);
		clear_command_cache(h);
		return;

	case NAND_CMD_READOOB:
		cs752x_do_read_oob(h, page_addr);
		h->buf_top = h->own_buf;
		h->buf_offs = column > 0 ? column : 0;
		h->buf_data_len = mtd->oobsize;
		clear_command_cache(h);
		return;

	case NAND_CMD_PAGEPROG:
	case NAND_CMD_SEQIN:
	case NAND_CMD_READ0:
		return;

	case NAND_CMD_RESET:
		check_flash_ctrl_status(h);
		udelay(chip->legacy.chip_delay);
		fl_writel(h, FLASH_NF_ECC_CONTROL, 0);
		fl_writel(h, FLASH_NF_COUNT,
			  NF_CNT_OOB(NCNT_EMPTY_OOB) | NF_CNT_DATA(NCNT_EMPTY_DATA) |
			  NF_CNT_ADDR(NCNT_EMPTY_ADDR) | NF_CNT_CMD(NCNT_CMD_1));
		fl_writel(h, FLASH_NF_COMMAND, mk_nf_command(NAND_CMD_RESET, 0, 0));
		fl_writel(h, FLASH_NF_ADDRESS_1, 0);
		fl_writel(h, FLASH_NF_ADDRESS_2, 0);
		fl_writel(h, FLASH_NF_ACCESS,
			  NF_ACCESS_CEALT(h->chip_en) | NF_ACCESS_WIDTH8);
		fl_writel(h, FLASH_FLASH_ACCESS_START,
			  FLASH_START_REGREQ | FLASH_START_CMD(FLASH_WT));
		wait_flash_start_clear(h, FLASH_START_REGREQ);
		udelay(100);
		break;

	default:
		if (!chip->legacy.dev_ready) {
			udelay(chip->legacy.chip_delay);
			return;
		}
	}

	udelay(100);
	nand_wait_ready(chip);
}

static void cs752x_nand_select_chip(struct nand_chip *chip, int cs)
{
	to_host(chip)->chip_en = (cs == 1) ? 1 : 0;
}

static int cs752x_nand_dev_ready(struct nand_chip *chip)
{
	struct cs752x_nand_host *h = to_host(chip);
	unsigned long timeo = jiffies + HZ;
	int ready;

	check_flash_ctrl_status(h);
	fl_writel(h, FLASH_NF_DATA, 0xffffffff);

	do {
		cs752x_nand_command(chip, NAND_CMD_STATUS, -1, -1);
		ready = cs752x_nand_read_byte(chip);
		if (ready != 0xff)
			break;
		cond_resched();
	} while (time_before(jiffies, timeo));

	if (ready == 0xff) {
		dev_err(h->dev, "timeout waiting for the chip status\n");
		return 0;
	}
	return ready & NAND_STATUS_READY;
}

static void cs752x_nand_hwcontrol(struct nand_chip *chip, int cmd, unsigned int ctrl)
{
}

static int cs752x_nand_features(struct nand_chip *chip, int addr, u8 *param)
{
	return -ENOTSUPP;
}

static int init_dma_ssp(struct cs752x_nand_host *h)
{
	u32 v;
	int i;

	v = dma_readl(h, DMA_SSP_RXDMA_CONTROL);
	if ((v & (DMA_CTRL_ENABLE | DMA_CTRL_CHECK_OWN)) != (DMA_CTRL_ENABLE | DMA_CTRL_CHECK_OWN))
		dma_writel(h, DMA_SSP_RXDMA_CONTROL, v | DMA_CTRL_ENABLE | DMA_CTRL_CHECK_OWN);
	v = dma_readl(h, DMA_SSP_TXDMA_CONTROL);
	if ((v & (DMA_CTRL_ENABLE | DMA_CTRL_CHECK_OWN)) != (DMA_CTRL_ENABLE | DMA_CTRL_CHECK_OWN))
		dma_writel(h, DMA_SSP_TXDMA_CONTROL, v | DMA_CTRL_ENABLE | DMA_CTRL_CHECK_OWN);

	h->tx_desc = dmam_alloc_coherent(h->dev, sizeof(*h->tx_desc) * FDMA_DESC_NUM,
					 &h->tx_desc_dma, GFP_KERNEL);
	h->rx_desc = dmam_alloc_coherent(h->dev, sizeof(*h->rx_desc) * FDMA_DESC_NUM,
					 &h->rx_desc_dma, GFP_KERNEL);
	if (!h->tx_desc || !h->rx_desc)
		return -ENOMEM;

	dma_writel(h, DMA_SSP_RXQ5_BASE_DEPTH, (h->rx_desc_dma & ~0xf) | FDMA_DEPTH);
	dma_writel(h, DMA_SSP_TXQ5_BASE_DEPTH, (h->tx_desc_dma & ~0xf) | FDMA_DEPTH);

	for (i = 0; i < FDMA_DESC_NUM; i++)
		h->tx_desc[i].word0 = DESC_OWN(OWN_SW) | DESC_SGM;

	return 0;
}

static int cs752x_nand_attach_chip(struct nand_chip *chip)
{
	struct mtd_info *mtd = nand_to_mtd(chip);
	struct cs752x_nand_host *h = to_host(chip);

	chip->ecc.engine_type = NAND_ECC_ENGINE_TYPE_ON_HOST;
	chip->ecc.placement = NAND_ECC_PLACEMENT_OOB;

#if defined(CONFIG_CS752X_NAND_ECC_HW_BCH_8_512)
	chip->ecc.algo = NAND_ECC_ALGO_BCH;
	chip->ecc.size = 512;
	chip->ecc.bytes = 13;
	chip->ecc.strength = 8;
#elif defined(CONFIG_CS752X_NAND_ECC_HW_BCH_12_512)
	chip->ecc.algo = NAND_ECC_ALGO_BCH;
	chip->ecc.size = 512;
	chip->ecc.bytes = 20;
	chip->ecc.strength = 12;
#elif defined(CONFIG_CS752X_NAND_ECC_HW_HAMMING_512)
	chip->ecc.algo = NAND_ECC_ALGO_HAMMING;
	chip->ecc.size = 512;
	chip->ecc.bytes = 3;
	chip->ecc.strength = 1;
#else
	chip->ecc.algo = NAND_ECC_ALGO_HAMMING;
	chip->ecc.size = 256;
	chip->ecc.bytes = 3;
	chip->ecc.strength = 1;
#endif

	h->ecc_calc = devm_kzalloc(h->dev, mtd->oobsize, GFP_KERNEL);
	h->ecc_code = devm_kzalloc(h->dev, mtd->oobsize, GFP_KERNEL);
	h->page_buf = devm_kmalloc(h->dev, mtd->writesize, GFP_KERNEL);
	if (!h->ecc_calc || !h->ecc_code || !h->page_buf)
		return -ENOMEM;

	if (mtd->writesize < chip->ecc.size) {
		dev_err(h->dev, "%d byte HW ECC not possible on %d byte page\n",
			chip->ecc.size, mtd->writesize);
		return -EINVAL;
	}

#ifdef CS752X_ECC_BCH
	if (mtd->oobsize == 16)
		mtd_set_ooblayout(mtd, &cs752x_ooblayout_ops_bch16);
	else
		mtd_set_ooblayout(mtd, &cs752x_ooblayout_ops_bch_lp);
#else
	if (mtd->oobsize == 8 || mtd->oobsize == 16)
		mtd_set_ooblayout(mtd, nand_get_small_page_ooblayout());
	else
		mtd_set_ooblayout(mtd, nand_get_large_page_hamming_ooblayout());
#endif

	chip->ecc.read_page	 = cs752x_nand_read_page_hwecc;
	chip->ecc.write_page	 = cs752x_nand_write_page_hwecc;
	chip->ecc.read_page_raw	 = cs752x_nand_read_page_raw;
	chip->ecc.write_page_raw = cs752x_nand_write_page_raw;
	chip->ecc.write_oob	 = cs752x_nand_write_oob_std;

	dev_info(h->dev, "HW ECC %s, %d bytes per %d-byte step, page %u, oob %u\n",
		 chip->ecc.algo == NAND_ECC_ALGO_BCH ? "BCH" : "Hamming",
		 chip->ecc.bytes, chip->ecc.size, mtd->writesize, mtd->oobsize);
	return 0;
}

static const struct nand_controller_ops cs752x_nand_controller_ops = {
	.attach_chip = cs752x_nand_attach_chip,
};

static int cs752x_nand_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	struct cs752x_nand_host *h;
	struct nand_chip *chip;
	struct mtd_info *mtd;
	struct resource *rmem;
	int err;

	h = devm_kzalloc(dev, sizeof(*h), GFP_KERNEL);
	if (!h)
		return -ENOMEM;
	h->dev = dev;

	rmem = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if (!rmem)
		return dev_err_probe(dev, -ENODEV, "no NAND memory window\n");
	h->dma_phy_base = rmem->start;

	h->iobase_fl = devm_platform_ioremap_resource(pdev, 1);
	if (IS_ERR(h->iobase_fl))
		return PTR_ERR(h->iobase_fl);

	h->iobase_dma = devm_platform_ioremap_resource(pdev, 2);
	if (IS_ERR(h->iobase_dma))
		return PTR_ERR(h->iobase_dma);

	err = dma_set_mask_and_coherent(dev, DMA_BIT_MASK(32));
	if (err)
		return err;

	err = init_dma_ssp(h);
	if (err)
		return err;

	nand_controller_init(&h->controller);
	h->controller.ops = &cs752x_nand_controller_ops;

	chip = &h->chip;
	mtd = nand_to_mtd(chip);
	chip->controller = &h->controller;
	nand_set_flash_node(chip, dev->of_node);
	nand_set_controller_data(chip, h);
	mtd->dev.parent = dev;
	mtd->name = "cs752x_nand_flash";

	chip->legacy.chip_delay	= 20;
	chip->legacy.cmd_ctrl	= cs752x_nand_hwcontrol;
	chip->legacy.dev_ready	= cs752x_nand_dev_ready;
	chip->legacy.cmdfunc	= cs752x_nand_command;
	chip->legacy.select_chip = cs752x_nand_select_chip;
	chip->legacy.read_byte	= cs752x_nand_read_byte;
	chip->legacy.read_buf	= cs752x_nand_read_buf;
	chip->legacy.write_buf	= cs752x_nand_write_buf;
	chip->legacy.set_features = cs752x_nand_features;
	chip->legacy.get_features = cs752x_nand_features;

	chip->options |= NAND_NO_SUBPAGE_WRITE;

	dev_info(dev, "flash type 0x%08x\n", fl_readl(h, FLASH_TYPE));

	err = nand_scan(chip, 1);
	if (err)
		return err;

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
	{ .compatible = "cortina,cs752x-nand" },
	{}
};
MODULE_DEVICE_TABLE(of, cs752x_nand_dt_ids);

static struct platform_driver cs752x_nand_driver = {
	.driver = {
		.name	= "cs752x_nand",
		.of_match_table = cs752x_nand_dt_ids,
	},
	.probe		= cs752x_nand_probe,
	.remove		= cs752x_nand_remove,
};
module_platform_driver(cs752x_nand_driver);

MODULE_LICENSE("GPL v2");
MODULE_DESCRIPTION("NAND flash driver for Cortina CS752x");
MODULE_ALIAS("platform:cs752x_nand");

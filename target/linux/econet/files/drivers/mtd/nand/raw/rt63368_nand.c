// SPDX-License-Identifier: GPL-2.0-only
/*
 * RT63365/RT63368 parallel NAND flash controller (TrendChip/Ralink NFC).
 *
 * Written for the ZyXEL Keenetic LTE from the vendor ralink_nand.c
 * (TCSUPPORT_NAND_RT63368), which the stock U-Boot also uses. The
 * controller runs whole transactions (commands, address cycles and data
 * length in CONF) and moves data through a 32-bit PIO FIFO, so this driver
 * implements the legacy cmdfunc interface on top of a page bounce buffer.
 *
 * ECC is the controller's Hamming code: 3 bytes per 512-byte sector,
 * stored at OOB 5+16*i..7+16*i, 1-bit correction. OOB bytes 0-3 belong to
 * the vendor BMT (see en75_bmt, "econet,bmt").
 *
 * All-0xff pages are never programmed (they would get ECC 000000 and could
 * not be programmed again).
 */

#include <linux/bitops.h>
#include <linux/delay.h>
#include <linux/io.h>
#include <linux/iopoll.h>
#include <linux/module.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/partitions.h>
#include <linux/mtd/rawnand.h>
#include <linux/of.h>
#include <linux/platform_device.h>

#include <linux/mtd/mtk_bmt.h>

#define NFC_CTRL		0x10
#define NFC_CONF		0x14
#define NFC_CMD1		0x18
#define NFC_CMD2		0x1c
#define NFC_CMD3		0x20
#define NFC_ADDR		0x24
#define NFC_DATA		0x28
#define NFC_STATUS		0x30
#define NFC_INT_ST		0x38
#define NFC_CTRLII		0x3c
#define NFC_ECC(i)		(0x40 + (i) * 4)
#define NFC_ECC_ST(i)		(0x50 + (i) * 4)
#define NFC_ADDRII		0x60

#define STATUS_READY		BIT(2)

#define INT_ND_DONE		BIT(0)
#define INT_TX_BUF_RDY		BIT(1)
#define INT_RX_BUF_RDY		BIT(2)

#define CTRL_WP_DISABLE		BIT(0)	/* drive WP# high: writes allowed */

#define ECC_ST_FOUND		BIT(0)
#define ECC_ST_BYTE(x)		(((x) >> 6) & 0x1ff)
#define ECC_ST_BIT(x)		(((x) >> 2) & 0x7)

/* 2K page, big-endian FIFO (first flash byte in bits 31:24) */
#define CTRLII_VAL		0x76503

/* CONF: [31:20] length, [18:16] address cycles, bit3 ECC, [1:0] mode */
#define CONF_LEN(n)		((u32)(n) << 20)
#define CONF_ADDR_CYCLES(n)	((u32)(n) << 16)
#define CONF_ECC		BIT(3)
#define CONF_READ_PAGE		0x511	/* CMD1 + CMD2, read (vendor value) */
#define CONF_CMD_READ		0x101	/* CMD1 only, read (vendor value) */
#define CONF_PROGRAM		0x1103	/* CMD1 + CMD3, write (vendor value) */
#define CONF_ERASE		0x503	/* CMD1 + CMD2, no data (vendor value) */

#define RT63368_PAGE_SIZE	2048
#define RT63368_OOB_SIZE	64
#define RT63368_ECC_STEPS	4
#define RT63368_ECC_SIZE	512
#define RT63368_ECC_BYTES	3
#define RT63368_ADDR_CYCLES	4	/* 2 column + 2 row */

#define NFC_TIMEOUT_US		100000

/* HIR: RT63365-family ID in the upper half; pin mux for NAND. */
#define RT63368_HIR		0x1fb00064
#define RT63368_PINMUX		0x1fb00860
#define RT63368_PINMUX_NAND	BIT(8)

struct rt63368_nfc {
	struct device *dev;
	void __iomem *base;
	struct nand_controller controller;
	struct nand_chip chip;

	u8 buf[RT63368_PAGE_SIZE + RT63368_OOB_SIZE] __aligned(4);
	unsigned int buf_len;
	unsigned int buf_pos;
	int buf_err;

	/* SEQIN/write_buf/PAGEPROG (raw program) and ERASE1/2 state */
	u8 wbuf[RT63368_PAGE_SIZE + RT63368_OOB_SIZE] __aligned(4);
	unsigned int wlen;
	int wpage, wcol, epage;

	/* ECC state of the last full page read */
	bool ecc_valid;
	u32 ecc[RT63368_ECC_STEPS];
	u32 ecc_st[RT63368_ECC_STEPS];
};

static inline struct rt63368_nfc *to_nfc(struct nand_chip *chip)
{
	return container_of(chip, struct rt63368_nfc, chip);
}

static inline u32 nfc_r(struct rt63368_nfc *nfc, u32 reg)
{
	return __raw_readl(nfc->base + reg);
}

static inline void nfc_w(struct rt63368_nfc *nfc, u32 reg, u32 val)
{
	__raw_writel(val, nfc->base + reg);
}

static void nfc_clear_int(struct rt63368_nfc *nfc)
{
	nfc_w(nfc, NFC_INT_ST, nfc_r(nfc, NFC_INT_ST));
}

static int nfc_wait_ready(struct rt63368_nfc *nfc)
{
	u32 val;
	int ret;

	ret = readx_poll_timeout_atomic(__raw_readl, nfc->base + NFC_INT_ST,
					val, val & INT_ND_DONE, 1,
					NFC_TIMEOUT_US);
	if (ret)
		return ret;
	return readx_poll_timeout_atomic(__raw_readl, nfc->base + NFC_STATUS,
					 val, val & STATUS_READY, 1,
					 NFC_TIMEOUT_US);
}

/* Run one read transaction and drain @len bytes from the FIFO into @dst. */
static int nfc_read_xfer(struct rt63368_nfc *nfc, u8 cmd1, int cmd2,
			 u32 addr, u32 addr2, u32 conf, u8 *dst,
			 unsigned int len)
{
	unsigned int n = 0;
	u32 st, word;
	int ret;

	nfc_clear_int(nfc);
	nfc_w(nfc, NFC_CMD1, cmd1);
	if (cmd2 >= 0)
		nfc_w(nfc, NFC_CMD2, cmd2);
	nfc_w(nfc, NFC_ADDR, addr);
	nfc_w(nfc, NFC_ADDRII, addr2);
	nfc_w(nfc, NFC_CONF, conf);

	while (n < len) {
		ret = readx_poll_timeout_atomic(__raw_readl,
						nfc->base + NFC_INT_ST, st,
						st & INT_RX_BUF_RDY, 1,
						NFC_TIMEOUT_US);
		if (ret) {
			dev_err_ratelimited(nfc->dev,
				"read cmd %02x: FIFO timeout at %u/%u (INT_ST=%08x)\n",
				cmd1, n, len, st);
			return ret;
		}
		word = nfc_r(nfc, NFC_DATA);
		nfc_w(nfc, NFC_INT_ST, INT_RX_BUF_RDY);
		/* Big-endian FIFO: first byte in bits 31:24. Bounce the tail. */
		dst[n++] = word >> 24;
		if (n < len)
			dst[n++] = word >> 16;
		if (n < len)
			dst[n++] = word >> 8;
		if (n < len)
			dst[n++] = word;
	}

	return nfc_wait_ready(nfc);
}

/* Run one program transaction pushing @len bytes from @src into the FIFO. */
static int nfc_write_xfer(struct rt63368_nfc *nfc, u32 addr, u32 addr2,
			  u32 conf, const u8 *src, unsigned int len)
{
	unsigned int n = 0;
	u32 st, word;
	int ret, k;

	nfc_clear_int(nfc);
	nfc_w(nfc, NFC_CMD1, NAND_CMD_SEQIN);
	nfc_w(nfc, NFC_CMD3, NAND_CMD_PAGEPROG);
	nfc_w(nfc, NFC_ADDR, addr);
	nfc_w(nfc, NFC_ADDRII, addr2);
	nfc_w(nfc, NFC_CONF, conf);

	while (n < len) {
		ret = readx_poll_timeout_atomic(__raw_readl,
						nfc->base + NFC_INT_ST, st,
						st & INT_TX_BUF_RDY, 1,
						NFC_TIMEOUT_US);
		if (ret) {
			dev_err_ratelimited(nfc->dev,
				"program: FIFO timeout at %u/%u (INT_ST=%08x)\n",
				n, len, st);
			return ret;
		}
		/* Big-endian FIFO: first byte in bits 31:24; pad with 0xff. */
		word = 0;
		for (k = 0; k < 4; k++)
			word = word << 8 | (n + k < len ? src[n + k] : 0xff);
		nfc_w(nfc, NFC_INT_ST, INT_TX_BUF_RDY);
		nfc_w(nfc, NFC_DATA, word);
		n += 4;
	}

	return nfc_wait_ready(nfc);
}

static int nfc_program(struct rt63368_nfc *nfc, int page, int col,
		       const u8 *src, unsigned int len, bool ecc)
{
	return nfc_write_xfer(nfc, ((u32)page << 16) | col, (u32)page >> 16,
			      CONF_PROGRAM | (ecc ? CONF_ECC : 0) |
			      CONF_ADDR_CYCLES(RT63368_ADDR_CYCLES) |
			      CONF_LEN(len), src, len);
}

static int nfc_erase(struct rt63368_nfc *nfc, int page)
{
	nfc_clear_int(nfc);
	nfc_w(nfc, NFC_CMD1, NAND_CMD_ERASE1);
	nfc_w(nfc, NFC_CMD2, NAND_CMD_ERASE2);
	nfc_w(nfc, NFC_ADDR, page);
	nfc_w(nfc, NFC_ADDRII, 0);
	nfc_w(nfc, NFC_CONF, CONF_ERASE | CONF_ADDR_CYCLES(2));
	return nfc_wait_ready(nfc);
}

static int nfc_status(struct rt63368_nfc *nfc, u8 *status)
{
	return nfc_read_xfer(nfc, NAND_CMD_STATUS, -1, 0, 0,
			     CONF_CMD_READ | CONF_LEN(1), status, 1);
}

static int nfc_reset(struct rt63368_nfc *nfc)
{
	nfc_clear_int(nfc);
	nfc_w(nfc, NFC_CMD1, NAND_CMD_RESET);
	nfc_w(nfc, NFC_CONF, CONF_CMD_READ);
	return nfc_wait_ready(nfc);
}

/* Full page + OOB read with hardware ECC; remembers the ECC registers. */
static int nfc_read_page_ecc(struct rt63368_nfc *nfc, int page)
{
	int i, ret;

	nfc->ecc_valid = false;
	ret = nfc_read_xfer(nfc, NAND_CMD_READ0, NAND_CMD_READSTART,
			    (u32)page << 16, (u32)page >> 16,
			    CONF_READ_PAGE | CONF_ECC |
			    CONF_ADDR_CYCLES(RT63368_ADDR_CYCLES) |
			    CONF_LEN(RT63368_PAGE_SIZE + RT63368_OOB_SIZE),
			    nfc->buf, RT63368_PAGE_SIZE + RT63368_OOB_SIZE);
	if (ret)
		return ret;

	for (i = 0; i < RT63368_ECC_STEPS; i++) {
		nfc->ecc[i] = nfc_r(nfc, NFC_ECC(i)) & 0xffffff;
		nfc->ecc_st[i] = nfc_r(nfc, NFC_ECC_ST(i));
	}
	nfc->ecc_valid = true;
	return 0;
}

static int nfc_read_oob(struct rt63368_nfc *nfc, int page)
{
	return nfc_read_xfer(nfc, NAND_CMD_READ0, NAND_CMD_READSTART,
			     ((u32)page << 16) | RT63368_PAGE_SIZE,
			     (u32)page >> 16,
			     CONF_READ_PAGE |
			     CONF_ADDR_CYCLES(RT63368_ADDR_CYCLES) |
			     CONF_LEN(RT63368_OOB_SIZE),
			     nfc->buf, RT63368_OOB_SIZE);
}

static void rt63368_cmdfunc(struct nand_chip *chip, unsigned int command,
			    int column, int page_addr)
{
	struct rt63368_nfc *nfc = to_nfc(chip);
	unsigned int len;
	int ret = 0;

	switch (command) {
	case NAND_CMD_RESET:
		ret = nfc_reset(nfc);
		nfc->buf_len = 0;
		break;

	case NAND_CMD_READID:
		len = 8;
		ret = nfc_read_xfer(nfc, NAND_CMD_READID, -1, column & 0xff, 0,
				    CONF_CMD_READ | CONF_ADDR_CYCLES(1) |
				    CONF_LEN(len), nfc->buf, len);
		nfc->buf_len = len;
		nfc->buf_pos = 0;
		break;

	case NAND_CMD_PARAM:
		len = 3 * 256;
		ret = nfc_read_xfer(nfc, NAND_CMD_PARAM, -1, column & 0xff, 0,
				    CONF_CMD_READ | CONF_ADDR_CYCLES(1) |
				    CONF_LEN(len), nfc->buf, len);
		nfc->buf_len = len;
		nfc->buf_pos = 0;
		break;

	case NAND_CMD_STATUS:
		ret = nfc_read_xfer(nfc, NAND_CMD_STATUS, -1, 0, 0,
				    CONF_CMD_READ | CONF_LEN(1), nfc->buf, 1);
		nfc->buf_len = 1;
		nfc->buf_pos = 0;
		break;

	case NAND_CMD_READ0:
		ret = nfc_read_page_ecc(nfc, page_addr);
		nfc->buf_len = RT63368_PAGE_SIZE + RT63368_OOB_SIZE;
		nfc->buf_pos = column;
		break;

	case NAND_CMD_READOOB:
		ret = nfc_read_oob(nfc, page_addr);
		nfc->ecc_valid = false;
		nfc->buf_len = RT63368_OOB_SIZE;
		nfc->buf_pos = column;
		break;

	case NAND_CMD_RNDOUT:
		/* Whole page is buffered: just move the read pointer. */
		nfc->buf_pos = column;
		break;

	case NAND_CMD_READSTART:
	case NAND_CMD_RNDOUTSTART:
		break;

	case NAND_CMD_SEQIN:
		nfc->wpage = page_addr;
		nfc->wcol = column;
		nfc->wlen = 0;
		break;

	case NAND_CMD_PAGEPROG:
		/* Raw program (no ECC): OOB-only and raw page writes. */
		ret = nfc_program(nfc, nfc->wpage, nfc->wcol, nfc->wbuf,
				  nfc->wlen, false);
		break;

	case NAND_CMD_ERASE1:
		nfc->epage = page_addr;
		break;

	case NAND_CMD_ERASE2:
		ret = nfc_erase(nfc, nfc->epage);
		break;

	default:
		dev_warn_ratelimited(nfc->dev, "unsupported NAND command 0x%02x\n",
				     command);
		ret = -EINVAL;
		break;
	}

	nfc->buf_err = ret;
	if (ret && ret != -EROFS)
		dev_err_ratelimited(nfc->dev, "command 0x%02x page %d: %d\n",
				    command, page_addr, ret);
}

static u8 rt63368_read_byte(struct nand_chip *chip)
{
	struct rt63368_nfc *nfc = to_nfc(chip);

	if (nfc->buf_pos >= nfc->buf_len)
		return 0xff;
	return nfc->buf[nfc->buf_pos++];
}

static void rt63368_read_buf(struct nand_chip *chip, u8 *buf, int len)
{
	struct rt63368_nfc *nfc = to_nfc(chip);
	unsigned int avail = nfc->buf_pos < nfc->buf_len ?
			     nfc->buf_len - nfc->buf_pos : 0;
	unsigned int n = min_t(unsigned int, len, avail);

	memcpy(buf, nfc->buf + nfc->buf_pos, n);
	if (n < len)
		memset(buf + n, 0xff, len - n);
	nfc->buf_pos += n;
}

static void rt63368_write_buf(struct nand_chip *chip, const u8 *buf, int len)
{
	struct rt63368_nfc *nfc = to_nfc(chip);
	unsigned int n = min_t(unsigned int, len, sizeof(nfc->wbuf) - nfc->wlen);

	memcpy(nfc->wbuf + nfc->wlen, buf, n);
	nfc->wlen += n;
}

static void rt63368_select_chip(struct nand_chip *chip, int cs)
{
	/*
	 * The controller asserts CE# itself for every transaction. Without
	 * this hook the core installs nand_select_chip(), which needs the
	 * legacy cmd_ctrl() this driver does not have.
	 */
}

static int rt63368_dev_ready(struct nand_chip *chip)
{
	/* Every transaction waits for R/B# itself. */
	return 1;
}

static int rt63368_read_page(struct nand_chip *chip, u8 *buf,
			     int oob_required, int page)
{
	struct rt63368_nfc *nfc = to_nfc(chip);
	struct mtd_info *mtd = nand_to_mtd(chip);
	unsigned int max_bitflips = 0;
	int i, ret;

	ret = nand_read_page_op(chip, page, 0, NULL, 0);
	if (ret)
		return ret;
	if (nfc->buf_err)
		return nfc->buf_err;
	if (!nfc->ecc_valid)
		return -EIO;

	memcpy(buf, nfc->buf, RT63368_PAGE_SIZE);
	memcpy(chip->oob_poi, nfc->buf + RT63368_PAGE_SIZE, RT63368_OOB_SIZE);

	/* Vendor rule: HW ECC 0 for sector 0 means a clean (erased) page. */
	if (!nfc->ecc[0]) {
		int bits = nand_check_erased_ecc_chunk(buf, RT63368_PAGE_SIZE,
						       chip->oob_poi,
						       RT63368_OOB_SIZE,
						       NULL, 0,
						       chip->ecc.strength);
		if (bits >= 0)
			return bits;
	}

	for (i = 0; i < RT63368_ECC_STEPS; i++) {
		u8 *data = buf + i * RT63368_ECC_SIZE;
		u8 *eccb = chip->oob_poi + 5 + 16 * i;
		u32 stored = eccb[0] | eccb[1] << 8 | eccb[2] << 16;
		u32 syndrome = stored ^ nfc->ecc[i];
		int bits;

		if (stored == 0xffffff) {
			if (nfc->ecc[i] == 0xffffff)
				continue;	/* genuine all-ones ECC */
			bits = nand_check_erased_ecc_chunk(data,
							   RT63368_ECC_SIZE,
							   eccb,
							   RT63368_ECC_BYTES,
							   NULL, 0, 1);
			if (bits >= 0) {
				mtd->ecc_stats.corrected += bits;
				max_bitflips = max_t(unsigned int,
						     max_bitflips, bits);
			} else {
				/*
				 * Data without stored ECC: the vendor driver
				 * and U-Boot skip the check ("ECC data not be
				 * written"). Do the same.
				 */
				dev_dbg_ratelimited(nfc->dev,
					"page %d sector %d: no stored ECC, unchecked\n",
					page, i);
			}
			continue;
		}

		if (!syndrome)
			continue;

		bits = hweight32(syndrome);
		if (bits == 1) {
			/* Bitflip in the stored ECC bytes, data is fine. */
			mtd->ecc_stats.corrected++;
			max_bitflips = max_t(unsigned int, max_bitflips, 1);
		} else if (bits == 12 && (nfc->ecc_st[i] & ECC_ST_FOUND)) {
			unsigned int byte = ECC_ST_BYTE(nfc->ecc_st[i]);

			if (byte < RT63368_ECC_SIZE)
				data[byte] ^= BIT(ECC_ST_BIT(nfc->ecc_st[i]));
			mtd->ecc_stats.corrected++;
			max_bitflips = max_t(unsigned int, max_bitflips, 1);
		} else {
			mtd->ecc_stats.failed++;
			dev_warn_ratelimited(nfc->dev,
				"ECC fail page %d sector %d: stored %06x hw %06x syndrome %06x (%d bits) ECC_ST %08x\n",
				page, i, stored, nfc->ecc[i], syndrome, bits,
				nfc->ecc_st[i]);
		}
	}

	return max_bitflips;
}

static bool rt63368_all_ff(const u8 *p, unsigned int len)
{
	while (len--)
		if (*p++ != 0xff)
			return false;
	return true;
}

static int rt63368_write_page(struct nand_chip *chip, const u8 *buf,
			      int oob_required, int page)
{
	struct rt63368_nfc *nfc = to_nfc(chip);
	u8 *oob = nfc->wbuf + RT63368_PAGE_SIZE;
	u8 status;
	int i, ret;


	memcpy(nfc->wbuf, buf, RT63368_PAGE_SIZE);
	if (oob_required)
		memcpy(oob, chip->oob_poi, RT63368_OOB_SIZE);
	else
		memset(oob, 0xff, RT63368_OOB_SIZE);
	/* The controller inserts the ECC bytes; send 0xff there. */
	for (i = 0; i < RT63368_ECC_STEPS; i++)
		memset(oob + 5 + 16 * i, 0xff, RT63368_ECC_BYTES);

	/* Never program an all-0xff page: leave it erased. */
	if (rt63368_all_ff(nfc->wbuf, sizeof(nfc->wbuf)))
		return 0;

	ret = nfc_program(nfc, page, 0, nfc->wbuf, sizeof(nfc->wbuf), true);
	if (ret)
		return ret;
	ret = nfc_status(nfc, &status);
	if (ret)
		return ret;
	return status & NAND_STATUS_FAIL ? -EIO : 0;
}

static int rt63368_ooblayout_ecc(struct mtd_info *mtd, int section,
				 struct mtd_oob_region *oobregion)
{
	if (section >= RT63368_ECC_STEPS)
		return -ERANGE;
	oobregion->offset = 5 + 16 * section;
	oobregion->length = RT63368_ECC_BYTES;
	return 0;
}

static int rt63368_ooblayout_free(struct mtd_info *mtd, int section,
				  struct mtd_oob_region *oobregion)
{
	/* Vendor free list; bytes 0-3 are the bad-block marker and BMT. */
	static const u8 free_regions[][2] = {
		{ 4, 1 }, { 8, 13 }, { 24, 13 }, { 40, 11 }, { 52, 1 },
		{ 56, 8 },
	};

	if (section >= ARRAY_SIZE(free_regions))
		return -ERANGE;
	oobregion->offset = free_regions[section][0];
	oobregion->length = free_regions[section][1];
	return 0;
}

static const struct mtd_ooblayout_ops rt63368_ooblayout_ops = {
	.ecc = rt63368_ooblayout_ecc,
	.free = rt63368_ooblayout_free,
};

static int rt63368_attach_chip(struct nand_chip *chip)
{
	struct mtd_info *mtd = nand_to_mtd(chip);

	if (mtd->writesize != RT63368_PAGE_SIZE ||
	    mtd->oobsize != RT63368_OOB_SIZE) {
		dev_err(to_nfc(chip)->dev, "unsupported page %u / oob %u\n",
			mtd->writesize, mtd->oobsize);
		return -EINVAL;
	}

	chip->ecc.engine_type = NAND_ECC_ENGINE_TYPE_ON_HOST;
	chip->ecc.placement = NAND_ECC_PLACEMENT_OOB;
	chip->ecc.size = RT63368_ECC_SIZE;
	chip->ecc.bytes = RT63368_ECC_BYTES;
	chip->ecc.strength = 1;
	chip->ecc.steps = RT63368_ECC_STEPS;
	chip->ecc.total = RT63368_ECC_STEPS * RT63368_ECC_BYTES;
	chip->ecc.read_page = rt63368_read_page;
	chip->ecc.write_page = rt63368_write_page;
	mtd_set_ooblayout(mtd, &rt63368_ooblayout_ops);
	return 0;
}

static const struct nand_controller_ops rt63368_controller_ops = {
	.attach_chip = rt63368_attach_chip,
};

static void rt63368_nfc_hw_init(struct rt63368_nfc *nfc)
{
	void __iomem *hir = ioremap(RT63368_HIR, 4);
	void __iomem *mux = ioremap(RT63368_PINMUX, 4);

	/* As the vendor driver: NAND pin mux on RT63365-family parts. */
	if (hir && mux && (__raw_readl(hir) >> 16) == 4 &&
	    !(__raw_readl(mux) & RT63368_PINMUX_NAND))
		__raw_writel(__raw_readl(mux) | RT63368_PINMUX_NAND, mux);
	if (hir)
		iounmap(hir);
	if (mux)
		iounmap(mux);

	/* Keep the bootloader's CTRL timing; force 2K page, BE FIFO. */
	nfc_w(nfc, NFC_CTRLII, CTRLII_VAL);
}

static int rt63368_nfc_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	struct rt63368_nfc *nfc;
	struct nand_chip *chip;
	struct mtd_info *mtd;
	int ret;

	nfc = devm_kzalloc(dev, sizeof(*nfc), GFP_KERNEL);
	if (!nfc)
		return -ENOMEM;
	nfc->dev = dev;

	nfc->base = devm_platform_ioremap_resource(pdev, 0);
	if (IS_ERR(nfc->base))
		return PTR_ERR(nfc->base);

	rt63368_nfc_hw_init(nfc);
	nfc_w(nfc, NFC_CTRL, nfc_r(nfc, NFC_CTRL) | CTRL_WP_DISABLE);

	nand_controller_init(&nfc->controller);
	nfc->controller.ops = &rt63368_controller_ops;

	chip = &nfc->chip;
	chip->controller = &nfc->controller;
	nand_set_flash_node(chip, dev->of_node);
	chip->options |= NAND_NO_SUBPAGE_WRITE | NAND_SKIP_BBTSCAN;
	chip->badblockpos = 0;
	chip->legacy.cmdfunc = rt63368_cmdfunc;
	chip->legacy.read_byte = rt63368_read_byte;
	chip->legacy.read_buf = rt63368_read_buf;
	chip->legacy.write_buf = rt63368_write_buf;
	chip->legacy.select_chip = rt63368_select_chip;
	chip->legacy.dev_ready = rt63368_dev_ready;
	chip->legacy.chip_delay = 20;

	mtd = nand_to_mtd(chip);
	mtd->dev.parent = dev;
	mtd->owner = THIS_MODULE;
	mtd->name = "rt63368-nand";

	ret = nand_scan(chip, 1);
	if (ret)
		return ret;

	ret = mtk_bmt_attach(mtd);
	if (ret)
		dev_warn(dev, "BMT attach failed: %d\n", ret);

	ret = mtd_device_register(mtd, NULL, 0);
	if (ret) {
		mtk_bmt_detach(mtd);
		nand_cleanup(chip);
		return ret;
	}

	platform_set_drvdata(pdev, nfc);
	return 0;
}

static void rt63368_nfc_remove(struct platform_device *pdev)
{
	struct rt63368_nfc *nfc = platform_get_drvdata(pdev);
	struct mtd_info *mtd = nand_to_mtd(&nfc->chip);

	WARN_ON(mtd_device_unregister(mtd));
	mtk_bmt_detach(mtd);
	nand_cleanup(&nfc->chip);
}

static const struct of_device_id rt63368_nfc_of_match[] = {
	{ .compatible = "econet,rt63368-nfc" },
	{ }
};
MODULE_DEVICE_TABLE(of, rt63368_nfc_of_match);

static struct platform_driver rt63368_nfc_driver = {
	.probe = rt63368_nfc_probe,
	.remove = rt63368_nfc_remove,
	.driver = {
		.name = "rt63368-nfc",
		.of_match_table = rt63368_nfc_of_match,
	},
};
module_platform_driver(rt63368_nfc_driver);

MODULE_DESCRIPTION("RT63368 parallel NAND flash controller");
MODULE_LICENSE("GPL");

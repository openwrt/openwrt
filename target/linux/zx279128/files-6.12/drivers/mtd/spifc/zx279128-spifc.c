// SPDX-License-Identifier: GPL-2.0
/*
 * ZTE ZX27912x SPIFC SPI-NAND controller driver.
 *
 * Reverse-engineered from the stock ZTE kernel (Linux 4.1.25) driver
 * "ZX279121-spifc". See plans/OPENWRT_BUILD_AND_PORT_PLAN.md Phase 7.1 for
 * the full derivation. Implements chip identification, page reads, page
 * program (0x02 load + 0x10 execute) and block erase (0xd8), mirroring the
 * stock sequences byte-for-byte. OOB access is not implemented; the
 * on-die ECC covers main data only as seen by this driver.
 *
 * Controller model (all register offsets are the byte offsets from the MMIO
 * base; the stock disassembly prints them in decimal):
 *
 *   0x00 config        idle 0x01040000
 *   0x04 trigger       |= 1 to start a transaction
 *   0x08 status        bit1 checked, bit0 set when !bit1
 *   0x0c config        |= 0x1c440 before trigger
 *   0x10 cmd field     control bitfields only (NO address)
 *   0x14 cmd field     width/phase bitfields
 *   0x18 len-1         transfer length minus one
 *   0x1c cmd field     the address argument (page/column/feature addr)
 *   0x20 opcode        raw SPI-NAND opcode
 *   0x2c completion    bit0 = transaction done, bit1 = error
 *   0x30 int clear     = 0x3f to clear
 *   0x34 fifo status   bit8 field 0x1f00 = read FIFO non-empty
 *                      bit16 field 0x1f0000 = write FIFO not full
 *   0x38 fifo data     read POPS a word / write pushes a word (HAZARD)
 *
 * Every transaction is the same handshake:
 *   lock; if (!(reg08 & 2)) reg08 |= 1; config_cmd(idx, addr, len);
 *   reg0c |= 0x1c440; if (!(reg2c & 2)) reg30 = 0x3f; reg04 |= 1;
 *   [FIFO drain/push]; spin until reg2c & 1; unlock.
 */

#include <linux/delay.h>
#include <linux/io.h>
#include <linux/jiffies.h>
#include <linux/module.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/partitions.h>
#include <linux/of.h>
#include <linux/of_address.h>
#include <linux/platform_device.h>
#include <linux/slab.h>
#include <linux/spinlock.h>
#include <linux/unaligned.h>

#define SPIFC_REG_CONFIG	0x00
#define SPIFC_REG_TRIGGER	0x04
#define SPIFC_REG_STATUS	0x08
#define SPIFC_REG_CONFIG2	0x0c
#define SPIFC_REG_CMD10		0x10
#define SPIFC_REG_CMD14		0x14
#define SPIFC_REG_LEN18		0x18
#define SPIFC_REG_ADDR1C	0x1c
#define SPIFC_REG_OPCODE	0x20
#define SPIFC_REG_DONE		0x2c
#define SPIFC_REG_INTCLEAR	0x30
#define SPIFC_REG_FIFOSTAT	0x34
#define SPIFC_REG_FIFODATA	0x38

#define SPIFC_CONFIG2_MASK	0x1c440
#define SPIFC_DONE_OK		BIT(0)
#define SPIFC_DONE_ERR		BIT(1)
#define SPIFC_FIFO_RD_READY	0x1f00
#define SPIFC_FIFO_WR_READY	0x1f0000

/* Command-table indices used by the read/write/erase paths. */
#define CMD_WRITE_ENABLE	0
#define CMD_SECTOR_ERASE	2
#define CMD_READ_JEDEC_ID	14
#define CMD_PROGRAM_EXEC	17
#define CMD_PROGRAM_LOAD	25
#define CMD_PAGE_READ_CACHE	18
#define CMD_DUAL_OUTPUT_READ	20
#define CMD_GET_FEATURE		22
#define CMD_SET_FEATURE		23
#define CMD_RESET		24

/* ZD35Q1GAIBR geometry. */
#define SPIFC_PAGE_DATA		2048
#define SPIFC_PAGE_OOB		64
#define SPIFC_ERASE_SIZE	(128 * 1024)

/* Feature addresses for the non-dual-die (ZD35Q1GAIBR) variant. */
#define FEAT_BLOCK_PROTECT	0xa0
#define FEAT_ECC_READ		0xb0
#define FEAT_STATUS		0xc0
#define FEAT_ECC_ENABLE		BIT(4)

#define SPIFC_TIMEOUT_JIF	500

/*
 * One entry of the stock 34-entry command table. reg10/reg14 are the final
 * field values the stock spifc_config_cmd writes (the d-bitfields ORed into a
 * zeroed register); len is the fixed transfer length (0 => caller supplies).
 * The address is never part of reg10; it goes to reg0x1c.
 */
struct spifc_cmd {
	u8	opcode;
	u32	reg10;
	u32	reg14;
	u32	len;
	const char *name;
};

static const struct spifc_cmd spifc_cmds[] = {
	[0]  = { 0x06, 0x0000000, 0x0000040, 0, "write enable" },
	[1]  = { 0x04, 0x0000000, 0x0000040, 0, "write disable" },
	[2]  = { 0xd8, 0x0000010, 0x0000040, 0, "sector erase" },
	[3]  = { 0x9f, 0x0000002, 0x0000040, 4, "read jedec id" },
	[4]  = { 0x90, 0x0000012, 0x0000040, 2, "read id" },
	[5]  = { 0x03, 0x0000012, 0x0000040, 0, "read data bytes" },
	[6]  = { 0x60, 0x0000000, 0x0000040, 0, "bulk erase" },
	[7]  = { 0x02, 0x0000011, 0x0000040, 0, "page program" },
	[8]  = { 0x3b, 0x0000016, 0x0001040, 0, "dual output read" },
	[9]  = { 0x6b, 0x0000016, 0x0001040, 0, "quad output read" },
	[10] = { 0x32, 0x0000011, 0x0000040, 0, "quad page program" },
	[11] = { 0x01, 0x0000001, 0x0000040, 0, "write status&config" },
	[12] = { 0x35, 0x0000002, 0x0000040, 1, "read config" },
	[13] = { 0x05, 0x0000002, 0x0000040, 1, "read status" },
	[14] = { 0x9f, 0x0000012, 0x0000000, 5, "read jedec id" },
	[15] = { 0x84, 0x0000011, 0x0000020, 0, "program random load" },
	[16] = { 0x34, 0x0000011, 0x0000020, 0, "program random load x4" },
	[17] = { 0x10, 0x0000010, 0x0000040, 0, "program execute" },
	[18] = { 0x13, 0x0000010, 0x0000040, 0, "page read to cache" },
	[19] = { 0x03, 0x0000016, 0x0001020, 0, "single output read" },
	[20] = { 0x3b, 0x0000016, 0x0001020, 0, "dual output read" },
	[21] = { 0x6b, 0x0000016, 0x0001020, 0, "quad output read" },
	[22] = { 0x0f, 0x0000012, 0x0000000, 1, "get feature" },
	[23] = { 0x1f, 0x0000011, 0x0000000, 1, "set feature" },
	[24] = { 0xff, 0x0000000, 0x0000000, 0, "reset" },
	[25] = { 0x02, 0x0000011, 0x0000020, 0, "program load" },
	[26] = { 0x32, 0x0000011, 0x0000020, 0, "program load x4" },
	[27] = { 0x36, 0x0000011, 0x0000040, 0, "individual block lock" },
	[28] = { 0x39, 0x0000011, 0x0000040, 0, "individual block lock" },
	[29] = { 0x7e, 0x0000010, 0x0000040, 0, "globalblock lock" },
	[30] = { 0x98, 0x0000010, 0x0000040, 0, "global block ulock" },
	[31] = { 0xc2, 0x0000010, 0x0000000, 0, "spi nand chip selected" },
	[32] = { 0x7c, 0x0000006, 0x0001000, 1, "mxic get ecc status" },
	[33] = { 0x00, 0x0000000, 0x0000000, 0, "(empty)" },
};

/*
 * Vendor flash name table, recovered verbatim from the stock U-Boot
 * (mtd1_bootloader.bin, table at file offset 0x528bc, 39 entries of
 * 28 bytes: {name*, device_id, page, MiB, erase, 0x11d, manu_id}).
 * U-Boot prints "Manu ID: 0x%02x, Chip ID: 0x%02x (<name>)" from this
 * table; the (manu, device) pair is required since DID 0x71 exists for
 * both Dosilicon (0xe5) and Zetta (0xba).
 */
struct spifc_flash_id {
	u8	manu_id;
	u8	dev_id;
	const char *name;
};

static const struct spifc_flash_id spifc_flash_ids[] = {
	{ 0xc8, 0xd1, "GD5F1GQ4UBYIG 128MiB 3,3V" },
	{ 0xc8, 0xd2, "GD5F2GQ4UBYIGR 256MiB 3,3V" },
	{ 0xc8, 0xc1, "GD5F1GQ4R1G 128MiB 3,3V" },
	{ 0xc8, 0xc2, "GD5F2GQ4R2G 256MiB 3,3V" },
	{ 0xc8, 0x21, "F50L1G41A (2Y) 128MiB 3,3V" },
	{ 0xef, 0xaa, "W25N01GVZEIR WINBOND 128MiB 3,3V" },
	{ 0xef, 0xab, "W25N02GVZEIR WINBOND 256MiB 3,3V" },
	{ 0x98, 0xcb, "TC58CVG1S3HRAIG 256MiB 3,3V" },
	{ 0x98, 0xc2, "TC58CVG0S3HRAIG 128MiB 3,3V" },
	{ 0xe5, 0x72, "DS35Q2GA 256MiB 3,3V" },
	{ 0xe5, 0x71, "DS35Q1GA 128MiB 3,3V" },
	{ 0xe5, 0x21, "DS35M1GA 128MiB 1,8V" },
	{ 0xa1, 0xe1, "PN26G01A 128MiB 3,3V" },
	{ 0xa1, 0xe2, "PN26G02A 256MiB 3,3V" },
	{ 0x0b, 0xe2, "XT26G02A 256MiB 3,3V" },
	{ 0x0b, 0xe1, "XT26G01A 128MiB 3,3V" },
	{ 0xba, 0x71, "ZD35Q1GAIBR 128MiB 3,3V" },
	{ 0xba, 0x72, "ZD35Q2GA-1B 256MiB 3,3V" },
	{ 0xa1, 0xd1, "FM25G01B_1G 128MiB 3,3V" },
	{ 0xa1, 0xd2, "FM25G02B_2G 256MiB 3,3V" },
	{ 0xa1, 0xe4, "FM25S1GA 128MiB 3,3V" },
	{ 0xc2, 0x12, "MX35LF1GE4AB 128MiB 3,3V" },
	{ 0xc2, 0x22, "MX35LF2GE4AB 256MiB 3,3V" },
	{ 0x2c, 0x14, "MT29F1G01ABAFDWB 128MiB 3,3V" },
	{ 0x2c, 0x24, "MT29F2G01ABAGDWB 256MiB 3,3V" },
	{ 0xc8, 0x01, "F50L1G41LB 128MiB 3,3V" },
	{ 0xc8, 0x0a, "F50L2G41LB 256MiB 3,3V" },
	{ 0xd5, 0x11, "EM73C044SNB 128MiB 3,3V" },
	{ 0xd5, 0x12, "EM73D044SNA-G 256MiB 3,3V" },
	{ 0xd5, 0x10, "EM73D044SNF-G 256MiB 3,3V" },
	{ 0xc9, 0x21, "HYF1GQ4UDACAE 128MiB 3,3V" },
	{ 0xc9, 0x22, "HYF2GQ4UDACAE 256MiB 3,3V" },
	{ 0xc9, 0x51, "HYF1GQ4UAACAE 128MiB 3,3V" },
	{ 0xc9, 0x52, "HYF2GQ4UAACAE 256MiB 3,3V" },
	{ 0xc9, 0xa1, "HYF1GQ4UPACAE 128MiB 3,3V" },
	{ 0xc9, 0xd4, "HYF4GQ4UAACBE 512MiB 3,3V" },
	{ 0xcd, 0xa1, "FS35ND01G-D1F1QWHI100 128MiB 3,3V" },
	{ 0xcd, 0xea, "FS35ND01G-S1Y2QWFI100 128MiB 3,3V" },
	{ 0xcd, 0xa2, "FS35ND02G-S2F1QWFI000 256MiB 3,3V" },
};

static const char *spifc_match_flash_name(const u8 *id)
{
	int i;

	/* id[] is the 5-byte wrapped 9Fh read: [mid did mid did mid];
	 * U-Boot matches on the 2-byte (mid, did) pair. */
	for (i = 0; i < ARRAY_SIZE(spifc_flash_ids); i++)
		if (spifc_flash_ids[i].manu_id == id[0] &&
		    spifc_flash_ids[i].dev_id == id[1])
			return spifc_flash_ids[i].name;
	return NULL;
}

struct spifc {
	void __iomem *regs;
	struct mtd_info mtd;
	spinlock_t lock;
};

static inline struct spifc *mtd_to_spifc(struct mtd_info *mtd)
{
	return container_of(mtd, struct spifc, mtd);
}

/*
 * Load one command-table entry into the controller registers. Mirrors
 * spifc_config_cmd: reg14, reg18 (len-1), reg1c (addr), reg20 (opcode),
 * reg10 (control fields). The address is forced to 0 for the opcodes the
 * stock driver special-cases.
 */
static void spifc_config_cmd(struct spifc *spifc, unsigned int idx, u32 addr,
			     u32 len)
{
	const struct spifc_cmd *c = &spifc_cmds[idx];
	void __iomem *r = spifc->regs;
	u32 xfer = c->len ? c->len : len;

	writel(c->reg14, r + SPIFC_REG_CMD14);
	writel(xfer ? xfer - 1 : 0, r + SPIFC_REG_LEN18);

	switch (c->opcode) {
	case 0x04:
	case 0x05:
	case 0x06:
	case 0x60:
	case 0x90:
	case 0x9f:
		writel(0, r + SPIFC_REG_ADDR1C);
		break;
	default:
		writel(addr, r + SPIFC_REG_ADDR1C);
		break;
	}

	writel(c->opcode, r + SPIFC_REG_OPCODE);
	writel(c->reg10, r + SPIFC_REG_CMD10);
}

static void spifc_clear_int(struct spifc *spifc)
{
	void __iomem *r = spifc->regs;

	if (!(readl(r + SPIFC_REG_DONE) & SPIFC_DONE_ERR))
		writel(0x3f, r + SPIFC_REG_INTCLEAR);
}

static int spifc_wait_done(struct spifc *spifc)
{
	void __iomem *r = spifc->regs;
	unsigned long end = jiffies + SPIFC_TIMEOUT_JIF;

	while (!(readl(r + SPIFC_REG_DONE) & SPIFC_DONE_OK)) {
		if (time_after(jiffies, end))
			return -ETIMEDOUT;
		cpu_relax();
	}

	if (readl(r + SPIFC_REG_DONE) & SPIFC_DONE_ERR)
		return -EIO;

	return 0;
}

/* Arm the controller and pulse the trigger. */
static void spifc_trigger(struct spifc *spifc)
{
	void __iomem *r = spifc->regs;
	u32 v;

	writel(readl(r + SPIFC_REG_CONFIG2) | SPIFC_CONFIG2_MASK,
	       r + SPIFC_REG_CONFIG2);
	spifc_clear_int(spifc);
	v = readl(r + SPIFC_REG_TRIGGER);
	writel(v | 1, r + SPIFC_REG_TRIGGER);
}

static void spifc_kick(struct spifc *spifc)
{
	void __iomem *r = spifc->regs;

	if (!(readl(r + SPIFC_REG_STATUS) & BIT(1)))
		writel(readl(r + SPIFC_REG_STATUS) | BIT(0),
		       r + SPIFC_REG_STATUS);
}

/* Drain len bytes from the read FIFO. Reading 0x38 pops one word. */
static int spifc_read_fifo(struct spifc *spifc, void *out, u32 len)
{
	void __iomem *r = spifc->regs;
	unsigned long end = jiffies + SPIFC_TIMEOUT_JIF;
	u8 *dst = out;
	u32 words = len >> 2;
	u32 i = 0;

	while (i < words) {
		if (readl(r + SPIFC_REG_FIFOSTAT) & SPIFC_FIFO_RD_READY) {
			u32 word = readl(r + SPIFC_REG_FIFODATA);

			memcpy(dst, &word, 4);
			dst += 4;
			i++;
		} else if (time_after(jiffies, end)) {
			return -ETIMEDOUT;
		}
	}

	if (len & 3) {
		u32 word = 0;

		end = jiffies + SPIFC_TIMEOUT_JIF;
		while (!(readl(r + SPIFC_REG_FIFOSTAT) & SPIFC_FIFO_RD_READY))
			if (time_after(jiffies, end))
				return -ETIMEDOUT;
		word = readl(r + SPIFC_REG_FIFODATA);
		memcpy(dst, &word, len & 3);
	}

	return 0;
}

/*
 * Run a command that produces no host data (page-to-cache, erase, reset,
 * set-feature). Caller holds the lock.
 */
static int spifc_exec(struct spifc *spifc, unsigned int idx, u32 addr, u32 len)
{
	spifc_kick(spifc);
	spifc_config_cmd(spifc, idx, addr, len);
	spifc_trigger(spifc);

	return spifc_wait_done(spifc);
}

/* get-feature: returns the single feature byte, or a negative errno. */
static int spifc_get_feature(struct spifc *spifc, u32 addr)
{
	u8 val;
	int ret;

	spifc_kick(spifc);
	spifc_config_cmd(spifc, CMD_GET_FEATURE, addr, 1);
	spifc_trigger(spifc);

	ret = spifc_read_fifo(spifc, &val, 1);
	if (ret)
		return ret;

	ret = spifc_wait_done(spifc);
	if (ret)
		return ret;

	return val;
}

static int spifc_set_feature(struct spifc *spifc, u32 addr, u8 val)
{
	void __iomem *r = spifc->regs;
	unsigned long end;
	int ret;

	spifc_kick(spifc);
	spifc_config_cmd(spifc, CMD_SET_FEATURE, addr, 1);
	writel(readl(r + SPIFC_REG_CONFIG2) | SPIFC_CONFIG2_MASK,
	       r + SPIFC_REG_CONFIG2);
	spifc_clear_int(spifc);

	/* Push the one-byte value, then trigger (stock order). */
	writel(readl(r + SPIFC_REG_TRIGGER) | 1, r + SPIFC_REG_TRIGGER);

	end = jiffies + SPIFC_TIMEOUT_JIF;
	while (!(readl(r + SPIFC_REG_FIFOSTAT) & SPIFC_FIFO_WR_READY))
		if (time_after(jiffies, end))
			return -ETIMEDOUT;
	writel(val, r + SPIFC_REG_FIFODATA);

	ret = spifc_wait_done(spifc);

	return ret;
}

/*
 * Poll the status feature until the busy bit (bit 0) clears. When
 * status_out is non-NULL it receives the final status byte so callers can
 * inspect the ECC result field (bits [5:4]), exactly as the stock
 * spifcnand_nand_wait() does before reporting success.
 */
static int spifc_nand_wait_ex(struct spifc *spifc, u8 *status_out)
{
	unsigned long end = jiffies + SPIFC_TIMEOUT_JIF;
	int ret;

	for (;;) {
		ret = spifc_get_feature(spifc, FEAT_STATUS);
		if (ret < 0)
			return ret;
		if (!(ret & BIT(0))) {
			if (status_out)
				*status_out = ret;
			return 0;
		}
		if (time_after(jiffies, end))
			return -ETIMEDOUT;
		cpu_relax();
	}
}

static int spifc_nand_wait(struct spifc *spifc)
{
	return spifc_nand_wait_ex(spifc, NULL);
}

/*
 * Address encoding for the cache commands (page-to-cache, program load,
 * program execute, erase). The ZD35Q1GAIBR wants the 16-bit page number in
 * the low half of the 24-bit address field. This is the one value to
 * revisit if a full-partition SHA does not match the canonical dump (see
 * plan §7.1c calibration note).
 */
static u32 spifc_page_addr(unsigned int page)
{
	return page & 0xffff;
}

/*
 * Push up to len bytes into the write FIFO, returning how many were
 * accepted. Non-blocking: it stops as soon as the WR-ready field (0x34 bit
 * 0x1f0000) clears, which is how the stock spifc_write_fifo() lets
 * nand_page_load() keep feeding the FIFO while the controller drains it
 * after the trigger. The tail (len & 3) pushes one final whole word when
 * the FIFO still has room, exactly like the stock tail path; the caller's
 * buffer must therefore be part of a 4-byte-sized allocation.
 */
static u32 spifc_write_fifo(struct spifc *spifc, const u8 *buf, u32 len)
{
	void __iomem *r = spifc->regs;
	u32 pushed = 0;
	u32 words = len >> 2;

	while (pushed < words) {
		u32 word;

		if (!(readl(r + SPIFC_REG_FIFOSTAT) & SPIFC_FIFO_WR_READY))
			return pushed << 2;
		memcpy(&word, buf + (pushed << 2), 4);
		writel(word, r + SPIFC_REG_FIFODATA);
		pushed++;
	}

	if (len & 3) {
		u32 word = 0;

		if (!(readl(r + SPIFC_REG_FIFOSTAT) & SPIFC_FIFO_WR_READY))
			return pushed << 2;
		memcpy(&word, buf + (pushed << 2), len & 3);
		writel(word, r + SPIFC_REG_FIFODATA);
		return (pushed << 2) + (len & 3);
	}

	return pushed << 2;
}

/*
 * Load one page of main data into the device cache with the 0x02
 * "program load" command (table idx 25, reg14 = 0x20).
 *
 * page_addr is the command's address field, which on this NAND family is
 * a COLUMN address: the stock spi_flash_cmd_write() always passes 0 here
 * and puts the page number only in the 0x10 program-execute command.
 *
 * The write FIFO is shallow (hardware-measured: 64 bytes), so the whole
 * page cannot be queued up front. Push whatever the FIFO accepts before
 * the trigger (a few words of slack against the drain), then start the
 * drain and keep feeding it until the page is in, mirroring the stock
 * trigger-then-refill order.
 */
static int spifc_nand_page_load(struct spifc *spifc, u32 page_addr,
				const u8 *buf, u32 len)
{
	void __iomem *r = spifc->regs;
	u32 done = 0;
	unsigned long end;
	int stall = 0;

	spifc_kick(spifc);
	spifc_config_cmd(spifc, CMD_PROGRAM_LOAD, page_addr, len);
	writel(readl(r + SPIFC_REG_CONFIG2) | SPIFC_CONFIG2_MASK,
	       r + SPIFC_REG_CONFIG2);
	spifc_clear_int(spifc);

	/* Fill the FIFO before arming the drain; stop once it is full
	 * (bounded no-progress spins, the FIFO is far shallower than a
	 * page so this exits quickly on real hardware).
	 */
	while (done < len && stall < 64) {
		u32 n = spifc_write_fifo(spifc, buf + done, len - done);

		if (n) {
			done += n;
			stall = 0;
			continue;
		}
		stall++;
		cpu_relax();
	}

	writel(readl(r + SPIFC_REG_TRIGGER) | 1, r + SPIFC_REG_TRIGGER);

	end = jiffies + SPIFC_TIMEOUT_JIF;
	while (done < len) {
		u32 n;

		n = spifc_write_fifo(spifc, buf + done, len - done);
		done += n;
		if (!n && time_after(jiffies, end))
			return -ETIMEDOUT;
	}

	return spifc_wait_done(spifc);
}

/*
 * Program one page: WEL, 0x02 load, WEL, 0x10 execute, then wait for the
 * device busy bit and check the ECC field (status bits [5:4]) exactly as
 * the stock spi_flash_cmd_write()/spifcnand_nand_wait() pair does for the
 * ZD35Q1GA (0xba71): field 01 or 11 is an uncorrectable error.
 */
static int spifc_program_page(struct spifc *spifc, unsigned int page,
			      const u8 *buf)
{
	u8 status;
	int ret;

	/*
	 * Stock spi_flash_cmd_write() (c025e40c) calls nand_page_load with
	 * r2 = #0: the 0x02 program-load command ALWAYS gets address 0 (it
	 * is a column address on this chip, and the column is 0). The page
	 * number goes only into the 0x10 program-execute. Passing the page
	 * number here made the chip start loading at column=page, which is
	 * exactly the "FF*(page+1024) + data" corruption head seen in the
	 * run-8/9 marker tests (page 0 -> 1024, page 576 -> 1600).
	 */
	ret = spifc_exec(spifc, CMD_WRITE_ENABLE, 0, 0);
	if (ret)
		return ret;

	ret = spifc_nand_page_load(spifc, 0, buf, SPIFC_PAGE_DATA);
	if (ret)
		return ret;

	ret = spifc_exec(spifc, CMD_WRITE_ENABLE, 0, 0);
	if (ret)
		return ret;

	ret = spifc_exec(spifc, CMD_PROGRAM_EXEC, spifc_page_addr(page), 0);
	if (ret)
		return ret;

	ret = spifc_nand_wait_ex(spifc, &status);
	if (ret)
		return ret;

	/*
	 * Stock spifcnand_nand_wait() for device id 0x71/0x72 (ZD35Q1GA):
	 * the ECC field (status bits [5:4]) equal to 01 is the failure
	 * case; every other encoding is reported as success.
	 */
	if (((status >> 4) & 3) == 1) {
		pr_err("spifc: page %u ECC failure, status %02x\n",
		       page, status);
		return -EUCLEAN;
	}

	/* ZD35Q1GA datasheet: bit 2 of 0xc0 is P_FAIL/EFAIL. */
	if (status & BIT(2)) {
		pr_err("spifc: page %u program failure (P_FAIL), status %02x\n",
		       page, status);
		return -EIO;
	}

	return 0;
}

/*
 * Erase one block: WEL then 0xd8 with the address of the block's first
 * page (stock spi_sector_erase), then wait for busy.
 */
static int spifc_erase_block(struct spifc *spifc, unsigned int block)
{
	u32 first_page = (block * SPIFC_ERASE_SIZE) / SPIFC_PAGE_DATA;
	int ret;

	ret = spifc_exec(spifc, CMD_WRITE_ENABLE, 0, 0);
	if (ret)
		return ret;

	ret = spifc_exec(spifc, CMD_SECTOR_ERASE, first_page, 0);
	if (ret)
		return ret;

	return spifc_nand_wait(spifc);
}

static int spifc_mtd_write(struct mtd_info *mtd, loff_t to, size_t len,
			   size_t *retlen, const u_char *buf)
{
	struct spifc *spifc = mtd_to_spifc(mtd);
	u8 *page;
	int ret = 0;

	*retlen = 0;
	if (!len)
		return 0;
	if (to + len > mtd->size)
		return -EINVAL;
	/*
	 * The cache-program path has no partial-page read-modify-write
	 * (OOB is not exposed), so require page-aligned writes. flashcp
	 * and nandwrite both satisfy this.
	 */
	if (to % SPIFC_PAGE_DATA || len % SPIFC_PAGE_DATA)
		return -EINVAL;

	page = kmalloc(SPIFC_PAGE_DATA, GFP_KERNEL);
	if (!page)
		return -ENOMEM;

	spin_lock_bh(&spifc->lock);

	while (len) {
		unsigned int pnum = div_u64(to, SPIFC_PAGE_DATA);

		memcpy(page, buf, SPIFC_PAGE_DATA);

		ret = spifc_program_page(spifc, pnum, page);
		if (ret)
			break;

		buf += SPIFC_PAGE_DATA;
		to += SPIFC_PAGE_DATA;
		len -= SPIFC_PAGE_DATA;
		*retlen += SPIFC_PAGE_DATA;
	}

	spin_unlock_bh(&spifc->lock);
	kfree(page);

	return ret;
}

static int spifc_mtd_erase(struct mtd_info *mtd, struct erase_info *instr)
{
	struct spifc *spifc = mtd_to_spifc(mtd);
	size_t remaining = instr->len;	/* size_t: no 64-bit modulo needed */
	u32 rem;
	u64 addr = instr->addr;
	int ret = 0;

	if (addr + remaining > mtd->size)
		return -EINVAL;
	/* div_u64_rem: plain % on a u64 would need __aeabi_uldivmod (ARM). */
	(void)div_u64_rem(addr, mtd->erasesize, &rem);
	if (rem || remaining % mtd->erasesize)
		return -EINVAL;

	spin_lock_bh(&spifc->lock);

	while (remaining) {
		unsigned int block = div_u64_rem(addr, mtd->erasesize, &rem);

		ret = spifc_erase_block(spifc, block);
		if (ret)
			break;

		addr += mtd->erasesize;
		remaining -= mtd->erasesize;
	}

	spin_unlock_bh(&spifc->lock);

	return ret;
}

/*
 * Enable the on-die ECC engine (feature 0xb0 bit 4), mirroring the stock
 * ECC_ctrl_for_read(). Kept as read-modify-write of the power-on default
 * (hardware-verified by the run-7 read gate) rather than the stock
 * _flash_init_cfg() hard-write of 0x18.
 */
static int spifc_ecc_enable(struct spifc *spifc)
{
	int cur;
	int ret;

	cur = spifc_get_feature(spifc, FEAT_ECC_READ);
	if (cur < 0)
		return cur;

	ret = spifc_set_feature(spifc, FEAT_ECC_READ, cur | FEAT_ECC_ENABLE);
	if (ret)
		return ret;

	return spifc_nand_wait(spifc);
}

/*
 * Clear the block-protection feature (0xa0 = 0) exactly as the stock
 * _flash_init_cfg() does before any program/erase is possible. The chip is
 * factory-unprotected, so this is normally a no-op, but it makes the
 * driver independent of whatever a previous boot left behind.
 */
static int spifc_unlock(struct spifc *spifc)
{
	int ret;

	ret = spifc_set_feature(spifc, FEAT_BLOCK_PROTECT, 0);
	if (ret)
		return ret;

	return spifc_nand_wait(spifc);
}

/* Read one full page (2048 bytes) of main data into buf. Caller holds lock. */
static int spifc_read_page_data(struct spifc *spifc, unsigned int page,
				u8 *buf)
{
	void __iomem *r = spifc->regs;
	int ret;

	/* array -> cache */
	ret = spifc_exec(spifc, CMD_PAGE_READ_CACHE, spifc_page_addr(page), 0);
	if (ret)
		return ret;

	ret = spifc_nand_wait(spifc);
	if (ret)
		return ret;

	/* cache -> host, dual-output, column 0 */
	spifc_kick(spifc);
	spifc_config_cmd(spifc, CMD_DUAL_OUTPUT_READ, 0, SPIFC_PAGE_DATA);
	writel((readl(r + SPIFC_REG_CMD14) & ~5u) | 4u, r + SPIFC_REG_CMD14);
	spifc_trigger(spifc);

	ret = spifc_read_fifo(spifc, buf, SPIFC_PAGE_DATA);
	if (ret)
		return ret;

	return spifc_wait_done(spifc);
}

static int spifc_mtd_read(struct mtd_info *mtd, loff_t from, size_t len,
			  size_t *retlen, u_char *buf)
{
	struct spifc *spifc = mtd_to_spifc(mtd);
	u8 *page;
	int ret = 0;

	*retlen = 0;
	if (!len)
		return 0;
	if (from + len > mtd->size)
		return -EINVAL;

	page = kmalloc(SPIFC_PAGE_DATA, GFP_KERNEL);
	if (!page)
		return -ENOMEM;

	/*
	 * spin_lock_bh (not irqsave) mirrors the stock _raw_spin_lock_bh:
	 * the timer interrupt must stay enabled so jiffies advance and the
	 * busy-wait timeouts below can actually fire.
	 */
	spin_lock_bh(&spifc->lock);

	while (len) {
		unsigned int pnum = div_u64(from, SPIFC_PAGE_DATA);
		unsigned int off = from % SPIFC_PAGE_DATA;
		size_t chunk = min_t(size_t, SPIFC_PAGE_DATA - off, len);

		ret = spifc_read_page_data(spifc, pnum, page);
		if (ret)
			break;

		memcpy(buf, page + off, chunk);
		buf += chunk;
		from += chunk;
		len -= chunk;
		*retlen += chunk;
	}

	spin_unlock_bh(&spifc->lock);
	kfree(page);

	return ret;
}

static const struct of_device_id spifc_of_match[] = {
	{ .compatible = "zte,ZX279127-spifc" },
	{ /* sentinel */ }
};
MODULE_DEVICE_TABLE(of, spifc_of_match);

static int spifc_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	struct spifc *spifc;
	struct mtd_info *mtd;
	u8 id[5];
	int ret;

	BUILD_BUG_ON(ARRAY_SIZE(spifc_cmds) != 34);

	spifc = devm_kzalloc(dev, sizeof(*spifc), GFP_KERNEL);
	if (!spifc)
		return -ENOMEM;

	spin_lock_init(&spifc->lock);

	spifc->regs = devm_platform_ioremap_resource(pdev, 0);
	if (IS_ERR(spifc->regs))
		return PTR_ERR(spifc->regs);

	mtd = &spifc->mtd;
	mtd->name = "zx279128-spifc";
	mtd->dev.parent = dev;
	mtd->owner = THIS_MODULE;
	/* mtdchar_open() rejects a device whose type is MTD_ABSENT (0) with
	 * -ENODEV, and the zero-initialised struct defaults to exactly that,
	 * so every /dev/mtdN open failed. This is a 2048+64 SLC SPI-NAND.
	 */
	mtd->type = MTD_NANDFLASH;
	mtd->size = 0x08000000;	/* ZD35Q1GAIBR: 1 Gbit = 128 MiB */
	mtd->writesize = SPIFC_PAGE_DATA;
	mtd->erasesize = SPIFC_ERASE_SIZE;
	mtd->oobsize = SPIFC_PAGE_OOB;
	mtd->oobavail = SPIFC_PAGE_OOB;
	mtd->_read = spifc_mtd_read;
	mtd->_write = spifc_mtd_write;
	mtd->_erase = spifc_mtd_erase;
	/*
	 * add_mtd_device() rejects a device implementing BOTH ->_read and
	 * ->_read_oob (WARN_ON -> -EINVAL), so the stub above must not be
	 * hooked up: mtd_read_oob() already falls back to ->_read through
	 * mtd_read_oob_std() when ->_read_oob is NULL, so plain reads keep
	 * working and OOB simply stays unsupported for this gate.
	 */
	mtd->flags = MTD_WRITEABLE;

	platform_set_drvdata(pdev, spifc);

	/* Reset the chip, then bring up ECC and identify it. */
	spin_lock_bh(&spifc->lock);
	ret = spifc_exec(spifc, CMD_RESET, 0, 0);
	if (ret) {
		spin_unlock_bh(&spifc->lock);
		dev_err(dev, "reset failed: %d\n", ret);
		return ret;
	}

	ret = spifc_ecc_enable(spifc);
	if (ret) {
		spin_unlock_bh(&spifc->lock);
		dev_err(dev, "ECC enable failed: %d\n", ret);
		return ret;
	}

	ret = spifc_unlock(spifc);
	if (ret) {
		spin_unlock_bh(&spifc->lock);
		dev_err(dev, "unlock failed: %d\n", ret);
		return ret;
	}

	/* JEDEC ID (idx 14, 5 bytes). */
	spifc_kick(spifc);
	spifc_config_cmd(spifc, CMD_READ_JEDEC_ID, 0, 0);
	spifc_trigger(spifc);
	ret = spifc_read_fifo(spifc, id, 5);
	if (!ret)
		ret = spifc_wait_done(spifc);
	spin_unlock_bh(&spifc->lock);

	if (ret) {
		dev_err(dev, "JEDEC ID read failed: %d\n", ret);
		return ret;
	}

	{
		const char *name = spifc_match_flash_name(id);

		if (name)
			dev_info(dev, "SPI-NAND JEDEC ID: %*phN (%s)\n", 5, id,
				 name);
		else
			dev_info(dev, "SPI-NAND JEDEC ID: %*phN (unknown)\n", 5,
				 id);
	}

	/* The ofpart parser locates the "partitions" child through
	 * mtd_get_of_node(mtd); without this the fixed-partitions node in
	 * zxhn-e1600.dts is invisible and no partitions are created.
	 */
	mtd_set_of_node(mtd, dev->of_node);

	ret = mtd_device_parse_register(mtd, NULL, NULL, NULL, 0);
	if (ret) {
		dev_err(dev, "MTD registration failed: %d\n", ret);
		return ret;
	}

	return 0;
}

static void spifc_remove(struct platform_device *pdev)
{
	struct spifc *spifc = platform_get_drvdata(pdev);

	mtd_device_unregister(&spifc->mtd);
}

static struct platform_driver spifc_driver = {
	.probe = spifc_probe,
	.remove = spifc_remove,
	.driver = {
		.name = "zx279128-spifc",
		.of_match_table = spifc_of_match,
	},
};
module_platform_driver(spifc_driver);

MODULE_DESCRIPTION("ZTE ZX27912x SPIFC SPI-NAND controller");
MODULE_LICENSE("GPL");

// SPDX-License-Identifier: GPL-2.0
/*
 * sif_port.c - portable reconstruction of the EcoNet/Airoha EN7528 "SIF"
 *              (Serial InterFace == hardware I2C / "SIRM") read/write path.
 *              For a Linux 6.18 MIPS (mipsel / MIPS32r2) kernel module.
 *
 * *** INSTRUMENTED DEBUG BUILD (rev 2) ***
 *   On-device test: SIF_X_Read returns 0 because a sif_exec() hits -ETIMEDOUT.
 *   This build adds "sifdbg:" printk() tracing to find out WHICH wait fails and
 *   with what register values, plus a one-shot register dump at init. See the
 *   ROOT-CAUSE block below and sif_port_notes.md §8.
 *
 * Signatures unchanged (drop-in for the GPL stubs):
 *   UINT16 SIF_X_Read /SIF_X_Write(UINT8 ch, UINT16 clkDiv, UINT8 devAddr,
 *          UINT8 wordAddrNum, UINT32 wordAddr, UINT8 *pBuf, UINT16 byteCnt);
 * Returns byteCnt on success, 0 on error.
 *
 * ===========================================================================
 * ROOT-CAUSE ANALYSIS (rev 4 - from a full disasm of sif.ko, with relocs):
 *
 *   The completion primitive is _SIF_WaitHwDone @0x145c. Decoded, it does:
 *     1. BEST-EFFORT wait STAT(0x60).bit0 == 1   (udelay(1), tmo 0x3fc00).
 *        *** On timeout it does NOT abort *** (@0x1598 only logs, then @0x15c4
 *        falls through to step 2). STAT-ready is advisory, not a precondition.
 *     2. write GO(0x64).bit0 = 1   (reg_field_write @0x12bc, RMW). GO is the
 *        execute strobe; it reads back ~0.
 *     3. wait CMD(0x44).bit0 == 0  (tmo 0xffff). THIS is the real completion
 *        gate; only this timeout is fatal (returns -6 @0x1570).
 *
 *   TWO bugs in earlier revs (NOT clock-gating - the block answers):
 *   (A) Treating STAT(0x60).bit0==1 as a hard precondition (rev2) or dropping
 *       the GO kick entirely (rev3). Both wrong: stock proceeds past a STAT
 *       timeout and the completion is CMD.bit0->0, but GO is mandatory.
 *   (B) The "run-arm" @0x1350 (lazily done by stock on the FIRST transaction of
 *       each channel) writes GO(0x64).b0=1 then **CFG2(0x5c).b0=1**. INIT @0x2b34
 *       leaves CFG2.b0=0; the first xfer flips it to 1. The OpenWrt port never
 *       did this, so the controller stayed un-armed and STAT(0x60) read 0.
 *
 *   Both are fixed below. Decisive on-device signals (see §8):
 *     * "exec OK statRdy=1" + sane SFF-8472 bytes => fully fixed; CFG2.b0 arm
 *       was the cause of STAT staying 0.
 *     * "exec OK statRdy=0" + correct bytes => STAT 0x60 is a phantom on this
 *       rev; the non-fatal STAT wait (bug A fix) is what saves us.
 * ===========================================================================
 */

#include <linux/kernel.h>
#include <linux/io.h>
#include <linux/delay.h>
#include <linux/mutex.h>
#include <linux/types.h>

/* ----- KSEG1 direct register access (uncached); == regRead32/regWrite32 ----- */
#define SIF_RD(a)        (*(volatile u32 *)(uintptr_t)(a))
#define SIF_WR(a, v)     (*(volatile u32 *)(uintptr_t)(a) = (u32)(v))
static inline void SIF_RMW(u32 a, u32 val, u32 mask)
{
	SIF_WR(a, (SIF_RD(a) & ~mask) | (val & mask));
}

/* ----- controller base per channel (from .data sifBusMap[]) ----- */
#define SIF_NCH          2
static const u32 sif_base[SIF_NCH] = { 0xbfbf8000u, 0xbfbf8100u };

/* SoC config/version "gate" regs the stock driver READS (never writes) */
#define SOC_SYS_CFG      0xbfb00284u   /* bit9 = use-spinlock / 1004K-MP gate */
#define SOC_CHIP_VER     0xbfb00064u   /* top16 == 0x000a selects "V2" SIF    */

/* ----- register map (offset from base; all confirmed via reg_field_*) ----- */
#define SIF_CFG          0x40   /* clkdiv[27:16], enable bit1, line bits 2/3 */
#define SIF_CMD          0x44   /* command/status; busy = bit0, ACK = bit16  */
#define SIF_DATA0        0x50   /* bytes 0..3 little-endian (TX & RX)        */
#define SIF_DATA1        0x54   /* bytes 4..7 little-endian (TX & RX)        */
#define SIF_CFG2         0x5c   /* aux config; bit0 = run-arm/enable (@0x1350)*/
#define SIF_STAT         0x60   /* bit0 = controller idle (advisory only)    */
#define SIF_GO           0x64   /* bit0 = execute strobe (write-1, reads ~0) */

/* CMD (0x44) bit fields (proven @0x16c4/0x1628/0x1760/0x1a3c/0x24ac) */
#define CMD_START        0x0001u
#define CMD_ACK          0x0010u   /* "ACK & continue"; cleared on last RX byte */
#define CMD_WR           0x0020u
#define CMD_RD           0x0040u
#define CMD_CNT(n)       ((((n) - 1) & 7) << 8)
#define CMD_MASK_HDR     0x0071u   /* start/stop ops */
#define CMD_MASK_XFER    0x0771u   /* byte transfer ops */
#define CMD_RESP(v)      (((v) >> 16) & 1)

#define OP_START         0x11u   /* getStartBit @0x16c4: CMD=0x11 mask 0x71 */
#define OP_STOP          0x31u   /* getStopBit  @0x1628: CMD=0x31 mask 0x71 */

/* CFG (0x40) clkdiv program (@0x1964): ((div&0xfff)<<16)|0x320, mask 0x8fffff21 */
#define CFG_CLKDIV(div)  ((((u32)(div) & 0xfffu) << 16) | 0x320u)
#define CFG_CLKDIV_MASK  0x8fffff21u

#define SIF_RDY_TMO      0x3fc00
#define SIF_BUSY_TMO     0xffff
#define SIF_FIFO         8

static DEFINE_MUTEX(sif_lock);

/* ---- debug throttling: log only the first few execs / reads ---- */
#define SIF_DBG_MAX      8
static int sif_dbg_exec_n;
static int sif_dbg_read_n;

/*
 * Optional platform clock/reset/pinmux de-gate.  *** OFF by default. ***
 * sif.ko itself contains NO such write (U-Boot did it on stock), so the exact
 * EN7528 SYSCTL/CGU register is NOT recoverable from the .ko. Do NOT enable
 * this with a guessed address - a wrong write to a CGU/reset register can hang
 * the SoC. Fill in the verified register(s) from the EN7528 datasheet or the
 * OpenWrt EN7528 clk/reset driver, then build with -DSIF_PLATFORM_CLK_DEGATE.
 */
#ifdef SIF_PLATFORM_CLK_DEGATE
#ifndef SIF_CLK_REG
#error "Define SIF_CLK_REG / SIF_CLK_BIT (verified EN7528 SIF clock gate) first"
#endif
static void sif_platform_degate(void)
{
	SIF_RMW(SIF_CLK_REG, SIF_CLK_BIT, SIF_CLK_BIT);     /* ungate clock   */
#ifdef SIF_RST_REG
	SIF_RMW(SIF_RST_REG, 0, SIF_RST_BIT);               /* release reset  */
#endif
	udelay(10);
}
#else
static inline void sif_platform_degate(void) { }
#endif

/* --------------------------------------------------------------------------
 * sif_exec - faithful port of _SIF_WaitHwDone @0x145c. The caller has already
 * staged DATA/CMD (CMD holds START + WR/RD + count). This routine then:
 *   1. BEST-EFFORT wait STAT(0x60).bit0 == 1  (udelay(1), SIF_RDY_TMO).
 *      *** NON-FATAL: stock (@0x1598->0x15c4) proceeds even if it never sets. ***
 *   2. write GO(0x64).bit0 = 1  (RMW; the execute strobe).
 *   3. wait CMD(0x44).bit0 == 0  (SIF_BUSY_TMO).  THIS is the real completion
 *      gate; only this timeout is fatal (stock returns -6).
 * Instrumented: reports statRdy (did step 1 succeed) + register state.
 * -------------------------------------------------------------------------- */
static int sif_exec(u32 base, const char *tag)
{
	int t;
	int log = (sif_dbg_exec_n < SIF_DBG_MAX);
	u32 cmd0 = SIF_RD(base + SIF_CMD);
	int stat_ready;

	if (log)
		sif_dbg_exec_n++;

	/* step 1: advisory STAT-ready wait (NON-FATAL, exactly like stock) */
	for (t = SIF_RDY_TMO; t && !(SIF_RD(base + SIF_STAT) & 1); t--)
		udelay(1);
	stat_ready = (t != 0);

	/* step 2: kick the execute strobe (mandatory) */
	SIF_RMW(base + SIF_GO, 1, 1);

	/* step 3: wait for CMD busy to clear -- the real done detection */
	for (t = SIF_BUSY_TMO; t && (SIF_RD(base + SIF_CMD) & 1); t--)
		cpu_relax();
	if (!t) {
		if (log)
			printk(KERN_INFO "sifdbg: exec[%s] BUSY-TIMEOUT base=%08x "
			       "statRdy=%d CMD0=%08x CMDnow=%08x CFG=%08x "
			       "CFG2=%08x STAT=%08x GO=%08x\n",
			       tag, base, stat_ready, cmd0, SIF_RD(base + SIF_CMD),
			       SIF_RD(base + SIF_CFG), SIF_RD(base + SIF_CFG2),
			       SIF_RD(base + SIF_STAT), SIF_RD(base + SIF_GO));
		return -ETIMEDOUT;
	}

	if (log)
		printk(KERN_INFO "sifdbg: exec[%s] OK base=%08x statRdy=%d CMD0=%08x "
		       "CMDend=%08x DATA0=%08x DATA1=%08x\n",
		       tag, base, stat_ready, cmd0, SIF_RD(base + SIF_CMD),
		       SIF_RD(base + SIF_DATA0), SIF_RD(base + SIF_DATA1));
	return 0;
}

/* I2C (repeated) START. getStartBit @0x16c4: CMD=0x11 mask 0x71, then exec. */
static int sif_start(u32 base, const char *tag)
{
	SIF_RMW(base + SIF_CMD, OP_START, CMD_MASK_HDR);
	return sif_exec(base, tag);
}

/* I2C STOP. getStopBit @0x1628: CMD=0x31 mask 0x71, then exec. */
static int sif_stop(u32 base, const char *tag)
{
	SIF_RMW(base + SIF_CMD, OP_STOP, CMD_MASK_HDR);
	return sif_exec(base, tag);
}

/* send one byte (device-address byte). cmd = WR|START, len 1. @0x1760 */
static int sif_put_byte(u32 base, u8 b, const char *tag)
{
	SIF_WR(base + SIF_DATA0, b);
	SIF_RMW(base + SIF_CMD, CMD_WR | CMD_START | CMD_CNT(1), CMD_MASK_XFER);
	if (sif_exec(base, tag))
		return -ETIMEDOUT;
	return CMD_RESP(SIF_RD(base + SIF_CMD));
}

/* write n payload/subaddr bytes, 8/FIFO, little-endian into DATA0/DATA1. @0x1a3c */
static int sif_put_bytes(u32 base, const u8 *p, int n, const char *tag)
{
	int done = 0;

	while (n > 0) {
		int cnt = min(n, SIF_FIFO);
		u32 w0 = 0, w1 = 0;
		int i;

		for (i = 0; i < cnt && i < 4; i++)
			w0 |= (u32)p[i] << (8 * i);
		for (; i < cnt; i++)
			w1 |= (u32)p[i] << (8 * (i - 4));

		SIF_WR(base + SIF_DATA0, w0);
		SIF_WR(base + SIF_DATA1, w1);
		SIF_RMW(base + SIF_CMD,
			CMD_WR | CMD_START | CMD_CNT(cnt), CMD_MASK_XFER);
		if (sif_exec(base, tag))
			return -ETIMEDOUT;

		p += cnt;
		n -= cnt;
		done += cnt;
	}
	return done;
}

/* read n bytes, 8/FIFO, little-endian out of DATA0/DATA1. @0x24ac */
static int sif_get_bytes(u32 base, u8 *buf, int n)
{
	int done = 0;

	while (n > 0) {
		int cnt = min(n, SIF_FIFO);
		int more = (n > cnt);
		u32 cmd = CMD_RD | CMD_START | CMD_CNT(cnt) | (more ? CMD_ACK : 0);
		u32 w0, w1;
		int i;

		SIF_RMW(base + SIF_CMD, cmd, CMD_MASK_XFER);
		if (sif_exec(base, "read"))
			return -ETIMEDOUT;

		w0 = SIF_RD(base + SIF_DATA0);
		w1 = SIF_RD(base + SIF_DATA1);
		for (i = 0; i < cnt && i < 4; i++)
			buf[i] = (u8)(w0 >> (8 * i));
		for (; i < cnt; i++)
			buf[i] = (u8)(w1 >> (8 * (i - 4)));

		buf += cnt;
		n -= cnt;
		done += cnt;
	}
	return done;
}

static void sif_pack_subaddr(u8 *sa, u8 wordAddrNum, u32 wordAddr)
{
	int i;

	for (i = 0; i < wordAddrNum; i++)
		sa[i] = (u8)(wordAddr >> (8 * (wordAddrNum - 1 - i)));
}

static void sif_dump(const char *where, u32 base)
{
	printk(KERN_INFO "sifdbg: %s base=%08x CFG=%08x CMD=%08x DATA0=%08x "
	       "DATA1=%08x CFG2=%08x STAT=%08x GO=%08x\n",
	       where, base,
	       SIF_RD(base + SIF_CFG),  SIF_RD(base + SIF_CMD),
	       SIF_RD(base + SIF_DATA0), SIF_RD(base + SIF_DATA1),
	       SIF_RD(base + SIF_CFG2), SIF_RD(base + SIF_STAT),
	       SIF_RD(base + SIF_GO));
}

/* full-block read-only map: which offsets in 0x40..0x7c are ALIVE on this
 * silicon (non-zero / non-deadbeef) => locates the real status/done reg. */
static void sif_dump_full(const char *where, u32 base)
{
	int o;

	for (o = 0x40; o <= 0x7c; o += 0x10)
		printk(KERN_INFO "sifdbg-map: %s base=%08x +%02x=%08x +%02x=%08x "
		       "+%02x=%08x +%02x=%08x\n", where, base,
		       o,        SIF_RD(base + o),
		       o + 0x04, SIF_RD(base + o + 0x04),
		       o + 0x08, SIF_RD(base + o + 0x08),
		       o + 0x0c, SIF_RD(base + o + 0x0c));
}

/* --------------------------------------------------------------------------
 * sif_port_init - one-time bring-up (mirrors the register work of @0x2b34).
 * NOTE: this matches stock exactly; if it is insufficient on OpenWrt the
 * missing step is a SoC clock/reset/pinmux de-gate that is NOT in sif.ko
 * (see sif_platform_degate / §8 of the notes).
 * -------------------------------------------------------------------------- */
void sif_port_init(void)
{
	int ch;

	sif_platform_degate();   /* no-op unless -DSIF_PLATFORM_CLK_DEGATE */

	printk(KERN_INFO "sifdbg: SOC_SYS_CFG(0xbfb00284)=%08x  "
	       "SOC_CHIP_VER(0xbfb00064)=%08x\n",
	       SIF_RD(SOC_SYS_CFG), SIF_RD(SOC_CHIP_VER));

	for (ch = 0; ch < SIF_NCH; ch++) {
		u32 base = sif_base[ch];

		sif_dump("init-pre ", base);
		sif_dump_full("map-pre  ", base);

		/* reset handshake (@0x2b34: 0x2d2c..0x2d7c) */
		SIF_RMW(base + SIF_CFG2, 0, 1);   /* CFG2.b0 = 0 */
		SIF_RMW(base + SIF_GO,   1, 1);   /* GO.b0   = 1 */
		SIF_RMW(base + SIF_CFG2, 0, 2);   /* CFG2.b1 = 0 */
		SIF_RMW(base + SIF_GO,   2, 2);   /* GO.b1   = 1 */

		/* enable (@0x2c4c: reg_field_write(0x40, 2, 2)) */
		SIF_RMW(base + SIF_CFG, 2, 2);    /* CFG.b1  = 1 */

		/*
		 * RUN-ARM (@0x1350): stock does this lazily on the first transaction
		 * of each channel, flipping CFG2.b0 from 0 -> 1. Without it the
		 * controller stays disabled and STAT(0x60) never asserts. We do it
		 * once here at bring-up (idempotent RMW set bits).
		 */
		SIF_RMW(base + SIF_GO,   1, 1);   /* GO.b0   = 1 */
		SIF_RMW(base + SIF_CFG2, 1, 1);   /* CFG2.b0 = 1  <-- the missing arm */

		sif_dump("init-post", base);
	}
}

/* --------------------------------------------------------------------------
 * sif_i2c_scan - probe every 7-bit address on a channel and report the ACK bit
 * (CMD[16], CMD_RESP). For each addr: START, write (addr<<1)|W, STOP; capture
 * the ACK response. On an I2C bus the vast majority of addresses are empty, so
 * whichever ACK value dominates == "no device" and the minority == a real slave.
 * This both pins the ACK polarity AND locates any slave (e.g. is 0x51 the DDM,
 * or does the EN7571 live at a different address, or is the bus truly empty).
 * Diagnostic only; does not change the data path.
 * -------------------------------------------------------------------------- */
void sif_i2c_scan(u8 ch)
{
	u32 base;
	int a, ack;
	int resp[128];
	int ones = 0, zeros = 0, errs = 0;

	if (ch >= SIF_NCH) {
		printk(KERN_INFO "sifscan: bad channel %u\n", ch);
		return;
	}
	base = sif_base[ch];

	mutex_lock(&sif_lock);
	SIF_RMW(base + SIF_CFG, CFG_CLKDIV(0x60), CFG_CLKDIV_MASK);

	for (a = 0; a < 128; a++) {
		ack = -1;
		sif_start(base, "scan");                  /* START */
		SIF_WR(base + SIF_DATA0, (u32)(a << 1));  /* addr + W */
		SIF_RMW(base + SIF_CMD,
			CMD_WR | CMD_START | CMD_CNT(1), CMD_MASK_XFER);
		if (sif_exec(base, "scan") == 0)
			ack = CMD_RESP(SIF_RD(base + SIF_CMD));
		sif_stop(base, "scan");                   /* STOP */
		resp[a] = ack;
		if (ack < 0)       errs++;
		else if (ack)      ones++;
		else               zeros++;
	}
	mutex_unlock(&sif_lock);

	printk(KERN_INFO "sifscan ch%u: ACK-bit(CMD[16]) per 7-bit addr "
	       "(. = exec-timeout):\n", ch);
	for (a = 0; a < 128; a += 16) {
		int i;
		char line[80];
		int p = scnprintf(line, sizeof(line), "  %02x:", a);
		for (i = 0; i < 16; i++)
			p += scnprintf(line + p, sizeof(line) - p, " %c",
				resp[a + i] < 0 ? '.' : (resp[a + i] ? '1' : '0'));
		printk(KERN_INFO "%s\n", line);
	}
	printk(KERN_INFO "sifscan ch%u: ack1=%d ack0=%d timeouts=%d  "
	       "=> minority value marks real slave(s); majority = bus-empty polarity\n",
	       ch, ones, zeros, errs);
	{
		int a2, mv = (ones <= zeros) ? 1 : 0;   /* minority ack value = real slave */
		char buf[200];
		int p2 = scnprintf(buf, sizeof(buf), "sifscan ch%u REAL-SLAVE addrs (ackbit=%d):", ch, mv);
		for (a2 = 0; a2 < 128; a2++)
			if (resp[a2] == mv)
				p2 += scnprintf(buf + p2, sizeof(buf) - p2, " %02x", a2);
		printk(KERN_INFO "%s\n", buf);
	}
}

/* -------------------------------------------------------------------------- */
u16 SIF_X_Read(u8 u1ChannelID, u16 u2ClkDiv, u8 u1DevAddr,
	       u8 u1WordAddrNum, u32 u4WordAddr, u8 *pu1Buf, u16 u2ByteCnt)
{
	u32 base;
	u8 sa[4];
	int got;
	int log;

	if (u1ChannelID >= SIF_NCH)
		return 0;
	if (u1WordAddrNum >= 4 || !pu1Buf || u2ByteCnt == 0)
		return 0;
	base = sif_base[u1ChannelID];

	mutex_lock(&sif_lock);

	log = (sif_dbg_read_n < SIF_DBG_MAX);
	if (log)
		sif_dbg_read_n++;

	/* program clock divider into CFG (@0x1964) */
	SIF_RMW(base + SIF_CFG, CFG_CLKDIV(u2ClkDiv), CFG_CLKDIV_MASK);

	if (log)
		printk(KERN_INFO "sifdbg: Rd ch%u dev=%02x(wire wr=%02x rd=%02x) "
		       "subN=%u off=%x cnt=%u clkDiv=%03x CFG(rdbk)=%08x STAT=%08x GO=%08x\n",
		       u1ChannelID, u1DevAddr, (u8)(u1DevAddr << 1),
		       (u8)((u1DevAddr << 1) | 1), u1WordAddrNum, u4WordAddr,
		       u2ByteCnt, u2ClkDiv & 0xfff,
		       SIF_RD(base + SIF_CFG), SIF_RD(base + SIF_STAT),
		       SIF_RD(base + SIF_GO));

	/* ---- addressing phase: START, devAddr+W, sub-address (no STOP) ---- */
	if (u1WordAddrNum > 0) {
		sif_pack_subaddr(sa, u1WordAddrNum, u4WordAddr);
		if (sif_start(base, "start") < 0)
			goto fail;
		if (sif_put_byte(base, (u8)(u1DevAddr << 1), "wraddr") < 0)
			goto fail;
		if (sif_put_bytes(base, sa, u1WordAddrNum, "subaddr") < 0)
			goto fail;
	}

	/* ---- data read phase: (repeated) START, devAddr+R, read N, STOP ---- */
	if (sif_start(base, "rstart") < 0)
		goto fail;
	if (sif_put_byte(base, (u8)((u1DevAddr << 1) | 1), "rdaddr") < 0)
		goto fail;
	got = sif_get_bytes(base, pu1Buf, u2ByteCnt);
	sif_stop(base, "stop");
	if (got != (int)u2ByteCnt)
		goto fail;

	if (log)
		printk(KERN_INFO "sifdbg: Rd OK ch%u got=%d b0=%02x b1=%02x\n",
		       u1ChannelID, got, pu1Buf[0], u2ByteCnt > 1 ? pu1Buf[1] : 0);

	mutex_unlock(&sif_lock);
	return u2ByteCnt;

fail:
	if (log)
		printk(KERN_INFO "sifdbg: Rd FAIL ch%u (exec timeout upstream)\n",
		       u1ChannelID);
	mutex_unlock(&sif_lock);
	return 0;
}

/* -------------------------------------------------------------------------- */
u16 SIF_X_Write(u8 u1ChannelID, u16 u2ClkDiv, u8 u1DevAddr,
		u8 u1WordAddrNum, u32 u4WordAddr, u8 *pu1Buf, u16 u2ByteCnt)
{
	u32 base;
	u8 sa[4];

	if (u1ChannelID >= SIF_NCH)
		return 0;
	if (u1WordAddrNum >= 4 || !pu1Buf || u2ByteCnt == 0)
		return 0;
	base = sif_base[u1ChannelID];

	mutex_lock(&sif_lock);

	SIF_RMW(base + SIF_CFG, CFG_CLKDIV(u2ClkDiv), CFG_CLKDIV_MASK);

	/* START, devAddr+W, [sub-address], data, STOP */
	if (sif_start(base, "w-start") < 0)
		goto fail;
	if (sif_put_byte(base, (u8)(u1DevAddr << 1), "w-wraddr") < 0)
		goto fail;

	if (u1WordAddrNum > 0) {
		sif_pack_subaddr(sa, u1WordAddrNum, u4WordAddr);
		if (sif_put_bytes(base, sa, u1WordAddrNum, "w-subaddr") < 0)
			goto fail;
	}

	if (sif_put_bytes(base, pu1Buf, u2ByteCnt, "w-data") < 0)
		goto fail;

	sif_stop(base, "w-stop");

	mutex_unlock(&sif_lock);
	return u2ByteCnt;

fail:
	mutex_unlock(&sif_lock);
	return 0;
}

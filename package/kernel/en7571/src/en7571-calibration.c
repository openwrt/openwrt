/* SPDX-License-Identifier: GPL-2.0 */
/*
 * EN7571 GPON laser - calibration and bring-up
 *
 * What the part needs before it can work: the calibration matrix out of the OEM
 * flash (coefficients, DDMI mode, T0C/T1C burst timing, the GPON magic), and
 * the register programming the vendor's en7571_init() does on every start.
 * Nothing here is a factory operation; the factory entries erase the matrix,
 * substitute PRBS23 and open the bias loop, and their commands are commented
 * out in the sources this was reconstructed from.
 */

#include <linux/array_size.h>
#include <linux/delay.h>
#include <linux/err.h>
#include <linux/lockdep.h>
#include <linux/minmax.h>
#include <linux/nvmem-consumer.h>
#include <linux/slab.h>
#include <linux/string.h>
#include <linux/unaligned.h>

#include "en7571.h"

/*
 * The matrix carries the MBI hold-off around an Ibias crossing in
 * microseconds (BOB_MBI_DELAY).  A word with a sane value is a few
 * microseconds; anything in the seconds is a corrupt record, and udelay()
 * of it under priv->lock would stall the machine and hold the upstream
 * burst off for that long.  Bound what the flash is allowed to ask for.
 */
#define EN7571_MBI_DELAY_MAX_US	10000

/* TGEN, below with the rest of the bring-up; set_pon_mode() needs it first */
static int en7571_tgen_locked(struct en7571 *priv, enum en7571_pon_mode mode);

/*
 * The magics stock en7571_init() initializes from.
 * All three are programmed; they differ in the TGEN argument and in whether
 * the APD converter is started.
 */
static bool en7571_magic_valid(u32 magic)
{
	return magic == BOB_MAGIC_GPON || magic == BOB_MAGIC_GPON_SYM ||
	       magic == BOB_MAGIC_EPON;
}

/*
 * Load the calibration matrix from the "calibration" nvmem cell.
 *
 * The vendor reads its matrix from a file and, when that fails, sets every word
 * to 0xffffffff and carries on; the magic is checked later, by en7571_init(),
 * which then only arms the LOS detector.  The same happens here: a missing,
 * unreadable or short cell, or an unknown magic, leaves or makes the matrix all
 * ones and is not an error, so that path is reachable and probe does not fail.
 * Only a deferral is passed back.
 *
 * The cell is read once and released, not held.  A failure after probe leaves
 * an already valid matrix alone, since the hardware was programmed from it.
 */
int en7571_read_calibration(struct en7571 *priv)
{
	struct device *dev = &priv->client->dev;
	u32 cal[BOB_ENTRIES];
	struct nvmem_cell *cell;
	unsigned int i;
	size_t len;
	u32 magic;
	u8 *buf;

	memset(cal, 0xff, sizeof(cal));

	cell = nvmem_cell_get(dev, "calibration");
	if (IS_ERR(cell)) {
		if (PTR_ERR(cell) == -EPROBE_DEFER)
			return -EPROBE_DEFER;
		if (PTR_ERR(cell) == -ENOENT || PTR_ERR(cell) == -ENODEV)
			dev_info(dev, "no calibration cell, using device defaults\n");
		else
			dev_warn(dev, "cannot get calibration cell (%pe), using device defaults\n",
				 cell);
		goto fail;
	}

	buf = nvmem_cell_read(cell, &len);
	nvmem_cell_put(cell);
	if (IS_ERR(buf)) {
		if (PTR_ERR(buf) == -EPROBE_DEFER)
			return -EPROBE_DEFER;
		dev_warn(dev, "cannot read calibration (%pe), using device defaults\n",
			 buf);
		goto fail;
	}

	if (len < BOB_LEN) {
		dev_warn(dev, "calibration cell is %zu bytes, need %d; using device defaults\n",
			 len, BOB_LEN);
		kfree(buf);
		goto fail;
	}

	/*
	 * The vendor reads 400 bytes; everything this driver uses ends at the
	 * LUT (BOB_MAX_LEN).  A cell that stops at the magic, as on boards
	 * that only map the 161-byte record, leaves the LUT words empty.
	 */
	for (i = 0; i < min_t(size_t, len / 4, ARRAY_SIZE(cal)); i++)
		cal[i] = get_unaligned_le32(&buf[i * 4]);
	kfree(buf);

	magic = cal[BOB_MAGIC / 4];
	if (!en7571_magic_valid(magic))
		dev_warn(dev, "bad calibration magic %#x\n", magic);

	scoped_guard(mutex, &priv->lock)
		memcpy(priv->cal, cal, sizeof(priv->cal));

	return 0;

fail:
	scoped_guard(mutex, &priv->lock) {
		magic = priv->cal[BOB_MAGIC / 4];
		if (!en7571_magic_valid(magic))
			memcpy(priv->cal, cal, sizeof(priv->cal));
	}

	return 0;
}

/*
 * The mode a calibration magic selects, and the only place the three magics
 * are told apart.  Stock has one branch per magic in en7571_init():
 * GPON and GPON-SYM share a branch and differ only in
 * the TGEN argument, EPON has its own and skips the APD converter.
 *
 * A magic this driver does not know is reported through the return value
 * rather than as a mode: there is no mode for it, because a matrix that
 * cannot be identified leaves the transmitter deliberately unarmed.
 */
int en7571_pon_mode_from_magic(u32 magic, enum en7571_pon_mode *mode)
{
	switch (magic) {
	case BOB_MAGIC_GPON:
		*mode = EN7571_PON_GPON;
		return 0;
	case BOB_MAGIC_GPON_SYM:
		*mode = EN7571_PON_GPON_SYM;
		return 0;
	case BOB_MAGIC_EPON:
		*mode = EN7571_PON_EPON;
		return 0;
	default:
		return -ENODEV;
	}
}

/*
 * The mode as the log prints it, which is the profile name rather than the
 * number: "calibration matrix: GPON-SYM" says more than "10".  Anything that
 * is not a mode the enum names reads as GPON, which is what a zeroed value is.
 */
const char *en7571_pon_mode_name(enum en7571_pon_mode mode)
{
	switch (mode) {
	case EN7571_PON_GPON_SYM:
		return "GPON-SYM";
	case EN7571_PON_EPON:
		return "EPON";
	default:
		return "GPON";
	}
}

/*
 * Select the mode at runtime.  The mode is the MAC's decision: the matrix is
 * calibrated for one mode but not restricted to it, and the T0/T1 delay byte
 * and the APD gating are the only mode-dependent steps.  Only the TGEN part is
 * replayed; the bias loop, the receive path and the flash calibration are
 * mode-independent and left alone.
 *
 * The timing byte may only be rewritten with the link down: under a burst in
 * flight it would leave the burst described by a word that no longer matches.
 * That is the one step here that is not self-correcting, and the vendor
 * sequences the same steps from a place where TX is already off.
 *
 * A change with the link up is refused with -EBUSY.  Bringing the transmitter
 * down is the caller's decision, not this driver's: it is the one that knows
 * whether the stack above is coming back up, and a live laser left off would
 * be a state nobody asked for.  Both traffic (a burst is running) and
 * tx_enabled (the transmitter belongs to the regulator's reference count)
 * refuse it.
 */
int en7571_set_pon_mode(struct en7571 *priv, enum en7571_pon_mode mode)
{
	enum en7571_pon_mode old_mode;
	int ret;

	if (!priv || !priv->client)
		return -EINVAL;

	if (mode != EN7571_PON_GPON && mode != EN7571_PON_GPON_SYM &&
	    mode != EN7571_PON_EPON)
		return -EINVAL;

	guard(mutex)(&priv->lock);

	if (priv->pon_mode == mode)
		return 0;

	old_mode = priv->pon_mode;

	if (READ_ONCE(priv->traffic) || READ_ONCE(priv->tx_enabled))
		return -EBUSY;

	priv->pon_mode = mode;

	ret = en7571_tgen_locked(priv, mode);
	if (ret) {
		/* Put the timing byte back the way the failed change found it */
		priv->pon_mode = old_mode;
		en7571_tgen_locked(priv, old_mode);

		return ret;
	}

	/*
	 * Stock leaves the APD converter inside the GPON branch, so a mode that
	 * is not GPON stops the tracking that follows it.  Both directions are
	 * needed and neither is a hardware change: the converter is started once
	 * during calibration and stays started, and this is only the gate on the
	 * temperature tracking that reads it - en7571_APD_initialization() is a
	 * one-way enable, which is why nothing here tries to undo it and why the
	 * converter being live in a mode that does not track is the state stock
	 * never leaves either.
	 */
	priv->gpon = mode != EN7571_PON_EPON;

	return 0;
}

/*
 * Every step below is in the vendor form, "keep these bits, force those", on
 * the 4-byte register, which is what en7571_rmw_byte() does.  All of this file
 * runs with priv->lock held.
 */

static int en7571_sw_reset(struct en7571 *priv)
{
	/* SW_RESET byte 0, code field */
	return en7571_rmw_byte(priv, EN7571_SW_RESET, 0, 0xf8, 0x01);
}

static int en7571_mode_enable(struct en7571 *priv)
{
	/* PWR_CTRL_0 byte 0: run the controller in EN7571 mode */
	return en7571_rmw_byte(priv, EN7571_PWR_CTRL_0, 0, EN7571_PWR_MODE_MASK,
			  EN7571_PWR_MODE_EN7571);
}

static int en7571_pwradc_enable(struct en7571 *priv)
{
	int ret;

	/* RG_TIAMUX back to its neutral position (1-byte access) */
	en7571_try(en7571_rmw8(priv, EN7571_TIAMUX, 0xf1, 0x00));

	/* RG_PWRK1 off, the bias source is no longer being calibrated */
	en7571_try(en7571_rmw_byte(priv, EN7571_RG_ANA_CTRL1, 3, 0xef, 0x00));

	/* rg_pwr_ctrl_e3_ben_block = 2'b01 (1-byte access) */
	return en7571_rmw8(priv, EN7571_RG_PWR_CTRL_BEN_0, 0xfc, 0x01);
}

/*
 * Sample the on-die temperature reference.
 *
 * The eFuse holds a per-die trim for the temperature sensor, signed and
 * biased at 127, at 0.3 C per code.  It is added to the fixed 3.44 C the
 * vendor always subtracts, so leaving it out shifts every temperature
 * reading by however far this die is from nominal.
 */
static int en7571_efuse_temperature_read(struct en7571 *priv)
{
	s32 code;
	u32 val;
	int ret;

	en7571_try(en7571_rmw_byte(priv, EN7571_RG_EFUSE_TEMP, 2, 0xfe, 0x01));

	msleep(50);

	en7571_try(en7571_read_reg(priv, EN7571_RG_EFUSE_TEMP, 4, &val));

	code = val & 0xff;
	if (code < 127)
		priv->efuse_temp_offset_millideg = code * 300;
	else
		priv->efuse_temp_offset_millideg = -(256 - code) * 300;

	return 0;
}

/*
 * Program the bias loop.
 *
 * These are fixed operating points rather than measurements: the loop is put
 * into force mode, its step sizes and interval limits are set, and the
 * averages it takes are fixed.  The current it settles on comes later, from
 * the calibration matrix.
 *
 * They are held as a table walked by one loop rather than as thirty-odd
 * spelled-out read-modify-writes.  Each row is one byte of one 4-byte
 * register, in the vendor's order, in the same "keep these bits, force those"
 * form en7571_rmw_byte() takes, so the sequence written to the part is
 * unchanged.  Keep masks of 0x00 overwrite the whole byte and need no read
 * back, which is what the vendor's own writes did for those bytes.
 *
 * The order matters and is the vendor's: the loop is programmed before the
 * operating point is loaded from flash, and the KT bit is cleared last.
 */
struct en7571_reg_init_step {
	u16 reg;
	u8 index;
	u8 keep;
	u8 set;
};

static const struct en7571_reg_init_step en7571_reg_init_seq[] = {
	/* Imax into the two clamp registers, with their low halves cleared */
	{ EN7571_PWR_CTRL_6, 3, 0x00, 0x0f }, { EN7571_PWR_CTRL_6, 2, 0x00, 0xff },
	{ EN7571_PWR_CTRL_6, 1, 0x00, 0x00 }, { EN7571_PWR_CTRL_6, 0, 0x00, 0x00 },

	{ EN7571_PWR_CTRL_7, 3, 0x00, 0x0f }, { EN7571_PWR_CTRL_7, 2, 0x00, 0xff },
	{ EN7571_PWR_CTRL_7, 1, 0x00, 0x00 }, { EN7571_PWR_CTRL_7, 0, 0x00, 0x00 },

	/* Iav and Imod command values; overwritten below by flash */
	{ EN7571_PWR_CTRL_9, 3, 0xf0, 0x02 }, { EN7571_PWR_CTRL_9, 2, 0x00, 0x00 },
	{ EN7571_PWR_CTRL_9, 1, 0xf0, 0x03 }, { EN7571_PWR_CTRL_9, 0, 0x00, 0x00 },

	/* Step multiplier select and the three Pav/MPDH step sizes */
	{ EN7571_PWR_CTRL_3, 0, 0xfc, 0x02 },
	{ EN7571_PWR_CTRL_3, 3, 0xc0, 0x3f },
	{ EN7571_PWR_CTRL_3, 2, 0xc0, 0x25 },
	{ EN7571_PWR_CTRL_3, 1, 0xc0, 0x3f },

	{ EN7571_PWR_CTRL_4, 2, 0xc0, 0x25 },
	{ EN7571_PWR_CTRL_4, 1, 0xc0, 0x3f },
	{ EN7571_PWR_CTRL_4, 0, 0xc0, 0x25 },

	/* Update period, and how far Iav and Imod may move in one step */
	{ EN7571_PWR_CTRL_8, 3, 0xf8, 0x01 },
	{ EN7571_PWR_CTRL_8, 1, 0x00, 0x02 },
	{ EN7571_PWR_CTRL_8, 0, 0x00, 0x04 },

	/*
	 * Force mode rather than auto-lock.  Auto-lock would re-measure the
	 * loop against flash it does not have, and the vendor only ever turns
	 * it on from the factory command that writes that flash first.
	 */
	{ EN7571_PWR_CTRL_C, 3, 0xfc, 0x03 },

	/* Phase 1/2 timing */
	{ EN7571_PWR_CTRL_5, 1, 0xfc, 0x00 },
	{ EN7571_PWR_CTRL_5, 0, 0x00, 0x07 },

	/* MPDH step size and averaging */
	{ EN7571_PWR_CTRL_1, 3, 0xc0, 0x02 },
	{ EN7571_PWR_CTRL_1, 0, 0xf8, 0x07 },

	/* PWRADC averaging */
	{ EN7571_PWR_CTRL_E, 3, 0xf8, 0x03 },

	/* Adjustment interval: disabled, the loop follows every sample */
	{ EN7571_PWR_CTRL_B, 3, 0x00, 0x00 },
	{ EN7571_PWR_CTRL_B, 2, 0x00, 0x00 },

	/* rg_pwr_ctrl_e3_Imod_IavScale = 0 */
	{ EN7571_PWR_CTRL_A, 2, 0x80, 0x00 },

	/*
	 * Hardware KT off (PWR_CTRL_2 byte 0 bit 0, 4-byte access in the
	 * stock driver); en7571_link_reg() turns it back on.
	 */
	{ EN7571_PWR_CTRL_2, 0, EN7571_IMOD_ADJ_SEL_MASK, 0x00 },
};

static int en7571_reg_init(struct en7571 *priv)
{
	const struct en7571_reg_init_step *s;
	u32 limit;
	int ret;

	/*
	 * The limiters.  The vendor source writes fixed minimums (Ibias 0x80,
	 * Imod 0x32); the stock driver instead writes flash 0x70 and 0x74
	 * whole when they are present and otherwise leaves the reset values,
	 * which is followed here.
	 */
	limit = priv->cal[BOB_IBIAS_LIMIT / 4];
	if (limit != 0xffffffff)
		en7571_try(en7571_write_reg(priv, EN7571_PWR_LIMITER_0, 4, limit));

	limit = priv->cal[BOB_IMOD_LIMIT / 4];
	if (limit != 0xffffffff)
		en7571_try(en7571_write_reg(priv, EN7571_PWR_LIMITER_2, 4, limit));

	for (s = en7571_reg_init_seq;
	     s < en7571_reg_init_seq + ARRAY_SIZE(en7571_reg_init_seq); s++)
		en7571_try(en7571_rmw_byte(priv, s->reg, s->index, s->keep,
					  s->set));

	return 0;
}
/*
 * Load the operating point measured on the bench.
 *
 * Iav and Imod go to the live command registers and to the pair the loop falls
 * back to, Pav and P1 the same way with the two halves exchanged on the way in.
 * No matrix entry leaves the defaults reg_init() just wrote, as in the vendor.
 *
 * Two of the vendor's four steps differ here:
 *
 *  - flash 0x48 goes into [15:14] and [11:9] of register 0x00.  Despite the
 *    name the SDK gives that word it is not a power point, see BOB_TIAMUX_CFG.
 *  - flash 0x08 is not written to P0/P1_PWR_CTRL_CS2, because this driver
 *    reaches the same numbers by a better route: en7571_lut_recover() reads
 *    0x08 and offsets the flash table so its entry at the calibration
 *    temperature is exactly those DAC codes, and en7571_lut_tracking() writes
 *    them to the same two registers every ten seconds, tracking temperature
 *    where the vendor writes once and leaves them.  In dual closed loop the
 *    hardware loop drives those DACs and the write would be overwritten before
 *    it was read.
 */
static int en7571_load_tx_cal_data(struct en7571 *priv)
{
	u8 buf[4];
	u32 flash;
	int ret;

	flash = priv->cal[BOB_IAV_IMOD / 4];
	if (flash != 0xffffffff) {
		put_unaligned_le16((flash & EN7571_FLASH_IAV_MASK) >> 16, &buf[0]);
		put_unaligned_le16(flash & EN7571_FLASH_IMOD_MASK, &buf[2]);

		en7571_try(en7571_reg_write(priv, EN7571_PWR_CTRL_9, buf, 4));
		en7571_try(en7571_reg_write(priv, EN7571_PWR_CTRL_FLASH_1, buf, 4));
	}

	flash = priv->cal[BOB_PAV_P1 / 4];
	if (flash != 0xffffffff) {
		put_unaligned_le16((flash & EN7571_FLASH_PAV_MASK) >> 16, &buf[0]);
		put_unaligned_le16(flash & EN7571_FLASH_P1_MASK, &buf[2]);
		en7571_try(en7571_reg_write(priv, EN7571_PWR_CTRL_D, buf, 4));

		/* The fallback pair carries P1 in the low half instead */
		put_unaligned_le16(flash & EN7571_FLASH_P1_MASK, &buf[0]);
		put_unaligned_le16((flash & EN7571_FLASH_PAV_MASK) >> 16, &buf[2]);
		en7571_try(en7571_reg_write(priv, EN7571_PWR_CTRL_FLASH_2, buf, 4));
	}

	/* The vendor's BitWrite(0, 14, 15, hi) and BitWrite(0, 9, 11, lo):
	 * a read-modify-write of the whole 32-bit word around two fields in
	 * its low half.  en7571_update() with len 4 and index 1 is the same
	 * access, so only the field positions differ from the call.
	 */
	flash = priv->cal[BOB_TIAMUX_CFG / 4];
	if (flash != 0xffffffff) {
		ret = en7571_update(priv, EN7571_TIAMUX, 4, 1,
				    EN7571_TIAMUX_HI_MASK,
				    ((flash >> 16) & 0x3) <<
				    EN7571_TIAMUX_HI_SHIFT);
		if (ret)
			return ret;

		ret = en7571_update(priv, EN7571_TIAMUX, 4, 1,
				    EN7571_TIAMUX_LO_MASK,
				    (flash & 0x7) << EN7571_TIAMUX_LO_SHIFT);
		if (ret)
			return ret;
	}

	return 0;
}

/*
 * One averaged transmit power detector reading: eight trigger-and-read pairs
 * summed and rounded to their mean.
 *
 * Both inputs of the shutdown threshold are measured this way in the vendor -
 * the loop at phy.ann:0x2da18 for the filter input and the one at 0x2dabc for
 * the shutdown input run the same eight rounds and the same rounding.  The
 * average matters here more than elsewhere, because the threshold is a
 * difference of two readings weighted about 1.56 (see below), so a single pair
 * passes the full quantisation noise straight into the transmitter shutdown
 * level.
 */
static int en7571_pwradc_average(struct en7571 *priv, u32 *mean)
{
	u32 sum = 0;
	unsigned int i;
	int ret;

	for (i = 0; i < EN7571_ADC_CAL_SAMPLES; i++) {
		u32 raw;

		ret = en7571_rmw8(priv, EN7571_RG_PWRADC_DATA2 + 3, 0xff, 0x80);
		if (ret)
			return ret;

		ret = en7571_read_reg(priv, EN7571_RG_PWRADC_DATA, 2, &raw);
		if (ret)
			return ret;

		sum += raw & 0x3ff;
	}

	/* ((sum >> 2) + 1) >> 1 */
	*mean = ((sum >> 2) + 1) >> 1;

	return 0;
}

/*
 * Set the transmit shutdown threshold.
 *
 * The power detector is read twice, once through the filter input and once
 * through the shutdown input, and the difference between them plus a fixed
 * fraction of the calibrated optical power places the shutdown point a little
 * above the operating point.  Everything is in detector counts.
 */
static int en7571_txsd_level_set(struct en7571 *priv)
{
	u32 mux, reg, flash;
	u32 a, b, offs;
	u16 d;
	s32 num;
	u8 sd[2];
	int ret, restore;

	/*
	 * Two bytes, not one: the coefficient selector is TIAMUX[15], in the
	 * high byte, and the vendor reads it as such (phy.ann:0x2db78).  Only
	 * the low byte is ever written back below, as before.
	 */
	en7571_try(en7571_read_reg(priv, EN7571_TIAMUX, 2, &mux));

	/* The vendor zeroes the threshold first, before A as well (0x2d9a4) */
	en7571_try_goto(en7571_reg_read(priv, EN7571_TIASD, sd, 2), out);
	sd[0] = 0x00;
	sd[1] &= 0xfe;
	en7571_try_goto(en7571_reg_write(priv, EN7571_TIASD, sd, 2), out);

	/* Measure the filter input: TIAMUX b'100, settle, then read A */
	en7571_try_goto(en7571_write_reg(priv, EN7571_TIAMUX, 1, (mux & 0xf1) | 0x08), out);
	/*
	 * Five 1 ms waits sit between the mux switch and the first sample
	 * (phy.ann:0x2d9d4-0x2da08).  Only on this leg: the shutdown input is
	 * read straight after its own write, with nothing in between.
	 */
	msleep(5);
	en7571_try_goto(en7571_pwradc_average(priv, &reg), out);
	a = reg;

	/* Measure the shutdown input, the threshold still at zero */
	en7571_try_goto(en7571_rmw8(priv, EN7571_TIAMUX, 0xf1, 0x02), out);
	en7571_try_goto(en7571_pwradc_average(priv, &reg), out);
	b = reg;

	flash = priv->cal[BOB_PAV_P1 / 4];
	offs = priv->pwradc_offset >> 6;
	/* D' is a 16-bit value the vendor then widens unsigned (0x2db60-0x2db9c) */
	d = ((flash & EN7571_FLASH_PAV_MASK) >> 18) - (s16)offs;

	/*
	 * The vendor keeps this in Q20 fixed point, not floating point, and
	 * finishes with an arithmetic shift (phy.ann:0x2db94-0x2dc04):
	 *
	 *   (D' * Cd + (A - B) * 1631118 + 0x600000) >> 20
	 *
	 * 1631118/2^20 is 2.8/1.8, so the (A - B) leg is weighted about 1.56
	 * and dominates the expression, and 0x600000/2^20 is the constant 6.
	 * Cd is 163112 - which is 0.1*2.8/1.8 - or 108741 when TIAMUX[15] is
	 * set, exactly two thirds of it, so on those boards the threshold sits
	 * a third lower against the optical power leg.  That bit is set from
	 * the flash calibration block (en7571_load_Tx_cal_data), never by this
	 * function.
	 *
	 * Exactly as the vendor: D' enters the product zero-extended, the
	 * sum is 32-bit with an arithmetic shift, and only the low nine bits
	 * reach the field - no clamp, no floor at zero.
	 */
	num = (mux & BIT(15) ? 108741 : 163112) * (u32)d +
	      1631118 * (s32)(a - b) + 0x600000;
	num >>= 20;

	/* BitWrite(0xc, 0, 8, ...): bits 15:9 of the register stay as they are */
	sd[0] = num & 0xff;
	sd[1] = (sd[1] & 0xfe) | ((num >> 8) & 0x01);
	ret = en7571_reg_write(priv, EN7571_TIASD, sd, 2);

out:
	restore = en7571_write_reg(priv, EN7571_TIAMUX, 1, mux & 0xff);

	return ret ? ret : restore;
}

/*
 * Arm the loss-of-signal detector and place its thresholds.
 *
 * The trigger, averaging counts and confidence window are programmed first so
 * the detector has settled; the thresholds come from the matrix, the low word
 * holding LOS and the one above it shutdown, both seven bits with the top bit
 * reserved for the comparison threshold.  No entry means the silicon defaults,
 * 0x20 and 0x30.
 *
 * One pair covers the whole operating range, so the thresholds are per device
 * from the bench, not per temperature.
 */
static int en7571_los_init(struct en7571 *priv)
{
	int ret;

	en7571_try(en7571_rmw_byte(priv, EN7571_LOS_CTRL1, 0, 0xfe, 0x01));
	en7571_try(en7571_rmw_byte(priv, EN7571_LOS_CTRL1, 1, 0xe0, 0x1f));
	en7571_try(en7571_rmw_byte(priv, EN7571_SVADC_PD, 3, 0xfb, 0x04));
	en7571_try(en7571_rmw_byte(priv, EN7571_SVADC_PD, 2, 0xbf, 0x40));
	en7571_try(en7571_rmw_byte(priv, EN7571_LOS_CTRL2, 1, 0xe0, 0x1f));
	return en7571_rmw_byte(priv, EN7571_LOS_CTRL2, 0, 0x80, 0x05);
}

static int en7571_los_level_set(struct en7571 *priv)
{
	u32 flash = priv->cal[BOB_LOS_THLD / 4];
	u32 los, sd;
	int ret;

	en7571_try(en7571_los_init(priv));

	if (flash != 0xffffffff) {
		los = flash & EN7571_FLASH_LOS_MASK;
		sd = (flash & EN7571_FLASH_SD_MASK) >> EN7571_FLASH_SD_SHIFT;
	} else {
		los = EN7571_LOS_THLD_DEFAULT;
		sd = EN7571_SD_THLD_DEFAULT;
	}

	en7571_try(en7571_rmw_byte(priv, EN7571_LOS_CTRL1, 3,
				   EN7571_LOS_COMP_MASK, los));
	return en7571_rmw_byte(priv, EN7571_LOS_CTRL1, 2,
			       EN7571_LOS_COMP_MASK, sd);
}

static int en7571_rssi_gain_init(struct en7571 *priv)
{
	return en7571_rmw_byte(priv, EN7571_LA_PWD, 2, EN7571_RSSI_GAIN_MASK,
			  EN7571_RSSI_GAIN_MAX);
}

static int en7571_rogue_onu_clear(struct en7571 *priv)
{
	/* 1-byte access to ROGUE_ONU_DET_CTRL + 1 */
	return en7571_rmw8(priv, EN7571_ROGUE_ONU_DET_CTRL + 1, 0xfe, 0x01);
}

/*
 * Read the runtime options the matrix carries and start the laser control the
 * way they ask for (vendor en7571_config; the older vendor source has an ER
 * controller on 0x60 instead of the mbi_delay below).
 *
 *   0x2c  diagnostics: exactly 0 off, 2 fast (everything every second), any
 *         other value, empty included, staggered
 *   0x90  1 single closed loop, 2 open loop, each driven from the LUT;
 *         otherwise dual closed loop with KT, 0x64 the factors (default
 *         0x00060020 when empty) and 0x68 inverse (2) or enhanced (3) KT.
 *         The default factors stay in RAM; the vendor writes them back to
 *         flash, this driver never writes the matrix.
 *   0x6c  exactly 0 turns crossing protection off
 *   0x60  mbi_delay in microseconds when present
 *
 * The Pav loop is always enabled.
 */
static int en7571_config(struct en7571 *priv)
{
	struct device *dev = &priv->client->dev;
	u32 val = priv->cal[BOB_INTERNAL_DDMI / 4];
	int ret;

	if (val == 0) {
		priv->ddmi_mode = EN7571_DDMI_OFF;
	} else if (val == EN7571_DDMI_FAST) {
		priv->ddmi_mode = EN7571_DDMI_FAST;
	} else {
		priv->ddmi_mode = EN7571_DDMI_NORMAL;
	}

	val = priv->cal[BOB_LOOP_MODE / 4];
	if (val == 1 || val == 2) {
		if (val == 1) {
			priv->scl = true;
			ret = en7571_single_closed_loop_mode(priv);
		} else {
			priv->dol = true;
			ret = en7571_open_loop_mode(priv, true);
		}
		if (ret)
			return ret;

		en7571_lut_recover(priv);

		/* The vendor takes a fresh temperature before tracking */
		en7571_try(en7571_refresh_locked(priv, EN7571_DD_TEMP));

		en7571_try(en7571_lut_tracking(priv));

		dev_info(dev, "%s loop mode\n",
			 priv->scl ? "single closed" : "open");
	} else {
		val = priv->cal[BOB_KT_SWITCH / 4];
		priv->kt = val == 2 || val == 3 ? val : 1;
		dev_info(dev, "KT %s\n", priv->kt == 2 ? "inversed" :
			 priv->kt == 3 ? "enhanced" : "enabled");
	}

	if (!priv->cal[BOB_CROSS / 4])
		priv->cross = false;

	val = priv->cal[BOB_MBI_DELAY / 4];
	if (val != 0xffffffff) {
		if (val > EN7571_MBI_DELAY_MAX_US) {
			dev_warn(dev, "mbi_delay %uus is out of range, using %uus\n",
				 val, EN7571_MBI_DELAY_MAX_US);
			val = EN7571_MBI_DELAY_MAX_US;
		}
		priv->mbi_delay_us = val;
		dev_info(dev, "mbi_delay=%uus\n", val);
	}

	return 0;
}

/*
 * Check that the part on the bus is an EN7571 (vendor en7571_EN7571_detection):
 * FT_ADC_CLK_CLR byte 0 reads 0x03 and DUMMY byte 0 at least 0x03.  The
 * vendor source takes only DUMMY exactly 0x03 as revision 2; the stock
 * driver sets revision 2 for every part it accepts.  Nothing is written.
 */
int en7571_detect(struct en7571 *priv)
{
	u32 clk, dummy;
	int ret;

	en7571_try(en7571_read_reg(priv, EN7571_FT_ADC_CLK_CLR, 1, &clk));
	en7571_try(en7571_read_reg(priv, EN7571_DUMMY, 1, &dummy));

	if (clk != 0x03 || dummy < 0x03)
		return -ENODEV;

	return 0;
}

/*
 * TGEN, runtime part (vendor en7571_TGEN).
 *
 * When flash 0x0c is empty the vendor first runs a 32-round search for the
 * largest T0C/T1C with the PHY sending PRBS23.  That is a live factory
 * measurement and is not done here: without the word, T1C/T0C are left as
 * found.  With it, T0C is byte 2 and T1C byte 3.
 *
 * T1DELAY byte 0, the T0/T1 delay, is the low byte of flash 0x0c unless that
 * byte is 0xff, else it follows the mode: 0xaa for GPON, 0x77 for GPON-SYM and
 * EPON.  Any other argument leaves the byte as read.
 *
 * Then, as the vendor: the byte and T1C/T0C written, 10 ms, a T1/T0 timer reset
 * pulse 10 ms wide, 10 ms, and ERC enable.
 *
 * Called with priv->lock held.
 */
static int en7571_tgen_locked(struct en7571 *priv, enum en7571_pon_mode mode)
{
	struct device *dev = &priv->client->dev;
	u32 flash = priv->cal[BOB_T0CT1C / 4];
	u8 buf[4];
	int ret;

	en7571_try(en7571_reg_read(priv, EN7571_T1DELAY, buf, sizeof(buf)));

	if ((flash & 0xff) != 0xff) {
		buf[0] = flash & 0xff;
		dev_info(dev, "fixed T0/T1 delay 0x%02x\n", buf[0]);
	} else if (mode == EN7571_PON_GPON) {
		buf[0] = EN7571_T1T0_DELAY_GPON;
	} else if (mode == EN7571_PON_GPON_SYM || mode == EN7571_PON_EPON) {
		buf[0] = EN7571_T1T0_DELAY_OTHER;
	}

	en7571_try(en7571_reg_write(priv, EN7571_T1DELAY, buf, sizeof(buf)));

	if (flash != 0xffffffff) {
		u8 t0c = (flash & FLASH_T0C_MASK) >> 16;
		u8 t1c = FIELD_GET(FLASH_T1C_MASK, flash);

		dev_info(dev, "burst timing T0C=0x%02x T1C=0x%02x\n", t0c, t1c);

		en7571_try(en7571_reg_read(priv, EN7571_T1DELAY, buf, sizeof(buf)));
		buf[1] = t1c;
		buf[2] = t0c;
		en7571_try(en7571_reg_write(priv, EN7571_T1DELAY, buf, sizeof(buf)));
	} else {
		dev_warn(dev, "no T0C/T1C in calibration matrix, leaving burst timing as found\n");
	}

	msleep(10);

	ret = en7571_rmw_byte(priv, EN7571_T1DELAY, 3, EN7571_TGEN_RESET_MASK,
			      EN7571_TGEN_RESET_T1T0);
	if (ret)
		return ret;

	msleep(10);

	en7571_try(en7571_rmw_byte(priv, EN7571_T1DELAY, 3, EN7571_TGEN_RESET_MASK, 0));

	msleep(10);

	return en7571_rmw_byte(priv, EN7571_T1DELAY, 3, EN7571_ERC_ENABLE_MASK,
			       EN7571_ERC_ENABLE);
}

/*
 * Bring the controller up, in the order the vendor's en7571_init() does it.
 *
 * Common part: reset, mode select, the PWRADC dark calibration, the ADC and RSSI
 * calibrations and the eFuse temperature trim.  Then the magic decides: GPON and
 * GPON-SYM run TGEN and start the APD converter at the vendor's initial 20 C,
 * EPON runs TGEN alone, and anything else arms the LOS detector, turns the
 * internal diagnostics off and stops there, leaving the bias loop at its reset
 * values.  With a usable matrix the bias loop is programmed and loaded from
 * flash, then the receive side and the arming bits.
 *
 * Two vendor steps are not done here: putting the PHY into GPON mode
 * (phy_mode_config), which is the PHY driver's business, and enabling the
 * transmitter, which the vendor does just after en7571_init() and this driver
 * leaves to the MAC.
 *
 * Called with priv->lock held.
 */
int en7571_hw_init(struct en7571 *priv)
{
	struct device *dev = &priv->client->dev;
	enum en7571_pon_mode mode;
	u32 tapd;
	int ret;

	lockdep_assert_held(&priv->lock);

	/* The vendor resets the loop state on every (re)initialization */
	en7571_ctrl_reset(priv);
	priv->ddmi_mode = EN7571_DDMI_NORMAL;

	en7571_try_probe(dev, en7571_sw_reset(priv), "cannot reset the laser\n");

	en7571_try_probe(dev, en7571_hw_reset(priv), "cannot pulse the hardware reset\n");

	en7571_try_probe(dev, en7571_mode_enable(priv), "cannot select EN7571 mode\n");

	en7571_try_probe(dev, en7571_pwradc_calibration(priv), "cannot measure the dark offset\n");

	en7571_try_probe(dev, en7571_pwradc_enable(priv), "cannot start the power detector\n");

	en7571_try_probe(dev, en7571_adc_calibration(priv), "cannot calibrate the ADC\n");

	en7571_try_probe(dev, en7571_rssi_calibrate(priv), "cannot calibrate the receive path\n");

	en7571_try_probe(dev, en7571_efuse_temperature_read(priv), "cannot read the temperature trim\n");

	/* XPON_SETTING (0x10f) is written by the xPON PHY driver, which owns
	 * the register (phy_trans_model_setting()). */

	priv->tapd = EN7571_TAPD_DEFAULT;
	priv->bosa_temp_millideg = EN7571_BOSA_TEMP_INIT_MD;
	priv->apd_mv = EN7571_APD_VOLTAGE_INIT_MV;

	/*
	 * The vendor clears the alarm word and the DDMI values on every
	 * initialization; zero values are also what makes the tick retake
	 * bias and both powers at once.
	 */
	priv->alarm = 0;
	priv->bias_8472 = 0;
	priv->mod_8472 = 0;
	priv->tx_power_8472 = 0;
	priv->rx_power_8472 = 0;
	priv->ddmi_valid = 0;

	/*
	 * GPON and xPON (0xa7050701) take the same path in the stock driver:
	 * the PHY in GPON mode, en7571_xPON_mode = 1 (APD tracking on), TGEN,
	 * the APD converter and T_APD.  They differ only in the TGEN
	 * argument, which picks the T0/T1 delay (0xaa GPON, 0x77 GPON-SYM).
	 */
	if (en7571_pon_mode_from_magic(priv->cal[BOB_MAGIC / 4], &mode)) {
		en7571_try(en7571_los_init(priv));
		priv->ddmi_mode = EN7571_DDMI_OFF;
		dev_warn(dev, "no usable calibration matrix, only LOS armed; internal DDMI disabled\n");
		return 0;
	}

	priv->pon_mode = mode;

	/*
	 * Stock runs the APD converter in the GPON branch only and skips it for
	 * EPON, so a matrix naming EPON used to take the path below with the
	 * converter left unstarted.  The mode the part will run in is not
	 * decided here - it arrives from the stack when the link comes up, and
	 * en7571_set_pon_mode() applies it - so all this has to do is leave the
	 * part able to track in either.  Starting the converter once is what
	 * makes that possible: en7571_APD_initialization() is a one-way enable
	 * with no matching entry to undo it, so a part that came up without it
	 * could not be given one later.
	 *
	 * The TGEN argument is the one thing still taken from the matrix, since
	 * it picks the T0/T1 delay the part starts with (0xaa GPON, 0x77
	 * GPON-SYM and EPON).  A mode that disagrees with it overwrites the byte
	 * before the transmitter is ever enabled.
	 *
	 * What is *not* taken from the matrix is the tracking gate, and here it
	 * has to agree with en7571_set_pon_mode() below or the two disagree: stock
	 * en7571_init() stores xPON_mode = 0 on the EPON magic 0xe7050701 and 1
	 * on the other two, and en7571_internal_clock() calls en7571_APD_control()
	 * only when it reads 1.  A matrix naming EPON therefore never writes
	 * APD_DAC_CODE again after init.  Leaving priv->gpon set made the driver
	 * rewrite it every T_APD seconds forever - harmless to the laser, which
	 * follows temperature, but a register state stock never has.
	 */
	priv->gpon = mode != EN7571_PON_EPON;

	en7571_try_probe(dev, en7571_tgen_locked(priv, mode), "cannot run TGEN\n");

	en7571_try_probe(dev, en7571_apd_init(priv), "cannot start the APD converter\n");

	tapd = priv->cal[BOB_T_APD / 4];
	if (tapd != 0xffffffff) {
		if (tapd < EN7571_TAPD_MIN) {
			dev_warn(dev, "APD update period %u s is too short, using %u s\n",
				 tapd, EN7571_TAPD_MIN);
			tapd = EN7571_TAPD_MIN;
		}
		priv->tapd = tapd;
	}

	ret = en7571_apd_set_locked(priv, priv->bosa_temp_millideg);
	if (ret == -EINVAL)
		dev_warn(dev, "APD voltage points in flash are not increasing, APD code not set\n");
	else if (ret)
		return dev_err_probe(dev, ret, "cannot set the APD voltage\n");

	en7571_try_probe(dev, en7571_reg_init(priv), "cannot program the bias loop\n");

	en7571_try_probe(dev, en7571_load_tx_cal_data(priv), "cannot load the operating point\n");

	en7571_try_probe(dev, en7571_txsd_level_set(priv), "cannot set the shutdown threshold\n");

	en7571_try(en7571_rssi_gain_init(priv));

	en7571_try_probe(dev, en7571_los_level_set(priv), "cannot arm loss of signal\n");

	en7571_try(en7571_rogue_onu_clear(priv));

	en7571_try(en7571_safe_circuit_reset_locked(priv));

	en7571_try(en7571_link_reg(priv, true));

	en7571_try(en7571_dcl_start(priv));

	en7571_try_probe(dev, en7571_config(priv), "cannot start the laser control\n");

	dev_info(dev, "initialized (%s), %s diagnostics\n",
		 en7571_pon_mode_name(mode),
		 priv->ddmi_mode == EN7571_DDMI_OFF ? "no" :
		 priv->ddmi_mode == EN7571_DDMI_FAST ? "fast" : "staggered");

	return 0;
}

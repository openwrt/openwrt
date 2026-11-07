/* SPDX-License-Identifier: GPL-2.0 */
/*
 * EN7571 GPON laser - Laser control loop
 *
 * Temperature compensation and the closed loops that run once a second from
 * the DDMI tick (en7571_config, en7571_internal_clock and what they call).
 * The older vendor sources the register names come from differ from the stock
 * driver in several places; where they do, the stock driver is followed.
 *
 * Then what the loops drive: the transmitter, the power monitors, the APD and
 * the RSSI.
 *
 * Three ways of running the laser exist, chosen by flash 0x90:
 *
 *   1  single closed loop: the MPDH loop stopped, Ibias/Imod from a LUT
 *   2  open loop: both DACs held, Ibias/Imod from a LUT every ten seconds
 *   *  dual closed loop with kill-transmit (KT) compensation of Imod
 *
 * In the last, ordinary, case the hardware loop runs while the link is down
 * and SWKT only reprograms the KT factor.  When PON traffic comes up the DACs
 * are frozen at their current codes (open loop) and software takes over: Ibias
 * follows the average power target (Pav loop) and Imod the KT target derived
 * from Iav (OPKT).  Codes crossing a 256-code DAC boundary are written with
 * the MAC burst interface stopped.
 *
 * The vendor's floating-point arithmetic is redone in integers that give the
 * same results bit for bit; see en7571_f32().  Everything here runs with
 * priv->lock held unless stated otherwise.
 */

#include <linux/bitfield.h>
#include <linux/bitops.h>
#include <linux/delay.h>
#include <linux/err.h>
#include <linux/export.h>
#include <linux/gpio/consumer.h>
#include <linux/math64.h>
#include <linux/minmax.h>
#include <linux/module.h>
#include <linux/overflow.h>
#include <linux/pon_laser.h>
#include <linux/string.h>
#include <linux/unaligned.h>

#include "en7571.h"
#include "en7571-hw.h"

/*
 * Initial LUT (en7571_LUT_Ibias_Imod), used until
 * en7571_LUT_recover() rebuilds it and kept if the calibration temperature
 * is out of range.  {Ibias, Imod} per 2.5 C from -40 C.
 */
static const s32 en7571_lut_default[EN7571_LUT_NR][2] = {
	{ 0x106, 0x3b2 }, { 0x10a, 0x3b2 }, { 0x10f, 0x3b6 }, { 0x118, 0x3bb },
	{ 0x11d, 0x3bb }, { 0x121, 0x3c0 }, { 0x12a, 0x3c4 }, { 0x12f, 0x3c4 },
	{ 0x133, 0x3c9 }, { 0x13c, 0x3cd }, { 0x141, 0x3cd }, { 0x146, 0x3d2 },
	{ 0x14e, 0x3d6 }, { 0x157, 0x3d8 }, { 0x164, 0x3e4 }, { 0x171, 0x3ed },
	{ 0x17e, 0x3f6 }, { 0x18b, 0x3ff }, { 0x198, 0x408 }, { 0x1a5, 0x411 },
	{ 0x1ad, 0x416 }, { 0x1ba, 0x41f }, { 0x1c7, 0x428 }, { 0x1d4, 0x431 },
	{ 0x1e1, 0x43a }, { 0x1ee, 0x444 }, { 0x1fb, 0x44d }, { 0x214, 0x45a },
	{ 0x223, 0x468 }, { 0x236, 0x476 }, { 0x245, 0x488 }, { 0x258, 0x495 },
	{ 0x268, 0x4a3 }, { 0x277, 0x4b1 }, { 0x28a, 0x4c3 }, { 0x299, 0x4d1 },
	{ 0x2ac, 0x4de }, { 0x2bb, 0x4ec }, { 0x2cf, 0x4fe }, { 0x2fe, 0x510 },
	{ 0x332, 0x527 }, { 0x362, 0x53e }, { 0x396, 0x555 }, { 0x3c6, 0x56b },
	{ 0x3fa, 0x582 }, { 0x46b, 0x5b9 }, { 0x4dd, 0x5ef }, { 0x54e, 0x626 },
	{ 0x5c4, 0x65c }, { 0x635, 0x693 }, { 0x666, 0x6ce }, { 0x666, 0x6ce },
	{ 0x666, 0x6ce }, { 0x666, 0x6ce }, { 0x666, 0x6ce }, { 0x666, 0x6ce },
	{ 0x666, 0x6ce }, { 0x666, 0x6ce }, { 0x666, 0x6ce }, { 0x666, 0x6ce },
	{ 0x666, 0x6ce }, { 0x666, 0x6ce }, { 0x666, 0x6ce }, { 0x666, 0x6ce },
};

/*
 * Reset the loop state, as the start of the vendor's en7571_init() does.
 * The function-local statics of the vendor (link state, unstable count,
 * SWKT "activated", last TSSI) and the LUT survive a reinitialization
 * there and do so here; they start at zero and at the default table.
 */
void en7571_ctrl_reset(struct en7571 *priv)
{
	priv->kt = 0;
	priv->scl = false;
	priv->dol = false;
	priv->cross = true;
	priv->mbi_delay_us = 0;
	priv->need_cross_ibias = false;
	priv->need_cross_imod = false;
	priv->ibias_now = 0;
	priv->imod_now = 0;
	priv->iav_now = 0;
}

/*
 * Initial state of a newly probed device: the default LUT.
 */
void en7571_ctrl_init(struct en7571 *priv)
{
	static_assert(sizeof(priv->lut) == sizeof(en7571_lut_default));
	memcpy(priv->lut, en7571_lut_default, sizeof(priv->lut));
}

/*
 * Round an integer to the nearest single-precision float, ties to even.
 *
 * Every float the vendor's KT arithmetic produces is a sum or product of
 * integers, so it is an integer itself, and IEEE single precision is just
 * that integer rounded to 24 significant bits.  Applying this after each
 * operation reproduces __aeabi_fadd/fsub/fmul exactly.
 */
static s64 en7571_f32(s64 v)
{
	u64 m = v < 0 ? -(u64)v : (u64)v;
	unsigned int shift;
	u64 rem, half;

	if (m < BIT_ULL(24))
		return v;

	shift = fls64(m) - 24;
	rem = m & (BIT_ULL(shift) - 1);
	half = BIT_ULL(shift - 1);
	m >>= shift;
	if (rem > half || (rem == half && (m & 1)))
		m++;
	m <<= shift;

	return v < 0 ? -(s64)m : (s64)m;
}

/*
 * The Imod target of SWKT and OPKT (vendor, in float):
 *
 *   ratio <  0, KT 1:  Imod_cal * (0x20000 + dIav * LT) >> 17
 *   ratio <  0, KT 2:  Imod_cal * (0x20000 - dIav * LT) >> 17
 *   ratio <  0, KT 3:  Imod_cal * (0x400 + ratio * LT) >> 12
 *   ratio >= 0, KT 3:  Imod_cal * (0x400 + ratio * HT) >> 12
 *   ratio >= 0, else:  Imod_cal * (0x20000 + dIav * HT) >> 17
 *
 * capped at 0xfff, where ratio is Iav as a percentage above calibration and
 * dIav = Iav - Iav_cal.  All inputs are exact in float; each product and sum is
 * rounded as the vendor's soft-float does.  A negative ratio with any KT but 2
 * leaves the target at 0, as in the vendor.
 */
static u32 en7571_kt_target(u8 kt, s32 ratio, s32 iav_delta, u32 imod_cal,
			    u32 factor)
{
	unsigned int shift = EN7571_KT_SHIFT;
	s64 x;

	if (kt == 3) {
		x = en7571_f32(en7571_f32((s64)ratio * factor) +
			       EN7571_KT_ENH_ONE);
		shift = EN7571_KT_ENH_SHIFT;
	} else if (ratio < 0 && kt == 2) {
		x = en7571_f32(EN7571_KT_ONE -
			       en7571_f32((s64)factor * iav_delta));
	} else if (ratio >= 0 || kt == 1) {
		x = en7571_f32(en7571_f32((s64)factor * iav_delta) +
			       EN7571_KT_ONE);
	} else {
		return 0;
	}

	x = en7571_f32(imod_cal * x);

	return min(clamp_t(s64, x, 0, U32_MAX) >> shift, 0xfffU);
}

/* The two KT factors, with the vendor default when flash 0x64 is empty */
static u32 en7571_kt_factors(struct en7571 *priv)
{
	u32 kt = priv->cal[BOB_KT / 4];

	/*
	 * The vendor also writes the default back into the matrix
	 * (set_flash_register); this driver never writes the matrix.
	 */
	return kt == 0xffffffff ? EN7571_KT_DEFAULT : kt;
}

/*
 * Hold or release the loop step sizes in PWR_CTRL_8 bytes [1:0] around the
 * read of a live current, as en7571_info() does.
 */
static int en7571_loop_hold(struct en7571 *priv, bool hold)
{
	u8 buf[4];
	int ret;

	en7571_try(en7571_reg_read(priv, EN7571_PWR_CTRL_8, buf, sizeof(buf)));

	put_unaligned_le16(hold ? EN7571_LOOP_STEP_HOLD : EN7571_LOOP_STEP_RUN,
			   buf);

	return en7571_reg_write(priv, EN7571_PWR_CTRL_8, buf, sizeof(buf));
}

/* One 16-bit half of a 4-byte register */
static int en7571_read_half(struct en7571 *priv, u16 reg, bool high, u32 *val)
{
	u8 buf[4];
	int ret;

	en7571_try(en7571_reg_read(priv, reg, buf, sizeof(buf)));

	*val = get_unaligned_le16(&buf[high ? 2 : 0]);

	return 0;
}

static int en7571_read_held(struct en7571 *priv, u16 reg, bool high,
			    u32 *val)
{
	int ret, restore;

	en7571_try(en7571_loop_hold(priv, true));

	ret = en7571_read_half(priv, reg, high, val);

	restore = en7571_loop_hold(priv, false);

	return ret ? ret : restore;
}

/*
 * Loop readings (vendor en7571_info):
 *
 *   PAV_CAL    PWR_CTRL_D [11:0], the average power target
 *   IAV_NOW    RO_PWR_CTRL_3 [11:0], loop held
 *   IBIAS_NOW  P0_PWR_CTRL_CS3 [27:16], loop held
 *   IMOD_NOW   P1_PWR_CTRL_CS3 [27:16], loop held
 *   P1_CAL     PWR_CTRL_D [25:16], the MPDH target
 *   PAV_NOW    the transmit power sum >> 4
 */
/* transmit power sum, see the definition further down */
static int en7571_tssi_get_locked(struct en7571 *priv, u32 *tssi);

int en7571_info(struct en7571 *priv, int sel, u32 *val)
{
	u32 v = 0;
	int ret;

	switch (sel) {
	case EN7571_SEL_PAV_CAL:
		ret = en7571_read_half(priv, EN7571_PWR_CTRL_D, false, &v);
		v &= 0xfff;
		break;
	case EN7571_SEL_IAV_NOW:
		ret = en7571_read_held(priv, EN7571_RO_PWR_CTRL_3, false, &v);
		v &= 0xfff;
		break;
	case EN7571_SEL_IBIAS_NOW:
		ret = en7571_read_held(priv, EN7571_P0_PWR_CTRL_CS3, true, &v);
		v &= 0xfff;
		break;
	case EN7571_SEL_IMOD_NOW:
		ret = en7571_read_held(priv, EN7571_P1_PWR_CTRL_CS3, true, &v);
		v &= 0xfff;
		break;
	case EN7571_SEL_P1_CAL:
		ret = en7571_read_half(priv, EN7571_PWR_CTRL_D, true, &v);
		v &= 0x3ff;
		break;
	case EN7571_SEL_PAV_NOW:
		ret = en7571_tssi_get_locked(priv, &v);
		v >>= 4;
		break;
	default:
		return -EINVAL;
	}

	if (!ret)
		*val = v;

	return ret;
}

/*
 * Write a DAC code into bytes [1:0] of a PWR_CTRL_CS2 register, keeping the
 * high nibble of byte 1 (vendor en7571_change_Ibias/Imod).  The code is
 * taken as the vendor's 32-bit argument: the low byte, and bits [15:8]
 * ORed into the nibble.
 */
static int en7571_change_dac(struct en7571 *priv, u16 reg, u32 code)
{
	u8 buf[4];
	int ret;

	en7571_try(en7571_reg_read(priv, reg, buf, sizeof(buf)));

	buf[0] = code;
	buf[1] = (buf[1] & 0xf0) | (u8)(code >> 8);

	return en7571_reg_write(priv, reg, buf, sizeof(buf));
}

static int en7571_change_ibias(struct en7571 *priv, u32 code)
{
	return en7571_change_dac(priv, EN7571_P0_PWR_CTRL_CS2, code);
}

static int en7571_change_imod(struct en7571 *priv, u32 code)
{
	return en7571_change_dac(priv, EN7571_P1_PWR_CTRL_CS2, code);
}

/*
 * Write a DAC code across a 256-code boundary (vendor en7571_cross_Imod and
 * en7571_cross_Ibias).  Unlike the source, the stock driver writes the two
 * low bytes whole, without reading the register first, and only the Ibias
 * crossing waits mbi_delay before the burst interface is restarted.
 */
static int en7571_cross_dac(struct en7571 *priv, u16 reg, u32 code,
			    bool delay)
{
	int ret;

	if (!priv->cross)
		return en7571_write_reg(priv, reg, 2, code & 0xffff);

	xpon_burst_ctrl(false);
	ret = en7571_write_reg(priv, reg, 2, code & 0xffff);
	if (delay)
		udelay(priv->mbi_delay_us);
	xpon_burst_ctrl(true);

	return ret;
}

/*
 * Load a KT factor into PWR_CTRL_2 (vendor en7571_tune_KT): byte 1 takes
 * the low byte, and byte 2 takes bits [9:8] over the old byte 1 masked with
 * 0xfc.  That looks like a slip for byte 2 in the vendor code, but it is
 * what the stock driver does (read byte at +1, bic #3, orr, store at +2).
 */
static int en7571_tune_kt(struct en7571 *priv, u16 factor)
{
	u8 buf[4];
	int ret;

	en7571_try(en7571_reg_read(priv, EN7571_PWR_CTRL_2, buf, sizeof(buf)));

	buf[2] = (buf[1] & 0xfc) | (u8)(factor >> 8);
	buf[1] = factor;

	return en7571_reg_write(priv, EN7571_PWR_CTRL_2, buf, sizeof(buf));
}

/* Hardware KT on or off (vendor en7571_HWKT), a 3-byte access */
static int en7571_hwkt(struct en7571 *priv, bool enable)
{
	return en7571_update(priv, EN7571_PWR_CTRL_2, 3, 0,
			     EN7571_IMOD_ADJ_SEL_MASK,
			     enable ? EN7571_IMOD_ADJ_SEL_KT : 0);
}

/*
 * MPDH step sizes (vendor en7571_mpdh_stepsize).  Zero stops the MPDH loop
 * walking; the run value is the one en7571_reg_init() programs.
 */
static int en7571_mpdh_stepsize(struct en7571 *priv, bool enable)
{
	u8 step = enable ? EN7571_STEPSIZE_MPDH : 0x00;
	int ret;

	ret = en7571_rmw_byte(priv, EN7571_PWR_CTRL_3, 2, EN7571_STEPSIZE_MASK,
			      step);
	if (ret)
		return ret;
	ret = en7571_rmw_byte(priv, EN7571_PWR_CTRL_4, 2, EN7571_STEPSIZE_MASK,
			      step);
	if (ret)
		return ret;
	return en7571_rmw_byte(priv, EN7571_PWR_CTRL_4, 0, EN7571_STEPSIZE_MASK,
			       step);
}

/*
 * T0/T1 delay in T1DELAY byte 0 (vendor en7571_T1delay_setting).  While the
 * link is up only the high nibble is kept; restoring takes the fixed delay
 * from flash 0x0c [7:0] or, when that byte is empty, the mode default.
 * The mode is taken from priv->pon_mode rather than
 * from the matrix magic so that a mode the MAC selected at runtime is restored
 * with its own delay.
 */
static int en7571_t1delay_setting(struct en7571 *priv, bool restore)
{
	u32 fixed = priv->cal[BOB_T0CT1C / 4] & 0xff;
	u8 buf[4];
	int ret;

	en7571_try(en7571_reg_read(priv, EN7571_T1DELAY, buf, sizeof(buf)));

	if (!restore)
		buf[0] &= EN7571_T1DELAY_LINK_MASK;
	else if (fixed != 0xff)
		buf[0] = fixed;
	else if (priv->pon_mode == EN7571_PON_GPON)
		buf[0] = EN7571_T1T0_DELAY_GPON;
	else
		buf[0] = EN7571_T1T0_DELAY_OTHER;

	return en7571_reg_write(priv, EN7571_T1DELAY, buf, sizeof(buf));
}

/*
 * Burst control (vendor en7571_burst_ctrl).  The source clears or sets
 * DUMMY byte 1 bit 2 by its argument; the stock driver ignores the argument
 * and always sets it (burst control off), which is what is done here.
 */
static int en7571_burst_ctrl(struct en7571 *priv)
{
	return en7571_rmw_byte(priv, EN7571_DUMMY, 1, EN7571_BURST_CTRL_MASK,
			       EN7571_BURST_CTRL_OFF);
}

/*
 * Arm (enable) or disarm the link registers (vendor en7571_link_reg).
 * Revision 2 freezes the MPDH steps and the T0/T1 delay, turns burst
 * control off and switches hardware KT; other revisions only switch KT and
 * the delay.  The stock driver treats every part it detects as revision 2.
 */
int en7571_link_reg(struct en7571 *priv, bool enable)
{
	int ret;

	en7571_try(en7571_mpdh_stepsize(priv, !enable));
	en7571_try(en7571_t1delay_setting(priv, !enable));
	en7571_try(en7571_burst_ctrl(priv));
	return en7571_hwkt(priv, enable);
}

int en7571_hw_reset(struct en7571 *priv)
{
	/* DUMMY byte 1, taken low and then high again */
	int ret;

	ret = en7571_rmw_byte(priv, EN7571_DUMMY, 1, EN7571_DUMMY_HWRESET_MASK,
			 0x00);
	if (ret)
		return ret;

	return en7571_rmw_byte(priv, EN7571_DUMMY, 1,
				EN7571_DUMMY_HWRESET_MASK,
				EN7571_DUMMY_HWRESET_SET);
}

int en7571_dcl_start(struct en7571 *priv)
{
	/* PWR_CTRL_0 byte 1: release the reset of the dual closed loop */
	return en7571_rmw_byte(priv, EN7571_PWR_CTRL_0, 1, EN7571_PWR_DCL_RST_MASK,
			  EN7571_PWR_DCL_RST_RELEASE);
}

static int en7571_dcl_stop(struct en7571 *priv)
{
	/* PWR_CTRL_0 byte 1: hold the dual closed loop in reset */
	return en7571_rmw_byte(priv, EN7571_PWR_CTRL_0, 1,
			       EN7571_PWR_DCL_RST_MASK, 0);
}

/* Vendor en7571_SingleClosedLoopMode: MPDH loop stopped, DACs from the LUT */
int en7571_single_closed_loop_mode(struct en7571 *priv)
{
	int ret;

	en7571_try(en7571_dcl_stop(priv));
	en7571_try(en7571_mpdh_stepsize(priv, false));
	en7571_try(en7571_hw_reset(priv));
	return en7571_dcl_start(priv);
}

/*
 * Hold (open) or release both DACs (vendor en7571_OpenLoopMode), 1-byte
 * accesses to P0/P1_PWR_CTRL_CS3.
 */
int en7571_open_loop_mode(struct en7571 *priv, bool open)
{
	u8 set = open ? EN7571_OPEN_LOOP : 0;
	int ret;

	ret = en7571_rmw8(priv, EN7571_P0_PWR_CTRL_CS3, EN7571_OPEN_LOOP_MASK,
			  set);
	if (ret)
		return ret;
	return en7571_rmw8(priv, EN7571_P1_PWR_CTRL_CS3, EN7571_OPEN_LOOP_MASK,
			   set);
}

/*
 * Restart the closed loop after link down (vendor en7571_closeloop_reset):
 * loop reset and EN7571 mode off, hardware reset pulse, then both back on.
 */
static int en7571_closeloop_reset(struct en7571 *priv)
{
	int ret;

	ret = en7571_rmw_byte(priv, EN7571_PWR_CTRL_0, 1,
			      EN7571_PWR_DCL_RST_MASK, 0);
	if (ret)
		return ret;
	ret = en7571_rmw_byte(priv, EN7571_PWR_CTRL_0, 0,
			      EN7571_PWR_MODE_MASK, 0);
	if (ret)
		return ret;
	en7571_try(en7571_hw_reset(priv));
	ret = en7571_rmw_byte(priv, EN7571_PWR_CTRL_0, 0,
			      EN7571_PWR_MODE_MASK, EN7571_PWR_MODE_EN7571);
	if (ret)
		return ret;
	return en7571_dcl_start(priv);
}

/* Iav implied by the DAC codes (vendor en7571_Iavg_transfer) */
static u16 en7571_iavg_transfer(struct en7571 *priv)
{
	return (priv->ibias_now * 0x472 + priv->imod_now * 0x200) >> 10;
}

/*
 * Freeze the codes the hardware loop reached at link up and adopt them as
 * the software setpoints (vendor en7571_get_linkup_infor).
 */
static int en7571_get_linkup_info(struct en7571 *priv)
{
	u32 code;
	int ret;

	en7571_try(en7571_info(priv, EN7571_SEL_IMOD_NOW, &code));
	priv->imod_now = code;
	en7571_try(en7571_change_imod(priv, code));

	en7571_try(en7571_info(priv, EN7571_SEL_IBIAS_NOW, &code));
	priv->ibias_now = code;
	en7571_try(en7571_change_ibias(priv, code));

	priv->iav_now = en7571_iavg_transfer(priv);

	return 0;
}

/*
 * Track PON traffic (vendor en7571_islinkstauschange) and set *up to the
 * state the loop runs in.  At link up the transmit power has to be within
 * 3 of the Pav target before the DACs are frozen; after 5 failed attempts
 * they are frozen anyway.  At link down the hardware loop is restarted.
 */
static int en7571_link_status_change(struct en7571 *priv, bool *up_out)
{
	bool up = READ_ONCE(priv->traffic);
	u32 pav_now, pav_cal;
	int ret;

	if (up && !priv->linkup) {
		en7571_try(en7571_info(priv, EN7571_SEL_PAV_NOW, &pav_now));
		en7571_try(en7571_info(priv, EN7571_SEL_PAV_CAL, &pav_cal));

		if (abs((s32)pav_now - (s32)pav_cal) >= EN7571_LINKUP_PAV_ERR) {
			up = false;
			if (++priv->unstable_cnt == EN7571_LINKUP_TRIES) {
				up = true;
				priv->unstable_cnt = 0;
			}
		}

		if (up) {
			ret = en7571_get_linkup_info(priv);
			if (!ret)
				ret = en7571_open_loop_mode(priv, true);
			if (ret)
				return ret;
			dev_dbg(&priv->client->dev, "link up, Pav %u target %u\n",
				pav_now, pav_cal);
		}
	} else if (!up && priv->linkup) {
		ret = en7571_open_loop_mode(priv, false);
		if (!ret)
			ret = en7571_closeloop_reset(priv);
		if (ret)
			return ret;
		priv->unstable_cnt = 0;
		dev_dbg(&priv->client->dev, "link down\n");
	}

	priv->linkup = up;
	*up_out = up;

	return 0;
}

/*
 * Ibias for the next Pav step (vendor en7571_Ibias_hysteresis).  The target
 * is clamped to 0x80..0xf7f and then raised to at least 0x100.  Within the
 * same 256 codes it is taken as is; across a boundary only for a Pav error
 * of at least 0x10, and then as a crossing write.
 *
 * The vendor reads min/max from flash 0x70 inside its debug-print macro, so
 * the stock driver only uses them with debug messages on; the fixed limits
 * are what normally runs and what is used here.
 */
static u32 en7571_ibias_hysteresis(struct en7571 *priv, s32 target,
				   s32 pav_delta)
{
	s32 v = clamp(target, EN7571_IBIAS_MIN, EN7571_IBIAS_MAX);

	if (v <= EN7571_IBIAS_FLOOR)
		v = EN7571_IBIAS_FLOOR;

	if (((v >> 8) & 0xff) == priv->ibias_now >> 8)
		return v;

	if (abs(pav_delta) < EN7571_PAV_HYST)
		return priv->ibias_now;

	priv->need_cross_ibias = true;

	return v;
}

/*
 * Imod for the next KT step (vendor en7571_Imod_hysteresis): the target
 * clamped to 0x32..0xf7f, kept from crossing up until it is 0x11 into the
 * next 256 codes and from crossing down until it is below 0xf0 of the one
 * underneath.  (Flash 0x74 has the same debug-only fate as 0x70.)
 */
static u32 en7571_imod_hysteresis(struct en7571 *priv, u32 now, u32 target)
{
	u32 v = clamp(target, (u32)EN7571_IMOD_MIN, (u32)EN7571_IMOD_MAX);
	u32 hi_now = (now >> 8) & 0xff, hi_v = (v >> 8) & 0xff;

	if (hi_now < hi_v) {
		if ((v & 0xff) < EN7571_IMOD_HYST_UP)
			return now;
	} else if (hi_v < hi_now) {
		if ((v & 0xff) >= EN7571_IMOD_HYST_DN)
			return now;
	} else {
		return v;
	}

	priv->need_cross_imod = true;

	return v;
}

/* Iav as a percentage above calibration, as the vendor's unsigned division */
static s32 en7571_iav_ratio(u32 iav, u32 iav_cal)
{
	return (s32)(iav * 100 / iav_cal) - 100;
}

/*
 * Software KT, while the link is down (vendor en7571_SWKT): load the KT factor
 * for the side of the calibration Iav is on (LT below, HT above) and let the
 * hardware do the rest.
 *
 * The vendor divides by Iav_cal unchecked; a matrix with Iav_cal 0 would fault
 * there and is skipped here.
 */
static int en7571_swkt(struct en7571 *priv)
{
	u32 cal = priv->cal[BOB_IAV_IMOD / 4];
	u32 iav_cal = FIELD_GET(EN7571_FLASH_IAV_MASK, cal);
	u32 factors = en7571_kt_factors(priv);
	u16 ht = factors & 0xffff, lt = factors >> 16;
	u32 iav;
	s32 ratio;
	int ret;

	if (!iav_cal)
		return 0;

	en7571_try(en7571_info(priv, EN7571_SEL_IAV_NOW, &iav));
	ratio = en7571_iav_ratio(iav, iav_cal);

	return en7571_tune_kt(priv, ratio < 0 ? lt : ht);
}

/*
 * KT with the link up (vendor en7571_OPKT): the same target as SWKT, but
 * from the Iav the software setpoints imply, applied to the Imod DAC.
 */
static int en7571_opkt(struct en7571 *priv)
{
	u32 cal = priv->cal[BOB_IAV_IMOD / 4];
	u32 iav_cal = FIELD_GET(EN7571_FLASH_IAV_MASK, cal);
	u32 imod_cal = cal & EN7571_FLASH_IMOD_MASK;
	u32 factors = en7571_kt_factors(priv);
	u16 ht = factors & 0xffff, lt = factors >> 16;
	u32 imod, target, code;
	s32 ratio;
	int ret;

	if (!READ_ONCE(priv->traffic) || !iav_cal)
		return 0;

	ratio = en7571_iav_ratio(priv->iav_now, iav_cal);
	target = en7571_kt_target(priv->kt, ratio,
				  (s32)priv->iav_now - (s32)iav_cal,
				  imod_cal, ratio < 0 ? lt : ht);

	en7571_try(en7571_info(priv, EN7571_SEL_IMOD_NOW, &imod));
	priv->imod_now = imod;

	code = en7571_imod_hysteresis(priv, imod, target);
	if (priv->need_cross_imod) {
		ret = en7571_cross_dac(priv, EN7571_P1_PWR_CTRL_CS2, code, false);
		priv->need_cross_imod = false;
	} else {
		ret = en7571_change_imod(priv, code);
	}
	if (ret)
		return ret;

	priv->imod_now = code;
	priv->iav_now = en7571_iavg_transfer(priv);

	dev_dbg(&priv->client->dev, "OPKT target 0x%03x Imod 0x%03x\n",
		target, code);

	return 0;
}

/*
 * Average power loop with the link up (vendor en7571_Pavg_close_loop):
 * Ibias moves 4 codes towards the Pav target per call.
 */
static int en7571_pavg_close_loop(struct en7571 *priv)
{
	u32 pav_cal, pav_now, ibias, code;
	s32 delta, target;
	int ret;

	if (!READ_ONCE(priv->traffic))
		return 0;

	en7571_try(en7571_info(priv, EN7571_SEL_PAV_CAL, &pav_cal));
	en7571_try(en7571_info(priv, EN7571_SEL_PAV_NOW, &pav_now));
	delta = (s32)pav_cal - (s32)pav_now;

	en7571_try(en7571_info(priv, EN7571_SEL_IBIAS_NOW, &ibias));
	priv->ibias_now = ibias;

	target = ibias;
	if (delta < 0)
		target -= EN7571_IBIAS_STEP;
	else if (delta > 0)
		target += EN7571_IBIAS_STEP;

	code = en7571_ibias_hysteresis(priv, target, delta);
	if (priv->need_cross_ibias) {
		ret = en7571_cross_dac(priv, EN7571_P0_PWR_CTRL_CS2, code, true);
		priv->need_cross_ibias = false;
	} else {
		ret = en7571_change_ibias(priv, code);
	}
	if (ret)
		return ret;

	priv->ibias_now = code;
	priv->iav_now = en7571_iavg_transfer(priv);

	return 0;
}

/* Linear interpolation between two 12-bit flash points, vendor rounding */
static s32 en7571_lut_interp(u32 lo, u32 hi, s32 frac)
{
	return (u16)((s16)(((s32)hi - (s32)lo) * frac / 10) + lo);
}

/* The two flash points around segment seg (0..15) */
static void en7571_lut_points(struct en7571 *priv, unsigned int seg,
			      u32 *lo, u32 *hi)
{
	*lo = priv->cal[BOB_LUT / 4 + seg];
	*hi = priv->cal[BOB_LUT / 4 + min(seg + 1, BOB_LUT_NR - 1U)];
}

/*
 * Rebuild the LUT from flash (vendor en7571_LUT_recover).
 *
 * The flash LUT holds Ibias/Imod every 10 C from -40 C.  Both are offset so
 * that, interpolated at the calibration temperature (flash 0x8c, 0.1 C),
 * they equal the codes in flash 0x08, and then interpolated onto 64 points
 * 2.5 C apart.  The vendor works in whole degrees: each point's
 * temperature is (25 * k - 400) / 10 truncated, and so is everything here.
 * A calibration temperature outside -40..110 C leaves the table as it was.
 *
 * Empty fields are not checked, as in stock: an empty 0x8c
 * reads as -1 / 10 = 0 C and passes the range test.  With empty LUT words,
 * as on a matrix that has none, every entry comes out 0xfff, so
 * en7571_lut_tracking() never writes the DACs and there is no temperature
 * compensation; the offsets in the debug message are then meaningless.
 * This is only warned about, the table is rebuilt as stock does.
 */
void en7571_lut_recover(struct en7571 *priv)
{
	u32 raw = priv->cal[BOB_TEMP_CAL / 4];
	s32 tcal = (s32)raw / 10;
	u32 point = priv->cal[BOB_IBIAS_IMOD / 4];
	s32 ibias_off, imod_off, t, frac;
	unsigned int seg, k;
	u32 lo, hi;

	if (raw == 0xffffffff || point == 0xffffffff)
		dev_warn(&priv->client->dev,
			 "calibration temperature or Ibias/Imod point empty, the rebuilt LUT is not usable\n");

	if (tcal < EN7571_TCAL_MIN || tcal > EN7571_TCAL_MAX) {
		dev_warn(&priv->client->dev,
			 "calibration temperature %d C out of range, LUT not rebuilt\n",
			 tcal);
		return;
	}

	seg = (tcal + 40) / 10;
	frac = tcal + 40 - 10 * seg;
	en7571_lut_points(priv, seg, &lo, &hi);
	imod_off = (s32)(point & 0xfff) -
		   en7571_lut_interp(lo & 0xfff, hi & 0xfff, frac);
	ibias_off = (s32)((point >> 16) & 0xfff) -
		    en7571_lut_interp((lo >> 16) & 0xfff, (hi >> 16) & 0xfff,
				      frac);

	for (k = 0; k < EN7571_LUT_NR; k++) {
		t = ((s32)k * 25 - 400) / 10;
		seg = min((t + 40) / 10, BOB_LUT_NR - 1);
		frac = t + 40 - 10 * seg;
		en7571_lut_points(priv, seg, &lo, &hi);

		priv->lut[k][0] = en7571_lut_interp((lo >> 16) & 0xfff,
						    (hi >> 16) & 0xfff, frac) +
				  ibias_off;
		priv->lut[k][1] = en7571_lut_interp(lo & 0xfff, hi & 0xfff,
						    frac) + imod_off;
	}

	dev_dbg(&priv->client->dev, "LUT rebuilt at %d C, offsets Ibias %d Imod %d\n",
		tcal, ibias_off, imod_off);
}

/*
 * Ambient temperature in millidegrees (vendor en7571_Env_temperature): the
 * die temperature less flash 0x24 [31:16] / 10 as a signed byte, 10 C when
 * that half is empty.
 */
s32 en7571_env_temp_md(struct en7571 *priv)
{
	u32 off = priv->cal[BOB_TEMP_OFFSET / 4] >> 16;
	s32 env_off = off != 0xffff ? (s8)(off / 10) : EN7571_ENV_OFFSET_DEFAULT;

	return priv->temp_millideg - env_off * 1000;
}

/*
 * Apply the LUT entry for the ambient temperature (vendor
 * en7571_LUT_Tracking).  The vendor's index is (T + 40.0) / 2.5 truncated
 * toward zero, i.e. (T_md + 40000) / 2500 in C integer division, clamped to
 * the table.  Only open-loop mode writes the DACs, and only from a complete
 * entry.
 */
int en7571_lut_tracking(struct en7571 *priv)
{
	s32 idx, ibias, imod;
	int ret;

	if (priv->scl || !priv->dol)
		return 0;

	idx = (en7571_env_temp_md(priv) - EN7571_LUT_T0_MD) /
	      EN7571_LUT_STEP_MD;
	idx = clamp(idx, 0, EN7571_LUT_NR - 1);

	ibias = priv->lut[idx][0];
	imod = priv->lut[idx][1];
	if (ibias == EN7571_LUT_EMPTY || imod == EN7571_LUT_EMPTY)
		return 0;

	en7571_try(en7571_change_ibias(priv, ibias));

	return en7571_change_imod(priv, imod);
}

/*
 * The control part of the vendor's 1 s en7571_internal_clock(), after the
 * DDMI part: LUT tracking on the 7th second of ten in single closed or open
 * loop mode; otherwise SWKT every second while the link is down, and with
 * it up OPKT on the 7th second and the Pav loop on the others.
 */
int en7571_ctrl_tick(struct en7571 *priv, unsigned int slot)
{
	bool up;
	/* the tenth second of ten with neither KT nor the Pav loop does nothing */
	int ret = 0;

	if (priv->scl || priv->dol)
		return slot % 10 == 7 ? en7571_lut_tracking(priv) : 0;

	en7571_try(en7571_link_status_change(priv, &up));

	if (!up)
		return priv->kt ? en7571_swkt(priv) : 0;

	if (slot % 10 == 7)
		ret = priv->kt ? en7571_opkt(priv) : 0;
	else
		ret = en7571_pavg_close_loop(priv);

	if (slot % 5 == 2)
		dev_dbg(&priv->client->dev, "Ibias 0x%x Imod 0x%x Iav 0x%x\n",
			priv->ibias_now, priv->imod_now, priv->iav_now);

	return ret;
}

/*
 * Report PON traffic up or down (vendor phy_set_traffic_status, which sets
 * the trafficStatus flag the loop reads).  The loop switches between the
 * hardware closed loop and the software loops on it, and the transmit
 * power is only measured while it is set.  Lock-free, any context.
 */
void en7571_set_traffic(struct en7571 *priv, bool up)
{
	if (priv) {
		if (READ_ONCE(priv->traffic) != up)
			en7571_dbg(priv, EN7571_DBG_OPS, "traffic %s\n",
				   up ? "up" : "down");
		WRITE_ONCE(priv->traffic, up);
	}
}

/*
 * Reset the safe circuit, which latches a transmitter fault (vendor
 * en7571_safe_circuit_reset): SAFE_PROTECT byte 1 bit 6.  This is a register
 * of the laser on I2C, not of the xPON PHY.  It sleeps.
 */
int en7571_safe_circuit_reset_locked(struct en7571 *priv)
{
	return en7571_rmw_byte(priv, EN7571_SAFE_PROTECT, 1,
			       EN7571_SAFE_CIRCUIT_MASK,
			       EN7571_SAFE_CIRCUIT_RESET);
}

int en7571_safe_circuit_reset(struct en7571 *priv)
{
	int ret;

	if (!priv)
		return -EINVAL;

	mutex_lock(&priv->lock);
	ret = en7571_safe_circuit_reset_locked(priv);
	mutex_unlock(&priv->lock);
	en7571_dbg(priv, EN7571_DBG_OPS, "safe circuit reset: %d\n", ret);
	if (en7571_debug & EN7571_DBG_DUMP)
		en7571_dump(priv);

	return ret;
}

/*
 * EN7571 GPON laser - Power monitoring and PWRADC functions
 *
 * Everything in this file is called with priv->lock held.
 */

/*
 * One PWRADC conversion, the core of vendor en7571_TSSI_get: trigger in
 * RG_PWRADC_DATA2 byte 3, wait 20 ms, and if byte 2 bit 4 says the sum is
 * valid read the register again and take the 20-bit sum shifted down by 4.
 * *valid tells the caller whether *adc holds a sample.
 *
 * The accumulation count is not touched here: the vendor sets it to 1024 once,
 * in en7571_PWRADC_calibration(), and never resets it, so every later
 * conversion (DDMI and the Pav loop alike) sums 1024 samples.
 */
static int en7571_pwradc_convert(struct en7571 *priv, u32 *adc, bool *valid)
{
	u32 val;
	int ret;

	*valid = false;

	ret = en7571_rmw_byte(priv, EN7571_RG_PWRADC_DATA2, 3,
			      EN7571_PWRADC_TRIG_MASK, EN7571_PWRADC_TRIG);
	if (ret)
		return ret;

	msleep(PWRADC_CAL_DELAY_MS);

	en7571_try(en7571_read_reg(priv, EN7571_RG_PWRADC_DATA2, 4, &val));

	if ((val >> 16) & EN7571_PWRADC_VALID) {
		en7571_try(en7571_read_reg(priv, EN7571_RG_PWRADC_DATA2, 4, &val));
		*adc = (val & EN7571_PWRADC_SUM_MASK) >> 4;
		*valid = true;
	}

	return 0;
}

/*
 * PWRADC dark calibration (vendor en7571_PWRADC_calibration), at init with
 * the laser off: RG_PWRK1 is set for the conversion and cleared after it,
 * and the count is set to 1024 and left there.  A conversion that never
 * becomes valid leaves the offset at 0, as the vendor does, and is only
 * reported.
 */
int en7571_pwradc_calibration(struct en7571 *priv)
{
	bool valid;
	u32 adc = 0;
	int ret, restore;

	ret = en7571_rmw_byte(priv, EN7571_RG_ANA_CTRL1, 3, EN7571_PWRK1_MASK,
			      EN7571_PWRK1_1);
	if (ret)
		return ret;

	ret = en7571_rmw_byte(priv, EN7571_RG_PWRADC_DATA2, 3,
			      EN7571_PWRADC_COUNT_MASK,
			      EN7571_PWRADC_COUNT_1024);
	if (!ret)
		ret = en7571_pwradc_convert(priv, &adc, &valid);

	restore = en7571_rmw_byte(priv, EN7571_RG_ANA_CTRL1, 3,
				  EN7571_PWRK1_MASK, 0);
	if (ret || restore)
		return ret ? ret : restore;

	if (valid)
		priv->pwradc_offset = adc;
	else
		dev_warn(&priv->client->dev,
			 "PWRADC dark conversion not valid, offset left at %u\n",
			 priv->pwradc_offset);

	return 0;
}

/*
 * Transmit power sum (vendor en7571_TSSI_get).  A conversion that is not
 * valid returns the last valid one, as the vendor's static does.
 */
static int en7571_tssi_get_locked(struct en7571 *priv, u32 *tssi)
{
	bool valid;
	u32 adc;
	int ret;

	en7571_try(en7571_pwradc_convert(priv, &adc, &valid));

	if (valid)
		priv->tssi = adc;
	*tssi = priv->tssi;

	return 0;
}

/*
 * PWRADC read (vendor en7571_PWRADC_get).  The vendor only converts while PON
 * traffic is up (or the factory pattern runs) and otherwise reports the dark
 * offset, i.e. no transmit power; anything below the dark offset is reported as
 * the offset.
 */
static int en7571_pwradc_get_locked(struct en7571 *priv, u32 *pwradc)
{
	u32 adc;
	int ret;

	if (!READ_ONCE(priv->traffic)) {
		*pwradc = priv->pwradc_offset;
		return 0;
	}

	en7571_try(en7571_tssi_get_locked(priv, &adc));

	*pwradc = max(adc, priv->pwradc_offset);

	return 0;
}

/*
 * Tx power (vendor en7571_TxPower_get_8472).
 *
 * A straight line from (dark offset, 0) to the single calibration point in
 * flash 0x40, whose high half is the power in 0.01 mW and low half the PWRADC
 * code:
 *
 *   tx_now = (PWRADC_now - offset) * tx_up / (PWRADC_up - offset)  [0.01 mW]
 *   8472   = (int)(10 * tx_now) / 0.1                              [0.1 uW]
 *
 * i.e. floor(10 * tx_now) * 10.  Without a usable point the result is 0.  The
 * vendor returns a 16-bit value and wraps; this saturates.  The product is
 * below 2^16 * 2^16 * 10 and fits in u64.
 */
int en7571_calc_tx_power_locked(struct en7571 *priv, u32 *tx_power_8472)
{
	u32 flash = priv->cal[BOB_DDMI_TX_P1 / 4];
	u32 now, up = 0, tx_up = 0;
	u64 tenth;
	int ret;

	en7571_try(en7571_pwradc_get_locked(priv, &now));

	if (flash != 0xffffffff) {
		up = flash & FLASH_MPD_ADC_MASK;
		tx_up = (flash & FLASH_TX_POWER_MASK) >> FLASH_TX_POWER_OFFSET;
	}

	/*
	 * In open-loop mode the stock driver reports the calibrated power
	 * itself while traffic is up, whatever the detector says:
	 * (int)(10 * tx_up) / 0.1 is exactly tx_up * 100.
	 */
	if (priv->dol && READ_ONCE(priv->traffic)) {
		*tx_power_8472 = min_t(u32, tx_up * 100, 0xffff);
		return 0;
	}

	if (up <= priv->pwradc_offset) {
		*tx_power_8472 = 0;
		return 0;
	}

	tenth = div_u64((u64)(now - priv->pwradc_offset) * tx_up * 10,
			up - priv->pwradc_offset);
	*tx_power_8472 = (u32)min_t(u64, tenth * 10, 0xffff);

	return 0;
}

/*
 * EN7571 GPON laser - APD bias supply control
 */

/*
 * Bring the APD bias converter up.
 *
 * Soft start is enabled before the converter itself, in two separate
 * transactions, so the output ramps instead of stepping to whatever the code
 * happens to be.  Skipping the order risks a step large enough to damage the
 * photodiode, which is the one part of this module where the wrong sequence
 * is not self-correcting.  Called with priv->lock held.
 */
int en7571_apd_init(struct en7571 *priv)
{
	int ret = en7571_rmw_byte(priv, EN7571_APD_DAC_CODE, 2, 0xff,
				  EN7571_APD_SOFTSTART_ENABLE);

	return ret ? ret : en7571_rmw_byte(priv, EN7571_APD_DAC_CODE, 1, 0xff,
					   EN7571_APD_CONTROL_ENABLE);
}

/*
 * Track the APD bias with temperature.  The target voltage moves with the case
 * temperature - the vendor raises it above 25 C and lowers it below, with
 * separate slopes, because a reverse biased photodiode draws more current as it
 * heats - and is then mapped onto the converter code through a four-segment
 * ramp with 64 steps per segment.
 *
 * All integer: voltages in millivolts, slopes per degree, temperature in
 * millidegrees, so the products stay inside 64 bits with no rounding beyond the
 * ramp's own division.  The vendor divides twice, for the segment step and for
 * the position within it; multiplying by the segment length first skips the
 * first truncation and gives the same code.
 *
 * The segment voltages come from flash only when (0x30 | 0x34) != 0xffffffff,
 * which is the vendor's test.  A voltage below the 0x00 point or a code past
 * 0xff wraps in the vendor's u8; both are clamped here.
 *
 * Called with priv->lock held.
 */
int en7571_apd_set_locked(struct en7571 *priv, s32 bosa_millideg)
{
	s32 v_nt, v_00, v_40, v_80, v_c0, slope_up, slope_dn;
	s32 target, delta, span;
	u32 code;
	int ret;

	if (priv->cal[BOB_APD_CHANGE_POINT / 4] != 0xffffffff)
		v_nt = (s32)priv->cal[BOB_APD_CHANGE_POINT / 4] * 10;
	else
		v_nt = EN7571_APD_V_NT_DEFAULT;

	slope_up = priv->cal[BOB_APD_SLOPE_UP / 4] != 0xffffffff ?
		   (s32)priv->cal[BOB_APD_SLOPE_UP / 4] * 10 :
		   EN7571_APD_SLOPE_UP_DEFAULT;
	slope_dn = priv->cal[BOB_APD_SLOPE_DN / 4] != 0xffffffff ?
		   (s32)priv->cal[BOB_APD_SLOPE_DN / 4] * 10 :
		   EN7571_APD_SLOPE_DN_DEFAULT;

	if ((priv->cal[BOB_APD_VOLTAGE_HI / 4] |
	     priv->cal[BOB_APD_VOLTAGE_LO / 4]) != 0xffffffff) {
		u32 hi = priv->cal[BOB_APD_VOLTAGE_HI / 4];
		u32 lo = priv->cal[BOB_APD_VOLTAGE_LO / 4];

		v_00 = ((hi >> 16) & 0xffff) * 100;
		v_40 = (hi & 0xffff) * 100;
		v_80 = ((lo >> 16) & 0xffff) * 100;
		v_c0 = (lo & 0xffff) * 100;
	} else {
		v_00 = EN7571_APD_V_00_DEFAULT;
		v_40 = EN7571_APD_V_40_DEFAULT;
		v_80 = EN7571_APD_V_80_DEFAULT;
		v_c0 = EN7571_APD_V_C0_DEFAULT;
	}

	if (v_40 <= v_00 || v_80 <= v_40 || v_c0 <= v_80)
		return -EINVAL;

	if (bosa_millideg > EN7571_APD_T_PIVOT_MD)
		target = v_nt + div_s64((s64)slope_up *
					(bosa_millideg - EN7571_APD_T_PIVOT_MD),
					1000);
	else
		target = v_nt - div_s64((s64)slope_dn *
					(EN7571_APD_T_PIVOT_MD - bosa_millideg),
					1000);
	priv->apd_mv = target;

	if (target < v_00) {
		code = 0;
	} else if (target < v_40) {
		delta = target - v_00;
		span = v_40 - v_00;
		code = div_s64((s64)delta * EN7571_APD_SEGMENT_BASE, span);
	} else if (target < v_80) {
		delta = target - v_40;
		span = v_80 - v_40;
		code = EN7571_APD_SEGMENT_BASE +
		       div_s64((s64)delta * EN7571_APD_SEGMENT_BASE, span);
	} else if (target < v_c0) {
		delta = target - v_80;
		span = v_c0 - v_80;
		code = 2 * EN7571_APD_SEGMENT_BASE +
		       div_s64((s64)delta * EN7571_APD_SEGMENT_BASE, span);
	} else {
		delta = target - v_c0;
		span = v_c0 - v_80;
		code = 3 * EN7571_APD_SEGMENT_BASE +
		       div_s64((s64)delta * EN7571_APD_SEGMENT_BASE, span);
		code = min(code, 0xffU);
	}

	ret = en7571_write_reg(priv, EN7571_APD_DAC_CODE, 1, code);
	if (!ret)
		dev_dbg(&priv->client->dev, "APD %d mV at BOSA %d mC, code 0x%02x\n",
			target, bosa_millideg, code);

	return ret;
}

/*
 * EN7571 GPON laser - RSSI (receive signal strength) functions
 *
 * Everything in this file is called with priv->lock held.
 */

/* Gain factor per RSSI gain setting 0..5 (vendor en7571_RSSI_get switch) */
static const u32 en7571_rssi_gain_scale[] = {
	1, 4, 16, 64, 128, 256,
};

/*
 * Calibrate the receive signal strength path once, at init (vendor
 * en7571_RSSI_calibration).
 *
 * With RSSI calibration and V mode on in LA_PWD byte 1, the SVADC switched to
 * the RSSI input reads RSSI_Vref, the reference every later reading is measured
 * against.  V mode is then turned off and a second sample taken, calibration off
 * and the select byte restored.  The vendor uses that second sample only for a
 * factor it never reads again, so it is taken and dropped here too.
 */
int en7571_rssi_calibrate(struct en7571 *priv)
{
	u32 saved, raw;
	int ret, restore;

	ret = en7571_update(priv, EN7571_LA_PWD, 2, 1, EN7571_RSSI_CAL_MASK,
			    EN7571_RSSI_CAL_ENABLE);
	if (ret)
		return ret;

	ret = en7571_update(priv, EN7571_LA_PWD, 2, 1, EN7571_RSSI_CAL_SEL_MASK,
			    EN7571_RSSI_CAL_SEL);
	if (ret)
		goto out_cal;

	en7571_try_goto(en7571_read_reg(priv, EN7571_SVADC_PD, 1, &saved), out_vmode);

	ret = en7571_write_reg(priv, EN7571_SVADC_PD, 1,
			       (saved & EN7571_ADC_SELECT_MASK) |
			       EN7571_ADC_SEL_RSSI);
	if (!ret)
		ret = en7571_svadc_get(priv, &priv->rssi_vref);

	restore = en7571_update(priv, EN7571_LA_PWD, 2, 1,
				EN7571_RSSI_CAL_SEL_MASK, 0);
	if (!ret)
		ret = restore;
	if (!ret)
		ret = en7571_svadc_get(priv, &raw);

	restore = en7571_update(priv, EN7571_LA_PWD, 2, 1,
				EN7571_RSSI_CAL_MASK, 0);
	if (!ret)
		ret = restore;

	restore = en7571_write_reg(priv, EN7571_SVADC_PD, 1, saved);

	if (!ret)
		dev_dbg(&priv->client->dev, "RSSI_Vref = 0x%x\n",
			priv->rssi_vref);

	return ret ? ret : restore;

out_vmode:
	en7571_update(priv, EN7571_LA_PWD, 2, 1, EN7571_RSSI_CAL_SEL_MASK, 0);
out_cal:
	en7571_update(priv, EN7571_LA_PWD, 2, 1, EN7571_RSSI_CAL_MASK, 0);
	return ret;
}

/*
 * Measure the receive signal strength (vendor en7571_RSSI_get).
 *
 * The SVADC is switched to the RSSI input and the ADLCH accumulator set to 1024
 * samples.  The gain in LA_PWD byte 2 is walked down from 5 until the reading
 * drops below RSSI_Vref minus the noise guard; each step triggers the
 * accumulator, waits 20 ms and, if byte 2 bit 7 says the sum is valid, takes
 * the 20-bit sum shifted down by 10.  The comparison is unsigned, as in the
 * vendor.  Gain is put back to 5 and the select byte restored.
 *
 * The result is (RSSI_Vref - reading) times the gain factor, or 0.  The
 * accumulator count is left at 1024: the vendor's "count back to default" step
 * writes back the byte it read unchanged.
 */
int en7571_read_rssi_locked(struct en7571 *priv, u32 *rssi)
{
	u32 saved, val, voltage = 0;
	unsigned int gain = EN7571_RSSI_GAIN_MAX;
	unsigned int step;
	int ret, restore;

	en7571_try(en7571_read_reg(priv, EN7571_SVADC_PD, 1, &saved));

	ret = en7571_write_reg(priv, EN7571_SVADC_PD, 1,
			       (saved & EN7571_ADC_SELECT_MASK) |
			       EN7571_ADC_SEL_RSSI);
	if (ret)
		goto out;

	ret = en7571_rmw_byte(priv, EN7571_RG_ADLCH_CTRL, 3,
			      EN7571_ADLCH_COUNT_MASK, EN7571_ADLCH_COUNT_1024);
	if (ret)
		goto out;

	for (step = 0; step <= EN7571_RSSI_GAIN_MAX; step++) {
		gain = EN7571_RSSI_GAIN_MAX - step;

		ret = en7571_rmw_byte(priv, EN7571_LA_PWD, 2,
				      EN7571_RSSI_GAIN_MASK, gain);
		if (ret)
			goto out_gain;

		ret = en7571_rmw_byte(priv, EN7571_RG_ADLCH_CTRL, 3,
				      EN7571_ADLCH_TRIG_MASK, EN7571_ADLCH_TRIG);
		if (ret)
			goto out_gain;

		msleep(ADLCH_CONV_DELAY_MS);

		en7571_try_goto(en7571_read_reg(priv, EN7571_RG_ADLCH_CTRL, 4, &val), out_gain);

		if ((val >> 16) & EN7571_ADLCH_VALID) {
			ret = en7571_read_reg(priv, EN7571_RG_ADLCH_CTRL, 4,
					      &val);
			if (ret)
				goto out_gain;
			voltage = (val & EN7571_ADLCH_SUM_MASK) >> 10;
		}

		/*
		 * Unsigned, as in the vendor - but only where the guard
		 * itself fits: a reference below the noise guard would wrap
		 * this round to about four billion, never match, and send the
		 * walk through every gain before it gave up.
		 */
		if (priv->rssi_vref >= EN7571_RSSI_NOISE_GUARD &&
		    voltage < priv->rssi_vref - EN7571_RSSI_NOISE_GUARD)
			break;
	}

out_gain:
	restore = en7571_rmw_byte(priv, EN7571_LA_PWD, 2, EN7571_RSSI_GAIN_MASK,
				  EN7571_RSSI_GAIN_MAX);
	if (!ret)
		ret = restore;
out:
	restore = en7571_write_reg(priv, EN7571_SVADC_PD, 1, saved);
	if (!ret)
		ret = restore;
	if (ret)
		return ret;

	if (priv->rssi_vref >= voltage)
		*rssi = (priv->rssi_vref - voltage) *
			en7571_rssi_gain_scale[gain];
	else
		*rssi = 0;

	return 0;
}

/*
 * Quadratic Rx power (vendor en7571_RxPower_get_8472 when flash 0x5c low
 * nibble is 1): the three flash points (RSSI, power) define
 * P = x*R^2 + y*R + z with x = e/d, y = f/d, z = g/d by Cramer's rule.  The
 * vendor evaluates this in floats; here it is (e*R^2 + f*R + g) / d in s64
 * with every step checked, and a curve that does not fit is reported as an
 * error rather than a wrapped number.
 */
static bool en7571_mul(s64 *acc, s64 a, s64 b)
{
	return check_mul_overflow(a, b, acc);
}

static bool en7571_term(s64 *sum, s64 a, s64 b, s64 c, s64 d, bool neg)
{
	s64 t;

	if (en7571_mul(&t, a, b) || en7571_mul(&t, t, c) ||
	    en7571_mul(&t, t, d))
		return true;
	if (neg)
		return check_sub_overflow(*sum, t, sum);
	return check_add_overflow(*sum, t, sum);
}

static int en7571_rx_power_quadratic(struct en7571 *priv, u32 rssi_now,
				     s64 *power)
{
	const u32 *cal = priv->cal;
	s64 r1 = cal[BOB_DDMI_RX_P1 / 4] & FLASH_RX_RSSI_MASK;
	s64 r2 = cal[BOB_DDMI_RX_P2 / 4] & FLASH_RX_RSSI_MASK;
	s64 r3 = cal[BOB_DDMI_RX_P3 / 4] & FLASH_RX_RSSI_MASK;
	s64 k1 = (cal[BOB_DDMI_RX_P1 / 4] & FLASH_RX_POWER_MASK) >> FLASH_RX_POWER_OFFSET;
	s64 k2 = (cal[BOB_DDMI_RX_P2 / 4] & FLASH_RX_POWER_MASK) >> FLASH_RX_POWER_OFFSET;
	s64 k3 = (cal[BOB_DDMI_RX_P3 / 4] & FLASH_RX_POWER_MASK) >> FLASH_RX_POWER_OFFSET;
	s64 r = rssi_now;
	s64 d = 0, e = 0, f = 0, g = 0, num = 0, t;
	bool ovf = false;

	/* d = r1^2 r2 + r1 r3^2 + r2^2 r3 - r2 r3^2 - r1 r2^2 - r1^2 r3 */
	ovf |= en7571_term(&d, r1, r1, r2, 1, false);
	ovf |= en7571_term(&d, r1, r3, r3, 1, false);
	ovf |= en7571_term(&d, r2, r2, r3, 1, false);
	ovf |= en7571_term(&d, r2, r3, r3, 1, true);
	ovf |= en7571_term(&d, r1, r2, r2, 1, true);
	ovf |= en7571_term(&d, r1, r1, r3, 1, true);
	/* e = k1 r2 + r1 k3 + k2 r3 - r2 k3 - r1 k2 - k1 r3 */
	ovf |= en7571_term(&e, k1, r2, 1, 1, false);
	ovf |= en7571_term(&e, r1, k3, 1, 1, false);
	ovf |= en7571_term(&e, k2, r3, 1, 1, false);
	ovf |= en7571_term(&e, r2, k3, 1, 1, true);
	ovf |= en7571_term(&e, r1, k2, 1, 1, true);
	ovf |= en7571_term(&e, k1, r3, 1, 1, true);
	/* f = r1^2 k2 + k1 r3^2 + r2^2 k3 - k2 r3^2 - k1 r2^2 - r1^2 k3 */
	ovf |= en7571_term(&f, r1, r1, k2, 1, false);
	ovf |= en7571_term(&f, k1, r3, r3, 1, false);
	ovf |= en7571_term(&f, r2, r2, k3, 1, false);
	ovf |= en7571_term(&f, k2, r3, r3, 1, true);
	ovf |= en7571_term(&f, k1, r2, r2, 1, true);
	ovf |= en7571_term(&f, r1, r1, k3, 1, true);
	/* g = r1^2 r2 k3 + r1 k2 r3^2 + k1 r2^2 r3
	 *   - k1 r2 r3^2 - r1 r2^2 k3 - r1^2 k2 r3
	 */
	ovf |= en7571_term(&g, r1, r1, r2, k3, false);
	ovf |= en7571_term(&g, r1, k2, r3, r3, false);
	ovf |= en7571_term(&g, k1, r2, r2, r3, false);
	ovf |= en7571_term(&g, k1, r2, r3, r3, true);
	ovf |= en7571_term(&g, r1, r2, r2, k3, true);
	ovf |= en7571_term(&g, r1, r1, k2, r3, true);
	if (ovf)
		return -ERANGE;

	if (!d)
		return -EINVAL;

	/* num = e r^2 + f r + g */
	ovf |= en7571_term(&num, e, r, r, 1, false);
	ovf |= en7571_term(&num, f, r, 1, 1, false);
	ovf |= check_add_overflow(num, g, &num);
	if (ovf)
		return -ERANGE;

	t = div64_s64(num, d);
	*power = t > 0 ? t : 0;

	return 0;
}

/*
 * Three-line Rx power (vendor en7571_RxPower_get_8472, default branch).
 *
 * Flash 0x50, 0x54 and 0x58 are the up, middle and down points, each with the
 * power in 0.01 uW in the high half and the RSSI current in the low half; a
 * missing point counts as zero, as in the vendor.
 *
 *   zone 1  R <= dn        P = R * Pdn / Rdn
 *   zone 2  dn < R < md    P = (R - Rdn) * (Pmd - Pdn) / (Rmd - Rdn) + Pdn
 *   zone 3  md <= R        P = (R - Rmd) * (Pup - Pmd) / (Rup - Rmd) + Pmd
 *
 * A zero divisor leaves the power at 0, like the vendor.  The differences are
 * signed here; the vendor computes them in u32 and wraps on a non-monotonic
 * calibration.
 */
static void en7571_rx_point(const u32 *cal, unsigned int off, s64 *r, s64 *p)
{
	u32 v = cal[off / 4];

	if (v == 0xffffffff) {
		*r = 0;
		*p = 0;
		return;
	}

	*r = v & FLASH_RX_RSSI_MASK;
	*p = (v & FLASH_RX_POWER_MASK) >> FLASH_RX_POWER_OFFSET;
}

static s64 en7571_rx_power_lines(struct en7571 *priv, u32 rssi_now)
{
	s64 r_up, r_md, r_dn, p_up, p_md, p_dn;
	s64 r = rssi_now, p = 0;

	en7571_rx_point(priv->cal, BOB_DDMI_RX_P1, &r_up, &p_up);
	en7571_rx_point(priv->cal, BOB_DDMI_RX_P2, &r_md, &p_md);
	en7571_rx_point(priv->cal, BOB_DDMI_RX_P3, &r_dn, &p_dn);

	if (r <= r_dn) {
		if (r_dn)
			p = div64_s64(r * p_dn, r_dn);
	} else if (r < r_md) {
		if (r_md != r_dn)
			p = div64_s64((r - r_dn) * (p_md - p_dn),
				      r_md - r_dn) + p_dn;
	} else {
		if (r_up != r_md)
			p = div64_s64((r - r_md) * (p_up - p_md),
				      r_up - r_md) + p_md;
	}

	return p > 0 ? p : 0;
}

/*
 * Rx power in SFF-8472 units of 0.1 uW from an RSSI reading: the curve
 * gives 0.01 uW and the vendor divides by 10.  The vendor returns 16 bits
 * and wraps; this saturates.
 */
int en7571_calc_rx_power_locked(struct en7571 *priv, u32 rssi,
				u32 *rx_power_8472)
{
	s64 p;
	int ret;

	if ((priv->cal[BOB_DDMI_RX_P4 / 4] & 0xf) == 1) {
		en7571_try(en7571_rx_power_quadratic(priv, rssi, &p));
	} else {
		p = en7571_rx_power_lines(priv, rssi);
	}

	*rx_power_8472 = (u32)min_t(s64, div_s64(p, 10), 0xffff);

	return 0;
}

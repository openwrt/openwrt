/* SPDX-License-Identifier: GPL-2.0 */
/*
 * EN7571 GPON laser - registers, sampling and the measurements
 *
 * The bottom of the driver: access to the 16-bit register I2C bus of the
 * laser, the ADC readers (supply voltage, bias current, temperatures), and
 * the background DDMI tick that samples the channel groups, feeds the control
 * loops and evaluates the transceiver alarm word.
 *
 * Everything above works with what this file measures; everything here is
 * called with priv->lock held.  The tick is the one thing up here that calls
 * out - into the control loops and up into the PON core - because it is the
 * only place in the driver where all three meet.
 */

#include <linux/bitops.h>
#include <linux/delay.h>
#include <linux/err.h>
#include <linux/i2c.h>
#include <linux/jiffies.h>
#include <linux/lockdep.h>
#include <linux/math64.h>
#include <linux/minmax.h>
#include <linux/pon_laser.h>
#include <linux/string.h>
#include <linux/unaligned.h>
#include <linux/workqueue.h>

#include "en7571.h"

/*
 * Register access over I2C.
 *
 * The wire format is the vendor's: the 16-bit register offset goes out MSB
 * first, the data bytes least significant first.  Getting either half wrong
 * produces reads that look plausible but are byte-swapped.
 *
 *   write:  [reg_hi] [reg_lo] [d0] [d1] ...
 *   read:   [reg_hi] [reg_lo] (repeated start) [d0] [d1] ...
 */
int en7571_reg_read(struct en7571 *priv, u16 reg, u8 *buf, u8 len)
{
	struct i2c_client *client = priv->client;
	u8 addr[2] = { reg >> 8, reg };
	struct i2c_msg msgs[2] = {
		{
			.addr = client->addr,
			.len = sizeof(addr),
			.buf = addr,
		},
		{
			.addr = client->addr,
			.flags = I2C_M_RD,
			.len = len,
			.buf = buf,
		},
	};

	int ret;

	/* Register data is at most four bytes, see en7571_read_reg() */
	if (!len || len > 4)
		return -EINVAL;

	ret = i2c_transfer(client->adapter, msgs, 2);
	if (ret != 2)
		return ret < 0 ? ret : -EIO;

	return 0;
}

int en7571_reg_write(struct en7571 *priv, u16 reg, const u8 *buf, u8 len)
{
	struct i2c_client *client = priv->client;
	u8 msg[16];
	struct i2c_msg m;
	int ret;

	if (len + 2 > sizeof(msg))
		return -EINVAL;

	put_unaligned_be16(reg, msg);
	memcpy(&msg[2], buf, len);

	m.addr = client->addr;
	m.flags = 0;
	m.len = len + 2;
	m.buf = msg;

	ret = i2c_transfer(client->adapter, &m, 1);

	return ret == 1 ? 0 : (ret < 0 ? ret : -EIO);
}

/*
 * Read-modify-write one byte of a register.
 *
 * The laser mixes fields of different widths in single bytes and the vendor
 * touches them one at a time, always as "read, (byte & keep) | set, write".
 * en7571_update() is that form at the vendor's access width; "keep" is the
 * mask the byte is ANDed with, not the bits to clear.
 */
int en7571_update(struct en7571 *priv, u16 reg, u8 len, u8 index,
		  u8 keep, u8 set)
{
	u8 buf[4];
	int ret;

	if (!len || len > sizeof(buf) || index >= len)
		return -EINVAL;

	en7571_try(en7571_reg_read(priv, reg, buf, len));

	buf[index] = (u8)((buf[index] & keep) | set);

	return en7571_reg_write(priv, reg, buf, len);
}

/* The 4-byte form the vendor uses for most registers. */
int en7571_rmw_byte(struct en7571 *priv, u16 reg, u8 index, u8 keep, u8 set)
{
	return en7571_update(priv, reg, 4, index, keep, set);
}

/* The 1-byte form, for the sub-addressed accesses (e.g. 0x0025, 0x0159). */
int en7571_rmw8(struct en7571 *priv, u16 reg, u8 keep, u8 set)
{
	return en7571_update(priv, reg, 1, 0, keep, set);
}

/* Convenience wrappers for the 1-, 2- and 4-byte registers. */
int en7571_read_reg(struct en7571 *priv, u16 reg, u8 len, u32 *val)
{
	u8 buf[4] = { 0, 0, 0, 0 };
	int ret;

	/* Below the transfer: an oversized len must not reach the I2C layer */
	if (!len || len > sizeof(buf))
		return -EINVAL;

	ret = en7571_reg_read(priv, reg, buf, len);

	if (ret)
		return ret;

	switch (len) {
	case 1:
		*val = buf[0];
		return 0;
	case 2:
		*val = get_unaligned_le16(buf);
		return 0;
	case 4:
		*val = get_unaligned_le32(buf);
		return 0;
	default:
		return -EINVAL;
	}
}

int en7571_write_reg(struct en7571 *priv, u16 reg, u8 len, u32 val)
{
	u8 buf[4];

	switch (len) {
	case 1:
		buf[0] = val;
		break;
	case 2:
		put_unaligned_le16(val, buf);
		break;
	case 4:
		put_unaligned_le32(val, buf);
		break;
	default:
		return -EINVAL;
	}

	return en7571_reg_write(priv, reg, buf, len);
}

/*
 * EN7571 GPON laser - background DDMI sampling and laser control tick
 *
 * The vendor runs en7571_internal_clock() once a second from a kernel thread
 * and, because reading the whole SFF-8472 table over the laser's register bus
 * is slow, touches only one of the five channel groups per second.  They land
 * on the even seconds 0, 2, 4, 6 and 8 of every ten, so each is sampled once
 * every ten seconds; the "fast" matrix option drops the stagger and samples
 * everything every second instead.  A group that reads as nothing (bias
 * current 0, either power below 2 in SFF-8472 units) is retaken every second
 * until it does not.  The APD voltage is retuned from the same tick every
 * T_APD seconds (flash 0x1c) when the matrix is GPON.
 *
 * After the diagnostics the same tick runs the laser control loop
 * (en7571-ctrl.c).  That part runs whatever the diagnostics setting, as in
 * the vendor, so the work is always scheduled.
 *
 * The transceiver alarm word is evaluated here and only here, right after
 * each power sample; on demand reads refresh the values but not the alarms.
 */

/* The groups due this second (vendor en7571_internal_clock) */
static u32 en7571_ddmi_due(struct en7571 *priv, unsigned int slot)
{
	u32 mask = 0;

	if (priv->ddmi_mode == EN7571_DDMI_FAST)
		return EN7571_DD_ALL;

	switch (slot % 10) {
	case 0:
		mask = EN7571_DD_VCC;
		break;
	case 2:
		mask = EN7571_DD_BIAS;
		break;
	case 4:
		mask = EN7571_DD_TEMP;
		break;
	case 6:
		mask = EN7571_DD_TXPWR;
		break;
	case 8:
		mask = EN7571_DD_RXPWR;
		break;
	}

	/*
	 * The vendor retakes the bias current while its SFF-8472 value
	 * (2 uA units) is 0, and both powers while below 2 (0.1 uW units).
	 */
	if (!priv->bias_8472)
		mask |= EN7571_DD_BIAS;
	if (priv->tx_power_8472 < 2)
		mask |= EN7571_DD_TXPWR;
	if (priv->rx_power_8472 < 2)
		mask |= EN7571_DD_RXPWR;

	return mask;
}

/* Record a successful sample of one group.  Must be called under the lock. */
static void en7571_ddmi_mark(struct en7571 *priv, u32 bit)
{
	priv->ddmi_stamp[__ffs(bit)] = jiffies;
	priv->ddmi_valid |= bit;
}

/*
 * The three ADC readers are file-local: en7571_refresh_locked() below is their
 * only caller, so they are declared here and defined at the bottom with the
 * rest of the ADC code.
 */
static int en7571_read_vcc_locked(struct en7571 *priv, u32 *vcc_uV);
static int en7571_read_bias_locked(struct en7571 *priv, u16 *bias_8472,
				   u16 *mod_8472);
static int en7571_read_temperatures_locked(struct en7571 *priv,
					   s32 *temp_millideg,
					   s32 *bosa_millideg);

/*
 * Sample the requested channel groups into the cache, with priv->lock held.
 *
 * Only what is asked for is sampled, so the background poller and an on
 * demand hwmon read can run side by side without each doing the other's
 * work.  A group whose sample fails is left marked invalid, which is what
 * makes en7571_read_cached_locked() re-sample it on the next attempt.  Every group
 * is attempted; the first error is returned.
 */
int en7571_refresh_locked(struct en7571 *priv, u32 mask)
{
	u32 vcc, tx_pwr, rx_pwr, rssi;
	u16 bias, mod;
	s32 temp, bosa;
	int ret, err = 0;

	lockdep_assert_held(&priv->lock);

	priv->ddmi_valid &= ~mask;

	if (mask & EN7571_DD_VCC) {
		ret = en7571_read_vcc_locked(priv, &vcc);
		if (!ret) {
			priv->vcc_uV = vcc;
			en7571_ddmi_mark(priv, EN7571_DD_VCC);
		} else if (!err) {
			err = ret;
		}
	}

	if (mask & EN7571_DD_BIAS) {
		ret = en7571_read_bias_locked(priv, &bias, &mod);
		if (!ret) {
			priv->bias_8472 = bias;
			priv->mod_8472 = mod;
			en7571_ddmi_mark(priv, EN7571_DD_BIAS);
		} else if (!err) {
			err = ret;
		}
	}

	if (mask & EN7571_DD_TEMP) {
		ret = en7571_read_temperatures_locked(priv, &temp, &bosa);
		if (!ret) {
			priv->temp_millideg = temp;
			priv->bosa_temp_millideg = bosa;
			en7571_ddmi_mark(priv, EN7571_DD_TEMP);
		} else if (!err) {
			err = ret;
		}
	}

	if (mask & EN7571_DD_TXPWR) {
		ret = en7571_calc_tx_power_locked(priv, &tx_pwr);
		if (!ret) {
			priv->tx_power_8472 = tx_pwr;
			en7571_ddmi_mark(priv, EN7571_DD_TXPWR);
		} else if (!err) {
			err = ret;
		}
	}

	if (mask & EN7571_DD_RXPWR) {
		ret = en7571_read_rssi_locked(priv, &rssi);
		if (!ret) {
			ret = en7571_calc_rx_power_locked(priv, rssi, &rx_pwr);
			/* A curve that cannot be evaluated reads as no light */
			if (ret)
				rx_pwr = 0;
			priv->rx_power_8472 = rx_pwr;
			en7571_ddmi_mark(priv, EN7571_DD_RXPWR);
		} else if (!err) {
			err = ret;
		}
	}

	return err;
}
/*
 * Pick out the faults the PON core has to hear about that have just gone from
 * clear to asserted: LOS from the comparator armed by en7571_hw_init(), and
 * TX_FAULT from the safe circuit latching a transmitter fault.
 *
 * priv->fe_alarms holds the word as it was last seen, so a fault that clears
 * and comes back is reported again.
 *
 * Nothing is called up from here; the tick reports with priv->lock dropped,
 * because the core takes its own locks on the way to the MAC.
 *
 * With priv->lock held.
 */
static int en7571_fault_check_locked(struct en7571 *priv, u32 *fresh)
{
	u32 val, alarm = 0;
	int ret;

	en7571_try(en7571_read_reg(priv, EN7571_LOS_STATUS, 4, &val));

	if (val & EN7571_LOS_STATUS_LOS)
		alarm |= PON_LASER_ALARM_LOS;

	en7571_try(en7571_read_reg(priv, EN7571_SAFE_PROTECT, 2, &val));

	if (val & EN7571_SAFE_PROTECT_FAULT)
		alarm |= PON_LASER_ALARM_TX_FAULT;

	/* The latch follows the word, so it is a rising-edge detector and not
	 * a one-shot: bits that have gone away are forgotten here and can be
	 * reported the next time they appear.
	 */
	*fresh = alarm & ~priv->fe_alarms;
	priv->fe_alarms = alarm;

	return 0;
}

/*
 * The vendor's SFF-8472 temperature: the die temperature in float times
 * 256, truncated toward zero (__aeabi_f2iz) and kept as 16 bits.  The die
 * temperature is held here in millidegrees, so this is that value times
 * 256 / 1000, truncated the same way.
 */
s16 en7571_temp_8472(s32 millideg)
{
	return (s16)div_s64((s64)millideg * 256, 1000);
}

/*
 * The vendor's SFF-8472 supply voltage: 2 * V / 0.0001 truncated
 * (__aeabi_d2uiz), from the voltage held here in microvolts.
 */
u16 en7571_vcc_8472(u32 uV)
{
	return min_t(u32, uV / 100, U16_MAX);
}

/*
 * Transmit side alarms (vendor en7571_tx_power_alarm_get), evaluated
 * right after a transmit power sample, against that sample and the last
 * bias current, in this order.  Each bit is set or cleared on every call:
 * there is no hysteresis.  Called with priv->lock held.
 */
static void en7571_tx_power_alarm_get(struct en7571 *priv)
{
	u16 tx = priv->tx_power_8472;
	u16 bias = priv->bias_8472;
	u32 alarm = priv->alarm;

	lockdep_assert_held(&priv->lock);

	if (tx < EN7571_TX_POWER_LOW_ALARM_THOD)
		alarm |= EN7571_ALARM_TX_POWER_LOW;
	else
		alarm &= ~EN7571_ALARM_TX_POWER_LOW;
	if (tx > EN7571_TX_POWER_HIGH_ALARM_THOD)
		alarm |= EN7571_ALARM_TX_POWER_HIGH;
	else
		alarm &= ~EN7571_ALARM_TX_POWER_HIGH;
	if (bias < EN7571_TX_CUR_LOW_ALARM_THOD)
		alarm |= EN7571_ALARM_BIAS_LOW;
	else
		alarm &= ~EN7571_ALARM_BIAS_LOW;
	if (bias > EN7571_TX_CUR_HIGH_ALARM_THOD)
		alarm |= EN7571_ALARM_BIAS_HIGH;
	else
		alarm &= ~EN7571_ALARM_BIAS_HIGH;

	priv->alarm = alarm;
}

/* Receive power alarms (stock en7571_rx_power_alarm_get), likewise */
static void en7571_rx_power_alarm_get(struct en7571 *priv)
{
	u16 rx = priv->rx_power_8472;
	u32 alarm = priv->alarm;

	lockdep_assert_held(&priv->lock);

	if (rx < EN7571_RX_POWER_LOW_ALARM_THOD)
		alarm |= EN7571_ALARM_RX_POWER_LOW;
	else
		alarm &= ~EN7571_ALARM_RX_POWER_LOW;
	if (rx > EN7571_RX_POWER_HIGH_ALARM_THOD)
		alarm |= EN7571_ALARM_RX_POWER_HIGH;
	else
		alarm &= ~EN7571_ALARM_RX_POWER_HIGH;

	priv->alarm = alarm;
}

/*
 * Die temperature and supply alarms (EN7571_ALARM_TEMP_* / _VCC_*):
 * the en7571_def.h thresholds compared like the others, on the
 * SFF-8472 values, without hysteresis.  Called with priv->lock held.
 */
static void en7571_env_alarm_get(struct en7571 *priv)
{
	s16 temp = en7571_temp_8472(priv->temp_millideg);
	u16 vcc = en7571_vcc_8472(priv->vcc_uV);
	u32 alarm = priv->alarm;

	lockdep_assert_held(&priv->lock);

	alarm &= ~(EN7571_ALARM_TEMP_HIGH | EN7571_ALARM_TEMP_LOW |
		   EN7571_ALARM_VCC_HIGH | EN7571_ALARM_VCC_LOW);
	if (temp > EN7571_TEMP_HIGH_ALARM_THOD)
		alarm |= EN7571_ALARM_TEMP_HIGH;
	if (temp < EN7571_TEMP_LOW_ALARM_THOD)
		alarm |= EN7571_ALARM_TEMP_LOW;
	if (vcc > EN7571_VCC_HIGH_ALARM_THOD)
		alarm |= EN7571_ALARM_VCC_HIGH;
	if (vcc < EN7571_VCC_LOW_ALARM_THOD)
		alarm |= EN7571_ALARM_VCC_LOW;

	priv->alarm = alarm;
}
static void en7571_ddmi_tick(struct work_struct *work)
{
	struct en7571 *priv = container_of(to_delayed_work(work),
					   struct en7571, ddmi_work);
	unsigned int slot = priv->ddmi_cnt++;
	u32 due, fresh = 0;
	int ret;

	mutex_lock(&priv->lock);

	if (priv->ddmi_mode != EN7571_DDMI_OFF) {
		/*
		 * APD tracking, before the samples as in the vendor: every
		 * T_APD seconds, GPON only, from the last BOSA temperature
		 * measured (the vendor's initial 20 C until the first one).
		 */
		if (priv->gpon && priv->tapd &&
		    slot % priv->tapd == priv->tapd - 1)
			en7571_apd_set_locked(priv, priv->bosa_temp_millideg);

		due = en7571_ddmi_due(priv, slot);
		en7571_refresh_locked(priv, due);

		/*
		 * The alarms follow the power sample they belong to, as in
		 * the vendor; a sample that failed leaves them as they were.
		 */
		if (due & priv->ddmi_valid & EN7571_DD_TXPWR)
			en7571_tx_power_alarm_get(priv);
		if (due & priv->ddmi_valid & EN7571_DD_RXPWR)
			en7571_rx_power_alarm_get(priv);
		if (due & priv->ddmi_valid & (EN7571_DD_TEMP | EN7571_DD_VCC))
			en7571_env_alarm_get(priv);

		/*
		 * The vendor runs a watchdog on the 9th second, reinitializing
		 * the part when Iav runs away.  It cannot fire: it is armed by
		 * en7571_sys_mode, which detection clears before the loop ever
		 * runs and which nothing ever sets again in the stock driver.
		 * So the protection is absent from the stock firmware too and
		 * there is nothing here to call.
		 */

		/*
		 * Faults for the PON core, once a second.  Polling is what a
		 * part with no alarm output line leaves us with, and the vendor
		 * has no such reporting at all: it reads these bits only from
		 * phy_write_proc and en7571_BoB_info (phy.ann:0xb6c8), on
		 * request, and never hands LOS or TX_FAULT to the core.  So
		 * reporting them at all is an addition here, not a copy.
		 * Only the bits that have just appeared are collected; the
		 * reporting happens below, with the lock dropped.
		 */
		ret = en7571_fault_check_locked(priv, &fresh);
		if (ret)
			dev_dbg(&priv->client->dev,
				"fault check failed: %d\n", ret);
	}

	ret = en7571_ctrl_tick(priv, slot);
	if (ret)
		dev_dbg(&priv->client->dev, "laser control step failed: %d\n",
			ret);

	mutex_unlock(&priv->lock);

	/*
	 * Up to the PON core.  The core takes its own locks on the way to the
	 * MAC, so this is handed over from under the laser lock rather than
	 * holding one across a call that ends in the state machine - the two
	 * lock orders would otherwise be able to close on each other.
	 */
	if (fresh)
		xpon_alarm(fresh);

	/* Rearm.  The work is not periodic on its own, and canceling it
	 * later still wins because cancel_delayed_work_sync() also stops a
	 * self-requeueing work.
	 */
	schedule_delayed_work(&priv->ddmi_work, HZ);
}

/*
 * Set the work up.  This has to happen before anything that can cancel it,
 * which means before it is registered with devm.
 */
void en7571_ddmi_init(struct en7571 *priv)
{
	INIT_DELAYED_WORK(&priv->ddmi_work, en7571_ddmi_tick);
}

/*
 * Start the 1 s tick.  It runs even with diagnostics disabled in the matrix,
 * since the laser control loop runs from it; it then samples nothing, and
 * on-demand reads are unaffected.
 */
void en7571_ddmi_start(struct en7571 *priv)
{
	schedule_delayed_work(&priv->ddmi_work, HZ);
}

/*
 * Stop background sampling.  Registered as a devm action so it also runs when
 * probe fails after this point, which is why the argument is void *.
 */
void en7571_ddmi_release(void *data)
{
	struct en7571 *priv = data;

	cancel_delayed_work_sync(&priv->ddmi_work);
}

/*
 * EN7571 GPON laser - ADC and sensor reading functions
 *
 * Everything in this file is called with priv->lock held.
 */

/*
 * Latch and read one SVADC sample (vendor en7571_SVADC_get): bit 4 of
 * PROBE_CONTROL + 1 with the rest of that byte preserved, then the 16-bit
 * little-endian result in ADC_PROBE_STATUS.  Selecting the source is the
 * caller's business.
 */
int en7571_svadc_get(struct en7571 *priv, u32 *raw)
{
	int ret;

	ret = en7571_rmw8(priv, EN7571_ADC_LATCH_REG, EN7571_ADC_LATCH_MASK,
			  EN7571_ADC_LATCH);
	if (ret)
		return ret;

	return en7571_read_reg(priv, EN7571_ADC_PROBE_STATUS, 2, raw);
}

/*
 * Read one ADC sample from a source in the SVADC_PD byte 0 select field.
 *
 * The byte is saved, the field set with the other bits preserved, the sample
 * latched and the byte written back, as the vendor does around every sample.
 */
static int en7571_adc_read(struct en7571 *priv, u8 source, u32 *raw)
{
	u32 saved;
	int ret, restore;

	en7571_try(en7571_read_reg(priv, EN7571_SVADC_PD, 1, &saved));

	ret = en7571_write_reg(priv, EN7571_SVADC_PD, 1,
			       (saved & EN7571_ADC_SELECT_MASK) | source);
	if (!ret)
		ret = en7571_svadc_get(priv, raw);

	restore = en7571_write_reg(priv, EN7571_SVADC_PD, 1, saved);

	return ret ? ret : restore;
}

/* Sum EN7571_ADC_CAL_SAMPLES samples and round to their mean, as the vendor */
static int en7571_adc_average(struct en7571 *priv, u32 *mean)
{
	u32 sum = 0, raw;
	unsigned int i;
	int ret;

	for (i = 0; i < EN7571_ADC_CAL_SAMPLES; i++) {
		en7571_try(en7571_svadc_get(priv, &raw));
		sum += raw;
	}

	/* ((temp >> 2) + 1) >> 1 */
	*mean = ((sum >> 2) + 1) >> 1;

	return 0;
}

/*
 * Calibrate the ADC against its two bandgap references (vendor
 * en7571_ADC_calibration), once at initialization: the 1.76 V reference in the
 * byte 0 select field and the 0.875 V one in the byte above, each averaged
 * over eight samples with the byte restored after.  The two codes give the
 * slope and offset every later reading is converted with.  References that
 * are not ordered are rejected, and readings that need them then report no
 * data instead of a number.
 */
int en7571_adc_calibration(struct en7571 *priv)
{
	struct device *dev = &priv->client->dev;
	u32 saved, bg_1v76, bg_0v875;
	int ret, restore;

	priv->adc_cal_ok = false;

	en7571_try(en7571_read_reg(priv, EN7571_SVADC_PD, 1, &saved));
	ret = en7571_write_reg(priv, EN7571_SVADC_PD, 1,
			       (saved & EN7571_ADC_SELECT_MASK) |
			       EN7571_ADC_SEL_BG_1V76);
	if (!ret)
		ret = en7571_adc_average(priv, &bg_1v76);
	restore = en7571_write_reg(priv, EN7571_SVADC_PD, 1, saved);
	if (ret || restore)
		return ret ? ret : restore;

	en7571_try(en7571_read_reg(priv, EN7571_SVADC_PD + 1, 1, &saved));
	ret = en7571_write_reg(priv, EN7571_SVADC_PD + 1, 1,
			       (saved & EN7571_ADC_BG_0V875_MASK) |
			       EN7571_ADC_SEL_BG_0V875);
	if (!ret)
		ret = en7571_adc_average(priv, &bg_0v875);
	restore = en7571_write_reg(priv, EN7571_SVADC_PD + 1, 1, saved);
	if (ret || restore)
		return ret ? ret : restore;

	dev_dbg(dev, "BG 1.76V = 0x%x, BG 0.875V = 0x%x\n", bg_1v76, bg_0v875);

	if (bg_1v76 <= bg_0v875) {
		dev_warn(dev, "ADC calibration failed (BG 1.76V 0x%x, BG 0.875V 0x%x)\n",
			 bg_1v76, bg_0v875);
		return 0;
	}

	priv->adc_bg_1v76 = bg_1v76;
	priv->adc_bg_0v875 = bg_0v875;
	priv->adc_cal_ok = true;

	return 0;
}

/*
 * Convert an ADC code to microvolts with the calibration above.  Slope and
 * offset from the two references collapse to
 * 1760000 + 885000 * (code - BG1V76) / (BG1V76 - BG0V875), and every operand
 * is a 16-bit code, so the product stays far inside 64 bits.
 */
static s64 en7571_adc_to_uV(struct en7571 *priv, u32 code)
{
	s64 span = (s64)priv->adc_bg_1v76 - priv->adc_bg_0v875;

	return 1760000LL +
	       div64_s64(885000LL * ((s64)code - priv->adc_bg_1v76), span);
}

/*
 * Supply voltage (vendor en7571_ADC_voltage_get /
 * en7571_supply_voltage_get_8472): the receive termination is switched to high
 * impedance, the select field cleared to the supply input, and the sample
 * converted.  The input is the supply halved, so the result is doubled.
 */
static int en7571_read_vcc_locked(struct en7571 *priv, u32 *vcc_uV)
{
	u32 saved, raw;
	s64 uV;
	int ret, restore;

	if (!priv->adc_cal_ok)
		return -ENODATA;

	ret = en7571_rmw8(priv, EN7571_LA_PWD, EN7571_LA_RX_HIGHZ_MASK,
			  EN7571_LA_RX_HIGHZ);
	if (ret)
		return ret;

	ret = en7571_read_reg(priv, EN7571_SVADC_PD, 1, &saved);
	if (ret) {
		en7571_rmw8(priv, EN7571_LA_PWD, EN7571_LA_RX_HIGHZ_MASK, 0);
		return ret;
	}

	ret = en7571_write_reg(priv, EN7571_SVADC_PD, 1,
			       (saved & EN7571_ADC_SELECT_MASK) |
			       EN7571_ADC_SEL_VCC);
	if (!ret)
		ret = en7571_svadc_get(priv, &raw);

	/* Termination back on, then the select byte back to what it was */
	restore = en7571_rmw8(priv, EN7571_LA_PWD, EN7571_LA_RX_HIGHZ_MASK, 0);
	if (!ret)
		ret = restore;
	restore = en7571_write_reg(priv, EN7571_SVADC_PD, 1, saved);
	if (!ret)
		ret = restore;
	if (ret)
		return ret;

	uV = 2 * en7571_adc_to_uV(priv, raw);
	*vcc_uV = uV > 0 ? (u32)min_t(s64, uV, U32_MAX) : 0;

	return 0;
}

/*
 * Bias and modulation current in SFF-8472 units of 2 uA (vendor
 * en7571_bias_current_get_8472 / en7571_mod_current_get_8472).
 *
 * The codes are en7571_info() loop readings, which hold the loop step sizes
 * in PWR_CTRL_8 around a read of P0/P1_PWR_CTRL_CS3 [27:16].  Without a
 * calibration word (flash 0x38 bias, 0x3c modulation) the vendor converts in
 * double:
 *
 *   bias = code * 1221 / 100   (that is 0.02442 mA/code, doubled to 2 uA)
 *   mod  = code * 1099 / 100
 *
 * With one, the low half is a gain in 1/256 and the high half an offset:
 *
 *   bias = ((u16)((gain * Ibias) >> 8) + offset)
 *	    + (u16)(Imod * 4650 / 4095), negative (as s16) clamped to 0
 *   mod  = ((u16)((gain * Imod) >> 8) + offset)
 *
 * The bias form reads the modulation code as well; it is read here every time
 * since the same sample feeds the modulation channel.
 */
static int en7571_read_bias_locked(struct en7571 *priv, u16 *bias_8472,
			    u16 *mod_8472)
{
	u32 fbias = priv->cal[BOB_DDMI_IBIAS / 4];
	u32 fmod = priv->cal[BOB_DDMI_IMOD / 4];
	u32 ibias, imod, v;
	u16 bias;
	int ret;

	en7571_try(en7571_info(priv, EN7571_SEL_IBIAS_NOW, &ibias));

	en7571_try(en7571_info(priv, EN7571_SEL_IMOD_NOW, &imod));

	if (fbias == 0xffffffff) {
		bias = ibias * 1221 / 100;
	} else {
		bias = (u16)(((fbias & 0xffff) * ibias) >> 8) + (fbias >> 16);
		bias += (u16)(imod * 4650 / 4095);
		if ((s16)bias < 0)
			bias = 0;
	}

	if (fmod == 0xffffffff) {
		v = imod * 1099 / 100;
		if (!(imod % 100) && (EN7571_MOD_LOW_CODES & BIT_ULL(imod / 100)))
			v--;
	} else {
		v = (u16)(((fmod & 0xffff) * imod) >> 8) + (fmod >> 16);
	}

	*bias_8472 = bias;
	*mod_8472 = v;

	return 0;
}

/*
 * Die and BOSA temperature (vendor en7571_temperature_get).
 *
 *   T_ic   = offset + slope * V - eFuse - 3.44
 *   T_bosa = T_ic - (s8)flash 0x24 [7:0]
 *
 * slope and offset default to -327.5 C/V and 495.8 C.  Flash 0x28 trims them:
 * a low half other than 0xffff scales the slope by low / 256, a high half
 * other than 0xffff adds that signed 16-bit count of degrees to the offset.
 * The BOSA offset is the sign-extended low byte of flash 0x24 when the low
 * half is not 0xffff, else 5 C.
 *
 * In millidegrees with V in microvolts, 327.5 C/V is 3275/10000 mC per uV, so
 * the slope term is 3275 * uV * scale / (10000 * 256).  Both results may be
 * negative.
 */
static int en7571_read_temperatures_locked(struct en7571 *priv, s32 *temp_millideg,
				    s32 *bosa_millideg)
{
	u32 sensor = priv->cal[BOB_TEMP_SENSOR / 4];
	u32 toff = priv->cal[BOB_TEMP_OFFSET / 4];
	s64 uV, t_ic, offset_md = 495800;
	u32 raw, scale = 256;
	s32 bosa_off = EN7571_BOSA_OFFSET_DEFAULT;
	int ret;

	if (!priv->adc_cal_ok)
		return -ENODATA;

	en7571_try(en7571_adc_read(priv, EN7571_ADC_SEL_TEMP, &raw));

	if ((sensor & 0xffff) != 0xffff)
		scale = sensor & 0xffff;
	if ((sensor >> 16) != 0xffff)
		offset_md += (s16)(sensor >> 16) * 1000;

	if ((toff & 0xffff) != 0xffff)
		bosa_off = (s8)(toff & 0xff);

	uV = en7571_adc_to_uV(priv, raw);
	t_ic = offset_md - div_s64(3275LL * scale * uV, 10000 * 256) -
	       priv->efuse_temp_offset_millideg - 3440LL;

	*temp_millideg = clamp_t(s64, t_ic, S32_MIN, S32_MAX);
	*bosa_millideg = clamp_t(s64, t_ic - bosa_off * 1000,
				 S32_MIN, S32_MAX);

	return 0;
}

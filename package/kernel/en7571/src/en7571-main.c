/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Airoha/EcoNet EN7571 GPON laser - Main driver entry point, probe, i2c_driver
 *
 * The EN7571 is the transceiver controller fitted in the BOSA of an xPON ONT.
 * It sits behind the 16-bit register I2C bus of the PON PHY and provides the
 * digital diagnostic monitoring interface (DDMI) defined by SFF-8472:
 * supply voltage, laser bias and modulation currents, optical output and
 * input power, and two temperatures (die and BOSA case).
 *
 * On the router this driver targets it is an APD transmitter, not a DFB: the
 * bias current is several tens of milliamps and the reported operating point
 * includes an APD voltage in the mid thirties.
 *
 * This file is the outside of the driver: probe and remove, the hwmon device
 * (supply, bias, optical powers and both temperatures, with the alarm bits the
 * sampler in en7571-regs.c has already set), the read-only debugfs pages, and
 * the regulator that stands for the transmitter.  What it registers, it
 * registers with devm.  It samples nothing itself: the values it reports were
 * taken by the tick or by the on-demand path below.
 *
 * Register access is 16-bit addressed, 1 to 4 bytes wide, big-endian, which
 * regmap cannot express with its own address/data split, so the I2C transfers
 * are done here directly.
 */

#include <linux/bitfield.h>
#include <linux/debugfs.h>
#include <linux/device.h>
#include <linux/err.h>
#include <linux/export.h>
#include <linux/find.h>
#include <linux/fs.h>
#include <linux/gpio/consumer.h>
#include <linux/hwmon-sysfs.h>
#include <linux/hwmon.h>
#include <linux/i2c.h>
#include <linux/int_log.h>
#include <linux/jiffies.h>
#include <linux/kernel.h>
#include <linux/lockdep.h>
#include <linux/math64.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/mutex.h>
#include <linux/of.h>
#include <linux/pon_laser.h>
#include <linux/regulator/driver.h>
#include <linux/regulator/machine.h>
#include <linux/seq_file.h>
#include <linux/slab.h>
#include <linux/unaligned.h>

#include "en7571.h"
#include "en7571-hw.h"

/* the interfaces probe registers, their implementations are further down */
static const struct hwmon_chip_info en7571_chip_info;
static const struct regulator_ops en7571_regulator_ops;
static const struct regulator_desc en7571_regulator_desc;
static const struct regulator_init_data en7571_regulator_init;
static int en7571_debugfs_init(struct en7571 *priv);

/*
 * This part, so the two operations below have something to work on.  The core
 * holds their addresses and calls them with no handle - there is one
 * transceiver and one MAC, so there is never a question of which part a call
 * is for - which leaves the driver to remember its own single instance.
 * Declared here because remove() has to clear it, and probe has to set it
 * before the registration that publishes the two addresses.  The accesses
 * carry READ_ONCE()/WRITE_ONCE() so the concurrent publication is visible to
 * the compiler and the race tools; what keeps a call from outliving the
 * device is xpon_unregister_device(), not these annotations.
 */
static struct en7571 *en7571_laser;

/*
 * The transmitter line, below with the regulator that owns it.  Module-internal:
 * only the regulator ops and the devm action that leaves the laser off reach it.
 */
static int en7571_tx_set(struct en7571 *priv, bool on);

static void en7571_shutdown(struct i2c_client *client)
{
	struct en7571 *priv = dev_get_drvdata(&client->dev);

	if (priv)
		en7571_tx_set(priv, false);
}

/*
 * Let go of the PON core.  Done before the transmitter is put away, so that
 * nothing can arrive for a device that is on its way out; the line itself is
 * dropped by the shutdown path above, which also covers an unbind.
 */
static void en7571_remove(struct i2c_client *client)
{
	struct en7571 *priv = dev_get_drvdata(&client->dev);

	if (!READ_ONCE(en7571_laser))
		return;

	/*
	 * The tick goes first.  It is the one thing in this driver that calls up
	 * into the PON core, and nothing has stopped it yet: the devm action that
	 * cancels it does not run until after this returns.  Left running it can
	 * do one of two things on the way out.
	 *
	 *  - stop the MAC's upstream burst generator with xpon_burst_ctrl(false)
	 *    from inside a crossing DAC write and never pair it with the true,
	 *    which is exactly the outcome xpon_burst_ctrl() warns leaves the
	 *    upstream quiet;
	 *  - hand a fault to a MAC that an7552_laser_detached() is busy taking
	 *    apart, since xpon_alarm() tests the MAC pointer and not whether a
	 *    front end is still registered.
	 *
	 * Cancelling twice is harmless, so the devm action can keep doing it for
	 * the probe paths that fail after the tick was started.
	 */
	if (priv)
		en7571_ddmi_release(priv);

	/*
	 * Table next, so that once the core has let go there is nothing left
	 * that could reach through it, and the pointer last: the two operations
	 * take no handle, so this is all that stands between a call arriving
	 * and a device on its way out.
	 */
	xpon_unregister_device();
	WRITE_ONCE(en7571_laser, NULL);
}

/* Registered right after the lines are taken: unbind leaves the laser off */
static void en7571_tx_off_action(void *data)
{
	en7571_tx_set(data, false);
}

/*
 * Take one transmitter line, asserted (transmitter off).  Both lines are
 * optional: without the line the driver cannot drive it, but a line held by
 * someone else (for instance a gpio-hog keeping the laser off, or the same
 * pin given for both properties) is not a reason to fail the probe either.
 * Only a deferral is passed back.
 */
static int en7571_get_line(struct device *dev, const char *con_id,
			   struct gpio_desc **desc)
{
	struct gpio_desc *d;

	d = devm_gpiod_get_optional(dev, con_id, GPIOD_OUT_HIGH);
	if (IS_ERR(d)) {
		if (PTR_ERR(d) == -EPROBE_DEFER)
			return -EPROBE_DEFER;
		dev_warn(dev, "%s-gpios unavailable (%pe), line not driven\n",
			 con_id, d);
		d = NULL;
	} else if (!d) {
		dev_warn(dev, "no %s-gpios in DT\n", con_id);
	} else if (gpiod_cansleep(d)) {
		dev_warn(dev, "%s line can sleep; en7571_tx_set() must not be called in atomic context\n",
			 con_id);
	}

	*desc = d;

	return 0;
}

/*
 * The operations this transceiver offers a PON MAC, and the faults it reports
 * back.  The MAC is the core and declares the interface (<linux/pon_laser.h>);
 * this side implements it and registers, so nothing here is exported - the
 * dependency runs from the leaf device up to the core, the way a wireless
 * driver depends on cfg80211.
 */
static int en7571_laser_safe_circuit_reset(void)
{
	return en7571_safe_circuit_reset(READ_ONCE(en7571_laser));
}

/*
 * Translate between the two numbering schemes: the core says which PON profile
 * it means in PON_LASER_MODE_* bits, this driver and the SerDes side keep the
 * vendor's numbers.  Both directions live in this one place.
 */
static u32 en7571_laser_mode_of(enum en7571_pon_mode m)
{
	switch (m) {
	case EN7571_PON_GPON_SYM:
		return PON_LASER_MODE_GPON_SYM;
	case EN7571_PON_EPON:
		return PON_LASER_MODE_EPON;
	default:
		return PON_LASER_MODE_GPON;
	}
}

/*
 * Run the transmitter in @mode, or stop it if that is PON_LASER_MODE_OFF.
 *
 * All three modes come out of the same part and differ only in the T0/T1 delay
 * the matrix programs, so nothing refuses on capability; a link that is up
 * refuses a mode change with -EBUSY from en7571_set_pon_mode().
 *
 * OFF is the traffic flag rather than a mode: with nothing to send the loops run
 * open and the transmit power is not measured, as in the vendor.  It never turns
 * the laser on.
 */
static int en7571_laser_set_traffic(u32 mode)
{
	struct en7571 *priv = READ_ONCE(en7571_laser);
	enum en7571_pon_mode m;
	int ret;

	if (mode == PON_LASER_MODE_OFF) {
		en7571_set_traffic(priv, false);
		return 0;
	}

	switch (mode) {
	case PON_LASER_MODE_GPON:
		m = EN7571_PON_GPON;
		break;
	case PON_LASER_MODE_GPON_SYM:
		m = EN7571_PON_GPON_SYM;
		break;
	case PON_LASER_MODE_EPON:
		m = EN7571_PON_EPON;
		break;
	default:
		return -EINVAL;
	}

	/*
	 * A mode that is already the current one costs nothing and is answered
	 * before the busy test, which is what lets traffic come up at all: by
	 * then the MAC has usually already powered the transmitter, and asking
	 * for a mode change under a running one is -EBUSY.  So the flag is
	 * raised here, once the mode it belongs to is in place.
	 */
	en7571_try(en7571_set_pon_mode(priv, m));

	en7571_set_traffic(priv, true);

	return 0;
}

static int en7571_probe(struct i2c_client *client)
{
	struct regulator_config config = { };
	u8 mode;
	struct device *dev = &client->dev;
	struct en7571 *priv;
	struct device *hwmon;
	int ret;

	/*
	 * One front end and one MAC, and pon_laser.h carries no handle, so the
	 * driver has to remember its instance for the core's callbacks.  This is
	 * the only thing that keeps a second one from taking the pointer away
	 * from the first and orphaning it.
	 */
	if (READ_ONCE(en7571_laser))
		return -EBUSY;

	priv = devm_kzalloc(dev, sizeof(*priv), GFP_KERNEL);
	if (!priv)
		return -ENOMEM;

	priv->client = client;
	en7571_try(devm_mutex_init(dev, &priv->lock));

	/*
	 * The transmitter lines come first, requested asserted, so the laser
	 * is held off before anything is programmed and stays off whatever
	 * fails below.
	 */
	en7571_try(en7571_get_line(dev, "tx-disable", &priv->tx_disable));
	en7571_try(en7571_get_line(dev, "phy-vcc-disable", &priv->vcc_disable));
	if (!priv->tx_disable)
		dev_warn(dev, "transmitter cannot be enabled by this driver\n");

	en7571_try(devm_add_action_or_reset(dev, en7571_tx_off_action, priv));

	en7571_try(en7571_read_calibration(priv));

	{
		enum en7571_pon_mode cal_mode;

		if (!en7571_pon_mode_from_magic(priv->cal[BOB_MAGIC / 4],
						&cal_mode))
			dev_info(dev, "calibration matrix: %s\n",
				 en7571_pon_mode_name(cal_mode));
		else
			dev_warn(dev, "no recognised calibration matrix, using device defaults\n");
	}

	en7571_ctrl_init(priv);

	scoped_guard(mutex, &priv->lock) {
		ret = en7571_detect(priv);
		if (ret)
			return dev_err_probe(dev, ret, "no EN7571 at 0x%02x\n",
					     client->addr);

		/*
		 * Everything the laser needs programmed is done here, in the
		 * vendor's order.  The transmitter is left off throughout; an
		 * ONU that transmits before it has ranged disturbs every other
		 * ONU behind the same splitter.
		 */
		ret = en7571_hw_init(priv);
	}
	if (ret)
		return ret;

	config.dev = dev;
	config.driver_data = priv;
	config.of_node = dev->of_node;
	config.init_data = &en7571_regulator_init;

	priv->rdev = devm_regulator_register(dev, &en7571_regulator_desc, &config);
	if (IS_ERR(priv->rdev))
		return dev_err_probe(dev, PTR_ERR(priv->rdev),
				     "cannot register regulator\n");

	hwmon = devm_hwmon_device_register_with_info(dev, EN7571_DRV_NAME, priv,
						     &en7571_chip_info, NULL);
	if (IS_ERR(hwmon))
		return dev_err_probe(dev, PTR_ERR(hwmon),
				     "cannot register hwmon\n");

	/* Set the work up before it can be cancelled, then register the
	 * cancellation so that a probe failure from here on still stops it.
	 */
	en7571_ddmi_init(priv);

	en7571_try_probe(dev, devm_add_action_or_reset(dev, en7571_ddmi_release, priv), "cannot register DDMI cleanup\n");

	en7571_ddmi_start(priv);

	en7571_try(en7571_debugfs_init(priv));

	/* Published last: the MAC looks the device up through its drvdata */
	dev_set_drvdata(dev, priv);

	/*
	 * Announced to the PON core last, once everything the MAC may call is in
	 * place.  Nothing here names a MAC: the core takes the two addresses and
	 * calls them with no handle, so this is where it learns what to reach -
	 * the pointer is published just above, and remove() takes it back.
	 *
	 * A MAC that has not probed yet is waiting for exactly this, which is why
	 * no MAC at this point is not an error.
	 */
	WRITE_ONCE(en7571_laser, priv);

	/*
	 * The mode this part came up in, out of the calibration matrix, before
	 * the core has told it anything.
	 */
	mode = en7571_laser_mode_of(priv->pon_mode);
	ret = xpon_register_device(en7571_laser_set_traffic,
				   en7571_laser_safe_circuit_reset, mode);
	if (ret) {
		WRITE_ONCE(en7571_laser, NULL);
		return dev_err_probe(dev, ret, "cannot attach to the PON core\n");
	}

	dev_info(dev, "EN7571 laser ready (transmitter off)\n");

	return 0;
}

static const struct i2c_device_id en7571_i2c_id[] = {
	{ .name = EN7571_DRV_NAME },
	{ }
};
MODULE_DEVICE_TABLE(i2c, en7571_i2c_id);

static const struct of_device_id en7571_of_match[] = {
	{ .compatible = "airoha,en7571" },
	{ }
};
MODULE_DEVICE_TABLE(of, en7571_of_match);

static struct i2c_driver en7571_driver = {
	.probe		= en7571_probe,
	.remove		= en7571_remove,
	.shutdown	= en7571_shutdown,
	.id_table	= en7571_i2c_id,
	.driver		= {
		/*
		 * Not optional.  i2c_register_driver() fills in owner, bus
		 * and clients, but never name, and driver_register() starts
		 * with driver_find(drv->name, drv->bus) -> kset_find_obj(),
		 * which does strcmp(kobj_name(kobj), name).  With no name
		 * here that is strcmp(valid, NULL) and the kernel oopses in
		 * strcmp while kmodloader is still insmod'ing us.
		 */
		.name		= EN7571_DRV_NAME,
		.of_match_table = en7571_of_match,
	},
};
MODULE_DESCRIPTION("Airoha EN7571 xPON laser");
MODULE_LICENSE("GPL");

module_i2c_driver(en7571_driver);

/*
 * EN7571 GPON laser - hwmon interface
*/

/*
 * Return with the requested groups sampled, refreshing only those whose
 * cached value has aged out.  The bound is the poller's own worst case, so a
 * group is never handed out older than the slot it is supposed to be
 * refreshed on; with diagnostics off nothing runs in the background and the
 * cache is bypassed entirely.  With priv->lock held: the caller keeps it for
 * the value it reads next, so a sample and the value that describes it come
 * from the same critical section.
 */
static int en7571_read_cached_locked(struct en7571 *priv, u32 mask)
{
	unsigned long bits = mask;
	unsigned long need = 0;
	unsigned long i;
	int ret = 0;

	lockdep_assert_held(&priv->lock);

	if (priv->ddmi_mode == EN7571_DDMI_OFF) {
		need = mask;
	} else {
		for_each_set_bit(i, &bits, EN7571_DD_NR) {
			if (!(priv->ddmi_valid & BIT(i)) ||
			    !time_before(jiffies,
					 priv->ddmi_stamp[i] + EN7571_DD_MAX_AGE))
				need |= BIT(i);
		}
	}

	if (need)
		ret = en7571_refresh_locked(priv, need);

	return ret;
}

/* Diagnostics in SFF-8472 units, as filled in by en7571_get_status() */
struct en7571_status {
	s16 temperature;	/* 1/256 C, two's complement */
	u16 voltage;		/* 100 uV */
	u16 bias;		/* 2 uA */
	u16 tx_power;		/* 0.1 uW */
	u16 rx_power;		/* 0.1 uW */
	u32 alarm;		/* EN7571_ALARM_* */
};

/*
 * Last diagnostics in SFF-8472 units and the alarm word (vendor
 * en7571_param_status_real + en7571_alarm).  Nothing is sampled: these are the
 * values last taken by the 1 s tick or an on-demand read, zero after
 * (re)initialization until then.  The alarm word changes only on the tick, so
 * it stays 0 while the matrix disables internal DDMI, as in the vendor.
 * Temperature and voltage are derived from the millidegree/microvolt values,
 * truncated as the vendor's float conversions truncate; bias and the powers
 * are the SFF-8472 values themselves.
 *
 * Takes priv->lock: process context only.
 */
static int en7571_get_status(struct en7571 *priv, struct en7571_status *st)
{
	if (!priv || !st)
		return -EINVAL;

	guard(mutex)(&priv->lock);
	st->temperature = en7571_temp_8472(priv->temp_millideg);
	st->voltage = en7571_vcc_8472(priv->vcc_uV);
	st->bias = priv->bias_8472;
	st->tx_power = min_t(u32, priv->tx_power_8472, U16_MAX);
	st->rx_power = min_t(u32, priv->rx_power_8472, U16_MAX);
	st->alarm = priv->alarm;

	return 0;
}

/*
 * Alarm thresholds and flags.  The vendor has alarm levels only, so they are the
 * lcrit/crit attributes: bias (curr1), transmit (power1) and receive (power2),
 * plus supply (in0) and die temperature (temp1) from thresholds the vendor defines
 * but never checks.  No warning levels, nothing for the BOSA temperature.
 * Thresholds are fixed and read only, converted to the hwmon units (mV, mC, mA,
 * uW) and truncated: the transmit high alarm is 3548.1 uW and reads 3548, the
 * receive high alarm 251.1 uW reads 251.  The flags are evaluated on the
 * SFF-8472 values and are exact.
 *
 * The flags are those of the last DDMI tick: reading them samples nothing, and
 * they stay 0 while the matrix disables internal DDMI.  Returns 1 when attr is
 * one of them.
 */
static int en7571_read_limit(struct en7571 *priv,
			     enum hwmon_sensor_types type, u32 attr,
			     int channel, long *val)
{
	enum { LCRIT, CRIT, LCRIT_ALARM, CRIT_ALARM, NONE } what = NONE;
	long lo, hi;
	u32 lo_bit, hi_bit;

	switch (type) {
	case hwmon_in:
		/* 100 uV to mV */
		lo = EN7571_VCC_LOW_ALARM_THOD / 10;
		hi = EN7571_VCC_HIGH_ALARM_THOD / 10;
		lo_bit = EN7571_ALARM_VCC_LOW;
		hi_bit = EN7571_ALARM_VCC_HIGH;
		what = attr == hwmon_in_lcrit ? LCRIT :
		       attr == hwmon_in_crit ? CRIT :
		       attr == hwmon_in_lcrit_alarm ? LCRIT_ALARM :
		       attr == hwmon_in_crit_alarm ? CRIT_ALARM : NONE;
		break;
	case hwmon_temp:
		if (channel)	/* BOSA: no thresholds */
			return 0;
		/* 1/256 C to millidegrees */
		lo = EN7571_TEMP_LOW_ALARM_THOD * 1000L / 256;
		hi = EN7571_TEMP_HIGH_ALARM_THOD * 1000L / 256;
		lo_bit = EN7571_ALARM_TEMP_LOW;
		hi_bit = EN7571_ALARM_TEMP_HIGH;
		what = attr == hwmon_temp_lcrit ? LCRIT :
		       attr == hwmon_temp_crit ? CRIT :
		       attr == hwmon_temp_lcrit_alarm ? LCRIT_ALARM :
		       attr == hwmon_temp_crit_alarm ? CRIT_ALARM : NONE;
		break;
	case hwmon_curr:
		/* 2 uA to mA */
		lo = EN7571_TX_CUR_LOW_ALARM_THOD / 500;
		hi = EN7571_TX_CUR_HIGH_ALARM_THOD / 500;
		lo_bit = EN7571_ALARM_BIAS_LOW;
		hi_bit = EN7571_ALARM_BIAS_HIGH;
		what = attr == hwmon_curr_lcrit ? LCRIT :
		       attr == hwmon_curr_crit ? CRIT :
		       attr == hwmon_curr_lcrit_alarm ? LCRIT_ALARM :
		       attr == hwmon_curr_crit_alarm ? CRIT_ALARM : NONE;
		break;
	case hwmon_power:
		/* 0.1 uW to uW */
		if (channel == EN7571_TX_POWER_CHANNEL) {
			lo = EN7571_TX_POWER_LOW_ALARM_THOD / 10;
			hi = EN7571_TX_POWER_HIGH_ALARM_THOD / 10;
			lo_bit = EN7571_ALARM_TX_POWER_LOW;
			hi_bit = EN7571_ALARM_TX_POWER_HIGH;
		} else {
			lo = EN7571_RX_POWER_LOW_ALARM_THOD / 10;
			hi = EN7571_RX_POWER_HIGH_ALARM_THOD / 10;
			lo_bit = EN7571_ALARM_RX_POWER_LOW;
			hi_bit = EN7571_ALARM_RX_POWER_HIGH;
		}
		what = attr == hwmon_power_lcrit ? LCRIT :
		       attr == hwmon_power_crit ? CRIT :
		       attr == hwmon_power_lcrit_alarm ? LCRIT_ALARM :
		       attr == hwmon_power_crit_alarm ? CRIT_ALARM : NONE;
		break;
	default:
		return 0;
	}

	switch (what) {
	case LCRIT:
		*val = lo;
		break;
	case CRIT:
		*val = hi;
		break;
	case LCRIT_ALARM:
	case CRIT_ALARM:
		scoped_guard(mutex, &priv->lock)
			*val = !!(priv->alarm & (what == LCRIT_ALARM ?
						 lo_bit : hi_bit));
		break;
	default:
		return 0;
	}

	return 1;
}

static int en7571_read(struct device *dev, enum hwmon_sensor_types type,
		       u32 attr, int channel, long *val)
{
	struct en7571 *priv = dev_get_drvdata(dev);
	u32 mask;
	int ret;

	ret = en7571_read_limit(priv, type, attr, channel, val);
	if (ret)
		return 0;

	switch (type) {
	case hwmon_in:
		mask = EN7571_DD_VCC;
		break;
	case hwmon_temp:
		mask = EN7571_DD_TEMP;
		break;
	case hwmon_curr:
		mask = EN7571_DD_BIAS;
		break;
	case hwmon_power:
		mask = channel == EN7571_TX_POWER_CHANNEL ? EN7571_DD_TXPWR :
							    EN7571_DD_RXPWR;
		break;
	default:
		return -EOPNOTSUPP;
	}

	/* Sample only what is not already fresh; when the background poller
	 * is running this is normally nothing at all.  The value below is
	 * taken under the same lock, so it is the one just sampled.
	 */
	guard(mutex)(&priv->lock);
	en7571_try(en7571_read_cached_locked(priv, mask));

	switch (type) {
	case hwmon_in:
		/* mV */
		*val = DIV_ROUND_CLOSEST(priv->vcc_uV, 1000);
		break;
	case hwmon_temp:
		/* millidegrees, may be negative */
		*val = channel ? priv->bosa_temp_millideg :
				 priv->temp_millideg;
		break;
	case hwmon_curr:
		/* mA from SFF-8472 2 uA */
		*val = DIV_ROUND_CLOSEST((channel ? priv->mod_8472 :
					  priv->bias_8472) * 2, 1000);
		break;
	default:
		/* uW from SFF-8472 0.1 uW */
		*val = (channel == EN7571_TX_POWER_CHANNEL ?
			priv->tx_power_8472 : priv->rx_power_8472) / 10;
		break;
	}

	return 0;
}

static const char * const en7571_temp_label[] = { "die", "bosa" };
static const char * const en7571_curr_label[] = { "bias", "modulation" };
static const char * const en7571_power_label[] = { "tx", "rx" };

static int en7571_read_string(struct device *dev,
			      enum hwmon_sensor_types type, u32 attr,
			      int channel, const char **str)
{
	switch (type) {
	case hwmon_in:
		*str = "vcc";
		return 0;
	case hwmon_temp:
		*str = en7571_temp_label[channel];
		return 0;
	case hwmon_curr:
		*str = en7571_curr_label[channel];
		return 0;
	case hwmon_power:
		*str = en7571_power_label[channel];
		return 0;
	default:
		return -EOPNOTSUPP;
	}
}

static const struct hwmon_channel_info * const en7571_hwmon_info[] = {
	HWMON_CHANNEL_INFO(in, HWMON_I_INPUT | HWMON_I_LABEL |
			   HWMON_I_LCRIT | HWMON_I_CRIT |
			   HWMON_I_LCRIT_ALARM | HWMON_I_CRIT_ALARM),
	HWMON_CHANNEL_INFO(temp,
			   HWMON_T_INPUT | HWMON_T_LABEL |
			   HWMON_T_LCRIT | HWMON_T_CRIT |
			   HWMON_T_LCRIT_ALARM | HWMON_T_CRIT_ALARM,
			   HWMON_T_INPUT | HWMON_T_LABEL),
	HWMON_CHANNEL_INFO(curr,
			   HWMON_C_INPUT | HWMON_C_LABEL |
			   HWMON_C_LCRIT | HWMON_C_CRIT |
			   HWMON_C_LCRIT_ALARM | HWMON_C_CRIT_ALARM,
			   HWMON_C_INPUT | HWMON_C_LABEL),
	HWMON_CHANNEL_INFO(power,
			   HWMON_P_INPUT | HWMON_P_LABEL |
			   HWMON_P_LCRIT | HWMON_P_CRIT |
			   HWMON_P_LCRIT_ALARM | HWMON_P_CRIT_ALARM,
			   HWMON_P_INPUT | HWMON_P_LABEL |
			   HWMON_P_LCRIT | HWMON_P_CRIT |
			   HWMON_P_LCRIT_ALARM | HWMON_P_CRIT_ALARM),
	NULL
};

static const struct hwmon_ops en7571_hwmon_ops = {
	.visible	= 0444,
	.read		= en7571_read,
	.read_string	= en7571_read_string,
};

static const struct hwmon_chip_info en7571_chip_info = {
	.ops	= &en7571_hwmon_ops,
	.info	= en7571_hwmon_info,
};

/*
 * EN7571 debugfs, read only: "a2" (diagnostics as an SFF-8472 A2h page)
 * and "bob" (the vendor BoB information dump).
 *
 * "a2": the vendor driver builds no A0h/A2h image for the on-board EN7571: it
 * keeps the word (en7571_get_status()) and builds no A0h/A2h image, so the page is
 * synthesized here.  Laid out as the diagnostic page of SFF-8472 rev 12.x,
 * internally calibrated, hence debugfs and not nvmem:
 *
 *   0..39    alarm/warning thresholds, big endian; the vendor has alarm
 *            levels only, the warning fields repeat them
 *   95       CC_DMI, low byte of the sum of bytes 0..94
 *   96..105  temperature, Vcc, bias, TX power, RX power
 *   112/113  alarm flags (EN7571_ALARM_*)
 *   116/117  warning flags, the same as the alarm flags
 *
 * Everything else reads 0: no identification page, no external calibration, no
 * control byte.  Reading samples nothing; the values are those of the last DDMI
 * tick.
*/

/* high alarm, low alarm per quantity, in the A2h order */
static const u16 en7571_a2_thresholds[][2] = {
	{ EN7571_TEMP_HIGH_ALARM_THOD, EN7571_TEMP_LOW_ALARM_THOD },
	{ EN7571_VCC_HIGH_ALARM_THOD, EN7571_VCC_LOW_ALARM_THOD },
	{ EN7571_TX_CUR_HIGH_ALARM_THOD, EN7571_TX_CUR_LOW_ALARM_THOD },
	{ EN7571_TX_POWER_HIGH_ALARM_THOD, EN7571_TX_POWER_LOW_ALARM_THOD },
	{ EN7571_RX_POWER_HIGH_ALARM_THOD, EN7571_RX_POWER_LOW_ALARM_THOD },
};

/* A2h byte 112 / byte 113 bits 7..6 from EN7571_ALARM_* */
static void en7571_a2_flags(u8 *flags, u32 alarm)
{
	flags[0] = ((alarm & EN7571_ALARM_TEMP_HIGH) ? BIT(7) : 0) |
		   ((alarm & EN7571_ALARM_TEMP_LOW) ? BIT(6) : 0) |
		   ((alarm & EN7571_ALARM_VCC_HIGH) ? BIT(5) : 0) |
		   ((alarm & EN7571_ALARM_VCC_LOW) ? BIT(4) : 0) |
		   ((alarm & EN7571_ALARM_BIAS_HIGH) ? BIT(3) : 0) |
		   ((alarm & EN7571_ALARM_BIAS_LOW) ? BIT(2) : 0) |
		   ((alarm & EN7571_ALARM_TX_POWER_HIGH) ? BIT(1) : 0) |
		   ((alarm & EN7571_ALARM_TX_POWER_LOW) ? BIT(0) : 0);
	flags[1] = ((alarm & EN7571_ALARM_RX_POWER_HIGH) ? BIT(7) : 0) |
		   ((alarm & EN7571_ALARM_RX_POWER_LOW) ? BIT(6) : 0);
}

static void en7571_a2_build(const struct en7571_status *st, u8 *a2)
{
	unsigned int i, sum = 0;

	memset(a2, 0, EN7571_SFF_A2_SIZE);
	for (i = 0; i < ARRAY_SIZE(en7571_a2_thresholds); i++) {
		u8 *t = &a2[i * 8];

		/* high alarm, low alarm, high warning, low warning */
		put_unaligned_be16(en7571_a2_thresholds[i][0], &t[0]);
		put_unaligned_be16(en7571_a2_thresholds[i][1], &t[2]);
		put_unaligned_be16(en7571_a2_thresholds[i][0], &t[4]);
		put_unaligned_be16(en7571_a2_thresholds[i][1], &t[6]);
	}
	for (i = 0; i < EN7571_SFF_CC_DMI; i++)
		sum += a2[i];
	a2[EN7571_SFF_CC_DMI] = sum;

	put_unaligned_be16(st->temperature, &a2[EN7571_SFF_TEMP]);
	put_unaligned_be16(st->voltage, &a2[EN7571_SFF_VCC]);
	put_unaligned_be16(st->bias, &a2[EN7571_SFF_BIAS]);
	put_unaligned_be16(st->tx_power, &a2[EN7571_SFF_TX_POWER]);
	put_unaligned_be16(st->rx_power, &a2[EN7571_SFF_RX_POWER]);
	en7571_a2_flags(&a2[EN7571_SFF_ALARM_FLAGS], st->alarm);
	en7571_a2_flags(&a2[EN7571_SFF_WARN_FLAGS], st->alarm);
}

static ssize_t en7571_a2_read(struct file *file, char __user *buf,
			      size_t count, loff_t *ppos)
{
	struct en7571_status st;
	u8 a2[EN7571_SFF_A2_SIZE];
	int ret;

	en7571_try(en7571_get_status(file->private_data, &st));
	en7571_a2_build(&st, a2);

	return simple_read_from_buffer(buf, count, ppos, a2, sizeof(a2));
}

static const struct file_operations en7571_a2_fops = {
	.owner	= THIS_MODULE,
	.open	= simple_open,
	.read	= en7571_a2_read,
	.llseek	= default_llseek,
};

/*
 * "bob": what the vendor prints for en7571_BoB_info() and en7571_BoB_Debug(),
 * from the driver's state and plain register reads only.  Nothing is written
 * to the EN7571 and nothing is sampled, with these differences:
 *
 *  - the DDMI values (bias, modulation, temperatures, Vcc, powers) are those of
 *    the last DDMI tick, as the vendor's globals;
 *  - IBIAS_NOW, IMOD_NOW and IAV_NOW are the codes the control loop last set:
 *    the vendor reads them live through en7571_info(2/3/4), which holds the
 *    loop (PWR_CTRL_8) around the read;
 *  - PWRADC is the last valid transmit power sum >> 4, the vendor starts a
 *    conversion (en7571_info(7));
 *  - powers are 10 * log10(P / 1 mW) through intlog10(); the vendor uses a
 *    six-term series (en7571_Taylor_series());
 *  - the vendor's driver version constant and its pwradc_dbg flag have no
 *    counterpart and are left out.
 */

static int en7571_bob_get(struct en7571 *priv, struct en7571_bob *b)
{
	int ret;

	guard(mutex)(&priv->lock);
	ret = en7571_read_reg(priv, EN7571_LOS_STATUS, 4, &b->los);
	if (!ret)
		ret = en7571_read_reg(priv, EN7571_ROGUE_ONU_DET_CTRL, 4,
				      &b->rogue);
	if (!ret)
		ret = en7571_read_reg(priv, EN7571_SAFE_PROTECT, 4, &b->safe);
	if (!ret)
		ret = en7571_read_reg(priv, EN7571_RO_PWR_CTRL_0, 4,
				      &b->ro_pwr);

	b->bias = priv->bias_8472;
	b->mod = priv->mod_8472;
	b->ic_md = priv->temp_millideg;
	b->bosa_md = priv->bosa_temp_millideg;
	b->env_md = en7571_env_temp_md(priv);
	b->apd_mv = priv->apd_mv;
	b->vcc_uV = priv->vcc_uV;
	b->tx_power = priv->tx_power_8472;
	b->rx_power = priv->rx_power_8472;
	b->traffic = READ_ONCE(priv->traffic);
	b->dol = priv->dol;
	b->ibias_now = priv->ibias_now;
	b->imod_now = priv->imod_now;
	b->iav_now = priv->iav_now;
	b->tssi = priv->tssi;
	b->ddmi_mode = priv->ddmi_mode;
	b->iav_imod = priv->cal[BOB_IAV_IMOD / 4];
	b->pav_p1 = priv->cal[BOB_PAV_P1 / 4];
	b->t0ct1c = priv->cal[BOB_T0CT1C / 4];

	return ret;
}

/* milli units as "[-]x.y", one decimal truncated, as the vendor prints */
static void en7571_bob_milli(struct seq_file *m, const char *name, s32 v,
			     const char *unit)
{
	u32 a = abs(v);

	seq_printf(m, "%s = %s%u.%u%s\n", name, v < 0 ? "-" : "", a / 1000,
		   (a % 1000) / 100, unit);
}

/* SFF-8472 power (0.1 uW) in dBm with two decimals; below 2 it is -40 */
static void en7571_bob_power(struct seq_file *m, const char *name, u32 p)
{
	s32 cdbm;
	u32 a;

	if (p < 2) {
		seq_printf(m, "%s = -40 dBm\n", name);
		return;
	}

	/* 10 * log10(p * 0.1 uW / 1 mW) = 10 * log10(p) - 40, in 0.01 dB */
	cdbm = (s32)div_u64((u64)intlog10(p) * 1000, 1 << 24) - 4000;
	a = abs(cdbm);
	seq_printf(m, "%s = %s%u.%02u dBm (%u.%u uW)\n", name,
		   cdbm < 0 ? "-" : "", a / 100, a % 100, p / 10, p % 10);
}

static int en7571_bob_show(struct seq_file *m, void *v)
{
	struct en7571_bob b;
	u32 cal_iav;
	int ret;

	en7571_try(en7571_bob_get(m->private, &b));

	/* en7571_BoB_info() */
	seq_printf(m, "LOS = %d\n", !!(b.los & EN7571_LOS_STATUS_LOS));
	seq_printf(m, "Tx SD = %d\n", !!(b.rogue & EN7571_TX_SD_STATUS));
	seq_printf(m, "Tx fault = %d\n", !!(b.safe & EN7571_TX_FAULT_STATUS));
	/* 2 uA units: / 50 gives 0.1 mA */
	seq_printf(m, "I bias = %u.%01u mA\n", b.bias / 50 / 10,
		   b.bias / 50 % 10);
	seq_printf(m, "I mod = %u.%01u mA\n", b.mod / 50 / 10, b.mod / 50 % 10);
	en7571_bob_milli(m, "IC temperature", b.ic_md, "'C");
	en7571_bob_milli(m, "BOSA temperature", b.bosa_md, "'C");
	en7571_bob_milli(m, "Enviroment temperature", b.env_md, "'C");
	en7571_bob_milli(m, "APD Voltage", b.apd_mv, "V");
	seq_printf(m, "VCC = %u.%02uV\n", b.vcc_uV / 1000000,
		   b.vcc_uV % 1000000 / 10000);
	en7571_bob_power(m, "Tx power", b.tx_power);
	en7571_bob_power(m, "Rx power", b.rx_power);

	/* en7571_BoB_Debug() */
	seq_printf(m, "\nTraffic %s\n", b.traffic ? "up" : "down");
	seq_printf(m, "IBIAS_NOW = 0x%03X\n", b.ibias_now);
	seq_printf(m, "IMOD_NOW = 0x%03X\n", b.imod_now);
	if (!b.dol)
		seq_printf(m, "IAV_NOW = 0x%03X\n", b.iav_now);
	seq_printf(m, "PWRADC = 0x%03X\n", b.tssi >> 4);
	seq_printf(m, "ER DAC = 0x%03X\n",
		   (u32)FIELD_GET(EN7571_RO_ER_DAC, b.ro_pwr));

	cal_iav = b.iav_imod >> 16;
	seq_puts(m, "\nFLASH:\n");
	seq_printf(m, "IBIAS cal = 0x%03X\n",
		   cal_iav - ((b.iav_imod >> 1) & 0x7ff));
	seq_printf(m, "IMOD  cal = 0x%03X\n", b.iav_imod & 0xfff);
	seq_printf(m, "IAV   cal = 0x%03X\n", cal_iav);
	seq_printf(m, "PWR target = 0x%03X\n", b.pav_p1 >> 16);
	seq_printf(m, "ER  target = 0x%03X\n", b.pav_p1 & 0xfff);
	seq_printf(m, "T0/T1 delay = 0x%02X\n", b.t0ct1c & 0xff);
	seq_printf(m, "en7571_internal_DDMI = %u\n", b.ddmi_mode);

	return 0;
}
DEFINE_SHOW_ATTRIBUTE(en7571_bob);

unsigned int en7571_debug;
module_param_named(debug, en7571_debug, uint, 0644);
MODULE_PARM_DESC(debug, "kernel log: bit 0 laser operations, bit 1 values dump after the safe circuit reset");

/* the "bob" values (en7571_BoB_info() and _Debug()) as kernel log lines */
void en7571_dump(struct en7571 *priv)
{
	struct device *dev = &priv->client->dev;
	/*
	 * en7571_bob_show() takes the device out of m->private, exactly as
	 * DEFINE_SHOW_ATTRIBUTE's single_open() leaves it, so a hand-built
	 * seq_file has to carry it too - otherwise en7571_bob_get() locks
	 * priv->lock through a NULL.
	 */
	struct seq_file m = { .private = priv, .size = EN7571_DUMP_SIZE };
	char *line, *next;

	m.buf = kzalloc(EN7571_DUMP_SIZE, GFP_KERNEL);
	if (!m.buf)
		return;

	if (en7571_bob_show(&m, priv)) {
		dev_info(dev, "dump: cannot read the values\n");
		goto out;
	}
	if (seq_has_overflowed(&m))
		dev_info(dev, "dump: truncated\n");

	m.buf[min_t(size_t, m.count, EN7571_DUMP_SIZE - 1)] = 0;
	for (line = m.buf; line && *line; line = next) {
		next = strchr(line, '\n');
		if (next)
			*next++ = 0;
		if (*line)
			dev_info(dev, "dump: %s\n", line);
	}
out:
	kfree(m.buf);
}

static void en7571_debugfs_remove(void *data)
{
	debugfs_remove(data);
}

/* <debugfs>/en7571-<i2c dev>/{a2,bob}; failures are not fatal */
static int en7571_debugfs_init(struct en7571 *priv)
{
	struct device *dev = &priv->client->dev;
	struct dentry *dir;
	char name[32];

	snprintf(name, sizeof(name), "en7571-%s", dev_name(dev));
	dir = debugfs_create_dir(name, NULL);
	debugfs_create_file_size("a2", 0400, dir, priv, &en7571_a2_fops,
				 EN7571_SFF_A2_SIZE);
	debugfs_create_file("bob", 0400, dir, priv, &en7571_bob_fops);

	return devm_add_action_or_reset(dev, en7571_debugfs_remove, dir);
}

/*
 * EN7571 GPON laser - Regulator interface for transmitter control
 *
 * The xPON PHY does not drive the transmitter line itself: it goes through
 * this regulator, so the line is governed by the usual regulator reference
 * counting and shows up in the regulator debugfs.
 */

static void en7571_gpio_set(struct gpio_desc *desc, int value)
{
	if (!desc)
		return;

	/*
	 * en7571_tx_set() is reachable from the regulator enable path, which
	 * the xPON PHY calls from process context; a line that cannot sleep
	 * is driven with the atomic accessor all the same.
	 */
	if (gpiod_cansleep(desc))
		gpiod_set_value_cansleep(desc, value);
	else
		gpiod_set_value(desc, value);
}

/*
 * Turn the transmitter on or off.
 *
 * The board has two separate lines and the vendor keeps them apart:
 *
 *   tx-disable      the power stage of the transmitter itself
 *   vcc-disable     the supply to the optical sub-assembly
 *
 * Powering up is supply first, then the stage, down is the reverse.  The order
 * is not cosmetic: driving the stage while the supply collapses puts a
 * transient across the laser the bias loop has not ramped down for.
 *
 * Asserted means disabled for both, and both are requested asserted while the
 * driver is not driving them.  Without a tx-disable line there is nothing that
 * can turn the stage on, so enabling fails and the transmitter stays as the
 * board left it.
 *
 * This does not touch the I2C bus and may be called from atomic context as long
 * as the GPIO lines do not sleep.  It is reached only through the regulator
 * below; the xPON PHY never calls it directly, which keeps the line's reference
 * count the regulator core's business.
 */
static int en7571_tx_set(struct en7571 *priv, bool on)
{
	if (!priv)
		return -EINVAL;

	if (on) {
		if (!priv->tx_disable)
			return -ENODEV;
		/*
		 * Flag first: en7571_set_pon_mode() refuses to rewrite the
		 * timing byte while this is set, and the transmitter must not
		 * be up and unknown to it for the length of the two writes.
		 */
		WRITE_ONCE(priv->tx_enabled, true);
		en7571_gpio_set(priv->vcc_disable, 0);
		en7571_gpio_set(priv->tx_disable, 0);
	} else {
		en7571_gpio_set(priv->tx_disable, 1);
		en7571_gpio_set(priv->vcc_disable, 1);
		/* The reverse: off only once both lines are down */
		WRITE_ONCE(priv->tx_enabled, false);
	}

	en7571_dbg(priv, EN7571_DBG_OPS, "transmitter %s (tx_disable %d vcc_disable %d)\n",
		   on ? "on" : "off", on ? 0 : 1, on ? 0 : 1);

	return 0;
}

/*
 * EN7571 GPON laser - Regulator interface for transmitter control
*/

static int en7571_regulator_enable(struct regulator_dev *rdev)
{
	struct en7571 *priv = rdev_get_drvdata(rdev);

	dev_dbg(rdev->dev.parent, "laser enabled\n");

	return en7571_tx_set(priv, true);
}

static int en7571_regulator_disable(struct regulator_dev *rdev)
{
	struct en7571 *priv = rdev_get_drvdata(rdev);

	dev_dbg(rdev->dev.parent, "laser disabled\n");

	return en7571_tx_set(priv, false);
}

static int en7571_regulator_is_enabled(struct regulator_dev *rdev)
{
	struct en7571 *priv = rdev_get_drvdata(rdev);

	return READ_ONCE(priv->tx_enabled);
}

static const struct regulator_ops en7571_regulator_ops = {
	.enable		= en7571_regulator_enable,
	.disable	= en7571_regulator_disable,
	.is_enabled	= en7571_regulator_is_enabled,
};

static const struct regulator_desc en7571_regulator_desc = {
	.name	= "en7571-laser",
	.ops	= &en7571_regulator_ops,
	.type	= REGULATOR_CURRENT,
	.owner	= THIS_MODULE,
};

/*
 * Without constraints the core has no REGULATOR_CHANGE_STATUS and refuses
 * the consumer's enable with -EPERM before en7571_regulator_enable() runs.
 */
static const struct regulator_init_data en7571_regulator_init = {
	.constraints = {
		.valid_ops_mask	= REGULATOR_CHANGE_STATUS,
	},
};

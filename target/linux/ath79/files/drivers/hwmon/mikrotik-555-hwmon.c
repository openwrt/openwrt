// SPDX-License-Identifier: GPL-2.0-or-later
/*
 *  MikroTik RouterBOARD 555 timer health monitor
 *
 *  Some RouterBOARDs measure their input voltage and PCB temperature with
 *  a 555 timer whose output is sampled on a GPIO. A mux GPIO selects
 *  whether the timing network depends on the input voltage or on a
 *  thermistor. Like RouterOS, the low time fraction of the signal is
 *  scaled by a factory correction stored in hard_config, then converted
 *  through per-board tables.
 *
 *  Copyright (C) 2026 Santiago Hormazabal <santiagohssl@gmail.com>
 *
 *  The measurement method, the use of the hard_config correction and the
 *  conversion tables (in the device tree) were obtained by examining
 *  MikroTik RouterOS 7.23.8 and 7.24.4 (the GPL voltage.ko module and the
 *  sys2 health code). No RouterOS code is included.
 */

#include <linux/delay.h>
#include <linux/gpio/consumer.h>
#include <linux/hwmon.h>
#include <linux/interrupt.h>
#include <linux/ktime.h>
#include <linux/module.h>
#include <linux/mutex.h>
#include <linux/nvmem-consumer.h>
#include <linux/of.h>
#include <linux/platform_device.h>
#include <linux/property.h>
#include <linux/slab.h>
#include <linux/sort.h>
#include <linux/spinlock.h>
#include <linux/workqueue.h>

#define MT555_SAMPLES		32
#define MT555_SETTLE_MS		50
#define MT555_SAMPLE_MS		250
#define MT555_INTERVAL_MS	2000
#define MT555_SCALE		100000
#define MT555_CORR_ONE		1000

enum mt555_channel {
	MT555_TEMP,	/* mux inactive */
	MT555_VOLTAGE,	/* mux active */
	MT555_CHANNELS,
};

struct mt555_table {
	const u32 *raw;
	const s32 *val;
	int len;
};

struct mt555 {
	struct device *dev;
	struct gpio_desc *signal;
	struct gpio_desc *mux;
	int irq;

	spinlock_t lock;
	u64 last_edge;
	u32 high[MT555_SAMPLES];
	u32 low[MT555_SAMPLES];
	int nhigh;
	int nlow;

	struct mt555_table tables[MT555_CHANNELS];
	u32 corr;

	struct mutex result_lock;
	long value[MT555_CHANNELS];
	bool valid[MT555_CHANNELS];

	struct delayed_work work;
};

static irqreturn_t mt555_irq(int irq, void *data)
{
	struct mt555 *mt = data;
	u64 now = ktime_get_ns();
	u64 delta;

	spin_lock(&mt->lock);
	if (mt->last_edge) {
		delta = div_u64(now - mt->last_edge, NSEC_PER_USEC);
		/* The level after the edge tells which phase just ended */
		if (gpiod_get_value(mt->signal)) {
			if (mt->nlow < MT555_SAMPLES)
				mt->low[mt->nlow++] = delta;
		} else {
			if (mt->nhigh < MT555_SAMPLES)
				mt->high[mt->nhigh++] = delta;
		}
	}
	mt->last_edge = now;
	spin_unlock(&mt->lock);

	return IRQ_HANDLED;
}

static int mt555_cmp_u32(const void *a, const void *b)
{
	u32 x = *(const u32 *)a, y = *(const u32 *)b;

	return x < y ? -1 : x > y;
}

static u32 mt555_median(u32 *v, int n)
{
	sort(v, n, sizeof(*v), mt555_cmp_u32, NULL);

	return v[n / 2];
}

/* Linear interpolation in a table sorted by decreasing raw value */
static s32 mt555_interpolate(const struct mt555_table *t, u32 raw)
{
	int i;

	if (raw >= t->raw[0])
		return t->val[0];

	for (i = 1; i < t->len; i++) {
		if (raw >= t->raw[i]) {
			s64 span = (s64)t->val[i] - t->val[i - 1];

			return t->val[i - 1] +
			       div_s64(span * (t->raw[i - 1] - raw),
				       t->raw[i - 1] - t->raw[i]);
		}
	}

	return t->val[t->len - 1];
}

/* Inverse of mt555_interpolate(), for values increasing along the table */
static u32 mt555_raw_for(const struct mt555_table *t, s32 val)
{
	int i;

	if (val <= t->val[0])
		return t->raw[0];

	for (i = 1; i < t->len; i++) {
		if (val <= t->val[i]) {
			s64 span = (s64)t->raw[i - 1] - t->raw[i];

			return t->raw[i - 1] -
			       div_s64(span * (val - t->val[i - 1]),
				       t->val[i] - t->val[i - 1]);
		}
	}

	return t->raw[t->len - 1];
}

/* Measure one channel; returns the low time fraction scaled by 100000 */
static int mt555_measure(struct mt555 *mt, enum mt555_channel ch, u32 *raw)
{
	u32 high[MT555_SAMPLES], low[MT555_SAMPLES];
	int nhigh, nlow;
	u32 mh, ml;

	gpiod_set_value_cansleep(mt->mux, ch == MT555_VOLTAGE);
	msleep(MT555_SETTLE_MS);

	spin_lock_irq(&mt->lock);
	mt->last_edge = 0;
	mt->nhigh = 0;
	mt->nlow = 0;
	spin_unlock_irq(&mt->lock);

	enable_irq(mt->irq);
	msleep(MT555_SAMPLE_MS);
	disable_irq(mt->irq);

	spin_lock_irq(&mt->lock);
	nhigh = mt->nhigh;
	nlow = mt->nlow;
	memcpy(high, mt->high, nhigh * sizeof(*high));
	memcpy(low, mt->low, nlow * sizeof(*low));
	spin_unlock_irq(&mt->lock);

	if (nhigh < 3 || nlow < 3)
		return -ENODATA;

	mh = mt555_median(high, nhigh);
	ml = mt555_median(low, nlow);
	if (!mh || !ml)
		return -ENODATA;

	*raw = div_u64((u64)ml * MT555_SCALE, mh + ml);

	return 0;
}

static void mt555_work(struct work_struct *work)
{
	struct mt555 *mt = container_of(to_delayed_work(work), struct mt555,
					work);
	int ch;

	for (ch = 0; ch < MT555_CHANNELS; ch++) {
		u32 raw;
		int ret;

		ret = mt555_measure(mt, ch, &raw);

		mutex_lock(&mt->result_lock);
		mt->valid[ch] = !ret;
		if (!ret) {
			raw = div_u64((u64)raw * mt->corr, MT555_CORR_ONE);
			mt->value[ch] = mt555_interpolate(&mt->tables[ch], raw);
		}
		mutex_unlock(&mt->result_lock);
	}

	queue_delayed_work(system_freezable_wq, &mt->work,
			   msecs_to_jiffies(MT555_INTERVAL_MS));
}

static int mt555_read(struct device *dev, enum hwmon_sensor_types type,
		      u32 attr, int channel, long *val)
{
	struct mt555 *mt = dev_get_drvdata(dev);
	enum mt555_channel ch = type == hwmon_in ? MT555_VOLTAGE : MT555_TEMP;
	int ret = 0;

	mutex_lock(&mt->result_lock);
	if (mt->valid[ch])
		*val = mt->value[ch];
	else
		ret = -ENODATA;
	mutex_unlock(&mt->result_lock);

	return ret;
}

static umode_t mt555_is_visible(const void *data, enum hwmon_sensor_types type,
				u32 attr, int channel)
{
	return 0444;
}

static const struct hwmon_channel_info * const mt555_info[] = {
	HWMON_CHANNEL_INFO(in, HWMON_I_INPUT),
	HWMON_CHANNEL_INFO(temp, HWMON_T_INPUT),
	NULL
};

static const struct hwmon_ops mt555_hwmon_ops = {
	.is_visible = mt555_is_visible,
	.read = mt555_read,
};

static const struct hwmon_chip_info mt555_chip_info = {
	.ops = &mt555_hwmon_ops,
	.info = mt555_info,
};

static int mt555_parse_table(struct mt555 *mt, const char *prop,
			     struct mt555_table *t)
{
	struct device *dev = mt->dev;
	u32 *cells, *raw;
	s32 *val;
	int n, i, ret;

	n = device_property_count_u32(dev, prop);
	if (n < 4 || n % 2)
		return dev_err_probe(dev, -EINVAL, "invalid %s\n", prop);

	cells = devm_kcalloc(dev, n, sizeof(*cells), GFP_KERNEL);
	raw = devm_kcalloc(dev, n / 2, sizeof(*raw), GFP_KERNEL);
	val = devm_kcalloc(dev, n / 2, sizeof(*val), GFP_KERNEL);
	if (!cells || !raw || !val)
		return -ENOMEM;

	ret = device_property_read_u32_array(dev, prop, cells, n);
	if (ret)
		return ret;

	for (i = 0; i < n / 2; i++) {
		raw[i] = cells[2 * i];
		val[i] = cells[2 * i + 1];
		if (i && (raw[i] >= raw[i - 1] || val[i] <= val[i - 1]))
			return dev_err_probe(dev, -EINVAL,
					     "%s must have decreasing raw and increasing values\n",
					     prop);
	}

	t->raw = raw;
	t->val = val;
	t->len = n / 2;

	return 0;
}

/*
 * The factory calibration in hard_config holds a point count, the
 * reference input voltage in 0.1 V units and the raw reading measured
 * at that voltage.
 */
static int mt555_read_calibration(struct mt555 *mt)
{
	const struct mt555_table *t = &mt->tables[MT555_VOLTAGE];
	struct device *dev = mt->dev;
	struct nvmem_cell *cell;
	u32 *cal, ref_raw;
	size_t len;
	u64 corr;

	mt->corr = MT555_CORR_ONE;

	cell = devm_nvmem_cell_get(dev, "calibration");
	if (IS_ERR(cell)) {
		if (PTR_ERR(cell) == -EPROBE_DEFER)
			return -EPROBE_DEFER;
		dev_info(dev, "no calibration, readings are uncorrected\n");
		return 0;
	}

	cal = nvmem_cell_read(cell, &len);
	if (IS_ERR(cal))
		return dev_err_probe(dev, PTR_ERR(cal),
				     "failed to read calibration\n");

	if (len < 3 * sizeof(*cal) || !cal[0] || !cal[1] || !cal[2] ||
	    cal[1] == U32_MAX || cal[2] == U32_MAX) {
		dev_warn(dev, "ignoring invalid calibration\n");
		goto out;
	}

	ref_raw = mt555_raw_for(t, cal[1] * 100);
	corr = div_u64((u64)ref_raw * MT555_CORR_ONE, cal[2]);
	if (corr < MT555_CORR_ONE / 2 || corr > MT555_CORR_ONE * 2) {
		dev_warn(dev, "ignoring out of range correction %llu\n", corr);
		goto out;
	}

	mt->corr = corr;
	dev_info(dev, "calibrated at %u mV, correction %u/%u\n",
		 cal[1] * 100, mt->corr, MT555_CORR_ONE);
out:
	kfree(cal);

	return 0;
}

static void mt555_stop(void *data)
{
	struct mt555 *mt = data;

	cancel_delayed_work_sync(&mt->work);
}

static int mt555_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	struct device *hwmon;
	struct mt555 *mt;
	int ret;

	mt = devm_kzalloc(dev, sizeof(*mt), GFP_KERNEL);
	if (!mt)
		return -ENOMEM;

	mt->dev = dev;
	spin_lock_init(&mt->lock);
	ret = devm_mutex_init(dev, &mt->result_lock);
	if (ret)
		return ret;

	ret = mt555_parse_table(mt, "mikrotik,voltage-table",
				&mt->tables[MT555_VOLTAGE]);
	if (ret)
		return ret;

	ret = mt555_parse_table(mt, "mikrotik,temperature-table",
				&mt->tables[MT555_TEMP]);
	if (ret)
		return ret;

	ret = mt555_read_calibration(mt);
	if (ret)
		return ret;

	mt->signal = devm_gpiod_get(dev, NULL, GPIOD_IN);
	if (IS_ERR(mt->signal))
		return dev_err_probe(dev, PTR_ERR(mt->signal),
				     "failed to get signal GPIO\n");

	mt->mux = devm_gpiod_get(dev, "mux", GPIOD_OUT_LOW);
	if (IS_ERR(mt->mux))
		return dev_err_probe(dev, PTR_ERR(mt->mux),
				     "failed to get mux GPIO\n");

	mt->irq = gpiod_to_irq(mt->signal);
	if (mt->irq < 0)
		return dev_err_probe(dev, mt->irq, "no IRQ for signal GPIO\n");

	/* Only sampled in short bursts, see mt555_measure() */
	ret = devm_request_irq(dev, mt->irq, mt555_irq,
			       IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING |
			       IRQF_NO_AUTOEN, dev_name(dev), mt);
	if (ret)
		return dev_err_probe(dev, ret, "failed to request IRQ\n");

	INIT_DELAYED_WORK(&mt->work, mt555_work);
	ret = devm_add_action_or_reset(dev, mt555_stop, mt);
	if (ret)
		return ret;

	hwmon = devm_hwmon_device_register_with_info(dev, "mikrotik_555", mt,
						     &mt555_chip_info, NULL);
	if (IS_ERR(hwmon))
		return PTR_ERR(hwmon);

	queue_delayed_work(system_freezable_wq, &mt->work, 0);

	return 0;
}

static const struct of_device_id mt555_match[] = {
	{ .compatible = "mikrotik,555-health-monitor" },
	{}
};
MODULE_DEVICE_TABLE(of, mt555_match);

static struct platform_driver mt555_driver = {
	.probe = mt555_probe,
	.driver = {
		.name = "mikrotik-555-hwmon",
		.of_match_table = mt555_match,
	},
};

module_platform_driver(mt555_driver);

MODULE_DESCRIPTION("MikroTik RouterBOARD 555 timer health monitor");
MODULE_LICENSE("GPL");

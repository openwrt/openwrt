/* Linux kernel driver for the ST LIS302D 3-axis accelerometer
 *
 * Copyright (C) 2007-2008 by Openmoko, Inc.
 * Author: Harald Welte <laforge@openmoko.org>
 *         converted to private bitbang by:
 *         Andy Green <andy@openmoko.com>
 *         ability to set acceleration threshold added by:
 *         Simon Kagstrom <simon.kagstrom@gmail.com>
 * All rights reserved.
 * Copyright (C) 2009, Lars-Peter Clausen <lars@metafoo.de>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 *
 * TODO
 * 	* statistics for overflow events
 * 	* configuration interface (sysfs) for
 * 		* enable/disable x/y/z axis data ready
 * 		* enable/disable resume from freee fall / click
 * 		* free fall / click parameters
 * 		* high pass filter parameters
 */
#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/platform_device.h>
#include <linux/delay.h>
#include <linux/irq.h>
#include <linux/interrupt.h>
#include <linux/sysfs.h>
#include <linux/spi/spi.h>

#include <linux/lis302dl.h>

static uint8_t lis_reg_read(struct lis302dl_info *lis, uint8_t reg)
{
    return spi_w8r8(lis->spi, 0xc0 | reg);
}

static void lis_reg_write(struct lis302dl_info *lis, uint8_t reg, uint8_t val)
{
	uint8_t data[2] = {reg, val};

    spi_write(lis->spi, data, sizeof(data));
}

static void lis_reg_set_bit_mask(struct lis302dl_info *lis, uint8_t reg, uint8_t mask,
		uint8_t val)
{
	uint8_t tmp;

	val &= mask;

	tmp = lis_reg_read(lis, reg);
	tmp &= ~mask;
	tmp |= val;
	lis_reg_write(lis, reg, tmp);
}

static int __ms_to_duration(struct lis302dl_info *lis, int ms)
{
	/* If we have 400 ms sampling rate, the stepping is 2.5 ms,
	 * on 100 ms the stepping is 10ms */
	if (lis->flags & LIS302DL_F_DR)
		return min((ms * 10) / 25, 637);

	return min(ms / 10, 2550);
}

static int __duration_to_ms(struct lis302dl_info *lis, int duration)
{
	if (lis->flags & LIS302DL_F_DR)
		return (duration * 25) / 10;

	return duration * 10;
}

static uint8_t __mg_to_threshold(struct lis302dl_info *lis, int mg)
{
	/* If FS is set each bit is 71mg, otherwise 18mg. The THS register
	 * has 7 bits for the threshold value */
	if (lis->flags & LIS302DL_F_FS)
		return min(mg / 71, 127);

	return min(mg / 18, 127);
}

static int __threshold_to_mg(struct lis302dl_info *lis, uint8_t threshold)
{
	if (lis->flags & LIS302DL_F_FS)
		return threshold * 71;

	return threshold * 18;
}

/* interrupt handling related */

enum lis302dl_intmode {
	LIS302DL_INTMODE_GND		= 0x00,
	LIS302DL_INTMODE_FF_WU_1	= 0x01,
	LIS302DL_INTMODE_FF_WU_2	= 0x02,
	LIS302DL_INTMODE_FF_WU_12	= 0x03,
	LIS302DL_INTMODE_DATA_READY	= 0x04,
	LIS302DL_INTMODE_CLICK		= 0x07,
};

static void lis302dl_set_int_mode(struct device *dev, int int_pin,
			      enum lis302dl_intmode mode)
{
	struct lis302dl_info *lis = dev_get_drvdata(dev);
	switch (int_pin) {
	case 1:
		lis_reg_set_bit_mask(lis, LIS302DL_REG_CTRL3, 0x07, mode);
		break;
	case 2:
		lis_reg_set_bit_mask(lis, LIS302DL_REG_CTRL3, 0x38, mode << 3);
		break;
	default:
		BUG();
	}
}

static void __enable_wakeup(struct lis302dl_info *lis)
{
	lis_reg_write(lis, LIS302DL_REG_CTRL1, 0);

	/* First zero to get to a known state */
	lis_reg_write(lis, LIS302DL_REG_FF_WU_CFG_1, LIS302DL_FFWUCFG_XHIE |
			LIS302DL_FFWUCFG_YHIE | LIS302DL_FFWUCFG_ZHIE |
			LIS302DL_FFWUCFG_LIR);
	lis_reg_write(lis, LIS302DL_REG_FF_WU_THS_1,
			__mg_to_threshold(lis, lis->wakeup.threshold));
	lis_reg_write(lis, LIS302DL_REG_FF_WU_DURATION_1,
			__ms_to_duration(lis, lis->wakeup.duration));

	/* Route the interrupt for wakeup */
	lis302dl_set_int_mode(lis->dev, 1,
			LIS302DL_INTMODE_FF_WU_1);

	lis_reg_read(lis, LIS302DL_REG_HP_FILTER_RESET);
	lis_reg_read(lis, LIS302DL_REG_OUT_X);
	lis_reg_read(lis, LIS302DL_REG_OUT_Y);
	lis_reg_read(lis, LIS302DL_REG_OUT_Z);
	lis_reg_read(lis, LIS302DL_REG_STATUS);
	lis_reg_read(lis, LIS302DL_REG_FF_WU_SRC_1);
	lis_reg_read(lis, LIS302DL_REG_FF_WU_SRC_2);
	lis_reg_write(lis, LIS302DL_REG_CTRL1, LIS302DL_CTRL1_PD | 7);
}

static void __enable_data_collection(struct lis302dl_info *lis)
{
	u_int8_t ctrl1 = LIS302DL_CTRL1_PD | LIS302DL_CTRL1_Xen |
			 LIS302DL_CTRL1_Yen | LIS302DL_CTRL1_Zen;

	/* make sure we're powered up and generate data ready */
	lis_reg_set_bit_mask(lis, LIS302DL_REG_CTRL1, ctrl1, ctrl1);

	/* If the threshold is zero, let the device generated an interrupt
	 * on each datum */
	if (lis->threshold == 0) {
		lis_reg_write(lis, LIS302DL_REG_CTRL2, 0);
		lis302dl_set_int_mode(lis->dev, 1, LIS302DL_INTMODE_DATA_READY);
		lis302dl_set_int_mode(lis->dev, 2, LIS302DL_INTMODE_DATA_READY);
	} else {
		lis_reg_write(lis, LIS302DL_REG_CTRL2,
				LIS302DL_CTRL2_HPFF1);
		lis_reg_write(lis, LIS302DL_REG_FF_WU_THS_1,
				__mg_to_threshold(lis, lis->threshold));
		lis_reg_write(lis, LIS302DL_REG_FF_WU_DURATION_1,
				__ms_to_duration(lis, lis->duration));

		/* Clear the HP filter "starting point" */
		lis_reg_read(lis, LIS302DL_REG_HP_FILTER_RESET);
		lis_reg_write(lis, LIS302DL_REG_FF_WU_CFG_1,
				LIS302DL_FFWUCFG_XHIE | LIS302DL_FFWUCFG_YHIE |
				LIS302DL_FFWUCFG_ZHIE | LIS302DL_FFWUCFG_LIR);
		lis302dl_set_int_mode(lis->dev, 1, LIS302DL_INTMODE_FF_WU_12);
		lis302dl_set_int_mode(lis->dev, 2, LIS302DL_INTMODE_FF_WU_12);
	}
}

#if 0
static void _report_btn_single(struct input_dev *inp, int btn)
{
	input_report_key(inp, btn, 1);
	input_sync(inp);
	input_report_key(inp, btn, 0);
}

static void _report_btn_double(struct input_dev *inp, int btn)
{
	input_report_key(inp, btn, 1);
	input_sync(inp);
	input_report_key(inp, btn, 0);
	input_sync(inp);
	input_report_key(inp, btn, 1);
	input_sync(inp);
	input_report_key(inp, btn, 0);
}
#endif


static void lis302dl_bitbang_read_sample(struct lis302dl_info *lis)
{
	uint8_t data[(LIS302DL_REG_OUT_Z - LIS302DL_REG_STATUS) + 2] = {0xC0 | LIS302DL_REG_STATUS};
	uint8_t *read = data + 1;
	unsigned long flags;
	int mg_per_sample = __threshold_to_mg(lis, 1);
	struct spi_message msg;
	struct spi_transfer t;

	spi_message_init(&msg);
	memset(&t, 0, sizeof t);
	t.len = sizeof(data);
	spi_message_add_tail(&t, &msg);
	t.tx_buf = &data[0];
	t.rx_buf = &data[0];

	/* grab the set of register containing status and XYZ data */
	local_irq_save(flags);
	/* Should complete without blocking */
	if (spi_sync(lis->spi, &msg) < 0)
		dev_err(lis->dev, "Error reading registers\n");

	local_irq_restore(flags);
	/*
	 * at the minute the test below fails 50% of the time due to
	 * a problem with level interrupts causing ISRs to get called twice.
	 * This is a workaround for that, but actually this test is still
	 * valid and the information can be used for overrrun stats.
	 */

	/* has any kind of overrun been observed by the lis302dl? */
	if (read[0] & (LIS302DL_STATUS_XOR |
		       LIS302DL_STATUS_YOR |
		       LIS302DL_STATUS_ZOR))
		lis->overruns++;

	/* we have a valid sample set? */
	if (read[0] & LIS302DL_STATUS_XYZDA) {
		input_report_abs(lis->input_dev, ABS_X, mg_per_sample *
			    (s8)read[LIS302DL_REG_OUT_X - LIS302DL_REG_STATUS]);
		input_report_abs(lis->input_dev, ABS_Y, mg_per_sample *
			    (s8)read[LIS302DL_REG_OUT_Y - LIS302DL_REG_STATUS]);
		input_report_abs(lis->input_dev, ABS_Z, mg_per_sample *
			    (s8)read[LIS302DL_REG_OUT_Z - LIS302DL_REG_STATUS]);

		input_sync(lis->input_dev);
	} else {
        printk("invalid sample\n");
    }

	if (lis->threshold)
		/* acknowledge the wakeup source */
		lis_reg_read(lis,	LIS302DL_REG_FF_WU_SRC_1);
	enable_irq(lis->pdata->interrupt);
}

static void lis302dl_irq_worker(struct work_struct *work) {
	struct lis302dl_info *lis = container_of(work, struct lis302dl_info, work);
	lis302dl_bitbang_read_sample(lis);
}

static irqreturn_t lis302dl_interrupt(int irq, void *_lis)
{
	struct lis302dl_info *lis = _lis;
	disable_irq_nosync(lis->pdata->interrupt);
	schedule_work(&lis->work);
    return IRQ_HANDLED;
}

/* sysfs */

static ssize_t show_overruns(struct device *dev, struct device_attribute *attr,
			 char *buf)
{
	struct lis302dl_info *lis = dev_get_drvdata(dev);

	return sprintf(buf, "%u\n", lis->overruns);
}

static DEVICE_ATTR(overruns, S_IRUGO, show_overruns, NULL);

static ssize_t show_rate(struct device *dev, struct device_attribute *attr,
			 char *buf)
{
	struct lis302dl_info *lis = dev_get_drvdata(dev);
	uint8_t ctrl1;
	unsigned long flags;

	local_irq_save(flags);
	ctrl1 = lis_reg_read(lis, LIS302DL_REG_CTRL1);
	local_irq_restore(flags);

	return sprintf(buf, "%d\n", ctrl1 & LIS302DL_CTRL1_DR ? 400 : 100);
}

static ssize_t set_rate(struct device *dev, struct device_attribute *attr,
			const char *buf, size_t count)
{
	struct lis302dl_info *lis = dev_get_drvdata(dev);
	unsigned long flags;

	local_irq_save(flags);

	if (!strcmp(buf, "400\n")) {
		lis_reg_set_bit_mask(lis, LIS302DL_REG_CTRL1, LIS302DL_CTRL1_DR,
				 LIS302DL_CTRL1_DR);
		lis->flags |= LIS302DL_F_DR;
	} else {
		lis_reg_set_bit_mask(lis, LIS302DL_REG_CTRL1, LIS302DL_CTRL1_DR,
				0);
		lis->flags &= ~LIS302DL_F_DR;
	}
	local_irq_restore(flags);

	return count;
}

static DEVICE_ATTR(sample_rate, S_IRUGO | S_IWUSR, show_rate, set_rate);

static ssize_t show_scale(struct device *dev, struct device_attribute *attr,
			  char *buf)
{
	struct lis302dl_info *lis = dev_get_drvdata(dev);
	u_int8_t ctrl1;
	unsigned long flags;

	local_irq_save(flags);
	ctrl1 = lis_reg_read(lis, LIS302DL_REG_CTRL1);
	local_irq_restore(flags);

	return sprintf(buf, "%s\n", ctrl1 & LIS302DL_CTRL1_FS ? "9.2" : "2.3");
}

static ssize_t set_scale(struct device *dev, struct device_attribute *attr,
			 const char *buf, size_t count)
{
	struct lis302dl_info *lis = dev_get_drvdata(dev);
	unsigned long flags;

	local_irq_save(flags);

	if (!strcmp(buf, "9.2\n")) {
		lis_reg_set_bit_mask(lis, LIS302DL_REG_CTRL1, LIS302DL_CTRL1_FS,
				 LIS302DL_CTRL1_FS);
		lis->flags |= LIS302DL_F_FS;
	} else {
		lis_reg_set_bit_mask(lis, LIS302DL_REG_CTRL1, LIS302DL_CTRL1_FS,
				0);
		lis->flags &= ~LIS302DL_F_FS;
	}

	if (lis->flags & LIS302DL_F_INPUT_OPEN)
		__enable_data_collection(lis);

	local_irq_restore(flags);

	return count;
}

static DEVICE_ATTR(full_scale, S_IRUGO | S_IWUSR, show_scale, set_scale);

static ssize_t show_threshold(struct device *dev, struct device_attribute *attr,
		 char *buf)
{
	struct lis302dl_info *lis = dev_get_drvdata(dev);

	/* Display the device view of the threshold setting */
	return sprintf(buf, "%d\n", __threshold_to_mg(lis,
			__mg_to_threshold(lis, lis->threshold)));
}

static ssize_t set_threshold(struct device *dev, struct device_attribute *attr,
		 const char *buf, size_t count)
{
	struct lis302dl_info *lis = dev_get_drvdata(dev);
	unsigned int val;

	if (sscanf(buf, "%u\n", &val) != 1)
		return -EINVAL;
	/* 8g is the maximum if FS is 1 */
	if (val > 8000)
		return -ERANGE;

	/* Set the threshold and write it out if the device is used */
	lis->threshold = val;

	if (lis->flags & LIS302DL_F_INPUT_OPEN) {
		unsigned long flags;

		local_irq_save(flags);
		__enable_data_collection(lis);
		local_irq_restore(flags);
	}

	return count;
}

static DEVICE_ATTR(threshold, S_IRUGO | S_IWUSR, show_threshold, set_threshold);

static ssize_t show_duration(struct device *dev, struct device_attribute *attr,
		 char *buf)
{
	struct lis302dl_info *lis = dev_get_drvdata(dev);

	return sprintf(buf, "%d\n", __duration_to_ms(lis,
			__ms_to_duration(lis, lis->duration)));
}

static ssize_t set_duration(struct device *dev, struct device_attribute *attr,
		 const char *buf, size_t count)
{
	struct lis302dl_info *lis = dev_get_drvdata(dev);
	unsigned int val;

	if (sscanf(buf, "%u\n", &val) != 1)
		return -EINVAL;
	if (val > 2550)
		return -ERANGE;

	lis->duration = val;
	if (lis->flags & LIS302DL_F_INPUT_OPEN)
		lis_reg_write(lis, LIS302DL_REG_FF_WU_DURATION_1,
				__ms_to_duration(lis, lis->duration));

	return count;
}

static DEVICE_ATTR(duration, S_IRUGO | S_IWUSR, show_duration, set_duration);

static ssize_t lis302dl_dump(struct device *dev, struct device_attribute *attr,
								      char *buf)
{
	struct lis302dl_info *lis = dev_get_drvdata(dev);
	int n = 0;
	uint8_t reg[0x40];
	char *end = buf;
	unsigned long flags;

	local_irq_save(flags);

	for (n = 0; n < sizeof(reg); n++)
		reg[n] = lis_reg_read(lis, n);

	local_irq_restore(flags);

	for (n = 0; n < sizeof(reg); n += 16) {
		hex_dump_to_buffer(reg + n, 16, 16, 1, end, 128, 0);
		end += strlen(end);
		*end++ = '\n';
		*end++ = '\0';
	}

	return end - buf;
}
static DEVICE_ATTR(dump, S_IRUGO, lis302dl_dump, NULL);

/* Configure freefall/wakeup interrupts */
static ssize_t set_wakeup_threshold(struct device *dev,
		struct device_attribute *attr, const char *buf, size_t count)
{
	struct lis302dl_info *lis = dev_get_drvdata(dev);
	unsigned int threshold;

	if (sscanf(buf, "%u\n", &threshold) != 1)
		return -EINVAL;

	if (threshold > 8000)
		return -ERANGE;

	/* Zero turns the feature off */
	if (threshold == 0) {
		if (lis->flags & LIS302DL_F_IRQ_WAKE) {
			disable_irq_wake(lis->pdata->interrupt);
			lis->flags &= ~LIS302DL_F_IRQ_WAKE;
		}

		return count;
	}

	lis->wakeup.threshold = threshold;

	if (!(lis->flags & LIS302DL_F_IRQ_WAKE)) {
		enable_irq_wake(lis->pdata->interrupt);
		lis->flags |= LIS302DL_F_IRQ_WAKE;
	}

	return count;
}

static ssize_t show_wakeup_threshold(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	struct lis302dl_info *lis = dev_get_drvdata(dev);

	/* All events off? */
	if (lis->wakeup.threshold == 0)
		return sprintf(buf, "off\n");

	return sprintf(buf, "%u\n", lis->wakeup.threshold);
}

static DEVICE_ATTR(wakeup_threshold, S_IRUGO | S_IWUSR, show_wakeup_threshold,
		set_wakeup_threshold);

static ssize_t set_wakeup_duration(struct device *dev,
		struct device_attribute *attr, const char *buf, size_t count)
{
	struct lis302dl_info *lis = dev_get_drvdata(dev);
	unsigned int duration;

	if (sscanf(buf, "%u\n", &duration) != 1)
		return -EINVAL;

	if (duration > 2550)
		return -ERANGE;

	lis->wakeup.duration = duration;

	return count;
}

static ssize_t show_wakeup_duration(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	struct lis302dl_info *lis = dev_get_drvdata(dev);

	return sprintf(buf, "%u\n", lis->wakeup.duration);
}

static DEVICE_ATTR(wakeup_duration, S_IRUGO | S_IWUSR, show_wakeup_duration,
		set_wakeup_duration);

static struct attribute *lis302dl_sysfs_entries[] = {
	&dev_attr_sample_rate.attr,
	&dev_attr_full_scale.attr,
	&dev_attr_threshold.attr,
	&dev_attr_duration.attr,
	&dev_attr_dump.attr,
	&dev_attr_wakeup_threshold.attr,
	&dev_attr_wakeup_duration.attr,
	&dev_attr_overruns.attr,
	NULL
};

static struct attribute_group lis302dl_attr_group = {
	.name	= NULL,
	.attrs	= lis302dl_sysfs_entries,
};

/* input device handling and driver core interaction */

static int lis302dl_input_open(struct input_dev *inp)
{
	struct lis302dl_info *lis = input_get_drvdata(inp);
	unsigned long flags;

	local_irq_save(flags);

	__enable_data_collection(lis);
	lis->flags |= LIS302DL_F_INPUT_OPEN;

	local_irq_restore(flags);

	return 0;
}

static void lis302dl_input_close(struct input_dev *inp)
{
	struct lis302dl_info *lis = input_get_drvdata(inp);
	u_int8_t ctrl1 = LIS302DL_CTRL1_Xen | LIS302DL_CTRL1_Yen |
			 LIS302DL_CTRL1_Zen;
	unsigned long flags;

	local_irq_save(flags);

	/* since the input core already serializes access and makes sure we
	 * only see close() for the close of the last user, we can safely
	 * disable the data ready events */
	lis_reg_set_bit_mask(lis, LIS302DL_REG_CTRL1, ctrl1, 0x00);
	lis->flags &= ~LIS302DL_F_INPUT_OPEN;

	/* however, don't power down the whole device if still needed */
	if (!(lis->flags & LIS302DL_F_WUP_FF ||
	      lis->flags & LIS302DL_F_WUP_CLICK)) {
		lis_reg_set_bit_mask(lis, LIS302DL_REG_CTRL1, LIS302DL_CTRL1_PD,
				 0x00);
	}
	local_irq_restore(flags);
}

/* get the device to reload its coefficients from EEPROM and wait for it
 * to complete
 */

static int __lis302dl_reset_device(struct lis302dl_info *lis)
{
	int timeout = 10;

	lis_reg_write(lis, LIS302DL_REG_CTRL2,
			LIS302DL_CTRL2_BOOT | LIS302DL_CTRL2_FDS);

	while ((lis_reg_read(lis, LIS302DL_REG_CTRL2)
			& LIS302DL_CTRL2_BOOT) && (timeout--))
		mdelay(1);

	return !!(timeout < 0);
}

static int __devinit lis302dl_probe(struct spi_device *spi)
{
	int rc;
	struct lis302dl_info *lis;
	u_int8_t wai;
	unsigned long flags;
	struct lis302dl_platform_data *pdata = spi->dev.platform_data;

	spi->mode = SPI_MODE_3;
	rc = spi_setup(spi);
	if (rc < 0) {
		dev_err(&spi->dev, "spi_setup failed\n");
		return rc;
	}

	lis = kzalloc(sizeof(*lis), GFP_KERNEL);
	if (!lis)
		return -ENOMEM;

	lis->dev = &spi->dev;
	lis->spi = spi;

	dev_set_drvdata(lis->dev, lis);

	lis->pdata = pdata;

	rc = sysfs_create_group(&lis->dev->kobj, &lis302dl_attr_group);
	if (rc) {
		dev_err(lis->dev, "error creating sysfs group\n");
		goto bail_free_lis;
	}

	/* initialize input layer details */
	lis->input_dev = input_allocate_device();
	if (!lis->input_dev) {
		dev_err(lis->dev, "Unable to allocate input device\n");
		goto bail_sysfs;
	}

	input_set_drvdata(lis->input_dev, lis);
	lis->input_dev->name = pdata->name;
	 /* SPI Bus not defined as a valid bus for input subsystem*/
	lis->input_dev->id.bustype = BUS_I2C; /* lie about it */
	lis->input_dev->open = lis302dl_input_open;
	lis->input_dev->close = lis302dl_input_close;

	rc = input_register_device(lis->input_dev);
	if (rc) {
		dev_err(lis->dev, "error %d registering input device\n", rc);
		goto bail_inp_dev;
	}

	local_irq_save(flags);
	/* Configure our IO */
	(lis->pdata->lis302dl_suspend_io)(lis, 1);

	wai = lis_reg_read(lis, LIS302DL_REG_WHO_AM_I);
	if (wai != LIS302DL_WHO_AM_I_MAGIC) {
		dev_err(lis->dev, "unknown who_am_i signature 0x%02x\n", wai);
		dev_set_drvdata(lis->dev, NULL);
		rc = -ENODEV;
		local_irq_restore(flags);
		goto bail_inp_reg;
	}

	set_bit(EV_ABS, lis->input_dev->evbit);
	input_set_abs_params(lis->input_dev, ABS_X, 0, 0, 0, 0);
	input_set_abs_params(lis->input_dev, ABS_Y, 0, 0, 0, 0);
	input_set_abs_params(lis->input_dev, ABS_Z, 0, 0, 0, 0);

	lis->threshold = 0;
	lis->duration = 0;
	memset(&lis->wakeup, 0, sizeof(lis->wakeup));

	if (__lis302dl_reset_device(lis))
		dev_err(lis->dev, "device BOOT reload failed\n");

	/* force us powered */
	lis_reg_write(lis, LIS302DL_REG_CTRL1, LIS302DL_CTRL1_PD |
			LIS302DL_CTRL1_Xen |
			LIS302DL_CTRL1_Yen |
			LIS302DL_CTRL1_Zen);
	mdelay(1);

	lis_reg_write(lis, LIS302DL_REG_CTRL2, 0);
	lis_reg_write(lis, LIS302DL_REG_CTRL3,
			LIS302DL_CTRL3_PP_OD | LIS302DL_CTRL3_IHL);
	lis_reg_write(lis, LIS302DL_REG_FF_WU_THS_1, 0x0);
	lis_reg_write(lis, LIS302DL_REG_FF_WU_DURATION_1, 0x00);
	lis_reg_write(lis, LIS302DL_REG_FF_WU_CFG_1, 0x0);

	/* start off in powered down mode; we power up when someone opens us */
	lis_reg_write(lis, LIS302DL_REG_CTRL1, LIS302DL_CTRL1_Xen |
			LIS302DL_CTRL1_Yen | LIS302DL_CTRL1_Zen);

	if (pdata->open_drain)
		/* switch interrupt to open collector, active-low */
		lis_reg_write(lis, LIS302DL_REG_CTRL3,
				LIS302DL_CTRL3_PP_OD | LIS302DL_CTRL3_IHL);
	else
		/* push-pull, active-low */
		lis_reg_write(lis, LIS302DL_REG_CTRL3, LIS302DL_CTRL3_IHL);

	lis302dl_set_int_mode(lis->dev, 1, LIS302DL_INTMODE_GND);
	lis302dl_set_int_mode(lis->dev, 2, LIS302DL_INTMODE_GND);

	lis_reg_read(lis, LIS302DL_REG_STATUS);
	lis_reg_read(lis, LIS302DL_REG_FF_WU_SRC_1);
	lis_reg_read(lis, LIS302DL_REG_FF_WU_SRC_2);
	lis_reg_read(lis, LIS302DL_REG_CLICK_SRC);
	local_irq_restore(flags);

	dev_info(lis->dev, "Found %s\n", pdata->name);

	lis->pdata = pdata;

    INIT_WORK(&lis->work, lis302dl_irq_worker);

    set_irq_handler(lis->pdata->interrupt, handle_level_irq);
    set_irq_type(lis->pdata->interrupt, IRQ_TYPE_LEVEL_LOW);
	rc = request_irq(lis->pdata->interrupt, lis302dl_interrupt,
			 IRQF_TRIGGER_FALLING, "lis302dl", lis);

	if (rc < 0) {
		dev_err(lis->dev, "error requesting IRQ %d\n",
			lis->pdata->interrupt);
		goto bail_inp_reg;
	}
	return 0;

bail_inp_reg:
	input_unregister_device(lis->input_dev);
bail_inp_dev:
	input_free_device(lis->input_dev);
bail_sysfs:
	sysfs_remove_group(&lis->dev->kobj, &lis302dl_attr_group);
bail_free_lis:
	kfree(lis);
	return rc;
}

static int __devexit lis302dl_remove(struct spi_device *spi)
{
	struct lis302dl_info *lis = dev_get_drvdata(&spi->dev);
	unsigned long flags;

	/* Disable interrupts */
	if (lis->flags & LIS302DL_F_IRQ_WAKE)
		disable_irq_wake(lis->pdata->interrupt);
	free_irq(lis->pdata->interrupt, lis);

	/* Reset and power down the device */
	local_irq_save(flags);
	lis_reg_write(lis, LIS302DL_REG_CTRL3, 0x00);
	lis_reg_write(lis, LIS302DL_REG_CTRL2, 0x00);
	lis_reg_write(lis, LIS302DL_REG_CTRL1, 0x00);
	local_irq_restore(flags);

	/* Cleanup resources */
	sysfs_remove_group(&spi->dev.kobj, &lis302dl_attr_group);
	input_unregister_device(lis->input_dev);
	if (lis->input_dev)
		input_free_device(lis->input_dev);
	dev_set_drvdata(lis->dev, NULL);
	kfree(lis);

	return 0;
}

#ifdef CONFIG_PM

static uint8_t regs_to_save[] = {
	LIS302DL_REG_CTRL1,
	LIS302DL_REG_CTRL2,
	LIS302DL_REG_CTRL3,
	LIS302DL_REG_FF_WU_CFG_1,
	LIS302DL_REG_FF_WU_THS_1,
	LIS302DL_REG_FF_WU_DURATION_1,
	LIS302DL_REG_FF_WU_CFG_2,
	LIS302DL_REG_FF_WU_THS_2,
	LIS302DL_REG_FF_WU_DURATION_2,
	LIS302DL_REG_CLICK_CFG,
	LIS302DL_REG_CLICK_THSY_X,
	LIS302DL_REG_CLICK_THSZ,
	LIS302DL_REG_CLICK_TIME_LIMIT,
	LIS302DL_REG_CLICK_LATENCY,
	LIS302DL_REG_CLICK_WINDOW,

};

static int lis302dl_suspend(struct spi_device *spi, pm_message_t state)
{
	struct lis302dl_info *lis = dev_get_drvdata(&spi->dev);
	unsigned long flags;
	u_int8_t tmp;
	int n;

	/* determine if we want to wake up from the accel. */
	if (lis->flags & LIS302DL_F_WUP_CLICK)
		return 0;

	disable_irq(lis->pdata->interrupt);
	local_irq_save(flags);

	/*
	 * When we share SPI over multiple sensors, there is a race here
	 * that one or more sensors will lose.  In that case, the shared
	 * SPI bus GPIO will be in sleep mode and partially pulled down.  So
	 * we explicitly put our IO into "wake" mode here before the final
	 * traffic to the sensor.
	 */
	(lis->pdata->lis302dl_suspend_io)(lis, 1);

	/* save registers */
	for (n = 0; n < ARRAY_SIZE(regs_to_save); n++)
		lis->regs[regs_to_save[n]] =
			lis_reg_read(lis, regs_to_save[n]);

	/* power down or enable wakeup */

	if (lis->wakeup.threshold == 0) {
		tmp = lis_reg_read(lis, LIS302DL_REG_CTRL1);
		tmp &= ~LIS302DL_CTRL1_PD;
		lis_reg_write(lis, LIS302DL_REG_CTRL1, tmp);
	} else
		__enable_wakeup(lis);

	/* place our IO to the device in sleep-compatible states */
	(lis->pdata->lis302dl_suspend_io)(lis, 0);

	local_irq_restore(flags);

	return 0;
}

static int lis302dl_resume(struct spi_device *spi)
{
	struct lis302dl_info *lis = dev_get_drvdata(&spi->dev);
	unsigned long flags;
	int n;

	if (lis->flags & LIS302DL_F_WUP_CLICK)
		return 0;

	local_irq_save(flags);

	/* get our IO to the device back in operational states */
	(lis->pdata->lis302dl_suspend_io)(lis, 1);

	/* resume from powerdown first! */
	lis_reg_write(lis, LIS302DL_REG_CTRL1,
			LIS302DL_CTRL1_PD |
			LIS302DL_CTRL1_Xen |
			LIS302DL_CTRL1_Yen |
			LIS302DL_CTRL1_Zen);
	mdelay(1);

	if (__lis302dl_reset_device(lis))
		dev_err(&spi->dev, "device BOOT reload failed\n");

	lis->regs[LIS302DL_REG_CTRL1] |=	LIS302DL_CTRL1_PD |
						LIS302DL_CTRL1_Xen |
						LIS302DL_CTRL1_Yen |
						LIS302DL_CTRL1_Zen;

	/* restore registers after resume */
	for (n = 0; n < ARRAY_SIZE(regs_to_save); n++)
		lis_reg_write(lis, regs_to_save[n], lis->regs[regs_to_save[n]]);

	/* if someone had us open, reset the non-wake threshold stuff */
	if (lis->flags & LIS302DL_F_INPUT_OPEN)
		__enable_data_collection(lis);

	local_irq_restore(flags);
	enable_irq(lis->pdata->interrupt);

	return 0;
}
#else
#define lis302dl_suspend	NULL
#define lis302dl_resume		NULL
#endif

static struct spi_driver lis302dl_spi_driver = {
	.driver = {
		.name = "lis302dl",
		.owner = THIS_MODULE,
	},

	.probe 	= lis302dl_probe,
	.remove	= __devexit_p(lis302dl_remove),
	.suspend = lis302dl_suspend,
	.resume	 = lis302dl_resume,
};

static int __devinit lis302dl_init(void)
{
	return spi_register_driver(&lis302dl_spi_driver);
}
module_init(lis302dl_init);

static void __exit lis302dl_exit(void)
{
	spi_unregister_driver(&lis302dl_spi_driver);
}
module_exit(lis302dl_exit);

MODULE_AUTHOR("Harald Welte <laforge@openmoko.org>");
MODULE_LICENSE("GPL");

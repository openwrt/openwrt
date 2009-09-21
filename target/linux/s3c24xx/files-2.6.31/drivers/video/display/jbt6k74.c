/* Linux kernel driver for the tpo JBT6K74-AS LCM ASIC
 *
 * Copyright (C) 2006-2007 by Openmoko, Inc.
 * Author: Harald Welte <laforge@openmoko.org>,
 * 	   Stefan Schmidt <stefan@openmoko.org>
 * Copyright (C) 2008 by Harald Welte <laforge@openmoko.org>
 * Copyright (C) 2009 by Lars-Peter Clausen <lars@metafoo.de>
 * All rights reserved.
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
 */

#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/platform_device.h>
#include <linux/delay.h>
#include <linux/workqueue.h>
#include <linux/jbt6k74.h>
#include <linux/fb.h>
#include <linux/lcd.h>
#include <linux/time.h>

enum jbt_register {
	JBT_REG_SLEEP_IN		= 0x10,
	JBT_REG_SLEEP_OUT		= 0x11,

	JBT_REG_DISPLAY_OFF		= 0x28,
	JBT_REG_DISPLAY_ON		= 0x29,

	JBT_REG_RGB_FORMAT		= 0x3a,
	JBT_REG_QUAD_RATE		= 0x3b,

	JBT_REG_POWER_ON_OFF		= 0xb0,
	JBT_REG_BOOSTER_OP		= 0xb1,
	JBT_REG_BOOSTER_MODE		= 0xb2,
	JBT_REG_BOOSTER_FREQ		= 0xb3,
	JBT_REG_OPAMP_SYSCLK		= 0xb4,
	JBT_REG_VSC_VOLTAGE		= 0xb5,
	JBT_REG_VCOM_VOLTAGE		= 0xb6,
	JBT_REG_EXT_DISPL		= 0xb7,
	JBT_REG_OUTPUT_CONTROL		= 0xb8,
	JBT_REG_DCCLK_DCEV		= 0xb9,
	JBT_REG_DISPLAY_MODE1		= 0xba,
	JBT_REG_DISPLAY_MODE2		= 0xbb,
	JBT_REG_DISPLAY_MODE		= 0xbc,
	JBT_REG_ASW_SLEW		= 0xbd,
	JBT_REG_DUMMY_DISPLAY		= 0xbe,
	JBT_REG_DRIVE_SYSTEM		= 0xbf,

	JBT_REG_SLEEP_OUT_FR_A		= 0xc0,
	JBT_REG_SLEEP_OUT_FR_B		= 0xc1,
	JBT_REG_SLEEP_OUT_FR_C		= 0xc2,
	JBT_REG_SLEEP_IN_LCCNT_D	= 0xc3,
	JBT_REG_SLEEP_IN_LCCNT_E	= 0xc4,
	JBT_REG_SLEEP_IN_LCCNT_F	= 0xc5,
	JBT_REG_SLEEP_IN_LCCNT_G	= 0xc6,

	JBT_REG_GAMMA1_FINE_1		= 0xc7,
	JBT_REG_GAMMA1_FINE_2		= 0xc8,
	JBT_REG_GAMMA1_INCLINATION	= 0xc9,
	JBT_REG_GAMMA1_BLUE_OFFSET	= 0xca,

	/* VGA */
	JBT_REG_BLANK_CONTROL		= 0xcf,
	JBT_REG_BLANK_TH_TV		= 0xd0,
	JBT_REG_CKV_ON_OFF		= 0xd1,
	JBT_REG_CKV_1_2			= 0xd2,
	JBT_REG_OEV_TIMING		= 0xd3,
	JBT_REG_ASW_TIMING_1		= 0xd4,
	JBT_REG_ASW_TIMING_2		= 0xd5,

	/* QVGA */
	JBT_REG_BLANK_CONTROL_QVGA	= 0xd6,
	JBT_REG_BLANK_TH_TV_QVGA	= 0xd7,
	JBT_REG_CKV_ON_OFF_QVGA		= 0xd8,
	JBT_REG_CKV_1_2_QVGA		= 0xd9,
	JBT_REG_OEV_TIMING_QVGA		= 0xde,
	JBT_REG_ASW_TIMING_1_QVGA	= 0xdf,
	JBT_REG_ASW_TIMING_2_QVGA	= 0xe0,


	JBT_REG_HCLOCK_VGA		= 0xec,
	JBT_REG_HCLOCK_QVGA		= 0xed,

};

enum jbt_resolution {
	JBT_RESOLUTION_VGA,
	JBT_RESOLUTION_QVGA,
};

enum jbt_power_mode {
	JBT_POWER_MODE_DEEP_STANDBY,
	JBT_POWER_MODE_SLEEP,
	JBT_POWER_MODE_NORMAL,
};

static const char *jbt_power_mode_names[] = {
	[JBT_POWER_MODE_DEEP_STANDBY]	= "deep-standby",
	[JBT_POWER_MODE_SLEEP]		= "sleep",
	[JBT_POWER_MODE_NORMAL]		= "normal",
};

static const char *jbt_resolution_names[] = {
	[JBT_RESOLUTION_VGA] = "vga",
	[JBT_RESOLUTION_QVGA] = "qvga",
};

struct jbt_info {
	struct mutex lock;		/* protects this structure */
	enum jbt_resolution resolution;
	enum jbt_power_mode power_mode;
	enum jbt_power_mode suspend_mode;
	int suspended;
	struct spi_device *spi_dev;
	struct lcd_device *lcd_dev;
	unsigned long last_sleep;
	struct delayed_work blank_work;
	int blank_mode;
	u16 tx_buf[4];
	u16 reg_cache[0xEE];
};

#define JBT_COMMAND	0x000
#define JBT_DATA	0x100

static int jbt_reg_write_nodata(struct jbt_info *jbt, u8 reg)
{
	int rc;

	jbt->tx_buf[0] = JBT_COMMAND | reg;
	rc = spi_write(jbt->spi_dev, (u8 *)jbt->tx_buf,
	               1*sizeof(u16));
	if (rc == 0)
		jbt->reg_cache[reg] = 0;
	else
		dev_err(&jbt->spi_dev->dev, "jbt_reg_write_nodata spi_write ret %d\n",
		       rc);

	return rc;
}


static int jbt_reg_write(struct jbt_info *jbt, u8 reg, u8 data)
{
	int rc;

	jbt->tx_buf[0] = JBT_COMMAND | reg;
	jbt->tx_buf[1] = JBT_DATA | data;
	rc = spi_write(jbt->spi_dev, (u8 *)jbt->tx_buf,
	               2*sizeof(u16));
	if (rc == 0)
		jbt->reg_cache[reg] = data;
	else
		dev_err(&jbt->spi_dev->dev, "jbt_reg_write spi_write ret %d\n", rc);

	return rc;
}

static int jbt_reg_write16(struct jbt_info *jbt, u8 reg, u16 data)
{
	int rc;

	jbt->tx_buf[0] = JBT_COMMAND | reg;
	jbt->tx_buf[1] = JBT_DATA | (data >> 8);
	jbt->tx_buf[2] = JBT_DATA | (data & 0xff);

	rc = spi_write(jbt->spi_dev, (u8 *)jbt->tx_buf,
	               3*sizeof(u16));
	if (rc == 0)
		jbt->reg_cache[reg] = data;
	else
		dev_err(&jbt->spi_dev->dev, "jbt_reg_write16 spi_write ret %d\n", rc);

	return rc;
}

static int jbt_init_regs(struct jbt_info *jbt)
{
	int rc;

	dev_dbg(&jbt->spi_dev->dev, "entering %cVGA mode\n",
			jbt->resolution == JBT_RESOLUTION_QVGA ? 'Q' : ' ');

	rc = jbt_reg_write(jbt, JBT_REG_DISPLAY_MODE1, 0x01);
	rc |= jbt_reg_write(jbt, JBT_REG_DISPLAY_MODE2, 0x00);
	rc |= jbt_reg_write(jbt, JBT_REG_RGB_FORMAT, 0x60);
	rc |= jbt_reg_write(jbt, JBT_REG_DRIVE_SYSTEM, 0x10);
	rc |= jbt_reg_write(jbt, JBT_REG_BOOSTER_OP, 0x56);
	rc |= jbt_reg_write(jbt, JBT_REG_BOOSTER_MODE, 0x33);
	rc |= jbt_reg_write(jbt, JBT_REG_BOOSTER_FREQ, 0x11);
	rc |= jbt_reg_write(jbt, JBT_REG_OPAMP_SYSCLK, 0x02);
	rc |= jbt_reg_write(jbt, JBT_REG_VSC_VOLTAGE, 0x2b);
	rc |= jbt_reg_write(jbt, JBT_REG_VCOM_VOLTAGE, 0x40);
	rc |= jbt_reg_write(jbt, JBT_REG_EXT_DISPL, 0x03);
	rc |= jbt_reg_write(jbt, JBT_REG_DCCLK_DCEV, 0x04);
	/*
	 * default of 0x02 in JBT_REG_ASW_SLEW responsible for 72Hz requirement
	 * to avoid red / blue flicker
	 */
	rc |= jbt_reg_write(jbt, JBT_REG_ASW_SLEW, 0x04 | (1 << 5));
	rc |= jbt_reg_write(jbt, JBT_REG_DUMMY_DISPLAY, 0x00);

	rc |= jbt_reg_write(jbt, JBT_REG_SLEEP_OUT_FR_A, 0x11);
	rc |= jbt_reg_write(jbt, JBT_REG_SLEEP_OUT_FR_B, 0x11);
	rc |= jbt_reg_write(jbt, JBT_REG_SLEEP_OUT_FR_C, 0x11);
	rc |= jbt_reg_write16(jbt, JBT_REG_SLEEP_IN_LCCNT_D, 0x2040);
	rc |= jbt_reg_write16(jbt, JBT_REG_SLEEP_IN_LCCNT_E, 0x60c0);
	rc |= jbt_reg_write16(jbt, JBT_REG_SLEEP_IN_LCCNT_F, 0x1020);
	rc |= jbt_reg_write16(jbt, JBT_REG_SLEEP_IN_LCCNT_G, 0x60c0);

	rc |= jbt_reg_write16(jbt, JBT_REG_GAMMA1_FINE_1, 0x5533);
	rc |= jbt_reg_write(jbt, JBT_REG_GAMMA1_FINE_2, 0x00);
	rc |= jbt_reg_write(jbt, JBT_REG_GAMMA1_INCLINATION, 0x00);
	rc |= jbt_reg_write(jbt, JBT_REG_GAMMA1_BLUE_OFFSET, 0x00);

	if (jbt->resolution != JBT_RESOLUTION_QVGA) {
		rc |= jbt_reg_write16(jbt, JBT_REG_HCLOCK_VGA, 0x1f0);
		rc |= jbt_reg_write(jbt, JBT_REG_BLANK_CONTROL, 0x02);
		rc |= jbt_reg_write16(jbt, JBT_REG_BLANK_TH_TV, 0x0804);

		rc |= jbt_reg_write(jbt, JBT_REG_CKV_ON_OFF, 0x01);
		rc |= jbt_reg_write16(jbt, JBT_REG_CKV_1_2, 0x0000);

		rc |= jbt_reg_write16(jbt, JBT_REG_OEV_TIMING, 0x0d0e);
		rc |= jbt_reg_write16(jbt, JBT_REG_ASW_TIMING_1, 0x11a4);
		rc |= jbt_reg_write(jbt, JBT_REG_ASW_TIMING_2, 0x0e);
	} else {
		rc |= jbt_reg_write16(jbt, JBT_REG_HCLOCK_QVGA, 0x00ff);
		rc |= jbt_reg_write(jbt, JBT_REG_BLANK_CONTROL_QVGA, 0x02);
		rc |= jbt_reg_write16(jbt, JBT_REG_BLANK_TH_TV_QVGA, 0x0804);

		rc |= jbt_reg_write(jbt, JBT_REG_CKV_ON_OFF_QVGA, 0x01);
		rc |= jbt_reg_write16(jbt, JBT_REG_CKV_1_2_QVGA, 0x0008);

		rc |= jbt_reg_write16(jbt, JBT_REG_OEV_TIMING_QVGA, 0x050a);
		rc |= jbt_reg_write16(jbt, JBT_REG_ASW_TIMING_1_QVGA, 0x0a19);
		rc |= jbt_reg_write(jbt, JBT_REG_ASW_TIMING_2_QVGA, 0x0a);
	}

	return rc ? -EIO : 0;
}

static int standby_to_sleep(struct jbt_info *jbt)
{
	int rc;

	/* three times command zero */
	rc = jbt_reg_write_nodata(jbt, 0x00);
	mdelay(1);
	rc |= jbt_reg_write_nodata(jbt, 0x00);
	mdelay(1);
	rc |= jbt_reg_write_nodata(jbt, 0x00);
	mdelay(1);

	/* deep standby out */
	rc |= jbt_reg_write(jbt, JBT_REG_POWER_ON_OFF, 0x11);
	mdelay(1);
	rc = jbt_reg_write(jbt, JBT_REG_DISPLAY_MODE, 0x28);

	/* (re)initialize register set */
	rc |= jbt_init_regs(jbt);

	return rc ? -EIO : 0;
}

static int sleep_to_normal(struct jbt_info *jbt)
{
	int rc;

	/* Make sure we are 120 ms after SLEEP_OUT */
	if (time_before(jiffies, jbt->last_sleep))
		mdelay(jiffies_to_msecs(jbt->last_sleep - jiffies));

	if (jbt->resolution == JBT_RESOLUTION_VGA) {
		/* RGB I/F on, RAM wirte off, QVGA through, SIGCON enable */
		rc = jbt_reg_write(jbt, JBT_REG_DISPLAY_MODE, 0x80);

		/* Quad mode off */
		rc |= jbt_reg_write(jbt, JBT_REG_QUAD_RATE, 0x00);
	} else {
		/* RGB I/F on, RAM wirte off, QVGA through, SIGCON enable */
		rc = jbt_reg_write(jbt, JBT_REG_DISPLAY_MODE, 0x81);

		/* Quad mode on */
		rc |= jbt_reg_write(jbt, JBT_REG_QUAD_RATE, 0x22);
	}

	/* AVDD on, XVDD on */
	rc |= jbt_reg_write(jbt, JBT_REG_POWER_ON_OFF, 0x16);

	/* Output control */
	rc |= jbt_reg_write16(jbt, JBT_REG_OUTPUT_CONTROL, 0xfff9);

	/* Turn on display */
	rc |= jbt_reg_write_nodata(jbt, JBT_REG_DISPLAY_ON);

	/* Sleep mode off */
	rc |= jbt_reg_write_nodata(jbt, JBT_REG_SLEEP_OUT);
	jbt->last_sleep = jiffies + msecs_to_jiffies(120);

	/* Allow the booster and display controller to restart stably */
	mdelay(5);

	return rc ? -EIO : 0;
}

static int normal_to_sleep(struct jbt_info *jbt)
{
	int rc;

	/* Make sure we are 120 ms after SLEEP_OUT */
	while (time_before(jiffies, jbt->last_sleep))
		cpu_relax();

	rc = jbt_reg_write_nodata(jbt, JBT_REG_DISPLAY_OFF);
	rc |= jbt_reg_write16(jbt, JBT_REG_OUTPUT_CONTROL, 0x8000 | 1 << 3);
	rc |= jbt_reg_write_nodata(jbt, JBT_REG_SLEEP_IN);
	jbt->last_sleep = jiffies + msecs_to_jiffies(120);

	/* Allow the internal circuits to stop automatically */
	mdelay(5);

	return rc ? -EIO : 0;
}

static int sleep_to_standby(struct jbt_info *jbt)
{
	return jbt_reg_write(jbt, JBT_REG_POWER_ON_OFF, 0x00);
}

int jbt6k74_enter_power_mode(struct jbt_info *jbt, enum jbt_power_mode new_mode)
{
	struct jbt6k74_platform_data *pdata = jbt->spi_dev->dev.platform_data;
	int rc = -EINVAL;

	dev_dbg(&jbt->spi_dev->dev, "entering (old_state=%s, new_state=%s)\n",
			jbt_power_mode_names[jbt->power_mode],
			jbt_power_mode_names[new_mode]);

	mutex_lock(&jbt->lock);

	if (jbt->suspended) {
		switch (new_mode) {
		case JBT_POWER_MODE_DEEP_STANDBY:
		case JBT_POWER_MODE_SLEEP:
		case JBT_POWER_MODE_NORMAL:
			rc = 0;
			jbt->suspend_mode = new_mode;
			break;
		default:
			break;
		}
	} else if (new_mode == JBT_POWER_MODE_NORMAL &&
	           pdata->enable_pixel_clock) {
		pdata->enable_pixel_clock(&jbt->spi_dev->dev, 1);
	}

	switch (jbt->power_mode) {
	case JBT_POWER_MODE_DEEP_STANDBY:
		switch (new_mode) {
		case JBT_POWER_MODE_DEEP_STANDBY:
			rc = 0;
			break;
		case JBT_POWER_MODE_SLEEP:
			rc = standby_to_sleep(jbt);
			break;
		case JBT_POWER_MODE_NORMAL:
			/* first transition into sleep */
			rc = standby_to_sleep(jbt);
			/* then transition into normal */
			rc |= sleep_to_normal(jbt);
			break;
		}
		break;
	case JBT_POWER_MODE_SLEEP:
		switch (new_mode) {
		case JBT_POWER_MODE_SLEEP:
			rc = 0;
			break;
		case JBT_POWER_MODE_DEEP_STANDBY:
			rc = sleep_to_standby(jbt);
			break;
		case JBT_POWER_MODE_NORMAL:
			rc = sleep_to_normal(jbt);
			break;
		}
		break;
	case JBT_POWER_MODE_NORMAL:
		switch (new_mode) {
		case JBT_POWER_MODE_NORMAL:
			rc = 0;
			break;
		case JBT_POWER_MODE_DEEP_STANDBY:
			/* first transition into sleep */
			rc = normal_to_sleep(jbt);
			/* then transition into deep standby */
			rc |= sleep_to_standby(jbt);
			break;
		case JBT_POWER_MODE_SLEEP:
			rc = normal_to_sleep(jbt);
			break;
		}
	}

	if (rc == 0) {
		jbt->power_mode = new_mode;
		if (new_mode != JBT_POWER_MODE_NORMAL &&
		    pdata->enable_pixel_clock)
			pdata->enable_pixel_clock(&jbt->spi_dev->dev, 0);
	} else {
		dev_err(&jbt->spi_dev->dev, "Failed enter state '%s')\n",
				jbt_power_mode_names[new_mode]);
	}

	mutex_unlock(&jbt->lock);

	return rc;
}
EXPORT_SYMBOL_GPL(jbt6k74_enter_power_mode);

int jbt6k74_set_resolution(struct jbt_info *jbt, enum jbt_resolution new_resolution) {
	int rc = 0;
	enum jbt_resolution old_resolution;

	if (new_resolution != JBT_RESOLUTION_VGA &&
	    new_resolution != JBT_RESOLUTION_QVGA)
		return -EINVAL;

	mutex_lock(&jbt->lock);

	if (jbt->resolution == new_resolution)
		goto out_unlock;

	old_resolution = jbt->resolution;
	jbt->resolution = new_resolution;

	if (jbt->power_mode == JBT_POWER_MODE_NORMAL) {

		/* first transition into sleep */
		rc = normal_to_sleep(jbt);
		/* second transition into deep standby */
/*		rc |= sleep_to_standby(jbt);*/
		/* third transition into sleep */
/*		rc |= standby_to_sleep(jbt);*/
		/* fourth transition into normal */
		rc |= sleep_to_normal(jbt);

		if (rc) {
			jbt->resolution = old_resolution;
			dev_err(&jbt->spi_dev->dev, "Failed to set resolution '%s')\n",
			        jbt_resolution_names[new_resolution]);
		}
	}

out_unlock:
	mutex_unlock(&jbt->lock);

	return rc;
}
EXPORT_SYMBOL_GPL(jbt6k74_set_resolution);

static ssize_t resolution_read(struct device *dev, struct device_attribute *attr,
			  char *buf)
{
	struct jbt_info *jbt = dev_get_drvdata(dev);

	if (jbt->resolution >= ARRAY_SIZE(jbt_resolution_names))
		return -EIO;

	return sprintf(buf, "%s\n", jbt_resolution_names[jbt->resolution]);
}

static ssize_t resolution_write(struct device *dev, struct device_attribute *attr,
			   const char *buf, size_t count)
{
	struct jbt_info *jbt = dev_get_drvdata(dev);
	int i, rc;

	for (i = 0; i < ARRAY_SIZE(jbt_resolution_names); i++) {
		if (!strncmp(buf, jbt_resolution_names[i],
		       strlen(jbt_resolution_names[i]))) {
			rc = jbt6k74_set_resolution(jbt, i);
			if (rc)
				return rc;
			return count;
		}
	}

	return -EINVAL;
}

static DEVICE_ATTR(resolution, 0644, resolution_read, resolution_write);

static int reg_by_string(const char *name)
{
	if (!strcmp(name, "gamma_fine1"))
		return JBT_REG_GAMMA1_FINE_1;
	else if (!strcmp(name, "gamma_fine2"))
		return JBT_REG_GAMMA1_FINE_2;
	else if (!strcmp(name, "gamma_inclination"))
		return JBT_REG_GAMMA1_INCLINATION;
	else
		return JBT_REG_GAMMA1_BLUE_OFFSET;
}

static ssize_t gamma_read(struct device *dev, struct device_attribute *attr,
			  char *buf)
{
	struct jbt_info *jbt = dev_get_drvdata(dev);
	int reg = reg_by_string(attr->attr.name);
	u16 val;

	mutex_lock(&jbt->lock);
	val = jbt->reg_cache[reg];
		mutex_unlock(&jbt->lock);

	return sprintf(buf, "0x%04x\n", val);
}

static ssize_t gamma_write(struct device *dev, struct device_attribute *attr,
			   const char *buf, size_t count)
{
	struct jbt_info *jbt = dev_get_drvdata(dev);
	int reg = reg_by_string(attr->attr.name);
	unsigned long val = simple_strtoul(buf, NULL, 10);

	dev_info(dev, "writing gama %lu\n", val & 0xff);

	mutex_lock(&jbt->lock);
	jbt_reg_write(jbt, reg, val & 0xff);
	mutex_unlock(&jbt->lock);

	return count;
}

static ssize_t reset_write(struct device *dev, struct device_attribute *attr,
			   const char *buf, size_t count)
{
	int rc;
	struct jbt_info *jbt = dev_get_drvdata(dev);
	struct jbt6k74_platform_data *pdata = jbt->spi_dev->dev.platform_data;

	dev_info(dev, "reset\n");

	mutex_lock(&jbt->lock);

	/* hard reset the jbt6k74 */
	(pdata->reset)(0, 0);
	mdelay(1);
	(pdata->reset)(0, 1);
	mdelay(120);

	rc = jbt_reg_write_nodata(jbt, 0x01);
	if (rc < 0)
		dev_err(&jbt->spi_dev->dev, "cannot soft reset\n");
	mdelay(120);

	mutex_unlock(&jbt->lock);

	jbt6k74_enter_power_mode(jbt, jbt->power_mode);

	return count;
}

static DEVICE_ATTR(gamma_fine1, 0644, gamma_read, gamma_write);
static DEVICE_ATTR(gamma_fine2, 0644, gamma_read, gamma_write);
static DEVICE_ATTR(gamma_inclination, 0644, gamma_read, gamma_write);
static DEVICE_ATTR(gamma_blue_offset, 0644, gamma_read, gamma_write);
static DEVICE_ATTR(reset, 0600, NULL, reset_write);

static struct attribute *jbt_sysfs_entries[] = {
	&dev_attr_resolution.attr,
	&dev_attr_gamma_fine1.attr,
	&dev_attr_gamma_fine2.attr,
	&dev_attr_gamma_inclination.attr,
	&dev_attr_gamma_blue_offset.attr,
	&dev_attr_reset.attr,
	NULL,
};

static struct attribute_group jbt_attr_group = {
	.name	= NULL,
	.attrs	= jbt_sysfs_entries,
};

/* FIXME: This in an ugly hack to delay display blanking.
  When the jbt is in sleep mode it displays an all white screen and thus one
  will a see a short flash.
  By delaying the blanking we will give the backlight a chance to turn off and
  thus avoid getting the flash */
static void jbt_blank_worker(struct work_struct *work) {
	struct jbt_info *jbt  = container_of(work, struct jbt_info,
						blank_work.work);

	switch (jbt->blank_mode) {
	case FB_BLANK_NORMAL:
		jbt6k74_enter_power_mode(jbt, JBT_POWER_MODE_SLEEP);
		break;
	case FB_BLANK_POWERDOWN:
		jbt6k74_enter_power_mode(jbt, JBT_POWER_MODE_DEEP_STANDBY);
		break;
	default:
		break;
	}
}

static int jbt6k74_set_mode(struct lcd_device *ld, struct fb_videomode *m) {
	int rc = -EINVAL;
	struct jbt_info *jbt = dev_get_drvdata(&ld->dev);

	if (m->xres == 240 && m->yres == 320) {
		rc = jbt6k74_set_resolution(jbt, JBT_RESOLUTION_QVGA);
	} else if (m->xres == 480 && m->yres == 640) {
		rc = jbt6k74_set_resolution(jbt, JBT_RESOLUTION_VGA);
	} else {
		dev_err(&jbt->spi_dev->dev, "Unknown resolution. Entering sleep mode.\n");
		jbt6k74_enter_power_mode(jbt, JBT_POWER_MODE_SLEEP);
	}

	return rc;
}

static int jbt6k74_set_power(struct lcd_device *ld, int power) {
	int rc = -EINVAL;
	struct jbt_info *jbt = dev_get_drvdata(&ld->dev);

	jbt->blank_mode = power;
	cancel_rearming_delayed_work(&jbt->blank_work);

	switch (power) {
	case FB_BLANK_UNBLANK:
		dev_dbg(&jbt->spi_dev->dev, "unblank\n");
		rc = jbt6k74_enter_power_mode(jbt, JBT_POWER_MODE_NORMAL);
		break;
	case FB_BLANK_NORMAL:
		dev_dbg(&jbt->spi_dev->dev, "blank\n");
		rc = schedule_delayed_work(&jbt->blank_work, HZ);
		break;
	case FB_BLANK_POWERDOWN:
		dev_dbg(&jbt->spi_dev->dev, "powerdown\n");
		rc = schedule_delayed_work(&jbt->blank_work, HZ);
		break;
	default:
		break;
	}

	return rc;
}

static int jbt6k74_get_power(struct lcd_device *ld) {
	struct jbt_info *jbt = dev_get_drvdata(&ld->dev);

	switch (jbt->power_mode) {
        case JBT_POWER_MODE_NORMAL:
		return FB_BLANK_UNBLANK;
        case JBT_POWER_MODE_SLEEP:
		return FB_BLANK_NORMAL;
        default:
		return JBT_POWER_MODE_DEEP_STANDBY;
	}
}

struct lcd_ops jbt6k74_lcd_ops = {
	.set_power = jbt6k74_set_power,
	.get_power = jbt6k74_get_power,
	.set_mode  = jbt6k74_set_mode,
};

/* linux device model infrastructure */

static int __devinit jbt_probe(struct spi_device *spi)
{
	int rc;
	struct jbt_info *jbt;
	struct jbt6k74_platform_data *pdata = spi->dev.platform_data;

	/* the controller doesn't have a MISO pin; we can't do detection */

	spi->mode = SPI_CPOL | SPI_CPHA;
	spi->bits_per_word = 9;

	rc = spi_setup(spi);
	if (rc < 0) {
		dev_err(&spi->dev,
			"error during spi_setup of jbt6k74 driver\n");
		return rc;
	}

	jbt = kzalloc(sizeof(*jbt), GFP_KERNEL);
	if (!jbt)
		return -ENOMEM;

	jbt->spi_dev = spi;

	jbt->lcd_dev = lcd_device_register("jbt6k74-lcd", &spi->dev,
	                   jbt, &jbt6k74_lcd_ops);

	if (IS_ERR(jbt->lcd_dev)) {
		rc = PTR_ERR(jbt->lcd_dev);
		goto err_free_drvdata;
	}

	INIT_DELAYED_WORK(&jbt->blank_work, jbt_blank_worker);

	jbt->resolution = JBT_RESOLUTION_VGA;
	jbt->power_mode = JBT_POWER_MODE_DEEP_STANDBY;
	jbt->last_sleep = jiffies + msecs_to_jiffies(120);
	mutex_init(&jbt->lock);

	dev_set_drvdata(&spi->dev, jbt);

	rc = jbt6k74_enter_power_mode(jbt, JBT_POWER_MODE_NORMAL);
	if (rc < 0) {
		dev_err(&spi->dev, "cannot enter NORMAL state\n");
		goto err_unregister_lcd;
	}

	rc = sysfs_create_group(&spi->dev.kobj, &jbt_attr_group);
	if (rc < 0) {
		dev_err(&spi->dev, "cannot create sysfs group\n");
		goto err_standby;
	}

	if (pdata->probe_completed)
		(pdata->probe_completed)(&spi->dev);

	return 0;

err_standby:
	jbt6k74_enter_power_mode(jbt, JBT_POWER_MODE_DEEP_STANDBY);
err_unregister_lcd:
	lcd_device_unregister(jbt->lcd_dev);
err_free_drvdata:
	dev_set_drvdata(&spi->dev, NULL);
	kfree(jbt);

	return rc;
}

static int __devexit jbt_remove(struct spi_device *spi)
{
	struct jbt_info *jbt = dev_get_drvdata(&spi->dev);

	/* We don't want to switch off the display in case the user
	 * accidentially unloads the module (whose use count normally is 0) */
	jbt6k74_enter_power_mode(jbt, JBT_POWER_MODE_NORMAL);

	sysfs_remove_group(&spi->dev.kobj, &jbt_attr_group);
	dev_set_drvdata(&spi->dev, NULL);

	lcd_device_unregister(jbt->lcd_dev);

	kfree(jbt);

	return 0;
}

#ifdef CONFIG_PM
static int jbt_suspend(struct spi_device *spi, pm_message_t state)
{
	struct jbt_info *jbt = dev_get_drvdata(&spi->dev);

	jbt->suspend_mode = jbt->power_mode;

	jbt6k74_enter_power_mode(jbt, JBT_POWER_MODE_DEEP_STANDBY);
	jbt->suspended = 1;

	dev_info(&spi->dev, "suspended\n");

	return 0;
}

int jbt6k74_resume(struct spi_device *spi)
{
	struct jbt_info *jbt = dev_get_drvdata(&spi->dev);

	jbt->suspended = 0;
	jbt6k74_enter_power_mode(jbt, jbt->suspend_mode);

	dev_info(&spi->dev, "resumed\n");

	return 0;
}
EXPORT_SYMBOL_GPL(jbt6k74_resume);

#else
#define jbt_suspend	NULL
#define jbt6k74_resume	NULL
#endif

static struct spi_driver jbt6k74_driver = {
	.driver = {
		.name	= "jbt6k74",
		.owner	= THIS_MODULE,
	},

	.probe	 = jbt_probe,
	.remove	 = __devexit_p(jbt_remove),
	.suspend = jbt_suspend,
	.resume	 = jbt6k74_resume,
};

static int __init jbt_init(void)
{
	return spi_register_driver(&jbt6k74_driver);
}

static void __exit jbt_exit(void)
{
	spi_unregister_driver(&jbt6k74_driver);
}

MODULE_DESCRIPTION("SPI driver for tpo JBT6K74-AS LCM control interface");
MODULE_AUTHOR("Harald Welte <laforge@openmoko.org>");
MODULE_LICENSE("GPL");

module_init(jbt_init);
module_exit(jbt_exit);

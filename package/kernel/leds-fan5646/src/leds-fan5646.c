/*
 * Copyright (C) 2008-2013 Motorola, Inc.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA.
 */

/*
 * Fairchild FAN5646 blinking LED Driver for Android
 *
 * Alina Yakovleva qvdh43@motorola.com
 */

#include <linux/kernel.h>
#include <linux/hrtimer.h>
#include <linux/ctype.h>
#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/leds.h>
#include <linux/time.h>
#include <linux/err.h>
#include <linux/unistd.h>
#include <linux/sysfs.h>
#include <linux/delay.h>
#include <linux/slab.h>
#include <linux/jiffies.h>
#include <linux/spinlock.h>
#include <linux/of_platform.h>
#include <linux/of_device.h>
#include <linux/of_gpio.h>
#include <linux/module.h>
#include <linux/regulator/consumer.h>
#include <linux/gpio.h>
#ifdef CONFIG_ANDROID_POWER
#include <linux/android_power.h>
#endif

#define FAN5646_SLEW1_REG   0
#define FAN5646_PULSE1_REG  0x1
#define FAN5646_SLEW2_REG   0x2
#define FAN5646_PULSE2_REG  0x3
#define FAN5646_CONTROL_REG 0x4

#define FAN5646_ISET_5mA  0
#define FAN5646_ISET_10mA 0x40
#define FAN5646_ISET_15mA 0x80
#define FAN5646_ISET_20mA 0xC0

#define FAN5646_FOLLOW 0x1
#define FAN5646_PLAY   0x2
#define FAN5646_SLOW   0x4

#define FAN5646_MAX_TON        1600
#define FAN5646_MAX_TOFF       4800
#define FAN5646_MAX_TRISE      1550
#define FAN5646_MAX_TFALL      1550
#define FAN5646_MAX_ON         4700	/* tRise + tFall + tOn */
#define FAN5646_MAX_ON_SLOW    6300	/* tRise + tFall + 2 * tOn */
#define FAN5646_MAX_OFF        FAN5646_MAX_TOFF
#define FAN5646_MAX_OFF_SLOW   9600

#define FAN5646_VREG_DELAY   600 /* Fixed regulator ramp up time */

#define TRESET 110		/* Sleep time in us */

#define DEFAULT_UP   45
#define DEFAULT_DOWN 45

/** Struct fan5646
* led - led device for the connected LED
* rgb - led device for combined brightness/blink control
* dev - device structure pointer
* lock - spinlock
* full_current - full scale current
* tsleep - bit duration in us.
* com_gpio - gpio used for IC control
*/

struct fan5646_data {
	struct led_classdev led;
	struct led_classdev rgb;
	struct device *dev;
	spinlock_t lock;
	struct mutex mlock;
	struct regulator *vreg;
	const char *regname;
	unsigned full_current;
	unsigned tsleep;
	int com_gpio;
	unsigned delay_on;
	unsigned delay_off;
	unsigned ramp_up;
	unsigned ramp_down;
	unsigned level;
	int power;
};

static void fan5646_brightness_set(struct led_classdev *led_cdev,
				   enum led_brightness bvalue);
static void fan5646_set_pulse(unsigned msOn, unsigned msOff,
			      unsigned ramp_up, unsigned ramp_down, __u8 *slew,
			      __u8 *pulse);
static int fan5646_blink_set(struct led_classdev *led_cdev,
			     unsigned long *delay_on, unsigned long *delay_off);

static inline void tinywire_send_bit(int gpio, __u8 bit, unsigned tsleep)
{
	if (bit == 0) {
		gpio_set_value(gpio, 1);
		udelay(tsleep);
		gpio_set_value(gpio, 0);
		udelay(tsleep * 4);
	} else {
		gpio_set_value(gpio, 1);
		udelay(tsleep * 4);
		gpio_set_value(gpio, 0);
		udelay(tsleep);
	}
}

static inline void tinywire_send_reset(int gpio)
{
	gpio_set_value(gpio, 0);
	udelay(TRESET);
}

static inline void tinywire_send_exec(int gpio)
{
	gpio_set_value(gpio, 1);
	udelay(TRESET);
}

static inline void fan5646_power_on(struct fan5646_data *cb)
{
	int rc = 0;

	mutex_lock(&cb->mlock);
	if (!cb->power) {
		rc = regulator_enable(cb->vreg);
		if (rc) {
			pr_err("%s reg. enable error %d state %d\n",
				__func__, rc, regulator_is_enabled(cb->vreg));
			mutex_unlock(&cb->mlock);
			return;
		}
		udelay(FAN5646_VREG_DELAY);
		//usleep(FAN5646_VREG_DELAY);
		//nanosleep((struct timespec[]){{0, FAN5646_VREG_DELAY}},NULL);
		cb->power = 1;
		pr_debug("%s reg. enabled state %d\n",
				__func__, regulator_is_enabled(cb->vreg));
	} else
		pr_debug("%s regulator is already enabled state %d\n",
			__func__, regulator_is_enabled(cb->vreg));
	mutex_unlock(&cb->mlock);
}

static inline void fan5646_power_off(struct fan5646_data *cb)
{
	int rc = 0;

	tinywire_send_reset(cb->com_gpio);
	mutex_lock(&cb->mlock);
	if (cb->power) {
		rc = regulator_disable(cb->vreg);
		if (rc) {
			pr_err("%s reg. disable error %d state %d\n",
				__func__, rc, regulator_is_enabled(cb->vreg));
			mutex_unlock(&cb->mlock);
			return;
		}
		cb->power = 0;
		pr_debug("%s reg. disabled state %d\n",
				__func__, regulator_is_enabled(cb->vreg));
	} else
		pr_debug("%s regulator is already disabled state %d\n",
			__func__, regulator_is_enabled(cb->vreg));
	mutex_unlock(&cb->mlock);
}

static void tinywire_set_reg(int gpio, __u8 reg, __u8 value, unsigned tsleep)
{
	int i;
	__u8 mask = 0x1;

	pr_debug("%s: reg=0x%x, value=0x%0x, tsleep=%dus\n",
		 __func__, reg, value, tsleep);
	/* Register address is 3 bits.  Send it LSB first */
	for (i = 0; i < 3; i++)
		tinywire_send_bit(gpio, reg & (mask << i), tsleep);
	/* Now send data LSB first */
	for (i = 0; i < 8; i++)
		tinywire_send_bit(gpio, value & (mask << i), tsleep);
	/* Send STOP bit */
	tinywire_send_bit(gpio, 0, tsleep);
	/* Wait for TRESET so that it stays IDLE */
	udelay(TRESET);
}

static void
fan5646_brightness_set(struct led_classdev *led_cdev, enum led_brightness value)
{
	__u8 ctrl_value;
	unsigned long flags;
	struct fan5646_data *pdata = dev_get_drvdata(led_cdev->dev->parent);

	pr_debug("%s: %d\n", __func__, value);

	if (value)
		fan5646_power_on(pdata);
	else {
		fan5646_power_off(pdata);
		return;
	}

	spin_lock_irqsave(&pdata->lock, flags);

	tinywire_send_reset(pdata->com_gpio);
	if (value) {
		/* Set default current and follow bit and raise control */
		ctrl_value = pdata->full_current | FAN5646_FOLLOW;
		tinywire_set_reg(pdata->com_gpio, FAN5646_CONTROL_REG,
				   ctrl_value, pdata->tsleep);
		/* Clear second pulse or it will keep blinking */
		tinywire_set_reg(pdata->com_gpio, FAN5646_PULSE1_REG, 0,
				   pdata->tsleep);
		tinywire_send_exec(pdata->com_gpio);
	}

	spin_unlock_irqrestore(&pdata->lock, flags);
}

static int fan5646_blink_set(struct led_classdev *led_cdev,
			     unsigned long *delay_on, unsigned long *delay_off)
{
	struct fan5646_data *pdata = dev_get_drvdata(led_cdev->dev->parent);
	__u8 ctrl_value = pdata->full_current | FAN5646_PLAY;
	__u8 slew, pulse;
	unsigned long flags;

	pr_debug("%s: delay_on = %lu, delay_off = %lu\n",
		 __func__, *delay_on, *delay_off);
	if (*delay_on == 0 && *delay_off == 0) {
		*delay_on = 500;
		*delay_off = 500;
	}
	fan5646_power_on(pdata);
	spin_lock_irqsave(&pdata->lock, flags);
	tinywire_send_reset(pdata->com_gpio);
	tinywire_set_reg(pdata->com_gpio, FAN5646_CONTROL_REG, ctrl_value,
			   pdata->tsleep);
	fan5646_set_pulse(*delay_on, *delay_off, DEFAULT_UP, DEFAULT_DOWN,
			  &slew, &pulse);
	tinywire_set_reg(pdata->com_gpio, FAN5646_SLEW1_REG, slew,
			   pdata->tsleep);
	tinywire_set_reg(pdata->com_gpio, FAN5646_PULSE1_REG, pulse,
			   pdata->tsleep);
	tinywire_set_reg(pdata->com_gpio, FAN5646_SLEW2_REG, 0, pdata->tsleep);
	tinywire_set_reg(pdata->com_gpio, FAN5646_PULSE2_REG, 0, pdata->tsleep);
	tinywire_send_exec(pdata->com_gpio);
	spin_unlock_irqrestore(&pdata->lock, flags);
	return 0;
}

static void
fan5646_set_pulse(unsigned msOn, unsigned msOff,
		  unsigned ramp_up, unsigned ramp_down, __u8 *slew,
		  __u8 *pulse)
{
	__u8 nRise, nFall, nOn, nOff;
	unsigned tRise, tFall, tOn, tOff;
	unsigned slow = 0;

	pr_debug("%s: msOn = %d, msOff = %d, ramp up = %d%%, down = %d%%\n",
		 __func__, msOn, msOff, ramp_up, ramp_down);
	*slew = 0;
	*pulse = 0;

	if (msOn == 0 && msOff == 0)
		return;
	/* We won't do slow for now */
	if (msOn > FAN5646_MAX_ON)
		msOn = FAN5646_MAX_ON;
	if (msOff > FAN5646_MAX_OFF)
		msOff = FAN5646_MAX_OFF;
	tOff = msOff;
	/* Now the blinking part
	 * msOn consists of 3 parts: tRise, tFall, and tOn.
	 */
	if (ramp_up + ramp_down > 100) {
		pr_err("%s: bad ramp up %d%%, ramp down %d%%; resetting\n",
		       __func__, ramp_up, ramp_down);
		ramp_up = DEFAULT_UP;
		ramp_down = DEFAULT_DOWN;
	}
	tOn = (100 - ramp_up - ramp_down) * msOn / 100;
	tRise = ramp_up * msOn / 100;
	tFall = ramp_down * msOn / 100;
	if (tRise > FAN5646_MAX_TRISE) {
		tOn += tRise - FAN5646_MAX_TRISE;
		tRise = FAN5646_MAX_TRISE;
	}
	if (tFall > FAN5646_MAX_TRISE) {
		tOn += tFall - FAN5646_MAX_TRISE;
		tFall = FAN5646_MAX_TRISE;
	}
	/* Now we need to calculate nRise, nFall, nOn and nOff
	   tRise = 31 * nRise * 3.33 ms, same for tFall
	   nRise = tRise / 103.23 */
	nRise = tRise * 100 / 10323;
	if (nRise > 0xF)
		nRise = 0xF;
	if (nRise == 0 && ramp_up != 0)
		nRise = 1;
	nFall = tFall * 100 / 10323;
	if (nFall > 0xF)
		nFall = 0xF;
	if (nFall == 0 && ramp_down != 0)
		nFall = 1;

	*slew = nRise << 4 | nFall;

	/* Now tOn and tOff
	 * tOn = (SLOW + 1) * nOn * 106.6
	 * tOff = (SLOW + 1) * nOff * 320
	 * nOn = tOn / ((SLOW + 1) * 106.6)
	 * nOff = tOff / ((SLOW + 1) * 320)
	 */
	nOn = tOn * 10 / ((slow + 1) * 1066);
	nOff = tOff / ((slow + 1) * 320);
	if (nOn > 0xF)
		nOn = 0xF;
	if (nOff > 0xF)
		nOff = 0xF;
	if (nOn == 0 && (ramp_up + ramp_down < 100))
		nOn = 1;
	if (nOff == 0 && msOff != 0)
		nOff = 1;
	*pulse = nOn << 4 | nOff;

	pr_debug("%s: tRise = %d, tFall = %d, tOn = %d, tOff = %d, slow = %d\n",
		 __func__, tRise, tFall, tOn, tOff, slow);
	pr_debug("%s: nRise = 0x%x, nFall = 0x%x, nOn = 0x%x, nOff = 0x%x\n",
		 __func__, nRise, nFall, nOn, nOff);
}

static ssize_t
fan5646_blink_show(struct device *dev, struct device_attribute *attr, char *buf)
{
	return 0;
}

static ssize_t
fan5646_blink_store(struct device *dev,
		    struct device_attribute *attr, const char *buf, size_t size)
{
	unsigned msOn = 0, msOff = 0, bvalue = 0;
	unsigned msOn1 = 0, msOff1 = 0;
	int n;
	struct led_classdev *led_cdev;
	__u8 ctrl_value, slew, pulse, slew1 = 0, pulse1 = 0;
	int ramp_up = DEFAULT_UP;
	int ramp_down = DEFAULT_DOWN;
	int ramp_up1 = DEFAULT_UP;
	int ramp_down1 = DEFAULT_DOWN;
	char *ptr;
	unsigned long flags;
	struct fan5646_data *pdata = dev_get_drvdata(dev->parent);

	if (!buf || size == 0) {
		pr_err("%s: invalid command\n", __func__);
		return -EINVAL;
	}
	led_cdev = dev_get_drvdata(dev);

	/* The format is: "brightness msOn msOff" */
	n = sscanf(buf, "%d %d %d %d %d", &bvalue, &msOn, &msOff, &msOn1,
		&msOff1);

	if (n != 3 && n != 5) {
		pr_err("%s: inavlid command: %s\n", __func__, buf);
		return -EINVAL;
	}
	pr_debug("%s: %s, b=%d, msOn=%d, msOff=%d, msOn1=%d msOff1=%d\n",
		__func__, led_cdev->name, bvalue, msOn, msOff, msOn1, msOff1);

	if (bvalue == 0 || (bvalue != 0 && msOn == 0 && msOff == 0)) {
		fan5646_brightness_set(led_cdev, bvalue);
		return size;
	}

	/* Now see if ramp values are there */
	ptr = strnstr(buf, "ramp", size);
	if (ptr) {
		ptr = strpbrk(ptr, "0123456789");
		if (!ptr) {
			pr_err("%s: inavlid command (ramp): %s\n",
			       __func__, buf);
			return -EINVAL;
		}
		n = sscanf(ptr, "%d %d %d %d",
			   &ramp_up, &ramp_down, &ramp_up1, &ramp_down1);
		if (n < 2) {
			pr_err("%s: inavlid command (ramp): %s\n",
			       __func__, buf);
			return -EINVAL;
		}
		if (ramp_up < 0)
			ramp_up = DEFAULT_UP;
		if (ramp_down < 0)
			ramp_down = DEFAULT_DOWN;
		if (ramp_up1 < 0)
			ramp_up1 = DEFAULT_UP;
		if (ramp_down1 < 0)
			ramp_down1 = DEFAULT_DOWN;
		if (ramp_up + ramp_down > 100 || ramp_up1 + ramp_down1 > 100) {
			pr_err("%s bad ramp time up/down %d%%/%d%% %d%%/%d%%\n",
				__func__, ramp_up, ramp_down, ramp_up1,
			       ramp_down1);
			return -EINVAL;
		}
	}
	pr_debug("%s: %s up = %d%%, down = %d%%, up1 = %d%% down1 = %d%%\n",
		__func__, led_cdev->name,
		 ramp_up, ramp_down, ramp_up1, ramp_down1);

	fan5646_set_pulse(msOn, msOff, ramp_up, ramp_down, &slew, &pulse);
	fan5646_set_pulse(msOn1, msOff1, ramp_up1, ramp_down1, &slew1, &pulse1);
	ctrl_value = pdata->full_current | FAN5646_PLAY;

	fan5646_power_on(pdata);
	spin_lock_irqsave(&pdata->lock, flags);
	tinywire_send_reset(pdata->com_gpio);
	tinywire_set_reg(pdata->com_gpio, FAN5646_SLEW1_REG, slew,
			   pdata->tsleep);
	tinywire_set_reg(pdata->com_gpio, FAN5646_PULSE1_REG, pulse,
			   pdata->tsleep);
	tinywire_set_reg(pdata->com_gpio, FAN5646_SLEW2_REG, slew1,
			   pdata->tsleep);
	tinywire_set_reg(pdata->com_gpio, FAN5646_PULSE2_REG, pulse1,
			   pdata->tsleep);
	tinywire_set_reg(pdata->com_gpio, FAN5646_CONTROL_REG, ctrl_value,
			   pdata->tsleep);
	tinywire_send_exec(pdata->com_gpio);
	spin_unlock_irqrestore(&pdata->lock, flags);
	return size;
}

static DEVICE_ATTR(blink, 0644, fan5646_blink_show, fan5646_blink_store);

static ssize_t
fan5646_settings_show(struct device *dev,
		      struct device_attribute *attr, char *buf)
{
	struct fan5646_data *pdata = dev_get_drvdata(dev->parent);

	return scnprintf(buf, PAGE_SIZE, "current = %dmA, timing = %d\n",
		(pdata->full_current + 1) * 5, pdata->tsleep);
}

static ssize_t
fan5646_settings_store(struct device *dev,
		       struct device_attribute *attr,
		       const char *buf, size_t size)
{
	unsigned long value = 0;
	unsigned type = 0;
	char *ptr;
	int rc = 0;
	struct fan5646_data *pdata = dev_get_drvdata(dev->parent);

	if (!buf || size == 0) {
		pr_err("%s: invalid command\n", __func__);
		return -EINVAL;
	}

	if (strnstr(buf, "current", size)) {
		type = 1;
	} else if (strnstr(buf, "timing", size)) {
		type = 2;
	} else {
		pr_err("%s: inavlid command: %s\n", __func__, buf);
		return -EINVAL;
	}
	ptr = strpbrk(buf, "0123456789");
	if (ptr) {
		rc = kstrtoul(ptr, 10, &value);
		if (!rc) {
			pr_err("%s: inavlid parameter: %d\n", __func__, rc);
			return -EINVAL;
		}
	}
	else {
		pr_err("%s: inavlid command: %s\n", __func__, buf);
		return -EINVAL;
	}

	switch (type) {
	case 1:
		switch (value) {
		case 5:
			pdata->full_current = FAN5646_ISET_5mA;
			break;
		case 10:
			pdata->full_current = FAN5646_ISET_10mA;
			break;
		case 15:
			pdata->full_current = FAN5646_ISET_15mA;
			break;
		case 20:
			pdata->full_current = FAN5646_ISET_20mA;
			break;
		default:
			pr_err("%s: inavlid current value: %ld\n",
			       __func__, value);
			return -EINVAL;
		}
		pr_debug("%s: changing current to %ldmA\n", __func__, value);
		break;
	case 2:
		pr_debug("%s: changing timing to %ldus\n", __func__, value);
		pdata->tsleep = value;
		break;
	default:
		pr_err("%s: inavlid command: %s\n", __func__, buf);
		return -EINVAL;
	}

	return size;
}

static DEVICE_ATTR(settings, 0644, fan5646_settings_show,
		   fan5646_settings_store);

static ssize_t
fan5646_prot_show(struct device *dev, struct device_attribute *attr, char *buf)
{
	return 0;
}

static ssize_t
fan5646_prot_store(struct device *dev,
		   struct device_attribute *attr, const char *buf, size_t size)
{
	unsigned reg, value = 0;
	__u8 _reg, _value = 0;
	int n;
	unsigned long flags;
	struct fan5646_data *pdata = dev_get_drvdata(dev->parent);

	if (!buf || size == 0) {
		pr_err("%s: invalid command\n", __func__);
		return -EINVAL;
	}

	n = sscanf(buf, "%d %d", &reg, &value);
	_reg = (__u8) reg;
	_value = (__u8) value;

	if (n == 0) {
		pr_err("%s: inavlid command: %s\n", __func__, buf);
		return -EINVAL;
	}
	fan5646_power_on(pdata);
	if (n == 1) {
		pr_debug("%s: %s CTRL\n", __func__,
			 reg ? "raising" : "lowering");
		spin_lock_irqsave(&pdata->lock, flags);
		if (reg)
			tinywire_send_exec(pdata->com_gpio);
		else
			tinywire_send_reset(pdata->com_gpio);
		spin_unlock_irqrestore(&pdata->lock, flags);
	} else if (n == 2) {
		pr_debug("%s: reg=%d, value=%d\n", __func__, _reg, _value);
		spin_lock_irqsave(&pdata->lock, flags);
		tinywire_send_reset(pdata->com_gpio);
		tinywire_set_reg(pdata->com_gpio, _reg, _value, pdata->tsleep);
		spin_unlock_irqrestore(&pdata->lock, flags);
	}
	return size;
}

static DEVICE_ATTR(prot, 0644, fan5646_prot_show, fan5646_prot_store);

static ssize_t
fan5646_rgb_show_control(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	struct fan5646_data *pdata = dev_get_drvdata(dev->parent);

	return scnprintf(buf, PAGE_SIZE,
			 "RGB=%#x, on/off=%d/%d ms, ramp=%d/%d\n",
			 pdata->level, pdata->delay_on,
			 pdata->delay_off, pdata->ramp_up, pdata->ramp_down);
}

static ssize_t
fan5646_rgb_store_control(struct device *dev,
		  struct device_attribute *attr, const char *buf, size_t len)
{
	__u8 ctrl_value, slew, pulse;
	unsigned long flags;
	struct fan5646_data *pdata = dev_get_drvdata(dev->parent);

	if (len == 0)
		return 0;

	/* Clear pulse parameters in case of incomplete input */
	pdata->level = 0;
	pdata->delay_on = 0;
	pdata->delay_off = 0;
	pdata->ramp_up = 0;
	pdata->ramp_down = 0;

	sscanf(buf, "%x %u %u %u %u", &pdata->level, &pdata->delay_on,
	       &pdata->delay_off, &pdata->ramp_up, &pdata->ramp_down);
	pr_debug("0x%X, on/off=%u/%u ms, ramp=%u%%/%u%%\n", pdata->level,
		 pdata->delay_on, pdata->delay_off, pdata->ramp_up,
		 pdata->ramp_down);

	/* RGB control level has one byte for each color. We use only "R" */
	pdata->level = (pdata->level >> 16) & 0xFF;

	/* LED that is off is not blinking */
	if (pdata->level == 0) {
		pdata->delay_on = 0;
		pdata->delay_off = 0;
	}

	if (pdata->level == 0 || (pdata->level != 0
				  && pdata->delay_on == 0
				  && pdata->delay_off == 0)) {
		fan5646_brightness_set(&pdata->rgb, pdata->level);
		return len;
	}

	/* If delay_on is not 0 but delay_off is 0 we won't blink */
	if (pdata->delay_off == 0)
		pdata->delay_on = 0;

	if (pdata->ramp_up < 0)
		pdata->ramp_up = DEFAULT_UP;
	if (pdata->ramp_down < 0)
		pdata->ramp_down = DEFAULT_DOWN;
	if (pdata->ramp_up + pdata->ramp_down > 100) {
		pr_err("%s: ramp times >100%% %d%% and %d%% setting default\n",
		     __func__, pdata->ramp_up, pdata->ramp_down);
		pdata->ramp_up = DEFAULT_UP;
		pdata->ramp_down = DEFAULT_DOWN;
	}

	/* Use only first pulse configuration */
	fan5646_set_pulse(pdata->delay_on, pdata->delay_off,
			  pdata->ramp_up, pdata->ramp_down, &slew, &pulse);
	ctrl_value = pdata->full_current | FAN5646_PLAY;

	fan5646_power_on(pdata);
	spin_lock_irqsave(&pdata->lock, flags);
	tinywire_send_reset(pdata->com_gpio);
	tinywire_set_reg(pdata->com_gpio, FAN5646_SLEW1_REG, slew,
			   pdata->tsleep);
	tinywire_set_reg(pdata->com_gpio, FAN5646_PULSE1_REG, pulse,
			   pdata->tsleep);
	tinywire_set_reg(pdata->com_gpio, FAN5646_CONTROL_REG, ctrl_value,
			   pdata->tsleep);
	tinywire_send_exec(pdata->com_gpio);
	spin_unlock_irqrestore(&pdata->lock, flags);
	return len;
}

/* LED class device attributes */
static DEVICE_ATTR(control, S_IRUGO | S_IRUSR,
		   fan5646_rgb_show_control, fan5646_rgb_store_control);

static void fan5646_remove_device_files(struct device *dev)
{
	struct fan5646_data *pdata = dev_get_drvdata(dev);

	device_remove_file(pdata->led.dev, &dev_attr_blink);
	device_remove_file(pdata->led.dev, &dev_attr_prot);
	device_remove_file(pdata->led.dev, &dev_attr_settings);
	device_remove_file(pdata->rgb.dev, &dev_attr_control);
}

static int fan5646_create_device_files(struct device *dev)
{
	int ret;
	struct fan5646_data *pdata = dev_get_drvdata(dev);

	ret = device_create_file(pdata->led.dev, &dev_attr_blink);
	if (ret) {
		pr_err("%s: unable to create device file for %s: %d\n",
		       __func__, pdata->led.name, ret);
		fan5646_remove_device_files(dev);
		return ret;
	}

	ret = device_create_file(pdata->led.dev, &dev_attr_prot);
	if (ret) {
		pr_err("%s: unable to create \"prot\" device file %s: %d\n",
		       __func__, pdata->led.name, ret);
		fan5646_remove_device_files(dev);
		return ret;
	}

	ret = device_create_file(pdata->led.dev, &dev_attr_settings);
	if (ret) {
		pr_err("%s: unable to create \"settings\" device file %s: %d\n",
		       __func__, pdata->led.name, ret);
		fan5646_remove_device_files(dev);
		return ret;
	}

	ret = device_create_file(pdata->rgb.dev, &dev_attr_control);
	if (ret) {
		pr_err("%s: unable to create \"settings\" device file %s: %d\n",
		       __func__, pdata->rgb.name, ret);
		fan5646_remove_device_files(dev);
		return ret;
	}

	return 0;
}

static void fan5646_unregister_leds(struct device *dev)
{
	struct fan5646_data *pdata = dev_get_drvdata(dev);
	led_classdev_unregister(&pdata->led);
	led_classdev_unregister(&pdata->rgb);
}

static int fan5646_register_leds(struct device *dev)
{
	int ret;
	struct fan5646_data *pdata = dev_get_drvdata(dev);

	pdata->led.brightness_set = fan5646_brightness_set;
	pdata->led.blink_set = fan5646_blink_set;

	ret = led_classdev_register(dev, &pdata->led);
	if (ret) {
		pr_err("%s: unable to register led %s: error %d\n",
		       __func__, pdata->led.name, ret);
		return ret;
	}

	pdata->rgb.brightness_set = fan5646_brightness_set;
	ret = led_classdev_register(dev, &pdata->rgb);
	if (ret) {
		pr_err("%s: unable to register rgb %s: error %d\n",
		       __func__, pdata->rgb.name, ret);
		led_classdev_unregister(&pdata->led);
		return ret;
	}

	return 0;
}

static int fan5646_of_init(struct device *dev)
{
	struct device_node *np, *sp;
	int rc = 0;
	struct fan5646_data *pdata = dev_get_drvdata(dev);

	np = dev->of_node;
	if (!np)
		return -ENODEV;

	rc = of_property_read_string(np, "linux-name", &pdata->led.name);
	if (rc) {
		dev_err(dev, "Error reading name rc %d\n", rc);
		return rc;
	}

	rc = of_property_read_string(np, "rgb-name", &pdata->rgb.name);
	if (rc) {
		dev_err(dev, "Error reading name rc %d\n", rc);
		return rc;
	}

	rc = of_property_read_u32(np, "full-current",
				  &pdata->full_current);
	if (rc) {
		dev_err(dev, "Error reading current rc %d\n", rc);
		return rc;
	}

	rc = of_property_read_u32(np, "tsleep", &pdata->tsleep);
	if (rc) {
		dev_err(dev, "Error reading tsleep rc %d\n", rc);
		return rc;
	}

	pdata->com_gpio = of_get_named_gpio(np, "gpios", 0);
	if (pdata->com_gpio < 0) {
		dev_err(dev, "Error getting comm gpio\n");
		return -EINVAL;
	}

	/* Done reading fan5646 node. Find node describing our regulator */
	/* For now we need only regulator-name */

	sp = of_parse_phandle(np, "fan5646-supply", 0);
	if (!sp) {
		dev_err(dev, "Error getting vreg node\n");
		return -ENODEV;
	}


	rc = of_property_read_string(sp, "regulator-name", &pdata->regname);
	if (rc)
		dev_err(dev, "Error reading regulator name rc %d\n", rc);

	of_node_put(sp);
	return rc;
}

static int fan5646_probe(struct platform_device *pdev)
{
	int rc;
	struct fan5646_data *pdata;

	pdata = devm_kzalloc(&pdev->dev, sizeof(struct fan5646_data),
			     GFP_KERNEL);
	if (pdata == NULL) {
		dev_err(&pdev->dev, "failed to alloc memory\n");
		return -ENOMEM;
	}

	dev_set_drvdata(&pdev->dev, pdata);

	rc = fan5646_of_init(&pdev->dev);
	if (rc)
		return rc;

	spin_lock_init(&pdata->lock);
	mutex_init(&pdata->mlock);

	rc = gpio_request(pdata->com_gpio, "leds-fan5646_com");
	if (rc) {
		dev_err(&pdev->dev, "gpio_request(%d, fan5646_ctrl) error %d\n",
			pdata->com_gpio, rc);
		return rc;
	}

	rc = gpio_direction_output(pdata->com_gpio, 1);
	if (rc) {
		dev_err(&pdev->dev, "gpio_direction_output(%d, 1) error %d\n",
			pdata->com_gpio, rc);
		goto free_gpios;
	}

	gpio_set_value(pdata->com_gpio, 0);

	pdata->vreg = regulator_get(&pdev->dev, pdata->regname);
	rc = PTR_ERR_OR_ZERO(pdata->vreg);
	if (rc) {
		dev_err(&pdev->dev, "regulator get for %s error %d\n",
			pdata->regname, rc);
		goto free_gpios;
	}

	rc = fan5646_register_leds(&pdev->dev);
	if (rc)
		goto free_gpios;

	rc = fan5646_create_device_files(&pdev->dev);
	if (rc)
		goto unregister;

	return rc;

 unregister:
	fan5646_unregister_leds(&pdev->dev);

 free_gpios:
	gpio_free(pdata->com_gpio);
	mutex_destroy(&pdata->mlock);

	return rc;
}

static void fan5646_remove(struct platform_device *pdev)
{
	struct fan5646_data *pdata = dev_get_drvdata(&pdev->dev);

	gpio_set_value(pdata->com_gpio, 0);
	gpio_free(pdata->com_gpio);
	regulator_disable(pdata->vreg);
	fan5646_remove_device_files(&pdev->dev);
	fan5646_unregister_leds(&pdev->dev);
}

static struct of_device_id fan5646_match_table[] = {
	{.compatible = "fsi,leds-fan5646"},
	{},
};

static struct platform_driver fan5646_driver = {
	.probe = fan5646_probe,
	.remove = fan5646_remove,
	.driver = {
		   .name = "leds-fan5646",
		   .owner = THIS_MODULE,
		   .of_match_table = fan5646_match_table},
};

static int __init fan5646_init(void)
{
	return platform_driver_register(&fan5646_driver);
}

static void __exit fan5646_exit(void)
{
	platform_driver_unregister(&fan5646_driver);
}

module_init(fan5646_init);
module_exit(fan5646_exit);

MODULE_AUTHOR("Motorola Mobility");
MODULE_DESCRIPTION("Fairchild FAN5646 LED Driver");
MODULE_LICENSE("GPL v2");

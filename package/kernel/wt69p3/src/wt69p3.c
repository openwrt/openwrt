// SPDX-License-Identifier: GPL-2.0-only
#include <linux/module.h>
#include <linux/serdev.h>
#include <linux/rtc.h>
#include <linux/bcd.h>
#include <linux/of.h>
#include <linux/mutex.h>
#include <linux/completion.h>
#include <linux/jiffies.h>
#include <linux/reboot.h>
#include <linux/leds.h>
#include <linux/hwmon.h>
#include <linux/spinlock.h>
#include <linux/cleanup.h>
#include <linux/bits.h>
#include <linux/ethtool.h>
#include <linux/netdevice.h>
#include <linux/of_net.h>
#include <linux/rtnetlink.h>

/* Default values */
#define WT69P3_DEFAULT_POWER_OFF_DELAY_SEC	20 /* 0x14 */
#define WT69P3_DEFAULT_REBOOT_DELAY_SEC		10 /* 0x0a */

/* Frame delimiters and lengths */
#define WT69P3_FRAME_START		0xfa
#define WT69P3_FRAME_END		0xfb
#define WT69P3_CMD_LEN			7
#define WT69P3_RTC_READ_REPLY_LEN	13
#define WT69P3_MAX_RX			16

/* Command ACK & unsolicited MCU Events (byte 1) */
#define WT69P3_EVT_RTC_ALARM		0x20
#define WT69P3_EVT_PWR_BTN		0x21
#define WT69P3_EVT_RST_BTN		0x23
#define WT69P3_CMD_PWR_LED		0x26
#define WT69P3_CMD_ACK			0x30

/* Command groups (byte 1)
 *
 * Group 0x02 is used for MCU-driven fan speed control
 * (stop/half/full); on the My Cloud Mirror Gen 1 the fan is
 * controlled via SoC GPIOs (gpio-fan).
 */
#define WT69P3_GRP_RTC			0x01
#define WT69P3_GRP_SYS			0x03

/* RTC commands (group 0x01) */
#define WT69P3_RTC_THSEC		0x00
#define WT69P3_RTC_SEC			0x01
#define WT69P3_RTC_MIN			0x02
#define WT69P3_RTC_HOUR			0x03
#define WT69P3_RTC_WDAY			0x04
#define WT69P3_RTC_MDAY			0x05
#define WT69P3_RTC_MON			0x06
#define WT69P3_RTC_YEAR			0x07
#define WT69P3_RTC_READ			0x08

/* RTC alarm commands (group 0x01) */
#define WT69P3_RTC_ALARM_MON		0x0a
#define WT69P3_RTC_ALARM_MDAY		0x0b
#define WT69P3_RTC_ALARM_HOUR		0x0c
#define WT69P3_RTC_ALARM_MIN		0x0d
#define WT69P3_RTC_ALARM_SEC		0x0e
#define WT69P3_RTC_ALARM_FLAG		0x0f
#define WT69P3_RTC_ALARM_EN		0x10

/* RTC access directions */
#define WT69P3_RTC_DIR_READ		0x01
#define WT69P3_RTC_DIR_WRITE		0x02

/* System commands (group 0x03)
 *
 * Unused/unapplicable commands on My Cloud Mirror Gen 1:
 *   0x04: reserved
 *   0x06: blue power LED - addressed via WT69P3_CMD_PWR_LED
 *   0x07: manufacturing test mode (MFGMode)
 *   0x09: fan status (fan is GPIO-driven)
 *   0x0b: MCU PWM fan duty cycle read/write (RPWMValue/WPWMValue; fan is GPIO-driven)
 */
#define WT69P3_SYS_READY		0x01
#define WT69P3_SYS_APR			0x02
#define WT69P3_SYS_POWER		0x03
#define WT69P3_SYS_MCU_VER		0x05
#define WT69P3_SYS_TEMP			0x08
#define WT69P3_SYS_WOL			0x0a
#define WT69P3_SYS_PHY			0x0c

/* Auto-Power-Recovery (APR) subcommands (group 0x03, cmd 0x02) */
#define WT69P3_APR_DISABLE		0x00
#define WT69P3_APR_ENABLE		0x01
#define WT69P3_APR_STATUS		0x02

/* Power subcommands (group 0x03, cmd 0x03)
 *
 * Unimplemented subcommands:
 *   0x03: UPS-triggered power-off (DeviceShutdownByUPS; redundant with 0x01)
 */
#define WT69P3_POWER_OFF		0x01
#define WT69P3_POWER_REBOOT		0x02

/* LED modes */
#define WT69P3_LED_MODE_OFF	0x00
#define WT69P3_LED_MODE_ON	0x01
#define WT69P3_LED_MODE_BLINK	0x02
#define WT69P3_LED_MODE_BREATH	0x04

/* LED colors */
#define WT69P3_LED_BLUE		0x00
#define WT69P3_LED_YELLOW	0x01
#define WT69P3_LED_RED		0x02
#define WT69P3_LED_MAX		3

#define WT69P3_CMD(grp, cmd, a1, a2, a3) \
	{ WT69P3_FRAME_START, (grp), (cmd), (a1), (a2), (a3), WT69P3_FRAME_END }

struct wt69p3 {
	struct serdev_device *serdev;
	struct rtc_device *rtc;
	struct mutex lock;
	spinlock_t rx_lock;
	struct completion rx_done;
	u8 rx_buf[WT69P3_MAX_RX];
	size_t rx_len;
	bool rx_waiting;
	bool wol_enabled;
};

static u8 power_off_delay_sec = WT69P3_DEFAULT_POWER_OFF_DELAY_SEC;
module_param(power_off_delay_sec, byte, 0644);
MODULE_PARM_DESC(power_off_delay_sec,
		 "Hardware power-off countdown delay in seconds (0-255, default 20)");

static u8 reboot_delay_sec = WT69P3_DEFAULT_REBOOT_DELAY_SEC;
module_param(reboot_delay_sec, byte, 0644);
MODULE_PARM_DESC(reboot_delay_sec,
		 "Hardware reboot countdown delay in seconds (0-255, default 10)");

static void wt69p3_process_frame(struct wt69p3 *mcu, size_t expected_len)
{
	struct device *dev = &mcu->serdev->dev;

	if (mcu->rx_buf[expected_len - 1] != WT69P3_FRAME_END) {
		dev_dbg(dev, "RX framing error: %*ph\n",
			(int)mcu->rx_len, mcu->rx_buf);
		mcu->rx_len = 0;
		return;
	}

	dev_dbg(dev, "RX (%zu): %*ph\n", expected_len,
		(int)expected_len, mcu->rx_buf);

	switch (mcu->rx_buf[1]) {
	case WT69P3_EVT_RTC_ALARM:
		if (mcu->rtc)
			rtc_update_irq(mcu->rtc, 1, RTC_AF | RTC_IRQF);
		mcu->rx_len = 0;
		break;
	case WT69P3_EVT_PWR_BTN:
	case WT69P3_EVT_RST_BTN:
		mcu->rx_len = 0;
		break;
	default:
		if (mcu->rx_waiting)
			complete(&mcu->rx_done);
		else
			mcu->rx_len = 0;
		break;
	}
}

static size_t wt69p3_receive_buf(struct serdev_device *serdev, const u8 *buf,
		size_t count)
{
	struct wt69p3 *mcu = serdev_device_get_drvdata(serdev);
	unsigned long flags;
	size_t i, expected_len;

	spin_lock_irqsave(&mcu->rx_lock, flags);

	for (i = 0; i < count; i++) {
		if (completion_done(&mcu->rx_done))
			break;

		if (mcu->rx_len == 0 && buf[i] != WT69P3_FRAME_START)
			continue;

		if (mcu->rx_len < WT69P3_MAX_RX)
			mcu->rx_buf[mcu->rx_len++] = buf[i];

		if (mcu->rx_len < 4)
			continue;

		if (mcu->rx_buf[1] == WT69P3_GRP_RTC &&
		    mcu->rx_buf[2] == WT69P3_RTC_READ &&
		    mcu->rx_buf[3] == WT69P3_RTC_DIR_READ)
			expected_len = WT69P3_RTC_READ_REPLY_LEN;
		else
			expected_len = WT69P3_CMD_LEN;

		if (mcu->rx_len == expected_len)
			wt69p3_process_frame(mcu, expected_len);
	}

	spin_unlock_irqrestore(&mcu->rx_lock, flags);

	return count;
}

static void wt69p3_serdev_close(void *data)
{
	serdev_device_close(data);
}

static const struct serdev_device_ops wt69p3_serdev_ops = {
	.receive_buf = wt69p3_receive_buf,
};

static int __wt69p3_query_cmd(struct wt69p3 *mcu, const u8 *cmd,
		size_t len, u8 *reply, size_t reply_max)
{
	int ret;
	unsigned long timeout, flags;

	lockdep_assert_held(&mcu->lock);

	spin_lock_irqsave(&mcu->rx_lock, flags);
	mcu->rx_len = 0;
	mcu->rx_waiting = true;
	reinit_completion(&mcu->rx_done);
	spin_unlock_irqrestore(&mcu->rx_lock, flags);

	dev_dbg(&mcu->serdev->dev, "TX (%zu): %*ph\n", len, (int)len, cmd);
	ret = serdev_device_write_buf(mcu->serdev, cmd, len);
	if (ret < 0) {
		spin_lock_irqsave(&mcu->rx_lock, flags);
		mcu->rx_waiting = false;
		mcu->rx_len = 0;
		reinit_completion(&mcu->rx_done);
		spin_unlock_irqrestore(&mcu->rx_lock, flags);
		return ret;
	}
	serdev_device_wait_until_sent(mcu->serdev, msecs_to_jiffies(100));

	timeout = wait_for_completion_timeout(&mcu->rx_done, msecs_to_jiffies(300));

	spin_lock_irqsave(&mcu->rx_lock, flags);
	mcu->rx_waiting = false;
	if (!timeout) {
		mcu->rx_len = 0;
		reinit_completion(&mcu->rx_done);
		spin_unlock_irqrestore(&mcu->rx_lock, flags);
		return -ETIMEDOUT;
	}

	ret = min(mcu->rx_len, reply_max);
	memcpy(reply, mcu->rx_buf, ret);
	mcu->rx_len = 0;
	reinit_completion(&mcu->rx_done);
	spin_unlock_irqrestore(&mcu->rx_lock, flags);

	return ret;
}

static int wt69p3_query_cmd(struct wt69p3 *mcu, const u8 *cmd,
			    size_t len, u8 *reply, size_t reply_max)
{
	int ret;

	mutex_lock(&mcu->lock);
	ret = __wt69p3_query_cmd(mcu, cmd, len, reply, reply_max);
	mutex_unlock(&mcu->lock);

	return ret;
}

static int __wt69p3_write_cmd(struct wt69p3 *mcu, const u8 *cmd, size_t len)
{
	u8 reply[WT69P3_MAX_RX];
	int ret;

	ret = __wt69p3_query_cmd(mcu, cmd, len, reply, sizeof(reply));
	if (ret < WT69P3_CMD_LEN)
		return ret < 0 ? ret : -EIO;

	if (reply[0] != WT69P3_FRAME_START ||
	    reply[1] != WT69P3_CMD_ACK ||
	    reply[6] != WT69P3_FRAME_END)
		return -EIO;

	return 0;
}

static int wt69p3_write_cmd(struct wt69p3 *mcu, const u8 *cmd, size_t len)
{
	int ret;

	mutex_lock(&mcu->lock);
	ret = __wt69p3_write_cmd(mcu, cmd, len);
	mutex_unlock(&mcu->lock);

	return ret;
}

static int wt69p3_send_cmd(struct wt69p3 *mcu, const u8 *cmd, size_t len)
{
	int ret;

	dev_dbg(&mcu->serdev->dev, "TX (%zu): %*ph\n", len, (int)len, cmd);
	mutex_lock(&mcu->lock);
	ret = serdev_device_write_buf(mcu->serdev, cmd, len);
	serdev_device_wait_until_sent(mcu->serdev, msecs_to_jiffies(100));
	mutex_unlock(&mcu->lock);

	return ret < 0 ? ret : 0;
}

static int wt69p3_set_wol(struct wt69p3 *mcu, bool enable)
{
	u8 cmd[WT69P3_CMD_LEN] = WT69P3_CMD(
		WT69P3_GRP_SYS, WT69P3_SYS_WOL, enable ? 0x01 : 0x00, 0x00, 0x00);

	return wt69p3_write_cmd(mcu, cmd, sizeof(cmd));
}

static umode_t wt69p3_hwmon_is_visible(const void *drvdata,
		enum hwmon_sensor_types type,
		u32 attr, int channel)
{
	switch (type) {
	case hwmon_temp:
		return 0444;
	default:
		return 0;
	}
}

static int wt69p3_rtc_read_time(struct device *dev, struct rtc_time *tm)
{
	struct wt69p3 *mcu = dev_get_drvdata(dev);
	static const u8 rtc_read_cmd[WT69P3_CMD_LEN] = WT69P3_CMD(
			WT69P3_GRP_RTC, WT69P3_RTC_READ, 0x01, 0x01, 0x00);
	u8 reply[WT69P3_MAX_RX];
	int ret;

	ret = wt69p3_query_cmd(mcu, rtc_read_cmd, sizeof(rtc_read_cmd),
				 reply, sizeof(reply));
	if (ret < 13)
		return ret < 0 ? ret : -EIO;

	if (reply[0] != WT69P3_FRAME_START ||
	    reply[1] != WT69P3_GRP_RTC ||
	    reply[2] != WT69P3_RTC_READ ||
	    reply[3] != 0x01 ||
	    reply[4] != 0x01 ||
	    reply[WT69P3_RTC_READ_REPLY_LEN - 1] != WT69P3_FRAME_END)
		return -EIO;

	tm->tm_sec  = bcd2bin(reply[5]);
	tm->tm_min  = bcd2bin(reply[6]);
	tm->tm_hour = bcd2bin(reply[7]);
	tm->tm_wday = bcd2bin(reply[8]) % 7;
	tm->tm_mday = bcd2bin(reply[9]);
	tm->tm_mon  = bcd2bin(reply[10]) - 1;
	tm->tm_year = bcd2bin(reply[11]) + 100;

	return rtc_valid_tm(tm);
}

static int __wt69p3_rtc_write_reg(struct wt69p3 *mcu, u8 reg, u8 val)
{
	u8 cmd[WT69P3_CMD_LEN] = WT69P3_CMD(WT69P3_GRP_RTC, reg, WT69P3_RTC_DIR_WRITE,
			0x01, val);

	return __wt69p3_write_cmd(mcu, cmd, sizeof(cmd));
}

static int wt69p3_rtc_set_time(struct device *dev, struct rtc_time *tm)
{
	struct wt69p3 *mcu = dev_get_drvdata(dev);
	const struct {
		u8 reg;
		u8 val;
	} fields[] = {
		{ WT69P3_RTC_YEAR, bin2bcd(tm->tm_year - 100) },
		{ WT69P3_RTC_MON, bin2bcd(tm->tm_mon + 1) },
		{ WT69P3_RTC_MDAY, bin2bcd(tm->tm_mday) },
		{ WT69P3_RTC_WDAY, bin2bcd(tm->tm_wday) },
		{ WT69P3_RTC_HOUR, bin2bcd(tm->tm_hour) },
		{ WT69P3_RTC_MIN, bin2bcd(tm->tm_min) },
		{ WT69P3_RTC_SEC, bin2bcd(tm->tm_sec) },
		{ WT69P3_RTC_THSEC, 0x00 },
	};
	int i, ret = 0;

	if (tm->tm_year < 100 || tm->tm_year > 199)
		return -EINVAL;

	mutex_lock(&mcu->lock);
	for (i = 0; i < ARRAY_SIZE(fields); i++) {
		ret = __wt69p3_rtc_write_reg(mcu, fields[i].reg, fields[i].val);
		if (ret)
			break;
	}
	mutex_unlock(&mcu->lock);

	return ret;
}

static int __wt69p3_rtc_read_reg(struct wt69p3 *mcu, u8 reg, u8 *val)
{
	u8 cmd[WT69P3_CMD_LEN] =
		WT69P3_CMD(WT69P3_GRP_RTC, reg, WT69P3_RTC_DIR_READ, 0x01, 0x00);
	u8 reply[WT69P3_MAX_RX];
	int ret;

	ret = __wt69p3_query_cmd(mcu, cmd, sizeof(cmd), reply, sizeof(reply));
	if (ret < WT69P3_CMD_LEN)
		return ret < 0 ? ret : -EIO;

	if (reply[0] != WT69P3_FRAME_START ||
	    reply[1] != WT69P3_GRP_RTC ||
	    reply[2] != reg ||
	    reply[3] != 0x01 ||
	    reply[6] != WT69P3_FRAME_END)
		return -EIO;

	*val = reply[5];
	return 0;
}

static int wt69p3_rtc_read_alarm(struct device *dev, struct rtc_wkalrm *alrm)
{
	struct wt69p3 *mcu = dev_get_drvdata(dev);
	struct rtc_time now;
	u8 sec, min, hour, mday, mon, flag;
	int ret;

	ret = wt69p3_rtc_read_time(dev, &now);
	if (ret)
		return ret;

	mutex_lock(&mcu->lock);
	ret = __wt69p3_rtc_read_reg(mcu, WT69P3_RTC_ALARM_SEC, &sec) ?:
	      __wt69p3_rtc_read_reg(mcu, WT69P3_RTC_ALARM_MIN, &min) ?:
	      __wt69p3_rtc_read_reg(mcu, WT69P3_RTC_ALARM_HOUR, &hour) ?:
	      __wt69p3_rtc_read_reg(mcu, WT69P3_RTC_ALARM_MDAY, &mday) ?:
	      __wt69p3_rtc_read_reg(mcu, WT69P3_RTC_ALARM_MON, &mon) ?:
	      __wt69p3_rtc_read_reg(mcu, WT69P3_RTC_ALARM_FLAG, &flag);
	mutex_unlock(&mcu->lock);
	if (ret)
		return ret;

	alrm->time.tm_sec  = bcd2bin(sec);
	alrm->time.tm_min  = bcd2bin(min);
	alrm->time.tm_hour = bcd2bin(hour);
	alrm->time.tm_mday = bcd2bin(mday);
	alrm->time.tm_mon  = bcd2bin(mon) - 1;
	alrm->time.tm_year = now.tm_year;
	alrm->enabled = !!(flag & 0x01);
	alrm->pending = 0;

	return rtc_valid_tm(&alrm->time);
}

static int wt69p3_rtc_alarm_irq_enable(struct device *dev, unsigned int enabled)
{
	struct wt69p3 *mcu = dev_get_drvdata(dev);
	u8 cmd[WT69P3_CMD_LEN] = WT69P3_CMD(WT69P3_GRP_RTC, WT69P3_RTC_ALARM_EN,
			WT69P3_RTC_DIR_WRITE, 0x01, enabled ? 0x01 : 0x00);

	return wt69p3_write_cmd(mcu, cmd, sizeof(cmd));
}

static int wt69p3_rtc_set_alarm(struct device *dev, struct rtc_wkalrm *alrm)
{
	struct wt69p3 *mcu = dev_get_drvdata(dev);
	const struct {
		u8 reg;
		u8 val;
	} fields[] = {
		{ WT69P3_RTC_ALARM_SEC,  bin2bcd(alrm->time.tm_sec) },
		{ WT69P3_RTC_ALARM_MIN,  bin2bcd(alrm->time.tm_min) },
		{ WT69P3_RTC_ALARM_HOUR, bin2bcd(alrm->time.tm_hour) },
		{ WT69P3_RTC_ALARM_MDAY, bin2bcd(alrm->time.tm_mday) },
		{ WT69P3_RTC_ALARM_MON,  bin2bcd(alrm->time.tm_mon + 1) },
		{ WT69P3_RTC_ALARM_EN,   alrm->enabled ? 0x01 : 0x00 },
	};
	int i, ret = 0;

	mutex_lock(&mcu->lock);
	for (i = 0; i < ARRAY_SIZE(fields); i++) {
		ret = __wt69p3_rtc_write_reg(mcu, fields[i].reg, fields[i].val);
		if (ret)
			break;
	}
	mutex_unlock(&mcu->lock);

	return ret;
}

static const struct rtc_class_ops wt69p3_rtc_ops = {
	.read_time = wt69p3_rtc_read_time,
	.set_time = wt69p3_rtc_set_time,
	.read_alarm       = wt69p3_rtc_read_alarm,
	.set_alarm        = wt69p3_rtc_set_alarm,
	.alarm_irq_enable = wt69p3_rtc_alarm_irq_enable,
};

/* NTC thermistor ADC-to-Celsius lookup table (indices 0..186 -> 116..0 °C) */
static const u8 wt69p3_temp_table[] = {
	0x74, 0x73, 0x72, 0x71, 0x70, 0x6f, 0x6e, 0x6d,
	0x6c, 0x6b, 0x6a, 0x69, 0x68, 0x67, 0x66, 0x65,
	0x64, 0x63, 0x62, 0x61, 0x60, 0x5f, 0x5e, 0x5d,
	0x5c, 0x5b, 0x5a, 0x59, 0x58, 0x57, 0x56, 0x55,
	0x54, 0x53, 0x52, 0x51, 0x50, 0x4f, 0x4e, 0x4d,
	0x4c, 0x4b, 0x4a, 0x49, 0x48, 0x47, 0x46, 0x45,
	0x44, 0x43, 0x42, 0x41, 0x41, 0x40, 0x3f, 0x3e,
	0x3e, 0x3d, 0x3d, 0x3c, 0x3b, 0x3a, 0x3a, 0x39,
	0x38, 0x38, 0x37, 0x36, 0x36, 0x35, 0x34, 0x34,
	0x33, 0x33, 0x32, 0x31, 0x31, 0x30, 0x30, 0x2f,
	0x2f, 0x2e, 0x2e, 0x2d, 0x2c, 0x2c, 0x2b, 0x2b,
	0x2a, 0x2a, 0x29, 0x29, 0x28, 0x28, 0x27, 0x27,
	0x27, 0x26, 0x26, 0x25, 0x25, 0x24, 0x24, 0x23,
	0x23, 0x22, 0x22, 0x21, 0x21, 0x21, 0x20, 0x20,
	0x1f, 0x1f, 0x1e, 0x1e, 0x1e, 0x1d, 0x1d, 0x1c,
	0x1c, 0x1b, 0x1b, 0x1b, 0x1b, 0x1a, 0x19, 0x19,
	0x19, 0x18, 0x18, 0x17, 0x17, 0x16, 0x16, 0x16,
	0x15, 0x15, 0x14, 0x14, 0x14, 0x13, 0x13, 0x12,
	0x12, 0x12, 0x11, 0x11, 0x10, 0x10, 0x10, 0x0f,
	0x0f, 0x0e, 0x0e, 0x0e, 0x0d, 0x0d, 0x0c, 0x0c,
	0x0c, 0x0b, 0x0b, 0x0a, 0x0a, 0x09, 0x09, 0x09,
	0x08, 0x08, 0x07, 0x07, 0x07, 0x06, 0x06, 0x05,
	0x05, 0x04, 0x04, 0x04, 0x03, 0x03, 0x02, 0x02,
	0x01, 0x01, 0x00,
};

static int wt69p3_read_temp(struct wt69p3 *mcu, long *val)
{
	static const u8 temp_cmd[WT69P3_CMD_LEN] = WT69P3_CMD(
			WT69P3_GRP_SYS, WT69P3_SYS_TEMP, 0x00, 0x00, 0x00);
	u8 reply[WT69P3_MAX_RX];
	int ret;

	ret = wt69p3_query_cmd(mcu, temp_cmd, sizeof(temp_cmd),
					 reply, sizeof(reply));
	if (ret < WT69P3_CMD_LEN)
		return ret < 0 ? ret : -EIO;

	if (reply[0] != WT69P3_FRAME_START ||
	    reply[1] != 0x03 ||
	    reply[2] != 0x08 ||
	    reply[3] != 0x00 ||
	    reply[5] == 0 ||
	    reply[5] >= ARRAY_SIZE(wt69p3_temp_table) ||
	    reply[6] != WT69P3_FRAME_END)
		return -EIO;

	*val = (long)wt69p3_temp_table[reply[5]] * 1000;

	return 0;
}

static int wt69p3_hwmon_read(struct device *dev, enum hwmon_sensor_types type,
		u32 attr, int channel, long *val)
{
	struct wt69p3 *mcu = dev_get_drvdata(dev);

	switch (type) {
	case hwmon_temp:
		if (attr != hwmon_temp_input)
			return -EOPNOTSUPP;
		return wt69p3_read_temp(mcu, val);

	default:
		return -EOPNOTSUPP;
	}
}

static int wt69p3_hwmon_read_string(struct device *dev, enum hwmon_sensor_types type,
		u32 attr, int channel, const char **str)
{
	if (type == hwmon_temp && attr == hwmon_temp_label && channel == 0) {
		*str = "Chassis";
		return 0;
	}

	return -EOPNOTSUPP;
}

static const struct hwmon_channel_info * const wt69p3_hwmon_info[] = {
	HWMON_CHANNEL_INFO(temp, HWMON_T_INPUT | HWMON_T_LABEL),
	NULL
};

static const struct hwmon_ops wt69p3_hwmon_ops = {
	.is_visible = wt69p3_hwmon_is_visible,
	.read = wt69p3_hwmon_read,
	.read_string = wt69p3_hwmon_read_string,
};

static const struct hwmon_chip_info wt69p3_chip_info = {
	.ops = &wt69p3_hwmon_ops,
	.info = wt69p3_hwmon_info,
};

static int wt69p3_set_phy(struct wt69p3 *mcu, bool enable)
{
	u8 cmd[WT69P3_CMD_LEN] = WT69P3_CMD(
		WT69P3_GRP_SYS, WT69P3_SYS_PHY, enable ? 0x01 : 0x00, 0x00, 0x00);

	return wt69p3_write_cmd(mcu, cmd, sizeof(cmd));
}

static bool wt69p3_eth_wol_enabled(struct device *dev)
{
	struct device_node *np;
	struct net_device *ndev;
	struct ethtool_wolinfo wol = { .cmd = ETHTOOL_GWOL };

	np = of_parse_phandle(dev->of_node, "ethernet", 0);
	if (!np)
		return false;

	ndev = of_find_net_device_by_node(np);
	of_node_put(np);
	if (!ndev)
		return false;

	rtnl_lock();
	if (ndev->ethtool_ops && ndev->ethtool_ops->get_wol)
		ndev->ethtool_ops->get_wol(ndev, &wol);
	rtnl_unlock();

	dev_put(ndev);
	return !!wol.wolopts;
}

static int wt69p3_power_off_prepare(struct sys_off_data *data)
{
	struct wt69p3 *mcu = data->cb_data;

	mcu->wol_enabled = wt69p3_eth_wol_enabled(&mcu->serdev->dev);

	return NOTIFY_DONE;
}

static int wt69p3_power_off(struct sys_off_data *data)
{
	struct wt69p3 *mcu = data->cb_data;
	const u8 cmd[WT69P3_CMD_LEN] = WT69P3_CMD(
			WT69P3_GRP_SYS, WT69P3_SYS_POWER,
			WT69P3_POWER_OFF, 0x01,
			power_off_delay_sec);
	int ret;

	wt69p3_set_wol(mcu, mcu->wol_enabled);
	wt69p3_set_phy(mcu, mcu->wol_enabled);

	ret = wt69p3_send_cmd(mcu, cmd, sizeof(cmd));
	if (ret)
		return notifier_from_errno(ret);

	return NOTIFY_DONE;
}

struct wt69p3_led {
	struct led_classdev cdev;
	struct wt69p3 *mcu;
	u8 color;
};

static int wt69p3_led_write_mode(struct wt69p3_led *led, u8 mode)
{
	u8 arg = (mode << 4) | BIT(led->color);
	u8 cmd[WT69P3_CMD_LEN] = WT69P3_CMD(
			WT69P3_CMD_PWR_LED, 0x00, arg, 0x00, 0x01);

	return wt69p3_write_cmd(led->mcu, cmd, sizeof(cmd));
}

static int wt69p3_led_set(struct led_classdev *led_cdev, enum led_brightness brightness)
{
	struct wt69p3_led *led = container_of(led_cdev, struct wt69p3_led, cdev);

	return wt69p3_led_write_mode(led, brightness ?
			WT69P3_LED_MODE_ON : WT69P3_LED_MODE_OFF);
}

static int wt69p3_led_blink_set(struct led_classdev *led_cdev, unsigned long *delay_on,
		unsigned long *delay_off)
{
	struct wt69p3_led *led = container_of(led_cdev, struct wt69p3_led, cdev);

	if (*delay_on == 0 && *delay_off == 0) {
		*delay_on = 500;
		*delay_off = 500;
	}

	if (*delay_on == 500 && *delay_off == 500)
		return wt69p3_led_write_mode(led, WT69P3_LED_MODE_BLINK);

	return -EINVAL;
}

static int wt69p3_get_mcu_ver(struct wt69p3 *mcu, u8 *ver)
{
	static const u8 cmd[WT69P3_CMD_LEN] = WT69P3_CMD(
		WT69P3_GRP_SYS, WT69P3_SYS_MCU_VER, 0x00, 0x00, 0x00);
	u8 reply[WT69P3_MAX_RX];
	int ret;

	ret = wt69p3_query_cmd(mcu, cmd, sizeof(cmd), reply, sizeof(reply));
	if (ret < WT69P3_CMD_LEN)
		return ret < 0 ? ret : -EIO;

	if (reply[0] != WT69P3_FRAME_START ||
	    reply[1] != WT69P3_GRP_SYS ||
	    reply[2] != WT69P3_SYS_MCU_VER ||
	    reply[3] != 0x00 ||
	    reply[4] != 0x01 ||
	    reply[WT69P3_CMD_LEN - 1] != WT69P3_FRAME_END)
		return -EIO;

	*ver = reply[5] & GENMASK(5, 0);

	return 0;
}

static ssize_t fw_version_show(struct device *dev, struct device_attribute *attr, char *buf)
{
	struct wt69p3 *mcu = dev_get_drvdata(dev);
	u8 ver;
	int ret;

	ret = wt69p3_get_mcu_ver(mcu, &ver);
	if (ret)
		return ret;

	return sysfs_emit(buf, "1.%.2d\n", ver);
}
static DEVICE_ATTR_RO(fw_version);

static int wt69p3_restart_prepare(struct sys_off_data *data)
{
	struct wt69p3 *mcu = data->cb_data;
	const u8 cmd[WT69P3_CMD_LEN] = WT69P3_CMD(WT69P3_GRP_SYS,
			WT69P3_SYS_POWER, WT69P3_POWER_REBOOT, 0x01,
			reboot_delay_sec);
	int ret;

	ret = wt69p3_send_cmd(mcu, cmd, sizeof(cmd));
	if (ret)
		return notifier_from_errno(ret);

	return NOTIFY_DONE;
}

static int wt69p3_led_pattern_set(struct led_classdev *led_cdev,
				  struct led_pattern *pattern, u32 len, int repeat)
{
	struct wt69p3_led *led = container_of(led_cdev, struct wt69p3_led, cdev);

	/* Require continuous hardware breathing (repeat == -1) with at least 2 points */
	if (repeat != -1 || len < 2)
		return -EINVAL;

	/* Programmable hardware fade curve on blue power LED via 0x26 */
	if (led->color == WT69P3_LED_BLUE && pattern[0].delta_t > 0) {
		u32 half_period_ms = pattern[0].delta_t;
		u8 pwm_limit = 255;
		u8 count_sector = 5; /* 51 steps per ramp */
		u8 delay_10ms = clamp_t(u32, half_period_ms / (51 * 10), 1, 255);
		u8 cmd[WT69P3_CMD_LEN] = {
			WT69P3_FRAME_START, WT69P3_CMD_PWR_LED,
			pwm_limit, count_sector, delay_10ms, 0x00,
			WT69P3_FRAME_END
		};

		return wt69p3_write_cmd(led->mcu, cmd, sizeof(cmd));
	}

	/* Fallback to MCU's built-in breathing preset (0x03 0x06 0x04) */
	return wt69p3_led_write_mode(led, WT69P3_LED_MODE_BREATH);
}

static int wt69p3_led_pattern_clear(struct led_classdev *led_cdev)
{
	struct wt69p3_led *led = container_of(led_cdev, struct wt69p3_led, cdev);

	return wt69p3_led_write_mode(led, WT69P3_LED_MODE_OFF);
}

static int wt69p3_probe(struct serdev_device *serdev)
{
	struct device *dev = &serdev->dev;
	struct wt69p3 *mcu;
	struct device *hwmon_dev;
	struct fwnode_handle *leds_fwnode __free(fwnode_handle) = NULL;
	static const u8 cmd[WT69P3_CMD_LEN] = WT69P3_CMD(WT69P3_GRP_SYS, WT69P3_SYS_READY,
		       0x00, 0x00, 0x00);
	int ret;

	mcu = devm_kzalloc(&serdev->dev, sizeof(*mcu), GFP_KERNEL);
	if (!mcu)
		return -ENOMEM;

	mcu->serdev = serdev;
	mutex_init(&mcu->lock);
	init_completion(&mcu->rx_done);
	serdev_device_set_drvdata(serdev, mcu);
	serdev_device_set_client_ops(serdev, &wt69p3_serdev_ops);
	spin_lock_init(&mcu->rx_lock);
	ret = serdev_device_open(serdev);
	if (ret)
		return ret;
	ret = devm_add_action_or_reset(&serdev->dev, wt69p3_serdev_close, serdev);
	if (ret)
		return ret;

	serdev_device_set_baudrate(serdev, 115200);
	serdev_device_set_flow_control(serdev, false);
	serdev_device_set_parity(serdev, SERDEV_PARITY_NONE);

	/* Signal "OS load done" */
	wt69p3_write_cmd(mcu, cmd, sizeof(cmd));

	/* Register HWMON temperature sensor */
	struct device_node *hwmon_np __free(device_node) =
		of_get_child_by_name(dev->of_node, "hwmon");
	if (hwmon_np) {
		hwmon_dev = devm_hwmon_device_register_with_info(dev, "wt69p3", mcu,
							 &wt69p3_chip_info, NULL);
		if (IS_ERR(hwmon_dev))
			return PTR_ERR(hwmon_dev);
	}

	/* Register RTC device */
	struct device_node *rtc_np __free(device_node) = of_get_child_by_name(
			dev->of_node, "rtc");
	if (rtc_np) {
		struct rtc_device *rtc;
		unsigned long flags;

		rtc = devm_rtc_allocate_device(dev);
		if (IS_ERR(rtc))
			return PTR_ERR(rtc);

		device_set_node(&rtc->dev, of_fwnode_handle(rtc_np));
		rtc->ops = &wt69p3_rtc_ops;
		set_bit(RTC_FEATURE_ALARM, rtc->features);
		device_init_wakeup(dev, of_property_read_bool(rtc_np, "wakeup-source"));

		spin_lock_irqsave(&mcu->rx_lock, flags);
		mcu->rtc = rtc;
		spin_unlock_irqrestore(&mcu->rx_lock, flags);

		ret = devm_rtc_register_device(rtc);
		if (ret)
			return ret;
	}

	/* Register LEDs */
	leds_fwnode = device_get_named_child_node(dev, "leds");
	fwnode_for_each_child_node_scoped(leds_fwnode, child) {
		struct led_init_data init_data = { .fwnode = child };
		struct wt69p3_led *led;
		enum led_default_state state = led_init_default_state_get(child);
		u32 reg;

		if (fwnode_property_read_u32(child, "reg", &reg) ||
				reg >= WT69P3_LED_MAX)
			continue;

		led = devm_kzalloc(dev, sizeof(*led), GFP_KERNEL);
		if (!led)
			return -ENOMEM;

		led->mcu = mcu;
		led->color = reg;
		led->cdev.max_brightness = 1;
		led->cdev.brightness_set_blocking = wt69p3_led_set;
		led->cdev.blink_set = wt69p3_led_blink_set;
		led->cdev.pattern_set = wt69p3_led_pattern_set;
		led->cdev.pattern_clear = wt69p3_led_pattern_clear;
		switch (state) {
		case LEDS_DEFSTATE_ON:
			led->cdev.brightness = 1;
			break;
		case LEDS_DEFSTATE_KEEP:
			break;
		case LEDS_DEFSTATE_OFF:
		default:
			led->cdev.brightness = LED_OFF;
		}
		ret = devm_led_classdev_register_ext(dev, &led->cdev,
							&init_data);
		if (ret)
			return ret;

		if (state != LEDS_DEFSTATE_KEEP)
			wt69p3_led_set(&led->cdev, led->cdev.brightness);
	}

	ret = devm_register_sys_off_handler(dev, SYS_OFF_MODE_POWER_OFF_PREPARE,
					    SYS_OFF_PRIO_DEFAULT,
					    wt69p3_power_off_prepare, mcu);
	if (ret)
		return dev_err_probe(&serdev->dev, ret,
				"Failed to register power-off prepare handler\n");

	ret = devm_register_sys_off_handler(&serdev->dev,
			SYS_OFF_MODE_POWER_OFF,
			SYS_OFF_PRIO_DEFAULT,
			wt69p3_power_off,
			mcu);
	if (ret)
		return dev_err_probe(&serdev->dev, ret,
				"Failed to register power-off handler\n");

	ret = devm_register_sys_off_handler(&serdev->dev,
			SYS_OFF_MODE_RESTART_PREPARE,
			SYS_OFF_PRIO_DEFAULT,
			wt69p3_restart_prepare,
			mcu);
	if (ret)
		return dev_err_probe(&serdev->dev, ret,
				"Failed to register restart handler\n");

	return 0;
}

static const struct of_device_id wt69p3_of_match[] = {
	{ .compatible = "weltrend,wt69p3", },
	{ }
};
MODULE_DEVICE_TABLE(of, wt69p3_of_match);

static ssize_t power_recovery_show(struct device *dev,
				   struct device_attribute *attr, char *buf)
{
	struct wt69p3 *mcu = dev_get_drvdata(dev);
	static const u8 cmd[WT69P3_CMD_LEN] =
		WT69P3_CMD(WT69P3_GRP_SYS, WT69P3_SYS_APR,
			   WT69P3_APR_STATUS, 0x00, 0x00);
	u8 reply[WT69P3_MAX_RX];
	int ret;

	ret = wt69p3_query_cmd(mcu, cmd, sizeof(cmd), reply, sizeof(reply));
	if (ret < WT69P3_CMD_LEN)
		return ret < 0 ? ret : -EIO;

	if (reply[0] != WT69P3_FRAME_START ||
	    reply[1] != WT69P3_GRP_SYS ||
	    reply[2] != WT69P3_SYS_APR ||
	    reply[3] != WT69P3_APR_STATUS ||
	    reply[6] != WT69P3_FRAME_END)
		return -EIO;

	return sysfs_emit(buf, "%u\n", !!(reply[5] & 0x80));
}

static ssize_t power_recovery_store(struct device *dev,
				    struct device_attribute *attr,
				    const char *buf, size_t count)
{
	struct wt69p3 *mcu = dev_get_drvdata(dev);
	bool enable;
	int ret;

	ret = kstrtobool(buf, &enable);
	if (ret)
		return ret;

	u8 cmd[WT69P3_CMD_LEN] =
		WT69P3_CMD(WT69P3_GRP_SYS, WT69P3_SYS_APR,
			   enable ? WT69P3_APR_ENABLE : WT69P3_APR_DISABLE,
			   0x00, 0x00);

	ret = wt69p3_write_cmd(mcu, cmd, sizeof(cmd));
	if (ret)
		return ret;

	return count;
}
static DEVICE_ATTR_RW(power_recovery);

static struct attribute *wt69p3_attrs[] = {
	&dev_attr_fw_version.attr,
	&dev_attr_power_recovery.attr,
	NULL,
};
ATTRIBUTE_GROUPS(wt69p3);

static struct serdev_device_driver wt69p3_driver = {
	.probe = wt69p3_probe,
	.driver = {
		.name = "wt69p3",
		.of_match_table = wt69p3_of_match,
		.dev_groups = wt69p3_groups,
	},
};
module_serdev_device_driver(wt69p3_driver);

MODULE_AUTHOR("Lutz Müller <github@topfrose.de>");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Weltrend Control MCU serdev LED, hwmon and RTC driver");

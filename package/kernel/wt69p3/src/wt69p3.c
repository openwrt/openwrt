// SPDX-License-Identifier: GPL-2.0-only
#include <linux/module.h>
#include <linux/serdev.h>
#include <linux/gpio/driver.h>
#include <linux/hwmon.h>
#include <linux/rtc.h>
#include <linux/bcd.h>
#include <linux/of.h>
#include <linux/slab.h>
#include <linux/mutex.h>
#include <linux/completion.h>
#include <linux/jiffies.h>
#include <linux/reboot.h>

#define WT69P3_FRAME_START	0xfa
#define WT69P3_FRAME_END	0xfb
#define WT69P3_CMD_LEN	7
#define WT69P3_MAX_RX		16

struct wt69p3 {
	struct serdev_device *serdev;
	struct gpio_chip gc;
	struct rtc_device *rtc;
	struct mutex lock;
	struct completion rx_done;
	u8 rx_buf[WT69P3_MAX_RX];
	size_t rx_len;
	u8 gpio_state;
};

static size_t wt69p3_receive_buf(struct serdev_device *serdev,
				   const u8 *buf, size_t count)
{
	struct wt69p3 *mcu = serdev_device_get_drvdata(serdev);
	size_t i;

	for (i = 0; i < count; i++) {
		if (mcu->rx_len == 0 && buf[i] != WT69P3_FRAME_START)
			continue;

		if (mcu->rx_len < sizeof(mcu->rx_buf))
			mcu->rx_buf[mcu->rx_len++] = buf[i];

		if (buf[i] == WT69P3_FRAME_END && mcu->rx_len >= WT69P3_CMD_LEN) {
			complete(&mcu->rx_done);
			break;
		}
	}

	return count;
}

static const struct serdev_device_ops wt69p3_serdev_ops = {
	.receive_buf = wt69p3_receive_buf,
};

static int wt69p3_send_cmd(struct wt69p3 *mcu, const u8 *cmd, size_t len)
{
	int ret;

	mutex_lock(&mcu->lock);
	ret = serdev_device_write_buf(mcu->serdev, cmd, len);
	serdev_device_wait_until_sent(mcu->serdev, msecs_to_jiffies(100));
	mutex_unlock(&mcu->lock);

	return ret < 0 ? ret : 0;
}

static int wt69p3_query_cmd(struct wt69p3 *mcu, const u8 *cmd,
			      size_t len, u8 *reply, size_t reply_max)
{
	int ret;
	unsigned long timeout;

	mutex_lock(&mcu->lock);
	mcu->rx_len = 0;
	reinit_completion(&mcu->rx_done);

	ret = serdev_device_write_buf(mcu->serdev, cmd, len);
	if (ret < 0) {
		mutex_unlock(&mcu->lock);
		return ret;
	}
	serdev_device_wait_until_sent(mcu->serdev, msecs_to_jiffies(100));

	timeout = wait_for_completion_timeout(&mcu->rx_done, msecs_to_jiffies(300));
	if (!timeout) {
		mutex_unlock(&mcu->lock);
		return -ETIMEDOUT;
	}

	ret = min(mcu->rx_len, reply_max);
	memcpy(reply, mcu->rx_buf, ret);
	mutex_unlock(&mcu->lock);

	return ret;
}

/* --- RTC Driver (/dev/rtc0) --- */
static int wt69p3_rtc_read_time(struct device *dev, struct rtc_time *tm)
{
	struct wt69p3 *mcu = dev_get_drvdata(dev);
	static const u8 rtc_read_cmd[WT69P3_CMD_LEN] = {
		0xfa, 0x01, 0x08, 0x01, 0x01, 0x00, 0xfb
	};
	u8 reply[WT69P3_MAX_RX];
	int ret;

	ret = wt69p3_query_cmd(mcu, rtc_read_cmd, sizeof(rtc_read_cmd),
				 reply, sizeof(reply));
	if (ret < 9)
		return ret < 0 ? ret : -EIO;

	/* Log raw RTC frame once/at debug level to verify BCD vs decimal byte offsets */
	dev_dbg(dev, "RTC raw reply (%d bytes): %*ph\n", ret, ret, reply);

	/*
	 * Standard Weltrend RTC response layout (BCD encoded):
	 * reply[3]=sec, reply[4]=min, reply[5]=hour,
	 * reply[6]=mday, reply[7]=mon (1-12), reply[8]=year (00-99 -> 2000+)
	 */
	tm->tm_sec  = bcd2bin(reply[3]);
	tm->tm_min  = bcd2bin(reply[4]);
	tm->tm_hour = bcd2bin(reply[5]);
	tm->tm_mday = bcd2bin(reply[6]);
	tm->tm_mon  = bcd2bin(reply[7]) - 1;
	tm->tm_year = bcd2bin(reply[8]) + 100; /* Years since 1900 */

	return rtc_valid_tm(tm);
}

static const struct rtc_class_ops wt69p3_rtc_ops = {
	.read_time = wt69p3_rtc_read_time,
};

/* --- HWMON Temperature Sensor --- */
static umode_t wt69p3_hwmon_is_visible(const void *data,
					 enum hwmon_sensor_types type,
					 u32 attr, int channel)
{
	if (type == hwmon_temp && attr == hwmon_temp_input)
		return 0444;
	return 0;
}

static int wt69p3_hwmon_read(struct device *dev, enum hwmon_sensor_types type,
			       u32 attr, int channel, long *val)
{
	struct wt69p3 *mcu = dev_get_drvdata(dev);
	static const u8 temp_cmd[WT69P3_CMD_LEN] = {
		0xfa, 0x03, 0x08, 0x00, 0x00, 0x00, 0xfb
	};
	u8 reply[WT69P3_MAX_RX];
	int ret;

	if (type != hwmon_temp || attr != hwmon_temp_input)
		return -EOPNOTSUPP;

	ret = wt69p3_query_cmd(mcu, temp_cmd, sizeof(temp_cmd),
				 reply, sizeof(reply));
	if (ret < WT69P3_CMD_LEN)
		return ret < 0 ? ret : -EIO;

	dev_dbg(dev, "Temp raw reply (%d bytes): %*ph\n", ret, ret, reply);

	/* Temperature in Celsius is typically in reply[3] or reply[4] */
	*val = (long)reply[3] * 1000;
	return 0;
}

static const struct hwmon_channel_info * const wt69p3_hwmon_info[] = {
	HWMON_CHANNEL_INFO(temp, HWMON_T_INPUT),
	NULL
};

static const struct hwmon_ops wt69p3_hwmon_ops = {
	.is_visible = wt69p3_hwmon_is_visible,
	.read = wt69p3_hwmon_read,
};

static const struct hwmon_chip_info wt69p3_chip_info = {
	.ops = &wt69p3_hwmon_ops,
	.info = wt69p3_hwmon_info,
};

/* --- GPIO Controller (3 Lines: 0=Power LED, 1=Shutdown, 2=WOL) --- */
static int wt69p3_gpio_get_direction(struct gpio_chip *gc, unsigned int offset)
{
	return GPIO_LINE_DIRECTION_OUT;
}

static int wt69p3_gpio_direction_output(struct gpio_chip *gc,
					  unsigned int offset, int value)
{
	gc->set(gc, offset, value);
	return 0;
}

static int wt69p3_gpio_get(struct gpio_chip *gc, unsigned int offset)
{
	struct wt69p3 *mcu = gpiochip_get_data(gc);

	return !!(mcu->gpio_state & BIT(offset));
}

static int wt69p3_gpio_set(struct gpio_chip *gc, unsigned int offset, int value)
{
	struct wt69p3 *mcu = gpiochip_get_data(gc);
	u8 cmd[WT69P3_CMD_LEN] = { 0xfa, 0x03, 0x00, 0x00, 0x00, 0x00, 0xfb };

	if (value)
		mcu->gpio_state |= BIT(offset);
	else
		mcu->gpio_state &= ~BIT(offset);

	switch (offset) {
	case 0: /* Power LED On/Off */
		cmd[2] = 0x06;
		cmd[3] = value ? 0x01 : 0x00;
		cmd[5] = 0x01;
		break;
	case 1: /* Wake-on-LAN Enable/Disable */
		cmd[2] = 0x0a;
		cmd[3] = value ? 0x01 : 0x00;
		break;
	default:
		return -EINVAL;
	}

	return wt69p3_send_cmd(mcu, cmd, sizeof(cmd));
}

static int wt69p3_power_off_prepare(struct sys_off_data *data)
{
	struct wt69p3 *mcu = data->cb_data;
	static const u8 cmd[WT69P3_CMD_LEN] = {
		0xfa, 0x03, 0x03, 0x01, 0x01, 0x14, 0xfb
	};
	int ret;

	ret = wt69p3_send_cmd(mcu, cmd, sizeof(cmd));
	if (ret) {
		dev_err(&mcu->serdev->dev,
			"Failed to send power-off command: %d\n", ret);
		return notifier_from_errno(ret);
	}

	return NOTIFY_DONE;
}

static int wt69p3_probe(struct serdev_device *serdev)
{
	struct wt69p3 *mcu;
	struct device *hwmon_dev;
	static const u8 ready_cmd[WT69P3_CMD_LEN] = {
		0xfa, 0x03, 0x01, 0x00, 0x00, 0x00, 0xfb
	};
	int ret;

	mcu = devm_kzalloc(&serdev->dev, sizeof(*mcu), GFP_KERNEL);
	if (!mcu)
		return -ENOMEM;

	mcu->serdev = serdev;
	mutex_init(&mcu->lock);
	init_completion(&mcu->rx_done);
	serdev_device_set_drvdata(serdev, mcu);
	serdev_device_set_client_ops(serdev, &wt69p3_serdev_ops);

	ret = serdev_device_open(serdev);
	if (ret)
		return ret;

	serdev_device_set_baudrate(serdev, 115200);
	serdev_device_set_flow_control(serdev, false);
	serdev_device_set_parity(serdev, SERDEV_PARITY_NONE);

	/* Signal "OS load done" */
	wt69p3_send_cmd(mcu, ready_cmd, sizeof(ready_cmd));

	/* Register GPIO controller */
	mcu->gc.label = "weltrend,wt69p3";
	mcu->gc.parent = &serdev->dev;
	mcu->gc.owner = THIS_MODULE;
	mcu->gc.get_direction = wt69p3_gpio_get_direction;
	mcu->gc.direction_output = wt69p3_gpio_direction_output;
	mcu->gc.get = wt69p3_gpio_get;
	mcu->gc.set = wt69p3_gpio_set;
	mcu->gc.base = -1;
	mcu->gc.ngpio = 3;
	mcu->gc.can_sleep = true;

	ret = devm_gpiochip_add_data(&serdev->dev, &mcu->gc, mcu);
	if (ret)
		return ret;

	/* Register HWMON temperature sensor */
	hwmon_dev = devm_hwmon_device_register_with_info(&serdev->dev,
							 "wt69p3",
							 mcu,
							 &wt69p3_chip_info,
							 NULL);
	if (IS_ERR(hwmon_dev))
		return PTR_ERR(hwmon_dev);

	/* Register RTC device (/dev/rtc0) */
	mcu->rtc = devm_rtc_allocate_device(&serdev->dev);
	if (IS_ERR(mcu->rtc))
		return PTR_ERR(mcu->rtc);

	mcu->rtc->ops = &wt69p3_rtc_ops;
	mcu->rtc->range_min = RTC_TIMESTAMP_BEGIN_2000;
	mcu->rtc->range_max = RTC_TIMESTAMP_END_2099;
	
	/* Register the power-off handler */
	ret = devm_register_sys_off_handler(&serdev->dev, 
			SYS_OFF_MODE_POWER_OFF_PREPARE,
			SYS_OFF_PRIO_DEFAULT,
			wt69p3_power_off_prepare,
			mcu);
	if (ret)
		return dev_err_probe(&serdev->dev, ret, 
				"Failed to register power-off handler\n");

	return devm_rtc_register_device(mcu->rtc);
}

static void wt69p3_shutdown(struct serdev_device *serdev)
{
	struct wt69p3 *mcu = serdev_device_get_drvdata(serdev);
	static const u8 blink_cmd[WT69P3_CMD_LEN] = {
		0xfa, 0x03, 0x06, 0x02, 0x00, 0x01, 0xfb
	};

	/* Set Power LED to hardware blink during reboot/poweroff */
	wt69p3_send_cmd(mcu, blink_cmd, sizeof(blink_cmd));
}

static const struct of_device_id wt69p3_of_match[] = {
	{ .compatible = "weltrend,wt69p3" },
	{ }
};
MODULE_DEVICE_TABLE(of, wt69p3_of_match);

static struct serdev_device_driver wt69p3_driver = {
	.probe = wt69p3_probe,
	.shutdown = wt69p3_shutdown,
	.driver = {
		.name = "wt69p3",
		.of_match_table = wt69p3_of_match,
	},
};
module_serdev_device_driver(wt69p3_driver);

MODULE_AUTHOR("Lutz Müller <github@topfrose.de>");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Weltrend Control MCU serdev GPIO, hwmon and RTC driver");

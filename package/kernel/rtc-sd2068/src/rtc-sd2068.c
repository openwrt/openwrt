/*
 * RTC client/driver for the Whwave SD2068 Real-Time Clock over I2C
 *
 * Copyright (C) 2013 Tang, Haifeng <tanghaifeng-gz@loongson.cn>.
 *
 * base on ds3232
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 */
/*
 * It would be more efficient to use i2c msgs/i2c_transfer directly but, as
 * recommened in .../Documentation/i2c/writing-clients section
 * "Sending and receiving", using SMBus level communication is preferred.
 */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/interrupt.h>
#include <linux/i2c.h>
#include <linux/rtc.h>
#include <linux/bcd.h>
#include <linux/workqueue.h>
#include <linux/slab.h>

#define SD2068_SECONDS	0x00
#define SD2068_MINUTES	0x01
#define SD2068_HOURS	0x02
#define SD2068_AMPM		0x02
#define SD2068_DAY		0x03
#define SD2068_DATE		0x04
#define SD2068_MONTH	0x05
#define SD2068_YEAR		0x06

#define SD2068_ALARM	0x07	/* Alarm 1 BASE */

#define SD2068_ALARM_EN	0x0E
#       define SD2068_ALARM_EAY		0x40
#       define SD2068_ALARM_EAMO	0x20
#       define SD2068_ALARM_EAD		0x10
#       define SD2068_ALARM_EAW		0x08
#       define SD2068_ALARM_EAH		0x04
#       define SD2068_ALARM_EAMN	0x02
#       define SD2068_ALARM_EAS		0x01

#define SD2068_CTR1		0x0F	/* Control register 1 */
#       define SD2068_CTR1_WRTC3	0x80
#       define SD2068_CTR1_INTAF	0x20
#       define SD2068_CTR1_INTDF	0x10
#       define SD2068_CTR1_WRTC2	0x04
#       define SD2068_CTR1_RTCF		0x01

#define SD2068_CTR2		0x10
#       define SD2068_CTR2_WRTC1	0x80
#       define SD2068_CTR2_IM		0x40
#       define SD2068_CTR2_INTS1	0x20
#       define SD2068_CTR2_INTS0	0x10
#       define SD2068_CTR2_FOBAT	0x08
#       define SD2068_CTR2_INTDE	0x04
#       define SD2068_CTR2_INTAE	0x02
#       define SD2068_CTR2_INTFE	0x01

#define SD2068_CTR3		0x11
#       define SD2068_CTR3_ARST		0x80
#       define SD2068_CTR3_TDS1		0x20
#       define SD2068_CTR3_TDS0		0x10
#       define SD2068_CTR3_FS3		0x08
#       define SD2068_CTR3_FS2		0x04
#       define SD2068_CTR3_FS1		0x02
#       define SD2068_CTR3_FS0		0x01

#define SD2068_TIME_ADJ		0x12

struct sd2068 {
	struct i2c_client *client;
	struct rtc_device *rtc;
	struct work_struct work;

	/* The mutex protects alarm operations, and prevents a race
	 * between the enable_irq() in the workqueue and the free_irq()
	 * in the remove function.
	 */
	struct mutex mutex;
	int exiting;
};

static void sd2068_write_enable(struct i2c_client *client)
{
	char ret;

	ret = i2c_smbus_read_byte_data(client, SD2068_CTR2);
	ret = ret | SD2068_CTR2_WRTC1;
	i2c_smbus_write_byte_data(client, SD2068_CTR2, ret);

	ret = i2c_smbus_read_byte_data(client, SD2068_CTR1);
	ret = ret | SD2068_CTR1_WRTC3 | SD2068_CTR1_WRTC2;
	i2c_smbus_write_byte_data(client, SD2068_CTR1, ret);
}

static void sd2068_write_disable(struct i2c_client *client)
{
	char ret;

	ret = i2c_smbus_read_byte_data(client, SD2068_CTR1);
	ret = ret & (~SD2068_CTR1_WRTC3) & (~SD2068_CTR1_WRTC2);
	i2c_smbus_write_byte_data(client, SD2068_CTR1, ret);

	ret = i2c_smbus_read_byte_data(client, SD2068_CTR2);
	ret = ret & (~SD2068_CTR2_WRTC1);
	i2c_smbus_write_byte_data(client, SD2068_CTR2, ret);
}

static void sd2068_hw_init(struct i2c_client *client)
{
	char ret;

	sd2068_write_enable(client);

	ret = i2c_smbus_read_byte_data(client, SD2068_CTR2);
	ret = ret & (~SD2068_CTR2_IM);	/* 只使用单事件报警 */
	ret = ret & ((~SD2068_CTR2_INTS1) | SD2068_CTR2_INTS0);
	ret = ret & (~SD2068_CTR2_FOBAT);
	ret = ret & (((~SD2068_CTR2_INTDE) | SD2068_CTR2_INTAE) & (~SD2068_CTR2_INTFE));
	i2c_smbus_write_byte_data(client, SD2068_CTR2, ret);

	ret = i2c_smbus_read_byte_data(client, SD2068_CTR3);
	ret = ret & (~SD2068_CTR3_ARST);
	i2c_smbus_write_byte_data(client, SD2068_CTR3, ret);

	sd2068_write_disable(client);
}

static int sd2068_read_time(struct device *dev, struct rtc_time *time)
{
	struct i2c_client *client = to_i2c_client(dev);
	int ret;
	u8 buf[7];
	unsigned char year, month, day, hour, minute, second;
	unsigned char week, twelve_hr, am_pm;

	ret = i2c_smbus_read_i2c_block_data(client, SD2068_SECONDS, 7, buf);
	if (ret < 0)
		return ret;
	if (ret < 7)
		return -EIO;

	second = buf[0];
	minute = buf[1];
	hour = buf[2];
	week = buf[3];
	day = buf[4];
	month = buf[5];
	year = buf[6];

	/* Extract additional information for AM/PM */
	twelve_hr = hour & 0x80;
	am_pm = hour & 0x20;

	/* Write to rtc_time structure */
	time->tm_sec = bcd2bin(second & 0x7f);
	time->tm_min = bcd2bin(minute & 0x7f);
	if (twelve_hr) {
		time->tm_hour = bcd2bin(hour & 0x3f);
	} else {
		/* Convert to 24 hr */
		if (am_pm)
			time->tm_hour = bcd2bin(hour & 0x1f) + 12;
		else
			time->tm_hour = bcd2bin(hour & 0x1f);
	}

	time->tm_wday = bcd2bin(week & 0x07);
	time->tm_mday = bcd2bin(day & 0x3f);
	/* linux tm_mon range:0~11, while month range is 1~12 in RTC chip */
	time->tm_mon = bcd2bin(month & 0x7F) - 1;
	time->tm_year = bcd2bin(year);
	if (time->tm_year < 70)
		time->tm_year += 100;

	dev_dbg(dev, "%s secs=%d, mins=%d, "
		"hours=%d, mday=%d, mon=%d, year=%d, wday=%d\n",
		"read", time->tm_sec, time->tm_min,
		time->tm_hour, time->tm_mday,
		time->tm_mon, time->tm_year, time->tm_wday);

	return rtc_valid_tm(time);
}

static int sd2068_set_time(struct device *dev, struct rtc_time *time)
{
	struct i2c_client *client = to_i2c_client(dev);
	u8 buf[7];

	/* Extract time from rtc_time and load into sd2068*/
	sd2068_write_enable(client);

	buf[0] = bin2bcd(time->tm_sec);
	buf[1] = bin2bcd(time->tm_min);
	buf[2] = bin2bcd(time->tm_hour) | 0x80; /* only 24 hr? */
	buf[3] = bin2bcd(time->tm_wday);
	buf[4] = bin2bcd(time->tm_mday); /* Date */
	/* linux tm_mon range:0~11, while month range is 1~12 in RTC chip */
	buf[5] = bin2bcd(time->tm_mon + 1);
	buf[6] = bin2bcd(time->tm_year % 100);

	i2c_smbus_write_i2c_block_data(client, SD2068_SECONDS, 7, buf);
	i2c_smbus_write_byte_data(client, SD2068_TIME_ADJ, 0x00);
	
	sd2068_write_disable(client);

	return 0;
}

/*
 * According to linux specification, only support one-shot alarm
 * no periodic alarm mode
 */
static int sd2068_read_alarm(struct device *dev, struct rtc_wkalrm *alarm)
{
	struct i2c_client *client = to_i2c_client(dev);
	struct sd2068 *sd2068 = i2c_get_clientdata(client);
	unsigned char control, alm_en;
	unsigned char ret;
	u8 buf[7];

	mutex_lock(&sd2068->mutex);

	ret = i2c_smbus_read_byte_data(client, SD2068_CTR2);
	if (ret < 0)
		goto out;
	control = ret;
	alarm->enabled = (control & SD2068_CTR2_INTAE) ? 1 : 0;

	ret = i2c_smbus_read_i2c_block_data(client, SD2068_ALARM, 7, buf);
	if (ret < 0)
		goto out;

	ret = i2c_smbus_read_byte_data(client, SD2068_ALARM_EN);
	if (ret < 0)
		goto out;
	alm_en = ret;
	/* decode the alarm enable field */
	if (alm_en & SD2068_ALARM_EAS)
		alarm->time.tm_sec = bcd2bin(buf[0] & 0x7F);
	else
		alarm->time.tm_sec = -1;
	
	if (alm_en & SD2068_ALARM_EAMN)
		alarm->time.tm_min = bcd2bin(buf[1] & 0x7F);
	else
		alarm->time.tm_min = -1;

	if (alm_en & SD2068_ALARM_EAH)
		alarm->time.tm_hour = bcd2bin(buf[2] & 0x3F);
	else
		alarm->time.tm_hour = -1;
	
	if (alm_en & SD2068_ALARM_EAW)
		alarm->time.tm_wday = bcd2bin(buf[3] & 0x7F);
	else
		alarm->time.tm_wday = -1;
	
	if (alm_en & SD2068_ALARM_EAD)
		alarm->time.tm_mday = bcd2bin(buf[4] & 0x3F);
	else
		alarm->time.tm_mday = -1;
	
	if (alm_en & SD2068_ALARM_EAMO)
		alarm->time.tm_mon = bcd2bin(buf[5] & 0x1F);
	else
		alarm->time.tm_mon = -1;
	
	if (alm_en & SD2068_ALARM_EAY)
		alarm->time.tm_year = bcd2bin(buf[6]);
	else
		alarm->time.tm_year = -1;

	ret = 0;
out:
	mutex_unlock(&sd2068->mutex);
	return ret;
}

/*
 * linux rtc-module does not support wday alarm
 * and only 24h time mode supported indeed
 */
static int sd2068_set_alarm(struct device *dev, struct rtc_wkalrm *alarm)
{
	struct i2c_client *client = to_i2c_client(dev);
	struct sd2068 *sd2068 = i2c_get_clientdata(client);
	int control;
	int ret;
	u8 buf[7];

	if (client->irq <= 0)
		return -EINVAL;

	mutex_lock(&sd2068->mutex);

	sd2068_write_enable(client);

	buf[0] = bin2bcd(alarm->time.tm_sec);
	buf[1] = bin2bcd(alarm->time.tm_min);
	buf[2] = bin2bcd(alarm->time.tm_hour);
	buf[3] = bin2bcd(alarm->time.tm_wday);
	buf[4] = bin2bcd(alarm->time.tm_mday);
	buf[5] = bin2bcd(alarm->time.tm_mon);
	buf[6] = bin2bcd(alarm->time.tm_year);

	/* clear alarm interrupt enable bit */
	ret = i2c_smbus_read_byte_data(client, SD2068_CTR2);
	if (ret < 0)
		goto out;
	control = ret;
	control &= ~(SD2068_CTR2_INTAE);
	ret = i2c_smbus_write_byte_data(client, SD2068_CTR2, control);
	if (ret < 0)
		goto out;

	ret = i2c_smbus_write_i2c_block_data(client, SD2068_ALARM, 7, buf);

	ret = i2c_smbus_write_byte_data(client, SD2068_ALARM_EN, 0x7f);

	if (alarm->enabled) {
		control |= SD2068_CTR2_INTAE;
		ret = i2c_smbus_write_byte_data(client, SD2068_CTR2, control);
	}
out:
	sd2068_write_disable(client);
	mutex_unlock(&sd2068->mutex);
	return ret;
}

static int sd2068_alarm_irq_enable(struct device *dev, unsigned int enabled)
{
	struct i2c_client *client = to_i2c_client(dev);
	struct sd2068 *sd2068 = i2c_get_clientdata(client);
	unsigned char control;

	pr_debug("%s: aie=%d\n", __func__, enabled);

	if (client->irq <= 0)
		return -EINVAL;

	sd2068_write_enable(client);

	control = i2c_smbus_read_byte_data(client, SD2068_CTR2);

	if (enabled) {
		sd2068->rtc->irq_data |= RTC_AF;
		control |= SD2068_CTR2_INTAE;
		i2c_smbus_write_byte_data(client, SD2068_CTR2, control);
	} else {
		sd2068->rtc->irq_data &= ~RTC_AF;
		control &= ~SD2068_CTR2_INTAE;
		i2c_smbus_write_byte_data(client, SD2068_CTR2, control);
	}

	sd2068_write_disable(client);

	return 0;
}

static irqreturn_t sd2068_irq(int irq, void *dev_id)
{
	struct i2c_client *client = dev_id;
	struct sd2068 *sd2068 = i2c_get_clientdata(client);

	disable_irq_nosync(irq);
	schedule_work(&sd2068->work);
	return IRQ_HANDLED;
}

static void sd2068_work(struct work_struct *work)
{
	struct sd2068 *sd2068 = container_of(work, struct sd2068, work);
	struct i2c_client *client = sd2068->client;

	mutex_lock(&sd2068->mutex);

	rtc_update_irq(sd2068->rtc, 1, RTC_AF | RTC_IRQF);

	if (!sd2068->exiting)
		enable_irq(client->irq);

	mutex_unlock(&sd2068->mutex);
}

static const struct rtc_class_ops sd2068_rtc_ops = {
	.read_time = sd2068_read_time,
	.set_time = sd2068_set_time,
	.read_alarm = sd2068_read_alarm,
	.set_alarm = sd2068_set_alarm,
	.alarm_irq_enable = sd2068_alarm_irq_enable,
};

static int sd2068_probe(struct i2c_client *client,
		const struct i2c_device_id *id)
{
	struct sd2068 *sd2068;
	struct rtc_time rtc_tm;
	int ret;

	sd2068 = kzalloc(sizeof(struct sd2068), GFP_KERNEL);
	if (!sd2068)
		return -ENOMEM;

	sd2068->client = client;
	i2c_set_clientdata(client, sd2068);

	INIT_WORK(&sd2068->work, sd2068_work);
	mutex_init(&sd2068->mutex);

	sd2068->rtc = rtc_device_register(client->name, &client->dev,
					  &sd2068_rtc_ops, THIS_MODULE);
	if (IS_ERR(sd2068->rtc)) {
		ret = PTR_ERR(sd2068->rtc);
		dev_err(&client->dev, "unable to register the class device\n");
		goto out_irq;
	}

	if (client->irq >= 0) {
		ret = request_irq(client->irq, sd2068_irq, 0,
				 "sd2068", client);
		if (ret) {
			dev_err(&client->dev, "unable to request IRQ\n");
			goto out_free;
		}
	}

	sd2068_hw_init(client);

	/* Check RTC Time */
	sd2068_read_time(&client->dev, &rtc_tm);

	if (rtc_valid_tm(&rtc_tm)) {
		rtc_tm.tm_year	= 100;
		rtc_tm.tm_mon	= 0;
		rtc_tm.tm_mday	= 1;
		rtc_tm.tm_hour	= 0;
		rtc_tm.tm_min	= 0;
		rtc_tm.tm_sec	= 0;

		sd2068_set_time(&client->dev, &rtc_tm);

		dev_warn(&client->dev, "warning: invalid RTC value so initializing it\n");
	}

	return 0;

out_irq:
	if (client->irq >= 0)
		free_irq(client->irq, client);

out_free:
	kfree(sd2068);
	return ret;
}

static int sd2068_remove(struct i2c_client *client)
{
	struct sd2068 *sd2068 = i2c_get_clientdata(client);

	if (client->irq >= 0) {
		mutex_lock(&sd2068->mutex);
		sd2068->exiting = 1;
		mutex_unlock(&sd2068->mutex);

		free_irq(client->irq, client);
		cancel_work_sync(&sd2068->work);
	}

	rtc_device_unregister(sd2068->rtc);
	kfree(sd2068);
	return 0;
}

static const struct i2c_device_id sd2068_id[] = {
	{ "sd2068", 0 },
	{ }
};
MODULE_DEVICE_TABLE(i2c, sd2068_id);

static struct i2c_driver sd2068_driver = {
	.driver = {
		.name = "rtc-sd2068",
		.owner = THIS_MODULE,
	},
	.probe = sd2068_probe,
	.remove = sd2068_remove,
	.id_table = sd2068_id,
};

static int __init sd2068_init(void)
{
	return i2c_add_driver(&sd2068_driver);
}

static void __exit sd2068_exit(void)
{
	i2c_del_driver(&sd2068_driver);
}

module_init(sd2068_init);
module_exit(sd2068_exit);

MODULE_AUTHOR("Loongson-gz <tanghaifeng-gz@loongson.cn>");
MODULE_DESCRIPTION("Whwave SD2068 RTC Driver");
MODULE_LICENSE("GPL");

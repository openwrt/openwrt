// SPDX-License-Identifier: GPL-2.0-only
/*
 * GL.iNet GL-E750 (Mudi) status MCU driver
 *
 * The MCU sits on the SoC UART and drives the OLED screen and the battery
 * fuel gauge. It speaks text lines: one JSON object per line from the host,
 * where every value is a string. When a line contains "mcu_status": "1",
 * the MCU answers with
 *
 *	{OK},<percent>,<temperature>,<charging>,<cycles>
 *
 * The MCU only handles one line at a time, so this driver owns the cadence:
 * it periodically sends the latest screen payload written by userspace to
 * /dev/gl-e750-mcu, merged with the status request, and exposes the battery
 * through the power_supply class.
 *
 * Copyright (C) 2026 Serv Pol <servpol@w420.ru>
 */

#include <linux/completion.h>
#include <linux/ctype.h>
#include <linux/delay.h>
#include <linux/miscdevice.h>
#include <linux/module.h>
#include <linux/mod_devicetable.h>
#include <linux/mutex.h>
#include <linux/power_supply.h>
#include <linux/property.h>
#include <linux/reboot.h>
#include <linux/serdev.h>
#include <linux/slab.h>
#include <linux/spinlock.h>
#include <linux/string.h>
#include <linux/uaccess.h>
#include <linux/workqueue.h>

#define MCU_BAUDRATE		115200
#define MCU_RX_LINE_MAX		128
#define MCU_PAYLOAD_MAX		1024
#define MCU_TX_GAP_MS		1000
#define MCU_REPLY_TIMEOUT_MS	1500
#define MCU_STATUS_REQ		"\"mcu_status\": \"1\""

static unsigned int poll_interval = 30;
module_param(poll_interval, uint, 0644);
MODULE_PARM_DESC(poll_interval, "Seconds between status updates (default 30)");

struct gl_e750_mcu {
	struct serdev_device *serdev;
	struct miscdevice misc;
	struct power_supply *psy;
	struct delayed_work poll_work;
	struct work_struct poweroff_work;

	/* serializes transmissions and access to the payload */
	struct mutex tx_lock;
	char payload[MCU_PAYLOAD_MAX];
	size_t payload_len;
	unsigned long last_tx;

	/* receive side, filled from the serdev callback */
	spinlock_t rx_lock;
	char rx_line[MCU_RX_LINE_MAX];
	size_t rx_len;
	struct completion reply;

	/* battery state, protected by rx_lock */
	bool valid;
	int capacity;		/* percent */
	int temp;		/* tenths of a degree Celsius */
	bool charging;
	int cycles;
};

/* "37.4", "40" or "-1.5" -> tenths of a degree */
static int mcu_parse_tenths(const char *s, int *out)
{
	const char *dot = strchr(s, '.');
	char whole_str[8];
	int whole, frac = 0;

	if (!dot) {
		if (kstrtoint(s, 10, &whole))
			return -EINVAL;
		*out = whole * 10;
		return 0;
	}

	if (dot == s || dot - s >= sizeof(whole_str))
		return -EINVAL;
	memcpy(whole_str, s, dot - s);
	whole_str[dot - s] = '\0';
	if (kstrtoint(whole_str, 10, &whole))
		return -EINVAL;
	if (isdigit(dot[1]))
		frac = dot[1] - '0';
	*out = whole * 10 + (*s == '-' ? -frac : frac);
	return 0;
}

/*
 * "{OK},<percent>,<temperature>,<charging>,<cycles>", called with rx_lock
 * held. Returns true if the line was a valid status reply.
 */
static bool mcu_parse_status(struct gl_e750_mcu *mcu, char *line)
{
	char *p = line + 4, *f[4];
	int capacity, temp, charging, cycles, i;
	bool changed;

	if (*p++ != ',')
		return false;
	for (i = 0; i < ARRAY_SIZE(f); i++) {
		f[i] = strsep(&p, ",");
		if (!f[i] || !*f[i])
			return false;
	}

	if (kstrtoint(f[0], 10, &capacity) || mcu_parse_tenths(f[1], &temp) ||
	    kstrtoint(f[2], 10, &charging) || kstrtoint(f[3], 10, &cycles))
		return false;
	if (capacity < 0 || capacity > 100)
		return false;

	changed = !mcu->valid || mcu->capacity != capacity ||
		  mcu->charging != !!charging || mcu->cycles != cycles ||
		  abs(mcu->temp - temp) >= 10;
	mcu->capacity = capacity;
	mcu->temp = temp;
	mcu->charging = !!charging;
	mcu->cycles = cycles;
	mcu->valid = true;

	complete(&mcu->reply);
	if (changed && mcu->psy)
		power_supply_changed(mcu->psy);
	return true;
}

/* called with rx_lock held */
static void mcu_handle_line(struct gl_e750_mcu *mcu, char *line)
{
	if (!strncmp(line, "{OK}", 4)) {
		mcu_parse_status(mcu, line);
	} else if (strstr(line, "shut_down")) {
		/* the power switch was turned off, the MCU will cut power */
		schedule_work(&mcu->poweroff_work);
	} else {
		dev_info_ratelimited(&mcu->serdev->dev, "unhandled: %s\n", line);
	}
}

static size_t mcu_receive_buf(struct serdev_device *serdev, const u8 *data,
			      size_t count)
{
	struct gl_e750_mcu *mcu = serdev_device_get_drvdata(serdev);
	unsigned long flags;
	size_t i;

	spin_lock_irqsave(&mcu->rx_lock, flags);
	for (i = 0; i < count; i++) {
		char c = data[i];

		if (c == '\r' || c == '\n') {
			if (mcu->rx_len) {
				mcu->rx_line[mcu->rx_len] = '\0';
				mcu_handle_line(mcu, mcu->rx_line);
				mcu->rx_len = 0;
			}
			continue;
		}
		if (mcu->rx_len < MCU_RX_LINE_MAX - 1)
			mcu->rx_line[mcu->rx_len++] = c;

		/* JSON events from the MCU may come without a line ending */
		if (c == '}' && mcu->rx_line[0] == '{' &&
		    strncmp(mcu->rx_line, "{OK}", 4)) {
			mcu->rx_line[mcu->rx_len] = '\0';
			mcu_handle_line(mcu, mcu->rx_line);
			mcu->rx_len = 0;
		}
	}
	spin_unlock_irqrestore(&mcu->rx_lock, flags);

	return count;
}

static const struct serdev_device_ops mcu_serdev_ops = {
	.receive_buf = mcu_receive_buf,
	.write_wakeup = serdev_device_write_wakeup,
};

/* send one line: the latest payload merged with the status request */
static void mcu_send_update(struct gl_e750_mcu *mcu)
{
	char *buf;
	size_t len = 0;
	unsigned long next, flags;
	int ret;

	buf = kmalloc(MCU_PAYLOAD_MAX + sizeof(MCU_STATUS_REQ) + 8, GFP_KERNEL);
	if (!buf)
		return;

	mutex_lock(&mcu->tx_lock);

	/* "{ ... }" -> "{ ..., "mcu_status": "1" }" */
	len = mcu->payload_len ? mcu->payload_len - 1 : 0;
	memcpy(buf, mcu->payload, len);
	while (len && isspace(buf[len - 1]))
		len--;
	if (len <= 1)
		len = sprintf(buf, "{ " MCU_STATUS_REQ " }");
	else
		len += sprintf(buf + len, ", " MCU_STATUS_REQ " }");
	buf[len++] = '\n';

	next = mcu->last_tx + msecs_to_jiffies(MCU_TX_GAP_MS);
	if (mcu->last_tx && time_before(jiffies, next))
		msleep(jiffies_to_msecs(next - jiffies));

	spin_lock_irqsave(&mcu->rx_lock, flags);
	mcu->rx_len = 0;
	reinit_completion(&mcu->reply);
	spin_unlock_irqrestore(&mcu->rx_lock, flags);

	ret = serdev_device_write(mcu->serdev, (const u8 *)buf, len,
				  msecs_to_jiffies(MCU_REPLY_TIMEOUT_MS));
	mcu->last_tx = jiffies;
	if (ret < 0) {
		dev_warn_ratelimited(&mcu->serdev->dev, "write failed: %d\n", ret);
	} else if (!wait_for_completion_timeout(&mcu->reply,
					      msecs_to_jiffies(MCU_REPLY_TIMEOUT_MS))) {
		/* the reply may come without a line ending */
		spin_lock_irqsave(&mcu->rx_lock, flags);
		mcu->rx_line[mcu->rx_len] = '\0';
		if (!(mcu->rx_len > 4 && !strncmp(mcu->rx_line, "{OK}", 4) &&
		      mcu_parse_status(mcu, mcu->rx_line)))
			dev_dbg(&mcu->serdev->dev, "no status reply\n");
		mcu->rx_len = 0;
		spin_unlock_irqrestore(&mcu->rx_lock, flags);
	}

	mutex_unlock(&mcu->tx_lock);
	kfree(buf);
}

static void mcu_poll_work(struct work_struct *work)
{
	struct gl_e750_mcu *mcu = container_of(to_delayed_work(work),
					       struct gl_e750_mcu, poll_work);

	mcu_send_update(mcu);
	schedule_delayed_work(&mcu->poll_work,
			      msecs_to_jiffies(max(poll_interval, 5U) * 1000));
}

static void mcu_poweroff_work(struct work_struct *work)
{
	struct gl_e750_mcu *mcu = container_of(work, struct gl_e750_mcu,
					       poweroff_work);

	/* the MCU cuts the power a few seconds after this message */
	dev_info(&mcu->serdev->dev, "power switch off, shutting down\n");
	orderly_poweroff(true);
}

/* userspace writes one JSON object for the screen */
static ssize_t mcu_misc_write(struct file *file, const char __user *ubuf,
			      size_t count, loff_t *ppos)
{
	struct gl_e750_mcu *mcu = container_of(file->private_data,
					       struct gl_e750_mcu, misc);
	char *buf, *start, *end;
	size_t len, i;

	if (!count || count >= MCU_PAYLOAD_MAX)
		return -EINVAL;

	buf = memdup_user_nul(ubuf, count);
	if (IS_ERR(buf))
		return PTR_ERR(buf);

	start = strim(buf);
	len = strlen(start);
	end = start + len - 1;
	if (len < 2 || *start != '{' || *end != '}') {
		kfree(buf);
		return -EINVAL;
	}
	for (i = 0; i < len; i++) {
		if (start[i] < 0x20) {
			kfree(buf);
			return -EINVAL;
		}
	}

	mutex_lock(&mcu->tx_lock);
	memcpy(mcu->payload, start, len);
	mcu->payload_len = len;
	mutex_unlock(&mcu->tx_lock);
	kfree(buf);

	mod_delayed_work(system_wq, &mcu->poll_work, 0);
	return count;
}

static const struct file_operations mcu_misc_fops = {
	.owner = THIS_MODULE,
	.write = mcu_misc_write,
	.llseek = noop_llseek,
};

static enum power_supply_property mcu_battery_props[] = {
	POWER_SUPPLY_PROP_PRESENT,
	POWER_SUPPLY_PROP_STATUS,
	POWER_SUPPLY_PROP_TECHNOLOGY,
	POWER_SUPPLY_PROP_CAPACITY,
	POWER_SUPPLY_PROP_TEMP,
	POWER_SUPPLY_PROP_CYCLE_COUNT,
};

static int mcu_battery_get_property(struct power_supply *psy,
				    enum power_supply_property psp,
				    union power_supply_propval *val)
{
	struct gl_e750_mcu *mcu = power_supply_get_drvdata(psy);
	unsigned long flags;
	int ret = 0;

	spin_lock_irqsave(&mcu->rx_lock, flags);
	switch (psp) {
	case POWER_SUPPLY_PROP_PRESENT:
		val->intval = 1;
		break;
	case POWER_SUPPLY_PROP_TECHNOLOGY:
		val->intval = POWER_SUPPLY_TECHNOLOGY_LION;
		break;
	case POWER_SUPPLY_PROP_STATUS:
		if (!mcu->valid)
			val->intval = POWER_SUPPLY_STATUS_UNKNOWN;
		else if (mcu->charging)
			val->intval = mcu->capacity >= 100 ?
				      POWER_SUPPLY_STATUS_FULL :
				      POWER_SUPPLY_STATUS_CHARGING;
		else
			val->intval = POWER_SUPPLY_STATUS_DISCHARGING;
		break;
	case POWER_SUPPLY_PROP_CAPACITY:
		if (mcu->valid)
			val->intval = mcu->capacity;
		else
			ret = -ENODATA;
		break;
	case POWER_SUPPLY_PROP_TEMP:
		if (mcu->valid)
			val->intval = mcu->temp;
		else
			ret = -ENODATA;
		break;
	case POWER_SUPPLY_PROP_CYCLE_COUNT:
		if (mcu->valid)
			val->intval = mcu->cycles;
		else
			ret = -ENODATA;
		break;
	default:
		ret = -EINVAL;
	}
	spin_unlock_irqrestore(&mcu->rx_lock, flags);

	return ret;
}

static const struct power_supply_desc mcu_battery_desc = {
	.name = "gl-e750-battery",
	.type = POWER_SUPPLY_TYPE_BATTERY,
	.properties = mcu_battery_props,
	.num_properties = ARRAY_SIZE(mcu_battery_props),
	.get_property = mcu_battery_get_property,
};

static int gl_e750_mcu_probe(struct serdev_device *serdev)
{
	struct device *dev = &serdev->dev;
	struct power_supply_config psy_cfg = {};
	struct gl_e750_mcu *mcu;
	int ret;

	mcu = devm_kzalloc(dev, sizeof(*mcu), GFP_KERNEL);
	if (!mcu)
		return -ENOMEM;

	mcu->serdev = serdev;
	mutex_init(&mcu->tx_lock);
	spin_lock_init(&mcu->rx_lock);
	init_completion(&mcu->reply);
	INIT_DELAYED_WORK(&mcu->poll_work, mcu_poll_work);
	INIT_WORK(&mcu->poweroff_work, mcu_poweroff_work);

	serdev_device_set_drvdata(serdev, mcu);
	serdev_device_set_client_ops(serdev, &mcu_serdev_ops);

	ret = devm_serdev_device_open(dev, serdev);
	if (ret)
		return dev_err_probe(dev, ret, "failed to open serdev\n");

	serdev_device_set_baudrate(serdev, MCU_BAUDRATE);
	serdev_device_set_flow_control(serdev, false);
	ret = serdev_device_set_parity(serdev, SERDEV_PARITY_NONE);
	if (ret)
		return dev_err_probe(dev, ret, "failed to set parity\n");

	psy_cfg.drv_data = mcu;
	psy_cfg.fwnode = dev_fwnode(dev);
	mcu->psy = devm_power_supply_register(dev, &mcu_battery_desc, &psy_cfg);
	if (IS_ERR(mcu->psy))
		return dev_err_probe(dev, PTR_ERR(mcu->psy),
				     "failed to register battery\n");

	mcu->misc.minor = MISC_DYNAMIC_MINOR;
	mcu->misc.name = "gl-e750-mcu";
	mcu->misc.fops = &mcu_misc_fops;
	mcu->misc.mode = 0600;
	ret = misc_register(&mcu->misc);
	if (ret)
		return dev_err_probe(dev, ret, "failed to register misc device\n");

	schedule_delayed_work(&mcu->poll_work, 0);
	return 0;
}

static void gl_e750_mcu_remove(struct serdev_device *serdev)
{
	struct gl_e750_mcu *mcu = serdev_device_get_drvdata(serdev);

	misc_deregister(&mcu->misc);
	cancel_delayed_work_sync(&mcu->poll_work);
	cancel_work_sync(&mcu->poweroff_work);
	mcu->psy = NULL;
}

static const struct of_device_id gl_e750_mcu_of_match[] = {
	{ .compatible = "glinet,gl-e750-mcu" },
	{ }
};
MODULE_DEVICE_TABLE(of, gl_e750_mcu_of_match);

static struct serdev_device_driver gl_e750_mcu_driver = {
	.probe = gl_e750_mcu_probe,
	.remove = gl_e750_mcu_remove,
	.driver = {
		.name = "gl-e750-mcu",
		.of_match_table = gl_e750_mcu_of_match,
	},
};
module_serdev_device_driver(gl_e750_mcu_driver);

MODULE_AUTHOR("Serv Pol <servpol@w420.ru>");
MODULE_DESCRIPTION("GL.iNet GL-E750 status MCU (OLED and battery) driver");
MODULE_LICENSE("GPL");

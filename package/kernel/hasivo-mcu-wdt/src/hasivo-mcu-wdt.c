// SPDX-License-Identifier: GPL-2.0-only
/*
 * Hasivo MCU Watchdog Driver
 *
 * Hardware watchdog driver for the external management MCU found on
 * Hasivo / Horaco network switches. Communicates over I2C.
 *
 * Protocol reverse-engineered from the stock firmware ("imi" daemon +
 * i2c-poe.ko kernel module):
 *   - Arm:       write 0x4F -> reg 0x09 (unlock), 0x4F -> reg 0x0A
 *   - Disarm:    write 0x4F -> reg 0x09 (unlock), 0x3F -> reg 0x0A
 *   - Keepalive: write 0xEE -> reg 0x0B (no unlock needed)
 *
 * Copyright (C) 2026 Manuel Stocker <mensi@mensi.ch>
 * Copyright (C) 2026 Carlo Szelinsky <github@szelinsky.de>
 */

#include <linux/i2c.h>
#include <linux/module.h>
#include <linux/watchdog.h>

#define HASIVO_REG_WDT_UNLOCK		0x09
  #define HASIVO_WDT_UNLOCK_MAGIC	0x4f

#define HASIVO_REG_WDT_CMD		0x0a
  #define HASIVO_WDT_CMD_ARM		0x4f
  #define HASIVO_WDT_CMD_DISARM		0x3f

#define HASIVO_REG_WDT_KEEPALIVE	0x0b
  #define HASIVO_WDT_KEEPALIVE_MAGIC	0xee

/*
 * Hardware timeout is fixed in MCU firmware and not software-
 * configurable. Stock keepalive cadence is 2 s; the actual reset
 * threshold is empirically in the 10-30 s range. Pick a conservative
 * value so the kernel-core auto-feed (which fires at timeout/2) leaves
 * generous slack.
 */
#define HASIVO_WDT_TIMEOUT		15

struct hasivo_mcu {
	struct i2c_client *client;
	struct watchdog_device wdd;
};

static int hasivo_mcu_wdt_send_cmd(struct hasivo_mcu *mcu, u8 cmd)
{
	int ret;

	ret = i2c_smbus_write_byte_data(mcu->client, HASIVO_REG_WDT_UNLOCK,
					HASIVO_WDT_UNLOCK_MAGIC);
	if (ret)
		return ret;

	return i2c_smbus_write_byte_data(mcu->client, HASIVO_REG_WDT_CMD, cmd);
}

static int hasivo_mcu_wdt_start(struct watchdog_device *wdd)
{
	struct hasivo_mcu *mcu = watchdog_get_drvdata(wdd);

	return hasivo_mcu_wdt_send_cmd(mcu, HASIVO_WDT_CMD_ARM);
}

static int hasivo_mcu_wdt_stop(struct watchdog_device *wdd)
{
	struct hasivo_mcu *mcu = watchdog_get_drvdata(wdd);

	return hasivo_mcu_wdt_send_cmd(mcu, HASIVO_WDT_CMD_DISARM);
}

static int hasivo_mcu_wdt_ping(struct watchdog_device *wdd)
{
	struct hasivo_mcu *mcu = watchdog_get_drvdata(wdd);

	return i2c_smbus_write_byte_data(mcu->client,
					 HASIVO_REG_WDT_KEEPALIVE,
					 HASIVO_WDT_KEEPALIVE_MAGIC);
}

static const struct watchdog_info hasivo_mcu_wdt_info = {
	.identity	= "Hasivo MCU Watchdog",
	.options	= WDIOF_KEEPALIVEPING | WDIOF_MAGICCLOSE,
};

static const struct watchdog_ops hasivo_mcu_wdt_ops = {
	.owner		= THIS_MODULE,
	.start		= hasivo_mcu_wdt_start,
	.stop		= hasivo_mcu_wdt_stop,
	.ping		= hasivo_mcu_wdt_ping,
};

static int hasivo_mcu_probe(struct i2c_client *client)
{
	struct device *dev = &client->dev;
	struct hasivo_mcu *mcu;
	int ret;

	mcu = devm_kzalloc(dev, sizeof(*mcu), GFP_KERNEL);
	if (!mcu)
		return -ENOMEM;

	mcu->client = client;

	/*
	 * Arm the watchdog. The stock bootloader normally leaves it
	 * already armed (and the chip's timer has been counting since
	 * then); re-arm explicitly so unexpected boot paths still leave
	 * it in a known state. It is not specified whether the arm
	 * sequence resets the chip's timeout counter, so send an
	 * explicit keepalive afterwards to guarantee a fresh window
	 * before the kernel-side feeder takes over at timeout/2.
	 */
	ret = hasivo_mcu_wdt_send_cmd(mcu, HASIVO_WDT_CMD_ARM);
	if (ret)
		return dev_err_probe(dev, ret, "failed to arm watchdog\n");

	ret = i2c_smbus_write_byte_data(client, HASIVO_REG_WDT_KEEPALIVE,
					HASIVO_WDT_KEEPALIVE_MAGIC);
	if (ret)
		return dev_err_probe(dev, ret,
				     "initial watchdog ping failed\n");

	mcu->wdd.info = &hasivo_mcu_wdt_info;
	mcu->wdd.ops = &hasivo_mcu_wdt_ops;
	mcu->wdd.parent = dev;
	mcu->wdd.timeout = HASIVO_WDT_TIMEOUT;
	mcu->wdd.min_timeout = HASIVO_WDT_TIMEOUT;
	mcu->wdd.max_timeout = HASIVO_WDT_TIMEOUT;

	/*
	 * Tell the watchdog core the hardware is already running so it
	 * pings via its own timer at timeout/2 until userspace (procd)
	 * opens /dev/watchdog0 and takes over.
	 */
	set_bit(WDOG_HW_RUNNING, &mcu->wdd.status);

	watchdog_set_drvdata(&mcu->wdd, mcu);
	watchdog_stop_on_unregister(&mcu->wdd);

	ret = devm_watchdog_register_device(dev, &mcu->wdd);
	if (ret)
		return dev_err_probe(dev, ret,
				     "failed to register watchdog\n");

	dev_info(dev, "Hasivo MCU watchdog armed at 0x%02x (timeout %us)\n",
		 client->addr, mcu->wdd.timeout);

	return 0;
}

static const struct of_device_id hasivo_mcu_of_match[] = {
	{ .compatible = "hasivo,mcu-wdt" },
	{ }
};
MODULE_DEVICE_TABLE(of, hasivo_mcu_of_match);

static const struct i2c_device_id hasivo_mcu_id[] = {
	{ "hasivo-mcu-wdt" },
	{ }
};
MODULE_DEVICE_TABLE(i2c, hasivo_mcu_id);

static struct i2c_driver hasivo_mcu_driver = {
	.driver = {
		.name		= "hasivo-mcu-wdt",
		.of_match_table	= hasivo_mcu_of_match,
	},
	.probe		= hasivo_mcu_probe,
	.id_table	= hasivo_mcu_id,
};
module_i2c_driver(hasivo_mcu_driver);

MODULE_DESCRIPTION("Hasivo MCU Watchdog Driver");
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Manuel Stocker <mensi@mensi.ch>");
MODULE_AUTHOR("Carlo Szelinsky <github@szelinsky.de>");

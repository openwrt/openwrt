// SPDX-License-Identifier: GPL-2.0-only
/*
 * Driver for the front LED bar microcontroller of the TP-Link Archer BE550 v1.
 *
 * The microcontroller (I2C address 0x50) drives 36 white LEDs through its own
 * LED driver chip and offers a set of canned effects. A command is a single
 * byte write; replies, where there are any, come from a separate read.
 *
 * The microcontroller also implements a firmware update protocol (commands
 * 0x30-0x32). This driver must never start it, so every write goes through
 * tplink_ledbar_send(), which only accepts the commands used below.
 */

#include <linux/delay.h>
#include <linux/i2c.h>
#include <linux/module.h>
#include <linux/mod_devicetable.h>
#include <linux/mutex.h>
#include <linux/string.h>
#include <linux/sysfs.h>

#define TPLINK_LEDBAR_CMD_VERSION	0x20
#define TPLINK_LEDBAR_CMD_MODE		0x21
#define TPLINK_LEDBAR_CMD_EFFECT(i)	(0xa0 + (i))

#define TPLINK_LEDBAR_MODE_APP		0x03
#define TPLINK_LEDBAR_NOT_READY		0xff
#define TPLINK_LEDBAR_VERSION_LEN	32

#define TPLINK_LEDBAR_EFFECT_OFF	0
#define TPLINK_LEDBAR_EFFECT_CHASE	1

/* effect i is command 0xa0 + i, in this order */
static const char * const tplink_ledbar_effects[] = {
	"off", "chase", "on", "partial", "partial-breathing",
	"breathing", "upgrade", "reset", "wps",
};

struct tplink_ledbar {
	struct i2c_client *client;
	struct mutex lock;
	int effect;
	char version[TPLINK_LEDBAR_VERSION_LEN - 1];
};

static bool tplink_ledbar_cmd_allowed(u8 cmd)
{
	return cmd == TPLINK_LEDBAR_CMD_VERSION ||
	       cmd == TPLINK_LEDBAR_CMD_MODE ||
	       (cmd >= TPLINK_LEDBAR_CMD_EFFECT(0) &&
		cmd < TPLINK_LEDBAR_CMD_EFFECT(ARRAY_SIZE(tplink_ledbar_effects)));
}

static int tplink_ledbar_send(struct tplink_ledbar *ledbar, u8 cmd)
{
	int ret;

	if (!tplink_ledbar_cmd_allowed(cmd))
		return -EINVAL;

	ret = i2c_master_send(ledbar->client, &cmd, 1);
	if (ret < 0)
		return ret;

	return ret == 1 ? 0 : -EIO;
}

/* Read a reply, retrying while the microcontroller reports "not ready". */
static int tplink_ledbar_recv(struct tplink_ledbar *ledbar, u8 *buf, int len)
{
	int i, ret;

	for (i = 1; i <= 3; i++) {
		ret = i2c_master_recv(ledbar->client, buf, len);
		if (ret < 0)
			return ret;
		if (ret != len)
			return -EIO;
		if (buf[0] != TPLINK_LEDBAR_NOT_READY)
			return 0;
		msleep(10 * i);
	}

	return -ETIMEDOUT;
}

static int tplink_ledbar_query(struct tplink_ledbar *ledbar, u8 cmd,
			       unsigned int delay_ms, u8 *buf, int len)
{
	int ret;

	ret = tplink_ledbar_send(ledbar, cmd);
	if (ret)
		return ret;

	msleep(delay_ms);

	return tplink_ledbar_recv(ledbar, buf, len);
}

/* CRC-16/XMODEM: polynomial 0x1021, initial value 0 */
static u16 tplink_ledbar_crc16(const u8 *buf, int len)
{
	u16 crc = 0;
	int i;

	while (len--) {
		crc ^= *buf++ << 8;
		for (i = 0; i < 8; i++)
			crc = crc & 0x8000 ? (crc << 1) ^ 0x1021 : crc << 1;
	}

	return crc;
}

static ssize_t effect_show(struct device *dev, struct device_attribute *attr,
			   char *buf)
{
	struct tplink_ledbar *ledbar = dev_get_drvdata(dev);

	return sysfs_emit(buf, "%s\n", tplink_ledbar_effects[ledbar->effect]);
}

static ssize_t effect_store(struct device *dev, struct device_attribute *attr,
			    const char *buf, size_t count)
{
	struct tplink_ledbar *ledbar = dev_get_drvdata(dev);
	int effect, ret;

	effect = sysfs_match_string(tplink_ledbar_effects, buf);
	if (effect < 0)
		return effect;

	mutex_lock(&ledbar->lock);
	ret = tplink_ledbar_send(ledbar, TPLINK_LEDBAR_CMD_EFFECT(effect));
	if (!ret)
		ledbar->effect = effect;
	mutex_unlock(&ledbar->lock);

	return ret ? ret : count;
}
static DEVICE_ATTR_RW(effect);

static ssize_t version_show(struct device *dev, struct device_attribute *attr,
			    char *buf)
{
	struct tplink_ledbar *ledbar = dev_get_drvdata(dev);

	return sysfs_emit(buf, "%s\n", ledbar->version);
}
static DEVICE_ATTR_RO(version);

static struct attribute *tplink_ledbar_attrs[] = {
	&dev_attr_effect.attr,
	&dev_attr_version.attr,
	NULL
};
ATTRIBUTE_GROUPS(tplink_ledbar);

static int tplink_ledbar_probe(struct i2c_client *client)
{
	struct device *dev = &client->dev;
	u8 buf[TPLINK_LEDBAR_VERSION_LEN];
	struct tplink_ledbar *ledbar;
	int ret;

	ledbar = devm_kzalloc(dev, sizeof(*ledbar), GFP_KERNEL);
	if (!ledbar)
		return -ENOMEM;

	ledbar->client = client;
	/* the microcontroller plays "chase" from power-on until told otherwise */
	ledbar->effect = TPLINK_LEDBAR_EFFECT_CHASE;

	ret = devm_mutex_init(dev, &ledbar->lock);
	if (ret)
		return ret;

	i2c_set_clientdata(client, ledbar);

	ret = tplink_ledbar_query(ledbar, TPLINK_LEDBAR_CMD_MODE, 50, buf, 1);
	if (ret)
		return dev_err_probe(dev, ret, "mode query failed\n");
	if (buf[0] != TPLINK_LEDBAR_MODE_APP)
		return dev_err_probe(dev, -ENODEV,
				     "not in application mode (0x%02x)\n", buf[0]);

	ret = tplink_ledbar_query(ledbar, TPLINK_LEDBAR_CMD_VERSION, 500, buf,
				  sizeof(buf));
	if (ret)
		return dev_err_probe(dev, ret, "version query failed\n");
	if (tplink_ledbar_crc16(buf, sizeof(buf) - 2) !=
	    ((buf[sizeof(buf) - 2] << 8) | buf[sizeof(buf) - 1]))
		return dev_err_probe(dev, -EIO, "bad version checksum\n");

	memcpy(ledbar->version, buf, sizeof(ledbar->version) - 1);
	dev_info(dev, "TP-Link LED MCU %s\n", ledbar->version);

	return 0;
}

static void tplink_ledbar_shutdown(struct i2c_client *client)
{
	struct tplink_ledbar *ledbar = i2c_get_clientdata(client);

	/* stock shows "chase" while rebooting, unless the LEDs are switched off */
	mutex_lock(&ledbar->lock);
	if (ledbar->effect != TPLINK_LEDBAR_EFFECT_OFF)
		tplink_ledbar_send(ledbar,
				   TPLINK_LEDBAR_CMD_EFFECT(TPLINK_LEDBAR_EFFECT_CHASE));
	mutex_unlock(&ledbar->lock);
}

static const struct i2c_device_id tplink_ledbar_id[] = {
	{ "tplink-ledbar" },
	{ }
};
MODULE_DEVICE_TABLE(i2c, tplink_ledbar_id);

static const struct of_device_id tplink_ledbar_of_match[] = {
	{ .compatible = "tplink,be550-ledbar" },
	{ }
};
MODULE_DEVICE_TABLE(of, tplink_ledbar_of_match);

static struct i2c_driver tplink_ledbar_driver = {
	.driver = {
		.name = "tplink-ledbar",
		.of_match_table = tplink_ledbar_of_match,
		.dev_groups = tplink_ledbar_groups,
	},
	.probe = tplink_ledbar_probe,
	.shutdown = tplink_ledbar_shutdown,
	.id_table = tplink_ledbar_id,
};
module_i2c_driver(tplink_ledbar_driver);

MODULE_DESCRIPTION("TP-Link Archer BE550 LED bar driver");
MODULE_AUTHOR("Patrick Lawler <patricktlawler@gmail.com>");
MODULE_LICENSE("GPL");

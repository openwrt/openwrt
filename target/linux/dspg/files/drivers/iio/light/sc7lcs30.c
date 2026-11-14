// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (C) 2026 John Crispin <john@phrozen.org>
 */

#include <linux/bits.h>
#include <linux/delay.h>
#include <linux/i2c.h>
#include <linux/iio/iio.h>
#include <linux/mod_devicetable.h>
#include <linux/module.h>
#include <linux/mutex.h>
#include <linux/regmap.h>
#include <linux/unaligned.h>

#define SC7LCS30_ENABLE			0x80
#define SC7LCS30_ATIME			0x81
#define SC7LCS30_PERS			0x8c
#define SC7LCS30_CFG0			0x8d
#define SC7LCS30_PCFG0			0x8e
#define SC7LCS30_PCFG2			0x8f
#define SC7LCS30_CFG1			0x90
#define SC7LCS30_CHIP_ID		0x92
#define SC7LCS30_STATUS			0x93
#define SC7LCS30_ALS_DATA		0x94
#define SC7LCS30_PS_DATA		0x9c
#define SC7LCS30_CFG3			0xab
#define SC7LCS30_CFG4			0xac
#define SC7LCS30_CFG6			0xae
#define SC7LCS30_INTENAB		0xdd

#define SC7LCS30_ENABLE_PON		BIT(0)
#define SC7LCS30_ENABLE_AEN		BIT(1)
#define SC7LCS30_ENABLE_PEN		BIT(2)
#define SC7LCS30_ENABLE_WEN		BIT(3)

#define SC7LCS30_STATUS_PVALID		BIT(5)

#define SC7LCS30_ATIME_INIT		0x3f
#define SC7LCS30_PERS_INIT		0x1f
#define SC7LCS30_CFG0_INIT		0x41
#define SC7LCS30_PCFG0_INIT		0x4f
#define SC7LCS30_PCFG2_INIT		0x80
#define SC7LCS30_CFG1_INIT		0x23
#define SC7LCS30_CFG3_INIT		0x84
#define SC7LCS30_CFG4_INIT		0x3d
#define SC7LCS30_CFG6_INIT		0x3f

#define SC7LCS30_LUX_SCALE_INT		2
#define SC7LCS30_LUX_SCALE_MICRO	900390

#define SC7LCS30_PS_POLL_US		10000
#define SC7LCS30_PS_POLL_TRIES		10

struct sc7lcs30 {
	struct regmap *regmap;
	struct mutex lock;
};

static const struct reg_sequence sc7lcs30_init_seq[] = {
	{ SC7LCS30_ATIME, SC7LCS30_ATIME_INIT },
	{ SC7LCS30_PERS, SC7LCS30_PERS_INIT },
	{ SC7LCS30_CFG0, SC7LCS30_CFG0_INIT },
	{ SC7LCS30_PCFG0, SC7LCS30_PCFG0_INIT },
	{ SC7LCS30_PCFG2, SC7LCS30_PCFG2_INIT },
	{ SC7LCS30_CFG1, SC7LCS30_CFG1_INIT },
	{ SC7LCS30_CFG3, SC7LCS30_CFG3_INIT },
	{ SC7LCS30_CFG4, SC7LCS30_CFG4_INIT },
	{ SC7LCS30_CFG6, SC7LCS30_CFG6_INIT },
	{ SC7LCS30_INTENAB, 0 },
	{ SC7LCS30_ENABLE, SC7LCS30_ENABLE_PON | SC7LCS30_ENABLE_AEN |
			   SC7LCS30_ENABLE_PEN | SC7LCS30_ENABLE_WEN },
};

static const struct iio_chan_spec sc7lcs30_channels[] = {
	{
		.type = IIO_LIGHT,
		.address = SC7LCS30_ALS_DATA,
		.info_mask_separate = BIT(IIO_CHAN_INFO_RAW) |
				      BIT(IIO_CHAN_INFO_SCALE),
	},
	{
		.type = IIO_PROXIMITY,
		.address = SC7LCS30_PS_DATA,
		.info_mask_separate = BIT(IIO_CHAN_INFO_RAW),
	},
};

static int sc7lcs30_ps_wait(struct sc7lcs30 *data)
{
	unsigned int status;
	int ret, i;

	for (i = 0; i < SC7LCS30_PS_POLL_TRIES; i++) {
		ret = regmap_read(data->regmap, SC7LCS30_STATUS, &status);
		if (ret)
			return ret;

		if (status & SC7LCS30_STATUS_PVALID)
			return 0;

		fsleep(SC7LCS30_PS_POLL_US);
	}

	return -ETIMEDOUT;
}

static int sc7lcs30_read_data(struct sc7lcs30 *data,
			      const struct iio_chan_spec *chan, int *val)
{
	u8 buf[2];
	int ret;

	guard(mutex)(&data->lock);

	if (chan->type == IIO_PROXIMITY) {
		ret = sc7lcs30_ps_wait(data);
		if (ret)
			return ret;
	}

	ret = regmap_bulk_read(data->regmap, chan->address, buf, sizeof(buf));
	if (ret)
		return ret;

	*val = get_unaligned_le16(buf);

	return IIO_VAL_INT;
}

static int sc7lcs30_read_raw(struct iio_dev *indio_dev,
			     const struct iio_chan_spec *chan, int *val,
			     int *val2, long mask)
{
	struct sc7lcs30 *data = iio_priv(indio_dev);

	switch (mask) {
	case IIO_CHAN_INFO_RAW:
		return sc7lcs30_read_data(data, chan, val);
	case IIO_CHAN_INFO_SCALE:
		*val = SC7LCS30_LUX_SCALE_INT;
		*val2 = SC7LCS30_LUX_SCALE_MICRO;
		return IIO_VAL_INT_PLUS_MICRO;
	default:
		return -EINVAL;
	}
}

static const struct iio_info sc7lcs30_info = {
	.read_raw = sc7lcs30_read_raw,
};

static const struct regmap_config sc7lcs30_regmap_config = {
	.reg_bits = 8,
	.val_bits = 8,
	.max_register = SC7LCS30_INTENAB,
};

static void sc7lcs30_power_off(void *regmap)
{
	regmap_write(regmap, SC7LCS30_ENABLE, 0);
}

static int sc7lcs30_probe(struct i2c_client *client)
{
	struct device *dev = &client->dev;
	struct iio_dev *indio_dev;
	struct sc7lcs30 *data;
	unsigned int id;
	int ret;

	indio_dev = devm_iio_device_alloc(dev, sizeof(*data));
	if (!indio_dev)
		return -ENOMEM;

	data = iio_priv(indio_dev);

	ret = devm_mutex_init(dev, &data->lock);
	if (ret)
		return ret;

	data->regmap = devm_regmap_init_i2c(client, &sc7lcs30_regmap_config);
	if (IS_ERR(data->regmap))
		return PTR_ERR(data->regmap);

	ret = regmap_read(data->regmap, SC7LCS30_CHIP_ID, &id);
	if (ret)
		return dev_err_probe(dev, ret, "failed to read chip id\n");

	ret = regmap_multi_reg_write(data->regmap, sc7lcs30_init_seq,
				     ARRAY_SIZE(sc7lcs30_init_seq));
	if (ret)
		return dev_err_probe(dev, ret, "failed to configure sensor\n");

	ret = devm_add_action_or_reset(dev, sc7lcs30_power_off, data->regmap);
	if (ret)
		return ret;

	dev_info(dev, "chip id 0x%02x\n", id);

	indio_dev->name = "sc7lcs30";
	indio_dev->info = &sc7lcs30_info;
	indio_dev->modes = INDIO_DIRECT_MODE;
	indio_dev->channels = sc7lcs30_channels;
	indio_dev->num_channels = ARRAY_SIZE(sc7lcs30_channels);

	return devm_iio_device_register(dev, indio_dev);
}

static const struct of_device_id sc7lcs30_of_match[] = {
	{ .compatible = "silan,sc7lcs30" },
	{ }
};
MODULE_DEVICE_TABLE(of, sc7lcs30_of_match);

static const struct i2c_device_id sc7lcs30_id[] = {
	{ "sc7lcs30" },
	{ }
};
MODULE_DEVICE_TABLE(i2c, sc7lcs30_id);

static struct i2c_driver sc7lcs30_driver = {
	.driver = {
		.name = "sc7lcs30",
		.of_match_table = sc7lcs30_of_match,
	},
	.probe = sc7lcs30_probe,
	.id_table = sc7lcs30_id,
};
module_i2c_driver(sc7lcs30_driver);

MODULE_AUTHOR("John Crispin <john@phrozen.org>");
MODULE_DESCRIPTION("Silan SC7LCS30 ambient light and proximity sensor driver");
MODULE_LICENSE("GPL");

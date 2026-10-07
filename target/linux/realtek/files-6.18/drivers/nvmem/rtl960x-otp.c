// SPDX-License-Identifier: GPL-2.0-only
/*
 *  OTP/Efuse driver for Realtek RTL960X family of SoCs.
 */

#include <linux/bitfield.h>
#include <linux/delay.h>
#include <linux/mfd/syscon.h>
#include <linux/module.h>
#include <linux/nvmem-provider.h>
#include <linux/platform_device.h>
#include <linux/property.h>
#include <linux/regmap.h>
#include <linux/sizes.h>

#define RTL960X_EFUSE_WD	0x18
#define RTL960X_EFUSE_CMD	0x1c
#define  EFUSE_MODE_SEL		GENMASK(23, 20)
#define  EFUSE_WRITE_EN		BIT(17)
#define  EFUSE_READ_EN		0
#define  EFUSE_CMD_EN		BIT(16)
#define  EFUSE_ADDRESS		GENMASK(15, 0)
#define RTL960X_EFUSE_RD	0x20
#define  EFUSE_BUSY		BIT(16)

#define EFUSE_16BIT_DATA_MASK	GENMASK(15, 0)
#define EFUSE_8BIT_DATA_MASK	GENMASK(7, 0)

struct rtl960x_otp_pdata {
	u32 data_mask;
	int read_size;
	nvmem_reg_read_t reg_read;
};

struct rtl960x_otp {
	struct regmap *regmap;
	u32 data_mask;
};

static int rtl960x_otp_run_cmd(void *context, u32 op, u32 address, u32 *val)
{
	struct rtl960x_otp *otp = context;
	u32 tmp;
	int ret;

	ret = regmap_write(otp->regmap, RTL960X_EFUSE_CMD, op | EFUSE_CMD_EN |
			   FIELD_PREP(EFUSE_ADDRESS, address));
	if (ret)
		return ret;

	/* Wait OTP ready. HW busy status response need 2us */
	udelay(2);

	ret = regmap_read_poll_timeout(otp->regmap, RTL960X_EFUSE_RD, tmp,
				       !(tmp & EFUSE_BUSY), 2, 2000);
	if (ret)
		return ret;

	if (!val)
		return 0;

	ret = regmap_read(otp->regmap, RTL960X_EFUSE_RD, val);
	if (!ret)
		*val = *val & otp->data_mask;

	return ret;
}

static int rtl9607c_gen2_otp_read(void *context, unsigned int offset,
				  void *val, size_t bytes)
{
	u32 tmp;
	u8 *buf = val;
	int ret, i;

	for (i = 0; i < bytes; i++, buf++) {
		ret = rtl960x_otp_run_cmd(context, EFUSE_READ_EN, offset + i,
					  &tmp);
		if (ret)
			return ret;

		*buf = tmp;
	}

	return 0;
}

static int rtl9607c_gen1_otp_read(void *context, unsigned int offset,
				  void *val, size_t bytes)
{
	u32 tmp;
	u16 *buf = val;
	int ret, i;

	for (i = 0; i < bytes / 2; i++, buf++) {
		ret = rtl960x_otp_run_cmd(context, EFUSE_READ_EN, offset / 2 + i,
					  &tmp);
		if (ret)
			return ret;

		*buf = tmp;
	}

	return 0;
}

static int rtl960x_otp_probe(struct platform_device *pdev)
{
	struct nvmem_config otp_config = {
		.name = "rtl960x-otp",
		.size = SZ_64K,
	};
	const struct rtl960x_otp_pdata *pdata;
	struct device *dev = &pdev->dev;
	struct nvmem_device *nvmem;
	struct rtl960x_otp *otp;

	otp = devm_kzalloc(&pdev->dev, sizeof(*otp), GFP_KERNEL);
	if (!otp)
		return -ENOMEM;

	otp->regmap = syscon_node_to_regmap(dev->of_node->parent);
	if (IS_ERR(otp->regmap))
		return PTR_ERR(otp->regmap);

	pdata = device_get_match_data(dev);

	otp->data_mask = pdata->data_mask;

	otp_config.priv = otp;
	otp_config.dev = dev;
	otp_config.stride = pdata->read_size;
	otp_config.word_size = pdata->read_size;
	otp_config.reg_read = pdata->reg_read;
	nvmem = devm_nvmem_register(dev, &otp_config);

	return PTR_ERR_OR_ZERO(nvmem);
}

static const struct rtl960x_otp_pdata rtl9607c_gen2_otp_pdata = {
	.data_mask = EFUSE_8BIT_DATA_MASK,
	.read_size = 1,
	.reg_read = rtl9607c_gen2_otp_read,
};

static const struct rtl960x_otp_pdata rtl9607c_gen1_otp_pdata = {
	.data_mask = EFUSE_16BIT_DATA_MASK,
	.read_size = 2,
	.reg_read = rtl9607c_gen1_otp_read,
};

static const struct of_device_id rtl960x_otp_of_match[] = {
	{ .compatible = "realtek,rtl9607c-gen2-otp", .data = &rtl9607c_gen2_otp_pdata },
	{ .compatible = "realtek,rtl9607c-gen1-otp", .data = &rtl9607c_gen1_otp_pdata },
	{ /* sentinel */ }
};

MODULE_DEVICE_TABLE(of, rtl960x_otp_of_match);

static struct platform_driver rtl960x_otp_driver = {
	.probe = rtl960x_otp_probe,
	.driver = {
		.name = "rtl960x-otp",
		.of_match_table = rtl960x_otp_of_match,
	},
};
module_platform_driver(rtl960x_otp_driver);

MODULE_DESCRIPTION("Driver for Realtek RTL960X SoCs OTP");
MODULE_LICENSE("GPL");

// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (C) 2019 Realtek Semiconductor Corporation
 * Author: Cheng-Yu Lee <cylee12@realtek.com>
 */

#include <linux/clk.h>
#include <linux/clk-provider.h>
#include <linux/bitops.h>
#include <linux/platform_device.h>
#include <linux/mfd/syscon.h>
#include <linux/module.h>
#include "common.h"
#include "clk-pll.h"
#include "clk-regmap-gate.h"
#include "clk-regmap-mux.h"
#include "reset.h"
#include <dt-bindings/clock/rtd1295-clk.h>

#define GATE_COMMON(_id, _name, _parent, _flags, _ofs, _shift) \
	CLK_GATE_DATA(_id, _name, _parent, _flags, _ofs, _shift, 0, 0)
#define GATE(_id, _name, _parent, _ofs, _shift) \
	GATE_COMMON(_id, _name, _parent, 0, _ofs, _shift)

static struct clk_gate_data ic_gates[] = {
	GATE(RTD1295_ISO_CLK_EN_CEC0, "cec0", NULL, 0x8C, 2),
	GATE(RTD1295_ISO_CLK_EN_CBUSRX_SYS, "cbusrx_sys", NULL, 0x8C, 3),
	GATE(RTD1295_ISO_CLK_EN_CBUSTX_SYS, "cbustx_sys", NULL, 0x8C, 4),
	GATE(RTD1295_ISO_CLK_EN_CBUS_SYS, "cbus_sys", NULL, 0x8C, 5),
	GATE(RTD1295_ISO_CLK_EN_CBUS_OSC, "cbus_osc", NULL, 0x8C, 6),
	GATE(RTD1295_ISO_CLK_EN_IR, "ir", NULL, 0x8C, 7),
	GATE(RTD1295_ISO_CLK_EN_UR0, "ur0", "osc27m", 0x8C, 8),
	GATE(RTD1295_ISO_CLK_EN_I2C0, "i2c0", "osc27m", 0x8C, 9),
	GATE(RTD1295_ISO_CLK_EN_I2C1, "i2c1", "osc27m", 0x8C, 10),
	GATE(RTD1295_ISO_CLK_EN_ETN_250M, "etn_250m", NULL, 0x8C, 11),
	GATE(RTD1295_ISO_CLK_EN_ETN_SYS, "etn_sys", NULL, 0x8C, 12),
};

static struct rtk_reset_bank ic_reset_banks[] = {
	{ .ofs = 0x88, },
};

static struct rtk_reset_initdata ic_reset_initdata = {
	.banks     = ic_reset_banks,
	.num_banks = ARRAY_SIZE(ic_reset_banks),
};

static int rtd1295_ic_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	struct device_node *np = dev->of_node;
	struct rtk_clk_data *data;
	int ret;

	data = rtk_clk_alloc_data(RTD1295_ISO_CLK_MAX);
	if (!data)
		return -ENOMEM;

	ret = rtk_clk_of_init_data(np, data);
	if (ret) {
		rtk_clk_free_data(data);
		return ret;
	}

	platform_set_drvdata(pdev, data);

	rtk_clk_add_gates(dev, data, ic_gates, ARRAY_SIZE(ic_gates));

	ret = of_clk_add_provider(np, of_clk_src_onecell_get, &data->clk_data);
	if (ret)
		dev_err(dev, "failed to add clk provider: %d\n", ret);

	ic_reset_initdata.lock = data->lock;
	ic_reset_initdata.regmap = data->regmap;
	rtk_reset_controller_add(dev, &ic_reset_initdata);

	return 0;
}

static const struct of_device_id rtd1295_ic_match[] = {
	{ .compatible = "realtek,rtd1295-iso-clk", },
	{ /* sentinel */ }
};
MODULE_DEVICE_TABLE(of, rtd1295_ic_match);

static struct platform_driver rtd1295_ic_driver = {
	.probe = rtd1295_ic_probe,
	.driver = {
		.name = "rtk-rtd1295-iso-clk",
		.of_match_table = of_match_ptr(rtd1295_ic_match),
	},
};

static int __init rtd1295_ic_init(void)
{
	return platform_driver_register(&rtd1295_ic_driver);
}
subsys_initcall(rtd1295_ic_init);
MODULE_DESCRIPTION("Reatek RTD1295 ISO Controller Driver");
MODULE_AUTHOR("Cheng-Yu Lee <cylee12@realtek.com>");
MODULE_LICENSE("GPL v2");

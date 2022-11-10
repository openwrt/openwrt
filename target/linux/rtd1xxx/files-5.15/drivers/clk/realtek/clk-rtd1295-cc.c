// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (C) 2017,2019 Realtek Semiconductor Corporation
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

#define DIV_DV(_r, _d, _v)    { .rate = _r, .div = _d, .val = _v, }
#define FREQ_NF_MASK          (0x7FFFF)
#define FREQ_NF(_r, _n, _f)   { .rate = _r, .val = ((_n) << 11) | (_f), }
#define FREQ_MNO_MASK         (0x63FF0)
#define FREQ_MNO(_r, _m, _n, _o) \
	{  .rate = _r, .val = ((_m) << 4) | ((_n) << 12) | ((_o) << 17), }


static const struct div_table scpu_div_tbl[] = {
	DIV_DV(1000000000, 1, 0),
	DIV_DV(500000000,  2, 2),
	DIV_DV(250000000,  4, 3),
	DIV_TABLE_END
};

static const struct freq_table scpu_tbl[] = {
	FREQ_NF(1000000000, 34,   75),
	FREQ_NF(1100000000, 37, 1517),
	FREQ_NF(1200000000, 41,  910),
	FREQ_NF(1300000000, 45,  303),
	FREQ_NF(1400000000, 48, 1745),
	FREQ_NF(1500000000, 52, 1137),
	FREQ_NF(1600000000, 56,  531),
	FREQ_NF(1800000000, 63, 1365),
	FREQ_NF(1200000000, 41, 1024),
	FREQ_NF(1300000000, 45, 1024),
	FREQ_NF(1503000000, 48, 1744),
	FREQ_TABLE_END
};

static struct clk_pll_div pll_scpu = {
	.div_ofs    = 0x030,
	.div_shift  = 7,
	.div_width  = 2,
	.div_tbl    = scpu_div_tbl,
	.clkp       = {
		.pll_type     = CLK_PLL_TYPE_NF_SSC,
		.freq_mask    = FREQ_NF_MASK,
		.flags        = CLK_PLL_DIV_WORKAROUND,
		.ssc_ofs      = 0x500,
		.pll_ofs      = CLK_OFS_INVALID,
		.freq_tbl     = scpu_tbl,
		.clkr.hw.init = CLK_HW_INIT("pll_scpu", "osc27m", &clk_pll_div_ops, CLK_IGNORE_UNUSED | CLK_GET_RATE_NOCACHE),
		.clkr.shared = 1,
	},
};

static const struct freq_table bus_tbl[] = {
	FREQ_NF(200000000,  4,  835),
	FREQ_NF(243000000,  6,    0),
	FREQ_NF(256000000,  6, 1024),
	FREQ_NF(256000000,  6,  986),
	FREQ_NF(257000000,  6, 1061),
	FREQ_NF(459000000, 14,    0),
	FREQ_NF(486000000, 15,    0),
	FREQ_NF(482000000, 14, 1744),
	FREQ_NF(513000000, 16,    0),
	FREQ_NF(540000000, 17,    0),
	FREQ_TABLE_END
};

static const struct div_table bus_div_tbl[] = {
	DIV_DV(256000000,  1, 0),
	DIV_DV(1,          2, 1),
	DIV_TABLE_END
};

static struct clk_pll_div pll_bus = {
	.div_ofs    = 0x030,
	.div_shift  = 0,
	.div_width  = 1,
	.div_tbl    = bus_div_tbl,
	.clkp       = {
		.pll_type     = CLK_PLL_TYPE_NF_SSC,
		.freq_mask    = FREQ_NF_MASK,
		.freq_tbl     = bus_tbl,
		.pll_ofs      = CLK_OFS_INVALID,
		.ssc_ofs      = 0x520,
		.clkr.hw.init = CLK_HW_INIT("pll_bus", "osc27m", &clk_pll_div_ops, CLK_IGNORE_UNUSED | CLK_GET_RATE_NOCACHE),
		.clkr.shared = 1,
	},
};

static struct clk_fixed_factor clk_sys = {
	.div     = 1,
	.mult    = 1,
	.hw.init = CLK_HW_INIT("clk_sys", "pll_bus", &clk_fixed_factor_ops, CLK_SET_RATE_PARENT),
};

static struct clk_pll pll_dcsb = {
	.pll_type     = CLK_PLL_TYPE_NF_SSC,
	.freq_mask    = FREQ_NF_MASK,
	.freq_tbl     = bus_tbl,
	.pll_ofs      = CLK_OFS_INVALID,
	.ssc_ofs      = 0x540,
	.clkr.hw.init = CLK_HW_INIT("pll_dcsb", "osc27m", &clk_pll_ops, CLK_IGNORE_UNUSED | CLK_GET_RATE_NOCACHE),
};

static struct clk_fixed_factor clk_sysh = {
	.div     = 1,
	.mult    = 1,
	.hw.init = CLK_HW_INIT("clk_sysh", "pll_dcsb", &clk_fixed_factor_ops, CLK_SET_RATE_PARENT),
};

static const struct freq_table ddsx_tbl[] = {
	FREQ_NF(432000000, 13, 0),
	FREQ_TABLE_END
};

static struct clk_pll pll_ddsa = {
	.pll_type     = CLK_PLL_TYPE_NF_SSC,
	.freq_mask    = FREQ_NF_MASK,
	.freq_tbl     = ddsx_tbl,
	.pll_ofs      = 0x120,
	.pow_loc      = CLK_PLL_CONF_POW_LOC_CTL3,
	.ssc_ofs      = 0x560,
	.clkr.hw.init = CLK_HW_INIT("pll_ddsa", "osc27m", &clk_pll_ops, CLK_IGNORE_UNUSED | CLK_GET_RATE_NOCACHE),
};

static struct clk_pll pll_ddsb = {
	.pll_type     = CLK_PLL_TYPE_NF_SSC,
	.freq_mask    = FREQ_NF_MASK,
	.freq_tbl     = ddsx_tbl,
	.pll_ofs      = 0x174,
	.pow_loc      = CLK_PLL_CONF_POW_LOC_CTL2,
	.ssc_ofs      = 0x580,
	.clkr.hw.init = CLK_HW_INIT("pll_ddsb", "osc27m", &clk_pll_ops, CLK_GET_RATE_NOCACHE),
};

static const struct freq_table gpu_tbl[] = {
	FREQ_NF(300000000,  8,  227),
	FREQ_NF(320000000,  8, 1744),
	FREQ_NF(340000000,  9, 1213),
	FREQ_NF(360000000, 10,  682),
	FREQ_NF(380000000, 11,  151),
	FREQ_NF(400000000, 11, 1668),
	FREQ_NF(420000000, 12, 1137),
	FREQ_NF(440000000, 13,  606),
	FREQ_NF(460000000, 14,   75),
	FREQ_NF(480000000, 14, 1592),
	FREQ_NF(500000000, 15, 1061),
	FREQ_NF(520000000, 16,  530),
	FREQ_NF(540000000, 17,    0),
	FREQ_NF(560000000, 17, 1517),
	FREQ_NF(580000000, 18,  986),
	FREQ_NF(600000000, 19,  455),
	FREQ_NF(620000000, 19, 1972),
	FREQ_NF(640000000, 20, 1441),
	FREQ_NF(660000000, 21,  910),
	FREQ_NF(680000000, 22,  379),
	FREQ_NF(460000000, 13, 1365),
	FREQ_TABLE_END
};

static struct clk_pll pll_gpu = {
	.pll_type     = CLK_PLL_TYPE_NF_SSC,
	.freq_mask    = FREQ_NF_MASK,
	.freq_tbl     = gpu_tbl,
	.pll_ofs      = 0x1C0,
	.pow_loc      = CLK_PLL_CONF_POW_LOC_CTL2,
	.ssc_ofs      = 0x5A0,
	.clkr.hw.init = CLK_HW_INIT("pll_gpu", "osc27m", &clk_pll_ops, CLK_GET_RATE_NOCACHE),
};

static const struct freq_table ve_tbl[] = {
	FREQ_MNO(189000000,  5, 0, 0),
	FREQ_MNO(270000000,  8, 0, 0),
	FREQ_MNO(405000000, 13, 0, 0),
	FREQ_MNO(432000000, 14, 0, 0),
	FREQ_MNO(459000000, 15, 0, 0),
	FREQ_MNO(486000000, 16, 0, 0),
	FREQ_MNO(513000000, 17, 0, 0),
	FREQ_MNO(540000000, 18, 0, 0),
	FREQ_MNO(567000000, 19, 0, 0),
	FREQ_MNO(594000000, 20, 0, 0),
	FREQ_MNO(648000000, 22, 0, 0),
	FREQ_MNO(675000000, 23, 0, 0),
	FREQ_MNO(702000000, 24, 0, 0),
	FREQ_MNO(715000000, 51, 1, 0),
	FREQ_TABLE_END
};

static struct clk_pll pll_ve1 = {
	.pll_type     = CLK_PLL_TYPE_MNO,
	.freq_mask    = FREQ_MNO_MASK,
	.freq_tbl     = ve_tbl,
	.pll_ofs      = 0x114,
	.pow_loc      = CLK_PLL_CONF_POW_LOC_CTL2,
	.ssc_ofs      = CLK_OFS_INVALID,
	.clkr.hw.init = CLK_HW_INIT("pll_ve1", "osc27m", &clk_pll_ops, CLK_GET_RATE_NOCACHE | CLK_SET_RATE_GATE),
};

static struct clk_pll pll_ve2 = {
	.pll_type     = CLK_PLL_TYPE_MNO,
	.freq_mask    = FREQ_MNO_MASK,
	.freq_tbl     = ve_tbl,
	.pll_ofs      = 0x1D0,
	.pow_loc      = CLK_PLL_CONF_POW_LOC_CTL2,
	.ssc_ofs      = CLK_OFS_INVALID,
	.clkr.hw.init = CLK_HW_INIT("pll_ve2", "osc27m", &clk_pll_ops, CLK_GET_RATE_NOCACHE | CLK_SET_RATE_GATE),
};

static struct clk_hw *cc_hws[] = {
	[RTD1295_CRT_PLL_SCPU]    = &__clk_pll_div_hw(&pll_scpu),
	[RTD1295_CRT_PLL_BUS]     = &__clk_pll_div_hw(&pll_bus),
	[RTD1295_CRT_PLL_DCSB]    = &__clk_pll_hw(&pll_dcsb),
	[RTD1295_CRT_PLL_DDSA]    = &__clk_pll_hw(&pll_ddsa),
	[RTD1295_CRT_PLL_DDSB]    = &__clk_pll_hw(&pll_ddsb),
	[RTD1295_CRT_PLL_GPU]     = &__clk_pll_hw(&pll_gpu),
	[RTD1295_CRT_PLL_VE1]     = &__clk_pll_hw(&pll_ve1),
	[RTD1295_CRT_PLL_VE2]     = &__clk_pll_hw(&pll_ve2),
	[RTD1295_CRT_CLK_SYS]     = &clk_sys.hw,
	[RTD1295_CRT_CLK_SYSH]    = &clk_sysh.hw,
};

static const char * const ve_parents[] = {
	"clk_sysh",
	"pll_ve1",
	"pll_ve2",
	"pll_ve2",
};

static struct clk_composite_data cc_composites[] = {
	{
		.id           = RTD1295_CRT_CLK_GPU,
		.flags        = CLK_SET_RATE_PARENT,
		.gate_ofs     = 0x00C,
		.gate_shift   = 11,
		.mux_ofs      = CLK_OFS_INVALID,
		.name         = "clk_gpu",
		.num_parents  = 1,
		.parent_names = (const char *[]){ "pll_gpu" },
		.shared       = 1,
	},
	{
		.id           = RTD1295_CRT_CLK_VE1,
		.flags        = CLK_SET_RATE_PARENT | CLK_SET_RATE_NO_REPARENT,
		.gate_ofs     = 0x00C,
		.gate_shift   = 12,
		.mux_ofs      = 0x04C,
		.mux_shift    = 0,
		.mux_width    = 2,
		.name         = "clk_ve1",
		.num_parents  = ARRAY_SIZE(ve_parents),
		.parent_names = ve_parents,
		.shared       = 1
	},
	{
		.id           = RTD1295_CRT_CLK_VE2,
		.flags        = CLK_SET_RATE_PARENT | CLK_SET_RATE_NO_REPARENT,
		.gate_ofs     = 0x00C,
		.gate_shift   = 13,
		.mux_ofs      = 0x04C,
		.mux_shift    = 2,
		.mux_width    = 2,
		.name         = "clk_ve2",
		.num_parents  = ARRAY_SIZE(ve_parents),
		.parent_names = ve_parents,
		.shared       = 1
	},
	{
		.id           = RTD1295_CRT_CLK_VE3,
		.flags        = CLK_SET_RATE_PARENT | CLK_SET_RATE_NO_REPARENT,
		.gate_ofs     = 0x00C,
		.gate_shift   = 29,
		.mux_ofs      = 0x04C,
		.mux_shift    = 4,
		.mux_width    = 2,
		.name         = "clk_ve3",
		.num_parents  = ARRAY_SIZE(ve_parents),
		.parent_names = ve_parents,
		.shared       = 1
	},
};

#define GATE_COMMON(_id, _name, _parent, _flags, _ofs, _shift) \
	CLK_GATE_DATA(_id, _name, _parent, _flags, _ofs, _shift, 0, 1)
#define GATE_CRITICAL(_id, _name, _parent, _ofs, _shift) \
	GATE_COMMON(_id, _name, _parent, CLK_IS_CRITICAL, _ofs, _shift)
#define GATE(_id, _name, _parent, _ofs, _shift) \
	GATE_COMMON(_id, _name, _parent, 0, _ofs, _shift)
#define GATE_IGNORE(_id, _name, _parent, _ofs, _shift) \
	GATE_COMMON(_id, _name, _parent, CLK_IGNORE_UNUSED, _ofs, _shift)

static struct clk_gate_data cc_gates[] = {
	GATE_CRITICAL(RTD1295_CRT_CLK_EN_MISC, "misc", "osc27m",  0x0C, 0),
	GATE(RTD1295_CRT_CLK_EN_PCIE0, "pcie0", NULL, 0x0C, 1),
	GATE(RTD1295_CRT_CLK_EN_SATA_0, "sata_0", NULL, 0x0C, 2),
	GATE(RTD1295_CRT_CLK_EN_GSPI, "gspi", NULL, 0x0C, 3),
	GATE(RTD1295_CRT_CLK_EN_USB, "usb", NULL, 0x0C, 4),
	GATE(RTD1295_CRT_CLK_EN_SATA_ALIVE_0, "sata_alive_0", NULL, 0x0C, 7),
	GATE_IGNORE(RTD1295_CRT_CLK_EN_HDMI, "hdmi", NULL, 0x0C, 8),
	GATE(RTD1295_CRT_CLK_EN_ETN, "etn", NULL, 0x0C, 9),
	GATE(RTD1295_CRT_CLK_EN_SE, "se", NULL, 0x0C, 17),
	GATE_IGNORE(RTD1295_CRT_CLK_EN_CP, "cp", NULL, 0x0C, 19),
	GATE_IGNORE(RTD1295_CRT_CLK_EN_MD, "md", NULL, 0x0C, 20),
	GATE_IGNORE(RTD1295_CRT_CLK_EN_TP, "tp", NULL, 0x0C, 21),
	GATE(RTD1295_CRT_CLK_EN_NF, "nf", NULL, 0x0C, 23),
	GATE(RTD1295_CRT_CLK_EN_EMMC, "emmc", NULL, 0x0C, 24),
	GATE(RTD1295_CRT_CLK_EN_CR, "cr", NULL, 0x0C, 25),
	GATE(RTD1295_CRT_CLK_EN_SDIO_IP, "sdio_ip", NULL, 0x0C, 26),
	GATE(RTD1295_CRT_CLK_EN_MIPI, "mipi", NULL, 0x0C, 27),
	GATE(RTD1295_CRT_CLK_EN_EMMC_IP, "emmc_ip", NULL, 0x0C, 28),
	GATE(RTD1295_CRT_CLK_EN_SDIO, "sdio", NULL, 0x0C, 30),
	GATE(RTD1295_CRT_CLK_EN_SD_IP, "sd_ip", NULL, 0x0C, 31),
	GATE(RTD1295_CRT_CLK_EN_NAT, "nat", NULL, 0x10, 0),
	GATE(RTD1295_CRT_CLK_EN_MISC_I2C_5, "i2c5", NULL, 0x10, 1),
	GATE(RTD1295_CRT_CLK_EN_JPEG, "jpeg", NULL, 0x10, 3),
	GATE(RTD1295_CRT_CLK_EN_PCIE1, "pcie1", NULL, 0x10, 5),
	GATE(RTD1295_CRT_CLK_EN_MISC_SC, "sc", "misc", 0x10, 6),
	GATE(RTD1295_CRT_CLK_EN_CBUS_TX, "cbus_tx", NULL, 0x10, 7),
	GATE(RTD1295_CRT_CLK_EN_MISC_RTC, "rtc", NULL, 0x10, 10),
	GATE(RTD1295_CRT_CLK_EN_MISC_I2C_4, "i2c4", "misc", 0x10, 13),
	GATE(RTD1295_CRT_CLK_EN_MISC_I2C_3, "i2c3", "misc", 0x10, 14),
	GATE(RTD1295_CRT_CLK_EN_MISC_I2C_2, "i2c2", "misc", 0x10, 15),
	GATE(RTD1295_CRT_CLK_EN_HDMIRX, "hdmirx", NULL, 0x10, 24),
	GATE(RTD1295_CRT_CLK_EN_SATA_1, "sata_1", NULL, 0x10, 25),
	GATE(RTD1295_CRT_CLK_EN_SATA_ALIVE_1, "sata_alive_1", NULL, 0x10, 26),
	GATE(RTD1295_CRT_CLK_EN_UR2, "ur2", "clk432m", 0x10, 27),
	GATE(RTD1295_CRT_CLK_EN_UR1, "ur1", "clk432m", 0x10, 28),
	GATE(RTD1295_CRT_CLK_EN_FAN, "fan", "misc", 0x10, 29),
	GATE(RTD1295_CRT_CLK_EN_LSADC, "lsadc", NULL, 0x450, 2),
	GATE_IGNORE(RTD1295_CRT_CLK_EN_TVE, "tve", NULL, 0x0C, 14),
	GATE_IGNORE(RTD1295_CRT_CLK_EN_VO, "vo", NULL, 0x0C, 15),
	GATE_IGNORE(RTD1295_CRT_CLK_EN_LVDS, "lvds", NULL, 0x0C, 16),
};

static struct rtk_reset_bank cc_reset_banks[] = {
	{ .ofs = 0x00, },
	{ .ofs = 0x04, },
	{ .ofs = 0x50, },
};

static struct rtk_reset_initdata cc_reset_initdata = {
	.banks     = cc_reset_banks,
	.num_banks = ARRAY_SIZE(cc_reset_banks),
	.shared    = 1,
};

static int rtd1295_cc_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	struct device_node *np = dev->of_node;
	struct rtk_clk_data *data;
	int ret;

	data = rtk_clk_alloc_data(RTD1295_CRT_CLK_MAX);
	if (!data)
		return -ENOMEM;

	ret = rtk_clk_of_init_data(np, data);
	if (ret) {
		rtk_clk_free_data(data);
		return ret;
	}

	platform_set_drvdata(pdev, data);

	rtk_clk_add_hws(dev, data, cc_hws, ARRAY_SIZE(cc_hws));
	rtk_clk_add_composites(dev, data, cc_composites,
			       ARRAY_SIZE(cc_composites));
	rtk_clk_add_gates(dev, data, cc_gates, ARRAY_SIZE(cc_gates));

	ret = of_clk_add_provider(np, of_clk_src_onecell_get, &data->clk_data);
	if (ret)
		dev_err(dev, "failed to add clk provider: %d\n", ret);

	cc_reset_initdata.lock = data->lock;
	cc_reset_initdata.regmap = data->regmap;
	rtk_reset_controller_add(dev, &cc_reset_initdata);

	return 0;
}

static const struct of_device_id rtd1295_cc_match[] = {
	{ .compatible = "realtek,rtd1295-crt-clk", },
	{ /* sentinel */ }
};
MODULE_DEVICE_TABLE(of, rtd1295_cc_match);

static struct platform_driver rtd1295_cc_driver = {
	.probe = rtd1295_cc_probe,
	.driver = {
		.name = "rtk-rtd1295-crt-clk",
		.of_match_table = of_match_ptr(rtd1295_cc_match),
	},
};

static int __init rtd1295_cc_init(void)
{
	return platform_driver_register(&rtd1295_cc_driver);
}
subsys_initcall(rtd1295_cc_init);
MODULE_DESCRIPTION("Reatek RTD1295 CRT Controller Driver");
MODULE_AUTHOR("Cheng-Yu Lee <cylee12@realtek.com>");
MODULE_LICENSE("GPL v2");

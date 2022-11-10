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
#include <linux/of_device.h>
#include <linux/module.h>
#include "common.h"
#include "clk-pll.h"
#include "clk-regmap-gate.h"
#include "clk-regmap-mux.h"
#include "reset.h"
#include <dt-bindings/clock/rtd1319-clk.h>
#include <dt-bindings/reset/rtd1319-reset.h>

#define DIV_DV(_r, _d, _v)    { .rate = _r, .div = _d, .val = _v, }
#define FREQ_NF_MASK          (0x7FFFF)
#define FREQ_NF(_r, _n, _f)   { .rate = _r, .val = ((_n) << 11) | (_f), }

#define FREQ_MNO_RS_MASK      (0x7FFFE)
#define FREQ_MNO_RS(_r, _m, _n, _o, _rs) \
	{ .rate = _r, .val = ((_m) << 4) | ((_n) << 12) | ((_o) << 17) | (_rs), }
#define FREQ_MNO_MASK         (0x63FF0)
#define FREQ_MNO(_r, _m, _n, _o) \
	FREQ_MNO_RS(_r, _m, _n, _o, ((_m) <= 0x18 ? 0x0000c002 : 0x0001c000))


static const struct div_table scpu_div_tbl[] = {
	DIV_DV(1000000000,  1, 0x00),
	DIV_DV(500000000,   2, 0x88),
	DIV_DV(250000000,   4, 0x90),
	DIV_DV(200000000,   8, 0xA0),
	DIV_DV(100000000,  10, 0xA8),
	DIV_TABLE_END
};

static const struct freq_table scpu_tbl[] = {
	FREQ_NF(918000000,  31,    0),
	FREQ_NF(1000000000, 34,   75),
	FREQ_NF(1100000000, 37, 1517),
	FREQ_NF(1200000000, 41,  910),
	FREQ_NF(1300000000, 45,  303),
	FREQ_NF(1350000000, 47,    0),
	FREQ_NF(1400000000, 48, 1745),
	FREQ_NF(1500000000, 52, 1137),
	FREQ_NF(1600000000, 56,  530),
	FREQ_NF(1700000000, 59, 1972),
	FREQ_NF(1800000000, 63, 1365),
	FREQ_NF(1900000000, 67,  758),
	FREQ_NF(2000000000, 71,  151),
	FREQ_NF(1000000000, 35,    0),
	FREQ_NF(1200000000, 41,    0),
	FREQ_NF(1800000000, 65,    0),
	FREQ_NF(1800000000, 64,    0),
	FREQ_TABLE_END
};

static struct clk_pll_div pll_scpu = {
	.div_ofs = 0x030,
	.div_shift  = 6,
	.div_width  = 8,
	.div_tbl    = scpu_div_tbl,
	.clkp       = {
		.ssc_ofs   = 0x500,
		.pll_ofs   = CLK_OFS_INVALID,
		.pll_type  = CLK_PLL_TYPE_NF_SSC,
		.freq_tbl  = scpu_tbl,
		.freq_mask = FREQ_NF_MASK,
		.clkr.hw.init = CLK_HW_INIT("pll_scpu", "osc27m", &clk_pll_div_ops, CLK_IGNORE_UNUSED | CLK_GET_RATE_NOCACHE),
		.clkr.shared = 1,
	},
};

static const struct div_table bus_div_tbl[] = {
	DIV_DV(300000000, 1, 0),
	DIV_DV(1, 2, 2),
	DIV_DV(1, 4, 3),
	DIV_TABLE_END
};

static const struct freq_table bus_tbl[] = {
	FREQ_NF(459000000, 31,    0),
	FREQ_NF(486000000, 33,    0),
	FREQ_NF(499500000, 34,    0),
	FREQ_NF(513000000, 35,    0),
	FREQ_TABLE_END
};

static struct clk_pll_div pll_bus = {
	.div_ofs = 0x030,
	.div_shift  = 0,
	.div_width  = 2,
	.div_tbl    = bus_div_tbl,
	.clkp       = {
		.ssc_ofs   = 0x520,
		.pll_ofs   = CLK_OFS_INVALID,
		.pll_type  = CLK_PLL_TYPE_NF_SSC,
		.freq_tbl  = bus_tbl,
		.freq_mask = FREQ_NF_MASK,
		.clkr.hw.init = CLK_HW_INIT("pll_bus", "osc27m", &clk_pll_div_ops, CLK_IGNORE_UNUSED | CLK_GET_RATE_NOCACHE),
		.clkr.shared = 1,
	},
};

static const struct div_table dcsb_div_tbl[] = {
	DIV_DV(300000000, 1, 0),
	DIV_DV(1, 2, 2),
	DIV_DV(1, 4, 3),
	DIV_TABLE_END
};

static const struct freq_table dcsb_tbl[] = {
	FREQ_NF(337500000, 22, 0),
	FREQ_NF(351000000, 23, 0),
	FREQ_NF(459000000, 31, 0),
	FREQ_NF(472000000, 32, 0),
	FREQ_NF(553500000, 38, 0),
	FREQ_TABLE_END
};

static struct clk_pll_div pll_dcsb = {
	.div_ofs = 0x030,
	.div_shift  = 2,
	.div_width  = 2,
	.div_tbl    = dcsb_div_tbl,
	.clkp       = {
		.ssc_ofs   = 0x540,
		.pll_ofs   = CLK_OFS_INVALID,
		.pll_type  = CLK_PLL_TYPE_NF_SSC,
		.freq_tbl  = dcsb_tbl,
		.freq_mask = FREQ_NF_MASK,
		.clkr.hw.init = CLK_HW_INIT("pll_dcsb", "osc27m", &clk_pll_div_ops, CLK_IGNORE_UNUSED | CLK_GET_RATE_NOCACHE),
		.clkr.shared = 1,
	},
};

static struct clk_fixed_factor clk_sys = {
	.div     = 1,
	.mult    = 1,
	.hw.init = CLK_HW_INIT("clk_sys", "pll_bus", &clk_fixed_factor_ops, CLK_SET_RATE_PARENT),
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
	.ssc_ofs   = 0x560,
	.pll_ofs   = 0x120,
	.pow_loc   = CLK_PLL_CONF_POW_LOC_CTL3,
	.pll_type  = CLK_PLL_TYPE_NF_SSC,
	.freq_tbl  = ddsx_tbl,
	.freq_mask = FREQ_NF_MASK,
	.clkr.hw.init = CLK_HW_INIT("pll_ddsa", "osc27m", &clk_pll_ops, CLK_IGNORE_UNUSED | CLK_GET_RATE_NOCACHE),
};

static const struct freq_table gpu_tbl[] = {
	FREQ_NF(300000000, 19,  455),
	FREQ_NF(400000000, 26, 1289),
	FREQ_NF(500000000, 34,   75),
	FREQ_NF(550000000, 37, 1517),
	FREQ_NF(553500000, 38,    0),
	FREQ_NF(585000000, 40,  682),
	FREQ_NF(600000000, 41,  910),
	FREQ_NF(650000000, 45,  303),
	FREQ_NF(700000000, 48, 1745),
	FREQ_NF(720000000, 50,  682),
	FREQ_NF(750000000, 52, 1137),
	FREQ_TABLE_END
};

static struct clk_pll pll_gpu = {
	.ssc_ofs   = 0x5A0,
	.pll_ofs   = 0x1C0,
	.pow_loc   = CLK_PLL_CONF_POW_LOC_CTL2,
	.pll_type  = CLK_PLL_TYPE_NF_SSC,
	.freq_tbl  = gpu_tbl,
	.freq_mask = FREQ_NF_MASK,
	.clkr.hw.init = CLK_HW_INIT("pll_gpu", "osc27m", &clk_pll_ops, CLK_GET_RATE_NOCACHE),
};

static struct clk_pll rtd1312c_pll_gpu = {
	.ssc_ofs   = 0x5A0,
	.pll_ofs   = 0x1C0,
	.pow_loc   = CLK_PLL_CONF_POW_LOC_CTL2,
	.pll_type  = CLK_PLL_TYPE_NF_SSC,
	.freq_tbl  = gpu_tbl,
	.freq_mask = FREQ_NF_MASK,
	.clkr.hw.init = CLK_HW_INIT("pll_gpu", "osc27m", &clk_pll_ops, CLK_GET_RATE_NOCACHE),
	CLK_PLL_CONF_ANALOG(0x0033fe00, 0x0011e000),
};


static const struct freq_table ve_tbl[] = {
	FREQ_MNO(189000000, 12, 0, 1),
	FREQ_MNO(270000000, 18, 0, 1),
	FREQ_MNO(405000000, 13, 0, 0),
	FREQ_MNO(432000000, 14, 0, 0),
	FREQ_MNO(459000000, 15, 0, 0),
	FREQ_MNO(486000000, 16, 0, 0),
	FREQ_MNO(500000000, 35, 1, 0),
	FREQ_MNO(513000000, 17, 0, 0),
	FREQ_MNO(540000000, 18, 0, 0),
	FREQ_MNO(550000000, 59, 2, 0),
	FREQ_MNO(567000000, 19, 0, 0),
	FREQ_MNO(594000000, 20, 0, 0),
	FREQ_MNO(621000000, 21, 0, 0),
	FREQ_MNO(648000000, 22, 0, 0),
	FREQ_MNO(675000000, 23, 0, 0),
	FREQ_MNO(702000000, 24, 0, 0),
	FREQ_MNO(715000000, 51, 1, 0),
	FREQ_TABLE_END
};

static struct clk_pll pll_ve1 = {
	.ssc_ofs   = CLK_OFS_INVALID,
	.pll_ofs   = 0x114,
	.pow_loc   = CLK_PLL_CONF_POW_LOC_CTL2,
	.pll_type  = CLK_PLL_TYPE_MNO,
	.freq_tbl  = ve_tbl,
	.freq_mask = FREQ_MNO_MASK,
	.freq_mask_set = FREQ_MNO_RS_MASK,
	.clkr.hw.init = CLK_HW_INIT("pll_ve1", "osc27m", &clk_pll_ops, CLK_GET_RATE_NOCACHE | CLK_SET_RATE_GATE),
};

static struct clk_pll pll_ve2 = {
	.ssc_ofs   = CLK_OFS_INVALID,
	.pll_ofs   = 0x1D0,
	.pow_loc   = CLK_PLL_CONF_POW_LOC_CTL2,
	.pll_type  = CLK_PLL_TYPE_MNO,
	.freq_tbl  = ve_tbl,
	.freq_mask = FREQ_MNO_MASK,
	.freq_mask_set = FREQ_MNO_RS_MASK,
	.clkr.hw.init = CLK_HW_INIT("pll_ve2", "osc27m", &clk_pll_ops, CLK_GET_RATE_NOCACHE | CLK_SET_RATE_GATE),
};

static struct clk_fixed_factor pll_emmc_ref = {
	.div = 6,
	.mult = 1,
	.hw.init = CLK_HW_INIT("pll_emmc_ref", "osc27m", &clk_fixed_factor_ops, 0),
};

static struct clk_pll_mmc pll_emmc = {
	.pll_ofs = 0x1f0,
	.clkr.hw.init = CLK_HW_INIT("pll_emmc", "pll_emmc_ref", &clk_pll_mmc_ops, 0),
	.phase0_hw.init = CLK_HW_INIT("pll_emmc_vp0", "pll_emmc", &clk_pll_mmc_phase_ops, 0),
	.phase1_hw.init = CLK_HW_INIT("pll_emmc_vp1", "pll_emmc", &clk_pll_mmc_phase_ops, 0),
};

static struct clk_hw *cc_hws[] = {
	[RTD1319_CRT_PLL_SCPU]    = &__clk_pll_div_hw(&pll_scpu),
	[RTD1319_CRT_PLL_BUS]     = &__clk_pll_div_hw(&pll_bus),
	[RTD1319_CRT_PLL_DCSB]    = &__clk_pll_div_hw(&pll_dcsb),
	[RTD1319_CRT_CLK_SYS]     = &clk_sys.hw,
	[RTD1319_CRT_CLK_SYSH]    = &clk_sysh.hw,
	[RTD1319_CRT_PLL_DDSA]    = &__clk_pll_hw(&pll_ddsa),
	[RTD1319_CRT_PLL_GPU]     = &__clk_pll_hw(&pll_gpu),
	[RTD1319_CRT_PLL_VE1]     = &__clk_pll_hw(&pll_ve1),
	[RTD1319_CRT_PLL_VE2]     = &__clk_pll_hw(&pll_ve2),

	[RTD1319_CRT_PLL_EMMC_REF] = &pll_emmc_ref.hw,
	[RTD1319_CRT_PLL_EMMC]     = &__clk_pll_mmc_hw(&pll_emmc),
	[RTD1319_CRT_PLL_EMMC_VP0] = &pll_emmc.phase0_hw,
	[RTD1319_CRT_PLL_EMMC_VP1] = &pll_emmc.phase1_hw,

};

static const char * const ve_parents[] = {
	"pll_gpu",
	"clk_sysh",
	"pll_ve1",
	"pll_ve2",
};

static struct clk_composite_data cc_composites[] = {
	{
		.id            = RTD1319_CRT_CLK_GPU,
		.mux_ofs       = CLK_OFS_INVALID,
		.gate_ofs      = 0x050,
		.gate_shift    = 18,
		.gate_write_en = 1,
		.parent_names  = (const char *[]){ "pll_gpu" },
		.num_parents   = 1,
		.name          = "clk_gpu",
		.flags         = CLK_SET_RATE_PARENT,
	},
	{
		.id            = RTD1319_CRT_CLK_VE1,
		.gate_ofs      = 0x050,
		.gate_shift    = 20,
		.gate_write_en = 1,
		.mux_ofs       = 0x04C,
		.mux_width     = 3,
		.mux_shift     = 0,
		.parent_names  = ve_parents,
		.num_parents   = ARRAY_SIZE(ve_parents),
		.name          = "clk_ve1",
		.flags         = CLK_SET_RATE_PARENT | CLK_SET_RATE_NO_REPARENT,
	},
	{
		.id            = RTD1319_CRT_CLK_VE2,
		.gate_ofs      = 0x050,
		.gate_shift    = 22,
		.gate_write_en = 1,
		.mux_ofs       = 0x04C,
		.mux_width     = 3,
		.mux_shift     = 3,
		.parent_names  = ve_parents,
		.num_parents   = ARRAY_SIZE(ve_parents),
		.name          = "clk_ve2",
		.flags         = CLK_SET_RATE_PARENT | CLK_SET_RATE_NO_REPARENT,
	},
	{
		.id            = RTD1319_CRT_CLK_VE3,
		.gate_ofs      = 0x05C,
		.gate_shift    = 26,
		.gate_write_en = 1,
		.mux_ofs       = 0x04C,
		.mux_width     = 3,
		.mux_shift     = 6,
		.parent_names  = ve_parents,
		.num_parents   = ARRAY_SIZE(ve_parents),
		.name          = "clk_ve3",
		.flags         = CLK_SET_RATE_PARENT | CLK_SET_RATE_NO_REPARENT,
	},
	{
		.id            = RTD1319_CRT_CLK_VE3_BPU,
		.gate_ofs      = CLK_OFS_INVALID,
		.mux_ofs       = 0x04C,
		.mux_width     = 3,
		.mux_shift     = 9,
		.parent_names  = ve_parents,
		.num_parents   = ARRAY_SIZE(ve_parents),
		.name          = "clk_ve3_bpu",
		.flags         = CLK_SET_RATE_PARENT | CLK_SET_RATE_NO_REPARENT,
	},
};


#define GATE_COMMON(_id, _name, _parent, _flags, _ofs, _shift) \
	CLK_GATE_DATA(_id, _name, _parent, _flags, _ofs, _shift, 1, 0)
#define GATE_CRITICAL(_id, _name, _parent, _ofs, _shift) \
	GATE_COMMON(_id, _name, _parent, CLK_IS_CRITICAL, _ofs, _shift)
#define GATE(_id, _name, _parent, _ofs, _shift) \
	GATE_COMMON(_id, _name, _parent, 0, _ofs, _shift)
#define GATE_IGNORE(_id, _name, _parent, _ofs, _shift) \
	GATE_COMMON(_id, _name, _parent, CLK_IGNORE_UNUSED, _ofs, _shift)
#define GATE_SET_RATE_PARENT(_id, _name, _parent, _ofs, _shift) \
	GATE_COMMON(_id, _name, _parent, CLK_SET_RATE_PARENT,  _ofs, _shift)

static struct clk_gate_data cc_gates[] = {
	GATE_CRITICAL(RTD1319_CRT_CLK_EN_MISC, "misc", "osc27m", 0x50, 0),
	GATE(RTD1319_CRT_CLK_EN_PCIE0, "pcie0", NULL, 0x50, 2),
	GATE(RTD1319_CRT_CLK_EN_GSPI, "gspi", "misc", 0x50, 6),
	GATE(RTD1319_CRT_CLK_EN_SDS, "sds", NULL, 0x50, 12),
	GATE_IGNORE(RTD1319_CRT_CLK_EN_HDMI, "hdmi", NULL, 0x50, 14),
	GATE(RTD1319_CRT_CLK_EN_LSADC, "lsadc", NULL, 0x50, 28),
	GATE_IGNORE(RTD1319_CRT_CLK_EN_CP, "cp", NULL, 0x54, 2),
	GATE_CRITICAL(RTD1319_CRT_CLK_EN_TP, "tp", NULL, 0x54, 6),
	GATE(RTD1319_CRT_CLK_EN_RSA, "rsa", NULL, 0x54, 8),
	GATE(RTD1319_CRT_CLK_EN_NF, "nf", NULL, 0x54, 10),
	GATE(RTD1319_CRT_CLK_EN_EMMC, "emmc", NULL, 0x54, 12),
	GATE(RTD1319_CRT_CLK_EN_SD, "sd", NULL, 0x54, 14),
	GATE(RTD1319_CRT_CLK_EN_SDIO_IP, "sdio_ip", NULL, 0x54, 16),
	GATE(RTD1319_CRT_CLK_EN_MIPI, "mipi", NULL, 0x54, 18),
	GATE_SET_RATE_PARENT(RTD1319_CRT_CLK_EN_EMMC_IP, "emmc_ip", "pll_emmc", 0x54, 20),
	GATE(RTD1319_CRT_CLK_EN_SDIO, "sdio", NULL, 0x54, 22),
	GATE(RTD1319_CRT_CLK_EN_SD_IP, "sd_ip", NULL, 0x54, 24),
	GATE(RTD1319_CRT_CLK_EN_CABLERX, "cablerx", NULL, 0x54, 26),
	GATE(RTD1319_CRT_CLK_EN_TPB, "tpb", NULL, 0x54, 28),
	GATE(RTD1319_CRT_CLK_EN_MISC_SC1, "sc1", "misc", 0x54, 30),
	GATE(RTD1319_CRT_CLK_EN_MISC_I2C_3, "i2c3", "misc", 0x58, 0),
	GATE(RTD1319_CRT_CLK_EN_JPEG, "jpeg", NULL, 0x58, 4),
	GATE(RTD1319_CRT_CLK_EN_MISC_SC0, "sc0", "misc", 0x58, 10),
	GATE(RTD1319_CRT_CLK_EN_HDMIRX, "hdmirx", NULL, 0x58, 26),
	GATE(RTD1319_CRT_CLK_EN_HSE, "hse", NULL, 0x58, 28),
	GATE(RTD1319_CRT_CLK_EN_UR2, "ur2", "clk432m", 0x58, 30),
	GATE(RTD1319_CRT_CLK_EN_UR1, "ur1", "clk432m", 0x5C, 0),
	GATE(RTD1319_CRT_CLK_EN_FAN, "fan", NULL, 0x5C, 2),
	GATE(RTD1319_CRT_CLK_EN_SATA_WRAP_SYS, "sata_wrap_sys", NULL, 0x5C, 8),
	GATE(RTD1319_CRT_CLK_EN_SATA_WRAP_SYSH, "sata_wrap_sysh", NULL, 0x5C, 10),
	GATE(RTD1319_CRT_CLK_EN_SATA_MAC_SYSH, "sata_mac_sysh", NULL, 0x5C, 12),
	GATE(RTD1319_CRT_CLK_EN_R2RDSC, "r2rdsc", NULL, 0x5C, 14),
	GATE(RTD1319_CRT_CLK_EN_TPC, "tpc", NULL, 0x5C, 16),
	GATE(RTD1319_CRT_CLK_EN_PCIE1, "pcie1", NULL, 0x5C, 18),
	GATE(RTD1319_CRT_CLK_EN_MISC_I2C_4, "i2c4", "misc", 0x5C, 20),
	GATE(RTD1319_CRT_CLK_EN_MISC_I2C_5, "i2c5", "misc", 0x5C, 22),
	GATE(RTD1319_CRT_CLK_EN_EDP, "edp", NULL, 0x5C, 28),
	GATE(RTD1319_CRT_CLK_EN_PCIE2, "pcie2", NULL, 0x8C, 0),
};

static struct rtk_reset_bank cc_reset_banks[] = {
	{ .ofs = 0x000, .write_en = 1, },
	{ .ofs = 0x004, .write_en = 1, },
	{ .ofs = 0x008, .write_en = 1, },
	{ .ofs = 0x00C, .write_en = 1, },
	{ .ofs = 0x068, .write_en = 1, },
	{ .ofs = 0x090, .write_en = 1, },
	{ .ofs = 0x454, },
	{ .ofs = 0x458, },
	{ .ofs = 0x464, },
};

static struct rtk_reset_initdata cc_reset_initdata = {
	.banks     = cc_reset_banks,
	.num_banks = ARRAY_SIZE(cc_reset_banks),
};

static struct clk_hw_map rtd1319_sys_map = {
	.group = &(struct clk_hw_group) {
		.hws = cc_hws,
		.num_hws = ARRAY_SIZE(cc_hws),
	},
	.start_index = 0,
};

static int rtd1319_cc_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	struct device_node *np = dev->of_node;
	struct rtk_clk_data *data;
	int ret;
	const struct clk_hw_map *map;

	map = of_device_get_match_data(dev);
	if (!map)
		return -EINVAL;

	data = rtk_clk_alloc_data(RTD1319_CRT_CLK_MAX);
	if (!data)
		return -ENOMEM;

	ret = rtk_clk_of_init_data(np, data);
	if (ret) {
		rtk_clk_free_data(data);
		return ret;
	}

	platform_set_drvdata(pdev, data);

	rtk_clk_add_hw_map(dev, data, map);

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

static const struct of_device_id rtd1319_cc_match[] = {
	{ .compatible = "realtek,rtd1319-crt-clk",    .data = &rtd1319_sys_map, },
	{ /* sentinel */ }
};
MODULE_DEVICE_TABLE(of, rtd1319_cc_match);

static struct platform_driver rtd1319_cc_driver = {
	.probe = rtd1319_cc_probe,
	.driver = {
		.name = "rtk-rtd1319-crt-clk",
		.of_match_table = of_match_ptr(rtd1319_cc_match),
	},
};

static int __init rtd1319_cc_init(void)
{
	return platform_driver_register(&rtd1319_cc_driver);
}
subsys_initcall(rtd1319_cc_init);
MODULE_DESCRIPTION("Reatek RTD1319 CRT Controller Driver");
MODULE_AUTHOR("Cheng-Yu Lee <cylee12@realtek.com>");
MODULE_LICENSE("GPL v2");

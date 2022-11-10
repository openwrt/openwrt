// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (C) 2021 Realtek Semiconductor Corporation
 * Author: Cheng-Yu Lee <cylee12@realtek.com>
 */

#include <linux/clk-provider.h>
#include <linux/module.h>
#include "common.h"
#include "clk-pll.h"

static inline uint32_t get_phrt0(struct clk_pll_mmc *clkm)
{
	return (clk_regmap_read(&clkm->clkr, clkm->pll_ofs) >> 1) & 0x1;
}

static inline void set_phrt0(struct clk_pll_mmc *clkm, uint32_t val)
{
	clk_regmap_update(&clkm->clkr, clkm->pll_ofs, 0x00000002, val << 1);
}

static inline uint32_t get_phsel(struct clk_pll_mmc *clkm, int id)
{
	uint32_t sft = id ? 8 : 3;

	return (clk_regmap_read(&clkm->clkr, clkm->pll_ofs) >> sft) & 0x1f;
}

static inline void set_phsel(struct clk_pll_mmc *clkm, int id, uint32_t val)
{
	uint32_t sft = id ? 8 : 3;
	//printk(KERN_ERR "%s: sft=%d, id=%d, val=0x%x\n", __func__, sft, id, val);
	clk_regmap_update(&clkm->clkr, clkm->pll_ofs, 0x1f << sft, val << sft);
}

static inline void set_sscpll_rs(struct clk_pll_mmc *clkm, uint32_t val)
{
	clk_regmap_update(&clkm->clkr, clkm->pll_ofs + 4, 0x00001c00, val << 10);
}

static inline void set_sscpll_icp(struct clk_pll_mmc *clkm, uint32_t val)
{
	clk_regmap_update(&clkm->clkr, clkm->pll_ofs + 4, 0x000003e0, val << 5);
}

static inline void set_ssc_div_ext_f(struct clk_pll_mmc *clkm, uint32_t val)
{
        clk_regmap_update(&clkm->clkr, clkm->pll_ofs + 4, 0x03ffe000, val << 13);
}
static inline void set_pi_ibselh(struct clk_pll_mmc *clkm, uint32_t val)
{
        clk_regmap_update(&clkm->clkr, clkm->pll_ofs + 4, 0x18000000, val << 27);
}
static inline void set_ssc_div_n(struct clk_pll_mmc *clkm, uint32_t val)
{
	clk_regmap_update(&clkm->clkr, clkm->pll_ofs + 8, 0x03ff0000, val << 16);
}
static inline void set_ssc_div_n_2(struct clk_pll_mmc *clkm, uint32_t val)
{
        clk_regmap_update(&clkm->clkr, clkm->pll_ofs + 8, 0x00ff0000, val << 16);
}
static inline uint32_t get_ssc_div_n(struct clk_pll_mmc *clkm)
{
	return (clk_regmap_read(&clkm->clkr, clkm->pll_ofs + 8) >> 16) & 0x3ff;
}

static inline void set_pow_ctl(struct clk_pll_mmc *clkm, uint32_t val)
{
	clk_regmap_write(&clkm->clkr, clkm->pll_ofs + 0xc, val);
}

static inline uint32_t get_pow_ctl(struct clk_pll_mmc *clkm)
{
	return clk_regmap_read(&clkm->clkr, clkm->pll_ofs + 0xc);
}

static int clk_pll_mmc_phase_set_phase(struct clk_hw *hw, int degrees)
{
	struct clk_hw *hwp = clk_hw_get_parent(hw);
	struct clk_pll_mmc *clkm = to_clk_pll_mmc(hwp);
	int phase_id = (hw - &clkm->phase0_hw) ? 1 : 0;
	uint32_t val;

	val = DIV_ROUND_CLOSEST(degrees * 100, 1125);
	set_phsel(clkm, phase_id, val);

	//pr_err("%pC: %s: id=%d, degrees=%d, reg=%08x\n", hw->clk, __func__, phase_id, degrees, val);
	udelay(10);
	return 0;
}

static int clk_pll_mmc_phase_get_phase(struct clk_hw *hw)
{
	struct clk_hw *hwp = clk_hw_get_parent(hw);
	struct clk_pll_mmc *clkm = to_clk_pll_mmc(hwp);
	int phase_id = (hw - &clkm->phase0_hw) ? 1 : 0;
	uint32_t val;

	val = get_phsel(clkm, phase_id);
	val = DIV_ROUND_CLOSEST(val * 360, 32);

	pr_debug("%pC: %s: id=%d, degrees=%d\n", hw->clk, __func__, phase_id, val);
	return val;
}

const struct clk_ops clk_pll_mmc_phase_ops = {
	.set_phase = clk_pll_mmc_phase_set_phase,
	.get_phase = clk_pll_mmc_phase_get_phase,
};
EXPORT_SYMBOL_GPL(clk_pll_mmc_phase_ops);

static int clk_pll_mmc_prepare(struct clk_hw *hw)
{
	struct clk_pll_mmc *clkm = to_clk_pll_mmc(hw);

	set_pow_ctl(clkm, 7);
	return 0;
}

static void clk_pll_mmc_unprepare(struct clk_hw *hw)
{
	struct clk_pll_mmc *clkm = to_clk_pll_mmc(hw);

	set_pow_ctl(clkm, 0);
}

static int clk_pll_mmc_is_prepared(struct clk_hw *hw)
{
	struct clk_pll_mmc *clkm = to_clk_pll_mmc(hw);

	return get_pow_ctl(clkm) != 0x0;
}

static void clk_pll_mmc_unprepare_unused(struct clk_hw *hw)
{
	pr_err("%s\n", __func__);
}

static int clk_pll_mmc_enable(struct clk_hw *hw)
{
	struct clk_pll_mmc *clkm = to_clk_pll_mmc(hw);

	set_phrt0(clkm, 1);
	udelay(10);
	return 0;
}

static void clk_pll_mmc_disable(struct clk_hw *hw)
{
	struct clk_pll_mmc *clkm = to_clk_pll_mmc(hw);

	set_phrt0(clkm, 0);
	udelay(10);
}

static int clk_pll_mmc_is_enabled(struct clk_hw *hw)
{
	struct clk_pll_mmc *clkm = to_clk_pll_mmc(hw);

	return get_phrt0(clkm) == 0x1;
}

static void clk_pll_mmc_disable_unused(struct clk_hw *hw)
{
	pr_err("%s\n", __func__);
}

static unsigned long clk_pll_mmc_recalc_rate(struct clk_hw *hw, unsigned long parent_rate)
{
	struct clk_pll_mmc *clkm = to_clk_pll_mmc(hw);
	uint32_t val = get_ssc_div_n(clkm);

	return parent_rate / 4 * (val + 3);
}

static int clk_pll_mmc_set_rate(struct clk_hw *hw, unsigned long rate,
				unsigned long parent_rate)
{
	struct clk_pll_mmc *clkm = to_clk_pll_mmc(hw);
	/* In Realtek platform, always use 0xa6 fixed pll and use variant
	 * divider to calculate the wanted clk rate
	 */
	uint32_t val = 0xa6;

	set_pow_ctl(clkm, 6);
	set_ssc_div_n(clkm, val);

	switch (val) {
	case 53 ... 97:
		set_sscpll_rs(clkm, 5);
		if (clkm->set_rate_val_53_97_set_ipc)
			set_sscpll_icp(clkm, 0);
		break;
	case 98 ... 237:
		set_sscpll_rs(clkm, 5);
		set_sscpll_icp(clkm, 1);
		break;
	}
	set_pow_ctl(clkm, 7);
	udelay(60);

	pr_err("%pC: %s: pll_emmc1=0x%x, pll_emmc2=0x%x, pll_emmc3=0x%x, pll_emmc4=0x%x\n",
		hw->clk,
		__func__,
		clk_regmap_read(&clkm->clkr, clkm->pll_ofs),
		clk_regmap_read(&clkm->clkr, clkm->pll_ofs + 4),
		clk_regmap_read(&clkm->clkr, clkm->pll_ofs + 8),
		clk_regmap_read(&clkm->clkr, clkm->pll_ofs + 0xc));

	return 0;
}

static long clk_pll_mmc_round_rate(struct clk_hw *hw, unsigned long rate,
				   unsigned long *parent_rate)
{
	uint32_t val = DIV_ROUND_CLOSEST(rate * 4, *parent_rate);

	return *parent_rate * val / 4;
}

const struct clk_ops clk_pll_mmc_ops = {
	.prepare          = clk_pll_mmc_prepare,
	.unprepare        = clk_pll_mmc_unprepare,
	.is_prepared      = clk_pll_mmc_is_prepared,
	.unprepare_unused = clk_pll_mmc_unprepare_unused,
	.enable           = clk_pll_mmc_enable,
	.disable          = clk_pll_mmc_disable,
	.is_enabled       = clk_pll_mmc_is_enabled,
	.disable_unused   = clk_pll_mmc_disable_unused,
	.recalc_rate      = clk_pll_mmc_recalc_rate,
	.set_rate         = clk_pll_mmc_set_rate,
	.round_rate       = clk_pll_mmc_round_rate,
};
EXPORT_SYMBOL_GPL(clk_pll_mmc_ops);

static int clk_pll_mmc_v2_set_rate(struct clk_hw *hw, unsigned long rate,
                            unsigned long parent_rate)
{
        struct clk_pll_mmc *clkm = to_clk_pll_mmc(hw);
        uint32_t val = 0x1a;

	clk_regmap_update(&clkm->clkr, clkm->ssc_dig_ofs, 0x100, 0x0 << 8);

	// 4. Set PLL_EMMC3_ssc_div_n, PLL_EMMC3_ssc_div_ext_f
	set_ssc_div_n_2(clkm, val);
	set_ssc_div_ext_f(clkm, 0);

	switch (val) {
        case 31 ... 46:
		set_pi_ibselh(clkm, 3);
                set_sscpll_rs(clkm, 3);
		set_sscpll_icp(clkm, 2);
                break;
	case 20 ... 30:
                set_pi_ibselh(clkm, 2);
                set_sscpll_rs(clkm, 3);
                set_sscpll_icp(clkm, 1);
                break;
	case 10 ... 19:
                set_pi_ibselh(clkm, 1);
                set_sscpll_rs(clkm, 2);
                set_sscpll_icp(clkm, 1);
                break;
        case 5 ... 9:
		set_pi_ibselh(clkm, 0);
                set_sscpll_rs(clkm, 2);
                set_sscpll_icp(clkm, 0);
                break;
        }

	clk_regmap_update(&clkm->clkr, clkm->ssc_dig_ofs + 0xc, 0x1fe000, 25 << 13);
	clk_regmap_update(&clkm->clkr, clkm->ssc_dig_ofs + 0xc, 0x1fff, 5898);
        clk_regmap_update(&clkm->clkr, clkm->ssc_dig_ofs + 0x10, 0x1fffff, 11485);
        clk_regmap_update(&clkm->clkr, clkm->ssc_dig_ofs, 0x2, 0x1 << 1	);

	//5.6.7.
	clk_regmap_update(&clkm->clkr, clkm->ssc_dig_ofs, 0x1, 0x1);
	clk_regmap_update(&clkm->clkr, clkm->ssc_dig_ofs, 0x1, 0x0);
	clk_regmap_update(&clkm->clkr, clkm->ssc_dig_ofs, 0x100, 0x1 << 8);
	udelay(10);

	pr_err("%pC: %s: pll_emmc1=0x%x, pll_emmc2=0x%x, pll_emmc3=0x%x, pll_emmc4=0x%x\n",
                hw->clk,
                __func__,
                clk_regmap_read(&clkm->clkr, clkm->pll_ofs),
                clk_regmap_read(&clkm->clkr, clkm->pll_ofs + 4),
                clk_regmap_read(&clkm->clkr, clkm->pll_ofs + 8),
                clk_regmap_read(&clkm->clkr, clkm->pll_ofs + 0xc));

	return 0;
}

// FIXME
const struct clk_ops clk_pll_mmc_v2_ops = {
	.prepare          = clk_pll_mmc_prepare,
	.unprepare        = clk_pll_mmc_unprepare,
	.is_prepared      = clk_pll_mmc_is_prepared,
	.unprepare_unused = clk_pll_mmc_unprepare_unused,
	.enable           = clk_pll_mmc_enable,
	.disable          = clk_pll_mmc_disable,
	.is_enabled       = clk_pll_mmc_is_enabled,
	.disable_unused   = clk_pll_mmc_disable_unused,
	.recalc_rate      = clk_pll_mmc_recalc_rate,
	.set_rate         = clk_pll_mmc_v2_set_rate,
	.round_rate       = clk_pll_mmc_round_rate,
};
EXPORT_SYMBOL_GPL(clk_pll_mmc_v2_ops);
MODULE_LICENSE("GPL v2");

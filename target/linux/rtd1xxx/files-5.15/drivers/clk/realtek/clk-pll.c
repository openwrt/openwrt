// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (C) 2017-2018 Realtek Semiconductor Corporation
 * Author: Cheng-Yu Lee <cylee12@realtek.com>
 */

#include <linux/io.h>
#include <linux/slab.h>
#include <linux/clk-provider.h>
#include <linux/clk.h>
#include <linux/spinlock.h>
#include <linux/delay.h>
#include "common.h"
#include "clk-pll.h"

static inline int __clk_pll_get_pow(struct clk_pll *clkp);

static inline int __clk_pll_set_oc_en(struct clk_pll *clkp)
{
	uint32_t val;
	uint32_t pollval;

	val = clk_regmap_read(&clkp->clkr, clkp->ssc_ofs + 0x0);
	if ((val & 0x7) == 0x5)
		return 0;
	clk_regmap_update(&clkp->clkr, clkp->ssc_ofs + 0x0, 0x7, 0x5);

	return regmap_read_poll_timeout(clkp->clkr.regmap, clkp->ssc_ofs + 0x1c,
		pollval, pollval & BIT(20), 0, 2000);
}

static inline int __clk_pll_set_pow_on_common(struct clk_pll *clkp)
{
	struct clk_regmap *clkr = &clkp->clkr;
	uint32_t pow = clk_pll_get_pow_offset(clkp);

	if (clkp->pow_set_rs)
		clk_regmap_update(clkr, clkp->pll_ofs, clkp->rs_mask, clkp->rs_val);

	clk_regmap_update(clkr, clkp->pll_ofs + pow, 0x7, 0x5);
	clk_regmap_update(clkr, clkp->pll_ofs + pow, 0x7, 0x7);

	if (clkp->pow_set_analog)
		clk_regmap_update(clkr, clkp->pll_ofs, clkp->analog_mask, clkp->analog_val);

	udelay(200);

	if (clkp->pow_set_pi_bps)
		clk_regmap_update(clkr, clkp->pll_ofs, 0x10, 0);

	clk_regmap_update(clkr, clkp->pll_ofs + pow, 0x7, 0x3);

	return clkp->pll_type == CLK_PLL_TYPE_NF_SSC ? __clk_pll_set_oc_en(clkp) : 0;
}

static inline int __clk_pll_set_pow_on_mno_v2(struct clk_pll *clkp)
{
	struct clk_regmap *clkr = &clkp->clkr;
	uint32_t pow = clk_pll_get_pow_offset(clkp);
	uint32_t val = 0, mask = 0;

	clk_regmap_update(clkr, clkp->pll_ofs + pow, 0x7, 0x5);
	clk_regmap_update(clkr, clkp->pll_ofs + pow, 0x7, 0x7);

	if (clkp->pow_set_rs) {
		val  |= clkp->rs_val;
		mask |= clkp->rs_mask;
	}

	if (clkp->freq_val) {
		val  |= clkp->freq_val;
		mask |= clk_pll_get_freq_mask(clkp);
	}

	if (mask)
		clk_regmap_update(clkr, clkp->pll_ofs, mask, val);
	udelay(200);

	clk_regmap_update(clkr, clkp->pll_ofs + pow, 0x7, 0x3);

	return 0;
}

static inline int __clk_pll_set_pow_on(struct clk_pll *clkp)
{
	/* already powered */
	if (__clk_pll_get_pow(clkp) > 0)
		return 0;

	if (clkp->pll_type == CLK_PLL_TYPE_MNO_V2)
		return __clk_pll_set_pow_on_mno_v2(clkp);

	return __clk_pll_set_pow_on_common(clkp);
}

static inline int __clk_pll_set_pow_off(struct clk_pll *clkp)
{
	struct clk_regmap *clkr = &clkp->clkr;
	uint32_t pow = clk_pll_get_pow_offset(clkp);

	clk_regmap_update(clkr, clkp->pll_ofs + pow, 0x7, 0x4);
	if (clkp->pll_type != CLK_PLL_TYPE_NF_SSC)
		return 0;

	if (clkp->pow_set_pi_bps)
		clk_regmap_update(clkr, clkp->pll_ofs, 0x10, 0x10);
	return 0;
}

static inline int __clk_pll_get_pow(struct clk_pll *clkp)
{
	uint32_t pow = clk_pll_get_pow_offset(clkp);
	uint32_t val;

	if (!clk_pll_has_pow(clkp))
		return -EINVAL;

	pow = clk_pll_get_pow_offset(clkp);
	val = clk_regmap_read(&clkp->clkr, clkp->pll_ofs + pow);
	return !!(val & 0x1);
}

static uint32_t __clk_pll_freq_raw_get(struct clk_pll *clkp)
{
	uint32_t val = 0;

	switch (clkp->pll_type) {
	case CLK_PLL_TYPE_MNO_V2:
		if (clkp->freq_val) {
			val = clkp->freq_val;
			break;
		}
		fallthrough;
	case CLK_PLL_TYPE_MNO:
		val = clk_regmap_read(&clkp->clkr, clkp->pll_ofs + 0x0);
		break;

	case CLK_PLL_TYPE_NF:
		val = clk_regmap_read(&clkp->clkr, clkp->pll_ofs + 0x4);
		break;

	case CLK_PLL_TYPE_NF_SSC:
		val = clk_regmap_read(&clkp->clkr, clkp->ssc_ofs + 0x4);
		break;

	default:
		break;
	}
	return val;
}

static inline int __clk_pll_freq_set(struct clk_pll *clkp, uint32_t val)
{
	int ret = 0;
	uint32_t mask = clk_pll_get_freq_mask(clkp);

	switch (clkp->pll_type) {
	case CLK_PLL_TYPE_MNO:
		clk_regmap_update(&clkp->clkr, clkp->pll_ofs, mask, val);
		break;

	case CLK_PLL_TYPE_MNO_V2:
		clkp->freq_val = val;
		break;

	case CLK_PLL_TYPE_NF:
		clk_regmap_update(&clkp->clkr, clkp->pll_ofs + 0x4, mask, val);
		if (__clk_pll_get_pow(clkp) > 0) {
			__clk_pll_set_pow_off(clkp);
			__clk_pll_set_pow_on(clkp);
		}
		break;

	case CLK_PLL_TYPE_NF_SSC:
		clk_regmap_update(&clkp->clkr, clkp->ssc_ofs + 0x0, 0x7, 0x4);
		clk_regmap_update(&clkp->clkr, clkp->ssc_ofs + 0x4, mask, val);

		if (__clk_pll_get_pow(clkp) == 0)
			break;

		ret = __clk_pll_set_oc_en(clkp);
		break;

	default:
		ret = -EINVAL;
		break;
	}
	return ret;
}

static inline int __clk_pll_get_plldiv(struct clk_pll *clkp)
{
	uint32_t val;

	if (clkp->pll_type != CLK_PLL_TYPE_NF_SSC)
		return 1;

	if (!CLK_OFS_IS_VALID(clkp->pll_ofs))
		return 1;

	val = (clk_regmap_read(&clkp->clkr, clkp->pll_ofs) >> 22) & 0x7;
	return val + 1;
}

static int clk_pll_enable(struct clk_hw *hw)
{
	struct clk_pll *clkp = to_clk_pll(hw);
	int ret = 0;

	if (clk_pll_has_pow(clkp))
		ret = __clk_pll_set_pow_on(clkp);
	if (ret)
		pr_warn("%pC: error in %s: %d\n", hw->clk, __func__, ret);
	return 0;
}

static void clk_pll_disable(struct clk_hw *hw)
{
	struct clk_pll *clkp = to_clk_pll(hw);
	int ret = 0;

	if (clk_pll_has_pow(clkp))
		ret = __clk_pll_set_pow_off(clkp);
	if (ret)
		pr_warn("%pC: error in %s: %d\n", hw->clk, __func__, ret);
}

static void clk_pll_disable_unused(struct clk_hw *hw)
{
	pr_info("%pC: %s\n", hw->clk, __func__);
	clk_pll_disable(hw);
}

static int clk_pll_is_enabled(struct clk_hw *hw)
{
	struct clk_pll *clkp = to_clk_pll(hw);

	return __clk_pll_get_pow(clkp);
}

static long clk_pll_round_rate(struct clk_hw *hw, unsigned long rate,
			       unsigned long *parent_rate)
{
	struct clk_pll *clkp = to_clk_pll(hw);
	const struct freq_table *ftblv = NULL;
	int plldiv = __clk_pll_get_plldiv(clkp);

	ftblv = ftbl_find_by_rate(clkp->freq_tbl, rate * plldiv);
	return ftblv ? ftblv->rate / plldiv : 0;
}

static unsigned long clk_pll_recalc_rate(struct clk_hw *hw,
					 unsigned long parent_rate)
{
	struct clk_pll *clkp = to_clk_pll(hw);
	unsigned long flags = 0;
	const struct freq_table *fv;
	uint32_t raw;
	int plldiv = __clk_pll_get_plldiv(clkp);

	flags = clk_pll_lock(clkp);
	raw = __clk_pll_freq_raw_get(clkp);
	clk_pll_unlock(clkp, flags);

	fv = ftbl_find_by_val_with_mask(clkp->freq_tbl, clkp->freq_mask, raw);
	return fv ? fv->rate / plldiv : 0;
}

static int clk_pll_set_rate(struct clk_hw *hw, unsigned long rate,
			    unsigned long parent_rate)
{
	struct clk_pll *clkp = to_clk_pll(hw);
	unsigned long flags = 0;
	const struct freq_table *fv;
	int ret = 0;
	int plldiv = __clk_pll_get_plldiv(clkp);

	fv = ftbl_find_by_rate(clkp->freq_tbl, rate * plldiv);
	if (!fv)
		return -EINVAL;

	pr_debug("%pC: %s: tbl_rate=%ld, reg_val=0x%08x, plldiv=%d, target_rate=%ld\n", hw->clk, __func__,
		 fv->rate, fv->val, plldiv, rate);

	flags = clk_pll_lock(clkp);
	ret = __clk_pll_freq_set(clkp, fv->val);
	clk_pll_unlock(clkp, flags);
	if (ret)
		pr_warn("%pC %s: failed to set freq: %d\n", hw->clk, __func__, ret);
	return 0;
}

static void __clk_pll_div_set(struct clk_pll_div *clkpd, uint32_t val)
{
	uint32_t m = (BIT(clkpd->div_width) - 1) << clkpd->div_shift;
	uint32_t s = clkpd->div_shift;

	clk_regmap_update(&clkpd->clkp.clkr, clkpd->div_ofs, m, val << s);
}

static uint32_t __clk_pll_div_get(struct clk_pll_div *clkpd)
{
	uint32_t m = (BIT(clkpd->div_width) - 1) << clkpd->div_shift;
	uint32_t s = clkpd->div_shift;

	return (clk_regmap_read(&clkpd->clkp.clkr, clkpd->div_ofs) & m) >> s;
}

static long clk_pll_div_round_rate(struct clk_hw *hw, unsigned long rate,
				   unsigned long *parent_rate)
{
	struct clk_pll_div *clkpd = to_clk_pll_div(hw);
	const struct div_table *dv;

	/* lookup div in dtbl */
	dv = dtbl_find_by_rate(clkpd->div_tbl, rate);
	if (!dv)
		return 0;

	rate *= dv->div;
	rate = clk_pll_round_rate(hw, rate, parent_rate);
	return rate / dv->div;
}

static unsigned long clk_pll_div_recalc_rate(struct clk_hw *hw,
					     unsigned long parent_rate)
{
	struct clk_pll_div *clkpd = to_clk_pll_div(hw);
	unsigned long rate;
	const struct div_table *dv;
	uint32_t val;

	rate = clk_pll_recalc_rate(hw, parent_rate);

	val = __clk_pll_div_get(clkpd);
	dv = dtbl_find_by_val(clkpd->div_tbl, val);
	if (!dv)
		return 0;

	rate /= dv->div;
	pr_debug("%pC: %s: current rate=%lu, div=%d, reg_val=0x%x\n",
		 hw->clk, __func__, rate, dv->div, val);

	return rate;
}

static int clk_pll_div_set_rate(struct clk_hw *hw, unsigned long rate,
				unsigned long parent_rate)
{
	struct clk *clk = hw->clk;
	struct clk_pll_div *clkpd = to_clk_pll_div(hw);
	unsigned long flags;
	const struct div_table *ndv, *cdv;
	unsigned long target;
	uint32_t cur_d;
	int ret;

	/* find next in the dtbl */
	ndv = dtbl_find_by_rate(clkpd->div_tbl, rate);
	if (!ndv)
		return -EINVAL;

	target = rate * ndv->div;

	/* find current in the dtbl */
	cur_d = __clk_pll_div_get(clkpd);
	cdv = dtbl_find_by_val(clkpd->div_tbl, cur_d);
	if (!cdv)
		return -EINVAL;

	pr_debug("%pC: rate=%lu, cdv={%d,0x%x}, ndv={%d,0x%x}\n",
		 clk, rate, cdv->div, cdv->val, ndv->div, ndv->val);

	flags = clk_pll_div_lock(clkpd);

	/* workaround to prevent glitch */
#ifdef CONFIG_COMMON_CLK_RTD129X
	if ((&clkpd->clkp.flags & CLK_PLL_DIV_WORKAROUND) &&
		ndv->val != cdv->val && (ndv->val == 1 || cdv->val == 1)) {

		pr_debug("%pC: apply rate=%u\n", clk, 1000000000);
		clk_pll_set_rate(hw, 1000000000, parent_rate);

		pr_debug("%pC: apply dv={%d, 0x%x}\n", clk, ndv->div, ndv->val);
		__clk_pll_div_set(clkpd, ndv->val);
		cdv = ndv;
	}
#endif

	if (ndv->div > cdv->div)
		__clk_pll_div_set(clkpd, ndv->val);
	ret = clk_pll_set_rate(hw, target, parent_rate);
	if (ndv->div < cdv->div)
		__clk_pll_div_set(clkpd, ndv->val);

	clk_pll_div_unlock(clkpd, flags);

	return ret;
}

const struct clk_ops clk_pll_ops = {
	.round_rate       = clk_pll_round_rate,
	.recalc_rate      = clk_pll_recalc_rate,
	.set_rate         = clk_pll_set_rate,
	.enable           = clk_pll_enable,
	.disable          = clk_pll_disable,
	.disable_unused   = clk_pll_disable_unused,
	.is_enabled       = clk_pll_is_enabled,
};
EXPORT_SYMBOL_GPL(clk_pll_ops);

const struct clk_ops clk_pll_div_ops = {
	.round_rate       = clk_pll_div_round_rate,
	.recalc_rate      = clk_pll_div_recalc_rate,
	.set_rate         = clk_pll_div_set_rate,
	.enable           = clk_pll_enable,
	.disable          = clk_pll_disable,
	.disable_unused   = clk_pll_disable_unused,
	.is_enabled       = clk_pll_is_enabled,
};
EXPORT_SYMBOL_GPL(clk_pll_div_ops);

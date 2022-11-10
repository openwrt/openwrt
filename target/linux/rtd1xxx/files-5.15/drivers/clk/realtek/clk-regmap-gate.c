// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (C) 2017 Realtek Semiconductor Corporation
 * Author: Cheng-Yu Lee <cylee12@realtek.com>
 */

#include <linux/slab.h>
#include <linux/clk-provider.h>
#include "clk-regmap-gate.h"

static int clk_regmap_gate_enable(struct clk_hw *hw)
{
	struct clk_regmap_gate *clkg = to_clk_regmap_gate(hw);
	unsigned long flags = 0;
	unsigned int mask;
	unsigned int val;

	pr_debug("%pC: clk enable\n", hw->clk);

	if (clkg->lock)
		spin_lock_irqsave(clkg->lock, flags);

	mask = BIT(clkg->bit_idx);
	val  = BIT(clkg->bit_idx);

	if (clkg->write_en) {
		mask |= BIT(clkg->bit_idx + 1);
		val  |= BIT(clkg->bit_idx + 1);
	}

	clk_regmap_update(&clkg->clkr, clkg->gate_ofs, mask, val);

	if (clkg->lock)
		spin_unlock_irqrestore(clkg->lock, flags);

	return 0;
}

static void clk_regmap_gate_disable(struct clk_hw *hw)
{
	struct clk_regmap_gate *clkg = to_clk_regmap_gate(hw);
	unsigned long flags = 0;
	unsigned int mask;
	unsigned int val;

	pr_debug("%pC: clk disable\n", hw->clk);

	if (clkg->lock)
		spin_lock_irqsave(clkg->lock, flags);

	mask = BIT(clkg->bit_idx);
	val  = 0;

	if (clkg->write_en) {
		mask |= BIT(clkg->bit_idx + 1);
		val  |= BIT(clkg->bit_idx + 1);
	}

	clk_regmap_update(&clkg->clkr, clkg->gate_ofs, mask, val);

	if (clkg->lock)
		spin_unlock_irqrestore(clkg->lock, flags);
}

static void clk_regmap_gate_disable_unused(struct clk_hw *hw)
{
	pr_info("%pC: %s\n", hw->clk, __func__);
	clk_regmap_gate_disable(hw);
}

static int clk_regmap_gate_is_enabled(struct clk_hw *hw)
{
	struct clk_regmap_gate *clkg = to_clk_regmap_gate(hw);
	int ret;
	unsigned long flags = 0;

	if (clkg->lock)
		spin_lock_irqsave(clkg->lock, flags);

	ret = clk_regmap_read(&clkg->clkr, clkg->gate_ofs) & BIT(clkg->bit_idx);

	if (clkg->lock)
		spin_unlock_irqrestore(clkg->lock, flags);

	return !!ret;
}

const struct clk_ops clk_regmap_gate_ops = {
	.enable = clk_regmap_gate_enable,
	.disable = clk_regmap_gate_disable,
	.disable_unused = clk_regmap_gate_disable_unused,
	.is_enabled = clk_regmap_gate_is_enabled,
};

const struct clk_ops clk_regmap_gate_ro_ops = {
	.is_enabled = clk_regmap_gate_is_enabled,
};

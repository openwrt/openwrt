// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (C) 2019 Realtek Semiconductor Corporation
 * Author: Cheng-Yu Lee <cylee12@realtek.com>
 */

#include <linux/clk.h>
#include <linux/clkdev.h>
#include <linux/clk-provider.h>
#include <linux/device.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/pm.h>
#include <linux/module.h>
#include "common.h"
#include "clk-pll.h"
#include "clk-regmap-gate.h"
#include "clk-regmap-mux.h"
#include <linux/mfd/syscon.h>
#include <linux/hwspinlock.h>

MODULE_LICENSE("GPL v2");

struct rtk_clk_data *rtk_clk_alloc_data(int clk_num)
{
	struct rtk_clk_data *data;

	data = kzalloc(sizeof(*data), GFP_KERNEL);
	if (!data)
		return NULL;

	data->clk_num = clk_num;
	data->clk_data.clk_num = clk_num;
	data->clk_data.clks = kcalloc(clk_num, sizeof(*data->clk_data.clks),
				      GFP_KERNEL);
	if (!data->clk_data.clks)
		goto free_data;
	return data;

free_data:
	kfree(data->clk_data.clks);
	kfree(data);
	return NULL;
}
EXPORT_SYMBOL_GPL(rtk_clk_alloc_data);

void rtk_clk_free_data(struct rtk_clk_data *data)
{
	kfree(data->clk_data.clks);
	kfree(data);
}
EXPORT_SYMBOL_GPL(rtk_clk_free_data);

int rtk_clk_of_init_data(struct device_node *np, struct rtk_clk_data *data)
{
	struct regmap *regmap;
	struct hwspinlock *lock;
	int ret;
	struct device_node *parent = of_get_parent(np);
	int lock_id;

	regmap = syscon_node_to_regmap(parent);
	of_node_put(parent);
	if (IS_ERR(regmap)) {
		ret = PTR_ERR(regmap);
		pr_err("%s: failed to get regmap form %s: %d\n", __func__,
			np->name, ret);
		return ret;
	}
	data->regmap = regmap;

	lock_id = of_hwspin_lock_get_id(np, 0);
	if (lock_id > 0 || (IS_ENABLED(CONFIG_HWSPINLOCK) && lock_id == 0)) {
		lock = hwspin_lock_request_specific(lock_id);
		if (lock) {
			pr_info("%s: %s: use hwlock%d\n", __func__, np->name, lock_id);
			data->lock = lock;
		} else
			pr_info("%s: %s: failed to request hwlock%d \n", __func__, np->name, lock_id);
	}

	return 0;
}
EXPORT_SYMBOL_GPL(rtk_clk_of_init_data);

static inline
int __cell_clk_add(struct clk_onecell_data *clk_data, int i, struct clk *clk)
{
	if (clk_data->clks[i]) {
		pr_err("%s: failed to add %pC, cell%d is used by %pC\n",
			__func__, clk, i, clk_data->clks[i]);
		return -EINVAL;
	}
	clk_data->clks[i] = clk;
	return 0;
}

struct clk_regmap_init_data {
	struct regmap *regmap;
	struct hwspinlock *lock;
	int shared;
};

static void clk_regmap_init(struct clk_regmap *clkr, struct clk_regmap_init_data *initdata)
{
	clkr->regmap = initdata->regmap;
	clkr->shared = initdata->shared;
	if (!initdata->shared)
		return;
	clkr->lock = initdata->lock;
	WARN_ON_ONCE(!clkr->lock);
}

static inline int is_clk_regmap_ops(const struct clk_ops *ops)
{
	if (is_clk_pll_ops(ops))
		return 1;

	if (is_clk_regmap_mux_ops(ops))
		return 1;

	if (is_clk_regmap_gate_ops(ops))
		return 1;

	return 0;
}

static
struct clk *rtk_clk_register_hw(struct device *dev, struct rtk_clk_data *data,
				struct clk_hw *hw)
{
	struct clk_regmap *clkr = to_clk_regmap(hw);
	struct clk_regmap_init_data initdata = {
		.regmap = data->regmap,
		.lock   = data->lock,
		.shared = clkr->shared,
	};

	clk_regmap_init(clkr, &initdata);

	return clk_register(dev, hw);
}

int rtk_clk_add_hws_from(struct device *dev, struct rtk_clk_data *data,
			 struct clk_hw **hws, int size, int start_index)
{
	struct clk_onecell_data *clk_data = &data->clk_data;
	int i, j;

	for (i = 0, j = start_index; i < size; i++, j++) {
		struct clk_hw *hw = hws[i];
		const char *name;
		struct clk *clk;

		if (IS_ERR_OR_NULL(hw))
			continue;

		name = hw->init->name;
		dev_dbg(dev, "%s: registering clk_hw '%s' (idx=%d) at slot%d\n",
			__func__, name, i, j);

		clk = rtk_clk_register_hw(dev, data, hw);
		if (IS_ERR(clk)) {
			pr_err("%s: failed to register clk_hw '%s' (idx=%d): %ld\n",
			       __func__, name, i, PTR_ERR(clk));
			continue;
		}

		clk_register_clkdev(clk, name, NULL);
		__cell_clk_add(clk_data, j, clk);
	}
	return 0;
}
EXPORT_SYMBOL_GPL(rtk_clk_add_hws_from);

static
struct clk *rtk_clk_register_composite(struct device *dev,
				       struct rtk_clk_data *data,
				       struct clk_composite_data *comp)
{
	struct clk_regmap_mux *clkm = NULL;
	const struct clk_ops *mux_op = NULL;
	struct clk_hw *mux_hw = NULL;
	struct clk_regmap_gate *clkg = NULL;
	const struct clk_ops *gate_op = NULL;
	struct clk_hw *gate_hw = NULL;
	struct clk_hw *hw;
	struct clk_regmap_init_data initdata = {
		.regmap = data->regmap,
		.lock   = data->lock,
		.shared = comp->shared,
	};

	if (comp->mux_ofs != CLK_OFS_INVALID) {
		clkm = kzalloc(sizeof(*clkm), GFP_KERNEL);
		if (!clkm) {
			hw = ERR_PTR(-ENOMEM);
			goto check_err;
		}

		clkm->mux_ofs     = comp->mux_ofs;
		clkm->mask        = BIT(comp->mux_width) - 1;
		clkm->shift       = comp->mux_shift;

		clk_regmap_init(&clkm->clkr, &initdata);

		mux_op = &clk_regmap_mux_ops;
		mux_hw = &__clk_regmap_mux_hw(clkm);
	}

	if (comp->gate_ofs != CLK_OFS_INVALID) {
		clkg = kzalloc(sizeof(*clkg), GFP_KERNEL);
		if (!clkg) {
			hw = ERR_PTR(-ENOMEM);
			goto check_err;
		}

		clkg->gate_ofs    = comp->gate_ofs;
		clkg->bit_idx     = comp->gate_shift;
		clkg->write_en    = comp->gate_write_en;

		clk_regmap_init(&clkg->clkr, &initdata);

		gate_op = &clk_regmap_gate_ops;
		gate_hw = &__clk_regmap_gate_hw(clkg);
	}

	hw = clk_hw_register_composite(NULL, comp->name, comp->parent_names,
				     comp->num_parents, mux_hw, mux_op,
				     NULL, NULL, gate_hw, gate_op, comp->flags);
check_err:
	if (IS_ERR(hw)) {
		kfree(clkm);
		kfree(clkg);
		return ERR_CAST(hw);
	}
	return hw->clk;
}

int rtk_clk_add_composites(struct device *dev, struct rtk_clk_data *data,
			   struct clk_composite_data *comps, int num)
{
	struct clk_onecell_data *clk_data = &data->clk_data;
	int i;

	for (i = 0; i < num; i++) {
		struct clk_composite_data *comp = &comps[i];
		const char *name = comp->name;
		struct clk *clk;

		clk = rtk_clk_register_composite(dev, data, comp);
		if (IS_ERR(clk)) {
			pr_err("%s: failed to add composite%d(%s): %ld\n",
				__func__, i, name, PTR_ERR(clk));
			continue;
		}

		clk_register_clkdev(clk, name, NULL);
		__cell_clk_add(clk_data, comp->id, clk);
	}

	return 0;
}
EXPORT_SYMBOL_GPL(rtk_clk_add_composites);

static
struct clk *rtk_clk_register_gate(struct device *dev, struct rtk_clk_data *data,
				  struct clk_gate_data *gate)
{
	struct clk_regmap_gate *clkg;
	struct clk_init_data init = { 0 };
	struct clk_hw *hw;
	struct clk_regmap_init_data initdata = {
		.regmap = data->regmap,
		.lock   = data->lock,
		.shared = gate->shared,
	};

	clkg = kzalloc(sizeof(*clkg), GFP_KERNEL);
	if (!clkg)
		return ERR_PTR(-ENOMEM);

	clkg->gate_ofs    = gate->gate_ofs;
	clkg->bit_idx     = gate->gate_shift;
	clkg->write_en    = gate->gate_write_en;

	clk_regmap_init(&clkg->clkr, &initdata);

	init.name         = gate->name;
	init.ops          = &clk_regmap_gate_ops;
	init.flags         = gate->flags;
	if (gate->parent) {
		init.parent_names = &gate->parent;
		init.num_parents  = 1;
	}

	hw = &__clk_regmap_gate_hw(clkg);
	hw->init = &init;
	return clk_register(dev, hw);
}

int rtk_clk_add_gates(struct device *dev, struct rtk_clk_data *data,
		      struct clk_gate_data *gates, int num)
{
	struct clk_onecell_data *clk_data = &data->clk_data;
	int i;

	for (i = 0; i < num; i++) {
		struct clk_gate_data *gate = &gates[i];
		const char *name = gate->name;
		struct clk *clk;

		clk = rtk_clk_register_gate(dev, data, gate);
		if (IS_ERR(clk)) {
			pr_err("%s: failed to add gate%d(%s): %ld\n", __func__,
				i, name, PTR_ERR(clk));
			continue;
		}

		clk_register_clkdev(clk, name, NULL);
		__cell_clk_add(clk_data, gate->id, clk);
	}

	return 0;
}
EXPORT_SYMBOL_GPL(rtk_clk_add_gates);

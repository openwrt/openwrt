/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * Copyright (C) 2016-2019 Realtek Semiconductor Corporation
 * Author: Cheng-Yu Lee <cylee12@realtek.com>
 */

#ifndef __CLK_REALTEK_COMMON_H
#define __CLK_REALTEK_COMMON_H

#include <linux/bitops.h>
#include <linux/clk-provider.h>
#include <linux/device.h>
#include <linux/io.h>
#include <linux/of.h>
#include <linux/regmap.h>
#include <linux/hwspinlock.h>

struct device;
struct platform_device;

struct clk_regmap {
	struct clk_hw hw;
	struct regmap *regmap;
	struct hwspinlock *lock;
	int shared;
};

#define to_clk_regmap(_hw) container_of(_hw, struct clk_regmap, hw)
#define __clk_regmap_hw(_p) ((_p)->hw)

#define CLK_REGMAP_LOCK_TIMEOUT_MS  2

/**
 * clk_regmap_lock() - lock a clk_regmap
 * @clkr: a clk_regmap
 *
 * Returns 1 when the lock  was successfully taken, 0 when the lock was ingored.
 */
static inline int clk_regmap_lock(struct clk_regmap *clkr)
{
	int ret;

	if (!clkr->shared || !clkr->lock)
		return 0;

	ret = hwspin_lock_timeout_raw(clkr->lock, CLK_REGMAP_LOCK_TIMEOUT_MS);
	if (ret)
		pr_warn("%s: hwlock ignored with %pe\n", __func__, ERR_PTR(ret));
	return ret == 0;
}

/**
 * clk_regmap_unlock() - unlock a clk_regmap
 * @clkr: a clk_regmap
 */
static inline void clk_regmap_unlock(struct clk_regmap *clkr, int locked)
{
	if (!clkr->shared || !clkr->lock || !locked)
		return;
	hwspin_unlock_raw(clkr->lock);
}

static inline
void clk_regmap_write(struct clk_regmap *clkr, uint32_t ofs, uint32_t val)
{
	int locked;

	pr_debug("%s: ofs=%03x, val=%08x\n", __func__, ofs, val);

	locked = clk_regmap_lock(clkr);
	regmap_write(clkr->regmap, ofs, val);
	clk_regmap_unlock(clkr, locked);
}

static inline
uint32_t clk_regmap_read(struct clk_regmap *clkr, uint32_t ofs)
{
	uint32_t val = 0;
	int locked;

	locked = clk_regmap_lock(clkr);
	regmap_read(clkr->regmap, ofs, &val);
	clk_regmap_unlock(clkr, locked);

	pr_debug("%s: ofs=%03x, val=%08x\n", __func__, ofs, val);
	return val;
}

static inline void clk_regmap_update(struct clk_regmap *clkr, uint32_t ofs,
				     uint32_t mask, uint32_t val)
{
	int locked;

	pr_debug("%s: ofs=%03x, mask=%08x, val=%08x\n", __func__, ofs,
		 mask, val);

	locked = clk_regmap_lock(clkr);
	regmap_update_bits(clkr->regmap, ofs, mask, val);
	clk_regmap_unlock(clkr, locked);
}

/* ofs check */
#define CLK_OFS_INVALID                 (-1)
#define CLK_OFS_IS_VALID(_ofs)          ((_ofs) != CLK_OFS_INVALID)

struct clk_composite_data {
	int id;
	const char *name;
	unsigned long flags;
	struct clk *clk;

	int gate_ofs;
	int gate_shift;
	int gate_write_en;

	int mux_ofs;
	int mux_width;
	int mux_shift;
	const char * const *parent_names;
	int num_parents;

	int shared;
};

struct clk_gate_data {
	int id;
	const char *name;
	const char *parent;
	unsigned long flags;
	struct clk *clk;

	int gate_ofs;
	int gate_shift;
	int gate_write_en;

	int shared;
};

#define CLK_GATE_DATA(_id, _name, _parent, _flags, _ofs, _shift, _write_en, \
	_shared) \
{ \
	.id = _id, \
	.name = _name, \
	.parent = _parent, \
	.flags = _flags, \
	.gate_ofs = _ofs, \
	.gate_shift = _shift, \
	.gate_write_en = _write_en, \
	.shared = _shared, \
}

struct rtk_clk_data {
	int clk_num;
	struct regmap *regmap;
	struct hwspinlock *lock;
	struct clk_onecell_data clk_data;
};

struct rtk_clk_data *rtk_clk_alloc_data(int clk_num);
void rtk_clk_free_data(struct rtk_clk_data *data);
int rtk_clk_of_init_data(struct device_node *np, struct rtk_clk_data *data);
int rtk_clk_add_composites(struct device *dev, struct rtk_clk_data *data,
			   struct clk_composite_data *comps, int num);
int rtk_clk_add_gates(struct device *dev, struct rtk_clk_data *data,
		      struct clk_gate_data *gates, int num);

/**
 * rtk_clk_add_hws_from() - register clocks form a clk_hw array to clock
 *			    controller with a start index of the provider
 * @dev: the controller device
 * @data: the controller data
 * @hws: a array of clk_hw to be added,
 * @size: size of the array
 * @start_index: the start index to be added,
 *
 * Return 0 if success
 */
int rtk_clk_add_hws_from(struct device *dev, struct rtk_clk_data *data,
			 struct clk_hw **hws, int size, int start_index);

static inline int rtk_clk_add_hws(struct device *dev, struct rtk_clk_data *data,
				  struct clk_hw **hws, int num)
{
	return rtk_clk_add_hws_from(dev, data, hws, num, 0);
}

/**
 * struct clk_hw_group - a group of clk_hw
 * @hws: clk_hw array
 * @num_hws: number of clk_hw in array
 */
struct clk_hw_group {
	struct clk_hw **hws;
	int num_hws;
};

/**
 * struct clk_hw_map - map of a clk_hw_group
 * @group: a clk_hw group
 * @start_index: first id of the group
 */
struct clk_hw_map {
	struct clk_hw_group *group;
	int start_index;
};

static inline
int rtk_clk_add_hw_group_from(struct device *dev, struct rtk_clk_data *data,
			      struct clk_hw_group *grp, int start_index)
{
	return rtk_clk_add_hws_from(dev, data, grp->hws, grp->num_hws,
				    start_index);
}

static inline
int rtk_clk_add_hw_map(struct device *dev, struct rtk_clk_data *data,
		       const struct clk_hw_map *map)
{
	return rtk_clk_add_hw_group_from(dev, data, map->group,
					 map->start_index);
}

static inline
int rtk_clk_add_hw_maps(struct device *dev, struct rtk_clk_data *data,
			struct clk_hw_map * const *maps)
{
	int ret;
	int i;

	for (i = 0; maps[i]; i++) {
		ret = rtk_clk_add_hw_map(dev, data, maps[i]);
		if (ret)
			return ret;
	}
	return 0;
}

struct freq_table {
	uint32_t val;
	unsigned long rate;
};

#define FREQ_TABLE_END                  { .rate = 0 }
#define IS_FREQ_TABLE_END(_f)           ((_f)->rate == 0)

struct div_table {
	unsigned long rate;
	uint32_t div;
	uint32_t val;
};

#define DIV_TABLE_END                   { .rate = 0 }
#define IS_DIV_TABLE_END(_d)            ((_d)->rate == 0)

const struct freq_table *ftbl_find_by_rate(const struct freq_table *ftbl,
					   unsigned long rate);
const struct freq_table *ftbl_find_by_val_with_mask(const struct freq_table *ftbl,
						    uint32_t mask, uint32_t value);
const struct div_table *dtbl_find_by_rate(const struct div_table *dtbl, unsigned long rate);
const struct div_table *dtbl_find_by_val(const struct div_table *dtbl, uint32_t val);

#endif /* __CLK_REALTEK_COMMON_H */


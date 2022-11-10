/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * Copyright (C) 2017 Realtek Semiconductor Corporation
 * Author: Cheng-Yu Lee <cylee12@realtek.com>
 */

#ifndef __CLK_REALTEK_CLK_MMIO_MUX_H
#define __CLK_REALTEK_CLK_MMIO_MUX_H

#include "common.h"

struct clk_regmap_mux {
	struct clk_regmap clkr;
	int mux_ofs;
	unsigned int mask;
	unsigned int shift;
	spinlock_t *lock;
};

#define to_clk_regmap_mux(_hw) \
	container_of(to_clk_regmap(_hw), struct clk_regmap_mux, clkr)
#define __clk_regmap_mux_hw(_p) __clk_regmap_hw(&(_p)->clkr)

extern const struct clk_ops clk_regmap_mux_ops;


static inline int is_clk_regmap_mux_ops(const struct clk_ops *ops)
{
	return ops == &clk_regmap_mux_ops;
}

#endif

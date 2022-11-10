/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * Copyright (C) 2017 Realtek Semiconductor Corporation
 * Author: Cheng-Yu Lee <cylee12@realtek.com>
 */

#ifndef __CLK_REALTEK_CLK_MMIO_GATE_H
#define __CLK_REALTEK_CLK_MMIO_GATE_H

#include "common.h"

struct clk_regmap_gate {
	struct clk_regmap clkr;
	int gate_ofs;
	uint8_t bit_idx;
	spinlock_t *lock;
	int write_en:1;
};

#define to_clk_regmap_gate(_hw) \
	container_of(to_clk_regmap(_hw), struct clk_regmap_gate, clkr)
#define __clk_regmap_gate_hw(_p) __clk_regmap_hw(&(_p)->clkr)

extern const struct clk_ops clk_regmap_gate_ops;
extern const struct clk_ops clk_regmap_gate_ro_ops;

static inline int is_clk_regmap_gate_ops(const struct clk_ops *ops)
{
	return ops == &clk_regmap_gate_ops || ops == &clk_regmap_gate_ro_ops;
}

#endif

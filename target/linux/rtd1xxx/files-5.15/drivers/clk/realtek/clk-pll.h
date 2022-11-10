/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * Copyright (C) 2017-2019 Realtek Semiconductor Corporation
 * Author: Cheng-Yu Lee <cylee12@realtek.com>
 */

#ifndef __CLK_REALTEK_CLK_PLL_H
#define __CLK_REALTEK_CLK_PLL_H

#include "common.h"

struct clk_pll {
	struct clk_regmap clkr;
	uint32_t pll_type;
#define CLK_PLL_TYPE_MNO         1
#define CLK_PLL_TYPE_NF          2
#define CLK_PLL_TYPE_NF_SSC      3
#define CLK_PLL_TYPE_MNO_V2      4

	int pll_ofs;
	int ssc_ofs;

	const struct freq_table *freq_tbl;
	uint32_t freq_mask;
	uint32_t freq_mask_set;
	uint32_t freq_val;

	uint32_t pow_loc;
#define CLK_PLL_CONF_NO_POW             0
#define CLK_PLL_CONF_POW_LOC_CTL2       1
#define CLK_PLL_CONF_POW_LOC_CTL3       2
	uint32_t pow_set_rs : 1;
	uint32_t pow_set_pi_bps : 1;
	uint32_t pow_set_analog : 1;
	uint32_t rs_mask;
	uint32_t rs_val;
	uint32_t analog_mask;
	uint32_t analog_val;

	spinlock_t *lock;

	uint32_t flags;
};

#define CLK_PLL_CONF_RS(_m, _v) \
	.pow_set_rs = 1,  \
	.rs_mask  = (_m), \
	.rs_val   = (_v)

#define CLK_PLL_CONF_PI_BPS() \
	.pow_set_pi_bps = 1

#define CLK_PLL_CONF_ANALOG(_m, _v) \
	.pow_set_analog = 1,  \
	.analog_mask  = (_m), \
	.analog_val   = (_v)

#define to_clk_pll(_hw) container_of(to_clk_regmap(_hw), struct clk_pll, clkr)
#define __clk_pll_hw(_ptr)  __clk_regmap_hw(&(_ptr)->clkr)

/* clk_pll flags */
#define CLK_PLL_DIV_WORKAROUND          BIT(2)

static inline uint32_t clk_pll_get_pow_offset(struct clk_pll *clkp)
{
	return (clkp->pow_loc == CLK_PLL_CONF_POW_LOC_CTL3) ? 0x8 : 0x4;
}

static inline uint32_t clk_pll_get_freq_mask(struct clk_pll *clkp)
{
	return clkp->freq_mask_set ?: clkp->freq_mask;
}

static inline bool clk_pll_has_pow(struct clk_pll *pll)
{
	if (pll->pow_loc != CLK_PLL_CONF_NO_POW)
		return true;
	return false;
}

static inline unsigned long clk_pll_lock(struct clk_pll *pll)
{
	unsigned long flags = 0;

	if (pll->lock)
		spin_lock_irqsave(pll->lock, flags);
	return flags;
}

static inline void clk_pll_unlock(struct clk_pll *pll, unsigned long flags)
{
	if (pll->lock)
		spin_unlock_irqrestore(pll->lock, flags);
}

struct clk_pll_div {
	struct clk_pll clkp;
	int div_ofs;
	int div_shift;
	int div_width;
	const struct div_table *div_tbl;
	spinlock_t *lock;
};

#define to_clk_pll_div(_hw) \
	container_of(to_clk_pll(_hw), struct clk_pll_div, clkp)
#define __clk_pll_div_hw(_ptr) __clk_pll_hw(&(_ptr)->clkp)

/* clk_pll_div helper functions */
static inline unsigned long clk_pll_div_lock(struct clk_pll_div *plld)
{
	unsigned long flags = 0;

	if (plld->lock)
		spin_lock_irqsave(plld->lock, flags);
	return flags;
}

static inline void clk_pll_div_unlock(struct clk_pll_div *plld,
	unsigned long flags)
{
	if (plld->lock)
		spin_unlock_irqrestore(plld->lock, flags);
}

extern const struct clk_ops clk_pll_ops;
extern const struct clk_ops clk_pll_div_ops;

struct clk_pll_psaud {
	struct clk_regmap clkr;
	int id;
#define CLK_PLL_PSAUD1A       (0x1)
#define CLK_PLL_PSAUD2A       (0x2)
	int reg;
	spinlock_t *lock;
};


#define to_clk_pll_psaud(_hw) \
	container_of(to_clk_regmap(_hw), struct clk_pll_psaud, clkr)
#define __clk_pll_psaud_hw(_ptr)  __clk_regmap_hw(&(_ptr)->clkr)

struct clk_pll_dif {
	struct clk_regmap clkr;
	int pll_ofs;
	int ssc_ofs;
	uint32_t status;
	spinlock_t *lock;
	uint32_t adtv_conf[8];
	int freq;
};
#define to_clk_pll_dif(_hw) \
	container_of(to_clk_regmap(_hw), struct clk_pll_dif, clkr)
#define __clk_pll_dif_hw(_ptr)  __clk_regmap_hw(&(_ptr)->clkr)

struct clk_pll_mmc {
	struct clk_regmap clkr;
	int pll_ofs;
	int ssc_dig_ofs;

	struct clk_hw phase0_hw;
	struct clk_hw phase1_hw;

	int set_rate_val_53_97_set_ipc: 1;
};

#define to_clk_pll_mmc(_hw) \
	container_of(to_clk_regmap(_hw), struct clk_pll_mmc, clkr)
#define __clk_pll_mmc_hw(_ptr)  __clk_regmap_hw(&(_ptr)->clkr)

extern const struct clk_ops clk_pll_psaud_ops;
extern const struct clk_ops clk_pll_dif_ops;
extern const struct clk_ops clk_pll_dif_v2_ops;
extern const struct clk_ops clk_pll_mmc_ops;
extern const struct clk_ops clk_pll_mmc_v2_ops;
extern const struct clk_ops clk_pll_mmc_phase_ops;

static inline int is_clk_pll_ops(const struct clk_ops *ops)
{
	if (ops == &clk_pll_ops || ops == &clk_pll_div_ops)
		return 1;

	if (IS_ENABLED(CONFIG_RTK_CLK_PLL_PSAUD) && ops == &clk_pll_psaud_ops)
		return 1;

	if (IS_ENABLED(CONFIG_RTK_CLK_PLL_DIF) &&
		(ops == &clk_pll_dif_ops || ops == &clk_pll_dif_v2_ops))
		return 1;

	if (IS_ENABLED(CONFIG_RTK_CLK_PLL_MMC) && (ops == &clk_pll_mmc_ops || ops == &clk_pll_mmc_v2_ops))
		return 1;

	return 0;
}

#endif /* __CLK_REALTEK_CLK_PLL_H */

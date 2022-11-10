// SPDX-License-Identifier: GPL-2.0-only
#define pr_fmt(fmt) "rtk-clk-of: " fmt
#include <linux/regmap.h>
#include <linux/of.h>

static void pll_gpu_use_low_freq_mode(struct regmap *regmap)
{
	pr_info("%s\n", __func__);
	regmap_update_bits(regmap, 0x1c0, 0x01C00000, 0x00400000);
}

void of_rtk_clk_setup_crt(struct device_node *np, struct regmap *regmap)
{
	if (of_find_property(np, "pll-gpu,use-low-freq-mode", NULL))
		pll_gpu_use_low_freq_mode(regmap);
}
EXPORT_SYMBOL_GPL(of_rtk_clk_setup_crt);

/* SPDX-License-Identifier: GPL-2.0 */

/* Realtek RTL96XX simple clock driver
 *
 * Example configuration for this driver in the device tree
 *
 * clocks {
 * 	soc_clk: soc_clk {
 * 		compatible = "realtek,rtl96xx-clock"
 * 		#clock-cells = <1>;
 * 		}
 * 	}
 *
 * and one can access/identify in the other device tree devices
 * with <&soc_clk 0> or <&soc_clk 1> where 0 is CLK_OCP and 1 is CLK_LX
 *
 */

#include <linux/clk.h>
#include <linux/clkdev.h>
#include <linux/err.h>
#include <linux/of.h>
#include <linux/of_address.h>
#include <linux/of_irq.h>
#include <linux/delay.h>
#include <linux/spinlock.h>
#include <linux/clk-provider.h>

#include "clk-rtl96xx.h"

#define CLK_OCP 0
#define CLK_LX  1
#define CLK_MAX 2

#define read_sw(reg)		ioread32(((void *)RTL_SW_CORE_BASE) + reg)
#define read_soc(reg)		ioread32(((void *)RTL_SOC_BASE) + reg)

#define write_sw(val, reg)	iowrite32(val, ((void *)RTL_SW_CORE_BASE) + reg)
#define write_soc(val, reg)	iowrite32(val, ((void *)RTL_SOC_BASE) + reg)

struct rtcl_clk {
	unsigned int ocp_clk;
	unsigned int lx_clk;
	struct clk_onecell_data clk_data;
	struct clk *clks[CLK_MAX];
};

struct rtcl_clk *rtcl_clk;

static unsigned int pll_ocp_freq_mhz(void)
{
	unsigned int ctrl0, ctrl3, cmugcr;
	unsigned int cpu_freq_sel0, en_div2_cpu0, cmu_mode, freq_div, cpu_mhz;

	ctrl0 = read_soc(RTL_OCP_PLL_CTRL0);
	ctrl3 = read_soc(RTL_OCP_PLL_CTRL3);
	cmugcr = read_soc(RTL_OC0_CMU_GCR);

	pr_debug("ocp_pll_ctrl0 = 0x%X, ocp_pll_ctrl3 = 0x%X,oc0_cmugcr = 0x%X \n", ctrl0, ctrl3, cmugcr);

	cpu_freq_sel0 = RTL_PLL_CTRL0_CPU_FREQ_SEL0(ctrl0);
	en_div2_cpu0 = RTL_PLL_CTRL3_EN_DIV2_CPU0(ctrl3);
	cmu_mode = RTL_CMU_GCR_CMU_MODE(cmugcr);
	freq_div = RTL_CMU_GCR_FREQ_DIV(cmugcr);
	cpu_mhz = ((cpu_freq_sel0 + 2) * 50) >> en_div2_cpu0;

	if (0 != cmu_mode) {
		cpu_mhz /= (1 << freq_div);
	}
	pr_info("CPU clock is %u\n", cpu_mhz);
	return cpu_mhz;
}

static unsigned int pll_sys_LX_freq_mhz(void)
{
	unsigned int rg5x_pll, LX_freq;

	rg5x_pll = read_sw(RTL_PHY_RG5X_PLL);
	rg5x_pll &= (0xf);
	LX_freq = 1000 / (rg5x_pll + 5);

	return LX_freq * 1000 * 1000;
}

static int rtcl_clk_register(struct device_node *np)
{
	rtcl_clk->ocp_clk = pll_ocp_freq_mhz() * 1000 * 1000;
	rtcl_clk->clks[CLK_OCP] = clk_register_fixed_rate(NULL, "OCP", NULL, 0, rtcl_clk->ocp_clk);

	if (IS_ERR(rtcl_clk->clks[CLK_OCP])) {
		pr_err("%s: Couldn't register OCP clock\n", __func__);
		return 1;
	}

	rtcl_clk->lx_clk = pll_sys_LX_freq_mhz();
	rtcl_clk->clks[CLK_LX] = clk_register_fixed_rate(NULL, "LX", NULL, 0, rtcl_clk->lx_clk);

	if (IS_ERR(rtcl_clk->clks[CLK_LX])) {
		pr_err("%s: Couldn't register LX clock\n", __func__);
		clk_unregister_fixed_rate(rtcl_clk->clks[CLK_OCP]);
		goto err_lx;
	}

	rtcl_clk->clk_data.clks = rtcl_clk->clks;
	rtcl_clk->clk_data.clk_num = CLK_MAX;

	if (of_clk_add_provider(np, of_clk_src_onecell_get, &rtcl_clk->clk_data)) {
		pr_err("%s: Couldn't register clock provider\n", __func__);
		goto err_add;
	}

	return 0;

err_add:
	clk_unregister_fixed_rate(rtcl_clk->clks[CLK_LX]);
err_lx:
	clk_unregister_fixed_rate(rtcl_clk->clks[CLK_OCP]);
	return 1;
}

static void __init rtcl_clocks_init(struct device_node *np)
{
	rtcl_clk = kzalloc(sizeof(*rtcl_clk), GFP_KERNEL);
	if (IS_ERR(rtcl_clk))
		return;

	if(rtcl_clk_register(np))
		kfree(rtcl_clk);
	else
		pr_info("%s: clk_data.clk_num=%d, ocp=%u, lx=%u\n", __func__, rtcl_clk->clk_data.clk_num, rtcl_clk->ocp_clk, rtcl_clk->lx_clk);
}

CLK_OF_DECLARE(rtl96xx_clk, "realtek,rtl96xx-clock", rtcl_clocks_init);

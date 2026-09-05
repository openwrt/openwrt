// SPDX-License-Identifier: GPL-2.0
#include <linux/clk-provider.h>
#include <linux/clkdev.h>
#include <linux/io.h>
#include <linux/ioport.h>
#include <linux/of_address.h>
#include <linux/slab.h>

static struct clk_onecell_data clk_data;

static void __init cs75xx_clk_init_dt(struct device_node *np)
{
	void __iomem *reg;
	unsigned int rev;
	unsigned int rate_cpu;
	unsigned int rate_apb;
	unsigned int rate_axi;

	reg = of_iomap(np, 0);
	if (!reg) {
		pr_err("cs75xx-clk: could not get I/O space.\n");
		return;
	}

	rev = (readl(reg) >> 1) & 0x07;

	switch (rev) {
	case 0:
		rate_cpu = 400 * 1000000;
		rate_apb = 100 * 1000000;
		rate_axi = 133333333;
		break;
	case 1:
		rate_cpu = 600 * 1000000;
		rate_apb = 100 * 1000000;
		rate_axi = 150 * 1000000;
		break;
	case 2:
		rate_cpu = 700 * 1000000;
		rate_apb = 100 * 1000000;
		rate_axi = 140 * 1000000;
		break;
	case 3:
		rate_cpu = 800 * 1000000;
		rate_apb = 100 * 1000000;
		rate_axi = 160 * 1000000;
		break;
	case 4:
		rate_cpu = 900 * 1000000;
		rate_apb = 100 * 1000000;
		rate_axi = 150 * 1000000;
		break;
	case 5:
		rate_cpu = 750 * 1000000;
		rate_apb = 150 * 1000000;
		rate_axi = 150 * 1000000;
		break;
	case 6:
		rate_cpu = 850 * 1000000;
		rate_apb = 170 * 1000000;
		rate_axi = 141666667;
		break;
	default:
		pr_err("cs75xx-clk: unknown strap pin for cpu clock\n");
		goto l_err;
	}

	clk_data.clk_num = 3;

	clk_data.clks = kzalloc(clk_data.clk_num * sizeof(struct clk *), GFP_KERNEL);
	if (!clk_data.clks) {
		pr_err("cs75xx-clk: could not allocate clk object buffer.\n");
		goto l_err;
	}

	clk_data.clks[0] = clk_register_fixed_rate(NULL, "apb", NULL, 0, rate_apb);
	if (IS_ERR(clk_data.clks[0])) {
		pr_err("cs75xx-clk: failed APB clk object registration.\n");
		goto l_err;
	}
	clk_data.clks[1] = clk_register_fixed_rate(NULL, "cpu", NULL, 0, rate_cpu);
	if (IS_ERR(clk_data.clks[1])) {
		pr_err("cs75xx-clk: failed CPU clk object registration.\n");
		goto l_err;
	}
	clk_data.clks[2] = clk_register_fixed_rate(NULL, "axi", NULL, 0, rate_axi);
	if (IS_ERR(clk_data.clks[2])) {
		pr_err("cs75xx-clk: failed AXI clk object registration.\n");
		goto l_err;
	}

	iounmap(reg);

	of_clk_add_provider(np, of_clk_src_onecell_get, &clk_data);

	return;

l_err:
	iounmap(reg);
}

CLK_OF_DECLARE(cs75xx, "cortina,cs75xx-clk", cs75xx_clk_init_dt);

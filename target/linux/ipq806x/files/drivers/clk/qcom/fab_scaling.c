/*
 * Copyright (c) 2015, The Linux Foundation. All rights reserved.
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/err.h>
#include <linux/io.h>
#include <linux/of.h>
#include <linux/of_device.h>
#include <linux/clk.h>
#include <linux/clk-provider.h>
#include <linux/fab_scaling.h>

#define MAX_CLK_COUNT	5

#define APPS_FAB_CLK	"apps-fab-clk"
#define DDR_FAB_CLK	"ddr-fab-clk"

static u32 fab_freq_high;
static u32 fab_freq_nominal;

static struct clk *apps_fab_clk;
static struct clk *ddr_fab_clk;

static struct fab_scaling_info arr_monitor_clk[MAX_CLK_COUNT];

int scale_fabrics(void)
{
	int i, clk_cnt = 0, scale_down = 1;
	unsigned long curr_freq, new_freq;

	if (!apps_fab_clk || !ddr_fab_clk)
		return -1;

	for (i = 0; i < MAX_CLK_COUNT; i++) {
		if (!arr_monitor_clk[i].clk || !arr_monitor_clk[i].idle_freq)
			continue;

		clk_cnt++;
		curr_freq = clk_get_rate(arr_monitor_clk[i].clk);
		if (curr_freq > arr_monitor_clk[i].idle_freq) {
			scale_down = 0;
			break;
		}
	}

	if (clk_cnt) {
		if (scale_down)
			new_freq = fab_freq_nominal;
		else
			new_freq = fab_freq_high;

		clk_set_rate(apps_fab_clk, new_freq);
		clk_set_rate(ddr_fab_clk, new_freq);
	}

	return 0;
}
EXPORT_SYMBOL(scale_fabrics);

int fab_scaling_register(struct fab_scaling_info *data)
{
	int i, ret = -1;

	if (!data)
		return ret;

	if (!data->clk)
		return ret;

	for (i = 0; i < MAX_CLK_COUNT; i++) {
		if (arr_monitor_clk[i].clk == data->clk) {
			pr_err("Clk already registered!!!\n");
			return -1;
		}
	}

	for (i = 0; i < MAX_CLK_COUNT; i++) {
		if (!arr_monitor_clk[i].clk) {
			arr_monitor_clk[i].clk = data->clk;
			arr_monitor_clk[i].idle_freq = data->idle_freq;
			ret = 0;
			break;
		}
	}

	if (ret)
		pr_err("FABRIC scaling registration failed.\n");

	return ret;
}
EXPORT_SYMBOL(fab_scaling_register);

int fab_scaling_unregister(struct clk *clk)
{
	int i;

	if (!clk)
		return -1;

	for (i = 0; i < MAX_CLK_COUNT; i++) {
		if (arr_monitor_clk[i].clk == clk) {
			arr_monitor_clk[i].clk = 0;
			arr_monitor_clk[i].idle_freq = 0;
			break;
		}
	}

	return 0;
}
EXPORT_SYMBOL(fab_scaling_unregister);


static int ipq806x_fab_scaling_probe(struct platform_device *pdev)
{
	struct device_node *np = pdev->dev.of_node;
	int ret;

	if (!np)
		return -ENODEV;

	if (of_property_read_u32(np, "fab_freq_high", &fab_freq_high)) {
		pr_err("FABRICS turbo freq not found. Using defaults...\n");
		fab_freq_high = 533000000;
	}

	if (of_property_read_u32(np, "fab_freq_nominal", &fab_freq_nominal)) {
		pr_err("FABRICS nominal freq not found. Using defaults...\n");
		fab_freq_nominal = 400000000;
	}

	apps_fab_clk = devm_clk_get(&pdev->dev, APPS_FAB_CLK);
	ret = PTR_ERR_OR_ZERO(apps_fab_clk);
	if(ret) {
		/*
		 * If apps fab clk node is present, but clock is not yet
		 * registered, we should try defering probe.
		 */
		if (ret != -EPROBE_DEFER) {
			pr_err("Failed to get APPS FABRIC clock: %d\n", ret);
			ret = -ENODEV;
		}
		else
			pr_warn("Deferring APPS FABRIC clock probe: %d\n", ret);

		apps_fab_clk = 0;

		return ret;
	}
	pr_info("APPS FABRIC clock successfully obtained.\n");

	ddr_fab_clk = devm_clk_get(&pdev->dev, DDR_FAB_CLK);
	ret = PTR_ERR_OR_ZERO(ddr_fab_clk);
	if (ret) {
		/*
		 * If ddr fab clk node is present, but clock is not yet
		 * registered, we should try defering probe.
		 */
		if (ret != -EPROBE_DEFER) {
			pr_err("Failed to get DDR FABRIC clock: %d\n", ret);
			ddr_fab_clk = NULL;
			ret = -ENODEV;
		}
		else
			pr_warn("Deferring DDR FABRIC clock probe: %d\n", ret);

		ddr_fab_clk = 0;

		return ret;
	}
	pr_info("DDR FABRIC clock successfully obtained.\n");

	clk_prepare_enable(apps_fab_clk);
	clk_prepare_enable(ddr_fab_clk);

	return 0;
}

static int ipq806x_fab_scaling_remove(struct platform_device *pdev)
{
	int i;

	for (i = 0; i < MAX_CLK_COUNT; i++) {
		arr_monitor_clk[i].clk = 0;
		arr_monitor_clk[i].idle_freq = 0;
	}

	return 0;
}

static const struct of_device_id fab_scaling_ipq806x_match_table[] = {
	{ .compatible = "qcom,fab-scaling" },
	{ }
};

static struct platform_driver fab_scaling_ipq806x_driver = {
	.probe		= ipq806x_fab_scaling_probe,
	.remove		= ipq806x_fab_scaling_remove,
	.driver		= {
		.name   = "fab-scaling",
		.owner  = THIS_MODULE,
		.of_match_table = fab_scaling_ipq806x_match_table,
	},
};

static int __init fab_scaling_ipq806x_init(void)
{
	return platform_driver_register(&fab_scaling_ipq806x_driver);
}
late_initcall(fab_scaling_ipq806x_init);

static void __exit fab_scaling_ipq806x_exit(void)
{
	platform_driver_unregister(&fab_scaling_ipq806x_driver);
}
module_exit(fab_scaling_ipq806x_exit);

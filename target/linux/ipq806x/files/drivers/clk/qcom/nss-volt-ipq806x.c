/*
 * Copyright (c) 2015-2016, The Linux Foundation. All rights reserved.
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
#include <linux/err.h>
#include <linux/platform_device.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/of_device.h>
#include "nss-volt-ipq806x.h"

static struct regulator *nss_reg;
static u32 nss_core_vdd_nominal;
static u32 nss_core_vdd_high;
static u32 nss_core_threshold_freq;

static int get_required_vdd_nss_core(unsigned long rate)
{
	if (rate >= nss_core_threshold_freq)
		return nss_core_vdd_high;
	else
		return nss_core_vdd_nominal;
}

int nss_ramp_voltage(unsigned long rate, bool ramp_up)
{
	int ret = 0, curr_uV, uV;

	if (!nss_reg) {
		pr_err("NSS core regulator not found.\n");
		return -1;
	}

	uV = get_required_vdd_nss_core(rate);
	curr_uV = regulator_get_voltage(nss_reg);

	if (ramp_up) {
		if (uV > curr_uV) {
			ret = regulator_set_voltage(nss_reg, uV,
							nss_core_vdd_high);
		if (ret)
			pr_err("NSS volt scaling failed (%d)\n", uV);
	}
	} else {
		if (uV < curr_uV) {
			ret = regulator_set_voltage(nss_reg, uV,
							nss_core_vdd_high);
		if (ret)
			pr_err("NSS volt scaling failed (%d)\n", uV);
		}
	}

	return ret;
}

static const struct of_device_id nss_ipq806x_match_table[] = {
	{ .compatible = "qcom,nss-common" },
	{ }
};

static int nss_ipq806x_probe(struct platform_device *pdev)
{
	struct device_node *np = pdev->dev.of_node, *vdd;
	int curr_uV;

	if (!np)
		return -ENODEV;

	vdd = of_parse_phandle(np, "nss_core-supply", 0);
	if (vdd) {
		nss_reg = devm_regulator_get(&pdev->dev, vdd->name);
		if (IS_ERR(nss_reg)) {
			pr_err("NSS regulator_get error\n");
			return -ENODEV;
		}
	}
	else
		return -ENODEV;

	pr_info("NSS nss_core-supply name: %s\n", vdd->name);

	curr_uV = regulator_get_voltage(nss_reg);
	if(curr_uV < 0) {
		pr_warn("NSS regulator_get_voltage error: %d\n", curr_uV);
		return -EPROBE_DEFER;
	}

	if (of_property_read_u32(np, "nss_core_vdd_nominal",
					&nss_core_vdd_nominal)) {
		pr_err("NSS core vdd nominal not found. Using defaults...\n");
		nss_core_vdd_nominal = 1100000;
	}

	if (of_property_read_u32(np, "nss_core_vdd_high",
					&nss_core_vdd_high)) {
		pr_err("NSS core vdd high not found. Using defaults...\n");
		nss_core_vdd_high = 1150000;
	}

	if (of_property_read_u32(np, "nss_core_threshold_freq",
					&nss_core_threshold_freq)) {
		pr_err("NSS core thres freq not found. Using defaults...\n");
		nss_core_threshold_freq = 733000000;
	}

	return 0;
}

static struct platform_driver nss_ipq806x_driver = {
	.probe          = nss_ipq806x_probe,
	.driver         = {
		.name   = "nss-common-ipq806x",
		.owner  = THIS_MODULE,
		.of_match_table = nss_ipq806x_match_table,
	},
};

static int __init nss_ipq806x_init(void)
{
	return platform_driver_register(&nss_ipq806x_driver);
}
device_initcall(nss_ipq806x_init);

static void __exit nss_ipq806x_exit(void)
{
	platform_driver_unregister(&nss_ipq806x_driver);
}
module_exit(nss_ipq806x_exit);

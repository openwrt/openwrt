/*
 * Copyright (c) 2020, The Linux Foundation. All rights reserved.
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

#include <linux/of.h>
#include <linux/clk.h>
#include <linux/ioport.h>
#include <linux/qcom_scm.h>
#include "nss_dp_hal.h"

/*
 * nss_dp_hal_tcsr_base_get
 *	Reads TCSR base address from DTS
 */
static uint32_t nss_dp_hal_tcsr_base_get(void)
{
	uint32_t tcsr_base_addr = 0;
	struct device_node *dp_cmn;

	/*
	 * Get reference to NSS dp common device node
	 */
	dp_cmn = of_find_node_by_name(NULL, "nss-dp-common");
	if (!dp_cmn) {
		pr_info("%s: NSS DP common node not found\n", __func__);
		return 0;
	}

	if (of_property_read_u32(dp_cmn, "qcom,tcsr-base", &tcsr_base_addr)) {
		pr_err("%s: error reading TCSR base\n", __func__);
	}
	of_node_put(dp_cmn);

	return tcsr_base_addr;
}

/*
 * nss_dp_hal_tcsr_set
 *	Sets the TCSR axi cache override register
 */
static void nss_dp_hal_tcsr_set(void)
{
	void __iomem *tcsr_addr = NULL;
	uint32_t tcsr_base;
	int err;

	tcsr_base = nss_dp_hal_tcsr_base_get();
	if (!tcsr_base) {
		pr_err("%s: Unable to get TCSR base address\n", __func__);
		return;
	}

	/*
	 * Check if Trust Zone is enabled in the system.
	 * If yes, we need to go through SCM API call to program TCSR register.
	 * If TZ is not enabled, we can write to the register directly.
	 */
	if (qcom_scm_is_available()) {
#if (LINUX_VERSION_CODE < KERNEL_VERSION(5, 4, 0))
		err = qcom_scm_tcsr_reg_write((tcsr_base + TCSR_GMAC_AXI_CACHE_OVERRIDE_OFFSET),
						TCSR_GMAC_AXI_CACHE_OVERRIDE_VALUE);
#else
		err = qti_scm_tcsr_reg_write((tcsr_base + TCSR_GMAC_AXI_CACHE_OVERRIDE_OFFSET),
						TCSR_GMAC_AXI_CACHE_OVERRIDE_VALUE);
#endif
		if (err) {
			pr_err("%s: SCM TCSR write error: %d\n", __func__, err);
		}
	} else {
		tcsr_addr = ioremap_nocache((tcsr_base + TCSR_GMAC_AXI_CACHE_OVERRIDE_OFFSET),
						TCSR_GMAC_AXI_CACHE_OVERRIDE_REG_SIZE);
		if (!tcsr_addr) {
			pr_err("%s: ioremap failed\n", __func__);
			return;
		}
		writel(TCSR_GMAC_AXI_CACHE_OVERRIDE_VALUE, tcsr_addr);
		iounmap(tcsr_addr);
	}
}

/*
 * nss_dp_hal_get_data_plane_ops
 *	Return the data plane ops for GMAC data plane.
 */
struct nss_dp_data_plane_ops *nss_dp_hal_get_data_plane_ops(void)
{
	return &nss_dp_gmac_ops;
}

/*
 * nss_dp_hal_clk_enable
 *	Function to enable GCC_SNOC_GMAC_AXI_CLK.
 *
 * These clocks are required for GMAC operations.
 */
void nss_dp_hal_clk_enable(struct nss_dp_dev *dp_priv)
{
	struct platform_device *pdev = dp_priv->pdev;
	struct device *dev = &pdev->dev;
	struct clk *gmac_clk = NULL;
	int err;

	gmac_clk = devm_clk_get(dev, NSS_SNOC_GMAC_AXI_CLK);
	if (IS_ERR(gmac_clk)) {
		pr_err("%s: cannot get clock: %s\n", __func__,
						NSS_SNOC_GMAC_AXI_CLK);
		return;
	}

	err = clk_prepare_enable(gmac_clk);
	if (err) {
		pr_err("%s: cannot enable clock: %s, err: %d\n", __func__,
						NSS_SNOC_GMAC_AXI_CLK, err);
		return;
	}
}

/*
 * nss_dp_hal_init
 *	Sets the gmac ops based on the GMAC type.
 */
bool nss_dp_hal_init(void)
{
	nss_dp_hal_set_gmac_ops(&syn_hal_ops, GMAC_HAL_TYPE_SYN_GMAC);

	/*
	 * Program the global GMAC AXI Cache override register
	 * for optimized AXI DMA operation.
	 */
	nss_dp_hal_tcsr_set();
	return true;
}

/*
 * nss_dp_hal_cleanup
 *	Sets the gmac ops to NULL.
 */
void nss_dp_hal_cleanup(void)
{
	nss_dp_hal_set_gmac_ops(NULL, GMAC_HAL_TYPE_SYN_GMAC);
}

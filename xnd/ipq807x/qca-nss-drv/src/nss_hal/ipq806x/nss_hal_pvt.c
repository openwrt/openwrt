/*
 **************************************************************************
 * Copyright (c) 2013, 2015-2021, The Linux Foundation. All rights reserved.
 * Permission to use, copy, modify, and/or distribute this software for
 * any purpose with or without fee is hereby granted, provided that the
 * above copyright notice and this permission notice appear in all copies.
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT
 * OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 **************************************************************************
 */

/**
 * nss_hal_pvt.c
 *	NSS HAL private APIs.
 */

#include <linux/delay.h>
#include <linux/err.h>
#include <linux/gpio.h>
#include <linux/version.h>
#include <linux/clk.h>
#if (NSS_DT_SUPPORT != 1)
#include <mach/gpiomux.h>
#include <mach/msm_nss.h>
#else
#include <linux/of.h>
#include <linux/of_net.h>
#include <linux/of_irq.h>
#include <linux/of_address.h>
#include <linux/reset.h>
#endif
#include "nss_hal.h"
#include "nss_clocks.h"
#include "nss_core.h"
#if (NSS_PM_SUPPORT == 1)
#include "nss_pm.h"
#endif
#if (NSS_FABRIC_SCALING_SUPPORT == 1)
#include <linux/fab_scaling.h>
#endif

#define NSS_H2N_INTR_EMPTY_BUFFER_QUEUE_BIT		0
#define NSS_H2N_INTR_DATA_COMMAND_QUEUE_BIT		1
#define NSS_H2N_INTR_TX_UNBLOCKED_BIT			11
#define NSS_H2N_INTR_EMPTY_PAGED_BUFFER_QUEUE_BIT	12
#define NSS_H2N_INTR_TRIGGER_COREDUMP_BIT		15

/*
 * Interrupt type to cause vector.
 */
static uint32_t intr_cause[] = {(1 << NSS_H2N_INTR_EMPTY_BUFFER_QUEUE_BIT),
				(1 << NSS_H2N_INTR_DATA_COMMAND_QUEUE_BIT),
				(1 << NSS_H2N_INTR_TX_UNBLOCKED_BIT),
				(1 << NSS_H2N_INTR_TRIGGER_COREDUMP_BIT),
				(1 << NSS_H2N_INTR_EMPTY_PAGED_BUFFER_QUEUE_BIT)};

#if (NSS_DT_SUPPORT == 1)
bool nss_crypto_is_scaled = false;
#endif

#if (NSS_FW_DBG_SUPPORT == 1)
/*
 * NSS debug pins configuration
 */

/*
 * Core 0, Data
 * No pull up, Function 2
 */
static struct gpiomux_setting nss_spi_data_0 = {
	.func = GPIOMUX_FUNC_2,
	.drv = GPIOMUX_DRV_8MA,
	.pull = GPIOMUX_PULL_NONE,
	.dir = GPIOMUX_IN,
};

/*
 * Core 0, CLK, CS
 * Pull up high, Function 2
 */
static struct gpiomux_setting nss_spi_cs_clk_0 = {
	.func = GPIOMUX_FUNC_2,
	.drv = GPIOMUX_DRV_8MA,
	.pull = GPIOMUX_PULL_UP,
	.dir = GPIOMUX_IN,
};

/*
 * Core 1, CS
 * Pull up high, Function 4
 */
static struct gpiomux_setting nss_spi_cs_1 = {
	.func = GPIOMUX_FUNC_4,
	.drv = GPIOMUX_DRV_8MA,
	.pull = GPIOMUX_PULL_UP,
	.dir = GPIOMUX_IN,
};

/*
 * Core 1, CLK
 * Pull up high, Function 5
 */
static struct gpiomux_setting nss_spi_clk_1 = {
	.func = GPIOMUX_FUNC_5,
	.drv = GPIOMUX_DRV_8MA,
	.pull = GPIOMUX_PULL_UP,
	.dir = GPIOMUX_IN,
};

/*
 * Core 1, Data
 * Pull up none, Function 5
 */
static struct gpiomux_setting nss_spi_data_1 = {
	.func = GPIOMUX_FUNC_5,
	.drv = GPIOMUX_DRV_8MA,
	.pull = GPIOMUX_PULL_NONE,
	.dir = GPIOMUX_IN,
};

static struct msm_gpiomux_config nss_spi_gpiomux[] = {
	{
		.gpio = 14,
		.settings = {
			[GPIOMUX_ACTIVE] = &nss_spi_data_0,
			[GPIOMUX_SUSPENDED] = &nss_spi_data_0,
		},
	},
	{
		.gpio = 15,
		.settings = {
			[GPIOMUX_ACTIVE] = &nss_spi_data_0,
			[GPIOMUX_SUSPENDED] = &nss_spi_data_0,
		},
	},
	{
		.gpio = 16,
		.settings = {
			[GPIOMUX_ACTIVE] = &nss_spi_cs_clk_0,
			[GPIOMUX_SUSPENDED] = &nss_spi_cs_clk_0,
		},
	},
	{
		.gpio = 17,
		.settings = {
			[GPIOMUX_ACTIVE] = &nss_spi_cs_clk_0,
			[GPIOMUX_SUSPENDED] = &nss_spi_cs_clk_0,
		},
	},
	{
		.gpio = 55,
		.settings = {
			[GPIOMUX_ACTIVE] = &nss_spi_data_1,
			[GPIOMUX_SUSPENDED] = &nss_spi_data_1,
		},
	},
	{
		.gpio = 56,
		.settings = {
			[GPIOMUX_ACTIVE] = &nss_spi_data_1,
			[GPIOMUX_SUSPENDED] = &nss_spi_data_1,
		},
	},
	{
		.gpio = 57,
		.settings = {
			[GPIOMUX_ACTIVE] = &nss_spi_cs_1,
			[GPIOMUX_SUSPENDED] = &nss_spi_cs_1,
		},
	},
	{
		.gpio = 58,
		.settings = {
			[GPIOMUX_ACTIVE] = &nss_spi_clk_1,
			[GPIOMUX_SUSPENDED] = &nss_spi_clk_1,
		},
	},
};
#endif /* NSS_FW_DBG_SUPPORT */

/*
 * nss_hal_scale_fabric()
 *	DT supported fabric scaling
 */
void nss_hal_scale_fabric(uint32_t work_frequency)
{
#if (NSS_DT_SUPPORT == 1)
	nss_crypto_pm_event_callback_t crypto_pm_cb;
	bool auto_scale;
	bool turbo;

#if (NSS_FABRIC_SCALING_SUPPORT == 1)
	/*
	 * PM framework
	 */
	scale_fabrics();
#endif
	if ((nss_fab0_clk != NULL) && (nss_fab1_clk != NULL)) {
		if (work_frequency >= NSS_FREQ_733) {
			clk_set_rate(nss_fab0_clk, NSS_FABRIC0_TURBO);
			clk_set_rate(nss_fab1_clk, NSS_FABRIC1_TURBO);
		} else if (work_frequency > NSS_FREQ_110) {
			clk_set_rate(nss_fab0_clk, NSS_FABRIC0_NOMINAL);
			clk_set_rate(nss_fab1_clk, NSS_FABRIC1_NOMINAL);
		} else {
			clk_set_rate(nss_fab0_clk, NSS_FABRIC0_IDLE);
			clk_set_rate(nss_fab1_clk, NSS_FABRIC1_IDLE);
		}

		/*
		 * notify crypto about the clock change
		 */
		crypto_pm_cb = nss_top_main.crypto_pm_callback;
		if (crypto_pm_cb) {
			turbo = (work_frequency >= NSS_FREQ_733);
			auto_scale = nss_cmd_buf.auto_scale;
			nss_crypto_is_scaled = crypto_pm_cb(nss_top_main.crypto_pm_ctx, turbo, auto_scale);
		}
	}
#endif
}

/*
 * nss_hal_pm_support()
 *	Supported in 3.4
 */
void nss_hal_pm_support(uint32_t work_frequency)
{
#if (NSS_PM_SUPPORT == 1)
	if (!pm_client) {
		return;
	}

	if (work_frequency >= NSS_FREQ_733) {
		nss_pm_set_perf_level(pm_client, NSS_PM_PERF_LEVEL_TURBO);
	} else if (work_frequency > NSS_FREQ_110) {
		nss_pm_set_perf_level(pm_client, NSS_PM_PERF_LEVEL_NOMINAL);
	} else {
		nss_pm_set_perf_level(pm_client, NSS_PM_PERF_LEVEL_IDLE);
	}
#endif
}

/*
 * nss_hal_freq_change()
 *	Send frequency change message, and clock adjustment
 */
void nss_hal_freq_change(nss_work_t *my_work)
{
	nss_freq_change(&nss_top_main.nss[NSS_CORE_0], my_work->frequency, my_work->stats_enable, 0);
	if (nss_top_main.nss[NSS_CORE_1].state == NSS_CORE_STATE_INITIALIZED) {
		nss_freq_change(&nss_top_main.nss[NSS_CORE_1], my_work->frequency, my_work->stats_enable, 0);
	}
	clk_set_rate(nss_core0_clk, my_work->frequency);

	nss_freq_change(&nss_top_main.nss[NSS_CORE_0], my_work->frequency, my_work->stats_enable, 1);
	if (nss_top_main.nss[NSS_CORE_1].state == NSS_CORE_STATE_INITIALIZED) {
		nss_freq_change(&nss_top_main.nss[NSS_CORE_1], my_work->frequency, my_work->stats_enable, 1);
	}
}

/*
 * nss_hal_wq_function()
 *	Added to Handle BH requests to kernel
 */
void nss_hal_wq_function(struct work_struct *work)
{
	nss_work_t *my_work = (nss_work_t *)work;

	mutex_lock(&nss_top_main.wq_lock);
#if (NSS_DT_SUPPORT == 1)
	/*
	 * If crypto clock is in Turbo, disable scaling for other
	 * NSS subsystem components and retain them at turbo
	 */
	if (nss_crypto_is_scaled) {
		nss_cmd_buf.current_freq = nss_runtime_samples.freq_scale[NSS_FREQ_HIGH_SCALE].frequency;
		mutex_unlock(&nss_top_main.wq_lock);
		return;
	}
#endif

	nss_hal_freq_change(my_work);

	/*
	 * Supported in 3.4
	 */
	nss_hal_pm_support(my_work->frequency);

	nss_hal_scale_fabric(my_work->frequency);

	mutex_unlock(&nss_top_main.wq_lock);
	kfree((void *)work);
}

/*
 * nss_hal_handle_irq()
 *	HLOS interrupt handler for nss interrupts
 */
static irqreturn_t nss_hal_handle_irq(int irq, void *ctx)
{
	struct int_ctx_instance *int_ctx = (struct int_ctx_instance *) ctx;
	struct nss_ctx_instance *nss_ctx = int_ctx->nss_ctx;

	/*
	 * Mask interrupt until our bottom half re-enables it
	 */
	nss_hal_disable_interrupt(nss_ctx, int_ctx->shift_factor, NSS_HAL_SUPPORTED_INTERRUPTS);

	/*
	 * Schedule tasklet to process interrupt cause
	 */
	napi_schedule(&int_ctx->napi);
	return IRQ_HANDLED;
}

#if (NSS_DT_SUPPORT != 1)
#if defined(NSS_ENABLE_CLK)
/*
 * nss_hal_pvt_enable_pll18()
 *	Enable PLL18
 */
static uint32_t nss_hal_pvt_enable_pll18(uint32_t speed)
{
	uint32_t retries = 100;

	/*
	 * Prevent Compiler from commenting out the loop.
	 */
	uint32_t value;
	uint32_t mask = (1 << 2);

	/*
	 * Start with clean slate
	 */
	writel(0, PLL18_MODE);

	/*
	 * Effective VCO Frequency = 1100 MHz Post Divide 2
	 */
	if (speed == 1100) {
		writel(0x4000042C, PLL18_L_VAL);
		writel(0x0, PLL18_M_VAL);
		writel(0x1, PLL18_N_VAL);

		/*
		 * PLL configuration (as provided by HW team)
		 */
		writel(0x01495625, PLL18_CONFIG);
		writel(0x00003080, PLL18_TEST_CTL);
	} else if (speed == 1466) {
		/*
		 * Effective VCO Frequency = 1466 MHz Post Divide 2
		 */

		writel(0x4000043A, PLL18_L_VAL);
		writel(0x10, PLL18_M_VAL);
		writel(0x19, PLL18_N_VAL);

		/*
		 * PLL configuration (as provided by HW team)
		 */
		writel(0x014B5625, PLL18_CONFIG);
		writel(0x00003080, PLL18_TEST_CTL);
	} else {
		BUG_ON(1);
	}

	/*
	 * Enable PLL18 output (sequence provided by HW team)
	 */
	writel(0x2, PLL18_MODE);
	mdelay(1);
	writel(0x6, PLL18_MODE);
	writel(0x7, PLL18_MODE);

	/*
	 * Enable NSS Vote for PLL18.
	 */
	writel(mask, PLL_ENA_NSS);
	do {
		value = readl(PLL_LOCK_DET_STATUS);
		if (value & mask) {
			return PLL_LOCKED;
		}

		mdelay(1);
	} while (retries-- > 0);

	return PLL_NOT_LOCKED;
}
#endif
#else
/*
 * __nss_hal_of_get_pdata()
 *	Retrieve platform data from device node.
 */
static struct nss_platform_data *__nss_hal_of_get_pdata(struct platform_device *pdev)
{
	struct device_node *np = of_node_get(pdev->dev.of_node);
	struct nss_platform_data *npd;
	struct nss_ctx_instance *nss_ctx = NULL;
	struct nss_top_instance *nss_top = &nss_top_main;
	struct resource res_nphys, res_vphys;
	int32_t i;

	npd = devm_kzalloc(&pdev->dev, sizeof(struct nss_platform_data), GFP_KERNEL);
	if (!npd) {
		return NULL;
	}

	if (of_property_read_u32(np, "qcom,id", &npd->id)
	    || of_property_read_u32(np, "qcom,load-addr", &npd->load_addr)
	    || of_property_read_u32(np, "qcom,num-queue", &npd->num_queue)
	    || of_property_read_u32(np, "qcom,num-irq", &npd->num_irq)) {
		pr_err("%s: error reading critical device node properties\n", np->name);
		goto out;
	}

	/*
	 * Read frequencies. If failure, load default values.
	 */
	of_property_read_u32(np, "qcom,low-frequency", &nss_runtime_samples.freq_scale[NSS_FREQ_LOW_SCALE].frequency);
	of_property_read_u32(np, "qcom,mid-frequency", &nss_runtime_samples.freq_scale[NSS_FREQ_MID_SCALE].frequency);
	of_property_read_u32(np, "qcom,max-frequency", &nss_runtime_samples.freq_scale[NSS_FREQ_HIGH_SCALE].frequency);

	if (npd->num_irq < npd->num_queue) {
		pr_err("%s: not enough interrupts configured for all the queues\n", np->name);
		goto out;
	}

	if (npd->num_irq > NSS_MAX_IRQ_PER_CORE) {
		pr_err("%s: exceeds maximum interrupt numbers per core\n", np->name);
		goto out;
	}

	nss_ctx = &nss_top->nss[npd->id];
	nss_ctx->id = npd->id;

	if (of_address_to_resource(np, 0, &res_nphys) != 0) {
		nss_info_always("%px: nss%d: of_address_to_resource() fail for nphys\n", nss_ctx, nss_ctx->id);
		goto out;
	}

	if (of_address_to_resource(np, 1, &res_vphys) != 0) {
		nss_info_always("%px: nss%d: of_address_to_resource() fail for vphys\n", nss_ctx, nss_ctx->id);
		goto out;
	}

	/*
	 * Save physical addresses
	 */
	npd->nphys = res_nphys.start;
	npd->vphys = res_vphys.start;

	npd->nmap = ioremap_nocache(npd->nphys, resource_size(&res_nphys));
	if (!npd->nmap) {
		nss_info_always("%px: nss%d: ioremap() fail for nphys\n", nss_ctx, nss_ctx->id);
		goto out;
	}

	nss_assert(npd->vphys);
	npd->vmap = ioremap_cache(npd->vphys, resource_size(&res_vphys));
	if (!npd->vmap) {
		nss_info_always("%px: nss%d: ioremap() fail for vphys\n", nss_ctx, nss_ctx->id);
		goto out;
	}

	/*
	 * Clear TCM memory used by this core
	 */
	for (i = 0; i < resource_size(&res_vphys) ; i += 4) {
		nss_write_32(npd->vmap, i, 0);
		NSS_CORE_DMA_CACHE_MAINT((npd->vmap + i), 4, DMA_TO_DEVICE);
	}
	NSS_CORE_DSB();

	/*
	 * Get IRQ numbers
	 */
	for (i = 0 ; i < npd->num_irq; i++) {
		npd->irq[i] = irq_of_parse_and_map(np, i);
		if (!npd->irq[i]) {
			nss_info_always("%px: nss%d: irq_of_parse_and_map() fail for irq %d\n", nss_ctx, nss_ctx->id, i);
			goto out;
		}
	}

	nss_hal_dt_parse_features(np, npd);

	of_node_put(np);
	return npd;

out:
	if (npd->nmap) {
		iounmap(npd->nmap);
	}

	if (npd->vmap) {
		iounmap(npd->vmap);
	}

	devm_kfree(&pdev->dev, npd);
	of_node_put(np);
	return NULL;
}
#endif

/*
 * __nss_hal_core_reset()
 */
static int __nss_hal_core_reset(struct platform_device *nss_dev, void __iomem *map, uint32_t addr, uint32_t clk_src)
{
#if (NSS_DT_SUPPORT == 1)
	struct reset_control *rstctl = NULL;

	/*
	 * Remove UBI32 reset clamp
	 */
	rstctl = devm_reset_control_get(&nss_dev->dev, "clkrst-clamp");
	if (IS_ERR(rstctl)) {
		nss_info_always("%px: Deassert UBI32 core%d reset clamp failed", nss_dev, nss_dev->id);
		return -EFAULT;
	}
	reset_control_deassert(rstctl);

	/*
	 * Remove UBI32 core clamp
	 */
	rstctl = devm_reset_control_get(&nss_dev->dev, "clamp");
	if (IS_ERR(rstctl)) {
		nss_info_always("%px: Deassert UBI32 core%d clamp failed", nss_dev, nss_dev->id);
		return -EFAULT;
	}
	reset_control_deassert(rstctl);

	/*
	 * Remove UBI32 AHB reset
	 */
	rstctl = devm_reset_control_get(&nss_dev->dev, "ahb");
	if (IS_ERR(rstctl)) {
		nss_info_always("%px: Deassert AHB core%d reset failed", nss_dev, nss_dev->id);
		return -EFAULT;
	}
	reset_control_deassert(rstctl);

	/*
	 * Remove UBI32 AXI reset
	 */
	rstctl = devm_reset_control_get(&nss_dev->dev, "axi");
	if (IS_ERR(rstctl)) {
		nss_info_always("%px: Deassert core%d AXI reset failed", nss_dev, nss_dev->id);
		return -EFAULT;
	}
	reset_control_deassert(rstctl);
#else
#if defined(NSS_ENABLE_CLOCK)
	/*
	 * Enable mpt clock
	 */
	writel(0x10, UBI32_MPT0_CLK_CTL);

	/*
	 * UBI coren clock root enable
	 */
	if (clk_src == NSS_REGS_CLK_SRC_DEFAULT) {
		/* select Src0 */
		writel(0x02, UBI32_COREn_CLK_SRC_CTL(nss_dev->id));
	} else {
		/* select Src1 */
		writel(0x03, UBI32_COREn_CLK_SRC_CTL(nss_dev->id));
	}

	/*
	 * Src0: Bypass M value configuration.
	 */

	/*
	 * Src1: M val is 0x01 and NOT_2D value is 0xfd, 400 MHz with PLL0.
	 */
	writel(0x100fd, UBI32_COREn_CLK_SRC1_MD(nss_dev->id));

	/*
	 * Bypass, pll18
	 * Effective frequency = 550 MHz
	 */
	writel(0x00000001, UBI32_COREn_CLK_SRC0_NS(nss_dev->id));

	/*
	 * Dual edge, pll0, NOT(N_M) = 0xfe.
	 * Effective frequency = 400 MHz
	 */
	writel(0x00fe0142, UBI32_COREn_CLK_SRC1_NS(nss_dev->id));

	/*
	 * UBI32 coren clock control branch.
	 */
	writel(0x4f, UBI32_COREn_CLK_FS(nss_dev->id));

	/*
	 * UBI32 coren clock control branch.
	 */
	writel(0x10, UBI32_COREn_CLK_CTL(nss_dev->id));
#endif
	/*
	 * Remove UBI32 reset clamp
	 */
	writel(0xB, UBI32_COREn_RESET_CLAMP(nss_dev->id));

	/*
	 * Busy wait for few cycles
	 */
	mdelay(1);

	/*
	 * Remove UBI32 core clamp
	 */
	writel(0x3, UBI32_COREn_RESET_CLAMP(nss_dev->id));

	mdelay(1);

	/*
	 * Remove UBI32 AHB reset
	 */
	writel(0x1, UBI32_COREn_RESET_CLAMP(nss_dev->id));

	mdelay(1);

	/*
	 * Remove UBI32 AXI reset
	 */
	writel(0x0, UBI32_COREn_RESET_CLAMP(nss_dev->id));

	mdelay(1);
#endif /* NSS_DT_SUPPORT */

	/*
	 * Apply ubi32 core reset
	 */
	nss_write_32(map, NSS_REGS_RESET_CTRL_OFFSET, 1);

	/*
	 * Program address configuration
	 */
	nss_write_32(map, NSS_REGS_CORE_AMC_OFFSET, 1);
	nss_write_32(map, NSS_REGS_CORE_BAR_OFFSET, 0x3c000000);
	nss_write_32(map, NSS_REGS_CORE_BOOT_ADDR_OFFSET, addr);

	/*
	 * C2C interrupts are level sensitive
	 */
	nss_write_32(map, NSS_REGS_CORE_INT_STAT2_TYPE_OFFSET, 0xFFFF);

	/*
	 * Enable Instruction Fetch range checking between 0x4000 0000 to 0xBFFF FFFF.
	 */
	nss_write_32(map, NSS_REGS_CORE_IFETCH_RANGE_OFFSET, 0xBF004001);

	/*
	 * De-assert ubi32 core reset
	 */
	nss_write_32(map, NSS_REGS_RESET_CTRL_OFFSET, 0);

	return 0;
}

/*
 * __nss_hal_debug_enable()
 *	Enable NSS debug
 */
static void __nss_hal_debug_enable(void)
{
#if (NSS_FW_DBG_SUPPORT == 1)
	msm_gpiomux_install(nss_spi_gpiomux,
				ARRAY_SIZE(nss_spi_gpiomux));
#endif
}

/*
 * __nss_hal_common_reset
 *	Do reset/clock configuration common to all cores
 */
static int __nss_hal_common_reset(struct platform_device *nss_dev)
{
#if (NSS_DT_SUPPORT == 1)
	struct device_node *cmn = NULL;
	struct resource res_nss_fpb_base;
	struct clk *nss_tcm_src = NULL;
	struct clk *nss_tcm_clk = NULL;
	void __iomem *fpb_base;
	int err;

	/*
	 * Get reference to NSS common device node
	 */
	cmn = of_find_node_by_name(NULL, "nss-common");
	if (!cmn) {
		pr_err("%px: Unable to find nss-common node\n", nss_dev);
		return -EFAULT;
	}

	if (of_address_to_resource(cmn, 0, &res_nss_fpb_base) != 0) {
		pr_err("%px: of_address_to_resource() return error for nss_fpb_base\n", nss_dev);
		of_node_put(cmn);
		return -EFAULT;
	}
	of_node_put(cmn);

	fpb_base = ioremap_nocache(res_nss_fpb_base.start, resource_size(&res_nss_fpb_base));
	if (!fpb_base) {
		pr_err("%px: ioremap fail for nss_fpb_base\n", nss_dev);
		return -EFAULT;
	}

	/*
	 * Attach debug interface to TLMM
	 */
	nss_write_32(fpb_base, NSS_REGS_FPB_CSR_CFG_OFFSET, 0x360);

	/*
	 * NSS TCM CLOCK
	 */
	nss_tcm_src = clk_get(&nss_dev->dev, NSS_TCM_SRC_CLK);
	if (IS_ERR(nss_tcm_src)) {
		pr_err("%px: cannot get clock: %s\n", nss_dev, NSS_TCM_SRC_CLK);
		return -EFAULT;
	}

	err = clk_set_rate(nss_tcm_src, NSSTCM_FREQ);
	if (err) {
		pr_err("%px: cannot set NSSTCM freq\n", nss_dev);
		return -EFAULT;
	}

	err = clk_prepare_enable(nss_tcm_src);
	if (err) {
		pr_err("%px: cannot enable NSSTCM clock source\n", nss_dev);
		return -EFAULT;
	}

	nss_tcm_clk = clk_get(&nss_dev->dev, NSS_TCM_CLK);
	if (IS_ERR(nss_tcm_clk)) {
		pr_err("%px: cannot get clock: %s\n", nss_dev, NSS_TCM_CLK);
		return -EFAULT;
	}

	err = clk_prepare_enable(nss_tcm_clk);
	if (err) {
		pr_err("%px: cannot enable NSSTCM clock\n", nss_dev);
		return -EFAULT;
	}

	/*
	 * NSS Fabric Clocks.
	 */
	nss_fab0_clk = clk_get(&nss_dev->dev, NSS_FABRIC0_CLK);
	if (IS_ERR(nss_fab0_clk)) {
		pr_err("%px: cannot get clock: %s\n", nss_dev, NSS_FABRIC0_CLK);
		nss_fab0_clk = NULL;
	} else {
		err = clk_prepare_enable(nss_fab0_clk);
		if (err) {
			pr_err("%px: cannot enable clock: %s\n", nss_dev, NSS_FABRIC0_CLK);
			return -EFAULT;
		}
	}

	nss_fab1_clk = clk_get(&nss_dev->dev, NSS_FABRIC1_CLK);
	if (IS_ERR(nss_fab1_clk)) {
		pr_err("%px: cannot get clock: %s\n", nss_dev, NSS_FABRIC1_CLK);
		nss_fab1_clk = NULL;
	} else {
		err = clk_prepare_enable(nss_fab1_clk);
		if (err) {
			pr_err("%px: cannot enable clock: %s\n", nss_dev, NSS_FABRIC1_CLK);
			return -EFAULT;
		}
	}

	nss_top_main.nss_hal_common_init_done = true;
	nss_info("nss_hal_common_reset Done\n");
	return 0;
}
#else
	uint32_t i;
	uint32_t value;
	uint32_t status_mask = 0x1;
	uint32_t wait_cycles = 100;

#if defined(NSS_ENABLE_CLK)
	/*
	 * NSS FPB CLOCK
	 */

	/*
	 * Enable clock root and Divider 0
	 * NOTE: Default value is good so no work here
	 */

	/*
	 * PLL0 (800 MHZ). SRC_SEL is 2 (3'b010)
	 * src_div selected is Div-6 (4'b0101).
	 *
	 * Effective frequency (Divider 0) = 133 MHz
	 */
	writel(0x2a, NSSFPB_CLK_SRC0_NS);

	/*
	 * Enable clock branch
	 */
	writel(0x50, NSSFPB_CLK_CTL);

	/*
	 * NSS FABRIC0 CLOCK
	 */

	/*
	 * Enable clock root and Divider 0
	 * NOTE: Default value is good so no work here
	 */

	/*
	 * PLL0 (800 MHZ) and div is set to 2.
	 * Effective frequency = 400 MHZ.
	 */
	writel(0x0a, NSSFB0_CLK_SRC0_NS);

	/*
	 * NSS Fabric0 Branch and dynamic clock gating enabled.
	 */
	writel(0x50, NSSFB0_CLK_CTL);

	/*
	 * Enable clock root and Divider 0
	 * NOTE: Default value is good so no work here
	 */

	/*
	 * PLL0 (800 MHZ) and div is set to 4.
	 * Effective frequency = 200 MHZ.
	 */
	writel(0x1a, NSSFB1_CLK_SRC0_NS);

	/*
	 * NSS Fabric1 Branch enable and fabric clock gating enabled.
	 */
	writel(0x50, NSSFB1_CLK_CTL);

	/*
	 * NSS TCM CLOCK
	 */

	/*
	 * Enable NSS TCM clock root source and select divider 0.
	 *
	 * NOTE: Default value is not good here
	 */
	writel(0x2, NSSTCM_CLK_SRC_CTL);

	/*
	 * PLL0 (800 MHZ) and div is set to 2.
	 * Effective frequency = 400 MHZ
	 */
	writel(0xa, NSSTCM_CLK_SRC0_NS);

	/*
	 * NSS TCM Branch enable and fabric clock gating enabled.
	 */
	writel(0x50, NSSTCM_CLK_CTL);

	/*
	 * Enable global NSS clock branches.
	 * NSS global Fab Branch enable and fabric clock gating enabled.
	 */
	writel(0xf, NSSFAB_GLOBAL_BUS_NS);

	/*
	 * Send reset interrupt to NSS
	 */
	writel(0x0, NSS_RESET);

	/*
	 * Enable PLL18
	 */
	pll18_status = nss_hal_pvt_enable_pll18();
	if (!pll18_status) {
		/*
		 * Select alternate good source (Src1/pll0)
		 */
		nss_top->clk_src = NSS_REGS_CLK_SRC_ALTERNATE;
		return;
	}

	/*
	 * Select default source (Src0/pll18)
	 */
	nss_top->clk_src = NSS_REGS_CLK_SRC_DEFAULT;
#endif

	/*
	 * Attach debug interface to TLMM
	 */
	nss_write_32((uint32_t)MSM_NSS_FPB_BASE, NSS_REGS_FPB_CSR_CFG_OFFSET, 0x360);

	/*
	 * NSS TCM CLOCK
	 */

	/*
	 * Enable NSS TCM clock root source - SRC1.
	 *
	 */
	writel(0x3, NSSTCM_CLK_SRC_CTL);

	/* Enable PLL Voting for 0 */
	writel((readl(PLL_ENA_NSS) | 0x1), PLL_ENA_NSS);
	do {
		value = readl(PLL_LOCK_DET_STATUS);
		if (value & status_mask) {
			break;
		}
		mdelay(1);
	} while (wait_cycles-- > 0);

	/*
	 * PLL0 (800 MHZ) and div is set to 3/4.
	 * Effective frequency = 266/400 Mhz for SRC0/1
	 */
	writel(0x12, NSSTCM_CLK_SRC0_NS);
	writel(0xa, NSSTCM_CLK_SRC1_NS);

	/*
	 * NSS TCM Branch enable and fabric clock gating enabled.
	 */
	writel(0x50, NSSTCM_CLK_CTL);

	/*
	 * Clear TCM memory
	 */
	for (i = 0; i < IPQ806X_NSS_TCM_SIZE; i += 4) {
		nss_write_32((uint32_t)MSM_NSS_TCM_BASE, i, 0);
	}

	return 0;
}
#endif /* NSS_DT_SUPPORT */

/*
 * __nss_hal_clock_configure()
 */
static int __nss_hal_clock_configure(struct nss_ctx_instance *nss_ctx, struct platform_device *nss_dev, struct nss_platform_data *npd)
{
#if (NSS_FABRIC_SCALING_SUPPORT == 1)
	struct fab_scaling_info fab_data;
#endif
	int i, err;

	/*
	 * Both ubi core on ipq806x attach to the same clock, configure just the core0
	 */
	if (nss_ctx->id) {
		return 0;
	}

	nss_core0_clk = clk_get(&nss_dev->dev, NSS_CORE_CLK);
	if (IS_ERR(nss_core0_clk)) {
		err = PTR_ERR(nss_core0_clk);
		nss_info_always("%px: Regulator %s get failed, err=%d\n", nss_ctx, dev_name(&nss_dev->dev), err);
		return err;
	}

	/*
	 * Check if turbo is supported
	 */
	if (npd->turbo_frequency) {
		nss_info_always("nss_driver - Turbo Support %d\n", npd->turbo_frequency);
#if (NSS_PM_SUPPORT == 1)
		nss_pm_set_turbo();
#endif
	} else {
		nss_info_always("nss_driver - Turbo No Support %d\n", npd->turbo_frequency);
	}

	/*
	 * If valid entries - from dtsi - then just init clks.
	 * Otherwise query for clocks.
	 */
	if ((nss_runtime_samples.freq_scale[NSS_FREQ_LOW_SCALE].frequency != 0) &&
		(nss_runtime_samples.freq_scale[NSS_FREQ_MID_SCALE].frequency != 0) &&
		(nss_runtime_samples.freq_scale[NSS_FREQ_HIGH_SCALE].frequency != 0)) {
		goto clk_complete;
	}

	/*
	 * Load default scales, then query for higher.
	 * If basic set cannot be set, then go to error, and abort
	 * Two set of defaults, 110, 550, 733 or 110, 275 and 550
	 */
	if (clk_set_rate(nss_core0_clk, NSS_FREQ_110) != 0) {
		return -EFAULT;
	}
	nss_runtime_samples.freq_scale[NSS_FREQ_LOW_SCALE].frequency = NSS_FREQ_110;

	if (npd->turbo_frequency) {
		/*
		 * Figure out the middle scale
		 */
		if (clk_set_rate(nss_core0_clk, NSS_FREQ_600) == 0) {
			nss_runtime_samples.freq_scale[NSS_FREQ_MID_SCALE].frequency = NSS_FREQ_600;
		} else if (clk_set_rate(nss_core0_clk, NSS_FREQ_550) == 0) {
			nss_runtime_samples.freq_scale[NSS_FREQ_MID_SCALE].frequency = NSS_FREQ_550;
		} else {
			return -EFAULT;
		}

		/*
		 * Figure out the max scale
		 */
		if (clk_set_rate(nss_core0_clk, NSS_FREQ_800) == 0) {
			nss_runtime_samples.freq_scale[NSS_FREQ_HIGH_SCALE].frequency = NSS_FREQ_800;
		} else if (clk_set_rate(nss_core0_clk, NSS_FREQ_733) == 0) {
			nss_runtime_samples.freq_scale[NSS_FREQ_HIGH_SCALE].frequency = NSS_FREQ_733;
		} else {
			return -EFAULT;
		}

	} else {
		if (clk_set_rate(nss_core0_clk, NSS_FREQ_275) != 0) {
			return -EFAULT;
		}
		nss_runtime_samples.freq_scale[NSS_FREQ_MID_SCALE].frequency = NSS_FREQ_275;

		if (clk_set_rate(nss_core0_clk, NSS_FREQ_550) != 0) {
			return -EFAULT;
		}
		nss_runtime_samples.freq_scale[NSS_FREQ_HIGH_SCALE].frequency = NSS_FREQ_550;
	}

clk_complete:
#if (NSS_FABRIC_SCALING_SUPPORT == 1)
	if (npd->turbo_frequency) {
		fab_data.idle_freq = nss_runtime_samples.freq_scale[NSS_FREQ_MID_SCALE].frequency;
	} else {
		fab_data.idle_freq = nss_runtime_samples.freq_scale[NSS_FREQ_HIGH_SCALE].frequency;
	}
	fab_data.clk = nss_core0_clk;
	fab_scaling_register(&fab_data);
#endif

	/*
	 * Setup Ranges
	 */
	for (i = 0; i < NSS_FREQ_MAX_SCALE; i++) {
		if (nss_runtime_samples.freq_scale[i].frequency == NSS_FREQ_110) {
			nss_runtime_samples.freq_scale[i].minimum = NSS_FREQ_110_MIN;
			nss_runtime_samples.freq_scale[i].maximum = NSS_FREQ_110_MAX;
		}
		if (nss_runtime_samples.freq_scale[i].frequency == NSS_FREQ_275) {
			nss_runtime_samples.freq_scale[i].minimum = NSS_FREQ_275_MIN;
			nss_runtime_samples.freq_scale[i].maximum = NSS_FREQ_275_MAX;
		}
		if (nss_runtime_samples.freq_scale[i].frequency == NSS_FREQ_550) {
			nss_runtime_samples.freq_scale[i].minimum = NSS_FREQ_550_MIN;
			nss_runtime_samples.freq_scale[i].maximum = NSS_FREQ_550_MAX;
		}
		if (nss_runtime_samples.freq_scale[i].frequency == NSS_FREQ_600) {
			nss_runtime_samples.freq_scale[i].minimum = NSS_FREQ_600_MIN;
			nss_runtime_samples.freq_scale[i].maximum = NSS_FREQ_600_MAX;
		}
		if (nss_runtime_samples.freq_scale[i].frequency == NSS_FREQ_733) {
			nss_runtime_samples.freq_scale[i].minimum = NSS_FREQ_733_MIN;
			nss_runtime_samples.freq_scale[i].maximum = NSS_FREQ_733_MAX;
		}
		if (nss_runtime_samples.freq_scale[i].frequency == NSS_FREQ_800) {
			nss_runtime_samples.freq_scale[i].minimum = NSS_FREQ_800_MIN;
			nss_runtime_samples.freq_scale[i].maximum = NSS_FREQ_800_MAX;
		}
	}

	nss_info_always("Supported Frequencies - ");
	for (i = 0; i < NSS_FREQ_MAX_SCALE; i++) {
		if (nss_runtime_samples.freq_scale[i].frequency == NSS_FREQ_110) {
			nss_info_always("110Mhz ");
		} else if (nss_runtime_samples.freq_scale[i].frequency == NSS_FREQ_275) {
			nss_info_always("275Mhz ");
		} else if (nss_runtime_samples.freq_scale[i].frequency == NSS_FREQ_550) {
			nss_info_always("550Mhz ");
		} else if (nss_runtime_samples.freq_scale[i].frequency == NSS_FREQ_600) {
			nss_info_always("600Mhz ");
		} else if (nss_runtime_samples.freq_scale[i].frequency == NSS_FREQ_733) {
			nss_info_always("733Mhz ");
		} else if (nss_runtime_samples.freq_scale[i].frequency == NSS_FREQ_800) {
			nss_info_always("800Mhz ");
		} else {
			nss_info_always("Error\nNo Table/Invalid Frequency Found - Loading Old Tables -");
			return -EFAULT;
		}
	}
	nss_info_always("\n");

	/*
	 * Set default frequency
	 */
	err = clk_set_rate(nss_core0_clk, nss_runtime_samples.freq_scale[NSS_FREQ_MID_SCALE].frequency);
	if (err) {
		nss_info_always("%px: cannot set nss core0 clock\n", nss_ctx);
		return -EFAULT;
	}

	err = clk_prepare_enable(nss_core0_clk);
	if (err) {
		nss_info_always("%px: cannot enable nss core0 clock\n", nss_ctx);
		return -EFAULT;
	}

	return 0;
}

/*
 * __nss_hal_read_interrupt_cause()
 */
static void __nss_hal_read_interrupt_cause(struct nss_ctx_instance *nss_ctx, uint32_t shift_factor, uint32_t *cause)
{
	uint32_t value = nss_read_32(nss_ctx->nmap, NSS_REGS_N2H_INTR_STATUS_OFFSET);
	*cause = (((value) >> shift_factor) & 0x7FFF);
}

/*
 * __nss_hal_clear_interrupt_cause()
 */
static void __nss_hal_clear_interrupt_cause(struct nss_ctx_instance *nss_ctx, uint32_t shift_factor, uint32_t cause)
{
	nss_write_32(nss_ctx->nmap, NSS_REGS_N2H_INTR_CLR_OFFSET, (cause << shift_factor));
}

/*
 * __nss_hal_disable_interrupt()
 */
static void __nss_hal_disable_interrupt(struct nss_ctx_instance *nss_ctx, uint32_t shift_factor, uint32_t cause)
{
	nss_write_32(nss_ctx->nmap, NSS_REGS_N2H_INTR_MASK_CLR_OFFSET, (cause << shift_factor));
}

/*
 * __nss_hal_enable_interrupt()
 */
static void __nss_hal_enable_interrupt(struct nss_ctx_instance *nss_ctx, uint32_t shift_factor, uint32_t cause)
{
	nss_write_32(nss_ctx->nmap, NSS_REGS_N2H_INTR_MASK_SET_OFFSET, (cause << shift_factor));
}

/*
 * __nss_hal_send_interrupt()
 */
static void __nss_hal_send_interrupt(struct nss_ctx_instance *nss_ctx, uint32_t type)
{
	nss_write_32(nss_ctx->nmap, NSS_REGS_C2C_INTR_SET_OFFSET, intr_cause[type]);
}

/*
 * __nss_hal_request_irq()
 */
static int __nss_hal_request_irq(struct nss_ctx_instance *nss_ctx, struct nss_platform_data *npd, int irq_num)
{
	struct int_ctx_instance *int_ctx = &nss_ctx->int_ctx[irq_num];
	int err;

	if (irq_num == 1) {
		int_ctx->shift_factor = 15;
		err = request_irq(npd->irq[irq_num], nss_hal_handle_irq, 0, "nss_queue1", int_ctx);
	} else {
		int_ctx->shift_factor = 0;
		err = request_irq(npd->irq[irq_num], nss_hal_handle_irq, 0, "nss", int_ctx);
	}
	if (err) {
		nss_info_always("%px: IRQ%d request failed", nss_ctx, npd->irq[irq_num]);
		return err;
	}

	int_ctx->irq = npd->irq[irq_num];
	netif_napi_add(&nss_ctx->napi_ndev, &int_ctx->napi, nss_core_handle_napi, 64);

	return 0;
}

/*
 * __nss_hal_init_imem
 */
void __nss_hal_init_imem(struct nss_ctx_instance *nss_ctx)
{
	struct nss_meminfo_ctx *mem_ctx = &nss_ctx->meminfo_ctx;

	mem_ctx->imem_head = NSS_IMEM_START + NSS_IMEM_SIZE * nss_ctx->id;
	mem_ctx->imem_end = mem_ctx->imem_head + NSS_IMEM_SIZE;
	mem_ctx->imem_tail = mem_ctx->imem_head;

	nss_info("%px: IMEM init: head: 0x%x end: 0x%x tail: 0x%x\n", nss_ctx,
			mem_ctx->imem_head, mem_ctx->imem_end, mem_ctx->imem_tail);
}

/*
 * __nss_hal_init_utcm_shared
 */
bool __nss_hal_init_utcm_shared(struct nss_ctx_instance *nss_ctx, uint32_t *meminfo_start)
{
	/*
	 * Nothing to be done as there are no UTCM_SHARED defined for ipq806x
	 */
	return true;
}

/*
 * nss_hal_ipq806x_ops
 */
struct nss_hal_ops nss_hal_ipq806x_ops = {
	.common_reset = __nss_hal_common_reset,
	.core_reset = __nss_hal_core_reset,
	.clock_configure = __nss_hal_clock_configure,
	.firmware_load = nss_hal_firmware_load,
	.debug_enable = __nss_hal_debug_enable,
#if (NSS_DT_SUPPORT == 1)
	.of_get_pdata = __nss_hal_of_get_pdata,
#endif
	.request_irq = __nss_hal_request_irq,
	.send_interrupt = __nss_hal_send_interrupt,
	.enable_interrupt = __nss_hal_enable_interrupt,
	.disable_interrupt = __nss_hal_disable_interrupt,
	.clear_interrupt_cause = __nss_hal_clear_interrupt_cause,
	.read_interrupt_cause = __nss_hal_read_interrupt_cause,
	.init_imem = __nss_hal_init_imem,
	.init_utcm_shared = __nss_hal_init_utcm_shared,
};

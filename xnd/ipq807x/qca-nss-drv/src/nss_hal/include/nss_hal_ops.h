/*
 **************************************************************************
 * Copyright (c) 2016-2019, The Linux Foundation. All rights reserved.
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
 * nss_hal_ops.h
 *	NSS HAL ops structure declaration.
 */

#ifndef __NSS_HAL_OPS_H
#define __NSS_HAL_OPS_H

#if (NSS_DT_SUPPORT != 1)
#include <mach/gpiomux.h>
#include <mach/msm_nss.h>
#endif

/*
 * nss_hal_ops defines the HAL layer API required to support multiple targets
 */
struct nss_hal_ops {
	int (*common_reset)(struct platform_device *pdev);
	int (*core_reset)(struct platform_device *nss_dev, void __iomem *map, uint32_t addr, uint32_t clk_src);
	int (*clock_configure)(struct nss_ctx_instance *nss_ctx, struct platform_device *nss_dev, struct nss_platform_data *npd);
	void (*debug_enable)(void);
	struct nss_platform_data * (*of_get_pdata)(struct platform_device *pdev);
	int (*firmware_load)(struct nss_ctx_instance *nss_ctx, struct platform_device *nss_dev, struct nss_platform_data *npd);
	void (*read_interrupt_cause)(struct nss_ctx_instance *nss_ctx, uint32_t shift_factor, uint32_t *cause);
	int (*request_irq)(struct nss_ctx_instance *nss_ctx, struct nss_platform_data *npd, int irq_num);
	void (*clear_interrupt_cause)(struct nss_ctx_instance *nss_ctx, uint32_t shift_factor, uint32_t cause);
	void (*send_interrupt)(struct nss_ctx_instance *nss_ctx, uint32_t type);
	void (*enable_interrupt)(struct nss_ctx_instance *nss_ctx, uint32_t shift_factor, uint32_t cause);
	void (*disable_interrupt)(struct nss_ctx_instance *nss_ctx, uint32_t shift_factor, uint32_t cause);
	void (*init_imem)(struct nss_ctx_instance *nss_ctx);
	bool (*init_utcm_shared)(struct nss_ctx_instance *nss_ctx, uint32_t *meminfo_start);
};
#endif /* __NSS_HAL_OPS_H */

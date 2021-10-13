/*
 **************************************************************************
 * Copyright (c) 2013, 2016-2020 The Linux Foundation. All rights reserved.
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
 * nss_hal.h
 *	NSS HAL public declarations.
 */

#ifndef __NSS_HAL_H
#define __NSS_HAL_H

#include <linux/platform_device.h>
#include <nss_core.h>
#include <nss_regs.h>
#include <nss_hal_ops.h>

extern struct clk *nss_core0_clk;
extern struct clk *nss_core1_clk;
extern struct nss_runtime_sampling nss_runtime_samples;
extern struct clk *nss_fab0_clk;
extern struct clk *nss_fab1_clk;
extern void nss_hal_wq_function(struct work_struct *work);

#if defined(NSS_HAL_IPQ806X_SUPPORT)
extern struct nss_hal_ops nss_hal_ipq806x_ops;
#endif
#if defined(NSS_HAL_IPQ807x_SUPPORT)
extern struct nss_hal_ops nss_hal_ipq807x_ops;
#endif
#if defined(NSS_HAL_IPQ60XX_SUPPORT)
extern struct nss_hal_ops nss_hal_ipq60xx_ops;
#endif
#if defined(NSS_HAL_IPQ50XX_SUPPORT)
extern struct nss_hal_ops nss_hal_ipq50xx_ops;
#endif
#if defined(NSS_HAL_FSM9010_SUPPORT)
extern struct nss_hal_ops nss_hal_fsm9010_ops;
#endif

#define NSS_HAL_SUPPORTED_INTERRUPTS (NSS_N2H_INTR_EMPTY_BUFFER_QUEUE | \
					NSS_N2H_INTR_DATA_QUEUE_0 | \
					NSS_N2H_INTR_DATA_QUEUE_1 | \
					NSS_N2H_INTR_EMPTY_BUFFERS_SOS | \
					NSS_N2H_INTR_TX_UNBLOCKED | \
					NSS_N2H_INTR_COREDUMP_COMPLETE | \
					NSS_N2H_INTR_PROFILE_DMA | \
					NSS_N2H_INTR_PAGED_EMPTY_BUFFERS_SOS)

/*
 * nss_hal_read_interrupt_cause()
 */
static inline void nss_hal_read_interrupt_cause(struct nss_ctx_instance *nss_ctx, uint32_t shift_factor, uint32_t *cause)
{
	nss_top_main.hal_ops->read_interrupt_cause(nss_ctx, shift_factor, cause);
}

/*
 * nss_hal_clear_interrupt_cause()
 */
static inline void nss_hal_clear_interrupt_cause(struct nss_ctx_instance *nss_ctx, uint32_t shift_factor, uint32_t cause)
{
	nss_top_main.hal_ops->clear_interrupt_cause(nss_ctx, shift_factor, cause);
}

/*
 * nss_hal_disable_interrupt()
 */
static inline void nss_hal_disable_interrupt(struct nss_ctx_instance *nss_ctx, uint32_t shift_factor, uint32_t cause)
{
	nss_top_main.hal_ops->disable_interrupt(nss_ctx, shift_factor, cause);
}

/*
 * nss_hal_enable_interrupt()
 */
static inline void nss_hal_enable_interrupt(struct nss_ctx_instance *nss_ctx, uint32_t shift_factor, uint32_t cause)
{
	nss_top_main.hal_ops->enable_interrupt(nss_ctx, shift_factor, cause);
}

/*
 * nss_hal_send_interrupt()
 */
static inline void nss_hal_send_interrupt(struct nss_ctx_instance *nss_ctx, uint32_t cause)
{
	nss_top_main.hal_ops->send_interrupt(nss_ctx, cause);
}

/*
 * nss_hal_debug_enable()
 */
static inline void nss_hal_debug_enable(void)
{
	nss_top_main.hal_ops->debug_enable();
}

/*
 * nss_hal_probe()
 */
int nss_hal_probe(struct platform_device *nss_dev);

/*
 * nss_hal_remove()
 */
int nss_hal_remove(struct platform_device *nss_dev);

/*
 * nss_hal_firmware_load()
 */
int nss_hal_firmware_load(struct nss_ctx_instance *nss_ctx, struct platform_device *nss_dev, struct nss_platform_data *npd);

/*
 * nss_hal_dt_parse_features()
 */
void nss_hal_dt_parse_features(struct device_node *np, struct nss_platform_data *npd);
#endif /* __NSS_HAL_H */

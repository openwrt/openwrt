/*
 **************************************************************************
 * Copyright (c) 2015-2021, The Linux Foundation. All rights reserved.
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

#include <linux/err.h>
#include <linux/of.h>
#include <linux/of_net.h>
#include <linux/of_irq.h>
#include <linux/of_address.h>
#include "nss_hal.h"
#include "nss_core.h"

#define NSS_H2N_INTR_EMPTY_BUFFER_QUEUE_BIT     0
#define NSS_H2N_INTR_DATA_COMMAND_QUEUE_BIT     1
#define NSS_H2N_INTR_TX_UNBLOCKED_BIT           11
#define NSS_H2N_INTR_TRIGGER_COREDUMP_BIT       15

/*
 * Interrupt type to cause vector.
 */
static uint32_t intr_cause[] = {(1 << NSS_H2N_INTR_EMPTY_BUFFER_QUEUE_BIT),
				(1 << NSS_H2N_INTR_DATA_COMMAND_QUEUE_BIT),
				(1 << NSS_H2N_INTR_TX_UNBLOCKED_BIT),
				(1 << NSS_H2N_INTR_TRIGGER_COREDUMP_BIT)};

/*
 * nss_hal_wq_function()
 *	Added to Handle BH requests to kernel
 */
void nss_hal_wq_function(struct work_struct *work)
{
	/*
	 * Not supported in FSM9010
	 */
	kfree((void *)work);
}

/*
 * nss_hal_get_num_irqs()
 *	get number of irqs from interrupt resource of device tree
 */
static inline int nss_hal_get_num_irqs(struct device_node *np)
{
	int num_irqs = 0;

	while (of_irq_to_resource(np, num_irqs, NULL)) {
		num_irqs++;
	}

	return num_irqs;
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

/*
 * nss_hal_of_get_pdata()
 *	Retrieve platform data from device node.
 */
static struct nss_platform_data *__nss_hal_of_get_pdata(struct platform_device *pdev)
{
	struct device_node *np = of_node_get(pdev->dev.of_node);
	struct nss_platform_data *npd = NULL;
	struct nss_ctx_instance *nss_ctx = NULL;
	struct nss_top_instance *nss_top = &nss_top_main;
	struct resource res_nphys, res_vphys;
	int32_t i;

	npd = devm_kzalloc(&pdev->dev, sizeof(struct nss_platform_data), GFP_KERNEL);
	if (!npd) {
		return NULL;
	}

	if (of_property_read_u32(np, "qcom,id", &npd->id)
	    || of_property_read_u32(np, "qcom,num-queue", &npd->num_queue)) {
		pr_err("%s: error reading critical device node properties\n", np->name);
		goto out;
	}

	if (of_property_read_u32(np, "qcom,num-irq", &npd->num_irq)) {
		npd->num_irq = nss_hal_get_num_irqs(np);
	}

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
		iounmap((void *)npd->nmap);
	}

	if (npd->vmap) {
		iounmap((void *)npd->vmap);
	}

	devm_kfree(&pdev->dev, npd);
	of_node_put(np);
	return NULL;
}

/*
 * __nss_hal_debug_enable()
 *	Enable NSS debug
 */
static void __nss_hal_debug_enable(void)
{
	return;
}

/*
 * __nss_hal_common_reset()
 */
static int __nss_hal_common_reset(struct platform_device *nss_dev)
{
	return 0;
}

/*
 * __nss_hal_core_reset()
 */
static int __nss_hal_core_reset(struct platform_device *nss_dev, void __iomem *map, uint32_t addr, uint32_t clk_src)
{
	return 0;
}

/*
 * __nss_hal_firmware_load()
 */
static int __nss_hal_firmware_load(struct nss_ctx_instance *nss_ctx, struct platform_device *nss_dev, struct nss_platform_data *npd)
{
	return 0;
}

/*
 * __nss_hal_clock_configure()
 */
static int __nss_hal_clock_configure(struct nss_ctx_instance *nss_ctx, struct platform_device *nss_dev, struct nss_platform_data *npd)
{
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
		nss_warning("%px: IRQ%d request failed", nss_ctx, npd->irq[irq_num]);
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
	 * Nothing to be done as there are no UTCM_SHARED defined for fsm9010
	 */
	return true;
}

/*
 * nss_hal_fsm9010_ops
 */
struct nss_hal_ops nss_hal_fsm9010_ops = {
	.common_reset = __nss_hal_common_reset,
	.core_reset = __nss_hal_core_reset,
	.clock_configure = __nss_hal_clock_configure,
	.firmware_load = __nss_hal_firmware_load,
	.debug_enable = __nss_hal_debug_enable,
	.of_get_pdata = __nss_hal_of_get_pdata,
	.request_irq = __nss_hal_request_irq,
	.send_interrupt = __nss_hal_send_interrupt,
	.enable_interrupt = __nss_hal_enable_interrupt,
	.disable_interrupt = __nss_hal_disable_interrupt,
	.clear_interrupt_cause = __nss_hal_clear_interrupt_cause,
	.read_interrupt_cause = __nss_hal_read_interrupt_cause,
	.init_imem = __nss_hal_init_imem,
	.init_utcm_shared = __nss_hal_init_utcm_shared,
};

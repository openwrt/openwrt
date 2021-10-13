/*
 **************************************************************************
 * Copyright (c) 2015-2020, The Linux Foundation. All rights reserved.
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

/*
 * nss_core.c
 *	NSS driver core APIs source file.
 */

#include "nss_core.h"
#include "nss_hal.h"
#include "nss_log.h"
#include <linux/kernel.h>
#include <linux/notifier.h>	/* for panic_notifier_list */
#include <linux/jiffies.h>	/* for time */
#include "nss_tx_rx_common.h"

#if NSS_MAX_CORES > 2	/* see comment in nss_fw_coredump_notify */
#error	too many NSS Cores: should be 1 or 2
#endif

static struct delayed_work coredump_queuewait;
static struct workqueue_struct *coredump_workqueue;

/*
 * nss_coredump_wait()
 *	reboot (panic) if all finished coredump interrupts will not come.
 *   N2H (C2C) interrupt may get lost during trap, as well NSS may start
 * only one core; so timeout if less than desird core sends back finished
 * coredump interrupt.
 */
static void nss_coredump_wait(struct work_struct *work)
{
	panic("did not get all coredump finished signals\n");
}

/*
 * nss_coredump_init_delay_work()
 *	set a wait function in case coredump finish interrupt lost or
 * only one NSS core is up.
 */
int nss_coredump_init_delay_work(void)
{
	coredump_workqueue = create_singlethread_workqueue("coredump_wait");
	if (!coredump_workqueue) {
		nss_warning("can't set wait: hopefully all int will come\n");
		return -ENOMEM;
	}

	INIT_DELAYED_WORK(&coredump_queuewait, nss_coredump_wait);
	return 0;
}

/*
 * nss_panic_handler()
 *	notification callback register to panic chain
 */
static int nss_panic_handler(struct notifier_block *nb,
			unsigned long action, void *data)
{
	int dumped, timed;
	int i;

	for (i = 0; i < nss_top_main.num_nss; i++) {
		struct nss_ctx_instance *nss_ctx = &nss_top_main.nss[i];
		if (nss_ctx->state & NSS_CORE_STATE_FW_DEAD || !nss_ctx->nmap)
			continue;
		nss_ctx->state |= NSS_CORE_STATE_PANIC;
		nss_hal_send_interrupt(nss_ctx, NSS_H2N_INTR_TRIGGER_COREDUMP);
		nss_warning("panic call NSS FW %px to dump %x\n",
			nss_ctx->nmap, nss_ctx->state);
	}

	/*
	 * wait for FW coredump done: maximum 2 rounds for each core
	 * 200ms per round -- 16MB * 10 over 200MHz 32-bit memory bus
	 * panic will take another 3-5 seconds to reboot, so longer enough.
	 */
	dumped = timed = 0;
	do {
		mdelay(200);
		for (i = 0; i < nss_top_main.num_nss; i++) {
			struct nss_ctx_instance *nss_ctx = &nss_top_main.nss[i];
			if ((nss_ctx->state & NSS_CORE_STATE_FW_DEAD ||
				!nss_ctx->nmap) &&
			    !(nss_ctx->state & NSS_CORE_STATE_FW_DUMP)) {
				nss_ctx->state |= NSS_CORE_STATE_FW_DUMP;
				dumped++;
			}
		}
		if (dumped >= nss_top_main.num_nss) {
			nss_warning("NSS FW dump completed\n");
			break;
		}
	} while (timed++ < nss_top_main.num_nss * 2);

	if (timed >= nss_top_main.num_nss * 2)
		nss_warning("might get %d FW dumped", dumped);

	return	NOTIFY_DONE;
}

static struct notifier_block	nss_panic_nb = {
	.notifier_call = nss_panic_handler,
};

/*
 * nss_coredump_notify_register()
 *	API for nss_init to register coredump notifier to panic chain
 */
void nss_coredump_notify_register(void)
{
	atomic_notifier_chain_register(&panic_notifier_list, &nss_panic_nb);
}

/*
 * nss_fw_coredump_notify()
 *	handler for coredump notification from NSS FW
 */
void nss_fw_coredump_notify(struct nss_ctx_instance *nss_own,
				int intr __attribute__ ((unused)))
{
	int i, j, curr_index, useful_entries, num_cores_wait;
	struct nss_log_descriptor *nld;
	struct nss_log_entry *nle_init, *nle_print;
	dma_addr_t dma_addr;
	uint32_t offset, index;

	nss_warning("%px: COREDUMP %x Baddr %px stat %x",
			nss_own, intr, nss_own->nmap, nss_own->state);
	nss_own->state |= NSS_CORE_STATE_FW_DEAD;
	queue_delayed_work(coredump_workqueue, &coredump_queuewait,
			msecs_to_jiffies(3456));

	/*
	 * If external log buffer is not set, use the nss initial log buffer.
	 */
	nld = (struct nss_log_descriptor *)(nss_rbe[nss_own->id].addr);
	dma_addr = nss_rbe[nss_own->id].dma_addr;
	if (!nld) {
		nld = nss_own->meminfo_ctx.logbuffer;
		dma_addr = nss_own->meminfo_ctx.logbuffer_dma;
	}

	dma_sync_single_for_cpu(NULL, dma_addr, sizeof(struct nss_log_descriptor), DMA_FROM_DEVICE);

	/*
	 * If the current entry is smaller than or equal to the number of NSS_LOG_COREDUMP_LINE_NUM,
	 * only print whatever is in the buffer. Otherwise, dump last NSS_LOG_COREDUMP_LINE_NUM
	 * to the dmessage.
	 */
	nss_info_always("%px: Starting NSS-FW logbuffer dump for core %u\n",
			nss_own, nss_own->id);
	nle_init = nld->log_ring_buffer;
	if (nld->current_entry <= NSS_LOG_COREDUMP_LINE_NUM) {
		curr_index = 0;
		useful_entries = nld->current_entry;
	} else {
		curr_index = ((nld->current_entry - NSS_LOG_COREDUMP_LINE_NUM) % nld->log_nentries);
		useful_entries = NSS_LOG_COREDUMP_LINE_NUM;
	}

	nle_print = nle_init + curr_index;
	for (j = index = curr_index; j < (curr_index + useful_entries); j++, index++) {
		if (j == nld->log_nentries) {
			nle_print = nle_init;
			index = 0;
		}

		offset = (index * sizeof(struct nss_log_entry))
			+ offsetof(struct nss_log_descriptor, log_ring_buffer);
		dma_sync_single_for_cpu(NULL, dma_addr + offset,
				sizeof(struct nss_log_entry), DMA_FROM_DEVICE);
		nss_info_always("%px: %s\n", nss_own, nle_print->message);
		nle_print++;
	}

	if (nss_own->state & NSS_CORE_STATE_PANIC)
		return;

	/*
	 * We need to wait until all other cores finish their dump.
	 */
	num_cores_wait = (nss_top_main.num_nss - 1);
	if (!num_cores_wait) {
		/*
		 * nss_cmd_buf.coredump values:
		 *	0 ==	normal coredump and panic
		 * non-zero value is for debug purpose:
		 *	1 ==	force coredump and panic
		 * otherwise	coredump but do not panic.
		 */
		if (!(nss_cmd_buf.coredump & 0xFFFFFFFE)) {
			panic("NSS FW coredump: bringing system down\n");
		}
		nss_info_always("NSS core dump completed & use mdump to collect dump to debug\n");
		return;
	}

	for (i = 0; i < nss_top_main.num_nss; i++) {
		struct nss_ctx_instance *nss_ctx = &nss_top_main.nss[i];

		/*
		 * Skip waiting for ourselves to coredump, we already have.
		 */
		if (nss_ctx == nss_own) {
			continue;
		}

		/*
		 * Notify any live core to dump.
		 */
		if (!(nss_ctx->state & NSS_CORE_STATE_FW_DEAD) && nss_ctx->nmap) {
			nss_warning("notify NSS FW %px for coredump\n", nss_ctx->nmap);
			nss_hal_send_interrupt(nss_ctx, NSS_H2N_INTR_TRIGGER_COREDUMP);
			continue;
		}

		/*
		 * bit 1 is used for testing coredump. Any other
		 * bit(s) (value other than 0/1) disable panic
		 * in order to use mdump utility: see mdump/src/README
		 * for more info.
		 */
		if (nss_cmd_buf.coredump & 0xFFFFFFFE) {
			nss_info_always("NSS core dump completed and please use mdump to collect dump data\n");
			continue;
		}

		/*
		 * Ideally we need to unregister ourselves from the panic
		 * notifier list before calling the panic to prevent infinite calling.
		 * However, When we tried, we couldn't make it work. Therefore, We just leave the corresponding call here
		 * if it will be needed in the future.
		 *
		 * atomic_notifier_chain_unregister(&panic_notifier_list, &nss_panic_nb);
		 */
		num_cores_wait--;
		if (!num_cores_wait) {
			panic("NSS FW coredump: bringing system down\n");
			return;
		}

	}
}

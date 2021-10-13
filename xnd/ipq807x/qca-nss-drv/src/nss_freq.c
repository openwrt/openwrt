/*
 **************************************************************************
 * Copyright (c) 2013, 2015-2020 The Linux Foundation. All rights reserved.
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
 * nss_freq.c
 *	NSS frequency change APIs
 */

#include "nss_stats.h"
#include "nss_tx_rx_common.h"
#include "nss_freq_log.h"
#include "nss_freq_stats.h"

#define NSS_ACK_STARTED 0
#define NSS_ACK_FINISHED 1

#define NSS_FREQ_USG_AVG_FREQUENCY	1000		/* Time in ms over which CPU Usage is averaged */
#define NSS_FREQ_CPU_USAGE_MAX_BOUND	75		/* MAX CPU usage equivalent to running max instructions excluding all the hazards */
#define NSS_FREQ_CPU_USAGE_MAX		100 		/* MAX CPU usage equivalent to running max instructions including all the hazards.
							   This is also the ideal maximum usage value. */

/*
 * Spinlock to protect the global data structure nss_freq_cpu_status
 */
DEFINE_SPINLOCK(nss_freq_cpu_usage_lock);

/*
 * At any point, this object has the latest data about CPU utilization.
 */
struct nss_freq_cpu_usage nss_freq_cpu_status;

extern struct nss_runtime_sampling nss_runtime_samples;
extern struct workqueue_struct *nss_wq;
extern nss_work_t *nss_work;

/*
 * nss_freq_msg_init()
 *	Initialize the freq message
 */
static void nss_freq_msg_init(struct nss_corefreq_msg *ncm, uint16_t if_num, uint32_t type, uint32_t len,
			void *cb, void *app_data)
{
	nss_cmn_msg_init(&ncm->cm, if_num, type, len, cb, app_data);
}

/*
 * nss_freq_handle_ack()
 *	Handle the nss ack of frequency change.
 */
static void nss_freq_handle_ack(struct nss_ctx_instance *nss_ctx, struct nss_freq_msg *nfa)
{
	if (nfa->ack == NSS_ACK_STARTED) {
		/*
		 * NSS finished start noficiation - HW change clocks and send end notification
		 */
		nss_info("%px: NSS ACK Received: %d - Change HW CLK/Send Finish to NSS\n", nss_ctx, nfa->ack);

		return;
	}

	if (nfa->ack == NSS_ACK_FINISHED) {
		/*
		 * NSS finished end notification - Done
		 */
		nss_info("%px: NSS ACK Received: %d - End Notification ACK - Running: %dmhz\n", nss_ctx, nfa->ack, nfa->freq_current);
		nss_runtime_samples.freq_scale_ready = 1;
		return;
	}

	nss_info("%px: NSS had an error - Running: %dmhz\n", nss_ctx, nfa->freq_current);
}

/*
 * nss_freq_queue_work()
 *	Queue Work to the NSS Workqueue based on Current index.
 */
static bool nss_freq_queue_work(void)
{
	nss_freq_scales_t index = nss_runtime_samples.freq_scale_index;

	BUG_ON(!nss_wq);

	nss_info("frequency:%d index:%d sample count:%x\n", nss_runtime_samples.freq_scale[index].frequency,
					index, nss_runtime_samples.average);

	/*
	 * schedule freq change with autoscale ON
	 */
	return nss_freq_sched_change(index, true);
}

/*
 * nss_freq_get_cpu_usage()
 * 	Returns the CPU usage value in percentage at any instance for a required core. Returns -1 in case of an error.
 *
 * Calculation frequency is 1 second. Range of usage is 0-100. This API returns -1 if CPU usage is requested for core 1.
 * TODO: Extend this API to get CPU usage for core 1.
 */
int8_t nss_freq_get_cpu_usage(uint32_t core_id)
{
	int8_t usage;

	if (core_id == 0) {
		spin_lock_bh(&nss_freq_cpu_usage_lock);
		usage = nss_freq_cpu_status.used;
		spin_unlock_bh(&nss_freq_cpu_usage_lock);

		return usage;
	}

	nss_warning("CPU usage functionality is not supported for core %u\n", core_id);
	return -1;
}

/*
 * nss_freq_compute_cpu_usage()
 * 	Computes the CPU utilization and maximum-minumun cpu utilization since boot.
 */
static void nss_freq_compute_cpu_usage(struct nss_ctx_instance *nss_ctx, uint32_t inst_cnt)
{
	uint32_t estimated_ins_capacity;
	uint8_t actual_usage;
	uint8_t usage;

	spin_lock_bh(&nss_freq_cpu_usage_lock);

	/*
	 * If actual CPU usage turns up higher than 100, there is something wrong with the received data.
	 * Upper bound average varies between 80% usage to 100% usage.
	 *
	 * TODO: To improve estimation algorithm for calculating how many actual instructions are executed.
	 */
	actual_usage = (inst_cnt * 100) / nss_freq_cpu_status.max_ins;
	if ((actual_usage > NSS_FREQ_CPU_USAGE_MAX) || (actual_usage == 0)) {
		spin_unlock_bh(&nss_freq_cpu_usage_lock);
		return;
	}

	/*
	 * Simpler version of below math: This is calculating the reduced number of maximum instructions
	 * estimated_ins_capacity = nss_freq_cpu_status.avg_up% of nss_freq_cpu_status.max_ins
	 * Calculating usage percentage: usage = (inst_cnt/estimated_ins_capacity) * 100
	 */
	estimated_ins_capacity = ((NSS_FREQ_CPU_USAGE_MAX_BOUND * nss_freq_cpu_status.max_ins) / 100);
	if (estimated_ins_capacity == 0) {
		spin_unlock_bh(&nss_freq_cpu_usage_lock);
		return;
	}
	usage = (inst_cnt * 100) / estimated_ins_capacity;

	/*
	 * Average the instructions over NSS_FREQ_USG_AVG_FREQUENCY ms
	 */
	if (nss_freq_cpu_status.avg_ctr == NSS_FREQ_USG_AVG_FREQUENCY) {
		nss_freq_cpu_status.used = nss_freq_cpu_status.total / NSS_FREQ_USG_AVG_FREQUENCY;

		/*
		 * Due to our estimation, this could go beyond the end limit of 100%
		 */
		if (nss_freq_cpu_status.used > NSS_FREQ_CPU_USAGE_MAX) {
			nss_freq_cpu_status.used = NSS_FREQ_CPU_USAGE_MAX;
		}

		/*
		 * Getting the all time max and min usage
		 */
		if (nss_freq_cpu_status.used > nss_freq_cpu_status.max) {
			nss_freq_cpu_status.max = nss_freq_cpu_status.used;
		}

		if (nss_freq_cpu_status.used < nss_freq_cpu_status.min) {
			nss_freq_cpu_status.min = nss_freq_cpu_status.used;
		}

		nss_trace("%px: max_instructions:%d cpu_usage:%d max_usage:%d min_usage:%d\n", nss_ctx,
				nss_freq_cpu_status.max_ins, nss_freq_cpu_status.used, nss_freq_cpu_status.max, nss_freq_cpu_status.min);

		nss_freq_cpu_status.total = 0;
		nss_freq_cpu_status.avg_ctr = 0;
	}

	nss_freq_cpu_status.total += usage;
	nss_freq_cpu_status.avg_ctr++;

	spin_unlock_bh(&nss_freq_cpu_usage_lock);
}

/*
 * nss_freq_scale_frequency()
 * 	Frequency scaling algorithm to scale frequency.
 */
void nss_freq_scale_frequency(struct nss_ctx_instance *nss_ctx, uint32_t inst_cnt)
{
	uint32_t b_index;
	uint32_t minimum;
	uint32_t maximum;
	uint32_t index = nss_runtime_samples.freq_scale_index;

	/*
	 * We do not accept any statistics if auto scaling is off,
	 * we start with a fresh sample set when scaling is
	 * eventually turned on.
	 */
	if (!nss_cmd_buf.auto_scale && nss_runtime_samples.initialized) {
		return;
	}

	/*
	 * Delete Current Index Value, Add New Value, Recalculate new Sum, Shift Index
	 */
	b_index = nss_runtime_samples.buffer_index;

	nss_runtime_samples.sum = nss_runtime_samples.sum - nss_runtime_samples.buffer[b_index];
	nss_runtime_samples.buffer[b_index] = inst_cnt;
	nss_runtime_samples.sum = nss_runtime_samples.sum + nss_runtime_samples.buffer[b_index];
	nss_runtime_samples.buffer_index = (b_index + 1) & NSS_SAMPLE_BUFFER_MASK;

	if (nss_runtime_samples.sample_count < NSS_SAMPLE_BUFFER_SIZE) {
		nss_runtime_samples.sample_count++;

		/*
		 * Samples Are All Ready, Start Auto Scale
		 */
		if (nss_runtime_samples.sample_count == NSS_SAMPLE_BUFFER_SIZE ) {
			nss_cmd_buf.auto_scale = 1;
			nss_runtime_samples.freq_scale_ready = 1;
			nss_runtime_samples.initialized = 1;
		}

		return;
	}

	nss_runtime_samples.average = nss_runtime_samples.sum / nss_runtime_samples.sample_count;

	/*
	 * Print out statistics every 10 samples
	 */
	if (nss_runtime_samples.message_rate_limit++ >= NSS_MESSAGE_RATE_LIMIT) {
		nss_trace("%px: Running AVG:%x Sample:%x Divider:%d\n", nss_ctx, nss_runtime_samples.average, inst_cnt, nss_runtime_samples.sample_count);
		nss_trace("%px: Current Frequency Index:%d\n", nss_ctx, index);
		nss_trace("%px: Auto Scale Ready:%d Auto Scale:%d\n", nss_ctx, nss_runtime_samples.freq_scale_ready, nss_cmd_buf.auto_scale);
		nss_trace("%px: Current Rate:%x\n", nss_ctx, nss_runtime_samples.average);

		nss_runtime_samples.message_rate_limit = 0;
	}

	/*
	 * Don't scale if we are not ready or auto scale is disabled.
	 */
	if ((nss_runtime_samples.freq_scale_ready != 1) || (nss_cmd_buf.auto_scale != 1)) {
		return;
	}

	/*
	 * Scale Algorithmn
	 *	Algorithmn will limit how fast it will transition each scale, by the number of samples seen.
	 *	If any sample is out of scale during the idle count, the rate_limit will reset to 0.
	 *	Scales are limited to the max number of cpu scales we support.
	 */
	if (nss_runtime_samples.freq_scale_rate_limit_up++ >= NSS_FREQUENCY_SCALE_RATE_LIMIT_UP) {
		maximum = nss_runtime_samples.freq_scale[index].maximum;
		if ((nss_runtime_samples.average > maximum) && (index < (NSS_FREQ_MAX_SCALE - 1))) {
			nss_runtime_samples.freq_scale_index++;
			nss_runtime_samples.freq_scale_ready = 0;

			/*
			 * If fail to increase frequency, decrease index
			 */
			nss_trace("frequency increase to %d inst:%x > maximum:%x\n", nss_runtime_samples.freq_scale[nss_runtime_samples.freq_scale_index].frequency, inst_cnt, maximum);
			if (!nss_freq_queue_work()) {
				nss_runtime_samples.freq_scale_index--;
			}
		}

		/*
		 * Reset the down scale counter based on running average, so can idle properly
		 */
		if (nss_runtime_samples.average > maximum) {
			nss_trace("down scale timeout reset running average:%x\n", nss_runtime_samples.average);
			nss_runtime_samples.freq_scale_rate_limit_down = 0;
		}

		nss_runtime_samples.freq_scale_rate_limit_up = 0;
		return;
	}

	if (nss_runtime_samples.freq_scale_rate_limit_down++ >= NSS_FREQUENCY_SCALE_RATE_LIMIT_DOWN) {
		minimum = nss_runtime_samples.freq_scale[index].minimum;

		/*
		 * Check if we need to lower the frequency. For some SoC like IPQ50xx, low frequency
		 * is not supported. So check if the next lower frequency is configured before shifting down
		 */
		if ((nss_runtime_samples.average < minimum) && (index > 0) && nss_runtime_samples.freq_scale[index - 1].maximum) {
			nss_runtime_samples.freq_scale_index--;
			nss_runtime_samples.freq_scale_ready = 0;

			/*
			 * If fail to decrease frequency, increase index
			 */
			nss_trace("frequency decrease to %d inst:%x < minumum:%x\n", nss_runtime_samples.freq_scale[nss_runtime_samples.freq_scale_index].frequency, nss_runtime_samples.average, minimum);
			if (!nss_freq_queue_work()) {
				nss_runtime_samples.freq_scale_index++;
			}
		}
		nss_runtime_samples.freq_scale_rate_limit_down = 0;
		return;
	}
}

/*
 *  nss_freq_handle_core_stats()
 *	Handle the core stats.
 */
static void nss_freq_handle_core_stats(struct nss_ctx_instance *nss_ctx, struct nss_core_stats *core_stats)
{
	uint32_t inst_cnt = core_stats->inst_cnt_total;

	/*
	 * compute CPU utilization by using the instruction count
	 */
	nss_freq_compute_cpu_usage(nss_ctx, inst_cnt);

	/*
	 * Perform frequency scaling
	 */
	nss_freq_scale_frequency(nss_ctx, inst_cnt);
}

/*
 * nss_freq_interface_handler()
 *	Handle NSS -> HLOS messages for Frequency Changes and Statistics.
 */
static void nss_freq_interface_handler(struct nss_ctx_instance *nss_ctx, struct nss_cmn_msg *ncm, __attribute__((unused))void *app_data) {

	struct nss_corefreq_msg *ncfm = (struct nss_corefreq_msg *)ncm;

	/*
	 * Trace Messages
	 */
	nss_freq_log_rx_msg(ncfm);

	switch (ncfm->cm.type) {
	case COREFREQ_METADATA_TYPE_TX_FREQ_ACK:
		nss_freq_handle_ack(nss_ctx, &ncfm->msg.nfc);
		break;
	case COREFREQ_METADATA_TYPE_TX_CORE_STATS:
		nss_freq_handle_core_stats(nss_ctx, &ncfm->msg.ncs);
		break;

	default:
		if (ncm->response != NSS_CMN_RESPONSE_ACK) {
			/*
			 * Check response
			 */
			nss_info("%px: Received response %d for type %d, interface %d", nss_ctx, ncm->response, ncm->type, ncm->interface);
		}
	}
}

/*
 * nss_freq_change()
 *	NSS frequency change API.
 */
nss_tx_status_t nss_freq_change(struct nss_ctx_instance *nss_ctx, uint32_t eng, uint32_t stats_enable, uint32_t start_or_end)
{
	struct nss_corefreq_msg ncm;
	struct nss_freq_msg *nfc;

	nss_info("%px: frequency changing to: %d\n", nss_ctx, eng);

	/*
	 * Update the max instruction count for a frequency during down scaling.
	 * Better to update this as late as possible in the frequency update call.
	 */
	spin_lock_bh(&nss_freq_cpu_usage_lock);
	nss_freq_cpu_status.max_ins = eng / 1000;
	spin_unlock_bh(&nss_freq_cpu_usage_lock);

	nss_freq_msg_init(&ncm, NSS_COREFREQ_INTERFACE, NSS_TX_METADATA_TYPE_NSS_FREQ_CHANGE,
				sizeof(struct nss_freq_msg), NULL, NULL);
	nfc = &ncm.msg.nfc;
	nfc->frequency = eng;
	nfc->start_or_end = start_or_end;
	nfc->stats_enable = stats_enable;

	return nss_core_send_cmd(nss_ctx, &ncm, sizeof(ncm), NSS_NBUF_PAYLOAD_SIZE);
}

/*
 * nss_freq_sched_change()
 *	Schedule a frequency work.
 */
bool nss_freq_sched_change(nss_freq_scales_t index, bool auto_scale)
{
	if (index >= NSS_FREQ_MAX_SCALE) {
		nss_info("NSS freq scale beyond limit\n");
		return false;
	}

	nss_work = (nss_work_t *)kmalloc(sizeof(nss_work_t), GFP_ATOMIC);
	if (!nss_work) {
		nss_info("NSS Freq WQ kmalloc fail");
		return false;
	}

	INIT_WORK((struct work_struct *)nss_work, nss_hal_wq_function);

	nss_work->frequency = nss_runtime_samples.freq_scale[index].frequency;

	nss_work->stats_enable = auto_scale;
	nss_cmd_buf.current_freq = nss_work->frequency;
	queue_work(nss_wq, (struct work_struct *)nss_work);

	return true;
}

/*
 * nss_freq_get_context()
 *	Get NSS context instance for frequency.
 */
struct nss_ctx_instance *nss_freq_get_context(void)
{
	return (struct nss_ctx_instance *)&nss_top_main.nss[nss_top_main.frequency_handler_id];
}
EXPORT_SYMBOL(nss_freq_get_context);

/*
 * nss_freq_register_handler()
 */
void nss_freq_register_handler(void)
{
	struct nss_ctx_instance *nss_ctx = nss_freq_get_context();
	nss_core_register_handler(nss_ctx, NSS_COREFREQ_INTERFACE, nss_freq_interface_handler, NULL);
}

/*
 * nss_freq_cpu_usage_init()
 * 	Initialize cpu usage computing.
 *
 * TODO: Add support to retrieve CPU usage even if frequency scaling is disabled.
 */
void nss_freq_init_cpu_usage(void)
{
	nss_freq_cpu_status.used = 0;
	nss_freq_cpu_status.max_ins = nss_runtime_samples.freq_scale[nss_runtime_samples.freq_scale_index].frequency / 1000;
	nss_freq_cpu_status.total = 0;
	nss_freq_cpu_status.max = 0;					/* Initial value is 0 to capture the highest most value during the run */
	nss_freq_cpu_status.min = NSS_FREQ_CPU_USAGE_MAX;		/* Initial value is 100 to capture the lowest most value during the run */
	nss_freq_cpu_status.avg_up = NSS_FREQ_CPU_USAGE_MAX_BOUND;
	nss_freq_cpu_status.avg_ctr = 0;

	nss_freq_stats_dentry_create();
}

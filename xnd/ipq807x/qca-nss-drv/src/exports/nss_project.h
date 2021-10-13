/*
 **************************************************************************
 * Copyright (c) 2017-2018, The Linux Foundation. All rights reserved.
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
 * @file nss_project.h
 *	NSS project interface definitions.
 */

#ifndef __NSS_PROJECT_H
#define __NSS_PROJECT_H

/**
 * @addtogroup nss_project_subsystem
 * @{
 */

 /**
  * Maximum number of IRQs for which a message will have statistics.
  *
  * Must be defined on firmware and host such that NSS_PROJECT_IRQS_PER_MESSAGE *
  * sizeof(struct nss_project_irq_stats) + 8 + sizeof(struct nss_cmn_msg) is smaller
  * than the maximum payload size of an sk_buff (1792), 8 being the number of
  * bytes needed to store the thread number and number of statistics written.
  */
#define NSS_PROJECT_IRQS_PER_MESSAGE 32

/**
 * nss_project_message_types
 *	Project message types.
 */
enum nss_project_message_types {
	NSS_PROJECT_MSG_WT_STATS_ENABLE,
			/**< Message to enable or disable worker thread statistics. */
	NSS_PROJECT_MSG_WT_STATS_NOTIFY,
			/**< NSS to HLOS message containing worker thread statistics. */
	NSS_PROJECT_MSG_MAX,
};

/**
 * nss_project_error_types
 *	Project error types.
 */
enum nss_project_error_types {
	NSS_PROJECT_ERROR_UNKNOWN_MSG,
			/**< Unrecognized message type. */
	NSS_PROJECT_ERROR_WT_STATS_UNSUPPORTED,
			/**< The firmware does not support worker thread statistics. */
	NSS_PROJECT_ERROR_WT_STATS_REDUNDANT_ENABLE,
			/**< The firmware received a redundant request to enable worker thread statistics. */
	NSS_PROJECT_ERROR_MAX,
};

/**
 * nss_project_msg_wt_stats_enable
 *	Enables or disables worker thread statistics collection.
 */
struct nss_project_msg_wt_stats_enable {

	/*
	 * NSS to HLOS
	 */
	uint32_t worker_thread_count;
			/**< Number of worker threads supported by this core. */
	uint32_t irq_count;
			/**< Number of IRQs supported by this core. */

	/*
	 * HLOS to NSS
	 */
	bool enable;	/**< True to enable, false to disable. */
};

/**
 * nss_project_irq_stats
 *	Statistics for an individual IRQ on a worker thread.
 */
struct nss_project_irq_stats {
	uint64_t count;		/**< Number of times callback has been executed */
	uint32_t callback;	/**< Address of the callback function */
	uint32_t irq;		/**< IRQ number to which callback function is bound */
	uint32_t ticks_min;	/**< Fewest ticks taken in callback function */
	uint32_t ticks_avg;	/**< Exponential moving average of ticks */
	uint32_t ticks_max;	/**< Maximum ticks */
	uint32_t insn_min;	/**< Fewest instructions executed in callback function */
	uint32_t insn_avg;	/**< Exponential moving average of instruction count */
	uint32_t insn_max;	/**< Maximum instructions */
};

/**
 * nss_project_msg_wt_stats_notify
 *	Message containing statistics for active worker_thread IRQs.
 */
struct nss_project_msg_wt_stats_notify {
	uint32_t threadno;	/**< The thread whose stats are contained. */
	uint32_t stats_written;	/**< The number of statistics written to the array. */
	struct nss_project_irq_stats stats[NSS_PROJECT_IRQS_PER_MESSAGE];
				/**< The per-IRQ statistics for the worker thread */
};

/**
 * nss_project_msg
 *	General message structure for project messages.
 */
struct nss_project_msg {
	struct nss_cmn_msg cm;	/**< Common message header. */

	/**
	 * Payload of a message to or from the project code.
	 */
	union {
		struct nss_project_msg_wt_stats_enable wt_stats_enable;
				/**< Enable or disable worker thread statistics. */
		struct nss_project_msg_wt_stats_notify wt_stats_notify;
				/**< One-way worker thread statistics message. */
	} msg;			/**< Message payload. */
};

/**
 * Callback function for receiving project messages.
 *
 * @datatypes
 * nss_project_msg
 *
 * @param[in] app_data  Pointer to the application context of the message.
 * @param[in] msg       Pointer to the project message.
 */
typedef void (*nss_project_msg_callback_t)(void *app_data, struct nss_project_msg *msg);

/**
 * nss_project_register_sysctl
 *	Registers the project sysctl table to the sysctl tree.
 *
 * @return
 * None.
 */
void nss_project_register_sysctl(void);

/**
 * nss_project_unregister_sysctl
 *	De-registers the project sysctl table from the sysctl tree.
 *
 * @return
 * None.
 *
 * @dependencies
 * The system control must have been previously registered.
 */
void nss_project_unregister_sysctl(void);

/**
 * nss_project_register_handler
 *	Registers the project message handler.
 *
 * @return
 * None.
 */
void nss_project_register_handler(struct nss_ctx_instance *nss_ctx);

/**
 * @}
 */

#endif /* __NSS_PROJECT_H */

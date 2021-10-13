/*
 **************************************************************************
 * Copyright (c) 2016-2018, 2020-2021, The Linux Foundation. All rights reserved.
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
 * @file nss_ppe.h
 *	NSS PPE interface definitions.
 */

#ifndef _NSS_PPE_H_
#define _NSS_PPE_H_

typedef int32_t nss_ppe_port_t;

/**
 * @addtogroup nss_ppe_subsystem
 * @{
 */

/*
 * NSS PORT defines
 */
#define NSS_PPE_NUM_PHY_PORTS_MAX		8
		/**< Maximum number of PPE physical ports. */
#define NSS_PPE_PORT_IPSEC			7
		/**< Port number of PPE inline IPsec port. */

/**
 * nss_ppe_sc_type
 *	PPE service code types.
 */
enum nss_ppe_sc_type {
	NSS_PPE_SC_NONE,		/**< Normal PPE processing. */
	NSS_PPE_SC_BYPASS_ALL,		/**< Bypasses all stages in PPE. */
	NSS_PPE_SC_ADV_QOS_BRIDGED,	/**< Advance QoS redirection for bridged flow. */
	NSS_PPE_SC_BR_QOS,		/**< Bridge QoS redirection. */
	NSS_PPE_SC_BNC_0,		/**< QoS bounce. */
	NSS_PPE_SC_BNC_CMPL_0,		/**< QoS bounce complete. */
	NSS_PPE_SC_ADV_QOS_ROUTED,	/**< Advance QoS redirection for routed flow. */
	NSS_PPE_SC_IPSEC_PPE2EIP,	/**< Inline IPsec redirection from PPE-to-EIP. */
	NSS_PPE_SC_IPSEC_EIP2PPE,	/**< Inline IPsec redirection from EIP-to-PPE. */
	NSS_PPE_SC_PTP,			/**< Service code for PTP packets. */
	NSS_PPE_SC_VLAN_FILTER_BYPASS,	/**< VLAN filter bypass for bridge flows between 2 different VSIs. */
	NSS_PPE_SC_L3_EXCEPT,		/**< Indicate exception post tunnel/TAP operation. */
	NSS_PPE_SC_SPF_BYPASS,		/**< Source port filtering bypass in PPE. */
	NSS_PPE_SC_MAX,			/**< Maximum service code. */
};

/**
 * nss_ppe_stats_unregister_notifier
 *	Deregisters a statistics notifier.
 *
 * @datatypes
 *	notifier_block
 *
 * @param[in] nb Notifier block.
 *
 * @return
 * 0 on success or non-zero on failure.
 */
extern int nss_ppe_stats_unregister_notifier(struct notifier_block *nb);

/**
 * nss_ppe_stats_register_notifier
 *	Registers a statistics notifier.
 *
 * @datatypes
 *	notifier_block
 *
 * @param[in] nb Notifier block.
 *
 * @return
 * 0 on success or non-zero on failure.
 */
extern int nss_ppe_stats_register_notifier(struct notifier_block *nb);

/** @} */ /* end_addtogroup nss_ppe_subsystem */

#endif /* _NSS_PPE_H_ */

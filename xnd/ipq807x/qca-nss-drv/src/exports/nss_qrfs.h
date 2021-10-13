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
 * @file nss_qrfs.h
 *	NSS QRFS interface definitions.
 */

#ifndef __NSS_QRFS_H
#define __NSS_QRFS_H

/**
 * @addtogroup nss_qrfs_subsystem
 * @{
 */

/**
 * nss_qrfs_msg_types
 *	Message types for the NSS QRFS.
 */
enum nss_qrfs_msg_types {
	NSS_QRFS_MSG_FLOW_ADD,
	NSS_QRFS_MSG_FLOW_DELETE,
	NSS_QRFS_MSG_MAC_ADD,
	NSS_QRFS_MSG_MAC_DELETE,
	NSS_QRFS_MSG_STATS_SYNC,
	NSS_QRFS_MSG_MAX,
};

/**
 * nss_qrfs_error_types
 *	Error types for the NSS QRFS.
 */
enum nss_qrfs_error_types {
	NSS_QRFS_ERROR_INVALID_MSG_TYPE,
	NSS_QRFS_ERROR_INVALID_MSG_SIZE,
	NSS_QRFS_ERROR_INVALID_IP_VERSION,
	NSS_QRFS_ERROR_V4_FLOW_TABLE_FULL,
	NSS_QRFS_ERROR_V6_FLOW_TABLE_FULL,
	NSS_QRFS_ERROR_MAC_TABLE_FULL,
	NSS_QRFS_ERROR_MAX,
};

/**
 * nss_qrfs_flow_rule_msg
 *	Information for the NSS QRFS flow rule message.
 */
struct nss_qrfs_flow_rule_msg {
	uint16_t src_port;		/**< Source port. */
	uint16_t dst_port;		/**< Destination port. */
	uint32_t ip_version;		/**< IPv4:4 IPv6:6. */
	uint32_t src_addr[4];		/**< Source IP address. */
	uint32_t dst_addr[4];		/**< Destination IP address. */
	uint16_t protocol;		/**< IP protocol. */
	uint16_t cpu;			/**< CPU core ID. */
	uint32_t if_num;		/**< Physical interface number. */
};

/**
 * nss_qrfs_mac_rule_msg
 *	Information for the NSS QRFS MAC rule message.
 */
struct nss_qrfs_mac_rule_msg {
	uint8_t mac[ETH_ALEN];	/**< Ethernet address. */
	uint16_t cpu;		/**< CPU core ID. */
	uint32_t if_num;	/**< Physical interface number. */
};

/**
 * nss_qrfs_stats_sync_msg
 *	Information for the NSS QRFS statistics message.
 */
struct nss_qrfs_stats_sync_msg {
	struct nss_cmn_node_stats node_stats;	/**< Common pnode statistics. */
	uint32_t invalid_offset;		/**< Packets with invalid offset. */
	uint32_t unknown_protocol;		/**< Protocol other than TCP, UDP. */
	uint32_t ipv4_flow_rule_hits;		/**< Number of IPv4 flow rule hits. */
	uint32_t ipv6_flow_rule_hits;		/**< Number of IPv6 flow rule hits. */
};

/**
 * nss_qrfs_msg
 *	Data for sending and receiving NSS QRFS rule or statistics messages.
 */
struct nss_qrfs_msg {
	struct nss_cmn_msg cm;	/**< Common message header. */

	/**
	 * Payload of a NSS QRFS rule or statistics message.
	 */
	union {
		struct nss_qrfs_flow_rule_msg flow_add;		/**< Add flow rule. */
		struct nss_qrfs_flow_rule_msg flow_delete;	/**< Delete flow rule. */
		struct nss_qrfs_mac_rule_msg mac_add;		/**< Add MAC rule. */
		struct nss_qrfs_mac_rule_msg mac_delete;	/**< Delete MAC rule. */
		struct nss_qrfs_stats_sync_msg stats_sync;	/**< Synchronize statistics. */
	} msg;			/**< Message payload. */
};

/**
 * Callback function for receiving QRFS messages.
 *
 * @datatypes
 * nss_qrfs_msg
 *
 * @param[in] app_data Pointer to the application context of the message.
 * @param[in] msg      Pointer to the message data.
 */
typedef void (*nss_qrfs_msg_callback_t)(void *app_data, struct nss_qrfs_msg *msg);

/**
 * nss_qrfs_register_handler
 *	Registers the QRFS interface with the NSS for sending and receiving
 *	messages.
 *
 * @datatypes
 * nss_ctx_instance
 *
 * @return
 * None.
 */
void nss_qrfs_register_handler(struct nss_ctx_instance *nss_ctx);

/**
 * nss_qrfs_notify_register
 *	Registers a notifier callback for QRFS messages with the NSS.
 *
 * @datatypes
 * nss_qrfs_msg_callback_t
 *
 * @param[in] core      NSS core number index to the notifier callback table.
 * @param[in] cb        Callback function for the message.
 * @param[in] app_data  Pointer to the application context of the message.
 *
 * @return
 * Pointer to the NSS core context.
 */
struct nss_ctx_instance *nss_qrfs_notify_register(int core, nss_qrfs_msg_callback_t cb, void *app_data);

/**
 * nss_qrfs_notify_unregister
 *	Deregisters a QRFS message notifier callback from the NSS.
 *
 * @param[in] core  NSS core number index to the notifier callback table.
 *
 * @return
 * None.
 */
void nss_qrfs_notify_unregister(int core);

/**
 * nss_qrfs_set_flow_rule
 *	Sends a QRFS message to the NSS core to set the flow rule.
 *
 * @datatypes
 * sk_buff
 *
 * @param[in] skb     Pointer to the SKB buffer.
 * @param[in] cpu     CPU number to set in the flow table.
 * @param[in] action  Action to perform on the flow table.
 *
 * @return
 * Status of the Tx operation.
 */
nss_tx_status_t nss_qrfs_set_flow_rule(struct sk_buff *skb, uint32_t cpu, uint32_t action);

/**
 * nss_qrfs_init
 *	Initializes the QRFS.
 *
 * @return
 * None.
 */
void nss_qrfs_init(void);

/**
 * @}
 */

#endif /* __NSS_QRFS_H */

/*
 **************************************************************************
 * Copyright (c) 2019, The Linux Foundation. All rights reserved.
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
 * @file nss_igs.h
 *	NSS ingress shaper interface definitions.
 */

#ifndef _NSS_IGS_H_
#define _NSS_IGS_H_

#ifndef DOXYGEN_SHOULD_SKIP_THIS
#ifdef CONFIG_NET_CLS_ACT
#include <net/act_api.h>
#endif
#endif

/**
 * @addtogroup nss_ingress_shaper_subsystem
 * @{
 */

/**
 * Maximum number of supported ingress shaping interfaces.
 */
#define NSS_MAX_IGS_DYNAMIC_INTERFACES 8

/**
 * nss_igs_msg_types
 *	Message types for ingress shaper requests and responses.
 */
enum nss_igs_msg_types {
	NSS_IGS_MSG_SYNC_STATS = NSS_IF_MAX_MSG_TYPES + 1,
	NSS_IGS_MSG_MAX
};

/**
 * nss_igs_node_stats
 *	Ingress shaping node debug statistics structure.
 */
struct nss_igs_node_stats {
	uint32_t tx_dropped;		/**< Dropped post shaping. */
	uint32_t shaper_drop;		/**< Dropped during shaper enqueue. */
	uint32_t ipv4_parse_fail;	/**< IPv4 parse fail. */
	uint32_t ipv4_unknown_gre_type;	/**< IPv4 unknown GRE type. */
	uint32_t ipv4_unknown_l4;	/**< IPv4 unknown L4 type. */
	uint32_t ipv4_no_cme;		/**< IPv4 connection match entry not found. */
	uint32_t ipv4_frag_initial;	/**< IPv4 initial fragment. */
	uint32_t ipv4_frag_non_initial;	/**< Ipv4 subsequent fragment. */
	uint32_t ipv4_malformed_udp;	/**< Incomplete IPv4 UDP packet. */
	uint32_t ipv4_malformed_tcp;	/**< Incomplete IPv4 TCP packet. */
	uint32_t ipv4_malformed_udpl;	/**< Incomplete IPv4 UDP-Lite packet. */
	uint32_t ipv4_malformed_gre;	/**< Incomplete IPv4 GRE packet. */
	uint32_t ipv6_parse_fail;	/**< IPv6 parse fail. */
	uint32_t ipv6_unknown_l4;	/**< IPv6 unknown L4 type. */
	uint32_t ipv6_no_cme;		/**< IPv6 connection match entry not found. */
	uint32_t ipv6_frag_initial;	/**< IPv6 initial fragment. */
	uint32_t ipv6_frag_non_initial;	/**< Ipv6 subsequent fragment. */
	uint32_t ipv6_malformed_udp;	/**< Incomplete IPv6 UDP packet. */
	uint32_t ipv6_malformed_tcp;	/**< Incomplete IPv6 TCP packet. */
	uint32_t ipv6_malformed_udpl;	/**< Incomplete IPv6 UDP-Lite packet. */
	uint32_t ipv6_malformed_frag;	/**< Incomplete IPv6 fragment. */
	uint32_t event_no_si;		/**< No shaper configured. */
	uint32_t eth_parse_fail;	/**< Ethernet header parse failed. */
	uint32_t eth_unknown_type;	/**< Non-IP/PPPoE ether type. */
	uint32_t pppoe_non_ip;		/**< Non-IP PPPoE packet. */
	uint32_t pppoe_malformed;	/**< Incomplete PPPoE packet. */
};

/**
 * nss_igs_stats_sync_msg
 *	Message information for ingress shaping synchronization statistics.
 */
struct nss_igs_stats_sync_msg {
	struct nss_cmn_node_stats node_stats;	/**< Common node statistics. */
	struct nss_igs_node_stats igs_stats;	/**< Debug statistics for ingress shaping. */
};

/**
 * nss_igs_msg
 *	Data for sending and receiving ingress shaper messages.
 */
struct nss_igs_msg {
	struct nss_cmn_msg cm;		/**< Common message header. */

	/**
	 * Payload of a ingress shaper message.
	 */
	union {
		union nss_if_msgs if_msg;
				/**< NSS interface base message. */
		struct nss_igs_stats_sync_msg stats;
				/**< Statistics message to host. */
	} msg;			/**< Message payload. */
};

/**
 * Callback function for receiving ingress shaper messages.
 *
 * @datatypes
 * nss_cmn_msg
 *
 * @param[in] app_data  Pointer to the application context of the message.
 * @param[in] msg       Pointer to the message data.
 */
typedef void (*nss_igs_msg_callback_t)(void *app_data, struct nss_cmn_msg *msg);

/**
 * nss_igs_get_context
 *	Gets the ingress shaper context.
 *
 * @return
 * Pointer to the NSS core context.
 */
extern struct nss_ctx_instance *nss_igs_get_context(void);

/**
 * nss_igs_register_if
 *	Registers a ingress shaper interface with the NSS for sending and receiving messages.
 *
 * @datatypes
 * nss_igs_msg_callback_t \n
 * net_device
 *
 * @param[in] if_num          NSS interface number.
 * @param[in] type            NSS interface type.
 * @param[in] msg_callback    Callback for the ingress shaper message.
 * @param[in] netdev          Pointer to the associated network device.
 * @param[in] features        Data socket buffer types supported by this interface.
 *
 * @return
 * Pointer to the NSS core context.
 */
extern struct nss_ctx_instance *nss_igs_register_if(uint32_t if_num, uint32_t type,
		nss_igs_msg_callback_t msg_callback, struct net_device *netdev, uint32_t features);

/**
 * nss_igs_unregister_if
 *	Deregisters a ingress shaper interface from the NSS.
 *
 * @param[in] if_num  NSS interface number.
 *
 * @return
 * None.
 */
extern void nss_igs_unregister_if(uint32_t if_num);

/**
 * nss_igs_verify_if_num
 *	Verify whether interface is an ingress shaper interface or not.
 *
 * @param[in] if_num  NSS interface number.
 *
 * @return
 * True if interface is an ingress shaper interface.
 */
extern bool nss_igs_verify_if_num(uint32_t if_num);


#ifdef CONFIG_NET_CLS_ACT
/*
 * nss_igs_module_save()
 *	Save the ingress shaping module reference.
 *
 * @datatypes
 * tc_action_ops \n
 * module
 *
 * @param[in] act     Operation structure for ingress shaping action.
 * @param[in] module  Module structure of ingress shaping module.
 *
 * @return
 * None.
 */
extern void nss_igs_module_save(struct tc_action_ops *act, struct module *module);
#endif

/*
 * nss_igs_module_get()
 *	Get the ingress shaping module reference.
 *
 * @return
 * False if not able to take the ingress shaping module reference, otherwise true.
 *
 */
extern bool nss_igs_module_get(void);

/*
 * nss_igs_module_put()
 *	Release the ingress shaping module reference.
 *
 * @return
 * None.
 */
extern void nss_igs_module_put(void);

/**
 * @}
 */
#endif

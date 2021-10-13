/*
 **************************************************************************
 * Copyright (c) 2014-2017, The Linux Foundation. All rights reserved.
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
 * @file nss_ipsecmgr.h
 *	NSS IPSec Manager interface definitions.
 */

#ifndef __NSS_IPSECMGR_H
#define __NSS_IPSECMGR_H

/**
 * @addtogroup nss_ipsec_subsystem
 * @{
 */

#define NSS_IPSECMGR_DEBUG_LVL_ERROR 1		/**< Turn on debug for an error. */
#define NSS_IPSECMGR_DEBUG_LVL_WARN 2		/**< Turn on debug for a warning. */
#define NSS_IPSECMGR_DEBUG_LVL_INFO 3		/**< Turn on debug for information. */
#define NSS_IPSECMGR_DEBUG_LVL_TRACE 4		/**< Turn on debug for trace. */

#define NSS_IPSECMGR_TUN_NAME "ipsectun%d"
		/**< IPsec tunnel name. */
#define NSS_IPSECMGR_MAX_TUNNELS (NSS_CRYPTO_MAX_IDXS/2)
		/**< Maximum number of IPsec tunnels. */

/**
 * Length of the header added after encapsulation.
 *
 * This estimate must be accurate but large enough to accomodate most use cases.
 */
#define NSS_IPSECMGR_TUN_MAX_HDR_LEN 96

/*
 * Space required in the head and tail of the buffer
 */
#define NSS_IPSECMGR_TUN_HEADROOM 128		/**< Size of the buffer headroom. */
#define NSS_IPSECMGR_TUN_TAILROOM 192		/**< Size of the buffer tailroom. */

#define NSS_IPSECMGR_TUN_MTU(x) (x - NSS_IPSECMGR_TUN_MAX_HDR_LEN)
		/**< MTU of the IPsec tunnel. */

#define NSS_IPSECMGR_NATT_PORT_DATA 4500	/**< Number of the NATT port. */

#define NSS_IPSECMGR_MIN_REPLAY_WIN 32		/**< Minimum size of the replay window. */
#define NSS_IPSECMGR_MAX_REPLAY_WIN 1024	/**< Maximum size of the replay window. */
#define NSS_IPSECMGR_MAX_ICV_LEN 32		/**< Maximum size of the ICV. */
#define NSS_IPSECMGR_MAX_DSCP 63		/**< Maximum size of the descriptor. */

/**
 * nss_ipsecmgr_flow_type
 *	Flow types for the IPsec manager.
 */
enum nss_ipsecmgr_flow_type {
	NSS_IPSECMGR_FLOW_TYPE_NONE = 0,
	NSS_IPSECMGR_FLOW_TYPE_V4_TUPLE = 1,
	NSS_IPSECMGR_FLOW_TYPE_V6_TUPLE = 2,
	NSS_IPSECMGR_FLOW_TYPE_V4_SUBNET = 3,
	NSS_IPSECMGR_FLOW_TYPE_V6_SUBNET = 4,
	NSS_IPSECMGR_FLOW_TYPE_MAX
};

/**
 * nss_ipsecmgr_sa_type
 *	Security association types for the IPsec manager.
 */
enum nss_ipsecmgr_sa_type {
	NSS_IPSECMGR_SA_TYPE_NONE = 0,
	NSS_IPSECMGR_SA_TYPE_V4 = 1,
	NSS_IPSECMGR_SA_TYPE_V6 = 2,
	NSS_IPSECMGR_SA_TYPE_MAX
};

/**
 * nss_ipsecmgr_event_type
 *	Event types for the IPsec manager.
 */
enum nss_ipsecmgr_event_type {
	NSS_IPSECMGR_EVENT_NONE = 0,
	NSS_IPSECMGR_EVENT_SA_STATS,
	NSS_IPSECMGR_EVENT_MAX
};

/**
 * nss_ipsecmgr_sa_v4
 *	IPv4 security associations for the IPsec manager.
 */
struct nss_ipsecmgr_sa_v4 {
	uint32_t src_ip;		/**< IPv4 source IP. */
	uint32_t dst_ip;		/**< IPv4 destination IP. */
	uint32_t ttl;			/**< IPv4 time-to-live. */
	uint32_t spi_index;		/**< ESP SPI index. */
};

/**
 * nss_ipsecmgr_sa_v6
 *	IPv6 security associations for the IPsec manager.
 */
struct nss_ipsecmgr_sa_v6 {
	uint32_t src_ip[4];	/**< IPv6 source IP. */
	uint32_t dst_ip[4];	/**< IPv6 destination IP. */
	uint32_t hop_limit;	/**< IPv6 hop limit. */
	uint32_t spi_index;	/**< SPI index of the encapsulating security payload (ESP). */
};

/**
 * nss_ipsecmgr_sa_data
 *	Security association data for the IPsec manager.
 *
 * For DSCP marking, use the following settings:
 * - Copy inner header to outer header:
 *    - dscp_copy = 1
 *    - dscp = 0
 * - Fixed mark on outer header:
 *    - dscp_copy = 0
 *    - dscp = <0 to 63>
 */
struct nss_ipsecmgr_sa_data {
	uint32_t crypto_index;	/**< Crypto session index returned by the driver. */

	/**
	 * Security association data for the IPsec manager.
	 */
	struct {
		uint16_t replay_win;
				/**< Sequence number window size for anti-replay. */
		uint8_t icv_len;
				/**< Hash length. */
		uint8_t dscp;
				/**< Default DSCP value of the security association. */

		bool dscp_copy;
				/**< Copy DSCP from the inner header to the outer header. */
		bool nat_t_req;
				/**< NAT-T is required. */
		bool seq_skip;
				/**< Skip the ESP sequence for encapsulation. */
		bool trailer_skip;
				/**< Skip the ESP trailer for encapsulation. */
		bool df_copy;
				/**< Copy DF from the inner header to the outer header. */
		uint8_t df;
				/**< DF value for the outer header, if nocopy is selected. */
	} esp;		/**< Payload of security association data. */

	bool enable_esn;	/**< Enable the extended sequence number. */
	bool use_pattern;	/**< Use a random pattern in a hash calculation. */
	uint32_t fail_hash_thresh;	/**< Threshold for consecutive hash failure. */
};

/**
 * nss_ipsecmgr_encap_v4_tuple
 *	IPv4 encapsulation flow tuple for the IPsec manager.
 */
struct nss_ipsecmgr_encap_v4_tuple {
	uint32_t src_ip;		/**< Source IP. */
	uint32_t dst_ip;		/**< Destination IP. */
	uint32_t protocol;		/**< Protocol. */
};

/**
 * nss_ipsecmgr_encap_v6_tuple
 *	IPv6 encapsulation flow tuple for the IPsec manager.
 */
struct nss_ipsecmgr_encap_v6_tuple {
	uint32_t src_ip[4];		/**< Source IP. */
	uint32_t dst_ip[4];		/**< Destination IP. */
	uint32_t next_hdr;		/**< Transport layer protocol. */
};

/**
 * nss_ipsecmgr_encap_v4_subnet
 *	IPv4 encapsulation flow subnet for the IPsec manager.
 */
struct nss_ipsecmgr_encap_v4_subnet {
	uint32_t dst_subnet;		/**< Destination subnet. */
	uint32_t dst_mask;		/**< Destination subnet mask. */
	uint32_t protocol;		/**< IPv4 or IPv6 protocol. */
};

/**
 * nss_ipsecmgr_encap_v6_subnet
 *	IPv6 encapsulation flow subnet for the IPsec manager.
 *
 * Store least significant word in dst_subnet[0] and the most significant word
 * in dst_subnet[3].
 */
struct nss_ipsecmgr_encap_v6_subnet {
	uint32_t dst_subnet[4];		/**< Destination subnet. */
	uint32_t dst_mask[4];		/**< Destination subnet mask. */
	uint32_t next_hdr;		/**< Transport layer protocol. */
};

/**
 * nss_ipsecmgr_sa
 *	Security association information for the IPsec manager.
 */
struct nss_ipsecmgr_sa {
	enum nss_ipsecmgr_sa_type type;		/**< Security association type. */

	/**
	 * IPsec manager security association data.
	 */
	union {
		struct nss_ipsecmgr_sa_v4 v4;	/**< IPv4 security association. */
		struct nss_ipsecmgr_sa_v6 v6;	/**< IPv6 security association. */
	} data;		/**< IPsec manager security association data. */
};

/**
 * nss_ipsecmgr_sa_stats
 *	Security association statistics exported by the IPsec manager.
 */
struct nss_ipsecmgr_sa_stats {
	struct nss_ipsecmgr_sa sa;	/**< Security association information. */
	uint32_t crypto_index;		/**< Crypto session index. */

	/**
	 * Security association statistics used by the IPsec manager.
	 */
	struct {
		uint32_t bytes;		/**< Number of bytes processed. */
		uint32_t count;		/**< Number of packets processed. */
	} pkts;		/**< Processing statistics. */

	uint64_t seq_num;		/**< Current sequence number. */
	uint64_t window_max;		/**< Maximum size of the window. */
	uint32_t window_size;		/**< Current size of the window. */

	bool fail_hash_alarm;
			/**< Alarm for consecutive hash fail. */
	bool esn_enabled;
			/**< Specifies whether ESN is enabled. */
};

/**
 * nss_ipsecmgr_event
 *	Event information for the IPsec manager.
 */
struct nss_ipsecmgr_event {
	enum nss_ipsecmgr_event_type type;	/**< Event type. */

	/**
	 * Event information statistics for the IPsec manager.
	 */
	union {
		struct nss_ipsecmgr_sa_stats stats;
				/**< Security association statistics. */
	} data;			/**< Event information. */
};

/**
 * nss_ipsecmgr_encap_flow
 *	Encapsulation flow information for the IPsec manager.
 */
struct nss_ipsecmgr_encap_flow {
	enum nss_ipsecmgr_flow_type type;	/**< Flow type. */

	/**
	 * Payload of encapsulation flow data for the IPsec manager.
	 */
	union {
		struct nss_ipsecmgr_encap_v4_tuple v4_tuple;
				/**< IPv4 tuple. */
		struct nss_ipsecmgr_encap_v4_subnet v4_subnet;
				/**< IPv4 subnet. */
		struct nss_ipsecmgr_encap_v6_tuple v6_tuple;
				/**< IPv6 tuple. */
		struct nss_ipsecmgr_encap_v6_subnet v6_subnet;
				/**< IPv6 subnet. */
	} data;		/**< Encapsulation flow information. */
};

#ifdef __KERNEL__ /* only kernel will use. */

/**
 * Callback function for receiving IPsec data.
 *
 * @datatypes
 * sk_buff
 *
 * @param[in] ctx  Pointer to the context of the data.
 * @param[in] skb  Pointer to the data socket buffer.
 */
typedef void (*nss_ipsecmgr_data_cb_t) (void *ctx, struct sk_buff *skb);

/**
 * Callback function for receiving IPsec events.
 *
 * @datatypes
 * nss_ipsecmgr_event
 *
 * @param[in] ctx  Pointer to the context of the event.
 * @param[in] ev   Pointer to the event.
 */
typedef void (*nss_ipsecmgr_event_cb_t) (void *ctx, struct nss_ipsecmgr_event *ev);

/**
 * nss_ipsecmgr_callback
 *	Callback information.
 */
struct nss_ipsecmgr_callback {
	void *ctx;				/**< Context of the caller. */
	nss_ipsecmgr_data_cb_t data_fn;		/**< Data callback function. */
	nss_ipsecmgr_event_cb_t event_fn;	/**< Event callback function. */
};

/**
 * nss_ipsecmgr_tunnel_add
 *	Adds a new IPsec tunnel.
 *
 * @datatypes
 * nss_ipsecmgr_callback
 *
 * @param[in] cb  Pointer to the message callback.
 *
 * @return
 * Linux NETDEVICE or NULL.
 */
struct net_device *nss_ipsecmgr_tunnel_add(struct nss_ipsecmgr_callback *cb);

/**
 * nss_ipsecmgr_tunnel_del
 *	Deletes an existing IPsec tunnel.
 *
 * @datatypes
 * net_device
 *
 * @param[in] tun  Pointer to the network device associated with the tunnel.
 *
 * @return
 * Success or failure.
 */
bool nss_ipsecmgr_tunnel_del(struct net_device *tun);

/**
 * nss_ipsecmgr_tunnel_update_callback
 *	Updates the binding of netdevice and callback.
 *
 * @datatypes
 * net_device
 *
 * @param[in] tun  Pointer to IPsec tunnel.
 * @param[in] cur  Pointer to Linux netdevice.
 *
 * @return
 * None.
 */
void nss_ipsecmgr_tunnel_update_callback(struct net_device *tun, struct net_device *cur);

/**
 * nss_ipsecmgr_encap_add
 *	Adds an encapsulation flow rule to the IPsec offload database.
 *
 * @datatypes
 * net_device \n
 * nss_ipsecmgr_encap_flow \n
 * nss_ipsecmgr_sa \n
 * nss_ipsecmgr_sa_data
 *
 * @param[in] tun   Pointer to the network device associated with the tunnel.
 * @param[in] flow  Pointer to the flow or subnet to add.
 * @param[in] sa    Pointer to the security association for the flow.
 * @param[in] data  Pointer to additional security association data.
 *
 * @return
 * Success or failure.
 */
bool nss_ipsecmgr_encap_add(struct net_device *tun, struct nss_ipsecmgr_encap_flow *flow, struct nss_ipsecmgr_sa *sa,
				struct nss_ipsecmgr_sa_data *data);

/**
 * nss_ipsecmgr_encap_del
 *	Deletes an encapsulation flow rule from the IPsec offload database.
 *
 * @datatypes
 * net_device \n
 * nss_ipsecmgr_encap_flow \n
 * nss_ipsecmgr_sa
 *
 * @param[in] tun   Pointer to the network device associated with the tunnel.
 * @param[in] flow  Pointer to the flow or subnet to delete.
 * @param[in] sa    Pointer to the security association for the flow.
 *
 * @return
 * Success or failure.
 */
bool nss_ipsecmgr_encap_del(struct net_device *tun, struct nss_ipsecmgr_encap_flow *flow, struct nss_ipsecmgr_sa *sa);

/**
 * nss_ipsecmgr_decap_add
 *	Adds a decapsulation security association to the offload database.
 *
 * @datatypes
 * net_device \n
 * nss_ipsecmgr_sa \n
 * nss_ipsenss_ipsecmgr_sa_datacmgr_sa
 *
 * @param[in] tun   Pointer to the network device associated with the tunnel.
 * @param[in] sa    Pointer to the security association for the decapsulation.
 * @param[in] data  Pointer to additional security association data.
 *
 * @return
 * Success or failure.
 */
bool nss_ipsecmgr_decap_add(struct net_device *tun, struct nss_ipsecmgr_sa *sa, struct nss_ipsecmgr_sa_data *data);

/**
 * nss_ipsecmgr_sa_flush
 *	Flushes the security association and all associated flows and subnets.
 *
 * @datatypes
 * net_device \n
 * nss_ipsecmgr_sa
 *
 * @param[in] tun  Pointer to the network device associated with the tunnel.
 * @param[in] sa   Pointer to the security association to flush.
 *
 * @return
 * Success or failure.
 */
bool nss_ipsecmgr_sa_flush(struct net_device *tun, struct nss_ipsecmgr_sa *sa);

#endif /* __KERNEL__ */

/**
 * @}
 */

#endif /* __NSS_IPSECMGR_H */

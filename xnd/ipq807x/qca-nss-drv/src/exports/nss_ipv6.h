/*
 **************************************************************************
 * Copyright (c) 2014-2021, The Linux Foundation. All rights reserved.
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
 * @file nss_ipv6.h
 *	NSS IPv6 interface definitions.
 */

#ifndef __NSS_IPV6_H
#define __NSS_IPV6_H

/**
 * @addtogroup nss_ipv6_subsystem
 * @{
 */

/**
 * Converts the format of an IPv6 address from Linux to NSS. @hideinitializer
 */
#define IN6_ADDR_TO_IPV6_ADDR(ipv6, in6) \
	{ \
		((uint32_t *)ipv6)[0] = in6.in6_u.u6_addr32[0]; \
		((uint32_t *)ipv6)[1] = in6.in6_u.u6_addr32[1]; \
		((uint32_t *)ipv6)[2] = in6.in6_u.u6_addr32[2]; \
		((uint32_t *)ipv6)[3] = in6.in6_u.u6_addr32[3]; \
	}

/**
 * Converts the format of an IPv6 address from NSS to Linux. @hideinitializer
 */
#define IPV6_ADDR_TO_IN6_ADDR(in6, ipv6) \
	{ \
		in6.in6_u.u6_addr32[0] = ((uint32_t *)ipv6)[0]; \
		in6.in6_u.u6_addr32[1] = ((uint32_t *)ipv6)[1]; \
		in6.in6_u.u6_addr32[2] = ((uint32_t *)ipv6)[2]; \
		in6.in6_u.u6_addr32[3] = ((uint32_t *)ipv6)[3]; \
	}

/**
 * Format of an IPv6 address (16 * 8 bits).
 */
#define IPV6_ADDR_OCTAL_FMT "%04x:%04x:%04x:%04x:%04x:%04x:%04x:%04x"

/**
 * Prints an IPv6 address (16 * 8 bits).
 */
#define IPV6_ADDR_TO_OCTAL(ipv6) ((uint16_t *)ipv6)[0], ((uint16_t *)ipv6)[1], ((uint16_t *)ipv6)[2], ((uint16_t *)ipv6)[3], ((uint16_t *)ipv6)[4], ((uint16_t *)ipv6)[5], ((uint16_t *)ipv6)[6], ((uint16_t *)ipv6)[7]

/*
 * IPv6 connection flags (to be used with nss_ipv6_create::flags.
 */
#define NSS_IPV6_CREATE_FLAG_NO_SEQ_CHECK 0x1
		/**< Indicates that sequence numbers are not to be checked. */
#define NSS_IPV6_CREATE_FLAG_BRIDGE_FLOW 0x02
		/**< Indicates that this is a pure bridge flow (no routing is involved). */
#define NSS_IPV6_CREATE_FLAG_ROUTED 0x04	/**< Rule is for a routed connection. */
#define NSS_IPV6_CREATE_FLAG_DSCP_MARKING 0x08	/**< Rule for DSCP marking. */
#define NSS_IPV6_CREATE_FLAG_VLAN_MARKING 0x10	/**< Rule for VLAN marking. */
#define NSS_IPV6_CREATE_FLAG_QOS_VALID 0x20	/**< Rule for Valid QoS. */

/**
 * nss_ipv6_create
 *	Information for an IPv6 flow or connection create rule.
 *
 * All fields must be passed in host-endian order.
 */
struct nss_ipv6_create {
	int32_t src_interface_num;
			/**< Source interface number (virtual or physical). */
	int32_t dest_interface_num;
			/**< Destination interface number (virtual or physical). */
	int32_t protocol;	/**< L4 protocol (e.g., TCP or UDP). */
	uint32_t flags;		/**< Flags (if any) associated with this rule. */
	uint32_t from_mtu;	/**< MTU of the incoming interface. */
	uint32_t to_mtu;	/**< MTU of the outgoing interface. */
	uint32_t src_ip[4];	/**< Source IP address. */
	int32_t src_port;	/**< Source L4 port (e.g., TCP or UDP port). */
	uint32_t dest_ip[4];	/**< Destination IP address. */
	int32_t dest_port;	/**< Destination L4 port (e.g., TCP or UDP port). */
	uint8_t src_mac[ETH_ALEN];	/**< Source MAC address. */
	uint8_t dest_mac[ETH_ALEN];	/**< Destination MAC address. */
	uint8_t flow_window_scale;	/**< Window scaling factor (TCP). */
	uint32_t flow_max_window;	/**< Maximum window size (TCP). */
	uint32_t flow_end;		/**< TCP window end. */
	uint32_t flow_max_end;		/**< TCP window maximum end. */
	uint32_t flow_pppoe_if_exist;
			/**< Flow direction: PPPoE interface existence flag. */
	int32_t flow_pppoe_if_num;
			/**< Flow direction: PPPoE interface number. */
	uint16_t ingress_vlan_tag;
			/**< Ingress VLAN tag expected for this flow. */
	uint8_t return_window_scale;
			/**< Window scaling factor (TCP) for the return direction. */
	uint32_t return_max_window;
			/**< Maximum window size (TCP) for the return direction. */
	uint32_t return_end;
			/**< End for the return direction. */
	uint32_t return_max_end;
			/**< Maximum end for the return direction. */
	uint32_t return_pppoe_if_exist;
			/**< Return direction: PPPoE interface exist flag. */
	int32_t return_pppoe_if_num;
			/**< Return direction: PPPoE interface number. */
	uint16_t egress_vlan_tag;	/**< Egress VLAN tag expected for this flow. */
	uint32_t qos_tag;		/**< Deprecated; will be removed soon. */
	uint32_t flow_qos_tag;		/**< QoS tag value for flow direction. */
	uint32_t return_qos_tag;	/**< QoS tag value for the return direction. */
	uint8_t dscp_itag;		/**< DSCP marking tag. */
	uint8_t dscp_imask;		/**< DSCP marking input mask. */
	uint8_t dscp_omask;		/**< DSCP marking output mask. */
	uint8_t dscp_oval;		/**< DSCP marking output value. */
	uint16_t vlan_itag;		/**< VLAN marking tag. */
	uint16_t vlan_imask;		/**< VLAN marking input mask. */
	uint16_t vlan_omask;		/**< VLAN marking output mask. */
	uint16_t vlan_oval;		/**< VLAN marking output value. */
	uint32_t in_vlan_tag[MAX_VLAN_DEPTH];
					/**< Ingress VLAN tag expected for this flow. */
	uint32_t out_vlan_tag[MAX_VLAN_DEPTH];
					/**< Egress VLAN tag expected for this flow. */
	uint8_t flow_dscp;		/**< IP DSCP value for flow direction. */
	uint8_t return_dscp;		/**< IP DSCP value for the return direction. */
};

/**
 * nss_ipv6_destroy
 *	Information for an IPv6 flow or connection destroy rule.
 */
struct nss_ipv6_destroy {
	int32_t protocol;	/**< L4 protocol (e.g., TCP or UDP). */
	uint32_t src_ip[4];	/**< Source IP address. */
	int32_t src_port;	/**< Source L4 port (e.g., TCP or UDP port). */
	uint32_t dest_ip[4];	/**< Destination IP address. */
	int32_t dest_port;	/**< Destination L4 port (e.g., TCP or UDP port). */
};

/**
 * nss_ipv6_stats_types
 *	IPv6 node statistics.
 */
enum nss_ipv6_stats_types {
	NSS_IPV6_STATS_ACCELERATED_RX_PKTS,
					/**< Accelerated IPv6 Rx packets. */
	NSS_IPV6_STATS_ACCELERATED_RX_BYTES,
					/**< Accelerated IPv6 Rx bytes. */
	NSS_IPV6_STATS_ACCELERATED_TX_PKTS,
					/**< Accelerated IPv6 Tx packets. */
	NSS_IPV6_STATS_ACCELERATED_TX_BYTES,
					/**< Accelerated IPv6 Tx bytes. */
	NSS_IPV6_STATS_CONNECTION_CREATE_REQUESTS,
					/**< Number of IPv6 connection create requests. */
	NSS_IPV6_STATS_CONNECTION_CREATE_COLLISIONS,
					/**< Number of IPv6 connection create requests that collided with existing entries. */
	NSS_IPV6_STATS_CONNECTION_CREATE_INVALID_INTERFACE,
					/**< Number of IPv6 connection create requests that had invalid interface. */
	NSS_IPV6_STATS_CONNECTION_DESTROY_REQUESTS,
					/**< Number of IPv6 connection destroy requests. */
	NSS_IPV6_STATS_CONNECTION_DESTROY_MISSES,
					/**< Number of IPv6 connection destroy requests that missed the cache. */
	NSS_IPV6_STATS_CONNECTION_HASH_HITS,
					/**< Number of IPv6 connection hash hits. */
	NSS_IPV6_STATS_CONNECTION_HASH_REORDERS,
					/**< Number of IPv6 connection hash reorders. */
	NSS_IPV6_STATS_CONNECTION_FLUSHES,
					/**< Number of IPv6 connection flushes. */
	NSS_IPV6_STATS_CONNECTION_EVICTIONS,
					/**< Number of IPv6 connection evictions. */
	NSS_IPV6_STATS_FRAGMENTATIONS,
					/**< Number of successful IPv6 fragmentations performed. */
	NSS_IPV6_STATS_FRAG_FAILS,
					/**< Number of IPv6 fragmentation fails. */
	NSS_IPV6_STATS_DROPPED_BY_RULE,
					/**< Number of IPv6 packets dropped by a drop rule. */
	NSS_IPV6_STATS_MC_CONNECTION_CREATE_REQUESTS,
					/**< Number of successful IPv6 multicast create requests. */
	NSS_IPV6_STATS_MC_CONNECTION_UPDATE_REQUESTS,
					/**< Number of successful IPv6 multicast update requests. */
	NSS_IPV6_STATS_MC_CONNECTION_CREATE_INVALID_INTERFACE,
					/**< Number of IPv6 multicast connection create requests that had invalid interface. */
	NSS_IPV6_STATS_MC_CONNECTION_DESTROY_REQUESTS,
					/**< Number of IPv6 multicast connection destroy requests. */
	NSS_IPV6_STATS_MC_CONNECTION_DESTROY_MISSES,
					/**< Number of IPv6 multicast connection destroy requests that missed the cache. */
	NSS_IPV6_STATS_MC_CONNECTION_FLUSHES,
					/**< Number of IPv6 multicast connection flushes. */
	NSS_IPV6_STATS_CONNECTION_CREATE_INVALID_MIRROR_IFNUM,
		/**< Number of IPv6 mirror connection requests with an invalid interface number. */
	NSS_IPV6_STATS_CONNECTION_CREATE_INVALID_MIRROR_IFTYPE,
		/**< Number of IPv6 mirror connection requests with an invalid interface type. */

	NSS_IPV6_STATS_MIRROR_FAILURES,
		/**< Number of IPv6 mirror failures. */

	NSS_IPV6_STATS_MAX,
					/**< Maximum message type. */
};

/**
 * nss_ipv6_message_types
 *	IPv6 bridge and routing rule message types.
 *
 * NSS_IPV6_RX_DEPRECATED0 is a deprecated type. It is kept for backward compatibility.
 */
enum nss_ipv6_message_types {
	NSS_IPV6_TX_CREATE_RULE_MSG,
	NSS_IPV6_TX_DESTROY_RULE_MSG,
	NSS_IPV6_RX_DEPRECATED0,
	NSS_IPV6_RX_CONN_STATS_SYNC_MSG,
	NSS_IPV6_RX_NODE_STATS_SYNC_MSG,
	NSS_IPV6_TX_CONN_CFG_RULE_MSG,
	NSS_IPV6_TX_CREATE_MC_RULE_MSG,
	NSS_IPV6_TX_CONN_STATS_SYNC_MANY_MSG,
	NSS_IPV6_TX_ACCEL_MODE_CFG_MSG,
	NSS_IPV6_TX_CONN_CFG_INQUIRY_MSG,
	NSS_IPV6_TX_CONN_TABLE_SIZE_MSG,
	NSS_IPV6_TX_DSCP2PRI_CFG_MSG,
	NSS_IPV6_TX_RPS_HASH_BITMAP_CFG_MSG,
	NSS_IPV6_MAX_MSG_TYPES,
};

/**
 * nss_ipv6_dscp_map_actions
 *	Action types mapped to DSCP values.
 */
enum nss_ipv6_dscp_map_actions {
	NSS_IPV6_DSCP_MAP_ACTION_ACCEL,
	NSS_IPV6_DSCP_MAP_ACTION_DONT_ACCEL,
	NSS_IPV6_DSCP_MAP_ACTION_MAX,
};

/*
 * NSS IPv6 rule creation flags.
 */
#define NSS_IPV6_RULE_CREATE_FLAG_NO_SEQ_CHECK 0x01
		/**< Do not perform TCP sequence number checks. */
#define NSS_IPV6_RULE_CREATE_FLAG_BRIDGE_FLOW 0x02
		/**< This is a pure bridge forwarding flow. */
#define NSS_IPV6_RULE_CREATE_FLAG_ROUTED 0x04
		/**< Rule is for a routed connection. */
#define NSS_IPV6_RULE_CREATE_FLAG_DSCP_MARKING 0x08
		/**< Rule has for a DSCP marking configured. */
#define NSS_IPV6_RULE_CREATE_FLAG_VLAN_MARKING 0x10
		/**< Rule has for a VLAN marking configured. */
#define NSS_IPV6_RULE_CREATE_FLAG_ICMP_NO_CME_FLUSH 0x20
		/**< Rule for not flushing connection match entry on ICMP packet. */
#define NSS_IPV6_RULE_UPDATE_FLAG_CHANGE_MTU 0x40
		/**< Rule updation for MTU change. */

/** The L2 payload is not IPv6 but consists of an encapsulating protocol that carries an IPv6 payload within it.
 */
#define NSS_IPV6_RULE_CREATE_FLAG_L2_ENCAP 0x80

#define NSS_IPV6_RULE_CREATE_FLAG_DROP 0x100
		/**< Drop packets. */
#define NSS_IPV6_RULE_CREATE_FLAG_EXCEPTION 0x200
		/**< Rule to except packets. */
#define NSS_IPV6_RULE_CREATE_FLAG_SRC_INTERFACE_CHECK 0x400
		/**< Check the source interface for the rule. */
#define NSS_IPV6_RULE_CREATE_FLAG_NO_SRC_IDENT 0x800
		/**< Flag to indicate NSS to ignore src_ident and use value 0 for it during rule addition. */
#define NSS_IPV6_RULE_CREATE_FLAG_NO_MAC 0x1000
		/**< Flag to bypass writing MAC addresses. */
#define NSS_IPV6_RULE_CREATE_FLAG_EMESH_SP 0x2000
		/**< Mark rule as E-MESH Service Prioritization valid. */

/*
 * IPv6 rule creation validity flags.
 */
#define NSS_IPV6_RULE_CREATE_CONN_VALID 0x01	/**< Connection is valid. */
#define NSS_IPV6_RULE_CREATE_TCP_VALID 0x02	/**< TCP protocol fields are valid. */
#define NSS_IPV6_RULE_CREATE_PPPOE_VALID 0x04	/**< PPPoE fields are valid. */
#define NSS_IPV6_RULE_CREATE_QOS_VALID 0x08	/**< QoS fields are valid. */
#define NSS_IPV6_RULE_CREATE_VLAN_VALID 0x10	/**< VLAN fields are valid. */
#define NSS_IPV6_RULE_CREATE_DSCP_MARKING_VALID 0x20
						/**< DSCP marking fields are valid. */
#define NSS_IPV6_RULE_CREATE_VLAN_MARKING_VALID 0x40
						/**< VLAN marking fields are valid. */
#define NSS_IPV6_RULE_CREATE_SRC_MAC_VALID 0x80
						/**< Source MAC address fields are valid. */
#define NSS_IPV6_RULE_CREATE_NEXTHOP_VALID 0x100
						/**< Next hop interface number fields are valid. */
#define NSS_IPV6_RULE_CREATE_RPS_VALID 0x200	/**< RPS for core selection is valid. */
#define NSS_IPV6_RULE_CREATE_DEST_MAC_VALID 0x400
		/**< Destination MAC address fields are valid. */
#define NSS_IPV6_RULE_CREATE_IGS_VALID 0x800	/**< Ingress shaping fields are valid. */
#define NSS_IPV6_RULE_CREATE_IDENTIFIER_VALID 0x1000	/**< Identifier is valid. */
#define NSS_IPV6_RULE_CREATE_MIRROR_VALID 0x2000	/**< Mirror fields are valid. */

/*
 * Multicast command rule flags
 */
#define NSS_IPV6_MC_RULE_CREATE_FLAG_MC_UPDATE 0x01	/**< Multicast rule update. */
#define NSS_IPV6_MC_RULE_CREATE_FLAG_MC_EMESH_SP  0x02
		/**< Mark multicast rule as E-MESH Service Prioritization valid. */

/*
 * Multicast command validity flags
 */
#define NSS_IPV6_MC_RULE_CREATE_FLAG_QOS_VALID 0x01
		/**< QoS fields are valid. */
#define NSS_IPV6_MC_RULE_CREATE_FLAG_DSCP_MARKING_VALID 0x02
		/**< DSCP fields are valid. */
#define NSS_IPV6_MC_RULE_CREATE_FLAG_INGRESS_VLAN_VALID 0x04
		/**< Ingress VLAN fields are valid. */
#define NSS_IPV6_MC_RULE_CREATE_FLAG_INGRESS_PPPOE 0x08
		/**< Ingress PPPoE fields are valid. */
#define NSS_IPV6_MC_RULE_CREATE_FLAG_IGS_VALID 0x10
		/**< Ingress shaping fields are valid. */

/*
 * Per-interface rule flags for a multicast connection (to be used with the rule_flags
 * field of nss_ipv6_mc_if_rule structure).
 */
#define NSS_IPV6_MC_RULE_CREATE_IF_FLAG_BRIDGE_FLOW 0x01
		/**< Bridge flow. */
#define NSS_IPV6_MC_RULE_CREATE_IF_FLAG_ROUTED_FLOW 0x02
		/**< Routed flow. */
#define NSS_IPV6_MC_RULE_CREATE_IF_FLAG_JOIN 0x04
		/**< Interface has joined the flow. */
#define NSS_IPV6_MC_RULE_CREATE_IF_FLAG_LEAVE 0x08
		/**< Interface has left the flow. */

/*
 * Per-interface valid flags for a multicast connection (to be used with the valid_flags
 * field of nss_ipv6_mc_if_rule structure).
 */
#define NSS_IPV6_MC_RULE_CREATE_IF_FLAG_VLAN_VALID 0x01
		/**< VLAN fields are valid. */
#define NSS_IPV6_MC_RULE_CREATE_IF_FLAG_PPPOE_VALID 0x02
		/**< PPPoE fields are valid. */

/*
 * Source MAC address valid flags (to be used with mac_valid_flags field of nss_ipv6_src_mac_rule structure)
 */
#define NSS_IPV6_SRC_MAC_FLOW_VALID 0x01
		/**< MAC address for the flow interface is valid. */
#define NSS_IPV6_SRC_MAC_RETURN_VALID 0x02
		/**< MAC address for the return interface is valid. */

/*
 * Identifier valid flags (to be used with identifier_valid_flags field of nss_ipv6_identifier_rule structure)
 */
#define NSS_IPV6_FLOW_IDENTIFIER_VALID 0x01
		/**< Identifier for flow direction is valid. */
#define NSS_IPV6_RETURN_IDENTIFIER_VALID 0x02
		/**< Identifier for return direction is valid. */

/*
 * Mirror valid flags (to be used with the valid field of nss_ipv6_mirror_rule structure)
 */
#define NSS_IPV6_MIRROR_FLOW_VALID 0x01
		/**< Mirror interface number for the flow direction is valid. */
#define NSS_IPV6_MIRROR_RETURN_VALID 0x02
		/**< Mirror interface number for the return direction is valid. */

/**
 * nss_ipv6_exception_events
 *	Exception events from an IPv6 bridge or route handler.
 */
enum nss_ipv6_exception_events {
	NSS_IPV6_EXCEPTION_EVENT_ICMP_HEADER_INCOMPLETE,
	NSS_IPV6_EXCEPTION_EVENT_ICMP_UNHANDLED_TYPE,
	NSS_IPV6_EXCEPTION_EVENT_ICMP_IPV6_HEADER_INCOMPLETE,
	NSS_IPV6_EXCEPTION_EVENT_ICMP_IPV6_UDP_HEADER_INCOMPLETE,
	NSS_IPV6_EXCEPTION_EVENT_ICMP_IPV6_TCP_HEADER_INCOMPLETE,
	NSS_IPV6_EXCEPTION_EVENT_ICMP_IPV6_UNKNOWN_PROTOCOL,
	NSS_IPV6_EXCEPTION_EVENT_ICMP_NO_ICME,
	NSS_IPV6_EXCEPTION_EVENT_ICMP_FLUSH_TO_HOST,
	NSS_IPV6_EXCEPTION_EVENT_TCP_HEADER_INCOMPLETE,
	NSS_IPV6_EXCEPTION_EVENT_TCP_NO_ICME,
	NSS_IPV6_EXCEPTION_EVENT_TCP_SMALL_HOP_LIMIT,
	NSS_IPV6_EXCEPTION_EVENT_TCP_NEEDS_FRAGMENTATION,
	NSS_IPV6_EXCEPTION_EVENT_TCP_FLAGS,
	NSS_IPV6_EXCEPTION_EVENT_TCP_SEQ_EXCEEDS_RIGHT_EDGE,
	NSS_IPV6_EXCEPTION_EVENT_TCP_SMALL_DATA_OFFS,
	NSS_IPV6_EXCEPTION_EVENT_TCP_BAD_SACK,
	NSS_IPV6_EXCEPTION_EVENT_TCP_BIG_DATA_OFFS,
	NSS_IPV6_EXCEPTION_EVENT_TCP_SEQ_BEFORE_LEFT_EDGE,
	NSS_IPV6_EXCEPTION_EVENT_TCP_ACK_EXCEEDS_RIGHT_EDGE,
	NSS_IPV6_EXCEPTION_EVENT_TCP_ACK_BEFORE_LEFT_EDGE,
	NSS_IPV6_EXCEPTION_EVENT_UDP_HEADER_INCOMPLETE,
	NSS_IPV6_EXCEPTION_EVENT_UDP_NO_ICME,
	NSS_IPV6_EXCEPTION_EVENT_UDP_SMALL_HOP_LIMIT,
	NSS_IPV6_EXCEPTION_EVENT_UDP_NEEDS_FRAGMENTATION,
	NSS_IPV6_EXCEPTION_EVENT_WRONG_TARGET_MAC,
	NSS_IPV6_EXCEPTION_EVENT_HEADER_INCOMPLETE,
	NSS_IPV6_EXCEPTION_EVENT_UNKNOWN_PROTOCOL,
	NSS_IPV6_EXCEPTION_EVENT_IVID_MISMATCH,
	NSS_IPV6_EXCEPTION_EVENT_IVID_MISSING,
	NSS_IPV6_EXCEPTION_EVENT_DSCP_MARKING_MISMATCH,
	NSS_IPV6_EXCEPTION_EVENT_VLAN_MARKING_MISMATCH,
	NSS_IPV6_EXCEPTION_EVENT_INTERFACE_MISMATCH,
	NSS_IPV6_EXCEPTION_EVENT_GRE_NO_ICME,
	NSS_IPV6_EXCEPTION_EVENT_GRE_NEEDS_FRAGMENTATION,
	NSS_IPV6_EXCEPTION_EVENT_GRE_SMALL_HOP_LIMIT,
	NSS_IPV6_EXCEPTION_EVENT_DESTROY,
	NSS_IPV6_EXCEPTION_EVENT_ICMP_IPV6_UDPLITE_HEADER_INCOMPLETE,
	NSS_IPV6_EXCEPTION_EVENT_UDPLITE_HEADER_INCOMPLETE,
	NSS_IPV6_EXCEPTION_EVENT_UDPLITE_NO_ICME,
	NSS_IPV6_EXCEPTION_EVENT_UDPLITE_SMALL_HOP_LIMIT,
	NSS_IPV6_EXCEPTION_EVENT_UDPLITE_NEEDS_FRAGMENTATION,
	NSS_IPV6_EXCEPTION_EVENT_MC_UDP_NO_ICME,
	NSS_IPV6_EXCEPTION_EVENT_MC_MEM_ALLOC_FAILURE,
	NSS_IPV6_EXCEPTION_EVENT_MC_UPDATE_FAILURE,
	NSS_IPV6_EXCEPTION_EVENT_MC_PBUF_ALLOC_FAILURE,
	NSS_IPV6_EXCEPTION_EVENT_ESP_HEADER_INCOMPLETE,
	NSS_IPV6_EXCEPTION_EVENT_ESP_NO_ICME,
	NSS_IPV6_EXCEPTION_EVENT_ESP_IP_FRAGMENT,
	NSS_IPV6_EXCEPTION_EVENT_ESP_SMALL_HOP_LIMIT,
	NSS_IPV6_EXCEPTION_EVENT_ESP_NEEDS_FRAGMENTATION,
	NSS_IPV6_EXCEPTION_EVENT_TUNIPIP6_NO_ICME,
	NSS_IPV6_EXCEPTION_EVENT_TUNIPIP6_SMALL_HOP_LIMIT,
	NSS_IPV6_EXCEPTION_EVENT_TUNIPIP6_NEEDS_FRAGMENTATION,
	NSS_IPV6_EXCEPTION_EVENT_PPPOE_BRIDGE_NO_ICME,
	NSS_IPV6_EXCEPTION_EVENT_DONT_FRAG_SET,
	NSS_IPV6_EXCEPTION_EVENT_REASSEMBLY_NOT_SUPPORTED,
	NSS_IPV6_EXCEPTION_EVENT_PPPOE_NO_SESSION,
	NSS_IPV6_EXCEPTION_EVENT_ICMP_IPV6_GRE_HEADER_INCOMPLETE,
	NSS_IPV6_EXCEPTION_EVENT_ICMP_IPV6_ESP_HEADER_INCOMPLETE,
	NSS_IPV6_EXCEPTION_EVENT_EMESH_PRIO_MISMATCH,
	NSS_IPV6_EXCEPTION_EVENT_MAX
};

/**
 * nss_ipv6_5tuple
 *	Common 5-tuple information.
 */
struct nss_ipv6_5tuple {
	uint32_t flow_ip[4];		/**< Flow IP address. */
	uint32_t flow_ident;		/**< Flow identifier (e.g., TCP or UDP port). */
	uint32_t return_ip[4];		/**< Return IP address. */
	uint32_t return_ident;		/**< Return identier (e.g., TCP or UDP port). */
	uint8_t  protocol;		/**< Protocol number. */
	uint8_t  reserved[3];		/**< Padded for alignment. */
};

/**
 * nss_ipv6_connection_rule
 *	Information for creating a connection.
 */
struct nss_ipv6_connection_rule {
	uint16_t flow_mac[3];		/**< Flow MAC address. */
	uint16_t return_mac[3];		/**< Return MAC address. */
	int32_t flow_interface_num;	/**< Flow interface number. */
	int32_t return_interface_num;	/**< Return interface number. */
	uint32_t flow_mtu;		/**< MTU for the flow interface. */
	uint32_t return_mtu;		/**< MTU for the return interface. */
};

/**
 * nss_ipv6_pppoe_rule
 *	Information for PPPoE connection rules.
 */
struct nss_ipv6_pppoe_rule {
	uint32_t flow_if_exist;
			/**< PPPoE interface existence flag for the flow direction. */
	int32_t flow_if_num;
			/**< PPPoE interface number for the flow direction. */
	uint32_t return_if_exist;
			/**< PPPoE interface existence flag for the return direction. */
	int32_t return_if_num;
			/**< PPPoE interface number for the return direction. */
};

/**
 * nss_ipv6_dscp_rule
 *	Information for DSCP connection rules.
 */
struct nss_ipv6_dscp_rule {
	uint8_t flow_dscp;	/**< Egress DSCP value for the flow direction. */
	uint8_t return_dscp;	/**< Egress DSCP value for the return direction. */
	uint8_t reserved[2];	/**< Padded for alignment. */
};

/**
 * nss_ipv6_vlan_rule
 *	Information for VLAN connection rules.
 */
struct nss_ipv6_vlan_rule {
	uint32_t ingress_vlan_tag;	/**< VLAN tag for the ingress packets. */
	uint32_t egress_vlan_tag;	/**< VLAN tag for egress packets. */
};

/**
 * nss_ipv6_nexthop
 *	Information for the next hop interface numbers.
 *
 * A next hop is the next interface that will receive the packet as opposed to
 * the final interface when the packet leaves the device.
 */
struct nss_ipv6_nexthop {
	/**
	 * Next hop interface number of the flow direction (from which the connection
	 * originated).
	 */
	int32_t flow_nexthop;
	/**
	 * Next hop interface number of the return direction (to which the connection
	 * is destined).
	 */
	int32_t return_nexthop;
};

/**
 * nss_ipv6_protocol_tcp_rule
 *	Information for TCP connection rules.
 */
struct nss_ipv6_protocol_tcp_rule {
	uint32_t flow_max_window;
			/**< Largest seen window for the flow direction. */
	uint32_t flow_end;
			/**< Largest seen sequence + segment length for the flow direction. */
	uint32_t flow_max_end;
			/**< Largest seen ack + max(1, win) for the flow direction. */
	uint32_t return_max_window;
			/**< Largest seen window for the return direction. */
	uint32_t return_end;
			/**< Largest seen sequence + segment length for the return direction. */
	uint32_t return_max_end;
			/**< Largest seen ack + max(1, win) for the return direction. */
	uint8_t flow_window_scale;
			/**< Window scaling factor for the flow direction. */
	uint8_t return_window_scale;
			/**< Window scaling factor for the return direction. */
	uint16_t reserved;
			/**< Alignment padding. */
};

/**
 * nss_ipv6_igs_rule
 *	Information for ingress shaping connection rules.
 */
struct nss_ipv6_igs_rule {
	uint16_t igs_flow_qos_tag;
			/**< Ingress shaping QoS tag associated with this rule for the flow direction. */
	uint16_t igs_return_qos_tag;
			/**< Ingress shaping QoS tag associated with this rule for the return direction. */
};

/**
 * nss_ipv6_qos_rule
 *	Information for QoS connection rules.
 */
struct nss_ipv6_qos_rule {
	uint32_t flow_qos_tag;
			/**< QoS tag associated with this rule for the flow direction. */
	uint32_t return_qos_tag;
			/**< QoS tag associated with this rule for the return direction. */
};

/**
 * nss_ipv6_src_mac_rule
 *	Information for source MAC address rules.
 */
struct nss_ipv6_src_mac_rule {
	uint32_t mac_valid_flags;	/**< MAC address validity flags. */
	uint16_t flow_src_mac[3];	/**< Source MAC address for the flow direction. */
	uint16_t return_src_mac[3];	/**< Source MAC address for the return direction. */
};

/**
 * nss_ipv6_rps_rule
 *	RPS rule structure.
 */
struct nss_ipv6_rps_rule {
	uint8_t flow_rps;
		/**< RPS for core selection for flow direction. */
	uint8_t return_rps;
		/**< RPS for core selection for return direction. */
	uint8_t reserved[2];
		/**< Padded for alignment. */
};

/**
 * nss_ipv6_identifier_rule
 *	Identifier rule structure.
 */
struct nss_ipv6_identifier_rule {
	uint32_t identifier_valid_flags;
		/**< Identifier validity flags. */
	uint32_t flow_identifier;
		/**< Identifier for flow direction. */
	uint32_t return_identifier;
		/**< Identifier for return direction. */
};

/**
 * nss_ipv6_mirror_rule
 *	Mirror rule structure.
 */
struct nss_ipv6_mirror_rule {
	uint32_t valid;			/**< Mirror validity flags. */
	nss_if_num_t flow_ifnum;	/**< Flow mirror interface number. */
	nss_if_num_t return_ifnum;	/**< Return mirror interface number. */
};

/**
 * nss_ipv6_error_response_types
 *	Error types for IPv6 messages.
 */
enum nss_ipv6_error_response_types {
	NSS_IPV6_UNKNOWN_MSG_TYPE = 1,			/**< Unknown error. */
	NSS_IPV6_CR_INVALID_PNODE_ERROR,		/**< Invalid interface number. */
	NSS_IPV6_CR_MISSING_CONNECTION_RULE_ERROR,	/**< Missing connection rule. */
	NSS_IPV6_CR_BUFFER_ALLOC_FAIL_ERROR,		/**< Buffer allocation failed. */
	NSS_IPV6_DR_NO_CONNECTION_ENTRY_ERROR,
		/**< No connection was found to delete. */
	NSS_IPV6_CR_CONN_CFG_ALREADY_CONFIGURED_ERROR,
		/**< Connection configuration was already done once. */

	NSS_IPV6_CR_CONN_CFG_NOT_MULTIPLE_OF_QUANTA_ERROR,
	/**< Input for connection configuration is not a multiple of quanta. */

	/**
	 * Input for connection configuration exceeds the maximum number of supported
	 * connections.
	 */
	NSS_IPV6_CR_CONN_CFG_EXCEEDS_LIMIT_ERROR,

	/**
	 * Memory allocation for connection configuration failed at the NSS firmware.
	 */
	NSS_IPV6_CR_CONN_CFG_MEM_ALLOC_FAIL_ERROR,

	NSS_IPV6_CR_MULTICAST_INVALID_PROTOCOL,
		/**< Invalid L4 protocol for creating a multicast rule. */
	NSS_IPV6_CR_MULTICAST_UPDATE_INVALID_FLAGS,
		/**< Invalid multicast flags for updating multicast. */
	NSS_IPV6_CR_MULTICAST_UPDATE_INVALID_IF,
		/**< Invalid interface for updating multicast. */
	NSS_IPV6_CR_ACCEL_MODE_CONFIG_INVALID,
		/**< Invalid config value for acceleration mode. */
	NSS_IPV6_CR_INVALID_MSG_ERROR,
		/**< Invalid message size error. */
	NSS_IPV6_CR_DSCP2PRI_PRI_INVALID,
		/**< Priority value out of range error. */
	NSS_IPV6_CR_DSCP2PRI_CONFIG_INVALID,
		/**< Invalid DSCP value. */
	NSS_IPV6_CR_INVALID_RPS,
		/**< Invalid RPS Value. */
	NSS_IPV6_HASH_BITMAP_INVALID,
		/**< Invalid hash bitmap. */
	NSS_IPV6_DR_HW_DECEL_FAIL_ERROR,
		/**< Hardware deceleration fail error. */
	NSS_IPV6_CR_RETURN_EXIST_ERROR,
		/**< Rule creation failed because a 5-tuple return already exists. */
	NSS_IPV6_CR_INVALID_IDENTIFIER,
		/**< Invalid identifier value. */
	NSS_IPV6_CR_EMESH_SP_CONFIG_INVALID,
		/**< Rule creation failed because Qos tag was not set for a Emesh SP rule. */
	NSS_IPV6_LAST
		/**< Maximum number of error responses. */
};

/**
 * nss_ipv6_rule_create_msg
 *	IPv6 rule for creating sub-messages.
 */
struct nss_ipv6_rule_create_msg {
	/*
	 * Request
	 */
	uint16_t valid_flags;
			/**< Bit flags associated with the validity of parameters. */
	uint16_t rule_flags;
			/**< Bit flags associated with the rule. */
	struct nss_ipv6_5tuple tuple;
			/**< Holds values of the 5 tuple. */
	struct nss_ipv6_connection_rule conn_rule;
			/**< Basic connection-specific data. */
	struct nss_ipv6_protocol_tcp_rule tcp_rule;
			/**< Protocol-related accleration parameters. */
	struct nss_ipv6_pppoe_rule pppoe_rule;
			/**< PPPoE-related accleration parameters. */
	struct nss_ipv6_qos_rule qos_rule;
			/**< QoS-related accleration parameters. */
	struct nss_ipv6_dscp_rule dscp_rule;
			/**< DSCP-related accleration parameters. */
	struct nss_ipv6_vlan_rule vlan_primary_rule;
			/**< VLAN-related accleration parameters. */
	struct nss_ipv6_vlan_rule vlan_secondary_rule;
			/**< VLAN-related accleration parameters. */
	struct nss_ipv6_src_mac_rule src_mac_rule;
			/**< Source MAC address-related acceleration parameters. */
	struct nss_ipv6_nexthop nexthop_rule;
			/**< Parameters related to the next hop. */
	struct nss_ipv6_rps_rule rps_rule;
			/**< RPS parameter. */
	struct nss_ipv6_igs_rule igs_rule;
			/**< Ingress shaping related accleration parameters. */
	struct nss_ipv6_identifier_rule identifier;
			/**< Rule for adding identifier. */
	struct nss_ipv6_mirror_rule mirror_rule;
			/**< Mirror rule parameter. */
};

/**
 * nss_ipv6_inquiry_msg
 *	IPv6 connection inquiry sub-messages.
 */
struct nss_ipv6_inquiry_msg {
	/**
	 * Request by 5-tuple and response in other items.
	 */
	struct nss_ipv6_rule_create_msg rr;
};

/**
 * nss_ipv6_mc_if_rule
 *	IPv6 multicast rule for creating a per-interface payload.
 */
struct nss_ipv6_mc_if_rule {
	uint16_t rule_flags;
			/**< Bit flags associated with the rule for this interface. */

	/**
	 * Bit flags associated with the validity of parameters for this interface.
	 */
	uint16_t valid_flags;

	uint32_t egress_vlan_tag[MAX_VLAN_DEPTH];
					/**< VLAN tag stack for the egress packets. */
	int32_t pppoe_if_num;		/**< PPPoE interface number. */
	uint32_t if_num;		/**< Interface number. */
	uint32_t if_mtu;		/**< MTU of the interface. */
	uint16_t if_mac[3];		/**< Interface MAC address. */
	uint8_t reserved[2];		/**< Reserved 2 bytes for alignment. */
};

/**
 * nss_ipv6_mc_rule_create_msg
 *	IPv6 multicast rule for creating sub-messages.
 */
struct nss_ipv6_mc_rule_create_msg {
	struct nss_ipv6_5tuple tuple;	/**< Holds values of the 5 tuple. */

	uint32_t rule_flags;		/**< Multicast command rule flags. */
	uint32_t valid_flags;		/**< Multicast command validity flags. */
	uint32_t src_interface_num;
			/**< Source interface number (virtual or physical). */
	uint32_t ingress_vlan_tag[MAX_VLAN_DEPTH];
			/**< VLAN tag stack for the ingress packets. */
	uint32_t qos_tag;		/**< QoS tag for the flow. */
	uint16_t dest_mac[3];		/**< Destination multicast MAC address. */
	uint16_t if_count;		/**< Number of destination interfaces. */
	uint8_t egress_dscp;		/**< Egress DSCP value for the flow. */
	uint8_t reserved[1];		/**< Reserved 1 byte for alignment. */
	uint16_t igs_qos_tag;		/**< Ingress shaping QoS tag for the flow. */

	struct nss_ipv6_mc_if_rule if_rule[NSS_MC_IF_MAX];
			/**< Per-interface information. */
};

/**
 * nss_ipv6_rule_destroy_msg
 *	IPv6 rule for deleting sub-messages.
 */
struct nss_ipv6_rule_destroy_msg {
	struct nss_ipv6_5tuple tuple;	/**< Holds values of the 5 tuple. */
};

/**
 * nss_ipv6_rule_conn_get_table_size_msg
 *	IPv6 rule for fetching connection tables size.
 */
struct nss_ipv6_rule_conn_get_table_size_msg {
	uint32_t num_conn;
		/**< Number of supported IPv6 connections. */
	uint32_t ce_table_size;
		/**< Size of the connection entry table in NSS firmware. */
	uint32_t cme_table_size;
		/**< Size of the connection match entry table in NSS firmware. */
};

/**
 * nss_ipv6_rule_conn_cfg_msg
 *	IPv6 rule for connection configuration sub-messages.
 */
struct nss_ipv6_rule_conn_cfg_msg {
	uint32_t num_conn;		/**< Number of supported IPv6 connections. */
	uint32_t ce_mem;		/**< Memory allocated by host for connection entries table. */
	uint32_t cme_mem;		/**< Memory allocated by host for connection match entries table. */
};

/*
 * IPv6 rule synchronization reasons.
 */
#define NSS_IPV6_RULE_SYNC_REASON_STATS 0
		/**< Rule for synchronizing statistics. */
#define NSS_IPV6_RULE_SYNC_REASON_FLUSH 1
		/**< Rule for flushing a cache entry. */
#define NSS_IPV6_RULE_SYNC_REASON_EVICT 2
		/**< Rule for evicting a cache entry. */
#define NSS_IPV6_RULE_SYNC_REASON_DESTROY 3
		/**< Rule for destroying a cache entry (requested by the host OS). */

/**
 * nss_ipv6_conn_sync
 *	IPv6 connection synchronization message.
 */
struct nss_ipv6_conn_sync {
	uint32_t reserved;		/**< Reserved field for backward compatibility. */
	uint8_t protocol;		/**< Protocol number. */
	uint32_t flow_ip[4];		/**< Flow IP address. */
	uint32_t flow_ident;		/**< Flow identifier (e.g., port). */
	uint32_t flow_max_window;	/**< Largest seen window for the flow direction. */

	/**
	 * Largest seen sequence + segment length for the flow direction.
	 */
	uint32_t flow_end;

	uint32_t flow_max_end;
			/**< Largest seen ack + max(1, win) for the flow direction. */
	uint32_t flow_rx_packet_count;	/**< Rx packet count for the flow interface. */
	uint32_t flow_rx_byte_count;	/**< Rx byte count for the flow interface. */
	uint32_t flow_tx_packet_count;	/**< Tx packet count for the flow interface. */
	uint32_t flow_tx_byte_count;	/**< Tx byte count for the flow interface. */
	uint32_t return_ip[4];		/**< Return IP address. */
	uint32_t return_ident;		/**< Return identier (e.g., port). */
	uint32_t return_max_window;
			/**< Largest seen window for the return direction. */

	/**
	 * Largest seen sequence + segment length for the return direction.
	 */
	uint32_t return_end;

	uint32_t return_max_end;
			/**< Largest seen ack + max(1, win) for the return direction. */
	uint32_t return_rx_packet_count;
			/**< Rx packet count for the return interface. */
	uint32_t return_rx_byte_count;
			/**< Rx byte count for the return interface. */
	uint32_t return_tx_packet_count;
			/**< Tx packet count for the return interface. */
	uint32_t return_tx_byte_count;
			/**< Tx byte count for the return interface. */
	uint32_t inc_ticks;	/**< Number of ticks since the last synchronization. */
	uint32_t reason;	/**< Reason for the synchronization. */
	uint8_t flags;		/**< Bit flags associated with the rule. */
	uint32_t qos_tag;	/**< QoS tag. */
	uint32_t cause;		/**< Flush cause associated with the rule. */
};

/**
 * nss_ipv6_conn_sync_many_msg
 *	Information for a multiple IPv6 connection statistics synchronization message.
 */
struct nss_ipv6_conn_sync_many_msg {
	/* Request. */
	uint16_t index;	/**< Request connection statistics from the index. */
	uint16_t size;	/**< Buffer size of this message. */

	/* Response. */
	uint16_t next;	/**< Firmware response for the next connection to be requested. */
	uint16_t count;	/**< Number of synchronized connections included in this message. */

	struct nss_ipv6_conn_sync conn_sync[];	/**< Array for the statistics. */
};

/**
 * nss_ipv6_accel_mode_cfg_msg
 *	IPv6 acceleration mode configuration.
 */
struct nss_ipv6_accel_mode_cfg_msg {
	uint32_t mode;		/**< Type of acceleration mode. */
};

/**
 * nss_ipv6_dscp2pri_cfg_msg
 *	IPv6 dscp2pri configuration msg.
 */
struct nss_ipv6_dscp2pri_cfg_msg {
	uint8_t dscp;		/**< Value of DSCP. */
	uint8_t priority;	/**< Corresponding priority. */
};

/**
 * nss_ipv6_rps_hash_bitmap_cfg_msg
 *	RPS hash mask configuration.
 *
 * The bitmap represents the host cores to which NSS firmware can steer
 * packets based on packet hash. The least significant bit represents core0.
 */
struct nss_ipv6_rps_hash_bitmap_cfg_msg {
	uint32_t hash_bitmap;	/**< Hash mask. */
};

/**
 * nss_ipv6_node_sync
 *	IPv6 node synchronization statistics.
 */
struct nss_ipv6_node_sync {
	struct nss_cmn_node_stats node_stats;		/**< Common node statistics. */
	uint32_t ipv6_connection_create_requests;
			/**< Number of connection create requests. */

	/**
	 * Number of connection create requests that collided with the existing entries.
	 */
	uint32_t ipv6_connection_create_collisions;

	/**
	 * Number of connection create requests that had invalid interfaces.
	 */
	uint32_t ipv6_connection_create_invalid_interface;

	uint32_t ipv6_connection_destroy_requests;
			/**< Number of connection destroy requests. */
	uint32_t ipv6_connection_destroy_misses;
			/**< Number of connection destroy requests that missed the cache. */
	uint32_t ipv6_connection_hash_hits;	/**< Number of connection hash hits. */
	uint32_t ipv6_connection_hash_reorders;	/**< Number of connection hash reorders. */
	uint32_t ipv6_connection_flushes;	/**< Number of connection flushes. */
	uint32_t ipv6_connection_evictions;	/**< Number of connection evictions. */
	uint32_t ipv6_fragmentations;		/**< Number of successful fragmentations. */
	uint32_t ipv6_frag_fails;		/**< Number of fragmentation fails. */
	uint32_t ipv6_dropped_by_rule;		/**< Number of packets dropped by a drop rule.*/
	uint32_t ipv6_mc_connection_create_requests;
			/**< Number of multicast connection create requests. */
	uint32_t ipv6_mc_connection_update_requests;
			/**< Number of multicast connection update requests. */

	/**
	 * Number of multicast connection create requests that had invalid interfaces.
	 */
	uint32_t ipv6_mc_connection_create_invalid_interface;

	uint32_t ipv6_mc_connection_destroy_requests;
			/**< Number of multicast connection destroy requests. */

	/**
	 * Number of multicast connection destroy requests that missed the cache.
	 */
	uint32_t ipv6_mc_connection_destroy_misses;

	uint32_t ipv6_mc_connection_flushes;
			/**< Number of multicast connection flushes. */

	uint32_t ipv6_connection_create_invalid_mirror_ifnum;
			/**< Number of create request failed with an invalid mirror interface number. */

	uint32_t ipv6_connection_create_invalid_mirror_iftype;
			/**< Number of create request failed with an invalid mirror interface type. */

	uint32_t ipv6_mirror_failures;
			/**< Mirror packet failed. */

	uint32_t exception_events[NSS_IPV6_EXCEPTION_EVENT_MAX];
			/**< Number of exception events. */
};

/**
 * nss_ipv6_msg
 *	Data for sending and receiving IPv6 bridge or routing messages.
 */
struct nss_ipv6_msg {
	struct nss_cmn_msg cm;		/**< Common message header. */

	/**
	 * Payload of an IPv6 bridge or routing message.
	 */
	union {
		struct nss_ipv6_rule_create_msg rule_create;
				/**< Create a rule. */
		struct nss_ipv6_rule_destroy_msg rule_destroy;
				/**< Destroy a rule. */
		struct nss_ipv6_conn_sync conn_stats;
				/**< Synchronize statistics. */
		struct nss_ipv6_node_sync node_stats;
				/**< Synchronize node statistics. */
		struct nss_ipv6_rule_conn_get_table_size_msg size;
				/**< Get the size for connection tables. */
		struct nss_ipv6_rule_conn_cfg_msg rule_conn_cfg;
				/**< Configure a rule connection. */
		struct nss_ipv6_mc_rule_create_msg mc_rule_create;
				/**< Create a multicast rule. */
		struct nss_ipv6_conn_sync_many_msg conn_stats_many;
				/**< Synchronize multiple connection statistics. */
		struct nss_ipv6_accel_mode_cfg_msg accel_mode_cfg;
				/**< Configure acceleration mode. */
		struct nss_ipv6_inquiry_msg inquiry;
				/**< Inquiry if a connection has been created. */
		struct nss_ipv6_dscp2pri_cfg_msg dscp2pri_cfg;
				/**< Configure DSCP-to-priority mapping. */
		struct nss_ipv6_rps_hash_bitmap_cfg_msg rps_hash_bitmap;
				/**< Configure rps_hash_bitmap. */
	} msg;			/**< Message payload. */
};

/**
 * nss_ipv6_stats_notification
 *	Data for sending IPv6 statistics.
 */
struct nss_ipv6_stats_notification {
	uint32_t core_id;					/**< Core ID. */
	uint64_t cmn_node_stats[NSS_STATS_NODE_MAX];		/**< Common node statistics. */
	uint64_t special_stats[NSS_IPV6_STATS_MAX];		/**< IPv6 special statistics. */
	uint64_t exception_stats[NSS_IPV6_EXCEPTION_EVENT_MAX];	/**< IPv6 exception statistics. */
};

/**
 * Configured IPv6 connection number to use for calculating the total number of
 * connections.
 */
extern int nss_ipv6_conn_cfg;

#ifdef __KERNEL__

/**
 * nss_ipv6_max_conn_count
 *	Returns the maximum number of IPv6 connections that the NSS acceleration
 *	engine supports.
 *
 * @return
 * Number of connections that can be accelerated.
 */
extern int nss_ipv6_max_conn_count(void);

/**
 * Callback function for receiving IPv6 messages.
 *
 * @datatypes
 * nss_ipv6_msg
 *
 * @param[in] app_data  Pointer to the application context of the message.
 * @param[in] msg       Pointer to the message data.
 */
typedef void (*nss_ipv6_msg_callback_t)(void *app_data, struct nss_ipv6_msg *msg);

/**
 * nss_ipv6_tx
 *	Transmits an IPv6 message to the NSS.
 *
 * @datatypes
 * nss_ctx_instance \n
 * nss_ipv6_msg
 *
 * @param[in] nss_ctx  Pointer to the NSS context.
 * @param[in] msg      Pointer to the message data.
 *
 * @return
 * Status of the Tx operation.
 */
extern nss_tx_status_t nss_ipv6_tx(struct nss_ctx_instance *nss_ctx, struct nss_ipv6_msg *msg);

/**
 * nss_ipv6_tx_sync
 *	Transmits a synchronous IPv6 message to the NSS.
 *
 * @datatypes
 * nss_ctx_instance \n
 * nss_ipv6_msg
 *
 * @param[in] nss_ctx  Pointer to the NSS context.
 * @param[in] msg      Pointer to the message data.
 *
 * @return
 * Status of the Tx operation.
 */
extern nss_tx_status_t nss_ipv6_tx_sync(struct nss_ctx_instance *nss_ctx, struct nss_ipv6_msg *msg);

/**
 * nss_ipv6_tx_with_size
 *	Transmits an IPv6 message with a specified size to the NSS.
 *
 * @datatypes
 * nss_ctx_instance \n
 * nss_ipv6_msg
 *
 * @param[in] nss_ctx  Pointer to the NSS context.
 * @param[in] msg      Pointer to the message data.
 * @param[in] size     Actual size of this message.
 *
 * @return
 * Status of the Tx operation.
 */
extern nss_tx_status_t nss_ipv6_tx_with_size(struct nss_ctx_instance *nss_ctx, struct nss_ipv6_msg *msg, uint32_t size);

/**
 * nss_ipv6_notify_register
 *	Registers a notifier callback to forward the IPv6 messages received from the NSS
 *	firmware to the registered subsystem.
 *
 * @datatypes
 * nss_ipv6_msg_callback_t
 *
 * @param[in] cb        Callback function for the message.
 * @param[in] app_data  Pointer to the application context of the message.
 *
 * @return
 * Pointer to the NSS core context.
 */
extern struct nss_ctx_instance *nss_ipv6_notify_register(nss_ipv6_msg_callback_t cb, void *app_data);

/**
 * nss_ipv6_notify_unregister
 *	Deregisters a notifier callback from the NSS.
 *
 * @return
 * None.
 *
 * @dependencies
 * The notifier callback must have been previously registered.
 */
extern void nss_ipv6_notify_unregister(void);

/**
 * nss_ipv6_conn_sync_many_notify_register
 *	Registers a notifier callback with the NSS for connection synchronization
 *	message responses.
 *
 * @datatypes
 * nss_ipv6_msg_callback_t
 *
 * @param[in] cb  Callback function for the message.
 *
 * @return
 * None.
 */
extern void nss_ipv6_conn_sync_many_notify_register(nss_ipv6_msg_callback_t cb);

/**
 * nss_ipv6_conn_sync_many_notify_unregister
 *	Degisters a notifier callback from the NSS.
 *
 * @return
 * None.
 *
 * @dependencies
 * The notifier callback must have been previously registered.
 */
extern void nss_ipv6_conn_sync_many_notify_unregister(void);

/**
 * nss_ipv6_get_mgr
 *	Gets the NSS context that is managing the IPv6 processes.
 *
 * @return
 * Pointer to the NSS core context.
 */
extern struct nss_ctx_instance *nss_ipv6_get_mgr(void);

/**
 * nss_ipv6_msg_init
 *	Initializes IPv6-specific messages.
 *
 * @datatypes
 * nss_ipv6_msg \n
 * nss_ipv6_msg_callback_t
 *
 * @param[in,out] nim       Pointer to the NSS interface message.
 * @param[in]     if_num    NSS interface number.
 * @param[in]     type      Type of message.
 * @param[in]     len       Size of the payload.
 * @param[in]     cb        Callback function for the message.
 * @param[in]     app_data  Pointer to the application context of the message.
 *
 * @return
 * None.
 */
extern void nss_ipv6_msg_init(struct nss_ipv6_msg *nim, uint16_t if_num, uint32_t type, uint32_t len,
			nss_ipv6_msg_callback_t cb, void *app_data);

/**
 * nss_ipv6_register_handler
 *	Registers the IPv6 message handler.
 *
 * @return
 * None.
 */
void nss_ipv6_register_handler(void);

/**
 * nss_ipv6_register_sysctl
 *	Registers the IPv6 system control table.
 *
 * @return
 * None.
 */
void nss_ipv6_register_sysctl(void);

/**
 * nss_ipv6_unregister_sysctl
 *	Deregisters the IPv6 system control table.
 *
 * @return
 * None.
 *
 * @dependencies
 * The system control table must have been previously registered.
 */
void nss_ipv6_unregister_sysctl(void);

/**
 * nss_ipv6_update_conn_count
 *	Sets the maximum number of IPv6 connections.
 *
 * @param[in] ipv6_num_conn  Maximum number.
 *
 * @return
 * 0 -- Success
 */
int nss_ipv6_update_conn_count(int ipv6_num_conn);

/**
 * nss_ipv6_free_conn_tables
 *	Frees memory allocated for connection tables.
 *
 * @return
 * None.
 */
void nss_ipv6_free_conn_tables(void);

/**
 * nss_ipv6_dscp_action_get
 *	Gets the action value of the DSCP.
 *
 * @param[in]	dscp	Value of the DSCP field.
 *
 * @return
 * Action value of the DSCP field.
 */
enum nss_ipv6_dscp_map_actions nss_ipv6_dscp_action_get(uint8_t dscp);

/*
 * Logger APIs
 */

/**
 * nss_ipv6_log_tx_msg
 *	Logs an IPv6 message that is sent to the NSS firmware.
 *
 * @datatypes
 * nss_ipv6_msg
 *
 * @param[in] nim  Pointer to the NSS interface message.
 *
 * @return
 * None.
 */
void nss_ipv6_log_tx_msg(struct nss_ipv6_msg *nim);

/**
 * nss_ipv6_log_rx_msg
 *	Logs an IPv6 message that is received from the NSS firmware.
 *
 * @datatypes
 * nss_ipv6_msg
 *
 * @param[in] nim  Pointer to the NSS interface message.
 *
 * @return
 * None.
 */
void nss_ipv6_log_rx_msg(struct nss_ipv6_msg *nim);

/**
 * nss_ipv6_stats_register_notifier
 *	Registers a statistics notifier.
 *
 * @datatypes
 * notifier_block
 *
 * @param[in] nb Notifier block.
 *
 * @return
 * 0 on success or -2 on failure.
 */
extern int nss_ipv6_stats_register_notifier(struct notifier_block *nb);

/**
 * nss_ipv6_stats_unregister_notifier
 *	Deregisters a statistics notifier.
 *
 * @datatypes
 * notifier_block
 *
 * @param[in] nb Notifier block.
 *
 * @return
 * 0 on success or -2 on failure.
 */
extern int nss_ipv6_stats_unregister_notifier(struct notifier_block *nb);
#endif

/**
 * @}
 */

#endif /* __NSS_IPV6_H */

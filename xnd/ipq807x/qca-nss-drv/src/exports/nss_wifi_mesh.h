/*
 **************************************************************************
 * Copyright (c) 2021, The Linux Foundation. All rights reserved.
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 **************************************************************************
 */

/**
 * @file nss_wifi_mesh.h
 *	NSS TO HLOS Wi-Fi mesh device interface definitions.
 */

#ifndef __NSS_WIFI_MESH_H
#define __NSS_WIFI_MESH_H

/**
 * @addtogroup nss_wifi_mesh_subsystem
 * @{
 */
#define NSS_WIFI_MESH_WIFI_HDRLEN_MAX 48
/*
 * Wi-Fi mesh maximum dynamic interface.
 */
#define NSS_WIFI_MESH_MAX_DYNAMIC_INTERFACE 32

/**
 * Mesh path update flags.
 */
#define NSS_WIFI_MESH_PATH_UPDATE_FLAG_NEXTHOP 0x01
#define NSS_WIFI_MESH_PATH_UPDATE_FLAG_HOPCOUNT 0x02
#define NSS_WIFI_MESH_PATH_UPDATE_FLAG_METRIC 0x04
#define NSS_WIFI_MESH_PATH_UPDATE_FLAG_MESH_FLAGS 0x08
#define NSS_WIFI_MESH_PATH_UPDATE_FLAG_EXPIRY_TIME 0x10
#define NSS_WIFI_MESH_PATH_UPDATE_FLAG_MESH_GATE 0x20
#define NSS_WIFI_MESH_PATH_UPDATE_FLAG_BLOCK_MESH_FWD 0x40
#define NSS_WIFI_MESH_PATH_UPDATE_FLAG_METADATA_ENABLE_VALID 0x80

/**
 * Mesh proxy path update flags.
 */
#define NSS_WIFI_MESH_PROXY_PATH_UPDATE_FLAG_MDA 0x1
#define NSS_WIFI_MESH_PROXY_PATH_UPDATE_FLAG_MESH 0x2

/**
 * Mesh path update flags.
 */
#define NSS_WIFI_MESH_PATH_FLAG_REFRESH_SENT 0x1

/**
 * Mesh path maximum entries.
 */
#define NSS_WIFI_MESH_PATH_MAX_ENTRIES 10

/**
 * Mesh proxy path maximum entries.
 */
#define NSS_WIFI_MESH_PROXY_PATH_MAX_ENTRIES 10

/**
 * Mesh configuration flags.
 */
#define NSS_WIFI_MESH_CONFIG_FLAG_TTL_VALID 0x01
#define NSS_WIFI_MESH_CONFIG_FLAG_MPATH_REFRESH_VALID 0x02
#define NSS_WIFI_MESH_CONFIG_FLAG_BLOCK_MESH_FORWARDING_VALID 0x04
#define NSS_WIFI_MESH_CONFIG_FLAG_LOCAL_MAC_VALID 0x08
#define NSS_WIFI_MESH_CONFIG_FLAG_MPP_LEARNING_MODE_VALID 0x10
#define NSS_WIFI_MESH_CONFIG_FLAG_SIBLING_IF_NUM_VALID 0x20
#define NSS_WIFI_MESH_CONFIG_FLAG_BLOCK_MESH_FWD_VALID 0x40
#define NSS_WIFI_MESH_CONFIG_FLAG_METADATA_ENABLE_VALID 0x80

/**
 * nss_wifi_mesh_path_flags
 *	Wi-Fi mesh path flags.
 */
#define NSS_WIFI_MESH_PATH_FLAG_ACTIVE 0x01
#define NSS_WIFI_MESH_PATH_FLAG_RESOLVING 0x02
#define NSS_WIFI_MESH_PATH_FLAG_RESOLVED 0x04
#define NSS_WIFI_MESH_PATH_FLAG_FIXED 0x08

#define NSS_WIFI_MESH_ENCAP_METADATA_OFFSET_TYPE 4

/*
 * nss_wifi_mesh_pre_header_type {
 *	Wi-Fi pre header types.
 */
enum nss_wifi_mesh_pre_header_type {
	NSS_WIFI_MESH_PRE_HEADER_NONE = 0xdcb,		/**< No preheader. */
	NSS_WIFI_MESH_PRE_HEADER_80211 = 0xabc,		/**< 802.11 preheader. */
	NSS_WIFI_MESH_PRE_HEADER_MAX = 0xdea		/**< Max preheader. */
};

/*
 * nss_wifi_mesh_extended_data_pkt_types
 * 	Wi-Fi mesh extended data pkt types.
 */
enum nss_wifi_mesh_extended_data_pkt_types {
	WIFI_MESH_EXT_DATA_PKT_TYPE_NONE,		/**< No packet type. */
	WIFI_MESH_EXT_DATA_PKT_TYPE_EXCEPTION		/**< Exception packet type. */
};

/*
 * nss_wifi_mesh_ieee80211_hdr
 *	Wi-Fi header
 */
struct nss_wifi_mesh_ieee80211_hdr {
	uint16_t frame_ctl;				/* Frame control. */
	uint16_t duration_id;				/* Duration ID. */
	uint8_t addr1[ETH_ALEN];			/* Address 1. */
	uint8_t addr2[ETH_ALEN];			/* Address 2. */
	uint8_t addr3[ETH_ALEN];			/* Address 3. */
	uint16_t seq_ctrl;				/* Sequence control. */
	uint8_t addr4[ETH_ALEN];			/* Address 4. */
}__packed;

/*
 * nss_wifi_mesh_ieee80211s_hdr
 *	Wi-Fi mesh header
 */
struct nss_wifi_mesh_ieee80211s_hdr {
	uint8_t flags;					/* Mesh flags. */
	uint8_t ttl;					/* TTL. */
	uint32_t seq_num;				/* Sequence number. */
	uint8_t eaddr1[ETH_ALEN];			/* Mesh Address1. */
	uint8_t eaddr2[ETH_ALEN];			/* Mesh Address2. */
}__packed;

/*
 * nss_wifi_mesh_per_packet_metadata
 * 	Wi-Fi mesh per packet metadata structure.
 */
struct nss_wifi_mesh_per_packet_metadata {
	uint16_t pkt_type;					/* Packet type of the metadata. */
	uint8_t wifi_hdr_bytes[NSS_WIFI_MESH_WIFI_HDRLEN_MAX];	/* Wi-Fi header byte stream. */
};

/**
 * nss_wifi_mesh_dp_type
 *	Interface datapath types.
 *	NSS-to-host path will be seen by ECM for rules.
 */
enum nss_wifi_mesh_dp_type {
	NSS_WIFI_MESH_DP_INNER,		/**< Inner/Encapsulation Interface. */
	NSS_WIFI_MESH_DP_OUTER,		/**< Outer/Decapsulation Interface. */
};

/**
 * nss_wifi_mesh_msg_types
 *	Wi-Fi mesh messages.
 */
enum nss_wifi_mesh_msg_types {
	NSS_WIFI_MESH_MSG_INTERFACE_CONFIGURE = NSS_IF_MAX_MSG_TYPES + 1,	/**< Wi-Fi mesh interface configure message. */
	NSS_WIFI_MESH_MSG_MPATH_ADD,						/**< Wi-Fi mesh path add message. */
	NSS_WIFI_MESH_MSG_MPATH_DELETE,						/**< Wi-Fi mesh path delete message. */
	NSS_WIFI_MESH_MSG_MPATH_UPDATE,						/**< Wi-Fi mesh path update. */
	NSS_WIFI_MESH_MSG_PROXY_PATH_LEARN,					/**< Wi-Fi mesh proxy path learn. */
	NSS_WIFI_MESH_MSG_PROXY_PATH_ADD,					/**< Wi-Fi mesh proxy path add. */
	NSS_WIFI_MESH_MSG_PROXY_PATH_DELETE,					/**< Wi-Fi mesh proxy path delete. */
	NSS_WIFI_MESH_MSG_PROXY_PATH_UPDATE,					/**< Wi-Fi mesh proxy path update. */
	NSS_WIFI_MESH_MSG_PATH_NOT_FOUND,					/**< Wi-Fi mesh path not found message. */
	NSS_WIFI_MESH_MSG_PATH_REFRESH,						/**< Wi-Fi mesh path refresh message. */
	NSS_WIFI_MESH_MSG_PATH_EXPIRY,						/**< Wi-Fi mesh path expiry message. */
	NSS_WIFI_MESH_MSG_PATH_TABLE_DUMP,					/**< Wi-Fi mesh path table dump. */
	NSS_WIFI_MESH_MSG_PROXY_PATH_TABLE_DUMP,				/**< Wi-Fi mesh proxy path table dump. */
	NSS_WIFI_MESH_MSG_STATS_SYNC,						/**< Wi-Fi mesh statistics sync messgae. */
	NSS_WIFI_MESH_MSG_EXCEPTION_FLAG,					/**< Wi-Fi mesh exception Flag. */
	NSS_WIFI_MESH_CONFIG_EXCEPTION,						/**< Wi-Fi mesh configuration exception. */
	NSS_WIFI_MESH_MSG_MAX							/**< Wi-Fi mesh maximum message. */
};

/*
 * nss_wifi_mesh_error_types
 * Wi-Fi mesh error types.
 */
enum nss_wifi_mesh_error_types {
	NSS_WIFI_MESH_ERROR_NONE = NSS_IF_ERROR_TYPE_MAX + 1,		/**< Wi-Fi mesh no error type. */
	NSS_WIFI_MESH_ERROR_UNKNOWN_MSG,				/**< Wi-Fi mesh unknown message error. */
	NSS_WIFI_MESH_ERROR_TTL_CONFIG,					/**< Wi-Fi mesh invalid ttl error. */
	NSS_WIFI_MESH_ERROR_REFRESH_TIME_CONFIG,			/**< Wi-Fi mesh invalid refresh time. */
	NSS_WIFI_MESH_ERROR_MPP_LEARNING_MODE_CONFIG,			/**< Wi-Fi mesh invalid mpp learning mode. */
	NSS_WIFI_MESH_ERROR_PATH_ADD_MAX_RADIO_CNT,			/**< Wi-Fi mesh path add error due to maximum radio count. */
	NSS_WIFI_MESH_ERROR_PATH_ADD_INVALID_INTERFACE_NUM,		/**< Wi-Fi mesh path invalid interface number. */
	NSS_WIFI_MESH_ERROR_PATH_ADD_INTERFACE_NUM_NOT_FOUND,		/**< Wi-Fi mesh path interface number not found. */
	NSS_WIFI_MESH_ERROR_PATH_TABLE_FULL,				/**< Wi-Fi mesh path table full error. */
	NSS_WIFI_MESH_ERROR_PATH_ALLOC_FAIL,				/**< Wi-Fi mesh path alloc error. */
	NSS_WIFI_MESH_ERROR_PATH_INSERT_FAIL,				/**< Wi-Fi mesh path insert fail. */
	NSS_WIFI_MESH_ERROR_PATH_NOT_FOUND,				/**< Wi-Fi mesh path not found error. */
	NSS_WIFI_MESH_ERROR_PATH_UNHASHED,				/**< Wi-Fi mesh proxy path unhashed error. */
	NSS_WIFI_MESH_ERROR_PATH_DELETE_FAIL,				/**< Wi-Fi mesh proxy path delete error. */
	NSS_WIFI_MESH_ERROR_PROXY_PATH_NOT_FOUND,			/**< Wi-Fi mesh proxy path not found error. */
	NSS_WIFI_MESH_ERROR_PROXY_PATH_UNHASHED,			/**< Wi-Fi mesh proxy path unhashed error. */
	NSS_WIFI_MESH_ERROR_PROXY_PATH_DELETE_FAIL,			/**< Wi-Fi mesh proxy path delete error. */
	NSS_WIFI_MESH_ERROR_PROXY_PATH_EXISTS,				/**< Wi-Fi mesh proxy path exists error. */
	NSS_WIFI_MESH_ERROR_PROXY_PATH_ALLOC_FAIL,			/**< Wi-Fi mesh proxy path alloc error. */
	NSS_WIFI_MESH_ERROR_PROXY_PATH_INSERT_FAIL,			/**< Wi-Fi mesh proxy path insert error. */
	NSS_WIFI_MESH_ERROR_PROXY_PATH_TABLE_FULL,			/**< Wi-Fi mesh proxy path table full error. */
	NSS_WIFI_MESH_ERROR_PB_ALLOC_FAIL,				/**< Wi-Fi mesh pbuf allocation failures. */
	NSS_WIFI_MESH_ERROR_ENQUEUE_TO_HOST_FAIL,			/**< Wi-Fi mesh enqueue to host failures. */
	NSS_WIFI_MESH_ERROR_ENABLE_INTERFACE_FAIL,			/**< Wi-Fi mesh enabling interface failures. */
	NSS_WIFI_MESH_ERROR_DISABLE_INTERFACE_FAIL,			/**< Wi-Fi mesh disabling interface failures. */
	NSS_WIFI_MESH_ERROR_INVALID_EXCEPTION_NUM,			/**< Wi-Fi mesh invalid exception number. */
	NSS_WIFI_MESH_ERROR_ONESHOT_ALREADY_ATTACHED,			/**< Wi-Fi mesh oneshot already attached error. */
};

/**
 * nss_wifi_mesh_mpp_learning_mode
 *	Mesh device proxy path learning types.
 */
enum nss_wifi_mesh_mpp_learning_mode {
	NSS_WIFI_MESH_MPP_LEARNING_MODE_INDEPENDENT_NSS,		/**< Independent NSS learning. */
	NSS_WIFI_MESH_MPP_LEARNING_MODE_NSS_ASSISTED_HOST,		/**< NSS assisted host learning. */
	NSS_WIFI_MESH_MPP_LEARNING_MODE_MAX				/**< Mesh maximum learning type. */
};

/**
 * nss_wifi_mesh_configurable_exceptions
 *	Mesh configurable exceptions.
 */
enum nss_wifi_mesh_configurable_exceptions {
	NSS_WIFI_MESH_DS_MESH_PATH_NOT_FOUND = 1,		/**< Downstream (Eth - Wi-Fi) mesh path not found exception. */
	NSS_WIFI_MESH_US_MESH_PROXY_NOT_FOUND = 2,		/**< Upstream (Wi-Fi - Eth) mesh proxy path not found exception. */
	NSS_WIFI_MESH_US_MESH_PATH_NOT_FOUND = 3,		/**< Upstream (Wi-Fi - Eth) mesh path not found exception. */
	NSS_WIFI_MESH_EXCEPTION_MAX = 4
};

/*
 * nss_wifi_mesh_encap_ext_data_pkt_type
 *	Mesh encap extended data packet type.
 */
enum nss_wifi_mesh_encap_ext_data_pkt_type {
	NSS_WIFI_MESH_ENCAP_EXT_DATA_PKT_TYPE_NONE,			/**< No packet type. */
	NSS_WIFI_MESH_ENCAP_EXT_DATA_PKT_TYPE_MPATH_NOT_FOUND_EXC,	/**< Packet when mesh path is not found. */
	NSS_WIFI_MESH_ENCAP_EXT_DATA_PKT_TYPE_MAX,			/**< Maximum packet type. */
};

/**
 * nss_wifi_mesh_config_msg
 *	Mesh device configuration.
 */
struct nss_wifi_mesh_config_msg {
	uint8_t local_mac_addr[ETH_ALEN];	/**< Local MAC address. */
	uint16_t reserved;			/**< Reserved field. */
	uint32_t ttl;				/**< TTL for packet. */
	uint32_t mesh_path_refresh_time;	/**< Mesh path refresh time. */
	uint32_t config_flags;			/**< Flags indicating which fields are valid. */
	uint32_t sibling_ifnum;			/**< Sibling interface number. */
	uint8_t mpp_learning_mode;             /**< Mesh proxy path learning mode. */
	uint8_t block_mesh_forwarding;		/**< If enabled, blocks packet forwarding. */
	uint8_t reserved_2[2];			/**< Reserved bytes. */
	uint32_t metadata_type;			/**< Indicates if metadata should be enabled when block_mesh_forwarding is true. */
};

/**
 * nss_wifi_mesh_mpath_add_msg
 *	Add a mesh path message for a mesh device.
 */
struct nss_wifi_mesh_mpath_add_msg {
	uint8_t dest_mac_addr[ETH_ALEN];	/**< Destination MAC address. */
	uint8_t next_hop_mac_addr[ETH_ALEN];	/**< Next hop MAC address. */
	uint32_t metric;			/**< Metric for a mesh path. */
	uint32_t link_vap_id;			/**< Radio ID of the mesh path. */
	uint32_t expiry_time;			/**< Expiry time in order of ms. */
	uint8_t hop_count;			/**< Hop count. */
	uint8_t path_flags;			/**< Mesh path flags. */
	uint8_t is_mesh_gate;			/**< Destination of this path is a mesh gate. */
	uint8_t block_mesh_fwd;			/**< Block intra mesh forward. */
	uint32_t metadata_type;			/**< Indicates if metadata should be enabled when block_mesh_forwarding is true. */
};

/**
 * nss_wifi_mesh_mpath_delete_msg
 *	Delete a mesh path message for a mesh device.
 */
struct nss_wifi_mesh_mpath_del_msg {
	uint32_t link_vap_id;			/**< Radio ID of the mesh path. */
	uint8_t mesh_dest_mac_addr[ETH_ALEN];	/**< Destination MAC address. */
	uint8_t next_hop_mac_addr[ETH_ALEN];	/**< Next hop MAC address. */
};

/**
 * nss_wifi_mesh_mpath_update_msg
 *	Update a mesh path message for a mesh device.
 */
struct nss_wifi_mesh_mpath_update_msg {
	uint8_t dest_mac_addr[ETH_ALEN];	/**< Destination MAC address. */
	uint8_t next_hop_mac_addr[ETH_ALEN];	/**< Next hop MAC address. */
	uint8_t old_next_hop_mac_addr[ETH_ALEN];/**< Old next hop MAC address. */
	uint32_t metric;			/**< Metric for a mesh path. */
	uint32_t link_vap_id;			/**< Radio ID of the mesh path. */
	uint32_t expiry_time;			/**< Expiration time of mesh path. */
	uint8_t hop_count;			/**< Hop count. */
	uint8_t path_flags;			/**< Mesh path flags. */
	uint8_t is_mesh_gate;			/**< Indicates if the mesh path is a mesh gate. */
	uint8_t update_flags;			/**< Update flags. */
	uint8_t block_mesh_fwd;			/**< Block intra mesh forward. */
	uint8_t reserved[3];			/**< Reserved bytes. */
	uint8_t metadata_type;			/**< Indicates if metadata should be enabled when block_mesh_forwarding is true. */
};

/**
 * nss_wifi_mesh_proxy_path_learn_msg
 *	Learn a mesh proxy path message for a mesh device.
 */
struct nss_wifi_mesh_proxy_path_learn_msg {
	uint8_t mesh_dest_mac[ETH_ALEN];	/**< Mesh destination MAC address. */
	uint8_t dest_mac_addr[ETH_ALEN];	/**< Destination MAC address. */
	uint8_t path_flags;			/**< Mesh path flags. */
	uint8_t is_update;			/**< Indicates if the learn is an update. */
};

/**
 * nss_wifi_mesh_proxy_path_add_msg
 *	Add a mesh proxy path message for a mesh device.
 */
struct nss_wifi_mesh_proxy_path_add_msg {
	uint8_t mesh_dest_mac[ETH_ALEN];	/**< Mesh destination MAC address. */
	uint8_t dest_mac_addr[ETH_ALEN];	/**< Destination MAC address. */
	uint8_t path_flags;			/**< Mesh path flags. */
};

/**
 * nss_wifi_mesh_proxy_path_update_msg
 *	Update a mesh proxy path message for a mesh device.
 */
struct nss_wifi_mesh_proxy_path_update_msg {
	uint8_t mesh_dest_mac[ETH_ALEN];	/**< Mesh destination MAC address. */
	uint8_t dest_mac_addr[ETH_ALEN];	/**< Destination MAC address. */
	uint8_t path_flags;			/**< Mesh path flags. */
	uint8_t bitmap;				/**< Bitmap indicating valid fields in the update msg. */
};

/**
 * nss_wifi_mesh_proxy_path_del_msg
 *	Delete a mesh proxy path message for a mesh device.
 */
struct nss_wifi_mesh_proxy_path_del_msg {
	uint8_t mesh_dest_mac_addr[ETH_ALEN];	/**< Mesh destination MAC. */
	uint8_t dest_mac_addr[ETH_ALEN];	/**< Destination MAC address. */
};

/**
 * nss_wifi_mesh_mpath_not_found_msg
 *	Wi-Fi mesh path not found meesage.
 */
struct nss_wifi_mesh_mpath_not_found_msg {
	uint8_t dest_mac_addr[ETH_ALEN];		/**< Destination MAC address. */
	uint8_t transmitter_mac_addr[ETH_ALEN];		/**< Transmitter address. */
	uint32_t link_vap_id;				/**< NSS interface number of the link vap if received from WiFi. */
	uint8_t is_mesh_forward_path;			/**< Indicates if the message is from a forward path. */
};

/**
 * nss_wifi_mesh_path_refresh_msg
 *	Refresh mesh path message.
 */
struct nss_wifi_mesh_path_refresh_msg {
	uint8_t dest_mac_addr[ETH_ALEN];		/**< Destination MAC address. */
	uint8_t next_hop_mac_addr[ETH_ALEN];		/**< Next hop MAC address. */
	uint32_t link_vap_id;				/**< Link VAP of the mesh path. */
	uint8_t path_flags;				/**< Mesh path flags. */
};

/**
 * nss_wifi_mesh_path_expiry_msg
 *	Mesh path expiration message.
 */
struct nss_wifi_mesh_path_expiry_msg {
	uint8_t mesh_dest_mac_addr[ETH_ALEN];		/**< Destination MAC address. */
	uint8_t next_hop_mac_addr[ETH_ALEN];		/**< Next hop MAC address. */
	uint32_t link_vap_id;				/**< Link VAP of the mesh path. */
	uint8_t path_flags;				/**< Mesh path flags. */
};

/*
 * nss_wifi_mesh_encap_stats
 *	Encapsulation statistics.
 */
struct nss_wifi_mesh_encap_stats {
	uint32_t expiry_notify_sent;		/* Number of times expiry notification sent to host. */
	uint32_t mc_count;			/* Number of multicast packets. */
	uint32_t mp_not_found;			/* Number of times mesh path is not found. */
	uint32_t mp_active;			/* Number of times mesh path is active. */
	uint32_t mpp_not_found;			/* Number of times proxy path is not found. */
	uint32_t mpp_found;			/* Number of times proxy path is found. */
	uint32_t encap_hdr_fail;		/* Number of times encapsulating mesh header failed. */
	uint32_t mp_del_notify_fail;		/* Number of times notifying mesh path delete failed. */
	uint32_t link_enqueue;			/* Number of packets enqueued to the link VAP. */
	uint32_t link_enq_fail;			/* Number of times enqueue to link vap failed. */
	uint32_t ra_lup_fail;			/* Number of times receiver address look up is failed. */
	uint32_t dummy_add_count;		/* Number of times dummy path is added. */
	uint32_t encap_mp_add_notify_fail;	/* Number of times add notification failed. */
	uint32_t dummy_add_fail;		/* Number of times dummy addition failed. */
	uint32_t dummy_lup_fail;		/* Number of times dummy lookup failed. */
	uint32_t send_to_host_failed;		/* Number of packets failed to be sent to host. */
	uint32_t sent_to_host;			/* Number of packets sent to host. */
	uint32_t expiry_notify_fail; 		/* Number of times expiry notification to host failed. */
	uint32_t no_headroom;			/* Number of packets dropped because there is no headroom. */
	uint32_t path_refresh_sent;		/* Number of times path refresh is sent to host. */
	uint32_t linearise_failed;		/* Number of packets dropped because pb_linearise. */
};

/*
 * nss_wifi_mesh_decap_stats
 *	Mesh decapsulation statistics.
 */
struct nss_wifi_mesh_decap_stats {
	uint32_t path_refresh_sent;		/**< Number of times path refresh is sent to host. */
	uint32_t reserved;			/**< Reserved field. */
	uint32_t mc_drop;			/**< Number of MC drop counts. */
	uint32_t ttl_0;				/**< Number of TTL0 counts. */
	uint32_t mpp_lup_fail;			/**< Number of mpp lookup failures. */
	uint32_t decap_hdr_fail;		/**< Number of decap HDR failures. */
	uint32_t rx_fwd_fail;			/**< Number of receive forward failures. */
	uint32_t rx_fwd_success;		/**< Number of receive forward success counts. */
	uint32_t mp_fwd_lookup_fail;		/**< Number of mpath forward lookup failures. */
	uint32_t mp_fwd_inactive;		/**< Number of mpath forward inactive. */
	uint32_t nxt_mnode_fwd_success;		/**< Number of next mnode forward successes. */
	uint32_t nxt_mnode_fwd_fail;		/**< Number of next mnode forward failures. */
	uint32_t mpp_add_fail;			/**< Number of MPP add failures. */
	uint32_t mpp_add_event2host_fail;	/**< Number of MPP add event-to-host failures. */
	uint32_t mpp_upate_fail;		/**< Number of MPP update failures. */
	uint32_t mpp_update_even2host_fail;	/**< Number of MPP update event-to-host failure counts. */
	uint32_t mpp_learn2host_fail;		/**< Number of MPP learn-to-host failure counts. */
	uint32_t block_mesh_fwd_packets;	/**< Number of packets that are blocked for intra mesh forward. */
	uint32_t no_headroon;			/**< Number of packets dropped due to insufficient headroom.. */
	uint32_t linearise_failed;		/**< Number of packets dropped due to linear copy failure. */
	uint32_t mpp_learn_events_rate_limited;	/**< Number of mesh proxy path learn events dropped due to rate limiting */
	uint32_t mp_missing_events_rate_limited;/**< Number of path missing notifications dropped due to rate limiting */
};

/**
 * nss_wifi_mesh_path_dump_entry
 *	Wi-Fi mesh path dump entry.
 */
struct nss_wifi_mesh_path_dump_entry {
	uint8_t dest_mac_addr[ETH_ALEN];		/**< Destination MAC address. */
	uint8_t next_hop_mac_addr[ETH_ALEN];		/**< Next hop MAC address. */
	uint32_t metric;				/**< Mesh path metric. */
	uint32_t expiry_time[2];			/**< Mesh path expiration time. */
	uint8_t hop_count;				/**< Number of hop counts. */
	uint8_t flags;					/**< Mesh path flags. */
	uint8_t is_mesh_gate;				/**< Determines whether gateway capability is enabled. */
	uint8_t reserved[1];				/**< Reserved field. */
	uint32_t link_vap_id;				/**< Link interface number. */
};

/**
 * nss_wifi_mesh_proxy_path_dump_entry
 *	Wi-Fi mesh proxy path dump entry.
 */
struct nss_wifi_mesh_proxy_path_dump_entry {
	uint8_t dest_mac_addr[ETH_ALEN];		/**< Destination MAC address. */
	uint8_t mesh_dest_mac[ETH_ALEN];		/**< Mesh destination address. */
	uint8_t flags;					/**< Mesh path flags. */
	uint8_t reserved[3];				/**< Reserved field. */
	uint32_t time_diff;				/**< Difference of current time and active time. */
};

/**
 * nss_wifi_mesh_path_table_dump
 *	Wi-Fi mesh path table dump.
 */
struct nss_wifi_mesh_path_table_dump {
	uint32_t num_entries;									/**< Number of entries. */
	uint32_t more_events;									/**< Determines whether more events are pending. */
	struct nss_wifi_mesh_path_dump_entry path_entry[0];					/**< Mesh path entries. */
};

/**
 * nss_wifi_mesh_proxy_path_table_dump
 *	Wi-Fi mesh proxy path table dump.
 */
struct nss_wifi_mesh_proxy_path_table_dump {
	uint32_t num_entries;										/**< Number of entries. */
	uint32_t more_events;										/**< More events are pending. */
	struct nss_wifi_mesh_proxy_path_dump_entry path_entry[0];	/**< Mesh proxy path entry. */
};

/**
 * nss_wifi_mesh_assoc_link_vap
 *	Associate a link VAP to mesh.
 */
struct nss_wifi_mesh_assoc_link_vap {
	uint32_t link_vap_id;		/**< Link interface number. */
};

/**
 * nss_wifi_mesh_path_stats
 *	Wi-Fi mesh path statistics.
 */
struct nss_wifi_mesh_path_stats {
	uint32_t alloc_failures;			/**< Mesh path allocation failures. */
	uint32_t error_max_radio_count;			/**< Mesh path error maximum radio count. */
	uint32_t invalid_interface_failures;		/**< Mesh path invalid interface number failures count. */
	uint32_t add_success;				/**< Mesh path add success count. */
	uint32_t table_full_errors;			/**< Mesh path table full error count. */
	uint32_t insert_failures;			/**< Mesh path insert failure count. */
	uint32_t not_found;				/**< Mesh path not found failure count. */
	uint32_t delete_success;			/**< Mesh path delete success count. */
	uint32_t update_success;			/**< Mesh path update success count. */
	uint32_t mesh_path_expired;			/**< Mesh path expired. */
	uint32_t mesh_path_refresh_needed;		/**< Mesh path refresh needed. */
	uint32_t add_requests;				/**< Mesh path add request. */
	uint32_t del_requests;				/**< Mesh path delete request. */
	uint32_t update_requests;			/**< Mesh path update requests. */
	uint32_t next_hop_updations;			/**< Mesh path next hop updations. */
	uint32_t hop_count_updations;			/**< Mesh path hop count updations. */
	uint32_t flag_updations;			/**< Mesh path mesh flag updations. */
	uint32_t metric_updations;			/**< Mesh path metric updations. */
	uint32_t block_mesh_fwd_updations;		/**< Mesh path block mesh forward updations. */
	uint32_t delete_failures;			/**< Mesh path delete failures. */
};

/**
 * nss_wifi_mesh_proxy_path_stats
 *	Wi-Fi mesh proxy path statistics.
 */
struct nss_wifi_mesh_proxy_path_stats {
	uint32_t alloc_failures;		/**< Mesh proxy path alloc failure count. */
	uint32_t entry_exist_failures;		/**< Mesh proxy path entry already exists. */
	uint32_t add_success;			/**< Mesh proxy path add success count. */
	uint32_t table_full_errors;		/**< Mesh proxy path table full count. */
	uint32_t insert_failures;		/**< Mesh proxy path insert failure count. */
	uint32_t not_found;			/**< Mesh proxy path not found count. */
	uint32_t unhashed_errors;		/**< Mesh proxy path unhased erorr count. */
	uint32_t delete_failures;		/**< Mesh proxy path delete failure count. */
	uint32_t delete_success;		/**< Mesh proxy path delete success count. */
	uint32_t update_success;		/**< Mesh proxy path update success count. */
	uint32_t lookup_success;		/**< Mesh proxy path lookup success count. */
	uint32_t add_requests;			/**< Mesh proxy path addition requests. */
	uint32_t del_requests;			/**< Mesh proxy path deletion requests. */
	uint32_t update_requests;		/**< Mesh proxy path updation requests. */
	uint32_t mda_updations;			/**< Mesh proxy path mda updations. */
	uint32_t flag_updations;		/**< Mesh proxy path flags updations. */
};

/**
 * nss_wifi_mesh_exception_stats
 *	Wi-Fi mesh exception statistics.
 */
struct nss_wifi_mesh_exception_stats {
	uint32_t packets_success;	/**< Mesh exception successful packets count. */
	uint32_t packets_dropped;	/**< Mesh exception dropped packets count. */
};

/**
 * nss_wifi_mesh_stats_sync_msg
 *	Message to get mesh device statistics from NSS firmware to the host.
 */
struct nss_wifi_mesh_stats_sync_msg {
	struct nss_cmn_node_stats pnode_stats;					/**< Common firmware statistics. */
	struct nss_wifi_mesh_encap_stats mesh_encap_stats;            		/**< Encapsulation statistics. */
	struct nss_wifi_mesh_decap_stats mesh_decap_stats;			/**< Decapsulation statistics. */
	struct nss_wifi_mesh_path_stats mesh_path_stats;			/**< Mesh path statistics. */
	struct nss_wifi_mesh_proxy_path_stats mesh_proxy_path_stats;		/**< Mesh proxy path statistics. */
	struct nss_wifi_mesh_exception_stats mesh_except_stats;			/**< Mesh exception statistics. */
};

/* nss_wifi_mesh_exception_flag_msg
 * 	Messsage to send exception packets to host.
 */
struct nss_wifi_mesh_exception_flag_msg {
	uint8_t dest_mac_addr[ETH_ALEN];		/**< Destination MAC address. */
	uint8_t exception;				/**< Exception flag bit. */
	uint8_t reserved[2];				/**< Reserved field. */
};

/**
 * nss_wifi_mesh_rate_limit_config
 *	Message to configure exceptions
 */
struct nss_wifi_mesh_rate_limit_config {
	uint32_t exception_num;				/**< Indicates the exception - enum wifi_mesh_configurable_exceptions. */
	uint32_t enable;				/**< Indicates if exception is enabled. */
	uint32_t rate_limit;				/**< Rate limit value in us. */
};

/**
 * nss_wifi_mesh_encap_ext_pkt_metadata
 *	Metadata to extended data callback
 */
struct nss_wifi_mesh_encap_ext_pkt_metadata {
	uint16_t pkt_type;				/**< Packet type of the exception packet. */
};

/**
 * nss_wifi_mesh_msg
 *	Data sent and received in mesh device-specific messages.
 */
struct nss_wifi_mesh_msg {
	struct nss_cmn_msg cm;		/**< Common message header. */

	/**
	 * Payload of a virtual device specific message.
	 */
	union {
		union nss_if_msgs if_msg;
				/**< NSS interface base message. */
		struct nss_wifi_mesh_config_msg mesh_config;
				/**< Mesh device configuration. */
		struct nss_wifi_mesh_mpath_add_msg mpath_add;
				/**< Add a message for a mesh path addition. */
		struct nss_wifi_mesh_mpath_del_msg mpath_del;
				/**< Add a message for a mesh path deletion. */
		struct nss_wifi_mesh_mpath_update_msg mpath_update;
				/**< Add a message for a mesh path update. */
		struct nss_wifi_mesh_proxy_path_learn_msg proxy_learn_msg;
				/**< Add a message for a mesh proxy path learning. */
		struct nss_wifi_mesh_proxy_path_add_msg proxy_add_msg;
				/**< Add a message for a mesh proxy path addition. */
		struct nss_wifi_mesh_proxy_path_update_msg proxy_update_msg;
				/**< Add a message for a mesh proxy path update. */
		struct nss_wifi_mesh_proxy_path_del_msg proxy_del_msg;
				/**< Add a message for a mesh proxy path deletion. */
		struct nss_wifi_mesh_mpath_not_found_msg mpath_not_found_msg;
				/**< Mesh path not found message. */
		struct nss_wifi_mesh_path_refresh_msg path_refresh_msg;
				/**< Add a message for a mesh path refresh. */
		struct nss_wifi_mesh_path_expiry_msg path_expiry_msg;
				/**< Add a message for a mesh path expiration. */
		struct nss_wifi_mesh_path_table_dump mpath_table_dump;
				/**< Add a message to dump mesh path table. */
		struct nss_wifi_mesh_proxy_path_table_dump proxy_path_table_dump;
				/**< Add a message to dump mesh proxy path table. */
		struct nss_wifi_mesh_stats_sync_msg stats_sync_msg;
				/**< Statistics synchronization message. */
		struct nss_wifi_mesh_exception_flag_msg exception_msg;
				/**< Exception to host message. */
		struct nss_wifi_mesh_rate_limit_config exc_cfg;
				/**< Add a message to configure the rate limit for exception events. */
	} msg;		/**< Virtual device message payload. */
};

/**
 * nss_wifi_mesh_encap_stats_type
 *	Wi-Fi mesh encapsulation statistics types.
 */
enum nss_wifi_mesh_encap_stats_type {
	NSS_WIFI_MESH_ENCAP_STATS_TYPE_PNODE_RX_PACKETS,		/**< Wi-Fi mesh common node receive packets. */
	NSS_WIFI_MESH_ENCAP_STATS_TYPE_PNODE_RX_BYTES,			/**< Wi-Fi mesh common node receive bytes. */
	NSS_WIFI_MESH_ENCAP_STATS_TYPE_PNODE_TX_PACKETS,		/**< Wi-Fi mesh common node transmit packets. */
	NSS_WIFI_MESH_ENCAP_STATS_TYPE_PNODE_TX_BYTES,			/**< Wi-Fi mesh common node transmit bytes. */
	NSS_WIFI_MESH_ENCAP_STATS_TYPE_PNODE_RX_DROPPED,		/**< Wi-Fi mesh common node receive dropped. */
	NSS_WIFI_MESH_ENCAP_STATS_TYPE_EXPIRY_NOTIFY_SENT,		/**< Wi-Fi mesh encapsulation statistics expiry notify sent. */
	NSS_WIFI_MESH_ENCAP_STATS_TYPE_MC_COUNT,			/**< Wi-Fi mesh encapsulation statistics mc count. */
	NSS_WIFI_MESH_ENCAP_STATS_TYPE_MP_NOT_FOUND,			/**< Wi-Fi mesh encapsulation statistics mpath not found. */
	NSS_WIFI_MESH_ENCAP_STATS_TYPE_MP_ACTIVE,			/**< Wi-Fi mesh encapsulation statistics mpath active */
	NSS_WIFI_MESH_ENCAP_STATS_TYPE_MPP_NOT_FOUND,			/**< Wi-Fi mesh encapsulation statistics mpp not found. */
	NSS_WIFI_MESH_ENCAP_STATS_TYPE_MPP_FOUND,			/**< Wi-Fi mesh encapsulation statistics mpp found. */
	NSS_WIFI_MESH_ENCAP_STATS_TYPE_HDR_FAIL,			/**< Wi-Fi mesh encapsulation statistics header failed. */
	NSS_WIFI_MESH_ENCAP_STATS_TYPE_MP_DEL_NOTIFY_FAIL,		/**< Wi-Fi mesh encapsulation statistics mpath delete notify. */
	NSS_WIFI_MESH_ENCAP_STATS_TYPE_LINK_ENQUEUE,			/**< Wi-Fi mesh encapsulation statistics link enqueue. */
	NSS_WIFI_MESH_ENCAP_STATS_TYPE_LINK_ENQUEUE_FAIL,		/**< Wi-Fi mesh encapsulation statistics link enqueue failed. */
	NSS_WIFI_MESH_ENCAP_STATS_TYPE_RA_LOOKUP_FAIL,			/**< Wi-Fi mesh encapsulation statistics receiver lookup failed. */
	NSS_WIFI_MESH_ENCAP_STATS_TYPE_DUMMY_ADD_COUNT,			/**< Wi-Fi mesh encapsulation statistics dummy add count. */
	NSS_WIFI_MESH_ENCAP_STATS_TYPE_MP_ADD_NOTIFY_FAIL,		/**< Wi-Fi mesh encapsulation statistics mpath add notify failed. */
	NSS_WIFI_MESH_ENCAP_STATS_TYPE_DUMMY_ADD_FAIL,			/**< Wi-Fi mesh encapsulation statistics dummy add failed. */
	NSS_WIFI_MESH_ENCAP_STATS_TYPE_DUMMY_LOOKUP_FAIL,		/**< Wi-Fi mesh encapsulation statistics dummy look-up failed. */
	NSS_WIFI_MESH_ENCAP_STATS_TYPE_SEND_TO_HOST_FAILED,		/**< Wi-Fi mesh encapsulation statistics when a packet fails to send to host. */
	NSS_WIFI_MESH_ENCAP_STATS_TYPE_SENT_TO_HOST,			/**< Wi-Fi mesh encapsulation statistics when packet is sent to host. */
	NSS_WIFI_MESH_ENCAP_STATS_TYPE_EXPIRY_NOTIFY_FAIL,		/**< Wi-Fi mesh encapsulation statistics expiry notified fail. */
	NSS_WIFI_MESH_ENCAP_STATS_TYPE_NO_HEADROOM,			/**< Wi-Fi mesh encapsulation statistics no headroom. */
	NSS_WIFI_MESH_ENCAP_STATS_TYPE_PATH_REFRESH_SENT,		/**< Wi-Fi mesh encapsulation statistics path refresh sent. */
	NSS_WIFI_MESH_ENCAP_STATS_TYPE_LINEARISE_FAILED,		/**< Wi-Fi mesh encapsulation statistics when linearisation failed. */
	NSS_WIFI_MESH_ENCAP_STATS_TYPE_MAX				/**< Wi-Fi mesh encapsulation statistics maximum. */
};

/**
 * nss_wifi_mesh_decap_stats_type
 *	Wi-Fi mesh decapsulation statistics types.
 */
enum nss_wifi_mesh_decap_stats_type {
	NSS_WIFI_MESH_DECAP_STATS_TYPE_PNODE_RX_PACKETS,		/**< Wi-Fi mesh common node receive packets. */
	NSS_WIFI_MESH_DECAP_STATS_TYPE_PNODE_RX_BYTES,			/**< Wi-Fi mesh common node receive bytes. */
	NSS_WIFI_MESH_DECAP_STATS_TYPE_PNODE_TX_PACKETS,		/**< Wi-Fi mesh common node transmit packets. */
	NSS_WIFI_MESH_DECAP_STATS_TYPE_PNODE_TX_BYTES,			/**< Wi-Fi mesh common node transmit bytes. */
	NSS_WIFI_MESH_DECAP_STATS_TYPE_PNODE_RX_DROPPED,		/**< Wi-Fi mesh common node receive dropped. */
	NSS_WIFI_MESH_DECAP_STATS_TYPE_PATH_REFRESH_SENT,		/**< Wi-Fi mesh decapsulation statistics path refresh sent. */
	NSS_WIFI_MESH_DECAP_STATS_TYPE_RESERVED,			/**< Wi-Fi mesh decapsulation statistics reserved field. */
	NSS_WIFI_MESH_DECAP_STATS_TYPE_MC_DROP,				/**< Wi-Fi mesh decapsulation statistics MAC dropped count. */
	NSS_WIFI_MESH_DECAP_STATS_TYPE_TTL0,				/**< Wi-Fi mesh decapsulation statistics ttl0. */
	NSS_WIFI_MESH_DECAP_STATS_TYPE_MPP_LOOKUP_FAIL,			/**< Wi-Fi mesh decapsulation statistics mpp lookup failed. */
	NSS_WIFI_MESH_DECAP_STATS_TYPE_HDR_FAIL,			/**< Wi-Fi mesh decapsulation statistics header failed. */
	NSS_WIFI_MESH_DECAP_STATS_TYPE_RX_FWD_FAIL,			/**< Wi-Fi mesh decapsulation statistics receive forward failed. */
	NSS_WIFI_MESH_DECAP_STATS_TYPE_RX_FWD_SUCCESS,			/**< Wi-Fi mesh decapsulation statistics receive forward successful. */
	NSS_WIFI_MESH_DECAP_STATS_TYPE_MP_FWD_LOOKUP_FAIL,		/**< Wi-Fi mesh decapsulation statistics mpath forward lookup failed. */
	NSS_WIFI_MESH_DECAP_STATS_TYPE_MP_FWD_INACTIVE,			/**< Wi-Fi mesh decapsulation statistics mpath forward inactive. */
	NSS_WIFI_MESH_DECAP_STATS_TYPE_MNODE_FWD_SUCCESS,		/**< Wi-Fi mesh decapsulation statistics mnode forward successful. */
	NSS_WIFI_MESH_DECAP_STATS_TYPE_MNODE_FWD_FAIL,			/**< Wi-Fi mesh decapsulation statistics mnode forward failed. */
	NSS_WIFI_MESH_DECAP_STATS_TYPE_MPP_ADD_FAIL,			/**< Wi-Fi mesh decapsulation statistics mpp add failed. */
	NSS_WIFI_MESH_DECAP_STATS_TYPE_MPP_ADD_EVENT_TO_HOST_FAIL,	/**< Wi-Fi mesh decapsulation statistics mpp add event to host failed. */
	NSS_WIFI_MESH_DECAP_STATS_TYPE_MPP_UPDATE_FAIL,			/**< Wi-Fi mesh decapsulation statistics mpp update failed. */
	NSS_WIFI_MESH_DECAP_STATS_TYPE_MPP_UPDATE_EVENT_TO_HOST_FAIL,	/**< Wi-Fi mesh decapsulation statistics mpp update event to host failed. */
	NSS_WIFI_MESH_DECAP_STATS_TYPE_MPP_LEARN_TO_HOST_FAIL,		/**< Wi-Fi mesh decapsulation statistics mpp learn to host failed. */
	NSS_WIFI_MESH_DECAP_STATS_TYPE_BLOCK_MESH_FWD_PACKETS,		/**< Wi-Fi mesh decapsulation statistics block mesh fwd packets. */
	NSS_WIFI_MESH_DECAP_STATS_TYPE_NO_HEADROOM,			/**< Wi-Fi mesh decapsulation statistics no headroom. */
	NSS_WIFI_MESH_DECAP_STATS_TYPE_LINEARISE_FAILED,		/**< Wi-Fi mesh decapsulation statistics linearise failed. */
	NSS_WIFI_MESH_DECAP_STATS_TYPE_MPP_LEARN_EVENT_RL_DROPPED,	/**< Wi-Fi mesh decapsulation statistics mpp learn event rl dropped. */
	NSS_WIFI_MESH_DECAP_STATS_TYPE_MP_MISSING_EVENT_RL_DROPPED,	/**< Wi-Fi mesh decapsulation statistics mp missing event rl dropped. */
	NSS_WIFI_MESH_DECAP_STATS_TYPE_MAX				/**< Wi-Fi mesh decapsulation statistics maximum. */
};

/**
 * nss_wifi_mesh_path_stats_type
 *	Wi-Fi mesh path statistics types.
 */
enum nss_wifi_mesh_path_stats_type {
	NSS_WIFI_MESH_PATH_STATS_TYPE_ALLOC_FAILURES,			/**< Wi-Fi mesh path statistics allocation failures. */
	NSS_WIFI_MESH_PATH_STATS_TYPE_ERROR_MAX_RADIO_COUNT,		/**< Wi-Fi mesh path statistics maximum radio error count. */
	NSS_WIFI_MESH_PATH_STATS_TYPE_INVALID_INTERFACE_FAILURES,	/**< Wi-Fi mesh path statistics invalid interface failures. */
	NSS_WIFI_MESH_PATH_STATS_TYPE_ADD_SUCCESS,			/**< Wi-Fi mesh path statistics add success. */
	NSS_WIFI_MESH_PATH_STATS_TYPE_TABLE_FULL_ERRORS,		/**< Wi-Fi mesh path statistics table full errors. */
	NSS_WIFI_MESH_PATH_STATS_TYPE_INSERT_FAILURES,			/**< Wi-Fi mesh path statistics insertion failures. */
	NSS_WIFI_MESH_PATH_STATS_TYPE_NOT_FOUND,			/**< Wi-Fi mesh path statistics not found. */
	NSS_WIFI_MESH_PATH_STATS_TYPE_DELETE_SUCCESS,			/**< Wi-Fi mesh path statistics successful deletion. */
	NSS_WIFI_MESH_PATH_STATS_TYPE_UPDATE_SUCCESS,			/**< Wi-Fi mesh path statistics successful updation. */
	NSS_WIFI_MESH_PATH_STATS_TYPE_EXPIRED,				/**< Wi-Fi mesh path statistics expired. */
	NSS_WIFI_MESH_PATH_STATS_TYPE_REFRESH_NEEDED,			/**< Wi-Fi mesh path statistics refresh needed. */
	NSS_WIFI_MESH_PATH_STATS_TYPE_ADD_REQUESTS,			/**< Wi-Fi mesh path statistics add requests. */
	NSS_WIFI_MESH_PATH_STATS_TYPE_DELETE_REQUESTS,			/**< Wi-Fi mesh path statistics delete requests. */
	NSS_WIFI_MESH_PATH_STATS_TYPE_UPDATE_REQUESTS,			/**< Wi-Fi mesh path statistics update requests. */
	NSS_WIFI_MESH_PATH_STATS_TYPE_NEXT_HOP_UPDATIONS,		/**< Wi-Fi mesh path statistics next hop updations. */
	NSS_WIFI_MESH_PATH_STATS_TYPE_HOP_COUNT_UPDATIONS,		/**< Wi-Fi mesh path statistics hop count updations. */
	NSS_WIFI_MESH_PATH_STATS_TYPE_MESH_FLAG_UPDATIONS,		/**< Wi-Fi mesh path statistics mesh flag updations. */
	NSS_WIFI_MESH_PATH_STATS_TYPE_METRIC_UPDATIONS,			/**< Wi-Fi mesh path statistics metric updations. */
	NSS_WIFI_MESH_PATH_STATS_TYPE_BLOCK_MESH_FWD_UPDATIONS,		/**< Wi-Fi mesh path statistics block mesh forward updations. */
	NSS_WIFI_MESH_PATH_STATS_TYPE_MESH_PATH_DELETE_FAILURES,	/**< Wi-Fi mesh path statistics mesh path delete failures. */
	NSS_WIFI_MESH_PATH_STATS_TYPE_MAX				/**< Wi-Fi mesh path statistics maximum. */
};

/**
 * nss_wifi_mesh_proxy_path_stats_type
 *	Wi-Fi mesh proxy path statistics types.
 */
enum nss_wifi_mesh_proxy_path_stats_type {
	NSS_WIFI_MESH_PROXY_PATH_STATS_TYPE_ALLOC_FAILURES,		/**< Wi-Fi mesh proxy path statistics allocation failures. */
	NSS_WIFI_MESH_PROXY_PATH_STATS_TYPE_ENTRY_EXIST_FAILURES,	/**< Wi-Fi mesh proxy path statistics entry exist failures. */
	NSS_WIFI_MESH_PROXY_PATH_STATS_TYPE_ADD_SUCCESS,		/**< Wi-Fi mesh proxy path statistics add success. */
	NSS_WIFI_MESH_PROXY_PATH_STATS_TYPE_TABLE_FULL_ERRORS,		/**< Wi-Fi mesh proxy path statistics table full errors. */
	NSS_WIFI_MESH_PROXY_PATH_STATS_TYPE_INSERT_FAILURES,		/**< Wi-Fi mesh proxy path statistics insert failures. */
	NSS_WIFI_MESH_PROXY_PATH_STATS_TYPE_NOT_FOUND,			/**< Wi-Fi mesh proxy path statistics not found. */
	NSS_WIFI_MESH_PROXY_PATH_STATS_TYPE_UNHASHED_ERRORS,		/**< Wi-Fi mesh proxy path statistics unhashed errors. */
	NSS_WIFI_MESH_PROXY_PATH_STATS_TYPE_DELTE_FAILURES,		/**< Wi-Fi mesh proxy path statistics delete failures */
	NSS_WIFI_MESH_PROXY_PATH_STATS_TYPE_DELETE_SUCCESS,		/**< Wi-Fi mesh proxy path statistics delete success. */
	NSS_WIFI_MESH_PROXY_PATH_STATS_TYPE_UPDATE_SUCCESS,		/**< Wi-Fi mesh proxy path statistics update_success. */
	NSS_WIFI_MESH_PROXY_PATH_STATS_TYPE_LOOKUP_SUCCESS,		/**< Wi-Fi mesh proxy path statistics lookup sccesss. */
	NSS_WIFI_MESH_PROXY_PATH_STATS_TYPE_ADD_REQUESTS,		/**< Wi-Fi mesh proxy path statistics add requests. */
	NSS_WIFI_MESH_PROXY_PATH_STATS_TYPE_DELETE_REQUESTS,		/**< Wi-Fi mesh proxy path statistics delete requests. */
	NSS_WIFI_MESH_PROXY_PATH_STATS_TYPE_UPDATE_REQUESTS,		/**< Wi-Fi mesh proxy path statistics update request. */
	NSS_WIFI_MESH_PROXY_PATH_STATS_TYPE_MDA_UPDATIONS,		/**< Wi-Fi mesh proxy path statistics mda updations. */
	NSS_WIFI_MESH_PROXY_PATH_STATS_TYPE_FLAGS_UPDATIONS,		/**< Wi-Fi mesh proxy path statistics flags updations. */
	NSS_WIFI_MESH_PROXY_PATH_STATS_TYPE_MAX				/**< Wi-Fi mesh proxy path statistics maximum. */
};

/**
 * nss_wifi_mesh_exception_stats_type
 *	Wi-Fi mesh exception statistics types.
 */
enum nss_wifi_mesh_exception_stats_type {
	NSS_WIFI_MESH_EXCEPTION_STATS_TYPE_PACKETS_SUCCESS,		/**< Wi-Fi mesh exception statistics packets success. */
	NSS_WIFI_MESH_EXCEPTION_STATS_TYPE_PACKETS_DROPPED,		/**< Wi-Fi mesh exception statistics packets dropped. */
	NSS_WIFI_MESH_EXCEPTION_STATS_TYPE_MAX				/**< Wi-Fi mesh exception statistics maximum. */
};

/**
 * nss_wifi_mesh_hdl_stats_sync_msg
 * 	Message to get mesh device statistics from NSS firmware to the host.
 */
struct nss_wifi_mesh_hdl_stats_sync_msg {
	uint64_t encap_stats[NSS_WIFI_MESH_ENCAP_STATS_TYPE_MAX];		/**< Encap statistics. */
	uint64_t decap_stats[NSS_WIFI_MESH_DECAP_STATS_TYPE_MAX];		/**< Decap statistics. */
	uint64_t path_stats[NSS_WIFI_MESH_PATH_STATS_TYPE_MAX];			/**< Path statistics. */
	uint64_t proxy_path_stats[NSS_WIFI_MESH_PROXY_PATH_STATS_TYPE_MAX];	/**< Proxy path statistics. */
	uint64_t except_stats[NSS_WIFI_MESH_EXCEPTION_STATS_TYPE_MAX];		/**< Exception statistics. */
};

/**
 * nss_wifi_mesh_stats_notification
 * 	Wi-Fi mesh statistics structure.
 */
struct nss_wifi_mesh_stats_notification {
	uint32_t core_id;				/**< Core ID. */
	nss_if_num_t if_num;				/**< Interface number. */
	struct nss_wifi_mesh_hdl_stats_sync_msg stats;	/**< Encapsulation-decapsulation statistics. */
};

/**
 * nss_wifi_mesh_tx_msg
 *	Sends a Wi-Fi mesh message to the NSS interface.
 *
 * @datatypes
 * nss_ctx_instance \n
 * nss_wifi_mesh_msg
 *
 * @param[in] nss_ctx  Pointer to the NSS core context.
 * @param[in] msg      Pointer to the message data.
 *
 * @return
 * Status of the transmit operation.
 */
nss_tx_status_t nss_wifi_mesh_tx_msg(struct nss_ctx_instance *nss_ctx,
				struct nss_wifi_mesh_msg *msg);

/**
 * nss_wifi_mesh_tx_buf
 *	Sends a Wi-Fi mesh data packet to the NSS interface.
 *
 * @datatypes
 * nss_ctx_instance \n
 * sk_buff
 *
 * @param[in] nss_ctx  Pointer to the NSS core context.
 * @param[in] os_buf   Pointer to the OS data buffer.
 * @param[in] if_num   NSS interface number.
 *
 * @return
 * Status of the transmit operation.
 */
nss_tx_status_t nss_wifi_mesh_tx_buf(struct nss_ctx_instance *nss_ctx,
				struct sk_buff *os_buf, nss_if_num_t if_num);

/**
 * Callback function for receiving Wi-Fi virtual device messages.
 *
 * @datatypes
 * nss_cmn_msg
 *
 * @param[in] app_data  Pointer to the application context of the message.
 * @param[in] msg       Pointer to the message data.
 */
typedef void (*nss_wifi_mesh_msg_callback_t)(void *app_data,
					struct nss_cmn_msg *msg);

/**
 * Callback function for receiving Wi-Fi virtual device data.
 *
 * @datatypes
 * net_device \n
 * sk_buff \n
 * napi_struct
 *
 * @param[in] netdev  Pointer to the associated network device.
 * @param[in] skb     Pointer to the data socket buffer.
 * @param[in] napi    Pointer to the NAPI structure.
 */
typedef void (*nss_wifi_mesh_data_callback_t)(struct net_device *netdev,
				struct sk_buff *skb, struct napi_struct *napi);

/**
 * Callback function for receiving extended data plane Wi-Fi virtual device data.
 *
 * @datatypes
 * net_device \n
 * sk_buff \n
 * napi_struct
 *
 * @param[in] netdev  Pointer to the associated network device.
 * @param[in] skb     Pointer to the data socket buffer.
 * @param[in] napi    Pointer to the NAPI structure.
 * @param[in] netdev  Pointer to the associated network device.
 */
typedef void (*nss_wifi_mesh_ext_data_callback_t)(struct net_device *netdev,
				struct sk_buff *skb, struct napi_struct *napi);

/**
 * nss_wifi_mesh_msg_init
 *	Initializes a Wi-Fi mesh device message.
 *
 * @datatypes
 * nss_wifi_mesh_msg \n
 * nss_wifi_mesh_msg_callback_t
 *
 * @param[in] nim       Pointer to the NSS interface message.
 * @param[in] if_num    NSS interface number.
 * @param[in] type      Type of message.
 * @param[in] len       Length of message.
 * @param[in] cb        Message callback.
 * @param[in] app_data  Pointer to the application context of the message.
 *
 * @return
 * None.
 */
void nss_wifi_mesh_msg_init(struct nss_wifi_mesh_msg *nim, nss_if_num_t if_num, uint32_t type, uint32_t len,
				nss_wifi_mesh_msg_callback_t cb, void *app_data);

/**
 * nss_wifi_mesh_get_context
 *	Gets the NSS Wi-Fi extended virtual interface context.
 *
 * @return
 * Pointer to the NSS core context.
 */
extern struct nss_ctx_instance *nss_wifi_mesh_get_context(void);

/**
 * nss_register_wifi_mesh_if
 *	Registers a Wi-Fi mesh device interface with the NSS interface.
 *
 * @datatypes
 * nss_if_num_t \n
 * nss_wifi_mesh_data_callback_t \n
 * nss_wifi_mesh_ext_data_callback_t \n
 * nss_wifi_mesh_msg_callback_t \n
 * net_device
 * @param[in]     if_num                  NSS interface number.
 * @param[in]     mesh_data_callback      Callback for the Wi-Fi virtual device data.
 * @param[in]     mesh_ext_data_callback  Callback for the extended data.
 * @param[in]     mesh_event_callback     Callback for the message.
 * @param[in]	  dp_type                 Datapath type.
 * @param[in]     netdev                  Pointer to the associated network device.
 * @param[in]     features                Data socket buffer types supported by this
 *                                        interface.
 *
 * @return
 * NSS_CORE_STATUS_SUCCESS in case of success.
 * NSS_CORE_STATUS_FAILURE in case of failure.
 */
uint32_t nss_register_wifi_mesh_if(nss_if_num_t if_num, nss_wifi_mesh_data_callback_t mesh_data_callback,
			nss_wifi_mesh_ext_data_callback_t mesh_ext_data_callback, nss_wifi_mesh_msg_callback_t mesh_event_callback,
			uint32_t dp_type, struct net_device *netdev, uint32_t features);

/**
 * nss_unregister_wifi_mesh_if
 *	Deregisters a Wi-Fi mesh device interface from the NSS interface.
 *
 * @param[in] if_num  NSS interface number.
 *
 * @return
 * None.
 */
void nss_unregister_wifi_mesh_if(nss_if_num_t if_num);

/**
 * nss_wifi_mesh_tx_msg_ext
 *	Sends Wi-Fi mesh data packet along with metadata as a message to the NSS.
 *
 * @datatypes
 * nss_ctx_instance \n
 * sk_buff
 *
 * @param[in,out] nss_ctx  Pointer to the NSS core context.
 * @param[in]     os_buf   Pointer to the OS data buffer.
 *
 * @return
 * Status of the transmit operation.
 */
nss_tx_status_t nss_wifi_mesh_tx_msg_ext(struct nss_ctx_instance *nss_ctx, struct sk_buff *os_buf);

/**
 * nss_wifi_mesh_verify_if_num
 *	Verify Wi-Fi mesh interface number.
 *
 * @datatypes
 * interface number \n
 *
 * @param[in]  nss_if_num_t  NSS interface number.
 *
 * @return
 * TRUE or FALSE.
 */
extern bool nss_wifi_mesh_verify_if_num(nss_if_num_t if_num);

/**
 * nss_wifi_mesh_stats_register_notifier
 *	Registers a statistics notifier.
 *
 * @datatypes
 * notifier_block
 *
 * @param[in] nb Notifier block.
 *
 * @return
 * 0 on success or non-zero on failure.
 */
extern int nss_wifi_mesh_stats_register_notifier(struct notifier_block *nb);

/**
 * nss_wifi_mesh_stats_unregister_notifier
 *	Deregisters a statistics notifier.
 *
 * @datatypes
 * notifier_block
 *
 * @param[in] nb Notifier block.
 *
 * @return
 * 0 on success or non-zero on failure.
 */
extern int nss_wifi_mesh_stats_unregister_notifier(struct notifier_block *nb);
#endif /* __NSS_WIFI_MESH_H */

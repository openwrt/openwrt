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
 * @file nss_wifi_vdev.h
 *	NSS-to-HLOS Wi-Fi virtual device interface definitions.
 */

#ifndef __NSS_WIFI_VDEV_H
#define __NSS_WIFI_VDEV_H

/**
 * @addtogroup nss_wifi_vdev_subsystem
 * @{
 */
#define NSS_WIFI_HTT_TRANSFER_HDRSIZE_WORD 6	/**< Size of the Host-To-Target (HTT) message transfer header. */
#define NSS_WIFI_VDEV_PER_PACKET_METADATA_OFFSET 4
/**< Offset of the metadata in a virtual device message. */
#define NSS_WIFI_VDEV_DSCP_MAP_LEN 64		/**< Length of the DSCP MAP field. */
#define NSS_WIFI_VDEV_IPV6_ADDR_LENGTH 16	/**< Size of the IPv6 address field. */
#define NSS_WIFI_MAX_SRCS 4			/**< Maximum number of multicast sources. */
#define NSS_WIFI_VDEV_MAX_ME_ENTRIES 32		/**< Maximum number of multicast enhancement entries. */

/**
 * nss_wifi_vdev_msg_types
 *	Wi-Fi virtual device messages.
 */
enum nss_wifi_vdev_msg_types {
	NSS_WIFI_VDEV_INTERFACE_CONFIGURE_MSG = NSS_IF_MAX_MSG_TYPES + 1,
	NSS_WIFI_VDEV_INTERFACE_UP_MSG,
	NSS_WIFI_VDEV_INTERFACE_DOWN_MSG,
	NSS_WIFI_VDEV_INTERFACE_CMD_MSG,
	NSS_WIFI_VDEV_SNOOPLIST_GRP_LIST_CREATE_MSG,
	NSS_WIFI_VDEV_SNOOPLIST_GRP_LIST_DELETE_MSG,
	NSS_WIFI_VDEV_SNOOPLIST_GRP_MEMBER_ADD_MSG,
	NSS_WIFI_VDEV_SNOOPLIST_GRP_MEMBER_REMOVE_MSG,
	NSS_WIFI_VDEV_SNOOPLIST_GRP_MEMBER_UPDATE_MSG,
	NSS_WIFI_VDEV_SNOOPLIST_DENY_MEMBER_ADD_MSG,
	NSS_WIFI_VDEV_SNOOPLIST_DENY_LIST_DELETE_MSG,
	NSS_WIFI_VDEV_SNOOPLIST_DENY_LIST_DUMP_MSG,
	NSS_WIFI_VDEV_SNOOPLIST_DUMP_MSG,
	NSS_WIFI_VDEV_SNOOPLIST_RESET_MSG,
	NSS_WIFI_VDEV_SPECIAL_DATA_TX_MSG,
	NSS_WIFI_VDEV_VOW_DBG_CFG_MSG,
	NSS_WIFI_VDEV_VOW_DBG_STATS_REQ_MSG,
	NSS_WIFI_VDEV_DSCP_TID_MAP_MSG,
	NSS_WIFI_VDEV_SNOOPLIST_TOGGLE_MSG,
	NSS_WIFI_VDEV_UPDATECHDR_MSG,
	NSS_WIFI_VDEV_ME_SYNC_MSG,
	NSS_WIFI_VDEV_STATS_MSG,
	NSS_WIFI_VDEV_SET_NEXT_HOP,
	NSS_WIFI_VDEV_DSCP_TID_MAP_ID_MSG,
	NSS_WIFI_VDEV_EXTAP_ADD_ENTRY,
	NSS_WIFI_VDEV_EXTAP_REMOVE_ENTRY,
	NSS_WIFI_VDEV_QWRAP_PSTA_DELETE_ENTRY,
	NSS_WIFI_VDEV_QWRAP_PSTA_ADD_ENTRY,
	NSS_WIFI_VDEV_QWRAP_ISOLATION_ENABLE,
	NSS_WIFI_VDEV_SET_PEER_NEXT_HOP,
	NSS_WIFI_VDEV_CONFIG_VLAN_ID_MSG,
	NSS_WIFI_VDEV_CONFIG_VLAN_MODE_MSG,
	NSS_WIFI_VDEV_INTERFACE_RECOVERY_RESET_MSG,
	NSS_WIFI_VDEV_INTERFACE_RECOVERY_RECONF_MSG,
	NSS_WIFI_VDEV_SET_GROUP_KEY,
	NSS_WIFI_VDEV_HMMC_MEMBER_ADD_MSG,
	NSS_WIFI_VDEV_HMMC_MEMBER_DEL_MSG,
	NSS_WIFI_VDEV_MAX_MSG
};

/**
 * nss_wifi_vdev_err_types
 *	Error types for a Wi-Fi virtual device.
 */
enum nss_wifi_vdev_err_types {
	NSS_WIFI_VDEV_ENONE,
	NSS_WIFI_VDEV_EUNKNOWN_MSG,
	NSS_WIFI_VDEV_EINV_VID_CONFIG,
	NSS_WIFI_VDEV_EINV_EPID_CONFIG,
	NSS_WIFI_VDEV_EINV_DL_CONFIG,
	NSS_WIFI_VDEV_EINV_CMD,
	NSS_WIFI_VDEV_EINV_ENCAP,
	NSS_WIFI_VDEV_EINV_DECAP,
	NSS_WIFI_VDEV_EINV_RX_NXTN,
	NSS_WIFI_VDEV_EINV_VID_INDEX,
	NSS_WIFI_VDEV_EINV_MC_CFG,
	NSS_WIFI_VDEV_SNOOPTABLE_FULL,
	NSS_WIFI_VDEV_SNOOPTABLE_ENOMEM,
	NSS_WIFI_VDEV_SNOOPTABLE_GRP_LIST_UNAVAILABLE,
	NSS_WIFI_VDEV_SNOOPTABLE_GRP_MEMBER_UNAVAILABLE,
	NSS_WIFI_VDEV_SNOOPTABLE_PEER_UNAVAILABLE,
	NSS_WIFI_VDEV_SNOOPTABLE_GRP_LIST_ENOMEM,
	NSS_WIFI_VDEV_SNOOPTABLE_GRP_LIST_EXIST,
	NSS_WIFI_VDEV_ME_ENOMEM,
	NSS_WIFI_VDEV_EINV_NAWDS_CFG,
	NSS_WIFI_VDEV_EINV_EXTAP_CFG,
	NSS_WIFI_VDEV_EINV_VOW_DBG_CFG,
	NSS_WIFI_VDEV_EINV_DSCP_TID_MAP,
	NSS_WIFI_VDEV_INVALID_ETHER_TYPE,
	NSS_WIFI_VDEV_SNOOPTABLE_GRP_MEMBER_EXIST,
	NSS_WIFI_VDEV_ME_INVALID_NSRCS,
	NSS_WIFI_VDEV_EINV_RADIO_ID,
	NSS_WIFI_VDEV_RADIO_NOT_PRESENT,
	NSS_WIFI_VDEV_CHDRUPD_FAIL,
	NSS_WIFI_VDEV_ME_DENY_GRP_MAX_RCHD,
	NSS_WIFI_VDEV_EINV_NEXT_HOP,
	NSS_WIFI_VDEV_EINV_DSCP_TID_MAP_ID,
	NSS_WIFI_VDEV_EINV_TID_VALUE,
	NSS_WIFI_VDEV_EINV_EXTAP_TABLE,
	NSS_WIFI_VDEV_EXTAP_ENTRY_UPDATE_FAIL,
	NSS_WIFI_VDEV_QWRAP_PSTA_ADD_FAIL,
	NSS_WIFI_VDEV_QWRAP_PSTA_DEL_FAIL,
	NSS_WIFI_VDEV_QWRAP_ISOLATION_EN_FAIL,
	NSS_WIFI_VDEV_QWRAP_ALLOC_FAIL,
	NSS_WIFI_VDEV_PEER_NOT_FOUND_BY_MAC,
	NSS_WIFI_VDEV_PEER_NEXT_HOP_NOT_FOUND,
	NSS_VDEV_EUNKNOWN_NEXT_HOP,
	NSS_WIFI_VDEV_VLAN_ID_CONFIG_FAIL,
	NSS_WIFI_VDEV_VLAN_MODE_CONFIG_FAIL,
	NSS_WIFI_VDEV_RECOVERY_RESET_FAIL,
	NSS_WIFI_VDEV_RECOVERY_RECONF_FAIL,
	NSS_WIFI_VDEV_CONFIG_GROUP_KEY_FAIL,
	NSS_WIFI_VDEV_MULTIPASS_NOT_ENABLED,
	NSS_WIFI_VDEV_ALLOC_VLAN_MAP_FAILED,
	NSS_WIFI_VDEV_MTU_CHANGE_FAIL,
	NSS_WIFI_VDEV_MAC_ADDR_CHANGE_FAIL,
	NSS_WIFI_VDEV_PPE_PORT_CREATE_FAIL,
	NSS_WIFI_VDEV_PPE_PORT_DESTROY_FAIL,
	NSS_WIFI_VDEV_PPE_VSI_ASSIGN_FAIL,
	NSS_WIFI_VDEV_PPE_VSI_UNASSIGN_FAIL,
	NSS_WIFI_VDEV_EINV_MAX_CFG
};

/**
 * nss_wifi_vdev_ext_data_pkt_type
 *	Types of extended data plane packets sent from the NSS to the host.
 */
enum nss_wifi_vdev_ext_data_pkt_type {
	NSS_WIFI_VDEV_EXT_DATA_PKT_TYPE_NONE = 0,
	NSS_WIFI_VDEV_EXT_DATA_PKT_TYPE_IGMP = 1,	/**< IGMP packets. */
	NSS_WIFI_VDEV_EXT_DATA_PKT_TYPE_MESH = 2,	/**< MESH packets. */
	NSS_WIFI_VDEV_EXT_DATA_PKT_TYPE_INSPECT = 3,	/**< Host inspect packets. */
	NSS_WIFI_VDEV_EXT_DATA_PKT_TYPE_TXINFO = 4,	/**< Tx completion information packets. */
	NSS_WIFI_VDEV_EXT_DATA_PKT_TYPE_MPSTA_TX = 5,	/**< MP station Tx metadata. */
	NSS_WIFI_VDEV_EXT_DATA_PKT_TYPE_MPSTA_RX = 6,	/**< MP station Rx metadata. */
	NSS_WIFI_VDEV_EXT_DATA_PKT_TYPE_RX_ERR = 7,	/**< Rx error packets metadata. */
	NSS_WIFI_VDEV_EXT_DATA_PKT_TYPE_EXTAP_TX = 8,	/**< ExtAP Tx metadata. */
	NSS_WIFI_VDEV_EXT_DATA_PKT_TYPE_EXTAP_RX = 9,	/**< ExtAP Rx metadata. */
	NSS_WIFI_VDEV_EXT_DATA_PKT_TYPE_WNM_TFS = 10,	/**< WNM TFS related metadata. */
	NSS_WIFI_VDEV_EXT_TX_COMPL_PKT_TYPE = 11,	/**< Tx completion. */
	NSS_WIFI_VDEV_EXT_DATA_PKT_TYPE_WDS_LEARN = 12,	/**< WDS source port learning command. */
	NSS_WIFI_VDEV_EXT_DATA_PPDU_INFO = 13,		/**< PPDU metadata information. */
	NSS_WIFI_VDEV_EXT_DATA_PKT_TYPE_MCBC_RX = 14,	/**< Multicast/broadcast packet received. */
	NSS_WIFI_VDEV_MESH_EXT_DATA_PKT_TYPE_RX_SPL_PACKET = 15,
							/**< Mesh link VAP special packet. */
	NSS_WIFI_VDEV_MESH_EXT_DATA_PKT_TYPE_RX_MCAST_EXC = 16,
							/**< Mesh link VAP multicast packet. */
	NSS_WIFI_VDEV_EXT_DATA_PKT_TYPE_MAX
};

/**
 * nss_wifi_vdev_cmd
 *	Commands for the Wi-Fi virtual device.
 */
enum nss_wifi_vdev_cmd {
	NSS_WIFI_VDEV_DROP_UNENC_CMD,	/**< Configuration to drop unencrypted frames on VAP. */
	NSS_WIFI_VDEV_ENCAP_TYPE_CMD,	/**< Configuration to set encapsulation type on VAP. */
	NSS_WIFI_VDEV_DECAP_TYPE_CMD,	/**< Configuration to set decapsulation type on VAP. */
	NSS_WIFI_VDEV_ENABLE_ME_CMD,	/**< Configuration to set multicast enhancement on VAP. */
	NSS_WIFI_VDEV_NAWDS_MODE_CMD,	/**< Configuration to set NAWDS mode on VAP. */
	NSS_WIFI_VDEV_EXTAP_CONFIG_CMD,	/**< Configuration to set extended AP mode on VAP. */
	NSS_WIFI_VDEV_CFG_BSTEER_CMD,	/**< Configuration to set bandsteering on VAP. */
	NSS_WIFI_VDEV_VOW_DBG_MODE_CMD,	/**< Configuration to set video over wireless (VOW) debug mode on VAP. */
	NSS_WIFI_VDEV_VOW_DBG_RST_STATS_CMD,
					/**< Configuration to reset video over wireless (VOW) debug mode on VAP. */
	NSS_WIFI_VDEV_CFG_DSCP_OVERRIDE_CMD,
					/**< Configuration to set DSCP/TID value override on VAP. */
	NSS_WIFI_VDEV_CFG_WNM_CAP_CMD,	/**< Configuration to set wireless network management (WNM) capability on VAP. */
	NSS_WIFI_VDEV_CFG_WNM_TFS_CMD,	/**< Configuration to set WNM traffic filtering and sleep mode (TFS) capability on VAP. */
	NSS_WIFI_VDEV_CFG_WDS_EXT_ENABLE_CMD,
						/**< Configuration to set WDS extention capability on VAP. */
	NSS_WIFI_VDEV_CFG_WDS_CMD,		/**< Configuration to set WDS on VAP. */
	NSS_WIFI_VDEV_CFG_AP_BRIDGE_CMD,        /**< Configuration to enable/disable client isolation. */
	NSS_WIFI_VDEV_SECURITY_TYPE_CMD,	/**< Configuration to set security type per VAP. */
	NSS_WIFI_VDEV_CFG_AST_OVERRIDE_CMD,	/**< Configuration to set AST (Address Search Table) override on VAP. */
	NSS_WIFI_VDEV_CFG_SON_CAP_CMD,		/**< Configuration to set software defined network capability on VAP. */
	NSS_WIFI_VDEV_CFG_MULTIPASS_CMD,	/**< Configuration to enable multipass phrase capability on VAP. */
	NSS_WIFI_VDEV_CFG_HLOS_TID_OVERRIDE_CMD,
					/**< Configuration to enable HLOS TID override on VAP. */
	NSS_WIFI_VDEV_ENABLE_IGMP_ME_CMD,	/**< Configuration to set IGMP multicast enhancement on VAP. */
	NSS_WIFI_VDEV_CFG_WDS_BACKHAUL_CMD,
					/**< Configuration to set WDS backhaul extension on VAP. */
	NSS_WIFI_VDEV_CFG_MCBC_EXC_TO_HOST_CMD, /**< Configuration to set multicast/broadcast exception to host on VAP. */
	NSS_WIFI_VDEV_CFG_PEER_AUTHORIZE_CMD,
					/**< Configuration to enable peer authorization on VAP. */
	NSS_WIFI_VDEV_MAX_CMD
};

/**
 * nss_wifi_vdev_dp_type
 *	Virtual device datapath types.
 */
enum nss_wifi_vdev_dp_type {
	NSS_WIFI_VDEV_DP_ACCELERATED,		/**< Wi-Fi accelerated VAP type. */
	NSS_WIFI_VDEV_DP_NON_ACCELERATED,	/**< Wi-Fi non-acclerated VAP type. */
	NSS_WIFI_VDEV_DP_TYPE_MAX		/**< Wi-Fi maximum VAP type. */
};

/**
 * nss_wifi_vdev_vlan_tagging_mode
 *	Supported VLAN tagging modes.
 */
enum nss_wifi_vdev_vlan_tagging_mode {
	NSS_WIFI_VDEV_VLAN_NONE,	/**< VLAN support disabled. */

	/**
	 * Default VLAN mode to add VLAN tag in Rx path and
	 * remove VLAN tag only when matching with configured
	 * VLAN tag in Tx path.
	 */
	NSS_WIFI_VDEV_VLAN_INGRESS_ADD_EGRESS_STRIP_ON_ID_MATCH,

	/**
	 * Port-based VLAN mode to add VLAN tag in Rx path
	 * and remove any VLAN tag in Tx path.
	 */
	NSS_WIFI_VDEV_VLAN_INGRESS_ADD_EGRESS_STRIP_ALWAYS,
	NSS_WIFI_VDEV_VLAN_MAX		/**< Wi-Fi maximum VLAN support type. */
};

enum vap_ext_mode {
	WIFI_VDEV_EXT_MODE_MESH_LINK = 1,	/* Wi-Fi mesh VAP mode */
	WIFI_VDEV_EXT_MODE_MAX,			/* Wi-Fi maximum VAP mode */
};

/**
 * nss_wifi_vdev_config_msg
 *	Virtual device configuration.
 */
struct nss_wifi_vdev_config_msg {
	uint8_t mac_addr[ETH_ALEN];	/**< MAC address. */
	uint16_t radio_ifnum;		/**< Corresponding radio interface number. */
	uint32_t vdev_id;		/**< Virtual device ID. */
	uint32_t epid;			/**< Endpoint ID of the copy engine. */
	uint32_t downloadlen;		/**< Size of the header download length. */
	uint32_t hdrcachelen;		/**< Size of the header cache. */
	uint32_t hdrcache[NSS_WIFI_HTT_TRANSFER_HDRSIZE_WORD];
					/**< Cached per descriptor metedata shared with NSS Firmware. */
	uint32_t opmode;		/**< VAP operating mode: Access-Point (AP) or Station (STA). */
	uint32_t mesh_mode_en;		/**< Mesh mode is enabled. */
	uint8_t is_mpsta;
					/**< Specifies whether the station is a VAP Master-Proxy (MP) station. */
	uint8_t is_psta;
					/**< Specifies whether the station is a proxy station. */
	uint8_t special_vap_mode;
					/**< Special VAP for monitoring received management packets. */
	uint8_t smartmesh_mode_en;
					/**< VAP is configured as a smart monitor VAP. */
	uint8_t is_wrap;		/**< Specifies whether the VAP is a WRAP-AP. */
	uint8_t is_nss_qwrap_en;	/**< VAP is configured for NSS firmware QWRAP logic. */
	uint8_t tx_per_pkt_vdev_id_check;	/**< Transmit per-packet virtual device ID check. */
	uint8_t align_pad;		/**< Reserved field. */
	uint32_t vap_ext_mode;		/**< Different VAP extended modes. */
};

/**
 * nss_wifi_vdev_enable_msg
 *	Enable a message for a virtual device.
 */
struct nss_wifi_vdev_enable_msg {
	uint8_t mac_addr[ETH_ALEN];	/**< MAC address. */
	uint8_t reserved[2];		/**< Reserved for 4-byte alignment padding. */
};

/**
 * nss_wifi_vdev_disable_msg
 *	Disable message for a virtual device.
 */
struct nss_wifi_vdev_disable_msg {
	uint32_t reserved;		/**< Placeholder for future enhancement. */
};

/**
 * nss_wifi_vdev_recovery_msg
 *	Recovery message for a virtual device.
 */
struct nss_wifi_vdev_recovery_msg {
	uint32_t reserved;		/**< Placeholder for future enhancement. */
};

/**
 * nss_wifi_vdev_set_next_hop_msg
 *	Set next hop for Wi-Fi virtual device.
 */
struct nss_wifi_vdev_set_next_hop_msg {
	uint32_t ifnumber;	/**< Next hop interface number. */
};

/**
 * nss_wifi_vdev_extap_map
 *	Wi-Fi EXTAP map for IPv4/IPv6 addresses.
 */
struct nss_wifi_vdev_extap_map {
	uint16_t ip_version;			/**< IPv4 or IPv6 address. */
	uint8_t h_dest[ETH_ALEN];		/**< MAC address of original backend. */
	union {
		uint8_t IPv4[4];		/**< IPv4 address of the backend. */
		uint8_t IPv6[NSS_WIFI_VDEV_IPV6_ADDR_LENGTH];	/**< IPv6 group IP address. */
	} u;
};

/**
 * nss_wifi_vdev_cmd_msg
 *	Virtual device commands.
 */
struct nss_wifi_vdev_cmd_msg {
	uint32_t cmd;			/**< Command type. */
	uint32_t value;			/**< Command value. */
};

/**
 * nss_wifi_vdev_me_snptbl_grp_create_msg
 *	Information for creating the snooptable group of a virtual device.
 */
struct nss_wifi_vdev_me_snptbl_grp_create_msg {
	uint32_t ether_type;		/**< Ether type of the multicast group. */

	/**
	 * IP address of a multicast group.
	 */
	union {
		uint32_t grpaddr_ip4;
				/**< IPv4 address. */
		uint8_t grpaddr_ip6[NSS_WIFI_VDEV_IPV6_ADDR_LENGTH];
				/**< IPv6 address. */
	} u;			/**< IP address of the multicast group. */

	uint8_t grp_addr[ETH_ALEN];
				/**< MAC address of the multicast group. */
};

/**
 * nss_wifi_vdev_me_snptbl_grp_delete_msg
 *	Information for deleting a snooplist group list.
 */
struct nss_wifi_vdev_me_snptbl_grp_delete_msg {
	uint32_t ether_type;	/**< Ether type of the multicast group. */

	/**
	 * IP address of the multicast group.
	 */
	union {
		uint32_t grpaddr_ip4;
				/**< IPv4 address. */
		uint8_t grpaddr_ip6[NSS_WIFI_VDEV_IPV6_ADDR_LENGTH];
				/**< IPv6 address. */
	} u;			/**< IP address of the multicast group. */

	uint8_t grp_addr[ETH_ALEN];	/**< MAC address of the multicast group. */
};

/**
 * struct nss_wifi_vdev_me_mbr_ra_info
 *	Address details of receiver members.
 */
struct nss_wifi_vdev_me_mbr_ra_info {
	bool dup;
					/**< Duplicate bit to identify if next hop address is present. */
	uint8_t ramac[ETH_ALEN];
					/**< MAC address of receiver. */
};

/**
 * nss_wifi_vdev_me_snptbl_grp_mbr_add_msg
 *	Information for adding a snooplist group member.
 */
struct nss_wifi_vdev_me_snptbl_grp_mbr_add_msg {
	uint32_t ether_type;	/**< Ether type of the multicast group. */

	/**
	 * IP address of the multicast group.
	 */
	union {
		uint32_t grpaddr_ip4;
				/**< IPv4 address. */
		uint8_t grpaddr_ip6[NSS_WIFI_VDEV_IPV6_ADDR_LENGTH];
				/**< IPv6 address. */
	} u;			/**< IP address of the multicast group. */

	uint32_t peer_id;	/**< Peer ID. */
	uint8_t grp_addr[ETH_ALEN];
				/**< MAC address of the multicast group. */
	uint8_t grp_member_addr[ETH_ALEN];
				/**< MAC address of the multicast group member. */
	uint8_t mode;		/**< Multicast enhancement mode - mode 2 and mode 5. */
	uint8_t nsrcs;		/**< Number of source IP addresses for selective source multicast. */
	uint8_t src_ip_addr[NSS_WIFI_VDEV_IPV6_ADDR_LENGTH * NSS_WIFI_MAX_SRCS];
				/**< Source IP address. */
	struct nss_wifi_vdev_me_mbr_ra_info ra_entry;
				/**< Receiver address entry corresponding to the member. */
};

/**
 * nss_wifi_vdev_me_snptbl_grp_mbr_delete_msg
 *	Information for removing a snooplist group member.
 */
struct nss_wifi_vdev_me_snptbl_grp_mbr_delete_msg {
	uint32_t ether_type;		/**< Ether type of the multicast group. */

	/**
	 * IP address of the multicast group.
	 */
	union {
		uint32_t grpaddr_ip4;
				/**< IPv4 address. */
		uint8_t grpaddr_ip6[NSS_WIFI_VDEV_IPV6_ADDR_LENGTH];
				/**< IPv6 address. */
	}u;			/**< IP address of the multicast group. */
	uint8_t grp_addr[ETH_ALEN];
				/**< MAC address of the multicast group. */
	uint8_t grp_member_addr[ETH_ALEN];
				/**< MAC address of the multicast group member. */
};

/**
 * nss_wifi_vdev_me_snptbl_grp_mbr_update_msg
 *	Information for updating a snooplist group member.
 */
struct nss_wifi_vdev_me_snptbl_grp_mbr_update_msg {
	uint32_t ether_type;	/**< Ether type of the multicast group. */

	/**
	 * IP address of the multicast group.
	 */
	union {
		uint32_t grpaddr_ip4;
				/**< IPv4 address. */
		uint8_t grpaddr_ip6[NSS_WIFI_VDEV_IPV6_ADDR_LENGTH];
				/**< IPv6 address. */
	}u;			/**< IP address of the multicast group. */

	uint8_t grp_addr[ETH_ALEN];
				/**< MAC address of the multicast group. */
	uint8_t grp_member_addr[ETH_ALEN];
				/**< MAC address of the multicast group member. */
	uint8_t mode;		/**< Multicast enhancement mode - mode 2 and mode 5. */
	uint8_t nsrcs;		/**< Number of source IP addresses for selective source multicast. */
	uint8_t src_ip_addr[NSS_WIFI_VDEV_IPV6_ADDR_LENGTH * NSS_WIFI_MAX_SRCS];
				/**< Source IP address. */
};

/**
 * nss_wifi_vdev_me_hmmc_add_msg
 *	Information for adding an entry into the host-managed multicast list.
 */
struct nss_wifi_vdev_me_hmmc_add_msg {
	uint32_t ether_type;	/**< IPv4 or IPv6. */
	union {
		uint32_t ipv4_addr;
			/**< IPv4 multicast group address. */
		uint8_t ipv6_addr[NSS_WIFI_VDEV_IPV6_ADDR_LENGTH];
			/**< IPv6 multicast group address. */
	} u;	/**< Type of group addresses. */
	uint32_t netmask;	/**< IP subnet netmask. */
};

/**
 * nss_wifi_vdev_me_hmmc_del_msg
 *	Information for deleting an entry from the host-managed multicast list.
 */
struct nss_wifi_vdev_me_hmmc_del_msg {
	uint32_t ether_type;	/**< IPv4 or IPv6. */
	union {
		uint32_t ipv4_addr;
			/**< IPv4 multicast group address. */
		uint8_t ipv6_addr[NSS_WIFI_VDEV_IPV6_ADDR_LENGTH];
			/**< IPv6 multicast group address. */
	} u;	/**< Type of group addresses. */
	uint32_t netmask;	/**< IP subnet netmask. */
};

/**
 * nss_wifi_vdev_me_deny_ip_add_msg
 *	Information for adding an entry into the deny list.
 */
struct nss_wifi_vdev_me_deny_ip_add_msg {
	uint32_t ether_type;	/**< IPv4 or IPv6. */
	union {
		uint32_t ipv4_addr;
			/**< IPv4 multicast group address. */
		uint8_t ipv6_addr[NSS_WIFI_VDEV_IPV6_ADDR_LENGTH];
			/**< IPv6 multicast group address. */
	} u;	/**< Type of group addresses. */
	uint32_t netmask;	/**< IP subnet netmask. */
};

/**
 * nss_wifi_vdev_me_deny_ip_del_msg
 *	Information for deleting an entry from the deny list.
 */
struct nss_wifi_vdev_me_deny_ip_del_msg {
	uint32_t ether_type;	/**< IPv4 or IPv6. */
	union {
		uint32_t ipv4_addr;
			/**< IPv4 multicast group address. */
		uint8_t ipv6_addr[NSS_WIFI_VDEV_IPV6_ADDR_LENGTH];
			/**< IPv6 multicast group address. */
	} u;	/**< Type of group addresses. */
	uint32_t netmask;	/**< IP subnet netmask. */
};

/**
 * nss_wifi_vdev_me_snptbl_deny_grp_add_msg
 *	Information for adding a snooplist member to a deny list.
 */
struct nss_wifi_vdev_me_snptbl_deny_grp_add_msg {
	uint32_t grpaddr;	/**< IP address of the multicast group. */
};

/**
 * nss_wifi_vdev_txmsg
 *	Information for transmitting special data.
 */
struct nss_wifi_vdev_txmsg {
	uint16_t peer_id;	/**< Peer ID. */
	uint16_t tid;		/**< Traffic ID. */
};

/**
 * nss_wifi_vdev_vow_dbg_stats
 *	Types of VoW debug statistics.
 */
struct nss_wifi_vdev_vow_dbg_stats {
	uint32_t rx_vow_dbg_counters;		/**< VoW Rx debug counter. */
	uint32_t tx_vow_dbg_counters[8];	/**< VoW Tx debug counter. */
};

/**
 * nss_wifi_vdev_vow_dbg_cfg_msg
 *	Information for configuring VoW debug statistics.
 */
struct nss_wifi_vdev_vow_dbg_cfg_msg {
	uint8_t vow_peer_list_idx;	/**< Index of the peer list. */
	uint8_t tx_dbg_vow_peer_mac4;	/**< MAC address 4 for the peer. */
	uint8_t tx_dbg_vow_peer_mac5;	/**< MAC address 5 for the peer. */
};

/**
 * nss_wifi_vdev_dscp_tid_map
 *	DSCP-to-TID mapping.
 */
struct nss_wifi_vdev_dscp_tid_map {
	uint32_t dscp_tid_map[NSS_WIFI_VDEV_DSCP_MAP_LEN];
		/**< Array holding the DSCP-to-TID mapping. */
};

/**
 * nss_wifi_vdev_dscptid_map_id
 *	DSCP-to-TID map ID.
 */
struct nss_wifi_vdev_dscptid_map_id {
	uint8_t dscp_tid_map_id;
		/**< DSCP-to-TID mapping ID to be used.  */
};

/**
 * nss_wifi_vdev_set_peer_next_hop
 *	Set per peer next hop.
 */
struct nss_wifi_vdev_set_peer_next_hop_msg {
	uint8_t peer_mac_addr[ETH_ALEN];   /**< MAC peer address. */
	uint16_t reserved;		   /**< Reserved. */
	uint32_t if_num;                   /**< Next hop interface number. */
};

/**
 * nss_wifi_vdev_qwrap_psta_msg
 *	PSTA VAP entry map in QWRAP mode.
 */
struct nss_wifi_vdev_qwrap_psta_msg {
	uint8_t oma[ETH_ALEN];	/**< Original MAC address of PSTA VAP. */
	uint8_t vma[ETH_ALEN];	/**< Virtual MAC address of PSTA VAP. */
	uint8_t vdev_id;	/**< ID of PSTA VAP.  */
	uint8_t is_wired;	/**< Is the entry for wired PSTA VAP.  */
	uint8_t reserved[2];	/**< Reserved for 4-byte alignment. */
};

/**
 * nss_wifi_vdev_qwrap_isolation_en_msg
 *	Qwrap isolation mode enable.
 */
struct nss_wifi_vdev_qwrap_isolation_en_msg {
	uint8_t isolation_enable;	/**< QWRAP isolation mode enable.  */
	uint8_t reserved[3];		/**< Reserved for 4-byte alignment. */
};

/**
 * nss_wifi_vdev_igmp_per_packet_metadata
 *	Per-packet metadata for IGMP packets.
 */
struct nss_wifi_vdev_igmp_per_packet_metadata {
	uint32_t tid;		/**< TID. */
	uint32_t tsf32;		/**< TSF value. */
	uint8_t peer_mac_addr[ETH_ALEN];
				/**< Peer MAC address. */
	uint8_t reserved[2];	/**< Reserved for 4-byte alignment. */
};

/**
 * nss_wifi_vdev_mesh_per_packet_metadata
 *	Per-packet metadata for Mesh packets.
 */
struct nss_wifi_vdev_mesh_per_packet_metadata {
	uint32_t status;	/**< Meshmode Status. */
	uint32_t rssi;		/**< Received signal strength indication. */
	uint32_t tsf;		/**< Tx expiry time. */
	uint16_t tx_retries;	/**< Retry count. */
};

/**
 * nss_wifi_vdev_vlan_config_msg
 * 	Enable special handling on this VAP where VLAN tagging is added in Rx and removed in Tx.
 */
struct nss_wifi_vdev_vlan_config_msg {
	uint16_t vlan_id;	/**< VLAN ID configured. */
	uint8_t reserved[2];	/**< Reserved for 4-byte alignment. */
};

/**
 * nss_wifi_vdev_vlan_enable_msg
 *	Enable VLAN tagging mode on this VAP.
 */
struct nss_wifi_vdev_vlan_enable_msg {
	uint8_t vlan_tagging_mode;	/**< Flag to enable default or port-based VLAN tagging mode. */
	uint8_t reserved[3];		/**< Reserved for 4-byte alignment. */
};

/**
 * nss_wifi_vdev_set_vlan_group_key
 *	Set VLAN ID for special peer.
 */
struct nss_wifi_vdev_set_vlan_group_key {
	uint16_t vlan_id;		/**< VLAN ID. */
	uint16_t group_key;		/**< Group key. */
};

/**
 * nss_wifi_vdev_txinfo_per_packet_metadata
 *	Per-packet metadata for Tx completion information packets.
 */
struct nss_wifi_vdev_txinfo_per_packet_metadata {
	uint32_t status;	/**< Tx completion status. */
	uint16_t msdu_count;	/**< Count of MSDUs in the MSDU list. */
	uint16_t num_msdu;	/**< Sequence Number of MSDU in the MSDU list. */
	uint32_t msdu_q_time;	/**< Time spent by an MSDU in the Wi-Fi firmware. */
	uint32_t ppdu_rate;	/**< PPDU rate in code rate. */
	uint8_t ppdu_num_mpdus_success;
				/**< Number of successful MPDUs. */
	uint8_t ppdu_num_mpdus_fail;
				/**< Number of failed MPDUs. */
	uint16_t ppdu_num_msdus_success;
				/**< Number of successful MSDUs. */
	uint32_t ppdu_bytes_success;
				/**< Number of successful bytes. */
	uint32_t ppdu_duration;	/**< Estimated air time. */
	uint8_t ppdu_retries;	/**< Number of times a PPDU is retried. */
	uint8_t ppdu_is_aggregate;
				/**< Flag to check whether a PPDU is aggregated. */
	uint16_t start_seq_num;	/**< Starting MSDU ID for this PPDU. */
	uint16_t version;	/**< PPDU statistics version. */
	uint32_t ppdu_ack_timestamp;
				/**< Timestamp (in ms) when an acknowledgement was received. */
	uint32_t ppdu_bmap_enqueued_lo;
				/**< Bitmap of packets enqueued to the hardware (LSB). */
	uint32_t ppdu_bmap_enqueued_hi;
				/**< Bitmap of packets enqueued to the hardware (MSB). */
	uint32_t ppdu_bmap_tried_lo;
				/**< Bitmap of packets sent over the air (LSB). */
	uint32_t ppdu_bmap_tried_hi;
				/**< Bitmap of packets sent over the air (MSB). */
	uint32_t ppdu_bmap_failed_lo;
				/**< Bitmap of packets that failed to be acknowledged (LSB). */
	uint32_t ppdu_bmap_failed_hi;
				/**< Bitmap of packets that failed to be acknowledged (MSB). */
};

/**
 * nss_wifi_vdev_qwrap_tx_metadata_types
 *	Per-packet metadata types for Qwrap Tx packets.
 */
enum nss_wifi_vdev_qwrap_tx_metadata_types {
	NSS_WIFI_VDEV_QWRAP_TYPE_NONE = 0,
	NSS_WIFI_VDEV_QWRAP_TYPE_TX = 1,
	NSS_WIFI_VDEV_QWRAP_TYPE_RX_TO_TX = 2
};

/**
 * nss_wifi_vdev_extap_pkt_types
 *	Per-packet metadata types for ExtAP Tx packets.
 */
enum nss_wifi_vdev_extap_pkt_types {
	NSS_WIFI_VDEV_EXTAP_PKT_TYPE_NONE = 0,
	NSS_WIFI_VDEV_EXTAP_PKT_TYPE_TX = 1,
	NSS_WIFI_VDEV_EXTAP_PKT_TYPE_RX_TO_TX = 2
};

/**
 * nss_wifi_vdev_mpsta_per_packet_tx_metadata
 *	Per-packet metadata for transmitting packets to an MP station.
 */
struct nss_wifi_vdev_mpsta_per_packet_tx_metadata {
	uint16_t vdev_id;	/**< Virtual device ID. */
	uint16_t metadata_type;	/**< Tx metadata type. */
};

/**
 * nss_wifi_vdev_mpsta_per_packet_rx_metadata
 *	Per-packet metadata for receiving packets from an MP station.
 */
struct nss_wifi_vdev_mpsta_per_packet_rx_metadata {
	uint16_t vdev_id;	/**< Virtual device ID. */
	uint16_t peer_id;	/**< Peer ID. */
};

/**
 * nss_wifi_vdev_rx_err_per_packet_metadata
 *	Per-packet metadata for error packets received.
 */
struct nss_wifi_vdev_rx_err_per_packet_metadata {
	uint8_t peer_mac_addr[ETH_ALEN];
				/**< Peer MAC address. */
	uint8_t tid;		/**< TID. */
	uint8_t vdev_id;	/**< Virtual device ID. */
	uint8_t err_type;	/**< Error type. */
	uint8_t rsvd[3];	/**< Reserved for future enhancement. */
};

/**
 * nss_wifi_vdev_extap_per_packet_metadata
 *	Per-packet metadata for ExtAP.
 */
struct nss_wifi_vdev_extap_per_packet_metadata {
	uint16_t pkt_type;	/**< ExtAP packet type. */
	uint8_t res[2];		/**< Reserved for 4-byte alignment. */
};

/**
 * nss_wifi_vdev_tx_compl_metadata
 *	Per-packet metadata for Tx completion message.
 */
struct nss_wifi_vdev_tx_compl_metadata {
	uint8_t ta[ETH_ALEN];	/**< Transmitter MAC address. */
	uint8_t ra[ETH_ALEN];	/**< Receiver MAC address. */
	uint16_t ppdu_id;	/**< PPDU ID. */
	uint16_t peer_id;	/**< Peer ID. */
};

/**
 * nss_wifi_vdev_wds_info_type
 *	Specifies the type of WDS notification information.
 */
enum wifi_vdev_ext_wds_info_type {
	NSS_WIFI_VDEV_WDS_TYPE_NONE = 0,
	NSS_WIFI_VDEV_WDS_TYPE_RX,	/**< Rx WDS entry. */
	NSS_WIFI_VDEV_WDS_TYPE_MEC,	/**< Multicast Tx WDS entry. */
	NSS_WIFI_VDEV_WDS_TYPE_DA	/**< Rx WDS entry for destination address. */
};

/**
 * nss_wifi_vdev_per_packet_metadata
 *	Payload of per-packet metadata.
 */
struct nss_wifi_vdev_wds_per_packet_metadata {
	uint16_t peer_id;	/**< Peer ID. */
	uint8_t is_sa_valid;	/**< Specifies whether source address is valid. */
	uint8_t reserved;	/**< Reserve bytes for alignment. */
	enum wifi_vdev_ext_wds_info_type wds_type;
				/**< WDS message type. */
	uint8_t addr4_valid;	/**< 802.11 4th address valid flag. */
	uint8_t rsvd;		/**< Reserve bytes for alignment. */
	uint16_t sa_idx;	/**< Source address index. */
	uint16_t sa_sw_peer_id;	/**< Software/Address-Search-Table peer ID. */
};

/**
 * nss_wifi_vdev_ppdu_mdata_dir
 * 	Physical layer protocol data unit (PPDU) metadata direction.
 */
enum nss_wifi_vdev_ppdu_mdata_dir {
	WIFI_VDEV_PPDU_MDATA_TX,	/**< PPDU metadata for transmit direction. */
	WIFI_VDEV_PPDU_MDATA_RX		/**< PPDU metadata for receive direction. */
};

/**
 * nss_wifi_vdev_ppdu_metadata
 * 	PPDU metadata.
 */
struct nss_wifi_vdev_ppdu_metadata {
	uint32_t dir;		/**< Data direction for metadata. */
	uint32_t ppdu_id;	/**< PPDU ID. */
	uint16_t peer_id;	/**< Peer ID. */
	uint8_t first_msdu;	/**< First MSDU. */
	uint8_t last_msdu;	/**< Last MSDU. */
};

/**
 * nss_wifi_vdev_per_packet_metadata
 *	Wi-Fi per packet metadata content.
 */
struct nss_wifi_vdev_per_packet_metadata {
	uint32_t pkt_type;	/**< Type of packet. */

	/**
	 * Metadata payload for special data receive messages.
	 */
	union {
		struct nss_wifi_vdev_igmp_per_packet_metadata igmp_metadata;
			/**< Per packet metadata structure for IGMP. */
		struct nss_wifi_vdev_mesh_per_packet_metadata mesh_metadata;
			/**< Per packet metadata structure for mesh mode. */
		struct nss_wifi_vdev_txinfo_per_packet_metadata txinfo_metadata;
			/**< Per packet metadata structure for Tx information. */
		struct nss_wifi_vdev_mpsta_per_packet_tx_metadata mpsta_tx_metadata;
			/**< Per packet Tx metadata structure for master-proxy station. */
		struct nss_wifi_vdev_mpsta_per_packet_rx_metadata mpsta_rx_metadata;
			/**< Per packet Rx metadata structure for master-proxy station. */
		struct nss_wifi_vdev_rx_err_per_packet_metadata rx_err_metadata;
			/**< Per packet metadata structure for Rx error. */
		struct nss_wifi_vdev_tx_compl_metadata tx_compl_metadata;
			/**< Per packet Tx metadata structure for Tx completion. */
		struct nss_wifi_vdev_wds_per_packet_metadata wds_metadata;
			/**< Per packet Tx metadata structure for wireless distribution system mode. */
		struct nss_wifi_vdev_ppdu_metadata ppdu_metadata;
			/**< Per packet PPDU metadata needed for per PPDU copy mode. */
	} metadata;
			/**< Metadata payload for special data receive message. */
};

/**
 * nss_wifi_vdev_meshmode_rx_metadata
 *	Metadata payload for Mesh mode receive.
 */
struct nss_wifi_vdev_meshmode_rx_metadata {
	uint16_t rs_ratephy_lo;	/**< PHY rate lower order bytes. */
	uint16_t rs_ratephy_hi;	/**< PHY rate higher order bytes. */
	uint16_t cntr_chan_freq;	/** Center channel frequency. */
	uint16_t vdev_id;	/**< Virtual device ID. */
	uint16_t peer_id;	/**< Peer ID. */
	uint16_t rs_rssi;	/**< Received signal strength indication (noise floor adjusted). */
	uint8_t rs_flags;	/**< First/last MSDU flags. */
	uint8_t rs_channel;	/**< Operational channel. */
	uint8_t rs_keyix;	/**< Key index. */
	uint8_t padd;		/**< Padding to ensure alignment. */
};

/**
 * nss_wifi_vdev_rawmode_rx_metadata
 *	Metadata payload for Raw Mode receive.
 */
struct nss_wifi_vdev_rawmode_rx_metadata {
	uint16_t vdev_id;	/**< Virtual device ID. */
	uint16_t peer_id;	/**< Peer ID. */
};

/**
 * nss_wifi_vdev_updchdr_msg
 *	Information for updating a cache header.
 */
struct nss_wifi_vdev_updchdr_msg {
	uint32_t hdrcache[NSS_WIFI_HTT_TRANSFER_HDRSIZE_WORD];
				/**< Updated header cache. */
	uint32_t vdev_id;	/**< Virtual device ID. */
};

/**
 * nss_wifi_vdev_me_host_sync_grp_entry
 *	Multicast enhancement host synchronization group table.
 */
struct nss_wifi_vdev_me_host_sync_grp_entry {
	uint8_t group_addr[ETH_ALEN];		/**< Group address for this list. */
	uint8_t grp_member_addr[ETH_ALEN];	/**< MAC address of the multicast group member. */

	/**
	 * Type of group addresses.
	 */
	union {
		uint32_t grpaddr_ip4;
			/**< IPv4 group address. */
		uint8_t  grpaddr_ip6[NSS_WIFI_VDEV_IPV6_ADDR_LENGTH];
			/**< IPv6 group address. */
	} u;	/**< Type of group addresses. */

	uint32_t src_ip_addr;
			/**< Source IP address. */
};

/**
 * wifi_vdev_me_host_sync_msg
 *	Synchronization message for a multicast enhancement host group.
 */
struct nss_wifi_vdev_me_host_sync_msg {
	uint16_t vdev_id;		/**< Virtual device ID. */
	uint8_t nentries;		/**< Number of group entries carried by this message. */
	uint8_t radio_ifnum;		/**< Interface number of the Wi-Fi radio. */
	struct nss_wifi_vdev_me_host_sync_grp_entry grp_entry[NSS_WIFI_VDEV_MAX_ME_ENTRIES];
					/**< Array for multicast group entries. */
};

/**
 * nss_wifi_vdev_mcast_enhance_stats
 *	Multicast enhancement-related statistics.
 */
struct nss_wifi_vdev_mcast_enhance_stats {

	/**
	 * Number of multicast packets recieved for multicast enhancement conversion.
	 */
	uint32_t mcast_rcvd;

	/**
	 * Number of unicast packets sent as part of multicast enhancement conversion.
	 */
	uint32_t mcast_ucast_converted;

	/**
	 * Number of multicast enhancement frames dropped because of a
	 * buffer allocation failure.
	 */
	uint32_t mcast_alloc_fail;

	/**
	 * Number of multicast enhancement frames dropped because of a
	 * buffer enqueue failure.
	 */
	uint32_t mcast_pbuf_enq_fail;

	/**
	 * Number of multicast enhancement frames dropped because of a
	 * buffer copy failure.
	 */
	uint32_t mcast_pbuf_copy_fail;

	/**
	 * Number of multicast enhancement frames dropped because of a
	 * failure in sending flow control to a peer.
	 */
	uint32_t mcast_peer_flow_ctrl_send_fail;

	/**
	 * Number of multicast enhancement buffer frames dropped when
	 * destination MAC is the same as source MAC.
	 */
	uint32_t mcast_loopback_err;

	/**
	 * Number of multicast enhancement buffer frames dropped
	 * because of an empty destination MAC.
	 */
	uint32_t mcast_dst_address_err;

	/**
	 * Number of multicast enhancement buffer frames dropped
	 * because no member is listening on the group.
	 */
	uint32_t mcast_no_enhance_drop_cnt;

	/**
	 * Number of multicast bytes received for multicast enhancement.
	 */
	uint32_t mcast_rcvd_bytes;

	/**
	 * Number of IGMP packets received for conversion to unicast.
	 */
	uint32_t igmp_rcvd;

	/**
	 * Number of IGMP packets converted to unicast as a part of
	 * VoW IGMP improvements.
	 */
	uint32_t igmp_ucast_converted;
};

/**
 * nss_wifi_vdev_stats_sync_msg
 *	Message to get virtual device statistics from NSS Firmware to Host.
 */
struct nss_wifi_vdev_stats_sync_msg {
	uint32_t dropped;			/**< Number of dropped packets. */
	uint32_t tx_enqueue_cnt;		/**< Transmit pnode enqueue count. */
	uint32_t tx_enqueue_fail_cnt;		/**< Transmit pnode enqueue count. */
	uint32_t tx_intra_bss_enqueue_cnt;	/**< Intra BSS enqueue count. */
	uint32_t tx_intra_bss_enqueue_fail_cnt;
						/**< Intra BSS enqueue fail count. */
	uint32_t tx_intra_bss_mcast_send_cnt;
						/**< Virual device multicast/broadcast packet count in AP mode. */
	uint32_t tx_intra_bss_mcast_send_fail_cnt;
						/**< Virtual device multicast/broadcast packet count in AP mode. */
	uint32_t tx_enqueue_bytes;		/**< Transmit enqueue bytes count. */
	uint32_t rx_enqueue_cnt;		/**< Ethernet node enqueue count. */
	uint32_t rx_enqueue_fail_cnt;		/**< Ethernet node enqueue fail count. */
	uint32_t rx_except_enqueue_cnt;		/**< N2H (NSS to Host) node enqueue count. */
	uint32_t rx_except_enqueue_fail_cnt;	/**< N2H (NSS to Host) node enqueue fail count. */
	uint32_t rx_enqueue_bytes;		/**< Receive enqueue bytes count. */
	uint32_t rx_wds_learn_send_cnt;		/**< Virtual device WDS source port learn count. */
	uint32_t rx_wds_learn_send_fail_cnt;	/**< Virtual device WDS source count fail. */
	struct nss_wifi_vdev_mcast_enhance_stats wvmes;
						/**< Multicast enhancement statistics. */
	uint32_t num_tx_exception;		/**< Number of Tx exception to firmware. */
	uint32_t tx_dma_map_fail;		/**< DMA map failure. */
	uint32_t tx_desc_alloc_fail;		/**< Descriptor allocation failure. */
	uint32_t tx_hw_ring_full;		/**< Hardware ring is full. */
	uint32_t tx_tso_pkt;			/**< Number of TSO packets. */
	uint32_t tx_num_seg;			/**< Number of segments in TSO packets. */
	uint32_t tx_rcvd;			/**< Number of packets received from host. */
	uint32_t tx_rcvd_bytes;			/**< Number of bytes received from host. */
	uint32_t cce_classified;
			/**< Number of packets that are classified and sent to firmware as an exception. */
	uint32_t cce_classified_raw;
			/**< Number of raw packets that are classified and sent to firmware as an exception. */
	uint32_t tx_eapol_cnt;			/**< Number of EAPoL frames in transmit direction. */
	uint32_t nawds_tx_mcast_cnt;		/**< Number of NAWDS packets sent. */
	uint32_t nawds_tx_mcast_bytes;		/**< Number of NAWDS bytes sent. */
	uint32_t per_pkt_vdev_check_fail;	/**< Number of packets that failed vdev id check in Tx. */
	uint32_t rx_mcast_cnt;			/**< Receive multicast packet count. */
	uint32_t rx_mcast_bytes;		/**< Receive multicast bytes count. */
	uint32_t rx_decrypt_err;		/**< Receive decryption error */
	uint32_t rx_mic_err;			/**< Receive MIC error */
	uint32_t mcbc_exc_host_fail_cnt;
			/**< Number of multicast/broadcast packets failed to send to host through exception path. */
};

/**
 * nss_wifi_vdev_msg
 *	Data for sending and receiving virtual device specific messages.
 */
struct nss_wifi_vdev_msg {
	struct nss_cmn_msg cm;		/**< Common message header. */

	/**
	 * Payload of a virtual device specific message.
	 */
	union {
		struct nss_wifi_vdev_config_msg vdev_config;
				/**< Virtual device configuration. */
		struct nss_wifi_vdev_enable_msg vdev_enable;
				/**< Enable a message for a virtual device. */
		struct nss_wifi_vdev_cmd_msg vdev_cmd;
				/**< Command message for a virtual device. */
		struct nss_wifi_vdev_me_snptbl_grp_create_msg vdev_grp_list_create;
				/**< Creates the snooptable group of a virtual device. */
		struct nss_wifi_vdev_me_snptbl_grp_delete_msg vdev_grp_list_delete;
				/**< Deletes a snooplist group list. */
		struct nss_wifi_vdev_me_snptbl_grp_mbr_add_msg vdev_grp_member_add;
				/**< Adds a snooplist group member. */
		struct nss_wifi_vdev_me_snptbl_grp_mbr_delete_msg vdev_grp_member_remove;
				/**< Removes a snooplist group member. */
		struct nss_wifi_vdev_me_snptbl_grp_mbr_update_msg vdev_grp_member_update;
				/**< Updates a snooplist group member. */
		struct nss_wifi_vdev_me_snptbl_deny_grp_add_msg vdev_deny_member_add;
				/**< Add a snooplist member to the deny list. */
		struct nss_wifi_vdev_me_hmmc_add_msg vdev_hmmc_member_add;
				/**< Adds a new member into the HMMC list. */
		struct nss_wifi_vdev_me_hmmc_del_msg vdev_hmmc_member_del;
				/**< Delete a member from the HMMC list. */
		struct nss_wifi_vdev_me_deny_ip_add_msg vdev_deny_list_member_add;
				/**< Adds a new member into the deny list. */
		struct nss_wifi_vdev_me_deny_ip_del_msg vdev_deny_list_member_del;
				/**< Delete a member from the deny list. */
		struct nss_wifi_vdev_txmsg vdev_txmsgext;
				/**< Transmits special data. */
		struct nss_wifi_vdev_vow_dbg_cfg_msg vdev_vow_dbg_cfg;
				/**< Configures VoW debug statistics. */
		struct nss_wifi_vdev_vow_dbg_stats vdev_vow_dbg_stats;
				/**< Types of VoW debug statistics. */
		struct nss_wifi_vdev_dscp_tid_map vdev_dscp_tid_map;
				/**< DSCP-to-TID mapping. */
		struct nss_wifi_vdev_updchdr_msg vdev_updchdr;
				/**< Updates a cache header. */
		struct nss_wifi_vdev_me_host_sync_msg vdev_me_sync;
				/**< Message for a multicast enhancement host group table synchronization. */
		struct nss_wifi_vdev_stats_sync_msg vdev_stats;
				/**< Message to get virtual device statistics from NSS firmware to host. */
		struct nss_wifi_vdev_set_next_hop_msg next_hop;
				/**< Next hop message for virtual device. */
		struct nss_wifi_vdev_dscptid_map_id vdev_dscp_tid_map_id;
				/**< Message to get DSCP-to-TID mapping id to be used on virtual device. */
		struct nss_wifi_vdev_extap_map vdev_extap_map;
				/**< Message to add entry in EXTAP table on virtual device. */
		struct nss_wifi_vdev_qwrap_psta_msg vdev_qwrap_psta_map;
				/**< Message to get PSTA VAP details in QWRAP mode. */
		struct nss_wifi_vdev_qwrap_isolation_en_msg vdev_qwrap_isolation_en;
				/**< Message to enable QWRAP isolation mode. */
		struct nss_wifi_vdev_set_peer_next_hop_msg vdev_set_peer_next_hp;
				/**< Message to set next hop per peer. */
		struct nss_wifi_vdev_vlan_config_msg vdev_vlan_config;
				/**< Message to set VLAN configured on a particular virtual device. */
		struct nss_wifi_vdev_vlan_enable_msg vdev_vlan_enable;
				/**< Message to enable VLAN tagging support on a particular virtual device. */
		struct nss_wifi_vdev_set_vlan_group_key vlan_group_key;
				/**< Message to set group key for peer. */
	} msg;		/**< Virtual device message payload. */
};

/**
 * nss_wifi_vdev_tx_msg
 *	Sends a Wi-Fi message to the NSS interface.
 *
 * @datatypes
 * nss_ctx_instance \n
 * nss_wifi_vdev_msg
 *
 * @param[in] nss_ctx  Pointer to the NSS core context.
 * @param[in] msg      Pointer to the message data.
 *
 * @return
 * Status of the Tx operation.
 */
nss_tx_status_t nss_wifi_vdev_tx_msg(struct nss_ctx_instance *nss_ctx,
				struct nss_wifi_vdev_msg *msg);

/**
 * nss_wifi_vdev_base_tx_msg
 *	Sends a Wi-Fi message to the NSS VAP interface.
 *
 * @datatypes
 * nss_ctx_instance \n
 * nss_wifi_vdev_msg
 *
 * @param[in] nss_ctx  Pointer to the NSS core context.
 * @param[in] msg      Pointer to the message data.
 *
 * @return
 * Status of the Tx operation.
 */
nss_tx_status_t nss_wifi_vdev_base_tx_msg(struct nss_ctx_instance *nss_ctx,
				struct nss_wifi_vdev_msg *msg);

/**
 * nss_wifi_vdev_tx_buf
 *	Sends a Wi-Fi data packet to the NSS interface.
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
 * Status of the Tx operation.
 */
nss_tx_status_t nss_wifi_vdev_tx_buf(struct nss_ctx_instance *nss_ctx,
				struct sk_buff *os_buf, uint32_t if_num);

/**
 * Callback function for receiving Wi-Fi virtual device messages.
 *
 * @datatypes
 * nss_cmn_msg
 *
 * @param[in] app_data  Pointer to the application context of the message.
 * @param[in] msg       Pointer to the message data.
 */
typedef void (*nss_wifi_vdev_msg_callback_t)(void *app_data,
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
typedef void (*nss_wifi_vdev_callback_t)(struct net_device *netdev,
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
typedef void (*nss_wifi_vdev_ext_data_callback_t)(struct net_device *netdev,
				struct sk_buff *skb, struct napi_struct *napi);

/**
 * nss_wifi_vdev_msg_init
 *	Initializes a Wi-Fi virtual device message.
 *
 * @datatypes
 * nss_wifi_vdev_msg \n
 * nss_wifi_vdev_msg_callback_t
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
void nss_wifi_vdev_msg_init(struct nss_wifi_vdev_msg *nim, uint32_t if_num, uint32_t type, uint32_t len,
				nss_wifi_vdev_msg_callback_t *cb, void *app_data);

/**
 * nss_register_wifi_vdev_if
 *	Registers a Wi-Fi virtual device interface with the NSS interface.
 *
 * @datatypes
 * nss_ctx_instance \n
 * nss_wifi_vdev_callback_t \n
 * nss_wifi_vdev_ext_data_callback_t \n
 * nss_wifi_vdev_msg_callback_t \n
 * net_device
 *
 * @param[in,out] nss_ctx                 Pointer to the NSS core context.
 * @param[in]     if_num                  NSS interface number.
 * @param[in]     wifi_data_callback      Callback for the Wi-Fi virtual device data.
 * @param[in]     vdev_ext_data_callback  Callback for the extended data.
 * @param[in]     wifi_event_callback     Callback for the message.
 * @param[in]     netdev                  Pointer to the associated network device.
 * @param[in]     features                Data socket buffer types supported by this
 *                                        interface.
 *
 * @return
 * None.
 */
uint32_t nss_register_wifi_vdev_if(struct nss_ctx_instance *nss_ctx, int32_t if_num, nss_wifi_vdev_callback_t wifi_data_callback,
			nss_wifi_vdev_ext_data_callback_t vdev_ext_data_callback, nss_wifi_vdev_msg_callback_t wifi_event_callback,
			struct net_device *netdev, uint32_t features);

/**
 * nss_unregister_wifi_vdev_if
 *	Deregisters a Wi-Fi virtual device interface from the NSS interface.
 *
 * @param[in] if_num  NSS interface number.
 *
 * @return
 * None.
 */
void nss_unregister_wifi_vdev_if(uint32_t if_num);

/**
 * nss_wifi_vdev_tx_msg_ext
 *	Sends Wi-Fi data packet along with metadata as message to the NSS.
 *
 * @datatypes
 * nss_ctx_instance \n
 * sk_buff
 *
 * @param[in,out] nss_ctx  Pointer to the NSS core context.
 * @param[in]     os_buf   Pointer to the OS data buffer.
 *
 * @return
 * Status of the Tx operation.
 */
nss_tx_status_t nss_wifi_vdev_tx_msg_ext(struct nss_ctx_instance *nss_ctx, struct sk_buff *os_buf);

/**
 * nss_wifi_vdev_set_next_hop
 *	Send next hop message to Wi-Fi virtual device.
 *
 * @datatypes
 * nss_ctx_instance
 *
 * @param[in]    nss_ctx  Pointer to the NSS core context.
 * @param[in]    if_num   NSS interface number.
 * @param[in]    next_hop Next hop interface number.
 *
 * @return
 * Status of the Tx operation.
 */
nss_tx_status_t nss_wifi_vdev_set_next_hop(struct nss_ctx_instance *nss_ctx, int if_num, int next_hop);

/**
 * nss_wifi_vdev_base_set_next_hop
 *	Sends the next hop message to Wi-Fi virtual access point.
 *
 * @datatypes
 * nss_ctx_instance
 *
 * @param[in]    nss_ctx  Pointer to the NSS core context.
 * @param[in]    next_hop Next hop interface number.
 *
 * @return
 * Status of the Tx operation.
 */
nss_tx_status_t nss_wifi_vdev_base_set_next_hop(struct nss_ctx_instance *nss_ctx, int next_hop);

/**
 * nss_wifi_vdev_set_peer_next_hop
 *	Sends the peer next hop message to Wi-Fi virtual device.
 *
 * @datatypes
 * nss_ctx_instance
 *
 * @param[in]    nss_ctx      Pointer to the NSS core context.
 * @param[in]    nss_if       NSS interface number.
 * @param[in]    addr         Peer MAC address.
 * @param[in]    next_hop_if  Next hop interface number.
 *
 * @return
 * Status of the Tx operation.
 */
nss_tx_status_t nss_wifi_vdev_set_peer_next_hop(struct nss_ctx_instance *nss_ctx, uint32_t nss_if, uint8_t *addr, uint32_t next_hop_if);

/*
 * nss_wifi_vdev_set_dp_type
 *	Sets the datapath type for virtual device.
 *
 * @datatypes
 * nss_ctx_instance \n
 * net_device \n
 * uint32_t \n
 * enum nss_wifi_vdev_dp_type
 *
 * @param[in]   nss_ctx  Pointer to the NSS core context.
 * @param[in]   netdev   Pointer to the associated network device.
 * @param[in]   if_num   Interface number of the VAP.
 * @param[in]   dp_type  Datapath type of the VAP.
 *
 * @return
 * True if a success, or false if a failure.
 */
bool nss_wifi_vdev_set_dp_type(struct nss_ctx_instance *nss_ctx, struct net_device *netdev,
						uint32_t if_num, enum nss_wifi_vdev_dp_type dp_type);
/**
 * @}
 */

#endif /* __NSS_WIFI_VDEV_H */

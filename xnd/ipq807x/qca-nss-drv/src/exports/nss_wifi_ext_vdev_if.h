/*
 **************************************************************************
 * Copyright (c) 2020, The Linux Foundation. All rights reserved.
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
 * @file nss_wifi_ext_vdev_if.h
 *	NSS Wi-Fi extended virtual device interface definitions.
 */

#ifndef __NSS_WIFI_EXT_VDEV_IF_H
#define __NSS_WIFI_EXT_VDEV_IF_H

#define NSS_WIFI_EXT_VDEV_MAX 16

/*
 * nss_wifi_ext_vdev_msg_types
 *	WiFi extension virtual device mesage types.
 */
enum nss_wifi_ext_vdev_msg_types {
	NSS_WIFI_EXT_VDEV_MSG_CONFIGURE_IF = NSS_IF_MAX_MSG_TYPES + 1,
	NSS_WIFI_EXT_VDEV_MSG_CONFIGURE_WDS,
	NSS_WIFI_EXT_VDEV_SET_NEXT_HOP,
	NSS_WIFI_EXT_VDEV_MSG_STATS_SYNC,
	NSS_WIFI_EXT_VDEV_MSG_CONFIGURE_VLAN,
	NSS_WIFI_EXT_VDEV_MSG_MAX
};

/**
 * nss_wifi_ext_vdev_error_tyes
 *	WiFi extension error types.
 */
enum nss_wifi_ext_vdev_error_types {
	NSS_WIFI_EXT_VDEV_ERROR_NONE = NSS_IF_ERROR_TYPE_MAX + 1,
						/** Configuration successful. */
	NSS_WIFI_EXT_VDEV_ERROR_NULL_MAC,		/**< NULL MAC received. */
	NSS_WIFI_EXT_VDEV_ERROR_INV_RADIO_ID,	/**< Invalid radio interface number. */
	NSS_WIFI_EXT_VDEV_ERROR_INV_PVAP_ID,	/**< Invalid parent virtual device interface number. */
	NSS_WIFI_EXT_VDEV_ERROR_RADIO_NOT_PRESENT,	/**< Radio node is not present. */
	NSS_WIFI_EXT_VDEV_ERROR_INV_IF,		/**< Message sent on invalid interface number. */
	NSS_WIFI_EXT_VDEV_ERROR_INV_VLAN_ID,	/**< Invalid VLAN ID. */
	NSS_WIFI_EXT_VDEV_ERROR_INV_CMD,	/**< Invalid command. */
	NSS_WIFI_EXT_VDEV_ERROR_PEERID_ALREADY_CONFIGURED,
						/**< Peer ID is already configured. */
	NSS_WIFI_EXT_VDEV_ERROR_MAX		/**< Maxiumum error types. */
};

/**
 * nss_wifi_ext_vdev_wds_msg
 *	Extended WDS configuration message.
 */
struct nss_wifi_ext_vdev_wds_msg {
	uint16_t wds_peer_id;	/**< WDS station peer ID. */
	uint16_t mac_addr[3];	/**< Remote MAC address. */
};

/**
 * nss_wifi_ext_vdev_stats
 *	Statistics message structure.
 */
struct nss_wifi_ext_vdev_stats {
	struct nss_cmn_node_stats node_stats;	/**< Ethernet node statistics. */
	uint32_t mc_count;			/**< Number of mulitcast counts. */
	uint32_t nxt_hop_drp;			/**< Next hop drop. */
};

/**
 * nss_wifi_vdev_config_msg
 *	NSS Wi-Fi virtual device configuration message.
 */
struct nss_wifi_ext_vdev_configure_if_msg {
	uint8_t mac_addr[ETH_ALEN];	/**< MAC address. */
	uint16_t radio_ifnum;		/**< Radio interface corresponding to virtual AP. */
	uint16_t pvap_ifnum;		/**< Parent virtual device interface number. */
};

/**
 * nss_wifi_ext_vdev_set_next_hop_msg
 *	Message to set the next hop.
 */
struct nss_wifi_ext_vdev_set_next_hop_msg {
	uint32_t if_num;		/**< Interface number. */
};

/**
 * nss_wifi_ext_vdev_vlan_msg
 *	Extended VLAN configuration message.
 */
struct nss_wifi_ext_vdev_vlan_msg {
	uint16_t vlan_id;	/**< VLAN ID. */
};

/**
 * nss_wifi_ext_vdev_msg
 *	Message structure to Send/Receive commands.
 */
struct nss_wifi_ext_vdev_msg {
	struct nss_cmn_msg cm;				/**< Cnode message. */
	union {
		union nss_if_msgs if_msg; /**< NSS interface base message. */
		struct nss_wifi_ext_vdev_configure_if_msg cmsg;	/**< Interface configuration message. */
		struct nss_wifi_ext_vdev_wds_msg wmsg;	/**< WDS configure message. */
		struct nss_wifi_ext_vdev_set_next_hop_msg wnhm;	/**< Next hop set message. */
		struct nss_wifi_ext_vdev_stats stats;	/**< Statistics messasge. */
		struct nss_wifi_ext_vdev_vlan_msg vmsg;	/**< VLAN message. */
	} msg;
};

/**
 * Callback function for receiving Wi-Fi extended virtual device data.
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
typedef void (*nss_wifi_ext_vdev_data_callback_t)(struct net_device *netdev, struct sk_buff *skb, struct napi_struct *napi);

/**
 * Callback function for receiving Wi-Fi extended virtual device messages.
 *
 * @datatypes
 * nss_wifi_ext_vdev_msg
 *
 * @param[in] app_data  Pointer to the application context of the message.
 * @param[in] wevm      Pointer to the message data.
 */
typedef void (*nss_wifi_ext_vdev_msg_callback_t)(void *app_data, struct nss_cmn_msg *ncm);

/**
 * Callback function for receiving extended data from the Wi-Fi extended virtual device interface.
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
typedef void (*nss_wifi_ext_vdev_ext_data_callback_t)(struct net_device *netdev,
				struct sk_buff *skb, struct napi_struct *napi);
/**
 * nss_wifi_ext_vdev_msg_init
 *	Initializes a Wi-Fi extended virtual device message.
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
void nss_wifi_ext_vdev_msg_init(struct nss_wifi_ext_vdev_msg *nim, uint32_t if_num, uint32_t type, uint32_t len,
				nss_wifi_ext_vdev_msg_callback_t cb, void *app_data);

/**
 * nss_wifi_ext_vdev_unregister_if
 *      Deregisters a Wi-Fi extended virtual interface from the NSS.
 *
 * @param[in] if_num  Wi-Fi extended virtual interface number.
 *
 * @return
 * None.
 *
 * @dependencies
 * The Wi-Fi extended virtual interface must have been previously registered.
 *
 * @return
 * True if successful, else false.
 */
extern bool nss_wifi_ext_vdev_unregister_if(uint32_t if_num);

/**
 * nss_wifi_ext_vdev_tx_buf
 *      Sends data buffers to NSS firmware asynchronously.
 *
 * @datatypes
 * nss_ctx_instance \n
 * sk_buff
 *
 * @param[in] nss_ctx  Pointer to the NSS context.
 * @param[in] os_buf   Pointer to the OS buffer (e.g. skbuff).
 * @param[in] if_num   Wi-Fi extended virtual interface number.
 *
 * @return
 * Status of the transmit operation.
 */
extern nss_tx_status_t nss_wifi_ext_vdev_tx_buf(struct nss_ctx_instance *nss_ctx, struct sk_buff *os_buf,
		uint32_t if_num);

/**
 * nss_wifi_ext_vdev_tx_msg
 *	Sends Wi-Fi extended virtual interface messages.
 *
 * @datatypes
 * nss_ctx_instance \n
 * nss_wifi_ext_vdev_msg
 *
 * @param[in] nss_ctx  Pointer to the NSS context.
 * @param[in] wevm      Pointer to the message data.
 *
 * @return
 * Status of the transmit operation.
 */
extern nss_tx_status_t nss_wifi_ext_vdev_tx_msg(struct nss_ctx_instance *nss_ctx, struct nss_wifi_ext_vdev_msg *wevm);

/**
 * nss_wifi_ext_vdev_tx_msg_sync
 *	Sends messages to NSS firmware synchronously.
 *
 * @datatypes
 * nss_ctx_instance \n
 * nss_wifi_ext_vdev_msg
 *
 * @param[in] nss_ctx   NSS core context.
 * @param[in] nwevm     Pointer to Wi-Fi extended virtual interface message data.
 *
 * @return
 * Status of the transmit operation.
 */
extern nss_tx_status_t nss_wifi_ext_vdev_tx_msg_sync(struct nss_ctx_instance *nss_ctx, struct nss_wifi_ext_vdev_msg *nwevm);

/**
 * nss_wifi_ext_vdev_set_next_hop
 *	Sets the extended virtual interface next hop.
 *
 * @datatypes
 * nss_ctx_instance \n
 *
 * @param[in] ctx      NSS core context.
 * @param[in] if_num   NSS interface number.
 * @param[in] next_hop Next hop interface number.
 */
extern nss_tx_status_t nss_wifi_ext_vdev_set_next_hop(struct nss_ctx_instance *ctx, int if_num, int next_hop);

/**
 * nss_wifi_ext_vdev_get_ctx
 *	Gets the NSS Wi-Fi extended virtual interface context.
 *
 * @return
 * Pointer to the NSS core context.
 */
extern struct nss_ctx_instance *nss_wifi_ext_vdev_get_ctx(void);

/**
 * nss_wifi_ext_vdev_register_if
 *      Registers Wi-Fi extended virtual interface with NSS.
 *
 * @datatypes
 * net_device \n
 * nss_wifi_ext_vdev_data_callback_t \n
 * nss_wifi_ext_vdev_ext_data_callback_t \n
 * nss_wifi_ext_vdev_msg_callback_t
 *
 * @param[in] if_num         NSS interface number.
 * @param[in] cb_func_data   Callback for the data.
 * @param[in] cb_func_ext    Callback for the message.
 * @param[in] cb_func_msg    Callback for the event message.
 * @param[in] features       Data socket buffer types supported by this interface.
 * @param[in] netdev         Pointer to the associated network device.
 * @param[in] app_ctx        Pointer to the application context.
 *
 * @return
 * NSS interface number allocated.
 */
extern struct nss_ctx_instance *nss_wifi_ext_vdev_register_if(uint32_t if_num,
		nss_wifi_ext_vdev_data_callback_t cb_func_data, nss_wifi_ext_vdev_ext_data_callback_t cb_func_ext,
		nss_wifi_ext_vdev_msg_callback_t cb_func_msg, struct net_device *netdev, uint32_t features,
		void *app_ctx);
#endif

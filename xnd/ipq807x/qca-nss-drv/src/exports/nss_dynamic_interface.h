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

/*
 * @file nss_dynamic_interface.h
 *	NSS Dynamic interface definitions.
 */

#ifndef __NSS_DYNAMIC_INTERFACE_H
#define __NSS_DYNAMIC_INTERFACE_H

/**
 * @addtogroup nss_dynamic_interface_subsystem
 * @{
 */

#define NSS_MAX_DYNAMIC_INTERFACES 128	/**< Maximum number of dynamic interfaces. */

/**
 * nss_dynamic_interface_type
 *	Dynamic interface types.
 *
 * @note
 * Every time a new dynamic interface type is added to an enumeration in the following list,
 * a corresponding type name string should be added in the dynamic interface type string array.
 */
enum nss_dynamic_interface_type {
	NSS_DYNAMIC_INTERFACE_TYPE_NONE,
	NSS_DYNAMIC_INTERFACE_TYPE_GRE_REDIR,
	NSS_DYNAMIC_INTERFACE_TYPE_RESERVED_5,
	NSS_DYNAMIC_INTERFACE_TYPE_TUNIPIP6_INNER,
	NSS_DYNAMIC_INTERFACE_TYPE_TUNIPIP6_OUTER,
	NSS_DYNAMIC_INTERFACE_TYPE_RESERVED,
	NSS_DYNAMIC_INTERFACE_TYPE_VAP,
	NSS_DYNAMIC_INTERFACE_TYPE_RESERVED_0,
	NSS_DYNAMIC_INTERFACE_TYPE_PPPOE,
	NSS_DYNAMIC_INTERFACE_TYPE_VIRTIF_DEPRECATED,
	NSS_DYNAMIC_INTERFACE_TYPE_L2TPV2,
	NSS_DYNAMIC_INTERFACE_TYPE_RESERVED_4,
	NSS_DYNAMIC_INTERFACE_TYPE_PORTID,
	NSS_DYNAMIC_INTERFACE_TYPE_DTLS,
	NSS_DYNAMIC_INTERFACE_TYPE_QVPN_INNER,
	NSS_DYNAMIC_INTERFACE_TYPE_QVPN_OUTER,
	NSS_DYNAMIC_INTERFACE_TYPE_BRIDGE,
	NSS_DYNAMIC_INTERFACE_TYPE_VLAN,
	NSS_DYNAMIC_INTERFACE_TYPE_RESERVED_3,
	NSS_DYNAMIC_INTERFACE_TYPE_WIFILI_INTERNAL,
	NSS_DYNAMIC_INTERFACE_TYPE_MAP_T_INNER,
	NSS_DYNAMIC_INTERFACE_TYPE_MAP_T_OUTER,
	NSS_DYNAMIC_INTERFACE_TYPE_GRE_TUNNEL_INNER,
	NSS_DYNAMIC_INTERFACE_TYPE_GRE_TUNNEL_OUTER,
	NSS_DYNAMIC_INTERFACE_TYPE_DTLS_CMN_INNER,
	NSS_DYNAMIC_INTERFACE_TYPE_DTLS_CMN_OUTER,
	NSS_DYNAMIC_INTERFACE_TYPE_GRE_REDIR_WIFI_HOST_INNER,
	NSS_DYNAMIC_INTERFACE_TYPE_GRE_REDIR_WIFI_OFFL_INNER,
	NSS_DYNAMIC_INTERFACE_TYPE_GRE_REDIR_SJACK_INNER,
	NSS_DYNAMIC_INTERFACE_TYPE_GRE_REDIR_OUTER,
	NSS_DYNAMIC_INTERFACE_TYPE_GRE_TUNNEL_INLINE_INNER,
	NSS_DYNAMIC_INTERFACE_TYPE_GRE_TUNNEL_INLINE_OUTER,
	NSS_DYNAMIC_INTERFACE_TYPE_GENERIC_REDIR_N2H,
	NSS_DYNAMIC_INTERFACE_TYPE_GENERIC_REDIR_H2N,
	NSS_DYNAMIC_INTERFACE_TYPE_TUN6RD_INNER,
	NSS_DYNAMIC_INTERFACE_TYPE_TUN6RD_OUTER,
	NSS_DYNAMIC_INTERFACE_TYPE_GRE_REDIR_LAG_US,
	NSS_DYNAMIC_INTERFACE_TYPE_GRE_REDIR_LAG_DS,
	NSS_DYNAMIC_INTERFACE_TYPE_GRE_TUNNEL_INNER_EXCEPTION,
	NSS_DYNAMIC_INTERFACE_TYPE_GRE_REDIR_EXCEPTION_US,
	NSS_DYNAMIC_INTERFACE_TYPE_GRE_REDIR_EXCEPTION_DS,
	NSS_DYNAMIC_INTERFACE_TYPE_GRE_INNER,
	NSS_DYNAMIC_INTERFACE_TYPE_GRE_OUTER,
	NSS_DYNAMIC_INTERFACE_TYPE_PPTP_INNER,
	NSS_DYNAMIC_INTERFACE_TYPE_PPTP_OUTER,
	NSS_DYNAMIC_INTERFACE_TYPE_PPTP_HOST_INNER,
	NSS_DYNAMIC_INTERFACE_TYPE_IPSEC_CMN_INNER,
	NSS_DYNAMIC_INTERFACE_TYPE_IPSEC_CMN_MDATA_INNER,
	NSS_DYNAMIC_INTERFACE_TYPE_IPSEC_CMN_OUTER,
	NSS_DYNAMIC_INTERFACE_TYPE_IPSEC_CMN_MDATA_OUTER,
	NSS_DYNAMIC_INTERFACE_TYPE_IPSEC_CMN_REDIRECT,
	NSS_DYNAMIC_INTERFACE_TYPE_PVXLAN_HOST_INNER,
	NSS_DYNAMIC_INTERFACE_TYPE_PVXLAN_OUTER,
	NSS_DYNAMIC_INTERFACE_TYPE_IGS,
	NSS_DYNAMIC_INTERFACE_TYPE_CLMAP_US,
	NSS_DYNAMIC_INTERFACE_TYPE_CLMAP_DS,
	NSS_DYNAMIC_INTERFACE_TYPE_VXLAN_INNER,
	NSS_DYNAMIC_INTERFACE_TYPE_VXLAN_OUTER,
	NSS_DYNAMIC_INTERFACE_TYPE_MATCH,
	NSS_DYNAMIC_INTERFACE_TYPE_RMNET_RX_N2H,
	NSS_DYNAMIC_INTERFACE_TYPE_RMNET_RX_H2N,
	NSS_DYNAMIC_INTERFACE_TYPE_WIFILI_EXTERNAL0,
	NSS_DYNAMIC_INTERFACE_TYPE_WIFILI_EXTERNAL1,
	NSS_DYNAMIC_INTERFACE_TYPE_TLS_INNER,
	NSS_DYNAMIC_INTERFACE_TYPE_TLS_OUTER,
	NSS_DYNAMIC_INTERFACE_TYPE_MIRROR,
	NSS_DYNAMIC_INTERFACE_TYPE_WIFI_EXT_VDEV_WDS,
	NSS_DYNAMIC_INTERFACE_TYPE_CAPWAP_HOST_INNER,
	NSS_DYNAMIC_INTERFACE_TYPE_CAPWAP_OUTER,
	NSS_DYNAMIC_INTERFACE_TYPE_WIFI_EXT_VDEV_VLAN,
	NSS_DYNAMIC_INTERFACE_TYPE_WIFI_MESH_INNER,
	NSS_DYNAMIC_INTERFACE_TYPE_WIFI_MESH_OUTER,
	NSS_DYNAMIC_INTERFACE_TYPE_MAX
};

typedef enum nss_dynamic_interface_type nss_dynamic_interface_assigned;

/**
 * nss_dynamic_interface_message_types
 *	Message types for dynamic interface requests.
 */
enum nss_dynamic_interface_message_types {
	NSS_DYNAMIC_INTERFACE_ALLOC_NODE,
	NSS_DYNAMIC_INTERFACE_DEALLOC_NODE,
	NSS_DYNAMIC_INTERFACE_MAX,
};

/**
 * nss_dynamic_interface_error_types
 *	Error types for dynamic interface requests.
 */
enum nss_dynamic_interface_error_types {
	NSS_DYNAMIC_INTERFACE_ERR_EUNKNOWN = 1,
	NSS_DYNAMIC_INTERFACE_ERR_EUNAVAIL,
	NSS_DYNAMIC_INTERFACE_ERR_INVALID_TYPE,
	NSS_DYNAMIC_INTERFACE_ERR_INVALID_INTERFACE_NUM,
	NSS_DYNAMIC_INTERFACE_ERR_ALLOC_FUNC_UNAVAILABLE,
	NSS_DYNAMIC_INTERFACE_ERR_DEALLOC_FUNC_UNAVAILABLE,
	NSS_DYNAMIC_INTERFACE_ERR_EALLOC,
	NSS_DYNAMIC_INTERFACE_ERR_IFNUM_TYPE_MISMATCH,
	NSS_DYNAMIC_INTERFACE_ERR_MAX,
};

/**
 * nss_dynamic_interface_stats_notification
 *	Dynamic interface statistics structure.
 */
struct nss_dynamic_interface_notification {
	uint32_t core_id;	/**< Core ID. */
	uint32_t if_num;	/**< Dynamic interface number. */
};

#ifdef __KERNEL__ /* only kernel will use. */
/**
 * nss_dynamic_interface_alloc_node_msg
 *	Message information for a dynamic interface allocation node.
 */
struct nss_dynamic_interface_alloc_node_msg {
	enum nss_dynamic_interface_type type;	/**< Type of dynamic interface. */

	/*
	 * Response.
	 */
	int if_num;				/**< Dynamic interface number. */
};

/**
 * nss_dynamic_interface_dealloc_node_msg
 *	Message information for dynamic interface deallocation node.
 */
struct nss_dynamic_interface_dealloc_node_msg {
	enum nss_dynamic_interface_type type;
			/**< Type of dynamic interface. */
	int if_num;	/**< Dynamic interface number. */
};

/**
 * nss_dynamic_interface_msg
 *	Data for sending and receiving dynamic interface messages.
 */
struct nss_dynamic_interface_msg {
	struct nss_cmn_msg cm;		/**< Common message header. */

	/**
	 * Payload of a dynamic interface message.
	 */
	union {
		struct nss_dynamic_interface_alloc_node_msg alloc_node;
				/**< Allocates a dynamic node. */
		struct nss_dynamic_interface_dealloc_node_msg dealloc_node;
				/**< Deallocates a dynamic node. */
	} msg;			/**< Message payload. */
};

/**
 * nss_dynamic_interface_alloc_node
 *	Allocates a node for a dynamic interface.
 *
 * @datatypes
 * nss_dynamic_interface_type
 *
 * @param[in] type  Type of dynamic interface.
 *
 * @return
 * Number for the dynamic interface created.
 * @par
 * Otherwise, -1 for a failure.
 */
extern int nss_dynamic_interface_alloc_node(enum nss_dynamic_interface_type type);

/**
 * nss_dynamic_interface_dealloc_node
 *	Deallocates a node created for a dynamic interface on the NSS.
 *
 * @datatypes
 * nss_dynamic_interface_type
 *
 * @param[in] if_num  Dynamic interface number.
 * @param[in] type    Type of dynamic interface.
 *
 * @return
 * Status of the Tx operation.
 */
extern nss_tx_status_t nss_dynamic_interface_dealloc_node(int if_num, enum nss_dynamic_interface_type type);

/**
 * nss_is_dynamic_interface
 *	Specifies whether the interface number belongs to the dynamic interface.
 *
 * @param[in] if_num  Dynamic interface number.
 *
 * @return
 * TRUE or FALSE
 */
extern bool nss_is_dynamic_interface(int if_num);

/**
 * nss_dynamic_interface_get_nss_ctx_by_type
 *	Returns NSS context corresponding to the dynamic interface type.
 *
 * @datatypes
 * nss_dynamic_interface_type
 *
 * @param[in] type  Type of dynamic interface.
 *
 * @return
 * Pointer to the NSS context.
 */
extern struct nss_ctx_instance *nss_dynamic_interface_get_nss_ctx_by_type(enum nss_dynamic_interface_type type);

/**
 * nss_dynamic_interface_get_type
 *	Returns the type of dynamic interface.
 *
 * @param[in] nss_ctx  Pointer to the NSS context.
 * @param[in] if_num   Interface number of dynamic interface.
 *
 * @return
 * Type of dynamic interface per the dynamic interface number.
 */
extern enum nss_dynamic_interface_type nss_dynamic_interface_get_type(struct nss_ctx_instance *nss_ctx, int if_num);

/**
 * nss_dynamic_interface_tx
 *	Transmits an asynchronous message to the firmware.
 *
 * @datatypes
 * nss_ctx_instance \n
 * nss_dynamic_interface_msg
 *
 * @param[in] nss_ctx  Pointer to the NSS context.
 * @param[in] msg      Pointer to the message data.
 *
 * @return
 * Status of the transmit operation.
 */
extern nss_tx_status_t nss_dynamic_interface_tx(struct nss_ctx_instance *nss_ctx, struct nss_dynamic_interface_msg *msg);

/**
 * Callback function for dynamic interface messages.
 *
 * @datatypes
 * nss_cmn_msg
 *
 * @param[in] app_data  Pointer to the application context of the message.
 * @param[in] msg       Pointer to the message data.
 */
typedef void (*nss_dynamic_interface_msg_callback_t)(void *app_data, struct nss_cmn_msg *msg);

/**
 * nss_dynamic_interface_msg_init
 *	Initializes a dynamic interface message.
 *
 * @datatypes
 * nss_dynamic_interface_msg
 *
 * @param[in] ndm       Pointer to the dynamic interface message.
 * @param[in] if_num    Dynamic interface number.
 * @param[in] type      Type of message.
 * @param[in] len       Size of the payload.
 * @param[in] cb        Pointer to the message callback.
 * @param[in] app_data  Pointer to the application context that is passed to the callback function.
 *
 * @return
 * None.
 */
void nss_dynamic_interface_msg_init(struct nss_dynamic_interface_msg *ndm, uint16_t if_num, uint32_t type, uint32_t len,
						void *cb, void *app_data);

/**
 * nss_dynamic_interface_stats_register_notifier
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
extern int nss_dynamic_interface_stats_register_notifier(struct notifier_block *nb);

/**
 * nss_dynamic_interface_stats_unregister_notifier
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
extern int nss_dynamic_interface_stats_unregister_notifier(struct notifier_block *nb);
#endif /*__KERNEL__ */

/**
 * @}
 */

#endif /* __NSS_DYNAMIC_INTERFACE_H*/

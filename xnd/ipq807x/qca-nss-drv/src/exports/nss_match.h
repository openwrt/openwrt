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
 * @file nss_match.h
 *	NSS match interface definitions.
 */

#ifndef _NSS_MATCH_H_
#define _NSS_MATCH_H_


/**
 * @addtogroup nss_match_subsystem
 * @{
 */

/**
 * Maximum number of supported match instances.
 */
#define NSS_MATCH_INSTANCE_MAX 4

/**
 * Maximum number of rules supported per instance.
 */
#define NSS_MATCH_INSTANCE_RULE_MAX 32

/**
 * Maximum number of masksets.
 */
#define NSS_MATCH_MASK_MAX 2

/**
 * Maximum number of words per maskset.
 */
#define NSS_MATCH_MASK_WORDS_MAX 4

/**
 * nss_match_error_type
 *	Match message error types.
 */
typedef enum nss_match_error_type {
	NSS_MATCH_SUCCESS,			/**< No error. */
	NSS_MATCH_ERROR_UNKNOWN_MSG,		/**< Message unknown. */
	NSS_MATCH_ERROR_DSCP_OUTOFBOUND,	/**< DSCP out of bound. */
	NSS_MATCH_ERROR_OUTER_8021P_OUTOFBOUND,	/**< Outer 802.1p out of bound. */
	NSS_MATCH_ERROR_INNER_8021P_OUTOFBOUND,	/**< Inner 802.1p out of bound. */
	NSS_MATCH_ERROR_RULE_ID_OUTOFBOUND,	/**< Rule ID is out of bound. */
	NSS_MATCH_ERROR_ACTION_TYPE_INVALID,	/**< Invalid action type. */
	NSS_MATCH_ERROR_RULE_EXISTS,		/**< Rule ID already in use. */
	NSS_MATCH_ERROR_RULE_DOES_NOT_EXIST,	/**< Rule does not exist. */
	NSS_MATCH_ERROR_INSTANCE_CONFIGURED,	/**< Error in instance configuration. */
	NSS_MATCH_ERROR_PROFILE_CONFIG_INVALID,	/**< Invalid profile configuration message. */
	NSS_MATCH_ERROR_DB_INIT_FAILED,		/**< Database initialization failed. */
	NSS_MATCH_ERROR_TABLE_ID_OUTOFBOUND,	/**< Table ID is out of bound. */
	NSS_MATCH_ERROR_RULE_ADD,		/**< Error in adding rule. */
	NSS_MATCH_ERROR_RULE_DELETE,		/**< Error in deleting rule. */
	NSS_MATCH_ERROR_TABLE_ADD,		/**< Error in adding table. */
	NSS_MATCH_ERROR_TABLE_DELETE,		/**< Error in deleting table. */
	NSS_MATCH_ERROR_MASK_ID_OUTOFBOUND,	/**< Mask ID is out of bound. */
	NSS_MATCH_ERROR_IFNUM_INVALID,		/**< Next node interface number is invalid. */
	NSS_MATCH_ERROR_MAX,			/**< Maximum error. */
} nss_match_status_t;

/**
 * nss_match_action_type
 * 	Bit positions for possible actions that can be taken.
 */
enum nss_match_action_type {
	NSS_MATCH_ACTION_NONE,		/**< No action. */
	NSS_MATCH_ACTION_SETPRIO = 1,	/**< Set given priority to the packet. */
	NSS_MATCH_ACTION_FORWARD = 2,	/**< Forward the packet to a given node. */
	NSS_MATCH_ACTION_DROP = 4,	/**< Drop the packet. */
	NSS_MATCH_ACTION_MAX,		/**< Maximum action type. */
};

/**
 * nss_match_profile_type
 *	Different profile types.
 */
enum nss_match_profile_type {
	NSS_MATCH_PROFILE_TYPE_NONE,
	NSS_MATCH_PROFILE_TYPE_VOW,		/**< Matches on interface number/DSCP/802.1P. */
	NSS_MATCH_PROFILE_TYPE_L2,		/**< Matches on interface number/destination MAC/source MAC/Ether type. */
	NSS_MATCH_PROFILE_TYPE_MAX,		/**< Maximum profile type. */
};

/**
 * nss_match_msg_types.
 *	Message types for match requests and responses.
 */
enum nss_match_msg_types {
	NSS_MATCH_INSTANCE_NONE,	/**< Invalid message. */
	NSS_MATCH_TABLE_CONFIGURE_MSG,	/**< Instance configure. */
	NSS_MATCH_ADD_VOW_RULE_MSG,	/**< Insert VoW rule. */
	NSS_MATCH_ADD_L2_RULE_MSG,	/**< Insert l2 rule. */
	NSS_MATCH_DELETE_VOW_RULE_MSG,	/**< Delete VoW rule. */
	NSS_MATCH_DELETE_L2_RULE_MSG,	/**< Delete l2 rule. */
	NSS_MATCH_STATS_SYNC,		/**< Instance statistics. */
	NSS_MATCH_MSG_MAX,		/**< Maximum instance messages. */
};

/**
 * nss_match_stats_sync
 *	Synchronization message structure.
 */
struct nss_match_stats_sync {
	struct nss_cmn_node_stats p_stats;			/**< Pnode statistics. */
	uint32_t hit_count[NSS_MATCH_INSTANCE_RULE_MAX];	/**< Exception events. */
};

/**
 * nss_match_profile_configure_msg
 *	Message for configuring the profile for a match instance.
 */
struct nss_match_profile_configure_msg {
	uint32_t profile_type;				/**< Profile type. */
	uint32_t valid_mask_flag;			/**< Valid maskset flag. */
	uint32_t maskset[NSS_MATCH_MASK_MAX][NSS_MATCH_MASK_WORDS_MAX];	/**< Maskset. */
};

/*
 * nss_match_rule_action
 *	Message information for action.
 */
struct nss_match_rule_action {
	uint32_t action_flag;		/**< Action bit map. */
	uint32_t forward_ifnum;	/**< Next node interface number. */
	uint16_t setprio;	/**< Priority number to be set in packet. */
	uint16_t reserved;	/**< Reserved 2 bytes. */
};

/**
 * nss_match_rule_vow_msg
 *	Rule message for VoW profile.
 */
struct nss_match_rule_vow_msg {
	uint16_t rule_id;	/**< Rule ID for the rule. */
	uint16_t mask_id;       /**< Mask number used for the rule. */
	struct nss_match_rule_action action;	/**< Action related with the rule. */
	uint32_t if_num;	/**< Interface number. */
	uint8_t dscp;		/**< DSCP. */
	uint8_t outer_8021p;	/**< Outer 802.1p. */
	uint8_t inner_8021p;	/**< Inner 802.1p. */
	uint8_t reserved;	/**< Reserved byte. */
};

/**
 * nss_match_rule_l2_msg
 *	Rule message for L2 profile.
 */
struct nss_match_rule_l2_msg {
	uint16_t rule_id;	/**< Rule ID for the rule. */
	uint16_t mask_id;       /**< Mask number used for the rule. */
	struct nss_match_rule_action action;	/**< Action related with the rule. */
	uint32_t if_num;	/**< Interface number. */
	uint16_t dmac[3];	/**< Destination MAC address. */
	uint16_t smac[3];	/**< Source MAC address. */
	uint16_t ethertype;	/**< Ethernet type. */
};

/**
 * nss_match_msg
 *	Data for sending and receiving match messages.
 */
struct nss_match_msg {
	struct nss_cmn_msg cm;	/**< Message header. */

	/**
	 * Payload of a match message.
	 */
	union {
		struct nss_match_profile_configure_msg configure_msg;	/**< Configure message. */
		struct nss_match_rule_vow_msg vow_rule;		/**< Insertion or deletion message for VoW profile. */
		struct nss_match_rule_l2_msg l2_rule;		/**< Insertion or deletion message for l2 profile. */
		struct nss_match_stats_sync stats;		/**< Instance synchronization statistics. */
	} msg;	/**< Message payload. */
};

/**
 * nss_match_msg_tx_sync
 *	Sends proxy match messages to the NSS.
 *
 * Do not call this function from a softirq or interrupt because it
 * might sleep if the NSS firmware is busy serving another host thread.
 *
 * @datatypes
 * nss_ctx_instance \n
 * nss_match_msg
 *
 * @param[in] nss_ctx  Pointer to the NSS context.
 * @param[in] msg      Pointer to the message data.
 *
 * @return
 * Status of the Tx operation.
 */
nss_tx_status_t nss_match_msg_tx_sync(struct nss_ctx_instance *nss_ctx, struct nss_match_msg *msg);

/**
 * Callback function for receiving match messages.
 *
 * @datatypes
 * nss_match_msg
 *
 * @param[in] app_data  Pointer to the application context of the message.
 * @param[in] msg       Pointer to the message data.
 */
typedef void (*nss_match_msg_sync_callback_t)(void *app_data, struct nss_match_msg *msg);

/**
 * nss_match_get_context
 * 	Returns NSS context of match.
 *
 * @datatypes
 * nss_ctx_instance
 *
 * @return
 * Pointer to the NSS core context.
 */
extern struct nss_ctx_instance *nss_match_get_context(void);

/**
 * nss_match_register_instance
 * 	Registers match dynamic node to NSS.
 *
 * @datatypes
 * nss_match_msg_sync_callback_t
 *
 * @param[in] if_num     Interface number of match instance.
 * @param[in] notify_cb  Notify callback function for the message.
 *
 * @return
 * Pointer to the NSS core context.
 */
extern struct nss_ctx_instance *nss_match_register_instance(int if_num, nss_match_msg_sync_callback_t notify_cb);

/**
 * nss_match_unregister_instance
 * 	Unregisters match dynamic node to NSS.
 *
 * @param[in] if_num  Interface number of match instance.
 *
 * @return
 * True if successful, else false.
 */
extern bool nss_match_unregister_instance(int if_num);

/**
 * nss_match_msg_init
 * 	Initializes a match message.
 *
 * @datatypes
 * nss_match_msg \n
 * nss_match_msg_sync_callback_t
 *
 * @param[in,out] nmm       Pointer to the message.
 * @param[in]     if_num    NSS interface number.
 * @param[in]     type      Type of message.
 * @param[in]     len       Size of the payload.
 * @param[in]     cb        Callback function for the message.
 * @param[in]     app_data  Pointer to the application context of the message.
 *
 * @return
 * None.
 */
extern void nss_match_msg_init(struct nss_match_msg *nmm, uint16_t if_num, uint32_t type, uint32_t len,
		nss_match_msg_sync_callback_t cb, void *app_data);

/**
 * nss_match_init
 * 	Initializes match.
 *
 * @return
 * None.
 */
extern void nss_match_init(void);
/**
 * @}
 */

#endif /* _NSS_MATCH_H_ */

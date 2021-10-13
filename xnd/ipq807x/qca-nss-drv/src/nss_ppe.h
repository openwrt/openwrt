/*
 **************************************************************************
 * Copyright (c) 2016-2018, The Linux Foundation. All rights reserved.
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
 * nss_ppe.h
 *      NSS PPE header file
 */

#include <net/sock.h>
#include "nss_tx_rx_common.h"

#define PPE_BASE_ADDR			0x3a000000
#define PPE_REG_SIZE			0x1000000

#define PPE_L3_DBG_WR_OFFSET		0x200c04
#define PPE_L3_DBG_RD_OFFSET		0x200c0c
#define PPE_L3_DBG0_OFFSET		0x10001
#define PPE_L3_DBG1_OFFSET		0x10002
#define PPE_L3_DBG2_OFFSET		0x10003
#define PPE_L3_DBG3_OFFSET		0x10004
#define PPE_L3_DBG4_OFFSET		0x10005
#define PPE_L3_DBG_PORT_OFFSET		0x11e80

#define PPE_PKT_CODE_WR_OFFSET		0x100080
#define PPE_PKT_CODE_RD_OFFSET		0x100084
#define PPE_PKT_CODE_DROP0_OFFSET	0xf000000
#define PPE_PKT_CODE_DROP1_OFFSET	0x10000000
#define PPE_PKT_CODE_CPU_OFFSET		0x40000000

#define PPE_PKT_CODE_DROP0_GET(x)	(((x) & 0xe0000000) >> 29)
#define PPE_PKT_CODE_DROP1_GET(x)	(((x) & 0x7) << 3)
#define PPE_PKT_CODE_DROP_GET(d0, d1)	(PPE_PKT_CODE_DROP0_GET(d0) | PPE_PKT_CODE_DROP1_GET(d1))

#define PPE_PKT_CODE_CPU_GET(x)		(((x) >> 3) & 0xff)

#define PPE_IPE_PC_REG 0x100000

/*
 * NSS_SYS_REG_DROP_CPU_CNT_TBL
 * 	Address map and access APIs for DROP_CPU_CNT table.
 */
#define PPE_DROP_CPU_CNT_TBL_OFFSET 0x60000
#define PPE_DROP_CPU_CNT_TBL_ENTRY_SIZE 0x10
#define PPE_DROP_CPU_CNT_TBL_BASE_OFFSET (PPE_IPE_PC_REG + PPE_DROP_CPU_CNT_TBL_OFFSET)
#define PPE_CPU_CODE_MAX_NUM 256

/*
 * CPU code offset
 */
#define PPE_CPU_CODE_OFFSET(n) (PPE_DROP_CPU_CNT_TBL_BASE_OFFSET + ((n) * PPE_DROP_CPU_CNT_TBL_ENTRY_SIZE))

/*
 * DROP code offset
 */
#define PPE_DROP_CODE_IDX(code, src_port) (PPE_CPU_CODE_MAX_NUM + (8 * (code)) + (src_port))
#define PPE_DROP_CODE_OFFSET(code, src_port) (PPE_DROP_CPU_CNT_TBL_BASE_OFFSET + ((PPE_DROP_CODE_IDX(code, src_port)) * PPE_DROP_CPU_CNT_TBL_ENTRY_SIZE))

#define NSS_PPE_TX_TIMEOUT 1000 /* 1 Second */

/*
 * Maximum number of VSI
 */
#define NSS_PPE_VSI_NUM_MAX 32

/*
 * ppe nss debug stats lock
 */
extern spinlock_t nss_ppe_stats_lock;

/*
 * Private data structure
 */
struct nss_ppe_pvt {
	void * __iomem ppe_base;
	struct semaphore sem;
	struct completion complete;
	int response;
	void *cb;
	void *app_data;
};

/*
 * Data structure to store to PPE private context
 */
extern struct nss_ppe_pvt ppe_pvt;

/**
 * nss_ppe_message_types
 *	Message types for Packet Processing Engine (PPE) requests and responses.
 *
 * Note: PPE messages are added as short term approach, expect all
 * messages below to be deprecated for more integrated approach.
 */
enum nss_ppe_message_types {
	NSS_PPE_MSG_SYNC_STATS,
	NSS_PPE_MSG_IPSEC_PORT_CONFIG,
	NSS_PPE_MSG_IPSEC_PORT_MTU_CHANGE,
	NSS_PPE_MSG_IPSEC_ADD_INTF,
	NSS_PPE_MSG_IPSEC_DEL_INTF,
	NSS_PPE_MSG_MAX,
};

/**
 * nss_ppe_msg_error_type
 *	PPE error types.
 */
enum nss_ppe_msg_error_type {
	PPE_MSG_ERROR_OK,
	PPE_MSG_ERROR_UNKNOWN_TYPE,
	PPE_MSG_ERROR_PORT_CREATION_FAIL,
	PPE_MSG_ERROR_INVALID_PORT_VSI,
	PPE_MSG_ERROR_INVALID_L3_IF,
	PPE_MSG_ERROR_IPSEC_PORT_CONFIG,
	PPE_MSG_ERROR_IPSEC_INTF_TABLE_FULL,
	PPE_MSG_ERROR_IPSEC_INTF_ATTACHED,
	PPE_MSG_ERROR_IPSEC_INTF_UNATTACHED,
	PPE_ERROR_MAX
};

/**
 * nss_ppe_stats_sc
 *	Message structure for per service code stats.
 */
struct nss_ppe_stats_sc {
	uint32_t nss_ppe_sc_cb_unregister;	/* Per service-code counter for callback not registered */
	uint32_t nss_ppe_sc_cb_success;		/* Per service-code coutner for successful callback */
	uint32_t nss_ppe_sc_cb_failure;		/* Per service-code counter for failure callback */
};

/**
 * nss_ppe_stats
 *	Message structure for ppe general stats
 */
struct nss_ppe_stats {
	uint32_t nss_ppe_v4_l3_flows;		/**< Number of IPv4 routed flows. */
	uint32_t nss_ppe_v4_l2_flows;		/**< Number of IPv4 bridge flows. */
	uint32_t nss_ppe_v4_create_req;		/**< Number of IPv4 create requests. */
	uint32_t nss_ppe_v4_create_fail;	/**< Number of IPv4 create failures. */
	uint32_t nss_ppe_v4_destroy_req;	/**< Number of IPv4 delete requests. */
	uint32_t nss_ppe_v4_destroy_fail;	/**< Number of IPv4 delete failures. */
	uint32_t nss_ppe_v4_mc_create_req;	/**< Number of IPv4 MC create requests. */
	uint32_t nss_ppe_v4_mc_create_fail;	/**< Number of IPv4 MC create failure. */
	uint32_t nss_ppe_v4_mc_update_req;	/**< Number of IPv4 MC update requests. */
	uint32_t nss_ppe_v4_mc_update_fail;	/**< Number of IPv4 MC update failure. */
	uint32_t nss_ppe_v4_mc_destroy_req;	/**< Number of IPv4 MC delete requests. */
	uint32_t nss_ppe_v4_mc_destroy_fail;	/**< Number of IPv4 MC delete failure. */
	uint32_t nss_ppe_v4_unknown_interface;	/**< Number of IPv4 create failures */

	uint32_t nss_ppe_v6_l3_flows;		/**< Number of IPv6 routed flows. */
	uint32_t nss_ppe_v6_l2_flows;		/**< Number of IPv6 bridge flows. */
	uint32_t nss_ppe_v6_create_req;		/**< Number of IPv6 create requests. */
	uint32_t nss_ppe_v6_create_fail;	/**< Number of IPv6 create failures. */
	uint32_t nss_ppe_v6_destroy_req;	/**< Number of IPv6 delete requests. */
	uint32_t nss_ppe_v6_destroy_fail;	/**< Number of IPv6 delete failures. */
	uint32_t nss_ppe_v6_mc_create_req;	/**< Number of IPv6 MC create requests. */
	uint32_t nss_ppe_v6_mc_create_fail;	/**< Number of IPv6 MC create failure. */
	uint32_t nss_ppe_v6_mc_update_req;	/**< Number of IPv6 MC update requests. */
	uint32_t nss_ppe_v6_mc_update_fail;	/**< Number of IPv6 MC update failure. */
	uint32_t nss_ppe_v6_mc_destroy_req;	/**< Number of IPv6 MC delete requests. */
	uint32_t nss_ppe_v6_mc_destroy_fail;	/**< Number of IPv6 MC delete failure. */
	uint32_t nss_ppe_v6_unknown_interface;	/**< Number of IPv6 create failures */

	uint32_t nss_ppe_fail_vp_full;
			/**< Request failed because the virtual port table is full */
	uint32_t nss_ppe_fail_nh_full;
			/**< Request failed because the next hop table is full. */
	uint32_t nss_ppe_fail_flow_full;
			/**< Request failed because the flow table is full. */
	uint32_t nss_ppe_fail_host_full;
			/**< Request failed because the host table is full. */
	uint32_t nss_ppe_fail_pubip_full;
			/**< Request failed because the public IP table is full. */
	uint32_t nss_ppe_fail_port_setup;
			/**< Request failed because the PPE port is not setup. */
	uint32_t nss_ppe_fail_rw_fifo_full;
			/**< Request failed because the read/write FIFO is full. */
	uint32_t nss_ppe_fail_flow_command;
			/**< Request failed because the PPE flow command failed. */
	uint32_t nss_ppe_fail_unknown_proto;
			/**< Request failed because of an unknown protocol. */
	uint32_t nss_ppe_fail_ppe_unresponsive;
			/**< Request failed because the PPE is not responding. */
	uint32_t nss_ppe_ce_opaque_invalid;
			/**< Request failed because of invalid opaque in connection entry. */
	uint32_t nss_ppe_fail_fqg_full;
			/**< Request failed because the flow QoS group is full. */
};


/**
 * nss_ppe_sync_stats_msg
 *	Message information for PPE synchronization statistics.
 */
struct nss_ppe_sync_stats_msg {
	struct nss_ppe_stats stats;			/**< General stats */
	struct nss_ppe_stats_sc sc_stats[NSS_PPE_SC_MAX];
							/**< Per service-code stats */
};

/**
 * nss_ppe_ipsec_port_config_msg
 *	Message structure for inline IPsec port configuration.
 */
struct nss_ppe_ipsec_port_config_msg {
	uint32_t nss_ifnum;		/**< NSS interface number corresponding to inline IPsec port. */
	uint16_t mtu;			/**< MTU value for inline IPsec port. */
	uint8_t vsi_num;		/**< Default port VSI for inline IPsec port. */
};

/**
 * nss_ppe_ipsec_port_mtu_msg
 *	Message structure for inline IPsec port MTU change.
 */
struct nss_ppe_ipsec_port_mtu_msg {
	uint32_t nss_ifnum;		/**< NSS interface number corresponding to inline IPsec port. */
	uint16_t mtu;			/**< MTU value for inline IPsec port. */
};

/**
 * nss_ppe_ipsec_add_intf_msg
 *	Message structure for adding dynamic IPsec/DTLS interface to inline IPsec port.
 */
struct nss_ppe_ipsec_add_intf_msg {
	uint32_t nss_ifnum;	/**< Dynamic IPsec/DTLS interface number. */
};

/**
 * nss_ppe_ipsec_del_intf_msg
 *	Message structure for deleting dynamic IPsec/DTLS interface to inline IPsec port.
 */
struct nss_ppe_ipsec_del_intf_msg {
	uint32_t nss_ifnum;	/**< Dynamic IPsec/DTLS interface number. */
};

/**
 * nss_ppe_msg
 *	Data for sending and receiving PPE host-to-NSS messages.
 */
struct nss_ppe_msg {
	struct nss_cmn_msg cm;		/**< Common message header. */

	/**
	 * Payload of a PPE host-to-NSS message.
	 */
	union {
		struct nss_ppe_sync_stats_msg stats;
				/**< Synchronization statistics. */
		struct nss_ppe_ipsec_port_config_msg ipsec_config;
				/**< PPE inline IPsec port configuration message. */
		struct nss_ppe_ipsec_port_mtu_msg ipsec_mtu;
				/**< Inline IPsec port MTU change message. */
		struct nss_ppe_ipsec_add_intf_msg ipsec_addif;
				/**< Inline IPsec NSS interface attach message. */
		struct nss_ppe_ipsec_del_intf_msg ipsec_delif;
				/**< Inline IPsec NSS interface detach message. */
	} msg;			/**< Message payload. */
};

/**
 * Callback function for receiving PPE messages.
 *
 * @datatypes
 * nss_ppe_msg
 *
 * @param[in] app_data  Pointer to the application context of the message.
 * @param[in] msg       Pointer to the message data.
 */
typedef void (*nss_ppe_msg_callback_t)(void *app_data, struct nss_ppe_msg *msg);

/**
 * nss_ppe_tx_msg
 *	Sends PPE messages to the NSS.
 *
 * @datatypes
 * nss_ctx_instance \n
 * nss_ppe_msg
 *
 * @param[in] nss_ctx  Pointer to the NSS context.
 * @param[in] msg      Pointer to the message data.
 *
 * @return
 * Status of the Tx operation.
 */
nss_tx_status_t nss_ppe_tx_msg(struct nss_ctx_instance *nss_ctx, struct nss_ppe_msg *msg);

/**
 * nss_ppe_tx_msg_sync
 *	Sends PPE messages synchronously to the NSS.
 *
 * @datatypes
 * nss_ctx_instance \n
 * nss_ppe_msg
 *
 * @param[in]     nss_ctx  Pointer to the NSS context.
 * @param[in,out] msg      Pointer to the message data.
 *
 * @return
 * Status of the Tx operation.
 */
nss_tx_status_t nss_ppe_tx_msg_sync(struct nss_ctx_instance *nss_ctx, struct nss_ppe_msg *msg);

/**
 * nss_ppe_msg_init
 *	Initializes a PPE message.
 *
 * @datatypes
 * nss_ppe_msg
 *
 * @param[in,out] ncm       Pointer to the message.
 * @param[in]     if_num    Interface number
 * @param[in]     type      Type of message.
 * @param[in]     len       Size of the payload.
 * @param[in]     cb        Callback function for the message.
 * @param[in]     app_data  Pointer to the application context of the message.
 *
 * @return
 * None.
 */
void nss_ppe_msg_init(struct nss_ppe_msg *ncm, uint16_t if_num, uint32_t type,  uint32_t len, void *cb, void *app_data);

/**
 * nss_ppe_get_context
 *	Gets the PPE context used in nss_ppe_tx.
 *
 * @return
 * Pointer to the NSS core context.
 */
struct nss_ctx_instance *nss_ppe_get_context(void);

/**
 * nss_ppe_tx_ipsec_config_msg
 *      Sends the PPE a message to configure inline IPsec port.
 *
 * @param[in] if_num        Static IPsec interface number.
 * @param[in] vsi_num       Default VSI number associated with inline IPsec port.
 * @param[in] mtu           Default MTU of static inline IPsec port.
 * @param[in] mru           Default MRU of static inline IPsec port.
 *
 * @return
 * Status of the Tx operation.
 */
nss_tx_status_t nss_ppe_tx_ipsec_config_msg(uint32_t nss_ifnum, uint32_t vsi_num, uint16_t mtu, uint16_t mru);

/**
 * nss_ppe_tx_ipsec_mtu_msg
 *      Sends the PPE a message to configure MTU value on IPsec port.
 *
 * @param[in] nss_ifnum  Static IPsec interface number.
 * @param[in] mtu        MTU of static IPsec interface.
 * @param[in] mru        MRU of static IPsec interface.
 *
 * @return
 * Status of the Tx operation.
 */
nss_tx_status_t nss_ppe_tx_ipsec_mtu_msg(uint32_t nss_ifnum, uint16_t mtu, uint16_t mru);

/**
 * nss_ppe_tx_ipsec_add_intf_msg
 *      Sends the PPE a message to attach a dynamic interface number to IPsec port.
 *
 * @param[in] if_num  Dynamic IPsec/DTLS interface number.
 *
 * @return
 * Status of the Tx operation.
 */
nss_tx_status_t nss_ppe_tx_ipsec_add_intf_msg(uint32_t nss_ifnum);

/**
 * nss_ppe_tx_ipsec_del_intf_msg
 *      Sends the PPE a message to detach a dynamic interface number to IPsec port.
 *
 * @param[in] if_num  Dynamic IPsec/DTLS interface number.
 *
 * @return
 * Status of the Tx operation.
 */
nss_tx_status_t nss_ppe_tx_ipsec_del_intf_msg(uint32_t nss_ifnum);

/*
 * nss_ppe_reg_read()
 */
static inline void nss_ppe_reg_read(u32 reg, u32 *val)
{
	*val = readl((ppe_pvt.ppe_base + reg));
}

/*
 * nss_ppe_reg_write()
 */
static inline void nss_ppe_reg_write(u32 reg, u32 val)
{
	writel(val, (ppe_pvt.ppe_base + reg));
}

/*
 * nss_ppe_log.h
 *	NSS PPE Log Header File
 */

/*
 * nss_ppe_log_tx_msg
 *	Logs a ppe message that is sent to the NSS firmware.
 */
void nss_ppe_log_tx_msg(struct nss_ppe_msg *npm);

/*
 * nss_ppe_log_rx_msg
 *	Logs a ppe message that is received from the NSS firmware.
 */
void nss_ppe_log_rx_msg(struct nss_ppe_msg *npm);

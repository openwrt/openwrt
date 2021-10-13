/*
 **************************************************************************
 * Copyright (c) 2016-2020, The Linux Foundation. All rights reserved.
 *
 * Permission to use, copy, modify, and/or distribute this software for
 * any purpose with or without fee is hereby granted, provided that the
 * above copyright notice and this permission notice appear in all copies.
 *
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
 * @file nss_dp_api_if.h
 *	nss-dp exported structures/apis.
 *
 * This file declares all the public interfaces
 * for NSS data-plane driver.
 */

#ifndef __NSS_DP_API_IF_H
#define __NSS_DP_API_IF_H

#include "nss_dp_arch.h"

/**
 * @addtogroup nss_dp_subsystem
 * @{
 */

/*
 * NSS DP status
 */
#define NSS_DP_SUCCESS	0
#define NSS_DP_FAILURE	-1

/*
 * NSS DP platform specific defines
 */
#define NSS_DP_START_IFNUM	NSS_DP_HAL_START_IFNUM
			/**< First GMAC interface number (0/1) depending on SoC. */
#define NSS_DP_MAX_MTU_SIZE	NSS_DP_HAL_MAX_MTU_SIZE
#define NSS_DP_MAX_PACKET_LEN	NSS_DP_HAL_MAX_PACKET_LEN
#define NSS_DP_MAX_INTERFACES	(NSS_DP_HAL_MAX_PORTS + NSS_DP_HAL_START_IFNUM)
			/**< Last interface index for the SoC, to be used by qca-nss-drv. */

/*
 * NSS PTP service code
 */
#define NSS_PTP_EVENT_SERVICE_CODE	0x9

/**
 * nss_dp_data_plane_ctx
 *	Data plane context base class.
 */
struct nss_dp_data_plane_ctx {
	struct net_device *dev;
};

/**
 * nss_dp_gmac_stats
 *	The per-GMAC statistics structure.
 */
struct nss_dp_gmac_stats {
	struct nss_dp_hal_gmac_stats stats;
};

/**
 * nss_dp_data_plane_ops
 *	Per data-plane ops structure.
 *
 * Default would be slowpath and can be overridden by nss-drv
 */
struct nss_dp_data_plane_ops {
	int (*init)(struct nss_dp_data_plane_ctx *dpc);
	int (*open)(struct nss_dp_data_plane_ctx *dpc, uint32_t tx_desc_ring,
		    uint32_t rx_desc_ring, uint32_t mode);
	int (*close)(struct nss_dp_data_plane_ctx *dpc);
	int (*link_state)(struct nss_dp_data_plane_ctx *dpc,
			  uint32_t link_state);
	int (*mac_addr)(struct nss_dp_data_plane_ctx *dpc, uint8_t *addr);
	int (*change_mtu)(struct nss_dp_data_plane_ctx *dpc, uint32_t mtu);
	netdev_tx_t (*xmit)(struct nss_dp_data_plane_ctx *dpc, struct sk_buff *os_buf);
	void (*set_features)(struct nss_dp_data_plane_ctx *dpc);
	int (*pause_on_off)(struct nss_dp_data_plane_ctx *dpc,
			    uint32_t pause_on);
	int (*vsi_assign)(struct nss_dp_data_plane_ctx *dpc, uint32_t vsi);
	int (*vsi_unassign)(struct nss_dp_data_plane_ctx *dpc, uint32_t vsi);
	int (*rx_flow_steer)(struct nss_dp_data_plane_ctx *dpc, struct sk_buff *skb,
				uint32_t cpu, bool is_add);
	void (*get_stats)(struct nss_dp_data_plane_ctx *dpc, struct nss_dp_gmac_stats *stats);
	int (*deinit)(struct nss_dp_data_plane_ctx *dpc);
};

/**
 * nss_dp_receive
 *	Called by overlay drivers to deliver packets to nss-dp.
 *
 * @datatypes
 * net_device
 * sk_buff
 * napi_struct
 *
 * @param[in] netdev Pointer to netdev structure on which packet is received.
 * @param[in] skb Pointer to the received packet.
 * @param[in] napi Pointer to napi context.
 */
void nss_dp_receive(struct net_device *netdev, struct sk_buff *skb,
						struct napi_struct *napi);

/**
 * nss_dp_is_in_open_state
 *	Returns if a data plane is opened or not.
 *
 * @datatypes
 * net_device
 *
 * @param[in] netdev Pointer to netdev structure.
 *
 * @return
 * bool
 */
bool nss_dp_is_in_open_state(struct net_device *netdev);

/**
 * nss_dp_override_data_palne
 *	API to allow overlay drivers to override the data plane.
 *
 * @datatypes
 * net_device
 * nss_dp_data_plane_ops
 * nss_dp_data_plane_ctx
 *
 * @param[in] netdev Pointer to netdev structure.
 * @param[in] dp_ops Pointer to respective data plane ops structure.
 * @param[in] dpc Pointer to data plane context.
 *
 * @return
 * int
 */
int nss_dp_override_data_plane(struct net_device *netdev,
			       struct nss_dp_data_plane_ops *dp_ops,
			       struct nss_dp_data_plane_ctx *dpc);

/**
 * nss_dp_start_data_plane
 *	Dataplane API to inform netdev when it is ready to start.
 *
 * @datatypes
 * net_device
 * nss_dp_data_plane_ctx
 *
 * @param[in] netdev Pointer to netdev structure.
 * @param[in] dpc Pointer to data plane context.
 */
void nss_dp_start_data_plane(struct net_device *netdev,
			     struct nss_dp_data_plane_ctx *dpc);

/**
 * nss_dp_restore_data_plane
 *	Called by overlay drivers to detach itself from nss-dp.
 *
 * @datatypes
 * net_device
 *
 * @param[in] netdev Pointer to netdev structure.
 */
void nss_dp_restore_data_plane(struct net_device *netdev);

/**
 * nss_dp_get_netdev_by_nss_if_num
 *	Returns the net device of the corresponding id if it exists.
 *
 * @datatypes
 * int
 *
 * @param[in] interface ID of the physical mac port.
 *
 * @return
 * Pointer to netdev structure.
 */
struct net_device *nss_dp_get_netdev_by_nss_if_num(int if_num);

/**
 * nss_phy_tstamp_rx_buf
 *	Receive timestamp packet.
 *
 * @datatypes
 * sk_buff
 *
 * @param[in] app_data Pointer to the application context of the message.
 * @param[in] skb Pointer to the packet.
 */
void nss_phy_tstamp_rx_buf(void *app_data, struct sk_buff *skb);

/**
 * nss_phy_tstamp_tx_buf
 *	Transmit timestamp packet
 *
 * @datatypes
 * net_device
 * sk_buff
 *
 * @param[in] net_device Pointer to netdev structure.
 * @param[in] skb Pointer to the packet.
 */
void nss_phy_tstamp_tx_buf(struct net_device *ndev, struct sk_buff *skb);

/**
 *@}
 */

#endif	/** __NSS_DP_API_IF_H */

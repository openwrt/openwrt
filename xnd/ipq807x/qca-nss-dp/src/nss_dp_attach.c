/*
 **************************************************************************
 * Copyright (c) 2016-2017, 2019-2020 The Linux Foundation. All rights reserved.
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

#include <linux/version.h>
#include "nss_dp_hal.h"

/*
 * nss_dp_reset_netdev_features()
 *	Resets the netdev features
 */
static inline void nss_dp_reset_netdev_features(struct net_device *netdev)
{
	netdev->features = 0;
	netdev->hw_features = 0;
	netdev->vlan_features = 0;
	netdev->wanted_features = 0;
}

/*
 * nss_dp_receive()
 *	Called by overlay drivers to deliver packets to nss-dp
 */
void nss_dp_receive(struct net_device *netdev, struct sk_buff *skb,
					       struct napi_struct *napi)
{
	struct nss_dp_dev *dp_dev = netdev_priv(netdev);

	skb->dev = netdev;
	skb->protocol = eth_type_trans(skb, netdev);
	netdev_dbg(netdev, "Rx on port%d, packet len %d, CSUM %d\n",
			dp_dev->macid, skb->len, skb->ip_summed);

#ifdef CONFIG_NET_SWITCHDEV
#if (LINUX_VERSION_CODE < KERNEL_VERSION(4, 5, 0))
	skb->offload_fwd_mark = netdev->offload_fwd_mark;
#else
	/*
	 * TODO: Implement ndo_get_devlink_port()
	 */
	skb->offload_fwd_mark = 0;
#endif
#endif

	napi_gro_receive(napi, skb);
}
EXPORT_SYMBOL(nss_dp_receive);

/*
 * nss_dp_is_in_open_state()
 *	Return if a data plane is opened or not
 */
bool nss_dp_is_in_open_state(struct net_device *netdev)
{
	struct nss_dp_dev *dp_dev = (struct nss_dp_dev *)netdev_priv(netdev);

	if (test_bit(__NSS_DP_UP, &dp_dev->flags))
		return true;
	return false;
}
EXPORT_SYMBOL(nss_dp_is_in_open_state);

/*
 * nss_dp_override_data_plane()
 *	API to allow overlay drivers to override the data plane
 */
int nss_dp_override_data_plane(struct net_device *netdev,
				struct nss_dp_data_plane_ops *dp_ops,
				struct nss_dp_data_plane_ctx *dpc)
{
	struct nss_dp_dev *dp_dev = (struct nss_dp_dev *)netdev_priv(netdev);

	if (!dp_ops->open || !dp_ops->close || !dp_ops->link_state
		|| !dp_ops->mac_addr || !dp_ops->change_mtu || !dp_ops->xmit
		|| !dp_ops->set_features || !dp_ops->pause_on_off || !dp_ops->deinit) {
		netdev_dbg(netdev, "All the op functions must be present, reject this registeration\n");
		return NSS_DP_FAILURE;
	}

	/*
	 * If this data plane is up, close the netdev to force TX/RX stop, and
	 * also reset the features
	 */
	if (test_bit(__NSS_DP_UP, &dp_dev->flags)) {
		netdev->netdev_ops->ndo_stop(netdev);
		nss_dp_reset_netdev_features(netdev);
	}

	/*
	 * Free up the resources used by the data plane
	 */
	if (dp_dev->drv_flags & NSS_DP_PRIV_FLAG(INIT_DONE)) {
		if (dp_dev->data_plane_ops->deinit(dpc)) {
			netdev_dbg(netdev, "Data plane init failed\n");
			return -ENOMEM;
		}

		dp_dev->drv_flags &= ~NSS_DP_PRIV_FLAG(INIT_DONE);
	}

	/*
	 * Override the data_plane_ctx, data_plane_ops
	 */
	dp_dev->dpc = dpc;
	dp_dev->data_plane_ops = dp_ops;

	return NSS_DP_SUCCESS;
}
EXPORT_SYMBOL(nss_dp_override_data_plane);

/*
 * nss_dp_start_data_plane()
 *	Data plane to inform netdev it is ready to start
 */
void nss_dp_start_data_plane(struct net_device *netdev,
			     struct nss_dp_data_plane_ctx *dpc)
{
	struct nss_dp_dev *dp_dev = (struct nss_dp_dev *)netdev_priv(netdev);

	if (test_bit(__NSS_DP_UP, &dp_dev->flags)) {
		netdev_dbg(netdev, "This netdev already up, something is wrong\n");
		return;
	}

	if (dp_dev->dpc != dpc) {
		netdev_dbg(netdev, "Cookie %px does not match, reject\n", dpc);
		return;
	}

	netdev->netdev_ops->ndo_open(dp_dev->netdev);
}
EXPORT_SYMBOL(nss_dp_start_data_plane);

/*
 * nss_dp_restore_data_plane()
 *	Called by overlay drivers to detach itself from nss-dp
 */
void nss_dp_restore_data_plane(struct net_device *netdev)
{
	struct nss_dp_dev *dp_dev = (struct nss_dp_dev *)netdev_priv(netdev);

	/*
	 * If this data plane is up, close the netdev to force TX/RX stop, and
	 * also reset the features
	 */
	if (test_bit(__NSS_DP_UP, &dp_dev->flags)) {
		netdev->netdev_ops->ndo_stop(netdev);
		nss_dp_reset_netdev_features(netdev);
	}

	dp_dev->data_plane_ops = nss_dp_hal_get_data_plane_ops();
	dp_dev->dpc = &dp_global_data_plane_ctx[dp_dev->macid - NSS_DP_START_IFNUM];

	/*
	 * TODO: Re-initialize EDMA dataplane
	 */
}
EXPORT_SYMBOL(nss_dp_restore_data_plane);

/*
 * nss_dp_get_netdev_by_nss_if_num()
 *	return the net device of the corrsponding id if exist
 */
struct net_device *nss_dp_get_netdev_by_nss_if_num(int if_num)
{
	struct nss_dp_dev *dp_dev;

	if ((if_num > NSS_DP_HAL_MAX_PORTS) || (if_num < NSS_DP_START_IFNUM)) {
		pr_err("Invalid if_num %d\n", if_num);
		return NULL;
	}

	dp_dev = dp_global_ctx.nss_dp[if_num - NSS_DP_START_IFNUM];
	if (!dp_dev)
		return NULL;
	return dp_dev->netdev;
}
EXPORT_SYMBOL(nss_dp_get_netdev_by_nss_if_num);

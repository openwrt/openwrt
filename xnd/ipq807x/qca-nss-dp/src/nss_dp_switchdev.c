/*
 **************************************************************************
 * Copyright (c) 2017-2021, The Linux Foundation. All rights reserved.
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
#include <net/switchdev.h>
#include <linux/if_bridge.h>
#include <net/switchdev.h>

#include "nss_dp_dev.h"
#include "fal/fal_stp.h"
#include "fal/fal_ctrlpkt.h"

#define NSS_DP_SWITCH_ID		0
#define NSS_DP_SW_ETHTYPE_PID		0 /* PPE ethtype profile ID for slow protocols */
#define ETH_P_NONE			0

/*
 * nss_dp_set_slow_proto_filter()
 * 	Enable/Disable filter to allow Ethernet slow-protocol
 */
static void nss_dp_set_slow_proto_filter(struct nss_dp_dev *dp_priv, bool filter_enable)
{
	sw_error_t ret = 0;
	fal_ctrlpkt_profile_t profile;
	fal_ctrlpkt_action_t action;

	memset(&profile, 0, sizeof(profile));

	/*
	 * Action is redirect cpu
	 */
	action.action = FAL_MAC_RDT_TO_CPU;
	action.sg_bypass = A_FALSE;

	/*
	 * Bypass stp
	 */
	action.in_stp_bypass = A_TRUE;
	action.in_vlan_fltr_bypass = A_FALSE;
	action.l2_filter_bypass = A_FALSE;
	profile.action = action;
	profile.ethtype_profile_bitmap = 0x1;

	/*
	 * Set port map
	 */
	profile.port_map = (1 << dp_priv->macid);
	if (filter_enable) {
		ret = fal_mgmtctrl_ctrlpkt_profile_add(NSS_DP_SWITCH_ID, &profile);
		if (ret != SW_OK) {
			netdev_dbg(dp_priv->netdev, "failed to add profile for port_map: 0x%x, ret: %d\n", profile.port_map, ret);
			return;
		}

		/*
		 * Enable filter to allow ethernet slow-protocol,
		 * if this is the first port being disabled by STP
		 */
		if (!dp_priv->ctx->slowproto_acl_bm) {
			ret = fal_mgmtctrl_ethtype_profile_set(NSS_DP_SWITCH_ID, NSS_DP_SW_ETHTYPE_PID, ETH_P_SLOW);
			if (ret != SW_OK) {
				netdev_dbg(dp_priv->netdev, "failed to set ethertype profile: 0x%x, ret: %d\n", ETH_P_SLOW, ret);
				ret = fal_mgmtctrl_ctrlpkt_profile_del(NSS_DP_SWITCH_ID, &profile);
				if (ret != SW_OK) {
					netdev_dbg(dp_priv->netdev, "failed to delete profile for port_map: 0x%x, ret: %d\n", profile.port_map, ret);
				}
				return;
			}
		}

		/*
		 * Add port to port bitmap
		 */
		dp_priv->ctx->slowproto_acl_bm = dp_priv->ctx->slowproto_acl_bm | (1 << dp_priv->macid);
	} else {

		ret = fal_mgmtctrl_ctrlpkt_profile_del(NSS_DP_SWITCH_ID, &profile);
		if (ret != SW_OK) {
			netdev_dbg(dp_priv->netdev, "failed to delete profile for port_map: 0x%x, ret: %d\n", profile.port_map, ret);
			return;
		}

		/*
		 * Delete port from port bitmap
		 */
		dp_priv->ctx->slowproto_acl_bm = dp_priv->ctx->slowproto_acl_bm & (~(1 << dp_priv->macid));

		/*
		 * If all ports are in STP-enabled state, then we do not need
		 * the filter to allow ethernet slow protocol packets
		 */
		if (!dp_priv->ctx->slowproto_acl_bm) {
			ret = fal_mgmtctrl_ethtype_profile_set(NSS_DP_SWITCH_ID, NSS_DP_SW_ETHTYPE_PID, ETH_P_NONE);
			if (ret != SW_OK) {
				netdev_dbg(dp_priv->netdev, "failed to reset ethertype profile: 0x%x ret: %d\n", ETH_P_NONE, ret);
			}
		}
	}
}

/*
 * nss_dp_stp_state_set()
 *	Set bridge port STP state to the port of NSS data plane.
 */
static int nss_dp_stp_state_set(struct nss_dp_dev *dp_priv, u8 state)
{
	sw_error_t err;
	fal_stp_state_t stp_state;

	switch (state) {
	case BR_STATE_DISABLED:
		stp_state = FAL_STP_DISABLED;

		/*
		 * Dynamic bond interfaces which are bridge slaves need to receive
		 * ethernet slow protocol packets for LACP protocol even in STP
		 * disabled state
		 */
		nss_dp_set_slow_proto_filter(dp_priv, true);
		break;
	case BR_STATE_LISTENING:
		stp_state = FAL_STP_LISTENING;
		break;
	case BR_STATE_BLOCKING:
		stp_state = FAL_STP_BLOCKING;
		break;
	case BR_STATE_LEARNING:
		stp_state = FAL_STP_LEARNING;
		break;
	case BR_STATE_FORWARDING:
		stp_state = FAL_STP_FORWARDING;

		/*
		 * Remove the filter for allowing ethernet slow protocol packets
		 * for bond interfaces
		 */
		nss_dp_set_slow_proto_filter(dp_priv, false);
		break;
	default:
		return -EOPNOTSUPP;
	}

	err = fal_stp_port_state_set(NSS_DP_SWITCH_ID, 0, dp_priv->macid,
				     stp_state);
	if (err) {
		netdev_dbg(dp_priv->netdev, "failed to set ftp state\n");

		/*
		 * Restore the slow proto filters
		 */
		if (state == BR_STATE_DISABLED)
			nss_dp_set_slow_proto_filter(dp_priv, false);
		else if (state == BR_STATE_FORWARDING)
			nss_dp_set_slow_proto_filter(dp_priv, true);

		return -EINVAL;
	}

	return 0;
}

#if (LINUX_VERSION_CODE < KERNEL_VERSION(4, 5, 0))
/*
 * nss_dp_attr_get()
 *	Get port information to update switchdev attribute for NSS data plane.
 */
static int nss_dp_attr_get(struct net_device *dev, struct switchdev_attr *attr)
{
	struct nss_dp_dev *dp_priv = (struct nss_dp_dev *)netdev_priv(dev);

	switch (attr->id) {
	case SWITCHDEV_ATTR_ID_PORT_PARENT_ID:
		attr->u.ppid.id_len = 1;
		attr->u.ppid.id[0] = NSS_DP_SWITCH_ID;
		break;

	case SWITCHDEV_ATTR_ID_PORT_BRIDGE_FLAGS:
		attr->u.brport_flags = dp_priv->brport_flags;
		break;
	default:
		return -EOPNOTSUPP;
	}

	return 0;
}

/*
 * nss_dp_attr_set()
 *	Get switchdev attribute and set to the device of NSS data plane.
 */
static int nss_dp_attr_set(struct net_device *dev,
				const struct switchdev_attr *attr,
				struct switchdev_trans *trans)
{
	struct nss_dp_dev *dp_priv = (struct nss_dp_dev *)netdev_priv(dev);
	struct net_device *upper_dev;
	struct vlan_dev_priv *vlan;
	struct list_head *iter;
	uint32_t stp_state = attr->u.stp_state;

	if (switchdev_trans_ph_prepare(trans))
		return 0;

	switch (attr->id) {
	case SWITCHDEV_ATTR_ID_PORT_BRIDGE_FLAGS:
		dp_priv->brport_flags = attr->u.brport_flags;
		netdev_dbg(dev, "set brport_flags %lu\n", attr->u.brport_flags);
		return 0;
	case SWITCHDEV_ATTR_ID_PORT_STP_STATE:
		/*
		 * The stp state is not changed to FAL_STP_DISABLED if
		 * the net_device (dev) has any vlan configured. Otherwise
		 * traffic on other vlan(s) will not work.
		 *
		 * Note: STP for VLANs is not supported by PPE.
		 */
		if ((stp_state == BR_STATE_DISABLED) ||
			(stp_state == BR_STATE_BLOCKING)) {
			rcu_read_lock();
			netdev_for_each_upper_dev_rcu(dev, upper_dev, iter) {
				if (!is_vlan_dev(upper_dev))
					continue;

				vlan = vlan_dev_priv(upper_dev);
				if (vlan->real_dev == dev) {
					rcu_read_unlock();
					netdev_dbg(dev, "Do not update stp state to: %u since vlan id: %d is configured on netdevice: %s\n",
							stp_state, vlan->vlan_id, vlan->real_dev->name);
					return 0;
				}
			}

			rcu_read_unlock();
		}

		return nss_dp_stp_state_set(dp_priv, stp_state);
	default:
		return -EOPNOTSUPP;
	}
}

/*
 * nss_dp_switchdev_ops
 *	Switchdev operations of NSS data plane.
 */
static const struct switchdev_ops nss_dp_switchdev_ops = {
	.switchdev_port_attr_get	= nss_dp_attr_get,
	.switchdev_port_attr_set	= nss_dp_attr_set,
};

/*
 * nss_dp_switchdev_setup()
 *	Set up NSS data plane switchdev operations.
 */
void nss_dp_switchdev_setup(struct net_device *dev)
{
	dev->switchdev_ops = &nss_dp_switchdev_ops;
	switchdev_port_fwd_mark_set(dev, NULL, false);
}
#else

/*
 * nss_dp_port_attr_set()
 *	Sets attributes
 */
static int nss_dp_port_attr_set(struct net_device *dev,
				const struct switchdev_attr *attr,
				struct switchdev_trans *trans)
{
	struct nss_dp_dev *dp_priv = (struct nss_dp_dev *)netdev_priv(dev);

	if (switchdev_trans_ph_prepare(trans))
		return 0;

	switch (attr->id) {
	case SWITCHDEV_ATTR_ID_PORT_BRIDGE_FLAGS:
		dp_priv->brport_flags = attr->u.brport_flags;
		netdev_dbg(dev, "set brport_flags %lu\n", attr->u.brport_flags);
		return 0;
	case SWITCHDEV_ATTR_ID_PORT_STP_STATE:
		return nss_dp_stp_state_set(dp_priv, attr->u.stp_state);
	default:
		return -EOPNOTSUPP;
	}

}

/*
 * nss_dp_switchdev_port_attr_set_event()
 *	Attribute set event
 */
static int nss_dp_switchdev_port_attr_set_event(struct net_device *netdev,
		struct switchdev_notifier_port_attr_info *port_attr_info)
{
	int err;

	err = nss_dp_port_attr_set(netdev, port_attr_info->attr,
				   port_attr_info->trans);

	port_attr_info->handled = true;
	return notifier_from_errno(err);
}

/*
 * nss_dp_switchdev_event()
 *	Switch dev event on netdevice
 */
static int nss_dp_switchdev_event(struct notifier_block *unused,
				  unsigned long event, void *ptr)
{
	struct net_device *dev = switchdev_notifier_info_to_dev(ptr);

	/*
	 * Handle switchdev event only for physical devices
	 */
	if (!nss_dp_is_phy_dev(dev)) {
		return NOTIFY_DONE;
	}

	if (event == SWITCHDEV_PORT_ATTR_SET)
		nss_dp_switchdev_port_attr_set_event(dev, ptr);

	return NOTIFY_DONE;
}

static struct notifier_block nss_dp_switchdev_notifier = {
	.notifier_call = nss_dp_switchdev_event,
};

static bool switch_init_done;

/*
 * nss_dp_switchdev_setup()
 *	Setup switch dev
 */
void nss_dp_switchdev_setup(struct net_device *dev)
{
	int err;

	if (switch_init_done) {
		return;
	}

	err = register_switchdev_blocking_notifier(&nss_dp_switchdev_notifier);
	if (err) {
		netdev_dbg(dev, "%px:Failed to register switchdev notifier\n", dev);
	}

	switch_init_done = true;

}
#endif

// SPDX-License-Identifier: GPL-2.0-only

#include <linux/dsa/tag_rtl_otto.h>
#include <linux/delay.h>
#include <linux/etherdevice.h>
#include <linux/if_bridge.h>
#include <linux/if_vlan.h>
#include <linux/netdevice.h>
#include <linux/rcupdate.h>
#include <linux/rtl838x_eth.h>
#include <linux/rtnetlink.h>
#include <linux/slab.h>
#include <net/netlink.h>
#include <net/switchdev.h>

#include "rtl-otto.h"

#define RTL931X_STACK_INVENTORY_BUSY_ATTEMPTS	8
#define RTL931X_STACK_INVENTORY_RETRY_MIN_US	10000
#define RTL931X_STACK_INVENTORY_RETRY_MAX_US	20000

struct rtl931x_stack_reps {
	struct rtl838x_switch_priv *priv;
	struct rtl_otto_remote_port_map *map;
	struct net_device *ports[RTL931X_STACK_MAX_PORTS];
	u64 port_mask;
	u32 peer_capabilities;
	u8 peer_device;
	u8 fabric_port;
	struct net_device *bridge_dev;
	struct list_head vlans;
	u64 bridge_port_mask;
	u64 bridge_cleanup_mask;
	bool remote_delegation_possible;
	bool published;
	bool active;
	bool fenced;
	bool bridge_dirty;
	bool teardown;
};

struct rtl931x_stack_rep_priv {
	struct rtl931x_stack_reps *reps;
	struct net_device *bridge_dev;
	u8 port;
};

struct rtl931x_stack_rep_vlan {
	struct list_head list;
	u16 vid;
	u16 flags;
	u8 port;
};

struct rtl931x_stack_peer_inventory {
	struct rtl931x_stack_peer_switch_info switch_info;
	struct rtl931x_stack_peer_port_info ports[RTL931X_STACK_MAX_PORTS];
};

static netdev_tx_t rtl931x_stack_rep_xmit(struct sk_buff *skb,
					  struct net_device *dev)
{
	struct rtl931x_stack_rep_priv *rep = netdev_priv(dev);
	struct rtl931x_stack_reps *reps = rep->reps;
	unsigned int len = skb->len;
	int err;

	if (unlikely(!READ_ONCE(reps->active))) {
		dev_kfree_skb_any(skb);
		dev_core_stats_tx_dropped_inc(dev);
		return NETDEV_TX_OK;
	}

	err = rtl838x_eth_stack_port_xmit(reps->priv->stack.talk_conduit,
					  skb, reps->peer_device,
					  rep->port);
	if (unlikely(err))
		dev_core_stats_tx_dropped_inc(dev);
	else
		dev_sw_netstats_tx_add(dev, 1, len);

	return NETDEV_TX_OK;
}

static int rtl931x_stack_rep_open(struct net_device *dev)
{
	struct rtl931x_stack_rep_priv *rep = netdev_priv(dev);

	if (READ_ONCE(rep->reps->active))
		netif_start_queue(dev);
	else
		netif_stop_queue(dev);

	return 0;
}

static int rtl931x_stack_rep_stop(struct net_device *dev)
{
	netif_stop_queue(dev);
	return 0;
}

static int rtl931x_stack_rep_change_mtu(struct net_device *dev, int mtu)
{
	return -EOPNOTSUPP;
}

static int
rtl931x_stack_rep_set_mac_address(struct net_device *dev, void *addr)
{
	return -EOPNOTSUPP;
}

static int rtl931x_stack_rep_phys_port_name(struct net_device *dev, char *name,
					    size_t len)
{
	struct rtl931x_stack_rep_priv *rep = netdev_priv(dev);
	int ret;

	ret = snprintf(name, len, "d%up%u", rep->reps->peer_device,
		       rep->port);
	return ret >= len ? -EOPNOTSUPP : 0;
}

static int rtl931x_stack_rep_get_iflink(const struct net_device *dev)
{
	struct rtl931x_stack_rep_priv *rep = netdev_priv(dev);
	struct rtl838x_switch_priv *priv = rep->reps->priv;
	const struct dsa_port *dp = priv->ports[rep->reps->fabric_port].dp;

	return dp && dp->user ? dp->user->ifindex : 0;
}

static const struct net_device_ops rtl931x_stack_rep_netdev_ops = {
	.ndo_open = rtl931x_stack_rep_open,
	.ndo_stop = rtl931x_stack_rep_stop,
	.ndo_start_xmit = rtl931x_stack_rep_xmit,
	.ndo_change_mtu = rtl931x_stack_rep_change_mtu,
	.ndo_set_mac_address = rtl931x_stack_rep_set_mac_address,
	.ndo_get_phys_port_name = rtl931x_stack_rep_phys_port_name,
	.ndo_get_iflink = rtl931x_stack_rep_get_iflink,
};

static void rtl931x_stack_rep_setup(struct net_device *dev)
{
	ether_setup(dev);
	dev->netdev_ops = &rtl931x_stack_rep_netdev_ops;
	dev->pcpu_stat_type = NETDEV_PCPU_STAT_TSTATS;
	dev->priv_flags |= IFF_NO_QUEUE | IFF_DISABLE_NETPOLL;
	dev->needed_tailroom = RTL_OTTO_TAG_LEN;
}

static bool rtl931x_stack_rep_is_ours(const struct net_device *dev)
{
	return dev->netdev_ops == &rtl931x_stack_rep_netdev_ops;
}

static void rtl931x_stack_reps_fence(struct rtl931x_stack_reps *reps);
static void rtl931x_stack_reps_recover_later(struct rtl931x_stack_reps *reps,
					     int err);

static struct rtl931x_stack_rep_vlan *
rtl931x_stack_rep_vlan_find(struct rtl931x_stack_reps *reps, u8 port, u16 vid)
{
	struct rtl931x_stack_rep_vlan *vlan;

	list_for_each_entry(vlan, &reps->vlans, list)
		if (vlan->port == port && vlan->vid == vid)
			return vlan;

	return NULL;
}

static void
rtl931x_stack_rep_vlans_forget(struct rtl931x_stack_rep_priv *rep)
{
	struct rtl931x_stack_rep_vlan *vlan, *tmp;

	list_for_each_entry_safe(vlan, tmp, &rep->reps->vlans, list) {
		if (vlan->port != rep->port)
			continue;
		rtl931x_stack_local_fabric_vlan(rep->reps->priv, vlan->vid,
						 false);
		list_del(&vlan->list);
		kfree(vlan);
	}
}

static int
rtl931x_stack_rep_bridge_validate(struct rtl931x_stack_rep_priv *rep,
				  struct net_device *bridge,
				  struct netlink_ext_ack *extack)
{
	u16 proto;
	int err;

	if (!(rep->reps->peer_capabilities &
	      RTL931X_STACK_PEER_CAP_BRIDGE_VLAN)) {
		NL_SET_ERR_MSG_MOD(extack,
				   "peer does not support bridge VLAN transport");
		return -EOPNOTSUPP;
	}
	if (rep->reps->teardown)
		return -ENODEV;
	if (READ_ONCE(rep->reps->priv->stack.flags)) {
		NL_SET_ERR_MSG_MOD(extack,
				   "software peer bridging requires stack flags 0");
		return -EOPNOTSUPP;
	}

	if (!netif_is_bridge_master(bridge)) {
		NL_SET_ERR_MSG_MOD(extack,
				   "RTL931x peer ports support only a bridge upper");
		return -EOPNOTSUPP;
	}
	if (!br_vlan_enabled(bridge)) {
		NL_SET_ERR_MSG_MOD(extack,
				   "RTL931x peer ports require VLAN filtering");
		return -EOPNOTSUPP;
	}
	err = br_vlan_get_proto(bridge, &proto);
	if (err || proto != ETH_P_8021Q) {
		NL_SET_ERR_MSG_MOD(extack,
				   "RTL931x peer ports require 802.1Q VLANs");
		return -EPROTONOSUPPORT;
	}
	if (br_mst_enabled(bridge)) {
		NL_SET_ERR_MSG_MOD(extack,
				   "RTL931x peer ports do not support MST");
		return -EOPNOTSUPP;
	}
	if (rep->reps->bridge_dev && rep->reps->bridge_dev != bridge) {
		NL_SET_ERR_MSG_MOD(extack,
				   "all RTL931x peer ports must use one bridge");
		return -EBUSY;
	}

	return 0;
}

static int
rtl931x_stack_rep_bridge_join(struct rtl931x_stack_rep_priv *rep,
			      struct net_device *bridge,
			      struct netlink_ext_ack *extack)
{
	struct rtl931x_stack_reps *reps = rep->reps;
	int err;

	err = rtl931x_stack_local_bridge_port(reps->priv, true);
	if (err)
		return err;
	err = rtl931x_stack_peer_set_bridge_port(reps->priv, rep->port, true);
	if (err) {
		rtl931x_stack_local_bridge_port(reps->priv, false);
		reps->bridge_dirty = true;
		reps->bridge_cleanup_mask |= BIT_ULL(rep->port);
		rtl931x_stack_reps_fence(reps);
		rtl931x_stack_reps_recover_later(reps, err);
		NL_SET_ERR_MSG_MOD(extack,
				   "failed to configure peer bridge transport");
		return err;
	}

	rep->bridge_dev = bridge;
	reps->bridge_dev = bridge;
	reps->bridge_port_mask |= BIT_ULL(rep->port);
	reps->bridge_cleanup_mask &= ~BIT_ULL(rep->port);

	return 0;
}

static void rtl931x_stack_rep_bridge_leave(struct rtl931x_stack_rep_priv *rep)
{
	struct rtl931x_stack_reps *reps = rep->reps;
	int err;

	if (!rep->bridge_dev)
		return;
	err = reps->teardown ? 0 :
		rtl931x_stack_peer_set_bridge_port(reps->priv, rep->port, false);
	if (err) {
		reps->bridge_dirty = true;
		reps->bridge_cleanup_mask |= BIT_ULL(rep->port);
		rtl931x_stack_reps_fence(reps);
		rtl931x_stack_reps_recover_later(reps, err);
		netdev_err(reps->ports[rep->port],
			   "failed to remove peer bridge transport: %pe\n",
			   ERR_PTR(err));
	}
	rtl931x_stack_rep_vlans_forget(rep);
	rtl931x_stack_local_bridge_port(reps->priv, false);
	reps->bridge_port_mask &= ~BIT_ULL(rep->port);
	rep->bridge_dev = NULL;
	if (!reps->bridge_port_mask)
		reps->bridge_dev = NULL;
}

static int rtl931x_stack_rep_netdev_event(struct notifier_block *nb,
					  unsigned long event, void *ptr)
{
	struct net_device *dev = netdev_notifier_info_to_dev(ptr);
	struct netdev_notifier_changeupper_info *info;
	struct rtl931x_stack_rep_priv *rep;
	struct netlink_ext_ack *extack;
	int err;

	if ((event != NETDEV_PRECHANGEUPPER && event != NETDEV_CHANGEUPPER) ||
	    !rtl931x_stack_rep_is_ours(dev))
		return NOTIFY_DONE;

	info = ptr;
	rep = netdev_priv(dev);
	extack = netdev_notifier_info_to_extack(&info->info);
	if (event == NETDEV_PRECHANGEUPPER)
		return info->linking ?
			notifier_from_errno(
				rtl931x_stack_rep_bridge_validate(rep,
							  info->upper_dev,
							  extack)) :
			NOTIFY_DONE;

	if (info->linking) {
		err = rtl931x_stack_rep_bridge_join(rep, info->upper_dev, extack);
		return notifier_from_errno(err);
	}
	rtl931x_stack_rep_bridge_leave(rep);
	return NOTIFY_OK;
}

static struct notifier_block rtl931x_stack_rep_netdev_nb = {
	.notifier_call = rtl931x_stack_rep_netdev_event,
};

static int
rtl931x_stack_rep_vlan_add(struct net_device *dev, const void *ctx,
			   const struct switchdev_obj *obj,
			   struct netlink_ext_ack *extack)
{
	const struct switchdev_obj_port_vlan *vlan;
	struct rtl931x_stack_rep_priv *rep = netdev_priv(dev);
	struct rtl931x_stack_rep_vlan *state;
	bool created = false;
	u16 flags = 0;
	int err;

	if (obj->id != SWITCHDEV_OBJ_ID_PORT_VLAN || obj->orig_dev != dev)
		return -EOPNOTSUPP;
	vlan = SWITCHDEV_OBJ_PORT_VLAN(obj);
	if (!rep->bridge_dev || !vlan->vid || vlan->vid >= VLAN_N_VID)
		return -EINVAL;
	if (vlan->flags & ~(BRIDGE_VLAN_INFO_BRENTRY |
			    BRIDGE_VLAN_INFO_UNTAGGED |
			    BRIDGE_VLAN_INFO_PVID))
		return -EOPNOTSUPP;
	if (vlan->flags & BRIDGE_VLAN_INFO_UNTAGGED)
		flags |= RTL931X_STACK_VLAN_F_UNTAGGED;
	if (vlan->flags & BRIDGE_VLAN_INFO_PVID)
		flags |= RTL931X_STACK_VLAN_F_PVID;

	state = rtl931x_stack_rep_vlan_find(rep->reps, rep->port, vlan->vid);
	if (state && state->flags == flags)
		return -EOPNOTSUPP;
	if (!state) {
		state = kzalloc(sizeof(*state), GFP_KERNEL);
		if (!state)
			return -ENOMEM;
		state->port = rep->port;
		state->vid = vlan->vid;
		INIT_LIST_HEAD(&state->list);
		created = true;
		err = rtl931x_stack_local_fabric_vlan(rep->reps->priv,
							 vlan->vid, true);
		if (err)
			goto err_free;
	}

	err = rtl931x_stack_peer_set_port_vlan(rep->reps->priv, rep->port,
						       vlan->vid, flags, true);
	if (err) {
		NL_SET_ERR_MSG_MOD(extack, "failed to configure peer VLAN");
		if (created)
			rtl931x_stack_local_fabric_vlan(rep->reps->priv,
							 vlan->vid, false);
		rep->reps->bridge_dirty = true;
		rtl931x_stack_reps_fence(rep->reps);
		rtl931x_stack_reps_recover_later(rep->reps, err);
		goto err_free;
	}
	state->flags = flags;
	if (created)
		list_add_tail(&state->list, &rep->reps->vlans);

	/* Keep Linux bridge VLAN processing in software. */
	return -EOPNOTSUPP;

err_free:
	if (created)
		kfree(state);
	return err;
}

static int
rtl931x_stack_rep_vlan_del(struct net_device *dev, const void *ctx,
			   const struct switchdev_obj *obj)
{
	const struct switchdev_obj_port_vlan *vlan;
	struct rtl931x_stack_rep_priv *rep = netdev_priv(dev);
	struct rtl931x_stack_rep_vlan *state;
	int err;

	if (obj->id != SWITCHDEV_OBJ_ID_PORT_VLAN || obj->orig_dev != dev)
		return -EOPNOTSUPP;
	vlan = SWITCHDEV_OBJ_PORT_VLAN(obj);
	state = rtl931x_stack_rep_vlan_find(rep->reps, rep->port, vlan->vid);
	if (!state)
		return -EOPNOTSUPP;

	list_del_init(&state->list);
	err = rep->reps->teardown ? 0 :
		rtl931x_stack_peer_set_port_vlan(rep->reps->priv, rep->port,
						  vlan->vid, 0, false);
	rtl931x_stack_local_fabric_vlan(rep->reps->priv, vlan->vid, false);
	kfree(state);
	if (err) {
		rep->reps->bridge_dirty = true;
		rtl931x_stack_reps_fence(rep->reps);
		rtl931x_stack_reps_recover_later(rep->reps, err);
		netdev_err(dev, "failed to remove peer VLAN %u: %pe\n",
			   vlan->vid, ERR_PTR(err));
	}

	/* Bridge teardown cannot be vetoed; reconcile the absolute port state. */
	return -EOPNOTSUPP;
}

static int
rtl931x_stack_rep_attr_set(struct net_device *dev, const void *ctx,
			   const struct switchdev_attr *attr,
			   struct netlink_ext_ack *extack)
{
	switch (attr->id) {
	case SWITCHDEV_ATTR_ID_BRIDGE_VLAN_FILTERING:
		if (!attr->u.vlan_filtering) {
			NL_SET_ERR_MSG_MOD(extack,
					   "RTL931x peer bridge requires VLAN filtering");
			return -EINVAL;
		}
		return 0;
	case SWITCHDEV_ATTR_ID_BRIDGE_VLAN_PROTOCOL:
		if (attr->u.vlan_protocol != ETH_P_8021Q) {
			NL_SET_ERR_MSG_MOD(extack,
					   "RTL931x peer bridge requires 802.1Q");
			return -EPROTONOSUPPORT;
		}
		return 0;
	case SWITCHDEV_ATTR_ID_BRIDGE_MST:
		if (attr->u.mst) {
			NL_SET_ERR_MSG_MOD(extack,
					   "RTL931x peer bridge does not support MST");
			return -EINVAL;
		}
		return 0;
	default:
		return -EOPNOTSUPP;
	}
}

static int rtl931x_stack_rep_switchdev_event(struct notifier_block *nb,
					      unsigned long event, void *ptr)
{
	struct net_device *dev = switchdev_notifier_info_to_dev(ptr);
	int err;

	switch (event) {
	case SWITCHDEV_PORT_OBJ_ADD:
		err = switchdev_handle_port_obj_add(dev, ptr,
						    rtl931x_stack_rep_is_ours,
						    rtl931x_stack_rep_vlan_add);
		break;
	case SWITCHDEV_PORT_OBJ_DEL:
		err = switchdev_handle_port_obj_del(dev, ptr,
						    rtl931x_stack_rep_is_ours,
						    rtl931x_stack_rep_vlan_del);
		break;
	case SWITCHDEV_PORT_ATTR_SET:
		err = switchdev_handle_port_attr_set(dev, ptr,
						     rtl931x_stack_rep_is_ours,
						     rtl931x_stack_rep_attr_set);
		break;
	default:
		return NOTIFY_DONE;
	}

	return notifier_from_errno(err);
}

static struct notifier_block rtl931x_stack_rep_switchdev_nb = {
	.notifier_call = rtl931x_stack_rep_switchdev_event,
};

static void rtl931x_stack_reps_map_put(struct rtl931x_stack_reps *reps)
{
	int port;

	if (!reps->map)
		return;

	for (port = 0; port < RTL931X_STACK_MAX_PORTS; port++)
		if (reps->map->ports[port])
			dev_put(reps->map->ports[port]);

	kfree(reps->map);
	reps->map = NULL;
}

static int rtl931x_stack_reps_map_build(struct rtl931x_stack_reps *reps)
{
	struct rtl_otto_remote_port_map *map;
	int port;

	if (reps->map)
		return 0;

	map = kzalloc(sizeof(*map), GFP_KERNEL);
	if (!map)
		return -ENOMEM;

	map->device = reps->peer_device;
	map->fabric_port = reps->fabric_port;
	for (port = 0; port < RTL931X_STACK_MAX_PORTS; port++) {
		if (!(reps->port_mask & BIT_ULL(port)))
			continue;
		dev_hold(reps->ports[port]);
		map->ports[port] = reps->ports[port];
	}
	reps->map = map;

	return 0;
}

static void rtl931x_stack_reps_destroy(struct rtl931x_stack_reps *reps)
{
	struct rtl931x_stack_context *stack = &reps->priv->stack;
	int port;

	reps->teardown = true;
	WARN_ON_ONCE(reps->published);
	rtl931x_stack_reps_map_put(reps);
	if (stack->reps == reps)
		stack->reps = NULL;

	for (port = 0; port < RTL931X_STACK_MAX_PORTS; port++) {
		if (!reps->ports[port])
			continue;
		unregister_netdevice(reps->ports[port]);
		free_netdev(reps->ports[port]);
	}
	WARN_ON_ONCE(reps->bridge_port_mask);
	WARN_ON_ONCE(!list_empty(&reps->vlans));
	kfree(reps);
}

static int
rtl931x_stack_peer_get_inventory(struct rtl838x_switch_priv *priv,
				 struct rtl931x_stack_peer_inventory *inventory,
				 struct netlink_ext_ack *extack)
{
	struct rtl931x_stack_peer_switch_info *info = &inventory->switch_info;
	int attempt, port, err;

	memset(inventory, 0, sizeof(*inventory));
	for (attempt = 0; attempt < RTL931X_STACK_INVENTORY_BUSY_ATTEMPTS;
	     attempt++) {
		err = rtl931x_stack_peer_get_switch_info(priv, info, NULL);
		if (err != -EBUSY)
			break;
		usleep_range(RTL931X_STACK_INVENTORY_RETRY_MIN_US,
			     RTL931X_STACK_INVENTORY_RETRY_MAX_US);
	}
	if (err)
		NL_SET_ERR_MSG_MOD(extack,
				   "failed to read peer switch inventory");
	if (err)
		return err;
	if (!(info->capabilities & RTL931X_STACK_PEER_CAP_GET_PORT_STATE) ||
	    !info->user_port_mask) {
		NL_SET_ERR_MSG_MOD(extack,
				   "peer does not provide per-port inventory");
		return -EOPNOTSUPP;
	}

	for (port = 0; port < RTL931X_STACK_MAX_PORTS; port++) {
		if (!(info->user_port_mask & BIT_ULL(port)))
			continue;
		for (attempt = 0;
		     attempt < RTL931X_STACK_INVENTORY_BUSY_ATTEMPTS; attempt++) {
			err = rtl931x_stack_peer_get_port_info(priv, port,
							       &inventory->ports[port],
							       NULL);
			if (err != -EBUSY)
				break;
			usleep_range(RTL931X_STACK_INVENTORY_RETRY_MIN_US,
				     RTL931X_STACK_INVENTORY_RETRY_MAX_US);
		}
		if (err)
			NL_SET_ERR_MSG_FMT_MOD(extack,
					       "failed to read peer port %d state",
					       port);
		if (err)
			return err;
		if (inventory->ports[port].mtu < ETH_MIN_MTU) {
			NL_SET_ERR_MSG_MOD(extack,
					   "peer returned an invalid port MTU");
			return -EPROTO;
		}
	}

	return 0;
}

static unsigned int
rtl931x_stack_rep_mtu(const struct net_device *conduit,
		      const struct rtl931x_stack_peer_port_info *port_info)
{
	return min3((unsigned int)conduit->mtu, port_info->mtu,
		    (unsigned int)ETH_DATA_LEN);
}

static int
rtl931x_stack_reps_create(struct rtl838x_switch_priv *priv,
			  const struct rtl931x_stack_peer_inventory *inventory,
			  struct rtl931x_stack_reps **new_reps)
{
	const struct rtl931x_stack_peer_switch_info *info =
		&inventory->switch_info;
	struct rtl931x_stack_context *stack = &priv->stack;
	const struct dsa_port *fabric_dp = priv->ports[stack->port].dp;
	struct rtl931x_stack_reps *reps;
	struct net_device *conduit = stack->talk_conduit;
	struct net_device *fabric;
	int port, err;

	if (!conduit || !fabric_dp || !fabric_dp->user ||
	    conduit->mtu < ETH_MIN_MTU)
		return -ENODEV;
	fabric = fabric_dp->user;
	if (!net_eq(dev_net(conduit), dev_net(fabric)))
		return -EXDEV;

	reps = kzalloc(sizeof(*reps), GFP_KERNEL);
	if (!reps)
		return -ENOMEM;
	reps->priv = priv;
	reps->port_mask = info->user_port_mask;
	reps->peer_capabilities = info->capabilities;
	reps->peer_device = stack->peer_id;
	reps->fabric_port = stack->port;
	reps->fenced = true;
	INIT_LIST_HEAD(&reps->vlans);

	for (port = 0; port < RTL931X_STACK_MAX_PORTS; port++) {
		const struct rtl931x_stack_peer_port_info *port_info =
			&inventory->ports[port];
		struct rtl931x_stack_rep_priv *rep;
		struct net_device *dev;
		unsigned int mtu;
		char name[IFNAMSIZ];

		if (!(reps->port_mask & BIT_ULL(port)))
			continue;

		snprintf(name, sizeof(name), "sw%up%u", reps->peer_device,
			 port);
		dev = alloc_netdev(sizeof(*rep), name, NET_NAME_PREDICTABLE,
				   rtl931x_stack_rep_setup);
		if (!dev) {
			err = -ENOMEM;
			goto err_destroy;
		}

		rep = netdev_priv(dev);
		rep->reps = reps;
		rep->port = port;
		SET_NETDEV_DEV(dev, priv->dev);
		dev_net_set(dev, dev_net(fabric));
		dev->netns_immutable = true;
		dev->dev_port = (reps->peer_device << 6) | port;
		eth_hw_addr_set(dev, port_info->mac);
		ether_addr_copy(dev->perm_addr, port_info->mac);
		mtu = rtl931x_stack_rep_mtu(conduit, port_info);
		dev->min_mtu = ETH_MIN_MTU;
		dev->max_mtu = mtu;
		dev->mtu = mtu;
		netif_carrier_off(dev);
		netif_dormant_on(dev);

		err = register_netdevice(dev);
		if (err) {
			free_netdev(dev);
			goto err_destroy;
		}
		netif_tx_disable(dev);
		reps->ports[port] = dev;
	}

	err = rtl931x_stack_reps_map_build(reps);
	if (err)
		goto err_destroy;

	stack->reps = reps;
	*new_reps = reps;
	return 0;

err_destroy:
	rtl931x_stack_reps_destroy(reps);
	return err;
}

static void
rtl931x_stack_reps_refresh(struct rtl931x_stack_reps *reps,
			   const struct rtl931x_stack_peer_switch_info *info)
{
	int port;

	for (port = 0; port < RTL931X_STACK_MAX_PORTS; port++) {
		struct net_device *dev = reps->ports[port];

		if (!dev)
			continue;
		if (info->carrier_mask & BIT_ULL(port))
			netif_carrier_on(dev);
		else
			netif_carrier_off(dev);
	}
}

static void rtl931x_stack_reps_fence(struct rtl931x_stack_reps *reps)
{
	int port;

	WRITE_ONCE(reps->active, false);
	if (reps->map)
		WRITE_ONCE(reps->map->active, false);
	WRITE_ONCE(reps->fenced, true);
	for (port = 0; port < RTL931X_STACK_MAX_PORTS; port++) {
		struct net_device *dev = reps->ports[port];

		if (!dev)
			continue;
		netif_tx_disable(dev);
		netif_carrier_off(dev);
		netif_dormant_on(dev);
	}
}

static int
rtl931x_stack_reps_drain_conduit(struct rtl838x_switch_priv *priv,
				 int fabric_port)
{
	struct rtl931x_stack_conduit conduit = {};
	int err;

	err = rtl931x_stack_quiesce_conduit(priv, fabric_port, &conduit);
	rtl931x_stack_resume_conduit(&conduit);

	return err;
}

static void rtl931x_stack_reps_recover_later(struct rtl931x_stack_reps *reps,
					     int err)
{
	struct rtl931x_stack_context *stack = &reps->priv->stack;
	bool pending = READ_ONCE(stack->reps_recovery_pending);

	WRITE_ONCE(stack->reps_last_error, err);
	WRITE_ONCE(stack->reps_recovery_pending, true);
	if (!pending)
		WRITE_ONCE(stack->reps_recovery_attempts, 0);
	if (READ_ONCE(stack->fabric_link_up))
		mod_delayed_work(system_wq, &stack->reps_recovery_work, 0);
}

static int
rtl931x_stack_reps_activate(struct rtl931x_stack_reps *reps,
			    const struct rtl931x_stack_peer_switch_info *info,
			    int link_epoch)
{
	int port;

	if (!READ_ONCE(reps->priv->stack.fabric_link_up) ||
	    atomic_read(&reps->priv->stack.fabric_link_epoch) != link_epoch) {
		rtl931x_stack_reps_recover_later(reps, -ENOLINK);
		return -ENOLINK;
	}
	synchronize_net();
	if (rtl931x_stack_reps_drain_conduit(reps->priv,
					     reps->fabric_port))
		return -EIO;
	if (!READ_ONCE(reps->priv->stack.fabric_link_up) ||
	    atomic_read(&reps->priv->stack.fabric_link_epoch) != link_epoch) {
		rtl931x_stack_reps_recover_later(reps, -ENOLINK);
		return -ENOLINK;
	}

	WRITE_ONCE(reps->active, true);
	WRITE_ONCE(reps->map->active, true);
	for (port = 0; port < RTL931X_STACK_MAX_PORTS; port++) {
		struct net_device *dev = reps->ports[port];

		if (!dev)
			continue;
		netif_dormant_off(dev);
		if (netif_running(dev))
			netif_tx_wake_all_queues(dev);
	}
	rtl931x_stack_reps_refresh(reps, info);
	WRITE_ONCE(reps->fenced, false);
	WRITE_ONCE(reps->priv->stack.reps_desired, true);
	WRITE_ONCE(reps->priv->stack.reps_recovery_pending, false);
	WRITE_ONCE(reps->priv->stack.reps_recovery_attempts, 0);
	WRITE_ONCE(reps->priv->stack.reps_last_error, 0);

	/* Close a publication race with the phylink link-down callback. */
	if (!READ_ONCE(reps->priv->stack.fabric_link_up) ||
	    !READ_ONCE(reps->active) ||
	    atomic_read(&reps->priv->stack.fabric_link_epoch) != link_epoch) {
		rtl931x_stack_reps_recover_later(reps, -ENOLINK);
		rtl931x_stack_reps_fence(reps);
		return -ENOLINK;
	}

	return 0;
}

static bool
rtl931x_stack_reps_info_matches(const struct rtl931x_stack_reps *reps,
				const struct rtl931x_stack_peer_inventory *inventory)
{
	const struct rtl931x_stack_peer_switch_info *info =
		&inventory->switch_info;
	const struct net_device *conduit = reps->priv->stack.talk_conduit;
	int port;

	if (!conduit || reps->port_mask != info->user_port_mask ||
	    reps->peer_capabilities != info->capabilities ||
	    reps->peer_device != reps->priv->stack.peer_id ||
	    reps->fabric_port != reps->priv->stack.port)
		return false;

	for (port = 0; port < RTL931X_STACK_MAX_PORTS; port++) {
		const struct rtl931x_stack_peer_port_info *port_info =
			&inventory->ports[port];
		struct net_device *dev = reps->ports[port];
		unsigned int mtu;

		if (!(reps->port_mask & BIT_ULL(port))) {
			if (dev)
				return false;
			continue;
		}
		if (!dev || !ether_addr_equal(dev->dev_addr, port_info->mac))
			return false;
		mtu = rtl931x_stack_rep_mtu(conduit, port_info);
		if (dev->mtu != mtu || dev->max_mtu != mtu)
			return false;
	}

	return true;
}

static int
rtl931x_stack_reps_publish(struct rtl931x_stack_reps *reps,
			   const struct rtl931x_stack_peer_switch_info *info,
			   int link_epoch)
{
	struct rtl_otto_tagger_data *tagger_data = reps->priv->ds->tagger_data;
	int err;

	if (!tagger_data)
		return -ENODEV;
	if (rcu_access_pointer(tagger_data->remote_ports))
		return -EBUSY;

	err = rtl931x_stack_reps_map_build(reps);
	if (err)
		return err;

	/* RX depends on delegated DSA ports retaining their host MACs. */
	WRITE_ONCE(reps->priv->stack.reps_desired, true);
	rcu_assign_pointer(tagger_data->remote_ports, reps->map);
	reps->published = true;

	return rtl931x_stack_reps_activate(reps, info, link_epoch);
}

static void rtl931x_stack_reps_deactivate(struct rtl931x_stack_reps *reps)
{
	struct rtl_otto_tagger_data *tagger_data = reps->priv->ds->tagger_data;
	struct rtl_otto_remote_port_map *old_map;

	rtl931x_stack_reps_fence(reps);

	if (!reps->published)
		return;

	old_map = rcu_replace_pointer(tagger_data->remote_ports, NULL,
				      lockdep_rtnl_is_held());
	WARN_ON_ONCE(old_map != reps->map);
	reps->published = false;
	synchronize_net();
	rtl931x_stack_reps_map_put(reps);
}

static bool rtl931x_stack_rpc_uncertain(int err)
{
	return err == -EIO || err == -ETIMEDOUT || err == -ERESTARTSYS;
}

static int rtl931x_stack_reps_bridge_replay(struct rtl931x_stack_reps *reps)
{
	struct rtl931x_stack_rep_vlan *vlan;
	int port, err;

	/* SET_DELEGATED(true) cleared the complete follower bridge shadow. */
	reps->bridge_cleanup_mask = 0;

	for (port = 0; port < RTL931X_STACK_MAX_PORTS; port++) {
		if (!(reps->bridge_port_mask & BIT_ULL(port)))
			continue;
		err = rtl931x_stack_peer_set_bridge_port(reps->priv, port, true);
		if (err)
			return err;
		list_for_each_entry(vlan, &reps->vlans, list) {
			if (vlan->port != port)
				continue;
			err = rtl931x_stack_peer_set_port_vlan(reps->priv, port,
							      vlan->vid,
							      vlan->flags,
							      true);
			if (err)
				return err;
		}
	}

	reps->bridge_dirty = false;
	return 0;
}

static int
rtl931x_stack_reps_disable(struct rtl838x_switch_priv *priv,
			   struct netlink_ext_ack *extack)
{
	struct rtl931x_stack_context *stack = &priv->stack;
	struct rtl931x_stack_conduit conduit = {};
	struct rtl931x_stack_reps *reps = stack->reps;
	int err;

	if (reps && reps->bridge_port_mask && extack) {
		NL_SET_ERR_MSG_MOD(extack,
				   "remove peer ports from their bridge first");
		return -EBUSY;
	}
	WRITE_ONCE(stack->reps_desired, false);
	WRITE_ONCE(stack->reps_recovery_pending, false);
	WRITE_ONCE(stack->reps_recovery_attempts, 0);
	if (reps)
		rtl931x_stack_reps_deactivate(reps);

	err = rtl931x_stack_quiesce_conduit(priv, stack->port, &conduit);
	rtl931x_stack_resume_conduit(&conduit);
	if (err) {
		NL_SET_ERR_MSG_MOD(extack,
				   "failed to drain peer-port transmissions");
		return err;
	}

	err = rtl931x_stack_peer_set_delegated(priv, false);
	if (err) {
		if (reps && rtl931x_stack_rpc_uncertain(err))
			reps->remote_delegation_possible = true;
		NL_SET_ERR_MSG_MOD(extack, "failed to undelegate peer ports");
		return err;
	}
	err = rtl931x_stack_local_remote_matrices(priv, 0);
	if (err) {
		NL_SET_ERR_MSG_MOD(extack,
				   "failed to restore peer source isolation");
		return err;
	}

	if (reps) {
		reps->remote_delegation_possible = false;
		rtl931x_stack_reps_destroy(reps);
	}

	return 0;
}

static int
rtl931x_stack_reps_enable(struct rtl838x_switch_priv *priv,
			  struct netlink_ext_ack *extack)
{
	struct rtl931x_stack_context *stack = &priv->stack;
	struct rtl931x_stack_peer_inventory inventory;
	struct rtl931x_stack_peer_switch_info *info = &inventory.switch_info;
	struct rtl931x_stack_reps *reps = stack->reps;
	bool created = false;
	int link_epoch;
	int err;

	if (!READ_ONCE(stack->fabric_link_up))
		return -ENOLINK;
	link_epoch = atomic_read(&stack->fabric_link_epoch);

	err = rtl931x_stack_peer_get_inventory(priv, &inventory, extack);
	if (err)
		return err;
	if (!(info->capabilities & RTL931X_STACK_PEER_CAP_SET_DELEGATED)) {
		NL_SET_ERR_MSG_MOD(extack,
				   "peer does not support delegated user ports");
		return -EOPNOTSUPP;
	}
	if (!reps) {
		err = rtl931x_stack_reps_create(priv, &inventory, &reps);
		if (err)
			return err;
		created = true;
	} else if (!rtl931x_stack_reps_info_matches(reps, &inventory)) {
		rtl931x_stack_reps_deactivate(reps);
		NL_SET_ERR_MSG_MOD(extack, "peer port inventory changed");
		return -ESTALE;
	}

	reps->remote_delegation_possible = !!info->delegated_port_mask;
	if (reps->published)
		rtl931x_stack_reps_deactivate(reps);
	err = rtl931x_stack_local_remote_matrices(priv,
						  info->user_port_mask);
	if (err) {
		int rollback_err;

		rollback_err = rtl931x_stack_local_remote_matrices(priv, 0);
		if (created && !rollback_err)
			rtl931x_stack_reps_destroy(reps);
		NL_SET_ERR_MSG_MOD(extack,
				   "failed to isolate peer source ports");
		return rollback_err ? -EIO : err;
	}
	err = rtl931x_stack_peer_set_delegated(priv, true);
	if (err) {
		if (rtl931x_stack_rpc_uncertain(err))
			reps->remote_delegation_possible = true;
		else if (created && !reps->remote_delegation_possible) {
			int rollback_err;

			rollback_err =
				rtl931x_stack_local_remote_matrices(priv, 0);
			if (!rollback_err)
				rtl931x_stack_reps_destroy(reps);
			else
				err = -EIO;
		}
		NL_SET_ERR_MSG_MOD(extack, "failed to delegate peer ports");
		return err;
	}
	reps->remote_delegation_possible = true;
	err = rtl931x_stack_reps_bridge_replay(reps);
	if (err) {
		reps->bridge_dirty = true;
		NL_SET_ERR_MSG_MOD(extack,
				   "failed to replay peer bridge state");
		return err;
	}
	rtl931x_stack_local_bridge_replay(priv);

	err = rtl931x_stack_peer_get_inventory(priv, &inventory, extack);
	if (err)
		return err;
	if (!rtl931x_stack_reps_info_matches(reps, &inventory) ||
	    info->delegated_port_mask != info->user_port_mask) {
		NL_SET_ERR_MSG_MOD(extack,
				   "peer delegation acknowledgment is inconsistent");
		return -EPROTO;
	}

	return rtl931x_stack_reps_publish(reps, info, link_epoch);
}

void rtl931x_stack_reps_link_change(struct rtl838x_switch_priv *priv,
				    int port, bool up)
{
	struct rtl931x_stack_context *stack = &priv->stack;
	struct rtl_otto_tagger_data *tagger_data = priv->ds->tagger_data;
	struct rtl_otto_remote_port_map *map;
	struct rtl931x_stack_rep_priv *rep;
	struct net_device *dev = NULL;
	int remote_port;

	if (!READ_ONCE(stack->enabled) || port != READ_ONCE(stack->port))
		return;

	if (!up)
		atomic_inc(&stack->fabric_link_epoch);
	WRITE_ONCE(stack->fabric_link_up, up);
	if (up) {
		if (READ_ONCE(stack->reps_recovery_pending))
			mod_delayed_work(system_wq, &stack->reps_recovery_work,
					 0);
		return;
	}
	WRITE_ONCE(stack->reps_recovery_attempts, 0);
	if (!tagger_data)
		return;

	rcu_read_lock();
	map = rcu_dereference(tagger_data->remote_ports);
	if (!map || map->fabric_port != port)
		goto out_unlock;

	for (remote_port = 0; remote_port < RTL931X_STACK_MAX_PORTS;
	     remote_port++) {
		dev = map->ports[remote_port];
		if (dev)
			break;
	}
	if (!dev || !rtl931x_stack_rep_is_ours(dev))
		goto out_unlock;

	rep = netdev_priv(dev);
	if (!READ_ONCE(stack->reps_desired))
		goto out_unlock;
	WRITE_ONCE(stack->reps_last_error, -ENOLINK);
	WRITE_ONCE(stack->reps_recovery_pending, true);
	if (!READ_ONCE(rep->reps->active))
		goto out_unlock;
	rtl931x_stack_reps_fence(rep->reps);

out_unlock:
	rcu_read_unlock();
}

int rtl931x_stack_reps_set(struct rtl838x_switch_priv *priv, bool enabled,
			   struct netlink_ext_ack *extack)
{
	struct rtl931x_stack_context *stack = &priv->stack;
	int err;

	ASSERT_RTNL();

	if (!stack->registered || !stack->enabled || !stack->saved_valid ||
	    stack->state != RTL931X_STACK_STATE_CONFIGURED)
		return -ENOTCONN;
	if (stack->member_id != stack->master_id)
		return enabled ? -EPERM : 0;
	if (!stack->talk_conduit)
		return -EOPNOTSUPP;

	if (enabled)
		WRITE_ONCE(stack->reps_desired, true);
	err = enabled ? rtl931x_stack_reps_enable(priv, extack) :
			rtl931x_stack_reps_disable(priv, extack);
	WRITE_ONCE(stack->reps_last_error, err);

	return err;
}

void rtl931x_stack_reps_get_status(struct rtl838x_switch_priv *priv,
				   bool *active, bool *published,
				   bool *fenced)
{
	struct rtl931x_stack_reps *reps = priv->stack.reps;

	ASSERT_RTNL();
	*active = reps && READ_ONCE(reps->active);
	*published = reps && READ_ONCE(reps->published);
	*fenced = reps && READ_ONCE(reps->fenced);
}

void rtl931x_stack_reps_unregister(struct rtl838x_switch_priv *priv)
{
	struct rtl931x_stack_reps *reps = priv->stack.reps;
	int err;

	ASSERT_RTNL();

	if (!reps)
		return;

	err = rtl931x_stack_reps_disable(priv, NULL);
	if (err) {
		dev_err(priv->dev,
			"failed to undelegate peer ports during removal: %pe\n",
			ERR_PTR(err));
		reps = priv->stack.reps;
		if (reps) {
			rtl931x_stack_reps_deactivate(reps);
			rtl931x_stack_reps_destroy(reps);
		}
	}
}

int rtl931x_stack_reps_init(void)
{
	int err;

	err = register_netdevice_notifier(&rtl931x_stack_rep_netdev_nb);
	if (err)
		return err;
	err = register_switchdev_blocking_notifier(
					&rtl931x_stack_rep_switchdev_nb);
	if (err)
		unregister_netdevice_notifier(&rtl931x_stack_rep_netdev_nb);

	return err;
}

void rtl931x_stack_reps_exit(void)
{
	unregister_switchdev_blocking_notifier(&rtl931x_stack_rep_switchdev_nb);
	unregister_netdevice_notifier(&rtl931x_stack_rep_netdev_nb);
}

// SPDX-License-Identifier: GPL-2.0-or-later OR MIT

/* Virtual source ports for the ports of a bridge that are not switch ports.
 *
 * A frame the CPU sends through the EDMA with a source port from the virtual
 * range and no destination gets the PPE's ingress lookup as if it had been
 * received on that port. On IPQ8074 the L3 port, MRU and MTU tables have 256
 * entries, so a virtual port can be given the bridge's VSI and routed from; the
 * L2 port tables (bridge control, VLAN, FDB port, isolation) have eight, so a
 * virtual port is never a member of a bridge's L2 domain, and bridged traffic
 * stays with the bridge.
 */

#include <linux/debugfs.h>
#include <linux/if_arp.h>
#include <linux/if_bridge.h>
#include <linux/if_vlan.h>
#include <linux/netdevice.h>
#include <linux/seq_file.h>

#include "qca_ppe.h"

/**
 * struct ppe_vp - one virtual source port
 * @dev: the bridge port it stands for
 * @br: the bridge it is a port of
 * @dev_tracker: reference on @dev
 * @br_tracker: reference on @br
 * @rcu: defers the free past readers of the port table
 * @id: the port number in the PPE
 * @vsi: the bridge's VSI the port is classified into
 */
struct ppe_vp {
	struct net_device *dev;
	struct net_device *br;
	netdevice_tracker dev_tracker;
	netdevice_tracker br_tracker;
	struct rcu_head rcu;
	u8 id;
	u8 vsi;
};

/* A port stands for the netdevs the bridge forwards for the host's own
 * stations or peers, never for one of the switch's ports, a device stacked on
 * a bridge or switch port, or the driver's own shaper.
 */
static bool ppe_vp_eligible(struct qca_ppe_priv *priv, struct net_device *dev)
{
	struct dsa_port *dp;

	if (!net_eq(dev_net(dev), &init_net) || dev->type != ARPHRD_ETHER ||
	    is_vlan_dev(dev) || netif_is_bridge_master(dev) ||
	    netdev_uses_dsa(dev))
		return false;

	dp = dsa_port_from_netdev(dev);
	if (!IS_ERR(dp))
		return false;

	return true;
}

static struct ppe_vp *ppe_vp_find(struct qca_ppe_priv *priv,
				  const struct net_device *dev)
{
	unsigned int id;

	lockdep_assert_held(&priv->flow_lock);

	for (id = priv->data->first_vport; id < PPE_MAX_VPORT; id++) {
		struct ppe_vp *vp = rcu_dereference_protected(priv->vp[id],
				lockdep_is_held(&priv->flow_lock));

		if (vp && vp->dev == dev)
			return vp;
	}

	return NULL;
}

static void ppe_vp_free_rcu(struct rcu_head *head)
{
	struct ppe_vp *vp = container_of(head, struct ppe_vp, rcu);

	netdev_put(vp->br, &vp->br_tracker);
	netdev_put(vp->dev, &vp->dev_tracker);
	kfree(vp);
}

/* The port's VSI goes first into the L3 port table, which is what lets the
 * lookup classify a frame at all; the frame size limits are what a switch port
 * is given when it is created, and a frame over them goes to the CPU.
 */
static void ppe_vp_hw_set(struct qca_ppe_priv *priv, unsigned int id, u32 vsi)
{
	ppe_port_mru_mtu_set(priv, id, PPE_MAX_FRAME_SIZE);
	ppe_port_vsi_set(priv, id, vsi);
}

static void ppe_vp_destroy(struct qca_ppe_priv *priv, struct ppe_vp *vp)
{
	lockdep_assert_held(&priv->flow_lock);
	lockdep_assert_held(&priv->vlan_lock);

	RCU_INIT_POINTER(priv->vp[vp->id], NULL);
	/* No frame is sent from the port once the table says nothing names
	 * it, and the number is not handed on while frames sent from it may
	 * still be in the rings: ids are taken round robin.
	 */
	ppe_port_vsi_set(priv, vp->id, PPE_VSI_INVALID);
	call_rcu(&vp->rcu, ppe_vp_free_rcu);
}

/* A port that cannot get a virtual port keeps the ordinary path, so every
 * failure here is silent apart from the debugfs listing.
 */
static void ppe_vp_create(struct qca_ppe_priv *priv, struct net_device *dev,
			  struct net_device *br, u32 vsi)
{
	unsigned int first = priv->data->first_vport;
	unsigned int n = PPE_MAX_VPORT - first, i, id;
	struct ppe_vp *vp;

	lockdep_assert_held(&priv->flow_lock);
	lockdep_assert_held(&priv->vlan_lock);

	if (!first || ppe_vp_find(priv, dev))
		return;

	for (i = 0; i < n; i++) {
		id = first + (priv->vp_next + i) % n;
		if (!rcu_access_pointer(priv->vp[id]))
			break;
	}
	if (i == n)
		return;

	vp = kzalloc(sizeof(*vp), GFP_KERNEL);
	if (!vp)
		return;

	vp->id = id;
	vp->vsi = vsi;
	vp->dev = dev;
	vp->br = br;
	netdev_hold(dev, &vp->dev_tracker, GFP_KERNEL);
	netdev_hold(br, &vp->br_tracker, GFP_KERNEL);

	ppe_vp_hw_set(priv, id, vsi);
	rcu_assign_pointer(priv->vp[id], vp);
	priv->vp_next = (id - first + 1) % n;
}

static void ppe_vp_remove_dev(struct qca_ppe_priv *priv, struct net_device *dev)
{
	struct ppe_vp *vp = ppe_vp_find(priv, dev);

	if (vp)
		ppe_vp_destroy(priv, vp);
}

/* A non-switch port joined a bridge. Without a VSI the bridge has no DSA port
 * and nothing is offloaded for it; the join of its first one comes back here
 * through ppe_vp_bridge_up().
 */
static void ppe_vp_add_dev(struct qca_ppe_priv *priv, struct net_device *dev,
			   struct net_device *br)
{
	struct qca_ppe_bridge_vsi *bvsi;

	if (priv->vp_stopped || !ppe_vp_eligible(priv, dev))
		return;

	bvsi = bridge_vsi_find(priv, br);
	if (bvsi)
		ppe_vp_create(priv, dev, br, bvsi->vsi);
}

/**
 * ppe_vp_bridge_up - give the non-switch ports of a bridge their virtual ports
 * @priv: the switch
 * @br_dev: the bridge, which now has a switch port and so a VSI
 *
 * Called with both locks held and the RTNL, from a switch port's bridge join.
 */
void ppe_vp_bridge_up(struct qca_ppe_priv *priv, struct net_device *br_dev)
{
	struct net_device *lower;
	struct list_head *iter;

	ASSERT_RTNL();

	netdev_for_each_lower_dev(br_dev, lower, iter)
		ppe_vp_add_dev(priv, lower, br_dev);
}

/**
 * ppe_vp_bridge_down - take the virtual ports of a bridge away
 * @priv: the switch
 * @br_dev: the bridge, whose last switch port has left or whose VSI is going
 *
 * Called with both locks held.
 */
void ppe_vp_bridge_down(struct qca_ppe_priv *priv, struct net_device *br_dev)
{
	unsigned int id;

	lockdep_assert_held(&priv->flow_lock);

	for (id = priv->data->first_vport; id < PPE_MAX_VPORT; id++) {
		struct ppe_vp *vp = rcu_dereference_protected(priv->vp[id],
				lockdep_is_held(&priv->flow_lock));

		if (vp && vp->br == br_dev)
			ppe_vp_destroy(priv, vp);
	}
}

static int ppe_vp_netdev_event(struct notifier_block *nb, unsigned long event,
			       void *ptr)
{
	struct qca_ppe_priv *priv = container_of(nb, struct qca_ppe_priv,
						 vp_nb);
	struct net_device *dev = netdev_notifier_info_to_dev(ptr);
	struct netdev_notifier_changeupper_info *info = ptr;

	if (event == NETDEV_CHANGEUPPER) {
		if (!netif_is_bridge_master(info->upper_dev))
			return NOTIFY_DONE;

		guard(mutex)(&priv->flow_lock);
		guard(mutex)(&priv->vlan_lock);

		if (info->linking)
			ppe_vp_add_dev(priv, dev, info->upper_dev);
		else
			ppe_vp_remove_dev(priv, dev);
	} else if (event == NETDEV_UNREGISTER) {
		guard(mutex)(&priv->flow_lock);
		guard(mutex)(&priv->vlan_lock);

		ppe_vp_remove_dev(priv, dev);
	}

	return NOTIFY_DONE;
}

/**
 * ppe_vp_rx - deliver a frame the switch returned to the port it was sent from
 * @ds: the switch
 * @skb: the frame, with skb->data at its Ethernet header
 * @port: the virtual source port the frame carries
 *
 * The frame is the one the CPU sent, unchanged, back for the host to forward:
 * a size limit or an exception of the lookup turned it away. It is received on
 * the port's netdev, so the bridge, the firewall and the routing take it as
 * they would have when it first arrived. Called from the EDMA NAPI poll.
 *
 * Return: false, with @skb untouched, when @port names no virtual port.
 */
bool ppe_vp_rx(struct dsa_switch *ds, struct sk_buff *skb, u32 port)
{
	struct qca_ppe_priv *priv = ds_to_priv(ds);
	struct ppe_vp *vp;

	if (port >= PPE_MAX_VPORT)
		return false;

	vp = rcu_dereference_bh(priv->vp[port]);
	if (!vp || !netif_running(vp->dev))
		return false;

	skb->dev = vp->dev;
	skb_record_rx_queue(skb, 0);
	skb->protocol = eth_type_trans(skb, vp->dev);
	netif_receive_skb(skb);

	return true;
}

static int ppe_vp_show(struct seq_file *s, void *data)
{
	struct qca_ppe_priv *priv = s->private;
	unsigned int id, used = 0;

	guard(mutex)(&priv->flow_lock);

	seq_puts(s, "vport netdev bridge vsi\n");

	for (id = priv->data->first_vport; id < PPE_MAX_VPORT; id++) {
		struct ppe_vp *vp = rcu_dereference_protected(priv->vp[id],
				lockdep_is_held(&priv->flow_lock));

		if (!vp)
			continue;

		used++;
		seq_printf(s, "%-5u %s %s %u\n", id, vp->dev->name, vp->br->name,
			   vp->vsi);
	}

	seq_printf(s, "used %u of %u\n", used,
		   priv->data->first_vport ?
		   PPE_MAX_VPORT - priv->data->first_vport : 0);

	return 0;
}
DEFINE_SHOW_ATTRIBUTE(ppe_vp);

void ppe_vp_debugfs_init(struct qca_ppe_priv *priv)
{
	debugfs_create_file("vports", 0400, priv->debugfs, priv, &ppe_vp_fops);
}

int ppe_vp_init(struct qca_ppe_priv *priv)
{
	priv->vp_nb.notifier_call = ppe_vp_netdev_event;

	return register_netdevice_notifier(&priv->vp_nb);
}

/**
 * ppe_vp_stop - take every virtual port away and stop making new ones
 * @priv: the switch
 *
 * The notifier is gone and no port is left on return; the switch's own
 * teardown, which follows, finds nothing of this file to undo.
 */
void ppe_vp_stop(struct qca_ppe_priv *priv)
{
	unsigned int id;

	unregister_netdevice_notifier(&priv->vp_nb);

	guard(mutex)(&priv->flow_lock);
	guard(mutex)(&priv->vlan_lock);

	priv->vp_stopped = true;
	for (id = priv->data->first_vport; id < PPE_MAX_VPORT; id++) {
		struct ppe_vp *vp = rcu_dereference_protected(priv->vp[id],
				lockdep_is_held(&priv->flow_lock));

		if (vp)
			ppe_vp_destroy(priv, vp);
	}
}

void ppe_vp_exit(struct qca_ppe_priv *priv)
{
	/* The ports' callbacks are code of this module. */
	rcu_barrier();
}

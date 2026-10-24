// SPDX-License-Identifier: GPL-2.0-only
/*
 * ZTE zx279128s Ethernet switch: hardware LAN switching
 *
 * Copyright (C) 2026 Navid Ghahremani <ghahramani.navid@gmail.com>
 */

#include <linux/bitfield.h>
#include <linux/etherdevice.h>
#include <linux/if_bridge.h>
#include <linux/io.h>
#include <linux/iopoll.h>
#include <linux/rtnetlink.h>
#include <net/switchdev.h>

#include "zx279128s-eth.h"

/* ---- Hardware LAN switching ---------------------------------------- */

/*
 * The bridge block forwards a unicast frame from one switch port to another
 * when its source table knows the destination and both ports are members
 * of the port VLAN (1).  Normally learning is off and the VLAN table empty,
 * so every frame goes to the Linux bridge.  Ports of the same Linux bridge
 * switch between each other in hardware, as long as the bridge does not
 * filter VLANs and the port forwards, learns, and is neither isolated nor
 * locked (zx_sw_bridge()); any other port stays on the CPU path:
 * - VLAN 1 has them as untagged members, each may send only to the other
 *   ports of its bridge (egress mask 0 for all other ports, so nothing is
 *   switched to or from a port outside the bridge), and only they learn;
 * - the IPv4 and IPv6 ethertype traps are off (zx_update_traps()).
 * Only known unicast is switched.  Frames for the router, broadcast,
 * multicast and unknown unicast reach the CPU alone, and the Linux bridge
 * floods them, also to the hosts on its other ports (Wi-Fi).  So received
 * frames are never marked as forwarded already (offload_fwd_mark).
 * A host that moves loses its hardware entry: the entries of a port are
 * dropped when its link goes down, and a MAC is dropped as soon as the
 * Linux bridge learns it on a port that is not a switch port.
 */
static int zx_sbrg_ind(struct zx_eth_adapter *adapter, u32 cmd)
{
	u32 val;

	writel(cmd, adapter->base + ZX_SBRG_IND_CMD);
	return readl_poll_timeout_atomic(adapter->base + ZX_SBRG_IND_DONE, val,
					 val & 1, 0, 1000);
}

static void zx_sbrg_flush(struct zx_eth_adapter *adapter, u8 brports)
{
	void __iomem *reg = adapter->base + ZX_SBRG_PORT_CTRL;
	u32 val = readl(reg) & ~(ZX_SBRG_FLUSH_PORTS | ZX_SBRG_FLUSH);

	writel(val | FIELD_PREP(ZX_SBRG_FLUSH_PORTS, brports) | ZX_SBRG_FLUSH, reg);
	usleep_range(1000, 2000);
	writel(val, reg);
}

/* Remove one MAC from the source table: scan the four RAMs */
static void zx_sbrg_fdb_del(struct zx_eth_adapter *adapter, const u8 *mac)
{
	static const int sizes[4] = { 1024, 256, 512, 512 };
	void __iomem *base = adapter->base;
	u32 hi = mac[0] << 16 | mac[1] << 8 | mac[2];
	u32 lo = mac[3] << 24 | mac[4] << 16 | mac[5] << 8;
	int n = sizes[readl(base + ZX_SBRG_TABLE_SEL) & 3];
	int ram, i;

	for (ram = 0; ram < 4; ram++) {
		for (i = 0; i < n; i++) {
			if (zx_sbrg_ind(adapter, ZX_SBRG_IND_READ | ram << 22 | i))
				return;
			if (!(readl(base + ZX_SBRG_IND_DATA(2)) & 0xf0) ||
			    (readl(base + ZX_SBRG_IND_DATA(1)) & 0xffffff) != hi ||
			    (readl(base + ZX_SBRG_IND_DATA(0)) & 0xffffff00) != lo)
				continue;
			if (zx_sbrg_ind(adapter, ram << 22 | i))
				return;
			writel(0, base + ZX_SBRG_IND_DATA(2));
			writel(0, base + ZX_SBRG_IND_DATA(1));
			writel(0, base + ZX_SBRG_IND_DATA(0));
			adapter->sw_fdb_deleted++;
		}
	}
}

/* the bridge a port switches in, or NULL if it stays on the CPU path */
static struct net_device *zx_sw_bridge(struct net_device *dev)
{
	struct net_device *br;

	ASSERT_RTNL();
	if (!dev || !netif_is_bridge_port(dev))
		return NULL;
	br = netdev_master_upper_dev_get(dev);
	if (!br || br_vlan_enabled(br) ||
	    br_port_get_stp_state(dev) != BR_STATE_FORWARDING ||
	    !br_port_flag_is_set(dev, BR_LEARNING) ||
	    br_port_flag_is_set(dev, BR_ISOLATED) ||
	    br_port_flag_is_set(dev, BR_PORT_LOCKED))
		return NULL;
	return br;
}

static void zx_sw_apply(struct zx_eth_adapter *adapter)
{
	struct net_device *br[ZX_NUM_PORTS] = {};
	void __iomem *base = adapter->base;
	u8 egress[ZX_NUM_PORTS] = {};
	u8 members = 0;
	u32 vlan = 0;
	int i, j;

	lockdep_assert_held(&adapter->sw_lock);

	rtnl_lock();
	for (i = 0; adapter->sw_ready && i < ZX_NUM_PORTS; i++)
		br[i] = zx_sw_bridge(adapter->ports[i]);
	rtnl_unlock();

	for (i = 0; i < ZX_NUM_PORTS; i++)
		for (j = 0; j < ZX_NUM_PORTS; j++)
			if (j != i && br[i] && br[j] == br[i])
				egress[i] |= BIT(ZX_BRPORT(j));
	for (i = 0; i < ZX_NUM_PORTS; i++) {
		if (!egress[i]) {
			br[i] = NULL;
			continue;
		}
		members |= BIT(ZX_BRPORT(i));
		vlan |= ZX_SBRG_VLAN_UNTAG(ZX_BRPORT(i));
	}

	spin_lock_bh(&adapter->sw_fdb_lock);
	memcpy(adapter->sw_bridge, br, sizeof(br));
	spin_unlock_bh(&adapter->sw_fdb_lock);

	/* traps back on before the tables go, off once they are in place */
	if (!members) {
		WRITE_ONCE(adapter->sw_members, 0);
		zx_update_traps(adapter);
	}

	/* no learning while the tables change, then start empty */
	writel(0, base + ZX_SBRG_LEARN);
	if (zx_sbrg_ind(adapter, ZX_SBRG_MEM_VLAN << 22 | 1))
		dev_warn(adapter->dev, "bridge VLAN table access timed out\n");
	else
		writel(members ? vlan | ZX_SBRG_VLAN_VALID : 0,
		       base + ZX_SBRG_IND_DATA(0));
	/* the reset value lets a port send to every other port */
	for (i = 0; i < ZX_NUM_PORTS; i++)
		writel(members ? egress[i] : (u8)~BIT(ZX_BRPORT(i)),
		       base + ZX_SBRG_EGRESS(ZX_BRPORT(i)));
	zx_sbrg_flush(adapter, 0xff);
	writel(members, base + ZX_SBRG_LEARN);
	if (members) {
		WRITE_ONCE(adapter->sw_members, members);
		zx_update_traps(adapter);
	}
	dev_dbg(adapter->dev, "hardware LAN switching %s (bridge ports %#x)\n",
		members ? "on" : "off", members);
}

void zx_sw_work(struct work_struct *work)
{
	struct zx_eth_adapter *adapter = container_of(work, struct zx_eth_adapter,
						      sw_work);
	u8 macs[ZX_SW_FDB_QUEUE][ETH_ALEN];
	bool recompute, overflow;
	unsigned int n, i;
	u8 flush;

	spin_lock_bh(&adapter->sw_fdb_lock);
	recompute = adapter->sw_recompute;
	overflow = adapter->sw_fdb_overflow;
	flush = adapter->sw_flush;
	n = adapter->sw_fdb_count;
	memcpy(macs, adapter->sw_fdb_mac, n * ETH_ALEN);
	adapter->sw_recompute = false;
	adapter->sw_fdb_overflow = false;
	adapter->sw_flush = 0;
	adapter->sw_fdb_count = 0;
	spin_unlock_bh(&adapter->sw_fdb_lock);

	mutex_lock(&adapter->sw_lock);
	if (recompute) {
		zx_sw_apply(adapter);
	} else if (READ_ONCE(adapter->sw_members)) {
		if (overflow)
			flush = 0xff;
		if (flush)
			zx_sbrg_flush(adapter, flush);
		for (i = 0; !overflow && i < n; i++)
			zx_sbrg_fdb_del(adapter, macs[i]);
	}
	mutex_unlock(&adapter->sw_lock);
}

void zx_sw_port_down(struct zx_eth_adapter *adapter, int port)
{
	if (!READ_ONCE(adapter->sw_members))
		return;
	spin_lock_bh(&adapter->sw_fdb_lock);
	adapter->sw_flush |= BIT(ZX_BRPORT(port));
	spin_unlock_bh(&adapter->sw_fdb_lock);
	schedule_work(&adapter->sw_work);
}

static bool zx_sw_is_port(struct zx_eth_adapter *adapter,
			  const struct net_device *dev)
{
	int i;

	for (i = 0; i < ZX_NUM_PORTS; i++)
		if (adapter->ports[i] == dev)
			return true;
	return false;
}

static void zx_sw_recompute(struct zx_eth_adapter *adapter)
{
	spin_lock_bh(&adapter->sw_fdb_lock);
	adapter->sw_recompute = true;
	spin_unlock_bh(&adapter->sw_fdb_lock);
	schedule_work(&adapter->sw_work);
}

/* a switch port joined or left a bridge */
static int zx_sw_netdev_event(struct notifier_block *nb, unsigned long event,
			      void *ptr)
{
	struct zx_eth_adapter *adapter = container_of(nb, struct zx_eth_adapter,
						      sw_netdev_nb);
	struct net_device *dev = netdev_notifier_info_to_dev(ptr);

	if (event == NETDEV_CHANGEUPPER && zx_sw_is_port(adapter, dev))
		zx_sw_recompute(adapter);
	return NOTIFY_DONE;
}

static bool zx_sw_dev_check(const struct net_device *dev)
{
	return zx_eth_is_port(dev);
}

/* something that decides whether the port switches (zx_sw_bridge()) */
static int zx_sw_port_attr_set(struct net_device *dev, const void *ctx,
			       const struct switchdev_attr *attr,
			       struct netlink_ext_ack *extack)
{
	struct zx_eth_priv *priv = netdev_priv(dev);

	switch (attr->id) {
	case SWITCHDEV_ATTR_ID_PORT_PRE_BRIDGE_FLAGS:
		/* the CPU path handles what the switch does not */
		return 0;
	case SWITCHDEV_ATTR_ID_PORT_STP_STATE:
	case SWITCHDEV_ATTR_ID_PORT_BRIDGE_FLAGS:
	case SWITCHDEV_ATTR_ID_BRIDGE_VLAN_FILTERING:
		zx_sw_recompute(priv->adapter);
		return 0;
	default:
		return -EOPNOTSUPP;
	}
}

static int zx_sw_switchdev_blocking_event(struct notifier_block *nb,
					  unsigned long event, void *ptr)
{
	struct net_device *dev = switchdev_notifier_info_to_dev(ptr);
	int err;

	if (event != SWITCHDEV_PORT_ATTR_SET)
		return NOTIFY_DONE;
	err = switchdev_handle_port_attr_set(dev, ptr, zx_sw_dev_check,
					     zx_sw_port_attr_set);
	return notifier_from_errno(err);
}

/* the Linux bridge learned a MAC; if it is on a non-switch port of a
 * switching bridge (Wi-Fi), the host left its switch port
 */
static int zx_sw_switchdev_event(struct notifier_block *nb,
				 unsigned long event, void *ptr)
{
	struct zx_eth_adapter *adapter = container_of(nb, struct zx_eth_adapter,
						      sw_switchdev_nb);
	struct net_device *dev = switchdev_notifier_info_to_dev(ptr);
	struct switchdev_notifier_fdb_info *fdb;
	struct net_device *br;
	bool queued = false;
	int i;

	if (event != SWITCHDEV_FDB_ADD_TO_DEVICE || zx_sw_is_port(adapter, dev))
		return NOTIFY_DONE;
	fdb = container_of(ptr, struct switchdev_notifier_fdb_info, info);
	if (fdb->is_local)
		return NOTIFY_DONE;

	rcu_read_lock();
	br = netdev_master_upper_dev_get_rcu(dev);
	rcu_read_unlock();
	if (!br)
		return NOTIFY_DONE;

	spin_lock_bh(&adapter->sw_fdb_lock);
	for (i = 0; i < ZX_NUM_PORTS; i++) {
		if (adapter->sw_bridge[i] != br)
			continue;
		if (adapter->sw_fdb_count < ZX_SW_FDB_QUEUE)
			memcpy(adapter->sw_fdb_mac[adapter->sw_fdb_count++],
			       fdb->addr, ETH_ALEN);
		else
			adapter->sw_fdb_overflow = true;
		queued = true;
		break;
	}
	spin_unlock_bh(&adapter->sw_fdb_lock);
	if (queued)
		schedule_work(&adapter->sw_work);
	return NOTIFY_DONE;
}

/* Follow the bridges the ports are in; called at the end of probe */
void zx_sw_init(struct zx_eth_adapter *adapter)
{
	int ret;

	adapter->sw_netdev_nb.notifier_call = zx_sw_netdev_event;
	adapter->sw_switchdev_nb.notifier_call = zx_sw_switchdev_event;
	adapter->sw_switchdev_blocking_nb.notifier_call =
		zx_sw_switchdev_blocking_event;
	ret = register_netdevice_notifier(&adapter->sw_netdev_nb);
	if (ret)
		goto err;
	ret = register_switchdev_notifier(&adapter->sw_switchdev_nb);
	if (ret)
		goto err_netdev;
	ret = register_switchdev_blocking_notifier(&adapter->sw_switchdev_blocking_nb);
	if (ret)
		goto err_switchdev;
	adapter->sw_ready = true;
	return;

err_switchdev:
	unregister_switchdev_notifier(&adapter->sw_switchdev_nb);
err_netdev:
	unregister_netdevice_notifier(&adapter->sw_netdev_nb);
err:
	dev_warn(adapter->dev,
		 "no bridge notifications: hardware LAN switching unavailable (%d)\n",
		 ret);
}

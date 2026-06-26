// SPDX-License-Identifier: GPL-2.0-only

#include <linux/if_vlan.h>
#include <linux/inetdevice.h>
#include <linux/notifier.h>
#include <linux/rhashtable.h>
#include <net/arp.h>
#include <net/fib_notifier.h>
#include <net/ip6_fib.h>
#include <net/netevent.h>
#include <net/nexthop.h>
#include <uapi/linux/rtnetlink.h>

#include "l3.h"
#include "rtl-otto.h"

struct otto_l3_net_event_work {
	struct work_struct work;
	struct rtl838x_switch_priv *priv;
	u64 mac;
	u32 gw_addr;
};

struct otto_l3_fib_event_work {
	struct work_struct work;
	union {
		struct fib_entry_notifier_info fen_info;
		struct fib6_entry_notifier_info fen6_info;
		struct fib_rule_notifier_info fr_info;
	};
	struct rtl838x_switch_priv *priv;
	bool is_fib6;
	unsigned long event;
};

struct otto_l3_walk_data {
	struct rtl838x_switch_priv *priv;
	int port;
};

static int otto_l3_port_lower_walk(struct net_device *lower, struct netdev_nested_priv *_priv)
{
	struct otto_l3_walk_data *data = (struct otto_l3_walk_data *)_priv->data;
	struct rtl838x_switch_priv *priv = data->priv;
	int ret = 0;
	int index;

	index = rtl83xx_port_is_under(lower, priv);
	data->port = index;
	if (index >= 0) {
		pr_debug("Found DSA-port, index %d\n", index);
		ret = 1;
	}

	return ret;
}

static int otto_l3_port_dev_lower_find(struct net_device *dev, struct rtl838x_switch_priv *priv)
{
	struct otto_l3_walk_data data;
	struct netdev_nested_priv _priv;

	data.priv = priv;
	data.port = 0;
	_priv.data = (void *)&data;

	netdev_walk_all_lower_dev(dev, otto_l3_port_lower_walk, &_priv);

	return data.port;
}

/* On the RTL93xx, an L3 termination endpoint MAC address on which the router waits
 * for packets to be routed needs to be allocated.
 */
static int otto_l3_alloc_router_mac(struct rtl838x_switch_priv *priv, u64 mac)
{
	int free_mac = -1;
	struct rtl93xx_rt_mac m;

	mutex_lock(&priv->reg_mutex);
	for (int i = 0; i < MAX_ROUTER_MACS; i++) {
		priv->r->get_l3_router_mac(i, &m);
		if (free_mac < 0 && !m.valid) {
			free_mac = i;
			continue;
		}
		if (m.valid && m.mac == mac) {
			free_mac = i;
			break;
		}
	}

	if (free_mac < 0) {
		pr_err("No free router MACs, cannot offload\n");
		mutex_unlock(&priv->reg_mutex);
		return -1;
	}

	m.valid = true;
	m.mac = mac;
	m.p_type = 0; /* An individual port, not a trunk port */
	m.p_id = 0x3f;			/* Listen on any port */
	m.p_id_mask = 0;
	m.vid = 0;			/* Listen on any VLAN... */
	m.vid_mask = 0;			/* ... so mask needs to be 0 */
	m.mac_mask = 0xffffffffffffULL;	/* We want an exact match of the interface MAC */
	m.action = L3_FORWARD;		/* Route the packet */
	priv->r->set_l3_router_mac(free_mac, &m);

	mutex_unlock(&priv->reg_mutex);

	return 0;
}

static int otto_l3_alloc_egress_intf(struct rtl838x_switch_priv *priv, u64 mac, int vlan)
{
	int free_mac = -1;
	struct rtl838x_l3_intf intf;
	u64 m;

	mutex_lock(&priv->reg_mutex);
	for (int i = 0; i < MAX_SMACS; i++) {
		m = priv->r->get_l3_egress_mac(L3_EGRESS_DMACS + i);
		if (free_mac < 0 && !m) {
			free_mac = i;
			continue;
		}
		if (m == mac) {
			mutex_unlock(&priv->reg_mutex);
			return i;
		}
	}

	if (free_mac < 0) {
		pr_err("No free egress interface, cannot offload\n");
		return -1;
	}

	/* Set up default egress interface 1 */
	intf.vid = vlan;
	intf.smac_idx = free_mac;
	intf.ip4_mtu_id = 1;
	intf.ip6_mtu_id = 1;
	intf.ttl_scope = 1; /* TTL */
	intf.hl_scope = 1;  /* Hop Limit */
	intf.ip4_icmp_redirect = intf.ip6_icmp_redirect = 2;  /* FORWARD */
	intf.ip4_pbr_icmp_redirect = intf.ip6_pbr_icmp_redirect = 2; /* FORWARD; */
	priv->r->set_l3_egress_intf(free_mac, &intf);

	priv->r->set_l3_egress_mac(L3_EGRESS_DMACS + free_mac, mac);

	mutex_unlock(&priv->reg_mutex);

	return free_mac;
}

/* Updates an L3 next hop entry in the ROUTING table */
static int otto_l3_nexthop_update(struct rtl838x_switch_priv *priv, __be32 ip_addr, u64 mac)
{
	struct otto_l3_ctrl *ctrl = priv->l3_ctrl;
	struct rhlist_head *tmp, *list;
	struct otto_l3_route *r;

	rcu_read_lock();
	list = rhltable_lookup(&ctrl->routes, &ip_addr, otto_l3_route_ht_params);
	if (!list) {
		rcu_read_unlock();
		return -ENOENT;
	}

	rhl_for_each_entry_rcu(r, tmp, list, linkage) {
		pr_debug("%s: Setting up fwding: ip %pI4, GW mac %016llx\n",
			 __func__, &ip_addr, mac);

		/* Reads the ROUTING table entry associated with the route */
		priv->r->route_read(r->id, r);
		pr_debug("Route with id %d to %pI4 / %d\n", r->id, &r->dst_ip, r->prefix_len);

		r->nh.mac = r->nh.gw = mac;
		r->nh.port = priv->r->port_ignore;
		r->nh.id = r->id;

		/* Do we need to explicitly add a DMAC entry with the route's nh index? */
		if (priv->r->set_l3_egress_mac)
			priv->r->set_l3_egress_mac(r->id, mac);

		/* Update ROUTING table: map gateway-mac and switch-mac id to route id */
		rtl83xx_l2_nexthop_add(priv, &r->nh);

		r->attr.valid = true;
		r->attr.action = ROUTE_ACT_FORWARD;
		r->attr.type = 0;
		r->attr.hit = false; /* Reset route-used indicator */

		/* Add PIE entry with dst_ip and prefix_len */
		r->pr.dip = r->dst_ip;
		r->pr.dip_m = inet_make_mask(r->prefix_len);

		if (r->is_host_route) {
			int slot = priv->r->find_l3_slot(r, false);

			pr_info("%s: Got slot for route: %d\n", __func__, slot);
			priv->r->host_route_write(slot, r);
		} else {
			priv->r->route_write(r->id, r);
			r->pr.fwd_sel = true;
			r->pr.fwd_data = r->nh.l2_id;
			r->pr.fwd_act = PIE_ACT_ROUTE_UC;
		}

		if (priv->r->set_l3_nexthop)
			priv->r->set_l3_nexthop(r->nh.id, r->nh.l2_id, r->nh.if_id);

		if (r->pr.id < 0) {
			r->pr.packet_cntr = rtl83xx_packet_cntr_alloc(priv);
			if (r->pr.packet_cntr >= 0) {
				pr_info("Using packet counter %d\n", r->pr.packet_cntr);
				r->pr.log_sel = true;
				r->pr.log_data = r->pr.packet_cntr;
			}
			priv->r->pie_rule_add(priv, &r->pr);
		} else {
			int pkts = priv->r->packet_cntr_read(r->pr.packet_cntr);

			pr_debug("%s: total packets: %d\n", __func__, pkts);

			priv->r->pie_rule_write(priv, r->pr.id, &r->pr);
		}
	}
	rcu_read_unlock();

	return 0;
}

static int otto_l3_port_ipv4_resolve(struct rtl838x_switch_priv *priv,
				     struct net_device *dev, __be32 ip_addr)
{
	struct neighbour *n = neigh_lookup(&arp_tbl, &ip_addr, dev);
	int err = 0;
	u64 mac;

	if (!n) {
		n = neigh_create(&arp_tbl, &ip_addr, dev);
		if (IS_ERR(n))
			return PTR_ERR(n);
	}

	/* If the neigh is already resolved, then go ahead and
	 * install the entry, otherwise start the ARP process to
	 * resolve the neigh.
	 */
	if (n->nud_state & NUD_VALID) {
		mac = ether_addr_to_u64(n->ha);
		pr_info("%s: resolved mac: %016llx\n", __func__, mac);
		otto_l3_nexthop_update(priv, ip_addr, mac);
	} else {
		pr_info("%s: need to wait\n", __func__);
		neigh_event_send(n, NULL);
	}

	neigh_release(n);

	return err;
}

static void otto_l3_route_remove(struct rtl838x_switch_priv *priv, struct otto_l3_route *r)
{
	struct otto_l3_ctrl *ctrl = priv->l3_ctrl;
	int id;

	if (rhltable_remove(&ctrl->routes, &r->linkage, otto_l3_route_ht_params))
		dev_warn(priv->dev, "Could not remove route\n");

	if (r->is_host_route) {
		id = priv->r->find_l3_slot(r, false);
		pr_debug("%s: Got id for host route: %d\n", __func__, id);
		r->attr.valid = false;
		priv->r->host_route_write(id, r);
		clear_bit(r->id - MAX_ROUTES, priv->host_route_use_bm);
	} else {
		/* If there is a HW representation of the route, delete it */
		if (priv->r->route_lookup_hw) {
			id = priv->r->route_lookup_hw(r);
			pr_info("%s: Got id for prefix route: %d\n", __func__, id);
			r->attr.valid = false;
			priv->r->route_write(id, r);
		}
		clear_bit(r->id, priv->route_use_bm);
	}

	kfree(r);
}

static struct otto_l3_route *otto_l3_host_route_alloc(struct rtl838x_switch_priv *priv, u32 ip)
{
	struct otto_l3_ctrl *ctrl = priv->l3_ctrl;
	struct otto_l3_route *r;
	int idx = 0, err;

	mutex_lock(&priv->reg_mutex);

	idx = find_first_zero_bit(priv->host_route_use_bm, MAX_HOST_ROUTES);
	pr_debug("%s id: %d, ip %pI4\n", __func__, idx, &ip);

	r = kzalloc(sizeof(*r), GFP_KERNEL);
	if (!r) {
		mutex_unlock(&priv->reg_mutex);
		return r;
	}

	/* We require a unique route ID irrespective of whether it is a prefix or host
	 * route (on RTL93xx) as we use this ID to associate a DMAC and next-hop entry
	 */
	r->id = idx + MAX_ROUTES;

	r->gw_ip = ip;
	r->pr.id = -1; /* We still need to allocate a rule in HW */
	r->is_host_route = true;

	err = rhltable_insert(&ctrl->routes, &r->linkage, otto_l3_route_ht_params);
	if (err) {
		pr_err("Could not insert new rule\n");
		mutex_unlock(&priv->reg_mutex);
		goto out_free;
	}

	set_bit(idx, priv->host_route_use_bm);

	mutex_unlock(&priv->reg_mutex);

	return r;

out_free:
	kfree(r);

	return NULL;
}

static struct otto_l3_route *otto_l3_route_alloc(struct rtl838x_switch_priv *priv, u32 ip)
{
	struct otto_l3_ctrl *ctrl = priv->l3_ctrl;
	struct otto_l3_route *r;
	int idx = 0, err;

	mutex_lock(&priv->reg_mutex);

	idx = find_first_zero_bit(priv->route_use_bm, MAX_ROUTES);
	pr_debug("%s id: %d, ip %pI4\n", __func__, idx, &ip);

	r = kzalloc(sizeof(*r), GFP_KERNEL);
	if (!r) {
		mutex_unlock(&priv->reg_mutex);
		return r;
	}

	r->id = idx;
	r->gw_ip = ip;
	r->pr.id = -1; /* We still need to allocate a rule in HW */
	r->is_host_route = false;

	err = rhltable_insert(&ctrl->routes, &r->linkage, otto_l3_route_ht_params);
	if (err) {
		pr_err("Could not insert new rule\n");
		mutex_unlock(&priv->reg_mutex);
		goto out_free;
	}

	set_bit(idx, priv->route_use_bm);

	mutex_unlock(&priv->reg_mutex);

	return r;

out_free:
	kfree(r);

	return NULL;
}

static int otto_l3_fib_check_v4(struct rtl838x_switch_priv *priv,
			     struct fib_entry_notifier_info *info,
			     enum fib_event_type event)
{
	struct net_device *ndev = fib_info_nh(info->fi, 0)->fib_nh_dev;
	int vlan = is_vlan_dev(ndev) ? vlan_dev_vlan_id(ndev) : 0;
	struct fib_nh *nh = fib_info_nh(info->fi, 0);
	char gw_message[32] = "";

	if (nh->fib_nh_gw4)
		snprintf(gw_message, sizeof(gw_message), "via %pI4 ", &nh->fib_nh_gw4);

	dev_info(priv->dev, "%s IPv4 route %pI4/%d %s(VLAN %d, MAC %pM)\n",
		 event == FIB_EVENT_ENTRY_ADD ? "add" : "delete",
		 &info->dst, info->dst_len, gw_message, vlan, ndev->dev_addr);

	if ((info->type == RTN_BROADCAST) || ipv4_is_loopback(info->dst) || !info->dst) {
		dev_warn(priv->dev, "skip loopback/broadcast addresses and default routes\n");
		return -EINVAL;
	}

	return 0;
}

static int otto_l3_fib_add_v4(struct rtl838x_switch_priv *priv,
			 struct fib_entry_notifier_info *info)
{
	struct net_device *ndev = fib_info_nh(info->fi, 0)->fib_nh_dev;
	int vlan = is_vlan_dev(ndev) ? vlan_dev_vlan_id(ndev) : 0;
	struct fib_nh *nh = fib_info_nh(info->fi, 0);
	struct otto_l3_route *route;
	int port;

	if (otto_l3_fib_check_v4(priv, info, FIB_EVENT_ENTRY_ADD))
		return 0;

	port = otto_l3_port_dev_lower_find(ndev, priv);
	if (port < 0) {
		dev_err(priv->dev, "lower interface %s not found\n", ndev->name);
		return -ENODEV;
	}

	/* Allocate route or host-route entry (if hardware supports this) */
	if (info->dst_len == 32 && priv->r->host_route_write)
		route = otto_l3_host_route_alloc(priv, nh->fib_nh_gw4);
	else
		route = otto_l3_route_alloc(priv, nh->fib_nh_gw4);

	if (route)
		dev_info(priv->dev, "route hashtable extended for gw %pI4\n", &nh->fib_nh_gw4);
	else {
		dev_err(priv->dev, "could not extend route hashtable for gw %pI4\n", &nh->fib_nh_gw4);
		return -ENOSPC;
	}

	route->dst_ip = info->dst;
	route->prefix_len = info->dst_len;
	route->nh.rvid = vlan;

	if (priv->r->set_l3_router_mac) {
		u64 mac = ether_addr_to_u64(ndev->dev_addr);

		pr_debug("Local route and router MAC %pM\n", ndev->dev_addr);
		if (otto_l3_alloc_router_mac(priv, mac))
			goto out_free_rt;

		/* vid = 0: Do not care about VID */
		route->nh.if_id = otto_l3_alloc_egress_intf(priv, mac, vlan);
		if (route->nh.if_id < 0)
			goto out_free_rmac;

		if (!nh->fib_nh_gw4) {
			int slot;

			route->nh.mac = mac;
			route->nh.port = priv->r->port_ignore;
			route->attr.valid = true;
			route->attr.action = ROUTE_ACT_TRAP2CPU;
			route->attr.type = 0;

			slot = priv->r->find_l3_slot(route, false);
			pr_debug("%s: Got slot for route: %d\n", __func__, slot);
			priv->r->host_route_write(slot, route);
		}
	}

	/* We need to resolve the mac address of the GW */
	if (nh->fib_nh_gw4)
		otto_l3_port_ipv4_resolve(priv, ndev, nh->fib_nh_gw4);

	nh->fib_nh_flags |= RTNH_F_OFFLOAD;

	return 0;

out_free_rmac:
out_free_rt:
	return 0;
}

static int otto_l3_fib_del_v4(struct rtl838x_switch_priv *priv,
			   struct fib_entry_notifier_info *info)
{
	struct fib_nh *nh = fib_info_nh(info->fi, 0);
	struct otto_l3_ctrl *ctrl = priv->l3_ctrl;
	struct rhlist_head *tmp, *list;
	struct otto_l3_route *route;

	if (otto_l3_fib_check_v4(priv, info, FIB_EVENT_ENTRY_DEL))
		return 0;

	rcu_read_lock();
	list = rhltable_lookup(&ctrl->routes, &nh->fib_nh_gw4, otto_l3_route_ht_params);
	if (!list) {
		rcu_read_unlock();
		dev_err(priv->dev, "no such gateway: %pI4\n", &nh->fib_nh_gw4);
		return -ENOENT;
	}
	rhl_for_each_entry_rcu(route, tmp, list, linkage) {
		if (route->dst_ip == info->dst && route->prefix_len == info->dst_len) {
			dev_info(priv->dev, "found a route with id %d, nh-id %d\n",
				 route->id, route->nh.id);
			break;
		}
	}
	rcu_read_unlock();

	rtl83xx_l2_nexthop_rm(priv, &route->nh);

	dev_info(priv->dev, "releasing packet counter %d\n", route->pr.packet_cntr);
	set_bit(route->pr.packet_cntr, priv->packet_cntr_use_bm);
	priv->r->pie_rule_rm(priv, &route->pr);

	otto_l3_route_remove(priv, route);

	nh->fib_nh_flags &= ~RTNH_F_OFFLOAD;

	return 0;
}

static int otto_l3_fib_add_v6(struct rtl838x_switch_priv *priv,
			    struct fib6_entry_notifier_info *info)
{
	pr_debug("In %s\n", __func__);
/*	nh->fib_nh_flags |= RTNH_F_OFFLOAD; */

	return 0;
}

static void otto_l3_fib_event_work_do(struct work_struct *work)
{
	struct otto_l3_fib_event_work *fib_work =
		container_of(work, struct otto_l3_fib_event_work, work);
	struct rtl838x_switch_priv *priv = fib_work->priv;
	struct fib_rule *rule;
	int err;

	/* Protect internal structures from changes */
	rtnl_lock();
	pr_debug("%s: doing work, event %ld\n", __func__, fib_work->event);
	switch (fib_work->event) {
	case FIB_EVENT_ENTRY_ADD:
	case FIB_EVENT_ENTRY_REPLACE:
	case FIB_EVENT_ENTRY_APPEND:
		if (fib_work->is_fib6)
			err = otto_l3_fib_add_v6(priv, &fib_work->fen6_info);
		else
			err = otto_l3_fib_add_v4(priv, &fib_work->fen_info);
		if (err)
			dev_err(priv->dev, "fib_add() failed\n");

		fib_info_put(fib_work->fen_info.fi);
		break;
	case FIB_EVENT_ENTRY_DEL:
		err = otto_l3_fib_del_v4(priv, &fib_work->fen_info);
		if (err)
			dev_err(priv->dev, "fib_del() failed\n");

		fib_info_put(fib_work->fen_info.fi);
		break;
	case FIB_EVENT_RULE_ADD:
	case FIB_EVENT_RULE_DEL:
		rule = fib_work->fr_info.rule;
		if (!fib4_rule_default(rule))
			pr_err("%s: FIB4 default rule failed\n", __func__);
		fib_rule_put(rule);
		break;
	}
	rtnl_unlock();
	kfree(fib_work);
}


/* Called with rcu_read_lock() */
static int otto_l3_fib_notifier(struct notifier_block *this, unsigned long event, void *ptr)
{
	struct otto_l3_ctrl *ctrl = container_of(this, struct otto_l3_ctrl, fib_nb);
	struct rtl838x_switch_priv *priv = ctrl->priv;
	struct otto_l3_fib_event_work *fib_work;
	struct fib_notifier_info *info = ptr;

	if ((info->family != AF_INET && info->family != AF_INET6 &&
	     info->family != RTNL_FAMILY_IPMR &&
	     info->family != RTNL_FAMILY_IP6MR))
		return NOTIFY_DONE;

	/* ignore FIB events for HW with missing L3 offloading implementation */
	if (!priv->r->l3_setup)
		return NOTIFY_DONE;

	fib_work = kzalloc(sizeof(*fib_work), GFP_ATOMIC);
	if (!fib_work)
		return NOTIFY_BAD;

	INIT_WORK(&fib_work->work, otto_l3_fib_event_work_do);
	fib_work->priv = priv;
	fib_work->event = event;
	fib_work->is_fib6 = false;

	switch (event) {
	case FIB_EVENT_ENTRY_ADD:
	case FIB_EVENT_ENTRY_REPLACE:
	case FIB_EVENT_ENTRY_APPEND:
	case FIB_EVENT_ENTRY_DEL:
		pr_debug("%s: FIB_ENTRY ADD/DEL, event %ld\n", __func__, event);
		if (info->family == AF_INET) {
			struct fib_entry_notifier_info *fen_info = ptr;

			if (fen_info->fi->fib_nh_is_v6) {
				NL_SET_ERR_MSG_MOD(info->extack,
						   "IPv6 gateway with IPv4 route is not supported");
				kfree(fib_work);
				return notifier_from_errno(-EINVAL);
			}

			memcpy(&fib_work->fen_info, ptr, sizeof(fib_work->fen_info));
			/* Take referece on fib_info to prevent it from being
			 * freed while work is queued. Release it afterwards.
			 */
			fib_info_hold(fib_work->fen_info.fi);

		} else if (info->family == AF_INET6) {
			//struct fib6_entry_notifier_info *fen6_info = ptr;
			pr_warn("%s: FIB_RULE ADD/DEL for IPv6 not supported\n", __func__);
			kfree(fib_work);
			return NOTIFY_DONE;
		}
		break;

	case FIB_EVENT_RULE_ADD:
	case FIB_EVENT_RULE_DEL:
		pr_debug("%s: FIB_RULE ADD/DEL, event: %ld\n", __func__, event);
		memcpy(&fib_work->fr_info, ptr, sizeof(fib_work->fr_info));
		fib_rule_get(fib_work->fr_info.rule);
		break;
	}

	queue_work(priv->wq, &fib_work->work);

	return NOTIFY_DONE;
}

static void otto_l3_net_event_work_do(struct work_struct *work)
{
	struct otto_l3_net_event_work *net_work =
		container_of(work, struct otto_l3_net_event_work, work);
	struct rtl838x_switch_priv *priv = net_work->priv;

	otto_l3_nexthop_update(priv, net_work->gw_addr, net_work->mac);

	kfree(net_work);
}

static int otto_l3_netevent_notifier(struct notifier_block *this, unsigned long event, void *ptr)
{
	struct otto_l3_ctrl *ctrl = container_of(this, struct otto_l3_ctrl, ne_nb);
	struct rtl838x_switch_priv *priv = ctrl->priv;
	struct otto_l3_net_event_work *net_work;
	struct neighbour *n = ptr;
	struct net_device *dev;
	int err, port;

	switch (event) {
	case NETEVENT_NEIGH_UPDATE:
		/* ignore events for HW with missing L3 offloading implementation */
		if (!priv->r->l3_setup)
			return NOTIFY_DONE;

		if (n->tbl != &arp_tbl)
			return NOTIFY_DONE;
		dev = n->dev;
		port = otto_l3_port_dev_lower_find(dev, priv);
		if (port < 0 || !(n->nud_state & NUD_VALID)) {
			pr_debug("%s: Neigbour invalid, not updating\n", __func__);
			return NOTIFY_DONE;
		}

		net_work = kzalloc(sizeof(*net_work), GFP_ATOMIC);
		if (!net_work)
			return NOTIFY_BAD;

		INIT_WORK(&net_work->work, otto_l3_net_event_work_do);
		net_work->priv = priv;

		net_work->mac = ether_addr_to_u64(n->ha);
		net_work->gw_addr = *(__be32 *)n->primary_key;

		pr_debug("%s: updating neighbour on port %d, mac %016llx\n",
			 __func__, port, net_work->mac);
		queue_work(priv->wq, &net_work->work);
		if (err)
			netdev_warn(dev, "failed to handle neigh update (err %d)\n", err);
		break;
	}

	return NOTIFY_DONE;
}

void otto_l3_remove(struct rtl838x_switch_priv *priv)
{
	struct otto_l3_ctrl *ctrl = priv->l3_ctrl;

	if (ctrl->ne_nb.notifier_call) {
		unregister_netevent_notifier(&ctrl->ne_nb);
		ctrl->ne_nb.notifier_call = NULL;
	}
	if (ctrl->fib_nb.notifier_call) {
		unregister_fib_notifier(&init_net, &ctrl->fib_nb);
		ctrl->fib_nb.notifier_call = NULL;
	}
}

int otto_l3_probe(struct device *dev, struct rtl838x_switch_priv *priv)
{
	struct otto_l3_ctrl *ctrl;
	int err;

	ctrl = devm_kzalloc(dev, sizeof(struct otto_l3_ctrl), GFP_KERNEL);
	if (!ctrl)
		return -ENOMEM;
	priv->l3_ctrl = ctrl;
	ctrl->priv = priv;

	/* Initialize hash table for L3 routing */
	rhltable_init(&ctrl->routes, &otto_l3_route_ht_params);

	/*
	 * Register netevent notifier callback to catch notifications about neighboring changes
	 * to update nexthop entries for L3 routing.
	 */
	ctrl->ne_nb.notifier_call = otto_l3_netevent_notifier;
	err = register_netevent_notifier(&ctrl->ne_nb);
	if (err) {
		ctrl->ne_nb.notifier_call = NULL;
		return dev_err_probe(dev, err, "Failed to register netevent notifier\n");
	}

	/*
	 * Register Forwarding Information Base notifier to offload routes where possible. Only
	 * FIBs pointing to our own netdevs are programmed into the device, so no need to pass a
	 * callback.
	 */
	ctrl->fib_nb.notifier_call = otto_l3_fib_notifier;
	err = register_fib_notifier(&init_net, &ctrl->fib_nb, NULL, NULL);
	if (err) {
		ctrl->fib_nb.notifier_call = NULL;
		otto_l3_remove(priv);
		return dev_err_probe(dev, err, "Failed to register fib event notifier\n");
	}

	return 0;
}

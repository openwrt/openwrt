// SPDX-License-Identifier: GPL-2.0-only
/* Conntrack-approved IPv4/IPv6 flows, routed by the RTL930x VACL/L3 engine. */

#include <linux/debugfs.h>
#include <linux/etherdevice.h>
#include <linux/if_vlan.h>
#include <linux/kref.h>
#include <linux/seq_file.h>
#include <net/addrconf.h>
#include <net/fib_notifier.h>
#include <net/arp.h>
#include <net/flow_offload.h>
#include <net/ipv6.h>
#include <net/ndisc.h>
#include <net/netevent.h>
#include <net/net_namespace.h>
#include <net/route.h>
#include <net/tcp.h>

#include "flow.h"
#include "flow_neigh.h"
#include "l2.h"
#include "l3.h"
#include "pie.h"

enum otto_flow_remove_reason {
	OTTO_FLOW_CONNTRACK,
	OTTO_FLOW_NEIGHBOUR,
	OTTO_FLOW_FDB,
	OTTO_FLOW_ROUTE,
	OTTO_FLOW_DEVICE,
	OTTO_FLOW_VLAN,
	OTTO_FLOW_REPLACE,
	OTTO_FLOW_HW_ERROR,
	OTTO_FLOW_UNBIND,
	OTTO_FLOW_DISABLED,
	OTTO_FLOW_SHUTDOWN,
	OTTO_FLOW_REMOVE_REASONS,
};

static const char * const otto_flow_remove_names[] = {
	[OTTO_FLOW_CONNTRACK] = "conntrack",
	[OTTO_FLOW_NEIGHBOUR] = "neighbour",
	[OTTO_FLOW_FDB] = "fdb",
	[OTTO_FLOW_ROUTE] = "route",
	[OTTO_FLOW_DEVICE] = "device",
	[OTTO_FLOW_VLAN] = "vlan",
	[OTTO_FLOW_REPLACE] = "replace",
	[OTTO_FLOW_HW_ERROR] = "hw_error",
	[OTTO_FLOW_UNBIND] = "unbind",
	[OTTO_FLOW_DISABLED] = "disabled",
	[OTTO_FLOW_SHUTDOWN] = "shutdown",
};

struct otto_flow_ctrl {
	struct kref refs;
	struct mutex lock;
	struct otto_dsa_priv *priv;
	struct list_head flows;
	struct notifier_block fib_nb;
	struct notifier_block ne_nb;
	struct notifier_block nd_nb;
	struct work_struct invalidate_work;
	struct work_struct neigh_work;
	atomic_t generation;
	atomic_t pending_invalidations;
	struct dentry *debugfs;
	bool enabled;
	u64 installs;
	u64 refreshes;
	u64 removed[OTTO_FLOW_REMOVE_REASONS];
};

struct otto_flow_block {
	struct otto_flow_ctrl *ctrl;
	struct net *net;
	unsigned int port_refs[57];
};

struct otto_flow {
	struct list_head list;
	struct otto_flow_block *block;
	unsigned long cookie;
	struct pie_rule rule;
	struct otto_l3_nexthop nh;
	struct list_head neighbours;
	int generation;
	unsigned long lastused;
};

static LIST_HEAD(otto_flow_block_list);

static struct otto_flow_ctrl *otto_flow_ctrl_get(struct otto_dsa_priv *priv)
{
	struct otto_flow_ctrl *ctrl;

	guard(mutex)(&priv->reg_mutex);
	ctrl = priv->flow_ctrl;
	if (ctrl)
		kref_get(&ctrl->refs);
	return ctrl;
}

static void otto_flow_ctrl_free(struct kref *ref)
{
	struct otto_flow_ctrl *ctrl = container_of(ref, struct otto_flow_ctrl, refs);

	mutex_destroy(&ctrl->lock);
	kfree(ctrl);
}

static void otto_flow_remove(struct otto_flow_ctrl *ctrl, struct otto_flow *flow,
			     enum otto_flow_remove_reason reason)
{
	/* Withdraw the classifier before releasing anything it can reference. */
	otto_pie_930x_flow_rule_rm(ctrl->priv, &flow->rule);
	otto_l3_flow_nexthop_put(ctrl->priv, &flow->nh);
	otto_flow_neigh_put(&flow->neighbours);
	ctrl->removed[reason]++;
	list_del(&flow->list);
	kfree(flow);
}

static void otto_flow_flush(struct otto_flow_ctrl *ctrl, enum otto_flow_remove_reason reason)
{
	struct otto_flow *flow, *tmp;

	list_for_each_entry_safe(flow, tmp, &ctrl->flows, list)
		otto_flow_remove(ctrl, flow, reason);
}

/* Called before VLAN hardware changes, which do not necessarily send a
 * netdevice event. The caller must not hold a register or PIE mutex.
 */
void otto_flow_flush_all(struct otto_dsa_priv *priv)
{
	struct otto_flow_ctrl *ctrl = otto_flow_ctrl_get(priv);

	if (!ctrl)
		return;
	mutex_lock(&ctrl->lock);
	if (ctrl->priv) {
		atomic_inc(&ctrl->generation);
		otto_flow_flush(ctrl, OTTO_FLOW_VLAN);
	}
	mutex_unlock(&ctrl->lock);
	kref_put(&ctrl->refs, otto_flow_ctrl_free);
}

/* Topology and route changes retire saved forwarding decisions. No notifier
 * waits for RTNL or a flow workqueue: NETDEV_DOWN itself flushes netfilter's
 * offload workqueues while holding RTNL.
 */
static void otto_flow_invalidate_work(struct work_struct *work)
{
	struct otto_flow_ctrl *ctrl = container_of(work, struct otto_flow_ctrl,
						 invalidate_work);
	int pending;

	guard(mutex)(&ctrl->lock);
	pending = atomic_xchg(&ctrl->pending_invalidations, 0);
	if (ctrl->priv && pending)
		otto_flow_flush(ctrl, pending & BIT(OTTO_FLOW_ROUTE) ?
				 OTTO_FLOW_ROUTE : OTTO_FLOW_DEVICE);
}

static void otto_flow_invalidate(struct otto_flow_ctrl *ctrl, enum otto_flow_remove_reason reason)
{
	atomic_inc(&ctrl->generation);
	atomic_or(BIT(reason), &ctrl->pending_invalidations);
	schedule_work(&ctrl->invalidate_work);
}

static void otto_flow_neigh_work(struct work_struct *work)
{
	struct otto_flow_ctrl *ctrl = container_of(work, struct otto_flow_ctrl, neigh_work);
	struct otto_flow *flow, *tmp;

	guard(mutex)(&ctrl->lock);
	if (!ctrl->priv)
		return;
	list_for_each_entry_safe(flow, tmp, &ctrl->flows, list)
		if (!otto_flow_neigh_valid(&flow->neighbours, flow->nh.mac))
			otto_flow_remove(ctrl, flow, OTTO_FLOW_NEIGHBOUR);
}

static int otto_flow_fib_event(struct notifier_block *nb, unsigned long event, void *ptr)
{
	struct otto_flow_ctrl *ctrl = container_of(nb, struct otto_flow_ctrl, fib_nb);
	struct fib_notifier_info *info = ptr;

	if (info->family == AF_INET || info->family == AF_INET6)
		otto_flow_invalidate(ctrl, OTTO_FLOW_ROUTE);
	return NOTIFY_DONE;
}

static int otto_flow_neigh_event(struct notifier_block *nb, unsigned long event, void *ptr)
{
	struct otto_flow_ctrl *ctrl = container_of(nb, struct otto_flow_ctrl, ne_nb);
	struct neighbour *neigh = ptr;

	/* Reachability refreshes do not change forwarding. Revalidate held
	 * ARP/ND neighbours instead of deleting every flow for any ARP/ND event.
	 * A valid neighbour whose MAC changed must still retire affected rules.
	 */
	if (event == NETEVENT_NEIGH_UPDATE &&
	    (neigh->tbl == &arp_tbl || (IS_REACHABLE(CONFIG_IPV6) && neigh->tbl == &nd_tbl)) &&
	    net_eq(dev_net(neigh->dev), &init_net))
		schedule_work(&ctrl->neigh_work);
	return NOTIFY_DONE;
}

static int otto_flow_netdev_event(struct notifier_block *nb, unsigned long event, void *ptr)
{
	struct otto_flow_ctrl *ctrl = container_of(nb, struct otto_flow_ctrl, nd_nb);
	struct net_device *dev = netdev_notifier_info_to_dev(ptr);

	if (!net_eq(dev_net(dev), &init_net))
		return NOTIFY_DONE;

	switch (event) {
	case NETDEV_DOWN:
	case NETDEV_UNREGISTER:
	case NETDEV_CHANGEADDR:
	case NETDEV_CHANGEUPPER:
	case NETDEV_CHANGELOWERSTATE:
	case NETDEV_CHANGE:
	case NETDEV_CHANGEMTU:
		otto_flow_invalidate(ctrl, OTTO_FLOW_DEVICE);
		break;
	}
	return NOTIFY_DONE;
}

static struct otto_flow *otto_flow_find(struct otto_flow_block *block, unsigned long cookie)
{
	struct otto_flow *flow;

	list_for_each_entry(flow, &block->ctrl->flows, list)
		if (flow->block == block && flow->cookie == cookie)
			return flow;
	return NULL;
}

/* Decode netfilter's four Ethernet writes explicitly. The offset-4 writes
 * describe native u16 values in fixed numeric halves of a u32; applying them
 * to a native u32 loaded from the packet swaps the two halves on big endian.
 */
static int otto_flow_eth_mangle(const struct flow_action_entry *act, u8 *eth, u8 *written)
{
	u16 half;
	u32 offset = act->mangle.offset;

	if (act->mangle.htype != FLOW_ACT_MANGLE_HDR_TYPE_ETH ||
	    offset > 8 || offset % 4)
		return -EOPNOTSUPP;
	if ((offset == 0 || offset == 8) && !act->mangle.mask) {
		memcpy(eth + offset, &act->mangle.val, sizeof(u32));
		memset(written + offset, 0xff, sizeof(u32));
	} else if (offset == 4 && act->mangle.mask == 0x0000ffff &&
		   !(act->mangle.val & 0x0000ffff)) {
		half = act->mangle.val >> 16;
		memcpy(eth + 6, &half, sizeof(half));
		memset(written + 6, 0xff, sizeof(half));
	} else if (offset == 4 && act->mangle.mask == 0xffff0000 &&
		   !(act->mangle.val & 0xffff0000)) {
		half = act->mangle.val;
		memcpy(eth + 4, &half, sizeof(half));
		memset(written + 4, 0xff, sizeof(half));
	} else {
		return -EOPNOTSUPP;
	}
	return 0;
}

static int otto_flow_upper_check(struct net_device *dev, struct netdev_nested_priv *nested)
{
	/* The flower API does not carry the routed MTU. Only admit paths whose
	 * VLAN and bridge devices share the flow engine's 1500-byte limit.
	 */
	if (dev->mtu != ETH_DATA_LEN ||
	    (!is_vlan_dev(dev) && !netif_is_bridge_master(dev)))
		return -EOPNOTSUPP;
	return 0;
}

static bool otto_flow_path_supported(struct net_device *dev)
{
	struct netdev_nested_priv nested = {};

	return dev->mtu == ETH_DATA_LEN &&
	       !netdev_walk_all_upper_dev_rcu(dev, otto_flow_upper_check, &nested);
}

static bool otto_flow_addresses_supported(struct net *net, __be32 src, __be32 dst)
{
	return inet_addr_type(net, src) == RTN_UNICAST &&
	       inet_addr_type(net, dst) == RTN_UNICAST;
}

static bool otto_flow_ipv6_addresses_supported(struct net *net,
					      const struct in6_addr *src,
					      const struct in6_addr *dst)
{
	if (!IS_REACHABLE(CONFIG_IPV6))
		return false;

	/* Scoped and IPv4-mapped endpoints are outside this routed flow path.
	 * Include tentative local addresses in the CPU-only boundary as well.
	 */
	return ipv6_addr_type(src) == IPV6_ADDR_UNICAST &&
	       ipv6_addr_type(dst) == IPV6_ADDR_UNICAST &&
	       !ipv6_chk_addr_and_flags(net, src, NULL, true, 0, 0) &&
	       !ipv6_chk_addr_and_flags(net, dst, NULL, true, 0, 0);
}

static int otto_flow_parse_addresses(struct net *net, struct flow_rule *rule,
				     struct pie_rule *pr)
{
	struct flow_match_control control;
	struct flow_match_basic basic;

	flow_rule_match_control(rule, &control);
	flow_rule_match_basic(rule, &basic);
	if (basic.mask->n_proto != htons(U16_MAX) || control.mask->addr_type != U16_MAX)
		return -EOPNOTSUPP;
	if (basic.key->n_proto == htons(ETH_P_IP)) {
		struct flow_match_ipv4_addrs ipv4;

		if (control.key->addr_type != FLOW_DISSECTOR_KEY_IPV4_ADDRS ||
		    !flow_rule_match_key(rule, FLOW_DISSECTOR_KEY_IPV4_ADDRS) ||
		    flow_rule_match_key(rule, FLOW_DISSECTOR_KEY_IPV6_ADDRS))
			return -EOPNOTSUPP;
		flow_rule_match_ipv4_addrs(rule, &ipv4);
		if (ipv4.mask->src != htonl(U32_MAX) || ipv4.mask->dst != htonl(U32_MAX) ||
		    !otto_flow_addresses_supported(net, ipv4.key->src, ipv4.key->dst))
			return -EOPNOTSUPP;
		pr->is_ipv6 = false;
		pr->frame_type = 2;
		pr->sip = ntohl(ipv4.key->src);
		pr->sip_m = U32_MAX;
		pr->dip = ntohl(ipv4.key->dst);
		pr->dip_m = U32_MAX;
	} else if (basic.key->n_proto == htons(ETH_P_IPV6)) {
		struct flow_match_ipv6_addrs ipv6;

		if (control.key->addr_type != FLOW_DISSECTOR_KEY_IPV6_ADDRS ||
		    !flow_rule_match_key(rule, FLOW_DISSECTOR_KEY_IPV6_ADDRS) ||
		    flow_rule_match_key(rule, FLOW_DISSECTOR_KEY_IPV4_ADDRS))
			return -EOPNOTSUPP;
		flow_rule_match_ipv6_addrs(rule, &ipv6);
		memset(&pr->sip6_m, 0xff, sizeof(pr->sip6_m));
		memset(&pr->dip6_m, 0xff, sizeof(pr->dip6_m));
		if (memcmp(&ipv6.mask->src, &pr->sip6_m, sizeof(pr->sip6_m)) ||
		    memcmp(&ipv6.mask->dst, &pr->dip6_m, sizeof(pr->dip6_m)) ||
		    !otto_flow_ipv6_addresses_supported(net, &ipv6.key->src, &ipv6.key->dst))
			return -EOPNOTSUPP;
		pr->is_ipv6 = true;
		pr->frame_type = 3;
		pr->sip6 = ipv6.key->src;
		pr->dip6 = ipv6.key->dst;
	} else {
		return -EOPNOTSUPP;
	}
	return 0;
}

static int otto_flow_parse(struct otto_flow_block *block, struct flow_cls_offload *cls,
			   struct otto_flow *flow, int *out_port, u16 *out_vid,
			   u8 *eth)
{
	struct otto_dsa_priv *priv = block->ctrl->priv;
	struct flow_rule *rule = flow_cls_offload_flow_rule(cls);
	struct flow_match_control control;
	struct flow_match_basic basic;
	struct flow_match_ports ports;
	struct flow_match_meta meta;
	const struct flow_action_entry *act;
	struct net_device *indev, *outdev = NULL;
	u8 written[2 * ETH_ALEN] = {};
	bool push = false, pop = false;
	u64 required, allowed;
	int in_port, i, err;

	required = BIT_ULL(FLOW_DISSECTOR_KEY_META) |
		   BIT_ULL(FLOW_DISSECTOR_KEY_CONTROL) |
		   BIT_ULL(FLOW_DISSECTOR_KEY_BASIC) |
		   BIT_ULL(FLOW_DISSECTOR_KEY_PORTS);
	if ((rule->match.dissector->used_keys & required) != required)
		return -EOPNOTSUPP;
	flow_rule_match_basic(rule, &basic);
	if (basic.key->n_proto == htons(ETH_P_IP))
		required |= BIT_ULL(FLOW_DISSECTOR_KEY_IPV4_ADDRS);
	else if (basic.key->n_proto == htons(ETH_P_IPV6))
		required |= BIT_ULL(FLOW_DISSECTOR_KEY_IPV6_ADDRS);
	else
		return -EOPNOTSUPP;
	allowed = required | BIT_ULL(FLOW_DISSECTOR_KEY_TCP) |
		  BIT_ULL(FLOW_DISSECTOR_KEY_VLAN);
	if ((rule->match.dissector->used_keys & required) != required ||
	    rule->match.dissector->used_keys & ~allowed || cls->common.chain_index)
		return -EOPNOTSUPP;

	flow_rule_match_meta(rule, &meta);
	flow_rule_match_control(rule, &control);
	flow_rule_match_ports(rule, &ports);
	if (meta.mask->ingress_ifindex != U32_MAX || meta.mask->ingress_iftype ||
	    meta.mask->l2_miss ||
	    control.mask->addr_type != U16_MAX || control.mask->flags || control.mask->thoff ||
	    basic.mask->ip_proto != U8_MAX ||
	    ports.mask->src != htons(U16_MAX) || ports.mask->dst != htons(U16_MAX))
		return -EOPNOTSUPP;

	/* The nftables forwarding hook already excludes INPUT and OUTPUT.
	 * Enforce the same boundary here before allocating any hardware state:
	 * neither direction of a connection to this router may be offloaded.
	 */
	err = otto_flow_parse_addresses(block->net, rule, &flow->rule);
	if (err)
		return err;

	indev = __dev_get_by_index(block->net, meta.key->ingress_ifindex);
	if (!indev || !dsa_user_dev_check(indev))
		return -EOPNOTSUPP;
	in_port = dsa_port_from_netdev(indev)->index;
	if (dsa_port_from_netdev(indev)->ds != priv->ds || !netif_running(indev))
		return -EOPNOTSUPP;
	if (!block->port_refs[in_port])
		return -EOPNOTSUPP;
	if (priv->lagmembers & BIT_ULL(in_port))
		return -EOPNOTSUPP;

	flow->rule.phase = PHASE_VACL;
	flow->rule.spn = in_port;
	flow->rule.spn_m = 0x7f;
	flow->rule.frame_type_m = 3;
	flow->rule.not_first_frag_m = true;
	flow->rule.content_too_deep_m = true;
	flow->rule.sport = ntohs(ports.key->src);
	flow->rule.sport_m = U16_MAX;
	flow->rule.dport = ntohs(ports.key->dst);
	flow->rule.dport_m = U16_MAX;
	flow->rule.tos_proto = basic.key->ip_proto;
	flow->rule.tos_proto_m = U8_MAX;
	flow->rule.frame_type_l4_m = 7;
	if (basic.key->ip_proto == IPPROTO_TCP) {
		struct flow_match_tcp tcp;

		if (!flow_rule_match_key(rule, FLOW_DISSECTOR_KEY_TCP))
			return -EOPNOTSUPP;
		flow_rule_match_tcp(rule, &tcp);
		if (tcp.key->flags || tcp.mask->flags != htons(TCPHDR_FIN | TCPHDR_RST))
			return -EOPNOTSUPP;
		flow->rule.frame_type_l4 = 1;
		/* Leave tuple reuse and TCP closing packets on the CPU path. */
		flow->rule.tcp_info_m = TCPHDR_FIN | TCPHDR_RST | TCPHDR_SYN;
	} else if (basic.key->ip_proto != IPPROTO_UDP ||
		   flow_rule_match_key(rule, FLOW_DISSECTOR_KEY_TCP)) {
		return -EOPNOTSUPP;
	}

	/* The VID match is the classified inner VID; tag presence keeps an
	 * untagged flow from admitting tagged packets in a different domain.
	 */
	flow->rule.itag_m = VLAN_VID_MASK;
	flow->rule.itag_exist_m = true;
	flow->rule.otag_exist_m = true;
	if (flow_rule_match_key(rule, FLOW_DISSECTOR_KEY_VLAN)) {
		struct flow_match_vlan vlan;

		flow_rule_match_vlan(rule, &vlan);
		if (vlan.mask->vlan_id != VLAN_VID_MASK || vlan.mask->vlan_priority ||
		    vlan.mask->vlan_dei || vlan.mask->vlan_eth_type ||
		    vlan.mask->vlan_tpid != htons(U16_MAX) ||
		    vlan.key->vlan_tpid != htons(ETH_P_8021Q) || !vlan.key->vlan_id)
			return -EOPNOTSUPP;
		flow->rule.itag = vlan.key->vlan_id;
		flow->rule.itag_exist = true;
	} else {
		flow->rule.itag = priv->ports[in_port].pvid;
	}

	*out_vid = 0;
	flow_action_for_each(i, act, &rule->action) {
		switch (act->id) {
		case FLOW_ACTION_MANGLE:
			err = otto_flow_eth_mangle(act, eth, written);
			if (err)
				return err;
			break;
		case FLOW_ACTION_VLAN_POP:
			if (pop || !flow_rule_match_key(rule, FLOW_DISSECTOR_KEY_VLAN))
				return -EOPNOTSUPP;
			pop = true;
			break;
		case FLOW_ACTION_VLAN_PUSH:
			if (push || act->vlan.proto != htons(ETH_P_8021Q) ||
			    act->vlan.prio || !act->vlan.vid || act->vlan.vid >= VLAN_VID_MASK)
				return -EOPNOTSUPP;
			push = true;
			*out_vid = act->vlan.vid;
			break;
		case FLOW_ACTION_REDIRECT:
			if (outdev)
				return -EOPNOTSUPP;
			outdev = act->dev;
			break;
		default:
			/* Includes every NAT, checksum, tunnel and PPPoE action. */
			return -EOPNOTSUPP;
		}
	}
	if (!outdev || !dsa_user_dev_check(outdev) ||
	    dsa_port_from_netdev(outdev)->ds != priv->ds ||
	    !net_eq(dev_net(outdev), block->net) || !netif_running(outdev) ||
	    !is_valid_ether_addr(eth) || !is_valid_ether_addr(eth + ETH_ALEN))
		return -EOPNOTSUPP;
	if (pop != flow_rule_match_key(rule, FLOW_DISSECTOR_KEY_VLAN))
		return -EOPNOTSUPP;
	for (i = 0; i < sizeof(written); i++)
		if (written[i] != U8_MAX)
			return -EOPNOTSUPP;

	*out_port = dsa_port_from_netdev(outdev)->index;
	if (!push)
		*out_vid = priv->ports[*out_port].pvid;
	if (!otto_flow_path_supported(indev) || !otto_flow_path_supported(outdev))
		return -EOPNOTSUPP;
	return 0;
}

static int otto_flow_replace(struct otto_flow_block *block, struct flow_cls_offload *cls)
{
	struct otto_flow_ctrl *ctrl = block->ctrl;
	struct otto_dsa_priv *priv = ctrl->priv;
	struct otto_flow *flow;
	u8 eth[2 * ETH_ALEN] = {};
	int err, out_port, generation;
	u16 out_vid;

	flow = otto_flow_find(block, cls->cookie);
	if (flow) {
		/* Software flowtable hits can refresh the same cookie repeatedly.
		 * Keep a still-current classifier, next hop, and counter intact.
		 */
		if (flow->generation == atomic_read(&ctrl->generation) &&
		    otto_flow_neigh_valid(&flow->neighbours, flow->nh.mac) &&
		    otto_l2_nexthop_current(priv, &flow->nh)) {
			ctrl->refreshes++;
			return 0;
		}
		otto_flow_remove(ctrl, flow, OTTO_FLOW_REPLACE);
	}

	/* Never wait for RTNL: device-down holds it while flushing this very
	 * callback's netfilter workqueue. Contention leaves the flow in software.
	 */
	if (!rtnl_trylock())
		return -EAGAIN;
	generation = atomic_read(&ctrl->generation);
	flow = kzalloc(sizeof(*flow), GFP_KERNEL);
	if (!flow) {
		err = -ENOMEM;
		goto unlock;
	}
	INIT_LIST_HEAD(&flow->neighbours);
	err = otto_flow_parse(block, cls, flow, &out_port, &out_vid, eth);
	if (err)
		goto free;
	err = otto_flow_neigh_get(&flow->neighbours, block->net, eth + ETH_ALEN,
				  eth, out_vid, flow->rule.is_ipv6 ? AF_INET6 : AF_INET);
	if (err)
		goto free;
	err = otto_l3_flow_nexthop_get(priv, &flow->nh,
				       ether_addr_to_u64(eth + ETH_ALEN),
				       ether_addr_to_u64(eth), out_vid, out_port);
	if (err)
		goto free;
	flow->rule.fwd_sel = true;
	flow->rule.fwd_act = PIE_ACT_ROUTE_UC;
	flow->rule.fwd_data = flow->nh.id;
	flow->rule.log_sel = true;
	flow->rule.packet_cntr = -1;
	err = otto_pie_930x_flow_rule_add(priv, &flow->rule);
	if (err)
		goto put;
	if (generation != atomic_read(&ctrl->generation)) {
		otto_pie_930x_flow_rule_rm(priv, &flow->rule);
		err = -EAGAIN;
		goto put;
	}
	flow->cookie = cls->cookie;
	flow->block = block;
	flow->generation = generation;
	flow->lastused = jiffies;
	list_add_tail(&flow->list, &ctrl->flows);
	ctrl->installs++;
	rtnl_unlock();
	return 0;
put:
	otto_l3_flow_nexthop_put(priv, &flow->nh);
free:
	otto_flow_neigh_put(&flow->neighbours);
	kfree(flow);
unlock:
	rtnl_unlock();
	return err;
}

static int otto_flow_cb(enum tc_setup_type type, void *type_data, void *cb_priv)
{
	struct otto_flow_block *block = cb_priv;
	struct otto_flow_ctrl *ctrl = block->ctrl;
	struct flow_cls_offload *cls = type_data;
	struct otto_flow *flow;
	u32 packets, delta;

	if (type != TC_SETUP_CLSFLOWER)
		return -EOPNOTSUPP;
	guard(mutex)(&ctrl->lock);
	if (!ctrl->priv)
		return -ENODEV;
	if (!ctrl->enabled)
		return -EOPNOTSUPP;
	if (READ_ONCE(ctrl->priv->flow_pie_failed)) {
		otto_flow_flush(ctrl, OTTO_FLOW_HW_ERROR);
		return -EIO;
	}
	if (cls->command == FLOW_CLS_REPLACE)
		return otto_flow_replace(block, cls);
	flow = otto_flow_find(block, cls->cookie);
	if (!flow)
		return -ENOENT;
	switch (cls->command) {
	case FLOW_CLS_DESTROY:
		otto_flow_remove(ctrl, flow, OTTO_FLOW_CONNTRACK);
		return 0;
	case FLOW_CLS_STATS:
		if (!otto_flow_neigh_valid(&flow->neighbours, flow->nh.mac)) {
			otto_flow_remove(ctrl, flow, OTTO_FLOW_NEIGHBOUR);
			return -ENOENT;
		}
		if (!otto_l2_nexthop_current(ctrl->priv, &flow->nh)) {
			otto_flow_remove(ctrl, flow, OTTO_FLOW_FDB);
			return -ENOENT;
		}
		mutex_lock(&ctrl->priv->reg_mutex);
		packets = ctrl->priv->r->packet_cntr_read(ctrl->priv, flow->rule.packet_cntr);
		mutex_unlock(&ctrl->priv->reg_mutex);
		delta = packets - flow->rule.last_packet_cnt;
		flow->rule.last_packet_cnt = packets;
		if (delta)
			flow->lastused = jiffies;
		flow_stats_update(&cls->stats, 0, delta, 0, flow->lastused,
				  FLOW_ACTION_HW_STATS_DELAYED);
		return 0;
	default:
		return -EOPNOTSUPP;
	}
}

static void otto_flow_block_release(void *cb_priv)
{
	struct otto_flow_block *block = cb_priv;
	struct otto_flow_ctrl *ctrl = block->ctrl;
	struct otto_flow *flow, *tmp;

	mutex_lock(&ctrl->lock);
	list_for_each_entry_safe(flow, tmp, &ctrl->flows, list)
		if (flow->block == block)
			otto_flow_remove(ctrl, flow, OTTO_FLOW_UNBIND);
	mutex_unlock(&ctrl->lock);
	put_net(block->net);
	kref_put(&ctrl->refs, otto_flow_ctrl_free);
	kfree(block);
	module_put(THIS_MODULE);
}

static int otto_flow_show(struct seq_file *s, void *unused)
{
	struct otto_flow_ctrl *ctrl = s->private;
	struct otto_flow *flow;

	guard(mutex)(&ctrl->lock);
	seq_puts(s, "rule nexthop ingress vlan proto source destination packets idle_ms cookie block\n");
	list_for_each_entry(flow, &ctrl->flows, list) {
		seq_printf(s, "%d %u %u %u %u ",
			   flow->rule.id, flow->nh.id, flow->rule.spn, flow->rule.itag,
			   flow->rule.tos_proto);
		if (flow->rule.is_ipv6) {
			seq_printf(s, "[%pI6c]:%u [%pI6c]:%u ",
				   &flow->rule.sip6, flow->rule.sport,
				   &flow->rule.dip6, flow->rule.dport);
		} else {
			__be32 src = htonl(flow->rule.sip);
			__be32 dst = htonl(flow->rule.dip);

			seq_printf(s, "%pI4:%u %pI4:%u ", &src, flow->rule.sport,
				   &dst, flow->rule.dport);
		}
		seq_printf(s, "%u %u %p %p\n", flow->rule.last_packet_cnt,
			   jiffies_to_msecs(jiffies - flow->lastused),
			   (void *)flow->cookie, flow->block);
	}
	return 0;
}

static int otto_flow_hardware_show(struct seq_file *s, void *unused)
{
	struct otto_flow_ctrl *ctrl = s->private;
	struct otto_flow *flow;
	int err;

	guard(mutex)(&ctrl->lock);
	if (!ctrl->priv)
		return -ENODEV;
	err = otto_l3_flow_dump(ctrl->priv, s);
	if (err)
		return err;
	err = otto_pie_930x_flow_dump(ctrl->priv, s, NULL);
	if (err)
		return err;
	list_for_each_entry(flow, &ctrl->flows, list) {
		seq_printf(s, "flow rule %d nexthop %u cookie %p block %p\n",
			   flow->rule.id, flow->nh.id, (void *)flow->cookie, flow->block);
		err = otto_pie_930x_flow_dump(ctrl->priv, s, &flow->rule);
		if (err)
			return err;
	}
	return 0;
}

static int otto_flow_status_show(struct seq_file *s, void *unused)
{
	struct otto_flow_ctrl *ctrl = s->private;
	struct otto_flow *flow;
	unsigned int active = 0;
	unsigned int v4_used = 0, v6_used = 0;
	const unsigned int v4_total = RTL930X_PIE_FLOW6_FIRST - RTL930X_PIE_FLOW_FIRST;
	const unsigned int v6_total = RTL930X_PIE_FLOW_END - RTL930X_PIE_FLOW6_FIRST;
	bool enabled;

	guard(mutex)(&ctrl->lock);
	list_for_each_entry(flow, &ctrl->flows, list) {
		active++;
		if (flow->rule.is_ipv6)
			v6_used += 2;
		else
			v4_used++;
	}
	enabled = ctrl->priv && ctrl->enabled && !READ_ONCE(ctrl->priv->flow_pie_failed);
	seq_printf(s, "enabled %u\nactive %u\ninstalls %llu\nrefreshes %llu\n",
		   enabled, active,
		   ctrl->installs, ctrl->refreshes);
	seq_printf(s, "rules_total %u\nrules_used %u\nrules_available %u\n",
		   v4_total + v6_total, v4_used + v6_used,
		   enabled ? v4_total + v6_total - v4_used - v6_used : 0);
	seq_printf(s, "ipv4_rules_total %u\nipv4_rules_used %u\nipv4_rules_available %u\n",
		   v4_total, v4_used, enabled ? v4_total - v4_used : 0);
	seq_printf(s, "ipv6_rules_total %u\nipv6_rules_used %u\nipv6_rules_available %u\n",
		   v6_total, v6_used, enabled ? v6_total - v6_used : 0);
	for (int i = 0; i < OTTO_FLOW_REMOVE_REASONS; i++)
		seq_printf(s, "removed_%s %llu\n", otto_flow_remove_names[i], ctrl->removed[i]);
	return 0;
}

static int otto_flow_debugfs_open_common(struct inode *inode, struct file *file,
					int (*show)(struct seq_file *, void *))
{
	struct otto_flow_ctrl *ctrl = inode->i_private;
	int err;

	kref_get(&ctrl->refs);
	err = single_open(file, show, ctrl);
	if (err)
		kref_put(&ctrl->refs, otto_flow_ctrl_free);
	return err;
}

static int otto_flow_debugfs_open(struct inode *inode, struct file *file)
{
	return otto_flow_debugfs_open_common(inode, file, otto_flow_show);
}

static int otto_flow_status_open(struct inode *inode, struct file *file)
{
	return otto_flow_debugfs_open_common(inode, file, otto_flow_status_show);
}

static int otto_flow_hardware_open(struct inode *inode, struct file *file)
{
	return otto_flow_debugfs_open_common(inode, file, otto_flow_hardware_show);
}

static int otto_flow_debugfs_release(struct inode *inode, struct file *file)
{
	struct seq_file *seq = file->private_data;
	struct otto_flow_ctrl *ctrl = seq->private;
	int err = single_release(inode, file);

	kref_put(&ctrl->refs, otto_flow_ctrl_free);
	return err;
}

static const struct file_operations otto_flow_debugfs_fops = {
	.owner = THIS_MODULE,
	.open = otto_flow_debugfs_open,
	.read = seq_read,
	.llseek = seq_lseek,
	.release = otto_flow_debugfs_release,
};

static const struct file_operations otto_flow_status_fops = {
	.owner = THIS_MODULE,
	.open = otto_flow_status_open,
	.read = seq_read,
	.llseek = seq_lseek,
	.release = otto_flow_debugfs_release,
};

static const struct file_operations otto_flow_hardware_fops = {
	.owner = THIS_MODULE,
	.open = otto_flow_hardware_open,
	.read = seq_read,
	.llseek = seq_lseek,
	.release = otto_flow_debugfs_release,
};

static int otto_flow_setup_block(struct otto_flow_ctrl *ctrl, int port,
				 struct flow_block_offload *f)
{
	struct flow_block_cb *cb;
	struct otto_flow_block *block;

	mutex_lock(&ctrl->lock);
	if (!ctrl->priv && f->command == FLOW_BLOCK_BIND) {
		mutex_unlock(&ctrl->lock);
		return -ENODEV;
	}
	mutex_unlock(&ctrl->lock);
	f->driver_block_list = &otto_flow_block_list;
	cb = flow_block_cb_lookup(f->block, otto_flow_cb, ctrl);
	switch (f->command) {
	case FLOW_BLOCK_BIND:
		if (cb) {
			block = cb->cb_priv;
			block->port_refs[port]++;
			flow_block_cb_incref(cb);
			return 0;
		}
		block = kzalloc(sizeof(*block), GFP_KERNEL);
		if (!block)
			return -ENOMEM;
		if (!try_module_get(THIS_MODULE)) {
			kfree(block);
			return -ENODEV;
		}
		block->ctrl = ctrl;
		block->net = get_net(f->net);
		block->port_refs[port] = 1;
		kref_get(&ctrl->refs);
		cb = flow_block_cb_alloc(otto_flow_cb, ctrl, block, otto_flow_block_release);
		if (IS_ERR(cb)) {
			otto_flow_block_release(block);
			return PTR_ERR(cb);
		}
		flow_block_cb_incref(cb);
		flow_block_cb_add(cb, f);
		list_add_tail(&cb->driver_list, &otto_flow_block_list);
		return 0;
	case FLOW_BLOCK_UNBIND:
		if (!cb)
			return -ENOENT;
		block = cb->cb_priv;
		if (!block->port_refs[port])
			return -ENOENT;
		block->port_refs[port]--;
		if (!flow_block_cb_decref(cb)) {
			flow_block_cb_remove(cb, f);
			list_del(&cb->driver_list);
		}
		return 0;
	default:
		return -EOPNOTSUPP;
	}
}

int otto_flow_setup_tc(struct dsa_switch *ds, int port,
		       enum tc_setup_type type, void *type_data)
{
	struct otto_dsa_priv *priv = ds->priv;
	struct flow_block_offload *f = type_data;
	struct otto_flow_ctrl *ctrl;
	int err;

	if (type != TC_SETUP_FT ||
	    f->binder_type != FLOW_BLOCK_BINDER_TYPE_CLSACT_INGRESS ||
	    !net_eq(f->net, &init_net))
		return -EOPNOTSUPP;
	ctrl = otto_flow_ctrl_get(priv);
	if (!ctrl)
		return -EOPNOTSUPP;
	err = otto_flow_setup_block(ctrl, port, f);
	kref_put(&ctrl->refs, otto_flow_ctrl_free);
	return err;
}

int otto_flow_init(struct otto_dsa_priv *priv)
{
	struct otto_flow_ctrl *ctrl;
	int err;

	/* Keep bindings alive while disabled so an existing nft hardware
	 * flowtable can start offloading when the parameter is enabled later.
	 */
	if (!priv->r->pie_rule_id_is_log_counter || priv->r->n_pie_blocks != 16)
		return 0;
	ctrl = kzalloc(sizeof(*ctrl), GFP_KERNEL);
	if (!ctrl)
		return -ENOMEM;
	kref_init(&ctrl->refs);
	mutex_init(&ctrl->lock);
	INIT_LIST_HEAD(&ctrl->flows);
	INIT_WORK(&ctrl->invalidate_work, otto_flow_invalidate_work);
	INIT_WORK(&ctrl->neigh_work, otto_flow_neigh_work);
	atomic_set(&ctrl->generation, 0);
	atomic_set(&ctrl->pending_invalidations, 0);
	ctrl->priv = priv;
	ctrl->nd_nb.notifier_call = otto_flow_netdev_event;
	err = register_netdevice_notifier(&ctrl->nd_nb);
	if (err)
		goto free;
	ctrl->ne_nb.notifier_call = otto_flow_neigh_event;
	err = register_netevent_notifier(&ctrl->ne_nb);
	if (err)
		goto unregister_netdev;
	ctrl->fib_nb.notifier_call = otto_flow_fib_event;
	err = register_fib_notifier(&init_net, &ctrl->fib_nb, NULL, NULL);
	if (err)
		goto unregister_neigh;
	ctrl->debugfs = debugfs_create_dir("realtek_otto_flow", NULL);
	debugfs_create_file("flows", 0400, ctrl->debugfs, ctrl, &otto_flow_debugfs_fops);
	debugfs_create_file("status", 0400, ctrl->debugfs, ctrl, &otto_flow_status_fops);
	debugfs_create_file("hardware", 0400, ctrl->debugfs, ctrl, &otto_flow_hardware_fops);
	mutex_lock(&priv->reg_mutex);
	priv->flow_ctrl = ctrl;
	mutex_unlock(&priv->reg_mutex);
	if (otto_l3_fasttrack(priv))
		return otto_flow_set_enabled(priv, true);
	return 0;
unregister_neigh:
	unregister_netevent_notifier(&ctrl->ne_nb);
unregister_netdev:
	unregister_netdevice_notifier(&ctrl->nd_nb);
free:
	cancel_work_sync(&ctrl->invalidate_work);
	cancel_work_sync(&ctrl->neigh_work);
	kref_put(&ctrl->refs, otto_flow_ctrl_free);
	return err;
}

/* The L3 transition prepares the classifier while ordinary route forwarding
 * is still enabled. Do not admit connections until every FIB row is safe.
 * Caller holds RTNL, but no resource or register lock.
 */
int otto_flow_prepare(struct otto_dsa_priv *priv)
{
	struct otto_flow_ctrl *ctrl = otto_flow_ctrl_get(priv);
	int err;

	if (!ctrl)
		return -EOPNOTSUPP;
	mutex_lock(&ctrl->lock);
	err = ctrl->priv ? otto_pie_930x_flow_init(priv) : -ENODEV;
	mutex_unlock(&ctrl->lock);
	kref_put(&ctrl->refs, otto_flow_ctrl_free);
	return err;
}

int otto_flow_set_enabled(struct otto_dsa_priv *priv, bool enabled)
{
	struct otto_flow_ctrl *ctrl = otto_flow_ctrl_get(priv);
	int err = 0;

	if (!ctrl)
		return -EOPNOTSUPP;
	mutex_lock(&ctrl->lock);
	if (!ctrl->priv) {
		err = -ENODEV;
		goto out;
	}
	if (enabled) {
		if (!otto_l3_fasttrack(priv)) {
			err = -EINVAL;
			goto out;
		}
		err = otto_pie_930x_flow_init(priv);
		if (!err)
			ctrl->enabled = true;
	} else {
		ctrl->enabled = false;
		atomic_inc(&ctrl->generation);
		otto_flow_flush(ctrl, OTTO_FLOW_DISABLED);
		/* Also undo a prepare that never opened the admission gate. */
		err = otto_pie_930x_flow_cleanup(priv);
	}
out:
	mutex_unlock(&ctrl->lock);
	kref_put(&ctrl->refs, otto_flow_ctrl_free);
	return err;
}

void otto_flow_cleanup(struct otto_dsa_priv *priv)
{
	struct otto_flow_ctrl *ctrl;

	mutex_lock(&priv->reg_mutex);
	ctrl = priv->flow_ctrl;
	priv->flow_ctrl = NULL;
	mutex_unlock(&priv->reg_mutex);
	if (!ctrl)
		return;
	debugfs_remove_recursive(ctrl->debugfs);
	unregister_fib_notifier(&init_net, &ctrl->fib_nb);
	unregister_netevent_notifier(&ctrl->ne_nb);
	unregister_netdevice_notifier(&ctrl->nd_nb);
	cancel_work_sync(&ctrl->invalidate_work);
	cancel_work_sync(&ctrl->neigh_work);
	mutex_lock(&ctrl->lock);
	ctrl->enabled = false;
	otto_flow_flush(ctrl, OTTO_FLOW_SHUTDOWN);
	otto_pie_930x_flow_cleanup(priv);
	ctrl->priv = NULL;
	mutex_unlock(&ctrl->lock);
	kref_put(&ctrl->refs, otto_flow_ctrl_free);
}

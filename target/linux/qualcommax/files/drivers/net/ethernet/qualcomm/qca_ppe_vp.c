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
#include <linux/jhash.h>
#include <linux/netdevice.h>
#include <linux/netfilter.h>
#include <linux/netfilter_ipv4.h>
#include <linux/netfilter_netdev.h>
#include <linux/qca_edma.h>
#include <linux/seq_file.h>
#include <net/ip.h>
#include <net/tcp.h>

#include "qca_ppe.h"

/* What the port did, per CPU. */
struct ppe_vp_stats {
	u64 sent;
	u64 bytes;
	u64 refused;
	u64 parked;
	u64 dropped;
	u64 held;
	u64 passed;
	u64 returned;
	u64 aggr;
	u64 segs;
};

/**
 * struct ppe_vp - one virtual source port
 * @priv: the switch
 * @ops: its netdev ingress hook
 * @stats: what it did
 * @hooked: @ops is registered
 * @dev: the bridge port it stands for
 * @br: the bridge it is a port of
 * @dev_tracker: reference on @dev
 * @br_tracker: reference on @br
 * @rcu: defers the free past readers of the port table
 * @id: the port number in the PPE
 * @vsi: the bridge's VSI the port is classified into
 */
struct ppe_vp {
	struct qca_ppe_priv *priv;
	struct nf_hook_ops ops;
	struct ppe_vp_stats __percpu *stats;
	bool hooked;
	struct net_device *dev;
	struct net_device *br;
	netdevice_tracker dev_tracker;
	netdevice_tracker br_tracker;
	struct rcu_head rcu;
	u8 id;
	u8 vsi;
};

/* Run before every other netdev ingress hook of the port, nftables chains and
 * flowtables included: a frame of a flow the PPE forwards is taken from the
 * receive path at once, so that neither a chain nor the flowtable's software
 * path sees, counts or forwards it a second time. Frames the PPE forwards for
 * the switch's ports never reach the CPU at all, so none of those run for them
 * either.
 */
#define PPE_VP_HOOK_PRIORITY	NF_IP_PRI_FIRST

/* The bytes of a frame's headers the parse may read in the head. */
#define PPE_VP_HDR_MAX		(sizeof(struct ipv6hdr) + sizeof(struct tcphdr))

/* How long the flow's frames take the host's path after the PPE returned
 * one, and how often one frame of a flow is let through on purpose.
 */
#define PPE_VP_HOLD		HZ
#define PPE_VP_PASS		HZ

/* The tuple of a frame a flow can be offloaded for, with the headers read from
 * @nh, @len bytes of which are in the head and @total of which are in the
 * frame. Anything the PPE would not take as a plain routed TCP or UDP packet
 * is refused: IPv4 options, fragments, IPv6 extension headers and a length the
 * packet does not have, which the host's own receive path would also drop.
 */
static bool ppe_vp_parse(const u8 *nh, unsigned int len, unsigned int total,
			 __be16 proto, struct ppe_vp_key *key, u8 *ttl,
			 u8 *tcpflags)
{
	const u8 *l4;
	unsigned int l4len;

	memset(key, 0, sizeof(*key));
	*tcpflags = 0;

	if (proto == htons(ETH_P_IP)) {
		const struct iphdr *iph = (const void *)nh;

		if (len < sizeof(*iph) || iph->version != 4 || iph->ihl != 5 ||
		    iph->frag_off & htons(IP_MF | IP_OFFSET) ||
		    ntohs(iph->tot_len) < sizeof(*iph) ||
		    ntohs(iph->tot_len) > total || ip_fast_csum(nh, 5))
			return false;

		key->proto = iph->protocol;
		key->addr.v4[0] = iph->saddr;
		key->addr.v4[1] = iph->daddr;
		*ttl = iph->ttl;
		l4 = nh + sizeof(*iph);
		l4len = len - sizeof(*iph);
	} else if (proto == htons(ETH_P_IPV6)) {
		const struct ipv6hdr *ip6h = (const void *)nh;

		if (len < sizeof(*ip6h) || ip6h->version != 6 ||
		    !ip6h->payload_len ||
		    sizeof(*ip6h) + ntohs(ip6h->payload_len) > total)
			return false;

		key->v6 = 1;
		key->proto = ip6h->nexthdr;
		key->addr.v6[0] = ip6h->saddr;
		key->addr.v6[1] = ip6h->daddr;
		*ttl = ip6h->hop_limit;
		l4 = nh + sizeof(*ip6h);
		l4len = len - sizeof(*ip6h);
	} else {
		return false;
	}

	if (key->proto == IPPROTO_TCP) {
		const struct tcphdr *th = (const void *)l4;

		if (l4len < sizeof(*th) || th->doff < 5)
			return false;
		key->port[0] = th->source;
		key->port[1] = th->dest;
		*tcpflags = tcp_flag_byte(th);
	} else if (key->proto == IPPROTO_UDP) {
		const struct udphdr *uh = (const void *)l4;

		if (l4len < sizeof(*uh))
			return false;
		key->port[0] = uh->source;
		key->port[1] = uh->dest;
	} else {
		return false;
	}

	return true;
}

static struct hlist_head *ppe_vp_bucket(struct qca_ppe_priv *priv,
					const struct ppe_vp_key *key)
{
	return &priv->vp_hash[jhash2((const u32 *)key, sizeof(*key) / 4, 0) &
			      priv->vp_hash_mask];
}

static struct ppe_vp_flow *ppe_vp_flow_find(struct qca_ppe_priv *priv,
					    const struct ppe_vp_key *key)
{
	struct ppe_vp_flow *f;

	hlist_for_each_entry_rcu(f, ppe_vp_bucket(priv, key), node)
		if (!memcmp(&f->key, key, sizeof(*key)))
			return f;

	return NULL;
}

/* A CPU's frames mostly belong to the flow of the frame before, as a GRO
 * burst or a train of one stream is, so the tuple is compared with the last
 * one found before it is hashed. The entry the cache names is freed only a
 * grace period after the generation moves on, and a frame is handled inside
 * one.
 */
static struct ppe_vp_flow *ppe_vp_flow_lookup(struct qca_ppe_priv *priv,
					      const struct ppe_vp_key *key)
{
	struct ppe_vp_cache *c = this_cpu_ptr(priv->vp_cache);
	/* Pairs with the release in ppe_vp_flow_add() and ppe_vp_flow_del(). */
	unsigned int gen = smp_load_acquire(&priv->vp_gen);
	struct ppe_vp_flow *f;

	if (likely(c->gen == gen && c->f && !memcmp(&c->key, key, sizeof(*key))))
		return c->f;

	f = ppe_vp_flow_find(priv, key);
	if (f) {
		c->key = *key;
		c->f = f;
		c->gen = gen;
	}

	return f;
}

/**
 * ppe_vp_flow_add - make a flow's frames stealable
 * @priv: the switch
 * @f: the flow's part, with its key filled in
 *
 * A flow with no virtual port in its key is left alone. Called with the flow
 * lock held, once the flow is in the hardware.
 */
void ppe_vp_flow_add(struct qca_ppe_priv *priv, struct ppe_vp_flow *f)
{
	lockdep_assert_held(&priv->flow_lock);

	if (!f->key.vp)
		return;

	f->hold = 0;
	f->pass = jiffies;
	hlist_add_head_rcu(&f->node, ppe_vp_bucket(priv, &f->key));
	/* After the table changed, so a reader of the new count sees it. */
	smp_store_release(&priv->vp_gen, priv->vp_gen + 1);
}

/**
 * ppe_vp_flow_del - stop stealing a flow's frames
 * @priv: the switch
 * @f: the flow's part
 *
 * Called with the flow lock held, before the flow leaves the hardware. The
 * entry is freed only after the readers that may have found it are done.
 */
void ppe_vp_flow_del(struct qca_ppe_priv *priv, struct ppe_vp_flow *f)
{
	lockdep_assert_held(&priv->flow_lock);

	if (f->key.vp) {
		hlist_del_init_rcu(&f->node);
		/* After the table changed, as in ppe_vp_flow_add(). */
		smp_store_release(&priv->vp_gen, priv->vp_gen + 1);
	}
}

/* A frame of an offloaded flow goes to the PPE as if it had arrived on the
 * port's own switch port, and is gone from the receive path; every other frame
 * is left for the bridge, the firewall and the routing.
 *
 * The frame must be one the PPE would route: addressed to the bridge, an
 * untagged TCP or UDP packet of IPv4 or IPv6 that is not a fragment, has no
 * options and has a TTL left, and not one a TCP flag sends to the CPU. Then it
 * must belong to a flow in the hardware that ingresses on this port. The flow
 * lets one frame in PPE_VP_PASS through for the bridge to learn the source
 * address from, and none for PPE_VP_HOLD after the PPE returned one.
 */
static unsigned int ppe_vp_hook(void *data, struct sk_buff *skb,
				const struct nf_hook_state *state)
{
	struct ppe_vp *vp = data;
	struct qca_ppe_priv *priv = vp->priv;
	struct ppe_vp_stats *st;
	struct net_device *conduit;
	struct ppe_vp_flow *f;
	struct ppe_vp_key key;
	unsigned long t;
	unsigned int len, segs;
	u8 ttl, flags;
	int rc;

	if (skb->protocol != htons(ETH_P_IP) &&
	    skb->protocol != htons(ETH_P_IPV6))
		return NF_ACCEPT;

	if (skb_vlan_tag_present(skb) || skb_shared(skb) ||
	    !skb_mac_header_was_set(skb) ||
	    skb_mac_header(skb) + ETH_HLEN != skb->data ||
	    !ether_addr_equal(eth_hdr(skb)->h_dest, vp->br->dev_addr))
		return NF_ACCEPT;

	/* A GRO aggregate of TCP goes to the engine whole, which cuts it into
	 * the frames it was built from, so the switch sees those and the flow
	 * is looked up for each, with the lookup done here once. An aggregate
	 * whose type says more than that, such as a CWR mark the engine would
	 * copy to every segment, is left to the host.
	 */
	if (skb_is_gso(skb)) {
		u32 tcp = skb->protocol == htons(ETH_P_IP) ? SKB_GSO_TCPV4 :
							     SKB_GSO_TCPV6;
		u32 type = skb_shinfo(skb)->gso_type;

		if (!(type & tcp) ||
		    type & ~(tcp | SKB_GSO_TCP_FIXEDID | SKB_GSO_FRAGLIST))
			return NF_ACCEPT;
	}

	if (!pskb_may_pull(skb, min_t(unsigned int, skb->len, PPE_VP_HDR_MAX)))
		return NF_ACCEPT;

	if (!ppe_vp_parse(skb->data, skb_headlen(skb), skb->len, skb->protocol,
			  &key, &ttl, &flags) ||
	    ttl <= 1 || flags & (TCPHDR_FIN | TCPHDR_SYN | TCPHDR_RST))
		return NF_ACCEPT;

	key.vp = vp->id;
	f = ppe_vp_flow_lookup(priv, &key);
	if (!f)
		return NF_ACCEPT;

	st = this_cpu_ptr(vp->stats);

	t = READ_ONCE(f->hold);
	if (t) {
		if (time_before(jiffies, t)) {
			st->held++;
			return NF_ACCEPT;
		}
		WRITE_ONCE(f->hold, 0);
	}
	t = READ_ONCE(f->pass);
	if (time_after_eq(jiffies, t)) {
		WRITE_ONCE(f->pass, jiffies + PPE_VP_PASS);
		st->passed++;
		return NF_ACCEPT;
	}

	conduit = dsa_to_port(&priv->ds, QCA_PPE_CPU_PORT)->conduit;
	len = skb->len + ETH_HLEN;
	segs = skb_is_gso(skb) ? skb_shinfo(skb)->gso_segs : 0;

	/* The frame is not sent the host's way when the ring has no room, or
	 * when the engine cannot take it whole: it would arrive behind the
	 * frames of its flow the switch is still forwarding. The transmit
	 * keeps it, in order, until the ring has room, cuts an aggregate the
	 * engine cannot take into its segments, and frees what it cannot
	 * keep. Only a conduit that cannot be sent on refuses the frame.
	 */
	skb_push(skb, ETH_HLEN);
	rc = qca_edma_src_xmit(skb, conduit, vp->id);
	if (rc < 0) {
		skb_pull(skb, ETH_HLEN);
		st->refused++;
		return NF_ACCEPT;
	}

	if (rc == QCA_EDMA_SRC_DROPPED) {
		st->dropped++;
	} else {
		if (rc == QCA_EDMA_SRC_PARKED)
			st->parked++;
		st->sent++;
		st->bytes += len;
		if (segs) {
			st->aggr++;
			st->segs += segs;
		}
	}

	return NF_STOLEN;
}

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
	free_percpu(vp->stats);
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

	/* No frame is stolen for the port once the hook is gone and the readers
	 * in it have finished.
	 */
	if (vp->hooked) {
		nf_unregister_net_hook(dev_net(vp->dev), &vp->ops);
		synchronize_net();
		vp->hooked = false;
	}

	/* The flows keyed on the port go with it: the flowtable puts back what
	 * is still live, and a port that is no longer there refuses them.
	 */
	ppe_flow_drop_vp(priv, vp->id, NULL);
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
	vp->stats = alloc_percpu(struct ppe_vp_stats);
	if (!vp->stats) {
		kfree(vp);
		return;
	}

	vp->priv = priv;
	vp->id = id;
	vp->vsi = vsi;
	vp->dev = dev;
	vp->br = br;
	netdev_hold(dev, &vp->dev_tracker, GFP_KERNEL);
	netdev_hold(br, &vp->br_tracker, GFP_KERNEL);

	vp->ops.hook = ppe_vp_hook;
	vp->ops.priv = vp;
	vp->ops.pf = NFPROTO_NETDEV;
	vp->ops.hooknum = NF_NETDEV_INGRESS;
	vp->ops.priority = PPE_VP_HOOK_PRIORITY;
	vp->ops.dev = dev;

	ppe_vp_hw_set(priv, id, vsi);
	rcu_assign_pointer(priv->vp[id], vp);
	priv->vp_next = (id - first + 1) % n;

	/* Last: a frame can be stolen from here on. A refusal leaves the
	 * netdev on the ordinary path with no port.
	 */
	if (nf_register_net_hook(dev_net(dev), &vp->ops))
		ppe_vp_destroy(priv, vp);
	else
		vp->hooked = true;
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
 * ppe_vp_ingress - the flow offload's view of a virtual port's netdev
 * @priv: the switch
 * @ifindex: the ingress interface a flow was seen on
 * @port: set to a switch port of the same bridge
 * @id: set to the virtual port
 *
 * A virtual port is classified into the bridge's VSI as the bridge's switch
 * ports are, so a flow from it is built as one from the first of them.
 * Called with the flow lock held.
 *
 * Return: 0, or -ENODEV when @ifindex has no virtual port.
 */
int ppe_vp_ingress(struct qca_ppe_priv *priv, int ifindex, int *port, u8 *id)
{
	unsigned int i, p;

	lockdep_assert_held(&priv->flow_lock);

	for (i = priv->data->first_vport; i < PPE_MAX_VPORT; i++) {
		struct ppe_vp *vp = rcu_dereference_protected(priv->vp[i],
				lockdep_is_held(&priv->flow_lock));

		if (!vp || vp->dev->ifindex != ifindex)
			continue;

		for (p = 0; p < priv->ds.num_ports; p++) {
			if (priv->port_br_dev[p] != vp->br)
				continue;

			*port = p;
			*id = vp->id;
			return 0;
		}
	}

	return -ENODEV;
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

	/* The frame is back because the PPE had no use for it, so the host
	 * takes the flow's next frames for a while rather than send each
	 * through the PPE to see it come back, and does not steal this one
	 * again.
	 */
	if (skb_headlen(skb) >= ETH_HLEN) {
		struct ppe_vp_key key;
		struct ppe_vp_flow *f;
		u8 ttl, flags;

		if (ppe_vp_parse(skb->data + ETH_HLEN, skb_headlen(skb) - ETH_HLEN,
				 skb->len - ETH_HLEN,
				 ((struct ethhdr *)skb->data)->h_proto, &key,
				 &ttl, &flags)) {
			key.vp = vp->id;
			f = ppe_vp_flow_find(priv, &key);
			if (f)
				WRITE_ONCE(f->hold, jiffies + PPE_VP_HOLD);
		}
	}
	this_cpu_inc(vp->stats->returned);

	skb->dev = vp->dev;
	skb_record_rx_queue(skb, 0);
	skb->protocol = eth_type_trans(skb, vp->dev);
	netif_receive_skb(skb);

	return true;
}

static int ppe_vp_show(struct seq_file *s, void *data)
{
	struct qca_ppe_priv *priv = s->private;
	struct qca_edma_src_stats es;
	unsigned int id, used = 0;

	guard(mutex)(&priv->flow_lock);

	seq_puts(s, "vport netdev bridge vsi sent bytes refused parked dropped held passed returned aggr segs\n");

	for (id = priv->data->first_vport; id < PPE_MAX_VPORT; id++) {
		struct ppe_vp *vp = rcu_dereference_protected(priv->vp[id],
				lockdep_is_held(&priv->flow_lock));

		if (!vp)
			continue;

		struct ppe_vp_stats sum = {};
		int cpu;

		for_each_possible_cpu(cpu) {
			const struct ppe_vp_stats *st = per_cpu_ptr(vp->stats, cpu);

			sum.sent += READ_ONCE(st->sent);
			sum.bytes += READ_ONCE(st->bytes);
			sum.refused += READ_ONCE(st->refused);
			sum.parked += READ_ONCE(st->parked);
			sum.dropped += READ_ONCE(st->dropped);
			sum.held += READ_ONCE(st->held);
			sum.passed += READ_ONCE(st->passed);
			sum.returned += READ_ONCE(st->returned);
			sum.aggr += READ_ONCE(st->aggr);
			sum.segs += READ_ONCE(st->segs);
		}

		used++;
		seq_printf(s, "%-5u %s %s %u %llu %llu %llu %llu %llu %llu %llu %llu %llu %llu\n",
			   id, vp->dev->name, vp->br->name, vp->vsi, sum.sent,
			   sum.bytes, sum.refused, sum.parked, sum.dropped,
			   sum.held, sum.passed, sum.returned, sum.aggr,
			   sum.segs);
	}

	/* The backlog is the conduit's, so it is counted for every port. */
	qca_edma_src_stats(dsa_to_port(&priv->ds, QCA_PPE_CPU_PORT)->conduit,
			   &es);
	seq_printf(s, "backlog parked %llu drained %llu dropped %llu stop: drv %llu stack %llu ring %llu order %llu split %llu segs %llu\n",
		   es.parked, es.drained, es.dropped, es.park_drv,
		   es.park_stack, es.park_ring, es.park_order, es.split,
		   es.split_segs);
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
	int ret;

	/* A flow with a virtual port is a flow of the table, so the table
	 * sizes the buckets and the memory is the same however many ports
	 * there are.
	 */
	priv->vp_hash_mask = roundup_pow_of_two(priv->data->num_flow_entries) - 1;
	priv->vp_hash = kvcalloc(priv->vp_hash_mask + 1, sizeof(*priv->vp_hash),
				 GFP_KERNEL);
	if (!priv->vp_hash)
		return -ENOMEM;

	priv->vp_cache = alloc_percpu(struct ppe_vp_cache);
	if (!priv->vp_cache) {
		kvfree(priv->vp_hash);
		return -ENOMEM;
	}

	priv->vp_nb.notifier_call = ppe_vp_netdev_event;
	ret = register_netdevice_notifier(&priv->vp_nb);
	if (ret) {
		free_percpu(priv->vp_cache);
		kvfree(priv->vp_hash);
	}

	return ret;
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
	free_percpu(priv->vp_cache);
	kvfree(priv->vp_hash);
}

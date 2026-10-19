// SPDX-License-Identifier: GPL-2.0-only
/* Keep the resolved neighbours behind a flow's saved Ethernet rewrite. */

#include <linux/etherdevice.h>
#include <linux/if_vlan.h>
#include <linux/rtnetlink.h>
#include <linux/slab.h>
#include <net/arp.h>
#include <net/ndisc.h>
#include <net/neighbour.h>
#include <net/net_namespace.h>

#include "flow_neigh.h"

struct otto_flow_neigh {
	struct list_head list;
	struct neighbour *neigh;
};

struct otto_flow_neigh_search {
	struct list_head *head;
	struct net *net;
	const u8 *smac;
	const u8 *dmac;
	u16 vid;
	int err;
};

static void otto_flow_neigh_collect(struct neighbour *neigh, void *cookie)
{
	struct otto_flow_neigh_search *search = cookie;
	struct net_device *dev = neigh->dev;
	struct otto_flow_neigh *entry;

	if (search->err || dev->addr_len != ETH_ALEN ||
	    !net_eq(dev_net(dev), search->net) ||
	    !ether_addr_equal(dev->dev_addr, search->smac) ||
	    (is_vlan_dev(dev) && vlan_dev_vlan_id(dev) != search->vid))
		return;

	/* neigh_for_each holds the table lock with BH disabled. A neighbour
	 * can still change its state or MAC while it remains in that table.
	 */
	read_lock_bh(&neigh->lock);
	if (neigh->dead || !(neigh->nud_state & NUD_VALID) ||
	    !ether_addr_equal(neigh->ha, search->dmac)) {
		read_unlock_bh(&neigh->lock);
		return;
	}
	entry = kmalloc(sizeof(*entry), GFP_ATOMIC);
	if (!entry) {
		search->err = -ENOMEM;
		read_unlock_bh(&neigh->lock);
		return;
	}
	entry->neigh = neigh_clone(neigh);
	list_add_tail(&entry->list, search->head);
	read_unlock_bh(&neigh->lock);
}

int otto_flow_neigh_get(struct list_head *head, struct net *net,
			const u8 *smac, const u8 *dmac, u16 vid, int family)
{
	struct neigh_table *table;
	struct otto_flow_neigh_search search = {
		.head = head,
		.net = net,
		.smac = smac,
		.dmac = dmac,
		.vid = vid,
	};

	ASSERT_RTNL();
	INIT_LIST_HEAD(head);
	if (family == AF_INET)
		table = &arp_tbl;
	else if (IS_REACHABLE(CONFIG_IPV6) && family == AF_INET6)
		table = &nd_tbl;
	else
		return -EAFNOSUPPORT;
	neigh_for_each(table, otto_flow_neigh_collect, &search);
	if (search.err) {
		otto_flow_neigh_put(head);
		return search.err;
	}
	return list_empty(head) ? -EAGAIN : 0;
}

bool otto_flow_neigh_valid(struct list_head *head, u64 dmac)
{
	struct otto_flow_neigh *entry;
	u8 mac[ETH_ALEN];

	if (list_empty(head))
		return false;
	u64_to_ether_addr(dmac, mac);
	list_for_each_entry(entry, head, list) {
		struct neighbour *neigh = entry->neigh;
		bool valid;

		read_lock_bh(&neigh->lock);
		valid = !neigh->dead && (neigh->nud_state & NUD_VALID) &&
			ether_addr_equal(neigh->ha, mac);
		read_unlock_bh(&neigh->lock);
		if (!valid)
			return false;
	}
	return true;
}

void otto_flow_neigh_put(struct list_head *head)
{
	struct otto_flow_neigh *entry, *tmp;

	list_for_each_entry_safe(entry, tmp, head, list) {
		list_del(&entry->list);
		neigh_release(entry->neigh);
		kfree(entry);
	}
}

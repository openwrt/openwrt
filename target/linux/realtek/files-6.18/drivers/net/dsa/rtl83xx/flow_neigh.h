/* SPDX-License-Identifier: GPL-2.0-only */
#ifndef _OTTO_FLOW_NEIGH_H
#define _OTTO_FLOW_NEIGH_H

#include <linux/list.h>
#include <linux/types.h>

struct net;

/* Get initializes an empty output list and requires RTNL. The caller
 * serializes list access; valid and put require no RTNL.
 */
int otto_flow_neigh_get(struct list_head *head, struct net *net,
			const u8 *smac, const u8 *dmac, u16 vid, int family);
bool otto_flow_neigh_valid(struct list_head *head, u64 dmac);
void otto_flow_neigh_put(struct list_head *head);

#endif /* _OTTO_FLOW_NEIGH_H */

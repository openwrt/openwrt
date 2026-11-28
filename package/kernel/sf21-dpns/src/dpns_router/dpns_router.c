/*
* Description
*
* Copyright (C) 2016-2023 Qin.Xia <qin.xia@siflower.com.cn>
*
* Siflower software
*/
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/bitops.h>
#include <linux/bitfield.h>
#include <linux/of_platform.h>
#include <linux/mfd/syscon.h>
#include <linux/inetdevice.h>
#include <linux/list_sort.h>
#include <linux/mutex.h>
#include <net/fib_rules.h>
#include <net/fib_notifier.h>
#include <net/netevent.h>
#include <net/nexthop.h>
#include <net/arp.h>
#include <net/ndisc.h>
#include <net/cfg80211.h>
#include "dpns_router.h"

#ifdef CONFIG_SF_REBOOT_RECORD
#include <siflower/sys_events.h>
#endif

static struct pppoe_info ppp_info[PPPOE_WAN_MAX];
static char lan_dev[IFNAMSIZ] = "br-lan";

static const char* fib_event_names[] = {
	"ent-replace", 	// FIB_EVENT_ENTRY_REPLACE,
	"ent-append", 	// FIB_EVENT_ENTRY_APPEND,
	"ent-add", 	// FIB_EVENT_ENTRY_ADD,
	"ent-del", 	// FIB_EVENT_ENTRY_DEL,
	"rule-add", 	// FIB_EVENT_RULE_ADD,
	"rule-del", 	// FIB_EVENT_RULE_DEL,
	"nh-add", 	// FIB_EVENT_NH_ADD,
	"nh-del", 	// FIB_EVENT_NH_DEL,
	"vif-add", 	// FIB_EVENT_VIF_ADD,
	"vif-del", 	// FIB_EVENT_VIF_DEL,
	NULL,		// FIB_EVENT_MAX
};

const char* fib_event_name(enum fib_event_type ev)
{
#ifdef CONFIG_SF_REBOOT_RECORD
	SF_BUG_ON(ETH_BUG, (ev > FIB_EVENT_VIF_DEL));
#else
	BUG_ON(ev > FIB_EVENT_VIF_DEL);
#endif

	return fib_event_names[ev];
}

static int dpns_add_or_get_l2_idx(MAC_t* mac_priv, u8 port_id, u16 ovid, u8* dmac)
{
        int l2_index, hit;
	u64 port_bitmap;
        u32 result_data[2];

        hit = mac_priv->hw_search(mac_priv, dmac, ovid, result_data);


        port_bitmap = FIELD_GET(SE_HW_RESULT0_DATA1_BITMAP_19_26,
                        result_data[1]) <<19 |
                        FIELD_GET(SE_HW_RESULT0_DATA0_BITMAP_0_18,
                        result_data[0]);

        if (!hit || (hit && fls64(port_bitmap)-1 != port_id)) {
                l2_index = mac_priv->mac_table_update(
                                                mac_priv,
                                                dmac,
                                                true,
                                                ovid,
                                                BIT(port_id),
                                                false,
                                                true,
                                                CML_FORWARD,
                                                CML_FORWARD,
                                                0,
                                                0,
                                                0);
                if (l2_index < 0)
                        goto err_mac_add;
        } else {
                l2_index = FIELD_GET(SE_HW_RESULT0_DATA1_MAC_IDX, result_data[1]);
        }

        return l2_index;
err_mac_add:
        L3_DBG(DBG_LV, "err mac add dmac:%pM ovid:%u\n",
                        dmac, ovid);
        return -1;
}

void dpns_router_table4_add(COMMON_t* priv, struct router_tbl_entry *entry)
{
	union l3_uc_ipv4_table_cfg key = {}, mask = {};

	key.table.next_hop_ptr = entry->next_hop_ptr;
	key.table.intf_id      = entry->intf_index;
	key.table.oport_id     = entry->ovport;
	key.table.ovid         = entry->ovid;
	key.table.dip          = *(u32 *)entry->addr;

	memset(&mask, 0xff, sizeof(mask));
	mask.table.dip = ~ntohl(inet_make_mask(entry->prefix_len));

	priv->tcam_update(priv, TCAM_L3UCMCG, entry->req_id,
			entry->req_addr, key.data, mask.data, sizeof(key), TBID_KMD_V4_UC);
}

void dpns_router_table6_add(COMMON_t* priv, struct router_tbl_entry *entry)
{
	union l3_uc_ipv6_table_cfg key = {}, mask = {};

	key.table.next_hop_ptr = entry->next_hop_ptr;
	key.table.intf_id      = entry->intf_index;
	key.table.oport_id     = entry->ovport;
	key.table.ovid         = entry->ovid;
	key.table.dip          = *(u128 *)entry->addr;

	memset(&mask, 0xff, sizeof(mask));

	if (entry->prefix_len == (u8)0) {
		mask.table.dip = (u128)0 - 1;
	} else {
		mask.table.dip = ((u128)1 << (128 - entry->prefix_len)) - 1;
	}

        priv->tcam_update(priv, TCAM_L3UCMCG, entry->req_id,
                        entry->req_addr, key.data, mask.data, sizeof(key), TBID_KMD_V6_UC);
}

void dpns_router_table_add(COMMON_t* priv, struct router_tbl_entry *entry)
{
	if (entry->addr_len == V4_ADDR_LEN) {
		dpns_router_table4_add(priv, entry);
	} else {                     //v6
		dpns_router_table6_add(priv, entry);
	}
}

void dpns_router_table4_del(COMMON_t* priv, struct router_tbl_entry *entry)
{
	union l3_uc_ipv4_table_cfg key = {}, mask = {};

	priv->tcam_update(priv, TCAM_L3UCMCG, entry->req_id, entry->req_addr,
			key.data, mask.data, sizeof(key), TBID_KMD_V4_UC);
}

void dpns_router_table6_del(COMMON_t* priv, struct router_tbl_entry *entry)
{
	union l3_uc_ipv6_table_cfg key = {}, mask = {};

        priv->tcam_update(priv, TCAM_L3UCMCG, entry->req_id,
                        entry->req_addr, key.data, mask.data, sizeof(key), TBID_KMD_V6_UC);
}

void dpns_router_table_del(COMMON_t* priv, struct router_tbl_entry *entry)
{
	if (entry->addr_len == V4_ADDR_LEN)
		dpns_router_table4_del(priv, entry);
	else
		dpns_router_table6_del(priv, entry);
}

static int dpns_router_table_cmp(void *priv, const struct list_head *a,
		const struct list_head *b)
{
	struct router_tbl_entry *entry1, *entry2;

	entry1 = container_of(a, struct router_tbl_entry, node);
	entry2 = container_of(b, struct router_tbl_entry, node);

	if (entry1->prefix_len == entry2->prefix_len)
		return (entry1->prio -  entry2->prio);

	return (entry1->prefix_len - entry2->prefix_len);
}

/*
 * IPv4 max router table entry is 4 * 8 (rows * columns)
 * */
static void dpns_router_table4_reorder(COMMON_t* priv)
{
	struct router_tbl_entry *pos;
	int i = 7, j = 3;

	// order by desc
	list_sort(NULL, &priv->router_priv->rt4_list, dpns_router_table_cmp);

	// write to router table by desc
	list_for_each_entry(pos, &priv->router_priv->rt4_list, node) {
		pos->req_id = TCAM_BLK_RAM_ID(TCAM_L3UCMCG, i);
		pos->req_addr = j;

		if (pos->flags == DPNS_NEIGH_UNRESOLVED)
			continue;
		dpns_router_table_add(priv, pos);
		j = (i == 0) ? (j - 1) : j;
		i = (i == 0) ? 7 : (i - 1);
		if (j < 0) {
			L3_DBG(WARN_LV, "router table full\n");
			return;
		}
	}
}

/*
 * IPv6 max router table entry is 4 * 2 (rows * columns)
 * */
static void dpns_router_table6_reorder(COMMON_t* priv)
{
	struct router_tbl_entry *pos;
	int i = 1, j = 7;

	// order by desc
	list_sort(NULL, &priv->router_priv->rt6_list, dpns_router_table_cmp);

	// write to router table by desc
	list_for_each_entry(pos, &priv->router_priv->rt6_list, node) {
		pos->req_id = TCAM_BLK_RAM_ID(TCAM_L3UCMCG, i * 4);
		pos->req_addr = j;
		dpns_router_table_add(priv, pos);
		j = (i == 0) ? (j - 1) : j;
		i = (i == 0) ? 1 : (i - 1);
		if (j < 4) {
			L3_DBG(WARN_LV, "router table full\n");
			return;
		}
	}
}

void dump_dpns_router4_tbl(COMMON_t* priv)
{
	union l3_uc_ipv4_table_cfg key, mask;
	u8 i, j, req_id, req_addr;
	u32 v4_addr;

	for (i = 0; i < 4; i++) { //v4 table has 4 rows and 8 cols
		for (j = 0; j < 8; j++) {
			req_id = TCAM_BLK_RAM_ID(TCAM_L3UCMCG, j);
			req_addr = i * 2;
			memset(&key, 0, sizeof(key));
			memset(&mask, 0, sizeof(mask));
			priv->tcam_access(priv, SE_OPT_R, req_id, req_addr,
					key.data, sizeof(key));
			priv->tcam_access(priv, SE_OPT_R, req_id, (req_addr+1),
					mask.data, sizeof(mask));
			if (mask.table.ovid != 0) {
				printk("%s: req_id:%u req_addr:%u\n",
						__func__, req_id, req_addr);
				v4_addr = key.table.dip;
				printk("%s: DATA: next_hop_ptr:%u oport_id:%u "
						"intf_id:%u dip:%pI4 ovid:%u\n",
						__func__,
						key.table.next_hop_ptr,
						key.table.oport_id,
						key.table.intf_id,
						(void *)&v4_addr,
						key.table.ovid);
				v4_addr = mask.table.dip;
				printk("%s: MASK: next_hop_ptr:%x oport_id:%x "
						"intf_id:%x dip:%pI4 ovid:%x\n",
						__func__,
						mask.table.next_hop_ptr,
						mask.table.oport_id,
						mask.table.intf_id,
						(void *)&v4_addr,
						mask.table.ovid);
			}
		}
	}
}

void dump_dpns_router6_tbl(COMMON_t* priv)
{
	union l3_uc_ipv6_table_cfg key, mask;
	u8 i, j, req_id, req_addr;
	u128 v6_addr;

	for (i = 4; i < 8; i++) { //v6 table has 4 rows and 2 cols
		for (j = 0; j < 2; j++) {
			req_id = TCAM_BLK_RAM_ID(TCAM_L3UCMCG, j * 4);
			req_addr = i * 2;
			memset(&key, 0, sizeof(key));
			memset(&mask, 0, sizeof(mask));

			priv->tcam_access(priv, SE_OPT_R, req_id, req_addr,
						key.data, sizeof(key));
			priv->tcam_access(priv, SE_OPT_R, req_id, req_addr+1,
						mask.data, sizeof(mask));

			if (mask.table.ovid != 0) {
				printk("%s: req_id:%u req_addr:%u\n",
						__func__, req_id, req_addr);
				v6_addr = key.table.dip;
				printk("%s: DATA: next_hop_ptr:%u oport_id:%u "
						"intf_id:%u ovid:%u dip: %pI6\n",
						__func__,
						key.table.next_hop_ptr,
						key.table.oport_id,
						key.table.intf_id,
						key.table.ovid,
						(void *)&v6_addr);

				v6_addr = mask.table.dip;
				printk("%s: MASK: next_hop_ptr:%x oport_id:%x "
						"intf_id:%x ovid:%x dip: %pI6\n",
						__func__,
						mask.table.next_hop_ptr,
						mask.table.oport_id,
						mask.table.intf_id,
						mask.table.ovid,
						(void *)&v6_addr);
			}
		}
	}
}

void dump_dpns_router4_list(COMMON_t* priv)
{
	struct router_tbl_entry *pos;

	mutex_lock(&priv->router_priv->lock);
	list_for_each_entry(pos, &priv->router_priv->rt4_list, node) {
		printk("dump router4 ip:%pI4 prefix_len:%u gw:%pI4 "
				"ovid:%u ovport:%u intf_index:%u next_hop_ptr:%u "
				"mac:%pM fib_dev:%s\n",
				pos->addr, pos->prefix_len, pos->gw_addr,
				pos->ovid, pos->ovport, pos->intf_index,
				pos->next_hop_ptr, pos->mac,
				pos->fib_ndev ? pos->fib_ndev->name : "NULL");
	}
	mutex_unlock(&priv->router_priv->lock);
}

void dump_dpns_router6_list(COMMON_t* priv)
{
	struct router_tbl_entry *pos;

	mutex_lock(&priv->router_priv->lock);
	list_for_each_entry(pos, &priv->router_priv->rt6_list, node) {
		printk("dump router6 ip:%pI6 prefix_len:%u gw:%pI6 "
				"ovid:%u ovport:%u intf_index:%u next_hop_ptr:%u "
				"mac:%pM fib_dev:%s\n",
				pos->addr, pos->prefix_len, pos->gw_addr,
				pos->ovid, pos->ovport, pos->intf_index,
				pos->next_hop_ptr, pos->mac,
				pos->fib_ndev ? pos->fib_ndev->name : "NULL");
	}
	mutex_unlock(&priv->router_priv->lock);
}

void dump_dpns_router_tbl(COMMON_t* priv)
{
	dump_dpns_router4_tbl(priv);
	dump_dpns_router6_tbl(priv);
	dump_dpns_router4_list(priv);
	dump_dpns_router6_list(priv);
}

static void sf_router_fib_dump_flush(struct notifier_block *nb)
{
	ROUTER_t *priv = container_of(nb, ROUTER_t, fib_nb);

	/* Flush pending FIB notifications and then flush the device's
	 * table before requesting another dump. The FIB notification
	 * block is unregistered, so no need to take RTNL.
	 */
	flush_workqueue(priv->owq);
	L3_DBG(DBG_LV, "End %s\n", __func__);
}

static struct router_tbl_entry* dpns_router_table_lookup(ROUTER_t *priv,
		const void *addr, size_t addr_len, u8 prefix_len, struct net_device *ndev)
{
	struct router_tbl_entry *pos;

	if (addr_len == V4_ADDR_LEN) {
		list_for_each_entry(pos, &priv->rt4_list, node) {
			if (!memcmp(pos->addr, addr, addr_len) &&
					(pos->prefix_len == prefix_len) &&
					(pos->fib_ndev == ndev))
				return pos;
		}
	}else {
		list_for_each_entry(pos, &priv->rt6_list, node) {
			if (!memcmp(pos->addr, addr, addr_len) &&
					(pos->prefix_len == prefix_len) &&
					(pos->fib_ndev == ndev))
				return pos;
		}
	}

	return NULL;
}

static struct router_tbl_entry* dpns_router_table_lookup_by_gw(ROUTER_t *priv,
		const void *addr, size_t addr_len, u32 l2_index)
{
	struct router_tbl_entry *pos, *found = NULL;

	if (addr_len == V4_ADDR_LEN) {
		list_for_each_entry(pos, &priv->rt4_list, node) {
			if (!memcmp(pos->gw_addr, addr, addr_len) &&
					(pos->next_hop_ptr == l2_index)) {
				found = pos;

				// if same gw already exist, do not add again
				if (pos->next_hop_ptr != DPNS_RESERVED_MAC_INDEX)
					return NULL;
			}
		}
	}else {
		list_for_each_entry(pos, &priv->rt6_list, node) {
			if (!memcmp(pos->gw_addr, addr, addr_len) &&
					(pos->next_hop_ptr == l2_index)) {
				found = pos;

				// if same gw already exist, do not add again
				if (pos->next_hop_ptr != DPNS_RESERVED_MAC_INDEX)
					return NULL;
			}
		}
	}

	return found;
}

static void dpns_router_fib4_add(ROUTER_t *priv,
		const struct fib_entry_notifier_info *fen_info)
{
	COMMON_t *cpriv = priv->cpriv;
	MAC_t *mac_priv = cpriv->mac_priv;
	struct router_tbl_entry *found, *found_gw;
	struct net_device *fib_dev, *org_dev;
	const struct fib_nh *nh;
	int intf_index = -1, l2_index = DPNS_RESERVED_MAC_INDEX, flags = 0, i;
	unsigned int gw_addr = 0;
	bool has_gw;
	u16 ovid = DPA_UNTAGGED_VID;
	u8 pppoe_en = 0, port_id = DPNS_HOST_PORT, smac[ETH_ALEN] = {}, dmac[ETH_ALEN] = {};

	L3_DBG(DBG_LV, "for dst:%pI4 mask:%d type:%d\n",
			&fen_info->dst,
			fen_info->dst_len,
			fen_info->type);

	if (priv->rt4_count >= DPNS_ROUTER_TBL4_MAX) {
		L3_DBG(ERR_LV, "can not find slot for entry:%pI4\n",
				&fen_info->dst);
		return;
	}

	if (!fen_info->fi) {
		L3_DBG(DBG_LV, "fen_info with no fi\n");
		return;
	}

	nh = fib_info_nh(fen_info->fi, 0);
	org_dev = nh->fib_nh_dev;
	fib_dev = nh->fib_nh_dev;
	has_gw = !!nh->fib_nh_gw4;
	gw_addr = ntohl(nh->fib_nh_gw4);

	memcpy(smac, fib_dev->dev_addr, ETH_ALEN);
	if (!strcmp(fib_dev->name, "br-lan")) {
		// in repeater mode br-lan can be as wan
		if (priv->rep_dev) {
			if (!has_gw)
				return;

			memcpy(smac, priv->rep_dev->dev_addr, ETH_ALEN);
			if (cpriv->port_id_by_netdev(cpriv, priv->rep_dev, &port_id) < 0)
					goto err_dpns_dev;
			flags = DPNS_NEIGH_UNRESOLVED;
			sf_update(priv, NPU_NAT_MPP_CFG, 0, NAT_MPP_CFG_BYPASS);
		} else {
			sf_update(priv, NPU_NAT_MPP_CFG, NAT_MPP_CFG_BYPASS, 0);
		}

		if (has_gw) {
			flags = DPNS_NEIGH_UNRESOLVED;
			l2_index = DPNS_RESERVED_MAC_INDEX;
		}
	}else {
		if (!strncmp("pppoe-wan", fib_dev->name, strlen("pppoe-wan")) || !strncmp("pppoe-wwan", fib_dev->name, strlen("pppoe-wwan"))) {
			for (i = 0;i < PPPOE_WAN_MAX; i++) {
				if (!strncmp(fib_dev->name, ppp_info[i].ifname, IFNAMSIZ)) {
					if (!ppp_info[i].valid)
						goto err_ppp_info;
					pppoe_en = 1;
					memcpy(smac, ppp_info[i].rel_dev->dev_addr, ETH_ALEN);
					memcpy(dmac, ppp_info[i].gw_mac, ETH_ALEN);
					fib_dev = ppp_info[i].rel_dev;
					ovid = ppp_info[i].ovid;
					break;
				}
			}
		}else {
			struct neighbour *neigh;
			if (!has_gw)
				goto err_has_gw;

			found_gw = dpns_router_table_lookup_by_gw(priv, &nh->fib_nh_gw4,
						sizeof(fen_info->dst), DPNS_RESERVED_MAC_INDEX);

			neigh = neigh_lookup(&arp_tbl, &nh->fib_nh_gw4,
					fib_dev);
			if (neigh && !found_gw) {
				memcpy(dmac, neigh->ha, ETH_ALEN);
				neigh_release(neigh);
			}else {
				flags = DPNS_NEIGH_UNRESOLVED;
				l2_index = DPNS_RESERVED_MAC_INDEX;
			}

			if (is_vlan_dev(fib_dev)) {
				ovid = vlan_dev_vlan_id(fib_dev);
				fib_dev = vlan_dev_real_dev(fib_dev);
			}
		}

		if (cpriv->port_id_by_netdev(cpriv, fib_dev, &port_id) < 0)
			goto err_dpns_dev;

		if (flags != DPNS_NEIGH_UNRESOLVED) {
			if((l2_index = dpns_add_or_get_l2_idx(mac_priv,
							port_id, ovid, dmac))<0)
				return;
		}
	}

	intf_index = cpriv->intf_add(cpriv, ovid,
			pppoe_en, 0, 1, smac);
	if (intf_index < 0)
		goto err_add_intf;

	found = dpns_router_table_lookup(priv, &fen_info->dst,
			sizeof(fen_info->dst), fen_info->dst_len, org_dev);
	if (found) {
		list_del(&found->node);
	}else {
		found = l3_kzalloc(sizeof(*found), GFP_KERNEL);
		if (!found)
			return;
		priv->rt4_count++;
	}

	found->fib_ndev = org_dev;
	found->next_hop_ptr = (u32)l2_index;
	found->intf_index = intf_index;
	found->ovport = port_id;
	found->ovid = ovid;
	found->prio = fen_info->fi->fib_priority;
	found->type = fen_info->type;
	found->prefix_len = fen_info->dst_len;
	found->addr_len = sizeof(fen_info->dst);
	memcpy(found->addr, &fen_info->dst, found->addr_len);

	if (!is_zero_ether_addr(dmac))
		ether_addr_copy(found->mac, dmac);

	if (has_gw)
		memcpy(found->gw_addr, (u8*)&gw_addr, found->addr_len);
	list_add(&found->node, &priv->rt4_list);
	L3_DBG(DBG_LV, "add router ip:%pI4 dst_len:%u gw:%pI4 "
			"ovid:%u smac:%pM dmac:%pM\n",
			found->addr, found->prefix_len, &gw_addr,
			ovid, smac, dmac);

	if (flags != DPNS_NEIGH_UNRESOLVED)
		dpns_router_table4_reorder(cpriv);

	return;

err_add_intf:
	L3_DBG(DBG_LV, "err intf add smac:%pM ovid:%u\n",
			smac, ovid);
	return;
err_dpns_dev:
	L3_DBG(DBG_LV, "err fib_dev:%s is not a dpns ndev\n",
			fib_dev->name);
	return;
err_has_gw:
	L3_DBG(DBG_LV, "err find gw for:%pI4 dev:%s\n",
			&fen_info->dst, fib_dev->name);
	return;
err_ppp_info:
	L3_DBG(DBG_LV, "invalid ppp info for intf table\n");
}

static void dpns_router_fib4_del(ROUTER_t *priv,
		const struct fib_entry_notifier_info *fen_info)
{
	MAC_t *mac_priv = priv->cpriv->mac_priv;
	struct router_tbl_entry *found, *found_gw = NULL;
	struct net_device *fib_dev;
	const struct fib_nh *nh;
	unsigned int gw_addr = 0;
	int l2_index;
	bool has_gw;

	L3_DBG(DBG_LV, "for dst:%pI4 mask:%d type:%d\n",
			&fen_info->dst,
			fen_info->dst_len,
			fen_info->type);

	if (!fen_info->fi) {
		L3_DBG(DBG_LV, "fen_info with no fi\n");
		return;
	}

	nh = fib_info_nh(fen_info->fi, 0);
	fib_dev = nh->fib_nh_dev;
	has_gw = !!nh->fib_nh_gw4;
	gw_addr = ntohl(nh->fib_nh_gw4);

	found = dpns_router_table_lookup(priv, &fen_info->dst,
			sizeof(fen_info->dst), fen_info->dst_len, fib_dev);
	if (!found)
		return;

	L3_DBG(DBG_LV, "del router ip:%pI4 mac:%pM ovport:%d l2_index:%u intf:%u\n",
			found->addr, found->mac, found->ovport,
			found->next_hop_ptr, found->intf_index);
	if (found->next_hop_ptr != DPNS_RESERVED_MAC_INDEX)
		dpns_router_table_del(priv->cpriv, found);
	list_del(&found->node);
	priv->rt4_count--;

	// del wan mac
	if(!is_zero_ether_addr(found->mac)) {
		L3_DBG(DBG_LV, "this is the gate way mac del\n");
		priv->cpriv->mac_priv->mac_del_entry(priv->cpriv->mac_priv,
						found->mac, found->ovid, true, false);
		if (has_gw)
			found_gw = dpns_router_table_lookup_by_gw(priv, &gw_addr,
					sizeof(fen_info->dst), DPNS_RESERVED_MAC_INDEX);

		if (found_gw) {
			l2_index = mac_priv->mac_table_update(
					mac_priv,
					found->mac,
					true,
					found_gw->ovid,
					BIT(found_gw->ovport),
					false,
					true,
					CML_FORWARD,
					CML_FORWARD,
					0,
					0,
					0);
			if (l2_index >= 0) {
				found_gw->next_hop_ptr = l2_index;
				L3_DBG(DBG_LV, "update router ip:%pI4 dst_len:%u gw:%pI4 "
						"ovid:%u mac:%pM dev:%s\n",
						found_gw->addr, found_gw->prefix_len, &gw_addr,
						found_gw->ovid, found->mac, found_gw->fib_ndev->name);
				dpns_router_table4_reorder(priv->cpriv);
			}
		}
	}

	priv->cpriv->intf_del(priv->cpriv, found->intf_index);
	l3_kfree(found);
}

static void dpns_nexthop4_event(ROUTER_t *priv,
		unsigned long event, struct fib_nh *fib_nh)
{
	MAC_t *mac_priv = priv->cpriv->mac_priv;
	struct router_tbl_entry *found;
	struct net_device *fib_dev;
	struct neighbour *neigh;
	u64 port_bitmap;
	bool is_del = (event == FIB_EVENT_NH_DEL);
	unsigned int ipaddr;
	int hit;
	int l2_index = -1;
	u32 result_data[2];
	u16 dmac_index;

	L3_DBG(DBG_LV, "for %s nh_saddr:%pI4 fib_nh_gw4:%pI4\n",
			fib_event_name(event),
			&fib_nh->nh_saddr,
			&fib_nh->fib_nh_gw4);

	if (!fib_nh->fib_nh_gw4)
		return;

	fib_dev = fib_nh->fib_nh_dev;
	neigh = neigh_lookup(&arp_tbl, &fib_nh->fib_nh_gw4, fib_dev);
	if (!neigh)
		return;

	ipaddr = ntohl(fib_nh->fib_nh_gw4);
	found = dpns_router_table_lookup_by_gw(priv, (u8*)&ipaddr,
			sizeof(u32), DPNS_RESERVED_MAC_INDEX);
	if (!found) {
		L3_DBG(DBG_LV, "find route by gw:%pI4 dev:%s failed\n",
				&ipaddr, fib_dev->name);
		goto out;
	}

	if (is_del) {
		priv->rt4_count--;
		list_del(&found->node);
		dpns_router_table_del(priv->cpriv, found);
		l3_kfree(found);
		L3_DBG(DBG_LV, "del router next_hot_ptr by gateway mac:%pM\n",
				neigh->ha);
		goto out;
	}

	hit = mac_priv->hw_search(mac_priv, neigh->ha, found->ovid, result_data);

	port_bitmap = FIELD_GET(SE_HW_RESULT0_DATA1_BITMAP_19_26,
			result_data[1]) <<19 |
			FIELD_GET(SE_HW_RESULT0_DATA0_BITMAP_0_18,
			result_data[0]);

	if (!hit || ( hit && fls64(port_bitmap)-1 != found->ovport )) {
		l2_index = mac_priv->mac_table_update(
				mac_priv,
				neigh->ha,
				true,
				found->ovid,
				BIT(found->ovport),
				false,
				true,
				CML_FORWARD,
				CML_FORWARD,
				0,
				0,
				0);
		if (l2_index < 0)
			goto out;
	} else {
        	dmac_index = FIELD_GET(SE_HW_RESULT0_DATA1_MAC_IDX, result_data[1]);
		l2_index = dmac_index;
	}

	found->next_hop_ptr = l2_index;
	dpns_router_table4_reorder(priv->cpriv);
	L3_DBG(DBG_LV, "add router next_hot_ptr by gateway mac:%pM\n",
			neigh->ha);

out:
	neigh_release(neigh);
}

/*
 * type : like RTN_UNICAST
 */
static void dpns_router_fib4_event_work(struct work_struct *work)
{
	struct dpns_fib_event_work *fib_work =
		container_of(work, struct dpns_fib_event_work, work);
	ROUTER_t *priv = fib_work->priv;

	mutex_lock(&priv->lock);

	L3_DBG(DBG_LV, "for v4 %s\n", fib_event_name(fib_work->event));
	switch (fib_work->event) {
	case FIB_EVENT_ENTRY_REPLACE:
		dpns_router_fib4_add(priv, &fib_work->fen_info);
		fib_info_put(fib_work->fen_info.fi);
		break;
	case FIB_EVENT_ENTRY_DEL:
		dpns_router_fib4_del(priv, &fib_work->fen_info);
		fib_info_put(fib_work->fen_info.fi);
		break;
	case FIB_EVENT_NH_ADD:
	case FIB_EVENT_NH_DEL:
		dpns_nexthop4_event(priv, fib_work->event,
					fib_work->fnh_info.fib_nh);
		fib_info_put(fib_work->fnh_info.fib_nh->nh_parent);
		break;
	}
	mutex_unlock(&priv->lock);
	l3_kfree(fib_work);
}

static int dpns_router_fib_rule_event(unsigned long event,
		struct fib_notifier_info *info,
		ROUTER_t *priv)
{
	struct fib_rule_notifier_info *fr_info;
	struct fib_rule *rule;
	int err = 0;

	/* nothing to do at the moment */
	/* qin: only ipv6 trigger this, don't know why */
	if (event == FIB_EVENT_RULE_DEL)
		return 0;

	fr_info = container_of(info, struct fib_rule_notifier_info, info);
	rule = fr_info->rule;

	/* Rule only affects locally generated traffic */
	if (rule->iifindex == init_net.loopback_dev->ifindex)
		return 0;

	switch (info->family) {
	case AF_INET:
		if (!fib4_rule_default(rule) && !rule->l3mdev)
			err = -EOPNOTSUPP;
		break;
	case AF_INET6:
		if (!fib6_rule_default(rule) && !rule->l3mdev)
			err = -EOPNOTSUPP;
		break;
	}

	/* qin: it seems only rule target 0.0.0.0 enter this func
	 * mellanox do noting here, rocker just do abort while error condition
	 * we don't need fib_aborted, so do nothing here
	 * */
	L3_DBG(DBG_LV, "event:0x%lx family:%u target:0x%x action:%u\n",
			event, info->family, rule->target,
			rule->action);
	return err;
}

static void dpns_router_fib4_event(struct dpns_fib_event_work *fib_work,
		struct fib_notifier_info *info)
{
	struct fib_entry_notifier_info *fen_info;
	struct fib_nh_notifier_info *fnh_info;

	switch (fib_work->event) {
	case FIB_EVENT_ENTRY_REPLACE:
	case FIB_EVENT_ENTRY_DEL:
		fen_info = container_of(info, struct fib_entry_notifier_info,
					info);
		fib_work->fen_info = *fen_info;
		/* Take reference on fib_info to prevent it from being
		 * freed while work is queued. Release it afterwards.
		 */
		fib_info_hold(fib_work->fen_info.fi);
		break;
	case FIB_EVENT_NH_ADD:
	case FIB_EVENT_NH_DEL:
		fnh_info = container_of(info, struct fib_nh_notifier_info,
					info);
		fib_work->fnh_info = *fnh_info;
		fib_info_hold(fib_work->fnh_info.fib_nh->nh_parent);
		break;
	}
}

static void dpns_router_fib6_add(ROUTER_t *priv,
		const struct fib6_entry_notifier_info *fen6_info)
{
	COMMON_t *cpriv = priv->cpriv;
	MAC_t *mac_priv = cpriv->mac_priv;
	struct router_tbl_entry *found, *found_gw;
	struct net_device *fib_dev, *org_dev;
	const struct fib6_nh *fib6_nh;
	int i;
	int intf_index = -1, l2_index = DPNS_RESERVED_MAC_INDEX, flags = 0;
	u128 gw_addr = 0;
	bool has_gw;
	u16 ovid = DPA_UNTAGGED_VID;
	u8 pppoe_en = 0, port_id = DPNS_HOST_PORT, smac[ETH_ALEN] = {},
			dmac[ETH_ALEN] = {}, data[16];

	L3_DBG(DBG_LV, "for dst:%pI6 mask:%d type:%d\n",
                         &fen6_info->rt->fib6_dst.addr.in6_u,
                         fen6_info->rt->fib6_dst.plen,
                         fen6_info->rt->fib6_type);

	if (priv->rt6_count >= DPNS_ROUTER_TBL6_MAX) {
		L3_DBG(ERR_LV, "can not find slot for entry:%pI6\n",
		&fen6_info->rt->fib6_dst.addr.in6_u);
		return;
	}

	if (!fen6_info->rt) {
		L3_DBG(DBG_LV, "fen6_info with no rt\n");
		return;
	}

	fib6_nh = fen6_info->rt->fib6_nh;
	fib_dev = fib6_nh->nh_common.nhc_dev;
	org_dev = fib6_nh->nh_common.nhc_dev;
	has_gw  = !(*(u128 *)fib6_nh->nh_common.nhc_gw.ipv6.in6_u.u6_addr8 == (u128)0);
	gw_addr = *(u128 *)fib6_nh->nh_common.nhc_gw.ipv6.in6_u.u6_addr8;
	memcpy(smac, fib_dev->dev_addr, ETH_ALEN);

	if(!strcmp(fib_dev->name, "br-lan")) {
		// in repeater mode br-lan can be as wan
		if (has_gw) {
 			flags = DPNS_NEIGH_UNRESOLVED;
		}
	}else {
		if (!strncmp("pppoe-wan", fib_dev->name, strlen("pppoe-wan")) || !strncmp("pppoe-wwan", fib_dev->name, strlen("pppoe-wwan"))) {
			for (i = 0;i < PPPOE_WAN_MAX; i++) {
				if (!strncmp(fib_dev->name, ppp_info[i].ifname, IFNAMSIZ)) {
					if (!ppp_info[i].valid)
						goto err_ppp_info;
					pppoe_en = 1;
					memcpy(smac, ppp_info[i].rel_dev->dev_addr, ETH_ALEN);
					memcpy(dmac, ppp_info[i].gw_mac, ETH_ALEN);
					fib_dev = ppp_info[i].rel_dev;
					ovid = ppp_info[i].ovid;
					break;
				}
			}
		}else {
			struct neighbour *neigh;
			if (!has_gw)
				goto err_has_gw;

			found_gw = dpns_router_table_lookup_by_gw(priv, (u8*)&gw_addr,
						V6_ADDR_LEN, DPNS_RESERVED_MAC_INDEX);

			neigh = neigh_lookup(&nd_tbl, &fib6_nh->nh_common.nhc_gw, fib_dev);

			if (neigh && !found_gw) {
				memcpy(dmac, neigh->ha, ETH_ALEN);
				neigh_release(neigh);
			}else {
				L3_DBG(DBG_LV, "gw unresolved: %pI6\n",
					&fen6_info->rt->fib6_dst.addr.in6_u);
				flags = DPNS_NEIGH_UNRESOLVED;
				l2_index = DPNS_RESERVED_MAC_INDEX;
			}

			if (is_vlan_dev(fib_dev)) {
				ovid = vlan_dev_vlan_id(fib_dev);
				fib_dev = vlan_dev_real_dev(fib_dev);
			}
		}

		if (cpriv->port_id_by_netdev(cpriv, fib_dev, &port_id) < 0) {
			goto err_dpns_dev;
		}

		if (flags != DPNS_NEIGH_UNRESOLVED) {
			if((l2_index = dpns_add_or_get_l2_idx(mac_priv, port_id, ovid,
                                                dmac))<0)
                        return;
		}
	}

	intf_index = cpriv->intf_add(cpriv, ovid, pppoe_en,
					0, 1, smac);
	if (intf_index < 0)
		goto err_add_intf;

	for (i = 0; i < 16; i++) {
		data[i] = fen6_info->rt->fib6_dst.addr.in6_u.u6_addr8[15-i];
	}

	found = dpns_router_table_lookup(priv, data,
			V6_ADDR_LEN, fen6_info->rt->fib6_dst.plen, org_dev);
	if (found) {
		list_del(&found->node);
	}else {
		found = l3_kzalloc(sizeof(*found), GFP_KERNEL);
		if(!found)
			return;
		priv->rt6_count++;
	}

	found->fib_ndev = org_dev;
	found->next_hop_ptr = (u32)l2_index;
	found->intf_index = intf_index;
	found->ovport = port_id;
	found->ovid = ovid;
	found->prio = fen6_info->rt->fib6_metric;
	found->type = fen6_info->rt->fib6_type;
	found->prefix_len = fen6_info->rt->fib6_dst.plen;
	found->addr_len = V6_ADDR_LEN;
	ether_addr_copy(found->mac, dmac);

	memcpy(found->addr, data, V6_ADDR_LEN);
	if (has_gw)
		memcpy(found->gw_addr, (u8*)&gw_addr, found->addr_len);
	list_add(&found->node, &priv->rt6_list);

	L3_DBG(DBG_LV, "add router ip:%pI6 dst_len:%u gw:%pI6 "
                         "ovid:%u smac:%pM dmac:%pM\n",
                         found->addr, found->prefix_len, &gw_addr,
                         ovid, smac, dmac);

	if (flags != DPNS_NEIGH_UNRESOLVED)
		dpns_router_table6_reorder(cpriv);

	return;

err_add_intf:
	L3_DBG(DBG_LV, "err intf add smac:%pM ovid:%u\n",
			smac, ovid);
	return;
err_dpns_dev:
	L3_DBG(DBG_LV, "err fib_dev:%s is not a dpns ndev\n",
			fib_dev->name);
	return;
err_has_gw:
	L3_DBG(DBG_LV, "err find gw for:%pI6 dev:%s\n",
			&fen6_info->rt->fib6_dst.addr.in6_u, fib_dev->name);
         return;
err_ppp_info:
	L3_DBG(DBG_LV, "invalid ppp info for intf table\n");
}

static void dpns_router_fib6_del(ROUTER_t *priv,
		const struct fib6_entry_notifier_info *fen6_info)
{
	MAC_t *mac_priv = priv->cpriv->mac_priv;
	struct router_tbl_entry *found, *found_gw = NULL;
	struct net_device *fib_dev;
	const struct fib6_nh *fib6_nh;
	u8 v6_addr[V6_ADDR_LEN];
	u128 gw_addr = 0;
	bool has_gw;
	int i, l2_index;

	L3_DBG(DBG_LV, "for dst:%pI6 mask: %d type: %d \n",
			&fen6_info->rt->fib6_dst.addr.in6_u,
			fen6_info->rt->fib6_dst.plen,
			fen6_info->rt->fib6_type);

	for (i = 0; i < V6_ADDR_LEN; i ++){
		v6_addr[i] = fen6_info->rt->fib6_dst.addr.in6_u.u6_addr8[V6_ADDR_LEN - 1 - i];
	}

	fib6_nh = fen6_info->rt->fib6_nh;
	fib_dev = fib6_nh->nh_common.nhc_dev;
	has_gw  = !(*(u128 *)fib6_nh->nh_common.nhc_gw.ipv6.in6_u.u6_addr8 == (u128)0);
	gw_addr = *(u128 *)fib6_nh->nh_common.nhc_gw.ipv6.in6_u.u6_addr8;
	found = dpns_router_table_lookup(priv, v6_addr,
					V6_ADDR_LEN, fen6_info->rt->fib6_dst.plen, fib_dev);
	if (!found)
		return;

	L3_DBG(DBG_LV, "del router ip:%pI6 ovport:%d l2_index:%u intf:%u\n",
			found->addr, found->ovport,
			found->next_hop_ptr, found->intf_index);
	if (found->next_hop_ptr != DPNS_RESERVED_MAC_INDEX)
		dpns_router_table_del(priv->cpriv, found);
	list_del(&found->node);
	priv->rt6_count--;
	// del wan mac
	if(!is_zero_ether_addr(found->mac)) {
		L3_DBG(DBG_LV, "this is the gate way6 mac del\n");
		priv->cpriv->mac_priv->mac_del_entry(priv->cpriv->mac_priv,
						found->mac, found->ovid, true, false);
		if (has_gw)
			found_gw = dpns_router_table_lookup_by_gw(priv, (u8*)&gw_addr,
						V6_ADDR_LEN, DPNS_RESERVED_MAC_INDEX);

		if (found_gw) {
			l2_index = mac_priv->mac_table_update(
					mac_priv,
					found->mac,
					true,
					found_gw->ovid,
					BIT(found_gw->ovport),
					false,
					true,
					CML_FORWARD,
					CML_FORWARD,
					0,
					0,
					0);
			if (l2_index >= 0) {
				found_gw->next_hop_ptr = l2_index;
				L3_DBG(DBG_LV, "update router ip:%pI6 dst_len:%u gw:%pI6 "
						"ovid:%u mac:%pM dev:%s\n",
						found_gw->addr, found_gw->prefix_len, &gw_addr,
						found_gw->ovid, found->mac, found_gw->fib_ndev->name);
				dpns_router_table4_reorder(priv->cpriv);
			}
		}
	}

	priv->cpriv->intf_del(priv->cpriv, found->intf_index);
	l3_kfree(found);
}

static void inline dpns_router_fib6_replace(ROUTER_t *priv,
            const struct fib6_entry_notifier_info *fen6_info)
{
    struct router_tbl_entry *found;
    struct net_device *fib_dev;
    const struct fib6_nh *fib6_nh;
    u8 data[16];
    int i;

	if (!fen6_info->rt) {
		L3_DBG(DBG_LV, "fen6_info with no rt\n");
		return;
	}

    for (i = 0; i < V6_ADDR_LEN; i++) {
        data[i] = fen6_info->rt->fib6_dst.addr.in6_u.u6_addr8[V6_ADDR_LEN -1 - i];
    }
	fib6_nh = fen6_info->rt->fib6_nh;
	fib_dev = fib6_nh->nh_common.nhc_dev;
    found = dpns_router_table_lookup(priv, data,
             V6_ADDR_LEN, fen6_info->rt->fib6_dst.plen, fib_dev);

    if(!found){
        if(priv->rt6_count < DPNS_ROUTER_TBL6_MAX){
            dpns_router_fib6_add(priv, fen6_info);
        }
    }else {
        list_del(&found->node);
        priv->rt6_count--;
        dpns_router_table_del(priv->cpriv, found);
        l3_kfree(found);
        dpns_router_fib6_add(priv, fen6_info);
    }
}

static void dpns_router_fib6_event_work(struct work_struct *work)
{
	struct dpns_fib_event_work *fib_work =
		container_of(work, struct dpns_fib_event_work, work);
	ROUTER_t *priv = fib_work->priv;
	struct fib6_entry_notifier_info *fen6_info = &fib_work->fen6_info;

	mutex_lock(&priv->lock);

	L3_DBG(DBG_LV, "for v6 %s\n", fib_event_name(fib_work->event));
	switch (fib_work->event) {
	case FIB_EVENT_ENTRY_REPLACE:
		dpns_router_fib6_replace(priv, fen6_info);
		fib6_info_release(fen6_info->rt);
		break;
	case FIB_EVENT_ENTRY_APPEND:
		dpns_router_fib6_add(priv, fen6_info);
		fib6_info_release(fen6_info->rt);
		break;
	case FIB_EVENT_ENTRY_DEL:
		dpns_router_fib6_del(priv, fen6_info);
		fib6_info_release(fen6_info->rt);
		break;
	default :
		fib6_info_release(fen6_info->rt);
		break;
	}

	mutex_unlock(&priv->lock);
	l3_kfree(fib_work);
}

static int dpns_router_fib6_event(struct dpns_fib_event_work *fib_work,
		struct fib_notifier_info *info)
{
	struct fib6_entry_notifier_info *fen6_info;

	switch (fib_work->event) {
	case FIB_EVENT_ENTRY_REPLACE:
	case FIB_EVENT_ENTRY_APPEND:
	case FIB_EVENT_ENTRY_DEL:
		fen6_info = container_of(info, struct fib6_entry_notifier_info,
					 info);
		fib6_info_hold(fen6_info->rt);
		fib_work->fen6_info = *fen6_info;
		break;
	}

	return 0;
}

/* Called with rcu_read_lock() */
static int dpns_router_fib_event(struct notifier_block *nb,
				   unsigned long event, void *ptr)
{
	ROUTER_t *priv = container_of(nb, ROUTER_t, fib_nb);
	struct fib_notifier_info *info = ptr;
	struct dpns_fib_event_work *fib_work;
	int err;

	if (info->family != AF_INET && info->family != AF_INET6)
		return NOTIFY_DONE;

	switch (event) {
	case FIB_EVENT_RULE_ADD:
	case FIB_EVENT_RULE_DEL:
		err = dpns_router_fib_rule_event(event, info, priv);
		return NOTIFY_DONE;
	case FIB_EVENT_ENTRY_ADD:
	case FIB_EVENT_ENTRY_REPLACE:
	case FIB_EVENT_ENTRY_APPEND:
		if (info->family == AF_INET) {
			struct fib_entry_notifier_info *fen_info = ptr;

			if (fen_info->fi->fib_nh_is_v6) {
				L3_DBG(ERR_LV, "IPv6 gateway with IPv4 route is "
						"not supported\n");
				return notifier_from_errno(-EINVAL);
			}

			if (fen_info->fi->nh) {
				L3_DBG(ERR_LV, "IPv4 route with nexthop objects "
						"is not supported\n");
				return notifier_from_errno(-EINVAL);
			}

			// dpns ucast route table only handle RT_TABLE_MAIN
			if (fen_info->tb_id != RT_TABLE_MAIN)
				return NOTIFY_DONE;

		}else if (info->family == AF_INET6){
			struct fib6_entry_notifier_info *fen6_info;

			fen6_info = container_of(info,
					struct fib6_entry_notifier_info,
					info);
			if(fen6_info->rt->nh) {
				L3_DBG(ERR_LV, "IPv6 route with nexthop objects "
						"is not supported\n");
				return notifier_from_errno(-EINVAL);
			}

			// dpns ucast route table only handle RT_TABLE_MAIN
			if (fen6_info->rt->fib6_table->tb6_id != RT_TABLE_MAIN)
				return NOTIFY_DONE;
		}
		break;
	}

	fib_work = l3_kzalloc(sizeof(*fib_work), GFP_ATOMIC);
	if (!fib_work)
		return NOTIFY_DONE;

	fib_work->priv = priv;
	fib_work->event = event;

	switch (info->family) {
	case AF_INET:
		INIT_WORK(&fib_work->work, dpns_router_fib4_event_work);
		dpns_router_fib4_event(fib_work, info);
		break;
	case AF_INET6:
		INIT_WORK(&fib_work->work, dpns_router_fib6_event_work);
		err = dpns_router_fib6_event(fib_work, info);
		if (err)
			goto err_out;
		break;
	default:
		goto err_out;
	}

	queue_work(priv->owq, &fib_work->work);

	return NOTIFY_DONE;

err_out:
	l3_kfree(fib_work);
	return NOTIFY_DONE;
}

static void dpns_router_neigh_event_work(struct work_struct *work)
{
	struct dpns_netevent_work *net_work =
		container_of(work, struct dpns_netevent_work, work);
	ROUTER_t *priv = net_work->priv;
	COMMON_t *cpriv = priv->cpriv;
	MAC_t *mac_priv = cpriv->mac_priv;
	struct neighbour *n = net_work->n;
	struct router_tbl_entry *found;
	u64 port_bitmap;
	bool is_add = (n->nud_state & NUD_VALID) && !(n->dead);
	int hit;
	u32 l2_index, addrlen, ipaddr;
	u32 result_data[2];
	u16 dmac_index;
	u8 *ip_addr = (u8 *)n->primary_key;

	if (is_zero_ether_addr(n->ha))
		goto err_ha;

	switch (net_work->family) {
	case AF_INET:
		ipaddr = ntohl(*((__be32 *) n->primary_key));
		ip_addr = (u8 *)&ipaddr;
		addrlen = sizeof(u32);
		L3_DBG(DBG_LV, "update neigh mac:%pM ip:%pI4 dev:%s\n",
				n->ha, ip_addr, n->dev->name);
		break;
	case AF_INET6:
		addrlen = sizeof(struct in6_addr);
		L3_DBG(DBG_LV, "update neigh mac:%pM ip:%pI6 dev:%s\n",
				n->ha, ip_addr, n->dev->name);
		break;
	}

	mutex_lock(&priv->lock);
	found = dpns_router_table_lookup_by_gw(priv, ip_addr, addrlen,
			DPNS_RESERVED_MAC_INDEX);
	if (!found)
		goto out;

	if (is_add) {
        	hit = mac_priv->hw_search(mac_priv, n->ha, found->ovid, result_data);


	         port_bitmap = FIELD_GET(SE_HW_RESULT0_DATA1_BITMAP_19_26,
		                 result_data[1]) <<19 |
			         FIELD_GET(SE_HW_RESULT0_DATA0_BITMAP_0_18,
				 result_data[0]);

		if (!hit || ( hit && fls64(port_bitmap)-1 != found->ovport )) {
			l2_index = mac_priv->mac_table_update(
					mac_priv,
					n->ha,
					true,
					found->ovid,
					BIT(found->ovport),
					false,
					true,
					CML_FORWARD,
					CML_FORWARD,
					0,
					0,
					0);
			if (l2_index < 0)
				goto out;
		} else {
        		dmac_index = FIELD_GET(SE_HW_RESULT0_DATA1_MAC_IDX, result_data[1]);
			l2_index = dmac_index;
		}
		found->next_hop_ptr = l2_index;
		found->flags = 0;
		ether_addr_copy(found->mac, n->ha);

		if(found->addr_len == V4_ADDR_LEN)
			dpns_router_table4_reorder(cpriv);
		else
			dpns_router_table6_reorder(cpriv);
		L3_DBG(DBG_LV, "add router next_hot_ptr by gateway mac:%pM\n",
				n->ha);
	}else {
		if (found->addr_len == V4_ADDR_LEN)
			priv->rt4_count--;
		else
			priv->rt6_count--;
		list_del(&found->node);
		dpns_router_table_del(cpriv, found);
		//TODO:del mac wan
		l3_kfree(found);
		L3_DBG(DBG_LV, "del router next_hot_ptr by gateway mac:%pM\n",
				n->ha);
	}

out:
	mutex_unlock(&priv->lock);
err_ha:
	neigh_release(n);
	l3_kfree(net_work);
}

static int dpns_router_netevent_event(struct notifier_block *nb,
				   unsigned long event, void *ptr)
{
	ROUTER_t *priv = container_of(nb, ROUTER_t, netevent_nb);
	struct dpns_netevent_work *net_work;
	struct neighbour *n = ptr;

	switch (event) {
	case NETEVENT_NEIGH_UPDATE:
		if (n->tbl->family != AF_INET && n->tbl->family != AF_INET6)
			return NOTIFY_DONE;

		net_work = l3_kzalloc(sizeof(*net_work), GFP_ATOMIC);
		if (!net_work) {
			return NOTIFY_DONE;
		}

		INIT_WORK(&net_work->work, dpns_router_neigh_event_work);
		net_work->family = n->tbl->family;
		net_work->priv = priv;
		net_work->n = n;

		/* Take a reference to ensure the neighbour won't be
		 * destructed until we drop the reference in delayed
		 * work.
		 */
		neigh_clone(n);
		queue_work(priv->owq, &net_work->work);
		break;
	}

	return NOTIFY_DONE;
}


static int dpns_router_netdevice_event(struct notifier_block *this,
		unsigned long event, void *ptr)
{
	struct net_device *dev = netdev_notifier_info_to_dev(ptr);
	struct net_device_path_stack stack;
	struct net_device_path *path;
	ROUTER_t *priv = container_of(this, ROUTER_t, netdevice_nb);
	int i, j;

	switch(event) {
	case NETDEV_UP:

		if (dev->ieee80211_ptr && (dev->ieee80211_ptr->iftype == NL80211_IFTYPE_STATION) &&
				!dev->ieee80211_ptr->use_4addr) {
			priv->rep_dev = dev;
			L3_DBG(DBG_LV, "up rep mac:%pM\n", priv->rep_dev->dev_addr);
		}

		if (strncmp("pppoe-wan", dev->name, strlen("pppoe-wan")) && strncmp("pppoe-wwan", dev->name, strlen("pppoe-wwan")))
			break;

		if (dpns_fill_forward_path(dev, dev->dev_addr, &stack)) {
			L3_DBG(DBG_LV, "mac:%pM get pppoe stack fail\n",
					dev->dev_addr);
			return NOTIFY_DONE;
		}

		for (i = 0; i < stack.num_paths; i++) {
			path = &stack.path[i];
			switch (path->type) {
			case DEV_PATH_PPPOE:
				for (j = 0;j < PPPOE_WAN_MAX; j++) {
					if (ppp_info[j].valid == 0) {
						ppp_info[j].valid = 1;
						ppp_info[j].ppp_sid = path->encap.id;
						memcpy(ppp_info[j].gw_mac,
								path->encap.h_dest,
								ETH_ALEN);
						memcpy(ppp_info[j].ifname, dev->name, IFNAMSIZ);
						break;
					}
				}
				break;
			case DEV_PATH_VLAN:
				if ( j == PPPOE_WAN_MAX)
					break;
				if (is_vlan_dev(path->dev))
					ppp_info[j].ovid = vlan_dev_vlan_id(path->dev);
				break;
			case DEV_PATH_ETHERNET:
				if ( j == PPPOE_WAN_MAX)
					break;
				ppp_info[j].rel_dev =
					(struct net_device *)path->dev;
				L3_DBG(DBG_LV, "get pppoe real dev %s\n",
						ppp_info[j].rel_dev->name);
			default:
				break;
			}
		}
		break;
	case NETDEV_DOWN:
		if ((priv->rep_dev) && !strcmp(dev->name, priv->rep_dev->name)) {
			L3_DBG(DBG_LV, "down rep mac:%pM\n", priv->rep_dev->dev_addr);
			priv->rep_dev = NULL;
		}

		if (strncmp("pppoe-wan", dev->name, strlen("pppoe-wan")) && strncmp("pppoe-wwan", dev->name, strlen("pppoe-wwan")))
			break;

		if (dpns_fill_forward_path(dev, dev->dev_addr, &stack)) {
			L3_DBG(DBG_LV, "mac:%pM get pppoe stack fail\n",
					dev->dev_addr);
			return NOTIFY_DONE;
		}

		for (i = 0; i < stack.num_paths; i++) {
			path = &stack.path[i];
			switch (path->type) {
			case DEV_PATH_ETHERNET:
				for (j = 0; j < PPPOE_WAN_MAX; j++) {
					if (ppp_info[j].valid == 0)
						continue;
					if (strncmp(ppp_info[j].rel_dev->name, ((struct net_device *)path->dev)->name, IFNAMSIZ))
						continue;
					memset(&ppp_info[j], 0, sizeof(struct pppoe_info));
				}
			default:
				break;
			}
		}
		break;
	default:
		break;
	}

	return NOTIFY_DONE;
}

static int dpns_router_inetaddr_event(struct notifier_block *nb,
		unsigned long event, void *ptr)
{
	ROUTER_t *priv = container_of(nb, ROUTER_t, inetaddr_nb);
	COMMON_t *cpriv = priv->cpriv;
	struct in_ifaddr *ifa = ptr;
	struct net_device *dev = ifa->ifa_dev->dev;
	struct router_tbl_entry *found;
	int intf_index = -1;
	u32 ip, ip_mask;
	u8 pppoe_en = 0, smac[ETH_ALEN] = {};

	if (strncmp(lan_dev, dev->name, IFNAMSIZ))
		return NOTIFY_DONE;

	if (!priv->rep_dev)
		return NOTIFY_DONE;

	ip = ntohl(ifa->ifa_address);
	ip_mask = ntohl(ifa->ifa_mask);
	memcpy(smac, dev->dev_addr, ETH_ALEN);
	L3_DBG(DBG_LV, "inetaddr event, ifa addr:%pI4 mask:%pI4 prefixlen:%d, priority:%d dev_addr:%pM\n",
		&ifa->ifa_address, &ifa->ifa_mask, (u32)ifa->ifa_prefixlen, ifa->ifa_rt_priority, dev->dev_addr);

	mutex_lock(&priv->lock);
	switch (event) {
	case NETDEV_UP:
		intf_index = cpriv->intf_add(cpriv, DPA_UNTAGGED_VID,
			pppoe_en, 0, 1, smac);
		if (intf_index < 0)
			break;

		found = l3_kzalloc(sizeof(*found), GFP_KERNEL);
		if (!found)
			break;
		priv->rt4_count++;

		found->next_hop_ptr = DPNS_RESERVED_MAC_INDEX;
		found->intf_index = intf_index;
		found->ovport = DPNS_HOST_PORT;
		found->ovid = DPA_UNTAGGED_VID;
		found->prio = ifa->ifa_rt_priority;
		found->prefix_len = 32;
		found->addr_len = sizeof(ifa->ifa_address);
		memcpy(found->addr, &ip, found->addr_len);

		list_add(&found->node, &priv->rt4_list);
		dpns_router_table4_reorder(cpriv);

		L3_DBG(DBG_LV, "added %pI4 mask:%pI4 to private IPv4\n",
			&ifa->ifa_address, &ifa->ifa_mask);
		break;
	case NETDEV_DOWN:
		found = dpns_router_table_lookup(priv, &ip,
			sizeof(ifa->ifa_address), (u32)ifa->ifa_prefixlen, dev);

		if (found) {
			list_del(&found->node);
			dpns_router_table_del(cpriv, found);
			cpriv->intf_del(cpriv, found->intf_index);
			l3_kfree(found);
		}

		L3_DBG(DBG_LV, "deleted %pI4 mask:%pI4 from private IPv4\n",
			&ifa->ifa_address, &ifa->ifa_mask);
		break;
	default:
		break;
	}
	mutex_unlock(&priv->lock);
	return NOTIFY_DONE;
}

int dpns_router_probe(struct platform_device *pdev)
{
	COMMON_t* common_priv = platform_get_drvdata(pdev);
	ROUTER_t* priv;
	int err;

	priv = devm_kzalloc(&pdev->dev, sizeof(ROUTER_t), GFP_KERNEL);
	if (!priv)
		return -ENOMEM;

	priv->owq = alloc_ordered_workqueue("dpns_router", WQ_MEM_RECLAIM);
	if (!priv->owq)
		return -ENOMEM;

	mutex_init(&priv->lock);
	INIT_LIST_HEAD(&priv->rt4_list);
	INIT_LIST_HEAD(&priv->rt6_list);
	common_priv->router_priv = priv;
	priv->cpriv = common_priv;
	priv->iobase = common_priv->iobase;
	priv->fib_nb.notifier_call = dpns_router_fib_event;
	priv->netevent_nb.notifier_call = dpns_router_netevent_event;
	priv->netdevice_nb.notifier_call = dpns_router_netdevice_event;
	priv->inetaddr_nb.notifier_call = dpns_router_inetaddr_event;

	err = register_netdevice_notifier(&priv->netdevice_nb);
	if (err) {
		dev_err(&pdev->dev, "Failed to register netdevice notifier\n");
		goto err_register_netdevice_notifier;
	}

	err = register_fib_notifier(&init_net, &priv->fib_nb,
			sf_router_fib_dump_flush, NULL);
	if (err) {
		dev_err(&pdev->dev, "Failed to register fib notifier\n");
		goto err_register_fib_notifier;
	}

	err = register_netevent_notifier(&priv->netevent_nb);
	if (err) {
		dev_err(&pdev->dev, "Failed to register netevent notifier\n");
		goto err_register_netevent_notifier;
	}

	err = register_inetaddr_notifier(&priv->inetaddr_nb);
	if (err) {
		dev_err(&pdev->dev, "Failed to register inetaddr notifier\n");
		goto err_register_inetaddr_notifier;
	}

	// clean router table first
	common_priv->tcam_clean(common_priv, TCAM_L3UCMCG);
	dpns_router_genl_init(priv);

	printk("End %s\n", __func__);
	return 0;

err_register_inetaddr_notifier:
	unregister_netevent_notifier(&priv->netevent_nb);
err_register_netevent_notifier:
	unregister_fib_notifier(&init_net, &priv->fib_nb);
err_register_fib_notifier:
	unregister_netdevice_notifier(&priv->netdevice_nb);
err_register_netdevice_notifier:
	destroy_workqueue(priv->owq);
	return err;
}
EXPORT_SYMBOL(dpns_router_probe);

void dpns_router_remove(struct platform_device *pdev)
{
	COMMON_t* common_priv = platform_get_drvdata(pdev);
	ROUTER_t* priv = common_priv->router_priv;

	dpns_router_genl_exit();

	unregister_netdevice_notifier(&priv->netdevice_nb);
	unregister_netevent_notifier(&priv->netevent_nb);
	unregister_fib_notifier(&init_net, &priv->fib_nb);
	unregister_inetaddr_notifier(&priv->inetaddr_nb);
	destroy_workqueue(priv->owq);
	mutex_destroy(&priv->lock);

	common_priv->router_priv = NULL;
	printk("End %s\n", __func__);
}
EXPORT_SYMBOL(dpns_router_remove);

MODULE_LICENSE("GPL v2");
MODULE_AUTHOR("Qin Xia <qin.xia@siflower.com.cn>");
MODULE_DESCRIPTION("DPNS Router Driver");

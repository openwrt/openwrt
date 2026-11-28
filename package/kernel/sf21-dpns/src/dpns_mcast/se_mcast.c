#include <linux/rtnetlink.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/rhashtable.h>
#include <linux/bitfield.h>
#include <linux/bitops.h>
#include <linux/bitmap.h>
#include <linux/slab.h>
#include <linux/mm.h>
#include <linux/io.h>
#include <linux/netdevice.h>
#include <linux/etherdevice.h>
#include <linux/of_platform.h>
#include <linux/compiler.h>
#include <linux/if_vlan.h>
#include "dpns_common.h"
#include "se_mcast.h"

// TODO: if tcam block reset somewhere else, need to sync
// TODO: query bridge fdb/mdb to find interfaces that want mcast
// TODO: internal -> external need to do NAT in some cases
// TODO: when interface mac is changed...
// TODO: VID is HARDCODED as 4095 NOW, dpns port vid api is BROKEN
// TODO: L2 table needs refcount
// TODO: merge macro definitions
// TODO: reset tcam block on module init
// TODO: SE_TCAM_BLK_CONFIG0 MUST BE inited and MCSG assigned somewhere
// TODO: INTF table APIs need to replace
// TODO: REMOVE DEBUG MODULEPARAM INTERFACES
// TODO: CLEAN DEBUG MESSAGES

// keep a copy of hw tcam block
static se_mcsg_blk_t se_mcsg_tcam_blk = { 0 };
u32 mcsg_blk_id = TCAM_BLK_CFG0_BLK_SEL_INVALID;
static const struct rhashtable_params l3_mcast_tbl_params = {
        .key_len = sizeof(((l3_mcast_entry_t *)0)->key),
        .key_offset = offsetof(l3_mcast_entry_t, key),
        .head_offset = offsetof(l3_mcast_entry_t, node),
};

static struct rhashtable l3_mcast_tbl;
static se_l3_mcsg_rule_t mcsg_mask;
MCAST_t* g_mcast = NULL;

extern struct dpns_nat_subnet_info sf_wan_subnet[8];
/* The vendor kernel exported this from the bridge; use the generic lower-device walk instead. */
static void br_get_port_netdev(struct net_device *br_dev,
			       void (*cb)(struct net_device *, void *), void *arg)
{
	struct net_device *lower;
	struct list_head *iter;

	rtnl_lock();
	netdev_for_each_lower_dev(br_dev, lower, iter)
		cb(lower, arg);
	rtnl_unlock();
}

// FIXME: there should be a function like this in linux kernel
static void ipv4_mcast_ethaddr(u32 ip, u8 *mac)
{
	u64 t = 0x01005e000000 + (ip & 0x7fffff);

	u64_to_ether_addr(t, mac);
}

int is_valid_tcam_idx(tcam_blk_idx_t *idx)
{
	if (idx->item < 0 || idx->item >= ARRAY_SIZE(((se_mcsg_blk_t *)0)->items))
		return 0;

	if (idx->slice < 0 || idx->slice >= TCAM_SLICES_PER_ITEM)
		return 0;

	return 1;
}

static int se_mcsg_slice_alloc(se_mcsg_blk_t *blk, tcam_blk_idx_t *idx)
{
	int i;

	idx->item = -1;
	idx->slice = -1;

	for_each_blk_item(i) {
		se_mcsg_tbl_item_t *item = &blk->items[i];
		u32 slice;

		slice = find_first_zero_bit(item->slice_map, TCAM_MCSG_SLICES);
		if (slice >= TCAM_MCSG_SLICES)
			continue;

		set_bit(slice, item->slice_map);

		idx->item = i;
		idx->slice = slice * 2; // mcsg rule takes 2 slices to store

		return 0;
	}

	return -ENOSPC;
}

static int se_mcsg_slice_free(se_mcsg_blk_t *blk, tcam_blk_idx_t *idx)
{
	se_mcsg_tbl_item_t *item = &blk->items[idx->item];

	if (!is_valid_tcam_idx(idx))
		return -EINVAL;

	clear_bit(idx->slice/2, item->slice_map);

	return 0;
}

static void se_l3_mcsg_rule_print(se_l3_mcsg_rule_t *r)
{
	MCAST_DBG(INFO_LV, "intf_idx: %u\n", r->intf_idx);
	MCAST_DBG(INFO_LV, "oport_bitmap: 0x%08llx\n", (u64){ r->oport_bitmap });
	MCAST_DBG(INFO_LV, "iport_id: %u\n", r->iport_id);
	MCAST_DBG(INFO_LV, "dip: 0x%08x %pI4\n", r->dip, &(u32){ htonl(r->dip) });
	MCAST_DBG(INFO_LV, "sip: 0x%08x %pI4\n", r->sip, &(u32){ htonl(r->sip) });
	MCAST_DBG(INFO_LV, "ovid: %u\n", ntohs((u16){ r->ovid }));
}

static void se_l3_entry_print(l3_mcast_entry_t *e)
{
	MCAST_DBG(INFO_LV, "tcam idx: item: %d slice: %d\n", e->tcam_idx.item, e->tcam_idx.slice);
	se_l3_mcsg_rule_print(&e->rule.mcsg);
}

static int se_l3_mcsg_iport_set(se_l3_mcsg_rule_t *rule, se_l3_mcast_cfg_t *cfg)
{
	struct net_device *iif;
	u8 port_id;

	iif = dev_get_by_index(&init_net, cfg->iif);
	if (!iif) {
		MCAST_DBG(INFO_LV, "iif %u is not found\n", cfg->iif);
		return -ENODEV;
	}

	if (g_mcast->cpriv->port_id_by_netdev(g_mcast->cpriv, iif, &port_id)) {
		MCAST_DBG(INFO_LV, "not a dpns port: %s\n", iif->name);
		return -EINVAL;
	}

	rule->iport_id = port_id;

	return 0;
}

static void br_oif_port_id(struct net_device *dev, void *arg)
{
	u64 *port_map = arg;
	u8 port_id;

	if (g_mcast->cpriv->port_id_by_netdev(g_mcast->cpriv, dev, &port_id)) {
		MCAST_DBG(INFO_LV, "not dpns port: %s\n", dev->name);
		return;
	}

	*port_map |= BIT_ULL(port_id);

	MCAST_DBG(INFO_LV, "ifname: %s port_id: %u\n", dev->name, port_id);
}

static inline void oport_vid_set(u16 *oport_vid, u16 vid)
{
	// FIXME: ovid can be different
	if (*oport_vid != 0 && *oport_vid != vid) {
		MCAST_DBG(ERR_LV, "different ovid detected, not supported now\n");
	return;
	}

	*oport_vid = vid;
}

static int se_l3_mcsg_oport_set(se_l3_mcsg_rule_t *rule, se_l3_mcast_cfg_t *cfg)
{
	u16 oport_vid = 0;
	u64 oport_map = 0;
	u32 i;

	for (i = 0; i < cfg->oif_cnt && i < ARRAY_SIZE(cfg->oif); i++) {
		struct net_device *oif = dev_get_by_index(&init_net, cfg->oif[i]);

		if (!oif) {
			MCAST_DBG(INFO_LV, "oif index %d is not found\n", cfg->oif[i]);
			return -ENODEV;
		}

		if (netif_is_bridge_master(oif)) {
			br_get_port_netdev(oif, br_oif_port_id, &oport_map);
		} else {
			u8 port_id;

			if (g_mcast->cpriv->port_id_by_netdev(g_mcast->cpriv, oif, &port_id)) {
				MCAST_DBG(INFO_LV, "skip, not a dpns port: %s\n", oif->name);
				continue;
			}

			oport_map |= BIT_ULL(port_id);
		}

	if(is_vlan_dev(oif))
		oport_vid = vlan_dev_priv(oif)->vlan_id;
	}

	if (oport_map == 0) {
		MCAST_DBG(INFO_LV, "oport_map is empty, no dpns port found\n");
		return -ENOENT;
	}

	rule->oport_bitmap = oport_map;
	rule->ovid = oport_vid;

	return 0;
}

static int se_l3_mcsg_inft_idx_set(se_l3_mcsg_rule_t *rule, se_l3_mcast_cfg_t *cfg)
{
	struct net_device *iif = dev_get_by_index(&init_net, cfg->iif);
	u8 iif_mac[ETH_ALEN] = { };
	u16 ovid = 0;
	int intf_idx;

	if (!iif) {
		MCAST_DBG(ERR_LV, "failed to get net device: %s\n", iif->name);
		return -ENODEV;
	}

	memcpy(iif_mac, iif->dev_addr, ETH_ALEN);
	if(is_vlan_dev(iif))
		ovid = vlan_dev_priv(iif)->vlan_id;

	MCAST_DBG(INFO_LV, "iif %s hwaddr: %pM ovid: %hu\n", iif->name, iif_mac, ovid);

	intf_idx = g_mcast->cpriv->intf_add(g_mcast->cpriv, ovid, 0, 0, 0, iif_mac);
	if (intf_idx < 0) {
		MCAST_DBG(ERR_LV, "failed to add intf table entry, err = %d\n", intf_idx);
		return intf_idx;
	}

	rule->intf_idx = intf_idx;

	MCAST_DBG(INFO_LV, "intf_idx: %d\n", intf_idx);

	return 0;
}

static int se_l3_mcsg_rule_set(se_l3_mcsg_rule_t *rule, se_l3_mcast_cfg_t *cfg)
{
	int err;

	if ((err = se_l3_mcsg_iport_set(rule, cfg)))
		return err;

	if ((err = se_l3_mcsg_oport_set(rule, cfg)))
		return err;

	if ((err = se_l3_mcsg_inft_idx_set(rule, cfg)))
		return err;

	rule->sip = cfg->sip.ip4.d;
	rule->dip = cfg->dip.ip4.d;

	return 0;
}

static int mcsg_mask_set(const char *buf, const struct kernel_param *kp)
{
	u32 intf_idx, iport_id, dip, sip, ovid;
	u64 oport_bitmap;
	se_l3_mcsg_rule_t *r = &mcsg_mask;

	if (6 != sscanf(buf, "intf_idx=%u iport_id=%u dip=%x sip=%x ovid=%u oport_bitmap=%llx", &intf_idx, &iport_id, &dip, &sip, &ovid, &oport_bitmap))
		return -EINVAL;

	r->intf_idx = intf_idx;
	r->iport_id = iport_id;
	r->dip = dip;
	r->sip = sip;
	r->ovid = ovid;
	r->oport_bitmap = oport_bitmap;

	return 0;
}

static int mcsg_mask_get(char *buf, const struct kernel_param *kp)
{
	se_l3_mcsg_rule_print(&mcsg_mask);
	return 0;
}

static const struct kernel_param_ops mcsg_mask_param_ops = {
	.set = mcsg_mask_set,
	.get = mcsg_mask_get,
};

module_param_cb(mcsg_mask, &mcsg_mask_param_ops, NULL, 0600);

static void mcsg_write_data(void *key, void *mask, u32 reqid,  u32 reqaddr)
{
	int i;
	u32 req_id = TCAM_BLK_REQ_ID(TCAM_L3MCSG, reqid);
	int count = sizeof(se_l3_mcsg_rule_t) / TCAM_SLICE_SIZE;

	for (i = 0; i < count; i++, key += TCAM_SLICE_SIZE, mask += TCAM_SLICE_SIZE) {
		g_mcast->cpriv->tcam_access(g_mcast->cpriv, SE_OPT_W, req_id + i,
				reqaddr * 2, key, TCAM_SLICE_SIZE);
		g_mcast->cpriv->tcam_access(g_mcast->cpriv, SE_OPT_W, req_id + i,
				reqaddr * 2 + 1, mask, TCAM_SLICE_SIZE);
	}

}

int se_l3_mcsg_write(se_l3_mcsg_rule_t *rule, tcam_blk_idx_t *idx)
{
	u32 req_addr = idx->item;
	u32 req_id = idx->slice;
	u32 mode = (L3_MCSG_KEY_MODE << 2) + L3_MCSG_TBL_ID;

	if (!is_valid_tcam_idx(idx))
		return -EINVAL;

	mcsg_write_data((u8 *)rule, (u8 *)&mcsg_mask, req_id, req_addr);

	sf_writel(g_mcast->cpriv, SE_TCAM_TB_WRDATA_LO, mode);
	sf_writel(g_mcast->cpriv,SE_TCAM_OPT_ADDR, TCAM_OPT_WR |
				FIELD_PREP(TCAM_OPT_ID, TCAM_BLK_MODE_ID(TCAM_L3MCSG)) |
				FIELD_PREP(TCAM_OPT_REQ_ADDR, req_addr));

	g_mcast->cpriv->se_wait(g_mcast->cpriv, SE_TCAM_OPT_ADDR, TCAM_OPT_BUSY);

	return 0;
}


int se_l3_mcsg_clear(tcam_blk_idx_t *idx)
{
	se_l3_mcsg_rule_t r[2] = { };
	u32 req_addr = idx->item;
	u32 req_id = idx->slice;

		mcsg_write_data((u8 *)&r[0], (u8 *)&r[1], req_id, req_addr);

	return 0;
}

static int se_l3_mcag_write(l3_mcast_entry_t *e)
{
	// TODO: (*, G) mode needs to share tcam block with L3
	return 0;
}

static int se_l2_mcast_dmac_add(l3_mcast_entry_t *e)
{
	struct net_device *iif = dev_get_by_index(&init_net, e->cfg.iif);
	u8 dmac[ETH_ALEN] = { };
	u8 port_id = 0;
	u64 oport_bitmap = 0;
	u16 vlan_id = DPA_UNTAGGED_VID;
	int ret;
	bool l3_en = false;
	int i;

	if (!iif)
		return -ENODEV;

	if ((ret = g_mcast->cpriv->port_id_by_netdev(g_mcast->cpriv, iif, &port_id)))
		return ret;

	ipv4_mcast_ethaddr(cpu_to_le32(e->cfg.dip.ip4.d), dmac);

	oport_bitmap = e->rule.mcsg.oport_bitmap;
	if(is_vlan_dev(iif))
		vlan_id = vlan_dev_priv(iif)->vlan_id;

	for(i = 0; i < 8; i++) {
		if (!strncmp(sf_wan_subnet[i].ifname, iif->name, IFNAMSIZ)) {
			l3_en = true;
			break;
		}
	}

	MCAST_DBG(INFO_LV, "dip: %pI4 dmac: %pM port_id: %u oport_bitmap:%llu vlan:%u \n", &(u32){ htonl(e->cfg.dip.ip4.d) }, dmac, port_id, oport_bitmap, vlan_id);
	// FIXME: hardcoded VID
	ret = g_mcast->cpriv->mac_priv->mac_table_update(g_mcast->cpriv->mac_priv, dmac, true, vlan_id, oport_bitmap, false, l3_en, SA_CML, DA_CML, 0, 0, 0);
	if (ret < 0) {
		MCAST_DBG(ERR_LV, "failed to add l2 entry\n");
		return ret;
	}

	e->l2_idx = ret;

	MCAST_DBG(INFO_LV, "l2 entry index: %d\n", ret);

	return 0;
}

static int se_l2_mcast_dmac_del(l3_mcast_entry_t *e)
{
	struct net_device *iif = dev_get_by_index(&init_net, e->cfg.iif);
	u8 dmac[ETH_ALEN] = { };
	u8 port_id = 0;
	int err;

	if (!iif)
		return -ENODEV;

	if (e->l2_idx < 0)
		return -EINVAL;

	if ((err = g_mcast->cpriv->port_id_by_netdev(g_mcast->cpriv, iif, &port_id)))
		return err;

	ipv4_mcast_ethaddr(cpu_to_le32(e->cfg.dip.ip4.d), dmac);

	g_mcast->cpriv->mac_priv->mac_table_del(g_mcast->cpriv->mac_priv, e->l2_idx);

	return err;
}

static int __se_l3_mcast_add(l3_mcast_entry_t *e)
{
	se_l3_mcast_cfg_t *cfg = &e->cfg;
	int err;

	if ((err = se_l2_mcast_dmac_add(e)))
		return err;

	// FIXME: remove dmac on error
	if (cfg->is_mcsg) {
		MCAST_DBG(INFO_LV, "source ip:0x%x, dst ip:0x%x \n", e->rule.mcsg.sip, e->rule.mcsg.dip);
		return se_l3_mcsg_write(&e->rule.mcsg, &e->tcam_idx);
	} else {
		return se_l3_mcag_write(e);
	}
}

static void l3_mcast_entry_init(l3_mcast_entry_t *e, l3_mcast_entry_key_t *key)
{
	e->tcam_idx.item = -1;
	e->tcam_idx.slice = -1;
	e->l2_idx = -1;

	if (key)
		memcpy(&e->key, key, sizeof(e->key));
}

static int l3_mcast_key_generate(se_l3_mcast_cfg_t *cfg, l3_mcast_entry_key_t *key)
{
	if (cfg->iif == SE_INVALID_IF_IDX)
		return -EINVAL;

	key->iif_idx = cfg->iif;
	key->sip = cfg->sip.ip4.d;
	key->dip = cfg->dip.ip4.d;

	return 0;
}

static void mergeArrays(uint8_t new_oif[SE_MCAST_OIF_MAX], uint8_t old_oif[SE_MCAST_OIF_MAX])
{
	int i,j;
	int isDuplicate = 0;
	int len1 = strlen(new_oif);
	int len2 = strlen(old_oif);

	for (i = 0; i < len2; i++) {
		isDuplicate = 0;

		for (j = 0; j < len1; j++) {
			if (old_oif[i] == new_oif[j]) {
				isDuplicate = 1;
				break;
			}
		}

		if (!isDuplicate) {
			new_oif[len1++] = old_oif[i];
		}
	}

	new_oif[len1] = '\0'; // 添加 null 终止符
}


static void delArrays(uint8_t oif[SE_MCAST_OIF_MAX], uint8_t del_oif[SE_MCAST_OIF_MAX])
{

	int i,j,k;
	int isDuplicate = 0;
	int len1 = strlen(oif);
	int len2 = strlen(del_oif);

	for (i = 0; i < len2; i++) {
		isDuplicate = 0;
		for (j = 0; j < len1; j++) {
			if (del_oif[i] == oif[j]) {
				isDuplicate = 1;
				break;
			}
		}

		MCAST_DBG(INFO_LV, "del oif index is %d\n",j);
		if (isDuplicate){
			for (k = j; k < len1 - 1; k++) {
				oif[k] = oif[k + 1];
			}
			len1--;
			oif[len1] = '\0';
		}
	}
}

static int l3_mcsg_add(se_l3_mcast_cfg_t *cfg)
{
	l3_mcast_entry_key_t key = { };
	l3_mcast_entry_t *e;
	int err;

	if ((err = l3_mcast_key_generate(cfg, &key)))
		return err;

	e = rhashtable_lookup(&l3_mcast_tbl, &key, l3_mcast_tbl_params);
	if (!e) {
		MCAST_DBG(INFO_LV, "new l3 mcsg entry\n");

		e = mcast_kzalloc(sizeof(l3_mcast_entry_t), GFP_KERNEL);
		if (!e) {
			MCAST_DBG(ERR_LV, "failed to allocate memory\n");
			return -ENOMEM;
		}

		l3_mcast_entry_init(e, &key);

		err = rhashtable_insert_fast(&l3_mcast_tbl, &e->node, l3_mcast_tbl_params);
		if (err) {
			MCAST_DBG(ERR_LV, "failed to insert rhashtable\n");
			mcast_kfree(e);

			return err;
		}
	} else {
		if (0 == memcmp(&e->cfg, cfg, sizeof(e->cfg))) {
			MCAST_DBG(DBG_LV, "config is not changed\n");
			return 0;
		}

		MCAST_DBG(INFO_LV, "update existed l3 mcsg entry\n");
		//update iof
		mergeArrays(cfg->oif, e->cfg.oif);
		cfg->oif_cnt = strlen(cfg->oif);
	}

	memcpy(&e->cfg, cfg, sizeof(e->cfg));
	MCAST_DBG(INFO_LV, "tcam item before : %d slice: %d\n", e->tcam_idx.item, e->tcam_idx.slice);

	if (e->tcam_idx.item < 0 || e->tcam_idx.slice < 0) {
		if ((err = se_mcsg_slice_alloc(&se_mcsg_tcam_blk, &e->tcam_idx))) {
			MCAST_DBG(ERR_LV, "MCSG tcam table is full\n");
			goto out_err;
		}
	}

	MCAST_DBG(INFO_LV, "tcam item: %d slice: %d\n", e->tcam_idx.item, e->tcam_idx.slice);

	if ((err = se_l3_mcsg_rule_set(&e->rule.mcsg, &e->cfg)))
		goto out_err;

	if ((err = __se_l3_mcast_add(e)))
		goto out_err;

	return err;

out_err:
	rhashtable_remove_fast(&l3_mcast_tbl, &e->node, l3_mcast_tbl_params);

	se_mcsg_slice_free(&se_mcsg_tcam_blk, &e->tcam_idx);

	mcast_kfree(e);

	return err;
}

static int l3_mcag_add(se_l3_mcast_cfg_t *cfg)
{
	MCAST_DBG(ERR_LV, "mc*g is not supported now\n");

	return -ENOTSUPP;
}

int se_l3_mcast_add(se_l3_mcast_cfg_t *cfg)
{
	if (cfg->is_mcsg) {
		return l3_mcsg_add(cfg);
	} else {
		return l3_mcag_add(cfg);
	}
}

static int l3_mcsg_del(l3_mcast_entry_t *e)
{
	int err;

	if ((err = se_l3_mcsg_clear(&e->tcam_idx)))
		return err;

	if ((err = se_mcsg_slice_free(&se_mcsg_tcam_blk, &e->tcam_idx)))
		return err;

	if ((err = se_l2_mcast_dmac_del(e)))
		return err;

	g_mcast->cpriv->intf_del(g_mcast->cpriv, e->rule.mcsg.intf_idx);

	err = rhashtable_remove_fast(&l3_mcast_tbl, &e->node, l3_mcast_tbl_params);
	if (err) {
		MCAST_DBG(ERR_LV, "failed to remove object from list\n");
		return err;
	}

	return 0;
}

int se_l3_mcast_del(se_l3_mcast_cfg_t *cfg)
{
	l3_mcast_entry_key_t key = { };
	l3_mcast_entry_t *e;
	int err;

	if ((err = l3_mcast_key_generate(cfg, &key)))
		return err;

	e = rhashtable_lookup_fast(&l3_mcast_tbl, &key, l3_mcast_tbl_params);
	if (!e) {
		err = -ENOENT;
		goto out;
	} else {
		if (0 == memcmp(&e->cfg, cfg, sizeof(e->cfg)))
			goto del;

		MCAST_DBG(INFO_LV, " l3 mcsg entry is used, just update\n");

		delArrays(e->cfg.oif, cfg->oif);

		e->cfg.oif_cnt = strlen(e->cfg.oif);

		if (e->cfg.oif_cnt == 0){
			memcpy(&e->cfg, cfg, sizeof(e->cfg));
			goto del;
		}

		MCAST_DBG(INFO_LV, "tcam item: %d slice: %d\n", e->tcam_idx.item, e->tcam_idx.slice);

		if ((err = se_l3_mcsg_rule_set(&e->rule.mcsg, &e->cfg)))
			goto out;

		if ((err = __se_l3_mcast_add(e)))
			goto out;

		return err;
	}

del:
	if ((err = l3_mcsg_del(e))) {
		MCAST_DBG(ERR_LV, "failed to delete entry\n");
		se_l3_entry_print(e);
		goto out;
	}

	mcast_kfree(e);

out:
	return err;
}

int se_l3_mcast_del_marked(char *mark)
{
	struct rhashtable_iter iter = { };
	l3_mcast_entry_t *e;
	int err = -ENOENT;

	if (mark[0] == '\0')
		return -EINVAL;

	rhashtable_walk_enter(&l3_mcast_tbl, &iter);
	rhashtable_walk_start(&iter);
	while ((e = rhashtable_walk_next(&iter)) != NULL) {
		if (IS_ERR(e))
			continue;
		MCAST_DBG(INFO_LV, "rulemark is %s cfg mark is %s \n",e->cfg.mark, mark);

		if (strlen(mark) != strlen(e->cfg.mark))
			continue;

		if (0 != strncmp(e->cfg.mark, mark, strlen(e->cfg.mark)))
			continue;

		if ((err = l3_mcsg_del(e))) {
			MCAST_DBG(ERR_LV, "l3_mcsg_del() failed, err: %d\n", err);
			se_l3_entry_print(e);

			continue;
		}

		mcast_kfree(e);
	}
	rhashtable_walk_stop(&iter);
	rhashtable_walk_exit(&iter);

	return err;
}

/**
 * kvfree @list outside
 */
int se_l3_mcast_list(se_l3_mcast_cfg_t **list, size_t *sz)
{
	size_t list_sz = atomic_read(&l3_mcast_tbl.nelems) * sizeof(se_l3_mcast_cfg_t);
	se_l3_mcast_cfg_t *l;
	struct rhashtable_iter iter = { };
	l3_mcast_entry_t *e;
	size_t i = 0;

	*list = NULL;
	*sz = 0;

	if (list_sz == 0)
		return -ENOENT;

	l = kvmalloc(list_sz, GFP_KERNEL);
	if (!l) {
		MCAST_DBG(ERR_LV, "failed to allocate memory\n");
		return -ENOMEM;
	}

	rhashtable_walk_enter(&l3_mcast_tbl, &iter);
	rhashtable_walk_start(&iter);
	while ((e = rhashtable_walk_next(&iter)) != NULL) {
		if (IS_ERR(e))
			continue;

		memcpy(&l[i++], &e->cfg, sizeof(se_l3_mcast_cfg_t));
	}
	rhashtable_walk_stop(&iter);
	rhashtable_walk_exit(&iter);

	*list = l;
	*sz = list_sz;

	return 0;
}

static int se_mcast_rhashtbl_init(void)
{
	return rhashtable_init(&l3_mcast_tbl, &l3_mcast_tbl_params);
}

static void l3_mcast_tbl_free(void *ptr, void *arg)
{
	(void)arg;

	mcast_kfree(ptr);
}

static int se_mcast_rhashtbl_exit(void)
{
	rhashtable_free_and_destroy(&l3_mcast_tbl, l3_mcast_tbl_free, NULL);

	return 0;
}

static void se_mcsg_blk_id_get(void)
{
	u32 blk_config = sf_readl(g_mcast, SE_TCAM_BLK_CONFIG0);
	ulong mask = TCAM_BLK_CONFIG0_BLK0_CFG;
	u32 i;

	for (i = 0; i < TCAM_BLK_CFG0_BLK_SEL_CNT; i++) {
		ulong sel = (mask & blk_config) >> find_first_bit(&mask, BITS_PER_LONG);

		if (sel == TCAM_L3MCSG) {
			mcsg_blk_id = i;
			break;
		}

		mask <<= TCAM_BLK_CFG0_BLK_SEL_WIDTH;
	}
}

static int se_mcsg_blk_id_init(void)
{
	se_mcsg_blk_id_get();

	if (mcsg_blk_id == TCAM_BLK_CFG0_BLK_SEL_INVALID) {
		MCAST_DBG(ERR_LV, "failed to lookup mcsg blk id\n");
		MCAST_DBG(ERR_LV, "SE_TCAM_BLK_CONFIG0: 0x%08x\n", sf_readl(g_mcast, SE_TCAM_BLK_CONFIG0));

		return -EINVAL;
	}

	MCAST_DBG(INFO_LV, "mcsg tcam blk id: %u\n", mcsg_blk_id);

	return 0;
}

static void se_mcast_init(MCAST_t *priv)
{
	int err;

	// mask init
	memset(&mcsg_mask, 0xff, sizeof(mcsg_mask));
	mcsg_mask.sip = 0;
	mcsg_mask.dip = 0;
	mcsg_mask.ovid = 0;

	if ((err = se_mcsg_blk_id_init()))
		return;

	// nat bypasses mcast pkts
	sf_update(priv, NPU_NAT_PKT_TYPE_IGNORE,
		NAT_IGNORE_MCAST_DNAT | NAT_IGNORE_MCAST_SNAT,
		NAT_IGNORE_MCAST_DNAT | NAT_IGNORE_MCAST_SNAT);

	// disable mcast dmac replacing
	// a HW bug will change mcast dmac incorrectly, disable it
	sf_update(priv, NPU_ARP_MPP_CFG, MC_DMAC_REPLACE_EN, 0);
	// fix issue that not forwarding to CPU port when l3 mcast mismatched
	sf_update(priv, NPU_L3_MPP_CFG, L3_MPP_MC_USE_L2_EN, 0);
	// disable MC*G
	sf_update(priv, NPU_L3_MPP_CFG, L3_MPP_ASM_EN, 0);
	// enable MCSG
	sf_update(priv, NPU_L3_MPP_CFG, 0, L3_MPP_SSM_EN);

	if ((err = se_mcast_rhashtbl_init())) {
		MCAST_DBG(ERR_LV, "failed to init rhashtable\n");
		return;
	}

#ifdef CONFIG_SIFLOWER_DPNS_MCAST_GENL
	if ((err = se_mcast_genl_init())) {
		MCAST_DBG(ERR_LV, "failed to init genl netlink interface\n");
		return;
	}
#endif

	return;
}

int dpns_mcast_probe(struct platform_device *pdev)
{
	COMMON_t * common_priv = platform_get_drvdata(pdev);
	MCAST_t* priv = NULL;

	priv = devm_kzalloc(&pdev->dev, sizeof(MCAST_t), GFP_KERNEL);
	if (priv == NULL)
		return -ENOMEM;

	g_mcast = priv;
	common_priv->mcast_priv = priv;
	priv->cpriv = common_priv;
	priv->iobase = common_priv->iobase;
	se_mcast_init(priv);

	priv->ubus_wq = alloc_workqueue("dpns_mcast_ubus",  WQ_UNBOUND | WQ_SYSFS, 0);
	if (!priv->ubus_wq)
		return -ENOMEM;

	printk("End %s\n", __func__);
	return 0;
}
EXPORT_SYMBOL(dpns_mcast_probe);

void dpns_mcast_remove(struct platform_device *pdev)
{
	COMMON_t * common_priv = platform_get_drvdata(pdev);

	destroy_workqueue(common_priv->mcast_priv->ubus_wq);
	common_priv->mcast_priv = NULL;

#ifdef CONFIG_SIFLOWER_DPNS_MCAST_GENL
	se_mcast_genl_exit();
#endif
	se_mcast_rhashtbl_exit();
	printk("End %s\n", __func__);
}
EXPORT_SYMBOL(dpns_mcast_remove);

MODULE_LICENSE("GPL v2");
MODULE_AUTHOR("0xc0cafe");

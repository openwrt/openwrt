#include <linux/of_platform.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/platform_device.h>
#include <linux/workqueue.h>
#include <net/switchdev.h>
#include <linux/if_bridge.h>
#include <linux/if_vlan.h>
#include <linux/rtnetlink.h>
#include <net/cfg80211.h>
#include <linux/ieee80211.h>

#include "dpns_l2_genl.h"
#include "dpns_mib_proc.h"

MAC_t *g_mac_priv = NULL;

extern struct dpns_nat_subnet_info sf_lan_subnet[8];
extern struct dpns_nat_subnet_info sf_wan_subnet[8];

u16 crc16_custom(const u8 *buf, size_t len, u8 poly_sel)
{
	static const u16 poly[] = {
		0x1021, 0x8005, 0xA097, 0x8BB7, 0xC867, 0x3D65, 0x0589, 0x509D,
	};
	u16 crc = 0, arith = poly[poly_sel];
	int i;

	while (len--) {
		crc  = crc ^ (*buf++ << 8);
		for (i = 0; i < CRC_BIT_COUNT; i++) {
			if (crc & 0x8000)
				crc = (crc << 1) ^ arith;
			else
				crc = crc << 1;
		}
	}

	return crc;
}

bool dpns_is_wan_device (char *name)
{
	int i = 0;

	for (i = 0; i < 8; i ++) {
		if(!strcmp(name, sf_wan_subnet[i].ifname))
			return true;
	}
	return false;
}

int dpns_mac_hw_search(MAC_t *priv, const u8 *dsmac, u16 vid, u32 *result)
{
	COMMON_t * cpriv = priv->cpriv;
	u64 mac;
	int i;
	u32 data;

	spin_lock_bh(&cpriv->hw_lock);
	/* initialize registers to 0 */
	for (i = 0; i < 3; i++) {
		se_write32(priv, SE_MAC_KEY_RAM_DATA(i), 0);
	}

	mac = ether_addr_to_u64(dsmac);
	se_write32(priv, SE_MAC_KEY_RAM_DATA(0), mac);
	data = mac >> 32 | vid << 16;
	se_write32(priv, SE_MAC_KEY_RAM_DATA(1), data);
	se_write32(priv, SE_MAC_LKP_REQ, 0x1);

	cpriv->se_wait(cpriv, SE_MAC_LKP_REQ, BIT(0));

	result[0] = se_read32(priv, SE_MAC_RESULT_RAM_DATA(0));
	result[1] = se_read32(priv, SE_MAC_RESULT_RAM_DATA(1));
	spin_unlock_bh(&cpriv->hw_lock);

	return result[1] & SE_HW_RESULT0_DATA1_HIT;
}

static int sf_search_empty_dmac_index(MAC_t *priv)
{
	int mac_index = 0;

	spin_lock_bh(&priv->bit_lock);
	mac_index = find_first_zero_bit(priv->mac_tbl_bitmap, MAC_SZ);
	set_bit(mac_index, priv->mac_tbl_bitmap);
	spin_unlock_bh(&priv->bit_lock);

	L2_DBG(DBG_LV, "Found available mac_index is %d\n",mac_index);

	return mac_index;
}

void sf_update_mac_tbl_da_cml(MAC_t *priv, u8 da_cml)
{
	int mac_index, last_mac_index;

	if (da_cml == CML_TO_CPU)
		priv->wan_bridge_to_br = true;
	else
		priv->wan_bridge_to_br = false;

	spin_lock_bh(&priv->bit_lock);
	last_mac_index = find_last_bit(priv->mac_tbl_bitmap, L2_MAC_NUM_MAX);

	for (mac_index = 1; mac_index <= last_mac_index; mac_index++) {
		union mac_table_cfg key = {0};
		if (!test_bit(mac_index, priv->mac_tbl_bitmap)) {
			continue;
		}

		priv->cpriv->table_read(priv->cpriv,
						ARP_SE_MAC_TABLE,
						mac_index,
						(u32*)&key,
						sizeof(key));

		if (key.table.valid && fls64(key.table.port_bitmap) - 1 >= 0 &&
							fls64(key.table.port_bitmap) - 1 < DPNS_HOST_PORT) {
				key.table.da_cml = da_cml;
				priv->cpriv->table_write(priv->cpriv,
				ARP_SE_MAC_TABLE,
				mac_index,
				(u32*)&key,
				sizeof(key));
		}
	}
	spin_unlock_bh(&priv->bit_lock);
}

u8 sf_ts_mode(MAC_t *priv, const u8 *mac, u16 vlan_id, int nat_id, u16 soft_key_crc)
{
	struct sf_traffic_statics_info *pos;
	u8 ts_mode = 0;

	spin_lock_bh(&priv->ts_lock);
	if (mac != NULL) {
		hash_for_each_possible(priv->ts_list, pos, snode, soft_key_crc)
		{
			if (ether_addr_equal(pos->ts_info.mac, mac) &&
				pos->ts_info.vid == vlan_id) {
				ts_mode = pos->ts_info.mode;
				break;
			}
		}
	} else {
		hash_for_each_possible(priv->ts_list, pos, snode, soft_key_crc)
		{
			if (pos->ts_info.nat_id == nat_id) {
				ts_mode = pos->ts_info.mode;
				break;
			}
		}
	}
	spin_unlock_bh(&priv->ts_lock);
	return ts_mode;
}

static int sf_crc_del_dmac_index(MAC_t* priv, u16 dsmac_index)
{
	spin_lock_bh(&priv->bit_lock);
	clear_bit(dsmac_index, priv->mac_tbl_bitmap);
	spin_unlock_bh(&priv->bit_lock);

	L2_DBG(DBG_LV, "mac del successfully\n");
	return 0;
}

int sf_del_ts_info(MAC_t *priv, const u8 *mac, u16 vid, int nat_id, u16 soft_key_crc)
{
	struct sf_traffic_statics_info * p;
	struct hlist_node *tmp;

	if (mac != NULL) {
		spin_lock_bh(&priv->ts_lock);
		hash_for_each_possible_safe(priv->ts_list, p, tmp, snode, soft_key_crc)
		{
			if (ether_addr_equal(p->ts_info.mac, mac) &&
					vid == p->ts_info.vid ) {
				hash_del(&p->snode);
				l2_kfree(p);
				spin_unlock_bh(&priv->ts_lock);
				L2_DBG(DBG_LV, "ts del successfully\n");
				return 0;
			}
		}
		spin_unlock_bh(&priv->ts_lock);
	} else {
		spin_lock_bh(&priv->ts_lock);
		hash_for_each_possible_safe(priv->ts_list, p, tmp, snode, soft_key_crc)
		{
			if (p->ts_info.nat_id == nat_id) {
				hash_del(&p->snode);
				l2_kfree(p);
				spin_unlock_bh(&priv->ts_lock);
				L2_DBG(DBG_LV, "ts del successfully\n");
				return 0;
			}
		}
		spin_unlock_bh(&priv->ts_lock);
	}
	return 0;
}


static int sf_del_l2_hash_index(MAC_t *priv, u32 mac_index)
{
	int i, layer;
	u32 layer_offset = 0, item_idx = 0;

	const u8 layer_width[L2_HASH_TABLE_MAX] = {
		10, 9, 9, 8, 8, 7, 7, 7, 6, 6};

	for (layer = 0; layer < 3; layer++) {
		for (i = 0; i < (0x1 << layer_width[layer]); i++) {
			u32 index = 0;

			priv->cpriv->table_read(priv->cpriv, L2_SE_HASH0_TABLE,
						layer_offset + i, &index,
						sizeof(index));
			if (mac_index == index) {
				priv->cpriv->table_write(priv->cpriv,
						L2_SE_HASH0_TABLE,
						layer_offset + i,
						&item_idx, sizeof(index));
				L2_DBG(DBG_LV, "hash del successfully\n");
				return 0;
			}
		}
		layer_offset += (0x1 << (layer_width[layer]));
	}

	layer_offset = 0;
	for (layer = 3; layer < L2_HASH_TABLE_MAX; layer++) {
		for (i = 0; i < (0x1 << layer_width[layer]); i++) {
			u32 index = 0;

			priv->cpriv->table_read(priv->cpriv, L2_SE_HASH1_TABLE,
						layer_offset + i,
						(u32*)&index, sizeof(index));
			if (mac_index == index) {
				priv->cpriv->table_write(priv->cpriv,
						L2_SE_HASH1_TABLE,
						layer_offset + i, &item_idx,
						sizeof(index));
				L2_DBG(DBG_LV, "hash del successfully\n");
				return 0;
			}
		}
		layer_offset += (0x1 << (layer_width[layer]));
	}

	/** TODO :Which one takes less time, calculating during traversal or
	*   looking up in a table?
	* */
	L2_DBG(ERR_LV, "hash del fail\n");
	return -ENOMEM;
}

static void
l2_iso_table_update(MAC_t *priv, u8 iport_num, u32 port_bitmap,
					u32 offload_bitmap)
{
	union l2_iso_table_cfg param = {0};

	param.table.port_isolation_bitmap = port_bitmap;
	param.table.isolation_offload_bitmap = offload_bitmap;
	priv->cpriv->table_write(priv->cpriv, L2_ISO_TABLE, iport_num,
			param.data, sizeof(param));
}

/*
 * Hardware L2 forwarding must not cross Linux bridges and must honour the
 * bridge "isolated" port flag. The NPU looks up destination MACs in one
 * global table, so without restrictions a host on a port of one bridge
 * reaches a host on a port of another bridge directly in hardware.
 *
 * The L2 isolation table, indexed by 32 + ingress port (entries 0..31 apply
 * to lookups requested by the CPU, see dpns_mac_hw_search), holds the egress ports
 * the NPU may forward to. Packets for other ports are sent to the CPU, where
 * the bridge decides. The same check applies to routed (NAT/LF) flows, so
 * the host port and ports outside any bridge are always allowed; flows
 * routed between two different bridges are left to the CPU.
 */
#define L2_ISO_ALL_PORTS	GENMASK(DPNS_MAX_PORT - 1, 0)
#define L2_ISO_PORT_IDX		32
#define L2_ISO_BR_MAX		4

static bool l2_bridge_iso = true;
module_param(l2_bridge_iso, bool, 0644);
MODULE_PARM_DESC(l2_bridge_iso, "Restrict hardware L2 forwarding to ports of the same bridge, honour isolated ports (default on)");

/* bridges a port belongs to, directly or through VLAN upper devices; under RTNL */
static int l2_port_bridges(struct net_device *dev, struct net_device **br)
{
	struct net_device *upper;
	struct list_head *iter;
	int n = 0;

	if (netif_is_bridge_port(dev))
		br[n++] = netdev_master_upper_dev_get(dev);

	rcu_read_lock();
	netdev_for_each_upper_dev_rcu(dev, upper, iter) {
		if (n < L2_ISO_BR_MAX && is_vlan_dev(upper) && netif_is_bridge_port(upper))
			br[n++] = netdev_master_upper_dev_get_rcu(upper);
	}
	rcu_read_unlock();

	return n;
}

static bool l2_share_bridge(struct net_device **a, int na, struct net_device **b, int nb)
{
	int i, j;

	for (i = 0; i < na; i++)
		for (j = 0; j < nb; j++)
			if (a[i] && a[i] == b[j])
				return true;
	return false;
}

static void l2_iso_sync(MAC_t *priv)
{
	static struct net_device *br[DPNS_MAX_PORT][L2_ISO_BR_MAX];
	COMMON_t *cpriv = priv->cpriv;
	int nbr[DPNS_MAX_PORT] = {};
	u32 bridged = 0, allow;
	int p, q;

	ASSERT_RTNL();

	for (p = 0; p < DPNS_MAX_PORT && p < cpriv->port_count; p++) {
		dpns_port_t *dp = cpriv->ports[p];

		if (dp && dp->dev && p != DPNS_HOST_PORT)
			nbr[p] = l2_port_bridges(dp->dev, br[p]);
		if (nbr[p])
			bridged |= BIT(p);
	}

	for (p = 0; p < DPNS_MAX_PORT; p++) {
		if (p == DPNS_HOST_PORT)
			continue;

		allow = L2_ISO_ALL_PORTS;
		if (l2_bridge_iso && nbr[p]) {
			allow &= ~bridged;
			for (q = 0; q < DPNS_MAX_PORT; q++) {
				if (q == p || !nbr[q] || !l2_share_bridge(br[p], nbr[p], br[q], nbr[q]))
					continue;
				if (test_bit(p, priv->iso_ports) && test_bit(q, priv->iso_ports))
					continue;
				allow |= BIT(q);
			}
		}

		/* entries 0..31 must stay open: the driver's own MAC lookups use them */
		l2_iso_table_update(priv, L2_ISO_PORT_IDX + p, allow, L2_ISO_ALL_PORTS);
		L2_DBG(DBG_LV, "iso port %d allow 0x%x\n", p, allow);
	}
}

/* deferred switchdev attributes, called under RTNL */
static int dpns_mac_switchdev_blocking_event(struct notifier_block *nb,
					     unsigned long event, void *ptr)
{
	struct net_device *dev = switchdev_notifier_info_to_dev(ptr);
	struct switchdev_notifier_port_attr_info *info = ptr;
	MAC_t *priv = container_of(nb, MAC_t, switchdev_blocking_nb);
	COMMON_t *cpriv = priv->cpriv;
	const struct switchdev_attr *attr;
	u8 port_id;

	if (event != SWITCHDEV_PORT_ATTR_SET)
		return NOTIFY_DONE;

	attr = info->attr;
	if (attr->id != SWITCHDEV_ATTR_ID_PORT_BRIDGE_FLAGS ||
	    !(attr->u.brport_flags.mask & BR_ISOLATED))
		return NOTIFY_DONE;

	if (!cpriv->port_dev_check(cpriv, dev) || cpriv->port_id_by_netdev(cpriv, dev, &port_id))
		return NOTIFY_DONE;

	if (attr->u.brport_flags.val & BR_ISOLATED)
		set_bit(port_id, priv->iso_ports);
	else
		clear_bit(port_id, priv->iso_ports);

	l2_iso_sync(priv);
	info->handled = true;

	return NOTIFY_OK;
}

static void l2_cleanup_ts_timer(struct timer_list *t)
{
	MAC_t *priv = timer_container_of(priv, t, l2_cleanup_ts_timer);
	unsigned long next_timer = jiffies + priv->mib_time;

	/** read mib count */
	if (priv->dpnsmib_en)
		dpns_mib(priv);

	mod_timer(&priv->l2_cleanup_ts_timer, round_jiffies_up(next_timer));
}

int se_l2_mac_table_del(MAC_t* priv, u32 mac_index)
{
	union mac_table_cfg key = {0};
	int err = 0;

	// write ARP MAC table[item-index], u64*(&table)
	priv->cpriv->table_write(priv->cpriv,
			ARP_SE_MAC_TABLE,
			mac_index,
			(u32*)&key,
			sizeof(key));

	err = sf_crc_del_dmac_index(priv, mac_index);

	return err;
}

#ifdef CONFIG_SIWIFI_EASYMESH
static int notify_link_to_mesh(MAC_t* priv, struct sf_mac_updown *ctx)
{
	struct nlmsghdr *nlh;
	struct genlmsghdr *hdr;
	struct sk_buff *skb;
	int ret = 0;

	if (!priv->nl_sock)
		return 0;

	skb = genlmsg_new(sizeof(struct sf_mac_updown), GFP_KERNEL);
	if (!skb)
		return -ENOMEM;

	nlh = nlmsg_put(skb, 0, 0, 0, sizeof(struct sf_mac_updown) + GENL_HDRLEN, 0);
	if (!nlh) {
		L2_DBG(ERR_LV, "Failed to add message header\n");
		kfree_skb(skb);
		return -ENOMEM;
	}
	hdr = nlmsg_data(nlh);
	hdr->cmd = SF_GENL_EVT_UPDOWN;
	hdr->reserved = 0;
	memcpy(nlmsg_data(nlh) + GENL_HDRLEN, ctx, sizeof(struct sf_mac_updown));

	ret = netlink_broadcast(priv->nl_sock, skb, 0, 1, GFP_KERNEL);
	return 0;
}
#endif

int notify_link_event(MAC_t* priv, u8 port, const u8 *dmac, bool updown, u16 vlan_id, bool notify_easymesh_flag)
{

	struct sf_mac_updown ctx = {0};
	struct xgmac_dma_priv *dma_priv = priv->cpriv->edma_priv;

	if (port == DPNS_HOST_PORT || port >= DPNS_MAX_PORT)
		return 0;
	else if (port >= EXTDEV_OFFSET)
		ctx.is_wifi = true;

	ether_addr_copy(ctx.dsmac, dmac);
	ctx.vlan_id = vlan_id;
	if (dma_priv->ndevs[port]) {
		memcpy(ctx.ifname, dma_priv->ndevs[port]->name, sizeof(ctx.ifname));
		L2_DBG(DBG_LV, "mac:%pM vlan_id:%d port:%d ifname:%s\n", dmac, vlan_id, port, ctx.ifname);
	}
	ctx.port = port;
	ctx.updown = updown;
	ctx.notify_easymesh_flag = notify_easymesh_flag;

	if (updown)
		L2_DBG(DBG_LV, "mac:%pM vlan_id:%d port:%d Link up\n", dmac, vlan_id, port);
	else
		L2_DBG(DBG_LV, "mac:%pM vlan_id:%d port:%d Link down\n", dmac, vlan_id, port);
#ifdef CONFIG_SIWIFI_EASYMESH
	notify_link_to_mesh(priv, &ctx);
#endif
	return sfgenl_event_send(SF_GENL_COMP_L2_MAC,
				SF_GENL_EVT_UPDOWN,
				&ctx, sizeof(struct sf_mac_updown));
}

int vlan_mac_add(MAC_t* priv,
		 u16 vlan_id,
		 const u8 *dsmac,
		 u64 port_map,
		 u8 valid,
		 u8 age_en,
		 u8 l3_en,
		 u8 sa_cml,
		 u8 da_cml,
		 u8 vlan_en,
		 u16 sta_id,
		 u16 repeater_id)
{
	union mac_table_cfg key;
	l2_hash_key_t hashkey;
	const u8 poly[L2_HASH_TABLE_MAX] = {0, 1, 0, 1, 0, 1, 0, 1, 0, 1};
	const u8 layer_width[L2_HASH_TABLE_MAX] = {
		10, 9, 9, 8, 8, 7, 7, 7, 6, 6};
	u64 port_bitmap;
	u32 mib_index = 0, spl_index = 0, l2_spl_mode = 0, l2_mib_mode = 0;
	u32 item_idx = 1, hash_item_idx = 1;
	int layer, i;
	u32 result_data[2];
	u16 layer_offset = 0, soft_key_crc = 0, iram, crc, req_addr;
	bool hit, notify_easymesh_flag = false;

	if (!valid) {
		L2_DBG(WARN_LV, "now the mac :%pM is invalid!\n", dsmac);
		return -EPERM;
	}

	BUILD_BUG_ON(sizeof(key.table) != 20);
	BUILD_BUG_ON(sizeof(hashkey) != 6);

	hit = dpns_mac_hw_search(priv, dsmac, vlan_id, result_data);

	port_bitmap = FIELD_GET(SE_HW_RESULT0_DATA1_BITMAP_19_26,
			result_data[1]) <<19 |
			FIELD_GET(SE_HW_RESULT0_DATA0_BITMAP_0_18,
			result_data[0]);

	item_idx = FIELD_GET(SE_HW_RESULT0_DATA1_MAC_IDX,
			result_data[1]);

	if ( hit && (port_bitmap == port_map || port_bitmap == BIT(DPNS_HOST_PORT)) ) {
			L2_DBG(DBG_LV, "the mac is already in mac table\n");
			return item_idx;
	}

	if (hit && (port_bitmap != port_map)) {
		sf_mac_del_entry(priv, dsmac, vlan_id, false, false);

		spin_lock_bh(&priv->bit_lock);
		set_bit(item_idx, priv->mac_tbl_bitmap);
		spin_unlock_bh(&priv->bit_lock);
		L2_DBG(DBG_LV, "\tupdate the mac and use the old index\n");
	} else {
		/* new item index */
		L2_DBG(DBG_LV, "\tnew mac: %pM vlan_id: %d\n", dsmac, vlan_id);
		item_idx = sf_search_empty_dmac_index(priv);
	}

	if (!(item_idx && item_idx < L2_MAC_NUM_MAX)) {
		L2_DBG(WARN_LV, "l2 mac table full!\n");
		return -ENOMEM;
	}

	/** add the mac index into the hash table */
	ether_addr_copy(hashkey.mac, dsmac);

	for (i = 0; i < sizeof(hashkey); i++)
		L2_DBG(DBG_LV, "hashkey: %02x", ((u8 *)&hashkey)[i]);

	/** hash search each layer for token */
	for (layer = 0; layer < L2_HASH_TABLE_MAX; layer++) {
		iram = (layer <= 2 ? L2_SE_HASH0_TABLE : L2_SE_HASH1_TABLE);
		crc = crc16_custom((u8*)&hashkey, sizeof(hashkey),
				poly[layer]);
		req_addr = crc & (0xFFFF >> (16 - layer_width[layer]));

		if (layer == 0)
			soft_key_crc = crc;

		req_addr += layer_offset;

		/** layer offset for next round */
		if (layer == HASH1_TABLE_BEGIN) {
			layer_offset = 0;
		} else {
			layer_offset += (0x1 << (layer_width[layer]));
		}

		priv->cpriv->table_read(priv->cpriv, iram, req_addr, &hash_item_idx,
								sizeof(u32));
		if (hash_item_idx > 0 && hash_item_idx < L2_MAC_NUM_MAX)
			continue;
		else if (hash_item_idx == 0)
			break;
	}

	if (layer == L2_HASH_TABLE_MAX) {
		L2_DBG(ERR_LV, "l2 hash table full!\n");
		return -ENOMEM;
	}

	priv->cpriv->table_write(priv->cpriv, iram, req_addr,
			&item_idx,sizeof(u32));

	/** write the mac table */
	memset(&key, 0, sizeof(key));

	if (port_map)
		key.table.port_bitmap = port_map;

	if (vlan_id)
		key.table.vid = vlan_id;

	spin_lock_bh(&priv->mac_lock);
	if (priv->wan_bridge_to_br && fls64(port_map) - 1 >= 0 && fls64(port_map) - 1 < DPNS_HOST_PORT)
		da_cml = CML_TO_CPU;
	spin_unlock_bh(&priv->mac_lock);

	key.table.valid			= 1;
	key.table.age_en		= age_en;
	key.table.mib_en		= l2_mib_mode;
	key.table.mib_id		= mib_index;
	key.table.spl_en		= l2_spl_mode;
	key.table.spl_id		= spl_index;
	key.table.l3_en			= l3_en;
	key.table.da_cml		= da_cml;
	key.table.sa_cml		= sa_cml;
	key.table.sta_id		= sta_id;
	key.table.repeater_id		= repeater_id;
	key.table.mac			= ether_addr_to_u64(dsmac);
	key.table.vlan_offload_en	= vlan_en;


	L2_DBG(DBG_LV, "arp mac item %d\n", item_idx);
	// write ARP MAC table[item-index], u64*(&table)
	priv->cpriv->table_write(priv->cpriv,
			 ARP_SE_MAC_TABLE,
			 item_idx,
			 (u32*)&key,
			 sizeof(key));

	spin_lock_bh(&priv->dev_num_lock);
	priv->dev_num[fls64(port_map) - 1]++;
	if (priv->dev_num[fls64(port_map) - 1] == 1 && fls64(port_map) - 1 != 4)
		notify_easymesh_flag = true;
	spin_unlock_bh(&priv->dev_num_lock);

	notify_link_event(priv, fls64(port_map) - 1, dsmac, true, vlan_id, notify_easymesh_flag);

	return item_idx;
}

int se_l2_mac_table_update(MAC_t* priv,
		const u8 *dsmac,
		u8 valid,
		u16 vlan_id,
		u64 port_map,
		u8 age_en,
		u8 l3_en,
		u8 sa_cml,
		u8 da_cml,
		u8 vlan_en,
		u16 sta_id,
		u16 repeater_id)
{
	u32 tmp_idx;	// max 2048
	int err;

	if (is_zero_ether_addr(dsmac))
		return 0;

	if (!priv->l2_learning_en)
		return 0;


	err = vlan_mac_add(priv, vlan_id, dsmac, port_map,
			   valid, age_en, l3_en, sa_cml, da_cml,
			   vlan_en, sta_id, repeater_id);

	L2_DBG(DBG_LV, "add_vlan_id:%d, add_dsmac:%pM\n", vlan_id, dsmac);

	if (err < 0) {
		L2_DBG(ERR_LV, "vlan_mac_add fail\n");
		return err;
	}
	tmp_idx = err;

	return tmp_idx;
}

static void sf_destroy_maclist(MAC_t *priv)
{
	spin_lock_bh(&priv->bit_lock);
	memset(&priv->mac_tbl_bitmap, 0, sizeof(priv->mac_tbl_bitmap));
	priv->mac_tbl_bitmap[0] = 1;
	spin_unlock_bh(&priv->bit_lock);

	//clear hw mac/hash entry
	spin_lock_bh(&priv->mac_lock);
	se_clean_set(priv, CLR_CTRL_RAM_ADDR, 0, 0x1e0);
	spin_unlock_bh(&priv->mac_lock);
}

void sf_destroy_tslist(MAC_t *priv)
{
	struct sf_traffic_statics_info *pos;
	struct hlist_node *tmp;
	int bkt;

	spin_lock_bh(&priv->ts_lock);
	hash_for_each_safe(priv->ts_list, bkt, tmp, pos, snode)
	{
		hash_del(&pos->snode);
		l2_kfree(pos);
	}
	spin_unlock_bh(&priv->ts_lock);
}

void sf_mac_clear(MAC_t *priv)
{
	//clear ts list
	sf_destroy_tslist(priv);
	//clear mac list
	sf_destroy_maclist(priv);
}

int vlan_mac_del(MAC_t* mac_priv, const u8 *dmac, u16 vlan_id, u8 port)
{
	l2_hash_key_t hashkey;
	int err;
	u32 result_data[2];
	u16 dmac_index;
	u16 crc_16;
	u8 ts_mode;

	dpns_mac_hw_search(mac_priv, dmac, vlan_id, result_data);

	dmac_index = FIELD_GET(SE_HW_RESULT0_DATA1_MAC_IDX,
			result_data[1]);
	//TODO:del wan mac
	/* manually calculate the value of crc */
	ether_addr_copy(hashkey.mac, dmac);
	crc_16 = crc16_custom((u8*)&hashkey, sizeof(hashkey), 0);
	ts_mode = sf_ts_mode(mac_priv, dmac, vlan_id, 0, crc_16);
	if (ts_mode)
		sf_del_ts_info(mac_priv, dmac, vlan_id, 0, crc_16);

	err = se_l2_mac_table_del(mac_priv, dmac_index);
	if (err)
		return err;


	err = sf_del_l2_hash_index(mac_priv, dmac_index);
	if (err)
		return err;

	return 0;
}

int sf_mac_del_entry(MAC_t* priv, const u8 *dmac, u16 vlan_id, bool is_deled_by_event, bool is_netdev_event)
{
	int err, hit;
	u64 port_bitmap;
	u32 result_data[2];
	int mac_index;
	bool updown = false, notify_easymesh_flag = false;

	struct xgmac_dma_priv *dma_priv = priv->cpriv->edma_priv;
	struct net_device *dev;
	struct switchdev_notifier_fdb_info fdb_info;
	struct dpns_port_vlan_info *pos;
	union mac_table_cfg key = {0};
	dpns_port_t *dp_port;

	if (is_zero_ether_addr(dmac))
		return 0;

	hit = dpns_mac_hw_search(priv, dmac, vlan_id, result_data);
	if (!hit)
		return 0;

	port_bitmap = FIELD_GET(SE_HW_RESULT0_DATA1_BITMAP_19_26
				, result_data[1]) <<19 |
			FIELD_GET(SE_HW_RESULT0_DATA0_BITMAP_0_18,
				result_data[0]);

	/* fls64(0) - 1 would index ndevs[] out of bounds */
	if (!port_bitmap || fls64(port_bitmap) > DPNS_MAX_PORT)
		return 0;

	dev = dma_priv->ndevs[fls64(port_bitmap) - 1];
	fdb_info.addr = dmac;
	fdb_info.vid = 0;

	if (!dev && !is_netdev_event && (port_bitmap == BIT(DPNS_HOST_PORT))) {
		L2_DBG(DBG_LV, "this is the dev's mac\n");
		return 0;
	}

	// the gateway's mac and wifi's mac can't del by user.
	if (!is_deled_by_event) {
		dp_port = priv->cpriv->port_by_netdev(priv->cpriv, dev);
		L2_DBG(DBG_LV, "now the dp_port->ref_count is %d\n", dp_port->ref_count);

		if (dp_port->ref_count > 1) {
			spin_lock_bh(&dp_port->lock);
			list_for_each_entry(pos, &dp_port->vlan_list, node) {
				if (pos->vlan_id == vlan_id) {
					dev = pos->dev;
				}
			}
			spin_unlock_bh(&dp_port->lock);
		}

		if (dpns_is_wan_device(dev->name)) {
			L2_DBG(WARN_LV, "this is the gateway's mac, we can't del by user\n");
			return 0;
		}

		if ((fls64(port_bitmap) - 1) >= EXTDEV_OFFSET) {
			mac_index = FIELD_GET(SE_HW_RESULT0_DATA1_MAC_IDX,
				result_data[1]);

			priv->cpriv->table_read(priv->cpriv,
				ARP_SE_MAC_TABLE,
				mac_index,
				(u32*)&key,
				sizeof(key));

			if (key.table.repeater_id || key.table.sta_id) {
				L2_DBG(WARN_LV, "this is the wifi's mac, we can't del by user\n");
				return 0;
			}
		}
	}

	err = vlan_mac_del(priv, dmac, vlan_id, fls64(port_bitmap) - 1);
	if (err < 0) {
		L2_DBG(ERR_LV, "vlan_mac_del fail\n");
		return err;
	}

	L2_DBG(DBG_LV, "is_deled_by_event now is %d\n", is_deled_by_event);

	if (!is_deled_by_event) {
		if (vlan_id == 0) {
			err = call_switchdev_notifiers(SWITCHDEV_FDB_DEL_TO_BRIDGE,
					dev, &fdb_info.info,
					NULL);
			if (!err) {
				L2_DBG(DBG_LV, "fdb del successfully\n");
			} else {
				L2_DBG(DBG_LV, "fdb del failed, err = %d\n", err);
			}
		} else {
			spin_lock_bh(&dp_port->lock);
			list_for_each_entry(pos, &dp_port->vlan_list, node) {
				if (pos->vlan_id == vlan_id) {
					err = call_switchdev_notifiers(SWITCHDEV_FDB_DEL_TO_BRIDGE,
							pos->dev, &fdb_info.info,
							NULL);
					if (!err) {
						L2_DBG(DBG_LV, "fdb del successfully\n");
					} else {
						L2_DBG(DBG_LV, "fdb del failed, err = %d\n", err);
					}
					break;
				}
			}
			spin_unlock_bh(&dp_port->lock);
		}
	}

	spin_lock_bh(&priv->dev_num_lock);
	priv->dev_num[fls64(port_bitmap) - 1]--;
	if (priv->dev_num[fls64(port_bitmap) - 1] == 0 && fls64(port_bitmap) - 1 != 4)
		notify_easymesh_flag = true;
	spin_unlock_bh(&priv->dev_num_lock);

	notify_link_event(priv, fls64(port_bitmap) - 1, dmac, updown, vlan_id, notify_easymesh_flag);

	return 0;
}

static int l2_age_del(MAC_t* priv)
{
	union mac_table_cfg key = {};
	u8 mac[ETH_ALEN];
	int cur_depth, cur_width, mac_index, age_mac_index = 0;

	u16 vlan_id;
	/* SE_MAC_AGE_RAM 0x11198000~0x111980ff 256B */

	for (mac_index = 1; mac_index < MAC_SZ; mac_index++) {

		spin_lock_bh(&priv->bit_lock);
		if (!test_bit(mac_index, priv->mac_tbl_bitmap)) {
			spin_unlock_bh(&priv->bit_lock);
			continue;
		}
		spin_unlock_bh(&priv->bit_lock);

		cur_depth = mac_index / L2_AGE_REG_WIDTH;
		cur_width = mac_index % L2_AGE_REG_WIDTH;

		priv->cpriv->table_read(priv->cpriv, ARP_SE_MAC_TABLE, cur_depth*L2_AGE_REG_WIDTH + cur_width,
					(u32*)&key, sizeof(key));

		if (!key.table.valid)
			continue;
		if (key.table.age_en == 0)
			continue;

		u64_to_ether_addr(key.table.mac, mac);
		vlan_id = key.table.vid;

		age_mac_index = se_read32(priv,
			SE_MAC_AGE_RAM + SE_MAC_AGE_REG_OFFSET_RAM *
			cur_depth) >> cur_width & 0x1;

		if (age_mac_index) {
			L2_DBG(DBG_LV,"need age mac_index is %d, depth:%d width:%d\n",
					cur_depth*L2_AGE_REG_WIDTH + cur_width, cur_depth,
					cur_width);

			/* Write 0 into the register */
			se_clean_set(priv, SE_MAC_AGE_RAM +
				SE_MAC_AGE_REG_OFFSET_RAM *
				cur_depth, 1 << cur_width, 0);
		} else {
			L2_DBG(DBG_LV, "need del age_mac_index is %d, depth:%d width:%d\n",
					cur_depth * L2_AGE_REG_WIDTH + cur_width,
					cur_depth, cur_width);
			L2_DBG(DBG_LV,"this is the l2 age's mac table del\n");
			sf_mac_del_entry(priv, mac, vlan_id, false, false);
		}
	}

	return 0;
}

static void dpns_l2_fdb_cleanup(struct timer_list *t)
{
	MAC_t *priv = timer_container_of(priv, t, l2_cleanup_age_timer);
	unsigned long next_age_timer;
	priv->ageing_time = priv->age_update_time;

	/* for each NPU arp learn items */
	if (priv->l2_age_en)
		l2_age_del(priv);
	next_age_timer = jiffies + priv->ageing_time;

	mod_timer(&priv->l2_cleanup_age_timer, round_jiffies_up(next_age_timer));
}

static void dpns_fdb_offload_notify(MAC_t *priv,
			  struct switchdev_notifier_fdb_info *recv_info,
			  struct net_device *dev)
{
	struct switchdev_notifier_fdb_info info;

	info.addr = recv_info->addr;
	info.vid = recv_info->vid;
	info.offloaded = true;
	call_switchdev_notifiers(SWITCHDEV_FDB_OFFLOADED, dev, &info.info,
			NULL);
}

struct dpns_switchdev_event_work {
	struct work_struct work;
	struct switchdev_notifier_fdb_info fdb_info;
	struct net_device *dev;
	MAC_t	*priv;
	u8 port_id;
	u8 l3_en;
	u16 ovid;
	unsigned long event;
};

static const char *swev_to_names[] = {
	"undefined",		// 0
	"fdb-add-br",		// SWITCHDEV_FDB_ADD_TO_BRIDGE = 1,
	"fdb-del-br",		// SWITCHDEV_FDB_DEL_TO_BRIDGE,
	"fdb-add-dev",		// SWITCHDEV_FDB_ADD_TO_DEVICE,
	"fdb-del-dev",		// SWITCHDEV_FDB_DEL_TO_DEVICE,
	"fdb-offload",		// SWITCHDEV_FDB_OFFLOADED,
	"fdb-flush-br",		// SWITCHDEV_FDB_FLUSH_TO_BRIDGE,
};

static void dpns_switchdev_event_work_fn(struct work_struct *work)
{
	struct dpns_switchdev_event_work *switchdev_work =
		container_of(work, struct dpns_switchdev_event_work, work);
	struct switchdev_notifier_fdb_info *fdb_info =
			&switchdev_work->fdb_info;
	MAC_t *priv = switchdev_work->priv;
	VLAN_t *vlan_priv = priv->cpriv->vlan_priv;
	struct vlan_vport_entry *pos_vport;
	u16 vlan_id = switchdev_work->ovid ?: DPA_UNTAGGED_VID;
	const u8 *mac = fdb_info->addr;
	u8 l3_enable = switchdev_work->l3_en;
	u8 sa_cml = CML_FORWARD;
	u8 da_cml = CML_FORWARD;
	u8 valid = 1;
	u8 port_id = switchdev_work->port_id;
	int err;

	spin_lock_bh(&vlan_priv->vport_lock);
	list_for_each_entry(pos_vport, &vlan_priv->vport_list, node) {
		if (pos_vport->vlan_id == vlan_id && pos_vport->port == port_id) {
			port_id = pos_vport->vport;
			break;
		}
	}
	spin_unlock_bh(&vlan_priv->vport_lock);

	L2_DBG(DBG_LV, "%pM vid %d port_id: %d sa %d,da %d %s\n", mac, vlan_id,
			port_id, sa_cml, da_cml,
			swev_to_names[switchdev_work->event]);

	rtnl_lock();
	switch (switchdev_work->event) {
	case SWITCHDEV_FDB_ADD_TO_DEVICE:
		L2_DBG(DBG_LV,"this is SWITCHDEV_FDB_ADD_TO_DEVICE's mac table update\n");
		err = se_l2_mac_table_update(priv,
				mac,
				valid,
				vlan_id,
				BIT(port_id),
				true,
				l3_enable,
				sa_cml,
				da_cml,
				0,
				0,
				0);

		if (err < 0) {
			L2_DBG(DBG_LV, "fdb add failed err=%d\n", err);
			break;
		}
		dpns_fdb_offload_notify(priv, fdb_info, switchdev_work->dev);
		call_switchdev_notifiers(SWITCHDEV_FDB_ADD_TO_BRIDGE,
				switchdev_work->dev, &fdb_info->info,
				NULL);
		break;
	case SWITCHDEV_FDB_DEL_TO_DEVICE:
		L2_DBG(DBG_LV,"this is SWITCHDEV_FDB_DEL_TO_DEVICE's mac table del\n");
		err = sf_mac_del_entry(priv, mac, vlan_id, true, false);
		if (err < 0 ) {
			L2_DBG(DBG_LV, "fdb del failed err=%d\n", err);
		} else {
			call_switchdev_notifiers(SWITCHDEV_FDB_DEL_TO_BRIDGE,
						switchdev_work->dev, &fdb_info->info,
						NULL);
			L2_DBG(DBG_LV, "fdb del successfully!\n");
		}
		break;
	}

	rtnl_unlock();

	l2_kfree(fdb_info->addr);
	dev_put(switchdev_work->dev);
	l2_kfree(switchdev_work);
	atomic_dec(&priv->work_cnt);
}

/* called under rcu_read_lock() */
static int dpns_mac_switchdev_event(struct notifier_block *unused,
				  unsigned long event, void *ptr)
{
	struct net_device *dev = switchdev_notifier_info_to_dev(ptr);
	struct dpns_switchdev_event_work *switchdev_work;
	struct switchdev_notifier_fdb_info *fdb_info = ptr;
	MAC_t *priv = container_of(unused, MAC_t, switchdev_notifier);
	COMMON_t *cpriv = priv->cpriv;
	bool learning = !fdb_info->added_by_user && !fdb_info->is_local;
	u8 port_id;
	u8 l3_en = 0;

	L2_DBG(DBG_LV, "swev notify %lu\n", event);

	if (!learning)
		return NOTIFY_DONE;

	if (!cpriv->port_dev_check(cpriv, dev))
		return NOTIFY_DONE;

	if (cpriv->port_id_by_netdev(cpriv, dev, &port_id))
		return NOTIFY_DONE;

	if (port_id >= EXTDEV_OFFSET) {
		if (!(dev->ieee80211_ptr && (dev->ieee80211_ptr->iftype == NL80211_IFTYPE_STATION)))
			return NOTIFY_DONE;

		if (cpriv->router_priv->rep_dev)
			l3_en = 1;
	}

	if (atomic_read(&priv->work_cnt) > L2_MAC_NUM_MAX) {
		L2_DBG(DBG_LV, "dpns_l2 swithdev work full\n");
		return NOTIFY_DONE;
	}

	switch (event) {
	case SWITCHDEV_FDB_ADD_TO_DEVICE:
	case SWITCHDEV_FDB_DEL_TO_DEVICE:
		switchdev_work = l2_kzalloc(sizeof(*switchdev_work), GFP_ATOMIC);
		if (WARN_ON(!switchdev_work))
			return NOTIFY_BAD;

		INIT_WORK(&switchdev_work->work, dpns_switchdev_event_work_fn);

		if (is_vlan_dev(dev)) {
			switchdev_work->ovid = vlan_dev_priv(dev)->vlan_id;
		} else {
			switchdev_work->ovid = fdb_info->vid;
		}

		switchdev_work->priv = priv;
		switchdev_work->event = event;
		switchdev_work->dev = dev;
		switchdev_work->port_id = port_id;
		switchdev_work->l3_en = l3_en;

		memcpy(&switchdev_work->fdb_info, ptr,
			sizeof(switchdev_work->fdb_info));
		switchdev_work->fdb_info.addr = l2_kzalloc(ETH_ALEN,
				GFP_ATOMIC);
		if (unlikely(!switchdev_work->fdb_info.addr)) {
			l2_kfree(switchdev_work);
			return NOTIFY_BAD;
		}

		ether_addr_copy((u8 *)switchdev_work->fdb_info.addr,
				fdb_info->addr);
		/* Take a reference on the dpns device */
		dev_hold(dev);
		break;
	default:
		return NOTIFY_DONE;
	}

	atomic_inc(&priv->work_cnt);
	queue_work(priv->owq, &switchdev_work->work);
	return NOTIFY_DONE;
}

int se_wifi_set_mac_entry(struct net_device *dev, const u8 *mac,
			bool is_intf, u16 sta_id, u16 repeater_id, bool is_add)
{
	u8 port_id, age_en = 0, l3_en = 0, vlan_en = 0;
	u16 vlan_id = 0;
	int err = 0;
	COMMON_t *cpriv;

	if (!g_mac_priv)
		return err;

	if (is_vlan_dev(dev))
		vlan_id = vlan_dev_priv(dev)->vlan_id;

	if (!is_add) {
		L2_DBG(DBG_LV,"this is the set wifi's mac table del\n");
		err = sf_mac_del_entry(g_mac_priv, mac, vlan_id ?:
					DPA_UNTAGGED_VID, true, false);
		return err;
	}

	cpriv = g_mac_priv->cpriv;

	if (is_intf) {
		port_id = DPNS_HOST_PORT;
	} else {
		if(cpriv->port_id_by_netdev(cpriv, dev, &port_id))
			return NOTIFY_DONE;
	}

	if (cpriv->router_priv->rep_dev)
		l3_en = 1;

	L2_DBG(DBG_LV,"%s %pM vid %d sta_id %d repeater_id %d\n",\
					__func__, mac, vlan_id, sta_id,
					repeater_id);
	L2_DBG(DBG_LV,"this is the set wifi's mac table update\n");

	err = se_l2_mac_table_update(g_mac_priv, mac, 1, vlan_id, BIT(port_id),
				age_en, l3_en, CML_FORWARD, CML_FORWARD,
				vlan_en, sta_id, repeater_id);
	return err;

}
EXPORT_SYMBOL(se_wifi_set_mac_entry);

int se_l2_set_term_mac(MAC_t *priv, const u8 *mac, u16 vlan_id, bool is_add, bool is_netdev_down)
{
	int err = 0;
	u16 port_num = DPNS_HOST_PORT;
	/** TO CPU Port, not 'flow->term_mac.in_pport'; */
	u8 valid = 1;
	u8 l3_enable = true;
	u8 sa_cml = CML_FORWARD;
	u8 da_cml = CML_FORWARD;

	if (!is_add) {
		L2_DBG(DBG_LV,"this is the set term's mac table del\n");
		err = sf_mac_del_entry(priv, mac, vlan_id ?: DPA_UNTAGGED_VID, true, is_netdev_down);
		return err;
	}
	L2_DBG(DBG_LV,"%s inp %d, %pM vid %d l3_en %d sa %d,da %d\n",\
			 __func__, port_num, mac, vlan_id, l3_enable, sa_cml,
			 da_cml);
	L2_DBG(DBG_LV,"this is the set term's mac table update\n");
	err = se_l2_mac_table_update(priv,
				 mac,
				 valid,
				 vlan_id,
				 BIT(port_num),
				 0,
				 l3_enable,
				 sa_cml,
				 da_cml,
				 0,
				 0,
				 0);

	return err;
}

/*
 * The MAC table is keyed by the MAC address only, so all NPU devices with the
 * same address (the ports of a bridge, a port and its VLAN devices) share one
 * router MAC entry. Keep it while another of them is still up; removing it made
 * the NPU copy every packet for the router to the CPU.
 */
static bool dpns_term_mac_in_use(MAC_t *priv, struct net_device *dev)
{
	struct net_device *other;

	ASSERT_RTNL();
	for_each_netdev(dev_net(dev), other) {
		if (other == dev || !netif_running(other) ||
		    !ether_addr_equal(other->dev_addr, dev->dev_addr))
			continue;
		if (priv->cpriv->port_dev_check(priv->cpriv, other))
			return true;
	}

	return false;
}

static int dpns_mac_netdevice_event(struct notifier_block *unused,
				  unsigned long event, void *ptr)
{
	struct net_device *dev = netdev_notifier_info_to_dev(ptr);
	struct netdev_notifier_changeupper_info *info;
	MAC_t *priv = container_of(unused, MAC_t, netdevice_nb);
	int err = 0, vlan_id = 0, i;
	COMMON_t *cpriv = priv->cpriv;
	u8 port_id;
	dpns_port_t *dp_port;

	if (!priv->cpriv->port_dev_check(priv->cpriv, dev))
		return NOTIFY_DONE;

	if (is_vlan_dev(dev))
		vlan_id = vlan_dev_vlan_id(dev);

	if (cpriv->port_id_by_netdev(cpriv, dev, &port_id))
		return NOTIFY_DONE;

	/* bridge membership changed: update hardware L2 forwarding domains */
	if (event == NETDEV_CHANGEUPPER || event == NETDEV_UNREGISTER) {
		info = ptr;
		if (event == NETDEV_UNREGISTER ||
		    (netif_is_bridge_master(info->upper_dev) && !info->linking))
			clear_bit(port_id, priv->iso_ports);
		l2_iso_sync(priv);
	}

	if (port_id >= EXTDEV_OFFSET) {
		if (!(dev->ieee80211_ptr && (dev->ieee80211_ptr->iftype == NL80211_IFTYPE_STATION)))
			return NOTIFY_DONE;

		spin_lock_bh(&priv->dev_num_lock);
		priv->dev_num[port_id]++;
		spin_unlock_bh(&priv->dev_num_lock);
	}


	L2_DBG(DBG_LV, "%s dev:%s mac:%pM vid:%u\n",
			netdev_cmd_to_name(event), dev->name,
			dev->dev_addr, vlan_id);
	dp_port = priv->cpriv->port_by_netdev(priv->cpriv, dev);

	switch (event) {
	case NETDEV_UP:
		err = se_l2_set_term_mac(priv, dev->dev_addr, vlan_id,
				true, false);
		break;
	case NETDEV_DOWN:
		if (!vlan_id)
			clear_bit(dp_port->port_id, priv->dev_port_bitmap);

		if (dpns_term_mac_in_use(priv, dev))
			break;
		err = se_l2_set_term_mac(priv, dev->dev_addr, vlan_id,
				false, true);
		break;
	case NETDEV_PRE_CHANGEADDR:
		/** pre address change, del from se */
		if (vlan_id) {
			if (test_and_set_bit(dp_port->port_id, priv->dev_port_bitmap))
				break;
		}

		if (dpns_term_mac_in_use(priv, dev))
			break;
		err = se_l2_set_term_mac(priv, dev->dev_addr, vlan_id,
				false, false);
		break;
	case NETDEV_CHANGEADDR:
		/** post address change, apply new mac from dev to se */
		err = se_l2_set_term_mac(priv, dev->dev_addr, vlan_id, true, false);
		set_bit(dp_port->port_id, priv->dev_port_bitmap);
		break;
	case NETDEV_CHANGEUPPER:
		info = ptr;
		if (!netif_is_bridge_port(dev))
			break;
		for (i = 0; i < 8; i++) {
			if (!strncmp(sf_wan_subnet[i].ifname, dev->name, IFNAMSIZ)) {
				if (info->linking) {
					sf_update_mac_tbl_da_cml(priv, CML_TO_CPU);
					break;
				} else {
					sf_update_mac_tbl_da_cml(priv, CML_FORWARD);
					break;

				}
			}
		}
		break;
	default:
		break;
	}

	if (err < 0)
		L2_DBG(ERR_LV, "%s event %s error %d\n", __func__,
				netdev_cmd_to_name(event), err);


	return NOTIFY_DONE;
}

static int mac_init(MAC_t *priv)
{
	u64 ret = 0;
	int i;

	/** enable l2 mac table match */
	se_clean_set(priv, CONFIG2_RGT_ADDR, 0,
			FIELD_PREP(CONFIG2_MAC_TAB_EN, 1));

	/* enable 802X unauth force up */
	se_clean_set(priv, CONFIG_L2_MPP_CFG2_ADDR, 0,
			CONFIG_BCAST_DA_SRCH | CONFIG_X_UNAUTH_FROCE_UP);

	/** disenable mac age table read_clear*/
	se_clean_set(priv, CONFIG2_RGT_ADDR,
			FIELD_PREP(CONFIG2_MAC_AGE_EN, 1), 0);

	/** The maximum sub table used by the configuration is 9*/
	se_clean_set(priv, CONFIG2_RGT_ADDR, 0,
			FIELD_PREP(CONFIG2_MAC_SEG_NUM, 0x9));

	/**
	* NOTE: keep sync witch CONFIG1 register;
	* default: POLY use 0-1 ping pong.
	*      b01 000 001 000 001 000 001 000 001 000 = 0x8208208
	*/
	se_write32(priv, CONFIG1_RGT_ADDR, 0x48208208);

	/**  enable iport spl ,l2 mac spl mode 0/1
	 * 0: The message with insufficient remaining quota will be discarded,
	 *    otherwise it will be sent
	 * 1: The message whose remaining quota is not 0 is sent, otherwise it
	 *    will be discarded
	 **/
	se_clean_set(priv, CONFIG2_RGT_ADDR, 0,
			FIELD_PREP(CONFIG2_MACSPL_MODE_EN, 1));

	/** l2 mac spl table timer
	 *	defalut: 600MHz 100ms reg value is 0x39386ff
	 *	*/
	ret = clk_get_rate(priv->cpriv->clk);	//get NPU clock Hz
	se_write32(priv, SE_SPL_CONFIG0_RGT_REG_ADDR, ret-1);

	for (i = 0; i < L2_ISO_NUM_MAX; i++) {
		l2_iso_table_update(priv, i, 0x7ffffff, 0x7ffffff);
	}

	// reserve the mac table index 0 for router table
	priv->mac_tbl_bitmap[0] = 1;

	return ret;
}

static struct sfgenl_msg_ops l2_mac_genl_msg_ops = {
	.msg_recv = l2_mac_genl_msg_recv,
};

static int l2_mac_genl_init(void)
{
	return sfgenl_ops_register(SF_GENL_COMP_L2_MAC, &l2_mac_genl_msg_ops);
}

static int l2_mac_genl_exit(void)
{
	return sfgenl_msg_ops_unregister(SF_GENL_COMP_L2_MAC);
}

int dpns_mac_probe(struct platform_device *pdev)
{
	COMMON_t * common_priv = platform_get_drvdata(pdev);
	MAC_t *priv = NULL;
	int err;

	priv = devm_kzalloc(&pdev->dev, sizeof(MAC_t), GFP_KERNEL);
	if (priv == NULL)
		return -ENOMEM;

	priv->owq = alloc_ordered_workqueue("dpns_l2", WQ_MEM_RECLAIM);
	if (!priv->owq)
		return -ENOMEM;

	priv->ubus_wq = alloc_workqueue("dpns_l2_ubus",  WQ_UNBOUND | WQ_SYSFS, 0);
	if (!priv->ubus_wq)
		goto err_ubus_wq;

	g_mac_priv = priv;
	common_priv->mac_priv = priv;
	priv->cpriv = common_priv;

	/* hw io resource */
	priv->iobase = common_priv->iobase;

	atomic_set(&priv->work_cnt, 0);
	priv->l2_age_en = true;
	priv->l2_learning_en = true;
	priv->dpnsmib_en = true;

	memset(priv->dev_num, 0, sizeof(priv->dev_num));

	spin_lock_init(&priv->mac_lock);
	spin_lock_init(&priv->ts_lock);
	spin_lock_init(&priv->mac_tbl_lock);
	spin_lock_init(&priv->bit_lock);
	spin_lock_init(&priv->dev_num_lock);

	priv->ageing_time = L2_DEFAULT_AGEING_TIME;
	priv->age_update_time = L2_DEFAULT_AGEING_TIME;
	timer_setup(&priv->l2_cleanup_age_timer, dpns_l2_fdb_cleanup, 0);
	mod_timer(&priv->l2_cleanup_age_timer, jiffies);

	priv->mib_time = L2_MIB_DEFAULT_TIME;
	timer_setup(&priv->l2_cleanup_ts_timer, l2_cleanup_ts_timer, 0);
	mod_timer(&priv->l2_cleanup_ts_timer, jiffies);

	priv->switchdev_notifier.notifier_call = dpns_mac_switchdev_event,
	priv->switchdev_blocking_nb.notifier_call = dpns_mac_switchdev_blocking_event;
	priv->netdevice_nb.notifier_call = dpns_mac_netdevice_event;

	hash_init(priv->ts_list);
	INIT_LIST_HEAD(&priv->mac_table_list);

	priv->hw_search = dpns_mac_hw_search;
	priv->mac_table_update = se_l2_mac_table_update;
	priv->mac_table_del = se_l2_mac_table_del;
	priv->set_term_mac = se_l2_set_term_mac;
	priv->mac_del_entry = sf_mac_del_entry;
	priv->iso_table_update = l2_iso_table_update;
	priv->iso_table_dump = mac_iso_table_dump;
	priv->get_mibmode = sf_ts_mode;
	priv->sf_del_ts_info = sf_del_ts_info;
#ifdef CONFIG_SIWIFI_EASYMESH
	priv->nl_sock = netlink_kernel_create(&init_net, NETLINK_USERSOCK, NULL);
	if (!priv->nl_sock)
		L2_DBG(ERR_LV, "Failed to create netlink socket\n");
#endif
	/* Bridge fdb events */
	err = register_switchdev_notifier(&priv->switchdev_notifier);
	if (err) {
		L2_DBG(ERR_LV, "Failed to register switchdev notifier\n");
		goto err_register_switchdev_notifier;
	}

	err = register_switchdev_blocking_notifier(&priv->switchdev_blocking_nb);
	if (err) {
		L2_DBG(ERR_LV, "Failed to register switchdev blocking notifier\n");
		goto err_register_switchdev_blocking_notifier;
	}

	err = register_netdevice_notifier(&priv->netdevice_nb);
	if (err) {
		L2_DBG(ERR_LV, "Failed to register netdevice notifier\n");
		goto err_register_netdevice_notifier;
	}

	err = dpns_mib_proc_init(priv);
	if (err)
		goto err_procfs_create;

	l2_mac_genl_init();
	mac_init(priv);
	rtnl_lock();
	l2_iso_sync(priv);
	rtnl_unlock();
	printk("End %s\n", __func__);
	return 0;

err_procfs_create:
	unregister_netdevice_notifier(&priv->netdevice_nb);
err_register_netdevice_notifier:
	unregister_switchdev_blocking_notifier(&priv->switchdev_blocking_nb);
err_register_switchdev_blocking_notifier:
	unregister_switchdev_notifier(&priv->switchdev_notifier);
err_register_switchdev_notifier:
	sf_destroy_tslist(priv);
	sf_destroy_maclist(priv);
	destroy_workqueue(priv->ubus_wq);
err_ubus_wq:
	destroy_workqueue(priv->owq);
	return err;
}
EXPORT_SYMBOL(dpns_mac_probe);

void dpns_mac_remove(struct platform_device *pdev)
{
	COMMON_t * common_priv = platform_get_drvdata(pdev);
	MAC_t *priv = common_priv->mac_priv;
	timer_delete_sync(&priv->l2_cleanup_ts_timer);
	timer_delete_sync(&priv->l2_cleanup_age_timer);
#ifdef CONFIG_SIWIFI_EASYMESH
	if (priv->nl_sock) {
		netlink_kernel_release(priv->nl_sock);
		priv->nl_sock = NULL;
	}
#endif
	unregister_netdevice_notifier(&priv->netdevice_nb);
	unregister_switchdev_blocking_notifier(&priv->switchdev_blocking_nb);
	unregister_switchdev_notifier(&priv->switchdev_notifier);
	destroy_workqueue(priv->ubus_wq);
	destroy_workqueue(priv->owq);

	sf_destroy_tslist(priv);
	sf_destroy_maclist(priv);

	dpns_mib_proc_exit();

	l2_mac_genl_exit();
	common_priv->mac_priv = NULL;

	printk("End %s\n", __func__);
}
EXPORT_SYMBOL(dpns_mac_remove);

MODULE_LICENSE("GPL v2");
MODULE_AUTHOR("Huan.Liu <huan.liu@siflower.com.cn>");

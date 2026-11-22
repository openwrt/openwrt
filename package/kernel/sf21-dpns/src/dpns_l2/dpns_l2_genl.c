#include "dpns_l2_genl.h"

extern MAC_t *g_mac_priv;

void se_mac_table_dump(tbl_mac_t *table, u32 index)
{
	u8 mac[ETH_ALEN];

	u64_to_ether_addr(table->mac, mac);
	L2_DBG(DBG_LV, "NPU mac table: %d\n", index);
	L2_DBG(DBG_LV, "\t mac:	%pM\n",		mac);
	L2_DBG(DBG_LV, "\t vid:	%d\n",		table->vid);
	L2_DBG(DBG_LV, "\t valid:	%d\n",		table->valid);
	L2_DBG(DBG_LV, "\t age_en:	%d\n",		table->age_en);
	L2_DBG(DBG_LV, "\t l3_en:	%d\n",		table->l3_en);
	L2_DBG(DBG_LV, "\t spl_en:	%d\n",		table->spl_en);
	L2_DBG(DBG_LV, "\t spl_id:	%d\n",		table->spl_id);
	L2_DBG(DBG_LV, "\t mib_en:	%d\n",		table->mib_en);
	L2_DBG(DBG_LV, "\t mib_id:	%d\n",		table->mib_id);
	L2_DBG(DBG_LV, "\t da_cml:	%d\n",		table->da_cml);
	L2_DBG(DBG_LV, "\t sa_cml:	%d\n",		table->sa_cml);
	L2_DBG(DBG_LV, "\t port_map:	%llx\n",
		(u64)table->port_bitmap);
}

void mac_iso_table_dump(MAC_t *priv, u8 iport_num)
{
	union l2_iso_table_cfg param = {0};

	priv->cpriv->table_read(priv->cpriv, L2_ISO_TABLE,
			iport_num, param.data, sizeof(param));
	printk("\n---------------------------------------\n");
	printk("iso table:\n");
	printk("\t port_isolation_bitmap                0x%x\n",
		param.table.port_isolation_bitmap);
	printk("\t isolation_offload_bitmap             0x%x\n",
		param.table.isolation_offload_bitmap);
}

int se_l2_mac_table_dump(MAC_t *priv)
{
	unsigned long expire_age_time;
	int cur_depth, cur_width, age_mac_index = 0, mac_index, last_mac_index;
	u8 mac[ETH_ALEN];

	printk("idx  mac\t\t\t vid\t valid\t age\t l3\t spl"
		"\t spl-id\t mib\t mib-id\t da,sa\t map\t vlan-offload\t sta-id"
		"\t repeater-id\t expire(sec)\n");

	spin_lock_bh(&priv->bit_lock);
	last_mac_index = find_last_bit(priv->mac_tbl_bitmap, L2_MAC_NUM_MAX);

	for (mac_index = 1; mac_index <= last_mac_index; mac_index++) {
		union mac_table_cfg it = {};

		if (!test_bit(mac_index, priv->mac_tbl_bitmap)) {
			continue;
		}
		priv->cpriv->table_read(priv->cpriv, ARP_SE_MAC_TABLE, mac_index,
								(u32*)&it, sizeof(it));

		u64_to_ether_addr(it.table.mac, mac);

		expire_age_time = L2_DEFAULT_AGEING_TIME * 2 /HZ;
		if (it.table.age_en != 0) {
			cur_depth = mac_index / L2_AGE_REG_WIDTH;
			cur_width = mac_index % L2_AGE_REG_WIDTH;
			age_mac_index = se_read32(priv,
				SE_MAC_AGE_RAM + SE_MAC_AGE_REG_OFFSET_RAM *
				cur_depth) >> cur_width & 0x1;
			if (age_mac_index) {
				expire_age_time = (priv->l2_cleanup_age_timer.expires + priv->age_update_time - jiffies)/HZ;
			} else {
				expire_age_time = (priv->l2_cleanup_age_timer.expires - jiffies)/HZ;
			}

		}
		expire_age_time = expire_age_time > priv->age_update_time + priv->ageing_time ? 0 : expire_age_time;

		printk("%-4d %pM\t %u\t %u\t %u\t"
			" %u\t %u\t %u\t %u\t"
			" %u\t 0x%u,%u\t 0x%llX\t %u\t\t %u\t %u\t\t %lu\n",
			mac_index, mac,
			it.table.vid,
			it.table.valid,
			it.table.age_en,
			it.table.l3_en,
			it.table.spl_en,
			it.table.spl_id,
			it.table.mib_en,
			it.table.mib_id,
			it.table.da_cml,
			it.table.sa_cml,
			(u64)it.table.port_bitmap,
			it.table.vlan_offload_en,
			it.table.sta_id,
			it.table.repeater_id,
			expire_age_time);
	}
	spin_unlock_bh(&priv->bit_lock);

	return 0;
}

int se_l2_hash_dump(MAC_t *priv)
{
	int i, layer;

	u32 layer_offset = 0;
	const u8 layer_width[L2_HASH_TABLE_MAX] = {
		10, 9, 9, 8, 8, 7, 7, 7, 6, 6};

	L2_DBG(DBG_LV, "\n---------------------------------------\n");
	for (layer = 0; layer < 3; layer++) {
		for (i = 0; i < (0x1 << layer_width[layer]); i++) {
			u32 mac_index = 0;

			priv->cpriv->table_read(priv->cpriv, L2_SE_HASH0_TABLE,
						layer_offset + i,
						&mac_index, sizeof(mac_index));
			if (mac_index != 0)
				printk("\tRAM0: layer %d, req_addr %x@%X, mac idx %d\n",
					layer, layer_offset, i, mac_index);
		}
		layer_offset += (0x1 << (layer_width[layer]));
	}

	layer_offset = 0;
	for (layer = 3; layer < L2_HASH_TABLE_MAX; layer++) {
		for (i = 0; i < (0x1 << layer_width[layer]); i++) {
			u32 mac_index = 0;

			priv->cpriv->table_read(priv->cpriv, L2_SE_HASH1_TABLE,
						layer_offset + i,
						&mac_index, sizeof(mac_index));
			if (mac_index != 0)
				printk("\tRAM1: layer %d, req_addr %X@%X, mac idx %d\n",
					layer, layer_offset, i, mac_index);
		}
		layer_offset += (0x1 << (layer_width[layer]));
	}

	return 0;
}

void mac_spl_table_dump(MAC_t *priv, u8 index)
{
	union mac_spl_table_cfg param = {0};

	priv->cpriv->table_read(priv->cpriv, ARP_SE_MACSPL_TABLE, index*2,
				param.data, sizeof(param));
	printk("\n---------------------------------------\n");
	printk("smac spl table:\n");
	printk("\t credit                0x%x\n", param.table.credit);
	printk("\t cnt                   %u\n",	param.table.count);

	memset(&param, 0, sizeof(param));
	priv->cpriv->table_read(priv->cpriv, ARP_SE_MACSPL_TABLE, index*2 + 1,
				param.data, sizeof(param));
	printk("dmac spl table:\n");
	printk("\t credit                0x%x\n", param.table.credit);
	printk("\t cnt                   %u\n", param.table.count);
}

void sf_clear_ts_list(MAC_t *priv)
{
	struct sf_traffic_statics_info *pos = NULL;
	int bkt;
	size_t offset = offsetof(struct sf_ts_info, tx_bytes);
	size_t size = offsetof(struct sf_ts_info, mib_index) - offset;
	spin_lock_bh(&priv->ts_lock);
	hash_for_each(priv->ts_list, bkt, pos, snode)
	{
		memset((char*)&pos->ts_info + offset, 0, size);
	}
	spin_unlock_bh(&priv->ts_lock);
}

static int sf_set_nat_mib_en(MAC_t *priv, u32 *public_ip, u32 *private_ip, u32 *router_ip,
		      u16 public_port, u16 private_port, u16 router_port, u8 mib_mode,
		      u16 mib_index, int mib_op, bool is_v6, bool is_udp, bool is_dnat)
{
	struct sf_traffic_statics_info *ts_newinfo;
	struct dpns_natmib_info info;
	struct dpns_nat_priv *nat_priv;
	int nat_id, i;
	u16 crc_16;

	if (!priv->dpnsmib_en) {
		L2_DBG(ERR_LV, "dpnsmib_en is not enabled\n");
		return -ENOTSUPP;
	}

	priv->mibmode = mib_mode;
	nat_priv = priv->cpriv->nat_priv;

	info.nat_id = &nat_id;
	info.mib_index = mib_index;
	info.mib_mode = mib_mode;
	info.public_port = public_port;
	info.private_port = private_port;
	info.router_port = router_port;
	info.is_v6 = is_v6;
	info.is_udp = is_udp;
	info.is_dnat = is_dnat;

	for (i = 0; i < 4; i++) {
		info.public_ip[i] = public_ip[i];
		info.private_ip[i] = private_ip[i];
		info.router_ip[i] = router_ip[i];
	}

	if (mib_op == MIB_ON) {
		info.natmib_en = true;
		nat_priv->set_natmib_en(nat_priv, &info);

		if (nat_id < 0)
			return -EINVAL;
		crc_16 = crc16_custom((u8*)&nat_id, sizeof(nat_id), 0);

		if (sf_search_ts_entry(priv, NULL, 0, nat_id, crc_16))
			return 0;

		ts_newinfo = l2_kzalloc(sizeof(struct sf_traffic_statics_info), GFP_KERNEL);

		ts_newinfo->ts_info.nat_id	= nat_id;
		ts_newinfo->ts_info.mib_index	= mib_index;
		ts_newinfo->ts_info.mode 	= mib_mode;

		spin_lock_bh(&priv->ts_lock);
		hash_add(priv->ts_list,  &(ts_newinfo->snode), crc_16);
		spin_unlock_bh(&priv->ts_lock);
	} else if (mib_op == MIB_OFF) {
		nat_priv->set_natmib_en(nat_priv, &info);
		if (nat_id < 0)
			return -EINVAL;

		sf_del_ts_info(priv, NULL, 0, nat_id, crc_16);
	} else if (mib_op == MIB_CLEAR) {
		sf_clear_ts_list(priv);
	}

	return 0;
}

static void sf_set_dpnsmib_en(MAC_t *priv, bool dpnsmib_en)
{
	if (dpnsmib_en == true) {
		priv->dpnsmib_en = dpnsmib_en;
	} else {
		sf_destroy_tslist(priv);
		priv->dpnsmib_en = dpnsmib_en;
	}
}

void dpns_l2_ubus_handler(struct work_struct *work)
{
	struct l2_ubus_work *ubus_work = container_of(work, struct l2_ubus_work, work);
	struct l2_mac_genl_msg_add *msg = ubus_work->msg;
	int err = 0;
	u32 result_data[2];
	u8 mac[ETH_ALEN];
	bool hit;

	switch (msg->method) {
		case L2_MAC_ADD:
			u64_to_ether_addr(msg->mac, mac);
			err = se_l2_mac_table_update(g_mac_priv, mac, 1,
					msg->vid,BIT(msg->port),msg->mage_en,
					msg->l3_en,msg->sa_cml, msg->da_cml,
					msg->vlan_en,msg->sta_id,
					msg->repeater_id);

			if (err < 0)
				L2_DBG(ERR_LV, "ubus call dpns.l2 add err: %pe\n", ERR_PTR(err));
			break;
		case L2_MAC_MIB_EN:
			u64_to_ether_addr(msg->mac, mac);
			if (msg->mib_mode < 8)
				err = sf_set_l2_mib_en(g_mac_priv, mac,
						msg->vid,msg->mib_mode,
						msg->mib_index, msg->mib_op);
			else
				err = -EINVAL;

			if (err < 0)
				L2_DBG(ERR_LV, "ubus call dpns.l2 macmib err: %pe\n", ERR_PTR(err));
			break;
		case L2_MAC_SPL_EN:
			u64_to_ether_addr(msg->mac, mac);
			L2_DBG(INFO_LV, "mac:%pM vid:%d spl_en:%d spl_index:%d\
					scredit:%d dcredit:%d\n",mac, msg->vid,
					msg->l2_spl_mode, msg->spl_index,
					msg->scredit, msg->dcredit);
			if (msg->spl_index < L2_SPL_NUM_MAX / 2)
				err = sf_set_l2_spl_en(g_mac_priv, mac,
						msg->vid, msg->l2_spl_mode,
						msg->spl_index, msg->scredit,
						msg->dcredit);
			else
				err = -EINVAL;

			if (err < 0)
				L2_DBG(ERR_LV, "ubus call dpns.l2 spl err: %pe\n", ERR_PTR(err));
			break;
		case L2_MAC_DEL:
			u64_to_ether_addr(msg->mac, mac);
			err = sf_mac_del_entry(g_mac_priv, mac, msg->vid, false, false);
			break;
		case L2_MAC_SET_AGEING_EN:
			g_mac_priv->l2_age_en = msg->enable;
			if (msg->enable)
				L2_DBG(DBG_LV, "l2_age_en is true, open l2 age");
			else
				L2_DBG(DBG_LV, "l2_age_en is false, close l2 age");
			break;
		case L2_MAC_SET_LEARNING_EN:
			g_mac_priv->l2_learning_en = msg->enable;
			break;
		case L2_MAC_SET_AGE_TIME:
			g_mac_priv->age_update_time = msg->value * HZ/2;
			break;
		case L2_MAC_DUMP_MAC_TB:
				se_l2_hash_dump(g_mac_priv);
				se_l2_mac_table_dump(g_mac_priv);
			break;
		case L2_MAC_DUMP_SPL_TB:
			mac_spl_table_dump(g_mac_priv, msg->value);
			break;
		case L2_MAC_CLEAR:
			sf_mac_clear(g_mac_priv);
			break;
		case L2_MAC_NUM_DUMP:
			sf_mac_num_dump(g_mac_priv);
			break;
		case NAT_MIB_EN:
			err = sf_set_nat_mib_en(g_mac_priv, msg->public_ip, msg->private_ip,
					  msg->router_ip, msg->public_port, msg->private_port,
					  msg->router_port, msg->mib_mode, msg->mib_index,
					  msg->mib_op, msg->is_v6, msg->is_udp, msg->is_dnat);

			if (err < 0)
				L2_DBG(ERR_LV, "ubus call dpns.l2 natmib err: %pe\n", ERR_PTR(err));
			break;
		case DPNS_MIB_EN:
			sf_set_dpnsmib_en(g_mac_priv, msg->mib_en);
			break;
		case L2_MAC_HIT:
			u64_to_ether_addr(msg->mac, mac);
			hit = dpns_mac_hw_search(g_mac_priv, mac, msg->vid, result_data);
			if (hit)
				printk("the mac is in mac table now\n");
			else
				printk("the mac isn't in mac table now\n");
			break;
		default:
			err = -EINVAL;
	}

	l2_kfree(ubus_work->msg);
	l2_kfree(ubus_work);
}

int l2_mac_genl_msg_recv(struct genl_info *info, void *buf, size_t buflen)
{
	struct l2_mac_genl_msg_add *msg = buf;
	struct l2_ubus_work *ubus_work;
	int err = 0;
	// u8 mac[ETH_ALEN];

	ubus_work = l2_kzalloc(sizeof(*ubus_work), GFP_ATOMIC);
	if (!ubus_work)
		return -ENOMEM;

	ubus_work->msg = l2_kzalloc(sizeof(struct l2_mac_genl_msg_add), GFP_ATOMIC);
	if (!ubus_work->msg)
		goto err_msg_alloc;

	INIT_WORK(&ubus_work->work, dpns_l2_ubus_handler);

	if (WARN_ON_ONCE(!g_mac_priv))
		goto err_mac_priv;

	ubus_work->priv = g_mac_priv;
	*(ubus_work->msg) = *msg;
	queue_work(g_mac_priv->ubus_wq, &ubus_work->work);

	sfgenl_msg_reply(info, &err, sizeof(err));

	return err;
err_mac_priv:
	l2_kfree(ubus_work->msg);
err_msg_alloc:
	l2_kfree(ubus_work);
	err = -ENOMEM;
	sfgenl_msg_reply(info, &err, sizeof(err));
	return -ENOMEM;
}

void sf_mac_num_dump(MAC_t *priv)
{
	int mac_index = 0,num = 0, last_mac_index;

	spin_lock_bh(&priv->bit_lock);
	last_mac_index = find_last_bit(priv->mac_tbl_bitmap, L2_MAC_NUM_MAX);

	for (mac_index = 1; mac_index <= last_mac_index; mac_index++) {
		if (!test_bit(mac_index, priv->mac_tbl_bitmap)) {
			continue;
		}
		num++;
	}
	spin_unlock_bh(&priv->bit_lock);
	printk("the num of the l2 mac is %d\n", num);
}

int sf_set_l2_mib_en(MAC_t *priv, u8 *dsmac,
		u16 vlan_id, u8 l2_mib_mode, u16 mib_index, int mib_op)
{
	union mac_table_cfg key = {};
	l2_hash_key_t hashkey;
	struct sf_traffic_statics_info *ts_newinfo, *pos;
	int hit;
	u32 result_data[2];
	u16 dmac_index, crc_16;
	u8 mac[ETH_ALEN];

	if (!priv->dpnsmib_en) {
		L2_DBG(ERR_LV, "dpnsmib_en is not enabled\n");
		return -ENOTSUPP;
	}

	/* manually calculate the value of crc */
	ether_addr_copy(hashkey.mac, dsmac);
	crc_16 = crc16_custom((u8*)&hashkey, sizeof(hashkey), 0);

	hit = dpns_mac_hw_search(priv, dsmac, vlan_id, result_data);

	if (hit && mib_op == MIB_ON) {
		dmac_index = FIELD_GET(SE_HW_RESULT0_DATA1_MAC_IDX, result_data[1]);
		priv->cpriv->table_read(priv->cpriv, ARP_SE_MAC_TABLE,
				dmac_index, (u32*)&key,
				sizeof(key));
		u64_to_ether_addr(key.table.mac, mac);
		L2_DBG(INFO_LV, "index: %u mac: %pM vid: %u l2_mib_mode: %u\
				mib_index: %u\n", dmac_index, mac,
				key.table.vid, key.table.mib_en,
				key.table.mib_id);
		key.table.mib_en = l2_mib_mode;
		key.table.mib_id = mib_index;

		priv->cpriv->table_write(priv->cpriv, ARP_SE_MAC_TABLE,
				dmac_index, (u32*)&key, sizeof(key));
		L2_DBG(INFO_LV, "index: %u mac: %pM vid: %u l2_mib_mode: %u\
				mib_index: %u\n",dmac_index, mac,
				key.table.vid, key.table.mib_en,
				key.table.mib_id);
		if (!sf_search_ts_entry(priv, dsmac, vlan_id, 0, crc_16)) {
			ts_newinfo = l2_kzalloc(sizeof(struct sf_traffic_statics_info),
						GFP_KERNEL);

			ether_addr_copy(ts_newinfo->ts_info.mac, dsmac);
			ts_newinfo->ts_info.mib_index	= mib_index;
			ts_newinfo->ts_info.vid	= vlan_id;
			ts_newinfo->ts_info.mode = l2_mib_mode;

			spin_lock_bh(&priv->ts_lock);
			hash_add(priv->ts_list,  &(ts_newinfo->snode), crc_16);
			spin_unlock_bh(&priv->ts_lock);
		} else {
			spin_lock_bh(&priv->ts_lock);
			hash_for_each_possible(priv->ts_list, pos, snode, crc_16)
			{
				if (ether_addr_equal(pos->ts_info.mac, mac) &&
					pos->ts_info.vid == vlan_id) {
					pos->ts_info.mib_index = mib_index;
					pos->ts_info.mode = l2_mib_mode;
					L2_DBG(DBG_LV,"update ts mode successfully");
					break;
				}
			}
			spin_unlock_bh(&priv->ts_lock);
		}
	} else if (hit && mib_op == MIB_OFF) {
		dmac_index = FIELD_GET(SE_HW_RESULT0_DATA1_MAC_IDX, result_data[1]);
		priv->cpriv->table_read(priv->cpriv, ARP_SE_MAC_TABLE,
				dmac_index, (u32*)&key,
				sizeof(key));
		u64_to_ether_addr(key.table.mac, mac);
		L2_DBG(INFO_LV, "index: %u mac: %pM vid: %u l2_mib_mode: %u\
				mib_index: %u\n", dmac_index, mac,
				key.table.vid, key.table.mib_en,
				key.table.mib_id);
		key.table.mib_en = 0;
		key.table.mib_id = 0;

		priv->cpriv->table_write(priv->cpriv, ARP_SE_MAC_TABLE,
				dmac_index, (u32*)&key, sizeof(key));
		L2_DBG(INFO_LV, "index: %u mac: %pM vid: %u l2_mib_mode: %u\
				mib_index: %u\n",dmac_index, mac,
				key.table.vid, key.table.mib_en,
				key.table.mib_id);

		sf_del_ts_info(priv, dsmac, vlan_id, 0, crc_16);
	} else if (hit && mib_op == MIB_CLEAR) {
		sf_clear_ts_list(priv);
	} else {
		L2_DBG(DBG_LV, "dmac: %pM vlan_id: %u not found!!!\n", dsmac,
				vlan_id);
		return -1;
	}

	return 0;
}

int sf_set_l2_spl_en(MAC_t *priv, u8 *dsmac, u16 vlan_id,
		bool l2_spl_mode, u8 spl_index, u32 scredit, u32 dcredit)
{
	union mac_table_cfg key = {};
	int hit;
	u32 result_data[2];
	u16 dmac_index;
	u8 mac[ETH_ALEN];

	hit = dpns_mac_hw_search(priv, dsmac, vlan_id, result_data);

	if (hit) {
		dmac_index = FIELD_GET(SE_HW_RESULT0_DATA1_MAC_IDX, result_data[1]);
		priv->cpriv->table_read(priv->cpriv, ARP_SE_MAC_TABLE,
				dmac_index, (u32*)&key,
				sizeof(key));
		u64_to_ether_addr(key.table.mac, mac);
		L2_DBG(INFO_LV,
				"index: %u mac: %pM vid: %u l2_spl_mode: %u\
				spl_index: %u\n", dmac_index, mac,
				key.table.vid, key.table.spl_en,
				key.table.spl_id);

		key.table.spl_en = l2_spl_mode;
		key.table.spl_id = spl_index;

		priv->cpriv->table_write(priv->cpriv, ARP_SE_MAC_TABLE,
				dmac_index, (u32*)&key,
				sizeof(key));
		L2_DBG(INFO_LV,
				"index: %u mac: %pM vid: %u l2_spl_mode: %u\
				spl_index: %u\n",dmac_index, mac,
				key.table.vid, key.table.spl_en,
				key.table.spl_id);

		//smac spl
		sf_mac_spl_unlimit(priv, spl_index*2, scredit);
		//dmac spl
		sf_mac_spl_unlimit(priv, spl_index*2+1, dcredit);

	} else {
		L2_DBG(DBG_LV, "dmac: %pM vlan_id: %u not found!!!\n", dsmac,
				vlan_id);
		return -1;
	}

	return 0;
}

bool sf_search_ts_entry(MAC_t *priv, u8 *mac, u16 vlan_id, int nat_id, u16 soft_key_crc)
{
	struct sf_traffic_statics_info *pos;

	spin_lock_bh(&priv->ts_lock);
	if (mac != NULL) {
		hash_for_each_possible(priv->ts_list, pos, snode, soft_key_crc)
		{
			if (ether_addr_equal(pos->ts_info.mac, mac) &&
				pos->ts_info.vid == vlan_id) {
				spin_unlock_bh(&priv->ts_lock);
				return true;
			}
		}
	} else {
		hash_for_each_possible(priv->ts_list, pos, snode, soft_key_crc)
		{
			if (pos->ts_info.nat_id == nat_id) {
				spin_unlock_bh(&priv->ts_lock);
				return true;
			}
		}
	}
	spin_unlock_bh(&priv->ts_lock);
	return false;
}

void sf_mac_spl_unlimit(MAC_t *priv, u16 spl_index, u32 credit)
{
	union mac_spl_table_cfg param = {0};
	param.table.credit = credit;

	priv->cpriv->table_write(priv->cpriv, ARP_SE_MACSPL_TABLE, spl_index,
						param.data, sizeof(param));
}

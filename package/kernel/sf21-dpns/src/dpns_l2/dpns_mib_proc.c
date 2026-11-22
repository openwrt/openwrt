#include <linux/proc_fs.h>
#include "dpns_mib_proc.h"

extern MAC_t *g_mac_priv;

void sf_proc_clear_ts_list(MAC_t *priv)
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

int sf_proc_set_l2_mib_en(MAC_t *priv, u8 *dsmac,
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
		sf_proc_clear_ts_list(priv);
	} else {
		L2_DBG(DBG_LV, "dmac: %pM vlan_id: %u not found!!!\n", dsmac,
				vlan_id);
		return -1;
	}

	return 0;
}

static void dpns_read_nat_mib(MAC_t* priv, int index, u64 *pkts, u64 *bytes)
{
	*pkts = se_read32(priv, NPU_MIB_ADDR + index * 4);
	*bytes = se_read32(priv, NPU_MIB_BYTES_LO) | (u64)se_read32(priv, NPU_MIB_BYTES_HI) << 32;
	*bytes = ((*bytes / *pkts) + CRC_LEN + PREAMBLE_LEN + IFG_LEN ) * *pkts;
}

static void dpns_read_mac_mib(MAC_t* priv, int index, u64 *pkts, u64 *bytes)
{
	*pkts = se_read32(priv, NPU_MIB_ADDR + index * 4);
	*bytes = se_read32(priv, NPU_MIB_BYTES_LO) |
			   (u64)se_read32(priv, NPU_MIB_BYTES_HI) << 32;
}

/*
 *
 |mib_mode[2:0] | smac_rx | smac_tx | dmac_rx  |  dmac_tx  |
 |   0          | no      |  no     |   no     |     no    |
 |   1          | addr1   |  no     |   addr0  |     no    |
 |   2          | addr1   |  no     |   no     |     addr0 |
 |   3          | no      |  addr1  |   addr0  |     no    |
 |   4          | no      |  addr1  |   no     |     addr0 |
 |   5          | addr1   |  addr0  |   no     |     no    |
 |   6          | no      |  no     |   addr1  |     addr0 |
 |   7          | addr5   |  addr4  |   addr3  |     addr2 |
 *
 */

int dpns_mib(MAC_t* priv)
{
	struct sf_traffic_statics_info *ts_info;
	int bkt;

	spin_lock_bh(&priv->ts_lock);
	hash_for_each(priv->ts_list, bkt, ts_info, snode)
	{
		u32 rx_rate = 0, tx_rate = 0, total_rate = 0;
		u64 rx_bytes = 0, tx_bytes = 0, rx_pkts = 0, tx_pkts = 0, total_pkts = 0, total_bytes = 0;

		if (ts_info->ts_info.mac != NULL && !is_zero_ether_addr(ts_info->ts_info.mac)) {//macmib
			//addr0 mib,Rx Count Frame
			dpns_read_mac_mib(priv, ts_info->ts_info.mib_index * 2, &rx_pkts, &rx_bytes);
			rx_rate = (rx_bytes * 8) / (priv->mib_time / HZ + 1);
			//addr1 mib,TX Count Frame
			dpns_read_mac_mib(priv, ts_info->ts_info.mib_index * 2 + 1, &tx_pkts, &tx_bytes);
			tx_rate = (tx_bytes * 8) / (priv->mib_time / HZ + 1);

			ts_info->ts_info.rx_rate  =  rx_rate;
			ts_info->ts_info.tx_rate  =  tx_rate;
			ts_info->ts_info.rx_bytes += rx_bytes;
			ts_info->ts_info.tx_bytes += tx_bytes;
			ts_info->ts_info.rx_pkts += rx_pkts;
			ts_info->ts_info.tx_pkts += tx_pkts;
		} else {//natmib
			switch (priv->mibmode) {
			case 0:
			case 1:
			case 2:
			case 3:
			case 4:
			case 8:
				//addr0 mib,DNAT Count Frame
				dpns_read_nat_mib(priv, ts_info->ts_info.mib_index * 2, &rx_pkts, &rx_bytes);
				rx_rate = (rx_bytes * 8) / (priv->mib_time / HZ + 1);
				//addr1 mib,SNAT Count Frame
				dpns_read_nat_mib(priv, ts_info->ts_info.mib_index * 2 + 1, &tx_pkts, &tx_bytes);
				tx_rate = (tx_bytes * 8) / (priv->mib_time / HZ + 1);

				ts_info->ts_info.rx_rate  =  rx_rate;
				ts_info->ts_info.tx_rate  =  tx_rate;
				ts_info->ts_info.rx_bytes += rx_bytes;
				ts_info->ts_info.tx_bytes += tx_bytes;
				ts_info->ts_info.rx_pkts += rx_pkts;
				ts_info->ts_info.tx_pkts += tx_pkts;
				break;
			case 5:
			case 13:
				//addr1 mib,SNAT Count Frame
				dpns_read_nat_mib(priv, ts_info->ts_info.mib_index * 2 + 1, &tx_pkts, &tx_bytes);
				tx_rate = (tx_bytes * 8) / (priv->mib_time / HZ + 1);

				ts_info->ts_info.tx_rate  =  tx_rate;
				ts_info->ts_info.tx_bytes += tx_bytes;
				ts_info->ts_info.tx_pkts += tx_pkts;
				break;
			case 6:
			case 14:
				//addr1 mib,DNAT Count Frame
				dpns_read_nat_mib(priv, ts_info->ts_info.mib_index * 2 + 1, &rx_pkts, &rx_bytes);
				rx_rate = (rx_bytes * 8) / (priv->mib_time / HZ + 1);

				ts_info->ts_info.rx_rate  =  rx_rate;
				ts_info->ts_info.rx_bytes += rx_bytes;
				ts_info->ts_info.rx_pkts += rx_pkts;
				break;
			case 9:
			case 10:
			case 11:
			case 12:
				//addr1 mib,DNAT Count Frame
				dpns_read_nat_mib(priv, ts_info->ts_info.mib_index, &total_pkts, &total_bytes);
				total_rate = (total_bytes * 8) / (priv->mib_time / HZ + 1);

				ts_info->ts_info.total_rate  =  total_rate;
				ts_info->ts_info.total_bytes += total_bytes;
				ts_info->ts_info.total_pkts += total_pkts;
				break;
			case 7:
			case 15:
				//addr0 mib,DNAT Count Frame
				dpns_read_nat_mib(priv, ts_info->ts_info.mib_index * 2 + 3, &rx_pkts, &rx_bytes);
				rx_rate = (rx_bytes * 8) / (priv->mib_time / HZ + 1);
				//addr1 mib,SNAT Count Frame
				dpns_read_nat_mib(priv, ts_info->ts_info.mib_index * 2 + 1, &tx_pkts, &tx_bytes);
				tx_rate = (tx_bytes * 8) / (priv->mib_time / HZ + 1);

				ts_info->ts_info.rx_rate  =  rx_rate;
				ts_info->ts_info.tx_rate  =  tx_rate;
				ts_info->ts_info.rx_bytes += rx_bytes;
				ts_info->ts_info.tx_bytes += tx_bytes;
				ts_info->ts_info.rx_pkts += rx_pkts;
				ts_info->ts_info.tx_pkts += tx_pkts;
				break;
			}
		}
	}
	spin_unlock_bh(&priv->ts_lock);
	return 0;
}

static int sf_l2_mib_show(struct seq_file *file)
{
	struct sf_traffic_statics_info *pos = NULL;
	MAC_t *priv = file->private;
	int bkt;

	seq_printf(file, "=========== mac mib part ==========\n");
	seq_printf(file, "%-24s %-20s %-20s %-20s %-20s %-20s\n",
			"mac", "vlan", "upload(bps)", "download(bps)", "total.up(byte)", "total.down(byte)");
	spin_lock_bh(&priv->ts_lock);
	hash_for_each(priv->ts_list, bkt, pos, snode)
	{
		if (!pos->ts_info.mac || is_zero_ether_addr(pos->ts_info.mac))
			continue;
		seq_printf(file,
			"%pM        %-20u %-20u %-20u  %-20llu  %-20llu\n",
			pos->ts_info.mac,pos->ts_info.vid,
			pos->ts_info.tx_rate, pos->ts_info.rx_rate,
			pos->ts_info.tx_bytes, pos->ts_info.rx_bytes);
	}
	spin_unlock_bh(&priv->ts_lock);
	seq_printf(file, "\n");

	return 0;
}

static int sf_nat_mib_show(struct seq_file *file)
{
	struct sf_traffic_statics_info *pos = NULL;
	MAC_t *priv = file->private;
	int bkt;

	seq_printf(file, "=========== nat mib part ==========\n");
	seq_printf(file, "%-10s %-20s %-20s %-20s %-20s %-20s %-20s %-20s %-20s %-20s\n",
			"nat_id", "snat(bps)", "dnat(bps)", "total(bps)", "snat(byte)", "dnat(byte)",
			"total(byte)", "snat_pkts", "dnat_pkts", "total_pkts");
	spin_lock_bh(&priv->ts_lock);
	hash_for_each(priv->ts_list, bkt, pos, snode)
	{
		if (!is_zero_ether_addr(pos->ts_info.mac))
			continue;
		seq_printf(file,
			"%-10d %-20u %-20u %-20u %-20llu %-20llu %-20llu %-20llu %-20llu %-20llu\n",
			pos->ts_info.nat_id, pos->ts_info.tx_rate,
			pos->ts_info.rx_rate, pos->ts_info.total_rate, pos->ts_info.tx_bytes,
			pos->ts_info.rx_bytes, pos->ts_info.total_bytes, pos->ts_info.tx_pkts,
			pos->ts_info.rx_pkts, pos->ts_info.total_pkts);
	}
	spin_unlock_bh(&priv->ts_lock);
	seq_printf(file, "\n");
	return 0;
}

static int dpns_mib_show(struct seq_file *file, void *data)
{
	sf_l2_mib_show(file);
	sf_nat_mib_show(file);
	return 0;
}

static int sf_l2_mib_open(struct inode *inode, struct file *file)
{
	return single_open(file, dpns_mib_show, pde_data(inode));
}

static const struct proc_ops sf_l2_mib_ops = {
	.proc_open	= sf_l2_mib_open,
	.proc_read	= seq_read,
	.proc_lseek	= seq_lseek,
	.proc_release 	= single_release,
};

int dpns_mib_proc_init(struct dpns_mac_priv *priv)
{
	priv->mac_mib = proc_create_data("dpns_mib",
			0644, NULL, &sf_l2_mib_ops, (void*)priv);

	if (!priv->mac_mib) {
		L2_DBG(ERR_LV, "dpns mib proc create failed!\n");
		return -ENOMEM;
	}

        return 0;
}

int dpns_mib_proc_exit(void)
{
        remove_proc_entry("dpns_mib", NULL);

	return 0;
}

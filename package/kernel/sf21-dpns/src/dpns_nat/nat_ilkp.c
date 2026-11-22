#include <dpns_common.h>
#include "nat.h"
#include "nat_ilkp.h"
#include <linux/string.h>
#include <linux/timekeeping.h>

/**
 * Limit IPv4 to the first 8 entries.
 * IPv6 uses the same IP lookup table as IPv4 2nd slot.
 * Leave the rest for IPv6 to make IP lookup table management easier.
 */
#define RT_IP4_NUM_ENTRIES 8
/* IPv6 uses 8-15. */
#define RT_IP6_OFFSET 8
#define RT_IP6_NUM_ENTRIES 8

#define NAT_HASH0_SIZE 0x2000

static const u16 nat_hash_mask[] = {
	0xfff, 0x7ff, 0x7ff,	// hash0 0x2000
	0x3ff, 0x3ff, 0x3ff, 0x1ff, 0x1ff,	// hash1 0x1000
};
static const u16 nat_hash_offset[] = {
	0, 0x1000, 0x1800,	// hash0
	0, 0x400, 0x800, 0xc00, 0xe00,	// hash1
};

#define DPNS_NAT_ENTRY_ID_INVALID 0xffff

/* FIXME: global variables */
static ip_address Router_ip[RT_IP4_NUM_ENTRIES];
static ip6_address Router_ip6[RT_IP6_NUM_ENTRIES];

int dpns_nat_ilkp_init(struct dpns_nat_priv *priv) {
	memset(Router_ip, 0, sizeof(Router_ip));
	memset(Router_ip6, 0, sizeof(Router_ip6));
	NAT_DBG(DBG_LV, "ilkp init\n");

	return 0;
}

void dpns_nat_ilkp_exit(struct dpns_nat_priv *priv) {
	NAT_DBG(DBG_LV, "ilkp exit\n");
}

void dpns_nat_dump_ilkp4_entry(nat_ipv4_table *tb) {
	NAT_DBG(DBG_LV, "%016llX %016llX %016llX\n", tb->data[0], tb->data[1], tb->data[2]);
	if(!tb->valid) {
		NAT_DBG(DBG_LV, "invalid.\n");
		return;
	}
	NAT_DBG(DBG_LV, "%s: %pI4h:%u > [%u]%pI4h:%u > %pI4h:%u\n",
		tb->l4_type ? "UDP" : "TCP",
		&tb->public_ip, tb->public_port,
		tb->router_ip_index, &Router_ip[tb->router_ip_index].ip, tb->router_port,
		&tb->private_ip, tb->private_port
		);
	NAT_DBG(DBG_LV, "mac_idx: srt: %u drt: %u pri: %u pub: %u\n",
		tb->srtmac_index, tb->drtmac_index, tb->primac_index, tb->pubmac_index);
	NAT_DBG(DBG_LV, "repl_pri(%u): %u stat(%u): %u spl(%u): %u\n",
		tb->repl_pri_en, tb->repl_pri, tb->stat_en, tb->stat_index, tb->spl_en, tb->spl_index
	);
	NAT_DBG(DBG_LV, "oport_id: s: %u d: %u\n",
		tb->soport_id, tb->doport_id);
}

void dpns_nat_dump_ipv4_data(struct nat_ipv4_data *tb) {
	NAT_DBG(DBG_LV, "%s: %pI4h:%u > %pI4h:%u > %pI4h:%u\n",
		tb->l4_type ? "UDP" : "TCP",
		&tb->public_ip, tb->public_port,
		&tb->router_ip, tb->router_port,
		&tb->private_ip, tb->private_port
		);
	NAT_DBG(DBG_LV, "crc16: 0: %04X 1: %04X 2: %04X 3: %04X 4: %04X 5: %04X 6: %04X 7: %04X\n",
		tb->crc16_poly[0], tb->crc16_poly[1], tb->crc16_poly[2], tb->crc16_poly[3], tb->crc16_poly[4], tb->crc16_poly[5], tb->crc16_poly[6], tb->crc16_poly[7]);
	NAT_DBG(DBG_LV, "mac_idx: srt: %u drt: %u pri: %u pub: %u\n",
		tb->srtmac_index, tb->drtmac_index, tb->primac_index, tb->pubmac_index);
	NAT_DBG(DBG_LV, "oport_id: s: %u d: %u\n",
		tb->soport_id, tb->doport_id);
}

/**
 * return:
 *   negative on error
 *   0 or positive for nat_id (index * 2 on INAPT0, index * 2 + 1 on INAPT1)
 */
int dpns_nat_new_ilkp4_entry(struct dpns_nat_priv *priv, struct nat_ipv4_data *data) {
	nat_ipv4_table tb = {
		.valid = 1,
	};
	bool second_slot = false;
	int index;
	int ret;
	int bm_block;
	int i;

	enum se_nat_tb_op_req_id nat_table;

	/**
	 * Find a free slot in the hashtable
	 * Bitmaps are ulong arrays.
	 * Let's assume NAT_ILKP_SZ is a multiple of BITS_PER_LONG.
	 */
	for (bm_block = 0; bm_block < NAT_ILKP_SZ / BITS_PER_LONG ; bm_block++) {
		unsigned long bm0 = priv->nat0_bitmap[bm_block];
		unsigned long bm1 = priv->nat1_bitmap[bm_block];
		unsigned long xor_bm;

		xor_bm = bm0 ^ bm1;
		if(xor_bm) {
			/* nat0 or nat1 is free */
			size_t bit_pos = __ffs(xor_bm);
			second_slot = !!(bm0 & BIT(bit_pos));
			index = bm_block * BITS_PER_LONG + bit_pos;
			break;
		}

		/* nat0 and nat1 are both free or both occupied. */
		if(!(~bm0))
			continue; /* bits in bm0 are all 1. */
		/* take nat0 */
		index = bm_block * BITS_PER_LONG + __ffs(~bm0);
		second_slot = false;
		break;
	}

	if (bm_block == NAT_ILKP_SZ / BITS_PER_LONG)
		return -ENOSPC;

	/* find if there are existing entry for current router ip */
	for (i = 0 ; i < RT_IP4_NUM_ENTRIES ; i++) {
		if(Router_ip[i].ip == data->router_ip) {
			tb.router_ip_index = i;
			break;
		}
	}

	if (i == RT_IP4_NUM_ENTRIES) {
		/* Not found. Insert a new one at a free slot. */
		for (i = 0 ; i < RT_IP4_NUM_ENTRIES ; i++) {
			if(!Router_ip[i].refcnt) {
				/**
				 * To fill both slots in NAPT it's likely we need
				 * the same address on both IP_RAM anyway.
				 * Write both and track once.
				 */
				sf_writel(priv, SE_NAT_TB_WRDATA0, data->router_ip);
				sf_writel(priv, SE_NAT_TB_OP, NAT_TB_OP_WR |
				FIELD_PREP(NAT_TB_OP_REQ_ID, NAT_RT_IP_RAM0) |
				FIELD_PREP(NAT_TB_OP_REQ_ADDR, i));
				dpns_nat_wait_rw(priv);

				sf_writel(priv, SE_NAT_TB_OP, NAT_TB_OP_WR |
				FIELD_PREP(NAT_TB_OP_REQ_ID, NAT_RT_IP_RAM1) |
				FIELD_PREP(NAT_TB_OP_REQ_ADDR, i));

				tb.router_ip_index = i;
				Router_ip[i].ip = data->router_ip;
				NAT_DBG(DBG_LV, "new router IP:%pI4h at %d\n", &Router_ip[i].ip, i);
				dpns_nat_wait_rw(priv);
				break;
			}
		}
	}

	if (i == RT_IP4_NUM_ENTRIES) {
		/* No free slot in Router_ip. */
		return -ENOSPC;
	}

	if (second_slot) {
		nat_table = NAPT1_TABLE;
		set_bit(index, priv->nat1_bitmap);
	} else {
		nat_table = NAPT0_TABLE;
		set_bit(index, priv->nat0_bitmap);
	}

	/* start writing */
	tb.public_ip = data->public_ip;
	tb.public_port = data->public_port;
	tb.private_ip = data->private_ip;
	tb.private_port = data->private_port;
	// tb.router_ip_index assigned before.
	tb.router_port = data->router_port;
	tb.l4_type = data->l4_type;
	tb.srtmac_index = data->srtmac_index;
	tb.drtmac_index = data->drtmac_index;
	tb.primac_index = data->primac_index;
	tb.pubmac_index = data->pubmac_index;
	tb.soport_id = data->soport_id;
	tb.doport_id = data->doport_id;

	sf_writeq(priv, SE_NAT_TB_WRDATA0, tb.data[0]);
	sf_writeq(priv, SE_NAT_TB_WRDATA0 + 8, tb.data[1]);
	sf_writeq(priv, SE_NAT_TB_WRDATA0 + 16, tb.data[2]);

	sf_writel(priv, SE_NAT_TB_OP, NAT_TB_OP_WR |
		FIELD_PREP(NAT_TB_OP_REQ_ID, nat_table) |
		FIELD_PREP(NAT_TB_OP_REQ_ADDR, index));

	Router_ip[tb.router_ip_index].refcnt++;

	dpns_nat_wait_rw(priv);

	NAT_DBG(DBG_LV, "===== new IPv4 entry: %s[%u] =====\n",
		(second_slot ? "NAPT1" : "NAPT0"), index);
	dpns_nat_dump_ilkp4_entry(&tb);
	ret = (index << 1) | second_slot;
	return ret;
}

static void dpns_nat_trigger_read_imem(struct dpns_nat_priv *priv, enum se_nat_tb_op_req_id op, u16 idx) {
	sf_writel(priv, SE_NAT_TB_OP, FIELD_PREP(NAT_TB_OP_REQ_ID, op) | FIELD_PREP(NAT_TB_OP_REQ_ADDR, idx));
	dpns_nat_wait_rw(priv);
}

static void dpns_nat_read_ilkp4_entry(struct dpns_nat_priv *priv, int nat_id, nat_ipv4_table *tb) {
	enum se_nat_tb_op_req_id nat_table;

	nat_table = nat_id & 1 ? NAPT1_TABLE : NAPT0_TABLE;

	sf_writel(priv, SE_NAT_TB_OP,
		FIELD_PREP(NAT_TB_OP_REQ_ID, nat_table) |
		FIELD_PREP(NAT_TB_OP_REQ_ADDR, nat_id >> 1));

	dpns_nat_wait_rw(priv);

	tb->data[0] = sf_readq(priv, SE_NAT_TB_RDDATA0);
	tb->data[1] = sf_readq(priv, SE_NAT_TB_RDDATA0 + 8);
	tb->data[2] = sf_readq(priv, SE_NAT_TB_RDDATA0 + 16);
}

void dpns_nat_update_ilkp4_entry(struct dpns_nat_priv *priv, int nat_id, struct nat_ipv4_data *data, bool is_dnat) {
	enum se_nat_tb_op_req_id nat_table;
	nat_ipv4_table tb;

	nat_table = nat_id & 1 ? NAPT1_TABLE : NAPT0_TABLE;
	dpns_nat_read_ilkp4_entry(priv, nat_id, &tb);
	NAT_DBG(DBG_LV, "===== v4 orig: %s[%u] =====\n",
		(nat_id & 1 ? "NAPT1" : "NAPT0"),
		nat_id >> 1);
	dpns_nat_dump_ilkp4_entry(&tb);
	if (is_dnat) {
		tb.doport_id = data->doport_id;
		tb.drtmac_index = data->drtmac_index;
		tb.primac_index = data->primac_index;
	} else {
		tb.soport_id = data->soport_id;
		tb.pubmac_index = data->pubmac_index;
		tb.srtmac_index = data->srtmac_index;
	}

	sf_writeq(priv, SE_NAT_TB_WRDATA0, tb.data[0]);
	sf_writeq(priv, SE_NAT_TB_WRDATA0 + 8, tb.data[1]);
	sf_writeq(priv, SE_NAT_TB_WRDATA0 + 16, tb.data[2]);

	sf_writel(priv, SE_NAT_TB_OP, NAT_TB_OP_WR |
		FIELD_PREP(NAT_TB_OP_REQ_ID, nat_table) |
		FIELD_PREP(NAT_TB_OP_REQ_ADDR, nat_id >> 1));
	NAT_DBG(DBG_LV, "===== v4 updated: %s[%u] =====\n",
		(nat_id & 1 ? "NAPT1" : "NAPT0"),
		nat_id >> 1);
	dpns_nat_dump_ilkp4_entry(&tb);
	dpns_nat_wait_rw(priv);
}

void dpns_nat_free_ilkp4_entry(struct dpns_nat_priv *priv, int nat_id)
{
	u32 op;
	nat_ipv4_table tb;
	MAC_t *mac_priv;
	u16 crc_16;

	mac_priv = priv->cpriv->mac_priv;
	crc_16 = crc16_custom((u8*)&nat_id, sizeof(nat_id), 0);
	/* We need to fetch router IP index. Maybe it can be fetched elsewhere? */
	dpns_nat_read_ilkp4_entry(priv, nat_id, &tb);

	sf_writeq(priv, SE_NAT_TB_WRDATA0, 0);
	sf_writeq(priv, SE_NAT_TB_WRDATA0 + 8, 0);
	sf_writeq(priv, SE_NAT_TB_WRDATA0 + 16, 0);
	op = NAT_TB_OP_WR | FIELD_PREP(NAT_TB_OP_REQ_ADDR, nat_id >> 1);
	if(nat_id & 1) {
		op |= FIELD_PREP(NAT_TB_OP_REQ_ID, NAPT1_TABLE);
		clear_bit(nat_id >> 1, priv->nat1_bitmap);
	} else {
		op |= FIELD_PREP(NAT_TB_OP_REQ_ID, NAPT0_TABLE);
		clear_bit(nat_id >> 1, priv->nat0_bitmap);
	}
	sf_writel(priv, SE_NAT_TB_OP, op);

	if (tb.valid)
		Router_ip[tb.router_ip_index].refcnt--;

	NAT_DBG(DBG_LV, "removed %s[%u]\n",
		nat_id & 1 ? "NAPT1" : "NAPT0",
		nat_id >> 1);

	dpns_nat_wait_rw(priv);
	mac_priv->sf_del_ts_info(mac_priv, NULL, 0, nat_id, crc_16);
}

void dpns_nat_hw_lookup4(struct dpns_nat_priv *priv, bool is_dnat, struct nat_ipv4_data *tb, bool is_offload)
{
	/**
	 * Writing 32-bit little-endian values using writeq will clear
	 * the second 32-bit register.
	 */
	NAT_DBG(DBG_LV, "dpns_nat_hw_lookup4(): is_offload = %u\n", is_offload);
	if (!is_dnat) {
		sf_writeq(priv, SE_NAT_KEY_RAM_DATA(0), tb->public_ip);
		sf_writeq(priv, SE_NAT_KEY_RAM_DATA(2), 0);
		sf_writeq(priv, SE_NAT_KEY_RAM_DATA(4), tb->private_ip);
		sf_writeq(priv, SE_NAT_KEY_RAM_DATA(6), 0);
		sf_writeq(priv, SE_NAT_KEY_RAM_DATA(8), tb->private_port << 16 | tb->public_port);
		if (is_offload)
			sf_writel(priv, SE_NAT_KEY_RAM_DATA(10), tb->l4_type | OFFLOAD_FLAG);
		else
			sf_writel(priv, SE_NAT_KEY_RAM_DATA(10), tb->l4_type);
	} else {
		sf_writeq(priv, SE_NAT_KEY_RAM_DATA(0), tb->router_ip);
		sf_writeq(priv, SE_NAT_KEY_RAM_DATA(2), 0);
		sf_writeq(priv, SE_NAT_KEY_RAM_DATA(4), tb->public_ip);
		sf_writeq(priv, SE_NAT_KEY_RAM_DATA(6), 0);
		sf_writeq(priv, SE_NAT_KEY_RAM_DATA(8), tb->public_port << 16 | tb->router_port);
		sf_writel(priv, SE_NAT_KEY_RAM_DATA(10), tb->l4_type | DNAT_FLAG);
	}

	sf_writel(priv, SE_NAT_LKP_REQ, 0x1);
	dpns_nat_wait_lkp(priv);
}

/*
 * return:
 * -ENOENT entry not found.
 * -EEXIST entry found on external NAPT.
 */
static int dpns_nat_hw_lookup_get_id(struct dpns_nat_priv *priv)
{
	u32 d7 = sf_readl(priv, SE_NAT_RESULT_RAM_DATA(7));
	u32 d6;
	int nat_id;
	if (!(d7 & SE_NAT_RESULT7_HIT)) {
		NAT_DBG(DBG_LV, "lookup_result: not found.\n");
		return -ENOENT;
	}

	d6 = sf_readl(priv, SE_NAT_RESULT_RAM_DATA(6));
	nat_id = FIELD_GET(SE_NAT_RESULT6_NAT_ID, d6);
	NAT_DBG(DBG_LV, "NAT lookup hit Data 6 7 %08X %08X nat_id %d\n",
		d6, d7, nat_id);
	if ((d7 & SE_NAT_RESULT7_HIT_ENAPT) && (nat_id < NPU_HNAT_INAPT_MAXID)) {
		NAT_DBG(ERR_LV, "Found %d in ENAPT. Check ENAPT filling!\n", nat_id);
		return -EINVAL;
	}
	return nat_id;
}

static u16 dpns_nat_read_hash_entry(struct dpns_nat_priv *priv, u16 hash, bool is_hash1, bool is_dnat)
{
	enum se_nat_tb_op_req_id hash_table;
	if(!is_dnat)
		hash_table = is_hash1 ? SNAT_HASH1_TABLE : SNAT_HASH0_TABLE;
	else
		hash_table = is_hash1 ? DNAT_HASH1_TABLE : DNAT_HASH0_TABLE;
	dpns_nat_trigger_read_imem(priv, hash_table, hash);
	dpns_nat_wait_rw(priv);
	return (u16)sf_readl(priv, SE_NAT_TB_RDDATA0);
}

int dpns_nat_insert_hash(struct dpns_nat_priv *priv, bool is_dnat, u16 nat_id, u16 *poly) {
	u16 napt_idx = nat_id >> 1;
	struct hash_position *hash_nat0, *hash_nat1;
	u16 hash_val0, hash_val1;
	bool hash_id0, hash_id1;
	int i;
	if (is_dnat) {
		hash_nat0 = &priv->nat_inapt01_hash[0].dnat;
		hash_nat1 = &priv->nat_inapt01_hash[1].dnat;
	} else {
		hash_nat0 = &priv->nat_inapt01_hash[0].snat;
		hash_nat1 = &priv->nat_inapt01_hash[1].snat;
	}
	hash_val0 = hash_nat0->valid ? hash_nat0->hash : 0xffff;
	hash_val1 = hash_nat1->valid ? hash_nat1->hash : 0xffff;
	hash_id0 = hash_nat0->hash1;
	hash_id1 = hash_nat1->hash1;

	for (i = 0; i < NPU_NAT_SUB_TB; i++) {
		enum se_nat_tb_op_req_id hash_table;
		bool is_hash1 = i >= 3;
		u16 index, hash;

		hash = poly[i] & nat_hash_mask[i];
		hash |= nat_hash_offset[i];


		if (!is_dnat)
			hash_table = is_hash1 ? SNAT_HASH1_TABLE : SNAT_HASH0_TABLE;
		else
			hash_table = is_hash1 ? DNAT_HASH1_TABLE : DNAT_HASH0_TABLE;

		index = dpns_nat_read_hash_entry(priv, hash, is_hash1, is_dnat);

		if ((index != 0) ||
			(hash_id0 == is_hash1 && hash_val0 == hash) ||
			(hash_id1 == is_hash1 && hash_val1 == hash))
			continue; /* entry occupied. */

		sf_writel(priv, SE_NAT_TB_WRDATA0, napt_idx);
		sf_writel(priv, SE_NAT_TB_OP, NAT_TB_OP_WR |
			FIELD_PREP(NAT_TB_OP_REQ_ID, hash_table) |
			FIELD_PREP(NAT_TB_OP_REQ_ADDR, hash));

		if (napt_idx == 0) {
			if(is_dnat) {
				priv->nat_inapt01_hash[nat_id].dnat.valid = true;
				priv->nat_inapt01_hash[nat_id].dnat.hash1 = is_hash1;
				priv->nat_inapt01_hash[nat_id].dnat.hash = hash;
			} else {
				priv->nat_inapt01_hash[nat_id].snat.valid = true;
				priv->nat_inapt01_hash[nat_id].snat.hash1 = is_hash1;
				priv->nat_inapt01_hash[nat_id].snat.hash = hash;
			}
		}

		NAT_DBG(DBG_LV, "New %s: %s[%04X] -> %d\n",
		is_dnat ? "DNAT" : "SNAT",
		is_hash1 ? "Hash1" : "Hash0",
		hash, napt_idx);
		dpns_nat_wait_rw(priv); /* Wait for hash writing */
		/* Here's the hash_index assignment rule: */
		return is_hash1 ? hash + NAT_HASH0_SIZE : hash;
	}

	return -ENOSPC;
}

static int dpns_nat_add_ipv4_int(struct dpns_nat_priv *priv, struct dpns_nat_entry *entry,
				 bool is_dnat, struct nat_ipv4_data *data)
{
	int ret;
	int nat_id;

	dpns_nat_dump_ipv4_data(data);

	if (entry->nat_id == DPNS_NAT_ENTRY_ID_INVALID) {
		/* No existing flow found. Add a new entry. */
		NAT_DBG(DBG_LV, "new v4 entry in internal napt.\n");
		nat_id = dpns_nat_new_ilkp4_entry(priv, data);
		if (nat_id < 0) {
			NAT_DBG(DBG_LV, "new_ilkp4_entry return %d\n", nat_id);
			return nat_id;
		}
	} else {
		/* Found a flow. Update the entry. */
		nat_id = entry->nat_id;
		NAT_DBG(DBG_LV, "Updating entry %d due to INAPT hit.\n", nat_id);
		dpns_nat_update_ilkp4_entry(priv, nat_id, data, is_dnat);
	}

	/* Insert a new hash for the current direction. */
	ret = dpns_nat_insert_hash(priv, is_dnat, nat_id, data->crc16_poly);

	if (ret < 0) {
		/* error due to hash table conflict. */
		if(!test_bit(nat_id >> 1, nat_id & 1 ? priv->nat1_odd_hash : priv->nat0_odd_hash))
			dpns_nat_free_ilkp4_entry(priv, nat_id);

		NAT_DBG(INFO_LV, "error due to hash table conflict %d is_dnat %d\n", nat_id, is_dnat);
		return -EEXIST;
	} else {
		change_bit(nat_id >> 1, nat_id & 1 ? priv->nat1_odd_hash : priv->nat0_odd_hash);
		entry->nat_id = nat_id;
		entry->hash_index = ret;
	}

	return 0;
}

static int dpns_nat_add_ipv4_ext(struct dpns_nat_priv *priv, struct dpns_nat_entry *entry,
				 bool is_dnat, struct nat_ipv4_data *data)
{
	union nat_table_u *t;
	nat_ipv4_ext_table tb = {
		.valid = 1,
		.dummy = -1,
	};
	u32 index, offset;
	int i;
	u8 sub_tb;

	if (entry->nat_id == DPNS_NAT_ENTRY_ID_INVALID || entry->nat_id < NPU_HNAT_INAPT_MAXID) {
		entry->nat_id = find_first_zero_bit(priv->natid_bitmap,
			NPU_HNAT_VISIT_SIZE - NPU_HNAT_INAPT_MAXID) +
			NPU_HNAT_INAPT_MAXID;
		if (entry->nat_id >= NPU_HNAT_VISIT_SIZE) {
			NAT_DBG(DBG_LV, "No NAT ID left for ENAPT.\n");
			return -ENOSPC;
		}
	}

	tb.nat_id = entry->nat_id;

	t = !is_dnat ? priv->snat_table : priv->dnat_table;
	/* for elkp, index is the lowest x bits of crc16,
	* x = log 2 (table entries / sub-tables count),
	* for 4M(64B*64K) size with 8 sub-tables, each sub-table has 8K
	* entries.
	* poly1 (0x8005) is used for even number of sub-tables, and poly0
	* (0x1021) is used for odd number ones.
	*/
	sub_tb = ELKP_SUB_TB(priv->elkp_v4_acs_times);
	offset = ELKP_OFFSET(priv->elkp_size, sub_tb);
	for (i = 0; i < ELKP_SUB_TB(priv->elkp_v4_acs_times); i++) {
		index = i * offset;
		index += ((i % 2) ? data->crc16_poly[0] : data->crc16_poly[1]) & (offset - 1);

		if (t[index].v6.flag)
			continue;

		/* index is vacant */
		if (!t[index].v4[0].valid)
			break;

		if (!t[index].v4[1].valid) {
			entry->second_slot = true;
			break;
		}
	}
	if (i == sub_tb) {
		NAT_DBG(DBG_LV, "No hash slot left in ENAPT for %s %d.\n",
			is_dnat ? "DNAT" : "SNAT", entry->nat_id);
		entry->second_slot = false;
		return -ENOSPC;
	}

	entry->index = index;

	tb.public_ip = data->public_ip;
	tb.public_port = data->public_port;
	tb.private_ip = data->private_ip;
	tb.private_port = data->private_port;
	tb.router_ip = data->router_ip;
	tb.router_port = data->router_port;
	tb.l4_type = data->l4_type;
	tb.primac_index = data->primac_index;
	tb.pubmac_index = data->pubmac_index;
	tb.stat_en = 1;

	if (!is_dnat) {
		tb.rtmac_index = data->srtmac_index;
		tb.oport_id = data->soport_id;
	} else {
		tb.rtmac_index = data->drtmac_index;
		tb.oport_id = data->doport_id;
	}

	memcpy(&t[index].v4[entry->second_slot], &tb, sizeof(tb));

	NAT_DBG(DBG_LV, "New ENAPT entry %s %d inserted. index %d second_slot %d\n",
		is_dnat ? "DNAT" : "SNAT",
		entry->nat_id, entry->index, entry->second_slot);
	/* Mark nat_id as used. (It's ok to do it twice) */
	set_bit(entry->nat_id - NPU_HNAT_INAPT_MAXID, priv->natid_bitmap);
	/* nat_id refcnt += 1 */
	change_bit(entry->nat_id - NPU_HNAT_INAPT_MAXID, priv->natid_odd_entries);

	return 0;
}

static int dpns_nat_del_ipv4_ext(struct dpns_nat_priv *priv, struct dpns_nat_entry *entry,
	u16 nat_id, u16 is_dnat)
{
	int ret = -ENOSPC;
	u16 id_tmp, index_tmp;
	bool slot_tmp;
	union nat_table_u *t;
	struct dpns_nat_entry *entry_r;
	struct rhashtable_iter iter;
	struct nat_ipv4_data data_r = {};

	rhashtable_walk_enter(&priv->flow_table, &iter);
	rhashtable_walk_start(&iter);
	while ((entry_r = rhashtable_walk_next(&iter))) {
		if (IS_ERR(entry_r))
			continue;
		if (entry_r->nat_id != nat_id)
			continue;
		if(entry_r->v6_flag)
			continue;
		if (entry_r->is_dnat != is_dnat)
			continue;
		t = (!entry_r->is_dnat) ? priv->snat_table : priv->dnat_table;

		data_r.public_ip = t[entry_r->index].v4[entry_r->second_slot].public_ip;
		data_r.public_port = t[entry_r->index].v4[entry_r->second_slot].public_port;
		data_r.private_ip = t[entry_r->index].v4[entry_r->second_slot].private_ip;
		data_r.private_port = t[entry_r->index].v4[entry_r->second_slot].private_port;
		data_r.router_ip = t[entry_r->index].v4[entry_r->second_slot].router_ip;
		data_r.router_port = t[entry_r->index].v4[entry_r->second_slot].router_port;
		data_r.l4_type = t[entry_r->index].v4[entry_r->second_slot].l4_type;
		data_r.primac_index = t[entry_r->index].v4[entry_r->second_slot].primac_index;
		data_r.pubmac_index = t[entry_r->index].v4[entry_r->second_slot].pubmac_index;
		data_r.crc16_poly[0] = entry_r->crc16_poly[0];
		data_r.crc16_poly[1] = entry_r->crc16_poly[1];

		if (!entry_r->is_dnat) {
			data_r.srtmac_index = t[entry_r->index].v4[entry_r->second_slot].rtmac_index;
			data_r.soport_id = t[entry_r->index].v4[entry_r->second_slot].oport_id;
		} else {
			data_r.drtmac_index = t[entry_r->index].v4[entry_r->second_slot].rtmac_index;
			data_r.doport_id = t[entry_r->index].v4[entry_r->second_slot].oport_id;
		}

		id_tmp = entry_r->nat_id;
		index_tmp = entry_r->index;
		slot_tmp = entry_r->second_slot;

		entry_r->nat_id = DPNS_NAT_ENTRY_ID_INVALID;
		entry_r->index = 0;
		entry->second_slot = false;
		/* del flow in ENAPT should add it into INAPT because flow is already considered as nat hit */
		ret = dpns_nat_add_ipv4_int(priv, entry_r, entry->is_dnat, &data_r);
		if (ret == 0) {
			/* add INAPT successful and del ENAPT */
			ret = entry_r->nat_id;
			memset(&t[index_tmp].v4[slot_tmp], 0, sizeof(nat_ipv4_ext_table));
			if (test_and_change_bit(id_tmp - NPU_HNAT_INAPT_MAXID, priv->natid_odd_entries))
				clear_bit(id_tmp - NPU_HNAT_INAPT_MAXID, priv->natid_bitmap);

			NAT_DBG(DBG_LV, "del ELKP and add ILKP %d.\n", ret);
		} else {
			/* add INAPT failed so cannot del ENAPT */
			entry_r->nat_id = id_tmp;
			entry_r->index = index_tmp;
			entry->second_slot = slot_tmp;
			NAT_DBG(DBG_LV, "add ILKP %d failed.\n", entry_r->nat_id);
		}
		break;
	}

	rhashtable_walk_stop(&iter);
	rhashtable_walk_exit(&iter);

	return ret;
}

static void dpns_nat_read_ipv4_data(struct dpns_nat_priv *priv, struct dpns_nat_entry *entry, struct nat_ipv4_data *data)
{
	nat_ipv4_table tb;
	int i;
	dpns_nat_read_ilkp4_entry(priv, entry->nat_id, &tb);
	dpns_nat_dump_ilkp4_entry(&tb);

	data->public_ip = tb.public_ip;
	data->public_port = tb.public_port;
	data->private_ip = tb.private_ip;
	data->private_port = tb.private_port;
	data->router_ip = Router_ip[tb.router_ip_index].ip;
	data->router_port = tb.router_port;
	data->l4_type = tb.l4_type;
	for (i = 0; i < NPU_NAT_SUB_TB; i++)
		data->crc16_poly[i] = entry->crc16_poly[i];

	if (!entry->is_dnat) {
		data->pubmac_index = tb.pubmac_index;
		data->srtmac_index = tb.srtmac_index;
		data->soport_id = tb.soport_id;
	} else {
		data->primac_index = tb.primac_index;
		data->drtmac_index = tb.drtmac_index;
		data->doport_id = tb.doport_id;
	}

}

static void dpns_nat_aging_ipv4_int(struct dpns_nat_priv *priv)
{
	int ret = -ENOSPC, count1 = 0, count2 = 0;
	u16 id_tmp;
	u16 index, hash_index, id_ext, second_slot, slot_ext;
	struct dpns_nat_entry *entry;
	struct nat_ipv4_data data_r = {};
	struct rhashtable_iter iter;

	/* ENAPT is no space to add new */
	ret = bitmap_weight(priv->natid_bitmap,
		NPU_HNAT_VISIT_SIZE - NPU_HNAT_INAPT_MAXID);
	if (ret >= NPU_HNAT_VISIT_SIZE - NPU_HNAT_INAPT_MAXID)
		return;

	rhashtable_walk_enter(&priv->flow_table, &iter);
	rhashtable_walk_start(&iter);
	while ((entry = rhashtable_walk_next(&iter))) {
		if (IS_ERR(entry))
			continue;
		if(entry->v6_flag)
			continue;
		if (entry->nat_id >= NPU_HNAT_INAPT_MAXID)
			continue;
		/* hash is 0/1 always can find in INAPT so cannot lookup ENAPT ID */
		if ((entry->nat_id >> 1) == 0)
			continue;
		id_tmp = entry->nat_id;
		hash_index = entry->hash_index;
		second_slot = entry->second_slot;
		dpns_nat_read_ipv4_data(priv, entry, &data_r);
		dpns_nat_hw_lookup4(priv, !entry->is_dnat, &data_r, false);
		ret = dpns_nat_hw_lookup_get_id(priv);
		entry->nat_id = ret >= NPU_HNAT_INAPT_MAXID ? ret : DPNS_NAT_ENTRY_ID_INVALID;
		entry->index = 0;
		entry->second_slot = false;
		ret = dpns_nat_add_ipv4_ext(priv, entry, entry->is_dnat, &data_r);
		memset(&data_r, 0, sizeof(data_r));
		/* add ENAPT failed so cannot del INAPT */
		if (ret != 0) {
			entry->nat_id = id_tmp;
			entry->hash_index = hash_index;
			entry->second_slot = second_slot;
			count1++;
			continue;
		}
		id_ext = entry->nat_id;
		index = entry->index;
		slot_ext = entry->second_slot;
		entry->nat_id = id_tmp;
		entry->hash_index = hash_index;
		entry->second_slot = second_slot;
		dpns_nat_rm_ilkp4_hw(priv, entry);
		entry->second_slot = slot_ext;
		entry->nat_id = id_ext;
		entry->index = index;
		count2++;
		continue;
	}

	NAT_DBG(DBG_LV, "elkp %d aging %d failed %d\n", ret, count2, count1);

	rhashtable_walk_stop(&iter);
	rhashtable_walk_exit(&iter);

}

int dpns_nat_add_napt4(struct dpns_nat_priv *priv, struct dpns_nat_entry *entry,
		       bool is_lf, bool is_dnat, struct nat_ipv4_data *data)
{
	int ret;
	u16 flag = 0;
	/* Make sure there's no duplicated flows first */
	dpns_nat_hw_lookup4(priv, is_dnat, data, false);
	ret = dpns_nat_hw_lookup_get_id(priv);
	entry->nat_id = ret < 0 ? DPNS_NAT_ENTRY_ID_INVALID : ret;
	if (ret == -EINVAL) {
		return ret;
	} else if ((ret >> 1) == 0) {
		/* there may be a spurious hit for nat_id 0/1. */
		struct hash_position pos = is_dnat ?
			priv->nat_inapt01_hash[ret].dnat :
			priv->nat_inapt01_hash[ret].snat;
		if (pos.valid) {
			NAT_DBG(DBG_LV, "refuse to add duplicated IPv4 entry.\n");
			return -EEXIST;
		}
	} else if (ret != -ENOENT) {
		NAT_DBG(DBG_LV, "refuse to add duplicated IPv4 entry.\n");
		return -EEXIST;
	}

	if (!is_lf) {
		/* No duplicated entry in current direction. Do a reversed lookup. */
		dpns_nat_hw_lookup4(priv, !is_dnat, data, false);
		ret = dpns_nat_hw_lookup_get_id(priv);
		entry->nat_id = ret < 0 ? DPNS_NAT_ENTRY_ID_INVALID : ret;

		NAT_DBG(DBG_LV, "Reverse lookup ret: %d\n", ret);
	}

	if (priv->napt_add_mode == FIRST_ILKP) {
		/* napt_add_mode is FIRST_ILKP means insert INAPT first
		and then insert ELKP if insertion failed */
		if (ret < NPU_HNAT_INAPT_MAXID) {
			/* Found a reversed flow in INAPT or nothing found. */
			ret = dpns_nat_add_ipv4_int(priv, entry, is_dnat, data);
			if (ret == 0)
				return 0;
		}
		// found a flow in ENAPT or insert INAPT failed
		ret = dpns_nat_add_ipv4_ext(priv, entry, is_dnat, data);
	} else if (priv->napt_add_mode == FIRST_ELKP) {
		/* napt_add_mode is FIRST_ELKP means insert ENAPT first
		and then insert ILKP if insertion failed */
		if (ret >= NPU_HNAT_INAPT_MAXID || ret < 0) {
			// found a flow in ENAPT or nothing found
			flag = ret >= NPU_HNAT_INAPT_MAXID ? ret : 0;
			ret = dpns_nat_add_ipv4_ext(priv, entry, is_dnat, data);
			if (ret == 0)
				return 0;
			/* ENAPT insertion current flow failed and found a reversed flow in ENAPT
				need to del the reversed in ENAPT and insert into INAPT.
			*/
			if (flag) {
				ret = dpns_nat_del_ipv4_ext(priv, entry, flag, !is_dnat);
				if (ret < 0)
					return ret;
				entry->nat_id = ret;
			} else
				entry->nat_id = DPNS_NAT_ENTRY_ID_INVALID;
		}
		// found a flow in INAPT or insert ENAPT failed
		ret = dpns_nat_add_ipv4_int(priv, entry, is_dnat, data);
	} else if (priv->napt_add_mode == SWAP_DYAM) {
		/* napt_add_mode is SWAP_DYAM means insert INAPT first
		and if failed del an old flow in INAPT and insert the new one */
		if (ret < NPU_HNAT_INAPT_MAXID) {
			ret = dpns_nat_add_ipv4_int(priv, entry, is_dnat, data);
			if (ret == 0)
				return 0;
			if (ret == -ENOSPC) {
				/* INAPT has no space to add */
				dpns_nat_aging_ipv4_int(priv);
				ret = dpns_nat_add_ipv4_int(priv, entry, is_dnat, data);
				if (ret == 0)
					return 0;
			}
		}
		// found a flow in ENAPT or insert INAPT failed
		ret = dpns_nat_add_ipv4_ext(priv, entry, is_dnat, data);
	}

	return ret;
}

void dpns_nat_rm_ihash(struct dpns_nat_priv *priv, struct dpns_nat_entry *entry) {
	enum se_nat_tb_op_req_id hash_table;
	bool is_dnat = entry->is_dnat;
	int nat_id = entry->nat_id;
	struct hash_position hash_pos;

	hash_pos.hash = entry->hash_index;
	hash_pos.hash1 = false;
	if (hash_pos.hash >= NAT_HASH0_SIZE) {
		hash_pos.hash1 = true;
		hash_pos.hash -= NAT_HASH0_SIZE;
	}

	sf_writel(priv, SE_NAT_TB_WRDATA0, 0);
	if (!is_dnat) {
		hash_table = hash_pos.hash1 ? SNAT_HASH1_TABLE : SNAT_HASH0_TABLE;
		sf_writel(priv, SE_NAT_TB_OP, NAT_TB_OP_WR |
				FIELD_PREP(NAT_TB_OP_REQ_ID, hash_table) |
				FIELD_PREP(NAT_TB_OP_REQ_ADDR, hash_pos.hash));
		dpns_nat_wait_rw(priv);

		priv->cpriv->intf_del(priv->cpriv, entry->sintf_index);
		NAT_DBG(DBG_LV, "Dropped SNAT Hash %s[%04X]\n",
			hash_pos.hash1 ? "Hash1" : "Hash0", hash_pos.hash);
	} else {
		hash_table = hash_pos.hash1 ? DNAT_HASH1_TABLE : DNAT_HASH0_TABLE;
		sf_writel(priv, SE_NAT_TB_OP, NAT_TB_OP_WR |
				FIELD_PREP(NAT_TB_OP_REQ_ID, hash_table) |
				FIELD_PREP(NAT_TB_OP_REQ_ADDR, hash_pos.hash));
		dpns_nat_wait_rw(priv);

		priv->cpriv->intf_del(priv->cpriv, entry->dintf_index);
		NAT_DBG(DBG_LV, "Dropped DNAT Hash %s[%04X]\n",
			hash_pos.hash1 ? "Hash1" : "Hash0", hash_pos.hash);
	}

	/* We need to invalidate software stored value for index0 */
	if((nat_id >> 1) == 0) {
		if (is_dnat)
			priv->nat_inapt01_hash[nat_id].dnat.valid = false;
		else
			priv->nat_inapt01_hash[nat_id].snat.valid = false;
	}

	change_bit(nat_id >> 1, nat_id & 1 ? priv->nat1_odd_hash : priv->nat0_odd_hash);
}

void dpns_nat_rm_ilkp4_hw(struct dpns_nat_priv *priv, struct dpns_nat_entry *entry) {
	int nat_id;
	dpns_nat_rm_ihash(priv, entry);
	nat_id = entry->nat_id;

	if (!(test_bit(nat_id >> 1, nat_id & 1 ? priv->nat1_odd_hash : priv->nat0_odd_hash))) {
		dpns_nat_free_ilkp4_entry(priv, entry->nat_id);
	}
}

void dpns_ip6_hton(u32 *dst, u32 *src) {
	*(u128 *)dst = swab128(*(u128 *)src);
}

static void dpns_nat_dump_ilkp6_entry(nat_ipv6_table *tb) {
	u32 pubip[4], privip[4], rtip[4];

	NAT_DBG(DBG_LV, "%016llX %016llX %016llX %016llX\n", tb->data[0], tb->data[1], tb->data[2], tb->data[3]);
	NAT_DBG(DBG_LV, "%016llX %016llX %016llX\n", tb->data[4], tb->data[5], tb->data[6]);

	if(!tb->valid) {
		NAT_DBG(DBG_LV, "invalid.\n");
		return;
	}

	dpns_ip6_hton(pubip, tb->public_ip);
	dpns_ip6_hton(privip, tb->private_ip);
	dpns_ip6_hton(rtip, Router_ip6[tb->router_ip_index - RT_IP6_OFFSET].ip);
	NAT_DBG(DBG_LV, "%s: [%pI6c]:%u > [%u][%pI6c]:%u > [%pI6c]:%u\n",
		tb->l4_type ? "UDP" : "TCP",
		pubip, tb->public_port,
		tb->router_ip_index, rtip, tb->router_port,
		privip, tb->private_port
		);
	NAT_DBG(DBG_LV, "mac_idx: srt: %u drt: %u pri: %u pub: %u\n",
		tb->srtmac_index, tb->drtmac_index, tb->primac_index, tb->pubmac_index);
	NAT_DBG(DBG_LV, "repl_pri(%u): %u stat(%u): %u spl(%u): %u\n",
		tb->repl_pri_en, tb->repl_pri, tb->stat_en, tb->stat_index, tb->spl_en, tb->spl_index
	);
	NAT_DBG(DBG_LV, "oport_id: s: %u d: %u\n",
		tb->soport_id, tb->doport_id);
}

void dpns_nat_dump_ipv6_data(struct nat_ipv6_data *tb) {
	NAT_DBG(DBG_LV, "crc16: 0: %04X 1: %04X 2: %04X 3: %04X 4: %04X 5: %04X 6: %04X 7: %04X\n",
		tb->crc16_poly[0], tb->crc16_poly[1], tb->crc16_poly[2], tb->crc16_poly[3], tb->crc16_poly[4], tb->crc16_poly[5], tb->crc16_poly[6], tb->crc16_poly[7]);
}

/**
 * It's expecting no conflicts in the hash table.
 * Call dpns_nat_wait_rw(priv) after this function.
 */
int dpns_nat_new_ilkp6_entry(struct dpns_nat_priv *priv, struct nat_ipv6_data *data) {
	nat_ipv6_table tb6 = {
		.valid = 1,
		.flag = 1,
	};
	u32 ip6[4];
	int index;
	int bm_block;
	int i, j;

	/**
	 * Find a free slot in the hashtable
	 * Bitmaps are ulong arrays.
	 * Let's assume NAT_ILKP_SZ is a multiple of BITS_PER_LONG.
	 */
	for (bm_block = 0; bm_block < NAT_ILKP_SZ / BITS_PER_LONG ; bm_block++) {
		unsigned long bm0 = priv->nat0_bitmap[bm_block];
		unsigned long bm1 = priv->nat1_bitmap[bm_block];
		unsigned long or_bm;

		or_bm = bm0 | bm1;
		if(!(~or_bm))
			continue;

		index = bm_block * BITS_PER_LONG + __ffs(~bm0);
		break;
	}

	if (bm_block == NAT_ILKP_SZ / BITS_PER_LONG)
		return -ENOSPC;

	/* find if there are existing entry for current router ip */
	for (i = 0 ; i < RT_IP6_NUM_ENTRIES ; i++) {
		if(!memcmp(Router_ip6[i].ip, data->router_ip, 16)) {
			tb6.router_ip_index = i + RT_IP6_OFFSET;
			break;
		}
	}

	if (i == RT_IP6_NUM_ENTRIES) {
		/* Not found. Insert a new one at a free slot. */
		for (i = 0 ; i < RT_IP6_NUM_ENTRIES ; i++) {
			if(!Router_ip6[i].refcnt) {
				sf_writel(priv, SE_NAT_TB_WRDATA(0), data->router_ip[0]);
				sf_writel(priv, SE_NAT_TB_WRDATA(1), data->router_ip[1]);
				sf_writel(priv, SE_NAT_TB_WRDATA(2), data->router_ip[2]);
				sf_writel(priv, SE_NAT_TB_WRDATA(3), data->router_ip[3]);

				sf_writel(priv, SE_NAT_TB_OP, NAT_TB_OP_WR |
				FIELD_PREP(NAT_TB_OP_REQ_ID, NAT_RT_IP_RAM1) |
				FIELD_PREP(NAT_TB_OP_REQ_ADDR, i + RT_IP6_OFFSET));

				tb6.router_ip_index = i + RT_IP6_OFFSET;
				memcpy(Router_ip6[i].ip, data->router_ip, 16);
				dpns_ip6_hton(ip6, Router_ip6[i].ip);
				NAT_DBG(DBG_LV, "new router IP:[%pI6c] at %d + %d\n", ip6, RT_IP6_OFFSET, i);
				dpns_nat_wait_rw(priv);
				break;
			}
		}
	}

	if (i == RT_IP6_NUM_ENTRIES) {
		/* No free slot in Router_ip6. */
		return -ENOSPC;
	}

	set_bit(index, priv->nat1_bitmap);
	set_bit(index, priv->nat0_bitmap);

	/* start writing */
	for (j = 0; j < 4; j++) {
		tb6.public_ip[j] = data->public_ip[j];
		tb6.private_ip[j] = data->private_ip[j];
	}

	tb6.public_port = data->public_port;
	tb6.private_port = data->private_port;
	// tb6.router_ip_index assigned.
	tb6.router_port = data->router_port;
	tb6.l4_type = data->l4_type;
	tb6.srtmac_index = data->srtmac_index;
	tb6.drtmac_index = data->drtmac_index;
	tb6.primac_index = data->primac_index;
	tb6.pubmac_index = data->pubmac_index;
	tb6.soport_id = data->soport_id;
	tb6.doport_id = data->doport_id;

	for (j = 0 ; j < ARRAY_SIZE(tb6.data) ; j++)
		sf_writeq(priv, SE_NAT_TB_WRDATA(2*j), tb6.data[j]);

	sf_writel(priv, SE_NAT_TB_OP, NAT_TB_OP_WR |
		FIELD_PREP(NAT_TB_OP_REQ_ID, NAPT01_TABLE) |
		FIELD_PREP(NAT_TB_OP_REQ_ADDR, index));

	Router_ip6[tb6.router_ip_index - RT_IP6_OFFSET].refcnt++;

	NAT_DBG(DBG_LV, "===== new entry at [%d] =====", index);
	dpns_nat_dump_ilkp6_entry(&tb6);
	dpns_nat_wait_rw(priv);

	return index << 1;
}

static void dpns_nat_read_ilkp6_entry(struct dpns_nat_priv *priv, int nat_id, nat_ipv6_table *tb) {
	int i;

	sf_writel(priv, SE_NAT_TB_OP,
		FIELD_PREP(NAT_TB_OP_REQ_ID, NAPT01_TABLE) |
		FIELD_PREP(NAT_TB_OP_REQ_ADDR, nat_id >> 1));
	dpns_nat_wait_rw(priv);

	for (i = 0 ; i < ARRAY_SIZE(tb->data) ; i++)
		tb->data[i] = sf_readq(priv, SE_NAT_TB_RDDATA(2 * i));
}

void dpns_nat_update_ilkp6_entry(struct dpns_nat_priv *priv, int nat_id, struct nat_ipv6_data *data, bool is_dnat) {
	nat_ipv6_table tb;
	int j;

	dpns_nat_read_ilkp6_entry(priv, nat_id, &tb);

	NAT_DBG(DBG_LV, "===== orig entry[%d] =====", nat_id >> 1);
	dpns_nat_dump_ilkp6_entry(&tb);

	if (is_dnat) {
		tb.doport_id = data->doport_id;
		tb.drtmac_index = data->drtmac_index;
		tb.primac_index = data->primac_index;
	} else {
		tb.soport_id = data->soport_id;
		tb.pubmac_index = data->pubmac_index;
		tb.srtmac_index = data->srtmac_index;
	}

	for (j = 0 ; j < ARRAY_SIZE(tb.data) ; j++)
		sf_writeq(priv, SE_NAT_TB_WRDATA(2 * j), tb.data[j]);

	sf_writel(priv, SE_NAT_TB_OP, NAT_TB_OP_WR |
		FIELD_PREP(NAT_TB_OP_REQ_ID, NAPT01_TABLE) |
		FIELD_PREP(NAT_TB_OP_REQ_ADDR, nat_id >> 1));
	NAT_DBG(DBG_LV, "===== updated entry[%d] =====", nat_id >> 1);
	dpns_nat_dump_ilkp6_entry(&tb);
	dpns_nat_wait_rw(priv);
}

void dpns_nat_free_ilkp6_entry(struct dpns_nat_priv *priv, int nat_id)
{
	int j;
	nat_ipv6_table tb;
	MAC_t *mac_priv;
	u16 crc_16;

	mac_priv = priv->cpriv->mac_priv;
	crc_16 = crc16_custom((u8*)&nat_id, sizeof(nat_id), 0);

	/* We need to fetch router IP index. Maybe it can be fetched elsewhere? */
	dpns_nat_read_ilkp6_entry(priv, nat_id, &tb);

	for (j = 0; j < ARRAY_SIZE(tb.data); j++)
		sf_writeq(priv, SE_NAT_TB_WRDATA(2 * j), 0);

	sf_writel(priv, SE_NAT_TB_OP, NAT_TB_OP_WR |
		FIELD_PREP(NAT_TB_OP_REQ_ID, NAPT01_TABLE) |
		FIELD_PREP(NAT_TB_OP_REQ_ADDR, nat_id >> 1));
	clear_bit(nat_id >> 1, priv->nat1_bitmap);
	clear_bit(nat_id >> 1, priv->nat0_bitmap);

	if (tb.valid)
		Router_ip6[tb.router_ip_index - RT_IP6_OFFSET].refcnt--;

	NAT_DBG(DBG_LV, "removed IP6[%u]\n", nat_id >> 1);

	dpns_nat_wait_rw(priv);
	mac_priv->sf_del_ts_info(mac_priv, NULL, 0, nat_id, crc_16);
}

void dpns_nat_hw_lookup6(struct dpns_nat_priv *priv, bool is_dnat, struct nat_ipv6_data *tb, bool is_offload)
{
	/**
	 * Writing 32-bit little-endian values using writeq will clear
	 * the second 32-bit register.
	 */
	if (!is_dnat) {
		sf_writeq(priv, SE_NAT_KEY_RAM_DATA(0), *(u64 *)&tb->public_ip[0]);
		sf_writeq(priv, SE_NAT_KEY_RAM_DATA(2), *(u64 *)&tb->public_ip[2]);
		sf_writeq(priv, SE_NAT_KEY_RAM_DATA(4), *(u64 *)&tb->private_ip[0]);
		sf_writeq(priv, SE_NAT_KEY_RAM_DATA(6), *(u64 *)&tb->private_ip[2]);
		sf_writeq(priv, SE_NAT_KEY_RAM_DATA(8), tb->private_port << 16 | tb->public_port);
		if (is_offload)
			sf_writel(priv, SE_NAT_KEY_RAM_DATA(10), tb->l4_type | V6_FLAG | OFFLOAD_FLAG);
		else
			sf_writel(priv, SE_NAT_KEY_RAM_DATA(10), tb->l4_type | V6_FLAG);
	} else {
		sf_writeq(priv, SE_NAT_KEY_RAM_DATA(0), *(u64 *)&tb->router_ip[0]);
		sf_writeq(priv, SE_NAT_KEY_RAM_DATA(2), *(u64 *)&tb->router_ip[2]);
		sf_writeq(priv, SE_NAT_KEY_RAM_DATA(4), *(u64 *)&tb->public_ip[0]);
		sf_writeq(priv, SE_NAT_KEY_RAM_DATA(6), *(u64 *)&tb->public_ip[2]);
		sf_writeq(priv, SE_NAT_KEY_RAM_DATA(8), tb->public_port << 16 | tb->router_port);
		sf_writel(priv, SE_NAT_KEY_RAM_DATA(10), tb->l4_type | DNAT_FLAG | V6_FLAG);
	}

	sf_writel(priv, SE_NAT_LKP_REQ, 0x1);
	dpns_nat_wait_lkp(priv);
}

static int dpns_nat_add_ipv6_int(struct dpns_nat_priv *priv, struct dpns_nat_entry *entry,
				 bool is_dnat, struct nat_ipv6_data *data)
{
	int ret;
	int nat_id;
	dpns_nat_dump_ipv6_data(data);

	if (entry->nat_id == DPNS_NAT_ENTRY_ID_INVALID) {
		/* Reversed flow not found. Add a new entry. */
		NAT_DBG(DBG_LV, "new v6 entry in internal napt.\n");
		nat_id = dpns_nat_new_ilkp6_entry(priv, data);
		if (nat_id < 0) {
			NAT_DBG(DBG_LV, "new_ilkp6_entry return %d\n", nat_id);
			return nat_id;
		}
	} else {
		/* Found a flow. Update the entry. */
		nat_id = entry->nat_id;
		NAT_DBG(DBG_LV, "Updating entry %d due to INAPT hit.\n", nat_id);
		dpns_nat_update_ilkp6_entry(priv, nat_id, data, is_dnat);
	}

	/* Insert a new hash for the current direction. */
	ret = dpns_nat_insert_hash(priv, is_dnat, nat_id, data->crc16_poly);

	if (ret < 0) {
		/* error due to hash table conflict. */
		if(!test_bit(nat_id >> 1, priv->nat0_odd_hash))
			dpns_nat_free_ilkp6_entry(priv, nat_id);

		NAT_DBG(INFO_LV, "error due to hash table conflict %d is_dnat %d\n", nat_id, is_dnat);
		return -EEXIST;
	} else {
		change_bit(nat_id >> 1, priv->nat0_odd_hash);
		entry->nat_id = nat_id;
		entry->hash_index = ret;
	}

	return 0;
}

static int dpns_nat_add_ipv6_ext(struct dpns_nat_priv *priv, struct dpns_nat_entry *entry,
			  bool is_dnat, struct nat_ipv6_data *data)
{
	union nat_table_u *t;
	nat_ipv6_ext_table tb6 = {
		.valid = 1,
		.flag = 1,
		.dummy = -1,
	};
	u32 index, offset;
	int i, j;
	u8 sub_tb;

	if(entry->nat_id == DPNS_NAT_ENTRY_ID_INVALID || entry->nat_id < NPU_HNAT_INAPT_MAXID) {
		/* If there's no nat_id found, get a new one. */
		entry->nat_id = find_first_zero_bit(priv->natid_bitmap,
			NPU_HNAT_VISIT_SIZE - NPU_HNAT_INAPT_MAXID) +
			NPU_HNAT_INAPT_MAXID;
		if (entry->nat_id >= NPU_HNAT_VISIT_SIZE) {
			NAT_DBG(DBG_LV, "No NAT ID left for ENAPT.\n");
			return -ENOSPC;
		}
	}

	tb6.nat_id = entry->nat_id;

	t = !is_dnat ? priv->snat_table : priv->dnat_table;
	/* for elkp, index is the lowest x bits of crc16,
	 * x = log 2 (table entries / sub-tables count),
	 * for 4M(64B*64K) size with 8 sub-tables, each sub-table has 8K
	 * entries.
	 * poly1 (0x8005) is used for even number of sub-tables, and poly0
	 * (0x1021) is used for odd number ones.
	 */
	sub_tb = ELKP_SUB_TB(priv->elkp_v6_acs_times);
	offset = ELKP_OFFSET(priv->elkp_size, sub_tb);
	for (i = 0; i < sub_tb; i++) {
		index = i * offset;
		index += ((i % 2) ? data->crc16_poly[0] : data->crc16_poly[1]) & (offset - 1);

		/* index is vacant */
		if (!t[index].v4[0].dummy && !t[index].v6.dummy)
			break;
	}
	if (i == sub_tb) {
		NAT_DBG(DBG_LV, "No hash slot left in ENAPT for %s %d.\n",
			is_dnat ? "DNAT" : "SNAT", entry->nat_id);
		return -ENOSPC;
	}

	for (j = 0; j < 4; j++) {
		tb6.public_ip[j] = data->public_ip[j];
		tb6.private_ip[j] = data->private_ip[j];
		tb6.router_ip[j] = data->router_ip[j];
	}

	if (!is_dnat) {
		tb6.rtmac_index = data->srtmac_index;
		tb6.oport_id = data->soport_id;
	} else {
		tb6.rtmac_index = data->drtmac_index;
		tb6.oport_id = data->doport_id;
	}
	tb6.public_port = data->public_port;
	tb6.private_port = data->private_port;
	tb6.router_port = data->router_port;
	tb6.l4_type = data->l4_type;
	tb6.primac_index = data->primac_index;
	tb6.pubmac_index = data->pubmac_index;

	entry->index = index;
	memcpy(&t[index].v6, &tb6, sizeof(tb6));

	NAT_DBG(DBG_LV, "New ENAPT entry %s %d inserted.\n",
		is_dnat ? "DNAT" : "SNAT",
		entry->nat_id);
	/* Mark nat_id as used. (It's ok to do it twice) */
	set_bit(entry->nat_id - NPU_HNAT_INAPT_MAXID, priv->natid_bitmap);
	/* nat_id refcnt += 1 */
	change_bit(entry->nat_id - NPU_HNAT_INAPT_MAXID, priv->natid_odd_entries);

	return 0;
}

static int dpns_nat_del_ipv6_ext(struct dpns_nat_priv *priv, struct dpns_nat_entry *entry,
	u16 nat_id, u16 is_dnat)
{
	int ret = -ENOSPC, i;
	u16 id_tmp, index_tmp;
	union nat_table_u *t;
	struct dpns_nat_entry *entry_r;
	struct rhashtable_iter iter;
	struct nat_ipv6_data data_r = {};

	rhashtable_walk_enter(&priv->flow_table, &iter);
	rhashtable_walk_start(&iter);
	while ((entry_r = rhashtable_walk_next(&iter))) {
		if (IS_ERR(entry_r))
			continue;
		if (entry_r->nat_id != nat_id)
			continue;
		if(!entry_r->v6_flag)
			continue;
		if (entry_r->is_dnat != is_dnat)
			continue;

		t = (!entry_r->is_dnat) ? priv->snat_table : priv->dnat_table;
		for (i = 0; i < 4; i++) {
			data_r.public_ip[i] = t[entry_r->index].v6.public_ip[i];
			data_r.private_ip[i] = t[entry_r->index].v6.private_ip[i];
			data_r.router_ip[i] = t[entry_r->index].v6.router_ip[i];
		}
		if (!entry_r->is_dnat) {
			data_r.srtmac_index = t[entry_r->index].v6.rtmac_index;
			data_r.soport_id = t[entry_r->index].v6.oport_id;
		} else {
			data_r.drtmac_index = t[entry_r->index].v6.rtmac_index;
			data_r.doport_id = t[entry_r->index].v6.oport_id;
		}
		data_r.public_port = t[entry_r->index].v6.public_port;
		data_r.private_port = t[entry_r->index].v6.private_port;
		data_r.router_port = t[entry_r->index].v6.router_port;
		data_r.l4_type = t[entry_r->index].v6.l4_type;
		data_r.primac_index = t[entry_r->index].v6.primac_index;
		data_r.pubmac_index = t[entry_r->index].v6.pubmac_index;
		data_r.crc16_poly[0] = entry_r->crc16_poly[0];
		data_r.crc16_poly[1] = entry_r->crc16_poly[1];

		id_tmp = entry_r->nat_id;
		index_tmp = entry_r->index;
		entry_r->nat_id = DPNS_NAT_ENTRY_ID_INVALID;
		entry_r->index = 0;
		/* del flow in ENAPT should add it into INAPT because flow is already considered as nat hit */
		ret = dpns_nat_add_ipv6_int(priv, entry_r, entry_r->is_dnat, &data_r);
		if (ret == 0) {
			/* add INAPT successful and del ENAPT */
			ret = entry_r->nat_id;
			memset(&t[index_tmp].v6, 0, sizeof(nat_ipv6_ext_table));
			if (test_and_change_bit(id_tmp - NPU_HNAT_INAPT_MAXID, priv->natid_odd_entries))
				clear_bit(id_tmp - NPU_HNAT_INAPT_MAXID, priv->natid_bitmap);

			NAT_DBG(DBG_LV, "del ELKP and add ILKP %d.\n", ret);
		} else {
			/* add INAPT failed so cannot del ENAPT */
			entry_r->nat_id = id_tmp;
			entry_r->index = index_tmp;
			NAT_DBG(DBG_LV, "add ILKP %d failed\n", entry_r->nat_id);
		}
		break;
	}

	rhashtable_walk_stop(&iter);
	rhashtable_walk_exit(&iter);

	return ret;
}

static void dpns_nat_read_ipv6_data(struct dpns_nat_priv *priv, struct dpns_nat_entry *entry, struct nat_ipv6_data *data)
{
	int i;
	nat_ipv6_table tb6;
	dpns_nat_read_ilkp6_entry(priv, entry->nat_id, &tb6);
	dpns_nat_dump_ilkp6_entry(&tb6);

	for (i = 0; i < 4; i++) {
		data->public_ip[i] = tb6.public_ip[i];
		data->private_ip[i] = tb6.private_ip[i];
		data->router_ip[i] = Router_ip6[tb6.router_ip_index - RT_IP6_OFFSET].ip[i];
	}

	data->public_port = tb6.public_port;
	data->private_port = tb6.private_port;
	data->router_port = tb6.router_port;
	data->l4_type = tb6.l4_type;
	for (i = 0; i < NPU_NAT_SUB_TB; i++)
		data->crc16_poly[i] = entry->crc16_poly[i];

	if (!entry->is_dnat) {
		data->pubmac_index = tb6.pubmac_index;
		data->srtmac_index = tb6.srtmac_index;
		data->soport_id = tb6.soport_id;
	} else {
		data->primac_index = tb6.primac_index;
		data->drtmac_index = tb6.drtmac_index;
		data->doport_id = tb6.doport_id;
	}
}

static void dpns_nat_aging_ipv6_int(struct dpns_nat_priv *priv)
{
	int ret = -ENOSPC, count1 = 0, count2 = 0;
	u16 id_tmp;
	u16 index, hash_index, id_ext, second_slot, slot_ext;
	struct dpns_nat_entry *entry;
	struct nat_ipv6_data data_r = {};
	struct rhashtable_iter iter;

	/* ENAPT is no space to add new */
	ret = bitmap_weight(priv->natid_bitmap,
		NPU_HNAT_VISIT_SIZE - NPU_HNAT_INAPT_MAXID);
	if (ret >= NPU_HNAT_VISIT_SIZE - NPU_HNAT_INAPT_MAXID)
		return;

	rhashtable_walk_enter(&priv->flow_table, &iter);
	rhashtable_walk_start(&iter);
	while ((entry = rhashtable_walk_next(&iter))) {
		if (IS_ERR(entry))
			continue;
		if(!entry->v6_flag)
			continue;
		if (entry->nat_id >= NPU_HNAT_INAPT_MAXID)
			continue;
		/* hash is 0/1 always can find in INAPT so cannot lookup ENAPT ID */
		if ((entry->nat_id >> 1) == 0)
			continue;
		id_tmp = entry->nat_id;
		hash_index = entry->hash_index;
		second_slot = entry->second_slot;
		dpns_nat_read_ipv6_data(priv, entry, &data_r);
		dpns_nat_hw_lookup6(priv, !entry->is_dnat, &data_r, false);
		ret = dpns_nat_hw_lookup_get_id(priv);
		entry->nat_id = (ret >= NPU_HNAT_INAPT_MAXID) ? ret : DPNS_NAT_ENTRY_ID_INVALID;
		entry->index = 0;
		entry->second_slot = false;
		ret = dpns_nat_add_ipv6_ext(priv, entry, entry->is_dnat, &data_r);
		memset(&data_r, 0, sizeof(data_r));
		if (ret != 0) {
			/* add ENAPT failed */
			entry->nat_id = id_tmp;
			entry->hash_index = hash_index;
			entry->second_slot = second_slot;
			count1++;
			continue;
		}
		id_ext = entry->nat_id;
		index = entry->index;
		slot_ext = entry->second_slot;
		entry->nat_id = id_tmp;
		entry->hash_index = hash_index;
		entry->second_slot = second_slot;
		dpns_nat_rm_ilkp6_hw(priv, entry);
		entry->second_slot = slot_ext;
		entry->nat_id = id_ext;
		entry->index = index;
		count2++;
		continue;
	}

	NAT_DBG(DBG_LV, "aging %d failed %d\n",count2, count1);

	rhashtable_walk_stop(&iter);
	rhashtable_walk_exit(&iter);

}

int dpns_nat_add_napt6(struct dpns_nat_priv *priv, struct dpns_nat_entry *entry,
		       bool is_lf, bool is_dnat, struct nat_ipv6_data *data)
{
	int ret;
	u16 flag = 0;
	/* Make sure there's no duplicated flows first */
	dpns_nat_hw_lookup6(priv, is_dnat, data, false);
	ret = dpns_nat_hw_lookup_get_id(priv);
	entry->nat_id = ret < 0 ? DPNS_NAT_ENTRY_ID_INVALID : ret;
	if (ret == -EINVAL) {
		return ret;
	} else if ((ret >> 1) == 0) {
		/* there may be a spurious hit for nat_id 0/1. */
		struct hash_position pos = is_dnat ?
			priv->nat_inapt01_hash[ret].dnat :
			priv->nat_inapt01_hash[ret].snat;
		if (pos.valid) {
			NAT_DBG(DBG_LV, "refuse to add duplicated IPv6 entry\n");
			return -EEXIST;
		}
	} else if (ret != -ENOENT) {
		NAT_DBG(DBG_LV, "refuse to add duplicated IPv6 entry.\n");
		return -EEXIST;
	}

	if (!is_lf) {
		/* No duplicated entry in current direction. Do a reversed lookup. */
		dpns_nat_hw_lookup6(priv, !is_dnat, data, false);
		ret = dpns_nat_hw_lookup_get_id(priv);
		entry->nat_id = ret < 0 ? DPNS_NAT_ENTRY_ID_INVALID : ret;

		NAT_DBG(DBG_LV, "Reverse lookup ret: %d\n", ret);
	}

	if (priv->napt_add_mode == FIRST_ILKP) {
		/* napt_add_mode is FIRST_ILKP means insert INAPT first
		and then insert ELKP if insertion failed */
		if (ret < NPU_HNAT_INAPT_MAXID) {
			/* Found a reversed flow in INAPT or nothing found. */
			ret = dpns_nat_add_ipv6_int(priv, entry, is_dnat, data);
			if (ret == 0)
				return 0;
			/* INAPT insertion failed. Try ENAPT. */
			entry->nat_id = DPNS_NAT_ENTRY_ID_INVALID;
		}
		/* INAPT insertion failed or found a reversed flow in ENAPT */
		ret = dpns_nat_add_ipv6_ext(priv, entry, is_dnat, data);
	} else if (priv->napt_add_mode == 1) {
		/* napt_add_mode is FIRST_ELKP means insert ENAPT first
		and then insert ILKP if insertion failed */
		if (ret >= NPU_HNAT_INAPT_MAXID || ret < 0) {
			/* found a reversed flow in ENAPT or nothing found. */
			flag = ret >= NPU_HNAT_INAPT_MAXID ? ret : 0;
			ret = dpns_nat_add_ipv6_ext(priv, entry, is_dnat, data);
			if (ret == 0)
				return 0;
			/* ENAPT insertion current flow failed and found a reversed flow in ENAPT
				need to del the reversed in ENAPT and insert into INAPT. */
			if (flag) {
				ret = dpns_nat_del_ipv6_ext(priv, entry, flag, !is_dnat);
				if (ret < 0)
					return ret;
				entry->nat_id = ret;
			}
			else
				entry->nat_id = DPNS_NAT_ENTRY_ID_INVALID;
		}
		/* ENAPT insertion failed or found a reversed flow in INAPT */
		ret = dpns_nat_add_ipv6_int(priv, entry, is_dnat, data);
	}  else if (priv->napt_add_mode == SWAP_DYAM) {
		/* napt_add_mode is SWAP_DYAM means insert INAPT first
		and if failed del an old flow in INAPT and insert the new one */
		if (ret < NPU_HNAT_INAPT_MAXID) {
			ret = dpns_nat_add_ipv6_int(priv, entry, is_dnat, data);
			if (ret == 0)
				return 0;
			if (ret == -ENOSPC) {
				/* INAPT has no space to add */
				dpns_nat_aging_ipv6_int(priv);
				ret = dpns_nat_add_ipv6_int(priv, entry, is_dnat, data);
				if (ret == 0)
					return 0;
			}
		}
		/* INAPT insertion failed or found a reversed flow in ENAPT*/
		ret = dpns_nat_add_ipv6_ext(priv, entry, is_dnat, data);
	}
	return ret;
}

void dpns_nat_rm_ilkp6_hw(struct dpns_nat_priv *priv, struct dpns_nat_entry *entry) {
	dpns_nat_rm_ihash(priv, entry);

	if(!(test_bit(entry->nat_id >> 1, priv->nat0_odd_hash))) {
		dpns_nat_free_ilkp6_entry(priv, entry->nat_id);
	}
}

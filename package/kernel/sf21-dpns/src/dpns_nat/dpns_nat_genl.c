#include <linux/kernel.h>
#include <net/genetlink.h>
#include <linux/netfilter.h>
#include <linux/netfilter_ipv4.h>
#include <net/netfilter/nf_flow_table.h>
#include <linux/ppp_defs.h>
#include <linux/timekeeping.h>

#include "sf_genl_msg.h"
#include "dpns_common.h"
#include "dpns_nat_genl.h"
#include "nat.h"
#include "nat_ilkp.h"

#define PREAMBLE 8
#define INTERFRAME_GAP 12

static struct dpns_nat_priv *g_priv;
extern struct dpns_nat_subnet_info sf_lan_subnet[8];
extern struct dpns_nat_subnet_info sf_wan_subnet[8];
extern const struct rhashtable_params dpns_nat_ht_params;


static int dpns_nat_hw_lkp(struct dpns_nat_priv *priv)
{
	int nat_id;

	if (!(sf_readl(priv, SE_NAT_RESULT_RAM_DATA(7)) & SE_NAT_RESULT7_HIT)) {
		printk("lookup_result: not found.\n");
		return -ENOENT;
	}

	nat_id = FIELD_GET(SE_NAT_RESULT6_NAT_ID,
		 sf_readl(priv, SE_NAT_RESULT_RAM_DATA(6)));
	printk("NAT lookup hit, nat_id %d\n", nat_id);
	return nat_id;
}

int dpns_nat_hw_search4(struct dpns_nat_priv *priv, struct nat_genl_msg *msg)
{
	struct nat_ipv4_data *tb;

	tb = kzalloc(sizeof(struct nat_ipv4_data), GFP_KERNEL);
	if (!tb)
		return -ENOMEM;

	tb->l4_type = msg->is_udp;
	tb->public_ip = msg->public_ip[3];
	tb->public_port = msg->public_port;
	tb->private_ip = msg->private_ip[3];
	tb->private_port = msg->private_port;
	tb->router_ip = msg->router_ip[3];
	tb->router_port = msg->router_port;

	dpns_nat_hw_lookup4(priv, msg->is_dnat, tb, msg->offload_en);
	dpns_nat_hw_lkp(priv);
	kfree(tb);
	return 0;
}

int dpns_nat_hw_search6(struct dpns_nat_priv *priv, struct nat_genl_msg *msg)
{
	struct nat_ipv6_data *tb6;
	int i;

	tb6 = kzalloc(sizeof(struct nat_ipv6_data), GFP_KERNEL);
	if (!tb6)
		return -ENOMEM;

	for (i = 0; i < 4; i++) {
		tb6->public_ip[i] = msg->public_ip[i];
		tb6->private_ip[i] = msg->private_ip[i];
		tb6->router_ip[i] = msg->router_ip[i];
	}

	tb6->public_port = msg->public_port;
	tb6->private_port = msg->private_port;
	tb6->router_port = msg->router_port;
	tb6->l4_type = msg->is_udp;

	dpns_nat_hw_lookup6(priv, msg->is_dnat, tb6, msg->offload_en);
	dpns_nat_hw_lkp(priv);
	kfree(tb6);
	return 0;
}

static int dpns_nat_napt_add4(struct dpns_nat_priv *priv, struct nat_genl_msg *msg)
{
	struct nat_ipv4_data *tb;
	struct dpns_nat_entry *entry1, *entry2;
	struct nat_hash_tuple tuple = {};
	unsigned long cookie;
	int nat_id, i;

	cookie = ktime_get_ns();
	entry1 = rhashtable_lookup_fast(&priv->flow_table, &cookie, dpns_nat_ht_params);
	if (entry1)
		return -EEXIST;

	tb = kzalloc(sizeof(struct nat_ipv4_data), GFP_KERNEL);
	if (!tb)
		return -ENOMEM;

	entry1 = kzalloc(sizeof(struct dpns_nat_entry), GFP_KERNEL);
	if (!entry1)
		return -ENOMEM;

	entry1->cookie = cookie;
	entry1->sintf_index = msg->srtmac_index;
	tb->l4_type = msg->is_udp;
	tb->public_ip = msg->public_ip[3];
	tb->public_port = msg->public_port;
	tb->private_ip = msg->private_ip[3];
	tb->private_port = msg->private_port;
	tb->router_ip = msg->router_ip[3];
	tb->router_port = msg->router_port;

	dpns_nat_hw_lookup4(priv, msg->is_dnat, tb, msg->is_lf);
	printk(" public_ip = %pI4h\n", &tb->public_ip);
	printk(" public_port = %u\n", tb->public_port);
	printk(" private_ip = %pI4h\n", &tb->private_ip);
	printk(" private_port = %u\n", tb->private_port);
	printk(" router_ip_index = %pI4h\n", &tb->router_ip);
	printk(" router_port = %u\n", tb->router_port);
	nat_id = dpns_nat_hw_lkp(priv);

	if (nat_id != -ENOENT) {
		printk("napt entry already exists.\n");
		return 0;
	}

	tb->pubmac_index = msg->pubmac_index;
	tb->srtmac_index = msg->srtmac_index;
	tb->soport_id = msg->soport_id;

	tuple.sipv4 = htonl(msg->private_ip[3]);
	tuple.dipv4 = htonl(msg->public_ip[3]);
	tuple.sport = htons(msg->private_port);
	tuple.dport = htons(msg->public_port);
	tuple.l4_type = msg->is_udp;
	NAT_DBG(DBG_LV, "tuple.l4_type = %u\n", tuple.l4_type);
	NAT_DBG(DBG_LV, "tuple.dport = %u\n", tuple.dport);
	NAT_DBG(DBG_LV, "tuple.sport = %u\n", tuple.sport);
	NAT_DBG(DBG_LV, "tuple.dipv4 = %u\n", tuple.dipv4);
	NAT_DBG(DBG_LV, "tuple.sipv4 = %u\n", tuple.sipv4);
	if (msg->is_lf)
		dpns_nat_offload_tuple_set(msg->hnat_mode, &tuple);
	else
		dpns_nat_hnat_tuple_set(msg->hnat_mode, &tuple, false);

	NAT_DBG(DBG_LV, "tuple.l4_type = %u\n", tuple.l4_type);
	NAT_DBG(DBG_LV, "tuple.dport = %u\n", tuple.dport);
	NAT_DBG(DBG_LV, "tuple.sport = %u\n", tuple.sport);
	NAT_DBG(DBG_LV, "tuple.dipv4 = %u\n", tuple.dipv4);
	NAT_DBG(DBG_LV, "tuple.sipv4 = %u\n", tuple.sipv4);
	for (i = 0; i < NPU_NAT_SUB_TB; i++)
		tb->crc16_poly[i] = crc16_custom((u8 *)&tuple, sizeof(tuple), i);
	rhashtable_insert_fast(&priv->flow_table, &entry1->node, dpns_nat_ht_params);
	dpns_nat_add_napt4(priv, entry1, msg->is_lf, 0, tb);

	memset(tb, 0, sizeof(struct nat_ipv4_data));

	if (msg->is_lf) {
		cookie = ktime_get_ns();
		entry2 = rhashtable_lookup_fast(&priv->flow_table, &cookie, dpns_nat_ht_params);
		if (entry2) {
			return -EEXIST;
		}
		entry2 = kzalloc(sizeof(struct dpns_nat_entry), GFP_KERNEL);
		if (!entry2)
			return -ENOMEM;
		entry2->cookie = cookie;
		entry2->sintf_index = msg->drtmac_index;

		tb->l4_type = msg->is_udp;
		tb->public_ip = msg->private_ip[3];
		tb->public_port = msg->private_port;
		tb->private_ip = msg->public_ip[3];
		tb->private_port = msg->public_port;
		tb->router_ip = msg->router_ip[3];
		tb->router_port = msg->router_port;
		tb->pubmac_index = msg->primac_index;
		tb->srtmac_index = msg->drtmac_index;
		tb->soport_id = msg->doport_id;

		tuple.sipv4 = htonl(msg->public_ip[3]);
		tuple.dipv4 = htonl(msg->private_ip[3]);
		tuple.sport = htons(msg->public_port);
		tuple.dport = htons(msg->private_port);
		tuple.l4_type = msg->is_udp;
		dpns_nat_offload_tuple_set(msg->hnat_mode, &tuple);
		for (i = 0; i < NPU_NAT_SUB_TB; i++)
			tb->crc16_poly[i] = crc16_custom((u8 *)&tuple, sizeof(tuple), i);
		rhashtable_insert_fast(&priv->flow_table, &entry2->node,
				dpns_nat_ht_params);
		dpns_nat_add_napt4(priv, entry2, msg->is_lf, 0, tb);
	} else {
		cookie = ktime_get_ns();
		entry2 = rhashtable_lookup_fast(&priv->flow_table, &cookie, dpns_nat_ht_params);
		if (entry2) {
			return -EEXIST;
		}
		entry2 = kzalloc(sizeof(struct dpns_nat_entry), GFP_KERNEL);
		if (!entry2)
			return -ENOMEM;
		entry2->cookie = cookie;
		entry2->is_dnat = true;
		entry2->dintf_index = msg->drtmac_index;

		tb->l4_type = msg->is_udp;
		tb->public_ip = msg->public_ip[3];
		tb->public_port = msg->public_port;
		tb->private_ip = msg->private_ip[3];
		tb->private_port = msg->private_port;
		tb->router_ip = msg->router_ip[3];
		tb->router_port = msg->router_port;
		tb->primac_index = msg->primac_index;
		tb->drtmac_index = msg->drtmac_index;
		tb->doport_id = msg->doport_id;

		tuple.sipv4 = htonl(msg->public_ip[3]);
		tuple.dipv4 = htonl(msg->router_ip[3]);
		tuple.sport = htons(msg->public_port);
		tuple.dport = htons(msg->router_port);
		tuple.l4_type = msg->is_udp;
		dpns_nat_hnat_tuple_set(msg->hnat_mode, &tuple, true);
		for (i = 0; i < NPU_NAT_SUB_TB; i++)
			tb->crc16_poly[i] = crc16_custom((u8 *)&tuple, sizeof(tuple), i);
		rhashtable_insert_fast(&priv->flow_table, &entry2->node,
				dpns_nat_ht_params);
		dpns_nat_add_napt4(priv, entry2, msg->is_lf, 1, tb);
	}

	kfree(tb);
	return 0;
}

static int dpns_nat_napt_add6(struct dpns_nat_priv *priv, struct nat_genl_msg *msg)
{
	struct nat_ipv6_data *tb6;
	struct dpns_nat_entry *entry1, *entry2;
	struct nat_hash_tuple tuple = {};
	unsigned long cookie;
	u32 pubip[4], privip[4], rtip[4];
	int i, nat_id;

	cookie = ktime_get_ns();
	entry1 = rhashtable_lookup_fast(&priv->flow_table, &cookie, dpns_nat_ht_params);
	if (entry1)
		return -EEXIST;

	tb6 = kzalloc(sizeof(struct nat_ipv6_data), GFP_KERNEL);
	if (!tb6)
		return -ENOMEM;

	entry1 = kzalloc(sizeof(struct dpns_nat_entry), GFP_KERNEL);
	if (!entry1)
		return -ENOMEM;

	for (i = 0; i < 4; i++) {
		tb6->public_ip[i] = msg->public_ip[i];
		tb6->private_ip[i] = msg->private_ip[i];
		tb6->router_ip[i] = msg->router_ip[i];
		tuple.sipv6.s6_addr32[i] = htonl(msg->private_ip[3-i]);
		tuple.dipv6.s6_addr32[i] = htonl(msg->public_ip[3-i]);
	}
	entry1->cookie = cookie;
	entry1->v6_flag = true;
	entry1->sintf_index = msg->srtmac_index;
	tb6->public_port = msg->public_port;
	tb6->private_port = msg->private_port;
	tb6->router_port = msg->router_port;
	tb6->l4_type = msg->is_udp;

	dpns_ip6_hton(pubip, (u32 *)&tb6->public_ip);
	dpns_ip6_hton(privip, (u32 *)&tb6->private_ip);
	dpns_ip6_hton(rtip, (u32 *)&tb6->router_ip);

	tuple.sport = htons(msg->private_port);
	tuple.dport = htons(msg->public_port);
	tuple.l4_type = msg->is_udp;

	dpns_nat_hw_lookup6(priv, msg->is_dnat, tb6, msg->is_lf);
	printk(" public_ip = %pI6c\n", pubip);
	printk(" public_port = %u\n", tb6->public_port);
	printk(" private_ip = %pI6c\n", privip);
	printk(" private_port = %u\n", tb6->private_port);
	printk(" router_ip = %pI6c\n", rtip);
	printk(" router_port = %u\n", tb6->router_port);
	nat_id = dpns_nat_hw_lkp(priv);

	if (nat_id != -ENOENT) {
		printk("napt entry already exists.\n");
		return 0;
	}

	tb6->pubmac_index = msg->pubmac_index;
	tb6->srtmac_index = msg->srtmac_index;
	tb6->soport_id = msg->soport_id;

	NAT_DBG(DBG_LV, "tuple.l4_type = %u\n", tuple.l4_type);
	NAT_DBG(DBG_LV, "tuple.dport = %u\n", tuple.dport);
	NAT_DBG(DBG_LV, "tuple.sport = %u\n", tuple.sport);
	NAT_DBG(DBG_LV, "tuple.dipv4 = %pI6c\n", &tuple.dipv4);
	NAT_DBG(DBG_LV, "tuple.sipv4 = %pI6c\n", &tuple.sipv4);
	if (msg->is_lf)
		dpns_nat_offload_tuple_set(msg->hnat_mode, &tuple);
	else
		dpns_nat_hnat_tuple_set(msg->hnat_mode, &tuple, false);
	NAT_DBG(DBG_LV, "tuple.l4_type = %u\n", tuple.l4_type);
	NAT_DBG(DBG_LV, "tuple.dport = %u\n", tuple.dport);
	NAT_DBG(DBG_LV, "tuple.sport = %u\n", tuple.sport);
	NAT_DBG(DBG_LV, "tuple.dipv4 = %pI6c\n", &tuple.dipv4);
	NAT_DBG(DBG_LV, "tuple.sipv4 = %pI6c\n", &tuple.sipv4);

	for (i = 0; i < NPU_NAT_SUB_TB; i++)
		tb6->crc16_poly[i] = crc16_custom((u8 *)&tuple, sizeof(tuple), i);

	rhashtable_insert_fast(&priv->flow_table, &entry1->node,
			dpns_nat_ht_params);

	dpns_nat_add_napt6(priv, entry1, msg->is_lf, 0, tb6);

	memset(tb6, 0, sizeof(struct nat_ipv6_data));

	if (msg->is_lf) {
		cookie = ktime_get_ns();
		entry2 = rhashtable_lookup_fast(&priv->flow_table, &cookie, dpns_nat_ht_params);
		if (entry2) {
			return -EEXIST;
		}
		entry2 = kzalloc(sizeof(struct dpns_nat_entry), GFP_KERNEL);
		if (!entry2)
			return -ENOMEM;
		entry2->cookie = cookie;
		entry2->v6_flag = true;
		entry2->sintf_index = msg->drtmac_index;
		for (i = 0; i < 4; i++) {
			tb6->public_ip[i] = msg->private_ip[i];
			tb6->private_ip[i] = msg->public_ip[i];
			tb6->router_ip[i] = msg->router_ip[i];
			tuple.sipv6.s6_addr32[i] = htonl(msg->public_ip[3-i]);
			tuple.dipv6.s6_addr32[i] = htonl(msg->private_ip[3-i]);
		}

		tb6->public_port = msg->private_port;
		tb6->private_port = msg->public_port;
		tb6->router_port = msg->router_port;
		tb6->l4_type = msg->is_udp;

		tb6->pubmac_index = msg->primac_index;
		tb6->srtmac_index = msg->drtmac_index;
		tb6->soport_id = msg->doport_id;

		tuple.sport = htons(msg->public_port);
		tuple.dport = htons(msg->private_port);
		tuple.l4_type = msg->is_udp;

		NAT_DBG(DBG_LV, "tuple.l4_type = %u\n", tuple.l4_type);
		NAT_DBG(DBG_LV, "tuple.dport = %u\n", tuple.dport);
		NAT_DBG(DBG_LV, "tuple.sport = %u\n", tuple.sport);
		NAT_DBG(DBG_LV, "tuple.dipv4 = %pI6c\n", &tuple.dipv4);
		NAT_DBG(DBG_LV, "tuple.sipv4 = %pI6c\n", &tuple.sipv4);
		dpns_nat_offload_tuple_set(msg->hnat_mode, &tuple);
		NAT_DBG(DBG_LV, "tuple.l4_type = %u\n", tuple.l4_type);
		NAT_DBG(DBG_LV, "tuple.dport = %u\n", tuple.dport);
		NAT_DBG(DBG_LV, "tuple.sport = %u\n", tuple.sport);
		NAT_DBG(DBG_LV, "tuple.dipv4 = %pI6c\n", &tuple.dipv4);
		NAT_DBG(DBG_LV, "tuple.sipv4 = %pI6c\n", &tuple.sipv4);
		for (i = 0; i < NPU_NAT_SUB_TB; i++)
			tb6->crc16_poly[i] = crc16_custom((u8 *)&tuple, sizeof(tuple), i);

		rhashtable_insert_fast(&priv->flow_table, &entry2->node,
				dpns_nat_ht_params);
		dpns_nat_add_napt6(priv, entry2, msg->is_lf, 0, tb6);
	} else {
		cookie = ktime_get_ns();
		entry2 = rhashtable_lookup_fast(&priv->flow_table, &cookie, dpns_nat_ht_params);
		if (entry2) {
			return -EEXIST;
		}
		entry2 = kzalloc(sizeof(struct dpns_nat_entry), GFP_KERNEL);
		if (!entry2)
			return -ENOMEM;
		entry2->cookie = cookie;
		entry2->is_dnat = true;
		entry2->v6_flag = true;
		entry2->dintf_index = msg->drtmac_index;
		for (i = 0; i < 4; i++) {
			tb6->public_ip[i] = msg->public_ip[i];
			tb6->private_ip[i] = msg->private_ip[i];
			tb6->router_ip[i] = msg->router_ip[i];
			tuple.sipv6.s6_addr32[i] = htonl(msg->public_ip[3-i]);
			tuple.dipv6.s6_addr32[i] = htonl(msg->router_ip[3-i]);
		}

		tb6->public_port = msg->public_port;
		tb6->private_port = msg->private_port;
		tb6->router_port = msg->router_port;
		tb6->l4_type = msg->is_udp;

		tb6->primac_index = msg->primac_index;
		tb6->drtmac_index = msg->drtmac_index;
		tb6->doport_id = msg->doport_id;

		tuple.sport = htons(msg->public_port);
		tuple.dport = htons(msg->router_port);
		tuple.l4_type = msg->is_udp;

		dpns_nat_hnat_tuple_set(msg->hnat_mode, &tuple, true);
		for (i = 0; i < NPU_NAT_SUB_TB; i++)
			tb6->crc16_poly[i] = crc16_custom((u8 *)&tuple, sizeof(tuple), i);

		rhashtable_insert_fast(&priv->flow_table, &entry2->node,
				dpns_nat_ht_params);
		dpns_nat_add_napt6(priv, entry2, msg->is_lf, 1, tb6);
	}

	kfree(tb6);
	return 0;
}

int dpns_nat_count(struct dpns_nat_priv *priv)
{
	int ilkp_count __maybe_unused, elkp_count, count;

	mutex_lock(&priv->tbl_lock);
	//ilkp_count = bitmap_weight(priv->nat0_bitmap, NAT_ILKP_SZ) +
	//	     bitmap_weight(priv->nat1_bitmap, NAT_ILKP_SZ);
	elkp_count = bitmap_weight(priv->natid_bitmap,
			NPU_HNAT_VISIT_SIZE - NPU_HNAT_INAPT_MAXID);
	/*
	 * flow_table will count snat + dnat as 2,
	 * but we will only count once.
	 */
	count = atomic_read(&priv->flow_table.nelems) / 2;
	mutex_unlock(&priv->tbl_lock);

	printk("\nELKP count = %d\nTotal count %d\n", elkp_count, count);
	return 0;
}

static void dpns_nat_reset_mode(struct dpns_nat_priv *priv)
{
	sf_update(priv, NPU_NAT_IPV6_MASK_LEN67_HOST_MODE, NPU_V6LF_MODE_SET | NPU_V4LF_MODE_SET |
			NPU_UDP_HNAT_MODE | NPU_TCP_HNAT_MODE, FIELD_PREP(NPU_UDP_HNAT_MODE, NPU_HNAT_MODE_SYMMETRIC) |
			FIELD_PREP(NPU_TCP_HNAT_MODE, NPU_HNAT_MODE_SYMMETRIC));
	sf_update(priv, SE_NAT_CONFIG1, NAT_CONFIG1_V6LF_EN | NAT_CONFIG1_V4LF_MODE | NAT_CONFIG1_TCP_HNAT_MODE |
			NAT_CONFIG1_V4LF_EN, FIELD_PREP(NAT_CONFIG1_TCP_HNAT_MODE, NPU_HNAT_MODE_SYMMETRIC));
	sf_update(priv, SE_NAT_CONFIG0, NAT_CONFIG0_UDP_HNAT_MODE, FIELD_PREP(NAT_CONFIG0_UDP_HNAT_MODE, NPU_HNAT_MODE_SYMMETRIC));
	sf_update(priv, SE_NAT_CONFIG5, NAT_CONFIG5_V6LF_MODE, 0);
}

int dpns_nat_mode_set(struct dpns_nat_priv *priv, struct nat_genl_msg *msg)
{
	if (msg->lf_mode > 8 || msg->hnat_mode > NPU_HNAT_MODE_PORT_RESTRICTED) {
		printk("ERROR mode set, hnat mode should be 0~4, lf mode should be 0~0xf\n");
		return -EINVAL;
	}

	if (msg->is_lf) {
		if (msg->is_v6_mode) {
			sf_update(priv, NPU_NAT_IPV6_MASK_LEN67_HOST_MODE, 0, NPU_V6LF_MODE_SET);
			sf_update(priv, SE_NAT_CONFIG1, 0, NAT_CONFIG1_V6LF_EN);
			sf_update(priv, SE_NAT_CONFIG5, NAT_CONFIG5_V6LF_MODE,
					FIELD_PREP(NAT_CONFIG5_V6LF_MODE, msg->lf_mode));
		} else {
			sf_update(priv, NPU_NAT_IPV6_MASK_LEN67_HOST_MODE, 0,
					NPU_V4LF_MODE_SET);
			sf_update(priv, SE_NAT_CONFIG1, NAT_CONFIG1_V4LF_MODE,
					FIELD_PREP(NAT_CONFIG1_V4LF_MODE, msg->lf_mode) |
					NAT_CONFIG1_V4LF_EN);
		}
	} else {
		if (msg->is_v6_mode) {
			sf_update(priv, NPU_NAT_IPV6_MASK_LEN67_HOST_MODE,
					NPU_V6LF_MODE_SET, 0);
			sf_update(priv, SE_NAT_CONFIG1, NAT_CONFIG1_V6LF_EN, 0);
		} else {
			sf_update(priv, NPU_NAT_IPV6_MASK_LEN67_HOST_MODE,
					NPU_V4LF_MODE_SET, 0);
			/* also clear the enable bit, not only the mode field */
			sf_update(priv, SE_NAT_CONFIG1,
					NAT_CONFIG1_V4LF_MODE | NAT_CONFIG1_V4LF_EN, 0);
		}

		if (msg->is_udp) {
			sf_update(priv, NPU_NAT_IPV6_MASK_LEN67_HOST_MODE,
					NPU_UDP_HNAT_MODE,
					FIELD_PREP(NPU_UDP_HNAT_MODE, msg->hnat_mode));
			sf_update(priv, SE_NAT_CONFIG0,
					NAT_CONFIG0_UDP_HNAT_MODE, msg->hnat_mode);
		} else {
			sf_update(priv, NPU_NAT_IPV6_MASK_LEN67_HOST_MODE,
					NPU_TCP_HNAT_MODE,
					FIELD_PREP(NPU_TCP_HNAT_MODE, msg->hnat_mode));
			sf_update(priv, SE_NAT_CONFIG1,
					NAT_CONFIG1_TCP_HNAT_MODE,
					FIELD_PREP(NAT_CONFIG1_TCP_HNAT_MODE, msg->hnat_mode));
		}
	}

	return 0;
}

static void dpns_nat_offload_en(bool offload_en)
{
	g_priv->nat_offload_en = offload_en;
}

int dpns_nat_subnet_op(struct dpns_nat_priv *priv, struct nat_genl_msg *msg)
{
	int i;

	if (msg->is_get) {
		if (msg->is_lan) {
			for (i = 0; i < 8; i++) {
				printk("ifname:%s IPv4 ipaddr:%pI4 masklen:%u valid:%d\n"
						"         IPv6 ipaddr:%pI6 masklen:%u valid:%d\n",
						sf_lan_subnet[i].ifname,
						&sf_lan_subnet[i].v4.ip, sf_lan_subnet[i].v4.masklen, sf_lan_subnet[i].v4.valid,
						sf_lan_subnet[i].v6.ip, sf_lan_subnet[i].v6.masklen, sf_lan_subnet[i].v6.valid);
			}
		} else {
			for (i = 0; i < 8; i++) {
				printk("ifname:%s IPv4 ipaddr:%pI4 masklen:%u valid:%d\n"
						"         IPv6 ipaddr:%pI6 masklen:%u valid:%d\n",
						sf_wan_subnet[i].ifname,
						&sf_wan_subnet[i].v4.ip, sf_wan_subnet[i].v4.masklen, sf_wan_subnet[i].v4.valid,
						sf_wan_subnet[i].v6.ip, sf_wan_subnet[i].v6.masklen, sf_wan_subnet[i].v6.valid);
			}
		}
	} else {
		if (msg->index > 7)
			return -EINVAL;

		if (msg->is_lan) {
			strscpy(sf_lan_subnet[msg->index].ifname, msg->ifname, IFNAMSIZ);
		} else {
			if (strncmp(sf_wan_subnet[msg->index].ifname, msg->ifname, IFNAMSIZ))
				dpns_nat_wan_clear(msg->index);
			strscpy(sf_wan_subnet[msg->index].ifname, msg->ifname, IFNAMSIZ);
			/* apply addresses that are already configured on the device */
			dpns_nat_wan_sync();
		}
	}
	return 0;
}

static int dpns_nat_napt_add_mode_set(struct dpns_nat_priv *priv, struct nat_genl_msg *msg)
{
	if (msg->napt_add_mode > SWAP_DYAM || msg->napt_add_mode < FIRST_ILKP) {
	/* 0 means add ILKP first, 1 measns add ELKP first, 2 means add ILKP and ELKP both */
		printk("ERROR mode set, mode should be 0~2\n");
		return -EINVAL;
	}
	priv->napt_add_mode = msg->napt_add_mode;
	printk("set mode %d success\n", priv->napt_add_mode);
	return 0;
}


void xgmac_dma_set_ovport(struct xgmac_dma_priv *priv, u8 port)
{
	u32 status;
	regmap_read(priv->ethsys, ETHSYS_MRI_Q_EN, &status);

	if (port == 0)
		status |= 0x3F;
	else
		status = (status | 0x3F) & (~ETHSYS_MRI_OVPORT_TOP_PRIO | port);

	regmap_write(priv->ethsys, ETHSYS_MRI_Q_EN, status);
}

static int dpns_nat_ovport_set(struct dpns_nat_priv *priv, struct nat_genl_msg *msg)
{
	struct xgmac_dma_priv *dma_priv = priv->cpriv->edma_priv;
	int i;
	if (!strncmp(msg->ifname, "none", 4)) {
		dma_priv->ifindex = 0;
		xgmac_dma_set_ovport(dma_priv, 0);
		return 0;
	}
	for (i = 0; i < DPNS_MAX_PORT; i++) {
		if (dma_priv->ndevs[i]) {
			if (!strncmp(dma_priv->ndevs[i]->name, msg->ifname, IFNAMSIZ)) {
				COMMON_t *cpriv = priv->cpriv;
				dpns_port_t *dp_port;
				dp_port = cpriv->port_by_netdev(cpriv, dma_priv->ndevs[i]);
				if (!dp_port)
					return -EINVAL;
				dma_priv->ifindex = dma_priv->ndevs[i]->ifindex;
				xgmac_dma_set_ovport(dma_priv, i);
				return 0;
			}
		}
	}
	return -EINVAL;
}

static int dpns_nat_ovport_get(struct dpns_nat_priv *priv, struct genl_info *info)
{
	struct nat_genl_msg resp;
	struct xgmac_dma_priv *dma_priv = priv->cpriv->edma_priv;
	int ret, i;
	if (dma_priv->ifindex == 0)
		sprintf(resp.ifname, "%s", "none");
	else {
		for (i = 0; i < DPNS_MAX_PORT; i++) {
			if (dma_priv->ndevs[i]) {
				if ((dma_priv->ndevs[i]->ifindex == dma_priv->ifindex)) {
					memcpy(resp.ifname, dma_priv->ndevs[i]->name, IFNAMSIZ);
					ret = sfgenl_msg_reply(info, &resp, sizeof(resp));
				}
			}
		}
	}
	return ret;
}

static int dpns_nat_update6_byid(struct dpns_nat_priv *priv, struct nat_genl_msg *msg)
{
	nat_ipv6_table rd;
	int i;

	if (msg->nat_id < 0) {
		printk("invalid nat_id.\n");
		return 0;
	}

	mutex_lock(&priv->tbl_lock);
	sf_writel(priv, SE_NAT_TB_OP,
		FIELD_PREP(NAT_TB_OP_REQ_ID, NAPT01_TABLE) |
		FIELD_PREP(NAT_TB_OP_REQ_ADDR, msg->nat_id >> 1));
	dpns_nat_wait_rw(priv);

	for (i = 0 ; i < ARRAY_SIZE(rd.data) ; i++)
		rd.data[i] = sf_readq(priv, SE_NAT_TB_RDDATA(2 * i));

	rd.stat_en = msg->stat_en;
	rd.stat_index = msg->stat_index;
	rd.spl_en = msg->spl_en;
	rd.spl_index = msg->spl_index;
	rd.repl_pri = msg->repl_pri;
	rd.repl_pri_en = msg->repl_pri_en;

	rd.srtmac_index = msg->srtmac_index;
	rd.pubmac_index = msg->pubmac_index;
	rd.drtmac_index = msg->drtmac_index;
	rd.primac_index = msg->primac_index;
	rd.soport_id = msg->soport_id;
	rd.doport_id = msg->doport_id;

	for (i = 0 ; i < ARRAY_SIZE(rd.data) ; i++)
		sf_writeq(priv, SE_NAT_TB_WRDATA(2*i), rd.data[i]);

	sf_writel(priv, SE_NAT_TB_OP, NAT_TB_OP_WR |
		FIELD_PREP(NAT_TB_OP_REQ_ID, NAPT01_TABLE) |
		FIELD_PREP(NAT_TB_OP_REQ_ADDR, msg->nat_id >> 1));
	dpns_nat_wait_rw(priv);
	mutex_unlock(&priv->tbl_lock);

	return 0;
}

static int dpns_nat_dump6_byid(struct dpns_nat_priv *priv, struct nat_genl_msg *msg)
{
	nat_ipv6_table rd;
	u32 pubip[4], privip[4];
	int i;

	if (msg->nat_id < 0) {
		printk("invalid nat_id.\n");
		return 0;
	}

	mutex_lock(&priv->tbl_lock);
	sf_writel(priv, SE_NAT_TB_OP,
		FIELD_PREP(NAT_TB_OP_REQ_ID, NAPT01_TABLE) |
		FIELD_PREP(NAT_TB_OP_REQ_ADDR, msg->nat_id >> 1));
	dpns_nat_wait_rw(priv);

	for (i = 0 ; i < ARRAY_SIZE(rd.data) ; i++)
		rd.data[i] = sf_readq(priv, SE_NAT_TB_RDDATA(2 * i));
	mutex_unlock(&priv->tbl_lock);

	dpns_ip6_hton(pubip, (u32 *)&rd.public_ip);
	dpns_ip6_hton(privip, (u32 *)&rd.private_ip);

	printk(" public_ip = %pI6c\n", pubip);
	printk(" public_port = %u\n", rd.public_port);
	printk(" private_ip = %pI6c\n", privip);
	printk(" private_port = %u\n", rd.private_port);
	printk(" router_ip_index = %u\n", rd.router_ip_index);
	printk(" router_port = %u\n", rd.router_port);
	printk(" l4_type = %d\n", rd.l4_type);
	printk(" valid = %d\n", rd.valid);
	printk(" srtmac_index = %d\n", rd.srtmac_index);
	printk(" drtmac_index = %d\n", rd.drtmac_index);
	printk(" primac_index = %d\n", rd.primac_index);
	printk(" pubmac_index = %d\n", rd.pubmac_index);
	printk(" repl_pri_en = %d\n", rd.repl_pri_en);
	printk(" repl_pri = %d\n", rd.repl_pri);
	printk(" stat_en = %d\n", rd.stat_en);
	printk(" stat_index = %d\n", rd.stat_index);
	printk(" spl_en = %d\n", rd.spl_en);
	printk(" spl_index = %d\n", rd.spl_index);
	printk(" soport_id = %d\n", rd.soport_id);
	printk(" doport_id = %d\n", rd.doport_id);
	printk(" flag = %d\n", rd.flag);

	return 0;
}

static int dpns_nat_update4_byid(struct dpns_nat_priv *priv, struct nat_genl_msg *msg)
{
        nat_ipv4_table rd;
	enum se_nat_tb_op_req_id nat_table;

	if (msg->nat_id < 0) {
		printk("invalid nat_id.\n");
		return 0;
	}

	nat_table = msg->nat_id & 1 ? NAPT1_TABLE : NAPT0_TABLE;

	mutex_lock(&priv->tbl_lock);
	sf_writel(priv, SE_NAT_TB_OP,
		FIELD_PREP(NAT_TB_OP_REQ_ID, nat_table) |
		FIELD_PREP(NAT_TB_OP_REQ_ADDR, msg->nat_id >> 1));

	dpns_nat_wait_rw(priv);

	rd.data[0] = sf_readq(priv, SE_NAT_TB_RDDATA0);
	rd.data[1] = sf_readq(priv, SE_NAT_TB_RDDATA(2));
	rd.data[2] = sf_readq(priv, SE_NAT_TB_RDDATA(4));

	rd.stat_en = msg->stat_en;
	rd.stat_index = msg->stat_index;
	rd.spl_en = msg->spl_en;
	rd.spl_index = msg->spl_index;
	rd.repl_pri = msg->repl_pri;
	rd.repl_pri_en = msg->repl_pri_en;

	rd.srtmac_index = msg->srtmac_index;
	rd.pubmac_index = msg->pubmac_index;
	rd.drtmac_index = msg->drtmac_index;
	rd.primac_index = msg->primac_index;
	rd.soport_id = msg->soport_id;
	rd.doport_id = msg->doport_id;

	sf_writeq(priv, SE_NAT_TB_WRDATA0, rd.data[0]);
	sf_writeq(priv, SE_NAT_TB_WRDATA(2), rd.data[1]);
	sf_writeq(priv, SE_NAT_TB_WRDATA(4), rd.data[2]);

	sf_writel(priv, SE_NAT_TB_OP, NAT_TB_OP_WR |
		FIELD_PREP(NAT_TB_OP_REQ_ID, nat_table) |
		FIELD_PREP(NAT_TB_OP_REQ_ADDR, msg->nat_id >> 1));
	dpns_nat_wait_rw(priv);
	mutex_unlock(&priv->tbl_lock);

	return 0;
}

static int dpns_nat_dump4_byid(struct dpns_nat_priv *priv, struct nat_genl_msg *msg)
{
        nat_ipv4_table rd;
	enum se_nat_tb_op_req_id nat_table;

	if (msg->nat_id < 0) {
		printk("invalid nat_id.\n");
		return 0;
	}

	nat_table = msg->nat_id & 1 ? NAPT1_TABLE : NAPT0_TABLE;

	mutex_lock(&priv->tbl_lock);
	sf_writel(priv, SE_NAT_TB_OP,
		FIELD_PREP(NAT_TB_OP_REQ_ID, nat_table) |
		FIELD_PREP(NAT_TB_OP_REQ_ADDR, msg->nat_id >> 1));

	dpns_nat_wait_rw(priv);

	rd.data[0] = sf_readq(priv, SE_NAT_TB_RDDATA0);
	rd.data[1] = sf_readq(priv, SE_NAT_TB_RDDATA(2));
	rd.data[2] = sf_readq(priv, SE_NAT_TB_RDDATA(4));
	mutex_unlock(&priv->tbl_lock);

	printk(" public_ip = %pI4h\n", &rd.public_ip);
	printk(" public_port = %u\n", rd.public_port);
	printk(" private_ip = %pI4h\n", &rd.private_ip);
	printk(" private_port = %u\n", rd.private_port);
	printk(" router_ip_index = %u\n", rd.router_ip_index);
	printk(" router_port = %u\n", rd.router_port);
	printk(" l4_type = %d\n", rd.l4_type);
	printk(" valid = %d\n", rd.valid);
	printk(" srtmac_index = %d\n", rd.srtmac_index);
	printk(" drtmac_index = %d\n", rd.drtmac_index);
	printk(" primac_index = %d\n", rd.primac_index);
	printk(" pubmac_index = %d\n", rd.pubmac_index);
	printk(" repl_pri_en = %d\n", rd.repl_pri_en);
	printk(" repl_pri = %d\n", rd.repl_pri);
	printk(" stat_en = %d\n", rd.stat_en);
	printk(" stat_index = %d\n", rd.stat_index);
	printk(" spl_en = %d\n", rd.spl_en);
	printk(" spl_index = %d\n", rd.spl_index);
	printk(" soport_id = %d\n", rd.soport_id);
	printk(" doport_id = %d\n", rd.doport_id);
	printk(" v6_flag = %d\n", rd.v6_flag);

	return 0;
}

int dpns_nat_spl_set(struct dpns_nat_priv *priv, struct nat_genl_msg *msg)
{
	u32 config;
	int credit;

	config = sf_readl(priv, SE_NAT_CONFIG5);

	if (msg->nat_mib_mode) {
		config &= ~NAT_CONFIG5_MIB_MODE;
		config |= FIELD_PREP(NAT_CONFIG5_MIB_MODE, msg->nat_mib_mode);
	}

	if (msg->is_zero_lmt) {
		config &= ~NAT_CONFIG5_SPL_ZERO_LIMIT;
		config |= FIELD_PREP(NAT_CONFIG5_SPL_ZERO_LIMIT, 1);
	}

	if (msg->spl_mode) {
		config &= ~NAT_CONFIG5_SPL_MODE;
		config |= FIELD_PREP(NAT_CONFIG5_SPL_MODE, 1);
	} else if (!msg->spl_mode) {
		config &= ~NAT_CONFIG5_SPL_MODE;
		config |= FIELD_PREP(NAT_CONFIG5_SPL_MODE, 0);
	}

	switch (msg->spl_cnt_mode) {
	case SPL_PKT:
		config &= ~NAT_CONFIG5_SPL_CNT_MODE;
		config |= FIELD_PREP(NAT_CONFIG5_SPL_CNT_MODE, SPL_PKT);
		if (!msg->pkt_length) {
			NAT_DBG(ERR_LV, "no pkt_length param input\n");
			return 0;
		}
		credit = msg->spl_value;
		break;
	default:
		config &= ~NAT_CONFIG5_SPL_CNT_MODE;
		config |= FIELD_PREP(NAT_CONFIG5_SPL_CNT_MODE, SPL_BYTE);
		credit = msg->spl_value / 512;
		config &= ~NAT_CONFIG5_SPL_MODE;
		config |= FIELD_PREP(NAT_CONFIG5_SPL_MODE, 1);
	}

	config &= ~NAT_CONFIG5_SPL_SOURCE;
	config |= FIELD_PREP(NAT_CONFIG5_SPL_SOURCE, msg->spl_source);

	sf_writel(priv, SE_NAT_CONFIG5, config);

	sf_writel(priv, SE_NAT_TB_WRDATA0, credit);

	if (msg->spl_source == SEPARATE_SNAT_DNAT) {
		if (msg->is_dnat)
			msg->spl_index |= BIT(0);
		else
			msg->spl_index &= ~BIT(0);
	}

	sf_update(priv, SE_NAT_TB_OP, NAT_TB_OP_WR | NAT_TB_OP_REQ_ID | NAT_TB_OP_REQ_ADDR,
			FIELD_PREP(NAT_TB_OP_WR, 1) | FIELD_PREP(NAT_TB_OP_REQ_ID, SPEEDLIMIT_TABLE) |
			FIELD_PREP(NAT_TB_OP_REQ_ADDR, msg->spl_index));

	return 0;
}

static int set_v4_mib_en(struct dpns_nat_priv *priv, int nat_id, bool natmib_en, uint16_t mib_index)
{
	enum se_nat_tb_op_req_id nat_table;
        nat_ipv4_table rd;

	nat_table = nat_id & 1 ? NAPT1_TABLE : NAPT0_TABLE;

	mutex_lock(&priv->tbl_lock);
	sf_writel(priv, SE_NAT_TB_OP,
		FIELD_PREP(NAT_TB_OP_REQ_ID, nat_table) |
		FIELD_PREP(NAT_TB_OP_REQ_ADDR, nat_id >> 1));

	dpns_nat_wait_rw(priv);

	rd.data[0] = sf_readq(priv, SE_NAT_TB_RDDATA0);
	rd.data[1] = sf_readq(priv, SE_NAT_TB_RDDATA(2));
	rd.data[2] = sf_readq(priv, SE_NAT_TB_RDDATA(4));

	if (natmib_en) {
		rd.stat_en = 1;
		rd.stat_index = mib_index;

		sf_writeq(priv, SE_NAT_TB_WRDATA0, rd.data[0]);
		sf_writeq(priv, SE_NAT_TB_WRDATA(2), rd.data[1]);
		sf_writeq(priv, SE_NAT_TB_WRDATA(4), rd.data[2]);

		sf_writel(priv, SE_NAT_TB_OP, NAT_TB_OP_WR |
			FIELD_PREP(NAT_TB_OP_REQ_ID, nat_table) |
			FIELD_PREP(NAT_TB_OP_REQ_ADDR, nat_id >> 1));
		dpns_nat_wait_rw(priv);

	} else {
		rd.stat_en = 0;
		rd.stat_index = 0;

		sf_writeq(priv, SE_NAT_TB_WRDATA0, rd.data[0]);
		sf_writeq(priv, SE_NAT_TB_WRDATA(2), rd.data[1]);
		sf_writeq(priv, SE_NAT_TB_WRDATA(4), rd.data[2]);

		sf_writel(priv, SE_NAT_TB_OP, NAT_TB_OP_WR |
			FIELD_PREP(NAT_TB_OP_REQ_ID, nat_table) |
			FIELD_PREP(NAT_TB_OP_REQ_ADDR, nat_id >> 1));
		dpns_nat_wait_rw(priv);
	}
	mutex_unlock(&priv->tbl_lock);

	return 0;
}

static int set_v6_mib_en(struct dpns_nat_priv *priv, int nat_id, bool natmib_en, uint16_t mib_index)
{
	nat_ipv6_table rd;
	int i;

	mutex_lock(&priv->tbl_lock);
	sf_writel(priv, SE_NAT_TB_OP,
		FIELD_PREP(NAT_TB_OP_REQ_ID, NAPT01_TABLE) |
		FIELD_PREP(NAT_TB_OP_REQ_ADDR, nat_id >> 1));
	dpns_nat_wait_rw(priv);

	for (i = 0 ; i < ARRAY_SIZE(rd.data) ; i++)
		rd.data[i] = sf_readq(priv, SE_NAT_TB_RDDATA(2 * i));

	if (natmib_en) {
		rd.stat_en = 1;
		rd.stat_index = mib_index;

		for (i = 0 ; i < ARRAY_SIZE(rd.data) ; i++)
			sf_writeq(priv, SE_NAT_TB_WRDATA(2*i), rd.data[i]);

		sf_writel(priv, SE_NAT_TB_OP, NAT_TB_OP_WR |
			FIELD_PREP(NAT_TB_OP_REQ_ID, NAPT01_TABLE) |
			FIELD_PREP(NAT_TB_OP_REQ_ADDR, nat_id >> 1));
		dpns_nat_wait_rw(priv);
	} else {
		rd.stat_en = 0;
		rd.stat_index = 0;

		for (i = 0 ; i < ARRAY_SIZE(rd.data) ; i++)
			sf_writeq(priv, SE_NAT_TB_WRDATA(2*i), rd.data[i]);

		sf_writel(priv, SE_NAT_TB_OP, NAT_TB_OP_WR |
			FIELD_PREP(NAT_TB_OP_REQ_ID, NAPT01_TABLE) |
			FIELD_PREP(NAT_TB_OP_REQ_ADDR, nat_id >> 1));
		dpns_nat_wait_rw(priv);
	}
	mutex_unlock(&priv->tbl_lock);

	return 0;
}

int set_natmib_en(struct dpns_nat_priv *priv, struct dpns_natmib_info *info)
{
	struct nat_ipv4_data tb;
	struct nat_ipv6_data tb6;
	int i, *nat_id = info->nat_id;

	sf_update(priv, SE_NAT_CONFIG5, NAT_CONFIG5_MIB_MODE,
		  FIELD_PREP(NAT_CONFIG5_MIB_MODE, info->mib_mode));

	if (info->is_v6) {
		for (i = 0; i < 4; i++) {
			tb6.public_ip[i] = info->public_ip[i];
			tb6.private_ip[i] = info->private_ip[i];
			tb6.router_ip[i] = info->router_ip[i];
		}
		tb6.public_port = info->public_port;
		tb6.private_port = info->private_port;
		tb6.router_port = info->router_port;
		tb6.l4_type = info->is_udp;
		dpns_nat_hw_lookup6(priv, info->is_dnat, &tb6, false);
		*nat_id = dpns_nat_hw_lkp(priv);
		if (*nat_id < 0) {
			printk("lookup_result: nat not found.\n");
			return 0;
		}
		set_v6_mib_en(priv, *nat_id, info->natmib_en, info->mib_index);
	} else {
		tb.l4_type = info->is_udp;
		tb.public_ip = info->public_ip[3];
		tb.public_port = info->public_port;
		tb.private_ip = info->private_ip[3];
		tb.private_port = info->private_port;
		tb.router_ip = info->router_ip[3];
		tb.router_port = info->router_port;

		dpns_nat_hw_lookup4(priv, info->is_dnat, &tb, false);
		*nat_id = dpns_nat_hw_lkp(priv);
		if (*nat_id < 0) {
			printk("lookup_result: nat not found.\n");
			return 0;
		}
		set_v4_mib_en(priv, *nat_id, info->natmib_en, info->mib_index);
	}
	return 0;

}

static int dpns_nat_elkp_delay_get(struct dpns_nat_priv *priv, struct nat_genl_msg *msg)
{
	int i, reg, fbdiv, min_timing_thd, timing_unit;
	u32 max_delay, min_delay, total_delay, total_cycle = 0;
	unsigned long npu_clk_rate, unit;

	npu_clk_rate = clk_get_rate(priv->cpriv->clk);
	if (npu_clk_rate == 0)
		return 0;

	if (npu_clk_rate != 562500000)
		npu_clk_rate = 562500000;

	reg = sf_readl(priv, AXI_TIMING_RCD_CTRL);
	fbdiv = FIELD_GET(AXI_DELAY_FBDIV, reg);
	/* unit ps */
	unit = fbdiv * 1000000000000 / npu_clk_rate;
	printk("AXI_TIMING_RCD_CTRL offset:0x%08x val:0x%08x fbdiv:%d\n", AXI_TIMING_RCD_CTRL, reg, fbdiv);

	reg = sf_readl(priv, AXI_TIMING_RCD_RANGE_CTRL);
	timing_unit = FIELD_GET(TIMING_INTERVAL_UNIT, reg);
	timing_unit = timing_unit * unit / 1000;
	min_timing_thd = FIELD_GET(MIN_TIMING_THRESHOLD, reg);
	min_timing_thd = min_timing_thd * unit / 1000;
	printk("AXI_TIMING_RCD_RANGE_CTRL offset:0x%08x val:0x%08x min_timing_thd:%d ns timing_unit:%d ns\n",
			AXI_TIMING_RCD_RANGE_CTRL, reg, min_timing_thd, timing_unit);

	reg = sf_readl(priv, AXI_RD_TIMING_DELAY(0));
	min_delay = FIELD_GET(AXI_RD_MIN_DELAY, reg);
	max_delay = FIELD_GET(AXI_RD_MAX_DELAY, reg);
	total_delay = sf_readl(priv, AXI_RD_TIMING_DELAY(1));
	printk("read min delay:%ld ns max delay:%ld ns total delay:%ld ns\n",
			min_delay*unit/1000, max_delay*unit/1000, total_delay*unit/1000);

	for (i = 0; i < 7; i++) {
		reg = sf_readl(priv, AXI_RD_TIMING_RCD_RESULT(i));
		total_cycle += reg;
		printk("rd result_%d [%09d - %09d ns] = %d\n", i,
				(i == 0) ? 0 : min_timing_thd + timing_unit*i,
				min_timing_thd + timing_unit*(i+1), reg);
	}
	printk("read average delay:%ld ns\n", total_delay*unit/total_cycle/1000);

	return 0;
}

static int dpns_nat_clean_hash(struct dpns_nat_priv *priv)
{
	struct dpns_nat_entry *entry;
	struct rhashtable_iter iter;

	rhashtable_walk_enter(&priv->flow_table, &iter);
	rhashtable_walk_start(&iter);
	while ((entry = rhashtable_walk_next(&iter))) {
		if (IS_ERR(entry))
			continue;
		rhashtable_walk_stop(&iter);
		mutex_lock(&priv->tbl_lock);
		dpns_nat_rm_ihash(priv, entry);
		mutex_unlock(&priv->tbl_lock);
		rhashtable_walk_start(&iter);
	}
	rhashtable_walk_stop(&iter);
	rhashtable_walk_exit(&iter);

	return 0;
}

static int dpns_nat_clean_entries(struct dpns_nat_priv *priv)
{
	struct dpns_nat_entry *entry;
	struct rhashtable_iter iter;

	rhashtable_walk_enter(&priv->flow_table, &iter);
	rhashtable_walk_start(&iter);
	while ((entry = rhashtable_walk_next(&iter))) {
		if (IS_ERR(entry))
			continue;
		rhashtable_walk_stop(&iter);
		if (entry->v6_flag) {
			mutex_lock(&priv->tbl_lock);
			dpns_nat_free_ilkp6_entry(priv, entry->nat_id);
			mutex_unlock(&priv->tbl_lock);
		} else {
			mutex_lock(&priv->tbl_lock);
			dpns_nat_free_ilkp4_entry(priv, entry->nat_id);
			mutex_unlock(&priv->tbl_lock);
		}
		rhashtable_walk_start(&iter);
		rhashtable_remove_fast(&priv->flow_table, &entry->node,
				dpns_nat_ht_params);
		kfree_rcu(entry, rcu);

	}
	rhashtable_walk_stop(&iter);
	rhashtable_walk_exit(&iter);

	return 0;
}

static int dpns_nat_del_hash_byid(struct dpns_nat_priv *priv, struct nat_genl_msg *msg)
{
	struct dpns_nat_entry *entry;
	struct rhashtable_iter iter;

	rhashtable_walk_enter(&priv->flow_table, &iter);
	rhashtable_walk_start(&iter);
	while ((entry = rhashtable_walk_next(&iter))) {
		if (IS_ERR(entry))
			continue;
		if (entry->nat_id == msg->nat_id){
			rhashtable_walk_stop(&iter);
			mutex_lock(&priv->tbl_lock);
			dpns_nat_rm_ihash(priv, entry);
			mutex_unlock(&priv->tbl_lock);
			rhashtable_walk_start(&iter);
		}
	}
	rhashtable_walk_stop(&iter);
	rhashtable_walk_exit(&iter);
	return 0;
}

static int dpns_nat_del_entry_byid(struct dpns_nat_priv *priv, struct nat_genl_msg *msg)
{
	struct dpns_nat_entry *entry;
	struct rhashtable_iter iter;

	rhashtable_walk_enter(&priv->flow_table, &iter);
	rhashtable_walk_start(&iter);
	while ((entry = rhashtable_walk_next(&iter))) {
		if (IS_ERR(entry))
			continue;
		if (entry->nat_id == msg->nat_id){
			rhashtable_walk_stop(&iter);
			if (entry->v6_flag) {
				mutex_lock(&priv->tbl_lock);
				dpns_nat_free_ilkp6_entry(priv, entry->nat_id);
				mutex_unlock(&priv->tbl_lock);
			} else {
				mutex_lock(&priv->tbl_lock);
				dpns_nat_free_ilkp4_entry(priv, entry->nat_id);
				mutex_unlock(&priv->tbl_lock);
			}
			rhashtable_walk_start(&iter);
			rhashtable_remove_fast(&priv->flow_table, &entry->node, dpns_nat_ht_params);
			kfree_rcu(entry, rcu);
		}
	}
	rhashtable_walk_stop(&iter);
	rhashtable_walk_exit(&iter);
	return 0;
}

static int dpns_nat_add_pppoe_hdr(struct dpns_nat_priv *priv, struct nat_genl_msg *msg)
{
	u64 pppoe_hdr = 0;
	if (msg->is_v6)
		pppoe_hdr = (0x1100ULL << 48) | ((u64)msg->pppoe_sid << 32) | PPP_IPV6;
	else
		pppoe_hdr = (0x1100ULL << 48) | ((u64)msg->pppoe_sid << 32) | PPP_IP;
	priv->cpriv->table_write(priv->cpriv, 16, msg->index, (u32 *)&pppoe_hdr, sizeof(pppoe_hdr));
	return 0;
}

void dpns_nat_set_offload_mode(struct dpns_nat_priv *priv, u8 offload_mode)
{
	g_priv->nat_offload_mode = OFFLOAD_OFF;
	sf_update(priv, SE_NAT_CONFIG5, NAT_CONFIG5_RELAY_MODE_EN, FIELD_PREP(NAT_CONFIG5_RELAY_MODE_EN, 0));
	sf_update(priv, NPU_CFG_MODE_0, CFG_RELAY_MODE, FIELD_PREP(CFG_RELAY_MODE, 0));
	g_priv->nat_offload_mode = offload_mode;
	if (g_priv->nat_offload_mode == RELAY_OFFLOAD) {
		sf_update(priv, SE_NAT_CONFIG5, NAT_CONFIG5_RELAY_MODE_EN, FIELD_PREP(NAT_CONFIG5_RELAY_MODE_EN, 1));
		sf_update(priv, NPU_CFG_MODE_0, CFG_RELAY_MODE, FIELD_PREP(CFG_RELAY_MODE, 1));
	}
}

static int nat_genl_msg_recv(struct genl_info *info, void *buf, size_t buflen)
{
	struct dpns_nat_priv *priv = g_priv;
	struct nat_genl_msg *msg = buf;
	int err = 0;

	if(WARN_ON_ONCE(!priv))
		return -EBUSY;

	switch(msg->method) {
	case NAT_DUMP_NAPT_TB:
		err = dpns_nat_show(priv);
		break;
	case NAT_DUMP_NAPT_COUNT:
		err = dpns_nat_count(priv);
		break;
	case NAT_HW_SEARCH:
		if (msg->is_v6)
			err = dpns_nat_hw_search6(priv, msg);
		else
			err = dpns_nat_hw_search4(priv, msg);
		break;
	case NAT_MODE_SET:
		err = dpns_nat_mode_set(priv, msg);
		break;
	case NAT_MODE_RESET:
		dpns_nat_reset_mode(priv);
		break;
	case NAT_OFFLOAD_EN:
		dpns_nat_offload_en(msg->offload_en);
		break;
	case NAT_SUBNET:
		err = dpns_nat_subnet_op(priv, msg);
		break;
	case NAT_OVPORT_SET:
		err = dpns_nat_ovport_set(priv, msg);
		break;
	case NAT_OVPORT_GET:
		if (!dpns_nat_ovport_get(priv, info))
			return 0;
		err = -EINVAL;
		break;
	case NAT_NAPT_ADD_MODE_SET:
		err = dpns_nat_napt_add_mode_set(priv, msg);
		break;
	case NAT_SPL_SET:
		err = dpns_nat_spl_set(priv, msg);
		break;
	case NAT_NAPT_ADD:
		if (msg->is_v6)
			err = dpns_nat_napt_add6(priv, msg);
		else
			err = dpns_nat_napt_add4(priv, msg);
		break;
	case NAT_DUMP_BYID:
		if (msg->is_v6)
			err = dpns_nat_dump6_byid(priv, msg);
		else
			err = dpns_nat_dump4_byid(priv, msg);
		break;
	case NAT_UPDATE_BYID:
		if (msg->is_v6)
			err = dpns_nat_update6_byid(priv, msg);
		else
			err = dpns_nat_update4_byid(priv, msg);
		break;
	case NAT_ELKP_DELAY:
		err = dpns_nat_elkp_delay_get(priv, msg);
		break;
	case NAT_CLEAN:
		dpns_nat_clean_hash(priv);
		dpns_nat_clean_entries(priv);
		break;
	case NAT_DEL:
		dpns_nat_del_hash_byid(priv, msg);
		dpns_nat_del_entry_byid(priv, msg);
		break;
	case NAT_ADD_PPPHDR:
		dpns_nat_add_pppoe_hdr(priv, msg);
		break;
	case NAT_OFFLOAD_MODE:
		dpns_nat_set_offload_mode(priv, msg->nat_offload_mode);
		break;
	default:
		err = -EINVAL;
		break;
	}

	sfgenl_msg_reply(info, &err, sizeof(err));

	return err;
}

static struct sfgenl_msg_ops nat_genl_msg_ops = {
	.msg_recv = nat_genl_msg_recv,
};

int dpns_nat_genl_init(struct dpns_nat_priv *priv)
{
	g_priv = priv;
	return sfgenl_ops_register(SF_GENL_COMP_NAT, &nat_genl_msg_ops);
}

int dpns_nat_genl_exit(void)
{
	return sfgenl_msg_ops_unregister(SF_GENL_COMP_NAT);
}

/*
* Description
*
* Copyright (C) 2016-2022 Qin.Xia <qin.xia@siflower.com.cn>
*
* Siflower software
*/

#include <linux/ppp_defs.h>
#include "io.h"
#include "ops.h"

#include <net/nexthop.h>
#include <net/arp.h>
#include <net/fib_rules.h>
#include <net/fib_notifier.h>
#include <linux/io-64-nonatomic-lo-hi.h>
#include <generated/utsrelease.h>
#include <linux/inetdevice.h>

#include "dpns_common.h"
#include "sfxgmac-ext.h"

static intf_entry sf_intf_entry[INTF_TABLE_MAX] = {0};
static tcam_block sf_tcam_block[TCAM_BLK_CFG_MAX] = {0};

struct dpns_nat_subnet_info sf_lan_subnet[8] = {0};
EXPORT_SYMBOL(sf_lan_subnet);
struct dpns_nat_subnet_info sf_wan_subnet[8] = {0};
EXPORT_SYMBOL(sf_wan_subnet);

struct dpns_mem *g_mem[DPNS_CNT_M] = {0};
DEFINE_SPINLOCK(intf_refcount_lock);

static char *dpns_module2name(u8 module)
{
	char *name = "ERROR";
	switch(module){
	case DPNS_COMMON_M:		name = "COMMON"; break;
	case DPNS_L2_M:			name = "L2"; break;
	case DPNS_VLAN_M:		name = "VLAN"; break;
	case DPNS_NAT_M:		name = "NAT"; break;
	case DPNS_L3_M:			name = "L3"; break;
	case DPNS_MCAST_M:		name = "MCAST"; break;
	case DPNS_ACL_M:		name = "ACL"; break;
	case DPNS_TMU_M:		name = "TMU"; break;
	default:
		COMMON_DBG(ERR_LV,"Invalid module number %d.\n", module);
		break;
	}

	return name;
}

int dpns_mem_alloc_init(u8 module)
{
	g_mem[module] = (struct dpns_mem *)kzalloc(sizeof(struct dpns_mem), GFP_KERNEL);
	if (!g_mem[module]) {
		COMMON_DBG(ERR_LV, "%s mem alloc init failed\n", dpns_module2name(module));
		return -ENOMEM;
	}

	INIT_LIST_HEAD(&g_mem[module]->list);
	spin_lock_init(&g_mem[module]->lock);
	return 0;
}

void dpns_mem_alloc_deinit(u8 module)
{
	if (unlikely(!list_empty(&g_mem[module]->list)))
		COMMON_DBG(ERR_LV, "detect module %s mem leak\n", dpns_module2name(module));
	kfree(g_mem[module]);
	g_mem[module] = NULL;
}

void * dpns_kmalloc(size_t size, gfp_t flag, u8 module)
{
	struct dpns_mem_info *memory;

	memory = kmalloc(size + sizeof(struct dpns_mem_info), flag);
	if (!memory)
		return NULL;

	memory->size = size;
	spin_lock_bh(&g_mem[module]->lock);
	list_add_tail(&memory->list, &g_mem[module]->list);
	g_mem[module]->total += size;
	spin_unlock_bh(&g_mem[module]->lock);
	return memory->buf;
}
EXPORT_SYMBOL(dpns_kmalloc);

void dpns_kfree(const void *data, u8 module)
{
	struct dpns_mem_info *m1, *m2;
	bool found = false;

	spin_lock_bh(&g_mem[module]->lock);
	list_for_each_entry_safe(m1, m2, &g_mem[module]->list, list)
	{
		if (m1->buf == data)
		{
			list_del(&m1->list);
			g_mem[module]->total -= m1->size;
			kfree(m1);
			found = true;
			break;
		}
	}
	spin_unlock_bh(&g_mem[module]->lock);

	if (!found) {
		COMMON_DBG(ERR_LV, "%s: can not find %px in memory list, still free it\n",
				dpns_module2name(module), data);
		kfree(data);
	}
}
EXPORT_SYMBOL(dpns_kfree);

void dump_dpns_mem_info(void)
{
	int i;
	for (i = 0; i < DPNS_CNT_M; i++) {
		spin_lock_bh(&g_mem[i]->lock);
		printk("module:%s\t\tmem_size: %lu\n",
			dpns_module2name(i), g_mem[i]->total);
		spin_unlock_bh(&g_mem[i]->lock);
	}
}

void se_reg_set_wait(COMMON_t *priv, u32 reg, u32 val,
			 u32 waitfor, u32 timeout)
{
	sf_writel(priv, reg, val);
	while(timeout--) {
		if(sf_readl(priv, reg) == waitfor)
			break;
		udelay(1000);
	}
}

void se_wait_busy(COMMON_t *priv, u32 reg, u32 mask)
{
	unsigned long timeout = jiffies + HZ;

	do {
		if (!(sf_readl(priv, reg) & mask))
			return;

		udelay(100); // spin_lock hold;
	} while (time_after(timeout, jiffies));

	COMMON_DBG(ERR_LV, "timed out\n");
}

int dpns_table_access(COMMON_t* priv, int opcode, u8 ram_id, u16 ram_addr,
		u32 *data, u32 size)
{
	int i;
	u32 access;
	u32 count = size/sizeof(u32);

	access = FIELD_PREP(IRAM_OPT_WR, opcode) |
		 FIELD_PREP(IRAM_OPT_ID, ram_id) |
		 FIELD_PREP(IRAM_OPT_REQ_ADDR, ram_addr);

	spin_lock_bh(&priv->hw_lock);
	if(opcode == SE_OPT_W) {
		for(i=0; i<count; i++) {
			sf_writel(priv, IRAM_W_ADDR(i), data[i]);
		}

		sf_writel(priv, SE_IRAM_OPT_ADDR, access);
		se_wait_busy(priv, SE_IRAM_OPT_ADDR, IRAM_OPT_BUSY);
	} else {
		sf_writel(priv, SE_IRAM_OPT_ADDR, access);
		se_wait_busy(priv, SE_IRAM_OPT_ADDR, IRAM_OPT_BUSY);

		for(i=0; i<count; i++) {
			data[i] = sf_readl(priv, IRAM_R_ADDR(i));
		}
	}
	spin_unlock_bh(&priv->hw_lock);

	return 0;
}

int dpns_table_read(COMMON_t *priv,
	 u8 ram_id, u16 table_addr, u32* data, u32 size)
{
	return dpns_table_access(priv, SE_OPT_R, ram_id, table_addr, data, size);
}

int dpns_table_write(COMMON_t *priv,
	 u8 ram_id, u16 table_addr, u32* data, u32 size)
{
	return dpns_table_access(priv, SE_OPT_W, ram_id, table_addr, data, size);
}

void dpns_intf_table_del_entry(COMMON_t *priv, u32 index)
{
	union arp_intf_table_cfg param = {0};
	dpns_table_write(priv, ARP_SE_INTF_DIRECT_TABLE, index, param.data, sizeof(param));
}

void dpns_intf_table_del(COMMON_t *priv, u32 index)
{
	spin_lock(&intf_refcount_lock);
	if (sf_intf_entry[index].valid == 1) {
		if (sf_intf_entry[index].ref_count > 0)
			sf_intf_entry[index].ref_count--;

		if (sf_intf_entry[index].ref_count == 0) {
			dpns_intf_table_del_entry(priv, index);
			sf_intf_entry[index].valid = 0;
		}
	} else {
		COMMON_DBG(ERR_LV, "[dpns error] can not find entry[%d] in intf table\n", index);
	}
	spin_unlock(&intf_refcount_lock);
}

int dpns_intf_table_add(COMMON_t *priv, int vid, bool pppoe_en, bool tunnel_en,
				bool wan_flag, u8 *smac)
{
	union arp_intf_table_cfg param = {0};
	int i = 0, tmp_index = -1;

	if (is_zero_ether_addr(smac))
		return -EINVAL;

	param.table.valid      = 1;
	param.table.ovid       = vid;
	param.table.pppoe_en   = pppoe_en;
	param.table.tunnel_en  = tunnel_en;
	param.table.wan_flag   = wan_flag;
	param.table.smac       = ether_addr_to_u64(smac);

	spin_lock(&intf_refcount_lock);
	for (i = 0; i < INTF_TABLE_MAX; i++) {
		if (sf_intf_entry[i].valid == 1) {
			if (!memcmp((u32*)&sf_intf_entry[i].data,
				    (u32*)&param.table,
				    sizeof(struct arp_intf_table))) {
					sf_intf_entry[i].ref_count ++;
					goto find;
			}
		} else {
			if (tmp_index < 0)
				tmp_index = i;
		}
	}

	if (i == INTF_TABLE_MAX) {
		if (tmp_index < 0) {
			COMMON_DBG(DBG_LV, "[dpns error] can not find a slot in intf table\n");
			spin_unlock(&intf_refcount_lock);
			return -EFAULT;
		} else {
			COMMON_DBG(DBG_LV, "intf add mac:%pM vid:%d pppoe_en:%u tunnel_en:%u wan_flag:%u index:%d\n",
				   smac, vid, pppoe_en, tunnel_en, wan_flag, i);
			sf_intf_entry[tmp_index].valid = 1;
			sf_intf_entry[tmp_index].data.valid = 1;
			sf_intf_entry[tmp_index].data.ovid = vid;
			sf_intf_entry[tmp_index].data.pppoe_en = pppoe_en;
			sf_intf_entry[tmp_index].data.tunnel_en = tunnel_en;
			sf_intf_entry[tmp_index].data.wan_flag = wan_flag;
			sf_intf_entry[tmp_index].data.smac = param.table.smac;
			sf_intf_entry[tmp_index].ref_count++;
			dpns_table_write(priv, ARP_SE_INTF_DIRECT_TABLE, tmp_index, param.data, sizeof(param));
			spin_unlock(&intf_refcount_lock);
			return tmp_index;
		}

	}
find:
	spin_unlock(&intf_refcount_lock);
	return i;
}
EXPORT_SYMBOL(dpns_intf_table_add);

void dump_intf_table(COMMON_t *priv)
{
	union arp_intf_table_cfg param;
	u8 mac[ETH_ALEN];
	int i = 0;

	spin_lock(&intf_refcount_lock);
	for (; i < INTF_TABLE_MAX; i++) {
		if (sf_intf_entry[i].valid == 1) {
			memset(&param, 0, sizeof(param));
			memset(mac, 0, sizeof(mac));
			dpns_table_read(priv, ARP_SE_INTF_DIRECT_TABLE, i, param.data, sizeof(param));
			u64_to_ether_addr(param.table.smac, mac);
			printk("\ndump intf index:%d ref_count:%d \
					\nvalid:%d ovid:%d smac:%pM \
					\npppoe_en:%d wan_flag:%d \
					\ntunnel_en:%d\n",
					i, sf_intf_entry[i].ref_count,
					param.table.valid, param.table.ovid, mac,
					param.table.pppoe_en, param.table.wan_flag,
					param.table.tunnel_en);
		}
	}
	spin_unlock(&intf_refcount_lock);
}
EXPORT_SYMBOL(dump_intf_table);

int dpns_tcam_access(COMMON_t *priv, int opcode, u8 req_id,
		u8 req_addr, void *data, u32 size)
{
	int i;
	int count = size / TCAM_SLICE_SIZE;

	WARN_ONCE(size % TCAM_SLICE_SIZE, "size %u is not multiple of %d\n",
			size, TCAM_SLICE_SIZE);

	if(opcode == SE_OPT_W) {
		for(i=0; i<count; i++, data += TCAM_SLICE_SIZE) {
			u32 access;

			sf_writel(priv, TCAM_W_ADDR(0), get_unaligned((u32 *)data));
			sf_writel(priv, TCAM_W_ADDR(1), get_unaligned((u32 *)(data + 4)));
			sf_writel(priv, TCAM_W_ADDR(2), *(u8 *)(data + 8));
			access = FIELD_PREP(TCAM_OPT_ID, req_id + i) |
				FIELD_PREP(TCAM_OPT_REQ_ADDR, req_addr) | TCAM_OPT_WR;
			sf_writel(priv, SE_TCAM_OPT_ADDR, access);

			se_wait_busy(priv, SE_TCAM_OPT_ADDR, TCAM_OPT_BUSY);
		}
	} else {
		for(i=0; i<count; i++, data += TCAM_SLICE_SIZE) {
			u32 access = FIELD_PREP(TCAM_OPT_ID, req_id + i) |
					FIELD_PREP(TCAM_OPT_REQ_ADDR, req_addr);

			sf_writel(priv, SE_TCAM_OPT_ADDR, access);
			se_wait_busy(priv, SE_TCAM_OPT_ADDR, TCAM_OPT_BUSY);

			put_unaligned(sf_readl(priv, TCAM_R_ADDR(0)), (u32 *)data);
			put_unaligned(sf_readl(priv, TCAM_R_ADDR(1)), (u32 *)(data + 4));
			*(u8 *)(data + 8) = sf_readl(priv, TCAM_R_ADDR(2));
		}
	}

	return 0;
}

void dpns_tcam_update(COMMON_t *priv, u8 block_id, u8 req_id, u8 req_addr,
		void *data, void *mask, u32 size, u8 tbid_and_kmd)
{
	u32 access = TCAM_OPT_WR |
		FIELD_PREP(TCAM_OPT_ID, TCAM_BLK_MODE_ID(block_id)) |
		FIELD_PREP(TCAM_OPT_REQ_ADDR, req_addr);

	dpns_tcam_access(priv, SE_OPT_W, req_id, 2*req_addr, data, size);
	dpns_tcam_access(priv, SE_OPT_W, req_id, (2*req_addr+1), mask, size);

	sf_writel(priv, TCAM_W_ADDR(0), tbid_and_kmd);
	sf_writel(priv, SE_TCAM_OPT_ADDR, access);

	se_wait_busy(priv, SE_TCAM_OPT_ADDR, TCAM_OPT_BUSY);
}

void dpns_tcam_clean(COMMON_t *priv, u8 block_id)
{
	COMMON_DBG(DBG_LV, "clear tcam:%d\n", block_id);
	sf_writel(priv, SE_TCAM_CLR, (1 << block_id));
	while (sf_readl(priv, SE_TCAM_CLR) != 0);
	memset(&sf_tcam_block[block_id], 0, sizeof(tcam_block));
}

static const char * const mib_pkt_strings[] = {
	"pkt_rcv_drop_port0",
	"pkt_rcv_drop_port1",
	"pkt_rcv_drop_port2",
	"pkt_rcv_drop_port3",
	"pkt_rcv_drop_port4",
	"pkt_rcv_drop_port5",
	"pkt_rcv_drop_port6",
	"pkt_rcv_drop_spl0",
	"pkt_rcv_drop_spl1",
	"pkt_rcv_drop_spl2",
	"pkt_rcv_drop_spl3",
	"pkt_rcv_drop_spl4",
	"pkt_rcv_drop_spl5",
	"pkt_rcv_drop_spl6",
	"pkt_rcv_trans_cnt0",
	"pkt_rcv_trans_cnt1",
	"pkt_rcv_trans_cnt2",
	"pkt_rcv_trans_cnt3",
	"pkt_rcv_trans_cnt4",
	"pkt_rcv_trans_cnt5",
	"pkt_rcv_trans_cnt6",
	"pkt_rcv_total0",
	"pkt_rcv_total1",
	"pkt_rcv_total2",
	"pkt_rcv_total3",
	"pkt_rcv_total4",
	"pkt_rcv_total5",
	"pkt_rcv_total6",
	"pkt_rcv",
	"udp",
	"tcp",
	"ipv4",
	"ipv6",
	"icmpv4",
	"icmpv6",
	"other_protocol",
	"ipv4_sip_eq_dip",
	"ipv4_icmp_frag",
	"ipv4_icmp_ping_too_big",
	"ipv4_udp_sp_eq_dp",
	"ipv4_tcp_flagchk_err",
	"ipv4_tcp_sq_eq_dp",
	"ipv4_tcp_frag_off1",
	"ipv4_tcp_syn_err",
	"ipv4_tcp_xmas",
	"ipv4_tcp_null",
	"ipv4_tcp_too_short",
	"ipv4_icmp4_redirect",
	"ipv4_icmp_smurf",
	"ipv6_sip_eq_dip",
	"ipv6_icmp_frag",
	"ipv6_icmp_ping_too_big",
	"ipv6_udp_sp_eq_dp",
	"ipv6_tcp_flagchk_err",
	"ipv6_tcp_sq_eq_dp",
	"ipv6_tcp_frag_off1",
	"ipv6_tcp_syn_err",
	"ipv6_tcp_xmas",
	"ipv6_tcp_null",
	"ipv6_tcp_too_short",
	"ipv6_icmp4_redirect",
	"ipv6_icmp_smurf",
	"ipv4in6_pls",
	"frame_ismc_pls",
	"reserved",
	"reserved",
	"reserved",
	"reserved",
	"arp_reply_err_fwd",
	"arp_req_err_fwd",
	"pkt_len_less_l2hd_err_fwd",
	"pkt_len_less_60B_err_fwd",
	"smac_is_mc_err_fwd",
	"smac_is_bc_err_fwd",
	"smac_eq_dmac_err_fwd",
	"smac_eq_zero_err_fwd",
	"dmac_eq_zero_err_fwd",
	"dribble_err_fwd",
	"runt_err_fwd",
	"giant_frame_err_fwd",
	"watchdog_err_fwd",
	"gmii_err_fwd",
	"dos_err_fwd",
	"ttl_err_fwd",
	"payload_chksum_err_fwd",
	"ip_version_err_fwd",
	"ip_hd_chksum_err_fwd",
	"crc_err_fwd",
	"pkt_len_err_fwd",
	"arp_reply_err_up",
	"arp_req_err_up",
	"pkt_len_less_l2hd_err_up",
	"pkt_len_less_60B_err_up",
	"smac_is_mc_err_up",
	"smac_is_bc_err_up",
	"smac_eq_dmac_err_up",
	"smac_eq_zero_err_up",
	"dmac_eq_zero_err_up",
	"dribble_err_up",
	"runt_err_up",
	"giant_frame_err_up",
	"watchdog_err_up",
	"gmii_err_up",
	"dos_err_up",
	"ttl_err_up",
	"payload_chksum_err_up",
	"ip_version_err_up",
	"ip_hd_chksum_err_up",
	"crc_err_up",
	"pkt_len_err_up",
	"arp_reply_err_drop",
	"arp_req_err_drop",
	"pkt_len_less_l2hd_err_drop",
	"pkt_len_less_60B_err_drop",
	"smac_is_mc_err_drop",
	"smac_is_bc_err_drop",
	"smac_eq_dmac_err_drop",
	"smac_eq_zero_err_drop",
	"dmac_eq_zero_err_drop",
	"dribble_err_drop",
	"runt_err_drop",
	"giant_frame_err_drop",
	"watchdog_err_drop",
	"gmii_err_drop",
	"dos_err_drop",
	"ttl_err_drop",
	"payload_chksum_err_drop",
	"ip_version_err_drop",
	"ip_hd_chksum_err_drop",
	"crc_err_drop",
	"pkt_len_err_drop",
	"ivlan_vid_input_mf",
	"ivlan_vid_pass_mf",
	"ivlan_vid_port_based_srch",
	"ivlan_vid_xlt_srch",
	"ivlan_vid_vfp_srch",
	"ivlan_vid_port_based_resp",
	"ivlan_vid_xlt_resp",
	"ivlan_vid_vfp_resp",
	"ivlan_vid_port_based_hit",
	"ivlan_vid_xlt_hit",
	"ivlan_vid_vfp_hit",
	"ivlan_vid_output_mf",
	"ivlan_vid_port_based_pass",
	"ivlan_vid_cp_drop",
	"ivlan_vid_cp_up",
	"ivlan_lkp_input_mf",
	"ivlan_lkp_pass_mf",
	"ivlan_lkp_srch",
	"ivlan_lkp_resp",
	"ivlan_lkp_hit",
	"ivlan_lkp_output_mf",
	"ivlan_lkp_cp_drop",
	"ivlan_lkp_cp_up",
	"l2_input_mf",
	"l2_pass_mf",
	"l2_flood_spl_srch_cnt",
	"l2_da_srch",
	"l2_sa_srch",
	"l2_flood_spl_resp_cnt",
	"l2_da_resp",
	"l2_sa_resp",
	"l2_flood_spl_cnt",
	"l2_da_hit",
	"l2_sa_hit",
	"l2_output_mf",
	"l2_cp_drop",
	"l2_cp_up",
	"l2_cp_fwd",
	"l2_cp_up_fwd",
	"nat_input_mf",
	"nat_pass_mf",
	"nat_srch",
	"nat_resp",
	"nat_hit",
	"nat_output_mf",
	"nat_v4_search",
	"nat_dnat",
	"nat_v4_hit",
	"nat_dnat_hit",
	"l3_input_mf",
	"l3_pass_mf",
	"l3_uc_srch",
	"l3_mcsg_srch",
	"l3_uc_resp",
	"l3_mcsg_resp",
	"l3_uc_hit",
	"l3_mcsg_hit",
	"l3_output_mf",
	"l3_v6_mf",
	"l3_mc",
	"l3_v6_srch",
	"l3_mc_srch",
	"l3_v6_hit",
	"l3_mc_hit",
	"iacl_input_mf",
	"iacl_pass_mf",
	"iacl_srch",
	"iacl_resp",
	"iacl_hit",
	"iacl_output_mf",
	"iacl_v6",
	"iacl_v6_srch",
	"iacl_v6_hit",
	"tmu_port0_phy_tran",
	"tmu_port1_phy_tran",
	"tmu_port2_phy_tran",
	"tmu_port3_phy_tran",
	"tmu_port4_phy_tran",
	"tmu_port5_phy_tran",
	"tmu_port6_phy_tran",
	"tmu_port7_phy_tran",
	"tmu_port8_phy_tran",
	"tmu_port9_phy_tran",
	"tmu_port10_phy_tran",
	"tmu_port11_phy_tran",
	"tmu_port12_phy_tran",
	"tmu_port13_phy_tran",
	"tmu_port14_phy_tran",
	"tmu_port15_phy_tran",
	"tmu_port16_phy_tran",
	"tmu_port17_phy_tran",
	"tmu_port18_phy_tran",
	"tmu_port19_phy_tran",
	"tmu_port20_phy_tran",
	"tmu_port21_phy_tran",
	"tmu_port22_phy_tran",
	"tmu_port23_phy_tran",
	"tmu_port24_phy_tran",
	"tmu_port25_phy_tran",
	"tmu_port26_phy_tran",
	"tmu_port0_phy_drop_rclm",
	"tmu_port1_phy_drop_rclm",
	"tmu_port2_phy_drop_rclm",
	"tmu_port3_phy_drop_rclm",
	"tmu_port4_phy_drop_rclm",
	"tmu_port5_phy_drop_rclm",
	"tmu_port6_phy_drop_rclm",
	"tmu_port7_phy_drop_rclm",
	"tmu_port8_phy_drop_rclm",
	"tmu_port9_phy_drop_rclm",
	"tmu_port10_phy_drop_rclm",
	"tmu_port11_phy_drop_rclm",
	"tmu_port12_phy_drop_rclm",
	"tmu_port13_phy_drop_rclm",
	"tmu_port14_phy_drop_rclm",
	"tmu_port15_phy_drop_rclm",
	"tmu_port16_phy_drop_rclm",
	"tmu_port17_phy_drop_rclm",
	"tmu_port18_phy_drop_rclm",
	"tmu_port19_phy_drop_rclm",
	"tmu_port20_phy_drop_rclm",
	"tmu_port21_phy_drop_rclm",
	"tmu_port22_phy_drop_rclm",
	"tmu_port23_phy_drop_rclm",
	"tmu_port24_phy_drop_rclm",
	"tmu_port25_phy_drop_rclm",
	"tmu_port26_phy_drop_rclm",
	"tmu_drop_bit_cnt",
	"nat_cp_drop_cnt",
	"nat_cp_up_cnt",
	"nat_fwd_cnt",
	"nat_cp_fwd_cnt",
	"l3_cp_up_fwd_cnt",
	"l3_cp_fwd_cnt",
	"l3_cp_up_cnt",
	"l3_drop_bit_cnt",
	"reserved",
	"reserved",
	"reserved",
	"reserved",
	"reserved",
	"reserved",
	"reserved",
	"reserved",
	"reserved",
	"reserved",
	"reserved",
	"reserved",
	"reserved",
	"reserved",
	"reserved",
	"reserved",
	"reserved",
	"reserved",
	"arp_intf_input_mf",
	"arp_intf_pass_mf",
	"arp_intf_intf_srch",
	"arp_intf_arp_srch",
	"arp_intf_intf_resp",
	"arp_intf_arp_resp",
	"arp_intf_intf_hit",
	"arp_intf_arp_hit",
	"arp_intf_output_mf",
	"arp_intf_v6_mf",
	"arp_intf_mc",
	"arp_intf_v6_srch",
	"arp_intf_mc_srch",
	"arp_intf_v6_hit",
	"arp_intf_mc_hit",
	"evlan_lkp_input_mf",
	"evlan_lkp_pass_mf",
	"evlan_lkp_port_tpid_srch",
	"evlan_lkp_tag_mem_srch",
	"evlan_lkp_vlan_srch",
	"evlan_lkp_port_tpid_resp",
	"evlan_lkp_tag_mem_resp",
	"evlan_lkp_vlan_resp",
	"evlan_lkp_port_tpid_hit",
	"evlan_lkp_tag_mem_hit",
	"evlan_lkp_vlan_hit",
	"evlan_lkp_output_mf",
	"evlan_lkp_cp_drop",
	"evlan_lkp_cp_up",
	"evlan_lkp_cp_fwd",
	"evlan_act_input_mf",
	"evlan_act_pass_mf",
	"evlan_act_srch",
	"evlan_xlt_srch_cnt",
	"evlan_act_resp",
	"evlan_xlt_resp_hit",
	"reserved",
	"evlan_xlt_hit_cnt",
	"evlan_act_output_mf",
	"evlan_act_cp_drop",
	"evlan_act_cp_cpu",
	"eacl_input_mf",
	"eacl_pass_mf",
	"eacl_srch",
	"eacl_resp",
	"eacl_hit",
	"eacl_output_mf",
	"eacl_v6",
	"eacl_v6_srch",
	"eacl_v6_hit",
	"md2port_0_data_sof",
	"md2port_0_data_eof",
	"md2port_1_data_sof",
	"md2port_1_data_eof",
	"md2port_2_data_sof",
	"md2port_2_data_eof",
	"md2port_3_data_sof",
	"md2port_3_data_eof",
	"md2port_4_data_sof",
	"md2port_4_data_eof",
	"md2port_5_data_sof",
	"md2port_5_data_eof",
	"md2port_6_data_sof",
	"md2port_6_data_eof",
	"pkt_separate_free_cnt",
	"pkt_whold_free_cnt",
	"se2md_result_cnt",
	"md2se_key_cnt",
	"mem2md_data_cnt",
	"md2mem_rd_cnt",
	"modify_drop_cnt",
	"mipp_cnt[0]",
	"mipp_cnt[1]",
	"ipv6_hdr_add",
	"ipv6_hdr_del",
	"otpid_replace",
	"itpid_replace",
	"ppp_hdr_add",
	"ppp_hdr_del",
	"avlan_replace",
	"avlan_add",
	"avlan_del",
	"ovlan_replace",
	"ovlan_add",
	"ovlan_del",
	"ivlan_replace",
	"ivlan_add",
	"ivlan_del",
};

void dpns_read_npu_mib(COMMON_t *priv)
{
	u64 tmp_addr = NPU_MIB_BASE_ADDR + NPU_MIB_PKT_RCV_PORT;
	u64 nci_rd_data2 = NPU_MIB_BASE_ADDR + NPU_MIB_NCI_RD_DATA2;
	u64 nci_rd_data3 = NPU_MIB_BASE_ADDR + NPU_MIB_NCI_RD_DATA3;
	unsigned int i;
	u32 count;
	u64 bytes;

	printk("dpns npu mib dump:\n");
	for (i = 0; i < ARRAY_SIZE(mib_pkt_strings); i++) {
		count = sf_readl(priv, NPU_MIB_BASE_ADDR + NPU_MIB_OFFSET * i);
		if (count != 0)
			printk("name:%-30s packets:%11u\n", mib_pkt_strings[i], count);
	}

	for (i = 0; i < DPNS_MAX_PORT; i++) {
		count = sf_readl(priv, tmp_addr + NPU_MIB_OFFSET * i);
		bytes = sf_readl(priv, nci_rd_data2) | (u64)sf_readl(priv, nci_rd_data3) << 32;
		if (count != 0 && bytes != 0)
			printk("name:pkt_rcv_port%-18u packets:%11u bytes:%20llu\n", i, count, bytes);
	}
}

static int dpa_port_set_learning(dpns_port_t *dp_port,
			u32 flags)
{
	if(!!(flags & BR_LEARNING))
		return -EINVAL;

	/** note:
	 * 1. DPA_CTRL_DFLT_BRIDGING, redirect package to CPU,
	 * 2. CPU bridge fdb leaninig, sync by dpa_port_fdb_learn();
	 */
	return 0;
}

static int dpns_port_init(COMMON_t *priv, dpns_port_t *dp_port)
{
	if(dp_port->port_id >= EXTDEV_OFFSET)
		return 0;

	dpa_port_set_learning(dp_port, !!(dp_port->brport_flags & BR_LEARNING));

	return 0;
}

static void dpns_port_fini(dpns_port_t *dp_port)
{
	return;
}

/********************
 * swdev interface
 ********************/

static int dpns_register_extdev(struct xgmac_dma_priv *dma_priv,
				struct net_device *dev)
{
	int i;

	for (i = EXTDEV_OFFSET; i < ARRAY_SIZE(dma_priv->ndevs); i++) {

		if (!dma_priv->ndevs[i]) {
			dma_priv->ndevs[i] = dev;
			return i;
		} else if (dma_priv->ndevs[i] == dev) {
			return -EEXIST;
		}
	}

	return -ENOSPC;
}

static void dpns_unregister_extdev(struct xgmac_dma_priv *dma_priv,
				   const struct net_device *dev)
{
	int i;

	for (i = EXTDEV_OFFSET; i < ARRAY_SIZE(dma_priv->ndevs); i++) {
		if (dma_priv->ndevs[i] == dev) {
			dma_priv->ndevs[i] = NULL;
			break;
		}
	}
}

static bool dpns_is_extdev(struct xgmac_dma_priv *dma_priv,
			   const struct net_device *dev)
{
	int i;

	for (i = EXTDEV_OFFSET; i < ARRAY_SIZE(dma_priv->ndevs); i++) {
		if (dma_priv->ndevs[i] == dev)
			return true;
	}

	return false;
}

dpns_port_t *dpns_port_by_netdev(COMMON_t *priv, const struct net_device* dev)
{
	const struct net_device_ops *net_ops = dev->netdev_ops;
	struct xgmac_dma_priv *dma_priv = priv->edma_priv;
	struct net_device *real_dev = (struct net_device*)dev;
	int i;

	if (is_vlan_dev(dev)) {
		real_dev = vlan_dev_real_dev(dev);
		net_ops = real_dev->netdev_ops;
	}

	for (i = EXTDEV_OFFSET; i < ARRAY_SIZE(dma_priv->ndevs); i++) {
		if (dma_priv->ndevs[i] == real_dev)
			return priv->ports[i];
	}

	if(!(net_ops && net_ops->ndo_get_phys_port_id))
		return NULL;

	return dev_dp_port_fetch(real_dev);
}

int dpns_port_id_by_netdev(COMMON_t *priv, const struct net_device *dev, u8 *port_id)
{
	dpns_port_t *port;

	if (!dev)
		return -EINVAL;

	port = dpns_port_by_netdev(priv, dev);
	if (!port)
		return -ENOENT;

	if (!port_id)
		return -EINVAL;

	*port_id = port->port_id;

	return 0;
}

int dpns_port_hwaddr_by_netdev(COMMON_t *priv, const struct net_device *dev, u8 *hwaddr)
{
	dpns_port_t *port;

	if (!dev)
		return -EINVAL;

	port = dpns_port_by_netdev(priv, dev);
	if (!port)
		return -ENOENT;

	if (!dev->dev_addr || !hwaddr)
		return -EINVAL;

	memcpy(hwaddr, dev->dev_addr, ETH_ALEN);

	return 0;
}
EXPORT_SYMBOL(dpns_port_hwaddr_by_netdev);

struct dpns_port_vlan_info* sf_search_vlan_info(dpns_port_t *dp_port, u16 vlan_id)
{
	struct dpns_port_vlan_info *pos;

	list_for_each_entry(pos, &dp_port->vlan_list, node) {
		if (pos->vlan_id == vlan_id && pos->port_id == dp_port->port_id) {
			return pos;
		}
	}

	return NULL;
}

static void dpns_port_vlan_add(dpns_port_t *dp_port, struct net_device *dev, u16 vlan_id)
{
	struct dpns_port_vlan_info *info;

	spin_lock_bh(&dp_port->lock);
	info = sf_search_vlan_info(dp_port, vlan_id);
	if (info) {
		info->dev = dev;
		spin_unlock_bh(&dp_port->lock);
		return;
	}
	spin_unlock_bh(&dp_port->lock);

	info = common_kzalloc(sizeof(struct dpns_port_vlan_info), GFP_KERNEL);
	if (!info)
		return;

	info->dev = dev;
	info->port_id = dp_port->port_id;
	info->vlan_id = vlan_id;
	spin_lock_bh(&dp_port->lock);
	list_add_tail(&info->node, &dp_port->vlan_list);
	spin_unlock_bh(&dp_port->lock);
}

static void dpns_port_vlan_del(dpns_port_t *dp_port, struct net_device *dev)
{
	struct dpns_port_vlan_info *info;
	u16 vlan_id;

	if (!is_vlan_dev(dev))
		return;

	vlan_id = vlan_dev_vlan_id(dev);

	spin_lock_bh(&dp_port->lock);
	info = sf_search_vlan_info(dp_port, vlan_id);
	if (!info) {
		spin_unlock_bh(&dp_port->lock);
		return;
	}

	list_del(&info->node);
	spin_unlock_bh(&dp_port->lock);

	common_kfree(info);
}

static void dpns_port_remove(COMMON_t *priv, dpns_port_t *dp_port,
		struct net_device *dev)
{
	dp_port->ref_count--;
	dpns_port_fini(dp_port);
	dpns_unregister_extdev(priv->edma_priv, dp_port->dev);
	dpns_port_vlan_del(dp_port, dev);
	dev_put(dev);

	if (dp_port->ref_count == 0) {
		priv->ports[dp_port->port_id] = NULL;
		common_kfree(dp_port);
	}
}

static void dpns_destroy_port_vlanlist(dpns_port_t *dp_port)
{
	struct dpns_port_vlan_info *pos, *tmp;

	spin_lock_bh(&dp_port->lock);
	list_for_each_entry_safe(pos, tmp, &dp_port->vlan_list, node) {
			list_del(&pos->node);
			common_kfree(pos);
	}
	spin_unlock_bh(&dp_port->lock);
}

void dpns_destroy_portsarray(COMMON_t *priv)
{
	dpns_port_t *dp_port;
	int i;

	for( i = 0; i < priv->port_count; i++) {
		if(priv->ports[i] != NULL) {
			dp_port = priv->ports[i];
			if (dp_port->port_id >= EXTDEV_OFFSET)
				dpns_unregister_extdev(priv->edma_priv, dp_port->dev);
			dpns_destroy_port_vlanlist(dp_port);
			common_kfree(dp_port);
		}
	}
}

static int dpns_port_probe(COMMON_t *priv, struct net_device *dev)
{
	const struct net_device_ops *net_ops = dev->netdev_ops;
	struct net_device *real_dev = dev;
	dpns_port_t *dp_port;
	int port_id, err;
	u16 vlan_id = DPA_UNTAGGED_VID;

	if (is_vlan_dev(dev)) {
		real_dev = vlan_dev_real_dev(dev);
		net_ops = real_dev->netdev_ops;
		vlan_id = vlan_dev_vlan_id(dev);
	}

	if (real_dev->flags & (IFF_LOOPBACK | IFF_POINTOPOINT))
		return 0;

	if (real_dev->priv_flags & (IFF_EBRIDGE | IFF_ISATAP | IFF_MACVLAN))
		return 0;

	if(!net_ops->ndo_get_phys_port_id) {
		port_id = dpns_register_extdev(priv->edma_priv, dev);
		if (port_id < 0)
			return 0;

	} else {
		struct netdev_phys_item_id ppid = {};

		net_ops->ndo_get_phys_port_id(real_dev, &ppid);
		for (port_id = 0; port_id < priv->port_count; port_id++) {
			if(ppid.id[0] == port_id)
				break;
		}
	}

	if (port_id == priv->port_count)
		return -EINVAL;

	if (vlan_id != DPA_UNTAGGED_VID) {
		dp_port = dpns_port_by_netdev(priv, real_dev);
		if (!dp_port)
			return -EINVAL;

		dev_hold(dev);
		dp_port->ref_count++;
		dpns_port_vlan_add(dp_port, dev, vlan_id);
		return 0;
	}

	dp_port = common_kzalloc(sizeof(dpns_port_t), GFP_KERNEL);
	if(!dp_port) {
		err = -ENOMEM;
		goto err_put_netdev;
	}

	dev_hold(dev);
	dp_port->ref_count++;
	dp_port->dev = dev;
	dp_port->port_id = port_id;
	dp_port->brport_flags = BR_LEARNING;
	INIT_LIST_HEAD(&dp_port->vlan_list);
	spin_lock_init(&dp_port->lock);

#ifdef CONFIG_DPNS_THROUGHPUT_WIFI_BEST
	/* dynamic config eth port buf cnt */
	if (port_id < DPNS_HOST_PORT) {
		if (port_id == 4)
			sf_writel(priv, PORT_CNT_NUM(dp_port->port_id), 0x258);
		else
			sf_writel(priv, PORT_CNT_NUM(dp_port->port_id), 0x12c);
	}
#endif

	priv->ports[port_id] = dp_port;
	if (port_id < EXTDEV_OFFSET)
		dev_dp_port_store(dev, dp_port);

	err = dpns_port_init(priv, dp_port);
	if (err < 0) {
		COMMON_DBG(ERR_LV, "port port %d init failed\n", port_id);
		goto err_free_dp;
	}

	return 0;

err_free_dp:
	common_kfree(dp_port);
err_put_netdev:
	dev_put(dev);
	if (port_id >= EXTDEV_OFFSET) {
		dpns_unregister_extdev(priv->edma_priv, dev);
	}
	return err;
}

bool dpns_port_dev_check(COMMON_t *priv, struct net_device *dev)
{
	struct netdev_phys_item_id ppid;
	struct net_device *real_dev = dev;
	const struct net_device_ops *net_ops = dev->netdev_ops;

	if (dpns_is_extdev(priv->edma_priv, dev))
		return true;
	/** next TODO: pppoe, vlan, ... support; */

	if(priv->mac_priv == NULL)
		return false;

	if (is_vlan_dev(dev)) {
		real_dev = vlan_dev_real_dev(dev);
		net_ops = real_dev->netdev_ops;
	}

	if(!net_ops)
		return false;
	if(!net_ops->ndo_get_phys_port_id)
		return false;
	/**
	 * TODO:  find inner net devcie,
	 * current by 'hw_features: L2FW_DOFFLOAD', dose more check
	 * ndo_get_port_parent_id(): 0xff need?
	 */
	ppid.id[0] = 0;
	net_ops->ndo_get_port_parent_id(real_dev, &ppid);
	if(ppid.id[0] == SF_GMAC_DUNMMY_ID)
		return true;

	return false;
}
EXPORT_SYMBOL(dpns_port_dev_check);

int dpns_common_netdevice_event(struct notifier_block *unused,
				  unsigned long event, void *ptr)
{
	struct net_device *dev = netdev_notifier_info_to_dev(ptr);
	COMMON_t *priv = container_of(unused, COMMON_t, netdevice_nb);
	dpns_port_t *dp_port;
	int err = 0;

	COMMON_DBG(DBG_LV, "dev:%s %s\n", dev->name, netdev_cmd_to_name(event));

	if (event != NETDEV_REGISTER && !dpns_port_dev_check(priv, dev))
		return NOTIFY_DONE;

	dp_port = dpns_port_by_netdev(priv, dev);
	switch (event) {
	case NETDEV_REGISTER:
		err = dpns_port_probe(priv, dev);
		break;
	case NETDEV_UNREGISTER:
		dpns_port_remove(priv, dp_port, dev);
		break;
	default:
		break;
	}

	if(err < 0)
		COMMON_DBG(ERR_LV, "%s event %s error %d\n",
				 __func__, netdev_cmd_to_name(event), err);
	return NOTIFY_DONE;
}

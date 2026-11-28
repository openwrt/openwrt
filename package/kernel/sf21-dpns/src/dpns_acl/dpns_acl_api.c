#ifndef __DPNS_ACL_API_H
#define __DPNS_ACL_API_H

#include <linux/netlink.h>

#include "dpns_acl.h"

#define print_data(x) do {printk(KERN_CONT #x ":%llx ", (unsigned long long)data->x);} while (0)

void acl_add_data_mode0(struct acl_key_mode0 *data, const struct acl_data_t *msg_data)
{
	fill_data(policy);
	fill_data(pkt_ctrl);
	fill_data(l3_hit);
	fill_data(mf_action);
	fill_data(frame_type_1_0);
	fill_data(frame_type_3_2);
	fill_data(frame_type_5_4);
	fill_data(frame_type_7_6);
	fill_data(frame_type_9_8);
	fill_data(frame_type_11_10);
	fill_data(ivport_id);
	fill_data(ovport_id);
	data->spec_info = msg_data->spec_info_l1;
}

void acl_dump_data_mode0(struct acl_key_mode0 *data)
{
	print_data(policy);
	print_data(pkt_ctrl);
	print_data(l3_hit);
	print_data(mf_action);
	print_data(frame_type_1_0);
	print_data(frame_type_3_2);
	print_data(frame_type_5_4);
	print_data(frame_type_7_6);
	print_data(frame_type_9_8);
	print_data(frame_type_11_10);
	print_data(ivport_id);
	print_data(ovport_id);
	print_data(spec_info);
	printk(KERN_CONT "\n");
}

void acl_add_data_v4_mode1(struct acl_key_v4_mode1 *data, const struct acl_data_t *msg_data)
{
	fill_data(policy);
	fill_data(pkt_ctrl);
	fill_data(l3_hit);
	fill_data(mf_action);
	fill_data(frame_type_1_0);
	fill_data(frame_type_11_10);
	fill_data(ivport_id);
	fill_data(ovport_id);
	fill_data(dip);
	data->spec_info = msg_data->spec_info_l1 >> 24; 
}

void acl_dump_data_v4_mode1(struct acl_key_v4_mode1 *data)
{
	print_data(policy);
	print_data(pkt_ctrl);
	print_data(l3_hit);
	print_data(mf_action);
	print_data(frame_type_1_0);
	print_data(frame_type_11_10);
	print_data(ivport_id);
	print_data(ovport_id);
	print_data(dip);
	print_data(spec_info);
	printk(KERN_CONT "\n");
}

void acl_add_data_v4_mode2(struct acl_key_v4_mode2 *data, const struct acl_data_t *msg_data)
{
	fill_data(policy);
	fill_data(pkt_ctrl);
	fill_data(l3_hit);
	fill_data(mf_action);
	fill_data(frame_type_1_0);
	fill_data(frame_type_11_10);
	fill_data(ivport_id);
	fill_data(ovport_id);
	fill_data(sip);
	data->spec_info = msg_data->spec_info_l1 >> 24;
}

void acl_dump_data_v4_mode2(struct acl_key_v4_mode2 *data)
{
	print_data(policy);
	print_data(pkt_ctrl);
	print_data(l3_hit);
	print_data(mf_action);
	print_data(frame_type_1_0);
	print_data(frame_type_11_10);
	print_data(ivport_id);
	print_data(ovport_id);
	print_data(sip);
	print_data(spec_info);
	printk(KERN_CONT "\n");
}

void acl_add_data_v4_mode3(struct acl_key_v4_mode3 *data, const struct acl_data_t *msg_data)
{
	fill_data(policy);
	fill_data(pkt_ctrl);
	fill_data(mf_action);
	fill_data(frame_type_1_0);
	fill_data(ivport_id);
	fill_data(ovport_id);
	fill_data(sip);
	fill_data(dip);
	fill_data(sport);
	fill_data(dport);
	fill_data(protocol);
	fill_data(ovid);
}

void acl_dump_data_v4_mode3(struct acl_key_v4_mode3 *data)
{
	print_data(policy);
	print_data(pkt_ctrl);
	print_data(mf_action);
	print_data(frame_type_1_0);
	print_data(ivport_id);
	print_data(ovport_id);
	print_data(sip);
	print_data(dip);
	print_data(sport);
	print_data(dport);
	print_data(protocol);
	print_data(ovid);
	printk(KERN_CONT "\n");
}

void acl_add_data_v4_mode4_v6_mode1(struct acl_key_v4_mode4_v6_mode1 *data, const struct acl_data_t *msg_data)
{
	fill_data(policy);
	fill_data(pkt_ctrl);
	fill_data(l3_hit);
	fill_data(mf_action);
	fill_data(frame_type_1_0);
	fill_data(frame_type_3_2);
	fill_data(frame_type_5_4);
	fill_data(frame_type_7_6);
	fill_data(frame_type_9_8);
	fill_data(frame_type_11_10);
	fill_data(smac);
	fill_data(dmac);
	fill_data(ivport_id);
	fill_data(ovport_id);
	fill_data(ovid);
	data->spec_info_l = (u64)msg_data->spec_info_l2 << 32 | msg_data->spec_info_l1;
	data->spec_info_h = (u64)msg_data->spec_info_h2 << 32 | msg_data->spec_info_h1;
}

void acl_dump_data_v4_mode4_v6_mode1(struct acl_key_v4_mode4_v6_mode1 *data)
{
	print_data(policy);
	print_data(pkt_ctrl);
	print_data(l3_hit);
	print_data(mf_action);
	print_data(frame_type_1_0);
	print_data(frame_type_3_2);
	print_data(frame_type_5_4);
	print_data(frame_type_7_6);
	print_data(frame_type_9_8);
	print_data(frame_type_11_10);
	print_data(smac);
	print_data(dmac);
	print_data(ivport_id);
	print_data(ovport_id);
	print_data(ovid);
	print_data(spec_info_l);
	print_data(spec_info_h);
	printk(KERN_CONT "\n");
}

void acl_add_data_v4_mode5(struct acl_key_v4_mode5 *data, const struct acl_data_t *msg_data)
{
	fill_data(policy);
	fill_data(pkt_ctrl);
	fill_data(l3_hit);
	fill_data(mf_action);
	fill_data(frame_type_1_0);
	fill_data(frame_type_3_2);
	fill_data(frame_type_5_4);
	fill_data(frame_type_7_6);
	fill_data(frame_type_9_8);
	fill_data(frame_type_11_10);
	fill_data(smac);
	fill_data(dmac);
	fill_data(ivport_id);
	fill_data(ovport_id);
	fill_data(sip);
	fill_data(dip);
	fill_data(sport);
	fill_data(dport);
	fill_data(protocol);
	fill_data(tos_pri);
	fill_data(ovid);
	data->spec_info = msg_data->spec_info_l1;
}

void acl_dump_data_v4_mode5(struct acl_key_v4_mode5 *data)
{
	print_data(policy);
	print_data(pkt_ctrl);
	print_data(l3_hit);
	print_data(mf_action);
	print_data(frame_type_1_0);
	print_data(frame_type_3_2);
	print_data(frame_type_5_4);
	print_data(frame_type_7_6);
	print_data(frame_type_9_8);
	print_data(frame_type_11_10);
	print_data(smac);
	print_data(dmac);
	print_data(ivport_id);
	print_data(ovport_id);
	print_data(sip);
	print_data(dip);
	print_data(sport);
	print_data(dport);
	print_data(protocol);
	print_data(tos_pri);
	print_data(ovid);
	print_data(spec_info);
	printk(KERN_CONT "\n");
}

void acl_add_data_v4_mode6(struct acl_key_v4_mode6 *data, const struct acl_data_t *msg_data)
{
	fill_data(policy);
	fill_data(pkt_ctrl);
	fill_data(l3_hit);
	fill_data(mf_action);
	fill_data(frame_type_1_0);
	fill_data(frame_type_3_2);
	fill_data(frame_type_5_4);
	fill_data(frame_type_7_6);
	fill_data(frame_type_9_8);
	fill_data(frame_type_11_10);
	fill_data(smac);
	fill_data(dmac);
	fill_data(ivport_id);
	fill_data(ovport_id);
	fill_data(sip);
	fill_data(dip);
	fill_data(sport);
	fill_data(dport);
	fill_data(protocol);
	fill_data(tos_pri);
	fill_data(ovid);
	data->spec_info_l = (u64)msg_data->spec_info_l2 << 32 | msg_data->spec_info_l1;
	data->spec_info_h = (u64)msg_data->spec_info_h2 << 32 | msg_data->spec_info_h1;
}

void acl_dump_data_v4_mode6(struct acl_key_v4_mode6 *data)
{
	print_data(policy);
	print_data(pkt_ctrl);
	print_data(l3_hit);
	print_data(mf_action);
	print_data(frame_type_1_0);
	print_data(frame_type_3_2);
	print_data(frame_type_5_4);
	print_data(frame_type_7_6);
	print_data(frame_type_9_8);
	print_data(frame_type_11_10);
	print_data(smac);
	print_data(dmac);
	print_data(ivport_id);
	print_data(ovport_id);
	print_data(sip);
	print_data(dip);
	print_data(sport);
	print_data(dport);
	print_data(protocol);
	print_data(tos_pri);
	print_data(ovid);
	print_data(spec_info_l);
	print_data(spec_info_h);
	printk(KERN_CONT "\n");
}

void acl_add_data_v6_mode2(struct acl_key_v6_mode2 *data, const struct acl_data_t *msg_data)
{
	fill_data(policy);
	fill_data(pkt_ctrl);
	fill_data(l3_hit);
	fill_data(mf_action);
	fill_data(frame_type_1_0);
	fill_data(frame_type_11_10);
	fill_data(ivport_id);
	fill_data(ovport_id);
	fill_data(sip_l);
	fill_data(sip_h);
	fill_data(dip_l);
	fill_data(dip_h);
}

void acl_dump_data_v6_mode2(struct acl_key_v6_mode2 *data)
{
	print_data(policy);
	print_data(pkt_ctrl);
	print_data(l3_hit);
	print_data(mf_action);
	print_data(frame_type_1_0);
	print_data(frame_type_11_10);
	print_data(ivport_id);
	print_data(ovport_id);
	print_data(sip_l);
	print_data(sip_h);
	print_data(dip_l);
	print_data(dip_h);
	printk(KERN_CONT "\n");
}

void acl_add_data_v6_mode3(struct acl_key_v6_mode3 *data, const struct acl_data_t *msg_data)
{
	fill_data(policy);
	fill_data(pkt_ctrl);
	fill_data(l3_hit);
	fill_data(mf_action);
	fill_data(frame_type_1_0);
	fill_data(frame_type_3_2);
	fill_data(frame_type_5_4);
	fill_data(frame_type_7_6);
	fill_data(frame_type_9_8);
	fill_data(frame_type_11_10);
	fill_data(dmac);
	fill_data(ivport_id);
	fill_data(ovport_id);
	fill_data(dip_l);
	fill_data(dip_h);
	fill_data(dport);
	fill_data(protocol);
	fill_data(tos_pri);
	fill_data(ovid);
	data->spec_info = msg_data->spec_info_l1;
}

void acl_dump_data_v6_mode3(struct acl_key_v6_mode3 *data)
{
	print_data(policy);
	print_data(pkt_ctrl);
	print_data(l3_hit);
	print_data(mf_action);
	print_data(frame_type_1_0);
	print_data(frame_type_3_2);
	print_data(frame_type_5_4);
	print_data(frame_type_7_6);
	print_data(frame_type_9_8);
	print_data(frame_type_11_10);
	print_data(dmac);
	print_data(ivport_id);
	print_data(ovport_id);
	print_data(dip_l);
	print_data(dip_h);
	print_data(dport);
	print_data(protocol);
	print_data(tos_pri);
	print_data(ovid);
	print_data(spec_info);
	printk(KERN_CONT "\n");
}

void acl_add_data_v4_mode7(struct acl_key_v4_mode7 *data, const struct acl_data_t *msg_data)
{
	fill_data(policy);
	fill_data(pkt_ctrl);
	fill_data(l3_hit);
	fill_data(mf_action);
	fill_data(frame_type_1_0);
	fill_data(frame_type_3_2);
	fill_data(frame_type_11_10);
	fill_data(smac);
	fill_data(dmac);
	fill_data(ivport_id);
	fill_data(ovport_id);
	fill_data(sip);
	fill_data(dip);
	fill_data(sport);
	fill_data(dport);
	fill_data(protocol);
	fill_data(tos_pri);
	fill_data(ovid);
	data->spec_info_l = (u64)msg_data->spec_info_l2 << 32 | msg_data->spec_info_l1;
	data->spec_info_h = (u64)msg_data->spec_info_h2 << 32 | msg_data->spec_info_h1;
}

void acl_dump_data_v4_mode7(struct acl_key_v4_mode7 *data)
{
	print_data(policy);
	print_data(pkt_ctrl);
	print_data(l3_hit);
	print_data(mf_action);
	print_data(frame_type_1_0);
	print_data(frame_type_3_2);
	print_data(frame_type_11_10);
	print_data(smac);
	print_data(dmac);
	print_data(ivport_id);
	print_data(ovport_id);
	print_data(sip);
	print_data(dip);
	print_data(sport);
	print_data(dport);
	print_data(protocol);
	print_data(tos_pri);
	print_data(ovid);
	print_data(spec_info_l);
	print_data(spec_info_h);
	printk(KERN_CONT "\n");
}

void acl_add_data_v6_mode7(struct acl_key_v6_mode7 *data, const struct acl_data_t *msg_data)
{
	fill_data(policy);
	fill_data(pkt_ctrl);
	fill_data(l3_hit);
	fill_data(mf_action);
	fill_data(frame_type_1_0);
	fill_data(frame_type_3_2);
	fill_data(frame_type_11_10);
	fill_data(smac);
	fill_data(dmac);
	fill_data(ivport_id);
	fill_data(ovport_id);
	fill_data(sip_l);
	fill_data(sip_h);
	fill_data(dip_l);
	fill_data(dip_h);
	fill_data(sport);
	fill_data(dport);
	fill_data(protocol);
	fill_data(tos_pri);
	fill_data(ovid);
	data->spec_info_l = (u64)msg_data->spec_info_l2 << 32 | msg_data->spec_info_l1;
	data->spec_info_h = (u64)msg_data->spec_info_h2 << 32 | msg_data->spec_info_h1;
}

void acl_dump_data_v6_mode7(struct acl_key_v6_mode7 *data)
{
	print_data(policy);
	print_data(pkt_ctrl);
	print_data(l3_hit);
	print_data(mf_action);
	print_data(frame_type_1_0);
	print_data(frame_type_3_2);
	print_data(frame_type_11_10);
	print_data(smac);
	print_data(dmac);
	print_data(ivport_id);
	print_data(ovport_id);
	print_data(sip_l);
	print_data(sip_h);
	print_data(dip_l);
	print_data(dip_h);
	print_data(sport);
	print_data(dport);
	print_data(protocol);
	print_data(tos_pri);
	print_data(ovid);
	print_data(spec_info_l);
	print_data(spec_info_h);
	printk(KERN_CONT "\n");
}
#endif

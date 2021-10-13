/*
 * Copyright (c) 2013, 2015-2017, 2019, The Linux Foundation. All rights reserved.
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#ifndef _SHELL_IO_H
#define _SHELL_IO_H

#include "sw.h"
#include "sw_api.h"
#include "fal.h"

#define SW_TYPE_DEF(type, parser, show) {type, parser, show}
typedef sw_error_t
	(*param_check_t)(char *, a_uint32_t *, a_uint32_t);
typedef sw_error_t
	(*param_check_range_t)(char *, a_uint32_t *, a_uint32_t, a_uint32_t);
typedef sw_error_t
	(*param_check_boolean_t)(char *, a_bool_t, a_bool_t *, a_uint32_t);
typedef struct
{
    sw_data_type_e data_type;
    param_check_t param_check;
    void (*show_func) (void);
} sw_data_type_t;

void  set_talk_mode(int mode);
int get_talk_mode(void);
void set_full_cmdstrp(char **cmdstrp);
int get_jump(void);
sw_data_type_t * cmd_data_type_find(sw_data_type_e type);
void  cmd_strtol(char *str, a_uint32_t * arg_val);
sw_error_t cmd_data_check_portmap(char *cmdstr, fal_pbmp_t * val, a_uint32_t size);
sw_error_t cmd_data_check_confirm(char *cmdstr, a_bool_t def, a_bool_t * val, a_uint32_t size);

sw_error_t cmd_data_check_uint32(char *cmd_str, a_uint32_t * arg_val,
                                 a_uint32_t size);
sw_error_t cmd_data_check_uint16(char *cmd_str, a_uint32_t * arg_val,
                                 a_uint32_t size);
sw_error_t cmd_data_check_uint8(char *cmd_str, a_uint32_t * arg_val,
                                 a_uint32_t size);
sw_error_t cmd_data_check_enable(char *cmd_str, a_uint32_t * arg_val,
                                 a_uint32_t size);
sw_error_t cmd_data_check_pbmp(char *cmd_str, a_uint32_t * arg_val,
                               a_uint32_t size);
#ifdef IN_PORTCONTROL
sw_error_t cmd_data_check_duplex(char *cmd_str, a_uint32_t * arg_val,
                                 a_uint32_t size);
sw_error_t cmd_data_check_speed(char *cmd_str, a_uint32_t * arg_val,
                                a_uint32_t size);
#ifndef IN_PORTCONTROL_MINI
sw_error_t
cmd_data_check_port_eee_config(char *cmd_str, void * val, a_uint32_t size);
sw_error_t
cmd_data_check_switch_port_loopback_config(char *cmd_str, void * val,
	a_uint32_t size);
#endif
#endif
#ifdef IN_PORTVLAN
sw_error_t cmd_data_check_1qmode(char *cmd_str, a_uint32_t * arg_val,
                                 a_uint32_t size);
sw_error_t cmd_data_check_egmode(char *cmd_str, a_uint32_t * arg_val,
                                 a_uint32_t size);
#ifdef HPPE
sw_error_t
cmd_data_check_global_qinqmode(char *info, void *val, a_uint32_t size);
sw_error_t
cmd_data_check_port_qinqmode(char *info, void *val, a_uint32_t size);
sw_error_t
cmd_data_check_tpid(char *info, void *val, a_uint32_t size);
sw_error_t
cmd_data_check_ingress_filter(char *info, void *val, a_uint32_t size);
sw_error_t
cmd_data_check_port_vlan_direction(char *cmd_str, a_uint32_t * arg_val, a_uint32_t size);
sw_error_t
cmd_data_check_port_default_vid_en(char *info, void *val, a_uint32_t size);
sw_error_t
cmd_data_check_port_vlan_tag(char *info, void *val, a_uint32_t size);
sw_error_t
cmd_data_check_tag_propagation(char *info, void *val, a_uint32_t size);
sw_error_t
cmd_data_check_egress_mode(char *info, void *val, a_uint32_t size);
sw_error_t
cmd_data_check_port_vlan_translation_adv_rule(char *info, void *val,
				a_uint32_t size);
sw_error_t
cmd_data_check_port_vlan_translation_adv_action(char *info, void *val,
				a_uint32_t size);
#endif
#endif
#ifdef IN_PORTCONTROL
sw_error_t cmd_data_check_capable(char *cmd_str, a_uint32_t * arg_val,
                                  a_uint32_t size);
#endif
#ifdef IN_FDB
sw_error_t cmd_data_check_fdbentry(char *cmdstr, void *val, a_uint32_t size);
#ifndef IN_FDB_MINI
sw_error_t cmd_data_check_maclimit_ctrl(char *info, void *val, a_uint32_t size);
#endif
#endif
sw_error_t cmd_data_check_macaddr(char *cmdstr, void *val, a_uint32_t size);
#ifdef IN_VLAN
sw_error_t cmd_data_check_vlan(char *cmdstr, fal_vlan_t * val, a_uint32_t size);
#endif
#ifdef IN_QOS
#ifndef IN_QOS_MINI
sw_error_t cmd_data_check_qos_sch(char *cmdstr, fal_sch_mode_t * val,
                                  a_uint32_t size);
sw_error_t cmd_data_check_qos_pt(char *cmdstr, fal_qos_mode_t * val,
                                 a_uint32_t size);
sw_error_t
cmd_data_check_port_group(char *cmd_str, void * val, a_uint32_t size);
sw_error_t
cmd_data_check_port_pri(char *cmd_str, void * val, a_uint32_t size);
sw_error_t
cmd_data_check_port_remark(char *cmd_str, void * val, a_uint32_t size);
sw_error_t
cmd_data_check_cosmap(char *cmd_str, void * val, a_uint32_t size);
sw_error_t
cmd_data_check_queue_scheduler(char *cmd_str, void * val, a_uint32_t size);
sw_error_t
cmd_data_check_ring_queue(char *cmd_str, void * val, a_uint32_t size);
#endif
#endif
#ifdef IN_RATE
sw_error_t cmd_data_check_storm(char *cmdstr, fal_storm_type_t * val,
                                a_uint32_t size);
#endif
#ifdef IN_STP
sw_error_t cmd_data_check_stp_state(char *cmdstr, fal_stp_state_t * val,
                                    a_uint32_t size);
#endif
#ifdef IN_LEAKY
sw_error_t cmd_data_check_leaky(char *cmdstr, fal_leaky_ctrl_mode_t * val,
                                a_uint32_t size);
#endif
sw_error_t cmd_data_check_uinta(char *cmdstr, a_uint32_t * val,
                                a_uint32_t size);
sw_error_t cmd_data_check_maccmd(char *cmdstr, fal_fwd_cmd_t * val,
                                 a_uint32_t size);
#ifdef IN_IP
#ifndef IN_IP_MINI
sw_error_t cmd_data_check_flowcmd(char *cmdstr, fal_default_flow_cmd_t * val,
                                 a_uint32_t size);
sw_error_t cmd_data_check_flowtype(char *cmdstr, fal_flow_type_t * val,
                                 a_uint32_t size);
#endif
#endif
#ifdef IN_LED
sw_error_t cmd_data_check_ledpattern(char *info, void * val, a_uint32_t size);
#endif
#ifdef IN_PORTVLAN
sw_error_t
cmd_data_check_invlan_mode(char *cmd_str, a_uint32_t * arg_val, a_uint32_t size);
sw_error_t
cmd_data_check_vlan_propagation(char *cmd_str, a_uint32_t * arg_val, a_uint32_t size);
#ifndef IN_PORTVLAN_MINI
sw_error_t
cmd_data_check_vlan_translation(char *info, fal_vlan_trans_entry_t *val, a_uint32_t size);
#endif
sw_error_t
cmd_data_check_qinq_mode(char *cmd_str, a_uint32_t * arg_val, a_uint32_t size);
sw_error_t
cmd_data_check_qinq_role(char *cmd_str, a_uint32_t * arg_val, a_uint32_t size);
#endif
#ifdef IN_PORTCONTROL
sw_error_t
cmd_data_check_hdrmode(char *cmd_str, a_uint32_t * arg_val, a_uint32_t size);
#endif
#ifdef IN_FDB
sw_error_t
cmd_data_check_fdboperation(char *cmd_str, void * val, a_uint32_t size);
#endif
#ifdef IN_PPPOE
sw_error_t
cmd_data_check_pppoe(char *cmd_str, void * val, a_uint32_t size);
sw_error_t
cmd_data_check_pppoe_less(char *cmd_str, void * val, a_uint32_t size);
#endif
#if defined(IN_IP) || defined(IN_NAT)
sw_error_t
cmd_data_check_host_entry(char *cmd_str, void * val, a_uint32_t size);
sw_error_t
cmd_data_check_arp_learn_mode(char *cmd_str, fal_arp_learn_mode_t * arg_val,
                              a_uint32_t size);

sw_error_t
cmd_data_check_ip_guard_mode(char *cmd_str, fal_source_guard_mode_t * arg_val, a_uint32_t size);


sw_error_t
cmd_data_check_nat_entry(char *cmd_str, void * val, a_uint32_t size);


sw_error_t
cmd_data_check_napt_entry(char *cmd_str, void * val, a_uint32_t size);

sw_error_t
cmd_data_check_flow_entry(char *cmd_str, void * val, a_uint32_t size);

sw_error_t
cmd_data_check_napt_mode(char *cmd_str, fal_napt_mode_t * arg_val, a_uint32_t size);


sw_error_t
cmd_data_check_intf_mac_entry(char *cmd_str, void * val, a_uint32_t size);

sw_error_t
cmd_data_check_pub_addr_entry(char *cmd_str, void * val, a_uint32_t size);
#endif

sw_error_t
cmd_data_check_ip4addr(char *cmdstr, void * val, a_uint32_t size);

sw_error_t
cmd_data_check_ip6addr(char *cmdstr, void * val, a_uint32_t size);


sw_error_t
cmd_data_check_egress_shaper(char *cmd_str, void * val, a_uint32_t size);

#ifdef IN_RATE
sw_error_t
cmd_data_check_port_policer(char *cmd_str, void * val, a_uint32_t size);

sw_error_t
cmd_data_check_policer_timesslot(char *cmd_str, a_uint32_t * val, a_uint32_t size);

sw_error_t
cmd_data_check_acl_policer(char *cmd_str, void * val, a_uint32_t size);
#endif
#ifdef IN_FDB
#ifndef IN_FDB_MINI
sw_error_t
cmd_data_check_fdb_smode(char *cmd_str, a_uint32_t * arg_val, a_uint32_t size);
#endif
#endif
#ifdef IN_IGMP
sw_error_t
cmd_data_check_multi(char *info, void *val, a_uint32_t size);
#endif
#ifdef IN_SEC
sw_error_t
cmd_data_check_sec_mac(char *cmd_str, a_uint32_t * arg_val, a_uint32_t size);

sw_error_t
cmd_data_check_sec_ip(char *cmd_str, a_uint32_t * arg_val, a_uint32_t size);

sw_error_t
cmd_data_check_sec_ip4(char *cmd_str, a_uint32_t * arg_val, a_uint32_t size);

sw_error_t
cmd_data_check_sec_ip6(char *cmd_str, a_uint32_t * arg_val, a_uint32_t size);

sw_error_t
cmd_data_check_sec_tcp(char *cmd_str, a_uint32_t * arg_val, a_uint32_t size);

sw_error_t
cmd_data_check_sec_udp(char *cmd_str, a_uint32_t * arg_val, a_uint32_t size);

sw_error_t
cmd_data_check_sec_icmp4(char *cmd_str, a_uint32_t * arg_val, a_uint32_t size);

sw_error_t
cmd_data_check_sec_icmp6(char *cmd_str, a_uint32_t * arg_val, a_uint32_t size);
#ifdef HPPE
sw_error_t
cmd_data_check_l3_parser(char *cmd_str, void * val, a_uint32_t size);

sw_error_t
cmd_data_check_l4_parser(char *cmd_str, void * val, a_uint32_t size);

sw_error_t
cmd_data_check_exp_ctrl(char *cmd_str, void * val, a_uint32_t size);
#endif
#endif
#ifdef IN_COSMAP
#ifndef IN_COSMAP_MINI
sw_error_t
cmd_data_check_remark_entry(char *info, void *val, a_uint32_t size);
#endif
#endif
#ifdef IN_IP
#ifndef IN_IP_MINI
sw_error_t
cmd_data_check_default_route_entry(char *cmd_str, void * val, a_uint32_t size);

sw_error_t
cmd_data_check_host_route_entry(char *cmd_str, void * val, a_uint32_t size);

sw_error_t
cmd_data_check_ip4_rfs_entry(char *cmd_str, void * val, a_uint32_t size);
sw_error_t
cmd_data_check_ip6_rfs_entry(char *cmd_str, void * val, a_uint32_t size);
sw_error_t
cmd_data_check_arp_sg(char *cmd_str, void * val, a_uint32_t size);
sw_error_t
cmd_data_check_network_route(char *cmd_str, void * val, a_uint32_t size);
sw_error_t
cmd_data_check_intf(char *cmd_str, void * val, a_uint32_t size);
sw_error_t
cmd_data_check_vsi_intf(char *cmd_str, void * val, a_uint32_t size);
sw_error_t
cmd_data_check_nexthop(char *cmd_str, void * val, a_uint32_t size);
sw_error_t
cmd_data_check_ip_sg(char *cmd_str, void * val, a_uint32_t size);
sw_error_t
cmd_data_check_ip_pub(char *cmd_str, void * val, a_uint32_t size);
sw_error_t
cmd_data_check_ip_portmac(char *cmd_str, void * val, a_uint32_t size);
sw_error_t
cmd_data_check_ip_mcmode(char *cmd_str, void * val, a_uint32_t size);
sw_error_t
cmd_data_check_ip_global(char *cmd_str, void * val, a_uint32_t size);
#endif
#endif
#if defined(IN_IP) || defined(IN_NAT)
sw_error_t
cmd_data_check_flow_cookie(char *cmd_str, void * val, a_uint32_t size);

sw_error_t
cmd_data_check_flow_rfs(char *cmd_str, void * val, a_uint32_t size);
#endif
#ifdef IN_PORTCONTROL
#ifndef IN_PORTCONTROL_MINI
sw_error_t
cmd_data_check_crossover_mode(char *cmd_str, a_uint32_t * arg_val, a_uint32_t size);

sw_error_t
cmd_data_check_crossover_status(char *cmd_str, a_uint32_t * arg_val, a_uint32_t size);

sw_error_t
cmd_data_check_prefer_medium(char *cmd_str, a_uint32_t * arg_val, a_uint32_t size);

sw_error_t
cmd_data_check_fiber_mode(char *cmd_str, a_uint32_t * arg_val, a_uint32_t size);

sw_error_t
cmd_data_check_src_filter_config(char *cmd_str, a_uint32_t * arg_val, a_uint32_t size);

sw_error_t
cmd_data_check_mtu_entry(char *cmd_str, void * val, a_uint32_t size);

sw_error_t
cmd_data_check_mru_entry(char *cmd_str, void * val, a_uint32_t size);


#endif
#endif
#ifdef IN_INTERFACECONTROL
sw_error_t
cmd_data_check_interface_mode(char *cmd_str, a_uint32_t * arg_val, a_uint32_t size);
#endif
#ifdef IN_VSI
sw_error_t
cmd_data_check_newadr_lrn(char *cmd_str, void * val, a_uint32_t size);

sw_error_t
cmd_data_check_stamove(char *cmd_str, void * val, a_uint32_t size);

sw_error_t
cmd_data_check_vsi_member(char *cmd_str, void * val, a_uint32_t size);

#endif
#ifdef IN_BM
sw_error_t
cmd_data_check_bm_dynamic_thresh(char *cmd_str, void * val, a_uint32_t size);
sw_error_t
cmd_data_check_bm_static_thresh(char *cmd_str, void * val, a_uint32_t size);
#endif
#ifdef IN_QM
sw_error_t
cmd_data_check_u_qmap(char *cmd_str, void * val, a_uint32_t size);

sw_error_t
cmd_data_check_ac_static_thresh(char *cmd_str, void * val, a_uint32_t size);

sw_error_t
cmd_data_check_ac_dynamic_thresh(char *cmd_str, void * val, a_uint32_t size);

sw_error_t
cmd_data_check_ac_group_buff(char *cmd_str, void * val, a_uint32_t size);

sw_error_t
cmd_data_check_ac_ctrl(char *cmd_str, void * val, a_uint32_t size);

sw_error_t
cmd_data_check_ac_obj(char *cmd_str, void * val, a_uint32_t size);

#endif
#ifdef IN_FLOW
sw_error_t
cmd_data_check_flow_age(char *cmd_str, void * val, a_uint32_t size);
sw_error_t
cmd_data_check_flow_ctrl(char *cmd_str, void * val, a_uint32_t size);
sw_error_t
cmd_data_check_flow(char *cmd_str, void * val, a_uint32_t size);
sw_error_t
cmd_data_check_flow_global(char *cmd_str, void * val, a_uint32_t size);
sw_error_t
cmd_data_check_flow_host(char *cmd_str, void * val, a_uint32_t size);
#endif

#ifdef IN_POLICER
sw_error_t
cmd_data_check_port_policer_config(char *cmd_str, void * val, a_uint32_t size);

sw_error_t
cmd_data_check_policer_cmd_config(char *cmd_str, void * val, a_uint32_t size);

sw_error_t
cmd_data_check_acl_policer_config(char *cmd_str, void * val, a_uint32_t size);

#endif

#ifdef IN_SHAPER
sw_error_t
cmd_data_check_port_shaper_token_config(char *cmd_str, void * val, a_uint32_t size);

sw_error_t
cmd_data_check_shaper_token_config(char *cmd_str, void * val, a_uint32_t size);

sw_error_t
cmd_data_check_port_shaper_config(char *cmd_str, void * val, a_uint32_t size);

sw_error_t
cmd_data_check_shaper_config(char *cmd_str, void * val, a_uint32_t size);

#endif

#ifdef IN_SERVCODE
sw_error_t
cmd_data_check_servcode_config(char *info, fal_servcode_config_t *val, a_uint32_t size);
#endif

#ifdef IN_RSS_HASH
sw_error_t
cmd_data_check_rss_hash_mode(char *cmd_str, a_uint32_t * arg_val, a_uint32_t size);
sw_error_t
cmd_data_check_rss_hash_config(char *info, fal_rss_hash_config_t *val, a_uint32_t size);
#endif

#ifdef IN_MIRROR
sw_error_t
cmd_data_check_mirr_analy_cfg(char *info, void *val, a_uint32_t size);
sw_error_t
cmd_data_check_mirr_direction(char *cmd_str, a_uint32_t * arg_val, a_uint32_t size);
#endif
sw_error_t
cmd_data_check_integer(char *cmd_str, a_uint32_t * arg_val, a_uint32_t max_val, a_uint32_t min_val);
#ifdef IN_CTRLPKT
sw_error_t
cmd_data_check_ctrlpkt_appprofile(char *info, void *val, a_uint32_t size);
#endif
#ifdef IN_ACL
sw_error_t
cmd_data_check_ruletype(char *cmd_str, fal_acl_rule_type_t * arg_val, a_uint32_t size);
sw_error_t
cmd_data_check_udf_pkt_type(char *cmdstr, fal_acl_udf_pkt_type_t * arg_val, a_uint32_t size);
sw_error_t
cmd_data_check_udf_type(char *cmdstr, fal_acl_udf_type_t * arg_val, a_uint32_t size);
sw_error_t
cmd_data_check_udf_element(char *cmdstr, a_uint8_t * val, a_uint32_t * len);
sw_error_t
cmd_data_check_fieldop(char *cmdstr, fal_acl_field_op_t def, fal_acl_field_op_t * val);

#endif
sw_error_t
cmd_data_check_module(char *cmd_str, a_uint32_t * arg_val, a_uint32_t size);
sw_error_t
cmd_data_check_func_ctrl(char *cmd_str, void * val, a_uint32_t size);
#endif


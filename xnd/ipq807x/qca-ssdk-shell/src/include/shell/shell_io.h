/*
 * Copyright (c) 2014-2019, The Linux Foundation. All rights reserved.
 * Permission to use, copy, modify, and/or distribute this software for
 * any purpose with or without fee is hereby granted, provided that the
 * above copyright notice and this permission notice appear in all copies.
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT
 * OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */
/*qca808x_start*/
#ifndef _SHELL_IO_H
#define _SHELL_IO_H

#include "sw.h"
#include "sw_api.h"
#include "fal.h"
/*qca808x_end*/
#include "ref_vlan.h"

/*qca808x_start*/
#define SW_TYPE_DEF(type, parser, show) {type, parser, show}
typedef struct
{
    sw_data_type_e data_type;
    sw_error_t(*param_check) ();
    void (*show_func) ();
} sw_data_type_t;

void  set_talk_mode(int mode);
int get_talk_mode(void);
void set_full_cmdstrp(char **cmdstrp);
int
get_jump(void);
sw_data_type_t * cmd_data_type_find(sw_data_type_e type);
void  cmd_strtol(char *str, a_uint32_t * arg_val);

sw_error_t __cmd_data_check_complex(char *info, char *defval, char *usage,
				sw_error_t(*chk_func)(), void *arg_val,
				a_uint32_t size);

sw_error_t cmd_data_check_portid(char *cmdstr, fal_port_t * val, a_uint32_t size);

sw_error_t cmd_data_check_portmap(char *cmdstr, fal_pbmp_t * val, a_uint32_t size);
sw_error_t cmd_data_check_confirm(char *cmdstr, a_bool_t def, a_bool_t * val, a_uint32_t size);

sw_error_t cmd_data_check_uint64(char *cmd_str, a_uint64_t * arg_val,
                                 a_uint32_t size);
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
sw_error_t cmd_data_check_duplex(char *cmd_str, a_uint32_t * arg_val,
                                 a_uint32_t size);
sw_error_t cmd_data_check_speed(char *cmd_str, a_uint32_t * arg_val,
                                a_uint32_t size);
/*qca808x_end*/
sw_error_t cmd_data_check_1qmode(char *cmd_str, a_uint32_t * arg_val,
                                 a_uint32_t size);
sw_error_t cmd_data_check_egmode(char *cmd_str, a_uint32_t * arg_val,
                                 a_uint32_t size);
/*qca808x_start*/
sw_error_t cmd_data_check_capable(char *cmd_str, a_uint32_t * arg_val,
                                  a_uint32_t size);
/*qca808x_end*/
sw_error_t cmd_data_check_fdbentry(char *cmdstr, void *val, a_uint32_t size);
sw_error_t cmd_data_check_maclimit_ctrl(char *cmdstr, void *val, a_uint32_t size);
/*qca808x_start*/
sw_error_t cmd_data_check_macaddr(char *cmdstr, void *val, a_uint32_t size);

void cmd_data_print_uint64(a_uint8_t * param_name, a_uint64_t * buf,
                           a_uint32_t size);
void cmd_data_print_uint32(a_char_t * param_name, a_uint32_t * buf,
                           a_uint32_t size);
void cmd_data_print_uint16(a_char_t * param_name, a_uint32_t * buf,
                           a_uint32_t size);
void cmd_data_print_uint8(a_uint8_t * param_name, a_uint32_t * buf,
                           a_uint32_t size);
void cmd_data_print_enable(a_char_t * param_name, a_uint32_t * buf,
                           a_uint32_t size);
void cmd_data_print_pbmp(a_uint8_t * param_name, a_uint32_t * buf,
                         a_uint32_t size);
void cmd_data_print_duplex(a_uint8_t * param_name, a_uint32_t * buf,
                           a_uint32_t size);
void cmd_data_print_speed(a_uint8_t * param_name, a_uint32_t * buf,
                          a_uint32_t size);
/*qca808x_end*/
sw_error_t cmd_data_check_vlan(char *cmdstr, fal_vlan_t * val, a_uint32_t size);
void cmd_data_print_vlan(a_uint8_t * param_name, a_uint32_t * buf,
                         a_uint32_t size);
sw_error_t cmd_data_check_lan_wan_cfg(char *cmd_str, void *arg_val, a_uint32_t size);

void cmd_data_print_lan_wan_cfg(a_uint8_t *param_name, a_ulong_t *buf, a_uint32_t size);

void cmd_data_print_mib(a_uint8_t * param_name, a_uint32_t * buf,
                        a_uint32_t size);
void cmd_data_print_mib_cntr(a_uint8_t * param_name, a_uint64_t * buf,
                        a_uint32_t size);
void cmd_data_print_xgmib(a_uint8_t * param_name, a_uint64_t * buf,
                        a_uint64_t size);
void cmd_data_print_1qmode(a_uint8_t * param_name, a_uint32_t * buf,
                           a_uint32_t size);
void cmd_data_print_egmode(a_char_t * param_name, a_uint32_t * buf,
                           a_uint32_t size);
/*qca808x_start*/
void cmd_data_print_capable(a_uint8_t * param_name, a_uint32_t * buf,
                            a_uint32_t size);
/*qca808x_end*/
void cmd_data_print_maclimit_ctrl(a_uint8_t * param_name, a_uint32_t * buf,
                            a_uint32_t size);
/*qca808x_start*/
void cmd_data_print_macaddr(a_char_t * param_name, a_uint32_t * buf,
                            a_uint32_t size);
/*qca808x_end*/
sw_error_t cmd_data_check_qos_sch(char *cmdstr, fal_sch_mode_t * val,
                                  a_uint32_t size);
void cmd_data_print_qos_sch(a_uint8_t * param_name, a_uint32_t * buf,
                            a_uint32_t size);
sw_error_t cmd_data_check_qos_pt(char *cmdstr, fal_qos_mode_t * val,
                                 a_uint32_t size);
void cmd_data_print_qos_pt(a_uint8_t * param_name, a_uint32_t * buf,
                           a_uint32_t size);
sw_error_t cmd_data_check_storm(char *cmdstr, fal_storm_type_t * val,
                                a_uint32_t size);
void cmd_data_print_storm(a_uint8_t * param_name, a_uint32_t * buf,
                          a_uint32_t size);
sw_error_t cmd_data_check_stp_state(char *cmdstr, fal_stp_state_t * val,
                                    a_uint32_t size);
void cmd_data_print_stp_state(a_uint8_t * param_name, a_uint32_t * buf,
                              a_uint32_t size);
sw_error_t cmd_data_check_leaky(char *cmdstr, fal_leaky_ctrl_mode_t * val,
                                a_uint32_t size);
void cmd_data_print_leaky(a_uint8_t * param_name, a_uint32_t * buf,
                          a_uint32_t size);

sw_error_t cmd_data_check_uinta(char *cmdstr, a_uint32_t * val,
                                a_uint32_t size);
void cmd_data_print_uinta(a_uint8_t * param_name, a_uint32_t * buf,
                          a_uint32_t size);
sw_error_t cmd_data_check_maccmd(char *cmdstr, fal_fwd_cmd_t * val,
                                 a_uint32_t size);
void cmd_data_print_maccmd(a_char_t * param_name, a_uint32_t * buf,
                           a_uint32_t size);
sw_error_t cmd_data_check_flowcmd(char *cmdstr, fal_default_flow_cmd_t * val,
                                 a_uint32_t size);
void cmd_data_print_flowcmd(a_char_t *param_name, a_uint32_t * buf,
                                 a_uint32_t size);
sw_error_t cmd_data_check_flowtype(char *cmdstr, fal_flow_type_t * val,
                                 a_uint32_t size);
void cmd_data_print_flowtype(a_char_t *param_name, a_uint32_t * buf,
                                 a_uint32_t size);
sw_error_t cmd_data_check_aclrule(char *info, void *val, a_uint32_t size);

void cmd_data_print_aclrule(a_char_t * param_name, a_uint32_t * buf,
                            a_uint32_t size);

sw_error_t
cmd_data_check_ledpattern(char *info, void * val, a_uint32_t size);

void
cmd_data_print_ledpattern(a_uint8_t * param_name, a_uint32_t * buf,
                          a_uint32_t size);

sw_error_t
cmd_data_check_mirr_analy_cfg(char *info, void *val, a_uint32_t size);
void
cmd_data_print_mirr_analy_cfg(a_uint8_t * param_name, a_uint32_t * buf, a_uint32_t size);
sw_error_t
cmd_data_check_mirr_direction(char *cmd_str, a_uint32_t * arg_val, a_uint32_t size);
void
cmd_data_print_mirr_direction(a_uint8_t * param_name, a_uint32_t * buf, a_uint32_t size);
sw_error_t
cmd_data_check_invlan_mode(char *cmd_str, a_uint32_t * arg_val, a_uint32_t size);
void
cmd_data_print_invlan_mode(a_uint8_t * param_name, a_uint32_t * buf, a_uint32_t size);
sw_error_t
cmd_data_check_vlan_propagation(char *cmd_str, a_uint32_t * arg_val, a_uint32_t size);
void
cmd_data_print_vlan_propagation(a_char_t * param_name, a_uint32_t * buf, a_uint32_t size);
sw_error_t
cmd_data_check_vlan_translation(char *info, fal_vlan_trans_entry_t *val, a_uint32_t size);
void
cmd_data_print_vlan_translation(a_uint8_t * param_name, a_uint32_t * buf, a_uint32_t size);
sw_error_t
cmd_data_check_qinq_mode(char *cmd_str, a_uint32_t * arg_val, a_uint32_t size);
void
cmd_data_print_qinq_mode(a_char_t * param_name, a_uint32_t * buf, a_uint32_t size);
sw_error_t
cmd_data_check_qinq_role(char *cmd_str, a_uint32_t * arg_val, a_uint32_t size);
void
cmd_data_print_qinq_role(a_char_t * param_name, a_uint32_t * buf, a_uint32_t size);
/*qca808x_start*/
void
cmd_data_print_cable_status(a_uint8_t * param_name, a_uint32_t * buf, a_uint32_t size);
void
cmd_data_print_cable_len(a_uint8_t * param_name, a_uint32_t * buf, a_uint32_t size);
void
cmd_data_print_ssdk_cfg(a_uint8_t * param_name, a_uint32_t * buf, a_uint32_t size);
/*qca808x_end*/
sw_error_t
cmd_data_check_hdrmode(char *cmd_str, a_uint32_t * arg_val, a_uint32_t size);

void
cmd_data_print_hdrmode(a_uint8_t * param_name, a_uint32_t * buf, a_uint32_t size);

sw_error_t
cmd_data_check_fdboperation(char *cmd_str, void * val, a_uint32_t size);

sw_error_t
cmd_data_check_pppoe(char *cmd_str, void * val, a_uint32_t size);

sw_error_t
cmd_data_check_pppoe_less(char *cmd_str, void * val, a_uint32_t size);

void
cmd_data_print_pppoe(a_uint8_t * param_name, a_uint32_t * buf, a_uint32_t size);

sw_error_t
cmd_data_check_udf_type(char *cmdstr, fal_acl_udf_type_t * arg_val, a_uint32_t size);

void
cmd_data_print_udf_type(a_char_t * param_name, a_uint32_t * buf,
                        a_uint32_t size);

sw_error_t
cmd_data_check_udf_pkt_type(a_char_t *cmdstr, fal_acl_udf_pkt_type_t * arg_val, a_uint32_t size);

void
cmd_data_print_udf_pkt_type(a_char_t * param_name, a_uint32_t * buf,
                        a_uint32_t size);

sw_error_t
cmd_data_check_host_entry(char *cmd_str, void * val, a_uint32_t size);

void
cmd_data_print_host_entry(a_uint8_t * param_name, a_uint32_t * buf, a_uint32_t size);

sw_error_t
cmd_data_check_arp_learn_mode(char *cmd_str, fal_arp_learn_mode_t * arg_val,
                              a_uint32_t size);

void
cmd_data_print_arp_learn_mode(a_uint8_t * param_name, a_uint32_t * buf, a_uint32_t size);

sw_error_t
cmd_data_check_ip_guard_mode(char *cmd_str, fal_source_guard_mode_t * arg_val, a_uint32_t size);

void
cmd_data_print_ip_guard_mode(a_uint8_t * param_name, a_uint32_t * buf, a_uint32_t size);

sw_error_t
cmd_data_check_nat_entry(char *cmd_str, void * val, a_uint32_t size);

void
cmd_data_print_nat_entry(a_uint8_t * param_name, a_uint32_t * buf, a_uint32_t size);

sw_error_t
cmd_data_check_napt_entry(char *cmd_str, void * val, a_uint32_t size);

void
cmd_data_print_napt_entry(a_uint8_t * param_name, a_uint32_t * buf, a_uint32_t size);

sw_error_t
cmd_data_check_flow_entry(char *cmd_str, void * val, a_uint32_t size);

void
cmd_data_print_flow_entry(a_uint8_t * param_name, a_uint32_t * buf, a_uint32_t size);

sw_error_t
cmd_data_check_napt_mode(char *cmd_str, fal_napt_mode_t * arg_val, a_uint32_t size);

void
cmd_data_print_napt_mode(a_uint8_t * param_name, a_uint32_t * buf, a_uint32_t size);

sw_error_t
cmd_data_check_intf_mac_entry(char *cmd_str, void * val, a_uint32_t size);

void
cmd_data_print_intf_mac_entry(a_uint8_t * param_name, a_uint32_t * buf, a_uint32_t size);

sw_error_t
cmd_data_check_ip4addr(char *cmdstr, void * val, a_uint32_t size);

void
cmd_data_print_ip4addr(a_char_t * param_name, a_uint32_t * buf, a_uint32_t size);

sw_error_t
cmd_data_check_ip6addr(char *cmdstr, void * val, a_uint32_t size);

void
cmd_data_print_ip6addr(a_char_t * param_name, a_uint32_t * buf, a_uint32_t size);

sw_error_t
cmd_data_check_pub_addr_entry(char *cmd_str, void * val, a_uint32_t size);


void
cmd_data_print_pub_addr_entry(a_uint8_t * param_name, a_uint32_t * buf, a_uint32_t size);


sw_error_t
cmd_data_check_egress_shaper(char *cmd_str, void * val, a_uint32_t size);


void
cmd_data_print_egress_shaper(a_uint8_t * param_name, a_uint32_t * buf, a_uint32_t size);


sw_error_t
cmd_data_check_acl_policer(char *cmd_str, void * val, a_uint32_t size);


void
cmd_data_print_acl_policer(a_uint8_t * param_name, a_uint32_t * buf, a_uint32_t size);


sw_error_t
cmd_data_check_port_policer(char *cmd_str, void * val, a_uint32_t size);

void
cmd_data_print_port_policer(a_uint8_t * param_name, a_uint32_t * buf, a_uint32_t size);

sw_error_t
cmd_data_check_mac_config(char *cmd_str, void * val, a_uint32_t size);

void
cmd_data_print_mac_config(a_uint8_t * param_name, a_uint32_t * buf, a_uint32_t size);
sw_error_t
cmd_data_check_phy_config(char *cmd_str, void * val, a_uint32_t size);

void
cmd_data_print_phy_config(a_uint8_t * param_name, a_uint32_t * buf, a_uint32_t size);

void cmd_data_print_fdbentry(a_uint8_t * param_name, a_uint32_t * buf, a_uint32_t size);

sw_error_t
cmd_data_check_fdb_smode(char *cmd_str, a_uint32_t * arg_val, a_uint32_t size);

void
cmd_data_print_fdb_smode(a_uint8_t * param_name, a_uint32_t * buf, a_uint32_t size);

void
cmd_data_print_fdb_ctrl_mode(a_uint8_t * param_name, a_uint32_t * buf, a_uint32_t size);
sw_error_t
cmd_data_check_fx100_config(char *cmd_str, void * arg_val, a_uint32_t size);

void
cmd_data_print_fx100_config(a_uint8_t * param_name, a_uint32_t * buf, a_uint32_t size);

sw_error_t
cmd_data_check_multi(char *info, void *val, a_uint32_t size);
void
cmd_data_print_multi(a_uint8_t * param_name, a_uint32_t * buf, a_uint32_t size);

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

sw_error_t
cmd_data_check_remark_entry(char *info, void *val, a_uint32_t size);

void
cmd_data_print_remark_entry(a_uint8_t * param_name, a_uint32_t * buf, a_uint32_t size);

sw_error_t
cmd_data_check_default_route_entry(char *cmd_str, void * val, a_uint32_t size);

sw_error_t
cmd_data_check_u_qmap(char *cmd_str, void * val, a_uint32_t size);

void
cmd_data_print_default_route_entry(a_uint8_t * param_name, a_uint32_t * buf, a_uint32_t size);

sw_error_t
cmd_data_check_host_route_entry(char *cmd_str, void * val, a_uint32_t size);

void
cmd_data_print_host_route_entry(a_uint8_t * param_name, a_uint32_t * buf, a_uint32_t size);

sw_error_t
cmd_data_check_arp_sg(char *cmd_str, void * val, a_uint32_t size);

void
cmd_data_print_arp_sg(a_uint8_t * param_name, a_uint32_t * buf, a_uint32_t size);

sw_error_t
cmd_data_check_intf(char *cmd_str, void * val, a_uint32_t size);

void
cmd_data_print_intf(a_uint8_t * param_name, a_uint32_t * buf, a_uint32_t size);

sw_error_t
cmd_data_check_flow_age(char *cmd_str, void * val, a_uint32_t size);

void
cmd_data_print_flow_age(a_uint8_t * param_name, a_uint32_t * buf, a_uint32_t size);

sw_error_t
cmd_data_check_flow_ctrl(char *cmd_str, void * val, a_uint32_t size);

void
cmd_data_print_flow_ctrl(a_uint8_t * param_name, a_uint32_t * buf, a_uint32_t size);

sw_error_t
cmd_data_check_ac_static_thresh(char *cmd_str, void * val, a_uint32_t size);

void
cmd_data_print_ac_static_thresh(a_uint8_t * param_name, a_uint32_t * buf, a_uint32_t size);

sw_error_t
cmd_data_check_ac_dynamic_thresh(char *cmd_str, void * val, a_uint32_t size);

void
cmd_data_print_ac_dynamic_thresh(a_uint8_t * param_name, a_uint32_t * buf, a_uint32_t size);

sw_error_t
cmd_data_check_ac_group_buff(char *cmd_str, void * val, a_uint32_t size);

void
cmd_data_print_ac_group_buff(a_uint8_t * param_name, a_uint32_t * buf, a_uint32_t size);

sw_error_t
cmd_data_check_ac_ctrl(char *cmd_str, void * val, a_uint32_t size);

void
cmd_data_print_ac_ctrl(a_uint8_t * param_name, a_uint32_t * buf, a_uint32_t size);

sw_error_t
cmd_data_check_ac_obj(char *cmd_str, void * val, a_uint32_t size);

void
cmd_data_print_ac_obj(a_uint8_t * param_name, a_uint32_t * buf, a_uint32_t size);

sw_error_t
cmd_data_check_vsi_intf(char *cmd_str, void * val, a_uint32_t size);

void
cmd_data_print_vsi_intf(a_uint8_t * param_name, a_uint32_t * buf, a_uint32_t size);

sw_error_t
cmd_data_check_ip_pub(char *cmd_str, void * val, a_uint32_t size);

void
cmd_data_print_ip_pub(a_uint8_t * param_name, a_uint32_t * buf, a_uint32_t size);

sw_error_t
cmd_data_check_ip_mcmode(char *cmd_str, void * val, a_uint32_t size);

void
cmd_data_print_ip_mcmode(a_uint8_t * param_name, a_uint32_t * buf, a_uint32_t size);

sw_error_t
cmd_data_check_ip_portmac(char *cmd_str, void * val, a_uint32_t size);

void
cmd_data_print_ip_portmac(a_uint8_t * param_name, a_uint32_t * buf, a_uint32_t size);

sw_error_t
cmd_data_check_ip_sg(char *cmd_str, void * val, a_uint32_t size);

void
cmd_data_print_ip_sg(a_uint8_t * param_name, a_uint32_t * buf, a_uint32_t size);

sw_error_t
cmd_data_check_nexthop(char *cmd_str, void * val, a_uint32_t size);

void
cmd_data_print_nexthop(a_uint8_t * param_name, a_uint32_t * buf, a_uint32_t size);

sw_error_t
cmd_data_check_network_route(char *cmd_str, void * val, a_uint32_t size);

void
cmd_data_print_network_route(a_uint8_t * param_name, a_uint32_t * buf, a_uint32_t size);

sw_error_t
cmd_data_check_ip_wcmp_entry(char *cmd_str, void * val, a_uint32_t size);

void
cmd_data_print_ip_wcmp_entry(a_uint8_t * param_name, a_uint32_t * buf, a_uint32_t size);

sw_error_t
cmd_data_check_ip4_rfs_entry(char *cmd_str, void * val, a_uint32_t size);
sw_error_t
cmd_data_check_ip6_rfs_entry(char *cmd_str, void * val, a_uint32_t size);
sw_error_t
cmd_data_check_flow_age_entry(char *cmd_str, void * val, a_uint32_t size);

void
cmd_data_print_flow_age_entry(a_uint8_t * param_name, a_uint32_t * buf, a_uint32_t size);
sw_error_t
cmd_data_check_flow_ctrl_entry(char *cmd_str, void * val, a_uint32_t size);

void
cmd_data_print_flow_ctrl_entry(a_uint8_t * param_name, a_uint32_t * buf, a_uint32_t size);

sw_error_t
cmd_data_check_flow(char *cmd_str, void * val, a_uint32_t size);

void
cmd_data_print_flow(a_uint8_t * param_name, a_uint32_t * buf, a_uint32_t size);

sw_error_t
cmd_data_check_flow_host(char *cmd_str, void * val, a_uint32_t size);

void
cmd_data_print_flow_host(a_uint8_t * param_name, a_uint32_t * buf, a_uint32_t size);

sw_error_t
cmd_data_check_ip_global(char *cmd_str, void * val, a_uint32_t size);

void
cmd_data_print_ip_global(a_uint8_t * param_name, a_uint32_t * buf, a_uint32_t size);

sw_error_t
cmd_data_check_flow_global(char *cmd_str, void * val, a_uint32_t size);

void
cmd_data_print_flow_global(a_uint8_t * param_name, a_uint32_t * buf, a_uint32_t size);

sw_error_t
cmd_data_check_l3_parser(char *cmd_str, void * val, a_uint32_t size);

void
cmd_data_print_l3_parser(a_uint8_t * param_name, a_uint32_t * buf, a_uint32_t size);

sw_error_t
cmd_data_check_l4_parser(char *cmd_str, void * val, a_uint32_t size);

void
cmd_data_print_l4_parser(a_uint8_t * param_name, a_uint32_t * buf, a_uint32_t size);

sw_error_t
cmd_data_check_exp_ctrl(char *cmd_str, void * val, a_uint32_t size);

void
cmd_data_print_exp_ctrl(a_uint8_t * param_name, a_uint32_t * buf, a_uint32_t size);

sw_error_t
cmd_data_check_port_group(char *cmd_str, void * val, a_uint32_t size);

void
cmd_data_print_port_group(a_uint8_t * param_name, a_uint32_t * buf, a_uint32_t size);

sw_error_t
cmd_data_check_port_pri(char *cmd_str, void * val, a_uint32_t size);

void
cmd_data_print_port_pri(a_uint8_t * param_name, a_uint32_t * buf, a_uint32_t size);

sw_error_t
cmd_data_check_port_remark(char *cmd_str, void * val, a_uint32_t size);

void
cmd_data_print_port_remark(a_uint8_t * param_name, a_uint32_t * buf, a_uint32_t size);

sw_error_t
cmd_data_check_cosmap(char *cmd_str, void * val, a_uint32_t size);

void
cmd_data_print_cosmap(a_uint8_t * param_name, a_uint32_t * buf, a_uint32_t size);

sw_error_t
cmd_data_check_queue_scheduler(char *cmd_str, void * val, a_uint32_t size);

void
cmd_data_print_queue_scheduler(a_uint8_t * param_name, a_uint32_t * buf, a_uint32_t size);

sw_error_t
cmd_data_check_ring_queue(char *cmd_str, void * val, a_uint32_t size);

void
cmd_data_print_ring_queue(a_uint8_t * param_name, a_uint32_t * buf, a_uint32_t size);

sw_error_t
cmd_data_check_bm_static_thresh(char *cmd_str, void * val, a_uint32_t size);

void
cmd_data_print_bm_static_thresh(a_uint8_t * param_name, a_uint32_t * buf, a_uint32_t size);

void
cmd_data_print_queue_cnt(a_uint8_t * param_name, a_uint32_t * buf, a_uint32_t size);

sw_error_t
cmd_data_check_bm_dynamic_thresh(char *cmd_str, void * val, a_uint32_t size);

void
cmd_data_print_bm_dynamic_thresh(a_uint8_t * param_name, a_uint32_t * buf, a_uint32_t size);

void
cmd_data_print_bm_port_counter(a_uint8_t * param_name, a_uint32_t * buf, a_uint32_t size);

sw_error_t
cmd_data_check_flow_cookie(char *cmd_str, void * val, a_uint32_t size);

sw_error_t
cmd_data_check_fdb_rfs(char *cmd_str, void * val, a_uint32_t size);
sw_error_t
cmd_data_check_flow_rfs(char *cmd_str, void * val, a_uint32_t size);
/*qca808x_start*/
sw_error_t
cmd_data_check_crossover_mode(char *cmd_str, a_uint32_t * arg_val, a_uint32_t size);

sw_error_t
cmd_data_check_crossover_status(char *cmd_str, a_uint32_t * arg_val, a_uint32_t size);
/*qca808x_end*/
sw_error_t
cmd_data_check_prefer_medium(char *cmd_str, a_uint32_t * arg_val, a_uint32_t size);

sw_error_t
cmd_data_check_fiber_mode(char *cmd_str, a_uint32_t * arg_val, a_uint32_t size);
/*qca808x_start*/
sw_error_t
cmd_data_check_interface_mode(char *cmd_str, a_uint32_t * arg_val, a_uint32_t size);
/*qca808x_end*/
sw_error_t
cmd_data_check_port_eee_config(char *cmd_str, void * val, a_uint32_t size);

void
cmd_data_print_port_eee_config(a_uint8_t * param_name, a_uint32_t * buf, a_uint32_t size);
sw_error_t
cmd_data_check_src_filter_config(char *cmd_str, a_uint32_t * arg_val, a_uint32_t size);

void
cmd_data_print_src_filter_config(a_uint8_t * param_name, a_uint32_t * buf, a_uint32_t size);

sw_error_t
cmd_data_check_switch_port_loopback_config(char *cmd_str, void * val,
	a_uint32_t size);
void
cmd_data_print_switch_port_loopback_config(a_uint8_t * param_name,
	a_uint32_t * buf, a_uint32_t size);
sw_error_t
cmd_data_check_newadr_lrn(char *cmd_str, void * val, a_uint32_t size);

void
cmd_data_print_newaddr_lrn_entry(a_uint8_t * param_name, a_uint32_t * buf, a_uint32_t size);

sw_error_t
cmd_data_check_stamove(char *cmd_str, void * val, a_uint32_t size);

void
cmd_data_print_stamove_entry(a_uint8_t * param_name, a_uint32_t * buf, a_uint32_t size);

sw_error_t
cmd_data_check_vsi_member(char *cmd_str, void * val, a_uint32_t size);

void
cmd_data_print_vsi_member_entry(a_uint8_t * param_name, a_uint32_t * buf, a_uint32_t size);

void
cmd_data_print_vsi_counter(a_uint8_t * param_name, a_uint32_t * buf, a_uint32_t size);

sw_error_t
cmd_data_check_mtu_entry(char *cmd_str, void * val, a_uint32_t size);

sw_error_t
cmd_data_check_mru_entry(char *cmd_str, void * val, a_uint32_t size);
/*qca808x_start*/
void
cmd_data_print_crossover_mode(a_uint8_t * param_name, a_uint32_t * buf, a_uint32_t size);

void
cmd_data_print_crossover_status(a_uint8_t * param_name, a_uint32_t * buf, a_uint32_t size);
/*qca808x_end*/

void
cmd_data_print_prefer_medium(a_uint8_t * param_name, a_uint32_t * buf, a_uint32_t size);

void
cmd_data_print_fiber_mode(a_uint8_t * param_name, a_uint32_t * buf, a_uint32_t size);
/*qca808x_start*/

void
cmd_data_print_interface_mode(a_uint8_t * param_name, a_uint32_t * buf, a_uint32_t size);

void
cmd_data_print_counter_info(a_uint8_t * param_name, a_uint32_t * buf, a_uint32_t size);

void
cmd_data_print_register_info(a_uint8_t * param_name, a_uint32_t * buf, a_uint32_t size);

void
cmd_data_print_phy_register_info(a_uint8_t * param_name, a_uint32_t * buf, a_uint32_t size);

void
cmd_data_print_debug_register_info(a_uint8_t * param_name, a_uint32_t * buf, a_uint32_t size);
/*qca808x_end*/

void
cmd_data_print_mtu_info(a_uint8_t * param_name, a_uint32_t * buf, a_uint32_t size);

void
cmd_data_print_mru_info(a_uint8_t * param_name, a_uint32_t * buf, a_uint32_t size);

sw_error_t
cmd_data_check_global_qinqmode(char *info, void *val, a_uint32_t size);

void
cmd_data_print_global_qinqmode(a_uint8_t * param_name, a_uint32_t * buf, a_uint32_t size);

sw_error_t
cmd_data_check_port_qinqmode(char *info, void *val, a_uint32_t size);

void
cmd_data_print_port_qinqmode(a_uint8_t * param_name, a_uint32_t * buf, a_uint32_t size);

sw_error_t
cmd_data_check_tpid(char *info, void *val, a_uint32_t size);

void
cmd_data_print_tpid(a_uint8_t * param_name, a_uint32_t * buf, a_uint32_t size);

sw_error_t
cmd_data_check_ingress_filter(char *info, void *val, a_uint32_t size);

void
cmd_data_print_ingress_filter(a_uint8_t * param_name, a_uint32_t * buf, a_uint32_t size);

sw_error_t
cmd_data_check_port_default_vid_en(char *info, void *val, a_uint32_t size);

void
cmd_data_print_port_default_vid_en(a_uint8_t * param_name, a_uint32_t * buf, a_uint32_t size);

sw_error_t
cmd_data_check_port_vlan_tag(char *info, void *val, a_uint32_t size);

void
cmd_data_print_port_vlan_tag(a_uint8_t * param_name, a_uint32_t * buf, a_uint32_t size);

sw_error_t
cmd_data_check_port_vlan_direction(char *cmd_str, a_uint32_t * arg_val, a_uint32_t size);

void
cmd_data_print_port_vlan_direction(a_uint8_t * param_name, a_uint32_t * buf, a_uint32_t size);

sw_error_t
cmd_data_check_port_vlan_translation_adv_rule(char *info, fal_vlan_trans_adv_rule_t *val, a_uint32_t size);

void
cmd_data_print_port_vlan_translation_adv_rule(a_uint8_t * param_name, a_uint32_t * buf, a_uint32_t size);

sw_error_t
cmd_data_check_port_vlan_translation_adv_action(char *info, fal_vlan_trans_adv_action_t *val, a_uint32_t size);

void
cmd_data_print_port_vlan_translation_adv_action(a_uint8_t * param_name, a_uint32_t * buf, a_uint32_t size);

void
cmd_data_print_port_vlan_counter(a_uint8_t * param_name, a_uint32_t * buf, a_uint32_t size);

sw_error_t
cmd_data_check_tag_propagation(char *info, void *val, a_uint32_t size);

void
cmd_data_print_tag_propagation(a_uint8_t * param_name, a_uint32_t * buf, a_uint32_t size);

sw_error_t
cmd_data_check_egress_vsi_tag(char *info, void *val, a_uint32_t size);

void
cmd_data_print_egress_vsi_tag(a_uint8_t * param_name, a_uint32_t * buf, a_uint32_t size);

sw_error_t
cmd_data_check_egress_mode(char *info, void *val, a_uint32_t size);

void
cmd_data_print_egress_mode(a_uint8_t * param_name, a_uint32_t * buf, a_uint32_t size);

sw_error_t
cmd_data_check_ctrlpkt_profile(char *info, void *val, a_uint32_t size);

void
cmd_data_print_ctrlpkt_profile(a_uint8_t * param_name, a_uint32_t * buf, a_uint32_t size);

sw_error_t
cmd_data_check_servcode_config(char *info, fal_servcode_config_t *val, a_uint32_t size);

void
cmd_data_print_servcode_config(a_uint8_t * param_name, a_uint32_t * buf, a_uint32_t size);

sw_error_t
cmd_data_check_rss_hash_mode(char *cmd_str, a_uint32_t * arg_val, a_uint32_t size);

sw_error_t
cmd_data_check_rss_hash_config(char *info, fal_rss_hash_config_t *val, a_uint32_t size);

void
cmd_data_print_rss_hash_config(a_uint8_t * param_name, a_uint32_t * buf, a_uint32_t size);

sw_error_t
cmd_data_check_port_policer_config(char *cmd_str, void * val, a_uint32_t size);

sw_error_t
cmd_data_check_policer_cmd_config(char *cmd_str, void * val, a_uint32_t size);

void
cmd_data_print_port_policer_config(a_uint8_t * param_name, a_uint32_t * buf, a_uint32_t size);

void
cmd_data_print_policer_cmd_config(a_uint8_t * param_name, a_uint32_t * buf, a_uint32_t size);

sw_error_t
cmd_data_check_acl_policer_config(char *cmd_str, void * val, a_uint32_t size);

void
cmd_data_print_acl_policer_config(a_uint8_t * param_name, a_uint32_t * buf, a_uint32_t size);

void
cmd_data_print_policer_counter_infor(a_uint8_t * param_name, a_uint32_t * buf, a_uint32_t size);

void
cmd_data_print_policer_global_counter_infor(a_uint8_t * param_name, a_uint32_t * buf, a_uint32_t size);

void
cmd_data_print_port_scheduler_resource(a_uint8_t * param_name, a_uint32_t * buf, a_uint32_t size);

sw_error_t
cmd_data_check_port_shaper_token_config(char *cmd_str, void * val, a_uint32_t size);

sw_error_t
cmd_data_check_shaper_token_config(char *cmd_str, void * val, a_uint32_t size);

sw_error_t
cmd_data_check_port_shaper_config(char *cmd_str, void * val, a_uint32_t size);

sw_error_t
cmd_data_check_shaper_config(char *cmd_str, void * val, a_uint32_t size);

void
cmd_data_print_port_shaper_token_config(a_uint8_t * param_name, a_uint32_t * buf, a_uint32_t size);

void
cmd_data_print_shaper_token_config(a_uint8_t * param_name, a_uint32_t * buf, a_uint32_t size);

void
cmd_data_print_port_shaper_config(a_uint8_t * param_name, a_uint32_t * buf, a_uint32_t size);

void
cmd_data_print_shaper_config(a_uint8_t * param_name, a_uint32_t * buf, a_uint32_t size);

sw_error_t
cmd_data_check_module(char *cmd_str, a_uint32_t * arg_val, a_uint32_t size);

void
cmd_data_print_module(a_uint8_t * param_name, a_uint32_t * buf, a_uint32_t size);

sw_error_t
cmd_data_check_func_ctrl(char *cmd_str, void * val, a_uint32_t size);

void
cmd_data_print_func_ctrl(a_uint8_t * param_name, a_uint32_t * buf, a_uint32_t size);

void
cmd_data_print_module_func_ctrl(a_uint32_t module, fal_func_ctrl_t *p);

sw_error_t
cmd_data_check_debug_port_counter_status(char *info, fal_counter_en_t *val, a_uint32_t size);

void
cmd_data_print_debug_port_counter_status(a_uint8_t * param_name, a_uint32_t * buf, a_uint32_t size);

sw_error_t
cmd_data_check_ptp_config(char *info, void *val, a_uint32_t size);

void
cmd_data_print_ptp_config(a_uint8_t * param_name, a_uint32_t * buf, a_uint32_t size);

sw_error_t
cmd_data_check_ptp_reference_clock(char *info, void *val, a_uint32_t size);

void
cmd_data_print_ptp_reference_clock(a_uint8_t * param_name, a_uint32_t * buf, a_uint32_t size);

sw_error_t
cmd_data_check_ptp_rx_timestamp_mode(char *info, void *val, a_uint32_t size);

void
cmd_data_print_ptp_rx_timestamp_mode(a_uint8_t * param_name, a_uint32_t * buf, a_uint32_t size);

sw_error_t
cmd_data_check_ptp_direction(char *info, void *val, a_uint32_t size);

sw_error_t
cmd_data_check_ptp_pkt_info(char *info, void *val, a_uint32_t size);

void
cmd_data_print_ptp_pkt_info(a_uint8_t * param_name, a_uint32_t * buf, a_uint32_t size);

sw_error_t
cmd_data_check_ptp_time(char *info, void *val, a_uint32_t size);

void
cmd_data_print_ptp_time(a_char_t * param_name, a_uint32_t * buf, a_uint32_t size);

sw_error_t
cmd_data_check_ptp_grandmaster_mode(char *info, void *val, a_uint32_t size);

void
cmd_data_print_ptp_grandmaster_mode(a_uint8_t * param_name, a_uint32_t * buf, a_uint32_t size);

sw_error_t
cmd_data_check_ptp_security(char *info, void *val, a_uint32_t size);

void
cmd_data_print_ptp_security(a_uint8_t * param_name, a_uint32_t * buf, a_uint32_t size);

sw_error_t
cmd_data_check_ptp_pps_sig_ctrl(char *info, void *val, a_uint32_t size);

void
cmd_data_print_ptp_pps_sig_ctrl(a_uint8_t * param_name, a_uint32_t * buf, a_uint32_t size);

sw_error_t
cmd_data_check_ptp_asym_correction(char *info, void *val, a_uint32_t size);

void
cmd_data_print_ptp_asym_correction(a_uint8_t * param_name, a_uint32_t * buf, a_uint32_t size);

sw_error_t
cmd_data_check_ptp_waveform(char *info, void *val, a_uint32_t size);

void
cmd_data_print_ptp_waveform(a_uint8_t * param_name, a_uint32_t * buf, a_uint32_t size);

sw_error_t
cmd_data_check_ptp_tod_uart(char *info, void *val, a_uint32_t size);

void
cmd_data_print_ptp_tod_uart(a_uint8_t * param_name, a_uint32_t * buf, a_uint32_t size);

sw_error_t
cmd_data_check_ptp_enhanced_timestamp_engine(char *info, void *val, a_uint32_t size);

void
cmd_data_print_ptp_enhanced_timestamp_engine(a_uint8_t * param_name, a_uint32_t * buf,
		a_uint32_t size);

sw_error_t
cmd_data_check_ptp_trigger(char *info, void *val, a_uint32_t size);

void
cmd_data_print_ptp_trigger(a_uint8_t * param_name, a_uint32_t * buf, a_uint32_t size);

sw_error_t
cmd_data_check_ptp_capture(char *info, void *val, a_uint32_t size);

void
cmd_data_print_ptp_capture(a_uint8_t * param_name, a_uint32_t * buf, a_uint32_t size);

sw_error_t
cmd_data_check_ptp_interrupt(char *info, void *val, a_uint32_t size);

void
cmd_data_print_ptp_interrupt(a_uint8_t * param_name, a_uint32_t * buf, a_uint32_t size);

sw_error_t
cmd_data_check_sfp_ccode_type(char *cmdstr, fal_sfp_cc_type_t *arg_val, a_uint32_t size);

sw_error_t
cmd_data_check_sfp_data(char *cmd_str, void *arg_val, a_uint32_t size);

void
cmd_data_print_sfp_data(a_uint8_t *param_name, a_ulong_t *buf, a_uint32_t size);

void
cmd_data_print_sfp_dev_type(a_uint8_t *param_name, a_ulong_t *buf, a_uint32_t size);

void
cmd_data_print_sfp_transc_code(a_uint8_t *param_name, a_ulong_t *buf, a_uint32_t size);

void
cmd_data_print_sfp_rate_encode(a_uint8_t *param_name, a_ulong_t *buf, a_uint32_t size);

void
cmd_data_print_sfp_link_length(a_uint8_t *param_name, a_ulong_t *buf, a_uint32_t size);

void
cmd_data_print_sfp_vendor_info(a_uint8_t *param_name, a_ulong_t *buf, a_uint32_t size);

void
cmd_data_print_sfp_laser_wavelength(a_uint8_t *param_name, a_ulong_t *buf, a_uint32_t size);

void
cmd_data_print_sfp_option(a_uint8_t *param_name, a_ulong_t *buf, a_uint32_t size);

void
cmd_data_print_sfp_ctrl_rate(a_uint8_t *param_name, a_ulong_t *buf, a_uint32_t size);

void
cmd_data_print_sfp_enhanced_cfg(a_uint8_t *param_name, a_ulong_t *buf, a_uint32_t size);

void
cmd_data_print_sfp_diag_threshold(a_uint8_t *param_name, a_ulong_t *buf, a_uint32_t size);

void
cmd_data_print_sfp_diag_cal_const(a_uint8_t *param_name, a_ulong_t *buf, a_uint32_t size);

void
cmd_data_print_sfp_diag_realtime(a_uint8_t *param_name, a_ulong_t *buf, a_uint32_t size);

void
cmd_data_print_sfp_ctrl_status(a_uint8_t *param_name, a_ulong_t *buf, a_uint32_t size);

void
cmd_data_print_sfp_alarm_warn_flag(a_uint8_t *param_name, a_ulong_t *buf, a_uint32_t size);
/*qca808x_start*/
#endif
/*qca808x_end*/

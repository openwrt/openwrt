/*
 * Copyright (c) 2014-2020, The Linux Foundation. All rights reserved.
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
#include <stdio.h>
#include "shell_io.h"
#include "shell.h"

#define SW_RTN_ON_NULL_PARAM(rtn) \
    do { if ((rtn) == NULL) return SW_BAD_PARAM; } while(0);

#define DEFAULT_FLAG "default"
static char **full_cmdstrp;
static int talk_mode = 1;

char g_aclcmd[500] = "\0";
a_uint32_t g_aclcmd_len = 0;;

void append_acl_cmd(char * cmd)
{
	if(500 > (g_aclcmd_len+1)) {
		g_aclcmd_len += snprintf(g_aclcmd+g_aclcmd_len, 500-g_aclcmd_len, "%s", cmd);
		g_aclcmd_len += snprintf(g_aclcmd+g_aclcmd_len, 500-g_aclcmd_len, "%s", " ");
	}
}

int
get_talk_mode(void)
{
    return talk_mode ;
}

void
set_talk_mode(int mode)
{
    talk_mode = mode;
}

char ** full_cmdstrp_bak;

void
set_full_cmdstrp(char **cmdstrp)
{
    full_cmdstrp = cmdstrp;
    full_cmdstrp_bak = cmdstrp;
}

int
get_jump(void)
{
    return (full_cmdstrp-full_cmdstrp_bak);
}

static char *
get_cmd_buf(char *tag, char *defval)
{
    if(!full_cmdstrp || !(*full_cmdstrp))
    {
        dprintf("parameter (%s) or default (%s) absent\n", tag, defval);
        exit(1);
    }

    if (!strcasecmp(*(full_cmdstrp), DEFAULT_FLAG))
    {
        full_cmdstrp++;
        return defval;
    }
    else
    {
        return *(full_cmdstrp++);
    }
}

static char *
get_cmd_stdin(char *tag, char *defval)
{
    static char gsubcmdstr[256];
    int pos = 0;
    int c;

    if(defval)
    {
        dprintf("%s(%s): ", tag, defval);
    }
    else
    {
        dprintf("%s: ", tag);
    }

    fflush(stdout);
    memset(gsubcmdstr, 0, sizeof(gsubcmdstr));

    while ((c = getchar()) != '\n')
    {
        gsubcmdstr[pos++] = c;
        if (pos == (sizeof(gsubcmdstr) - 1))
        {
            dprintf("too long command\n");
            return NULL;
        }
    }

    gsubcmdstr[pos] = '\0';
    if ('\0' == gsubcmdstr[0])
    {
        return defval;
    }
    else
    {
        return gsubcmdstr;
    }
}

static char *
get_sub_cmd(char *tag, char *defval)
{
    if(talk_mode)
        return get_cmd_stdin(tag, defval);
    else
        return get_cmd_buf(tag, defval);
}


static inline  a_bool_t
is_hex(char c)
{
    if ((c >= '0' && c <= '9') || (c >= 'a' && c <= 'f')
            || (c >= 'A' && c <= 'F'))
        return A_TRUE;

    return A_FALSE;
}

static inline a_bool_t
is_dec(char c)
{
    if ((c >= '0') && (c <= '9'))
        return A_TRUE;

    return A_FALSE;
}

static sw_data_type_t sw_data_type[] =
{
    SW_TYPE_DEF(SW_UINT8, cmd_data_check_uint8, cmd_data_print_uint8),
    SW_TYPE_DEF(SW_INT8, NULL, NULL),
    SW_TYPE_DEF(SW_UINT16, cmd_data_check_uint16, cmd_data_print_uint16),
    SW_TYPE_DEF(SW_INT16, NULL, NULL),
    SW_TYPE_DEF(SW_UINT32, cmd_data_check_uint32, cmd_data_print_uint32),
    SW_TYPE_DEF(SW_INT32, NULL, NULL),
    SW_TYPE_DEF(SW_UINT64, cmd_data_check_uint64, cmd_data_print_uint64),
    SW_TYPE_DEF(SW_INT64, NULL, NULL),
    SW_TYPE_DEF(SW_CAP, cmd_data_check_capable, cmd_data_print_capable),
    SW_TYPE_DEF(SW_DUPLEX, cmd_data_check_duplex, cmd_data_print_duplex),
    SW_TYPE_DEF(SW_SPEED, cmd_data_check_speed, cmd_data_print_speed),
/*qca808x_end*/
    SW_TYPE_DEF(SW_1QMODE, cmd_data_check_1qmode, cmd_data_print_1qmode),
    SW_TYPE_DEF(SW_EGMODE, cmd_data_check_egmode, cmd_data_print_egmode),
    SW_TYPE_DEF(SW_MIB, NULL, cmd_data_print_mib),
     SW_TYPE_DEF(SW_XGMIB, NULL, cmd_data_print_xgmib),
    SW_TYPE_DEF(SW_MIB_CNTR, NULL, cmd_data_print_mib_cntr),
    SW_TYPE_DEF(SW_VLAN, cmd_data_check_vlan, cmd_data_print_vlan),
    SW_TYPE_DEF(SW_LAN_WAN_CFG, cmd_data_check_lan_wan_cfg, cmd_data_print_lan_wan_cfg),
/*qca808x_start*/
    SW_TYPE_DEF(SW_PBMP, cmd_data_check_pbmp, cmd_data_print_pbmp),
    SW_TYPE_DEF(SW_ENABLE, cmd_data_check_enable, cmd_data_print_enable),
    SW_TYPE_DEF(SW_MACADDR, cmd_data_check_macaddr, cmd_data_print_macaddr),
/*qca808x_end*/
    SW_TYPE_DEF(SW_FDBENTRY, cmd_data_check_fdbentry, cmd_data_print_fdbentry),
    SW_TYPE_DEF(SW_MACLIMIT_CTRL, cmd_data_check_maclimit_ctrl, cmd_data_print_maclimit_ctrl),
    SW_TYPE_DEF(SW_SCH, cmd_data_check_qos_sch, cmd_data_print_qos_sch),
    SW_TYPE_DEF(SW_QOS, cmd_data_check_qos_pt, cmd_data_print_qos_pt),
    SW_TYPE_DEF(SW_STORM, cmd_data_check_storm, cmd_data_print_storm),
    SW_TYPE_DEF(SW_STP, cmd_data_check_stp_state, cmd_data_print_stp_state),
    SW_TYPE_DEF(SW_LEAKY, cmd_data_check_leaky, cmd_data_print_leaky),
    SW_TYPE_DEF(SW_MACCMD, cmd_data_check_maccmd, cmd_data_print_maccmd),
    SW_TYPE_DEF(SW_FLOWCMD, cmd_data_check_flowcmd, cmd_data_print_flowcmd),
    SW_TYPE_DEF(SW_FLOWTYPE, cmd_data_check_flowtype, cmd_data_print_flowtype),
    SW_TYPE_DEF(SW_UINT_A, cmd_data_check_uinta, cmd_data_print_uinta),
    SW_TYPE_DEF(SW_ACLRULE, cmd_data_check_aclrule, cmd_data_print_aclrule),
    SW_TYPE_DEF(SW_LEDPATTERN, cmd_data_check_ledpattern, cmd_data_print_ledpattern),
    SW_TYPE_DEF(SW_MIRR_ANALYSIS_CONFIG, cmd_data_check_mirr_analy_cfg, cmd_data_print_mirr_analy_cfg),
    SW_TYPE_DEF(SW_MIRR_DIRECTION, cmd_data_check_mirr_direction, cmd_data_print_mirr_direction),
    SW_TYPE_DEF(SW_INVLAN, cmd_data_check_invlan_mode, cmd_data_print_invlan_mode),
    SW_TYPE_DEF(SW_VLANPROPAGATION, cmd_data_check_vlan_propagation, cmd_data_print_vlan_propagation),
    SW_TYPE_DEF(SW_VLANTRANSLATION, cmd_data_check_vlan_translation, cmd_data_print_vlan_translation),
    SW_TYPE_DEF(SW_QINQMODE, cmd_data_check_qinq_mode, cmd_data_print_qinq_mode),
    SW_TYPE_DEF(SW_QINQROLE, cmd_data_check_qinq_role, cmd_data_print_qinq_role),
/*qca808x_start*/
    SW_TYPE_DEF(SW_CABLESTATUS, NULL, cmd_data_print_cable_status),
    SW_TYPE_DEF(SW_CABLELEN, NULL, cmd_data_print_cable_len),
    SW_TYPE_DEF(SW_SSDK_CFG, NULL, cmd_data_print_ssdk_cfg),
/*qca808x_end*/
    SW_TYPE_DEF(SW_HDRMODE, cmd_data_check_hdrmode, cmd_data_print_hdrmode),
    SW_TYPE_DEF(SW_FDBOPRATION, cmd_data_check_fdboperation, NULL),
    SW_TYPE_DEF(SW_PPPOE, cmd_data_check_pppoe, cmd_data_print_pppoe),
    SW_TYPE_DEF(SW_PPPOE_LESS, cmd_data_check_pppoe_less, cmd_data_print_pppoe),
    SW_TYPE_DEF(SW_ACL_UDF_TYPE, cmd_data_check_udf_type, cmd_data_print_udf_type),
    SW_TYPE_DEF(SW_IP_HOSTENTRY, cmd_data_check_host_entry, cmd_data_print_host_entry),
    SW_TYPE_DEF(SW_ARP_LEARNMODE, cmd_data_check_arp_learn_mode, cmd_data_print_arp_learn_mode),
    SW_TYPE_DEF(SW_IP_GUARDMODE, cmd_data_check_ip_guard_mode, cmd_data_print_ip_guard_mode),
    SW_TYPE_DEF(SW_NATENTRY, cmd_data_check_nat_entry, cmd_data_print_nat_entry),
    SW_TYPE_DEF(SW_NAPTENTRY, cmd_data_check_napt_entry, cmd_data_print_napt_entry),
    SW_TYPE_DEF(SW_FLOWENTRY, cmd_data_check_flow_entry, cmd_data_print_flow_entry),
    SW_TYPE_DEF(SW_NAPTMODE, cmd_data_check_napt_mode, cmd_data_print_napt_mode),
    SW_TYPE_DEF(SW_IP4ADDR, cmd_data_check_ip4addr, cmd_data_print_ip4addr),
    SW_TYPE_DEF(SW_IP6ADDR, cmd_data_check_ip6addr, cmd_data_print_ip6addr),
    SW_TYPE_DEF(SW_INTFMACENTRY, cmd_data_check_intf_mac_entry, cmd_data_print_intf_mac_entry),
    SW_TYPE_DEF(SW_PUBADDRENTRY, cmd_data_check_pub_addr_entry, cmd_data_print_pub_addr_entry),
    SW_TYPE_DEF(SW_INGPOLICER, cmd_data_check_port_policer, cmd_data_print_port_policer),
    SW_TYPE_DEF(SW_EGSHAPER, cmd_data_check_egress_shaper, cmd_data_print_egress_shaper),
    SW_TYPE_DEF(SW_ACLPOLICER, cmd_data_check_acl_policer, cmd_data_print_acl_policer),
    SW_TYPE_DEF(SW_MACCONFIG, cmd_data_check_mac_config, cmd_data_print_mac_config),
    SW_TYPE_DEF(SW_PHYCONFIG, cmd_data_check_phy_config, cmd_data_print_phy_config),
    SW_TYPE_DEF(SW_FDBSMODE, cmd_data_check_fdb_smode, cmd_data_print_fdb_smode),
    SW_TYPE_DEF(SW_FDB_CTRL_MODE, NULL, cmd_data_print_fdb_ctrl_mode),
    SW_TYPE_DEF(SW_FX100CONFIG, cmd_data_check_fx100_config, cmd_data_print_fx100_config),
    SW_TYPE_DEF(SW_SGENTRY, cmd_data_check_multi, cmd_data_print_multi),
    SW_TYPE_DEF(SW_SEC_MAC, cmd_data_check_sec_mac, NULL),
    SW_TYPE_DEF(SW_SEC_IP, cmd_data_check_sec_ip, NULL),
    SW_TYPE_DEF(SW_SEC_IP4, cmd_data_check_sec_ip4, NULL),
    SW_TYPE_DEF(SW_SEC_IP6, cmd_data_check_sec_ip6, NULL),
    SW_TYPE_DEF(SW_SEC_TCP, cmd_data_check_sec_tcp, NULL),
    SW_TYPE_DEF(SW_SEC_UDP, cmd_data_check_sec_udp, NULL),
    SW_TYPE_DEF(SW_SEC_ICMP4, cmd_data_check_sec_icmp4, NULL),
    SW_TYPE_DEF(SW_SEC_ICMP6, cmd_data_check_sec_icmp6, NULL),
    SW_TYPE_DEF(SW_REMARKENTRY, cmd_data_check_remark_entry, cmd_data_print_remark_entry),
    SW_TYPE_DEF(SW_DEFAULT_ROUTE_ENTRY, cmd_data_check_default_route_entry, cmd_data_print_default_route_entry),
    SW_TYPE_DEF(SW_HOST_ROUTE_ENTRY, cmd_data_check_host_route_entry, cmd_data_print_host_route_entry),
    SW_TYPE_DEF(SW_IP_WCMP_ENTRY, cmd_data_check_ip_wcmp_entry, cmd_data_print_ip_wcmp_entry),
    SW_TYPE_DEF(SW_IP_RFS_IP4, cmd_data_check_ip4_rfs_entry, NULL),
	SW_TYPE_DEF(SW_IP_RFS_IP6, cmd_data_check_ip6_rfs_entry, NULL),
	SW_TYPE_DEF(SW_FLOWCOOKIE, cmd_data_check_flow_cookie, NULL),
	SW_TYPE_DEF(SW_FLOWRFS, cmd_data_check_flow_rfs, NULL),
	SW_TYPE_DEF(SW_FDB_RFS, cmd_data_check_fdb_rfs, NULL),
/*qca808x_start*/
	SW_TYPE_DEF(SW_CROSSOVER_MODE, cmd_data_check_crossover_mode, cmd_data_print_crossover_mode),
    SW_TYPE_DEF(SW_CROSSOVER_STATUS, cmd_data_check_crossover_status, cmd_data_print_crossover_status),
/*qca808x_end*/
    SW_TYPE_DEF(SW_PORT_EEE_CONFIG, cmd_data_check_port_eee_config, cmd_data_print_port_eee_config),
    SW_TYPE_DEF(SW_PREFER_MEDIUM, cmd_data_check_prefer_medium, cmd_data_print_prefer_medium),
    SW_TYPE_DEF(SW_FIBER_MODE, cmd_data_check_fiber_mode, cmd_data_print_fiber_mode),
    SW_TYPE_DEF(SW_SRC_FILTER_CONFIG, cmd_data_check_src_filter_config, cmd_data_print_src_filter_config),
    SW_TYPE_DEF(SW_PORT_LOOPBACK_CONFIG, cmd_data_check_switch_port_loopback_config, cmd_data_print_switch_port_loopback_config),
/*qca808x_start*/
    SW_TYPE_DEF(SW_INTERFACE_MODE, cmd_data_check_interface_mode, cmd_data_print_interface_mode),
    SW_TYPE_DEF(SW_COUNTER_INFO, NULL, cmd_data_print_counter_info),
    SW_TYPE_DEF(SW_REG_DUMP, NULL, cmd_data_print_register_info),
    SW_TYPE_DEF(SW_PHY_DUMP, NULL, cmd_data_print_phy_register_info),
    SW_TYPE_DEF(SW_DBG_REG_DUMP, NULL, cmd_data_print_debug_register_info),
/*qca808x_end*/
    SW_TYPE_DEF(SW_VSI_NEWADDR_LRN, cmd_data_check_newadr_lrn, cmd_data_print_newaddr_lrn_entry),
    SW_TYPE_DEF(SW_VSI_STAMOVE, cmd_data_check_stamove, cmd_data_print_stamove_entry),
    SW_TYPE_DEF(SW_VSI_MEMBER, cmd_data_check_vsi_member, cmd_data_print_vsi_member_entry),
    SW_TYPE_DEF(SW_VSI_COUNTER, NULL, cmd_data_print_vsi_counter),
    SW_TYPE_DEF(SW_MTU_INFO, NULL, cmd_data_print_mtu_info),
    SW_TYPE_DEF(SW_MRU_INFO, NULL, cmd_data_print_mru_info),
    SW_TYPE_DEF(SW_MTU_ENTRY, cmd_data_check_mtu_entry, NULL),
    SW_TYPE_DEF(SW_MRU_ENTRY, cmd_data_check_mru_entry, NULL),
    SW_TYPE_DEF(SW_ARP_SG_CFG, cmd_data_check_arp_sg, cmd_data_print_arp_sg),
    SW_TYPE_DEF(SW_IP_NETWORK_ROUTE, cmd_data_check_network_route, cmd_data_print_network_route),
    SW_TYPE_DEF(SW_IP_INTF, cmd_data_check_intf, cmd_data_print_intf),
    SW_TYPE_DEF(SW_IP_VSI_INTF, cmd_data_check_vsi_intf, cmd_data_print_vsi_intf),
    SW_TYPE_DEF(SW_IP_NEXTHOP, cmd_data_check_nexthop, cmd_data_print_nexthop),
    SW_TYPE_DEF(SW_UCAST_QUEUE_MAP, cmd_data_check_u_qmap, NULL),
    SW_TYPE_DEF(SW_IP_SG, cmd_data_check_ip_sg, cmd_data_print_ip_sg),
    SW_TYPE_DEF(SW_IP_PUB, cmd_data_check_ip_pub, cmd_data_print_ip_pub),
    SW_TYPE_DEF(SW_IP_PORTMAC, cmd_data_check_ip_portmac, cmd_data_print_ip_portmac),
    SW_TYPE_DEF(SW_IP_MCMODE, cmd_data_check_ip_mcmode, cmd_data_print_ip_mcmode),
    SW_TYPE_DEF(SW_FLOW_AGE, cmd_data_check_flow_age, cmd_data_print_flow_age),
    SW_TYPE_DEF(SW_FLOW_CTRL, cmd_data_check_flow_ctrl, cmd_data_print_flow_ctrl),
    SW_TYPE_DEF(SW_STATIC_THRESH, cmd_data_check_ac_static_thresh, cmd_data_print_ac_static_thresh),
    SW_TYPE_DEF(SW_DYNAMIC_THRESH, cmd_data_check_ac_dynamic_thresh, cmd_data_print_ac_dynamic_thresh),
    SW_TYPE_DEF(SW_GROUP_BUFFER, cmd_data_check_ac_group_buff, cmd_data_print_ac_group_buff),
    SW_TYPE_DEF(SW_AC_CTRL, cmd_data_check_ac_ctrl, cmd_data_print_ac_ctrl),
    SW_TYPE_DEF(SW_AC_OBJ, cmd_data_check_ac_obj, cmd_data_print_ac_obj),
    SW_TYPE_DEF(SW_FLOW_ENTRY, cmd_data_check_flow, cmd_data_print_flow),
    SW_TYPE_DEF(SW_FLOW_HOST, cmd_data_check_flow_host, cmd_data_print_flow_host),
    SW_TYPE_DEF(SW_IP_GLOBAL, cmd_data_check_ip_global, cmd_data_print_ip_global),
    SW_TYPE_DEF(SW_FLOW_GLOBAL, cmd_data_check_flow_global, cmd_data_print_flow_global),
    SW_TYPE_DEF(SW_GLOBAL_QINQMODE, cmd_data_check_global_qinqmode, cmd_data_print_global_qinqmode),
    SW_TYPE_DEF(SW_PT_QINQMODE, cmd_data_check_port_qinqmode, cmd_data_print_port_qinqmode),
    SW_TYPE_DEF(SW_TPID, cmd_data_check_tpid, cmd_data_print_tpid),
    SW_TYPE_DEF(SW_INGRESS_FILTER, cmd_data_check_ingress_filter, cmd_data_print_ingress_filter),
    SW_TYPE_DEF(SW_PT_DEF_VID_EN, cmd_data_check_port_default_vid_en, cmd_data_print_port_default_vid_en),
    SW_TYPE_DEF(SW_PT_VLAN_TAG, cmd_data_check_port_vlan_tag, cmd_data_print_port_vlan_tag),
    SW_TYPE_DEF(SW_PT_VLAN_DIRECTION, cmd_data_check_port_vlan_direction, cmd_data_print_port_vlan_direction),
    SW_TYPE_DEF(SW_PT_VLAN_TRANS_ADV_RULE, cmd_data_check_port_vlan_translation_adv_rule, cmd_data_print_port_vlan_translation_adv_rule),
    SW_TYPE_DEF(SW_PT_VLAN_TRANS_ADV_ACTION, cmd_data_check_port_vlan_translation_adv_action, cmd_data_print_port_vlan_translation_adv_action),
    SW_TYPE_DEF(SW_PT_VLAN_COUNTER, NULL, cmd_data_print_port_vlan_counter),
    SW_TYPE_DEF(SW_DEBUG_COUNTER_EN, cmd_data_check_debug_port_counter_status, cmd_data_print_debug_port_counter_status),
    SW_TYPE_DEF(SW_TAG_PROPAGATION, cmd_data_check_tag_propagation, cmd_data_print_tag_propagation),
    SW_TYPE_DEF(SW_EGRESS_MODE, cmd_data_check_egress_mode, cmd_data_print_egress_mode),
    SW_TYPE_DEF(SW_CTRLPKT_PROFILE, cmd_data_check_ctrlpkt_profile, cmd_data_print_ctrlpkt_profile),
    SW_TYPE_DEF(SW_SERVCODE_CONFIG, cmd_data_check_servcode_config, cmd_data_print_servcode_config),
    SW_TYPE_DEF(SW_RSS_HASH_MODE, cmd_data_check_rss_hash_mode, NULL),
    SW_TYPE_DEF(SW_RSS_HASH_CONFIG, cmd_data_check_rss_hash_config, cmd_data_print_rss_hash_config),
    SW_TYPE_DEF(SW_L3_PARSER, cmd_data_check_l3_parser, cmd_data_print_l3_parser),
    SW_TYPE_DEF(SW_L4_PARSER, cmd_data_check_l4_parser, cmd_data_print_l4_parser),
    SW_TYPE_DEF(SW_EXP_CTRL, cmd_data_check_exp_ctrl, cmd_data_print_exp_ctrl),
    SW_TYPE_DEF(SW_ACL_UDF_PKT_TYPE, cmd_data_check_udf_pkt_type, cmd_data_print_udf_pkt_type),
    SW_TYPE_DEF(SW_PORTGROUP, cmd_data_check_port_group, cmd_data_print_port_group),
    SW_TYPE_DEF(SW_PORTPRI, cmd_data_check_port_pri, cmd_data_print_port_pri),
    SW_TYPE_DEF(SW_PORTREMARK, cmd_data_check_port_remark, cmd_data_print_port_remark),
    SW_TYPE_DEF(SW_COSMAP, cmd_data_check_cosmap, cmd_data_print_cosmap),
    SW_TYPE_DEF(SW_SCHEDULER, cmd_data_check_queue_scheduler, cmd_data_print_queue_scheduler),
    SW_TYPE_DEF(SW_QUEUEBMP, cmd_data_check_ring_queue, cmd_data_print_ring_queue),
    SW_TYPE_DEF(SW_PORT_SHAPER_TOKEN_CONFIG, cmd_data_check_port_shaper_token_config,
		    cmd_data_print_port_shaper_token_config),
    SW_TYPE_DEF(SW_SHAPER_TOKEN_CONFIG, cmd_data_check_shaper_token_config,
		    cmd_data_print_shaper_token_config),
    SW_TYPE_DEF(SW_PORT_SHAPER_CONFIG, cmd_data_check_port_shaper_config,
		    cmd_data_print_port_shaper_config),
    SW_TYPE_DEF(SW_SHAPER_CONFIG, cmd_data_check_shaper_config,
		    cmd_data_print_shaper_config),
    SW_TYPE_DEF(SW_BMSTHRESH, cmd_data_check_bm_static_thresh,
		    cmd_data_print_bm_static_thresh),
    SW_TYPE_DEF(SW_BMDTHRESH, cmd_data_check_bm_dynamic_thresh,
		    cmd_data_print_bm_dynamic_thresh),
    SW_TYPE_DEF(SW_BMPORTCNT, NULL, cmd_data_print_bm_port_counter),
    SW_TYPE_DEF(SW_MODULE, cmd_data_check_module, cmd_data_print_module),
    SW_TYPE_DEF(SW_FUNC_CTRL, cmd_data_check_func_ctrl, cmd_data_print_func_ctrl),
    SW_TYPE_DEF(SW_QM_CNT, NULL, cmd_data_print_queue_cnt),
    SW_TYPE_DEF(SW_POLICER_ACL_CONFIG, cmd_data_check_acl_policer_config,
		    cmd_data_print_acl_policer_config),
    SW_TYPE_DEF(SW_POLICER_PORT_CONFIG, cmd_data_check_port_policer_config,
		    cmd_data_print_port_policer_config),
    SW_TYPE_DEF(SW_POLICER_CMD_CONFIG, cmd_data_check_policer_cmd_config,
		    cmd_data_print_policer_cmd_config),
    SW_TYPE_DEF(SW_POLICER_COUNTER, NULL, cmd_data_print_policer_counter_infor),
    SW_TYPE_DEF(SW_POLICER_GLOBAL_COUNTER, NULL, cmd_data_print_policer_global_counter_infor),
    SW_TYPE_DEF(SW_RESOURCE_SCHE, NULL, cmd_data_print_port_scheduler_resource),
    SW_TYPE_DEF(SW_PTP_CONFIG, cmd_data_check_ptp_config, cmd_data_print_ptp_config),
    SW_TYPE_DEF(SW_PTP_REFERENCE_CLOCK, cmd_data_check_ptp_reference_clock,
		    cmd_data_print_ptp_reference_clock),
    SW_TYPE_DEF(SW_PTP_RX_TIMESTAMP_MODE, cmd_data_check_ptp_rx_timestamp_mode,
		    cmd_data_print_ptp_rx_timestamp_mode),
    SW_TYPE_DEF(SW_PTP_DIRECTION, cmd_data_check_ptp_direction, NULL),
    SW_TYPE_DEF(SW_PTP_PKT_INFO, cmd_data_check_ptp_pkt_info, cmd_data_print_ptp_pkt_info),
    SW_TYPE_DEF(SW_PTP_TIME, cmd_data_check_ptp_time, cmd_data_print_ptp_time),
    SW_TYPE_DEF(SW_PTP_GRANDMASTER_MODE, cmd_data_check_ptp_grandmaster_mode,
		    cmd_data_print_ptp_grandmaster_mode),
    SW_TYPE_DEF(SW_PTP_SECURITY, cmd_data_check_ptp_security,
		    cmd_data_print_ptp_security),
    SW_TYPE_DEF(SW_PTP_PPS_SIGNAL_CONTROL, cmd_data_check_ptp_pps_sig_ctrl,
		    cmd_data_print_ptp_pps_sig_ctrl),
    SW_TYPE_DEF(SW_PTP_ASYM_CORRECTION, cmd_data_check_ptp_asym_correction,
		    cmd_data_print_ptp_asym_correction),
    SW_TYPE_DEF(SW_PTP_OUTPUT_WAVEFORM, cmd_data_check_ptp_waveform,
		    cmd_data_print_ptp_waveform),
    SW_TYPE_DEF(SW_PTP_TOD_UART, cmd_data_check_ptp_tod_uart,
		    cmd_data_print_ptp_tod_uart),
    SW_TYPE_DEF(SW_PTP_ENHANCED_TS_ENGINE, cmd_data_check_ptp_enhanced_timestamp_engine,
		    cmd_data_print_ptp_enhanced_timestamp_engine),
    SW_TYPE_DEF(SW_PTP_TRIGGER, cmd_data_check_ptp_trigger, cmd_data_print_ptp_trigger),
    SW_TYPE_DEF(SW_PTP_CAPTURE, cmd_data_check_ptp_capture, cmd_data_print_ptp_capture),
    SW_TYPE_DEF(SW_PTP_INTERRUPT, cmd_data_check_ptp_interrupt, cmd_data_print_ptp_interrupt),
    SW_TYPE_DEF(SW_SFP_DATA, cmd_data_check_sfp_data, cmd_data_print_sfp_data),
    SW_TYPE_DEF(SW_SFP_DEV_TYPE, NULL, cmd_data_print_sfp_dev_type),
    SW_TYPE_DEF(SW_SFP_TRANSC_CODE, NULL, cmd_data_print_sfp_transc_code),
    SW_TYPE_DEF(SW_SFP_RATE_ENCODE, NULL, cmd_data_print_sfp_rate_encode),
    SW_TYPE_DEF(SW_SFP_LINK_LENGTH, NULL, cmd_data_print_sfp_link_length),
    SW_TYPE_DEF(SW_SFP_VENDOR_INFO, NULL, cmd_data_print_sfp_vendor_info),
    SW_TYPE_DEF(SW_SFP_LASER_WAVELENGTH, NULL, cmd_data_print_sfp_laser_wavelength),
    SW_TYPE_DEF(SW_SFP_OPTION, NULL, cmd_data_print_sfp_option),
    SW_TYPE_DEF(SW_SFP_CTRL_RATE, NULL, cmd_data_print_sfp_ctrl_rate),
    SW_TYPE_DEF(SW_SFP_ENHANCED_CFG, NULL, cmd_data_print_sfp_enhanced_cfg),
    SW_TYPE_DEF(SW_SFP_DIAG_THRESHOLD, NULL, cmd_data_print_sfp_diag_threshold),
    SW_TYPE_DEF(SW_SFP_DIAG_CAL_CONST, NULL, cmd_data_print_sfp_diag_cal_const),
    SW_TYPE_DEF(SW_SFP_DIAG_REALTIME, NULL, cmd_data_print_sfp_diag_realtime),
    SW_TYPE_DEF(SW_SFP_CTRL_STATUS, NULL, cmd_data_print_sfp_ctrl_status),
    SW_TYPE_DEF(SW_SFP_ALARM_WARN_FLAG, NULL, cmd_data_print_sfp_alarm_warn_flag),
    SW_TYPE_DEF(SW_SFP_CCODE_TYPE, cmd_data_check_sfp_ccode_type, NULL),
/*qca808x_start*/
};

sw_data_type_t *
cmd_data_type_find(sw_data_type_e type)
{
    a_uint16_t i = 0;

    do
    {
        if (type == sw_data_type[i].data_type)
            return &sw_data_type[i];
    }
    while ( ++i < sizeof(sw_data_type)/sizeof(sw_data_type[0]));

    return NULL;
}

sw_error_t __cmd_data_check_quit_help(char *cmd, char *usage)
{
    sw_error_t ret = SW_OK;

    if (!strncasecmp(cmd, "quit", 4)) {
        return SW_ABORTED;
    } else if (!strncasecmp(cmd, "help", 4)) {
        dprintf("%s", usage);
        ret = SW_BAD_VALUE;
    }

    return ret;
}

sw_error_t __cmd_data_check_complex(char *info, char *defval, char *usage,
				sw_error_t(*chk_func)(), void *arg_val,
				a_uint32_t size)
{
    sw_error_t ret;
    char *cmd;

    do {
        cmd = get_sub_cmd(info, defval);
        SW_RTN_ON_NULL_PARAM(cmd);

        ret = __cmd_data_check_quit_help(cmd, usage);
        if (ret == SW_ABORTED)
            return ret;
        else if (ret == SW_OK) {
            ret = chk_func(cmd, arg_val, size);
            if (ret)
                dprintf("%s", usage);
        }
    } while (talk_mode && (SW_OK != ret));

    return SW_OK;
}

sw_error_t
cmd_data_check_uint8(char *cmd_str, a_uint32_t *arg_val, a_uint32_t size)
{
    if (cmd_str == NULL)
        return SW_BAD_PARAM;

    if (0 == cmd_str[0])
    {
        return SW_BAD_VALUE;
    }

    if (cmd_str[0] == '0' && (cmd_str[1] == 'x' || cmd_str[1] == 'X'))
        sscanf(cmd_str, "%x", arg_val);
    else
        sscanf(cmd_str, "%d", arg_val);

    if (255 < *arg_val)
    {
        return SW_BAD_PARAM;
    }

    return SW_OK;
}

void
cmd_data_print_uint8(a_uint8_t * param_name, a_uint32_t * buf, a_uint32_t size)
{
    dprintf("[%s]:0x%x", param_name, *(a_uint8_t *) buf);

}


sw_error_t
cmd_data_check_uint32(char *cmd_str, a_uint32_t * arg_val, a_uint32_t size)
{
    if (cmd_str == NULL)
        return SW_BAD_PARAM;

    if (0 == cmd_str[0])
    {
        return SW_BAD_VALUE;
    }

    if (strspn(cmd_str, "1234567890abcdefABCDEFXx") != strlen(cmd_str)){
        return SW_BAD_VALUE;
    }

    if (cmd_str[0] == '0' && (cmd_str[1] == 'x' || cmd_str[1] == 'X'))
        sscanf(cmd_str, "%x", arg_val);
    else
        sscanf(cmd_str, "%d", arg_val);

    return SW_OK;
}

void
cmd_data_print_uint32(a_char_t * param_name, a_uint32_t * buf, a_uint32_t size)
{
    dprintf("[%s]:0x%x", param_name, *(a_uint32_t *) buf);
}

sw_error_t
cmd_data_check_uint64(char *cmd_str, a_uint64_t * arg_val, a_uint32_t size)
{
    if (cmd_str == NULL)
        return SW_BAD_PARAM;

    if (0 == cmd_str[0])
    {
        return SW_BAD_VALUE;
    }

    if (strspn(cmd_str, "1234567890abcdefABCDEFXx") != strlen(cmd_str)){
        return SW_BAD_VALUE;
    }

    if (cmd_str[0] == '0' && (cmd_str[1] == 'x' || cmd_str[1] == 'X'))
        sscanf(cmd_str, "%llx", arg_val);
    else
        sscanf(cmd_str, "%lld", arg_val);

    return SW_OK;
}

void
cmd_data_print_uint64(a_uint8_t * param_name, a_uint64_t * buf, a_uint32_t size)
{
    dprintf("[%s]:0x%llx", param_name, *(a_uint64_t *) buf);
}

sw_error_t
cmd_data_check_uint16(char *cmd_str, a_uint32_t *arg_val, a_uint32_t size)
{
    if (cmd_str == NULL)
        return SW_BAD_PARAM;

    if (0 == cmd_str[0])
    {
        return SW_BAD_VALUE;
    }

    if (cmd_str[0] == '0' && (cmd_str[1] == 'x' || cmd_str[1] == 'X'))
        sscanf(cmd_str, "%x", arg_val);
    else
        sscanf(cmd_str, "%d", arg_val);

    if (65535 < *arg_val)
    {
        return SW_BAD_PARAM;
    }

    return SW_OK;
}

void
cmd_data_print_uint16(a_char_t * param_name, a_uint32_t * buf, a_uint32_t size)
{
    dprintf("[%s]:0x%04x", param_name, *(a_uint16_t *) buf);

}

sw_error_t
cmd_data_check_pbmp(char *cmd_str, a_uint32_t * arg_val, a_uint32_t size)
{
    if (cmd_str == NULL)
        return SW_BAD_PARAM;

    if (cmd_str[0] == '0' && (cmd_str[1] == 'x' || cmd_str[1] == 'X'))
        sscanf(cmd_str, "%x", arg_val);
    else
        sscanf(cmd_str, "%d", arg_val);

    return SW_OK;

}

void
cmd_data_print_pbmp(a_uint8_t * param_name, a_uint32_t * buf, a_uint32_t size)
{
    dprintf("[%s]:0x%x", param_name, *(a_uint32_t *) buf);

}

sw_error_t
cmd_data_check_enable(char *cmd_str, a_uint32_t * arg_val, a_uint32_t size)
{
    if (cmd_str == NULL)
        return SW_BAD_PARAM;

    if (!strcasecmp(cmd_str, "disable"))
        *arg_val = FAL_DISABLE;
    else if (!strcasecmp(cmd_str, "enable"))
        *arg_val = FAL_ENABLE;
    else
    {
        //dprintf("input error");
        return SW_BAD_VALUE;
    }

    return SW_OK;
}

void
cmd_data_print_enable(a_char_t * param_name, a_uint32_t * buf, a_uint32_t size)
{
    dprintf("[%s]:", param_name);
    if (*(a_uint32_t *) buf == 1)
    {
        dprintf("ENABLE");
    }
    else if (*(a_uint32_t *) buf == 0)
    {
        dprintf("DISABLE");
    }
    else
    {
        dprintf("UNKNOWN VALUE");
    }
}
/*qca808x_end*/
/*mib*/
static char *mib_regname[] =
{
    "RxBroad",
    "RxPause",
    "RxMulti",
    "RxFcsErr",
    "RxAlignErr",
    "RxRunt",
    "RxFragment",
    "Rx64Byte",
    "Rx128Byte",
    "Rx256Byte",
    "Rx512Byte",
    "Rx1024Byte",
    "Rx1518Byte",
    "RxMaxByte",
    "RxTooLong",
    "RxGoodByte",
    "RxGoodByte1",
    "RxBadByte",
    "RxBadByte1",
    "RxOverFlow",
    "Filtered",
    "TxBroad",
    "TxPause",
    "TxMulti",
    "TxUnderRun",
    "Tx64Byte",
    "Tx128Byte",
    "Tx256Byte",
    "Tx512Byte",
    "Tx1024Byte",
    "Tx1518Byte",
    "TxMaxByte",
    "TxOverSize",
    "TxByte",
    "TxByte1",
    "TxCollision",
    "TxAbortCol",
    "TxMultiCol",
    "TxSingleCol",
    "TxExcDefer",
    "TxDefer",
    "TxLateCol",
    "RxUniCast",
    "TxUniCast",
    "RxJmFcsErr",
    "RxJmAligErr",
    "Rx14To63",
    "RxTooLongByte",
    "RxTooLongByte1",
    "RxRuntByte",
    "RxRuntByte1",
};

static char *mib_cntr_regname[] =
{
    "RxBroad",
    "RxPause",
    "RxMulti",
    "RxFcsErr",
    "RxAlignErr",
    "RxRunt",
    "RxFragment",
    "Rx64Byte",
    "Rx128Byte",
    "Rx256Byte",
    "Rx512Byte",
    "Rx1024Byte",
    "Rx1518Byte",
    "RxMaxByte",
    "RxTooLong",
    "RxGoodByte",
    "RxBadByte",
    "RxOverFlow",
    "Filtered",
    "TxBroad",
    "TxPause",
    "TxMulti",
    "TxUnderRun",
    "Tx64Byte",
    "Tx128Byte",
    "Tx256Byte",
    "Tx512Byte",
    "Tx1024Byte",
    "Tx1518Byte",
    "TxMaxByte",
    "TxOverSize",
    "TxByte",
    "TxCollision",
    "TxAbortCol",
    "TxMultiCol",
    "TxSingleCol",
    "TxExcDefer",
    "TxDefer",
    "TxLateCol",
    "RxUniCast",
    "TxUniCast",
    "RxJmFcsErr",
    "RxJmAligErr",
    "Rx14To63",
    "RxTooLongByte",
    "RxRuntByte",
};

static char *xgmib_regname[] =
{
   "RxFrame",
   "RxByte",
   "RxByteGood",
   "RxBroadGood",
   "RxMultiGood",
   "RxFcsErr",
   "RxRuntErr",
   "RxJabberError",
   "RxUndersizeGood",
   "RxOversizeGood",
   "Rx64Byte",
   "Rx128Byte",
   "Rx256Byte",
   "Rx512Byte",
   "Rx1024Byte",
   "RxMaxByte",
   "RxUnicastGood",
   "RxLengthError",
   "RxOutOfRangeError",
   "RxPause",
   "RxOverFlow",
   "RxVLANFrameGoodBad",
   "RxWatchDogError",
   "RxLPIUsec",
   "RxLPITran",
   "RxDropFrameGoodBad",
   "RxDropByteGoodBad",
   "TxByte" ,
   "TxFrame" ,
   "TxBroadGood",
   "TxMultiGood",
   "Tx64Byte" ,
   "Tx128Byte",
   "Tx256Byte",
   "Tx512Byte",
   "Tx1024Byte",
   "TxMaxByte",
   "TxUnicast",
   "TxMulti" ,
   "TxBroad",
   "TxUnderFlowError",
   "TxByteGood",
   "TxFrameGood",
   "TxPause",
   "TxVLANFrameGood",
   "TxLPIUsec" ,
   "TxLPITran"
};


void
cmd_data_print_mib(a_uint8_t * param_name, a_uint32_t * buf, a_uint32_t size)
{
    dprintf("\n[%s] \n", param_name);
    a_uint32_t offset = 0;
    for (offset = 0; offset < (sizeof (fal_mib_info_t) / sizeof (a_uint32_t));
            offset++)
    {

        dprintf("%-15s<0x%015x>  ", mib_regname[offset], *(buf + offset));
        if ((offset + 1) % 3 == 0)
            dprintf("\n");
    }
}

void
cmd_data_print_mib_cntr(a_uint8_t * param_name, a_uint64_t * buf, a_uint32_t size)
{
    dprintf("\n[%s] \n", param_name);
    a_uint32_t offset = 0;

    for (offset = 0; offset < (sizeof (fal_mib_counter_t) / sizeof (a_uint64_t));
            offset++)
    {
        dprintf("%-14s<0x%014llx>  ", mib_cntr_regname[offset], *(buf + offset));
        if ((offset + 1) % 3 == 0)
            dprintf("\n");
    }
}


void
cmd_data_print_xgmib(a_uint8_t * param_name, a_uint64_t * buf, a_uint64_t size)
{
    dprintf("\n[%s] \n", param_name);
    a_uint64_t offset = 0, number;

    number = sizeof (fal_xgmib_info_t) / sizeof (a_uint64_t);
    for (offset = 0; offset < number; offset++)
    {
        dprintf("%-20s<0x%016llx> ", xgmib_regname[offset], *(buf + offset));

        if ((offset + 1) % 3 == 0)
            dprintf("\n");
    }
}
/*qca808x_start*/
/*port counter*/
static char *counter_regname[] =
{
    "RxGoodFrame",
    "RxBadCRC   ",
    "TxGoodFrame",
    "TxBadCRC   ",
    "SysRxGoodFrame",
    "SysRxBadCRC",
    "SysTxGoodFrame",
    "SysTxBadCRC",
};

void
cmd_data_print_counter_info(a_uint8_t * param_name, a_uint32_t * buf, a_uint32_t size)
{
    dprintf("\n[%s] \n", param_name);
    a_uint32_t offset = 0;
    for (offset = 0; offset < (sizeof (fal_port_counter_info_t) / sizeof (a_uint32_t));
            offset++)
    {

        dprintf("%s<0x%08x>\n", counter_regname[offset], *(buf + offset));

    }
}

void
cmd_data_print_debug_register_info(a_uint8_t * param_name, a_uint32_t * buf, a_uint32_t size)
{
    dprintf("\n[%s]", param_name);
	fal_debug_reg_dump_t * reg_dump = (fal_debug_reg_dump_t * )buf;

	a_uint32_t reg_count;

	dprintf("\n%s. ", reg_dump->reg_name);

	reg_count = 0;
	dprintf("\n");
	for (;reg_count < reg_dump->reg_count;reg_count++)
	{
		dprintf("%08x:%08x  ",reg_dump->reg_addr[reg_count], reg_dump->reg_value[reg_count]);
		if ((reg_count + 1) % 4 == 0)
			dprintf("\n");
	}

	dprintf("\n\n\n");
}



void
cmd_data_print_register_info(a_uint8_t * param_name, a_uint32_t * buf, a_uint32_t size)
{
    dprintf("\n[%s]", param_name);
	fal_reg_dump_t * reg_dump = (fal_reg_dump_t * )buf;
	a_uint32_t n[8]={0,4,8,0xc,0x10,0x14,0x18,0x1c};
	a_uint32_t dump_addr, reg_count;

	dprintf("\n%s. ", reg_dump->reg_name);
	dprintf("\n	%8x %8x %8x %8x %8x %8x %8x %8x\n",
					n[0],n[1],n[2],n[3],n[4],n[5],n[6],n[7]);
	dprintf(" [%04x] ", reg_dump->reg_base);

	reg_count = 0;
	for (dump_addr = reg_dump->reg_base;
			(dump_addr <= reg_dump->reg_end )&& (reg_count <= reg_dump->reg_count);
			reg_count++)
	{
		dprintf("%08x ", reg_dump->reg_value[reg_count]);
		dump_addr += 4;
		if ((reg_count + 1) % 8 == 0)
			dprintf("\n [%04x] ", dump_addr);
	}

	dprintf("\n\n\n");
}

void
cmd_data_print_phy_register_info(a_uint8_t * param_name, a_uint32_t * buf, a_uint32_t size)
{
    dprintf("\n[%s]", param_name);
	fal_phy_dump_t * phy_dump = (fal_phy_dump_t * )buf;

	a_uint32_t n[8]={0,1,2,3,4,5,6,7};

	a_uint32_t dump_addr, reg_count;

	dprintf("\n%s. ", phy_dump->phy_name);
	dprintf("\n	%8x %8x %8x %8x %8x %8x %8x %8x\n",
					n[0],n[1],n[2],n[3],n[4],n[5],n[6],n[7]);
	dprintf(" [%04x] ", phy_dump->phy_base);

	reg_count = 0;
	for (dump_addr = phy_dump->phy_base;
			(dump_addr <= phy_dump->phy_end )&& (reg_count <= phy_dump->phy_count);
			reg_count++)
	{
		dprintf("%08x ", phy_dump->phy_value[reg_count]);
		dump_addr ++;
		if ((reg_count + 1) % 8 == 0)
			dprintf("\n [%04x] ", dump_addr);
	}

	dprintf("\n\n\n");
}


/*port ctrl*/
sw_error_t
cmd_data_check_duplex(char *cmd_str, a_uint32_t * arg_val, a_uint32_t size)
{
    if (cmd_str == NULL)
        return SW_BAD_PARAM;

    if (!strcasecmp(cmd_str, "half"))
        *arg_val = FAL_HALF_DUPLEX;
    else if (!strcasecmp(cmd_str, "full"))
        *arg_val = FAL_FULL_DUPLEX;
    else
    {
        //dprintf("input error \n");
        return SW_BAD_VALUE;
    }

    return SW_OK;
}

void
cmd_data_print_duplex(a_uint8_t * param_name, a_uint32_t * buf, a_uint32_t size)
{
    dprintf("[%s]:", param_name);
    if (*(a_uint32_t *) buf == 0)
    {
        dprintf("HALF");
    }
    else if (*(a_uint32_t *) buf == 1)
    {
        dprintf("FULL");
    }
    else
    {
        dprintf("UNKNOWN VALUE");
    }
}

sw_error_t
cmd_data_check_speed(char *cmd_str, a_uint32_t * arg_val, a_uint32_t size)
{
    if (cmd_str == NULL)
        return SW_BAD_PARAM;

    if (!strncasecmp(cmd_str, "10", 3))
        *arg_val = FAL_SPEED_10;
    else if (!strncasecmp(cmd_str, "100", 4))
        *arg_val = FAL_SPEED_100;
    else if (!strncasecmp(cmd_str, "1000", 5))
        *arg_val = FAL_SPEED_1000;
    else if (!strncasecmp(cmd_str, "2500", 5))
        *arg_val = FAL_SPEED_2500;
    else if (!strncasecmp(cmd_str, "5000", 5))
        *arg_val = FAL_SPEED_5000;
    else if (!strncasecmp(cmd_str, "10000", 6))
        *arg_val = FAL_SPEED_10000;
    else
    {
        //dprintf("input error \n");
        return SW_BAD_VALUE;
    }

    return SW_OK;
}

void
cmd_data_print_speed(a_uint8_t * param_name, a_uint32_t * buf, a_uint32_t size)
{
    dprintf("[%s]:", param_name);
    if (*(a_uint32_t *) buf == FAL_SPEED_10)
    {
        dprintf("10(Mbps)");
    }
    else if (*(a_uint32_t *) buf == FAL_SPEED_100)
    {
        dprintf("100(Mbps)");
    }
    else if (*(a_uint32_t *) buf == FAL_SPEED_1000)
    {
        dprintf("1000(Mbps)");
    }
    else if (*(a_uint32_t *) buf == FAL_SPEED_2500)
    {
        dprintf("2500(Mbps)");
    }
    else if (*(a_uint32_t *) buf == FAL_SPEED_5000)
    {
        dprintf("5000(Mbps)");
    }
    else if (*(a_uint32_t *) buf == FAL_SPEED_10000)
    {
        dprintf("10000(Mbps)");
    }
    else
    {
        dprintf("UNKNOWN VALUE");
    }
}

sw_error_t
cmd_data_check_capable(char *cmd_str, a_uint32_t * arg_val, a_uint32_t size)
{
    if (cmd_str == NULL)
        return SW_BAD_PARAM;

    cmd_strtol(cmd_str, arg_val);
    if (*arg_val & (~FAL_PHY_COMBO_ADV_ALL))
    {
        //dprintf("input error should be within 0x3f\n");
        return SW_BAD_VALUE;
    }

    return SW_OK;
}

void
cmd_data_print_capable(a_uint8_t * param_name, a_uint32_t * buf,
                       a_uint32_t size)
{
    dprintf("[%s]:", param_name);

    if (*(a_uint32_t *) buf == 0)
    {
        dprintf("None Capable");
        return;
    }

    if (*(a_uint32_t *) buf & FAL_PHY_ADV_10000T_FD)
    {
        dprintf("10000TX_FD|");
    }
    if (*(a_uint32_t *) buf & FAL_PHY_ADV_5000T_FD)
    {
        dprintf("5000TX_FD|");
    }
    if (*(a_uint32_t *) buf & FAL_PHY_ADV_2500T_FD)
    {
        dprintf("2500TX_FD|");
    }
    if (*(a_uint32_t *) buf & FAL_PHY_ADV_1000BX_FD)
    {
        dprintf("1000BX_FD|");
    }
    if (*(a_uint32_t *) buf & FAL_PHY_ADV_1000BX_HD)
    {
        dprintf("1000BX_HD|");
    }
    if (*(a_uint32_t *) buf & FAL_PHY_ADV_1000T_FD)
    {
        dprintf("1000T_FD|");
    }
    if (*(a_uint32_t *) buf & FAL_PHY_ADV_100TX_FD)
    {
        dprintf("100TX_FD|");
    }
    if (*(a_uint32_t *) buf & FAL_PHY_ADV_100TX_HD)
    {
        dprintf("100TX_HD|");
    }
    if (*(a_uint32_t *) buf & FAL_PHY_ADV_10T_HD)
    {
        dprintf("10T_HD|");
    }
    if (*(a_uint32_t *) buf & FAL_PHY_ADV_10T_FD)
    {
        dprintf("10T_FD|");
    }
    if (*(a_uint32_t *) buf & FAL_PHY_ADV_PAUSE)
    {
        dprintf("PAUSE|");
    }
    if (*(a_uint32_t *) buf & FAL_PHY_ADV_ASY_PAUSE)
    {
        dprintf("ASY_PAUSE|");
    }
}

sw_error_t
cmd_data_check_crossover_mode(char *cmd_str, a_uint32_t * arg_val, a_uint32_t size)
{
    if (cmd_str == NULL)
        return SW_BAD_PARAM;

    if (!strncasecmp(cmd_str, "auto", 5))
        *arg_val = PHY_MDIX_AUTO;
    else if (!strncasecmp(cmd_str, "mdi", 4))
        *arg_val = PHY_MDIX_MDI;
    else if (!strncasecmp(cmd_str, "mdix", 5))
        *arg_val = PHY_MDIX_MDIX;
    else
    {
        //dprintf("input error \n");
        return SW_BAD_VALUE;
    }

    return SW_OK;
}

void
cmd_data_print_crossover_mode(a_uint8_t * param_name, a_uint32_t * buf, a_uint32_t size)
{
    dprintf("[%s]:", param_name);
    if (*(a_uint32_t *) buf == PHY_MDIX_AUTO)
    {
        dprintf("AUTO");
    }
    else if (*(a_uint32_t *) buf == PHY_MDIX_MDI)
    {
        dprintf("MDI");
    }
    else if (*(a_uint32_t *) buf == PHY_MDIX_MDIX)
    {
        dprintf("MDIX");
    }
    else
    {
        dprintf("UNKNOWN VALUE");
    }
}

sw_error_t
cmd_data_check_crossover_status(char *cmd_str, a_uint32_t * arg_val, a_uint32_t size)
{
    if (cmd_str == NULL)
        return SW_BAD_PARAM;
    if (!strncasecmp(cmd_str, "mdi", 4))
        *arg_val = PHY_MDIX_STATUS_MDI;
    else if (!strncasecmp(cmd_str, "mdix", 5))
        *arg_val = PHY_MDIX_STATUS_MDIX;
    else
    {
        //dprintf("input error \n");
        return SW_BAD_VALUE;
    }

    return SW_OK;
}

void
cmd_data_print_crossover_status(a_uint8_t * param_name, a_uint32_t * buf, a_uint32_t size)
{
    dprintf("[%s]:", param_name);
    if (*(a_uint32_t *) buf == PHY_MDIX_STATUS_MDI)
    {
        dprintf("MDI");
    }
    else if (*(a_uint32_t *) buf == PHY_MDIX_STATUS_MDIX)
    {
        dprintf("MDIX");
    }
    else
    {
        dprintf("UNKNOWN VALUE");
    }
}
/*qca808x_end*/
sw_error_t
cmd_data_check_prefer_medium(char *cmd_str, a_uint32_t * arg_val, a_uint32_t size)
{
    if (cmd_str == NULL)
        return SW_BAD_PARAM;
    if (!strncasecmp(cmd_str, "copper", 7))
        *arg_val = PHY_MEDIUM_COPPER;
    else if (!strncasecmp(cmd_str, "fiber", 6))
        *arg_val = PHY_MEDIUM_FIBER;
    else
    {
        //dprintf("input error \n");
        return SW_BAD_VALUE;
    }

    return SW_OK;
}

void
cmd_data_print_prefer_medium(a_uint8_t * param_name, a_uint32_t * buf, a_uint32_t size)
{
    dprintf("[%s]:", param_name);
    if (*(a_uint32_t *) buf == PHY_MEDIUM_COPPER)
    {
        dprintf("COPPER");
    }
    else if (*(a_uint32_t *) buf == PHY_MEDIUM_FIBER)
    {
        dprintf("FIBER");
    }
    else
    {
        dprintf("UNKNOWN VALUE");
    }
}

sw_error_t
cmd_data_check_fiber_mode(char *cmd_str, a_uint32_t * arg_val, a_uint32_t size)
{
    if (cmd_str == NULL)
        return SW_BAD_PARAM;
    if (!strncasecmp(cmd_str, "100fx", 6))
        *arg_val = PHY_FIBER_100FX;
    else if (!strncasecmp(cmd_str, "1000bx", 7))
        *arg_val = PHY_FIBER_1000BX;
    else if (!strncasecmp(cmd_str, "10g_r", 7))
        *arg_val = PHY_FIBER_10G_R;
    else
    {
        //dprintf("input error \n");
        return SW_BAD_VALUE;
    }

    return SW_OK;
}

void
cmd_data_print_fiber_mode(a_uint8_t * param_name, a_uint32_t * buf, a_uint32_t size)
{
    dprintf("[%s]:", param_name);
    if (*(a_uint32_t *) buf == PHY_FIBER_100FX)
    {
        dprintf("100FX");
    }
    else if (*(a_uint32_t *) buf == PHY_FIBER_1000BX)
    {
        dprintf("1000BX");
    }
    else
    {
        dprintf("UNKNOWN VALUE");
    }
}
/*qca808x_start*/
sw_error_t
cmd_data_check_interface_mode(char *cmd_str, a_uint32_t * arg_val, a_uint32_t size)
{
    if (cmd_str == NULL)
        return SW_BAD_PARAM;

    if (!strncasecmp(cmd_str, "psgmii_baset", 13))
        *arg_val = PHY_PSGMII_BASET;
    else if (!strncasecmp(cmd_str, "psgmii_bx1000", 14))
        *arg_val = PHY_PSGMII_BX1000;
    else if (!strncasecmp(cmd_str, "psgmii_fx100", 13))
        *arg_val = PHY_PSGMII_FX100;
    else if (!strncasecmp(cmd_str, "psgmii_amdet", 13))
        *arg_val = PHY_PSGMII_AMDET;
    else if (!strncasecmp(cmd_str, "rgmii_amdet", 13))
        *arg_val = PORT_RGMII_AMDET;
    else if (!strncasecmp(cmd_str, "rgmii_baset", 13))
        *arg_val = PORT_RGMII_BASET;
    else if (!strncasecmp(cmd_str, "rgmii_bx1000", 13))
        *arg_val = PORT_RGMII_BX1000;
    else if (!strncasecmp(cmd_str, "rgmii_fx100", 13))
        *arg_val = PORT_RGMII_FX100;
    else if (!strncasecmp(cmd_str, "sgmii_baset", 13))
        *arg_val = PHY_SGMII_BASET;
    else if (!strncasecmp(cmd_str, "qsgmii", 13))
	  *arg_val = PORT_QSGMII;
    else if (!strncasecmp(cmd_str, "sgmii_plus", 13))
	  *arg_val = PORT_SGMII_PLUS;
    else if (!strncasecmp(cmd_str, "usxgmii", 13))
	  *arg_val = PORT_USXGMII;
    else if (!strncasecmp(cmd_str, "10gbase_r", 13))
	  *arg_val = PORT_10GBASE_R;
    else if (!strncasecmp(cmd_str, "sgmii_fiber", 20))
	  *arg_val = PORT_SGMII_FIBER;
    else if (!strncasecmp(cmd_str, "psgmii_fiber", 20))
	  *arg_val = PHY_PSGMII_FIBER;
    else if (!strncasecmp(cmd_str, "interfacemode_max", 20))
	  *arg_val = PORT_INTERFACE_MODE_MAX;
    else
    {
        //dprintf("input error \n");
        return SW_BAD_VALUE;
    }

    return SW_OK;
}

void
cmd_data_print_interface_mode(a_uint8_t * param_name, a_uint32_t * buf, a_uint32_t size)
{
    dprintf("[%s]:", param_name);
    if (*(a_uint32_t *) buf == PHY_PSGMII_BASET)
    {
        dprintf("PSGMII_BASET");
    }
    else if (*(a_uint32_t *) buf == PHY_PSGMII_BX1000)
    {
        dprintf("PSGMII_BX1000");
    }
    else if (*(a_uint32_t *) buf == PHY_PSGMII_FX100)
    {
	    dprintf("PSGMII_FX100");
    }
    else if (*(a_uint32_t *) buf == PHY_PSGMII_AMDET)
    {
	    dprintf("PSGMII_AMDET");
    }
    else if (*(a_uint32_t *) buf == PORT_RGMII_AMDET)
    {
	    dprintf("RGMII_AMDET");
    }
    else if (*(a_uint32_t *) buf == PORT_RGMII_BASET)
    {
	    dprintf("RGMII_BASET");
    }
    else if (*(a_uint32_t *) buf == PORT_RGMII_BX1000)
    {
	    dprintf("RGMII_BX1000");
    }
    else if (*(a_uint32_t *) buf == PORT_RGMII_FX100)
    {
	    dprintf("RGMII_FX100");
    }
    else if (*(a_uint32_t *) buf == PHY_SGMII_BASET)
    {
	    dprintf("SGMII_BASET");
    }
    else if (*(a_uint32_t *) buf == PORT_QSGMII)
    {
	    dprintf("QSGMII");
    }
    else if (*(a_uint32_t *) buf == PORT_SGMII_PLUS)
    {
	    dprintf("SGMII PLUS");
    }
    else if (*(a_uint32_t *) buf == PORT_USXGMII)
    {
	    dprintf("USXGMII");
    }
    else if (*(a_uint32_t *) buf == PORT_10GBASE_R)
    {
	    dprintf("10gbase_r");
    }
    else if (*(a_uint32_t *) buf == PORT_SGMII_FIBER)
    {
	    dprintf("sgmii_fiber");
    }
    else if (*(a_uint32_t *) buf == PHY_PSGMII_FIBER)
    {
	    dprintf("psgmii_fiber");
    }
    else if (*(a_uint32_t *) buf == PORT_INTERFACE_MODE_MAX)
    {
	    dprintf("INTERFACEMODE_MAX");
    }
    else
    {
	    dprintf("UNKNOWN VALUE");
    }
}
/*qca808x_end*/
void
cmd_data_print_mtu_info(a_uint8_t * param_name, a_uint32_t * buf, a_uint32_t size)
{
	fal_mtu_ctrl_t *mtu;
	mtu = (fal_mtu_ctrl_t *) buf;

	dprintf("\n[%s] \n", param_name);

	dprintf("mtu_size:0x%x\n",mtu->mtu_size);
	if(mtu->action == MRU_MTU_FORWARD)
		dprintf("mtu_action:forward\n");
	else if(mtu->action == MRU_MTU_DROP)
		dprintf("mtu_action:drop\n");
	else if(mtu->action == MRU_MTU_CPYCPU)
		dprintf("mtu_action:cpycpu\n");
	else if(mtu->action == MRU_MTU_RDTCPU)
		dprintf("mtu_action:rdtcpu\n");
	else
		dprintf("mtu_action:unknown\n");
}

void
cmd_data_print_mru_info(a_uint8_t * param_name, a_uint32_t * buf, a_uint32_t size)
{
	fal_mru_ctrl_t *mru;
	mru = (fal_mru_ctrl_t *) buf;

	dprintf("\n[%s] \n", param_name);

	dprintf("mru_size:0x%x\n",mru->mru_size);
	if(mru->action == MRU_MRU_FORWARD)
	{
		dprintf("mru_action:forward\n");
	}
	else if(mru->action == MRU_MRU_DROP)
	{
		dprintf("mru_action:drop\n");
	}
	else if(mru->action == MRU_MRU_CPYCPU)
	{
		dprintf("mru_action:cpycpu\n");
	}
	else if(mru->action == MRU_MRU_RDTCPU)
	{
		dprintf("mru_action:rdtcpu\n");
	}
	else
	{
		dprintf("mru_action:unknown\n");
	}
}

sw_error_t
cmd_data_check_mtu_entry(char *cmd_str, void * val, a_uint32_t size)
{
    char *cmd;
    sw_error_t rv;
    fal_mtu_ctrl_t entry;

    aos_mem_zero(&entry, sizeof (fal_mtu_ctrl_t));

    do
    {
        cmd = get_sub_cmd("mtu_size", "1514");
		SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: 1514 - 32767 \n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_uint32(cmd, &(entry.mtu_size), sizeof (a_uint32_t));
            if (SW_OK != rv)
                dprintf("usage: usage: 1514 - 32767 \n");
        }
    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("mtu_action", "forward");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: usage: forward/drop/cpycpu/rdtcpu\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
	    rv = cmd_data_check_maccmd(cmd, (fal_fwd_cmd_t *) (&(entry.action)),
				sizeof (a_uint32_t));
            if (SW_OK != rv)
                dprintf("usage: usage: forward/drop/cpycpu/rdtcpu\n");
        }
    }
    while (talk_mode && (SW_OK != rv));

    *(fal_mtu_ctrl_t *)val = entry;
    return SW_OK;
}

sw_error_t
cmd_data_check_mru_entry(char *cmd_str, void * val, a_uint32_t size)
{
    char *cmd;
    sw_error_t rv;
    fal_mru_ctrl_t entry;

    aos_mem_zero(&entry, sizeof (fal_mru_ctrl_t));

    do
    {
        cmd = get_sub_cmd("mru_size", "1514");
		SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: 1514 - 32767 \n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_uint32(cmd, &(entry.mru_size), sizeof (a_uint32_t));
            if (SW_OK != rv)
                dprintf("usage: usage: 1514 - 32767 \n");
        }
    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("mru_action", "forward");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: usage: forward/drop/cpycpu/rdtcpu\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_maccmd(cmd,(fal_fwd_cmd_t *) (&(entry.action)),
					sizeof (a_uint32_t));
            if (SW_OK != rv)
            {
                dprintf("usage: usage: forward/drop/cpycpu/rdtcpu\n");
            }
        }
    }
    while (talk_mode && (SW_OK != rv));

    *(fal_mru_ctrl_t *)val = entry;
    return SW_OK;
}

/*portvlan*/
sw_error_t
cmd_data_check_1qmode(char *cmd_str, a_uint32_t * arg_val, a_uint32_t size)
{
    if (cmd_str == NULL)
        return SW_BAD_PARAM;

    if (!strcasecmp(cmd_str, "disable"))
    {
        *arg_val = FAL_1Q_DISABLE;
    }
    else if (!strcasecmp(cmd_str, "secure"))
    {
        *arg_val = FAL_1Q_SECURE;
    }
    else if (!strcasecmp(cmd_str, "check"))
    {
        *arg_val = FAL_1Q_CHECK;
    }
    else if (!strcasecmp(cmd_str, "fallback"))
    {
        *arg_val = FAL_1Q_FALLBACK;
    }
    else
    {
        //dprintf("input error \n");
        return SW_BAD_VALUE;
    }

    return SW_OK;
}

void
cmd_data_print_1qmode(a_uint8_t * param_name, a_uint32_t * buf, a_uint32_t size)
{
    dprintf("[%s]:", param_name);
    if (*(a_uint32_t *) buf == FAL_1Q_DISABLE)
    {
        dprintf("DISABLE\n");
    }
    else if (*(a_uint32_t *) buf == FAL_1Q_SECURE)
    {
        dprintf("SECURE\n");
    }
    else if (*(a_uint32_t *) buf == FAL_1Q_CHECK)
    {
        dprintf("CHECK\n");
    }
    else if (*(a_uint32_t *) buf == FAL_1Q_FALLBACK)
    {
        dprintf("FALLBACK\n");
    }
    else
    {
        dprintf("UNKNOWN VALUE");
    }
}

sw_error_t
cmd_data_check_egmode(char *cmd_str, a_uint32_t * arg_val, a_uint32_t size)
{
    if (cmd_str == NULL)
        return SW_BAD_PARAM;

    if (!strcasecmp(cmd_str, "unmodified"))
    {
        *arg_val = FAL_EG_UNMODIFIED;
    }
    else if (!strcasecmp(cmd_str, "untagged"))
    {
        *arg_val = FAL_EG_UNTAGGED;
    }
    else if (!strcasecmp(cmd_str, "tagged"))
    {
        *arg_val = FAL_EG_TAGGED;
    }
    else if (!strcasecmp(cmd_str, "hybrid"))
    {
        *arg_val = FAL_EG_HYBRID;
    }
    else if (!strcasecmp(cmd_str, "untouched"))
    {
        *arg_val = FAL_EG_UNTOUCHED;
    }
    else
    {
        //dprintf("input error \n");
        return SW_BAD_VALUE;
    }

    return SW_OK;
}

void
cmd_data_print_egmode(a_char_t * param_name, a_uint32_t * buf, a_uint32_t size)
{
    dprintf("[%s]:", param_name);
    if (*(a_uint32_t *) buf == FAL_EG_UNMODIFIED)
    {
        dprintf("UNMODIFIED");
    }
    else if (*(a_uint32_t *) buf == FAL_EG_UNTAGGED)
    {
        dprintf("UNTAGGED");
    }
    else if (*(a_uint32_t *) buf == FAL_EG_TAGGED)
    {
        dprintf("TAGGED");
    }
    else if (*(a_uint32_t *) buf == FAL_EG_HYBRID)
    {
        dprintf("HYBRID");
    }
    else if (*(a_uint32_t *) buf == FAL_EG_UNTOUCHED)
    {
        dprintf("UNTOUCHED");
    }
    else
    {
        dprintf("UNKNOWN VALUE");
    }
}

/*vlan*/
sw_error_t
cmd_data_check_vlan(char *cmdstr, fal_vlan_t * val, a_uint32_t size)
{
    char *cmd;
    sw_error_t rv;
    fal_vlan_t entry;
    a_uint32_t tmp = 0;

    memset(&entry, 0, sizeof (fal_vlan_t));

    do
    {
        cmd = get_sub_cmd("vlanid", NULL);
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: the range is 0 -- 4095\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_uint32(cmd, &tmp, sizeof (a_uint32_t));
            if (SW_OK != rv)
                dprintf("usage: the range is 0 -- 4095\n");
        }

    }
    while (talk_mode && (SW_OK != rv));
    entry.vid = tmp & 0xffff;

    do
    {
        cmd = get_sub_cmd("fid", NULL);
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: the range is 0 -- 4095 or 65535\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_uint32(cmd, &tmp, sizeof (a_uint32_t));
            if (SW_OK != rv)
                dprintf("usage: the range is 0 -- 4095 or 65535\n");
        }

    }
    while (talk_mode && (SW_OK != rv));
    entry.fid = tmp & 0xffff;

    do
    {
        cmd = get_sub_cmd("port member", "null");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {

            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: input port number such as 1,3\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_portmap(cmd, &entry.mem_ports,
                                        sizeof (fal_pbmp_t));
            if (SW_OK != rv)
                dprintf("usage: input port number such as 1,3\n");
        }

    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("tagged member", "null");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {

            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: input port number such as 1,3\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_portmap(cmd, &entry.tagged_ports,
                                        sizeof (fal_pbmp_t));
            if (SW_OK != rv)
                dprintf("usage: input port number such as 1,3\n");
        }

    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("untagged member", "null");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {

            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: input port number such as 1,3\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_portmap(cmd, &entry.untagged_ports,
                                        sizeof (fal_pbmp_t));
            if (SW_OK != rv)
                dprintf("usage: input port number such as 1,3\n");
        }

    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("unmodify member", "null");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {

            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: input port number such as 1,3\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_portmap(cmd, &entry.unmodify_ports,
                                        sizeof (fal_pbmp_t));
            if (SW_OK != rv)
                dprintf("usage: input port number such as 1,3\n");
        }

    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("learn disable", "no");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {

            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: <yes/no/y/n>\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_confirm(cmd, A_FALSE, &entry.learn_dis,
                                        sizeof (a_bool_t));
            if (SW_OK != rv)
                dprintf("usage: <yes/no/y/n>\n");
        }

    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("queue override", "no");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {

            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: <yes/no/y/n>\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_confirm(cmd, A_FALSE, &entry.vid_pri_en,
                                        sizeof (a_bool_t));
            if (SW_OK != rv)
                dprintf("usage: <yes/no/y/n>\n");
        }

    }
    while (talk_mode && (SW_OK != rv));

    if (A_TRUE == entry.vid_pri_en)
    {
        do
        {
            cmd = get_sub_cmd("queue", NULL);
            SW_RTN_ON_NULL_PARAM(cmd);

            if (!strncasecmp(cmd, "quit", 4))
            {
                return SW_BAD_VALUE;
            }
            else if (!strncasecmp(cmd, "help", 4))
            {
                dprintf("usage: input number such as <0/1/2/3>\n");
                rv = SW_BAD_VALUE;
            }
            else
            {
                rv = cmd_data_check_uint32(cmd, &tmp, sizeof (a_uint32_t));
                if (SW_OK != rv)
                    dprintf("usage: input number such as <0/1/2/3>\n");
            }

        }
        while (talk_mode && (SW_OK != rv));
        entry.vid_pri = tmp;
    }

    *val = entry;
    return SW_OK;
}

void
cmd_data_print_vlan(a_uint8_t * param_name, a_uint32_t * buf, a_uint32_t size)
{
    fal_vlan_t *sw_vlan = (fal_vlan_t *) buf;

    dprintf("\n[vid]:%-4d  [fid]:%-5d  [member]:0x%-4x",
            sw_vlan->vid, sw_vlan->fid, sw_vlan->mem_ports);

    dprintf("\n[tagged_member]:0x%-4x  [untagged_member]:0x%-4x  [unmodify_member]:0x%-4x  ",
            sw_vlan->tagged_ports, sw_vlan->untagged_ports, sw_vlan->unmodify_ports);

    if (sw_vlan->learn_dis == 1)
    {
        dprintf("[learn_dis]:enable   ");
    }
    else
    {
        dprintf("[learn_dis]:disable  ");
    }

    if (sw_vlan->vid_pri_en == 1)
    {
        dprintf("[pri_en]:enable  [pri]:0x%-4x\n", sw_vlan->vid_pri);
    }
    else
    {
        dprintf("[pri_en]:disable [pri]:0x%-4x\n", 0);
    }
}

sw_error_t
cmd_data_check_lan_wan_cfg(char *cmdstr, void *val, a_uint32_t size)
{
	char *cmd;
	sw_error_t rv;
	char *tmp = NULL, *str_save;
	a_uint32_t port;
	a_uint32_t vid, pvlan_ports = 0, i = 0, j = 0;
	qca_lan_wan_cfg_t entry;

	memset(&entry, 0, sizeof (qca_lan_wan_cfg_t));

	do {
		cmd = get_sub_cmd("lan_ports", NULL);
		SW_RTN_ON_NULL_PARAM(cmd);

		if (!strncasecmp(cmd, "quit", 4)) {
			return SW_BAD_VALUE;
		}
		else if (!strncasecmp(cmd, "help", 4)) {
			dprintf("usage: input port number such as 1,3\n");
			rv = SW_BAD_VALUE;
		}
		else {
			tmp = (void *) strtok_r(cmd, ",", &str_save);
			while (tmp) {
				sscanf(tmp, "%d", &port);
				if (SW_MAX_NR_PORT <= port) {
					return SW_BAD_VALUE;
				}

				entry.v_port_info[i].port_id = port;
				entry.v_port_info[i].is_wan_port = A_FALSE;
				entry.v_port_info[i].valid = A_TRUE;

				tmp = (void *) strtok_r(NULL, ",", &str_save);
				i++;
			}
		}
		if (i == 0) {
			dprintf("usage: input port number such as 1,3\n");
			rv = SW_BAD_VALUE;
		} else {
			rv = SW_OK;
		}
	} while (talk_mode && (SW_OK != rv));

	entry.lan_only_mode = A_TRUE;
	do {
		cmd = get_sub_cmd("lan_vids", NULL);
		SW_RTN_ON_NULL_PARAM(cmd);

		if (!strncasecmp(cmd, "quit", 4)) {
			return SW_BAD_VALUE;
		}
		else if (!strncasecmp(cmd, "help", 4)) {
			dprintf("usage: input vlan ids such as 1,1, the vlan id range 0--4095\n");
			rv = SW_BAD_VALUE;
		}
		else {
			tmp = (void *) strtok_r(cmd, ",", &str_save);
			while (tmp) {
				sscanf(tmp, "%d", &vid);
				if (0xfff <= vid) {
					return SW_BAD_VALUE;
				}

				entry.v_port_info[j].vid = vid;

				if (vid == 0) {
					pvlan_ports++;
				} else {
					entry.lan_only_mode = A_FALSE;
				}

				tmp = (void *) strtok_r(NULL, ",", &str_save);
				j++;
			}
		}
		if (j == 0) {
			dprintf("usage: input vlan ids such as 1,1, the vlan id range 0--4095\n");
			rv = SW_BAD_VALUE;
		} else {
			rv = SW_OK;
		}
	} while (talk_mode && (SW_OK != rv));

	if (i != j) {
		dprintf("the lan ports and vids are unmatched\n");
		return SW_BAD_VALUE;
	}

	/*
	 * portbased vlan used:
	 * ssdk_sh vlan lan_wan_cfg set 1,2,3,4 0,0,0,0
	 */
	if (pvlan_ports == i && entry.lan_only_mode) {
		*(qca_lan_wan_cfg_t *)val = entry;
		return SW_OK;
	}

	do {
		cmd = get_sub_cmd("wan_ports", NULL);
		SW_RTN_ON_NULL_PARAM(cmd);

		if (!strncasecmp(cmd, "quit", 4)) {
			return SW_BAD_VALUE;
		}
		else if (!strncasecmp(cmd, "help", 4)) {
			dprintf("usage: input port number such as 1,3\n");
			rv = SW_BAD_VALUE;
		}
		else {
			tmp = (void *) strtok_r(cmd, ",", &str_save);
			while (tmp) {
				sscanf(tmp, "%d", &port);
				if (SW_MAX_NR_PORT <= port) {
					return SW_BAD_VALUE;
				}

				entry.v_port_info[i].port_id = port;
				entry.v_port_info[i].is_wan_port = A_TRUE;
				entry.v_port_info[i].valid = A_TRUE;

				tmp = (void *) strtok_r(NULL, ",", &str_save);
				i++;
			}
		}
		if (i == 0) {
			dprintf("usage: input port number such as 1,3\n");
			rv = SW_BAD_VALUE;
		} else {
			rv = SW_OK;
		}
	} while (talk_mode && (SW_OK != rv));

	do {
		cmd = get_sub_cmd("wan_vids", NULL);
		SW_RTN_ON_NULL_PARAM(cmd);

		if (!strncasecmp(cmd, "quit", 4)) {
			return SW_BAD_VALUE;
		}
		else if (!strncasecmp(cmd, "help", 4)) {
			dprintf("usage: input vlan ids such as 1,1, the vlan id range 0--4095\n");
			rv = SW_BAD_VALUE;
		}
		else {
			tmp = (void *) strtok_r(cmd, ",", &str_save);
			while (tmp) {
				sscanf(tmp, "%d", &vid);
				if (0xfff <= vid) {
					return SW_BAD_VALUE;
				}

				entry.v_port_info[j].vid = vid;

				tmp = (void *) strtok_r(NULL, ",", &str_save);
				j++;
			}
		}
		if (j == 0) {
			dprintf("usage: input vlan ids such as 1,1, the vlan id range 0--4095\n");
			rv = SW_BAD_VALUE;
		} else {
			rv = SW_OK;
		}
	} while (talk_mode && (SW_OK != rv));

	if (i != j) {
		dprintf("the wan ports and vids are unmatched\n");
		return SW_BAD_VALUE;
	}

	*(qca_lan_wan_cfg_t *)val = entry;

	return SW_OK;
}

void
cmd_data_print_lan_wan_cfg(a_uint8_t *param_name, a_ulong_t *buf, a_uint32_t size)
{
	qca_lan_wan_cfg_t *entry = (qca_lan_wan_cfg_t *)buf;
	a_uint32_t i;

	dprintf("\n[%s] \n", param_name);
	dprintf("[lan_only_mode]: %s\n", entry->lan_only_mode ? "enabled" : "disabled");

	dprintf("port_id\tvlan_id\tport_type\n");
	for (i = 0; i < sizeof(entry->v_port_info)/sizeof(entry->v_port_info[0]); i++) {
		if (entry->v_port_info[i].valid) {
			dprintf("%7d\t%7d\t%9s\n",
					entry->v_port_info[i].port_id,
					entry->v_port_info[i].vid,
					entry->v_port_info[i].is_wan_port ? "wan" : "lan");
		}
	}
	dprintf("\n");
}

/*qos*/
sw_error_t
cmd_data_check_qos_sch(char *cmdstr, fal_sch_mode_t * val, a_uint32_t size)
{
    if (cmdstr == NULL)
        return SW_BAD_PARAM;

    if (!strcasecmp(cmdstr, "sp"))
    {
        *val = FAL_SCH_SP_MODE;
    }
    else if (!strcasecmp(cmdstr, "wrr"))
    {
        *val = FAL_SCH_WRR_MODE;
    }
    else if (!strcasecmp(cmdstr, "mixplus"))
    {
        *val = FAL_SCH_MIX_PLUS_MODE;
    }
    else if (!strcasecmp(cmdstr, "mix"))
    {
        *val = FAL_SCH_MIX_MODE;
    }
    else
    {
        return SW_BAD_VALUE;
    }

    return SW_OK;
}

void
cmd_data_print_qos_sch(a_uint8_t * param_name, a_uint32_t * buf,
                       a_uint32_t size)
{
    dprintf("[%s]:", param_name);
    if (*(a_uint32_t *) buf == FAL_SCH_SP_MODE)
    {
        dprintf("SP");
    }
    else if (*(a_uint32_t *) buf == FAL_SCH_WRR_MODE)
    {
        dprintf("WRR");
    }
    else if (*(a_uint32_t *) buf == FAL_SCH_MIX_MODE)
    {
        dprintf("MIX");
    }
    else if (*(a_uint32_t *) buf == FAL_SCH_MIX_PLUS_MODE)
    {
        dprintf("MIXPLUS");
    }
    else
    {
        dprintf("UNKNOWN VALUE");
    }
}

sw_error_t
cmd_data_check_qos_pt(char *cmdstr, fal_qos_mode_t * val, a_uint32_t size)
{
    if (cmdstr == NULL)
        return SW_BAD_PARAM;

    if (!strcasecmp(cmdstr, "da"))
    {
        *val = FAL_QOS_DA_MODE;
    }
    else if (!strcasecmp(cmdstr, "up"))
    {
        *val = FAL_QOS_UP_MODE;
    }
    else if (!strcasecmp(cmdstr, "dscp"))
    {
        *val = FAL_QOS_DSCP_MODE;
    }
    else if (!strcasecmp(cmdstr, "port"))
    {
        *val = FAL_QOS_PORT_MODE;
    }
    else if (!strcasecmp(cmdstr, "flow"))
    {
        *val = FAL_QOS_FLOW_MODE;
    }
    else
    {
        return SW_BAD_VALUE;
    }

    return SW_OK;
}

void
cmd_data_print_qos_pt(a_uint8_t * param_name, a_uint32_t * buf, a_uint32_t size)
{
    dprintf("[%s]:", param_name);
    if (*(a_uint32_t *) buf == FAL_QOS_DA_MODE)
    {
        dprintf("DA");
    }
    else if (*(a_uint32_t *) buf == FAL_QOS_UP_MODE)
    {
        dprintf("UP");
    }
    else if (*(a_uint32_t *) buf == FAL_QOS_DSCP_MODE)
    {
        dprintf("DSCP");
    }
    else if (*(a_uint32_t *) buf == FAL_QOS_PORT_MODE)
    {
        dprintf("PORT");
    }
    else if (*(a_uint32_t *) buf == FAL_QOS_FLOW_MODE)
    {
        dprintf("FLOW");
    }
    else
    {
        dprintf("UNKNOWN VALUE");
    }
}

/*rate*/
sw_error_t
cmd_data_check_storm(char *cmdstr, fal_storm_type_t * val, a_uint32_t size)
{
    if (cmdstr == NULL)
        return SW_BAD_PARAM;

    if (!strcasecmp(cmdstr, "unicast"))
    {
        *val = FAL_UNICAST_STORM;
    }
    else if (!strcasecmp(cmdstr, "multicast"))
    {
        *val = FAL_MULTICAST_STORM;
    }
    else if (!strcasecmp(cmdstr, "broadcast"))
    {
        *val = FAL_BROADCAST_STORM;
    }
    else
    {
        return SW_BAD_VALUE;
    }

    return SW_OK;
}

void
cmd_data_print_storm(a_uint8_t * param_name, a_uint32_t * buf, a_uint32_t size)
{
    dprintf("[%s]:", param_name);
    if (*(a_uint32_t *) buf == FAL_UNICAST_STORM)
    {
        dprintf("UNICAST");
    }
    else if (*(a_uint32_t *) buf == FAL_MULTICAST_STORM)
    {
        dprintf("MULTICAST");
    }
    else if (*(a_uint32_t *) buf == FAL_BROADCAST_STORM)
    {
        dprintf("BROADCAST");
    }
    else
    {
        dprintf("UNKNOWN VALUE");
    }
}

/*stp*/
sw_error_t
cmd_data_check_stp_state(char *cmdstr, fal_stp_state_t * val, a_uint32_t size)
{
    if (cmdstr == NULL)
        return SW_BAD_PARAM;

    if (!strcasecmp(cmdstr, "disable"))
    {
        *val = FAL_STP_DISABLED;
    }
    else if (!strcasecmp(cmdstr, "block"))
    {
        *val = FAL_STP_BLOKING;
    }
    else if (!strcasecmp(cmdstr, "listen"))
    {
        *val = FAL_STP_LISTENING;
    }
    else if (!strcasecmp(cmdstr, "learn"))
    {
        *val = FAL_STP_LEARNING;
    }
    else if (!strcasecmp(cmdstr, "forward"))
    {
        *val = FAL_STP_FARWARDING;
    }
    else
    {
        return SW_BAD_VALUE;
    }

    return SW_OK;
}

void
cmd_data_print_stp_state(a_uint8_t * param_name, a_uint32_t * buf,
                         a_uint32_t size)
{
    dprintf("[%s]:", param_name);
    if (*(a_uint32_t *) buf == FAL_STP_DISABLED)
    {
        dprintf("DISABLE");
    }
    else if (*(a_uint32_t *) buf == FAL_STP_BLOKING)
    {
        dprintf("BLOCK");
    }
    else if (*(a_uint32_t *) buf == FAL_STP_LISTENING)
    {
        dprintf("LISTEN");
    }
    else if (*(a_uint32_t *) buf == FAL_STP_LEARNING)
    {
        dprintf("LEARN");
    }
    else if (*(a_uint32_t *) buf == FAL_STP_FARWARDING)
    {
        dprintf("FORWARD");
    }
    else
    {
        dprintf("UNKNOWN VALUE");
    }
}

/*general*/
sw_error_t
cmd_data_check_leaky(char *cmdstr, fal_leaky_ctrl_mode_t * val, a_uint32_t size)
{
    if (cmdstr == NULL)
        return SW_BAD_VALUE;

    if (!strcasecmp(cmdstr, "port"))
    {
        *val = FAL_LEAKY_PORT_CTRL;
    }
    else if (!strcasecmp(cmdstr, "fdb"))
    {
        *val = FAL_LEAKY_FDB_CTRL;
    }
    else
    {
        return SW_BAD_VALUE;
    }

    return SW_OK;
}

void
cmd_data_print_leaky(a_uint8_t * param_name, a_uint32_t * buf, a_uint32_t size)
{
    dprintf("[%s]:", param_name);
    if (*(a_uint32_t *) buf == FAL_LEAKY_PORT_CTRL)
    {
        dprintf("PORT");
    }
    else if (*(a_uint32_t *) buf == FAL_LEAKY_FDB_CTRL)
    {
        dprintf("FDB");
    }
    else
    {
        dprintf("UNKNOWN VALUE");
    }
}
sw_error_t
cmd_data_check_uinta(char *cmdstr, a_uint32_t * val, a_uint32_t size)
{
    char *tmp_str = NULL, *str_save;
    a_uint32_t *tmp_ptr = val;
    a_uint32_t i = 0;

    tmp_str = (void *) strtok_r(cmdstr, ",", &str_save);
    while (tmp_str)
    {
        if (i >= (size / 4))
        {
            return SW_BAD_VALUE;
        }

        sscanf(tmp_str, "%d", tmp_ptr);
        tmp_ptr++;

        i++;
        tmp_str = (void *) strtok_r(NULL, ",", &str_save);
    }

    if (i != (size / 4))
    {
        return SW_BAD_VALUE;
    }

    return SW_OK;
}

void
cmd_data_print_uinta(a_uint8_t * param_name, a_uint32_t * buf, a_uint32_t size)
{
    a_uint32_t i;
    a_uint32_t *tmp_ptr;

    dprintf("[%s]:", param_name);

    tmp_ptr = buf;
    for (i = 0; i < (size / 4); i++)
    {
        dprintf(" %d, ", *tmp_ptr);
        tmp_ptr++;
    }
}

/*fdb*/
sw_error_t
cmd_data_check_maccmd(char *cmdstr, fal_fwd_cmd_t * val, a_uint32_t size)
{
    if (NULL == cmdstr)
    {
        return SW_BAD_VALUE;
    }

    if (0 == cmdstr[0])
    {
        *val = FAL_MAC_FRWRD;   //defualt
    }
    else if (!strcasecmp(cmdstr, "forward"))
    {
        *val = FAL_MAC_FRWRD;
    }
    else if (!strcasecmp(cmdstr, "drop"))
    {
        *val = FAL_MAC_DROP;
    }
    else if (!strcasecmp(cmdstr, "cpycpu"))
    {
        *val = FAL_MAC_CPY_TO_CPU;
    }
    else if (!strcasecmp(cmdstr, "rdtcpu"))
    {
        *val = FAL_MAC_RDT_TO_CPU;
    }
    else
    {
        return SW_BAD_VALUE;
    }

    return SW_OK;
}
void
cmd_data_print_maccmd(a_char_t * param_name, a_uint32_t * buf, a_uint32_t size)
{
    dprintf("[%s]:", param_name);
    if (*(a_uint32_t *) buf == FAL_MAC_FRWRD)
    {
        dprintf("FORWARD");
    }
    else if (*(a_uint32_t *) buf == FAL_MAC_DROP)
    {
        dprintf("DROP");
    }
    else if (*(a_uint32_t *) buf == FAL_MAC_CPY_TO_CPU)
    {
        dprintf("CPYCPU");
    }
    else if (*(a_uint32_t *) buf == FAL_MAC_RDT_TO_CPU)
    {
        dprintf("RDTCPU");
    }
    else
    {
        dprintf("UNKNOWN VALUE");
    }
}
/*flow*/
sw_error_t
cmd_data_check_flowcmd(char *cmdstr, fal_default_flow_cmd_t * val, a_uint32_t size)
{
    if (NULL == cmdstr)
    {
        return SW_BAD_VALUE;
    }

    if (0 == cmdstr[0])
    {
        *val = FAL_DEFAULT_FLOW_FORWARD;   //defualt
    }
    else if (!strcasecmp(cmdstr, "forward"))
    {
        *val = FAL_DEFAULT_FLOW_FORWARD;
    }
    else if (!strcasecmp(cmdstr, "drop"))
    {
        *val = FAL_DEFAULT_FLOW_DROP;
    }
    else if (!strcasecmp(cmdstr, "rdtcpu"))
    {
        *val = FAL_DEFAULT_FLOW_RDT_TO_CPU;
    }
    else if (!strcasecmp(cmdstr, "admit_all"))
    {
        *val = FAL_DEFAULT_FLOW_ADMIT_ALL;
    }
    else
    {
        return SW_BAD_VALUE;
    }

    return SW_OK;
}

void
cmd_data_print_flowcmd(a_char_t * param_name, a_uint32_t * buf, a_uint32_t size)
{
    dprintf("[%s]:", param_name);
    if (*(a_uint32_t *) buf == FAL_DEFAULT_FLOW_FORWARD)
    {
        dprintf("FORWARD");
    }
    else if (*(a_uint32_t *) buf == FAL_DEFAULT_FLOW_DROP)
    {
        dprintf("DROP");
    }
    else if (*(a_uint32_t *) buf == FAL_DEFAULT_FLOW_RDT_TO_CPU)
    {
        dprintf("RDTCPU");
    }
    else if (*(a_uint32_t *) buf == FAL_DEFAULT_FLOW_ADMIT_ALL)
    {
        dprintf("ADMIT_ALL");
    }
    else
    {
        dprintf("UNKNOWN VALUE");
    }
}

sw_error_t
cmd_data_check_flowtype(char *cmd_str, fal_flow_type_t * arg_val,
                        a_uint32_t size)
{
    if (NULL == cmd_str)
    {
        return SW_BAD_VALUE;
    }

    if (!strcasecmp(cmd_str, "lan2lan"))
    {
        *arg_val = FAL_FLOW_LAN_TO_LAN;
    }
    else if (!strcasecmp(cmd_str, "wan2lan"))
    {
        *arg_val = FAL_FLOW_WAN_TO_LAN;
    }
    else if (!strcasecmp(cmd_str, "lan2wan"))
    {
        *arg_val = FAL_FLOW_LAN_TO_WAN;
    }
    else if (!strcasecmp(cmd_str, "wan2wan"))
    {
        *arg_val = FAL_FLOW_WAN_TO_WAN;
    }
    else
    {
        return SW_BAD_VALUE;
    }

    return SW_OK;
}

void
cmd_data_print_flowtype(a_char_t * param_name, a_uint32_t * buf, a_uint32_t size)
{
    dprintf("[%s]:", param_name);
    if (*(a_uint32_t *) buf == FAL_FLOW_LAN_TO_LAN)
    {
        dprintf("lan2lan");
    }
    else if (*(a_uint32_t *) buf == FAL_FLOW_WAN_TO_LAN)
    {
        dprintf("wan2lan");
    }
    else if (*(a_uint32_t *) buf == FAL_FLOW_LAN_TO_WAN)
    {
        dprintf("lan2wan");
    }
    else if (*(a_uint32_t *) buf == FAL_FLOW_WAN_TO_WAN)
    {
        dprintf("wan2wan");
    }
    else
    {
        dprintf("UNKNOWN VALUE");
    }
}
/*qca808x_start*/
sw_error_t
cmd_data_check_confirm(char *cmdstr, a_bool_t def, a_bool_t * val,
                       a_uint32_t size)
{
    if (0 == cmdstr[0])
    {
        *val = def;
    }
    else if ((!strcasecmp(cmdstr, "yes")) || (!strcasecmp(cmdstr, "y")))
    {
        *val = A_TRUE;
    }
    else if ((!strcasecmp(cmdstr, "no")) || (!strcasecmp(cmdstr, "n")))
    {
        *val = A_FALSE;
    }
    else
    {
        return SW_BAD_VALUE;
    }

    return SW_OK;
}

void
cmd_data_print_confirm(char * param_name, a_bool_t val, a_uint32_t size)
{
    dprintf("%s", param_name);
    if (A_TRUE == val)
    {
        dprintf("YES");
    }
    else
    {
        dprintf("NO");
    }

    return;
}

sw_error_t
cmd_data_check_portid(char *cmdstr, fal_port_t * val, a_uint32_t size)
{
    *val = 0;
    //default input null
    if(!strcasecmp(cmdstr, "null"))
    {
        if (ssdk_cfg.init_cfg.chip_type == CHIP_HPPE)
            return SW_BAD_VALUE;
        return SW_OK;
    }
   if (strstr(cmdstr, "0x") == NULL)
	sscanf(cmdstr, "%d", val);
   else
	sscanf(cmdstr, "%x", val);

    return SW_OK;
}

sw_error_t
cmd_data_check_portmap(char *cmdstr, fal_pbmp_t * val, a_uint32_t size)
{
    char *tmp = NULL, *str_save;
    a_uint32_t port;

    *val = 0;
    //default input null
    if(!strcasecmp(cmdstr, "null"))
    {
        return SW_OK;
    }

    tmp = (void *) strtok_r(cmdstr, ",", &str_save);
    while (tmp)
    {
        sscanf(tmp, "%d", &port);
        if (SW_MAX_NR_PORT <= port)
        {
            return SW_BAD_VALUE;
        }

        *val |= (0x1 << port);
        tmp = (void *) strtok_r(NULL, ",", &str_save);
    }

    return SW_OK;
}

void
cmd_data_print_portmap(char * param_name, fal_pbmp_t val, a_uint32_t size)
{
    a_uint32_t i;
    char tmp[16];
    tmp[0] = '\0';

    dprintf("%s", param_name);
    for (i = 0; i < SW_MAX_NR_PORT; i++)
    {
        if (val & (0x1 << i))
        {
            if(strlen(tmp) == 0)
                snprintf(tmp, sizeof(tmp), "%d", i);
            else
                snprintf(tmp+strlen(tmp), sizeof(tmp+strlen(tmp)), ",%d", i);
        }
    }
    dprintf("%s ", tmp);
    return;
}

sw_error_t
cmd_data_check_macaddr(char *cmdstr, void *val, a_uint32_t size)
{
    char *tmp = NULL, *str_save;
    a_uint32_t i = 0, j;
    a_uint32_t addr;
    fal_mac_addr_t mac;

    memset(&mac, 0, sizeof (fal_mac_addr_t));
    if (NULL == cmdstr)
    {
        *(fal_mac_addr_t *) val = mac;
        return SW_BAD_VALUE; /*was: SW_OK;*/
    }

    if (0 == cmdstr[0])
    {
        *(fal_mac_addr_t *) val = mac;
        return SW_OK;
    }

    tmp = (void *) strtok_r(cmdstr, "-", &str_save);
    while (tmp)
    {
        if (6 <= i)
        {
            return SW_BAD_VALUE;
        }

        if ((2 < strlen(tmp)) || (0 == strlen(tmp)))
        {
            return SW_BAD_VALUE;
        }

        for (j = 0; j < strlen(tmp); j++)
        {
            if (A_FALSE == is_hex(tmp[j]))
                return SW_BAD_VALUE;
        }

        sscanf(tmp, "%x", &addr);
        if (0xff < addr)
        {
            return SW_BAD_VALUE;
        }

        mac.uc[i++] = addr;
        tmp = (void *) strtok_r(NULL, "-", &str_save);
    }

    if (6 != i)
    {
        return SW_BAD_VALUE;
    }

    *(fal_mac_addr_t *) val = mac;
    return SW_OK;
}

void
cmd_data_print_macaddr(a_char_t * param_name, a_uint32_t * buf,
                       a_uint32_t size)
{
    a_uint32_t i;
    fal_mac_addr_t *val;

    val = (fal_mac_addr_t *) buf;
    dprintf("%s", param_name);
    for (i = 0; i < 5; i++)
    {
        dprintf("%02x-", val->uc[i]);
    }
    dprintf("%02x", val->uc[5]);
    fflush(stdout);

}
/*qca808x_end*/
sw_error_t
cmd_data_check_fdbentry(char *info, void *val, a_uint32_t size)
{
    char *cmd, *cmd_find;
    sw_error_t rv;
    fal_fdb_entry_t entry;
    a_uint32_t tmp = 0;

    memset(&entry, 0, sizeof (fal_fdb_entry_t));

    do
    {
        cmd = get_sub_cmd("addr", NULL);
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: the format is xx-xx-xx-xx-xx-xx \n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_macaddr(cmd, &entry.addr,
                                        sizeof (fal_mac_addr_t));
            if (SW_OK != rv)
                dprintf("usage: the format is xx-xx-xx-xx-xx-xx \n");
        }
    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("fid", "65535");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: the range is 1 -- 4095 or 65535\n");
            rv = SW_BAD_VALUE;
        }
        else if (0 == cmd[0])
        {
            entry.fid = 65535;
        }
        else
        {
            rv = cmd_data_check_uint16(cmd, &tmp, sizeof (a_uint32_t));
            if (SW_OK != rv)
                dprintf("usage: the range is 1 -- 4095 or 65535\n");
        }
    }
    while (talk_mode && (SW_OK != rv));
    entry.fid = tmp & 0xffff;

    do
    {
        cmd = get_sub_cmd("dacmd", "forward");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {

            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: <forward/drop/cpycpu/rdtcpu>\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_maccmd(cmd, &entry.dacmd,
                                       sizeof (fal_fwd_cmd_t));
            if (SW_OK != rv)
                dprintf("usage: <forward/drop/cpycpu/rdtcpu>\n");
        }

    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("sacmd", "forward");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: <forward/drop/cpycpu/rdtcpu>\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_maccmd(cmd, &entry.sacmd,
                                       sizeof (fal_fwd_cmd_t));
            if (SW_OK != rv)
                dprintf("usage: <forward/drop/cpycpu/rdtcpu>\n");
        }

    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("dest port", "null");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: input port number such as 1,3\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            cmd_find = strstr(cmd, ",");
            if (cmd_find == NULL)
            {
                rv = cmd_data_check_portid(cmd, &entry.port.id, sizeof (fal_port_t));
                entry.portmap_en = A_FALSE;
            }
            else
            {
                rv = cmd_data_check_portmap(cmd, &entry.port.map, sizeof (fal_pbmp_t));
                entry.portmap_en = A_TRUE;
            }
            if (SW_OK != rv)
                dprintf("usage: input port number such as 1,3\n");
        }
    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("static", "yes");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {

            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: <yes/no/y/n>\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_confirm(cmd, A_TRUE, &entry.static_en,
                                        sizeof (a_bool_t));
            if (SW_OK != rv)
                dprintf("usage: <yes/no/y/n>\n");
        }

    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("leaky", "no");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {

            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: <yes/no/y/n>\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_confirm(cmd, A_FALSE, &entry.leaky_en,
                                        sizeof (a_bool_t));
            if (SW_OK != rv)
                dprintf("usage: <yes/no/y/n>\n");
        }

    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("mirror", "no");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {

            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: <yes/no/y/n>\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_confirm(cmd, A_FALSE, &entry.mirror_en,
                                        sizeof (a_bool_t));
            if (SW_OK != rv)
                dprintf("usage: <yes/no/y/n>\n");
        }

    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("clone", "no");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {

            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: <yes/no/y/n>\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_confirm(cmd, A_FALSE, &entry.clone_en,
                                        sizeof (a_bool_t));
            if (SW_OK != rv)
                dprintf("usage: <yes/no/y/n>\n");
        }

    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("queue override", "no");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {

            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: <yes/no/y/n>\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_confirm(cmd, A_FALSE, &entry.da_pri_en,
                                        sizeof (a_bool_t));
            if (SW_OK != rv)
                dprintf("usage: <yes/no/y/n>\n");
        }

    }
    while (talk_mode && (SW_OK != rv));

    if (A_TRUE == entry.da_pri_en)
    {
        do
        {
            cmd = get_sub_cmd("queue", NULL);
            SW_RTN_ON_NULL_PARAM(cmd);

            if (!strncasecmp(cmd, "quit", 4))
            {
                return SW_BAD_VALUE;
            }
            else if (!strncasecmp(cmd, "help", 4))
            {
                dprintf("usage: input number such as <0/1/2/3>\n");
                rv = SW_BAD_VALUE;
            }
            else
            {
                rv = cmd_data_check_uint32(cmd, &tmp, sizeof (a_uint32_t));
                if (SW_OK != rv)
                    dprintf("usage: input number such as <0/1/2/3>\n");
            }

        }
        while (talk_mode && (SW_OK != rv));
        entry.da_queue = tmp;
    }

    do
    {
        cmd = get_sub_cmd("cross_pt_state", "no");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {

            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: <yes/no/y/n>\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_confirm(cmd, A_FALSE, &entry.cross_pt_state,
                                        sizeof (a_bool_t));
            if (SW_OK != rv)
                dprintf("usage: <yes/no/y/n>\n");
        }

    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("white_list_en", "no");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {

            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: <yes/no/y/n>\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_confirm(cmd, A_FALSE, &entry.white_list_en,
                                        sizeof (a_bool_t));
            if (SW_OK != rv)
                dprintf("usage: <yes/no/y/n>\n");
        }

    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("load_balance_en", "no");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {

            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: <yes/no/y/n>\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_confirm(cmd, A_FALSE, &entry.load_balance_en,
                                        sizeof (a_bool_t));
            if (SW_OK != rv)
                dprintf("usage: <yes/no/y/n>\n");
        }

    }
    while (talk_mode && (SW_OK != rv));

    if (A_TRUE == entry.load_balance_en)
    {
        do
        {
            cmd = get_sub_cmd("load_balance", NULL);
            SW_RTN_ON_NULL_PARAM(cmd);

            if (!strncasecmp(cmd, "quit", 4))
            {
                return SW_BAD_VALUE;
            }
            else if (!strncasecmp(cmd, "help", 4))
            {
                dprintf("usage: input number such as <0/1/2/3>\n");
                rv = SW_BAD_VALUE;
            }
            else
            {
                rv = cmd_data_check_uint32(cmd, &tmp, sizeof (a_uint32_t));
                if (SW_OK != rv)
                    dprintf("usage: input number such as <0/1/2/3>\n");
            }

        }
        while (talk_mode && (SW_OK != rv));
        entry.load_balance = tmp;
    }

    *(fal_fdb_entry_t *) val = entry;

    return SW_OK;
}

void
cmd_data_print_fdbentry(a_uint8_t * param_name, a_uint32_t * buf,
                        a_uint32_t size)
{
    a_uint32_t tmp, port_type;
    fal_fdb_entry_t *entry;

    entry = (fal_fdb_entry_t *) buf;
    dprintf("\n");
    cmd_data_print_macaddr("[addr]:", (a_uint32_t *) & (entry->addr),
                           sizeof (fal_mac_addr_t));
    dprintf(" ");
    dprintf("[fid]:%d", entry->fid);
    dprintf(" ");
    cmd_data_print_confirm("[static]:", entry->static_en, sizeof (a_bool_t));
    dprintf(" ");
    if (entry->portmap_en == A_TRUE)
        cmd_data_print_portmap("[dest_port]:", entry->port.map, sizeof (fal_pbmp_t));
    else {
        port_type = FAL_PORT_ID_TYPE(entry->port.id);
        if (port_type == 1 && entry->port.id == 0x1000020)
            dprintf("[dest_port]:0x%x(trunk0)", entry->port.id);
        else if (port_type == 1 && entry->port.id == 0x1000021)
            dprintf("[dest_port]:0x%x(trunk1)", entry->port.id);
        else if (port_type == 2)
            dprintf("[dest_port]:0x%x(virtual port)", entry->port.id);
        else
            dprintf("[dest_port]:%d", entry->port.id);
    }
    dprintf(" \n");
    cmd_data_print_maccmd("dacmd", (a_uint32_t *) & (entry->dacmd),
                          sizeof (fal_fwd_cmd_t));
    dprintf(" ");
    cmd_data_print_maccmd("sacmd", (a_uint32_t *) & (entry->sacmd),
                          sizeof (fal_fwd_cmd_t));
    dprintf(" ");
    cmd_data_print_confirm("[leaky]:", entry->leaky_en, sizeof (a_bool_t));
    dprintf(" ");
    cmd_data_print_confirm("[mirror]:", entry->mirror_en, sizeof (a_bool_t));
    dprintf(" ");
    cmd_data_print_confirm("[clone]:", entry->clone_en, sizeof (a_bool_t));
    dprintf(" ");
    cmd_data_print_confirm("[da_pri]:", entry->da_pri_en, sizeof (a_bool_t));
    dprintf(" ");
    if (A_TRUE == entry->da_pri_en)
    {
        tmp = entry->da_queue;
        dprintf("[queue]:%d", tmp);
    }
    else
    {
        dprintf("[queue]:0");
    }
    dprintf(" ");
    cmd_data_print_confirm("[cross_pt_state]:", entry->cross_pt_state, sizeof (a_bool_t));
    dprintf(" ");
    cmd_data_print_confirm("[white_list_en]:", entry->white_list_en, sizeof (a_bool_t));
    dprintf(" ");
    cmd_data_print_confirm("[load_balance_en]:", entry->load_balance_en, sizeof (a_bool_t));
    if (A_TRUE == entry->load_balance_en)
    {
        tmp = entry->load_balance;
	dprintf(" ");
        dprintf("[load_balance]:%d", tmp);
    }
    dprintf("\n");

    return;
}

sw_error_t
cmd_data_check_maclimit_ctrl(char *info, void *val, a_uint32_t size)
{
    a_char_t *cmd;
    sw_error_t rv;
    fal_maclimit_ctrl_t maclimit_ctrl;

    memset(&maclimit_ctrl, 0, sizeof (fal_maclimit_ctrl_t));

    do
    {
        cmd = get_sub_cmd("maclimit status", "yes");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: <yes/no/y/n>\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_confirm(cmd, A_TRUE, &maclimit_ctrl.enable,
                                        sizeof (a_bool_t));
            if (SW_OK != rv)
                dprintf("usage: <yes/no/y/n>\n");
        }
    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("maclimit counter", "2048");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: the range is 0 -- 2048 \n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_uint32(cmd, &maclimit_ctrl.limit_num,
                                        sizeof (a_uint32_t));
            if (SW_OK != rv)
                dprintf("usage: the range is 0 -- 2048 \n");
        }
    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("maclimit exceed action", "forward");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: <forward/drop/cpycpu/rdtcpu>\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_maccmd(cmd, &maclimit_ctrl.action,
                                        sizeof (fal_fwd_cmd_t));
            if (SW_OK != rv)
                dprintf("usage: <forward/drop/cpycpu/rdtcpu>\n");
        }
    }
    while (talk_mode && (SW_OK != rv));

    *(fal_maclimit_ctrl_t *) val = maclimit_ctrl;

    return SW_OK;
}

void
cmd_data_print_maclimit_ctrl(a_uint8_t * param_name, a_uint32_t * buf,
                        a_uint32_t size)
{
    fal_maclimit_ctrl_t *maclimit_ctrl;

    maclimit_ctrl = (fal_maclimit_ctrl_t *) buf;
    dprintf("\n");
    cmd_data_print_confirm("[maclimit status]:", maclimit_ctrl->enable, sizeof (a_bool_t));
    dprintf(" ");
    cmd_data_print_uint32("maclimit counter", (a_uint32_t *) & (maclimit_ctrl->limit_num), 4);
    dprintf(" ");
    cmd_data_print_maccmd("maclimit exceed action", (a_uint32_t *) & (maclimit_ctrl->action),
                          sizeof (fal_fwd_cmd_t));
    dprintf("\n");

    return;
}
/*qca808x_start*/

#define cmd_data_check_element(info, defval, usage, chk_func, param) \
{\
    sw_error_t ret;\
    do {\
        cmd = get_sub_cmd(info, defval);\
        SW_RTN_ON_NULL_PARAM(cmd);\
        \
        if (!strncasecmp(cmd, "quit", 4)) {\
            return SW_BAD_VALUE;\
        } else if (!strncasecmp(cmd, "help", 4)) {\
            dprintf("%s", usage);\
            ret = SW_BAD_VALUE;\
        } else {\
            ret = chk_func param; \
            if (SW_OK != ret)\
                dprintf("%s", usage);\
            else\
            append_acl_cmd(cmd);\
        }\
    } while (talk_mode && (SW_OK != ret));\
}

sw_error_t
cmd_data_check_integer(char *cmd_str, a_uint32_t * arg_val, a_uint32_t max_val,
                       a_uint32_t min_val)
{
    a_uint32_t tmp;
    a_uint32_t i;

    if (NULL == cmd_str)
    {
        return SW_BAD_PARAM;
    }

    if (0 == cmd_str[0])
    {
        return SW_BAD_PARAM;
    }

    if ((cmd_str[0] == '0') && ((cmd_str[1] == 'x') || (cmd_str[1] == 'X')))
    {
        for (i = 2; i < strlen(cmd_str); i++)
        {
            if (A_FALSE == is_hex(cmd_str[i]))
            {
                return SW_BAD_VALUE;
            }
        }
        sscanf(cmd_str, "%x", &tmp);
    }
    else
    {
        for (i = 0; i < strlen(cmd_str); i++)
        {
            if (A_FALSE == is_dec(cmd_str[i]))
            {
                return SW_BAD_VALUE;
            }
        }
        sscanf(cmd_str, "%d", &tmp);
    }

    if ((tmp > max_val) || (tmp < min_val))
        return SW_BAD_PARAM;

    *arg_val = tmp;
    return SW_OK;
}
/*qca808x_end*/
sw_error_t
cmd_data_check_ruletype(char *cmd_str, fal_acl_rule_type_t * arg_val,
                        a_uint32_t size)
{
    if (NULL == cmd_str)
    {
        return SW_BAD_VALUE;
    }

    if (!strcasecmp(cmd_str, "mac"))
    {
        *arg_val = FAL_ACL_RULE_MAC;
    }
    else if (!strcasecmp(cmd_str, "ip4"))
    {
        *arg_val = FAL_ACL_RULE_IP4;
    }
    else if (!strcasecmp(cmd_str, "ip6"))
    {
        *arg_val = FAL_ACL_RULE_IP6;
    }
    else if (!strcasecmp(cmd_str, "udf"))
    {
        *arg_val = FAL_ACL_RULE_UDF;
    }
    else
    {
        return SW_BAD_VALUE;
    }

    return SW_OK;
}

void
cmd_data_print_ruletype(char * param_name, a_uint32_t * buf,
                        a_uint32_t size)
{
    fal_acl_rule_type_t *val;

    val = (fal_acl_rule_type_t *) buf;
    dprintf("%s", param_name);

    if (FAL_ACL_RULE_MAC == *val)
    {
        dprintf("mac");
    }
    else if (FAL_ACL_RULE_IP4 == *val)
    {
        dprintf("ip4");
    }
    else if (FAL_ACL_RULE_IP6 == *val)
    {
        dprintf("ip6");
    }
    else if (FAL_ACL_RULE_UDF == *val)
    {
        dprintf("udf");
    }
    else
    {
        dprintf("unknow");
    }
}
sw_error_t
cmd_data_check_ip_packet_type(char *cmd_str, a_uint32_t * arg_val,
                        a_uint32_t size)
{
    if (NULL == cmd_str)
    {
        return SW_BAD_VALUE;
    }

    if (!strcasecmp(cmd_str, "tcp"))
    {
        *arg_val = 0;
    }
    else if (!strcasecmp(cmd_str, "udp"))
    {
        *arg_val = 1;
    }
    else if (!strcasecmp(cmd_str, "udp-lite"))
    {
        *arg_val = 3;
    }
    else if (!strcasecmp(cmd_str, "arp"))
    {
        *arg_val = 5;
    }
    else if (!strcasecmp(cmd_str, "icmp"))
    {
        *arg_val = 7;
    }
    else
    {
        return SW_BAD_VALUE;
    }

    return SW_OK;
}


void
cmd_data_print_ip_packet_type(char * param_name, a_uint16_t * buf,
                        a_uint32_t size)
{
    a_uint16_t *val;

    val =  buf;
    dprintf("%s", param_name);

    if (0 == *val)
    {
        dprintf("tcp");
    }
    else if (1 == *val)
    {
        dprintf("udp");
    }
    else if (3 == *val)
    {
        dprintf("udp-lite");
    }
    else if (5 == *val)
    {
        dprintf("arp");
    }
    else if (7 == *val)
    {
        dprintf("icmp");
    }
    else
    {
        dprintf("unknow");
    }
}

sw_error_t
cmd_data_check_fieldop(char *cmdstr, fal_acl_field_op_t def,
                       fal_acl_field_op_t * val)
{
    if (0 == cmdstr[0])
    {
        *val = def;
    }
    else if ((!strcasecmp(cmdstr, "mask")) || (!strcasecmp(cmdstr, "m")))
    {
        *val = FAL_ACL_FIELD_MASK;
    }
    else if ((!strcasecmp(cmdstr, "range")) || (!strcasecmp(cmdstr, "r")))
    {
        *val = FAL_ACL_FIELD_RANGE;
    }
    else if ((!strcasecmp(cmdstr, "le")) || (!strcasecmp(cmdstr, "l")))
    {
        *val = FAL_ACL_FIELD_LE;
    }
    else if ((!strcasecmp(cmdstr, "ge")) || (!strcasecmp(cmdstr, "g")))
    {
        *val = FAL_ACL_FIELD_GE;
    }
    else if ((!strcasecmp(cmdstr, "ne")) || (!strcasecmp(cmdstr, "n")))
    {
        *val = FAL_ACL_FIELD_NE;
    }
    else
    {
        return SW_BAD_VALUE;
    }

    return SW_OK;
}

void
cmd_data_print_fieldop(char * param_name, a_uint32_t * buf,
                       a_uint32_t size)
{
    fal_acl_field_op_t *val;

    val = (fal_acl_field_op_t *) buf;
    dprintf("%s", param_name);

    if (FAL_ACL_FIELD_MASK == *val)
    {
        dprintf("mask");
    }
    else if (FAL_ACL_FIELD_RANGE == *val)
    {
        dprintf("range");
    }
    else if (FAL_ACL_FIELD_LE == *val)
    {
        dprintf("le");
    }
    else if (FAL_ACL_FIELD_GE == *val)
    {
        dprintf("ge");
    }
    else if (FAL_ACL_FIELD_NE == *val)
    {
        dprintf("ne");
    }
    else
    {
        dprintf("unknow");
    }
}

sw_error_t
cmd_data_check_ip4addr(char *cmdstr, void * val, a_uint32_t size)
{
    char *tmp = NULL, *str_save;
    a_uint32_t i = 0, j;
    a_uint32_t addr;
    fal_ip4_addr_t ip4;
    char cmd[128+1] = { 0 };

    memset(&ip4, 0, sizeof (fal_ip4_addr_t));
    if (NULL == cmdstr)
    {
        return SW_BAD_VALUE;
    }

    if (0 == cmdstr[0])
    {
        return SW_BAD_VALUE;
    }

    for (i = 0; i < 128; i++)
    {
        if (0 == cmdstr[i])
        {
            break;
        }
    }

    i++;
    if (128 < i)
    {
        i = 128;
    }

    memcpy(cmd, cmdstr, i);

	/* make sure the string can be terminated */
	cmd[i] = '\0';
    tmp = (void *) strtok_r(cmd, ".", &str_save);
    i = 0;
    while (tmp)
    {
        if (4 <= i)
        {
            return SW_BAD_VALUE;
        }

        if ((3 < strlen(tmp)) || (0 == strlen(tmp)))
        {
            return SW_BAD_VALUE;
        }

        for (j = 0; j < strlen(tmp); j++)
        {
            if (A_FALSE == is_dec(tmp[j]))
            {
                return SW_BAD_VALUE;
            }
        }

        sscanf(tmp, "%d", &addr);
        if (255 < addr)
        {
            return SW_BAD_VALUE;
        }

        ip4 |= ((addr & 0xff) << (24 - i * 8));
        i++;
        tmp = (void *) strtok_r(NULL, ".", &str_save);
    }

    if (4 != i)
    {
        return SW_BAD_VALUE;
    }

    *(fal_ip4_addr_t*)val = ip4;
    return SW_OK;
}

void
cmd_data_print_ip4addr(a_char_t * param_name, a_uint32_t * buf,
                       a_uint32_t size)
{
    a_uint32_t i;
    fal_ip4_addr_t ip4;

    ip4 = *((fal_ip4_addr_t *) buf);
    dprintf("%s", param_name);
    for (i = 0; i < 3; i++)
    {
        dprintf("%d.", (ip4 >> (24 - i * 8)) & 0xff);
    }
    dprintf("%d", (ip4 & 0xff));
}

sw_error_t
cmd_data_check_multi(char *info, void *val, a_uint32_t size)
{
    char *cmd;
    sw_error_t rv;
    fal_igmp_sg_entry_t entry;

    memset(&entry, 0, sizeof (fal_igmp_sg_entry_t));

    do
    {
        cmd = get_sub_cmd("group type", "0");
		SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: integer\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_uint32(cmd, &(entry.group.type), sizeof (a_uint32_t));
            if (SW_OK != rv)
                dprintf("usage: integer\n");
        }
    }
    while (talk_mode && (SW_OK != rv));

    if(entry.group.type==0)
    {
        cmd_data_check_element("group ip4 addr", "0.0.0.0",
                               "usage: the format is xx.xx.xx.xx \n",
                               cmd_data_check_ip4addr, (cmd, &(entry.group.u.ip4_addr), 4));
    }
    else
        cmd_data_check_element("group ip6 addr", NULL,
                               "usage: the format is xxxx::xxxx \n",
                               cmd_data_check_ip6addr, (cmd, &(entry.group.u.ip6_addr), 16));

    do
    {
        cmd = get_sub_cmd("source type", "0");
		SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: integer\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_uint32(cmd, &(entry.source.type), sizeof (a_uint32_t));
            if (SW_OK != rv)
                dprintf("usage: integer\n");
        }
    }
    while (talk_mode && (SW_OK != rv));

    if(entry.source.type==0)
    {
        cmd_data_check_element("source ip4 addr", "0.0.0.0",
                               "usage: the format is xx.xx.xx.xx \n",
                               cmd_data_check_ip4addr, (cmd, &(entry.source.u.ip4_addr), 4));
    }
    else
        cmd_data_check_element("source ip6 addr", NULL,
                               "usage: the format is xxxx::xxxx \n",
                               cmd_data_check_ip6addr, (cmd, &(entry.source.u.ip6_addr), 16));

    do
    {
        cmd = get_sub_cmd("portmap", "0");
		SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: integer\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_uint32(cmd, &(entry.port_map), sizeof (a_uint32_t));
            if (SW_OK != rv)
                dprintf("usage: integer\n");
        }
    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("vlanid", "0xffff");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: the range is 0 -- 4095 or 65535\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_uint32(cmd, &(entry.vlan_id), sizeof (a_uint32_t));
            if (SW_OK != rv)
                dprintf("usage: the range is 0 -- 4095 or 65535\n");
        }
    }
    while (talk_mode && (SW_OK != rv));

    *(fal_igmp_sg_entry_t *)val = entry;

    return SW_OK;
}
void
cmd_data_print_multi(a_uint8_t * param_name, a_uint32_t * buf, a_uint32_t size)
{
    fal_igmp_sg_entry_t *entry;

    entry = (fal_igmp_sg_entry_t *) buf;

    dprintf("\n[multicast info]:  [group type]:%x [source type]:%x ", entry->group.type, entry->source.type);

    if(entry->group.type == 0)
        cmd_data_print_ip4addr("\n[group ip4 addr]:",
                               (a_uint32_t *) & (entry->group.u.ip4_addr),
                               sizeof (fal_ip4_addr_t));
    else
        cmd_data_print_ip6addr("\n[group ip6 addr]:",
                               (a_uint32_t *) & (entry->group.u.ip6_addr),
                               sizeof (fal_ip6_addr_t));

    if(entry->source.type == 0)
        cmd_data_print_ip4addr("\n[source ip4 addr]:",
                               (a_uint32_t *) & (entry->source.u.ip4_addr),
                               sizeof (fal_ip4_addr_t));
    else
        cmd_data_print_ip6addr("\n[source ip6 addr]:",
                               (a_uint32_t *) & (entry->source.u.ip6_addr),
                               sizeof (fal_ip6_addr_t));

    dprintf("\n[entry portmap]: [portmap]:0x%x  ", entry->port_map);
    dprintf("\n[entry vlanid]: [vlanid]:%d  ", entry->vlan_id);

}

sw_error_t
cmd_data_check_ip6addr(char *cmdstr, void * val, a_uint32_t size)
{
    char *tmp = NULL, *str_save;
    a_uint32_t j;
    a_uint32_t i = 0, cnt = 0, rep = 0, loc = 0;
    a_uint32_t data;
    a_uint32_t addr[8];
    fal_ip6_addr_t ip6;

    if (NULL == cmdstr)
    {
        return SW_BAD_VALUE;
    }

    if (0 == cmdstr[0])
    {
        return SW_BAD_VALUE;
    }

    for (i = 0; i < 8; i++)
    {
        addr[i] = 0;
    }

    for (i = 0; i < strlen(cmdstr); i++)
    {
        if (':' == cmdstr[i])
        {
            if ((i == (strlen(cmdstr) - 1))
                    || (0 == i))
            {
                return SW_BAD_VALUE;
            }
            cnt++;

            if (':' == cmdstr[i - 1])
            {
                rep++;
                loc = cnt - 1;
            }
        }
    }

    if (1 < rep)
    {
        return SW_BAD_VALUE;
    }

    tmp = (void *) strtok_r(cmdstr, ":", &str_save);
    i = 0;
    while (tmp)
    {
        if (8 <= i)
        {
            return SW_BAD_VALUE;
        }

        if ((4 < strlen(tmp)) || (0 == strlen(tmp)))
        {
            return SW_BAD_VALUE;
        }

        for (j = 0; j < strlen(tmp); j++)
        {
            if (A_FALSE == is_hex(tmp[j]))
            {
                return SW_BAD_VALUE;
            }
        }

        sscanf(tmp, "%x", &data);
        if (65535 < data)
        {
            return SW_BAD_VALUE;
        }

        addr[i++] = data;
        tmp = (void *) strtok_r(NULL, ":", &str_save);
    }

    if (0 == rep)
    {
        if (8 != i)
        {
            return SW_BAD_VALUE;
        }
    }
    else
    {
        if (8 <= i)
        {
            return SW_BAD_VALUE;
        }

        for (j = i - 1; j >= loc; j--)
        {
            addr[8 - i + j] = addr[j];
            addr[j] = 0;
        }
    }

    for (i = 0; i < 4; i++)
    {
        ip6.ul[i] = (addr[i * 2] << 16) | addr[i * 2 + 1];
    }

    dprintf("\n");
    for (i = 0; i < 4; i++)
    {
        dprintf("%08x  ", ip6.ul[i]);
    }
    dprintf("\n");

    *(fal_ip6_addr_t*)val = ip6;
    return SW_OK;
}

void
cmd_data_print_ip6addr(a_char_t * param_name, a_uint32_t * buf,
                       a_uint32_t size)
{
    a_uint32_t i;
    fal_ip6_addr_t ip6;

    ip6 = *(fal_ip6_addr_t *) buf;
    dprintf("%s", param_name);
    for (i = 0; i < 3; i++)
    {
        dprintf("%x:%x:", (ip6.ul[i] >> 16) & 0xffff, ip6.ul[i] & 0xffff);
    }
    dprintf("%x:%x", (ip6.ul[3] >> 16) & 0xffff, ip6.ul[3] & 0xffff);
}

sw_error_t
cmd_data_check_mac_field(fal_acl_rule_t * entry)
{
    char *cmd;
    a_uint32_t tmpdata = 0;

    /* get fake mac header field configuration */
    cmd_data_check_element("Fake mac header field", "no", "usage: <yes/no/y/n>\n",
                           cmd_data_check_confirm, (cmd, A_FALSE, &tmpdata,
                                   sizeof (a_bool_t)));
    if(tmpdata)
    {
            entry->is_fake_mac_header_mask = 1;
	        cmd_data_check_element("Is fake mac header", "no", "usage: <yes/no/y/n>\n",
	                           cmd_data_check_confirm, (cmd, A_FALSE, &entry->is_fake_mac_header_val,
	                                   sizeof (a_bool_t)));
            FAL_FIELD_FLG_SET(entry->field_flg, FAL_ACL_FIELD_FAKE_MAC_HEADER);
    }

    /* get SNAP field configuration */
    cmd_data_check_element("SNAP/LLC other field", "no", "usage: <yes/no/y/n>\n",
                           cmd_data_check_confirm, (cmd, A_FALSE, &tmpdata,
                                   sizeof (a_bool_t)));
    if(tmpdata)
    {
            entry->is_snap_mask= 1;
	        cmd_data_check_element("Is SNAP packet", "no", "usage: <yes/no/y/n>\n",
	                           cmd_data_check_confirm, (cmd, A_FALSE, &entry->is_snap_val,
	                                   sizeof (a_bool_t)));
            FAL_FIELD_FLG_SET(entry->field_flg, FAL_ACL_FIELD_SNAP);
    }

    /* get ethernet field configuration */
    cmd_data_check_element("ethernet/other field", "no", "usage: <yes/no/y/n>\n",
                           cmd_data_check_confirm, (cmd, A_FALSE, &tmpdata,
                                   sizeof (a_bool_t)));
    if(tmpdata)
    {
            entry->is_ethernet_mask = 1;
	    cmd_data_check_element("Is ethernet packet", "no", "usage: <yes/no/y/n>\n",
	                           cmd_data_check_confirm, (cmd, A_FALSE, &entry->is_ethernet_val,
	                                   sizeof (a_bool_t)));
            FAL_FIELD_FLG_SET(entry->field_flg, FAL_ACL_FIELD_ETHERNET);
    }

    /* get IP/NON-IP field configuration */
    cmd_data_check_element("IP/NON-IP field", "no", "usage: <yes/no/y/n>\n",
                           cmd_data_check_confirm, (cmd, A_FALSE, &tmpdata,
                                   sizeof (a_bool_t)));
    if(tmpdata)
    {
            entry->is_ip_mask = 1;
	    cmd_data_check_element("Is IP packet", "no", "usage: <yes/no/y/n>\n",
	                           cmd_data_check_confirm, (cmd, A_FALSE, &entry->is_ip_val,
	                                   sizeof (a_bool_t)));
            FAL_FIELD_FLG_SET(entry->field_flg, FAL_ACL_FIELD_IP);
    }
    /* get IPv4/IPv6 field configuration */
    cmd_data_check_element("IPv4/IPv6 field", "no", "usage: <yes/no/y/n>\n",
                           cmd_data_check_confirm, (cmd, A_FALSE, &tmpdata,
                                   sizeof (a_bool_t)));
    if(tmpdata)
    {
            entry->is_ipv6_mask = 1;
	    cmd_data_check_element("Is IPv6 packet", "no", "usage: <yes/no/y/n>\n",
	                           cmd_data_check_confirm, (cmd, A_FALSE, &entry->is_ipv6_val,
	                                   sizeof (a_bool_t)));
            FAL_FIELD_FLG_SET(entry->field_flg, FAL_ACL_FIELD_IPV6);
    }
    /* get destination mac address field configuration */
    cmd_data_check_element("mac dst addr field", "no", "usage: <yes/no/y/n>\n",
                           cmd_data_check_confirm, (cmd, A_FALSE, &tmpdata,
                                   sizeof (a_bool_t)));

    if (tmpdata)
    {
        cmd_data_check_element("dst mac addr", NULL,
                               "usage: the format is xx-xx-xx-xx-xx-xx \n",
                               cmd_data_check_macaddr, (cmd,
                                       &(entry->dest_mac_val),
                                       sizeof
                                       (fal_mac_addr_t)));

        cmd_data_check_element("dst mac addr mask", NULL,
                               "usage: the format is xx-xx-xx-xx-xx-xx \n",
                               cmd_data_check_macaddr, (cmd,
                                       &(entry->dest_mac_mask),
                                       sizeof
                                       (fal_mac_addr_t)));

        FAL_FIELD_FLG_SET(entry->field_flg, FAL_ACL_FIELD_MAC_DA);
    }

    /* get source mac address field configuration */
    cmd_data_check_element("mac src addr field", "no",  "usage: <yes/no/y/n>\n",
                           cmd_data_check_confirm, (cmd, A_FALSE, &tmpdata,
                                   sizeof (a_bool_t)));

    if (tmpdata)
    {
        cmd_data_check_element("src mac addr", NULL,
                               "usage: the format is xx-xx-xx-xx-xx-xx \n",
                               cmd_data_check_macaddr, (cmd,
                                       &(entry->src_mac_val),
                                       sizeof
                                       (fal_mac_addr_t)));

        cmd_data_check_element("src mac addr mask", NULL,
                               "usage: the format is xx-xx-xx-xx-xx-xx \n",
                               cmd_data_check_macaddr, (cmd,
                                       &(entry->src_mac_mask),
                                       sizeof
                                       (fal_mac_addr_t)));

        FAL_FIELD_FLG_SET(entry->field_flg, FAL_ACL_FIELD_MAC_SA);
    }

    /* get ethernet type field configuration */
    cmd_data_check_element("ethernet type field", "no", "usage: <yes/no/y/n>\n",
                           cmd_data_check_confirm, (cmd, A_FALSE, &tmpdata,
                                   sizeof (a_bool_t)));

    if (tmpdata)
    {
        cmd_data_check_element("ethernet type", NULL,
                               "usage: the format is 0x0-0xffff or 0-65535\n",
                               cmd_data_check_integer, (cmd, &tmpdata, 0xffff,
                                       0x0));
        entry->ethtype_val = tmpdata & 0xffff;

        cmd_data_check_element("ethernet type mask", NULL,
                               "usage: the format is 0x0-0xffff or 0-65535\n",
                               cmd_data_check_integer, (cmd, &tmpdata, 0xffff,
                                       0x0));
        entry->ethtype_mask = tmpdata & 0xffff;

        FAL_FIELD_FLG_SET(entry->field_flg, FAL_ACL_FIELD_MAC_ETHTYPE);
    }

    /* get vlanid field configuration */
    cmd_data_check_element("vlanid field", "no", "usage: <yes/no/y/n>\n",
                           cmd_data_check_confirm, (cmd, A_FALSE, &tmpdata,
                                   sizeof (a_bool_t)));

    if (tmpdata)
    {
        cmd_data_check_element("vlanid opration", "mask",
                               "usage: <mask/range/le/ge/ne> \n",
                               cmd_data_check_fieldop, (cmd, FAL_ACL_FIELD_MASK,
                                       &(entry->vid_op)));

        if (FAL_ACL_FIELD_MASK == entry->vid_op)
        {
            cmd_data_check_element("vlanid", NULL,
                                   "usage: the format is 0x0-0xfff or 0-4095 \n",
                                   cmd_data_check_integer, (cmd, &tmpdata,
                                           0xfff, 0x0));
            entry->vid_val = tmpdata & 0xfff;

            cmd_data_check_element("vlanid mask", NULL,
                                   "usage: the format is 0x0-0xfff or 0-4095 \n",
                                   cmd_data_check_integer, (cmd, &tmpdata,
                                           0xfff, 0x0));
            entry->vid_mask = tmpdata & 0xfff;
        }
        else if (FAL_ACL_FIELD_RANGE == entry->vid_op)
        {
            cmd_data_check_element("vlanid low", NULL,
                                   "usage: the format is 0x0-0xfff or 0-4095 \n",
                                   cmd_data_check_integer, (cmd, &tmpdata,
                                           0xfff, 0x0));
            entry->vid_val = tmpdata & 0xfff;

            cmd_data_check_element("vlanid high", NULL,
                                   "usage: the format is 0x0-0xfff or 0-4095 \n",
                                   cmd_data_check_integer, (cmd, &tmpdata,
                                           0xfff, 0x0));
            entry->vid_mask = tmpdata & 0xfff;
        }
        else
        {
            cmd_data_check_element("vlanid", NULL,
                                   "usage: the format is 0x0-0xfff or 0-4095 \n",
                                   cmd_data_check_integer, (cmd, &tmpdata,
                                           0xfff, 0x0));
            entry->vid_val = tmpdata & 0xfff;
        }

        FAL_FIELD_FLG_SET(entry->field_flg, FAL_ACL_FIELD_MAC_VID);
    }

    /* get vlan tagged field configuration */
    cmd_data_check_element("vlan tagged field", "no", "usage: <yes/no/y/n>\n",
                           cmd_data_check_confirm, (cmd, A_FALSE, &tmpdata,
                                   sizeof (a_bool_t)));

    if (tmpdata)
    {
        cmd_data_check_element("tagged", NULL,
                               "usage: the format is 0x0-0x1 or 0-1 \n",
                               cmd_data_check_integer, (cmd, &tmpdata, 0x1,
                                       0x0));
        entry->tagged_val = tmpdata & 0x1;

        cmd_data_check_element("tagged mask", NULL,
                               "usage: the format is 0x0-0x1 or 0-1 \n",
                               cmd_data_check_integer, (cmd, &tmpdata, 0x1,
                                       0x0));
        entry->tagged_mask = tmpdata & 0x1;

        FAL_FIELD_FLG_SET(entry->field_flg, FAL_ACL_FIELD_MAC_TAGGED);
    }

    /* get up field configuration */
    cmd_data_check_element("up field", "no", "usage: <yes/no/y/n>\n",
                           cmd_data_check_confirm, (cmd, A_FALSE, &tmpdata,
                                   sizeof (a_bool_t)));

    if (tmpdata)
    {
        cmd_data_check_element("up", NULL,
                               "usage: the format is 0x0-0x7 or 0-7 \n",
                               cmd_data_check_integer, (cmd, &tmpdata, 0x7,
                                       0x0));
        entry->up_val = tmpdata & 0x7;

        cmd_data_check_element("up mask", NULL,
                               "usage: the format is 0x0-0x7 or 0-7 \n",
                               cmd_data_check_integer, (cmd, &tmpdata, 0x7,
                                       0x0));
        entry->up_mask = tmpdata & 0x7;

        FAL_FIELD_FLG_SET(entry->field_flg, FAL_ACL_FIELD_MAC_UP);
    }

    /* get cfi field configuration */
    cmd_data_check_element("cfi field", "no", "usage: <yes/no/y/n>\n",
                           cmd_data_check_confirm, (cmd, A_FALSE, &tmpdata,
                                   sizeof (a_bool_t)));

    if (tmpdata)
    {
        cmd_data_check_element("cfi", NULL,
                               "usage: the format is 0x0-0x1 or 0-1 \n",
                               cmd_data_check_integer, (cmd, &tmpdata, 0x1,
                                       0x0));
        entry->cfi_val = tmpdata & 0x1;

        cmd_data_check_element("cfi mask", NULL,
                               "usage: the format is 0x0-0x1 or 0-1 \n",
                               cmd_data_check_integer, (cmd, &tmpdata, 0x1,
                                       0x0));
        entry->cfi_mask = tmpdata & 0x1;

        FAL_FIELD_FLG_SET(entry->field_flg, FAL_ACL_FIELD_MAC_CFI);
    }

    /* get svlan tagged field configuration */
    cmd_data_check_element("svlan tagged field", "no", "usage: <yes/no/y/n>\n",
                           cmd_data_check_confirm, (cmd, A_FALSE, &tmpdata,
                                   sizeof (a_bool_t)));

    if (tmpdata)
    {
        cmd_data_check_element("stagged", NULL,
                               "usage: the format is 0x0-0x7\n",
                               cmd_data_check_integer, (cmd, &tmpdata, 0x7,
                                       0x0));
        entry->stagged_val = tmpdata & 0x7;

        cmd_data_check_element("stagged mask", NULL,
                               "usage: the format is 0x0-0x7\n",
                               cmd_data_check_integer, (cmd, &tmpdata, 0x7,
                                       0x0));
        entry->stagged_mask = tmpdata & 0x7;

        FAL_FIELD_FLG_SET(entry->field_flg, FAL_ACL_FIELD_MAC_STAGGED);
    }

    /* get stag vlanid field configuration */
    cmd_data_check_element("stag vid field", "no", "usage: <yes/no/y/n>\n",
                           cmd_data_check_confirm, (cmd, A_FALSE, &tmpdata,
                                   sizeof (a_bool_t)));

    if (tmpdata)
    {
        cmd_data_check_element("stag vid opration", "mask",
                               "usage: <mask/range/le/ge/ne> \n",
                               cmd_data_check_fieldop, (cmd, FAL_ACL_FIELD_MASK,
                                       &(entry->stag_vid_op)));
        if (FAL_ACL_FIELD_MASK == entry->stag_vid_op)
        {
            cmd_data_check_element("stag vid", NULL,
                                   "usage: the format is 0x0-0xfff or 0-4095 \n",
                                   cmd_data_check_integer, (cmd, &tmpdata,
                                           0xfff, 0x0));
            entry->stag_vid_val = tmpdata & 0xfff;

            cmd_data_check_element("stag vid mask", NULL,
                                   "usage: the format is 0x0-0xfff or 0-4095 \n",
                                   cmd_data_check_integer, (cmd, &tmpdata,
                                           0xfff, 0x0));
            entry->stag_vid_mask = tmpdata & 0xfff;
        }
        else if (FAL_ACL_FIELD_RANGE == entry->stag_vid_op)
        {
            cmd_data_check_element("stag vid low", NULL,
                                   "usage: the format is 0x0-0xfff or 0-4095 \n",
                                   cmd_data_check_integer, (cmd, &tmpdata,
                                           0xfff, 0x0));
            entry->stag_vid_val = tmpdata & 0xfff;

            cmd_data_check_element("stag vid high", NULL,
                                   "usage: the format is 0x0-0xfff or 0-4095 \n",
                                   cmd_data_check_integer, (cmd, &tmpdata,
                                           0xfff, 0x0));
            entry->stag_vid_mask = tmpdata & 0xfff;
        }
        else
        {
            cmd_data_check_element("stag vid", NULL,
                                   "usage: the format is 0x0-0xfff or 0-4095 \n",
                                   cmd_data_check_integer, (cmd, &tmpdata,
                                           0xfff, 0x0));
            entry->stag_vid_val = tmpdata & 0xfff;
        }
        FAL_FIELD_FLG_SET(entry->field_flg, FAL_ACL_FIELD_MAC_STAG_VID);
    }


    /* get stag priority field configuration */
    cmd_data_check_element("stag pri field", "no", "usage: <yes/no/y/n>\n",
                           cmd_data_check_confirm, (cmd, A_FALSE, &tmpdata,
                                   sizeof (a_bool_t)));

    if (tmpdata)
    {
        cmd_data_check_element("stag pri", NULL,
                               "usage: the format is 0x0-0x7 or 0-7 \n",
                               cmd_data_check_integer, (cmd, &tmpdata, 0x7,
                                       0x0));
        entry->stag_pri_val = tmpdata & 0x7;

        cmd_data_check_element("stag pri mask", NULL,
                               "usage: the format is 0x0-0x7 or 0-7 \n",
                               cmd_data_check_integer, (cmd, &tmpdata, 0x7,
                                       0x0));
        entry->stag_pri_mask = tmpdata & 0x7;

        FAL_FIELD_FLG_SET(entry->field_flg, FAL_ACL_FIELD_MAC_STAG_PRI);
    }

    /* get stag dei field configuration */
    cmd_data_check_element("stag dei field", "no", "usage: <yes/no/y/n>\n",
                           cmd_data_check_confirm, (cmd, A_FALSE, &tmpdata,
                                   sizeof (a_bool_t)));

    if (tmpdata)
    {
        cmd_data_check_element("stag dei", NULL,
                               "usage: the format is 0x0-0x1 or 0-1 \n",
                               cmd_data_check_integer, (cmd, &tmpdata, 0x1,
                                       0x0));
        entry->stag_dei_val = tmpdata & 0x1;

        cmd_data_check_element("stag dei mask", NULL,
                               "usage: the format is 0x0-0x1 or 0-1 \n",
                               cmd_data_check_integer, (cmd, &tmpdata, 0x1,
                                       0x0));
        entry->stag_dei_mask = tmpdata & 0x1;

        FAL_FIELD_FLG_SET(entry->field_flg, FAL_ACL_FIELD_MAC_STAG_DEI);
    }

    /* get cvlan tagged field configuration */
    cmd_data_check_element("cvlan tagged field", "no", "usage: <yes/no/y/n>\n",
                           cmd_data_check_confirm, (cmd, A_FALSE, &tmpdata,
                                   sizeof (a_bool_t)));

    if (tmpdata)
    {
        cmd_data_check_element("ctagged", NULL,
                               "usage: the format is 0x0-0x7\n",
                               cmd_data_check_integer, (cmd, &tmpdata, 0x7,
                                       0x0));
        entry->ctagged_val = tmpdata & 0x7;

        cmd_data_check_element("ctagged mask", NULL,
                               "usage: the format is 0x0-0x7\n",
                               cmd_data_check_integer, (cmd, &tmpdata, 0x7,
                                       0x0));
        entry->ctagged_mask = tmpdata & 0x7;

        FAL_FIELD_FLG_SET(entry->field_flg, FAL_ACL_FIELD_MAC_CTAGGED);
    }

    /* get ctag vlanid field configuration */
    cmd_data_check_element("ctag vid field", "no", "usage: <yes/no/y/n>\n",
                           cmd_data_check_confirm, (cmd, A_FALSE, &tmpdata,
                                   sizeof (a_bool_t)));

    if (tmpdata)
    {
        cmd_data_check_element("ctag vid opration", "mask",
                               "usage: <mask/range/le/ge/ne> \n",
                               cmd_data_check_fieldop, (cmd, FAL_ACL_FIELD_MASK,
                                       &(entry->ctag_vid_op)));

        if (FAL_ACL_FIELD_MASK == entry->ctag_vid_op)
        {
            cmd_data_check_element("ctag vid", NULL,
                                   "usage: the format is 0x0-0xfff or 0-4095 \n",
                                   cmd_data_check_integer, (cmd, &tmpdata,
                                           0xfff, 0x0));
            entry->ctag_vid_val = tmpdata & 0xfff;

            cmd_data_check_element("ctag vid mask", NULL,
                                   "usage: the format is 0x0-0xfff or 0-4095 \n",
                                   cmd_data_check_integer, (cmd, &tmpdata,
                                           0xfff, 0x0));
            entry->ctag_vid_mask = tmpdata & 0xfff;
        }
        else if (FAL_ACL_FIELD_RANGE == entry->ctag_vid_op)
        {
            cmd_data_check_element("ctag vid low", NULL,
                                   "usage: the format is 0x0-0xfff or 0-4095 \n",
                                   cmd_data_check_integer, (cmd, &tmpdata,
                                           0xfff, 0x0));
            entry->ctag_vid_val = tmpdata & 0xfff;

            cmd_data_check_element("ctag vid high", NULL,
                                   "usage: the format is 0x0-0xfff or 0-4095 \n",
                                   cmd_data_check_integer, (cmd, &tmpdata,
                                           0xfff, 0x0));
            entry->ctag_vid_mask = tmpdata & 0xfff;
        }
        else
        {
            cmd_data_check_element("ctag vid", NULL,
                                   "usage: the format is 0x0-0xfff or 0-4095 \n",
                                   cmd_data_check_integer, (cmd, &tmpdata,
                                           0xfff, 0x0));
            entry->ctag_vid_val = tmpdata & 0xfff;
        }

        FAL_FIELD_FLG_SET(entry->field_flg, FAL_ACL_FIELD_MAC_CTAG_VID);
    }

    /* get ctag priority field configuration */
    cmd_data_check_element("ctag pri field", "no", "usage: <yes/no/y/n>\n",
                           cmd_data_check_confirm, (cmd, A_FALSE, &tmpdata,
                                   sizeof (a_bool_t)));

    if (tmpdata)
    {
        cmd_data_check_element("ctag pri", NULL,
                               "usage: the format is 0x0-0x7 or 0-7 \n",
                               cmd_data_check_integer, (cmd, &tmpdata, 0x7,
                                       0x0));
        entry->ctag_pri_val = tmpdata & 0x7;

        cmd_data_check_element("ctag pri mask", NULL,
                               "usage: the format is 0x0-0x7 or 0-7 \n",
                               cmd_data_check_integer, (cmd, &tmpdata, 0x7,
                                       0x0));
        entry->ctag_pri_mask = tmpdata & 0x7;

        FAL_FIELD_FLG_SET(entry->field_flg, FAL_ACL_FIELD_MAC_CTAG_PRI);
    }

    /* get ctag cfi field configuration */
    cmd_data_check_element("ctag cfi field", "no", "usage: <yes/no/y/n>\n",
                           cmd_data_check_confirm, (cmd, A_FALSE, &tmpdata,
                                   sizeof (a_bool_t)));

    if (tmpdata)
    {
        cmd_data_check_element("ctag cfi", NULL,
                               "usage: the format is 0x0-0x1 or 0-1 \n",
                               cmd_data_check_integer, (cmd, &tmpdata, 0x1,
                                       0x0));
        entry->ctag_cfi_val = tmpdata & 0x1;

        cmd_data_check_element("ctag cfi mask", NULL,
                               "usage: the format is 0x0-0x1 or 0-1 \n",
                               cmd_data_check_integer, (cmd, &tmpdata, 0x1,
                                       0x0));
        entry->ctag_cfi_mask = tmpdata & 0x1;

        FAL_FIELD_FLG_SET(entry->field_flg, FAL_ACL_FIELD_MAC_CTAG_CFI);
    }

    /* get vsi valid field configuration */
    cmd_data_check_element("vsi valid field", "no", "usage: <yes/no/y/n>\n",
                           cmd_data_check_confirm, (cmd, A_FALSE, &tmpdata,
                                   sizeof (a_bool_t)));
    if(tmpdata)
    {
            entry->vsi_valid_mask = 1;
	    cmd_data_check_element("is vsi valid", "no", "usage: <yes/no/y/n>\n",
	                           cmd_data_check_confirm, (cmd, A_FALSE, &entry->vsi_valid,
	                                   sizeof (a_bool_t)));
        FAL_FIELD_FLG_SET(entry->field_flg, FAL_ACL_FIELD_VSI_VALID);
    }

    /* get vsi field configuration */
    cmd_data_check_element("vsi field", "no", "usage: <yes/no/y/n>\n",
                           cmd_data_check_confirm, (cmd, A_FALSE, &tmpdata,
                                   sizeof (a_bool_t)));

    if (tmpdata)
    {
        cmd_data_check_element("vsi", "0x0",
                               "usage: the format is 0x0-0x1f or 0-31 \n",
                               cmd_data_check_integer, (cmd, &tmpdata, 0x1f,
                                       0x0));
        entry->vsi = tmpdata & 0x1f;

        cmd_data_check_element("vsi mask", NULL,
                               "usage: the format is 0x0-0x1f or 0-31 \n",
                               cmd_data_check_integer, (cmd, &tmpdata, 0x1f,
                                       0x0));
        entry->vsi_mask = tmpdata & 0x1f;

        FAL_FIELD_FLG_SET(entry->field_flg, FAL_ACL_FIELD_VSI);
    }

    /* get pppoe session id field configuration */
    cmd_data_check_element("pppoe session id field", "no", "usage: <yes/no/y/n>\n",
                           cmd_data_check_confirm, (cmd, A_FALSE, &tmpdata,
                                   sizeof (a_bool_t)));

    if (tmpdata)
    {
        cmd_data_check_element("pppoe session id", "0x0",
                               "usage: the format is 0x0-0xffff or 0-65535 \n",
                               cmd_data_check_integer, (cmd, &tmpdata, 0xffff,
                                       0x0));
        entry->pppoe_sessionid = tmpdata & 0xffff;

        cmd_data_check_element("pppoe session id mask", NULL,
                               "usage: the format is 0x0-0xffff or 0-65535 \n",
                               cmd_data_check_integer, (cmd, &tmpdata, 0xffff,
                                       0x0));
        entry->pppoe_sessionid_mask = tmpdata & 0xffff;

        FAL_FIELD_FLG_SET(entry->field_flg, FAL_ACL_FIELD_PPPOE_SESSIONID);
    }
    return SW_OK;
}

sw_error_t
cmd_data_check_ip4_field(fal_acl_rule_t * entry)
{
    char *cmd;
    a_uint32_t tmpdata = 0;

    /* get ip4 source address field configuration */
    cmd_data_check_element("ip4 src address field", "no",
                           "usage: <yes/no/y/n>\n", cmd_data_check_confirm,
                           (cmd, A_FALSE, &tmpdata, sizeof (tmpdata)));

    if (tmpdata)
    {
        cmd_data_check_element("ip4 src addr", NULL,
                               "usage: the format is xx.xx.xx.xx \n",
                               cmd_data_check_ip4addr, (cmd,
                                       &(entry->src_ip4_val), 4));

        cmd_data_check_element("ip4 src addr mask", NULL,
                               "usage: the format is xx.xx.xx.xx \n",
                               cmd_data_check_ip4addr, (cmd,
                                       &(entry->src_ip4_mask), 4));

        FAL_FIELD_FLG_SET(entry->field_flg, FAL_ACL_FIELD_IP4_SIP);
    }

    /* get ip4 destination address field configuration */
    cmd_data_check_element("ip4 dst address field", "no",
                           "usage: <yes/no/y/n>\n", cmd_data_check_confirm,
                           (cmd, A_FALSE, &tmpdata, sizeof (tmpdata)));

    if (tmpdata)
    {
        cmd_data_check_element("ip4 dst addr", NULL,
                               "usage: the format is xx.xx.xx.xx \n",
                               cmd_data_check_ip4addr, (cmd,
                                       &(entry->
                                         dest_ip4_val), 4));

        cmd_data_check_element("ip4 dst addr mask", NULL,
                               "usage: the format is xx.xx.xx.xx \n",
                               cmd_data_check_ip4addr, (cmd,
                                       &(entry->
                                         dest_ip4_mask), 4));

        FAL_FIELD_FLG_SET(entry->field_flg, FAL_ACL_FIELD_IP4_DIP);
    }

    /* get ripv1 field configuration */
    cmd_data_check_element("ripv1 field", "no", "usage: <yes/no/y/n>\n",
                           cmd_data_check_confirm, (cmd, A_FALSE, &tmpdata,
                                   sizeof (tmpdata)));

    if (tmpdata)
    {
        cmd_data_check_element("ripv1", NULL,
                               "usage: the format is 0x0-0x1 or 0-1 \n",
                               cmd_data_check_integer, (cmd, &tmpdata, 0x1,
                                       0x0));
        entry->ripv1_val = tmpdata & 0x1;

        cmd_data_check_element("ripv1 mask", NULL,
                               "usage: the format is 0x0-0x1 or 0-1 \n",
                               cmd_data_check_integer, (cmd, &tmpdata, 0x1,
                                       0x0));
        entry->ripv1_mask = tmpdata & 0x1;

        FAL_FIELD_FLG_SET(entry->field_flg, FAL_ACL_FIELD_RIPV1);
    }

    /* get dhcpv4 field configuration */
    cmd_data_check_element("dhcpv4 field", "no", "usage: <yes/no/y/n>\n",
                           cmd_data_check_confirm, (cmd, A_FALSE, &tmpdata,
                                   sizeof (tmpdata)));

    if (tmpdata)
    {
        cmd_data_check_element("dhcpv4", NULL,
                               "usage: the format is 0x0-0x1 or 0-1 \n",
                               cmd_data_check_integer, (cmd, &tmpdata, 0x1,
                                       0x0));
        entry->dhcpv4_val = tmpdata & 0x1;

        cmd_data_check_element("dhcpv4 mask", NULL,
                               "usage: the format is 0x0-0x1 or 0-1 \n",
                               cmd_data_check_integer, (cmd, &tmpdata, 0x1,
                                       0x0));
        entry->dhcpv4_mask = tmpdata & 0x1;

        FAL_FIELD_FLG_SET(entry->field_flg, FAL_ACL_FIELD_DHCPV4);
    }

    /* get ipv4 option field configuration */
    cmd_data_check_element("ipv4 option field", "no", "usage: <yes/no/y/n>\n",
                           cmd_data_check_confirm, (cmd, A_FALSE, &tmpdata,
                                   sizeof (tmpdata)));

    if (tmpdata)
    {
            entry->is_ipv4_option_mask = 1;
	    cmd_data_check_element("Is ipv4 option", "no", "usage: <yes/no/y/n>\n",
	                           cmd_data_check_confirm, (cmd, A_FALSE, &entry->is_ipv4_option_val,
	                                   sizeof (a_bool_t)));
            FAL_FIELD_FLG_SET(entry->field_flg, FAL_ACL_FIELD_IPV4_OPTION);
    }

    return SW_OK;
}

sw_error_t
cmd_data_check_ip6_field(fal_acl_rule_t * entry)
{
    char *cmd;
    a_uint32_t tmpdata = 0;

    /* get ip6 source address field configuration */
    cmd_data_check_element("ip6 src address field", "no",
                           "usage: <yes/no/y/n>\n", cmd_data_check_confirm,
                           (cmd, A_FALSE, &tmpdata, sizeof (tmpdata)));

    if (tmpdata)
    {
        cmd_data_check_element("ip6 src addr", NULL,
                               "usage: the format is xxxx::xxxx \n",
                               cmd_data_check_ip6addr, (cmd,
                                       &(entry->src_ip6_val), 16));

        cmd_data_check_element("ip6 src addr mask", NULL,
                               "usage: the format is xxxx::xxxx \n",
                               cmd_data_check_ip6addr, (cmd,
                                       &(entry->
                                         src_ip6_mask), 16));

        FAL_FIELD_FLG_SET(entry->field_flg, FAL_ACL_FIELD_IP6_SIP);
    }

    /* get ip6 destination address field configuration */
    cmd_data_check_element("ip6 dst address field", "no",
                           "usage: <yes/no/y/n>\n", cmd_data_check_confirm,
                           (cmd, A_FALSE, &tmpdata, sizeof (tmpdata)));

    if (tmpdata)
    {
        cmd_data_check_element("ip6 dst addr", NULL,
                               "usage: the format is xxxx::xxxx \n",
                               cmd_data_check_ip6addr, (cmd,
                                       &(entry->
                                         dest_ip6_val), 16));

        cmd_data_check_element("ip6 dst addr mask", NULL,
                               "usage: the format is xxxx::xxxx \n",
                               cmd_data_check_ip6addr, (cmd,
                                       &(entry->
                                         dest_ip6_mask), 16));

        FAL_FIELD_FLG_SET(entry->field_flg, FAL_ACL_FIELD_IP6_DIP);
    }

    /* get ip6 flow label field configuration */
    cmd_data_check_element("ip6 flow label field", "no",
                           "usage: <yes/no/y/n>\n", cmd_data_check_confirm,
                           (cmd, A_FALSE, &tmpdata, sizeof (tmpdata)));

    if (tmpdata)
    {
        cmd_data_check_element("ip6 label", NULL,
                               "usage: the format is 0x0-0xfffff or 0-1048575\n",
                               cmd_data_check_integer, (cmd,
                                       &(entry->ip6_lable_val),
                                       0xfffff, 0x0));

        cmd_data_check_element("ip6 label mask", NULL,
                               "usage: the format is 0x0-0xfffff or 0-1048575\n",
                               cmd_data_check_integer, (cmd,
                                       &(entry->
                                         ip6_lable_mask),
                                       0xfffff, 0x0));

        FAL_FIELD_FLG_SET(entry->field_flg, FAL_ACL_FIELD_IP6_LABEL);
    }

    /* get dhcpv6 field configuration */
    cmd_data_check_element("dhcpv6 field", "no", "usage: <yes/no/y/n>\n",
                           cmd_data_check_confirm, (cmd, A_FALSE, &tmpdata,
                                   sizeof (tmpdata)));

    if (tmpdata)
    {
        cmd_data_check_element("dhcpv6", NULL,
                               "usage: the format is 0x0-0x1 or 0-1 \n",
                               cmd_data_check_integer, (cmd, &tmpdata, 0xff,
                                       0x0));
        entry->dhcpv6_val = tmpdata & 0xff;

        cmd_data_check_element("dhcpv6 mask", NULL,
                               "usage: the format is 0x0-0x1 or 0-1 \n",
                               cmd_data_check_integer, (cmd, &tmpdata, 0xff,
                                       0x0));
        entry->dhcpv6_mask = tmpdata & 0xff;

        FAL_FIELD_FLG_SET(entry->field_flg, FAL_ACL_FIELD_DHCPV6);
    }

    /* get mobility header field configuration */
    cmd_data_check_element("mobility header field", "no", "usage: <yes/no/y/n>\n",
                           cmd_data_check_confirm, (cmd, A_FALSE, &tmpdata,
                                   sizeof (tmpdata)));

    if (tmpdata)
    {
            entry->is_mobility_header_mask = 1;
	    cmd_data_check_element("Is mobility header", "no", "usage: <yes/no/y/n>\n",
	                           cmd_data_check_confirm, (cmd, A_FALSE, &entry->is_mobility_header_val,
	                                   sizeof (a_bool_t)));
            FAL_FIELD_FLG_SET(entry->field_flg, FAL_ACL_FIELD_MOBILITY_HEADER);
    }

    /* get fragment header field configuration */
    cmd_data_check_element("fragment header field", "no", "usage: <yes/no/y/n>\n",
                           cmd_data_check_confirm, (cmd, A_FALSE, &tmpdata,
                                   sizeof (tmpdata)));

    if (tmpdata)
    {
            entry->is_fragment_header_mask = 1;
	    cmd_data_check_element("Is fragment header", "no", "usage: <yes/no/y/n>\n",
	                           cmd_data_check_confirm, (cmd, A_FALSE, &entry->is_fragment_header_val,
	                                   sizeof (a_bool_t)));
            FAL_FIELD_FLG_SET(entry->field_flg, FAL_ACL_FIELD_FRAGMENT_HEADER);
    }

    /* get other header field configuration */
    cmd_data_check_element("other header field", "no", "usage: <yes/no/y/n>\n",
                           cmd_data_check_confirm, (cmd, A_FALSE, &tmpdata,
                                   sizeof (tmpdata)));

    if (tmpdata)
    {
            entry->is_other_header_mask = 1;
	    cmd_data_check_element("Is other header", "no", "usage: <yes/no/y/n>\n",
	                           cmd_data_check_confirm, (cmd, A_FALSE, &entry->is_other_header_val,
	                                   sizeof (a_bool_t)));
            FAL_FIELD_FLG_SET(entry->field_flg, FAL_ACL_FIELD_OTHER_EXT_HEADER);
    }

    return SW_OK;
}

sw_error_t
cmd_data_check_ip_field(fal_acl_rule_t * entry)
{
    char *cmd;
    a_uint32_t tmpdata = 0;

    /* get ip protocol field configuration */
    cmd_data_check_element("ip protocol field", "no", "usage: <yes/no/y/n>\n",
                           cmd_data_check_confirm, (cmd, A_FALSE, &tmpdata,
                                   sizeof (tmpdata)));

    if (tmpdata)
    {
        cmd_data_check_element("ip protocol", NULL,
                               "usage: the format is 0x0-0xff or 0-255 \n",
                               cmd_data_check_integer, (cmd, &tmpdata, 0xff,
                                       0x0));
        entry->ip_proto_val = tmpdata & 0xff;

        cmd_data_check_element("ip protocol mask", NULL,
                               "usage: the format is 0x0-0xff or 0-255 \n",
                               cmd_data_check_integer, (cmd, &tmpdata, 0xff,
                                       0x0));
        entry->ip_proto_mask = tmpdata & 0xff;

        FAL_FIELD_FLG_SET(entry->field_flg, FAL_ACL_FIELD_IP_PROTO);
    }

    /* get ip dscp field configuration */
    cmd_data_check_element("ip dscp field", "no", "usage: <yes/no/y/n>\n",
                           cmd_data_check_confirm, (cmd, A_FALSE, &tmpdata,
                                   sizeof (tmpdata)));
    if (tmpdata)
    {
        cmd_data_check_element("ip dscp", NULL,
                               "usage: the format is 0x0-0xff or 0-255 \n",
                               cmd_data_check_integer, (cmd, &tmpdata, 0xff,
                                       0x0));
        entry->ip_dscp_val = tmpdata & 0xff;

        cmd_data_check_element("ip dscp mask", NULL,
                               "usage: the format is 0x0-0xff or 0-255 \n",
                               cmd_data_check_integer, (cmd, &tmpdata, 0xff,
                                       0x0));
        entry->ip_dscp_mask = tmpdata & 0xff;

        FAL_FIELD_FLG_SET(entry->field_flg, FAL_ACL_FIELD_IP_DSCP);
    }

    /* get ip l4 destination port field configuration */
    cmd_data_check_element("ip l4 dst port field", "no",
                           "usage: <yes/no/y/n>\n", cmd_data_check_confirm,
                           (cmd, A_FALSE, &tmpdata, sizeof (tmpdata)));

    if (tmpdata)
    {
        cmd_data_check_element("ip l4 dst port opration", "mask",
                               "usage: <mask/range/le/ge/ne> \n",
                               cmd_data_check_fieldop, (cmd, FAL_ACL_FIELD_MASK,
                                       &(entry->dest_l4port_op)));

        if (FAL_ACL_FIELD_MASK == entry->dest_l4port_op)
        {
            cmd_data_check_element("ip l4 dst port", NULL,
                                   "usage: the format is 0x0-0xffff or 0-65535 \n",
                                   cmd_data_check_integer, (cmd, &tmpdata,
                                           0xffff, 0x0));
            entry->dest_l4port_val = tmpdata & 0xffff;

            cmd_data_check_element("ip l4 dst port mask", NULL,
                                   "usage: the format is 0x0-0xffff or 0-65535 \n",
                                   cmd_data_check_integer, (cmd, &tmpdata,
                                           0xffff, 0x0));
            entry->dest_l4port_mask = tmpdata & 0xffff;
        }
        else if (FAL_ACL_FIELD_RANGE == entry->dest_l4port_op)
        {
            cmd_data_check_element("ip l4 dst port low", NULL,
                                   "usage: the format is 0x0-0xffff or 0-65535 \n",
                                   cmd_data_check_integer, (cmd, &tmpdata,
                                           0xffff, 0x0));
            entry->dest_l4port_val = tmpdata & 0xffff;

            cmd_data_check_element("ip l4 dst port high", NULL,
                                   "usage: the format is 0x0-0xffff or 0-65535 \n",
                                   cmd_data_check_integer, (cmd, &tmpdata,
                                           0xffff, 0x0));
            entry->dest_l4port_mask = tmpdata & 0xffff;
        }
        else
        {
            cmd_data_check_element("ip l4 dst port", NULL,
                                   "usage: the format is 0x0-0xffff or 0-65535 \n",
                                   cmd_data_check_integer, (cmd, &tmpdata,
                                           0xffff, 0x0));
            entry->dest_l4port_val = tmpdata & 0xffff;
        }

        FAL_FIELD_FLG_SET(entry->field_flg, FAL_ACL_FIELD_L4_DPORT);
    }

    /* get ip l4 source port field configuration */
    cmd_data_check_element("ip l4 src port field", "no",
                           "usage: <yes/no/y/n>\n", cmd_data_check_confirm,
                           (cmd, A_FALSE, &tmpdata, sizeof (tmpdata)));

    if (tmpdata)
    {
        cmd_data_check_element("ip l4 src port opration", "mask",
                               "usage: <mask/range/le/ge/ne> \n",
                               cmd_data_check_fieldop, (cmd, FAL_ACL_FIELD_MASK,
                                       &(entry->src_l4port_op)));

        if (FAL_ACL_FIELD_MASK == entry->src_l4port_op)
        {
            cmd_data_check_element("ip l4 src port", NULL,
                                   "usage: the format is 0x0-0xffff or 0-65535 \n",
                                   cmd_data_check_integer, (cmd, &tmpdata,
                                           0xffff, 0x0));
            entry->src_l4port_val = tmpdata & 0xffff;

            cmd_data_check_element("ip l4 src port mask", NULL,
                                   "usage: the format is 0x0-0xffff or 0-65535 \n",
                                   cmd_data_check_integer, (cmd, &tmpdata,
                                           0xffff, 0x0));
            entry->src_l4port_mask = tmpdata & 0xffff;
        }
        else if (FAL_ACL_FIELD_RANGE == entry->src_l4port_op)
        {
            cmd_data_check_element("ip l4 src port low", NULL,
                                   "usage: the format is 0x0-0xffff or 0-65535 \n",
                                   cmd_data_check_integer, (cmd, &tmpdata,
                                           0xffff, 0x0));
            entry->src_l4port_val = tmpdata & 0xffff;

            cmd_data_check_element("ip l4 src port high", NULL,
                                   "usage: the format is 0x0-0xffff or 0-65535 \n",
                                   cmd_data_check_integer, (cmd, &tmpdata,
                                           0xffff, 0x0));
            entry->src_l4port_mask = tmpdata & 0xffff;
        }
        else
        {
            cmd_data_check_element("ip l4 src port", NULL,
                                   "usage: the format is 0x0-0xffff or 0-65535 \n",
                                   cmd_data_check_integer, (cmd, &tmpdata,
                                           0xffff, 0x0));
            entry->src_l4port_val = tmpdata & 0xffff;
        }

        FAL_FIELD_FLG_SET(entry->field_flg, FAL_ACL_FIELD_L4_SPORT);
    }

    /* get tcp flags field configuration */
    cmd_data_check_element("tcp flags field", "no", "usage: <yes/no/y/n>\n",
                           cmd_data_check_confirm, (cmd, A_FALSE, &tmpdata,
                                   sizeof (tmpdata)));

    if (tmpdata)
    {
        cmd_data_check_element("tcp flags", NULL,
                               "usage: the format is 0x0-0x3f or 0-63 \n",
                               cmd_data_check_integer, (cmd, &tmpdata, 0x3f,
                                       0x0));
        entry->tcp_flag_val = tmpdata & 0x3f;

        cmd_data_check_element("tcp flags mask", NULL,
                               "usage: the format is 0x0-0x3f or 0-63 \n",
                               cmd_data_check_integer, (cmd, &tmpdata, 0x3f,
                                       0x0));
        entry->tcp_flag_mask = tmpdata & 0x3f;

        FAL_FIELD_FLG_SET(entry->field_flg, FAL_ACL_FIELD_TCP_FLAG);
    }


    /* get icmp type/code field configuration */
    cmd_data_check_element("icmp type code field", "no", "usage: <yes/no/y/n>\n",
                           cmd_data_check_confirm, (cmd, A_FALSE, &tmpdata,
                                   sizeof (tmpdata)));

    if (tmpdata)
    {
        cmd_data_check_element("icmp type code operation", "mask",
                               "usage: <mask/range/le/ge/ne> \n",
                               cmd_data_check_fieldop, (cmd, FAL_ACL_FIELD_MASK,
                                       &(entry->icmp_type_code_op)));

        if (FAL_ACL_FIELD_MASK == entry->icmp_type_code_op)
        {
	    /* get icmp type field configuration */
	    cmd_data_check_element("icmp type field", "no", "usage: <yes/no/y/n>\n",
	                           cmd_data_check_confirm, (cmd, A_FALSE, &tmpdata,
	                                   sizeof (tmpdata)));

	    if (tmpdata)
	    {
	        cmd_data_check_element("icmp type", NULL,
	                               "usage: the format is 0x0-0xff or 0-255 \n",
	                               cmd_data_check_integer, (cmd, &tmpdata, 0xff,
	                                       0x0));
	        entry->icmp_type_val = tmpdata & 0xff;

	        cmd_data_check_element("icmp type mask", NULL,
	                               "usage: the format is 0x0-0xff or 0-255 \n",
	                               cmd_data_check_integer, (cmd, &tmpdata, 0xff,
	                                       0x0));
	        entry->icmp_type_mask = tmpdata & 0xff;

	        FAL_FIELD_FLG_SET(entry->field_flg, FAL_ACL_FIELD_ICMP_TYPE);
	    }

	    /* get icmp code field configuration */
	    cmd_data_check_element("icmp code field", "no", "usage: <yes/no/y/n>\n",
	                           cmd_data_check_confirm, (cmd, A_FALSE, &tmpdata,
	                                   sizeof (tmpdata)));

	    if (tmpdata)
	    {
	        cmd_data_check_element("icmp code", NULL,
	                               "usage: the format is 0x0-0xff or 0-255 \n",
	                               cmd_data_check_integer, (cmd, &tmpdata, 0xff,
	                                       0x0));
	        entry->icmp_code_val = tmpdata & 0xff;

	        cmd_data_check_element("icmp code mask", NULL,
	                               "usage: the format is 0x0-0xff or 0-255 \n",
	                               cmd_data_check_integer, (cmd, &tmpdata, 0xff,
	                                       0x0));
	        entry->icmp_code_mask = tmpdata & 0xff;

	        FAL_FIELD_FLG_SET(entry->field_flg, FAL_ACL_FIELD_ICMP_CODE);
	    }
        }
        else if (FAL_ACL_FIELD_RANGE == entry->icmp_type_code_op)
        {
            cmd_data_check_element("icmp type code low", NULL,
                                   "usage: the format is 0x0-0xffff or 0-65535 \n",
                                   cmd_data_check_integer, (cmd, &tmpdata,
                                           0xffff, 0x0));
            entry->icmp_type_val= (tmpdata>>8) & 0xff;
            entry->icmp_code_val= tmpdata & 0xff;

            cmd_data_check_element("icmp type code high", NULL,
                                   "usage: the format is 0x0-0xffff or 0-65535 \n",
                                   cmd_data_check_integer, (cmd, &tmpdata,
                                           0xffff, 0x0));
            entry->icmp_type_mask = (tmpdata>>8) & 0xff;
            entry->icmp_code_mask= tmpdata & 0xff;
            FAL_FIELD_FLG_SET(entry->field_flg, FAL_ACL_FIELD_ICMP_TYPE);
            FAL_FIELD_FLG_SET(entry->field_flg, FAL_ACL_FIELD_ICMP_CODE);
        }
        else
        {
            cmd_data_check_element("icmp type code", NULL,
                                   "usage: the format is 0x0-0xffff or 0-65535 \n",
                                   cmd_data_check_integer, (cmd, &tmpdata,
                                           0xffff, 0x0));
            entry->icmp_type_val= (tmpdata>>8) & 0xff;
            entry->icmp_code_val= tmpdata & 0xff;
            FAL_FIELD_FLG_SET(entry->field_flg, FAL_ACL_FIELD_ICMP_TYPE);
            FAL_FIELD_FLG_SET(entry->field_flg, FAL_ACL_FIELD_ICMP_CODE);
        }
    }

    /* get fragment field configuration */
    cmd_data_check_element("fragment field", "no", "usage: <yes/no/y/n>\n",
                           cmd_data_check_confirm, (cmd, A_FALSE, &tmpdata,
                                   sizeof (tmpdata)));

    if (tmpdata)
    {
            entry->is_fragement_mask = 1;
	    cmd_data_check_element("Is fragment packet", "no", "usage: <yes/no/y/n>\n",
	                           cmd_data_check_confirm, (cmd, A_FALSE, &entry->is_fragement_val,
	                                   sizeof (a_bool_t)));
            FAL_FIELD_FLG_SET(entry->field_flg, FAL_ACL_FIELD_L3_FRAGMENT);
    }

    /* get first fragment field configuration */
    cmd_data_check_element("first fragment field", "no", "usage: <yes/no/y/n>\n",
                           cmd_data_check_confirm, (cmd, A_FALSE, &tmpdata,
                                   sizeof (tmpdata)));

    if (tmpdata)
    {
            entry->is_first_frag_mask = 1;
	    cmd_data_check_element("Is first fragment packet", "no", "usage: <yes/no/y/n>\n",
	                           cmd_data_check_confirm, (cmd, A_FALSE, &entry->is_first_frag_val,
	                                   sizeof (a_bool_t)));
            FAL_FIELD_FLG_SET(entry->field_flg, FAL_ACL_FIELD_FIRST_FRAGMENT);
    }

    /* get L3 TTL field configuration */
    cmd_data_check_element("l3 ttl field", "no", "usage: <yes/no/y/n>\n",
                           cmd_data_check_confirm, (cmd, A_FALSE, &tmpdata,
                                   sizeof (tmpdata)));

    if (tmpdata)
    {
            entry->l3_ttl_mask = 0x3;
            cmd_data_check_element("l3 ttl", "0",
                               "usage: 0-ttl/hoplimit is 0, 1-ttl/hoplimit is 1, 2-ttl/hoplimit is 255, 3-ttl/hoplimit is other \n",
                               cmd_data_check_integer, (cmd, &tmpdata, 0x3,
                                       0x0));
            entry->l3_ttl = tmpdata & 0x3;

            FAL_FIELD_FLG_SET(entry->field_flg, FAL_ACL_FIELD_L3_TTL);
    }

    /* get l3 length field configuration */
    cmd_data_check_element("l3 length field", "no", "usage: <yes/no/y/n>\n",
                           cmd_data_check_confirm, (cmd, A_FALSE, &tmpdata,
                                   sizeof (tmpdata)));

    if (tmpdata)
    {
        cmd_data_check_element("l3 length operation", "mask",
                               "usage: <mask/range/le/ge/ne> \n",
                               cmd_data_check_fieldop, (cmd, FAL_ACL_FIELD_MASK,
                                       &(entry->l3_length_op)));

        if (FAL_ACL_FIELD_MASK == entry->l3_length_op)
        {
            cmd_data_check_element("l3 length", NULL,
                                   "usage: the format is 0x0-0xffff or 0-65535 \n",
                                   cmd_data_check_integer, (cmd, &tmpdata,
                                           0xffff, 0x0));
            entry->l3_length = tmpdata & 0xffff;

            cmd_data_check_element("l3 length mask", NULL,
                                   "usage: the format is 0x0-0xffff or 0-65535 \n",
                                   cmd_data_check_integer, (cmd, &tmpdata,
                                           0xffff, 0x0));
            entry->l3_length_mask = tmpdata & 0xffff;
        }
        else if (FAL_ACL_FIELD_RANGE == entry->l3_length_op)
        {
            cmd_data_check_element("l3 length low", NULL,
                                   "usage: the format is 0x0-0xffff or 0-65535 \n",
                                   cmd_data_check_integer, (cmd, &tmpdata,
                                           0xffff, 0x0));
            entry->l3_length = tmpdata & 0xffff;

            cmd_data_check_element("l3 length high", NULL,
                                   "usage: the format is 0x0-0xffff or 0-65535 \n",
                                   cmd_data_check_integer, (cmd, &tmpdata,
                                           0xffff, 0x0));
            entry->l3_length_mask = tmpdata & 0xffff;
        }
        else
        {
            cmd_data_check_element("l3 length", NULL,
                                   "usage: the format is 0x0-0xffff or 0-65535 \n",
                                   cmd_data_check_integer, (cmd, &tmpdata,
                                           0xffff, 0x0));
            entry->l3_length = tmpdata & 0xffff;
        }

        FAL_FIELD_FLG_SET(entry->field_flg, FAL_ACL_FIELD_L3_LENGTH);
    }

    /* get L3 packet type field configuration */
    cmd_data_check_element("l3 packet type field", "no", "usage: <yes/no/y/n>\n",
                           cmd_data_check_confirm, (cmd, A_FALSE, &tmpdata,
                                   sizeof (tmpdata)));

    if (tmpdata)
    {
            entry->l3_pkt_type_mask = 0x7;
            cmd_data_check_element("l3 packet type", "tcp",
                               "usage: TCP, UDP, UDP-Lite, ARP, ICMP \n",
                               cmd_data_check_ip_packet_type, (cmd, &tmpdata,sizeof(tmpdata)));
            entry->l3_pkt_type = tmpdata & 0x7;

            FAL_FIELD_FLG_SET(entry->field_flg, FAL_ACL_FIELD_IP_PKT_TYPE);
    }

    /* get ah header field configuration */
    cmd_data_check_element("ah header field", "no", "usage: <yes/no/y/n>\n",
                           cmd_data_check_confirm, (cmd, A_FALSE, &tmpdata,
                                   sizeof (tmpdata)));

    if (tmpdata)
    {
            entry->is_ah_header_mask = 1;
	    cmd_data_check_element("Is AH header", "no", "usage: <yes/no/y/n>\n",
	                           cmd_data_check_confirm, (cmd, A_FALSE, &entry->is_ah_header_val,
	                                   sizeof (a_bool_t)));
            FAL_FIELD_FLG_SET(entry->field_flg, FAL_ACL_FIELD_AH_HEADER);
    }

    /* get esp header field configuration */
    cmd_data_check_element("esp header field", "no", "usage: <yes/no/y/n>\n",
                           cmd_data_check_confirm, (cmd, A_FALSE, &tmpdata,
                                   sizeof (tmpdata)));

    if (tmpdata)
    {
            entry->is_esp_header_mask = 1;
	    cmd_data_check_element("Is ESP header", "no", "usage: <yes/no/y/n>\n",
	                           cmd_data_check_confirm, (cmd, A_FALSE, &entry->is_esp_header_val,
	                                   sizeof (a_bool_t)));
            FAL_FIELD_FLG_SET(entry->field_flg, FAL_ACL_FIELD_ESP_HEADER);
    }

    return SW_OK;
}


sw_error_t
cmd_data_check_udf_type(char *cmdstr, fal_acl_udf_type_t * arg_val, a_uint32_t size)
{
    if (NULL == cmdstr)
    {
        return SW_BAD_VALUE;
    }

    if (!strcasecmp(cmdstr, "l2"))
    {
        *arg_val = FAL_ACL_UDF_TYPE_L2;
    }
    else if (!strcasecmp(cmdstr, "l2snap"))
    {
        *arg_val = FAL_ACL_UDF_TYPE_L2_SNAP;
    }
    else if (!strcasecmp(cmdstr, "l3"))
    {
        *arg_val = FAL_ACL_UDF_TYPE_L3;
    }
    else if (!strcasecmp(cmdstr, "l3plus"))
    {
        *arg_val = FAL_ACL_UDF_TYPE_L3_PLUS;
    }
    else if (!strcasecmp(cmdstr, "l4"))
    {
        *arg_val = FAL_ACL_UDF_TYPE_L4;
    }
    else
    {
        return SW_BAD_VALUE;
    }

    return SW_OK;
}

void
cmd_data_print_udf_type(a_char_t * param_name, a_uint32_t * buf,
                        a_uint32_t size)
{
    fal_acl_udf_type_t *val;

    val = (fal_acl_udf_type_t *) buf;
    dprintf("[%s]:", param_name);

    if (FAL_ACL_UDF_TYPE_L2 == *val)
    {
        dprintf("l2");
    }
    else if (FAL_ACL_UDF_TYPE_L2_SNAP == *val)
    {
        dprintf("l2snap");
    }
    else if (FAL_ACL_UDF_TYPE_L3 == *val)
    {
        dprintf("l3");
    }
    else if (FAL_ACL_UDF_TYPE_L3_PLUS == *val)
    {
        dprintf("l3plus");
    }
    else if (FAL_ACL_UDF_TYPE_L4 == *val)
    {
        dprintf("l4");
    }
    else
    {
        dprintf("unknow");
    }
}

sw_error_t
cmd_data_check_udf_pkt_type(a_char_t *cmdstr, fal_acl_udf_pkt_type_t * arg_val, a_uint32_t size)
{
    if (NULL == cmdstr)
    {
        return SW_BAD_VALUE;
    }

    if (!strcasecmp(cmdstr, "non-ip"))
    {
        *arg_val = FAL_ACL_UDF_NON_IP;
    }
    else if (!strcasecmp(cmdstr, "ipv4"))
    {
        *arg_val = FAL_ACL_UDF_IP4;
    }
    else if (!strcasecmp(cmdstr, "ipv6"))
    {
        *arg_val = FAL_ACL_UDF_IP6;
    }
    else
    {
        return SW_BAD_VALUE;
    }

    return SW_OK;
}

void
cmd_data_print_udf_pkt_type(a_char_t * param_name, a_uint32_t * buf,
                        a_uint32_t size)
{
    fal_acl_udf_pkt_type_t *val;

    val = (fal_acl_udf_pkt_type_t *) buf;
    dprintf("%s", param_name);

    if (FAL_ACL_UDF_NON_IP== *val)
    {
        dprintf("non-ip");
    }
    else if (FAL_ACL_UDF_IP4 == *val)
    {
        dprintf("ipv4");
    }
    else if (FAL_ACL_UDF_IP6== *val)
    {
        dprintf("ipv6");
    }
    else
    {
        dprintf("unknow");
    }
}

sw_error_t
cmd_data_check_udf_element(char *cmdstr, a_uint8_t * val, a_uint32_t * len)
{
    char *tmp = NULL, *str_save;
    a_uint32_t i = 0, j;
    a_uint32_t data;

    memset(val, 0, 16);
    if (NULL == cmdstr)
    {
        return SW_BAD_VALUE;
    }

    if (0 == cmdstr[0])
    {
        return SW_BAD_VALUE;
    }

    tmp = (void *) strtok_r(cmdstr, "-", &str_save);
    while (tmp)
    {
        if (16 <= i)
        {
            return SW_BAD_VALUE;
        }

        if ((2 < strlen(tmp)) || (0 == strlen(tmp)))
        {
            return SW_BAD_VALUE;
        }

        for (j = 0; j < strlen(tmp); j++)
        {
            if (A_FALSE == is_hex(tmp[j]))
            {
                return SW_BAD_VALUE;
            }
        }

        sscanf(tmp, "%x", &data);

        val[i++] = data & 0xff;
        tmp = (void *) strtok_r(NULL, "-", &str_save);
    }

    if (0 == i)
    {
        return SW_BAD_VALUE;
    }

    *len = i;
    return SW_OK;
}

void
cmd_data_print_udf_element(char * param_name, a_uint32_t * buf,
                           a_uint32_t size)
{
    a_uint8_t *val, i;

    if (size)
    {
        val = (a_uint8_t *) buf;
        dprintf("%s", param_name);

        for (i = 0; i < (size - 1); i++)
        {
            dprintf("%02x-", *val);
            val++;
        }
        dprintf("%02x", *val);
    }
}


sw_error_t
cmd_data_check_udf_field(fal_acl_rule_t * entry)
{
    char *cmd;
    a_uint32_t tmpdata = 0, vlen = 0, mlen = 0;

    /* get udf field configuration */
    cmd_data_check_element("user define field", "no",
                           "usage: <yes/no/y/n>\n", cmd_data_check_confirm,
                           (cmd, A_FALSE, &tmpdata, sizeof (tmpdata)));

    if (tmpdata)
    {
        cmd_data_check_element("udf type", NULL,
                               "usage: <l2/l3>\n",
                               cmd_data_check_udf_type, (cmd,
                                       &(entry->udf_type), 4));

        cmd_data_check_element("udf offset", NULL,
                               "usage: <0-126, must be even>\n",
                               cmd_data_check_uint32, (cmd, &tmpdata, vlen));
        entry->udf_offset = tmpdata;

        cmd_data_check_element("udf value", NULL,
                               "usage: the format is xx-xx-xx-xx-xx\n",
                               cmd_data_check_udf_element, (cmd,
                                       &(entry->udf_val[0]), &vlen));

        cmd_data_check_element("udf mask", NULL,
                               "usage: the format is xx-xx-xx-xx-xx\n",
                               cmd_data_check_udf_element, (cmd,
                                       &(entry->udf_mask[0]), &mlen));

        if (vlen != mlen)
        {
            return SW_BAD_VALUE;
        }
        entry->udf_len = vlen;

        FAL_FIELD_FLG_SET(entry->field_flg, FAL_ACL_FIELD_UDF);
    }

    /* get udf0 field configuration */
    cmd_data_check_element("udf0", "no",
                           "usage: <yes/no/y/n>\n", cmd_data_check_confirm,
                           (cmd, A_FALSE, &tmpdata, sizeof (tmpdata)));
    if (tmpdata)
    {
        cmd_data_check_element("udf0 opration", "mask",
                               "usage: <mask/range/le/ge/ne> \n",
                               cmd_data_check_fieldop, (cmd, FAL_ACL_FIELD_MASK,
                                       &(entry->udf0_op)));

        if (FAL_ACL_FIELD_MASK == entry->udf0_op)
        {
            cmd_data_check_element("udf0", NULL,
                                   "usage: the format is 0x0-0xffff\n",
                                   cmd_data_check_integer, (cmd, &tmpdata,
                                           0xffff, 0x0));
            entry->udf0_val = tmpdata & 0xffff;

            cmd_data_check_element("udf0 mask", NULL,
                                   "usage: the format is 0x0-0xffff\n",
                                   cmd_data_check_integer, (cmd, &tmpdata,
                                           0xffff, 0x0));
            entry->udf0_mask = tmpdata & 0xffff;
        }
        else if (FAL_ACL_FIELD_RANGE == entry->udf0_op)
        {
            cmd_data_check_element("udf0 low", NULL,
                                   "usage: the format is 0x0-0xffff or\n",
                                   cmd_data_check_integer, (cmd, &tmpdata,
                                           0xffff, 0x0));
            entry->udf0_val= tmpdata & 0xffff;

            cmd_data_check_element("udf0 high", NULL,
                                   "usage: the format is 0x0-0xffff\n",
                                   cmd_data_check_integer, (cmd, &tmpdata,
                                           0xffff, 0x0));
            entry->udf0_mask = tmpdata & 0xffff;
        }
        else
        {
            cmd_data_check_element("udf0", NULL,
                                   "usage: the format is 0x0-0xffff\n",
                                   cmd_data_check_integer, (cmd, &tmpdata,
                                           0xffff, 0x0));
            entry->udf0_val = tmpdata & 0xffff;
        }

        FAL_FIELD_FLG_SET(entry->field_flg, FAL_ACL_FIELD_UDF0);
    }

        /* get udf1 field configuration */
    cmd_data_check_element("udf1", "no",
                           "usage: <yes/no/y/n>\n", cmd_data_check_confirm,
                           (cmd, A_FALSE, &tmpdata, sizeof (tmpdata)));
    if (tmpdata)
    {
        cmd_data_check_element("udf1 opration", "mask",
                               "usage: <mask/range/le/ge/ne> \n",
                               cmd_data_check_fieldop, (cmd, FAL_ACL_FIELD_MASK,
                                       &(entry->udf1_op)));

        if (FAL_ACL_FIELD_MASK == entry->udf1_op)
        {
            cmd_data_check_element("udf1", NULL,
                                   "usage: the format is 0x0-0xffff\n",
                                   cmd_data_check_integer, (cmd, &tmpdata,
                                           0xffff, 0x0));
            entry->udf1_val = tmpdata & 0xffff;

            cmd_data_check_element("udf1 mask", NULL,
                                   "usage: the format is 0x0-0xffff\n",
                                   cmd_data_check_integer, (cmd, &tmpdata,
                                           0xffff, 0x0));
            entry->udf1_mask = tmpdata & 0xffff;
        }
        else if (FAL_ACL_FIELD_RANGE == entry->udf1_op)
        {
            cmd_data_check_element("udf1 low", NULL,
                                   "usage: the format is 0x0-0xffff or\n",
                                   cmd_data_check_integer, (cmd, &tmpdata,
                                           0xffff, 0x0));
            entry->udf1_val= tmpdata & 0xffff;

            cmd_data_check_element("udf1 high", NULL,
                                   "usage: the format is 0x0-0xffff\n",
                                   cmd_data_check_integer, (cmd, &tmpdata,
                                           0xffff, 0x0));
            entry->udf1_mask = tmpdata & 0xffff;
        }
        else
        {
            cmd_data_check_element("udf1", NULL,
                                   "usage: the format is 0x0-0xffff\n",
                                   cmd_data_check_integer, (cmd, &tmpdata,
                                           0xffff, 0x0));
            entry->udf1_val = tmpdata & 0xffff;
        }

        FAL_FIELD_FLG_SET(entry->field_flg, FAL_ACL_FIELD_UDF1);
    }

    /* get udf2 field configuration */
    cmd_data_check_element("udf2", "no",
                           "usage: <yes/no/y/n>\n", cmd_data_check_confirm,
                           (cmd, A_FALSE, &tmpdata, sizeof (tmpdata)));
    if (tmpdata)
    {

	cmd_data_check_element("udf2", NULL,
	                   "usage: the format is 0x0-0xffff\n",
	                   cmd_data_check_integer, (cmd, &tmpdata,
	                           0xffff, 0x0));
	entry->udf2_val = tmpdata & 0xffff;

	cmd_data_check_element("udf2 mask", NULL,
	                   "usage: the format is 0x0-0xffff\n",
	                   cmd_data_check_integer, (cmd, &tmpdata,
	                           0xffff, 0x0));
	entry->udf2_mask = tmpdata & 0xffff;

        FAL_FIELD_FLG_SET(entry->field_flg, FAL_ACL_FIELD_UDF2);
    }
    /* get udf3 field configuration */
    cmd_data_check_element("udf3", "no",
                           "usage: <yes/no/y/n>\n", cmd_data_check_confirm,
                           (cmd, A_FALSE, &tmpdata, sizeof (tmpdata)));
    if (tmpdata)
    {

	cmd_data_check_element("udf3", NULL,
	                   "usage: the format is 0x0-0xffff\n",
	                   cmd_data_check_integer, (cmd, &tmpdata,
	                           0xffff, 0x0));
	entry->udf3_val = tmpdata & 0xffff;

	cmd_data_check_element("udf3 mask", NULL,
	                   "usage: the format is 0x0-0xffff\n",
	                   cmd_data_check_integer, (cmd, &tmpdata,
	                           0xffff, 0x0));
	entry->udf3_mask = tmpdata & 0xffff;

        FAL_FIELD_FLG_SET(entry->field_flg, FAL_ACL_FIELD_UDF3);
    }

    if(entry->rule_type == FAL_ACL_RULE_UDF)
    {
	/* get IP/NON-IP field configuration */
	cmd_data_check_element("IP/NON-IP field", "no", "usage: <yes/no/y/n>\n",
                           cmd_data_check_confirm, (cmd, A_FALSE, &tmpdata,
                                   sizeof (a_bool_t)));
	if(tmpdata)
	{
		entry->is_ip_mask = 1;
		cmd_data_check_element("Is IP packet", "no", "usage: <yes/no/y/n>\n",
	                           cmd_data_check_confirm, (cmd, A_FALSE, &entry->is_ip_val,
	                                   sizeof (a_bool_t)));
		FAL_FIELD_FLG_SET(entry->field_flg, FAL_ACL_FIELD_IP);
	}
	/* get IPv4/IPv6 field configuration */
	cmd_data_check_element("IPv4/IPv6 field", "no", "usage: <yes/no/y/n>\n",
                           cmd_data_check_confirm, (cmd, A_FALSE, &tmpdata,
                                   sizeof (a_bool_t)));
	if(tmpdata)
	{
		entry->is_ipv6_mask = 1;
		cmd_data_check_element("Is IPv6 packet", "no", "usage: <yes/no/y/n>\n",
	                           cmd_data_check_confirm, (cmd, A_FALSE, &entry->is_ipv6_val,
	                                   sizeof (a_bool_t)));
		FAL_FIELD_FLG_SET(entry->field_flg, FAL_ACL_FIELD_IPV6);
	}
    }

    return SW_OK;
}

sw_error_t
cmd_data_check_acl_action(fal_acl_rule_t * entry)
{
    char *cmd, *cmd_find;
    a_uint32_t tmpdata = 0;
    sw_error_t rv;

    /* get permit action configuration */
    cmd_data_check_element("permit", "yes", "usage: <yes/no/y/n>\n",
                           cmd_data_check_confirm, (cmd, A_TRUE, &tmpdata,
                                   sizeof (a_bool_t)));

    if (tmpdata)
    {
        FAL_ACTION_FLG_SET(entry->action_flg, FAL_ACL_ACTION_PERMIT);
    }

    /* get deny action configuration */
    cmd_data_check_element("deny", "no", "usage: <yes/no/y/n>\n",
                           cmd_data_check_confirm, (cmd, A_FALSE, &tmpdata,
                                   sizeof (a_bool_t)));

    if (tmpdata)
    {
        FAL_ACTION_FLG_SET(entry->action_flg, FAL_ACL_ACTION_DENY);
    }

    /* get redirect to cpu action configuration */
    cmd_data_check_element("rdt to cpu", "no", "usage: <yes/no/y/n>\n",
                           cmd_data_check_confirm, (cmd, A_FALSE, &tmpdata,
                                   sizeof (a_bool_t)));

    if (tmpdata)
    {
        FAL_ACTION_FLG_SET(entry->action_flg, FAL_ACL_ACTION_RDTCPU);
    }

    /* get port redirection action configuration */
    cmd_data_check_element("rdt to port", "no", "usage: <yes/no/y/n>\n",
                           cmd_data_check_confirm, (cmd, A_FALSE, &tmpdata,
                                   sizeof (a_bool_t)));

    if (tmpdata)
    {
        do
        {
            cmd = get_sub_cmd("dest port", "null");
            SW_RTN_ON_NULL_PARAM(cmd);

            if (!strncasecmp(cmd, "quit", 4))
            {
                 return SW_BAD_VALUE;
            }
            else if (!strncasecmp(cmd, "help", 4))
            {
                dprintf("usage: physical port such as 1,3\n");
                dprintf("       nexthop with highest 8bits as 0x1\n");
                dprintf("       vp and trunk with highest 8bits as 0x2\n");
                return SW_BAD_VALUE;
            }
            else
            {
                cmd_find = strstr(cmd, ",");
                if (cmd_find == NULL)
                {
                    rv = cmd_data_check_portid(cmd, &entry->ports, sizeof (fal_pbmp_t));
                    if(entry->ports <= SW_MAX_NR_PORT)
                    {
                        entry->ports = 1<<(entry->ports);
                    }
                }
                else
                {
                    rv = cmd_data_check_portmap(cmd, &entry->ports, sizeof (fal_pbmp_t));
                }
                if(rv != SW_OK)
                {
                    dprintf("usage: physical port such as 1,3\n");
                    dprintf("       nexthop with highest 8bits as 0x1\n");
                    dprintf("       vp and trunk with highest 8bits as 0x2\n");
                }
                FAL_ACTION_FLG_SET(entry->action_flg, FAL_ACL_ACTION_REDPT);
            }
        }while (talk_mode && (SW_OK != rv));
    }

    /* get copy to cpu action configuration */
    cmd_data_check_element("copy to cpu", "no", "usage: <yes/no/y/n>\n",
                           cmd_data_check_confirm, (cmd, A_FALSE, &tmpdata,
                                   sizeof (a_bool_t)));

    if (tmpdata)
    {
        FAL_ACTION_FLG_SET(entry->action_flg, FAL_ACL_ACTION_CPYCPU);
    }

    /* get mirror action configuration */
    cmd_data_check_element("mirror", "no", "usage: <yes/no/y/n>\n",
                           cmd_data_check_confirm, (cmd, A_FALSE, &tmpdata,
                                   sizeof (a_bool_t)));

    if (tmpdata)
    {
        FAL_ACTION_FLG_SET(entry->action_flg, FAL_ACL_ACTION_MIRROR);
    }

    /* get remark dscp action configuration */
    cmd_data_check_element("remark dscp", "no", "usage: <yes/no/y/n>\n",
                           cmd_data_check_confirm, (cmd, A_FALSE, &tmpdata,
                                   sizeof (a_bool_t)));

    if (A_TRUE == tmpdata)
    {
        cmd_data_check_element("dscp", NULL,
                               "usage: the format is 0x0-0xff or 0-255\n",
                               cmd_data_check_integer, (cmd, &tmpdata, 0xff,
                                       0x0));
        entry->dscp = tmpdata & 0xff;

        cmd_data_check_element("dscp mask", NULL,
                               "usage: the format is 0x0-0xff or 0-255\n",
                               cmd_data_check_integer, (cmd, &tmpdata, 0xff,
                                       0x0));
        entry->dscp_mask = tmpdata & 0xff;

        FAL_ACTION_FLG_SET(entry->action_flg, FAL_ACL_ACTION_REMARK_DSCP);
    }

    /* get remark up action configuration */
    cmd_data_check_element("remark up", "no", "usage: <yes/no/y/n>\n",
                           cmd_data_check_confirm, (cmd, A_FALSE, &tmpdata,
                                   sizeof (a_bool_t)));

    if (A_TRUE == tmpdata)
    {
        cmd_data_check_element("up", NULL,
                               "usage: the format is 0x0-0x7 or 0-7 \n",
                               cmd_data_check_integer, (cmd, &tmpdata, 0x7,
                                       0x0));
        entry->up = tmpdata & 0x7;

        FAL_ACTION_FLG_SET(entry->action_flg, FAL_ACL_ACTION_REMARK_UP);
    }

    /* get remark queue action configuration */
    cmd_data_check_element("remark queue", "no", "usage: <yes/no/y/n>\n",
                           cmd_data_check_confirm, (cmd, A_FALSE, &tmpdata,
                                   sizeof (a_bool_t)));

    if (tmpdata)
    {
        cmd_data_check_element("queue", NULL,
                               "usage: the format is 0x0-0xff or 0-255 \n",
                               cmd_data_check_integer, (cmd, &tmpdata, 0xff,
                                       0x0));
        entry->queue = tmpdata & 0xff;

        FAL_ACTION_FLG_SET(entry->action_flg, FAL_ACL_ACTION_REMARK_QUEUE);
    }

    /* get modify vlan action configuration */
    cmd_data_check_element("modify vlan", "no", "usage: <yes/no/y/n>\n",
                           cmd_data_check_confirm, (cmd, A_FALSE, &tmpdata,
                                   sizeof (a_bool_t)));

    if (A_TRUE == tmpdata)
    {
        cmd_data_check_element("vlan", NULL,
                               "usage: the format is 0x0-0xfff or 0-4095 \n",
                               cmd_data_check_integer, (cmd, &tmpdata, 0xfff,
                                       0x0));
        entry->vid = tmpdata & 0xfff;
        FAL_ACTION_FLG_SET(entry->action_flg, FAL_ACL_ACTION_MODIFY_VLAN);

        if (!FAL_ACTION_FLG_TST(entry->action_flg, FAL_ACL_ACTION_REDPT))
        {
            cmd_data_check_element("port member", "null",
                                   "usage: input port number such as 1,3\n",
                                   cmd_data_check_portmap, (cmd, &entry->ports,
                                           sizeof (fal_pbmp_t)));
        }
    }

    /* get nest vlan action configuration */
    cmd_data_check_element("nest vlan", "no", "usage: <yes/no/y/n>\n",
                           cmd_data_check_confirm, (cmd, A_FALSE, &tmpdata,
                                   sizeof (a_bool_t)));

    if (A_TRUE == tmpdata)
    {
        cmd_data_check_element("vlan", NULL,
                               "usage: the format is 0x1-0xfff or 1-4095 \n",
                               cmd_data_check_integer, (cmd, &tmpdata, 0xfff,
                                       0x1));
        entry->vid = tmpdata & 0xfff;

        FAL_ACTION_FLG_SET(entry->action_flg, FAL_ACL_ACTION_NEST_VLAN);
    }

    cmd_data_check_element("stag vid", "0",
                           "usage: the format is 0x0-0xfff or 0-4095 \n",
                           cmd_data_check_integer, (cmd, &tmpdata, 0xfff,
                                   0x0));
    entry->stag_vid = tmpdata & 0xfff;

    cmd_data_check_element("ctag vid", "0",
                           "usage: the format is 0x0-0xfff or 0-4095 \n",
                           cmd_data_check_integer, (cmd, &tmpdata, 0xfff,
                                   0x0));
    entry->ctag_vid = tmpdata & 0xfff;

    /* chang lookup vid action configuration */
    cmd_data_check_element("lookup vid change", "no", "usage: <yes/no/y/n>\n",
                           cmd_data_check_confirm, (cmd, A_FALSE, &tmpdata,
                                   sizeof (a_bool_t)));
    if (A_TRUE == tmpdata)
    {
        FAL_ACTION_FLG_SET(entry->action_flg, FAL_ACL_ACTION_REMARK_LOOKUP_VID);
    }

    /* chang stag vid action configuration */
    cmd_data_check_element("stag vid change", "no", "usage: <yes/no/y/n>\n",
                           cmd_data_check_confirm, (cmd, A_FALSE, &tmpdata,
                                   sizeof (a_bool_t)));
    if (A_TRUE == tmpdata)
    {
        FAL_ACTION_FLG_SET(entry->action_flg, FAL_ACL_ACTION_REMARK_STAG_VID);
        cmd_data_check_element("stag format", NULL,
                               "usage: 0-untaged, 1-pritagged or tagged\n",
                               cmd_data_check_integer, (cmd, &tmpdata, 0x1,
                                       0x0));
        entry->stag_fmt = tmpdata & 0x1;
    }

    /* chang stag pri action configuration */
    cmd_data_check_element("stag pri change", "no", "usage: <yes/no/y/n>\n",
                           cmd_data_check_confirm, (cmd, A_FALSE, &tmpdata,
                                   sizeof (a_bool_t)));
    if (A_TRUE == tmpdata)
    {
        cmd_data_check_element("stag pri", NULL,
                               "usage: the format is 0x1-0x7 or 0-7 \n",
                               cmd_data_check_integer, (cmd, &tmpdata, 0x7,
                                       0x0));
        entry->stag_pri = tmpdata & 0x7;

        FAL_ACTION_FLG_SET(entry->action_flg, FAL_ACL_ACTION_REMARK_STAG_PRI);
    }

    /* chang stag dei action configuration */
    cmd_data_check_element("stag dei change", "no", "usage: <yes/no/y/n>\n",
                           cmd_data_check_confirm, (cmd, A_FALSE, &tmpdata,
                                   sizeof (a_bool_t)));
    if (A_TRUE == tmpdata)
    {
        cmd_data_check_element("stag dei", NULL,
                               "usage: the format is 0x0-0x1 or 0-1 \n",
                               cmd_data_check_integer, (cmd, &tmpdata, 0x1,
                                       0x0));
        entry->stag_dei = tmpdata & 0x1;

        FAL_ACTION_FLG_SET(entry->action_flg, FAL_ACL_ACTION_REMARK_STAG_DEI);
    }

    /* chang ctag vid action configuration */
    cmd_data_check_element("ctag vid change", "no", "usage: <yes/no/y/n>\n",
                           cmd_data_check_confirm, (cmd, A_FALSE, &tmpdata,
                                   sizeof (a_bool_t)));
    if (A_TRUE == tmpdata)
    {
        FAL_ACTION_FLG_SET(entry->action_flg, FAL_ACL_ACTION_REMARK_CTAG_VID);

        cmd_data_check_element("ctag format", NULL,
                               "usage: 0-untaged, 1-pritagged or tagged\n",
                               cmd_data_check_integer, (cmd, &tmpdata, 0x1,
                                       0x0));
        entry->ctag_fmt = tmpdata & 0x1;

    }


    /* chang ctag pri action configuration */
    cmd_data_check_element("ctag pri change", "no", "usage: <yes/no/y/n>\n",
                           cmd_data_check_confirm, (cmd, A_FALSE, &tmpdata,
                                   sizeof (a_bool_t)));
    if (A_TRUE == tmpdata)
    {
        cmd_data_check_element("ctag pri", NULL,
                               "usage: the format is 0x1-0x7 or 0-7 \n",
                               cmd_data_check_integer, (cmd, &tmpdata, 0x7,
                                       0x0));
        entry->ctag_pri = tmpdata & 0x7;

        FAL_ACTION_FLG_SET(entry->action_flg, FAL_ACL_ACTION_REMARK_CTAG_PRI);
    }

    /* chang ctag cfi action configuration */
    cmd_data_check_element("ctag cfi change", "no", "usage: <yes/no/y/n>\n",
                           cmd_data_check_confirm, (cmd, A_FALSE, &tmpdata,
                                   sizeof (a_bool_t)));
    if (A_TRUE == tmpdata)
    {
        cmd_data_check_element("ctag cfi", NULL,
                               "usage: the format is 0x0-0x1 or 0-1 \n",
                               cmd_data_check_integer, (cmd, &tmpdata, 0x1,
                                       0x0));
        entry->ctag_cfi = tmpdata & 0x1;

        FAL_ACTION_FLG_SET(entry->action_flg, FAL_ACL_ACTION_REMARK_CTAG_CFI);
    }

    /* police action configuration */
    cmd_data_check_element("policer en", "no", "usage: <yes/no/y/n>\n",
                           cmd_data_check_confirm, (cmd, A_FALSE, &tmpdata,
                                   sizeof (a_bool_t)));
    if (A_TRUE == tmpdata)
    {
        cmd_data_check_element("policer ptr", NULL,
                               "usage: the format is integer \n",
                               cmd_data_check_integer, (cmd, &tmpdata, 0xffffffff,
                                       0x0));
        entry->policer_ptr = tmpdata;

        FAL_ACTION_FLG_SET(entry->action_flg, FAL_ACL_ACTION_POLICER_EN);
    }

    /* wcmp action configuration */
    cmd_data_check_element("wcmp en", "no", "usage: <yes/no/y/n>\n",
                           cmd_data_check_confirm, (cmd, A_FALSE, &tmpdata,
                                   sizeof (a_bool_t)));
    if (A_TRUE == tmpdata)
    {
        cmd_data_check_element("wcmp ptr", NULL,
                               "usage: the format is integer \n",
                               cmd_data_check_integer, (cmd, &tmpdata, 0xffffffff,
                                       0x0));
        entry->wcmp_ptr = tmpdata;

        FAL_ACTION_FLG_SET(entry->action_flg, FAL_ACL_ACTION_WCMP_EN);
    }

    /* arp action configuration */
    cmd_data_check_element("arp en", "no", "usage: <yes/no/y/n>\n",
                           cmd_data_check_confirm, (cmd, A_FALSE, &tmpdata,
                                   sizeof (a_bool_t)));
    if (A_TRUE == tmpdata)
    {
        cmd_data_check_element("arp ptr", "0",
                               "usage: the format is integer \n",
                               cmd_data_check_integer, (cmd, &tmpdata, 0xffffffff,
                                       0x0));
        entry->arp_ptr = tmpdata;

        FAL_ACTION_FLG_SET(entry->action_flg, FAL_ACL_ACTION_ARP_EN);
    }

    /* policy forward action configuration */
    cmd_data_check_element("policy en", "no", "usage: <yes/no/y/n>\n",
                           cmd_data_check_confirm, (cmd, A_FALSE, &tmpdata,
                                   sizeof (a_bool_t)));
    if (A_TRUE == tmpdata)
    {
        cmd_data_check_element("route", "no", "usage: <yes/no/y/n>\n",
                               cmd_data_check_confirm, (cmd, A_FALSE, &tmpdata,
                                       sizeof (a_bool_t)));
        if (tmpdata)
        {
            entry->policy_fwd = FAL_ACL_POLICY_ROUTE;
        }

        cmd_data_check_element("snat", "no", "usage: <yes/no/y/n>\n",
                               cmd_data_check_confirm, (cmd, A_FALSE, &tmpdata,
                                       sizeof (a_bool_t)));
        if (tmpdata)
        {
            entry->policy_fwd = FAL_ACL_POLICY_SNAT;
        }

        cmd_data_check_element("dnat", "no", "usage: <yes/no/y/n>\n",
                               cmd_data_check_confirm, (cmd, A_FALSE, &tmpdata,
                                       sizeof (a_bool_t)));
        if (tmpdata)
        {
            entry->policy_fwd = FAL_ACL_POLICY_DNAT;
        }

        FAL_ACTION_FLG_SET(entry->action_flg, FAL_ACL_ACTION_POLICY_FORWARD_EN);
    }

    cmd_data_check_element("eg bypass", "no", "usage: <yes/no/y/n>\n",
                           cmd_data_check_confirm, (cmd, A_FALSE, &tmpdata,
                                   sizeof (a_bool_t)));

    if (tmpdata)
    {
        FAL_ACTION_FLG_SET(entry->action_flg, FAL_ACL_ACTION_BYPASS_EGRESS_TRANS);
    }

    cmd_data_check_element("trigger intr", "no", "usage: <yes/no/y/n>\n",
                           cmd_data_check_confirm, (cmd, A_FALSE, &tmpdata,
                                   sizeof (a_bool_t)));

    if (tmpdata)
    {
        FAL_ACTION_FLG_SET(entry->action_flg, FAL_ACL_ACTION_MATCH_TRIGGER_INTR);
    }

    /* by pass action configuration */
    cmd_data_check_element("bypass bitmap change", "no", "usage: <yes/no/y/n>\n",
                           cmd_data_check_confirm, (cmd, A_FALSE, &tmpdata,
                                   sizeof (a_bool_t)));
    if (A_TRUE == tmpdata)
    {
	cmd_data_check_element("bypass in vlan miss", "no", "usage: <yes/no/y/n>\n",
			cmd_data_check_confirm, (cmd, A_FALSE, &tmpdata,
			   sizeof (a_bool_t)));
	if (tmpdata)
	{
		entry->bypass_bitmap |= (1<<FAL_ACL_BYPASS_IN_VLAN_MISS);
	}

	cmd_data_check_element("bypass source guard", "no", "usage: <yes/no/y/n>\n",
			cmd_data_check_confirm, (cmd, A_FALSE, &tmpdata,
			   sizeof (a_bool_t)));
	if (tmpdata)
	{
		entry->bypass_bitmap |= (1<<FAL_ACL_BYPASS_SOUCE_GUARD);
	}

	cmd_data_check_element("bypass MRU/MTU check", "no", "usage: <yes/no/y/n>\n",
			cmd_data_check_confirm, (cmd, A_FALSE, &tmpdata,
			   sizeof (a_bool_t)));
	if (tmpdata)
	{
		entry->bypass_bitmap |= (1<<FAL_ACL_BYPASS_MRU_MTU_CHECK);
	}

	cmd_data_check_element("bypass egress VSI member check", "no", "usage: <yes/no/y/n>\n",
			cmd_data_check_confirm, (cmd, A_FALSE, &tmpdata,
			   sizeof (a_bool_t)));
	if (tmpdata)
	{
		entry->bypass_bitmap |= (1<<FAL_ACL_BYPASS_EG_VSI_MEMBER_CHECK);
	}

	cmd_data_check_element("bypass egress vlan translation", "no", "usage: <yes/no/y/n>\n",
			cmd_data_check_confirm, (cmd, A_FALSE, &tmpdata,
			   sizeof (a_bool_t)));
	if (tmpdata)
	{
		entry->bypass_bitmap |= (1<<FAL_ACL_BYPASS_EG_VLAN_TRANSLATION);
	}

	cmd_data_check_element("bypass egress vlan tag control", "no", "usage: <yes/no/y/n>\n",
			cmd_data_check_confirm, (cmd, A_FALSE, &tmpdata,
			   sizeof (a_bool_t)));
	if (tmpdata)
	{
		entry->bypass_bitmap |= (1<<FAL_ACL_BYPASS_EG_VLAN_TAG_CTRL);
	}

	cmd_data_check_element("bypass fdb learning", "no", "usage: <yes/no/y/n>\n",
			cmd_data_check_confirm, (cmd, A_FALSE, &tmpdata,
			   sizeof (a_bool_t)));
	if (tmpdata)
	{
		entry->bypass_bitmap |= (1<<FAL_ACL_BYPASS_FDB_LEARNING);
	}

	cmd_data_check_element("bypass fdb refresh", "no", "usage: <yes/no/y/n>\n",
			cmd_data_check_confirm, (cmd, A_FALSE, &tmpdata,
			   sizeof (a_bool_t)));
	if (tmpdata)
	{
		entry->bypass_bitmap |= (1<<FAL_ACL_BYPASS_FDB_REFRESH);
	}

	cmd_data_check_element("bypass L2 security", "no", "usage: <yes/no/y/n>\n",
			cmd_data_check_confirm, (cmd, A_FALSE, &tmpdata,
			   sizeof (a_bool_t)));
	if (tmpdata)
	{
		entry->bypass_bitmap |= (1<<FAL_ACL_BYPASS_L2_SECURITY);
	}

	cmd_data_check_element("bypass management forward", "no", "usage: <yes/no/y/n>\n",
			cmd_data_check_confirm, (cmd, A_FALSE, &tmpdata,
			   sizeof (a_bool_t)));
	if (tmpdata)
	{
		entry->bypass_bitmap |= (1<<FAL_ACL_BYPASS_MANAGEMENT_FWD);
	}

	cmd_data_check_element("bypass L2 forward", "no", "usage: <yes/no/y/n>\n",
			cmd_data_check_confirm, (cmd, A_FALSE, &tmpdata,
			   sizeof (a_bool_t)));
	if (tmpdata)
	{
		entry->bypass_bitmap |= (1<<FAL_ACL_BYPASS_L2_FWD);
	}


	cmd_data_check_element("bypass ingress STP check", "no", "usage: <yes/no/y/n>\n",
			cmd_data_check_confirm, (cmd, A_FALSE, &tmpdata,
			   sizeof (a_bool_t)));
	if (tmpdata)
	{
		entry->bypass_bitmap |= (1<<FAL_ACL_BYPASS_IN_STP_CHECK);
	}

	cmd_data_check_element("bypass egress STP check", "no", "usage: <yes/no/y/n>\n",
			cmd_data_check_confirm, (cmd, A_FALSE, &tmpdata,
			   sizeof (a_bool_t)));
	if (tmpdata)
	{
		entry->bypass_bitmap |= (1<<FAL_ACL_BYPASS_EG_STP_CHECK);
	}

	cmd_data_check_element("bypass source filter", "no", "usage: <yes/no/y/n>\n",
			cmd_data_check_confirm, (cmd, A_FALSE, &tmpdata,
			   sizeof (a_bool_t)));
	if (tmpdata)
	{
		entry->bypass_bitmap |= (1<<FAL_ACL_BYPASS_SOURCE_FILTER);
	}

	cmd_data_check_element("bypass policer", "no", "usage: <yes/no/y/n>\n",
			cmd_data_check_confirm, (cmd, A_FALSE, &tmpdata,
			   sizeof (a_bool_t)));
	if (tmpdata)
	{
		entry->bypass_bitmap |= (1<<FAL_ACL_BYPASS_POLICYER);
	}

	cmd_data_check_element("bypass L2 edit", "no", "usage: <yes/no/y/n>\n",
			cmd_data_check_confirm, (cmd, A_FALSE, &tmpdata,
			   sizeof (a_bool_t)));
	if (tmpdata)
	{
		entry->bypass_bitmap |= (1<<FAL_ACL_BYPASS_L2_EDIT);
	}

	cmd_data_check_element("bypass L3 edit", "no", "usage: <yes/no/y/n>\n",
			cmd_data_check_confirm, (cmd, A_FALSE, &tmpdata,
			   sizeof (a_bool_t)));
	if (tmpdata)
	{
		entry->bypass_bitmap |= (1<<FAL_ACL_BYPASS_L3_EDIT);
	}

	cmd_data_check_element("bypass post acl routing check", "no", "usage: <yes/no/y/n>\n",
			cmd_data_check_confirm, (cmd, A_FALSE, &tmpdata,
			   sizeof (a_bool_t)));
	if (tmpdata)
	{
		entry->bypass_bitmap |= (1<<FAL_ACL_BYPASS_POST_ACL_CHECK_ROUTING);
	}

	cmd_data_check_element("bypass port isolation", "no", "usage: <yes/no/y/n>\n",
			cmd_data_check_confirm, (cmd, A_FALSE, &tmpdata,
			   sizeof (a_bool_t)));
	if (tmpdata)
	{
		entry->bypass_bitmap |= (1<<FAL_ACL_BYPASS_PORT_ISOLATION);
	}
    }

    /*enqueue priority action configuration */
    cmd_data_check_element("enqueue priority change", "no", "usage: <yes/no/y/n>\n",
                           cmd_data_check_confirm, (cmd, A_FALSE, &tmpdata,
                                   sizeof (a_bool_t)));
    if (A_TRUE == tmpdata)
    {
        cmd_data_check_element("enqueue priority", NULL,
                               "usage: the format is integer, 0-15\n",
                               cmd_data_check_integer, (cmd, &tmpdata, 0xf,
                                       0x0));
        entry->enqueue_pri = tmpdata & 0xf;

        FAL_ACTION_FLG_SET(entry->action_flg, FAL_ACL_ACTION_ENQUEUE_PRI);
    }

    /*internal dp action configuration */
    cmd_data_check_element("internal dp change", "no", "usage: <yes/no/y/n>\n",
                           cmd_data_check_confirm, (cmd, A_FALSE, &tmpdata,
                                   sizeof (a_bool_t)));
    if (A_TRUE == tmpdata)
    {
        cmd_data_check_element("internal dp", NULL,
                               "usage: integer value, 0-3\n",
                               cmd_data_check_integer, (cmd, &tmpdata, 0x3,
                                       0x0));
        entry->int_dp = tmpdata & 0x3;

        FAL_ACTION_FLG_SET(entry->action_flg, FAL_ACL_ACTION_INT_DP);
    }

    /*service code action configuration */
    cmd_data_check_element("service code change", "no", "usage: <yes/no/y/n>\n",
                           cmd_data_check_confirm, (cmd, A_FALSE, &tmpdata,
                                   sizeof (a_bool_t)));
    if (A_TRUE == tmpdata)
    {
        cmd_data_check_element("service code", NULL,
                               "usage: 0-255\n",
                               cmd_data_check_integer, (cmd, &tmpdata, 0xff,
                                       0x0));
        entry->service_code = tmpdata & 0xff;

        FAL_ACTION_FLG_SET(entry->action_flg, FAL_ACL_ACTION_SERVICE_CODE);
    }

    /*cpu code action configuration */
    cmd_data_check_element("cpu code change", "no", "usage: <yes/no/y/n>\n",
                           cmd_data_check_confirm, (cmd, A_FALSE, &tmpdata,
                                   sizeof (a_bool_t)));
    if (A_TRUE == tmpdata)
    {
        cmd_data_check_element("cpu code", NULL,
                               "usage: 0-255\n",
                               cmd_data_check_integer, (cmd, &tmpdata, 0xff,
                                       0x0));
        entry->cpu_code = tmpdata & 0xff;

        FAL_ACTION_FLG_SET(entry->action_flg, FAL_ACL_ACTION_CPU_CODE);
    }

    /*sync toggle action configuration */
    cmd_data_check_element("sync toggle", "no", "usage: <yes/no/y/n>\n",
                           cmd_data_check_confirm, (cmd, A_FALSE, &tmpdata,
                                   sizeof (a_bool_t)));
    if (A_TRUE == tmpdata)
    {
        FAL_ACTION_FLG_SET(entry->action_flg, FAL_ACL_ACTION_SYN_TOGGLE);
    }

    /*meta data action configuration */
    cmd_data_check_element("meta data enable", "no", "usage: <yes/no/y/n>\n",
                           cmd_data_check_confirm, (cmd, A_FALSE, &tmpdata,
                                   sizeof (a_bool_t)));
    if (A_TRUE == tmpdata)
    {
        FAL_ACTION_FLG_SET(entry->action_flg, FAL_ACL_ACTION_METADATA_EN);
    }

    cmd_data_check_element("qos res prec", "0",
                           "usage: the format is 0x0-0x7 or 0-7\n",
                            cmd_data_check_integer, (cmd, &tmpdata, 0x7,
                                    0x0));
    entry->qos_res_prec = tmpdata;

    return SW_OK;
}

sw_error_t
cmd_data_check_aclrule(char *info, void *val, a_uint32_t size)
{
    char *cmd;
    sw_error_t rv;
    fal_acl_rule_t entry;
    a_uint32_t tmpdata = 0;

    memset(&entry, 0, sizeof (fal_acl_rule_t));

    dprintf("\n");
    g_aclcmd_len = snprintf(g_aclcmd, 500-g_aclcmd_len, "ssdk_sh acl rule add [listid] [ruleid] 1 ");

    cmd_data_check_element("post routing enable", "no",
                           "usage: <yes/no/y/n>\n", cmd_data_check_confirm,
                           (cmd, A_FALSE, &entry.post_routing, sizeof (a_bool_t)));

    cmd_data_check_element("priority", "0x0",
                       "usage: the format is 0x0-0x7 or 0-7 \n",
                       cmd_data_check_integer, (cmd, &tmpdata, 0x7,
                               0x0));
    entry.pri = tmpdata;
    cmd_data_check_element("acl pool", "0x0",
                       "usage: the format is 0x0-0x1 or 0-1 \n",
                       cmd_data_check_integer, (cmd, &tmpdata, 0x1,
                               0x0));
    entry.acl_pool = tmpdata;
    /* get rule type configuration */
    cmd_data_check_element("rule type", NULL, "usage: <mac/ip4/ip6/udf> \n",
                           cmd_data_check_ruletype, (cmd, &entry.rule_type,
                                   sizeof
                                   (fal_acl_rule_type_t)));

    if (FAL_ACL_RULE_MAC == entry.rule_type)
    {
        rv = cmd_data_check_mac_field(&entry);
        if (SW_OK != rv)
        {
            return rv;
        }
    }

    if (FAL_ACL_RULE_IP4 == entry.rule_type)
    {
        rv = cmd_data_check_mac_field(&entry);
        if (SW_OK != rv)
        {
            return rv;
        }

        rv = cmd_data_check_ip4_field(&entry);
        if (SW_OK != rv)
        {
            return rv;
        }

        rv = cmd_data_check_ip_field(&entry);
        if (SW_OK != rv)
        {
            return rv;
        }
    }

    if (FAL_ACL_RULE_IP6 == entry.rule_type)
    {
        rv = cmd_data_check_mac_field(&entry);
        if (SW_OK != rv)
        {
            return rv;
        }

        rv = cmd_data_check_ip6_field(&entry);
        if (SW_OK != rv)
        {
            return rv;
        }

        rv = cmd_data_check_ip_field(&entry);
        if (SW_OK != rv)
        {
            return rv;
        }
    }

    rv = cmd_data_check_udf_field(&entry);
    if (SW_OK != rv)
    {
        return rv;
    }

    /* get rule inverse configuration */
    cmd_data_check_element("rule inverse", "no", "usage: <yes/no/y/n>\n",
                           cmd_data_check_confirm, (cmd, A_FALSE, &tmpdata,
                                   sizeof (a_bool_t)));

    if (tmpdata)
    {
        FAL_FIELD_FLG_SET(entry.field_flg, FAL_ACL_FIELD_INVERSE_ALL);
    }

    rv = cmd_data_check_acl_action(&entry);
    if (SW_OK != rv)
    {
        return rv;
    }
    //printf("\n %s \n", g_aclcmd);
    *(fal_acl_rule_t *) val = entry;
    return SW_OK;
}

static void cmd_data_print_acl_bypass_bitmap(a_uint32_t bitmap)
{
        dprintf("\t[bypass_in_vlan_miss]:0x%x\n", (bitmap>>FAL_ACL_BYPASS_IN_VLAN_MISS)&0x1);
        dprintf("\t[bypass_source_guard]:0x%x\n", (bitmap>>FAL_ACL_BYPASS_SOUCE_GUARD)&0x1);
        dprintf("\t[bypass_mru_mtu_check]:0x%x\n", (bitmap>>FAL_ACL_BYPASS_MRU_MTU_CHECK)&0x1);
        dprintf("\t[bypass_eg_vsi_member_check]:0x%x\n", (bitmap>>FAL_ACL_BYPASS_EG_VSI_MEMBER_CHECK)&0x1);
        dprintf("\t[bypass_eg_vlan_translation]:0x%x\n", (bitmap>>FAL_ACL_BYPASS_EG_VLAN_TRANSLATION)&0x1);
        dprintf("\t[bypass_eg_vlan_tag_ctrl]:0x%x\n", (bitmap>>FAL_ACL_BYPASS_EG_VLAN_TAG_CTRL)&0x1);
        dprintf("\t[bypass_fdb_learning]:0x%x\n", (bitmap>>FAL_ACL_BYPASS_FDB_LEARNING)&0x1);
        dprintf("\t[bypass_fdb_refresh]:0x%x\n", (bitmap>>FAL_ACL_BYPASS_FDB_REFRESH)&0x1);
        dprintf("\t[bypass_l2_security]:0x%x\n", (bitmap>>FAL_ACL_BYPASS_L2_SECURITY)&0x1);
        dprintf("\t[bypass_management_fwd]:0x%x\n", (bitmap>>FAL_ACL_BYPASS_MANAGEMENT_FWD)&0x1);
        dprintf("\t[bypass_l2_fwd]:0x%x\n", (bitmap>>FAL_ACL_BYPASS_L2_FWD)&0x1);
        dprintf("\t[bypass_in_stp_check]:0x%x\n", (bitmap>>FAL_ACL_BYPASS_IN_STP_CHECK)&0x1);
        dprintf("\t[bypass_eg_stp_check]:0x%x\n", (bitmap>>FAL_ACL_BYPASS_EG_STP_CHECK)&0x1);
        dprintf("\t[bypass_source_filter]:0x%x\n", (bitmap>>FAL_ACL_BYPASS_SOURCE_FILTER)&0x1);
        dprintf("\t[bypass_policer]:0x%x\n", (bitmap>>FAL_ACL_BYPASS_POLICYER)&0x1);
        dprintf("\t[bypass_l2_edit]:0x%x\n", (bitmap>>FAL_ACL_BYPASS_L2_EDIT)&0x1);
        dprintf("\t[bypass_l3_edit]:0x%x\n", (bitmap>>FAL_ACL_BYPASS_L3_EDIT)&0x1);
        dprintf("\t[bypass_post_acl_check_routing]:0x%x\n", (bitmap>>FAL_ACL_BYPASS_POST_ACL_CHECK_ROUTING)&0x1);
        dprintf("\t[bypass_port_isolation]:0x%x\n", (bitmap>>FAL_ACL_BYPASS_PORT_ISOLATION)&0x1);
	return;
}
void
cmd_data_print_aclrule(a_char_t * param_name, a_uint32_t * buf,
                       a_uint32_t size)
{
    fal_acl_rule_t *rule;

    rule = (fal_acl_rule_t *) buf;

    cmd_data_print_ruletype("\n[rule_type]:",
                            (a_uint32_t *) & (rule->rule_type),
                            sizeof (fal_acl_rule_type_t));

    dprintf("\n[priority]:0x%x", rule->pri);
    cmd_data_print_confirm("\n[post_routing_en]:", rule->post_routing, sizeof(a_uint32_t));
    dprintf("\n[acl_pool]:0x%x", rule->acl_pool);

    if (FAL_FIELD_FLG_TST(rule->field_flg, FAL_ACL_FIELD_FAKE_MAC_HEADER))
    {
    	cmd_data_print_confirm("\n[fake_mac_header]:", rule->is_fake_mac_header_val, sizeof(a_uint32_t));
    }
    if (FAL_FIELD_FLG_TST(rule->field_flg, FAL_ACL_FIELD_SNAP))
    {
    	cmd_data_print_confirm("\n[snap]:", rule->is_snap_val, sizeof(a_uint32_t));
    }
    if (FAL_FIELD_FLG_TST(rule->field_flg, FAL_ACL_FIELD_ETHERNET))
    {
    	cmd_data_print_confirm("\n[ethernet]:", rule->is_ethernet_val, sizeof(a_uint32_t));
    }

    if (FAL_FIELD_FLG_TST(rule->field_flg, FAL_ACL_FIELD_MAC_DA))
    {
        cmd_data_print_macaddr("\n[mac_dst_addr]:",
                               (a_uint32_t *) & (rule->dest_mac_val),
                               sizeof (fal_mac_addr_t));
        cmd_data_print_macaddr("  [mac_dst_addr_mask]:",
                               (a_uint32_t *) & (rule->dest_mac_mask),
                               sizeof (fal_mac_addr_t));
    }

    if (FAL_FIELD_FLG_TST(rule->field_flg, FAL_ACL_FIELD_MAC_SA))
    {
        cmd_data_print_macaddr("\n[mac_src_addr]:",
                               (a_uint32_t *) & (rule->src_mac_val),
                               sizeof (fal_mac_addr_t));
        cmd_data_print_macaddr("  [mac_src_addr_mask]:",
                               (a_uint32_t *) & (rule->src_mac_mask),
                               sizeof (fal_mac_addr_t));
    }

    if (FAL_FIELD_FLG_TST(rule->field_flg, FAL_ACL_FIELD_MAC_ETHTYPE))
    {
        dprintf("\n[mac_eth_type]:0x%x", rule->ethtype_val);
        dprintf("  [mac_eth_type_mask]:0x%x", rule->ethtype_mask);
    }

    if (FAL_FIELD_FLG_TST(rule->field_flg, FAL_ACL_FIELD_MAC_TAGGED))
    {
        dprintf("\n[mac_tagged]:0x%x", rule->tagged_val);
        dprintf("  [mac_tagged_mask]:0x%x", rule->tagged_mask);
    }

    if (FAL_FIELD_FLG_TST(rule->field_flg, FAL_ACL_FIELD_MAC_UP))
    {
        dprintf("\n[mac_up]:0x%x", rule->up_val);
        dprintf("  [mac_up_mask]:0x%x", rule->up_mask);
    }

    if (FAL_FIELD_FLG_TST(rule->field_flg, FAL_ACL_FIELD_MAC_CFI))
    {
        dprintf("\n[mac_cfi]:0x%x", rule->cfi_val);
        dprintf("  [mac_cfi_mask]:0x%x", rule->cfi_mask);
    }

    if (FAL_FIELD_FLG_TST(rule->field_flg, FAL_ACL_FIELD_MAC_VID))
    {
        cmd_data_print_fieldop("\n[mac_vlanid_op]:",
                               (a_uint32_t *) & (rule->vid_op),
                               sizeof (fal_acl_field_op_t));
        if (FAL_ACL_FIELD_MASK == rule->vid_op)
        {
            dprintf("  [vlanid]:0x%x", rule->vid_val);
            dprintf("  [vlanid_mask]:0x%x", rule->vid_mask);
        }
        else
        {
            dprintf("  [vlanid_low]:0x%x", rule->vid_val);
            dprintf("  [vlanid_high]:0x%x", rule->vid_mask);
        }
    }

    if (FAL_FIELD_FLG_TST(rule->field_flg, FAL_ACL_FIELD_MAC_STAGGED))
    {
        dprintf("\n[mac_stagged]:0x%x", rule->stagged_val);
        dprintf("  [mac_stagged_mask]:0x%x", rule->stagged_mask);
    }

    if (FAL_FIELD_FLG_TST(rule->field_flg, FAL_ACL_FIELD_MAC_STAG_PRI))
    {
        dprintf("\n[mac_stag_pri]:0x%x", rule->stag_pri_val);
        dprintf("  [mac_stag_pri_mask]:0x%x", rule->stag_pri_mask);
    }

    if (FAL_FIELD_FLG_TST(rule->field_flg, FAL_ACL_FIELD_MAC_STAG_DEI))
    {
        dprintf("\n[mac_stag_dei]:0x%x", rule->stag_dei_val);
        dprintf("  [mac_stag_dei_mask]:0x%x", rule->stag_dei_mask);
    }

    if (FAL_FIELD_FLG_TST(rule->field_flg, FAL_ACL_FIELD_MAC_STAG_VID))
    {
        cmd_data_print_fieldop("\n[mac_stag_vlanid_op]:",
                               (a_uint32_t *) & (rule->stag_vid_op),
                               sizeof (fal_acl_field_op_t));
        if (FAL_ACL_FIELD_MASK == rule->stag_vid_op)
        {
            dprintf("  [stag_vlanid]:0x%x", rule->stag_vid_val);
            dprintf("  [stag_vlanid_mask]:0x%x", rule->stag_vid_mask);
        }
        else
        {
            dprintf("  [stag_vlanid_low]:0x%x", rule->stag_vid_val);
            dprintf("  [stag_vlanid_high]:0x%x", rule->stag_vid_mask);
        }
    }

    if (FAL_FIELD_FLG_TST(rule->field_flg, FAL_ACL_FIELD_MAC_CTAGGED))
    {
        dprintf("\n[mac_ctagged]:0x%x", rule->ctagged_val);
        dprintf("  [mac_ctagged_mask]:0x%x", rule->ctagged_mask);
    }

    if (FAL_FIELD_FLG_TST(rule->field_flg, FAL_ACL_FIELD_MAC_CTAG_PRI))
    {
        dprintf("\n[mac_ctag_pri]:0x%x", rule->ctag_pri_val);
        dprintf("  [mac_ctag_pri_mask]:0x%x", rule->ctag_pri_mask);
    }

    if (FAL_FIELD_FLG_TST(rule->field_flg, FAL_ACL_FIELD_MAC_CTAG_CFI))
    {
        dprintf("\n[mac_ctag_cfi]:0x%x", rule->ctag_cfi_val);
        dprintf("  [mac_ctag_cfi_mask]:0x%x", rule->ctag_cfi_mask);
    }

    if (FAL_FIELD_FLG_TST(rule->field_flg, FAL_ACL_FIELD_MAC_CTAG_VID))
    {
        cmd_data_print_fieldop("\n[mac_ctag_vlanid_op]:",
                               (a_uint32_t *) & (rule->ctag_vid_op),
                               sizeof (fal_acl_field_op_t));
        if (FAL_ACL_FIELD_MASK == rule->ctag_vid_op)
        {
            dprintf("  [ctag_vlanid]:0x%x", rule->ctag_vid_val);
            dprintf("  [ctag_vlanid_mask]:0x%x", rule->ctag_vid_mask);
        }
        else
        {
            dprintf("  [ctag_vlanid_low]:0x%x", rule->ctag_vid_val);
            dprintf("  [ctag_vlanid_high]:0x%x", rule->ctag_vid_mask);
        }
    }

    if (FAL_FIELD_FLG_TST(rule->field_flg, FAL_ACL_FIELD_VSI_VALID))
    {
    	cmd_data_print_confirm("\n[vsi_valid]:", rule->vsi_valid, sizeof(a_uint32_t));
    }

    if (FAL_FIELD_FLG_TST(rule->field_flg, FAL_ACL_FIELD_VSI))
    {
        dprintf("\n[vsi]:0x%x", rule->vsi);
        dprintf("  [vsi_mask]:0x%x", rule->vsi_mask);
    }

    if (FAL_FIELD_FLG_TST(rule->field_flg, FAL_ACL_FIELD_PPPOE_SESSIONID))
    {
        dprintf("\n[pppoe_session_id]:0x%x", rule->pppoe_sessionid);
        dprintf("  [pppoe_session_id_mask]:0x%x", rule->pppoe_sessionid_mask);
    }

    if (FAL_FIELD_FLG_TST(rule->field_flg, FAL_ACL_FIELD_IP))
    {
    	cmd_data_print_confirm("\n[is_ip]:", rule->is_ip_val, sizeof(a_uint32_t));
    }

    if (FAL_FIELD_FLG_TST(rule->field_flg, FAL_ACL_FIELD_IPV6))
    {
    	cmd_data_print_confirm("\n[is_ipv6]:", rule->is_ipv6_val, sizeof(a_uint32_t));
    }

    if (FAL_FIELD_FLG_TST(rule->field_flg, FAL_ACL_FIELD_IP4_DIP))
    {
        cmd_data_print_ip4addr("\n[ip4_dst_addr]:",
                               (a_uint32_t *) & (rule->dest_ip4_val),
                               sizeof (fal_ip4_addr_t));
        cmd_data_print_ip4addr("  [ip4_dst_addr_mask]:",
                               (a_uint32_t *) & (rule->dest_ip4_mask),
                               sizeof (fal_ip4_addr_t));
    }

    if (FAL_FIELD_FLG_TST(rule->field_flg, FAL_ACL_FIELD_IP4_SIP))
    {
        cmd_data_print_ip4addr("\n[ip4_src_addr]:",
                               (a_uint32_t *) & (rule->src_ip4_val),
                               sizeof (fal_ip4_addr_t));
        cmd_data_print_ip4addr("  [ip4_src_addr_mask]:",
                               (a_uint32_t *) & (rule->src_ip4_mask),
                               sizeof (fal_ip4_addr_t));
    }

    if (FAL_FIELD_FLG_TST(rule->field_flg, FAL_ACL_FIELD_RIPV1))
    {
        dprintf("\n[ip4_ripv1]:0x%x", rule->ripv1_val);
        dprintf("  [ip4_ripv1_mask]:0x%x", rule->ripv1_mask);
    }

    if (FAL_FIELD_FLG_TST(rule->field_flg, FAL_ACL_FIELD_DHCPV4))
    {
        dprintf("\n[ip4_dhcpv4]:0x%x", rule->dhcpv4_val);
        dprintf("  [ip4_dhcpv4_mask]:0x%x", rule->dhcpv4_mask);
    }

    if (FAL_FIELD_FLG_TST(rule->field_flg, FAL_ACL_FIELD_IP6_DIP))
    {
        cmd_data_print_ip6addr("\n[ip6_dst_addr]:",
                               (a_uint32_t *) & (rule->dest_ip6_val),
                               sizeof (fal_ip6_addr_t));
        cmd_data_print_ip6addr("\n[ip6_dst_addr_mask]:",
                               (a_uint32_t *) & (rule->dest_ip6_mask),
                               sizeof (fal_ip6_addr_t));
    }

    if (FAL_FIELD_FLG_TST(rule->field_flg, FAL_ACL_FIELD_IP6_SIP))
    {
        cmd_data_print_ip6addr("\n[ip6_src_addr]:",
                               (a_uint32_t *) & (rule->src_ip6_val),
                               sizeof (fal_ip6_addr_t));
        cmd_data_print_ip6addr("\n[ip6_src_addr_mask]:",
                               (a_uint32_t *) & (rule->src_ip6_mask),
                               sizeof (fal_ip6_addr_t));
    }

    if (FAL_FIELD_FLG_TST(rule->field_flg, FAL_ACL_FIELD_IP6_LABEL))
    {
        dprintf("\n[ip6_flow_label]:0x%x", rule->ip6_lable_val);
        dprintf("  [ip6_flow_label_mask]:0x%x", rule->ip6_lable_mask);
    }

    if (FAL_FIELD_FLG_TST(rule->field_flg, FAL_ACL_FIELD_DHCPV6))
    {
        dprintf("\n[ip6_dhcpv6]:0x%x", rule->dhcpv6_val);
        dprintf("  [ip6_dhcpv6_mask]:0x%x", rule->dhcpv6_mask);
    }

    if (FAL_FIELD_FLG_TST(rule->field_flg, FAL_ACL_FIELD_AH_HEADER))
    {
    	cmd_data_print_confirm("\n[is_ah_header]:", rule->is_ah_header_val, sizeof(a_uint32_t));
    }

    if (FAL_FIELD_FLG_TST(rule->field_flg, FAL_ACL_FIELD_ESP_HEADER))
    {
    	cmd_data_print_confirm("\n[is_esp_header]:", rule->is_esp_header_val, sizeof(a_uint32_t));
    }

    if (FAL_FIELD_FLG_TST(rule->field_flg, FAL_ACL_FIELD_MOBILITY_HEADER))
    {
    	cmd_data_print_confirm("\n[is_mobility_header]:", rule->is_mobility_header_val, sizeof(a_uint32_t));
    }

    if (FAL_FIELD_FLG_TST(rule->field_flg, FAL_ACL_FIELD_FRAGMENT_HEADER))
    {
    	cmd_data_print_confirm("\n[is_fragment_header]:", rule->is_fragment_header_val, sizeof(a_uint32_t));
    }

    if (FAL_FIELD_FLG_TST(rule->field_flg, FAL_ACL_FIELD_OTHER_EXT_HEADER))
    {
    	cmd_data_print_confirm("\n[is_other_header]:", rule->is_other_header_val, sizeof(a_uint32_t));
    }

    if (FAL_FIELD_FLG_TST(rule->field_flg, FAL_ACL_FIELD_L3_TTL))
    {
        dprintf("\n[l3_ttl]:0x%x", rule->l3_ttl);
        dprintf("  [l3_ttl_mask]:0x%x", rule->l3_ttl_mask);
    }

    if (FAL_FIELD_FLG_TST(rule->field_flg, FAL_ACL_FIELD_IPV4_OPTION))
    {
    	cmd_data_print_confirm("\n[is_ipv4_option]:", rule->is_ipv4_option_val, sizeof(a_uint32_t));
    }

    if (FAL_FIELD_FLG_TST(rule->field_flg, FAL_ACL_FIELD_FIRST_FRAGMENT))
    {
    	cmd_data_print_confirm("\n[is_first_fragment]:", rule->is_first_frag_val, sizeof(a_uint32_t));
    }

    if (FAL_FIELD_FLG_TST(rule->field_flg, FAL_ACL_FIELD_L3_LENGTH))
    {
        dprintf("\n[l3_length]:0x%x", rule->l3_length);
        dprintf("  [l3_length_mask]:0x%x", rule->l3_length_mask);
    }

    if (FAL_FIELD_FLG_TST(rule->field_flg, FAL_ACL_FIELD_IP_PKT_TYPE))
    {
    	cmd_data_print_ip_packet_type("\n[l3_packet_type]:", &rule->l3_pkt_type, sizeof(a_uint16_t));
    }

    if (FAL_FIELD_FLG_TST(rule->field_flg, FAL_ACL_FIELD_IP_PROTO))
    {
        dprintf("\n[ip_proto]:0x%x", rule->ip_proto_val);
        dprintf("  [ip_proto_mask]:0x%x", rule->ip_proto_mask);
    }

    if (FAL_FIELD_FLG_TST(rule->field_flg, FAL_ACL_FIELD_IP_DSCP))
    {
        dprintf("\n[ip_dscp]:0x%x", rule->ip_dscp_val);
        dprintf("  [ip_dscp_mask]:0x%x", rule->ip_dscp_mask);
    }

    if (FAL_FIELD_FLG_TST(rule->field_flg, FAL_ACL_FIELD_L3_FRAGMENT))
    {
    	cmd_data_print_confirm("\n[is_l3_fragment]:", rule->is_fragement_val, sizeof(a_uint32_t));
    }

    if (FAL_FIELD_FLG_TST(rule->field_flg, FAL_ACL_FIELD_L4_DPORT))
    {
        cmd_data_print_fieldop("\n[ip_l4_dport_op]:",
                               (a_uint32_t *) & (rule->dest_l4port_op),
                               sizeof (fal_acl_field_op_t));
        if (FAL_ACL_FIELD_MASK == rule->dest_l4port_op)
        {
            dprintf("  [dport]:0x%x", rule->dest_l4port_val);
            dprintf("  [dport_mask]:0x%x", rule->dest_l4port_mask);
        }
        else
        {
            dprintf("  [dport_low]:0x%x", rule->dest_l4port_val);
            dprintf("  [dport_high]:0x%x", rule->dest_l4port_mask);
        }
    }

    if (FAL_FIELD_FLG_TST(rule->field_flg, FAL_ACL_FIELD_L4_SPORT))
    {
        cmd_data_print_fieldop("\n[ip_l4_sport_op]:",
                               (a_uint32_t *) & (rule->src_l4port_op),
                               sizeof (fal_acl_field_op_t));
        if (FAL_ACL_FIELD_MASK == rule->src_l4port_op)
        {
            dprintf("  [sport]:0x%x", rule->src_l4port_val);
            dprintf("  [sport_mask]:0x%x", rule->src_l4port_mask);
        }
        else
        {
            dprintf("  [sport_low]:0x%x", rule->src_l4port_val);
            dprintf("  [sport_high]:0x%x", rule->src_l4port_mask);
        }
    }

    if (FAL_FIELD_FLG_TST(rule->field_flg, FAL_ACL_FIELD_TCP_FLAG))
    {
        dprintf("\n[ip_tcp_flags]:0x%x", rule->tcp_flag_val);
        dprintf("  [ip_tcp_flags_mask]:0x%x", rule->tcp_flag_mask);
    }

    if (FAL_FIELD_FLG_TST(rule->field_flg, FAL_ACL_FIELD_ICMP_TYPE))
    {
        dprintf("\n[ip_icmp_type]:0x%x", rule->icmp_type_val);
        dprintf("  [ip_icmp_type_mask]:0x%x", rule->icmp_type_mask);
    }

    if (FAL_FIELD_FLG_TST(rule->field_flg, FAL_ACL_FIELD_ICMP_CODE))
    {
        dprintf("\n[ip_icmp_code]:0x%x", rule->icmp_code_val);
        dprintf("  [ip_icmp_code_mask]:0x%x", rule->icmp_code_mask);
    }

    if (FAL_FIELD_FLG_TST(rule->field_flg, FAL_ACL_FIELD_UDF))
    {
        cmd_data_print_udf_type("\n[udf_type]:",
                                (a_uint32_t *) & (rule->udf_type),
                                sizeof (fal_acl_udf_type_t));

        dprintf("  [offset]:%d", rule->udf_offset);

        cmd_data_print_udf_element("\n[udf_value]:",
                                   (a_uint32_t *) & (rule->udf_val[0]),
                                   rule->udf_len);

        cmd_data_print_udf_element("\n[udf_mask]:",
                                   (a_uint32_t *) & (rule->udf_mask[0]),
                                   rule->udf_len);
    }

    if (FAL_FIELD_FLG_TST(rule->field_flg, FAL_ACL_FIELD_UDF0))
    {
        cmd_data_print_fieldop("\n[udf0_op]:",
                               (a_uint32_t *) & (rule->udf0_op),
                               sizeof (fal_acl_field_op_t));
        if (FAL_ACL_FIELD_MASK == rule->udf0_op)
        {
            dprintf("  [udf0]:0x%x", rule->udf0_val);
            dprintf("  [udf0_mask]:0x%x", rule->udf0_mask);
        }
        else
        {
            dprintf("  [udf0_low]:0x%x", rule->udf0_val);
            dprintf("  [udf0_high]:0x%x", rule->udf0_mask);
        }
    }

    if (FAL_FIELD_FLG_TST(rule->field_flg, FAL_ACL_FIELD_UDF1))
    {
        cmd_data_print_fieldop("\n[udf1_op]:",
                               (a_uint32_t *) & (rule->udf1_op),
                               sizeof (fal_acl_field_op_t));
        if (FAL_ACL_FIELD_MASK == rule->udf1_op)
        {
            dprintf("  [udf1]:0x%x", rule->udf1_val);
            dprintf("  [udf1_mask]:0x%x", rule->udf1_mask);
        }
        else
        {
            dprintf("  [udf1_low]:0x%x", rule->udf1_val);
            dprintf("  [udf1_high]:0x%x", rule->udf1_mask);
        }
    }

    if (FAL_FIELD_FLG_TST(rule->field_flg, FAL_ACL_FIELD_UDF2))
    {
	dprintf("  [udf2]:0x%x", rule->udf2_val);
	dprintf("  [udf2_mask]:0x%x", rule->udf2_mask);
    }

    if (FAL_FIELD_FLG_TST(rule->field_flg, FAL_ACL_FIELD_UDF3))
    {
	dprintf("  [udf3]:0x%x", rule->udf3_val);
	dprintf("  [udf3_mask]:0x%x", rule->udf3_mask);
    }

    if (FAL_FIELD_FLG_TST(rule->field_flg, FAL_ACL_FIELD_INVERSE_ALL))
    {
        dprintf("\n[rule_inverse]:yes");
    }

    if (FAL_ACTION_FLG_TST(rule->action_flg, FAL_ACL_ACTION_PERMIT))
    {
        dprintf("\n[permit]:yes");
    }

    if (FAL_ACTION_FLG_TST(rule->action_flg, FAL_ACL_ACTION_DENY))
    {
        dprintf("\n[deny]:yes");
    }

    if (FAL_ACTION_FLG_TST(rule->action_flg, FAL_ACL_ACTION_RDTCPU))
    {
        dprintf("\n[rdt_to_cpu]:yes");
    }

    if (FAL_ACTION_FLG_TST(rule->action_flg, FAL_ACL_ACTION_CPYCPU))
    {
        dprintf("\n[cpy_to_cpu]:yes");
    }

    if (FAL_ACTION_FLG_TST(rule->action_flg, FAL_ACL_ACTION_MIRROR))
    {
        dprintf("\n[mirror]:yes");
    }
    if (FAL_ACTION_FLG_TST(rule->action_flg, FAL_ACL_ACTION_REDPT))
    {
        a_uint32_t dest_type = FAL_ACL_DEST_TYPE(rule->ports);
        a_uint32_t dest_val = FAL_ACL_DEST_VALUE(rule->ports);
        dprintf("\n[rdt_to_port]:yes   ");
        if(dest_type == FAL_ACL_DEST_PORT_BMP)
        {
            cmd_data_print_portmap("[dest_port]:",
                                        dest_val, sizeof(a_uint32_t));
        }
        else if(dest_type == FAL_ACL_DEST_PORT_ID)
        {
            cmd_data_print_uint32("dest_port",
                                        &dest_val, sizeof(a_uint32_t));
        }
        else if(dest_type == FAL_ACL_DEST_NEXTHOP)
        {
            cmd_data_print_uint32("dest_port(next_hop)",
                                        &dest_val, sizeof(a_uint32_t));
        }
    }

    if (FAL_ACTION_FLG_TST(rule->action_flg, FAL_ACL_ACTION_MODIFY_VLAN))
    {
        dprintf("\n[modify_vlan_id]:yes");
        dprintf("  [vlan_id]:%d", rule->vid);
        if (!FAL_ACTION_FLG_TST(rule->action_flg, FAL_ACL_ACTION_REDPT))
        {
            cmd_data_print_portmap("  [port_member]:", rule->ports,
                                   sizeof (fal_pbmp_t));
        }
    }

    if (FAL_ACTION_FLG_TST(rule->action_flg, FAL_ACL_ACTION_NEST_VLAN))
    {
        dprintf("\n[nest_vlan]:yes");
        dprintf("  [vlan_id]:%d", rule->vid);
    }

    if (FAL_ACTION_FLG_TST(rule->action_flg, FAL_ACL_ACTION_REMARK_DSCP))
    {
        dprintf("\n[remark_dscp]:yes");
        dprintf("  [dscp]:%d", rule->dscp);
        dprintf("  [dscp_mask]:%d", rule->dscp_mask);
    }

    if (FAL_ACTION_FLG_TST(rule->action_flg, FAL_ACL_ACTION_REMARK_UP))
    {
        dprintf("\n[remark_up]:yes");
        dprintf("  [up]:%d", rule->up);
    }

    if (FAL_ACTION_FLG_TST(rule->action_flg, FAL_ACL_ACTION_REMARK_QUEUE))
    {
        dprintf("\n[remark_queue]:yes");
        dprintf("  [queue]:%d", rule->queue);
    }

    dprintf("\n[stag_fmt]:%d", rule->stag_fmt);
    dprintf("\n[stag_vid]:%d", rule->stag_vid);
    dprintf("\n[ctag_fmt]:%d", rule->ctag_fmt);
    dprintf("\n[ctag_vid]:%d", rule->ctag_vid);

    if (FAL_ACTION_FLG_TST(rule->action_flg, FAL_ACL_ACTION_REMARK_LOOKUP_VID))
    {
        dprintf("\n[change_lookup_vid]:yes");
    }

    if (FAL_ACTION_FLG_TST(rule->action_flg, FAL_ACL_ACTION_REMARK_STAG_VID))
    {
        dprintf("\n[change_stag_vid]:yes");
    }

    if (FAL_ACTION_FLG_TST(rule->action_flg, FAL_ACL_ACTION_REMARK_CTAG_VID))
    {
        dprintf("\n[change_ctag_vid]:yes");
    }

    if (FAL_ACTION_FLG_TST(rule->action_flg, FAL_ACL_ACTION_REMARK_STAG_PRI))
    {
        dprintf("\n[change_stag_pri]:yes");
        dprintf("  [stag_pri]:%d", rule->stag_pri);
    }

    if (FAL_ACTION_FLG_TST(rule->action_flg, FAL_ACL_ACTION_REMARK_STAG_DEI))
    {
        dprintf("\n[change_stag_dei]:yes");
        dprintf("  [stag_dei]:%d", rule->stag_dei);
    }

    if (FAL_ACTION_FLG_TST(rule->action_flg, FAL_ACL_ACTION_REMARK_CTAG_PRI))
    {
        dprintf("\n[change_ctag_pri]:yes");
        dprintf("  [ctag_pri]:%d", rule->ctag_pri);
    }

    if (FAL_ACTION_FLG_TST(rule->action_flg, FAL_ACL_ACTION_REMARK_CTAG_CFI))
    {
        dprintf("\n[change_ctag_cfi]:yes");
        dprintf("  [ctag_cfi]:%d", rule->ctag_cfi);
    }

    if (FAL_ACTION_FLG_TST(rule->action_flg, FAL_ACL_ACTION_POLICER_EN))
    {
        dprintf("\n[policer_en]:yes");
        dprintf("  [policer_ptr]:%d", rule->policer_ptr);
    }

    if (FAL_ACTION_FLG_TST(rule->action_flg, FAL_ACL_ACTION_WCMP_EN))
    {
        dprintf("\n[wcmp_en]:yes");
        dprintf("  [wcmp_ptr]:%d", rule->wcmp_ptr);
    }

    if (FAL_ACTION_FLG_TST(rule->action_flg, FAL_ACL_ACTION_ARP_EN))
    {
        dprintf("\n[arp_en]:yes");
        dprintf("  [arp_ptr]:%d", rule->arp_ptr);
    }

    if (FAL_ACTION_FLG_TST(rule->action_flg, FAL_ACL_ACTION_POLICY_FORWARD_EN))
    {
        if (FAL_ACL_POLICY_ROUTE == rule->policy_fwd)
        {
            dprintf("\n[policy_forward]:route");
        }

        if (FAL_ACL_POLICY_SNAT == rule->policy_fwd)
        {
            dprintf("\n[policy_forward]:snat");
        }

        if (FAL_ACL_POLICY_DNAT == rule->policy_fwd)
        {
            dprintf("\n[policy_forward]:dnat");
        }
    }

    if (FAL_ACTION_FLG_TST(rule->action_flg, FAL_ACL_ACTION_BYPASS_EGRESS_TRANS))
    {
        dprintf("\n[eg_bypass]:yes");
    }

    if (FAL_ACTION_FLG_TST(rule->action_flg, FAL_ACL_ACTION_MATCH_TRIGGER_INTR))
    {
        dprintf("\n[trigger_intr]:yes");
    }

    if (rule->bypass_bitmap != 0)
    {
        dprintf("\n[bypass_bitmap]:0x%x\n", rule->bypass_bitmap);
        cmd_data_print_acl_bypass_bitmap(rule->bypass_bitmap);
    }

    if (FAL_ACTION_FLG_TST(rule->action_flg, FAL_ACL_ACTION_ENQUEUE_PRI))
    {
        dprintf("\n[enqueue_priority]:0x%x", rule->enqueue_pri);
    }

    if (FAL_ACTION_FLG_TST(rule->action_flg, FAL_ACL_ACTION_INT_DP))
    {
        dprintf("\n[int_dp]:0x%x", rule->int_dp);
    }

    if (FAL_ACTION_FLG_TST(rule->action_flg, FAL_ACL_ACTION_SERVICE_CODE))
    {
        dprintf("\n[service_code]:0x%x", rule->service_code);
    }

    if (FAL_ACTION_FLG_TST(rule->action_flg, FAL_ACL_ACTION_CPU_CODE))
    {
        dprintf("\n[cpu_code]:0x%x", rule->cpu_code);
    }

    if (FAL_ACTION_FLG_TST(rule->action_flg, FAL_ACL_ACTION_SYN_TOGGLE))
    {
        dprintf("\n[syn_toggle]:yes");
    }
    else
    {
        dprintf("\n[syn_toggle]:no");
    }

    if (FAL_ACTION_FLG_TST(rule->action_flg, FAL_ACL_ACTION_METADATA_EN))
    {
        dprintf("\n[meta_data]:yes");
    }
    else
    {
        dprintf("\n[meta_data]:no");
    }

    dprintf("\n[qos_res_prec]:%d", rule->qos_res_prec);
    dprintf("\n[match_counter]:%d", rule->match_cnt);
    dprintf("\n[match_bytes]:%lld", rule->match_bytes);

    return;
}

sw_error_t
cmd_data_check_patternmode(char *cmd_str, led_pattern_mode_t * arg_val,
                           a_uint32_t size)
{
    if (NULL == cmd_str)
    {
        return SW_BAD_VALUE;
    }

    if (!strcasecmp(cmd_str, "always_off"))
    {
        *arg_val = LED_ALWAYS_OFF;
    }
    else if (!strcasecmp(cmd_str, "always_blink"))
    {
        *arg_val = LED_ALWAYS_BLINK;
    }
    else if (!strcasecmp(cmd_str, "always_on"))
    {
        *arg_val = LED_ALWAYS_ON;
    }
    else  if (!strcasecmp(cmd_str, "map"))
    {
        *arg_val = LED_PATTERN_MAP_EN;
    }
    else
    {
        return SW_BAD_VALUE;
    }

    return SW_OK;
}

sw_error_t
cmd_data_check_blinkfreq(char *cmd_str, led_blink_freq_t * arg_val,
                         a_uint32_t size)
{
    if (NULL == cmd_str)
    {
        return SW_BAD_VALUE;
    }

    if (!strcasecmp(cmd_str, "2HZ"))
    {
        *arg_val = LED_BLINK_2HZ;
    }
    else if (!strcasecmp(cmd_str, "4HZ"))
    {
        *arg_val = LED_BLINK_4HZ;
    }
    else if (!strcasecmp(cmd_str, "8HZ"))
    {
        *arg_val = LED_BLINK_8HZ;
    }
    else if (!strcasecmp(cmd_str, "TXRX"))
    {
        *arg_val = LED_BLINK_TXRX;
    }
    else
    {
        return SW_BAD_VALUE;
    }

    return SW_OK;
}

sw_error_t
cmd_data_check_ledpattern(char *info, void * val, a_uint32_t size)
{
    char *cmd;
    led_ctrl_pattern_t pattern;
    a_uint32_t tmpdata = 0;

    memset(&pattern, 0, sizeof (led_ctrl_pattern_t));

    dprintf("\n");

    /* get pattern mode configuration */
    cmd_data_check_element("pattern_mode", NULL, "usage: <always_off/always_blink/always_on/map>\n",
                           cmd_data_check_patternmode, (cmd, &pattern.mode,
                                   sizeof(led_pattern_mode_t)));

    if (LED_PATTERN_MAP_EN == pattern.mode)
    {
        cmd_data_check_element("full_duplex_light", "no", "usage: <yes/no/y/n>\n",
                               cmd_data_check_confirm, (cmd, A_FALSE, &tmpdata,
                                       sizeof (a_bool_t)));
        if (1 == tmpdata)
        {
            pattern.map |= (1 << FULL_DUPLEX_LIGHT_EN);
        }

        cmd_data_check_element("half_duplex_light", "no", "usage: <yes/no/y/n>\n",
                               cmd_data_check_confirm, (cmd, A_FALSE, &tmpdata,
                                       sizeof (a_bool_t)));
        if (1 == tmpdata)
        {
            pattern.map |= (1 << HALF_DUPLEX_LIGHT_EN);
        }

        cmd_data_check_element("power_on_light", "no", "usage: <yes/no/y/n>\n",
                               cmd_data_check_confirm, (cmd, A_FALSE, &tmpdata,
                                       sizeof (a_bool_t)));
        if (1 == tmpdata)
        {
            pattern.map |= (1 << POWER_ON_LIGHT_EN);
        }

        cmd_data_check_element("active_high", "no", "usage: <yes/no/y/n>\n",
                               cmd_data_check_confirm, (cmd, A_FALSE, &tmpdata,
                                       sizeof (a_bool_t)));
        if (1 == tmpdata)
        {
            pattern.map |= (1 << LED_ACTIVE_HIGH);
        }

        cmd_data_check_element("link_2500m_light", "no", "usage: <yes/no/y/n>\n",
                               cmd_data_check_confirm, (cmd, A_FALSE, &tmpdata,
                                       sizeof (a_bool_t)));
        if (1 == tmpdata)
        {
            pattern.map |= (1 << LINK_2500M_LIGHT_EN);
        }

        cmd_data_check_element("link_1000m_light", "no", "usage: <yes/no/y/n>\n",
                               cmd_data_check_confirm, (cmd, A_FALSE, &tmpdata,
                                       sizeof (a_bool_t)));
        if (1 == tmpdata)
        {
            pattern.map |= (1 << LINK_1000M_LIGHT_EN);
        }

        cmd_data_check_element("link_100m_light", "no", "usage: <yes/no/y/n>\n",
                               cmd_data_check_confirm, (cmd, A_FALSE, &tmpdata,
                                       sizeof (a_bool_t)));
        if (1 == tmpdata)
        {
            pattern.map |= (1 << LINK_100M_LIGHT_EN);
        }

        cmd_data_check_element("link_10m_light", "no", "usage: <yes/no/y/n>\n",
                               cmd_data_check_confirm, (cmd, A_FALSE, &tmpdata,
                                       sizeof (a_bool_t)));
        if (1 == tmpdata)
        {
            pattern.map |= (1 << LINK_10M_LIGHT_EN);
        }

        cmd_data_check_element("conllision_light", "no", "usage: <yes/no/y/n>\n",
                               cmd_data_check_confirm, (cmd, A_FALSE, &tmpdata,
                                       sizeof (a_bool_t)));
        if (1 == tmpdata)
        {
            pattern.map |= (1 << COLLISION_BLINK_EN);
        }

        cmd_data_check_element("rx_traffic_blink", "no", "usage: <yes/no/y/n>\n",
                               cmd_data_check_confirm, (cmd, A_FALSE, &tmpdata,
                                       sizeof (a_bool_t)));
        if (1 == tmpdata)
        {
            pattern.map |= (1 << RX_TRAFFIC_BLINK_EN);
        }

        cmd_data_check_element("tx_traffic_blink", "no", "usage: <yes/no/y/n>\n",
                               cmd_data_check_confirm, (cmd, A_FALSE, &tmpdata,
                                       sizeof (a_bool_t)));
        if (1 == tmpdata)
        {
            pattern.map |= (1 << TX_TRAFFIC_BLINK_EN);
        }

        cmd_data_check_element("linkup_override_light", "no", "usage: <yes/no/y/n>\n",
                               cmd_data_check_confirm, (cmd, A_FALSE, &tmpdata,
                                       sizeof (a_bool_t)));
        if (1 == tmpdata)
        {
            pattern.map |= (1 << LINKUP_OVERRIDE_EN);
        }

        cmd_data_check_element("blink freq", NULL, "usage: <2HZ/4HZ/8HZ/TXRX> \n",
                               cmd_data_check_blinkfreq, (cmd, &pattern.freq,
                                       sizeof(led_blink_freq_t)));
    }

    *(led_ctrl_pattern_t *)val = pattern;

    return SW_OK;
}

void
cmd_data_print_ledpattern(a_uint8_t * param_name, a_uint32_t * buf,
                          a_uint32_t size)
{
    led_ctrl_pattern_t *pattern;

    pattern = (led_ctrl_pattern_t *) buf;

    if (LED_ALWAYS_OFF == pattern->mode)
    {
        dprintf("[pattern_mode]:always_off");
    }
    else if (LED_ALWAYS_BLINK == pattern->mode)
    {
        dprintf("[pattern_mode]:always_blink");
    }
    else if (LED_ALWAYS_ON == pattern->mode)
    {
        dprintf("[pattern_mode]:always_on");
    }
    else
    {
        dprintf("[pattern_mode]:map");
    }
    dprintf("\n");

    if (LED_PATTERN_MAP_EN == pattern->mode)
    {
        if (pattern->map & (1 << FULL_DUPLEX_LIGHT_EN))
        {
            cmd_data_print_confirm("[full_duplex_light]:", A_TRUE, sizeof (a_bool_t));
            dprintf("\n");
        }

        if (pattern->map & (1 << HALF_DUPLEX_LIGHT_EN))
        {
            cmd_data_print_confirm("[half_duplex_light]:", A_TRUE, sizeof (a_bool_t));
            dprintf("\n");
        }

        if (pattern->map & (1 << POWER_ON_LIGHT_EN))
        {
            cmd_data_print_confirm("[power_on_light]:", A_TRUE, sizeof (a_bool_t));
            dprintf("\n");
        }

        if (pattern->map & (1 << LED_ACTIVE_HIGH))
        {
            cmd_data_print_confirm("[active_high]:", A_TRUE, sizeof (a_bool_t));
            dprintf("\n");
        }

        if (pattern->map & (1 << LINK_2500M_LIGHT_EN))
        {
            cmd_data_print_confirm("[link_2500m_light]:", A_TRUE, sizeof (a_bool_t));
            dprintf("\n");
        }

        if (pattern->map & (1 << LINK_1000M_LIGHT_EN))
        {
            cmd_data_print_confirm("[link_1000m_light]:", A_TRUE, sizeof (a_bool_t));
            dprintf("\n");
        }

        if (pattern->map & (1 << LINK_100M_LIGHT_EN))
        {
            cmd_data_print_confirm("[link_100m_light]:", A_TRUE, sizeof (a_bool_t));
            dprintf("\n");
        }

        if (pattern->map & (1 << LINK_10M_LIGHT_EN))
        {
            cmd_data_print_confirm("[link_10m_light]:", A_TRUE, sizeof (a_bool_t));
            dprintf("\n");
        }

        if (pattern->map & (1 << COLLISION_BLINK_EN))
        {
            cmd_data_print_confirm("[conllision_blink]:", A_TRUE, sizeof (a_bool_t));
            dprintf("\n");
        }

        if (pattern->map & (1 << RX_TRAFFIC_BLINK_EN))
        {
            cmd_data_print_confirm("[rx_traffic_blink]:", A_TRUE, sizeof (a_bool_t));
            dprintf("\n");
        }

        if (pattern->map & (1 << TX_TRAFFIC_BLINK_EN))
        {
            cmd_data_print_confirm("[tx_traffic_blink]:", A_TRUE, sizeof (a_bool_t));
            dprintf("\n");
        }

        if (pattern->map & (1 << LINKUP_OVERRIDE_EN))
        {
            cmd_data_print_confirm("[linkup_override]:", A_TRUE, sizeof (a_bool_t));
            dprintf("\n");
        }

        if (LED_BLINK_2HZ == pattern->freq)
        {
            dprintf("[blink_frequency]:2HZ\n");
        }
        else if (LED_BLINK_4HZ == pattern->freq)
        {
            dprintf("[blink_frequency]:4HZ\n");
        }
        else if (LED_BLINK_8HZ == pattern->freq)
        {
            dprintf("[blink_frequency]:8HZ\n");
        }
        else
        {
            dprintf("[blink_frequency]:TXRX\n");
        }
    }
}

sw_error_t
cmd_data_check_mirr_analy_cfg(char *info, void *val, a_uint32_t size)
{
    char *cmd;
    sw_error_t rv;
    fal_mirr_analysis_config_t *pEntry = (fal_mirr_analysis_config_t *)val;

    memset(pEntry, 0, sizeof(fal_mirr_analysis_config_t));

    do
    {
        cmd = get_sub_cmd("analysis_port", "0");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: port id\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_uint32(cmd, &(pEntry->port_id), sizeof(a_uint32_t));
            if (SW_OK != rv)
                dprintf("usage: port id\n");
        }
    }while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("analysis_priority", "0");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: priority range 0x0-0xf\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_uint32(cmd, &(pEntry->priority), sizeof(a_uint32_t));
            if (SW_OK != rv)
                dprintf("usage: priority range 0x0-0xf\n");
        }
    }while (talk_mode && (SW_OK != rv));

    return SW_OK;
}

void
cmd_data_print_mirr_analy_cfg(a_uint8_t * param_name, a_uint32_t * buf, a_uint32_t size)
{
    fal_mirr_analysis_config_t *entry;

    entry = (fal_mirr_analysis_config_t *) buf;
    dprintf("\n");
    dprintf("[analysis_port]:0x%x\n", entry->port_id);
    dprintf("[analysis_priority]:0x%x\n", entry->priority);
}

sw_error_t
cmd_data_check_mirr_direction(char *cmd_str, a_uint32_t * arg_val, a_uint32_t size)
{
    if (cmd_str == NULL)
        return SW_BAD_PARAM;

    if (!strcasecmp(cmd_str, "both"))
    {
        *arg_val = FAL_MIRR_BOTH;
    }
    else if (!strcasecmp(cmd_str, "ingress"))
    {
        *arg_val = FAL_MIRR_INGRESS;
    }
    else if (!strcasecmp(cmd_str, "egress"))
    {
        *arg_val = FAL_MIRR_EGRESS;
    }
    else
    {
        //dprintf("input error \n");
        return SW_BAD_VALUE;
    }

    return SW_OK;
}

void
cmd_data_print_mirr_direction(a_uint8_t * param_name, a_uint32_t * buf, a_uint32_t size)
{
    dprintf("[%s]:", param_name);
    if (*(a_uint32_t *) buf == FAL_MIRR_BOTH)
    {
        dprintf("BOTH");
    }
    else if (*(a_uint32_t *) buf == FAL_MIRR_INGRESS)
    {
        dprintf("INGRESS");
    }
    else if (*(a_uint32_t *) buf == FAL_MIRR_EGRESS)
    {
        dprintf("EGRESS");
    }
    else
    {
        dprintf("UNKNOWN VALUE");
    }
}

/*Shiva*/
sw_error_t
cmd_data_check_invlan_mode(char *cmd_str, a_uint32_t * arg_val, a_uint32_t size)
{
    if (cmd_str == NULL)
        return SW_BAD_PARAM;

    if (!strcasecmp(cmd_str, "admit_all"))
    {
        *arg_val = FAL_INVLAN_ADMIT_ALL;
    }
    else if (!strcasecmp(cmd_str, "admit_tagged"))
    {
        *arg_val = FAL_INVLAN_ADMIT_TAGGED;
    }
    else if (!strcasecmp(cmd_str, "admit_untagged"))
    {
        *arg_val = FAL_INVLAN_ADMIT_UNTAGGED;
    }
    else
    {
        //dprintf("input error \n");
        return SW_BAD_VALUE;
    }

    return SW_OK;
}

void
cmd_data_print_invlan_mode(a_uint8_t * param_name, a_uint32_t * buf, a_uint32_t size)
{
    dprintf("[%s]:", param_name);

    if (*(a_uint32_t *) buf == FAL_INVLAN_ADMIT_ALL)
    {
        dprintf("ADMIT_ALL");
    }
    else if (*(a_uint32_t *) buf == FAL_INVLAN_ADMIT_TAGGED)
    {
        dprintf("ADMIT_TAGGED");
    }
    else if (*(a_uint32_t *) buf == FAL_INVLAN_ADMIT_UNTAGGED)
    {
        dprintf("ADMIT_UNTAGGED");
    }
    else
    {
        dprintf("UNKNOWN VALUE");
    }
}

sw_error_t
cmd_data_check_vlan_propagation(char *cmd_str, a_uint32_t * arg_val, a_uint32_t size)
{
    if (cmd_str == NULL)
        return SW_BAD_PARAM;

    if (!strcasecmp(cmd_str, "disable"))
    {
        *arg_val = FAL_VLAN_PROPAGATION_DISABLE;
    }
    else if (!strcasecmp(cmd_str, "clone"))
    {
        *arg_val = FAL_VLAN_PROPAGATION_CLONE;
    }
    else if (!strcasecmp(cmd_str, "replace"))
    {
        *arg_val = FAL_VLAN_PROPAGATION_REPLACE;
    }
    else
    {
        //dprintf("input error \n");
        return SW_BAD_VALUE;
    }

    return SW_OK;
}

void
cmd_data_print_vlan_propagation(a_char_t * param_name, a_uint32_t * buf, a_uint32_t size)
{
    dprintf("[%s]:", param_name);

    if (*(a_uint32_t *) buf == FAL_VLAN_PROPAGATION_DISABLE)
    {
        dprintf("DISABLE");
    }
    else if (*(a_uint32_t *) buf == FAL_VLAN_PROPAGATION_CLONE)
    {
        dprintf("CLONE");
    }
    else if (*(a_uint32_t *) buf == FAL_VLAN_PROPAGATION_REPLACE)
    {
        dprintf("REPLACE");
    }
    else
    {
        dprintf("UNKNOWN VALUE");
    }
}

sw_error_t
cmd_data_check_vlan_translation(char *info, fal_vlan_trans_entry_t *val, a_uint32_t size)
{
    char *cmd = NULL;
    a_uint32_t tmp;
    sw_error_t rv;
    fal_vlan_trans_entry_t entry;

    memset(&entry, 0, sizeof (fal_vlan_trans_entry_t));

    if (ssdk_cfg.init_cfg.chip_type != CHIP_HPPE) {
	do
	{
		cmd = get_sub_cmd("ovid", "1");
		SW_RTN_ON_NULL_PARAM(cmd);

		if (!strncasecmp(cmd, "quit", 4))
		{
			return SW_BAD_VALUE;

		}
		else if (!strncasecmp(cmd, "help", 4))
		{
			dprintf("usage: the range is 0 -- 4095\n");
			rv = SW_BAD_VALUE;

		}
		else
		{
			rv = cmd_data_check_uint32(cmd, &entry.o_vid, sizeof (a_uint32_t));
			if (SW_OK != rv)
				dprintf("usage: the range is 0 -- 4095\n");
		}
	}
	while (talk_mode && (SW_OK != rv));

	do
	{
		cmd = get_sub_cmd("bi direction", "yes");
		SW_RTN_ON_NULL_PARAM(cmd);

		if (!strncasecmp(cmd, "quit", 4))
		{
			return SW_BAD_VALUE;

		}
		else if (!strncasecmp(cmd, "help", 4))
		{
			dprintf("usage: <yes/no/y/n>\n");
			rv = SW_BAD_VALUE;

		}
		else
		{
			rv = cmd_data_check_confirm(cmd, A_TRUE, &entry.bi_dir,
					sizeof (a_bool_t));
			if (SW_OK != rv)
				dprintf("usage: <yes/no/y/n>\n");
		}

	}
	while (talk_mode && (SW_OK != rv));

	do
	{
		cmd = get_sub_cmd("forward direction", "yes");
		SW_RTN_ON_NULL_PARAM(cmd);

		if (!strncasecmp(cmd, "quit", 4))
		{
			return SW_BAD_VALUE;

		}
		else if (!strncasecmp(cmd, "help", 4))
		{
			dprintf("usage: <yes/no/y/n>\n");
			rv = SW_BAD_VALUE;

		}
		else
		{
			rv = cmd_data_check_confirm(cmd, A_TRUE, &entry.forward_dir,
					sizeof (a_bool_t));
			if (SW_OK != rv)
				dprintf("usage: <yes/no/y/n>\n");
		}

	}
	while (talk_mode && (SW_OK != rv));

	do
	{
		cmd = get_sub_cmd("reverse direction", "yes");
		SW_RTN_ON_NULL_PARAM(cmd);

		if (!strncasecmp(cmd, "quit", 4))
		{
			return SW_BAD_VALUE;

		}
		else if (!strncasecmp(cmd, "help", 4))
		{
			dprintf("usage: <yes/no/y/n>\n");
			rv = SW_BAD_VALUE;

		}
		else
		{
			rv = cmd_data_check_confirm(cmd, A_TRUE, &entry.reverse_dir,
					sizeof (a_bool_t));
			if (SW_OK != rv)
				dprintf("usage: <yes/no/y/n>\n");
		}

	}
	while (talk_mode && (SW_OK != rv));

	do
	{
		cmd = get_sub_cmd("svid", "1");
		SW_RTN_ON_NULL_PARAM(cmd);

		if (!strncasecmp(cmd, "quit", 4))
		{
			return SW_BAD_VALUE;

		}
		else if (!strncasecmp(cmd, "help", 4))
		{
			dprintf("usage: the range is 0 -- 4095\n");
			rv = SW_BAD_VALUE;

		}
		else
		{
			rv = cmd_data_check_uint32(cmd, &entry.s_vid, sizeof (a_uint32_t));
			if (SW_OK != rv)
				dprintf("usage: the range is 0 -- 4095\n");
		}

	}
	while (talk_mode && (SW_OK != rv));

	do
	{
		cmd = get_sub_cmd("cvid", "1");
		SW_RTN_ON_NULL_PARAM(cmd);

		if (!strncasecmp(cmd, "quit", 4))
		{
			return SW_BAD_VALUE;

		}
		else if (!strncasecmp(cmd, "help", 4))
		{
			dprintf("usage: the range is 0 -- 4095\n");
			rv = SW_BAD_VALUE;

		}
		else
		{
			rv = cmd_data_check_uint32(cmd, &entry.c_vid, sizeof (a_uint32_t));
			if (SW_OK != rv)
				dprintf("usage: the range is 0 -- 4095\n");
		}

	}
	while (talk_mode && (SW_OK != rv));

	do
	{
		cmd = get_sub_cmd("ovid_is_cvid", "yes");
		SW_RTN_ON_NULL_PARAM(cmd);

		if (!strncasecmp(cmd, "quit", 4))
		{
			return SW_BAD_VALUE;

		}
		else if (!strncasecmp(cmd, "help", 4))
		{
			dprintf("usage: <yes/no/y/n>\n");
			rv = SW_BAD_VALUE;

		}
		else
		{
			rv = cmd_data_check_confirm(cmd, A_TRUE, &entry.o_vid_is_cvid,
					sizeof (a_bool_t));
			if (SW_OK != rv)
				dprintf("usage: <yes/no/y/n>\n");
		}

	}
	while (talk_mode && (SW_OK != rv));

	do
	{
		cmd = get_sub_cmd("svid_enable", "yes");
		SW_RTN_ON_NULL_PARAM(cmd);

		if (!strncasecmp(cmd, "quit", 4))
		{
			return SW_BAD_VALUE;

		}
		else if (!strncasecmp(cmd, "help", 4))
		{
			dprintf("usage: <yes/no/y/n>\n");
			rv = SW_BAD_VALUE;

		}
		else
		{
			rv = cmd_data_check_confirm(cmd, A_TRUE, &entry.s_vid_enable,
					sizeof (a_bool_t));
			if (SW_OK != rv)
				dprintf("usage: <yes/no/y/n>\n");
		}

	}
	while (talk_mode && (SW_OK != rv));

	do
	{
		cmd = get_sub_cmd("cvid_enable", "yes");
		SW_RTN_ON_NULL_PARAM(cmd);

		if (!strncasecmp(cmd, "quit", 4))
		{
			return SW_BAD_VALUE;

		}
		else if (!strncasecmp(cmd, "help", 4))
		{
			dprintf("usage: <yes/no/y/n>\n");
			rv = SW_BAD_VALUE;

		}
		else
		{
			rv = cmd_data_check_confirm(cmd, A_TRUE, &entry.c_vid_enable,
					sizeof (a_bool_t));
			if (SW_OK != rv)
				dprintf("usage: <yes/no/y/n>\n");
		}

	}
	while (talk_mode && (SW_OK != rv));

	do
	{
		cmd = get_sub_cmd("one_2_one_vlan", "no");
		SW_RTN_ON_NULL_PARAM(cmd);

		if (!strncasecmp(cmd, "quit", 4))
		{
			return SW_BAD_VALUE;

		}
		else if (!strncasecmp(cmd, "help", 4))
		{
			dprintf("usage: <yes/no/y/n>\n");
			rv = SW_BAD_VALUE;

		}
		else
		{
			rv = cmd_data_check_confirm(cmd, A_FALSE, &entry.one_2_one_vlan,
					sizeof (a_bool_t));
			if (SW_OK != rv)
				dprintf("usage: <yes/no/y/n>\n");
		}

	}
	while (talk_mode && (SW_OK != rv));
    }

    if (ssdk_cfg.init_cfg.chip_type == CHIP_HPPE) {
	do
	{
		cmd = get_sub_cmd("direction", "0");
		SW_RTN_ON_NULL_PARAM(cmd);

		if (!strncasecmp(cmd, "quit", 4))
		{
			return SW_BAD_VALUE;

		}
		else if (!strncasecmp(cmd, "help", 4))
		{
			dprintf("usage: 0 for ingress, 1 for egress\n");
			rv = SW_BAD_VALUE;

		}
		else
		{
			rv = cmd_data_check_uint32(cmd, &entry.trans_direction, sizeof (a_uint32_t));
			if (SW_OK != rv)
				dprintf("usage: value should be 0/1, 0=ingress, 1=egress\n");
			else if (entry.trans_direction !=0 && entry.trans_direction != 1)
			{
				dprintf("usage: value should be 0/1, 0=ingress, 1=egress\n");
				rv = SW_BAD_VALUE;
			}
		}

	}
	while (talk_mode && (SW_OK != rv));

	do
	{
		cmd = get_sub_cmd("port_bitmap", "0");
		SW_RTN_ON_NULL_PARAM(cmd);

		if (!strncasecmp(cmd, "quit", 4))
		{
			return SW_BAD_VALUE;
		}
		else if (!strncasecmp(cmd, "help", 4))
		{
			dprintf("usage: 0 for port 0, 1 for port 1,...\n");
			rv = SW_BAD_VALUE;
		}
		else
		{
			rv = cmd_data_check_uint32(cmd, &entry.port_bitmap, sizeof (a_uint32_t));
			if (SW_OK != rv)
				dprintf("usage: 0 for port 0, 1 for port 1,...\n");
		}
	}
	while (talk_mode && (SW_OK != rv));

	do
	{
		cmd = get_sub_cmd("svid_enable", "yes");
		SW_RTN_ON_NULL_PARAM(cmd);

		if (!strncasecmp(cmd, "quit", 4))
		{
			return SW_BAD_VALUE;

		}
		else if (!strncasecmp(cmd, "help", 4))
		{
			dprintf("usage: <yes/no/y/n>\n");
			rv = SW_BAD_VALUE;

		}
		else
		{
			rv = cmd_data_check_confirm(cmd, A_TRUE, &entry.s_vid_enable,
					sizeof (a_bool_t));
			if (SW_OK != rv)
				dprintf("usage: <yes/no/y/n>\n");
		}

	}
	while (talk_mode && (SW_OK != rv));

	do
	{
		cmd = get_sub_cmd("svid", "1");
		SW_RTN_ON_NULL_PARAM(cmd);

		if (!strncasecmp(cmd, "quit", 4))
		{
			return SW_BAD_VALUE;

		}
		else if (!strncasecmp(cmd, "help", 4))
		{
			dprintf("usage: the range is 0 -- 4095\n");
			rv = SW_BAD_VALUE;

		}
		else
		{
			rv = cmd_data_check_uint32(cmd, &entry.s_vid, sizeof (a_uint32_t));
			if (SW_OK != rv)
				dprintf("usage: the range is 0 -- 4095\n");
		}

	}
	while (talk_mode && (SW_OK != rv));

	do
	{
		cmd = get_sub_cmd("cvid_enable", "yes");
		SW_RTN_ON_NULL_PARAM(cmd);

		if (!strncasecmp(cmd, "quit", 4))
		{
			return SW_BAD_VALUE;

		}
		else if (!strncasecmp(cmd, "help", 4))
		{
			dprintf("usage: <yes/no/y/n>\n");
			rv = SW_BAD_VALUE;

		}
		else
		{
			rv = cmd_data_check_confirm(cmd, A_TRUE, &entry.c_vid_enable,
					sizeof (a_bool_t));
			if (SW_OK != rv)
				dprintf("usage: <yes/no/y/n>\n");
		}

	}
	while (talk_mode && (SW_OK != rv));

	do
	{
		cmd = get_sub_cmd("cvid", "1");
		SW_RTN_ON_NULL_PARAM(cmd);

		if (!strncasecmp(cmd, "quit", 4))
		{
			return SW_BAD_VALUE;

		}
		else if (!strncasecmp(cmd, "help", 4))
		{
			dprintf("usage: the range is 0 -- 4095\n");
			rv = SW_BAD_VALUE;

		}
		else
		{
			rv = cmd_data_check_uint32(cmd, &entry.c_vid, sizeof (a_uint32_t));
			if (SW_OK != rv)
				dprintf("usage: the range is 0 -- 4095\n");
		}

	}
	while (talk_mode && (SW_OK != rv));

	if (entry.trans_direction == 1) {
	    do
	    {
	        cmd = get_sub_cmd("vsi_valid", "yes");
			SW_RTN_ON_NULL_PARAM(cmd);

	        if (!strncasecmp(cmd, "quit", 4))
	        {
	            return SW_BAD_VALUE;
	        }
	        else if (!strncasecmp(cmd, "help", 4))
	        {
	            dprintf("usage: <yes/no/y/n>\n");
	            rv = SW_BAD_VALUE;
	        }
	        else
	        {
	            rv = cmd_data_check_confirm(cmd, A_FALSE, &entry.vsi_valid,
	                                        sizeof (a_bool_t));
	            if (SW_OK != rv)
	                dprintf("usage: <yes/no/y/n>\n");
	        }
	    }
	    while (talk_mode && (SW_OK != rv));

	    do
	    {
	        cmd = get_sub_cmd("vsi_enable", "yes");
			SW_RTN_ON_NULL_PARAM(cmd);

	        if (!strncasecmp(cmd, "quit", 4))
	        {
	            return SW_BAD_VALUE;
	        }
	        else if (!strncasecmp(cmd, "help", 4))
	        {
	            dprintf("usage: <yes/no/y/n>\n");
	            rv = SW_BAD_VALUE;
	        }
	        else
	        {
	            rv = cmd_data_check_confirm(cmd, A_FALSE, &entry.vsi_enable,
	                                        sizeof (a_bool_t));
	            if (SW_OK != rv)
	                dprintf("usage: <yes/no/y/n>\n");
	        }
	    }
	    while (talk_mode && (SW_OK != rv));

	    do
	    {
	        cmd = get_sub_cmd("vsi", "0");
			SW_RTN_ON_NULL_PARAM(cmd);

	        if (!strncasecmp(cmd, "quit", 4))
	        {
	            return SW_BAD_VALUE;
	        }
	        else if (!strncasecmp(cmd, "help", 4))
	        {
	            dprintf("usage: the range is 0 -- 31\n");
	            rv = SW_BAD_VALUE;
	        }
	        else
	        {
	            rv = cmd_data_check_uint32(cmd, &entry.vsi, sizeof (a_uint32_t));
	            if (SW_OK != rv)
	                dprintf("usage: the range is 0 -- 31\n");
	        }
	    }
	    while (talk_mode && (SW_OK != rv));
	}

	if (entry.trans_direction == 0) {
	    do
	    {
	        cmd = get_sub_cmd("protocol_enable", "yes");
			SW_RTN_ON_NULL_PARAM(cmd);

	        if (!strncasecmp(cmd, "quit", 4))
	        {
	            return SW_BAD_VALUE;
	        }
	        else if (!strncasecmp(cmd, "help", 4))
	        {
	            dprintf("usage: <yes/no/y/n>\n");
	            rv = SW_BAD_VALUE;
	        }
	        else
	        {
	            rv = cmd_data_check_confirm(cmd, A_FALSE, &entry.protocol_enable,
	                                        sizeof (a_bool_t));
	            if (SW_OK != rv)
	                dprintf("usage: <yes/no/y/n>\n");
	        }
	    }
	    while (talk_mode && (SW_OK != rv));

	    do
	    {
	        cmd = get_sub_cmd("protocol", "0");
			SW_RTN_ON_NULL_PARAM(cmd);

	        if (!strncasecmp(cmd, "quit", 4))
	        {
	            return SW_BAD_VALUE;
	        }
	        else if (!strncasecmp(cmd, "help", 4))
	        {
	            dprintf("usage: for example:0x0800 \n");
	            rv = SW_BAD_VALUE;

	        }
	        else
	        {
	            rv = cmd_data_check_uint16(cmd, &tmp, sizeof (a_uint32_t));
	            if (SW_OK != rv)
	            {
	                dprintf("usage: for example:0x0800 \n");
	            }
	            else
	            {
	                entry.protocol = tmp;
	            }
	        }
	    }
	    while (talk_mode && (SW_OK != rv));

	    do
	    {
	        cmd = get_sub_cmd("frmtype_enable", "yes");
			SW_RTN_ON_NULL_PARAM(cmd);

	        if (!strncasecmp(cmd, "quit", 4))
	        {
	            return SW_BAD_VALUE;
	        }
	        else if (!strncasecmp(cmd, "help", 4))
	        {
	            dprintf("usage: <yes/no/y/n>\n");
	            rv = SW_BAD_VALUE;
	        }
	        else
	        {
	            rv = cmd_data_check_confirm(cmd, A_FALSE, &entry.frmtype_enable,
	                                        sizeof (a_bool_t));
	            if (SW_OK != rv)
	                dprintf("usage: <yes/no/y/n>\n");
	        }
	    }
	    while (talk_mode && (SW_OK != rv));

	    do
	    {
	        cmd = get_sub_cmd("frmtype", "0");
			SW_RTN_ON_NULL_PARAM(cmd);

	        if (!strncasecmp(cmd, "quit", 4))
	        {
	            return SW_BAD_VALUE;
	        }
	        else if (!strncasecmp(cmd, "help", 4))
	        {
	            dprintf("usage: 0 for ethernet, 1 for rfc1024, 2 for llc and 3 for ethernet or rfc1024\n");
	            rv = SW_BAD_VALUE;
	        }
	        else
	        {
	            rv = cmd_data_check_uint32(cmd, &entry.frmtype, sizeof (a_uint32_t));
	            if (SW_OK != rv)
	            dprintf("usage: 0 for ethernet, 1 for rfc1024, 2 for llc and 3 for ethernet or rfc1024\n");
	        }
	    }
	    while (talk_mode && (SW_OK != rv));
	}

       do
       {
			cmd = get_sub_cmd("stag_format", "0");
			SW_RTN_ON_NULL_PARAM(cmd);

           if (!strncasecmp(cmd, "quit", 4))
           {
               return SW_BAD_VALUE;
              }
           else if (!strncasecmp(cmd, "help", 4))
           {
               dprintf("usage: bit 0 for untagged, bit 1 for priority tagged and "
                    "bit 2 for tagged\n");
               rv = SW_BAD_VALUE;
           }
           else
           {
               rv = cmd_data_check_uint8(cmd, &tmp, sizeof (a_uint32_t));
               if (SW_OK != rv)
               {
                    dprintf("usage: bit 0 for untagged, bit 1 for priority "
                        "tagged and bit 2 for tagged\n");
               }
               else
               {
                    entry.s_tagged = tmp;
               }
           }
       }
       while (talk_mode && (SW_OK != rv));

       do
       {
           cmd = get_sub_cmd("ctag_format", "0");
				SW_RTN_ON_NULL_PARAM(cmd);

           if (!strncasecmp(cmd, "quit", 4))
           {
               return SW_BAD_VALUE;
              }
           else if (!strncasecmp(cmd, "help", 4))
           {
               dprintf("usage: bit 0 for untagged, bit 1 for priority tagged and "
                    "bit 2 for tagged\n");
               rv = SW_BAD_VALUE;
           }
           else
           {
               rv = cmd_data_check_uint8(cmd, &tmp, sizeof (a_uint32_t));
               if (SW_OK != rv)
               {
                   dprintf("usage: bit 0 for untagged, bit 1 for priority "
                       "tagged and bit 2 for tagged\n");
               }
               else
               {
                    entry.c_tagged = tmp;
               }
           }
         }
       while (talk_mode && (SW_OK != rv));

       do
       {
           cmd = get_sub_cmd("s_pcp_enable", "yes");
			SW_RTN_ON_NULL_PARAM(cmd);

           if (!strncasecmp(cmd, "quit", 4))
           {
               return SW_BAD_VALUE;
           }
           else if (!strncasecmp(cmd, "help", 4))
           {
               dprintf("usage: <yes/no/y/n>\n");
               rv = SW_BAD_VALUE;
           }
           else
           {
               rv = cmd_data_check_confirm(cmd, A_TRUE, &entry.s_pcp_enable,
                                           sizeof (a_bool_t));
               if (SW_OK != rv)
                   dprintf("usage: <yes/no/y/n>\n");
           }

       }
       while (talk_mode && (SW_OK != rv));

       do
       {
           cmd = get_sub_cmd("s_pcp", "0");
			SW_RTN_ON_NULL_PARAM(cmd);

           if (!strncasecmp(cmd, "quit", 4))
           {
               return SW_BAD_VALUE;
           }
           else if (!strncasecmp(cmd, "help", 4))
           {
               dprintf("usage: the range is 0 -- 7\n");
               rv = SW_BAD_VALUE;
           }
           else
           {
               rv = cmd_data_check_uint8(cmd, &tmp, sizeof (a_uint32_t));
               if (SW_OK != rv)
               {
                   dprintf("usage: the range is 0 -- 7\n");
               }
               else
               {
                   entry.s_pcp = tmp;
               }
           }
       }
       while (talk_mode && (SW_OK != rv));

       do
       {
			cmd = get_sub_cmd("c_pcp_enable", "yes");
			SW_RTN_ON_NULL_PARAM(cmd);

           if (!strncasecmp(cmd, "quit", 4))
           {
               return SW_BAD_VALUE;
           }
           else if (!strncasecmp(cmd, "help", 4))
           {
               dprintf("usage: <yes/no/y/n>\n");
               rv = SW_BAD_VALUE;
           }
           else
           {
               rv = cmd_data_check_confirm(cmd, A_TRUE, &entry.c_pcp_enable,
                                           sizeof (a_bool_t));
               if (SW_OK != rv)
                   dprintf("usage: <yes/no/y/n>\n");
           }

       }
       while (talk_mode && (SW_OK != rv));

       do
       {
			cmd = get_sub_cmd("c_pcp", "0");
			SW_RTN_ON_NULL_PARAM(cmd);

           if (!strncasecmp(cmd, "quit", 4))
           {
               return SW_BAD_VALUE;
           }
           else if (!strncasecmp(cmd, "help", 4))
           {
               dprintf("usage: the range is 0 -- 7\n");
               rv = SW_BAD_VALUE;
           }
           else
           {
               rv = cmd_data_check_uint8(cmd, &tmp, sizeof (a_uint32_t));
               if (SW_OK != rv)
               {
                   dprintf("usage: the range is 0 -- 7\n");
               }
               else
               {
                   entry.c_pcp = tmp;
               }
           }
       }
       while (talk_mode && (SW_OK != rv));

       do
       {
           cmd = get_sub_cmd("s_dei_enable", "yes");
			SW_RTN_ON_NULL_PARAM(cmd);

           if (!strncasecmp(cmd, "quit", 4))
           {
               return SW_BAD_VALUE;
           }
           else if (!strncasecmp(cmd, "help", 4))
           {
               dprintf("usage: <yes/no/y/n>\n");
               rv = SW_BAD_VALUE;
           }
           else
           {
               rv = cmd_data_check_confirm(cmd, A_TRUE, &entry.s_dei_enable,
                                           sizeof (a_bool_t));
               if (SW_OK != rv)
                   dprintf("usage: <yes/no/y/n>\n");
           }

       }
       while (talk_mode && (SW_OK != rv));

       do
       {
           cmd = get_sub_cmd("s_dei", "0");
			SW_RTN_ON_NULL_PARAM(cmd);

           if (!strncasecmp(cmd, "quit", 4))
           {
               return SW_BAD_VALUE;
           }
           else if (!strncasecmp(cmd, "help", 4))
           {
               dprintf("usage: the range is 0 -- 1\n");
               rv = SW_BAD_VALUE;
           }
           else
           {
               rv = cmd_data_check_uint8(cmd, &tmp, sizeof (a_uint32_t));
               if (SW_OK != rv)
               {
                   dprintf("usage: the range is 0 -- 1\n");
               }
               else
               {
                   entry.s_dei = tmp;
               }
           }
       }
       while (talk_mode && (SW_OK != rv));

       do
       {
           cmd = get_sub_cmd("c_dei_enable", "yes");
			SW_RTN_ON_NULL_PARAM(cmd);

           if (!strncasecmp(cmd, "quit", 4))
           {
               return SW_BAD_VALUE;
           }
           else if (!strncasecmp(cmd, "help", 4))
           {
               dprintf("usage: <yes/no/y/n>\n");
               rv = SW_BAD_VALUE;
           }
           else
           {
               rv = cmd_data_check_confirm(cmd, A_TRUE, &entry.c_dei_enable,
                                           sizeof (a_bool_t));
               if (SW_OK != rv)
                   dprintf("usage: <yes/no/y/n>\n");
           }

       }
       while (talk_mode && (SW_OK != rv));

       do
       {
           cmd = get_sub_cmd("c_dei", "0");
			SW_RTN_ON_NULL_PARAM(cmd);

           if (!strncasecmp(cmd, "quit", 4))
           {
               return SW_BAD_VALUE;
           }
           else if (!strncasecmp(cmd, "help", 4))
           {
               dprintf("usage: the range is 0 -- 1\n");
               rv = SW_BAD_VALUE;
           }
           else
           {
               rv = cmd_data_check_uint8(cmd, &tmp, sizeof (a_uint32_t));
               if (SW_OK != rv)
               {
                    dprintf("usage: the range is 0 -- 1\n");
               }
               else
               {
                    entry.c_dei = tmp;
               }
           }
       }
       while (talk_mode && (SW_OK != rv));

       do
       {
           cmd = get_sub_cmd("counter_enable", "yes");
			SW_RTN_ON_NULL_PARAM(cmd);

           if (!strncasecmp(cmd, "quit", 4))
           {
               return SW_BAD_VALUE;
           }
           else if (!strncasecmp(cmd, "help", 4))
           {
               dprintf("usage: <yes/no/y/n>\n");
               rv = SW_BAD_VALUE;
           }
           else
           {
               rv = cmd_data_check_confirm(cmd, A_TRUE, &entry.counter_enable,
                                           sizeof (a_bool_t));
               if (SW_OK != rv)
                   dprintf("usage: <yes/no/y/n>\n");
           }

       }
       while (talk_mode && (SW_OK != rv));

       do
       {
           cmd = get_sub_cmd("counter_id", "0");
			SW_RTN_ON_NULL_PARAM(cmd);

           if (!strncasecmp(cmd, "quit", 4))
           {
               return SW_BAD_VALUE;
           }
           else if (!strncasecmp(cmd, "help", 4))
           {
               dprintf("usage: the range is 0 -- 63\n");
               rv = SW_BAD_VALUE;
           }
           else
           {
               rv = cmd_data_check_uint8(cmd, &tmp, sizeof (a_uint32_t));
               if (SW_OK != rv)
               {
                   dprintf("usage: the range is 0 -- 53\n");
               }
               else
               {
                   entry.counter_id = tmp;
               }
           }
       }
       while (talk_mode && (SW_OK != rv));

    if (entry.trans_direction == 0) {
       do
       {
           cmd = get_sub_cmd("vsi_action_enable", "yes");
			SW_RTN_ON_NULL_PARAM(cmd);

           if (!strncasecmp(cmd, "quit", 4))
           {
               return SW_BAD_VALUE;
           }
           else if (!strncasecmp(cmd, "help", 4))
           {
               dprintf("usage: <yes/no/y/n>\n");
               rv = SW_BAD_VALUE;
           }
           else
           {
               rv = cmd_data_check_confirm(cmd, A_TRUE, &entry.vsi_action_enable,
                                           sizeof (a_bool_t));
               if (SW_OK != rv)
                   dprintf("usage: <yes/no/y/n>\n");
           }

       }
       while (talk_mode && (SW_OK != rv));

       do
       {
           cmd = get_sub_cmd("vsi_action", "0");
			SW_RTN_ON_NULL_PARAM(cmd);

           if (!strncasecmp(cmd, "quit", 4))
           {
               return SW_BAD_VALUE;
           }
           else if (!strncasecmp(cmd, "help", 4))
           {
               dprintf("usage: the range is 0 -- 32\n");
               rv = SW_BAD_VALUE;
           }
           else
           {
               rv = cmd_data_check_uint8(cmd, &tmp, sizeof (a_uint32_t));
               if (SW_OK != rv)
               {
                   dprintf("usage: the range is 0 -- 32\n");
               }
               else
               {
                   entry.vsi_action = tmp;
               }
           }
       }
       while (talk_mode && (SW_OK != rv));
    }

       do
       {
           cmd = get_sub_cmd("svid_xlt_cmd", "0");
			SW_RTN_ON_NULL_PARAM(cmd);

           if (!strncasecmp(cmd, "quit", 4))
           {
               return SW_BAD_VALUE;
           }
           else if (!strncasecmp(cmd, "help", 4))
           {
               dprintf("usage: 0 for unchanged, 1 for add and replace and 2 for delete tag\n");
               rv = SW_BAD_VALUE;
           }
           else
           {
               rv = cmd_data_check_uint32(cmd, &entry.svid_xlt_cmd, sizeof (a_uint32_t));
               if (SW_OK != rv)
               dprintf("usage: 0 for unchanged, 1 for add and replace and 2 for delete tag\n");
           }
       }
       while (talk_mode && (SW_OK != rv));

       do
       {
           cmd = get_sub_cmd("svid_xlt", "0");
			SW_RTN_ON_NULL_PARAM(cmd);

           if (!strncasecmp(cmd, "quit", 4))
           {
               return SW_BAD_VALUE;
           }
           else if (!strncasecmp(cmd, "help", 4))
           {
               dprintf("usage: the range is 0 -- 4095\n");
               rv = SW_BAD_VALUE;
           }
           else
           {
               rv = cmd_data_check_uint16(cmd, &tmp, sizeof (a_uint32_t));
               if (SW_OK != rv)
               {
                    dprintf("usage: the range is 0 -- 4095\n");
               }
               else
               {
                    entry.svid_xlt = tmp;
               }
           }
       }
       while (talk_mode && (SW_OK != rv));

       do
       {
           cmd = get_sub_cmd("cvid_xlt_cmd", "0");
			SW_RTN_ON_NULL_PARAM(cmd);

           if (!strncasecmp(cmd, "quit", 4))
           {
               return SW_BAD_VALUE;
           }
           else if (!strncasecmp(cmd, "help", 4))
           {
               dprintf("usage: 0 for unchanged, 1 for add and replace and 2 for delete tag\n");
               rv = SW_BAD_VALUE;
           }
           else
           {
               rv = cmd_data_check_uint32(cmd, &entry.cvid_xlt_cmd, sizeof (a_uint32_t));
               if (SW_OK != rv)
               dprintf("usage: 0 for unchanged, 1 for add and replace and 2 for delete tag\n");
           }
       }
       while (talk_mode && (SW_OK != rv));

       do
       {
           cmd = get_sub_cmd("cvid_xlt", "0");
			SW_RTN_ON_NULL_PARAM(cmd);

           if (!strncasecmp(cmd, "quit", 4))
           {
               return SW_BAD_VALUE;
           }
           else if (!strncasecmp(cmd, "help", 4))
           {
               dprintf("usage: the range is 0 -- 4095\n");
               rv = SW_BAD_VALUE;
           }
           else
           {
               rv = cmd_data_check_uint16(cmd, &tmp, sizeof (a_uint32_t));
               if (SW_OK != rv)
               {
                    dprintf("usage: the range is 0 -- 4095\n");
               }
               else
               {
                    entry.cvid_xlt = tmp;
               }
           }
       }
       while (talk_mode && (SW_OK != rv));

       do
       {
           cmd = get_sub_cmd("swap_svid_cvid", "yes");
			SW_RTN_ON_NULL_PARAM(cmd);

           if (!strncasecmp(cmd, "quit", 4))
           {
               return SW_BAD_VALUE;
           }
           else if (!strncasecmp(cmd, "help", 4))
           {
               dprintf("usage: <yes/no/y/n>\n");
               rv = SW_BAD_VALUE;
           }
           else
           {
               rv = cmd_data_check_confirm(cmd, A_TRUE, &entry.swap_svid_cvid,
                                           sizeof (a_bool_t));
               if (SW_OK != rv)
                   dprintf("usage: <yes/no/y/n>\n");
           }

       }
       while (talk_mode && (SW_OK != rv));

       do
       {
           cmd = get_sub_cmd("spcp_xlt_enable", "yes");
			SW_RTN_ON_NULL_PARAM(cmd);

           if (!strncasecmp(cmd, "quit", 4))
           {
               return SW_BAD_VALUE;
           }
           else if (!strncasecmp(cmd, "help", 4))
           {
               dprintf("usage: <yes/no/y/n>\n");
               rv = SW_BAD_VALUE;
           }
           else
           {
               rv = cmd_data_check_confirm(cmd, A_TRUE, &entry.spcp_xlt_enable,
                                           sizeof (a_bool_t));
               if (SW_OK != rv)
                   dprintf("usage: <yes/no/y/n>\n");
           }

       }
       while (talk_mode && (SW_OK != rv));

       do
       {
           cmd = get_sub_cmd("spcp_xlt", "0");
			SW_RTN_ON_NULL_PARAM(cmd);

           if (!strncasecmp(cmd, "quit", 4))
           {
               return SW_BAD_VALUE;
           }
           else if (!strncasecmp(cmd, "help", 4))
           {
               dprintf("usage: the range is 0 -- 7\n");
               rv = SW_BAD_VALUE;
           }
           else
           {
               rv = cmd_data_check_uint8(cmd, &tmp, sizeof (a_uint32_t));
               if (SW_OK != rv)
               {
                    dprintf("usage: the range is 0 -- 7\n");
               }
               else
               {
                    entry.spcp_xlt = tmp;
               }
           }
       }
       while (talk_mode && (SW_OK != rv));

       do
       {
           cmd = get_sub_cmd("cpcp_xlt_enable", "yes");
			SW_RTN_ON_NULL_PARAM(cmd);

           if (!strncasecmp(cmd, "quit", 4))
           {
               return SW_BAD_VALUE;
           }
           else if (!strncasecmp(cmd, "help", 4))
           {
               dprintf("usage: <yes/no/y/n>\n");
               rv = SW_BAD_VALUE;
           }
           else
           {
               rv = cmd_data_check_confirm(cmd, A_TRUE, &entry.cpcp_xlt_enable,
                                           sizeof (a_bool_t));
               if (SW_OK != rv)
                   dprintf("usage: <yes/no/y/n>\n");
           }

       }
       while (talk_mode && (SW_OK != rv));

       do
       {
           cmd = get_sub_cmd("cpcp_xlt", "0");
			SW_RTN_ON_NULL_PARAM(cmd);

           if (!strncasecmp(cmd, "quit", 4))
           {
               return SW_BAD_VALUE;
           }
           else if (!strncasecmp(cmd, "help", 4))
           {
               dprintf("usage: the range is 0 -- 7\n");
               rv = SW_BAD_VALUE;
           }
           else
           {
               rv = cmd_data_check_uint8(cmd, &tmp, sizeof (a_uint32_t));
               if (SW_OK != rv)
               {
                    dprintf("usage: the range is 0 -- 7\n");
               }
               else
               {
                    entry.cpcp_xlt = tmp;
               }
           }
       }
       while (talk_mode && (SW_OK != rv));

       do
       {
           cmd = get_sub_cmd("swap_spcp_cpcp", "yes");
			SW_RTN_ON_NULL_PARAM(cmd);

           if (!strncasecmp(cmd, "quit", 4))
           {
               return SW_BAD_VALUE;
           }
           else if (!strncasecmp(cmd, "help", 4))
           {
               dprintf("usage: <yes/no/y/n>\n");
               rv = SW_BAD_VALUE;
           }
           else
           {
               rv = cmd_data_check_confirm(cmd, A_TRUE, &entry.swap_spcp_cpcp,
                                           sizeof (a_bool_t));
               if (SW_OK != rv)
                   dprintf("usage: <yes/no/y/n>\n");
           }

       }
       while (talk_mode && (SW_OK != rv));

       do
       {
           cmd = get_sub_cmd("sdei_xlt_enable", "yes");
			SW_RTN_ON_NULL_PARAM(cmd);

           if (!strncasecmp(cmd, "quit", 4))
           {
               return SW_BAD_VALUE;
           }
           else if (!strncasecmp(cmd, "help", 4))
           {
               dprintf("usage: <yes/no/y/n>\n");
               rv = SW_BAD_VALUE;
           }
           else
           {
               rv = cmd_data_check_confirm(cmd, A_TRUE, &entry.sdei_xlt_enable,
                                           sizeof (a_bool_t));
               if (SW_OK != rv)
                   dprintf("usage: <yes/no/y/n>\n");
           }

       }
       while (talk_mode && (SW_OK != rv));

       do
       {
           cmd = get_sub_cmd("sdei_xlt", "0");
			SW_RTN_ON_NULL_PARAM(cmd);

           if (!strncasecmp(cmd, "quit", 4))
           {
               return SW_BAD_VALUE;
           }
           else if (!strncasecmp(cmd, "help", 4))
           {
               dprintf("usage: the range is 0 -- 1\n");
               rv = SW_BAD_VALUE;
           }
           else
           {
               rv = cmd_data_check_uint8(cmd, &tmp, sizeof (a_uint32_t));
               if (SW_OK != rv)
               {
                    dprintf("usage: the range is 0 -- 1\n");
               }
               else
               {
                    entry.sdei_xlt = tmp;
               }
           }
       }
       while (talk_mode && (SW_OK != rv));

       do
       {
           cmd = get_sub_cmd("cdei_xlt_enable", "yes");
			SW_RTN_ON_NULL_PARAM(cmd);

           if (!strncasecmp(cmd, "quit", 4))
           {
               return SW_BAD_VALUE;
           }
           else if (!strncasecmp(cmd, "help", 4))
           {
               dprintf("usage: <yes/no/y/n>\n");
               rv = SW_BAD_VALUE;
           }
           else
           {
               rv = cmd_data_check_confirm(cmd, A_TRUE, &entry.cdei_xlt_enable,
                                           sizeof (a_bool_t));
               if (SW_OK != rv)
                   dprintf("usage: <yes/no/y/n>\n");
           }

       }
       while (talk_mode && (SW_OK != rv));

       do
       {
           cmd = get_sub_cmd("cdei_xlt", "0");
			SW_RTN_ON_NULL_PARAM(cmd);

           if (!strncasecmp(cmd, "quit", 4))
           {
               return SW_BAD_VALUE;
           }
           else if (!strncasecmp(cmd, "help", 4))
           {
               dprintf("usage: the range is 0 -- 7\n");
               rv = SW_BAD_VALUE;
           }
           else
           {
               rv = cmd_data_check_uint8(cmd, &tmp, sizeof (a_uint32_t));
               if (SW_OK != rv)
               {
                    dprintf("usage: the range is 0 -- 7\n");
               }
               else
               {
                    entry.cdei_xlt = tmp;
               }
           }
       }
       while (talk_mode && (SW_OK != rv));

       do
       {
           cmd = get_sub_cmd("swap_sdei_cdei", "yes");
			SW_RTN_ON_NULL_PARAM(cmd);

           if (!strncasecmp(cmd, "quit", 4))
           {
               return SW_BAD_VALUE;
           }
           else if (!strncasecmp(cmd, "help", 4))
           {
               dprintf("usage: <yes/no/y/n>\n");
               rv = SW_BAD_VALUE;
           }
           else
           {
               rv = cmd_data_check_confirm(cmd, A_TRUE, &entry.swap_sdei_cdei,
                                           sizeof (a_bool_t));
               if (SW_OK != rv)
                   dprintf("usage: <yes/no/y/n>\n");
           }

       }
       while (talk_mode && (SW_OK != rv));


    }

    *val = entry;
    return SW_OK;
}

void
cmd_data_print_vlan_translation(a_uint8_t * param_name, a_uint32_t * buf, a_uint32_t size)
{
    fal_vlan_trans_entry_t *entry;

    entry = (fal_vlan_trans_entry_t *) buf;

    if (ssdk_cfg.init_cfg.chip_type != CHIP_HPPE) {
	    dprintf("[Ovid]:0x%x  [Svid]:0x%x  [Cvid]:0x%x  [BiDirect]:%s  [ForwardDirect]:%s  [ReverseDirect]:%s",
		    entry->o_vid, entry->s_vid, entry->c_vid,
		    entry->bi_dir?"ENABLE":"DISABLE",
		    entry->forward_dir?"ENABLE":"DISABLE",
		    entry->reverse_dir?"ENABLE":"DISABLE");

	    dprintf("\n[OvidIsCvid]:%s  [SvidEnable]:%s  [CvidEnable]:%s  [One2OneVlan]:%s\n",
	            entry->o_vid_is_cvid?"YES":"NO",
		    entry->s_vid_enable?"YES":"NO",
		    entry->c_vid_enable?"YES":"NO",
		    entry->one_2_one_vlan?"YES":"NO");
    }

    if (ssdk_cfg.init_cfg.chip_type == CHIP_HPPE) {
	    dprintf("\n\n rule field: ");
	    dprintf("\n[TranslateDirect]:%d", entry->trans_direction);
	    dprintf("\n[port_bitmap]:0x%x",
		    entry->port_bitmap);
	    dprintf("\n[SvidEnable]:%s  [Svid]:0x%x  [CvidEnable]:%s  [Cvid]:0x%x",
		    entry->s_vid_enable?"YES":"NO", entry->s_vid,
		    entry->c_vid_enable?"YES":"NO", entry->c_vid);
	    if (entry->trans_direction == 1) {
		    dprintf("\n[vsi_valid]:%s  [vsi_enable]:%s  [vsi]:%d  ",
		            entry->vsi_valid?"ENABLE":"DISABLE",
		            entry->vsi_enable?"ENABLE":"DISABLE",
		            entry->vsi);
	    }
	    if (entry->trans_direction == 0) {
		    dprintf("\n[protocol_enable]:%s  [protocol]:0x%x  [frmtype_enable]:%s  [frmtype]:%d  ",
		            entry->protocol_enable?"ENABLE":"DISABLE",
		            entry->protocol,
		            entry->frmtype_enable?"ENABLE":"DISABLE",
		            entry->frmtype);
	    }
	    dprintf("\n[stag_format]:0x%x  [ctag_format]:0x%x",
	            entry->s_tagged,
	            entry->c_tagged);
	    dprintf("\n[s_pcp_enable]:%s  [s_pcp]:%d  [c_pcp_enable]:%s  [c_pcp]:%d",
		    entry->s_pcp_enable?"ENABLE":"DISABLE", entry->s_pcp,
		    entry->c_pcp_enable?"ENABLE":"DISABLE", entry->c_pcp);
	    dprintf("\n[s_dei_enable]:%s  [s_dei]:%d  [c_dei_enable]:%s  [c_dei]:%d",
		    entry->s_dei_enable?"ENABLE":"DISABLE", entry->s_dei,
		    entry->c_dei_enable?"ENABLE":"DISABLE", entry->c_dei);
	    dprintf("\n\n action field: ");
	    dprintf("\n[counter_enable]:%s  [counter_id]:%d",
	            entry->counter_enable?"ENABLE":"DISABLE",
	            entry->counter_id);
	    if (entry->trans_direction == 0) {
		    dprintf("\n[vsi_action_enable]:%s  [vsi_action]:%d",
			    entry->vsi_action_enable?"ENABLE":"DISABLE",
			    entry->vsi_action);
	    }
	    dprintf("\n[svid_xlt_cmd]:%d  [svid_xlt]:%d  [cvid_xlt_cmd]:%d  [cvid_xlt]:%d  ",
	            entry->svid_xlt_cmd,
	            entry->svid_xlt,
	            entry->cvid_xlt_cmd,
	            entry->cvid_xlt);
	    dprintf("\n[swap_svid_cvid]:%s  ",
	            entry->swap_svid_cvid?"ENABLE":"DISABLE");
	    dprintf("\n[spcp_xlt_enable]:%s  [spcp_xlt]:%d  [cpcp_xlt_enable]:%s  [cpcp_xlt]:%d  ",
	            entry->spcp_xlt_enable?"ENABLE":"DISABLE",
	            entry->spcp_xlt,
	            entry->cpcp_xlt_enable?"ENABLE":"DISABLE",
	            entry->cpcp_xlt);
	    dprintf("\n[swap_spcp_cpcp]:%s  ",
	            entry->swap_spcp_cpcp?"ENABLE":"DISABLE");
	    dprintf("\n[sdei_xlt_enable]:%s  [sdei_xlt]:%d  [cdei_xlt_enable]:%s  [cdei_xlt]:%d  ",
	            entry->sdei_xlt_enable?"ENABLE":"DISABLE",
	            entry->sdei_xlt,
	            entry->cdei_xlt_enable?"ENABLE":"DISABLE",
	            entry->cdei_xlt);
	    dprintf("\n[swap_sdei_cdei]:%s\n",
	            entry->swap_sdei_cdei?"ENABLE":"DISABLE");
    }
}

sw_error_t
cmd_data_check_qinq_mode(char *cmd_str, a_uint32_t * arg_val, a_uint32_t size)
{
    if (cmd_str == NULL)
        return SW_BAD_PARAM;

    if (!strcasecmp(cmd_str, "ctag"))
    {
        *arg_val = FAL_QINQ_CTAG_MODE;
    }
    else if (!strcasecmp(cmd_str, "stag"))
    {
        *arg_val = FAL_QINQ_STAG_MODE;
    }
    else
    {
        //dprintf("input error \n");
        return SW_BAD_VALUE;
    }

    return SW_OK;
}

void
cmd_data_print_qinq_mode(a_char_t * param_name, a_uint32_t * buf, a_uint32_t size)
{
    dprintf("[%s]:", param_name);

    if (*(a_uint32_t *) buf == FAL_QINQ_CTAG_MODE)
    {
        dprintf("CTAG");
    }
    else if (*(a_uint32_t *) buf == FAL_QINQ_STAG_MODE)
    {
        dprintf("STAG");
    }
    else
    {
        dprintf("UNKNOWN VALUE");
    }
}

sw_error_t
cmd_data_check_qinq_role(char *cmd_str, a_uint32_t * arg_val, a_uint32_t size)
{
    if (cmd_str == NULL)
        return SW_BAD_PARAM;

    if (!strcasecmp(cmd_str, "edge"))
    {
        *arg_val = FAL_QINQ_EDGE_PORT;
    }
    else if (!strcasecmp(cmd_str, "core"))
    {
        *arg_val = FAL_QINQ_CORE_PORT;
    }
    else
    {
        //dprintf("input error \n");
        return SW_BAD_VALUE;
    }

    return SW_OK;
}

void
cmd_data_print_qinq_role(a_char_t * param_name, a_uint32_t * buf, a_uint32_t size)
{
    dprintf("[%s]:", param_name);

    if (*(a_uint32_t *) buf == FAL_QINQ_EDGE_PORT)
    {
        dprintf("EDGE");
    }
    else if (*(a_uint32_t *) buf == FAL_QINQ_CORE_PORT)
    {
        dprintf("CORE");
    }
    else
    {
        dprintf("UNKNOWN VALUE");
    }
}
sw_error_t
cmd_data_check_port_eee_config(char *cmd_str, void * val, a_uint32_t size)
{
    char *cmd;
    sw_error_t rv;
    fal_port_eee_cfg_t cfg;

    aos_mem_zero(&cfg, sizeof (fal_port_eee_cfg_t));

    do
    {
        cmd = get_sub_cmd("eee_enable", "yes");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {

            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: <yes/no/y/n>\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_confirm(cmd, A_FALSE, &(cfg.enable),
                                        sizeof (a_bool_t));
            if (SW_OK != rv)
                dprintf("usage: <yes/no/y/n>\n");
        }
    }
    while (talk_mode && (SW_OK != rv));
    do
    {
        cmd = get_sub_cmd("eee_capability", "0-0xffff");
		SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: integer\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_uint32(cmd, &(cfg.capability), sizeof (a_uint32_t));
            if (SW_OK != rv)
                dprintf("usage: integer\n");
        }
    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("lpi_sleep_timer", "0-0xffff");
		SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: integer\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_uint32(cmd, &(cfg.lpi_sleep_timer), sizeof (a_uint32_t));
            if (SW_OK != rv)
                dprintf("usage: integer\n");
        }
    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("advertisement", "0-0xffff");
		SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: integer\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_uint32(cmd, &(cfg.advertisement), sizeof (a_uint32_t));
            if (SW_OK != rv)
                dprintf("usage: integer\n");
        }
    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("lpi_tx_enable", "0x1");
		SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: integer\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_uint32(cmd, &(cfg.lpi_tx_enable), sizeof (a_uint32_t));
            if (SW_OK != rv)
                dprintf("usage: integer\n");
        }
    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("eee_status", "0-0xffff");
		SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: integer\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_uint32(cmd, &(cfg.eee_status), sizeof (a_uint32_t));
            if (SW_OK != rv)
                dprintf("usage: integer\n");
        }
    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("lpi_wakeup_timer", "0-0xffff");
		SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: integer\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_uint32(cmd, &(cfg.lpi_wakeup_timer), sizeof (a_uint32_t));
            if (SW_OK != rv)
                dprintf("usage: integer\n");
        }
    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("link_partner_advertisement", "0-0xffff");
		SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: integer\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_uint32(cmd, &(cfg.link_partner_advertisement), sizeof (a_uint32_t));
            if (SW_OK != rv)
                dprintf("usage: integer\n");
        }
    }
    while (talk_mode && (SW_OK != rv));

    *(fal_port_eee_cfg_t *)val = cfg;
    return SW_OK;
}
void
cmd_data_print_port_eee_config(a_uint8_t * param_name, a_uint32_t * buf, a_uint32_t size)
{
    fal_port_eee_cfg_t *cfg;

    cfg = (fal_port_eee_cfg_t *) buf;

    if (A_TRUE == cfg->enable)
    {
        dprintf("\n[eee_enable]:yes  ");
    }
    else
    {
        dprintf("\n[eee_enable]:no  ");
    }
    dprintf("\n[eee_capability]:0x%x", cfg->capability);
    dprintf("\n[eee_lpi_sleep_timer]:0x%x", cfg->lpi_sleep_timer);
    dprintf("\n[eee_advertisement]:0x%x", cfg->advertisement);
    dprintf("\n[eee_lpi_tx_enable]:0x%x", cfg->lpi_tx_enable);
    dprintf("\n[eee_status]:0x%x  ", cfg->eee_status);
    dprintf("\n[eee_lpi_wakeup_timer]:0x%x  ", cfg->lpi_wakeup_timer);
    dprintf("\n[eee_link_partner_advertisement]:0x%x  ", cfg->link_partner_advertisement);

    return;
}

sw_error_t
cmd_data_check_switch_port_loopback_config(char *cmd_str, void * val,
	a_uint32_t size)
{
    char *cmd;
    sw_error_t rv;
    fal_loopback_config_t cfg;

    aos_mem_zero(&cfg, sizeof (fal_loopback_config_t));

    do
    {
        cmd = get_sub_cmd("loopback_enable", "no");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {

            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: <yes/no/y/n>\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_confirm(cmd, A_FALSE, &(cfg.enable),
                                        sizeof (a_bool_t));
            if (SW_OK != rv)
                dprintf("usage: <yes/no/y/n>\n");
        }
    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("crc_stripped_enable", "no");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {

            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: <yes/no/y/n>\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_confirm(cmd, A_FALSE, &(cfg.crc_stripped),
                                        sizeof (a_bool_t));
            if (SW_OK != rv)
                dprintf("usage: <yes/no/y/n>\n");
        }
    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("loopback_rate", "1-0x12c");
		SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: integer [unit is Mpps]\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_uint32(cmd, &(cfg.loopback_rate), sizeof (a_uint32_t));
            if (SW_OK != rv)
                dprintf("usage: integer [unit is Mpps]\n");
        }
    }
    while (talk_mode && (SW_OK != rv));

    *(fal_loopback_config_t *)val = cfg;
    return SW_OK;
}
void
cmd_data_print_switch_port_loopback_config(a_uint8_t * param_name,
	a_uint32_t * buf, a_uint32_t size)
{
    fal_loopback_config_t *cfg;

    cfg = (fal_loopback_config_t *) buf;

    if (A_TRUE == cfg->enable)
    {
        dprintf("\n[loopback_enable]:yes  ");
    }
    else
    {
        dprintf("\n[loopback_enable]:no  ");
    }
    if (A_TRUE == cfg->crc_stripped)
    {
        dprintf("\n[crc_stripped_enable]:yes  ");
    }
    else
    {
        dprintf("\n[crc_stripped_enable]:no  ");
    }

    dprintf("\n[loopback_rate]:%d[Mpps]", cfg->loopback_rate);

    return;
}

/*qca808x_start*/
void
cmd_data_print_cable_status(a_uint8_t * param_name, a_uint32_t * buf, a_uint32_t size)
{
    dprintf("[%s]:", param_name);

    if (*(a_uint32_t *) buf == FAL_CABLE_STATUS_NORMAL)
    {
        dprintf("NORMAL");
    }
    else if (*(a_uint32_t *) buf == FAL_CABLE_STATUS_SHORT)
    {
        dprintf("SHORT");
    }
    else if (*(a_uint32_t *) buf == FAL_CABLE_STATUS_OPENED)
    {
        dprintf("OPENED");
    }
    else if (*(a_uint32_t *) buf == FAL_CABLE_STATUS_INVALID)
    {
        dprintf("INVALID");
    }
    else if (*(a_uint32_t *) buf == FAL_CABLE_STATUS_CROSSOVERA)
    {
        dprintf("CROSSOVERA");
    }
    else if (*(a_uint32_t *) buf == FAL_CABLE_STATUS_CROSSOVERB)
    {
        dprintf("CROSSOVERB");
    }
    else if (*(a_uint32_t *) buf == FAL_CABLE_STATUS_CROSSOVERC)
    {
        dprintf("CROSSOVERC");
    }
    else if (*(a_uint32_t *) buf == FAL_CABLE_STATUS_CROSSOVERD)
    {
        dprintf("CROSSOVERD");
    }
    else if (*(a_uint32_t *) buf == FAL_CABLE_STATUS_LOW_MISMATCH)
    {
        dprintf("LOW_MISMATCH");
    }
    else if (*(a_uint32_t *) buf == FAL_CABLE_STATUS_HIGH_MISMATCH)
    {
        dprintf("HIGH_MISMATCH");
    }
    else
    {
        dprintf("UNKNOWN VALUE");
    }
}

void
cmd_data_print_cable_len(a_uint8_t * param_name, a_uint32_t * buf, a_uint32_t size)
{
    dprintf("[%s]:%d", param_name, *(a_uint32_t *) buf);
}

char*
cmd_cpu_mode(hsl_init_mode mode)
{
    switch (mode)
    {
        case HSL_NO_CPU:
            return "no_cpu";
        case HSL_CPU_1:
            return "cpu_1";
        case HSL_CPU_2:
            return "cpu_2";
        case HSL_CPU_1_PLUS:
            return "cpu_1_plus";
    }

    return "unknow";
}

char*
cmd_access_mode(hsl_access_mode mode)
{
    switch (mode)
    {
        case HSL_MDIO:
            return "mdio";
        case HSL_HEADER:
            return "header";
    }

    return "unknow";
}

static void
_cmd_collect_shell_cfg(ssdk_cfg_t *shell_cfg)
{
    memset(shell_cfg, 0, sizeof(ssdk_cfg_t));
    shell_cfg->init_cfg = init_cfg;

#ifdef VERSION
    aos_mem_copy(shell_cfg->build_ver, VERSION, sizeof(VERSION));
#endif

#ifdef BUILD_DATE
    aos_mem_copy(shell_cfg->build_date, BUILD_DATE, sizeof(BUILD_DATE));
#endif

    if (ssdk_cfg.init_cfg.chip_type == CHIP_ATHENA)
        aos_mem_copy(shell_cfg->chip_type, "athena", sizeof("athena"));
    else if (ssdk_cfg.init_cfg.chip_type == CHIP_GARUDA)
        aos_mem_copy(shell_cfg->chip_type, "garuda", sizeof("garuda"));
    else if (ssdk_cfg.init_cfg.chip_type == CHIP_SHIVA)
        aos_mem_copy(shell_cfg->chip_type, "shiva", sizeof("shiva"));
    else if (ssdk_cfg.init_cfg.chip_type == CHIP_HORUS)
        aos_mem_copy(shell_cfg->chip_type, "horus", sizeof("horus"));
    else if (ssdk_cfg.init_cfg.chip_type == CHIP_ISIS)
        aos_mem_copy(shell_cfg->chip_type, "isis", sizeof("isis"));
    else if (ssdk_cfg.init_cfg.chip_type == CHIP_ISISC)
        aos_mem_copy(shell_cfg->chip_type, "isisc", sizeof("isisc"));

#ifdef CPU
    aos_mem_copy(shell_cfg->cpu_type, CPU, sizeof(CPU));
#endif

#ifdef OS
    aos_mem_copy(shell_cfg->os_info, OS, sizeof(OS));
#if defined KVER26
    aos_mem_copy(shell_cfg->os_info+sizeof(OS)-1, " version 2.6", sizeof(" version 2.6"));
#elif defined KVER24
    aos_mem_copy(shell_cfg->os_info+sizeof(OS)-1, " version 2.4", sizeof(" version 2.4"));
#else
    aos_mem_copy(shell_cfg->os_info+sizeof(OS)-1, " version unknown", sizeof(" version unknown"));
#endif
#endif

#ifdef HSL_STANDALONG
    shell_cfg->fal_mod = A_FALSE;
#else
    shell_cfg->fal_mod = A_TRUE;
#endif

#ifdef USER_MODE
    shell_cfg->kernel_mode = A_FALSE;
#else
    shell_cfg->kernel_mode = A_TRUE;
#endif

#ifdef UK_IF
    shell_cfg->uk_if = A_TRUE;
#else
    shell_cfg->uk_if = A_FALSE;
#endif

    return;
}

#define BOOL2STR(val_bool) (((val_bool)==A_TRUE)?"true":"false" )
/*qca808x_end*/
#define BOOL2NAME(val_bool) (((feature->in_##val_bool)==A_TRUE)?(#val_bool):"" )
#define DEFINED2STR(name) (((init->reg_func.name))?"y":"n" )
/*qca808x_start*/
static void
_cmd_data_print_cfg(ssdk_cfg_t *entry)
{
    ssdk_init_cfg *init = &(entry->init_cfg);

    dprintf("[build verison]:%-10s [build date]:%s\n", entry->build_ver, entry->build_date);
    dprintf("[chip type]:%-14s [arch]:%-12s [os]:%s\n", entry->chip_type, entry->cpu_type, entry->os_info);
    dprintf("[fal]:%-20s [kernel mode]:%-5s [uk if]:%s\n",
            BOOL2STR(entry->fal_mod), BOOL2STR(entry->kernel_mode), BOOL2STR(entry->uk_if));

    dprintf("[cpu mode]:%-15s [reg access]:%-6s [ioctl minor]:%d\n",
            cmd_cpu_mode(init->cpu_mode), cmd_access_mode(init->reg_mode),
            init->nl_prot);
/*qca808x_end*/
    dprintf("[inf defined]:mdio_set(%s) mdio_get(%s) header_reg_set(%s) header_reg_get(%s)\n",
            DEFINED2STR(mdio_set), DEFINED2STR(mdio_get), DEFINED2STR(header_reg_set), DEFINED2STR(header_reg_get));
/*qca808x_start*/
}

void
cmd_data_print_ssdk_cfg(a_uint8_t * param_name, a_uint32_t * buf, a_uint32_t size)
{
    ssdk_cfg_t *ssdk_cfg = (ssdk_cfg_t *) buf;
    dprintf("1.SSDK CONFIGURATION:\n");
    _cmd_data_print_cfg(ssdk_cfg);

    dprintf("\n2.DEMO SHELL CONFIGURATION:\n");
    ssdk_cfg_t shell_cfg;
    _cmd_collect_shell_cfg(&shell_cfg);
    _cmd_data_print_cfg(&shell_cfg);

    dprintf("\n3.SSDK FEATURES LIST:\n");
/*qca808x_end*/
    ssdk_features *feature = &(ssdk_cfg->features);
    dprintf("%s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s\n",
            BOOL2NAME(acl), BOOL2NAME(fdb), BOOL2NAME(igmp), BOOL2NAME(leaky),
            BOOL2NAME(led), BOOL2NAME(mib), BOOL2NAME(mirror), BOOL2NAME(misc),
            BOOL2NAME(portcontrol), BOOL2NAME(portvlan), BOOL2NAME(qos), BOOL2NAME(rate),
            BOOL2NAME(stp), BOOL2NAME(vlan), BOOL2NAME(reduced_acl),
            BOOL2NAME(cosmap), BOOL2NAME(ip), BOOL2NAME(nat), BOOL2NAME(sec), BOOL2NAME(trunk), BOOL2NAME(interfacectrl));
/*qca808x_start*/

}
/*qca808x_end*/
sw_error_t
cmd_data_check_hdrmode(char *cmd_str, a_uint32_t * arg_val, a_uint32_t size)
{
    if (cmd_str == NULL)
        return SW_BAD_PARAM;

    if (!strcasecmp(cmd_str, "noheader"))
    {
        *arg_val = FAL_NO_HEADER_EN;
    }
    else if (!strcasecmp(cmd_str, "onlymanagement"))
    {
        *arg_val = FAL_ONLY_MANAGE_FRAME_EN;
    }
    else if (!strcasecmp(cmd_str, "allframe"))
    {
        *arg_val = FAL_ALL_TYPE_FRAME_EN;
    }
    else
    {
        //dprintf("input error \n");
        return SW_BAD_VALUE;
    }

    return SW_OK;
}

void
cmd_data_print_hdrmode(a_uint8_t * param_name, a_uint32_t * buf, a_uint32_t size)
{
    dprintf("[%s]:", param_name);
    if (*(a_uint32_t *) buf == FAL_NO_HEADER_EN)
    {
        dprintf("NOHEADER");
    }
    else if (*(a_uint32_t *) buf == FAL_ONLY_MANAGE_FRAME_EN)
    {
        dprintf("ONLYMANAGEMENT");
    }
    else if (*(a_uint32_t *) buf == FAL_ALL_TYPE_FRAME_EN)
    {
        dprintf("ALLFRAME");
    }
    else
    {
        dprintf("UNKNOWN VALUE");
    }
}

sw_error_t
cmd_data_check_fdboperation(char *cmd_str, void * val, a_uint32_t size)
{
    char *cmd;
    sw_error_t rv;
    fal_fdb_op_t entry;

    memset(&entry, 0, sizeof (fal_fdb_op_t));

    do
    {
        cmd = get_sub_cmd("port_en", "no");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: <yes/no/y/n>\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_confirm(cmd, A_FALSE, &entry.port_en,
                                        sizeof (a_bool_t));
            if (SW_OK != rv)
                dprintf("usage: <yes/no/y/n>\n");
        }

    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("fid_en", "no");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: <yes/no/y/n>\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_confirm(cmd, A_FALSE, &entry.fid_en,
                                        sizeof (a_bool_t));
            if (SW_OK != rv)
                dprintf("usage: <yes/no/y/n>\n");
        }

    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("multi_en", "no");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: <yes/no/y/n>\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_confirm(cmd, A_FALSE, &entry.multicast_en,
                                        sizeof (a_bool_t));
            if (SW_OK != rv)
                dprintf("usage: <yes/no/y/n>\n");
        }

    }
    while (talk_mode && (SW_OK != rv));

    *(fal_fdb_op_t *) val = entry;
    return SW_OK;
}

sw_error_t
cmd_data_check_pppoe(char *cmd_str, void * val, a_uint32_t size)
{
    char *cmd;
    sw_error_t rv;
    fal_pppoe_session_t entry;

    aos_mem_zero(&entry, sizeof (fal_pppoe_session_t));

    do
    {
        cmd = get_sub_cmd("entryid", "0");
		SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: integer\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_uint32(cmd, &(entry.entry_id), sizeof (a_uint32_t));
            if (SW_OK != rv)
                dprintf("usage: integer\n");
        }
    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("sessionid", "0");
		SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: the range is 0 -- 65535\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_uint32(cmd, &entry.session_id, sizeof (a_uint32_t));
            if (SW_OK != rv)
                dprintf("usage: the range is 0 -- 65535\n");
        }
    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("multi_session", "no");
		SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: <yes/no/y/n>\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_confirm(cmd, A_FALSE, &entry.multi_session,
                                        sizeof (a_bool_t));
            if (SW_OK != rv)
                dprintf("usage: <yes/no/y/n>\n");
        }

    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("uni_session", "no");
		SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: <yes/no/y/n>\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_confirm(cmd, A_FALSE, &entry.uni_session,
                                        sizeof (a_bool_t));
            if (SW_OK != rv)
                dprintf("usage: <yes/no/y/n>\n");
        }

    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("vrf_id", "0");
		SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: the range is 0 -- 7\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_uint32(cmd, &entry.vrf_id, sizeof (a_uint32_t));
            if (SW_OK != rv)
                dprintf("usage: the range is 0 -- 7\n");
        }
    }
    while (talk_mode && (SW_OK != rv));

    if (ssdk_cfg.init_cfg.chip_type == CHIP_HPPE)
    {
        do
        {
            cmd = get_sub_cmd("port", "null");
            SW_RTN_ON_NULL_PARAM(cmd);

            if (!strncasecmp(cmd, "quit", 4))
            {
                return SW_BAD_VALUE;
            }
            else if (!strncasecmp(cmd, "help", 4))
            {
                dprintf("usage: input port number such as 1,3\n");
                rv = SW_BAD_VALUE;
            }
            else
            {
                rv = cmd_data_check_portmap(cmd, &entry.port_bitmap, sizeof (a_uint32_t));
                if (SW_OK != rv)
                    dprintf("usage: input port number such as 1,3\n");
            }
        }
        while (talk_mode && (SW_OK != rv));

        do
        {
            cmd = get_sub_cmd("l3if_index", "0");
            SW_RTN_ON_NULL_PARAM(cmd);

            if (!strncasecmp(cmd, "quit", 4))
            {
                return SW_BAD_VALUE;
            }
            else if (!strncasecmp(cmd, "help", 4))
            {
                dprintf("usage: the range is 0 -- 255\n");
                rv = SW_BAD_VALUE;
            }
            else
            {
                rv = cmd_data_check_uint32(cmd, &entry.l3_if_index, sizeof (a_uint32_t));
                if (SW_OK != rv)
                    dprintf("usage: the range is 0 -- 255\n");
            }
        }
        while (talk_mode && (SW_OK != rv));

        do
        {
            cmd = get_sub_cmd("l3if_index_valid", "no");
            SW_RTN_ON_NULL_PARAM(cmd);

            if (!strncasecmp(cmd, "quit", 4))
            {
                return SW_BAD_VALUE;
            }
            else if (!strncasecmp(cmd, "help", 4))
            {
                dprintf("usage: <yes/no/y/n>\n");
                rv = SW_BAD_VALUE;
            }
            else
            {
                rv = cmd_data_check_confirm(cmd, A_FALSE, &entry.l3_if_valid,
                                            sizeof (a_bool_t));
                if (SW_OK != rv)
                    dprintf("usage: <yes/no/y/n>\n");
            }

        }
        while (talk_mode && (SW_OK != rv));

        do
        {
            cmd = get_sub_cmd("smacaddr", NULL);
            SW_RTN_ON_NULL_PARAM(cmd);

            if (!strncasecmp(cmd, "quit", 4))
            {
                return SW_BAD_VALUE;
            }
            else if (!strncasecmp(cmd, "help", 4))
            {
                dprintf("usage: the format is xx-xx-xx-xx-xx-xx \n");
                rv = SW_BAD_VALUE;
            }
            else
            {
                rv = cmd_data_check_macaddr(cmd, &entry.smac_addr,
                                            sizeof (fal_mac_addr_t));
                if (SW_OK != rv)
                    dprintf("usage: the format is xx-xx-xx-xx-xx-xx \n");
            }
        }
        while (talk_mode && (SW_OK != rv));

        do
        {
            cmd = get_sub_cmd("smacaddr_valid", "no");
            SW_RTN_ON_NULL_PARAM(cmd);

            if (!strncasecmp(cmd, "quit", 4))
            {
                return SW_BAD_VALUE;
            }
            else if (!strncasecmp(cmd, "help", 4))
            {
                dprintf("usage: <yes/no/y/n>\n");
                rv = SW_BAD_VALUE;
            }
            else
            {
                rv = cmd_data_check_confirm(cmd, A_FALSE, &entry.smac_valid,
                                            sizeof (a_bool_t));
                if (SW_OK != rv)
                    dprintf("usage: <yes/no/y/n>\n");
            }

        }
        while (talk_mode && (SW_OK != rv));
    }

    *(fal_pppoe_session_t*)val = entry;
    return SW_OK;
}

void
cmd_data_print_pppoe(a_uint8_t * param_name, a_uint32_t * buf, a_uint32_t size)
{
    fal_pppoe_session_t *entry;

    entry = (fal_pppoe_session_t *) buf;
    dprintf("[EntryID]:0x%x  [SessionID]:0x%x  [MultiSession]:%s  [UniSession]:%s  [Vrf_ID]:0x%x\n",
            entry->entry_id,
            entry->session_id,
            entry->multi_session ? "YES":"NO",
            entry->uni_session ?   "YES":"NO",
            entry->vrf_id);
    cmd_data_print_portmap("[Port]:", entry->port_bitmap, sizeof (fal_pbmp_t));
    dprintf("  [l3if_index]:0x%x  [l3if_index_valid]:%s\n",
            entry->l3_if_index, entry->l3_if_valid ? "YES":"NO");
    cmd_data_print_macaddr("[smacaddr]:", (a_uint32_t *) & (entry->smac_addr), sizeof (fal_mac_addr_t));
    dprintf("  [smacaddr_valid]:%s", entry->smac_valid ? "YES":"NO");
}

sw_error_t
cmd_data_check_pppoe_less(char *cmd_str, void * val, a_uint32_t size)
{
    char *cmd;
    sw_error_t rv;
    fal_pppoe_session_t entry;

    aos_mem_zero(&entry, sizeof (fal_pppoe_session_t));

    do
    {
        cmd = get_sub_cmd("sessionid", "0");
		SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: the range is 0 -- 65535\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_uint32(cmd, &entry.session_id, sizeof (a_uint32_t));
            if (SW_OK != rv)
                dprintf("usage: the range is 0 -- 65535\n");
        }
    }
    while (talk_mode && (SW_OK != rv));

    *(fal_pppoe_session_t*)val = entry;
    return SW_OK;
}

sw_error_t
cmd_data_check_host_entry(char *cmd_str, void * val, a_uint32_t size)
{
    char *cmd;
    a_uint32_t tmp;
    sw_error_t rv;
    fal_host_entry_t entry;

    aos_mem_zero(&entry, sizeof (fal_host_entry_t));

    do
    {
        cmd = get_sub_cmd("entryid", "0");
		SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: integer\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_uint32(cmd, &(entry.entry_id), sizeof (a_uint32_t));
            if (SW_OK != rv)
                dprintf("usage: integer\n");
        }
    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("entryflags", "0x1");
		SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: bitmap for host entry\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_uint32(cmd, &(entry.flags), sizeof (a_uint32_t));
            if (SW_OK != rv)
                dprintf("usage: bitmap for host entry\n");
        }
    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("entrystatus", "0");
		SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: integer\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_uint32(cmd, &(entry.status), sizeof (a_uint32_t));
            if (SW_OK != rv)
                dprintf("usage: integer\n");
        }
    }
    while (talk_mode && (SW_OK != rv));

    if ((FAL_IP_IP4_ADDR & (entry.flags)) == FAL_IP_IP4_ADDR ||
		(FAL_IP_IP4_ADDR_MCAST& (entry.flags)) == FAL_IP_IP4_ADDR_MCAST)
    {
        cmd_data_check_element("ip4 addr", NULL,
                               "usage: the format is xx.xx.xx.xx \n",
                               cmd_data_check_ip4addr, (cmd, &(entry.ip4_addr), 4));
    }
    else if ((FAL_IP_IP6_ADDR & (entry.flags)) == FAL_IP_IP6_ADDR ||
		(FAL_IP_IP6_ADDR_MCAST& (entry.flags)) == FAL_IP_IP6_ADDR_MCAST)
    {
        cmd_data_check_element("ip6 addr", NULL,
                               "usage: the format is xxxx::xxxx \n",
                               cmd_data_check_ip6addr, (cmd, &(entry.ip6_addr), 16));
    }

    cmd_data_check_element("mac addr", NULL,
                           "usage: the format is xx-xx-xx-xx-xx-xx \n",
                           cmd_data_check_macaddr, (cmd,
                                   &(entry.mac_addr),
                                   sizeof (fal_mac_addr_t)));

    do
    {
        cmd = get_sub_cmd("interface id", "0");
		SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: integer\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_uint32(cmd, &(entry.intf_id), sizeof (a_uint32_t));
            if (SW_OK != rv)
                dprintf("usage: integer\n");
        }
    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("load_balance num", "0");
		SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: integer\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_uint32(cmd, &(entry.lb_num), sizeof (a_uint32_t));
            if (SW_OK != rv)
                dprintf("usage: integer\n");
        }
    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("vrf id", "0");
		SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: integer\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_uint32(cmd, &(entry.vrf_id), sizeof (a_uint32_t));
            if (SW_OK != rv)
                dprintf("usage: integer\n");
        }
    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("port id", "0");
		SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: integer\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_uint32(cmd, &(entry.port_id), sizeof (a_uint32_t));
            if (SW_OK != rv)
                dprintf("usage: integer\n");
        }
    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("action", "forward");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: <forward/drop/cpycpu/rdtcpu>\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_maccmd(cmd, &(entry.action),
                                       sizeof (fal_fwd_cmd_t));
            if (SW_OK != rv)
                dprintf("usage: <forward/drop/cpycpu/rdtcpu>\n");
        }

    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("mirror", "no");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {

            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: <yes/no/y/n>\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_confirm(cmd, A_FALSE, &entry.mirror_en,
                                        sizeof (a_bool_t));
            if (SW_OK != rv)
                dprintf("usage: <yes/no/y/n>\n");
        }

    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("counter", "no");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {

            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: <yes/no/y/n>\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_confirm(cmd, A_FALSE, &entry.counter_en,
                                        sizeof (a_bool_t));
            if (SW_OK != rv)
                dprintf("usage: <yes/no/y/n>\n");
        }

    }
    while (talk_mode && (SW_OK != rv));

    if (A_TRUE == entry.counter_en)
    {
        do
        {
            cmd = get_sub_cmd("counter id", "0");
			SW_RTN_ON_NULL_PARAM(cmd);

            if (!strncasecmp(cmd, "quit", 4))
            {
                return SW_BAD_VALUE;
            }
            else if (!strncasecmp(cmd, "help", 4))
            {
                dprintf("usage: integer\n");
                rv = SW_BAD_VALUE;
            }
            else
            {
                rv = cmd_data_check_uint32(cmd, &(entry.counter_id), sizeof (a_uint32_t));
                if (SW_OK != rv)
                    dprintf("usage: integer\n");
            }
        }
        while (talk_mode && (SW_OK != rv));
    }

    do
    {
        cmd = get_sub_cmd("dst info", "0");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {

            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: dst info\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
           rv = cmd_data_check_uint32(cmd, &(entry.dst_info), sizeof (a_uint32_t));
            if (SW_OK != rv)
                dprintf("usage: dst info\n");
        }

    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("sync toggle", "0");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {

            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: toglle\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
           rv = cmd_data_check_uint8(cmd, &tmp, sizeof (a_uint32_t));
            if (SW_OK != rv)
            {
                dprintf("usage: toggle\n");
            }
            else
            {
                entry.syn_toggle = tmp;
            }
        }

    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("lan wan", "0");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {

            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: lan wan\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
           rv = cmd_data_check_uint8(cmd, &tmp, sizeof (a_uint32_t));
            if (SW_OK != rv)
            {
                dprintf("usage: lan wan\n");
            }
            else
            {
                entry.lan_wan = tmp;
            }
        }

    }
    while (talk_mode && (SW_OK != rv));

    if ((FAL_IP_IP4_ADDR_MCAST & (entry.flags)) == FAL_IP_IP4_ADDR_MCAST ||
		(FAL_IP_IP6_ADDR_MCAST& (entry.flags)) == FAL_IP_IP6_ADDR_MCAST)
    {
        do
        {
            cmd = get_sub_cmd("vsi", "0");
			SW_RTN_ON_NULL_PARAM(cmd);

            if (!strncasecmp(cmd, "quit", 4))
            {
                return SW_BAD_VALUE;
            }
            else if (!strncasecmp(cmd, "help", 4))
            {
                dprintf("usage: vsi\n");
                rv = SW_BAD_VALUE;
            }
            else
            {
                rv = cmd_data_check_uint8(cmd, &tmp, sizeof (a_uint32_t));
                if (SW_OK != rv)
                {
                    dprintf("usage: integer\n");
                }
                else
                {
                    entry.mcast_info.vsi = tmp;
                }
            }
        }
        while (talk_mode && (SW_OK != rv));

        if ((FAL_IP_IP4_ADDR_MCAST & (entry.flags)) == FAL_IP_IP4_ADDR_MCAST) {
            cmd_data_check_element("ip4 addr", NULL,
                               "usage: the format is xx.xx.xx.xx \n",
                               cmd_data_check_ip4addr, (cmd, &(entry.mcast_info.sip4_addr), 4));
        } else {
            cmd_data_check_element("ip6 addr", NULL,
                               "usage: the format is xxxx::xxxx \n",
                               cmd_data_check_ip6addr, (cmd, &(entry.mcast_info.sip6_addr), 16));
        }
    }

    *(fal_host_entry_t *)val = entry;
    return SW_OK;
}

void
cmd_data_print_host_entry(a_uint8_t * param_name, a_uint32_t * buf, a_uint32_t size)
{
    fal_host_entry_t *entry;

    entry = (fal_host_entry_t *) buf;
    dprintf("\n[entryid]:0x%x  [entryflags]:0x%x  [entrystatus]:0x%x",
            entry->entry_id, entry->flags, entry->status);

    if ((FAL_IP_IP4_ADDR & entry->flags) == FAL_IP_IP4_ADDR ||
		(FAL_IP_IP4_ADDR_MCAST & entry->flags) == FAL_IP_IP4_ADDR_MCAST)
    {
        cmd_data_print_ip4addr("\n[ip_addr]:",
                               (a_uint32_t *) & (entry->ip4_addr),
                               sizeof (fal_ip4_addr_t));
    }
    else if ((FAL_IP_IP6_ADDR & entry->flags) == FAL_IP_IP6_ADDR ||
		(FAL_IP_IP6_ADDR_MCAST & entry->flags) == FAL_IP_IP6_ADDR_MCAST)
    {
        cmd_data_print_ip6addr("\n[ip_addr]:",
                               (a_uint32_t *) & (entry->ip6_addr),
                               sizeof (fal_ip6_addr_t));
    }

    cmd_data_print_macaddr("  [mac_addr]:",
                           (a_uint32_t *) & (entry->mac_addr),
                           sizeof (fal_mac_addr_t));

    dprintf("\n[interfaceid]:0x%x  [portid]:0x%x  ", entry->intf_id, entry->port_id);
    dprintf("\n[load_balance num]:0x%x  [vrfid]:0x%x  ", entry->lb_num, entry->vrf_id);

    cmd_data_print_maccmd("action", (a_uint32_t *) & (entry->action),
                          sizeof (fal_fwd_cmd_t));

    if (A_TRUE == entry->mirror_en)
    {
        dprintf("\n[mirror]:Enable   ");
    }
    else
    {
        dprintf("\n[mirror]:Disable   ");
    }

    if (A_TRUE == entry->counter_en)
    {
        dprintf("\n[counter]:Enable   [counter_id]:%d    [pkt]%d    [byte]%d",
                entry->counter_id, entry->packet, entry->byte);
    }
    else
    {
        dprintf("\n[couter]:Disable   ");
    }

    if (A_TRUE == entry->pppoe_en)
    {
        dprintf("\n[pppoe]:Enable   [pppoe_id]:%d", entry->pppoe_id);
    }
    else
    {
        dprintf("\n[pppoe]:Disable   ");
    }
    dprintf("\n[lan_wan]:0x%x  [sync_toggle]:0x%x  [dst_info]:0x%x ", entry->lan_wan, entry->syn_toggle, entry->dst_info);
    dprintf("\n[vsi]:0x%x ", entry->mcast_info.vsi);

    if ((FAL_IP_IP4_ADDR_MCAST & entry->flags) == FAL_IP_IP4_ADDR_MCAST)
    {
        cmd_data_print_ip4addr("\n[ip_addr]:",
                               (a_uint32_t *) & (entry->mcast_info.sip4_addr),
                               sizeof (fal_ip4_addr_t));
    }
    else if ((FAL_IP_IP6_ADDR_MCAST & entry->flags) == FAL_IP_IP6_ADDR_MCAST)
    {
        cmd_data_print_ip6addr("\n[ip_addr]:",
                               (a_uint32_t *) & (entry->mcast_info.sip6_addr),
                               sizeof (fal_ip6_addr_t));
    }

}

sw_error_t
cmd_data_check_arp_learn_mode(char *cmd_str, fal_arp_learn_mode_t * arg_val,
                              a_uint32_t size)
{
    if (NULL == cmd_str)
    {
        return SW_BAD_VALUE;
    }

    if (!strcasecmp(cmd_str, "learnlocal"))
    {
        *arg_val = FAL_ARP_LEARN_LOCAL;
    }
    else if (!strcasecmp(cmd_str, "learnall"))
    {
        *arg_val = FAL_ARP_LEARN_ALL;
    }
    else
    {
        return SW_BAD_VALUE;
    }

    return SW_OK;
}

void
cmd_data_print_arp_learn_mode(a_uint8_t * param_name, a_uint32_t * buf, a_uint32_t size)
{
    dprintf("[%s]:", param_name);
    if (*(a_uint32_t *) buf == FAL_ARP_LEARN_LOCAL)
    {
        dprintf("LearnLocal");
    }
    else if (*(a_uint32_t *) buf == FAL_ARP_LEARN_ALL)
    {
        dprintf("LearnAll");
    }
    else
    {
        dprintf("UNKNOWN VALUE");
    }
}

sw_error_t
cmd_data_check_ip_guard_mode(char *cmd_str, fal_source_guard_mode_t * arg_val, a_uint32_t size)
{
    if (NULL == cmd_str)
    {
        return SW_BAD_VALUE;
    }

    if (!strcasecmp(cmd_str, "mac_ip"))
    {
        *arg_val = FAL_MAC_IP_GUARD;
    }
    else if (!strcasecmp(cmd_str, "mac_ip_port"))
    {
        *arg_val = FAL_MAC_IP_PORT_GUARD;
    }
    else if (!strcasecmp(cmd_str, "mac_ip_vlan"))
    {
        *arg_val = FAL_MAC_IP_VLAN_GUARD;
    }
    else if (!strcasecmp(cmd_str, "mac_ip_port_vlan"))
    {
        *arg_val = FAL_MAC_IP_PORT_VLAN_GUARD;
    }
    else if (!strcasecmp(cmd_str, "no_guard"))
    {
        *arg_val = FAL_NO_SOURCE_GUARD;
    }
    else
    {
        return SW_BAD_VALUE;
    }

    return SW_OK;
}

void
cmd_data_print_ip_guard_mode(a_uint8_t * param_name, a_uint32_t * buf, a_uint32_t size)
{
    dprintf("[%s]:", param_name);
    if (*(a_uint32_t *) buf == FAL_MAC_IP_GUARD)
    {
        dprintf("MAC_IP_GUARD");
    }
    else if (*(a_uint32_t *) buf == FAL_MAC_IP_PORT_GUARD)
    {
        dprintf("MAC_IP_PORT_GUARD");
    }
    else if (*(a_uint32_t *) buf == FAL_MAC_IP_VLAN_GUARD)
    {
        dprintf("MAC_IP_VLAN_GUARD");
    }
    else if (*(a_uint32_t *) buf == FAL_MAC_IP_PORT_VLAN_GUARD)
    {
        dprintf("MAC_IP_PORT_VLAN_GUARD");
    }
    else if (*(a_uint32_t *) buf == FAL_NO_SOURCE_GUARD)
    {
        dprintf("NO_GUARD");
    }
    else
    {
        dprintf("UNKNOWN VALUE");
    }
}

sw_error_t
cmd_data_check_nat_entry(char *cmd_str, void * val, a_uint32_t size)
{
    char *cmd;
    sw_error_t rv;
    a_uint32_t tmp = 0;
    fal_nat_entry_t entry;

    aos_mem_zero(&entry, sizeof (fal_nat_entry_t));

    do
    {
        cmd = get_sub_cmd("entryid", "0");
		SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: integer\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_uint32(cmd, &(entry.entry_id), sizeof (a_uint32_t));
            if (SW_OK != rv)
                dprintf("usage: integer\n");
        }
    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("entryflags", "0");
		SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: bitmap for host entry\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_uint32(cmd, &(entry.flags), sizeof (a_uint32_t));
            if (SW_OK != rv)
                dprintf("usage: bitmap for host entry\n");
        }
    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("entrystatus", "0xf");
		SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: integer\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_uint32(cmd, &(entry.status), sizeof (a_uint32_t));
            if (SW_OK != rv)
                dprintf("usage: integer\n");
        }
    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("select_idx", "0");
		SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: integer\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_uint32(cmd, &(entry.slct_idx), sizeof (a_uint32_t));
            if (SW_OK != rv)
                dprintf("usage: integer\n");
        }
    }
    while (talk_mode && (SW_OK != rv));

	do
    {
        cmd = get_sub_cmd("vrf_id", "0");
		SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: integer\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_uint32(cmd, &(entry.vrf_id), sizeof (a_uint32_t));
            if (SW_OK != rv)
                dprintf("usage: integer\n");
        }
    }
    while (talk_mode && (SW_OK != rv));

    cmd_data_check_element("src addr", "0.0.0.0",
                           "usage: the format is xx.xx.xx.xx \n",
                           cmd_data_check_ip4addr, (cmd, &(entry.src_addr), 4));

    cmd_data_check_element("trans addr", "0.0.0.0",
                           "usage: the format is xx.xx.xx.xx \n",
                           cmd_data_check_ip4addr, (cmd, &(entry.trans_addr), 4));

    do
    {
        cmd = get_sub_cmd("port num", "0");
		SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: 0- 65535\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_uint16(cmd, &tmp, sizeof (a_uint32_t));
            if (SW_OK != rv)
                dprintf("usage: 0- 65535\n");
        }
    }
    while (talk_mode && (SW_OK != rv));
    entry.port_num = tmp & 0xffff;

    do
    {
        cmd = get_sub_cmd("port range", "0");
		SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: 0- 65535\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_uint16(cmd, &tmp, sizeof (a_uint32_t));
            if (SW_OK != rv)
                dprintf("usage: 0- 65535\n");
        }
    }
    while (talk_mode && (SW_OK != rv));
    entry.port_range = tmp & 0xffff;

    do
    {
        cmd = get_sub_cmd("action", "forward");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: <forward/drop/cpycpu/rdtcpu>\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_maccmd(cmd, &entry.action,
                                       sizeof (fal_fwd_cmd_t));
            if (SW_OK != rv)
                dprintf("usage: <forward/drop/cpycpu/rdtcpu>\n");
        }

    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("mirror", "no");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {

            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: <yes/no/y/n>\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_confirm(cmd, A_FALSE, &entry.mirror_en,
                                        sizeof (a_bool_t));
            if (SW_OK != rv)
                dprintf("usage: <yes/no/y/n>\n");
        }

    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("counter", "no");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {

            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: <yes/no/y/n>\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_confirm(cmd, A_FALSE, &entry.counter_en,
                                        sizeof (a_bool_t));
            if (SW_OK != rv)
                dprintf("usage: <yes/no/y/n>\n");
        }

    }
    while (talk_mode && (SW_OK != rv));

    if (A_TRUE == entry.counter_en)
    {
        do
        {
            cmd = get_sub_cmd("counter id", "0");
			SW_RTN_ON_NULL_PARAM(cmd);

            if (!strncasecmp(cmd, "quit", 4))
            {
                return SW_BAD_VALUE;
            }
            else if (!strncasecmp(cmd, "help", 4))
            {
                dprintf("usage: integer\n");
                rv = SW_BAD_VALUE;
            }
            else
            {
                rv = cmd_data_check_uint32(cmd, &(entry.counter_id), sizeof (a_uint32_t));
                if (SW_OK != rv)
                    dprintf("usage: integer\n");
            }
        }
        while (talk_mode && (SW_OK != rv));
    }

    *(fal_nat_entry_t *)val = entry;
    return SW_OK;
}

void
cmd_data_print_nat_entry(a_uint8_t * param_name, a_uint32_t * buf, a_uint32_t size)
{
    fal_nat_entry_t *entry;

    entry = (fal_nat_entry_t *) buf;
    dprintf("\n[entryid]:0x%x  [entryflags]:0x%x  [entrystatus]:0x%x  [select_idx]:0x%x",
            entry->entry_id, entry->flags, entry->status, entry->slct_idx);

	dprintf("\n[vrf_id]:0x%x  ", entry->vrf_id);

    cmd_data_print_ip4addr("\n[src_addr]:",
                           (a_uint32_t *) & (entry->src_addr),
                           sizeof (fal_ip4_addr_t));

    cmd_data_print_ip4addr("\n[trans_addr]:",
                           (a_uint32_t *) & (entry->trans_addr),
                           sizeof (fal_ip4_addr_t));

    dprintf("\n[port_num]:0x%x  [port_range]:0x%x  ", entry->port_num, entry->port_range);

    cmd_data_print_maccmd("action", (a_uint32_t *) & (entry->action),
                          sizeof (fal_fwd_cmd_t));

    if (A_TRUE == entry->mirror_en)
    {
        dprintf("\n[mirror]:Enable   ");
    }
    else
    {
        dprintf("\n[mirror]:Disable   ");
    }

    if (A_TRUE == entry->counter_en)
    {
        dprintf("\n[counter]:Enable   [counter_id]:%d    [in_pkt]%d    [in_byte]%d    [eg_pkt]%d    [eg_byte]%d    ",
                entry->counter_id, entry->ingress_packet, entry->ingress_byte,
                entry->egress_packet, entry->egress_byte);
    }
    else
    {
        dprintf("\n[couter]:Disable   ");
    }
}

sw_error_t
cmd_data_check_napt_entry(char *cmd_str, void * val, a_uint32_t size)
{
    char *cmd;
    sw_error_t rv;
    a_uint32_t tmp = 0;
    fal_napt_entry_t entry;

    aos_mem_zero(&entry, sizeof (fal_napt_entry_t));

    do
    {
        cmd = get_sub_cmd("entryid", "0");
		SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: integer\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_uint32(cmd, &(entry.entry_id), sizeof (a_uint32_t));
            if (SW_OK != rv)
                dprintf("usage: integer\n");
        }
    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("entryflags", "0");
		SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: bitmap for host entry\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_uint32(cmd, &(entry.flags), sizeof (a_uint32_t));
            if (SW_OK != rv)
                dprintf("usage: bitmap for host entry\n");
        }
    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("entrystatus", "0xf");
		SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: integer\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_uint32(cmd, &(entry.status), sizeof (a_uint32_t));
            if (SW_OK != rv)
                dprintf("usage: integer\n");
        }
    }
    while (talk_mode && (SW_OK != rv));

	do
    {
        cmd = get_sub_cmd("vrf_id", "0");
		SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: integer\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_uint32(cmd, &(entry.vrf_id), sizeof (a_uint32_t));
            if (SW_OK != rv)
                dprintf("usage: integer\n");
        }
    }
    while (talk_mode && (SW_OK != rv));

	do
    {
        cmd = get_sub_cmd("flow_cookie", "0");
		SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: integer\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_uint32(cmd, &(entry.flow_cookie), sizeof (a_uint32_t));
            if (SW_OK != rv)
                dprintf("usage: integer\n");
        }
    }
    while (talk_mode && (SW_OK != rv));

	do
    {
        cmd = get_sub_cmd("load_balance", "0");
		SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: integer\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_uint32(cmd, &(entry.load_balance), sizeof (a_uint32_t));
            if (SW_OK != rv)
                dprintf("usage: integer\n");
        }
    }
    while (talk_mode && (SW_OK != rv));

    cmd_data_check_element("src addr", "0.0.0.0",
                           "usage: the format is xx.xx.xx.xx \n",
                           cmd_data_check_ip4addr, (cmd, &(entry.src_addr), 4));

    cmd_data_check_element("dst addr", "0.0.0.0",
                           "usage: the format is xx.xx.xx.xx \n",
                           cmd_data_check_ip4addr, (cmd, &(entry.dst_addr), 4));

    if (FAL_NAT_ENTRY_TRANS_IPADDR_INDEX & (entry.flags))
    {
        do
        {
            cmd = get_sub_cmd("trans addr index", "0");
			SW_RTN_ON_NULL_PARAM(cmd);

            if (!strncasecmp(cmd, "quit", 4))
            {
                return SW_BAD_VALUE;
            }
            else if (!strncasecmp(cmd, "help", 4))
            {
                dprintf("usage: integer\n");
                rv = SW_BAD_VALUE;
            }
            else
            {
                rv = cmd_data_check_uint32(cmd, &(entry.trans_addr), sizeof (a_uint32_t));
                if (SW_OK != rv)
                    dprintf("usage: integer\n");
            }
        }
        while (talk_mode && (SW_OK != rv));
    }
    else
    {
        cmd_data_check_element("trans addr", "0.0.0.0",
                               "usage: the format is xx.xx.xx.xx \n",
                               cmd_data_check_ip4addr, (cmd, &(entry.trans_addr), 4));
    }

    do
    {
        cmd = get_sub_cmd("src port", "0");
		SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: 0- 65535\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_uint16(cmd, &tmp, sizeof (a_uint32_t));
            if (SW_OK != rv)
                dprintf("usage: 0- 65535\n");
        }
    }
    while (talk_mode && (SW_OK != rv));
    entry.src_port = tmp & 0xffff;

    do
    {
        cmd = get_sub_cmd("dst port", "0");
		SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: 0- 65535\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_uint16(cmd, &tmp, sizeof (a_uint32_t));
            if (SW_OK != rv)
                dprintf("usage: 0- 65535\n");
        }
    }
    while (talk_mode && (SW_OK != rv));
    entry.dst_port = tmp & 0xffff;

    do
    {
        cmd = get_sub_cmd("trans port", "0");
		SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: 0- 65535\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_uint16(cmd, &tmp, sizeof (a_uint32_t));
            if (SW_OK != rv)
                dprintf("usage: 0- 65535\n");
        }
    }
    while (talk_mode && (SW_OK != rv));
    entry.trans_port = tmp & 0xffff;

    do
    {
        cmd = get_sub_cmd("action", "forward");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: <forward/drop/cpycpu/rdtcpu>\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_maccmd(cmd, &(entry.action),
                                       sizeof (fal_fwd_cmd_t));
            if (SW_OK != rv)
                dprintf("usage: <forward/drop/cpycpu/rdtcpu>\n");
        }

    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("mirror", "no");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {

            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: <yes/no/y/n>\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_confirm(cmd, A_FALSE, &entry.mirror_en,
                                        sizeof (a_bool_t));
            if (SW_OK != rv)
                dprintf("usage: <yes/no/y/n>\n");
        }

    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("counter", "no");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {

            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: <yes/no/y/n>\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_confirm(cmd, A_FALSE, &entry.counter_en,
                                        sizeof (a_bool_t));
            if (SW_OK != rv)
                dprintf("usage: <yes/no/y/n>\n");
        }

    }
    while (talk_mode && (SW_OK != rv));

    if (A_TRUE == entry.counter_en)
    {
        do
        {
            cmd = get_sub_cmd("counter id", "0");
			SW_RTN_ON_NULL_PARAM(cmd);

            if (!strncasecmp(cmd, "quit", 4))
            {
                return SW_BAD_VALUE;
            }
            else if (!strncasecmp(cmd, "help", 4))
            {
                dprintf("usage: integer\n");
                rv = SW_BAD_VALUE;
            }
            else
            {
                rv = cmd_data_check_uint32(cmd, &(entry.counter_id), sizeof (a_uint32_t));
                if (SW_OK != rv)
                    dprintf("usage: integer\n");
            }
        }
        while (talk_mode && (SW_OK != rv));
    }

	do
    {
        cmd = get_sub_cmd("priority", "no");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {

            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: <yes/no/y/n>\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_confirm(cmd, A_FALSE, &entry.priority_en,
                                        sizeof (a_bool_t));
            if (SW_OK != rv)
                dprintf("usage: <yes/no/y/n>\n");
        }

    }
    while (talk_mode && (SW_OK != rv));


	if (A_TRUE == entry.priority_en)
		{
			do
			{
				cmd = get_sub_cmd("priority value", "0");
				SW_RTN_ON_NULL_PARAM(cmd);

				if (!strncasecmp(cmd, "quit", 4))
				{
					return SW_BAD_VALUE;
				}
				else if (!strncasecmp(cmd, "help", 4))
				{
					dprintf("usage: integer\n");
					rv = SW_BAD_VALUE;
				}
				else
				{
					rv = cmd_data_check_uint32(cmd, &(entry.priority_val), sizeof (a_uint32_t));
					if (SW_OK != rv)
						dprintf("usage: integer\n");
				}
			}
			while (talk_mode && (SW_OK != rv));
		}

    *(fal_napt_entry_t *)val = entry;
    return SW_OK;
}

void
cmd_data_print_napt_entry(a_uint8_t * param_name, a_uint32_t * buf, a_uint32_t size)
{
    fal_napt_entry_t *entry;

    entry = (fal_napt_entry_t *) buf;
    dprintf("\n[entryid]:0x%x  [entryflags]:0x%x  [entrystatus]:0x%x",
            entry->entry_id, entry->flags, entry->status);

	dprintf("\n[vrf_id]:0x%x  [flow_cookie]:0x%x  [load_balance]:0x%x",
            entry->vrf_id, entry->flow_cookie, entry->load_balance);

    cmd_data_print_ip4addr("\n[src_addr]:",
                           (a_uint32_t *) & (entry->src_addr),
                           sizeof (fal_ip4_addr_t));

    cmd_data_print_ip4addr("\n[dst_addr]:",
                           (a_uint32_t *) & (entry->dst_addr),
                           sizeof (fal_ip4_addr_t));

    if (FAL_NAT_ENTRY_TRANS_IPADDR_INDEX & entry->flags)
    {
        dprintf("\n[trans_addr_index]:0x%x", entry->trans_addr);
    }
    else
    {
        cmd_data_print_ip4addr("\n[trans_addr]:",
                               (a_uint32_t *) & (entry->trans_addr),
                               sizeof (fal_ip4_addr_t));
    }

    dprintf("\n[src_port]:0x%x  [dst_port]:0x%x  [trans_port]:0x%x  ", entry->src_port, entry->dst_port, entry->trans_port);

    cmd_data_print_maccmd("action", (a_uint32_t *) & (entry->action),
                          sizeof (fal_fwd_cmd_t));

    if (A_TRUE == entry->mirror_en)
    {
        dprintf("\n[mirror]:Enable   ");
    }
    else
    {
        dprintf("\n[mirror]:Disable   ");
    }

    if (A_TRUE == entry->counter_en)
    {
        dprintf("\n[counter]:Enable   [counter_id]:%d    [in_pkt]%d    [in_byte]%d    [eg_pkt]%d    [eg_byte]%d    ",
                entry->counter_id, entry->ingress_packet, entry->ingress_byte,
                entry->egress_packet, entry->egress_byte);
    }
    else
    {
        dprintf("\n[couter]:Disable   ");
    }

	if (A_TRUE == entry->priority_en)
    {
        dprintf("\n[priority]:Enable   [priority_val]:%d    ",
                entry->priority_val);
    }
    else
    {
        dprintf("\n[priority]:Disable   ");
    }
}

sw_error_t
cmd_data_check_flow_entry(char *cmd_str, void * val, a_uint32_t size)
{
    char *cmd;
    sw_error_t rv;
    a_uint32_t tmp = 0;
    fal_napt_entry_t entry;

    aos_mem_zero(&entry, sizeof (fal_napt_entry_t));

    do
    {
        cmd = get_sub_cmd("entryid", "0");
		SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: integer\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_uint32(cmd, &(entry.entry_id), sizeof (a_uint32_t));
            if (SW_OK != rv)
                dprintf("usage: integer\n");
        }
    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("entryflags", "0");
		SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: bitmap for host entry\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_uint32(cmd, &(entry.flags), sizeof (a_uint32_t));
            if (SW_OK != rv)
                dprintf("usage: bitmap for host entry\n");
        }
    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("entrystatus", "0xf");
		SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: integer\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_uint32(cmd, &(entry.status), sizeof (a_uint32_t));
            if (SW_OK != rv)
                dprintf("usage: integer\n");
        }
    }
    while (talk_mode && (SW_OK != rv));

	do
    {
        cmd = get_sub_cmd("vrf_id", "0");
		SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: integer\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_uint32(cmd, &(entry.vrf_id), sizeof (a_uint32_t));
            if (SW_OK != rv)
                dprintf("usage: integer\n");
        }
    }
    while (talk_mode && (SW_OK != rv));

	do
    {
        cmd = get_sub_cmd("flow_cookie", "0");
		SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: integer\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_uint32(cmd, &(entry.flow_cookie), sizeof (a_uint32_t));
            if (SW_OK != rv)
                dprintf("usage: integer\n");
        }
    }
    while (talk_mode && (SW_OK != rv));

	do
    {
        cmd = get_sub_cmd("load_balance", "0");
		SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: integer\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_uint32(cmd, &(entry.load_balance), sizeof (a_uint32_t));
            if (SW_OK != rv)
                dprintf("usage: integer\n");
        }
    }
    while (talk_mode && (SW_OK != rv));

    cmd_data_check_element("src addr", "0.0.0.0",
                           "usage: the format is xx.xx.xx.xx \n",
                           cmd_data_check_ip4addr, (cmd, &(entry.src_addr), 4));

    cmd_data_check_element("dst addr", "0.0.0.0",
                           "usage: the format is xx.xx.xx.xx \n",
                           cmd_data_check_ip4addr, (cmd, &(entry.dst_addr), 4));

    do
    {
        cmd = get_sub_cmd("src port", "0");
		SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: 0- 65535\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_uint16(cmd, &tmp, sizeof (a_uint32_t));
            if (SW_OK != rv)
                dprintf("usage: 0- 65535\n");
        }
    }
    while (talk_mode && (SW_OK != rv));
    entry.src_port = tmp & 0xffff;

    do
    {
        cmd = get_sub_cmd("dst port", "0");
		SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: 0- 65535\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_uint16(cmd, &tmp, sizeof (a_uint32_t));
            if (SW_OK != rv)
                dprintf("usage: 0- 65535\n");
        }
    }
    while (talk_mode && (SW_OK != rv));
    entry.dst_port = tmp & 0xffff;

    do
    {
        cmd = get_sub_cmd("action", "forward");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: <forward/drop/cpycpu/rdtcpu>\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_maccmd(cmd, &(entry.action),
                                       sizeof (fal_fwd_cmd_t));
            if (SW_OK != rv)
                dprintf("usage: <forward/drop/cpycpu/rdtcpu>\n");
        }

    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("mirror", "no");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {

            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: <yes/no/y/n>\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_confirm(cmd, A_FALSE, &entry.mirror_en,
                                        sizeof (a_bool_t));
            if (SW_OK != rv)
                dprintf("usage: <yes/no/y/n>\n");
        }

    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("counter", "no");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {

            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: <yes/no/y/n>\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_confirm(cmd, A_FALSE, &entry.counter_en,
                                        sizeof (a_bool_t));
            if (SW_OK != rv)
                dprintf("usage: <yes/no/y/n>\n");
        }

    }
    while (talk_mode && (SW_OK != rv));

    if (A_TRUE == entry.counter_en)
    {
        do
        {
            cmd = get_sub_cmd("counter id", "0");
			SW_RTN_ON_NULL_PARAM(cmd);

            if (!strncasecmp(cmd, "quit", 4))
            {
                return SW_BAD_VALUE;
            }
            else if (!strncasecmp(cmd, "help", 4))
            {
                dprintf("usage: integer\n");
                rv = SW_BAD_VALUE;
            }
            else
            {
                rv = cmd_data_check_uint32(cmd, &(entry.counter_id), sizeof (a_uint32_t));
                if (SW_OK != rv)
                    dprintf("usage: integer\n");
            }
        }
        while (talk_mode && (SW_OK != rv));
    }

	do
    {
        cmd = get_sub_cmd("priority", "no");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {

            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: <yes/no/y/n>\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_confirm(cmd, A_FALSE, &entry.priority_en,
                                        sizeof (a_bool_t));
            if (SW_OK != rv)
                dprintf("usage: <yes/no/y/n>\n");
        }

    }
    while (talk_mode && (SW_OK != rv));


	if (A_TRUE == entry.priority_en)
		{
			do
			{
				cmd = get_sub_cmd("priority value", "0");
				SW_RTN_ON_NULL_PARAM(cmd);

				if (!strncasecmp(cmd, "quit", 4))
				{
					return SW_BAD_VALUE;
				}
				else if (!strncasecmp(cmd, "help", 4))
				{
					dprintf("usage: integer\n");
					rv = SW_BAD_VALUE;
				}
				else
				{
					rv = cmd_data_check_uint32(cmd, &(entry.priority_val), sizeof (a_uint32_t));
					if (SW_OK != rv)
						dprintf("usage: integer\n");
				}
			}
			while (talk_mode && (SW_OK != rv));
		}

    *(fal_napt_entry_t *)val = entry;
    return SW_OK;
}

void
cmd_data_print_flow_entry(a_uint8_t * param_name, a_uint32_t * buf, a_uint32_t size)
{
    fal_napt_entry_t *entry;

    entry = (fal_napt_entry_t *) buf;
    dprintf("\n[entryid]:0x%x  [entryflags]:0x%x  [entrystatus]:0x%x",
            entry->entry_id, entry->flags, entry->status);

	dprintf("\n[vrf_id]:0x%x  [flow_cookie]:0x%x  [load_balance]:0x%x",
            entry->vrf_id, entry->flow_cookie, entry->load_balance);

    cmd_data_print_ip4addr("\n[src_addr]:",
                           (a_uint32_t *) & (entry->src_addr),
                           sizeof (fal_ip4_addr_t));

    cmd_data_print_ip4addr("\n[dst_addr]:",
                           (a_uint32_t *) & (entry->dst_addr),
                           sizeof (fal_ip4_addr_t));

    dprintf("\n[src_port]:0x%x  [dst_port]:0x%x  ", entry->src_port, entry->dst_port);

    cmd_data_print_maccmd("action", (a_uint32_t *) & (entry->action),
                          sizeof (fal_fwd_cmd_t));

    if (A_TRUE == entry->mirror_en)
    {
        dprintf("\n[mirror]:Enable   ");
    }
    else
    {
        dprintf("\n[mirror]:Disable   ");
    }

    if (A_TRUE == entry->counter_en)
    {
        dprintf("\n[counter]:Enable   [counter_id]:%d    [in_pkt]%d    [in_byte]%d    [eg_pkt]%d    [eg_byte]%d    ",
                entry->counter_id, entry->ingress_packet, entry->ingress_byte,
                entry->egress_packet, entry->egress_byte);
    }
    else
    {
        dprintf("\n[couter]:Disable   ");
    }

	if (A_TRUE == entry->priority_en)
    {
        dprintf("\n[priority]:Enable   [priority_val]:%d    ",
                entry->priority_val);
    }
    else
    {
        dprintf("\n[priority]:Disable   ");
    }
}

sw_error_t
cmd_data_check_napt_mode(char *cmd_str, fal_napt_mode_t * arg_val,
                         a_uint32_t size)
{
    if (NULL == cmd_str)
    {
        return SW_BAD_VALUE;
    }

    if (!strcasecmp(cmd_str, "fullcone"))
    {
        *arg_val = FAL_NAPT_FULL_CONE;
    }
    else if (!strcasecmp(cmd_str, "strictcone"))
    {
        *arg_val = FAL_NAPT_STRICT_CONE;
    }
    else if (!strcasecmp(cmd_str, "portstrict"))
    {
        *arg_val = FAL_NAPT_PORT_STRICT;
    }
    else if (!strcasecmp(cmd_str, "synmatric"))
    {
        *arg_val = FAL_NAPT_SYNMETRIC;
    }
    else
    {
        return SW_BAD_VALUE;
    }

    return SW_OK;
}

void
cmd_data_print_napt_mode(a_uint8_t * param_name, a_uint32_t * buf, a_uint32_t size)
{
    dprintf("[%s]:", param_name);
    if (*(a_uint32_t *) buf == FAL_NAPT_FULL_CONE)
    {
        dprintf("FullCone");
    }
    else if (*(a_uint32_t *) buf == FAL_NAPT_STRICT_CONE)
    {
        dprintf("StrictCone");
    }
    else if (*(a_uint32_t *) buf == FAL_NAPT_PORT_STRICT)
    {
        dprintf("PortStrict");
    }
    else if (*(a_uint32_t *) buf == FAL_NAPT_SYNMETRIC)
    {
        dprintf("Synmatric");
    }
    else
    {
        dprintf("UNKNOWN VALUE");
    }
}

sw_error_t
cmd_data_check_intf_mac_entry(char *cmd_str, void * val, a_uint32_t size)
{
    char *cmd;
    a_uint32_t tmp = 0;
    sw_error_t rv;
    fal_intf_mac_entry_t entry;

    aos_mem_zero(&entry, sizeof (fal_intf_mac_entry_t));

    do
    {
        cmd = get_sub_cmd("entryid", "0");
		SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: integer\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_uint32(cmd, &(entry.entry_id), sizeof (a_uint32_t));
            if (SW_OK != rv)
                dprintf("usage: integer\n");
        }
    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("vrfid", "0");
		SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: integer\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_uint32(cmd, &(entry.vrf_id), sizeof (a_uint32_t));
            if (SW_OK != rv)
                dprintf("usage: integer\n");
        }
    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("vid low", NULL);
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: low vlan id\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_uint16(cmd, &tmp, sizeof (a_uint32_t));
            if (SW_OK != rv)
                dprintf("usage: low vlan id\n");
        }
    }
    while (talk_mode && (SW_OK != rv));
    entry.vid_low = tmp & 0xffff;

    do
    {
        cmd = get_sub_cmd("vid high", NULL);
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: high vlan id\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_uint16(cmd, &tmp, sizeof (a_uint32_t));
            if (SW_OK != rv)
                dprintf("usage: high vlan id\n");
        }
    }
    while (talk_mode && (SW_OK != rv));
    entry.vid_high = tmp & 0xffff;

    cmd_data_check_element("mac addr", NULL,
                           "usage: the format is xx-xx-xx-xx-xx-xx \n",
                           cmd_data_check_macaddr, (cmd, &(entry.mac_addr),
                                   sizeof (fal_mac_addr_t)));

    do
    {
        cmd = get_sub_cmd("ip4_route", "yes");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {

            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: <yes/no/y/n>\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_confirm(cmd, A_TRUE, &entry.ip4_route,
                                        sizeof (a_bool_t));
            if (SW_OK != rv)
                dprintf("usage: <yes/no/y/n>\n");
        }

    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("ip6_route", "yes");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {

            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: <yes/no/y/n>\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_confirm(cmd, A_TRUE, &entry.ip6_route,
                                        sizeof (a_bool_t));
            if (SW_OK != rv)
                dprintf("usage: <yes/no/y/n>\n");
        }
    }
    while (talk_mode && (SW_OK != rv));

    *(fal_intf_mac_entry_t *)val = entry;
    return SW_OK;
}

void
cmd_data_print_intf_mac_entry(a_uint8_t * param_name, a_uint32_t * buf, a_uint32_t size)
{
    fal_intf_mac_entry_t *entry;

    entry = (fal_intf_mac_entry_t *) buf;
    dprintf("\n[entryid]:0x%x  [vrf_id]:0x%x  [vid_low]:0x%x  [vid_high]:0x%x",
            entry->entry_id, entry->vrf_id, entry->vid_low, entry->vid_high);

    cmd_data_print_macaddr("\n[mac_addr]:",
                           (a_uint32_t *) & (entry->mac_addr),
                           sizeof (fal_mac_addr_t));

    if (A_TRUE == entry->ip4_route)
    {
        dprintf("\n[ip4_route]:TRUE");
    }
    else
    {
        dprintf("\n[ip4_route]:FALSE");
    }

    if (A_TRUE == entry->ip6_route)
    {
        dprintf("  [ip6_route]:TRUE");
    }
    else
    {
        dprintf("  [ip6_route]:FALSE");
    }
}

sw_error_t
cmd_data_check_pub_addr_entry(char *cmd_str, void * val, a_uint32_t size)
{
    char *cmd;
    sw_error_t rv;
    fal_nat_pub_addr_t entry;

    aos_mem_zero(&entry, sizeof (fal_nat_pub_addr_t));

    do
    {
        cmd = get_sub_cmd("entryid", "0");
		SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: integer\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_uint32(cmd, &(entry.entry_id), sizeof (a_uint32_t));
            if (SW_OK != rv)
                dprintf("usage: integer\n");
        }
    }
    while (talk_mode && (SW_OK != rv));

    cmd_data_check_element("pub addr", NULL,
                           "usage: the format is xx.xx.xx.xx \n",
                           cmd_data_check_ip4addr, (cmd, &(entry.pub_addr), 4));

    *(fal_nat_pub_addr_t *)val = entry;
    return SW_OK;
}

void
cmd_data_print_pub_addr_entry(a_uint8_t * param_name, a_uint32_t * buf, a_uint32_t size)
{
    fal_nat_pub_addr_t *entry;

    entry = (fal_nat_pub_addr_t *) buf;
    dprintf("[entryid]:0x%x  ", entry->entry_id);
    cmd_data_print_ip4addr("[pub_addr]:",
                           (a_uint32_t *) & (entry->pub_addr),
                           sizeof (fal_ip4_addr_t));

}

sw_error_t
cmd_data_check_egress_shaper(char *cmd_str, void * val, a_uint32_t size)
{
    char *cmd;
    sw_error_t rv;
    a_bool_t bool = A_FALSE;
    fal_egress_shaper_t entry;

    aos_mem_zero(&entry, sizeof (fal_egress_shaper_t));

    do
    {
        cmd = get_sub_cmd("bytebased", "yes");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {

            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: <yes/no/y/n>\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_confirm(cmd, A_TRUE, &bool,
                                        sizeof (a_bool_t));
            if (SW_OK != rv)
                dprintf("usage: <yes/no/y/n>\n");
        }
    }
    while (talk_mode && (SW_OK != rv));

    if (A_TRUE == bool)
    {
        entry.meter_unit = FAL_BYTE_BASED;
    }
    else
    {
        entry.meter_unit = FAL_FRAME_BASED;
    }

    do
    {
        cmd = get_sub_cmd("cir", "0");
		SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: integer\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_uint32(cmd, &(entry.cir), sizeof (a_uint32_t));
            if (SW_OK != rv)
                dprintf("usage: integer\n");
        }
    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("cbs", "0");
		SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: integer\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_uint32(cmd, &(entry.cbs), sizeof (a_uint32_t));
            if (SW_OK != rv)
                dprintf("usage: integer\n");
        }
    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("eir", "0");
		SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: integer\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_uint32(cmd, &(entry.eir), sizeof (a_uint32_t));
            if (SW_OK != rv)
                dprintf("usage: integer\n");
        }
    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("ebs", "0");
		SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: integer\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_uint32(cmd, &(entry.ebs), sizeof (a_uint32_t));
            if (SW_OK != rv)
                dprintf("usage: integer\n");
        }
    }
    while (talk_mode && (SW_OK != rv));

    *(fal_egress_shaper_t *)val = entry;
    return SW_OK;
}

void
cmd_data_print_egress_shaper(a_uint8_t * param_name, a_uint32_t * buf, a_uint32_t size)
{
    fal_egress_shaper_t *entry;

    entry = (fal_egress_shaper_t *) buf;

    if (FAL_BYTE_BASED == entry->meter_unit)
    {
        dprintf("\n[byte_based]:yes  ");
    }
    else
    {
        dprintf("\n[byte_based]:no  ");
    }

    dprintf("[cir]:0x%08x  [cbs]:0x%08x  [eir]:0x%08x  [ebs]:0x%08x",
            entry->cir, entry->cbs, entry->eir, entry->ebs);
}

sw_error_t
cmd_data_check_policer_timesslot(char *cmd_str, a_uint32_t * arg_val, a_uint32_t size)
{
    if (cmd_str == NULL)
        return SW_BAD_PARAM;

    if (!strncasecmp(cmd_str, "100us", 5))
        *arg_val = FAL_RATE_MI_100US;
    else if (!strncasecmp(cmd_str, "1ms", 3))
        *arg_val = FAL_RATE_MI_1MS;
    else if (!strncasecmp(cmd_str, "10ms", 4))
        *arg_val = FAL_RATE_MI_10MS;
    else if (!strncasecmp(cmd_str, "100ms", 5))
        *arg_val = FAL_RATE_MI_100MS;
    else
    {
        //dprintf("input error \n");
        return SW_BAD_VALUE;
    }

    return SW_OK;
}

void
cmd_data_print_policer_timesslot(char * param_name, a_uint32_t * buf, a_uint32_t size)
{
    dprintf("[%s]:", param_name);
    if (*(a_uint32_t *) buf == FAL_RATE_MI_100US)
    {
        dprintf("100us");
    }
    else if (*(a_uint32_t *) buf == FAL_RATE_MI_1MS)
    {
        dprintf("1ms");
    }
    else if (*(a_uint32_t *) buf == FAL_RATE_MI_10MS)
    {
        dprintf("10ms");
    }
    else if (*(a_uint32_t *) buf == FAL_RATE_MI_100MS)
    {
        dprintf("100ms");
    }
    else
    {
        dprintf("UNKNOWN VALUE");
    }
}


sw_error_t
cmd_data_check_acl_policer(char *cmd_str, void * val, a_uint32_t size)
{
    char *cmd;
    sw_error_t rv;
    a_bool_t bool = A_FALSE;
    fal_acl_policer_t entry;

    aos_mem_zero(&entry, sizeof (fal_acl_policer_t));

    do
    {
        cmd = get_sub_cmd("counter_mode", "no");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {

            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: <yes/no/y/n>\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_confirm(cmd, A_FALSE, &(entry.counter_mode),
                                        sizeof (a_bool_t));
            if (SW_OK != rv)
                dprintf("usage: <yes/no/y/n>\n");
        }
    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("bytebased", "yes");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {

            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: <yes/no/y/n>\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_confirm(cmd, A_TRUE, &bool,
                                        sizeof (a_bool_t));
            if (SW_OK != rv)
                dprintf("usage: <yes/no/y/n>\n");
        }
    }
    while (talk_mode && (SW_OK != rv));

    if (A_TRUE == bool)
    {
        entry.meter_unit = FAL_BYTE_BASED;
    }
    else
    {
        entry.meter_unit = FAL_FRAME_BASED;
    }

    if (A_TRUE == entry.counter_mode)
    {
        *(fal_acl_policer_t *)val = entry;
        return SW_OK;
    }

    do
    {
        cmd = get_sub_cmd("couple_flag", "no");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {

            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: <yes/no/y/n>\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_confirm(cmd, A_FALSE, &(entry.couple_flag),
                                        sizeof (a_bool_t));
            if (SW_OK != rv)
                dprintf("usage: <yes/no/y/n>\n");
        }
    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("color_aware", "no");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {

            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: <yes/no/y/n>\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_confirm(cmd, A_FALSE, &(entry.color_mode),
                                        sizeof (a_bool_t));
            if (SW_OK != rv)
                dprintf("usage: <yes/no/y/n>\n");
        }
    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("deficit_flag", "no");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {

            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: <yes/no/y/n>\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_confirm(cmd, A_FALSE, &(entry.deficit_en),
                                        sizeof (a_bool_t));
            if (SW_OK != rv)
                dprintf("usage: <yes/no/y/n>\n");
        }
    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("cir", "0");
		SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: integer\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_uint32(cmd, &(entry.cir), sizeof (a_uint32_t));
            if (SW_OK != rv)
                dprintf("usage: integer\n");
        }
    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("cbs", "0");
		SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: integer\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_uint32(cmd, &(entry.cbs), sizeof (a_uint32_t));
            if (SW_OK != rv)
                dprintf("usage: integer\n");
        }
    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("eir", "0");
		SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: integer\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_uint32(cmd, &(entry.eir), sizeof (a_uint32_t));
            if (SW_OK != rv)
                dprintf("usage: integer\n");
        }
    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("ebs", "0");
		SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: integer\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_uint32(cmd, &(entry.ebs), sizeof (a_uint32_t));
            if (SW_OK != rv)
                dprintf("usage: integer\n");
        }
    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("meter_interval", "1ms");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {

            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: the format <100us/1ms/10ms/100ms>\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_policer_timesslot(cmd, &(entry.meter_interval),
                                                  sizeof (fal_rate_mt_t));
            if (SW_OK != rv)
                dprintf("usage: the format <100us/1ms/10ms/100ms>\n");
        }
    }
    while (talk_mode && (SW_OK != rv));

    *(fal_acl_policer_t *)val = entry;
    return SW_OK;
}

void
cmd_data_print_acl_policer(a_uint8_t * param_name, a_uint32_t * buf, a_uint32_t size)
{
    fal_acl_policer_t *entry;

    entry = (fal_acl_policer_t *) buf;

    if (A_TRUE == entry->counter_mode)
    {
        dprintf("[counter_mode]:yes  ");
    }
    else
    {
        dprintf("[counter_mode]:no  ");
    }

    if (FAL_BYTE_BASED == entry->meter_unit)
    {
        dprintf("[meter_unit]:byte_based  ");
    }
    else
    {
        dprintf("[meter_unit]:frame_based  ");
    }

    if (A_TRUE == entry->counter_mode)
    {
        dprintf("[counter_lo]:0x%x  [counter_hi]", entry->counter_low, entry->counter_high);
    }
    else
    {
        if (A_TRUE == entry->color_mode)
        {
            dprintf("[color_aware]:yes  ");
        }
        else
        {
            dprintf("[color_aware]:no  ");
        }

        if (A_TRUE == entry->couple_flag)
        {
            dprintf("[couple_falg]:yes  ");
        }
        else
        {
            dprintf("[couple_falg]:no  ");
        }

        if (A_TRUE == entry->deficit_en)
        {
            dprintf("[deficit_falg]:yes  ");
        }
        else
        {
            dprintf("[deficit_falg]:no  ");
        }

        cmd_data_print_policer_timesslot("meter_interval",
                                         (a_uint32_t *) & (entry->meter_interval),
                                         sizeof (fal_rate_mt_t));

        dprintf("\n[cir]:0x%08x  [cbs]:0x%08x  [eir]:0x%08x  [ebs]:0x%08x",
                entry->cir, entry->cbs, entry->eir, entry->ebs);
    }

    return;
}

sw_error_t
cmd_data_check_port_policer(char *cmd_str, void * val, a_uint32_t size)
{
    char *cmd;
    sw_error_t rv;
    a_bool_t bool = A_FALSE;
    fal_port_policer_t entry;

    aos_mem_zero(&entry, sizeof (fal_port_policer_t));

    do
    {
        cmd = get_sub_cmd("combine_enable", "no");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {

            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: <yes/no/y/n>\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_confirm(cmd, A_FALSE, &(entry.combine_mode),
                                        sizeof (a_bool_t));
            if (SW_OK != rv)
                dprintf("usage: <yes/no/y/n>\n");
        }
    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("bytebased", "yes");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {

            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: <yes/no/y/n>\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_confirm(cmd, A_TRUE, &bool,
                                        sizeof (a_bool_t));
            if (SW_OK != rv)
                dprintf("usage: <yes/no/y/n>\n");
        }
    }
    while (talk_mode && (SW_OK != rv));

    if (A_TRUE == bool)
    {
        entry.meter_unit = FAL_BYTE_BASED;
    }
    else
    {
        entry.meter_unit = FAL_FRAME_BASED;
    }

    do
    {
        cmd = get_sub_cmd("couple_flag", "no");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {

            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: <yes/no/y/n>\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_confirm(cmd, A_FALSE, &(entry.couple_flag),
                                        sizeof (a_bool_t));
            if (SW_OK != rv)
                dprintf("usage: <yes/no/y/n>\n");
        }
    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("color_aware", "no");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {

            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: <yes/no/y/n>\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_confirm(cmd, A_FALSE, &(entry.color_mode),
                                        sizeof (a_bool_t));
            if (SW_OK != rv)
                dprintf("usage: <yes/no/y/n>\n");
        }
    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("deficit_flag", "no");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {

            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: <yes/no/y/n>\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_confirm(cmd, A_FALSE, &(entry.deficit_en),
                                        sizeof (a_bool_t));
            if (SW_OK != rv)
                dprintf("usage: <yes/no/y/n>\n");
        }
    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("c_enable", "no");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {

            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: <yes/no/y/n>\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_confirm(cmd, A_FALSE, &(entry.c_enable),
                                        sizeof (a_bool_t));
            if (SW_OK != rv)
                dprintf("usage: <yes/no/y/n>\n");
        }
    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("cir", "0");
		SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: integer\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_uint32(cmd, &(entry.cir), sizeof (a_uint32_t));
            if (SW_OK != rv)
                dprintf("usage: integer\n");
        }
    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("cbs", "0");
		SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: integer\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_uint32(cmd, &(entry.cbs), sizeof (a_uint32_t));
            if (SW_OK != rv)
                dprintf("usage: integer\n");
        }
    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("c_rate_flag", "0xfe");
		SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: integer\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_uint32(cmd, &(entry.c_rate_flag), sizeof (a_uint32_t));
            if (SW_OK != rv)
                dprintf("usage: integer\n");
        }
    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("c_meter_interval", "1ms");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {

            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: the format <100us/1ms/10ms/100ms>\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_policer_timesslot(cmd, &(entry.c_meter_interval),
                                                  sizeof (fal_rate_mt_t));
            if (SW_OK != rv)
                dprintf("usage: the format <100us/1ms/10ms/100ms>\n");
        }
    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("e_enable", "no");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {

            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: <yes/no/y/n>\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_confirm(cmd, A_FALSE, &(entry.e_enable),
                                        sizeof (a_bool_t));
            if (SW_OK != rv)
                dprintf("usage: <yes/no/y/n>\n");
        }
    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("eir", "0");
		SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: integer\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_uint32(cmd, &(entry.eir), sizeof (a_uint32_t));
            if (SW_OK != rv)
                dprintf("usage: integer\n");
        }
    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("ebs", "0");
		SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: integer\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_uint32(cmd, &(entry.ebs), sizeof (a_uint32_t));
            if (SW_OK != rv)
                dprintf("usage: integer\n");
        }
    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("e_rate_flag", "0xfe");
		SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: integer\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_uint32(cmd, &(entry.e_rate_flag), sizeof (a_uint32_t));
            if (SW_OK != rv)
                dprintf("usage: integer\n");
        }
    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("e_meter_interval", "1ms");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {

            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: the format <100us/1ms/10ms/100ms>\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_policer_timesslot(cmd, &(entry.e_meter_interval),
                                                  sizeof (fal_rate_mt_t));
            if (SW_OK != rv)
                dprintf("usage: the format <100us/1ms/10ms/100ms>\n");
        }
    }
    while (talk_mode && (SW_OK != rv));

    *(fal_port_policer_t *)val = entry;
    return SW_OK;
}

void
cmd_data_print_port_policer(a_uint8_t * param_name, a_uint32_t * buf, a_uint32_t size)
{
    fal_port_policer_t *entry;

    entry = (fal_port_policer_t *) buf;

    if (A_TRUE == entry->combine_mode)
    {
        dprintf("[combine_mode]:yes  ");
    }
    else
    {
        dprintf("[combine_mode]:no  ");
    }

    if (FAL_BYTE_BASED == entry->meter_unit)
    {
        dprintf("[meter_unit]:byte_based  ");
    }
    else
    {
        dprintf("[meter_unit]:frame_based  ");
    }

    if (A_TRUE == entry->color_mode)
    {
        dprintf("[color_aware]:yes  ");
    }
    else
    {
        dprintf("[color_aware]:no  ");
    }

    if (A_TRUE == entry->couple_flag)
    {
        dprintf("[couple_falg]:yes  ");
    }
    else
    {
        dprintf("[couple_falg]:no  ");
    }

    if (A_TRUE == entry->deficit_en)
    {
        dprintf("[deficit_falg]:yes  ");
    }
    else
    {
        dprintf("[deficit_falg]:no  ");
    }

    if (A_TRUE == entry->c_enable)
    {
        dprintf("\n[c_enable]:yes  ");
    }
    else
    {
        dprintf("\n[c_enable]:no   ");
    }

    dprintf("[cir]:0x%08x  [cbs]:0x%08x  ", entry->cir,entry->cbs);



    dprintf("[c_rate_flag]:0x%08x  ", entry->c_rate_flag);

    cmd_data_print_policer_timesslot("c_meter_interval",
                                     (a_uint32_t *) & (entry->c_meter_interval),
                                     sizeof (fal_rate_mt_t));

    if (A_TRUE == entry->e_enable)
    {
        dprintf("\n[e_enable]:yes  ");
    }
    else
    {
        dprintf("\n[e_enable]:no   ");
    }

    dprintf("[eir]:0x%08x  [ebs]:0x%08x  ", entry->eir, entry->ebs);

    dprintf("[e_rate_flag]:0x%08x  ", entry->e_rate_flag);

    cmd_data_print_policer_timesslot("e_meter_interval",
                                     (a_uint32_t *) & (entry->e_meter_interval),
                                     sizeof (fal_rate_mt_t));
    return;
}

sw_error_t
cmd_data_check_mac_mode(char *cmd_str, fal_interface_mac_mode_t * arg_val,
                        a_uint32_t size)
{
    if (NULL == cmd_str)
    {
        return SW_BAD_VALUE;
    }

    if (0 == cmd_str[0])
    {
        *arg_val = FAL_MAC_MODE_RGMII;
    }
    else if (!strcasecmp(cmd_str, "rgmii"))
    {
        *arg_val = FAL_MAC_MODE_RGMII;
    }
    else if (!strcasecmp(cmd_str, "rmii"))
    {
        *arg_val = FAL_MAC_MODE_RMII;
    }
    else if (!strcasecmp(cmd_str, "gmii"))
    {
        *arg_val = FAL_MAC_MODE_GMII;
    }
    else if (!strcasecmp(cmd_str, "mii"))
    {
        *arg_val = FAL_MAC_MODE_MII;
    }
    else if (!strcasecmp(cmd_str, "sgmii"))
    {
        *arg_val = FAL_MAC_MODE_SGMII;
    }
    else if (!strcasecmp(cmd_str, "fiber"))
    {
        *arg_val = FAL_MAC_MODE_FIBER;
    }
    else if (!strcasecmp(cmd_str, "default"))
    {
        *arg_val = FAL_MAC_MODE_DEFAULT;
    }
    else
    {
        return SW_BAD_VALUE;
    }

    return SW_OK;
}

sw_error_t
cmd_data_check_clock_mode(char *cmd_str, fal_interface_clock_mode_t * arg_val,
                          a_uint32_t size)
{
    if (NULL == cmd_str)
    {
        return SW_BAD_VALUE;
    }

    if (0 == cmd_str[0])
    {
        *arg_val = FAL_INTERFACE_CLOCK_MAC_MODE;
    }
    if (!strcasecmp(cmd_str, "mac"))
    {
        *arg_val = FAL_INTERFACE_CLOCK_MAC_MODE;
    }
    else if (!strcasecmp(cmd_str, "phy"))
    {
        *arg_val = FAL_INTERFACE_CLOCK_PHY_MODE;
    }
    else
    {
        return SW_BAD_VALUE;
    }

    return SW_OK;
}


sw_error_t
cmd_data_check_mac_config(char *cmd_str, void * val, a_uint32_t size)
{
    char *cmd;
    fal_mac_config_t entry;

    aos_mem_zero(&entry, sizeof (fal_mac_config_t));

    cmd_data_check_element("mac_mode", "rgmii",
                           "usage: port0 <rgmii/rmii/gmii/mii/sgmii/fiber/default>\nport6 <rgmii/mii/sgmii/fiber/default>\n",
                           cmd_data_check_mac_mode, (cmd, &(entry.mac_mode), 4));

    if (FAL_MAC_MODE_RGMII == entry.mac_mode)
    {
        cmd_data_check_element("txclk_delay_cmd", "no",
                               "usage: <yes/no/y/n>\n",
                               cmd_data_check_confirm, (cmd, A_FALSE, &(entry.config.rgmii.txclk_delay_cmd), 4));

        cmd_data_check_element("txclk_delay_select", "0",
                               "usage: <0-3>\n",
                               cmd_data_check_uint32, (cmd, &(entry.config.rgmii.txclk_delay_sel), 4));

        cmd_data_check_element("rxclk_delay_cmd", "no",
                               "usage: <yes/no/y/n>\n",
                               cmd_data_check_confirm, (cmd, A_FALSE, &(entry.config.rgmii.rxclk_delay_cmd), 4));

        cmd_data_check_element("rxclk_delay_select", "0",
                               "usage: <0-3>\n",
                               cmd_data_check_uint32, (cmd, &(entry.config.rgmii.rxclk_delay_sel), 4));
    }

    if (FAL_MAC_MODE_RMII == entry.mac_mode)
    {
        cmd_data_check_element("master_mode", "no",
                               "usage: <yes/no/y/n>\n",
                               cmd_data_check_confirm, (cmd, A_FALSE, &(entry.config.rmii.master_mode), 4));

        cmd_data_check_element("slave_mode", "no",
                               "usage: <yes/no/y/n>\n",
                               cmd_data_check_confirm, (cmd, A_FALSE, &(entry.config.rmii.slave_mode), 4));

        cmd_data_check_element("clock_inverse", "no",
                               "usage: <yes/no/y/n>\n",
                               cmd_data_check_confirm, (cmd, A_FALSE, &(entry.config.rmii.clock_inverse), 4));
        cmd_data_check_element("pipe_rxclk_sel", "no",
                               "usage: <yes/no/y/n>\n",
                               cmd_data_check_confirm, (cmd, A_FALSE, &(entry.config.rmii.pipe_rxclk_sel), 4));

    }

    if ((FAL_MAC_MODE_GMII == entry.mac_mode)
            || (FAL_MAC_MODE_MII == entry.mac_mode))
    {
        cmd_data_check_element("clock_mode", "mac",
                               "usage: <phy/mac>\n",
                               cmd_data_check_clock_mode, (cmd, &(entry.config.gmii.clock_mode), 4));

        cmd_data_check_element("txclk_select", "0",
                               "usage: <0-1>\n",
                               cmd_data_check_uint32, (cmd, &(entry.config.gmii.txclk_select), 4));

        cmd_data_check_element("rxclk_select", "0",
                               "usage: <0-1>\n",
                               cmd_data_check_uint32, (cmd, &(entry.config.gmii.rxclk_select), 4));
    }

    if (FAL_MAC_MODE_SGMII == entry.mac_mode)
    {
        cmd_data_check_element("clock_mode", "mac",
                               "usage: <phy/mac>\n",
                               cmd_data_check_clock_mode, (cmd, &(entry.config.sgmii.clock_mode), 4));

        cmd_data_check_element("auto_neg", "no",
                               "usage: <yes/no/y/n>\n",
                               cmd_data_check_confirm, (cmd, A_FALSE, &(entry.config.sgmii.auto_neg), 4));

        cmd_data_check_element("force_speed", "no",
                               "usage: <yes/no/y/n>\n",
                               cmd_data_check_confirm, (cmd, A_FALSE, &(entry.config.sgmii.force_speed), 4));

        cmd_data_check_element("prbs_enable", "no",
                               "usage: <yes/no/y/n>\n",
                               cmd_data_check_confirm, (cmd, A_FALSE, &(entry.config.sgmii.prbs_enable), 4));

        cmd_data_check_element("rem_phy_lpbk", "no",
                               "usage: <yes/no/y/n>\n",
                               cmd_data_check_confirm, (cmd, A_FALSE, &(entry.config.sgmii.rem_phy_lpbk), 4));
    }

    if (FAL_MAC_MODE_FIBER == entry.mac_mode)
    {
        cmd_data_check_element("auto_neg", "no",
                               "usage: <yes/no/y/n>\n",
                               cmd_data_check_confirm, (cmd, A_FALSE, &(entry.config.fiber.auto_neg), 4));

        cmd_data_check_element("fx100_enable", "no",
                               "usage: <yes/no/y/n>\n",
                               cmd_data_check_confirm, (cmd, A_FALSE, &(entry.config.fiber.fx100_enable), 4));
    }

    *(fal_mac_config_t *)val = entry;
    return SW_OK;
}

void
cmd_data_print_mac_config(a_uint8_t * param_name, a_uint32_t * buf, a_uint32_t size)
{
    fal_mac_config_t *entry;

    entry = (fal_mac_config_t *) buf;

    if (FAL_MAC_MODE_RGMII == entry->mac_mode)
    {
        dprintf("[mac_mode]:rgmii");
    }
    else if (FAL_MAC_MODE_RMII == entry->mac_mode)
    {
        dprintf("[mac_mode]:rmii");
    }
    else if (FAL_MAC_MODE_GMII == entry->mac_mode)
    {
        dprintf("[mac_mode]:gmii");
    }
    else if (FAL_MAC_MODE_MII == entry->mac_mode)
    {
        dprintf("[mac_mode]:mii");
    }
    else if (FAL_MAC_MODE_SGMII == entry->mac_mode)
    {
        dprintf("[mac_mode]:sgmii");
    }
    else if (FAL_MAC_MODE_FIBER == entry->mac_mode)
    {
        dprintf("[mac_mode]:fiber");
    }
    else
    {
        dprintf("[mac_mode]:default");
    }

    if (FAL_MAC_MODE_RGMII == entry->mac_mode)
    {
        if (A_TRUE == entry->config.rgmii.txclk_delay_cmd)
        {
            dprintf("\n[txclk_delay_cmd]:yes [txclk_delay_select]:%d", entry->config.rgmii.txclk_delay_sel);
        }
        else
        {
            dprintf("\n[txclk_delay_cmd]:no");
        }

        if (A_TRUE == entry->config.rgmii.rxclk_delay_cmd)
        {
            dprintf("\n[rxclk_delay_cmd]:yes [rxclk_delay_select]:%d", entry->config.rgmii.rxclk_delay_sel);
        }
        else
        {
            dprintf("\n[rxclk_delay_cmd]:no");
        }

    }
    else if (FAL_MAC_MODE_RMII == entry->mac_mode)
    {
        if (A_TRUE == entry->config.rmii.master_mode)
        {
            dprintf("\n[master_mode]:yes");
        }
        else
        {
            dprintf("\n[master_mode]:no");
        }

        if (A_TRUE == entry->config.rmii.slave_mode)
        {
            dprintf("\n[slave_mode]:yes");
        }
        else
        {
            dprintf("\n[slave_mode]:no");
        }

        if (A_TRUE == entry->config.rmii.clock_inverse)
        {
            dprintf("\n[clock_inverse]:yes");
        }
        else
        {
            dprintf("\n[clock_inverse]:no");
        }

        if (A_TRUE == entry->config.rmii.pipe_rxclk_sel)
        {
            dprintf("\n[pipe_rxclk_sel]:yes");
        }
        else
        {
            dprintf("\n[pipe_rxclk_sel]:no");
        }


    }
    else if ((FAL_MAC_MODE_GMII == entry->mac_mode)
             || (FAL_MAC_MODE_MII == entry->mac_mode))
    {

        if (FAL_INTERFACE_CLOCK_PHY_MODE == entry->config.gmii.clock_mode)
        {
            dprintf("\n[clock_mode]:phy [txclk_select]:%d [rxclk_select]:%d", entry->config.gmii.txclk_select, entry->config.gmii.rxclk_select);
        }
        else
        {
            dprintf("\n[clock_mode]:mac [txclk_select]:%d [rxclk_select]:%d", entry->config.gmii.txclk_select, entry->config.gmii.rxclk_select);
        }
    }
    else if (FAL_MAC_MODE_SGMII == entry->mac_mode)
    {
        if (FAL_INTERFACE_CLOCK_PHY_MODE == entry->config.sgmii.clock_mode)
        {
            dprintf("\n[clock_mode]:phy");
        }
        else
        {
            dprintf("\n[clock_mode]:mac");
        }

        if (A_TRUE == entry->config.sgmii.auto_neg)
        {
            dprintf("\n[auto_neg]:yes");
        }
        else
        {
            dprintf("\n[auto_neg]:no");
        }
        if (A_TRUE == entry->config.sgmii.force_speed)
        {
            dprintf("\n[force_speed]:yes");
        }
        else
        {
            dprintf("\n[force_speed]:no");
        }
        if (A_TRUE == entry->config.sgmii.prbs_enable)
        {
            dprintf("\n[prbs_enable]:yes");
        }
        else
        {
            dprintf("\n[prbs_enable]:no");
        }
        if (A_TRUE == entry->config.sgmii.rem_phy_lpbk)
        {
            dprintf("\n[rem_phy_lpbk]:yes");
        }
        else
        {
            dprintf("\n[rem_phy_lpbk]:no");
        }
    }
    else if (FAL_MAC_MODE_FIBER == entry->mac_mode)
    {
        if (A_TRUE == entry->config.fiber.auto_neg)
        {
            dprintf("\n[auto_neg]:yes");
        }
        else
        {
            dprintf("\n[auto_neg]:no");
        }
        if (A_TRUE == entry->config.fiber.fx100_enable)
        {
            dprintf("\n[fx100_enable]:yes");
        }
        else
        {
            dprintf("\n[fx100_enable]:no");
        }
    }

    return;
}

sw_error_t
cmd_data_check_phy_config(char *cmd_str, void * val, a_uint32_t size)
{
    char *cmd;
    fal_phy_config_t entry;

    aos_mem_zero(&entry, sizeof (fal_phy_config_t));

    cmd_data_check_element("mac_mode", "rgmii",
                           "usage: <rgmii/default>\n",
                           cmd_data_check_mac_mode, (cmd, &(entry.mac_mode), 4));

    if (FAL_MAC_MODE_RGMII == entry.mac_mode)
    {

        cmd_data_check_element("txclk_delay_cmd", "no",
                               "usage: <yes/no/y/n>\n",
                               cmd_data_check_confirm, (cmd, A_FALSE, &(entry.txclk_delay_cmd), 4));

        cmd_data_check_element("txclk_delay_select", "0",
                               "usage: <0-3>\n",
                               cmd_data_check_uint32, (cmd, &(entry.txclk_delay_sel), 4));

        cmd_data_check_element("rxclk_delay_cmd", "no",
                               "usage: <yes/no/y/n>\n",
                               cmd_data_check_confirm, (cmd, A_FALSE, &(entry.rxclk_delay_cmd), 4));

        cmd_data_check_element("rxclk_delay_select", "0",
                               "usage: <0-3>\n",
                               cmd_data_check_uint32, (cmd, &(entry.rxclk_delay_sel), 4));
    }
    else
    {
        return SW_BAD_VALUE;
    }

    *(fal_phy_config_t *)val = entry;
    return SW_OK;
}

void
cmd_data_print_phy_config(a_uint8_t * param_name, a_uint32_t * buf, a_uint32_t size)
{
    fal_phy_config_t *entry;

    entry = (fal_phy_config_t *) buf;

    if (FAL_MAC_MODE_RGMII == entry->mac_mode)
    {
        dprintf("[mac_mode]:rgmii");
    }
    else
    {
        dprintf("[mac_mode]:default");
    }

    if (FAL_MAC_MODE_RGMII == entry->mac_mode)
    {
        if (A_TRUE == entry->txclk_delay_cmd)
        {
            dprintf("\n[txclk_delay_cmd]:yes [txclk_delay_select]:%d", entry->txclk_delay_sel);
        }
        else
        {
            dprintf("\n[txclk_delay_cmd]:no");
        }

        if (A_TRUE == entry->rxclk_delay_cmd)
        {
            dprintf("\n[rxclk_delay_cmd]:yes [rxclk_delay_select]:%d", entry->rxclk_delay_sel);
        }
        else
        {
            dprintf("\n[rxclk_delay_cmd]:no");
        }
    }
    return;
}

sw_error_t
cmd_data_check_fdb_smode(char *cmd_str, a_uint32_t * arg_val, a_uint32_t size)
{
    if (cmd_str == NULL)
        return SW_BAD_PARAM;

    if (!strcasecmp(cmd_str, "ivl"))
        *arg_val = INVALID_VLAN_IVL;
    else if (!strcasecmp(cmd_str, "svl"))
        *arg_val = INVALID_VLAN_SVL;
    else
    {
        return SW_BAD_VALUE;
    }

    return SW_OK;
}

void
cmd_data_print_fdb_smode(a_uint8_t * param_name, a_uint32_t * buf, a_uint32_t size)
{
    dprintf("[%s]:", param_name);
    if (*(a_uint32_t *) buf == 1)
    {
        dprintf("IVL");
    }
    else if (*(a_uint32_t *) buf == 0)
    {
        dprintf("SVL");
    }
    else
    {
        dprintf("UNKNOWN VALUE");
    }
}

void
cmd_data_print_fdb_ctrl_mode(a_uint8_t * param_name, a_uint32_t * buf, a_uint32_t size)
{
    dprintf("[%s]:", param_name);
    if (*(a_uint32_t *) buf == 0)
    {
        dprintf("auto mode");
    }
    else if (*(a_uint32_t *) buf == 1)
    {
        dprintf("control mode");
    }
    else
    {
        dprintf("UNKNOWN VALUE");
    }
}

sw_error_t
cmd_data_check_fx100_link_mode(char* cmd_str, fx100_ctrl_link_mode_t* arg_val)
{
    if (0 == cmd_str[0])
    {
        *arg_val = Fx100BASE_MODE;
    }
    else if (!strcasecmp(cmd_str, "fx100base"))
    {
        *arg_val = Fx100BASE_MODE;
    }
    else
    {
        dprintf("UNKNOWN VALUE");
        return SW_BAD_VALUE;
    }

    return SW_OK;
}

sw_error_t
cmd_data_check_fx100_fd_mode(char *cmd_str, a_uint32_t * arg_val)
{
    if (0 == cmd_str[0])
    {
        *arg_val = FX100_FULL_DUPLEX;
    }
    else if (!strcasecmp(cmd_str, "fullduplex"))
    {
        *arg_val = FX100_FULL_DUPLEX;
    }
    else if (!strcasecmp(cmd_str, "halfduplex"))
    {
        *arg_val = FX100_HALF_DUPLEX;
    }
    else
    {
        dprintf("UNKNOWN VALUE");
        return SW_BAD_VALUE;
    }

    return SW_OK;
}

sw_error_t
cmd_data_check_sgmii_fiber_mode(char *cmd_str, a_uint32_t * arg_val)
{
    if (0 == cmd_str[0])
    {
        *arg_val = FX100_SERDS_MODE;
    }
    else if (!strcasecmp(cmd_str, "fx100serds"))
    {
        *arg_val = FX100_SERDS_MODE;
    }
    else
    {
        dprintf("UNKNOWN VALUE");
        return SW_BAD_VALUE;
    }
    return SW_OK;
}

sw_error_t
cmd_data_check_fx100_config(char *cmd_str, void * val, a_uint32_t size)
{
    char *cmd;
    fal_fx100_ctrl_config_t entry;

    aos_mem_zero(&entry, sizeof (fal_fx100_ctrl_config_t));

    cmd_data_check_element("link_mode", "fx100base",
                           "usage: <fx100base>\n",
                           cmd_data_check_fx100_link_mode, (cmd, &(entry.link_mode)));

    cmd_data_check_element("overshoot", "no",
                           "usage: <yes/no/y/n>\n",
                           cmd_data_check_confirm, (cmd, A_FALSE, &(entry.overshoot), 4));

    cmd_data_check_element("loopback_mode", "no",
                           "usage: <yes/no/y/n>\n",
                           cmd_data_check_confirm, (cmd, A_FALSE, &(entry.loopback), 4));

    cmd_data_check_element("fd_mode", "fullduplex",
                           "usage: <fullduplex/halfduplex>\n",
                           cmd_data_check_fx100_fd_mode, (cmd, &(entry.fd_mode)));

    cmd_data_check_element("col_test", "no",
                           "usage: <yes/no/y/n>\n",
                           cmd_data_check_confirm, (cmd, A_FALSE, &(entry.col_test), 4));

    cmd_data_check_element("sgmii_fiber", "fx100serds",
                           "usage: <fx100serds>\n",
                           cmd_data_check_sgmii_fiber_mode, (cmd, &(entry.sgmii_fiber_mode)));

    cmd_data_check_element("crs_ctrl", "yes",
                           "usage: <yes/no/y/n>\n",
                           cmd_data_check_confirm, (cmd, A_TRUE, &(entry.crs_ctrl), 4));

    cmd_data_check_element("loopback_ctrl", "no",
                           "usage: <yes/no/y/n>\n",
                           cmd_data_check_confirm, (cmd, A_FALSE, &(entry.loopback_ctrl), 4));

    cmd_data_check_element("crs_col_100_ctrl", "yes",
                           "usage: <yes/no/y/n>\n",
                           cmd_data_check_confirm, (cmd, A_TRUE, &(entry.crs_col_100_ctrl), 4));

    cmd_data_check_element("loop_en", "no",
                           "usage: <yes/no/y/n>\n",
                           cmd_data_check_confirm, (cmd, A_FALSE, &(entry.loop_en), 4));



    *(fal_fx100_ctrl_config_t *)val = entry;
    return SW_OK;
}

void
cmd_data_print_fx100_config(a_uint8_t * param_name, a_uint32_t * buf, a_uint32_t size)
{
    fal_fx100_ctrl_config_t* entry;

    entry = (fal_fx100_ctrl_config_t*)buf;

    if (Fx100BASE_MODE == entry->link_mode)
    {
        dprintf("[link_mode]: fx100base\n");
    }

    if (A_TRUE == entry->overshoot)
    {
        dprintf("[overshoot]: yes\n");
    }
    else
    {
        dprintf("[overshoot]: no\n");
    }

    if (A_TRUE == entry->loopback)
    {
        dprintf("[loopback_mode]: yes\n");
    }
    else
    {
        dprintf("[loopback_mode]: no\n");
    }

    if (FX100_FULL_DUPLEX == entry->fd_mode)
    {
        dprintf("[fd_mode]: fullduplex\n");
    }
    else
    {
        dprintf("[fd_mode]: halfduplex\n");
    }

    if (A_TRUE == entry->col_test)
    {
        dprintf("[col_test]: yes\n");
    }
    else
    {
        dprintf("[col_test]: no\n");
    }

    if (FX100_SERDS_MODE == entry->sgmii_fiber_mode)
    {
        dprintf("[sgmii_fiber]: fx100_serds\n");
    }

    if (A_TRUE == entry->crs_ctrl)
    {
        dprintf("[crs_ctrl]: yes\n");
    }
    else
    {
        dprintf("[crs_ctrl]: no\n");
    }

    if (A_TRUE == entry->loopback_ctrl)
    {
        dprintf("[loopback_ctrl]: yes\n");
    }
    else
    {
        dprintf("[loopback_ctrl]: no\n");
    }

    if (A_TRUE == entry->crs_col_100_ctrl)
    {
        dprintf("[crs_col_100_ctrl]: yes\n");
    }
    else
    {
        dprintf("[crs_col_100_ctrl]: no\n");
    }

    if (A_TRUE == entry->loop_en)
    {
        dprintf("[loop_en]: yes\n");
    }
    else
    {
        dprintf("[loop_en]: no\n");
    }

}

sw_error_t
cmd_data_check_sec_mac(char *cmd_str, a_uint32_t * arg_val, a_uint32_t size)
{
    if (cmd_str == NULL)
        return SW_BAD_PARAM;

    if (!strcasecmp(cmd_str, "resv_vid"))
        *arg_val = FAL_NORM_MAC_RESV_VID_CMD;
    else if (!strcasecmp(cmd_str, "invalid_src_addr"))
        *arg_val = FAL_NORM_MAC_INVALID_SRC_ADDR_CMD;
    else
    {
        dprintf("UNKNOWN VALUE");
        return SW_BAD_VALUE;
    }

    return SW_OK;
}

sw_error_t
cmd_data_check_sec_ip(char *cmd_str, a_uint32_t * arg_val, a_uint32_t size)
{
    if (cmd_str == NULL)
        return SW_BAD_PARAM;

    if (!strcasecmp(cmd_str, "invalid_ver"))
        *arg_val = FAL_NORM_IP_INVALID_VER_CMD;
    else if (!strcasecmp(cmd_str, "same_addr"))
        *arg_val = FAL_NROM_IP_SAME_ADDR_CMD;
    else if (!strcasecmp(cmd_str, "ttl_change_status"))
        *arg_val = FAL_NROM_IP_TTL_CHANGE_STATUS;
    else if (!strcasecmp(cmd_str, "ttl_val"))
        *arg_val = FAL_NROM_IP_TTL_VALUE;
    else
    {
        dprintf("UNKNOWN VALUE");
        return SW_BAD_VALUE;
    }

    return SW_OK;
}


sw_error_t
cmd_data_check_sec_ip4(char *cmd_str, a_uint32_t * arg_val, a_uint32_t size)
{
    if (cmd_str == NULL)
        return SW_BAD_PARAM;

    if (!strcasecmp(cmd_str, "invalid_hl"))
        *arg_val = FAL_NROM_IP4_INVALID_HL_CMD;
    else if (!strcasecmp(cmd_str, "hdr_opts"))
        *arg_val = FAL_NROM_IP4_HDR_OPTIONS_CMD;
    else if (!strcasecmp(cmd_str, "invalid_df"))
        *arg_val = FAL_NROM_IP4_INVALID_DF_CMD;
    else if (!strcasecmp(cmd_str, "frag_offset_min_len"))
        *arg_val = FAL_NROM_IP4_FRAG_OFFSET_MIN_LEN_CMD;
    else if (!strcasecmp(cmd_str, "frag_offset_min_size"))
        *arg_val = FAL_NROM_IP4_FRAG_OFFSET_MIN_SIZE;
    else if (!strcasecmp(cmd_str, "frag_offset_max_len"))
        *arg_val = FAL_NROM_IP4_FRAG_OFFSET_MAX_LEN_CMD;
    else if (!strcasecmp(cmd_str, "invalid_frag_offset"))
        *arg_val = FAL_NROM_IP4_INVALID_FRAG_OFFSET_CMD;
    else if (!strcasecmp(cmd_str, "invalid_sip"))
        *arg_val = FAL_NROM_IP4_INVALID_SIP_CMD;
    else if (!strcasecmp(cmd_str, "invalid_dip"))
        *arg_val = FAL_NROM_IP4_INVALID_DIP_CMD;
    else if (!strcasecmp(cmd_str, "invalid_chksum"))
        *arg_val = FAL_NROM_IP4_INVALID_CHKSUM_CMD;
    else if (!strcasecmp(cmd_str, "invalid_pl"))
        *arg_val = FAL_NROM_IP4_INVALID_PL_CMD;
    else if (!strcasecmp(cmd_str, "df_clear_status"))
        *arg_val = FAL_NROM_IP4_DF_CLEAR_STATUS;
    else if (!strcasecmp(cmd_str, "ipid_random_status"))
        *arg_val = FAL_NROM_IP4_IPID_RANDOM_STATUS;
    else
    {
        dprintf("UNKNOWN VALUE");
        return SW_BAD_VALUE;
    }

    return SW_OK;
}

sw_error_t
cmd_data_check_sec_ip6(char *cmd_str, a_uint32_t * arg_val, a_uint32_t size)
{
    if (cmd_str == NULL)
        return SW_BAD_PARAM;

    if (!strcasecmp(cmd_str, "invalid_dip"))
        *arg_val = FAL_NROM_IP6_INVALID_DIP_CMD;
    else if (!strcasecmp(cmd_str, "invalid_sip"))
        *arg_val = FAL_NROM_IP6_INVALID_SIP_CMD;
    else if (!strcasecmp(cmd_str, "invalid_pl"))
        *arg_val = FAL_NROM_IP6_INVALID_PL_CMD;
    else
    {
        dprintf("UNKNOWN VALUE");
        return SW_BAD_VALUE;
    }

    return SW_OK;
}


sw_error_t
cmd_data_check_sec_tcp(char *cmd_str, a_uint32_t * arg_val, a_uint32_t size)
{
    if (cmd_str == NULL)
        return SW_BAD_PARAM;

    if (!strcasecmp(cmd_str, "blat"))
        *arg_val = FAL_NROM_TCP_BLAT_CMD;
    else if (!strcasecmp(cmd_str, "invalid_hl"))
        *arg_val = FAL_NROM_TCP_INVALID_HL_CMD;
    else if (!strcasecmp(cmd_str, "min_hdr_size"))
        *arg_val = FAL_NROM_TCP_MIN_HDR_SIZE;
    else if (!strcasecmp(cmd_str, "invalid_syn"))
        *arg_val = FAL_NROM_TCP_INVALID_SYN_CMD;
    else if (!strcasecmp(cmd_str, "su_block"))
        *arg_val = FAL_NROM_TCP_SU_BLOCK_CMD;
    else if (!strcasecmp(cmd_str, "sp_block"))
        *arg_val = FAL_NROM_TCP_SP_BLOCK_CMD;
    else if (!strcasecmp(cmd_str, "sap_block"))
        *arg_val = FAL_NROM_TCP_SAP_BLOCK_CMD;
    else if (!strcasecmp(cmd_str, "xmas_scan"))
        *arg_val = FAL_NROM_TCP_XMAS_SCAN_CMD;
    else if (!strcasecmp(cmd_str, "null_scan"))
        *arg_val = FAL_NROM_TCP_NULL_SCAN_CMD;
    else if (!strcasecmp(cmd_str, "sr_block"))
        *arg_val = FAL_NROM_TCP_SR_BLOCK_CMD;
    else if (!strcasecmp(cmd_str, "sf_block"))
        *arg_val = FAL_NROM_TCP_SF_BLOCK_CMD;
    else if (!strcasecmp(cmd_str, "sar_block"))
        *arg_val = FAL_NROM_TCP_SAR_BLOCK_CMD;
    else if (!strcasecmp(cmd_str, "rst_scan"))
        *arg_val = FAL_NROM_TCP_RST_SCAN_CMD;
    else if (!strcasecmp(cmd_str, "rst_with_data"))
        *arg_val = FAL_NROM_TCP_RST_WITH_DATA_CMD;
    else if (!strcasecmp(cmd_str, "fa_block"))
        *arg_val = FAL_NROM_TCP_FA_BLOCK_CMD;
    else if (!strcasecmp(cmd_str, "pa_block"))
        *arg_val = FAL_NROM_TCP_PA_BLOCK_CMD;
    else if (!strcasecmp(cmd_str, "ua_block"))
        *arg_val = FAL_NROM_TCP_UA_BLOCK_CMD;
    else if (!strcasecmp(cmd_str, "invalid_chksum"))
        *arg_val = FAL_NROM_TCP_INVALID_CHKSUM_CMD;
    else if (!strcasecmp(cmd_str, "invalid_urgptr"))
        *arg_val = FAL_NROM_TCP_INVALID_URGPTR_CMD;
    else if (!strcasecmp(cmd_str, "invalid_opts"))
        *arg_val = FAL_NROM_TCP_INVALID_OPTIONS_CMD;
    else
    {
        dprintf("UNKNOWN VALUE");
        return SW_BAD_VALUE;
    }

    return SW_OK;
}


sw_error_t
cmd_data_check_sec_udp(char *cmd_str, a_uint32_t * arg_val, a_uint32_t size)
{
    if (cmd_str == NULL)
        return SW_BAD_PARAM;

    if (!strcasecmp(cmd_str, "blat"))
        *arg_val = FAL_NROM_UDP_BLAT_CMD;
    else if (!strcasecmp(cmd_str, "invalid_len"))
        *arg_val = FAL_NROM_UDP_INVALID_LEN_CMD;
    else if (!strcasecmp(cmd_str, "invalid_chksum"))
        *arg_val = FAL_NROM_UDP_INVALID_CHKSUM_CMD;
    else
    {
        dprintf("UNKNOWN VALUE");
        return SW_BAD_VALUE;
    }

    return SW_OK;
}

sw_error_t
cmd_data_check_sec_icmp4(char *cmd_str, a_uint32_t * arg_val, a_uint32_t size)
{
    if (cmd_str == NULL)
        return SW_BAD_PARAM;

    if (!strcasecmp(cmd_str, "ping_pl_exceed"))
        *arg_val = FAL_NROM_ICMP4_PING_PL_EXCEED_CMD;
    else if (!strcasecmp(cmd_str, "ping_frag"))
        *arg_val = FAL_NROM_ICMP4_PING_FRAG_CMD;
    else if (!strcasecmp(cmd_str, "ping_max_pl"))
        *arg_val = FAL_NROM_ICMP4_PING_MAX_PL_VALUE;
    else
    {
        //dprintf("input error");
        return SW_BAD_VALUE;
    }

    return SW_OK;
}

sw_error_t
cmd_data_check_sec_icmp6(char *cmd_str, a_uint32_t * arg_val, a_uint32_t size)
{
    if (cmd_str == NULL)
        return SW_BAD_PARAM;

    if (!strcasecmp(cmd_str, "ping_pl_exceed"))
        *arg_val = FAL_NROM_ICMP6_PING_PL_EXCEED_CMD;
    else if (!strcasecmp(cmd_str, "ping_frag"))
        *arg_val = FAL_NROM_ICMP6_PING_FRAG_CMD;
    else if (!strcasecmp(cmd_str, "ping_max_pl"))
        *arg_val = FAL_NROM_ICMP6_PING_MAX_PL_VALUE;
    else
    {
        //dprintf("input error");
        return SW_BAD_VALUE;
    }

    return SW_OK;
}

sw_error_t
cmd_data_check_remark_entry(char *info, void *val, a_uint32_t size)
{
    char *cmd;
    sw_error_t rv;
    fal_egress_remark_table_t *pEntry = (fal_egress_remark_table_t *)val;
    a_uint32_t tmp = 0;

    memset(pEntry, 0, sizeof(fal_egress_remark_table_t));

    /* get remark_dscp */
    do
    {
        cmd = get_sub_cmd("remark dscp", "enable");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: <enable/disable>\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_enable(cmd, &(pEntry->remark_dscp), sizeof(a_bool_t));
            if (SW_OK != rv)
                dprintf("usage: <enable/disable>\n");
        }
    }
    while (talk_mode && (SW_OK != rv));

    /* get remark_up */
    do
    {
        cmd = get_sub_cmd("remark up", "enable");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: <enable/disable>\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_enable(cmd, &(pEntry->remark_up), sizeof(a_bool_t));
            if (SW_OK != rv)
                dprintf("usage: <enable/disable>\n");
        }
    }
    while (talk_mode && (SW_OK != rv));

    /* get remark_dei */
    do
    {
        cmd = get_sub_cmd("remark dei", "enable");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: <enable/disable>\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_enable(cmd, &(pEntry->remark_dei), sizeof(a_bool_t));
            if (SW_OK != rv)
                dprintf("usage: <enable/disable>\n");
        }
    }
    while (talk_mode && (SW_OK != rv));

    /* get g_dscp */
    do
    {
        cmd = get_sub_cmd("green dscp", NULL);
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: the range is 0 -- 63\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_uint16(cmd, &tmp, sizeof (a_uint32_t));
            if (SW_OK != rv)
            {
                dprintf("usage: the range is 0 -- 63\n");
            }

            if (tmp > 63)
            {
                dprintf("usage: the range is 0 -- 63\n");
                rv = SW_OUT_OF_RANGE;
            }
        }
    }
    while (talk_mode && (SW_OK != rv));
    pEntry->g_dscp = tmp;

    /* get y_dscp */
    do
    {
        cmd = get_sub_cmd("yellow dscp", NULL);
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: the range is 0 -- 63\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_uint16(cmd, &tmp, sizeof (a_uint32_t));
            if (SW_OK != rv)
            {
                dprintf("usage: the range is 0 -- 63\n");
            }

            if (tmp > 63)
            {
                dprintf("usage: the range is 0 -- 63\n");
                rv = SW_OUT_OF_RANGE;
            }
        }
    }
    while (talk_mode && (SW_OK != rv));
    pEntry->y_dscp = tmp;

    /* get g_up */
    do
    {
        cmd = get_sub_cmd("green up", NULL);
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: the range is 0 -- 63\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_uint16(cmd, &tmp, sizeof (a_uint32_t));
            if (SW_OK != rv)
            {
                dprintf("usage: the range is 0 -- 7\n");
            }

            if (tmp > 63)
            {
                dprintf("usage: the range is 0 -- 7\n");
                rv = SW_OUT_OF_RANGE;
            }
        }
    }
    while (talk_mode && (SW_OK != rv));
    pEntry->g_up = tmp;

    /* get y_up */
    do
    {
        cmd = get_sub_cmd("yellow up", NULL);
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: the range is 0 -- 63\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_uint16(cmd, &tmp, sizeof (a_uint32_t));
            if (SW_OK != rv)
            {
                dprintf("usage: the range is 0 -- 7\n");
            }

            if (tmp > 63)
            {
                dprintf("usage: the range is 0 -- 7\n");
                rv = SW_OUT_OF_RANGE;
            }
        }
    }
    while (talk_mode && (SW_OK != rv));
    pEntry->y_up = tmp;

    /* get g_dei */
    do
    {
        cmd = get_sub_cmd("green dei", NULL);
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: the range is 0 -- 1\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_uint16(cmd, &tmp, sizeof (a_uint32_t));
            if (SW_OK != rv)
            {
                dprintf("usage: the range is 0 -- 1\n");
            }

            if (tmp > 1)
            {
                dprintf("usage: the range is 0 -- 1\n");
                rv = SW_OUT_OF_RANGE;
            }
        }
    }
    while (talk_mode && (SW_OK != rv));
    pEntry->g_dei = tmp;

    /* get y_dei */
    do
    {
        cmd = get_sub_cmd("yellow dei", NULL);
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: the range is 0 -- 1\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_uint16(cmd, &tmp, sizeof (a_uint32_t));
            if (SW_OK != rv)
            {
                dprintf("usage: the range is 0 -- 1\n");
            }

            if (tmp > 1)
            {
                dprintf("usage: the range is 0 -- 1\n");
                rv = SW_OUT_OF_RANGE;
            }
        }
    }
    while (talk_mode && (SW_OK != rv));
    pEntry->y_dei = tmp;


/*
    dprintf("remark_dscp=%d, remark_up=%d, g_dscp=%d, y_dscp=%d\n",
            pEntry->remark_dscp,
            pEntry->remark_up,
            pEntry->g_dscp,
            pEntry->y_dscp);

    *(fal_egress_remark_table_t *) val = entry;
*/
    return SW_OK;
}

void
cmd_data_print_remark_entry(a_uint8_t * param_name, a_uint32_t * buf, a_uint32_t size)
{
    fal_egress_remark_table_t *entry;

    entry = (fal_egress_remark_table_t *) buf;
    dprintf("\n");
    dprintf("[remark dscp]:%s\n", entry->remark_dscp?"enabled":"disabled");
    dprintf("[remark up]:%s\n", entry->remark_up?"enabled":"disabled");
    dprintf("[remark dei]:%s\n", entry->remark_dei?"enabled":"disabled");
    dprintf("[green dscp]:%d\n", entry->g_dscp);
    dprintf("[yellow dscp]:%d\n", entry->y_dscp);
    dprintf("[green up]:%d\n", entry->g_up);
    dprintf("[yellow up]:%d\n", entry->y_up);
    dprintf("[green dei]:%d\n", entry->g_dei);
    dprintf("[yellow dei]:%d\n", entry->y_dei);

    return;
}

sw_error_t
cmd_data_check_default_route_entry(char *cmd_str, void * val, a_uint32_t size)
{
    char *cmd;
    sw_error_t rv;
    fal_default_route_t entry;

    aos_mem_zero(&entry, sizeof (fal_default_route_t));

    do
    {
        cmd = get_sub_cmd("entry valid", "0");
		SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: 0 for invalid and 1 for valid \n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_uint32(cmd, &(entry.valid), sizeof (a_uint32_t));
            if (SW_OK != rv)
                dprintf("usage: 0 for invalid and 1 for valid \n");
        }
    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("vrf id", "0");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: VRF id\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_uint32(cmd, &(entry.vrf_id), sizeof (a_uint32_t));
            if (SW_OK != rv)
                dprintf("usage: VRF id\n");
        }
    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("ip version", "0");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: 0 for ipv4 and 1 for ipv6 \n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_uint16(cmd, &(entry.ip_version), sizeof (a_uint32_t));
            if (SW_OK != rv)
                dprintf("usage: 0 for ipv4 and 1 for ipv6 \n");
        }
    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("route type", "0");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {

            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: 0 for arp and 1 for wcmp \n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_uint32(cmd, &(entry.droute_type), sizeof (a_uint32_t));
            if (SW_OK != rv)
                dprintf("usage: 0 for arp and 1 for wcmp \n");
        }
    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("index", "0");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {

            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: index for arp entry or wcmp entry \n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_uint32(cmd, &(entry.index), sizeof (a_uint32_t));
            if (SW_OK != rv)
                dprintf("usage: index for arp entry or wcmp entry \n");
        }
    }
    while (talk_mode && (SW_OK != rv));

    *(fal_default_route_t *)val = entry;
    return SW_OK;
}

sw_error_t
cmd_data_check_u_qmap(char *cmd_str, void * val, a_uint32_t size)
{
    char *cmd;
    a_uint32_t tmp;
    sw_error_t rv;
    fal_ucast_queue_dest_t entry;

    aos_mem_zero(&entry, sizeof (fal_ucast_queue_dest_t));

    do
    {
        cmd = get_sub_cmd("src_profile", "0");
		SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: src profile \n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_uint8(cmd, &tmp, sizeof (a_uint32_t));
            if (SW_OK != rv)
            {
                dprintf("usage: src profile \n");
            }
            else
            {
                entry.src_profile = tmp;
            }
        }
    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("service_code_en", "no");
		SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: <yes/no/y/n>\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_confirm(cmd, A_FALSE, &(entry.service_code_en), sizeof (a_bool_t));
            if (SW_OK != rv)
                dprintf("usage: <yes/no/y/n>\n");
        }
    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("service_code", "0");
		SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: service code \n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_uint16(cmd, &tmp, sizeof (a_uint32_t));
            if (SW_OK != rv)
            {
                dprintf("usage: service code \n");
            }
            else
            {
                entry.service_code = tmp;
            }
        }
    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("cpu_code_en", "no");
		SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: <yes/no/y/n>\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_confirm(cmd, A_FALSE, &(entry.cpu_code_en), sizeof (a_bool_t));
            if (SW_OK != rv)
                dprintf("usage: <yes/no/y/n>\n");
        }
    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("cpu_code", "0");
		SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: cpu code \n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_uint16(cmd, &tmp, sizeof (a_uint32_t));
            if (SW_OK != rv)
            {
                dprintf("usage: cpu code \n");
            }
            else
            {
                entry.cpu_code = tmp;
            }
        }
    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("dst_port", "0");
		SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: dest port\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_uint32(cmd, &(entry.dst_port), sizeof (a_uint32_t));
            if (SW_OK != rv)
                dprintf("usage: dst port \n");
        }
    }
    while (talk_mode && (SW_OK != rv));

    *(fal_ucast_queue_dest_t *)val = entry;
    return SW_OK;
}

void
cmd_data_print_default_route_entry(a_uint8_t * param_name, a_uint32_t * buf, a_uint32_t size)
{
    fal_default_route_t *entry;

    entry = (fal_default_route_t *) buf;
    dprintf("\n[valid]:0x%x  [vrf_id]:0x%x  [ip_version]:0x%x  [host_type]:0x%x  [index]:0x%x \n",
            entry->valid, entry->vrf_id, entry->ip_version, entry->droute_type, entry->index);
}

sw_error_t
cmd_data_check_host_route_entry(char *cmd_str, void * val, a_uint32_t size)
{
    char *cmd;
    sw_error_t rv;
    fal_host_route_t entry;

    aos_mem_zero(&entry, sizeof (fal_intf_mac_entry_t));

    do
    {
        cmd = get_sub_cmd("entry valid", "0");
		SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: 0 for invalid and 1 for valid \n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_uint32(cmd, &(entry.valid), sizeof (a_uint32_t));
            if (SW_OK != rv)
                dprintf("usage: 0 for invalid and 1 for valid \n");
        }
    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("vrf id", "0");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: VRF id\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_uint32(cmd, &(entry.vrf_id), sizeof (a_uint32_t));
            if (SW_OK != rv)
                dprintf("usage: VRF id\n");
        }
    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("ip version", "0");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: 0 for ipv4 and 1 for ipv6 \n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_uint32(cmd, &(entry.ip_version), sizeof (a_uint32_t));
            if (SW_OK != rv)
                dprintf("usage: 0 for ipv4 and 1 for ipv6 \n");
        }
    }
    while (talk_mode && (SW_OK != rv));

    if (entry.ip_version == 0) /*IPv4*/
    {
        cmd_data_check_element("ip4 addr", NULL,
                               "usage: the format is xx.xx.xx.xx \n",
                               cmd_data_check_ip4addr, (cmd, &(entry.route_addr.ip4_addr), 4));
    }
    else if (entry.ip_version == 1) /*IPv6*/
    {
        cmd_data_check_element("ip6 addr", NULL,
                               "usage: the format is xx.xx.xx.xx \n",
                               cmd_data_check_ip4addr, (cmd, &(entry.route_addr.ip6_addr), 16));
    }
    else
    {
        return SW_BAD_VALUE;
    }

    do
    {
        cmd = get_sub_cmd("prefix_length", "0");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: prefix length for this host route, 0~31 for "
                "ipv4 and 0~127 for ipv6 \n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_uint32(cmd, &(entry.prefix_length), sizeof (a_uint32_t));
            if (SW_OK != rv)
                dprintf("usage: prefix length for this host route, 0~31 for ipv4 "
                    "and 0~127 for ipv6 \n");
        }
    }
    while (talk_mode && (SW_OK != rv));

    *(fal_host_route_t *)val = entry;
    return SW_OK;
}

void
cmd_data_print_host_route_entry(a_uint8_t * param_name, a_uint32_t * buf, a_uint32_t size)
{
    fal_host_route_t *entry;

    entry = (fal_host_route_t *) buf;
    dprintf("\n[valid]:0x%x  [vrf_id]:0x%x  [prefix_length]:0x%x",
            entry->valid, entry->vrf_id, entry->prefix_length);

    if (0 == entry->ip_version)
    {
        cmd_data_print_ip4addr("\n[ip_addr]:",
                               (a_uint32_t *) & (entry->route_addr.ip4_addr),
                               sizeof (fal_ip4_addr_t));
    }
    else if (1 == entry->ip_version)
    {
        cmd_data_print_ip6addr("\n[ip_addr]:",
                               (a_uint32_t *) & (entry->route_addr.ip6_addr),
                               sizeof (fal_ip6_addr_t));
    }
}

sw_error_t
cmd_data_check_array(char *cmdstr, void *val, a_uint32_t size)
{
    char *tmp = NULL, *str_save;
    a_uint32_t i = 0, j;
    a_uint32_t addr;
    int *dst = (int*)val;

    if (NULL == cmdstr)
    {
        return SW_BAD_VALUE; /*was: SW_OK;*/
    }

    if (0 == cmdstr[0])
    {
        return SW_OK;
    }

    tmp = (void *) strtok_r(cmdstr, "-", &str_save);
    while (tmp)
    {
        if (size <= i)
        {
            return SW_BAD_VALUE;
        }

        if ((2 < strlen(tmp)) || (0 == strlen(tmp)))
        {
            return SW_BAD_VALUE;
        }

        for (j = 0; j < strlen(tmp); j++)
        {
            if (A_FALSE == is_hex(tmp[j]))
                return SW_BAD_VALUE;
        }

        sscanf(tmp, "%x", &addr);
        if (0xff < addr)
        {
            return SW_BAD_VALUE;
        }

        dst[i++] = addr;
        tmp = (void *) strtok_r(NULL, "-", &str_save);
    }

    if (size != i)
    {
        return SW_BAD_VALUE;
    }

    return SW_OK;
}


sw_error_t
cmd_data_check_ip_wcmp_entry(char *cmd_str, void * val, a_uint32_t size)
{

	char *cmd;
	sw_error_t rv;
	fal_ip_wcmp_t entry;

	aos_mem_zero(&entry, sizeof (fal_ip_wcmp_t));

	do
	{
		cmd = get_sub_cmd("nh_nr", "16");
		SW_RTN_ON_NULL_PARAM(cmd);

		if (!strncasecmp(cmd, "quit", 4))
		{
			return SW_BAD_VALUE;
		}
		else if (!strncasecmp(cmd, "help", 4))
		{
			dprintf("usage: integer\n");
			rv = SW_BAD_VALUE;
		}
		else
		{
			rv = cmd_data_check_uint32(cmd, &(entry.nh_nr), sizeof (a_uint32_t));
			if (SW_OK != rv)
				dprintf("usage: integer\n");
			else {
				if(entry.nh_nr > 16) {
					dprintf("usage: integer <= 16\n");
					rv = SW_BAD_VALUE;
				}
			}
		}

	}
	while (talk_mode && (SW_OK != rv));

	do
	{
		cmd = get_sub_cmd("nh_id", NULL);
        	SW_RTN_ON_NULL_PARAM(cmd);

        	if (!strncasecmp(cmd, "quit", 4))
        	{
            		return SW_BAD_VALUE;
        	}
        	else if (!strncasecmp(cmd, "help", 4))
        	{
            		dprintf("usage: the format is xx-xx-xx-xx-xx-xx \n");
            		rv = SW_BAD_VALUE;
        	}
        	else
        	{
            		rv = cmd_data_check_array(cmd, entry.nh_id, entry.nh_nr);
            		if (SW_OK != rv)
                		dprintf("usage: the format is xx-xx-xx-xx-xx-xx \n");
        	}
	}
	while (talk_mode && (SW_OK != rv));

	*(fal_ip_wcmp_t *)val = entry;
	return SW_OK;
}


void
cmd_data_print_ip_wcmp_entry(a_uint8_t * param_name, a_uint32_t * buf, a_uint32_t size)
{
	fal_ip_wcmp_t *entry;
	int i = 0;

    entry = (fal_ip_wcmp_t *) buf;
	dprintf("\n[nh_nr]:0x%x",
            entry->nh_nr);
	dprintf("\n");
	for(i = 0; i < entry->nh_nr; i++) {
		dprintf("[nh_id[%d]]:0x%x ", i, entry->nh_id[i]);
		if(((i+1) % 4) == 0)
			dprintf("\n");
	}
}

sw_error_t
cmd_data_check_ip4_rfs_entry(char *cmd_str, void * val, a_uint32_t size)
{
	a_uint32_t tmp;
	sw_error_t rv;
	fal_ip4_rfs_t entry;

	aos_mem_zero(&entry, sizeof (fal_ip4_rfs_t));

	rv = __cmd_data_check_complex("mac addr", NULL,
                        "usage: the format is xx-xx-xx-xx-xx-xx \n",
                        cmd_data_check_macaddr, &(entry.mac_addr),
                        sizeof (fal_mac_addr_t));
	if (rv)
	{
		return rv;
	}

	rv = __cmd_data_check_complex("ip4 addr", NULL,
                            "usage: the format is xx.xx.xx.xx \n",
                            cmd_data_check_ip4addr, &(entry.ip4_addr),
                            4);
	if (rv)
	{
		return rv;
	}

	rv = __cmd_data_check_complex("vid", "0",
                        "usage: the format is xx \n",
                        cmd_data_check_uint32, &(entry.vid),
                        sizeof (a_uint32_t));
	if (rv)
	{
		return rv;
	}

	rv = __cmd_data_check_complex("loadbalance", "0",
                        "usage: the format is xx \n",
                        cmd_data_check_uint32, &tmp,
                        sizeof (a_uint32_t));
	if (rv)
	{
		return rv;
	}

	entry.load_balance = tmp;
	*(fal_ip4_rfs_t *)val = entry;
	return SW_OK;
}

sw_error_t
cmd_data_check_fdb_rfs(char *cmd_str, void * val, a_uint32_t size)
{
	a_uint32_t tmp;
	sw_error_t rv;
	fal_fdb_rfs_t entry;

	aos_mem_zero(&entry, sizeof (fal_fdb_rfs_t));

	rv = __cmd_data_check_complex("mac addr", NULL,
                        "usage: the format is xx-xx-xx-xx-xx-xx \n",
                        cmd_data_check_macaddr, &(entry.addr),
                        sizeof (fal_mac_addr_t));
	if (rv)
	{
		return rv;
	}

	rv = __cmd_data_check_complex("fid", NULL,
                            "usage: the format is xx\n",
                            cmd_data_check_uint32, &tmp,
                            sizeof (a_uint32_t));
	if (rv)
	{
		return rv;
	}

	entry.fid = tmp;

	rv = __cmd_data_check_complex("loadbalance", "0",
                        "usage: the format is xx \n",
                        cmd_data_check_uint32, &tmp,
                        sizeof (a_uint32_t));
	if (rv)
	{
		return rv;
	}

	entry.load_balance = tmp;

	*(fal_fdb_rfs_t *)val = entry;
	return SW_OK;
}


sw_error_t
cmd_data_check_flow_cookie(char *cmd_str, void * val, a_uint32_t size)
{
	sw_error_t rv;
	fal_flow_cookie_t entry;

	aos_mem_zero(&entry, sizeof (fal_flow_cookie_t));

	rv = __cmd_data_check_complex("proto", "0",
                        "usage: the format is xx \n",
                        cmd_data_check_uint32, &(entry.proto),
                        sizeof (a_uint32_t));
	if (rv)
	{
		return rv;
	}

	rv = __cmd_data_check_complex("src addr", NULL,
                            "usage: the format is xx.xx.xx.xx \n",
                            cmd_data_check_ip4addr, &(entry.src_addr),
                            4);
	if (rv)
	{
		return rv;
	}

	rv = __cmd_data_check_complex("dst addr", NULL,
                            "usage: the format is xx.xx.xx.xx \n",
                            cmd_data_check_ip4addr, &(entry.dst_addr),
                            4);
	if (rv)
	{
		return rv;
	}

	rv = __cmd_data_check_complex("src port", "0",
                        "usage: the format is xx \n",
                        cmd_data_check_uint32, &(entry.src_port),
                        sizeof (a_uint32_t));
	if (rv)
	{
		return rv;
	}

	rv = __cmd_data_check_complex("dst port", "0",
                        "usage: the format is xx \n",
                        cmd_data_check_uint32, &(entry.dst_port),
                        sizeof (a_uint32_t));
	if (rv)
	{
		return rv;
	}

	rv = __cmd_data_check_complex("flow cookie", "0",
                        "usage: the format is xx \n",
                        cmd_data_check_uint32, &(entry.flow_cookie),
                        sizeof (a_uint32_t));
	if (rv)
	{
		return rv;
	}

	*(fal_flow_cookie_t *)val = entry;
	return SW_OK;
}

sw_error_t
cmd_data_check_flow_rfs(char *cmd_str, void * val, a_uint32_t size)
{
	a_uint32_t tmp;
	sw_error_t rv;
	fal_flow_rfs_t entry;

	aos_mem_zero(&entry, sizeof (fal_flow_cookie_t));

	rv = __cmd_data_check_complex("proto", "0",
                        "usage: the format is xx \n",
                        cmd_data_check_uint32, &(entry.proto),
                        sizeof (a_uint32_t));
	if (rv)
	{
		return rv;
	}

	rv = __cmd_data_check_complex("src addr", NULL,
                            "usage: the format is xx.xx.xx.xx \n",
                            cmd_data_check_ip4addr, &(entry.src_addr),
                            4);
	if (rv)
	{
		return rv;
	}

	rv = __cmd_data_check_complex("dst addr", NULL,
                            "usage: the format is xx.xx.xx.xx \n",
                            cmd_data_check_ip4addr, &(entry.dst_addr),
                            4);
	if (rv)
	{
		return rv;
	}

	rv = __cmd_data_check_complex("src port", "0",
                        "usage: the format is xx \n",
                        cmd_data_check_uint32, &(entry.src_port),
                        sizeof (a_uint32_t));
	if (rv)
	{
		return rv;
	}

	rv = __cmd_data_check_complex("dst port", "0",
                        "usage: the format is xx \n",
                        cmd_data_check_uint32, &(entry.dst_port),
                        sizeof (a_uint32_t));
	if (rv)
	{
		return rv;
	}

	rv = __cmd_data_check_complex("flow rfs", "0",
                        "usage: the format is xx \n",
                        cmd_data_check_uint32, &tmp,
                        sizeof (a_uint32_t));
	if (rv)
	{
		return rv;
	}

	entry.load_balance = tmp;


	*(fal_flow_rfs_t *)val = entry;
	return SW_OK;
}

sw_error_t
cmd_data_check_ip6_rfs_entry(char *cmd_str, void * val, a_uint32_t size)
{
	a_uint32_t tmp;
	sw_error_t rv;
	fal_ip6_rfs_t entry;

	aos_mem_zero(&entry, sizeof (fal_ip4_rfs_t));

	rv = __cmd_data_check_complex("mac addr", NULL,
                        "usage: the format is xx-xx-xx-xx-xx-xx \n",
                        cmd_data_check_macaddr, &(entry.mac_addr),
                        sizeof (fal_mac_addr_t));
	if (rv)
	{
		return rv;
	}

	rv = __cmd_data_check_complex("ip6 addr", NULL,
                            "usage: the format is xxxx::xxxx \n",
                            cmd_data_check_ip6addr, &(entry.ip6_addr),
                            16);
	if (rv)
	{
		return rv;
	}

	rv = __cmd_data_check_complex("vid", "0",
                        "usage: the format is xx \n",
                        cmd_data_check_uint32, &(entry.vid),
                        sizeof (a_uint32_t));
	if (rv)
	{
		return rv;
	}

	rv = __cmd_data_check_complex("loadbalance", "0",
                        "usage: the format is xx \n",
                        cmd_data_check_uint32, &tmp,
                        sizeof (a_uint32_t));
	if (rv)
	{
		return rv;
	}
	entry.load_balance = tmp;

	*(fal_ip6_rfs_t *)val = entry;
	return SW_OK;
}


sw_error_t
cmd_data_check_newadr_lrn(char *cmd_str, void * val, a_uint32_t size)
{
	char *cmd;
	fal_vsi_newaddr_lrn_t entry;

	aos_mem_zero(&entry, sizeof (fal_vsi_newaddr_lrn_t));

	cmd_data_check_element("learnstatus_en", "enable", "usage: enable/disable\n",
			cmd_data_check_enable, (cmd, &(entry.lrn_en), sizeof(entry.lrn_en)));

	cmd_data_check_element("learnaction", "forward", "usage: forward/drop/cpycpu/rdtcpu\n",
			cmd_data_check_maccmd, (cmd, &(entry.action), sizeof(entry.action)));

	*(fal_vsi_newaddr_lrn_t *)val = entry;
	return SW_OK;
}

void
cmd_data_print_newaddr_lrn_entry(a_uint8_t * param_name, a_uint32_t * buf, a_uint32_t size)
{
    fal_vsi_newaddr_lrn_t *entry;

    entry = (fal_vsi_newaddr_lrn_t *) buf;
    cmd_data_print_enable("learnstatus_en", &entry->lrn_en, sizeof(entry->lrn_en));
	dprintf("\n");
	cmd_data_print_maccmd("learnaction", &entry->action, sizeof(entry->action));

    return;
}

sw_error_t
cmd_data_check_stamove(char *cmd_str, void * val, a_uint32_t size)
{
	char *cmd;
	fal_vsi_stamove_t entry;

	aos_mem_zero(&entry, sizeof (fal_vsi_stamove_t));

	cmd_data_check_element("stationmove_en", "enable", "usage: enable/disable\n",
			cmd_data_check_enable, (cmd, &(entry.stamove_en), sizeof(entry.stamove_en)));

	cmd_data_check_element("stationmove_action", "forward", "usage: forward/drop/cpycpu/rdtcpu\n",
			cmd_data_check_maccmd, (cmd, &(entry.action), sizeof(entry.action)));

	*(fal_vsi_stamove_t *)val = entry;
	return SW_OK;
}

void
cmd_data_print_stamove_entry(a_uint8_t * param_name, a_uint32_t * buf, a_uint32_t size)
{
    fal_vsi_stamove_t *entry;

    entry = (fal_vsi_stamove_t *) buf;
    cmd_data_print_enable("stationmove_en", &entry->stamove_en, sizeof(entry->stamove_en));
	dprintf("\n");
    cmd_data_print_maccmd("stationmove_action", &entry->action, sizeof(entry->action));

    return;
}

sw_error_t
cmd_data_check_vsi_member(char *cmd_str, void * val, a_uint32_t size)
{
	sw_error_t rv;
	fal_vsi_member_t entry;

	aos_mem_zero(&entry, sizeof (fal_vsi_member_t));

	rv = __cmd_data_check_complex("membership", 0,
                        "usage: Bit0-port0 Bit1-port1 ....\n",
                        cmd_data_check_pbmp, &(entry.member_ports),
                        sizeof (a_uint32_t));
	if (rv)
		return rv;

	rv = __cmd_data_check_complex("unknown_unicast_membership", 0,
                        "usage: Bit0-port0 Bit1-port1 ....\n",
                        cmd_data_check_pbmp, &(entry.uuc_ports),
                        sizeof (a_uint32_t));
	if (rv)
		return rv;

	rv = __cmd_data_check_complex("unknown_multicast_membership", 0,
                        "usage: Bit0-port0 Bit1-port1 ....\n",
                        cmd_data_check_pbmp, &(entry.umc_ports),
                        sizeof (a_uint32_t));
	if (rv)
		return rv;

	rv = __cmd_data_check_complex("broadcast_membership", 0,
                        "usage: Bit0-port0 Bit1-port1 ....\n",
                        cmd_data_check_pbmp, &(entry.bc_ports),
                        sizeof (a_uint32_t));
	if (rv)
		return rv;

	*(fal_vsi_member_t *)val = entry;
	return SW_OK;
}

void
cmd_data_print_vsi_member_entry(a_uint8_t * param_name, a_uint32_t * buf, a_uint32_t size)
{
    fal_vsi_member_t *entry;

    entry = (fal_vsi_member_t *) buf;
    dprintf("\n");
    dprintf("[membership]:0x%x\n", entry->member_ports);
    dprintf("[unknown_unicast_membership]:0x%x\n", entry->uuc_ports);
    dprintf("[unknown_multicast_membership]:0x%x\n", entry->umc_ports);
    dprintf("[broadcast_membership]:0x%x\n", entry->bc_ports);
    return;
}

void
cmd_data_print_vsi_counter(a_uint8_t * param_name, a_uint32_t * buf, a_uint32_t size)
{
    fal_vsi_counter_t *entry;

    entry = (fal_vsi_counter_t *) buf;
    dprintf("\n");
    dprintf("[rx_bytes]:%lld\n", entry->rx_byte_counter);
    dprintf("[rx_packets]:%d\n", entry->rx_packet_counter);
    dprintf("[tx_bytes]:%lld\n", entry->tx_byte_counter);
    dprintf("[tx_packets]:%d\n", entry->tx_packet_counter);
    dprintf("[fwd_bytes]:%lld\n", entry->fwd_byte_counter);
    dprintf("[fwd_packets]:%d\n", entry->fwd_packet_counter);
    dprintf("[drop_bytes]:%lld\n", entry->drop_byte_counter);
    dprintf("[drop_packets]:%d\n", entry->drop_packet_counter);
    
    return;
}

sw_error_t
cmd_data_check_intf(char *cmd_str, void * val, a_uint32_t size)
{
    char *cmd;
    a_uint32_t tmp;
    sw_error_t rv;
    fal_intf_entry_t entry;

    aos_mem_zero(&entry, sizeof (fal_intf_entry_t));

    do
    {
        cmd = get_sub_cmd("mru", "0x5dc");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: mru \n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_uint16(cmd, &tmp, sizeof (a_uint32_t));
            if (SW_OK != rv)
            {
                dprintf("usage: mru \n");
            }
            else
            {
                entry.mru = tmp;
            }
        }
    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("mtu", "0x5dc");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: mtu \n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_uint16(cmd, &tmp, sizeof (a_uint32_t));
            if (SW_OK != rv)
            {
                dprintf("usage: mtu \n");
            }
            else
            {
                entry.mtu = tmp;
            }
        }
    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("ttl_dec_bypass_en", "no");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: <yes/no/y/n>\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_confirm(cmd, A_FALSE, &(entry.ttl_dec_bypass_en), sizeof (a_bool_t));
            if (SW_OK != rv)
                dprintf("usage: <yes/no/y/n>\n");
        }
    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("ipv4_uc_route_en", "yes");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: <yes/no/y/n>\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_confirm(cmd, A_TRUE, &(entry.ipv4_uc_route_en), sizeof (a_bool_t));
            if (SW_OK != rv)
                dprintf("usage: <yes/no/y/n>\n");
        }
    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("ipv6_uc_route_en", "yes");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: <yes/no/y/n>\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_confirm(cmd, A_TRUE, &(entry.ipv6_uc_route_en), sizeof (a_bool_t));
            if (SW_OK != rv)
                dprintf("usage: <yes/no/y/n>\n");
        }
    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("icmp_trigger_en", "no");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: <yes/no/y/n>\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_confirm(cmd, A_FALSE, &(entry.icmp_trigger_en), sizeof (a_bool_t));
            if (SW_OK != rv)
                dprintf("usage: <yes/no/y/n>\n");
        }
    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("ttl_exceed_action", "forward");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: <forward/drop/cpycpu/rdtcpu>\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_maccmd(cmd, &(entry.ttl_exceed_action),
                                       sizeof (fal_fwd_cmd_t));
            if (SW_OK != rv)
                dprintf("usage: <forward/drop/cpycpu/rdtcpu>\n");
        }

    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("ttl_exceed_deacclr_en", "no");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: <yes/no/y/n>\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_confirm(cmd, A_FALSE, &(entry.ttl_exceed_deacclr_en), sizeof (a_bool_t));
            if (SW_OK != rv)
                dprintf("usage: <yes/no/y/n>\n");
        }
    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("mac_addr_bitmap", "0");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: mac bitmap \n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_uint8(cmd, &tmp, sizeof (a_uint32_t));
            if (SW_OK != rv)
            {
                dprintf("usage: mac bitmap \n");
            }
            else
            {
                entry.mac_addr_bitmap = tmp;
            }
        }
    }
    while (talk_mode && (SW_OK != rv));

    cmd_data_check_element("mac_addr", NULL,
                           "usage: the format is xx-xx-xx-xx-xx-xx \n",
                           cmd_data_check_macaddr, (cmd, &(entry.mac_addr),
                                   sizeof (fal_mac_addr_t)));

    *(fal_intf_entry_t *)val = entry;
    return SW_OK;
}

sw_error_t
cmd_data_check_flow_age(char *cmd_str, void * val, a_uint32_t size)
{
	char *cmd;
    a_uint32_t tmp;
    sw_error_t rv;
    fal_flow_age_timer_t entry;

    aos_mem_zero(&entry, sizeof (fal_flow_age_timer_t));

    do
    {
        cmd = get_sub_cmd("age_time", "0");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: age time \n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_uint16(cmd, &tmp, sizeof (a_uint32_t));
            if (SW_OK != rv)
            {
                dprintf("usage: age time \n");
            }
            else
            {
                entry.age_time = tmp;
            }
        }
    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("age_unit", "0");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: age unit \n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_uint16(cmd, &tmp, sizeof (a_uint32_t));
            if (SW_OK != rv)
            {
                dprintf("usage: age unit \n");
            }
            else
            {
                entry.unit = tmp;
            }
        }
    }
    while (talk_mode && (SW_OK != rv));

    *(fal_flow_age_timer_t *)val = entry;
    return SW_OK;
}


sw_error_t
cmd_data_check_ac_dynamic_thresh(char *cmd_str, void * val, a_uint32_t size)
{
    char *cmd;
    a_uint32_t tmp;
    sw_error_t rv;
    fal_ac_dynamic_threshold_t entry;

    aos_mem_zero(&entry, sizeof (fal_ac_dynamic_threshold_t));

    do
    {
        cmd = get_sub_cmd("color_en", "no");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: <yes/no/y/n>\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_confirm(cmd, A_FALSE, &(entry.color_enable), sizeof (a_bool_t));
            if (SW_OK != rv)
                dprintf("usage: <yes/no/y/n>\n");
        }
    }
    while (talk_mode && (SW_OK != rv));


    do
    {
        cmd = get_sub_cmd("wred_en", "no");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: <yes/no/y/n>\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_confirm(cmd, A_FALSE, &(entry.wred_enable), sizeof (a_bool_t));
            if (SW_OK != rv)
                dprintf("usage: <yes/no/y/n>\n");
        }
    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("shared_weight", "0");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: weight \n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_uint8(cmd, &tmp, sizeof (a_uint32_t));
            if (SW_OK != rv)
            {
                dprintf("usage: weight\n");
            }
            else
            {
                entry.shared_weight = tmp;
            }
        }
    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("green_min_off", "0");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: green min offset \n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_uint16(cmd, &tmp, sizeof (a_uint32_t));
            if (SW_OK != rv)
            {
                dprintf("usage: green min offset \n");
            }
            else
            {
                entry.green_min_off = tmp;
            }
        }
    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("yel_max_off", "0");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: yel max offset \n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_uint16(cmd, &tmp, sizeof (a_uint32_t));
            if (SW_OK != rv)
            {
                dprintf("usage: green max offset \n");
            }
            else
            {
                entry.yel_max_off = tmp;
            }
        }
    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("yel_min_off", "0");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: yel min offset \n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_uint16(cmd, &tmp, sizeof (a_uint32_t));
            if (SW_OK != rv)
            {
                dprintf("usage: green min offset \n");
            }
            else
            {
                entry.yel_min_off = tmp;
            }
        }
    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("red_max_off", "0");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: red max offset \n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_uint16(cmd, &tmp, sizeof (a_uint32_t));
            if (SW_OK != rv)
            {
                dprintf("usage: red max offset \n");
            }
            else
            {
                entry.red_max_off = tmp;
            }
        }
    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("red_min_off", "0");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: red min offset \n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_uint16(cmd, &tmp, sizeof (a_uint32_t));
            if (SW_OK != rv)
            {
                dprintf("usage: red min offset \n");
            }
            else
            {
                entry.red_min_off = tmp;
            }
        }
    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("green_resume_off", "0");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: green resume offset \n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_uint16(cmd, &tmp, sizeof (a_uint32_t));
            if (SW_OK != rv)
            {
                dprintf("usage: green resume offset \n");
            }
            else
            {
                entry.green_resume_off = tmp;
            }
        }
    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("yel_resume_off", "0");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: yellow resume offset \n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_uint16(cmd, &tmp, sizeof (a_uint32_t));
            if (SW_OK != rv)
            {
                dprintf("usage: yellow resume offset \n");
            }
            else
            {
                entry.yel_resume_off = tmp;
            }
        }
    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("red_resume_off", "0");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: red resume offset \n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_uint16(cmd, &tmp, sizeof (a_uint32_t));
            if (SW_OK != rv)
            {
                dprintf("usage: red resume offset \n");
            }
            else
            {
                entry.red_resume_off = tmp;
            }
        }
    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("ceiling", "0");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: shared ceiling \n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_uint16(cmd, &tmp, sizeof (a_uint32_t));
            if (SW_OK != rv)
            {
                dprintf("usage: shared ceiling \n");
            }
            else
            {
                entry.ceiling = tmp;
            }
        }
    }
    while (talk_mode && (SW_OK != rv));
	
    *(fal_ac_dynamic_threshold_t *)val = entry;
    return SW_OK;
}

sw_error_t
cmd_data_check_ac_group_buff(char *cmd_str, void * val, a_uint32_t size)
{
    char *cmd;
    a_uint32_t tmp;
    sw_error_t rv;
    fal_ac_group_buffer_t entry;

    do
    {
        cmd = get_sub_cmd("prealloc_buffer", "0");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: prealloc buffer \n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_uint16(cmd, &tmp, sizeof (a_uint32_t));
            if (SW_OK != rv)
            {
                dprintf("usage: prealloc buffer \n");
            }
            else
            {
                entry.prealloc_buffer = tmp;
            }
        }
    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("total_buffer", "0");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: total buffer \n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_uint16(cmd, &tmp, sizeof (a_uint32_t));
            if (SW_OK != rv)
            {
                dprintf("usage: total buffer \n");
            }
            else
            {
                entry.total_buffer = tmp;
            }
        }
    }
    while (talk_mode && (SW_OK != rv));

    *(fal_ac_group_buffer_t *)val = entry;
    return SW_OK;
}

sw_error_t
cmd_data_check_ac_ctrl(char *cmd_str, void * val, a_uint32_t size)
{
    char *cmd;
    sw_error_t rv;
    fal_ac_ctrl_t entry;

    do
    {
        cmd = get_sub_cmd("ac_en", "yes");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: <yes/no/y/n>\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_confirm(cmd, A_TRUE, &(entry.ac_en), sizeof (a_bool_t));
            if (SW_OK != rv)
                dprintf("usage: <yes/no/y/n>\n");
        }
    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("ac_fc-en", "yes");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: <yes/no/y/n>\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_confirm(cmd, A_TRUE, &(entry.ac_fc_en), sizeof (a_bool_t));
            if (SW_OK != rv)
                dprintf("usage: <yes/no/y/n>\n");
        }
    }
    while (talk_mode && (SW_OK != rv));

    *(fal_ac_ctrl_t *)val = entry;
    return SW_OK;
}

sw_error_t
cmd_data_check_ac_obj(char *cmd_str, void * val, a_uint32_t size)
{
    char *cmd;
    sw_error_t rv;
    fal_ac_obj_t entry;

    do
    {
        cmd = get_sub_cmd("obj_type", "0");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: 0 for queue and 1 for group \n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_uint32(cmd, &(entry.type), sizeof (a_uint32_t));
            if (SW_OK != rv)
                dprintf("usage: 0 for queue and 1 for group \n");
        }
    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("obj_id", "0");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: obj id \n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_uint32(cmd, &(entry.obj_id), sizeof (a_uint32_t));
            if (SW_OK != rv)
                dprintf("usage: obj id \n");
        }
    }
    while (talk_mode && (SW_OK != rv));

    *(fal_ac_obj_t *)val = entry;
    return SW_OK;
}

sw_error_t
cmd_data_check_ac_static_thresh(char *cmd_str, void * val, a_uint32_t size)
{
    char *cmd;
    a_uint32_t tmp;
    sw_error_t rv;
    fal_ac_static_threshold_t entry;

    aos_mem_zero(&entry, sizeof (fal_ac_static_threshold_t));

    do
    {
        cmd = get_sub_cmd("color_en", "no");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: <yes/no/y/n>\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_confirm(cmd, A_FALSE, &(entry.color_enable), sizeof (a_bool_t));
            if (SW_OK != rv)
                dprintf("usage: <yes/no/y/n>\n");
        }
    }
    while (talk_mode && (SW_OK != rv));


    do
    {
        cmd = get_sub_cmd("wred_en", "no");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: <yes/no/y/n>\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_confirm(cmd, A_FALSE, &(entry.wred_enable), sizeof (a_bool_t));
            if (SW_OK != rv)
                dprintf("usage: <yes/no/y/n>\n");
        }
    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("green_max", "0");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: green max \n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_uint16(cmd, &tmp, sizeof (a_uint32_t));
            if (SW_OK != rv)
            {
                dprintf("usage: group id\n");
            }
            else
            {
                entry.green_max = tmp;
            }
        }
    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("green_min_off", "0");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: green min offset \n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_uint16(cmd, &tmp, sizeof (a_uint32_t));
            if (SW_OK != rv)
            {
                dprintf("usage: green min offset \n");
            }
            else
            {
                entry.green_min_off = tmp;
            }
        }
    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("yel_max_off", "0");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: yel max offset \n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_uint16(cmd, &tmp, sizeof (a_uint32_t));
            if (SW_OK != rv)
            {
                dprintf("usage: yel max offset \n");
            }
            else
            {
                entry.yel_max_off = tmp;
            }
        }
    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("yel_min_off", "0");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: yel min offset \n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_uint16(cmd, &tmp, sizeof (a_uint32_t));
            if (SW_OK != rv)
            {
                dprintf("usage: yel min offset \n");
            }
            else
            {
                entry.yel_min_off = tmp;
            }
        }
    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("red_max_off", "0");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: red max offset \n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_uint16(cmd, &tmp, sizeof (a_uint32_t));
            if (SW_OK != rv)
            {
                dprintf("usage: red max offset \n");
            }
            else
            {
                entry.red_max_off = tmp;
            }
        }
    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("red_min_off", "0");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: red min offset \n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_uint16(cmd, &tmp, sizeof (a_uint32_t));
            if (SW_OK != rv)
            {
                dprintf("usage: red min offset \n");
            }
            else
            {
                entry.red_min_off = tmp;
            }
        }
    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("green_resume_off", "0");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: green resume offset \n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_uint16(cmd, &tmp, sizeof (a_uint32_t));
            if (SW_OK != rv)
            {
                dprintf("usage: green resume offset \n");
            }
            else
            {
                entry.green_resume_off = tmp;
            }
        }
    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("yel_resume_off", "0");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: yellow resume offset \n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_uint16(cmd, &tmp, sizeof (a_uint32_t));
            if (SW_OK != rv)
            {
                dprintf("usage: yellow resume offset \n");
            }
            else
            {
                entry.yel_resume_off = tmp;
            }
        }
    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("red_resume_off", "0");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: red resume offset \n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_uint16(cmd, &tmp, sizeof (a_uint32_t));
            if (SW_OK != rv)
            {
                dprintf("usage: red resume offset \n");
            }
            else
            {
                entry.red_resume_off = tmp;
            }
        }
    }
    while (talk_mode && (SW_OK != rv));
	
    *(fal_ac_static_threshold_t *)val = entry;
    return SW_OK;
}


sw_error_t
cmd_data_check_ip_global(char *cmd_str, void * val, a_uint32_t size)
{
    char *cmd;
    a_uint32_t tmp;
    sw_error_t rv;
    fal_ip_global_cfg_t entry;

    aos_mem_zero(&entry, sizeof (fal_ip_global_cfg_t));

    do
    {
        cmd = get_sub_cmd("mru_fail_action", "forward");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: <forward/drop/cpycpu/rdtcpu>\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_maccmd(cmd, &(entry.mru_fail_action),
                                       sizeof (fal_fwd_cmd_t));
            if (SW_OK != rv)
                dprintf("usage: <forward/drop/cpycpu/rdtcpu>\n");
        }

    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("mru_deacclr_en", "no");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: <yes/no/y/n>\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_confirm(cmd, A_FALSE, &(entry.mru_deacclr_en),
                                       sizeof (a_bool_t));
            if (SW_OK != rv)
                dprintf("usage: <yes/no/y/n>\n");
        }

    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("mtu_fail_action", "forward");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: <forward/drop/cpycpu/rdtcpu>\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_maccmd(cmd, &(entry.mtu_fail_action),
                                       sizeof (fal_fwd_cmd_t));
            if (SW_OK != rv)
                dprintf("usage: <forward/drop/cpycpu/rdtcpu>\n");
        }

    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("mtu_deacclr_en", "no");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: <yes/no/y/n>\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_confirm(cmd, A_FALSE, &(entry.mtu_deacclr_en),
                                       sizeof (a_bool_t));
            if (SW_OK != rv)
                dprintf("usage: <yes/no/y/n>\n");
        }

    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("mtu_nonfrag_fail_action", "forward");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: <forward/drop/cpycpu/rdtcpu>\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_maccmd(cmd, &(entry.mtu_nonfrag_fail_action),
                                       sizeof (fal_fwd_cmd_t));
            if (SW_OK != rv)
                dprintf("usage: <forward/drop/cpycpu/rdtcpu>\n");
        }

    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("mtu_nonfrag_deacclr", "no");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: <yes/no/y/n>\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_confirm(cmd, A_FALSE, &(entry.mtu_df_deacclr_en),
                                       sizeof (a_bool_t));
            if (SW_OK != rv)
                dprintf("usage: <yes/no/y/n>\n");
        }

    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("prefix_bc_action", "forward");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: <forward/drop/cpycpu/rdtcpu>\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_maccmd(cmd, &(entry.prefix_bc_action),
                                       sizeof (fal_fwd_cmd_t));
            if (SW_OK != rv)
                dprintf("usage: <forward/drop/cpycpu/rdtcpu>\n");
        }

    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("prefix_bc_deacclr", "no");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: <yes/no/y/n>\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_confirm(cmd, A_FALSE, &(entry.prefix_deacclr_en),
                                       sizeof (a_bool_t));
            if (SW_OK != rv)
                dprintf("usage: <yes/no/y/n>\n");
        }

    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("icmp_rdt_action", "forward");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: <forward/drop/cpycpu/rdtcpu>\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_maccmd(cmd, &(entry.icmp_rdt_action),
                                       sizeof (fal_fwd_cmd_t));
            if (SW_OK != rv)
                dprintf("usage: <forward/drop/cpycpu/rdtcpu>\n");
        }

    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("icmp_rdt_deacclr_en", "no");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: <yes/no/y/n>\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_confirm(cmd, A_FALSE, &(entry.icmp_rdt_deacclr_en),
                                       sizeof (a_bool_t));
            if (SW_OK != rv)
                dprintf("usage: <yes/no/y/n>\n");
        }

    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("hash_mode_0", "0");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: hash mode\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_uint8(cmd, &tmp,
                                       sizeof (a_uint32_t));
            if (SW_OK != rv)
            {
                dprintf("usage: hash mode\n");
            }
            else
            {
                entry.hash_mode_0 = tmp;
            }
        }

    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("hash_mode_1", "0");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: hash mode\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_uint8(cmd, &tmp,
                                       sizeof (a_uint32_t));
            if (SW_OK != rv)
                dprintf("usage: hash mode\n");
            else
                entry.hash_mode_1 = tmp;
        }

    }
    while (talk_mode && (SW_OK != rv));

    *(fal_ip_global_cfg_t *)val = entry;
    return SW_OK;

}

void
cmd_data_print_ip_global(a_uint8_t * param_name, a_uint32_t * buf, a_uint32_t size)
{
    fal_ip_global_cfg_t *entry;

    entry = (fal_ip_global_cfg_t *) buf;

    dprintf("\n[mru_fail_action]:0x%x [mru_deacclr_en]:0x%x [mtu_fail_action]:0x%x [mtu_deacclr_en]:0x%x [mtu_nonfrag_fail_action]:0x%x ",
			entry->mru_fail_action, entry->mru_deacclr_en, entry->mtu_fail_action, entry->mtu_deacclr_en, entry->mtu_nonfrag_fail_action);
    dprintf("\n[mtu_df_deacclr_en]:0x%x [prefix_bc_action]:0x%x [prefix_bc_deacclr_en]:0x%x [icmp_rdt_action]:0x%x [icmp_rdt_deacclr]:0x%x ",
			entry->mtu_df_deacclr_en, entry->prefix_bc_action, entry->prefix_deacclr_en, entry->icmp_rdt_action, entry->icmp_rdt_deacclr_en);
    dprintf("\n[hash_mode_0]:0x%x [hash_mode_1]:0x%x ",
			entry->hash_mode_0, entry->hash_mode_1);

}

sw_error_t
cmd_data_check_l3_parser(char *cmd_str, void * val, a_uint32_t size)
{
    char *cmd;
    a_uint32_t tmp;
    sw_error_t rv;
    fal_l3_excep_parser_ctrl entry;

    aos_mem_zero(&entry, sizeof (fal_l3_excep_parser_ctrl));

    do
    {
        cmd = get_sub_cmd("small_ip4ttl", "0");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: small ttl value\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_uint8(cmd, &tmp,
                                       sizeof (a_uint32_t));
            if (SW_OK != rv)
                dprintf("usage: small ttl value\n");
            else
                entry.small_ip4ttl = tmp;
        }

    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("small_ip6hoplimit", "0");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: small hop limit value\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_uint8(cmd, &tmp,
                                       sizeof (a_uint32_t));
            if (SW_OK != rv)
                dprintf("usage: small hop limit value\n");
            else
                entry.small_ip6hoplimit = tmp;
        }

    }
    while (talk_mode && (SW_OK != rv));

    *(fal_l3_excep_parser_ctrl *)val = entry;
    return SW_OK;

}

void
cmd_data_print_l3_parser(a_uint8_t * param_name, a_uint32_t * buf, a_uint32_t size)
{
    fal_l3_excep_parser_ctrl *entry;

    entry = (fal_l3_excep_parser_ctrl *) buf;

    dprintf("\n[small_ip4ttl]:0x%x [small_ip6hoplimit]:0x%x ",
			entry->small_ip4ttl, entry->small_ip6hoplimit);
}

sw_error_t
cmd_data_check_l4_parser(char *cmd_str, void * val, a_uint32_t size)
{
    char *cmd;
    a_uint32_t tmp;
    sw_error_t rv;
    fal_l4_excep_parser_ctrl entry;

    aos_mem_zero(&entry, sizeof (fal_l4_excep_parser_ctrl));

    do
    {
        cmd = get_sub_cmd("tcp_flags0", "0");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: tcp flags value\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_uint8(cmd, &tmp,
                                       sizeof (tmp));
            if (SW_OK != rv)
                dprintf("usage: tcp flags value\n");
	entry.tcp_flags[0] = tmp;
        }

    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("tcp_flags_mask0", "0");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: tcp flags mask value\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_uint8(cmd, &tmp,
                                       sizeof (tmp));
            if (SW_OK != rv)
                dprintf("usage: tcp flags maskvalue\n");
	entry.tcp_flags_mask[0] = tmp;
        }

    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("tcp_flags1", "0");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: tcp flags value\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_uint8(cmd, &tmp,
                                       sizeof (tmp));
            if (SW_OK != rv)
                dprintf("usage: tcp flags value\n");
	entry.tcp_flags[1] = tmp;
        }

    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("tcp_flags_mask1", "0");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: tcp flags mask value\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_uint8(cmd, &tmp,
                                       sizeof (tmp));
            if (SW_OK != rv)
                dprintf("usage: tcp flags maskvalue\n");
	entry.tcp_flags_mask[1] = tmp;
        }

    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("tcp_flags2", "0");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: tcp flags value\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_uint8(cmd, &tmp,
                                       sizeof (tmp));
            if (SW_OK != rv)
                dprintf("usage: tcp flags value\n");
	entry.tcp_flags[2] = tmp;
        }

    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("tcp_flags_mask2", "0");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: tcp flags mask value\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_uint8(cmd, &tmp,
                                       sizeof (tmp));
            if (SW_OK != rv)
                dprintf("usage: tcp flags maskvalue\n");
	entry.tcp_flags_mask[2] = tmp;
        }

    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("tcp_flags3", "0");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: tcp flags value\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_uint8(cmd, &tmp,
                                       sizeof (tmp));
            if (SW_OK != rv)
                dprintf("usage: tcp flags value\n");
	entry.tcp_flags[3] = tmp;
        }

    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("tcp_flags_mask3", "0");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: tcp flags mask value\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_uint8(cmd, &tmp,
                                       sizeof (tmp));
            if (SW_OK != rv)
                dprintf("usage: tcp flags maskvalue\n");
	entry.tcp_flags_mask[3] = tmp;
        }

    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("tcp_flags4", "0");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: tcp flags value\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_uint8(cmd, &tmp,
                                       sizeof (tmp));
            if (SW_OK != rv)
                dprintf("usage: tcp flags value\n");
	entry.tcp_flags[4] = tmp;
        }

    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("tcp_flags_mask4", "0");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: tcp flags mask value\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_uint8(cmd, &tmp,
                                       sizeof (tmp));
            if (SW_OK != rv)
                dprintf("usage: tcp flags maskvalue\n");
	entry.tcp_flags_mask[4] = tmp;
        }

    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("tcp_flags5", "0");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: tcp flags value\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_uint8(cmd, &tmp,
                                       sizeof (tmp));
            if (SW_OK != rv)
                dprintf("usage: tcp flags value\n");
	entry.tcp_flags[5] = tmp;
        }

    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("tcp_flags_mask5", "0");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: tcp flags mask value\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_uint8(cmd, &tmp,
                                       sizeof (tmp));
            if (SW_OK != rv)
                dprintf("usage: tcp flags maskvalue\n");
	entry.tcp_flags_mask[5]= tmp;
        }

    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("tcp_flags6", "0");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: tcp flags value\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_uint8(cmd, &tmp,
                                       sizeof (tmp));
            if (SW_OK != rv)
                dprintf("usage: tcp flags value\n");
	entry.tcp_flags[6] = tmp;
        }

    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("tcp_flags_mask6", "0");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: tcp flags mask value\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_uint8(cmd, &tmp,
                                       sizeof (tmp));
            if (SW_OK != rv)
                dprintf("usage: tcp flags maskvalue\n");
	entry.tcp_flags_mask[6] = tmp;
        }

    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("tcp_flags7", "0");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: tcp flags value\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_uint8(cmd, &tmp,
                                       sizeof (tmp));
            if (SW_OK != rv)
                dprintf("usage: tcp flags value\n");
	entry.tcp_flags[7] = tmp;
        }

    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("tcp_flags_mask7", "0");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: tcp flags mask value\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_uint8(cmd, &tmp,
                                       sizeof (tmp));
            if (SW_OK != rv)
                dprintf("usage: tcp flags maskvalue\n");
	entry.tcp_flags_mask[7] = tmp;
        }

    }
    while (talk_mode && (SW_OK != rv));


    *(fal_l4_excep_parser_ctrl *)val = entry;
    return SW_OK;

}

void
cmd_data_print_l4_parser(a_uint8_t * param_name, a_uint32_t * buf, a_uint32_t size)
{
    fal_l4_excep_parser_ctrl *entry;
    a_uint8_t i = 0;

    entry = (fal_l4_excep_parser_ctrl *) buf;
    for (i = 0; i < 8; i++)
    	dprintf("\n[tcp_flags%d]:0x%x [tcp_flags_mask%d]:0x%x ",
			i, entry->tcp_flags[i], i, entry->tcp_flags_mask[i]);
}

sw_error_t
cmd_data_check_exp_ctrl(char *cmd_str, void * val, a_uint32_t size)
{
    char *cmd;
    sw_error_t rv;
    fal_l3_excep_ctrl_t entry;

    aos_mem_zero(&entry, sizeof (fal_l3_excep_ctrl_t));

    do
    {
        cmd = get_sub_cmd("excep_cmd", "forward");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: <forward/drop/cpycpu/rdtcpu>\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_maccmd(cmd, &(entry.cmd),
                                       sizeof (fal_fwd_cmd_t));
            if (SW_OK != rv)
                dprintf("usage: <forward/drop/cpycpu/rdtcpu>\n");
        }

    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("deacclr_en", "0");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: 0 for disable and 1 for enable\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_uint8(cmd, &(entry.deacclr_en),
                                       sizeof (a_uint8_t));
            if (SW_OK != rv)
                dprintf("usage: 0 for disable and 1 for enable\n");
        }

    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("l3route_only_en", "0");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: 0 for disable and 1 for enable\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_uint8(cmd, &(entry.l3route_only_en),
                                       sizeof (a_uint8_t));
            if (SW_OK != rv)
                dprintf("usage: 0 for disable and 1 for enable\n");
        }

    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("l2fwd_onl_en", "0");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: 0 for disable and 1 for enable\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_uint8(cmd, &(entry.l2fwd_only_en),
                                       sizeof (a_uint8_t));
            if (SW_OK != rv)
                dprintf("usage: 0 for disable and 1 for enable\n");
        }

    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("l2flow_en", "0");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: 0 for disable and 1 for enable\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_uint8(cmd, &(entry.l2flow_en),
                                       sizeof (a_uint8_t));
            if (SW_OK != rv)
                dprintf("usage: 0 for disable and 1 for enable\n");
        }

    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("l3flow_en", "0");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: 0 for disable and 1 for enable\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_uint8(cmd, &(entry.l3flow_en),
                                       sizeof (a_uint8_t));
            if (SW_OK != rv)
                dprintf("usage: 0 for disable and 1 for enable\n");
        }

    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("multicast_en", "0");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: 0 for disable and 1 for enable\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_uint8(cmd, &(entry.multicast_en),
                                       sizeof (a_uint8_t));
            if (SW_OK != rv)
                dprintf("usage: 0 for disable and 1 for enable\n");
        }

    }
    while (talk_mode && (SW_OK != rv));

    *(fal_l3_excep_ctrl_t *)val = entry;
    return SW_OK;
}

void
cmd_data_print_exp_ctrl(a_uint8_t * param_name, a_uint32_t * buf, a_uint32_t size)
{
    fal_l3_excep_ctrl_t *entry;

    entry = (fal_l3_excep_ctrl_t *) buf;
    
    dprintf("\n[except_cmd]:0x%x [deacclr_en]:0x%x [l3route_only_en]:0x%x [l2fwd_only_en]:0x%x ",
			entry->cmd, entry->deacclr_en, entry->l3route_only_en, entry->l2fwd_only_en);
    dprintf("\n[l3flow_en]:0x%x [l2flow_en]:0x%x [multicast_en]:0x%x ",
			entry->l3flow_en, entry->l2flow_en, entry->multicast_en);
}

sw_error_t
cmd_data_check_port_group(char *cmd_str, void * val, a_uint32_t size)
{
    char *cmd;
    a_uint32_t tmp;
    sw_error_t rv;
    fal_qos_group_t entry;

    aos_mem_zero(&entry, sizeof (fal_qos_group_t));

    do
    {
        cmd = get_sub_cmd("pcp_group", "0");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: group\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_uint8(cmd, &tmp,
                                       sizeof (a_uint32_t));
            if (SW_OK != rv)
            {
                dprintf("usage: group\n");
            }
            else
            {
                entry.pcp_group = tmp;
            }
        }

    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("dscp_group", "0");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: group\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_uint8(cmd, &tmp,
                                       sizeof (a_uint32_t));
            if (SW_OK != rv)
            {
                dprintf("usage: group\n");
            }
            else
            {
                entry.dscp_group = tmp;
            }
        }

    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("flow_group", "0");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: group\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_uint8(cmd, &tmp,
                                       sizeof (a_uint32_t));
            if (SW_OK != rv)
            {
                dprintf("usage: group\n");
            }
            else
            {
                entry.flow_group = tmp;
            }
        }

    }
    while (talk_mode && (SW_OK != rv));

    *(fal_qos_group_t *)val = entry;
    return SW_OK;
}

void
cmd_data_print_port_group(a_uint8_t * param_name, a_uint32_t * buf, a_uint32_t size)
{
    fal_qos_group_t *entry;

    entry = (fal_qos_group_t *) buf;
    
    dprintf("\n[pcp_group]:0x%x [dscp_group]:0x%x [flow_group]:0x%x ",
			entry->pcp_group, entry->dscp_group, entry->flow_group);
}

sw_error_t
cmd_data_check_port_pri(char *cmd_str, void * val, a_uint32_t size)
{
    char *cmd;
    a_uint32_t tmp;
    sw_error_t rv;
    fal_qos_pri_precedence_t entry;

    aos_mem_zero(&entry, sizeof (fal_qos_pri_precedence_t));

    do
    {
        cmd = get_sub_cmd("pcp_pri_prece", "0");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: priority\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_uint8(cmd, &tmp,
                                       sizeof (a_uint32_t));
            if (SW_OK != rv)
            {
                dprintf("usage: priority\n");
            }
            else
            {
                entry.pcp_pri = tmp;
            }
        }

    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("dscp_pri_prece", "0");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: priority\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_uint8(cmd, &tmp,
                                       sizeof (a_uint32_t));
            if (SW_OK != rv)
            {
                dprintf("usage: priority\n");
            }
            else
            {
                entry.dscp_pri = tmp;
            }
        }

    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("preheader_pri_prece", "0");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: priority\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_uint8(cmd, &tmp,
                                       sizeof (a_uint32_t));
            if (SW_OK != rv)
            {
                dprintf("usage: priority\n");
            }
            else
            {
                entry.preheader_pri = tmp;
            }
        }

    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("flow_pri_prece", "0");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: priority\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_uint8(cmd, &tmp,
                                       sizeof (a_uint32_t));
            if (SW_OK != rv)
            {
                dprintf("usage: priority\n");
            }
            else
            {
                entry.flow_pri = tmp;
            }
        }

    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("acl_pri_prece", "0");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: priority\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_uint8(cmd, &tmp,
                                       sizeof (a_uint32_t));
            if (SW_OK != rv)
            {
                dprintf("usage: priority\n");
            }
            else
            {
                entry.acl_pri = tmp;
            }
        }

    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("post_acl_pri_prece", "0");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: priority\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_uint8(cmd, &tmp,
                                       sizeof (a_uint32_t));
            if (SW_OK != rv)
            {
                dprintf("usage: priority\n");
            }
            else
            {
                entry.post_acl_pri = tmp;
            }
        }

    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("pcp_pri_force", "no");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: <yes/no/y/n>\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_confirm(cmd, A_FALSE, &(entry.pcp_pri_force),
                                       sizeof (a_bool_t));
            if (SW_OK != rv)
                dprintf("usage: <yes/no/y/n>\n");
        }

    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("dscp_pri_force", "no");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: <yes/no/y/n>\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_confirm(cmd, A_FALSE, &(entry.dscp_pri_force),
                                       sizeof (a_bool_t));
            if (SW_OK != rv)
                dprintf("usage: <yes/no/y/n>\n");
        }

    }
    while (talk_mode && (SW_OK != rv));

    *(fal_qos_pri_precedence_t *)val = entry;
    return SW_OK;
}

void
cmd_data_print_port_pri(a_uint8_t * param_name, a_uint32_t * buf, a_uint32_t size)
{
    fal_qos_pri_precedence_t *entry;

    entry = (fal_qos_pri_precedence_t *) buf;
    
    dprintf("\n[pcp_pri_prece]:0x%x [dscp_pri_prece]:0x%x [preheader_pri_prece]:0x%x ",
			entry->pcp_pri, entry->dscp_pri, entry->preheader_pri);
    dprintf("\n[flow_pri_prece]:0x%x [acl_pri_prece]:0x%x [post_acl_pri_prece]:0x%x ",
			entry->flow_pri, entry->acl_pri, entry->post_acl_pri);
    dprintf("\n[pcp_pri_force]:0x%x [dscp_pri_force]:0x%x ",
			entry->pcp_pri_force, entry->dscp_pri_force);
}

sw_error_t
cmd_data_check_port_remark(char *cmd_str, void * val, a_uint32_t size)
{
    char *cmd;
    sw_error_t rv;
    fal_qos_remark_enable_t entry;

    aos_mem_zero(&entry, sizeof (fal_qos_remark_enable_t));

    do
    {
        cmd = get_sub_cmd("pcp_change_en", "no");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: <yes/no/y/n>\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_confirm(cmd, A_FALSE, &(entry.pcp_change_en),
                                       sizeof (a_bool_t));
            if (SW_OK != rv)
                dprintf("usage: <yes/no/y/n>\n");
        }

    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("dei_change_en", "no");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: <yes/no/y/n>\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_confirm(cmd, A_FALSE, &(entry.dei_chage_en),
                                       sizeof (a_bool_t));
            if (SW_OK != rv)
                dprintf("usage: <yes/no/y/n>\n");
        }

    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("dscp_change_en", "no");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: <yes/no/y/n>\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_confirm(cmd, A_FALSE, &(entry.dscp_change_en),
                                       sizeof (a_bool_t));
            if (SW_OK != rv)
                dprintf("usage: <yes/no/y/n>\n");
        }

    }
    while (talk_mode && (SW_OK != rv));

    *(fal_qos_remark_enable_t *)val = entry;
    return SW_OK;
}

void
cmd_data_print_port_remark(a_uint8_t * param_name, a_uint32_t * buf, a_uint32_t size)
{
    fal_qos_remark_enable_t *entry;

    entry = (fal_qos_remark_enable_t *) buf;
    
    dprintf("\n[pcp_change_en]:0x%x [dei_chage_en]:0x%x [dscp_change_en]:0x%x ",
			entry->pcp_change_en, entry->dei_chage_en, entry->dscp_change_en);
}

sw_error_t
cmd_data_check_cosmap(char *cmd_str, void * val, a_uint32_t size)
{
    char *cmd;
    a_uint32_t tmp;
    sw_error_t rv;
    fal_qos_cosmap_t entry;

    aos_mem_zero(&entry, sizeof (fal_qos_cosmap_t));

    do
    {
        cmd = get_sub_cmd("internal_pcp", "0");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: internal pcp\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_uint8(cmd, &tmp,
                                       sizeof (a_uint32_t));
            if (SW_OK != rv)
            {
                dprintf("usage: internal pcp\n");
            }
            else
            {
                entry.internal_pcp = tmp;
            }
        }

    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("internal_dei", "0");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: internal dei\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_uint8(cmd, &tmp,
                                       sizeof (a_uint32_t));
            if (SW_OK != rv)
            {
                dprintf("usage: internal dei\n");
            }
            else
            {
                entry.internal_dei = tmp;
            }
        }

    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("internal_pri", "0");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: internal pri\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_uint8(cmd, &tmp,
                                       sizeof (a_uint32_t));
            if (SW_OK != rv)
            {
                dprintf("usage: internal pri\n");
            }
            else
            {
                entry.internal_pri = tmp;
            }
        }

    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("internal_dscp", "0");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: internal dscp\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_uint8(cmd, &tmp,
                                       sizeof (a_uint32_t));
            if (SW_OK != rv)
            {
                dprintf("usage: internal dscp\n");
            }
            else
            {
                entry.internal_dscp = tmp;
            }
        }

    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("internal_dp", "0");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: internal dp\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_uint8(cmd, &tmp,
                                       sizeof (a_uint32_t));
            if (SW_OK != rv)
            {
                dprintf("usage: internal dp\n");
            }
            else
            {
                entry.internal_dp = tmp;
            }
        }

    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("dscp_mask", "0");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: dscp mask\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_uint8(cmd, &tmp,
                                       sizeof (a_uint32_t));
            if (SW_OK != rv)
            {
                dprintf("usage: dscp mask\n");
            }
            else
            {
                entry.dscp_mask = tmp;
            }
        }

    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("dscp_en", "no");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: <yes/no/y/n>\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_confirm(cmd, A_FALSE, &(entry.dscp_en),
                                       sizeof (a_bool_t));
            if (SW_OK != rv)
                dprintf("usage: <yes/no/y/n>\n");
        }

    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("pcp_en", "no");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: <yes/no/y/n>\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_confirm(cmd, A_FALSE, &(entry.pcp_en),
                                       sizeof (a_bool_t));
            if (SW_OK != rv)
                dprintf("usage: <yes/no/y/n>\n");
        }

    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("dei_en", "no");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: <yes/no/y/n>\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_confirm(cmd, A_FALSE, &(entry.dei_en),
                                       sizeof (a_bool_t));
            if (SW_OK != rv)
                dprintf("usage: <yes/no/y/n>\n");
        }

    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("pri_en", "no");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: <yes/no/y/n>\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_confirm(cmd, A_FALSE, &(entry.pri_en),
                                       sizeof (a_bool_t));
            if (SW_OK != rv)
                dprintf("usage: <yes/no/y/n>\n");
        }

    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("dp_en", "no");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: <yes/no/y/n>\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_confirm(cmd, A_FALSE, &(entry.dp_en),
                                       sizeof (a_bool_t));
            if (SW_OK != rv)
                dprintf("usage: <yes/no/y/n>\n");
        }

    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("qos_prec", "0");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: dscp mask\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_uint8(cmd, &tmp,
                                       sizeof (a_uint32_t));
            if (SW_OK != rv)
            {
                dprintf("usage: qos prec\n");
            }
            else
            {
                entry.qos_prec = tmp;
            }
        }

    }
    while (talk_mode && (SW_OK != rv));

    *(fal_qos_cosmap_t *)val = entry;
    return SW_OK;
}

void
cmd_data_print_cosmap(a_uint8_t * param_name, a_uint32_t * buf, a_uint32_t size)
{
    fal_qos_cosmap_t *entry;

    entry = (fal_qos_cosmap_t *) buf;
    
    dprintf("\n[internal_pcp]:0x%x [internal_dei]:0x%x [internal_pri]:0x%x ",
			entry->internal_pcp, entry->internal_dei, entry->internal_pri);
    dprintf("\n[internal_dscp]:0x%x [internal_dp]:0x%x [dscp_mask]:0x%x ",
			entry->internal_dscp, entry->internal_dp, entry->dscp_mask);
    dprintf("\n[dscp_en]:0x%x [pcp_en]:0x%x [dei_en]:0x%x ",
			entry->dscp_en, entry->pcp_en, entry->dei_en);
    dprintf("\n[pri_en]:0x%x [dp_en]:0x%x [qos_prec]:0x%x ",
			entry->pri_en, entry->dp_en, entry->qos_prec);
}

void
cmd_data_print_port_scheduler_resource(a_uint8_t * param_name, a_uint32_t * buf, a_uint32_t size)
{
    fal_portscheduler_resource_t *entry;

    entry = (fal_portscheduler_resource_t *) buf;
    
    dprintf("\n[ucastq_start]:0x%x [ucastq_num]:0x%x [mcastq_start]:0x%x [mcastq_num]:0x%x ",
			entry->ucastq_start, entry->ucastq_num, entry->mcastq_start, entry->mcastq_num);
    dprintf("\n[l0sp_start]:0x%x [l0sp_num]:0x%x [l0cdrr_start]:0x%x [l0cdrr_num]:0x%x ",
			entry->l0sp_start, entry->l0sp_num, entry->l0cdrr_start, entry->l0cdrr_num);
    dprintf("\n[l0edrr_start]:0x%x [l0edrr_num]:0x%x [l1sp_start]:0x%x [l1sp_num]:0x%x ",
			entry->l0edrr_start, entry->l0edrr_num, entry->l1sp_start, entry->l1sp_num);
    dprintf("\n[l1cdrr_start]:0x%x [l1cdrr_num]:0x%x [l1edrr_start]:0x%x [l1edrr_num]:0x%x ",
			entry->l1cdrr_start, entry->l1cdrr_num, entry->l1edrr_start, entry->l1edrr_num);
}

sw_error_t
cmd_data_check_queue_scheduler(char *cmd_str, void * val, a_uint32_t size)
{
    char *cmd;
    a_uint32_t tmp;
    sw_error_t rv;
    fal_qos_scheduler_cfg_t entry;

    aos_mem_zero(&entry, sizeof (fal_qos_scheduler_cfg_t));

    do
    {
        cmd = get_sub_cmd("sp_id", "0");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: sp id\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_uint8(cmd, &tmp,
                                       sizeof (a_uint32_t));
            if (SW_OK != rv)
            {
                dprintf("usage: spi id\n");
            }
            else
            {
                entry.sp_id = tmp;
            }
        }

    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("e_pri", "0");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: e pri\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_uint8(cmd, &tmp,
                                       sizeof (a_uint32_t));
            if (SW_OK != rv)
            {
                dprintf("usage: e pri\n");
            }
            else
            {
                entry.e_pri = tmp;
            }
        }

    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("c_pri", "0");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: c pri\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_uint8(cmd, &tmp,
                                       sizeof (a_uint32_t));
            if (SW_OK != rv)
            {
                dprintf("usage: c pri\n");
            }
            else
            {
                entry.c_pri = tmp;
            }
        }

    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("c_drr_id", "0");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: c drr id\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_uint8(cmd, &tmp,
                                       sizeof (a_uint32_t));
            if (SW_OK != rv)
            {
                dprintf("usage: c drr id\n");
            }
            else
            {
                entry.c_drr_id = tmp;
            }
        }

    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("e_drr_id", "0");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: e drr id\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_uint8(cmd, &tmp,
                                       sizeof (a_uint32_t));
            if (SW_OK != rv)
            {
                dprintf("usage: e drr id\n");
            }
            else
            {
                entry.e_drr_id = tmp;
            }
        }

    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("e_drr_wt", "0");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: e drr wt\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_uint16(cmd, &tmp,
                                       sizeof (a_uint32_t));
            if (SW_OK != rv)
            {
                dprintf("usage: e drr wt\n");
            }
            else
            {
                entry.e_drr_wt = tmp;
            }
        }

    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("c_drr_wt", "0");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: c drr wt\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_uint16(cmd, &tmp,
                                       sizeof (a_uint32_t));
            if (SW_OK != rv)
            {
                dprintf("usage: c drr wt\n");
            }
            else
            {
                entry.c_drr_wt = tmp;
            }
        }

    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("c_drr_ut", "0");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: c drr unit\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_uint8(cmd, &tmp,
                                       sizeof (a_uint32_t));
            if (SW_OK != rv)
            {
                dprintf("usage: c drr unit\n");
            }
            else
            {
                entry.c_drr_unit = tmp;
            }
        }

    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("e_drr_ut", "0");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: e drr unit\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_uint8(cmd, &tmp,
                                       sizeof (a_uint32_t));
            if (SW_OK != rv)
            {
                dprintf("usage: e drr unit\n");
            }
            else
            {
                entry.e_drr_unit = tmp;
            }
        }

    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("drr_frame_mode", "0");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: drr frame mode\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_uint32(cmd, &(entry.drr_frame_mode),
                                       sizeof (a_uint32_t));
            if (SW_OK != rv)
                dprintf("usage: drr frame mode\n");
        }

    }
    while (talk_mode && (SW_OK != rv));

    *(fal_qos_scheduler_cfg_t *)val = entry;
    return SW_OK;
}

void
cmd_data_print_queue_scheduler(a_uint8_t * param_name, a_uint32_t * buf, a_uint32_t size)
{
    fal_qos_scheduler_cfg_t *entry;

    entry = (fal_qos_scheduler_cfg_t *) buf;
    
    dprintf("\n[sp_id]:0x%x [e_pri]:0x%x [c_pri]:0x%x [c_drr_id]:0x%x [e_drr_id]:0x%x ",
			entry->sp_id, entry->e_pri, entry->c_pri, entry->c_drr_id, entry->e_drr_id);
    dprintf("\n[e_drr_wt]:0x%x [c_drr_wt]:0x%x [c_drr_unit]:0x%x [e_drr_unit]:0x%x [drr_frame_mode]:0x%x ",
			entry->e_drr_wt, entry->c_drr_wt, entry->c_drr_unit, entry->e_drr_unit, entry->drr_frame_mode);
}

sw_error_t
cmd_data_check_bm_static_thresh(char *cmd_str, void * val, a_uint32_t size)
{
    char *cmd;
    a_uint32_t tmp;
    sw_error_t rv;
    fal_bm_static_cfg_t entry;

    aos_mem_zero(&entry, sizeof (fal_bm_static_cfg_t));

    do
    {
        cmd = get_sub_cmd("max_thresh", "0");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: max thresh\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_uint16(cmd, &tmp,
                                       sizeof (a_uint32_t));
            if (SW_OK != rv)
            {
                dprintf("usage: max thresh\n");
            }
            else
            {
                entry.max_thresh = tmp;
            }
        }

    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("resume_off", "0");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: resume offset\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_uint16(cmd, &tmp,
                                       sizeof (a_uint32_t));
            if (SW_OK != rv)
            {
                dprintf("usage: resume offset\n");
            }
            else
            {
                entry.resume_off = tmp;
            }
        }

    }
    while (talk_mode && (SW_OK != rv));

    *(fal_bm_static_cfg_t *)val = entry;
    return SW_OK;
}

void
cmd_data_print_bm_static_thresh(a_uint8_t * param_name, a_uint32_t * buf, a_uint32_t size)
{
    fal_bm_static_cfg_t *entry;

    entry = (fal_bm_static_cfg_t *) buf;

    dprintf("\n[max_thresh]:0x%x [resume_off]:0x%x ",
			entry->max_thresh, entry->resume_off);
}

void
cmd_data_print_queue_cnt(a_uint8_t * param_name, a_uint32_t * buf, a_uint32_t size)
{
    fal_queue_stats_t *entry;

    entry = (fal_queue_stats_t *) buf;

    dprintf("\n[tx_packets]:0x%x [tx_bytes]:0x%llx [pending_buff_num]:0x%x ",
			entry->tx_packets, entry->tx_bytes, entry->pending_buff_num);
    dprintf("\n[green_probability_drop_packets]:0x%x ", entry->drop_packets[0]);
    dprintf("\n[green_probability_drop_bytes]:0x%llx ", entry->drop_bytes[0]);
    dprintf("\n[yellow_probability_drop_packets]:0x%x ", entry->drop_packets[1]);
    dprintf("\n[yellow_probability_drop_bytes]:0x%llx ", entry->drop_bytes[1]);
    dprintf("\n[red_probability_drop_packets]:0x%x ", entry->drop_packets[2]);
    dprintf("\n[red_probability_drop_bytes]:0x%llx ", entry->drop_bytes[2]);
    dprintf("\n[green_force_drop_packets]:0x%x ", entry->drop_packets[3]);
    dprintf("\n[green_force_drop_bytes]:0x%llx ", entry->drop_bytes[3]);
    dprintf("\n[yellow_force_drop_packets]:0x%x ", entry->drop_packets[4]);
    dprintf("\n[yellow_force_drop_bytes]:0x%llx ", entry->drop_bytes[4]);
    dprintf("\n[red_force_drop_packets]:0x%x ", entry->drop_packets[5]);
    dprintf("\n[red_force_drop_bytes]:0x%llx ", entry->drop_bytes[5]);

}

void
cmd_data_print_bm_port_counter(a_uint8_t * param_name, a_uint32_t * buf, a_uint32_t size)
{
    fal_bm_port_counter_t *entry;

    entry = (fal_bm_port_counter_t *) buf;

    dprintf("\n[drop_byte_counter]:0x%llx [drop_packet_counter]:0x%x [fc_drop_byte_counter]:0x%llx [fc_drop_packet_counter]:0x%x ",
			entry->drop_byte_counter, entry->drop_packet_counter, entry->fc_drop_byte_counter, entry->fc_drop_packet_counter);
    dprintf("\n[used_counter]:0x%x [react_counter]:0x%x ",
			entry->used_counter, entry->react_counter);
}

sw_error_t
cmd_data_check_bm_dynamic_thresh(char *cmd_str, void * val, a_uint32_t size)
{
    char *cmd;
    a_uint32_t tmp;
    sw_error_t rv;
    fal_bm_dynamic_cfg_t entry;

    aos_mem_zero(&entry, sizeof (fal_bm_dynamic_cfg_t));

    do
    {
        cmd = get_sub_cmd("weight", "0");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: weight\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_uint8(cmd, &tmp,
                                       sizeof (a_uint32_t));
            if (SW_OK != rv)
            {
                dprintf("usage: weight\n");
            }
            else
            {
                entry.weight = tmp;
            }
        }

    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("shared_ceiling", "0");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: shared ceiling\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_uint16(cmd, &tmp,
                                       sizeof (a_uint32_t));
            if (SW_OK != rv)
            {
                dprintf("usage: shared ceiling\n");
            }
            else
            {
                entry.shared_ceiling = tmp;
            }
        }

    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("resume_off", "0");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: resume offset\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_uint16(cmd, &tmp,
                                       sizeof (a_uint32_t));
            if (SW_OK != rv)
            {
                dprintf("usage: resume offset\n");
            }
            else
            {
                entry.resume_off = tmp;
            }
        }

    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("resume_min_thresh", "0");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: resmue min thresh\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_uint16(cmd, &tmp,
                                       sizeof (a_uint32_t));
            if (SW_OK != rv)
            {
                dprintf("usage: resume min thresh\n");
            }
            else
            {
                entry.resume_min_thresh = tmp;
            }
        }

    }
    while (talk_mode && (SW_OK != rv));

    *(fal_bm_dynamic_cfg_t *)val = entry;
    return SW_OK;
}

void
cmd_data_print_bm_dynamic_thresh(a_uint8_t * param_name, a_uint32_t * buf, a_uint32_t size)
{
    fal_bm_dynamic_cfg_t *entry;

    entry = (fal_bm_dynamic_cfg_t *) buf;

    dprintf("\n[weight]:0x%x [shared_ceiling]:0x%x [resume_off]:0x%x [resume_min_thresh]:0x%x ",
			entry->weight, entry->shared_ceiling, entry->resume_off, entry->resume_min_thresh);
}

sw_error_t
cmd_data_check_ring_queue(char *cmd_str, void * val, a_uint32_t size)
{
    char *cmd;
    a_uint32_t i = 0;
    sw_error_t rv;
    fal_queue_bmp_t entry;

    aos_mem_zero(&entry, sizeof (fal_queue_bmp_t));

    do
    {
        cmd = get_sub_cmd("bmp", "0");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: bmp\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_uint32(cmd, &(entry.bmp[i]),
                                       sizeof (a_uint32_t));
            if (SW_OK != rv)
                dprintf("usage: bmp\n");
        }

    }
    while ((talk_mode && (SW_OK != rv)) || (++i < 10));

    *(fal_queue_bmp_t *)val = entry;
    return SW_OK;
}

void
cmd_data_print_ring_queue(a_uint8_t * param_name, a_uint32_t * buf, a_uint32_t size)
{
    fal_queue_bmp_t *entry;
    int i;

    entry = (fal_queue_bmp_t *) buf;

    for (i = 0; i < 10; i++)
    
    dprintf("\n[bmp%d]:0x%x ", i, entry->bmp[i]);

}

sw_error_t
cmd_data_check_flow_global(char *cmd_str, void * val, a_uint32_t size)
{
    char *cmd;
    a_uint32_t tmp;
    sw_error_t rv;
    fal_flow_global_cfg_t entry;

    aos_mem_zero(&entry, sizeof (fal_flow_global_cfg_t));

    do
    {
        cmd = get_sub_cmd("src_intf_check_action", "forward");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: <forward/drop/cpycpu/rdtcpu>\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_maccmd(cmd, &(entry.src_if_check_action),
                                       sizeof (fal_fwd_cmd_t));
            if (SW_OK != rv)
                dprintf("usage: <forward/drop/cpycpu/rdtcpu>\n");
        }

    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("src_intf_deacclr_en", "no");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: <yes/no/y/n>\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_confirm(cmd, A_FALSE, &(entry.src_if_check_deacclr_en),
                                       sizeof (a_bool_t));
            if (SW_OK != rv)
                dprintf("usage: <yes/no/y/n>\n");
        }

    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("service_loop_en", "no");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: <yes/no/y/n>\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_confirm(cmd, A_FALSE, &(entry.service_loop_en),
                                       sizeof (a_bool_t));
            if (SW_OK != rv)
                dprintf("usage: <yes/no/y/n>\n");
        }

    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("servcie_loop_action", "forward");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: <forward/drop/cpycpu/rdtcpu>\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_maccmd(cmd, &(entry.service_loop_action),
                                       sizeof (fal_fwd_cmd_t));
            if (SW_OK != rv)
                dprintf("usage: <forward/drop/cpycpu/rdtcpu>\n");
        }

    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("service_loop_deacclr_en", "no");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: <yes/no/y/n>\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_confirm(cmd, A_FALSE, &(entry.service_loop_deacclr_en),
                                       sizeof (a_bool_t));
            if (SW_OK != rv)
                dprintf("usage: <yes/no/y/n>\n");
        }

    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("flow_deacclr_action", "forward");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: <forward/drop/cpycpu/rdtcpu>\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_maccmd(cmd, &(entry.flow_deacclr_action),
                                       sizeof (fal_fwd_cmd_t));
            if (SW_OK != rv)
                dprintf("usage: <forward/drop/cpycpu/rdtcpu>\n");
        }

    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("sync_mismatch_action", "forward");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: <forward/drop/cpycpu/rdtcpu>\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_maccmd(cmd, &(entry.sync_mismatch_action),
                                       sizeof (fal_fwd_cmd_t));
            if (SW_OK != rv)
                dprintf("usage: <forward/drop/cpycpu/rdtcpu>\n");
        }

    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("sync_mismatch_deacclr_en", "no");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: <yes/no/y/n>\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_confirm(cmd, A_FALSE, &(entry.sync_mismatch_deacclr_en),
                                       sizeof (a_bool_t));
            if (SW_OK != rv)
                dprintf("usage: <yes/no/y/n>\n");
        }

    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("hash_mode)0", "0");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: hash mode\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_uint8(cmd, &tmp,
                                       sizeof (a_uint32_t));
            if (SW_OK != rv)
                dprintf("usage: hash mode\n");
            else
                entry.hash_mode_0 = tmp;
        }

    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("hash_mode_1", "0");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: hash mode\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_uint8(cmd, &tmp,
                                       sizeof (a_uint32_t));
            if (SW_OK != rv)
                dprintf("usage: hash mode\n");
            else
                entry.hash_mode_1 = tmp;
        }

    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("flow_mismatch_copy_escape_en", "no");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: <yes/no/y/n>\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_confirm(cmd, A_FALSE, &(entry.flow_mismatch_copy_escape_en),
                                       sizeof (a_bool_t));
            if (SW_OK != rv)
	    {
                dprintf("usage: <yes/no/y/n>\n");
	    }
        }

    }
    while (talk_mode && (SW_OK != rv));

    *(fal_flow_global_cfg_t *)val = entry;
    return SW_OK;

}

void
cmd_data_print_flow_global(a_uint8_t * param_name, a_uint32_t * buf, a_uint32_t size)
{
    fal_flow_global_cfg_t *entry;

    entry = (fal_flow_global_cfg_t *) buf;

    dprintf("\n[src_intf_check_action]:0x%x [src_intf_check_deacclr_en]:0x%x "
		    "[service_loop_en]:0x%x [service_loop_action]:0x%x "
		    "[service_loop_deacclr_en]:0x%x ",
		    entry->src_if_check_action, entry->src_if_check_deacclr_en,
		    entry->service_loop_en, entry->service_loop_action,
		    entry->service_loop_deacclr_en);
    dprintf("\n[flow_deacclr_action]:0x%x [sync_mismatch_action]:0x%x "
		    "[sync_mismatch_deacclr_en]:0x%x [hash_mode_0]:0x%x "
		    "[hash_mode_1]:0x%x [flow_mismatch_copy_escape_en]:0x%x",
		    entry->flow_deacclr_action, entry->sync_mismatch_action,
		    entry->sync_mismatch_deacclr_en, entry->hash_mode_0,
		    entry->hash_mode_1, entry->flow_mismatch_copy_escape_en);

}


sw_error_t
cmd_data_check_flow(char *cmd_str, void * val, a_uint32_t size)
{
    char *cmd;
    a_uint32_t tmp;
    sw_error_t rv;
    fal_flow_entry_t entry;

    aos_mem_zero(&entry, sizeof (fal_flow_entry_t));

    do
    {
        cmd = get_sub_cmd("entry id", "0");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: entry id \n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_uint32(cmd, &(entry.entry_id), sizeof (a_uint32_t));
            if (SW_OK != rv)
                dprintf("usage: entry id \n");
        }
    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("entry type", "0");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: entry type \n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_uint8(cmd, &tmp, sizeof (a_uint32_t));
            if (SW_OK != rv)
            {
                dprintf("usage: entry type \n");
            }
            else
            {
                entry.entry_type = tmp;
            }
        }
    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("host addr type", "0");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: host addr type \n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_uint8(cmd, &tmp, sizeof (a_uint32_t));
            if (SW_OK != rv)
            {
                dprintf("usage: host addr type \n");
            }
            else
            {
                entry.host_addr_type = tmp;
            }
        }
    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("host addr index", "0");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: host addr index \n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_uint16(cmd, &tmp, sizeof (a_uint32_t));
            if (SW_OK != rv)
            {
                dprintf("usage: host addr index \n");
            }
            else
            {
                entry.host_addr_index = tmp;
            }
        }
    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("protocol", "0");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: protocol \n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_uint8(cmd, &tmp, sizeof (a_uint32_t));
            if (SW_OK != rv)
            {
                dprintf("usage: protocol \n");
            }
            else
            {
                entry.protocol = tmp;
            }
        }
    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("age", "0");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: age \n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_uint8(cmd, &tmp, sizeof (a_uint32_t));
            if (SW_OK != rv)
            {
                dprintf("usage: age \n");
            }
            else
            {
                entry.age = tmp;
            }
        }
    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("src intf valid", "0");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: src intf valid \n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_uint8(cmd, &(entry.src_intf_valid), sizeof (a_uint8_t));
            if (SW_OK != rv)
                dprintf("usage: src intf valid \n");
        }
    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("src intf index", "0");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: src intf index \n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_uint8(cmd, &tmp, sizeof (a_uint32_t));
            if (SW_OK != rv)
            {
                dprintf("usage: src intf index \n");
            }
            else
            {
                entry.src_intf_index = tmp;
            }
        }
    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("fwd type", "0");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: fwd type \n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_uint8(cmd, &tmp, sizeof (a_uint32_t));
            if (SW_OK != rv)
            {
                dprintf("usage: fwd type \n");
            }
            else
            {
                entry.fwd_type = tmp;
            }
        }
    }
    while (talk_mode && (SW_OK != rv));

    
    do
    {
        cmd = get_sub_cmd("snat nexthop", "0");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: snat nexthop \n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_uint16(cmd, &tmp, sizeof (a_uint32_t));
            if (SW_OK != rv)
            {
                dprintf("usage: snat nexthop \n");
            }
            else
            {
                entry.snat_nexthop = tmp;
            }
        }
    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("snat srcport", "0");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: snat srcport \n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_uint16(cmd, &tmp, sizeof (a_uint32_t));
            if (SW_OK != rv)
            {
                dprintf("usage: snat srcport \n");
            }
            else
            {
                entry.snat_srcport = tmp;
            }
        }
    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("dnat nexthop", "0");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: dnat nexthop \n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_uint16(cmd, &tmp, sizeof (a_uint32_t));
            if (SW_OK != rv)
            {
                dprintf("usage: dnat nexthop \n");
            }
            else
            {
                entry.dnat_nexthop = tmp;
            }
        }
    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("dnat dstport", "0");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: dnat dstport \n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_uint16(cmd, &tmp, sizeof (a_uint32_t));
            if (SW_OK != rv)
            {
                dprintf("usage: dnat dstport \n");
            }
            else
            {
                entry.dnat_dstport = tmp;
            }
        }
    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("route nexthop", "0");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: route nexthop \n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_uint16(cmd, &tmp, sizeof (a_uint32_t));
            if (SW_OK != rv)
            {
                dprintf("usage: route nexthop \n");
            }
            else
            {
                entry.route_nexthop = tmp;
            }
        }
    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("port valid", "0");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: port valid \n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_uint8(cmd, &(entry.port_valid), sizeof (a_uint8_t));
            if (SW_OK != rv)
                dprintf("usage: port valid \n");
        }
    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("route port", "0");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: route port \n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_uint32(cmd, &(entry.route_port), sizeof (a_uint32_t));
            if (SW_OK != rv)
                dprintf("usage: route port \n");
        }
    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("bridge port", "0");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: bridge port \n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_uint32(cmd, &(entry.bridge_port), sizeof (a_uint32_t));
            if (SW_OK != rv)
                dprintf("usage: bridge port \n");
        }
    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("deacclr", "yes");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: <yes/no/y/n>\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_confirm(cmd, A_TRUE, &(entry.deacclr_en), sizeof (a_bool_t));
            if (SW_OK != rv)
                dprintf("usage: <yes/no/y/n>\n");
        }
    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("copy tocpu", "yes");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: <yes/no/y/n>\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_confirm(cmd, A_TRUE, &(entry.copy_tocpu_en), sizeof (a_bool_t));
            if (SW_OK != rv)
                dprintf("usage: <yes/no/y/n>\n");
        }
    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("syn toggle", "0");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: sync toggle \n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_uint8(cmd, &tmp, sizeof (a_uint32_t));
            if (SW_OK != rv)
            {
                dprintf("usage: sync toggle \n");
            }
            else
            {
                entry.syn_toggle = tmp;
            }
        }
    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("pri profile", "0");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: pri profile \n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_uint8(cmd, &tmp, sizeof (a_uint32_t));
            if (SW_OK != rv)
            {
                dprintf("usage: pri profile \n");
            }
            else
            {
                entry.pri_profile = tmp;
            }
        }
    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("service code", "0");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: service code \n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_uint8(cmd, &tmp, sizeof (a_uint32_t));
            if (SW_OK != rv)
            {
                dprintf("usage: service code \n");
            }
            else
            {
                entry.sevice_code = tmp;
            }
        }
    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("ip type", "0");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: ip type \n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_uint8(cmd, &tmp, sizeof (a_uint32_t));
            if (SW_OK != rv)
            {
                dprintf("usage: ip type \n");
            }
            else
            {
                entry.ip_type = tmp;
            }
        }
    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("src port", "0");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: src port \n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_uint16(cmd, &tmp, sizeof (a_uint32_t));
            if (SW_OK != rv)
            {
                dprintf("usage: src port \n");
            }
            else
            {
                entry.src_port = tmp;
            }
        }
    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("dst port", "0");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: dst port \n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_uint16(cmd, &tmp, sizeof (a_uint32_t));
            if (SW_OK != rv)
            {
                dprintf("usage: dst port \n");
            }
            else
            {
                entry.dst_port = tmp;
            }
        }
    }
    while (talk_mode && (SW_OK != rv));

    if (entry.entry_type & FAL_FLOW_IP4_5TUPLE_ADDR || entry.entry_type & FAL_FLOW_IP4_3TUPLE_ADDR) {
        cmd_data_check_element("ip addr", NULL,
                               "usage: the format is xx.xx.xx.xx \n",
                               cmd_data_check_ip4addr, (cmd, &(entry.flow_ip.ipv4), 4));
		
    } else if (entry.entry_type & FAL_FLOW_IP6_5TUPLE_ADDR || entry.entry_type &
				FAL_FLOW_IP6_3TUPLE_ADDR) {
        cmd_data_check_element("ip addr", NULL,
                               "usage: the format is xxxx::xx.xx \n",
                               cmd_data_check_ip6addr, (cmd, &(entry.flow_ip.ipv6), 16));
    }

    do
    {
        cmd = get_sub_cmd("tree id", "0");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: tree id \n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_uint32(cmd, &(entry.tree_id), sizeof (a_uint32_t));
            if (SW_OK != rv)
                dprintf("usage: tree id \n");
        }
    }
    while (talk_mode && (SW_OK != rv));

    *(fal_flow_entry_t *)val = entry;
    return SW_OK;
}

void
cmd_data_print_flow(a_uint8_t * param_name, a_uint32_t * buf, a_uint32_t size)
{
    fal_flow_entry_t *entry;

    entry = (fal_flow_entry_t *) buf;
    
    dprintf("\n[entry_id]:0x%x [entry_type]:0x%x [host_addr_type]:0x%x [host_addr_index]:0x%x ",
			entry->entry_id, entry->entry_type, entry->host_addr_type, entry->host_addr_index);
    dprintf("\n[protocol]:0x%x [agetime]:0x%x [src_intf_valid]:0x%x [src_intf_index]:0x%x [fwd_type]:0x%x ",
			entry->protocol, entry->age, entry->src_intf_valid, entry->src_intf_index, entry->fwd_type);
    dprintf("\n[snat_nexthop]:0x%x [snat_srcport]:0x%x [dnat_nexthop]:0x%x [dnat_dstport]:0x%x [route_nexthop]:0x%x ",
			entry->snat_nexthop, entry->snat_srcport, entry->dnat_nexthop, entry->dnat_dstport, entry->route_nexthop);
    dprintf("\n[port_valid]:0x%x [route_port]:0x%x [bridge_port]:0x%x [de_acclr]:0x%x [copy_tocpu]:0x%x ",
			entry->port_valid, entry->route_port, entry->bridge_port, entry->deacclr_en, entry->copy_tocpu_en);
    dprintf("\n[syn_toggle]:0x%x [pri_profile]:0x%x [sevice_code]:0x%x [ip_type]:0x%x [src_port]:0x%x [dst_port]:0x%x [tree_id]:0x%x ",
			entry->syn_toggle, entry->pri_profile, entry->sevice_code, entry->ip_type, entry->src_port, entry->dst_port, entry->tree_id);
    if (entry->entry_type & FAL_FLOW_IP4_5TUPLE_ADDR || entry->entry_type & FAL_FLOW_IP4_3TUPLE_ADDR) {
        cmd_data_print_ip4addr("\n[ip_addr]:",
                               (a_uint32_t *) & (entry->flow_ip.ipv4),
                               sizeof (fal_ip4_addr_t));
    } else {
        cmd_data_print_ip6addr("\n[ip_addr]:",
                               (a_uint32_t *) & (entry->flow_ip.ipv6),
                               sizeof (fal_ip6_addr_t));
    }
    dprintf("\n[pkt]:0x%x [byte]:0x%x ", entry->pkt_counter, entry->byte_counter);
}


void
cmd_data_print_ac_static_thresh(a_uint8_t * param_name, a_uint32_t * buf, a_uint32_t size)
{
    fal_ac_static_threshold_t *entry;

    entry = (fal_ac_static_threshold_t *) buf;
    
    dprintf("\n[color_en]:0x%x [wred_en]:0x%x [green_max]:0x%x ",
			entry->color_enable, entry->wred_enable, entry->green_max);
    dprintf("\n[green_min_off]:0x%x [yel_max_off]:0x%x [yel_min_off]:0x%x [red_max_off]:0x%x [red_min_off]:0x%x ",
			entry->green_min_off, entry->yel_max_off, entry->yel_min_off, entry->red_max_off, entry->red_min_off);
    dprintf("\n[green_resume_off]:0x%x [yel_resume_off]:0x%x [red_resume_off]:0x%x ",
			entry->green_resume_off, entry->yel_resume_off, entry->red_resume_off);
}

void
cmd_data_print_ac_dynamic_thresh(a_uint8_t * param_name, a_uint32_t * buf, a_uint32_t size)
{
    fal_ac_dynamic_threshold_t *entry;

    entry = (fal_ac_dynamic_threshold_t *) buf;
    
    dprintf("\n[color_en]:0x%x [wred_en]:0x%x [shared_weight]:0x%x ",
			entry->color_enable, entry->wred_enable, entry->shared_weight);
    dprintf("\n[green_min_off]:0x%x [yel_max_off]:0x%x [yel_min_off]:0x%x [red_max_off]:0x%x [red_min_off]:0x%x ",
			entry->green_min_off, entry->yel_max_off, entry->yel_min_off, entry->red_max_off, entry->red_min_off);
    dprintf("\n[green_resume_off]:0x%x [yel_resume_off]:0x%x [red_resume_off]:0x%x [ceiling]:0x%x ",
			entry->green_resume_off, entry->yel_resume_off, entry->red_resume_off, entry->ceiling);
}

void
cmd_data_print_ac_group_buff(a_uint8_t * param_name, a_uint32_t * buf, a_uint32_t size)
{
    fal_ac_group_buffer_t *entry;

    entry = (fal_ac_group_buffer_t *) buf;
    
    dprintf("\n[prealloc_buffer]:0x%x [total_buffer]:0x%x ",
			entry->prealloc_buffer, entry->total_buffer);
}

void
cmd_data_print_ac_ctrl(a_uint8_t * param_name, a_uint32_t * buf, a_uint32_t size)
{
    fal_ac_ctrl_t *entry;

    entry = (fal_ac_ctrl_t *) buf;
    
    dprintf("\n[ac_en]:0x%x [ac_fc_en]:0x%x ",
			entry->ac_en, entry->ac_fc_en);
}

void
cmd_data_print_ac_obj(a_uint8_t * param_name, a_uint32_t * buf, a_uint32_t size)
{
    fal_ac_obj_t *entry;

    entry = (fal_ac_obj_t *) buf;
    
    dprintf("\n[obj_type]:0x%x [obj_id]:0x%x ",
			entry->type, entry->obj_id);
}

sw_error_t
cmd_data_check_flow_ctrl(char *cmd_str, void * val, a_uint32_t size)
{
    char *cmd;
    a_uint32_t tmp;
    sw_error_t rv;
    fal_flow_mgmt_t entry;

    aos_mem_zero(&entry, sizeof (fal_flow_mgmt_t));

    do
    {
        cmd = get_sub_cmd("miss_action", "forward");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: <forward/drop/cpycpu/rdtcpu>\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_maccmd(cmd, &(entry.miss_action),
                                       sizeof (fal_fwd_cmd_t));
            if (SW_OK != rv)
                dprintf("usage: <forward/drop/cpycpu/rdtcpu>\n");
        }

    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("frag_bypass_en", "yes");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: <yes/no/y/n>\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_confirm(cmd, A_TRUE, &(entry.frag_bypass_en), sizeof (a_bool_t));
            if (SW_OK != rv)
                dprintf("usage: <yes/no/y/n>\n");
        }
    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("tcpspec_bypass_en", "yes");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: <yes/no/y/n>\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_confirm(cmd, A_TRUE, &(entry.tcp_spec_bypass_en), sizeof (a_bool_t));
            if (SW_OK != rv)
                dprintf("usage: <yes/no/y/n>\n");
        }
    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("all_bypass_en", "yes");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: <yes/no/y/n>\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_confirm(cmd, A_TRUE, &(entry.all_bypass_en), sizeof (a_bool_t));
            if (SW_OK != rv)
                dprintf("usage: <yes/no/y/n>\n");
        }
    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("key_sel", "0");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: key sel \n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_uint8(cmd, &tmp, sizeof (a_uint32_t));
            if (SW_OK != rv)
                dprintf("usage: key sel \n");
            else
                entry.key_sel = tmp;
        }
    }
    while (talk_mode && (SW_OK != rv));

    *(fal_flow_mgmt_t *)val = entry;
    return SW_OK;
}

void
cmd_data_print_flow_age(a_uint8_t * param_name, a_uint32_t * buf, a_uint32_t size)
{
    fal_flow_age_timer_t *entry;

    entry = (fal_flow_age_timer_t *) buf;
    
    dprintf("\n[age_time]:0x%x [age_unit]:0x%x ",
			entry->age_time, entry->unit);
}

void
cmd_data_print_flow_ctrl(a_uint8_t * param_name, a_uint32_t * buf, a_uint32_t size)
{
    fal_flow_mgmt_t *entry;

    entry = (fal_flow_mgmt_t *) buf;
    
    dprintf("\n[frag_bypass_en]:0x%x [tcp_spec_bypass_en]:0x%x [all_bypass_en]:0x%x "
			"[key_sel]:0x%x [miss_action]:0x%x ",
			entry->frag_bypass_en, entry->tcp_spec_bypass_en,
			entry->all_bypass_en, entry->key_sel, entry->miss_action);
}

sw_error_t
cmd_data_check_ip_mcmode(char *cmd_str, void * val, a_uint32_t size)
{
    a_char_t *cmd;
    sw_error_t rv;
    fal_mc_mode_cfg_t entry;

    aos_mem_zero(&entry, sizeof (fal_mc_mode_cfg_t));

    do
    {
        cmd = get_sub_cmd("ipv4_mc_en", "yes");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: <yes/no/y/n>\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_confirm(cmd, A_TRUE, &(entry.l2_ipv4_mc_en), sizeof (a_bool_t));
            if (SW_OK != rv)
                dprintf("usage: <yes/no/y/n>\n");
        }
    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("ipv4_igmpv3_mode", "0");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: mc mode \n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_uint8(cmd, &(entry.l2_ipv4_mc_mode), sizeof (a_uint8_t));
            if (SW_OK != rv)
                dprintf("usage: mc mode \n");
        }
    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("ipv6_mc_en", "yes");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: <yes/no/y/n>\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_confirm(cmd, A_TRUE, &(entry.l2_ipv6_mc_en), sizeof (a_bool_t));
            if (SW_OK != rv)
                dprintf("usage: <yes/no/y/n>\n");
        }
    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("ipv6_mldv2_mode", "0");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: mc mode \n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_uint8(cmd, &(entry.l2_ipv6_mc_mode), sizeof (a_uint8_t));
            if (SW_OK != rv)
                dprintf("usage: mc mode \n");
        }
    }
    while (talk_mode && (SW_OK != rv));

    *(fal_mc_mode_cfg_t *)val = entry;
    return SW_OK;
}

void
cmd_data_print_ip_mcmode(a_uint8_t * param_name, a_uint32_t * buf, a_uint32_t size)
{
    fal_mc_mode_cfg_t *entry;

    entry = (fal_mc_mode_cfg_t *) buf;
    
    dprintf("\n[ipv4_mc_en]:0x%x [ipv4_igmpv3_mode]:0x%x [ipv6_mc_en]:0x%x [ipv6_mldv2_mode]:0x%x ",
			entry->l2_ipv4_mc_en, entry->l2_ipv4_mc_mode,
			entry->l2_ipv6_mc_en, entry->l2_ipv6_mc_mode);
}


sw_error_t
cmd_data_check_ip_portmac(char *cmd_str, void * val, a_uint32_t size)
{
    a_char_t *cmd;
    sw_error_t rv;
    fal_macaddr_entry_t entry;

    aos_mem_zero(&entry, sizeof (fal_macaddr_entry_t));

    do
    {
        cmd = get_sub_cmd("entry_valid", "0");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: 1 for invalid and 1 for valid \n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_uint8(cmd, &(entry.valid), sizeof (a_uint8_t));
            if (SW_OK != rv)
                dprintf("usage: 1 for invalid and 1 for valid \n");
        }
    }
    while (talk_mode && (SW_OK != rv));

    cmd_data_check_element("mac_addr", NULL,
                           "usage: the format is xx-xx-xx-xx-xx-xx \n",
                           cmd_data_check_macaddr, (cmd, &(entry.mac_addr),
                                   sizeof (fal_mac_addr_t)));

    *(fal_macaddr_entry_t *)val = entry;
    return SW_OK;
}

void
cmd_data_print_ip_portmac(a_uint8_t * param_name, a_uint32_t * buf, a_uint32_t size)
{
    fal_macaddr_entry_t *entry;

    entry = (fal_macaddr_entry_t *) buf;
    
    dprintf("\n[entry_valid]:0x%x", entry->valid);
    cmd_data_print_macaddr("\n[mac_addr]:",
                           (a_uint32_t *) & (entry->mac_addr),
                           sizeof (fal_mac_addr_t));
}

sw_error_t
cmd_data_check_ip_pub(char *cmd_str, void * val, a_uint32_t size)
{
    a_char_t *cmd;
    fal_ip_pub_addr_t entry;

    aos_mem_zero(&entry, sizeof (fal_ip_pub_addr_t));

    cmd_data_check_element("pub_ip_addr", NULL,
                               "usage: the format is xx.xx.xx.xx \n",
                               cmd_data_check_ip4addr, (cmd, &(entry.pub_ip_addr), 4));

    *(fal_ip_pub_addr_t *)val = entry;
    return SW_OK;
}

void
cmd_data_print_ip_pub(a_uint8_t * param_name, a_uint32_t * buf, a_uint32_t size)
{
    fal_ip_pub_addr_t *entry;

    entry = (fal_ip_pub_addr_t *) buf;
    
    cmd_data_print_ip4addr("\n[pub_ip_addr]:",
                               (a_uint32_t *) & (entry->pub_ip_addr),
                               sizeof (fal_ip4_addr_t));
}

sw_error_t
cmd_data_check_ip_sg(char *cmd_str, void * val, a_uint32_t size)
{
    char *cmd;
    sw_error_t rv;
    fal_sg_cfg_t entry;

    aos_mem_zero(&entry, sizeof (fal_sg_cfg_t));

    do
    {
        cmd = get_sub_cmd("ipv4_sg_en", "yes");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: <yes/no/y/n>\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_confirm(cmd, A_TRUE, &(entry.ipv4_sg_en), sizeof (a_bool_t));
            if (SW_OK != rv)
                dprintf("usage: <yes/no/y/n>\n");
        }
    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("ipv4_sg_violation_action", "forward");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: <forward/drop/cpycpu/rdtcpu>\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_maccmd(cmd, &(entry.ipv4_sg_vio_action),
                                       sizeof (fal_fwd_cmd_t));
            if (SW_OK != rv)
                dprintf("usage: <forward/drop/cpycpu/rdtcpu>\n");
        }

    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("ipv4_sg_port_en", "yes");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: <yes/no/y/n>\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_confirm(cmd, A_TRUE, &(entry.ipv4_sg_port_en), sizeof (a_bool_t));
            if (SW_OK != rv)
                dprintf("usage: <yes/no/y/n>\n");
        }
    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("ipv4_sg_svlan_en", "yes");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: <yes/no/y/n>\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_confirm(cmd, A_TRUE, &(entry.ipv4_sg_svlan_en), sizeof (a_bool_t));
            if (SW_OK != rv)
                dprintf("usage: <yes/no/y/n>\n");
        }
    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("ipv4_sg_cvlan_en", "yes");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: <yes/no/y/n>\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_confirm(cmd, A_TRUE, &(entry.ipv4_sg_cvlan_en), sizeof (a_bool_t));
            if (SW_OK != rv)
                dprintf("usage: <yes/no/y/n>\n");
        }
    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("ipv4_src_unk_action", "forward");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: <forward/drop/cpycpu/rdtcpu>\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_maccmd(cmd, &(entry.ipv4_src_unk_action),
                                       sizeof (fal_fwd_cmd_t));
            if (SW_OK != rv)
                dprintf("usage: <forward/drop/cpycpu/rdtcpu>\n");
        }

    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("ipv6_sg_en", "yes");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: <yes/no/y/n>\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_confirm(cmd, A_TRUE, &(entry.ipv6_sg_en), sizeof (a_bool_t));
            if (SW_OK != rv)
                dprintf("usage: <yes/no/y/n>\n");
        }
    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("ipv6_sg_violation_action", "forward");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: <forward/drop/cpycpu/rdtcpu>\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_maccmd(cmd, &(entry.ipv6_sg_vio_action),
                                       sizeof (fal_fwd_cmd_t));
            if (SW_OK != rv)
                dprintf("usage: <forward/drop/cpycpu/rdtcpu>\n");
        }

    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("ipv6_sg_port_en", "yes");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: <yes/no/y/n>\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_confirm(cmd, A_TRUE, &(entry.ipv6_sg_port_en), sizeof (a_bool_t));
            if (SW_OK != rv)
                dprintf("usage: <yes/no/y/n>\n");
        }
    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("ipv6_sg_svlan_en", "yes");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: <yes/no/y/n>\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_confirm(cmd, A_TRUE, &(entry.ipv6_sg_svlan_en), sizeof (a_bool_t));
            if (SW_OK != rv)
                dprintf("usage: <yes/no/y/n>\n");
        }
    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("ipv6_sg_cvlan_en", "yes");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: <yes/no/y/n>\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_confirm(cmd, A_TRUE, &(entry.ipv6_sg_cvlan_en), sizeof (a_bool_t));
            if (SW_OK != rv)
                dprintf("usage: <yes/no/y/n>\n");
        }
    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("ipv6_src_unk_action", "forward");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: <forward/drop/cpycpu/rdtcpu>\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_maccmd(cmd, &(entry.ipv6_src_unk_action),
                                       sizeof (fal_fwd_cmd_t));
            if (SW_OK != rv)
                dprintf("usage: <forward/drop/cpycpu/rdtcpu>\n");
        }

    }
    while (talk_mode && (SW_OK != rv));

    *(fal_sg_cfg_t *)val = entry;
    return SW_OK;
}

void
cmd_data_print_ip_sg(a_uint8_t * param_name, a_uint32_t * buf, a_uint32_t size)
{
    fal_sg_cfg_t *entry;

    entry = (fal_sg_cfg_t *) buf;
    
    dprintf("\n[ipv4_sg_en]:0x%x  [ipv4_sg_violation_action]:0x%x [ipv4_sg_port_en]:0x%x ",
            entry->ipv4_sg_en, entry->ipv4_sg_vio_action, entry->ipv4_sg_port_en);
    dprintf("\n[ipv4_sg_svlan_en]:0x%x  [ipv4_sg_cvlan_en]:0x%x [ipv4_src_unk_action]:0x%x ",
            entry->ipv4_sg_svlan_en, entry->ipv4_sg_cvlan_en, entry->ipv4_src_unk_action);
    dprintf("\n[ipv6_sg_en]:0x%x  [ipv6_sg_violation_action]:0x%x [ipv6_sg_port_en]:0x%x ",
            entry->ipv6_sg_en, entry->ipv6_sg_vio_action, entry->ipv6_sg_port_en);
    dprintf("\n[ipv6_sg_svlan_en]:0x%x  [ipv6_sg_cvlan_en]:0x%x [ipv6_src_unk_action]:0x%x ",
            entry->ipv6_sg_svlan_en, entry->ipv6_sg_cvlan_en, entry->ipv6_src_unk_action);
}

sw_error_t
cmd_data_check_vsi_intf(char *cmd_str, void * val, a_uint32_t size)
{
    char *cmd;
    sw_error_t rv;
    fal_intf_id_t entry;

    aos_mem_zero(&entry, sizeof (fal_intf_id_t));

    do
    {
        cmd = get_sub_cmd("l3_if_valid", "0");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: 0 for invalid and 1 for valid \n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_uint8(cmd, &(entry.l3_if_valid), sizeof (a_uint8_t));
            if (SW_OK != rv)
                dprintf("usage: 0 for invalid and 1 for valid\n");
        }
    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("l3_if_index", "0");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: l3 if index \n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_uint32(cmd, &(entry.l3_if_index), sizeof (a_uint32_t));
            if (SW_OK != rv)
                dprintf("usage: l3 if index\n");
        }
    }
    while (talk_mode && (SW_OK != rv));

    *(fal_intf_id_t *)val = entry;
    return SW_OK;
}

sw_error_t
cmd_data_check_nexthop(char *cmd_str, void * val, a_uint32_t size)
{
    char *cmd;
    a_uint32_t tmp;
    sw_error_t rv;
    fal_ip_nexthop_t entry;

    aos_mem_zero(&entry, sizeof (fal_ip_nexthop_t));

    do
    {
        cmd = get_sub_cmd("type", "0");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: 0 l3 and 1 for vp \n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_uint8(cmd, &(entry.type), sizeof (a_uint8_t));
            if (SW_OK != rv)
                dprintf("usage: 0 for invalid and 1 for valid\n");
        }
    }
    while (talk_mode && (SW_OK != rv));

    if (entry.type == 0) {
		do
    		{
		        cmd = get_sub_cmd("vsi", "0");
		        SW_RTN_ON_NULL_PARAM(cmd);

		        if (!strncasecmp(cmd, "quit", 4))
		        {
		            return SW_BAD_VALUE;
		        }
		        else if (!strncasecmp(cmd, "help", 4))
		        {
		            dprintf("usage: vsi \n");
		            rv = SW_BAD_VALUE;
		        }
		        else
		        {
		            rv = cmd_data_check_uint8(cmd, &tmp, sizeof (a_uint32_t));
		            if (SW_OK != rv)
		            {
		                dprintf("usage: vsi\n");
		            }
		            else
		            {
		                entry.vsi = tmp;
		            }
		        }
    		}
    		while (talk_mode && (SW_OK != rv));
    } else {
    		do
    		{
		        cmd = get_sub_cmd("port", "0");
		        SW_RTN_ON_NULL_PARAM(cmd);

		        if (!strncasecmp(cmd, "quit", 4))
		        {
		            return SW_BAD_VALUE;
		        }
		        else if (!strncasecmp(cmd, "help", 4))
		        {
		            dprintf("usage: port \n");
		            rv = SW_BAD_VALUE;
		        }
		        else
		        {
		            rv = cmd_data_check_uint8(cmd, &(entry.port), sizeof (a_uint8_t));
		            if (SW_OK != rv)
		                dprintf("usage: port\n");
		        }
    		}
    		while (talk_mode && (SW_OK != rv));
    }

    do
    {
        cmd = get_sub_cmd("if_index", "0");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: post l3 if \n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_uint32(cmd, &(entry.if_index), sizeof (a_uint32_t));
            if (SW_OK != rv)
                dprintf("usage: post l3 if index\n");
        }
    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("ip_to_me_en", "no");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: <yes/no/y/n>\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_confirm(cmd, A_FALSE, &(entry.ip_to_me_en), sizeof (a_bool_t));
            if (SW_OK != rv)
                dprintf("usage: <yes/no/y/n>\n");
        }
    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("pub_ip_index", "0");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: pubip index \n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_uint8(cmd, &tmp, sizeof (a_uint32_t));
            if (SW_OK != rv)
            {
                dprintf("usage: pubip index\n");
            }
            else
            {
                entry.pub_ip_index = tmp;
            }
        }
    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("stag_fmt", "0");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: stag fmt \n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_uint8(cmd, &tmp, sizeof (a_uint32_t));
            if (SW_OK != rv)
            {
                dprintf("usage: stag fmt\n");
            }
            else
            {
                entry.stag_fmt = tmp;
            }
        }
    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("svid", "0");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: svid \n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_uint16(cmd, &tmp, sizeof (a_uint32_t));
            if (SW_OK != rv)
            {
                dprintf("usage: svid\n");
            }
            else
            {
                entry.svid = tmp;
            }
        }
    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("ctag_fmt", "0");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: ctag fmt \n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_uint8(cmd, &tmp, sizeof (a_uint32_t));
            if (SW_OK != rv)
            {
                dprintf("usage: ctag fmt\n");
            }
            else
            {
                entry.ctag_fmt = tmp;
            }
        }
    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("cvid", "0");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: cvid \n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_uint16(cmd, &tmp, sizeof (a_uint32_t));
            if (SW_OK != rv)
            {
                dprintf("usage: cvid\n");
            }
            else
            {
                entry.cvid = tmp;
            }
        }
    }
    while (talk_mode && (SW_OK != rv));

    cmd_data_check_element("mac_addr", NULL,
                           "usage: the format is xx-xx-xx-xx-xx-xx \n",
                           cmd_data_check_macaddr, (cmd, &(entry.mac_addr),
                                   sizeof (fal_mac_addr_t)));

    rv = __cmd_data_check_complex("dnat_ip", NULL,
                            "usage: the format is xx.xx.xx.xx \n",
                            cmd_data_check_ip4addr, &(entry.dnat_ip),
                            4);
    if (rv)
        return rv;   
	

    *(fal_ip_nexthop_t *)val = entry;
    return SW_OK;
}

void
cmd_data_print_nexthop(a_uint8_t * param_name, a_uint32_t * buf, a_uint32_t size)
{
    fal_ip_nexthop_t *entry;

    entry = (fal_ip_nexthop_t *) buf;
    
    dprintf("\n[type]:0x%x",
            entry->type);
    if (entry->type == 0)
		dprintf(" [vsi]:0x%x", entry->vsi);
    else
		dprintf("[port]:0x%x", entry->port);
    dprintf(" [if index]:0x%x",
            entry->if_index);

    dprintf("\n[ip_to_me_en]:0x%x [pub_ip_index]:0x%x [stag_fmt]:0x%x",
            entry->ip_to_me_en, entry->pub_ip_index, entry->stag_fmt);
    dprintf("\n[svid]:0x%x [ctag_fmt]:0x%x [cvid]:0x%x",
            entry->svid, entry->ctag_fmt, entry->cvid);
    cmd_data_print_macaddr("\n[mac_addr]:",
                           (a_uint32_t *) & (entry->mac_addr),
                           sizeof (fal_mac_addr_t));
    cmd_data_print_ip4addr("\n[dnat_ip]:",
                               (a_uint32_t *) & (entry->dnat_ip),
                               sizeof (fal_ip4_addr_t));
}

void
cmd_data_print_vsi_intf(a_uint8_t * param_name, a_uint32_t * buf, a_uint32_t size)
{
    fal_intf_id_t *entry;

    entry = (fal_intf_id_t *) buf;
    
    dprintf("\n[l3_if_valid]:0x%x  [l3_if_index]:0x%x",
            entry->l3_if_valid, entry->l3_if_index);
}


void
cmd_data_print_intf(a_uint8_t * param_name, a_uint32_t * buf, a_uint32_t size)
{
    fal_intf_entry_t *entry;

    entry = (fal_intf_entry_t *) buf;
    
    dprintf("\n[mru]:0x%x  [mtu]:0x%x  [ttl_dec_bypass_en]:0x%x",
            entry->mru, entry->mtu, entry->ttl_dec_bypass_en);
    dprintf("\n[ipv4_route_en]:0x%x  [ipv6_route_en]:0x%x  [icmp_trigger_en]:0x%x",
            entry->ipv4_uc_route_en, entry->ipv6_uc_route_en, entry->icmp_trigger_en);
    dprintf("\n[ttl_exceed_action]:0x%x  [ttl_exceed_de_acclr_en]:0x%x "
			"[mac_addr_bitmap]:0x%x",
			entry->ttl_exceed_action, entry->ttl_exceed_deacclr_en,
			entry->mac_addr_bitmap);
    cmd_data_print_macaddr("\n[mac_addr]:",
                           (a_uint32_t *) & (entry->mac_addr),
                           sizeof (fal_mac_addr_t));
    dprintf("\n[rx_pkt]:0x%x  [rx_byte]:0x%x  [rx_drop_pkt]:0x%x "
				"[rx_drop_byte]:0x%x  ",
				entry->counter.rx_pkt_counter, entry->counter.rx_byte_counter,
				entry->counter.rx_drop_pkt_counter,
				entry->counter.rx_drop_byte_counter);
    dprintf("\n[tx_pkt]:0x%x  [tx_byte]:0x%x  [tx_drop_pkt]:0x%x "
				"[tx_drop_byte]:0x%x  ",
				entry->counter.tx_pkt_counter, entry->counter.tx_byte_counter,
				entry->counter.tx_drop_pkt_counter,
				entry->counter.tx_drop_byte_counter);
}

sw_error_t
cmd_data_check_arp_sg(char *cmd_str, void * val, a_uint32_t size)
{
    char *cmd;
    sw_error_t rv;
    fal_arp_sg_cfg_t entry;

    aos_mem_zero(&entry, sizeof (fal_arp_sg_cfg_t));

    do
    {
        cmd = get_sub_cmd("arp_sg_en", "yes");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: <yes/no/y/n>\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_confirm(cmd, A_TRUE, &(entry.ipv4_arp_sg_en), sizeof (a_bool_t));
            if (SW_OK != rv)
                dprintf("usage: <yes/no/y/n>\n");
        }
    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("arp_sg_violation_action", "forward");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: <forward/drop/cpycpu/rdtcpu>\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_maccmd(cmd, &(entry.ipv4_arp_sg_vio_action),
                                       sizeof (fal_fwd_cmd_t));
            if (SW_OK != rv)
                dprintf("usage: <forward/drop/cpycpu/rdtcpu>\n");
        }

    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("arp_sg_port_en", "yes");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: <yes/no/y/n>\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_confirm(cmd, A_TRUE, &(entry.ipv4_arp_sg_port_en), sizeof (a_bool_t));
            if (SW_OK != rv)
                dprintf("usage: <yes/no/y/n>\n");
        }
    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("arp_sg_svlan_en", "yes");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: <yes/no/y/n>\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_confirm(cmd, A_TRUE, &(entry.ipv4_arp_sg_svlan_en), sizeof (a_bool_t));
            if (SW_OK != rv)
                dprintf("usage: <yes/no/y/n>\n");
        }
    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("arp_sg_cvlan_en", "yes");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: <yes/no/y/n>\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_confirm(cmd, A_TRUE, &(entry.ipv4_arp_sg_cvlan_en), sizeof (a_bool_t));
            if (SW_OK != rv)
                dprintf("usage: <yes/no/y/n>\n");
        }
    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("arp_sg_unk_action", "forward");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: <forward/drop/cpycpu/rdtcpu>\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_maccmd(cmd, &(entry.ipv4_arp_src_unk_action),
                                       sizeof (fal_fwd_cmd_t));
            if (SW_OK != rv)
                dprintf("usage: <forward/drop/cpycpu/rdtcpu>\n");
        }

    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("nd_sg_en", "yes");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: <yes/no/y/n>\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_confirm(cmd, A_TRUE, &(entry.ip_nd_sg_en), sizeof (a_bool_t));
            if (SW_OK != rv)
                dprintf("usage: <yes/no/y/n>\n");
        }
    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("nd_sg_violation_action", "forward");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: <forward/drop/cpycpu/rdtcpu>\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_maccmd(cmd, &(entry.ip_nd_sg_vio_action),
                                       sizeof (fal_fwd_cmd_t));
            if (SW_OK != rv)
                dprintf("usage: <forward/drop/cpycpu/rdtcpu>\n");
        }

    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("nd_sg_port_en", "yes");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: <yes/no/y/n>\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_confirm(cmd, A_TRUE, &(entry.ip_nd_sg_port_en), sizeof (a_bool_t));
            if (SW_OK != rv)
                dprintf("usage: <yes/no/y/n>\n");
        }
    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("nd_sg_svlan_en", "yes");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: <yes/no/y/n>\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_confirm(cmd, A_TRUE, &(entry.ip_nd_sg_svlan_en), sizeof (a_bool_t));
            if (SW_OK != rv)
                dprintf("usage: <yes/no/y/n>\n");
        }
    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("nd_sg_cvlan_en", "yes");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: <yes/no/y/n>\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_confirm(cmd, A_TRUE, &(entry.ip_nd_sg_cvlan_en), sizeof (a_bool_t));
            if (SW_OK != rv)
                dprintf("usage: <yes/no/y/n>\n");
        }
    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("nd_sg_unk_action", "forward");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: <forward/drop/cpycpu/rdtcpu>\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_maccmd(cmd, &(entry.ip_nd_src_unk_action),
                                       sizeof (fal_fwd_cmd_t));
            if (SW_OK != rv)
                dprintf("usage: <forward/drop/cpycpu/rdtcpu>\n");
        }

    }
    while (talk_mode && (SW_OK != rv));

    *(fal_arp_sg_cfg_t *)val = entry;
    return SW_OK;
}

void
cmd_data_print_arp_sg(a_uint8_t * param_name, a_uint32_t * buf, a_uint32_t size)
{
    fal_arp_sg_cfg_t *entry;

    entry = (fal_arp_sg_cfg_t *) buf;
    dprintf("\n[arp_sg_en]:0x%x  [arp_sg_violation_action]:0x%x  [arp_sg_port_en]:0x%x",
            entry->ipv4_arp_sg_en, entry->ipv4_arp_sg_vio_action, entry->ipv4_arp_sg_port_en);
    dprintf("\n[arp_sg_svlan_en]:0x%x  [arp_sg_cvlan_en]:0x%x  [arp_src_unk_action]:0x%x",
            entry->ipv4_arp_sg_svlan_en, entry->ipv4_arp_sg_cvlan_en, entry->ipv4_arp_src_unk_action);

    dprintf("\n[nd_sg_en]:0x%x  [nd_sg_violation_action]:0x%x  [nd_sg_port_en]:0x%x",
            entry->ip_nd_sg_en, entry->ip_nd_sg_vio_action, entry->ip_nd_sg_port_en);
    dprintf("\n[nd_sg_svlan_en]:0x%x  [nd_sg_cvlan_en]:0x%x  [nd_src_unk_action]:0x%x",
            entry->ip_nd_sg_svlan_en, entry->ip_nd_sg_cvlan_en, entry->ip_nd_src_unk_action);
}

sw_error_t
cmd_data_check_network_route(char *cmd_str, void * val, a_uint32_t size)
{
    char *cmd;
    a_uint32_t tmp;
    sw_error_t rv;
    fal_network_route_entry_t entry;

    aos_mem_zero(&entry, sizeof (fal_network_route_entry_t));

    do
    {
        cmd = get_sub_cmd("type", "0");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: 0 for ipv4 and 1 for ipv6 \n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_uint8(cmd, &tmp, sizeof (a_uint32_t));
            if (SW_OK != rv)
            {
                dprintf("usage: 0 for ipv4 and 1 for ipv6 \n");
            }
            else
            {
                entry.type = tmp;
            }
        }
    }
    while (talk_mode && (SW_OK != rv));

    if (entry.type == 0) /*IPv4*/
    {
        cmd_data_check_element("ip4 _addr", NULL,
                               "usage: the format is xx.xx.xx.xx \n",
                               cmd_data_check_ip4addr, (cmd, &(entry.route_addr.ip4_addr), 4));
    }
    else if (entry.type == 1) /*IPv6*/
    {
        cmd_data_check_element("ip6_addr", NULL,
                               "usage: the format is xxxx::xx.xx \n",
                               cmd_data_check_ip6addr, (cmd, &(entry.route_addr.ip6_addr), 16));
    }
    else
    {
        return SW_BAD_VALUE;
    }

    if (entry.type == 0) /*IPv4*/
    {
        cmd_data_check_element("ip4_addr_mask", NULL,
                               "usage: the format is xx.xx.xx.xx \n",
                               cmd_data_check_ip4addr, (cmd, &(entry.route_addr_mask.ip4_addr_mask), 4));
    }
    else if (entry.type == 1) /*IPv6*/
    {
        cmd_data_check_element("ip6_addr_mask", NULL,
                               "usage: the format is xxxx::xxxx \n",
                               cmd_data_check_ip6addr, (cmd, &(entry.route_addr_mask.ip6_addr_mask), 16));
    }
    else
    {
        return SW_BAD_VALUE;
    }

    do
    {
        cmd = get_sub_cmd("action", "forward");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: <forward/drop/cpycpu/rdtcpu>\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_maccmd(cmd, &(entry.action),
                                       sizeof (fal_fwd_cmd_t));
            if (SW_OK != rv)
                dprintf("usage: <forward/drop/cpycpu/rdtcpu>\n");
        }

    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("dst_info", "0");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: Dst info \n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_uint32(cmd, &(entry.dst_info), sizeof (a_uint32_t));
            if (SW_OK != rv)
                dprintf("usage: dst info \n");
        }
    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("lan_wan", "0");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: 0 for lan and 1 for wan \n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_uint8(cmd, &tmp, sizeof (a_uint32_t));
            if (SW_OK != rv)
            {
                dprintf("usage: 0 for lan and 1 for wan \n");
            }
            else
            {
                entry.lan_wan = tmp;
            }
        }
    }
    while (talk_mode && (SW_OK != rv));

    *(fal_network_route_entry_t *)val = entry;
    return SW_OK;
}

void
cmd_data_print_network_route(a_uint8_t * param_name, a_uint32_t * buf, a_uint32_t size)
{
    fal_network_route_entry_t *entry;

    entry = (fal_network_route_entry_t *) buf;
    dprintf("\n[type]:0x%x ",
            entry->type);
    
    dprintf("\n[dst_info]:0x%x  [lan_wan]:0x%x [action]:0x%x",
            entry->dst_info, entry->lan_wan, entry->action);

    if (entry->type == 0) {
        cmd_data_print_ip4addr("\n[ip4_addr]:",
                               (a_uint32_t *) & (entry->route_addr.ip4_addr),
                               sizeof (fal_ip4_addr_t));
        cmd_data_print_ip4addr("\n[ip4_addr_mask]:",
                               (a_uint32_t *) & (entry->route_addr_mask.ip4_addr_mask),
                               sizeof (fal_ip4_addr_t));
    } else {
        cmd_data_print_ip6addr("\n[ip6_addr]:",
                               (a_uint32_t *) & (entry->route_addr.ip6_addr),
                               sizeof (fal_ip6_addr_t));
        cmd_data_print_ip6addr("\n[ip6_addr_mask]:",
                               (a_uint32_t *) & (entry->route_addr_mask.ip6_addr_mask),
                               sizeof (fal_ip6_addr_t));
    }
}

sw_error_t
cmd_data_check_global_qinqmode(char *info, void *val, a_uint32_t size)
{
    char *cmd;
    sw_error_t rv;
    fal_global_qinq_mode_t *pEntry = (fal_global_qinq_mode_t *)val;

    memset(pEntry, 0, sizeof(fal_global_qinq_mode_t));

    /* get mask */
    do
    {
        cmd = get_sub_cmd("mask", "0x0");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: <bit 0 for ingress and bit 1 for egress>\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_uint32(cmd, &(pEntry->mask), sizeof(a_uint32_t));
            if (SW_OK != rv)
                dprintf("usage: <bit 0 for ingress and bit 1 for egress>\n");
        }
    }while (talk_mode && (SW_OK != rv));

    /* get ingress mode */
    do
    {
        cmd = get_sub_cmd("ingress_qinq_mode", "ctag");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: <stag/ctag>\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_qinq_mode(cmd, &(pEntry->ingress_mode), sizeof(a_uint32_t));
            if (SW_OK != rv)
                dprintf("usage: <stag/ctag>\n");
        }
    }while (talk_mode && (SW_OK != rv));

    /* get egress mode */
    do
    {
        cmd = get_sub_cmd("egress_qinq_mode", "ctag");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: <stag/ctag>\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_qinq_mode(cmd, &(pEntry->egress_mode), sizeof(a_uint32_t));
            if (SW_OK != rv)
                dprintf("usage: <stag/ctag>\n");
        }
    }while (talk_mode && (SW_OK != rv));

    return SW_OK;
}

void
cmd_data_print_global_qinqmode(a_uint8_t * param_name, a_uint32_t * buf, a_uint32_t size)
{
    fal_global_qinq_mode_t *entry;

    entry = (fal_global_qinq_mode_t *) buf;
    dprintf("\n");
    dprintf("[mask]:%d\n", entry->mask);

    cmd_data_print_qinq_mode("ingress_qinq_mode",
				(a_uint32_t *) & (entry->ingress_mode),
				sizeof(a_uint32_t));

    cmd_data_print_qinq_mode("egress_qinq_mode",
				(a_uint32_t *) & (entry->egress_mode),
				sizeof(a_uint32_t));

}

sw_error_t
cmd_data_check_port_qinqmode(char *info, void *val, a_uint32_t size)
{
    char *cmd;
    sw_error_t rv;
    fal_port_qinq_role_t *pEntry = (fal_port_qinq_role_t *)val;

    memset(pEntry, 0, sizeof(fal_port_qinq_role_t));

    /* get mask */
    do
    {
        cmd = get_sub_cmd("mask", "0x0");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: <bit 0 for ingress and bit 1 for egress>\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_uint32(cmd, &(pEntry->mask), sizeof(a_uint32_t));
            if (SW_OK != rv)
                dprintf("usage: <bit 0 for ingress and bit 1 for egress>\n");
        }
    }while (talk_mode && (SW_OK != rv));

    /* get ingress mode */
    do
    {
        cmd = get_sub_cmd("ingress_qinq_role", "edge");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: <edge/core>\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_qinq_role(cmd, &(pEntry->ingress_port_role), sizeof(a_uint32_t));
            if (SW_OK != rv)
                dprintf("usage: <edge/core>\n");
        }
    }while (talk_mode && (SW_OK != rv));

    /* get egress mode */
    do
    {
        cmd = get_sub_cmd("egress_qinq_role", "edge");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: <edge/core>\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_qinq_role(cmd, &(pEntry->egress_port_role), sizeof(a_uint32_t));
            if (SW_OK != rv)
                dprintf("usage: <edge/core>\n");
        }
    }while (talk_mode && (SW_OK != rv));

    return SW_OK;
}

void
cmd_data_print_port_qinqmode(a_uint8_t * param_name, a_uint32_t * buf, a_uint32_t size)
{
    fal_port_qinq_role_t *entry;

    entry = (fal_port_qinq_role_t *) buf;
    dprintf("\n");
    dprintf("[mask]:%d\n", entry->mask);

    cmd_data_print_qinq_role("ingress_qinq_role",
				(a_uint32_t *) & (entry->ingress_port_role),
				sizeof(a_uint32_t));

    cmd_data_print_qinq_role("egress_qinq_role",
				(a_uint32_t *) & (entry->egress_port_role),
				sizeof(a_uint32_t));

}

sw_error_t
cmd_data_check_tpid(char *info, void *val, a_uint32_t size)
{
    char *cmd;
    sw_error_t rv;
    fal_tpid_t *pEntry = (fal_tpid_t *)val;
    a_uint32_t tmp = 0;

    memset(pEntry, 0, sizeof(fal_tpid_t));

    /* get mask */
    do
    {
        cmd = get_sub_cmd("mask", "0x0");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: <bit 0 for ctpid and bit 1 for stpid>\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_uint32(cmd, &(pEntry->mask), sizeof(a_uint32_t));
            if (SW_OK != rv)
                dprintf("usage: <bit 0 for ctpid and bit 1 for stpid>\n");
        }
    }while (talk_mode && (SW_OK != rv));

    /* get ctpid */
    do
    {
        cmd = get_sub_cmd("ctagtpid", "0x8100");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: <0x8100>\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_uint16(cmd, &tmp, sizeof(a_uint32_t));
            if (SW_OK != rv)
                dprintf("usage: <0x8100>\n");

	     pEntry->ctpid = (a_uint16_t)tmp;
        }
    }while (talk_mode && (SW_OK != rv));

    /* get stpid */
    do
    {
        cmd = get_sub_cmd("stagtpid", "0x88a8");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: <0x88a8>\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_uint16(cmd, &tmp, sizeof(a_uint32_t));
            if (SW_OK != rv)
                dprintf("usage: <0x88a8>\n");

	     pEntry->stpid = (a_uint16_t)tmp;
        }
    }while (talk_mode && (SW_OK != rv));

    return SW_OK;
}

void
cmd_data_print_tpid(a_uint8_t * param_name, a_uint32_t * buf, a_uint32_t size)
{
    fal_tpid_t *entry;

    entry = (fal_tpid_t *) buf;
    dprintf("\n");
    dprintf("[mask]:%d\n", entry->mask);
    dprintf("[ctagtpid]:0x%x\n", entry->ctpid);
    dprintf("[stagtpid]:0x%x\n", entry->stpid);

}

sw_error_t
cmd_data_check_ingress_filter(char *info, void *val, a_uint32_t size)
{
    char *cmd;
    sw_error_t rv;
    fal_ingress_vlan_filter_t *pEntry = (fal_ingress_vlan_filter_t *)val;

    memset(pEntry, 0, sizeof(fal_ingress_vlan_filter_t));

    /* get in vlan filter */
    do
    {
        cmd = get_sub_cmd("membership_filter_en", "disable");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: <enable/disable>\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_enable(cmd, &(pEntry->membership_filter), sizeof(a_uint32_t));
            if (SW_OK != rv)
                dprintf("usage: <enable/disable>\n");
        }
    }while (talk_mode && (SW_OK != rv));

    /* get tag filter */
    do
    {
        cmd = get_sub_cmd("tagged_filter_en", "disable");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: <enable/disable>\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_enable(cmd, &(pEntry->tagged_filter), sizeof(a_uint32_t));
            if (SW_OK != rv)
                dprintf("usage: <enable/disable>\n");
        }
    }while (talk_mode && (SW_OK != rv));

    /* get untag filter */
    do
    {
        cmd = get_sub_cmd("untagged_filter_en", "disable");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: <enable/disable>\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_enable(cmd, &(pEntry->untagged_filter), sizeof(a_uint32_t));
            if (SW_OK != rv)
                dprintf("usage: <enable/disable>\n");
        }
    }while (talk_mode && (SW_OK != rv));

    /* get priority tag filter */
    do
    {
        cmd = get_sub_cmd("priority_tagged_filter_en", "disable");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: <enable/disable>\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_enable(cmd, &(pEntry->priority_filter), sizeof(a_uint32_t));
            if (SW_OK != rv)
                dprintf("usage: <enable/disable>\n");
        }
    }while (talk_mode && (SW_OK != rv));

    return SW_OK;
}

void
cmd_data_print_ingress_filter(a_uint8_t * param_name, a_uint32_t * buf, a_uint32_t size)
{
    fal_ingress_vlan_filter_t *entry;

    entry = (fal_ingress_vlan_filter_t *) buf;
    dprintf("\n");

    cmd_data_print_enable("membership_filter_en", (a_uint32_t *) & (entry->membership_filter), 4);
    dprintf("\n");
    cmd_data_print_enable("tagged_filter_en", (a_uint32_t *) & (entry->tagged_filter), 4);
    dprintf("\n");
    cmd_data_print_enable("untagged_filter_en", (a_uint32_t *) & (entry->untagged_filter), 4);
    dprintf("\n");
    cmd_data_print_enable("priority_tagged_filter_en", (a_uint32_t *) &
        (entry->priority_filter), 4);
    dprintf("\n");

}

sw_error_t
cmd_data_check_port_default_vid_en(char *info, void *val, a_uint32_t size)
{
    char *cmd;
    sw_error_t rv;
    fal_port_default_vid_enable_t *pEntry = (fal_port_default_vid_enable_t *)val;

    memset(pEntry, 0, sizeof(fal_port_default_vid_enable_t));

    do
    {
        cmd = get_sub_cmd("default_ctag_vid_en", "disable");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: <enable/disable>\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_enable(cmd, &(pEntry->default_cvid_en), sizeof(a_uint32_t));
            if (SW_OK != rv)
                dprintf("usage: <enable/disable>\n");
        }
    }while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("default_stag_vid_en", "disable");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: <enable/disable>\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_enable(cmd, &(pEntry->default_svid_en), sizeof(a_uint32_t));
            if (SW_OK != rv)
                dprintf("usage: <enable/disable>\n");
        }
    }while (talk_mode && (SW_OK != rv));

    return SW_OK;
}

void
cmd_data_print_port_default_vid_en(a_uint8_t * param_name, a_uint32_t * buf, a_uint32_t size)
{
    fal_port_default_vid_enable_t *entry;

    entry = (fal_port_default_vid_enable_t *) buf;
    dprintf("\n");

    cmd_data_print_enable("default_ctag_vid_en", (a_uint32_t *) & (entry->default_cvid_en), 4);
    dprintf("\n");
    cmd_data_print_enable("default_stag_vid_en", (a_uint32_t *) & (entry->default_svid_en), 4);
    dprintf("\n");

}

sw_error_t
cmd_data_check_port_vlan_tag(char *info, void *val, a_uint32_t size)
{
    char *cmd;
    sw_error_t rv;
    fal_port_vlan_tag_t *pEntry = (fal_port_vlan_tag_t *)val;
    a_uint32_t tmp = 0;

    memset(pEntry, 0, sizeof(fal_port_vlan_tag_t));

    /* get mask */
    do
    {
        cmd = get_sub_cmd("mask", "0x0");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: <bit 0 for cvid and bit 1 for svid; bit 2 for cpri and bit 3 for spri; bit 4 for cdei and bit 5 for sdei>\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_uint32(cmd, &(pEntry->mask), sizeof(a_uint32_t));
            if (SW_OK != rv)
                dprintf("usage: <bit 0 for cvid and bit 1 for svid; bit 2 for cpri and bit 3 for spri; bit 4 for cdei and bit 5 for sdei>\n");
        }
    }while (talk_mode && (SW_OK != rv));

    /* get cvid */
    do
    {
        cmd = get_sub_cmd("default_ctag_vid", "0");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: <0-4095>\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_uint16(cmd, &tmp, sizeof(a_uint32_t));
            if (SW_OK != rv)
                dprintf("usage: <0-4095>\n");

	     pEntry->cvid = (a_uint16_t)tmp;
        }
    }while (talk_mode && (SW_OK != rv));

    /* get svid */
    do
    {
        cmd = get_sub_cmd("default_stag_vid", "0");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: <0-4095>\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_uint16(cmd, &tmp, sizeof(a_uint32_t));
            if (SW_OK != rv)
                dprintf("usage: <0-4095>\n");

	     pEntry->svid = (a_uint16_t)tmp;
        }
    }while (talk_mode && (SW_OK != rv));

    /* get cpri */
    do
    {
        cmd = get_sub_cmd("default_ctag_pri", "0");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: <0-7>\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_uint16(cmd, &tmp, sizeof(a_uint32_t));
            if (SW_OK != rv)
                dprintf("usage: <0-7>\n");

	     pEntry->cpri = (a_uint16_t)tmp;
        }
    }while (talk_mode && (SW_OK != rv));

    /* get spri */
    do
    {
        cmd = get_sub_cmd("default_stag_pri", "0");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: <0-7>\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_uint16(cmd, &tmp, sizeof(a_uint32_t));
            if (SW_OK != rv)
                dprintf("usage: <0-7>\n");

	     pEntry->spri = (a_uint16_t)tmp;
        }
    }while (talk_mode && (SW_OK != rv));

    /* get cdei */
    do
    {
        cmd = get_sub_cmd("default_ctag_dei", "0");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: <0-1>\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_uint16(cmd, &tmp, sizeof(a_uint32_t));
            if (SW_OK != rv)
                dprintf("usage: <0-1>\n");

	     pEntry->cdei = (a_uint16_t)tmp;
        }
    }while (talk_mode && (SW_OK != rv));

    /* get sdei */
    do
    {
        cmd = get_sub_cmd("default_stag_dei", "0");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: <0-1>\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_uint16(cmd, &tmp, sizeof(a_uint32_t));
            if (SW_OK != rv)
                dprintf("usage: <0-1>\n");

	     pEntry->sdei = (a_uint16_t)tmp;
        }
    }while (talk_mode && (SW_OK != rv));

    return SW_OK;
}

void
cmd_data_print_port_vlan_tag(a_uint8_t * param_name, a_uint32_t * buf, a_uint32_t size)
{
    fal_port_vlan_tag_t *entry;

    entry = (fal_port_vlan_tag_t *) buf;
    dprintf("\n");

    cmd_data_print_uint32("mask", (a_uint32_t *) & (entry->mask), 4);
    dprintf("\n");
    cmd_data_print_uint16("default_ctag_vid", (a_uint32_t *) & (entry->cvid), 4);
    dprintf("\n");
    cmd_data_print_uint16("default_stag_vid", (a_uint32_t *) & (entry->svid), 4);
    dprintf("\n");
    cmd_data_print_uint16("default_ctag_pri", (a_uint32_t *) & (entry->cpri), 4);
    dprintf("\n");
    cmd_data_print_uint16("default_stag_pri", (a_uint32_t *) & (entry->spri), 4);
    dprintf("\n");
    cmd_data_print_uint16("default_ctag_dei", (a_uint32_t *) & (entry->cdei), 4);
    dprintf("\n");
    cmd_data_print_uint16("default_stag_dei", (a_uint32_t *) & (entry->sdei), 4);
    dprintf("\n");

}

sw_error_t
cmd_data_check_port_vlan_direction(char *cmd_str, a_uint32_t * arg_val, a_uint32_t size)
{
    if (cmd_str == NULL)
        return SW_BAD_PARAM;

    if (!strcasecmp(cmd_str, "all"))
    {
        *arg_val = FAL_PORT_VLAN_ALL;
    }
    else if (!strcasecmp(cmd_str, "ingress"))
    {
        *arg_val = FAL_PORT_VLAN_INGRESS;
    }
    else if (!strcasecmp(cmd_str, "egress"))
    {
        *arg_val = FAL_PORT_VLAN_EGRESS;
    }
    else
    {
        //dprintf("input error \n");
        return SW_BAD_VALUE;
    }

    return SW_OK;
}

void
cmd_data_print_port_vlan_direction(a_uint8_t * param_name, a_uint32_t * buf, a_uint32_t size)
{
    dprintf("[%s]:", param_name);
    if (*(a_uint32_t *) buf == FAL_PORT_VLAN_ALL)
    {
        dprintf("ALL");
    }
    else if (*(a_uint32_t *) buf == FAL_PORT_VLAN_INGRESS)
    {
        dprintf("INGRESS");
    }
    else if (*(a_uint32_t *) buf == FAL_PORT_VLAN_EGRESS)
    {
        dprintf("EGRESS");
    }
    else
    {
        dprintf("UNKNOWN VALUE");
    }

}

sw_error_t
cmd_data_check_port_vlan_translation_adv_rule(char *info, fal_vlan_trans_adv_rule_t *val, a_uint32_t size)
{
	char *cmd = NULL;
	a_uint32_t tmp;
	sw_error_t rv;
	fal_vlan_trans_adv_rule_t entry;

	memset(&entry, 0, sizeof (fal_vlan_trans_adv_rule_t));

	do
	{
		cmd = get_sub_cmd("stagformat", "0");
		SW_RTN_ON_NULL_PARAM(cmd);

		if (!strncasecmp(cmd, "quit", 4))
		{
			return SW_BAD_VALUE;
		}
		else if (!strncasecmp(cmd, "help", 4))
		{
			dprintf("usage: bit 0 for untagged, bit 1 for priority tagged and bit 2 "
				"for tagged\n");
			rv = SW_BAD_VALUE;
		}
		else
		{
			rv = cmd_data_check_uint8(cmd, &tmp, sizeof (a_uint32_t));
			if (SW_OK != rv)
			{
				dprintf("usage: bit 0 for untagged, bit 1 for priority tagged and "
					"bit 2 for tagged\n");
			}
			else
			{
				entry.s_tagged = tmp;
			}
		}
	}
	while (talk_mode && (SW_OK != rv));

	do
	{
		cmd = get_sub_cmd("svid_en", "yes");
		SW_RTN_ON_NULL_PARAM(cmd);

		if (!strncasecmp(cmd, "quit", 4))
		{
			return SW_BAD_VALUE;

		}
		else if (!strncasecmp(cmd, "help", 4))
		{
			dprintf("usage: <yes/no/y/n>\n");
			rv = SW_BAD_VALUE;

		}
		else
		{
			rv = cmd_data_check_confirm(cmd, A_TRUE, &entry.s_vid_enable,
					sizeof (a_bool_t));
			if (SW_OK != rv)
				dprintf("usage: <yes/no/y/n>\n");
		}

	}
	while (talk_mode && (SW_OK != rv));

	do
	{
		cmd = get_sub_cmd("svid", "0");
		SW_RTN_ON_NULL_PARAM(cmd);

		if (!strncasecmp(cmd, "quit", 4))
		{
			return SW_BAD_VALUE;

		}
		else if (!strncasecmp(cmd, "help", 4))
		{
			dprintf("usage: the range is 0 -- 4095\n");
			rv = SW_BAD_VALUE;

		}
		else
		{
			rv = cmd_data_check_uint32(cmd, &entry.s_vid, sizeof (a_uint32_t));
			if (SW_OK != rv)
				dprintf("usage: the range is 0 -- 4095\n");
		}

	}
	while (talk_mode && (SW_OK != rv));

	do
	{
		cmd = get_sub_cmd("spcp_en", "yes");
		SW_RTN_ON_NULL_PARAM(cmd);

		if (!strncasecmp(cmd, "quit", 4))
		{
			return SW_BAD_VALUE;
		}
		else if (!strncasecmp(cmd, "help", 4))
		{
			dprintf("usage: <yes/no/y/n>\n");
			rv = SW_BAD_VALUE;
		}
		else
		{
			rv = cmd_data_check_confirm(cmd, A_TRUE, &entry.s_pcp_enable,
					sizeof (a_bool_t));
			if (SW_OK != rv)
				dprintf("usage: <yes/no/y/n>\n");
		}

	}
	while (talk_mode && (SW_OK != rv));

	do
	{
		cmd = get_sub_cmd("spcp", "0");
		SW_RTN_ON_NULL_PARAM(cmd);

		if (!strncasecmp(cmd, "quit", 4))
		{
			return SW_BAD_VALUE;
		}
		else if (!strncasecmp(cmd, "help", 4))
		{
			dprintf("usage: the range is 0 -- 7\n");
			rv = SW_BAD_VALUE;
		}
		else
		{
			rv = cmd_data_check_uint8(cmd, &tmp, sizeof (a_uint32_t));
			if (SW_OK != rv)
			{
				dprintf("usage: the range is 0 -- 7\n");
			}
			else
			{
				entry.s_pcp = tmp;
			}
		}
	}
	while (talk_mode && (SW_OK != rv));

	do
	{
		cmd = get_sub_cmd("sdei_en", "yes");
		SW_RTN_ON_NULL_PARAM(cmd);

		if (!strncasecmp(cmd, "quit", 4))
		{
			return SW_BAD_VALUE;
		}
		else if (!strncasecmp(cmd, "help", 4))
		{
			dprintf("usage: <yes/no/y/n>\n");
			rv = SW_BAD_VALUE;
		}
		else
		{
			rv = cmd_data_check_confirm(cmd, A_TRUE, &entry.s_dei_enable,
					sizeof (a_bool_t));
			if (SW_OK != rv)
				dprintf("usage: <yes/no/y/n>\n");
		}

	}
	while (talk_mode && (SW_OK != rv));

	do
	{
		cmd = get_sub_cmd("sdei", "0");
		SW_RTN_ON_NULL_PARAM(cmd);

		if (!strncasecmp(cmd, "quit", 4))
		{
			return SW_BAD_VALUE;
		}
		else if (!strncasecmp(cmd, "help", 4))
		{
			dprintf("usage: the range is 0 -- 1\n");
			rv = SW_BAD_VALUE;
		}
		else
		{
			rv = cmd_data_check_uint8(cmd, &tmp, sizeof (a_uint32_t));
			if (SW_OK != rv)
			{
				dprintf("usage: the range is 0 -- 1\n");
			}
			else
			{
				entry.s_dei = tmp;
			}
		}
	}
	while (talk_mode && (SW_OK != rv));

	do
	{
		cmd = get_sub_cmd("ctagformat", "0");
		SW_RTN_ON_NULL_PARAM(cmd);

		if (!strncasecmp(cmd, "quit", 4))
		{
			return SW_BAD_VALUE;
		}
		else if (!strncasecmp(cmd, "help", 4))
		{
			dprintf("usage: bit 0 for untagged, bit 1 for priority tagged and "
				"bit 2 for tagged\n");
			rv = SW_BAD_VALUE;
		}
		else
		{
			rv = cmd_data_check_uint8(cmd, &tmp, sizeof (a_uint32_t));
			if (SW_OK != rv)
			{
				dprintf("usage: bit 0 for untagged, bit 1 for priority tagged "
					"and bit 2 for tagged\n");
			}
			else
			{
				entry.c_tagged = tmp;
			}
		}
	}
	while (talk_mode && (SW_OK != rv));

	do
	{
		cmd = get_sub_cmd("cvid_en", "yes");
		SW_RTN_ON_NULL_PARAM(cmd);

		if (!strncasecmp(cmd, "quit", 4))
		{
			return SW_BAD_VALUE;

		}
		else if (!strncasecmp(cmd, "help", 4))
		{
			dprintf("usage: <yes/no/y/n>\n");
			rv = SW_BAD_VALUE;

		}
		else
		{
			rv = cmd_data_check_confirm(cmd, A_TRUE, &entry.c_vid_enable,
					sizeof (a_bool_t));
			if (SW_OK != rv)
				dprintf("usage: <yes/no/y/n>\n");
		}

	}
	while (talk_mode && (SW_OK != rv));

	do
	{
		cmd = get_sub_cmd("cvid", "0");
		SW_RTN_ON_NULL_PARAM(cmd);

		if (!strncasecmp(cmd, "quit", 4))
		{
			return SW_BAD_VALUE;

		}
		else if (!strncasecmp(cmd, "help", 4))
		{
			dprintf("usage: the range is 0 -- 4095\n");
			rv = SW_BAD_VALUE;

		}
		else
		{
			rv = cmd_data_check_uint32(cmd, &entry.c_vid, sizeof (a_uint32_t));
			if (SW_OK != rv)
				dprintf("usage: the range is 0 -- 4095\n");
		}

	}
	while (talk_mode && (SW_OK != rv));

	do
	{
		cmd = get_sub_cmd("cpcp_en", "yes");
		SW_RTN_ON_NULL_PARAM(cmd);

		if (!strncasecmp(cmd, "quit", 4))
		{
			return SW_BAD_VALUE;
		}
		else if (!strncasecmp(cmd, "help", 4))
		{
			dprintf("usage: <yes/no/y/n>\n");
			rv = SW_BAD_VALUE;
		}
		else
		{
			rv = cmd_data_check_confirm(cmd, A_TRUE, &entry.c_pcp_enable,
					sizeof (a_bool_t));
			if (SW_OK != rv)
				dprintf("usage: <yes/no/y/n>\n");
		}

	}
	while (talk_mode && (SW_OK != rv));

	do
	{
		cmd = get_sub_cmd("cpcp", "0");
		SW_RTN_ON_NULL_PARAM(cmd);

		if (!strncasecmp(cmd, "quit", 4))
		{
			return SW_BAD_VALUE;
		}
		else if (!strncasecmp(cmd, "help", 4))
		{
			dprintf("usage: the range is 0 -- 7\n");
			rv = SW_BAD_VALUE;
		}
		else
		{
			rv = cmd_data_check_uint8(cmd, &tmp, sizeof (a_uint32_t));
			if (SW_OK != rv)
			{
				dprintf("usage: the range is 0 -- 7\n");
			}
			else
			{
				entry.c_pcp = tmp;
			}
		}
	}
	while (talk_mode && (SW_OK != rv));

	do
	{
		cmd = get_sub_cmd("cdei_en", "yes");
		SW_RTN_ON_NULL_PARAM(cmd);

		if (!strncasecmp(cmd, "quit", 4))
		{
			return SW_BAD_VALUE;
		}
		else if (!strncasecmp(cmd, "help", 4))
		{
			dprintf("usage: <yes/no/y/n>\n");
			rv = SW_BAD_VALUE;
		}
		else
		{
			rv = cmd_data_check_confirm(cmd, A_TRUE, &entry.c_dei_enable,
					sizeof (a_bool_t));
			if (SW_OK != rv)
				dprintf("usage: <yes/no/y/n>\n");
		}

	}
	while (talk_mode && (SW_OK != rv));

	do
	{
		cmd = get_sub_cmd("cdei", "0");
		SW_RTN_ON_NULL_PARAM(cmd);

		if (!strncasecmp(cmd, "quit", 4))
		{
			return SW_BAD_VALUE;
		}
		else if (!strncasecmp(cmd, "help", 4))
		{
			dprintf("usage: the range is 0 -- 1\n");
			rv = SW_BAD_VALUE;
		}
		else
		{
			rv = cmd_data_check_uint8(cmd, &tmp, sizeof (a_uint32_t));
			if (SW_OK != rv)
			{
				dprintf("usage: the range is 0 -- 1\n");
			}
			else
			{
				entry.c_dei = tmp;
			}
		}
	}
	while (talk_mode && (SW_OK != rv));

	do
	{
		cmd = get_sub_cmd("frame_type_en", "yes");
		SW_RTN_ON_NULL_PARAM(cmd);

		if (!strncasecmp(cmd, "quit", 4))
		{
			return SW_BAD_VALUE;
		}
		else if (!strncasecmp(cmd, "help", 4))
		{
			dprintf("usage: <yes/no/y/n>\n");
			rv = SW_BAD_VALUE;
		}
		else
		{
			rv = cmd_data_check_confirm(cmd, A_FALSE, &entry.frmtype_enable,
					sizeof (a_bool_t));
			if (SW_OK != rv)
				dprintf("usage: <yes/no/y/n>\n");
		}
	}
	while (talk_mode && (SW_OK != rv));

	do
	{
		cmd = get_sub_cmd("frametype", "0");
		SW_RTN_ON_NULL_PARAM(cmd);

		if (!strncasecmp(cmd, "quit", 4))
		{
			return SW_BAD_VALUE;
		}
		else if (!strncasecmp(cmd, "help", 4))
		{
			dprintf("usage: 0 for ethernet, 1 for rfc1024, 2 for llc and 3 for ethernet or rfc1024\n");
			rv = SW_BAD_VALUE;
		}
		else
		{
			rv = cmd_data_check_uint32(cmd, &entry.frmtype, sizeof (a_uint32_t));
			if (SW_OK != rv)
				dprintf("usage: 0 for ethernet, 1 for rfc1024, 2 for llc and 3 for ethernet or rfc1024\n");
		}
	}
	while (talk_mode && (SW_OK != rv));

	do
	{
		cmd = get_sub_cmd("protocol_en", "yes");
		SW_RTN_ON_NULL_PARAM(cmd);

		if (!strncasecmp(cmd, "quit", 4))
		{
			return SW_BAD_VALUE;
		}
		else if (!strncasecmp(cmd, "help", 4))
		{
			dprintf("usage: <yes/no/y/n>\n");
			rv = SW_BAD_VALUE;
		}
		else
		{
			rv = cmd_data_check_confirm(cmd, A_FALSE, &entry.protocol_enable,
					sizeof (a_bool_t));
			if (SW_OK != rv)
				dprintf("usage: <yes/no/y/n>\n");
		}
	}
	while (talk_mode && (SW_OK != rv));

	do
	{
		cmd = get_sub_cmd("protocol", "0");
		SW_RTN_ON_NULL_PARAM(cmd);

		if (!strncasecmp(cmd, "quit", 4))
		{
			return SW_BAD_VALUE;
		}
		else if (!strncasecmp(cmd, "help", 4))
		{
			dprintf("usage: for example:0x0800 \n");
			rv = SW_BAD_VALUE;

		}
		else
		{
			rv = cmd_data_check_uint16(cmd, &tmp, sizeof (a_uint32_t));
			if (SW_OK != rv)
			{
				dprintf("usage: for example:0x0800 \n");
			}
			else
			{
				entry.protocol = tmp;
			}
		}
	}
	while (talk_mode && (SW_OK != rv));

	do
	{
		cmd = get_sub_cmd("vsivalid", "yes");
		SW_RTN_ON_NULL_PARAM(cmd);

		if (!strncasecmp(cmd, "quit", 4))
		{
			return SW_BAD_VALUE;
		}
		else if (!strncasecmp(cmd, "help", 4))
		{
			dprintf("usage: <yes/no/y/n>\n");
			rv = SW_BAD_VALUE;
		}
		else
		{
			rv = cmd_data_check_confirm(cmd, A_FALSE, &entry.vsi_valid,
					sizeof (a_bool_t));
			if (SW_OK != rv)
				dprintf("usage: <yes/no/y/n>\n");
		}
	}
	while (talk_mode && (SW_OK != rv));

	do
	{
		cmd = get_sub_cmd("vsi_en", "yes");
		SW_RTN_ON_NULL_PARAM(cmd);

		if (!strncasecmp(cmd, "quit", 4))
		{
			return SW_BAD_VALUE;
		}
		else if (!strncasecmp(cmd, "help", 4))
		{
			dprintf("usage: <yes/no/y/n>\n");
			rv = SW_BAD_VALUE;
		}
		else
		{
			rv = cmd_data_check_confirm(cmd, A_FALSE, &entry.vsi_enable,
					sizeof (a_bool_t));
			if (SW_OK != rv)
				dprintf("usage: <yes/no/y/n>\n");
		}
	}
	while (talk_mode && (SW_OK != rv));

	do
	{
		cmd = get_sub_cmd("vsi", "0");
		SW_RTN_ON_NULL_PARAM(cmd);

		if (!strncasecmp(cmd, "quit", 4))
		{
			return SW_BAD_VALUE;
		}
		else if (!strncasecmp(cmd, "help", 4))
		{
			dprintf("usage: the range is 0 -- 31\n");
			rv = SW_BAD_VALUE;
		}
		else
		{
			rv = cmd_data_check_uint32(cmd, &entry.vsi, sizeof (a_uint32_t));
			if (SW_OK != rv)
				dprintf("usage: the range is 0 -- 31\n");
		}
	}
	while (talk_mode && (SW_OK != rv));

	*val = entry;
	return SW_OK;
}

void
cmd_data_print_port_vlan_translation_adv_rule(a_uint8_t * param_name, a_uint32_t * buf, a_uint32_t size)
{
	fal_vlan_trans_adv_rule_t *entry;

	entry = (fal_vlan_trans_adv_rule_t *) buf;

	dprintf("\n\n rule field: ");
	dprintf("\n[port_bitmap]:0x%x", entry->port_bitmap);
	dprintf("\n[stagformat]:0x%x", entry->s_tagged);
	dprintf("\n[svid_en]:%s  [svid]:%d", entry->s_vid_enable?"ENABLE":"DISABLE", entry->s_vid);
	dprintf("\n[spcp_en]:%s  [spcp]:%d", entry->s_pcp_enable?"ENABLE":"DISABLE", entry->s_pcp);
	dprintf("\n[sdei_en]:%s  [sdei]:%d", entry->s_dei_enable?"ENABLE":"DISABLE", entry->s_dei);

	dprintf("\n[ctagformat]:0x%x", entry->c_tagged);
	dprintf("\n[cvid_en]:%s  [cvid]:%d", entry->c_vid_enable?"ENABLE":"DISABLE", entry->c_vid);
	dprintf("\n[cpcp_en]:%s  [cpcp]:%d", entry->c_pcp_enable?"ENABLE":"DISABLE", entry->c_pcp);
	dprintf("\n[cdei_en]:%s  [cdei]:%d", entry->c_dei_enable?"ENABLE":"DISABLE", entry->c_dei);

	dprintf("\n[frame_type_en]:%s  [frametype]:0x%x", entry->frmtype_enable?"ENABLE":"DISABLE",
		entry->frmtype);
	dprintf("\n[protocol_en]:%s  [protocol]:0x%x", entry->protocol_enable?"ENABLE":"DISABLE",
		entry->protocol);

	dprintf("\n[vsivalid]:%s  [vsi_en]:%s  [vsi]:%d\n\n", entry->vsi_valid?"ENABLE":"DISABLE",
			entry->vsi_enable?"ENABLE":"DISABLE", entry->vsi);
}

sw_error_t
cmd_data_check_port_vlan_translation_adv_action(char *info,
	fal_vlan_trans_adv_action_t *val, a_uint32_t size)
{
	char *cmd = NULL;
	a_uint32_t tmp;
	sw_error_t rv;
	fal_vlan_trans_adv_action_t entry;

	memset(&entry, 0, sizeof (fal_vlan_trans_adv_action_t));

	do
	{
		cmd = get_sub_cmd("swap_svid_cvid", "yes");
		SW_RTN_ON_NULL_PARAM(cmd);

		if (!strncasecmp(cmd, "quit", 4))
		{
			return SW_BAD_VALUE;
		}
		else if (!strncasecmp(cmd, "help", 4))
		{
			dprintf("usage: <yes/no/y/n>\n");
			rv = SW_BAD_VALUE;
		}
		else
		{
			rv = cmd_data_check_confirm(cmd, A_TRUE, &entry.swap_svid_cvid,
					sizeof (a_bool_t));
			if (SW_OK != rv)
				dprintf("usage: <yes/no/y/n>\n");
		}

	}
	while (talk_mode && (SW_OK != rv));

	do
	{
		cmd = get_sub_cmd("svid_translation_cmd", "0");
		SW_RTN_ON_NULL_PARAM(cmd);

		if (!strncasecmp(cmd, "quit", 4))
		{
			return SW_BAD_VALUE;
		}
		else if (!strncasecmp(cmd, "help", 4))
		{
			dprintf("usage: 0 for unchanged, 1 for add and replace and 2 for delete tag\n");
			rv = SW_BAD_VALUE;
		}
		else
		{
			rv = cmd_data_check_uint32(cmd, &entry.svid_xlt_cmd, sizeof (a_uint32_t));
			if (SW_OK != rv)
				dprintf("usage: 0 for unchanged, 1 for add and replace and 2 for delete tag\n");
		}
	}
	while (talk_mode && (SW_OK != rv));

	do
	{
		cmd = get_sub_cmd("svidtranslation", "0");
		SW_RTN_ON_NULL_PARAM(cmd);

		if (!strncasecmp(cmd, "quit", 4))
		{
			return SW_BAD_VALUE;
		}
		else if (!strncasecmp(cmd, "help", 4))
		{
			dprintf("usage: the range is 0 -- 4095\n");
			rv = SW_BAD_VALUE;
		}
		else
		{
			rv = cmd_data_check_uint16(cmd, &tmp, sizeof (a_uint32_t));
			if (SW_OK != rv)
			{
				dprintf("usage: the range is 0 -- 4095\n");
			}
			else
			{
				entry.svid_xlt = tmp;
			}
		}
	}
	while (talk_mode && (SW_OK != rv));

	do
	{
		cmd = get_sub_cmd("cvid_translation_cmd", "0");
		SW_RTN_ON_NULL_PARAM(cmd);

		if (!strncasecmp(cmd, "quit", 4))
		{
			return SW_BAD_VALUE;
		}
		else if (!strncasecmp(cmd, "help", 4))
		{
			dprintf("usage: 0 for unchanged, 1 for add and replace and 2 for delete tag\n");
			rv = SW_BAD_VALUE;
		}
		else
		{
			rv = cmd_data_check_uint32(cmd, &entry.cvid_xlt_cmd, sizeof (a_uint32_t));
			if (SW_OK != rv)
				dprintf("usage: 0 for unchanged, 1 for add and replace and 2 for delete tag\n");
		}
	}
	while (talk_mode && (SW_OK != rv));

	do
	{
		cmd = get_sub_cmd("cvidtranslation", "0");
		SW_RTN_ON_NULL_PARAM(cmd);

		if (!strncasecmp(cmd, "quit", 4))
		{
			return SW_BAD_VALUE;
		}
		else if (!strncasecmp(cmd, "help", 4))
		{
			dprintf("usage: the range is 0 -- 4095\n");
			rv = SW_BAD_VALUE;
		}
		else
		{
			rv = cmd_data_check_uint16(cmd, &tmp, sizeof (a_uint32_t));
			if (SW_OK != rv)
			{
				dprintf("usage: the range is 0 -- 4095\n");
			}
			else
			{
				entry.cvid_xlt = tmp;
			}
		}
	}
	while (talk_mode && (SW_OK != rv));

	do
	{
		cmd = get_sub_cmd("swap_spcp_cpcp", "yes");
		SW_RTN_ON_NULL_PARAM(cmd);

		if (!strncasecmp(cmd, "quit", 4))
		{
			return SW_BAD_VALUE;
		}
		else if (!strncasecmp(cmd, "help", 4))
		{
			dprintf("usage: <yes/no/y/n>\n");
			rv = SW_BAD_VALUE;
		}
		else
		{
			rv = cmd_data_check_confirm(cmd, A_TRUE, &entry.swap_spcp_cpcp,
					sizeof (a_bool_t));
			if (SW_OK != rv)
				dprintf("usage: <yes/no/y/n>\n");
		}

	}
	while (talk_mode && (SW_OK != rv));

	do
	{
		cmd = get_sub_cmd("spcp_translation_en", "yes");
		SW_RTN_ON_NULL_PARAM(cmd);

		if (!strncasecmp(cmd, "quit", 4))
		{
			return SW_BAD_VALUE;
		}
		else if (!strncasecmp(cmd, "help", 4))
		{
			dprintf("usage: <yes/no/y/n>\n");
			rv = SW_BAD_VALUE;
		}
		else
		{
			rv = cmd_data_check_confirm(cmd, A_TRUE, &entry.spcp_xlt_enable,
					sizeof (a_bool_t));
			if (SW_OK != rv)
				dprintf("usage: <yes/no/y/n>\n");
		}

	}
	while (talk_mode && (SW_OK != rv));

	do
	{
		cmd = get_sub_cmd("spcptranslation", "0");
		SW_RTN_ON_NULL_PARAM(cmd);

		if (!strncasecmp(cmd, "quit", 4))
		{
			return SW_BAD_VALUE;
		}
		else if (!strncasecmp(cmd, "help", 4))
		{
			dprintf("usage: the range is 0 -- 7\n");
			rv = SW_BAD_VALUE;
		}
		else
		{
			rv = cmd_data_check_uint8(cmd, &tmp, sizeof (a_uint32_t));
			if (SW_OK != rv)
			{
				dprintf("usage: the range is 0 -- 7\n");
			}
			else
			{
				entry.spcp_xlt = tmp;
			}
		}
	}
	while (talk_mode && (SW_OK != rv));

	do
	{
		cmd = get_sub_cmd("cpcp_translation_en", "yes");
		SW_RTN_ON_NULL_PARAM(cmd);

		if (!strncasecmp(cmd, "quit", 4))
		{
			return SW_BAD_VALUE;
		}
		else if (!strncasecmp(cmd, "help", 4))
		{
			dprintf("usage: <yes/no/y/n>\n");
			rv = SW_BAD_VALUE;
		}
		else
		{
			rv = cmd_data_check_confirm(cmd, A_TRUE, &entry.cpcp_xlt_enable,
					sizeof (a_bool_t));
			if (SW_OK != rv)
				dprintf("usage: <yes/no/y/n>\n");
		}

	}
	while (talk_mode && (SW_OK != rv));

	do
	{
		cmd = get_sub_cmd("cpcptranslation", "0");
		SW_RTN_ON_NULL_PARAM(cmd);

		if (!strncasecmp(cmd, "quit", 4))
		{
			return SW_BAD_VALUE;
		}
		else if (!strncasecmp(cmd, "help", 4))
		{
			dprintf("usage: the range is 0 -- 7\n");
			rv = SW_BAD_VALUE;
		}
		else
		{
			rv = cmd_data_check_uint8(cmd, &tmp, sizeof (a_uint32_t));
			if (SW_OK != rv)
			{
				dprintf("usage: the range is 0 -- 7\n");
			}
			else
			{
				entry.cpcp_xlt = tmp;
			}
		}
	}
	while (talk_mode && (SW_OK != rv));

	do
	{
		cmd = get_sub_cmd("swap_sdei_cdei", "yes");
		SW_RTN_ON_NULL_PARAM(cmd);

		if (!strncasecmp(cmd, "quit", 4))
		{
			return SW_BAD_VALUE;
		}
		else if (!strncasecmp(cmd, "help", 4))
		{
			dprintf("usage: <yes/no/y/n>\n");
			rv = SW_BAD_VALUE;
		}
		else
		{
			rv = cmd_data_check_confirm(cmd, A_TRUE, &entry.swap_sdei_cdei,
					sizeof (a_bool_t));
			if (SW_OK != rv)
				dprintf("usage: <yes/no/y/n>\n");
		}

	}
	while (talk_mode && (SW_OK != rv));

	do
	{
		cmd = get_sub_cmd("sdei_translation_en", "yes");
		SW_RTN_ON_NULL_PARAM(cmd);

		if (!strncasecmp(cmd, "quit", 4))
		{
			return SW_BAD_VALUE;
		}
		else if (!strncasecmp(cmd, "help", 4))
		{
			dprintf("usage: <yes/no/y/n>\n");
			rv = SW_BAD_VALUE;
		}
		else
		{
			rv = cmd_data_check_confirm(cmd, A_TRUE, &entry.sdei_xlt_enable,
					sizeof (a_bool_t));
			if (SW_OK != rv)
				dprintf("usage: <yes/no/y/n>\n");
		}

	}
	while (talk_mode && (SW_OK != rv));

	do
	{
		cmd = get_sub_cmd("sdeitranslation", "0");
		SW_RTN_ON_NULL_PARAM(cmd);

		if (!strncasecmp(cmd, "quit", 4))
		{
			return SW_BAD_VALUE;
		}
		else if (!strncasecmp(cmd, "help", 4))
		{
			dprintf("usage: the range is 0 -- 1\n");
			rv = SW_BAD_VALUE;
		}
		else
		{
			rv = cmd_data_check_uint8(cmd, &tmp, sizeof (a_uint32_t));
			if (SW_OK != rv)
			{
				dprintf("usage: the range is 0 -- 1\n");
			}
			else
			{
				entry.sdei_xlt = tmp;
			}
		}
	}
	while (talk_mode && (SW_OK != rv));

	do
	{
		cmd = get_sub_cmd("cdei_translation_en", "yes");
		SW_RTN_ON_NULL_PARAM(cmd);

		if (!strncasecmp(cmd, "quit", 4))
		{
			return SW_BAD_VALUE;
		}
		else if (!strncasecmp(cmd, "help", 4))
		{
			dprintf("usage: <yes/no/y/n>\n");
			rv = SW_BAD_VALUE;
		}
		else
		{
			rv = cmd_data_check_confirm(cmd, A_TRUE, &entry.cdei_xlt_enable,
					sizeof (a_bool_t));
			if (SW_OK != rv)
				dprintf("usage: <yes/no/y/n>\n");
		}

	}
	while (talk_mode && (SW_OK != rv));

	do
	{
		cmd = get_sub_cmd("cdeitranslation", "0");
		SW_RTN_ON_NULL_PARAM(cmd);

		if (!strncasecmp(cmd, "quit", 4))
		{
			return SW_BAD_VALUE;
		}
		else if (!strncasecmp(cmd, "help", 4))
		{
			dprintf("usage: the range is 0 -- 7\n");
			rv = SW_BAD_VALUE;
		}
		else
		{
			rv = cmd_data_check_uint8(cmd, &tmp, sizeof (a_uint32_t));
			if (SW_OK != rv)
			{
				dprintf("usage: the range is 0 -- 7\n");
			}
			else
			{
				entry.cdei_xlt = tmp;
			}
		}
	}
	while (talk_mode && (SW_OK != rv));

	do
	{
		cmd = get_sub_cmd("counter_en", "yes");
		SW_RTN_ON_NULL_PARAM(cmd);

		if (!strncasecmp(cmd, "quit", 4))
		{
			return SW_BAD_VALUE;
		}
		else if (!strncasecmp(cmd, "help", 4))
		{
			dprintf("usage: <yes/no/y/n>\n");
			rv = SW_BAD_VALUE;
		}
		else
		{
			rv = cmd_data_check_confirm(cmd, A_TRUE, &entry.counter_enable,
					sizeof (a_bool_t));
			if (SW_OK != rv)
				dprintf("usage: <yes/no/y/n>\n");
		}

	}
	while (talk_mode && (SW_OK != rv));

	do
	{
		cmd = get_sub_cmd("counter_id", "0");
		SW_RTN_ON_NULL_PARAM(cmd);

		if (!strncasecmp(cmd, "quit", 4))
		{
			return SW_BAD_VALUE;
		}
		else if (!strncasecmp(cmd, "help", 4))
		{
			dprintf("usage: the range is 0 -- 63\n");
			rv = SW_BAD_VALUE;
		}
		else
		{
			rv = cmd_data_check_uint8(cmd, &tmp, sizeof (a_uint32_t));
			if (SW_OK != rv)
			{
				dprintf("usage: the range is 0 -- 53\n");
			}
			else
			{
				entry.counter_id = tmp;
			}
		}
	}
	while (talk_mode && (SW_OK != rv));

	do
	{
		cmd = get_sub_cmd("vsi_translation_en", "yes");
		SW_RTN_ON_NULL_PARAM(cmd);

		if (!strncasecmp(cmd, "quit", 4))
		{
			return SW_BAD_VALUE;
		}
		else if (!strncasecmp(cmd, "help", 4))
		{
			dprintf("usage: <yes/no/y/n>\n");
			rv = SW_BAD_VALUE;
		}
		else
		{
			rv = cmd_data_check_confirm(cmd, A_TRUE, &entry.vsi_xlt_enable,
					sizeof (a_bool_t));
			if (SW_OK != rv)
				dprintf("usage: <yes/no/y/n>\n");
		}

	}
	while (talk_mode && (SW_OK != rv));

	do
	{
		cmd = get_sub_cmd("vsitranslation", "0");
		SW_RTN_ON_NULL_PARAM(cmd);

		if (!strncasecmp(cmd, "quit", 4))
		{
			return SW_BAD_VALUE;
		}
		else if (!strncasecmp(cmd, "help", 4))
		{
			dprintf("usage: the range is 0 -- 32\n");
			rv = SW_BAD_VALUE;
		}
		else
		{
			rv = cmd_data_check_uint8(cmd, &tmp, sizeof (a_uint32_t));
			if (SW_OK != rv)
			{
				dprintf("usage: the range is 0 -- 32\n");
			}
			else
			{
				entry.vsi_xlt = tmp;
			}
		}
	}
	while (talk_mode && (SW_OK != rv));

	*val = entry;
	return SW_OK;
}

void
cmd_data_print_port_vlan_translation_adv_action(a_uint8_t * param_name, a_uint32_t * buf, a_uint32_t size)
{
	fal_vlan_trans_adv_action_t *entry;

	entry = (fal_vlan_trans_adv_action_t *) buf;

	dprintf("\n\n action field: ");
	dprintf("\n[swap_svid_cvid]:%s  ",
			entry->swap_svid_cvid?"ENABLE":"DISABLE");
	dprintf("\n[svid_translation_cmd]:%d  [svidtranslation]:%d  [cvid_translation_cmd]:%d  [cvidtranslation]:%d  ",
			entry->svid_xlt_cmd,
			entry->svid_xlt,
			entry->cvid_xlt_cmd,
			entry->cvid_xlt);
	dprintf("\n[swap_spcp_cpcp]:%s  ",
			entry->swap_spcp_cpcp?"ENABLE":"DISABLE");
	dprintf("\n[spcp_translation_en]:%s  [spcptranslation]:%d  [cpcp_translation_en]:%s  [cpcptranslation]:%d  ",
			entry->spcp_xlt_enable?"ENABLE":"DISABLE",
			entry->spcp_xlt,
			entry->cpcp_xlt_enable?"ENABLE":"DISABLE",
			entry->cpcp_xlt);
	dprintf("\n[swap_sdei_cdei]:%s",
			entry->swap_sdei_cdei?"ENABLE":"DISABLE");
	dprintf("\n[sdei_translation_en]:%s  [sdeitranslation]:%d  [cdei_translation_en]:%s  [cdeitranslation]:%d  ",
			entry->sdei_xlt_enable?"ENABLE":"DISABLE",
			entry->sdei_xlt,
			entry->cdei_xlt_enable?"ENABLE":"DISABLE",
			entry->cdei_xlt);
	dprintf("\n[counter_en]:%s  [counter_id]:%d",
			entry->counter_enable?"ENABLE":"DISABLE",
			entry->counter_id);
	dprintf("\n[vsi_translation_en]:%s  [vsitranslation]:%d \n\n",
			entry->vsi_xlt_enable?"ENABLE":"DISABLE",
			entry->vsi_xlt);
}

sw_error_t
cmd_data_check_debug_port_counter_status(char *info, fal_counter_en_t *val, a_uint32_t size)
{
	char *cmd = NULL;
	sw_error_t rv;
	fal_counter_en_t entry;

	memset(&entry, 0, sizeof (fal_counter_en_t));

	do
	{
		cmd = get_sub_cmd("rx_counter_en", "yes");
		SW_RTN_ON_NULL_PARAM(cmd);

		if (!strncasecmp(cmd, "quit", 4))
		{
			return SW_BAD_VALUE;
		}
		else if (!strncasecmp(cmd, "help", 4))
		{
			dprintf("usage: <yes/no/y/n>\n");
			rv = SW_BAD_VALUE;
		}
		else
		{
			rv = cmd_data_check_confirm(cmd, A_TRUE, &entry.rx_counter_en,
					sizeof (a_bool_t));
			if (SW_OK != rv)
				dprintf("usage: <yes/no/y/n>\n");
		}

	}
	while (talk_mode && (SW_OK != rv));

	do
	{
		cmd = get_sub_cmd("vp_uni_tx_counter_en", "yes");
		SW_RTN_ON_NULL_PARAM(cmd);

		if (!strncasecmp(cmd, "quit", 4))
		{
			return SW_BAD_VALUE;
		}
		else if (!strncasecmp(cmd, "help", 4))
		{
			dprintf("usage: <yes/no/y/n>\n");
			rv = SW_BAD_VALUE;
		}
		else
		{
			rv = cmd_data_check_confirm(cmd, A_TRUE, &entry.vp_uni_tx_counter_en,
					sizeof (a_bool_t));
			if (SW_OK != rv)
				dprintf("usage: <yes/no/y/n>\n");
		}

	}
	while (talk_mode && (SW_OK != rv));

	do
	{
		cmd = get_sub_cmd("port_mc_tx_counter_en", "yes");
		SW_RTN_ON_NULL_PARAM(cmd);

		if (!strncasecmp(cmd, "quit", 4))
		{
			return SW_BAD_VALUE;
		}
		else if (!strncasecmp(cmd, "help", 4))
		{
			dprintf("usage: <yes/no/y/n>\n");
			rv = SW_BAD_VALUE;
		}
		else
		{
			rv = cmd_data_check_confirm(cmd, A_TRUE, &entry.port_mc_tx_counter_en,
					sizeof (a_bool_t));
			if (SW_OK != rv)
				dprintf("usage: <yes/no/y/n>\n");
		}

	}
	while (talk_mode && (SW_OK != rv));

	do
	{
		cmd = get_sub_cmd("port_tx_counter_en", "yes");
		SW_RTN_ON_NULL_PARAM(cmd);

		if (!strncasecmp(cmd, "quit", 4))
		{
			return SW_BAD_VALUE;
		}
		else if (!strncasecmp(cmd, "help", 4))
		{
			dprintf("usage: <yes/no/y/n>\n");
			rv = SW_BAD_VALUE;
		}
		else
		{
			rv = cmd_data_check_confirm(cmd, A_TRUE, &entry.port_tx_counter_en,
					sizeof (a_bool_t));
			if (SW_OK != rv)
				dprintf("usage: <yes/no/y/n>\n");
		}

	}
	while (talk_mode && (SW_OK != rv));

	*val = entry;
	return SW_OK;
}

void
cmd_data_print_debug_port_counter_status(a_uint8_t * param_name, a_uint32_t * buf, a_uint32_t size)
{
	fal_counter_en_t *entry;

	entry = (fal_counter_en_t*) buf;

	dprintf("rx_counter_en:%s\n", entry->rx_counter_en?"ENABLE":"DISABLE");
	dprintf("vp_uni_tx_counter_en:%s\n", entry->vp_uni_tx_counter_en?"ENABLE":"DISABLE");
	dprintf("port_mc_tx_counter_en:%s\n", entry->port_mc_tx_counter_en?"ENABLE":"DISABLE");
	dprintf("port_tx_counter_en:%s\n", entry->port_tx_counter_en?"ENABLE":"DISABLE");
}

void
cmd_data_print_port_vlan_counter(a_uint8_t * param_name, a_uint32_t * buf, a_uint32_t size)
{
	fal_port_vlan_counter_t *entry;

	entry = (fal_port_vlan_counter_t *) buf;

	dprintf("rx_packet_counter:0x%x, rx_byte_counter:0x%llx\n", entry->rx_packet_counter, entry->rx_byte_counter);
	dprintf("tx_packet_counter:0x%x, tx_byte_counter:0x%llx\n", entry->tx_packet_counter, entry->tx_byte_counter);
}

sw_error_t
cmd_data_check_tag_propagation(char *info, void *val, a_uint32_t size)
{
    char *cmd;
    sw_error_t rv;
    fal_vlantag_propagation_t *pEntry = (fal_vlantag_propagation_t *)val;

    memset(pEntry, 0, sizeof(fal_vlantag_propagation_t));

    /* get mask */
    do
    {
        cmd = get_sub_cmd("mask", "0x0");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: <bit 0 for vid and bit 1 for pri; bit 2 for dei>\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_uint32(cmd, &(pEntry->mask), sizeof(a_uint32_t));
            if (SW_OK != rv)
                dprintf("usage: <bit 0 for vid and bit 1 for pri; bit 2 for dei>\n");
        }
    }while (talk_mode && (SW_OK != rv));

    /* get vid propagation */
    do
    {
        cmd = get_sub_cmd("vid_propagation_en", "disable");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: <disable/clone/replace>\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_vlan_propagation(cmd, (a_uint32_t *) & (pEntry->vid_propagation), sizeof(a_uint32_t));
            if (SW_OK != rv)
                dprintf("usage: <disable/clone/replace>\n");
        }
    }while (talk_mode && (SW_OK != rv));

    /* get pri propagation */
    do
    {
        cmd = get_sub_cmd("pri_propagation_en", "disable");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: <disable/clone/replace>\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_vlan_propagation(cmd, (a_uint32_t *) & (pEntry->pri_propagation), sizeof(a_uint32_t));
            if (SW_OK != rv)
                dprintf("usage: <disable/clone/replace>\n");
        }
    }while (talk_mode && (SW_OK != rv));

    /* get dei propagation */
    do
    {
        cmd = get_sub_cmd("dei_propagation_en", "disable");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: <disable/clone/replace>\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_vlan_propagation(cmd, (a_uint32_t *) & (pEntry->dei_propagation), sizeof(a_uint32_t));
            if (SW_OK != rv)
                dprintf("usage: <disable/clone/replace>\n");
        }
    }while (talk_mode && (SW_OK != rv));

    return SW_OK;
}

void
cmd_data_print_tag_propagation(a_uint8_t * param_name, a_uint32_t * buf, a_uint32_t size)
{
    fal_vlantag_propagation_t *entry;

    entry = (fal_vlantag_propagation_t *) buf;
    dprintf("\n");

    cmd_data_print_uint32("mask", (a_uint32_t *) & (entry->mask), 4);
    dprintf("\n");
    cmd_data_print_vlan_propagation("vid_propagation_en", (a_uint32_t *) &
		(entry->vid_propagation), 4);
    dprintf("\n");
    cmd_data_print_vlan_propagation("pri_propagation_en", (a_uint32_t *) &
		(entry->pri_propagation), 4);
    dprintf("\n");
    cmd_data_print_vlan_propagation("dei_propagation_en", (a_uint32_t *) &
		(entry->dei_propagation), 4);
    dprintf("\n");

}

sw_error_t
cmd_data_check_egress_mode(char *info, void *val, a_uint32_t size)
{
    char *cmd;
    sw_error_t rv;
    fal_vlantag_egress_mode_t *pEntry = (fal_vlantag_egress_mode_t *)val;

    memset(pEntry, 0, sizeof(fal_vlantag_egress_mode_t));

    /* get mask */
    do
    {
        cmd = get_sub_cmd("mask", "0x0");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: <bit 0 for ctpidag and bit 1 for stag>\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_uint32(cmd, &(pEntry->mask), sizeof(a_uint32_t));
            if (SW_OK != rv)
                dprintf("usage: <bit 0 for ctpidag and bit 1 for stag>\n");
        }
    }while (talk_mode && (SW_OK != rv));

    /* get ctag mode */
    do
    {
        cmd = get_sub_cmd("ctag_egress_vlan_mode", "unmodified");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: <tagged/untagged/unmodified/untouched>\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_egmode(cmd, (a_uint32_t *) & (pEntry->ctag_mode), sizeof(a_uint32_t));
            if (SW_OK != rv)
                dprintf("usage: <tagged/untagged/unmodified/untouched>\n");
        }
    }while (talk_mode && (SW_OK != rv));

    /* get stag mode */
    do
    {
        cmd = get_sub_cmd("stag_egress_vlan_mode", "unmodified");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: <tagged/untagged/unmodified/untouched>\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_egmode(cmd, (a_uint32_t *) & (pEntry->stag_mode), sizeof(a_uint32_t));
            if (SW_OK != rv)
                dprintf("usage: <tagged/untagged/unmodified/untouched>\n");
        }
    }while (talk_mode && (SW_OK != rv));

    return SW_OK;
}

void
cmd_data_print_egress_mode(a_uint8_t * param_name, a_uint32_t * buf, a_uint32_t size)
{
    fal_vlantag_egress_mode_t *entry;

    entry = (fal_vlantag_egress_mode_t *) buf;
    dprintf("\n");

    cmd_data_print_uint32("mask", (a_uint32_t *) & (entry->mask), 4);
    dprintf("\n");
    cmd_data_print_egmode("ctag_egress_vlan_mode", (a_uint32_t *) & (entry->ctag_mode), 4);
    dprintf("\n");
    cmd_data_print_egmode("stag_egress_vlan_mode", (a_uint32_t *) & (entry->stag_mode), 4);
    dprintf("\n");

}

sw_error_t
cmd_data_check_ctrlpkt_profile(char *info, void *val, a_uint32_t size)
{
    char *cmd;
    sw_error_t rv;
    fal_ctrlpkt_profile_t *pEntry = (fal_ctrlpkt_profile_t *)val;
    a_uint32_t tmp = 0;

    memset(pEntry, 0, sizeof(fal_ctrlpkt_profile_t));

    /* get port bitmap */
    do
    {
        cmd = get_sub_cmd("port_bitmap", "0");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: toal 8 bits for 8 ports\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_uint32(cmd, &tmp, sizeof(a_uint32_t));
            if (SW_OK != rv)
                dprintf("usage: toal 8 bits for 8 ports\n");

	     pEntry->port_map = tmp;
        }
    }while (talk_mode && (SW_OK != rv));

    /* get ethernet type profile */
    do
    {
        cmd = get_sub_cmd("ethtype_profile_bitmap", "0");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: toal 4 bits for 4 ethernet types\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_uint32(cmd, &tmp, sizeof(a_uint32_t));
            if (SW_OK != rv)
                dprintf("usage: toal 4 bits for 4 ethernet types\n");

	     pEntry->ethtype_profile_bitmap = tmp;
        }
    }while (talk_mode && (SW_OK != rv));

    /* get rfdb profile */
    do
    {
        cmd = get_sub_cmd("rfdb_profile_bitmap", "0");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: toal 32 bits for 32 rfdbs\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_uint32(cmd, &tmp, sizeof(a_uint32_t));
            if (SW_OK != rv)
                dprintf("usage: toal 4 bits for 4 ethernet type\n");

	     pEntry->rfdb_profile_bitmap= tmp;
        }
    }while (talk_mode && (SW_OK != rv));

    /* get mgt_eapol */
    do
    {
        cmd = get_sub_cmd("eapol_en", "no");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: <yes/no/y/n>\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_confirm(cmd, A_FALSE, &(pEntry->protocol_types.mgt_eapol),
                                        sizeof (a_bool_t));
            if (SW_OK != rv)
                dprintf("usage: <yes/no/y/n>\n");
        }
    }
    while (talk_mode && (SW_OK != rv));

    /* get mgt_eapol */
    do
    {
        cmd = get_sub_cmd("pppoe_en", "no");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: <yes/no/y/n>\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_confirm(cmd, A_FALSE, &(pEntry->protocol_types.mgt_pppoe),
                                        sizeof (a_bool_t));
            if (SW_OK != rv)
                dprintf("usage: <yes/no/y/n>\n");
        }
    }
    while (talk_mode && (SW_OK != rv));

    /* get mgt_igmp */
    do
    {
        cmd = get_sub_cmd("igmp_en", "no");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: <yes/no/y/n>\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_confirm(cmd, A_FALSE, &(pEntry->protocol_types.mgt_igmp),
                                        sizeof (a_bool_t));
            if (SW_OK != rv)
                dprintf("usage: <yes/no/y/n>\n");
        }
    }
    while (talk_mode && (SW_OK != rv));

    /* get mgt_arp_req */
    do
    {
        cmd = get_sub_cmd("arp_request_en", "no");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: <yes/no/y/n>\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_confirm(cmd, A_FALSE, &(pEntry->protocol_types.mgt_arp_req),
                                        sizeof (a_bool_t));
            if (SW_OK != rv)
                dprintf("usage: <yes/no/y/n>\n");
        }
    }
    while (talk_mode && (SW_OK != rv));

    /* get mgt_arp_rep */
    do
    {
        cmd = get_sub_cmd("arp_response_en", "no");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: <yes/no/y/n>\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_confirm(cmd, A_FALSE, &(pEntry->protocol_types.mgt_arp_rep),
                                        sizeof (a_bool_t));
            if (SW_OK != rv)
                dprintf("usage: <yes/no/y/n>\n");
        }
    }
    while (talk_mode && (SW_OK != rv));

    /* get mgt_dhcp4 */
    do
    {
        cmd = get_sub_cmd("dhcp4_en", "no");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: <yes/no/y/n>\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_confirm(cmd, A_FALSE, &(pEntry->protocol_types.mgt_dhcp4),
                                        sizeof (a_bool_t));
            if (SW_OK != rv)
                dprintf("usage: <yes/no/y/n>\n");
        }
    }
    while (talk_mode && (SW_OK != rv));

    /* get mgt_dhcp6 */
    do
    {
        cmd = get_sub_cmd("dhcp6_en", "no");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: <yes/no/y/n>\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_confirm(cmd, A_FALSE, &(pEntry->protocol_types.mgt_dhcp6),
                                        sizeof (a_bool_t));
            if (SW_OK != rv)
                dprintf("usage: <yes/no/y/n>\n");
        }
    }
    while (talk_mode && (SW_OK != rv));

    /* get mgt_mld */
    do
    {
        cmd = get_sub_cmd("mld_en", "no");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: <yes/no/y/n>\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_confirm(cmd, A_FALSE, &(pEntry->protocol_types.mgt_mld),
                                        sizeof (a_bool_t));
            if (SW_OK != rv)
                dprintf("usage: <yes/no/y/n>\n");
        }
    }
    while (talk_mode && (SW_OK != rv));


    /* get mgt_ns */
    do
    {
        cmd = get_sub_cmd("ip6ns_en", "no");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: <yes/no/y/n>\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_confirm(cmd, A_FALSE, &(pEntry->protocol_types.mgt_ns),
                                        sizeof (a_bool_t));
            if (SW_OK != rv)
                dprintf("usage: <yes/no/y/n>\n");
        }
    }
    while (talk_mode && (SW_OK != rv));

    /* get mgt_na */
    do
    {
        cmd = get_sub_cmd("ip6na_en", "no");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: <yes/no/y/n>\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_confirm(cmd, A_FALSE, &(pEntry->protocol_types.mgt_na),
                                        sizeof (a_bool_t));
            if (SW_OK != rv)
                dprintf("usage: <yes/no/y/n>\n");
        }
    }
    while (talk_mode && (SW_OK != rv));

    /* get forward command */
    do
    {
        cmd = get_sub_cmd("ctrlpkt_profile_action", "forward");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: forward/drop/rdtcpu/cpycpu\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_maccmd(cmd, &(pEntry->action.action), sizeof(a_uint32_t));
            if (SW_OK != rv)
                dprintf("usage: forward/drop/rdtcpu/cpycpu\n");
        }
    }while (talk_mode && (SW_OK != rv));

    /* get sg_byp */
    do
    {
        cmd = get_sub_cmd("sourceguard_bypass", "no");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: <yes/no/y/n>\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_confirm(cmd, A_FALSE, &(pEntry->action.sg_bypass),
                                        sizeof (a_bool_t));
            if (SW_OK != rv)
                dprintf("usage: <yes/no/y/n>\n");
        }
    }
    while (talk_mode && (SW_OK != rv));

    /* get l2_filter_byp */
    do
    {
        cmd = get_sub_cmd("l2filter_bypass", "no");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: <yes/no/y/n>\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_confirm(cmd, A_FALSE, &(pEntry->action.l2_filter_bypass),
                                        sizeof (a_bool_t));
            if (SW_OK != rv)
                dprintf("usage: <yes/no/y/n>\n");
        }
    }
    while (talk_mode && (SW_OK != rv));

    /* get in_stp_byp */
    do
    {
        cmd = get_sub_cmd("ingress_stp_bypass", "no");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: <yes/no/y/n>\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_confirm(cmd, A_FALSE, &(pEntry->action.in_stp_bypass),
                                        sizeof (a_bool_t));
            if (SW_OK != rv)
                dprintf("usage: <yes/no/y/n>\n");
        }
    }
    while (talk_mode && (SW_OK != rv));

    /* get in_vlan_fltr_byp */
    do
    {
        cmd = get_sub_cmd("ingress_vlan_filter_bypass", "no");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: <yes/no/y/n>\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_confirm(cmd, A_FALSE, &(pEntry->action.in_vlan_fltr_bypass),
                                        sizeof (a_bool_t));
            if (SW_OK != rv)
                dprintf("usage: <yes/no/y/n>\n");
        }
    }
    while (talk_mode && (SW_OK != rv));

    return SW_OK;
}

void
cmd_data_print_ctrlpkt_profile(a_uint8_t * param_name, a_uint32_t * buf, a_uint32_t size)
{
    fal_ctrlpkt_profile_t *entry;

    entry = (fal_ctrlpkt_profile_t *) buf;
    dprintf("\n");

    cmd_data_print_uint32("port_bitmap", (a_uint32_t *) & (entry->port_map), 4);
    dprintf(" ");
    cmd_data_print_uint32("ethtype_profile_bitmap", (a_uint32_t *) &
		(entry->ethtype_profile_bitmap), 4);
    dprintf(" ");
    cmd_data_print_uint32("rfdb_profile_bitmap", (a_uint32_t *) & (entry->rfdb_profile_bitmap),
		4);
    dprintf("\n");
    cmd_data_print_enable("eapol_en", (a_uint32_t *) & (entry->protocol_types.mgt_eapol), 4);
    dprintf(" ");
    cmd_data_print_enable("pppoe_en", (a_uint32_t *) & (entry->protocol_types.mgt_pppoe), 4);
    dprintf(" ");
    cmd_data_print_enable("igmp_en", (a_uint32_t *) & (entry->protocol_types.mgt_igmp), 4);
    dprintf(" ");
    cmd_data_print_enable("arp_request_en", (a_uint32_t *) & (entry->protocol_types.mgt_arp_req),
		4);
    dprintf(" ");
    cmd_data_print_enable("arp_response_en", (a_uint32_t *) & (entry->protocol_types.mgt_arp_rep),
		4);
    dprintf("\n");
    cmd_data_print_enable("dhcp4_en", (a_uint32_t *) & (entry->protocol_types.mgt_dhcp4), 4);
    dprintf(" ");
    cmd_data_print_enable("dhcp6_en", (a_uint32_t *) & (entry->protocol_types.mgt_dhcp6), 4);
    dprintf(" ");
    cmd_data_print_enable("mld_en", (a_uint32_t *) & (entry->protocol_types.mgt_mld), 4);
    dprintf(" ");
    cmd_data_print_enable("ip6ns_en", (a_uint32_t *) & (entry->protocol_types.mgt_ns), 4);
    dprintf(" ");
    cmd_data_print_enable("ip6na_en", (a_uint32_t *) & (entry->protocol_types.mgt_na), 4);
    dprintf("\n");
    cmd_data_print_maccmd("ctrlpkt_profile_action", (a_uint32_t *) & (entry->action.action), 4);
    dprintf(" ");
    cmd_data_print_enable("sourceguard_bypass", (a_uint32_t *) & (entry->action.sg_bypass), 4);
    dprintf("\n");
    cmd_data_print_enable("l2filter_bypass", (a_uint32_t *) & (entry->action.l2_filter_bypass),
		4);
    dprintf(" ");
    cmd_data_print_enable("ingress_stp_bypass", (a_uint32_t *) & (entry->action.in_stp_bypass),
		4);
    dprintf(" ");
    cmd_data_print_enable("ingress_vlan_filter_bypass", (a_uint32_t *) &
		(entry->action.in_vlan_fltr_bypass), 4);
    dprintf("\n");

}

sw_error_t
cmd_data_check_servcode_config(char *info, fal_servcode_config_t *val, a_uint32_t size)
{
	char *cmd = NULL;
	sw_error_t rv;
	fal_servcode_config_t entry;

	memset(&entry, 0, sizeof (fal_servcode_config_t));

	do
	{
		cmd = get_sub_cmd("destport_en", "yes");
		SW_RTN_ON_NULL_PARAM(cmd);

		if (!strncasecmp(cmd, "quit", 4))
		{
			return SW_BAD_VALUE;
		}
		else if (!strncasecmp(cmd, "help", 4))
		{
			dprintf("usage: <yes/no/y/n>\n");
			rv = SW_BAD_VALUE;
		}
		else
		{
			rv = cmd_data_check_confirm(cmd, A_TRUE, &entry.dest_port_valid,
					sizeof (a_bool_t));
			if (SW_OK != rv)
				dprintf("usage: <yes/no/y/n>\n");
		}
	}
	while (talk_mode && (SW_OK != rv));

	do
	{
		cmd = get_sub_cmd("destport_id", "0");
		SW_RTN_ON_NULL_PARAM(cmd);

		if (!strncasecmp(cmd, "quit", 4))
		{
			return SW_BAD_VALUE;
		}
		else if (!strncasecmp(cmd, "help", 4))
		{
			dprintf("physical port id: 0 - 7\n");
			rv = SW_BAD_VALUE;
		}
		else
		{
			rv = cmd_data_check_uint32(cmd, &entry.dest_port_id, sizeof (a_uint32_t));
			if (SW_OK != rv)
				dprintf("physical port id: 0 - 7\n");
		}
	}
	while (talk_mode && (SW_OK != rv));

	do
	{
		cmd = get_sub_cmd("bypass_bitmap_0", "0");
		SW_RTN_ON_NULL_PARAM(cmd);

		if (!strncasecmp(cmd, "quit", 4))
		{
			return SW_BAD_VALUE;
		}
		else if (!strncasecmp(cmd, "help", 4))
		{
			dprintf("usage: refer to service spec\n");
			rv = SW_BAD_VALUE;
		}
		else
		{
			rv = cmd_data_check_uint32(cmd, &entry.bypass_bitmap[0],
				sizeof (a_uint32_t));
			if (SW_OK != rv)
				dprintf("usage: refer to service spec\n");
		}
	}
	while (talk_mode && (SW_OK != rv));

	do
	{
		cmd = get_sub_cmd("bypass_bitmap_1", "0");
		SW_RTN_ON_NULL_PARAM(cmd);

		if (!strncasecmp(cmd, "quit", 4))
		{
			return SW_BAD_VALUE;
		}
		else if (!strncasecmp(cmd, "help", 4))
		{
			dprintf("usage: refer to service spec\n");
			rv = SW_BAD_VALUE;
		}
		else
		{
			rv = cmd_data_check_uint32(cmd, &entry.bypass_bitmap[1],
				sizeof (a_uint32_t));
			if (SW_OK != rv)
				dprintf("usage: refer to service spec\n");
		}
	}
	while (talk_mode && (SW_OK != rv));

	do
	{
		cmd = get_sub_cmd("bypass_bitmap_2", "0");
		SW_RTN_ON_NULL_PARAM(cmd);

		if (!strncasecmp(cmd, "quit", 4))
		{
			return SW_BAD_VALUE;
		}
		else if (!strncasecmp(cmd, "help", 4))
		{
			dprintf("usage: refer to service spec\n");
			rv = SW_BAD_VALUE;
		}
		else
		{
			rv = cmd_data_check_uint32(cmd, &entry.bypass_bitmap[2],
				sizeof (a_uint32_t));
			if (SW_OK != rv)
				dprintf("usage: refer to service spec\n");
		}
	}
	while (talk_mode && (SW_OK != rv));

	do
	{
		cmd = get_sub_cmd("direction", "0");
		SW_RTN_ON_NULL_PARAM(cmd);

		if (!strncasecmp(cmd, "quit", 4))
		{
			return SW_BAD_VALUE;
		}
		else if (!strncasecmp(cmd, "help", 4))
		{
			dprintf("usage: 0:dest, 1:src \n");
			rv = SW_BAD_VALUE;
		}
		else
		{
			rv = cmd_data_check_uint32(cmd, &entry.direction,
				sizeof (a_uint32_t));
			if (SW_OK != rv)
				dprintf("usage: 0:dest, 1:src \n");
		}
	}
	while (talk_mode && (SW_OK != rv));

	do
	{
		cmd = get_sub_cmd("field_update_bitmap", "0");
		SW_RTN_ON_NULL_PARAM(cmd);

		if (!strncasecmp(cmd, "quit", 4))
		{
			return SW_BAD_VALUE;
		}
		else if (!strncasecmp(cmd, "help", 4))
		{
			dprintf("usage: refer to service spec\n");
			rv = SW_BAD_VALUE;
		}
		else
		{
			rv = cmd_data_check_uint32(cmd, &entry.field_update_bitmap,
				sizeof (a_uint32_t));
			if (SW_OK != rv)
				dprintf("usage: refer to service spec\n");
		}
	}
	while (talk_mode && (SW_OK != rv));

	do
	{
		cmd = get_sub_cmd("next_servicecode", "0");
		SW_RTN_ON_NULL_PARAM(cmd);

		if (!strncasecmp(cmd, "quit", 4))
		{
			return SW_BAD_VALUE;
		}
		else if (!strncasecmp(cmd, "help", 4))
		{
			dprintf("usage: refer to service spec\n");
			rv = SW_BAD_VALUE;
		}
		else
		{
			rv = cmd_data_check_uint32(cmd, &entry.next_service_code,
				sizeof (a_uint32_t));
			if (SW_OK != rv)
				dprintf("usage: refer to service spec\n");
		}
	}
	while (talk_mode && (SW_OK != rv));

	do
	{
		cmd = get_sub_cmd("hardwareservices", "0");
		SW_RTN_ON_NULL_PARAM(cmd);

		if (!strncasecmp(cmd, "quit", 4))
		{
			return SW_BAD_VALUE;
		}
		else if (!strncasecmp(cmd, "help", 4))
		{
			dprintf("usage: refer to service spec\n");
			rv = SW_BAD_VALUE;
		}
		else
		{
			rv = cmd_data_check_uint32(cmd, &entry.hw_services,
				sizeof (a_uint32_t));
			if (SW_OK != rv)
				dprintf("usage: refer to service spec\n");
		}
	}
	while (talk_mode && (SW_OK != rv));

	do
	{
		cmd = get_sub_cmd("offsetselection", "0");
		SW_RTN_ON_NULL_PARAM(cmd);

		if (!strncasecmp(cmd, "quit", 4))
		{
			return SW_BAD_VALUE;
		}
		else if (!strncasecmp(cmd, "help", 4))
		{
			dprintf("usage: refer to service spec\n");
			rv = SW_BAD_VALUE;
		}
		else
		{
			rv = cmd_data_check_uint32(cmd, &entry.offset_sel,
				sizeof (a_uint32_t));
			if (SW_OK != rv)
				dprintf("usage: refer to service spec\n");
		}
	}
	while (talk_mode && (SW_OK != rv));

	*val = entry;
	return SW_OK;
}

void
cmd_data_print_servcode_config(a_uint8_t * param_name, a_uint32_t * buf, a_uint32_t size)
{
	fal_servcode_config_t *entry;

	entry = (fal_servcode_config_t *) buf;

	dprintf("\ndestport_en:%s  destport_id:%d\n",
				entry->dest_port_valid? "ENABLE" : "DISABLE",
				entry->dest_port_id);
	dprintf("bypass_bitmap_0:0x%x  bypass_bitmap_1:0x%x  bypass_bitmap_2:0x%x\n",
				entry->bypass_bitmap[0], entry->bypass_bitmap[1], entry->bypass_bitmap[2]);
	dprintf("direction:%d\n", entry->direction);
	dprintf("field_update_bitmap:0x%x  next_servicecode:%d\n",
				entry->field_update_bitmap, entry->next_service_code);
	dprintf("hardwareservices:%d  offsetselection:%d\n",
				entry->hw_services, entry->offset_sel);
}

sw_error_t
cmd_data_check_rss_hash_mode(char *cmd_str, a_uint32_t * arg_val, a_uint32_t size)
{
    if (cmd_str == NULL)
        return SW_BAD_PARAM;

    if (!strcasecmp(cmd_str, "ipv4v6"))
    {
        *arg_val = FAL_RSS_HASH_IPV4V6;
    }
    else if (!strcasecmp(cmd_str, "ipv4"))
    {
        *arg_val = FAL_RSS_HASH_IPV4ONLY;
    }
    else if (!strcasecmp(cmd_str, "ipv6"))
    {
        *arg_val = FAL_RSS_HASH_IPV6ONLY;
    }
    else
    {
        //dprintf("input error \n");
        return SW_BAD_VALUE;
    }

    return SW_OK;
}

sw_error_t
cmd_data_check_rss_hash_config(char *info, fal_rss_hash_config_t *val, a_uint32_t size)
{
	char *cmd = NULL;
	a_uint32_t tmp;
	sw_error_t rv;
	fal_rss_hash_config_t entry;

	memset(&entry, 0, sizeof (fal_rss_hash_config_t));

	do
	{
		cmd = get_sub_cmd("hash_mask", "0");
		SW_RTN_ON_NULL_PARAM(cmd);

		if (!strncasecmp(cmd, "quit", 4))
		{
			return SW_BAD_VALUE;
		}
		else if (!strncasecmp(cmd, "help", 4))
		{
			dprintf("hash_mask: 0 - 0x1fffff\n");
			rv = SW_BAD_VALUE;
		}
		else
		{
			rv = cmd_data_check_uint32(cmd, &entry.hash_mask, sizeof (a_uint32_t));
			if (SW_OK != rv)
				dprintf("hash_mask: 0 - 0x1fffff\n");
		}
	}
	while (talk_mode && (SW_OK != rv));

	do
	{
		cmd = get_sub_cmd("hash_fragment_mode", "no");
		SW_RTN_ON_NULL_PARAM(cmd);

		if (!strncasecmp(cmd, "quit", 4))
		{
			return SW_BAD_VALUE;
		}
		else if (!strncasecmp(cmd, "help", 4))
		{
			dprintf("usage: <yes/no/y/n>\n");
			rv = SW_BAD_VALUE;
		}
		else
		{
			rv = cmd_data_check_confirm(cmd, A_FALSE, &entry.hash_fragment_mode,
					sizeof (a_bool_t));
			if (SW_OK != rv)
				dprintf("usage: <yes/no/y/n>\n");
		}
	}
	while (talk_mode && (SW_OK != rv));

	do
	{
		cmd = get_sub_cmd("hash_seed", "0");
		SW_RTN_ON_NULL_PARAM(cmd);

		if (!strncasecmp(cmd, "quit", 4))
		{
			return SW_BAD_VALUE;
		}
		else if (!strncasecmp(cmd, "help", 4))
		{
			dprintf("hash_mask: 0 - 0xffffffff\n");
			rv = SW_BAD_VALUE;
		}
		else
		{
			rv = cmd_data_check_uint32(cmd, &entry.hash_seed, sizeof (a_uint32_t));
			if (SW_OK != rv)
				dprintf("hash_mask: 0 - ffffffff\n");
		}
	}
	while (talk_mode && (SW_OK != rv));

	do
	{
		cmd = get_sub_cmd("hash_sip_mix", "0");
		SW_RTN_ON_NULL_PARAM(cmd);

		if (!strncasecmp(cmd, "quit", 4))
		{
			return SW_BAD_VALUE;
		}
		else if (!strncasecmp(cmd, "help", 4))
		{
			dprintf("hash_mask: 0 - 0xfffff\n");
			rv = SW_BAD_VALUE;
		}
		else
		{
			rv = cmd_data_check_uint32(cmd, &entry.hash_sip_mix, sizeof (a_uint32_t));
			if (SW_OK != rv)
				dprintf("hash_mask: 0 - 0xfffff\n");
		}
	}
	while (talk_mode && (SW_OK != rv));

	do
	{
		cmd = get_sub_cmd("hash_dip_mix", "0");
		SW_RTN_ON_NULL_PARAM(cmd);

		if (!strncasecmp(cmd, "quit", 4))
		{
			return SW_BAD_VALUE;
		}
		else if (!strncasecmp(cmd, "help", 4))
		{
			dprintf("hash_mask: 0 - 0xfffff\n");
			rv = SW_BAD_VALUE;
		}
		else
		{
			rv = cmd_data_check_uint32(cmd, &entry.hash_dip_mix, sizeof (a_uint32_t));
			if (SW_OK != rv)
				dprintf("hash_mask: 0 - 0xfffff\n");
		}
	}
	while (talk_mode && (SW_OK != rv));

	do
	{
		cmd = get_sub_cmd("hash_protocol_mix", "0");
		SW_RTN_ON_NULL_PARAM(cmd);

		if (!strncasecmp(cmd, "quit", 4))
		{
			return SW_BAD_VALUE;
		}
		else if (!strncasecmp(cmd, "help", 4))
		{
			dprintf("hash_mask: 0 - 0x1f\n");
			rv = SW_BAD_VALUE;
		}
		else
		{
			rv = cmd_data_check_uint8(cmd, &tmp, sizeof (a_uint32_t));
			if (SW_OK != rv)
			{
				dprintf("hash_mask: 0 - 0x1f\n");
			}
			else
			{
				entry.hash_protocol_mix = tmp;
			}
		}
	}
	while (talk_mode && (SW_OK != rv));

	do
	{
		cmd = get_sub_cmd("hash_sport_mix", "0");
		SW_RTN_ON_NULL_PARAM(cmd);

		if (!strncasecmp(cmd, "quit", 4))
		{
			return SW_BAD_VALUE;
		}
		else if (!strncasecmp(cmd, "help", 4))
		{
			dprintf("hash_mask: 0 - 0x1f\n");
			rv = SW_BAD_VALUE;
		}
		else
		{
			rv = cmd_data_check_uint8(cmd, &tmp, sizeof (a_uint32_t));
			if (SW_OK != rv)
			{
				dprintf("hash_mask: 0 - 0x1f\n");
			}
			else
			{
				entry.hash_sport_mix = tmp;
			}
		}
	}
	while (talk_mode && (SW_OK != rv));

	do
	{
		cmd = get_sub_cmd("hash_dport_mix", "0");
		SW_RTN_ON_NULL_PARAM(cmd);

		if (!strncasecmp(cmd, "quit", 4))
		{
			return SW_BAD_VALUE;
		}
		else if (!strncasecmp(cmd, "help", 4))
		{
			dprintf("hash_mask: 0 - 0x1f\n");
			rv = SW_BAD_VALUE;
		}
		else
		{
			rv = cmd_data_check_uint8(cmd, &tmp, sizeof (a_uint32_t));
			if (SW_OK != rv)
			{
				dprintf("hash_mask: 0 - 0x1f\n");
			}
			else
			{
				entry.hash_dport_mix = tmp;
			}
		}
	}
	while (talk_mode && (SW_OK != rv));

	do
	{
		cmd = get_sub_cmd("hash_fin_inner", "0");
		SW_RTN_ON_NULL_PARAM(cmd);

		if (!strncasecmp(cmd, "quit", 4))
		{
			return SW_BAD_VALUE;
		}
		else if (!strncasecmp(cmd, "help", 4))
		{
			dprintf("hash_mask: 0 - 0x1ffffff\n");
			rv = SW_BAD_VALUE;
		}
		else
		{
			rv = cmd_data_check_uint32(cmd, &entry.hash_fin_inner, sizeof (a_uint32_t));
			if (SW_OK != rv)
				dprintf("hash_mask: 0 - 0x1ffffff\n");
		}
	}
	while (talk_mode && (SW_OK != rv));

	do
	{
		cmd = get_sub_cmd("hash_fin_outer", "0");
		SW_RTN_ON_NULL_PARAM(cmd);

		if (!strncasecmp(cmd, "quit", 4))
		{
			return SW_BAD_VALUE;
		}
		else if (!strncasecmp(cmd, "help", 4))
		{
			dprintf("hash_mask: 0 - 0x1ffffff\n");
			rv = SW_BAD_VALUE;
		}
		else
		{
			rv = cmd_data_check_uint32(cmd, &entry.hash_fin_outer, sizeof (a_uint32_t));
			if (SW_OK != rv)
				dprintf("hash_mask: 0 - 0x1ffffff\n");
		}
	}
	while (talk_mode && (SW_OK != rv));

	*val = entry;
	return SW_OK;
}

void
cmd_data_print_rss_hash_config(a_uint8_t * param_name, a_uint32_t * buf, a_uint32_t size)
{
	fal_rss_hash_config_t *entry;

	entry = (fal_rss_hash_config_t *) buf;

	dprintf("\nhash_mask:0x%x  hash_fragment_mode:%s\n",
				entry->hash_mask,
				entry->hash_fragment_mode? "ENABLE" : "DISABLE");
	dprintf("hash_seed:0x%x  hash_sip_mix:0x%x\n",
				entry->hash_seed,
				entry->hash_sip_mix);
	dprintf("hash_dip_mix:0x%x  hash_protocol_mix:0x%x\n",
				entry->hash_dip_mix,
				entry->hash_protocol_mix);
	dprintf("hash_sport_mix:0x%x  hash_dport_mix:0x%x\n",
				entry->hash_sport_mix,
				entry->hash_dport_mix);
	dprintf("hash_fin_inner:0x%x  hash_fin_outer:0x%x\n",
				entry->hash_fin_inner,
				entry->hash_fin_outer);
}

sw_error_t
cmd_data_check_flow_host(char *cmd_str, void * val, a_uint32_t size)
{
        fal_flow_host_entry_t *flow_host = (fal_flow_host_entry_t *)val;
        fal_flow_entry_t *flow_entry = &(flow_host->flow_entry);
        fal_host_entry_t *host_entry = &(flow_host->host_entry);

        cmd_data_check_flow(cmd_str, flow_entry, size);
        cmd_data_check_host_entry(cmd_str, host_entry, size);

	return SW_OK;
}

void
cmd_data_print_flow_host(a_uint8_t * param_name, a_uint32_t * buf, a_uint32_t size)
{
    fal_flow_host_entry_t *flow_host = (fal_flow_host_entry_t *) buf;
        fal_flow_entry_t *flow_entry = &(flow_host->flow_entry);
        fal_host_entry_t *host_entry = &(flow_host->host_entry);

        cmd_data_print_flow(param_name, (a_uint32_t *)flow_entry, size);
        cmd_data_print_host_entry(param_name, (a_uint32_t *)host_entry, size);
}

sw_error_t
cmd_data_check_port_shaper_token_config(char *cmd_str, void * val, a_uint32_t size)
{
    char *cmd;
    sw_error_t rv;
    fal_shaper_token_number_t entry;

    aos_mem_zero(&entry, sizeof (fal_shaper_token_number_t));

    do
    {
        cmd = get_sub_cmd("ctoken_negative_enable", "no");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {

            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: <yes/no/y/n>\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_confirm(cmd, A_FALSE, &(entry.c_token_number_negative_en),
                                        sizeof (a_bool_t));
            if (SW_OK != rv)
                dprintf("usage: <yes/no/y/n>\n");
        }
    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("ctoken_number", "0-0x3FFFFFFF");
		SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: integer\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_uint32(cmd, &(entry.c_token_number), sizeof (a_uint32_t));
            if (SW_OK != rv)
                dprintf("usage: integer\n");
        }
    }
    while (talk_mode && (SW_OK != rv));

    *(fal_shaper_token_number_t *)val = entry;
    return SW_OK;
}


sw_error_t
cmd_data_check_shaper_token_config(char *cmd_str, void * val, a_uint32_t size)
{
    char *cmd;
    sw_error_t rv;
    fal_shaper_token_number_t entry;

    aos_mem_zero(&entry, sizeof (fal_shaper_token_number_t));

    do
    {
        cmd = get_sub_cmd("ctoken_negative_enable", "no");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {

            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: <yes/no/y/n>\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_confirm(cmd, A_FALSE, &(entry.c_token_number_negative_en),
                                        sizeof (a_bool_t));
            if (SW_OK != rv)
                dprintf("usage: <yes/no/y/n>\n");
        }
    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("ctoken_number", "0-0x3FFFFFFF");
		SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: integer\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_uint32(cmd, &(entry.c_token_number), sizeof (a_uint32_t));
            if (SW_OK != rv)
                dprintf("usage: integer\n");
        }
    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("etoken_negative_enable", "no");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {

            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: <yes/no/y/n>\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_confirm(cmd, A_FALSE, &(entry.e_token_number_negative_en),
                                        sizeof (a_bool_t));
            if (SW_OK != rv)
                dprintf("usage: <yes/no/y/n>\n");
        }
    }
    while (talk_mode && (SW_OK != rv));


    do
    {
        cmd = get_sub_cmd("etoken_number", "0-0x3FFFFFFF");
		SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: integer\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_uint32(cmd, &(entry.e_token_number), sizeof (a_uint32_t));
            if (SW_OK != rv)
                dprintf("usage: integer\n");
        }
    }
    while (talk_mode && (SW_OK != rv));

    *(fal_shaper_token_number_t *)val = entry;
    return SW_OK;
}


sw_error_t
cmd_data_check_port_shaper_config(char *cmd_str, void * val, a_uint32_t size)
{
    char *cmd;
    sw_error_t rv;
    fal_shaper_config_t entry;

    aos_mem_zero(&entry, sizeof (fal_shaper_config_t));

    do
    {
        cmd = get_sub_cmd("meter_unit", "0-1");
		SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: integer\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_uint32(cmd, &(entry.meter_unit), sizeof (a_uint32_t));
            if (SW_OK != rv)
                dprintf("usage: integer\n");
        }
    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("cshaper_enable", "no");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {

            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: <yes/no/y/n>\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_confirm(cmd, A_FALSE, &(entry.c_shaper_en),
                                        sizeof (a_bool_t));
            if (SW_OK != rv)
                dprintf("usage: <yes/no/y/n>\n");
        }
    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("cir", "0");
		SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: integer\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_uint32(cmd, &(entry.cir), sizeof (a_uint32_t));
            if (SW_OK != rv)
                dprintf("usage: integer\n");
        }
    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("cbs", "0");
		SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: integer\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_uint32(cmd, &(entry.cbs), sizeof (a_uint32_t));
            if (SW_OK != rv)
                dprintf("usage: integer\n");
        }
    }
    while (talk_mode && (SW_OK != rv));

     do
    {
        cmd = get_sub_cmd("shaper_frame_mode", "0-2");
		SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: integer\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_uint32(cmd, &(entry.shaper_frame_mode), sizeof (fal_shaper_frame_mode_t));
            if (SW_OK != rv)
                dprintf("usage: integer\n");
        }
    }
    while (talk_mode && (SW_OK != rv));

    *(fal_shaper_config_t *)val = entry;
    return SW_OK;
}


sw_error_t
cmd_data_check_shaper_config(char *cmd_str, void * val, a_uint32_t size)
{
    char *cmd;
    sw_error_t rv;
    fal_shaper_config_t entry;

    aos_mem_zero(&entry, sizeof (fal_shaper_config_t));

    do
    {
        cmd = get_sub_cmd("couple_enable", "no");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {

            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: <yes/no/y/n>\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_confirm(cmd, A_FALSE, &(entry.couple_en),
                                        sizeof (a_bool_t));
            if (SW_OK != rv)
                dprintf("usage: <yes/no/y/n>\n");
        }
    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("meter_unit", "0-1");
		SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: integer\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_uint32(cmd, &(entry.meter_unit), sizeof (a_uint32_t));
            if (SW_OK != rv)
                dprintf("usage: integer\n");
        }
    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("cshaper_enable", "no");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {

            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: <yes/no/y/n>\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_confirm(cmd, A_FALSE, &(entry.c_shaper_en),
                                        sizeof (a_bool_t));
            if (SW_OK != rv)
                dprintf("usage: <yes/no/y/n>\n");
        }
    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("cir", "0");
		SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: integer\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_uint32(cmd, &(entry.cir), sizeof (a_uint32_t));
            if (SW_OK != rv)
                dprintf("usage: integer\n");
        }
    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("cbs", "0");
		SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: integer\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_uint32(cmd, &(entry.cbs), sizeof (a_uint32_t));
            if (SW_OK != rv)
                dprintf("usage: integer\n");
        }
    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("eshaper_enable", "no");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {

            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: <yes/no/y/n>\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_confirm(cmd, A_FALSE, &(entry.e_shaper_en),
                                        sizeof (a_bool_t));
            if (SW_OK != rv)
                dprintf("usage: <yes/no/y/n>\n");
        }
    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("eir", "0");
		SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: integer\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_uint32(cmd, &(entry.eir), sizeof (a_uint32_t));
            if (SW_OK != rv)
                dprintf("usage: integer\n");
        }
    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("ebs", "0");
		SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: integer\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_uint32(cmd, &(entry.ebs), sizeof (a_uint32_t));
            if (SW_OK != rv)
                dprintf("usage: integer\n");
        }
    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("shaper_frame_mode", "0-2");
		SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: integer\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_uint32(cmd, &(entry.shaper_frame_mode), sizeof (fal_shaper_frame_mode_t));
            if (SW_OK != rv)
                dprintf("usage: integer\n");
        }
    }
    while (talk_mode && (SW_OK != rv));

    *(fal_shaper_config_t *)val = entry;
    return SW_OK;
}

void
cmd_data_print_shaper_token_config(a_uint8_t * param_name, a_uint32_t * buf, a_uint32_t size)
{
	fal_shaper_token_number_t *entry;
	entry = (fal_shaper_token_number_t *) buf;

	if (A_TRUE == entry->c_token_number_negative_en)
	{
		dprintf("\n[shaper_ctoken_negative_enable]:yes  ");
	}
	else
	{
		dprintf("\n[shaper_ctoken_negative_enable]:no  ");
	}
	dprintf("\n[shaper_ctoken_number]:0x%x", entry->c_token_number);

	if (A_TRUE == entry->e_token_number_negative_en)
	{
		dprintf("\n[shaper_etoken_negative_enable]:yes  ");
	}
	else
	{
		dprintf("\n[shaper_etoken_negative_enable]:no  ");
	}

	dprintf("\n[shaper_etoken_number]:0x%x", entry->e_token_number);

	return;
}

void
cmd_data_print_port_shaper_token_config(a_uint8_t * param_name, a_uint32_t * buf, a_uint32_t size)
{
	fal_shaper_token_number_t *entry;
	entry = (fal_shaper_token_number_t *) buf;

	if (A_TRUE == entry->c_token_number_negative_en)
	{
		dprintf("\n[shaper_ctoken_negative_enable]:yes  ");
	}
	else
	{
		dprintf("\n[shaper_ctoken_negative_enable]:no  ");
	}
	dprintf("\n[shaper_ctoken_number]:0x%x", entry->c_token_number);

	return;
}

void
cmd_data_print_shaper_config(a_uint8_t * param_name, a_uint32_t * buf, a_uint32_t size)
{
    fal_shaper_config_t *entry;

    entry = (fal_shaper_config_t *) buf;

    if (A_TRUE == entry->couple_en)
    {
        dprintf("\n[shaper_couple_enable]:yes  ");
    }
    else
    {
        dprintf("\n[shaper_couple_enable]:no  ");
    }

    dprintf("\n[shaper_meter_unit]:0x%x", entry->meter_unit);

    if (A_TRUE == entry->c_shaper_en)
    {
        dprintf("\n[shaper_cshaper_enable]:yes  ");
    }
    else
    {
        dprintf("\n[shaper_cshaper_enable]:no  ");
    }

    dprintf("\n[shaper_cir]:0x%x", entry->cir);
    dprintf("\n[shaper_cbs]:0x%x", entry->cbs);

    if (A_TRUE == entry->e_shaper_en)
    {
        dprintf("\n[shaper_eshaper_enable]:yes  ");
    }
    else
    {
        dprintf("\n[shaper_eshaper_enable]:no  ");
    }

    dprintf("\n[shaper_eir]:0x%x", entry->eir);
    dprintf("\n[shaper_ebs]:0x%x", entry->ebs);

    dprintf("\n[shaper_frame_mode]:0x%x", entry->shaper_frame_mode);

    return;
}

void
cmd_data_print_port_shaper_config(a_uint8_t * param_name, a_uint32_t * buf, a_uint32_t size)
{
    fal_shaper_config_t *entry;

    entry = (fal_shaper_config_t *) buf;

    dprintf("\n[shaper_meter_unit]:0x%x", entry->meter_unit);

    if (A_TRUE == entry->c_shaper_en)
    {
        dprintf("\n[shaper_cshaper_enable]:yes  ");
    }
    else
    {
        dprintf("\n[shaper_cshaper_enable]:no  ");
    }

    dprintf("\n[shaper_cir]:0x%x", entry->cir);
    dprintf("\n[shaper_cbs]:0x%x", entry->cbs);

    dprintf("\n[shaper_frame_mode]:0x%x", entry->shaper_frame_mode);

    return;
}

sw_error_t
cmd_data_check_port_policer_config(char *cmd_str, void * val, a_uint32_t size)
{
    char *cmd;
    sw_error_t rv;
    fal_policer_config_t entry;

    aos_mem_zero(&entry, sizeof (fal_policer_config_t));

    do
    {
        cmd = get_sub_cmd("meter_enable", "no");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {

            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: <yes/no/y/n>\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_confirm(cmd, A_FALSE, &(entry.meter_en),
                                        sizeof (a_bool_t));
            if (SW_OK != rv)
                dprintf("usage: <yes/no/y/n>\n");
        }
    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("couple_enable", "no");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {

            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: <yes/no/y/n>\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_confirm(cmd, A_FALSE, &(entry.couple_en),
                                        sizeof (a_bool_t));
            if (SW_OK != rv)
                dprintf("usage: <yes/no/y/n>\n");
        }
    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("color_mode", "0-1");
		SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: integer\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_uint32(cmd, &(entry.color_mode), sizeof (a_uint32_t));
            if (SW_OK != rv)
                dprintf("usage: integer\n");
        }
    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("frame_type", "0-0x1f");
		SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: integer\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_uint32(cmd, &(entry.frame_type), sizeof (a_uint32_t));
            if (SW_OK != rv)
                dprintf("usage: integer\n");
        }
    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("meter_mode", "0-1");
		SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: integer\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_uint32(cmd, &(entry.meter_mode), sizeof (a_uint32_t));
            if (SW_OK != rv)
                dprintf("usage: integer\n");
        }
    }
    while (talk_mode && (SW_OK != rv));


    do
    {
        cmd = get_sub_cmd("meter_unit", "0-1");
		SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: integer\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_uint32(cmd, &(entry.meter_unit), sizeof (a_uint32_t));
            if (SW_OK != rv)
                dprintf("usage: integer\n");
        }
    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("cir", "0");
		SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: integer\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_uint32(cmd, &(entry.cir), sizeof (a_uint32_t));
            if (SW_OK != rv)
                dprintf("usage: integer\n");
        }
    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("cbs", "0");
		SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: integer\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_uint32(cmd, &(entry.cbs), sizeof (a_uint32_t));
            if (SW_OK != rv)
                dprintf("usage: integer\n");
        }
    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("eir", "0");
		SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: integer\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_uint32(cmd, &(entry.eir), sizeof (a_uint32_t));
            if (SW_OK != rv)
                dprintf("usage: integer\n");
        }
    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("ebs", "0");
		SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: integer\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_uint32(cmd, &(entry.ebs), sizeof (a_uint32_t));
            if (SW_OK != rv)
                dprintf("usage: integer\n");
        }
    }
    while (talk_mode && (SW_OK != rv));

    *(fal_policer_config_t *)val = entry;
    return SW_OK;
}

sw_error_t
cmd_data_check_acl_policer_config(char *cmd_str, void * val, a_uint32_t size)
{
    char *cmd;
    sw_error_t rv;
    fal_policer_config_t entry;

    aos_mem_zero(&entry, sizeof (fal_policer_config_t));

    do
    {
        cmd = get_sub_cmd("meter_enable", "no");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {

            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: <yes/no/y/n>\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_confirm(cmd, A_FALSE, &(entry.meter_en),
                                        sizeof (a_bool_t));
            if (SW_OK != rv)
                dprintf("usage: <yes/no/y/n>\n");
        }
    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("couple_enable", "no");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {

            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: <yes/no/y/n>\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_confirm(cmd, A_FALSE, &(entry.couple_en),
                                        sizeof (a_bool_t));
            if (SW_OK != rv)
                dprintf("usage: <yes/no/y/n>\n");
        }
    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("color_mode", "0-1");
		SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: integer\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_uint32(cmd, &(entry.color_mode), sizeof (a_uint32_t));
            if (SW_OK != rv)
                dprintf("usage: integer\n");
        }
    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("meter_mode", "0-1");
		SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: integer\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_uint32(cmd, &(entry.meter_mode), sizeof (a_uint32_t));
            if (SW_OK != rv)
                dprintf("usage: integer\n");
        }
    }
    while (talk_mode && (SW_OK != rv));


    do
    {
        cmd = get_sub_cmd("meter_unit", "0-1");
		SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: integer\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_uint32(cmd, &(entry.meter_unit), sizeof (a_uint32_t));
            if (SW_OK != rv)
                dprintf("usage: integer\n");
        }
    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("cir", "0");
		SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: integer\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_uint32(cmd, &(entry.cir), sizeof (a_uint32_t));
            if (SW_OK != rv)
                dprintf("usage: integer\n");
        }
    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("cbs", "0");
		SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: integer\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_uint32(cmd, &(entry.cbs), sizeof (a_uint32_t));
            if (SW_OK != rv)
                dprintf("usage: integer\n");
        }
    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("eir", "0");
		SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: integer\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_uint32(cmd, &(entry.eir), sizeof (a_uint32_t));
            if (SW_OK != rv)
                dprintf("usage: integer\n");
        }
    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("ebs", "0");
		SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: integer\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_uint32(cmd, &(entry.ebs), sizeof (a_uint32_t));
            if (SW_OK != rv)
                dprintf("usage: integer\n");
        }
    }
    while (talk_mode && (SW_OK != rv));

    *(fal_policer_config_t *)val = entry;
    return SW_OK;
}


sw_error_t
cmd_data_check_policer_cmd_config(char *cmd_str, void * val, a_uint32_t size)
{
    char *cmd;
    sw_error_t rv;
    fal_policer_action_t entry;

    aos_mem_zero(&entry, sizeof (fal_policer_action_t));

    do
    {
        cmd = get_sub_cmd("yellow_priority_remark", "no");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {

            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: <yes/no/y/n>\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_confirm(cmd, A_FALSE, &(entry.yellow_priority_en),
                                        sizeof (a_bool_t));
            if (SW_OK != rv)
                dprintf("usage: <yes/no/y/n>\n");
        }
    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("yellow_drop_priority_remark", "no");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {

            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: <yes/no/y/n>\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_confirm(cmd, A_FALSE, &(entry.yellow_drop_priority_en),
                                        sizeof (a_bool_t));
            if (SW_OK != rv)
                dprintf("usage: <yes/no/y/n>\n");
        }
    }
    while (talk_mode && (SW_OK != rv));


    do
    {
        cmd = get_sub_cmd("yellow_pcp_remark", "no");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {

            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: <yes/no/y/n>\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_confirm(cmd, A_FALSE, &(entry.yellow_pcp_en),
                                        sizeof (a_bool_t));
            if (SW_OK != rv)
                dprintf("usage: <yes/no/y/n>\n");
        }
    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("yellow_dei_remark", "no");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {

            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: <yes/no/y/n>\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_confirm(cmd, A_FALSE, &(entry.yellow_dei_en),
                                        sizeof (a_bool_t));
            if (SW_OK != rv)
                dprintf("usage: <yes/no/y/n>\n");
        }
    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("yellow_priority", "0-15");
		SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: integer\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_uint32(cmd, &(entry.yellow_priority), sizeof (a_uint32_t));
            if (SW_OK != rv)
                dprintf("usage: integer\n");
        }
    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("yellow_drop_priority", "0-3");
		SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: integer\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_uint32(cmd, &(entry.yellow_drop_priority), sizeof (a_uint32_t));
            if (SW_OK != rv)
                dprintf("usage: integer\n");
        }
    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("yellow_pcp", "0-7");
		SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: integer\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_uint32(cmd, &(entry.yellow_pcp), sizeof (a_uint32_t));
            if (SW_OK != rv)
                dprintf("usage: integer\n");
        }
    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("yellow_dei", "0-1");
		SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: integer\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_uint32(cmd, &(entry.yellow_dei), sizeof (a_uint32_t));
            if (SW_OK != rv)
                dprintf("usage: integer\n");
        }
    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("red_action", "drop");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: forward/drop\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_maccmd(cmd, &(entry.red_action), sizeof(a_uint32_t));
            if (SW_OK != rv)
                dprintf("usage: forward/drop\n");
        }
    }while (talk_mode && (SW_OK != rv));


    do
    {
        cmd = get_sub_cmd("red_priority_remark", "no");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {

            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: <yes/no/y/n>\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_confirm(cmd, A_FALSE, &(entry.red_priority_en),
                                        sizeof (a_bool_t));
            if (SW_OK != rv)
                dprintf("usage: <yes/no/y/n>\n");
        }
    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("red_drop_priority_remark", "no");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {

            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: <yes/no/y/n>\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_confirm(cmd, A_FALSE, &(entry.red_drop_priority_en),
                                        sizeof (a_bool_t));
            if (SW_OK != rv)
                dprintf("usage: <yes/no/y/n>\n");
        }
    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("red_pcp_remark", "no");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {

            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: <yes/no/y/n>\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_confirm(cmd, A_FALSE, &(entry.red_pcp_en),
                                        sizeof (a_bool_t));
            if (SW_OK != rv)
                dprintf("usage: <yes/no/y/n>\n");
        }
    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("red_dei_remark", "no");
        SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {

            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: <yes/no/y/n>\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_confirm(cmd, A_FALSE, &(entry.red_dei_en),
                                        sizeof (a_bool_t));
            if (SW_OK != rv)
                dprintf("usage: <yes/no/y/n>\n");
        }
    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("red_priority", "0-15");
		SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: integer\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_uint32(cmd, &(entry.red_priority), sizeof (a_uint32_t));
            if (SW_OK != rv)
                dprintf("usage: integer\n");
        }
    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("red_drop_priority", "0-3");
		SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: integer\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_uint32(cmd, &(entry.red_drop_priority), sizeof (a_uint32_t));
            if (SW_OK != rv)
                dprintf("usage: integer\n");
        }
    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("red_pcp", "0-7");
		SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: integer\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_uint32(cmd, &(entry.red_pcp), sizeof (a_uint32_t));
            if (SW_OK != rv)
                dprintf("usage: integer\n");
        }
    }
    while (talk_mode && (SW_OK != rv));

    do
    {
        cmd = get_sub_cmd("red_dei", "0-1");
		SW_RTN_ON_NULL_PARAM(cmd);

        if (!strncasecmp(cmd, "quit", 4))
        {
            return SW_BAD_VALUE;
        }
        else if (!strncasecmp(cmd, "help", 4))
        {
            dprintf("usage: integer\n");
            rv = SW_BAD_VALUE;
        }
        else
        {
            rv = cmd_data_check_uint32(cmd, &(entry.red_dei), sizeof (a_uint32_t));
            if (SW_OK != rv)
                dprintf("usage: integer\n");
        }
    }
    while (talk_mode && (SW_OK != rv));

    *(fal_policer_action_t *)val = entry;
    return SW_OK;
}


void
cmd_data_print_port_policer_config(a_uint8_t * param_name, a_uint32_t * buf, a_uint32_t size)
{
    fal_policer_config_t *entry;

    entry = (fal_policer_config_t *) buf;

    if (A_TRUE == entry->meter_en)
    {
        dprintf("\n[meter_enable]:yes  ");
    }
    else
    {
        dprintf("\n[meter_enable]:no  ");
    }

    if (A_TRUE == entry->couple_en)
    {
        dprintf("\n[couple_enable]:yes  ");
    }
    else
    {
        dprintf("\n[coupler_enable]:no  ");
    }

    dprintf("\n[color_mode]:0x%x", entry->color_mode);
    dprintf("\n[frame_type]:0x%x", entry->frame_type);
    dprintf("\n[meter_mode]:0x%x", entry->meter_mode);

    if (FAL_BYTE_BASED == entry->meter_unit)
    {
        dprintf("\n[meter_unit]:byte_based  ");
    }
    else
    {
        dprintf("\n[meter_unit]:frame_based  ");
    }

    dprintf("\n[cir]:0x%08x  [cbs]:0x%08x  ", entry->cir,entry->cbs);
    dprintf("\n[eir]:0x%08x  [ebs]:0x%08x  ", entry->eir,entry->ebs);

    return;
}


void
cmd_data_print_policer_cmd_config(a_uint8_t * param_name, a_uint32_t * buf, a_uint32_t size)
{
    fal_policer_action_t *entry;

    entry = (fal_policer_action_t *) buf;

    if (A_TRUE == entry->yellow_priority_en)
    {
        dprintf("\n[yellow_priority_remark]:yes  ");
    }
    else
    {
        dprintf("\n[yellow_priority_remark]:no  ");
    }

    if (A_TRUE == entry->yellow_drop_priority_en)
    {
        dprintf("\n[yellow_drop_priority_remark]:yes  ");
    }
    else
    {
        dprintf("\n[yellow_drop_priority_remark]:no  ");
    }

    if (A_TRUE == entry->yellow_pcp_en)
    {
        dprintf("\n[yellow_pcp_remark]:yes  ");
    }
    else
    {
        dprintf("\n[yellow_pcp_remark]:no  ");
    }

    if (A_TRUE == entry->yellow_dei_en)
    {
        dprintf("\n[yellow_dei_remark]:yes  ");
    }
    else
    {
        dprintf("\n[yellow_dei_remark]:no  ");
    }

    dprintf("\n[yellow_priority]:0x%x", entry->yellow_priority);
    dprintf("\n[yellow_drop_priority]:0x%x", entry->yellow_drop_priority);
    dprintf("\n[yellow_pcp]:0x%x", entry->yellow_pcp);
    dprintf("\n[yellow_dei]:0x%x", entry->yellow_dei);

    if (FAL_MAC_DROP == entry->red_action)
    {
        dprintf("\n[red_action]:drop");
    }
    else
    {
        dprintf("\n[red_action]:forward");
    }

    if (A_TRUE == entry->red_priority_en)
    {
        dprintf("\n[red_priority_remark]:yes  ");
    }
    else
    {
        dprintf("\n[red_priority_remark]:no  ");
    }

    if (A_TRUE == entry->red_drop_priority_en)
    {
        dprintf("\n[red_drop_priority_remark]:yes  ");
    }
    else
    {
        dprintf("\n[red_drop_priority_remark]:no  ");
    }

    if (A_TRUE == entry->red_pcp_en)
    {
        dprintf("\n[red_pcp_remark]:yes  ");
    }
    else
    {
        dprintf("\n[red_pcp_remark]:no  ");
    }

    if (A_TRUE == entry->red_dei_en)
    {
        dprintf("\n[red_dei_remark]:yes  ");
    }
    else
    {
        dprintf("\n[red_dei_remark]:no  ");
    }

    dprintf("\n[red_priority]:0x%x", entry->red_priority);
    dprintf("\n[red_drop_priority]:0x%x", entry->red_drop_priority);
    dprintf("\n[red_pcp]:0x%x", entry->red_pcp);
    dprintf("\n[red_dei]:0x%x", entry->red_dei);

    return;
}

void
cmd_data_print_acl_policer_config(a_uint8_t * param_name, a_uint32_t * buf, a_uint32_t size)
{
    fal_policer_config_t *entry;

    entry = (fal_policer_config_t *) buf;

    if (A_TRUE == entry->meter_en)
    {
        dprintf("[meter_enable]:yes  ");
    }
    else
    {
        dprintf("\n[meter_enable]:no  ");
    }

    if (A_TRUE == entry->couple_en)
    {
        dprintf("\n[couple_enable]:yes  ");
    }
    else
    {
        dprintf("\n[coupler_enable]:no  ");
    }

    dprintf("\n[color_mode]:0x%x", entry->color_mode);

    dprintf("\n[meter_mode]:0x%x", entry->meter_mode);

    if (FAL_BYTE_BASED == entry->meter_unit)
    {
        dprintf("\n[meter_unit]:byte_based  ");
    }
    else
    {
        dprintf("\n[meter_unit]:frame_based  ");
    }

    dprintf("\n[cir]:0x%08x  [cbs]:0x%08x  ", entry->cir,entry->cbs);
    dprintf("\n[eir]:0x%08x  [ebs]:0x%08x  ", entry->eir,entry->ebs);

    return;
}

void
cmd_data_print_policer_counter_infor(a_uint8_t * param_name, a_uint32_t * buf, a_uint32_t size)
{
	fal_policer_counter_t *entry;
	entry = (fal_policer_counter_t *) buf;

	dprintf("\n[green_packet_counter]:0x%x", entry->green_packet_counter);
	dprintf("\n[green_byte_counter]:0x%llx", entry->green_byte_counter);

	dprintf("\n[yellow_packet_counter]:0x%x", entry->yellow_packet_counter);
	dprintf("\n[yellow_byte_counter]:0x%llx", entry->yellow_byte_counter);

	dprintf("\n[red_packet_counter]:0x%x", entry->red_packet_counter);
	dprintf("\n[red_byte_counter]:0x%llx", entry->red_byte_counter);

	return;
}

void
cmd_data_print_policer_global_counter_infor(a_uint8_t * param_name, a_uint32_t * buf, a_uint32_t size)
{
	fal_policer_global_counter_t *entry;
	entry = (fal_policer_global_counter_t *) buf;

	dprintf("\n[policer_drop_packet_counter]:0x%x", entry->policer_drop_packet_counter);
	dprintf("\n[policer_drop_byte_counter]:0x%llx", entry->policer_drop_byte_counter);

	dprintf("\n[policer_forward_packet_counter]:0x%x", entry->policer_forward_packet_counter);
	dprintf("\n[policer_forward_byte_counter]:0x%llx", entry->policer_forward_byte_counter);

	dprintf("\n[policer_bypass_packet_counter]:0x%x", entry->policer_bypass_packet_counter);
	dprintf("\n[policer_bypass_byte_counter]:0x%llx", entry->policer_bypass_byte_counter);

	return;
}

sw_error_t
cmd_data_check_ptp_config(char *info, void *val, a_uint32_t size)
{
	char *cmd;
	sw_error_t rv;
	fal_ptp_config_t entry;

	aos_mem_zero(&entry, sizeof (fal_ptp_config_t));

	/* ptp en */
	do
	{
		cmd = get_sub_cmd("ptp_en", "yes");
		SW_RTN_ON_NULL_PARAM(cmd);

		if (!strncasecmp(cmd, "quit", 4))
		{
			return SW_BAD_VALUE;
		}
		else if (!strncasecmp(cmd, "help", 4))
		{
			dprintf("usage: <yes/no/y/n>\n");
			rv = SW_BAD_VALUE;
		}
		else
		{
			rv = cmd_data_check_confirm(cmd, A_FALSE, &(entry.ptp_en),
					sizeof (a_bool_t));
			if (SW_OK != rv)
				dprintf("usage: <yes/no/y/n>\n");
		}
	}
	while (talk_mode && (SW_OK != rv));

	/* clock mode */
	do
	{
		cmd = get_sub_cmd("clock_mode", "0");
		SW_RTN_ON_NULL_PARAM(cmd);

		if (!strncasecmp(cmd, "quit", 4))
		{
			return SW_BAD_VALUE;
		}
		else if (!strncasecmp(cmd, "help", 4))
		{
			dprintf("usage: 0 for OC, 1 for BC, 2 for E2ETC and 3 for P2PTC\n");
			rv = SW_BAD_VALUE;
		}
		else
		{
			rv = cmd_data_check_uint8(cmd, &(entry.clock_mode), sizeof(a_uint8_t));
			if (SW_OK != rv)
				dprintf("usage: 0 for OC, 1 for BC, 2 for E2ETC and 3 for P2PTC\n");
		}
	}while (talk_mode && (SW_OK != rv));

	/* step mode */
	do
	{
		cmd = get_sub_cmd("step_mode", "0");
		SW_RTN_ON_NULL_PARAM(cmd);

		if (!strncasecmp(cmd, "quit", 4))
		{
			return SW_BAD_VALUE;
		}
		else if (!strncasecmp(cmd, "help", 4))
		{
			dprintf("usage: 0 for one step, 1 for two step and 2 for auto mode\n");
			rv = SW_BAD_VALUE;
		}
		else
		{
			rv = cmd_data_check_uint8(cmd, &(entry.step_mode), sizeof(a_uint8_t));
			if (SW_OK != rv)
				dprintf("usage: 0 for one step, 1 for two step, 2 for auto mode\n");
		}
	}while (talk_mode && (SW_OK != rv));

	*(fal_ptp_config_t *)val = entry;
	return SW_OK;
}

void
cmd_data_print_ptp_config(a_uint8_t * param_name, a_uint32_t * buf, a_uint32_t size)
{
	fal_ptp_config_t *entry;

	entry = (fal_ptp_config_t *) buf;
	dprintf("[ptp_en]:%s\n", entry->ptp_en ? "YES" : "NO");
	dprintf("[clock_mode]:%d [step_mode]:%d\n", entry->clock_mode, entry->step_mode);
	dprintf("\n");

}

sw_error_t
cmd_data_check_ptp_reference_clock(char *info, void *val, a_uint32_t size)
{
	a_uint32_t *pValue = (a_uint32_t *)val;

	if (info == NULL)
		return SW_BAD_PARAM;

	if (!strcasecmp(info, "local"))
	{
		*pValue = FAL_REF_CLOCK_LOCAL;
	}
	else if (!strcasecmp(info, "synce"))
	{
		*pValue = FAL_REF_CLOCK_SYNCE;
	}
	else if (!strcasecmp(info, "external"))
	{
		*pValue = FAL_REF_CLOCK_EXTERNAL;
	}
	else
	{
		//dprintf("input error \n");
		return SW_BAD_VALUE;
	}

	return SW_OK;
}

void
cmd_data_print_ptp_reference_clock(a_uint8_t * param_name, a_uint32_t * buf, a_uint32_t size)
{
	if (*(a_uint32_t *) buf == FAL_REF_CLOCK_LOCAL)
	{
		dprintf("local");
	}
	else if (*(a_uint32_t *) buf == FAL_REF_CLOCK_SYNCE)
	{
		dprintf("synce");
	}
	else if (*(a_uint32_t *) buf == FAL_REF_CLOCK_EXTERNAL)
	{
		dprintf("external");
	}
	else
	{
		dprintf("UNKNOWN VALUE");
	}

}

sw_error_t
cmd_data_check_ptp_rx_timestamp_mode(char *info, void *val, a_uint32_t size)
{
	a_uint32_t *pValue = (a_uint32_t *)val;

	if (info == NULL)
		return SW_BAD_PARAM;

	if (!strcasecmp(info, "mdio"))
	{
		*pValue = FAL_RX_TS_MDIO;
	}
	else if (!strcasecmp(info, "embed"))
	{
		*pValue = FAL_RX_TS_EMBED;
	}
	else
	{
		//dprintf("input error \n");
		return SW_BAD_VALUE;
	}

	return SW_OK;
}

void
cmd_data_print_ptp_rx_timestamp_mode(a_uint8_t * param_name, a_uint32_t * buf, a_uint32_t size)
{
	if (*(a_uint32_t *) buf == FAL_RX_TS_MDIO)
	{
		dprintf("mdio");
	}
	else if (*(a_uint32_t *) buf == FAL_RX_TS_EMBED)
	{
		dprintf("embed");
	}
	else
	{
		dprintf("UNKNOWN VALUE");
	}

}

sw_error_t
cmd_data_check_ptp_direction(char *info, void *val, a_uint32_t size)
{
	a_uint32_t *pValue = (a_uint32_t *)val;

	if (info == NULL)
		return SW_BAD_PARAM;

	if (!strcasecmp(info, "rx"))
	{
		*pValue = FAL_RX_DIRECTION;
	}
	else if (!strcasecmp(info, "tx"))
	{
		*pValue = FAL_TX_DIRECTION;
	}
	else
	{
		//dprintf("input error \n");
		return SW_BAD_VALUE;
	}

	return SW_OK;
}

sw_error_t
cmd_data_check_ptp_pkt_info(char *info, void *val, a_uint32_t size)
{
	char *cmd;
	sw_error_t rv;
	fal_ptp_pkt_info_t *pEntry = (fal_ptp_pkt_info_t *)val;

	memset(pEntry, 0, sizeof(fal_ptp_pkt_info_t));

	/* sequence id */
	do
	{
		cmd = get_sub_cmd("sequence_id", "0");
		SW_RTN_ON_NULL_PARAM(cmd);

		if (!strncasecmp(cmd, "quit", 4))
		{
			return SW_BAD_VALUE;
		}
		else if (!strncasecmp(cmd, "help", 4))
		{
			dprintf("usage: sequence id, 16bits \n");
			rv = SW_BAD_VALUE;
		}
		else
		{
			rv = cmd_data_check_uint32(cmd, &(pEntry->sequence_id), sizeof(a_uint32_t));
			if (SW_OK != rv)
				dprintf("usage: sequence id, 16bits \n");
		}
	}while (talk_mode && (SW_OK != rv));

	/* clock identify */
	do
	{
		cmd = get_sub_cmd("clock_identify", "0");
		SW_RTN_ON_NULL_PARAM(cmd);

		if (!strncasecmp(cmd, "quit", 4))
		{
			return SW_BAD_VALUE;
		}
		else if (!strncasecmp(cmd, "help", 4))
		{
			dprintf("usage: clock identify, 64bits\n");
			rv = SW_BAD_VALUE;
		}
		else
		{
			rv = cmd_data_check_uint64(cmd, &(pEntry->clock_identify),
					sizeof(a_uint64_t));
			if (SW_OK != rv)
				dprintf("usage: clock identify, 64bits\n");
		}
	}while (talk_mode && (SW_OK != rv));

	/* port number */
	do
	{
		cmd = get_sub_cmd("port_number", "0");
		SW_RTN_ON_NULL_PARAM(cmd);

		if (!strncasecmp(cmd, "quit", 4))
		{
			return SW_BAD_VALUE;
		}
		else if (!strncasecmp(cmd, "help", 4))
		{
			dprintf("usage: source port id, 16bits\n");
			rv = SW_BAD_VALUE;
		}
		else
		{
			rv = cmd_data_check_uint32(cmd, &(pEntry->port_number), sizeof(a_uint32_t));
			if (SW_OK != rv)
				dprintf("usage: source port id, 16bits\n");
		}
	}while (talk_mode && (SW_OK != rv));

	/* message type */
	do
	{
		cmd = get_sub_cmd("msg_type", "0");
		SW_RTN_ON_NULL_PARAM(cmd);

		if (!strncasecmp(cmd, "quit", 4))
		{
			return SW_BAD_VALUE;
		}
		else if (!strncasecmp(cmd, "help", 4))
		{
			dprintf("usage: message, 4bits\n");
			rv = SW_BAD_VALUE;
		}
		else
		{
			rv = cmd_data_check_uint32(cmd, &(pEntry->msg_type), sizeof(a_uint32_t));
			if (SW_OK != rv)
				dprintf("usage: message type, 4bits\n");
		}
	}while (talk_mode && (SW_OK != rv));

	return SW_OK;
}

void
cmd_data_print_ptp_pkt_info(a_uint8_t * param_name, a_uint32_t * buf, a_uint32_t size)
{
	fal_ptp_pkt_info_t *entry;

	entry = (fal_ptp_pkt_info_t *) buf;
	dprintf("[sequence_id]:%s\n", entry->sequence_id);
	dprintf("[clock_identify]:0x%llx\n", entry->clock_identify);
	dprintf("[port_number]:%d [msg_type]:%d\n", entry->port_number, entry->msg_type);
	dprintf("\n");

}

sw_error_t
cmd_data_check_ptp_time(char *info, void *val, a_uint32_t size)
{
	char *cmd;
	sw_error_t rv;
	fal_ptp_time_t *pEntry = (fal_ptp_time_t *)val;

	memset(pEntry, 0, sizeof(fal_ptp_time_t));

	/* seconds */
	do
	{
		cmd = get_sub_cmd("seconds", "0");
		SW_RTN_ON_NULL_PARAM(cmd);

		if (!strncasecmp(cmd, "quit", 4))
		{
			return SW_BAD_VALUE;
		}
		else if (!strncasecmp(cmd, "help", 4))
		{
			dprintf("usage: seconds 48bits \n");
			rv = SW_BAD_VALUE;
		}
		else
		{
			rv = cmd_data_check_uint64(cmd, (a_uint64_t *) (&(pEntry->seconds)),
					sizeof(a_uint64_t));
			if (SW_OK != rv)
				dprintf("usage: seconds 48bits \n");
		}
	}while (talk_mode && (SW_OK != rv));

	/* nanoseconds */
	do
	{
		cmd = get_sub_cmd("nanoseconds", "0");
		SW_RTN_ON_NULL_PARAM(cmd);

		if (!strncasecmp(cmd, "quit", 4))
		{
			return SW_BAD_VALUE;
		}
		else if (!strncasecmp(cmd, "help", 4))
		{
			dprintf("usage: nanoseconds 32bits\n");
			rv = SW_BAD_VALUE;
		}
		else
		{
			rv = cmd_data_check_uint32(cmd, (a_uint32_t *) (&(pEntry->nanoseconds)),
					sizeof(a_uint32_t));
			if (SW_OK != rv)
				dprintf("usage: nanoseconds 32bits\n");
		}
	}while (talk_mode && (SW_OK != rv));

	/* fracnanoseconds */
	do
	{
		cmd = get_sub_cmd("fracnanoseconds", "0");
		SW_RTN_ON_NULL_PARAM(cmd);

		if (!strncasecmp(cmd, "quit", 4))
		{
			return SW_BAD_VALUE;
		}
		else if (!strncasecmp(cmd, "help", 4))
		{
			dprintf("usage: fracnanoseconds 32bits \n");
			rv = SW_BAD_VALUE;
		}
		else
		{
			rv = cmd_data_check_uint32(cmd, (a_uint32_t *) (&(pEntry->fracnanoseconds)),
					sizeof(a_uint32_t));
			if (SW_OK != rv)
				dprintf("usage: fracnanoseconds 32bits\n");
		}
	}while (talk_mode && (SW_OK != rv));

	return SW_OK;
}

void
cmd_data_print_ptp_time(a_char_t * param_name, a_uint32_t * buf, a_uint32_t size)
{
	fal_ptp_time_t *entry;

	entry = (fal_ptp_time_t *) buf;
	dprintf("[seconds]:0x%llx\n", entry->seconds);
	dprintf("[nanoseconds]:0x%x\n", entry->nanoseconds);
	dprintf("[fracnanoseconds]:0x%x\n", entry->fracnanoseconds);
	dprintf("\n");

}

sw_error_t
cmd_data_check_ptp_grandmaster_mode(char *info, void *val, a_uint32_t size)
{
	char *cmd;
	sw_error_t rv;
	fal_ptp_grandmaster_mode_t *pEntry = (fal_ptp_grandmaster_mode_t *)val;

	memset(pEntry, 0, sizeof(fal_ptp_grandmaster_mode_t));

	/* grandmaster mode en */
	do
	{
		cmd = get_sub_cmd("grandmaster_mode_en", "yes");
		SW_RTN_ON_NULL_PARAM(cmd);

		if (!strncasecmp(cmd, "quit", 4))
		{
			return SW_BAD_VALUE;
		}
		else if (!strncasecmp(cmd, "help", 4))
		{
			dprintf("usage: <yes/no/y/n>\n");
			rv = SW_BAD_VALUE;
		}
		else
		{
			rv = cmd_data_check_confirm(cmd, A_FALSE, &(pEntry->grandmaster_mode_en),
					sizeof (a_bool_t));
			if (SW_OK != rv)
				dprintf("usage: <yes/no/y/n>\n");
		}
	}
	while (talk_mode && (SW_OK != rv));

	/* grandmaster second sync en */
	do
	{
		cmd = get_sub_cmd("grandmaster_second_sync_en", "yes");
		SW_RTN_ON_NULL_PARAM(cmd);

		if (!strncasecmp(cmd, "quit", 4))
		{
			return SW_BAD_VALUE;
		}
		else if (!strncasecmp(cmd, "help", 4))
		{
			dprintf("usage: <yes/no/y/n>\n");
			rv = SW_BAD_VALUE;
		}
		else
		{
			rv = cmd_data_check_confirm(cmd, A_FALSE,
					&(pEntry->grandmaster_second_sync_en),
					sizeof (a_bool_t));
			if (SW_OK != rv)
				dprintf("usage: <yes/no/y/n>\n");
		}
	}
	while (talk_mode && (SW_OK != rv));

	/* frequency offset */
	do
	{
		cmd = get_sub_cmd("freq_offset", "0");
		SW_RTN_ON_NULL_PARAM(cmd);

		if (!strncasecmp(cmd, "quit", 4))
		{
			return SW_BAD_VALUE;
		}
		else if (!strncasecmp(cmd, "help", 4))
		{
			dprintf("usage: enum maxfrequency offset\n");
			rv = SW_BAD_VALUE;
		}
		else
		{
			rv = cmd_data_check_uint8(cmd, &(pEntry->freq_offset), sizeof(a_uint8_t));
			if (SW_OK != rv)
				dprintf("usage: enum maxfrequency offset\n");
		}
	}while (talk_mode && (SW_OK != rv));

	/* right shift in kp */
	do
	{
		cmd = get_sub_cmd("right_shift_in_kp", "yes");
		SW_RTN_ON_NULL_PARAM(cmd);

		if (!strncasecmp(cmd, "quit", 4))
		{
			return SW_BAD_VALUE;
		}
		else if (!strncasecmp(cmd, "help", 4))
		{
			dprintf("usage: <yes/no/y/n>\n");
			rv = SW_BAD_VALUE;
		}
		else
		{
			rv = cmd_data_check_confirm(cmd, A_FALSE, &(pEntry->right_shift_in_kp),
					sizeof (a_bool_t));
			if (SW_OK != rv)
				dprintf("usage: <yes/no/y/n>\n");
		}
	}while (talk_mode && (SW_OK != rv));

	/* kp value */
	do
	{
		cmd = get_sub_cmd("kp_value", "0");
		SW_RTN_ON_NULL_PARAM(cmd);

		if (!strncasecmp(cmd, "quit", 4))
		{
			return SW_BAD_VALUE;
		}
		else if (!strncasecmp(cmd, "help", 4))
		{
			dprintf("usage: kp value\n");
			rv = SW_BAD_VALUE;
		}
		else
		{
			rv = cmd_data_check_uint32(cmd, &(pEntry->kp_value), sizeof(a_uint32_t));
			if (SW_OK != rv)
				dprintf("usage: kp value\n");
		}
	}while (talk_mode && (SW_OK != rv));

	/* right shift in ki */
	do
	{
		cmd = get_sub_cmd("right_shift_in_ki", "yes");
		SW_RTN_ON_NULL_PARAM(cmd);

		if (!strncasecmp(cmd, "quit", 4))
		{
			return SW_BAD_VALUE;
		}
		else if (!strncasecmp(cmd, "help", 4))
		{
			dprintf("usage: <yes/no/y/n>\n");
			rv = SW_BAD_VALUE;
		}
		else
		{
			rv = cmd_data_check_confirm(cmd, A_FALSE, &(pEntry->right_shift_in_ki),
					sizeof (a_bool_t));
			if (SW_OK != rv)
				dprintf("usage: <yes/no/y/n>\n");
		}
	}while (talk_mode && (SW_OK != rv));

	/* ki value */
	do
	{
		cmd = get_sub_cmd("ki_value", "0");
		SW_RTN_ON_NULL_PARAM(cmd);

		if (!strncasecmp(cmd, "quit", 4))
		{
			return SW_BAD_VALUE;
		}
		else if (!strncasecmp(cmd, "help", 4))
		{
			dprintf("usage: ki value\n");
			rv = SW_BAD_VALUE;
		}
		else
		{
			rv = cmd_data_check_uint32(cmd, &(pEntry->ki_value), sizeof(a_uint32_t));
			if (SW_OK != rv)
				dprintf("usage: ki value\n");
		}
	}while (talk_mode && (SW_OK != rv));

	/* ns sync mode */
	do
	{
		cmd = get_sub_cmd("ns_sync_mode", "0");
		SW_RTN_ON_NULL_PARAM(cmd);

		if (!strncasecmp(cmd, "quit", 4))
		{
			return SW_BAD_VALUE;
		}
		else if (!strncasecmp(cmd, "help", 4))
		{
			dprintf("usage: 0 for PPSIN, 1 for HWPLL, 2 for SWPLL\n");
			rv = SW_BAD_VALUE;
		}
		else
		{
			rv = cmd_data_check_uint8(cmd, &(pEntry->ns_sync_mode), sizeof(a_uint8_t));
			if (SW_OK != rv)
				dprintf("usage: 0 for PPSIN, 1 for HWPLL, 2 for SWPLL\n");
		}
	}while (talk_mode && (SW_OK != rv));

	return SW_OK;
}

void
cmd_data_print_ptp_grandmaster_mode(a_uint8_t * param_name, a_uint32_t * buf, a_uint32_t size)
{
	fal_ptp_grandmaster_mode_t *entry;

	entry = (fal_ptp_grandmaster_mode_t *) buf;
	dprintf("[grandmaster_mode_en]:%s\n",
			entry->grandmaster_mode_en ? "YES" : "NO");
	dprintf("[grandmaster_second_sync_en]:%s\n",
			entry->grandmaster_second_sync_en ? "YES" : "NO");
	dprintf("[freq_offset]:%d\n", entry->freq_offset);
	dprintf("[right_shift_in_kp]:%s [kp_value]:%d\n",
			entry->right_shift_in_kp ? "YES" : "NO", entry->kp_value);
	dprintf("[right_shift_in_ki]:%s [ki_value]:%d\n",
			entry->right_shift_in_ki ? "YES" : "NO", entry->ki_value);
	dprintf("[ns_sync_mode]:%d\n", entry->ns_sync_mode);
	dprintf("\n");

}

sw_error_t
cmd_data_check_ptp_security(char *info, void *val, a_uint32_t size)
{
	char *cmd;
	sw_error_t rv;
	fal_ptp_security_t *pEntry = (fal_ptp_security_t *)val;

	memset(pEntry, 0, sizeof(fal_ptp_security_t));

	/* address_check_en */
	do
	{
		cmd = get_sub_cmd("address_check_en", "yes");
		SW_RTN_ON_NULL_PARAM(cmd);

		if (!strncasecmp(cmd, "quit", 4))
		{
			return SW_BAD_VALUE;
		}
		else if (!strncasecmp(cmd, "help", 4))
		{
			dprintf("usage: <yes/no/y/n>\n");
			rv = SW_BAD_VALUE;
		}
		else
		{
			rv = cmd_data_check_confirm(cmd, A_FALSE, &(pEntry->address_check_en),
					sizeof (a_bool_t));
			if (SW_OK != rv)
				dprintf("usage: <yes/no/y/n>\n");
		}
	}
	while (talk_mode && (SW_OK != rv));

	/* ipv6_udp_checksum_recal_en */
	do
	{
		cmd = get_sub_cmd("ipv6_udp_checksum_recal_en", "yes");
		SW_RTN_ON_NULL_PARAM(cmd);

		if (!strncasecmp(cmd, "quit", 4))
		{
			return SW_BAD_VALUE;
		}
		else if (!strncasecmp(cmd, "help", 4))
		{
			dprintf("usage: <yes/no/y/n>\n");
			rv = SW_BAD_VALUE;
		}
		else
		{
			rv = cmd_data_check_confirm(cmd, A_FALSE,
					&(pEntry->ipv6_udp_checksum_recal_en),
					sizeof (a_bool_t));
			if (SW_OK != rv)
				dprintf("usage: <yes/no/y/n>\n");
		}
	}
	while (talk_mode && (SW_OK != rv));

	/* version_check_en */
	do
	{
		cmd = get_sub_cmd("version_check_en", "yes");
		SW_RTN_ON_NULL_PARAM(cmd);

		if (!strncasecmp(cmd, "quit", 4))
		{
			return SW_BAD_VALUE;
		}
		else if (!strncasecmp(cmd, "help", 4))
		{
			dprintf("usage: <yes/no/y/n>\n");
			rv = SW_BAD_VALUE;
		}
		else
		{
			rv = cmd_data_check_confirm(cmd, A_FALSE,
					&(pEntry->version_check_en),
					sizeof (a_bool_t));
			if (SW_OK != rv)
				dprintf("usage: <yes/no/y/n>\n");
		}
	}
	while (talk_mode && (SW_OK != rv));


	/* ptp_version */
	do
	{
		cmd = get_sub_cmd("ptp_version", "0");
		SW_RTN_ON_NULL_PARAM(cmd);

		if (!strncasecmp(cmd, "quit", 4))
		{
			return SW_BAD_VALUE;
		}
		else if (!strncasecmp(cmd, "help", 4))
		{
			dprintf("usage: version, 8bits\n");
			rv = SW_BAD_VALUE;
		}
		else
		{
			rv = cmd_data_check_uint32(cmd, &(pEntry->ptp_version), sizeof(a_uint32_t));
			if (SW_OK != rv)
				dprintf("usage: version, 8bits\n");
		}
	}while (talk_mode && (SW_OK != rv));

	/* ipv4_udp_checksum_force_zero_en */
	do
	{
		cmd = get_sub_cmd("ipv4_udp_checksum_force_zero_en", "yes");
		SW_RTN_ON_NULL_PARAM(cmd);

		if (!strncasecmp(cmd, "quit", 4))
		{
			return SW_BAD_VALUE;
		}
		else if (!strncasecmp(cmd, "help", 4))
		{
			dprintf("usage: <yes/no/y/n>\n");
			rv = SW_BAD_VALUE;
		}
		else
		{
			rv = cmd_data_check_confirm(cmd, A_FALSE,
					&(pEntry->ipv4_udp_checksum_force_zero_en),
					sizeof (a_bool_t));
			if (SW_OK != rv)
				dprintf("usage: <yes/no/y/n>\n");
		}
	}
	while (talk_mode && (SW_OK != rv));

	/* ipv6_embed_udp_checksum_force_zero_en */
	do
	{
		cmd = get_sub_cmd("ipv6_embed_udp_checksum_force_zero_en", "yes");
		SW_RTN_ON_NULL_PARAM(cmd);

		if (!strncasecmp(cmd, "quit", 4))
		{
			return SW_BAD_VALUE;
		}
		else if (!strncasecmp(cmd, "help", 4))
		{
			dprintf("usage: <yes/no/y/n>\n");
			rv = SW_BAD_VALUE;
		}
		else
		{
			rv = cmd_data_check_confirm(cmd, A_FALSE,
					&(pEntry->ipv6_embed_udp_checksum_force_zero_en),
					sizeof (a_bool_t));
			if (SW_OK != rv)
				dprintf("usage: <yes/no/y/n>\n");
		}
	}
	while (talk_mode && (SW_OK != rv));


	return SW_OK;
}

void
cmd_data_print_ptp_security(a_uint8_t * param_name, a_uint32_t * buf, a_uint32_t size)
{
	fal_ptp_security_t *entry;

	entry = (fal_ptp_security_t *) buf;
	dprintf("[address_check_en]:%s\n",
			entry->address_check_en ? "YES" : "NO");
	dprintf("[ipv6_udp_checksum_recal_en]:%s\n",
			entry->ipv6_udp_checksum_recal_en ? "YES" : "NO");
	dprintf("[version_check_en]:%s\n",
			entry->version_check_en ? "YES" : "NO");
	dprintf("[ptp_version]:%d\n", entry->ptp_version);
	dprintf("[ipv4_udp_checksum_force_zero_en]:%s\n",
			entry->ipv4_udp_checksum_force_zero_en ? "YES" : "NO");
	dprintf("[ipv6_embed_udp_checksum_force_zero_en]:%s\n",
			entry->ipv6_embed_udp_checksum_force_zero_en ? "YES" : "NO");
	dprintf("\n");

}

sw_error_t
cmd_data_check_ptp_pps_sig_ctrl(char *info, void *val, a_uint32_t size)
{
	char *cmd;
	sw_error_t rv;
	fal_ptp_pps_signal_control_t *pEntry = (fal_ptp_pps_signal_control_t *)val;

	memset(pEntry, 0, sizeof(fal_ptp_pps_signal_control_t));

	/* negative_in_latency */
	do
	{
		cmd = get_sub_cmd("negative_in_latency", "yes");
		SW_RTN_ON_NULL_PARAM(cmd);

		if (!strncasecmp(cmd, "quit", 4))
		{
			return SW_BAD_VALUE;
		}
		else if (!strncasecmp(cmd, "help", 4))
		{
			dprintf("usage: <yes/no/y/n>\n");
			rv = SW_BAD_VALUE;
		}
		else
		{
			rv = cmd_data_check_confirm(cmd, A_FALSE, &(pEntry->negative_in_latency),
					sizeof (a_bool_t));
			if (SW_OK != rv)
				dprintf("usage: <yes/no/y/n>\n");
		}
	}
	while (talk_mode && (SW_OK != rv));

	/* in_latency */
	do
	{
		cmd = get_sub_cmd("in_latency", "0");
		SW_RTN_ON_NULL_PARAM(cmd);

		if (!strncasecmp(cmd, "quit", 4))
		{
			return SW_BAD_VALUE;
		}
		else if (!strncasecmp(cmd, "help", 4))
		{
			dprintf("usage: input latency\n");
			rv = SW_BAD_VALUE;
		}
		else
		{
			rv = cmd_data_check_uint32(cmd, &(pEntry->in_latency), sizeof(a_uint32_t));
			if (SW_OK != rv)
				dprintf("usage: input latency\n");
		}
	}while (talk_mode && (SW_OK != rv));

	/* out_phase */
	do
	{
		cmd = get_sub_cmd("out_phase", "0");
		SW_RTN_ON_NULL_PARAM(cmd);

		if (!strncasecmp(cmd, "quit", 4))
		{
			return SW_BAD_VALUE;
		}
		else if (!strncasecmp(cmd, "help", 4))
		{
			dprintf("usage: output phase\n");
			rv = SW_BAD_VALUE;
		}
		else
		{
			rv = cmd_data_check_uint32(cmd, &(pEntry->out_phase), sizeof(a_uint32_t));
			if (SW_OK != rv)
				dprintf("usage: output phase\n");
		}
	}while (talk_mode && (SW_OK != rv));

	/* out_pulse_width */
	do
	{
		cmd = get_sub_cmd("out_pulse_width", "0");
		SW_RTN_ON_NULL_PARAM(cmd);

		if (!strncasecmp(cmd, "quit", 4))
		{
			return SW_BAD_VALUE;
		}
		else if (!strncasecmp(cmd, "help", 4))
		{
			dprintf("usage: output pulse width\n");
			rv = SW_BAD_VALUE;
		}
		else
		{
			rv = cmd_data_check_uint32(cmd, &(pEntry->out_pulse_width),
					sizeof(a_uint32_t));
			if (SW_OK != rv)
				dprintf("usage: output pulse width\n");
		}
	}while (talk_mode && (SW_OK != rv));

	return SW_OK;
}

void
cmd_data_print_ptp_pps_sig_ctrl(a_uint8_t * param_name, a_uint32_t * buf, a_uint32_t size)
{
	fal_ptp_pps_signal_control_t *entry;

	entry = (fal_ptp_pps_signal_control_t *) buf;
	dprintf("[negative_in_latency]:%s\n", entry->negative_in_latency ? "YES" : "NO");
	dprintf("[in_latency]:0x%x\n", entry->in_latency);
	dprintf("[out_phase]:0x%x\n", entry->out_phase);
	dprintf("[out_pulse_width]:0x%x\n", entry->out_pulse_width);
	dprintf("\n");

}

sw_error_t
cmd_data_check_ptp_asym_correction(char *info, void *val, a_uint32_t size)
{
	char *cmd;
	sw_error_t rv;
	fal_ptp_asym_correction_t *pEntry = (fal_ptp_asym_correction_t *)val;

	memset(pEntry, 0, sizeof(fal_ptp_asym_correction_t));

	/* eg_asym_en */
	do
	{
		cmd = get_sub_cmd("eg_asym_en", "yes");
		SW_RTN_ON_NULL_PARAM(cmd);

		if (!strncasecmp(cmd, "quit", 4))
		{
			return SW_BAD_VALUE;
		}
		else if (!strncasecmp(cmd, "help", 4))
		{
			dprintf("usage: <yes/no/y/n>\n");
			rv = SW_BAD_VALUE;
		}
		else
		{
			rv = cmd_data_check_confirm(cmd, A_FALSE, &(pEntry->eg_asym_en),
					sizeof (a_bool_t));
			if (SW_OK != rv)
				dprintf("usage: <yes/no/y/n>\n");
		}
	}
	while (talk_mode && (SW_OK != rv));

	/* in_asym_en */
	do
	{
		cmd = get_sub_cmd("in_asym_en", "yes");
		SW_RTN_ON_NULL_PARAM(cmd);

		if (!strncasecmp(cmd, "quit", 4))
		{
			return SW_BAD_VALUE;
		}
		else if (!strncasecmp(cmd, "help", 4))
		{
			dprintf("usage: <yes/no/y/n>\n");
			rv = SW_BAD_VALUE;
		}
		else
		{
			rv = cmd_data_check_confirm(cmd, A_FALSE, &(pEntry->in_asym_en),
					sizeof (a_bool_t));
			if (SW_OK != rv)
				dprintf("usage: <yes/no/y/n>\n");
		}
	}
	while (talk_mode && (SW_OK != rv));

	/* eg_asym_value */
	do
	{
		cmd = get_sub_cmd("eg_asym_value", "0");
		SW_RTN_ON_NULL_PARAM(cmd);

		if (!strncasecmp(cmd, "quit", 4))
		{
			return SW_BAD_VALUE;
		}
		else if (!strncasecmp(cmd, "help", 4))
		{
			dprintf("usage: egress asymmetry value\n");
			rv = SW_BAD_VALUE;
		}
		else
		{
			rv = cmd_data_check_uint32(cmd, &(pEntry->eg_asym_value),
					sizeof(a_uint32_t));
			if (SW_OK != rv)
				dprintf("usage: egress asymmetry value\n");
		}
	}while (talk_mode && (SW_OK != rv));

	/* in_asym_value */
	do
	{
		cmd = get_sub_cmd("in_asym_value", "0");
		SW_RTN_ON_NULL_PARAM(cmd);

		if (!strncasecmp(cmd, "quit", 4))
		{
			return SW_BAD_VALUE;
		}
		else if (!strncasecmp(cmd, "help", 4))
		{
			dprintf("usage: ingress asymmetry value\n");
			rv = SW_BAD_VALUE;
		}
		else
		{
			rv = cmd_data_check_uint32(cmd, &(pEntry->in_asym_value),
					sizeof(a_uint32_t));
			if (SW_OK != rv)
				dprintf("usage: ingress asymmetry value\n");
		}
	}while (talk_mode && (SW_OK != rv));

	return SW_OK;
}

void
cmd_data_print_ptp_asym_correction(a_uint8_t * param_name, a_uint32_t * buf, a_uint32_t size)
{
	fal_ptp_asym_correction_t *entry;

	entry = (fal_ptp_asym_correction_t *) buf;
	dprintf("[eg_asym_en]:%s\n", entry->eg_asym_en ? "YES" : "NO");
	dprintf("[in_asym_en]:%s\n", entry->in_asym_en ? "YES" : "NO");
	dprintf("[eg_asym_value]:0x%x\n", entry->eg_asym_value);
	dprintf("[in_asym_value]:0x%x\n", entry->in_asym_value);
	dprintf("\n");

}

sw_error_t
cmd_data_check_ptp_waveform(char *info, void *val, a_uint32_t size)
{
	char *cmd;
	sw_error_t rv;
	fal_ptp_output_waveform_t *pEntry = (fal_ptp_output_waveform_t *)val;

	memset(pEntry, 0, sizeof(fal_ptp_output_waveform_t));

	/* waveform_type */
	do
	{
		cmd = get_sub_cmd("waveform_type", "0");
		SW_RTN_ON_NULL_PARAM(cmd);

		if (!strncasecmp(cmd, "quit", 4))
		{
			return SW_BAD_VALUE;
		}
		else if (!strncasecmp(cmd, "help", 4))
		{
			dprintf("usage: 0 for waveform frequency, 1 for pulse 10ms, "
					"2 for trigger0 and 3 for ptp rx state\n");
			rv = SW_BAD_VALUE;
		}
		else
		{
			rv = cmd_data_check_uint8(cmd, &(pEntry->waveform_type), sizeof(a_uint8_t));
			if (SW_OK != rv)
				dprintf("usage: 0 for waveform frequency, 1 for pulse 10ms, "
						"2 for trigger0 and 3 for ptp rx state\n");
		}
	}while (talk_mode && (SW_OK != rv));

	/* wave_align_pps_out_en */
	do
	{
		cmd = get_sub_cmd("wave_align_pps_out_en", "yes");
		SW_RTN_ON_NULL_PARAM(cmd);

		if (!strncasecmp(cmd, "quit", 4))
		{
			return SW_BAD_VALUE;
		}
		else if (!strncasecmp(cmd, "help", 4))
		{
			dprintf("usage: <yes/no/y/n>\n");
			rv = SW_BAD_VALUE;
		}
		else
		{
			rv = cmd_data_check_confirm(cmd, A_FALSE, &(pEntry->wave_align_pps_out_en),
					sizeof (a_bool_t));
			if (SW_OK != rv)
				dprintf("usage: <yes/no/y/n>\n");
		}
	}
	while (talk_mode && (SW_OK != rv));

	/* wave_period */
	do
	{
		cmd = get_sub_cmd("wave_period", "0");
		SW_RTN_ON_NULL_PARAM(cmd);

		if (!strncasecmp(cmd, "quit", 4))
		{
			return SW_BAD_VALUE;
		}
		else if (!strncasecmp(cmd, "help", 4))
		{
			dprintf("usage: waveform frequency period\n");
			rv = SW_BAD_VALUE;
		}
		else
		{
			rv = cmd_data_check_uint64(cmd, &(pEntry->wave_period), sizeof(a_uint64_t));
			if (SW_OK != rv)
				dprintf("usage: waveform frequency period\n");
		}
	}while (talk_mode && (SW_OK != rv));

	return SW_OK;
}

void
cmd_data_print_ptp_waveform(a_uint8_t * param_name, a_uint32_t * buf, a_uint32_t size)
{
	fal_ptp_output_waveform_t *entry;

	entry = (fal_ptp_output_waveform_t *) buf;
	dprintf("[waveform_type]:%d\n", entry->waveform_type);
	dprintf("[wave_align_pps_out_en]:%s\n", entry->wave_align_pps_out_en ? "YES" : "NO");
	dprintf("[wave_period]:0x%llx\n", entry->wave_period);
	dprintf("\n");

}

sw_error_t
cmd_data_check_ptp_tod_uart(char *info, void *val, a_uint32_t size)
{
	char *cmd;
        a_uint32_t tmp;
	sw_error_t rv;
	fal_ptp_tod_uart_t *pEntry = (fal_ptp_tod_uart_t *)val;

	memset(pEntry, 0, sizeof(fal_ptp_tod_uart_t));

	/* baud_config */
	do
	{
		cmd = get_sub_cmd("baud_config", "0");
		SW_RTN_ON_NULL_PARAM(cmd);

		if (!strncasecmp(cmd, "quit", 4))
		{
			return SW_BAD_VALUE;
		}
		else if (!strncasecmp(cmd, "help", 4))
		{
			dprintf("usage: baud config value \n");
			rv = SW_BAD_VALUE;
		}
		else
		{
			rv = cmd_data_check_uint16(cmd, &tmp, sizeof(a_uint32_t));
			if (SW_OK != rv)
			{
				dprintf("usage: baud config value \n");
			}
			else
			{
				pEntry->baud_config = tmp;
            }
		}
	}while (talk_mode && (SW_OK != rv));

	/* uart_config_bmp */
	do
	{
		cmd = get_sub_cmd("uart_config_bmp", "0");
		SW_RTN_ON_NULL_PARAM(cmd);

		if (!strncasecmp(cmd, "quit", 4))
		{
			return SW_BAD_VALUE;
		}
		else if (!strncasecmp(cmd, "help", 4))
		{
			dprintf("usage: UART config bitmap \n");
			rv = SW_BAD_VALUE;
		}
		else
		{
			rv = cmd_data_check_uint32(cmd, &(pEntry->uart_config_bmp),
					sizeof(a_uint32_t));
			if (SW_OK != rv)
				dprintf("usage: UART config bitmap \n");
		}
	}while (talk_mode && (SW_OK != rv));

	/* reset_buf_en */
	do
	{
		cmd = get_sub_cmd("reset_buf_en", "yes");
		SW_RTN_ON_NULL_PARAM(cmd);

		if (!strncasecmp(cmd, "quit", 4))
		{
			return SW_BAD_VALUE;
		}
		else if (!strncasecmp(cmd, "help", 4))
		{
			dprintf("usage: <yes/no/y/n>\n");
			rv = SW_BAD_VALUE;
		}
		else
		{
			rv = cmd_data_check_confirm(cmd, A_FALSE, &(pEntry->reset_buf_en),
					sizeof (a_bool_t));
			if (SW_OK != rv)
				dprintf("usage: <yes/no/y/n>\n");
		}
	}
	while (talk_mode && (SW_OK != rv));

	/* tx_buf_value */
	do
	{
		cmd = get_sub_cmd("tx_buf_value", "0");
		SW_RTN_ON_NULL_PARAM(cmd);

		if (!strncasecmp(cmd, "quit", 4))
		{
			return SW_BAD_VALUE;
		}
		else if (!strncasecmp(cmd, "help", 4))
		{
			dprintf("usage: tx buffer value \n");
			rv = SW_BAD_VALUE;
		}
		else
		{
			rv = cmd_data_check_uint16(cmd, &tmp,
					sizeof(a_uint32_t));
			if (SW_OK != rv)
			{
				dprintf("usage: tx buffer value \n");
			}
			else
			{
				pEntry->tx_buf_value = tmp;
			}
		}
	}while (talk_mode && (SW_OK != rv));

	return SW_OK;
}

void
cmd_data_print_ptp_tod_uart(a_uint8_t * param_name, a_uint32_t * buf, a_uint32_t size)
{
	fal_ptp_tod_uart_t *entry;

	entry = (fal_ptp_tod_uart_t *) buf;
	dprintf("[baud_config]:0x%x\n", entry->baud_config);
	dprintf("[uart_config_bmp]:0x%x\n", entry->uart_config_bmp);
	dprintf("[reset_buf_en]:%s\n", entry->reset_buf_en ? "YES" : "NO");
	dprintf("[buf_status_bmp]:0x%x\n", entry->buf_status_bmp);
	dprintf("[tx_buf_value]:0x%x\n", entry->tx_buf_value);
	dprintf("[rx_buf_value]:0x%x\n", entry->rx_buf_value);
	dprintf("\n");

}

sw_error_t
cmd_data_check_ptp_enhanced_timestamp_engine(char *info, void *val, a_uint32_t size)
{
	char *cmd;
	sw_error_t rv;
	fal_ptp_enhanced_ts_engine_t *pEntry = (fal_ptp_enhanced_ts_engine_t *)val;

	memset(pEntry, 0, sizeof(fal_ptp_enhanced_ts_engine_t));

	/* filt_en */
	do
	{
		cmd = get_sub_cmd("filt_en", "yes");
		SW_RTN_ON_NULL_PARAM(cmd);

		if (!strncasecmp(cmd, "quit", 4))
		{
			return SW_BAD_VALUE;
		}
		else if (!strncasecmp(cmd, "help", 4))
		{
			dprintf("usage: <yes/no/y/n>\n");
			rv = SW_BAD_VALUE;
		}
		else
		{
			rv = cmd_data_check_confirm(cmd, A_FALSE, &(pEntry->filt_en),
					sizeof (a_bool_t));
			if (SW_OK != rv)
				dprintf("usage: <yes/no/y/n>\n");
		}
	}
	while (talk_mode && (SW_OK != rv));

	/* enhance_ts_conf_bmp */
	do
	{
		cmd = get_sub_cmd("enhance_ts_conf_bmp", "0");
		SW_RTN_ON_NULL_PARAM(cmd);

		if (!strncasecmp(cmd, "quit", 4))
		{
			return SW_BAD_VALUE;
		}
		else if (!strncasecmp(cmd, "help", 4))
		{
			dprintf("usage: enhanced timestamp config bitmap \n");
			rv = SW_BAD_VALUE;
		}
		else
		{
			rv = cmd_data_check_uint16(cmd, &(pEntry->enhance_ts_conf_bmp),
					sizeof(a_uint16_t));
			if (SW_OK != rv)
				dprintf("usage: enhanced timestamp config bitmap \n");
		}
	}while (talk_mode && (SW_OK != rv));

	/* eth_type */
	do
	{
		cmd = get_sub_cmd("eth_type", "0");
		SW_RTN_ON_NULL_PARAM(cmd);

		if (!strncasecmp(cmd, "quit", 4))
		{
			return SW_BAD_VALUE;
		}
		else if (!strncasecmp(cmd, "help", 4))
		{
			dprintf("usage: ethernet type \n");
			rv = SW_BAD_VALUE;
		}
		else
		{
			rv = cmd_data_check_uint32(cmd, &(pEntry->eth_type), sizeof(a_uint32_t));
			if (SW_OK != rv)
				dprintf("usage: ethernet type \n");
		}
	}while (talk_mode && (SW_OK != rv));

	/* dmac_addr */
	do
	{
		cmd = get_sub_cmd("dmac_addr", NULL);
		SW_RTN_ON_NULL_PARAM(cmd);

		if (!strncasecmp(cmd, "quit", 4))
		{
			return SW_BAD_VALUE;
		}
		else if (!strncasecmp(cmd, "help", 4))
		{
			dprintf("usage: the format is xx-xx-xx-xx-xx-xx \n");
			rv = SW_BAD_VALUE;
		}
		else
		{
			rv = cmd_data_check_macaddr(cmd, &(pEntry->dmac_addr),
					sizeof (fal_mac_addr_t));
			if (SW_OK != rv)
				dprintf("usage: the format is xx-xx-xx-xx-xx-xx \n");
		}
	}
	while (talk_mode && (SW_OK != rv));

	/* ipv4_l4_proto */
	do
	{
		cmd = get_sub_cmd("ipv4_l4_proto", "0");
		SW_RTN_ON_NULL_PARAM(cmd);

		if (!strncasecmp(cmd, "quit", 4))
		{
			return SW_BAD_VALUE;
		}
		else if (!strncasecmp(cmd, "help", 4))
		{
			dprintf("usage: ipv4 l4 protocol, 32bits\n");
			rv = SW_BAD_VALUE;
		}
		else
		{
			rv = cmd_data_check_uint32(cmd, &(pEntry->ipv4_l4_proto),
					sizeof(a_uint32_t));
			if (SW_OK != rv)
				dprintf("usage: ipv4 l4 protocol, 32bits\n");
		}
	}while (talk_mode && (SW_OK != rv));

	cmd_data_check_element("ipv4_dip", "0.0.0.0",
			"usage: the format is xx.xx.xx.xx \n",
			cmd_data_check_ip4addr, (cmd, &(pEntry->ipv4_dip), 4));
	cmd_data_check_element("ipv6_dip", NULL,
			"usage: the format is xxxx::xxxx \n",
			cmd_data_check_ip6addr, (cmd, &(pEntry->ipv6_dip), 16));

	/* udp_dport */
	do
	{
		cmd = get_sub_cmd("udp_dport", "0");
		SW_RTN_ON_NULL_PARAM(cmd);

		if (!strncasecmp(cmd, "quit", 4))
		{
			return SW_BAD_VALUE;
		}
		else if (!strncasecmp(cmd, "help", 4))
		{
			dprintf("usage: udp dest port\n");
			rv = SW_BAD_VALUE;
		}
		else
		{
			rv = cmd_data_check_uint32(cmd, &(pEntry->udp_dport), sizeof(a_uint32_t));
			if (SW_OK != rv)
				dprintf("usage: udp dest port\n");
		}
	}while (talk_mode && (SW_OK != rv));

	/* y1731_mac_addr */
	do
	{
		cmd = get_sub_cmd("y1731_mac_addr", NULL);
		SW_RTN_ON_NULL_PARAM(cmd);

		if (!strncasecmp(cmd, "quit", 4))
		{
			return SW_BAD_VALUE;
		}
		else if (!strncasecmp(cmd, "help", 4))
		{
			dprintf("usage: the format is xx-xx-xx-xx-xx-xx \n");
			rv = SW_BAD_VALUE;
		}
		else
		{
			rv = cmd_data_check_macaddr(cmd, &(pEntry->y1731_mac_addr),
					sizeof (fal_mac_addr_t));
			if (SW_OK != rv)
				dprintf("usage: the format is xx-xx-xx-xx-xx-xx \n");
		}
	}
	while (talk_mode && (SW_OK != rv));

	return SW_OK;
}

void
cmd_data_print_ptp_enhanced_timestamp_engine(a_uint8_t * param_name,
		a_uint32_t * buf, a_uint32_t size)
{
	fal_ptp_enhanced_ts_engine_t *entry;
	entry = (fal_ptp_enhanced_ts_engine_t *) buf;

	dprintf("[filt_en]:%s\n", entry->filt_en ? "YES" : "NO");

	dprintf("[enhance_ts_conf_bmp]:0x%x\n", entry->enhance_ts_conf_bmp);
	dprintf("[eth_type]:0x%x\n", entry->eth_type);
	cmd_data_print_macaddr("[dmac_addr]:", (a_uint32_t *) & (entry->dmac_addr),
			sizeof (fal_mac_addr_t));
	dprintf("\n");
	dprintf("[ipv4_l4_proto]:0x%x\n", entry->ipv4_l4_proto);
	cmd_data_print_ip4addr("[ipv4_dip]:",
			(a_uint32_t *) & (entry->ipv4_dip),
			sizeof (fal_ip4_addr_t));
	cmd_data_print_ip6addr("\n[ipv6_dip]:",
			(a_uint32_t *) & (entry->ipv6_dip),
			sizeof (fal_ip6_addr_t));
	dprintf("\n");
	dprintf("[udp_dport]:0x%x\n", entry->udp_dport);
	cmd_data_print_macaddr("[y1731_mac_addr]:", (a_uint32_t *) & (entry->y1731_mac_addr),
			sizeof (fal_mac_addr_t));
	dprintf("\n");
	dprintf("[enhance_ts_status_bmp]:0x%x\n", entry->enhance_ts_status_bmp);
	dprintf("[enhance_ts_status_pre_bmp]:0x%x\n", entry->enhance_ts_status_pre_bmp);
	dprintf("[y1731_identity]:0x%x\n", entry->y1731_identity);
	dprintf("[y1731_identity_pre]:0x%x\n", entry->y1731_identity_pre);
	dprintf("timestamp:\n");
        dprintf("[seconds]:0x%llx\n", entry->timestamp.seconds);
        dprintf("[nanoseconds]:0x%x\n", entry->timestamp.nanoseconds);
        dprintf("[fracnanoseconds]:0x%x\n", entry->timestamp.fracnanoseconds);
	dprintf("timestamp pre:\n");
        dprintf("[seconds]:0x%llx\n", entry->timestamp_pre.seconds);
        dprintf("[nanoseconds]:0x%x\n", entry->timestamp_pre.nanoseconds);
        dprintf("[fracnanoseconds]:0x%x\n", entry->timestamp_pre.fracnanoseconds);
	printf("\n");

}

sw_error_t
cmd_data_check_ptp_trigger(char *info, void *val, a_uint32_t size)
{
	char *cmd;
	sw_error_t rv;
	fal_ptp_trigger_t *pEntry = (fal_ptp_trigger_t *)val;

	memset(pEntry, 0, sizeof(fal_ptp_trigger_t));

	/* trigger_en */
	do
	{
		cmd = get_sub_cmd("trigger_en", "yes");
		SW_RTN_ON_NULL_PARAM(cmd);

		if (!strncasecmp(cmd, "quit", 4))
		{
			return SW_BAD_VALUE;
		}
		else if (!strncasecmp(cmd, "help", 4))
		{
			dprintf("usage: <yes/no/y/n>\n");
			rv = SW_BAD_VALUE;
		}
		else
		{
			rv = cmd_data_check_confirm(cmd, A_FALSE,
					&(pEntry->trigger_conf.trigger_en),
					sizeof (a_bool_t));
			if (SW_OK != rv)
				dprintf("usage: <yes/no/y/n>\n");
		}
	}
	while (talk_mode && (SW_OK != rv));

	/* output_force_en */
	do
	{
		cmd = get_sub_cmd("output_force_en", "yes");
		SW_RTN_ON_NULL_PARAM(cmd);

		if (!strncasecmp(cmd, "quit", 4))
		{
			return SW_BAD_VALUE;
		}
		else if (!strncasecmp(cmd, "help", 4))
		{
			dprintf("usage: <yes/no/y/n>\n");
			rv = SW_BAD_VALUE;
		}
		else
		{
			rv = cmd_data_check_confirm(cmd, A_FALSE,
					&(pEntry->trigger_conf.output_force_en),
					sizeof (a_bool_t));
			if (SW_OK != rv)
				dprintf("usage: <yes/no/y/n>\n");
		}
	}
	while (talk_mode && (SW_OK != rv));

	/* output_force_value */
	do
	{
		cmd = get_sub_cmd("output_force_value", "0");
		SW_RTN_ON_NULL_PARAM(cmd);

		if (!strncasecmp(cmd, "quit", 4))
		{
			return SW_BAD_VALUE;
		}
		else if (!strncasecmp(cmd, "help", 4))
		{
			dprintf("usage: output force value \n");
			rv = SW_BAD_VALUE;
		}
		else
		{
			rv = cmd_data_check_uint32(cmd,
					(a_uint32_t *) (&(pEntry->trigger_conf.output_force_value)),
					sizeof(a_uint32_t));
			if (SW_OK != rv)
				dprintf("usage: output force value \n");
		}
	}while (talk_mode && (SW_OK != rv));

	/* patten_select */
	do
	{
		cmd = get_sub_cmd("patten_select", "0");
		SW_RTN_ON_NULL_PARAM(cmd);

		if (!strncasecmp(cmd, "quit", 4))
		{
			return SW_BAD_VALUE;
		}
		else if (!strncasecmp(cmd, "help", 4))
		{
			dprintf("usage: patten select \n");
			rv = SW_BAD_VALUE;
		}
		else
		{
			rv = cmd_data_check_uint32(cmd,
					(a_uint32_t *) (&(pEntry->trigger_conf.patten_select)),
					sizeof(a_uint32_t));
			if (SW_OK != rv)
				dprintf("usage: patten select \n");
		}
	}while (talk_mode && (SW_OK != rv));

	/* late_operation */
	do
	{
		cmd = get_sub_cmd("late_operation", "0");
		SW_RTN_ON_NULL_PARAM(cmd);

		if (!strncasecmp(cmd, "quit", 4))
		{
			return SW_BAD_VALUE;
		}
		else if (!strncasecmp(cmd, "help", 4))
		{
			dprintf("usage: late operation\n");
			rv = SW_BAD_VALUE;
		}
		else
		{
			rv = cmd_data_check_uint32(cmd,
					(a_uint32_t *) (&(pEntry->trigger_conf.late_operation)),
					sizeof(a_uint32_t));
			if (SW_OK != rv)
				dprintf("usage: late operation\n");
		}
	}while (talk_mode && (SW_OK != rv));

	/* notify */
	do
	{
		cmd = get_sub_cmd("notify", "0");
		SW_RTN_ON_NULL_PARAM(cmd);

		if (!strncasecmp(cmd, "quit", 4))
		{
			return SW_BAD_VALUE;
		}
		else if (!strncasecmp(cmd, "help", 4))
		{
			dprintf("usage: notify\n");
			rv = SW_BAD_VALUE;
		}
		else
		{
			rv = cmd_data_check_uint32(cmd,
					(a_uint32_t *) (&(pEntry->trigger_conf.notify)),
					sizeof(a_uint32_t));
			if (SW_OK != rv)
				dprintf("usage: notify\n");
		}
	}while (talk_mode && (SW_OK != rv));

	/* trigger_effect */
	do
	{
		cmd = get_sub_cmd("trigger_effect", "0");
		SW_RTN_ON_NULL_PARAM(cmd);

		if (!strncasecmp(cmd, "quit", 4))
		{
			return SW_BAD_VALUE;
		}
		else if (!strncasecmp(cmd, "help", 4))
		{
			dprintf("usage: trigger effect\n");
			rv = SW_BAD_VALUE;
		}
		else
		{
			rv = cmd_data_check_uint32(cmd,
					(a_uint32_t *) (&(pEntry->trigger_conf.trigger_effect)),
					sizeof(a_uint32_t));
			if (SW_OK != rv)
				dprintf("usage: trigger effect\n");
		}
	}while (talk_mode && (SW_OK != rv));

	/* seconds */
	do
	{
		cmd = get_sub_cmd("seconds", "0");
		SW_RTN_ON_NULL_PARAM(cmd);

		if (!strncasecmp(cmd, "quit", 4))
		{
			return SW_BAD_VALUE;
		}
		else if (!strncasecmp(cmd, "help", 4))
		{
			dprintf("usage: seconds 48bits \n");
			rv = SW_BAD_VALUE;
		}
		else
		{
			rv = cmd_data_check_uint64(cmd,
					(a_uint64_t *) (&(pEntry->trigger_conf.tim.seconds)),
					sizeof(a_uint64_t));
			if (SW_OK != rv)
				dprintf("usage: seconds 48bits \n");
		}
	}while (talk_mode && (SW_OK != rv));

	/* nanoseconds */
	do
	{
		cmd = get_sub_cmd("nanoseconds", "0");
		SW_RTN_ON_NULL_PARAM(cmd);

		if (!strncasecmp(cmd, "quit", 4))
		{
			return SW_BAD_VALUE;
		}
		else if (!strncasecmp(cmd, "help", 4))
		{
			dprintf("usage: nanoseconds 32bits\n");
			rv = SW_BAD_VALUE;
		}
		else
		{
			rv = cmd_data_check_uint32(cmd,
					(a_uint32_t *) (&(pEntry->trigger_conf.tim.nanoseconds)),
					sizeof(a_uint32_t));
			if (SW_OK != rv)
				dprintf("usage: nanoseconds 32bits\n");
		}
	}while (talk_mode && (SW_OK != rv));

	/* fracnanoseconds */
	do
	{
		cmd = get_sub_cmd("fracnanoseconds", "0");
		SW_RTN_ON_NULL_PARAM(cmd);

		if (!strncasecmp(cmd, "quit", 4))
		{
			return SW_BAD_VALUE;
		}
		else if (!strncasecmp(cmd, "help", 4))
		{
			dprintf("usage: fracnanoseconds 32bits \n");
			rv = SW_BAD_VALUE;
		}
		else
		{
			rv = cmd_data_check_uint32(cmd,
				(a_uint32_t *) (&(pEntry->trigger_conf.tim.fracnanoseconds)),
				sizeof(a_uint32_t));
			if (SW_OK != rv)
				dprintf("usage: fracnanoseconds 32bits\n");
		}
	}while (talk_mode && (SW_OK != rv));

	return SW_OK;
}

void
cmd_data_print_ptp_trigger(a_uint8_t * param_name, a_uint32_t * buf, a_uint32_t size)
{
	fal_ptp_trigger_t *entry;
	entry = (fal_ptp_trigger_t *) buf;

	dprintf("[trigger_en]:%s\n", entry->trigger_conf.trigger_en ? "YES" : "NO");
	dprintf("[output_force_en]:%s\n", entry->trigger_conf.output_force_en ? "YES" : "NO");
	dprintf("[output_force_value]:0x%x\n", entry->trigger_conf.output_force_value);
	dprintf("[patten_select]:0x%x\n", entry->trigger_conf.patten_select);
	dprintf("[late_operation]:0x%x\n", entry->trigger_conf.late_operation);
	dprintf("[notify]:0x%x\n", entry->trigger_conf.notify);
	dprintf("[trigger_effect]:0x%x\n", entry->trigger_conf.trigger_effect);
	dprintf("[trigger_finished]:0x%x\n", entry->trigger_status.trigger_finished);
	dprintf("[trigger_active]:0x%x\n", entry->trigger_status.trigger_active);
	dprintf("[trigger_error]:0x%x\n", entry->trigger_status.trigger_error);
	cmd_data_print_ptp_time("[time]:",
			(a_uint32_t *) & (entry->trigger_conf.tim),
			sizeof (fal_ptp_time_t));

}

sw_error_t
cmd_data_check_ptp_capture(char *info, void *val, a_uint32_t size)
{
	char *cmd;
	sw_error_t rv;
	fal_ptp_capture_t *pEntry = (fal_ptp_capture_t *)val;

	memset(pEntry, 0, sizeof(fal_ptp_capture_t));

	/* status_clear */
	do
	{
		cmd = get_sub_cmd("status_clear", "0");
		SW_RTN_ON_NULL_PARAM(cmd);

		if (!strncasecmp(cmd, "quit", 4))
		{
			return SW_BAD_VALUE;
		}
		else if (!strncasecmp(cmd, "help", 4))
		{
			dprintf("usage: status clear \n");
			rv = SW_BAD_VALUE;
		}
		else
		{
			rv = cmd_data_check_uint32(cmd,
					(a_uint32_t *) (&(pEntry->capture_conf.status_clear)),
					sizeof(a_uint32_t));
			if (SW_OK != rv)
				dprintf("usage: status clear \n");
		}
	}while (talk_mode && (SW_OK != rv));

	/* notify_event */
	do
	{
		cmd = get_sub_cmd("notify_event", "0");
		SW_RTN_ON_NULL_PARAM(cmd);

		if (!strncasecmp(cmd, "quit", 4))
		{
			return SW_BAD_VALUE;
		}
		else if (!strncasecmp(cmd, "help", 4))
		{
			dprintf("usage: notify event \n");
			rv = SW_BAD_VALUE;
		}
		else
		{
			rv = cmd_data_check_uint32(cmd,
					(a_uint32_t *) (&(pEntry->capture_conf.notify_event)),
					sizeof(a_uint32_t));
			if (SW_OK != rv)
				dprintf("usage: notify event \n");
		}
	}while (talk_mode && (SW_OK != rv));

	/* single_multi_select */
	do
	{
		cmd = get_sub_cmd("single_multi_select", "0");
		SW_RTN_ON_NULL_PARAM(cmd);

		if (!strncasecmp(cmd, "quit", 4))
		{
			return SW_BAD_VALUE;
		}
		else if (!strncasecmp(cmd, "help", 4))
		{
			dprintf("usage: single multiple select\n");
			rv = SW_BAD_VALUE;
		}
		else
		{
			rv = cmd_data_check_uint32(cmd,
				(a_uint32_t *) (&(pEntry->capture_conf.single_multi_select)),
				sizeof(a_uint32_t));
			if (SW_OK != rv)
				dprintf("usage: single multiple select\n");
		}
	}while (talk_mode && (SW_OK != rv));

	/* fall_edge_en */
	do
	{
		cmd = get_sub_cmd("fall_edge_en", "yes");
		SW_RTN_ON_NULL_PARAM(cmd);

		if (!strncasecmp(cmd, "quit", 4))
		{
			return SW_BAD_VALUE;
		}
		else if (!strncasecmp(cmd, "help", 4))
		{
			dprintf("usage: <yes/no/y/n>\n");
			rv = SW_BAD_VALUE;
		}
		else
		{
			rv = cmd_data_check_confirm(cmd, A_FALSE,
					&(pEntry->capture_conf.fall_edge_en),
					sizeof (a_bool_t));
			if (SW_OK != rv)
				dprintf("usage: <yes/no/y/n>\n");
		}
	}
	while (talk_mode && (SW_OK != rv));

	/* rise_edge_en */
	do
	{
		cmd = get_sub_cmd("rise_edge_en", "yes");
		SW_RTN_ON_NULL_PARAM(cmd);

		if (!strncasecmp(cmd, "quit", 4))
		{
			return SW_BAD_VALUE;
		}
		else if (!strncasecmp(cmd, "help", 4))
		{
			dprintf("usage: <yes/no/y/n>\n");
			rv = SW_BAD_VALUE;
		}
		else
		{
			rv = cmd_data_check_confirm(cmd, A_FALSE,
					&(pEntry->capture_conf.rise_edge_en),
					sizeof (a_bool_t));
			if (SW_OK != rv)
				dprintf("usage: <yes/no/y/n>\n");
		}
	}
	while (talk_mode && (SW_OK != rv));

	return SW_OK;
}

void
cmd_data_print_ptp_capture(a_uint8_t * param_name, a_uint32_t * buf, a_uint32_t size)
{
	fal_ptp_capture_t *entry;
	entry = (fal_ptp_capture_t *) buf;

	dprintf("[status_clear]:0x%x\n", entry->capture_conf.status_clear);
	dprintf("[notify_event]:0x%x\n", entry->capture_conf.notify_event);
	dprintf("[single_multi_select]:0x%x\n", entry->capture_conf.single_multi_select);
	dprintf("[fall_edge_en]:%s\n", entry->capture_conf.fall_edge_en ? "YES" : "NO");
	dprintf("[rise_edge_en]:%s\n", entry->capture_conf.rise_edge_en ? "YES" : "NO");

	dprintf("[event_detected]:0x%x\n", entry->capture_status.event_detected);
	dprintf("[fall_rise_edge_detected]:0x%x\n", entry->capture_status.fall_rise_edge_detected);
	dprintf("[single_multi_detected]:0x%x\n", entry->capture_status.single_multi_detected);
	dprintf("[event_missed_cnt]:0x%x\n", entry->capture_status.event_missed_cnt);
	cmd_data_print_ptp_time("\n[time]:",
			(a_uint32_t *) & (entry->capture_status.tim),
			sizeof (fal_ptp_time_t));

}

sw_error_t
cmd_data_check_ptp_interrupt(char *info, void *val, a_uint32_t size)
{
	char *cmd;
	sw_error_t rv;
	fal_ptp_interrupt_t *pEntry = (fal_ptp_interrupt_t *)val;

	memset(pEntry, 0, sizeof(fal_ptp_interrupt_t));

	/* intr_mask */
	do
	{
		cmd = get_sub_cmd("intr_mask", "0");
		SW_RTN_ON_NULL_PARAM(cmd);

		if (!strncasecmp(cmd, "quit", 4))
		{
			return SW_BAD_VALUE;
		}
		else if (!strncasecmp(cmd, "help", 4))
		{
			dprintf("usage: interrupt mask \n");
			rv = SW_BAD_VALUE;
		}
		else
		{
			rv = cmd_data_check_uint32(cmd, &(pEntry->intr_mask), sizeof(a_uint32_t));
			if (SW_OK != rv)
				dprintf("usage: interrupt mask \n");
		}
	}while (talk_mode && (SW_OK != rv));

	return SW_OK;
}

void
cmd_data_print_ptp_interrupt(a_uint8_t * param_name, a_uint32_t * buf, a_uint32_t size)
{
	fal_ptp_interrupt_t *entry;
	entry = (fal_ptp_interrupt_t *) buf;

	dprintf("[intr_mask]:0x%x\n", entry->intr_mask);
	dprintf("[intr_status]:0x%x\n", entry->intr_status);

}

sw_error_t
cmd_data_check_source_filter_mode(char *cmd_str, a_uint32_t * arg_val, a_uint32_t size)
{
	if (cmd_str == NULL)
	{
		return SW_BAD_PARAM;
	}

	if (!strncasecmp(cmd_str, "virtual_port", 15))
	{
		*arg_val = FAL_SRC_FILTER_MODE_VP;
	}
	else if (!strncasecmp(cmd_str, "physical_port", 15))
	{
		*arg_val = FAL_SRC_FILTER_MODE_PHYSICAL;
	}
	else
	{
	    return SW_BAD_VALUE;
	}

	return SW_OK;
}

sw_error_t
cmd_data_check_src_filter_config(char *cmd_str, a_uint32_t *arg_val, a_uint32_t size)
{
	char *cmd;
	sw_error_t rv;
	fal_src_filter_config_t src_filter_config;

	aos_mem_zero(&src_filter_config, sizeof (fal_src_filter_config_t));

	do
	{
	    cmd = get_sub_cmd("srcfilter_enable", "enable");
		SW_RTN_ON_NULL_PARAM(cmd);

	    if (!strncasecmp(cmd, "quit", 4))
	    {
	        return SW_BAD_VALUE;
	    }
	    else if (!strncasecmp(cmd, "help", 4))
	    {
	        dprintf("usage: enable/disable \n");
	        rv = SW_BAD_VALUE;
	    }
	    else
	    {
	        rv = cmd_data_check_enable(cmd, &(src_filter_config.src_filter_enable),
					sizeof (a_bool_t));
	        if (SW_OK != rv)
	            dprintf("usage: enable/disable \n");
	    }
	}
	while (talk_mode && (SW_OK != rv));

	do
	{
	    cmd = get_sub_cmd("srcfilter_mode", "virtual_port");
	    SW_RTN_ON_NULL_PARAM(cmd);

	    if (!strncasecmp(cmd, "quit", 4))
	    {
	        return SW_BAD_VALUE;
	    }
	    else if (!strncasecmp(cmd, "help", 4))
	    {
	        dprintf("usage: usage: virtual_port physical_port\n");
	        rv = SW_BAD_VALUE;
	    }
	    else
	    {
			cmd_data_check_source_filter_mode(cmd, &(src_filter_config.src_filter_mode),
				sizeof(a_uint32_t));
	    }
	}
	while (talk_mode && (SW_OK != rv));

	*(fal_src_filter_config_t *)arg_val = src_filter_config;

	return SW_OK;
}

void
cmd_data_print_src_filter_config(a_uint8_t * param_name, a_uint32_t * buf, a_uint32_t size)
{
	fal_src_filter_config_t *src_filter_config;
	src_filter_config = (fal_src_filter_config_t *) buf;

	dprintf("\n[%s] \n", param_name);

	if(src_filter_config->src_filter_enable)
	{
		dprintf("src_filter_enable:ENABLE\n");
	}
	else
	{
		dprintf("src_filter_enable:DISABLE\n");
	}
	if(src_filter_config->src_filter_mode == FAL_SRC_FILTER_MODE_VP)
	{
		dprintf("src_filter_mode:virtual_port mode\n");
	}
	else if(src_filter_config->src_filter_mode == FAL_SRC_FILTER_MODE_PHYSICAL)
	{
		dprintf("src_filter_mode:physical_port mode\n");
	}
	else
	{
		dprintf("src_filter_mode:unknown\n");
	}
}

sw_error_t
cmd_data_check_sfp_ccode_type(char *cmdstr, fal_sfp_cc_type_t *arg_val, a_uint32_t size)
{
	sw_error_t rv = SW_OK;

	if (cmdstr == NULL) {
		return SW_BAD_PARAM;
	} else if (0 == cmdstr[0]) {
		return SW_BAD_VALUE;
	}

	if (!strcasecmp(cmdstr, "base")) {
		*arg_val = FAL_SFP_CC_BASE;
	}
	else if (!strcasecmp(cmdstr, "ext")) {
		*arg_val = FAL_SFP_CC_EXT;
	}
	else if (!strcasecmp(cmdstr, "dmi")) {
		*arg_val = FAL_SFP_CC_DMI;
	}
	else {
		dprintf("usage: <base/ext/dmi>\n");
		rv = SW_BAD_VALUE;
	}

	return rv;
}

sw_error_t
cmd_data_check_sfp_data(char *cmd_str, void *arg_val, a_uint32_t size)
{
	fal_sfp_data_t entry;
	sw_error_t rv;
	char *cmd, cmd_byte[3];
	char fmt[3] = "%x";
	a_uint8_t bytes;

	aos_mem_zero(&entry, sizeof(fal_sfp_data_t));

	do {
		cmd = get_sub_cmd("addr", "0x50");
		SW_RTN_ON_NULL_PARAM(cmd);

		if (!strncasecmp(cmd, "quit", 4)) {
			return SW_BAD_VALUE;
		}
		else if (!strncasecmp(cmd, "help", 4)) {
			dprintf("usage: input valid i2c slave addr(0x0~0x7f) \n");
			rv = SW_BAD_VALUE;
		}
		else {
			rv = cmd_data_check_uint8(cmd, &(entry.addr), sizeof(a_uint8_t));
			if (SW_OK != rv) {
				rv = SW_BAD_VALUE;
			}
		}
	} while (talk_mode && (SW_OK != rv));

	do {
		cmd = get_sub_cmd("offset", "0x0");
		SW_RTN_ON_NULL_PARAM(cmd);

		if (!strncasecmp(cmd, "quit", 4)) {
			return SW_BAD_VALUE;
		}
		else if (!strncasecmp(cmd, "help", 4)) {
			dprintf("usage: 0x0~0xff \n");
			rv = SW_BAD_VALUE;
		}
		else {
			rv = cmd_data_check_uint8(cmd, &(entry.offset), sizeof(a_uint8_t));
			if (SW_OK != rv) {
				rv = SW_BAD_VALUE;
			}
		}
	} while (talk_mode && (SW_OK != rv));

	do {
		cmd = get_sub_cmd("count", "0x0");
		SW_RTN_ON_NULL_PARAM(cmd);

		if (!strncasecmp(cmd, "quit", 4)) {
			return SW_BAD_VALUE;
		}
		else if (!strncasecmp(cmd, "help", 4)) {
			dprintf("usage: 0x0~0xff \n");
			rv = SW_BAD_VALUE;
		}
		else {
			rv = cmd_data_check_uint8(cmd, &(entry.count), sizeof(a_uint8_t));
			if (SW_OK != rv) {
				rv = SW_BAD_VALUE;
			} else if (entry.offset + entry.count > 255) {
				dprintf("error: offset + count more than 255\n");
				rv = SW_BAD_VALUE;
			}
		}
	} while (talk_mode && (SW_OK != rv));

	do {
		cmd = get_sub_cmd("data", "null");
		SW_RTN_ON_NULL_PARAM(cmd);

		if (!strncmp(cmd, "null", strlen(cmd))) {
			/* the default value "null" of data is for getting operation */
			rv = SW_OK;
			break;
		}

		if (!strncasecmp(cmd, "quit", 4)) {
			return SW_BAD_VALUE;
		}
		else if (!strncasecmp(cmd, "help", 4)) {
			dprintf("input the matched hex data count: 0x%x \n", entry.count);
			rv = SW_BAD_VALUE;
		}
		else if (strspn(cmd, "1234567890abcdefABCDEFXx") != strlen(cmd) ||
				strlen(cmd) != (entry.count * 2 + 2)) {
			dprintf("the input data is invalid\n");
			rv = SW_BAD_VALUE;
		}
		else {
			if (cmd[0] == '0' && (cmd[1] == 'x' || cmd[1] == 'X')) {
				cmd += 2;
				for (bytes = 0; bytes < entry.count; bytes++) {
					if (strlen(cmd) == 0) {
						break;
					}
					/* copy 2 chars from cmd */
					strlcpy(cmd_byte, cmd, sizeof(cmd_byte));
					sscanf(cmd_byte, fmt, &(entry.data[bytes]));
					cmd += 2;
				}

				if (bytes < entry.count) {
					dprintf("the byte length of input data is less than %d\n",
							entry.count);
					rv = SW_BAD_VALUE;
				} else {
					rv = SW_OK;
				}
			} else {
				dprintf("need to input hex data\n");
				rv = SW_BAD_VALUE;
			}
		}
	} while (talk_mode && (SW_OK != rv));

	*(fal_sfp_data_t *)arg_val = entry;

	return SW_OK;
}

void
cmd_data_print_sfp_data(a_uint8_t *param_name, a_ulong_t *buf, a_uint32_t size)
{
	a_uint8_t data_cnt;
	fal_sfp_data_t *sfp_data = (fal_sfp_data_t *)buf;

	dprintf("\n[%s] \n", param_name);

	dprintf("eeprom addr: 0x%02x\n", sfp_data->addr);
	dprintf("eeprom data:");
	for (data_cnt = 0; data_cnt < sfp_data->count; data_cnt++) {
		if (!(data_cnt % 16)) {
			dprintf("\n%02x:", sfp_data->offset + data_cnt);
		}

		dprintf("%02x", sfp_data->data[data_cnt]);
	}

	dprintf("\n\n");
}

void
cmd_data_print_sfp_dev_type(a_uint8_t *param_name, a_ulong_t *buf, a_uint32_t size)
{
	fal_sfp_dev_type_t *entry;

	dprintf("\n[%s] \n", param_name);

	entry = (fal_sfp_dev_type_t *)buf;
	dprintf("[id]:0x%x [ext_id]:0x%x\n", entry->identifier, entry->ext_indentifier);
	dprintf("[connector_type]:0x%x\n", entry->connector_type);
	dprintf("\n");
}

void
cmd_data_print_sfp_transc_code(a_uint8_t *param_name, a_ulong_t *buf, a_uint32_t size)
{
	fal_sfp_transc_code_t *entry;

	dprintf("\n[%s] \n", param_name);

	entry = (fal_sfp_transc_code_t *)buf;
	dprintf("[eth_10g_ccode]:0x%x [infiniband_ccode]:0x%x [escon_ccode]:0x%x "
			"[sonet_ccode]:0x%x [eth_ccode]:0x%x [fibre_chan_link_length]:0x%x "
			"[fibre_chan_tech]: 0x%x [sfp_cable_tech]:0x%x [fibre_chan_trans_md]:0x%x "
			"[fibre_chan_speed]:0x%x\n",
			entry->eth_10g_ccode, entry->infiniband_ccode, entry->escon_ccode,
			entry->sonet_ccode, entry->eth_ccode, entry->fibre_chan_link_length,
			entry->fibre_chan_tech, entry->sfp_cable_tech, entry->fibre_chan_trans_md,
			entry->fibre_chan_speed);
	dprintf("\n");
}

void
cmd_data_print_sfp_rate_encode(a_uint8_t *param_name, a_ulong_t *buf, a_uint32_t size)
{
	fal_sfp_rate_encode_t *entry;

	dprintf("\n[%s] \n", param_name);

	entry = (fal_sfp_rate_encode_t *)buf;
	dprintf("[encode]:0x%x [nominal_bit_rate]:0x%x\n", entry->encode, entry->nominal_bit_rate);
	dprintf("[rate_id]:0x%x\n", entry->rate_id);
	dprintf("\n");
}

void
cmd_data_print_sfp_link_length(a_uint8_t *param_name, a_ulong_t *buf, a_uint32_t size)
{
	fal_sfp_link_length_t *entry;

	dprintf("\n[%s] \n", param_name);

	entry = (fal_sfp_link_length_t *)buf;
	dprintf("[single_mode_length_km]:0x%x [single_mode_length_100m]:0x%x\n",
			entry->single_mode_length_km, entry->single_mode_length_100m);
	dprintf("[om2_mode_length_10m]:0x%x [om1_mode_length_10m]:0x%x\n",
			entry->om2_mode_length_10m, entry->om1_mode_length_10m);
	dprintf("[copper_mode_length_1m]:0x%x [om3_mode_length_1m]:0x%x\n",
			entry->copper_mode_length_1m, entry->om3_mode_length_1m);
	dprintf("\n");
}

void
cmd_data_print_sfp_vendor_info(a_uint8_t *param_name, a_ulong_t *buf, a_uint32_t size)
{
	fal_sfp_vendor_info_t *entry;
	a_uint8_t index;

	dprintf("\n[%s] \n", param_name);

	entry = (fal_sfp_vendor_info_t *)buf;
	dprintf("[vendor_name]:");
	index = 0;
	while (index < sizeof(entry->vendor_name)) {
		dprintf("%c", entry->vendor_name[index]);
		index++;
	}
	dprintf("\n");

	dprintf("[vendor_oui]:");
	index = 0;
	while (index < sizeof(entry->vendor_oui)) {
		dprintf("%02x", entry->vendor_oui[index]);
		index++;
	}
	dprintf("\n");

	dprintf("[vendor_pn]:");
	index = 0;
	while (index < sizeof(entry->vendor_pn)) {
		dprintf("%c", entry->vendor_pn[index]);
		index++;
	}
	dprintf("\n");

	dprintf("[vendor_rev]:");
	index = 0;
	while (index < sizeof(entry->vendor_rev)) {
		dprintf("%c", entry->vendor_rev[index]);
		index++;
	}
	dprintf("\n");

	dprintf("[vendor_sn]:");
	index = 0;
	while (index < sizeof(entry->vendor_sn)) {
		dprintf("%c", entry->vendor_sn[index]);
		index++;
	}
	dprintf("\n");

	dprintf("[vendor_date_code]:");
	index = 0;
	while (index < sizeof(entry->vendor_date_code)) {
		dprintf("%c", entry->vendor_date_code[index]);
		index++;
	}

	dprintf("\n\n");
}

void
cmd_data_print_sfp_laser_wavelength(a_uint8_t *param_name, a_ulong_t *buf, a_uint32_t size)
{
	fal_sfp_laser_wavelength_t *entry;

	dprintf("\n[%s] \n", param_name);

	entry = (fal_sfp_laser_wavelength_t *)buf;
	dprintf("[laser_wavelength]:0x%x\n", entry->laser_wavelength);
	dprintf("\n");
}

void
cmd_data_print_sfp_option(a_uint8_t *param_name, a_ulong_t *buf, a_uint32_t size)
{
	fal_sfp_option_t *entry;

	dprintf("\n[%s] \n", param_name);

	entry = (fal_sfp_option_t *)buf;
	dprintf("[linear_recv_output]:0x%x [pwr_level_declar]:0x%x "
			"[cool_transc_declar]:0x%x [loss_signal]:0x%x "
			"[loss_invert_signal]:0x%x [tx_fault_signal]:0x%x "
			"[tx_disable]:0x%x [rate_sel]:0x%x\n",
			entry->linear_recv_output, entry->pwr_level_declar,
			entry->cool_transc_declar, entry->loss_signal,
			entry->loss_invert_signal, entry->tx_fault_signal,
			entry->tx_disable, entry->rate_sel);
	dprintf("\n");
}

void
cmd_data_print_sfp_ctrl_rate(a_uint8_t *param_name, a_ulong_t *buf, a_uint32_t size)
{
	fal_sfp_rate_t *entry;

	dprintf("\n[%s] \n", param_name);

	entry = (fal_sfp_rate_t *)buf;
	dprintf("[upper_rate_limit]:0x%x [lower_rate_limit]:0x%x\n",
			entry->upper_rate_limit, entry->lower_rate_limit);
	dprintf("\n");
}

void
cmd_data_print_sfp_enhanced_cfg(a_uint8_t *param_name, a_ulong_t *buf, a_uint32_t size)
{
	fal_sfp_enhanced_cfg_t *entry;

	dprintf("\n[%s] \n", param_name);

	entry = (fal_sfp_enhanced_cfg_t *)buf;
	dprintf("[addr_mode]:0x%x [rec_pwr_type]:0x%x [external_cal]:0x%x "
			"[internal_cal]:0x%x [diag_mon_flag]:0x%x [legacy_type]:0x%x\n",
			entry->addr_mode, entry->rec_pwr_type, entry->external_cal,
			entry->internal_cal, entry->diag_mon_flag, entry->legacy_type);

	dprintf("[soft_rate_sel_op]:0x%x [app_sel_op]:0x%x, [soft_rate_ctrl_op]:0x%x "
			"[rx_los_op]:0x%x [tx_fault_op]:0x%x [tx_disable_ctrl_op]:0x%x "
			"[alarm_warning_flag_op]:0x%x\n",
			entry->soft_rate_sel_op, entry->app_sel_op, entry->soft_rate_ctrl_op,
			entry->rx_los_op, entry->tx_fault_op, entry->tx_disable_ctrl_op,
			entry->alarm_warning_flag_op);

	dprintf("[compliance_feature]:0x%x\n", entry->compliance_feature);
	dprintf("\n");
}

void
cmd_data_print_sfp_diag_threshold(a_uint8_t *param_name, a_ulong_t *buf, a_uint32_t size)
{
	fal_sfp_internal_threshold_t *entry;

	dprintf("\n[%s] \n", param_name);

	entry = (fal_sfp_internal_threshold_t *)buf;
	dprintf("[temp_high_alarm]:0x%x [temp_low_alarm]:0x%x "
			"[temp_high_warning]:0x%x [temp_low_warning]:0x%x\n",
			entry->temp_high_alarm, entry->temp_low_alarm,
			entry->temp_high_warning, entry->temp_low_warning);
	dprintf("[vol_high_alarm]:0x%x [vol_low_alarm]:0x%x "
			"[vol_high_warning]:0x%x [vol_low_warning]:0x%x\n",
			entry->vol_high_alarm, entry->vol_low_alarm,
			entry->vol_high_warning, entry->vol_low_warning);
	dprintf("[bias_high_alarm]:0x%x [bias_low_alarm]:0x%x "
			"[bias_high_warning]:0x%x [bias_low_warning]:0x%x\n",
			entry->bias_high_alarm, entry->bias_low_alarm,
			entry->bias_high_warning, entry->bias_low_warning);
	dprintf("[tx_power_high_alarm]:0x%x [tx_power_low_alarm]:0x%x "
			"[tx_power_high_warning]:0x%x [tx_power_low_warning]:0x%x\n",
			entry->tx_power_high_alarm, entry->tx_power_low_alarm,
			entry->tx_power_high_warning, entry->tx_power_low_warning);
	dprintf("[rx_power_high_alarm]:0x%x [rx_power_low_alarm]:0x%x "
			"[rx_power_high_warning]:0x%x [rx_power_low_warning]:0x%x\n",
			entry->rx_power_high_alarm, entry->rx_power_low_alarm,
			entry->rx_power_high_warning, entry->rx_power_low_warning);
	dprintf("\n");
}

void
cmd_data_print_sfp_diag_cal_const(a_uint8_t *param_name, a_ulong_t *buf, a_uint32_t size)
{
	fal_sfp_cal_const_t *entry;

	dprintf("\n[%s] \n", param_name);

	entry = (fal_sfp_cal_const_t *)buf;
	dprintf("[rx_power4]:0x%x [rx_power3]:0x%x [rx_power2]:0x%x "
			"[rx_power1]:0x%x [rx_power0]:0x%x\n",
			entry->rx_power4, entry->rx_power3, entry->rx_power2,
			entry->rx_power1, entry->rx_power0);
	dprintf("[tx_bias_slope]:0x%x [tx_bias_offset]:0x%x\n",
			entry->tx_bias_slope, entry->tx_bias_offset);
	dprintf("[tx_power_slope]:0x%x [tx_power_offset]:0x%x\n",
			entry->tx_power_slope, entry->tx_power_offset);
	dprintf("[temp_slope]:0x%x [temp_offset]:0x%x\n",
			entry->temp_slope, entry->temp_offset);
	dprintf("[vol_slope]:0x%x [vol_offset]:0x%x\n",
			entry->vol_slope, entry->vol_offset);
	dprintf("\n");
}

void
cmd_data_print_sfp_diag_realtime(a_uint8_t *param_name, a_ulong_t *buf, a_uint32_t size)
{
	fal_sfp_realtime_diag_t *entry;

	dprintf("\n[%s] \n", param_name);

	entry = (fal_sfp_realtime_diag_t *)buf;
	dprintf("[cur_temp]:0x%x [cur_vol]:0x%x [tx_cur_bias]:0x%x\n",
			entry->cur_temp, entry->cur_vol, entry->tx_cur_bias);
	dprintf("[tx_cur_power]:0x%x [rx_cur_power]:0x%x\n",
			entry->tx_cur_power, entry->rx_cur_power);
	dprintf("\n");
}

void
cmd_data_print_sfp_ctrl_status(a_uint8_t *param_name, a_ulong_t *buf, a_uint32_t size)
{
	fal_sfp_ctrl_status_t *entry;

	dprintf("\n[%s] \n", param_name);

	entry = (fal_sfp_ctrl_status_t *)buf;
	dprintf("[data_ready]:0x%x [rx_los]:0x%x [tx_fault]:0x%x [soft_rate_sel]:0x%x "
			"[rate_sel]:0x%x [rs_state]:0x%x [soft_tx_disable]:0x%x "
			"[tx_disable]:0x%x\n",
			entry->data_ready, entry->rx_los, entry->tx_fault, entry->soft_rate_sel,
			entry->rate_sel, entry->rs_state, entry->soft_tx_disable,
			entry->tx_disable);

	dprintf("[pwr_level_sel]:0x%x [pwr_level_op_state]:0x%x [soft_rs_sel]:0x%x\n",
			entry->pwr_level_sel, entry->pwr_level_op_state, entry->soft_rs_sel);

	dprintf("\n");
}

void
cmd_data_print_sfp_alarm_warn_flag(a_uint8_t *param_name, a_ulong_t *buf, a_uint32_t size)
{
	fal_sfp_alarm_warn_flag_t *entry;

	dprintf("\n[%s] \n", param_name);

	entry = (fal_sfp_alarm_warn_flag_t *)buf;
	dprintf("[tx_pwr_low_alarm]:0x%x [tx_pwr_high_alarm]:0x%x "
			"[tx_bias_low_alarm]:0x%x [tx_bias_high_alarm]:0x%x "
			"[vcc_low_alarm]:0x%x [vcc_high_alarm]:0x%x "
			"[tmp_low_alarm]:0x%x [tmp_high_alarm]:0x%x "
			"[rx_pwr_low_alarm]:0x%x [rx_pwr_high_alarm]:0x%x\n",
			entry->tx_pwr_low_alarm, entry->tx_pwr_high_alarm,
			entry->tx_bias_low_alarm, entry->tx_bias_high_alarm,
			entry->vcc_low_alarm, entry->vcc_high_alarm,
			entry->tmp_low_alarm, entry->tmp_high_alarm,
			entry->rx_pwr_low_alarm, entry->rx_pwr_high_alarm);

	dprintf("[tx_pwr_low_warning]:0x%x [tx_pwr_high_warning]:0x%x "
			"[tx_bias_low_warning]:0x%x [tx_bias_high_warning]:0x%x "
			"[vcc_low_warning]:0x%x [vcc_high_warning]:0x%x "
			"[tmp_low_warning]:0x%x [tmp_high_warning]:0x%x "
			"[rx_pwr_low_warning]:0x%x [rx_pwr_high_warning]:0x%x\n",
			entry->tx_pwr_low_warning, entry->tx_pwr_high_warning,
			entry->tx_bias_low_warning, entry->tx_bias_high_warning,
			entry->vcc_low_warning, entry->vcc_high_warning,
			entry->tmp_low_warning, entry->tmp_high_warning,
			entry->rx_pwr_low_warning, entry->rx_pwr_high_warning);
	dprintf("\n");
}

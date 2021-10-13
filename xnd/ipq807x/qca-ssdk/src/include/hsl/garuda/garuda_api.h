/*
 * Copyright (c) 2012, The Linux Foundation. All rights reserved.
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



#ifndef _GARUDA_API_H_
#define _GARUDA_API_H_

#ifdef __cplusplus
extern "C" {
#endif                          /* __cplusplus */

#ifdef IN_PORTCONTROL
#define PORTCONTROL_API \
    SW_API_DEF(SW_API_PT_DUPLEX_GET, garuda_port_duplex_get), \
    SW_API_DEF(SW_API_PT_DUPLEX_SET, garuda_port_duplex_set), \
    SW_API_DEF(SW_API_PT_SPEED_GET, garuda_port_speed_get), \
    SW_API_DEF(SW_API_PT_SPEED_SET, garuda_port_speed_set), \
    SW_API_DEF(SW_API_PT_AN_GET, garuda_port_autoneg_status_get), \
    SW_API_DEF(SW_API_PT_AN_ENABLE, garuda_port_autoneg_enable), \
    SW_API_DEF(SW_API_PT_AN_RESTART, garuda_port_autoneg_restart), \
    SW_API_DEF(SW_API_PT_AN_ADV_GET, garuda_port_autoneg_adv_get), \
    SW_API_DEF(SW_API_PT_AN_ADV_SET, garuda_port_autoneg_adv_set), \
    SW_API_DEF(SW_API_PT_HDR_SET, garuda_port_hdr_status_set), \
    SW_API_DEF(SW_API_PT_HDR_GET, garuda_port_hdr_status_get), \
    SW_API_DEF(SW_API_PT_FLOWCTRL_SET, garuda_port_flowctrl_set), \
    SW_API_DEF(SW_API_PT_FLOWCTRL_GET, garuda_port_flowctrl_get), \
    SW_API_DEF(SW_API_PT_FLOWCTRL_MODE_SET, garuda_port_flowctrl_forcemode_set), \
    SW_API_DEF(SW_API_PT_FLOWCTRL_MODE_GET, garuda_port_flowctrl_forcemode_get),\
    SW_API_DEF(SW_API_PT_POWERSAVE_SET, garuda_port_powersave_set), \
    SW_API_DEF(SW_API_PT_POWERSAVE_GET, garuda_port_powersave_get), \
    SW_API_DEF(SW_API_PT_HIBERNATE_SET, garuda_port_hibernate_set), \
    SW_API_DEF(SW_API_PT_HIBERNATE_GET, garuda_port_hibernate_get), \
    SW_API_DEF(SW_API_PT_CDT, garuda_port_cdt),

#define PORTCONTROL_API_PARAM \
    SW_API_DESC(SW_API_PT_DUPLEX_GET) \
    SW_API_DESC(SW_API_PT_DUPLEX_SET) \
    SW_API_DESC(SW_API_PT_SPEED_GET) \
    SW_API_DESC(SW_API_PT_SPEED_SET) \
    SW_API_DESC(SW_API_PT_AN_GET) \
    SW_API_DESC(SW_API_PT_AN_ENABLE) \
    SW_API_DESC(SW_API_PT_AN_RESTART) \
    SW_API_DESC(SW_API_PT_AN_ADV_GET) \
    SW_API_DESC(SW_API_PT_AN_ADV_SET) \
    SW_API_DESC(SW_API_PT_HDR_SET) \
    SW_API_DESC(SW_API_PT_HDR_GET) \
    SW_API_DESC(SW_API_PT_FLOWCTRL_SET) \
    SW_API_DESC(SW_API_PT_FLOWCTRL_GET) \
    SW_API_DESC(SW_API_PT_FLOWCTRL_MODE_SET) \
    SW_API_DESC(SW_API_PT_FLOWCTRL_MODE_GET)\
    SW_API_DESC(SW_API_PT_POWERSAVE_SET) \
    SW_API_DESC(SW_API_PT_POWERSAVE_GET) \
    SW_API_DESC(SW_API_PT_HIBERNATE_SET) \
    SW_API_DESC(SW_API_PT_HIBERNATE_GET) \
    SW_API_DESC(SW_API_PT_CDT)

#else
#define PORTCONTROL_API
#define PORTCONTROL_API_PARAM
#endif

#ifdef IN_VLAN
#define VLAN_API \
    SW_API_DEF(SW_API_VLAN_ADD, garuda_vlan_create), \
    SW_API_DEF(SW_API_VLAN_DEL, garuda_vlan_delete), \
    SW_API_DEF(SW_API_VLAN_MEM_UPDATE, garuda_vlan_member_update), \
    SW_API_DEF(SW_API_VLAN_FIND, garuda_vlan_find), \
    SW_API_DEF(SW_API_VLAN_NEXT, garuda_vlan_next), \
    SW_API_DEF(SW_API_VLAN_APPEND, garuda_vlan_entry_append),

#define VLAN_API_PARAM \
    SW_API_DESC(SW_API_VLAN_ADD) \
    SW_API_DESC(SW_API_VLAN_DEL) \
    SW_API_DESC(SW_API_VLAN_MEM_UPDATE) \
    SW_API_DESC(SW_API_VLAN_FIND) \
    SW_API_DESC(SW_API_VLAN_NEXT) \
    SW_API_DESC(SW_API_VLAN_APPEND)
#else
#define VLAN_API
#define VLAN_API_PARAM
#endif

#ifdef IN_PORTVLAN
#define PORTVLAN_API \
    SW_API_DEF(SW_API_PT_ING_MODE_GET, garuda_port_1qmode_get), \
    SW_API_DEF(SW_API_PT_ING_MODE_SET, garuda_port_1qmode_set), \
    SW_API_DEF(SW_API_PT_EG_MODE_GET, garuda_port_egvlanmode_get), \
    SW_API_DEF(SW_API_PT_EG_MODE_SET, garuda_port_egvlanmode_set), \
    SW_API_DEF(SW_API_PT_VLAN_MEM_ADD, garuda_portvlan_member_add), \
    SW_API_DEF(SW_API_PT_VLAN_MEM_DEL, garuda_portvlan_member_del), \
    SW_API_DEF(SW_API_PT_VLAN_MEM_UPDATE, garuda_portvlan_member_update), \
    SW_API_DEF(SW_API_PT_VLAN_MEM_GET, garuda_portvlan_member_get), \
    SW_API_DEF(SW_API_PT_DEF_VID_SET, garuda_port_default_vid_set), \
    SW_API_DEF(SW_API_PT_DEF_VID_GET, garuda_port_default_vid_get), \
    SW_API_DEF(SW_API_PT_FORCE_DEF_VID_SET, garuda_port_force_default_vid_set), \
    SW_API_DEF(SW_API_PT_FORCE_DEF_VID_GET, garuda_port_force_default_vid_get), \
    SW_API_DEF(SW_API_PT_FORCE_PORTVLAN_SET, garuda_port_force_portvlan_set), \
    SW_API_DEF(SW_API_PT_FORCE_PORTVLAN_GET, garuda_port_force_portvlan_get), \
    SW_API_DEF(SW_API_PT_NESTVLAN_SET, garuda_port_nestvlan_set), \
    SW_API_DEF(SW_API_PT_NESTVLAN_GET, garuda_port_nestvlan_get), \
    SW_API_DEF(SW_API_NESTVLAN_TPID_SET, garuda_nestvlan_tpid_set), \
    SW_API_DEF(SW_API_NESTVLAN_TPID_GET, garuda_nestvlan_tpid_get),

#define PORTVLAN_API_PARAM \
    SW_API_DESC(SW_API_PT_ING_MODE_GET) \
    SW_API_DESC(SW_API_PT_ING_MODE_SET) \
    SW_API_DESC(SW_API_PT_EG_MODE_GET) \
    SW_API_DESC(SW_API_PT_EG_MODE_SET) \
    SW_API_DESC(SW_API_PT_VLAN_MEM_ADD) \
    SW_API_DESC(SW_API_PT_VLAN_MEM_DEL) \
    SW_API_DESC(SW_API_PT_VLAN_MEM_UPDATE) \
    SW_API_DESC(SW_API_PT_VLAN_MEM_GET)    \
    SW_API_DESC(SW_API_PT_DEF_VID_SET)    \
    SW_API_DESC(SW_API_PT_DEF_VID_GET)    \
    SW_API_DESC(SW_API_PT_FORCE_DEF_VID_SET)    \
    SW_API_DESC(SW_API_PT_FORCE_DEF_VID_GET)    \
    SW_API_DESC(SW_API_PT_FORCE_PORTVLAN_SET)    \
    SW_API_DESC(SW_API_PT_FORCE_PORTVLAN_GET)    \
    SW_API_DESC(SW_API_PT_NESTVLAN_SET)    \
    SW_API_DESC(SW_API_PT_NESTVLAN_GET)    \
    SW_API_DESC(SW_API_NESTVLAN_TPID_SET)    \
    SW_API_DESC(SW_API_NESTVLAN_TPID_GET)
#else
#define PORTVLAN_API
#define PORTVLAN_API_PARAM
#endif

#ifdef IN_FDB
#define FDB_API \
    SW_API_DEF(SW_API_FDB_ADD, garuda_fdb_add), \
    SW_API_DEF(SW_API_FDB_DELALL, garuda_fdb_del_all), \
    SW_API_DEF(SW_API_FDB_DELPORT,garuda_fdb_del_by_port), \
    SW_API_DEF(SW_API_FDB_DELMAC, garuda_fdb_del_by_mac), \
    SW_API_DEF(SW_API_FDB_FIRST,  garuda_fdb_first), \
    SW_API_DEF(SW_API_FDB_NEXT,   garuda_fdb_next), \
    SW_API_DEF(SW_API_FDB_FIND,   garuda_fdb_find), \
    SW_API_DEF(SW_API_FDB_PT_LEARN_SET,   garuda_fdb_port_learn_set), \
    SW_API_DEF(SW_API_FDB_PT_LEARN_GET,   garuda_fdb_port_learn_get), \
    SW_API_DEF(SW_API_FDB_AGE_CTRL_SET,   garuda_fdb_age_ctrl_set), \
    SW_API_DEF(SW_API_FDB_AGE_CTRL_GET,   garuda_fdb_age_ctrl_get), \
    SW_API_DEF(SW_API_FDB_AGE_TIME_SET,   garuda_fdb_age_time_set), \
    SW_API_DEF(SW_API_FDB_AGE_TIME_GET,   garuda_fdb_age_time_get),

#define FDB_API_PARAM \
    SW_API_DESC(SW_API_FDB_ADD) \
    SW_API_DESC(SW_API_FDB_DELALL) \
    SW_API_DESC(SW_API_FDB_DELPORT) \
    SW_API_DESC(SW_API_FDB_DELMAC) \
    SW_API_DESC(SW_API_FDB_FIRST) \
    SW_API_DESC(SW_API_FDB_NEXT) \
    SW_API_DESC(SW_API_FDB_FIND) \
    SW_API_DESC(SW_API_FDB_PT_LEARN_SET) \
    SW_API_DESC(SW_API_FDB_PT_LEARN_GET) \
    SW_API_DESC(SW_API_FDB_AGE_CTRL_SET) \
    SW_API_DESC(SW_API_FDB_AGE_CTRL_GET) \
    SW_API_DESC(SW_API_FDB_AGE_TIME_SET) \
    SW_API_DESC(SW_API_FDB_AGE_TIME_GET)
#else
#define FDB_API
#define FDB_API_PARAM
#endif

#ifdef IN_ACL
#define ACL_API \
    SW_API_DEF(SW_API_ACL_LIST_CREAT, garuda_acl_list_creat), \
    SW_API_DEF(SW_API_ACL_LIST_DESTROY, garuda_acl_list_destroy), \
    SW_API_DEF(SW_API_ACL_RULE_ADD, garuda_acl_rule_add), \
    SW_API_DEF(SW_API_ACL_RULE_DELETE, garuda_acl_rule_delete), \
    SW_API_DEF(SW_API_ACL_RULE_QUERY, garuda_acl_rule_query), \
    SW_API_DEF(SW_API_ACL_LIST_BIND, garuda_acl_list_bind), \
    SW_API_DEF(SW_API_ACL_LIST_UNBIND, garuda_acl_list_unbind), \
    SW_API_DEF(SW_API_ACL_STATUS_SET, garuda_acl_status_set), \
    SW_API_DEF(SW_API_ACL_STATUS_GET, garuda_acl_status_get), \
    SW_API_DEF(SW_API_ACL_LIST_DUMP, garuda_acl_list_dump), \
    SW_API_DEF(SW_API_ACL_RULE_DUMP, garuda_acl_rule_dump),

#define ACL_API_PARAM \
    SW_API_DESC(SW_API_ACL_LIST_CREAT) \
    SW_API_DESC(SW_API_ACL_LIST_DESTROY) \
    SW_API_DESC(SW_API_ACL_RULE_ADD) \
    SW_API_DESC(SW_API_ACL_RULE_DELETE) \
    SW_API_DESC(SW_API_ACL_RULE_QUERY) \
    SW_API_DESC(SW_API_ACL_LIST_BIND) \
    SW_API_DESC(SW_API_ACL_LIST_UNBIND) \
    SW_API_DESC(SW_API_ACL_STATUS_SET) \
    SW_API_DESC(SW_API_ACL_STATUS_GET) \
    SW_API_DESC(SW_API_ACL_LIST_DUMP) \
    SW_API_DESC(SW_API_ACL_RULE_DUMP)
#else
#define ACL_API
#define ACL_API_PARAM
#endif

#ifdef IN_QOS
#define QOS_API \
    SW_API_DEF(SW_API_QOS_SCH_MODE_SET, garuda_qos_sch_mode_set), \
    SW_API_DEF(SW_API_QOS_SCH_MODE_GET, garuda_qos_sch_mode_get), \
    SW_API_DEF(SW_API_QOS_QU_TX_BUF_ST_SET, garuda_qos_queue_tx_buf_status_set), \
    SW_API_DEF(SW_API_QOS_QU_TX_BUF_ST_GET, garuda_qos_queue_tx_buf_status_get), \
    SW_API_DEF(SW_API_QOS_QU_TX_BUF_NR_SET, garuda_qos_queue_tx_buf_nr_set), \
    SW_API_DEF(SW_API_QOS_QU_TX_BUF_NR_GET, garuda_qos_queue_tx_buf_nr_get), \
    SW_API_DEF(SW_API_QOS_PT_TX_BUF_ST_SET, garuda_qos_port_tx_buf_status_set), \
    SW_API_DEF(SW_API_QOS_PT_TX_BUF_ST_GET, garuda_qos_port_tx_buf_status_get), \
    SW_API_DEF(SW_API_QOS_PT_TX_BUF_NR_SET, garuda_qos_port_tx_buf_nr_set), \
    SW_API_DEF(SW_API_QOS_PT_TX_BUF_NR_GET, garuda_qos_port_tx_buf_nr_get), \
    SW_API_DEF(SW_API_COSMAP_UP_QU_SET, garuda_cosmap_up_queue_set), \
    SW_API_DEF(SW_API_COSMAP_UP_QU_GET, garuda_cosmap_up_queue_get), \
    SW_API_DEF(SW_API_COSMAP_DSCP_QU_SET, garuda_cosmap_dscp_queue_set), \
    SW_API_DEF(SW_API_COSMAP_DSCP_QU_GET, garuda_cosmap_dscp_queue_get), \
    SW_API_DEF(SW_API_QOS_PT_MODE_SET, garuda_qos_port_mode_set), \
    SW_API_DEF(SW_API_QOS_PT_MODE_GET, garuda_qos_port_mode_get), \
    SW_API_DEF(SW_API_QOS_PT_MODE_PRI_SET, garuda_qos_port_mode_pri_set), \
    SW_API_DEF(SW_API_QOS_PT_MODE_PRI_GET, garuda_qos_port_mode_pri_get), \
    SW_API_DEF(SW_API_QOS_PORT_DEF_UP_SET, garuda_qos_port_default_up_set), \
    SW_API_DEF(SW_API_QOS_PORT_DEF_UP_GET, garuda_qos_port_default_up_get),

#define QOS_API_PARAM \
    SW_API_DESC(SW_API_QOS_SCH_MODE_SET) \
    SW_API_DESC(SW_API_QOS_SCH_MODE_GET) \
    SW_API_DESC(SW_API_QOS_QU_TX_BUF_ST_SET) \
    SW_API_DESC(SW_API_QOS_QU_TX_BUF_ST_GET) \
    SW_API_DESC(SW_API_QOS_QU_TX_BUF_NR_SET) \
    SW_API_DESC(SW_API_QOS_QU_TX_BUF_NR_GET) \
    SW_API_DESC(SW_API_QOS_PT_TX_BUF_ST_SET) \
    SW_API_DESC(SW_API_QOS_PT_TX_BUF_ST_GET) \
    SW_API_DESC(SW_API_QOS_PT_TX_BUF_NR_SET) \
    SW_API_DESC(SW_API_QOS_PT_TX_BUF_NR_GET) \
    SW_API_DESC(SW_API_COSMAP_UP_QU_SET) \
    SW_API_DESC(SW_API_COSMAP_UP_QU_GET) \
    SW_API_DESC(SW_API_COSMAP_DSCP_QU_SET) \
    SW_API_DESC(SW_API_COSMAP_DSCP_QU_GET) \
    SW_API_DESC(SW_API_QOS_PT_MODE_SET) \
    SW_API_DESC(SW_API_QOS_PT_MODE_GET) \
    SW_API_DESC(SW_API_QOS_PT_MODE_PRI_SET) \
    SW_API_DESC(SW_API_QOS_PT_MODE_PRI_GET) \
    SW_API_DESC(SW_API_QOS_PORT_DEF_UP_SET) \
    SW_API_DESC(SW_API_QOS_PORT_DEF_UP_GET)
#else
#define QOS_API
#define QOS_API_PARAM
#endif

#ifdef IN_IGMP
#define IGMP_API \
    SW_API_DEF(SW_API_PT_IGMPS_MODE_SET, garuda_port_igmps_status_set), \
    SW_API_DEF(SW_API_PT_IGMPS_MODE_GET, garuda_port_igmps_status_get), \
    SW_API_DEF(SW_API_IGMP_MLD_CMD_SET, garuda_igmp_mld_cmd_set), \
    SW_API_DEF(SW_API_IGMP_MLD_CMD_GET, garuda_igmp_mld_cmd_get), \
    SW_API_DEF(SW_API_IGMP_PT_JOIN_SET, garuda_port_igmp_mld_join_set), \
    SW_API_DEF(SW_API_IGMP_PT_JOIN_GET, garuda_port_igmp_mld_join_get), \
    SW_API_DEF(SW_API_IGMP_PT_LEAVE_SET, garuda_port_igmp_mld_leave_set), \
    SW_API_DEF(SW_API_IGMP_PT_LEAVE_GET, garuda_port_igmp_mld_leave_get), \
    SW_API_DEF(SW_API_IGMP_RP_SET, garuda_igmp_mld_rp_set), \
    SW_API_DEF(SW_API_IGMP_RP_GET, garuda_igmp_mld_rp_get), \
    SW_API_DEF(SW_API_IGMP_ENTRY_CREAT_SET, garuda_igmp_mld_entry_creat_set), \
    SW_API_DEF(SW_API_IGMP_ENTRY_CREAT_GET, garuda_igmp_mld_entry_creat_get),

#define IGMP_API_PARAM \
    SW_API_DESC(SW_API_PT_IGMPS_MODE_SET) \
    SW_API_DESC(SW_API_PT_IGMPS_MODE_GET) \
    SW_API_DESC(SW_API_IGMP_MLD_CMD_SET)  \
    SW_API_DESC(SW_API_IGMP_MLD_CMD_GET)  \
    SW_API_DESC(SW_API_IGMP_PT_JOIN_SET)  \
    SW_API_DESC(SW_API_IGMP_PT_JOIN_GET)  \
    SW_API_DESC(SW_API_IGMP_PT_LEAVE_SET) \
    SW_API_DESC(SW_API_IGMP_PT_LEAVE_GET) \
    SW_API_DESC(SW_API_IGMP_RP_SET) \
    SW_API_DESC(SW_API_IGMP_RP_GET) \
    SW_API_DESC(SW_API_IGMP_ENTRY_CREAT_SET) \
    SW_API_DESC(SW_API_IGMP_ENTRY_CREAT_GET)
#else
#define IGMP_API
#define IGMP_API_PARAM
#endif

#ifdef IN_LEAKY
#define LEAKY_API \
    SW_API_DEF(SW_API_UC_LEAKY_MODE_SET, garuda_uc_leaky_mode_set), \
    SW_API_DEF(SW_API_UC_LEAKY_MODE_GET, garuda_uc_leaky_mode_get), \
    SW_API_DEF(SW_API_MC_LEAKY_MODE_SET, garuda_mc_leaky_mode_set), \
    SW_API_DEF(SW_API_MC_LEAKY_MODE_GET, garuda_mc_leaky_mode_get), \
    SW_API_DEF(SW_API_ARP_LEAKY_MODE_SET, garuda_port_arp_leaky_set), \
    SW_API_DEF(SW_API_ARP_LEAKY_MODE_GET, garuda_port_arp_leaky_get), \
    SW_API_DEF(SW_API_PT_UC_LEAKY_MODE_SET, garuda_port_uc_leaky_set), \
    SW_API_DEF(SW_API_PT_UC_LEAKY_MODE_GET, garuda_port_uc_leaky_get), \
    SW_API_DEF(SW_API_PT_MC_LEAKY_MODE_SET, garuda_port_mc_leaky_set), \
    SW_API_DEF(SW_API_PT_MC_LEAKY_MODE_GET, garuda_port_mc_leaky_get),

#define LEAKY_API_PARAM \
    SW_API_DESC(SW_API_UC_LEAKY_MODE_SET)  \
    SW_API_DESC(SW_API_UC_LEAKY_MODE_GET)  \
    SW_API_DESC(SW_API_MC_LEAKY_MODE_SET)  \
    SW_API_DESC(SW_API_MC_LEAKY_MODE_GET)  \
    SW_API_DESC(SW_API_ARP_LEAKY_MODE_SET) \
    SW_API_DESC(SW_API_ARP_LEAKY_MODE_GET) \
    SW_API_DESC(SW_API_PT_UC_LEAKY_MODE_SET) \
    SW_API_DESC(SW_API_PT_UC_LEAKY_MODE_GET) \
    SW_API_DESC(SW_API_PT_MC_LEAKY_MODE_SET) \
    SW_API_DESC(SW_API_PT_MC_LEAKY_MODE_GET)
#else
#define LEAKY_API
#define LEAKY_API_PARAM
#endif

#ifdef IN_MIRROR
#define MIRROR_API \
    SW_API_DEF(SW_API_MIRROR_ANALY_PT_SET, garuda_mirr_analysis_port_set), \
    SW_API_DEF(SW_API_MIRROR_ANALY_PT_GET, garuda_mirr_analysis_port_get), \
    SW_API_DEF(SW_API_MIRROR_IN_PT_SET, garuda_mirr_port_in_set), \
    SW_API_DEF(SW_API_MIRROR_IN_PT_GET, garuda_mirr_port_in_get), \
    SW_API_DEF(SW_API_MIRROR_EG_PT_SET, garuda_mirr_port_eg_set), \
    SW_API_DEF(SW_API_MIRROR_EG_PT_GET, garuda_mirr_port_eg_get),

#define MIRROR_API_PARAM \
    SW_API_DESC(SW_API_MIRROR_ANALY_PT_SET) \
    SW_API_DESC(SW_API_MIRROR_ANALY_PT_GET) \
    SW_API_DESC(SW_API_MIRROR_IN_PT_SET) \
    SW_API_DESC(SW_API_MIRROR_IN_PT_GET) \
    SW_API_DESC(SW_API_MIRROR_EG_PT_SET) \
    SW_API_DESC(SW_API_MIRROR_EG_PT_GET)
#else
#define MIRROR_API
#define MIRROR_API_PARAM
#endif

#ifdef IN_RATE
#define RATE_API \
    SW_API_DEF(SW_API_RATE_QU_EGRL_SET, garuda_rate_queue_egrl_set), \
    SW_API_DEF(SW_API_RATE_QU_EGRL_GET, garuda_rate_queue_egrl_get), \
    SW_API_DEF(SW_API_RATE_PT_EGRL_SET, garuda_rate_port_egrl_set), \
    SW_API_DEF(SW_API_RATE_PT_EGRL_GET, garuda_rate_port_egrl_get), \
    SW_API_DEF(SW_API_RATE_PT_INRL_SET, garuda_rate_port_inrl_set), \
    SW_API_DEF(SW_API_RATE_PT_INRL_GET, garuda_rate_port_inrl_get), \
    SW_API_DEF(SW_API_STORM_CTRL_FRAME_SET, garuda_storm_ctrl_frame_set), \
    SW_API_DEF(SW_API_STORM_CTRL_FRAME_GET, garuda_storm_ctrl_frame_get), \
    SW_API_DEF(SW_API_STORM_CTRL_RATE_SET, garuda_storm_ctrl_rate_set), \
    SW_API_DEF(SW_API_STORM_CTRL_RATE_GET, garuda_storm_ctrl_rate_get),

#define RATE_API_PARAM \
    SW_API_DESC(SW_API_RATE_QU_EGRL_SET) \
    SW_API_DESC(SW_API_RATE_QU_EGRL_GET) \
    SW_API_DESC(SW_API_RATE_PT_EGRL_SET) \
    SW_API_DESC(SW_API_RATE_PT_EGRL_GET) \
    SW_API_DESC(SW_API_RATE_PT_INRL_SET) \
    SW_API_DESC(SW_API_RATE_PT_INRL_GET) \
    SW_API_DESC(SW_API_STORM_CTRL_FRAME_SET) \
    SW_API_DESC(SW_API_STORM_CTRL_FRAME_GET) \
    SW_API_DESC(SW_API_STORM_CTRL_RATE_SET) \
    SW_API_DESC(SW_API_STORM_CTRL_RATE_GET)
#else
#define RATE_API
#define RATE_API_PARAM
#endif

#ifdef IN_STP
#define STP_API \
    SW_API_DEF(SW_API_STP_PT_STATE_SET, garuda_stp_port_state_set), \
    SW_API_DEF(SW_API_STP_PT_STATE_GET, garuda_stp_port_state_get),

#define STP_API_PARAM \
    SW_API_DESC(SW_API_STP_PT_STATE_SET) \
    SW_API_DESC(SW_API_STP_PT_STATE_GET)
#else
#define STP_API
#define STP_API_PARAM
#endif

#ifdef IN_MIB
#define MIB_API \
    SW_API_DEF(SW_API_PT_MIB_GET, garuda_get_mib_info), \
    SW_API_DEF(SW_API_MIB_STATUS_SET, garuda_mib_status_set), \
    SW_API_DEF(SW_API_MIB_STATUS_GET, garuda_mib_status_get),

#define MIB_API_PARAM \
    SW_API_DESC(SW_API_PT_MIB_GET) \
    SW_API_DESC(SW_API_MIB_STATUS_SET) \
    SW_API_DESC(SW_API_MIB_STATUS_GET)
#else
#define MIB_API
#define MIB_API_PARAM
#endif

#ifdef IN_MISC
#define MISC_API \
    SW_API_DEF(SW_API_ARP_STATUS_SET, garuda_arp_status_set), \
    SW_API_DEF(SW_API_ARP_STATUS_GET, garuda_arp_status_get), \
    SW_API_DEF(SW_API_FRAME_MAX_SIZE_SET, garuda_frame_max_size_set), \
    SW_API_DEF(SW_API_FRAME_MAX_SIZE_GET, garuda_frame_max_size_get), \
    SW_API_DEF(SW_API_PT_UNK_SA_CMD_SET, garuda_port_unk_sa_cmd_set), \
    SW_API_DEF(SW_API_PT_UNK_SA_CMD_GET, garuda_port_unk_sa_cmd_get), \
    SW_API_DEF(SW_API_PT_UNK_UC_FILTER_SET, garuda_port_unk_uc_filter_set), \
    SW_API_DEF(SW_API_PT_UNK_UC_FILTER_GET, garuda_port_unk_uc_filter_get), \
    SW_API_DEF(SW_API_PT_UNK_MC_FILTER_SET, garuda_port_unk_mc_filter_set), \
    SW_API_DEF(SW_API_PT_UNK_MC_FILTER_GET, garuda_port_unk_mc_filter_get), \
    SW_API_DEF(SW_API_CPU_PORT_STATUS_SET, garuda_cpu_port_status_set), \
    SW_API_DEF(SW_API_CPU_PORT_STATUS_GET, garuda_cpu_port_status_get), \
    SW_API_DEF(SW_API_BC_TO_CPU_PORT_SET, garuda_bc_to_cpu_port_set), \
    SW_API_DEF(SW_API_BC_TO_CPU_PORT_GET, garuda_bc_to_cpu_port_get), \
    SW_API_DEF(SW_API_PPPOE_CMD_SET, garuda_pppoe_cmd_set), \
    SW_API_DEF(SW_API_PPPOE_CMD_GET, garuda_pppoe_cmd_get), \
    SW_API_DEF(SW_API_PPPOE_STATUS_SET, garuda_pppoe_status_set), \
    SW_API_DEF(SW_API_PPPOE_STATUS_GET, garuda_pppoe_status_get), \
    SW_API_DEF(SW_API_PT_DHCP_SET, garuda_port_dhcp_set), \
    SW_API_DEF(SW_API_PT_DHCP_GET, garuda_port_dhcp_get),

#define MISC_API_PARAM \
    SW_API_DESC(SW_API_ARP_STATUS_SET) \
    SW_API_DESC(SW_API_ARP_STATUS_GET) \
    SW_API_DESC(SW_API_FRAME_MAX_SIZE_SET) \
    SW_API_DESC(SW_API_FRAME_MAX_SIZE_GET) \
    SW_API_DESC(SW_API_PT_UNK_SA_CMD_SET) \
    SW_API_DESC(SW_API_PT_UNK_SA_CMD_GET) \
    SW_API_DESC(SW_API_PT_UNK_UC_FILTER_SET) \
    SW_API_DESC(SW_API_PT_UNK_UC_FILTER_GET) \
    SW_API_DESC(SW_API_PT_UNK_MC_FILTER_SET) \
    SW_API_DESC(SW_API_PT_UNK_MC_FILTER_GET) \
    SW_API_DESC(SW_API_CPU_PORT_STATUS_SET) \
    SW_API_DESC(SW_API_CPU_PORT_STATUS_GET) \
    SW_API_DESC(SW_API_BC_TO_CPU_PORT_SET) \
    SW_API_DESC(SW_API_BC_TO_CPU_PORT_GET) \
    SW_API_DESC(SW_API_PPPOE_CMD_SET) \
    SW_API_DESC(SW_API_PPPOE_CMD_GET) \
    SW_API_DESC(SW_API_PPPOE_STATUS_SET) \
    SW_API_DESC(SW_API_PPPOE_STATUS_GET) \
    SW_API_DESC(SW_API_PT_DHCP_SET) \
    SW_API_DESC(SW_API_PT_DHCP_GET)
#else
#define MISC_API
#define MISC_API_PARAM
#endif

#ifdef IN_LED
#define LED_API \
    SW_API_DEF(SW_API_LED_PATTERN_SET, garuda_led_ctrl_pattern_set), \
    SW_API_DEF(SW_API_LED_PATTERN_GET, garuda_led_ctrl_pattern_get),

#define LED_API_PARAM \
    SW_API_DESC(SW_API_LED_PATTERN_SET) \
    SW_API_DESC(SW_API_LED_PATTERN_GET)
#else
#define LED_API
#define LED_API_PARAM
#endif

#define REG_API \
    SW_API_DEF(SW_API_PHY_GET, garuda_phy_get), \
    SW_API_DEF(SW_API_PHY_SET, garuda_phy_set), \
    SW_API_DEF(SW_API_REG_GET, garuda_reg_get), \
    SW_API_DEF(SW_API_REG_SET, garuda_reg_set), \
    SW_API_DEF(SW_API_REG_FIELD_GET, garuda_reg_field_get), \
    SW_API_DEF(SW_API_REG_FIELD_SET, garuda_reg_field_set),

#define REG_API_PARAM \
    SW_API_DESC(SW_API_PHY_GET) \
    SW_API_DESC(SW_API_PHY_SET) \
    SW_API_DESC(SW_API_REG_GET) \
    SW_API_DESC(SW_API_REG_SET) \
    SW_API_DESC(SW_API_REG_FIELD_GET) \
    SW_API_DESC(SW_API_REG_FIELD_SET)

#define SSDK_API \
    SW_API_DEF(SW_API_SWITCH_RESET, garuda_reset), \
    SW_API_DEF(SW_API_SSDK_CFG, hsl_ssdk_cfg), \
    PORTCONTROL_API \
    VLAN_API \
    PORTVLAN_API \
    FDB_API \
    ACL_API \
    QOS_API \
    IGMP_API \
    LEAKY_API \
    MIRROR_API \
    RATE_API \
    STP_API \
    MIB_API \
    MISC_API \
    LED_API \
    REG_API \
    SW_API_DEF(SW_API_MAX, NULL),

#define SSDK_PARAM  \
    SW_PARAM_DEF(SW_API_SWITCH_RESET, SW_UINT32, 4, SW_PARAM_IN, "Dev ID"), \
    SW_PARAM_DEF(SW_API_SSDK_CFG, SW_UINT32, 4, SW_PARAM_IN, "Dev ID"), \
    SW_PARAM_DEF(SW_API_SSDK_CFG, SW_SSDK_CFG, sizeof(ssdk_cfg_t), SW_PARAM_PTR|SW_PARAM_OUT, "ssdk configuration"), \
    MIB_API_PARAM \
    LEAKY_API_PARAM \
    MISC_API_PARAM \
    IGMP_API_PARAM \
    MIRROR_API_PARAM \
    PORTCONTROL_API_PARAM \
    PORTVLAN_API_PARAM \
    VLAN_API_PARAM \
    FDB_API_PARAM \
    QOS_API_PARAM \
    RATE_API_PARAM \
    STP_API_PARAM \
    ACL_API_PARAM \
    LED_API_PARAM \
    REG_API_PARAM \
    SW_PARAM_DEF(SW_API_MAX, SW_UINT32, 4, SW_PARAM_IN, "Dev ID"),

#if (defined(USER_MODE) && defined(KERNEL_MODULE))
#undef SSDK_API
#undef SSDK_PARAM

#define SSDK_API \
    REG_API \
    SW_API_DEF(SW_API_MAX, NULL),

#define SSDK_PARAM  \
    REG_API_PARAM \
    SW_PARAM_DEF(SW_API_MAX, SW_UINT32, 4, SW_PARAM_IN, "Dev ID"),
#endif

#ifdef __cplusplus
}
#endif                          /* __cplusplus */

#endif                          /* _GARUDA_API_H_ */

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



#ifndef _ISIS_API_H_
#define _ISIS_API_H_

#ifdef __cplusplus
extern "C" {
#endif                          /* __cplusplus */

#ifdef IN_PORTCONTROL
#define PORTCONTROL_API \
    SW_API_DEF(SW_API_PT_DUPLEX_GET, isis_port_duplex_get), \
    SW_API_DEF(SW_API_PT_DUPLEX_SET, isis_port_duplex_set), \
    SW_API_DEF(SW_API_PT_SPEED_GET, isis_port_speed_get), \
    SW_API_DEF(SW_API_PT_SPEED_SET, isis_port_speed_set), \
    SW_API_DEF(SW_API_PT_AN_GET, isis_port_autoneg_status_get), \
    SW_API_DEF(SW_API_PT_AN_ENABLE, isis_port_autoneg_enable), \
    SW_API_DEF(SW_API_PT_AN_RESTART, isis_port_autoneg_restart), \
    SW_API_DEF(SW_API_PT_AN_ADV_GET, isis_port_autoneg_adv_get), \
    SW_API_DEF(SW_API_PT_AN_ADV_SET, isis_port_autoneg_adv_set), \
    SW_API_DEF(SW_API_PT_FLOWCTRL_SET, isis_port_flowctrl_set), \
    SW_API_DEF(SW_API_PT_FLOWCTRL_GET, isis_port_flowctrl_get), \
    SW_API_DEF(SW_API_PT_FLOWCTRL_MODE_SET, isis_port_flowctrl_forcemode_set), \
    SW_API_DEF(SW_API_PT_FLOWCTRL_MODE_GET, isis_port_flowctrl_forcemode_get), \
    SW_API_DEF(SW_API_PT_POWERSAVE_SET, isis_port_powersave_set), \
    SW_API_DEF(SW_API_PT_POWERSAVE_GET, isis_port_powersave_get), \
    SW_API_DEF(SW_API_PT_HIBERNATE_SET, isis_port_hibernate_set), \
    SW_API_DEF(SW_API_PT_HIBERNATE_GET, isis_port_hibernate_get), \
    SW_API_DEF(SW_API_PT_CDT, isis_port_cdt), \
    SW_API_DEF(SW_API_PT_TXHDR_SET, isis_port_txhdr_mode_set), \
    SW_API_DEF(SW_API_PT_TXHDR_GET, isis_port_txhdr_mode_get), \
    SW_API_DEF(SW_API_PT_RXHDR_SET, isis_port_rxhdr_mode_set), \
    SW_API_DEF(SW_API_PT_RXHDR_GET, isis_port_rxhdr_mode_get), \
    SW_API_DEF(SW_API_HEADER_TYPE_SET, isis_header_type_set),  \
    SW_API_DEF(SW_API_HEADER_TYPE_GET, isis_header_type_get), \
    SW_API_DEF(SW_API_TXMAC_STATUS_SET, isis_port_txmac_status_set), \
    SW_API_DEF(SW_API_TXMAC_STATUS_GET, isis_port_txmac_status_get), \
    SW_API_DEF(SW_API_RXMAC_STATUS_SET, isis_port_rxmac_status_set), \
    SW_API_DEF(SW_API_RXMAC_STATUS_GET, isis_port_rxmac_status_get), \
    SW_API_DEF(SW_API_TXFC_STATUS_SET, isis_port_txfc_status_set),   \
    SW_API_DEF(SW_API_TXFC_STATUS_GET, isis_port_txfc_status_get),   \
    SW_API_DEF(SW_API_RXFC_STATUS_SET, isis_port_rxfc_status_set),   \
    SW_API_DEF(SW_API_RXFC_STATUS_GET, isis_port_rxfc_status_get),   \
    SW_API_DEF(SW_API_BP_STATUS_SET, isis_port_bp_status_set),   \
    SW_API_DEF(SW_API_BP_STATUS_GET, isis_port_bp_status_get),   \
    SW_API_DEF(SW_API_PT_LINK_MODE_SET, isis_port_link_forcemode_set),   \
    SW_API_DEF(SW_API_PT_LINK_MODE_GET, isis_port_link_forcemode_get), \
    SW_API_DEF(SW_API_PT_LINK_STATUS_GET, isis_port_link_status_get), \
    SW_API_DEF(SW_API_PT_MAC_LOOPBACK_SET, isis_port_mac_loopback_set), \
    SW_API_DEF(SW_API_PT_MAC_LOOPBACK_GET, isis_port_mac_loopback_get), \
    SW_API_DEF(SW_API_PT_8023AZ_SET, isis_port_8023az_set), \
    SW_API_DEF(SW_API_PT_8023AZ_GET, isis_port_8023az_get),

#define PORTCONTROL_API_PARAM \
    SW_API_DESC(SW_API_PT_DUPLEX_GET) \
    SW_API_DESC(SW_API_PT_DUPLEX_SET) \
    SW_API_DESC(SW_API_PT_SPEED_GET)  \
    SW_API_DESC(SW_API_PT_SPEED_SET)  \
    SW_API_DESC(SW_API_PT_AN_GET)  \
    SW_API_DESC(SW_API_PT_AN_ENABLE)   \
    SW_API_DESC(SW_API_PT_AN_RESTART)  \
    SW_API_DESC(SW_API_PT_AN_ADV_GET)  \
    SW_API_DESC(SW_API_PT_AN_ADV_SET)  \
    SW_API_DESC(SW_API_PT_FLOWCTRL_SET)  \
    SW_API_DESC(SW_API_PT_FLOWCTRL_GET)  \
    SW_API_DESC(SW_API_PT_FLOWCTRL_MODE_SET)  \
    SW_API_DESC(SW_API_PT_FLOWCTRL_MODE_GET) \
    SW_API_DESC(SW_API_PT_POWERSAVE_SET) \
    SW_API_DESC(SW_API_PT_POWERSAVE_GET) \
    SW_API_DESC(SW_API_PT_HIBERNATE_SET) \
    SW_API_DESC(SW_API_PT_HIBERNATE_GET) \
    SW_API_DESC(SW_API_PT_CDT) \
    SW_API_DESC(SW_API_PT_TXHDR_SET) \
    SW_API_DESC(SW_API_PT_TXHDR_GET) \
    SW_API_DESC(SW_API_PT_RXHDR_SET) \
    SW_API_DESC(SW_API_PT_RXHDR_GET) \
    SW_API_DESC(SW_API_HEADER_TYPE_SET) \
    SW_API_DESC(SW_API_HEADER_TYPE_GET) \
    SW_API_DESC(SW_API_TXMAC_STATUS_SET) \
    SW_API_DESC(SW_API_TXMAC_STATUS_GET) \
    SW_API_DESC(SW_API_RXMAC_STATUS_SET) \
    SW_API_DESC(SW_API_RXMAC_STATUS_GET) \
    SW_API_DESC(SW_API_TXFC_STATUS_SET) \
    SW_API_DESC(SW_API_TXFC_STATUS_GET) \
    SW_API_DESC(SW_API_RXFC_STATUS_SET) \
    SW_API_DESC(SW_API_RXFC_STATUS_GET) \
    SW_API_DESC(SW_API_BP_STATUS_SET) \
    SW_API_DESC(SW_API_BP_STATUS_GET) \
    SW_API_DESC(SW_API_PT_LINK_MODE_SET) \
    SW_API_DESC(SW_API_PT_LINK_MODE_GET) \
    SW_API_DESC(SW_API_PT_LINK_STATUS_GET) \
    SW_API_DESC(SW_API_PT_MAC_LOOPBACK_SET) \
    SW_API_DESC(SW_API_PT_MAC_LOOPBACK_GET) \
    SW_API_DESC(SW_API_PT_8023AZ_SET) \
    SW_API_DESC(SW_API_PT_8023AZ_GET)
#else
#define PORTCONTROL_API
#define PORTCONTROL_API_PARAM
#endif

#ifdef IN_VLAN
#define VLAN_API \
    SW_API_DEF(SW_API_VLAN_ADD, isis_vlan_create), \
    SW_API_DEF(SW_API_VLAN_DEL, isis_vlan_delete), \
    SW_API_DEF(SW_API_VLAN_FIND, isis_vlan_find), \
    SW_API_DEF(SW_API_VLAN_NEXT, isis_vlan_next), \
    SW_API_DEF(SW_API_VLAN_APPEND, isis_vlan_entry_append), \
    SW_API_DEF(SW_API_VLAN_FLUSH, isis_vlan_flush), \
    SW_API_DEF(SW_API_VLAN_FID_SET, isis_vlan_fid_set), \
    SW_API_DEF(SW_API_VLAN_FID_GET, isis_vlan_fid_get), \
    SW_API_DEF(SW_API_VLAN_MEMBER_ADD, isis_vlan_member_add), \
    SW_API_DEF(SW_API_VLAN_MEMBER_DEL, isis_vlan_member_del), \
    SW_API_DEF(SW_API_VLAN_LEARN_STATE_SET, isis_vlan_learning_state_set), \
    SW_API_DEF(SW_API_VLAN_LEARN_STATE_GET, isis_vlan_learning_state_get),

#define VLAN_API_PARAM \
    SW_API_DESC(SW_API_VLAN_ADD)     \
    SW_API_DESC(SW_API_VLAN_DEL)     \
    SW_API_DESC(SW_API_VLAN_FIND)    \
    SW_API_DESC(SW_API_VLAN_NEXT)    \
    SW_API_DESC(SW_API_VLAN_APPEND)  \
    SW_API_DESC(SW_API_VLAN_FLUSH)   \
    SW_API_DESC(SW_API_VLAN_FID_SET) \
    SW_API_DESC(SW_API_VLAN_FID_GET) \
    SW_API_DESC(SW_API_VLAN_MEMBER_ADD) \
    SW_API_DESC(SW_API_VLAN_MEMBER_DEL) \
    SW_API_DESC(SW_API_VLAN_LEARN_STATE_SET) \
    SW_API_DESC(SW_API_VLAN_LEARN_STATE_GET)
#else
#define VLAN_API
#define VLAN_API_PARAM
#endif

#ifdef IN_PORTVLAN
#define PORTVLAN_API \
    SW_API_DEF(SW_API_PT_ING_MODE_GET, isis_port_1qmode_get), \
    SW_API_DEF(SW_API_PT_ING_MODE_SET, isis_port_1qmode_set), \
    SW_API_DEF(SW_API_PT_EG_MODE_GET, isis_port_egvlanmode_get), \
    SW_API_DEF(SW_API_PT_EG_MODE_SET, isis_port_egvlanmode_set), \
    SW_API_DEF(SW_API_PT_VLAN_MEM_ADD, isis_portvlan_member_add), \
    SW_API_DEF(SW_API_PT_VLAN_MEM_DEL, isis_portvlan_member_del), \
    SW_API_DEF(SW_API_PT_VLAN_MEM_UPDATE, isis_portvlan_member_update), \
    SW_API_DEF(SW_API_PT_VLAN_MEM_GET, isis_portvlan_member_get), \
    SW_API_DEF(SW_API_PT_FORCE_DEF_VID_SET, isis_port_force_default_vid_set), \
    SW_API_DEF(SW_API_PT_FORCE_DEF_VID_GET, isis_port_force_default_vid_get), \
    SW_API_DEF(SW_API_PT_FORCE_PORTVLAN_SET, isis_port_force_portvlan_set), \
    SW_API_DEF(SW_API_PT_FORCE_PORTVLAN_GET, isis_port_force_portvlan_get), \
    SW_API_DEF(SW_API_NESTVLAN_TPID_SET, isis_nestvlan_tpid_set), \
    SW_API_DEF(SW_API_NESTVLAN_TPID_GET, isis_nestvlan_tpid_get), \
    SW_API_DEF(SW_API_PT_IN_VLAN_MODE_SET, isis_port_invlan_mode_set), \
    SW_API_DEF(SW_API_PT_IN_VLAN_MODE_GET, isis_port_invlan_mode_get), \
    SW_API_DEF(SW_API_PT_TLS_SET, isis_port_tls_set), \
    SW_API_DEF(SW_API_PT_TLS_GET, isis_port_tls_get), \
    SW_API_DEF(SW_API_PT_PRI_PROPAGATION_SET, isis_port_pri_propagation_set), \
    SW_API_DEF(SW_API_PT_PRI_PROPAGATION_GET, isis_port_pri_propagation_get), \
    SW_API_DEF(SW_API_PT_DEF_SVID_SET, isis_port_default_svid_set), \
    SW_API_DEF(SW_API_PT_DEF_SVID_GET, isis_port_default_svid_get), \
    SW_API_DEF(SW_API_PT_DEF_CVID_SET, isis_port_default_cvid_set), \
    SW_API_DEF(SW_API_PT_DEF_CVID_GET, isis_port_default_cvid_get), \
    SW_API_DEF(SW_API_PT_VLAN_PROPAGATION_SET, isis_port_vlan_propagation_set), \
    SW_API_DEF(SW_API_PT_VLAN_PROPAGATION_GET, isis_port_vlan_propagation_get), \
    SW_API_DEF(SW_API_PT_VLAN_TRANS_ADD, isis_port_vlan_trans_add), \
    SW_API_DEF(SW_API_PT_VLAN_TRANS_DEL, isis_port_vlan_trans_del), \
    SW_API_DEF(SW_API_PT_VLAN_TRANS_GET, isis_port_vlan_trans_get), \
    SW_API_DEF(SW_API_QINQ_MODE_SET, isis_qinq_mode_set), \
    SW_API_DEF(SW_API_QINQ_MODE_GET, isis_qinq_mode_get), \
    SW_API_DEF(SW_API_PT_QINQ_ROLE_SET, isis_port_qinq_role_set), \
    SW_API_DEF(SW_API_PT_QINQ_ROLE_GET, isis_port_qinq_role_get), \
    SW_API_DEF(SW_API_PT_VLAN_TRANS_ITERATE, isis_port_vlan_trans_iterate), \
    SW_API_DEF(SW_API_PT_MAC_VLAN_XLT_SET, isis_port_mac_vlan_xlt_set), \
    SW_API_DEF(SW_API_PT_MAC_VLAN_XLT_GET, isis_port_mac_vlan_xlt_get),

#define PORTVLAN_API_PARAM \
    SW_API_DESC(SW_API_PT_ING_MODE_GET)  \
    SW_API_DESC(SW_API_PT_ING_MODE_SET)  \
    SW_API_DESC(SW_API_PT_EG_MODE_GET)  \
    SW_API_DESC(SW_API_PT_EG_MODE_SET)  \
    SW_API_DESC(SW_API_PT_VLAN_MEM_ADD) \
    SW_API_DESC(SW_API_PT_VLAN_MEM_DEL) \
    SW_API_DESC(SW_API_PT_VLAN_MEM_UPDATE)  \
    SW_API_DESC(SW_API_PT_VLAN_MEM_GET)     \
    SW_API_DESC(SW_API_PT_FORCE_DEF_VID_SET)    \
    SW_API_DESC(SW_API_PT_FORCE_DEF_VID_GET)    \
    SW_API_DESC(SW_API_PT_FORCE_PORTVLAN_SET)   \
    SW_API_DESC(SW_API_PT_FORCE_PORTVLAN_GET)   \
    SW_API_DESC(SW_API_NESTVLAN_TPID_SET)    \
    SW_API_DESC(SW_API_NESTVLAN_TPID_GET)    \
    SW_API_DESC(SW_API_PT_IN_VLAN_MODE_SET)    \
    SW_API_DESC(SW_API_PT_IN_VLAN_MODE_GET)    \
    SW_API_DESC(SW_API_PT_TLS_SET)    \
    SW_API_DESC(SW_API_PT_TLS_GET)    \
    SW_API_DESC(SW_API_PT_PRI_PROPAGATION_SET)    \
    SW_API_DESC(SW_API_PT_PRI_PROPAGATION_GET)    \
    SW_API_DESC(SW_API_PT_DEF_SVID_SET)    \
    SW_API_DESC(SW_API_PT_DEF_SVID_GET)    \
    SW_API_DESC(SW_API_PT_DEF_CVID_SET)    \
    SW_API_DESC(SW_API_PT_DEF_CVID_GET)    \
    SW_API_DESC(SW_API_PT_VLAN_PROPAGATION_SET)    \
    SW_API_DESC(SW_API_PT_VLAN_PROPAGATION_GET)    \
    SW_API_DESC(SW_API_PT_VLAN_TRANS_ADD)  \
    SW_API_DESC(SW_API_PT_VLAN_TRANS_DEL)  \
    SW_API_DESC(SW_API_PT_VLAN_TRANS_GET)  \
    SW_API_DESC(SW_API_QINQ_MODE_SET)      \
    SW_API_DESC(SW_API_QINQ_MODE_GET)      \
    SW_API_DESC(SW_API_PT_QINQ_ROLE_SET)   \
    SW_API_DESC(SW_API_PT_QINQ_ROLE_GET)   \
    SW_API_DESC(SW_API_PT_VLAN_TRANS_ITERATE) \
    SW_API_DESC(SW_API_PT_MAC_VLAN_XLT_SET)   \
    SW_API_DESC(SW_API_PT_MAC_VLAN_XLT_GET)
#else
#define PORTVLAN_API
#define PORTVLAN_API_PARAM
#endif

#ifdef IN_FDB
#define FDB_API \
    SW_API_DEF(SW_API_FDB_ADD, isis_fdb_add), \
    SW_API_DEF(SW_API_FDB_DELALL, isis_fdb_del_all), \
    SW_API_DEF(SW_API_FDB_DELPORT,isis_fdb_del_by_port), \
    SW_API_DEF(SW_API_FDB_DELMAC, isis_fdb_del_by_mac), \
    SW_API_DEF(SW_API_FDB_FIND,   isis_fdb_find), \
    SW_API_DEF(SW_API_FDB_EXTEND_NEXT,    isis_fdb_extend_next),  \
    SW_API_DEF(SW_API_FDB_EXTEND_FIRST,   isis_fdb_extend_first),  \
    SW_API_DEF(SW_API_FDB_TRANSFER,       isis_fdb_transfer),  \
    SW_API_DEF(SW_API_FDB_PT_LEARN_SET,   isis_fdb_port_learn_set), \
    SW_API_DEF(SW_API_FDB_PT_LEARN_GET,   isis_fdb_port_learn_get), \
    SW_API_DEF(SW_API_FDB_AGE_CTRL_SET,   isis_fdb_age_ctrl_set), \
    SW_API_DEF(SW_API_FDB_AGE_CTRL_GET,   isis_fdb_age_ctrl_get), \
    SW_API_DEF(SW_API_FDB_AGE_TIME_SET,   isis_fdb_age_time_set), \
    SW_API_DEF(SW_API_FDB_AGE_TIME_GET,   isis_fdb_age_time_get), \
    SW_API_DEF(SW_API_PT_FDB_LEARN_LIMIT_SET,    isis_port_fdb_learn_limit_set),  \
    SW_API_DEF(SW_API_PT_FDB_LEARN_LIMIT_GET,    isis_port_fdb_learn_limit_get),  \
    SW_API_DEF(SW_API_PT_FDB_LEARN_EXCEED_CMD_SET,    isis_port_fdb_learn_exceed_cmd_set),  \
    SW_API_DEF(SW_API_PT_FDB_LEARN_EXCEED_CMD_GET,    isis_port_fdb_learn_exceed_cmd_get), \
    SW_API_DEF(SW_API_FDB_LEARN_LIMIT_SET,    isis_fdb_learn_limit_set),  \
    SW_API_DEF(SW_API_FDB_LEARN_LIMIT_GET,    isis_fdb_learn_limit_get),  \
    SW_API_DEF(SW_API_FDB_LEARN_EXCEED_CMD_SET,    isis_fdb_learn_exceed_cmd_set),  \
    SW_API_DEF(SW_API_FDB_LEARN_EXCEED_CMD_GET,    isis_fdb_learn_exceed_cmd_get),  \
    SW_API_DEF(SW_API_FDB_RESV_ADD, isis_fdb_resv_add), \
    SW_API_DEF(SW_API_FDB_RESV_DEL, isis_fdb_resv_del), \
    SW_API_DEF(SW_API_FDB_RESV_FIND, isis_fdb_resv_find), \
    SW_API_DEF(SW_API_FDB_RESV_ITERATE, isis_fdb_resv_iterate), \
    SW_API_DEF(SW_API_FDB_EXTEND_FIRST, isis_fdb_extend_first), \
    SW_API_DEF(SW_API_FDB_PT_LEARN_STATIC_SET,   isis_fdb_port_learn_static_set), \
    SW_API_DEF(SW_API_FDB_PT_LEARN_STATIC_GET,   isis_fdb_port_learn_static_get), \
    SW_API_DEF(SW_API_FDB_PORT_ADD,   isis_fdb_port_add), \
    SW_API_DEF(SW_API_FDB_PORT_DEL,   isis_fdb_port_del),

#define FDB_API_PARAM \
    SW_API_DESC(SW_API_FDB_ADD) \
    SW_API_DESC(SW_API_FDB_DELALL)  \
    SW_API_DESC(SW_API_FDB_DELPORT) \
    SW_API_DESC(SW_API_FDB_DELMAC) \
    SW_API_DESC(SW_API_FDB_FIND) \
    SW_API_DESC(SW_API_FDB_EXTEND_NEXT)  \
    SW_API_DESC(SW_API_FDB_EXTEND_FIRST) \
    SW_API_DESC(SW_API_FDB_TRANSFER) \
    SW_API_DESC(SW_API_FDB_PT_LEARN_SET) \
    SW_API_DESC(SW_API_FDB_PT_LEARN_GET) \
    SW_API_DESC(SW_API_FDB_AGE_CTRL_SET) \
    SW_API_DESC(SW_API_FDB_AGE_CTRL_GET) \
    SW_API_DESC(SW_API_FDB_AGE_TIME_SET) \
    SW_API_DESC(SW_API_FDB_AGE_TIME_GET) \
    SW_API_DESC(SW_API_PT_FDB_LEARN_LIMIT_SET)  \
    SW_API_DESC(SW_API_PT_FDB_LEARN_LIMIT_GET)  \
    SW_API_DESC(SW_API_PT_FDB_LEARN_EXCEED_CMD_SET)  \
    SW_API_DESC(SW_API_PT_FDB_LEARN_EXCEED_CMD_GET)  \
    SW_API_DESC(SW_API_FDB_LEARN_LIMIT_SET)  \
    SW_API_DESC(SW_API_FDB_LEARN_LIMIT_GET)  \
    SW_API_DESC(SW_API_FDB_LEARN_EXCEED_CMD_SET)  \
    SW_API_DESC(SW_API_FDB_LEARN_EXCEED_CMD_GET)  \
    SW_API_DESC(SW_API_FDB_RESV_ADD)  \
    SW_API_DESC(SW_API_FDB_RESV_DEL)  \
    SW_API_DESC(SW_API_FDB_RESV_FIND) \
    SW_API_DESC(SW_API_FDB_RESV_ITERATE) \
    SW_API_DESC(SW_API_FDB_EXTEND_FIRST) \
    SW_API_DESC(SW_API_FDB_PT_LEARN_STATIC_SET) \
    SW_API_DESC(SW_API_FDB_PT_LEARN_STATIC_GET) \
    SW_API_DESC(SW_API_FDB_PORT_ADD)  \
    SW_API_DESC(SW_API_FDB_PORT_DEL)

#else
#define FDB_API
#define FDB_API_PARAM
#endif


#ifdef IN_ACL
#define ACL_API \
    SW_API_DEF(SW_API_ACL_LIST_CREAT, isis_acl_list_creat), \
    SW_API_DEF(SW_API_ACL_LIST_DESTROY, isis_acl_list_destroy), \
    SW_API_DEF(SW_API_ACL_RULE_ADD, isis_acl_rule_add), \
    SW_API_DEF(SW_API_ACL_RULE_DELETE, isis_acl_rule_delete), \
    SW_API_DEF(SW_API_ACL_RULE_QUERY, isis_acl_rule_query), \
    SW_API_DEF(SW_API_ACL_LIST_BIND, isis_acl_list_bind), \
    SW_API_DEF(SW_API_ACL_LIST_UNBIND, isis_acl_list_unbind), \
    SW_API_DEF(SW_API_ACL_STATUS_SET, isis_acl_status_set), \
    SW_API_DEF(SW_API_ACL_STATUS_GET, isis_acl_status_get), \
    SW_API_DEF(SW_API_ACL_LIST_DUMP, isis_acl_list_dump), \
    SW_API_DEF(SW_API_ACL_RULE_DUMP, isis_acl_rule_dump), \
    SW_API_DEF(SW_API_ACL_PT_UDF_PROFILE_SET, isis_acl_port_udf_profile_set), \
    SW_API_DEF(SW_API_ACL_PT_UDF_PROFILE_GET, isis_acl_port_udf_profile_get), \
    SW_API_DEF(SW_API_ACL_RULE_ACTIVE, isis_acl_rule_active), \
    SW_API_DEF(SW_API_ACL_RULE_DEACTIVE, isis_acl_rule_deactive),\
    SW_API_DEF(SW_API_ACL_RULE_GET_OFFSET, isis_acl_rule_get_offset),

#define ACL_API_PARAM \
    SW_API_DESC(SW_API_ACL_LIST_CREAT)   \
    SW_API_DESC(SW_API_ACL_LIST_DESTROY) \
    SW_API_DESC(SW_API_ACL_RULE_ADD)     \
    SW_API_DESC(SW_API_ACL_RULE_DELETE)  \
    SW_API_DESC(SW_API_ACL_RULE_QUERY)   \
    SW_API_DESC(SW_API_ACL_LIST_BIND)    \
    SW_API_DESC(SW_API_ACL_LIST_UNBIND)  \
    SW_API_DESC(SW_API_ACL_STATUS_SET)   \
    SW_API_DESC(SW_API_ACL_STATUS_GET)   \
    SW_API_DESC(SW_API_ACL_LIST_DUMP)    \
    SW_API_DESC(SW_API_ACL_RULE_DUMP)    \
    SW_API_DESC(SW_API_ACL_PT_UDF_PROFILE_SET)    \
    SW_API_DESC(SW_API_ACL_PT_UDF_PROFILE_GET)    \
    SW_API_DESC(SW_API_ACL_RULE_ACTIVE)    \
    SW_API_DESC(SW_API_ACL_RULE_DEACTIVE)
#else
#define ACL_API
#define ACL_API_PARAM
#endif


#ifdef IN_QOS
#define QOS_API \
    SW_API_DEF(SW_API_QOS_QU_TX_BUF_ST_SET, isis_qos_queue_tx_buf_status_set), \
    SW_API_DEF(SW_API_QOS_QU_TX_BUF_ST_GET, isis_qos_queue_tx_buf_status_get), \
    SW_API_DEF(SW_API_QOS_QU_TX_BUF_NR_SET, isis_qos_queue_tx_buf_nr_set), \
    SW_API_DEF(SW_API_QOS_QU_TX_BUF_NR_GET, isis_qos_queue_tx_buf_nr_get), \
    SW_API_DEF(SW_API_QOS_PT_TX_BUF_ST_SET, isis_qos_port_tx_buf_status_set), \
    SW_API_DEF(SW_API_QOS_PT_TX_BUF_ST_GET, isis_qos_port_tx_buf_status_get), \
    SW_API_DEF(SW_API_QOS_PT_TX_BUF_NR_SET, isis_qos_port_tx_buf_nr_set), \
    SW_API_DEF(SW_API_QOS_PT_TX_BUF_NR_GET, isis_qos_port_tx_buf_nr_get), \
    SW_API_DEF(SW_API_QOS_PT_RX_BUF_NR_SET, isis_qos_port_rx_buf_nr_set), \
    SW_API_DEF(SW_API_QOS_PT_RX_BUF_NR_GET, isis_qos_port_rx_buf_nr_get), \
    SW_API_DEF(SW_API_QOS_PT_MODE_SET, isis_qos_port_mode_set), \
    SW_API_DEF(SW_API_QOS_PT_MODE_GET, isis_qos_port_mode_get), \
    SW_API_DEF(SW_API_QOS_PT_MODE_PRI_SET, isis_qos_port_mode_pri_set), \
    SW_API_DEF(SW_API_QOS_PT_MODE_PRI_GET, isis_qos_port_mode_pri_get), \
    SW_API_DEF(SW_API_QOS_PORT_SCH_MODE_SET, isis_qos_port_sch_mode_set), \
    SW_API_DEF(SW_API_QOS_PORT_SCH_MODE_GET, isis_qos_port_sch_mode_get), \
    SW_API_DEF(SW_API_QOS_PT_DEF_SPRI_SET, isis_qos_port_default_spri_set), \
    SW_API_DEF(SW_API_QOS_PT_DEF_SPRI_GET, isis_qos_port_default_spri_get), \
    SW_API_DEF(SW_API_QOS_PT_DEF_CPRI_SET, isis_qos_port_default_cpri_set), \
    SW_API_DEF(SW_API_QOS_PT_DEF_CPRI_GET, isis_qos_port_default_cpri_get), \
    SW_API_DEF(SW_API_QOS_QUEUE_REMARK_SET, isis_qos_queue_remark_table_set), \
    SW_API_DEF(SW_API_QOS_QUEUE_REMARK_GET, isis_qos_queue_remark_table_get),


#define QOS_API_PARAM \
    SW_API_DESC(SW_API_QOS_QU_TX_BUF_ST_SET) \
    SW_API_DESC(SW_API_QOS_QU_TX_BUF_ST_GET) \
    SW_API_DESC(SW_API_QOS_QU_TX_BUF_NR_SET) \
    SW_API_DESC(SW_API_QOS_QU_TX_BUF_NR_GET) \
    SW_API_DESC(SW_API_QOS_PT_TX_BUF_ST_SET) \
    SW_API_DESC(SW_API_QOS_PT_TX_BUF_ST_GET) \
    SW_API_DESC(SW_API_QOS_PT_TX_BUF_NR_SET) \
    SW_API_DESC(SW_API_QOS_PT_TX_BUF_NR_GET) \
    SW_API_DESC(SW_API_QOS_PT_RX_BUF_NR_SET) \
    SW_API_DESC(SW_API_QOS_PT_RX_BUF_NR_GET) \
    SW_API_DESC(SW_API_QOS_PT_MODE_SET) \
    SW_API_DESC(SW_API_QOS_PT_MODE_GET) \
    SW_API_DESC(SW_API_QOS_PT_MODE_PRI_SET) \
    SW_API_DESC(SW_API_QOS_PT_MODE_PRI_GET) \
    SW_API_DESC(SW_API_QOS_PORT_DEF_UP_SET) \
    SW_API_DESC(SW_API_QOS_PORT_DEF_UP_GET) \
    SW_API_DESC(SW_API_QOS_PORT_SCH_MODE_SET) \
    SW_API_DESC(SW_API_QOS_PORT_SCH_MODE_GET) \
    SW_API_DESC(SW_API_QOS_PT_DEF_SPRI_SET)  \
    SW_API_DESC(SW_API_QOS_PT_DEF_SPRI_GET)  \
    SW_API_DESC(SW_API_QOS_PT_DEF_CPRI_SET)  \
    SW_API_DESC(SW_API_QOS_PT_DEF_CPRI_GET)  \
    SW_API_DESC(SW_API_QOS_QUEUE_REMARK_SET) \
    SW_API_DESC(SW_API_QOS_QUEUE_REMARK_GET)
#else
#define QOS_API
#define QOS_API_PARAM
#endif


#ifdef IN_IGMP
#define IGMP_API \
    SW_API_DEF(SW_API_PT_IGMPS_MODE_SET, isis_port_igmps_status_set), \
    SW_API_DEF(SW_API_PT_IGMPS_MODE_GET, isis_port_igmps_status_get), \
    SW_API_DEF(SW_API_IGMP_MLD_CMD_SET, isis_igmp_mld_cmd_set), \
    SW_API_DEF(SW_API_IGMP_MLD_CMD_GET, isis_igmp_mld_cmd_get), \
    SW_API_DEF(SW_API_IGMP_PT_JOIN_SET, isis_port_igmp_mld_join_set), \
    SW_API_DEF(SW_API_IGMP_PT_JOIN_GET, isis_port_igmp_mld_join_get), \
    SW_API_DEF(SW_API_IGMP_PT_LEAVE_SET, isis_port_igmp_mld_leave_set), \
    SW_API_DEF(SW_API_IGMP_PT_LEAVE_GET, isis_port_igmp_mld_leave_get), \
    SW_API_DEF(SW_API_IGMP_RP_SET, isis_igmp_mld_rp_set), \
    SW_API_DEF(SW_API_IGMP_RP_GET, isis_igmp_mld_rp_get), \
    SW_API_DEF(SW_API_IGMP_ENTRY_CREAT_SET, isis_igmp_mld_entry_creat_set), \
    SW_API_DEF(SW_API_IGMP_ENTRY_CREAT_GET, isis_igmp_mld_entry_creat_get), \
    SW_API_DEF(SW_API_IGMP_ENTRY_STATIC_SET, isis_igmp_mld_entry_static_set), \
    SW_API_DEF(SW_API_IGMP_ENTRY_STATIC_GET, isis_igmp_mld_entry_static_get), \
    SW_API_DEF(SW_API_IGMP_ENTRY_LEAKY_SET, isis_igmp_mld_entry_leaky_set), \
    SW_API_DEF(SW_API_IGMP_ENTRY_LEAKY_GET, isis_igmp_mld_entry_leaky_get), \
    SW_API_DEF(SW_API_IGMP_ENTRY_V3_SET, isis_igmp_mld_entry_v3_set), \
    SW_API_DEF(SW_API_IGMP_ENTRY_V3_GET, isis_igmp_mld_entry_v3_get), \
    SW_API_DEF(SW_API_IGMP_ENTRY_QUEUE_SET, isis_igmp_mld_entry_queue_set), \
    SW_API_DEF(SW_API_IGMP_ENTRY_QUEUE_GET, isis_igmp_mld_entry_queue_get), \
    SW_API_DEF(SW_API_PT_IGMP_LEARN_LIMIT_SET,    isis_port_igmp_mld_learn_limit_set),  \
    SW_API_DEF(SW_API_PT_IGMP_LEARN_LIMIT_GET,    isis_port_igmp_mld_learn_limit_get),  \
    SW_API_DEF(SW_API_PT_IGMP_LEARN_EXCEED_CMD_SET,    isis_port_igmp_mld_learn_exceed_cmd_set),  \
    SW_API_DEF(SW_API_PT_IGMP_LEARN_EXCEED_CMD_GET,    isis_port_igmp_mld_learn_exceed_cmd_get), \
    SW_API_DEF(SW_API_IGMP_SG_ENTRY_SET,    isis_igmp_sg_entry_set),  \
    SW_API_DEF(SW_API_IGMP_SG_ENTRY_CLEAR,    isis_igmp_sg_entry_clear),  \
    SW_API_DEF(SW_API_IGMP_SG_ENTRY_SHOW,    isis_igmp_sg_entry_show),

#define IGMP_API_PARAM \
    SW_API_DESC(SW_API_PT_IGMPS_MODE_SET) \
    SW_API_DESC(SW_API_PT_IGMPS_MODE_GET) \
    SW_API_DESC(SW_API_IGMP_MLD_CMD_SET)  \
    SW_API_DESC(SW_API_IGMP_MLD_CMD_GET)  \
    SW_API_DESC(SW_API_IGMP_PT_JOIN_SET)  \
    SW_API_DESC(SW_API_IGMP_PT_JOIN_GET)  \
    SW_API_DESC(SW_API_IGMP_PT_LEAVE_SET) \
    SW_API_DESC(SW_API_IGMP_PT_LEAVE_GET) \
    SW_API_DESC(SW_API_IGMP_RP_SET)  \
    SW_API_DESC(SW_API_IGMP_RP_GET)  \
    SW_API_DESC(SW_API_IGMP_ENTRY_CREAT_SET)  \
    SW_API_DESC(SW_API_IGMP_ENTRY_CREAT_GET)  \
    SW_API_DESC(SW_API_IGMP_ENTRY_STATIC_SET) \
    SW_API_DESC(SW_API_IGMP_ENTRY_STATIC_GET) \
    SW_API_DESC(SW_API_IGMP_ENTRY_LEAKY_SET)  \
    SW_API_DESC(SW_API_IGMP_ENTRY_LEAKY_GET)  \
    SW_API_DESC(SW_API_IGMP_ENTRY_V3_SET)  \
    SW_API_DESC(SW_API_IGMP_ENTRY_V3_GET)  \
    SW_API_DESC(SW_API_IGMP_ENTRY_QUEUE_SET)  \
    SW_API_DESC(SW_API_IGMP_ENTRY_QUEUE_GET) \
    SW_API_DESC(SW_API_PT_IGMP_LEARN_LIMIT_SET) \
    SW_API_DESC(SW_API_PT_IGMP_LEARN_LIMIT_GET) \
    SW_API_DESC(SW_API_PT_IGMP_LEARN_EXCEED_CMD_SET) \
    SW_API_DESC(SW_API_PT_IGMP_LEARN_EXCEED_CMD_GET) \
    SW_API_DESC(SW_API_IGMP_SG_ENTRY_SET) \
    SW_API_DESC(SW_API_IGMP_SG_ENTRY_CLEAR) \
    SW_API_DESC(SW_API_IGMP_SG_ENTRY_SHOW)
#else
#define IGMP_API
#define IGMP_API_PARAM
#endif


#ifdef IN_LEAKY
#define LEAKY_API \
    SW_API_DEF(SW_API_UC_LEAKY_MODE_SET, isis_uc_leaky_mode_set), \
    SW_API_DEF(SW_API_UC_LEAKY_MODE_GET, isis_uc_leaky_mode_get), \
    SW_API_DEF(SW_API_MC_LEAKY_MODE_SET, isis_mc_leaky_mode_set), \
    SW_API_DEF(SW_API_MC_LEAKY_MODE_GET, isis_mc_leaky_mode_get), \
    SW_API_DEF(SW_API_ARP_LEAKY_MODE_SET, isis_port_arp_leaky_set), \
    SW_API_DEF(SW_API_ARP_LEAKY_MODE_GET, isis_port_arp_leaky_get), \
    SW_API_DEF(SW_API_PT_UC_LEAKY_MODE_SET, isis_port_uc_leaky_set), \
    SW_API_DEF(SW_API_PT_UC_LEAKY_MODE_GET, isis_port_uc_leaky_get), \
    SW_API_DEF(SW_API_PT_MC_LEAKY_MODE_SET, isis_port_mc_leaky_set), \
    SW_API_DEF(SW_API_PT_MC_LEAKY_MODE_GET, isis_port_mc_leaky_get),

#define LEAKY_API_PARAM \
    SW_API_DESC(SW_API_UC_LEAKY_MODE_SET)  \
    SW_API_DESC(SW_API_UC_LEAKY_MODE_GET)  \
    SW_API_DESC(SW_API_MC_LEAKY_MODE_SET)  \
    SW_API_DESC(SW_API_MC_LEAKY_MODE_GET)  \
    SW_API_DESC(SW_API_ARP_LEAKY_MODE_SET) \
    SW_API_DESC(SW_API_ARP_LEAKY_MODE_GET) \
    SW_API_DESC(SW_API_PT_UC_LEAKY_MODE_SET)  \
    SW_API_DESC(SW_API_PT_UC_LEAKY_MODE_GET)  \
    SW_API_DESC(SW_API_PT_MC_LEAKY_MODE_SET)  \
    SW_API_DESC(SW_API_PT_MC_LEAKY_MODE_GET)
#else
#define LEAKY_API
#define LEAKY_API_PARAM
#endif


#ifdef IN_MIRROR
#define MIRROR_API \
    SW_API_DEF(SW_API_MIRROR_ANALY_PT_SET, isis_mirr_analysis_port_set), \
    SW_API_DEF(SW_API_MIRROR_ANALY_PT_GET, isis_mirr_analysis_port_get), \
    SW_API_DEF(SW_API_MIRROR_IN_PT_SET, isis_mirr_port_in_set), \
    SW_API_DEF(SW_API_MIRROR_IN_PT_GET, isis_mirr_port_in_get), \
    SW_API_DEF(SW_API_MIRROR_EG_PT_SET, isis_mirr_port_eg_set), \
    SW_API_DEF(SW_API_MIRROR_EG_PT_GET, isis_mirr_port_eg_get),

#define MIRROR_API_PARAM \
    SW_API_DESC(SW_API_MIRROR_ANALY_PT_SET)  \
    SW_API_DESC(SW_API_MIRROR_ANALY_PT_GET)  \
    SW_API_DESC(SW_API_MIRROR_IN_PT_SET)  \
    SW_API_DESC(SW_API_MIRROR_IN_PT_GET)  \
    SW_API_DESC(SW_API_MIRROR_EG_PT_SET)  \
    SW_API_DESC(SW_API_MIRROR_EG_PT_GET)
#else
#define MIRROR_API
#define MIRROR_API_PARAM
#endif


#ifdef IN_RATE
#define RATE_API \
    SW_API_DEF(SW_API_RATE_PORT_POLICER_SET, isis_rate_port_policer_set), \
    SW_API_DEF(SW_API_RATE_PORT_POLICER_GET, isis_rate_port_policer_get), \
    SW_API_DEF(SW_API_RATE_PORT_SHAPER_SET, isis_rate_port_shaper_set), \
    SW_API_DEF(SW_API_RATE_PORT_SHAPER_GET, isis_rate_port_shaper_get), \
    SW_API_DEF(SW_API_RATE_QUEUE_SHAPER_SET, isis_rate_queue_shaper_set), \
    SW_API_DEF(SW_API_RATE_QUEUE_SHAPER_GET, isis_rate_queue_shaper_get), \
    SW_API_DEF(SW_API_RATE_ACL_POLICER_SET, isis_rate_acl_policer_set), \
    SW_API_DEF(SW_API_RATE_ACL_POLICER_GET, isis_rate_acl_policer_get), \
    SW_API_DEF(SW_API_RATE_PT_ADDRATEBYTE_SET, isis_rate_port_add_rate_byte_set), \
    SW_API_DEF(SW_API_RATE_PT_ADDRATEBYTE_GET, isis_rate_port_add_rate_byte_get),

#define RATE_API_PARAM \
    SW_API_DESC(SW_API_RATE_PORT_POLICER_SET) \
    SW_API_DESC(SW_API_RATE_PORT_POLICER_GET) \
    SW_API_DESC(SW_API_RATE_PORT_SHAPER_SET) \
    SW_API_DESC(SW_API_RATE_PORT_SHAPER_GET) \
    SW_API_DESC(SW_API_RATE_QUEUE_SHAPER_SET) \
    SW_API_DESC(SW_API_RATE_QUEUE_SHAPER_GET) \
    SW_API_DESC(SW_API_RATE_ACL_POLICER_SET) \
    SW_API_DESC(SW_API_RATE_ACL_POLICER_GET) \
    SW_API_DESC(SW_API_RATE_PT_ADDRATEBYTE_SET) \
    SW_API_DESC(SW_API_RATE_PT_ADDRATEBYTE_GET)
#else
#define RATE_API
#define RATE_API_PARAM
#endif


#ifdef IN_STP
#define STP_API \
    SW_API_DEF(SW_API_STP_PT_STATE_SET, isis_stp_port_state_set), \
    SW_API_DEF(SW_API_STP_PT_STATE_GET, isis_stp_port_state_get),

#define STP_API_PARAM \
    SW_API_DESC(SW_API_STP_PT_STATE_SET) \
    SW_API_DESC(SW_API_STP_PT_STATE_GET)
#else
#define STP_API
#define STP_API_PARAM
#endif


#ifdef IN_MIB
#define MIB_API \
    SW_API_DEF(SW_API_PT_MIB_GET, isis_get_mib_info), \
    SW_API_DEF(SW_API_MIB_STATUS_SET, isis_mib_status_set), \
    SW_API_DEF(SW_API_MIB_STATUS_GET, isis_mib_status_get),

#define MIB_API_PARAM \
    SW_API_DESC(SW_API_PT_MIB_GET)  \
    SW_API_DESC(SW_API_MIB_STATUS_SET) \
    SW_API_DESC(SW_API_MIB_STATUS_GET)
#else
#define MIB_API
#define MIB_API_PARAM
#endif


#ifdef IN_MISC
#define MISC_API \
    SW_API_DEF(SW_API_FRAME_MAX_SIZE_SET, isis_frame_max_size_set), \
    SW_API_DEF(SW_API_FRAME_MAX_SIZE_GET, isis_frame_max_size_get), \
    SW_API_DEF(SW_API_PT_UNK_UC_FILTER_SET, isis_port_unk_uc_filter_set), \
    SW_API_DEF(SW_API_PT_UNK_UC_FILTER_GET, isis_port_unk_uc_filter_get), \
    SW_API_DEF(SW_API_PT_UNK_MC_FILTER_SET, isis_port_unk_mc_filter_set), \
    SW_API_DEF(SW_API_PT_UNK_MC_FILTER_GET, isis_port_unk_mc_filter_get), \
    SW_API_DEF(SW_API_PT_BC_FILTER_SET, isis_port_bc_filter_set), \
    SW_API_DEF(SW_API_PT_BC_FILTER_GET, isis_port_bc_filter_get), \
    SW_API_DEF(SW_API_CPU_PORT_STATUS_SET, isis_cpu_port_status_set), \
    SW_API_DEF(SW_API_CPU_PORT_STATUS_GET, isis_cpu_port_status_get), \
    SW_API_DEF(SW_API_PPPOE_CMD_SET, isis_pppoe_cmd_set), \
    SW_API_DEF(SW_API_PPPOE_CMD_GET, isis_pppoe_cmd_get), \
    SW_API_DEF(SW_API_PPPOE_STATUS_SET, isis_pppoe_status_set), \
    SW_API_DEF(SW_API_PPPOE_STATUS_GET, isis_pppoe_status_get), \
    SW_API_DEF(SW_API_PT_DHCP_SET, isis_port_dhcp_set), \
    SW_API_DEF(SW_API_PT_DHCP_GET, isis_port_dhcp_get), \
    SW_API_DEF(SW_API_ARP_CMD_SET, isis_arp_cmd_set), \
    SW_API_DEF(SW_API_ARP_CMD_GET, isis_arp_cmd_get), \
    SW_API_DEF(SW_API_EAPOL_CMD_SET, isis_eapol_cmd_set), \
    SW_API_DEF(SW_API_EAPOL_CMD_GET, isis_eapol_cmd_get), \
    SW_API_DEF(SW_API_EAPOL_STATUS_SET, isis_eapol_status_set), \
    SW_API_DEF(SW_API_EAPOL_STATUS_GET, isis_eapol_status_get), \
    SW_API_DEF(SW_API_RIPV1_STATUS_SET, isis_ripv1_status_set), \
    SW_API_DEF(SW_API_RIPV1_STATUS_GET, isis_ripv1_status_get), \
    SW_API_DEF(SW_API_PT_ARP_REQ_STATUS_SET, isis_port_arp_req_status_set), \
    SW_API_DEF(SW_API_PT_ARP_REQ_STATUS_GET, isis_port_arp_req_status_get), \
    SW_API_DEF(SW_API_PT_ARP_ACK_STATUS_SET, isis_port_arp_ack_status_set), \
    SW_API_DEF(SW_API_PT_ARP_ACK_STATUS_GET, isis_port_arp_ack_status_get), \
    SW_API_DEF(SW_API_PPPOE_SESSION_TABLE_ADD, isis_pppoe_session_table_add), \
    SW_API_DEF(SW_API_PPPOE_SESSION_TABLE_DEL, isis_pppoe_session_table_del), \
    SW_API_DEF(SW_API_PPPOE_SESSION_TABLE_GET, isis_pppoe_session_table_get), \
    SW_API_DEF(SW_API_PPPOE_SESSION_ID_SET, isis_pppoe_session_id_set), \
    SW_API_DEF(SW_API_PPPOE_SESSION_ID_GET, isis_pppoe_session_id_get), \
    SW_API_DEF(SW_API_INTR_MASK_SET, isis_intr_mask_set), \
    SW_API_DEF(SW_API_INTR_MASK_GET, isis_intr_mask_get), \
    SW_API_DEF(SW_API_INTR_STATUS_GET, isis_intr_status_get),   \
    SW_API_DEF(SW_API_INTR_STATUS_CLEAR, isis_intr_status_clear), \
    SW_API_DEF(SW_API_INTR_PORT_LINK_MASK_SET, isis_intr_port_link_mask_set), \
    SW_API_DEF(SW_API_INTR_PORT_LINK_MASK_GET, isis_intr_port_link_mask_get), \
    SW_API_DEF(SW_API_INTR_PORT_LINK_STATUS_GET, isis_intr_port_link_status_get),

#define MISC_API_PARAM \
    SW_API_DESC(SW_API_FRAME_MAX_SIZE_SET)  \
    SW_API_DESC(SW_API_FRAME_MAX_SIZE_GET)  \
    SW_API_DESC(SW_API_PT_UNK_UC_FILTER_SET)  \
    SW_API_DESC(SW_API_PT_UNK_UC_FILTER_GET)  \
    SW_API_DESC(SW_API_PT_UNK_MC_FILTER_SET)  \
    SW_API_DESC(SW_API_PT_UNK_MC_FILTER_GET)  \
    SW_API_DESC(SW_API_PT_BC_FILTER_SET)  \
    SW_API_DESC(SW_API_PT_BC_FILTER_GET)  \
    SW_API_DESC(SW_API_CPU_PORT_STATUS_SET)  \
    SW_API_DESC(SW_API_CPU_PORT_STATUS_GET)  \
    SW_API_DESC(SW_API_PPPOE_CMD_SET)  \
    SW_API_DESC(SW_API_PPPOE_CMD_GET)  \
    SW_API_DESC(SW_API_PPPOE_STATUS_SET)  \
    SW_API_DESC(SW_API_PPPOE_STATUS_GET)  \
    SW_API_DESC(SW_API_PT_DHCP_SET)  \
    SW_API_DESC(SW_API_PT_DHCP_GET)  \
    SW_API_DESC(SW_API_ARP_CMD_SET)  \
    SW_API_DESC(SW_API_ARP_CMD_GET)  \
    SW_API_DESC(SW_API_EAPOL_CMD_SET) \
    SW_API_DESC(SW_API_EAPOL_CMD_GET) \
    SW_API_DESC(SW_API_PPPOE_SESSION_ADD)  \
    SW_API_DESC(SW_API_PPPOE_SESSION_DEL)  \
    SW_API_DESC(SW_API_PPPOE_SESSION_GET)  \
    SW_API_DESC(SW_API_EAPOL_STATUS_SET)  \
    SW_API_DESC(SW_API_EAPOL_STATUS_GET)  \
    SW_API_DESC(SW_API_RIPV1_STATUS_SET)  \
    SW_API_DESC(SW_API_RIPV1_STATUS_GET) \
    SW_API_DESC(SW_API_PT_ARP_REQ_STATUS_SET) \
    SW_API_DESC(SW_API_PT_ARP_REQ_STATUS_GET) \
    SW_API_DESC(SW_API_PT_ARP_ACK_STATUS_SET) \
    SW_API_DESC(SW_API_PT_ARP_ACK_STATUS_GET) \
    SW_API_DESC(SW_API_PPPOE_SESSION_TABLE_ADD) \
    SW_API_DESC(SW_API_PPPOE_SESSION_TABLE_DEL) \
    SW_API_DESC(SW_API_PPPOE_SESSION_TABLE_GET) \
    SW_API_DESC(SW_API_PPPOE_SESSION_ID_SET) \
    SW_API_DESC(SW_API_PPPOE_SESSION_ID_GET) \
    SW_API_DESC(SW_API_INTR_MASK_SET) \
    SW_API_DESC(SW_API_INTR_MASK_GET) \
    SW_API_DESC(SW_API_INTR_STATUS_GET)   \
    SW_API_DESC(SW_API_INTR_STATUS_CLEAR) \
    SW_API_DESC(SW_API_INTR_PORT_LINK_MASK_SET) \
    SW_API_DESC(SW_API_INTR_PORT_LINK_MASK_GET) \
    SW_API_DESC(SW_API_INTR_PORT_LINK_STATUS_GET)
#else
#define MISC_API
#define MISC_API_PARAM
#endif


#ifdef IN_LED
#define LED_API \
    SW_API_DEF(SW_API_LED_PATTERN_SET, isis_led_ctrl_pattern_set), \
    SW_API_DEF(SW_API_LED_PATTERN_GET, isis_led_ctrl_pattern_get),

#define LED_API_PARAM \
    SW_API_DESC(SW_API_LED_PATTERN_SET) \
    SW_API_DESC(SW_API_LED_PATTERN_GET)
#else
#define LED_API
#define LED_API_PARAM
#endif

#ifdef IN_COSMAP
#define COSMAP_API \
    SW_API_DEF(SW_API_COSMAP_DSCP_TO_PRI_SET, isis_cosmap_dscp_to_pri_set), \
    SW_API_DEF(SW_API_COSMAP_DSCP_TO_PRI_GET, isis_cosmap_dscp_to_pri_get), \
    SW_API_DEF(SW_API_COSMAP_DSCP_TO_DP_SET, isis_cosmap_dscp_to_dp_set), \
    SW_API_DEF(SW_API_COSMAP_DSCP_TO_DP_GET, isis_cosmap_dscp_to_dp_get), \
    SW_API_DEF(SW_API_COSMAP_UP_TO_PRI_SET, isis_cosmap_up_to_pri_set), \
    SW_API_DEF(SW_API_COSMAP_UP_TO_PRI_GET, isis_cosmap_up_to_pri_get), \
    SW_API_DEF(SW_API_COSMAP_UP_TO_DP_SET, isis_cosmap_up_to_dp_set), \
    SW_API_DEF(SW_API_COSMAP_UP_TO_DP_GET, isis_cosmap_up_to_dp_get), \
    SW_API_DEF(SW_API_COSMAP_PRI_TO_QU_SET, isis_cosmap_pri_to_queue_set), \
    SW_API_DEF(SW_API_COSMAP_PRI_TO_QU_GET, isis_cosmap_pri_to_queue_get), \
    SW_API_DEF(SW_API_COSMAP_PRI_TO_EHQU_SET, isis_cosmap_pri_to_ehqueue_set), \
    SW_API_DEF(SW_API_COSMAP_PRI_TO_EHQU_GET, isis_cosmap_pri_to_ehqueue_get),

#define COSMAP_API_PARAM \
    SW_API_DESC(SW_API_COSMAP_DSCP_TO_PRI_SET)  \
    SW_API_DESC(SW_API_COSMAP_DSCP_TO_PRI_GET)  \
    SW_API_DESC(SW_API_COSMAP_DSCP_TO_DP_SET)  \
    SW_API_DESC(SW_API_COSMAP_DSCP_TO_DP_GET)  \
    SW_API_DESC(SW_API_COSMAP_UP_TO_PRI_SET)  \
    SW_API_DESC(SW_API_COSMAP_UP_TO_PRI_GET)  \
    SW_API_DESC(SW_API_COSMAP_UP_TO_DP_SET)  \
    SW_API_DESC(SW_API_COSMAP_UP_TO_DP_GET)  \
    SW_API_DESC(SW_API_COSMAP_PRI_TO_QU_SET)  \
    SW_API_DESC(SW_API_COSMAP_PRI_TO_QU_GET)  \
    SW_API_DESC(SW_API_COSMAP_PRI_TO_EHQU_SET) \
    SW_API_DESC(SW_API_COSMAP_PRI_TO_EHQU_GET)
#else
#define COSMAP_API
#define COSMAP_API_PARAM
#endif

#ifdef IN_SEC
#define SEC_API \
    SW_API_DEF(SW_API_SEC_NORM_SET, isis_sec_norm_item_set), \
    SW_API_DEF(SW_API_SEC_NORM_GET, isis_sec_norm_item_get),

#define SEC_API_PARAM \
    SW_API_DESC(SW_API_SEC_NORM_SET) \
    SW_API_DESC(SW_API_SEC_NORM_GET)
#else
#define SEC_API
#define SEC_API_PARAM
#endif

#ifdef IN_IP
#define IP_API \
    SW_API_DEF(SW_API_IP_HOST_ADD, isis_ip_host_add), \
    SW_API_DEF(SW_API_IP_HOST_DEL, isis_ip_host_del), \
    SW_API_DEF(SW_API_IP_HOST_GET, isis_ip_host_get), \
    SW_API_DEF(SW_API_IP_HOST_NEXT, isis_ip_host_next), \
    SW_API_DEF(SW_API_IP_HOST_COUNTER_BIND, isis_ip_host_counter_bind), \
    SW_API_DEF(SW_API_IP_HOST_PPPOE_BIND, isis_ip_host_pppoe_bind), \
    SW_API_DEF(SW_API_IP_PT_ARP_LEARN_SET, isis_ip_pt_arp_learn_set), \
    SW_API_DEF(SW_API_IP_PT_ARP_LEARN_GET, isis_ip_pt_arp_learn_get), \
    SW_API_DEF(SW_API_IP_ARP_LEARN_SET, isis_ip_arp_learn_set), \
    SW_API_DEF(SW_API_IP_ARP_LEARN_GET, isis_ip_arp_learn_get), \
    SW_API_DEF(SW_API_IP_SOURCE_GUARD_SET, isis_ip_source_guard_set), \
    SW_API_DEF(SW_API_IP_SOURCE_GUARD_GET, isis_ip_source_guard_get), \
    SW_API_DEF(SW_API_IP_ARP_GUARD_SET, isis_ip_arp_guard_set), \
    SW_API_DEF(SW_API_IP_ARP_GUARD_GET, isis_ip_arp_guard_get), \
    SW_API_DEF(SW_API_IP_ROUTE_STATUS_SET, isis_ip_route_status_set), \
    SW_API_DEF(SW_API_IP_ROUTE_STATUS_GET, isis_ip_route_status_get), \
    SW_API_DEF(SW_API_IP_INTF_ENTRY_ADD, isis_ip_intf_entry_add), \
    SW_API_DEF(SW_API_IP_INTF_ENTRY_DEL, isis_ip_intf_entry_del), \
    SW_API_DEF(SW_API_IP_INTF_ENTRY_NEXT, isis_ip_intf_entry_next), \
    SW_API_DEF(SW_API_IP_UNK_SOURCE_CMD_SET, isis_ip_unk_source_cmd_set),  \
    SW_API_DEF(SW_API_IP_UNK_SOURCE_CMD_GET, isis_ip_unk_source_cmd_get), \
    SW_API_DEF(SW_API_ARP_UNK_SOURCE_CMD_SET, isis_arp_unk_source_cmd_set), \
    SW_API_DEF(SW_API_ARP_UNK_SOURCE_CMD_GET, isis_arp_unk_source_cmd_get), \
    SW_API_DEF(SW_API_IP_AGE_TIME_SET, isis_ip_age_time_set), \
    SW_API_DEF(SW_API_IP_AGE_TIME_GET, isis_ip_age_time_get), \
    SW_API_DEF(SW_API_WCMP_HASH_MODE_SET, isis_ip_wcmp_hash_mode_set), \
    SW_API_DEF(SW_API_WCMP_HASH_MODE_GET, isis_ip_wcmp_hash_mode_get),

#define IP_API_PARAM \
    SW_API_DESC(SW_API_IP_HOST_ADD) \
    SW_API_DESC(SW_API_IP_HOST_DEL) \
    SW_API_DESC(SW_API_IP_HOST_GET) \
    SW_API_DESC(SW_API_IP_HOST_NEXT) \
    SW_API_DESC(SW_API_IP_HOST_COUNTER_BIND) \
    SW_API_DESC(SW_API_IP_HOST_PPPOE_BIND) \
    SW_API_DESC(SW_API_IP_PT_ARP_LEARN_SET) \
    SW_API_DESC(SW_API_IP_PT_ARP_LEARN_GET) \
    SW_API_DESC(SW_API_IP_ARP_LEARN_SET) \
    SW_API_DESC(SW_API_IP_ARP_LEARN_GET) \
    SW_API_DESC(SW_API_IP_SOURCE_GUARD_SET) \
    SW_API_DESC(SW_API_IP_SOURCE_GUARD_GET) \
    SW_API_DESC(SW_API_IP_ARP_GUARD_SET) \
    SW_API_DESC(SW_API_IP_ARP_GUARD_GET) \
    SW_API_DESC(SW_API_IP_ROUTE_STATUS_SET) \
    SW_API_DESC(SW_API_IP_ROUTE_STATUS_GET) \
    SW_API_DESC(SW_API_IP_INTF_ENTRY_ADD)  \
    SW_API_DESC(SW_API_IP_INTF_ENTRY_DEL)  \
    SW_API_DESC(SW_API_IP_INTF_ENTRY_NEXT) \
    SW_API_DESC(SW_API_IP_UNK_SOURCE_CMD_SET)  \
    SW_API_DESC(SW_API_IP_UNK_SOURCE_CMD_GET)  \
    SW_API_DESC(SW_API_ARP_UNK_SOURCE_CMD_SET) \
    SW_API_DESC(SW_API_ARP_UNK_SOURCE_CMD_GET) \
    SW_API_DESC(SW_API_IP_AGE_TIME_SET) \
    SW_API_DESC(SW_API_IP_AGE_TIME_GET) \
    SW_API_DESC(SW_API_WCMP_HASH_MODE_SET) \
    SW_API_DESC(SW_API_WCMP_HASH_MODE_GET)

#else
#define IP_API
#define IP_API_PARAM
#endif

#ifdef IN_NAT
#define NAT_API \
    SW_API_DEF(SW_API_NAT_ADD, isis_nat_add), \
    SW_API_DEF(SW_API_NAT_DEL, isis_nat_del), \
    SW_API_DEF(SW_API_NAT_GET, isis_nat_get), \
    SW_API_DEF(SW_API_NAT_NEXT, isis_nat_next), \
    SW_API_DEF(SW_API_NAT_COUNTER_BIND, isis_nat_counter_bind), \
    SW_API_DEF(SW_API_NAPT_ADD, isis_napt_add), \
    SW_API_DEF(SW_API_NAPT_DEL, isis_napt_del), \
    SW_API_DEF(SW_API_NAPT_GET, isis_napt_get), \
    SW_API_DEF(SW_API_NAPT_NEXT, isis_napt_next), \
    SW_API_DEF(SW_API_NAPT_COUNTER_BIND, isis_napt_counter_bind), \
    SW_API_DEF(SW_API_NAT_STATUS_SET, isis_nat_status_set), \
    SW_API_DEF(SW_API_NAT_STATUS_GET, isis_nat_status_get), \
    SW_API_DEF(SW_API_NAT_HASH_MODE_SET, isis_nat_hash_mode_set), \
    SW_API_DEF(SW_API_NAT_HASH_MODE_GET, isis_nat_hash_mode_get), \
    SW_API_DEF(SW_API_NAPT_STATUS_SET, isis_napt_status_set), \
    SW_API_DEF(SW_API_NAPT_STATUS_GET, isis_napt_status_get), \
    SW_API_DEF(SW_API_NAPT_MODE_SET, isis_napt_mode_set), \
    SW_API_DEF(SW_API_NAPT_MODE_GET, isis_napt_mode_get), \
    SW_API_DEF(SW_API_PRV_BASE_ADDR_SET, isis_nat_prv_base_addr_set), \
    SW_API_DEF(SW_API_PRV_BASE_ADDR_GET, isis_nat_prv_base_addr_get), \
    SW_API_DEF(SW_API_PRV_ADDR_MODE_SET, isis_nat_prv_addr_mode_set), \
    SW_API_DEF(SW_API_PRV_ADDR_MODE_GET, isis_nat_prv_addr_mode_get), \
    SW_API_DEF(SW_API_PUB_ADDR_ENTRY_ADD, isis_nat_pub_addr_add), \
    SW_API_DEF(SW_API_PUB_ADDR_ENTRY_DEL, isis_nat_pub_addr_del), \
    SW_API_DEF(SW_API_PUB_ADDR_ENTRY_NEXT, isis_nat_pub_addr_next), \
    SW_API_DEF(SW_API_NAT_UNK_SESSION_CMD_SET, isis_nat_unk_session_cmd_set), \
    SW_API_DEF(SW_API_NAT_UNK_SESSION_CMD_GET, isis_nat_unk_session_cmd_get), \
    SW_API_DEF(SW_API_NAT_GLOBAL_SET, isis_nat_global_set),

#define NAT_API_PARAM \
    SW_API_DESC(SW_API_NAT_ADD) \
    SW_API_DESC(SW_API_NAT_DEL) \
    SW_API_DESC(SW_API_NAT_GET) \
    SW_API_DESC(SW_API_NAT_NEXT) \
    SW_API_DESC(SW_API_NAT_COUNTER_BIND) \
    SW_API_DESC(SW_API_NAPT_ADD) \
    SW_API_DESC(SW_API_NAPT_DEL) \
    SW_API_DESC(SW_API_NAPT_GET) \
    SW_API_DESC(SW_API_NAPT_NEXT) \
    SW_API_DESC(SW_API_NAPT_COUNTER_BIND) \
    SW_API_DESC(SW_API_NAT_STATUS_SET) \
    SW_API_DESC(SW_API_NAT_STATUS_GET) \
    SW_API_DESC(SW_API_NAT_HASH_MODE_SET) \
    SW_API_DESC(SW_API_NAT_HASH_MODE_GET) \
    SW_API_DESC(SW_API_NAPT_STATUS_SET) \
    SW_API_DESC(SW_API_NAPT_STATUS_GET) \
    SW_API_DESC(SW_API_NAPT_MODE_SET) \
    SW_API_DESC(SW_API_NAPT_MODE_GET) \
    SW_API_DESC(SW_API_PRV_BASE_ADDR_SET) \
    SW_API_DESC(SW_API_PRV_BASE_ADDR_GET) \
    SW_API_DESC(SW_API_PRV_ADDR_MODE_SET) \
    SW_API_DESC(SW_API_PRV_ADDR_MODE_GET) \
    SW_API_DESC(SW_API_PUB_ADDR_ENTRY_ADD) \
    SW_API_DESC(SW_API_PUB_ADDR_ENTRY_DEL) \
    SW_API_DESC(SW_API_PUB_ADDR_ENTRY_NEXT) \
    SW_API_DESC(SW_API_NAT_UNK_SESSION_CMD_SET) \
    SW_API_DESC(SW_API_NAT_UNK_SESSION_CMD_GET) \
    SW_API_DESC(SW_API_NAT_GLOBAL_SET)
#else
#define NAT_API
#define NAT_API_PARAM
#endif

#ifdef IN_TRUNK
#define TRUNK_API \
    SW_API_DEF(SW_API_TRUNK_GROUP_SET, isis_trunk_group_set), \
    SW_API_DEF(SW_API_TRUNK_GROUP_GET, isis_trunk_group_get), \
    SW_API_DEF(SW_API_TRUNK_HASH_SET, isis_trunk_hash_mode_set), \
    SW_API_DEF(SW_API_TRUNK_HASH_GET, isis_trunk_hash_mode_get), \
    SW_API_DEF(SW_API_TRUNK_MAN_SA_SET, isis_trunk_manipulate_sa_set), \
    SW_API_DEF(SW_API_TRUNK_MAN_SA_GET, isis_trunk_manipulate_sa_get),

#define TRUNK_API_PARAM \
    SW_API_DESC(SW_API_TRUNK_GROUP_SET) \
    SW_API_DESC(SW_API_TRUNK_GROUP_GET) \
    SW_API_DESC(SW_API_TRUNK_HASH_SET)  \
    SW_API_DESC(SW_API_TRUNK_HASH_GET)  \
    SW_API_DESC(SW_API_TRUNK_MAN_SA_SET)\
    SW_API_DESC(SW_API_TRUNK_MAN_SA_GET)
#else
#define TRUNK_API
#define TRUNK_API_PARAM
#endif

#ifdef IN_INTERFACECONTROL
#define INTERFACECTRL_API \
    SW_API_DEF(SW_API_MAC_MODE_SET, isis_interface_mac_mode_set), \
    SW_API_DEF(SW_API_MAC_MODE_GET, isis_interface_mac_mode_get), \
    SW_API_DEF(SW_API_PORT_3AZ_STATUS_SET, isis_port_3az_status_set), \
    SW_API_DEF(SW_API_PORT_3AZ_STATUS_GET, isis_port_3az_status_get), \
    SW_API_DEF(SW_API_PHY_MODE_SET, isis_interface_phy_mode_set), \
    SW_API_DEF(SW_API_PHY_MODE_GET, isis_interface_phy_mode_get),

#define INTERFACECTRL_API_PARAM \
    SW_API_DESC(SW_API_MAC_MODE_SET)  \
    SW_API_DESC(SW_API_MAC_MODE_GET)  \
    SW_API_DESC(SW_API_PORT_3AZ_STATUS_SET)  \
    SW_API_DESC(SW_API_PORT_3AZ_STATUS_GET)  \
    SW_API_DESC(SW_API_PHY_MODE_SET)  \
    SW_API_DESC(SW_API_PHY_MODE_GET)

#else
#define INTERFACECTRL_API
#define INTERFACECTRL_API_PARAM
#endif

#define REG_API \
    SW_API_DEF(SW_API_PHY_GET, isis_phy_get), \
    SW_API_DEF(SW_API_PHY_SET, isis_phy_set), \
    SW_API_DEF(SW_API_REG_GET, isis_reg_get), \
    SW_API_DEF(SW_API_REG_SET, isis_reg_set), \
    SW_API_DEF(SW_API_REG_FIELD_GET, isis_reg_field_get), \
    SW_API_DEF(SW_API_REG_FIELD_SET, isis_reg_field_set), \
    SW_API_DEF(SW_API_REG_DUMP, isis_regsiter_dump), \
    SW_API_DEF(SW_API_DBG_REG_DUMP, isis_debug_regsiter_dump),

#define REG_API_PARAM \
    SW_API_DESC(SW_API_PHY_GET) \
    SW_API_DESC(SW_API_PHY_SET) \
    SW_API_DESC(SW_API_REG_GET) \
    SW_API_DESC(SW_API_REG_SET) \
    SW_API_DESC(SW_API_REG_FIELD_GET) \
    SW_API_DESC(SW_API_REG_FIELD_SET) \
    SW_API_DESC(SW_API_REG_DUMP)  \
    SW_API_DESC(SW_API_DBG_REG_DUMP)


#define SSDK_API \
    SW_API_DEF(SW_API_SWITCH_RESET, isis_reset), \
    SW_API_DEF(SW_API_SSDK_CFG, hsl_ssdk_cfg), \
    PORTCONTROL_API \
    VLAN_API \
    PORTVLAN_API \
    FDB_API    \
    ACL_API    \
    QOS_API    \
    IGMP_API   \
    LEAKY_API  \
    MIRROR_API \
    RATE_API \
    STP_API  \
    MIB_API  \
    MISC_API \
    LED_API  \
    COSMAP_API \
    SEC_API  \
    IP_API \
    NAT_API \
    TRUNK_API \
    INTERFACECTRL_API \
    REG_API  \
    SW_API_DEF(SW_API_MAX, NULL),


#define SSDK_PARAM  \
    SW_PARAM_DEF(SW_API_SWITCH_RESET, SW_UINT32, 4, SW_PARAM_IN, "Dev ID"), \
    SW_PARAM_DEF(SW_API_SSDK_CFG, SW_UINT32, 4, SW_PARAM_IN, "Dev ID"), \
    SW_PARAM_DEF(SW_API_SSDK_CFG, SW_SSDK_CFG, sizeof(ssdk_cfg_t), SW_PARAM_PTR|SW_PARAM_OUT, "ssdk configuration"), \
    MIB_API_PARAM    \
    LEAKY_API_PARAM  \
    MISC_API_PARAM   \
    IGMP_API_PARAM   \
    MIRROR_API_PARAM \
    PORTCONTROL_API_PARAM \
    PORTVLAN_API_PARAM    \
    VLAN_API_PARAM \
    FDB_API_PARAM  \
    QOS_API_PARAM  \
    RATE_API_PARAM \
    STP_API_PARAM  \
    ACL_API_PARAM  \
    LED_API_PARAM  \
    COSMAP_API_PARAM \
    SEC_API_PARAM  \
    IP_API_PARAM \
    NAT_API_PARAM \
    TRUNK_API_PARAM \
    INTERFACECTRL_API_PARAM \
    REG_API_PARAM  \
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

#endif                          /* _ISIS_API_H_ */

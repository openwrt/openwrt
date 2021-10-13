/*
 * Copyright (c) 2012, 2015-2019, 2021, The Linux Foundation. All rights reserved.
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
#ifndef _FAL_API_H_
#define _FAL_API_H_

#ifdef __cplusplus
extern "C" {
#endif                          /* __cplusplus */
/*qca808x_end*/
#ifdef IN_PORTCONTROL
#ifndef IN_PORTCONTROL_MINI
/*qca808x_start*/
#define PORTCONTROL_API \
    SW_API_DEF(SW_API_PT_DUPLEX_GET, fal_port_duplex_get), \
    SW_API_DEF(SW_API_PT_DUPLEX_SET, fal_port_duplex_set), \
    SW_API_DEF(SW_API_PT_SPEED_GET, fal_port_speed_get), \
    SW_API_DEF(SW_API_PT_SPEED_SET, fal_port_speed_set), \
    SW_API_DEF(SW_API_PT_AN_GET, fal_port_autoneg_status_get), \
    SW_API_DEF(SW_API_PT_AN_ENABLE, fal_port_autoneg_enable), \
    SW_API_DEF(SW_API_PT_AN_RESTART, fal_port_autoneg_restart), \
    SW_API_DEF(SW_API_PT_AN_ADV_GET, fal_port_autoneg_adv_get), \
    SW_API_DEF(SW_API_PT_AN_ADV_SET, fal_port_autoneg_adv_set), \
/*qca808x_end*/\
    SW_API_DEF(SW_API_PT_HDR_SET, fal_port_hdr_status_set), \
    SW_API_DEF(SW_API_PT_HDR_GET, fal_port_hdr_status_get), \
    SW_API_DEF(SW_API_PT_FLOWCTRL_SET, fal_port_flowctrl_set), \
    SW_API_DEF(SW_API_PT_FLOWCTRL_GET, fal_port_flowctrl_get), \
    SW_API_DEF(SW_API_PT_FLOWCTRL_MODE_SET, fal_port_flowctrl_forcemode_set), \
    SW_API_DEF(SW_API_PT_FLOWCTRL_MODE_GET, fal_port_flowctrl_forcemode_get), \
    SW_API_DEF(SW_API_PT_POWERSAVE_SET, fal_port_powersave_set), \
    SW_API_DEF(SW_API_PT_POWERSAVE_GET, fal_port_powersave_get), \
/*qca808x_start*/\
    SW_API_DEF(SW_API_PT_HIBERNATE_SET, fal_port_hibernate_set), \
    SW_API_DEF(SW_API_PT_HIBERNATE_GET, fal_port_hibernate_get), \
    SW_API_DEF(SW_API_PT_CDT, fal_port_cdt),  \
/*qca808x_end*/\
    SW_API_DEF(SW_API_PT_TXHDR_SET, fal_port_txhdr_mode_set), \
    SW_API_DEF(SW_API_PT_TXHDR_GET, fal_port_txhdr_mode_get), \
    SW_API_DEF(SW_API_PT_RXHDR_SET, fal_port_rxhdr_mode_set), \
    SW_API_DEF(SW_API_PT_RXHDR_GET, fal_port_rxhdr_mode_get), \
    SW_API_DEF(SW_API_HEADER_TYPE_SET, fal_header_type_set),  \
    SW_API_DEF(SW_API_HEADER_TYPE_GET, fal_header_type_get),  \
    SW_API_DEF(SW_API_TXMAC_STATUS_SET, fal_port_txmac_status_set), \
    SW_API_DEF(SW_API_TXMAC_STATUS_GET, fal_port_txmac_status_get), \
    SW_API_DEF(SW_API_RXMAC_STATUS_SET, fal_port_rxmac_status_set), \
    SW_API_DEF(SW_API_RXMAC_STATUS_GET, fal_port_rxmac_status_get), \
    SW_API_DEF(SW_API_TXFC_STATUS_SET, fal_port_txfc_status_set),   \
    SW_API_DEF(SW_API_TXFC_STATUS_GET, fal_port_txfc_status_get),   \
    SW_API_DEF(SW_API_RXFC_STATUS_SET, fal_port_rxfc_status_set),   \
    SW_API_DEF(SW_API_RXFC_STATUS_GET, fal_port_rxfc_status_get),   \
    SW_API_DEF(SW_API_BP_STATUS_SET, fal_port_bp_status_set),   \
    SW_API_DEF(SW_API_BP_STATUS_GET, fal_port_bp_status_get),   \
    SW_API_DEF(SW_API_PT_LINK_MODE_SET, fal_port_link_forcemode_set),   \
    SW_API_DEF(SW_API_PT_LINK_MODE_GET, fal_port_link_forcemode_get), \
/*qca808x_start*/\
    SW_API_DEF(SW_API_PT_LINK_STATUS_GET, fal_port_link_status_get), \
/*qca808x_end*/\
    SW_API_DEF(SW_API_PT_MAC_LOOPBACK_SET, fal_port_mac_loopback_set), \
    SW_API_DEF(SW_API_PT_MAC_LOOPBACK_GET, fal_port_mac_loopback_get), \
    SW_API_DEF(SW_API_PTS_LINK_STATUS_GET, fal_ports_link_status_get), \
    SW_API_DEF(SW_API_PT_CONGESTION_DROP_SET, fal_port_congestion_drop_set), \
    SW_API_DEF(SW_API_PT_CONGESTION_DROP_GET, fal_port_congestion_drop_get), \
    SW_API_DEF(SW_API_PT_RING_FLOW_CTRL_THRES_SET, fal_ring_flow_ctrl_thres_set), \
    SW_API_DEF(SW_API_PT_RING_FLOW_CTRL_THRES_GET, fal_ring_flow_ctrl_thres_get), \
/*qca808x_start*/\
    SW_API_DEF(SW_API_PT_8023AZ_SET, fal_port_8023az_set), \
    SW_API_DEF(SW_API_PT_8023AZ_GET, fal_port_8023az_get), \
    SW_API_DEF(SW_API_PT_MDIX_SET, fal_port_mdix_set), \
    SW_API_DEF(SW_API_PT_MDIX_GET, fal_port_mdix_get), \
    SW_API_DEF(SW_API_PT_MDIX_STATUS_GET, fal_port_mdix_status_get), \
/*qca808x_end*/\
    SW_API_DEF(SW_API_PT_COMBO_PREFER_MEDIUM_SET, fal_port_combo_prefer_medium_set), \
    SW_API_DEF(SW_API_PT_COMBO_PREFER_MEDIUM_GET, fal_port_combo_prefer_medium_get), \
    SW_API_DEF(SW_API_PT_COMBO_MEDIUM_STATUS_GET, fal_port_combo_medium_status_get), \
    SW_API_DEF(SW_API_PT_COMBO_FIBER_MODE_SET, fal_port_combo_fiber_mode_set), \
    SW_API_DEF(SW_API_PT_COMBO_FIBER_MODE_GET, fal_port_combo_fiber_mode_get), \
/*qca808x_start*/\
    SW_API_DEF(SW_API_PT_LOCAL_LOOPBACK_SET, fal_port_local_loopback_set), \
    SW_API_DEF(SW_API_PT_LOCAL_LOOPBACK_GET, fal_port_local_loopback_get), \
    SW_API_DEF(SW_API_PT_REMOTE_LOOPBACK_SET, fal_port_remote_loopback_set), \
    SW_API_DEF(SW_API_PT_REMOTE_LOOPBACK_GET, fal_port_remote_loopback_get), \
    SW_API_DEF(SW_API_PT_RESET, fal_port_reset), \
    SW_API_DEF(SW_API_PT_POWER_OFF, fal_port_power_off), \
    SW_API_DEF(SW_API_PT_POWER_ON, fal_port_power_on), \
    SW_API_DEF(SW_API_PT_MAGIC_FRAME_MAC_SET, fal_port_magic_frame_mac_set), \
    SW_API_DEF(SW_API_PT_MAGIC_FRAME_MAC_GET, fal_port_magic_frame_mac_get), \
    SW_API_DEF(SW_API_PT_PHY_ID_GET, fal_port_phy_id_get), \
    SW_API_DEF(SW_API_PT_WOL_STATUS_SET, fal_port_wol_status_set), \
    SW_API_DEF(SW_API_PT_WOL_STATUS_GET, fal_port_wol_status_get), \
/*qca808x_end*/\
    SW_API_DEF(SW_API_PT_INTERFACE_MODE_SET, fal_port_interface_mode_set), \
    SW_API_DEF(SW_API_PT_INTERFACE_MODE_APPLY, fal_port_interface_mode_apply), \
    SW_API_DEF(SW_API_PT_INTERFACE_MODE_GET, fal_port_interface_mode_get), \
/*qca808x_start*/\
    SW_API_DEF(SW_API_PT_INTERFACE_MODE_STATUS_GET, fal_port_interface_mode_status_get), \
    SW_API_DEF(SW_API_DEBUG_PHYCOUNTER_SET, fal_debug_phycounter_set), \
    SW_API_DEF(SW_API_DEBUG_PHYCOUNTER_GET, fal_debug_phycounter_get), \
    SW_API_DEF(SW_API_DEBUG_PHYCOUNTER_SHOW, fal_debug_phycounter_show),\
/*qca808x_end*/\
    SW_API_DEF(SW_API_PT_MTU_SET, fal_port_mtu_set), \
    SW_API_DEF(SW_API_PT_MTU_GET, fal_port_mtu_get), \
    SW_API_DEF(SW_API_PT_MRU_SET, fal_port_mru_set), \
    SW_API_DEF(SW_API_PT_MRU_GET, fal_port_mru_get), \
    SW_API_DEF(SW_API_PT_SOURCE_FILTER_GET, fal_port_source_filter_status_get), \
    SW_API_DEF(SW_API_PT_SOURCE_FILTER_SET, fal_port_source_filter_enable), \
    SW_API_DEF(SW_API_PT_FRAME_MAX_SIZE_GET, fal_port_max_frame_size_get), \
    SW_API_DEF(SW_API_PT_FRAME_MAX_SIZE_SET, fal_port_max_frame_size_set), \
    SW_API_DEF(SW_API_PT_INTERFACE_3AZ_STATUS_SET, fal_port_interface_3az_status_set), \
    SW_API_DEF(SW_API_PT_INTERFACE_3AZ_STATUS_GET, fal_port_interface_3az_status_get), \
    SW_API_DEF(SW_API_PT_PROMISC_MODE_SET, fal_port_promisc_mode_set),  \
    SW_API_DEF(SW_API_PT_PROMISC_MODE_GET, fal_port_promisc_mode_get), \
    SW_API_DEF(SW_API_PT_INTERFACE_EEE_CFG_SET, fal_port_interface_eee_cfg_set),  \
    SW_API_DEF(SW_API_PT_INTERFACE_EEE_CFG_GET, fal_port_interface_eee_cfg_get),  \
    SW_API_DEF(SW_API_PT_SOURCE_FILTER_CONFIG_GET, fal_port_source_filter_config_get), \
    SW_API_DEF(SW_API_PT_SOURCE_FILTER_CONFIG_SET, fal_port_source_filter_config_set),  \
    SW_API_DEF(SW_API_PT_SWITCH_PORT_LOOPBACK_SET, fal_switch_port_loopback_set),  \
    SW_API_DEF(SW_API_PT_SWITCH_PORT_LOOPBACK_GET, fal_switch_port_loopback_get),
/*qca808x_start*/
/*end of PORTCONTROL_API*/
#define PORTCONTROL_API_PARAM \
    SW_API_DESC(SW_API_PT_DUPLEX_GET) \
    SW_API_DESC(SW_API_PT_DUPLEX_SET) \
    SW_API_DESC(SW_API_PT_SPEED_GET)  \
    SW_API_DESC(SW_API_PT_SPEED_SET)  \
    SW_API_DESC(SW_API_PT_AN_GET) \
    SW_API_DESC(SW_API_PT_AN_ENABLE) \
    SW_API_DESC(SW_API_PT_AN_RESTART) \
    SW_API_DESC(SW_API_PT_AN_ADV_GET) \
    SW_API_DESC(SW_API_PT_AN_ADV_SET) \
/*qca808x_end*/\
    SW_API_DESC(SW_API_PT_HDR_SET) \
    SW_API_DESC(SW_API_PT_HDR_GET) \
    SW_API_DESC(SW_API_PT_FLOWCTRL_SET) \
    SW_API_DESC(SW_API_PT_FLOWCTRL_GET) \
    SW_API_DESC(SW_API_PT_FLOWCTRL_MODE_SET) \
    SW_API_DESC(SW_API_PT_FLOWCTRL_MODE_GET) \
    SW_API_DESC(SW_API_PT_POWERSAVE_SET) \
    SW_API_DESC(SW_API_PT_POWERSAVE_GET) \
/*qca808x_start*/\
    SW_API_DESC(SW_API_PT_HIBERNATE_SET) \
    SW_API_DESC(SW_API_PT_HIBERNATE_GET) \
    SW_API_DESC(SW_API_PT_CDT) \
/*qca808x_end*/\
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
/*qca808x_start*/\
    SW_API_DESC(SW_API_PT_LINK_STATUS_GET) \
/*qca808x_end*/\
    SW_API_DESC(SW_API_PT_MAC_LOOPBACK_SET) \
    SW_API_DESC(SW_API_PT_MAC_LOOPBACK_GET) \
    SW_API_DESC(SW_API_PTS_LINK_STATUS_GET) \
    SW_API_DESC(SW_API_PT_CONGESTION_DROP_SET) \
    SW_API_DESC(SW_API_PT_CONGESTION_DROP_GET) \
    SW_API_DESC(SW_API_PT_RING_FLOW_CTRL_THRES_SET) \
    SW_API_DESC(SW_API_PT_RING_FLOW_CTRL_THRES_GET) \
/*qca808x_start*/\
    SW_API_DESC(SW_API_PT_8023AZ_SET) \
    SW_API_DESC(SW_API_PT_8023AZ_GET) \
    SW_API_DESC(SW_API_PT_MDIX_SET) \
    SW_API_DESC(SW_API_PT_MDIX_GET) \
    SW_API_DESC(SW_API_PT_MDIX_STATUS_GET) \
/*qca808x_end*/\
    SW_API_DESC(SW_API_PT_COMBO_PREFER_MEDIUM_SET) \
    SW_API_DESC(SW_API_PT_COMBO_PREFER_MEDIUM_GET) \
    SW_API_DESC(SW_API_PT_COMBO_MEDIUM_STATUS_GET) \
    SW_API_DESC(SW_API_PT_COMBO_FIBER_MODE_SET) \
    SW_API_DESC(SW_API_PT_COMBO_FIBER_MODE_GET) \
/*qca808x_start*/\
    SW_API_DESC(SW_API_PT_LOCAL_LOOPBACK_SET) \
    SW_API_DESC(SW_API_PT_LOCAL_LOOPBACK_GET) \
    SW_API_DESC(SW_API_PT_REMOTE_LOOPBACK_SET) \
    SW_API_DESC(SW_API_PT_REMOTE_LOOPBACK_GET) \
    SW_API_DESC(SW_API_PT_RESET) \
    SW_API_DESC(SW_API_PT_POWER_OFF) \
    SW_API_DESC(SW_API_PT_POWER_ON) \
    SW_API_DESC(SW_API_PT_MAGIC_FRAME_MAC_SET) \
    SW_API_DESC(SW_API_PT_MAGIC_FRAME_MAC_GET) \
    SW_API_DESC(SW_API_PT_PHY_ID_GET) \
    SW_API_DESC(SW_API_PT_WOL_STATUS_SET) \
    SW_API_DESC(SW_API_PT_WOL_STATUS_GET) \
/*qca808x_end*/\
    SW_API_DESC(SW_API_PT_INTERFACE_MODE_SET) \
    SW_API_DESC(SW_API_PT_INTERFACE_MODE_GET) \
    SW_API_DESC(SW_API_PT_INTERFACE_MODE_APPLY) \
/*qca808x_start*/\
    SW_API_DESC(SW_API_PT_INTERFACE_MODE_STATUS_GET) \
    SW_API_DESC(SW_API_DEBUG_PHYCOUNTER_SET) \
    SW_API_DESC(SW_API_DEBUG_PHYCOUNTER_GET) \
    SW_API_DESC(SW_API_DEBUG_PHYCOUNTER_SHOW) \
/*qca808x_end*/\
    SW_API_DESC(SW_API_PT_MTU_SET) \
    SW_API_DESC(SW_API_PT_MTU_GET) \
    SW_API_DESC(SW_API_PT_MRU_SET) \
    SW_API_DESC(SW_API_PT_MRU_GET) \
    SW_API_DESC(SW_API_PT_SOURCE_FILTER_GET) \
    SW_API_DESC(SW_API_PT_SOURCE_FILTER_SET) \
    SW_API_DESC(SW_API_PT_FRAME_MAX_SIZE_GET) \
    SW_API_DESC(SW_API_PT_FRAME_MAX_SIZE_SET) \
    SW_API_DESC(SW_API_PT_INTERFACE_3AZ_STATUS_SET)  \
    SW_API_DESC(SW_API_PT_INTERFACE_3AZ_STATUS_GET) \
    SW_API_DESC(SW_API_PT_PROMISC_MODE_SET)  \
    SW_API_DESC(SW_API_PT_PROMISC_MODE_GET) \
    SW_API_DESC(SW_API_PT_INTERFACE_EEE_CFG_SET)  \
    SW_API_DESC(SW_API_PT_INTERFACE_EEE_CFG_GET)  \
    SW_API_DESC(SW_API_PT_SOURCE_FILTER_CONFIG_GET) \
    SW_API_DESC(SW_API_PT_SOURCE_FILTER_CONFIG_SET)  \
    SW_API_DESC(SW_API_PT_SWITCH_PORT_LOOPBACK_SET)  \
    SW_API_DESC(SW_API_PT_SWITCH_PORT_LOOPBACK_GET)
/*qca808x_start*/
/*end of PORTCONTROL_API_PARAM*/
/*qca808x_end*/
#else
#define PORTCONTROL_API \
    SW_API_DEF(SW_API_PT_DUPLEX_SET, fal_port_duplex_set), \
    SW_API_DEF(SW_API_PT_SPEED_SET, fal_port_speed_set), \
    SW_API_DEF(SW_API_PT_AN_ENABLE, fal_port_autoneg_enable), \
    SW_API_DEF(SW_API_PT_AN_RESTART, fal_port_autoneg_restart), \
    SW_API_DEF(SW_API_PT_AN_ADV_SET, fal_port_autoneg_adv_set), \
    SW_API_DEF(SW_API_PT_LINK_MODE_SET, fal_port_link_forcemode_set),   \
    SW_API_DEF(SW_API_PT_TXHDR_SET, fal_port_txhdr_mode_set), \
    SW_API_DEF(SW_API_PT_RXHDR_SET, fal_port_rxhdr_mode_set), \
    SW_API_DEF(SW_API_HEADER_TYPE_SET, fal_header_type_set),  \
    SW_API_DEF(SW_API_TXMAC_STATUS_SET, fal_port_txmac_status_set), \
    SW_API_DEF(SW_API_RXMAC_STATUS_SET, fal_port_rxmac_status_set), \
    SW_API_DEF(SW_API_PT_POWER_OFF, fal_port_power_off), \
    SW_API_DEF(SW_API_PT_POWER_ON, fal_port_power_on), \
    SW_API_DEF(SW_API_TXFC_STATUS_SET, fal_port_txfc_status_set),   \
    SW_API_DEF(SW_API_RXFC_STATUS_SET, fal_port_rxfc_status_set), \
    SW_API_DEF(SW_API_PT_SWITCH_PORT_LOOPBACK_SET, fal_switch_port_loopback_set),  \
    SW_API_DEF(SW_API_PT_SWITCH_PORT_LOOPBACK_GET, fal_switch_port_loopback_get),
/*end of PORTCONTROL_API*/
#define PORTCONTROL_API_PARAM \
    SW_API_DESC(SW_API_PT_DUPLEX_SET) \
    SW_API_DESC(SW_API_PT_SPEED_SET)  \
    SW_API_DESC(SW_API_PT_AN_ENABLE) \
    SW_API_DESC(SW_API_PT_AN_RESTART) \
    SW_API_DESC(SW_API_PT_AN_ADV_SET) \
    SW_API_DESC(SW_API_PT_LINK_MODE_SET) \
    SW_API_DESC(SW_API_PT_TXHDR_SET) \
    SW_API_DESC(SW_API_PT_RXHDR_SET) \
    SW_API_DESC(SW_API_HEADER_TYPE_SET) \
    SW_API_DESC(SW_API_TXMAC_STATUS_SET) \
    SW_API_DESC(SW_API_RXMAC_STATUS_SET) \
    SW_API_DESC(SW_API_PT_POWER_OFF) \
    SW_API_DESC(SW_API_PT_POWER_ON) \
    SW_API_DESC(SW_API_TXFC_STATUS_SET) \
    SW_API_DESC(SW_API_RXFC_STATUS_SET) \
    SW_API_DESC(SW_API_PT_SWITCH_PORT_LOOPBACK_SET)  \
    SW_API_DESC(SW_API_PT_SWITCH_PORT_LOOPBACK_GET)
/*end of PORTCONTROL_API_PARAM*/
#endif

#else
#define PORTCONTROL_API
#define PORTCONTROL_API_PARAM
#endif
#ifdef IN_VLAN
#define VLAN_API \
    SW_API_DEF(SW_API_VLAN_ADD, fal_vlan_create), \
    SW_API_DEF(SW_API_VLAN_DEL, fal_vlan_delete), \
    SW_API_DEF(SW_API_VLAN_MEM_UPDATE, fal_vlan_member_update), \
    SW_API_DEF(SW_API_VLAN_FIND, fal_vlan_find), \
    SW_API_DEF(SW_API_VLAN_NEXT, fal_vlan_next), \
    SW_API_DEF(SW_API_VLAN_APPEND, fal_vlan_entry_append), \
    SW_API_DEF(SW_API_VLAN_FLUSH, fal_vlan_flush), \
    SW_API_DEF(SW_API_VLAN_FID_SET, fal_vlan_fid_set), \
    SW_API_DEF(SW_API_VLAN_FID_GET, fal_vlan_fid_get), \
    SW_API_DEF(SW_API_VLAN_MEMBER_ADD, fal_vlan_member_add), \
    SW_API_DEF(SW_API_VLAN_MEMBER_DEL, fal_vlan_member_del), \
    SW_API_DEF(SW_API_VLAN_LEARN_STATE_SET, fal_vlan_learning_state_set), \
    SW_API_DEF(SW_API_VLAN_LEARN_STATE_GET, fal_vlan_learning_state_get),

#define VLAN_API_PARAM \
    SW_API_DESC(SW_API_VLAN_ADD) \
    SW_API_DESC(SW_API_VLAN_DEL) \
    SW_API_DESC(SW_API_VLAN_MEM_UPDATE) \
    SW_API_DESC(SW_API_VLAN_FIND) \
    SW_API_DESC(SW_API_VLAN_NEXT) \
    SW_API_DESC(SW_API_VLAN_APPEND) \
    SW_API_DESC(SW_API_VLAN_FLUSH) \
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
#ifndef IN_PORTVLAN_MINI
#define PORTVLAN_API \
    SW_API_DEF(SW_API_PT_ING_MODE_GET, fal_port_1qmode_get), \
    SW_API_DEF(SW_API_PT_ING_MODE_SET, fal_port_1qmode_set), \
    SW_API_DEF(SW_API_PT_EG_MODE_GET, fal_port_egvlanmode_get), \
    SW_API_DEF(SW_API_PT_EG_MODE_SET, fal_port_egvlanmode_set), \
    SW_API_DEF(SW_API_PT_VLAN_MEM_ADD, fal_portvlan_member_add), \
    SW_API_DEF(SW_API_PT_VLAN_MEM_DEL, fal_portvlan_member_del), \
    SW_API_DEF(SW_API_PT_VLAN_MEM_UPDATE, fal_portvlan_member_update), \
    SW_API_DEF(SW_API_PT_VLAN_MEM_GET, fal_portvlan_member_get), \
    SW_API_DEF(SW_API_PT_DEF_VID_SET, fal_port_default_vid_set), \
    SW_API_DEF(SW_API_PT_DEF_VID_GET, fal_port_default_vid_get), \
    SW_API_DEF(SW_API_PT_FORCE_DEF_VID_SET, fal_port_force_default_vid_set), \
    SW_API_DEF(SW_API_PT_FORCE_DEF_VID_GET, fal_port_force_default_vid_get), \
    SW_API_DEF(SW_API_PT_FORCE_PORTVLAN_SET, fal_port_force_portvlan_set), \
    SW_API_DEF(SW_API_PT_FORCE_PORTVLAN_GET, fal_port_force_portvlan_get), \
    SW_API_DEF(SW_API_PT_NESTVLAN_SET, fal_port_nestvlan_set), \
    SW_API_DEF(SW_API_PT_NESTVLAN_GET, fal_port_nestvlan_get), \
    SW_API_DEF(SW_API_NESTVLAN_TPID_SET, fal_nestvlan_tpid_set), \
    SW_API_DEF(SW_API_NESTVLAN_TPID_GET, fal_nestvlan_tpid_get), \
    SW_API_DEF(SW_API_PT_IN_VLAN_MODE_SET, fal_port_invlan_mode_set), \
    SW_API_DEF(SW_API_PT_IN_VLAN_MODE_GET, fal_port_invlan_mode_get), \
    SW_API_DEF(SW_API_PT_TLS_SET, fal_port_tls_set), \
    SW_API_DEF(SW_API_PT_TLS_GET, fal_port_tls_get), \
    SW_API_DEF(SW_API_PT_PRI_PROPAGATION_SET, fal_port_pri_propagation_set), \
    SW_API_DEF(SW_API_PT_PRI_PROPAGATION_GET, fal_port_pri_propagation_get), \
    SW_API_DEF(SW_API_PT_DEF_SVID_SET, fal_port_default_svid_set), \
    SW_API_DEF(SW_API_PT_DEF_SVID_GET, fal_port_default_svid_get), \
    SW_API_DEF(SW_API_PT_DEF_CVID_SET, fal_port_default_cvid_set), \
    SW_API_DEF(SW_API_PT_DEF_CVID_GET, fal_port_default_cvid_get), \
    SW_API_DEF(SW_API_PT_VLAN_PROPAGATION_SET, fal_port_vlan_propagation_set), \
    SW_API_DEF(SW_API_PT_VLAN_PROPAGATION_GET, fal_port_vlan_propagation_get), \
    SW_API_DEF(SW_API_PT_VLAN_TRANS_ADD, fal_port_vlan_trans_add), \
    SW_API_DEF(SW_API_PT_VLAN_TRANS_DEL, fal_port_vlan_trans_del), \
    SW_API_DEF(SW_API_PT_VLAN_TRANS_GET, fal_port_vlan_trans_get), \
    SW_API_DEF(SW_API_QINQ_MODE_SET, fal_qinq_mode_set), \
    SW_API_DEF(SW_API_QINQ_MODE_GET, fal_qinq_mode_get), \
    SW_API_DEF(SW_API_PT_QINQ_ROLE_SET, fal_port_qinq_role_set), \
    SW_API_DEF(SW_API_PT_QINQ_ROLE_GET, fal_port_qinq_role_get), \
    SW_API_DEF(SW_API_PT_VLAN_TRANS_ITERATE, fal_port_vlan_trans_iterate), \
    SW_API_DEF(SW_API_PT_MAC_VLAN_XLT_SET, fal_port_mac_vlan_xlt_set), \
    SW_API_DEF(SW_API_PT_MAC_VLAN_XLT_GET, fal_port_mac_vlan_xlt_get), \
    SW_API_DEF(SW_API_NETISOLATE_SET, fal_netisolate_set), \
    SW_API_DEF(SW_API_NETISOLATE_GET, fal_netisolate_get), \
    SW_API_DEF(SW_API_EG_FLTR_BYPASS_EN_SET, fal_eg_trans_filter_bypass_en_set), \
    SW_API_DEF(SW_API_EG_FLTR_BYPASS_EN_GET, fal_eg_trans_filter_bypass_en_get), \
    SW_API_DEF(SW_API_PT_VRF_ID_SET, fal_port_vrf_id_set), \
    SW_API_DEF(SW_API_PT_VRF_ID_GET, fal_port_vrf_id_get), \
    SW_API_DEF(SW_API_GLOBAL_QINQ_MODE_SET, fal_global_qinq_mode_set), \
    SW_API_DEF(SW_API_GLOBAL_QINQ_MODE_GET, fal_global_qinq_mode_get), \
    SW_API_DEF(SW_API_PORT_QINQ_MODE_SET, fal_port_qinq_mode_set), \
    SW_API_DEF(SW_API_PORT_QINQ_MODE_GET, fal_port_qinq_mode_get), \
    SW_API_DEF(SW_API_TPID_SET, fal_ingress_tpid_set), \
    SW_API_DEF(SW_API_TPID_GET, fal_ingress_tpid_get), \
    SW_API_DEF(SW_API_EGRESS_TPID_SET, fal_egress_tpid_set), \
    SW_API_DEF(SW_API_EGRESS_TPID_GET, fal_egress_tpid_get), \
    SW_API_DEF(SW_API_PT_INGRESS_VLAN_FILTER_SET, fal_port_ingress_vlan_filter_set), \
    SW_API_DEF(SW_API_PT_INGRESS_VLAN_FILTER_GET, fal_port_ingress_vlan_filter_get), \
    SW_API_DEF(SW_API_PT_DEFAULT_VLANTAG_SET, fal_port_default_vlantag_set), \
    SW_API_DEF(SW_API_PT_DEFAULT_VLANTAG_GET, fal_port_default_vlantag_get), \
    SW_API_DEF(SW_API_PT_TAG_PROPAGATION_SET, fal_port_tag_propagation_set), \
    SW_API_DEF(SW_API_PT_TAG_PROPAGATION_GET, fal_port_tag_propagation_get), \
    SW_API_DEF(SW_API_PT_VLANTAG_EGMODE_SET, fal_port_vlantag_egmode_set), \
    SW_API_DEF(SW_API_PT_VLANTAG_EGMODE_GET, fal_port_vlantag_egmode_get), \
    SW_API_DEF(SW_API_PT_VLAN_XLT_MISS_CMD_SET, fal_port_vlan_xlt_miss_cmd_set), \
    SW_API_DEF(SW_API_PT_VLAN_XLT_MISS_CMD_GET, fal_port_vlan_xlt_miss_cmd_get), \
    SW_API_DEF(SW_API_PT_VSI_EGMODE_SET, fal_port_vsi_egmode_set), \
    SW_API_DEF(SW_API_PT_VSI_EGMODE_GET, fal_port_vsi_egmode_get), \
    SW_API_DEF(SW_API_PT_VLANTAG_VSI_EGMODE_EN_SET, fal_port_vlantag_vsi_egmode_enable), \
    SW_API_DEF(SW_API_PT_VLANTAG_VSI_EGMODE_EN_GET, fal_port_vlantag_vsi_egmode_status_get), \
    SW_API_DEF(SW_API_PT_VLAN_TRANS_ADV_ADD, fal_port_vlan_trans_adv_add), \
    SW_API_DEF(SW_API_PT_VLAN_TRANS_ADV_DEL, fal_port_vlan_trans_adv_del), \
    SW_API_DEF(SW_API_PT_VLAN_TRANS_ADV_GETFIRST, fal_port_vlan_trans_adv_getfirst), \
    SW_API_DEF(SW_API_PT_VLAN_TRANS_ADV_GETNEXT, fal_port_vlan_trans_adv_getnext), \
    SW_API_DEF(SW_API_PT_VLAN_COUNTER_GET, fal_port_vlan_counter_get), \
    SW_API_DEF(SW_API_PT_VLAN_COUNTER_CLEANUP, fal_port_vlan_counter_cleanup),

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
    SW_API_DESC(SW_API_NESTVLAN_TPID_GET)    \
    SW_API_DESC(SW_API_PT_IN_VLAN_MODE_SET)   \
    SW_API_DESC(SW_API_PT_IN_VLAN_MODE_GET)   \
    SW_API_DESC(SW_API_PT_TLS_SET)   \
    SW_API_DESC(SW_API_PT_TLS_GET)   \
    SW_API_DESC(SW_API_PT_PRI_PROPAGATION_SET)    \
    SW_API_DESC(SW_API_PT_PRI_PROPAGATION_GET)   \
    SW_API_DESC(SW_API_PT_DEF_SVID_SET)    \
    SW_API_DESC(SW_API_PT_DEF_SVID_GET)    \
    SW_API_DESC(SW_API_PT_DEF_CVID_SET)    \
    SW_API_DESC(SW_API_PT_DEF_CVID_GET)    \
    SW_API_DESC(SW_API_PT_VLAN_PROPAGATION_SET)   \
    SW_API_DESC(SW_API_PT_VLAN_PROPAGATION_GET)   \
    SW_API_DESC(SW_API_PT_VLAN_TRANS_ADD) \
    SW_API_DESC(SW_API_PT_VLAN_TRANS_DEL) \
    SW_API_DESC(SW_API_PT_VLAN_TRANS_GET) \
    SW_API_DESC(SW_API_QINQ_MODE_SET)    \
    SW_API_DESC(SW_API_QINQ_MODE_GET)    \
    SW_API_DESC(SW_API_PT_QINQ_ROLE_SET) \
    SW_API_DESC(SW_API_PT_QINQ_ROLE_GET) \
    SW_API_DESC(SW_API_PT_VLAN_TRANS_ITERATE) \
    SW_API_DESC(SW_API_PT_MAC_VLAN_XLT_SET)   \
    SW_API_DESC(SW_API_PT_MAC_VLAN_XLT_GET)  \
    SW_API_DESC(SW_API_NETISOLATE_SET)   \
    SW_API_DESC(SW_API_NETISOLATE_GET)   \
    SW_API_DESC(SW_API_EG_FLTR_BYPASS_EN_SET)   \
    SW_API_DESC(SW_API_EG_FLTR_BYPASS_EN_GET)   \
    SW_API_DESC(SW_API_PT_VRF_ID_SET)  \
    SW_API_DESC(SW_API_PT_VRF_ID_GET) \
    SW_API_DESC(SW_API_GLOBAL_QINQ_MODE_SET)  \
    SW_API_DESC(SW_API_GLOBAL_QINQ_MODE_GET) \
    SW_API_DESC(SW_API_PORT_QINQ_MODE_SET)  \
    SW_API_DESC(SW_API_PORT_QINQ_MODE_GET) \
    SW_API_DESC(SW_API_TPID_SET)  \
    SW_API_DESC(SW_API_TPID_GET) \
    SW_API_DESC(SW_API_EGRESS_TPID_SET)  \
    SW_API_DESC(SW_API_EGRESS_TPID_GET) \
    SW_API_DESC(SW_API_PT_INGRESS_VLAN_FILTER_SET)  \
    SW_API_DESC(SW_API_PT_INGRESS_VLAN_FILTER_GET) \
    SW_API_DESC(SW_API_PT_DEFAULT_VLANTAG_SET)  \
    SW_API_DESC(SW_API_PT_DEFAULT_VLANTAG_GET) \
    SW_API_DESC(SW_API_PT_TAG_PROPAGATION_SET)  \
    SW_API_DESC(SW_API_PT_TAG_PROPAGATION_GET) \
    SW_API_DESC(SW_API_PT_VLANTAG_EGMODE_SET)  \
    SW_API_DESC(SW_API_PT_VLANTAG_EGMODE_GET) \
    SW_API_DESC(SW_API_PT_VLAN_XLT_MISS_CMD_SET)  \
    SW_API_DESC(SW_API_PT_VLAN_XLT_MISS_CMD_GET) \
    SW_API_DESC(SW_API_PT_VSI_EGMODE_SET) \
    SW_API_DESC(SW_API_PT_VSI_EGMODE_GET) \
    SW_API_DESC(SW_API_PT_VLANTAG_VSI_EGMODE_EN_SET) \
    SW_API_DESC(SW_API_PT_VLANTAG_VSI_EGMODE_EN_GET) \
    SW_API_DESC(SW_API_PT_VLAN_TRANS_ADV_ADD) \
    SW_API_DESC(SW_API_PT_VLAN_TRANS_ADV_DEL) \
    SW_API_DESC(SW_API_PT_VLAN_TRANS_ADV_GETFIRST) \
    SW_API_DESC(SW_API_PT_VLAN_TRANS_ADV_GETNEXT) \
    SW_API_DESC(SW_API_PT_VLAN_COUNTER_GET) \
    SW_API_DESC(SW_API_PT_VLAN_COUNTER_CLEANUP)

#else
#define PORTVLAN_API \
    SW_API_DEF(SW_API_GLOBAL_QINQ_MODE_SET, fal_global_qinq_mode_set), \
    SW_API_DEF(SW_API_GLOBAL_QINQ_MODE_GET, fal_global_qinq_mode_get), \
    SW_API_DEF(SW_API_PORT_QINQ_MODE_SET, fal_port_qinq_mode_set), \
    SW_API_DEF(SW_API_PORT_QINQ_MODE_GET, fal_port_qinq_mode_get), \
    SW_API_DEF(SW_API_QINQ_MODE_SET, fal_qinq_mode_set), \
    SW_API_DEF(SW_API_PT_QINQ_ROLE_SET, fal_port_qinq_role_set), \
    SW_API_DEF(SW_API_PT_VLAN_TRANS_ADD, fal_port_vlan_trans_add), \
    SW_API_DEF(SW_API_TPID_SET, fal_ingress_tpid_set), \
    SW_API_DEF(SW_API_TPID_GET, fal_ingress_tpid_get), \
    SW_API_DEF(SW_API_EGRESS_TPID_SET, fal_egress_tpid_set), \
    SW_API_DEF(SW_API_EGRESS_TPID_GET, fal_egress_tpid_get), \
    SW_API_DEF(SW_API_PT_INGRESS_VLAN_FILTER_SET, fal_port_ingress_vlan_filter_set), \
    SW_API_DEF(SW_API_PT_INGRESS_VLAN_FILTER_GET, fal_port_ingress_vlan_filter_get), \
    SW_API_DEF(SW_API_PT_DEFAULT_VLANTAG_SET, fal_port_default_vlantag_set), \
    SW_API_DEF(SW_API_PT_DEFAULT_VLANTAG_GET, fal_port_default_vlantag_get), \
    SW_API_DEF(SW_API_PT_TAG_PROPAGATION_SET, fal_port_tag_propagation_set), \
    SW_API_DEF(SW_API_PT_TAG_PROPAGATION_GET, fal_port_tag_propagation_get), \
    SW_API_DEF(SW_API_PT_VLANTAG_EGMODE_SET, fal_port_vlantag_egmode_set), \
    SW_API_DEF(SW_API_PT_VLANTAG_EGMODE_GET, fal_port_vlantag_egmode_get), \
    SW_API_DEF(SW_API_PT_VLAN_XLT_MISS_CMD_SET, fal_port_vlan_xlt_miss_cmd_set), \
    SW_API_DEF(SW_API_PT_VLAN_XLT_MISS_CMD_GET, fal_port_vlan_xlt_miss_cmd_get), \
    SW_API_DEF(SW_API_PT_VSI_EGMODE_SET, fal_port_vsi_egmode_set), \
    SW_API_DEF(SW_API_PT_VSI_EGMODE_GET, fal_port_vsi_egmode_get), \
    SW_API_DEF(SW_API_PT_VLANTAG_VSI_EGMODE_EN_SET, fal_port_vlantag_vsi_egmode_enable), \
    SW_API_DEF(SW_API_PT_VLANTAG_VSI_EGMODE_EN_GET, fal_port_vlantag_vsi_egmode_status_get), \
    SW_API_DEF(SW_API_PT_VLAN_TRANS_ADV_ADD, fal_port_vlan_trans_adv_add), \
    SW_API_DEF(SW_API_PT_VLAN_TRANS_ADV_DEL, fal_port_vlan_trans_adv_del), \
    SW_API_DEF(SW_API_PT_VLAN_TRANS_ADV_GETFIRST, fal_port_vlan_trans_adv_getfirst), \
    SW_API_DEF(SW_API_PT_VLAN_TRANS_ADV_GETNEXT, fal_port_vlan_trans_adv_getnext), \
    SW_API_DEF(SW_API_PT_VLAN_COUNTER_GET, fal_port_vlan_counter_get), \
    SW_API_DEF(SW_API_PT_VLAN_COUNTER_CLEANUP, fal_port_vlan_counter_cleanup), \
    SW_API_DEF(SW_API_PT_ING_MODE_SET, fal_port_1qmode_set), \
    SW_API_DEF(SW_API_PT_EG_MODE_SET, fal_port_egvlanmode_set), \
    SW_API_DEF(SW_API_PT_VLAN_MEM_ADD, fal_portvlan_member_add), \
    SW_API_DEF(SW_API_PT_VLAN_MEM_DEL, fal_portvlan_member_del), \
    SW_API_DEF(SW_API_PT_VLAN_MEM_UPDATE, fal_portvlan_member_update), \
    SW_API_DEF(SW_API_PT_DEF_VID_SET, fal_port_default_vid_set), \
    SW_API_DEF(SW_API_PT_FORCE_DEF_VID_SET, fal_port_force_default_vid_set), \
    SW_API_DEF(SW_API_PT_FORCE_PORTVLAN_SET, fal_port_force_portvlan_set), \
    SW_API_DEF(SW_API_PT_NESTVLAN_SET, fal_port_nestvlan_set), \
    SW_API_DEF(SW_API_NESTVLAN_TPID_SET, fal_nestvlan_tpid_set), \
    SW_API_DEF(SW_API_PT_IN_VLAN_MODE_SET, fal_port_invlan_mode_set), \
    SW_API_DEF(SW_API_PT_DEF_SVID_SET, fal_port_default_svid_set), \
    SW_API_DEF(SW_API_PT_DEF_CVID_SET, fal_port_default_cvid_set),

#define PORTVLAN_API_PARAM \
    SW_API_DESC(SW_API_GLOBAL_QINQ_MODE_SET)  \
    SW_API_DESC(SW_API_GLOBAL_QINQ_MODE_GET) \
    SW_API_DESC(SW_API_PORT_QINQ_MODE_SET)  \
    SW_API_DESC(SW_API_PORT_QINQ_MODE_GET) \
    SW_API_DESC(SW_API_QINQ_MODE_SET)    \
    SW_API_DESC(SW_API_PT_QINQ_ROLE_SET) \
    SW_API_DESC(SW_API_PT_VLAN_TRANS_ADD) \
    SW_API_DESC(SW_API_TPID_SET)  \
    SW_API_DESC(SW_API_TPID_GET) \
    SW_API_DESC(SW_API_EGRESS_TPID_SET)  \
    SW_API_DESC(SW_API_EGRESS_TPID_GET) \
    SW_API_DESC(SW_API_PT_INGRESS_VLAN_FILTER_SET)  \
    SW_API_DESC(SW_API_PT_INGRESS_VLAN_FILTER_GET) \
    SW_API_DESC(SW_API_PT_DEFAULT_VLANTAG_SET)  \
    SW_API_DESC(SW_API_PT_DEFAULT_VLANTAG_GET) \
    SW_API_DESC(SW_API_PT_TAG_PROPAGATION_SET)  \
    SW_API_DESC(SW_API_PT_TAG_PROPAGATION_GET) \
    SW_API_DESC(SW_API_PT_VLANTAG_EGMODE_SET)  \
    SW_API_DESC(SW_API_PT_VLANTAG_EGMODE_GET) \
    SW_API_DESC(SW_API_PT_VLAN_XLT_MISS_CMD_SET)  \
    SW_API_DESC(SW_API_PT_VLAN_XLT_MISS_CMD_GET) \
    SW_API_DESC(SW_API_PT_VSI_EGMODE_SET) \
    SW_API_DESC(SW_API_PT_VSI_EGMODE_GET) \
    SW_API_DESC(SW_API_PT_VLANTAG_VSI_EGMODE_EN_SET) \
    SW_API_DESC(SW_API_PT_VLANTAG_VSI_EGMODE_EN_GET) \
    SW_API_DESC(SW_API_PT_VLAN_TRANS_ADV_ADD) \
    SW_API_DESC(SW_API_PT_VLAN_TRANS_ADV_DEL) \
    SW_API_DESC(SW_API_PT_VLAN_TRANS_ADV_GETFIRST) \
    SW_API_DESC(SW_API_PT_VLAN_TRANS_ADV_GETNEXT) \
    SW_API_DESC(SW_API_PT_VLAN_COUNTER_GET) \
    SW_API_DESC(SW_API_PT_VLAN_COUNTER_CLEANUP) \
    SW_API_DESC(SW_API_PT_ING_MODE_SET) \
    SW_API_DESC(SW_API_PT_EG_MODE_SET) \
    SW_API_DESC(SW_API_PT_VLAN_MEM_ADD) \
    SW_API_DESC(SW_API_PT_VLAN_MEM_DEL) \
    SW_API_DESC(SW_API_PT_VLAN_MEM_UPDATE) \
    SW_API_DESC(SW_API_PT_DEF_VID_SET)    \
    SW_API_DESC(SW_API_PT_FORCE_DEF_VID_SET)    \
    SW_API_DESC(SW_API_PT_FORCE_PORTVLAN_SET)    \
    SW_API_DESC(SW_API_PT_NESTVLAN_SET)    \
    SW_API_DESC(SW_API_NESTVLAN_TPID_SET)    \
    SW_API_DESC(SW_API_PT_IN_VLAN_MODE_SET)   \
    SW_API_DESC(SW_API_PT_DEF_SVID_SET)    \
    SW_API_DESC(SW_API_PT_DEF_CVID_SET)

#endif

#else
#define PORTVLAN_API
#define PORTVLAN_API_PARAM
#endif

#ifdef IN_FDB
#ifndef IN_FDB_MINI
#define FDB_API \
    SW_API_DEF(SW_API_FDB_ADD, fal_fdb_entry_add), \
    SW_API_DEF(SW_API_FDB_DELALL, fal_fdb_entry_flush), \
    SW_API_DEF(SW_API_FDB_DELPORT,fal_fdb_entry_del_byport), \
    SW_API_DEF(SW_API_FDB_DELMAC, fal_fdb_entry_del_bymac), \
    SW_API_DEF(SW_API_FDB_FIRST,  fal_fdb_entry_getfirst), \
    SW_API_DEF(SW_API_FDB_NEXT,   fal_fdb_entry_getnext), \
    SW_API_DEF(SW_API_FDB_FIND,   fal_fdb_entry_search), \
    SW_API_DEF(SW_API_FDB_PT_LEARN_SET,   fal_fdb_port_learn_set), \
    SW_API_DEF(SW_API_FDB_PT_LEARN_GET,   fal_fdb_port_learn_get), \
    SW_API_DEF(SW_API_FDB_PT_NEWADDR_LEARN_SET, fal_fdb_port_learning_ctrl_set), \
    SW_API_DEF(SW_API_FDB_PT_NEWADDR_LEARN_GET, fal_fdb_port_learning_ctrl_get), \
    SW_API_DEF(SW_API_FDB_PT_STAMOVE_SET,       fal_fdb_port_stamove_ctrl_set), \
    SW_API_DEF(SW_API_FDB_PT_STAMOVE_GET,       fal_fdb_port_stamove_ctrl_get), \
    SW_API_DEF(SW_API_FDB_AGE_CTRL_SET,   fal_fdb_aging_ctrl_set), \
    SW_API_DEF(SW_API_FDB_AGE_CTRL_GET,   fal_fdb_aging_ctrl_get), \
    SW_API_DEF(SW_API_FDB_LEARN_CTRL_SET,  fal_fdb_learning_ctrl_set), \
    SW_API_DEF(SW_API_FDB_LEARN_CTRL_GET,  fal_fdb_learning_ctrl_get), \
    SW_API_DEF(SW_API_FDB_VLAN_IVL_SVL_SET, fal_fdb_vlan_ivl_svl_set),\
    SW_API_DEF(SW_API_FDB_VLAN_IVL_SVL_GET, fal_fdb_vlan_ivl_svl_get),\
    SW_API_DEF(SW_API_FDB_AGE_TIME_SET,   fal_fdb_aging_time_set), \
    SW_API_DEF(SW_API_FDB_AGE_TIME_GET,   fal_fdb_aging_time_get), \
    SW_API_DEF(SW_API_FDB_ITERATE,        fal_fdb_entry_getnext_byindex), \
    SW_API_DEF(SW_API_FDB_EXTEND_NEXT,    fal_fdb_entry_extend_getnext),  \
    SW_API_DEF(SW_API_FDB_EXTEND_FIRST,   fal_fdb_entry_extend_getfirst), \
    SW_API_DEF(SW_API_FDB_TRANSFER,       fal_fdb_entry_update_byport), \
    SW_API_DEF(SW_API_PT_FDB_LEARN_COUNTER_GET,  fal_fdb_port_learned_mac_counter_get), \
    SW_API_DEF(SW_API_PT_FDB_LEARN_LIMIT_SET,    fal_port_fdb_learn_limit_set),  \
    SW_API_DEF(SW_API_PT_FDB_LEARN_LIMIT_GET,    fal_port_fdb_learn_limit_get),  \
    SW_API_DEF(SW_API_PT_FDB_LEARN_EXCEED_CMD_SET,    fal_port_fdb_learn_exceed_cmd_set),  \
    SW_API_DEF(SW_API_PT_FDB_LEARN_EXCEED_CMD_GET,    fal_port_fdb_learn_exceed_cmd_get), \
    SW_API_DEF(SW_API_FDB_LEARN_LIMIT_SET,    fal_fdb_learn_limit_set),  \
    SW_API_DEF(SW_API_FDB_LEARN_LIMIT_GET,    fal_fdb_learn_limit_get),  \
    SW_API_DEF(SW_API_FDB_LEARN_EXCEED_CMD_SET,    fal_fdb_learn_exceed_cmd_set),  \
    SW_API_DEF(SW_API_FDB_LEARN_EXCEED_CMD_GET,    fal_fdb_learn_exceed_cmd_get),  \
    SW_API_DEF(SW_API_FDB_RESV_ADD, fal_fdb_resv_add), \
    SW_API_DEF(SW_API_FDB_RESV_DEL, fal_fdb_resv_del), \
    SW_API_DEF(SW_API_FDB_RESV_FIND, fal_fdb_resv_find), \
    SW_API_DEF(SW_API_FDB_RESV_ITERATE, fal_fdb_resv_iterate), \
    SW_API_DEF(SW_API_FDB_PT_LEARN_STATIC_SET,   fal_fdb_port_learn_static_set), \
    SW_API_DEF(SW_API_FDB_PT_LEARN_STATIC_GET,   fal_fdb_port_learn_static_get), \
    SW_API_DEF(SW_API_FDB_PORT_ADD,   fal_fdb_port_add), \
    SW_API_DEF(SW_API_FDB_PORT_DEL,   fal_fdb_port_del), \
    SW_API_DEF(SW_API_FDB_RFS_SET,   fal_fdb_rfs_set),   \
    SW_API_DEF(SW_API_FDB_RFS_DEL,   fal_fdb_rfs_del),   \
    SW_API_DEF(SW_API_FDB_PT_MACLIMIT_CTRL_SET, fal_fdb_port_maclimit_ctrl_set), \
    SW_API_DEF(SW_API_FDB_PT_MACLIMIT_CTRL_GET, fal_fdb_port_maclimit_ctrl_get), \
    SW_API_DEF(SW_API_FDB_DEL_BY_FID, fal_fdb_entry_del_byfid),

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
    SW_API_DESC(SW_API_FDB_PT_NEWADDR_LEARN_SET) \
    SW_API_DESC(SW_API_FDB_PT_NEWADDR_LEARN_GET) \
    SW_API_DESC(SW_API_FDB_PT_STAMOVE_SET) \
    SW_API_DESC(SW_API_FDB_PT_STAMOVE_GET) \
    SW_API_DESC(SW_API_FDB_AGE_CTRL_SET) \
    SW_API_DESC(SW_API_FDB_AGE_CTRL_GET) \
    SW_API_DESC(SW_API_FDB_LEARN_CTRL_SET) \
    SW_API_DESC(SW_API_FDB_LEARN_CTRL_GET) \
    SW_API_DESC(SW_API_FDB_VLAN_IVL_SVL_SET) \
    SW_API_DESC(SW_API_FDB_VLAN_IVL_SVL_GET) \
    SW_API_DESC(SW_API_FDB_AGE_TIME_SET) \
    SW_API_DESC(SW_API_FDB_AGE_TIME_GET) \
    SW_API_DESC(SW_API_FDB_ITERATE) \
    SW_API_DESC(SW_API_FDB_EXTEND_NEXT)  \
    SW_API_DESC(SW_API_FDB_EXTEND_FIRST) \
    SW_API_DESC(SW_API_FDB_TRANSFER) \
    SW_API_DESC(SW_API_PT_FDB_LEARN_COUNTER_GET) \
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
    SW_API_DESC(SW_API_FDB_PT_LEARN_STATIC_SET) \
    SW_API_DESC(SW_API_FDB_PT_LEARN_STATIC_GET) \
    SW_API_DESC(SW_API_FDB_PORT_ADD)  \
    SW_API_DESC(SW_API_FDB_PORT_DEL)  \
    SW_API_DESC(SW_API_FDB_RFS_SET)   \
    SW_API_DESC(SW_API_FDB_RFS_DEL)   \
    SW_API_DESC(SW_API_FDB_PT_MACLIMIT_CTRL_SET) \
    SW_API_DESC(SW_API_FDB_PT_MACLIMIT_CTRL_GET) \
    SW_API_DESC(SW_API_FDB_DEL_BY_FID)
#else
#define FDB_API \
    SW_API_DEF(SW_API_FDB_PT_LEARN_SET,   fal_fdb_port_learn_set), \
    SW_API_DEF(SW_API_FDB_EXTEND_FIRST,   fal_fdb_entry_extend_getfirst), \
    SW_API_DEF(SW_API_FDB_EXTEND_NEXT,    fal_fdb_entry_extend_getnext),

#define FDB_API_PARAM \
    SW_API_DESC(SW_API_FDB_PT_LEARN_SET) \
    SW_API_DESC(SW_API_FDB_EXTEND_FIRST) \
    SW_API_DESC(SW_API_FDB_EXTEND_NEXT)
#endif
#else
#define FDB_API
#define FDB_API_PARAM
#endif

#ifdef IN_ACL
#define ACL_API \
    SW_API_DEF(SW_API_ACL_LIST_CREAT, fal_acl_list_creat), \
    SW_API_DEF(SW_API_ACL_LIST_DESTROY, fal_acl_list_destroy), \
    SW_API_DEF(SW_API_ACL_RULE_ADD, fal_acl_rule_add), \
    SW_API_DEF(SW_API_ACL_RULE_DELETE, fal_acl_rule_delete), \
    SW_API_DEF(SW_API_ACL_RULE_QUERY, fal_acl_rule_query), \
    SW_API_DEF(SW_API_ACL_LIST_BIND, fal_acl_list_bind), \
    SW_API_DEF(SW_API_ACL_LIST_UNBIND, fal_acl_list_unbind), \
    SW_API_DEF(SW_API_ACL_STATUS_SET, fal_acl_status_set), \
    SW_API_DEF(SW_API_ACL_STATUS_GET, fal_acl_status_get), \
    SW_API_DEF(SW_API_ACL_LIST_DUMP, fal_acl_list_dump), \
    SW_API_DEF(SW_API_ACL_RULE_DUMP, fal_acl_rule_dump), \
    SW_API_DEF(SW_API_ACL_PT_UDF_PROFILE_SET, fal_acl_port_udf_profile_set), \
    SW_API_DEF(SW_API_ACL_PT_UDF_PROFILE_GET, fal_acl_port_udf_profile_get), \
    SW_API_DEF(SW_API_ACL_RULE_ACTIVE, fal_acl_rule_active), \
    SW_API_DEF(SW_API_ACL_RULE_DEACTIVE, fal_acl_rule_deactive),\
    SW_API_DEF(SW_API_ACL_RULE_SRC_FILTER_STS_SET, fal_acl_rule_src_filter_sts_set),\
    SW_API_DEF(SW_API_ACL_RULE_SRC_FILTER_STS_GET, fal_acl_rule_src_filter_sts_get),\
    SW_API_DEF(SW_API_ACL_UDF_SET, fal_acl_udf_profile_set),\
    SW_API_DEF(SW_API_ACL_UDF_GET, fal_acl_udf_profile_get),

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
    SW_API_DESC(SW_API_ACL_RULE_DUMP) \
    SW_API_DESC(SW_API_ACL_PT_UDF_PROFILE_SET)    \
    SW_API_DESC(SW_API_ACL_PT_UDF_PROFILE_GET)    \
    SW_API_DESC(SW_API_ACL_RULE_ACTIVE)    \
    SW_API_DESC(SW_API_ACL_RULE_DEACTIVE) \
    SW_API_DESC(SW_API_ACL_RULE_SRC_FILTER_STS_SET)\
    SW_API_DESC(SW_API_ACL_RULE_SRC_FILTER_STS_GET)\
    SW_API_DESC(SW_API_ACL_UDF_SET)    \
    SW_API_DESC(SW_API_ACL_UDF_GET)
#else
#define ACL_API
#define ACL_API_PARAM
#endif

#ifdef IN_QOS
#ifndef IN_QOS_MINI
#define QOS_API \
    SW_API_DEF(SW_API_QOS_SCH_MODE_SET, fal_qos_sch_mode_set), \
    SW_API_DEF(SW_API_QOS_SCH_MODE_GET, fal_qos_sch_mode_get), \
    SW_API_DEF(SW_API_QOS_QU_TX_BUF_ST_SET, fal_qos_queue_tx_buf_status_set), \
    SW_API_DEF(SW_API_QOS_QU_TX_BUF_ST_GET, fal_qos_queue_tx_buf_status_get), \
    SW_API_DEF(SW_API_QOS_QU_TX_BUF_NR_SET, fal_qos_queue_tx_buf_nr_set), \
    SW_API_DEF(SW_API_QOS_QU_TX_BUF_NR_GET, fal_qos_queue_tx_buf_nr_get), \
    SW_API_DEF(SW_API_QOS_PT_TX_BUF_ST_SET, fal_qos_port_tx_buf_status_set), \
    SW_API_DEF(SW_API_QOS_PT_TX_BUF_ST_GET, fal_qos_port_tx_buf_status_get), \
    SW_API_DEF(SW_API_QOS_PT_RED_EN_SET, fal_qos_port_red_en_set), \
    SW_API_DEF(SW_API_QOS_PT_RED_EN_GET, fal_qos_port_red_en_get), \
    SW_API_DEF(SW_API_QOS_PT_TX_BUF_NR_SET, fal_qos_port_tx_buf_nr_set), \
    SW_API_DEF(SW_API_QOS_PT_TX_BUF_NR_GET, fal_qos_port_tx_buf_nr_get), \
    SW_API_DEF(SW_API_QOS_PT_RX_BUF_NR_SET, fal_qos_port_rx_buf_nr_set), \
    SW_API_DEF(SW_API_QOS_PT_RX_BUF_NR_GET, fal_qos_port_rx_buf_nr_get), \
    SW_API_DEF(SW_API_COSMAP_UP_QU_SET, fal_cosmap_up_queue_set), \
    SW_API_DEF(SW_API_COSMAP_UP_QU_GET, fal_cosmap_up_queue_get), \
    SW_API_DEF(SW_API_COSMAP_DSCP_QU_SET, fal_cosmap_dscp_queue_set), \
    SW_API_DEF(SW_API_COSMAP_DSCP_QU_GET, fal_cosmap_dscp_queue_get), \
    SW_API_DEF(SW_API_QOS_PT_MODE_SET, fal_qos_port_mode_set), \
    SW_API_DEF(SW_API_QOS_PT_MODE_GET, fal_qos_port_mode_get), \
    SW_API_DEF(SW_API_QOS_PT_MODE_PRI_SET, fal_qos_port_mode_pri_set), \
    SW_API_DEF(SW_API_QOS_PT_MODE_PRI_GET, fal_qos_port_mode_pri_get), \
    SW_API_DEF(SW_API_QOS_PORT_DEF_UP_SET, fal_qos_port_default_up_set), \
    SW_API_DEF(SW_API_QOS_PORT_DEF_UP_GET, fal_qos_port_default_up_get), \
    SW_API_DEF(SW_API_QOS_PORT_SCH_MODE_SET, fal_qos_port_sch_mode_set), \
    SW_API_DEF(SW_API_QOS_PORT_SCH_MODE_GET, fal_qos_port_sch_mode_get), \
    SW_API_DEF(SW_API_QOS_PT_DEF_SPRI_SET, fal_qos_port_default_spri_set), \
    SW_API_DEF(SW_API_QOS_PT_DEF_SPRI_GET, fal_qos_port_default_spri_get), \
    SW_API_DEF(SW_API_QOS_PT_DEF_CPRI_SET, fal_qos_port_default_cpri_set), \
    SW_API_DEF(SW_API_QOS_PT_DEF_CPRI_GET, fal_qos_port_default_cpri_get), \
    SW_API_DEF(SW_API_QOS_PT_FORCE_SPRI_ST_SET, fal_qos_port_force_spri_status_set), \
    SW_API_DEF(SW_API_QOS_PT_FORCE_SPRI_ST_GET, fal_qos_port_force_spri_status_get), \
    SW_API_DEF(SW_API_QOS_PT_FORCE_CPRI_ST_SET, fal_qos_port_force_cpri_status_set), \
    SW_API_DEF(SW_API_QOS_PT_FORCE_CPRI_ST_GET, fal_qos_port_force_cpri_status_get), \
    SW_API_DEF(SW_API_QOS_QUEUE_REMARK_SET, fal_qos_queue_remark_table_set), \
    SW_API_DEF(SW_API_QOS_QUEUE_REMARK_GET, fal_qos_queue_remark_table_get), \
    SW_API_DEF(SW_API_QOS_PORT_GROUP_GET, fal_qos_port_group_get), \
    SW_API_DEF(SW_API_QOS_PORT_GROUP_SET, fal_qos_port_group_set), \
    SW_API_DEF(SW_API_QOS_PORT_PRI_GET, fal_qos_port_pri_precedence_get), \
    SW_API_DEF(SW_API_QOS_PORT_PRI_SET, fal_qos_port_pri_precedence_set), \
    SW_API_DEF(SW_API_QOS_PORT_REMARK_GET, fal_qos_port_remark_get), \
    SW_API_DEF(SW_API_QOS_PORT_REMARK_SET, fal_qos_port_remark_set), \
    SW_API_DEF(SW_API_QOS_PCP_MAP_GET, fal_qos_cosmap_pcp_get), \
    SW_API_DEF(SW_API_QOS_PCP_MAP_SET, fal_qos_cosmap_pcp_set), \
    SW_API_DEF(SW_API_QOS_FLOW_MAP_GET, fal_qos_cosmap_flow_get), \
    SW_API_DEF(SW_API_QOS_FLOW_MAP_SET, fal_qos_cosmap_flow_set), \
    SW_API_DEF(SW_API_QOS_DSCP_MAP_GET, fal_qos_cosmap_dscp_get), \
    SW_API_DEF(SW_API_QOS_DSCP_MAP_SET, fal_qos_cosmap_dscp_set), \
    SW_API_DEF(SW_API_QOS_QUEUE_SCHEDULER_GET, fal_queue_scheduler_get), \
    SW_API_DEF(SW_API_QOS_QUEUE_SCHEDULER_SET, fal_queue_scheduler_set), \
    SW_API_DEF(SW_API_QOS_RING_QUEUE_MAP_GET, fal_edma_ring_queue_map_get), \
    SW_API_DEF(SW_API_QOS_RING_QUEUE_MAP_SET, fal_edma_ring_queue_map_set), \
    SW_API_DEF(SW_API_QOS_PORT_QUEUES_GET, fal_port_queues_get), \
    SW_API_DEF(SW_API_QOS_SCHEDULER_DEQUEU_CTRL_GET, fal_scheduler_dequeue_ctrl_get), \
    SW_API_DEF(SW_API_QOS_SCHEDULER_DEQUEU_CTRL_SET, fal_scheduler_dequeue_ctrl_set), \
    SW_API_DEF(SW_API_QOS_PORT_SCHEDULER_CFG_RESET, fal_port_scheduler_cfg_reset), \
    SW_API_DEF(SW_API_QOS_PORT_SCHEDULER_RESOURCE_GET, fal_port_scheduler_resource_get),

#define QOS_API_PARAM \
    SW_API_DESC(SW_API_QOS_SCH_MODE_SET) \
    SW_API_DESC(SW_API_QOS_SCH_MODE_GET) \
    SW_API_DESC(SW_API_QOS_QU_TX_BUF_ST_SET) \
    SW_API_DESC(SW_API_QOS_QU_TX_BUF_ST_GET) \
    SW_API_DESC(SW_API_QOS_QU_TX_BUF_NR_SET) \
    SW_API_DESC(SW_API_QOS_QU_TX_BUF_NR_GET) \
    SW_API_DESC(SW_API_QOS_PT_TX_BUF_ST_SET) \
    SW_API_DESC(SW_API_QOS_PT_TX_BUF_ST_GET) \
    SW_API_DESC(SW_API_QOS_PT_RED_EN_SET)\
    SW_API_DESC(SW_API_QOS_PT_RED_EN_GET)\
    SW_API_DESC(SW_API_QOS_PT_TX_BUF_NR_SET) \
    SW_API_DESC(SW_API_QOS_PT_TX_BUF_NR_GET) \
    SW_API_DESC(SW_API_QOS_PT_RX_BUF_NR_SET) \
    SW_API_DESC(SW_API_QOS_PT_RX_BUF_NR_GET) \
    SW_API_DESC(SW_API_COSMAP_UP_QU_SET) \
    SW_API_DESC(SW_API_COSMAP_UP_QU_GET) \
    SW_API_DESC(SW_API_COSMAP_DSCP_QU_SET) \
    SW_API_DESC(SW_API_COSMAP_DSCP_QU_GET) \
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
    SW_API_DESC(SW_API_QOS_PT_FORCE_SPRI_ST_SET)  \
    SW_API_DESC(SW_API_QOS_PT_FORCE_SPRI_ST_GET)  \
    SW_API_DESC(SW_API_QOS_PT_FORCE_CPRI_ST_SET)  \
    SW_API_DESC(SW_API_QOS_PT_FORCE_CPRI_ST_GET)  \
    SW_API_DESC(SW_API_QOS_QUEUE_REMARK_SET) \
    SW_API_DESC(SW_API_QOS_QUEUE_REMARK_GET) \
    SW_API_DESC(SW_API_QOS_PORT_GROUP_GET) \
    SW_API_DESC(SW_API_QOS_PORT_GROUP_SET) \
    SW_API_DESC(SW_API_QOS_PORT_PRI_GET) \
    SW_API_DESC(SW_API_QOS_PORT_PRI_SET) \
    SW_API_DESC(SW_API_QOS_PORT_REMARK_GET) \
    SW_API_DESC(SW_API_QOS_PORT_REMARK_SET) \
    SW_API_DESC(SW_API_QOS_PCP_MAP_GET) \
    SW_API_DESC(SW_API_QOS_PCP_MAP_SET) \
    SW_API_DESC(SW_API_QOS_FLOW_MAP_GET) \
    SW_API_DESC(SW_API_QOS_FLOW_MAP_SET) \
    SW_API_DESC(SW_API_QOS_DSCP_MAP_GET) \
    SW_API_DESC(SW_API_QOS_DSCP_MAP_SET) \
    SW_API_DESC(SW_API_QOS_QUEUE_SCHEDULER_GET) \
    SW_API_DESC(SW_API_QOS_QUEUE_SCHEDULER_SET) \
    SW_API_DESC(SW_API_QOS_RING_QUEUE_MAP_GET) \
    SW_API_DESC(SW_API_QOS_RING_QUEUE_MAP_SET) \
    SW_API_DESC(SW_API_QOS_PORT_QUEUES_GET) \
    SW_API_DESC(SW_API_QOS_SCHEDULER_DEQUEU_CTRL_GET) \
    SW_API_DESC(SW_API_QOS_SCHEDULER_DEQUEU_CTRL_SET) \
    SW_API_DESC(SW_API_QOS_PORT_SCHEDULER_CFG_RESET) \
    SW_API_DESC(SW_API_QOS_PORT_SCHEDULER_RESOURCE_GET)
#else
#define QOS_API \
    SW_API_DEF(SW_API_QOS_PT_MODE_SET, fal_qos_port_mode_set),


#define QOS_API_PARAM \
    SW_API_DESC(SW_API_QOS_PT_MODE_SET)

#endif
#else
#define QOS_API
#define QOS_API_PARAM
#endif

#ifdef IN_IGMP
#define IGMP_API \
    SW_API_DEF(SW_API_PT_IGMPS_MODE_SET, fal_port_igmps_status_set), \
    SW_API_DEF(SW_API_PT_IGMPS_MODE_GET, fal_port_igmps_status_get), \
    SW_API_DEF(SW_API_IGMP_MLD_CMD_SET, fal_igmp_mld_cmd_set), \
    SW_API_DEF(SW_API_IGMP_MLD_CMD_GET, fal_igmp_mld_cmd_get), \
    SW_API_DEF(SW_API_IGMP_PT_JOIN_SET, fal_port_igmp_mld_join_set), \
    SW_API_DEF(SW_API_IGMP_PT_JOIN_GET, fal_port_igmp_mld_join_get), \
    SW_API_DEF(SW_API_IGMP_PT_LEAVE_SET, fal_port_igmp_mld_leave_set), \
    SW_API_DEF(SW_API_IGMP_PT_LEAVE_GET, fal_port_igmp_mld_leave_get), \
    SW_API_DEF(SW_API_IGMP_RP_SET, fal_igmp_mld_rp_set), \
    SW_API_DEF(SW_API_IGMP_RP_GET, fal_igmp_mld_rp_get), \
    SW_API_DEF(SW_API_IGMP_ENTRY_CREAT_SET, fal_igmp_mld_entry_creat_set), \
    SW_API_DEF(SW_API_IGMP_ENTRY_CREAT_GET, fal_igmp_mld_entry_creat_get), \
    SW_API_DEF(SW_API_IGMP_ENTRY_STATIC_SET, fal_igmp_mld_entry_static_set), \
    SW_API_DEF(SW_API_IGMP_ENTRY_STATIC_GET, fal_igmp_mld_entry_static_get), \
    SW_API_DEF(SW_API_IGMP_ENTRY_LEAKY_SET, fal_igmp_mld_entry_leaky_set), \
    SW_API_DEF(SW_API_IGMP_ENTRY_LEAKY_GET, fal_igmp_mld_entry_leaky_get), \
    SW_API_DEF(SW_API_IGMP_ENTRY_V3_SET, fal_igmp_mld_entry_v3_set), \
    SW_API_DEF(SW_API_IGMP_ENTRY_V3_GET, fal_igmp_mld_entry_v3_get), \
    SW_API_DEF(SW_API_IGMP_ENTRY_QUEUE_SET, fal_igmp_mld_entry_queue_set), \
    SW_API_DEF(SW_API_IGMP_ENTRY_QUEUE_GET, fal_igmp_mld_entry_queue_get), \
    SW_API_DEF(SW_API_PT_IGMP_LEARN_LIMIT_SET,    fal_port_igmp_mld_learn_limit_set),  \
    SW_API_DEF(SW_API_PT_IGMP_LEARN_LIMIT_GET,    fal_port_igmp_mld_learn_limit_get),  \
    SW_API_DEF(SW_API_PT_IGMP_LEARN_EXCEED_CMD_SET,    fal_port_igmp_mld_learn_exceed_cmd_set),  \
    SW_API_DEF(SW_API_PT_IGMP_LEARN_EXCEED_CMD_GET,    fal_port_igmp_mld_learn_exceed_cmd_get), \
    SW_API_DEF(SW_API_IGMP_SG_ENTRY_SET,    fal_igmp_sg_entry_set),  \
    SW_API_DEF(SW_API_IGMP_SG_ENTRY_CLEAR,    fal_igmp_sg_entry_clear),  \
    SW_API_DEF(SW_API_IGMP_SG_ENTRY_SHOW,    fal_igmp_sg_entry_show),  \
    SW_API_DEF(SW_API_IGMP_SG_ENTRY_QUERY,    fal_igmp_sg_entry_query),

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
    SW_API_DESC(SW_API_IGMP_ENTRY_CREAT_GET) \
    SW_API_DESC(SW_API_IGMP_ENTRY_STATIC_SET) \
    SW_API_DESC(SW_API_IGMP_ENTRY_STATIC_GET) \
    SW_API_DESC(SW_API_IGMP_ENTRY_LEAKY_SET) \
    SW_API_DESC(SW_API_IGMP_ENTRY_LEAKY_GET) \
    SW_API_DESC(SW_API_IGMP_ENTRY_V3_SET) \
    SW_API_DESC(SW_API_IGMP_ENTRY_V3_GET) \
    SW_API_DESC(SW_API_IGMP_ENTRY_QUEUE_SET) \
    SW_API_DESC(SW_API_IGMP_ENTRY_QUEUE_GET) \
    SW_API_DESC(SW_API_PT_IGMP_LEARN_LIMIT_SET) \
    SW_API_DESC(SW_API_PT_IGMP_LEARN_LIMIT_GET) \
    SW_API_DESC(SW_API_PT_IGMP_LEARN_EXCEED_CMD_SET) \
    SW_API_DESC(SW_API_PT_IGMP_LEARN_EXCEED_CMD_GET) \
    SW_API_DESC(SW_API_IGMP_SG_ENTRY_SET) \
    SW_API_DESC(SW_API_IGMP_SG_ENTRY_CLEAR) \
    SW_API_DESC(SW_API_IGMP_SG_ENTRY_SHOW) \
    SW_API_DESC(SW_API_IGMP_SG_ENTRY_QUERY)
#else
#define IGMP_API
#define IGMP_API_PARAM
#endif

#ifdef IN_LEAKY
#define LEAKY_API \
    SW_API_DEF(SW_API_UC_LEAKY_MODE_SET, fal_uc_leaky_mode_set), \
    SW_API_DEF(SW_API_UC_LEAKY_MODE_GET, fal_uc_leaky_mode_get), \
    SW_API_DEF(SW_API_MC_LEAKY_MODE_SET, fal_mc_leaky_mode_set), \
    SW_API_DEF(SW_API_MC_LEAKY_MODE_GET, fal_mc_leaky_mode_get), \
    SW_API_DEF(SW_API_ARP_LEAKY_MODE_SET, fal_port_arp_leaky_set), \
    SW_API_DEF(SW_API_ARP_LEAKY_MODE_GET, fal_port_arp_leaky_get), \
    SW_API_DEF(SW_API_PT_UC_LEAKY_MODE_SET, fal_port_uc_leaky_set), \
    SW_API_DEF(SW_API_PT_UC_LEAKY_MODE_GET, fal_port_uc_leaky_get), \
    SW_API_DEF(SW_API_PT_MC_LEAKY_MODE_SET, fal_port_mc_leaky_set), \
    SW_API_DEF(SW_API_PT_MC_LEAKY_MODE_GET, fal_port_mc_leaky_get),

#define LEAKY_API_PARAM \
    SW_API_DESC(SW_API_UC_LEAKY_MODE_SET) \
    SW_API_DESC(SW_API_UC_LEAKY_MODE_GET) \
    SW_API_DESC(SW_API_MC_LEAKY_MODE_SET) \
    SW_API_DESC(SW_API_MC_LEAKY_MODE_GET) \
    SW_API_DESC(SW_API_ARP_LEAKY_MODE_SET)\
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
    SW_API_DEF(SW_API_MIRROR_ANALY_PT_SET, fal_mirr_analysis_port_set), \
    SW_API_DEF(SW_API_MIRROR_ANALY_PT_GET, fal_mirr_analysis_port_get), \
    SW_API_DEF(SW_API_MIRROR_IN_PT_SET, fal_mirr_port_in_set), \
    SW_API_DEF(SW_API_MIRROR_IN_PT_GET, fal_mirr_port_in_get), \
    SW_API_DEF(SW_API_MIRROR_EG_PT_SET, fal_mirr_port_eg_set), \
    SW_API_DEF(SW_API_MIRROR_EG_PT_GET, fal_mirr_port_eg_get), \
    SW_API_DEF(SW_API_MIRROR_ANALYSIS_CONFIG_SET, fal_mirr_analysis_config_set), \
    SW_API_DEF(SW_API_MIRROR_ANALYSIS_CONFIG_GET, fal_mirr_analysis_config_get),

#define MIRROR_API_PARAM \
    SW_API_DESC(SW_API_MIRROR_ANALY_PT_SET) \
    SW_API_DESC(SW_API_MIRROR_ANALY_PT_GET) \
    SW_API_DESC(SW_API_MIRROR_IN_PT_SET) \
    SW_API_DESC(SW_API_MIRROR_IN_PT_GET) \
    SW_API_DESC(SW_API_MIRROR_EG_PT_SET) \
    SW_API_DESC(SW_API_MIRROR_EG_PT_GET) \
    SW_API_DESC(SW_API_MIRROR_ANALYSIS_CONFIG_SET) \
    SW_API_DESC(SW_API_MIRROR_ANALYSIS_CONFIG_GET)

#else
#define MIRROR_API
#define MIRROR_API_PARAM
#endif

#ifdef IN_RATE
#define RATE_API \
    SW_API_DEF(SW_API_RATE_QU_EGRL_SET, fal_rate_queue_egrl_set), \
    SW_API_DEF(SW_API_RATE_QU_EGRL_GET, fal_rate_queue_egrl_get), \
    SW_API_DEF(SW_API_RATE_PT_EGRL_SET, fal_rate_port_egrl_set), \
    SW_API_DEF(SW_API_RATE_PT_EGRL_GET, fal_rate_port_egrl_get), \
    SW_API_DEF(SW_API_RATE_PT_INRL_SET, fal_rate_port_inrl_set), \
    SW_API_DEF(SW_API_RATE_PT_INRL_GET, fal_rate_port_inrl_get), \
    SW_API_DEF(SW_API_STORM_CTRL_FRAME_SET, fal_storm_ctrl_frame_set), \
    SW_API_DEF(SW_API_STORM_CTRL_FRAME_GET, fal_storm_ctrl_frame_get), \
    SW_API_DEF(SW_API_STORM_CTRL_RATE_SET, fal_storm_ctrl_rate_set), \
    SW_API_DEF(SW_API_STORM_CTRL_RATE_GET, fal_storm_ctrl_rate_get), \
    SW_API_DEF(SW_API_RATE_PORT_POLICER_SET, fal_rate_port_policer_set), \
    SW_API_DEF(SW_API_RATE_PORT_POLICER_GET, fal_rate_port_policer_get), \
    SW_API_DEF(SW_API_RATE_PORT_SHAPER_SET, fal_rate_port_shaper_set), \
    SW_API_DEF(SW_API_RATE_PORT_SHAPER_GET, fal_rate_port_shaper_get), \
    SW_API_DEF(SW_API_RATE_QUEUE_SHAPER_SET, fal_rate_queue_shaper_set), \
    SW_API_DEF(SW_API_RATE_QUEUE_SHAPER_GET, fal_rate_queue_shaper_get), \
    SW_API_DEF(SW_API_RATE_ACL_POLICER_SET, fal_rate_acl_policer_set), \
    SW_API_DEF(SW_API_RATE_ACL_POLICER_GET, fal_rate_acl_policer_get), \
    SW_API_DEF(SW_API_RATE_PT_ADDRATEBYTE_SET, fal_rate_port_add_rate_byte_set), \
    SW_API_DEF(SW_API_RATE_PT_ADDRATEBYTE_GET, fal_rate_port_add_rate_byte_get), \
    SW_API_DEF(SW_API_RATE_PT_GOL_FLOW_EN_SET, fal_rate_port_gol_flow_en_set),  \
    SW_API_DEF(SW_API_RATE_PT_GOL_FLOW_EN_GET, fal_rate_port_gol_flow_en_get),

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
    SW_API_DESC(SW_API_STORM_CTRL_RATE_GET) \
    SW_API_DESC(SW_API_RATE_PORT_POLICER_SET) \
    SW_API_DESC(SW_API_RATE_PORT_POLICER_GET) \
    SW_API_DESC(SW_API_RATE_PORT_SHAPER_SET) \
    SW_API_DESC(SW_API_RATE_PORT_SHAPER_GET) \
    SW_API_DESC(SW_API_RATE_QUEUE_SHAPER_SET) \
    SW_API_DESC(SW_API_RATE_QUEUE_SHAPER_GET) \
    SW_API_DESC(SW_API_RATE_ACL_POLICER_SET) \
    SW_API_DESC(SW_API_RATE_ACL_POLICER_GET) \
    SW_API_DESC(SW_API_RATE_PT_ADDRATEBYTE_SET) \
    SW_API_DESC(SW_API_RATE_PT_ADDRATEBYTE_GET) \
    SW_API_DESC(SW_API_RATE_PT_GOL_FLOW_EN_SET) \
    SW_API_DESC(SW_API_RATE_PT_GOL_FLOW_EN_GET)
#else
#define RATE_API
#define RATE_API_PARAM
#endif

#ifdef IN_STP
#define STP_API \
    SW_API_DEF(SW_API_STP_PT_STATE_SET, fal_stp_port_state_set), \
    SW_API_DEF(SW_API_STP_PT_STATE_GET, fal_stp_port_state_get),

#define STP_API_PARAM \
    SW_API_DESC(SW_API_STP_PT_STATE_SET) \
    SW_API_DESC(SW_API_STP_PT_STATE_GET)
#else
#define STP_API
#define STP_API_PARAM
#endif

#ifdef IN_MIB
#define MIB_API \
    SW_API_DEF(SW_API_PT_MIB_GET, fal_get_mib_info), \
    SW_API_DEF(SW_API_MIB_STATUS_SET, fal_mib_status_set), \
    SW_API_DEF(SW_API_MIB_STATUS_GET, fal_mib_status_get), \
    SW_API_DEF(SW_API_PT_MIB_FLUSH_COUNTERS, fal_mib_port_flush_counters), \
    SW_API_DEF(SW_API_MIB_CPU_KEEP_SET, fal_mib_cpukeep_set), \
    SW_API_DEF(SW_API_MIB_CPU_KEEP_GET, fal_mib_cpukeep_get),\
    SW_API_DEF(SW_API_PT_XGMIB_GET, fal_get_xgmib_info),\
    SW_API_DEF(SW_API_PT_MIB_COUNTER_GET, fal_mib_counter_get),

#define MIB_API_PARAM \
    SW_API_DESC(SW_API_PT_MIB_GET)  \
    SW_API_DESC(SW_API_PT_XGMIB_GET) \
    SW_API_DESC(SW_API_MIB_STATUS_SET) \
    SW_API_DESC(SW_API_MIB_STATUS_GET) \
    SW_API_DESC(SW_API_PT_MIB_FLUSH_COUNTERS) \
    SW_API_DESC(SW_API_MIB_CPU_KEEP_SET) \
    SW_API_DESC(SW_API_MIB_CPU_KEEP_GET) \
    SW_API_DESC(SW_API_PT_MIB_COUNTER_GET)
#else
#define MIB_API
#define MIB_API_PARAM
#endif

#ifdef IN_MISC
#ifndef IN_MISC_MINI
#define MISC_API \
    SW_API_DEF(SW_API_ARP_STATUS_SET, fal_arp_status_set), \
    SW_API_DEF(SW_API_ARP_STATUS_GET, fal_arp_status_get), \
    SW_API_DEF(SW_API_FRAME_MAX_SIZE_SET, fal_frame_max_size_set), \
    SW_API_DEF(SW_API_FRAME_MAX_SIZE_GET, fal_frame_max_size_get), \
    SW_API_DEF(SW_API_PT_UNK_SA_CMD_SET, fal_port_unk_sa_cmd_set), \
    SW_API_DEF(SW_API_PT_UNK_SA_CMD_GET, fal_port_unk_sa_cmd_get), \
    SW_API_DEF(SW_API_PT_UNK_UC_FILTER_SET, fal_port_unk_uc_filter_set), \
    SW_API_DEF(SW_API_PT_UNK_UC_FILTER_GET, fal_port_unk_uc_filter_get), \
    SW_API_DEF(SW_API_PT_UNK_MC_FILTER_SET, fal_port_unk_mc_filter_set), \
    SW_API_DEF(SW_API_PT_UNK_MC_FILTER_GET, fal_port_unk_mc_filter_get), \
    SW_API_DEF(SW_API_PT_BC_FILTER_SET, fal_port_bc_filter_set), \
    SW_API_DEF(SW_API_PT_BC_FILTER_GET, fal_port_bc_filter_get), \
    SW_API_DEF(SW_API_CPU_PORT_STATUS_SET, fal_cpu_port_status_set), \
    SW_API_DEF(SW_API_CPU_PORT_STATUS_GET, fal_cpu_port_status_get), \
    SW_API_DEF(SW_API_BC_TO_CPU_PORT_SET, fal_bc_to_cpu_port_set), \
    SW_API_DEF(SW_API_BC_TO_CPU_PORT_GET, fal_bc_to_cpu_port_get), \
    SW_API_DEF(SW_API_PT_DHCP_SET, fal_port_dhcp_set), \
    SW_API_DEF(SW_API_PT_DHCP_GET, fal_port_dhcp_get), \
    SW_API_DEF(SW_API_ARP_CMD_SET, fal_arp_cmd_set), \
    SW_API_DEF(SW_API_ARP_CMD_GET, fal_arp_cmd_get), \
    SW_API_DEF(SW_API_EAPOL_CMD_SET, fal_eapol_cmd_set), \
    SW_API_DEF(SW_API_EAPOL_CMD_GET, fal_eapol_cmd_get), \
    SW_API_DEF(SW_API_EAPOL_STATUS_SET, fal_eapol_status_set), \
    SW_API_DEF(SW_API_EAPOL_STATUS_GET, fal_eapol_status_get), \
    SW_API_DEF(SW_API_RIPV1_STATUS_SET, fal_ripv1_status_set), \
    SW_API_DEF(SW_API_RIPV1_STATUS_GET, fal_ripv1_status_get), \
    SW_API_DEF(SW_API_PT_ARP_REQ_STATUS_SET, fal_port_arp_req_status_set), \
    SW_API_DEF(SW_API_PT_ARP_REQ_STATUS_GET, fal_port_arp_req_status_get), \
    SW_API_DEF(SW_API_PT_ARP_ACK_STATUS_SET, fal_port_arp_ack_status_set), \
    SW_API_DEF(SW_API_PT_ARP_ACK_STATUS_GET, fal_port_arp_ack_status_get), \
    SW_API_DEF(SW_API_INTR_MASK_SET, fal_intr_mask_set), \
    SW_API_DEF(SW_API_INTR_MASK_GET, fal_intr_mask_get), \
    SW_API_DEF(SW_API_INTR_STATUS_GET, fal_intr_status_get),   \
    SW_API_DEF(SW_API_INTR_STATUS_CLEAR, fal_intr_status_clear), \
    SW_API_DEF(SW_API_INTR_PORT_LINK_MASK_SET, fal_intr_port_link_mask_set), \
    SW_API_DEF(SW_API_INTR_PORT_LINK_MASK_GET, fal_intr_port_link_mask_get), \
    SW_API_DEF(SW_API_INTR_PORT_LINK_STATUS_GET, fal_intr_port_link_status_get), \
    SW_API_DEF(SW_API_INTR_MASK_MAC_LINKCHG_SET, fal_intr_mask_mac_linkchg_set), \
    SW_API_DEF(SW_API_INTR_MASK_MAC_LINKCHG_GET, fal_intr_mask_mac_linkchg_get), \
    SW_API_DEF(SW_API_INTR_STATUS_MAC_LINKCHG_GET, fal_intr_status_mac_linkchg_get), \
    SW_API_DEF(SW_API_INTR_STATUS_MAC_LINKCHG_CLEAR, fal_intr_status_mac_linkchg_clear), \
    SW_API_DEF(SW_API_CPU_VID_EN_SET, fal_cpu_vid_en_set), \
    SW_API_DEF(SW_API_CPU_VID_EN_GET, fal_cpu_vid_en_get), \
    SW_API_DEF(SW_API_GLOBAL_MACADDR_SET, fal_global_macaddr_set), \
    SW_API_DEF(SW_API_GLOBAL_MACADDR_GET, fal_global_macaddr_get), \
    SW_API_DEF(SW_API_LLDP_STATUS_SET, fal_lldp_status_set), \
    SW_API_DEF(SW_API_LLDP_STATUS_GET, fal_lldp_status_get), \
    SW_API_DEF(SW_API_FRAME_CRC_RESERVE_SET, fal_frame_crc_reserve_set), \
    SW_API_DEF(SW_API_FRAME_CRC_RESERVE_GET, fal_frame_crc_reserve_get), \
    SW_API_DEF(SW_API_DEBUG_PORT_COUNTER_ENABLE, fal_debug_port_counter_enable), \
    SW_API_DEF(SW_API_DEBUG_PORT_COUNTER_STATUS_GET, fal_debug_port_counter_status_get),



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
    SW_API_DESC(SW_API_PT_BC_FILTER_SET) \
    SW_API_DESC(SW_API_PT_BC_FILTER_GET) \
    SW_API_DESC(SW_API_CPU_PORT_STATUS_SET) \
    SW_API_DESC(SW_API_CPU_PORT_STATUS_GET) \
    SW_API_DESC(SW_API_BC_TO_CPU_PORT_SET) \
    SW_API_DESC(SW_API_BC_TO_CPU_PORT_GET) \
    SW_API_DESC(SW_API_PT_DHCP_SET) \
    SW_API_DESC(SW_API_PT_DHCP_GET) \
    SW_API_DESC(SW_API_ARP_CMD_SET) \
    SW_API_DESC(SW_API_ARP_CMD_GET) \
    SW_API_DESC(SW_API_EAPOL_CMD_SET) \
    SW_API_DESC(SW_API_EAPOL_CMD_GET) \
    SW_API_DESC(SW_API_EAPOL_STATUS_SET) \
    SW_API_DESC(SW_API_EAPOL_STATUS_GET) \
    SW_API_DESC(SW_API_RIPV1_STATUS_SET) \
    SW_API_DESC(SW_API_RIPV1_STATUS_GET) \
    SW_API_DESC(SW_API_PT_ARP_REQ_STATUS_SET) \
    SW_API_DESC(SW_API_PT_ARP_REQ_STATUS_GET) \
    SW_API_DESC(SW_API_PT_ARP_ACK_STATUS_SET) \
    SW_API_DESC(SW_API_PT_ARP_ACK_STATUS_GET) \
    SW_API_DESC(SW_API_INTR_MASK_SET) \
    SW_API_DESC(SW_API_INTR_MASK_GET) \
    SW_API_DESC(SW_API_INTR_STATUS_GET)   \
    SW_API_DESC(SW_API_INTR_STATUS_CLEAR) \
    SW_API_DESC(SW_API_INTR_PORT_LINK_MASK_SET) \
    SW_API_DESC(SW_API_INTR_PORT_LINK_MASK_GET) \
    SW_API_DESC(SW_API_INTR_PORT_LINK_STATUS_GET) \
    SW_API_DESC(SW_API_INTR_MASK_MAC_LINKCHG_SET) \
    SW_API_DESC(SW_API_INTR_MASK_MAC_LINKCHG_GET) \
    SW_API_DESC(SW_API_INTR_STATUS_MAC_LINKCHG_GET) \
    SW_API_DESC(SW_API_INTR_STATUS_MAC_LINKCHG_CLEAR) \
    SW_API_DESC(SW_API_CPU_VID_EN_SET) \
    SW_API_DESC(SW_API_CPU_VID_EN_GET) \
    SW_API_DESC(SW_API_GLOBAL_MACADDR_SET) \
    SW_API_DESC(SW_API_GLOBAL_MACADDR_GET) \
    SW_API_DESC(SW_API_LLDP_STATUS_SET) \
    SW_API_DESC(SW_API_LLDP_STATUS_GET) \
    SW_API_DESC(SW_API_FRAME_CRC_RESERVE_SET) \
    SW_API_DESC(SW_API_FRAME_CRC_RESERVE_GET) \
    SW_API_DESC(SW_API_DEBUG_PORT_COUNTER_ENABLE) \
    SW_API_DESC(SW_API_DEBUG_PORT_COUNTER_STATUS_GET)
#else
#define MISC_API \
    SW_API_DEF(SW_API_PT_UNK_SA_CMD_SET, fal_port_unk_sa_cmd_set), \
    SW_API_DEF(SW_API_PT_UNK_UC_FILTER_SET, fal_port_unk_uc_filter_set), \
    SW_API_DEF(SW_API_PT_UNK_MC_FILTER_SET, fal_port_unk_mc_filter_set), \
    SW_API_DEF(SW_API_PT_BC_FILTER_SET, fal_port_bc_filter_set), \
    SW_API_DEF(SW_API_EAPOL_STATUS_SET, fal_eapol_status_set), \
    SW_API_DEF(SW_API_EAPOL_CMD_SET, fal_eapol_cmd_set), \
    SW_API_DEF(SW_API_CPU_PORT_STATUS_SET, fal_cpu_port_status_set),

#define MISC_API_PARAM \
    SW_API_DESC(SW_API_PT_UNK_SA_CMD_SET) \
    SW_API_DESC(SW_API_PT_UNK_UC_FILTER_SET) \
    SW_API_DESC(SW_API_PT_UNK_MC_FILTER_SET) \
    SW_API_DESC(SW_API_PT_BC_FILTER_SET) \
    SW_API_DESC(SW_API_EAPOL_CMD_SET) \
    SW_API_DESC(SW_API_EAPOL_STATUS_SET) \
    SW_API_DESC(SW_API_CPU_PORT_STATUS_SET)

#endif

#else
#define MISC_API
#define MISC_API_PARAM
#endif

#ifdef IN_LED
#define LED_API \
    SW_API_DEF(SW_API_LED_PATTERN_SET, fal_led_ctrl_pattern_set), \
    SW_API_DEF(SW_API_LED_PATTERN_GET, fal_led_ctrl_pattern_get),

#define LED_API_PARAM \
    SW_API_DESC(SW_API_LED_PATTERN_SET) \
    SW_API_DESC(SW_API_LED_PATTERN_GET)
#else
#define LED_API
#define LED_API_PARAM
#endif

#ifdef IN_COSMAP
#ifndef IN_COSMAP_MINI
#define COSMAP_API \
    SW_API_DEF(SW_API_COSMAP_DSCP_TO_PRI_SET, fal_cosmap_dscp_to_pri_set), \
    SW_API_DEF(SW_API_COSMAP_DSCP_TO_PRI_GET, fal_cosmap_dscp_to_pri_get), \
    SW_API_DEF(SW_API_COSMAP_DSCP_TO_DP_SET, fal_cosmap_dscp_to_dp_set), \
    SW_API_DEF(SW_API_COSMAP_DSCP_TO_DP_GET, fal_cosmap_dscp_to_dp_get), \
    SW_API_DEF(SW_API_COSMAP_UP_TO_PRI_SET, fal_cosmap_up_to_pri_set), \
    SW_API_DEF(SW_API_COSMAP_UP_TO_PRI_GET, fal_cosmap_up_to_pri_get), \
    SW_API_DEF(SW_API_COSMAP_UP_TO_DP_SET, fal_cosmap_up_to_dp_set), \
    SW_API_DEF(SW_API_COSMAP_UP_TO_DP_GET, fal_cosmap_up_to_dp_get), \
    SW_API_DEF(SW_API_COSMAP_PRI_TO_QU_SET, fal_cosmap_pri_to_queue_set), \
    SW_API_DEF(SW_API_COSMAP_PRI_TO_QU_GET, fal_cosmap_pri_to_queue_get), \
    SW_API_DEF(SW_API_COSMAP_PRI_TO_EHQU_SET, fal_cosmap_pri_to_ehqueue_set), \
    SW_API_DEF(SW_API_COSMAP_PRI_TO_EHQU_GET, fal_cosmap_pri_to_ehqueue_get), \
    SW_API_DEF(SW_API_COSMAP_EG_REMARK_SET, fal_cosmap_egress_remark_set), \
    SW_API_DEF(SW_API_COSMAP_EG_REMARK_GET, fal_cosmap_egress_remark_get), \
    SW_API_DEF(SW_API_COSMAP_DSCP_TO_EHPRI_SET, fal_cosmap_dscp_to_ehpri_set), \
    SW_API_DEF(SW_API_COSMAP_DSCP_TO_EHPRI_GET, fal_cosmap_dscp_to_ehpri_get), \
    SW_API_DEF(SW_API_COSMAP_DSCP_TO_EHDP_SET, fal_cosmap_dscp_to_ehdp_set), \
    SW_API_DEF(SW_API_COSMAP_DSCP_TO_EHDP_GET, fal_cosmap_dscp_to_ehdp_get), \
    SW_API_DEF(SW_API_COSMAP_UP_TO_EHPRI_SET, fal_cosmap_up_to_ehpri_set), \
    SW_API_DEF(SW_API_COSMAP_UP_TO_EHPRI_GET, fal_cosmap_up_to_ehpri_get), \
    SW_API_DEF(SW_API_COSMAP_UP_TO_EHDP_SET, fal_cosmap_up_to_ehdp_set), \
    SW_API_DEF(SW_API_COSMAP_UP_TO_EHDP_GET, fal_cosmap_up_to_ehdp_get),

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
    SW_API_DESC(SW_API_COSMAP_PRI_TO_EHQU_GET) \
    SW_API_DESC(SW_API_COSMAP_EG_REMARK_SET) \
    SW_API_DESC(SW_API_COSMAP_EG_REMARK_GET) \
    SW_API_DESC(SW_API_COSMAP_DSCP_TO_EHPRI_SET)  \
    SW_API_DESC(SW_API_COSMAP_DSCP_TO_EHPRI_GET)  \
    SW_API_DESC(SW_API_COSMAP_DSCP_TO_EHDP_SET)  \
    SW_API_DESC(SW_API_COSMAP_DSCP_TO_EHDP_GET)  \
    SW_API_DESC(SW_API_COSMAP_UP_TO_EHPRI_SET)  \
    SW_API_DESC(SW_API_COSMAP_UP_TO_EHPRI_GET)  \
    SW_API_DESC(SW_API_COSMAP_UP_TO_EHDP_SET)  \
    SW_API_DESC(SW_API_COSMAP_UP_TO_EHDP_GET)
#else
#define COSMAP_API \
    SW_API_DEF(SW_API_COSMAP_PRI_TO_QU_SET, fal_cosmap_pri_to_queue_set), \
    SW_API_DEF(SW_API_COSMAP_PRI_TO_EHQU_SET, fal_cosmap_pri_to_ehqueue_set),

#define COSMAP_API_PARAM \
    SW_API_DESC(SW_API_COSMAP_PRI_TO_QU_SET)  \
    SW_API_DESC(SW_API_COSMAP_PRI_TO_EHQU_SET)
#endif
#else
#define COSMAP_API
#define COSMAP_API_PARAM
#endif

#ifdef IN_SEC
#define SEC_API \
    SW_API_DEF(SW_API_SEC_NORM_SET, fal_sec_norm_item_set), \
    SW_API_DEF(SW_API_SEC_NORM_GET, fal_sec_norm_item_get), \
    SW_API_DEF(SW_API_SEC_MAC_SET, fal_sec_norm_item_set), \
    SW_API_DEF(SW_API_SEC_MAC_GET, fal_sec_norm_item_get), \
    SW_API_DEF(SW_API_SEC_IP_SET, fal_sec_norm_item_set), \
    SW_API_DEF(SW_API_SEC_IP_GET, fal_sec_norm_item_get), \
    SW_API_DEF(SW_API_SEC_IP4_SET, fal_sec_norm_item_set), \
    SW_API_DEF(SW_API_SEC_IP4_GET, fal_sec_norm_item_get), \
    SW_API_DEF(SW_API_SEC_IP6_SET, fal_sec_norm_item_set), \
    SW_API_DEF(SW_API_SEC_IP6_GET, fal_sec_norm_item_get), \
    SW_API_DEF(SW_API_SEC_TCP_SET, fal_sec_norm_item_set), \
    SW_API_DEF(SW_API_SEC_TCP_GET, fal_sec_norm_item_get), \
    SW_API_DEF(SW_API_SEC_UDP_SET, fal_sec_norm_item_set), \
    SW_API_DEF(SW_API_SEC_UDP_GET, fal_sec_norm_item_get), \
    SW_API_DEF(SW_API_SEC_ICMP4_SET, fal_sec_norm_item_set), \
    SW_API_DEF(SW_API_SEC_ICMP4_GET, fal_sec_norm_item_get), \
    SW_API_DEF(SW_API_SEC_ICMP6_SET, fal_sec_norm_item_set), \
    SW_API_DEF(SW_API_SEC_ICMP6_GET, fal_sec_norm_item_get), \
    SW_API_DEF(SW_API_SEC_L3_PARSER_CTRL_GET, fal_sec_l3_excep_parser_ctrl_get), \
    SW_API_DEF(SW_API_SEC_L3_PARSER_CTRL_SET, fal_sec_l3_excep_parser_ctrl_set), \
    SW_API_DEF(SW_API_SEC_L4_PARSER_CTRL_GET, fal_sec_l4_excep_parser_ctrl_get), \
    SW_API_DEF(SW_API_SEC_L4_PARSER_CTRL_SET, fal_sec_l4_excep_parser_ctrl_set), \
    SW_API_DEF(SW_API_SEC_EXP_CTRL_GET, fal_sec_l3_excep_ctrl_get), \
    SW_API_DEF(SW_API_SEC_EXP_CTRL_SET, fal_sec_l3_excep_ctrl_set),

#define SEC_API_PARAM \
    SW_API_DESC(SW_API_SEC_NORM_SET) \
    SW_API_DESC(SW_API_SEC_NORM_GET) \
    SW_API_DESC(SW_API_SEC_MAC_SET) \
    SW_API_DESC(SW_API_SEC_MAC_GET) \
    SW_API_DESC(SW_API_SEC_IP_SET) \
    SW_API_DESC(SW_API_SEC_IP_GET) \
    SW_API_DESC(SW_API_SEC_IP4_SET) \
    SW_API_DESC(SW_API_SEC_IP4_GET) \
    SW_API_DESC(SW_API_SEC_IP6_SET) \
    SW_API_DESC(SW_API_SEC_IP6_GET) \
    SW_API_DESC(SW_API_SEC_TCP_SET) \
    SW_API_DESC(SW_API_SEC_TCP_GET) \
    SW_API_DESC(SW_API_SEC_UDP_SET) \
    SW_API_DESC(SW_API_SEC_UDP_GET) \
    SW_API_DESC(SW_API_SEC_ICMP4_SET) \
    SW_API_DESC(SW_API_SEC_ICMP4_GET) \
    SW_API_DESC(SW_API_SEC_ICMP6_SET) \
    SW_API_DESC(SW_API_SEC_ICMP6_GET) \
    SW_API_DESC(SW_API_SEC_L3_PARSER_CTRL_GET) \
    SW_API_DESC(SW_API_SEC_L3_PARSER_CTRL_SET) \
    SW_API_DESC(SW_API_SEC_L4_PARSER_CTRL_GET) \
    SW_API_DESC(SW_API_SEC_L4_PARSER_CTRL_SET) \
    SW_API_DESC(SW_API_SEC_EXP_CTRL_GET) \
    SW_API_DESC(SW_API_SEC_EXP_CTRL_SET)
#else
#define SEC_API
#define SEC_API_PARAM
#endif

#ifdef IN_IP
#ifndef IN_IP_MINI
#define IP_API \
    SW_API_DEF(SW_API_IP_HOST_ADD, fal_ip_host_add), \
    SW_API_DEF(SW_API_IP_HOST_DEL, fal_ip_host_del), \
    SW_API_DEF(SW_API_IP_HOST_GET, fal_ip_host_get), \
    SW_API_DEF(SW_API_IP_HOST_NEXT, fal_ip_host_next), \
    SW_API_DEF(SW_API_IP_HOST_COUNTER_BIND, fal_ip_host_counter_bind), \
    SW_API_DEF(SW_API_IP_HOST_PPPOE_BIND, fal_ip_host_pppoe_bind), \
    SW_API_DEF(SW_API_IP_PT_ARP_LEARN_SET, fal_ip_pt_arp_learn_set), \
    SW_API_DEF(SW_API_IP_PT_ARP_LEARN_GET, fal_ip_pt_arp_learn_get), \
    SW_API_DEF(SW_API_IP_ARP_LEARN_SET, fal_ip_arp_learn_set), \
    SW_API_DEF(SW_API_IP_ARP_LEARN_GET, fal_ip_arp_learn_get), \
    SW_API_DEF(SW_API_IP_SOURCE_GUARD_SET, fal_ip_source_guard_set), \
    SW_API_DEF(SW_API_IP_SOURCE_GUARD_GET, fal_ip_source_guard_get), \
    SW_API_DEF(SW_API_IP_ARP_GUARD_SET, fal_ip_arp_guard_set), \
    SW_API_DEF(SW_API_IP_ARP_GUARD_GET, fal_ip_arp_guard_get), \
    SW_API_DEF(SW_API_IP_ROUTE_STATUS_SET, fal_ip_route_status_set), \
    SW_API_DEF(SW_API_IP_ROUTE_STATUS_GET, fal_ip_route_status_get), \
    SW_API_DEF(SW_API_IP_INTF_ENTRY_ADD, fal_ip_intf_entry_add), \
    SW_API_DEF(SW_API_IP_INTF_ENTRY_DEL, fal_ip_intf_entry_del), \
    SW_API_DEF(SW_API_IP_INTF_ENTRY_NEXT, fal_ip_intf_entry_next), \
    SW_API_DEF(SW_API_IP_UNK_SOURCE_CMD_SET, fal_ip_unk_source_cmd_set), \
    SW_API_DEF(SW_API_IP_UNK_SOURCE_CMD_GET, fal_ip_unk_source_cmd_get), \
    SW_API_DEF(SW_API_ARP_UNK_SOURCE_CMD_SET, fal_arp_unk_source_cmd_set), \
    SW_API_DEF(SW_API_ARP_UNK_SOURCE_CMD_GET, fal_arp_unk_source_cmd_get), \
    SW_API_DEF(SW_API_IP_AGE_TIME_SET, fal_ip_age_time_set), \
    SW_API_DEF(SW_API_IP_AGE_TIME_GET, fal_ip_age_time_get), \
    SW_API_DEF(SW_API_WCMP_HASH_MODE_SET, fal_ip_wcmp_hash_mode_set), \
    SW_API_DEF(SW_API_WCMP_HASH_MODE_GET, fal_ip_wcmp_hash_mode_get),  \
    SW_API_DEF(SW_API_IP_VRF_BASE_ADDR_SET, fal_ip_vrf_base_addr_set), \
    SW_API_DEF(SW_API_IP_VRF_BASE_ADDR_GET, fal_ip_vrf_base_addr_get), \
    SW_API_DEF(SW_API_IP_VRF_BASE_MASK_SET, fal_ip_vrf_base_mask_set), \
    SW_API_DEF(SW_API_IP_VRF_BASE_MASK_GET, fal_ip_vrf_base_mask_get), \
    SW_API_DEF(SW_API_IP_DEFAULT_ROUTE_SET, fal_ip_default_route_set), \
    SW_API_DEF(SW_API_IP_DEFAULT_ROUTE_GET, fal_ip_default_route_get), \
    SW_API_DEF(SW_API_IP_HOST_ROUTE_SET, fal_ip_host_route_set), \
    SW_API_DEF(SW_API_IP_HOST_ROUTE_GET, fal_ip_host_route_get), \
    SW_API_DEF(SW_API_IP_WCMP_ENTRY_SET, fal_ip_wcmp_entry_set), \
    SW_API_DEF(SW_API_IP_WCMP_ENTRY_GET, fal_ip_wcmp_entry_get), \
    SW_API_DEF(SW_API_IP_RFS_IP4_SET,    fal_ip_rfs_ip4_rule_set), \
	SW_API_DEF(SW_API_IP_RFS_IP6_SET,    fal_ip_rfs_ip6_rule_set), \
	SW_API_DEF(SW_API_IP_RFS_IP4_DEL,    fal_ip_rfs_ip4_rule_del), \
	SW_API_DEF(SW_API_IP_RFS_IP6_DEL,    fal_ip_rfs_ip6_rule_del), \
    SW_API_DEF(SW_API_IP_DEFAULT_FLOW_CMD_SET, fal_default_flow_cmd_set), \
    SW_API_DEF(SW_API_IP_DEFAULT_FLOW_CMD_GET, fal_default_flow_cmd_get), \
    SW_API_DEF(SW_API_IP_DEFAULT_RT_FLOW_CMD_SET, fal_default_rt_flow_cmd_set), \
    SW_API_DEF(SW_API_IP_DEFAULT_RT_FLOW_CMD_GET, fal_default_rt_flow_cmd_get), \
    SW_API_DEF(SW_API_IP_VIS_ARP_SG_CFG_GET, fal_ip_vsi_arp_sg_cfg_get), \
    SW_API_DEF(SW_API_IP_VIS_ARP_SG_CFG_SET, fal_ip_vsi_arp_sg_cfg_set), \
    SW_API_DEF(SW_API_IP_NETWORK_ROUTE_GET, fal_ip_network_route_get), \
    SW_API_DEF(SW_API_IP_NETWORK_ROUTE_ADD, fal_ip_network_route_add), \
    SW_API_DEF(SW_API_IP_INTF_GET, fal_ip_intf_get), \
    SW_API_DEF(SW_API_IP_INTF_SET, fal_ip_intf_set), \
    SW_API_DEF(SW_API_IP_VSI_INTF_GET, fal_ip_vsi_intf_get), \
    SW_API_DEF(SW_API_IP_VSI_INTF_SET, fal_ip_vsi_intf_set), \
    SW_API_DEF(SW_API_IP_NEXTHOP_GET, fal_ip_nexthop_get), \
    SW_API_DEF(SW_API_IP_NEXTHOP_SET, fal_ip_nexthop_set), \
    SW_API_DEF(SW_API_IP_VSI_SG_SET, fal_ip_vsi_sg_cfg_set), \
    SW_API_DEF(SW_API_IP_VSI_SG_GET, fal_ip_vsi_sg_cfg_get), \
    SW_API_DEF(SW_API_IP_PORT_SG_SET, fal_ip_port_sg_cfg_set), \
    SW_API_DEF(SW_API_IP_PORT_SG_GET, fal_ip_port_sg_cfg_get), \
    SW_API_DEF(SW_API_IP_PUB_IP_SET, fal_ip_pub_addr_set), \
    SW_API_DEF(SW_API_IP_PUB_IP_GET, fal_ip_pub_addr_get), \
    SW_API_DEF(SW_API_IP_NETWORK_ROUTE_DEL, fal_ip_network_route_del), \
    SW_API_DEF(SW_API_IP_PORT_INTF_GET, fal_ip_port_intf_get), \
    SW_API_DEF(SW_API_IP_PORT_INTF_SET, fal_ip_port_intf_set), \
    SW_API_DEF(SW_API_IP_PORT_MAC_GET, fal_ip_port_macaddr_get), \
    SW_API_DEF(SW_API_IP_PORT_MAC_SET, fal_ip_port_macaddr_set), \
    SW_API_DEF(SW_API_IP_ROUTE_MISS_GET, fal_ip_route_mismatch_action_get), \
    SW_API_DEF(SW_API_IP_ROUTE_MISS_SET, fal_ip_route_mismatch_action_set), \
    SW_API_DEF(SW_API_IP_PORT_ARP_SG_SET, fal_ip_port_arp_sg_cfg_set), \
    SW_API_DEF(SW_API_IP_PORT_ARP_SG_GET, fal_ip_port_arp_sg_cfg_get), \
    SW_API_DEF(SW_API_IP_VSI_MC_MODE_SET, fal_ip_vsi_mc_mode_set), \
    SW_API_DEF(SW_API_IP_VSI_MC_MODE_GET, fal_ip_vsi_mc_mode_get), \
    SW_API_DEF(SW_API_GLOBAL_CTRL_GET, fal_ip_global_ctrl_get), \
    SW_API_DEF(SW_API_GLOBAL_CTRL_SET, fal_ip_global_ctrl_set),

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
    SW_API_DESC(SW_API_IP_INTF_ENTRY_ADD) \
    SW_API_DESC(SW_API_IP_INTF_ENTRY_DEL) \
    SW_API_DESC(SW_API_IP_INTF_ENTRY_NEXT) \
    SW_API_DESC(SW_API_IP_UNK_SOURCE_CMD_SET)  \
    SW_API_DESC(SW_API_IP_UNK_SOURCE_CMD_GET)  \
    SW_API_DESC(SW_API_ARP_UNK_SOURCE_CMD_SET) \
    SW_API_DESC(SW_API_ARP_UNK_SOURCE_CMD_GET) \
    SW_API_DESC(SW_API_IP_AGE_TIME_SET)  \
    SW_API_DESC(SW_API_IP_AGE_TIME_GET) \
    SW_API_DESC(SW_API_WCMP_HASH_MODE_SET) \
    SW_API_DESC(SW_API_WCMP_HASH_MODE_GET) \
    SW_API_DESC(SW_API_IP_VRF_BASE_ADDR_SET) \
    SW_API_DESC(SW_API_IP_VRF_BASE_ADDR_GET) \
    SW_API_DESC(SW_API_IP_VRF_BASE_MASK_SET) \
    SW_API_DESC(SW_API_IP_VRF_BASE_MASK_GET) \
    SW_API_DESC(SW_API_IP_DEFAULT_ROUTE_SET) \
    SW_API_DESC(SW_API_IP_DEFAULT_ROUTE_GET) \
    SW_API_DESC(SW_API_IP_HOST_ROUTE_SET) \
    SW_API_DESC(SW_API_IP_HOST_ROUTE_GET) \
    SW_API_DESC(SW_API_IP_WCMP_ENTRY_SET) \
    SW_API_DESC(SW_API_IP_WCMP_ENTRY_GET) \
    SW_API_DESC(SW_API_IP_RFS_IP4_SET)  \
    SW_API_DESC(SW_API_IP_RFS_IP6_SET)  \
    SW_API_DESC(SW_API_IP_RFS_IP4_DEL)  \
    SW_API_DESC(SW_API_IP_RFS_IP6_DEL)  \
    SW_API_DESC(SW_API_IP_DEFAULT_FLOW_CMD_SET) \
    SW_API_DESC(SW_API_IP_DEFAULT_FLOW_CMD_GET) \
    SW_API_DESC(SW_API_IP_DEFAULT_RT_FLOW_CMD_SET) \
    SW_API_DESC(SW_API_IP_DEFAULT_RT_FLOW_CMD_GET) \
    SW_API_DESC(SW_API_IP_VIS_ARP_SG_CFG_GET) \
    SW_API_DESC(SW_API_IP_VIS_ARP_SG_CFG_SET) \
    SW_API_DESC(SW_API_IP_NETWORK_ROUTE_GET) \
    SW_API_DESC(SW_API_IP_NETWORK_ROUTE_ADD) \
    SW_API_DESC(SW_API_IP_INTF_GET) \
    SW_API_DESC(SW_API_IP_INTF_SET) \
    SW_API_DESC(SW_API_IP_VSI_INTF_GET) \
    SW_API_DESC(SW_API_IP_VSI_INTF_SET) \
    SW_API_DESC(SW_API_IP_NEXTHOP_GET) \
    SW_API_DESC(SW_API_IP_NEXTHOP_SET) \
    SW_API_DESC(SW_API_IP_VSI_SG_SET) \
    SW_API_DESC(SW_API_IP_VSI_SG_GET) \
    SW_API_DESC(SW_API_IP_PORT_SG_SET) \
    SW_API_DESC(SW_API_IP_PORT_SG_GET) \
    SW_API_DESC(SW_API_IP_PUB_IP_SET) \
    SW_API_DESC(SW_API_IP_PUB_IP_GET) \
    SW_API_DESC(SW_API_IP_NETWORK_ROUTE_DEL) \
    SW_API_DESC(SW_API_IP_PORT_INTF_GET) \
    SW_API_DESC(SW_API_IP_PORT_INTF_SET) \
    SW_API_DESC(SW_API_IP_PORT_MAC_GET) \
    SW_API_DESC(SW_API_IP_PORT_MAC_SET) \
    SW_API_DESC(SW_API_IP_ROUTE_MISS_GET) \
    SW_API_DESC(SW_API_IP_ROUTE_MISS_SET) \
    SW_API_DESC(SW_API_IP_PORT_ARP_SG_SET) \
    SW_API_DESC(SW_API_IP_PORT_ARP_SG_GET) \
    SW_API_DESC(SW_API_IP_VSI_MC_MODE_SET) \
    SW_API_DESC(SW_API_IP_VSI_MC_MODE_GET) \
    SW_API_DESC(SW_API_GLOBAL_CTRL_GET) \
    SW_API_DESC(SW_API_GLOBAL_CTRL_SET)
#else
#define IP_API
#define IP_API_PARAM
#endif
#else
#define IP_API
#define IP_API_PARAM
#endif

#ifdef IN_FLOW
#ifndef IN_FLOW_MINI
#define FLOW_API \
    SW_API_DEF(SW_API_FLOW_STATUS_SET, fal_flow_status_set), \
    SW_API_DEF(SW_API_FLOW_STATUS_GET, fal_flow_status_get), \
    SW_API_DEF(SW_API_FLOW_AGE_TIMER_SET, fal_flow_age_timer_set), \
    SW_API_DEF(SW_API_FLOW_AGE_TIMER_GET, fal_flow_age_timer_get), \
    SW_API_DEF(SW_API_FLOW_CTRL_SET, fal_flow_mgmt_set), \
    SW_API_DEF(SW_API_FLOW_CTRL_GET, fal_flow_mgmt_get), \
    SW_API_DEF(SW_API_FLOW_ENTRY_ADD, fal_flow_entry_add), \
    SW_API_DEF(SW_API_FLOW_ENTRY_DEL, fal_flow_entry_del), \
    SW_API_DEF(SW_API_FLOW_ENTRY_GET, fal_flow_entry_get), \
    SW_API_DEF(SW_API_FLOW_GLOBAL_CFG_GET, fal_flow_global_cfg_get), \
    SW_API_DEF(SW_API_FLOW_GLOBAL_CFG_SET, fal_flow_global_cfg_set), \
    SW_API_DEF(SW_API_FLOW_HOST_ADD, fal_flow_host_add), \
    SW_API_DEF(SW_API_FLOW_HOST_GET, fal_flow_host_get), \
    SW_API_DEF(SW_API_FLOW_HOST_DEL, fal_flow_host_del), \
    SW_API_DEF(SW_API_FLOWENTRY_NEXT, fal_flow_entry_next),

#define FLOW_API_PARAM \
    SW_API_DESC(SW_API_FLOW_STATUS_SET) \
    SW_API_DESC(SW_API_FLOW_STATUS_GET) \
    SW_API_DESC(SW_API_FLOW_AGE_TIMER_SET) \
    SW_API_DESC(SW_API_FLOW_AGE_TIMER_GET) \
    SW_API_DESC(SW_API_FLOW_CTRL_SET) \
    SW_API_DESC(SW_API_FLOW_CTRL_GET) \
    SW_API_DESC(SW_API_FLOW_ENTRY_ADD) \
    SW_API_DESC(SW_API_FLOW_ENTRY_DEL) \
    SW_API_DESC(SW_API_FLOW_ENTRY_GET) \
    SW_API_DESC(SW_API_FLOW_GLOBAL_CFG_GET) \
    SW_API_DESC(SW_API_FLOW_GLOBAL_CFG_SET) \
    SW_API_DESC(SW_API_FLOW_HOST_ADD) \
    SW_API_DESC(SW_API_FLOW_HOST_GET) \
    SW_API_DESC(SW_API_FLOW_HOST_DEL) \
    SW_API_DESC(SW_API_FLOWENTRY_NEXT)
#else
#define FLOW_API \
    SW_API_DEF(SW_API_FLOW_CTRL_SET, fal_flow_mgmt_set), \
    SW_API_DEF(SW_API_FLOW_CTRL_GET, fal_flow_mgmt_get),

#define FLOW_API_PARAM \
    SW_API_DESC(SW_API_FLOW_CTRL_SET) \
    SW_API_DESC(SW_API_FLOW_CTRL_GET)
#endif
#else
#define FLOW_API
#define FLOW_API_PARAM
#endif


#ifdef IN_NAT
#define NAT_API \
    SW_API_DEF(SW_API_NAT_ADD, fal_nat_add), \
    SW_API_DEF(SW_API_NAT_DEL, fal_nat_del), \
    SW_API_DEF(SW_API_NAT_GET, fal_nat_get), \
    SW_API_DEF(SW_API_NAT_NEXT, fal_nat_next), \
    SW_API_DEF(SW_API_NAT_COUNTER_BIND, fal_nat_counter_bind), \
    SW_API_DEF(SW_API_NAPT_ADD, fal_napt_add), \
    SW_API_DEF(SW_API_NAPT_DEL, fal_napt_del), \
    SW_API_DEF(SW_API_NAPT_GET, fal_napt_get), \
    SW_API_DEF(SW_API_NAPT_NEXT, fal_napt_next), \
    SW_API_DEF(SW_API_NAPT_COUNTER_BIND, fal_napt_counter_bind), \
    SW_API_DEF(SW_API_FLOW_ADD, fal_flow_add), \
    SW_API_DEF(SW_API_FLOW_DEL, fal_flow_del), \
    SW_API_DEF(SW_API_FLOW_GET, fal_flow_get), \
    SW_API_DEF(SW_API_FLOW_NEXT, fal_flow_next), \
    SW_API_DEF(SW_API_FLOW_COUNTER_BIND, fal_flow_counter_bind), \
    SW_API_DEF(SW_API_NAT_STATUS_SET, fal_nat_status_set), \
    SW_API_DEF(SW_API_NAT_STATUS_GET, fal_nat_status_get), \
    SW_API_DEF(SW_API_NAT_HASH_MODE_SET, fal_nat_hash_mode_set), \
    SW_API_DEF(SW_API_NAT_HASH_MODE_GET, fal_nat_hash_mode_get), \
    SW_API_DEF(SW_API_NAPT_STATUS_SET, fal_napt_status_set), \
    SW_API_DEF(SW_API_NAPT_STATUS_GET, fal_napt_status_get), \
    SW_API_DEF(SW_API_NAPT_MODE_SET, fal_napt_mode_set), \
    SW_API_DEF(SW_API_NAPT_MODE_GET, fal_napt_mode_get), \
    SW_API_DEF(SW_API_PRV_BASE_ADDR_SET, fal_nat_prv_base_addr_set), \
    SW_API_DEF(SW_API_PRV_BASE_ADDR_GET, fal_nat_prv_base_addr_get), \
    SW_API_DEF(SW_API_PRV_ADDR_MODE_SET, fal_nat_prv_addr_mode_set), \
    SW_API_DEF(SW_API_PRV_ADDR_MODE_GET, fal_nat_prv_addr_mode_get), \
    SW_API_DEF(SW_API_PUB_ADDR_ENTRY_ADD, fal_nat_pub_addr_add), \
    SW_API_DEF(SW_API_PUB_ADDR_ENTRY_DEL, fal_nat_pub_addr_del), \
    SW_API_DEF(SW_API_PUB_ADDR_ENTRY_NEXT, fal_nat_pub_addr_next), \
    SW_API_DEF(SW_API_NAT_UNK_SESSION_CMD_SET, fal_nat_unk_session_cmd_set), \
    SW_API_DEF(SW_API_NAT_UNK_SESSION_CMD_GET, fal_nat_unk_session_cmd_get), \
    SW_API_DEF(SW_API_PRV_BASE_MASK_SET, fal_nat_prv_base_mask_set), \
    SW_API_DEF(SW_API_PRV_BASE_MASK_GET, fal_nat_prv_base_mask_get), \
    SW_API_DEF(SW_API_NAT_GLOBAL_SET, fal_nat_global_set),           \
    SW_API_DEF(SW_API_FLOW_COOKIE_SET, fal_flow_cookie_set),         \
    SW_API_DEF(SW_API_FLOW_RFS_SET, fal_flow_rfs_set),

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
    SW_API_DESC(SW_API_FLOW_ADD) \
    SW_API_DESC(SW_API_FLOW_DEL) \
    SW_API_DESC(SW_API_FLOW_GET) \
    SW_API_DESC(SW_API_FLOW_NEXT) \
    SW_API_DESC(SW_API_FLOW_COUNTER_BIND) \
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
    SW_API_DESC(SW_API_PRV_BASE_MASK_SET) \
    SW_API_DESC(SW_API_PRV_BASE_MASK_GET) \
    SW_API_DESC(SW_API_NAT_GLOBAL_SET)    \
    SW_API_DESC(SW_API_FLOW_COOKIE_SET)   \
    SW_API_DESC(SW_API_FLOW_RFS_SET)
#else
#define NAT_API
#define NAT_API_PARAM
#endif

#ifdef IN_TRUNK
#define TRUNK_API \
    SW_API_DEF(SW_API_TRUNK_GROUP_SET, fal_trunk_group_set), \
    SW_API_DEF(SW_API_TRUNK_GROUP_GET, fal_trunk_group_get), \
    SW_API_DEF(SW_API_TRUNK_HASH_SET, fal_trunk_hash_mode_set), \
    SW_API_DEF(SW_API_TRUNK_HASH_GET, fal_trunk_hash_mode_get), \
    SW_API_DEF(SW_API_TRUNK_MAN_SA_SET, fal_trunk_manipulate_sa_set), \
    SW_API_DEF(SW_API_TRUNK_MAN_SA_GET, fal_trunk_manipulate_sa_get), \
    SW_API_DEF(SW_API_TRUNK_FAILOVER_EN_SET, fal_trunk_failover_enable), \
    SW_API_DEF(SW_API_TRUNK_FAILOVER_EN_GET, fal_trunk_failover_status_get),

#define TRUNK_API_PARAM \
    SW_API_DESC(SW_API_TRUNK_GROUP_SET) \
    SW_API_DESC(SW_API_TRUNK_GROUP_GET) \
    SW_API_DESC(SW_API_TRUNK_HASH_SET)  \
    SW_API_DESC(SW_API_TRUNK_HASH_GET)  \
    SW_API_DESC(SW_API_TRUNK_MAN_SA_SET)\
    SW_API_DESC(SW_API_TRUNK_MAN_SA_GET) \
    SW_API_DESC(SW_API_TRUNK_FAILOVER_EN_SET)\
    SW_API_DESC(SW_API_TRUNK_FAILOVER_EN_GET)
#else
#define TRUNK_API
#define TRUNK_API_PARAM
#endif

#ifdef IN_INTERFACECONTROL
#define INTERFACECTRL_API \
    SW_API_DEF(SW_API_MAC_MODE_SET, fal_interface_mac_mode_set), \
    SW_API_DEF(SW_API_MAC_MODE_GET, fal_interface_mac_mode_get), \
    SW_API_DEF(SW_API_PORT_3AZ_STATUS_SET, fal_port_3az_status_set), \
    SW_API_DEF(SW_API_PORT_3AZ_STATUS_GET, fal_port_3az_status_get), \
    SW_API_DEF(SW_API_PHY_MODE_SET, fal_interface_phy_mode_set), \
    SW_API_DEF(SW_API_PHY_MODE_GET, fal_interface_phy_mode_get), \
    SW_API_DEF(SW_API_FX100_CTRL_SET, fal_interface_fx100_ctrl_set), \
    SW_API_DEF(SW_API_FX100_CTRL_GET, fal_interface_fx100_ctrl_get), \
    SW_API_DEF(SW_API_FX100_STATUS_GET, fal_interface_fx100_status_get),\
    SW_API_DEF(SW_API_MAC06_EXCH_SET, fal_interface_mac06_exch_set),\
    SW_API_DEF(SW_API_MAC06_EXCH_GET, fal_interface_mac06_exch_get),

#define INTERFACECTRL_API_PARAM \
    SW_API_DESC(SW_API_MAC_MODE_SET)  \
    SW_API_DESC(SW_API_MAC_MODE_GET)  \
    SW_API_DESC(SW_API_PORT_3AZ_STATUS_SET)  \
    SW_API_DESC(SW_API_PORT_3AZ_STATUS_GET)  \
    SW_API_DESC(SW_API_PHY_MODE_SET)  \
    SW_API_DESC(SW_API_PHY_MODE_GET)  \
    SW_API_DESC(SW_API_FX100_CTRL_SET)  \
    SW_API_DESC(SW_API_FX100_CTRL_GET) \
    SW_API_DESC(SW_API_FX100_STATUS_GET) \
    SW_API_DESC(SW_API_MAC06_EXCH_SET) \
    SW_API_DESC(SW_API_MAC06_EXCH_GET)

#else
#define INTERFACECTRL_API
#define INTERFACECTRL_API_PARAM
#endif

#ifdef IN_VSI
#ifndef IN_VSI_MINI
#define VSI_API \
    SW_API_DEF(SW_API_VSI_ALLOC, ppe_vsi_alloc), \
    SW_API_DEF(SW_API_VSI_FREE, ppe_vsi_free), \
    SW_API_DEF(SW_API_PORT_VSI_SET, fal_port_vsi_set), \
    SW_API_DEF(SW_API_PORT_VSI_GET, fal_port_vsi_get), \
    SW_API_DEF(SW_API_PORT_VLAN_VSI_SET, ppe_port_vlan_vsi_set), \
    SW_API_DEF(SW_API_PORT_VLAN_VSI_GET, ppe_port_vlan_vsi_get), \
    SW_API_DEF(SW_API_VSI_TBL_DUMP, ppe_vsi_tbl_dump), \
    SW_API_DEF(SW_API_VSI_NEWADDR_LRN_GET, fal_vsi_newaddr_lrn_get), \
    SW_API_DEF(SW_API_VSI_NEWADDR_LRN_SET, fal_vsi_newaddr_lrn_set), \
    SW_API_DEF(SW_API_VSI_STAMOVE_SET, fal_vsi_stamove_set), \
    SW_API_DEF(SW_API_VSI_STAMOVE_GET,fal_vsi_stamove_get),  \
    SW_API_DEF(SW_API_VSI_MEMBER_SET, fal_vsi_member_set), \
    SW_API_DEF(SW_API_VSI_MEMBER_GET, fal_vsi_member_get),  \
    SW_API_DEF(SW_API_VSI_COUNTER_GET,fal_vsi_counter_get),  \
    SW_API_DEF(SW_API_VSI_COUNTER_CLEANUP,fal_vsi_counter_cleanup),


#define VSI_API_PARAM \
    SW_API_DESC(SW_API_VSI_ALLOC)  \
    SW_API_DESC(SW_API_VSI_FREE)  \
    SW_API_DESC(SW_API_PORT_VSI_SET)  \
    SW_API_DESC(SW_API_PORT_VSI_GET)  \
    SW_API_DESC(SW_API_PORT_VLAN_VSI_SET)  \
    SW_API_DESC(SW_API_PORT_VLAN_VSI_GET)  \
    SW_API_DESC(SW_API_VSI_TBL_DUMP) \
    SW_API_DESC(SW_API_VSI_NEWADDR_LRN_GET) \
    SW_API_DESC(SW_API_VSI_NEWADDR_LRN_SET) \
    SW_API_DESC(SW_API_VSI_STAMOVE_SET) \
    SW_API_DESC(SW_API_VSI_STAMOVE_GET) \
    SW_API_DESC(SW_API_VSI_MEMBER_SET) \
    SW_API_DESC(SW_API_VSI_MEMBER_GET) \
    SW_API_DESC(SW_API_VSI_COUNTER_GET) \
    SW_API_DESC(SW_API_VSI_COUNTER_CLEANUP)
#else
#define VSI_API \
    SW_API_DEF(SW_API_VSI_ALLOC, ppe_vsi_alloc), \
    SW_API_DEF(SW_API_VSI_FREE, ppe_vsi_free), \
    SW_API_DEF(SW_API_PORT_VSI_SET, fal_port_vsi_set), \
    SW_API_DEF(SW_API_PORT_VLAN_VSI_SET, ppe_port_vlan_vsi_set), \
    SW_API_DEF(SW_API_PORT_VLAN_VSI_GET, ppe_port_vlan_vsi_get), \
    SW_API_DEF(SW_API_VSI_TBL_DUMP, ppe_vsi_tbl_dump), \
    SW_API_DEF(SW_API_VSI_NEWADDR_LRN_SET, fal_vsi_newaddr_lrn_set), \
    SW_API_DEF(SW_API_VSI_STAMOVE_SET, fal_vsi_stamove_set), \
    SW_API_DEF(SW_API_VSI_MEMBER_SET, fal_vsi_member_set), \
    SW_API_DEF(SW_API_VSI_MEMBER_GET, fal_vsi_member_get),


#define VSI_API_PARAM \
    SW_API_DESC(SW_API_VSI_ALLOC)  \
    SW_API_DESC(SW_API_VSI_FREE)  \
    SW_API_DESC(SW_API_PORT_VSI_SET)  \
    SW_API_DESC(SW_API_PORT_VLAN_VSI_SET)  \
    SW_API_DESC(SW_API_PORT_VLAN_VSI_GET)  \
    SW_API_DESC(SW_API_VSI_TBL_DUMP) \
    SW_API_DESC(SW_API_VSI_NEWADDR_LRN_SET) \
    SW_API_DESC(SW_API_VSI_STAMOVE_SET) \
    SW_API_DESC(SW_API_VSI_MEMBER_SET) \
    SW_API_DESC(SW_API_VSI_MEMBER_GET)
#endif
#else
#define VSI_API
#define VSI_API_PARAM
#endif

#ifdef IN_QM
#ifndef IN_QM_MINI
#define QM_API \
    SW_API_DEF(SW_API_UCAST_QUEUE_BASE_PROFILE_SET, fal_ucast_queue_base_profile_set), \
    SW_API_DEF(SW_API_UCAST_QUEUE_BASE_PROFILE_GET, fal_ucast_queue_base_profile_get), \
    SW_API_DEF(SW_API_UCAST_PRIORITY_CLASS_SET, fal_ucast_priority_class_set), \
    SW_API_DEF(SW_API_UCAST_PRIORITY_CLASS_GET, fal_ucast_priority_class_get), \
    SW_API_DEF(SW_API_MCAST_PRIORITY_CLASS_SET, fal_port_mcast_priority_class_set), \
    SW_API_DEF(SW_API_MCAST_PRIORITY_CLASS_GET, fal_port_mcast_priority_class_get), \
    SW_API_DEF(SW_API_QUEUE_FLUSH, fal_queue_flush), \
    SW_API_DEF(SW_API_UCAST_HASH_MAP_SET, fal_ucast_hash_map_set), \
    SW_API_DEF(SW_API_UCAST_HASH_MAP_GET, fal_ucast_hash_map_get), \
    SW_API_DEF(SW_API_UCAST_DFLT_HASH_MAP_SET, fal_ucast_default_hash_set), \
    SW_API_DEF(SW_API_UCAST_DFLT_HASH_MAP_GET, fal_ucast_default_hash_get), \
    SW_API_DEF(SW_API_MCAST_CPUCODE_CLASS_SET, fal_mcast_cpu_code_class_set), \
    SW_API_DEF(SW_API_MCAST_CPUCODE_CLASS_GET, fal_mcast_cpu_code_class_get), \
    SW_API_DEF(SW_API_AC_CTRL_SET, fal_ac_ctrl_set), \
    SW_API_DEF(SW_API_AC_CTRL_GET, fal_ac_ctrl_get), \
    SW_API_DEF(SW_API_AC_PRE_BUFFER_SET, fal_ac_prealloc_buffer_set), \
    SW_API_DEF(SW_API_AC_PRE_BUFFER_GET, fal_ac_prealloc_buffer_get), \
    SW_API_DEF(SW_API_QUEUE_GROUP_SET, fal_ac_queue_group_set), \
    SW_API_DEF(SW_API_QUEUE_GROUP_GET, fal_ac_queue_group_get), \
    SW_API_DEF(SW_API_STATIC_THRESH_SET, fal_ac_static_threshold_set), \
    SW_API_DEF(SW_API_STATIC_THRESH_GET, fal_ac_static_threshold_get), \
    SW_API_DEF(SW_API_DYNAMIC_THRESH_SET, fal_ac_dynamic_threshold_set), \
    SW_API_DEF(SW_API_DYNAMIC_THRESH_GET, fal_ac_dynamic_threshold_get), \
    SW_API_DEF(SW_API_GOURP_BUFFER_SET, fal_ac_group_buffer_set), \
    SW_API_DEF(SW_API_GOURP_BUFFER_GET, fal_ac_group_buffer_get), \
    SW_API_DEF(SW_API_QUEUE_CNT_CTRL_GET, fal_queue_counter_ctrl_get), \
    SW_API_DEF(SW_API_QUEUE_CNT_CTRL_SET, fal_queue_counter_ctrl_set), \
    SW_API_DEF(SW_API_QUEUE_CNT_GET, fal_queue_counter_get), \
    SW_API_DEF(SW_API_QUEUE_CNT_CLEANUP, fal_queue_counter_cleanup), \
    SW_API_DEF(SW_API_QM_ENQUEUE_CTRL_SET, fal_qm_enqueue_ctrl_set), \
    SW_API_DEF(SW_API_QM_ENQUEUE_CTRL_GET, fal_qm_enqueue_ctrl_get), \
    SW_API_DEF(SW_API_QM_SOURCE_PROFILE_SET, fal_qm_port_source_profile_set), \
    SW_API_DEF(SW_API_QM_SOURCE_PROFILE_GET, fal_qm_port_source_profile_get),

#define QM_API_PARAM \
    SW_API_DESC(SW_API_UCAST_QUEUE_BASE_PROFILE_SET) \
    SW_API_DESC(SW_API_UCAST_QUEUE_BASE_PROFILE_GET) \
    SW_API_DESC(SW_API_UCAST_PRIORITY_CLASS_SET) \
    SW_API_DESC(SW_API_UCAST_PRIORITY_CLASS_GET) \
    SW_API_DESC(SW_API_MCAST_PRIORITY_CLASS_SET) \
    SW_API_DESC(SW_API_MCAST_PRIORITY_CLASS_GET) \
    SW_API_DESC(SW_API_QUEUE_FLUSH) \
    SW_API_DESC(SW_API_UCAST_HASH_MAP_SET) \
    SW_API_DESC(SW_API_UCAST_HASH_MAP_GET) \
    SW_API_DESC(SW_API_UCAST_DFLT_HASH_MAP_SET) \
    SW_API_DESC(SW_API_UCAST_DFLT_HASH_MAP_GET) \
    SW_API_DESC(SW_API_MCAST_CPUCODE_CLASS_SET) \
    SW_API_DESC(SW_API_MCAST_CPUCODE_CLASS_GET) \
    SW_API_DESC(SW_API_AC_CTRL_SET) \
    SW_API_DESC(SW_API_AC_CTRL_GET) \
    SW_API_DESC(SW_API_AC_PRE_BUFFER_SET) \
    SW_API_DESC(SW_API_AC_PRE_BUFFER_GET) \
    SW_API_DESC(SW_API_QUEUE_GROUP_SET) \
    SW_API_DESC(SW_API_QUEUE_GROUP_GET) \
    SW_API_DESC(SW_API_STATIC_THRESH_SET) \
    SW_API_DESC(SW_API_STATIC_THRESH_GET) \
    SW_API_DESC(SW_API_DYNAMIC_THRESH_SET) \
    SW_API_DESC(SW_API_DYNAMIC_THRESH_GET) \
    SW_API_DESC(SW_API_GOURP_BUFFER_SET) \
    SW_API_DESC(SW_API_GOURP_BUFFER_GET) \
    SW_API_DESC(SW_API_QUEUE_CNT_CTRL_GET) \
    SW_API_DESC(SW_API_QUEUE_CNT_CTRL_SET) \
    SW_API_DESC(SW_API_QUEUE_CNT_GET) \
    SW_API_DESC(SW_API_QUEUE_CNT_CLEANUP) \
    SW_API_DESC(SW_API_QM_ENQUEUE_CTRL_SET) \
    SW_API_DESC(SW_API_QM_ENQUEUE_CTRL_GET) \
    SW_API_DESC(SW_API_QM_SOURCE_PROFILE_SET) \
    SW_API_DESC(SW_API_QM_SOURCE_PROFILE_GET)
#else
#define QM_API \
    SW_API_DEF(SW_API_UCAST_QUEUE_BASE_PROFILE_SET, fal_ucast_queue_base_profile_set), \
    SW_API_DEF(SW_API_QUEUE_FLUSH, fal_queue_flush), \
    SW_API_DEF(SW_API_AC_CTRL_SET, fal_ac_ctrl_set), \
    SW_API_DEF(SW_API_AC_PRE_BUFFER_SET, fal_ac_prealloc_buffer_set), \
    SW_API_DEF(SW_API_QUEUE_GROUP_SET, fal_ac_queue_group_set), \
    SW_API_DEF(SW_API_STATIC_THRESH_SET, fal_ac_static_threshold_set), \
    SW_API_DEF(SW_API_DYNAMIC_THRESH_SET, fal_ac_dynamic_threshold_set), \
    SW_API_DEF(SW_API_GOURP_BUFFER_SET, fal_ac_group_buffer_set), \
    SW_API_DEF(SW_API_QM_ENQUEUE_CTRL_SET, fal_qm_enqueue_ctrl_set),

#define QM_API_PARAM \
    SW_API_DESC(SW_API_UCAST_QUEUE_BASE_PROFILE_SET) \
    SW_API_DESC(SW_API_QUEUE_FLUSH) \
    SW_API_DESC(SW_API_AC_CTRL_SET) \
    SW_API_DESC(SW_API_AC_PRE_BUFFER_SET) \
    SW_API_DESC(SW_API_QUEUE_GROUP_SET) \
    SW_API_DESC(SW_API_STATIC_THRESH_SET) \
    SW_API_DESC(SW_API_DYNAMIC_THRESH_SET) \
    SW_API_DESC(SW_API_GOURP_BUFFER_SET) \
    SW_API_DESC(SW_API_QM_ENQUEUE_CTRL_SET)
#endif
#else
#define QM_API
#define QM_API_PARAM
#endif


#ifdef IN_PPPOE
#define PPPOE_API \
    SW_API_DEF(SW_API_PPPOE_CMD_SET, fal_pppoe_cmd_set), \
    SW_API_DEF(SW_API_PPPOE_CMD_GET, fal_pppoe_cmd_get), \
    SW_API_DEF(SW_API_PPPOE_STATUS_SET, fal_pppoe_status_set), \
    SW_API_DEF(SW_API_PPPOE_STATUS_GET, fal_pppoe_status_get), \
    SW_API_DEF(SW_API_PPPOE_SESSION_ADD, fal_pppoe_session_add), \
    SW_API_DEF(SW_API_PPPOE_SESSION_DEL, fal_pppoe_session_del), \
    SW_API_DEF(SW_API_PPPOE_SESSION_GET, fal_pppoe_session_get), \
    SW_API_DEF(SW_API_PPPOE_SESSION_TABLE_ADD, fal_pppoe_session_table_add), \
    SW_API_DEF(SW_API_PPPOE_SESSION_TABLE_DEL, fal_pppoe_session_table_del), \
    SW_API_DEF(SW_API_PPPOE_SESSION_TABLE_GET, fal_pppoe_session_table_get), \
    SW_API_DEF(SW_API_PPPOE_SESSION_ID_SET, fal_pppoe_session_id_set), \
    SW_API_DEF(SW_API_PPPOE_SESSION_ID_GET, fal_pppoe_session_id_get), \
    SW_API_DEF(SW_API_RTD_PPPOE_EN_SET, fal_rtd_pppoe_en_set), \
    SW_API_DEF(SW_API_RTD_PPPOE_EN_GET, fal_rtd_pppoe_en_get), \
    SW_API_DEF(SW_API_PPPOE_EN_SET, fal_pppoe_l3intf_enable), \
    SW_API_DEF(SW_API_PPPOE_EN_GET, fal_pppoe_l3intf_status_get),

#define PPPOE_API_PARAM \
    SW_API_DESC(SW_API_PPPOE_CMD_SET) \
    SW_API_DESC(SW_API_PPPOE_CMD_GET) \
    SW_API_DESC(SW_API_PPPOE_STATUS_SET) \
    SW_API_DESC(SW_API_PPPOE_STATUS_GET) \
    SW_API_DESC(SW_API_PPPOE_SESSION_ADD) \
    SW_API_DESC(SW_API_PPPOE_SESSION_DEL) \
    SW_API_DESC(SW_API_PPPOE_SESSION_GET) \
    SW_API_DESC(SW_API_PPPOE_SESSION_TABLE_ADD) \
    SW_API_DESC(SW_API_PPPOE_SESSION_TABLE_DEL) \
    SW_API_DESC(SW_API_PPPOE_SESSION_TABLE_GET) \
    SW_API_DESC(SW_API_PPPOE_SESSION_ID_SET) \
    SW_API_DESC(SW_API_PPPOE_SESSION_ID_GET) \
    SW_API_DESC(SW_API_RTD_PPPOE_EN_SET) \
    SW_API_DESC(SW_API_RTD_PPPOE_EN_GET) \
    SW_API_DESC(SW_API_PPPOE_EN_SET) \
    SW_API_DESC(SW_API_PPPOE_EN_GET)

#else
#define PPPOE_API
#define PPPOE_API_PARAM
#endif

#ifdef IN_BM
#ifndef IN_BM_MINI
#define BM_API \
    SW_API_DEF(SW_API_BM_CTRL_SET, fal_port_bm_ctrl_set), \
    SW_API_DEF(SW_API_BM_CTRL_GET, fal_port_bm_ctrl_get), \
    SW_API_DEF(SW_API_BM_PORTGROUP_MAP_SET, fal_port_bufgroup_map_set), \
    SW_API_DEF(SW_API_BM_PORTGROUP_MAP_GET, fal_port_bufgroup_map_get), \
    SW_API_DEF(SW_API_BM_GROUP_BUFFER_SET, fal_bm_bufgroup_buffer_set), \
    SW_API_DEF(SW_API_BM_GROUP_BUFFER_GET, fal_bm_bufgroup_buffer_get), \
    SW_API_DEF(SW_API_BM_PORT_RSVBUFFER_SET, fal_bm_port_reserved_buffer_set), \
    SW_API_DEF(SW_API_BM_PORT_RSVBUFFER_GET, fal_bm_port_reserved_buffer_get), \
    SW_API_DEF(SW_API_BM_STATIC_THRESH_SET, fal_bm_port_static_thresh_set), \
    SW_API_DEF(SW_API_BM_STATIC_THRESH_GET, fal_bm_port_static_thresh_get), \
    SW_API_DEF(SW_API_BM_DYNAMIC_THRESH_SET, fal_bm_port_dynamic_thresh_set), \
    SW_API_DEF(SW_API_BM_DYNAMIC_THRESH_GET, fal_bm_port_dynamic_thresh_get), \
    SW_API_DEF(SW_API_BM_PORT_COUNTER_GET, fal_bm_port_counter_get),

#define BM_API_PARAM \
    SW_API_DESC(SW_API_BM_CTRL_SET) \
    SW_API_DESC(SW_API_BM_CTRL_GET) \
    SW_API_DESC(SW_API_BM_PORTGROUP_MAP_SET) \
    SW_API_DESC(SW_API_BM_PORTGROUP_MAP_GET) \
    SW_API_DESC(SW_API_BM_GROUP_BUFFER_SET) \
    SW_API_DESC(SW_API_BM_GROUP_BUFFER_GET) \
    SW_API_DESC(SW_API_BM_PORT_RSVBUFFER_SET) \
    SW_API_DESC(SW_API_BM_PORT_RSVBUFFER_GET) \
    SW_API_DESC(SW_API_BM_STATIC_THRESH_SET) \
    SW_API_DESC(SW_API_BM_STATIC_THRESH_GET) \
    SW_API_DESC(SW_API_BM_DYNAMIC_THRESH_SET) \
    SW_API_DESC(SW_API_BM_DYNAMIC_THRESH_GET) \
    SW_API_DESC(SW_API_BM_PORT_COUNTER_GET)
#else
#define BM_API \
    SW_API_DEF(SW_API_BM_CTRL_SET, fal_port_bm_ctrl_set), \
    SW_API_DEF(SW_API_BM_PORTGROUP_MAP_SET, fal_port_bufgroup_map_set), \
    SW_API_DEF(SW_API_BM_GROUP_BUFFER_SET, fal_bm_bufgroup_buffer_set), \
    SW_API_DEF(SW_API_BM_PORT_RSVBUFFER_SET, fal_bm_port_reserved_buffer_set), \
    SW_API_DEF(SW_API_BM_DYNAMIC_THRESH_SET, fal_bm_port_dynamic_thresh_set),

#define BM_API_PARAM \
    SW_API_DESC(SW_API_BM_CTRL_SET) \
    SW_API_DESC(SW_API_BM_PORTGROUP_MAP_SET) \
    SW_API_DESC(SW_API_BM_GROUP_BUFFER_SET) \
    SW_API_DESC(SW_API_BM_PORT_RSVBUFFER_SET) \
    SW_API_DESC(SW_API_BM_DYNAMIC_THRESH_SET)
#endif
#else
#define BM_API
#define BM_API_PARAM
#endif

/*qca808x_start*/
#define REG_API \
    SW_API_DEF(SW_API_PHY_GET, fal_phy_get), \
	SW_API_DEF(SW_API_PHY_SET, fal_phy_set), \
/*qca808x_end*/\
    SW_API_DEF(SW_API_REG_GET, fal_reg_get), \
    SW_API_DEF(SW_API_REG_SET, fal_reg_set), \
    SW_API_DEF(SW_API_PSGMII_REG_GET, fal_psgmii_reg_get), \
    SW_API_DEF(SW_API_PSGMII_REG_SET, fal_psgmii_reg_set), \
    SW_API_DEF(SW_API_REG_FIELD_GET, fal_reg_field_get), \
    SW_API_DEF(SW_API_REG_FIELD_SET, fal_reg_field_set), \
    SW_API_DEF(SW_API_REG_DUMP, fal_reg_dump), \
    SW_API_DEF(SW_API_DBG_REG_DUMP, fal_debug_reg_dump), \
    SW_API_DEF(SW_API_DBG_PSGMII_SELF_TEST, fal_debug_psgmii_self_test), \
    SW_API_DEF(SW_API_PHY_DUMP, fal_phy_dump), \
    SW_API_DEF(SW_API_UNIPHY_REG_GET, fal_uniphy_reg_get), \
    SW_API_DEF(SW_API_UNIPHY_REG_SET, fal_uniphy_reg_set),
/*qca808x_start*/\
/*end of REG_API*/
#define REG_API_PARAM \
    SW_API_DESC(SW_API_PHY_GET) \
    SW_API_DESC(SW_API_PHY_SET) \
/*qca808x_end*/\
    SW_API_DESC(SW_API_REG_GET) \
    SW_API_DESC(SW_API_REG_SET) \
    SW_API_DESC(SW_API_PSGMII_REG_GET) \
    SW_API_DESC(SW_API_PSGMII_REG_SET) \
    SW_API_DESC(SW_API_REG_FIELD_GET) \
    SW_API_DESC(SW_API_REG_FIELD_SET) \
    SW_API_DESC(SW_API_REG_DUMP) \
    SW_API_DESC(SW_API_DBG_REG_DUMP) \
    SW_API_DESC(SW_API_DBG_PSGMII_SELF_TEST) \
    SW_API_DESC(SW_API_PHY_DUMP) \
    SW_API_DESC(SW_API_UNIPHY_REG_GET) \
    SW_API_DESC(SW_API_UNIPHY_REG_SET)
/*qca808x_start*/\
/*end of REG_API_PARAM*/
/*qca808x_end*/
#ifdef IN_CTRLPKT
#define CTRLPKT_API \
    SW_API_DEF(SW_API_MGMTCTRL_ETHTYPE_PROFILE_SET, fal_mgmtctrl_ethtype_profile_set), \
    SW_API_DEF(SW_API_MGMTCTRL_ETHTYPE_PROFILE_GET, fal_mgmtctrl_ethtype_profile_get), \
    SW_API_DEF(SW_API_MGMTCTRL_RFDB_PROFILE_SET, fal_mgmtctrl_rfdb_profile_set), \
    SW_API_DEF(SW_API_MGMTCTRL_RFDB_PROFILE_GET, fal_mgmtctrl_rfdb_profile_get), \
    SW_API_DEF(SW_API_MGMTCTRL_CTRLPKT_PROFILE_ADD, fal_mgmtctrl_ctrlpkt_profile_add), \
    SW_API_DEF(SW_API_MGMTCTRL_CTRLPKT_PROFILE_DEL, fal_mgmtctrl_ctrlpkt_profile_del), \
    SW_API_DEF(SW_API_MGMTCTRL_CTRLPKT_PROFILE_GETFIRST, fal_mgmtctrl_ctrlpkt_profile_getfirst), \
    SW_API_DEF(SW_API_MGMTCTRL_CTRLPKT_PROFILE_GETNEXT, fal_mgmtctrl_ctrlpkt_profile_getnext),

#define CTRLPKT_API_PARAM \
    SW_API_DESC(SW_API_MGMTCTRL_ETHTYPE_PROFILE_SET) \
    SW_API_DESC(SW_API_MGMTCTRL_ETHTYPE_PROFILE_GET) \
    SW_API_DESC(SW_API_MGMTCTRL_RFDB_PROFILE_SET) \
    SW_API_DESC(SW_API_MGMTCTRL_RFDB_PROFILE_GET) \
    SW_API_DESC(SW_API_MGMTCTRL_CTRLPKT_PROFILE_ADD) \
    SW_API_DESC(SW_API_MGMTCTRL_CTRLPKT_PROFILE_DEL) \
    SW_API_DESC(SW_API_MGMTCTRL_CTRLPKT_PROFILE_GETFIRST) \
    SW_API_DESC(SW_API_MGMTCTRL_CTRLPKT_PROFILE_GETNEXT)
#else
#define CTRLPKT_API
#define CTRLPKT_API_PARAM
#endif

#ifdef IN_SERVCODE
#define SERVCODE_API \
    SW_API_DEF(SW_API_SERVCODE_CONFIG_SET, fal_servcode_config_set), \
    SW_API_DEF(SW_API_SERVCODE_CONFIG_GET, fal_servcode_config_get), \
    SW_API_DEF(SW_API_SERVCODE_LOOPCHECK_EN, fal_servcode_loopcheck_en), \
    SW_API_DEF(SW_API_SERVCODE_LOOPCHECK_STATUS_GET, fal_servcode_loopcheck_status_get),

#define SERVCODE_API_PARAM \
    SW_API_DESC(SW_API_SERVCODE_CONFIG_SET) \
    SW_API_DESC(SW_API_SERVCODE_CONFIG_GET) \
    SW_API_DESC(SW_API_SERVCODE_LOOPCHECK_EN) \
    SW_API_DESC(SW_API_SERVCODE_LOOPCHECK_STATUS_GET)
#else
#define SERVCODE_API
#define SERVCODE_API_PARAM
#endif

#ifdef IN_RSS_HASH
#define RSS_HASH_API \
    SW_API_DEF(SW_API_RSS_HASH_CONFIG_SET, fal_rss_hash_config_set), \
    SW_API_DEF(SW_API_RSS_HASH_CONFIG_GET, fal_rss_hash_config_get),

#define RSS_HASH_API_PARAM \
    SW_API_DESC(SW_API_RSS_HASH_CONFIG_SET) \
    SW_API_DESC(SW_API_RSS_HASH_CONFIG_GET)
#else
#define RSS_HASH_API
#define RSS_HASH_API_PARAM
#endif

#ifdef IN_SHAPER
#ifndef IN_SHAPER_MINI
#define SHAPER_API \
    SW_API_DEF(SW_API_PORT_SHAPER_TIMESLOT_SET, fal_port_shaper_timeslot_set), \
    SW_API_DEF(SW_API_PORT_SHAPER_TIMESLOT_GET, fal_port_shaper_timeslot_get), \
    SW_API_DEF(SW_API_FLOW_SHAPER_TIMESLOT_SET, fal_flow_shaper_timeslot_set), \
    SW_API_DEF(SW_API_FLOW_SHAPER_TIMESLOT_GET, fal_flow_shaper_timeslot_get), \
    SW_API_DEF(SW_API_QUEUE_SHAPER_TIMESLOT_SET, fal_queue_shaper_timeslot_set), \
    SW_API_DEF(SW_API_QUEUE_SHAPER_TIMESLOT_GET, fal_queue_shaper_timeslot_get), \
    SW_API_DEF(SW_API_PORT_SHAPER_TOKEN_NUMBER_SET, fal_port_shaper_token_number_set), \
    SW_API_DEF(SW_API_PORT_SHAPER_TOKEN_NUMBER_GET, fal_port_shaper_token_number_get), \
    SW_API_DEF(SW_API_FLOW_SHAPER_TOKEN_NUMBER_SET, fal_flow_shaper_token_number_set), \
    SW_API_DEF(SW_API_FLOW_SHAPER_TOKEN_NUMBER_GET, fal_flow_shaper_token_number_get), \
    SW_API_DEF(SW_API_QUEUE_SHAPER_TOKEN_NUMBER_SET, fal_queue_shaper_token_number_set), \
    SW_API_DEF(SW_API_QUEUE_SHAPER_TOKEN_NUMBER_GET, fal_queue_shaper_token_number_get), \
    SW_API_DEF(SW_API_PORT_SHAPER_SET, fal_port_shaper_set), \
    SW_API_DEF(SW_API_PORT_SHAPER_GET,fal_port_shaper_get), \
    SW_API_DEF(SW_API_FLOW_SHAPER_SET, fal_flow_shaper_set), \
    SW_API_DEF(SW_API_FLOW_SHAPER_GET,fal_flow_shaper_get), \
    SW_API_DEF(SW_API_QUEUE_SHAPER_SET, fal_queue_shaper_set), \
    SW_API_DEF(SW_API_QUEUE_SHAPER_GET,fal_queue_shaper_get), \
    SW_API_DEF(SW_API_SHAPER_IPG_PRE_SET, fal_shaper_ipg_preamble_length_set), \
    SW_API_DEF(SW_API_SHAPER_IPG_PRE_GET,fal_shaper_ipg_preamble_length_get),


#define SHAPER_API_PARAM \
    SW_API_DESC(SW_API_PORT_SHAPER_TIMESLOT_SET)  \
    SW_API_DESC(SW_API_PORT_SHAPER_TIMESLOT_GET)  \
    SW_API_DESC(SW_API_FLOW_SHAPER_TIMESLOT_SET)  \
    SW_API_DESC(SW_API_FLOW_SHAPER_TIMESLOT_GET)  \
    SW_API_DESC(SW_API_QUEUE_SHAPER_TIMESLOT_SET)  \
    SW_API_DESC(SW_API_QUEUE_SHAPER_TIMESLOT_GET)  \
    SW_API_DESC(SW_API_PORT_SHAPER_TOKEN_NUMBER_SET) \
    SW_API_DESC(SW_API_PORT_SHAPER_TOKEN_NUMBER_GET) \
    SW_API_DESC(SW_API_FLOW_SHAPER_TOKEN_NUMBER_SET) \
    SW_API_DESC(SW_API_FLOW_SHAPER_TOKEN_NUMBER_GET) \
    SW_API_DESC(SW_API_QUEUE_SHAPER_TOKEN_NUMBER_SET) \
    SW_API_DESC(SW_API_QUEUE_SHAPER_TOKEN_NUMBER_GET) \
    SW_API_DESC(SW_API_PORT_SHAPER_SET) \
    SW_API_DESC(SW_API_PORT_SHAPER_GET) \
    SW_API_DESC(SW_API_FLOW_SHAPER_SET) \
    SW_API_DESC(SW_API_FLOW_SHAPER_GET) \
    SW_API_DESC(SW_API_QUEUE_SHAPER_SET) \
    SW_API_DESC(SW_API_QUEUE_SHAPER_GET) \
    SW_API_DESC(SW_API_SHAPER_IPG_PRE_SET) \
    SW_API_DESC(SW_API_SHAPER_IPG_PRE_GET)
#else
#define SHAPER_API \
    SW_API_DEF(SW_API_PORT_SHAPER_TIMESLOT_SET, fal_port_shaper_timeslot_set), \
    SW_API_DEF(SW_API_FLOW_SHAPER_TIMESLOT_SET, fal_flow_shaper_timeslot_set), \
    SW_API_DEF(SW_API_QUEUE_SHAPER_TIMESLOT_SET, fal_queue_shaper_timeslot_set), \
    SW_API_DEF(SW_API_PORT_SHAPER_TOKEN_NUMBER_SET, fal_port_shaper_token_number_set), \
    SW_API_DEF(SW_API_FLOW_SHAPER_TOKEN_NUMBER_SET, fal_flow_shaper_token_number_set), \
    SW_API_DEF(SW_API_QUEUE_SHAPER_TOKEN_NUMBER_SET, fal_queue_shaper_token_number_set), \
    SW_API_DEF(SW_API_PORT_SHAPER_SET, fal_port_shaper_set), \
    SW_API_DEF(SW_API_FLOW_SHAPER_SET, fal_flow_shaper_set), \
    SW_API_DEF(SW_API_QUEUE_SHAPER_SET, fal_queue_shaper_set), \
    SW_API_DEF(SW_API_QUEUE_SHAPER_GET,fal_queue_shaper_get), \
    SW_API_DEF(SW_API_SHAPER_IPG_PRE_SET, fal_shaper_ipg_preamble_length_set),


#define SHAPER_API_PARAM \
    SW_API_DESC(SW_API_PORT_SHAPER_TIMESLOT_SET)  \
    SW_API_DESC(SW_API_FLOW_SHAPER_TIMESLOT_SET)  \
    SW_API_DESC(SW_API_QUEUE_SHAPER_TIMESLOT_SET)  \
    SW_API_DESC(SW_API_PORT_SHAPER_TOKEN_NUMBER_SET) \
    SW_API_DESC(SW_API_FLOW_SHAPER_TOKEN_NUMBER_SET) \
    SW_API_DESC(SW_API_QUEUE_SHAPER_TOKEN_NUMBER_SET) \
    SW_API_DESC(SW_API_PORT_SHAPER_SET) \
    SW_API_DESC(SW_API_FLOW_SHAPER_SET) \
    SW_API_DESC(SW_API_QUEUE_SHAPER_SET) \
    SW_API_DESC(SW_API_QUEUE_SHAPER_GET) \
    SW_API_DESC(SW_API_SHAPER_IPG_PRE_SET)
#endif
#else
#define SHAPER_API
#define SHAPER_API_PARAM
#endif

#ifdef IN_POLICER
#ifndef IN_POLICER_MINI
#define POLICER_API \
    SW_API_DEF(SW_API_POLICER_TIMESLOT_SET, fal_policer_timeslot_set), \
    SW_API_DEF(SW_API_POLICER_TIMESLOT_GET, fal_policer_timeslot_get), \
    SW_API_DEF(SW_API_POLICER_PORT_COUNTER_GET, fal_port_policer_counter_get), \
    SW_API_DEF(SW_API_POLICER_ACL_COUNTER_GET, fal_acl_policer_counter_get), \
    SW_API_DEF(SW_API_POLICER_COMPENSATION_SET, fal_port_policer_compensation_byte_set), \
    SW_API_DEF(SW_API_POLICER_COMPENSATION_GET, fal_port_policer_compensation_byte_get), \
    SW_API_DEF(SW_API_POLICER_PORT_ENTRY_SET, fal_port_policer_entry_set), \
    SW_API_DEF(SW_API_POLICER_PORT_ENTRY_GET, fal_port_policer_entry_get), \
    SW_API_DEF(SW_API_POLICER_ACL_ENTRY_SET, fal_acl_policer_entry_set), \
    SW_API_DEF(SW_API_POLICER_ACL_ENTRY_GET,fal_acl_policer_entry_get), \
    SW_API_DEF(SW_API_POLICER_GLOBAL_COUNTER_GET, fal_policer_global_counter_get), \
    SW_API_DEF(SW_API_POLICER_BYPASS_EN_SET, fal_policer_bypass_en_set), \
    SW_API_DEF(SW_API_POLICER_BYPASS_EN_GET, fal_policer_bypass_en_get),

#define POLICER_API_PARAM \
    SW_API_DESC(SW_API_POLICER_TIMESLOT_SET)  \
    SW_API_DESC(SW_API_POLICER_TIMESLOT_GET)  \
    SW_API_DESC(SW_API_POLICER_PORT_COUNTER_GET) \
    SW_API_DESC(SW_API_POLICER_ACL_COUNTER_GET) \
    SW_API_DESC(SW_API_POLICER_COMPENSATION_SET) \
    SW_API_DESC(SW_API_POLICER_COMPENSATION_GET) \
    SW_API_DESC(SW_API_POLICER_PORT_ENTRY_SET) \
    SW_API_DESC(SW_API_POLICER_PORT_ENTRY_GET) \
    SW_API_DESC(SW_API_POLICER_ACL_ENTRY_SET) \
    SW_API_DESC(SW_API_POLICER_ACL_ENTRY_GET) \
    SW_API_DESC(SW_API_POLICER_GLOBAL_COUNTER_GET) \
    SW_API_DESC(SW_API_POLICER_BYPASS_EN_SET) \
    SW_API_DESC(SW_API_POLICER_BYPASS_EN_GET)
#else
#define POLICER_API \
    SW_API_DEF(SW_API_POLICER_TIMESLOT_SET, fal_policer_timeslot_set), \
    SW_API_DEF(SW_API_POLICER_COMPENSATION_SET, fal_port_policer_compensation_byte_set),


#define POLICER_API_PARAM \
    SW_API_DESC(SW_API_POLICER_TIMESLOT_SET)  \
    SW_API_DESC(SW_API_POLICER_COMPENSATION_SET)
#endif
#else
#define POLICER_API
#define POLICER_API_PARAM
#endif

#ifdef IN_PTP
#define PTP_API \
    SW_API_DEF(SW_API_PTP_CONFIG_SET, fal_ptp_config_set), \
    SW_API_DEF(SW_API_PTP_CONFIG_GET, fal_ptp_config_get), \
    SW_API_DEF(SW_API_PTP_REFERENCE_CLOCK_SET, fal_ptp_reference_clock_set), \
    SW_API_DEF(SW_API_PTP_REFERENCE_CLOCK_GET, fal_ptp_reference_clock_get), \
    SW_API_DEF(SW_API_PTP_RX_TIMESTAMP_MODE_SET, fal_ptp_rx_timestamp_mode_set), \
    SW_API_DEF(SW_API_PTP_RX_TIMESTAMP_MODE_GET, fal_ptp_rx_timestamp_mode_get), \
    SW_API_DEF(SW_API_PTP_TIMESTAMP_GET, fal_ptp_timestamp_get), \
    SW_API_DEF(SW_API_PTP_PKT_TIMESTAMP_SET, fal_ptp_pkt_timestamp_set), \
    SW_API_DEF(SW_API_PTP_PKT_TIMESTAMP_GET, fal_ptp_pkt_timestamp_get), \
    SW_API_DEF(SW_API_PTP_GRANDMASTER_MODE_SET, fal_ptp_grandmaster_mode_set), \
    SW_API_DEF(SW_API_PTP_GRANDMASTER_MODE_GET, fal_ptp_grandmaster_mode_get), \
    SW_API_DEF(SW_API_PTP_RTC_TIME_SET, fal_ptp_rtc_time_set), \
    SW_API_DEF(SW_API_PTP_RTC_TIME_GET, fal_ptp_rtc_time_get), \
    SW_API_DEF(SW_API_PTP_RTC_TIME_CLEAR, fal_ptp_rtc_time_clear), \
    SW_API_DEF(SW_API_PTP_RTC_ADJTIME_SET, fal_ptp_rtc_adjtime_set), \
    SW_API_DEF(SW_API_PTP_RTC_ADJFREQ_SET, fal_ptp_rtc_adjfreq_set), \
    SW_API_DEF(SW_API_PTP_RTC_ADJFREQ_GET, fal_ptp_rtc_adjfreq_get), \
    SW_API_DEF(SW_API_PTP_LINK_DELAY_SET, fal_ptp_link_delay_set), \
    SW_API_DEF(SW_API_PTP_LINK_DELAY_GET, fal_ptp_link_delay_get), \
    SW_API_DEF(SW_API_PTP_SECURITY_SET, fal_ptp_security_set), \
    SW_API_DEF(SW_API_PTP_SECURITY_GET, fal_ptp_security_get), \
    SW_API_DEF(SW_API_PTP_PPS_SIGNAL_CONTROL_SET, fal_ptp_pps_signal_control_set), \
    SW_API_DEF(SW_API_PTP_PPS_SIGNAL_CONTROL_GET, fal_ptp_pps_signal_control_get), \
    SW_API_DEF(SW_API_PTP_RX_CRC_RECALC_SET, fal_ptp_rx_crc_recalc_enable), \
    SW_API_DEF(SW_API_PTP_RX_CRC_RECALC_GET, fal_ptp_rx_crc_recalc_status_get), \
    SW_API_DEF(SW_API_PTP_ASYM_CORRECTION_SET, fal_ptp_asym_correction_set), \
    SW_API_DEF(SW_API_PTP_ASYM_CORRECTION_GET, fal_ptp_asym_correction_get), \
    SW_API_DEF(SW_API_PTP_OUTPUT_WAVEFORM_SET, fal_ptp_output_waveform_set), \
    SW_API_DEF(SW_API_PTP_OUTPUT_WAVEFORM_GET, fal_ptp_output_waveform_get), \
    SW_API_DEF(SW_API_PTP_RTC_TIME_SNAPSHOT_SET, fal_ptp_rtc_time_snapshot_enable), \
    SW_API_DEF(SW_API_PTP_RTC_TIME_SNAPSHOT_GET, fal_ptp_rtc_time_snapshot_status_get), \
    SW_API_DEF(SW_API_PTP_INCREMENT_SYNC_FROM_CLOCK_SET, \
		    fal_ptp_increment_sync_from_clock_enable), \
    SW_API_DEF(SW_API_PTP_INCREMENT_SYNC_FROM_CLOCK_GET, \
		    fal_ptp_increment_sync_from_clock_status_get), \
    SW_API_DEF(SW_API_PTP_TOD_UART_SET, fal_ptp_tod_uart_set), \
    SW_API_DEF(SW_API_PTP_TOD_UART_GET, fal_ptp_tod_uart_get), \
    SW_API_DEF(SW_API_PTP_ENHANCED_TIMESTAMP_ENGINE_SET, fal_ptp_enhanced_timestamp_engine_set), \
    SW_API_DEF(SW_API_PTP_ENHANCED_TIMESTAMP_ENGINE_GET, fal_ptp_enhanced_timestamp_engine_get), \
    SW_API_DEF(SW_API_PTP_TRIGGER_SET, fal_ptp_trigger_set), \
    SW_API_DEF(SW_API_PTP_TRIGGER_GET, fal_ptp_trigger_get), \
    SW_API_DEF(SW_API_PTP_CAPTURE_SET, fal_ptp_capture_set), \
    SW_API_DEF(SW_API_PTP_CAPTURE_GET, fal_ptp_capture_get), \
    SW_API_DEF(SW_API_PTP_INTERRUPT_SET, fal_ptp_interrupt_set), \
    SW_API_DEF(SW_API_PTP_INTERRUPT_GET, fal_ptp_interrupt_get),

#define PTP_API_PARAM \
    SW_API_DESC(SW_API_PTP_CONFIG_SET) \
    SW_API_DESC(SW_API_PTP_CONFIG_GET) \
    SW_API_DESC(SW_API_PTP_REFERENCE_CLOCK_SET) \
    SW_API_DESC(SW_API_PTP_REFERENCE_CLOCK_GET) \
    SW_API_DESC(SW_API_PTP_RX_TIMESTAMP_MODE_SET) \
    SW_API_DESC(SW_API_PTP_RX_TIMESTAMP_MODE_GET) \
    SW_API_DESC(SW_API_PTP_TIMESTAMP_GET) \
    SW_API_DESC(SW_API_PTP_PKT_TIMESTAMP_SET) \
    SW_API_DESC(SW_API_PTP_PKT_TIMESTAMP_GET) \
    SW_API_DESC(SW_API_PTP_GRANDMASTER_MODE_SET) \
    SW_API_DESC(SW_API_PTP_GRANDMASTER_MODE_GET) \
    SW_API_DESC(SW_API_PTP_RTC_TIME_SET) \
    SW_API_DESC(SW_API_PTP_RTC_TIME_GET) \
    SW_API_DESC(SW_API_PTP_RTC_TIME_CLEAR) \
    SW_API_DESC(SW_API_PTP_RTC_ADJTIME_SET) \
    SW_API_DESC(SW_API_PTP_RTC_ADJFREQ_SET) \
    SW_API_DESC(SW_API_PTP_RTC_ADJFREQ_GET) \
    SW_API_DESC(SW_API_PTP_LINK_DELAY_SET) \
    SW_API_DESC(SW_API_PTP_LINK_DELAY_GET) \
    SW_API_DESC(SW_API_PTP_SECURITY_SET) \
    SW_API_DESC(SW_API_PTP_SECURITY_GET) \
    SW_API_DESC(SW_API_PTP_PPS_SIGNAL_CONTROL_SET) \
    SW_API_DESC(SW_API_PTP_PPS_SIGNAL_CONTROL_GET) \
    SW_API_DESC(SW_API_PTP_RX_CRC_RECALC_SET) \
    SW_API_DESC(SW_API_PTP_RX_CRC_RECALC_GET) \
    SW_API_DESC(SW_API_PTP_ASYM_CORRECTION_SET) \
    SW_API_DESC(SW_API_PTP_ASYM_CORRECTION_GET) \
    SW_API_DESC(SW_API_PTP_OUTPUT_WAVEFORM_SET) \
    SW_API_DESC(SW_API_PTP_OUTPUT_WAVEFORM_GET) \
    SW_API_DESC(SW_API_PTP_RTC_TIME_SNAPSHOT_SET) \
    SW_API_DESC(SW_API_PTP_RTC_TIME_SNAPSHOT_GET) \
    SW_API_DESC(SW_API_PTP_INCREMENT_SYNC_FROM_CLOCK_SET) \
    SW_API_DESC(SW_API_PTP_INCREMENT_SYNC_FROM_CLOCK_GET) \
    SW_API_DESC(SW_API_PTP_TOD_UART_SET) \
    SW_API_DESC(SW_API_PTP_TOD_UART_GET) \
    SW_API_DESC(SW_API_PTP_ENHANCED_TIMESTAMP_ENGINE_SET) \
    SW_API_DESC(SW_API_PTP_ENHANCED_TIMESTAMP_ENGINE_GET) \
    SW_API_DESC(SW_API_PTP_TRIGGER_SET) \
    SW_API_DESC(SW_API_PTP_TRIGGER_GET) \
    SW_API_DESC(SW_API_PTP_CAPTURE_SET) \
    SW_API_DESC(SW_API_PTP_CAPTURE_GET) \
    SW_API_DESC(SW_API_PTP_INTERRUPT_SET) \
    SW_API_DESC(SW_API_PTP_INTERRUPT_GET)
#else
#define PTP_API
#define PTP_API_PARAM
#endif

#ifdef IN_SFP
#define SFP_API \
    SW_API_DEF(SW_API_SFP_DATA_GET, fal_sfp_eeprom_data_get), \
    SW_API_DEF(SW_API_SFP_DATA_SET, fal_sfp_eeprom_data_set), \
    SW_API_DEF(SW_API_SFP_DEV_TYPE_GET, fal_sfp_device_type_get), \
    SW_API_DEF(SW_API_SFP_TRANSC_CODE_GET, fal_sfp_transceiver_code_get), \
    SW_API_DEF(SW_API_SFP_RATE_ENCODE_GET, fal_sfp_rate_encode_get), \
    SW_API_DEF(SW_API_SFP_LINK_LENGTH_GET, fal_sfp_link_length_get), \
    SW_API_DEF(SW_API_SFP_VENDOR_INFO_GET, fal_sfp_vendor_info_get), \
    SW_API_DEF(SW_API_SFP_LASER_WAVELENGTH_GET, fal_sfp_laser_wavelength_get), \
    SW_API_DEF(SW_API_SFP_OPTION_GET, fal_sfp_option_get), \
    SW_API_DEF(SW_API_SFP_CTRL_RATE_GET, fal_sfp_ctrl_rate_get), \
    SW_API_DEF(SW_API_SFP_ENHANCED_CFG_GET, fal_sfp_enhanced_cfg_get), \
    SW_API_DEF(SW_API_SFP_DIAG_THRESHOLD_GET, fal_sfp_diag_internal_threshold_get), \
    SW_API_DEF(SW_API_SFP_DIAG_CAL_CONST_GET, fal_sfp_diag_extenal_calibration_const_get), \
    SW_API_DEF(SW_API_SFP_DIAG_REALTIME_GET, fal_sfp_diag_realtime_get), \
    SW_API_DEF(SW_API_SFP_DIAG_CTRL_STATUS_GET, fal_sfp_diag_ctrl_status_get), \
    SW_API_DEF(SW_API_SFP_DIAG_ALARM_WARN_FLAG_GET, fal_sfp_diag_alarm_warning_flag_get), \
    SW_API_DEF(SW_API_SFP_CHECKCODE_GET, fal_sfp_checkcode_get),

#define SFP_API_PARAM \
    SW_API_DESC(SW_API_SFP_DATA_GET) \
    SW_API_DESC(SW_API_SFP_DATA_SET) \
    SW_API_DESC(SW_API_SFP_DEV_TYPE_GET) \
    SW_API_DESC(SW_API_SFP_TRANSC_CODE_GET) \
    SW_API_DESC(SW_API_SFP_RATE_ENCODE_GET) \
    SW_API_DESC(SW_API_SFP_LINK_LENGTH_GET) \
    SW_API_DESC(SW_API_SFP_VENDOR_INFO_GET) \
    SW_API_DESC(SW_API_SFP_LASER_WAVELENGTH_GET) \
    SW_API_DESC(SW_API_SFP_OPTION_GET) \
    SW_API_DESC(SW_API_SFP_CTRL_RATE_GET) \
    SW_API_DESC(SW_API_SFP_ENHANCED_CFG_GET) \
    SW_API_DESC(SW_API_SFP_DIAG_THRESHOLD_GET) \
    SW_API_DESC(SW_API_SFP_DIAG_CAL_CONST_GET) \
    SW_API_DESC(SW_API_SFP_DIAG_REALTIME_GET) \
    SW_API_DESC(SW_API_SFP_DIAG_CTRL_STATUS_GET) \
    SW_API_DESC(SW_API_SFP_DIAG_ALARM_WARN_FLAG_GET) \
    SW_API_DESC(SW_API_SFP_CHECKCODE_GET)
#else
#define SFP_API
#define SFP_API_PARAM
#endif

/*qca808x_start*/
#define SSDK_API \
/*qca808x_end*/\
    SW_API_DEF(SW_API_SWITCH_RESET, fal_reset), \
    SW_API_DEF(SW_API_SSDK_CFG, fal_ssdk_cfg), \
    SW_API_DEF(SW_API_MODULE_FUNC_CTRL_SET, fal_module_func_ctrl_set), \
    SW_API_DEF(SW_API_MODULE_FUNC_CTRL_GET, fal_module_func_ctrl_get), \
/*qca808x_start*/\
    PORTCONTROL_API \
/*qca808x_end*/\
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
    COSMAP_API \
    SEC_API  \
    IP_API \
    NAT_API \
    FLOW_API \
    TRUNK_API \
    INTERFACECTRL_API \
    VSI_API \
    QM_API \
    BM_API \
    PPPOE_API \
/*qca808x_start*/\
    REG_API \
/*qca808x_end*/\
    CTRLPKT_API \
    SERVCODE_API \
    RSS_HASH_API \
    POLICER_API \
    SHAPER_API \
    PTP_API \
    SFP_API \
/*qca808x_start*/\
    SW_API_DEF(SW_API_MAX, NULL),


#define SSDK_PARAM  \
/*qca808x_end*/\
    SW_PARAM_DEF(SW_API_SWITCH_RESET, SW_UINT32, 4, SW_PARAM_IN, "Dev ID"), \
    SW_PARAM_DEF(SW_API_SSDK_CFG, SW_UINT32, 4, SW_PARAM_IN, "Dev ID"), \
    SW_PARAM_DEF(SW_API_SSDK_CFG, SW_SSDK_CFG, sizeof(ssdk_cfg_t), \
		    SW_PARAM_PTR|SW_PARAM_OUT, "ssdk configuration"), \
    SW_PARAM_DEF(SW_API_MODULE_FUNC_CTRL_SET, SW_UINT32, 4, SW_PARAM_IN, "Dev ID"), \
    SW_PARAM_DEF(SW_API_MODULE_FUNC_CTRL_SET, SW_MODULE, 4, SW_PARAM_IN, "Module"), \
    SW_PARAM_DEF(SW_API_MODULE_FUNC_CTRL_SET, SW_FUNC_CTRL, sizeof(fal_func_ctrl_t), \
		    SW_PARAM_PTR|SW_PARAM_IN, "Function bitmap"), \
    SW_PARAM_DEF(SW_API_MODULE_FUNC_CTRL_GET, SW_UINT32, 4, SW_PARAM_IN, "Dev ID"), \
    SW_PARAM_DEF(SW_API_MODULE_FUNC_CTRL_GET, SW_MODULE, 4, SW_PARAM_IN, "Module"), \
    SW_PARAM_DEF(SW_API_MODULE_FUNC_CTRL_GET, SW_FUNC_CTRL, sizeof(fal_func_ctrl_t), \
		    SW_PARAM_PTR|SW_PARAM_OUT, "Function bitmap"), \
    MIB_API_PARAM \
    LEAKY_API_PARAM \
    MISC_API_PARAM \
    IGMP_API_PARAM \
    MIRROR_API_PARAM \
/*qca808x_start*/\
    PORTCONTROL_API_PARAM \
/*qca808x_end*/\
    PORTVLAN_API_PARAM \
    VLAN_API_PARAM \
    FDB_API_PARAM \
    QOS_API_PARAM \
    RATE_API_PARAM \
    STP_API_PARAM \
    ACL_API_PARAM \
    LED_API_PARAM \
    COSMAP_API_PARAM \
    SEC_API_PARAM \
    IP_API_PARAM \
    NAT_API_PARAM \
    FLOW_API_PARAM \
    TRUNK_API_PARAM \
    INTERFACECTRL_API_PARAM \
    VSI_API_PARAM \
    QM_API_PARAM \
    BM_API_PARAM \
    PPPOE_API_PARAM \
/*qca808x_start*/\
    REG_API_PARAM \
/*qca808x_end*/\
    CTRLPKT_API_PARAM \
    SERVCODE_API_PARAM \
    RSS_HASH_API_PARAM \
    POLICER_API_PARAM \
    SHAPER_API_PARAM \
    PTP_API_PARAM \
    SFP_API_PARAM \
/*qca808x_start*/\
    SW_PARAM_DEF(SW_API_MAX, SW_UINT32, 4, SW_PARAM_IN, "Dev ID"),


#ifdef __cplusplus
}
#endif                          /* __cplusplus */

#endif                          /* _FAL_API_H_ */
/*qca808x_end*/

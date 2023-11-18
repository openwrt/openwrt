/** @file */
/******************************************************************************
 *
 * Copyright(c) 2019 Realtek Corporation. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 ******************************************************************************/

#ifndef _MAC_AX_HDR_CONV_H_
#define _MAC_AX_HDR_CONV_H_

#include "../type.h"

#define HW_HDR_CONV_HTC_ORDER_ENABLE 0x1
#define HW_HDR_CONV_VLANTAG_VALID_ENABLE 0x1
#define HW_HDR_CONV_QOS_FIELD_ENABLE 0x1

enum {
	R_AX_ETH_TYPE_IDX_0 = 0,
	R_AX_ETH_TYPE_IDX_1 = 1,
	R_AX_ETH_TYPE_IDX_2 = 2,
};

enum {
	R_AX_OUI_IDX_0 = 0,
	R_AX_OUI_IDX_1 = 1,
	R_AX_OUI_IDX_2 = 2,
	R_AX_OUI_IDX_3 = 3,
};

enum {
	R_AX_OUI_CFG_DISABLE = 0,
	R_AX_OUI_CFG_ENABLE = 1,
};

/**
 * @struct mac_ax_en_hdr_conv
 * @brief mac_ax_en_hdr_conv
 *
 * @var mac_ax_en_hdr_conv::enable
 * Please Place Description here.
 * @var mac_ax_en_hdr_conv::rsvd0
 * Please Place Description here.
 */
struct mac_ax_en_hdr_conv {
	u32 enable: 1;
	u32 rsvd0: 31;
};

/**
 * @brief mac_hdr_conv_tx_set_eth_type
 *
 * @param adapter
 * @param eth_type_idx
 * @param eth_type
 * @return Please Place Description here.
 * @retval u32
 */
u32 mac_hdr_conv_tx_set_eth_type(struct mac_ax_adapter *adapter,
				 u8 eth_type_idx, u16 eth_type);
/**
 * @}
 * @}
 */

/**
 * @brief mac_hdr_conv_tx_get_eth_type
 *
 * @param adapter
 * @param eth_type_idx
 * @param *eth_type
 * @return Please Place Description here.
 * @retval u32
 */
u32 mac_hdr_conv_tx_get_eth_type(struct mac_ax_adapter *adapter,
				 u8 eth_type_idx, u16 *eth_type);
/**
 * @}
 * @}
 */

/**
 * @brief mac_hdr_conv_tx_set_oui
 *
 * @param adapter
 * @param oui_idx
 * @param oui_val
 * @param enable
 * @return Please Place Description here.
 * @retval u32
 */
u32 mac_hdr_conv_tx_set_oui(struct mac_ax_adapter *adapter,
			    u8 oui_idx, u32 oui_val, u8 enable);
/**
 * @}
 * @}
 */

/**
 * @brief mac_hdr_conv_tx_get_oui
 *
 * @param adapter
 * @param oui_idx
 * @param *oui_val
 * @param *enable
 * @return Please Place Description here.
 * @retval u32
 */
u32 mac_hdr_conv_tx_get_oui(struct mac_ax_adapter *adapter,
			    u8 oui_idx, u32 *oui_val, u8 *enable);
/**
 * @}
 * @}
 */

/**
 * @brief mac_hdr_conv_tx_en
 *
 * @param adapter
 * @param en_tx_hdr_conv
 */
u32 mac_hdr_conv_en(struct mac_ax_adapter *adapter, u8 en_hdr_conv);
/**
 * @}
 * @}
 */

/**
 * @brief mac_hdr_conv_tx_macid_en
 *
 * @param adapter
 * @param macid
 * @param dctl_hdr_conv_cfg
 */
u32 mac_hdr_conv_tx_macid_en(struct mac_ax_adapter *adapter,
			     u8 macid, struct mac_ax_hdr_conv_dctl_cfg hdr_conv_dctl_cfg);
/**
 * @}
 * @}
 */

/**
 * @brief mac_hdr_conv_tx_vlan_tag_valid_en
 *
 * @param adapter
 * @param macid
 * @param vlan_tag_valid_en
 * @return Please Place Description here.
 * @retval u32
 */
u32 mac_hdr_conv_tx_vlan_tag_valid_en(struct mac_ax_adapter *adapter,
				      u8 macid, u8 vlan_tag_valid_en);
/**
 * @}
 * @}
 */

/**
 * @brief mac_hdr_conv_tx_get_vlan_tag_valid
 *
 * @param adapter
 * @param macid
 * @return Please Place Description here.
 * @retval u8
 */
u8 mac_hdr_conv_tx_get_vlan_tag_valid(struct mac_ax_adapter *adapter, u8 macid);
/**
 * @}
 * @}
 */

/**
 * @brief mac_hdr_conv_tx_qos_field_en
 *
 * @param adapter
 * @param macid
 * @param wlan_with_qosc_en
 * @param qos_field_h
 * @return Please Place Description here.
 * @retval u32
 */
u32 mac_hdr_conv_tx_qos_field_en(struct mac_ax_adapter *adapter,
				 u8 macid, u8 wlan_with_qosc_en, u8 qos_field_h);
/**
 * @}
 * @}
 */

/**
 * @brief mac_hdr_conv_tx_get_qos_field_en
 *
 * @param adapter
 * @param macid
 * @return Please Place Description here.
 * @retval u8
 */
u8 mac_hdr_conv_tx_get_qos_field_en(struct mac_ax_adapter *adapter, u8 macid);
/**
 * @}
 * @}
 */

/**
 * @brief mac_hdr_conv_tx_get_qos_field_h
 *
 * @param adapter
 * @param macid
 * @return Please Place Description here.
 * @retval u8
 */
u8 mac_hdr_conv_tx_get_qos_field_h(struct mac_ax_adapter *adapter, u8 macid);
/**
 * @}
 * @}
 */

/**
 * @brief mac_hdr_conv_tx_htc_en
 *
 * @param adapter
 * @param macid
 * @param wlan_with_htc_en
 * @param htc_lb
 * @return Please Place Description here.
 * @retval u32
 */
u32 mac_hdr_conv_tx_htc_en(struct mac_ax_adapter *adapter,
			   u8 macid, u8 wlan_with_htc_en, u8 htc_lb);
/**
 * @}
 * @}
 */

/**
 * @brief mac_hdr_conv_tx_get_htc_en
 *
 * @param adapter
 * @param macid
 * @return Please Place Description here.
 * @retval u8
 */
u8 mac_hdr_conv_tx_get_htc_en(struct mac_ax_adapter *adapter, u8 macid);
/**
 * @}
 * @}
 */

/**
 * @brief mac_hdr_conv_tx_get_htc_lb
 *
 * @param adapter
 * @param macid
 * @return Please Place Description here.
 * @retval u8
 */
u8 mac_hdr_conv_tx_get_htc_lb(struct mac_ax_adapter *adapter, u8 macid);
/**
 * @}
 * @}
 */

/**
 * @brief mac_hdr_conv_tx_target_wlan_hdr_len
 *
 * @param adapter
 * @param macid
 * @param wlan_hdr_len
 * @return Please Place Description here.
 * @retval u32
 */
u32 mac_hdr_conv_tx_target_wlan_hdr_len(struct mac_ax_adapter *adapter,
					u8 macid, u8 wlan_hdr_len);
/**
 * @}
 * @}
 */

/**
 * @brief mac_hdr_conv_tx_get_target_wlan_hdr_len
 *
 * @param adapter
 * @param macid
 * @return Please Place Description here.
 * @retval u8
 */
u8 mac_hdr_conv_tx_get_target_wlan_hdr_len(struct mac_ax_adapter *adapter, u8 macid);
/**
 * @}
 * @}
 */
#endif

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
#include "hdr_conv.h"

#if MAC_AX_FW_REG_OFLD
u32 mac_hdr_conv_en(struct mac_ax_adapter *adapter, u8 en_hdr_conv)
{
			u32 ret = 0;
			u8 *buf;
#if MAC_AX_PHL_H2C
			struct rtw_h2c_pkt *h2cb;
#else
			struct h2c_buf *h2cb;
#endif
			struct mac_ax_en_hdr_conv *content;

			h2cb = h2cb_alloc(adapter, H2CB_CLASS_CMD);
			if (!h2cb)
				return MACNPTR;

			buf = h2cb_put(h2cb, sizeof(struct mac_ax_en_hdr_conv));
			if (!buf) {
				ret = MACNOBUF;
				goto fail;
			}

			content = (struct mac_ax_en_hdr_conv *)buf;
			content->enable = en_hdr_conv;

			ret = h2c_pkt_set_hdr(adapter, h2cb,
					      FWCMD_TYPE_H2C,
					      FWCMD_H2C_CAT_MAC,
					      FWCMD_H2C_CL_FW_OFLD,
					      FWCMD_H2C_FUNC_EN_MAC_HDR_CONV,
					      0,
					      1);

			if (ret)
				goto fail;

			ret = h2c_pkt_build_txd(adapter, h2cb);
			if (ret)
				goto fail;

#if MAC_AX_PHL_H2C
			ret = PLTFM_TX(h2cb);
#else
			ret = PLTFM_TX(h2cb->data, h2cb->len);
#endif
fail:
			h2cb_free(adapter, h2cb);

			return ret;
}
#else
u32 mac_hdr_conv_en(struct mac_ax_adapter *adapter, u8 en_hdr_conv)
{
	u32 val;
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);

	if (en_hdr_conv) {
#if (MAC_AX_8852A_SUPPORT || MAC_AX_8852B_SUPPORT || MAC_AX_8851B_SUPPORT)
		if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852A) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8852B) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8851B)) {
			val = MAC_REG_R32(R_AX_HDR_SHCUT_SETTING);
			val |= (B_AX_MAC_MPDU_PROC_EN |
				B_AX_SHCUT_LLC_WR_LOCK |
				B_AX_SHCUT_PARSE_DASA);
			MAC_REG_W32(R_AX_HDR_SHCUT_SETTING, val);
			return MACSUCCESS;
		}
#endif
#if (MAC_AX_8852C_SUPPORT || MAC_AX_8192XB_SUPPORT  || MAC_AX_8851E_SUPPORT || \
MAC_AX_8852D_SUPPORT || MAC_AX_1115E_SUPPORT)
		if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852C) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8192XB) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8851E) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8852D) ||
		    is_chip_id(adapter, MAC_BE_CHIP_ID_1115E)) {
			val = MAC_REG_R32(R_AX_HDR_SHCUT_SETTING);
			val |= (B_AX_TX_MAC_MPDU_PROC_EN |
				B_AX_TX_HW_ACK_POLICY_EN);
			MAC_REG_W32(R_AX_HDR_SHCUT_SETTING, val);
			return MACSUCCESS;
		}
#endif
	} else {
#if (MAC_AX_8852A_SUPPORT || MAC_AX_8852B_SUPPORT || MAC_AX_8851B_SUPPORT)
		if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852A) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8852B) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8851B)) {
			val = MAC_REG_R32(R_AX_HDR_SHCUT_SETTING);
			val &= (~(B_AX_MAC_MPDU_PROC_EN |
				  B_AX_SHCUT_LLC_WR_LOCK |
				  B_AX_SHCUT_PARSE_DASA));
			MAC_REG_W32(R_AX_HDR_SHCUT_SETTING, val);
			return MACSUCCESS;
		}
#endif
#if (MAC_AX_8852C_SUPPORT || MAC_AX_8192XB_SUPPORT  || MAC_AX_8851E_SUPPORT || \
MAC_AX_8852D_SUPPORT || MAC_AX_1115E_SUPPORT)
		if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852C) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8192XB) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8851E) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8852D) ||
		    is_chip_id(adapter, MAC_BE_CHIP_ID_1115E)) {
			val = MAC_REG_R32(R_AX_HDR_SHCUT_SETTING);
			val &= (~(B_AX_TX_MAC_MPDU_PROC_EN |
				  B_AX_TX_HW_ACK_POLICY_EN));
			MAC_REG_W32(R_AX_HDR_SHCUT_SETTING, val);
			return MACSUCCESS;
		}
#endif
	}
	return MACNOTSUP;
}
#endif

u32 mac_hdr_conv_tx_macid_en(struct mac_ax_adapter *adapter,
			     u8 macid, struct mac_ax_hdr_conv_dctl_cfg hdr_conv_dctl_cfg)
{
	struct mac_ax_dctl_info info = {0};
	struct mac_ax_dctl_info mask = {0};
	struct mac_ax_ops *ops = adapter_to_mac_ops(adapter);
	u32 ret = 0;

	if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852C) ||
	    is_chip_id(adapter, MAC_AX_CHIP_ID_8192XB) ||
	    is_chip_id(adapter, MAC_AX_CHIP_ID_8851E) ||
	    is_chip_id(adapter, MAC_AX_CHIP_ID_8852D)) {
		info.vlan_tag_valid = hdr_conv_dctl_cfg.vlan_tag_valid;
		info.qos_field_h_en = hdr_conv_dctl_cfg.qos_field_h_en;
		info.qos_field_h = hdr_conv_dctl_cfg.qos_field_h;
		info.mhdr_len = hdr_conv_dctl_cfg.mhdr_len;

		mask.vlan_tag_valid = HW_HDR_CONV_VLANTAG_VALID_ENABLE;
		mask.qos_field_h_en = HW_HDR_CONV_QOS_FIELD_ENABLE;
		mask.qos_field_h = FWCMD_H2C_DCTRL_QOS_FIELD_H_MSK;
		mask.mhdr_len = FWCMD_H2C_DCTRL_V1_MHDR_LEN_MSK;
		ret = ops->upd_dctl_info(adapter, &info, &mask, macid, 1);
		return ret;
	} else {
		return MACNOTSUP;
	}
}

u32 mac_hdr_conv_tx_set_eth_type(struct mac_ax_adapter *adapter,
				 u8 eth_type_idx, u16 eth_type)
{
	u32 ret = MACNOTSUP;
	u32 val = 0;
	u32 reg = 0;
	u32 msk = 0;
	u32 sh = 0;
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);

	switch (eth_type_idx) {
	case R_AX_ETH_TYPE_IDX_0:
#if (MAC_AX_8852A_SUPPORT || MAC_AX_8852B_SUPPORT || MAC_AX_8852C_SUPPORT || \
MAC_AX_8192XB_SUPPORT || MAC_AX_8851B_SUPPORT  || MAC_AX_8851E_SUPPORT || \
MAC_AX_8852D_SUPPORT)
		if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852A) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8852B) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8852C) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8192XB) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8851B) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8851E) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8852D)) {
			reg = R_AX_SHCUT_LLC_ETH_TYPE0;
			msk = B_AX_SHUT_ETH_TYPE0_MSK;
			sh = B_AX_SHUT_ETH_TYPE0_SH;
			ret = MACSUCCESS;
		}
#endif
	break;
	case R_AX_ETH_TYPE_IDX_1:
#if (MAC_AX_8852A_SUPPORT || MAC_AX_8852B_SUPPORT || MAC_AX_8852C_SUPPORT || \
MAC_AX_8192XB_SUPPORT || MAC_AX_8851B_SUPPORT  || MAC_AX_8851E_SUPPORT || \
MAC_AX_8852D_SUPPORT)
		if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852A) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8852B) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8852C) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8192XB) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8851B) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8851E) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8852D)) {
			reg = R_AX_SHCUT_LLC_ETH_TYPE0;
			msk = B_AX_SHUT_ETH_TYPE1_MSK;
			sh = B_AX_SHUT_ETH_TYPE1_SH;
			ret = MACSUCCESS;
		}
#endif
	break;
	case R_AX_ETH_TYPE_IDX_2:
#if (MAC_AX_8852A_SUPPORT || MAC_AX_8852B_SUPPORT || MAC_AX_8852C_SUPPORT || \
MAC_AX_8192XB_SUPPORT || MAC_AX_8851B_SUPPORT  || MAC_AX_8851E_SUPPORT || \
MAC_AX_8852D_SUPPORT)
		if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852A) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8852B) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8852C) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8192XB) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8851B) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8851E) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8852D)) {
			reg = R_AX_SHCUT_LLC_ETH_TYPE1;
			msk = B_AX_SHUT_ETH_TYPE2_MSK;
			sh = B_AX_SHUT_ETH_TYPE2_SH;
			ret = MACSUCCESS;
		}
#endif
	break;
	default:
		ret = MACNOITEM;
	}

	if (ret == MACSUCCESS) {
		val = MAC_REG_R32(reg);
		val = val & ~(msk << sh);
		val = val | ((eth_type & msk) << sh);
		MAC_REG_W32(reg, val);
	}

	return ret;
}

u32 mac_hdr_conv_tx_get_eth_type(struct mac_ax_adapter *adapter,
				 u8 eth_type_idx, u16 *eth_type)
{
	u32 ret = MACNOTSUP;
	u32 val = 0;
	u32 reg = 0;
	u32 msk = 0;
	u32 sh = 0;
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);

	switch (eth_type_idx) {
	case R_AX_ETH_TYPE_IDX_0:
#if (MAC_AX_8852A_SUPPORT || MAC_AX_8852B_SUPPORT || MAC_AX_8852C_SUPPORT || \
MAC_AX_8192XB_SUPPORT || MAC_AX_8851B_SUPPORT  || MAC_AX_8851E_SUPPORT || \
MAC_AX_8852D_SUPPORT)
		if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852A) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8852B) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8852C) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8192XB) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8851B) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8851E) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8852D)) {
			reg = R_AX_SHCUT_LLC_ETH_TYPE0;
			msk = B_AX_SHUT_ETH_TYPE0_MSK;
			sh = B_AX_SHUT_ETH_TYPE0_SH;
			ret = MACSUCCESS;
		}
#endif
	break;
	case R_AX_ETH_TYPE_IDX_1:
#if (MAC_AX_8852A_SUPPORT || MAC_AX_8852B_SUPPORT || MAC_AX_8852C_SUPPORT || \
MAC_AX_8192XB_SUPPORT || MAC_AX_8851B_SUPPORT  || MAC_AX_8851E_SUPPORT || \
MAC_AX_8852D_SUPPORT)
		if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852A) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8852B) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8852C) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8192XB) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8851B) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8851E) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8852D)) {
			reg = R_AX_SHCUT_LLC_ETH_TYPE0;
			msk = B_AX_SHUT_ETH_TYPE1_MSK;
			sh = B_AX_SHUT_ETH_TYPE1_SH;
			ret = MACSUCCESS;
		}
#endif
	break;
	case R_AX_ETH_TYPE_IDX_2:
#if (MAC_AX_8852A_SUPPORT || MAC_AX_8852B_SUPPORT || MAC_AX_8852C_SUPPORT || \
MAC_AX_8192XB_SUPPORT || MAC_AX_8851B_SUPPORT  || MAC_AX_8851E_SUPPORT || \
MAC_AX_8852D_SUPPORT)
		if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852A) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8852B) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8852C) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8192XB) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8851B) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8851E) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8852D)) {
			reg = R_AX_SHCUT_LLC_ETH_TYPE1;
			msk = B_AX_SHUT_ETH_TYPE2_MSK;
			sh = B_AX_SHUT_ETH_TYPE2_SH;
			ret = MACSUCCESS;
		}
#endif
	break;
	default:
		ret = MACNOITEM;
	}

	if (ret == MACSUCCESS) {
		val = MAC_REG_R32(reg);
		*eth_type = (u16)((val & (msk << sh)) >> sh);
	}

	return ret;
}

u32 mac_hdr_conv_tx_set_oui(struct mac_ax_adapter *adapter,
			    u8 oui_idx, u32 oui_val, u8 enable)
{
	u32 ret = MACNOTSUP;
	u32 val = 0;
	u32 reg = 0;
	u32 msk = 0;
	u32 sh = 0;
	u32 en_bit = 0;
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);

	switch (oui_idx) {
	case R_AX_OUI_IDX_0:
#if (MAC_AX_8852A_SUPPORT || MAC_AX_8852B_SUPPORT || MAC_AX_8852C_SUPPORT || \
MAC_AX_8192XB_SUPPORT || MAC_AX_8851B_SUPPORT  || MAC_AX_8851E_SUPPORT || \
MAC_AX_8852D_SUPPORT)
		if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852A) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8852B) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8852C) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8192XB) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8851B) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8851E) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8852D)) {
			reg = R_AX_SHCUT_LLC_OUI0;
			msk = B_AX_SHUT_ETH_OUI0_MSK;
			sh = B_AX_SHUT_ETH_OUI0_SH;
			en_bit = B_AX_SHUT_ENABLE_OUI0;
			ret = MACSUCCESS;
		}
#endif
	break;
	case R_AX_OUI_IDX_1:
#if (MAC_AX_8852A_SUPPORT || MAC_AX_8852B_SUPPORT || MAC_AX_8852C_SUPPORT || \
MAC_AX_8192XB_SUPPORT || MAC_AX_8851B_SUPPORT  || MAC_AX_8851E_SUPPORT || \
MAC_AX_8852D_SUPPORT)
		if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852A) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8852B) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8852C) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8192XB) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8851B) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8851E) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8852D)) {
			reg = R_AX_SHCUT_LLC_OUI1;
			msk = B_AX_SHUT_ETH_OUI1_MSK;
			sh = B_AX_SHUT_ETH_OUI1_SH;
			en_bit = B_AX_SHUT_ENABLE_OUI1;
			ret = MACSUCCESS;
		}
#endif
	break;
	case R_AX_OUI_IDX_2:
#if (MAC_AX_8852A_SUPPORT || MAC_AX_8852B_SUPPORT || MAC_AX_8852C_SUPPORT || \
MAC_AX_8192XB_SUPPORT || MAC_AX_8851B_SUPPORT  || MAC_AX_8851E_SUPPORT || \
MAC_AX_8852D_SUPPORT)
		if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852A) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8852B) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8852C) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8192XB) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8851B) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8851E) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8852D)) {
			reg = R_AX_SHCUT_LLC_OUI2;
			msk = B_AX_SHUT_ETH_OUI2_MSK;
			sh = B_AX_SHUT_ETH_OUI2_SH;
			en_bit = B_AX_SHUT_ENABLE_OUI2;
			ret = MACSUCCESS;
		}
#endif
	break;
	case R_AX_OUI_IDX_3:
#if (MAC_AX_8852A_SUPPORT || MAC_AX_8852B_SUPPORT || MAC_AX_8852C_SUPPORT || \
MAC_AX_8192XB_SUPPORT || MAC_AX_8851B_SUPPORT  || MAC_AX_8851E_SUPPORT || \
MAC_AX_8852D_SUPPORT)
		if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852A) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8852B) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8852C) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8192XB) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8851B) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8851E) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8852D)) {
			reg = R_AX_SHCUT_LLC_OUI3;
			msk = B_AX_SHUT_ETH_OUI3_MSK;
			sh = B_AX_SHUT_ETH_OUI3_SH;
			en_bit = B_AX_SHUT_ENABLE_OUI3;
			ret = MACSUCCESS;
		}
#endif
	break;
	default:
		ret = MACNOITEM;
	}

	if (ret == MACSUCCESS) {
		val = MAC_REG_R32(reg);
		val = val & ~(msk << sh);
		val = val & ~en_bit;
		val = val | ((oui_val & msk) << sh);
		if (enable)
			val = val | en_bit;
		MAC_REG_W32(reg, val);
	}
	return ret;
}

u32 mac_hdr_conv_tx_get_oui(struct mac_ax_adapter *adapter,
			    u8 oui_idx, u32 *oui_val, u8 *enable)
{
	u32 ret = MACNOTSUP;
	u32 val = 0;
	u32 reg = 0;
	u32 msk = 0;
	u32 sh = 0;
	u32 en_bit = 0;
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);

	switch (oui_idx) {
	case R_AX_OUI_IDX_0:
#if (MAC_AX_8852A_SUPPORT || MAC_AX_8852B_SUPPORT || MAC_AX_8852C_SUPPORT || \
MAC_AX_8192XB_SUPPORT || MAC_AX_8851B_SUPPORT  || MAC_AX_8851E_SUPPORT || \
MAC_AX_8852D_SUPPORT)
		if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852A) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8852B) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8852C) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8192XB) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8851B) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8851E) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8852D)) {
			reg = R_AX_SHCUT_LLC_OUI0;
			msk = B_AX_SHUT_ETH_OUI0_MSK;
			sh = B_AX_SHUT_ETH_OUI0_SH;
			en_bit = B_AX_SHUT_ENABLE_OUI0;
			ret = MACSUCCESS;
		}
#endif
	break;
	case R_AX_OUI_IDX_1:
#if (MAC_AX_8852A_SUPPORT || MAC_AX_8852B_SUPPORT || MAC_AX_8852C_SUPPORT || \
MAC_AX_8192XB_SUPPORT || MAC_AX_8851B_SUPPORT  || MAC_AX_8851E_SUPPORT || \
MAC_AX_8852D_SUPPORT)
		if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852A) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8852B) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8852C) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8192XB) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8851B) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8851E) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8852D)) {
			reg = R_AX_SHCUT_LLC_OUI1;
			msk = B_AX_SHUT_ETH_OUI1_MSK;
			sh = B_AX_SHUT_ETH_OUI1_SH;
			en_bit = B_AX_SHUT_ENABLE_OUI1;
			ret = MACSUCCESS;
		}
#endif
	break;
	case R_AX_OUI_IDX_2:
#if (MAC_AX_8852A_SUPPORT || MAC_AX_8852B_SUPPORT || MAC_AX_8852C_SUPPORT || \
MAC_AX_8192XB_SUPPORT || MAC_AX_8851B_SUPPORT  || MAC_AX_8851E_SUPPORT || \
MAC_AX_8852D_SUPPORT)
		if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852A) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8852B) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8852C) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8192XB) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8851B) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8851E) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8852D)) {
			reg = R_AX_SHCUT_LLC_OUI2;
			msk = B_AX_SHUT_ETH_OUI2_MSK;
			sh = B_AX_SHUT_ETH_OUI2_SH;
			en_bit = B_AX_SHUT_ENABLE_OUI2;
			ret = MACSUCCESS;
		}
#endif
	break;
	case R_AX_OUI_IDX_3:
#if (MAC_AX_8852A_SUPPORT || MAC_AX_8852B_SUPPORT || MAC_AX_8852C_SUPPORT || \
MAC_AX_8192XB_SUPPORT || MAC_AX_8851B_SUPPORT  || MAC_AX_8851E_SUPPORT || \
MAC_AX_8852D_SUPPORT)
		if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852A) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8852B) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8852C) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8192XB) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8851B) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8851E) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8852D)) {
			reg = R_AX_SHCUT_LLC_OUI3;
			msk = B_AX_SHUT_ETH_OUI3_MSK;
			sh = B_AX_SHUT_ETH_OUI3_SH;
			en_bit = B_AX_SHUT_ENABLE_OUI3;
			ret = MACSUCCESS;
		}
#endif
	break;
	default:
		ret = MACNOITEM;
	}

	if (ret == MACSUCCESS) {
		val = MAC_REG_R32(reg);
		*oui_val = ((val & (msk << sh)) >> sh);
		*enable = ((val & en_bit) ? R_AX_OUI_CFG_ENABLE : R_AX_OUI_CFG_DISABLE);
	}
	return ret;
}

u32 mac_hdr_conv_tx_vlan_tag_valid_en(struct mac_ax_adapter *adapter,
				      u8 macid, u8 vlan_tag_valid_en)
{
	struct mac_ax_dctl_info info = {0};
	struct mac_ax_dctl_info mask = {0};
	struct mac_ax_ops *ops = adapter_to_mac_ops(adapter);
	u32 ret = 0;

	if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852C) ||
	    is_chip_id(adapter, MAC_AX_CHIP_ID_8192XB) ||
	    is_chip_id(adapter, MAC_AX_CHIP_ID_8851E) ||
	    is_chip_id(adapter, MAC_AX_CHIP_ID_8852D)) {
		info.vlan_tag_valid = vlan_tag_valid_en;
		mask.vlan_tag_valid = HW_HDR_CONV_VLANTAG_VALID_ENABLE;
		ret = ops->upd_dctl_info(adapter, &info, &mask, macid, 1);
		return ret;
	} else {
		return MACNOTSUP;
	}
}

u8 mac_hdr_conv_tx_get_vlan_tag_valid(struct mac_ax_adapter *adapter, u8 macid)
{
	struct mac_ax_dctl_info info = {0};
	struct mac_ax_dctl_info mask = {0};
	struct mac_ax_ops *ops = adapter_to_mac_ops(adapter);
	u32 ret = 0;

	if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852C) ||
	    is_chip_id(adapter, MAC_AX_CHIP_ID_8192XB) ||
	    is_chip_id(adapter, MAC_AX_CHIP_ID_8851E) ||
	    is_chip_id(adapter, MAC_AX_CHIP_ID_8852D)) {
		mask.vlan_tag_valid = HW_HDR_CONV_VLANTAG_VALID_ENABLE;
		ret = ops->upd_dctl_info(adapter, &info, &mask, macid, 0);

		if (ret != MACSUCCESS)
			return 0;
		else
			return (u8)info.vlan_tag_valid;
	} else {
		return MACNOTSUP;
	}
}

u32 mac_hdr_conv_tx_qos_field_en(struct mac_ax_adapter *adapter,
				 u8 macid, u8 wlan_with_qosc_en, u8 qos_field_h)
{
	struct mac_ax_dctl_info info = {0};
	struct mac_ax_dctl_info mask = {0};
	struct mac_ax_ops *ops = adapter_to_mac_ops(adapter);
	u32 ret = 0;

	info.qos_field_h_en = wlan_with_qosc_en;
	info.qos_field_h = qos_field_h;
	mask.qos_field_h_en = HW_HDR_CONV_QOS_FIELD_ENABLE;
	mask.qos_field_h = FWCMD_H2C_DCTRL_QOS_FIELD_H_MSK;
	ret = ops->upd_dctl_info(adapter, &info, &mask, macid, 1);

	return ret;
}

u8 mac_hdr_conv_tx_get_qos_field_en(struct mac_ax_adapter *adapter, u8 macid)
{
	struct mac_ax_dctl_info info = {0};
	struct mac_ax_dctl_info mask = {0};
	struct mac_ax_ops *ops = adapter_to_mac_ops(adapter);
	u32 ret = 0;

	mask.qos_field_h_en = HW_HDR_CONV_QOS_FIELD_ENABLE;
	ret = ops->upd_dctl_info(adapter, &info, &mask, macid, 0);

	if (ret != MACSUCCESS)
		return 0;
	else
		return (u8)info.qos_field_h_en;
}

u8 mac_hdr_conv_tx_get_qos_field_h(struct mac_ax_adapter *adapter, u8 macid)
{
	struct mac_ax_dctl_info info = {0};
	struct mac_ax_dctl_info mask = {0};
	struct mac_ax_ops *ops = adapter_to_mac_ops(adapter);
	u32 ret = 0;

	mask.qos_field_h = FWCMD_H2C_DCTRL_QOS_FIELD_H_MSK;
	ret = ops->upd_dctl_info(adapter, &info, &mask, macid, 0);

	if (ret != MACSUCCESS)
		return 0;
	else
		return (u8)info.qos_field_h;
}

u32 mac_hdr_conv_tx_target_wlan_hdr_len(struct mac_ax_adapter *adapter,
					u8 macid, u8 wlan_hdr_len)
{
	struct mac_ax_dctl_info info = {0};
	struct mac_ax_dctl_info mask = {0};
	struct mac_ax_ops *ops = adapter_to_mac_ops(adapter);
	u32 ret = 0;

	info.mhdr_len = wlan_hdr_len;
	mask.mhdr_len = FWCMD_H2C_DCTRL_V1_MHDR_LEN_MSK;
	ret = ops->upd_dctl_info(adapter, &info, &mask, macid, 1);

	return ret;
}

u8 mac_hdr_conv_tx_get_target_wlan_hdr_len(struct mac_ax_adapter *adapter, u8 macid)
{
	struct mac_ax_dctl_info info = {0};
	struct mac_ax_dctl_info mask = {0};
	struct mac_ax_ops *ops = adapter_to_mac_ops(adapter);
	u32 ret = 0;

	mask.mhdr_len = FWCMD_H2C_DCTRL_V1_MHDR_LEN_MSK;
	ret = ops->upd_dctl_info(adapter, &info, &mask, macid, 0);

	if (ret != MACSUCCESS)
		return 0;
	else
		return (u8)info.mhdr_len;
}


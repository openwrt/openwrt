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
#include "hw_seq.h"

#if MAC_AX_FW_REG_OFLD
u32 mac_set_hwseq_reg(struct mac_ax_adapter *adapter,
		      u8 idx,
		      u16 val)
{
	u32 ret = 0;
	u8 *buf;
#if MAC_AX_PHL_H2C
	struct rtw_h2c_pkt *h2cb;
#else
	struct h2c_buf *h2cb;
#endif
	struct mac_ax_set_hwseq_reg *content;

	h2cb = h2cb_alloc(adapter, H2CB_CLASS_CMD);
	if (!h2cb)
		return MACNPTR;

	buf = h2cb_put(h2cb, sizeof(struct mac_ax_set_hwseq_reg));
	if (!buf) {
		ret = MACNOBUF;
		goto fail;
	}

	content = (struct mac_ax_set_hwseq_reg *)buf;

	content->reg_idx = idx;
	content->seq_val = val;

	ret = h2c_pkt_set_hdr(adapter, h2cb,
			      FWCMD_TYPE_H2C,
			      FWCMD_H2C_CAT_MAC,
			      FWCMD_H2C_CL_FW_OFLD,
			      FWCMD_H2C_FUNC_SET_HWSEQ_REG,
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
u32 mac_set_hwseq_reg(struct mac_ax_adapter *adapter,
		      u8 idx,
		      u16 val)
{
	u32 reg_val;
	u32 ret = MACNOTSUP;
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);

	switch (idx) {
	case R_AX_HW_SEQ_0:
#if (MAC_AX_8852A_SUPPORT || MAC_AX_8852B_SUPPORT || MAC_AX_8851B_SUPPORT)
		if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852A) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8852B) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8851B)) {
			reg_val = MAC_REG_R32(R_AX_HW_SEQ_0_1);
			reg_val &= ~((u32)B_AX_HW_SEQ0_MSK << B_AX_HW_SEQ0_SH);
			reg_val |= (val << B_AX_HW_SEQ0_SH);
			MAC_REG_W32(R_AX_HW_SEQ_0_1, reg_val);
			ret = MACSUCCESS;
		}
#endif
#if (MAC_AX_8852C_SUPPORT || MAC_AX_8192XB_SUPPORT || MAC_AX_8851E_SUPPORT || \
MAC_AX_8852D_SUPPORT || MAC_AX_1115E_SUPPORT)
		if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852C) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8192XB) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8851E) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8852D) ||
		    is_chip_id(adapter, MAC_BE_CHIP_ID_1115E)) {
			reg_val = MAC_REG_R32(R_AX_CMAC_HWSSN01);
			reg_val &= ~((u32)B_AX_HW_CMAC_SSN0_MSK << B_AX_HW_CMAC_SSN0_SH);
			reg_val |= (val << B_AX_HW_CMAC_SSN0_SH);
			MAC_REG_W32(R_AX_CMAC_HWSSN01, reg_val);
			ret = MACSUCCESS;
		}
#endif
		break;
	case R_AX_HW_SEQ_1:
#if (MAC_AX_8852A_SUPPORT || MAC_AX_8852B_SUPPORT || MAC_AX_8851B_SUPPORT)
		if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852A) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8852B) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8851B)) {
			reg_val = MAC_REG_R32(R_AX_HW_SEQ_0_1);
			reg_val &= ~((u32)B_AX_HW_SEQ1_MSK << B_AX_HW_SEQ1_SH);
			reg_val |= (val << B_AX_HW_SEQ1_SH);
			MAC_REG_W32(R_AX_HW_SEQ_0_1, reg_val);
			ret = MACSUCCESS;
		}
#endif
#if (MAC_AX_8852C_SUPPORT || MAC_AX_8192XB_SUPPORT || MAC_AX_8851E_SUPPORT || \
MAC_AX_8852D_SUPPORT || MAC_AX_1115E_SUPPORT)
		if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852C) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8192XB) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8851E) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8852D) ||
		    is_chip_id(adapter, MAC_BE_CHIP_ID_1115E)) {
			reg_val = MAC_REG_R32(R_AX_CMAC_HWSSN01);
			reg_val &= ~((u32)B_AX_HW_CMAC_SSN1_MSK << B_AX_HW_CMAC_SSN1_SH);
			reg_val |= (val << B_AX_HW_CMAC_SSN1_SH);
			MAC_REG_W32(R_AX_CMAC_HWSSN01, reg_val);
			ret = MACSUCCESS;
		}
#endif
		break;
	case R_AX_HW_SEQ_2:
#if (MAC_AX_8852A_SUPPORT || MAC_AX_8852B_SUPPORT || MAC_AX_8851B_SUPPORT)
		if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852A) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8852B) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8851B)) {
			reg_val = MAC_REG_R32(R_AX_HW_SEQ_2_3);
			reg_val &= ~((u32)B_AX_HW_SEQ2_MSK << B_AX_HW_SEQ2_SH);
			reg_val |= (val << B_AX_HW_SEQ2_SH);
			MAC_REG_W32(R_AX_HW_SEQ_2_3, reg_val);
			ret = MACSUCCESS;
		}
#endif
#if (MAC_AX_8852C_SUPPORT || MAC_AX_8192XB_SUPPORT || MAC_AX_8851E_SUPPORT || \
MAC_AX_8852D_SUPPORT || MAC_AX_1115E_SUPPORT)
		if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852C) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8192XB) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8851E) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8852D) ||
		    is_chip_id(adapter, MAC_BE_CHIP_ID_1115E)) {
			reg_val = MAC_REG_R32(R_AX_CMAC_HWSSN23);
			reg_val &= ~((u32)B_AX_HW_CMAC_SSN2_MSK << B_AX_HW_CMAC_SSN2_SH);
			reg_val |= (val << B_AX_HW_CMAC_SSN2_SH);
			MAC_REG_W32(R_AX_CMAC_HWSSN23, reg_val);
			ret = MACSUCCESS;
		}
#endif
		break;
	case R_AX_HW_SEQ_3:
#if (MAC_AX_8852A_SUPPORT || MAC_AX_8852B_SUPPORT || MAC_AX_8851B_SUPPORT)
		if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852A) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8852B) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8851B)) {
			reg_val = MAC_REG_R32(R_AX_HW_SEQ_2_3);
			reg_val &= ~((u32)B_AX_HW_SEQ3_MSK << B_AX_HW_SEQ3_SH);
			reg_val |= (val << B_AX_HW_SEQ3_SH);
			MAC_REG_W32(R_AX_HW_SEQ_2_3, reg_val);
			ret = MACSUCCESS;
		}
#endif
#if (MAC_AX_8852C_SUPPORT || MAC_AX_8192XB_SUPPORT || MAC_AX_8851E_SUPPORT || \
MAC_AX_8852D_SUPPORT || MAC_AX_1115E_SUPPORT)
		if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852C) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8192XB) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8851E) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8852D) ||
		    is_chip_id(adapter, MAC_BE_CHIP_ID_1115E)) {
			reg_val = MAC_REG_R32(R_AX_CMAC_HWSSN23);
			reg_val &= ~((u32)B_AX_HW_CMAC_SSN3_MSK << B_AX_HW_CMAC_SSN3_SH);
			reg_val |= (val << B_AX_HW_CMAC_SSN3_SH);
			MAC_REG_W32(R_AX_CMAC_HWSSN23, reg_val);
			ret = MACSUCCESS;
		}
#endif
		break;
	default:
		ret = MACNOITEM;
	}
	return ret;
}
#endif

u32 mac_set_hwseq_dctrl(struct mac_ax_adapter *adapter,
			u8 macid,
			struct mac_ax_dctl_seq_cfg *seq_info)
{
	struct mac_ax_dctl_info info = {0};
	struct mac_ax_dctl_info mask = {0};
	struct mac_ax_ops *ops = adapter_to_mac_ops(adapter);
	u32 ret = 0;

	info.seq0 = seq_info->seq0_val;
	info.seq1 = seq_info->seq1_val;
	info.seq2 = seq_info->seq2_val;
	info.seq3 = seq_info->seq3_val;
	info.hw_exseq_macid = seq_info->hw_exseq_macid;
	mask.seq0 = FWCMD_H2C_DCTRL_SEQ0_MSK;
	mask.seq1 = FWCMD_H2C_DCTRL_SEQ1_MSK;
	mask.seq2 = FWCMD_H2C_DCTRL_SEQ2_MSK;
	mask.seq3 = FWCMD_H2C_DCTRL_SEQ3_MSK;
	mask.hw_exseq_macid = FWCMD_H2C_DCTRL_V1_HW_EXSEQ_MACID_MSK;
	ret = ops->upd_dctl_info(adapter, &info, &mask, macid, 1);

	return ret;
}

u32 mac_get_hwseq_cfg(struct mac_ax_adapter *adapter,
		      u8 macid, u8 ref_sel,
		      struct mac_ax_dctl_seq_cfg *seq_info)
{
	struct mac_ax_dctl_info info = {0};
	struct mac_ax_dctl_info mask = {0};
	struct mac_ax_ops *a_ops = adapter_to_mac_ops(adapter);
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
	u32 ret = 0;

	if (ref_sel) {
#if (MAC_AX_8852A_SUPPORT || MAC_AX_8852B_SUPPORT || MAC_AX_8851B_SUPPORT)
		if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852A) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8852B) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8851B)) {
			seq_info->seq0_val = (MAC_REG_R32(R_AX_HW_SEQ_0_1) >> B_AX_HW_SEQ0_SH)
					    & B_AX_HW_SEQ0_MSK;
			seq_info->seq1_val = (MAC_REG_R32(R_AX_HW_SEQ_0_1) >> B_AX_HW_SEQ1_SH)
					    & B_AX_HW_SEQ1_MSK;
			seq_info->seq2_val = (MAC_REG_R32(R_AX_HW_SEQ_2_3) >> B_AX_HW_SEQ2_SH)
					    & B_AX_HW_SEQ2_MSK;
			seq_info->seq3_val = (MAC_REG_R32(R_AX_HW_SEQ_2_3) >> B_AX_HW_SEQ3_SH)
					    & B_AX_HW_SEQ3_MSK;
			return MACSUCCESS;
			}
#endif
#if (MAC_AX_8852C_SUPPORT || MAC_AX_8192XB_SUPPORT || MAC_AX_8851E_SUPPORT || \
MAC_AX_8852D_SUPPORT || MAC_AX_1115E_SUPPORT)
		if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852C) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8192XB) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8851E) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8852D) ||
		    is_chip_id(adapter, MAC_BE_CHIP_ID_1115E)) {
			seq_info->seq0_val = (MAC_REG_R32(R_AX_CMAC_HWSSN01) >>
					      B_AX_HW_CMAC_SSN0_SH) & B_AX_HW_CMAC_SSN0_MSK;
			seq_info->seq1_val = (MAC_REG_R32(R_AX_CMAC_HWSSN01) >>
					      B_AX_HW_CMAC_SSN1_SH) & B_AX_HW_CMAC_SSN1_MSK;
			seq_info->seq2_val = (MAC_REG_R32(R_AX_CMAC_HWSSN23) >>
					      B_AX_HW_CMAC_SSN2_SH) & B_AX_HW_CMAC_SSN2_MSK;
			seq_info->seq3_val = (MAC_REG_R32(R_AX_CMAC_HWSSN23) >>
					      B_AX_HW_CMAC_SSN3_SH) & B_AX_HW_CMAC_SSN3_MSK;
			return MACSUCCESS;
		}
#endif
	} else {
		ret = a_ops->upd_dctl_info(adapter, &info, &mask, macid, 0);
		if (ret != MACSUCCESS)
			return ret;

		seq_info->seq0_val = info.seq0;
		seq_info->seq1_val = info.seq1;
		seq_info->seq2_val = info.seq2;
		seq_info->seq3_val = info.seq3;
		seq_info->hw_exseq_macid = info.hw_exseq_macid;
		return MACSUCCESS;
	}
	return MACNOTSUP;
}


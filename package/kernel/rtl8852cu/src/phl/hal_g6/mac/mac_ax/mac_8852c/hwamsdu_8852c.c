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

#include "hwamsdu_8852c.h"

#if MAC_AX_8852C_SUPPORT
u32 mac_enable_cut_hwamsdu_8852c(struct mac_ax_adapter *adapter,
				 u8 enable,
				 enum mac_ax_ex_shift aligned)
{
	u32 val;
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);

	if (chk_patch_cut_amsdu_rls_ple_issue(adapter) == (u32)PATCH_ENABLE)
		return MACNOTSUP;

	if (aligned > MAC_AX_BYTE_ALIGNED_8) {
		PLTFM_MSG_ERR("%s: invalid byte aligned configruation\n", __func__);
		return MACNOITEM;
	}

	val = MAC_REG_R32(R_AX_CUT_AMSDU_CTRL);
	if (!enable)
		val &= ~B_AX_EN_CUT_AMSDU_V1;
	else
		val |= B_AX_EN_CUT_AMSDU_V1;
	MAC_REG_W32(R_AX_CUT_AMSDU_CTRL, val);

	val = MAC_REG_R32(R_AX_CUT_AMSDU_CTRL_2);
	val = SET_CLR_WORD(val, aligned, B_AX_EXTRA_SHIFT);
	MAC_REG_W32(R_AX_CUT_AMSDU_CTRL_2, val);

	return MACSUCCESS;
}

u32 mac_cut_hwamsdu_chk_mpdu_len_en_8852c(struct mac_ax_adapter *adapter,
					  u8 enable,
					  u8 low_th,
					  u16 high_th)
{
	u32 val;
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);

	val = MAC_REG_R32(R_AX_CUT_AMSDU_CTRL);
	if (!enable)
		val &= ~B_AX_CUT_AMSDU_CHKLEN_EN_V1;
	else
		val |= B_AX_CUT_AMSDU_CHKLEN_EN_V1;
	val = SET_CLR_WORD(val, low_th, B_AX_CUT_AMSDU_CHKLEN_L_TH);
	val = SET_CLR_WORD(val, high_th, B_AX_CUT_AMSDU_CHKLEN_H_TH);
	MAC_REG_W32(R_AX_CUT_AMSDU_CTRL, val);

	return MACSUCCESS;
}
#endif
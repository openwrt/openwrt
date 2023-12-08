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
#include "cmac_tx_8852c.h"
#include "../cmac_tx.h"
#include "../../mac_reg.h"
#if MAC_AX_8852C_SUPPORT

#define PTCL_TXQ_TWT2		16
#define PTCL_TXQ_TWT3		17
#define PTCL_TXQ_TB		18

#define TX_EN_CTRL_ALL		0x3FFFF

u32 macid_idle_ck_8852c(struct mac_ax_adapter *adapter,
			struct mac_role_tbl *role)
{
	u32 cnt;
	u8 val8;
	u32 ret;
	u8 band;
	u32 val32, val32_1;
	u8 macid;
	u8 txq;
	u32 poll_addr;
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);

	band = role->info.band;
	ret = check_mac_en(adapter, band, MAC_AX_CMAC_SEL);
	if (ret != MACSUCCESS)
		return ret;

	poll_addr = band ? R_AX_PTCL_TX_CTN_SEL_C1 : R_AX_PTCL_TX_CTN_SEL;

	val8 = MAC_REG_R8(poll_addr);
	if (val8 & B_AX_PTCL_TX_ON_STAT)
		tx_on_dly(adapter, band);
	else
		return MACSUCCESS;

	macid = role->macid;

	cnt = PTCL_IDLE_POLL_CNT;
	while (--cnt) {
		val8 = MAC_REG_R8(poll_addr);
		txq = GET_FIELD(val8, B_AX_PTCL_TX_QUEUE_IDX);
		if ((val8 & B_AX_PTCL_TX_ON_STAT) && (val8 & B_AX_PTCL_DROP)) {
			PLTFM_DELAY_US(SW_CVR_DUR_US);
		} else if ((val8 & B_AX_PTCL_TX_ON_STAT) &&
			   txq != PTCL_TXQ_HIQ && txq != PTCL_TXQ_BCNQ &&
			   txq != PTCL_TXQ_MG0 && txq != PTCL_TXQ_MG1 &&
			   txq != PTCL_TXQ_MG2 && txq != PTCL_TXQ_TB) {
			PLTFM_DELAY_US(SW_CVR_DUR_US);
			val32 = MAC_REG_R32(band ? R_AX_PTCL_TX_MACID_0_V1_C1 :
					    R_AX_PTCL_TX_MACID_0_V1);
			val32_1 = MAC_REG_R32(band ? R_AX_PTCL_TX_MACID_1_C1 :
					      R_AX_PTCL_TX_MACID_1);
			if (macid == GET_FIELD(val32, B_AX_TX_MACID_0) ||
			    macid == GET_FIELD(val32, B_AX_TX_MACID_1) ||
			    macid == GET_FIELD(val32, B_AX_TX_MACID_2) ||
			    macid == GET_FIELD(val32, B_AX_TX_MACID_3) ||
			    macid == GET_FIELD(val32_1, B_AX_TX_MACID_4) ||
			    macid == GET_FIELD(val32_1, B_AX_TX_MACID_5) ||
			    macid == GET_FIELD(val32_1, B_AX_TX_MACID_6) ||
			    macid == GET_FIELD(val32_1, B_AX_TX_MACID_7))
				PLTFM_DELAY_US(SW_CVR_DUR_US);
			else
				break;
		} else {
			break;
		}
	}
	PLTFM_MSG_ALWAYS("%s: cnt %d, band %d, 0x%x\n", __func__, cnt, band, val8);
	if (!cnt)
		return MACPOLLTXIDLE;

	return MACSUCCESS;
}

u32 stop_sch_tx_8852c(struct mac_ax_adapter *adapter, enum sch_tx_sel sel,
		      struct mac_ax_sch_tx_en_cfg *bak)
{
	struct mac_ax_sch_tx_en_cfg cfg;
	u32 ret;

	ret = get_hw_sch_tx_en(adapter, bak);
	if (ret != MACSUCCESS)
		return ret;

	cfg.band = bak->band;
	u32_2_sch(adapter, &cfg.tx_en_mask, 0);
	u32_2_sch(adapter, &cfg.tx_en, 0);

	switch (sel) {
	case SCH_TX_SEL_ALL:
		u32_2_sch(adapter, &cfg.tx_en, 0);
		u32_2_sch(adapter, &cfg.tx_en_mask, TX_EN_CTRL_ALL);
		ret = set_hw_sch_tx_en(adapter, &cfg);
		if (ret != MACSUCCESS)
			return ret;
		break;
	case SCH_TX_SEL_HIQ:
		cfg.tx_en.hi = 0;
		cfg.tx_en_mask.hi = 1;
		ret = set_hw_sch_tx_en(adapter, &cfg);
		if (ret != MACSUCCESS)
			return ret;
		break;
	case SCH_TX_SEL_MG0:
		cfg.tx_en.mg0 = 0;
		cfg.tx_en_mask.mg0 = 1;
		ret = set_hw_sch_tx_en(adapter, &cfg);
		if (ret != MACSUCCESS)
			return ret;
		break;
	case SCH_TX_SEL_MACID:
		u32_2_sch(adapter, &cfg.tx_en, 0);
		u32_2_sch(adapter, &cfg.tx_en_mask, TX_EN_CTRL_ALL);
		cfg.tx_en_mask.mg1 = 0;
		cfg.tx_en_mask.mg2 = 0;
		cfg.tx_en_mask.hi = 0;
		cfg.tx_en_mask.bcn = 0;
		ret = set_hw_sch_tx_en(adapter, &cfg);
		if (ret != MACSUCCESS)
			return ret;
		break;
	default:
		return MACNOITEM;
	}

	return MACSUCCESS;
}

#endif /* #if MAC_AX_8852C_SUPPORT */

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
#include "dle_8852c.h"

#if MAC_AX_8852C_SUPPORT

const u32 b_preld_hiq_mbid_en_l_52c[MAC_AX_P0_MBID_MAX] = {
	B_AX_PRELD_HIQ_P0_EN, B_AX_PRELD_HIQ_P0MB1_EN, B_AX_PRELD_HIQ_P0MB2_EN,
	B_AX_PRELD_HIQ_P0MB3_EN, B_AX_PRELD_HIQ_P0MB4_EN,
	B_AX_PRELD_HIQ_P0MB5_EN, B_AX_PRELD_HIQ_P0MB6_EN,
	B_AX_PRELD_HIQ_P0MB7_EN, B_AX_PRELD_HIQ_P0MB8_EN,
	B_AX_PRELD_HIQ_P0MB9_EN, B_AX_PRELD_HIQ_P0MB10_EN,
	B_AX_PRELD_HIQ_P0MB11_EN, B_AX_PRELD_HIQ_P0MB12_EN,
	B_AX_PRELD_HIQ_P0MB13_EN, B_AX_PRELD_HIQ_P0MB14_EN,
	B_AX_PRELD_HIQ_P0MB15_EN
};

const u32 b_preld_hiq_port_en_l_52c[MAC_AX_PORT_NUM] = {
	B_AX_PRELD_HIQ_P0_EN, B_AX_PRELD_HIQ_P1_EN, B_AX_PRELD_HIQ_P2_EN,
	B_AX_PRELD_HIQ_P3_EN, B_AX_PRELD_HIQ_P4_EN
};

static struct mac_ax_dle_dfi_info dle_dfi_wde_bufmgn_freepg = {
	0,
	1,
	1
};

static struct mac_ax_dle_dfi_info dle_dfi_wde_bufmgn_quota = {
	0,
	4,
	1
};

static struct mac_ax_dle_dfi_info dle_dfi_wde_bufmgn_pagellt = {
	0,
	0xFFF,
	1
};

static struct mac_ax_dle_dfi_info dle_dfi_wde_bufmgn_pktinfo = {
	0,
	0xFFF,
	1
};

static struct mac_ax_dle_dfi_info dle_dfi_wde_quemgn_prepkt = {
	0,
	0xFFF,
	1
};

static struct mac_ax_dle_dfi_info dle_dfi_wde_quemgn_nxtpkt = {
	0,
	0xFFF,
	1
};

static struct mac_ax_dle_dfi_info dle_dfi_wde_quemgn_qlnktbl = {
	0,
	0x453,
	1
};

static struct mac_ax_dle_dfi_info dle_dfi_wde_quemgn_qempty = {
	0,
	0x11,
	1
};

static struct mac_ax_dle_dfi_info dle_dfi_ple_bufmgn_freepg = {
	0,
	1,
	1
};

static struct mac_ax_dle_dfi_info dle_dfi_ple_bufmgn_quota = {
	0,
	0xB,
	1
};

static struct mac_ax_dle_dfi_info dle_dfi_ple_bufmgn_pagellt = {
	0,
	0xDBF,
	1
};

static struct mac_ax_dle_dfi_info dle_dfi_ple_bufmgn_pktinfo = {
	0,
	0xDBF,
	1
};

static struct mac_ax_dle_dfi_info dle_dfi_ple_quemgn_prepkt = {
	0,
	0xDBF,
	1
};

static struct mac_ax_dle_dfi_info dle_dfi_ple_quemgn_nxtpkt = {
	0,
	0xDBF,
	1
};

static struct mac_ax_dle_dfi_info dle_dfi_ple_quemgn_qlnktbl = {
	0,
	0x41,
	1
};

static struct mac_ax_dle_dfi_info dle_dfi_ple_quemgn_qempty = {
	0,
	1,
	1
};

u32 dle_dfi_sel_8852c(struct mac_ax_adapter *adapter,
		      struct mac_ax_dle_dfi_info **info,
		      u32 *target, u32 sel)
{
	switch (sel) {
	case MAC_AX_DLE_DFI_SEL_WDE_BUFMGN_FREEPG:
		*info = &dle_dfi_wde_bufmgn_freepg;
		*target = DLE_DFI_TYPE_FREEPG;
		PLTFM_MSG_ALWAYS("Dump debug port WDE BUFMGN FREEPG:\n");
		break;
	case MAC_AX_DLE_DFI_SEL_WDE_BUFMGN_QUOTA:
		*info = &dle_dfi_wde_bufmgn_quota;
		*target = DLE_DFI_TYPE_QUOTA;
		PLTFM_MSG_ALWAYS("Dump debug port WDE BUFMGN QUOTA:\n");
		break;
	case MAC_AX_DLE_DFI_SEL_WDE_BUFMGN_PAGELLT:
		*info = &dle_dfi_wde_bufmgn_pagellt;
		*target = DLE_DFI_TYPE_PAGELLT;
		PLTFM_MSG_ALWAYS("Dump debug port WDE BUFMGN PAGELLT:\n");
		break;
	case MAC_AX_DLE_DFI_SEL_WDE_BUFMGN_PKTINFO:
		*info = &dle_dfi_wde_bufmgn_pktinfo;
		*target = DLE_DFI_TYPE_PKTINFO;
		PLTFM_MSG_ALWAYS("Dump debug port WDE BUFMGN PKTINFO:\n");
		break;
	case MAC_AX_DLE_DFI_SEL_WDE_QUEMGN_PREPKT:
		*info = &dle_dfi_wde_quemgn_prepkt;
		*target = DLE_DFI_TYPE_PREPKTLLT;
		PLTFM_MSG_ALWAYS("Dump debug port WDE QUEMGN PREPKT:\n");
		break;
	case MAC_AX_DLE_DFI_SEL_WDE_QUEMGN_NXTPKT:
		*info = &dle_dfi_wde_quemgn_nxtpkt;
		*target = DLE_DFI_TYPE_NXTPKTLLT;
		PLTFM_MSG_ALWAYS("Dump debug port WDE QUEMGN NXTPKT:\n");
		break;
	case MAC_AX_DLE_DFI_SEL_WDE_QUEMGN_QLNKTBL:
		*info = &dle_dfi_wde_quemgn_qlnktbl;
		*target = DLE_DFI_TYPE_QLNKTBL;
		PLTFM_MSG_ALWAYS("Dump debug port WDE QUEMGN QLNKTBL:\n");
		break;
	case MAC_AX_DLE_DFI_SEL_WDE_QUEMGN_QEMPTY:
		*info = &dle_dfi_wde_quemgn_qempty;
		*target = DLE_DFI_TYPE_QEMPTY;
		PLTFM_MSG_ALWAYS("Dump debug port WDE QUEMGN QEMPTY:\n");
		break;
	case MAC_AX_DLE_DFI_SEL_PLE_BUFMGN_FREEPG:
		*info = &dle_dfi_ple_bufmgn_freepg;
		*target = DLE_DFI_TYPE_FREEPG;
		PLTFM_MSG_ALWAYS("Dump debug port PLE BUFMGN FREEPG:\n");
		break;
	case MAC_AX_DLE_DFI_SEL_PLE_BUFMGN_QUOTA:
		*info = &dle_dfi_ple_bufmgn_quota;
		*target = DLE_DFI_TYPE_QUOTA;
		PLTFM_MSG_ALWAYS("Dump debug port PLE BUFMGN QUOTA:\n");
		break;
	case MAC_AX_DLE_DFI_SEL_PLE_BUFMGN_PAGELLT:
		*info = &dle_dfi_ple_bufmgn_pagellt;
		*target = DLE_DFI_TYPE_PAGELLT;
		PLTFM_MSG_ALWAYS("Dump debug port PLE BUFMGN PAGELLT:\n");
		break;
	case MAC_AX_DLE_DFI_SEL_PLE_BUFMGN_PKTINFO:
		*info = &dle_dfi_ple_bufmgn_pktinfo;
		*target = DLE_DFI_TYPE_PKTINFO;
		PLTFM_MSG_ALWAYS("Dump debug port PLE BUFMGN PKTINFO:\n");
		break;
	case MAC_AX_DLE_DFI_SEL_PLE_QUEMGN_PREPKT:
		*info = &dle_dfi_ple_quemgn_prepkt;
		*target = DLE_DFI_TYPE_PREPKTLLT;
		PLTFM_MSG_ALWAYS("Dump debug port PLE QUEMGN PREPKT:\n");
		break;
	case MAC_AX_DLE_DFI_SEL_PLE_QUEMGN_NXTPKT:
		*info = &dle_dfi_ple_quemgn_nxtpkt;
		*target = DLE_DFI_TYPE_NXTPKTLLT;
		PLTFM_MSG_ALWAYS("Dump debug port PLE QUEMGN NXTPKT:\n");
		break;
	case MAC_AX_DLE_DFI_SEL_PLE_QUEMGN_QLNKTBL:
		*info = &dle_dfi_ple_quemgn_qlnktbl;
		*target = DLE_DFI_TYPE_QLNKTBL;
		PLTFM_MSG_ALWAYS("Dump debug port PLE QUEMGN QLNKTBL:\n");
		break;
	case MAC_AX_DLE_DFI_SEL_PLE_QUEMGN_QEMPTY:
		*info = &dle_dfi_ple_quemgn_qempty;
		*target = DLE_DFI_TYPE_QEMPTY;
		PLTFM_MSG_ALWAYS("Dump debug port PLE QUEMGN QEMPTY:\n");
		break;
	default:
		PLTFM_MSG_ALWAYS("dle dfi select err\n");
		*info = NULL;
		return MACDBGPORTSEL;
	}

	return MACSUCCESS;
}

u32 dle_dfi_ctrl_8852c(struct mac_ax_adapter *adapter, struct dle_dfi_ctrl_t *ctrl_p)
{
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
	u32 cnt, ctrl_reg, data_reg, ctrl_data;

	switch (ctrl_p->type) {
	case DLE_CTRL_TYPE_WDE:
		ctrl_reg = R_AX_WDE_DBG_FUN_INTF_CTL;
		data_reg = R_AX_WDE_DBG_FUN_INTF_DATA;
		ctrl_data = SET_WORD(ctrl_p->target,
				     B_AX_WDE_DFI_TRGSEL) |
			    SET_WORD(ctrl_p->addr, B_AX_WDE_DFI_ADDR) |
			    B_AX_WDE_DFI_ACTIVE;
		break;
	case DLE_CTRL_TYPE_PLE:
		ctrl_reg = R_AX_PLE_DBG_FUN_INTF_CTL;
		data_reg = R_AX_PLE_DBG_FUN_INTF_DATA;
		ctrl_data = SET_WORD(ctrl_p->target,
				     B_AX_PLE_DFI_TRGSEL) |
			    SET_WORD(ctrl_p->addr, B_AX_PLE_DFI_ADDR) |
			    B_AX_PLE_DFI_ACTIVE;
		break;
	default:
		PLTFM_MSG_ERR("[ERR] dfi ctrl type %d\n", ctrl_p->type);
		return MACFUNCINPUT;
	}

	MAC_REG_W32(ctrl_reg, ctrl_data);

	cnt = DLE_DFI_WAIT_CNT;
	while (cnt && MAC_REG_R32(ctrl_reg) & B_AX_WDE_DFI_ACTIVE) {
		PLTFM_DELAY_US(DLE_DFI_WAIT_US);
		cnt--;
	}

	if (!cnt) {
		PLTFM_MSG_ERR("[ERR] dle dfi ctrl 0x%X set 0x%X timeout\n",
			      ctrl_reg, ctrl_data);
		return MACPOLLTO;
	}

	ctrl_p->out_data = MAC_REG_R32(data_reg);

	switch (ctrl_p->target) {
	case DLE_DFI_TYPE_FREEPG:
		switch (ctrl_p->addr) {
		case DFI_TYPE_FREEPG_IDX:
			ctrl_p->u.freepg.free_headpg =
				GET_FIELD(ctrl_p->out_data, B_AX_DLE_FREE_HEADPG);
			ctrl_p->u.freepg.free_tailpg =
				GET_FIELD(ctrl_p->out_data, B_AX_DLE_FREE_TAILPG);
			break;
		case DFI_TYPE_FREEPG_PUBNUM:
			ctrl_p->u.freepg.pub_pgnum =
				GET_FIELD(ctrl_p->out_data, B_AX_DLE_PUB_PGNUM);
			break;
		default:
			PLTFM_MSG_ERR("dle dfi ctrl tar%d addr%d mismatch\n",
				      ctrl_p->target, ctrl_p->addr);
			return MACFUNCINPUT;
		}
		break;
	case DLE_DFI_TYPE_QUOTA:
		ctrl_p->u.quota.rsv_pgnum =
			GET_FIELD(ctrl_p->out_data, B_AX_DLE_RSV_PGNUM);
		ctrl_p->u.quota.use_pgnum =
			GET_FIELD(ctrl_p->out_data, B_AX_DLE_USE_PGNUM);
		break;
	case DLE_DFI_TYPE_QEMPTY:
		ctrl_p->u.qempty.qempty =
			GET_FIELD(ctrl_p->out_data, B_AX_DLE_QEMPTY_GRP);
		break;
	default:
		break;
	}

	return MACSUCCESS;
}

u32 dle_is_txq_empty_8852c(struct mac_ax_adapter *adapter, u8 *val)
{
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
	u32 val32, rval32;

	val32 = B_AX_WDE_EMPTY_QUE_CMAC0_ALL_AC | B_AX_WDE_EMPTY_QUE_CMAC0_MBH |
		B_AX_WDE_EMPTY_QUE_CMAC1_MBH | B_AX_WDE_EMPTY_QUE_CMAC0_WMM0 |
		B_AX_WDE_EMPTY_QUE_CMAC0_WMM1 | B_AX_WDE_EMPTY_QUE_OTHERS |
		B_AX_PLE_EMPTY_QUE_DMAC_MPDU_TX | B_AX_PLE_EMPTY_QTA_DMAC_H2C |
		B_AX_PLE_EMPTY_QUE_DMAC_SEC_TX | B_AX_WDE_EMPTY_QUE_DMAC_PKTIN |
		B_AX_WDE_EMPTY_QTA_DMAC_HIF | B_AX_WDE_EMPTY_QTA_DMAC_WLAN_CPU |
		B_AX_WDE_EMPTY_QTA_DMAC_PKTIN | B_AX_WDE_EMPTY_QTA_DMAC_CPUIO |
		B_AX_PLE_EMPTY_QTA_DMAC_B0_TXPL |
		B_AX_PLE_EMPTY_QTA_DMAC_B1_TXPL |
		B_AX_PLE_EMPTY_QTA_DMAC_MPDU_TX |
		B_AX_PLE_EMPTY_QTA_DMAC_CPUIO |
		B_AX_WDE_EMPTY_QTA_DMAC_DATA_CPU |
		B_AX_PLE_EMPTY_QTA_DMAC_WLAN_CPU;

	rval32 = MAC_REG_R32(R_AX_DLE_EMPTY0);
	if (val32 != (rval32 & val32)) {
		*val = DLE_QUEUE_NONEMPTY;
		PLTFM_MSG_TRACE("[TRACE]TXQ non empty 0x%X\n", rval32);
	} else {
		*val = DLE_QUEUE_EMPTY;
		PLTFM_MSG_TRACE("[TRACE]TXQ empty 0x%X\n", rval32);
	}

	return MACSUCCESS;
}

u32 dle_is_rxq_empty_8852c(struct mac_ax_adapter *adapter, u8 *val)
{
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
	u32 val32, rval32;

	val32 = B_AX_WDE_EMPTY_QUE_DMAC_WDRLS | B_AX_PLE_EMPTY_QUE_DMAC_SEC_RX |
		B_AX_PLE_EMPTY_QUE_DMAC_MPDU_RX | B_AX_PLE_EMPTY_QUE_DMAC_HDP |
		B_AX_PLE_EMPTY_QUE_DMAC_PLRLS | B_AX_PLE_EMPTY_QUE_DMAC_CPUIO |
		B_AX_PLE_EMPTY_QTA_DMAC_C2H | B_AX_PLE_EMPTY_QTA_CMAC0_DMA_RX |
		B_AX_PLE_EMPTY_QTA_CMAC1_DMA_RX |
		B_AX_PLE_EMPTY_QTA_CMAC1_DMA_BBRPT |
		B_AX_PLE_EMPTY_QTA_DMAC_WDRLS;

	rval32 = MAC_REG_R32(R_AX_DLE_EMPTY1);
	if (val32 != (rval32 & val32)) {
		*val = DLE_QUEUE_NONEMPTY;
		PLTFM_MSG_TRACE("[TRACE] RXQ non empty 0x%X\n", rval32);
	} else {
		*val = DLE_QUEUE_EMPTY;
		PLTFM_MSG_TRACE("[TRACE] RXQ empty 0x%X\n", rval32);
	}

	return MACSUCCESS;
}

u32 mac_is_txq_empty_8852c(struct mac_ax_adapter *adapter,
			   struct mac_ax_tx_queue_empty *val)
{
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
	struct dle_dfi_qempty_t qempty;
	u32 val32, val32_emp0, ret;
	u32 i, j, qnum;

	PLTFM_MEMSET(val, 0xFF, sizeof(struct mac_ax_tx_queue_empty));

	qempty.dle_type = DLE_CTRL_TYPE_WDE;
	qnum = WDE_QEMPTY_ACQ_NUM_8852C;
	for (i = 0; i < qnum; i++) {
		qempty.grpsel = i;
		ret = dle_dfi_qempty(adapter, &qempty);
		if (ret != MACSUCCESS) {
			PLTFM_MSG_ERR("[ERR]dle dfi acq empty %d\n", ret);
			return ret;
		}
		for (j = 0 ; j < QEMP_ACQ_GRP_MACID_NUM; j++) {
			val32 = GET_FIEL2(qempty.qempty,
					  j * QEMP_ACQ_GRP_QSEL_SH,
					  QEMP_ACQ_GRP_QSEL_MASK);
			if (val32 != QEMP_ACQ_GRP_QSEL_MASK)
				val->macid_txq_empty[i] &= ~BIT(j);
		}
	}

	qempty.grpsel = WDE_QEMPTY_MGQ_SEL_8852C;
	ret = dle_dfi_qempty(adapter, &qempty);
	if (ret != MACSUCCESS) {
		PLTFM_MSG_ERR("[ERR]dle dfi mgq empty %d\n", ret);
		return ret;
	}
	if (!(qempty.qempty & B_CMAC0_MGQ_NORMAL))
		val->band0_mgnt_empty = 0;

	if (is_curr_dbcc(adapter) && !(qempty.qempty & B_CMAC1_MGQ_NORMAL))
		val->band1_mgnt_empty = 0;

	val32 = B_CMAC0_MGQ_NO_PWRSAV | B_CMAC0_CPUMGQ;
	if (is_curr_dbcc(adapter))
		val32 |= B_CMAC1_MGQ_NO_PWRSAV | B_CMAC1_CPUMGQ;
	if ((qempty.qempty & val32) != val32)
		val->fw_txq_empty = 0;

	val32 = B_AX_WDE_EMPTY_QTA_DMAC_WLAN_CPU |
		B_AX_WDE_EMPTY_QTA_DMAC_DATA_CPU |
		B_AX_PLE_EMPTY_QTA_DMAC_WLAN_CPU;
	val32_emp0 = MAC_REG_R32(R_AX_DLE_EMPTY0);
	if (val32 != (val32_emp0 & val32))
		val->fw_txq_empty = 0;

	if (!(val32_emp0 & B_AX_PLE_EMPTY_QTA_DMAC_H2C))
		val->h2c_empty = 0;

	val32 = B_AX_WDE_EMPTY_QUE_OTHERS | B_AX_PLE_EMPTY_QUE_DMAC_MPDU_TX |
		B_AX_WDE_EMPTY_QTA_DMAC_CPUIO | B_AX_PLE_EMPTY_QTA_DMAC_CPUIO |
		B_AX_WDE_EMPTY_QUE_DMAC_PKTIN | B_AX_WDE_EMPTY_QTA_DMAC_HIF |
		B_AX_PLE_EMPTY_QUE_DMAC_SEC_TX | B_AX_WDE_EMPTY_QTA_DMAC_PKTIN |
		B_AX_PLE_EMPTY_QTA_DMAC_B0_TXPL |
		B_AX_PLE_EMPTY_QTA_DMAC_B1_TXPL |
		B_AX_PLE_EMPTY_QTA_DMAC_MPDU_TX;

	if (val32 != (val32_emp0 & val32))
		val->others_empty = 0;

	return MACSUCCESS;
}

u32 mac_is_rxq_empty_8852c(struct mac_ax_adapter *adapter,
			   struct mac_ax_rx_queue_empty *val)
{
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
	u32 val32, val32_emp1;

	PLTFM_MEMSET(val, 0xFF, sizeof(struct mac_ax_rx_queue_empty));

	val32_emp1 = MAC_REG_R32(R_AX_DLE_EMPTY1);
	if (!(val32_emp1 & B_AX_PLE_EMPTY_QTA_CMAC0_DMA_RX))
		val->band0_rxq_empty = 0;

	if (!(val32_emp1 & B_AX_PLE_EMPTY_QTA_CMAC1_DMA_RX))
		val->band1_rxq_empty = 0;

	if (!(val32_emp1 & B_AX_PLE_EMPTY_QTA_DMAC_C2H))
		val->c2h_empty = 0;

	val32 = B_AX_WDE_EMPTY_QUE_DMAC_WDRLS | B_AX_PLE_EMPTY_QUE_DMAC_SEC_RX |
		B_AX_PLE_EMPTY_QUE_DMAC_MPDU_RX | B_AX_PLE_EMPTY_QUE_DMAC_HDP |
		B_AX_PLE_EMPTY_QUE_DMAC_PLRLS | B_AX_PLE_EMPTY_QUE_DMAC_CPUIO |
		B_AX_PLE_EMPTY_QTA_CMAC1_DMA_BBRPT |
		B_AX_PLE_EMPTY_QTA_DMAC_WDRLS;

	if (val32 != (val32_emp1 & val32))
		val->others_empty = 0;

	return MACSUCCESS;
}

u32 preload_cfg_set_8852c(struct mac_ax_adapter *adapter, enum mac_ax_band band,
			  struct mac_ax_preld_cfg *cfg)
{
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
	struct mac_ax_cctl_preld_cfg cctl_cfg = {0};
	u32 reg, val32, ret, i;
	u8 grp0_num, grp1_num, val8;
	u8 macid_en_num;

	macid_en_num = 0;
	for (i = MAC_AX_PRELD_MACID0; i < MAC_AX_PRELD_MACID_MAX; i++) {
		cctl_cfg.macid = cfg->macid[i];
		cctl_cfg.en = cfg->macid_en[i] ? 1 : 0;
		if (cctl_cfg.en)
			macid_en_num++;
		ret = mac_set_hw_value(adapter, MAC_AX_HW_SET_CCTL_PRELD,
				       (void *)&cctl_cfg);
		if (ret != MACSUCCESS) {
			PLTFM_MSG_ERR("[ERR]set cctl preload macid%d fail %d\n",
				      cctl_cfg.macid, ret);
			return ret;
		}
	}

	reg = band == MAC_AX_BAND_0 ? R_AX_PTCL_PRELD_CTRL :
				      R_AX_PTCL_PRELD_CTRL_C1;
	val32 = MAC_REG_R32(reg);
	val32 = cfg->mgq_en ? val32 | B_AX_PRELD_MGQ0_EN :
			      val32 & ~B_AX_PRELD_MGQ0_EN;
	for (i = MAC_AX_PORT_0; i < MAC_AX_PORT_NUM; i++)
		val32 &= ~b_preld_hiq_port_en_l_52c[i];
	for (i = MAC_AX_P0_MBID1; i < MAC_AX_P0_MBID_LAST; i++)
		val32 &= ~b_preld_hiq_mbid_en_l_52c[i];
	if (cfg->hiq_en) {
		val32 = cfg->hiq_port == MAC_AX_PORT_0 ?
			val32 | b_preld_hiq_mbid_en_l_52c[cfg->hiq_mbid] :
			val32 | b_preld_hiq_port_en_l_52c[cfg->hiq_port];
	}
	MAC_REG_W32(reg, val32);

	grp0_num = macid_en_num * 2;
	val8 = band == MAC_AX_BAND_0 ? PRELD_B0_ENT_NUM : PRELD_B1_ENT_NUM;
	if (grp0_num > val8) {
		PLTFM_MSG_ERR("[ERR]B%d preld macid en num %d illegal\n",
			      band, macid_en_num);
		return MACFUNCINPUT;
	}
	grp1_num = val8 - grp0_num;

	reg = band == MAC_AX_BAND_0 ? R_AX_TXPKTCTL_B0_PRELD_CFG0 :
				      R_AX_TXPKTCTL_B1_PRELD_CFG0;
	val32 = MAC_REG_R32(reg);
	val32 = SET_CLR_WORD(val32, grp0_num, B_AX_B0_PRELD_CAM_G0ENTNUM);
	val32 = SET_CLR_WORD(val32, grp1_num, B_AX_B0_PRELD_CAM_G1ENTNUM);
	MAC_REG_W32(reg, val32);

	return MACSUCCESS;
}

u32 preload_init_set_8852c(struct mac_ax_adapter *adapter, enum mac_ax_band band,
			   enum mac_ax_qta_mode mode)
{
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
	u32 reg, val32, max_preld_size, min_rsvd_size;

	max_preld_size = (band == MAC_AX_BAND_0 ?
			  PRELD_B0_ENT_NUM : PRELD_B1_ENT_NUM);
	max_preld_size *= PRELD_AMSDU_SIZE;

	reg = band == MAC_AX_BAND_0 ? R_AX_TXPKTCTL_B0_PRELD_CFG0 :
				      R_AX_TXPKTCTL_B1_PRELD_CFG0;
	val32 = MAC_REG_R32(reg);
	val32 = SET_CLR_WORD(val32, max_preld_size, B_AX_B0_PRELD_USEMAXSZ);
	val32 |= B_AX_B0_PRELD_FEN;
	MAC_REG_W32(reg, val32);

	min_rsvd_size = PRELD_AMSDU_SIZE;
	reg = band == MAC_AX_BAND_0 ? R_AX_TXPKTCTL_B0_PRELD_CFG1 :
				      R_AX_TXPKTCTL_B1_PRELD_CFG1;
	val32 = MAC_REG_R32(reg);
	val32 = SET_CLR_WORD(val32, PRELD_NEXT_WND, B_AX_B0_PRELD_NXT_TXENDWIN);
	val32 = SET_CLR_WORD(val32, min_rsvd_size, B_AX_B0_PRELD_NXT_RSVMINSZ);
	MAC_REG_W32(reg, val32);

	return MACSUCCESS;
}

u32 dle_func_en_8852c(struct mac_ax_adapter *adapter, u8 en)
{
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
	u32 val32;

	val32 = MAC_REG_R32(R_AX_DMAC_FUNC_EN);
	if (en == MAC_AX_FUNC_EN)
		val32 |= (B_AX_DLE_WDE_EN | B_AX_DLE_PLE_EN);
	else if (en == MAC_AX_FUNC_DIS)
		val32 &= ~(B_AX_DLE_WDE_EN | B_AX_DLE_PLE_EN);
	else
		return MACSUCCESS;
	MAC_REG_W32(R_AX_DMAC_FUNC_EN, val32);

	return MACSUCCESS;
}

u32 dle_clk_en_8852c(struct mac_ax_adapter *adapter, u8 en)
{
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
	u32 val32;

	val32 = MAC_REG_R32(R_AX_DMAC_CLK_EN);
	if (en == MAC_AX_FUNC_EN)
		val32 |= (B_AX_DLE_WDE_CLK_EN | B_AX_DLE_PLE_CLK_EN);
	else if (en == MAC_AX_FUNC_DIS)
		val32 &= ~(B_AX_DLE_WDE_CLK_EN | B_AX_DLE_PLE_CLK_EN);
	else
		return MACSUCCESS;
	MAC_REG_W32(R_AX_DMAC_CLK_EN, val32);

	return MACSUCCESS;
}

u32 dle_mix_cfg_8852c(struct mac_ax_adapter *adapter, struct dle_mem_t *cfg)
{
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
	struct dle_size_t *size_cfg;
	u32 val32, bound;

	val32 = MAC_REG_R32(R_AX_WDE_PKTBUF_CFG);
	bound = 0;
	size_cfg = cfg->wde_size;

	switch (size_cfg->pge_size) {
	default:
	case MAC_AX_WDE_PG_64:
		val32 = SET_CLR_WORD(val32, S_AX_WDE_PAGE_SEL_64,
				     B_AX_WDE_PAGE_SEL);
		break;
	case MAC_AX_WDE_PG_128:
		val32 = SET_CLR_WORD(val32, S_AX_WDE_PAGE_SEL_128,
				     B_AX_WDE_PAGE_SEL);
		break;
	case MAC_AX_WDE_PG_256:
		PLTFM_MSG_ERR("[ERR]WDE DLE doesn't support 256 byte!\n");
		return MACHWNOSUP;
	}

	val32 = SET_CLR_WORD(val32, bound, B_AX_WDE_START_BOUND);
	val32 = SET_CLR_WORD(val32, size_cfg->lnk_pge_num,
			     B_AX_WDE_FREE_PAGE_NUM);
	MAC_REG_W32(R_AX_WDE_PKTBUF_CFG, val32);

	val32 = MAC_REG_R32(R_AX_PLE_PKTBUF_CFG);
	bound = (size_cfg->lnk_pge_num + size_cfg->unlnk_pge_num)
		* size_cfg->pge_size / DLE_BOUND_UNIT;
	size_cfg = cfg->ple_size;

	switch (size_cfg->pge_size) {
	default:
	case MAC_AX_PLE_PG_64:
		PLTFM_MSG_ERR("[ERR]PLE DLE doesn't support 64 byte!\n");
		return MACHWNOSUP;
	case MAC_AX_PLE_PG_128:
		val32 = SET_CLR_WORD(val32, S_AX_PLE_PAGE_SEL_128,
				     B_AX_PLE_PAGE_SEL);
		break;
	case MAC_AX_PLE_PG_256:
		val32 = SET_CLR_WORD(val32, S_AX_PLE_PAGE_SEL_256,
				     B_AX_PLE_PAGE_SEL);
		break;
	}

	val32 = SET_CLR_WORD(val32, bound, B_AX_PLE_START_BOUND);
	val32 = SET_CLR_WORD(val32, size_cfg->lnk_pge_num,
			     B_AX_PLE_FREE_PAGE_NUM);
	MAC_REG_W32(R_AX_PLE_PKTBUF_CFG, val32);

	return MACSUCCESS;
}

u32 wde_quota_cfg_8852c(struct mac_ax_adapter *adapter,
			struct wde_quota_t *min_cfg,
			struct wde_quota_t *max_cfg)
{
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
	u32 val32;

	val32 = SET_WORD(min_cfg->hif, B_AX_WDE_Q0_MIN_SIZE) |
		SET_WORD(max_cfg->hif, B_AX_WDE_Q0_MAX_SIZE);
	MAC_REG_W32(R_AX_WDE_QTA0_CFG, val32);

	val32 = SET_WORD(min_cfg->wcpu, B_AX_WDE_Q1_MIN_SIZE) |
		SET_WORD(max_cfg->wcpu, B_AX_WDE_Q1_MAX_SIZE);
	MAC_REG_W32(R_AX_WDE_QTA1_CFG, val32);

	val32 = SET_WORD(min_cfg->pkt_in, B_AX_WDE_Q3_MIN_SIZE) |
		SET_WORD(max_cfg->pkt_in, B_AX_WDE_Q3_MAX_SIZE);
	MAC_REG_W32(R_AX_WDE_QTA3_CFG, val32);

	val32 = SET_WORD(min_cfg->cpu_io, B_AX_WDE_Q4_MIN_SIZE) |
		SET_WORD(max_cfg->cpu_io, B_AX_WDE_Q4_MAX_SIZE);
	MAC_REG_W32(R_AX_WDE_QTA4_CFG, val32);

	return MACSUCCESS;
}

u32 ple_quota_cfg_8852c(struct mac_ax_adapter *adapter,
			struct ple_quota_t *min_cfg,
			struct ple_quota_t *max_cfg)
{
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
	u32 val32;

	val32 = SET_WORD(min_cfg->cma0_tx, B_AX_PLE_Q0_MIN_SIZE) |
		SET_WORD(max_cfg->cma0_tx, B_AX_PLE_Q0_MAX_SIZE);
	MAC_REG_W32(R_AX_PLE_QTA0_CFG, val32);

	val32 = SET_WORD(min_cfg->cma1_tx, B_AX_PLE_Q1_MIN_SIZE) |
		SET_WORD(max_cfg->cma1_tx, B_AX_PLE_Q1_MAX_SIZE);
	MAC_REG_W32(R_AX_PLE_QTA1_CFG, val32);

	val32 = SET_WORD(min_cfg->c2h, B_AX_PLE_Q2_MIN_SIZE) |
		SET_WORD(max_cfg->c2h, B_AX_PLE_Q2_MAX_SIZE);
	MAC_REG_W32(R_AX_PLE_QTA2_CFG, val32);

	val32 = SET_WORD(min_cfg->h2c, B_AX_PLE_Q3_MIN_SIZE) |
		SET_WORD(max_cfg->h2c, B_AX_PLE_Q3_MAX_SIZE);
	MAC_REG_W32(R_AX_PLE_QTA3_CFG, val32);

	val32 = SET_WORD(min_cfg->wcpu, B_AX_PLE_Q4_MIN_SIZE) |
		SET_WORD(max_cfg->wcpu, B_AX_PLE_Q4_MAX_SIZE);
	MAC_REG_W32(R_AX_PLE_QTA4_CFG, val32);

	val32 = SET_WORD(min_cfg->mpdu_proc, B_AX_PLE_Q5_MIN_SIZE) |
		SET_WORD(max_cfg->mpdu_proc, B_AX_PLE_Q5_MAX_SIZE);
	MAC_REG_W32(R_AX_PLE_QTA5_CFG, val32);

	val32 = SET_WORD(min_cfg->cma0_dma, B_AX_PLE_Q6_MIN_SIZE) |
		SET_WORD(max_cfg->cma0_dma, B_AX_PLE_Q6_MAX_SIZE);
	MAC_REG_W32(R_AX_PLE_QTA6_CFG, val32);

	val32 = SET_WORD(min_cfg->cma1_dma, B_AX_PLE_Q7_MIN_SIZE) |
		SET_WORD(max_cfg->cma1_dma, B_AX_PLE_Q7_MAX_SIZE);
	MAC_REG_W32(R_AX_PLE_QTA7_CFG, val32);

	val32 = SET_WORD(min_cfg->bb_rpt, B_AX_PLE_Q8_MIN_SIZE) |
		SET_WORD(max_cfg->bb_rpt, B_AX_PLE_Q8_MAX_SIZE);
	MAC_REG_W32(R_AX_PLE_QTA8_CFG, val32);

	val32 = SET_WORD(min_cfg->wd_rel, B_AX_PLE_Q9_MIN_SIZE) |
		SET_WORD(max_cfg->wd_rel, B_AX_PLE_Q9_MAX_SIZE);
	MAC_REG_W32(R_AX_PLE_QTA9_CFG, val32);

	val32 = SET_WORD(min_cfg->cpu_io, B_AX_PLE_Q10_MIN_SIZE) |
		SET_WORD(max_cfg->cpu_io, B_AX_PLE_Q10_MAX_SIZE);
	MAC_REG_W32(R_AX_PLE_QTA10_CFG, val32);

	val32 = SET_WORD(min_cfg->tx_rpt, B_AX_PLE_Q11_MIN_SIZE) |
		SET_WORD(max_cfg->tx_rpt, B_AX_PLE_Q11_MAX_SIZE);
	MAC_REG_W32(R_AX_PLE_QTA11_CFG, val32);

	return MACSUCCESS;
}

u32 chk_dle_rdy_8852c(struct mac_ax_adapter *adapter, enum DLE_CTRL_TYPE type)
{
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
	u32 val32 = 0, reg, rdy_bit, cnt;

	if (type == DLE_CTRL_TYPE_WDE) {
		reg = R_AX_WDE_INI_STATUS;
		rdy_bit = B_AX_WDE_Q_MGN_INI_RDY | B_AX_WDE_BUF_MGN_INI_RDY;
	} else if (type == DLE_CTRL_TYPE_PLE) {
		reg = R_AX_PLE_INI_STATUS;
		rdy_bit = B_AX_PLE_Q_MGN_INI_RDY | B_AX_PLE_BUF_MGN_INI_RDY;
	} else {
		return MACFUNCINPUT;
	}

	cnt = DLE_WAIT_CNT;
	while (cnt--) {
		val32 = MAC_REG_R32(reg);
		if ((val32 & rdy_bit) == rdy_bit)
			break;
		PLTFM_DELAY_US(DLE_WAIT_US);
	}

	if (!++cnt) {
		PLTFM_MSG_ERR("[ERR]DLE%d cfg ready timeout 0x%X\n", type, val32);
		return MACPOLLTO;
	}

	return MACSUCCESS;
}
#endif

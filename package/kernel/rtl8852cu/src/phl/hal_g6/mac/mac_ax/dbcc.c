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

#include "dbcc.h"
#include "mac_priv.h"
#include "cpuio.h"

u32 dbcc_info_init(struct mac_ax_adapter *adapter)
{
	u32 ret = MACSUCCESS;

	adapter->dbcc_info =
		(struct mac_ax_dbcc_info *)PLTFM_MALLOC(DBCC_INFO_SIZE);
	if (!adapter->dbcc_info) {
		PLTFM_MSG_ERR("dbcc info malloc fail\n");
		return MACNPTR;
	}

	adapter->dbcc_info->dbcc_wmm_list = (u8 *)PLTFM_MALLOC(DBCC_WMM_LIST_SIZE);
	if (!adapter->dbcc_info->dbcc_wmm_list) {
		PLTFM_MSG_ERR("dbcc wmm list malloc fail\n");
		return MACNPTR;
	}

	ret = rst_dbcc_info(adapter);

	return ret;
}

u32 dbcc_info_exit(struct mac_ax_adapter *adapter)
{
	PLTFM_FREE(adapter->dbcc_info->dbcc_wmm_list, DBCC_WMM_LIST_SIZE);
	PLTFM_FREE(adapter->dbcc_info, DBCC_INFO_SIZE);

	return MACSUCCESS;
}

u32 rst_dbcc_info(struct mac_ax_adapter *adapter)
{
	struct mac_ax_dbcc_info *dbcc_info = adapter->dbcc_info;
	u32 i;

	if (!dbcc_info) {
		PLTFM_MSG_ERR("no dbcc info when rst dbcc info\n");
		return MACNPTR;
	}

	PLTFM_MEMSET(dbcc_info->dbcc_wmm_list, MAC_AX_DBCC_WMM_INVALID,
		     DBCC_WMM_LIST_SIZE);
	for (i = 0; i < MAC_AX_DBCC_WMM_MAX; i++) {
		dbcc_info->dbcc_role_cnt[i] = 0;
		dbcc_info->dbcc_wmm_type[i] = MAC_AX_NET_TYPE_INVLAID;
		dbcc_info->dbcc_wmm_bp[i] = DBCC_BP_INVALID;
	}

	for (i = MAC_AX_BAND_0; i < MAC_AX_BAND_NUM; i++) {
		dbcc_info->bkp_flag[i] = 0;
		PLTFM_MEMSET(&dbcc_info->ppdu_rpt_bkp, 0,
			     sizeof(struct mac_ax_phy_rpt_cfg));
		PLTFM_MEMSET(&dbcc_info->chinfo_bkp, 0,
			     sizeof(struct mac_ax_phy_rpt_cfg));
	}

	dbcc_info->notify_fw_flag = 0;

	return MACSUCCESS;
}

static u32 get_valid_dbcc_wmm(struct mac_ax_adapter *adapter, u8 *out_dbcc_wmm)
{
	struct mac_ax_dbcc_info *dbcc_info = adapter->dbcc_info;
	u8 wmmidx;

	if (!dbcc_info) {
		PLTFM_MSG_ERR("no dbcc info when get valid dbcc wmm\n");
		*out_dbcc_wmm = MAC_AX_DBCC_WMM_INVALID;
		return MACNPTR;
	}

	for (wmmidx = MAC_AX_DBCC_WMM0; wmmidx < MAC_AX_DBCC_WMM_MAX; wmmidx++) {
		if (!dbcc_info->dbcc_role_cnt[wmmidx])
			break;
	}

	if (wmmidx == MAC_AX_DBCC_WMM_MAX) {
		PLTFM_MSG_ERR("no valid dbcc wmm\n");
		*out_dbcc_wmm = MAC_AX_DBCC_WMM_INVALID;
		return MACNOITEM;
	}

	*out_dbcc_wmm = wmmidx;
	return MACSUCCESS;
}

u32 dbcc_wmm_add_macid(struct mac_ax_adapter *adapter,
		       struct mac_ax_role_info *info)
{
	struct mac_ax_dbcc_info *dbcc_info = adapter->dbcc_info;
	enum mac_ax_ss_wmm_tbl dst_link;
	u8 *cur_dbcc_wmm_type;
	u32 ret;
	u8 chk_emp, tar_dbcc_wmm, wmmidx, curr_bp;

	if (!dbcc_info) {
		PLTFM_MSG_ERR("no dbcc info when add macid\n");
		return MACNPTR;
	}

	curr_bp = (info->port & DBCC_PORT_MASK) |
		  (info->band == MAC_AX_BAND_1 ? DBCC_BAND_BIT : 0);
	tar_dbcc_wmm = MAC_AX_DBCC_WMM_INVALID;

	if (info->net_type == MAC_AX_NET_TYPE_AP) {
		for (wmmidx = MAC_AX_DBCC_WMM0; wmmidx < MAC_AX_DBCC_WMM_MAX; wmmidx++) {
			if (dbcc_info->dbcc_wmm_bp[wmmidx] == curr_bp &&
			    dbcc_info->dbcc_wmm_type[wmmidx] == info->net_type)
				break;
		}

		if (wmmidx < MAC_AX_DBCC_WMM_MAX)
			tar_dbcc_wmm = wmmidx;
	}

	if (tar_dbcc_wmm == MAC_AX_DBCC_WMM_INVALID) {
		ret = get_valid_dbcc_wmm(adapter, &tar_dbcc_wmm);
		if (ret != MACSUCCESS) {
			PLTFM_MSG_ERR("get_valid_dbcc_wmm %d\n", ret);
			return ret;
		}
	}

	cur_dbcc_wmm_type = &dbcc_info->dbcc_wmm_type[tar_dbcc_wmm];
	if ((info->net_type != MAC_AX_NET_TYPE_AP &&
	     *cur_dbcc_wmm_type != MAC_AX_NET_TYPE_INVLAID) ||
	    (info->net_type == MAC_AX_NET_TYPE_AP &&
	     *cur_dbcc_wmm_type != MAC_AX_NET_TYPE_INVLAID &&
	     *cur_dbcc_wmm_type != MAC_AX_NET_TYPE_AP)) {
		PLTFM_MSG_ERR("dbcc wmm%d is occupied by type%d\n",
			      tar_dbcc_wmm, *cur_dbcc_wmm_type);
		return MACPROCERR;
	}

	if (info->macid >= DBCC_WMM_LIST_SIZE) {
		PLTFM_MSG_ERR("macid%d not support in dbcc wmm add macid\n",
			      info->macid);
		return MACFUNCINPUT;
	}

	if (info->band == MAC_AX_BAND_0)
		dst_link = info->wmm ?
			   MAC_AX_SS_WMM_TBL_C0_WMM1 : MAC_AX_SS_WMM_TBL_C0_WMM0;
	else
		dst_link = info->wmm ?
			   MAC_AX_SS_WMM_TBL_C1_WMM1 : MAC_AX_SS_WMM_TBL_C1_WMM0;
	chk_emp = (!dbcc_info->dbcc_role_cnt[tar_dbcc_wmm] ||
		   *cur_dbcc_wmm_type != MAC_AX_NET_TYPE_AP) ? 1 : 0;
	ret = mac_ss_wmm_map_upd(adapter, (enum mac_ax_ss_wmm)tar_dbcc_wmm,
				 dst_link, chk_emp);
	if (ret != MACSUCCESS) {
		PLTFM_MSG_ERR("mac_ss_wmm_map_upd %d/%d/%d/%d\n",
			      tar_dbcc_wmm, dst_link, chk_emp, ret);
		return ret;
	}

	*(dbcc_info->dbcc_wmm_list + info->macid) = tar_dbcc_wmm;
	dbcc_info->dbcc_role_cnt[tar_dbcc_wmm]++;
	*cur_dbcc_wmm_type = info->net_type;
	dbcc_info->dbcc_wmm_bp[tar_dbcc_wmm] = curr_bp;

	return MACSUCCESS;
}

u32 dbcc_wmm_rm_macid(struct mac_ax_adapter *adapter,
		      struct mac_ax_role_info *info)
{
	struct mac_ax_dbcc_info *dbcc_info = adapter->dbcc_info;
	u8 dbcc_wmm;

	if (!dbcc_info) {
		PLTFM_MSG_ERR("no dbcc info when rm macid\n");
		return MACNPTR;
	}

	if (info->macid >= DBCC_WMM_LIST_SIZE) {
		PLTFM_MSG_ERR("macid%d not support in dbcc wmm rm macid\n",
			      info->macid);
		return MACFUNCINPUT;
	}

	dbcc_wmm = *(dbcc_info->dbcc_wmm_list + info->macid);
	if (dbcc_wmm == MAC_AX_DBCC_WMM_INVALID) {
		PLTFM_MSG_ERR("macid%d dbcc wmm invalid in dbcc wmm list\n",
			      info->macid);
		return MACPROCERR;
	}

	if (!dbcc_info->dbcc_role_cnt[dbcc_wmm]) {
		PLTFM_MSG_ERR("dbcc wmm%d rm macid%d underflow\n",
			      dbcc_wmm, info->macid);
		if (dbcc_info->dbcc_wmm_type[dbcc_wmm] !=
		    MAC_AX_NET_TYPE_INVLAID) {
			PLTFM_MSG_ERR("dbcc wmm not invalid when no role\n");
			return MACPROCERR;
		}
		return MACCMP;
	}

	*(dbcc_info->dbcc_wmm_list + info->macid) = MAC_AX_DBCC_WMM_INVALID;
	dbcc_info->dbcc_role_cnt[dbcc_wmm]--;
	if (!dbcc_info->dbcc_role_cnt[dbcc_wmm]) {
		dbcc_info->dbcc_wmm_type[dbcc_wmm] = MAC_AX_NET_TYPE_INVLAID;
		dbcc_info->dbcc_wmm_bp[dbcc_wmm] = DBCC_BP_INVALID;
	}

	return MACSUCCESS;
}

static u32 band1_enable(struct mac_ax_adapter *adapter,
			struct mac_ax_trx_info *info)
{
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
	struct mac_ax_priv_ops *p_ops = adapter_to_priv_ops(adapter);
	u32 ret;

	ret = tx_idle_poll_band(adapter, 0);
	if (ret != MACSUCCESS) {
		PLTFM_MSG_ERR("[ERR]tx idle poll %d\n", ret);
		return ret;
	}

	ret = dle_quota_change(adapter, info->qta_mode);
	if (ret != MACSUCCESS) {
		PLTFM_MSG_ERR("[ERR]DLE quota change %d\n", ret);
		return ret;
	}

	ret = preload_init(adapter, MAC_AX_BAND_1, info->qta_mode);
	if (ret != MACSUCCESS) {
		PLTFM_MSG_ERR("[ERR]preload init B1 %d\n", ret);
		return ret;
	}

	ret = p_ops->cmac_func_en(adapter, MAC_AX_BAND_1, MAC_AX_FUNC_EN);
	if (ret != MACSUCCESS) {
		PLTFM_MSG_ERR("[ERR]CMAC%d func en %d\n", MAC_AX_BAND_1, ret);
		return ret;
	}

	ret = p_ops->cmac_init(adapter, info, MAC_AX_BAND_1);
	if (ret != MACSUCCESS) {
		PLTFM_MSG_ERR("[ERR]CMAC%d init %d\n", MAC_AX_BAND_1, ret);
		return ret;
	}

	MAC_REG_W32(R_AX_SYS_ISO_CTRL_EXTEND,
		    MAC_REG_R32(R_AX_SYS_ISO_CTRL_EXTEND) |
		    B_AX_R_SYM_FEN_WLBBGLB_1 | B_AX_R_SYM_FEN_WLBBFUN_1);

	adapter->sm.bb1_func = MAC_AX_FUNC_ON;

	ret = mac_enable_imr(adapter, MAC_AX_BAND_1, MAC_AX_CMAC_SEL);
	if (ret != MACSUCCESS) {
		PLTFM_MSG_ERR("[ERR] enable CMAC1 IMR %d\n", ret);
		return ret;
	}

	return MACSUCCESS;
}

static u32 band1_disable(struct mac_ax_adapter *adapter,
			 struct mac_ax_trx_info *info)
{
	u32 ret;
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
	struct mac_ax_priv_ops *p_ops = adapter_to_priv_ops(adapter);

	MAC_REG_W32(R_AX_SYS_ISO_CTRL_EXTEND,
		    MAC_REG_R32(R_AX_SYS_ISO_CTRL_EXTEND) &
		    ~B_AX_R_SYM_FEN_WLBBFUN_1);

	ret = p_ops->cmac_func_en(adapter, MAC_AX_BAND_1, MAC_AX_FUNC_DIS);
	if (ret != MACSUCCESS) {
		PLTFM_MSG_ERR("[ERR]CMAC%d func dis %d\n", MAC_AX_BAND_1, ret);
		return ret;
	}

	ret = dle_quota_change(adapter, info->qta_mode);
	if (ret != MACSUCCESS) {
		PLTFM_MSG_ERR("[ERR]DLE quota change %d\n", ret);
		return ret;
	}

	ret = rst_port_info(adapter, MAC_AX_BAND_1);
	if (ret != MACSUCCESS) {
		PLTFM_MSG_ERR("[ERR]Reset band 1 port info %d\n", ret);
		return ret;
	}

	return 0;
}

static u32 dbcc_chk_notify_done(struct mac_ax_adapter *adapter)
{
	struct mac_ax_dbcc_info *dbcc_info = adapter->dbcc_info;
	u32 cnt = DBCC_CHK_NTFY_CNT;

	while (cnt) {
		if (!dbcc_info->notify_fw_flag)
			break;
		PLTFM_SLEEP_US(DBCC_CHK_NTFY_DLY);
		cnt--;
	}

	if (!cnt)
		return MACPOLLTO;

	PLTFM_MSG_ALWAYS("[TRACE]DBCC check notify cnt %d\n", DBCC_CHK_NTFY_CNT - cnt);
	return MACSUCCESS;
}

u32 dbcc_trx_ctrl_bkp(struct mac_ax_adapter *adapter, enum mac_ax_band band)
{
	struct mac_ax_ops *mops = adapter_to_mac_ops(adapter);
	struct mac_ax_dbcc_info *dbcc_info = adapter->dbcc_info;
	struct mac_ax_phy_rpt_cfg *ppdu_rpt_cfg;
	struct mac_ax_phy_rpt_cfg *chinfo_cfg;
	u32 ret;

	if (!dbcc_info) {
		PLTFM_MSG_ERR("no dbcc info when bkp trx\n");
		return MACNPTR;
	}

	if (dbcc_info->bkp_flag[band]) {
		PLTFM_MSG_ERR("band%d trx ctrl already backup\n", band);
		return MACPROCERR;
	}

	ppdu_rpt_cfg = &dbcc_info->ppdu_rpt_bkp[band];
	ppdu_rpt_cfg->type = MAC_AX_PPDU_STATUS;
	ppdu_rpt_cfg->u.ppdu.band = band;
	ret = mops->get_phy_rpt_cfg(adapter, ppdu_rpt_cfg);
	if (ret != MACSUCCESS) {
		PLTFM_MSG_ERR("get ppdu stus rpt cfg %d\n", ret);
		return ret;
	}

	chinfo_cfg = &dbcc_info->chinfo_bkp[band];
	chinfo_cfg->type = MAC_AX_CH_INFO;
	ret = mops->get_phy_rpt_cfg(adapter, chinfo_cfg);
	if (ret != MACSUCCESS) {
		PLTFM_MSG_ERR("get ch info cfg %d\n", ret);
		return ret;
	}

	dbcc_info->bkp_flag[band] = 1;
	return ret;
}

u32 mac_dbcc_trx_ctrl(struct mac_ax_adapter *adapter,
		      enum mac_ax_band band, u8 pause)
{
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
	struct mac_ax_ops *mops = adapter_to_mac_ops(adapter);
	struct mac_ax_dbcc_info *dbcc_info = adapter->dbcc_info;
	struct mac_ax_sch_tx_en_cfg sch_cfg;
	struct mac_ax_phy_rpt_cfg phyrpt_cfg;
	struct mac_ax_phy_rpt_cfg *ppdu_rpt_bkp;
	struct mac_ax_phy_rpt_cfg *chinfo_cfg_kbp;
	u32 ret, reg;
	u16 val16;

	if (!dbcc_info) {
		PLTFM_MSG_ERR("no dbcc info when trx ctrl\n");
		return MACNPTR;
	}

	if (pause) {
		ret = dbcc_trx_ctrl_bkp(adapter, band);
		if (ret != MACSUCCESS) {
			PLTFM_MSG_ERR("B%d dbcc trx bkp %d\n", band, ret);
			return ret;
		}
	} else if (!dbcc_info->bkp_flag[band]) {
		PLTFM_MSG_ERR("B%d dbcc trx is not backup\n", band);
		return MACPROCERR;
	}

	if (!pause && band == MAC_AX_BAND_1 &&
	    check_mac_en(adapter, MAC_AX_BAND_1, MAC_AX_CMAC_SEL) != MACSUCCESS) {
		dbcc_info->bkp_flag[band] = 0;
		return MACSUCCESS;
	}

	ppdu_rpt_bkp = &dbcc_info->ppdu_rpt_bkp[band];
	chinfo_cfg_kbp = &dbcc_info->chinfo_bkp[band];

	sch_cfg.band = (u8)band;
	if (pause)
		PLTFM_MEMSET(&sch_cfg.tx_en, 0, SCH_TX_EN_SIZE);
	else
		PLTFM_MEMSET(&sch_cfg.tx_en, 0xFF, SCH_TX_EN_SIZE);
	PLTFM_MEMSET(&sch_cfg.tx_en_mask, 0xFF, SCH_TX_EN_SIZE);
	ret = mops->set_hw_value(adapter, MAC_AX_HW_SET_SCH_TXEN_CFG,
				 (void *)&sch_cfg);
	if (ret != MACSUCCESS) {
		PLTFM_MSG_ERR("B%d pause%d sch txen cfg %d\n", band, pause, ret);
		return ret;
	}

	reg = band == MAC_AX_BAND_0 ? R_AX_RCR : R_AX_RCR_C1;
	val16 = MAC_REG_R16(reg);
	if (pause)
		MAC_REG_W16(reg, val16 | B_AX_STOP_RX_IN);
	else
		MAC_REG_W16(reg, val16 & ~B_AX_STOP_RX_IN);

	phyrpt_cfg.type = MAC_AX_PPDU_STATUS;
	phyrpt_cfg.en = pause ? 0 : ppdu_rpt_bkp->en;
	phyrpt_cfg.dest = ppdu_rpt_bkp->dest;
	phyrpt_cfg.u.ppdu = ppdu_rpt_bkp->u.ppdu;
	ret = mops->cfg_phy_rpt(adapter, &phyrpt_cfg);
	if (ret != MACSUCCESS) {
		PLTFM_MSG_ERR("B%d pause%d ppdu status cfg %d\n", band, pause, ret);
		return ret;
	}

	phyrpt_cfg.type = MAC_AX_CH_INFO;
	phyrpt_cfg.en = pause ? 0 : chinfo_cfg_kbp->en;
	phyrpt_cfg.dest = chinfo_cfg_kbp->dest;
	phyrpt_cfg.u.chif = chinfo_cfg_kbp->u.chif;
	ret = mops->cfg_phy_rpt(adapter, &phyrpt_cfg);
	if (ret != MACSUCCESS) {
		PLTFM_MSG_ERR("B%d pause%d ch info cfg %d\n", band, pause, ret);
		return ret;
	}

	if (!pause)
		dbcc_info->bkp_flag[band] = 0;

	return ret;
}

u32 mac_dbcc_enable(struct mac_ax_adapter *adapter,
		    struct mac_ax_trx_info *info, u8 dbcc_en)
{
	u32 ret;

	if (dbcc_en) {
		ret = band1_enable(adapter, info);
		if (ret != MACSUCCESS) {
			PLTFM_MSG_ERR("[ERR] band1_enable %d\n", ret);
			return ret;
		}
		if (adapter->sm.fwdl == MAC_AX_FWDL_INIT_RDY) {
			ret = mac_notify_fw_dbcc(adapter, 1);
			if (ret != MACSUCCESS) {
				PLTFM_MSG_ERR("%s:[ERR]notfify dbcc1 fail %d\n",
					      __func__, ret);
				return ret;
			}
		} else {
			PLTFM_MSG_WARN("%s fw not ready\n", __func__);
		}
	} else {
		if (adapter->sm.fwdl == MAC_AX_FWDL_INIT_RDY) {
			ret = mac_notify_fw_dbcc(adapter, 0);
			if (ret != MACSUCCESS) {
				PLTFM_MSG_ERR("%s:[ERR]notfify dbcc0 fail %d\n",
					      __func__, ret);
				return ret;
			}
		} else {
			PLTFM_MSG_WARN("%s fw not ready\n", __func__);
		}
		ret = band1_disable(adapter, info);
		if (ret != MACSUCCESS) {
			PLTFM_MSG_ERR("[ERR] band1_disable %d\n", ret);
			return ret;
		}
	}

	return MACSUCCESS;
}

u32 mac_dbcc_pre_cfg(struct mac_ax_adapter *adapter, struct mac_dbcc_cfg_info *info)
{
	struct mac_ax_dbcc_info *dbcc_info = adapter->dbcc_info;
	struct mac_ax_trx_info trx_info;
	u32 ret = MACSUCCESS;
	u8 notify_en;

	trx_info.trx_mode = info->trx_mode;
	trx_info.qta_mode = info->qta_mode;
	trx_info.rpr_cfg = NULL;

	if (info->dbcc_en) {
		notify_en = 1;
		ret = band1_enable(adapter, &trx_info);
		if (ret != MACSUCCESS) {
			PLTFM_MSG_ERR("[ERR] band1_enable %d\n", ret);
			return ret;
		}
		if (adapter->sm.fwdl == MAC_AX_FWDL_INIT_RDY) {
			if (dbcc_info->notify_fw_flag) {
				PLTFM_MSG_WARN("[WARN]Notify FW dbcc %d flag already set\n",
					       notify_en);
			}
			dbcc_info->notify_fw_flag = 1;
			ret = mac_notify_fw_dbcc(adapter, notify_en);
			if (ret != MACSUCCESS) {
				PLTFM_MSG_ERR("%s:[ERR]Notify dbcc %d fail %d\n",
					      __func__, notify_en, ret);
				return ret;
			}
		} else {
			PLTFM_MSG_WARN("%s en %d fw not ready\n", __func__, info->dbcc_en);
		}
	} else {
		notify_en = 0;
		if (adapter->sm.fwdl == MAC_AX_FWDL_INIT_RDY) {
			if (dbcc_info->notify_fw_flag) {
				PLTFM_MSG_WARN("[WARN]Notify FW dbcc %d flag already set\n",
					       notify_en);
			}
			dbcc_info->notify_fw_flag = 1;
			ret = mac_notify_fw_dbcc(adapter, notify_en);
			if (ret != MACSUCCESS) {
				PLTFM_MSG_ERR("%s:[ERR]Notify dbcc %d fail %d\n",
					      __func__, notify_en, ret);
				return ret;
			}
		} else {
			PLTFM_MSG_WARN("%s en %d fw not ready\n", __func__, info->dbcc_en);
		}
	}

	return ret;
}

u32 mac_dbcc_cfg(struct mac_ax_adapter *adapter, struct mac_dbcc_cfg_info *info)
{
	struct mac_ax_trx_info trx_info;
	u32 ret = MACSUCCESS;

	trx_info.trx_mode = info->trx_mode;
	trx_info.qta_mode = info->qta_mode;
	trx_info.rpr_cfg = NULL;

	if (info->dbcc_en) {
		ret = dbcc_chk_notify_done(adapter);
		if (ret != MACSUCCESS) {
			PLTFM_MSG_ERR("[ERR]Check dbcc notify %d fail %d\n",
				      info->dbcc_en, ret);
			return ret;
		}
	} else {
		ret = dbcc_chk_notify_done(adapter);
		if (ret != MACSUCCESS) {
			PLTFM_MSG_ERR("[ERR]Check dbcc notify %d fail %d\n",
				      info->dbcc_en, ret);
			return ret;
		}

		ret = band1_disable(adapter, &trx_info);
		if (ret != MACSUCCESS) {
			PLTFM_MSG_ERR("[ERR] band1_disable %d\n", ret);
			return ret;
		}
	}

	return ret;
}

u32 mac_dbcc_move_wmm(struct mac_ax_adapter *adapter,
		      struct mac_ax_role_info *info)
{
	struct mac_ax_dbcc_info *dbcc_info = adapter->dbcc_info;
	enum mac_ax_ss_wmm_tbl dst_link;
	u8 dbcc_wmm;
	u8 role_cnt;
	u8 wmm_type;
	u8 dbcc_en = info->band;
	u32 ret;

	if (!dbcc_info) {
		PLTFM_MSG_ERR("no dbcc info when move wmm\n");
		return MACNPTR;
	}

	dbcc_wmm = *(dbcc_info->dbcc_wmm_list + info->macid);
	role_cnt = dbcc_info->dbcc_role_cnt[dbcc_wmm];
	wmm_type = dbcc_info->dbcc_wmm_type[dbcc_wmm];

	if (!role_cnt) {
		PLTFM_MSG_ERR("dbcc en%d move wmm%d role cnt 0\n",
			      dbcc_en, dbcc_wmm);
		return MACCMP;
	}

	dst_link = dbcc_en ?
		   MAC_AX_SS_WMM_TBL_C1_WMM0 : MAC_AX_SS_WMM_TBL_C0_WMM0;
	if (dbcc_en == MAC_AX_BAND_0)
		dst_link = info->wmm ?
			   MAC_AX_SS_WMM_TBL_C0_WMM1 : MAC_AX_SS_WMM_TBL_C0_WMM0;
	else
		dst_link = info->wmm ?
			   MAC_AX_SS_WMM_TBL_C1_WMM1 : MAC_AX_SS_WMM_TBL_C1_WMM0;
	ret = mac_ss_wmm_sta_move(adapter,
				  (enum mac_ax_ss_wmm)dbcc_wmm,
				  dst_link);
	if (ret == MACARDYDONE) {
		if (wmm_type != MAC_AX_NET_TYPE_AP)
			PLTFM_MSG_WARN("dbcc en%d move wmm%d is already mapped\n",
				       dbcc_en, dbcc_wmm);
		return MACSUCCESS;
	} else if (ret != MACSUCCESS) {
		PLTFM_MSG_ERR("dbcc en%d move wmm%d sta %d\n",
			      dbcc_en, dbcc_wmm, ret);
		return ret;
	}

	return MACSUCCESS;
}


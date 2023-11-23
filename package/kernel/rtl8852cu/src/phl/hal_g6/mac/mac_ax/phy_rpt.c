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
#include "phy_rpt.h"
#include "mac_priv.h"

#define MAC_AX_RX_CNT_SIZE 96
#define MAC_AX_DISP_QID_HOST 0x2
#define MAC_AX_DISP_QID_WLCPU 0xB
#define MAC_AX_DFS_HDR_SIZE 8
#define MAC_AX_DFS_RPT_SIZE 8
#define MAC_AX_DFS_RPT_SIZE_SH 3
#define MAC_AX_CH_INFO_BUF 0
#define B_AX_CH_INFO_BUF_128 0
#define B_AX_GET_CH_INFO_TO_DIS 0
#define B_AX_GET_CH_INFO_TO_8 2
#define B_AX_GET_CH_INFO_TO_28 7
#define B_AX_CH_INFO_INTVL_DIS 0
#define B_AX_CH_INFO_INTVL_1 1
#define B_AX_CH_INFO_INTVL_2 2
#define B_AX_CH_INFO_INTVL_4 4
#define B_AX_CH_INFO_INTVL_7 7
#define B_AX_CH_INFO_REQ_2 1
#define B_AX_DFS_BUF_64 1

#define MAC_AX_MAC_INFO_USE_SIZE 4
struct mac_ax_mac_info_t {
	u32 dword0;
	u32 dword1;
};

struct mac_ax_dfs_hdr_t {
	u32 dword0;
	u32 dword1;
};

static u32 _patch_is_cfg_avl(struct mac_ax_adapter *adapter,
			     struct mac_ax_phy_rpt_cfg *cfg,
			     struct mac_ax_ppdu_stat *ppdu)
{
	if (cfg->dest == MAC_AX_PRPT_DEST_HOST && ppdu->dup2fw_en &&
	    ppdu->dup2fw_len != 0) {
		if (chk_patch_is_cfg_avl(adapter))
			return MACFUNCINPUT;
	}

	return MACSUCCESS;
}

static u32 get_ppdu_status_cfg(struct mac_ax_adapter *adapter,
			       struct mac_ax_phy_rpt_cfg *cfg)
{
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
	struct mac_ax_ppdu_stat *ppdu = &cfg->u.ppdu;
	u32 reg = (ppdu->band) ? R_AX_PPDU_STAT_C1 : R_AX_PPDU_STAT;
	u32 val, tmp;
	u32 ret = 0;

	ret = check_mac_en(adapter, ppdu->band, MAC_AX_CMAC_SEL);
	if (ret) {
		PLTFM_MSG_ERR("MAC%d is not ready\n", ppdu->band);
		return ret;
	}

	val = MAC_REG_R32(R_AX_HW_RPT_FWD);
	tmp = GET_FIELD(val, B_AX_FWD_PPDU_STAT);
	cfg->dest = (tmp == MAC_AX_FWD_TO_HOST) ? MAC_AX_PRPT_DEST_HOST :
			MAC_AX_PRPT_DEST_WLCPU;

	val = MAC_REG_R32(R_AX_RX_PPDU_STATUS_FW_MODE);
	ppdu->dup2fw_en = !!(val & B_AX_HDR_PPDU_ENQ_WLCPU_EN);
	ppdu->dup2fw_len = GET_FIELD(val, B_AX_CDR_PPDU_2_WLCPU_LEN);

	val = MAC_REG_R32(reg);
	ppdu->bmp_append_info = val & (MAC_AX_PPDU_MAC_INFO |
				       MAC_AX_PPDU_PLCP |
				       MAC_AX_PPDU_RX_CNT);
	ppdu->bmp_filter = val & (MAC_AX_PPDU_HAS_A1M |
				  MAC_AX_PPDU_HAS_CRC_OK |
				  MAC_AX_PPDU_HAS_DMA_OK);
	cfg->en = !!(val & B_AX_PPDU_STAT_RPT_EN);

	return MACSUCCESS;
}

static u32 cfg_ppdu_status(struct mac_ax_adapter *adapter,
			   struct mac_ax_phy_rpt_cfg *cfg)
{
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
	struct mac_ax_ppdu_stat *ppdu = &cfg->u.ppdu;
	u32 reg = (ppdu->band) ? R_AX_PPDU_STAT_C1 : R_AX_PPDU_STAT;
	u32 val;
	u32 ret = 0;

	ret = check_mac_en(adapter, ppdu->band, MAC_AX_CMAC_SEL);
	if (ret) {
		PLTFM_MSG_ERR("MAC%d is not ready\n", ppdu->band);
		goto END;
	}

	ret = _patch_is_cfg_avl(adapter, cfg, ppdu);
	if (ret) {
		PLTFM_MSG_ERR("The PPDU status config is INVALID\n");
		goto END;
	}

	if (!cfg->en) {
#if MAC_AX_FW_REG_OFLD
		if (adapter->sm.fwdl == MAC_AX_FWDL_INIT_RDY) {
			ret = MAC_REG_W_OFLD((u16)reg,
					     B_AX_PPDU_STAT_RPT_EN,
					     0, 1);
			if (ret != MACSUCCESS)
				PLTFM_MSG_ERR("%s: write offload fail %d",
					      __func__, ret);
			goto END;
		}
#endif
		val = MAC_REG_R32(reg);
		val = val & ~B_AX_PPDU_STAT_RPT_EN;
		MAC_REG_W32(reg, val);
		goto END;
	}

	val = MAC_REG_R32(R_AX_HW_RPT_FWD);
	switch (cfg->dest) {
	case MAC_AX_PRPT_DEST_HOST:
		if (ppdu->dup2fw_en) {
			MAC_REG_W32(R_AX_RX_PPDU_STATUS_FW_MODE,
				    B_AX_HDR_PPDU_ENQ_WLCPU_EN |
				    SET_WORD(ppdu->dup2fw_len,
					     B_AX_CDR_PPDU_2_WLCPU_LEN));
		}

		val = SET_CLR_WORD(val, MAC_AX_FWD_TO_HOST, B_AX_FWD_PPDU_STAT);
		break;
	case MAC_AX_PRPT_DEST_WLCPU:
		val = SET_CLR_WORD(val, MAC_AX_FWD_TO_WLAN_CPU,
				   B_AX_FWD_PPDU_STAT);
		break;
	default:
		PLTFM_MSG_ERR("Wrong PPDU status destination\n");
		ret = MACFUNCINPUT;
	}
	MAC_REG_W32(R_AX_HW_RPT_FWD, val);

	ppdu->bmp_append_info = ppdu->bmp_append_info &
				(MAC_AX_PPDU_MAC_INFO |
				 MAC_AX_PPDU_PLCP |
				 MAC_AX_PPDU_RX_CNT);
	ppdu->bmp_filter = ppdu->bmp_filter &
			   (MAC_AX_PPDU_HAS_A1M |
			    MAC_AX_PPDU_HAS_CRC_OK |
			    MAC_AX_PPDU_HAS_DMA_OK);

	val = B_AX_PPDU_STAT_RPT_EN |
		ppdu->bmp_filter |
		ppdu->bmp_append_info;

#if MAC_AX_FW_REG_OFLD
	if (adapter->sm.fwdl == MAC_AX_FWDL_INIT_RDY) {
		ret = MAC_REG_W32_OFLD((u16)reg, val, 1);
		if (ret != MACSUCCESS)
			PLTFM_MSG_ERR("%s: write offload fail %d",
				      __func__, ret);
		goto END;
	}
#endif

	MAC_REG_W32(reg, val);

END:
	return ret;
}

static u32 en_bbrpt(struct mac_ax_adapter *adapter)
{
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
	u32 val;

	val = MAC_REG_R32(R_AX_DMAC_CLK_EN);
	if (!(val & B_AX_BBRPT_CLK_EN)) {
		val = val | B_AX_BBRPT_CLK_EN;
		MAC_REG_W32(R_AX_DMAC_CLK_EN, val);
	}

	val = MAC_REG_R32(R_AX_DMAC_FUNC_EN);
	if (!(val & B_AX_BBRPT_EN)) {
		val = val | B_AX_BBRPT_EN;
		MAC_REG_W32(R_AX_DMAC_FUNC_EN, val);
	}

	return MACSUCCESS;
}

static u32 stop_ch_info(struct mac_ax_adapter *adapter, u32 ch_info_reg)
{
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
	u8 val;
	u32 count = 3000;
#if MAC_AX_FW_REG_OFLD
	u32 ret;
#endif

	val = MAC_REG_R8(R_AX_CH_INFO);

	if (!(val & B_AX_CH_INFO_EN))
		return MACSUCCESS;

	MAC_REG_W8(R_AX_CH_INFO, val | B_AX_CH_INFO_STOP_REQ);
	while (!(MAC_REG_R8(R_AX_CH_INFO) & B_AX_CH_INFO_STOP)) {
		count--;
		if (count == 0) {
			PLTFM_MSG_ERR("Polling ch info idle timeout\n");
			return MACPOLLTO;
		}
	}

	val = MAC_REG_R8(R_AX_CH_INFO);
	MAC_REG_W8(R_AX_CH_INFO, val | B_AX_CH_INFO_EN);

#if MAC_AX_FW_REG_OFLD
	if (adapter->sm.fwdl == MAC_AX_FWDL_INIT_RDY) {
		ret = MAC_REG_W_OFLD((u16)ch_info_reg, B_AX_GET_CH_INFO_EN, 0, 1);
		if (ret != MACSUCCESS)
			PLTFM_MSG_ERR("%s: write offload fail %d",
				      __func__, ret);
		return ret;
	}
#endif

	val = MAC_REG_R8(ch_info_reg);
	MAC_REG_W8(ch_info_reg, val & ~B_AX_GET_CH_INFO_EN);

	return MACSUCCESS;
}

static u32 get_ch_info_cfg(struct mac_ax_adapter *adapter,
			   struct mac_ax_phy_rpt_cfg *cfg)
{
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
	struct mac_ax_priv_ops *p_ops = adapter_to_priv_ops(adapter);
	u32 reg, p_id, q_id;
	u32 val;
	u32 ret = 0;
	struct mac_ax_ch_info *chif = &cfg->u.chif;
	u8 band;

	PLTFM_MEMSET(cfg, 0, sizeof(*cfg));

	val = MAC_REG_R32(R_AX_CH_INFO);

	cfg->en = !!(val & B_AX_CH_INFO_EN);
	if (!cfg->en)
		return MACSUCCESS;

	band = !!(val & B_AX_CH_INFO_PHY);

	ret = p_ops->get_bbrpt_dle_cfg(adapter,
				       MAC_AX_PRPT_DEST_WLCPU,
				       &p_id, &q_id);
	if (ret) {
		PLTFM_MSG_ERR("%s: get bbrpt cfg fail\n", __func__);
		return ret;
	}

	chif->seg_size = GET_FIELD(val, B_AX_CH_INFO_SEG);
	chif->dis_to = GET_FIELD(val, B_AX_GET_CH_INFO_TO) ? 0 : 1;
	cfg->dest = (p_id == GET_FIELD(val, B_AX_DFS_PRTID) &&
		     q_id == GET_FIELD(val, B_AX_DFS_QID)) ?
		MAC_AX_PRPT_DEST_WLCPU : MAC_AX_PRPT_DEST_HOST;

	ret = check_mac_en(adapter, band, MAC_AX_CMAC_SEL);
	if (ret) {
		PLTFM_MSG_ERR("MAC%d is not ready\n", band);
		return ret;
	}

	reg = (band == MAC_AX_BAND_0) ?
		R_AX_CH_INFO_QRY : R_AX_CH_INFO_QRY_C1;

	val = MAC_REG_R32(reg);
	chif->trigger = GET_FIELD(val, B_AX_CH_INFO_MODE);
	chif->macid = GET_FIELD(val, B_AX_CH_INFO_MACID);
	chif->bmp_filter = (val & (B_AX_CH_INFO_CRC_FAIL |
				   B_AX_CH_INFO_DATA_FRM |
				   B_AX_CH_INFO_CTRL_FRM |
				   B_AX_CH_INFO_MGNT_FRM)) >> 16;

	return MACSUCCESS;
}

static u32 cfg_ch_info(struct mac_ax_adapter *adapter,
		       struct mac_ax_phy_rpt_cfg *cfg)
{
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
	struct mac_ax_priv_ops *p_ops = adapter_to_priv_ops(adapter);
	u32 reg, p_id, q_id;
	u32 val;
	u32 ret = 0;
	struct mac_role_tbl *role;
	struct mac_ax_ch_info *chif = &cfg->u.chif;
	u8 band;
	u8 intvl;

	role = mac_role_srch(adapter, chif->macid);
	if (!role) {
		PLTFM_MSG_ERR("[ERR]cannot find macid: %d\n", chif->macid);
		ret = MACNOITEM;
		goto END;
	}
	band = role->info.band;

	en_bbrpt(adapter);

	reg = (band == MAC_AX_BAND_0) ?
		R_AX_CH_INFO_QRY : R_AX_CH_INFO_QRY_C1;

	ret = check_mac_en(adapter, band, MAC_AX_CMAC_SEL);
	if (ret) {
		PLTFM_MSG_ERR("MAC%d is not ready\n", band);
		goto END;
	}

	if (!cfg->en) {
		ret = stop_ch_info(adapter, reg);
		goto END;
	}

	ret = p_ops->get_bbrpt_dle_cfg(adapter,
				       cfg->dest == MAC_AX_PRPT_DEST_WLCPU,
				       &p_id, &q_id);
	if (ret) {
		PLTFM_MSG_ERR("%s: get bbrpt cfg fail\n", __func__);
		goto END;
	}

	switch (chif->seg_size) {
	case MAC_AX_CH_IFNO_SEG_128:
		intvl = B_AX_CH_INFO_INTVL_1;
		break;
	case MAC_AX_CH_IFNO_SEG_256:
		intvl = B_AX_CH_INFO_INTVL_2;
		break;
	case MAC_AX_CH_IFNO_SEG_512:
		intvl = B_AX_CH_INFO_INTVL_4;
		break;
	case MAC_AX_CH_IFNO_SEG_1024:
		intvl = B_AX_CH_INFO_INTVL_7;
		break;
	default:
		PLTFM_MSG_ERR("Wrong ch info segment\n");
		ret = MACFUNCINPUT;
		goto END;
	}

	MAC_REG_W8(R_AX_BBRPT_COEX_CFG, B_AX_BBRPT_COEX_EN);

	MAC_REG_W32(R_AX_CH_INFO,
		    SET_WORD(q_id, B_AX_CH_INFO_QID) |
		    SET_WORD(p_id, B_AX_CH_INFO_PRTID) |
		    SET_WORD(B_AX_CH_INFO_REQ_2, B_AX_CH_INFO_REQ) |
		    SET_WORD(chif->seg_size, B_AX_CH_INFO_SEG) |
		    SET_WORD(intvl, B_AX_CH_INFO_INTVL) |
		    (chif->dis_to ?
		     0 : SET_WORD(B_AX_GET_CH_INFO_TO_28,
				  B_AX_GET_CH_INFO_TO)) |
		    (band ? B_AX_CH_INFO_PHY : 0) |
		    SET_WORD(B_AX_CH_INFO_BUF_128, B_AX_CH_INFO_BUF) |
		    B_AX_CH_INFO_EN);

	switch (chif->trigger) {
	case MAC_AX_CH_INFO_MACID:
	case MAC_AX_CH_INFO_ACK:
		val = B_AX_GET_CH_INFO_EN |
		      SET_WORD(MAC_AX_CH_INFO_MACID, B_AX_CH_INFO_MODE) |
		      SET_WORD(chif->macid, B_AX_CH_INFO_MACID) |
		      ((chif->bmp_filter << 16) & (B_AX_CH_INFO_CRC_FAIL |
						   B_AX_CH_INFO_DATA_FRM |
						   B_AX_CH_INFO_CTRL_FRM |
						   B_AX_CH_INFO_MGNT_FRM));
		break;
	case MAC_AX_CH_INFO_NDP:
		val = B_AX_GET_CH_INFO_EN |
		      SET_WORD(chif->trigger, B_AX_CH_INFO_MODE) |
		      SET_WORD(chif->macid, B_AX_CH_INFO_MACID);
		break;
	case MAC_AX_CH_INFO_SND:
		val = B_AX_GET_CH_INFO_EN |
		      SET_WORD(chif->trigger, B_AX_CH_INFO_MODE) |
		      SET_WORD(chif->macid, B_AX_CH_INFO_MACID) |
		      ((chif->bmp_filter << 16) & (B_AX_CH_INFO_CRC_FAIL |
						   B_AX_CH_INFO_DATA_FRM |
						   B_AX_CH_INFO_CTRL_FRM |
						   B_AX_CH_INFO_MGNT_FRM));

		break;
	default:
		PLTFM_MSG_ERR("Wrong channel info mode\n");
		ret = MACFUNCINPUT;
		goto END;
	}
#if MAC_AX_FW_REG_OFLD
	if (adapter->sm.fwdl == MAC_AX_FWDL_INIT_RDY) {
		ret = MAC_REG_W32_OFLD((u16)reg, val, 1);
		if (ret != MACSUCCESS)
			PLTFM_MSG_ERR("%s: write offload fail %d",
				      __func__, ret);
		return ret;
	}
#endif

	MAC_REG_W32(reg, val);
END:
	return ret;
}

static u32 stop_dfs(struct mac_ax_adapter *adapter)
{
#define MAC_AX_PHY_RPT_CNT 3000
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
	u8 val;
	u32 count = MAC_AX_PHY_RPT_CNT;

	val = MAC_REG_R8(R_AX_DFS_CFG0);
	MAC_REG_W8(R_AX_DFS_CFG0, val | B_AX_STOP_DFS);
	while (!(MAC_REG_R8(R_AX_DFS_CFG0) & B_AX_DFS_IN_STOP)) {
		count--;
		if (count == 0) {
			PLTFM_MSG_ERR("Polling ch info idle timeout\n");
			return MACPOLLTO;
		}
	}

	MAC_REG_W8(R_AX_DFS_CFG0, val & ~B_AX_DFS_RPT_EN);

	return MACSUCCESS;
}

static u32 get_dfs_cfg(struct mac_ax_adapter *adapter,
		       struct mac_ax_phy_rpt_cfg *cfg)
{
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
	struct mac_ax_priv_ops *p_ops = adapter_to_priv_ops(adapter);
	struct mac_ax_dfs *dfs = &cfg->u.dfs;
	u32 ret = 0, val;
	u32 p_id, q_id;

	ret = p_ops->get_bbrpt_dle_cfg(adapter,
				       MAC_AX_PRPT_DEST_WLCPU,
				       &p_id, &q_id);
	if (ret) {
		PLTFM_MSG_ERR("%s: get bbrpt cfg fail\n", __func__);
		return ret;
	}

	val = MAC_REG_R32(R_AX_DFS_CFG0);
	cfg->en = !!(val & B_AX_DFS_RPT_EN);
	dfs->num_th = GET_FIELD(val, B_AX_DFS_NUM_TH);
	dfs->en_timeout = GET_FIELD(val, B_AX_DFS_TIME_TH);
	cfg->dest = (p_id == GET_FIELD(val, B_AX_DFS_PRTID) &&
		     q_id == GET_FIELD(val, B_AX_DFS_QID)) ?
		MAC_AX_PRPT_DEST_WLCPU : MAC_AX_PRPT_DEST_HOST;

	return MACSUCCESS;
}

static u32 cfg_dfs(struct mac_ax_adapter *adapter,
		   struct mac_ax_phy_rpt_cfg *cfg)
{
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
	struct mac_ax_priv_ops *p_ops = adapter_to_priv_ops(adapter);
	struct mac_ax_dfs *dfs = &cfg->u.dfs;
	u32 ret = 0, val;
	u32 p_id, q_id, dfs_to = 0, num_th;

	en_bbrpt(adapter);

	if (!cfg->en) {
		ret = stop_dfs(adapter);
		goto END;
	}

	ret = p_ops->get_bbrpt_dle_cfg(adapter,
				       cfg->dest == MAC_AX_PRPT_DEST_WLCPU,
				       &p_id, &q_id);
	if (ret) {
		PLTFM_MSG_ERR("%s: get bbrpt cfg fail\n", __func__);
		goto END;
	}

	if (dfs->en_timeout) {
		switch (dfs->dfs_to) {
		case MAC_AX_DFS_TO_20MS:
		case MAC_AX_DFS_TO_40MS:
		case MAC_AX_DFS_TO_80MS:
			dfs_to = dfs->dfs_to;
			break;
		default:
			PLTFM_MSG_ERR("Wrong DFS report timeout\n");
			ret = MACFUNCINPUT;
			goto END;
		}
	}

	switch (dfs->num_th) {
	case MAC_AX_DFS_TH_29:
	case MAC_AX_DFS_TH_61:
	case MAC_AX_DFS_TH_93:
	case MAC_AX_DFS_TH_125:
		num_th = dfs->num_th;
		break;
	default:
		PLTFM_MSG_ERR("Wrong DFS report num threshold\n");
		ret = MACFUNCINPUT;
		goto END;
	}

	val = B_AX_DFS_RPT_EN |
	      SET_WORD(B_AX_DFS_BUF_64, B_AX_DFS_BUF) |
	      SET_WORD(num_th, B_AX_DFS_NUM_TH) |
	      SET_WORD(dfs_to, B_AX_DFS_TIME_TH) |
	      SET_WORD(q_id, B_AX_DFS_QID) |
	      SET_WORD(p_id, B_AX_DFS_PRTID);

	MAC_REG_W32(R_AX_DFS_CFG0, val);
END:
	return ret;
}

u32 mac_cfg_phy_rpt(struct mac_ax_adapter *adapter,
		    struct mac_ax_phy_rpt_cfg *rpt)
{
	u32 (*handle)(struct mac_ax_adapter *adapter,
		      struct mac_ax_phy_rpt_cfg *rpt);

	switch (rpt->type) {
	case MAC_AX_PPDU_STATUS:
		handle = cfg_ppdu_status;
		break;
	case MAC_AX_CH_INFO:
		handle = cfg_ch_info;
		break;
	case MAC_AX_DFS:
		handle = cfg_dfs;
		break;
	default:
		PLTFM_MSG_ERR("Wrong PHY report type\n");
		return MACFUNCINPUT;
	}

	return handle(adapter, rpt);
}

u32 mac_get_phy_rpt_cfg(struct mac_ax_adapter *adapter,
			struct mac_ax_phy_rpt_cfg *rpt)
{
	u32 (*handle)(struct mac_ax_adapter *adapter,
		      struct mac_ax_phy_rpt_cfg *rpt);

	switch (rpt->type) {
	case MAC_AX_PPDU_STATUS:
		handle = get_ppdu_status_cfg;
		break;
	case MAC_AX_CH_INFO:
		handle = get_ch_info_cfg;
		break;
	case MAC_AX_DFS:
		handle = get_dfs_cfg;
		break;
	default:
		PLTFM_MSG_ERR("Wrong PHY report type\n");
		return MACFUNCINPUT;
	}

	return handle(adapter, rpt);
}

static u32 parse_mac_info(struct mac_ax_adapter *adapter,
			  u8 *buf, u32 len,
			  struct mac_ax_ppdu_rpt *rpt)
{
	struct mac_ax_ppdu_usr *usr;
	struct mac_ax_mac_info_t *macinfo;
	u8 i;
	u32 ret = MACSUCCESS;
	u32 accu_size = sizeof(struct mac_ax_mac_info_t);
	u32 val;
	u8 *ptr;

	macinfo = (struct mac_ax_mac_info_t *)buf;

	/* dword0 */
	val = le32_to_cpu(macinfo->dword0);
	rpt->usr_num = (u8)GET_FIELD(val, AX_MAC_INFO_USR_NUM);
#if MAC_AX_8852A_SUPPORT || MAC_AX_8852B_SUPPORT || MAC_AX_8851B_SUPPORT
		if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852A) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8852B) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8851B)) {
			if (rpt->usr_num > MAC_MAX_4_USR) {
				PLTFM_MSG_ERR("The user num in mac info is invalid\n");
				ret = MACPARSEERR;
				goto END;
			}
		}
#endif
#if MAC_AX_8852C_SUPPORT || MAC_AX_8192XB_SUPPORT || MAC_AX_8851E_SUPPORT || MAC_AX_8852D_SUPPORT
		if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852C) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8192XB) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8851E) ||
		    is_chip_id(adapter, MAC_AX_CHIP_ID_8852D)) {
			if (rpt->usr_num > MAC_MAX_8_USR) {
				PLTFM_MSG_ERR("The user num in mac info is invalid\n");
				ret = MACPARSEERR;
				goto END;
			}
		}
#endif
	rpt->fw_def = (u8)GET_FIELD(val, AX_MAC_INFO_FW_DEFINE);
	rpt->lsig_len = (u16)GET_FIELD(val, AX_MAC_INFO_LSIG_LEN);
	rpt->is_to_self = !!(val & AX_MAC_INFO_IS_TO_SELF);
	rpt->rx_cnt_size = val & AX_MAC_INFO_RX_CNT_VLD ?
				MAC_AX_RX_CNT_SIZE : 0;

	/* dowrd1 */
	val = le32_to_cpu(macinfo->dword1);
	rpt->service = (u16)GET_FIELD(val, AX_MAC_INFO_SERVICE);
	rpt->plcp_size = (u8)GET_FIELD(val, AX_MAC_INFO_PLCP_LEN) * 8;

	/* dword2 */
	usr = rpt->usr;
	ptr = (u8 *)(macinfo + 1);
	for (i = 0; i < rpt->usr_num; i++, usr++) {
		val = le32_to_cpu(*((u32 *)ptr));
		usr->vld = !!(val & AX_MAC_INFO_MAC_ID_VALID);
		usr->has_data = !!(val & AX_MAC_INFO_HAS_DATA);
		usr->has_ctrl = !!(val & AX_MAC_INFO_HAS_CTRL);
		usr->has_mgnt = !!(val & AX_MAC_INFO_HAS_MGNT);
		usr->has_bcn = !!(val & AX_MAC_INFO_HAS_BCN);
		usr->macid = (u8)GET_FIELD(val, AX_MAC_INFO_MACID);
		accu_size += MAC_AX_MAC_INFO_USE_SIZE;
		ptr += MAC_AX_MAC_INFO_USE_SIZE;
	}

	/* 8-byte alignment */
	accu_size += rpt->usr_num & BIT(0) ? MAC_AX_MAC_INFO_USE_SIZE : 0;
	ptr += rpt->usr_num & BIT(0) ? MAC_AX_MAC_INFO_USE_SIZE : 0;
	if (rpt->rx_cnt_size) {
		rpt->rx_cnt_ptr = ptr;
		accu_size += rpt->rx_cnt_size;
		ptr += rpt->rx_cnt_size;
	}

	if (rpt->plcp_size) {
		rpt->plcp_ptr = ptr;
		accu_size += rpt->plcp_size;
		ptr += rpt->plcp_size;
	}

	if (len > accu_size) {
		rpt->phy_st_ptr = ptr;
		rpt->phy_st_size = len - accu_size;
	}
END:
	return ret;
}

u32 mac_parse_ppdu(struct mac_ax_adapter *adapter,
		   u8 *buf, u32 ppdu_len, u8 mac_info,
		   struct mac_ax_ppdu_rpt *rpt)
{
	u32 ret = MACSUCCESS;

	PLTFM_MEMSET(rpt, 0, sizeof(struct mac_ax_ppdu_rpt));

	if (mac_info) {
		ret = parse_mac_info(adapter, buf, ppdu_len, rpt);
	} else {
		rpt->phy_st_ptr = buf;
		rpt->phy_st_size = ppdu_len;
	}

	return ret;
}

u32 mac_parse_dfs(struct mac_ax_adapter *adapter,
		  u8 *buf, u32 dfs_len, struct mac_ax_dfs_rpt *rpt)
{
	struct mac_ax_dfs_hdr_t *dfs;
	u32 len;
	u32 msk = BIT(MAC_AX_DFS_RPT_SIZE_SH) - 1;
	u32 ret = 0;
	u32 val;

	dfs = (struct mac_ax_dfs_hdr_t *)buf;

	/* dword0 */
	val = le32_to_cpu(dfs->dword0);
	rpt->drop_num = (u16)GET_FIELD(val, AX_DFS_DROP_NUM);
	rpt->max_cont_drop = (u16)GET_FIELD(val, AX_DFS_MAX_CONT_DROP);

	/* dword1 */
	val = le32_to_cpu(dfs->dword1);
	rpt->total_drop = (u16)GET_FIELD(val, AX_DFS_TOTAL_DROP);
	len = dfs_len - MAC_AX_DFS_HDR_SIZE;
	rpt->dfs_num = (u16)(len >> MAC_AX_DFS_RPT_SIZE_SH);
	if (len & msk) {
		PLTFM_MSG_ERR("The DFS report size is wrong\n");
		ret = MACPARSEERR;
	}
	rpt->dfs_ptr = buf + MAC_AX_DFS_HDR_SIZE;

	return ret;
}

u32 mac_rst_drv_info(struct mac_ax_adapter *adapter)
{
	adapter->hw_info->cmac0_drv_info = MAC_AX_DRV_INFO_NONE;
	adapter->hw_info->cmac1_drv_info = MAC_AX_DRV_INFO_NONE;

	return MACSUCCESS;
}
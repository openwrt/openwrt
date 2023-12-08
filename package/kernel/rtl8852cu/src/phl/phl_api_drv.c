/******************************************************************************
 *
 * Copyright(c) 2019 Realtek Corporation.
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
 *****************************************************************************/
#define _PHL_API_DRV_C_
#include "phl_headers.h"

void *rtw_phl_get_txbd_buf(struct rtw_phl_com_t *phl_com)
{
	struct phl_info_t *phl_info = (struct phl_info_t *)phl_com->phl_priv;

	return phl_info->hci_trx_ops->get_txbd_buf(phl_info);
}

void *rtw_phl_get_rxbd_buf(struct rtw_phl_com_t *phl_com)
{
	struct phl_info_t *phl_info = (struct phl_info_t *)phl_com->phl_priv;

	return phl_info->hci_trx_ops->get_rxbd_buf(phl_info);
}

struct rtw_h2c_pkt *rtw_phl_query_h2c_pkt(struct rtw_phl_com_t *phl_com,
											enum rtw_h2c_pkt_type type)
{
	struct rtw_h2c_pkt *h2c_pkt = NULL;
	struct phl_info_t *phl_info = (struct phl_info_t *)phl_com->phl_priv;

	h2c_pkt = (struct rtw_h2c_pkt *)phl_query_idle_h2c_pkt(phl_info, type);

	if (!h2c_pkt)
		phl_dump_h2c_pool_stats(phl_info->h2c_pool);

	return h2c_pkt;
}

enum rtw_phl_status rtw_phl_pltfm_tx(struct rtw_phl_com_t *phl_com,
										struct rtw_h2c_pkt *pkt)
{
	enum rtw_phl_status pstatus = RTW_PHL_STATUS_FAILURE;
	struct phl_info_t *phl_info = (struct phl_info_t *)phl_com->phl_priv;

	#ifdef CONFIG_PCI_HCI
	struct phl_hci_trx_ops *hci_trx_ops = phl_info->hci_trx_ops;
	hci_trx_ops->recycle_busy_h2c(phl_info);
	#endif

	#ifdef CONFIG_RTW_MIRROR_DUMP
	phl_mirror_dump_h2c(phl_com->phl_priv, pkt);
	#endif

	pstatus = phl_info->hci_trx_ops->pltfm_tx(phl_info, pkt);

	return pstatus;
}

void rtw_phl_proc_cmd(void *phl, char proc_cmd,
		      struct rtw_proc_cmd *incmd,
		      char *output, u32 out_len)
{
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;
	struct hal_info_t *hal_info = (struct hal_info_t *)phl_info->hal;

	/* Avoid string comparison mismatch since extra char '\n' was appended to buf */
	if (incmd->in_type == RTW_ARG_TYPE_BUF
		&& incmd->in_cnt_len
		&& incmd->in.buf[incmd->in_cnt_len -1] == '\n'
		) {
		incmd->in_cnt_len--;
		incmd->in.buf[incmd->in_cnt_len] = '\0';
	}

	if (RTW_PROC_CMD_PHL == proc_cmd)
		rtw_phl_dbg_proc_cmd(phl_info, incmd, output, out_len);
	else if (RTW_PROC_CMD_CORE == proc_cmd)
		rtw_phl_dbg_core_cmd(phl_info, incmd, output, out_len);
	else
		rtw_hal_proc_cmd(hal_info, proc_cmd, incmd, output, out_len);
}

void rtw_phl_get_fw_ver(void *phl, char *ver_str, u16 len)
{
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;

	rtw_hal_get_fw_ver(phl_info->hal, ver_str, len);
}

enum rtw_fw_status rtw_phl_get_fw_status(void *phl)
{
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;

	return rtw_hal_get_fw_status(phl_info->hal);
}

enum rtw_phl_status rtw_phl_msg_hub_hal_send(struct rtw_phl_com_t *phl_com,
							struct phl_msg_attribute* attr, struct phl_msg* msg)
{
	return phl_msg_hub_send((struct phl_info_t*)phl_com->phl_priv, attr, msg);
}

void rtw_phl_test_txtb_cfg(struct rtw_phl_com_t* phl_com,
	void *buf, u32 buf_len, u8 *cfg_bssid, u16 cfg_aid, u8 cfg_bsscolor)
{
	/* Get parameters from MP UL */
	u8 ui_bssid[6]={0x00, 0x09, 0x08, 0x07, 0x06, 0x05};
	u16 ui_aid = 2;
	u8 ui_bsscolor = 1;
	u8 cfg;
	struct phl_info_t *phl_info = (struct phl_info_t *)phl_com->phl_priv;
	struct rtw_wifi_role_t *role = &phl_com->wifi_roles[0];
	struct rtw_wifi_role_link_t *rlink = get_rlink(role,
	                                               RTW_RLINK_PRIMARY);
	struct rtw_phl_stainfo_t *sta = rtw_phl_get_stainfo_self(
		phl_com->phl_priv, rlink);

	cfg = (u8)*((u8 *)buf);
	switch (cfg) {
		case 0:
			/* disable */
			role->mstate = MLME_NO_LINK;
			rlink->mstate = MLME_NO_LINK;

			_os_mem_cpy(phlcom_to_drvpriv(phl_com), sta->mac_addr,
				role->mac_addr, MAC_ALEN);
			rtw_hal_update_sta_entry(phl_com, phl_info->hal, sta, false);
			rtw_hal_role_cfg(phl_info->hal, role, rlink);
		break;

		case 1:
			role->mstate = MLME_LINKED;
			rlink->mstate = MLME_LINKED;

			_os_mem_cpy(phlcom_to_drvpriv(phl_com), sta->mac_addr,
				cfg_bssid, MAC_ALEN);
			sta->wmode = WLAN_MD_MAX;
			sta->aid = cfg_aid;
			sta->asoc_cap.bsscolor = cfg_bsscolor;
			sta->asoc_cap.pkt_padding = 2;
			rtw_hal_update_sta_entry(phl_com, phl_info->hal, sta, false);
			rtw_hal_role_cfg(phl_info->hal, role, rlink);
		break;

		case 2:
			role->mstate = MLME_LINKED;
			rlink->mstate = MLME_LINKED;

			_os_mem_cpy(phlcom_to_drvpriv(phl_com), sta->mac_addr,
				ui_bssid, MAC_ALEN);
			sta->wmode = WLAN_MD_MAX;
			sta->aid = ui_aid;
			sta->asoc_cap.bsscolor = ui_bsscolor;
			sta->asoc_cap.pkt_padding = 2;
			rtw_hal_update_sta_entry(phl_com, phl_info->hal, sta, false);
			rtw_hal_role_cfg(phl_info->hal, role, rlink);
		break;

		default:
		break;
	}
}

void rtw_phl_pkt_ofld_del_all_entry_req(struct rtw_phl_com_t* phl_com)
{
	struct phl_info_t *phl_info = (struct phl_info_t *)phl_com->phl_priv;

	phl_pkt_ofld_del_all_entry_req(phl_info);
}

void rtw_phl_dbg_dump_rx(void *phl, struct rtw_wifi_role_t *wrole)
{
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;
	struct rtw_wifi_role_link_t *rlink = NULL;
	u8 idx = 0;

	for (idx = 0; idx < wrole->rlink_num; idx++) {
		rlink = get_rlink(wrole, idx);

		phl_rx_dbg_dump(phl_info, rlink->hw_band);
	}
}

u32 rtw_phl_get_phy_stat_info(void *phl, enum phl_band_idx hw_band,
			      enum phl_stat_info_query phy_stat)
{
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;

	return rtw_hal_get_phy_stat_info(phl_info->hal, hw_band, phy_stat);
}

#ifdef CONFIG_PHL_DRV_HAS_NVM
enum rtw_phl_status
rtw_phl_extract_efuse_info(void *phl, u8 *efuse_map,
                           u32 info_type,
                           void *value,
                           u8 size, u8 map_valid)
{
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;

	return rtw_hal_extract_efuse_info(phl_info->hal, efuse_map,
	                                  (enum rtw_efuse_info)info_type,
					  value,
	                                  size, map_valid);
}

enum rtw_phl_status
rtw_phl_get_efuse_size(void *phl, u32 *log_efuse_size, u32 *limit_efuse_size,
                       u32 *mask_size, u32 *limit_mask_size)
{
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;

	return rtw_hal_get_efuse_size(phl_info->hal, log_efuse_size,
	                              limit_efuse_size, mask_size,
	                              limit_mask_size);
}

enum rtw_phl_status
rtw_phl_nvm_apply_dev_cap(void *phl)
{
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;
	return rtw_hal_nvm_apply_dev_cap(phl_info->hal, phl_info->phl_com);
}

void rtw_phl_dump_tpu(void * phl)
{
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;
	struct hal_info_t *hal_info = (struct hal_info_t *)(phl_info->hal);

	rtw_hal_bb_dump_tpu_info(hal_info);
}

#endif /* CONFIG_PHL_DRV_HAS_NVM */

void rtw_phl_enable_interrupt_sync(struct rtw_phl_com_t* phl_com)
{
#ifdef CONFIG_SYNC_INTERRUPT
	struct rtw_phl_evt_ops *evt_ops = &phl_com->evt_ops;

	evt_ops->set_interrupt_caps(phl_com->drv_priv, true);
#else
	struct phl_info_t *phl_info = (struct phl_info_t *)phl_com->phl_priv;

	rtw_hal_enable_interrupt(phl_com, phl_info->hal);
#endif /* CONFIG_SYNC_INTERRUPT */
}

void rtw_phl_disable_interrupt_sync(struct rtw_phl_com_t* phl_com)
{
#ifdef CONFIG_SYNC_INTERRUPT
	struct rtw_phl_evt_ops *evt_ops = &phl_com->evt_ops;

	evt_ops->set_interrupt_caps(phl_com->drv_priv, false);
#else
	struct phl_info_t *phl_info = (struct phl_info_t *)phl_com->phl_priv;

	rtw_hal_disable_interrupt(phl_com, phl_info->hal);
#endif /* CONFIG_SYNC_INTERRUPT */
}


#ifdef CONFIG_PHL_FW_DUMP_EFUSE
void rtw_phl_fw_dump_efuse_precfg(struct rtw_phl_com_t* phl_com)
{
	struct phl_info_t *phl_info = (struct phl_info_t *)phl_com->phl_priv;

	phl_datapath_start_hw(phl_info);
	rtw_phl_enable_interrupt_sync(phl_com);
}

void rtw_phl_fw_dump_efuse_postcfg(struct rtw_phl_com_t* phl_com)
{
	struct phl_info_t *phl_info = (struct phl_info_t *)phl_com->phl_priv;

	rtw_phl_disable_interrupt_sync(phl_com);
	phl_datapath_stop_hw(phl_info);
}
#endif
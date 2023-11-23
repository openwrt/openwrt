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
#define _PHL_INIT_C_
#include "phl_headers.h"

void _phl_com_init_rssi_stat(struct rtw_phl_com_t *phl_com)
{
	u8 i = 0, j = 0;
	for (i = 0; i < RTW_RSSI_TYPE_MAX; i++) {
		phl_com->rssi_stat.ma_rssi_ele_idx[i] = 0;
		phl_com->rssi_stat.ma_rssi_ele_cnt[i] = 0;
		phl_com->rssi_stat.ma_rssi_ele_sum[i] = 0;
		phl_com->rssi_stat.ma_rssi[i] = 0;
		for (j = 0; j < PHL_RSSI_MAVG_NUM; j++)
			phl_com->rssi_stat.ma_rssi_ele[i][j] = 0;
	}
	_os_spinlock_init(phl_com->drv_priv, &(phl_com->rssi_stat.lock));
}

void _phl_com_deinit_rssi_stat(struct rtw_phl_com_t *phl_com)
{
	_os_spinlock_free(phl_com->drv_priv, &(phl_com->rssi_stat.lock));
}

/**
 * rtw_phl_init_ppdu_sts_para(...)
 * Description:
 * 	1. Do not call this api after rx started.
 * 	2. PPDU Status per PKT settings
 **/
void rtw_phl_init_ppdu_sts_para(struct rtw_phl_com_t *phl_com,
				bool en_psts_per_pkt, bool psts_ampdu,
				u8 rx_fltr)
{
#ifdef CONFIG_PHL_RX_PSTS_PER_PKT
	phl_com->ppdu_sts_info.en_psts_per_pkt = en_psts_per_pkt;
	phl_com->ppdu_sts_info.psts_ampdu = psts_ampdu;
#ifdef RTW_WKARD_DISABLE_PSTS_PER_PKT_DATA
	/* Forced disable PSTS for DATA frame, to avoid unknown performance issue */
	rx_fltr &= (~RTW_PHL_PSTS_FLTR_DATA);
#endif
	phl_com->ppdu_sts_info.ppdu_sts_filter = rx_fltr;
#else
	return;
#endif
}

void _phl_com_deinit_ppdu_sts(struct rtw_phl_com_t *phl_com)
{
#ifdef CONFIG_PHL_RX_PSTS_PER_PKT
	u8 i = 0;
	u8 j = 0;
	for (j = 0; j < HW_BAND_MAX; j++) {
		for (i = 0; i < PHL_MAX_PPDU_CNT; i++) {
			if (phl_com->ppdu_sts_info.sts_ent[j][i].frames.cnt != 0) {
				PHL_INFO("[Error] deinit_ppdu_sts : frame queue is not empty\n");
			}
			pq_deinit(phl_com->drv_priv,
				  &(phl_com->ppdu_sts_info.sts_ent[j][i].frames));
		}
	}
#else
	return;
#endif
}

void _phl_com_init_ppdu_sts(struct rtw_phl_com_t *phl_com)
{
#ifdef CONFIG_PHL_RX_PSTS_PER_PKT
	u8 i = 0;
#endif
	u8 j = 0;
	for (j = 0; j < HW_BAND_MAX; j++) {
		phl_com->ppdu_sts_info.cur_rx_ppdu_cnt[j] = 0xFF;
	}
#ifdef CONFIG_PHL_RX_PSTS_PER_PKT
	/* Default enable when compile flag is set. */
	phl_com->ppdu_sts_info.en_psts_per_pkt = true;
	/**
	 * Filter of buffer pkt for phy status:
	 *	if the correspond bit is set to 1,
	 *	the pkt will be buffer till ppdu sts or next ppdu is processed.
	 **/
	phl_com->ppdu_sts_info.ppdu_sts_filter =
			RTW_PHL_PSTS_FLTR_MGNT | RTW_PHL_PSTS_FLTR_CTRL |
			RTW_PHL_PSTS_FLTR_EXT_RSVD;

	/* if set to false, only the first mpdu in ppdu has phy status */
	phl_com->ppdu_sts_info.psts_ampdu = false;

	phl_com->ppdu_sts_info.en_fake_psts = false;

	for (j = 0; j < HW_BAND_MAX; j++) {
		for (i = 0; i < PHL_MAX_PPDU_CNT; i++) {
			pq_init(phl_com->drv_priv,
				&(phl_com->ppdu_sts_info.sts_ent[j][i].frames));
		}
	}
#endif
}

static void phl_msg_entry(void* priv, struct phl_msg *msg)
{
	struct phl_info_t *phl_info = (struct phl_info_t *)priv;
	u8 mdl_id = MSG_MDL_ID_FIELD(msg->msg_id);
	u16 evt_id = MSG_EVT_ID_FIELD(msg->msg_id);

	PHL_DBG("[PHL]%s, mdl_id(%d)\n", __FUNCTION__, mdl_id);

	/* dispatch received PHY msg here */
	switch(mdl_id) {
		case PHL_MDL_PHY_MGNT:
			phl_msg_hub_phy_mgnt_evt_hdlr(phl_info, evt_id);
			break;
		case PHL_MDL_RX:
			phl_msg_hub_rx_evt_hdlr(phl_info, evt_id, msg->inbuf, msg->inlen);
			break;
		default:
			break;
	}
}

static enum rtw_phl_status phl_register_msg_entry(struct phl_info_t *phl_info)
{
	struct phl_msg_receiver ctx;
	void *d = phl_to_drvpriv(phl_info);
	u8 imr[] = {PHL_MDL_PHY_MGNT, PHL_MDL_RX, PHL_MDL_MRC, PHL_MDL_POWER_MGNT
			, PHL_MDL_BTC};
	_os_mem_set(d, &ctx, 0, sizeof(struct phl_msg_receiver));
	ctx.incoming_evt_notify = phl_msg_entry;
	ctx.priv = (void*)phl_info;
	if( phl_msg_hub_register_recver((void*)phl_info,
				&ctx, MSG_RECV_PHL) == RTW_PHL_STATUS_SUCCESS) {
		/* PHL layer module should set IMR for receiving
		desired PHY msg  and handle it in phl_phy_evt_entry*/
		phl_msg_hub_update_recver_mask((void*)phl_info, MSG_RECV_PHL,
						imr, sizeof(imr), false);
		return RTW_PHL_STATUS_SUCCESS;
	}
	else
		return RTW_PHL_STATUS_FAILURE;

}

static enum rtw_phl_status phl_deregister_msg_entry(
					struct phl_info_t *phl_info)
{
	return phl_msg_hub_deregister_recver((void*)phl_info, MSG_RECV_PHL);
}

static enum rtw_phl_status phl_fw_init(struct phl_info_t *phl_info)
{
	enum rtw_phl_status phl_status = RTW_PHL_STATUS_RESOURCE;
	struct rtw_phl_com_t *phl_com = phl_info->phl_com;
	struct rtw_fw_info_t *fw_info = &phl_com->fw_info;

	FUNCIN_WSTS(phl_status);

	fw_info->rom_buff = _os_mem_alloc(phl_to_drvpriv(phl_info), RTW_MAX_FW_SIZE);

	if (!fw_info->rom_buff) {
		PHL_ERR("%s : rom buff allocate fail!!\n", __func__);
		goto mem_alloc_fail;
	}

	fw_info->ram_buff = _os_mem_alloc(phl_to_drvpriv(phl_info), RTW_MAX_FW_SIZE);

	if (!fw_info->ram_buff) {
		PHL_ERR("%s : ram buff allocate fail!!\n", __func__);
		goto mem_alloc_fail;
	}

#ifdef CONFIG_PHL_REUSED_FWDL_BUF
	fw_info->buf = _os_mem_alloc(phl_to_drvpriv(phl_info), RTW_MAX_FW_SIZE);

	/* if allocating failed, fw bin files will be reading every time */
	if (!fw_info->buf)
		PHL_WARN("%s : buf for fw storage allocate fail!!\n", __func__);

	fw_info->wow_buf = _os_mem_alloc(phl_to_drvpriv(phl_info), RTW_MAX_FW_SIZE);

	/* if allocating failed, fw bin files will be reading every time */
	if (!fw_info->wow_buf)
		PHL_WARN("%s : wow buf for wowlan fw storage allocate fail!!\n", __func__);
#endif

	phl_status = RTW_PHL_STATUS_SUCCESS;

	FUNCOUT_WSTS(phl_status);

mem_alloc_fail:
	return phl_status;
}

static void phl_fw_deinit(struct phl_info_t *phl_info)
{
	struct rtw_fw_info_t *fw_info = &phl_info->phl_com->fw_info;

	if (fw_info->rom_buff)
		_os_mem_free(phl_to_drvpriv(phl_info), fw_info->rom_buff,
			RTW_MAX_FW_SIZE);
	if (fw_info->ram_buff)
		_os_mem_free(phl_to_drvpriv(phl_info), fw_info->ram_buff,
			RTW_MAX_FW_SIZE);

#ifdef CONFIG_PHL_REUSED_FWDL_BUF
	if (fw_info->buf)
		_os_mem_free(phl_to_drvpriv(phl_info), fw_info->buf,
			RTW_MAX_FW_SIZE);
	if (fw_info->wow_buf)
		_os_mem_free(phl_to_drvpriv(phl_info), fw_info->wow_buf,
			RTW_MAX_FW_SIZE);
#endif

	/* allocate in rtw_hal_ld_fw_symbol */
	if (fw_info->sym_buf)
		_os_mem_free(phl_to_drvpriv(phl_info), fw_info->sym_buf,
			RTW_MAX_FW_SIZE);
}
static enum rtw_phl_status
phl_register_background_module_entry(struct phl_info_t *phl_info)
{
	enum rtw_phl_status phl_status = RTW_PHL_STATUS_FAILURE;
#ifdef CONFIG_CMD_DISP
	/*
	 * setup struct phl_module_ops & call dispr_register_module
	 * to register background module instance.
	 * call dispr_deregister_module if you need to dynamically
	 * deregister the instance of background module.
	*/

	/* 1,2,3 cmd controller section */


	/* 41 ~ 70 mandatory background module section*/
#ifdef CONFIG_PHL_CMD_SER
	phl_status = phl_register_ser_module(phl_info);
	if (phl_status != RTW_PHL_STATUS_SUCCESS)
		return phl_status;
#endif
#ifdef CONFIG_POWER_SAVE
	phl_status = phl_register_ps_module(phl_info);
	if (phl_status != RTW_PHL_STATUS_SUCCESS)
		return phl_status;
#endif
	/* 70 ~ 127 optional background module section*/
#ifdef CONFIG_MR_COEX_SUPPORT
	phl_status = phl_register_mr_coex_module(phl_info);
	if (phl_status != RTW_PHL_STATUS_SUCCESS)
		return phl_status;
#endif /* CONFIG_MR_COEX_SUPPORT */
#ifdef CONFIG_PHL_CMD_BTC
	phl_status = phl_register_btc_module(phl_info);
	if (phl_status != RTW_PHL_STATUS_SUCCESS)
		return phl_status;
#endif
	phl_status = phl_register_custom_module(phl_info, HW_BAND_0);
	if (phl_status != RTW_PHL_STATUS_SUCCESS)
		return phl_status;

	phl_status = phl_register_led_module(phl_info);
	if (phl_status != RTW_PHL_STATUS_SUCCESS)
		return phl_status;

	phl_status = phl_register_cmd_general(phl_info);
	if (phl_status != RTW_PHL_STATUS_SUCCESS)
		return phl_status;

	/* 10 ~ 40 protocol, wifi role section*/
	phl_status = phl_register_mrc_module(phl_info);
	if (phl_status != RTW_PHL_STATUS_SUCCESS)
		return phl_status;

	phl_status = phl_snd_cmd_register_module(phl_info);
	if (phl_status != RTW_PHL_STATUS_SUCCESS)
		return phl_status;
#else
	phl_status = RTW_PHL_STATUS_SUCCESS;
#endif
	return phl_status;
}

static void phl_txpwr_regu_info_init(struct phl_info_t *phl_info)
{
	struct rtw_phl_com_t *phl_com = phl_info->phl_com;
	struct txpwr_regu_info_t *pr_info = &phl_com->txpwr_regu_info;

	pr_info->force = false;
	pr_info->regu_2g = NULL;
	pr_info->regu_2g_len = 0;
	pr_info->regu_5g = NULL;
	pr_info->regu_5g_len = 0;
	pr_info->regu_6g = NULL;
	pr_info->regu_6g_len = 0;
}

static void phl_txpwr_regu_info_deinit(struct phl_info_t *phl_info)
{
	void *drv_priv = phl_to_drvpriv(phl_info);
	struct rtw_phl_com_t *phl_com = phl_info->phl_com;
	struct txpwr_regu_info_t *pr_info = &phl_com->txpwr_regu_info;

	if (pr_info->regu_2g) {
		_os_kmem_free(drv_priv, pr_info->regu_2g, pr_info->regu_2g_len);
		pr_info->regu_2g = NULL;
		pr_info->regu_2g_len = 0;
	}
	if (pr_info->regu_5g) {
		_os_kmem_free(drv_priv, pr_info->regu_5g, pr_info->regu_5g_len);
		pr_info->regu_5g = NULL;
		pr_info->regu_5g_len = 0;
	}
	if (pr_info->regu_6g) {
		_os_kmem_free(drv_priv, pr_info->regu_6g, pr_info->regu_6g_len);
		pr_info->regu_6g = NULL;
		pr_info->regu_6g_len = 0;
	}
}

static enum rtw_phl_status phl_com_init(void *drv_priv,
					struct phl_info_t *phl_info,
					struct rtw_ic_info *ic_info)
{
	enum rtw_phl_status phl_status = RTW_PHL_STATUS_FAILURE;

	phl_info->phl_com = _os_mem_alloc(drv_priv,
						sizeof(struct rtw_phl_com_t));
	if (phl_info->phl_com == NULL) {
		phl_status = RTW_PHL_STATUS_RESOURCE;
		PHL_ERR("alloc phl_com failed\n");
		goto error_phl_com_mem;
	}

	phl_info->phl_com->phl_priv = phl_info;
	phl_info->phl_com->drv_priv = drv_priv;
	phl_info->phl_com->hci_type = ic_info->hci_type;
	phl_info->phl_com->edcca_mode = RTW_EDCCA_NORMAL;
	phl_info->phl_com->tpe_info.valid_tpe_cnt = 0;

	phl_sw_cap_init(phl_info->phl_com);

	_os_spinlock_init(drv_priv, &phl_info->phl_com->evt_info.evt_lock);

	phl_fw_init(phl_info);
	#ifdef CONFIG_PHL_CHANNEL_INFO
	phl_status = phl_chaninfo_init(phl_info);
	if (phl_status)
		goto error_phl_com_mem;
	#endif /* CONFIG_PHL_CHANNEL_INFO */

	_phl_com_init_rssi_stat(phl_info->phl_com);
	_phl_com_init_ppdu_sts(phl_info->phl_com);
#ifdef PHL_RXSC_AMPDU
	_os_spinlock_init(drv_priv, &phl_info->phl_com->rxsc_entry.rxsc_lock);
#endif

	phl_txpwr_regu_info_init(phl_info);

	phl_status = RTW_PHL_STATUS_SUCCESS;
	return phl_status;

error_phl_com_mem:
	return phl_status;
}

static enum rtw_phl_status phl_hci_init(struct phl_info_t *phl_info,
									struct rtw_ic_info *ic_info)
{
	enum rtw_phl_status phl_status = RTW_PHL_STATUS_FAILURE;

	phl_info->hci = _os_mem_alloc(phl_to_drvpriv(phl_info),
					sizeof(struct hci_info_t));
	if (phl_info->hci == NULL) {
		phl_status = RTW_PHL_STATUS_RESOURCE;
		goto error_hci_mem;
	}
#ifdef CONFIG_USB_HCI
	phl_info->hci->usb_bulkout_size = ic_info->usb_info.usb_bulkout_size;
#endif

	/* init variable of hci_info_t struct */

	phl_status = RTW_PHL_STATUS_SUCCESS;

error_hci_mem:
	return phl_status;
}

static void phl_com_deinit(struct phl_info_t *phl_info,
				struct rtw_phl_com_t *phl_com)
{
	void *drv_priv = phl_to_drvpriv(phl_info);

	/* deinit variable or stop mechanism. */
	if (phl_com) {
		phl_sw_cap_deinit(phl_info->phl_com);
		_os_spinlock_free(drv_priv, &phl_com->evt_info.evt_lock);
		_phl_com_deinit_rssi_stat(phl_info->phl_com);
		_phl_com_deinit_ppdu_sts(phl_info->phl_com);
		phl_txpwr_regu_info_deinit(phl_info);
		phl_fw_deinit(phl_info);
		#ifdef CONFIG_PHL_CHANNEL_INFO
		phl_chaninfo_deinit(phl_info);
		#endif /* CONFIG_PHL_CHANNEL_INFO */
		#ifdef PHL_RXSC_AMPDU
		_os_spinlock_free(drv_priv, &phl_com->rxsc_entry.rxsc_lock);
		#endif /* PHL_RXSC_AMPDU */
		_os_mem_free(drv_priv, phl_com, sizeof(struct rtw_phl_com_t));
	}
}

static void phl_hci_deinit(struct phl_info_t *phl_info, struct hci_info_t *hci)
{

	/* deinit variable or stop mechanism. */
	if (hci)
		_os_mem_free(phl_to_drvpriv(phl_info), hci,
						sizeof(struct hci_info_t));
}

static enum rtw_phl_status _phl_hci_ops_check(struct phl_info_t *phl_info)
{
	enum rtw_phl_status status = RTW_PHL_STATUS_SUCCESS;
	struct phl_hci_trx_ops *trx_ops = phl_info->hci_trx_ops;

	if (!trx_ops->hci_trx_init) {
		phl_ops_error_msg("hci_trx_init");
		status = RTW_PHL_STATUS_FAILURE;
	}
	if (!trx_ops->hci_trx_deinit) {
		phl_ops_error_msg("hci_trx_deinit");
		status = RTW_PHL_STATUS_FAILURE;
	}
	if (!trx_ops->prepare_tx) {
		phl_ops_error_msg("prepare_tx");
		status = RTW_PHL_STATUS_FAILURE;
	}
	if (!trx_ops->recycle_rx_buf) {
		phl_ops_error_msg("recycle_rx_buf");
		status = RTW_PHL_STATUS_FAILURE;
	}
	if (!trx_ops->tx) {
		phl_ops_error_msg("tx");
		status = RTW_PHL_STATUS_FAILURE;
	}
	if (!trx_ops->rx) {
		phl_ops_error_msg("rx");
		status = RTW_PHL_STATUS_FAILURE;
	}
	if (!trx_ops->trx_cfg) {
		phl_ops_error_msg("trx_cfg");
		status = RTW_PHL_STATUS_FAILURE;
	}
	if (!trx_ops->pltfm_tx) {
		phl_ops_error_msg("pltfm_tx");
		status = RTW_PHL_STATUS_FAILURE;
	}
	if (!trx_ops->alloc_h2c_pkt_buf) {
		phl_ops_error_msg("alloc_h2c_pkt_buf");
		status = RTW_PHL_STATUS_FAILURE;
	}
	if (!trx_ops->free_h2c_pkt_buf) {
		phl_ops_error_msg("free_h2c_pkt_buf");
		status = RTW_PHL_STATUS_FAILURE;
	}
	if (!trx_ops->trx_reset) {
		phl_ops_error_msg("trx_reset");
		status = RTW_PHL_STATUS_FAILURE;
	}
	if (!trx_ops->trx_resume) {
		phl_ops_error_msg("trx_resume");
		status = RTW_PHL_STATUS_FAILURE;
	}
	if (!trx_ops->req_tx_stop) {
		phl_ops_error_msg("req_tx_stop");
		status = RTW_PHL_STATUS_FAILURE;
	}
	if (!trx_ops->req_rx_stop) {
		phl_ops_error_msg("req_rx_stop");
		status = RTW_PHL_STATUS_FAILURE;
	}
	if (!trx_ops->is_tx_pause) {
		phl_ops_error_msg("is_tx_pause");
		status = RTW_PHL_STATUS_FAILURE;
	}
	if (!trx_ops->is_rx_pause) {
		phl_ops_error_msg("is_rx_pause");
		status = RTW_PHL_STATUS_FAILURE;
	}
	if (!trx_ops->get_txbd_buf) {
		phl_ops_error_msg("get_txbd_buf");
		status = RTW_PHL_STATUS_FAILURE;
	}
	if (!trx_ops->get_rxbd_buf) {
		phl_ops_error_msg("get_rxbd_buf");
		status = RTW_PHL_STATUS_FAILURE;
	}
	if (!trx_ops->recycle_rx_pkt) {
		phl_ops_error_msg("recycle_rx_pkt");
		status = RTW_PHL_STATUS_FAILURE;
	}
	if (!trx_ops->rx_handle_normal) {
		phl_ops_error_msg("rx_handle_normal");
		status = RTW_PHL_STATUS_FAILURE;
	}
	if (!trx_ops->tx_watchdog) {
		phl_ops_error_msg("tx_watchdog");
		status = RTW_PHL_STATUS_FAILURE;
	}

#ifdef CONFIG_PCI_HCI
	if (!trx_ops->recycle_busy_wd) {
		phl_ops_error_msg("recycle_busy_wd");
		status = RTW_PHL_STATUS_FAILURE;
	}
	if (!trx_ops->recycle_busy_h2c) {
		phl_ops_error_msg("recycle_busy_h2c");
		status = RTW_PHL_STATUS_FAILURE;
	}
#endif

#ifdef CONFIG_USB_HCI
	if (!trx_ops->pend_rxbuf) {
		phl_ops_error_msg("pend_rxbuf");
		status = RTW_PHL_STATUS_FAILURE;
	}
	if (!trx_ops->recycle_tx_buf) {
		phl_ops_error_msg("recycle_tx_buf");
		status = RTW_PHL_STATUS_FAILURE;
	}
#endif

	return status;
}

static enum rtw_phl_status phl_set_hci_ops(struct phl_info_t *phl_info)
{
	#ifdef CONFIG_PCI_HCI
	if (phl_get_hci_type(phl_info->phl_com) == RTW_HCI_PCIE)
		phl_hook_trx_ops_pci(phl_info);
	#endif

	#ifdef CONFIG_USB_HCI
	if (phl_get_hci_type(phl_info->phl_com) == RTW_HCI_USB)
		phl_hook_trx_ops_usb(phl_info);
	#endif

	#ifdef CONFIG_SDIO_HCI
	if (phl_get_hci_type(phl_info->phl_com) == RTW_HCI_SDIO)
		phl_hook_trx_ops_sdio(phl_info);
	#endif

	return _phl_hci_ops_check(phl_info);
}

#ifdef CONFIG_FSM
static enum rtw_phl_status phl_cmd_init(struct phl_info_t *phl_info)
{
	if (phl_info->cmd_fsm != NULL)
		return RTW_PHL_STATUS_FAILURE;

	phl_info->cmd_fsm = phl_cmd_new_fsm(phl_info->fsm_root, phl_info);
	if (phl_info->cmd_fsm == NULL)
		return RTW_PHL_STATUS_FAILURE;

	if (phl_info->cmd_obj != NULL)
		goto obj_fail;

	phl_info->cmd_obj = phl_cmd_new_obj(phl_info->cmd_fsm, phl_info);
	if (phl_info->cmd_obj == NULL)
		goto obj_fail;

	return RTW_PHL_STATUS_SUCCESS;

obj_fail:
	phl_fsm_deinit_fsm(phl_info->cmd_fsm);
	phl_info->cmd_fsm = NULL;
	return RTW_PHL_STATUS_FAILURE;
}

static void phl_cmd_deinit(struct phl_info_t *phl_info)
{
	phl_cmd_destory_obj(phl_info->cmd_obj);
	phl_info->cmd_obj = NULL;
	phl_cmd_destory_fsm(phl_info->cmd_fsm);
	phl_info->cmd_fsm = NULL;
}

static enum rtw_phl_status phl_ser_init(struct phl_info_t *phl_info)
{
	if (phl_info->ser_fsm != NULL)
		return RTW_PHL_STATUS_FAILURE;

	phl_info->ser_fsm = phl_ser_new_fsm(phl_info->fsm_root, phl_info);
	if (phl_info->ser_fsm == NULL)
		return RTW_PHL_STATUS_FAILURE;

	if (phl_info->ser_obj != NULL)
		goto obj_fail;

	phl_info->ser_obj = phl_ser_new_obj(phl_info->ser_fsm, phl_info);
	if (phl_info->ser_obj == NULL)
		goto obj_fail;

	return RTW_PHL_STATUS_SUCCESS;

obj_fail:
	phl_ser_destory_fsm(phl_info->ser_fsm);
	phl_info->ser_fsm = NULL;
	return RTW_PHL_STATUS_FAILURE;
}

static void phl_ser_deinit(struct phl_info_t *phl_info)
{
	phl_ser_destory_obj(phl_info->ser_obj);
	phl_info->ser_obj = NULL;

	phl_ser_destory_fsm(phl_info->ser_fsm);
	phl_info->ser_fsm = NULL;
}

static enum rtw_phl_status phl_fsm_init(struct phl_info_t *phl_info)
{
	if (phl_info->fsm_root != NULL)
		return RTW_PHL_STATUS_FAILURE;

	/* allocate memory for fsm to do version control */
	phl_info->fsm_root = phl_fsm_init_root(phl_info);
	if (phl_info->fsm_root == NULL)
		return RTW_PHL_STATUS_FAILURE;

	return RTW_PHL_STATUS_SUCCESS;
}

static void phl_fsm_deinit(struct phl_info_t *phl_info)
{
	/* free memory for fsm */
	phl_fsm_deinit_root(phl_info->fsm_root);
	phl_info->fsm_root = NULL;
}

static enum rtw_phl_status phl_fsm_module_init(struct phl_info_t *phl_info)
{
	enum rtw_phl_status phl_status = RTW_PHL_STATUS_SUCCESS;

	phl_status = phl_cmd_init(phl_info);
	if (phl_status != RTW_PHL_STATUS_SUCCESS) {
		PHL_ERR("phl_cmd_init failed\n");
		goto cmd_fail;
	}

	phl_status = phl_ser_init(phl_info);
	if (phl_status != RTW_PHL_STATUS_SUCCESS) {
		PHL_ERR("phl_ser_init failed\n");
		goto ser_fail;
	}

	return phl_status;

ser_fail:
	phl_cmd_deinit(phl_info);
cmd_fail:
	return phl_status;
}

static void phl_fsm_module_deinit(struct phl_info_t *phl_info)
{
	phl_ser_deinit(phl_info);
	phl_cmd_deinit(phl_info);
}

static enum rtw_phl_status phl_fsm_start(struct phl_info_t *phl_info)
{
	return phl_fsm_start_root(phl_info->fsm_root);
}

static enum rtw_phl_status phl_fsm_stop(struct phl_info_t *phl_info)
{
	return phl_fsm_stop_root(phl_info->fsm_root);
}

static enum rtw_phl_status phl_fsm_module_start(struct phl_info_t *phl_info)
{
	enum rtw_phl_status phl_status = RTW_PHL_STATUS_SUCCESS;

	phl_status = phl_fsm_start_fsm(phl_info->ser_fsm);
	if (phl_status != RTW_PHL_STATUS_SUCCESS)
		goto ser_fail;

	phl_status = phl_cmd_start(phl_info->cmd_obj);
	if (phl_status != RTW_PHL_STATUS_SUCCESS)
		goto cmd_fail;

	return phl_status;

cmd_fail:
	phl_fsm_stop_fsm(phl_info->ser_fsm);
ser_fail:
	phl_fsm_cmd_stop(phl_info);

	return phl_status;
}

static enum rtw_phl_status phl_fsm_module_stop(struct phl_info_t *phl_info)
{
	enum rtw_phl_status phl_status = RTW_PHL_STATUS_SUCCESS;

	phl_fsm_stop_fsm(phl_info->ser_fsm);
	phl_fsm_cmd_stop(phl_info);

	return phl_status;
}

#endif /*CONFIG_FSM*/
static enum rtw_phl_status phl_module_init(struct phl_info_t *phl_info)
{
	enum rtw_phl_status phl_status = RTW_PHL_STATUS_SUCCESS;
	u8 phy_num = 1;

	phl_status = phl_msg_hub_init(phl_info);
	if (phl_status != RTW_PHL_STATUS_SUCCESS) {
		PHL_ERR("phl_msg_hub_init failed\n");
		goto msg_hub_fail;
	}

	phl_status = phl_wow_mdl_init(phl_info);
	if (phl_status != RTW_PHL_STATUS_SUCCESS) {
		PHL_ERR("phl_wow_mdl_init failed\n");
		goto wow_init_fail;
	}

	phl_status = phl_pkt_ofld_init(phl_info);
	if (phl_status != RTW_PHL_STATUS_SUCCESS) {
		PHL_ERR("phl_pkt_ofld_init failed\n");
		goto pkt_ofld_init_fail;
	}

	if (!phl_test_module_init(phl_info)) {
		PHL_ERR("phl_test_module_init failed\n");
		phl_status = RTW_PHL_STATUS_FAILURE;
		goto error_test_module_init;
	}

	phl_status = phl_p2pps_init(phl_info);
	if (phl_status != RTW_PHL_STATUS_SUCCESS) {
		PHL_ERR("phl_p2pps_init failed\n");
		goto error_p2pps_init;
	}

#ifdef CONFIG_DBCC_SUPPORT
	phy_num = HW_BAND_MAX;
#endif
	phl_status = phl_disp_eng_init(phl_info, phy_num);
	if (phl_status != RTW_PHL_STATUS_SUCCESS) {
		PHL_ERR("phl_disp_eng_init failed\n");
		goto error_disp_eng_init;
	}

	phl_status = phl_register_background_module_entry(phl_info);
	if (phl_status != RTW_PHL_STATUS_SUCCESS) {
		PHL_ERR("phl_register_disp_eng_module_entry failed\n");
		goto error_disp_eng_reg_init;
	}

	phl_status = phl_ecsa_ctrl_init(phl_info);
	if (phl_status != RTW_PHL_STATUS_SUCCESS) {
		PHL_ERR("phl_ecsa_ctrl_init failed\n");
		goto error_ecsa_ctrl_init;
	}
	return phl_status;

error_ecsa_ctrl_init:
error_disp_eng_reg_init:
	phl_disp_eng_deinit(phl_info);
error_disp_eng_init:
	phl_p2pps_deinit(phl_info);
error_p2pps_init:
	phl_test_module_deinit(phl_info->phl_com);
error_test_module_init:
	phl_pkt_ofld_deinit(phl_info);
pkt_ofld_init_fail:
	phl_wow_mdl_deinit(phl_info);
wow_init_fail:
	phl_msg_hub_deinit(phl_info);
msg_hub_fail:
	return phl_status;
}

static void phl_module_deinit(struct phl_info_t *phl_info)
{
	phl_ecsa_ctrl_deinit(phl_info);
	phl_disp_eng_deinit(phl_info);
	phl_test_module_deinit(phl_info->phl_com);
	phl_pkt_ofld_deinit(phl_info);
	phl_wow_mdl_deinit(phl_info);
	phl_msg_hub_deinit(phl_info);
	phl_p2pps_deinit(phl_info);
}

static enum rtw_phl_status phl_module_start(struct phl_info_t *phl_info)
{
	enum rtw_phl_status phl_status = RTW_PHL_STATUS_SUCCESS;

	if (!phl_test_module_start(phl_info->phl_com)) {
		PHL_ERR("phl_test_module_start failed\n");
		phl_status = RTW_PHL_STATUS_FAILURE;
		goto error_test_mdl_start;
	}

	phl_status = phl_disp_eng_start(phl_info);
	if (phl_status != RTW_PHL_STATUS_SUCCESS) {
		PHL_ERR("phl_disp_eng_start failed\n");
		goto error_disp_eng_start;
	}

	if(phl_info->msg_hub) {
		phl_msg_hub_start(phl_info);
		phl_register_msg_entry(phl_info);
	}

	return phl_status;

error_disp_eng_start:
	phl_test_module_stop(phl_info->phl_com);
error_test_mdl_start:
	return phl_status;
}

static enum rtw_phl_status phl_module_stop(struct phl_info_t *phl_info)
{
	enum rtw_phl_status phl_status = RTW_PHL_STATUS_SUCCESS;

	phl_status = phl_cmd_enqueue(phl_info, HW_BAND_0, MSG_EVT_MDL_CHECK_STOP,
		NULL, 0, NULL, PHL_CMD_WAIT, 500);

	phl_disp_eng_stop(phl_info);
	phl_test_module_stop(phl_info->phl_com);

	if(phl_info->msg_hub) {
		phl_deregister_msg_entry(phl_info);
		phl_msg_hub_stop(phl_info);
	}

	return phl_status;
}

static enum rtw_phl_status phl_var_init(struct phl_info_t *phl_info)
{
	return RTW_PHL_STATUS_SUCCESS;
}

static void phl_var_deinit(struct phl_info_t *phl_info)
{

}

struct rtw_phl_com_t *rtw_phl_get_com(void *phl)
{
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;

	return phl_info->phl_com;
}

static void phl_regulation_init(void *drv_priv, void *phl)
{
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;
	struct rtw_regulation *rg = NULL;

	if (!drv_priv || !phl)
		return;

	rg = &phl_info->regulation;

	_os_spinlock_init(drv_priv, &rg->lock);
	rg->init = 1;
	rg->domain.code = INVALID_DOMAIN_CODE;
	rg->domain_6g.code = INVALID_DOMAIN_CODE;
	rg->tpo = TPO_NA;
	phl_regu_policy_init(drv_priv, phl);
}

static void phl_regulation_deinit(void *drv_priv, void *phl)
{
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;
	struct rtw_regulation *rg = NULL;

	if (!drv_priv || !phl)
		return;

	rg = &phl_info->regulation;

	_os_spinlock_free(drv_priv, &rg->lock);
}

enum rtw_phl_status rtw_phl_init(void *drv_priv, void **phl,
					struct rtw_ic_info *ic_info)
{
	struct phl_info_t *phl_info = NULL;
	enum rtw_phl_status phl_status = RTW_PHL_STATUS_FAILURE;
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_FAILURE;

	FUNCIN();
	phl_info = _os_mem_alloc(drv_priv, sizeof(struct phl_info_t));
	if (phl_info == NULL) {
		phl_status = RTW_PHL_STATUS_RESOURCE;
		PHL_ERR("alloc phl_info failed\n");
		goto error_phl_mem;
	}
	_os_mem_set(drv_priv, phl_info, 0, sizeof(struct phl_info_t));
	*phl = phl_info;

	phl_regulation_init(drv_priv, phl_info);

	phl_status = phl_com_init(drv_priv, phl_info, ic_info);
	if (phl_status != RTW_PHL_STATUS_SUCCESS) {
		phl_status = RTW_PHL_STATUS_RESOURCE;
		PHL_ERR("alloc phl_com failed\n");
		goto error_phl_com_mem;
	}

	phl_status = phl_hci_init(phl_info, ic_info);
	if (phl_status != RTW_PHL_STATUS_SUCCESS) {
		PHL_ERR("phl_hci_init failed\n");
		goto error_hci_init;
	}

	phl_status = phl_set_hci_ops(phl_info);
	if (phl_status != RTW_PHL_STATUS_SUCCESS) {
		PHL_ERR("phl_set_hci_ops failed\n");
		goto error_set_hci_ops;
	}
#ifdef CONFIG_FSM
	phl_status = phl_fsm_init(phl_info);
	if (phl_status != RTW_PHL_STATUS_SUCCESS) {
		PHL_ERR("phl_fsm_init failed\n");
		goto error_fsm_init;
	}

	/* init FSM modules */
	phl_status = phl_fsm_module_init(phl_info);
	if (phl_status != RTW_PHL_STATUS_SUCCESS) {
		PHL_ERR("phl_fsm_module_init failed\n");
		goto error_fsm_module_init;
	}
#endif
	phl_status = phl_twt_init(*phl);
	if (phl_status != RTW_PHL_STATUS_SUCCESS) {
		PHL_ERR("phl_twt_init failed\n");
		goto error_phl_twt_init;
	}

#ifdef CONFIG_RTW_ACS
	phl_status = phl_acs_info_init(phl_info);
	if (phl_status != RTW_PHL_STATUS_SUCCESS) {
		PHL_ERR("phl_acs_info_init failed\n");
		goto error_phl_acs_info_init;
	}
#endif

	hal_status = rtw_hal_init(drv_priv, phl_info->phl_com,
					&(phl_info->hal), ic_info->ic_id);
	if ((hal_status != RTW_HAL_STATUS_SUCCESS) || (phl_info->hal == NULL)) {
		phl_status = RTW_PHL_STATUS_HAL_INIT_FAILURE;
		PHL_ERR("rtw_hal_init failed status(%d),phl_info->hal(%p)\n",
			hal_status, phl_info->hal);
		goto error_hal_init;
	}

	/*send bus info to hal*/
	rtw_hal_hci_cfg(phl_info->phl_com, phl_info->hal, ic_info);

	/*get hw capability from mac/bb/rf/btc/efuse/fw-defeature-rpt*/
	hal_status = rtw_hal_read_chip_info(phl_info->phl_com, phl_info->hal);
	if (hal_status != RTW_HAL_STATUS_SUCCESS) {
		phl_status = RTW_PHL_STATUS_HAL_INIT_FAILURE;
		PHL_ERR("rtw_hal_read_chip_info failed\n");
		goto error_hal_read_chip_info;
	}

	hal_status = rtw_hal_var_init(phl_info->phl_com, phl_info->hal);
	if (hal_status != RTW_HAL_STATUS_SUCCESS) {
		phl_status = RTW_PHL_STATUS_HAL_INIT_FAILURE;
		PHL_ERR("rtw_hal_var_init failed\n");
		goto error_hal_var_init;
	}

	phl_status = phl_var_init(phl_info);
	if (phl_status != RTW_PHL_STATUS_SUCCESS) {
		PHL_ERR("phl_var_init failed\n");
		goto error_phl_var_init;
	}

	/* init mr_ctrl, wifi_role[] */
	phl_status = phl_mr_ctrl_init(phl_info);
	if (phl_status != RTW_PHL_STATUS_SUCCESS) {
		PHL_ERR("phl_mr_ctrl_init failed\n");
		goto error_wifi_role_ctrl_init;
	}

	/* init modules */
	phl_status = phl_module_init(phl_info);
	if (phl_status != RTW_PHL_STATUS_SUCCESS) {
		PHL_ERR("phl_module_init failed\n");
		goto error_module_init;
	}

	/* init macid_ctrl , stainfo_ctrl*/
	/* init after get hw cap - macid number*/
	phl_status = phl_macid_ctrl_init(phl_info);
	if (phl_status != RTW_PHL_STATUS_SUCCESS) {
		PHL_ERR("phl_macid_ctrl_init failed\n");
		goto error_macid_ctrl_init;
	}

	/*init after hal_init - hal_sta_info*/
	phl_status = phl_stainfo_ctrl_init(phl_info);
	if (phl_status != RTW_PHL_STATUS_SUCCESS) {
		PHL_ERR("phl_stainfo_ctrl_init failed\n");
		goto error_stainfo_ctrl_init;
	}

	/* init mld_ctrl */
	phl_status = phl_mld_ctrl_init(phl_info);
	if (phl_status != RTW_PHL_STATUS_SUCCESS) {
		PHL_ERR("phl_mld_ctrl_init failed\n");
		goto error_mld_ctrl_init;
	}

	FUNCOUT();

	return phl_status;

error_mld_ctrl_init:
	phl_stainfo_ctrl_deinit(phl_info);
error_stainfo_ctrl_init:
	phl_macid_ctrl_deinit(phl_info);
error_macid_ctrl_init:
	phl_module_deinit(phl_info);
error_module_init:
	phl_mr_ctrl_deinit(phl_info);
error_wifi_role_ctrl_init:
	phl_var_deinit(phl_info);
error_phl_var_init:
error_hal_var_init:
error_hal_read_chip_info:
	rtw_hal_deinit(phl_info->phl_com, phl_info->hal);
error_hal_init:
#ifdef CONFIG_RTW_ACS
	phl_acs_info_deinit(phl_info);
error_phl_acs_info_init:
#endif
error_phl_twt_init:
	phl_twt_deinit(phl);
#ifdef CONFIG_FSM
	phl_fsm_module_deinit(phl_info);
error_fsm_module_init:
	phl_fsm_deinit(phl_info);
error_fsm_init:
	/* Do nothing */
#endif
error_set_hci_ops:
	phl_hci_deinit(phl_info, phl_info->hci);
error_hci_init:
	phl_com_deinit(phl_info, phl_info->phl_com);
error_phl_com_mem:
	if (phl_info) {
		phl_regulation_deinit(drv_priv, phl_info);
		_os_mem_free(drv_priv, phl_info, sizeof(struct phl_info_t));
		*phl = phl_info = NULL;
	}
error_phl_mem:
	return phl_status;
}

void rtw_phl_deinit(void *phl)
{
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;
	void *drv_priv = phl_to_drvpriv(phl_info);

	if (phl_info) {
		#ifdef CONFIG_RTW_ACS
		phl_acs_info_deinit(phl_info);
		#endif
		phl_twt_deinit(phl);
		phl_mld_ctrl_deinit(phl_info);
		phl_stainfo_ctrl_deinit(phl_info);
		phl_macid_ctrl_deinit(phl_info);
		/*deinit mr_ctrl, wifi_role[]*/
		phl_module_deinit(phl_info);
		phl_mr_ctrl_deinit(phl_info);
		rtw_hal_deinit(phl_info->phl_com, phl_info->hal);
		phl_var_deinit(phl_info);
		#ifdef CONFIG_FSM
		phl_fsm_module_deinit(phl_info);
		phl_fsm_deinit(phl_info);
		#endif
		phl_hci_deinit(phl_info, phl_info->hci);
		phl_com_deinit(phl_info, phl_info->phl_com);
		phl_regulation_deinit(drv_priv, phl_info);
		_os_mem_free(drv_priv, phl_info,
					sizeof(struct phl_info_t));
	}
}

enum rtw_phl_status
rtw_phl_trx_alloc(void *phl)
{
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;
	enum rtw_phl_status phl_status = RTW_PHL_STATUS_FAILURE;

	phl_status = phl_datapath_init(phl_info);
	if (phl_status != RTW_PHL_STATUS_SUCCESS) {
		PHL_ERR("phl_datapath_init failed\n");
		goto error_datapath;
	}

	phl_status = phl_trx_test_init(phl);
	if (phl_status != RTW_PHL_STATUS_SUCCESS) {
		PHL_ERR("phl_trx_test_init failed\n");
		goto error_trx_test;
	}

	return phl_status;

error_trx_test:
	phl_datapath_deinit(phl_info);
error_datapath:
	return phl_status;
}

void
rtw_phl_trx_free_handler(void *phl)
{
	phl_trx_free_handler(phl);
}

void
rtw_phl_trx_free_sw_rsc(void *phl)
{
	phl_trx_free_sw_rsc(phl);
	phl_trx_test_deinit(phl);
}

void
rtw_phl_trx_free(void *phl)
{
	rtw_phl_trx_free_handler(phl);
	rtw_phl_trx_free_sw_rsc(phl);
}


bool rtw_phl_is_init_completed(void *phl)
{
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;

	return rtw_hal_is_inited(phl_info->phl_com, phl_info->hal);
}

#ifdef CONFIG_VW_REFINE
void rtw_phl_show_vw_cnt(void *phl, u32 value)
{
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;
	struct rtw_hal_com_t *hal_com = rtw_hal_get_halcom(phl_info->hal);

	if (value == _CMD_DUMP_VW_CNT)
		PHL_ERR("vw_snd:%d vw_rec:%d vw_err:%d\n",
				hal_com->trx_stat.vw_cnt_snd,
				hal_com->trx_stat.vw_cnt_rev,
				hal_com->trx_stat.vw_cnt_err);
	else if (value == _CMD_RESET_VW_CNT) {
			hal_com->trx_stat.vw_cnt_snd = 0;
			hal_com->trx_stat.vw_cnt_rev = 0;
			hal_com->trx_stat.vw_cnt_err = 0;
	}
}

#ifdef CONFIG_PCI_HCI
void rtw_phl_cmd_dump(void *phl, u32 value)
{
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;
	struct phl_hci_trx_ops *trx_ops = phl_info->hci_trx_ops;

	if (!trx_ops->dump_wd_info) {
		trx_ops->dump_wd_info(phl_info, value);
	}
}

void rtw_phl_cmd_wd_info(void *phl, u32 value)
{
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;
	struct phl_hci_trx_ops *trx_ops = phl_info->hci_trx_ops;

	if (!trx_ops->dump_wd_info) {
		trx_ops->dump_wd_info(phl_info, value + _CMD_MAX_CPUID_VAL);
	}
}

void rtw_phl_cmd_debug_wd_release(void *phl, u32 value)
{
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;
	struct phl_hci_trx_ops *trx_ops = phl_info->hci_trx_ops;

	if (!trx_ops->dump_wd_info) {
		if (value < _CMD_WP_OFFSET)
			trx_ops->dump_wd_info(phl_info, (value + _CMD_MAX_RESET_WP_VAL));
		else
			trx_ops->dump_wd_info(phl_info, value);
	}
}
#endif
#endif

#ifdef RTW_PHL_BCN
enum rtw_phl_status
phl_add_beacon(struct phl_info_t *phl_info, struct rtw_bcn_info_cmn *bcn_cmn)
{
	struct rtw_phl_com_t *phl_com = phl_info->phl_com;
	void *hal = phl_info->hal;

	if(rtw_hal_add_beacon(phl_com, hal, bcn_cmn) == RTW_HAL_STATUS_SUCCESS)
		return RTW_PHL_STATUS_SUCCESS;
	else
		return RTW_PHL_STATUS_FAILURE;
}

enum rtw_phl_status phl_update_beacon(struct phl_info_t *phl_info, u8 bcn_id)
{
	struct rtw_phl_com_t *phl_com = phl_info->phl_com;
	void *hal = phl_info->hal;

	if(rtw_hal_update_beacon(phl_com, hal, bcn_id) == RTW_HAL_STATUS_SUCCESS)
		return RTW_PHL_STATUS_SUCCESS;
	else
		return RTW_PHL_STATUS_FAILURE;
}

enum rtw_phl_status
rtw_phl_free_bcn_entry(void *phl,
                       struct rtw_wifi_role_link_t *rlink)
{
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;
	struct rtw_phl_com_t *phl_com = phl_info->phl_com;
	struct rtw_bcn_info_cmn *bcn_cmn = &rlink->bcn_cmn;
	void *hal = phl_info->hal;
	enum rtw_phl_status phl_status = RTW_PHL_STATUS_SUCCESS;

	if (bcn_cmn->bcn_added == 1) {
		if (rtw_hal_free_beacon(phl_com, hal, bcn_cmn->bcn_id) == RTW_HAL_STATUS_SUCCESS) {
			bcn_cmn->bcn_added = 0;
			phl_status = RTW_PHL_STATUS_SUCCESS;
		} else {
			phl_status = RTW_PHL_STATUS_FAILURE;
		}
	}

	return phl_status;
}

enum rtw_phl_status
phl_beacon_stop(struct phl_info_t *phl_info,
                struct rtw_wifi_role_link_t *rlink,
                u8 stop)
{
	enum rtw_phl_status pstatus = RTW_PHL_STATUS_SUCCESS;
	enum rtw_hal_status hstatus = RTW_HAL_STATUS_SUCCESS;

	hstatus = rtw_hal_beacon_stop(phl_info->hal, rlink, stop);
	if (hstatus != RTW_HAL_STATUS_SUCCESS)
		pstatus = RTW_PHL_STATUS_FAILURE;

	return pstatus;
}

enum rtw_phl_status
phl_issue_beacon(struct phl_info_t *phl_info, struct rtw_bcn_info_cmn *bcn_cmn)
{
	struct rtw_phl_com_t *phl_com = phl_info->phl_com;
	struct rtw_bcn_info_cmn *wrole_bcn_cmn;
	struct rtw_wifi_role_t *wifi_role;
	struct rtw_wifi_role_link_t *rlink;
	u8 lidx;
	void *drv = phl_com->drv_priv;
	u8 bcn_id, role_idx, bcn_added;

	role_idx = bcn_cmn->role_idx;
	if (role_idx > MAX_WIFI_ROLE_NUMBER) {
		PHL_ERR("%s: role idx err(%d)\n", __func__, role_idx);
		return RTW_PHL_STATUS_FAILURE;
	}

	wifi_role = &phl_com->wifi_roles[role_idx];
	lidx = bcn_cmn->lidx;
	rlink = get_rlink(wifi_role, lidx);
	wrole_bcn_cmn = &rlink->bcn_cmn;
	bcn_added = wrole_bcn_cmn->bcn_added;
	_os_mem_cpy(drv, wrole_bcn_cmn, bcn_cmn, sizeof(struct rtw_bcn_info_cmn));

	/* BCN add */
	if (!bcn_added) {
		if(phl_add_beacon(phl_info, wrole_bcn_cmn) == RTW_PHL_STATUS_SUCCESS) {
			wrole_bcn_cmn->bcn_added = true;
			return RTW_PHL_STATUS_SUCCESS;
		} else {
			return RTW_PHL_STATUS_FAILURE;
		}
	} else {
		/* BCN update */
		bcn_id = wrole_bcn_cmn->bcn_id;
		if(phl_update_beacon(phl_info, bcn_id) == RTW_PHL_STATUS_SUCCESS)
			return RTW_PHL_STATUS_SUCCESS;
		else
			return RTW_PHL_STATUS_FAILURE;
	}
}

#ifdef CONFIG_RTW_DEBUG_BCN_TX
enum rtw_phl_status rtw_phl_get_beacon_cnt(void *phl,
			u8 bcn_id, struct rtw_bcn_stats **bcn_stats)
{
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;
	struct rtw_phl_com_t *phl_com = phl_info->phl_com;
	void *hal = phl_info->hal;

	if (RTW_HAL_STATUS_SUCCESS ==
		rtw_hal_get_beacon_cnt(phl_com, hal, bcn_id, bcn_stats))
		return RTW_PHL_STATUS_SUCCESS;
	else
		return RTW_PHL_STATUS_FAILURE;
}
#endif

#ifdef CONFIG_CMD_DISP
enum rtw_phl_status
phl_cmd_issue_bcn_hdl(struct phl_info_t *phl_info, u8 *param)
{
	struct rtw_bcn_info_cmn *bcn_cmn = (struct rtw_bcn_info_cmn *)param;

	return phl_issue_beacon(phl_info, bcn_cmn);
}

static void _phl_issue_bcn_done(void *drv_priv, u8 *buf, u32 buf_len,
						enum rtw_phl_status status)
{
	if (buf) {
		_os_kmem_free(drv_priv, buf, buf_len);
		buf = NULL;
		PHL_INFO("%s.....\n", __func__);
	}
}

enum rtw_phl_status
rtw_phl_cmd_issue_beacon(void *phl,
                         struct rtw_wifi_role_link_t *rlink,
                         struct rtw_bcn_info_cmn *bcn_cmn,
                         enum phl_cmd_type cmd_type,
                         u32 cmd_timeout)
{
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;
	void *drv = rlink->wrole->phl_com->drv_priv;
	u8 hw_band = rlink->hw_band;
	enum rtw_phl_status psts = RTW_PHL_STATUS_FAILURE;
	struct rtw_bcn_info_cmn *param = NULL;
	u32 param_len;

	if (cmd_type == PHL_CMD_DIRECTLY) {
		psts = phl_issue_beacon(phl_info, bcn_cmn);
		goto _exit;
	}

	param_len = sizeof(struct rtw_bcn_info_cmn);
	param = _os_kmem_alloc(drv, param_len);
	if (param == NULL) {
		PHL_ERR("%s: alloc param failed!\n", __func__);
		goto _exit;
	}

	_os_mem_cpy(drv, param, bcn_cmn, param_len);

	psts = phl_cmd_enqueue(phl_info,
	                       hw_band,
	                       MSG_EVT_ISSUE_BCN,
	                       (u8 *)param,
	                       param_len,
	                       _phl_issue_bcn_done,
	                       cmd_type, cmd_timeout);

	if (is_cmd_failure(psts)) {
		/* Send cmd success, but wait cmd fail*/
		psts = RTW_PHL_STATUS_FAILURE;
	} else if (psts != RTW_PHL_STATUS_SUCCESS) {
		/* Send cmd fail */
		_os_kmem_free(phl_to_drvpriv(phl_info), param, param_len);
		psts = RTW_PHL_STATUS_FAILURE;
	}

_exit:
	return psts;
}

struct stop_bcn_param {
	struct rtw_wifi_role_link_t *rlink;
	u8 stop;
};

enum rtw_phl_status
phl_cmd_stop_bcn_hdl(struct phl_info_t *phl_info, u8 *param)
{
	struct stop_bcn_param *bcn_param = (struct stop_bcn_param *)param;

	return phl_beacon_stop(phl_info, bcn_param->rlink, bcn_param->stop);
}

static void _phl_stop_bcn_done(void *drv_priv, u8 *buf, u32 buf_len,
						enum rtw_phl_status status)
{
	if (buf) {
		_os_kmem_free(drv_priv, buf, buf_len);
		buf = NULL;
		PHL_INFO("%s.....\n", __func__);
	}
}

enum rtw_phl_status
rtw_phl_cmd_stop_beacon(void *phl,
                        struct rtw_wifi_role_link_t *rlink,
                        u8 stop,
                        enum phl_cmd_type cmd_type,
                        u32 cmd_timeout)
{
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;
	void *drv = rlink->wrole->phl_com->drv_priv;
	u8 hw_band = rlink->hw_band;
	enum rtw_phl_status psts = RTW_PHL_STATUS_FAILURE;
	struct stop_bcn_param *param = NULL;
	u32 param_len;

	if (cmd_type == PHL_CMD_DIRECTLY) {
		psts = phl_beacon_stop(phl_info, rlink, stop);
		goto _exit;
	}

	param_len = sizeof(struct stop_bcn_param);
	param = _os_kmem_alloc(drv, param_len);
	if (param == NULL) {
		PHL_ERR("%s: alloc param failed!\n", __func__);
		goto _exit;
	}

	param->rlink = rlink;
	param->stop = stop;

	psts = phl_cmd_enqueue(phl_info,
	                       hw_band,
	                       MSG_EVT_STOP_BCN,
	                       (u8 *)param,
	                       param_len,
	                       _phl_stop_bcn_done,
	                       cmd_type,
	                       cmd_timeout);

	if (is_cmd_failure(psts)) {
		/* Send cmd success, but wait cmd fail*/
		psts = RTW_PHL_STATUS_FAILURE;
	} else if (psts != RTW_PHL_STATUS_SUCCESS) {
		/* Send cmd fail */
		_os_kmem_free(phl_to_drvpriv(phl_info), param, param_len);
		psts = RTW_PHL_STATUS_FAILURE;
	}
_exit:
	return psts;
}
#else /*for FSM*/
enum rtw_phl_status
rtw_phl_cmd_stop_beacon(void *phl,
                        struct rtw_wifi_role_link_t *rlink,
                        u8 stop,
                        enum phl_cmd_type cmd_type,
                        u32 cmd_timeout)
{
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;

	return phl_beacon_stop(phl_info, rlink, stop);
}

enum rtw_phl_status
rtw_phl_cmd_issue_beacon(void *phl,
                         struct rtw_wifi_role_link_t *rlink,
                         struct rtw_bcn_info_cmn *bcn_cmn,
                         enum phl_cmd_type cmd_type,
                         u32 cmd_timeout);
{
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;

	return phl_issue_beacon(phl_info, bcn_cmn);
}
#endif /*CONFIG_CMD_DISP*/
#endif /*RTW_PHL_BCN*/

void rtw_phl_cap_pre_config(void *phl)
{
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;
	/* FW Pre-config */
	rtw_hal_fw_cap_pre_config(phl_info->phl_com,phl_info->hal);
	/* Bus Pre-config */
	rtw_hal_bus_cap_pre_config(phl_info->phl_com,phl_info->hal);
}

enum rtw_phl_status rtw_phl_preload(void *phl)
{
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_SUCCESS;

	FUNCIN();

	hal_status = rtw_hal_preload(phl_info->phl_com, phl_info->hal);

	phl_datapath_reset(phl_info, PHL_CTRL_TX | PHL_CTRL_RX |
	                   PHL_CTRL_IN_PIPE | PHL_CTRL_OUT_PIPE);

	if (hal_status != RTW_HAL_STATUS_SUCCESS)
		return RTW_PHL_STATUS_FAILURE;

	return RTW_PHL_STATUS_SUCCESS;
}

enum rtw_phl_status rtw_phl_start(void *phl)
{
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;
	enum rtw_phl_status phl_status = RTW_PHL_STATUS_FAILURE;
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_SUCCESS;
#ifdef CONFIG_SYNC_INTERRUPT
	struct rtw_phl_evt_ops *evt_ops = &phl_info->phl_com->evt_ops;
#endif /* CONFIG_SYNC_INTERRUPT */
#ifdef CONFIG_POWER_SAVE
	struct rtw_ps_cap_t *ps_cap = _get_ps_sw_cap(phl_info);
#endif

#ifdef CONFIG_POWER_SAVE
	if (ps_cap->init_rf_state == RTW_RF_OFF)
		PHL_INFO("%s: init_rf_state is RTW_RF_OFF\n", __func__);
	else
#endif /* CONFIG_POWER_SAVE */
		hal_status = rtw_hal_start(phl_info->phl_com, phl_info->hal);


	if (hal_status == RTW_HAL_STATUS_MAC_INIT_FAILURE) {
		phl_status = RTW_PHL_STATUS_HAL_INIT_FAILURE;
		goto error_hal_start;
	} else if (hal_status == RTW_HAL_STATUS_BB_INIT_FAILURE) {
		phl_status = RTW_PHL_STATUS_HAL_INIT_FAILURE;
		goto error_hal_start;
	} else if (hal_status == RTW_HAL_STATUS_RF_INIT_FAILURE) {
		phl_status = RTW_PHL_STATUS_HAL_INIT_FAILURE;
		goto error_hal_start;
	} else if (hal_status == RTW_HAL_STATUS_BTC_INIT_FAILURE) {
		phl_status = RTW_PHL_STATUS_HAL_INIT_FAILURE;
		goto error_hal_start;
	} else if (hal_status != RTW_HAL_STATUS_SUCCESS) {
		phl_status = RTW_PHL_STATUS_HAL_INIT_FAILURE;
		goto error_hal_start;
	}

#ifdef CONFIG_LOAD_PHY_PARA_FROM_FILE
	/* if no need keep para buf, phl_com->dev_sw_cap->keep_para_info = false*/
	rtw_phl_init_free_para_buf(phl_info->phl_com);
#endif

#ifdef CONFIG_FSM
	/* start FSM framework */
	phl_status = phl_fsm_start(phl_info);
	if (phl_status != RTW_PHL_STATUS_SUCCESS)
		goto error_phl_fsm_start;

	/* start FSM modules */
	phl_status = phl_fsm_module_start(phl_info);
	if (phl_status != RTW_PHL_STATUS_SUCCESS)
		goto error_phl_fsm_module_start;
#endif
	/* start modules */
	phl_status = phl_module_start(phl_info);
	if (phl_status != RTW_PHL_STATUS_SUCCESS)
		goto error_phl_module_start;

#ifdef CONFIG_POWER_SAVE
	if (ps_cap->init_rf_state == RTW_RF_ON) {
#endif /* CONFIG_POWER_SAVE */
		phl_status = phl_datapath_start_hw(phl_info);
		if (phl_status != RTW_PHL_STATUS_SUCCESS)
			goto error_phl_datapath_start_hw;

		phl_status = phl_datapath_start_sw(phl_info, PHL_MDL_PHY_MGNT);
		if (phl_status != RTW_PHL_STATUS_SUCCESS)
			goto error_phl_datapath_start_sw;

#ifdef RTW_WKARD_98D_INTR_EN_TIMING
	/* NOT enable interrupt here to avoid system hang during WiFi up
		move interrupt enable to end of hw_iface init */
#else
#ifdef CONFIG_SYNC_INTERRUPT
	evt_ops->set_interrupt_caps(phl_to_drvpriv(phl_info), true);
#else
	rtw_hal_enable_interrupt(phl_info->phl_com, phl_info->hal);
#endif /* CONFIG_SYNC_INTERRUPT */
#endif /* RTW_WKARD_98D_INTR_EN_TIMING */
#ifdef CONFIG_POWER_SAVE
	}
#endif /* CONFIG_POWER_SAVE */

	phl_info->phl_com->dev_state = RTW_DEV_WORKING;

#ifdef PHL_RXSC_AMPDU
	phl_info->phl_com->rxsc_entry.cached_rx_macid = PHL_MACID_MAX_NUM;
#endif

	phl_status = RTW_PHL_STATUS_SUCCESS;

	return phl_status;

error_phl_datapath_start_sw:
	phl_datapath_stop_hw(phl_info);
error_phl_datapath_start_hw:
	phl_module_stop(phl_info);
error_phl_module_start:
#ifdef CONFIG_FSM
	phl_fsm_module_stop(phl_info);
error_phl_fsm_module_start:
	phl_fsm_stop(phl_info);
error_phl_fsm_start:
#endif
	rtw_hal_stop(phl_info->phl_com, phl_info->hal);
error_hal_start:
	return phl_status;
}

static void _phl_interrupt_stop(struct phl_info_t *phl_info)
{
#ifdef CONFIG_SYNC_INTERRUPT
	struct rtw_phl_evt_ops *evt_ops = &phl_info->phl_com->evt_ops;

	do {
		if (false == TEST_STATUS_FLAG(phl_info->phl_com->dev_state,
		                              RTW_DEV_SURPRISE_REMOVAL))
			evt_ops->set_interrupt_caps(phl_to_drvpriv(phl_info), false);
	} while (false);
#else
	do {
		if (false == TEST_STATUS_FLAG(phl_info->phl_com->dev_state,
		                              RTW_DEV_SURPRISE_REMOVAL))
			rtw_hal_disable_interrupt(phl_info->phl_com, phl_info->hal);
	} while (false);
#endif /* CONFIG_SYNC_INTERRUPT */
}

static enum rtw_phl_status _phl_cmd_send_msg_phy_on(struct phl_info_t *phl_info)
{
	enum rtw_phl_status sts = RTW_PHL_STATUS_FAILURE;
#ifdef DBG_PHY_ON_TIME
	u32 phyon_start = 0, phyon_t = 0;
#endif /* DBG_PHY_ON_TIME */

#ifdef DBG_PHY_ON_TIME
	phyon_start = _os_get_cur_time_ms();
#endif /* DBG_PHY_ON_TIME */

	sts = phl_cmd_enqueue(phl_info, HW_BAND_0, MSG_EVT_PHY_ON, NULL, 0, NULL,
			PHL_CMD_WAIT, 3000);

#ifdef DBG_PHY_ON_TIME
	phyon_t = phl_get_passing_time_ms(phyon_start);
	if (phyon_t > 1000) {
		PHL_TRACE(COMP_PHL_DBG, _PHL_WARNING_, "%s : phy on takes %u (ms).\n"
			  	, __func__, phyon_t);
	}
#endif /* DBG_PHY_ON_TIME */

	if (is_cmd_failure(sts)) {
		/* send cmd success, but wait cmd fail */
		sts = RTW_PHL_STATUS_FAILURE;
	} else if (sts != RTW_PHL_STATUS_SUCCESS) {
		/* send cmd fail */
		sts = RTW_PHL_STATUS_FAILURE;
	}
	return sts;
}

void rtw_phl_stop(void *phl)
{
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;

	PHL_INFO("%s\n", __func__);

	_phl_cmd_send_msg_phy_on(phl_info);

	if (rtw_hal_is_inited(phl_info->phl_com, phl_info->hal))
		_phl_interrupt_stop(phl_info);

	phl_module_stop(phl_info);

#ifdef DBG_PHL_MR
	phl_mr_info_dbg(phl_info);
#endif

#ifdef CONFIG_FSM
	phl_fsm_module_stop(phl_info);
	phl_fsm_stop(phl_info);
#endif

	if (rtw_hal_is_inited(phl_info->phl_com, phl_info->hal)) {
		phl_datapath_stop_sw(phl_info, PHL_MDL_PHY_MGNT);
		phl_datapath_stop_hw(phl_info);
		rtw_hal_stop(phl_info->phl_com, phl_info->hal);
		phl_datapath_reset(phl_info, PHL_CTRL_TX | PHL_CTRL_RX);
	}

	phl_free_deferred_tx_ring(phl_info);

	phl_info->phl_com->dev_state = 0;
}

enum rtw_phl_status phl_wow_start(struct phl_info_t *phl_info, struct rtw_phl_stainfo_t *sta)
{
#ifdef CONFIG_WOWLAN
	enum rtw_phl_status pstatus = RTW_PHL_STATUS_FAILURE;
	enum rtw_hal_status hstatus = RTW_HAL_STATUS_FAILURE;
	struct phl_wow_info *wow_info = phl_to_wow_info(phl_info);

#ifdef CONFIG_SYNC_INTERRUPT
	struct rtw_phl_evt_ops *evt_ops = &phl_info->phl_com->evt_ops;
#endif /* CONFIG_SYNC_INTERRUPT */

	/*
	 * How to do wow on MLD
	 */
	PHL_TRACE(COMP_PHL_WOW, _PHL_INFO_, "[wow] %s enter with sta state(%d)\n.", __func__, sta->wrole->mstate);

	phl_wow_decide_op_mode(wow_info, sta);

	if (wow_info->op_mode == RTW_WOW_OP_PWR_DOWN) {
		phl_cmd_role_suspend(phl_info, PHL_ROLE_SUSPEND_RSN_DEV_SUSP);
		rtw_phl_stop(phl_info);
		/* since control path stopped after rtw_phl_stop,
		   below action don't have to migrate to general module */
		hstatus = rtw_hal_set_wowlan(phl_info->phl_com, phl_info->hal, true);
		if (RTW_HAL_STATUS_SUCCESS != hstatus)
			PHL_WARN("[wow] rtw_hal_set_wowlan failed, status(%u)\n", hstatus);
		pstatus = RTW_PHL_STATUS_SUCCESS;
	} else {
		/* stop all active features */
		#ifdef CONFIG_WOW_WITH_SER
		rtw_hal_ser_ctrl(phl_info->hal, HAL_SER_RSN_WOW, false);
		#endif
		phl_cmd_role_suspend(phl_info, PHL_ROLE_SUSPEND_RSN_WOW);
		pstatus = phl_module_stop(phl_info);
		if (RTW_PHL_STATUS_SUCCESS != pstatus) {
			PHL_ERR("[wow] phl_module_stop failed.\n");
			goto end;
		}
		/* since control path stopped after phl_module_stop,
		   below action don't have to migrate to general module */
#ifdef CONFIG_FSM
		pstatus = phl_fsm_module_stop(phl_info);
		if (RTW_PHL_STATUS_SUCCESS != pstatus) {
			PHL_ERR("[wow] phl_fsm_module_stop failed.\n");
			goto end;
		}

		pstatus = phl_fsm_stop(phl_info);
		if (RTW_PHL_STATUS_SUCCESS != pstatus) {
			PHL_ERR("[wow] phl_fsm_stop failed.\n");
			goto end;
		}
#endif
		hstatus = rtw_hal_set_wowlan(phl_info->phl_com, phl_info->hal, true);
		if (RTW_HAL_STATUS_SUCCESS != hstatus)
			PHL_WARN("[wow] rtw_hal_set_wowlan failed, status(%u)\n", hstatus);
		pstatus = phl_wow_init_precfg(wow_info);
		if (RTW_PHL_STATUS_SUCCESS != pstatus) {
			PHL_ERR("[wow] phl_wow_init_precfg failed.\n");
			goto end;
		}

		pstatus = phl_wow_init(wow_info);
		if (RTW_PHL_STATUS_SUCCESS != pstatus)
			goto end;

		pstatus = phl_wow_func_en(wow_info);
		if (RTW_PHL_STATUS_SUCCESS != pstatus)
			goto end;
#ifdef CONFIG_POWER_SAVE
		/* power saving */
		phl_wow_ps_proto_cfg(wow_info, true);

		phl_wow_ps_pwr_ntfy(wow_info, true);
#endif
		pstatus = phl_wow_init_postcfg(wow_info);
		if (RTW_PHL_STATUS_SUCCESS != pstatus) {
			PHL_ERR("[wow] phl_wow_init_postcfg failed.\n");
			goto end;
		}
		#ifdef CONFIG_WOW_WITH_SER
		rtw_hal_ser_ctrl(phl_info->hal, HAL_SER_RSN_WOW, true);
		#endif
#ifdef CONFIG_POWER_SAVE
		/* power saving */
		phl_wow_ps_pwr_cfg(wow_info, true);
#endif
		pstatus = RTW_PHL_STATUS_SUCCESS;
	}

end:
	if (RTW_PHL_STATUS_SUCCESS != pstatus) {
		#ifdef CONFIG_SYNC_INTERRUPT
		evt_ops->set_interrupt_caps(phl_to_drvpriv(phl_info), false);
		#else
		rtw_hal_disable_interrupt(phl_info->phl_com, phl_info->hal);
		#endif /* CONFIG_SYNC_INTERRUPT */
		phl_role_suspend(phl_info, PHL_ROLE_SUSPEND_RSN_DEV_SUSP);
		phl_datapath_stop_sw(phl_info, PHL_MDL_PHY_MGNT);
		phl_datapath_stop_hw(phl_info);
		rtw_hal_stop(phl_info->phl_com, phl_info->hal);
		phl_datapath_reset(phl_info, PHL_CTRL_TX | PHL_CTRL_RX);
		phl_free_deferred_tx_ring(phl_info);

		wow_info->op_mode = RTW_WOW_OP_PWR_DOWN;
		PHL_ERR("[wow] %s fail, set op_mode %d!\n", __func__, wow_info->op_mode);
	} else {
		PHL_TRACE(COMP_PHL_WOW, _PHL_INFO_,
			"[wow] %s success, with func_en %d, op_mode %d.\n",
			__func__, wow_info->func_en, wow_info->op_mode);
	}

	return pstatus;
#else
	return RTW_PHL_STATUS_SUCCESS;
#endif /* CONFIG_WOWLAN */
}

static void _wow_stop_reinit(struct phl_info_t *phl_info)
{
	enum rtw_phl_status pstatus = RTW_PHL_STATUS_FAILURE;

	PHL_WARN("%s : reset hw!\n", __func__);
	phl_role_suspend(phl_info, PHL_ROLE_SUSPEND_RSN_DEV_SUSP);
	phl_datapath_stop_sw(phl_info, PHL_MDL_PHY_MGNT);
	phl_datapath_stop_hw(phl_info);
	rtw_hal_stop(phl_info->phl_com, phl_info->hal);
	phl_datapath_reset(phl_info, PHL_CTRL_TX | PHL_CTRL_RX);
	phl_free_deferred_tx_ring(phl_info);
	pstatus = rtw_phl_start(phl_info);
	if (pstatus)
		PHL_ERR("%s : rtw_phl_start fail!\n", __func__);
	phl_cmd_role_recover(phl_info);
}

void phl_wow_stop(struct phl_info_t *phl_info, struct rtw_phl_stainfo_t *sta, u8 *hw_reinit)
{
#ifdef CONFIG_WOWLAN
	enum rtw_phl_status pstatus = RTW_PHL_STATUS_FAILURE;
	enum rtw_hal_status hstatus = RTW_HAL_STATUS_FAILURE;
	struct phl_wow_info *wow_info = phl_to_wow_info(phl_info);
	u8 reset = 0;

	if (rtw_hal_get_pwr_state(phl_info->hal, &wow_info->mac_pwr)
		!= RTW_HAL_STATUS_SUCCESS)
		return;

	PHL_TRACE(COMP_PHL_WOW, _PHL_INFO_, "%s enter with mac power %d\n.",
			  __func__, wow_info->mac_pwr);

	if (wow_info->mac_pwr != RTW_MAC_PWR_OFF) {
		#ifdef CONFIG_WOW_WITH_SER
		rtw_hal_ser_ctrl(phl_info->hal, HAL_SER_RSN_WOW, false);
		#endif
		#ifdef CONFIG_POWER_SAVE
		/* leave clock/power gating */
		pstatus = phl_wow_ps_pwr_cfg(wow_info, false);
		if (RTW_PHL_STATUS_SUCCESS != pstatus) {
			PHL_ERR("[wow] HW leave power saving failed.\n");
			goto reinit;
		}
		#endif
	}

	hstatus = rtw_hal_set_wowlan(phl_info->phl_com, phl_info->hal, false);
	if (RTW_HAL_STATUS_SUCCESS != hstatus) {
		PHL_WARN("[wow] rtw_hal_set_wowlan failed, status(%u)\n", hstatus);
	}

	if (wow_info->mac_pwr == RTW_MAC_PWR_OFF) {
		if (wow_info->op_mode == RTW_WOW_OP_PWR_DOWN) {
			pstatus = rtw_phl_start(phl_info);
			phl_role_recover(phl_info);
			*hw_reinit = true;
		} else {
			PHL_WARN("[wow] enter suspend with wow enabled but mac is power down\n");
			goto reinit;
		}
	} else if (wow_info->mac_pwr == RTW_MAC_PWR_ON ||
			   wow_info->mac_pwr == RTW_MAC_PWR_LPS) {

		phl_wow_handle_wake_rsn(wow_info, &reset);
		if (reset)
			goto reinit;

		phl_module_start(phl_info);
#ifdef CONFIG_FSM
		phl_fsm_start(phl_info);
		phl_fsm_module_start(phl_info);
#endif

		phl_wow_deinit_precfg(wow_info);

		rtw_hal_fw_dbg_dump(phl_info->hal);
#ifdef CONFIG_POWER_SAVE
		phl_wow_ps_pwr_ntfy(wow_info, false);
		/* leave power saving */
		phl_wow_ps_proto_cfg(wow_info, false);
#endif
		phl_wow_func_dis(wow_info);

		pstatus = phl_wow_deinit(wow_info);
		if (pstatus != RTW_PHL_STATUS_SUCCESS)
			goto reinit;

		phl_wow_deinit_postcfg(wow_info);
		phl_cmd_role_recover(phl_info);
		#ifdef CONFIG_WOW_WITH_SER
		rtw_hal_ser_ctrl(phl_info->hal, HAL_SER_RSN_WOW, true);
		#endif
		*hw_reinit = false;
	} else {
		PHL_ERR("%s : unexpected mac pwr state %d.\n", __func__, wow_info->mac_pwr);
	}

	return;
reinit:
	_wow_stop_reinit(phl_info);
	*hw_reinit = true;
	return;

#endif /* CONFIG_WOWLAN */
}

enum rtw_phl_status rtw_phl_rf_on(void *phl)
{
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;
	enum rtw_phl_status phl_status = RTW_PHL_STATUS_FAILURE;
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_SUCCESS;
#ifdef CONFIG_SYNC_INTERRUPT
	struct rtw_phl_evt_ops *evt_ops = &phl_info->phl_com->evt_ops;
#endif /* CONFIG_SYNC_INTERRUPT */

	PHL_INFO("%s\n", __func__);

	phl_datapath_reset(phl_info, PHL_CTRL_IN_PIPE | PHL_CTRL_OUT_PIPE);

	hal_status = rtw_hal_start(phl_info->phl_com, phl_info->hal);
	if (hal_status == RTW_HAL_STATUS_MAC_INIT_FAILURE) {
		phl_status = RTW_PHL_STATUS_HAL_INIT_FAILURE;
		goto error_hal_start;
	} else if (hal_status == RTW_HAL_STATUS_BB_INIT_FAILURE) {
		phl_status = RTW_PHL_STATUS_HAL_INIT_FAILURE;
		goto error_hal_start;
	} else if (hal_status == RTW_HAL_STATUS_RF_INIT_FAILURE) {
		phl_status = RTW_PHL_STATUS_HAL_INIT_FAILURE;
		goto error_hal_start;
	} else if (hal_status == RTW_HAL_STATUS_BTC_INIT_FAILURE) {
		phl_status = RTW_PHL_STATUS_HAL_INIT_FAILURE;
		goto error_hal_start;
	}

	phl_datapath_start_hw(phl_info);
	phl_datapath_start_sw(phl_info, PHL_MDL_POWER_MGNT);

#ifdef CONFIG_SYNC_INTERRUPT
	evt_ops->set_interrupt_caps(phl_to_drvpriv(phl_info), true);
#else
	rtw_hal_enable_interrupt(phl_info->phl_com, phl_info->hal);
#endif /* CONFIG_SYNC_INTERRUPT */

	phl_role_recover(phl_info);

	return RTW_PHL_STATUS_SUCCESS;
error_hal_start:
	PHL_ERR("error_hal_start\n");
	return phl_status;
}

enum rtw_phl_status rtw_phl_rf_off(void *phl)
{
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;
#ifdef CONFIG_SYNC_INTERRUPT
	struct rtw_phl_evt_ops *evt_ops = &phl_info->phl_com->evt_ops;
#endif /* CONFIG_SYNC_INTERRUPT */

	PHL_INFO("%s\n", __func__);

	phl_role_suspend(phl_info, PHL_ROLE_SUSPEND_RSN_RF_OFF);

#ifdef CONFIG_SYNC_INTERRUPT
	evt_ops->set_interrupt_caps(phl_to_drvpriv(phl_info), false);
#else
	rtw_hal_disable_interrupt(phl_info->phl_com, phl_info->hal);
#endif /* CONFIG_SYNC_INTERRUPT */

	phl_datapath_stop_sw(phl_info, PHL_MDL_POWER_MGNT);
	phl_datapath_stop_hw(phl_info);
	rtw_hal_stop(phl_info->phl_com, phl_info->hal);
	phl_datapath_reset(phl_info, PHL_CTRL_TX | PHL_CTRL_RX);

	return RTW_PHL_STATUS_SUCCESS;
}

#ifdef CONFIG_PHL_HANDLE_SER_L2
enum rtw_phl_status rtw_phl_suspend_all_sta(void *phl, enum phl_cmd_type cmd_type)
{
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;
	struct mr_ctl_t *mr_ctl = phlcom_to_mr_ctrl(phl_info->phl_com);
	u8 role_idx;
	struct rtw_wifi_role_t *wrole;
	enum rtw_phl_status pstatus;
	struct rtw_phl_stainfo_t *n, *psta, *sta_self;
	void *drv = phl_to_drvpriv(phl_info);

	for (role_idx = 0; role_idx < MAX_WIFI_ROLE_NUMBER; role_idx++) {
		if (!(mr_ctl->role_map & BIT(role_idx)))
			continue;
		wrole = phl_get_wrole_by_ridx(phl_info, role_idx);
		if (wrole == NULL)
			continue;
		sta_self = rtw_phl_get_stainfo_self(phl_info, &wrole->rlink[RTW_RLINK_PRIMARY]);
		if (sta_self == NULL)
			continue;
		phl_list_for_loop_safe(psta, n, struct rtw_phl_stainfo_t,
			       &wrole->rlink[RTW_RLINK_PRIMARY].assoc_sta_queue.queue, list) {
			if(psta == NULL)
				continue;
			if(psta == sta_self)
				continue;
			if (!psta->active)
				continue;

			/* record STA is associated or not */
			psta->l2recover_connect_bk = rtw_hal_is_sta_linked(phl_info->hal, psta);
			/* record STA's assoc rssi */
			psta->l2recover_assoc_rssi_bk = psta->hal_sta->rssi_stat.assoc_rssi;
			/* record STA's assoc cap */
			_os_mem_cpy(drv, &psta->l2recover_asoc_cap_bk, &psta->asoc_cap, sizeof(struct protocol_cap_t));
			pstatus = rtw_phl_cmd_update_media_status(
				phl, psta, psta->mac_addr, false,
				cmd_type, 0);
			if (pstatus != RTW_PHL_STATUS_SUCCESS)
				PHL_ERR("%s(%d) failed\n", __func__, __LINE__);
		}
	}
exit:
	return pstatus;
}

enum rtw_phl_status rtw_phl_resume_all_sta(void *phl, enum phl_cmd_type cmd_type)
{
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;
	struct mr_ctl_t *mr_ctl = phlcom_to_mr_ctrl(phl_info->phl_com);
	u8 role_idx;
	struct rtw_wifi_role_t *wrole;
	enum rtw_phl_status pstatus;
	struct rtw_phl_stainfo_t *n, *psta, *sta_self;
	void *drv = phl_to_drvpriv(phl_info);

	for (role_idx = 0; role_idx < MAX_WIFI_ROLE_NUMBER; role_idx++) {
		if (!(mr_ctl->role_map & BIT(role_idx)))
			continue;
		wrole = phl_get_wrole_by_ridx(phl_info, role_idx);
		if(wrole == NULL)
			continue;
		sta_self = rtw_phl_get_stainfo_self(phl_info, &wrole->rlink[RTW_RLINK_PRIMARY]);
		if(sta_self == NULL)
			continue;
		phl_list_for_loop_safe(psta, n, struct rtw_phl_stainfo_t,
			       &wrole->rlink[RTW_RLINK_PRIMARY].assoc_sta_queue.queue, list) {
			if(psta == NULL)
				continue;
			if(psta == sta_self)
				continue;
			if (!psta->active)
				continue;

			psta->hal_sta->rssi_stat.assoc_rssi = psta->l2recover_assoc_rssi_bk;
			_os_mem_cpy(drv, &psta->asoc_cap, &psta->l2recover_asoc_cap_bk, sizeof(struct protocol_cap_t));
			pstatus = rtw_phl_cmd_alloc_stainfo(phl,
								&psta,
								(u8 *)psta->mac_addr,
								wrole,
								DEV_TYPE_INACTIVE,
								0,
								&wrole->rlink[RTW_RLINK_PRIMARY],
								_TRUE,
								_TRUE,
								cmd_type,
								0);
			if (pstatus != RTW_PHL_STATUS_SUCCESS) {
				PHL_ERR("%s(%d) failed\n", __func__, __LINE__);
				continue;
			}
			/* if STA is not originally associated, restore hw stainfo only */
			if (psta->l2recover_connect_bk == false) {
				PHL_WARN("macid %d not associated, skip!\n", psta->macid);
				continue;
			}

			pstatus = rtw_phl_cmd_update_media_status(
					phl, psta, psta->mac_addr, true,
					cmd_type, 0);
			if (pstatus != RTW_PHL_STATUS_SUCCESS) {
				PHL_ERR("%s(%d) failed\n", __func__, __LINE__);
			}
		}
	}
	return RTW_PHL_STATUS_SUCCESS;
}
#endif /* CONFIG_PHL_HANDLE_SER_L2 */

enum rtw_phl_status rtw_phl_pnp_stop(void *phl)
{
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;
#ifdef CONFIG_PCI_HCI
	struct phl_hci_trx_ops *trx_ops = phl_info->hci_trx_ops;
#endif
#ifdef CONFIG_SYNC_INTERRUPT
	struct rtw_phl_evt_ops *ops = &phl_info->phl_com->evt_ops;

	ops->set_interrupt_caps(phl_to_drvpriv(phl_info), false);
#else
	rtw_hal_disable_interrupt(phl_info->phl_com, phl_info->hal);
#endif /* CONFIG_SYNC_INTERRUPT */

	rtw_hal_cfg_txhci(phl_info->hal, false);
	rtw_hal_cfg_rxhci(phl_info->hal, false);
#ifdef CONFIG_PCI_HCI
	rtw_hal_cfg_dma_io(phl_info->hal, false);
	trx_ops->return_tx_wps(phl_info);
#endif
	return RTW_PHL_STATUS_SUCCESS;
}

enum rtw_phl_status rtw_phl_suspend(void *phl, struct rtw_phl_stainfo_t *sta, u8 wow_en)
{
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;
	enum rtw_phl_status pstatus = RTW_PHL_STATUS_SUCCESS;

	PHL_INFO("%s enter with wow_en(%d)\n.", __func__, wow_en);
#ifdef CONFIG_WOWLAN
	pstatus = _phl_cmd_send_msg_phy_on(phl_info);
	if (RTW_PHL_STATUS_SUCCESS != pstatus) {
		PHL_ERR("[wow] _phl_cmd_send_msg_phy_on fail!\n");
		wow_en = false;
	}

	if (wow_en) {
		pstatus = phl_wow_start(phl_info, sta);
	} else {
		phl_cmd_role_suspend(phl_info, PHL_ROLE_SUSPEND_RSN_DEV_SUSP);
		rtw_phl_stop(phl);
	}
#else
	PHL_INFO("%s enter with wow_en(%d)\n.", __func__, wow_en);

	phl_cmd_role_suspend(phl_info, PHL_ROLE_SUSPEND_RSN_DEV_SUSP);
	rtw_phl_stop(phl);
#endif

	FUNCOUT_WSTS(pstatus);

	return pstatus;
}

enum rtw_phl_status rtw_phl_resume(void *phl, struct rtw_phl_stainfo_t *sta, u8 *hw_reinit)
{
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;
	enum rtw_phl_status pstatus = RTW_PHL_STATUS_SUCCESS;
#ifdef CONFIG_WOWLAN
	struct phl_wow_info *wow_info = phl_to_wow_info(phl_info);
#endif

	/**
	 * Since some platforms require performance when device resuming, we need
	 * to finish "rtw_phl_resume" as fast as possible. In this situation, we
	 * prevent ps module entering any power saving mechanisms and try to do I/O
	 * operations directly without issue commands to cmd dispatcher. Therefore,
	 * ps module will not enter power saving if device state "RTW_DEV_RESUMING"
	 * is set. If device state "RTW_DEV_RESUMING" is set, operations of I/O
	 * should check whether the current power state can perform I/O or not and
	 * perform I/O directly without issuing commands to cmd dispatcher if device
	 * power state is I/O allowable. This kind of flow is only suitable for
	 * "rtw_phl_resume" because core layer will not perform any other tasks when
	 * calling rtw_phl_resume which is relatively simple enough.
	 */
	PHL_INFO("%s enter...\n.", __func__);
	SET_STATUS_FLAG(phl_info->phl_com->dev_state, RTW_DEV_RESUMING);

#ifdef CONFIG_WOWLAN
	if (wow_info->op_mode != RTW_WOW_OP_NONE) {
		phl_wow_stop(phl_info, sta, hw_reinit);
	} else {
		pstatus = rtw_phl_start(phl);
		#ifdef CONFIG_POWER_SAVE
		if (phl_ps_get_cur_pwr_lvl(phl_info) == PS_PWR_LVL_PWRON)
		#endif
			phl_role_recover(phl_info);
		*hw_reinit = true;
	}
	#if defined(RTW_WKARD_WOW_L2_PWR) && defined(CONFIG_PCI_HCI)
	rtw_hal_set_l2_leave(phl_info->hal);
	#endif
	phl_record_wow_stat(wow_info);
	phl_reset_wow_info(wow_info);
#else
	pstatus = rtw_phl_start(phl);
	#ifdef CONFIG_POWER_SAVE
	if (phl_ps_get_cur_pwr_lvl(phl_info) == PS_PWR_LVL_PWRON)
	#endif
		phl_role_recover(phl_info);
	*hw_reinit = true;
#endif

	CLEAR_STATUS_FLAG(phl_info->phl_com->dev_state, RTW_DEV_RESUMING);

	PHL_INFO("%s exit with hw_reinit %d.\n.", __func__, *hw_reinit);

	return pstatus;
}

enum rtw_phl_status rtw_phl_reset(void *phl)
{
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;
	enum rtw_phl_status phl_status = RTW_PHL_STATUS_SUCCESS;

	phl_status = phl_cmd_role_suspend(phl_info, PHL_ROLE_SUSPEND_RSN_DEV_SUSP);
	if (phl_status != RTW_PHL_STATUS_SUCCESS) {
		PHL_WARN("%s: role suspend fail!\n", __func__);
		goto exit;
	}

	phl_datapath_stop_sw(phl_info, PHL_MDL_PHY_MGNT);
	phl_datapath_stop_hw(phl_info);

	rtw_hal_stop(phl_info->phl_com, phl_info->hal);

	phl_datapath_reset(phl_info, PHL_CTRL_TX | PHL_CTRL_RX |
	                   PHL_CTRL_IN_PIPE | PHL_CTRL_OUT_PIPE);
	phl_free_deferred_tx_ring(phl_info);

	if (rtw_hal_start(phl_info->phl_com, phl_info->hal) !=
	    RTW_HAL_STATUS_SUCCESS) {
		PHL_WARN("%s: start hal fail!\n", __func__);
		phl_status = RTW_PHL_STATUS_HAL_INIT_FAILURE;
		goto exit;
	}

	phl_status = phl_datapath_start_hw(phl_info);
	if (phl_status != RTW_PHL_STATUS_SUCCESS) {
		PHL_WARN("%s: start hw datapath fail!\n", __func__);
		goto exit;
	}

	phl_status = phl_datapath_start_sw(phl_info, PHL_MDL_PHY_MGNT);
	if (phl_status != RTW_PHL_STATUS_SUCCESS) {
		PHL_WARN("%s: start sw datapath fail!\n", __func__);
		goto exit;
	}

	phl_status = phl_cmd_role_recover(phl_info);
	if (phl_status != RTW_PHL_STATUS_SUCCESS) {
		PHL_WARN("%s: role recover fail!\n", __func__);
		goto exit;
	}
exit:
	return phl_status;
}

enum rtw_phl_status rtw_phl_restart(void *phl)
{
	enum rtw_phl_status phl_status = RTW_PHL_STATUS_FAILURE;

	phl_status = RTW_PHL_STATUS_SUCCESS;

	return phl_status;
}


/******************* IO  APIs *******************/
u8 rtw_phl_read8(void *phl, u32 addr)
{
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;

	return rtw_hal_read8(phl_info->hal, addr);
}
u16 rtw_phl_read16(void *phl, u32 addr)
{
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;

	return rtw_hal_read16(phl_info->hal, addr);
}
u32 rtw_phl_read32(void *phl, u32 addr)
{
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;

	return rtw_hal_read32(phl_info->hal, addr);
}
void rtw_phl_write8(void *phl, u32 addr, u8 val)
{
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;

	rtw_hal_write8(phl_info->hal, addr, val);
}
void rtw_phl_write16(void *phl, u32 addr, u16 val)
{
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;

	rtw_hal_write16(phl_info->hal, addr, val);
}
void rtw_phl_write32(void *phl, u32 addr, u32 val)
{
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;

	rtw_hal_write32(phl_info->hal, addr, val);
}
u32 rtw_phl_read_macreg(void *phl, u32 offset, u32 bit_mask)
{
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;

	return rtw_hal_read_macreg(phl_info->hal, offset, bit_mask);
}
void rtw_phl_write_macreg(void *phl,
			u32 offset, u32 bit_mask, u32 data)
{
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;

	rtw_hal_write_macreg(phl_info->hal, offset, bit_mask, data);

}
u32 rtw_phl_read_bbreg(void *phl, u32 offset, u32 bit_mask)
{
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;

	return rtw_hal_read_bbreg(phl_info->hal, offset, bit_mask);
}
void rtw_phl_write_bbreg(void *phl,
			u32 offset, u32 bit_mask, u32 data)
{
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;

	rtw_hal_write_bbreg(phl_info->hal, offset, bit_mask, data);

}
u32 rtw_phl_read_rfreg(void *phl,
			enum rf_path path, u32 offset, u32 bit_mask)
{
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;

	return rtw_hal_read_rfreg(phl_info->hal, path, offset, bit_mask);
}
void rtw_phl_write_rfreg(void *phl,
			enum rf_path path, u32 offset, u32 bit_mask, u32 data)
{
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;

	rtw_hal_write_rfreg(phl_info->hal, path, offset, bit_mask, data);

}

void rtw_phl_restore_interrupt(void *phl)
{
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;
	rtw_hal_restore_interrupt(phl_info->phl_com, phl_info->hal);
}

enum rtw_phl_status
_phl_gtimer_event(struct gtimer_ctx *gt_ctx, u8 *skip_tx)
{
	enum rtw_phl_status phl_status = RTW_PHL_STATUS_SUCCESS;

	switch(gt_ctx->timer_type) {
		case _GT3_TYPE_SH_TASK:
			phl_status = RTW_PHL_STATUS_SH_TASK;
			*skip_tx = 1;
			break;
		default:
			break;
	}
	return phl_status;
}

enum rtw_phl_status rtw_phl_interrupt_handler(void *phl)
{
	enum rtw_phl_status phl_status = RTW_PHL_STATUS_SUCCESS;
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;
	u32 int_hdler_msk = 0x0;
#ifdef CONFIG_SYNC_INTERRUPT
	struct rtw_phl_evt_ops *ops = &phl_info->phl_com->evt_ops;
#endif /* CONFIG_SYNC_INTERRUPT */
	struct gtimer_ctx *gt_ctx = (struct gtimer_ctx *)&phl_info->gt3_ctx;
	u8 skip_tx = 0;

	int_hdler_msk = rtw_hal_interrupt_handler(phl_info->hal);

	if (!int_hdler_msk) {
		PHL_WARN("%s : 0x%x\n", __func__, int_hdler_msk);
		phl_status = RTW_PHL_STATUS_FAILURE;
		goto end;
	}

	PHL_DBG("%s : 0x%x\n", __func__, int_hdler_msk);
	/* beacon interrupt */
	if (int_hdler_msk & BIT0)
		;/* todo */

	/* rx interrupt */
	if (int_hdler_msk & BIT1) {
#ifdef DEBUG_PHL_RX
		phl_info->rx_stats.rx_isr++;
#endif
#if defined(CONFIG_SDIO_HCI) && defined(CONFIG_PHL_SDIO_READ_RXFF_IN_INT)
		phl_info->hci_trx_ops->recv_rxfifo(phl);
#else
		phl_status = rtw_phl_start_rx_process(phl);
#endif

#if defined(CONFIG_PCI_HCI) && !defined(CONFIG_DYNAMIC_RX_BUF)
		/* phl_status = hci_trx_ops->recycle_busy_wd(phl); */
#endif
	}

	/* tx interrupt */
	if (int_hdler_msk & BIT2)
		;

	/* cmd interrupt */
	if (int_hdler_msk & BIT3)
		;/* todo */

	/* halt c2h interrupt */
	if (int_hdler_msk & BIT4)
		phl_status = phl_ser_send_msg(phl, RTW_PHL_SER_EVENT_CHK);

	/* halt c2h interrupt */
	if (int_hdler_msk & BIT5)
		phl_status = phl_fw_watchdog_timeout_notify(phl);

	/* halt c2h interrupt - send msg to SER FSM to check ser event */
	if (int_hdler_msk & BIT6)
		phl_status = phl_ser_send_msg(phl, RTW_PHL_SER_EVENT_CHK);

	/* gt3 interrupt - gt3 hw timer timeout
		put the check to the last if BIT8, BIT9, etc are added
	*/
	if (int_hdler_msk & BIT7)
		phl_status = _phl_gtimer_event(gt_ctx, &skip_tx);

	if (skip_tx)
		goto end;

	if (phl_status != RTW_PHL_STATUS_SUCCESS)
		PHL_INFO("rtw_phl_interrupt_handler fail !!\n");

	/* schedule tx process */
	phl_status = phl_schedule_handler(phl_info->phl_com, &phl_info->phl_tx_handler);
end:

#ifdef CONFIG_SYNC_INTERRUPT
	ops->interrupt_restore(phl_to_drvpriv(phl_info), false);
#endif
	return phl_status;
}

void rtw_phl_enable_interrupt(void *phl)
{
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;
	rtw_hal_enable_interrupt(phl_info->phl_com, phl_info->hal);
}

void rtw_phl_disable_interrupt_isr(void *phl)
{
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;
	rtw_hal_disable_interrupt_isr(phl_info->phl_com, phl_info->hal);
}

void rtw_phl_disable_interrupt(void *phl)
{
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;
	rtw_hal_disable_interrupt(phl_info->phl_com, phl_info->hal);
}

bool rtw_phl_recognize_interrupt(void *phl)
{
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;

	return rtw_hal_recognize_interrupt(phl_info->hal);
}

void rtw_phl_clear_interrupt(void *phl)
{
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;

	rtw_hal_clear_interrupt(phl_info->hal);
}

enum rtw_phl_status rtw_phl_msg_hub_register_recver(void* phl,
		struct phl_msg_receiver* ctx, enum phl_msg_recver_layer layer)
{
	return phl_msg_hub_register_recver(phl, ctx, layer);
}
enum rtw_phl_status rtw_phl_msg_hub_update_recver_mask(void* phl,
		enum phl_msg_recver_layer layer, u8* mdl_id, u8 len, u8 clr)
{
	return phl_msg_hub_update_recver_mask(phl, layer, mdl_id, len, clr);
}
enum rtw_phl_status rtw_phl_msg_hub_deregister_recver(void* phl,
					enum phl_msg_recver_layer layer)
{
	return phl_msg_hub_deregister_recver(phl, layer);
}
enum rtw_phl_status rtw_phl_msg_hub_send(void* phl,
			struct phl_msg_attribute* attr, struct phl_msg* msg)
{
	return phl_msg_hub_send((struct phl_info_t*)phl, attr, msg);
}
#ifdef PHL_PLATFORM_LINUX
void rtw_phl_mac_reg_dump(void *sel, void *phl)
{
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;

	rtw_hal_mac_reg_dump(sel, phl_info->hal);
}

void rtw_phl_bb_reg_dump(void *sel, void *phl)
{
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;

	rtw_hal_bb_reg_dump(sel, phl_info->hal);
}

void rtw_phl_bb_reg_dump_ex(void *sel, void *phl)
{
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;

	rtw_hal_bb_reg_dump_ex(sel, phl_info->hal);
}

void rtw_phl_rf_reg_dump(void *sel, void *phl)
{
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;

	rtw_hal_rf_reg_dump(sel, phl_info->hal);
}
#endif

/**
 * rtw_phl_get_sec_cam() - get the security cam raw data from HW
 * @phl:		struct phl_info_t *
 * @num:		How many cam you wnat to dump from the first one.
 * @buf:		ptr to buffer which store the content from HW.
 *			If buf is NULL, use console as debug path.
 * @size		Size of allocated memroy for @buf.
 *			The size should be @num * size of security cam offset(0x20).
 *
 * Return true when function successfully works, otherwise, return fail.
 */
bool rtw_phl_get_sec_cam(void *phl, u16 num, u8 *buf, u16 size)
{
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;
	enum rtw_hal_status ret = RTW_HAL_STATUS_SUCCESS;

	ret = rtw_hal_get_sec_cam(phl_info->hal, num, buf, size);
	if (ret != RTW_HAL_STATUS_SUCCESS)
		return false;

	return true;
}

/**
 * rtw_phl_get_addr_cam() - get the address cam raw data from HW
 * @phl:		struct phl_info_t *
 * @num:		How many cam you wnat to dump from the first one.
 * @buf:		ptr to buffer which store the content from HW.
 *			If buf is NULL, use console as debug path.
 * @size		Size of allocated memroy for @buf.
 *			The size should be @num * size of Addr cam offset(0x40).
 *
 * Return true when function successfully works, otherwise, return fail.
 */
bool rtw_phl_get_addr_cam(void *phl, u16 num, u8 *buf, u16 size)
{
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;
	enum rtw_hal_status ret = RTW_HAL_STATUS_SUCCESS;

	ret = rtw_hal_get_addr_cam(phl_info->hal, num, buf, size);
	if (ret != RTW_HAL_STATUS_SUCCESS)
		return false;

	return true;
}

void rtw_phl_mac_dbg_status_dump(void *phl, u32 *val, u8 *en)
{
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;
	struct hal_mac_dbg_dump_cfg cfg = {0};

	cfg.ss_dbg_0 = val[0];
	cfg.ss_dbg_1 = val[1];

	cfg.ss_dbg = (*en & BIT0);
	cfg.dle_dbg = (*en & BIT1) >> 1;
	cfg.dmac_dbg = (*en & BIT2) >> 2;
	cfg.cmac_dbg = (*en & BIT3) >> 3;
	cfg.mac_dbg_port = (*en & BIT4) >> 4;
	cfg.plersvd_dbg = (*en & BIT5) >> 5;
	cfg.tx_flow_dbg = (*en & BIT6) >> 6;

	rtw_hal_dbg_status_dump(phl_info->hal, &cfg);
}

enum rtw_phl_status rtw_phl_get_mac_addr_efuse(void* phl, u8 *addr)
{
	enum rtw_phl_status pstatus = RTW_PHL_STATUS_SUCCESS;
	enum rtw_hal_status hstatus = RTW_HAL_STATUS_SUCCESS;
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;
	void *d = phl_to_drvpriv(phl_info);
	u8 addr_efuse[MAC_ADDRESS_LENGTH] = {0};

	hstatus = rtw_hal_get_efuse_info(phl_info->hal,
			EFUSE_INFO_MAC_ADDR,
			(void *)addr_efuse,
			MAC_ADDRESS_LENGTH);
	if (is_broadcast_mac_addr(addr_efuse)) {
		PHL_INFO("[WARNING] MAC Address from EFUSE is FF:FF:FF:FF:FF:FF\n");
		hstatus = RTW_HAL_STATUS_FAILURE;
	}
	if (RTW_HAL_STATUS_SUCCESS != hstatus) {
		pstatus = RTW_PHL_STATUS_FAILURE;
	} else {
		_os_mem_cpy(d, addr, addr_efuse, MAC_ADDRESS_LENGTH);
		PHL_INFO("%s : 0x%2x - 0x%2x - 0x%2x - 0x%2x - 0x%2x - 0x%2x\n",
			 __func__, addr[0], addr[1], addr[2],
			 addr[3], addr[4], addr[5]);

	}
	return pstatus;
}

enum rtw_phl_status
rtw_phl_cfg_trx_path(void* phl, enum rf_path tx, u8 tx_nss,
		     enum rf_path rx, u8 rx_nss)
{
	enum rtw_phl_status pstatus = RTW_PHL_STATUS_SUCCESS;
	enum rtw_hal_status hstatus = RTW_HAL_STATUS_SUCCESS;
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;

	hstatus = rtw_hal_cfg_trx_path(phl_info->hal, tx, tx_nss, rx, rx_nss);

	if (RTW_HAL_STATUS_SUCCESS != hstatus)
		pstatus = RTW_PHL_STATUS_FAILURE;

	return pstatus;
}


void rtw_phl_reset_stat_ma_rssi(struct rtw_phl_com_t *phl_com)
{
	u8 i = 0, j = 0;
	PHL_INFO("--> %s\n", __func__);
	do{
		if (NULL == phl_com)
			break;

		_os_spinlock(phl_com->drv_priv,
			     &(phl_com->rssi_stat.lock), _bh, NULL);
		for (i = 0; i < RTW_RSSI_TYPE_MAX; i++) {
			phl_com->rssi_stat.ma_rssi_ele_idx[i] = 0;
			phl_com->rssi_stat.ma_rssi_ele_cnt[i] = 0;
			phl_com->rssi_stat.ma_rssi_ele_sum[i] = 0;
			phl_com->rssi_stat.ma_rssi[i] = 0;
			for (j = 0; j < PHL_RSSI_MAVG_NUM; j++)
				phl_com->rssi_stat.ma_rssi_ele[i][j] = 0;
		}
		_os_spinunlock(phl_com->drv_priv,
			       &(phl_com->rssi_stat.lock), _bh, NULL);
	} while (0);

	PHL_INFO("<-- %s\n", __func__);
}

u8
rtw_phl_get_ma_rssi(struct rtw_phl_com_t *phl_com,
		    enum rtw_rssi_type rssi_type)
{

	u8 ret = 0;
	if (NULL == phl_com)
		return ret;

	_os_spinlock(phl_com->drv_priv,
		     &(phl_com->rssi_stat.lock), _bh, NULL);
	ret = phl_com->rssi_stat.ma_rssi[rssi_type];
	_os_spinunlock(phl_com->drv_priv,
		       &(phl_com->rssi_stat.lock), _bh, NULL);

	return ret;
}

#ifdef RTW_WKARD_DYNAMIC_BFEE_CAP
enum rtw_phl_status
rtw_phl_bfee_ctrl(void *phl, struct rtw_wifi_role_link_t *rlink, bool ctrl)
{
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;
	enum rtw_phl_status pstatus = RTW_PHL_STATUS_SUCCESS;
	if (RTW_HAL_STATUS_SUCCESS !=
	    rtw_hal_bf_bfee_ctrl(phl_info->hal, rlink->hw_band, ctrl)) {
		pstatus = RTW_PHL_STATUS_FAILURE;
	}
	return pstatus;
}
#endif

u8
rtw_phl_get_sta_mgnt_rssi(struct rtw_phl_stainfo_t *psta)
{
	u8 ret = PHL_MAX_RSSI;

	if (psta != NULL) {
		ret  = psta->hal_sta->rssi_stat.ma_rssi_mgnt;
	}

	return ret;
}

void rtw_phl_init_chdef(struct rtw_phl_com_t *phl_com, struct rtw_chan_def *chdef)
{
	_os_mem_set(phl_com->drv_priv, chdef, 0, sizeof(struct rtw_chan_def));
	chdef->band = BAND_MAX;
	chdef->bw = CHANNEL_WIDTH_MAX;
}

void rtw_phl_set_one_txring_mode(void *phl, u8 value)
{
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;

	phl_info->use_onetxring = value;
}

u8 rtw_phl_get_one_txring_mode(void *phl)
{
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;

	return phl_info->use_onetxring;
}

enum rtl_ic_id
rtw_phl_get_ic_id(void *phl)
{
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;
	enum rtw_chip_id chip_id = CHIP_WIFI6_MAX;
	enum rtl_ic_id ic_id = MAX_IC_ID;

	chip_id = rtw_hal_get_chip_id(phl_info->hal);
	switch (chip_id) {
	case CHIP_WIFI6_8852A:
		ic_id = RTL8852A;
		break;
	case CHIP_WIFI6_8834A:
		ic_id = RTL8834A;
		break;
	case CHIP_WIFI6_8852B:
		ic_id = RTL8852B;
		break;
	case CHIP_WIFI6_8852C:
		ic_id = RTL8852C;
		break;
	case CHIP_WIFI6_8192XB:
		ic_id = RTL8192XB;
		break;
	case CHIP_WIFI6_8832BR:
		ic_id = RTL8832BR;
		break;
	case CHIP_WIFI6_8852BP:
		ic_id = RTL8852BP;
		break;
	case CHIP_WIFI6_8851B:
		ic_id = RTL8851B;
		break;
	default :
		ic_id = MAX_IC_ID;
		break;
	}
	return ic_id;
}

enum rtw_phl_status
rtw_phl_pwr_switch_mac(void *phl, bool on)
{
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;
	enum rtw_phl_status pstatus = RTW_PHL_STATUS_SUCCESS;

	PHL_INFO("%s(): %d\n", __func__, on);
	if (RTW_HAL_STATUS_SUCCESS !=
	    rtw_hal_pwr_switch_mac(phl_info->hal, on)) {
		pstatus = RTW_PHL_STATUS_FAILURE;
	}
	return pstatus;
}
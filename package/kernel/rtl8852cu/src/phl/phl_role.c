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
#define _PHL_ROLE_C_
#include "phl_headers.h"

static enum rtw_phl_status
_phl_alloc_hw_port(struct phl_info_t *phl,
                   struct rtw_wifi_role_link_t *rlink)
{
	struct rtw_phl_com_t *phl_com = phl->phl_com;
	struct hal_spec_t *hal_spec = phl_get_ic_spec(phl_com);
	struct mr_ctl_t *mr_ctl = phlcom_to_mr_ctrl(phl_com);
	struct hw_band_ctl_t *band_ctrl;
	u8 max_port_num = hal_spec->port_num;
	u8 i = max_port_num;

	band_ctrl = &mr_ctl->band_ctrl[rlink->hw_band];

	_os_spinlock(phl_to_drvpriv(phl), &band_ctrl->lock, _bh, NULL);
	for (i = 0; i < max_port_num; i++) {
		if (!(band_ctrl->port_map & BIT(i))) {
			band_ctrl->port_map |= BIT(i);
			break;
		}
	}
	_os_spinunlock(phl_to_drvpriv(phl), &band_ctrl->lock, _bh, NULL);

	if (i == max_port_num) {
		PHL_ERR("%s Can't get port idx\n", __func__);
		return RTW_PHL_STATUS_FAILURE;
	}
	rlink->hw_port = i;

	return RTW_PHL_STATUS_SUCCESS;
}

static enum rtw_phl_status
_phl_realloc_hw_port(struct phl_info_t *phl,
                     struct rtw_wifi_role_link_t *rlink,
                     u8 new_port)
{
	struct rtw_phl_com_t *phl_com = phl->phl_com;
	struct mr_ctl_t *mr_ctl = phlcom_to_mr_ctrl(phl_com);
	struct hw_band_ctl_t *band_ctrl;

	band_ctrl = &mr_ctl->band_ctrl[rlink->hw_band];

	_os_spinlock(phl_to_drvpriv(phl), &band_ctrl->lock, _bh, NULL);
	if (!(band_ctrl->port_map & BIT(new_port)))
		band_ctrl->port_map |= BIT(new_port);
	_os_spinunlock(phl_to_drvpriv(phl), &band_ctrl->lock, _bh, NULL);

	rlink->hw_port = new_port;

	return RTW_PHL_STATUS_SUCCESS;
}

static enum rtw_phl_status
_phl_release_hw_port(struct phl_info_t *phl,
                     struct rtw_wifi_role_link_t *rlink)
{
	struct rtw_phl_com_t *phl_com = phl->phl_com;
	struct mr_ctl_t *mr_ctl = phlcom_to_mr_ctrl(phl_com);
	struct hw_band_ctl_t *band_ctrl;

	band_ctrl = &mr_ctl->band_ctrl[rlink->hw_band];

	_os_spinlock(phl_to_drvpriv(phl), &band_ctrl->lock, _bh, NULL);
	band_ctrl->port_map &= ~BIT(rlink->hw_port);
	_os_spinunlock(phl_to_drvpriv(phl), &band_ctrl->lock, _bh, NULL);
	return RTW_PHL_STATUS_SUCCESS;
}

static enum rtw_phl_status
_phl_alloc_rlink_hw_resource(struct phl_info_t *phl,
				struct rtw_wifi_role_t *wrole,
				struct rtw_wifi_role_link_t *rlink,
				u8 hw_band)
{
	enum rtw_phl_status psts = RTW_PHL_STATUS_FAILURE;
	void *drv = phl_to_drvpriv(phl);
	struct rtw_phl_com_t *phl_com = phl->phl_com;
	struct mr_ctl_t *mr_ctl = phlcom_to_mr_ctrl(phl_com);
	struct hal_spec_t *hal_spec = phl_get_ic_spec(phl_com);
	struct hw_band_ctl_t *band_ctrl;
	u8 port_num, max_port_num;
	u8 wmm_num, max_wmm_num;
	int ridx;
	u8 lidx;

	band_ctrl = &mr_ctl->band_ctrl[hw_band];
	max_port_num = hal_spec->port_num;
	max_wmm_num = hal_spec->wmm_num;
	ridx = wrole->id;
	lidx = rlink->id;

	_os_spinlock(drv, &band_ctrl->lock, _bh, NULL);
	/*alloc hw_port or M-BSSID*/
	rlink->hw_mbssid = 0;
#ifdef CONFIG_RTW_SUPPORT_MBSSID_VAP
	if (wrole->type == PHL_RTYPE_VAP) {
		u8 mbssid_idx; /* 0:root, 1~n:VAP */

		/* RTK MBSSID VAP is port 0 function. */
		port_num = 0;
		if ((band_ctrl->port_map & BIT(0)) == 0) {
			PHL_ERR("Creating VAP w.o. port 0 used.\n");
			port_num = max_port_num;
			goto _exit;
		}

		/* Find one unused MBSSID */
		for (mbssid_idx = 1; mbssid_idx <= MAX_MBSSID_NUMBER; mbssid_idx++) {
			if ((band_ctrl->mbssid_map & BIT(mbssid_idx)) == 0) {
				band_ctrl->mbssid_map |= BIT(mbssid_idx);
				break;
			}
		}
		if (mbssid_idx > MAX_MBSSID_NUMBER) {
			PHL_ERR("Can't allocate M-BSSID for VAP. (%u/%02X)\n",
			        MAX_MBSSID_NUMBER, band_ctrl->mbssid_map);
			goto _exit;
		}

		rlink->hw_mbssid = mbssid_idx;
		band_ctrl->mbssid_cnt++;

		PHL_PRINT("Creating VAP with M-BSSID %u.\n", rlink->hw_mbssid);
	} else
#endif /* CONFIG_RTW_SUPPORT_MBSSID_VAP */
	{
		for (port_num = 0; port_num < max_port_num; port_num++) {
			if (!(band_ctrl->port_map & BIT(port_num))) {
				band_ctrl->port_map |= BIT(port_num);
				break;
			}
		}

		if (port_num == max_port_num) {
			PHL_ERR("%s Can't allocate hw port\n", __func__);
			_os_warn_on(1);
			goto _exit;
		}
	}
	rlink->hw_port = port_num;

	/*alloc hw_wmm*/
	for (wmm_num = 0; wmm_num < max_wmm_num; wmm_num++) {
		if (!(band_ctrl->wmm_map & BIT(wmm_num))) {
			band_ctrl->wmm_map |= BIT(wmm_num);
			break;
		}
	}
	if (wmm_num == max_wmm_num) {
		PHL_INFO("%s ridx:%d lidx:%d - assigne wmm_id to 0\n",
			__func__, ridx, lidx);
		rlink->hw_wmm = 0;
	} else {
		rlink->hw_wmm = wmm_num;
	}
	#ifdef RTW_WKARD_HW_WMM_ALLOCATE
	if (rlink->hw_wmm == 0)
		_os_atomic_inc(phl_to_drvpriv(phl), &rlink->hw_wmm0_ref_cnt);
	#endif

	/*set mr_role_map*/
	band_ctrl->role_map |= BIT(ridx);
	rlink->hw_band = hw_band;
	psts = RTW_PHL_STATUS_SUCCESS;
_exit:
	_os_spinunlock(drv, &band_ctrl->lock, _bh, NULL);

	if (psts == RTW_PHL_STATUS_SUCCESS) {
		PHL_INFO("%s ridx:%d lidx:%d success\n", __func__, ridx, lidx);
		PHL_DUMP_RLINK(phl, wrole, rlink);
	}
	return psts;
}

static enum rtw_phl_status
_phl_free_rlink_hw_resource(struct phl_info_t *phl,
                            struct rtw_wifi_role_t *wrole,
                            struct rtw_wifi_role_link_t *rlink)
{
	enum rtw_phl_status psts = RTW_PHL_STATUS_FAILURE;
	void *drv = phl_to_drvpriv(phl);
	struct rtw_phl_com_t *phl_com = phl->phl_com;
	struct mr_ctl_t *mr_ctl = phlcom_to_mr_ctrl(phl_com);
	struct hw_band_ctl_t *band_ctrl;
	int ridx = wrole->id;
	u8 hw_band = rlink->hw_band;

	band_ctrl = &mr_ctl->band_ctrl[hw_band];
	PHL_DUMP_RLINK(phl, wrole, rlink);

	_os_spinlock(drv, &band_ctrl->lock, _bh, NULL);
	/*release hw_port*/
	#ifdef CONFIG_RTW_SUPPORT_MBSSID_VAP
	if (rlink->hw_mbssid != 0) { /* vap */
		PHL_INFO("Free MBSSID %u.\n", rlink->hw_mbssid);
		band_ctrl->mbssid_cnt--;
		band_ctrl->mbssid_map &= ~BIT(rlink->hw_mbssid);
		rlink->hw_mbssid = 0;
	} else
	#endif /* CONFIG_RTW_SUPPORT_MBSSID_VAP */
	{
		band_ctrl->port_map &= ~BIT(rlink->hw_port);
	}

	/*release hw_wmm*/
	#ifdef RTW_WKARD_HW_WMM_ALLOCATE
	if (rlink->hw_wmm == 0) {
		_os_atomic_dec(phl_to_drvpriv(phl), &rlink->hw_wmm0_ref_cnt);
		if (_os_atomic_read(phl_to_drvpriv(phl), &rlink->hw_wmm0_ref_cnt) == 0)
			band_ctrl->wmm_map &= ~BIT(rlink->hw_wmm);
	} else
	#endif
	{
		band_ctrl->wmm_map &= ~BIT(rlink->hw_wmm);
	}
	/*release mr_role_map*/
	band_ctrl->role_map &= ~BIT(ridx);
	_os_spinunlock(drv, &band_ctrl->lock, _bh, NULL);

	psts = RTW_PHL_STATUS_SUCCESS;
	PHL_INFO("%s ridx-id:%d rlink-id:%d success\n", __func__, ridx, rlink->id);
	return psts;
}

#ifdef CONFIG_DBCC_SUPPORT
static enum rtw_phl_status
_phl_realloc_rlink_hw_resource(struct phl_info_t *phl,
                               struct rtw_wifi_role_t *wrole,
					           struct rtw_wifi_role_link_t *rlink,
					           u8 new_band)
{
	enum rtw_phl_status psts = RTW_PHL_STATUS_FAILURE;
#ifdef DBG_DBCC_MONITOR_TIME
	u32 start_t = 0;

	phl_fun_monitor_start(&start_t, true, __FUNCTION__);
#endif /* DBG_DBCC_MONITOR_TIME */
	if (new_band == rlink->hw_band) {
		PHL_ERR("%s widx:%d lidx:%d at the same band(%d)\n",
			__func__, wrole->id, rlink->id, new_band);
		goto _exit;
	}

	psts = _phl_free_rlink_hw_resource(phl, wrole, rlink);
	if (psts != RTW_PHL_STATUS_SUCCESS)
		goto _exit;

	psts = _phl_alloc_rlink_hw_resource(phl, wrole, rlink, new_band);
	if (psts != RTW_PHL_STATUS_SUCCESS)
		goto _exit;

	psts = RTW_PHL_STATUS_SUCCESS;

_exit:
#ifdef DBG_DBCC_MONITOR_TIME
	phl_fun_monitor_end(&start_t, __FUNCTION__);
#endif /* DBG_DBCC_MONITOR_TIME */
	return psts;
}
#endif

static u8 _phl_search_roleidx_by_addr(
			struct phl_info_t *phl_info, u8 *mac_addr)
{
	struct rtw_phl_com_t *phl_com = phl_info->phl_com;
	u8 ridx = MAX_WIFI_ROLE_NUMBER;
	struct mr_ctl_t *mr_ctl = phlcom_to_mr_ctrl(phl_com);

	for (ridx = 0; ridx < MAX_WIFI_ROLE_NUMBER; ridx++) {
		if (mr_ctl->role_map & BIT(ridx)) {
			if (_os_mem_cmp(phl_to_drvpriv(phl_info),
				phl_com->wifi_roles[ridx].mac_addr,
				mac_addr, MAC_ALEN) == 0)
				break;
		}
	}

	#if 0
	if (ridx == MAX_WIFI_ROLE_NUMBER)
		PHL_INFO("%s cannot get rid\n", __func__);
	#endif
	return ridx;
}

struct rtw_wifi_role_link_t *
phl_get_rlink_by_hw_band(struct rtw_wifi_role_t *wrole,
                         u8 hw_band)
{
	struct rtw_wifi_role_link_t *rlink = NULL;
	u8 lidx;
	bool found = false;

	for (lidx = 0; lidx < wrole->rlink_num; lidx++) {
		rlink = get_rlink(wrole, lidx);
		if (rlink->hw_band == hw_band) {
			found = true;
			break;
		}
	}

	if (found == false)
		rlink = NULL;

	return rlink;
}

struct rtw_wifi_role_t *
phl_get_wrole_by_addr(struct phl_info_t *phl_info, u8 *mac_addr)
{
	struct rtw_phl_com_t *phl_com = phl_info->phl_com;
	int ridx = MAX_WIFI_ROLE_NUMBER;
	struct mr_ctl_t *mr_ctl = phlcom_to_mr_ctrl(phl_com);
	struct rtw_wifi_role_t *wrole = NULL;
	bool found = false;

	for (ridx = 0; ridx < MAX_WIFI_ROLE_NUMBER; ridx++) {
		if (mr_ctl->role_map & BIT(ridx)) {
			wrole = &(phl_com->wifi_roles[ridx]);
			if (wrole->active &&
			    _os_mem_cmp(phl_to_drvpriv(phl_info), wrole->mac_addr,
			                mac_addr, MAC_ALEN) == 0) {
				found = true;
				break;
			}
		}
	}

	if (found == false)
		wrole = NULL;

	return wrole;
}

struct rtw_wifi_role_t *
phl_get_wrole_by_ridx(struct phl_info_t *phl_info, u8 rold_idx)
{
	struct rtw_phl_com_t *phl_com = phl_info->phl_com;

	if (rold_idx < MAX_WIFI_ROLE_NUMBER)
		return &(phl_com->wifi_roles[rold_idx]);

	return NULL;
}

static void
_phl_role_notify_buf_done(void* priv, struct phl_msg* msg)
{
	struct phl_info_t *phl_info = (struct phl_info_t *)priv;

	if(msg->inbuf && msg->inlen){
		_os_mem_free(phl_to_drvpriv(phl_info), msg->inbuf, msg->inlen);
	}
}

#ifdef CONFIG_CMD_DISP
static void
_phl_role_cmd_done(void* priv, struct phl_msg* msg)
{
	struct phl_info_t *phl_info = (struct phl_info_t *)priv;

	if(msg->inbuf && msg->inlen){
		_os_kmem_free(phl_to_drvpriv(phl_info),
			msg->inbuf, msg->inlen);
	}
}

static void
_phl_send_role_notify_cmd(struct phl_info_t *phl_info,
			struct rtw_wifi_role_t *wrole, enum link_state lstate)
{
	struct phl_msg msg = {0};
	struct phl_msg_attribute attr = {0};
	struct rtw_role_cmd *rcmd = NULL;

	rcmd = (struct rtw_role_cmd *)_os_kmem_alloc(
		phl_to_drvpriv(phl_info), sizeof(struct rtw_role_cmd));
	if (rcmd == NULL) {
		PHL_ERR("%s: alloc role cmd fail.\n", __func__);
		return;
	}

	rcmd->wrole = wrole;
	rcmd->lstate = lstate;
	msg.inbuf = (u8 *)rcmd;
	msg.inlen = sizeof(*rcmd);

	SET_MSG_MDL_ID_FIELD(msg.msg_id, PHL_MDL_MRC);
	SET_MSG_EVT_ID_FIELD(msg.msg_id, MSG_EVT_ROLE_NTFY);
	msg.band_idx = HW_BAND_0;
	attr.completion.completion = _phl_role_cmd_done;
	attr.completion.priv = phl_info;

	if (phl_disp_eng_send_msg(phl_info, &msg, &attr, NULL) !=
				RTW_PHL_STATUS_SUCCESS) {
		PHL_ERR("%s: dispr_send_msg failed !\n", __func__);
		goto cmd_fail;
	}

	return;

cmd_fail:
	_os_mem_free(phl_to_drvpriv(phl_info), rcmd,
				sizeof(struct rtw_role_cmd));
}
#endif

static enum rtw_phl_status
_phl_role_notify(struct phl_info_t *phl_info,
		struct rtw_wifi_role_t *wrole,
		enum link_state lstate)
{
	if (lstate >= PHL_LINK_UNKNOWN) {
		PHL_ERR("%s Unknow lstate:%d\n", __func__, lstate);
		return RTW_PHL_STATUS_FAILURE;
	}

#if defined(CONFIG_PHL_CMD_BTC) || defined(CONFIG_POWER_SAVE)
	_phl_send_role_notify_cmd(phl_info, wrole, lstate);
#endif

	return RTW_PHL_STATUS_SUCCESS;
}

static struct rtw_wifi_role_t *
_phl_wifi_role_alloc_sw(struct phl_info_t *phl_info,
                        u8 *mac_addr,
                        enum role_type rtype,
                        u8 ridx,
                        enum rtw_device_type dtype)
{
	struct rtw_phl_com_t *phl_com = phl_info->phl_com;
	struct rtw_wifi_role_t *role = NULL;
	struct mr_ctl_t *mr_ctl = phlcom_to_mr_ctrl(phl_com);
	u8 role_idx = INVALID_WIFI_ROLE_IDX;
	u8 hw_band;
	struct rtw_wifi_role_link_t *rlink = NULL;
	u8 lidx = 0;

	_os_spinlock(phl_to_drvpriv(phl_info), &mr_ctl->lock, _bh, NULL);
	if (ridx == UNSPECIFIED_ROLE_ID) {
		/*search avaliable rid */
		for (role_idx = 0; role_idx < MAX_WIFI_ROLE_NUMBER; role_idx++) {
			if (!(mr_ctl->role_map & BIT(role_idx))) {
				mr_ctl->role_map |= BIT(role_idx);
				break;
			}
		}
	} else {
		if (mr_ctl->role_map & BIT(ridx)) {
			PHL_ERR("role_idx:%d has used\n", ridx);
			_os_warn_on(1);
		} else {
			mr_ctl->role_map |= BIT(ridx);
			role_idx = ridx;
		}
	}
	_os_spinunlock(phl_to_drvpriv(phl_info), &mr_ctl->lock, _bh, NULL);

	if (role_idx == INVALID_WIFI_ROLE_IDX) {
		PHL_ERR("%s Can't get available role idx\n", __func__);
		_os_warn_on(1);
		return role;
	}

	role = &phl_com->wifi_roles[role_idx];
	role->type = rtype;
	role->mstate = MLME_NO_LINK;
	if (dtype == DEV_TYPE_MLD)
		role->rlink_num = phl_com->dev_cap.max_link_num;
	else
		role->rlink_num = RTW_ONE_LINK;


	/*alloc hw_band and hw_port,hw_wmm*/
	for (lidx = 0; lidx < role->rlink_num; lidx++) {
		rlink = get_rlink(role, lidx);
		if (dtype == DEV_TYPE_MLD) {
			hw_band = lidx;
		}
		else {
			hw_band = HW_BAND_0;
#ifdef CONFIG_DBCC_FORCE
			if ((phl_com->dev_cap.dbcc_sup == true) &&
			    (is_frc_dbcc_mode(phl_info->phl_com))) {
				if (phl_com->dev_cap.dbcc_force_rmap & BIT(role->id)) {
					PHL_PRINT("%s Alloc HW_BAND_1 for RoleID(%d) by dbcc_force_rmap(0x%x)\n",
						__func__, role->id,
						phl_com->dev_cap.dbcc_force_rmap);
					hw_band = HW_BAND_1;
				}
			}
#endif
		}

		if (RTW_PHL_STATUS_SUCCESS !=
			_phl_alloc_rlink_hw_resource(phl_info, role, rlink, hw_band)) {
			PHL_ERR("%s alloc_hw resource failed\n", __func__);
			_os_warn_on(1);
			role = NULL;
			/* restore setting of mr_ctl->role_map */
			_os_spinlock(phl_to_drvpriv(phl_info), &mr_ctl->lock, _bh, NULL);
			mr_ctl->role_map &= ~BIT(role_idx);
			_os_spinunlock(phl_to_drvpriv(phl_info), &mr_ctl->lock, _bh, NULL);
			return role;
		}
	}

	_os_mem_cpy(phl_to_drvpriv(phl_info),
	            role->mac_addr, mac_addr, MAC_ALEN);

	for (lidx = 0; lidx < role->rlink_num; lidx++) {
		rlink = get_rlink(role, lidx);

#ifdef RTW_WKARD_STA_BCN_INTERVAL
	#ifdef RTW_PHL_BCN
		if (rtw_phl_role_is_ap_category(role)) {
			rlink->bcn_cmn.bcn_interval = 100;
			rlink->mstate = MLME_LINKED;
			rlink->hiq_win = 50;
			rlink->bss_params_chg_cnt = 0;
		}
	#endif
#endif
		phl_init_rlink_cap(phl_info, rlink);
		phl_init_protocol_cap(phl_info, role, rlink);
	}

	return role;
}

static enum rtw_phl_status
_phl_wifi_role_start(struct phl_info_t *phl_info,
				struct rtw_wifi_role_t *wrole,
				struct rtw_phl_mld_t *mld)
{
	enum rtw_phl_status psts = RTW_PHL_STATUS_FAILURE;
	enum rtw_hal_status hsts = RTW_HAL_STATUS_FAILURE;
	struct rtw_wifi_role_link_t *rlink;
	u8 lidx;

	for (lidx = 0; lidx < mld->sta_num; lidx++) {
		struct rtw_phl_stainfo_t *sta = mld->phl_sta[lidx];

		if (sta->active == false) {
			psts = phl_alloc_stainfo_hw(phl_info, sta);
			if (psts != RTW_PHL_STATUS_SUCCESS) {
				PHL_ERR("%s can't alloc self stainfo_hw\n", __func__);
				_os_warn_on(1);
				goto _exit;
			}
			PHL_INFO("%s sta_info hw - macid:%u %02x:%02x:%02x:%02x:%02x:%02x\n",
				__func__, sta->macid,
				sta->mac_addr[0], sta->mac_addr[1], sta->mac_addr[2],
				sta->mac_addr[3], sta->mac_addr[4], sta->mac_addr[5]);
		}
	}

	for (lidx = 0; lidx < wrole->rlink_num; lidx++) {
		rlink = get_rlink(wrole, lidx);
		hsts = rtw_hal_role_cfg(phl_info->hal, wrole, rlink);
		if(hsts != RTW_HAL_STATUS_SUCCESS) {
			PHL_ERR("%s hal role cfg failed\n", __func__);
			_os_warn_on(1);
			goto _exit;
		}
	}

	wrole->active = true;

	for (lidx = 0; lidx < wrole->rlink_num; lidx++) {
		rlink = get_rlink(wrole, lidx);
		rtw_phl_mr_set_rxfltr_type_by_mode(phl_info, rlink,
						RX_FLTR_TYPE_MODE_ROLE_INIT);
	}
	psts = RTW_PHL_STATUS_SUCCESS;
	return psts;

_exit:
	return psts;
}

static enum rtw_phl_status
_phl_wifi_role_free_sw(struct phl_info_t *phl_info, struct rtw_wifi_role_t *wrole)
{
	struct rtw_phl_com_t *phl_com = phl_info->phl_com;
	struct mr_ctl_t *mr_ctl = phlcom_to_mr_ctrl(phl_com);
	struct rtw_wifi_role_link_t *rlink;
	u8 lidx;

	/*release hw band,port,wmm*/
	for (lidx = 0; lidx < wrole->rlink_num; lidx++) {
		rlink = get_rlink(wrole, lidx);
		_phl_free_rlink_hw_resource(phl_info, wrole, rlink);
	}

	_os_mem_set(phl_to_drvpriv(phl_info), wrole->mac_addr, 0, MAC_ALEN);
	_os_spinlock(phl_to_drvpriv(phl_info), &mr_ctl->lock, _bh, NULL);
	mr_ctl->role_map &= ~BIT(wrole->id);
	_os_spinunlock(phl_to_drvpriv(phl_info), &mr_ctl->lock, _bh, NULL);

	wrole->rlink_num = 0;
	wrole->active = false;
	wrole->mstate = MLME_NO_LINK;
	wrole->type = PHL_RTYPE_NONE;
	return RTW_PHL_STATUS_SUCCESS;
}

#ifdef CONFIG_CMD_DISP
struct wr_start_param {
	struct rtw_wifi_role_t *wrole;
	struct rtw_phl_mld_t *mld;
};

enum rtw_phl_status
phl_wifi_role_start_hdl(struct phl_info_t *phl_info, u8 *param)
{
	struct wr_start_param *cmd_wr = (struct wr_start_param *)param;

	return _phl_wifi_role_start(phl_info, cmd_wr->wrole, cmd_wr->mld);
}

void phl_wifi_role_start_done(void *drv_priv, u8 *cmd, u32 cmd_len,
						enum rtw_phl_status status)
{
	if (cmd) {
		_os_kmem_free(drv_priv, cmd, cmd_len);
		cmd = NULL;
		PHL_INFO("%s.....\n", __func__);
	}
}

enum rtw_phl_status
phl_wifi_role_start(struct phl_info_t *phl_info,
				struct rtw_wifi_role_t *wrole,
				struct rtw_phl_mld_t *mld)
{
	enum rtw_phl_status psts = RTW_PHL_STATUS_FAILURE;
	struct wr_start_param *wr_start = NULL;
	void *drv = phl_to_drvpriv(phl_info);
	u32 wr_start_len;

	wr_start_len = sizeof(struct wr_start_param);
	wr_start = _os_kmem_alloc(drv, wr_start_len);
	if (wr_start == NULL) {
		PHL_ERR("%s: alloc wr_start_param failed!\n", __func__);
		psts = RTW_PHL_STATUS_RESOURCE;
		goto _exit;
	}
	wr_start->wrole = wrole;
	wr_start->mld = mld;

	psts = phl_cmd_enqueue(phl_info,
/* don't care band, role start will go through all links and start them */
	                       HW_BAND_0,
	                       MSG_EVT_ROLE_START,
	                       (u8*)wr_start,
	                       wr_start_len,
	                       phl_wifi_role_start_done,
	                       PHL_CMD_WAIT,
	                       0);

	if (is_cmd_failure(psts)) {
		/* Send cmd success, but wait cmd fail*/
		psts = RTW_PHL_STATUS_FAILURE;
		PHL_INFO("%s wr-id:%d failure\n", __func__, wrole->id);
	} else if (psts != RTW_PHL_STATUS_SUCCESS) {
		/* Send cmd fail */
		_os_kmem_free(drv, wr_start, wr_start_len);
		psts = RTW_PHL_STATUS_FAILURE;
		PHL_INFO("%s wr-id:%d failure\n", __func__, wrole->id);
	} else {
		PHL_INFO("%s wr-id:%d success\n", __func__, wrole->id);
	}

_exit:
	return psts;
}

enum rtw_phl_status
phl_cmd_get_cur_tsf_hdl(struct phl_info_t *phl_info, struct rtw_phl_port_tsf *tsf)
{
	enum rtw_hal_status hstatus = RTW_HAL_STATUS_FAILURE;

	hstatus = rtw_hal_get_tsf(phl_info->hal, tsf->hwband, tsf->port, &tsf->tsf_h, &tsf->tsf_l);
	if (RTW_HAL_STATUS_SUCCESS == hstatus)
		return RTW_PHL_STATUS_SUCCESS;
	else
		return RTW_PHL_STATUS_FAILURE;
}
#else
enum rtw_phl_status
phl_wifi_role_start(struct phl_info_t *phl_info,
				struct rtw_wifi_role_t *wrole,
				struct rtw_phl_mld_t *mld)
{
	return _phl_wifi_role_start(phl_info, wrole, mld);
}
#endif

u8 rtw_phl_wifi_role_alloc(void *phl,
                           u8 *mac_addr,
                           u8 **rlink_mac_addr,
                           enum role_type rtype,
                           u8 ridx,
                           struct rtw_wifi_role_t **wifi_role,
                           enum rtw_device_type dtype,
                           bool ignore_hw_fail)
{
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;
	struct rtw_phl_com_t *phl_com = phl_info->phl_com;
	u8 role_idx = INVALID_WIFI_ROLE_IDX;
	struct rtw_wifi_role_t *role = NULL;
	struct rtw_phl_stainfo_t *phl_sta = NULL;
	enum rtw_phl_status psts = RTW_PHL_STATUS_FAILURE;
	struct macid_ctl_t *mc = phl_to_mac_ctrl(phl_info);
	u16 macid = mc->max_num;
	struct rtw_phl_mld_t *mld = NULL;
	struct rtw_wifi_role_link_t *rlink;
	u8 lidx = 0;

	if (mac_addr == NULL) {
		PHL_ERR("%s mac_addr == NULL\n", __func__);
		goto _exit;
	}

	/*search rid by mac_addr, if had assigned then return role index*/
	role_idx = _phl_search_roleidx_by_addr(phl_info, mac_addr);
	if (role_idx != INVALID_WIFI_ROLE_IDX) {
		PHL_INFO("%s wifi-role(%d) had allociated\n", __func__, role_idx);
		*wifi_role = &phl_com->wifi_roles[role_idx];
		goto _exit;
	}

	role = _phl_wifi_role_alloc_sw(phl_info, mac_addr, rtype, ridx, dtype);
	if (role == NULL) {
		PHL_ERR("%s role alloc sw failed\n", __func__);
		_os_warn_on(1);
		goto _exit;
	}

	/* alloc mld for self */
	mld = phl_alloc_mld(phl_info, role, mac_addr, dtype);
	if (mld == NULL) {
		PHL_ERR("%s can't alloc self mld\n", __func__);
		_os_warn_on(1);
		goto _err_mld;
	}

	/*alloc sta_info for self*/
	for (lidx = 0; lidx < role->rlink_num; lidx++) {
		rlink = get_rlink(role, lidx);

		if (rlink_mac_addr[lidx] == NULL) {
			PHL_ERR("%s rlink_mac_addr == NULL\n", __func__);

			if (lidx != 0)
				phl_wifi_role_free_stainfo_sw(phl_info, role);

			goto _err_stainfo_sw;
		}

		_os_mem_cpy(phl_to_drvpriv(phl_info),
		            rlink->mac_addr,
		            rlink_mac_addr[lidx],
		            MAC_ALEN);

		phl_sta = phl_alloc_stainfo_sw(phl_info,
		                               rlink->mac_addr,
		                               role,
		                               dtype,
		                               macid,
		                               rlink);
		if (phl_sta == NULL) {
			PHL_ERR("%s can't alloc self stainfo_sw\n", __func__);
			_os_warn_on(1);

			if (lidx != 0)
				phl_wifi_role_free_stainfo_sw(phl_info, role);

			goto _err_stainfo_sw;
		}
		rtw_phl_link_mld_stainfo(mld, phl_sta);
		macid = phl_sta->macid;
	}

	psts = phl_wifi_role_start(phl_info, role, mld);
	if (psts != RTW_PHL_STATUS_SUCCESS) {
		PHL_ERR("%s role start failed\n", __func__);
		if (!ignore_hw_fail) {
			_os_warn_on(1);
			goto _err_role_start;
		}
		SET_STATUS_FLAG(role->status, WR_STATUS_HW_ALLOC_FAIL);
	}

	*wifi_role = role;
	role_idx = role->id;
	PHL_DUMP_MR_EX(phl_info);
	return role_idx;

_err_role_start:
	phl_wifi_role_free_stainfo_sw(phl_info, role);

_err_stainfo_sw:
	phl_wifi_role_free_mld(phl_info, role);

_err_mld:
	_phl_wifi_role_free_sw(phl_info, role);

_exit:
	return role_idx;
}

#ifdef CONFIG_DBCC_SUPPORT
/*execut this api after DBCC config success*/

enum rtw_phl_status
_phl_ap_role_assoc_sta_upt(struct phl_info_t *phl_info, struct rtw_wifi_role_t *wrole)
{
	enum rtw_phl_status psts = RTW_PHL_STATUS_FAILURE;
	enum rtw_hal_status hsts = RTW_HAL_STATUS_FAILURE;
	struct phl_queue *sta_queue = NULL;
	struct rtw_phl_stainfo_t *sta;
	bool is_ap_assoc = false;
	struct rtw_wifi_role_link_t *rlink = NULL;
	u8 ridx;

	for (ridx = 0; ridx < wrole->rlink_num; ridx++) {
		rlink = get_rlink(wrole, ridx);

		sta_queue = &rlink->assoc_sta_queue;
		_os_spinlock(phl_to_drvpriv(phl_info), &sta_queue->lock, _bh, NULL);
		if (sta_queue->cnt >= 2)
			is_ap_assoc = true;
		_os_spinunlock(phl_to_drvpriv(phl_info), &sta_queue->lock, _bh, NULL);

		if (is_ap_assoc == false) {
			PHL_INFO("wr(%d,%d) have no assoc sta\n", wrole->id, rlink->id);
			continue;
		}

		phl_list_for_loop(sta, struct rtw_phl_stainfo_t,
						&sta_queue->queue, list) {
			if (sta && _os_mem_cmp(phl_to_drvpriv(phl_info),
					sta->mac_addr, wrole->mac_addr, MAC_ALEN) != 0) {
				/*phl_re_register_tx_ring - phl_sta*/
				psts = phl_update_stainfo_sw(phl_info, sta);
				if (psts != RTW_PHL_STATUS_SUCCESS) {
					PHL_ERR("%s macid:%d phl_update_stainfo_sw failed\n",
						__func__, sta->macid);
					_os_warn_on(1);
					goto _err;
				}
				/*[HALMAC][HALBB]*/
				hsts = rtw_hal_change_sta_entry(phl_info->hal, sta, PHL_UPD_ROLE_BAND_SW);
				if (hsts != RTW_HAL_STATUS_SUCCESS) {
					PHL_ERR("%s macid:%d rtw_hal_update_sta_entry failure!\n",
						__func__, sta->macid);
					_os_warn_on(1);
					psts = RTW_PHL_STATUS_FAILURE;
					goto _err;
				}
			}
		}
	}
	psts = RTW_PHL_STATUS_SUCCESS;
_err:
	return psts;
}

enum rtw_phl_status
phl_wifi_role_realloc_band(struct phl_info_t *phl_info,
                           struct rtw_wifi_role_t *wrole,
                           struct rtw_wifi_role_link_t *rlink,
                           enum phl_band_idx new_band)
{
	enum rtw_phl_status psts = RTW_PHL_STATUS_FAILURE;
	enum rtw_hal_status hsts = RTW_HAL_STATUS_FAILURE;
	struct rtw_phl_stainfo_t *phl_sta = NULL;
	u32 func_en = false;
#ifdef DBG_DBCC_MONITOR_TIME
	u32 start_t = 0;

	phl_fun_monitor_start(&start_t, true, __FUNCTION__);
#endif /* DBG_DBCC_MONITOR_TIME */
	if (new_band == rlink->hw_band)
		goto _exit;

	PHL_DUMP_ROLE_EX(phl_info, wrole);
	PHL_DUMP_RLINK(phl_info, wrole, rlink);

	/*[HALMAC] - disable hw port*/
	hsts = rtw_hal_role_cfg_ex(phl_info->hal, rlink, PCFG_FUNC_SW, &func_en);
	if(hsts != RTW_HAL_STATUS_SUCCESS) {
		PHL_ERR("%s hal role disable failed\n", __func__);
		_os_warn_on(1);
		goto _exit;
	}

	phl_sta = rtw_phl_get_stainfo_self(phl_info, rlink);

	/*re-alloc sw resource - band, port, wmm*/
	psts = _phl_realloc_rlink_hw_resource(phl_info, wrole, rlink, new_band);
	if (psts != RTW_PHL_STATUS_SUCCESS)
		goto _exit;

	/*phl_re_register_tx_ring - phl_sta*/
	psts = phl_update_stainfo_sw(phl_info, phl_sta);
	if (psts != RTW_PHL_STATUS_SUCCESS) {
		PHL_ERR("%s phl_update_stainfo_sw failed\n", __func__);
		_os_warn_on(1);
		goto _exit;
	}

	/*[HALMAC][HALBB]*/
	hsts = rtw_hal_change_sta_entry(phl_info->hal, phl_sta, PHL_UPD_ROLE_BAND_SW);
	if (hsts != RTW_HAL_STATUS_SUCCESS) {
		PHL_ERR("rtw_hal_update_sta_entry failure!\n");
		psts = RTW_PHL_STATUS_FAILURE;
		goto _exit;
	}

	phl_init_rlink_cap(phl_info, rlink);
	phl_init_protocol_cap(phl_info, wrole, rlink);

	/*[HALMAC]*/
	hsts = rtw_hal_role_cfg(phl_info->hal, wrole, rlink);
	if(hsts != RTW_HAL_STATUS_SUCCESS) {
		PHL_ERR("%s hal role cfg failed\n", __func__);
		_os_warn_on(1);
		psts = RTW_PHL_STATUS_FAILURE;
		goto _exit;
	}

	if (rtw_phl_role_is_ap_category(wrole)) {
		psts = _phl_ap_role_assoc_sta_upt(phl_info, wrole);
		if (psts != RTW_PHL_STATUS_SUCCESS) {
			PHL_ERR("%s _phl_ap_role_assoc_sta_upt failed\n", __func__);
			_os_warn_on(1);
			goto _exit;
		}
	}
	psts = RTW_PHL_STATUS_SUCCESS;
	PHL_DUMP_ROLE_EX(phl_info, wrole);
	PHL_DUMP_RLINK(phl_info, wrole, rlink);

_exit:
#ifdef DBG_DBCC_MONITOR_TIME
	phl_fun_monitor_end(&start_t, __FUNCTION__);
#endif /* DBG_DBCC_MONITOR_TIME */
	return psts;
}

enum rtw_phl_status
rtw_phl_wifi_role_realloc_band(void *phl,
                               struct rtw_wifi_role_t *wrole,
                               struct rtw_wifi_role_link_t *rlink)
{
	enum phl_band_idx new_band;

	if (rlink->hw_band == HW_BAND_0)
		new_band = HW_BAND_1;
	else
		new_band = HW_BAND_0;

	phl_wifi_role_realloc_band((struct phl_info_t *)phl, wrole, rlink, new_band);

	return  RTW_PHL_STATUS_SUCCESS;
}
#endif /*CONFIG_DBCC_SUPPORT*/

enum rtw_phl_status
phl_wifi_role_realloc_port(struct phl_info_t *phl_info,
                           struct rtw_wifi_role_t *wrole,
                           struct rtw_wifi_role_link_t *rlink,
                           u8 new_port)
{
	enum rtw_phl_status psts = RTW_PHL_STATUS_FAILURE;
	enum rtw_hal_status hsts = RTW_HAL_STATUS_FAILURE;
	u32 func_en = false;
	struct rtw_phl_stainfo_t *phl_sta = NULL;

	if (new_port == rlink->hw_port)
		goto _exit;

	PHL_DUMP_ROLE_EX(phl_info, wrole);
	PHL_DUMP_RLINK(phl_info, wrole, rlink);

	/*free org-resource*/
	phl_sta = rtw_phl_get_stainfo_self(phl_info, rlink);
	psts = phl_free_stainfo_hw(phl_info, phl_sta);
	if (psts != RTW_PHL_STATUS_SUCCESS) {
		PHL_ERR("%s free self-sta failed\n", __func__);
		_os_warn_on(1);
		goto _exit;
	}

	/*disable hw port*/
	hsts = rtw_hal_role_cfg_ex(phl_info->hal, rlink, PCFG_FUNC_SW, &func_en);
	if(hsts != RTW_HAL_STATUS_SUCCESS) {
		PHL_ERR("%s hal role disable failed\n", __func__);
		_os_warn_on(1);
		psts = RTW_PHL_STATUS_FAILURE;
		goto _exit;
	}

	/*release hw port*/
	psts = _phl_release_hw_port(phl_info, rlink);
	if (psts != RTW_PHL_STATUS_SUCCESS) {
		PHL_ERR("%s release port failed\n", __func__);
		_os_warn_on(1);
		goto _exit;
	}

	/*re-alloc hw resource*/
	psts = _phl_realloc_hw_port(phl_info, rlink, new_port);
	if (psts != RTW_PHL_STATUS_SUCCESS) {
		PHL_ERR("%s realloc port failed\n", __func__);
		_os_warn_on(1);
		goto _exit;
	}

	psts = phl_alloc_stainfo_hw(phl_info, phl_sta);
	if (psts != RTW_PHL_STATUS_SUCCESS) {
		PHL_ERR("%s alloc sta hw failed\n", __func__);
		_os_warn_on(1);
		goto _exit;
	}

	/*Register RA*/
	if (rtw_phl_role_is_client_category(wrole) && wrole->mstate == MLME_LINKED) {
		psts = phl_change_stainfo(phl_info, phl_sta, PHL_UPD_STA_INFO_CHANGE);
		if (psts != RTW_PHL_STATUS_SUCCESS) {
			PHL_ERR(" phl_change_stainfo!\n");
			goto _exit;
		}
	}

	hsts = rtw_hal_role_cfg(phl_info->hal, wrole, rlink);
	if(hsts != RTW_HAL_STATUS_SUCCESS) {
		PHL_ERR("%s hal role cfg failed\n", __func__);
		_os_warn_on(1);
		psts = RTW_PHL_STATUS_FAILURE;
		goto _exit;
	}

	psts = RTW_PHL_STATUS_SUCCESS;
	PHL_DUMP_ROLE_EX(phl_info, wrole);
	PHL_DUMP_RLINK(phl_info, wrole, rlink);

_exit:
	return psts;
}

enum rtw_phl_status
rtw_phl_wifi_role_realloc_port(void *phl,
                               struct rtw_wifi_role_t *wrole,
                               struct rtw_wifi_role_link_t *rlink,
                               u8 new_port)
{
	return phl_wifi_role_realloc_port((struct phl_info_t *)phl, wrole, rlink, new_port);
}


enum rtw_phl_status
phl_wifi_role_realloc_wmm(struct phl_info_t *phl_info,
                          struct rtw_wifi_role_link_t *rlink,
                          u8 new_wmm)
{
	enum rtw_phl_status psts = RTW_PHL_STATUS_FAILURE;

	if (new_wmm == rlink->hw_wmm)
		goto _exit;

	psts = RTW_PHL_STATUS_SUCCESS;

_exit:
	return psts;
}


enum rtw_phl_status
phl_role_noa_notify(struct phl_info_t *phl_info, struct rtw_wifi_role_t *wrole)
{
	_phl_role_notify(phl_info, wrole, PHL_LINK_UP_NOA);

	return RTW_PHL_STATUS_SUCCESS;
}

enum rtw_phl_status
phl_role_ap_client_notify(struct phl_info_t *phl_info,
		struct rtw_wifi_role_t *wrole, u8 link_sts, u16 client_macid)
{
	enum link_state lstate = PHL_LINK_UNKNOWN;
#ifdef CONFIG_BTCOEX
	struct rtw_wifi_role_link_t *rlink = get_rlink(wrole, RTW_RLINK_PRIMARY);
#endif /* CONFIG_BTCOEX */

	if (link_sts == MLME_LINKING) {
		lstate = PHL_ClIENT_JOINING;
	} else if (link_sts == MLME_NO_LINK) {
		lstate = PHL_ClIENT_LEFT;
	} else {
		PHL_TRACE(COMP_PHL_DBG, _PHL_ERR_,
			"%s macid(%d), link_sts don't support(%d)\n",
			__func__, client_macid, link_sts);
		return RTW_PHL_STATUS_FAILURE;
	}
#ifdef CONFIG_BTCOEX
	rtw_hal_btc_ap_client_notify(phl_info->hal, rlink, lstate);
#endif /* CONFIG_BTCOEX */
#ifdef CONFIG_MCC_SUPPORT
	phl_mcc_ap_client_notify(phl_info, wrole, lstate);
#endif /* CONFIG_MCC_SUPPORT */

#ifdef CONFIG_P2PPS
	phl_p2pps_ap_client_notify(phl_info, wrole, lstate,
						client_macid);
#endif

	return RTW_PHL_STATUS_SUCCESS;
}

enum rtw_phl_status
phl_role_notify(struct phl_info_t *phl_info, struct rtw_wifi_role_t *wrole)
{
	enum link_state lstate = PHL_LINK_UNKNOWN;
	switch (wrole->type) {
	case PHL_RTYPE_STATION:
	case PHL_RTYPE_P2P_GC:
	{
		lstate = (wrole->mstate == MLME_NO_LINK)
			? PHL_LINK_STOPPED
			: PHL_LINK_STARTED;
	}
	break;

	case PHL_RTYPE_AP:
	case PHL_RTYPE_VAP:
	case PHL_RTYPE_MESH:
	case PHL_RTYPE_P2P_GO:
	{
		lstate = (wrole->mstate == MLME_NO_LINK)
			? PHL_LINK_STOPPED
			: PHL_LINK_STARTED;
	}
	break;

	case PHL_RTYPE_NONE:
	case PHL_RTYPE_ADHOC:
	case PHL_RTYPE_ADHOC_MASTER:
	case PHL_RTYPE_MONITOR:
	case PHL_RTYPE_P2P_DEVICE:
	case PHL_RTYPE_NAN:
	case PHL_MLME_MAX:
		PHL_TRACE(COMP_PHL_DBG, _PHL_ERR_,
			"%s: Unsupported case:%d in wrole notify, please check it\n",
			__func__, wrole->type);
		break;
	default:
		PHL_TRACE(COMP_PHL_DBG, _PHL_ERR_,
			"%s role-type(%d) not support\n",
			__func__, wrole->type);
		break;
	}

	_phl_role_notify(phl_info, wrole, lstate);
	return RTW_PHL_STATUS_SUCCESS;
}


/**
 * This function is called once wifi info changed
 * (see enum wr_chg_id)
 * @phl: see phl_info_t
 * @wrole: information is updated for this wifi role
 * @rlink: update information for specific role link
 * @chg_id: see enum wr_chg_id
 * @chg_info: the change info to be update
 */
enum rtw_phl_status
phl_wifi_role_change(struct phl_info_t *phl_info,
                     struct rtw_wifi_role_t *wrole,
                     struct rtw_wifi_role_link_t *rlink,
                     enum wr_chg_id chg_id,
                     void *chg_info
)
{
	enum rtw_phl_status pstate = RTW_PHL_STATUS_FAILURE;
	enum rtw_hal_status hstate;
	struct rtw_phl_stainfo_t *sta = NULL;
	enum phl_upd_mode mode = PHL_UPD_ROLE_MAX;
	void *drv = phl_to_drvpriv(phl_info);
	struct rtw_wifi_role_link_t *tmp_rlink = NULL;
	u8 idx = 0;

	switch (chg_id) {
	case WR_CHG_TYPE:
	{
		enum role_type type = *(enum role_type *)chg_info;
		u8 mac_addr[MAC_ALEN] = {0};

		if (wrole->type == type) {
			PHL_WARN("wrole type(%d) not change\n", wrole->type);
			pstate = RTW_PHL_STATUS_SUCCESS;
			return pstate;
		}

		#ifdef CONFIG_RTW_SUPPORT_MBSSID_VAP
		/* VAP does not own a port. Role type should not be changed
		 * in active state, or the root AP could be affected.
		 */
		if (wrole->type == PHL_RTYPE_VAP) {
			PHL_ERR("Role type VAP can not be changed.\n");
			return RTW_PHL_STATUS_INVALID_PARAM;
		}
		#endif /* CONFIG_RTW_SUPPORT_MBSSID_VAP */

		PHL_INFO("wrole type(%d) change to type(%d)\n",
							wrole->type, type);

		_os_mem_cpy(drv, mac_addr, wrole->mac_addr, MAC_ALEN);
		for (idx = 0; idx < wrole->rlink_num; idx++) {
			tmp_rlink = get_rlink(wrole, idx);

			sta = rtw_phl_get_stainfo_self(phl_info, tmp_rlink);
			if (sta) {
				if (type == PHL_RTYPE_TDLS || wrole->type == PHL_RTYPE_TDLS) {
					wrole->type = type;
					pstate = RTW_PHL_STATUS_SUCCESS;
				}
				else {
					wrole->type = type;
					wrole->mstate = MLME_NO_LINK;
					tmp_rlink->mstate = MLME_NO_LINK;

					#ifdef RTW_WKARD_STA_BCN_INTERVAL
					#ifdef RTW_PHL_BCN
					if (rtw_phl_role_is_ap_category(wrole)) {
						wrole->mstate = MLME_LINKED;
						tmp_rlink->bcn_cmn.bcn_interval = 100;
						tmp_rlink->mstate = MLME_LINKED;
						tmp_rlink->hiq_win = 16; /* unit: ms */
					}
					#endif
					#endif

					phl_init_rlink_cap(phl_info, tmp_rlink);
					phl_init_protocol_cap(phl_info, wrole, tmp_rlink);
					rtw_hal_role_cfg(phl_info->hal, wrole, tmp_rlink);

					rtw_phl_mr_set_rxfltr_type_by_mode(phl_info, tmp_rlink,
									   RX_FLTR_TYPE_MODE_ROLE_INIT);

					if (wrole->rlink_num > 1)
						mac_addr[5]++;
					_os_mem_cpy(drv, sta->mac_addr, mac_addr, MAC_ALEN);

					mode = PHL_UPD_ROLE_TYPE_CHANGE;
					pstate = phl_change_stainfo(phl_info, sta, mode);
					/*rtw_hal_change_sta_entry(phl_info->hal, sta);*/
				}
			}
			else {
				PHL_ERR("cannot get stainfo_self\n");
			}
		}
		PHL_DUMP_MR_EX(phl_info);
	}
		break;
	case WR_CHG_MADDR:
	{
		u8 *maddr = (u8 *)chg_info;

		if (wrole->mstate == MLME_LINKED) {
			PHL_WARN("Do not change wrole maddr when linked\n");
			return RTW_PHL_STATUS_SUCCESS;
		}

		if (rlink == NULL) {
			if(_os_mem_cmp(drv, wrole->mac_addr, maddr,  MAC_ALEN) == 0) {
				PHL_WARN("wrole maddr %02x:%02x:%02x:%02x:%02x:%02x not change\n",
				         maddr[0], maddr[1], maddr[2],
				         maddr[3], maddr[4], maddr[5]);
				return RTW_PHL_STATUS_SUCCESS;
			}
			PHL_INFO("wrole maddr %02x:%02x:%02x:%02x:%02x:%02x change to %02x:%02x:%02x:%02x:%02x:%02x\n",
			         wrole->mac_addr[0], wrole->mac_addr[1],
			         wrole->mac_addr[2], wrole->mac_addr[3],
			         wrole->mac_addr[4], wrole->mac_addr[5],
			         maddr[0], maddr[1], maddr[2],
			         maddr[3], maddr[4], maddr[5]);

			_os_mem_cpy(drv, wrole->mac_addr, maddr, MAC_ALEN);
		} else {
			if(_os_mem_cmp(drv, rlink->mac_addr, maddr,  MAC_ALEN) == 0) {
				PHL_WARN("rlink maddr %02x:%02x:%02x:%02x:%02x:%02x not change\n",
				         maddr[0], maddr[1], maddr[2],
				         maddr[3], maddr[4], maddr[5]);
				return RTW_PHL_STATUS_SUCCESS;
			}
			PHL_INFO("rlink maddr %02x:%02x:%02x:%02x:%02x:%02x change to %02x:%02x:%02x:%02x:%02x:%02x\n",
			         rlink->mac_addr[0], rlink->mac_addr[1],
			         rlink->mac_addr[2], rlink->mac_addr[3],
			         rlink->mac_addr[4], rlink->mac_addr[5],
			         maddr[0], maddr[1], maddr[2],
			         maddr[3], maddr[4], maddr[5]);

			_os_mem_cpy(drv, rlink->mac_addr, maddr, MAC_ALEN);
			sta = rtw_phl_get_stainfo_self(phl_info, rlink);
			if (sta) {
				mode = PHL_UPD_ROLE_INFO_CHANGE;
				_os_mem_cpy(drv, sta->mac_addr, wrole->mac_addr, MAC_ALEN);
				pstate = phl_change_stainfo(phl_info, sta, mode);
				/*rtw_hal_change_sta_entry(phl_info->hal, sta);*/
			}
		}
	}
		break;
	case WR_CHG_AP_PARAM:
	{
		struct rtw_ap_param *param = (struct rtw_ap_param *)chg_info;

		if (!rtw_phl_role_is_ap_category(wrole))
			break;

		if (rlink == NULL) {
			PHL_WARN("%s: Target rlink is NULL!\n", __func__);
			break;
		}

		if (param->cfg_id == CFG_BSS_CLR) {
			u32 bsscolor = param->value;

			rlink->protocol_cap.bsscolor = (u8)bsscolor;
			rtw_hal_role_cfg_ex(phl_info->hal,
			                    rlink,
			                    PCFG_BSS_CLR,
			                    &bsscolor);
		}
		else if (param->cfg_id == CFG_BCN_DRP_ALL) {
			u32 bcn_drp_en = param->value;

			rtw_hal_role_cfg_ex(phl_info->hal, rlink, PCFG_BCN_DRP_ALL, &bcn_drp_en);
		}
		/* else if (param->cfg_id == CFG_HIQ_DTIM) */
		else {
			PHL_INFO("wrole->type(%d) WR_CHG_AP_PARAM todo.....\n", wrole->type);
		}

		pstate = RTW_PHL_STATUS_SUCCESS;
	}
		break;
	case WR_CHG_EDCA_PARAM:
	{
		struct rtw_edca_param *param = (struct rtw_edca_param*)chg_info;

		if (rlink == NULL) {
			PHL_WARN("%s: Target rlink is NULL!\n", __func__);
			break;
		}

		hstate = rtw_hal_set_edca(phl_info->hal,
		                          rlink,
		                          param->ac,
		                          param->param);
		if (hstate == RTW_HAL_STATUS_SUCCESS) {
			pstate = RTW_PHL_STATUS_SUCCESS;
		} else {
			pstate = RTW_PHL_STATUS_FAILURE;
			break;
		}
	}
		break;
	case WR_CHG_MU_EDCA_PARAM:
	{
		struct rtw_mu_edca_param *param = (struct rtw_mu_edca_param*)chg_info;

		if (rlink == NULL) {
			PHL_WARN("%s: Target rlink is NULL!\n", __func__);
			break;
		}

		hstate = rtw_hal_set_mu_edca(phl_info->hal,
		                             rlink->hw_band,
		                             param->ac,
		                             param->timer,
		                             (param->cw & 0x0f),
		                             (param->cw>>4),
		                             param->aifsn);
		if (hstate == RTW_HAL_STATUS_SUCCESS) {
			pstate = RTW_PHL_STATUS_SUCCESS;
		} else {
			pstate = RTW_PHL_STATUS_FAILURE;
			break;
		}
	}
		break;
	case WR_CHG_MU_EDCA_CFG:
	{
		u8 val = *(u8 *)chg_info;

		if (rlink == NULL) {
			PHL_WARN("%s: Target rlink is NULL!\n", __func__);
			break;
		}

		hstate = rtw_hal_set_mu_edca_ctrl(phl_info->hal,
		                                  rlink->hw_band,
		                                  rlink->hw_wmm,
		                                  val);
		if (hstate == RTW_HAL_STATUS_SUCCESS) {
			pstate = RTW_PHL_STATUS_SUCCESS;
		} else {
			pstate = RTW_PHL_STATUS_FAILURE;
			break;
		}
	}
		break;
	case WR_CHG_BSS_COLOR:
	{
		u32 val = *(u8 *)chg_info;

		if (rlink == NULL) {
			PHL_WARN("%s: Target rlink is NULL!\n", __func__);
			break;
		}

		hstate = rtw_hal_role_cfg_ex(phl_info->hal,
		                             rlink,
		                             PCFG_BSS_CLR,
		                             &val);
		if (hstate == RTW_HAL_STATUS_SUCCESS) {
			pstate = RTW_PHL_STATUS_SUCCESS;
		} else {
			pstate = RTW_PHL_STATUS_FAILURE;
			break;
		}
	}
		break;
	case WR_CHG_RTS_TH:
	{
		struct rtw_rts_threshold *val =
				(struct rtw_rts_threshold *)chg_info;

		if (rlink == NULL) {
			PHL_WARN("%s: Target rlink is NULL!\n", __func__);
			break;
		}

		hstate = rtw_hal_config_rts_th(phl_info->hal,
		                               rlink->hw_band,
		                               val->rts_time_th,
		                               val->rts_len_th);
		if (hstate == RTW_HAL_STATUS_SUCCESS) {
			pstate = RTW_PHL_STATUS_SUCCESS;
		} else {
			pstate = RTW_PHL_STATUS_FAILURE;
			break;
		}
	}
	break;
	case WR_CHG_DFS_HE_TB_CFG:
	{
		u8 val = *(u8 *)chg_info;

		hstate = rtw_hal_set_dfs_tb_ctrl(phl_info->hal, val);
		if (hstate == RTW_HAL_STATUS_SUCCESS)
			pstate = RTW_PHL_STATUS_SUCCESS;
	}
	break;
	case WR_CHG_TRX_PATH:
	{
		struct rtw_trx_path_param *param = (struct rtw_trx_path_param*)chg_info;

		hstate = rtw_hal_cfg_trx_path(phl_info->hal,
		                              param->tx,
		                              param->tx_nss,
		                              param->rx,
		                              param->rx_nss);

		if (hstate == RTW_HAL_STATUS_SUCCESS)
			pstate = RTW_PHL_STATUS_SUCCESS;
	}
	break;
	case WR_CHG_STBC_CFG:
	{
		if (rlink == NULL) {
			PHL_WARN("%s: Target rlink is NULL!\n", __func__);
			break;
		}
		phl_init_proto_stbc_cap(rlink, phl_info, &rlink->protocol_cap);
	}
	break;
	case WR_CHG_MAX:
		PHL_TRACE(COMP_PHL_DBG, _PHL_ERR_,
				"%s: Unsupported case:%d, please check it\n",
				__func__, chg_id);
		break;
	default :
		PHL_TRACE(COMP_PHL_DBG, _PHL_ERR_,
				"%s: unknown chg_id(%d), please check it\n",
				__func__, chg_id);
		break;
	};
	return pstate;
}

#ifdef CONFIG_CMD_DISP
struct wr_chg_param {
	struct rtw_wifi_role_t *wrole;
	struct rtw_wifi_role_link_t *rlink;
	enum wr_chg_id id;
	u8 *info;
	u8 info_len;
};

enum rtw_phl_status
phl_wifi_role_chg_hdl(struct phl_info_t *phl_info, u8 *param)
{
	struct wr_chg_param *wr_chg = (struct wr_chg_param *)param;

	return phl_wifi_role_change(phl_info,
	                            wr_chg->wrole,
	                            wr_chg->rlink,
	                            wr_chg->id,
	                            wr_chg->info);
}

void phl_wifi_role_chg_done(void *drv_priv, u8 *cmd, u32 cmd_len,
						enum rtw_phl_status status)
{
	struct wr_chg_param *wr_chg = NULL;

	if (cmd == NULL || cmd_len == 0) {
		PHL_ERR("%s buf == NULL || buf_len == 0\n", __func__);
		_os_warn_on(1);
		return;
	}

	wr_chg = (struct wr_chg_param *)cmd;
	PHL_INFO("%s - id:%d .....\n", __func__, wr_chg->id);

	if (wr_chg->info)
		_os_kmem_free(drv_priv, wr_chg->info, wr_chg->info_len);

	_os_kmem_free(drv_priv, cmd, cmd_len);
	cmd = NULL;
}

enum rtw_phl_status
rtw_phl_cmd_wrole_change(void *phl,
                         struct rtw_wifi_role_t *wrole,
                         struct rtw_wifi_role_link_t *rlink,
                         enum wr_chg_id chg_id,
                         u8 *chg_info,
                         u8 chg_info_len,
                         enum phl_cmd_type cmd_type,
                         u32 cmd_timeout
)
{

	enum rtw_phl_status psts = RTW_PHL_STATUS_FAILURE;
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;
	void *drv = phl_to_drvpriv(phl_info);
	struct wr_chg_param *wr_chg = NULL;
	u32 wr_chg_len = 0;

	if (cmd_type == PHL_CMD_DIRECTLY) {
		psts = phl_wifi_role_change(phl_info, wrole, rlink, chg_id, chg_info);
		goto _exit;
	}

	wr_chg_len = sizeof(struct wr_chg_param);
	wr_chg = _os_kmem_alloc(drv, wr_chg_len);
	if (wr_chg == NULL) {
		PHL_ERR("%s: alloc wr_chg_param failed!\n", __func__);
		psts = RTW_PHL_STATUS_RESOURCE;
		goto _exit;
	}
	_os_mem_set(drv, wr_chg, 0, wr_chg_len);
	wr_chg->wrole = wrole;
	wr_chg->rlink = rlink;
	wr_chg->id = chg_id;

	wr_chg->info_len = chg_info_len;
	wr_chg->info = _os_kmem_alloc(drv, chg_info_len);
	if (wr_chg->info == NULL) {
		PHL_ERR("%s: alloc wr_chg_info failed!\n", __func__);
		psts = RTW_PHL_STATUS_RESOURCE;
		goto _err_info;
	}
	_os_mem_set(drv, wr_chg->info, 0, chg_info_len);
	_os_mem_cpy(drv, wr_chg->info, chg_info, chg_info_len);

	psts = phl_cmd_enqueue(phl_info,
/* don't care band, role change will go through all links and change settings */
	                       HW_BAND_0,
	                       MSG_EVT_ROLE_CHANGE,
	                       (u8*)wr_chg,
	                       wr_chg_len,
	                       phl_wifi_role_chg_done,
	                       cmd_type,
	                       cmd_timeout);

	if (is_cmd_failure(psts)) {
		/* Send cmd success, but wait cmd fail*/
		psts = RTW_PHL_STATUS_FAILURE;
		PHL_INFO("%s wr-id:%d failure\n", __func__, wrole->id);
	} else if (psts != RTW_PHL_STATUS_SUCCESS) {
		/* Send cmd fail */
		psts = RTW_PHL_STATUS_FAILURE;
		PHL_INFO("%s wr-id:%d failure\n", __func__, wrole->id);
		goto _err_cmd;
	} else {
		PHL_INFO("%s wr-id:%d success\n", __func__, wrole->id);
	}

	return psts;

_err_cmd:
	if (wr_chg->info)
		_os_kmem_free(drv, wr_chg->info, wr_chg->info_len);
_err_info:
	if (wr_chg)
		_os_kmem_free(drv, wr_chg, wr_chg_len);
_exit:
	return psts;
}

#else
enum rtw_phl_status
rtw_phl_cmd_wrole_change(void *phl,
                         struct rtw_wifi_role_t *wrole,
                         struct rtw_wifi_role_link_t *rlink,
                         enum wr_chg_id chg_id,
                         u8 *chg_info,
                         u8 chg_info_len,
                         enum phl_cmd_type cmd_type,
                         u32 cmd_timeout
)
{
	return phl_wifi_role_change((struct phl_info_t *)phl,
	                            wrole,
	                            rlink,
	                            chg_id,
	                            chg_info);
}
#endif /*CONFIG_CMD_DISP*/

enum rtw_phl_status
_phl_wifi_role_stop(struct phl_info_t *phl_i, struct rtw_wifi_role_t *wrole)
{
	enum rtw_phl_status psts = RTW_PHL_STATUS_FAILURE;
	u8 lidx;
	struct rtw_wifi_role_link_t *rlink;

	#ifdef CONFIG_RTW_SUPPORT_MBSSID_VAP
	if (wrole->type == PHL_RTYPE_VAP) {
		u32	param = 0;

		for (lidx = 0; lidx < wrole->rlink_num; lidx++) {
			rlink = get_rlink(wrole, lidx);
			PHL_PRINT("Stop MBSSID %u.\n", rlink->hw_mbssid);
			/* Configure port for M-BSSID */
			rtw_hal_role_cfg_ex(phl_i->hal, rlink, PCFG_MBSSID_EN, &param);
		}
	}
	#endif /* CONFIG_RTW_SUPPORT_MBSSID_VAP */

	wrole->active = false;
	psts = phl_wifi_role_free_stainfo_hw(phl_i, wrole);
	if (psts != RTW_PHL_STATUS_SUCCESS) {
		PHL_ERR("%s wr free stainfo_hw failed\n", __func__);
		_os_warn_on(1);
		goto _exit;
	}

	/*hw port cfg - mac_port_deinit*/
	for (lidx = 0; lidx < wrole->rlink_num; lidx++) {
		rlink = get_rlink(wrole, lidx);
		if (RTW_HAL_STATUS_SUCCESS != rtw_hal_role_deinit(phl_i->hal, rlink)) {
			PHL_ERR("%s wr deinit failed for link idx(%d), it may influence power consumption\n",
				__func__, lidx);
			psts = RTW_PHL_STATUS_FAILURE;
		}
	}
_exit:
	return psts;
}
#ifdef CONFIG_CMD_DISP
enum rtw_phl_status
phl_wifi_role_stop_hdl(struct phl_info_t *phl_info, u8 *param)
{
	struct rtw_wifi_role_t *wrole = (struct rtw_wifi_role_t *)param;

	return _phl_wifi_role_stop(phl_info, wrole);
}
void phl_wifi_role_stop_done(void *drv_priv, u8 *cmd, u32 cmd_len,
						enum rtw_phl_status status)
{
	if (cmd) {
		struct rtw_wifi_role_t *wrole = NULL;

		wrole = (struct rtw_wifi_role_t *)cmd;
		if (is_cmd_failure(status) && (RTW_PHL_STATUS_SUCCESS != status))
			PHL_ERR("%s wrole(%d) fail status(%d).....\n",
				__func__, wrole->id, status);
		else
			PHL_INFO("%s wrole(%d) success.....\n",
				__func__, wrole->id);
	}
}

enum rtw_phl_status
phl_wifi_role_stop(struct phl_info_t *phl_info, struct rtw_wifi_role_t *wrole)
{
	enum rtw_phl_status psts = RTW_PHL_STATUS_FAILURE;

	psts = phl_cmd_enqueue(phl_info,
	                       HW_BAND_0,
	                       MSG_EVT_ROLE_STOP,
	                       (u8 *)wrole,
	                       0,
	                       phl_wifi_role_stop_done,
	                       PHL_CMD_WAIT,
	                       0);

	if (is_cmd_failure(psts)) {
		/* Send cmd success, but wait cmd fail*/
		psts = RTW_PHL_STATUS_FAILURE;
		PHL_INFO("%s wr-id:%d failure\n", __func__, wrole->id);
	} else if (psts != RTW_PHL_STATUS_SUCCESS) {
		/* Send cmd fail */
		psts = RTW_PHL_STATUS_FAILURE;
		PHL_INFO("%s wr-id:%d failure\n", __func__, wrole->id);
	} else {
		PHL_INFO("%s wr-id:%d success\n", __func__, wrole->id);
	}

	return psts;
}
#else
enum rtw_phl_status
phl_wifi_role_stop(struct phl_info_t *phl_info, struct rtw_wifi_role_t *wrole)
{
	return _phl_wifi_role_stop(phl_info, wrole);
}
#endif
void rtw_phl_wifi_role_free(void *phl, u8 role_idx)
{
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;
	struct rtw_phl_com_t *phl_com = phl_info->phl_com;
	struct rtw_wifi_role_t *wrole = NULL;

	if (role_idx >= MAX_WIFI_ROLE_NUMBER) {
		PHL_ERR("%s invalid role index :%d\n", __func__, role_idx);
		return;
	}

	wrole = &phl_com->wifi_roles[role_idx];

	if (phl_wifi_role_stop(phl_info, wrole) != RTW_PHL_STATUS_SUCCESS) {
		PHL_ERR("%s role_stop failed :%d\n", __func__, role_idx);
		_os_warn_on(1);
	}

	if (phl_wifi_role_free_stainfo_sw(phl_info, wrole) != RTW_PHL_STATUS_SUCCESS) {
		PHL_ERR("%s wr free stainfo_sw failed\n", __func__);
		_os_warn_on(1);
	}

	if (phl_wifi_role_free_mld(phl_info, wrole) != RTW_PHL_STATUS_SUCCESS) {
		PHL_ERR("%s wr free mld failed\n", __func__);
		_os_warn_on(1);
	}

	_phl_wifi_role_free_sw(phl_info, wrole);
	PHL_DUMP_MR_EX(phl_info);
}

#ifdef CONFIG_CMD_DISP
enum rtw_phl_status
phl_register_mrc_module(struct phl_info_t *phl_info)
{
	enum rtw_phl_status phl_status = RTW_PHL_STATUS_FAILURE;
	struct mr_ctl_t *mr_ctl = phlcom_to_mr_ctrl(phl_info->phl_com);
	struct phl_bk_module_ops bk_ops = mr_ctl->bk_ops;
	struct phl_cmd_dispatch_engine *disp_eng = &(phl_info->disp_eng);
	u8 i = 0;

	for(i = 0; i < disp_eng->phy_num; i++)
	{
		phl_status = phl_disp_eng_register_module(phl_info, i,
						       PHL_MDL_MRC,
						       &bk_ops);
		if (RTW_PHL_STATUS_SUCCESS != phl_status) {
			PHL_ERR("%s register MRC module in cmd disp failed :%d\n", __func__, i+1);
			goto error_register_bk;
		}
	}

	return phl_status;

error_register_bk:
	while(i > 0){
		phl_status = phl_disp_eng_deregister_module(phl_info, --i,
							 PHL_MDL_MRC);
		if (RTW_PHL_STATUS_SUCCESS != phl_status) {
			PHL_ERR("%s deregister MRC module in cmd disp failed :%d\n", __func__, i+1);
		}
	}
	return RTW_PHL_STATUS_FAILURE;
}
#endif

/*
 * return role map of excluded role from suspension
 */
static u8 _phl_get_excld_susp_role_map(struct phl_info_t *phl_i, enum phl_role_susp_rsn rsn)
{
	u8 map = 0;

	switch (rsn) {
	#ifdef CONFIG_WOWLAN
	case PHL_ROLE_SUSPEND_RSN_WOW:
		map = phl_get_wow_excld_susp_role_map(phl_i);
		break;
	#endif /* CONFIG_WOWLAN */
	case PHL_ROLE_SUSPEND_RSN_DEV_SUSP:
	case PHL_ROLE_SUSPEND_RSN_RF_OFF:
	case PHL_ROLE_SUSPEND_RSN_MAX:
	default:
		break;
	}
	PHL_INFO("%s map(0x%x)\n", __func__, map);
	return map;
}

enum rtw_phl_status
phl_role_recover(struct phl_info_t *phl_info)
{
	u8 role_idx;
	struct rtw_wifi_role_t *wrole;
	struct rtw_phl_mld_t *mld;
	enum rtw_phl_status pstatus;

	for (role_idx = 0; role_idx < MAX_WIFI_ROLE_NUMBER; role_idx++) {
		wrole = phl_get_wrole_by_ridx(phl_info, role_idx);
		if (wrole == NULL)
			continue;
		if (!TEST_STATUS_FLAG(wrole->status, WR_STATUS_SUSPEND) &&
		    !TEST_STATUS_FLAG(wrole->status, WR_STATUS_HW_ALLOC_FAIL))
			continue;

		PHL_INFO("%s with role_idx %d status 0x%x\n",
			 __func__, role_idx, wrole->status);

		mld = rtw_phl_get_mld_self(phl_info, wrole);
		if (mld == NULL) {
			PHL_ERR("%s rid(%d), Failed to get self MLD\n",
				__func__, role_idx);
			continue;
		}

		pstatus = _phl_wifi_role_start(phl_info, wrole, mld);
		if (pstatus != RTW_PHL_STATUS_SUCCESS) {
			PHL_ERR("%s wifi role start failed\n", __func__);
			return RTW_PHL_STATUS_FAILURE;
		}

		if (wrole->target_type != PHL_RTYPE_NONE) {
			PHL_INFO("%s with role_idx %d change to role type %d\n", __func__, role_idx, wrole->target_type);
			phl_wifi_role_change(phl_info, wrole, NULL, WR_CHG_TYPE, (u8 *)&wrole->target_type);
			wrole->target_type = PHL_RTYPE_NONE;
		}

		CLEAR_STATUS_FLAG(wrole->status,
				  WR_STATUS_SUSPEND | WR_STATUS_HW_ALLOC_FAIL);
	}

	return RTW_PHL_STATUS_SUCCESS;
}

enum rtw_phl_status
phl_cmd_role_recover(struct phl_info_t *phl_info)
{
	enum rtw_phl_status pstatus = RTW_PHL_STATUS_FAILURE;

#ifdef CONFIG_CMD_DISP
	pstatus = phl_cmd_enqueue(phl_info, HW_BAND_0, MSG_EVT_ROLE_RECOVER, NULL, 0, NULL, PHL_CMD_WAIT, 0);
	if (is_cmd_failure(pstatus)) {
		/* Send cmd success, but wait cmd fail*/
		pstatus = RTW_PHL_STATUS_FAILURE;
	} else if (pstatus != RTW_PHL_STATUS_SUCCESS) {
		/* Send cmd fail */
		pstatus = RTW_PHL_STATUS_FAILURE;
	}
#else
	pstatus = phl_role_recover(phl_info);
#endif
	return pstatus;
}

static void _phl_role_suspend_done(void *drv_priv, u8 *cmd, u32 cmd_len, enum rtw_phl_status status)
{
	if (cmd) {
		_os_kmem_free(drv_priv, cmd, cmd_len);
		cmd = NULL;
		PHL_INFO("%s.....\n", __func__);
	}
}


enum rtw_phl_status
phl_role_suspend(struct phl_info_t *phl_info, enum phl_role_susp_rsn rsn)
{
	u8 role_idx;
	struct rtw_wifi_role_t *wrole;
	enum rtw_phl_status pstatus;
	u8 excld_map = 0;

	excld_map = _phl_get_excld_susp_role_map(phl_info, rsn);
	for (role_idx = 0; role_idx < MAX_WIFI_ROLE_NUMBER; role_idx++) {
		wrole = phl_get_wrole_by_ridx(phl_info, role_idx);
		if (wrole == NULL || !wrole->active)
			continue;
		if (excld_map & BIT(wrole->id))
			continue;
		PHL_INFO("%s with role_idx %d\n", __func__, role_idx);

		pstatus = _phl_wifi_role_stop(phl_info, wrole);
		SET_STATUS_FLAG(wrole->status, WR_STATUS_SUSPEND);

		if (pstatus != RTW_PHL_STATUS_SUCCESS) {
			PHL_ERR("%s wifi role stop failed\n", __func__);
			return RTW_PHL_STATUS_FAILURE;
		}
	}

	return RTW_PHL_STATUS_SUCCESS;
}
enum rtw_phl_status
phl_cmd_role_suspend(struct phl_info_t *phl_info, enum phl_role_susp_rsn rsn)
{
	enum rtw_phl_status pstatus = RTW_PHL_STATUS_FAILURE;

#ifdef CONFIG_CMD_DISP
	enum phl_role_susp_rsn *param = NULL;
	u32 param_len;

	param_len = sizeof(enum phl_role_susp_rsn);
	param = _os_kmem_alloc(phl_to_drvpriv(phl_info), param_len);
	if (param == NULL) {
		PHL_ERR("%s: alloc param failed!\n", __func__);
		goto _exit;
	}
	_os_mem_cpy(phl_to_drvpriv(phl_info), param, &rsn, param_len);
	pstatus = phl_cmd_enqueue(phl_info,
				  HW_BAND_0,
				  MSG_EVT_ROLE_SUSPEND,
				  (u8 *)param,
				  param_len,
				  _phl_role_suspend_done,
				  PHL_CMD_WAIT,
				  0);
	if (is_cmd_failure(pstatus)) {
		/* Send cmd success, but wait cmd fail*/
		pstatus = RTW_PHL_STATUS_FAILURE;
	} else if (pstatus != RTW_PHL_STATUS_SUCCESS) {
		/* Send cmd fail */
		_os_kmem_free(phl_to_drvpriv(phl_info), param, param_len);
		pstatus = RTW_PHL_STATUS_FAILURE;
	}
_exit:
#else
	pstatus = phl_role_suspend(phl_info, rsn);
#endif
	return pstatus;
}

u16 phl_role_get_bcn_intvl(struct phl_info_t *phl,
                           struct rtw_wifi_role_t *wrole,
                           struct rtw_wifi_role_link_t *rlink)
{
	u16 bcn_intvl = 0;

#ifdef RTW_PHL_BCN
	if (rtw_phl_role_is_ap_category(wrole)) {
		bcn_intvl = (u16)rlink->bcn_cmn.bcn_interval;
	} else
#endif
	if (rtw_phl_role_is_client_category(wrole)) {
		struct rtw_phl_stainfo_t *sta = rtw_phl_get_stainfo_self(phl, rlink);
		bcn_intvl = sta->asoc_cap.bcn_interval;
	} else {
		PHL_WARN("%s() Unknown category, role id(%d), rlink(%d), type(%d)\n",
			__func__, wrole->id, rlink->id, wrole->type);
	}
	return bcn_intvl;
}

enum rtw_phl_status
phl_wifi_role_macid_all_pause(struct phl_info_t *phl_info, struct rtw_wifi_role_t *wrole, bool pause)
{
	enum rtw_hal_status hstatus = RTW_HAL_STATUS_SUCCESS;
	enum rtw_phl_status pstatus = RTW_PHL_STATUS_SUCCESS;
	u8 mutli_macid = false;

	switch (wrole->type) {
	case PHL_RTYPE_NONE:
	case PHL_RTYPE_STATION:
	case PHL_RTYPE_ADHOC:
	case PHL_RTYPE_P2P_DEVICE:
	case PHL_RTYPE_P2P_GC:
		mutli_macid = false;
		break;
	default:
		mutli_macid = true;
		break;
	}

	if (mutli_macid) {
		void *drv = phl_to_drvpriv(phl_info);
		struct macid_ctl_t *mctrl = phl_to_mac_ctrl(phl_info);
		u32 macid_pause[PHL_MACID_MAX_ARRAY_NUM] = {0};

		_os_spinlock(phl_to_drvpriv(phl_info), &mctrl->lock, _bh, NULL);
		_os_mem_cpy(drv, macid_pause, &mctrl->wifi_role_usedmap[wrole->id][0], PHL_MACID_MAX_ARRAY_NUM);
		_os_spinunlock(phl_to_drvpriv(phl_info), &mctrl->lock, _bh, NULL);
		hstatus = rtw_hal_set_macid_grp_pause(phl_info->hal, macid_pause, PHL_MACID_MAX_ARRAY_NUM, pause);
		if (hstatus != RTW_HAL_STATUS_SUCCESS) {
			PHL_INFO("%s fail(hstatus=%d)\n", __func__, hstatus);
			pstatus = RTW_PHL_STATUS_FAILURE;
		}
	} else {
		struct rtw_phl_mld_t *self_mld = rtw_phl_get_mld_self((void *)phl_info, wrole);
		struct rtw_phl_stainfo_t *self_sta = NULL;
		u8 idx = 0;

		for (idx = 0; idx < wrole->rlink_num; idx++) {
			self_sta = self_mld->phl_sta[idx];

			hstatus = rtw_hal_set_macid_pause(phl_info->hal, self_sta->macid, pause);
			if (hstatus != RTW_HAL_STATUS_SUCCESS) {
				PHL_INFO("%s fail(hstatus=%d)\n", __func__, hstatus);
				pstatus = RTW_PHL_STATUS_FAILURE;
			}
		}
	}

	return pstatus;
}

enum rtw_phl_status
phl_get_cur_tsf(void *phl,
	struct rtw_phl_port_tsf *tsf)
{
	enum rtw_hal_status hstatus = RTW_HAL_STATUS_FAILURE;
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;

	hstatus = rtw_hal_get_tsf(phl_info->hal,
			tsf->hwband,
			tsf->port,
			&tsf->tsf_h,
			&tsf->tsf_l);
	if (RTW_HAL_STATUS_SUCCESS == hstatus)
		return RTW_PHL_STATUS_SUCCESS;
	else
		return RTW_PHL_STATUS_FAILURE;
}

enum rtw_phl_status
rtl_phl_cmd_get_cur_tsf(void *phl,
                        struct rtw_phl_port_tsf *tsf,
                        struct rtw_wifi_role_link_t *rlink,
                        enum phl_cmd_type cmd_type,
                        u32 cmd_timeout)
{
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;
	enum rtw_phl_status psts = RTW_PHL_STATUS_FAILURE;
	u8 hw_band = rlink->hw_band;
	u8 hw_port = rlink->hw_port;

	tsf->port = hw_port;
#ifdef CONFIG_CMD_DISP

	if (cmd_type == PHL_CMD_DIRECTLY) {
		psts = phl_get_cur_tsf(phl_info, tsf);
		return psts;
	}

	psts = phl_cmd_enqueue(phl_info,
				hw_band,
				MSG_EVT_GET_CUR_TSF,
				(u8*)tsf,
				sizeof(struct rtw_phl_port_tsf),
				NULL,
				cmd_type,
				cmd_timeout);
	if (is_cmd_failure(psts)) {
		/* Send cmd success, but wait cmd fail*/
		psts = RTW_PHL_STATUS_FAILURE;
	} else if (psts != RTW_PHL_STATUS_SUCCESS) {
		/* Send cmd fail */
		psts = RTW_PHL_STATUS_FAILURE;
	}
#else
	psts = phl_get_cur_tsf(phl_info, tsf);
#endif
	return psts;
}

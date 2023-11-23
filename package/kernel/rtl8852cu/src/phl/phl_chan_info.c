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
#define _PHL_CHAN_INFO_C_
#include "phl_headers.h"

#ifdef CONFIG_PHL_CHANNEL_INFO
static enum rtw_phl_status
_chinfo_chk_pkt_offload(struct phl_info_t *phl_info,
				struct rtw_chinfo_action_parm *act_param, u8 *id)
{
	enum rtw_phl_status sts = RTW_PHL_STATUS_SUCCESS;
	struct rtw_pkt_ofld_null_info null_info = {0};
	void *d = phl_to_drvpriv(phl_info);
	u32 token = 0;

	if (act_param->mode != CHINFO_MODE_ACK)
		goto exit;

	*id = phl_pkt_ofld_get_id(phl_info, act_param->sta->macid,
							PKT_TYPE_NULL_DATA);
	if (NOT_USED == *id) {
		_os_mem_cpy(d, &(null_info.a1[0]), &(act_param->sta->mac_addr[0]),
						MAC_ADDRESS_LENGTH);

		_os_mem_cpy(d,&(null_info.a2[0]), &(act_param->sta->rlink[0].mac_addr[0]),
						MAC_ADDRESS_LENGTH);

		_os_mem_cpy(d, &(null_info.a3[0]), &(act_param->sta->mac_addr[0]),
						MAC_ADDRESS_LENGTH);

		sts = rtw_phl_pkt_ofld_request(phl_info, act_param->sta->macid,
							PKT_TYPE_NULL_DATA, &token, &null_info, __func__);

		if (sts != RTW_PHL_STATUS_SUCCESS)
			goto exit;

		*id = phl_pkt_ofld_get_id(phl_info, act_param->sta->macid,
							PKT_TYPE_NULL_DATA);
	}
exit:
	return sts;
}

enum rtw_phl_status
_phl_cfg_chinfo(struct phl_info_t *phl_info,
			    struct rtw_chinfo_action_parm *act_param)
{
	enum rtw_phl_status sts = RTW_PHL_STATUS_SUCCESS;
	enum rtw_hal_status hsts = RTW_HAL_STATUS_SUCCESS;
	struct rtw_phl_com_t *phl_com = phl_info->phl_com;
	struct rtw_chinfo_cur_parm *cur_param = phl_com->cur_parm;
	struct rtw_wifi_role_link_t *rlink = act_param->sta->rlink;
	u8 pkt_id = NOT_USED;

	switch (act_param->act) {
	case CHINFO_ACT_EN:
		/* for light mode(from ppdu sts) */
		if (act_param->enable == true &&
			act_param->mode == CHINFO_MODE_ACK &&
			act_param->enable_mode == CHINFO_EN_LIGHT_MODE
			&& cur_param->num == 0)
			rtw_phl_mr_set_rxfltr_type_by_mode(phl_info, rlink,
						RX_FLTR_TYPE_MODE_CHAN_INFO_EN);
		else if (act_param->enable == false) {
			if (cur_param->num == 0) {
				PHL_ERR("[CHAN INFO]%s: enable before disable\n", __func__);
				sts = RTW_PHL_STATUS_FAILURE;
				goto exit;
			} else {
				if (cur_param->action_parm.mode == CHINFO_MODE_ACK &&
					cur_param->action_parm.enable_mode == CHINFO_EN_LIGHT_MODE &&
					cur_param->num == 1)
					rtw_phl_mr_set_rxfltr_type_by_mode(phl_info, rlink,
						RX_FLTR_TYPE_MODE_CHAN_INFO_DIS);
			}
		}

		sts = _chinfo_chk_pkt_offload(phl_info, act_param, &pkt_id);
		if (sts != RTW_PHL_STATUS_SUCCESS)
			goto exit;

		hsts = rtw_hal_ch_info_en(phl_info->hal, act_param, pkt_id);
		if (hsts != RTW_HAL_STATUS_SUCCESS)
			sts = RTW_PHL_STATUS_FAILURE;
		break;
	case CHINFO_ACT_CFG:
		sts = rtw_hal_cfg_chinfo(phl_info->hal, act_param);
		if (sts != RTW_PHL_STATUS_SUCCESS)
			PHL_ERR("%s: cfg_chinfo fail\n", __func__);
		break;
	default:
		PHL_ERR("%s: unknow act(%d)\n", __func__, act_param->act);
		sts = RTW_PHL_STATUS_INVALID_PARAM;
		break;
	}
exit:
	return sts;
}

#ifdef CONFIG_CMD_DISP
enum rtw_phl_status
phl_cmd_cfg_chinfo_hdl(struct phl_info_t *phl_info, u8 *param)
{
	struct rtw_chinfo_action_parm *act_parm = (struct rtw_chinfo_action_parm *)param;

	return _phl_cfg_chinfo(phl_info, act_parm);
}

static void _phl_cfg_chinfo_done(void *drv_priv, u8 *cmd, u32 cmd_len, enum rtw_phl_status status)
{
	if (cmd) {
		_os_kmem_free(drv_priv, cmd, cmd_len);
		cmd = NULL;
		PHL_INFO("%s.....\n", __func__);
	}
}

enum rtw_phl_status
_phl_cmd_cfg_chinfo(struct phl_info_t *phl_info,
			    struct rtw_chinfo_action_parm *act_parm,
			    enum phl_cmd_type cmd_type,
			    u32 cmd_timeout)
{
	enum rtw_phl_status sts = RTW_PHL_STATUS_FAILURE;
	void *d = phl_to_drvpriv(phl_info);
	struct rtw_chinfo_action_parm *param = NULL;
	u32 param_len;

	if (cmd_type == PHL_CMD_DIRECTLY)
		return _phl_cfg_chinfo(phl_info, act_parm);

	param_len = sizeof(struct rtw_chinfo_action_parm);
	param = _os_kmem_alloc(phl_to_drvpriv(phl_info), param_len);
	if (param == NULL) {
		PHL_ERR("%s: alloc param failed!\n", __func__);
		goto _exit;
	}

	_os_mem_cpy(d, param, act_parm, sizeof(struct rtw_chinfo_action_parm));

	sts = phl_cmd_enqueue(phl_info,
	                      act_parm->sta->rlink->hw_band,
	                      MSG_EVT_CFG_CHINFO,
	                      (u8 *)param,
	                      param_len,
	                      _phl_cfg_chinfo_done,
	                      cmd_type,
	                      cmd_timeout);
	if (is_cmd_failure(sts)) {
		/* Send cmd success, but wait cmd fail*/
		sts = RTW_PHL_STATUS_FAILURE;
	} else if (sts != RTW_PHL_STATUS_SUCCESS) {
		/* Send cmd fail */
		_os_kmem_free(phl_to_drvpriv(phl_info), param, param_len);
		sts = RTW_PHL_STATUS_FAILURE;
	}

_exit:
	return sts;
}

#endif

enum rtw_phl_status rtw_phl_cmd_cfg_chinfo(void *phl,
					   struct rtw_chinfo_action_parm *act_parm,
					   enum phl_cmd_type cmd_type,
					   u32 cmd_timeout)
{
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;

#ifdef CONFIG_CMD_DISP
	return _phl_cmd_cfg_chinfo(phl_info, act_parm, cmd_type, cmd_timeout);
#else
	return _phl_cfg_chinfo(phl_info, act_parm);
#endif
}

enum rtw_phl_status rtw_phl_query_chan_info(void *phl, u32 buf_len,
	u8* chan_info_buffer, u32 *length, struct csi_header_t *csi_header)
{
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;
	void *drv_priv = phl_to_drvpriv(phl_info);
	struct rtw_phl_com_t *phl_com = phl_info->phl_com;
	struct chan_info_t *chan_info_pkt_latest = NULL;
	enum rtw_phl_status status = RTW_PHL_STATUS_FAILURE;

	if(chan_info_buffer == NULL){
		PHL_ERR("buffer is not ready.\n");
		return status;
	}

	/* Get the latest channel info from busy queue. */
	chan_info_pkt_latest = rtw_phl_query_busy_chaninfo(drv_priv, phl_com);
	if (chan_info_pkt_latest != NULL) {
		if (buf_len < chan_info_pkt_latest->length) {
			PHL_ERR("%s: Buffer length not sufficient! \n", __func__);
			rtw_phl_enqueue_idle_chaninfo(drv_priv, phl_com, chan_info_pkt_latest);
			return status;
		}
		/* copy raw data resources. */
		_os_mem_cpy(drv_priv, chan_info_buffer,
			chan_info_pkt_latest->chan_info_buffer, chan_info_pkt_latest->length);
		_os_mem_cpy(drv_priv, csi_header,
			&chan_info_pkt_latest->csi_header, sizeof(struct csi_header_t));
		*length = chan_info_pkt_latest->length;

		rtw_phl_enqueue_idle_chaninfo(drv_priv, phl_com, chan_info_pkt_latest);
		status = RTW_PHL_STATUS_SUCCESS;
	} else {
		PHL_TRACE(COMP_PHL_CHINFO, _PHL_DEBUG_, "%s: There is no channel info packet.\n", __func__);
	}
	return status;
}

/*channel info packet pool init/deinit*/
static void _phl_chaninfo_deinit (struct phl_info_t *phl_info)
{
	struct rx_chan_info_pool *chan_info_pool = NULL;
	u8* chan_info_buffer = NULL;
	u32 buf_len, i = 0;
	FUNCIN();

	chan_info_pool = (struct rx_chan_info_pool *)phl_info->phl_com->chan_info_pool;
	if (NULL != chan_info_pool) {
		_os_spinlock_free(phl_to_drvpriv(phl_info),
					&chan_info_pool->idle_lock);
		_os_spinlock_free(phl_to_drvpriv(phl_info),
					&chan_info_pool->busy_lock);
		for (i = 0; i < CHAN_INFO_PKT_TOTAL; i++) {
			chan_info_buffer = chan_info_pool->channl_info_pkt[i].chan_info_buffer;
			if (chan_info_buffer != NULL) {
				_os_mem_free(phl_to_drvpriv(phl_info), chan_info_buffer,
					CHAN_INFO_MAX_SIZE);
				chan_info_pool->channl_info_pkt[i].chan_info_buffer = NULL;
			}
		}
		buf_len = sizeof(*chan_info_pool);
		_os_mem_free(phl_to_drvpriv(phl_info), chan_info_pool, buf_len);
	}

	if (NULL != phl_info->phl_com->cur_parm)
		_os_mem_free(phl_to_drvpriv(phl_info), phl_info->phl_com->cur_parm,
					sizeof(struct rtw_chinfo_cur_parm));
	FUNCOUT();
}

static enum rtw_phl_status _phl_chaninfo_init(struct phl_info_t *phl_info)
{
	enum rtw_phl_status pstatus = RTW_PHL_STATUS_SUCCESS;
	struct rx_chan_info_pool *chan_info_pool = NULL;
	struct chan_info_t *chan_info_pkt = NULL;
	u32 buf_len = 0, i = 0;
	FUNCIN_WSTS(pstatus);

	buf_len = sizeof(*chan_info_pool);
	chan_info_pool = _os_mem_alloc(phl_to_drvpriv(phl_info), buf_len);

	if (NULL != chan_info_pool) {
		_os_mem_set(phl_to_drvpriv(phl_info), chan_info_pool, 0, buf_len);
		INIT_LIST_HEAD(&chan_info_pool->idle);
		INIT_LIST_HEAD(&chan_info_pool->busy);
		_os_spinlock_init(phl_to_drvpriv(phl_info),
					&chan_info_pool->idle_lock);
		_os_spinlock_init(phl_to_drvpriv(phl_info),
					&chan_info_pool->busy_lock);
		chan_info_pool->idle_cnt = 0;

		for (i = 0; i < CHAN_INFO_PKT_TOTAL; i++) {
			chan_info_pkt = &chan_info_pool->channl_info_pkt[i];
			chan_info_pkt->chan_info_buffer = _os_mem_alloc(phl_to_drvpriv(phl_info),
				CHAN_INFO_MAX_SIZE);
			if (NULL != chan_info_pkt->chan_info_buffer) {
				chan_info_pkt->length = 0;
				INIT_LIST_HEAD(&chan_info_pkt->list);
				list_add_tail(&chan_info_pkt->list, &chan_info_pool->idle);
				chan_info_pool->idle_cnt++;
			} else {
				pstatus = RTW_PHL_STATUS_RESOURCE;
				goto exit;
				break;
			}
		}
		phl_info->phl_com->chan_info_pool = chan_info_pool;
	} else {
		pstatus = RTW_PHL_STATUS_RESOURCE;
		goto exit;
	}

	phl_info->phl_com->cur_parm = _os_mem_alloc(phl_to_drvpriv(phl_info),
				sizeof(struct rtw_chinfo_cur_parm));
	if (NULL == phl_info->phl_com->cur_parm) {
		pstatus = RTW_PHL_STATUS_RESOURCE;
		goto exit;
	}
exit:
	if (RTW_PHL_STATUS_SUCCESS != pstatus)
		_phl_chaninfo_deinit(phl_info);
	FUNCOUT_WSTS(pstatus);

	return pstatus;
}

enum rtw_phl_status phl_chaninfo_init(struct phl_info_t *phl_info)
{
	enum rtw_phl_status phl_status = RTW_PHL_STATUS_FAILURE;

	phl_status = _phl_chaninfo_init(phl_info);
	if (phl_status != RTW_PHL_STATUS_SUCCESS)
		PHL_ERR("channel info pool allocate fail\n");

	return phl_status;
}

void phl_chaninfo_deinit(struct phl_info_t *phl_info)
{
	_phl_chaninfo_deinit(phl_info);
}

u32 rtw_phl_get_chaninfo_idle_number(void* drv_priv, struct rtw_phl_com_t *phl_com)
{
	u32 number;
	struct rx_chan_info_pool *chan_info_pool = NULL;

	chan_info_pool = phl_com->chan_info_pool;
	number = chan_info_pool->idle_cnt;
	return number;
}

u32 rtw_phl_get_chaninfo_busy_number(void* drv_priv, struct rtw_phl_com_t *phl_com)
{
	u32 number;
	struct rx_chan_info_pool *chan_info_pool = NULL;

	chan_info_pool = phl_com->chan_info_pool;
	number = chan_info_pool->busy_cnt;
	return number;
}

struct chan_info_t *rtw_phl_query_idle_chaninfo(void* drv_priv, struct rtw_phl_com_t *phl_com)
{
	struct rx_chan_info_pool *chan_info_pool = NULL;
	struct chan_info_t *chan_info_pkt = NULL;

	chan_info_pool = phl_com->chan_info_pool;

	_os_spinlock(drv_priv, &chan_info_pool->idle_lock, _bh, NULL);
	if (false == list_empty(&chan_info_pool->idle)) {
		chan_info_pkt = list_first_entry(&chan_info_pool->idle,
			struct chan_info_t, list);
		list_del(&chan_info_pkt->list);
		chan_info_pool->idle_cnt--;
	}
	_os_spinunlock(drv_priv, &chan_info_pool->idle_lock, _bh, NULL);

	return chan_info_pkt;
}

struct chan_info_t *rtw_phl_query_busy_chaninfo(void* drv_priv, struct rtw_phl_com_t *phl_com)
{
	struct rx_chan_info_pool *chan_info_pool = NULL;
	struct chan_info_t *chan_info_pkt = NULL;

	chan_info_pool = phl_com->chan_info_pool;

	_os_spinlock(drv_priv, &chan_info_pool->busy_lock, _bh, NULL);
	if (false == list_empty(&chan_info_pool->busy)) {
		chan_info_pkt = list_first_entry(&chan_info_pool->busy,
			struct chan_info_t, list);
		list_del(&chan_info_pkt->list);
		chan_info_pool->busy_cnt--;
	}
	_os_spinunlock(drv_priv, &chan_info_pool->busy_lock, _bh, NULL);

	return chan_info_pkt;
}

struct chan_info_t *rtw_phl_query_busy_chaninfo_latest(void* drv_priv, struct rtw_phl_com_t *phl_com)
{
	struct rx_chan_info_pool *chan_info_pool = NULL;
	struct chan_info_t *chan_info_pkt = NULL;

	chan_info_pool = phl_com->chan_info_pool;

	_os_spinlock(drv_priv, &chan_info_pool->busy_lock, _bh, NULL);
	if (false == list_empty(&chan_info_pool->busy)) {
		chan_info_pkt = list_last_entry(&chan_info_pool->busy,
			struct chan_info_t, list);
		list_del(&chan_info_pkt->list);
		chan_info_pool->busy_cnt--;
	}
	_os_spinunlock(drv_priv, &chan_info_pool->busy_lock, _bh, NULL);

	return chan_info_pkt;
}


void rtw_phl_enqueue_idle_chaninfo(void* drv_priv, struct rtw_phl_com_t *phl_com,
				struct chan_info_t *chan_info_pkt)
{
	struct rx_chan_info_pool *chan_info_pool = NULL;

	chan_info_pool = phl_com->chan_info_pool;

	_os_spinlock(drv_priv, &chan_info_pool->idle_lock, _bh, NULL);
	_os_mem_set(drv_priv, &chan_info_pkt->csi_header, 0,
		sizeof( chan_info_pkt->csi_header));
	_os_mem_set(drv_priv, chan_info_pkt->chan_info_buffer, 0,
		CHAN_INFO_MAX_SIZE);
	chan_info_pkt->length = 0;
	INIT_LIST_HEAD(&chan_info_pkt->list);
	list_add_tail(&chan_info_pkt->list, &chan_info_pool->idle);
	chan_info_pool->idle_cnt++;
	_os_spinunlock(drv_priv, &chan_info_pool->idle_lock, _bh, NULL);
}

struct chan_info_t * rtw_phl_recycle_busy_chaninfo(void* drv_priv,
	struct rtw_phl_com_t *phl_com, struct chan_info_t *chan_info_pkt)
{
	struct rx_chan_info_pool *chan_info_pool = NULL;
	struct chan_info_t *chan_info_pkt_recycle = NULL;

	chan_info_pool = phl_com->chan_info_pool;

	_os_spinlock(drv_priv, &chan_info_pool->busy_lock, _bh, NULL);
	/* enqueue the latest first. */
	INIT_LIST_HEAD(&chan_info_pkt->list);
	list_add_tail(&chan_info_pkt->list, &chan_info_pool->busy);
	chan_info_pool->busy_cnt++;

	/* if the number is greater than max, dequeue the oldest one.*/
	if (chan_info_pool->busy_cnt > MAX_CHAN_INFO_PKT_KEEP) {
		chan_info_pkt_recycle = list_first_entry(&chan_info_pool->busy,
			struct chan_info_t, list);
		list_del(&chan_info_pkt_recycle->list);
		chan_info_pool->busy_cnt--;
	}
	_os_spinunlock(drv_priv, &chan_info_pool->busy_lock, _bh, NULL);

	return chan_info_pkt_recycle;
}

#endif /* CONFIG_PHL_CHANNEL_INFO */

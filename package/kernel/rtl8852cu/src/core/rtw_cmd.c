/******************************************************************************
 *
 * Copyright(c) 2007 - 2021 Realtek Corporation.
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
#define _RTW_CMD_C_

#include <drv_types.h>

#ifndef DBG_CMD_EXECUTE
	#define DBG_CMD_EXECUTE 0
#endif

/*
Caller and the rtw_cmd_thread can protect cmd_q by spin_lock.
No irqsave is necessary.
*/
u32 rtw_init_cmd_priv(struct dvobj_priv *dvobj)
{
	u32 res = _SUCCESS;
	struct cmd_priv *pcmdpriv = &dvobj->cmdpriv;

	pcmdpriv->dvobj = dvobj;
	#if 0 /*#ifdef CONFIG_CORE_CMD_THREAD*/
	_rtw_init_sema(&(pcmdpriv->cmd_queue_sema), 0);
	_rtw_init_sema(&(pcmdpriv->start_cmdthread_sema), 0);
	_rtw_init_queue(&(pcmdpriv->cmd_queue));
	#endif

	/* allocate DMA-able/Non-Page memory for cmd_buf and rsp_buf */

	pcmdpriv->cmd_seq = 1;

	pcmdpriv->cmd_allocated_buf = rtw_zmalloc(MAX_CMDSZ + CMDBUFF_ALIGN_SZ);

	if (pcmdpriv->cmd_allocated_buf == NULL) {
		res = _FAIL;
		goto exit;
	}

	pcmdpriv->cmd_buf = pcmdpriv->cmd_allocated_buf + CMDBUFF_ALIGN_SZ - ((SIZE_PTR)(pcmdpriv->cmd_allocated_buf) & (CMDBUFF_ALIGN_SZ - 1));

	pcmdpriv->rsp_allocated_buf = rtw_zmalloc(MAX_RSPSZ + 4);

	if (pcmdpriv->rsp_allocated_buf == NULL) {
		res = _FAIL;
		goto exit;
	}

	pcmdpriv->rsp_buf = pcmdpriv->rsp_allocated_buf  +  4 - ((SIZE_PTR)(pcmdpriv->rsp_allocated_buf) & 3);

	pcmdpriv->cmd_issued_cnt = 0;

	_rtw_mutex_init(&pcmdpriv->sctx_mutex);

	ATOMIC_SET(&pcmdpriv->event_seq, 0);
	pcmdpriv->evt_done_cnt = 0;
exit:
	return res;

}

void rtw_free_cmd_priv(struct dvobj_priv *dvobj)
{
	struct cmd_priv *pcmdpriv = &dvobj->cmdpriv;

	#if 0 /*#ifdef CONFIG_CORE_CMD_THREAD*/
	_rtw_spinlock_free(&(pcmdpriv->cmd_queue.lock));
	_rtw_free_sema(&(pcmdpriv->cmd_queue_sema));
	_rtw_free_sema(&(pcmdpriv->start_cmdthread_sema));
	#endif
	if (pcmdpriv->cmd_allocated_buf)
		rtw_mfree(pcmdpriv->cmd_allocated_buf, MAX_CMDSZ + CMDBUFF_ALIGN_SZ);

	if (pcmdpriv->rsp_allocated_buf)
		rtw_mfree(pcmdpriv->rsp_allocated_buf, MAX_RSPSZ + 4);

	_rtw_mutex_free(&pcmdpriv->sctx_mutex);
}

static int rtw_cmd_filter(struct cmd_priv *pcmdpriv, struct cmd_obj *cmd_obj)
{
	u8 bAllow = _FALSE; /* set to _TRUE to allow enqueuing cmd when hw_init_completed is _FALSE */
	struct dvobj_priv *dvobj = pcmdpriv->dvobj;


	if (cmd_obj->cmdcode == CMD_SET_CHANPLAN)
		bAllow = _TRUE;

	if (cmd_obj->no_io)
		bAllow = _TRUE;

	return _SUCCESS;
}

/*
Calling Context:

rtw_enqueue_cmd can only be called between kernel thread,
since only spin_lock is used.

ISR/Call-Back functions can't call this sub-function.

*/
#ifdef DBG_CMD_QUEUE
extern u8 dump_cmd_id;
#endif

#if 0 /*#ifdef CONFIG_CORE_CMD_THREAD*/
static sint _rtw_enqueue_cmd(_queue *queue, struct cmd_obj *obj, bool to_head)
{
	unsigned long sp_flags;

	if (obj == NULL)
		goto exit;

	/* _rtw_spinlock_bh(&queue->lock); */
	_rtw_spinlock_irq(&queue->lock, &sp_flags);

	if (to_head)
		rtw_list_insert_head(&obj->list, &queue->queue);
	else
		rtw_list_insert_tail(&obj->list, &queue->queue);

#ifdef DBG_CMD_QUEUE
	if (dump_cmd_id) {
		RTW_INFO("%s===> cmdcode:0x%02x\n", __FUNCTION__, obj->cmdcode);
		if (obj->cmdcode == CMD_SET_MLME_EVT) {
			if (obj->parmbuf) {
				struct rtw_evt_header *evt_hdr = (struct rtw_evt_header *)(obj->parmbuf);
				RTW_INFO("evt_hdr->id:%d\n", evt_hdr->id);
			}
		}
		if (obj->cmdcode == CMD_SET_DRV_EXTRA) {
			if (obj->parmbuf) {
				struct drvextra_cmd_parm *pdrvextra_cmd_parm = (struct drvextra_cmd_parm *)(obj->parmbuf);
				RTW_INFO("pdrvextra_cmd_parm->ec_id:0x%02x\n", pdrvextra_cmd_parm->ec_id);
			}
		}
	}

	if (queue->queue.prev->next != &queue->queue) {
		RTW_INFO("[%d] head %p, tail %p, tail->prev->next %p[tail], tail->next %p[head]\n", __LINE__,
			&queue->queue, queue->queue.prev, queue->queue.prev->prev->next, queue->queue.prev->next);

		RTW_INFO("==========%s============\n", __FUNCTION__);
		RTW_INFO("head:%p,obj_addr:%p\n", &queue->queue, obj);
		RTW_INFO("padapter: %p\n", obj->padapter);
		RTW_INFO("cmdcode: 0x%02x\n", obj->cmdcode);
		RTW_INFO("res: %d\n", obj->res);
		RTW_INFO("parmbuf: %p\n", obj->parmbuf);
		RTW_INFO("cmdsz: %d\n", obj->cmdsz);
		RTW_INFO("rsp: %p\n", obj->rsp);
		RTW_INFO("rspsz: %d\n", obj->rspsz);
		RTW_INFO("sctx: %p\n", obj->sctx);
		RTW_INFO("list->next: %p\n", obj->list.next);
		RTW_INFO("list->prev: %p\n", obj->list.prev);
	}
#endif /* DBG_CMD_QUEUE */

	/* _rtw_spinunlock_bh(&queue->lock);	 */
	_rtw_spinunlock_irq(&queue->lock, &sp_flags);

exit:


	return _SUCCESS;
}
#else
static sint _rtw_enqueue_cmd(struct cmd_obj *obj, bool to_head)
{
	u32 res;

	res = rtw_enqueue_phl_cmd(obj);

#ifdef DBG_CMD_QUEUE
	if (dump_cmd_id) {
		RTW_INFO("%s===> cmdcode:0x%02x\n", __FUNCTION__, obj->cmdcode);
		if (obj->cmdcode == CMD_SET_MLME_EVT) {
			if (obj->parmbuf) {
				struct rtw_evt_header *evt_hdr = (struct rtw_evt_header *)(obj->parmbuf);
				RTW_INFO("evt_hdr->id:%d\n", evt_hdr->id);
			}
		}
		if (obj->cmdcode == CMD_SET_DRV_EXTRA) {
			if (obj->parmbuf) {
				struct drvextra_cmd_parm *pdrvextra_cmd_parm = (struct drvextra_cmd_parm *)(obj->parmbuf);
				RTW_INFO("pdrvextra_cmd_parm->ec_id:0x%02x\n", pdrvextra_cmd_parm->ec_id);
			}
		}
	}
#endif /* DBG_CMD_QUEUE */
	return res;
}
#endif

u32 rtw_enqueue_cmd(struct cmd_priv *pcmdpriv, struct cmd_obj *cmd_obj)
{
	int res = _FAIL;

	if (cmd_obj == NULL)
		goto exit;

	res = rtw_cmd_filter(pcmdpriv, cmd_obj);
	if ((_FAIL == res) || (cmd_obj->cmdsz > MAX_CMDSZ)) {
		if (cmd_obj->cmdsz > MAX_CMDSZ) {
			RTW_INFO("%s failed due to obj->cmdsz(%d) > MAX_CMDSZ(%d)\n", __func__, cmd_obj->cmdsz, MAX_CMDSZ);
			rtw_warn_on(1);
		}

		if (cmd_obj->cmdcode == CMD_SET_DRV_EXTRA) {
			struct drvextra_cmd_parm *extra_parm = (struct drvextra_cmd_parm *)cmd_obj->parmbuf;

			if (extra_parm->pbuf && extra_parm->size > 0)
				rtw_mfree(extra_parm->pbuf, extra_parm->size);
		}
#ifdef CONFIG_80211BE_EHT
		else if (cmd_obj->cmdcode == CMD_SET_MLME_EVT) {
			struct rtw_evt_header *hdr =
				(struct rtw_evt_header*)cmd_obj->parmbuf;

			hdr = (struct rtw_evt_header*)cmd_obj->parmbuf;
			if (hdr && hdr->id == EVT_JOINBSS) {
				struct wlan_network *network =
					(struct wlan_network *)(cmd_obj->parmbuf + sizeof(*hdr));
				if (network && network->network.is_mld) {
					rtw_free_cloned_mld_network(network->network.mld_network);
					cmd_obj->parmbuf = NULL;
				}
			}
		}
#endif
		rtw_free_cmd_obj(cmd_obj);
		goto exit;
	}


	res = _rtw_enqueue_cmd(cmd_obj, 0);
	#if 0 /*#ifdef CONFIG_CORE_CMD_THREAD*/
	if (res == _SUCCESS)
		_rtw_up_sema(&pcmdpriv->cmd_queue_sema);
	#endif

exit:
	return res;
}

#if 0 /*#ifdef CONFIG_CORE_CMD_THREAD*/
struct	cmd_obj	*_rtw_dequeue_cmd(_queue *queue)
{
	struct cmd_obj *obj;
	unsigned long sp_flags;

	/* _rtw_spinlock_bh(&(queue->lock)); */
	_rtw_spinlock_irq(&queue->lock, &sp_flags);

#ifdef DBG_CMD_QUEUE
	if (queue->queue.prev->next != &queue->queue) {
		RTW_INFO("[%d] head %p, tail %p, tail->prev->next %p[tail], tail->next %p[head]\n", __LINE__,
			&queue->queue, queue->queue.prev, queue->queue.prev->prev->next, queue->queue.prev->next);
	}
#endif /* DBG_CMD_QUEUE */


	if (rtw_is_list_empty(&(queue->queue)))
		obj = NULL;
	else {
		obj = LIST_CONTAINOR(get_next(&(queue->queue)), struct cmd_obj, list);

#ifdef DBG_CMD_QUEUE
		if (queue->queue.prev->next != &queue->queue) {
			RTW_INFO("==========%s============\n", __FUNCTION__);
			RTW_INFO("head:%p,obj_addr:%p\n", &queue->queue, obj);
			RTW_INFO("padapter: %p\n", obj->padapter);
			RTW_INFO("cmdcode: 0x%02x\n", obj->cmdcode);
			RTW_INFO("res: %d\n", obj->res);
			RTW_INFO("parmbuf: %p\n", obj->parmbuf);
			RTW_INFO("cmdsz: %d\n", obj->cmdsz);
			RTW_INFO("rsp: %p\n", obj->rsp);
			RTW_INFO("rspsz: %d\n", obj->rspsz);
			RTW_INFO("sctx: %p\n", obj->sctx);
			RTW_INFO("list->next: %p\n", obj->list.next);
			RTW_INFO("list->prev: %p\n", obj->list.prev);
		}

		if (dump_cmd_id) {
			RTW_INFO("%s===> cmdcode:0x%02x\n", __FUNCTION__, obj->cmdcode);
			if (obj->cmdcode == CMD_SET_DRV_EXTRA) {
				if (obj->parmbuf) {
					struct drvextra_cmd_parm *pdrvextra_cmd_parm = (struct drvextra_cmd_parm *)(obj->parmbuf);
					printk("pdrvextra_cmd_parm->ec_id:0x%02x\n", pdrvextra_cmd_parm->ec_id);
				}
			}

		}
#endif /* DBG_CMD_QUEUE */

		rtw_list_delete(&obj->list);
	}

	/* _rtw_spinunlock_bh(&(queue->lock)); */
	_rtw_spinunlock_irq(&queue->lock, &sp_flags);


	return obj;
}

struct	cmd_obj	*rtw_dequeue_cmd(struct cmd_priv *pcmdpriv)
{
	struct cmd_obj *cmd_obj;

	cmd_obj = _rtw_dequeue_cmd(&pcmdpriv->cmd_queue);

	return cmd_obj;
}
#endif
void rtw_free_cmd_obj(struct cmd_obj *pcmd)
{
	if (pcmd->parmbuf != NULL) {
		/* free parmbuf in cmd_obj */
		rtw_mfree((unsigned char *)pcmd->parmbuf, pcmd->cmdsz);
	}
	if (pcmd->rsp != NULL) {
		if (pcmd->rspsz != 0) {
			/* free rsp in cmd_obj */
			rtw_mfree((unsigned char *)pcmd->rsp, pcmd->rspsz);
		}
	}

	/* free cmd_obj */
	rtw_mfree((unsigned char *)pcmd, sizeof(struct cmd_obj));
}
void rtw_run_cmd(_adapter *padapter, struct cmd_obj *pcmd, bool discard)
{
	u8 ret;
	u8 *pcmdbuf;
	systime cmd_start_time;
	u32 cmd_process_time;
	u8(*cmd_hdl)(_adapter *padapter, u8 *pbuf);
	void (*pcmd_callback)(_adapter *dev, struct cmd_obj *pcmd);
	struct cmd_priv *pcmdpriv = &(adapter_to_dvobj(padapter)->cmdpriv);
	struct drvextra_cmd_parm *extra_parm = NULL;

	cmd_start_time = rtw_get_current_time();
	pcmdpriv->cmd_issued_cnt++;

	if (discard)
		goto post_process;

	if (pcmd->cmdsz > MAX_CMDSZ) {
		RTW_ERR("%s cmdsz:%d > MAX_CMDSZ:%d\n", __func__, pcmd->cmdsz, MAX_CMDSZ);
		pcmd->res = H2C_PARAMETERS_ERROR;
		goto post_process;
	}

	if (pcmd->cmdcode >= (sizeof(wlancmds) / sizeof(struct rtw_cmd))) {
		RTW_ERR("%s undefined cmdcode:%d\n", __func__, pcmd->cmdcode);
		pcmd->res = H2C_PARAMETERS_ERROR;
		goto post_process;
	}

	cmd_hdl = wlancmds[pcmd->cmdcode].cmd_hdl;
	if (!cmd_hdl) {
		RTW_ERR("%s no cmd_hdl for cmdcode:%d\n", __func__, pcmd->cmdcode);
		pcmd->res = H2C_PARAMETERS_ERROR;
		goto post_process;
	}

	if (DBG_CMD_EXECUTE)
		RTW_INFO(ADPT_FMT" "CMD_FMT" %sexecute\n", ADPT_ARG(pcmd->padapter), CMD_ARG(pcmd)
			, pcmd->res == H2C_ENQ_HEAD ? "ENQ_HEAD " : (pcmd->res == H2C_ENQ_HEAD_FAIL ? "ENQ_HEAD_FAIL " : ""));

	pcmdbuf = pcmdpriv->cmd_buf;
	_rtw_memcpy(pcmdbuf, pcmd->parmbuf, pcmd->cmdsz);
	ret = cmd_hdl(pcmd->padapter, pcmdbuf);
	pcmd->res = ret;

	pcmdpriv->cmd_seq++;

post_process:

	_rtw_mutex_lock_interruptible(&pcmdpriv->sctx_mutex);
	if (pcmd->sctx) {
		if (0)
			RTW_PRINT(FUNC_ADPT_FMT" pcmd->sctx\n", FUNC_ADPT_ARG(pcmd->padapter));
		if (pcmd->res == H2C_SUCCESS)
			rtw_sctx_done(&pcmd->sctx);
		else
			rtw_sctx_done_err(&pcmd->sctx, RTW_SCTX_DONE_CMD_ERROR);
	}
	_rtw_mutex_unlock(&pcmdpriv->sctx_mutex);

	cmd_process_time = rtw_get_passing_time_ms(cmd_start_time);
	if (cmd_process_time > 1000) {
		RTW_INFO(ADPT_FMT" "CMD_FMT" process_time=%d\n", ADPT_ARG(pcmd->padapter), CMD_ARG(pcmd), cmd_process_time);
		if (0)
			rtw_warn_on(1);
	}

	/* call callback function for post-processed */
	if (pcmd->cmdcode < (sizeof(wlancmds) / sizeof(struct rtw_cmd)))
		pcmd_callback = wlancmds[pcmd->cmdcode].callback;
	else
		pcmd_callback = NULL;

	if (pcmd_callback == NULL) {
		rtw_free_cmd_obj(pcmd);
	} else {
		/* todo: !!! fill rsp_buf to pcmd->rsp if (pcmd->rsp!=NULL) */
		pcmd_callback(pcmd->padapter, pcmd);/* need conider that free cmd_obj in rtw_cmd_callback */
	}
}
#if 0 /*#ifdef CONFIG_CORE_CMD_THREAD*/
void rtw_stop_cmd_thread(_adapter *adapter)
{
	if (adapter->cmdThread) {
		_rtw_up_sema(&adapter->cmdpriv.cmd_queue_sema);
		rtw_thread_stop(adapter->cmdThread);
		adapter->cmdThread = NULL;
	}
}

thread_return rtw_cmd_thread(thread_context context)
{
	u8 ret;
	struct cmd_obj *pcmd;
	u8 *pcmdbuf, *prspbuf;
	systime cmd_start_time;
	u32 cmd_process_time;
	u8(*cmd_hdl)(_adapter *padapter, u8 *pbuf);
	void (*pcmd_callback)(_adapter *dev, struct cmd_obj *pcmd);
	_adapter *padapter = (_adapter *)context;
	struct cmd_priv *pcmdpriv = &(padapter->cmdpriv);
	struct drvextra_cmd_parm *extra_parm = NULL;
	unsigned long sp_flags;

	rtw_thread_enter("RTW_CMD_THREAD");

	pcmdbuf = pcmdpriv->cmd_buf;
	prspbuf = pcmdpriv->rsp_buf;
	ATOMIC_SET(&(pcmdpriv->cmdthd_running), _TRUE);
	_rtw_up_sema(&pcmdpriv->start_cmdthread_sema);


	while (1) {
		if (_rtw_down_sema(&pcmdpriv->cmd_queue_sema) == _FAIL) {
			RTW_PRINT(FUNC_ADPT_FMT" _rtw_down_sema(&pcmdpriv->cmd_queue_sema) return _FAIL, break\n", FUNC_ADPT_ARG(padapter));
			break;
		}

		if (RTW_CANNOT_RUN(adapter_to_dvobj(padapter))) {
			RTW_DBG(FUNC_ADPT_FMT "- bDriverStopped(%s) bSurpriseRemoved(%s)\n",
				FUNC_ADPT_ARG(padapter),
				dev_is_drv_stopped(adapter_to_dvobj(padapter)) ? "True" : "False",
				dev_is_surprise_removed(adapter_to_dvobj(padapter)) ? "True" : "False");
			break;
		}

		_rtw_spinlock_irq(&pcmdpriv->cmd_queue.lock, &sp_flags);
		if (rtw_is_list_empty(&(pcmdpriv->cmd_queue.queue))) {
			/* RTW_INFO("%s: cmd queue is empty!\n", __func__); */
			_rtw_spinunlock_irq(&pcmdpriv->cmd_queue.lock, &sp_flags);
			continue;
		}
		_rtw_spinunlock_irq(&pcmdpriv->cmd_queue.lock, &sp_flags);

_next:
		if (RTW_CANNOT_RUN(adapter_to_dvobj(padapter))) {
			RTW_PRINT("%s: DriverStopped(%s) SurpriseRemoved(%s) break at line %d\n",
				  __func__
				, dev_is_drv_stopped(adapter_to_dvobj(padapter)) ? "True" : "False"
				, dev_is_surprise_removed(adapter_to_dvobj(padapter)) ? "True" : "False"
				  , __LINE__);
			break;
		}

		pcmd = rtw_dequeue_cmd(pcmdpriv);
		if (!pcmd) {
			continue;
		}

		cmd_start_time = rtw_get_current_time();
		pcmdpriv->cmd_issued_cnt++;

		if (pcmd->cmdsz > MAX_CMDSZ) {
			RTW_ERR("%s cmdsz:%d > MAX_CMDSZ:%d\n", __func__, pcmd->cmdsz, MAX_CMDSZ);
			pcmd->res = H2C_PARAMETERS_ERROR;
			goto post_process;
		}

		if (pcmd->cmdcode >= (sizeof(wlancmds) / sizeof(struct rtw_cmd))) {
			RTW_ERR("%s undefined cmdcode:%d\n", __func__, pcmd->cmdcode);
			pcmd->res = H2C_PARAMETERS_ERROR;
			goto post_process;
		}

		cmd_hdl = wlancmds[pcmd->cmdcode].cmd_hdl;
		if (!cmd_hdl) {
			RTW_ERR("%s no cmd_hdl for cmdcode:%d\n", __func__, pcmd->cmdcode);
			pcmd->res = H2C_PARAMETERS_ERROR;
			goto post_process;
		}

		if (_FAIL == rtw_cmd_filter(pcmdpriv, pcmd)) {
			pcmd->res = H2C_DROPPED;
			if (pcmd->cmdcode == CMD_SET_DRV_EXTRA) {
				extra_parm = (struct drvextra_cmd_parm *)pcmd->parmbuf;
				if (extra_parm && extra_parm->pbuf && extra_parm->size > 0)
					rtw_mfree(extra_parm->pbuf, extra_parm->size);
			}
			goto post_process;
		}

		if (DBG_CMD_EXECUTE)
			RTW_INFO(ADPT_FMT" "CMD_FMT" %sexecute\n", ADPT_ARG(pcmd->padapter), CMD_ARG(pcmd)
				, pcmd->res == H2C_ENQ_HEAD ? "ENQ_HEAD " : (pcmd->res == H2C_ENQ_HEAD_FAIL ? "ENQ_HEAD_FAIL " : ""));

		_rtw_memcpy(pcmdbuf, pcmd->parmbuf, pcmd->cmdsz);
		ret = cmd_hdl(pcmd->padapter, pcmdbuf);
		pcmd->res = ret;

		pcmdpriv->cmd_seq++;

post_process:

		_rtw_mutex_lock_interruptible(&(pcmd->padapter->cmdpriv.sctx_mutex));
		if (pcmd->sctx) {
			if (0)
				RTW_PRINT(FUNC_ADPT_FMT" pcmd->sctx\n", FUNC_ADPT_ARG(pcmd->padapter));
			if (pcmd->res == H2C_SUCCESS)
				rtw_sctx_done(&pcmd->sctx);
			else
				rtw_sctx_done_err(&pcmd->sctx, RTW_SCTX_DONE_CMD_ERROR);
		}
		_rtw_mutex_unlock(&(pcmd->padapter->cmdpriv.sctx_mutex));

		cmd_process_time = rtw_get_passing_time_ms(cmd_start_time);
		if (cmd_process_time > 1000) {
			RTW_INFO(ADPT_FMT" "CMD_FMT" process_time=%d\n", ADPT_ARG(pcmd->padapter), CMD_ARG(pcmd), cmd_process_time);
			if (0)
				rtw_warn_on(1);
		}

		/* call callback function for post-processed */
		if (pcmd->cmdcode >= (sizeof(wlancmds) / sizeof(struct rtw_cmd)))
			pcmd_callback = wlancmds[pcmd->cmdcode].callback;
		else
			pcmd_callback = NULL;

		if (pcmd_callback == NULL) {
			rtw_free_cmd_obj(pcmd);
		} else {
			/* todo: !!! fill rsp_buf to pcmd->rsp if (pcmd->rsp!=NULL) */
			pcmd_callback(pcmd->padapter, pcmd);/* need conider that free cmd_obj in rtw_cmd_callback */
		}

		flush_signals_thread();

		goto _next;

	}

	/* to avoid enqueue cmd after free all cmd_obj */
	ATOMIC_SET(&(pcmdpriv->cmdthd_running), _FALSE);

	/* free all cmd_obj resources */
	do {
		pcmd = rtw_dequeue_cmd(pcmdpriv);
		if (pcmd == NULL)
			break;

		if (0)
			RTW_INFO("%s: leaving... drop "CMD_FMT"\n", __func__, CMD_ARG(pcmd));

		if (pcmd->cmdcode == CMD_SET_DRV_EXTRA) {
			extra_parm = (struct drvextra_cmd_parm *)pcmd->parmbuf;
			if (extra_parm->pbuf && extra_parm->size > 0)
				rtw_mfree(extra_parm->pbuf, extra_parm->size);
		}

		_rtw_mutex_lock_interruptible(&(pcmd->padapter->cmdpriv.sctx_mutex));
		if (pcmd->sctx) {
			if (0)
				RTW_PRINT(FUNC_ADPT_FMT" pcmd->sctx\n", FUNC_ADPT_ARG(pcmd->padapter));
			rtw_sctx_done_err(&pcmd->sctx, RTW_SCTX_DONE_CMD_DROP);
		}
		_rtw_mutex_unlock(&(pcmd->padapter->cmdpriv.sctx_mutex));

		rtw_free_cmd_obj(pcmd);
	} while (1);

	RTW_INFO(FUNC_ADPT_FMT " Exit\n", FUNC_ADPT_ARG(padapter));

	rtw_thread_wait_stop();

	return 0;
}
#endif

void rtw_readtssi_cmdrsp_callback(_adapter	*padapter,  struct cmd_obj *pcmd)
{

	rtw_mfree((unsigned char *) pcmd->parmbuf, pcmd->cmdsz);
	rtw_mfree((unsigned char *) pcmd, sizeof(struct cmd_obj));

#ifdef CONFIG_MP_INCLUDED
	if (padapter->registrypriv.mp_mode == 1)
		padapter->mppriv.workparam.bcompleted = _TRUE;
#endif

}

static u8 rtw_createbss_cmd(_adapter  *adapter, int flags, bool adhoc
	, u8 ifbmp, u8 excl_ifbmp, s8 req_band, s16 req_ch, s8 req_bw, s8 req_offset, u8 is_change_chbw)
{
	struct cmd_obj *cmdobj;
	struct createbss_parm *parm;
	struct cmd_priv *pcmdpriv = &adapter_to_dvobj(adapter)->cmdpriv;
	struct submit_ctx sctx;
	u8 res = _SUCCESS;

	if (req_ch > 0 && req_bw >= 0 && req_offset >= 0) {
		if (!rtw_chset_is_bchbw_valid(adapter_to_chset(adapter), req_band, req_ch, req_bw, req_offset, 0, 0)) {
			res = _FAIL;
			goto exit;
		}
	}

	/* prepare cmd parameter */
	parm = (struct createbss_parm *)rtw_zmalloc(sizeof(*parm));
	if (parm == NULL) {
		res = _FAIL;
		goto exit;
	}

	if (adhoc) {
		/* for now, adhoc doesn't support ch,bw,offset request */
		parm->adhoc = 1;
	} else {
		parm->adhoc = 0;
		parm->ifbmp = ifbmp;
		parm->excl_ifbmp = excl_ifbmp;
		parm->req_band = req_band;
		parm->req_ch = req_ch;
		parm->req_bw = req_bw;
		parm->req_offset = req_offset;
		parm->ifbmp_ch_changed = 0;
		parm->ch_to_set = 0;
		parm->bw_to_set = 0;
		parm->offset_to_set = 0;
		parm->do_rfk = _FALSE;
		parm->is_change_chbw = is_change_chbw;
	}

	if (flags & RTW_CMDF_DIRECTLY) {
		/* no need to enqueue, do the cmd hdl directly and free cmd parameter */
		if (H2C_SUCCESS != createbss_hdl(adapter, (u8 *)parm))
			res = _FAIL;
		rtw_mfree((u8 *)parm, sizeof(*parm));
	} else {
		/* need enqueue, prepare cmd_obj and enqueue */
		cmdobj = (struct cmd_obj *)rtw_zmalloc(sizeof(*cmdobj));
		if (cmdobj == NULL) {
			res = _FAIL;
			rtw_mfree((u8 *)parm, sizeof(*parm));
			goto exit;
		}
		cmdobj->padapter = adapter;

		init_h2fwcmd_w_parm_no_rsp(cmdobj, parm, CMD_CREATE_BSS);

		if (flags & RTW_CMDF_WAIT_ACK) {
			cmdobj->sctx = &sctx;
			rtw_sctx_init(&sctx, 5000);
		}

		res = rtw_enqueue_cmd(pcmdpriv, cmdobj);

		if (res == _SUCCESS && (flags & RTW_CMDF_WAIT_ACK)) {
			res = rtw_sctx_wait(&sctx, __func__);
			_rtw_mutex_lock_interruptible(&pcmdpriv->sctx_mutex);
			if (sctx.status == RTW_SCTX_SUBMITTED)
				cmdobj->sctx = NULL;
			_rtw_mutex_unlock(&pcmdpriv->sctx_mutex);
		}
	}

exit:
	return res;
}

inline u8 rtw_create_ibss_cmd(_adapter *adapter, int flags)
{
	return rtw_createbss_cmd(adapter, flags
		, 1
		, 0, 0, REQ_BAND_NONE
		, 0, REQ_BW_NONE, REQ_OFFSET_NONE /* for now, adhoc doesn't support ch,bw,offset request */
		, _FALSE
	);
}

inline u8 rtw_startbss_cmd(_adapter *adapter, int flags)
{
	return rtw_createbss_cmd(adapter, flags
		, 0
		, BIT(adapter->iface_id), 0, REQ_BAND_NONE
		, 0, REQ_BW_NONE, REQ_OFFSET_NONE /* excute entire AP setup cmd */
		, _FALSE
	);
}

inline u8 rtw_change_bss_bchbw_cmd(_adapter *adapter, int flags
	, u8 ifbmp, u8 excl_ifbmp, s8 req_band, s16 req_ch, s8 req_bw, s8 req_offset)
{
	return rtw_createbss_cmd(adapter, flags
		, 0
		, ifbmp, excl_ifbmp, req_band
		, req_ch, req_bw, req_offset
		, _TRUE
	);
}

u8 rtw_joinbss_cmd(_adapter *padapter, struct wlan_network *pnetwork)
{
	u8 res = _SUCCESS;
	WLAN_BSSID_EX		*psecnetwork;
	struct cmd_obj		*pcmd;
	struct cmd_priv		*pcmdpriv = &adapter_to_dvobj(padapter)->cmdpriv;
	struct joinbss_parm *param;
	NDIS_802_11_NETWORK_INFRASTRUCTURE ndis_network_mode = pnetwork->network.InfrastructureMode;

	rtw_led_control(padapter, LED_CTL_START_TO_LINK);

	pcmd = (struct cmd_obj *)rtw_zmalloc(sizeof(struct cmd_obj));
	if (pcmd == NULL) {
		res = _FAIL;
		goto exit;
	}
	pcmd->padapter = padapter;

	/* for hidden ap to set fw_state here */
	if (!MLME_IS_STA(padapter) || !MLME_IS_ADHOC(padapter)) {
		switch (ndis_network_mode) {
		case Ndis802_11IBSS:
			set_fwstate(&padapter->mlmepriv, WIFI_ADHOC_STATE);
			break;

		case Ndis802_11Infrastructure:
			set_fwstate(&padapter->mlmepriv, WIFI_STATION_STATE);
			break;

		default:
			rtw_warn_on(1);
			break;
		}
	}

	param = rtw_malloc(sizeof(struct joinbss_parm));
	if (param == NULL) {
		if (pcmd != NULL)
			rtw_mfree(pcmd, sizeof(struct cmd_obj));
		res = _FAIL;
		goto exit;
	}

	_rtw_memset(&param->network, 0, sizeof(WLAN_BSSID_EX));
	_rtw_memcpy(&param->network, &pnetwork->network, get_WLAN_BSSID_EX_sz(&pnetwork->network));

	init_h2fwcmd_w_parm_no_rsp(pcmd, param, CMD_JOINBSS);

	res = rtw_enqueue_cmd(pcmdpriv, pcmd);

exit:
	return res;
}

#ifdef CONFIG_STA_CMD_DISPR
/* for sta_mode only */
static u8 sta_disassoc_cmd(struct _ADAPTER *a, u32 deauth_timeout_ms, int flags)
{
	struct cmd_priv *cmdpriv = &adapter_to_dvobj(a)->cmdpriv;
	struct cmd_obj *cmd = NULL;
	struct disconnect_parm *param = NULL;
	struct submit_ctx sctx;
	enum rtw_phl_status status;
	int ret;
	u8 res = _FAIL;


	if (!MLME_IS_ASOC(a))
		return _SUCCESS;

	param = (struct disconnect_parm *)rtw_zmalloc(sizeof(*param));
	if (!param) {
		RTW_ERR(FUNC_ADPT_FMT ": alloc param FAIL!", FUNC_ADPT_ARG(a));
		goto exit;
	}
	cmd = (struct cmd_obj *)rtw_zmalloc(sizeof(*cmd));
	if (!cmd) {
		RTW_ERR(FUNC_ADPT_FMT ": alloc cmd FAIL!", FUNC_ADPT_ARG(a));
		rtw_mfree((u8 *)param, sizeof(*param));
		goto exit;
	}

	param->deauth_timeout_ms = deauth_timeout_ms;
	init_h2fwcmd_w_parm_no_rsp(cmd, param, CMD_DISCONNECT);
	cmd->padapter = a;
	if (flags & RTW_CMDF_WAIT_ACK) {
		cmd->sctx = &sctx;
		rtw_sctx_init(&sctx, 2000);
	}

	status = rtw_disconnect_cmd(a, cmd);
	if (status != RTW_PHL_STATUS_SUCCESS) {
		RTW_ERR(FUNC_ADPT_FMT ": send disconnect cmd FAIL!(0x%x)\n",
			FUNC_ADPT_ARG(a), status);
		rtw_mfree((u8 *)param, sizeof(*param));
		rtw_mfree((u8 *)cmd, sizeof(*cmd));
		goto exit;
	}
	res = _SUCCESS;

	if (flags & RTW_CMDF_WAIT_ACK) {
		ret = rtw_sctx_wait(&sctx, __func__);
		if (ret == _FAIL)
			res = _FAIL;
		_rtw_spinlock_bh(&a->disconnect_lock);
		if (a->discon_cmd) {
			a->discon_cmd->sctx = NULL;
			/*
			 * a->discon_param would be
			 * freed by disconnect cmd dispatcher.
			 */
		}
		_rtw_spinunlock_bh(&a->disconnect_lock);
	}

exit:
	return res;
}
#endif /* CONFIG_STA_CMD_DISPR */

u8 rtw_disassoc_cmd(_adapter *padapter, u32 deauth_timeout_ms, int flags) /* for sta_mode */
{
	struct cmd_obj *cmdobj = NULL;
	struct disconnect_parm *param = NULL;
	struct cmd_priv *cmdpriv = &adapter_to_dvobj(padapter)->cmdpriv;

	struct submit_ctx sctx;
	u8 res = _SUCCESS;


#ifdef CONFIG_STA_CMD_DISPR
	if (MLME_IS_STA(padapter))
		return sta_disassoc_cmd(padapter, deauth_timeout_ms, flags);
#endif /* CONFIG_STA_CMD_DISPR */

	/* prepare cmd parameter */
	param = (struct disconnect_parm *)rtw_zmalloc(sizeof(*param));
	if (param == NULL) {
		res = _FAIL;
		goto exit;
	}
	param->deauth_timeout_ms = deauth_timeout_ms;

	if (flags & RTW_CMDF_DIRECTLY) {
		/* no need to enqueue, do the cmd hdl directly and free cmd parameter */
		if (disconnect_hdl(padapter, (u8 *)param) != H2C_SUCCESS)
			res = _FAIL;
		rtw_mfree((u8 *)param, sizeof(*param));

	} else {
		cmdobj = (struct cmd_obj *)rtw_zmalloc(sizeof(*cmdobj));
		if (cmdobj == NULL) {
			res = _FAIL;
			rtw_mfree((u8 *)param, sizeof(*param));
			goto exit;
		}
		cmdobj->padapter = padapter;
		init_h2fwcmd_w_parm_no_rsp(cmdobj, param, CMD_DISCONNECT);
		if (flags & RTW_CMDF_WAIT_ACK) {
			cmdobj->sctx = &sctx;
			rtw_sctx_init(&sctx, 2000);
		}
		res = rtw_enqueue_cmd(cmdpriv, cmdobj);
		if (res == _FAIL) {
			RTW_ERR(FUNC_ADPT_FMT ": enqueue disconnect cmd FAIL!\n",
				FUNC_ADPT_ARG(padapter));
			goto exit;
		}
		if (flags & RTW_CMDF_WAIT_ACK) {
			rtw_sctx_wait(&sctx, __func__);
			_rtw_mutex_lock_interruptible(&cmdpriv->sctx_mutex);
			if (sctx.status == RTW_SCTX_SUBMITTED)
				cmdobj->sctx = NULL;
			_rtw_mutex_unlock(&cmdpriv->sctx_mutex);
		}
	}

exit:
	return res;
}


u8 rtw_stop_ap_cmd(_adapter *adapter, u8 flags)
{
#ifdef CONFIG_AP_MODE
	struct cmd_obj *cmdobj;
	struct drvextra_cmd_parm *parm;
	struct cmd_priv *pcmdpriv = &adapter_to_dvobj(adapter)->cmdpriv;
	struct submit_ctx sctx;
	u8 res = _SUCCESS;

	if (flags & RTW_CMDF_DIRECTLY) {
		/* no need to enqueue, do the cmd hdl directly and free cmd parameter */
		if (H2C_SUCCESS != stop_ap_hdl(adapter))
			res = _FAIL;
	} else {
		parm = (struct drvextra_cmd_parm *)rtw_zmalloc(sizeof(struct drvextra_cmd_parm));
		if (parm == NULL) {
			res = _FAIL;
			goto exit;
		}

		parm->ec_id = STOP_AP_WK_CID;
		parm->type = 0;
		parm->size = 0;
		parm->pbuf = NULL;

		/* need enqueue, prepare cmd_obj and enqueue */
		cmdobj = (struct cmd_obj *)rtw_zmalloc(sizeof(*cmdobj));
		if (cmdobj == NULL) {
			res = _FAIL;
			rtw_mfree((u8 *)parm, sizeof(*parm));
			goto exit;
		}
		cmdobj->padapter = adapter;

		init_h2fwcmd_w_parm_no_rsp(cmdobj, parm, CMD_SET_DRV_EXTRA);

		if (flags & RTW_CMDF_WAIT_ACK) {
			cmdobj->sctx = &sctx;
			rtw_sctx_init(&sctx, 2000);
		}

		res = rtw_enqueue_cmd(pcmdpriv, cmdobj);

		if (res == _SUCCESS && (flags & RTW_CMDF_WAIT_ACK)) {
			rtw_sctx_wait(&sctx, __func__);
			_rtw_mutex_lock_interruptible(&pcmdpriv->sctx_mutex);
			if (sctx.status == RTW_SCTX_SUBMITTED)
				cmdobj->sctx = NULL;
			_rtw_mutex_unlock(&pcmdpriv->sctx_mutex);
		}
	}
exit:
	return res;
#endif
}

#ifdef CONFIG_RTW_TOKEN_BASED_XMIT
u8 rtw_tx_control_cmd(_adapter *adapter)
{
	struct cmd_obj *cmd;
	struct drvextra_cmd_parm *pdrvextra_cmd_parm;
	struct cmd_priv *pcmdpriv = &adapter_to_dvobj(adapter)->cmdpriv;

	u8 res = _SUCCESS;

	cmd = (struct cmd_obj *)rtw_zmalloc(sizeof(struct cmd_obj));
	if (cmd == NULL){
		res = _FAIL;
		goto exit;
	}
	cmd->padapter = adapter;

	pdrvextra_cmd_parm = (struct drvextra_cmd_parm *)rtw_zmalloc(sizeof(struct drvextra_cmd_parm));
	if (pdrvextra_cmd_parm == NULL) {
		rtw_mfree((unsigned char *)cmd, sizeof(struct cmd_obj));
		res = _FAIL;
		goto exit;
	}

	pdrvextra_cmd_parm->ec_id = TBTX_CONTROL_TX_WK_CID;
	pdrvextra_cmd_parm->type = 0;
	pdrvextra_cmd_parm->size = 0;
	pdrvextra_cmd_parm->pbuf = NULL;
	init_h2fwcmd_w_parm_no_rsp(cmd, pdrvextra_cmd_parm, CMD_SET_DRV_EXTRA);

	res = rtw_enqueue_cmd(pcmdpriv, cmd);

exit:
	return res;	
}
#endif

u8 rtw_setopmode_cmd(_adapter  *adapter, NDIS_802_11_NETWORK_INFRASTRUCTURE networktype, u8 flags)
{
	struct cmd_obj *cmdobj;
	struct setopmode_parm *parm;
	struct cmd_priv *pcmdpriv = &adapter_to_dvobj(adapter)->cmdpriv;
	struct submit_ctx sctx;
	u8 res = _SUCCESS;

	/* prepare cmd parameter */
	parm = (struct setopmode_parm *)rtw_zmalloc(sizeof(*parm));
	if (parm == NULL) {
		res = _FAIL;
		goto exit;
	}
	parm->mode = (u8)networktype;

	if (flags & RTW_CMDF_DIRECTLY) {
		/* no need to enqueue, do the cmd hdl directly and free cmd parameter */
		if (H2C_SUCCESS != setopmode_hdl(adapter, (u8 *)parm))
			res = _FAIL;
		rtw_mfree((u8 *)parm, sizeof(*parm));
	} else {
		/* need enqueue, prepare cmd_obj and enqueue */
		cmdobj = (struct cmd_obj *)rtw_zmalloc(sizeof(*cmdobj));
		if (cmdobj == NULL) {
			res = _FAIL;
			rtw_mfree((u8 *)parm, sizeof(*parm));
			goto exit;
		}
		cmdobj->padapter = adapter;

		init_h2fwcmd_w_parm_no_rsp(cmdobj, parm, CMD_SET_OPMODE);

		if (flags & RTW_CMDF_WAIT_ACK) {
			cmdobj->sctx = &sctx;
			rtw_sctx_init(&sctx, 2000);
		}

		res = rtw_enqueue_cmd(pcmdpriv, cmdobj);

		if (res == _SUCCESS && (flags & RTW_CMDF_WAIT_ACK)) {
			rtw_sctx_wait(&sctx, __func__);
			_rtw_mutex_lock_interruptible(&pcmdpriv->sctx_mutex);
			if (sctx.status == RTW_SCTX_SUBMITTED)
				cmdobj->sctx = NULL;
			_rtw_mutex_unlock(&pcmdpriv->sctx_mutex);
		}
	}

exit:
	return res;
}

#ifdef CONFIG_CMD_DISP
u8 rtw_setstakey_cmd(_adapter *padapter, struct sta_info *sta, u8 key_type, bool enqueue)
{
	struct set_stakey_parm	setstakey_para;
	struct mlme_priv *pmlmepriv = &padapter->mlmepriv;
	struct security_priv *psecuritypriv = &padapter->securitypriv;
	struct _ADAPTER_LINK *padapter_link = sta->padapter_link;
	struct link_security_priv *lsecuritypriv = &padapter_link->securitypriv;
	u8 key_len =16;
	u8 res = _SUCCESS;

	_rtw_memset(&setstakey_para, 0, sizeof(struct set_stakey_parm));
	_rtw_memcpy(setstakey_para.addr, sta->phl_sta->mac_addr, ETH_ALEN);

	if (MLME_IS_STA(padapter))
		setstakey_para.algorithm = (unsigned char) psecuritypriv->dot11PrivacyAlgrthm;
	else
		GET_ENCRY_ALGO(psecuritypriv, sta, setstakey_para.algorithm, _FALSE);

	if ((setstakey_para.algorithm == _GCMP_256_) || (setstakey_para.algorithm == _CCMP_256_))
		key_len = 32;

	if (key_type == GROUP_KEY) {
		_rtw_memcpy(&setstakey_para.key, &lsecuritypriv->dot118021XGrpKey[lsecuritypriv->dot118021XGrpKeyid].skey, key_len);
		setstakey_para.gk = 1;
		setstakey_para.keyid = (u8)lsecuritypriv->dot118021XGrpKeyid;
	} else if (key_type == UNICAST_KEY)
		_rtw_memcpy(&setstakey_para.key, &sta->dot118021x_UncstKey, key_len);
#ifdef CONFIG_TDLS
	else if (key_type == TDLS_KEY) {
		_rtw_memcpy(&setstakey_para.key, sta->tpk.tk, key_len);
		setstakey_para.algorithm = (u8)sta->dot118021XPrivacy;
	}
#endif /* CONFIG_TDLS */

	/* jeff: set this becasue at least sw key is ready */
	padapter->securitypriv.busetkipkey = _TRUE;

	if (enqueue) {
		set_stakey_hdl(padapter, &setstakey_para, PHL_CMD_NO_WAIT, 0);
	} else {
		set_stakey_hdl(padapter, &setstakey_para, PHL_CMD_DIRECTLY, 0);
	}
exit:
	return res;
}

u8 rtw_clearstakey_cmd(_adapter *padapter, struct sta_info *sta, u8 enqueue)
{
	struct cmd_obj *cmd;
	struct set_stakey_parm	*psetstakey_para;
	struct cmd_priv *pcmdpriv = &adapter_to_dvobj(padapter)->cmdpriv;
	struct set_stakey_rsp *psetstakey_rsp = NULL;
	s16 cam_id = 0;
	u8 res = _SUCCESS;

	if (!sta) {
		RTW_ERR("%s sta == NULL\n", __func__);
		goto exit;
	}

	if (!enqueue)
		rtw_hw_del_all_key(padapter, sta, PHL_CMD_DIRECTLY, 0);
	else
		rtw_hw_del_all_key(padapter, sta, PHL_CMD_NO_WAIT, 0);

exit:


	return res;
}
#else /* CONFIG_FSM */
u8 rtw_setstakey_cmd(_adapter *padapter, struct sta_info *sta, u8 key_type, bool enqueue)
{
	struct cmd_obj *pcmd;
	struct set_stakey_parm	*psetstakey_para;
	struct cmd_priv *pcmdpriv = &adapter_to_dvobj(padapter)->cmdpriv;
	struct set_stakey_rsp *psetstakey_rsp = NULL;

	struct mlme_priv *pmlmepriv = &padapter->mlmepriv;
	struct security_priv *psecuritypriv = &padapter->securitypriv;
	u8 key_len =16;
	u8 res = _SUCCESS;


	psetstakey_para = (struct set_stakey_parm *)rtw_zmalloc(sizeof(struct set_stakey_parm));
	if (psetstakey_para == NULL) {
		res = _FAIL;
		goto exit;
	}

	_rtw_memcpy(psetstakey_para->addr, sta->phl_sta->mac_addr, ETH_ALEN);

	if (MLME_IS_STA(padapter))
		psetstakey_para->algorithm = (unsigned char) psecuritypriv->dot11PrivacyAlgrthm;
	else
		GET_ENCRY_ALGO(psecuritypriv, sta, psetstakey_para->algorithm, _FALSE);

	if ((psetstakey_para->algorithm == _GCMP_256_) || (psetstakey_para->algorithm == _CCMP_256_)) 
		key_len = 32;

	if (key_type == GROUP_KEY) {
		_rtw_memcpy(&psetstakey_para->key, &psecuritypriv->dot118021XGrpKey[psecuritypriv->dot118021XGrpKeyid].skey, key_len);
		psetstakey_para->gk = 1;
	} else if (key_type == UNICAST_KEY)
		_rtw_memcpy(&psetstakey_para->key, &sta->dot118021x_UncstKey, key_len);
#ifdef CONFIG_TDLS
	else if (key_type == TDLS_KEY) {
		_rtw_memcpy(&psetstakey_para->key, sta->tpk.tk, key_len);
		psetstakey_para->algorithm = (u8)sta->dot118021XPrivacy;
	}
#endif /* CONFIG_TDLS */

	/* jeff: set this becasue at least sw key is ready */
	padapter->securitypriv.busetkipkey = _TRUE;

	if (enqueue) {
		pcmd = (struct cmd_obj *)rtw_zmalloc(sizeof(struct cmd_obj));
		if (pcmd == NULL) {
			rtw_mfree((u8 *) psetstakey_para, sizeof(struct set_stakey_parm));
			res = _FAIL;
			goto exit;
		}
		pcmd->padapter = padapter;

		psetstakey_rsp = (struct set_stakey_rsp *)rtw_zmalloc(sizeof(struct set_stakey_rsp));
		if (psetstakey_rsp == NULL) {
			rtw_mfree((u8 *) pcmd, sizeof(struct cmd_obj));
			rtw_mfree((u8 *) psetstakey_para, sizeof(struct set_stakey_parm));
			res = _FAIL;
			goto exit;
		}

		init_h2fwcmd_w_parm_no_rsp(pcmd, psetstakey_para, CMD_SET_STAKEY);
		pcmd->rsp = (u8 *) psetstakey_rsp;
		pcmd->rspsz = sizeof(struct set_stakey_rsp);
		res = rtw_enqueue_cmd(pcmdpriv, pcmd);
	} else {
		set_stakey_hdl(padapter, (u8 *)psetstakey_para);
		rtw_mfree((u8 *) psetstakey_para, sizeof(struct set_stakey_parm));
	}
exit:
	return res;
}

u8 rtw_clearstakey_cmd(_adapter *padapter, struct sta_info *sta, u8 enqueue)
{
	struct cmd_obj *cmd;
	struct set_stakey_parm	*psetstakey_para;
	struct cmd_priv *pcmdpriv = &adapter_to_dvobj(padapter)->cmdpriv;
	struct set_stakey_rsp *psetstakey_rsp = NULL;
	s16 cam_id = 0;
	u8 res = _SUCCESS;

	if (!sta) {
		RTW_ERR("%s sta == NULL\n", __func__);
		goto exit;
	}

	if (!enqueue) {
		rtw_hw_del_all_key(padapter, sta, PHL_CMD_DIRECTLY, 0);
	} else {
		cmd = (struct cmd_obj *)rtw_zmalloc(sizeof(struct cmd_obj));
		if (cmd == NULL) {
			res = _FAIL;
			goto exit;
		}

		psetstakey_para = (struct set_stakey_parm *)rtw_zmalloc(sizeof(struct set_stakey_parm));
		if (psetstakey_para == NULL) {
			rtw_mfree((u8 *)cmd, sizeof(struct cmd_obj));
			res = _FAIL;
			goto exit;
		}
		cmd->padapter = padapter;

		psetstakey_rsp = (struct set_stakey_rsp *)rtw_zmalloc(sizeof(struct set_stakey_rsp));
		if (psetstakey_rsp == NULL) {
			rtw_mfree((u8 *)cmd, sizeof(struct cmd_obj));
			rtw_mfree((u8 *)psetstakey_para, sizeof(struct set_stakey_parm));
			res = _FAIL;
			goto exit;
		}

		init_h2fwcmd_w_parm_no_rsp(cmd, psetstakey_para, CMD_SET_STAKEY);
		cmd->rsp = (u8 *) psetstakey_rsp;
		cmd->rspsz = sizeof(struct set_stakey_rsp);

		_rtw_memcpy(psetstakey_para->addr, sta->phl_sta->mac_addr, ETH_ALEN);

		psetstakey_para->algorithm = _NO_PRIVACY_;

		res = rtw_enqueue_cmd(pcmdpriv, cmd);

	}

exit:


	return res;
}
#endif

u8 rtw_addbareq_cmd(_adapter *padapter, u8 tid, u8 *addr)
{
	struct cmd_priv *pcmdpriv = &adapter_to_dvobj(padapter)->cmdpriv;
	struct cmd_obj *cmd;
	struct addBaReq_parm *paddbareq_parm;

	u8	res = _SUCCESS;


	cmd = (struct cmd_obj *)rtw_zmalloc(sizeof(struct cmd_obj));
	if (cmd == NULL) {
		res = _FAIL;
		goto exit;
	}
	cmd->padapter = padapter;

	paddbareq_parm = (struct addBaReq_parm *)rtw_zmalloc(sizeof(struct addBaReq_parm));
	if (paddbareq_parm == NULL) {
		rtw_mfree((unsigned char *)cmd, sizeof(struct cmd_obj));
		res = _FAIL;
		goto exit;
	}

	paddbareq_parm->tid = tid;
	_rtw_memcpy(paddbareq_parm->addr, addr, ETH_ALEN);

	init_h2fwcmd_w_parm_no_rsp(cmd, paddbareq_parm, CMD_ADD_BAREQ);

	/* RTW_INFO("rtw_addbareq_cmd, tid=%d\n", tid); */

	/* rtw_enqueue_cmd(pcmdpriv, ph2c);	 */
	res = rtw_enqueue_cmd(pcmdpriv, cmd);

exit:
	return res;
}

u8 rtw_addbarsp_cmd(_adapter *padapter, u8 *addr, u16 tid,
		    struct ADDBA_request *paddba_req, u8 status,
		    u8 size, u16 start_seq)
{
	struct cmd_priv *pcmdpriv = &adapter_to_dvobj(padapter)->cmdpriv;
	struct cmd_obj *cmd;
	struct addBaRsp_parm *paddBaRsp_parm;
	u8 res = _SUCCESS;


	cmd = (struct cmd_obj *)rtw_zmalloc(sizeof(struct cmd_obj));
	if (cmd == NULL) {
		res = _FAIL;
		goto exit;
	}
	cmd->padapter = padapter;

	paddBaRsp_parm = (struct addBaRsp_parm *)rtw_zmalloc(sizeof(struct addBaRsp_parm));

	if (paddBaRsp_parm == NULL) {
		rtw_mfree((unsigned char *)cmd, sizeof(struct cmd_obj));
		res = _FAIL;
		goto exit;
	}

	_rtw_memcpy(paddBaRsp_parm->addr, addr, ETH_ALEN);
	_rtw_memcpy(&(paddBaRsp_parm->preq), paddba_req, sizeof(struct ADDBA_request));
	paddBaRsp_parm->tid = tid;
	paddBaRsp_parm->status = status;
	paddBaRsp_parm->size = size;
	paddBaRsp_parm->start_seq = start_seq;

	init_h2fwcmd_w_parm_no_rsp(cmd, paddBaRsp_parm, CMD_ADD_BARSP);

	res = rtw_enqueue_cmd(pcmdpriv, cmd);

exit:


	return res;
}

u8 rtw_delba_cmd(struct _ADAPTER *a, u8 *addr, u16 tid)
{
	struct cmd_priv *cmdpriv = &adapter_to_dvobj(a)->cmdpriv;
	struct cmd_obj *cmd = NULL;
	struct addBaReq_parm *parm = NULL;


	cmd = (struct cmd_obj *)rtw_zmalloc(sizeof(struct cmd_obj));
	if (!cmd)
		return _FAIL;
	cmd->padapter = a;

	parm = (struct addBaReq_parm *)rtw_zmalloc(sizeof(struct addBaReq_parm));
	if (!parm) {
		rtw_mfree(cmd, sizeof(struct cmd_obj));
		return _FAIL;
	}

	parm->tid = tid;
	_rtw_memcpy(parm->addr, addr, ETH_ALEN);
	init_h2fwcmd_w_parm_no_rsp(cmd, parm, CMD_DELBA);

	return rtw_enqueue_cmd(cmdpriv, cmd);
}

/* add for CONFIG_IEEE80211W, none 11w can use it */
u8 rtw_reset_securitypriv_cmd(_adapter *padapter)
{
	struct cmd_obj *cmd;
	struct drvextra_cmd_parm  *pdrvextra_cmd_parm;
	struct cmd_priv	*pcmdpriv = &adapter_to_dvobj(padapter)->cmdpriv;
	u8 res = _SUCCESS;


	cmd = (struct cmd_obj *)rtw_zmalloc(sizeof(struct cmd_obj));
	if (cmd == NULL) {
		res = _FAIL;
		goto exit;
	}
	cmd->padapter = padapter;

	pdrvextra_cmd_parm = (struct drvextra_cmd_parm *)rtw_zmalloc(sizeof(struct drvextra_cmd_parm));
	if (pdrvextra_cmd_parm == NULL) {
		rtw_mfree((unsigned char *)cmd, sizeof(struct cmd_obj));
		res = _FAIL;
		goto exit;
	}

	pdrvextra_cmd_parm->ec_id = RESET_SECURITYPRIV;
	pdrvextra_cmd_parm->type = 0;
	pdrvextra_cmd_parm->size = 0;
	pdrvextra_cmd_parm->pbuf = NULL;

	init_h2fwcmd_w_parm_no_rsp(cmd, pdrvextra_cmd_parm, CMD_SET_DRV_EXTRA);


	/* rtw_enqueue_cmd(pcmdpriv, ph2c);	 */
	res = rtw_enqueue_cmd(pcmdpriv, cmd);

exit:
	return res;

}

void free_assoc_resources_hdl(_adapter *padapter, u8 lock_scanned_queue)
{
	rtw_free_assoc_resources(padapter, lock_scanned_queue);
}

u8 rtw_free_assoc_resources_cmd(_adapter *padapter, u8 lock_scanned_queue, int flags)
{
	struct cmd_obj *cmd;
	struct drvextra_cmd_parm  *pdrvextra_cmd_parm;
	struct cmd_priv	*pcmdpriv = &adapter_to_dvobj(padapter)->cmdpriv;
	struct submit_ctx sctx;
	u8	res = _SUCCESS;

	if (flags & RTW_CMDF_DIRECTLY) {
		free_assoc_resources_hdl(padapter, lock_scanned_queue);
	}
	else {
		cmd = (struct cmd_obj *)rtw_zmalloc(sizeof(struct cmd_obj));
		if (cmd == NULL) {
			res = _FAIL;
			goto exit;
		}
		cmd->padapter = padapter;

		pdrvextra_cmd_parm = (struct drvextra_cmd_parm *)rtw_zmalloc(sizeof(struct drvextra_cmd_parm));
		if (pdrvextra_cmd_parm == NULL) {
			rtw_mfree((unsigned char *)cmd, sizeof(struct cmd_obj));
			res = _FAIL;
			goto exit;
		}

		pdrvextra_cmd_parm->ec_id = FREE_ASSOC_RESOURCES;
		pdrvextra_cmd_parm->type = lock_scanned_queue;
		pdrvextra_cmd_parm->size = 0;
		pdrvextra_cmd_parm->pbuf = NULL;

		init_h2fwcmd_w_parm_no_rsp(cmd, pdrvextra_cmd_parm, CMD_SET_DRV_EXTRA);
		if (flags & RTW_CMDF_WAIT_ACK) {
			cmd->sctx = &sctx;
			rtw_sctx_init(&sctx, 2000);
		}

		res = rtw_enqueue_cmd(pcmdpriv, cmd);

		if (res == _SUCCESS && (flags & RTW_CMDF_WAIT_ACK)) {
			rtw_sctx_wait(&sctx, __func__);
			_rtw_mutex_lock_interruptible(&pcmdpriv->sctx_mutex);
			if (sctx.status == RTW_SCTX_SUBMITTED)
				cmd->sctx = NULL;
			_rtw_mutex_unlock(&pcmdpriv->sctx_mutex);
		}
	}
exit:
	return res;

}
#if 0 /*#ifdef CONFIG_CORE_DM_CHK_TIMER*/
u8 rtw_dynamic_chk_wk_cmd(_adapter *padapter)
{
	struct cmd_obj *cmd;
	struct drvextra_cmd_parm *pdrvextra_cmd_parm;
	struct cmd_priv	*pcmdpriv = &adapter_to_dvobj(padapter)->cmdpriv;
	u8	res = _SUCCESS;


	/* only  primary padapter does this cmd */

	cmd = (struct cmd_obj *)rtw_zmalloc(sizeof(struct cmd_obj));
	if (cmd == NULL) {
		res = _FAIL;
		goto exit;
	}
	cmd->padapter = padapter;

	pdrvextra_cmd_parm = (struct drvextra_cmd_parm *)rtw_zmalloc(sizeof(struct drvextra_cmd_parm));
	if (pdrvextra_cmd_parm == NULL) {
		rtw_mfree((unsigned char *)cmd, sizeof(struct cmd_obj));
		res = _FAIL;
		goto exit;
	}

	pdrvextra_cmd_parm->ec_id = DYNAMIC_CHK_WK_CID;
	pdrvextra_cmd_parm->type = 0;
	pdrvextra_cmd_parm->size = 0;
	pdrvextra_cmd_parm->pbuf = NULL;
	init_h2fwcmd_w_parm_no_rsp(cmd, pdrvextra_cmd_parm, CMD_SET_DRV_EXTRA);


	/* rtw_enqueue_cmd(pcmdpriv, ph2c);	 */
	res = rtw_enqueue_cmd(pcmdpriv, cmd);

exit:
	return res;
}
#endif

u8 rtw_set_chbw_cmd(_adapter *padapter, struct _ADAPTER_LINK *padapter_link, enum band_type band, u8 ch, u8 bw, u8 ch_offset, u8 flags)
{
	struct cmd_obj *pcmdobj;
	struct set_ch_parm *set_ch_parm;
	struct cmd_priv *pcmdpriv = &adapter_to_dvobj(padapter)->cmdpriv;
	struct submit_ctx sctx;
	u8 res = _SUCCESS;


	RTW_INFO(FUNC_NDEV_FMT" band:%d, ch:%u, bw:%u, ch_offset:%u\n",
		 FUNC_NDEV_ARG(padapter->pnetdev), band, ch, bw, ch_offset);

	/* check input parameter */

	/* prepare cmd parameter */
	set_ch_parm = (struct set_ch_parm *)rtw_zmalloc(sizeof(*set_ch_parm));
	if (set_ch_parm == NULL) {
		res = _FAIL;
		goto exit;
	}
	set_ch_parm->band = band;
	set_ch_parm->ch = ch;
	set_ch_parm->bw = bw;
	set_ch_parm->ch_offset = ch_offset;
	set_ch_parm->do_rfk = _FALSE; /*TODO - Need check if do_rfk*/
	set_ch_parm->link_idx = padapter_link->wrlink->id;

	if (flags & RTW_CMDF_DIRECTLY) {
		/* no need to enqueue, do the cmd hdl directly and free cmd parameter */
		if (H2C_SUCCESS != rtw_set_chbw_hdl(padapter, (u8 *)set_ch_parm))
			res = _FAIL;

		rtw_mfree((u8 *)set_ch_parm, sizeof(*set_ch_parm));
	} else {
		/* need enqueue, prepare cmd_obj and enqueue */
		pcmdobj = (struct cmd_obj *)rtw_zmalloc(sizeof(struct cmd_obj));
		if (pcmdobj == NULL) {
			rtw_mfree((u8 *)set_ch_parm, sizeof(*set_ch_parm));
			res = _FAIL;
			goto exit;
		}
		pcmdobj->padapter = padapter;

		init_h2fwcmd_w_parm_no_rsp(pcmdobj, set_ch_parm, CMD_SET_CHANNEL);

		if (flags & RTW_CMDF_WAIT_ACK) {
			pcmdobj->sctx = &sctx;
			rtw_sctx_init(&sctx, 10 * 1000);
		}

		res = rtw_enqueue_cmd(pcmdpriv, pcmdobj);

		if (res == _SUCCESS && (flags & RTW_CMDF_WAIT_ACK)) {
			rtw_sctx_wait(&sctx, __func__);
			_rtw_mutex_lock_interruptible(&pcmdpriv->sctx_mutex);
			if (sctx.status == RTW_SCTX_SUBMITTED)
				pcmdobj->sctx = NULL;
			_rtw_mutex_unlock(&pcmdpriv->sctx_mutex);
		}
	}

	/* do something based on res... */
exit:
	RTW_INFO(FUNC_NDEV_FMT" res:%u\n", FUNC_NDEV_ARG(padapter->pnetdev), res);
	return res;
}

#ifdef CONFIG_RTW_LED_HANDLED_BY_CMD_THREAD
u8 rtw_led_blink_cmd(_adapter *padapter, void *pLed)
{
	struct cmd_obj	*pcmdobj;
	struct LedBlink_param *ledBlink_param;
	struct cmd_priv *pcmdpriv = &adapter_to_dvobj(padapter)->cmdpriv;
	u8 res = _SUCCESS;

	pcmdobj = (struct cmd_obj *)rtw_zmalloc(sizeof(struct cmd_obj));
	if (pcmdobj == NULL) {
		res = _FAIL;
		goto exit;
	}
	pcmdobj->padapter = padapter;

	ledBlink_param = (struct LedBlink_param *)rtw_zmalloc(sizeof(struct LedBlink_param));
	if (ledBlink_param == NULL) {
		rtw_mfree((u8 *)pcmdobj, sizeof(struct cmd_obj));
		res = _FAIL;
		goto exit;
	}

	ledBlink_param->pLed = pLed;

	init_h2fwcmd_w_parm_no_rsp(pcmdobj, ledBlink_param, CMD_LEDBLINK);
	res = rtw_enqueue_cmd(pcmdpriv, pcmdobj);

exit:
	return res;
}
#endif /*CONFIG_RTW_LED_HANDLED_BY_CMD_THREAD*/

u8 rtw_tdls_cmd(_adapter *padapter, u8 *addr, u8 option)
{
	u8 res = _SUCCESS;
#ifdef CONFIG_TDLS
	struct	cmd_obj	*pcmdobj;
	struct	TDLSoption_param *TDLSoption;
	struct	mlme_priv *pmlmepriv = &padapter->mlmepriv;
	struct	cmd_priv *pcmdpriv = &adapter_to_dvobj(padapter)->cmdpriv;

	pcmdobj = (struct cmd_obj *)rtw_zmalloc(sizeof(struct cmd_obj));
	if (pcmdobj == NULL) {
		res = _FAIL;
		goto exit;
	}
	pcmdobj->padapter = padapter;

	TDLSoption = (struct TDLSoption_param *)rtw_zmalloc(sizeof(struct TDLSoption_param));
	if (TDLSoption == NULL) {
		rtw_mfree((u8 *)pcmdobj, sizeof(struct cmd_obj));
		res = _FAIL;
		goto exit;
	}

	_rtw_spinlock_bh(&(padapter->tdlsinfo.cmd_lock));
	if (addr != NULL)
		_rtw_memcpy(TDLSoption->addr, addr, 6);
	TDLSoption->option = option;
	_rtw_spinunlock_bh(&(padapter->tdlsinfo.cmd_lock));
	init_h2fwcmd_w_parm_no_rsp(pcmdobj, TDLSoption, CMD_TDLS);
	res = rtw_enqueue_cmd(pcmdpriv, pcmdobj);

exit:
#endif /* CONFIG_TDLS */

	return res;
}

u8 rtw_ssmps_wk_hdl(_adapter *adapter, struct ssmps_cmd_parm *ssmp_param)
{
	u8 res = _SUCCESS;
	struct sta_info *sta = ssmp_param->sta;
	u8 smps = ssmp_param->smps;

	if (sta == NULL)
		return _FALSE;

	if (smps)
		rtw_ssmps_enter(adapter, sta);
	else
		rtw_ssmps_leave(adapter, sta);
	return res;
}

u8 rtw_ssmps_wk_cmd(_adapter *adapter, struct sta_info *sta, u8 smps, u8 enqueue)
{
	struct cmd_obj *cmdobj;
	struct drvextra_cmd_parm *cmd_parm;
	struct ssmps_cmd_parm *ssmp_param;
	struct cmd_priv	*pcmdpriv = &adapter_to_dvobj(adapter)->cmdpriv;
	u8 res = _SUCCESS;

	if (enqueue) {
		cmdobj = (struct cmd_obj *)rtw_zmalloc(sizeof(struct cmd_obj));
		if (cmdobj == NULL) {
			res = _FAIL;
			goto exit;
		}
		cmdobj->padapter = adapter;

		cmd_parm = (struct drvextra_cmd_parm *)rtw_zmalloc(sizeof(struct drvextra_cmd_parm));
		if (cmd_parm == NULL) {
			rtw_mfree((unsigned char *)cmdobj, sizeof(struct cmd_obj));
			res = _FAIL;
			goto exit;
		}

		ssmp_param = (struct ssmps_cmd_parm *)rtw_zmalloc(sizeof(struct ssmps_cmd_parm));
		if (ssmp_param == NULL) {
			rtw_mfree((u8 *)cmdobj, sizeof(struct cmd_obj));
			rtw_mfree((u8 *)cmd_parm, sizeof(struct drvextra_cmd_parm));
			res = _FAIL;
			goto exit;
		}

		ssmp_param->smps = smps;
		ssmp_param->sta = sta;

		cmd_parm->ec_id = SSMPS_WK_CID;
		cmd_parm->type = 0;
		cmd_parm->size = sizeof(struct ssmps_cmd_parm);
		cmd_parm->pbuf = (u8 *)ssmp_param;

		init_h2fwcmd_w_parm_no_rsp(cmdobj, cmd_parm, CMD_SET_DRV_EXTRA);

		res = rtw_enqueue_cmd(pcmdpriv, cmdobj);
	} else {
		struct ssmps_cmd_parm tmp_ssmp_param;

		tmp_ssmp_param.smps = smps;
		tmp_ssmp_param.sta = sta;
		rtw_ssmps_wk_hdl(adapter, &tmp_ssmp_param);
	}

exit:
	return res;
}

#ifdef CONFIG_SUPPORT_STATIC_SMPS
u8 _ssmps_chk_by_tp(_adapter *adapter, u8 from_timer)
{
	u8 enter_smps = _FALSE;
	struct mlme_priv *pmlmepriv = &adapter->mlmepriv;
	struct mlme_ext_priv *pmlmeext = &(adapter->mlmeextpriv);
	struct sta_priv *pstapriv = &adapter->stapriv;
	struct sta_info *psta;
	u32 tx_tp_mbits, rx_tp_mbits;

	if (!MLME_IS_STA(adapter) ||
		!rtw_hw_is_mimo_support(adapter) ||
		!pmlmeext->ssmps_en ||
		(pmlmeext->chandef.chan > 14)
	)
		return enter_smps;

	psta = rtw_get_stainfo(pstapriv, get_bssid(pmlmepriv));
	if (psta == NULL) {
		RTW_ERR(ADPT_FMT" sta == NULL\n", ADPT_ARG(adapter));
		rtw_warn_on(1);
		return enter_smps;
	}
	/*TODO*/
	if (psta->phl_sta->asoc_cap.nss_tx == 1)
		return enter_smps;

	tx_tp_mbits = psta->sta_stats.tx_tp_kbits >> 10;
	rx_tp_mbits = psta->sta_stats.rx_tp_kbits >> 10;

	#ifdef DBG_STATIC_SMPS
	if (pmlmeext->ssmps_test) {
		enter_smps = (pmlmeext->ssmps_test_en == 1) ? _TRUE : _FALSE;
	}
	else
	#endif
	{
		if ((tx_tp_mbits <= pmlmeext->ssmps_tx_tp_th) &&
			(rx_tp_mbits <= pmlmeext->ssmps_rx_tp_th))
			enter_smps = _TRUE;
		else
			enter_smps = _FALSE;
	}

	if (1) {
		RTW_INFO(FUNC_ADPT_FMT" tx_tp:%d [%d], rx_tp:%d [%d] , SSMPS enter :%s\n",
			FUNC_ADPT_ARG(adapter),
			tx_tp_mbits, pmlmeext->ssmps_tx_tp_th,
			rx_tp_mbits, pmlmeext->ssmps_rx_tp_th,
			(enter_smps == _TRUE) ? "True" : "False");
		#ifdef DBG_STATIC_SMPS
		RTW_INFO(FUNC_ADPT_FMT" test:%d test_en:%d\n",
			FUNC_ADPT_ARG(adapter),
			pmlmeext->ssmps_test,
			pmlmeext->ssmps_test_en);
		#endif
	}

	if (enter_smps) {
		if (!from_timer && psta->phl_sta->sm_ps != SM_PS_STATIC)
			rtw_ssmps_enter(adapter, psta);
	} else {
		if (!from_timer && psta->phl_sta->sm_ps != SM_PS_DISABLE)
			rtw_ssmps_leave(adapter, psta);
		else {
			u8 ps_change = _FALSE;

			if (enter_smps && psta->phl_sta->sm_ps != SM_PS_STATIC)
				ps_change = _TRUE;
			else if (!enter_smps && psta->phl_sta->sm_ps != SM_PS_DISABLE)
				ps_change = _TRUE;

			if (ps_change)
				rtw_ssmps_wk_cmd(adapter, psta, enter_smps, 1);
		}
	}

	return enter_smps;
}
#endif /*CONFIG_SUPPORT_STATIC_SMPS*/

#ifdef CONFIG_CTRL_TXSS_BY_TP
void rtw_ctrl_txss_update(_adapter *adapter, struct sta_info *sta)
{
	struct mlme_ext_priv *pmlmeext = &(adapter->mlmeextpriv);

	pmlmeext->txss_bk = sta->phl_sta->asoc_cap.nss_rx;
}

u8 rtw_ctrl_txss(_adapter *adapter, struct sta_info *sta, bool tx_1ss)
{
	struct mlme_ext_priv *pmlmeext = &(adapter->mlmeextpriv);
	struct pwrctrl_priv *pwrpriv = adapter_to_pwrctl(adapter);
	u8 lps_changed = _FALSE;
	u8 rst = _SUCCESS;

	if (pmlmeext->txss_1ss == tx_1ss)
		return _FALSE;
	/*
	if (pwrpriv->bLeisurePs && pwrpriv->pwr_mode != PM_PS_MODE_ACTIVE) {
		lps_changed = _TRUE;
		LPS_Leave(adapter, "LPS_CTRL_TXSS");
	}
	*/

	RTW_INFO(ADPT_FMT" STA [" MAC_FMT "] set tx to %d ss\n",
		ADPT_ARG(adapter), MAC_ARG(sta->phl_sta->mac_addr),
		(tx_1ss) ? 1 : rtw_get_sta_tx_nss(adapter, sta));

	/*update ra*/
	if (tx_1ss)
		sta->phl_sta->asoc_cap.nss_rx = 1;
	else
		sta->phl_sta->asoc_cap.nss_rx = pmlmeext->txss_bk;
	rtw_phl_cmd_change_stainfo(adapter_to_dvobj(adapter)->phl,
					   sta->phl_sta,
					   STA_CHG_RAMASK,
					   NULL,
					   0,
					   PHL_CMD_DIRECTLY,
					   0);

	/*configure trx mode*/
	/*rtw_phydm_trx_cfg(adapter, tx_1ss);*/
	pmlmeext->txss_1ss = tx_1ss;
	/*
	if (lps_changed)
		LPS_Enter(adapter, "LPS_CTRL_TXSS");
	*/

	return rst;
}

u8 rtw_ctrl_txss_wk_hdl(_adapter *adapter, struct txss_cmd_parm *txss_param)
{
	if (!txss_param->sta)
		return _FALSE;

	return rtw_ctrl_txss(adapter, txss_param->sta, txss_param->tx_1ss);
}

u8 rtw_ctrl_txss_wk_cmd(_adapter *adapter, struct sta_info *sta, bool tx_1ss, u8 flag)
{
	struct cmd_obj *cmdobj;
	struct drvextra_cmd_parm *cmd_parm;
	struct txss_cmd_parm *txss_param;
	struct cmd_priv *pcmdpriv = &adapter_to_dvobj(adapter)->cmdpriv;
	struct submit_ctx sctx;
	u8	res = _SUCCESS;

	txss_param = (struct txss_cmd_parm *)rtw_zmalloc(sizeof(struct txss_cmd_parm));
	if (txss_param == NULL) {
		res = _FAIL;
		goto exit;
	}

	txss_param->tx_1ss = tx_1ss;
	txss_param->sta = sta;

	if (flag & RTW_CMDF_DIRECTLY) {
		res = rtw_ctrl_txss_wk_hdl(adapter, txss_param);
		rtw_mfree((u8 *)txss_param, sizeof(*txss_param));
	} else {
		cmdobj = (struct cmd_obj *)rtw_zmalloc(sizeof(struct cmd_obj));
		if (cmdobj == NULL) {
			res = _FAIL;
			goto exit;
		}
		cmdobj->padapter = adapter;

		cmd_parm = (struct drvextra_cmd_parm *)rtw_zmalloc(sizeof(struct drvextra_cmd_parm));
		if (cmd_parm == NULL) {
			rtw_mfree((u8 *)cmdobj, sizeof(struct cmd_obj));
			res = _FAIL;
			goto exit;
		}

		cmd_parm->ec_id = TXSS_WK_CID;
		cmd_parm->type = 0;
		cmd_parm->size = sizeof(struct txss_cmd_parm);
		cmd_parm->pbuf = (u8 *)txss_param;

		init_h2fwcmd_w_parm_no_rsp(cmdobj, cmd_parm, CMD_SET_DRV_EXTRA);

		if (flag & RTW_CMDF_WAIT_ACK) {
			cmdobj->sctx = &sctx;
			rtw_sctx_init(&sctx, 10 * 1000);
		}

		res = rtw_enqueue_cmd(pcmdpriv, cmdobj);
		if (res == _SUCCESS && (flag & RTW_CMDF_WAIT_ACK)) {
			rtw_sctx_wait(&sctx, __func__);
			_rtw_mutex_lock_interruptible(&pcmdpriv->sctx_mutex);
			if (sctx.status == RTW_SCTX_SUBMITTED)
				cmdobj->sctx = NULL;
			_rtw_mutex_unlock(&pcmdpriv->sctx_mutex);
			if (sctx.status != RTW_SCTX_DONE_SUCCESS)
				res = _FAIL;
		}
	}

exit:
	return res;
}

void rtw_ctrl_tx_ss_by_tp(_adapter *adapter, u8 from_timer)
{
	bool tx_1ss  = _FALSE; /*change tx from 2ss to 1ss*/
	struct mlme_priv *pmlmepriv = &adapter->mlmepriv;
	struct mlme_ext_priv *pmlmeext = &(adapter->mlmeextpriv);
	struct sta_priv *pstapriv = &adapter->stapriv;
	struct sta_info *psta;
	u32 tx_tp_mbits;

	if (!MLME_IS_STA(adapter) ||
		!rtw_hw_is_mimo_support(adapter) ||
		!pmlmeext->txss_ctrl_en
	)
		return;

	psta = rtw_get_stainfo(pstapriv, get_bssid(pmlmepriv));
	if (psta == NULL) {
		RTW_ERR(ADPT_FMT" sta == NULL\n", ADPT_ARG(adapter));
		rtw_warn_on(1);
		return;
	}

	tx_tp_mbits = psta->sta_stats.tx_tp_kbits >> 10;
	if (tx_tp_mbits >= pmlmeext->txss_tp_th) {
		tx_1ss = _FALSE;
	} else {
		if (pmlmeext->txss_tp_chk_cnt && --pmlmeext->txss_tp_chk_cnt)
			tx_1ss = _FALSE;
		else
			tx_1ss = _TRUE;
	}

	if (1) {
		RTW_INFO(FUNC_ADPT_FMT" tx_tp:%d [%d] tx_1ss(%d):%s\n",
			FUNC_ADPT_ARG(adapter),
			tx_tp_mbits, pmlmeext->txss_tp_th,
			pmlmeext->txss_tp_chk_cnt,
			(tx_1ss == _TRUE) ? "True" : "False");
	}

	if (pmlmeext->txss_1ss != tx_1ss) {
		if (from_timer)
			rtw_ctrl_txss_wk_cmd(adapter, psta, tx_1ss, 0);
		else
			rtw_ctrl_txss(adapter, psta, tx_1ss);
	}
}
#ifdef DBG_CTRL_TXSS
void dbg_ctrl_txss(_adapter *adapter, bool tx_1ss)
{
	struct mlme_priv *pmlmepriv = &adapter->mlmepriv;
	struct mlme_ext_priv *pmlmeext = &(adapter->mlmeextpriv);
	struct sta_priv *pstapriv = &adapter->stapriv;
	struct sta_info *psta;

	if (!MLME_IS_STA(adapter) ||
		!rtw_hw_is_mimo_support(adapter)
	)
		return;

	psta = rtw_get_stainfo(pstapriv, get_bssid(pmlmepriv));
	if (psta == NULL) {
		RTW_ERR(ADPT_FMT" sta == NULL\n", ADPT_ARG(adapter));
		rtw_warn_on(1);
		return;
	}

	rtw_ctrl_txss(adapter, psta, tx_1ss);
}
#endif
#endif /*CONFIG_CTRL_TXSS_BY_TP*/

/* from_timer == 1 means driver is in LPS */
u8 traffic_status_watchdog(_adapter *padapter, u8 from_timer)
{
	u8	bEnterPS = _FALSE;
	u16 BusyThresholdHigh;
	u16	BusyThresholdLow;
	u16	BusyThreshold;
	u8	bBusyTraffic = _FALSE, bTxBusyTraffic = _FALSE, bRxBusyTraffic = _FALSE;
	u8	bHigherBusyTraffic = _FALSE, bHigherBusyRxTraffic = _FALSE, bHigherBusyTxTraffic = _FALSE;

	struct mlme_priv		*pmlmepriv = &(padapter->mlmepriv);
#ifdef CONFIG_TDLS
	struct tdls_info *ptdlsinfo = &(padapter->tdlsinfo);
	struct tdls_txmgmt txmgmt;
	u8 baddr[ETH_ALEN] = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };
#endif /* CONFIG_TDLS */
#ifdef CONFIG_TRAFFIC_PROTECT
	RT_LINK_DETECT_T *link_detect = &pmlmepriv->LinkDetectInfo;
#endif

#ifdef CONFIG_BTC
	if (padapter->registrypriv.wifi_spec != 1) {
		BusyThresholdHigh = 25;
		BusyThresholdLow = 10;
	} else
#endif /* CONFIG_BTC */
	{
		BusyThresholdHigh = 100;
		BusyThresholdLow = 75;
	}
	BusyThreshold = BusyThresholdHigh;


	/*  */
	/* Determine if our traffic is busy now */
	/*  */
	if ((check_fwstate(pmlmepriv, WIFI_ASOC_STATE) == _TRUE)
	    /*&& !MgntInitAdapterInProgress(pMgntInfo)*/) {
		/* if we raise bBusyTraffic in last watchdog, using lower threshold. */
		if (pmlmepriv->LinkDetectInfo.bBusyTraffic)
			BusyThreshold = BusyThresholdLow;

		if (pmlmepriv->LinkDetectInfo.NumRxOkInPeriod > BusyThreshold ||
		    pmlmepriv->LinkDetectInfo.NumTxOkInPeriod > BusyThreshold) {
			bBusyTraffic = _TRUE;

			if (pmlmepriv->LinkDetectInfo.NumRxOkInPeriod > pmlmepriv->LinkDetectInfo.NumTxOkInPeriod)
				bRxBusyTraffic = _TRUE;
			else
				bTxBusyTraffic = _TRUE;
		}

		/* Higher Tx/Rx data. */
		if (pmlmepriv->LinkDetectInfo.NumRxOkInPeriod > 4000 ||
		    pmlmepriv->LinkDetectInfo.NumTxOkInPeriod > 4000) {
			bHigherBusyTraffic = _TRUE;

			if (pmlmepriv->LinkDetectInfo.NumRxOkInPeriod > pmlmepriv->LinkDetectInfo.NumTxOkInPeriod)
				bHigherBusyRxTraffic = _TRUE;
			else
				bHigherBusyTxTraffic = _TRUE;
		}

#ifdef CONFIG_TRAFFIC_PROTECT
#define TX_ACTIVE_TH 10
#define RX_ACTIVE_TH 20
#define TRAFFIC_PROTECT_PERIOD_MS 4500

		if (link_detect->NumTxOkInPeriod > TX_ACTIVE_TH
		    || link_detect->NumRxUnicastOkInPeriod > RX_ACTIVE_TH) {

			RTW_INFO(FUNC_ADPT_FMT" acqiure wake_lock for %u ms(tx:%d,rx_unicast:%d)\n",
				 FUNC_ADPT_ARG(padapter),
				 TRAFFIC_PROTECT_PERIOD_MS,
				 link_detect->NumTxOkInPeriod,
				 link_detect->NumRxUnicastOkInPeriod);

			rtw_lock_traffic_suspend_timeout(TRAFFIC_PROTECT_PERIOD_MS);
		}
#endif

#ifdef CONFIG_TDLS
#ifdef CONFIG_TDLS_AUTOSETUP
		/* TDLS_WATCHDOG_PERIOD * 2sec, periodically send */
		if (rtw_hw_chk_wl_func(adapter_to_dvobj(padapter), WL_FUNC_TDLS) == _TRUE) {
			if ((ptdlsinfo->watchdog_count % TDLS_WATCHDOG_PERIOD) == 0) {
				_rtw_memcpy(txmgmt.peer, baddr, ETH_ALEN);
				issue_tdls_dis_req(padapter, &txmgmt);
			}
			ptdlsinfo->watchdog_count++;
		}
#endif /* CONFIG_TDLS_AUTOSETUP */
#endif /* CONFIG_TDLS */

#ifdef CONFIG_SUPPORT_STATIC_SMPS
		_ssmps_chk_by_tp(padapter, from_timer);
#endif
#ifdef CONFIG_CTRL_TXSS_BY_TP
		rtw_ctrl_tx_ss_by_tp(padapter, from_timer);
#endif

	}

#if 0
	session_tracker_chk_cmd(padapter, NULL);
#endif

#ifdef CONFIG_BEAMFORMING
#ifdef RTW_WKARD_TX_DISABLE_BFEE
	/*For each padapter*/
	rtw_core_bf_watchdog(padapter);
#endif
#endif

	pmlmepriv->LinkDetectInfo.NumRxOkInPeriod = 0;
	pmlmepriv->LinkDetectInfo.NumTxOkInPeriod = 0;
	pmlmepriv->LinkDetectInfo.NumRxUnicastOkInPeriod = 0;
	pmlmepriv->LinkDetectInfo.bBusyTraffic = bBusyTraffic;
	pmlmepriv->LinkDetectInfo.bTxBusyTraffic = bTxBusyTraffic;
	pmlmepriv->LinkDetectInfo.bRxBusyTraffic = bRxBusyTraffic;
	pmlmepriv->LinkDetectInfo.bHigherBusyTraffic = bHigherBusyTraffic;
	pmlmepriv->LinkDetectInfo.bHigherBusyRxTraffic = bHigherBusyRxTraffic;
	pmlmepriv->LinkDetectInfo.bHigherBusyTxTraffic = bHigherBusyTxTraffic;

	return bEnterPS;

}


/* for 11n Logo 4.2.31/4.2.32 */
void dynamic_update_bcn_check(_adapter *padapter)
{
	struct registry_priv *pregistrypriv = &padapter->registrypriv;
	/* ToDo CONFIG_RTW_MLD: [currently primary link only] */
	struct _ADAPTER_LINK *padapter_link = GET_PRIMARY_LINK(padapter);
	struct link_mlme_priv *pmlmepriv = &(padapter_link->mlmepriv);
	struct link_mlme_ext_priv *pmlmeext = &padapter_link->mlmeextpriv;

	if (!padapter->registrypriv.wifi_spec)
		return;

	if (!pregistrypriv->ht_enable || !is_supported_ht(pregistrypriv->wireless_mode))
		return;

	if (!MLME_IS_AP(padapter))
		return;

	if (pmlmeext->bstart_bss) {
		/* In 10 * 2 = 20s, there are no legacy AP, update HT info  */
		static u8 count = 1;

		if (count % 10 == 0) {
			count = 1;
#ifdef CONFIG_80211N_HT
			if (_FALSE == ATOMIC_READ(&pmlmepriv->olbc)
				&& _FALSE == ATOMIC_READ(&pmlmepriv->olbc_ht)) {

				if (rtw_ht_operation_update(padapter, padapter_link) > 0)
				{
					rtw_update_beacon(padapter, padapter_link,
							_HT_CAPABILITY_IE_, NULL, _FALSE, 0);
					rtw_update_beacon(padapter, padapter_link,
							_HT_ADD_INFO_IE_, NULL, _TRUE, 0);
				}
			}
#endif /* CONFIG_80211N_HT */
		}

#ifdef CONFIG_80211N_HT
		/* In 2s, there are any legacy AP, update HT info, and then reset count  */

		if (_FALSE != ATOMIC_READ(&pmlmepriv->olbc)
			&& _FALSE != ATOMIC_READ(&pmlmepriv->olbc_ht)) {

			if (rtw_ht_operation_update(padapter, padapter_link) > 0)
			{
				rtw_update_beacon(padapter, padapter_link,
						_HT_CAPABILITY_IE_, NULL, _FALSE, 0);
				rtw_update_beacon(padapter, padapter_link,
						_HT_ADD_INFO_IE_, NULL, _TRUE, 0);

			}
			ATOMIC_SET(&pmlmepriv->olbc, _FALSE);
			ATOMIC_SET(&pmlmepriv->olbc_ht, _FALSE);
			count = 0;
		}
#endif /* CONFIG_80211N_HT */
		count ++;
	}
}

struct turbo_edca_setting{
	u32 edca_ul; /* uplink, tx */
	u32 edca_dl; /* downlink, rx */
};

#define TURBO_EDCA_ENT(UL, DL) {UL, DL}

#define TURBO_EDCA_MODE_NUM 8
static struct turbo_edca_setting ctrl_turbo_edca[TURBO_EDCA_MODE_NUM] = {
	/* { UL, DL } */
	TURBO_EDCA_ENT(0x5e431c, 0x431c), /* mode 0 */

	TURBO_EDCA_ENT(0x431c, 0x431c), /* mode 1 */

	TURBO_EDCA_ENT(0x5e431c, 0x5e431c), /* mode 2 */

	TURBO_EDCA_ENT(0x5ea42b, 0x5ea42b), /* mode 3 */

	TURBO_EDCA_ENT(0x5ea42b, 0x431c), /* mode 4 */

	TURBO_EDCA_ENT(0x6ea42b, 0x6ea42b), /* mode 5 */

	TURBO_EDCA_ENT(0xa42b, 0xa42b), /* mode 6 */

	TURBO_EDCA_ENT(0x5e431c, 0xa42b), /* mode 7 */
};

void rtw_turbo_edca(_adapter *padapter)
{
	struct registry_priv *pregpriv = &padapter->registrypriv;
	struct dvobj_priv *dvobj = adapter_to_dvobj(padapter);
	struct rtw_phl_com_t *phl_com = GET_PHL_COM(dvobj);
	u32	edca_be_ul = padapter->last_edca;
	u32	edca_be_dl = padapter->last_edca;
	u32	ac_parm = padapter->last_edca;
	u8	ac;
	u8	is_linked = _FALSE;
	/* ToDo CONFIG_RTW_MLD: currently primary link only */
	struct _ADAPTER_LINK *padapter_link = GET_PRIMARY_LINK(padapter);
	u8	set_edca = _FALSE;

	if (padapter->dis_turboedca == DIS_TURBO)
		return;

	#ifdef CONFIG_THERMAL_PROTECT
	if (phl_com->drv_mode == RTW_DRV_MODE_HIGH_THERMAL) {
		padapter->edca_reset = _TRUE;
		return;
	}
	#endif/*CONFIG_THERMAL_PROTECT*/

	if (padapter->edca_reset == _TRUE ) {
		padapter->edca_reset = _FALSE;
		set_edca = _TRUE;
	}

	if (rtw_mi_check_status(padapter, MI_ASSOC))
		is_linked = _TRUE;

	if (is_linked != _TRUE)
		return;

	if (pregpriv->wifi_spec == 1)
		return;

	/* keep this condition at last check */
	if (padapter->dis_turboedca == DIS_TURBO_USE_MANUAL) {
		if (padapter->edca_param_mode < TURBO_EDCA_MODE_NUM) {

				struct turbo_edca_setting param;

				param = ctrl_turbo_edca[padapter->edca_param_mode];
				edca_be_ul = param.edca_ul;
				edca_be_dl = param.edca_dl;
			} else {
				edca_be_ul = padapter->edca_param_mode;
				edca_be_dl = padapter->edca_param_mode;
		}
	}

	if (phl_com->phl_stats.tx_traffic.lvl == RTW_TFC_HIGH)
		ac_parm = edca_be_ul;
	else if (phl_com->phl_stats.tx_traffic.lvl != RTW_TFC_HIGH)
		ac_parm = edca_be_dl;
	else
		return;

	if ((padapter->last_edca != ac_parm) || (set_edca == _TRUE)) {
		ac = 0;
		/*RTW_INFO("%s, edca(0x%08x), lvl(%d), sts(%d)\n", __func__, ac_parm,
				phl_com->phl_stats.tx_traffic.lvl, phl_com->phl_stats.tx_traffic.sts);*/
		rtw_hw_set_edca(padapter, padapter_link, ac, ac_parm);
		padapter->last_edca = ac_parm;
	}
}

u32 rtw_get_turbo_edca(_adapter *padapter, u8 aifs, u8 ecwmin, u8 ecwmax, u8 txop)
{
	struct registry_priv *pregpriv = &padapter->registrypriv;
	u32 ret = 0;
	u8 decide_txop = txop;
	u8 default_txop = 0x5e;
	u32 ac_parm = padapter->last_edca;
	u8 ac = 0;/*BE*/

	if (padapter->dis_turboedca == DIS_TURBO)
		return ret;

	if (pregpriv->wifi_spec == 1)
		return ret;

	if (default_txop > txop)
		decide_txop = default_txop;
	else
		decide_txop = txop;

	ac_parm = aifs | (ecwmin << 8) | (ecwmax << 12) | (decide_txop << 16);

	return  ac_parm;

}

void rtw_iface_dynamic_chk_wk_hdl(_adapter *padapter)
{
	rtw_turbo_edca(padapter);

	/* for debug purpose */
	_linked_info_dump(padapter);
}

void rtw_dynamic_chk_wk_hdl(_adapter *padapter)
{
	rtw_mi_dynamic_chk_wk_hdl(padapter);

#ifdef DBG_CONFIG_ERROR_DETECT
	rtw_hal_sreset_xmit_status_check(padapter);
	rtw_hal_sreset_linked_status_check(padapter);
#endif

	/* if(check_fwstate(pmlmepriv, WIFI_UNDER_LINKING|WIFI_UNDER_SURVEY)==_FALSE) */
	{
#ifdef DBG_RX_COUNTER_DUMP
		rtw_dump_rx_counters(padapter);
#endif
	}

#ifdef CONFIG_RTW_MULTI_AP
	rtw_ch_util_rpt(padapter);
#endif

#ifdef CONFIG_IPS_CHECK_IN_WD
	/* always call rtw_ps_processor() at last one. */
	rtw_ps_processor(padapter);
#endif
}

void rtw_dynamic_chk_wk_sw_hdl(_adapter *padapter)
{
#ifdef CONFIG_RTW_MULTI_AP
	rtw_ch_util_rpt(padapter);
#endif

	rtw_edcca_hal_update(adapter_to_dvobj(padapter));
}

void rtw_dynamic_chk_wk_hw_hdl(_adapter *padapter)
{
	rtw_mi_dynamic_chk_wk_hdl(padapter);

#ifdef DBG_CONFIG_ERROR_DETECT
	rtw_hal_sreset_xmit_status_check(padapter);
	rtw_hal_sreset_linked_status_check(padapter);
#endif

	/* if(check_fwstate(pmlmepriv, WIFI_UNDER_LINKING|WIFI_UNDER_SURVEY)==_FALSE) */
	{
#ifdef DBG_RX_COUNTER_DUMP
		rtw_dump_rx_counters(padapter);
#endif
	}

#ifdef CONFIG_IPS_CHECK_IN_WD
	/* always call rtw_ps_processor() at last one. */
	rtw_ps_processor(padapter);
#endif

#ifdef RTW_DETECT_HANG
	rtw_is_hang_check(padapter);
#endif
}

/* add for CONFIG_IEEE80211W, none 11w can use it */
void reset_securitypriv_hdl(_adapter *padapter)
{
	rtw_reset_securitypriv(padapter);
}

#ifdef CONFIG_IOCTL_CFG80211
#if 0 /*!CONFIG_PHL_ARCH*/
static u8 _p2p_roch_cmd(_adapter *adapter
	, u64 cookie, struct wireless_dev *wdev
	, struct ieee80211_channel *ch, enum nl80211_channel_type ch_type
	, unsigned int duration
	, u8 flags
)
{
	struct cmd_obj *cmdobj;
	struct drvextra_cmd_parm *parm;
	struct p2p_roch_parm *roch_parm;
	struct cmd_priv *pcmdpriv = &adapter_to_dvobj(adapter)->cmdpriv;
	struct submit_ctx sctx;
	u8 cancel = duration ? 0 : 1;
	u8 res = _SUCCESS;

	roch_parm = (struct p2p_roch_parm *)rtw_zmalloc(sizeof(struct p2p_roch_parm));
	if (roch_parm == NULL) {
		res = _FAIL;
		goto exit;
	}

	roch_parm->cookie = cookie;
	roch_parm->wdev = wdev;
	if (!cancel) {
		_rtw_memcpy(&roch_parm->ch, ch, sizeof(struct ieee80211_channel));
		roch_parm->ch_type = ch_type;
		roch_parm->duration = duration;
	}

	if (flags & RTW_CMDF_DIRECTLY) {
		/* no need to enqueue, do the cmd hdl directly and free cmd parameter */
		if (H2C_SUCCESS != p2p_protocol_wk_hdl(adapter, cancel ? P2P_CANCEL_RO_CH_WK : P2P_RO_CH_WK, (u8 *)roch_parm))
			res = _FAIL;
		rtw_mfree((u8 *)roch_parm, sizeof(*roch_parm));
	} else {
		/* need enqueue, prepare cmd_obj and enqueue */
		parm = (struct drvextra_cmd_parm *)rtw_zmalloc(sizeof(struct drvextra_cmd_parm));
		if (parm == NULL) {
			rtw_mfree((u8 *)roch_parm, sizeof(*roch_parm));
			res = _FAIL;
			goto exit;
		}

		parm->ec_id = P2P_PROTO_WK_CID;
		parm->type = cancel ? P2P_CANCEL_RO_CH_WK : P2P_RO_CH_WK;
		parm->size = sizeof(*roch_parm);
		parm->pbuf = (u8 *)roch_parm;

		cmdobj = (struct cmd_obj *)rtw_zmalloc(sizeof(*cmdobj));
		if (cmdobj == NULL) {
			res = _FAIL;
			rtw_mfree((u8 *)roch_parm, sizeof(*roch_parm));
			rtw_mfree((u8 *)parm, sizeof(*parm));
			goto exit;
		}
		cmdobj->padapter = adapter;

		init_h2fwcmd_w_parm_no_rsp(cmdobj, parm, CMD_SET_DRV_EXTRA);

		if (flags & RTW_CMDF_WAIT_ACK) {
			cmdobj->sctx = &sctx;
			rtw_sctx_init(&sctx, 10 * 1000);
		}

		res = rtw_enqueue_cmd(pcmdpriv, cmdobj);

		if (res == _SUCCESS && (flags & RTW_CMDF_WAIT_ACK)) {
			rtw_sctx_wait(&sctx, __func__);
			_rtw_mutex_lock_interruptible(&pcmdpriv->sctx_mutex);
			if (sctx.status == RTW_SCTX_SUBMITTED)
				cmdobj->sctx = NULL;
			_rtw_mutex_unlock(&pcmdpriv->sctx_mutex);
			if (sctx.status != RTW_SCTX_DONE_SUCCESS)
				res = _FAIL;
		}
	}

exit:
	return res;
}
#endif /*!CONFIG_PHL_ARCH*/

inline u8 rtw_mgnt_tx_cmd(_adapter *adapter, u8 tx_band, u8 tx_ch, u8 no_cck, const u8 *buf, size_t len, int wait_ack, u8 flags)
{
	struct cmd_obj *cmdobj;
	struct drvextra_cmd_parm *parm;
	struct mgnt_tx_parm *mgnt_parm;
	struct cmd_priv *pcmdpriv = &adapter_to_dvobj(adapter)->cmdpriv;
	struct submit_ctx sctx;
	u8	res = _SUCCESS;

	mgnt_parm = (struct mgnt_tx_parm *)rtw_zmalloc(sizeof(struct mgnt_tx_parm));
	if (mgnt_parm == NULL) {
		res = _FAIL;
		goto exit;
	}

	mgnt_parm->tx_band = tx_band;
	mgnt_parm->tx_ch = tx_ch;
	mgnt_parm->no_cck = no_cck;
	mgnt_parm->buf = buf;
	mgnt_parm->len = len;
	mgnt_parm->wait_ack = wait_ack;

	if (flags & RTW_CMDF_DIRECTLY) {
		/* no need to enqueue, do the cmd hdl directly and free cmd parameter */
		if (H2C_SUCCESS != rtw_mgnt_tx_handler(adapter, (u8 *)mgnt_parm))
			res = _FAIL;
		rtw_mfree((u8 *)mgnt_parm, sizeof(*mgnt_parm));
	} else {
		/* need enqueue, prepare cmd_obj and enqueue */
		parm = (struct drvextra_cmd_parm *)rtw_zmalloc(sizeof(struct drvextra_cmd_parm));
		if (parm == NULL) {
			rtw_mfree((u8 *)mgnt_parm, sizeof(*mgnt_parm));
			res = _FAIL;
			goto exit;
		}

		parm->ec_id = MGNT_TX_WK_CID;
		parm->type = 0;
		parm->size = sizeof(*mgnt_parm);
		parm->pbuf = (u8 *)mgnt_parm;

		cmdobj = (struct cmd_obj *)rtw_zmalloc(sizeof(*cmdobj));
		if (cmdobj == NULL) {
			res = _FAIL;
			rtw_mfree((u8 *)mgnt_parm, sizeof(*mgnt_parm));
			rtw_mfree((u8 *)parm, sizeof(*parm));
			goto exit;
		}
		cmdobj->padapter = adapter;

		init_h2fwcmd_w_parm_no_rsp(cmdobj, parm, CMD_SET_DRV_EXTRA);

		if (flags & RTW_CMDF_WAIT_ACK) {
			cmdobj->sctx = &sctx;
			rtw_sctx_init(&sctx, 10 * 1000);
		}

		res = rtw_enqueue_cmd(pcmdpriv, cmdobj);

		if (res == _SUCCESS && (flags & RTW_CMDF_WAIT_ACK)) {
			rtw_sctx_wait(&sctx, __func__);
			_rtw_mutex_lock_interruptible(&pcmdpriv->sctx_mutex);
			if (sctx.status == RTW_SCTX_SUBMITTED)
				cmdobj->sctx = NULL;
			_rtw_mutex_unlock(&pcmdpriv->sctx_mutex);
			if (sctx.status != RTW_SCTX_DONE_SUCCESS)
				res = _FAIL;
		}
	}

exit:
	return res;
}
#endif

#ifdef CONFIG_AP_MODE

static void rtw_chk_hi_queue_hdl(_adapter *padapter)
{
	struct sta_info *psta_bmc;
	struct sta_priv *pstapriv = &padapter->stapriv;
	systime start = rtw_get_current_time();
	u8 empty = _FALSE;
	/* ToDo CONFIG_RTW_MLD: [currently primary link only] */
	struct _ADAPTER_LINK *padapter_link = GET_PRIMARY_LINK(padapter);

	psta_bmc = rtw_get_bcmc_stainfo(padapter, padapter_link);
	if (!psta_bmc)
		return;

	rtw_hal_get_hwreg(padapter, HW_VAR_CHK_HI_QUEUE_EMPTY, &empty);

	while (_FALSE == empty && rtw_get_passing_time_ms(start) < rtw_get_wait_hiq_empty_ms()) {
		rtw_msleep_os(100);
		rtw_hal_get_hwreg(padapter, HW_VAR_CHK_HI_QUEUE_EMPTY, &empty);
	}

	if (psta_bmc->sleepq_len == 0) {
		if (empty == _SUCCESS) {
			bool update_tim = _FALSE;

			if (rtw_tim_map_is_set(padapter, pstapriv->tim_bitmap, 0))
				update_tim = _TRUE;

			rtw_tim_map_clear(padapter, pstapriv->tim_bitmap, 0);
			rtw_tim_map_clear(padapter, pstapriv->sta_dz_bitmap, 0);

			if (update_tim == _TRUE)
				_update_beacon(padapter, padapter_link,
						_TIM_IE_, NULL, _TRUE, 0,"bmc sleepq and HIQ empty");
		} else /* re check again */
			rtw_chk_hi_queue_cmd(padapter);

	}

}

u8 rtw_chk_hi_queue_cmd(_adapter *padapter)
{
	struct cmd_obj	*cmd;
	struct drvextra_cmd_parm *pdrvextra_cmd_parm;
	struct cmd_priv	*pcmdpriv = &adapter_to_dvobj(padapter)->cmdpriv;
	u8	res = _SUCCESS;

	cmd = (struct cmd_obj *)rtw_zmalloc(sizeof(struct cmd_obj));
	if (cmd == NULL) {
		res = _FAIL;
		goto exit;
	}
	cmd->padapter = padapter;

	pdrvextra_cmd_parm = (struct drvextra_cmd_parm *)rtw_zmalloc(sizeof(struct drvextra_cmd_parm));
	if (pdrvextra_cmd_parm == NULL) {
		rtw_mfree((unsigned char *)cmd, sizeof(struct cmd_obj));
		res = _FAIL;
		goto exit;
	}

	pdrvextra_cmd_parm->ec_id = CHECK_HIQ_WK_CID;
	pdrvextra_cmd_parm->type = 0;
	pdrvextra_cmd_parm->size = 0;
	pdrvextra_cmd_parm->pbuf = NULL;

	init_h2fwcmd_w_parm_no_rsp(cmd, pdrvextra_cmd_parm, CMD_SET_DRV_EXTRA);

	res = rtw_enqueue_cmd(pcmdpriv, cmd);

exit:
	return res;

}
#endif /* CONFIG_AP_MODE */

#if 0 /*def RTW_PHL_DBG_CMD*/

u8 sample_txwd[] = 
{
/*dword 00*/	0x80, 0x00, 0x40, 0x00,
/*dword 01*/	0x00, 0x00, 0x00, 0x00,  
/*dword 02*/	0xF2, 0x05, 0x00, 0x00, 
/*dword 03*/	0x3E, 0x11, 0x00, 0x00,  
/*dword 04*/	0x00, 0x00, 0x00, 0x00, 
/*dword 05*/	0x00, 0x00, 0x00, 0x00,  
/*dword 06*/	0x00, 0x07, 0x9B, 0x63, 
/*dword 07*/	0x3F, 0x00, 0x00, 0x00,  
/*dword 08*/	0x00, 0x00, 0x00, 0x00, 
/*dword 09*/	0x00, 0x00, 0x00, 0x00,  
/*dword 10*/	0x0C, 0x00, 0x00, 0x00, 
/*dword 11*/	0x00, 0x00, 0x00, 0x00,  
};

#define WD_SPEC(_name, _idx_dw, _bit_start, _bit_end) {              \
	.name        	= (_name),                   \
	.idx_dw         = (_idx_dw),                     \
	.bit_start      = (_bit_start),                   \
	.bit_end 		= (_bit_end)  \
}

struct parse_wd {
	char name[32];
	u8 idx_dw;
	u8 bit_start;
	u8 bit_end;
};

#define MAX_PHL_CMD_LEN 200
#define MAX_PHL_CMD_NUM 10
#define MAX_PHL_CMD_STR_LEN 200

char *get_next_para_str(char *para)
{
	return (para+MAX_PHL_CMD_STR_LEN);
}

static struct parse_wd parse_txwd_8852ae_full[] = {
	WD_SPEC("EN_HWSEQ_MODE"			, 0, 0, 1),
	WD_SPEC("HW_SSN_SEL"			, 0, 2, 3),
	WD_SPEC("SMH_EN"				, 0, 4, 4),
	WD_SPEC("HWAMSDU"				, 0, 5, 5),
	WD_SPEC("HW_AES_IV"				, 0, 6, 6),
	WD_SPEC("WD page"				, 0, 7, 7),
	WD_SPEC("CHK_EN"				, 0, 8, 8),
	WD_SPEC("WP_INT"				, 0, 9, 9),
	WD_SPEC("STF mode"				, 0, 10, 10),
	WD_SPEC("HEADERwLLC_LEN"		, 0, 11, 15),
	WD_SPEC("CHANNEL_DMA"			, 0, 16, 19),
	WD_SPEC("FW_download"			, 0, 20, 20),
	WD_SPEC("PKT_OFFSET"			, 0, 21, 21),
	WD_SPEC("WDINFO_EN"				, 0, 22, 22),
	WD_SPEC("MOREDATA"				, 0, 23, 23),
	WD_SPEC("WP_OFFSET" 			, 0, 24, 31),

	WD_SPEC("SHCUT_CAMID"			, 1, 0, 7),
	WD_SPEC("DMA_TXAGG_NUM"			, 1, 8, 15),
	WD_SPEC("PLD(Packet ID)"		, 1, 16, 31),

	WD_SPEC("TXPKTSIZE"			, 2, 0, 13),
	WD_SPEC("RU_TC"				, 2, 14, 16),
	WD_SPEC("QSEL"				, 2, 17, 22),
	WD_SPEC("TID_indicate"		, 2, 23, 23),
	WD_SPEC("MACID"				, 2, 24, 30),
	WD_SPEC("RSVD"				, 2, 31, 31),

	WD_SPEC("Wifi_SEQ"			, 3, 0, 11),
	WD_SPEC("AGG_EN"			, 3, 12, 12),
	WD_SPEC("BK"				, 3, 13, 13),
	WD_SPEC("RTS_TC"			, 3, 14, 19),
	WD_SPEC("DATA_TC"			, 3, 20, 25),
	WD_SPEC("MU_2nd_TC"			, 3, 26, 28),
	WD_SPEC("MU_TC"				, 3, 29, 31),

	WD_SPEC("TIMESTAMP"			, 4, 0, 15),
	WD_SPEC("AES_IV_L"			, 4, 16, 31),

	WD_SPEC("AES_IV_H"			, 5, 0, 31),	

	WD_SPEC("MBSSID"			, 6, 0, 3),
	WD_SPEC("Multiport_ID"		, 6, 4, 6),
	WD_SPEC("RSVD"				, 6, 7, 7),
	WD_SPEC("DATA_BW_ER"		, 6, 8, 8),
	WD_SPEC("DISRTSFB"			, 6, 9, 9),
	WD_SPEC("DISDATAFB"			, 6, 10, 10),
	WD_SPEC("DATA_LDPC"			, 6, 11, 11),
	WD_SPEC("DATA_STBC"			, 6, 12, 13),
	WD_SPEC("DATA_DCM"			, 6, 14, 14),
	WD_SPEC("DATA_ER"			, 6, 15, 15),
	WD_SPEC("DataRate"			, 6, 16, 24),
	WD_SPEC("GI_LTF"			, 6, 25, 27),
	WD_SPEC("DATA_BW"			, 6, 28, 29),
	WD_SPEC("USERATE_SEL" 		, 6, 30, 30),
	WD_SPEC("ACK_CH_INFO"		, 6, 31, 31),

	WD_SPEC("MAX_AGG_NUM"				, 7, 0, 7),
	WD_SPEC("BCN_SRCH_SEQ"				, 7, 8, 9),
	WD_SPEC("NAVUSEHDR"					, 7, 10, 10),
	WD_SPEC("BMC"						, 7, 11, 11),
	WD_SPEC("A_CTRL_BQR"				, 7, 12, 12),
	WD_SPEC("A_CTRL_UPH"				, 7, 13, 13),
	WD_SPEC("A_CTRL_BSR"				, 7, 14, 14),
	WD_SPEC("A_CTRL_CAS"				, 7, 15, 15),
	WD_SPEC("DATA_RTY_LOWEST_RATE"		, 7, 16, 24),
	WD_SPEC("DATA_TXCNT_LMT"			, 7, 25, 30),
	WD_SPEC("DATA_TXCNT_LMT_SEL"		, 7, 31, 31),

	WD_SPEC("SEC_CAM_IDX"				, 8, 0, 7),
	WD_SPEC("SEC_HW_ENC"				, 8, 8, 8),
	WD_SPEC("SECTYPE"					, 8, 9, 12),
	WD_SPEC("lifetime_sel"				, 8, 13, 15),
	WD_SPEC("RSVD"						, 8, 16, 16),
	WD_SPEC("FORCE_TXOP"				, 8, 17, 17),
	WD_SPEC("AMPDU_DENSITY"				, 8, 18, 20),
	WD_SPEC("LSIG_TXOP_EN"				, 8, 21, 21),
	WD_SPEC("TXPWR_OFSET_TYPE"			, 8, 22, 24),
	WD_SPEC("RSVD"						, 8, 25, 25),	
	WD_SPEC("obw_cts2self_dup_type"		, 8, 26, 29),
	WD_SPEC("RSVD"						, 8, 30, 31),

	WD_SPEC("Signaling_TA_PKT_EN"		, 9, 0, 0),
	WD_SPEC("NDPA"						, 9, 1, 2),
	WD_SPEC("SND_PKT_SEL"				, 9, 3, 5),
	WD_SPEC("SIFS_Tx"					, 9, 6, 6),
	WD_SPEC("HT_DATA_SND" 				, 9, 7, 7),
	WD_SPEC("RSVD"						, 9, 8, 8),
	WD_SPEC("RTT_EN" 					, 9, 9, 9),
	WD_SPEC("SPE_RPT" 					, 9, 10, 10),
	WD_SPEC("BT_NULL" 					, 9, 11, 11),
	WD_SPEC("TRI_FRAME"					, 9, 12, 12),
	WD_SPEC("NULL_1" 					, 9, 13, 13),
	WD_SPEC("NULL_0" 					, 9, 14, 14),
	WD_SPEC("RAW" 						, 9, 15, 15),
	WD_SPEC("Group_bit"					, 9, 16, 23),
	WD_SPEC("RSVD" 						, 9, 24, 25),
	WD_SPEC("BCNPKT_TSF_CTRL" 			, 9, 26, 26),
	WD_SPEC("Signaling_TA_PKT_SC" 		, 9, 27, 30),
	WD_SPEC("FORCE_BSS_CLR" 			, 9, 31, 31),

	WD_SPEC("SW_DEFINE"					, 10, 0, 3),
	WD_SPEC("RSVD"						, 10, 4, 26),
	WD_SPEC("RTS_EN"					, 10, 27, 27),
	WD_SPEC("CTS2SELF"					, 10, 28, 28),
	WD_SPEC("CCA_RTS" 					, 10, 29, 30),
	WD_SPEC("HW_RTS_EN" 				, 10, 31, 31),

	WD_SPEC("RSVD"						, 11, 0, 3),
	WD_SPEC("NDPA_duration"				, 11, 4, 26),
};

static struct parse_wd parse_rxwd_8852ae[] = {
	WD_SPEC("PKT_LEN"				, 0, 0, 13),
	WD_SPEC("SHIFT"					, 0, 14, 15),
	WD_SPEC("WL_HD_IV_LEN"			, 0, 16, 21),
	WD_SPEC("BB_SEL"				, 0, 22, 22),
	WD_SPEC("MAC_INFO_VLD"			, 0, 23, 23),
	WD_SPEC("RPKT_TYPE"				, 0, 24, 27),
	WD_SPEC("DRV_INFO_SIZE"			, 0, 28, 30),
	WD_SPEC("LONG_RXD"				, 0, 31, 31),
	
	WD_SPEC("PPDU_TYPE"			, 1, 0, 3),
	WD_SPEC("PPDU_CNT"			, 1, 4, 6),
	WD_SPEC("SR_EN"				, 1, 7, 7),
	WD_SPEC("USER_ID"			, 1, 8, 15),
	WD_SPEC("RX_DATARATE" 		, 1, 16, 24),
	WD_SPEC("RX_GI_LTF"			, 1, 25, 27),
	WD_SPEC("NON_SRG_PPDU"		, 1, 28, 28),
	WD_SPEC("INTER_PPDU" 		, 1, 29, 29),
	WD_SPEC("BW"				, 1, 30, 31),

	WD_SPEC("FREERUN_CNT"		, 2, 0, 31),

	WD_SPEC("A1_MATCH"			, 3, 1, 1),
	WD_SPEC("SW_DEC"			, 3, 2, 2),
	WD_SPEC("HW_DEC"			, 3, 3, 3),
	WD_SPEC("AMPDU"				, 3, 4, 4),
	WD_SPEC("AMPDU_END_PKT"		, 3, 5, 5),
	WD_SPEC("AMSDU"				, 3, 6, 6),
	WD_SPEC("AMSDU_CUT"			, 3, 7, 7),
	WD_SPEC("LAST_MSDU"			, 3, 8, 8),
	WD_SPEC("BYPASS"			, 3, 9, 9),
	WD_SPEC("CRC32" 			, 3, 10, 10),
	WD_SPEC("MAGIC_WAKE" 		, 3, 11, 11),
	WD_SPEC("UNICAST_WAKE"		, 3, 12, 12),
	WD_SPEC("PATTERN_WAKE"		, 3, 13, 13),
	WD_SPEC("GET_CH_INFO" 		, 3, 14, 14),
	WD_SPEC("RX_STATISTICS" 	, 3, 15, 15),
	WD_SPEC("PATTERN_IDX"		, 3, 16, 20),
	WD_SPEC("TARGET_IDC"		, 3, 21, 23),
	WD_SPEC("CHKSUM_OFFLOAD_EN"	, 3, 24, 24),
	WD_SPEC("WITH_LLC"			, 3, 25, 25),
	WD_SPEC("RSVD" 				, 3, 26, 31),

	WD_SPEC("TYPE"			, 4, 0, 1),
	WD_SPEC("MC"			, 4, 2, 2),
	WD_SPEC("BC" 			, 4, 3, 3),
	WD_SPEC("MD"			, 4, 4, 4),
	WD_SPEC("MF" 			, 4, 5, 5),
	WD_SPEC("PWR"			, 4, 6, 6),
	WD_SPEC("QOS" 			, 4, 7, 7),
	WD_SPEC("TID"			, 4, 8, 11),
	WD_SPEC("EOSP" 			, 4, 12, 12),
	WD_SPEC("HTC"			, 4, 13, 13),
	WD_SPEC("QNULL"			, 4, 14, 14),
	WD_SPEC("RSVD" 			, 4, 15, 15),
	WD_SPEC("SEQ"			, 4, 16, 27),
	WD_SPEC("FRAG" 			, 4, 28, 31),
	
	WD_SPEC("SEC_CAM_IDX"		, 5, 0, 7),	
	WD_SPEC("ADDR_CAM"			, 5, 8, 15),	
	WD_SPEC("MAC_ID"			, 5, 16, 23),	
	WD_SPEC("RX_PL_ID"			, 5, 24, 27),	
	WD_SPEC("ADDR_CAM_VLD"		, 5, 28, 28),	
	WD_SPEC("ADDR_FWD_EN"		, 5, 29, 29),	
	WD_SPEC("RX_PL_MATCH"		, 5, 30, 30),	
	WD_SPEC("RSVD"				, 5, 31, 31),	
};


enum WD_TYPE {
	TXWD_INFO = 0,
	TXWD_INFO_BODY,
	RXWD, 

};

u32 get_txdesc_element_val(u32 val_dw, u8 bit_start, u8 bit_end)
{
	u32 mask = 0;
	u32 i = 0;

	if(bit_start>31 
		|| bit_end>31
		|| (bit_start>bit_end)){
		printk("[%s] error %d %d\n", __FUNCTION__, bit_start, bit_end);
		return 0;
	}

	for(i = bit_start; i<=bit_end; i++){
		mask |= (1<<i);
	}

	return ((val_dw & mask)>>bit_start);
}


void parse_wd_8852ae(_adapter *adapter, u32 type, u32 idx_wd, u8 *wd)
{
	u32 i, val = 0;
	u32 cur_dw = 0xFF;
	u32 idx, val_dw = 0;
	u32 array_size = 0;
	struct parse_wd *parser = NULL;

	if(wd==NULL)
		return;

	if(type == TXWD_INFO_BODY){
		parser = parse_txwd_8852ae_full;
		array_size = ARRAY_SIZE(parse_txwd_8852ae_full);
	}
	else if(type == RXWD){
		parser = parse_rxwd_8852ae;
		array_size = ARRAY_SIZE(parse_rxwd_8852ae);
	}

	for(i = 0; i<array_size; i++){
			if(cur_dw != parser[i].idx_dw){
				cur_dw = parser[i].idx_dw;
				idx = (parser[i].idx_dw*4);
				val_dw = wd[idx] + (wd[idx+1]<<8) + (wd[idx+2]<<16) + (wd[idx+3]<<24);
				printk(">>>> WD[%03d].dw%02d = 0x%08x \n", idx_wd, cur_dw, val_dw);
			}

			val = get_txdesc_element_val(val_dw, 
				parser[i].bit_start, parser[i].bit_end);
				
			printk("%s[%d:%d] = (0x%x)\n",  
				parser[i].name, 
				parser[i].bit_end, parser[i].bit_start, val);
		}
		printk("\n");
	
}


void compare_wd_8852ae(_adapter *adapter, u32 type, u8 *wd1, u8 *wd2)
{
	u32 i, val1, val2 = 0;
	u32 cur_dw = 0xFF;
	u32 idx, val_dw1, val_dw2 = 0;
	u32 array_size = 0;
	struct parse_wd *parser = NULL;

	if((wd1==NULL) ||(wd2==NULL))
		return;

	if(type == TXWD_INFO_BODY){
		parser = parse_txwd_8852ae_full;
		array_size = ARRAY_SIZE(parse_txwd_8852ae_full);
	}

	for(i = 0; i<array_size; i++){
			if(cur_dw != parser[i].idx_dw){
				cur_dw = parser[i].idx_dw;
				idx = (parser[i].idx_dw*4);
				val_dw1 = wd1[idx] + (wd1[idx+1]<<8) + (wd1[idx+2]<<16) + (wd1[idx+3]<<24);
				val_dw2 = wd2[idx] + (wd2[idx+1]<<8) + (wd2[idx+2]<<16) + (wd2[idx+3]<<24);
			}

			val1 = get_txdesc_element_val(val_dw1, 
				parser[i].bit_start, parser[i].bit_end);
			val2 = get_txdesc_element_val(val_dw2, 
				parser[i].bit_start, parser[i].bit_end);

			if(val1 != val2){
				printk("Diff dw%02d: %s[%d:%d] = (0x%x) vs (0x%x)\n", cur_dw,
					parser[i].name, 
					parser[i].bit_end, parser[i].bit_start, val1, val2);
			}
		}
		printk("\n");
	
}

void core_dump_map_tx(_adapter *adapter)
{
	struct core_logs *log = &adapter->core_logs;
	u32 idx = 0;

 	printk("drvTx MAP");
	for(idx=0; idx<CORE_LOG_NUM; idx++){
		struct core_record *record = &log->drvTx[idx];
		if(idx >= log->txCnt_all)
			break;
		printk("[drvTx %03d]\n", idx);
		printk("type=%d totalSz=%d virtAddr=%p\n", record->type, record->totalSz, record->virtAddr[0]);
	}

	printk("========= \n\n");
	printk("phlTx MAP");
	for(idx=0; idx<CORE_LOG_NUM; idx++){
		struct core_record *record = &log->phlTx[idx];
		u32 idx1 = 0;
		if(idx >= log->txCnt_phl)
			break;
		printk("[phlTx %03d]\n", idx);
		printk("type=%d totalSz=%d fragNum=%d\n", record->type, record->totalSz, record->fragNum);
		for(idx1=0; idx1<record->fragNum; idx1++){
			printk("frag#%d: len=%d virtaddr=%p \n", idx1, 
				record->fragLen[idx1], record->virtAddr[idx1]);
			printk("frag#%d: phyAddrH=%d phyAddrL=%p \n", idx1, 
				record->phyAddrH[idx1], record->phyAddrL[idx1]);			
		}
	}

	printk("========= \n\n");
	printk("TxRcycle MAP");
	for(idx=0; idx<CORE_LOG_NUM; idx++){
		struct core_record *record = &log->txRcycle[idx];
		u32 idx1 = 0;
		if(idx >= log->txCnt_recycle)
			break;
		printk("[TxRcycle %03d]\n", idx);
		printk("type=%d totalSz=%d fragNum=%d\n", record->type, record->totalSz, record->fragNum);
		for(idx1=0; idx1<record->fragNum; idx1++){
			printk("frag#%d: len=%d virtaddr=%p \n", idx1, 
				record->fragLen[idx1], record->virtAddr[idx1]);
			printk("frag#%d: phyAddrH=%d phyAddrL=%p \n", idx1, 
				record->phyAddrH[idx1], record->phyAddrL[idx1]);			
		}
	}
}

void core_dump_map_rx(_adapter *adapter)
{
	struct core_logs *log = &adapter->core_logs;
	u32 idx = 0;

 	printk("drvRx MAP");
	for(idx=0; idx<CORE_LOG_NUM; idx++){
		struct core_record *record = &log->drvRx[idx];
		if(idx >= log->rxCnt_all)
			break;
		printk("[drvRx %03d]\n", idx);
		printk("type=%d totalSz=%d virtAddr=%p\n", record->type, record->totalSz, record->virtAddr[0]);
		printk("wl_seq=%d wl_type=0x%x wl_subtype=0x%x\n", record->wl_seq, record->wl_type, record->wl_subtype);
	}

	printk("========= \n\n");
	printk("phlRx MAP");
	for(idx=0; idx<CORE_LOG_NUM; idx++){
		struct core_record *record = &log->phlRx[idx];
		if(idx >= log->rxCnt_phl)
			break;
		printk("[phlRx %03d]\n", idx);
		printk("type=%d totalSz=%d virtAddr=%p\n", record->type, record->totalSz, record->virtAddr[0]);
	}

	printk("========= \n\n");
	printk("rxRcycle MAP");
	for(idx=0; idx<CORE_LOG_NUM; idx++){
		struct core_record *record = &log->rxRcycle[idx];
		if(idx >= log->rxCnt_recycle)
			break;
		printk("[rxRcycle %03d]\n", idx);
		printk("type=%d totalSz=%d virtAddr=%p\n", record->type, record->totalSz, record->virtAddr[0]);
	}

}


void core_dump_record(_adapter *adapter, u8 dump_type)
{
	struct core_logs *log = &adapter->core_logs;
	u32 idx = 0;

	printk("txCnt_all: %d (%d) \n", log->txCnt_all, log->txCnt_all%CORE_LOG_NUM);
	printk("txCnt_data: %d \n", log->txCnt_data);
	printk("txCnt_mgmt: %d \n", log->txCnt_mgmt);
	printk("txCnt_phl: %d (%d), Sz=%d \n", log->txCnt_phl, log->txCnt_phl%CORE_LOG_NUM, log->txSize_phl);
	printk("txCnt_recycle: %d (%d), Sz=%d \n", log->txCnt_recycle, log->txCnt_recycle%CORE_LOG_NUM, log->txSize_recycle);

	printk("rxCnt_all: %d (%d) \n", log->rxCnt_all, log->rxCnt_all%CORE_LOG_NUM);
	printk("rxCnt_data: %d (retry=%d)\n", log->rxCnt_data, log->rxCnt_data_retry);
	printk("rxCnt_mgmt: %d \n", log->rxCnt_mgmt);
	printk("rxCnt_phl: %d (%d), Sz=%d \n", log->rxCnt_phl, log->rxCnt_phl%CORE_LOG_NUM, log->rxSize_phl);
	printk("rxCnt_recycle: %d (%d), Sz=%d\n", log->rxCnt_recycle, log->rxCnt_recycle%CORE_LOG_NUM, log->rxSize_recycle);
#ifdef CONFIG_RTW_CORE_RXSC
	printk("enable_rxsc: %d \n", adapter->enable_rxsc);
	printk("rxCnt_data: orig=%d shortcut=%d(ratio=%d)\n",
		log->rxCnt_data_orig, log->rxCnt_data_shortcut,
		log->rxCnt_data_shortcut*100/(log->rxCnt_data_orig+log->rxCnt_data_shortcut));
	printk("rxCnt_coreInd: %d \n", log->rxCnt_coreInd);
#endif

	if(dump_type == REC_DUMP_TX){
		core_dump_map_tx(adapter);
	}
	else if(dump_type == REC_DUMP_RX){
		core_dump_map_rx(adapter);
	}
	else if(dump_type == REC_DUMP_ALL){
		core_dump_map_tx(adapter);
		core_dump_map_rx(adapter);
	}
}

void core_add_record(_adapter *adapter, u8 type, void *p)
{
	struct core_logs *log = &adapter->core_logs;

	if(!adapter->record_enable)
		return;

	if(type == REC_TX_DATA){
		u32 idx = log->txCnt_all%CORE_LOG_NUM;
		struct core_record *record = &(log->drvTx[idx]);
		struct sk_buff *skb = p;
		
		log->txCnt_data++;
		record->type = type;
		record->totalSz = skb->len;
		record->virtAddr[0] = skb->data;
	}

	if(type == REC_TX_MGMT){
		u32 idx = log->txCnt_all%CORE_LOG_NUM;
		struct core_record *record = &(log->drvTx[idx]);
		struct xmit_frame *pxframe = p;
		
		log->txCnt_mgmt++;
		record->type = type;
		record->totalSz = pxframe->attrib.pktlen;
		record->virtAddr[0] = pxframe->buf_addr;
	}

	if(type == REC_TX_PHL || type == REC_TX_PHL_RCC){
		u32 idx = 0;
		struct core_record *record = NULL;
		struct rtw_xmit_req *txreq = p;

		if(type == REC_TX_PHL){
			idx = log->txCnt_phl%CORE_LOG_NUM;
			record = &(log->phlTx[idx]);
			log->txCnt_phl++;
		}
		
		if(type == REC_TX_PHL_RCC){
			idx = log->txCnt_recycle%CORE_LOG_NUM;
			record = &(log->txRcycle[idx]);
			log->txCnt_recycle++;
		}
		
		record->type = type;
		record->totalSz = 0;
		record->fragNum = txreq->pkt_cnt;

		{
			struct rtw_pkt_buf_list *pkt_list =(struct rtw_pkt_buf_list *)txreq->pkt_list;
			u32 idx1 = 0;
			for(idx1=0; idx1<txreq->pkt_cnt; idx1++){
				if(idx1 >= MAX_FRAG){
					printk("!! WARN[%s][%d] type=%d frag>= %d \n", 
						__FUNCTION__, __LINE__, type, MAX_FRAG);
					break;
				}
				record->totalSz += pkt_list->length;
				record->fragLen[idx1] = pkt_list->length;
				record->virtAddr[idx1] = pkt_list->vir_addr;
				record->phyAddrL[idx1] = pkt_list->phy_addr_l;
				record->phyAddrH[idx1] = pkt_list->phy_addr_h;
				pkt_list++;
			}
		}

		if(type == REC_TX_PHL)
			log->txSize_phl += record->totalSz;
		else if(type == REC_TX_PHL_RCC)
			log->txSize_recycle += record->totalSz;
		
	}

	if(type == REC_RX_PHL || type == REC_RX_PHL_RCC){
		u32 idx = 0;
		struct core_record *record = NULL;
		struct rtw_recv_pkt *rx_req = p;
		struct rtw_pkt_buf_list *pkt = rx_req->pkt_list;

		if(type == REC_RX_PHL){
			idx = log->rxCnt_phl%CORE_LOG_NUM;
			record = &(log->phlRx[idx]);
			log->rxCnt_phl++;
		}

		if(type == REC_RX_PHL_RCC){
			idx = log->rxCnt_recycle%CORE_LOG_NUM;
			record = &(log->rxRcycle[idx]);
			log->rxCnt_recycle++;
		}

		record->type = type;
		record->totalSz = pkt->length;
		record->virtAddr[0] = pkt->vir_addr;

		if(type == REC_RX_PHL)
			log->rxSize_phl += record->totalSz;
		else if(type == REC_RX_PHL_RCC)
			log->rxSize_recycle += record->totalSz;
	}

	if(type == REC_RX_DATA || type == REC_RX_MGMT){
		u32 idx = log->rxCnt_all%CORE_LOG_NUM;
		struct core_record *record = &(log->drvRx[idx]);
		union recv_frame *prframe = p;
		struct rx_pkt_attrib *pattrib = &prframe->u.hdr.attrib;
		
		if(type == REC_RX_DATA){
			log->rxCnt_data++;
		}
		
		if(type == REC_RX_MGMT){
			log->rxCnt_mgmt++;
		}

		record->type = type;
		record->totalSz = prframe->u.hdr.len;
		record->virtAddr[0] = prframe->u.hdr.rx_data;

		record->wl_seq = pattrib->seq_num;
		record->wl_type = pattrib->wl_type;
		record->wl_subtype = pattrib->wl_subtype;

	}

	if(type == REC_RX_DATA_RETRY){
		log->rxCnt_data_retry++;
	}

	log->txCnt_all = log->txCnt_mgmt + log->txCnt_data;
	log->rxCnt_all = log->rxCnt_mgmt + log->rxCnt_data;
}


void phl_dump_map_tx(_adapter *adapter)
{
	struct phl_logs *log = &adapter->phl_logs;
	u32 idx = 0;

 	printk("txBd MAP");
	for(idx=0; idx<CORE_LOG_NUM; idx++){
		struct record_txbd *record = &log->txBd[idx];
		if(idx >= log->txCnt_bd)
			break;
		printk("[txBd %03d]\n", idx);
		{
			u8 *tmp=record->bd_buf;
			u32 len = record->bd_len;
			u32 idx1 = 0;
			if(tmp == NULL)
				break;
			for(idx1=0; idx1<len; idx1++){
				if(idx1%8==0) {
					printk("[%03d] %02x %02x %02x %02x %02x %02x %02x %02x \n", 
						idx1, tmp[idx1], tmp[idx1+1], tmp[idx1+2], tmp[idx1+3], 
						tmp[idx1+4], tmp[idx1+5], tmp[idx1+6], tmp[idx1+7]);
				}
			}
			printk("\n");
		}
	}
	
	printk("========= \n\n");
	printk("txWd MAP");
	for(idx=0; idx<CORE_LOG_NUM; idx++){
		struct record_txwd *record = &log->txWd[idx];
		if(idx >= log->txCnt_wd)
			break;
		printk("[txWd %03d]\n", idx);
		{
			u8 *tmp=record->wd_buf;
			u32 len = record->wd_len;
			u32 idx1 = 0;
			if(tmp == NULL)
				break;
			for(idx1=0; idx1<len; idx1++){
				if(idx1%8==0) {
					printk("[%03d] %02x %02x %02x %02x %02x %02x %02x %02x \n", 
						idx1, tmp[idx1], tmp[idx1+1], tmp[idx1+2], tmp[idx1+3], 
						tmp[idx1+4], tmp[idx1+5], tmp[idx1+6], tmp[idx1+7]);
				}
			}
			printk("\n");
		}
		parse_wd_8852ae(adapter, TXWD_INFO_BODY, idx, record->wd_buf);
		//compare_wd_8852ae(adapter, TXWD_INFO_BODY, record->wd_buf, sample_txwd);
	}
	
	printk("========= \n\n");
 	printk("wpRecycle MAP");
	for(idx=0; idx<CORE_LOG_NUM; idx++){
		struct record_wp_rcc *record = &log->wpRecycle[idx];
		if(idx >= log->txCnt_recycle)
			break;
		printk("[txRecycle %03d]\n", idx);
		printk("wp_seq=%d \n", record->wp_seq);
	}
}

void phl_dump_map_rx(_adapter *adapter)
{
	struct phl_logs *log = &adapter->phl_logs;
	u32 idx = 0;

	printk("rxPciMap MAP");
	for(idx=0; idx<CORE_LOG_NUM; idx++){
		struct record_pci *record = &log->rxPciMap[idx];
		if(idx >= log->rxCnt_map)
			break;
		printk("[rxPciMap %03d]\n", idx);
		printk("phyAddrL=%p len=%d\n", record->phyAddrL, record->map_len);
	}


	printk("========= \n\n");
	printk("rxPciUnmap MAP");
	for(idx=0; idx<CORE_LOG_NUM; idx++){
		struct record_pci *record = &log->rxPciUnmap[idx];
		if(idx >= log->rxCnt_unmap)
			break;
		printk("[rxPciUnmap %03d]\n", idx);
		printk("phyAddrL=%p len=%d\n", record->phyAddrL, record->map_len);
	}

	printk("========= \n\n");
	printk("rxWd MAP");
	for(idx=0; idx<CORE_LOG_NUM; idx++){
		struct record_rxwd *record = &log->rxWd[idx];
		if(idx >= log->rxCnt_wd)
			break;
		printk("[rxWd %03d]\n", idx);
		{
			u8 *tmp = record->wd_buf;
			u32 len = record->wd_len;
			u32 idx1 = 0;
			if(tmp == NULL)
				break;
			for(idx1=0; idx1<len; idx1++){
				if(idx1%8==0) {
					printk("[%03d] %02x %02x %02x %02x %02x %02x %02x %02x \n", 
						idx1, tmp[idx1], tmp[idx1+1], tmp[idx1+2], tmp[idx1+3], 
						tmp[idx1+4], tmp[idx1+5], tmp[idx1+6], tmp[idx1+7]);
				}
			}
			printk("\n");
		}
		parse_wd_8852ae(adapter, RXWD, idx, record->wd_buf);
	}

	printk("========= \n\n");
	printk("rxAmpdu MAP");
	for(idx=0; idx<CORE_LOG_NUM; idx++){
		if(idx >= log->rxCnt_ampdu)
			break;
		printk("[rxAmpdu %03d] = %d\n", idx, log->rxAmpdu[idx]);
	}

}

void phl_dump_record(_adapter *adapter, u8 dump_type)
{
	struct phl_logs *log = &adapter->phl_logs;
	u32 idx = 0;

	printk("txBd: %d (%d) \n", log->txCnt_bd, log->txCnt_bd%CORE_LOG_NUM);
	printk("txWd: %d (%d) \n", log->txCnt_wd, log->txCnt_wd%CORE_LOG_NUM);
	printk("wpCnt_recycle: %d (%d) \n", log->txCnt_recycle, log->txCnt_recycle%CORE_LOG_NUM);

	printk("rxMap: %d (%d), Sz=%d \n", log->rxCnt_map, log->rxCnt_map%CORE_LOG_NUM, log->rxSize_map);
	printk("rxUnmap: %d (%d), Sz=%d \n", log->rxCnt_unmap, log->txCnt_wd%CORE_LOG_NUM, log->rxSize_map);
	printk("rxWd: %d (%d) \n", log->rxCnt_wd, log->rxCnt_wd%CORE_LOG_NUM);
	printk("rxCnt_ampdu: %d (%d) \n", log->rxCnt_ampdu, log->rxCnt_ampdu%CORE_LOG_NUM);

	if(dump_type == REC_DUMP_TX)
		phl_dump_map_tx(adapter);
	else if(dump_type == REC_DUMP_RX)
		phl_dump_map_rx(adapter);
	else if(dump_type == REC_DUMP_ALL){
		phl_dump_map_tx(adapter);
		phl_dump_map_rx(adapter);
	}
}

u32 tmp_rx_last_ppdu = 0;

void phl_add_record(void *d, u8 type, void *p, u32 num)
{
	struct dvobj_priv *pobj = (struct dvobj_priv *)d;
	_adapter *adapter = dvobj_get_primary_adapter(pobj);
	struct phl_logs *log = &adapter->phl_logs;

	if(!adapter->record_enable)
		return;

	if(type == REC_TXWD){
		u32 idx = log->txCnt_wd%CORE_LOG_NUM;
		struct record_txwd *record = &(log->txWd[idx]);
		
		log->txCnt_wd++;
		record->wd_len = num;
		memset((u8 *)record->wd_buf, 0, MAX_TXWD_SIZE);
		memcpy((u8 *)record->wd_buf, p, num);
	}

	if(type == REC_TXBD){
		u32 idx = log->txCnt_bd%CORE_LOG_NUM;
		struct record_txbd *record = &(log->txBd[idx]);
	
		log->txCnt_bd++;
		record->bd_len = num;
		memset((u8 *)record->bd_buf, 0, MAX_TXBD_SIZE);
		memcpy((u8 *)record->bd_buf, p, num);
	}

	if(type == REC_WP_RCC){
		u32 idx = log->txCnt_recycle%CORE_LOG_NUM;
		struct record_wp_rcc *record = &(log->wpRecycle[idx]);

		log->txCnt_recycle++;
		record->wp_seq = num;
	}

	if(type == REC_RX_MAP || type == REC_RX_UNMAP){
		struct record_pci *record = NULL;
		if(type == REC_RX_MAP) {
			u32 idx = log->rxCnt_map%CORE_LOG_NUM;
			record = &(log->rxPciMap[idx]);
			log->rxCnt_map++;
			log->rxSize_map+=num;
		}
		else if(type == REC_RX_UNMAP) {
			u32 idx = log->rxCnt_unmap%CORE_LOG_NUM;
			record = &(log->rxPciUnmap[idx]);
			log->rxCnt_unmap++;
			log->rxSize_map+=num;
		}
		record->phyAddrL = p;
		record->map_len = num;
	}

	if(type == REC_RXWD){
		u32 idx = log->rxCnt_wd%CORE_LOG_NUM;
		struct record_rxwd *record = &(log->rxWd[idx]);
	
		log->rxCnt_wd++;
		record->wd_len = num;
		memset((u8 *)record->wd_buf, 0, MAX_RXWD_SIZE);
		memcpy((u8 *)record->wd_buf, p, num);
	}

	if(type == REC_RX_AMPDU){
		u32 idx = 0; 

		if(log->rxCnt_ampdu == 0 && (log->rxAmpdu[0] == 0))
			tmp_rx_last_ppdu = num;

		if(tmp_rx_last_ppdu != num){
			tmp_rx_last_ppdu = num;

			log->rxCnt_ampdu ++;
			idx = log->rxCnt_ampdu%CORE_LOG_NUM;
			log->rxAmpdu[idx] = 1;
		}
		else{
			idx = log->rxCnt_ampdu%CORE_LOG_NUM;
			log->rxAmpdu[idx]++;
	}
}

}

void core_cmd_record_trx(_adapter *adapter, void *cmd_para, u32 para_num)
{
	u32 idx = 0;
	char *para = (char *)cmd_para;
	
	if(para_num<=0)
		return;
	
	if(!strcmp(para, "start")){
		u8 *log = NULL;
		log = (u8*)&adapter->core_logs;
		memset(log, 0, sizeof(struct core_logs));
		log = (u8*)&adapter->phl_logs;
		memset(log, 0, sizeof(struct phl_logs));
		adapter->record_enable = 1;
	}else if(!strcmp(para, "stop")){
		adapter->record_enable = 0;
	}else if(!strcmp(para, "dump")){
		u32 dump_type = 0;
		para=get_next_para_str(para);
		sscanf(para, "%d", &dump_type);
		phl_dump_record(adapter, (u8)dump_type);
	}
}

void reset_txforce_para(_adapter *adapter)
{
	adapter->txForce_rate 	= INV_TXFORCE_VAL;
	adapter->txForce_agg 	= INV_TXFORCE_VAL;
	adapter->txForce_aggnum = INV_TXFORCE_VAL;
	adapter->txForce_gi 	= INV_TXFORCE_VAL;
}

void core_cmd_txforce(_adapter *adapter, void *cmd_para, u32 para_num)
{
	u32 idx = 0;
	char *para = (char *)cmd_para;

	if(para_num<=0)
	return;

	if(!strcmp(para, "start")){
		adapter->txForce_enable = 1;
		reset_txforce_para(adapter);
	}else if(!strcmp(para, "stop")){
		adapter->txForce_enable = 0;
		reset_txforce_para(adapter);
	}else if(!strcmp(para, "rate")){
		u32 rate = 0;
		para=get_next_para_str(para);
		sscanf(para, "%x", &rate);
		adapter->txForce_rate = rate;
	}else if(!strcmp(para, "agg")){
		u32 agg = 0;
		para=get_next_para_str(para);
		sscanf(para, "%x", &agg);
		adapter->txForce_agg = agg;
	}else if(!strcmp(para, "aggnum")){
		u32 aggnum = 0;
		para=get_next_para_str(para);
		sscanf(para, "%d", &aggnum);
		adapter->txForce_aggnum = aggnum;
	}else if(!strcmp(para, "gi")){
		u32 gi = 0;
		para=get_next_para_str(para);
		sscanf(para, "%d", &gi);
		adapter->txForce_gi = gi;
	}else if(!strcmp(para, "macid")){

	}else if(!strcmp(para, "retry")){
	
	}
}

#ifdef CONFIG_RTW_CORE_RXSC
void core_cmd_rxsc(_adapter *adapter, void *cmd_para, u32 para_num)
{
	u32 idx = 0;
	char *para = (char *)cmd_para;

	printk("core_cmd_rxsc \n");

	if(para_num<=0)
		return;

	if(!strcmp(para, "enable")){
		printk("enable");
		adapter->enable_rxsc = 1;
	}else if(!strcmp(para, "disable")){
		printk("disable");
		adapter->enable_rxsc = 0;
	}else if(!strcmp(para, "dump")){
		struct core_logs *log = &adapter->core_logs;
		printk("dump");
		printk("enable_rxsc: %d \n", adapter->enable_rxsc);
		printk("rxCnt_data: orig=%d shortcut=%d(ratio=%d)\n",
			log->rxCnt_data_orig, log->rxCnt_data_shortcut,
			log->rxCnt_data_shortcut*100/(log->rxCnt_data_orig+log->rxCnt_data_shortcut));
	}

}
#endif

void core_sniffer_rx(_adapter *adapter, u8 *pkt, u32 pktlen)
{
	struct sk_buff* pskb = NULL;

	if(!adapter->sniffer_enable)
		return;
	
	if(pkt==NULL)
		return;

	pskb = dev_alloc_skb(pktlen+200);
	
	if(pskb == NULL){
	return;
}

	_rtw_memcpy(pskb->data, pkt, pktlen);
	pskb->len = pktlen;

	skb_reset_mac_header(pskb);
	pskb->dev = adapter->pnetdev;
	pskb->dev->type = ARPHRD_IEEE80211;
	pskb->ip_summed = CHECKSUM_UNNECESSARY;
	pskb->pkt_type = PACKET_OTHERHOST;
	pskb->protocol = htons(ETH_P_802_2);
	netif_receive_skb(pskb);

	return;
}
	
void core_cmd_sniffer(_adapter *adapter, void *cmd_para, u32 para_num)
	{
		u32 idx=0;
	char *para = (char *)cmd_para;
	
	if(para_num<=0)
	return;

	if(!strcmp(para, "start")){
		adapter->sniffer_enable = 1;
	}else if(!strcmp(para, "stop")){
		adapter->sniffer_enable = 0;
	}
}


#define LEN_TEST_BUF 2000
u8 test_buf[LEN_TEST_BUF];

#ifdef CONFIG_PCI_HCI
#include <rtw_trx_pci.h>
#endif

#include "../phl/phl_headers.h"
#include "../phl/phl_api.h"
#include "../phl/hal_g6/hal_api_mac.h"
#include "../phl/hal_g6/mac/mac_reg.h"


#define SHOW_REG32(adapter, reg) \
	do { \
		printk("\t%04X = %08X\n", \
			  (reg), rtw_phl_read32(adapter->dvobj->phl, reg)); \
	} while (0)

#define SHOW_REG32_MSG(adapter, reg, msg) \
	do { \
		printk("\t%04X = %08X - %s\n", \
			  (reg), rtw_phl_read32(adapter->dvobj->phl, reg), msg); \
	} while (0)

#define SHOW_REG16(adapter, reg) \
	do { \
		printk("\t%04X = %04X\n", \
			  (reg), rtw_phl_read16(adapter->dvobj->phl, reg)); \
	} while (0)

#define SHOW_REG16_MSG(adapter, reg, msg) \
	do { \
		printk("\t%04X = %04X - %s\n", \
			  (reg), rtw_phl_read16(adapter->dvobj->phl, reg), msg); \
	} while (0)


static inline void _show_RX_counter(_adapter *adapter)
{
	/* Show RX PPDU counters */
	int i;
	u32 reg32 = rtw_phl_read32(adapter->dvobj->phl, R_AX_RX_DBG_CNT_SEL);
	static const char *cnt_name[] = {"Invalid packet",
	                                 "RE-CCA",
	                                 "RX FIFO overflow",
	                                 "RX packet full drop",
	                                 "RX packet dma OK",
	                                 "UD 0",
	                                 "UD 1",
	                                 "UD 2",
	                                 "UD 3",
	                                 "continuous FCS error",
	                                 "RX packet filter drop",
	                                 "CSI packet DMA OK",
	                                 "CSI packet DMA drop",
	                                 "RX MAC stop"
	};

	printk("CMAC0 RX PPDU Counters @%04X:\n", R_AX_RX_DBG_CNT_SEL);
	reg32 &= ~(B_AX_RX_CNT_IDX_MSK << B_AX_RX_CNT_IDX_SH);
	for (i = 30; i < 44; i++) {
		rtw_phl_write32(adapter->dvobj->phl, R_AX_RX_DBG_CNT_SEL,
		            reg32 | (i << B_AX_RX_CNT_IDX_SH));
		printk("    %02X: %d - %s\n", i,
		         (
		            (   rtw_phl_read32(adapter->dvobj->phl, R_AX_RX_DBG_CNT_SEL)
		             >> B_AX_RX_DBG_CNT_SH)
		          & B_AX_RX_DBG_CNT_MSK),
		          cnt_name[i - 30]);
	}

	reg32 = rtw_phl_read32(adapter->dvobj->phl, R_AX_RX_DBG_CNT_SEL_C1);
	printk("CMAC1 RX PPDU Counters @%04X:\n", R_AX_RX_DBG_CNT_SEL_C1);
	reg32 &= ~(B_AX_RX_CNT_IDX_MSK << B_AX_RX_CNT_IDX_SH);
	for (i = 30; i < 44; i++) {
		rtw_phl_write32(adapter->dvobj->phl, R_AX_RX_DBG_CNT_SEL_C1,
		            reg32 | (i << B_AX_RX_CNT_IDX_SH));
		printk("    %02X: %d - %s\n", i,
		         (
		            (   rtw_phl_read32(adapter->dvobj->phl, R_AX_RX_DBG_CNT_SEL_C1)
		             >> B_AX_RX_DBG_CNT_SH)
		          & B_AX_RX_DBG_CNT_MSK),
		          cnt_name[i - 30]);
	}
} /* _show_RX_counter */

void _show_TX_dbg_status(_adapter *adapter)
{
	u32	reg32 = rtw_phl_read32(adapter->dvobj->phl, 0x9F1C);
	printk("TX Debug: 0x%08X\n", reg32);
}

void _show_BCN_dbg_status(_adapter *adapter)
{
	SHOW_REG32_MSG(adapter, R_AX_PORT_CFG_P0,		"PORT_CFG_P0");
	SHOW_REG32_MSG(adapter, R_AX_TBTT_PROHIB_P0,	"TBTT_PROHIB_P0");
	SHOW_REG32_MSG(adapter, R_AX_EN_HGQ_NOLIMIT,	"EN_HGQ_NOLIMIT");
	SHOW_REG32_MSG(adapter, R_AX_TBTT_AGG_P0,		"TBTT_AGG_P0");

	SHOW_REG32_MSG(adapter, R_AX_PORT_CFG_P0_C1,	"PORT_CFG_P0_C1");
	SHOW_REG32_MSG(adapter, R_AX_TBTT_PROHIB_P0_C1,	"TBTT_PROHIB_P0_C1");
	SHOW_REG32_MSG(adapter, R_AX_EN_HGQ_NOLIMIT_C1,	"EN_HGQ_NOLIMIT_C1");
	SHOW_REG32_MSG(adapter, R_AX_TBTT_AGG_P0_C1,	"TBTT_AGG_P0_C1");

	SHOW_REG32_MSG(adapter, R_AX_WCPU_FW_CTRL,		"R_AX_WCPU_FW_CTRL");
}


void core_cmd_dump_debug(_adapter *adapter, void *cmd_para, u32 para_num)
{
	printk("TX path registers: \n");

	SHOW_REG32_MSG(adapter, R_AX_RXQ_RXBD_IDX, "RX_BD_IDX");
	SHOW_REG32_MSG(adapter, R_AX_RPQ_RXBD_IDX, "RP_BD_IDX");
	SHOW_REG32_MSG(adapter, R_AX_ACH0_TXBD_IDX, "ACH0 IDX");
	SHOW_REG32_MSG(adapter, R_AX_ACH1_TXBD_IDX, "ACH1 IDX");
	SHOW_REG32_MSG(adapter, R_AX_ACH2_TXBD_IDX, "ACH2 IDX");
	SHOW_REG32_MSG(adapter, R_AX_ACH3_TXBD_IDX, "ACH3 IDX");
	SHOW_REG32_MSG(adapter, R_AX_ACH4_TXBD_IDX, "ACH4 IDX");
	SHOW_REG32_MSG(adapter, R_AX_ACH5_TXBD_IDX, "ACH5 IDX");
	SHOW_REG32_MSG(adapter, R_AX_ACH6_TXBD_IDX, "ACH6 IDX");
	SHOW_REG32_MSG(adapter, R_AX_ACH7_TXBD_IDX, "ACH7 IDX");
	SHOW_REG32_MSG(adapter, R_AX_CH8_TXBD_IDX, "CH8 IDX");
	SHOW_REG32_MSG(adapter, R_AX_CH9_TXBD_IDX, "CH9 IDX");
	SHOW_REG32_MSG(adapter, R_AX_CH10_TXBD_IDX, "CH10 IDX");
	SHOW_REG32_MSG(adapter, R_AX_CH11_TXBD_IDX, "CH11 IDX");
	SHOW_REG32_MSG(adapter, R_AX_CH12_TXBD_IDX, "CH12 IDX");
#ifdef R_AX_PCIE_DBG_CTRL
	SHOW_REG32_MSG(adapter, R_AX_PCIE_DBG_CTRL, "DBG_CTRL");
#else
	SHOW_REG32_MSG(adapter, 0x11C0, "DBG_CTRL");
#endif
	SHOW_REG32_MSG(adapter, R_AX_DBG_ERR_FLAG, "DBG_ERR");
	SHOW_REG32_MSG(adapter, R_AX_PCIE_HIMR00, "IMR0");
	SHOW_REG32_MSG(adapter, R_AX_PCIE_HISR00, "ISR0");
	SHOW_REG32_MSG(adapter, R_AX_PCIE_HIMR10, "IMR1");
	SHOW_REG32_MSG(adapter, R_AX_PCIE_HISR10, "IMR1");
	SHOW_REG16_MSG(adapter, R_AX_ACH0_BDRAM_RWPTR, "CH0");
	SHOW_REG16_MSG(adapter, R_AX_ACH1_BDRAM_RWPTR, "CH1");
	SHOW_REG16_MSG(adapter, R_AX_ACH2_BDRAM_RWPTR, "CH2");
	SHOW_REG16_MSG(adapter, R_AX_ACH3_BDRAM_RWPTR, "CH3");
	SHOW_REG16_MSG(adapter, R_AX_ACH4_BDRAM_RWPTR, "CH4");
	SHOW_REG16_MSG(adapter, R_AX_ACH5_BDRAM_RWPTR, "CH5");
	SHOW_REG16_MSG(adapter, R_AX_ACH6_BDRAM_RWPTR, "CH6");
	SHOW_REG16_MSG(adapter, R_AX_ACH7_BDRAM_RWPTR, "CH7");
	SHOW_REG16_MSG(adapter, R_AX_CH8_BDRAM_RWPTR, "CH8");
	SHOW_REG16_MSG(adapter, R_AX_CH9_BDRAM_RWPTR, "CH9");
	SHOW_REG16_MSG(adapter, R_AX_CH10_BDRAM_RWPTR, "CH10");
	SHOW_REG16_MSG(adapter, R_AX_CH11_BDRAM_RWPTR, "CH11");
	SHOW_REG16_MSG(adapter, R_AX_CH12_BDRAM_RWPTR, "CH12");
	SHOW_REG32_MSG(adapter, R_AX_PCIE_DMA_STOP1, "DMA_STOP1");
	SHOW_REG32_MSG(adapter, R_AX_PCIE_DMA_BUSY1, "DMA_BUSY1");
	SHOW_REG32_MSG(adapter, R_AX_PCIE_DMA_STOP2, "DMA_STOP2");
	SHOW_REG32_MSG(adapter, R_AX_PCIE_DMA_BUSY2, "DMA_BUSY2");
	SHOW_REG32(adapter, 0x8840);
	SHOW_REG32(adapter, 0x8844);
	SHOW_REG32(adapter, 0x8854);
	SHOW_REG16(adapter, 0xCA22);
	SHOW_REG32(adapter, 0x8AA8);

	/* Show TX PPDU counters */
	do {
		int i;
		u32 reg32 = rtw_phl_read32(adapter->dvobj->phl, R_AX_TX_PPDU_CNT);

		printk("CMAC0 TX PPDU Counters @%04X:\n", R_AX_TX_PPDU_CNT);

		reg32 &= ~(B_AX_PPDU_CNT_IDX_MSK << B_AX_PPDU_CNT_IDX_SH);
		for (i = 0; i < 11; i++) {
			rtw_phl_write32(adapter->dvobj->phl, R_AX_TX_PPDU_CNT,
			            reg32 | (i << B_AX_PPDU_CNT_IDX_SH));
			printk("    %02X: %d\n", i,
			         (
			            (   rtw_phl_read32(adapter->dvobj->phl, R_AX_TX_PPDU_CNT)
			             >> B_AX_TX_PPDU_CNT_SH)
			          & B_AX_TX_PPDU_CNT_MSK));
		}

		reg32 = rtw_phl_read32(adapter->dvobj->phl, R_AX_TX_PPDU_CNT_C1);

		printk("CMAC1 TX PPDU Counters @%04X:\n", R_AX_TX_PPDU_CNT_C1);

		reg32 &= ~(B_AX_PPDU_CNT_IDX_MSK << B_AX_PPDU_CNT_IDX_SH);
		for (i = 0; i < 11; i++) {
			rtw_phl_write32(adapter->dvobj->phl, R_AX_TX_PPDU_CNT_C1,
			            reg32 | (i << B_AX_PPDU_CNT_IDX_SH));
			printk("    %02X: %d\n", i,
			         (
			            (   rtw_phl_read32(adapter->dvobj->phl, R_AX_TX_PPDU_CNT_C1)
			             >> B_AX_TX_PPDU_CNT_SH)
			          & B_AX_TX_PPDU_CNT_MSK));
		}
	} while (0);

	/* Show RX PPDU counters */
	_show_RX_counter(adapter);

	_show_TX_dbg_status(adapter);

	_show_BCN_dbg_status(adapter);

}

void core_cmd_dump_reg(_adapter *adapter, void *cmd_para, u32 para_num)
{
	u32 *para = (u32 *)cmd_para;
	void *phl = adapter->dvobj->phl;
	u32 reg_start, reg_end;
	u32 idx = 0;

	reg_start = para[0];
	reg_end = reg_start + para[1];

	while(1) {
		if((reg_start>=reg_end) || (reg_start >= 0xffff))
			break;
		
		printk("[%04x] %08x %08x %08x %08x \n", 
			reg_start, 
			rtw_phl_read32(phl, reg_start), rtw_phl_read32(phl, reg_start+4),
			rtw_phl_read32(phl, reg_start+8), rtw_phl_read32(phl, reg_start+12));

		reg_start+=16;
	}
}

enum _CORE_CMD_PARA_TYPE {
	CMD_PARA_DEC = 0,
	CMD_PARA_HEX,
	CMD_PARA_STR,
};

struct test_cmd_list {
	const char *name;
	void (*fun)(_adapter *, void *, u32);
	enum _CORE_CMD_PARA_TYPE para_type;
};

void test_dump_dec(_adapter *adapter, void *cmd_para, u32 para_num)
{
	u32 idx = 0;
	u32 *para = (u32 *)cmd_para;
	DBGP("para_num=%d\n", para_num);

	for(idx=0; idx<para_num; idx++)
		DBGP("para[%d]=%d\n", para_num, para[idx]);
	}

void test_dump_hex(_adapter *adapter, void *cmd_para, u32 para_num)
{
	u32 idx = 0;
	u32 *para = (u32 *)cmd_para;
	DBGP("para_num=%d\n", para_num);

	for(idx=0; idx<para_num; idx++)
		DBGP("para[%d]=0x%x\n", para_num, para[idx]);
}

void test_dump_str(_adapter *adapter, void *cmd_para, u32 para_num)
{
	u32 idx = 0;
	char *para = (char *)cmd_para;
	DBGP("para_num=%d\n", para_num);

	for(idx=0; idx<para_num; idx++, para+=MAX_PHL_CMD_STR_LEN)
		DBGP("para[%d]=%s\n", para_num, para);
}

void get_all_cmd_para_value(_adapter *adapter, char *buf, u32 len, u32 *para, u8 type, u32 *num)
{
	u8 *tmp = NULL;

	if(!buf || !len)
		return;

		DBGP("type=%d buf=%s para=%p num=%d\n", type, buf, para, *num);

	if(len > 0){
		tmp = strsep(&buf, ",");

		if(tmp){
			if(type == CMD_PARA_HEX)
				sscanf(tmp, "%x", para);
			else if(type == CMD_PARA_DEC)
				sscanf(tmp, "%d", para);
			para += 1;
			*num = *num+1;
		}
		else
			return;

		if(buf && (len>strlen(tmp)))
			get_all_cmd_para_value(adapter, buf, strlen(buf), para, type, num);
		else
			return;
	}
	}

void get_all_cmd_para_str(_adapter *adapter, char *buf, u32 len, char *para, u8 type, u32* num)
{
	u8 *tmp = NULL;

	if(!buf || !len)
		return;

	DBGP("type=%d buf=%s para=%p num=%d\n", type, buf, para, *num);

	if(len > 0){
		tmp = strsep(&buf, ",");

		if(tmp){
			strcpy(para, tmp);
			para += MAX_PHL_CMD_STR_LEN;
			*num = *num+1;
	}
		else
			return;

		if(buf && (len>strlen(tmp)))
			get_all_cmd_para_str(adapter, buf, strlen(buf), para, type, num);
		else
			return;
	}
	}

struct test_cmd_list core_test_cmd_list[] = {
	{"dec", test_dump_dec, CMD_PARA_DEC},
	{"hex", test_dump_hex, CMD_PARA_HEX},
	{"str", test_dump_str, CMD_PARA_STR},
	{"dump_reg", 		core_cmd_dump_reg, 		CMD_PARA_HEX},
	{"dump_debug", 		core_cmd_dump_debug, 	CMD_PARA_DEC},
	{"record", 			core_cmd_record_trx, 	CMD_PARA_STR},
	{"txforce",			core_cmd_txforce, 		CMD_PARA_STR},
	{"sniffer",			core_cmd_sniffer, 		CMD_PARA_STR},
#ifdef CONFIG_RTW_CORE_RXSC
	{"rxsc",	core_cmd_rxsc,		CMD_PARA_STR},
#endif
};

void core_cmd_phl_handler(_adapter *adapter, char *extra)
{
	u32 para[MAX_PHL_CMD_NUM]={0};
	char para_str[MAX_PHL_CMD_NUM][MAX_PHL_CMD_STR_LEN]={0};
	char *cmd_name, *cmd_para = NULL;
	struct test_cmd_list *cmd = &core_test_cmd_list;
	u32 array_size = ARRAY_SIZE(core_test_cmd_list);
	u32 i = 0;
	
	cmd_name = strsep(&extra, ",");

	if(!cmd_name){
		for(i = 0; i<array_size; i++, cmd++)
			printk(" - %s\n", cmd->name);
		return;
	}

	for(i = 0; i<array_size; i++, cmd++){
		if(!strcmp(cmd->name, cmd_name)){
			void *cmd_para = NULL;
			u32 cmd_para_num = 0;
			if(cmd->para_type == CMD_PARA_DEC || cmd->para_type == CMD_PARA_HEX){
				cmd_para = para;
				if(extra)
				get_all_cmd_para_value(adapter, extra, strlen(extra), para, cmd->para_type, &cmd_para_num);
		}
		else{
				cmd_para = para_str;
				if(extra)
				get_all_cmd_para_str(adapter, extra, strlen(extra), para_str, cmd->para_type, &cmd_para_num);
	}

			cmd->fun(adapter, cmd_para, cmd_para_num);
			break;
	}
	}
	
}

#endif

u8 rtw_test_h2c_cmd(_adapter *adapter, u8 *buf, u8 len)
{
	struct cmd_obj *pcmdobj;
	struct drvextra_cmd_parm *pdrvextra_cmd_parm;
	u8 *ph2c_content;
	struct cmd_priv *pcmdpriv = &adapter_to_dvobj(adapter)->cmdpriv;
	u8	res = _SUCCESS;

	pcmdobj = (struct cmd_obj *)rtw_zmalloc(sizeof(struct cmd_obj));
	if (pcmdobj == NULL) {
		res = _FAIL;
		goto exit;
	}
	pcmdobj->padapter = adapter;

	pdrvextra_cmd_parm = (struct drvextra_cmd_parm *)rtw_zmalloc(sizeof(struct drvextra_cmd_parm));
	if (pdrvextra_cmd_parm == NULL) {
		rtw_mfree((u8 *)pcmdobj, sizeof(struct cmd_obj));
		res = _FAIL;
		goto exit;
	}

	ph2c_content = rtw_zmalloc(len);
	if (ph2c_content == NULL) {
		rtw_mfree((u8 *)pcmdobj, sizeof(struct cmd_obj));
		rtw_mfree((u8 *)pdrvextra_cmd_parm, sizeof(struct drvextra_cmd_parm));
		res = _FAIL;
		goto exit;
	}

	pdrvextra_cmd_parm->ec_id = TEST_H2C_CID;
	pdrvextra_cmd_parm->type = 0;
	pdrvextra_cmd_parm->size = len;
	pdrvextra_cmd_parm->pbuf = ph2c_content;

	_rtw_memcpy(ph2c_content, buf, len);

	init_h2fwcmd_w_parm_no_rsp(pcmdobj, pdrvextra_cmd_parm, CMD_SET_DRV_EXTRA);

	res = rtw_enqueue_cmd(pcmdpriv, pcmdobj);

exit:
	return res;
}

#ifdef CONFIG_MP_INCLUDED
static s32 rtw_mp_cmd_hdl(_adapter *padapter, u8 mp_cmd_id)
{
	struct dvobj_priv *dvobj = adapter_to_dvobj(padapter);
	int ret = H2C_SUCCESS;
	uint status = _FALSE;
	struct mp_priv *pmppriv = &padapter->mppriv;
	struct rtw_test_module_info test_module_info;
	struct rtw_mp_test_rpt my_mp_test_rpt;

	if (mp_cmd_id == MP_START) {
		if (padapter->registrypriv.mp_mode == 0) {

			test_module_info.tm_mode = RTW_DRV_MODE_MP;
			test_module_info.tm_type = RTW_TEST_SUB_MODULE_MP;
			pmppriv->keep_ips_status = dvobj->phl_com->dev_sw_cap.ps_cap.ips_en;
			pmppriv->keep_lps_status = dvobj->phl_com->dev_sw_cap.ps_cap.lps_en;
			dvobj->phl_com->dev_sw_cap.ps_cap.ips_en = PS_OP_MODE_DISABLED;
			dvobj->phl_com->dev_sw_cap.ps_cap.lps_en = PS_OP_MODE_DISABLED;

			rtw_phl_test_submodule_init(dvobj->phl_com, &test_module_info);
#ifdef CONFIG_BTC
			if (dvobj->phl_com->dev_cap.btc_mode == BTC_MODE_NORMAL ||\
					dvobj->phl_com->dev_cap.btc_mode == BTC_MODE_BT) {

				pmppriv->mp_keep_btc_mode = dvobj->phl_com->dev_cap.btc_mode;
				rtw_mp_phl_config_arg(padapter, RTW_MP_CONFIG_CMD_SWITCH_BT_PATH);
				RTW_INFO("config BTC to WL Only\n");
			}
#endif
			rtw_mp_phl_config_arg(padapter, RTW_MP_CONFIG_CMD_SET_PHY_INDEX);
			status = rtw_mp_phl_config_arg(padapter, RTW_MP_CONFIG_CMD_START_DUT);

			if (status == _TRUE) {
				padapter->registrypriv.mp_mode = 1;
				MPT_InitializeAdapter(padapter, 1);
			}
		}

		if (padapter->registrypriv.mp_mode == 0) {
			ret = H2C_REJECTED;
			goto exit;
		}

		if (padapter->mppriv.mode == MP_OFF) {
			if (mp_start_test(padapter) == _FAIL) {
				ret = H2C_REJECTED;
				goto exit;
			}
			padapter->mppriv.mode = MP_ON;
		}
		padapter->mppriv.bmac_filter = _FALSE;

	} else if (mp_cmd_id == MP_STOP) {
		if (padapter->registrypriv.mp_mode == 1) {

			status = rtw_mp_phl_config_arg(padapter, RTW_MP_CONFIG_CMD_STOP_DUT);
			RTW_INFO("RTW_MP_CONFIG_CMD_STOP_DUT %s\n", status == _TRUE ? "ok" : "fail");
			rtw_phl_test_get_rpt(dvobj->phl_com, (u8*)&my_mp_test_rpt, sizeof(my_mp_test_rpt));

			if (my_mp_test_rpt.status == _TRUE)
				RTW_INFO("TM Sub finished OK!!!\n");
#ifdef CONFIG_BTC
			if (pmppriv->mp_keep_btc_mode != BTC_MODE_MAX) {
				pmppriv->btc_path = pmppriv->mp_keep_btc_mode;
				rtw_mp_phl_config_arg(padapter, RTW_MP_CONFIG_CMD_SWITCH_BT_PATH);
			}
#endif
			test_module_info.tm_mode = RTW_DRV_MODE_NORMAL;
			test_module_info.tm_type = RTW_TEST_SUB_MODULE_MP;
			dvobj->phl_com->dev_sw_cap.ps_cap.ips_en = pmppriv->keep_ips_status;
			dvobj->phl_com->dev_sw_cap.ps_cap.lps_en = pmppriv->keep_lps_status;

			rtw_phl_test_submodule_deinit(dvobj->phl_com, &test_module_info);

			MPT_DeInitAdapter(padapter);
			padapter->registrypriv.mp_mode = 0;
		}

		if (padapter->mppriv.mode != MP_OFF) {
			mp_stop_test(padapter);
			padapter->mppriv.mode = MP_OFF;
		}

	} else {
		RTW_INFO(FUNC_ADPT_FMT"invalid id:%d\n", FUNC_ADPT_ARG(padapter), mp_cmd_id);
		ret = H2C_PARAMETERS_ERROR;
		rtw_warn_on(1);
	}

exit:
	return ret;
}

u8 rtw_mp_cmd(_adapter *adapter, u8 mp_cmd_id, u8 flags)
{
	struct cmd_obj *cmdobj;
	struct drvextra_cmd_parm *parm;
	struct cmd_priv *pcmdpriv = &adapter_to_dvobj(adapter)->cmdpriv;
	struct submit_ctx sctx;
	u8	res = _SUCCESS;

	parm = (struct drvextra_cmd_parm *)rtw_zmalloc(sizeof(struct drvextra_cmd_parm));
	if (parm == NULL) {
		res = _FAIL;
		goto exit;
	}

	parm->ec_id = MP_CMD_WK_CID;
	parm->type = mp_cmd_id;
	parm->size = 0;
	parm->pbuf = NULL;

	if (flags & RTW_CMDF_DIRECTLY) {
		/* no need to enqueue, do the cmd hdl directly and free cmd parameter */
		if (H2C_SUCCESS != rtw_mp_cmd_hdl(adapter, mp_cmd_id))
			res = _FAIL;
		rtw_mfree((u8 *)parm, sizeof(*parm));
	} else {
		/* need enqueue, prepare cmd_obj and enqueue */
		cmdobj = (struct cmd_obj *)rtw_zmalloc(sizeof(*cmdobj));
		if (cmdobj == NULL) {
			res = _FAIL;
			rtw_mfree((u8 *)parm, sizeof(*parm));
			goto exit;
		}
		cmdobj->padapter = adapter;

		init_h2fwcmd_w_parm_no_rsp(cmdobj, parm, CMD_SET_DRV_EXTRA);

		if (flags & RTW_CMDF_WAIT_ACK) {
			cmdobj->sctx = &sctx;
			rtw_sctx_init(&sctx, 10 * 1000);
		}

		res = rtw_enqueue_cmd(pcmdpriv, cmdobj);

		if (res == _SUCCESS && (flags & RTW_CMDF_WAIT_ACK)) {
			rtw_sctx_wait(&sctx, __func__);
			_rtw_mutex_lock_interruptible(&pcmdpriv->sctx_mutex);
			if (sctx.status == RTW_SCTX_SUBMITTED)
				cmdobj->sctx = NULL;
			_rtw_mutex_unlock(&pcmdpriv->sctx_mutex);
			if (sctx.status != RTW_SCTX_DONE_SUCCESS)
				res = _FAIL;
		}
	}

exit:
	return res;
}
#endif	/*CONFIG_MP_INCLUDED*/

#ifdef CONFIG_RTW_CUSTOMER_STR
static s32 rtw_customer_str_cmd_hdl(_adapter *adapter, u8 write, const u8 *cstr)
{
	int ret = H2C_SUCCESS;

	if (write)
		ret = rtw_hal_h2c_customer_str_write(adapter, cstr);
	else
		ret = rtw_hal_h2c_customer_str_req(adapter);

	return ret == _SUCCESS ? H2C_SUCCESS : H2C_REJECTED;
}

static u8 rtw_customer_str_cmd(_adapter *adapter, u8 write, const u8 *cstr)
{
	struct cmd_obj *cmdobj;
	struct drvextra_cmd_parm *parm;
	u8 *str = NULL;
	struct cmd_priv *pcmdpriv = &adapter_to_dvobj(adapter)->cmdpriv;
	struct submit_ctx sctx;
	u8 res = _SUCCESS;

	parm = (struct drvextra_cmd_parm *)rtw_zmalloc(sizeof(struct drvextra_cmd_parm));
	if (parm == NULL) {
		res = _FAIL;
		goto exit;
	}

	if (write) {
		str = rtw_zmalloc(RTW_CUSTOMER_STR_LEN);
		if (str == NULL) {
			rtw_mfree((u8 *)parm, sizeof(struct drvextra_cmd_parm));
			res = _FAIL;
			goto exit;
		}
	}

	parm->ec_id = CUSTOMER_STR_WK_CID;
	parm->type = write;
	parm->size = write ? RTW_CUSTOMER_STR_LEN : 0;
	parm->pbuf = write ? str : NULL;

	if (write)
		_rtw_memcpy(str, cstr, RTW_CUSTOMER_STR_LEN);

	/* need enqueue, prepare cmd_obj and enqueue */
	cmdobj = (struct cmd_obj *)rtw_zmalloc(sizeof(*cmdobj));
	if (cmdobj == NULL) {
		res = _FAIL;
		rtw_mfree((u8 *)parm, sizeof(*parm));
		if (write)
			rtw_mfree(str, RTW_CUSTOMER_STR_LEN);
		goto exit;
	}
	cmdobj->padapter = adapter;

	init_h2fwcmd_w_parm_no_rsp(cmdobj, parm, CMD_SET_DRV_EXTRA);

	cmdobj->sctx = &sctx;
	rtw_sctx_init(&sctx, 2 * 1000);

	res = rtw_enqueue_cmd(pcmdpriv, cmdobj);

	if (res == _SUCCESS) {
		rtw_sctx_wait(&sctx, __func__);
		_rtw_mutex_lock_interruptible(&pcmdpriv->sctx_mutex);
		if (sctx.status == RTW_SCTX_SUBMITTED)
			cmdobj->sctx = NULL;
		_rtw_mutex_unlock(&pcmdpriv->sctx_mutex);
		if (sctx.status != RTW_SCTX_DONE_SUCCESS)
			res = _FAIL;
	}

exit:
	return res;
}

inline u8 rtw_customer_str_req_cmd(_adapter *adapter)
{
	return rtw_customer_str_cmd(adapter, 0, NULL);
}

inline u8 rtw_customer_str_write_cmd(_adapter *adapter, const u8 *cstr)
{
	return rtw_customer_str_cmd(adapter, 1, cstr);
}
#endif /* CONFIG_RTW_CUSTOMER_STR */

u8 rtw_c2h_wk_cmd(_adapter *padapter, u8 *pbuf, u16 length, u8 type)
{
	struct cmd_obj *cmd;
	struct drvextra_cmd_parm *pdrvextra_cmd_parm;
	struct cmd_priv *pcmdpriv = &adapter_to_dvobj(padapter)->cmdpriv;
	u8 *extra_cmd_buf;
	u8 res = _SUCCESS;

	cmd = (struct cmd_obj *)rtw_zmalloc(sizeof(struct cmd_obj));
	if (cmd == NULL) {
		res = _FAIL;
		goto exit;
	}
	cmd->padapter = padapter;

	pdrvextra_cmd_parm = (struct drvextra_cmd_parm *)rtw_zmalloc(sizeof(struct drvextra_cmd_parm));
	if (pdrvextra_cmd_parm == NULL) {
		rtw_mfree((u8 *)cmd, sizeof(struct cmd_obj));
		res = _FAIL;
		goto exit;
	}

	extra_cmd_buf = rtw_zmalloc(length);
	if (extra_cmd_buf == NULL) {
		rtw_mfree((u8 *)cmd, sizeof(struct cmd_obj));
		rtw_mfree((u8 *)pdrvextra_cmd_parm, sizeof(struct drvextra_cmd_parm));
		res = _FAIL;
		goto exit;
	}

	_rtw_memcpy(extra_cmd_buf, pbuf, length);
	pdrvextra_cmd_parm->ec_id = C2H_WK_CID;
	pdrvextra_cmd_parm->type = type;
	pdrvextra_cmd_parm->size = length;
	pdrvextra_cmd_parm->pbuf = extra_cmd_buf;

	init_h2fwcmd_w_parm_no_rsp(cmd, pdrvextra_cmd_parm, CMD_SET_DRV_EXTRA);

	res = rtw_enqueue_cmd(pcmdpriv, cmd);

exit:
	return res;
}

#define C2H_TYPE_PKT 1
inline u8 rtw_c2h_packet_wk_cmd(_adapter *adapter, u8 *c2h_evt, u16 length)
{
	return rtw_c2h_wk_cmd(adapter, c2h_evt, length, C2H_TYPE_PKT);
}

static u8 _rtw_run_in_thread_cmd(_adapter *adapter, void (*func)(void *), void *context, s32 timeout_ms)
{
	struct cmd_priv *cmdpriv = &adapter_to_dvobj(adapter)->cmdpriv;
	struct cmd_obj *cmdobj;
	struct RunInThread_param *parm;
	struct submit_ctx sctx;
	s32 res = _SUCCESS;

	cmdobj = (struct cmd_obj *)rtw_zmalloc(sizeof(struct cmd_obj));
	if (NULL == cmdobj) {
		res = _FAIL;
		goto exit;
	}
	cmdobj->padapter = adapter;

	parm = (struct RunInThread_param *)rtw_zmalloc(sizeof(struct RunInThread_param));
	if (NULL == parm) {
		rtw_mfree((u8 *)cmdobj, sizeof(struct cmd_obj));
		res = _FAIL;
		goto exit;
	}

	parm->func = func;
	parm->context = context;
	init_h2fwcmd_w_parm_no_rsp(cmdobj, parm, CMD_RUN_INTHREAD);

	if (timeout_ms >= 0) {
		cmdobj->sctx = &sctx;
		rtw_sctx_init(&sctx, timeout_ms);
	}

	res = rtw_enqueue_cmd(cmdpriv, cmdobj);

	if (res == _SUCCESS && timeout_ms >= 0) {
		rtw_sctx_wait(&sctx, __func__);
		_rtw_mutex_lock_interruptible(&cmdpriv->sctx_mutex);
		if (sctx.status == RTW_SCTX_SUBMITTED)
			cmdobj->sctx = NULL;
		_rtw_mutex_unlock(&cmdpriv->sctx_mutex);
		if (sctx.status != RTW_SCTX_DONE_SUCCESS)
			res = _FAIL;
	}

exit:
	return res;
}
u8 rtw_run_in_thread_cmd(_adapter *adapter, void (*func)(void *), void *context)
{
	return _rtw_run_in_thread_cmd(adapter, func, context, -1);
}

u8 rtw_run_in_thread_cmd_wait(_adapter *adapter, void (*func)(void *), void *context, s32 timeout_ms)
{
	return _rtw_run_in_thread_cmd(adapter, func, context, timeout_ms);
}


u8 session_tracker_cmd(_adapter *adapter, u8 cmd, struct sta_info *sta, u8 *local_naddr, u8 *local_port, u8 *remote_naddr, u8 *remote_port)
{
	struct cmd_priv	*cmdpriv = &adapter_to_dvobj(adapter)->cmdpriv;
	struct cmd_obj *cmdobj;
	struct drvextra_cmd_parm *cmd_parm;
	struct st_cmd_parm *st_parm;
	u8	res = _SUCCESS;

	cmdobj = (struct cmd_obj *)rtw_zmalloc(sizeof(struct cmd_obj));
	if (cmdobj == NULL) {
		res = _FAIL;
		goto exit;
	}
	cmdobj->padapter = adapter;

	cmd_parm = (struct drvextra_cmd_parm *)rtw_zmalloc(sizeof(struct drvextra_cmd_parm));
	if (cmd_parm == NULL) {
		rtw_mfree((u8 *)cmdobj, sizeof(struct cmd_obj));
		res = _FAIL;
		goto exit;
	}

	st_parm = (struct st_cmd_parm *)rtw_zmalloc(sizeof(struct st_cmd_parm));
	if (st_parm == NULL) {
		rtw_mfree((u8 *)cmdobj, sizeof(struct cmd_obj));
		rtw_mfree((u8 *)cmd_parm, sizeof(struct drvextra_cmd_parm));
		res = _FAIL;
		goto exit;
	}

	st_parm->cmd = cmd;
	st_parm->sta = sta;
	if (cmd != ST_CMD_CHK) {
		_rtw_memcpy(&st_parm->local_naddr, local_naddr, 4);
		_rtw_memcpy(&st_parm->local_port, local_port, 2);
		_rtw_memcpy(&st_parm->remote_naddr, remote_naddr, 4);
		_rtw_memcpy(&st_parm->remote_port, remote_port, 2);
	}

	cmd_parm->ec_id = SESSION_TRACKER_WK_CID;
	cmd_parm->type = 0;
	cmd_parm->size = sizeof(struct st_cmd_parm);
	cmd_parm->pbuf = (u8 *)st_parm;
	init_h2fwcmd_w_parm_no_rsp(cmdobj, cmd_parm, CMD_SET_DRV_EXTRA);
	cmdobj->no_io = 1;

	res = rtw_enqueue_cmd(cmdpriv, cmdobj);

exit:
	return res;
}

inline u8 session_tracker_chk_cmd(_adapter *adapter, struct sta_info *sta)
{
	return session_tracker_cmd(adapter, ST_CMD_CHK, sta, NULL, NULL, NULL, NULL);
}

inline u8 session_tracker_add_cmd(_adapter *adapter, struct sta_info *sta, u8 *local_naddr, u8 *local_port, u8 *remote_naddr, u8 *remote_port)
{
	return session_tracker_cmd(adapter, ST_CMD_ADD, sta, local_naddr, local_port, remote_naddr, remote_port);
}

inline u8 session_tracker_del_cmd(_adapter *adapter, struct sta_info *sta, u8 *local_naddr, u8 *local_port, u8 *remote_naddr, u8 *remote_port)
{
	return session_tracker_cmd(adapter, ST_CMD_DEL, sta, local_naddr, local_port, remote_naddr, remote_port);
}

void session_tracker_chk_for_sta(_adapter *adapter, struct sta_info *sta)
{
	struct st_ctl_t *st_ctl = &sta->st_ctl;
	int i;
	_list *plist, *phead, *pnext;
	_list dlist;
	struct session_tracker *st = NULL;
	u8 op_wfd_mode = MIRACAST_DISABLED;

	if (DBG_SESSION_TRACKER)
		RTW_INFO(FUNC_ADPT_FMT" sta:%p\n", FUNC_ADPT_ARG(adapter), sta);

	if (!(sta->state & WIFI_ASOC_STATE))
		goto exit;

	for (i = 0; i < SESSION_TRACKER_REG_ID_NUM; i++) {
		if (st_ctl->reg[i].s_proto != 0)
			break;
	}
	if (i >= SESSION_TRACKER_REG_ID_NUM)
		goto chk_sta;

	_rtw_init_listhead(&dlist);

	_rtw_spinlock_bh(&st_ctl->tracker_q.lock);

	phead = &st_ctl->tracker_q.queue;
	plist = get_next(phead);
	pnext = get_next(plist);
	while (rtw_end_of_queue_search(phead, plist) == _FALSE) {
		st = LIST_CONTAINOR(plist, struct session_tracker, list);
		plist = pnext;
		pnext = get_next(pnext);

		if (st->status != ST_STATUS_ESTABLISH
			&& rtw_get_passing_time_ms(st->set_time) > ST_EXPIRE_MS
		) {
			rtw_list_delete(&st->list);
			rtw_list_insert_tail(&st->list, &dlist);
		}

		/* TODO: check OS for status update */
		if (st->status == ST_STATUS_CHECK)
			st->status = ST_STATUS_ESTABLISH;

		if (st->status != ST_STATUS_ESTABLISH)
			continue;

		#ifdef CONFIG_WFD
		if (0)
			RTW_INFO(FUNC_ADPT_FMT" local:%u, remote:%u, rtsp:%u, %u, %u\n", FUNC_ADPT_ARG(adapter)
				, ntohs(st->local_port), ntohs(st->remote_port), adapter->wfd_info.rtsp_ctrlport, adapter->wfd_info.tdls_rtsp_ctrlport
				, adapter->wfd_info.peer_rtsp_ctrlport);
		if (ntohs(st->local_port) == adapter->wfd_info.rtsp_ctrlport)
			op_wfd_mode |= MIRACAST_SINK;
		if (ntohs(st->local_port) == adapter->wfd_info.tdls_rtsp_ctrlport)
			op_wfd_mode |= MIRACAST_SINK;
		if (ntohs(st->remote_port) == adapter->wfd_info.peer_rtsp_ctrlport)
			op_wfd_mode |= MIRACAST_SOURCE;
		#endif
	}

	_rtw_spinunlock_bh(&st_ctl->tracker_q.lock);

	plist = get_next(&dlist);
	while (rtw_end_of_queue_search(&dlist, plist) == _FALSE) {
		st = LIST_CONTAINOR(plist, struct session_tracker, list);
		plist = get_next(plist);
		rtw_mfree((u8 *)st, sizeof(struct session_tracker));
	}

chk_sta:
	if (STA_OP_WFD_MODE(sta) != op_wfd_mode) {
		STA_SET_OP_WFD_MODE(sta, op_wfd_mode);
		rtw_sta_media_status_rpt_cmd(adapter, sta, 1);
	}

exit:
	return;
}

void session_tracker_chk_for_adapter(_adapter *adapter)
{
	struct sta_priv *stapriv = &adapter->stapriv;
	struct sta_info *sta;
	int i;
	_list *plist, *phead;
	u8 op_wfd_mode = MIRACAST_DISABLED;

	_rtw_spinlock_bh(&stapriv->sta_hash_lock);

	for (i = 0; i < NUM_STA; i++) {
		phead = &(stapriv->sta_hash[i]);
		plist = get_next(phead);

		while ((rtw_end_of_queue_search(phead, plist)) == _FALSE) {
			sta = LIST_CONTAINOR(plist, struct sta_info, hash_list);
			plist = get_next(plist);

			session_tracker_chk_for_sta(adapter, sta);

			op_wfd_mode |= STA_OP_WFD_MODE(sta);
		}
	}

	_rtw_spinunlock_bh(&stapriv->sta_hash_lock);

#ifdef CONFIG_WFD
	adapter->wfd_info.op_wfd_mode = MIRACAST_MODE_REVERSE(op_wfd_mode);
#endif
}

void session_tracker_cmd_hdl(_adapter *adapter, struct st_cmd_parm *parm)
{
	u8 cmd = parm->cmd;
	struct sta_info *sta = parm->sta;

	if (cmd == ST_CMD_CHK) {
		if (sta)
			session_tracker_chk_for_sta(adapter, sta);
		else
			session_tracker_chk_for_adapter(adapter);

		goto exit;

	} else if (cmd == ST_CMD_ADD || cmd == ST_CMD_DEL) {
		struct st_ctl_t *st_ctl;
		u32 local_naddr = parm->local_naddr;
		u16 local_port = parm->local_port;
		u32 remote_naddr = parm->remote_naddr;
		u16 remote_port = parm->remote_port;
		struct session_tracker *st = NULL;
		_list *plist, *phead;
		u8 free_st = 0;
		u8 alloc_st = 0;

		if (DBG_SESSION_TRACKER)
			RTW_INFO(FUNC_ADPT_FMT" cmd:%u, sta:%p, local:"IP_FMT":"PORT_FMT", remote:"IP_FMT":"PORT_FMT"\n"
				, FUNC_ADPT_ARG(adapter), cmd, sta
				, IP_ARG(&local_naddr), PORT_ARG(&local_port)
				, IP_ARG(&remote_naddr), PORT_ARG(&remote_port)
			);

		if (!(sta->state & WIFI_ASOC_STATE))
			goto exit;

		st_ctl = &sta->st_ctl;

		_rtw_spinlock_bh(&st_ctl->tracker_q.lock);

		phead = &st_ctl->tracker_q.queue;
		plist = get_next(phead);
		while (rtw_end_of_queue_search(phead, plist) == _FALSE) {
			st = LIST_CONTAINOR(plist, struct session_tracker, list);

			if (st->local_naddr == local_naddr
				&& st->local_port == local_port
				&& st->remote_naddr == remote_naddr
				&& st->remote_port == remote_port)
				break;

			plist = get_next(plist);
		}

		if (rtw_end_of_queue_search(phead, plist) == _TRUE)
			st = NULL;

		switch (cmd) {
		case ST_CMD_DEL:
			if (st) {
				rtw_list_delete(plist);
				free_st = 1;
			}
			goto unlock;
		case ST_CMD_ADD:
			if (!st)
				alloc_st = 1;
		}

unlock:
		_rtw_spinunlock_bh(&st_ctl->tracker_q.lock);

		if (free_st) {
			rtw_mfree((u8 *)st, sizeof(struct session_tracker));
			goto exit;
		}

		if (alloc_st) {
			st = (struct session_tracker *)rtw_zmalloc(sizeof(struct session_tracker));
			if (!st)
				goto exit;

			st->local_naddr = local_naddr;
			st->local_port = local_port;
			st->remote_naddr = remote_naddr;
			st->remote_port = remote_port;
			st->set_time = rtw_get_current_time();
			st->status = ST_STATUS_CHECK;

			_rtw_spinlock_bh(&st_ctl->tracker_q.lock);
			rtw_list_insert_tail(&st->list, phead);
			_rtw_spinunlock_bh(&st_ctl->tracker_q.lock);
		}
	}

exit:
	return;
}

#if defined(CONFIG_RTW_MESH) && defined(RTW_PER_CMD_SUPPORT_FW)
static s32 rtw_req_per_cmd_hdl(_adapter *adapter)
{
	struct dvobj_priv *dvobj = adapter_to_dvobj(adapter);
	struct macid_ctl_t *macid_ctl = dvobj_to_macidctl(dvobj);
	struct macid_bmp req_macid_bmp, *macid_bmp;
	u8 i, ret = _FAIL;

	macid_bmp = &macid_ctl->if_g[adapter->iface_id];
	_rtw_memcpy(&req_macid_bmp, macid_bmp, sizeof(struct macid_bmp));

	/* Clear none mesh's macid */
	for (i = 0; i < macid_ctl->num; i++) {
		u8 role;
		role = GET_H2CCMD_MSRRPT_PARM_ROLE(&macid_ctl->h2c_msr[i]);
		if (role != H2C_MSR_ROLE_MESH)
			rtw_macid_map_clr(&req_macid_bmp, i);
	}

	/* group_macid: always be 0 in NIC, so only pass macid_bitmap.m0
	 * rpt_type: 0 includes all info in 1, use 0 for now 
	 * macid_bitmap: pass m0 only for NIC
	 */
	ret = rtw_hal_set_req_per_rpt_cmd(adapter, 0, 0, req_macid_bmp.m0);

	return ret;
}

u8 rtw_req_per_cmd(_adapter *adapter)
{
	struct cmd_obj *cmdobj;
	struct drvextra_cmd_parm *parm;
	struct cmd_priv *pcmdpriv = &adapter_to_dvobj(adapter)->cmdpriv;
	struct submit_ctx sctx;
	u8 res = _SUCCESS;

	parm = (struct drvextra_cmd_parm *)rtw_zmalloc(sizeof(struct drvextra_cmd_parm));
	if (parm == NULL) {
		res = _FAIL;
		goto exit;
	}

	parm->ec_id = REQ_PER_CMD_WK_CID;
	parm->type = 0;
	parm->size = 0;
	parm->pbuf = NULL;

	cmdobj = (struct cmd_obj *)rtw_zmalloc(sizeof(*cmdobj));
	if (cmdobj == NULL) {
		res = _FAIL;
		rtw_mfree((u8 *)parm, sizeof(*parm));
		goto exit;
	}
	cmdobj->padapter = adapter;

	init_h2fwcmd_w_parm_no_rsp(cmdobj, parm, CMD_SET_DRV_EXTRA);

	res = rtw_enqueue_cmd(pcmdpriv, cmdobj);

exit:
	return res;
}
#endif


void rtw_ac_parm_cmd_hdl(_adapter *padapter, struct _ADAPTER_LINK *padapter_link, u8 *_ac_parm_buf, int ac_type)
{

	u32 ac_parm_buf;

	_rtw_memcpy(&ac_parm_buf, _ac_parm_buf, sizeof(ac_parm_buf));
	switch (ac_type) {
	case XMIT_VO_QUEUE:
		RTW_INFO(FUNC_NDEV_FMT" AC_VO = 0x%08x\n", FUNC_ADPT_ARG(padapter), (unsigned int) ac_parm_buf);
		rtw_ap_set_edca(padapter, padapter_link, 3, ac_parm_buf);
		break;

	case XMIT_VI_QUEUE:
		RTW_INFO(FUNC_NDEV_FMT" AC_VI = 0x%08x\n", FUNC_ADPT_ARG(padapter), (unsigned int) ac_parm_buf);
		rtw_ap_set_edca(padapter, padapter_link, 2, ac_parm_buf);
		break;

	case XMIT_BE_QUEUE:
		RTW_INFO(FUNC_NDEV_FMT" AC_BE = 0x%08x\n", FUNC_ADPT_ARG(padapter), (unsigned int) ac_parm_buf);
		rtw_ap_set_edca(padapter, padapter_link, 0, ac_parm_buf);
		break;

	case XMIT_BK_QUEUE:
		RTW_INFO(FUNC_NDEV_FMT" AC_BK = 0x%08x\n", FUNC_ADPT_ARG(padapter), (unsigned int) ac_parm_buf);
		rtw_ap_set_edca(padapter, padapter_link, 1, ac_parm_buf);
		break;

	default:
		break;
	}

}

u8 rtw_drvextra_cmd_hdl(_adapter *padapter, unsigned char *pbuf)
{
	int ret = H2C_SUCCESS;
	struct drvextra_cmd_parm *pdrvextra_cmd;

	if (!pbuf)
		return H2C_PARAMETERS_ERROR;

	pdrvextra_cmd = (struct drvextra_cmd_parm *)pbuf;

	switch (pdrvextra_cmd->ec_id) {
	case STA_MSTATUS_RPT_WK_CID:
		rtw_sta_media_status_rpt_cmd_hdl(padapter, (struct sta_media_status_rpt_cmd_parm *)pdrvextra_cmd->pbuf);
		break;
	#if 0 /*#ifdef CONFIG_CORE_DM_CHK_TIMER*/
	case DYNAMIC_CHK_WK_CID:/*only  primary padapter go to this cmd, but execute dynamic_chk_wk_hdl() for two interfaces */
		rtw_dynamic_chk_wk_hdl(padapter);
		break;
	#endif
#ifdef CONFIG_ANTENNA_DIVERSITY
	case ANT_SELECT_WK_CID:
		antenna_select_wk_hdl(padapter, pdrvextra_cmd->type);
		break;
#endif
#ifdef CONFIG_P2P_PS
	case P2P_PS_WK_CID:
		p2p_ps_wk_hdl(padapter, pdrvextra_cmd->type);
		break;
#endif
#ifdef CONFIG_AP_MODE
	case CHECK_HIQ_WK_CID:
		rtw_chk_hi_queue_hdl(padapter);
		break;
#endif
	/* add for CONFIG_IEEE80211W, none 11w can use it */
	case RESET_SECURITYPRIV:
		reset_securitypriv_hdl(padapter);
		break;
	case FREE_ASSOC_RESOURCES:
		free_assoc_resources_hdl(padapter, (u8)pdrvextra_cmd->type);
		break;
	case C2H_WK_CID:
		switch (pdrvextra_cmd->type) {
		case C2H_TYPE_PKT:
			rtw_hal_c2h_pkt_hdl(padapter, pdrvextra_cmd->pbuf, pdrvextra_cmd->size);
			break;
		default:
			RTW_ERR("unknown C2H type:%d\n", pdrvextra_cmd->type);
			rtw_warn_on(1);
			break;
		}
		break;

#ifdef CONFIG_DFS_MASTER
	case DFS_RADAR_DETECT_WK_CID:
		rtw_dfs_rd_hdl(adapter_to_dvobj(padapter), (enum phl_band_idx)pdrvextra_cmd->type, 0, CHANNEL_WIDTH_MAX);
		break;
	case DFS_RADAR_DETECT_EN_DEC_WK_CID:
		rtw_dfs_rd_en_dec_update(adapter_to_dvobj(padapter), (enum phl_band_idx)pdrvextra_cmd->type);
		break;
#endif
	case SESSION_TRACKER_WK_CID:
		session_tracker_cmd_hdl(padapter, (struct st_cmd_parm *)pdrvextra_cmd->pbuf);
		break;
	case TEST_H2C_CID:
		rtw_hal_fill_h2c_cmd(padapter, pdrvextra_cmd->pbuf[0], pdrvextra_cmd->size - 1, &pdrvextra_cmd->pbuf[1]);
		break;
	case MP_CMD_WK_CID:
#ifdef CONFIG_MP_INCLUDED
		ret = rtw_mp_cmd_hdl(padapter, pdrvextra_cmd->type);
#endif
		break;
#ifdef CONFIG_RTW_CUSTOMER_STR
	case CUSTOMER_STR_WK_CID:
		ret = rtw_customer_str_cmd_hdl(padapter, pdrvextra_cmd->type, pdrvextra_cmd->pbuf);
		break;
#endif

#ifdef CONFIG_IOCTL_CFG80211
	case MGNT_TX_WK_CID:
		ret = rtw_mgnt_tx_handler(padapter, pdrvextra_cmd->pbuf);
		break;
#endif /* CONFIG_IOCTL_CFG80211 */
#if defined(CONFIG_RTW_MESH) && defined(RTW_PER_CMD_SUPPORT_FW)
	case REQ_PER_CMD_WK_CID:
		ret = rtw_req_per_cmd_hdl(padapter);
		break;
#endif
#ifdef CONFIG_SUPPORT_STATIC_SMPS
	case SSMPS_WK_CID :
		rtw_ssmps_wk_hdl(padapter, (struct ssmps_cmd_parm *)pdrvextra_cmd->pbuf);
		break;
#endif
#ifdef CONFIG_CTRL_TXSS_BY_TP
	case TXSS_WK_CID :
		rtw_ctrl_txss_wk_hdl(padapter, (struct txss_cmd_parm *)pdrvextra_cmd->pbuf);
		break;
#endif
	case AC_PARM_CMD_WK_CID:
		rtw_ac_parm_cmd_hdl(padapter, pdrvextra_cmd->adapter_link, pdrvextra_cmd->pbuf, pdrvextra_cmd->type);
		break;
#ifdef CONFIG_AP_MODE
	case STOP_AP_WK_CID:
		stop_ap_hdl(padapter);
		break;
#endif
#ifdef CONFIG_RTW_TOKEN_BASED_XMIT
	case TBTX_CONTROL_TX_WK_CID:
		tx_control_hdl(padapter);
		break;
#endif
#ifdef PRIVATE_R
	case FIND_REMOTE_WK_CID:
		ret = issue_action_find_remote(padapter);
		break;
#ifdef CONFIG_P2P
	case HIDE_SSID_WK_CID:
		issue_beacon(padapter, 0);
		break;
#endif
#endif
	default:
		break;
	}

	if (pdrvextra_cmd->pbuf && pdrvextra_cmd->size > 0)
		rtw_mfree(pdrvextra_cmd->pbuf, pdrvextra_cmd->size);

	return ret;
}


void rtw_disassoc_cmd_callback(_adapter *padapter,  struct cmd_obj *pcmd)
{
	struct	mlme_priv *pmlmepriv = &padapter->mlmepriv;


	if (pcmd->res != H2C_SUCCESS) {
		_rtw_spinlock_bh(&pmlmepriv->lock);
		set_fwstate(pmlmepriv, WIFI_ASOC_STATE);
		_rtw_spinunlock_bh(&pmlmepriv->lock);
		goto exit;
	}
#ifdef CONFIG_BR_EXT
	else /* clear bridge database */
		nat25_db_cleanup(padapter);
#endif /* CONFIG_BR_EXT */

	/* free cmd */
	rtw_free_cmd_obj(pcmd);

exit:
	return;
}

void rtw_create_ibss_post_hdl(_adapter *padapter, int status)
{
	struct wlan_network *pwlan = NULL;
	struct	mlme_priv *pmlmepriv = &padapter->mlmepriv;
	WLAN_BSSID_EX *pdev_network = &padapter->registrypriv.dev_network;
	struct wlan_network *mlme_cur_network = &(pmlmepriv->dev_cur_network);

	if (status != H2C_SUCCESS)
		set_assoc_timer(pmlmepriv, 1); /*_set_timer(&pmlmepriv->assoc_timer, 1);*/

	/*_cancel_timer_ex(&pmlmepriv->assoc_timer);*/
	cancel_assoc_timer(pmlmepriv);

	_rtw_spinlock_bh(&pmlmepriv->lock);

	{
		pwlan = _rtw_alloc_network(pmlmepriv);
		_rtw_spinlock_bh(&(pmlmepriv->scanned_queue.lock));
		if (pwlan == NULL) {
			pwlan = rtw_get_oldest_wlan_network(&pmlmepriv->scanned_queue);
			if (pwlan == NULL) {
				_rtw_spinunlock_bh(&(pmlmepriv->scanned_queue.lock));
				goto createbss_cmd_fail;
			}
			pwlan->last_scanned = rtw_get_current_time();
		} else
			rtw_list_insert_tail(&(pwlan->list), &pmlmepriv->scanned_queue.queue);

		pdev_network->Length = get_WLAN_BSSID_EX_sz(pdev_network);
		_rtw_memcpy(&(pwlan->network), pdev_network, pdev_network->Length);
		/* pwlan->fixed = _TRUE; */

		/* copy pdev_network information to pmlmepriv->cur_network */
		_rtw_memcpy(&mlme_cur_network->network, pdev_network, (get_WLAN_BSSID_EX_sz(pdev_network)));

#if 0
		/* reset DSConfig */
		mlme_cur_network->network.Configuration.DSConfig = (u32)rtw_ch2freq(pdev_network->Configuration.DSConfig);
#endif

		_clr_fwstate_(pmlmepriv, WIFI_UNDER_LINKING);
		_rtw_spinunlock_bh(&(pmlmepriv->scanned_queue.lock));
		/* we will set WIFI_ASOC_STATE when there is one more sat to join us (rtw_stassoc_event_callback) */
	}

createbss_cmd_fail:
	_rtw_spinunlock_bh(&pmlmepriv->lock);
	return;
}



void rtw_setstaKey_cmdrsp_callback(_adapter *padapter ,  struct cmd_obj *pcmd)
{

	struct sta_priv *pstapriv = &padapter->stapriv;
	struct set_stakey_rsp *psetstakey_rsp = (struct set_stakey_rsp *)(pcmd->rsp);
	struct sta_info	*psta = rtw_get_stainfo(pstapriv, psetstakey_rsp->addr);


	if (psta == NULL) {
		goto exit;
	}

	/* psta->phl_sta->aid = psta->phl_sta->macid = psetstakey_rsp->keyid; */ /* CAM_ID(CAM_ENTRY) */

exit:

	rtw_free_cmd_obj(pcmd);


}

void rtw_getrttbl_cmd_cmdrsp_callback(_adapter *padapter,  struct cmd_obj *pcmd)
{

	rtw_free_cmd_obj(pcmd);
#ifdef CONFIG_MP_INCLUDED
	if (padapter->registrypriv.mp_mode == 1)
		padapter->mppriv.workparam.bcompleted = _TRUE;
#endif


}

u8 set_txq_params_cmd(_adapter *adapter, struct _ADAPTER_LINK *padapter_link, u32 ac_parm, u8 ac_type)
{
	struct cmd_obj *cmdobj;
	struct drvextra_cmd_parm *pdrvextra_cmd_parm;
	struct cmd_priv *pcmdpriv = &adapter_to_dvobj(adapter)->cmdpriv;
	u8 *ac_parm_buf = NULL;
	u8 sz;
	u8 res = _SUCCESS;


	cmdobj = (struct cmd_obj *)rtw_zmalloc(sizeof(struct cmd_obj));
	if (cmdobj == NULL) {
		res = _FAIL;
		goto exit;
	}
	cmdobj->padapter = adapter;

	pdrvextra_cmd_parm = (struct drvextra_cmd_parm *)rtw_zmalloc(sizeof(struct drvextra_cmd_parm));
	if (pdrvextra_cmd_parm == NULL) {
		rtw_mfree((u8 *)cmdobj, sizeof(struct cmd_obj));
		res = _FAIL;
		goto exit;
	}

	sz = sizeof(ac_parm);
	ac_parm_buf = rtw_zmalloc(sz);
	if (ac_parm_buf == NULL) {
		rtw_mfree((u8 *)cmdobj, sizeof(struct cmd_obj));
		rtw_mfree((u8 *)pdrvextra_cmd_parm, sizeof(struct drvextra_cmd_parm));
		res = _FAIL;
		goto exit;
	}

	pdrvextra_cmd_parm->adapter_link = padapter_link;
	pdrvextra_cmd_parm->ec_id = AC_PARM_CMD_WK_CID;
	pdrvextra_cmd_parm->type = ac_type;
	pdrvextra_cmd_parm->size = sz;
	pdrvextra_cmd_parm->pbuf = ac_parm_buf;

	_rtw_memcpy(ac_parm_buf, &ac_parm, sz);

	init_h2fwcmd_w_parm_no_rsp(cmdobj, pdrvextra_cmd_parm, CMD_SET_DRV_EXTRA);
	res = rtw_enqueue_cmd(pcmdpriv, cmdobj);

exit:
	return res;
}

char UNKNOWN_CID[16] = "UNKNOWN_EXTRA";
char *rtw_extra_name(struct drvextra_cmd_parm *pdrvextra_cmd)
{
	switch(pdrvextra_cmd->ec_id) {
	case NONE_WK_CID:
		return "NONE_WK_CID";
		break;
	case STA_MSTATUS_RPT_WK_CID:
		return "STA_MSTATUS_RPT_WK_CID";
		break;
	#if 0 /*#ifdef CONFIG_CORE_DM_CHK_TIMER*/
	case DYNAMIC_CHK_WK_CID:
		return "DYNAMIC_CHK_WK_CID";
		break;
	#endif
	case DM_CTRL_WK_CID:
		return "DM_CTRL_WK_CID";
		break;
	case PBC_POLLING_WK_CID:
		return "PBC_POLLING_WK_CID";
		break;
	case LPS_CTRL_WK_CID:
		return "LPS_CTRL_WK_CID";
		break;
	case ANT_SELECT_WK_CID:
		return "ANT_SELECT_WK_CID";
		break;
	case P2P_PS_WK_CID:
		return "P2P_PS_WK_CID";
		break;
	case CHECK_HIQ_WK_CID:
		return "CHECK_HIQ_WK_CID";
		break;
	case C2H_WK_CID:
		return "C2H_WK_CID";
		break;
	case RESET_SECURITYPRIV:
		return "RESET_SECURITYPRIV";
		break;
	case FREE_ASSOC_RESOURCES:
		return "FREE_ASSOC_RESOURCES";
		break;
	case DM_IN_LPS_WK_CID:
		return "DM_IN_LPS_WK_CID";
		break;
	case LPS_CHANGE_DTIM_CID:
		return "LPS_CHANGE_DTIM_CID";
		break;
	case DFS_RADAR_DETECT_WK_CID:
		return "DFS_RADAR_DETECT_WK_CID";
		break;
	case DFS_RADAR_DETECT_EN_DEC_WK_CID:
		return "DFS_RADAR_DETECT_EN_DEC_WK_CID";
		break;
	case SESSION_TRACKER_WK_CID:
		return "SESSION_TRACKER_WK_CID";
		break;
	case TEST_H2C_CID:
		return "TEST_H2C_CID";
		break;
	case MP_CMD_WK_CID:
		return "MP_CMD_WK_CID";
		break;
	case CUSTOMER_STR_WK_CID:
		return "CUSTOMER_STR_WK_CID";
		break;
	case MGNT_TX_WK_CID:
		return "MGNT_TX_WK_CID";
		break;
	case REQ_PER_CMD_WK_CID:
		return "REQ_PER_CMD_WK_CID";
		break;
	case SSMPS_WK_CID:
		return "SSMPS_WK_CID";
		break;
#ifdef CONFIG_CTRL_TXSS_BY_TP
	case TXSS_WK_CID:
		return "TXSS_WK_CID";
		break;
#endif
	case AC_PARM_CMD_WK_CID:
		return "AC_PARM_CMD_WK_CID";
		break;
#ifdef CONFIG_AP_MODE
	case STOP_AP_WK_CID:
		return "STOP_AP_WK_CID";
		break;
#endif
#ifdef CONFIG_RTW_TOKEN_BASED_XMIT
	case TBTX_CONTROL_TX_WK_CID:
		return "TBTX_CONTROL_TX_WK_CID";
		break;
#endif
#ifdef PRIVATE_R
	case FIND_REMOTE_WK_CID:
		return "FIND_REMOTE_WK_CID";
		break;
#ifdef CONFIG_P2P
	case HIDE_SSID_WK_CID:
		return "HIDE_SSID_WK_CID";
		break;
#endif
#endif
	case MAX_WK_CID:
		return "MAX_WK_CID";
		break;
	default:
		return UNKNOWN_CID;
		break;
	}
	return UNKNOWN_CID;
}

char UNKNOWN_CMD[16] = "UNKNOWN_CMD";
char *rtw_cmd_name(struct cmd_obj *pcmd)
{
	struct rtw_evt_header *pev;

	if (pcmd->cmdcode >= (sizeof(wlancmds) / sizeof(struct rtw_cmd)))
		return UNKNOWN_CMD;

	if (pcmd->cmdcode == CMD_SET_MLME_EVT)
		return rtw_evt_name((struct rtw_evt_header*)pcmd->parmbuf);

	if (pcmd->cmdcode == CMD_SET_DRV_EXTRA)
		return rtw_extra_name((struct drvextra_cmd_parm*)pcmd->parmbuf);

	return wlancmds[pcmd->cmdcode].name;
}

#ifdef PRIVATE_R
u8 rtw_find_remote_wk_cmd(_adapter *adapter)
{
	struct cmd_obj		*cmdobj;
	struct drvextra_cmd_parm  *pdrvextra_cmd_parm;
	struct cmd_priv	*pcmdpriv = &adapter_to_dvobj(adapter)->cmdpriv;
	u8	res = _SUCCESS;

	cmdobj = (struct cmd_obj *)rtw_zmalloc(sizeof(struct cmd_obj));
	if (cmdobj == NULL) {
		res = _FAIL;
		goto exit;
	}

	cmdobj->padapter = adapter;

	pdrvextra_cmd_parm = (struct drvextra_cmd_parm *)rtw_zmalloc(sizeof(struct drvextra_cmd_parm));
	if (pdrvextra_cmd_parm == NULL) {
		rtw_mfree((unsigned char *)cmdobj, sizeof(struct cmd_obj));
		res = _FAIL;
		goto exit;
	}

	pdrvextra_cmd_parm->ec_id = FIND_REMOTE_WK_CID;
	pdrvextra_cmd_parm->type = 0;
	pdrvextra_cmd_parm->size = 0;
	pdrvextra_cmd_parm->pbuf = NULL;
	init_h2fwcmd_w_parm_no_rsp(cmdobj, pdrvextra_cmd_parm, CMD_SET_DRV_EXTRA);

	res = rtw_enqueue_cmd(pcmdpriv, cmdobj);

exit:
	return res;
}

#ifdef CONFIG_P2P
u8 rtw_hide_ssid_wk_cmd(_adapter *adapter)
{
	struct cmd_obj		*cmdobj;
	struct drvextra_cmd_parm  *pdrvextra_cmd_parm;
	struct cmd_priv	*pcmdpriv = &adapter_to_dvobj(adapter)->cmdpriv;
	u8	res = _SUCCESS;

	cmdobj = (struct cmd_obj *)rtw_zmalloc(sizeof(struct cmd_obj));
	if (cmdobj == NULL) {
		res = _FAIL;
		goto exit;
	}

	cmdobj->padapter = adapter;

	pdrvextra_cmd_parm = (struct drvextra_cmd_parm *)rtw_zmalloc(sizeof(struct drvextra_cmd_parm));
	if (pdrvextra_cmd_parm == NULL) {
		rtw_mfree((unsigned char *)cmdobj, sizeof(struct cmd_obj));
		res = _FAIL;
		goto exit;
	}

	pdrvextra_cmd_parm->ec_id = HIDE_SSID_WK_CID;
	pdrvextra_cmd_parm->type = 0;
	pdrvextra_cmd_parm->size = 0;
	pdrvextra_cmd_parm->pbuf = NULL;
	init_h2fwcmd_w_parm_no_rsp(cmdobj, pdrvextra_cmd_parm, CMD_SET_DRV_EXTRA);

	res = rtw_enqueue_cmd(pcmdpriv, cmdobj);

exit:
	return res;

}
#endif
#endif

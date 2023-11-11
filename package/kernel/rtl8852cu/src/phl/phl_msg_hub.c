/******************************************************************************
 *
 * Copyright(c) 2019 - 2020 Realtek Corporation.
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
 * Author: vincent_fann@realtek.com
 *
 *****************************************************************************/
#include "phl_headers.h"
#define MODL_MASK_LEN (PHL_BK_MDL_END / 8)
#define MAX_MSG_NUM	(16)

enum msg_hub_status {
	MSG_HUB_INIT = BIT0,
	MSG_HUB_STARTED = BIT1,
};

enum msg_recver_status {
	MSG_RECVER_INIT = BIT0,
	MSG_RECVER_CLR_CTX = BIT1,
};

struct phl_msg_receiver_ex {
	u8 status;
	u8 bitmap[ MODL_MASK_LEN ];
	struct phl_msg_receiver ctx;
};


struct phl_msg_ex {
	_os_list list;
	struct phl_msg ctx;
	struct msg_completion_routine completion;
};
/**
 * phl_msg_hub - responsible for phl msg forwarding,
 * @status: contain mgnt status flags, refer to enum msg_hub_status
 * @msg_pool: msg extension pool
 * @msg_notify_thread: thread fot forwarding msg
 * @recver: msg receiver, refer to enum phl_msg_recver_layer
 */
struct phl_msg_hub {
	u32 status;
		struct phl_msg_ex msg_pool[MAX_MSG_NUM];
		struct phl_queue  idle_msg_q;
		struct phl_queue  wait_msg_q;
		_os_sema msg_q_sema;
		_os_thread msg_notify_thread;
		/* for core & phl layer respectively */
		struct phl_msg_receiver_ex recver[MSG_RECV_MAX];
};

inline static u8 _is_bitmap_empty(void* d, u8* bitmap){
	u8 empty[MODL_MASK_LEN] = {0};
	return (!_os_mem_cmp(d, bitmap, empty, MODL_MASK_LEN))?(true):(false);
}

static u8 pop_front_idle_msg(struct phl_info_t* phl, struct phl_msg_ex** msg)
{
	void *d = phl_to_drvpriv(phl);
	struct phl_msg_hub* hub = (struct phl_msg_hub*)phl->msg_hub;
	_os_list* new_msg = NULL;

	(*msg) = NULL;
	if(pq_pop(d, &hub->idle_msg_q, &new_msg, _first, _bh)) {
		(*msg) = (struct phl_msg_ex*)new_msg;
		_os_mem_set(d, &((*msg)->ctx), 0, sizeof(struct phl_msg));
		(*msg)->completion.completion = NULL;
		(*msg)->completion.priv = NULL;
		return true;
	}
	else
		return false;
}

static void push_back_idle_msg(struct phl_info_t* phl, struct phl_msg_ex* ex)
{
	void *d = phl_to_drvpriv(phl);
	struct phl_msg_hub* hub = (struct phl_msg_hub*)phl->msg_hub;

	if(ex->completion.completion)
		ex->completion.completion(ex->completion.priv, &(ex->ctx));
	pq_push(d, &hub->idle_msg_q, &ex->list, _tail, _bh);
}

static u8 pop_front_wait_msg(struct phl_info_t* phl, struct phl_msg_ex** msg)
{
	void *d = phl_to_drvpriv(phl);
	struct phl_msg_hub* hub = (struct phl_msg_hub*)phl->msg_hub;
	_os_list* new_msg = NULL;

	(*msg) = NULL;
	if(pq_pop(d, &hub->wait_msg_q, &new_msg, _first, _bh)) {
		(*msg) = (struct phl_msg_ex*)new_msg;
		return true;
	}
	else
		return false;
}

static void push_back_wait_msg(struct phl_info_t* phl, struct phl_msg_ex* ex)
{
	void *d = phl_to_drvpriv(phl);
	struct phl_msg_hub* hub = (struct phl_msg_hub*)phl->msg_hub;
	pq_push(d, &hub->wait_msg_q, &ex->list, _tail, _bh);
	_os_sema_up(d, &(hub->msg_q_sema));
}

void msg_forward(struct phl_info_t* phl, struct phl_msg_ex* ex)
{
	void *d = phl_to_drvpriv(phl);
	u8 i = 0;
	struct phl_msg_hub* hub = (struct phl_msg_hub*)phl->msg_hub;
	struct phl_msg_receiver_ex* recver = NULL;
	u8 module_id = MSG_MDL_ID_FIELD(ex->ctx.msg_id);

	if (!TEST_STATUS_FLAG(hub->status, MSG_HUB_STARTED)) {
		PHL_INFO("%s, msg hub not working\n",__FUNCTION__);
		return;
	}
	for(i = 0; i < MSG_RECV_MAX; i++) {
		recver = &(hub->recver[i]);
		if(!TEST_STATUS_FLAG(recver->status, MSG_RECVER_INIT)) {
			if(TEST_STATUS_FLAG(recver->status, MSG_RECVER_CLR_CTX))
				_os_mem_set(d, recver, 0, sizeof(struct phl_msg_receiver_ex));
			continue;
		}
		if(_chk_bitmap_bit(recver->bitmap, MODL_MASK_LEN, module_id)) {
			PHL_TRACE(COMP_PHL_DBG, _PHL_DEBUG_, "%s notify %d layer\n",
				  __FUNCTION__, i);
			recver->ctx.incoming_evt_notify(recver->ctx.priv,
							&(ex->ctx));
			PHL_TRACE(COMP_PHL_DBG, _PHL_DEBUG_, "%s notify %d layer\n",
				  __FUNCTION__, i);
		}
	}

}

static int msg_hub_thread_hdl(void* param)
{
	struct phl_info_t* phl = (struct phl_info_t *)param;
	void *d = phl_to_drvpriv(phl);
	struct phl_msg_hub* hub = (struct phl_msg_hub*)phl->msg_hub;
	struct phl_msg_ex* ex = NULL;

	PHL_INFO("%s enter\n",__FUNCTION__);
	while(!_os_thread_check_stop(d, &(hub->msg_notify_thread))) {

		_os_sema_down(d, &hub->msg_q_sema);

		if(_os_thread_check_stop(d, &(hub->msg_notify_thread)))
			break;

		while(pop_front_wait_msg(phl, &ex)){
			msg_forward(phl, ex);
			push_back_idle_msg(phl, ex);
		}
	}
	while (hub->idle_msg_q.cnt != MAX_MSG_NUM) {
		while(pop_front_wait_msg(phl, &ex))
			push_back_idle_msg(phl, ex);
	}
	_os_thread_wait_stop(d, &(hub->msg_notify_thread));
	PHL_INFO("%s down\n",__FUNCTION__);
	return 0;
}


enum rtw_phl_status phl_msg_hub_init(struct phl_info_t* phl)
{
	struct phl_msg_hub* hub = NULL;
	void *d = phl_to_drvpriv(phl);


	if(phl->msg_hub != NULL)
		return RTW_PHL_STATUS_FAILURE;

	hub = (struct phl_msg_hub *)_os_mem_alloc(d,
					sizeof(struct phl_msg_hub));
	if(hub == NULL) {
		PHL_ERR("%s, alloc fail\n",__FUNCTION__);
		return RTW_PHL_STATUS_RESOURCE;
	}
	phl->msg_hub = hub;
	_os_sema_init(d, &(hub->msg_q_sema), 0);
	pq_init(d, &(hub->idle_msg_q));
	pq_init(d, &(hub->wait_msg_q));
	SET_STATUS_FLAG(hub->status, MSG_HUB_INIT);
	PHL_INFO("%s\n",__FUNCTION__);
	return RTW_PHL_STATUS_SUCCESS;
}

enum rtw_phl_status phl_msg_hub_deinit(struct phl_info_t* phl)
{
	struct phl_msg_hub* hub = (struct phl_msg_hub*)phl->msg_hub;
	void *d = phl_to_drvpriv(phl);

	if(!TEST_STATUS_FLAG(hub->status, MSG_HUB_INIT))
		return RTW_PHL_STATUS_FAILURE;
	CLEAR_STATUS_FLAG(hub->status, MSG_HUB_INIT);
	phl_msg_hub_stop(phl);
	pq_deinit(d, &(hub->idle_msg_q));
	pq_deinit(d, &(hub->wait_msg_q));
	_os_sema_free(d, &(hub->msg_q_sema));
	_os_mem_free(d, hub, sizeof(struct phl_msg_hub));
	PHL_INFO("%s\n",__FUNCTION__);
	return RTW_PHL_STATUS_SUCCESS;
}

enum rtw_phl_status phl_msg_hub_start(struct phl_info_t* phl)
{
	u8 i = 0;
	struct phl_msg_hub* hub = (struct phl_msg_hub*)phl->msg_hub;
	void *d = phl_to_drvpriv(phl);

	if(!TEST_STATUS_FLAG(hub->status, MSG_HUB_INIT)||
		TEST_STATUS_FLAG(hub->status, MSG_HUB_STARTED))
		return RTW_PHL_STATUS_FAILURE;

	_os_mem_set(d, hub->msg_pool, 0,
			sizeof(struct phl_msg_ex) * MAX_MSG_NUM );
	pq_reset(d, &(hub->idle_msg_q), _bh);
	pq_reset(d, &(hub->wait_msg_q), _bh);
	for(i = 0; i < MAX_MSG_NUM; i++) {
		pq_push(d, &hub->idle_msg_q, &hub->msg_pool[i].list, _tail, _bh);
	}
	_os_thread_init(d, &(hub->msg_notify_thread), msg_hub_thread_hdl, phl,
						"msg_notify_thread");
	_os_thread_schedule(d, &(hub->msg_notify_thread));
	SET_STATUS_FLAG(hub->status, MSG_HUB_STARTED);
	PHL_INFO("%s\n",__FUNCTION__);
	return RTW_PHL_STATUS_SUCCESS;
}

enum rtw_phl_status phl_msg_hub_stop(struct phl_info_t* phl)
{
	struct phl_msg_hub* hub = (struct phl_msg_hub*)phl->msg_hub;
	void *d = phl_to_drvpriv(phl);

	if(!TEST_STATUS_FLAG(hub->status, MSG_HUB_STARTED))
		return RTW_PHL_STATUS_FAILURE;

	CLEAR_STATUS_FLAG(hub->status, MSG_HUB_STARTED);
	_os_thread_stop(d, &(hub->msg_notify_thread));
	_os_sema_up(d, &(hub->msg_q_sema));
	_os_thread_deinit(d, &(hub->msg_notify_thread));
	pq_reset(d, &(hub->idle_msg_q), _bh);
	pq_reset(d, &(hub->wait_msg_q), _bh);

	PHL_INFO("%s\n",__FUNCTION__);
	return RTW_PHL_STATUS_SUCCESS;
}

enum rtw_phl_status phl_msg_hub_send(struct phl_info_t* phl,
						struct phl_msg_attribute* attr,	struct phl_msg* msg)
{
	struct phl_msg_hub* hub = (struct phl_msg_hub*)phl->msg_hub;
	void *d = phl_to_drvpriv(phl);
	struct phl_msg_ex* ex = NULL;

	if(!TEST_STATUS_FLAG(hub->status, MSG_HUB_STARTED) || msg == NULL)
		return RTW_PHL_STATUS_FAILURE;

	if(!pop_front_idle_msg(phl, &ex)) {
		PHL_ERR(" %s idle msg empty\n",__FUNCTION__);
		return RTW_PHL_STATUS_RESOURCE;
	}
	_os_mem_cpy(d, &(ex->ctx), msg, sizeof(struct phl_msg));
	if(attr && attr->completion.completion) {
		ex->completion.completion = attr->completion.completion;
		ex->completion.priv = attr->completion.priv;
	}

	push_back_wait_msg(phl, ex);

	PHL_TRACE(COMP_PHL_DBG, _PHL_DEBUG_, "%s, msg_id:0x%x enqueue\n",
		  __FUNCTION__, msg->msg_id);
	return RTW_PHL_STATUS_SUCCESS;
}


enum rtw_phl_status phl_msg_hub_register_recver(void* phl,
		struct phl_msg_receiver* ctx, enum phl_msg_recver_layer layer)
{
	struct phl_info_t* phl_info = (struct phl_info_t*)phl;
	struct phl_msg_hub* hub = (struct phl_msg_hub*)phl_info->msg_hub;
	void *d = phl_to_drvpriv(phl_info);
	struct phl_msg_receiver_ex* recver = NULL;

	if(!TEST_STATUS_FLAG(hub->status, MSG_HUB_INIT) ||
		layer >= MSG_RECV_MAX ||
		ctx == NULL)
		return RTW_PHL_STATUS_FAILURE;

	recver =  &(hub->recver[layer]);
	if(TEST_STATUS_FLAG(recver->status, MSG_RECVER_INIT)) {
		PHL_ERR("%s, layer registered\n",__FUNCTION__);
		return RTW_PHL_STATUS_FAILURE;
	}

	_os_mem_cpy(d, &(recver->ctx), ctx, sizeof(struct phl_msg_receiver));
	_os_mem_set(d, &(recver->bitmap), 0, MODL_MASK_LEN);
	SET_STATUS_FLAG(recver->status, MSG_RECVER_INIT);
	PHL_INFO("%s\n",__FUNCTION__);
	return RTW_PHL_STATUS_SUCCESS;
}

enum rtw_phl_status phl_msg_hub_update_recver_mask(void* phl,
		enum phl_msg_recver_layer layer, u8* mdl_id, u8 len, u8 clr)
{
	struct phl_info_t* phl_info = (struct phl_info_t*)phl;
	struct phl_msg_hub* hub = (struct phl_msg_hub*)phl_info->msg_hub;
	struct phl_msg_receiver_ex* recver = NULL;
	if(!TEST_STATUS_FLAG(hub->status, MSG_HUB_INIT) ||
		layer >= MSG_RECV_MAX)
		return RTW_PHL_STATUS_FAILURE;

	recver =  &(hub->recver[layer]);
	if(!TEST_STATUS_FLAG(recver->status, MSG_RECVER_INIT)) {
		PHL_ERR("%s, layer not registered\n",__FUNCTION__);
		return RTW_PHL_STATUS_FAILURE;
	}

	if(clr == true)
		_clr_bitmap_bit(recver->bitmap, MODL_MASK_LEN, mdl_id, len);
	else
		_add_bitmap_bit(recver->bitmap, MODL_MASK_LEN, mdl_id, len);
	PHL_INFO(" %s\n",__FUNCTION__);
	return RTW_PHL_STATUS_SUCCESS;
}

enum rtw_phl_status phl_msg_hub_deregister_recver(void* phl,
					enum phl_msg_recver_layer layer)
{
	struct phl_info_t* phl_info = (struct phl_info_t*)phl;
	struct phl_msg_hub* hub = (struct phl_msg_hub*)phl_info->msg_hub;
	struct phl_msg_receiver_ex* recver = NULL;
	if(!TEST_STATUS_FLAG(hub->status, MSG_HUB_INIT) ||
		layer >= MSG_RECV_MAX)
		return RTW_PHL_STATUS_FAILURE;

	recver =  &(hub->recver[layer]);
	if(!TEST_STATUS_FLAG(recver->status, MSG_RECVER_INIT)) {
		PHL_ERR("%s, layer not registered\n",__FUNCTION__);
		return RTW_PHL_STATUS_FAILURE;
	}
	CLEAR_STATUS_FLAG(recver->status, MSG_RECVER_INIT);
	SET_STATUS_FLAG(recver->status, MSG_RECVER_CLR_CTX);
	PHL_INFO("%s\n",__FUNCTION__);
	return RTW_PHL_STATUS_SUCCESS;
}

/* handling msg hub event with PHL_MDL_PHY_MGNT as module id */
void phl_msg_hub_phy_mgnt_evt_hdlr(struct phl_info_t* phl, u16 evt_id)
{
	PHL_INFO("%s : evt_id %d.\n", __func__, evt_id);

	switch (evt_id) {
	case MSG_EVT_FWDL_OK:
		break;
	case MSG_EVT_FWDL_FAIL:
		break;
	case MSG_EVT_DUMP_PLE_BUFFER:
		rtw_phl_ser_dump_ple_buffer(phl);
		break;
	default:
		break;
	}
}

void phl_msg_hub_rx_evt_hdlr(struct phl_info_t* phl, u16 evt_id,
		u8 *buf, u32 len)
{
	PHL_DBG("%s : evt_id %d.\n", __func__, evt_id);

	switch (evt_id) {
	case HAL_C2H_EV_BB_MUGRP_DOWN:
		break;
	case MSG_EVT_TSF32_TOG:
		PHL_TRACE(COMP_PHL_P2PPS, _PHL_INFO_, "[NOA]phl_msg_hub_rx_evt_hdlr():toggle happen!!\n");
		phl_p2pps_tsf32_tog_handler(phl);
		break;
	case MSG_EVT_DBG_RX_DUMP:
		phl_rx_dbg_dump(phl, HW_PHY_0);
		break;
#ifdef CONFIG_PHL_TWT
	case MSG_EVT_TWT_WAIT_ANNOUNCE:
		rtw_phl_twt_handle_c2h_wait_annc(phl, buf);
		break;
#endif
	default:
		break;
	}
}
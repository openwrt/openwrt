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
#define _TEST_MODULE_C_

#ifdef CONFIG_PHL_TEST_SUITE
#include "../phl_headers.h"

#define MAX_TEST_OBJ_NUM 128
#define THREAD_SLEEP_UNIT 1000 /* ms */
#define MAX_TEST_CMD_BUF 2000

#define TEST_MGNT_ALREADY_INIT(_test_mgnt) \
	((_test_mgnt) && TEST_STATUS_FLAG((_test_mgnt)->status,TM_STATUS_INIT))
#define IS_TEST_MGNT_ALLOC(_phl_com)	((_phl_com) && ((_phl_com)->test_mgnt))

enum TEST_MGNT_STATUS_FALGS{
	TM_STATUS_INIT = BIT0,
	TM_STATUS_FORCE_STOP = BIT1,
	TM_STATUS_THREAD_START = BIT2
};

enum TEST_OBJ_STATUS_FALGS{
/* regular status*/
	TO_STATUS_PASS = BIT0,
	TO_STATUS_FAIL = BIT1,
	TO_STATUS_ENQ = BIT2,
	TO_STATUS_RUNNING = BIT3,
/* process error status*/
	TO_STATUS_RUN_THREAD_FAIL = BIT24,
	TO_STATUS_TIMEOUT = BIT25,
};

struct test_object_ex {
	struct test_object test_obj;
	u32 start_time;
	u32 end_time;
	u32 status;
	struct rtw_phl_handler handler;
	void* test_mgnt;
	_os_thread objthread;
	u8 submd_obj_id;
	u8 submd_test_mode;
};

struct test_obj_queue {
	_os_list q;
	_os_mutex lock;
};

struct test_mgnt_info{
	struct test_obj_queue idle_queue;
	struct test_obj_queue busy_queue;
	struct test_obj_queue rpt_queue;
	struct test_obj_queue err_queue;
	struct test_object_ex test_obj_pool[MAX_TEST_OBJ_NUM];
	struct test_object_ex *cur_test_obj;
	s32 max_run_time[TEST_LVL_MAX];
	_os_thread thread;
	u32 status;
	_os_sema busy_chk;
	struct rtw_phl_com_t *phl_com;
	struct phl_info_t *phl;
	void *hal;

	/* test module context root */
	void *mp_ctx;
	void *ver_ctx;
	void *fpga_ctx;
};

struct test_module_cmd {
	u8 type;
	u16 len;
	u8 buf[MAX_TEST_CMD_BUF];
};

struct test_module_info {
	u8 tm_type;
	u8 tm_mode;
};

static void _init_test_obj_queue(void *d, struct test_obj_queue *queue)
{
	INIT_LIST_HEAD(&queue->q);
	_os_mutex_init(d, &queue->lock);
}

static void _deinit_test_obj_queue(void *d, struct test_obj_queue *queue)
{
	_os_mutex_deinit(d, &queue->lock);
}

static int _enqueue_obj(void *d,
	struct test_obj_queue *queue, struct test_object_ex *obj)
{
	_os_mutex_lock(d, &queue->lock);
	list_add_tail(&(obj->test_obj.list), &queue->q);
	_os_mutex_unlock(d, &queue->lock);

	return 0;
}

static u8 _dequeue_head_obj(void *d,
	struct test_obj_queue *queue, struct test_object_ex **obj)
{
	(*obj) = NULL;
	if(list_empty(&(queue->q)))
		return false;
	_os_mutex_lock(d, &queue->lock);
	(*obj) = list_first_entry(&(queue->q),
				struct test_object_ex, test_obj.list);
	list_del(&((*obj)->test_obj.list));
	_os_mutex_unlock(d, &queue->lock);
	return ((*obj) == NULL|| ((struct list_head*)(*obj)) == &(queue->q)) ? \
		(false) : (true);
}

int run_test(void *testobj)
{
	struct test_object_ex *obj = (struct test_object_ex *)testobj;
	struct test_mgnt_info *test_mgnt = obj->test_mgnt;
	u8 ret = false;
	u32 start_time = 0;
	u32 end_time = 0;
	void	*d = test_mgnt->phl_com->drv_priv;
	/* -1: no limit, 0: not set use default value */
	s32 limit = obj->test_obj.total_time_ms;
	enum rtw_phl_status pstatus = RTW_PHL_STATUS_SUCCESS;

	if (obj->test_obj.total_time_ms != -1) {
		limit = (obj->test_obj.total_time_ms) ? \
			(obj->test_obj.total_time_ms) : \
			(test_mgnt->max_run_time[obj->test_obj.run_lvl]);
	}

	SET_STATUS_FLAG(obj->status, TO_STATUS_RUNNING);

	pstatus = phl_schedule_handler(test_mgnt->phl_com, &(obj->handler));

	if (RTW_PHL_STATUS_SUCCESS != pstatus) {
		SET_STATUS_FLAG(obj->status, TO_STATUS_RUN_THREAD_FAIL);
		CLEAR_STATUS_FLAG(obj->status, TO_STATUS_RUNNING);
		return 0;
	}
	start_time = _os_get_cur_time_us();
	/*wait test end*/
	while (!(obj->test_obj.ctrl.is_test_end(obj->test_obj.priv))) {
		end_time = _os_get_cur_time_us();
		if ((limit >= 0) && ((s32)((end_time-start_time)/1000) >= limit)) {
			SET_STATUS_FLAG(obj->status, TO_STATUS_TIMEOUT);
			obj->end_time = end_time;
			break;
		}
		_os_sleep_us(test_mgnt->phl_com->drv_priv, THREAD_SLEEP_UNIT);
	}
	if (!TEST_STATUS_FLAG(obj->status, TO_STATUS_TIMEOUT))
		obj->end_time = _os_get_cur_time_us();

	ret = obj->test_obj.ctrl.is_test_pass(obj->test_obj.priv);

	SET_STATUS_FLAG(obj->status, (!ret) ? \
				(TO_STATUS_FAIL) : (TO_STATUS_PASS));

	_enqueue_obj(d, &(test_mgnt->rpt_queue), obj);

	if (obj->submd_test_mode == INTGR_TEST_MODE) {
		if (!_os_thread_check_stop(d, &(obj->objthread)))
			_os_thread_wait_stop(d, &obj->objthread);
	}

	CLEAR_STATUS_FLAG(obj->status, TO_STATUS_RUNNING);
	PHL_INFO("[TM]: obj thread (%s) stop.\n", obj->test_obj.name);

	return 0;
}

int test_thread(void *param)
{
	struct test_mgnt_info *test_mgnt
		= (struct test_mgnt_info *)phl_container_of(param,
							     struct test_mgnt_info,
							     thread);
	void *d = NULL;
	struct test_object_ex *obj;

	if (!IS_TEST_MGNT_ALLOC(test_mgnt->phl_com))
		return 0;

	d = test_mgnt->phl_com->drv_priv;
	if (!TEST_MGNT_ALREADY_INIT(test_mgnt)) {
		PHL_ERR("[TM]: return.\n");
		return 0;
	}
	PHL_INFO("[TM]: thread started.\n");
	SET_STATUS_FLAG(test_mgnt->status, TM_STATUS_THREAD_START);

	while (!TEST_STATUS_FLAG(test_mgnt->status, TM_STATUS_FORCE_STOP)) {
		_os_sema_down(d, &(test_mgnt->busy_chk));
		if (TEST_STATUS_FLAG(test_mgnt->status, TM_STATUS_FORCE_STOP))
			break;
		while (_dequeue_head_obj(d, &(test_mgnt->busy_queue),
					&(test_mgnt->cur_test_obj))) {
			_os_thread *thread = NULL;
			obj = test_mgnt->cur_test_obj;
			CLEAR_STATUS_FLAG(obj->status, TO_STATUS_ENQ);

			if (obj->submd_test_mode == UNIT_TEST_MODE) {
				PHL_INFO("[TM]: %s UNIT_TEST_MODE \n", __FUNCTION__);
				run_test(obj);
			} else if (obj->submd_test_mode == INTGR_TEST_MODE) {
				PHL_INFO("[TM]: %s INTGR_TEST_MODE \n", __FUNCTION__);
				thread = &obj->objthread;
				_os_thread_init(d, thread, run_test, obj, obj->test_obj.name);
				_os_thread_schedule(d, thread);
			}
			test_mgnt->cur_test_obj = NULL;
		}
	}
	CLEAR_STATUS_FLAG(test_mgnt->status, TM_STATUS_THREAD_START);
	PHL_INFO("[TM]: test mgnt thread is down\n");
	return 0;
}

static void _test_submodule_deinit(void *tm)
{
	struct test_mgnt_info *test_mgnt = (struct test_mgnt_info *)tm;

	phl_test_mp_deinit(test_mgnt->mp_ctx);
	phl_test_verify_deinit(test_mgnt->ver_ctx);
	phl_test_fpga_deinit(test_mgnt->fpga_ctx);
}

u8 phl_test_module_init(struct phl_info_t *phl_info)
{
	struct rtw_phl_com_t* phl_com = phl_info->phl_com;
	struct test_mgnt_info *test_mgnt = NULL;
	enum rtw_phl_status psts = RTW_PHL_STATUS_FAILURE;
	void *d = NULL;
	u8 i = 0;

	if(phl_com==NULL)
		goto _test_module_mem;
	d = phl_com->drv_priv;

	test_mgnt = (struct test_mgnt_info *)_os_mem_alloc(d,
					sizeof(struct test_mgnt_info));
	if (test_mgnt == NULL)
		goto _test_module_mem;
	_os_mem_set(d, test_mgnt, 0, sizeof(struct test_mgnt_info));
	phl_com->test_mgnt = test_mgnt;
	test_mgnt->phl_com = phl_com;
	test_mgnt->phl = phl_info;
	test_mgnt->hal = phl_info->hal;

	for(i = 0; i < MAX_TEST_OBJ_NUM; i++)
		test_mgnt->test_obj_pool[i].submd_obj_id = TEST_SUB_MODULE_UNKNOWN;

	psts = phl_test_mp_alloc(test_mgnt->phl, test_mgnt->hal, &(test_mgnt->mp_ctx));
	if (psts != RTW_PHL_STATUS_SUCCESS) {
		PHL_INFO("[TM]: %s phl_test_mp_alloc fail!\n",__FUNCTION__);
		goto _mp_module_mem;
	}

	psts = phl_test_verify_alloc(test_mgnt->phl, test_mgnt->hal, &(test_mgnt->ver_ctx));
	if(psts != RTW_PHL_STATUS_SUCCESS) {
		PHL_INFO("[TM]: %s phl_test_verify_alloc fail!\n",__FUNCTION__);
		goto _verify_module_mem;
	}

	psts = phl_test_fpga_alloc(test_mgnt->phl, test_mgnt->hal, &(test_mgnt->fpga_ctx));
	if(psts != RTW_PHL_STATUS_SUCCESS) {
		PHL_INFO("[TM]: %s phl_test_fpga_alloc fail!\n",__FUNCTION__);
		goto _fpga_module_mem;
	}

	PHL_INFO("[TM]: %s\n",__FUNCTION__);
	return true;

_fpga_module_mem:
	phl_test_verify_free(&test_mgnt->ver_ctx);
_verify_module_mem:
	phl_test_mp_free(&test_mgnt->mp_ctx);
_mp_module_mem:
	_os_mem_free(d, test_mgnt, sizeof(struct test_mgnt_info));
_test_module_mem:
	return false;
}

void phl_test_module_deinit(struct rtw_phl_com_t* phl_com)
{
	void *d = NULL;
	struct test_mgnt_info *test_mgnt = NULL;

	if(!IS_TEST_MGNT_ALLOC(phl_com))
		return;
	d = phl_com->drv_priv;
	test_mgnt = phlcom_to_test_mgnt(phl_com);

	if(TEST_MGNT_ALREADY_INIT(test_mgnt))
		phl_test_module_stop(phl_com);

	phl_test_mp_free(&test_mgnt->mp_ctx);
	phl_test_verify_free(&test_mgnt->ver_ctx);
	phl_test_fpga_free(&test_mgnt->fpga_ctx);
	_os_mem_free(d, test_mgnt, sizeof(struct test_mgnt_info));
	phl_com->test_mgnt = NULL;
	PHL_INFO("[TM]: %s\n",__FUNCTION__);
}

u8 phl_test_module_start(struct rtw_phl_com_t* phl_com)
{
	struct test_mgnt_info *test_mgnt = NULL;
	_os_thread *thread = NULL;
	void *d = NULL;
	u32	i = 0;

	if(!IS_TEST_MGNT_ALLOC(phl_com))
		return false;
	d = phl_com->drv_priv;
	test_mgnt = phlcom_to_test_mgnt(phl_com);

	if(TEST_MGNT_ALREADY_INIT(test_mgnt)) {
		PHL_INFO("[TM]: %s, already initialized\n",__FUNCTION__);
		return true;
	}
	test_mgnt->status = 0;
	_init_test_obj_queue(d, &(test_mgnt->busy_queue));
	_init_test_obj_queue(d, &(test_mgnt->idle_queue));
	_init_test_obj_queue(d, &(test_mgnt->rpt_queue));
	_init_test_obj_queue(d, &(test_mgnt->err_queue));

	/*insert all elements in obj pool into idle Q*/
	for(i = 0; i < MAX_TEST_OBJ_NUM;i++) {
		test_mgnt->test_obj_pool[i].submd_obj_id = TEST_SUB_MODULE_UNKNOWN;
		_enqueue_obj(d, &(test_mgnt->idle_queue),
				&(test_mgnt->test_obj_pool[i]));
	}
	test_mgnt->max_run_time[TEST_LVL_LOW] = TEST_LVL_LOW_TO;
	test_mgnt->max_run_time[TEST_LVL_NORMAL] = TEST_LVL_NORMAL_TO;
	test_mgnt->max_run_time[TEST_LVL_HIGH] = TEST_LVL_HIGH_TO;
	SET_STATUS_FLAG(test_mgnt->status, TM_STATUS_INIT);

	thread = &(test_mgnt->thread);
	_os_sema_init(d, &(test_mgnt->busy_chk), 0);
	_os_thread_init(d, thread, test_thread, thread, "test_thread");
	_os_thread_schedule(d, thread);
	PHL_INFO("[TM]: %s\n",__FUNCTION__);
	return true;
}

void phl_test_module_stop(struct rtw_phl_com_t* phl_com)
{
	struct test_mgnt_info *test_mgnt = NULL;
	void *d = NULL;
	u32	i = 0;

	if(!IS_TEST_MGNT_ALLOC(phl_com))
		return;
	d = phl_com->drv_priv;
	test_mgnt = phlcom_to_test_mgnt(phl_com);

	if(!TEST_MGNT_ALREADY_INIT(test_mgnt))
		return;

	/* Call all sub module deinit */
	_test_submodule_deinit(test_mgnt);

	SET_STATUS_FLAG(test_mgnt->status, TM_STATUS_FORCE_STOP);
	_os_thread_stop(d, &(test_mgnt->thread));
	_os_sema_up(d, &(test_mgnt->busy_chk));
	/*wait thread stop*/
	PHL_INFO("[TM]: %s stop test_thread\n",__FUNCTION__);
	_os_thread_deinit(d, &(test_mgnt->thread));
	_os_sema_free(d, &(test_mgnt->busy_chk));
	/*clean up test obj resource*/
	for (i = 0; i < MAX_TEST_OBJ_NUM; i++) {
		struct test_object_ex *obj = &(test_mgnt->test_obj_pool[i]);
		if (obj->submd_obj_id != TEST_SUB_MODULE_UNKNOWN && obj->status != TO_STATUS_ENQ) {
			if (obj->submd_test_mode == INTGR_TEST_MODE) {
				if (!_os_thread_check_stop(d, &(obj->objthread))) {
					_os_thread_stop(d, &(obj->objthread));
			_os_thread_deinit(d, &(obj->objthread));
				}
			}
			phl_deregister_handler(phl_com, &(obj->handler));
	}
	}
	/*no need to empty test obj Q before deinit*/
	_deinit_test_obj_queue(d, &(test_mgnt->busy_queue));
	_deinit_test_obj_queue(d, &(test_mgnt->idle_queue));
	_deinit_test_obj_queue(d, &(test_mgnt->rpt_queue));
	_deinit_test_obj_queue(d, &(test_mgnt->err_queue));
	CLEAR_STATUS_FLAG(test_mgnt->status, TM_STATUS_INIT);
	PHL_INFO("[TM]: %s\n",__FUNCTION__);
}

u8
rtw_phl_test_set_max_run_time(struct rtw_phl_com_t* phl_com,
                              enum TEST_RUN_LVL lvl,
                              u32 timeout_ms)
{
	struct test_mgnt_info *test_mgnt = NULL;
	if(!IS_TEST_MGNT_ALLOC(phl_com))
		return false;

	test_mgnt = phlcom_to_test_mgnt(phl_com);
	if(!TEST_MGNT_ALREADY_INIT(test_mgnt))
		return false;

	test_mgnt = phlcom_to_test_mgnt(phl_com);
	test_mgnt->max_run_time[lvl] = timeout_ms;
	return true;
}

void setup_test_rpt(void *d, struct test_rpt* rpt,struct test_object_ex *obj)
{
	char str[] = "Exceed Time Limit";
	char str2[] = "Test not run";
	char str3[] = "None";
	_os_mem_cpy(d,	rpt->name, obj->test_obj.name, TEST_NAME_LEN);

	if(TEST_STATUS_FLAG(obj->status, TO_STATUS_TIMEOUT)) {
		_os_mem_cpy(d, rpt->rsn, str, sizeof(str));
		rpt->status = 0;
		rpt->total_time = (u32)((obj->end_time - obj->start_time)/1000);
	}
	else if(TEST_STATUS_FLAG(obj->status, TO_STATUS_RUN_THREAD_FAIL)) {
		_os_mem_cpy(d, rpt->rsn, str2, sizeof(str2));
		rpt->status = 0;
		rpt->total_time = 0;
	}
	else{
		obj->test_obj.ctrl.get_fail_rsn(obj->test_obj.priv,
					(char*)&(rpt->rsn[0]), TEST_NAME_LEN-1);
		if(_os_strlen((u8*)rpt->rsn) <= 0)
			_os_mem_cpy(d, rpt->rsn, str3, sizeof(str3));
		if (TEST_STATUS_FLAG(obj->status, TO_STATUS_PASS))
			rpt->status = 1;
		else
			rpt->status = 0;
		rpt->total_time = (u32)((obj->end_time - obj->start_time)/1000);
	}
}

u8 rtw_phl_test_get_rpt(struct rtw_phl_com_t* phl_com, u8* buf, u32 len)
{
	struct test_mgnt_info *test_mgnt = NULL;
	struct test_object_ex *obj = NULL;
	u32 rpt_size = sizeof(struct test_rpt);
	void *d = NULL;
	u8 is_rpt_empty = true;

	if(!IS_TEST_MGNT_ALLOC(phl_com))
		return false;

	d = phl_com->drv_priv;
	test_mgnt = phlcom_to_test_mgnt(phl_com);
	if(!TEST_MGNT_ALREADY_INIT(test_mgnt))
		return false;

	while (len >= rpt_size) {
		if(_dequeue_head_obj(d,	&(test_mgnt->rpt_queue), &obj)) {
			is_rpt_empty = false;

			setup_test_rpt(d, (struct test_rpt*)buf, obj);

			if (TEST_STATUS_FLAG(obj->status, TO_STATUS_TIMEOUT))
				_enqueue_obj(d,	&(test_mgnt->err_queue), obj);
			else {
				if (obj->submd_test_mode == INTGR_TEST_MODE) {
					if (!_os_thread_check_stop(d, &(obj->objthread))) {
						PHL_INFO("[TM]: %s obj thread (%s) stop.\n",
						         __FUNCTION__,
						         obj->test_obj.name);
						_os_thread_stop(d, &(obj->objthread));
						_os_thread_deinit(d, &(obj->objthread));
					}
				}
				phl_deregister_handler(phl_com, &(obj->handler));
				obj->status = 0;
				obj->submd_obj_id = TEST_SUB_MODULE_UNKNOWN;
				_enqueue_obj(d,	&(test_mgnt->idle_queue), obj);
			}
			buf += rpt_size;
			len -= rpt_size;
		}
		else
			break;
	}
	return (is_rpt_empty)?(false):(true);
}

u8 rtw_phl_test_is_test_complete(struct rtw_phl_com_t* phl_com)
{
	struct test_mgnt_info *test_mgnt = NULL;
	void *d = NULL;

	if(!IS_TEST_MGNT_ALLOC(phl_com))
		return false;

	d = phl_com->drv_priv;
	test_mgnt = phlcom_to_test_mgnt(phl_com);
	if(!TEST_MGNT_ALREADY_INIT(test_mgnt))
		return false;
	if (list_empty(&(test_mgnt->busy_queue.q)) && !(test_mgnt->cur_test_obj))
		return true;
	return false;
}

static void _test_obj_thread_callback(void *context)
{
	struct rtw_phl_handler *phl_handler
		= (struct rtw_phl_handler *)phl_container_of(context,
							     struct rtw_phl_handler,
							     os_handler);
	struct test_object_ex *obj = (struct test_object_ex *)phl_handler->context;
	obj->start_time = _os_get_cur_time_us();
	obj->test_obj.ctrl.start_test(obj->test_obj.priv);
}

u8 init_obj_thread(struct test_mgnt_info *test_mgnt,
                   struct test_object_ex *obj,
                   enum TEST_RUN_LVL lvl)
{
	enum rtw_phl_status pstatus = RTW_PHL_STATUS_FAILURE;

	switch(lvl){
	case TEST_LVL_LOW:
	case TEST_LVL_NORMAL:
		obj->handler.type = RTW_PHL_HANDLER_PRIO_LOW;
		break;
	case TEST_LVL_HIGH:
		obj->handler.type = RTW_PHL_HANDLER_PRIO_HIGH;
		break;
	case TEST_LVL_NONE:
	case TEST_LVL_MAX:
		PHL_TRACE(COMP_PHL_DBG, _PHL_DEBUG_, "init_obj_thread(): Unsupported case:%d, please check it\n",
				lvl);
		return false;
	default:
		PHL_TRACE(COMP_PHL_DBG, _PHL_DEBUG_, "init_obj_thread(): Unrecognize case:%d, please check it\n",
				lvl);
		return false;
	}

	obj->handler.callback = _test_obj_thread_callback;
	obj->handler.context = obj;
	obj->handler.drv_priv = test_mgnt->phl_com->drv_priv;

	#if defined(CONFIG_RTW_OS_HANDLER_EXT)
	obj->handler.id = RTW_PHL_TEST_HANDLER;
	#endif /* CONFIG_RTW_OS_HANDLER_EXT */

	pstatus = phl_register_handler(test_mgnt->phl_com, &(obj->handler));
	return (RTW_PHL_STATUS_SUCCESS != pstatus) ? (false) : (true);
}

u8 rtw_phl_test_add_new_test_obj(struct rtw_phl_com_t *phl_com,
                                 char *name,
                                 void *custom_priv,
                                 enum TEST_RUN_LVL lvl,
                                 struct test_obj_ctrl_interface *ctrl_intf,
                                 s32 total_time_ms,
                                 u8 submd_id,
                                 u8 test_mode)
{
	struct test_mgnt_info *test_mgnt = NULL;
	struct test_object_ex *obj = NULL;
	void *d = NULL;
	u8 i;
	u8 is_idle_empty = true;
	u32 name_len = (_os_strlen((u8 *)name) > (u32)(TEST_NAME_LEN - 1)) ?\
		((u32)(TEST_NAME_LEN - 1)) :\
		(_os_strlen((u8 *)name) + 1);

	if (!IS_TEST_MGNT_ALLOC(phl_com))
		return false;

	d = phl_com->drv_priv;
	test_mgnt = phlcom_to_test_mgnt(phl_com);
	if (!TEST_MGNT_ALREADY_INIT(test_mgnt))
		return false;

	for (i = 0; i < MAX_TEST_OBJ_NUM; i++) {
		obj = &(test_mgnt->test_obj_pool[i]);
		if (obj->submd_obj_id == submd_id && obj->status == TO_STATUS_RUNNING) {
			PHL_INFO("%s() Error same submd id %d\n", __func__, obj->submd_obj_id);
		return false;
		}
	}

	if (_dequeue_head_obj(d, &(test_mgnt->idle_queue), &obj)) {
		is_idle_empty = false;
		if (TEST_STATUS_FLAG(obj->status, TO_STATUS_ENQ))
			phl_deregister_handler(phl_com, &(obj->handler));
		/*cleanup entire test obj before reuse*/
		_os_mem_set(d, obj, 0, sizeof(struct test_object_ex));

		_os_mem_cpy(d, &(obj->test_obj.ctrl), ctrl_intf,
				sizeof(struct test_obj_ctrl_interface));

		_os_mem_cpy(d, &(obj->test_obj.name[0]), name, name_len);
		obj->test_obj.priv = custom_priv;
		obj->test_obj.run_lvl = lvl;
		obj->test_mgnt = test_mgnt;
		obj->test_obj.total_time_ms = (lvl <= TEST_LVL_LOW) ?\
						(total_time_ms) : (0);
		obj->submd_obj_id = submd_id;
		obj->submd_test_mode = test_mode;

		if (init_obj_thread(test_mgnt, obj, lvl)) {
			SET_STATUS_FLAG(obj->status, TO_STATUS_ENQ);
			_enqueue_obj(d,	&(test_mgnt->busy_queue), obj);
			_os_sema_up(d, &(test_mgnt->busy_chk));
			PHL_INFO("[TM]: %s, id %u EnQ\n", __FUNCTION__, submd_id);
		} else {
			_enqueue_obj(d,	&(test_mgnt->idle_queue), obj);
			PHL_INFO("[TM]: %s, id %u EnQ fail\n", __FUNCTION__, submd_id);
		}
	}
	if (is_idle_empty || !TEST_STATUS_FLAG(obj->status, TO_STATUS_ENQ))
		return false;
	return true;
}

u8
rtw_phl_test_setup_bp(struct rtw_phl_com_t* phl_com,
                      struct test_bp_info* bp_info,
                      u8 submdid)
{
	u8 i;
	struct test_mgnt_info *test_mgnt = NULL;
	struct test_object_ex *obj = NULL;
	if(!IS_TEST_MGNT_ALLOC(phl_com))
		return false;

	test_mgnt = phlcom_to_test_mgnt(phl_com);

	for (i = 0; i < MAX_TEST_OBJ_NUM; i++) {
		obj = &(test_mgnt->test_obj_pool[i]);
		if (obj->submd_obj_id == submdid && TEST_STATUS_FLAG(obj->status, TO_STATUS_RUNNING)) {
			PHL_DBG("%s() objid = %d setup bp\n", __func__, obj->submd_obj_id);
			break;
		}
	}

	if(!TEST_MGNT_ALREADY_INIT(test_mgnt))
		return false;

	return (obj && TEST_STATUS_FLAG(obj->status, TO_STATUS_RUNNING))?\
		(obj->test_obj.ctrl.bp_handler(obj->test_obj.priv, bp_info)):\
		(BP_RET_RUN_ORIGIN_SEC);
}

void rtw_phl_test_submodule_init(struct rtw_phl_com_t* phl_com, void *buf)
{
	struct test_mgnt_info *test_mgnt = (struct test_mgnt_info *)phl_com->test_mgnt;
	struct test_module_info *tm_info = NULL;

	if(buf == NULL)
		return;

	tm_info = (struct test_module_info *)buf;

	switch(tm_info->tm_type) {
		case TEST_SUB_MODULE_MP:
			phl_test_mp_init(test_mgnt->mp_ctx);
			phl_test_mp_start(test_mgnt->mp_ctx, tm_info->tm_mode);
			break;
		case TEST_SUB_MODULE_VERIFY:
			phl_test_verify_init(test_mgnt->ver_ctx);
			phl_test_verify_start(test_mgnt->ver_ctx);
			break;
		case TEST_SUB_MODULE_FPGA:
			phl_test_fpga_init(test_mgnt->fpga_ctx);
			phl_test_fpga_start(test_mgnt->fpga_ctx, tm_info->tm_mode);
			break;
		case TEST_SUB_MODULE_TOOL:
		default:
			break;
	}
}

void rtw_phl_test_submodule_deinit(struct rtw_phl_com_t* phl_com, void *buf)
{
	struct test_mgnt_info *test_mgnt = (struct test_mgnt_info *)phl_com->test_mgnt;
	struct test_module_info *tm_info = NULL;

	if(buf == NULL)
		return;

	tm_info = (struct test_module_info *)buf;

	switch(tm_info->tm_type) {
		case TEST_SUB_MODULE_MP:
			phl_test_mp_stop(test_mgnt->mp_ctx, tm_info->tm_mode);
			phl_test_mp_deinit(test_mgnt->mp_ctx);
			break;
		case TEST_SUB_MODULE_VERIFY:
			phl_test_verify_stop(test_mgnt->ver_ctx);
			phl_test_verify_deinit(test_mgnt->ver_ctx);
			break;
		case TEST_SUB_MODULE_FPGA:
			phl_test_fpga_stop(test_mgnt->fpga_ctx, tm_info->tm_mode);
			phl_test_fpga_deinit(test_mgnt->fpga_ctx);
			break;
		case TEST_SUB_MODULE_TOOL:
		default:
			break;
	}
}

void
rtw_phl_test_submodule_cmd_process(struct rtw_phl_com_t* phl_com,
                                   void *buf,
                                   u32 buf_len)
{
	struct test_mgnt_info *test_mgnt = (struct test_mgnt_info *)phl_com->test_mgnt;
	struct test_module_cmd *cmd = NULL;
	u8 type = TEST_SUB_MODULE_UNKNOWN;

	if(buf == NULL)
		return;

	cmd = (struct test_module_cmd *)buf;
	type = cmd->type;
	/*debug_dump_data(cmd->buf, MAX_TEST_CMD_BUF, "[TM] cmd buf =");*/
	PHL_INFO("%s: len = %d\n", __FUNCTION__, cmd->len);
	PHL_INFO("%s: type = %d\n", __FUNCTION__, cmd->type);

	switch(type) {
		case TEST_SUB_MODULE_MP:
			phl_test_mp_cmd_process(test_mgnt->mp_ctx, cmd->buf, cmd->len, TEST_SUB_MODULE_MP);
			break;
		case TEST_SUB_MODULE_VERIFY:
			phl_test_verify_cmd_process(test_mgnt->ver_ctx, cmd->buf, cmd->len, TEST_SUB_MODULE_VERIFY);
			break;
		case TEST_SUB_MODULE_FPGA:
			phl_test_fpga_cmd_process(test_mgnt->fpga_ctx, cmd->buf, cmd->len, TEST_SUB_MODULE_FPGA);
			break;
		case TEST_SUB_MODULE_TOOL:
		default:
			break;
	}
}

void
rtw_phl_test_submodule_get_rpt(struct rtw_phl_com_t* phl_com,
                               void *buf,
                               u32 buf_len)
{
	struct test_mgnt_info *test_mgnt = (struct test_mgnt_info *)phl_com->test_mgnt;
	struct test_module_cmd *cmd = NULL;
	u8 type = TEST_SUB_MODULE_UNKNOWN;

	if(!buf)
		return;

	cmd = (struct test_module_cmd *)buf;
	type = cmd->type;

	switch(type) {
		case TEST_SUB_MODULE_MP:
			phl_test_mp_get_rpt(test_mgnt->mp_ctx, cmd->buf, cmd->len);
			break;
		case TEST_SUB_MODULE_VERIFY:
			phl_test_verify_get_rpt(test_mgnt->ver_ctx, cmd->buf, cmd->len);
			break;
		case TEST_SUB_MODULE_FPGA:
			phl_test_fpga_get_rpt(test_mgnt->fpga_ctx, cmd->buf, cmd->len);
			break;
		case TEST_SUB_MODULE_TOOL:
		default:
			break;
	}
}
#endif /*ifdef CONFIG_PHL_TEST_SUITE*/

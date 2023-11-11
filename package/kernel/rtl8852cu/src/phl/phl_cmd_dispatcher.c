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
#define _PHL_CMD_DISPR_C_
#include "phl_headers.h"
#ifdef CONFIG_CMD_DISP

#ifdef CONFIG_PHL_MSG_NUM
#define MAX_PHL_MSG_NUM CONFIG_PHL_MSG_NUM
#else
#define MAX_PHL_MSG_NUM (24)
#endif

#define MAX_CMD_REQ_NUM (8)
#define MODL_MASK_LEN (PHL_BK_MDL_END/8)

#define GEN_VALID_HDL(_idx) ((u32)(BIT31 | (u32)(_idx)))
#define IS_HDL_VALID(_hdl) ((_hdl) & BIT31)
#define GET_MSG_IDX_FROM_HDL(_hdl) ((u16)((_hdl) & 0xFFFF))
#define GET_REQ_IDX_FROM_HDL(_hdl) ((u8)((_hdl) & 0xFFFF))

#define GET_CUR_PENDING_EVT( _obj, _mdl_id) \
	((u16)((_obj)->mdl_info[(_mdl_id)].pending_evt_id))
#define SET_CUR_PENDING_EVT( _obj, _mdl_id, _evt_id) \
	((_obj)->mdl_info[(_mdl_id)].pending_evt_id = (_evt_id))

#define IS_EXCL_MDL(_obj, _mdl) ((_obj)->exclusive_mdl == (_mdl))
#define SET_EXCL_MDL(_obj, _mdl) ((_obj)->exclusive_mdl = (_mdl))
#define CLEAR_EXCL_MDL(_obj) ((_obj)->exclusive_mdl = PHL_MDL_ID_MAX)
#define STOP_DISPATCH_MSG(_ret) \
	((_ret) != MDL_RET_SUCCESS && (_ret) != MDL_RET_IGNORE)

#ifdef CONFIG_CMD_DISP_SUPPORT_CUSTOM_SEQ
#define SET_MDL_HANDLE( _obj, _mdl_id, _handle) \
	((_obj)->mdl_info[(_mdl_id)].handle = (void*)(_handle))
#define GET_MDL_HANDLE( _obj, _mdl_id) \
	((_obj)->mdl_info[(_mdl_id)].handle)
#endif

enum phl_msg_status {
	MSG_STATUS_ENQ = BIT0,
	MSG_STATUS_RUN = BIT1,
	MSG_STATUS_NOTIFY_COMPLETE = BIT2,
	MSG_STATUS_CANCEL = BIT3,
	MSG_STATUS_PRE_PHASE = BIT4,
	MSG_STATUS_FAIL = BIT5,
	MSG_STATUS_OWNER_BK_MDL = BIT6,
	MSG_STATUS_OWNER_REQ = BIT7,
	MSG_STATUS_CLR_SNDR_MSG_IF_PENDING = BIT8,
	MSG_STATUS_PENDING = BIT9,
	MSG_STATUS_FOR_ABORT = BIT10,
	MSG_STATUS_PENDING_DURING_CANNOT_IO = BIT11,
};

enum cmd_req_status {
	REQ_STATUS_ENQ = BIT0,
	REQ_STATUS_RUN = BIT1,
	REQ_STATUS_CANCEL = BIT2,
	REQ_STATUS_LAST_PERMIT = BIT3,
	REQ_STATUS_PREPARE = BIT4,
};

enum phl_mdl_status {
	MDL_INIT = BIT0,
	MDL_STARTED = BIT1,
};

enum dispatcher_status {
	DISPR_INIT = BIT0,
	DISPR_STARTED = BIT1,
	DISPR_SHALL_STOP = BIT2,
	DISPR_MSGQ_INIT = BIT3,
	DISPR_REQ_INIT = BIT4,
	DISPR_NOTIFY_IDLE = BIT5,
	DISPR_CLR_PEND_MSG = BIT6,
	DISPR_CTRL_PRESENT = BIT7,
	DISPR_WAIT_ABORT_MSG_DONE = BIT8,
	DISPR_CANNOT_IO = BIT9,
	DISPR_CLEARANCE = BIT10,
	DISPR_EXCLUSIVE_USE = BIT11,
};

enum token_op_type {
	TOKEN_OP_ADD_CMD_REQ = 1,
	TOKEN_OP_FREE_CMD_REQ = 2,
	TOKEN_OP_CANCEL_CMD_REQ = 3,
	TOKEN_OP_RENEW_CMD_REQ = 4,
};

/**
 * phl_bk_module - instance of phl background module,
 * @status: contain mgnt status flags, refer to enum phl_mdl_status
 * @id: refer to enum phl_module_id
 * @priv: private context
 * @ops: interface to interacting with phl_module
 */
struct phl_bk_module {
	_os_list list;
	u8 status;
	u8 id;
	void *priv;
	struct phl_bk_module_ops ops;
};
#ifdef CONFIG_CMD_DISP_SUPPORT_CUSTOM_SEQ
struct dispr_msg_attr {
	struct msg_self_def_seq self_def_seq;
};
#endif
/**
 * phl_dispr_msg_ex - phl msg extension,
 * @status: contain mgnt status flags, refer to enum phl_msg_status
 * @idx: idx in original msg_ex pool
 * @msg: msg content from external module
 * @premap: notifty map in pre-role phase, refer to enum phl_module_id
 * @postmap: notifty map in post-role phase, refer to enum phl_module_id
 * @completion: msg completion routine.
 * @priv: private context to completion routine.
 * @module: module handle of msg source, only used when msg fails
 */
struct phl_dispr_msg_ex {
	_os_list list;
	u16 status;
	u16 idx;
	struct phl_msg msg;
	u8 premap[MODL_MASK_LEN];
	u8 postmap[MODL_MASK_LEN];
	struct msg_completion_routine completion;
	struct phl_bk_module *module; /* module handle which assign in msg_id*/
#ifdef CONFIG_CMD_DISP_SUPPORT_CUSTOM_SEQ
	struct dispr_msg_attr *attr;
#endif
};

/**
 * phl_token_op_info - for internal mgnt purpose,
 * @info: mgnt data
 */
struct phl_token_op_info {
	_os_list list;
	u8 used;
	enum token_op_type type;
	u8 data;
};
/**
 * phl_cmd_token_req_ex - cmd token request extension,
 * @status: contain mgnt status flags, refer to enum cmd_req_status
 * @idx: idx in original req_ex pool
 * @req: req content from external module.
 */
struct phl_cmd_token_req_ex {
	_os_list list;
	u8 idx;
	u8 status;
	struct phl_cmd_token_req req;
	struct phl_token_op_info add_req_info;
	struct phl_token_op_info free_req_info;
};

struct mdl_mgnt_info {
	u16 pending_evt_id;
#ifdef CONFIG_CMD_DISP_SUPPORT_CUSTOM_SEQ
	void* handle;
#endif
};

/**
 * cmd_dispatcher,
 * @idx: idx in dispatch engine, corresponding to band idx
 * @status: contain mgnt status flags, refer to enum dispatcher_status
 * @phl_info: for general reference usage.
 * @module_q: module queue that link each modules based on priority
 * @msg_ex_pool: msg extension pool
 * @bk_thread: background thread
 * @token_req_ex_pool: req extension pool
 * @token_cnt: current token count,
 * 	       cmd req can be executed when dispatcher's token count is 0
 * @bitmap: cosist of existing background modules loaded in current dispatcher,
 *	    refer to enum phl_module_id
 * @basemap: BK modules that must be notified when handling msg
 * @controller: instance of dispr controller module
 * @renew_req_info: used to trigger next token req registration
 * @exclusive_mdl: In certain conditions, like dev IO status change,
 * 		   dispr would only allow designated module to send msg and cancel the rest,
 */
struct cmd_dispatcher {
	u8 idx;
	u16 status;
	struct phl_info_t *phl_info;
	struct phl_queue module_q[PHL_MDL_PRI_MAX];
	struct phl_dispr_msg_ex msg_ex_pool[MAX_PHL_MSG_NUM];
	struct phl_queue msg_wait_q;
	struct phl_queue msg_idle_q;
	struct phl_queue msg_pend_q;
	#ifdef CONFIG_CMD_DISP_SOLO_MODE
	_os_sema msg_q_sema; /* wake up background thread in SOLO_THREAD_MODE*/
	_os_thread bk_thread;
	#endif
	struct phl_cmd_token_req_ex token_req_ex_pool[MAX_CMD_REQ_NUM];
	struct phl_queue token_req_wait_q;
	struct phl_queue token_req_idle_q;
	struct phl_queue token_op_q;
	_os_lock token_op_q_lock;
	_os_atomic token_cnt; // atomic
	struct phl_cmd_token_req_ex *cur_cmd_req;
	u8 bitmap[MODL_MASK_LEN];
	u8 basemap[MODL_MASK_LEN];
	struct mdl_mgnt_info mdl_info[PHL_MDL_ID_MAX];
	struct phl_bk_module controller;
	struct phl_token_op_info renew_req_info;
	u8 exclusive_mdl;
};

enum rtw_phl_status dispr_process_token_req(struct cmd_dispatcher *obj);
void send_bk_msg_phy_on(struct cmd_dispatcher *obj);
void send_bk_msg_phy_idle(struct cmd_dispatcher *obj);
enum rtw_phl_status send_dev_io_status_change(struct cmd_dispatcher *obj, u8 allow_io);
void _notify_dispr_controller(struct cmd_dispatcher *obj, struct phl_dispr_msg_ex *ex);
static u8 dispr_enqueue_token_op_info(struct cmd_dispatcher *obj, struct phl_token_op_info *op_info,
			    	enum token_op_type type, u8 data);

#ifdef CONFIG_CMD_DISP_SUPPORT_CUSTOM_SEQ
static void free_dispr_attr(void *d, struct dispr_msg_attr **dispr_attr);
static enum phl_mdl_ret_code run_self_def_seq(struct cmd_dispatcher *obj, struct phl_dispr_msg_ex *ex,
			enum phl_bk_module_priority priority, u8 pre_prot_phase);
#endif

inline static
enum phl_bk_module_priority _get_mdl_priority(enum phl_module_id id)
{
	if (id < PHL_BK_MDL_ROLE_START)
		return PHL_MDL_PRI_MAX;
	else if (id <= PHL_BK_MDL_ROLE_END)
		return PHL_MDL_PRI_ROLE;
	else if ( id <= PHL_BK_MDL_MDRY_END)
		return PHL_MDL_PRI_MANDATORY;
	else if (id <= PHL_BK_MDL_OPT_END)
		return PHL_MDL_PRI_OPTIONAL;
	else
		return PHL_MDL_PRI_MAX;
}

inline static u8 _is_bitmap_empty(void *d, u8 *bitmap)
{
	u8 empty[MODL_MASK_LEN] = {0};

	return (!_os_mem_cmp(d, bitmap, empty, MODL_MASK_LEN))?(true):(false);
}

inline static void _print_bitmap(u8 *bitmap)
{
	u8 k = 0;

	PHL_TRACE(COMP_PHL_CMDDISP, _PHL_INFO_, "print bitmap: \n");

	for (k = 0; k < MODL_MASK_LEN; k++) {
		PHL_TRACE(COMP_PHL_CMDDISP, _PHL_DEBUG_,"[%d]:0x%x\n", k, bitmap[k]);
	}
}

static void notify_dispr_thread(struct cmd_dispatcher *obj)
{
	#ifdef CONFIG_CMD_DISP_SOLO_MODE
	void *d = phl_to_drvpriv(obj->phl_info);

	_os_sema_up(d, &(obj->msg_q_sema));
	#else
	disp_eng_notify_share_thread(obj->phl_info, (void*)obj);
	#endif
}

static void cmd_token_complete(struct cmd_dispatcher *obj)
{
	struct phl_cmd_dispatch_engine *disp_eng = &(obj->phl_info->disp_eng);
	u8 ntfyidx = HW_BAND_0;

	if(disp_eng->phy_num < 2)
		return;

	if (TEST_STATUS_FLAG(obj->status, DISPR_EXCLUSIVE_USE)) {
		CLEAR_STATUS_FLAG(obj->status, DISPR_EXCLUSIVE_USE);
		if(obj->idx == HW_BAND_0)
			ntfyidx = HW_BAND_1;
		phl_disp_eng_clearance_release(obj->phl_info, ntfyidx);
	}
	if (TEST_STATUS_FLAG(obj->status, DISPR_CLEARANCE)) {
		if(obj->idx == HW_BAND_0)
			ntfyidx = HW_BAND_1;
		phl_disp_eng_exclusive_ready(obj->phl_info, ntfyidx);
	}
}

static void on_abort_msg_complete(struct cmd_dispatcher *obj, struct phl_dispr_msg_ex *ex)
{
	/* since struct phl_token_op_info is used to synchronously handle token req in background thread
	 * here use add_req_info to notify background thread to run dispr_process_token_req again before handling next msg
	 */
	CLEAR_STATUS_FLAG(obj->status, DISPR_WAIT_ABORT_MSG_DONE);
	cmd_token_complete(obj);
	dispr_enqueue_token_op_info(obj, &obj->renew_req_info, TOKEN_OP_RENEW_CMD_REQ, 0xff);
}

static u8 pop_front_idle_msg(struct cmd_dispatcher *obj,
			     struct phl_dispr_msg_ex **msg)
{
	void *d = phl_to_drvpriv(obj->phl_info);
	_os_list *new_msg = NULL;

	(*msg) = NULL;
	if (pq_pop(d, &(obj->msg_idle_q), &new_msg, _first, _bh)) {
		(*msg) = (struct phl_dispr_msg_ex *)new_msg;
		(*msg)->status = 0;
		(*msg)->module = NULL;
		(*msg)->completion.priv = NULL;
		(*msg)->completion.completion = NULL;
		_os_mem_set(d, (*msg)->premap, 0, MODL_MASK_LEN);
		_os_mem_set(d, (*msg)->postmap, 0, MODL_MASK_LEN);
		_os_mem_set(d, &((*msg)->msg), 0, sizeof(struct phl_msg));
#ifdef CONFIG_CMD_DISP_SUPPORT_CUSTOM_SEQ
		(*msg)->attr = NULL;
#endif
		PHL_TRACE(COMP_PHL_CMDDISP, _PHL_INFO_, "%s[%d]: remain cnt(%d)\n",
		          __func__, obj->idx, obj->msg_idle_q.cnt);
		return true;
	} else {
		return false;
	}
}

static void push_back_idle_msg(struct cmd_dispatcher *obj,
			       struct phl_dispr_msg_ex *ex)
{
	void *d = phl_to_drvpriv(obj->phl_info);

	if (TEST_STATUS_FLAG(ex->status, MSG_STATUS_NOTIFY_COMPLETE) &&
	    ex->completion.completion) {
		if (TEST_STATUS_FLAG(ex->status, MSG_STATUS_CANCEL))
			SET_MSG_INDC_FIELD(ex->msg.msg_id, MSG_INDC_CANCEL);
		ex->completion.completion(ex->completion.priv, &(ex->msg));
		CLEAR_STATUS_FLAG(ex->status, MSG_STATUS_NOTIFY_COMPLETE);
	}
	if (TEST_STATUS_FLAG(ex->status, MSG_STATUS_FOR_ABORT))
		on_abort_msg_complete(obj, ex);
	ex->status = 0;
	if(GET_CUR_PENDING_EVT(obj, MSG_MDL_ID_FIELD(ex->msg.msg_id)) == MSG_EVT_ID_FIELD(ex->msg.msg_id))
		SET_CUR_PENDING_EVT(obj, MSG_MDL_ID_FIELD(ex->msg.msg_id), MSG_EVT_MAX);
	ex->msg.msg_id = 0;
#ifdef CONFIG_CMD_DISP_SUPPORT_CUSTOM_SEQ
	free_dispr_attr(d, &(ex->attr));
#endif
	pq_push(d, &(obj->msg_idle_q), &(ex->list), _tail, _bh);
	PHL_TRACE(COMP_PHL_CMDDISP, _PHL_INFO_, "%s[%d]: remain cnt(%d)\n",
		  __func__, obj->idx, obj->msg_idle_q.cnt);
}

static u8 pop_front_wait_msg(struct cmd_dispatcher *obj,
			     struct phl_dispr_msg_ex **msg)
{
	void *d = phl_to_drvpriv(obj->phl_info);
	_os_list *new_msg = NULL;

	(*msg) = NULL;
	if (pq_pop(d, &(obj->msg_wait_q), &new_msg, _first, _bh)) {
		(*msg) = (struct phl_dispr_msg_ex *)new_msg;
		SET_STATUS_FLAG((*msg)->status, MSG_STATUS_RUN);
		CLEAR_STATUS_FLAG((*msg)->status, MSG_STATUS_ENQ);
		CLEAR_STATUS_FLAG((*msg)->status, MSG_STATUS_PENDING);
		return true;
	} else {
		return false;
	}
}

static void push_back_wait_msg(struct cmd_dispatcher *obj,
			       struct phl_dispr_msg_ex *ex)
{
	void *d = phl_to_drvpriv(obj->phl_info);

	SET_STATUS_FLAG(ex->status, MSG_STATUS_ENQ);
	CLEAR_STATUS_FLAG(ex->status, MSG_STATUS_RUN);
	pq_push(d, &(obj->msg_wait_q), &(ex->list), _tail, _bh);
	notify_dispr_thread(obj);
}

 u8 is_higher_priority(void *d, void *priv,_os_list *input, _os_list *obj)
 {
	struct phl_dispr_msg_ex *ex_input = (struct phl_dispr_msg_ex *)input;
	struct phl_dispr_msg_ex *ex_obj = (struct phl_dispr_msg_ex *)obj;

	if (IS_DISPR_CTRL(MSG_MDL_ID_FIELD(ex_input->msg.msg_id)) &&
	    !IS_DISPR_CTRL(MSG_MDL_ID_FIELD(ex_obj->msg.msg_id)))
		return true;
	return false;
 }

static void insert_msg_by_priority(struct cmd_dispatcher *obj,
			       struct phl_dispr_msg_ex *ex)
{
	void *d = phl_to_drvpriv(obj->phl_info);

	SET_STATUS_FLAG(ex->status, MSG_STATUS_ENQ);
	CLEAR_STATUS_FLAG(ex->status, MSG_STATUS_RUN);
	pq_insert(d, &(obj->msg_wait_q), _bh, NULL, &(ex->list), is_higher_priority);
	notify_dispr_thread(obj);
}

static u8 pop_front_pending_msg(struct cmd_dispatcher *obj,
			     struct phl_dispr_msg_ex **msg)
{
	void *d = phl_to_drvpriv(obj->phl_info);
	_os_list *new_msg = NULL;

	(*msg) = NULL;
	if (pq_pop(d, &(obj->msg_pend_q), &new_msg, _first, _bh)) {
		(*msg) = (struct phl_dispr_msg_ex *)new_msg;
		return true;
	} else {
		return false;
	}
}

static void push_back_pending_msg(struct cmd_dispatcher *obj,
			       struct phl_dispr_msg_ex *ex)
{
	void *d = phl_to_drvpriv(obj->phl_info);

	SET_STATUS_FLAG(ex->status, MSG_STATUS_ENQ);
	CLEAR_STATUS_FLAG(ex->status, MSG_STATUS_RUN);

	if(TEST_STATUS_FLAG(ex->status, MSG_STATUS_CLR_SNDR_MSG_IF_PENDING))
		SET_CUR_PENDING_EVT(obj, MSG_MDL_ID_FIELD(ex->msg.msg_id), MSG_EVT_ID_FIELD(ex->msg.msg_id));
	pq_push(d, &(obj->msg_pend_q), &(ex->list), _tail, _bh);
	PHL_TRACE(COMP_PHL_CMDDISP, _PHL_INFO_, "%s[%d]: remain cnt(%d)\n",
	          __func__, obj->idx, obj->msg_pend_q.cnt);
}

static void clear_pending_msg(struct cmd_dispatcher *obj)
{
	struct phl_dispr_msg_ex *ex = NULL;

	if(!TEST_STATUS_FLAG(obj->status, DISPR_CLR_PEND_MSG))
		return;
	CLEAR_STATUS_FLAG(obj->status, DISPR_CLR_PEND_MSG);
	while (pop_front_pending_msg(obj, &ex)) {
		if (IS_DISPR_CTRL(MSG_MDL_ID_FIELD(ex->msg.msg_id)))
			insert_msg_by_priority(obj, ex);
		else
			push_back_wait_msg(obj, ex);
	}
}

static void clear_waiting_msg(struct cmd_dispatcher *obj)
{
	struct phl_dispr_msg_ex *ex = NULL;

	PHL_TRACE(COMP_PHL_CMDDISP, _PHL_INFO_, "%s[%d]: remain cnt(%d)\n",
	          __func__, obj->idx, obj->msg_idle_q.cnt);
	while(obj->msg_idle_q.cnt != MAX_PHL_MSG_NUM) {
		while (pop_front_pending_msg(obj, &ex))
			push_back_wait_msg(obj, ex);
		while (pop_front_wait_msg(obj, &ex))
			push_back_idle_msg(obj, ex);
	}
}

static bool is_special_msg(struct cmd_dispatcher *obj, struct phl_dispr_msg_ex *ex)
{
	u8 mdl_id = MSG_MDL_ID_FIELD(ex->msg.msg_id);
	u16 evt = MSG_EVT_ID_FIELD(ex->msg.msg_id);

	if (TEST_STATUS_FLAG(obj->status, DISPR_CANNOT_IO)) {
		if ( IS_EXCL_MDL(obj, mdl_id) ||
		     evt == MSG_EVT_DEV_CANNOT_IO ||
		     evt == MSG_EVT_DEV_RESUME_IO ||
		     evt == MSG_EVT_PHY_ON ||
		     evt == MSG_EVT_PHY_IDLE)
			return true;
	}
	return false;
}

static bool is_msg_canceled(struct cmd_dispatcher *obj, struct phl_dispr_msg_ex *ex)
{
	u16 pending_evt = GET_CUR_PENDING_EVT(obj, MSG_MDL_ID_FIELD(ex->msg.msg_id));

	if (!TEST_STATUS_FLAG(obj->status, DISPR_STARTED) ||
	    TEST_STATUS_FLAG(ex->status, MSG_STATUS_CANCEL))
		return true;

	if (pending_evt != MSG_EVT_MAX && pending_evt != MSG_EVT_ID_FIELD(ex->msg.msg_id)) {
		SET_MSG_INDC_FIELD(ex->msg.msg_id, MSG_INDC_CLR_SNDR);
		SET_STATUS_FLAG(ex->status, MSG_STATUS_CANCEL);
		PHL_TRACE(COMP_PHL_CMDDISP, _PHL_INFO_, "msg canceled, cur pending evt(%d)\n", pending_evt);
		return true;
	}

	if (TEST_STATUS_FLAG(obj->status, DISPR_SHALL_STOP)) {
		PHL_TRACE(COMP_PHL_CMDDISP, _PHL_INFO_, "msg canceled due to SHALL STOP status\n");

		SET_MSG_INDC_FIELD(ex->msg.msg_id, MSG_INDC_CANNOT_IO);
		SET_STATUS_FLAG(ex->status, MSG_STATUS_CANCEL);
		return true;
	}

	if (TEST_STATUS_FLAG(obj->status, DISPR_CANNOT_IO)) {
		if( is_special_msg(obj, ex)) {
			SET_MSG_INDC_FIELD(ex->msg.msg_id, MSG_INDC_CANNOT_IO);
			PHL_TRACE(COMP_PHL_CMDDISP, _PHL_INFO_, "special msg found, still sent with CANNOT IO flag set\n");
		}
		else if (!TEST_STATUS_FLAG(ex->status, MSG_STATUS_PENDING_DURING_CANNOT_IO)) {
			SET_STATUS_FLAG(ex->status, MSG_STATUS_CANCEL);
			SET_MSG_INDC_FIELD(ex->msg.msg_id, MSG_INDC_CANNOT_IO);
			PHL_TRACE(COMP_PHL_CMDDISP, _PHL_INFO_, "msg canceled due to CANNOT IO status\n");
			return true;
		} else {
			SET_STATUS_FLAG(ex->status, MSG_STATUS_PENDING);
			PHL_TRACE(COMP_PHL_CMDDISP, _PHL_INFO_, "msg pending due to CANNOT IO status\n");
		}
	}

	return false;
}

void init_dispr_msg_pool(struct cmd_dispatcher *obj)
{
	u16 i = 0;
	void *d = phl_to_drvpriv(obj->phl_info);

	if (TEST_STATUS_FLAG(obj->status, DISPR_MSGQ_INIT))
		return;
	pq_init(d, &(obj->msg_idle_q));
	pq_init(d, &(obj->msg_wait_q));
	pq_init(d, &(obj->msg_pend_q));
	_os_mem_set(d, obj->msg_ex_pool, 0,
		    sizeof(struct phl_dispr_msg_ex) * MAX_PHL_MSG_NUM);
	for (i = 0; i < MAX_PHL_MSG_NUM; i++) {
		obj->msg_ex_pool[i].idx = i;
		push_back_idle_msg(obj, &(obj->msg_ex_pool[i]));
	}

	SET_STATUS_FLAG(obj->status, DISPR_MSGQ_INIT);
}

void deinit_dispr_msg_pool(struct cmd_dispatcher *obj)
{
	void *d = phl_to_drvpriv(obj->phl_info);

	if (!TEST_STATUS_FLAG(obj->status, DISPR_MSGQ_INIT))
		return;
	CLEAR_STATUS_FLAG(obj->status, DISPR_MSGQ_INIT);

	pq_deinit(d, &(obj->msg_idle_q));
	pq_deinit(d, &(obj->msg_wait_q));
	pq_deinit(d, &(obj->msg_pend_q));
}

void cancel_msg(struct cmd_dispatcher *obj, struct phl_dispr_msg_ex *ex)
{
	void *d = phl_to_drvpriv(obj->phl_info);

	/* zero bitmap to ensure msg would not be forward to
	 * any modules after cancel.
	 * */
	_reset_bitmap(d, ex->premap, MODL_MASK_LEN);
	_reset_bitmap(d, ex->postmap, MODL_MASK_LEN);

	SET_STATUS_FLAG(ex->status, MSG_STATUS_CANCEL);
}

void cancel_running_msg(struct cmd_dispatcher *obj)
{
	u16 i = 0;

	for (i = 0; i < MAX_PHL_MSG_NUM;i++) {
		if(TEST_STATUS_FLAG(obj->msg_ex_pool[i].status, MSG_STATUS_RUN))
			cancel_msg(obj, &(obj->msg_ex_pool[i]));
	}
}
void set_msg_bitmap(struct cmd_dispatcher *obj, struct phl_dispr_msg_ex *ex, u8 mdl_id)
{
	void *d = phl_to_drvpriv(obj->phl_info);

	/* ensure mandatory & wifi role module recv all msg*/
	_os_mem_cpy(d, ex->premap, obj->bitmap, MODL_MASK_LEN);
	_os_mem_cpy(d, ex->postmap, obj->bitmap, MODL_MASK_LEN);
	if(_chk_bitmap_bit(obj->bitmap, MODL_MASK_LEN, mdl_id)) {
		_add_bitmap_bit(ex->premap, MODL_MASK_LEN, &mdl_id, 1);
		_add_bitmap_bit(ex->postmap, MODL_MASK_LEN, &mdl_id, 1);
	}
//_print_bitmap(ex->premap);
}

static void set_msg_custom_bitmap(struct cmd_dispatcher *obj, struct phl_dispr_msg_ex *ex,
		    enum phl_msg_opt opt, u8 *id_arr, u8 len, u8 mdl_id)
{
	void *d = phl_to_drvpriv(obj->phl_info);

	if (opt & MSG_OPT_SKIP_NOTIFY_OPT_MDL) {
		_os_mem_cpy(d, ex->premap, obj->basemap, MODL_MASK_LEN);
		_os_mem_cpy(d, ex->postmap, obj->basemap, MODL_MASK_LEN);
	}
	if (opt & MSG_OPT_BLIST_PRESENT) {
		_clr_bitmap_bit(ex->premap, MODL_MASK_LEN, id_arr, len);
		_clr_bitmap_bit(ex->postmap, MODL_MASK_LEN, id_arr, len);
	} else {
		_add_bitmap_bit(ex->premap, MODL_MASK_LEN, id_arr, len);
		_add_bitmap_bit(ex->postmap, MODL_MASK_LEN, id_arr, len);
	}
	if(_chk_bitmap_bit(obj->bitmap, MODL_MASK_LEN, mdl_id)) {
		_add_bitmap_bit(ex->premap, MODL_MASK_LEN, &mdl_id, 1);
		_add_bitmap_bit(ex->postmap, MODL_MASK_LEN, &mdl_id, 1);
	}
}

u8 *get_msg_bitmap(struct phl_dispr_msg_ex *ex)
{
	if (TEST_STATUS_FLAG(ex->status, MSG_STATUS_PRE_PHASE)) {
		SET_MSG_INDC_FIELD(ex->msg.msg_id, MSG_INDC_PRE_PHASE);
		return ex->premap;
	} else {
		CLEAR_MSG_INDC_FIELD(ex->msg.msg_id, MSG_INDC_PRE_PHASE);
		return ex->postmap;
	}
}


void init_dispr_mdl_mgnt_info(struct cmd_dispatcher *obj)
{
	u8 i = 0;

	for (i = 0; i < PHL_MDL_ID_MAX; i++)
		SET_CUR_PENDING_EVT(obj, i, MSG_EVT_MAX);

}

static u8 pop_front_idle_req(struct cmd_dispatcher *obj,
			     struct phl_cmd_token_req_ex **req)
{
	void *d = phl_to_drvpriv(obj->phl_info);
	_os_list *new_req = NULL;

	(*req) = NULL;
	if (pq_pop(d, &(obj->token_req_idle_q), &new_req, _first, _bh)) {
		(*req) = (struct phl_cmd_token_req_ex*)new_req;
		(*req)->status = 0;
		_os_mem_set(d, &((*req)->req), 0,
			    sizeof(struct phl_cmd_token_req));
		_os_mem_set(d, &((*req)->add_req_info), 0,
			    sizeof(struct phl_token_op_info));
		_os_mem_set(d, &((*req)->free_req_info), 0,
			    sizeof(struct phl_token_op_info));
		return true;
	} else {
		return false;
	}
}

static void push_back_idle_req(struct cmd_dispatcher *obj,
			       struct phl_cmd_token_req_ex *req)
{
	void *d = phl_to_drvpriv(obj->phl_info);

	req->status = 0;
	SET_CUR_PENDING_EVT(obj, req->req.module_id, MSG_EVT_MAX);
	pq_push(d, &(obj->token_req_idle_q), &(req->list), _tail, _bh);
}

static u8 pop_front_wait_req(struct cmd_dispatcher *obj,
			     struct phl_cmd_token_req_ex **req)
{
	void *d = phl_to_drvpriv(obj->phl_info);
	_os_list *new_req = NULL;

	(*req) = NULL;
	if (pq_pop(d, &(obj->token_req_wait_q), &new_req, _first, _bh)) {
		(*req) = (struct phl_cmd_token_req_ex*)new_req;
		SET_STATUS_FLAG((*req)->status, REQ_STATUS_PREPARE);
		CLEAR_STATUS_FLAG((*req)->status, REQ_STATUS_ENQ);
		return true;
	} else {
		return false;
	}
}

static u8 front_req_with_exclusive_opt(struct cmd_dispatcher *obj)
{
	void *d = phl_to_drvpriv(obj->phl_info);
	struct phl_cmd_token_req_ex *req = NULL;
	_os_list *new_req = NULL;

	if (pq_get_front(d, &(obj->token_req_wait_q), &new_req, _bh)) {
		req = (struct phl_cmd_token_req_ex*)new_req;
		if (req->req.opt == FG_CMD_OPT_EXCLUSIVE)
			return true;
	}
	return false;
}
static void push_back_wait_req(struct cmd_dispatcher *obj,
			       struct phl_cmd_token_req_ex *req)
{
	void *d = phl_to_drvpriv(obj->phl_info);

	pq_push(d, &(obj->token_req_wait_q), &(req->list), _tail, _bh);
	SET_STATUS_FLAG(req->status, REQ_STATUS_ENQ);
}

static void clear_wating_req(struct cmd_dispatcher *obj)
{
	 struct phl_cmd_token_req_ex *ex = NULL;

	PHL_TRACE(COMP_PHL_CMDDISP, _PHL_INFO_, "%s[%d]: remain cnt(%d)\n",
	          __func__, obj->idx, obj->token_req_idle_q.cnt);
	while(obj->token_req_idle_q.cnt != MAX_CMD_REQ_NUM) {
		while (pop_front_wait_req(obj, &ex)) {
			ex->req.abort(obj, ex->req.priv);
			push_back_idle_req(obj, ex);
		}
	}
}

void deregister_cur_cmd_req(struct cmd_dispatcher *obj, u8 notify)
{
	struct phl_cmd_token_req *req = NULL;
	void *d = phl_to_drvpriv(obj->phl_info);
	u16 i = 0;
	struct phl_dispr_msg_ex *ex = NULL;

	if (obj->cur_cmd_req) {
		req = &(obj->cur_cmd_req->req);
		PHL_TRACE(COMP_PHL_CMDDISP, _PHL_INFO_, "%s[%d], id(%d), status(%d)\n",
		          __func__, obj->idx, req->module_id, obj->cur_cmd_req->status);
		CLEAR_STATUS_FLAG(obj->cur_cmd_req->status, REQ_STATUS_RUN);
		for (i = 0; i < MAX_PHL_MSG_NUM; i++) {
			ex = &(obj->msg_ex_pool[i]);
			if (req->module_id != MSG_MDL_ID_FIELD(ex->msg.msg_id))
				continue;
			CLEAR_STATUS_FLAG(ex->status, MSG_STATUS_OWNER_REQ);
			cancel_msg(obj, ex);
			if(TEST_STATUS_FLAG(ex->status, MSG_STATUS_PENDING)) {
				dispr_clr_pending_msg((void*)obj);
				/* inserted pending msg from this sepecific sender back to wait Q before abort notify
				 * would guarantee msg sent in abort notify is exactly last msg from this sender
				 * */
				clear_pending_msg(obj);
			}
		}
		if (notify == true) {
			SET_STATUS_FLAG(obj->cur_cmd_req->status, REQ_STATUS_LAST_PERMIT);
			req->abort(obj, req->priv);
			CLEAR_STATUS_FLAG(obj->cur_cmd_req->status, REQ_STATUS_LAST_PERMIT);
		}
#ifdef CONFIG_CMD_DISP_SUPPORT_CUSTOM_SEQ
		SET_MDL_HANDLE(obj, obj->cur_cmd_req->req.module_id, NULL);
#endif
		push_back_idle_req(obj, obj->cur_cmd_req);
		_os_atomic_set(d, &(obj->token_cnt),
			       _os_atomic_read(d, &(obj->token_cnt))-1);
	}
	obj->cur_cmd_req = NULL;

	if (!TEST_STATUS_FLAG(obj->status, DISPR_WAIT_ABORT_MSG_DONE)) {
		cmd_token_complete(obj);
	}
	PHL_TRACE(COMP_PHL_CMDDISP, _PHL_INFO_, "%s[%d]\n", __func__, obj->idx);
}

u8 register_cur_cmd_req(struct cmd_dispatcher *obj,
			  struct phl_cmd_token_req_ex *req)
{
	void *d = phl_to_drvpriv(obj->phl_info);
	enum phl_mdl_ret_code ret = MDL_RET_SUCCESS;

	SET_STATUS_FLAG(req->status, REQ_STATUS_RUN);
	CLEAR_STATUS_FLAG(req->status, REQ_STATUS_PREPARE);
	obj->cur_cmd_req = req;
	_os_atomic_set(d, &(obj->token_cnt),
		       _os_atomic_read(d, &(obj->token_cnt))+1);
	PHL_TRACE(COMP_PHL_CMDDISP, _PHL_INFO_, "%s[%d], id(%d)\n",
	          __func__, obj->idx, obj->cur_cmd_req->req.module_id);
	ret = obj->cur_cmd_req->req.acquired((void*)obj, obj->cur_cmd_req->req.priv);
	PHL_TRACE(COMP_PHL_CMDDISP, _PHL_INFO_, "%s[%d], ret(%d)\n", __func__, obj->idx, ret);

	if (ret == MDL_RET_FAIL) {
		deregister_cur_cmd_req(obj, false);
		return false;
	}
	else {
#ifdef CONFIG_CMD_DISP_SUPPORT_CUSTOM_SEQ
		SET_MDL_HANDLE(obj, req->req.module_id, req);
#endif
		return true;
	}
}

void cancel_all_cmd_req(struct cmd_dispatcher *obj)
{
	u8 i = 0;
	struct phl_cmd_token_req_ex* req_ex = NULL;

	for (i = 0; i < MAX_CMD_REQ_NUM;i++) {
		req_ex = &(obj->token_req_ex_pool[i]);
		if (req_ex->status)
			SET_STATUS_FLAG(req_ex->status, REQ_STATUS_CANCEL);
	}
}

void init_cmd_req_pool(struct cmd_dispatcher *obj)
{
	u8 i = 0;
	void *d = phl_to_drvpriv(obj->phl_info);

	if (TEST_STATUS_FLAG(obj->status, DISPR_REQ_INIT))
		return;
	pq_init(d, &(obj->token_req_wait_q));
	pq_init(d, &(obj->token_req_idle_q));
	pq_init(d, &(obj->token_op_q));
	_os_mem_set(d, obj->token_req_ex_pool, 0,
		    sizeof(struct phl_cmd_token_req_ex) * MAX_CMD_REQ_NUM);
	for (i = 0; i < MAX_CMD_REQ_NUM;i++) {
		obj->token_req_ex_pool[i].idx = i;
		pq_push(d, &(obj->token_req_idle_q),
			&(obj->token_req_ex_pool[i].list), _tail, _bh);
	}
	SET_STATUS_FLAG(obj->status, DISPR_REQ_INIT);
}

void deinit_cmd_req_pool(struct cmd_dispatcher *obj)
{
	void *d = phl_to_drvpriv(obj->phl_info);

	CLEAR_STATUS_FLAG(obj->status, DISPR_REQ_INIT);

	pq_deinit(d, &(obj->token_req_wait_q));
	pq_deinit(d, &(obj->token_req_idle_q));
	pq_deinit(d, &(obj->token_op_q));
}

u8 chk_module_ops(struct phl_bk_module_ops *ops)
{
	if (ops == NULL ||
	    ops->init == NULL ||
	    ops->deinit == NULL ||
	    ops->msg_hdlr == NULL ||
	    ops->set_info == NULL ||
	    ops->query_info == NULL ||
	    ops->start == NULL ||
	    ops->stop == NULL)
		return false;
	return true;
}

u8 chk_cmd_req_ops(struct phl_cmd_token_req *req)
{
	if (req == NULL ||
	    req->module_id < PHL_FG_MDL_START ||
	    req->abort == NULL ||
	    req->acquired == NULL ||
	    req->msg_hdlr == NULL ||
	    req->set_info == NULL ||
	    req->query_info == NULL)
		return false;
	return true;
}
static u8 pop_front_token_op_info(struct cmd_dispatcher *obj,
				  struct phl_token_op_info **op_info)
{
	void *d = phl_to_drvpriv(obj->phl_info);
	_os_list *new_info = NULL;

	(*op_info) = NULL;
	if (pq_pop(d, &(obj->token_op_q), &new_info, _first, _bh)) {
		(*op_info) = (struct phl_token_op_info *)new_info;
		return true;
	} else {
		return false;
	}
}

static u8 push_back_token_op_info(struct cmd_dispatcher *obj,
				  struct phl_token_op_info *op_info,
				  enum token_op_type type,
			    	  u8 data)
{
	void *d = phl_to_drvpriv(obj->phl_info);
	_os_spinlockfg sp_flags;

	_os_spinlock(d, &obj->token_op_q_lock, _bh, &sp_flags);
	if (op_info->used == true) {
		_os_spinunlock(d, &obj->token_op_q_lock, _bh, &sp_flags);
		return false;
	}
	op_info->used = true;
	op_info->type = type;
	op_info->data = data;
	_os_spinunlock(d, &obj->token_op_q_lock, _bh, &sp_flags);
	pq_push(d, &(obj->token_op_q), &(op_info->list), _tail, _bh);
	notify_dispr_thread(obj);
	return true;
}

void _handle_token_op_info(struct cmd_dispatcher *obj, struct phl_token_op_info *op_info)
{
	struct phl_cmd_token_req_ex *req_ex = NULL;
	void *d = phl_to_drvpriv(obj->phl_info);

	switch (op_info->type) {
		case TOKEN_OP_RENEW_CMD_REQ:
			/* fall through*/
		case TOKEN_OP_ADD_CMD_REQ:
			dispr_process_token_req(obj);
			break;
		case TOKEN_OP_FREE_CMD_REQ:
			if (op_info->data >= MAX_CMD_REQ_NUM)
				return;
			req_ex = &(obj->token_req_ex_pool[op_info->data]);
			if (!TEST_STATUS_FLAG(req_ex->status, REQ_STATUS_RUN))
				break;
			deregister_cur_cmd_req(obj, false);
			dispr_process_token_req(obj);
			break;
		case TOKEN_OP_CANCEL_CMD_REQ:
			if (op_info->data >= MAX_CMD_REQ_NUM)
				return;
			req_ex = &(obj->token_req_ex_pool[op_info->data]);
			SET_STATUS_FLAG(req_ex->status, REQ_STATUS_CANCEL);
			if (TEST_STATUS_FLAG(req_ex->status, REQ_STATUS_ENQ)) {
				pq_del_node(d, &(obj->token_req_wait_q), &(req_ex->list), _bh);
				/*
				 * Call command abort handle, abort handle
				 * should decide it has been acquired or not.
				 */
				req_ex->req.abort(obj, req_ex->req.priv);
				push_back_idle_req(obj, req_ex);
			} else if (TEST_STATUS_FLAG(req_ex->status, REQ_STATUS_RUN)){
				deregister_cur_cmd_req(obj, true);
				dispr_process_token_req(obj);
			}
			break;
		default:
			break;
	}
}

void token_op_hanler(struct cmd_dispatcher *obj)
{
	struct phl_token_op_info *info = NULL;
	void *d = phl_to_drvpriv(obj->phl_info);
	u8 ntfyidx = HW_BAND_0;

	/* dispr1 might have no token cmd. */
	if (TEST_STATUS_FLAG(obj->status, DISPR_CLEARANCE)) {
		if (!obj->token_op_q.cnt && //!obj->token_req_wait_q.cnt &&
		    _os_atomic_read(d, &(obj->token_cnt)) == 0) {
			if(obj->idx == HW_BAND_0)
				ntfyidx = HW_BAND_1;
			phl_disp_eng_exclusive_ready(obj->phl_info, ntfyidx);
			return;
		}
	}

	while (pop_front_token_op_info(obj, &info)) {
		_handle_token_op_info(obj, info);
		info->used = false;
	}
}
static u8
dispr_enqueue_token_op_info(struct cmd_dispatcher *obj,
			    struct phl_token_op_info *op_info,
			    enum token_op_type type,
			    u8 data)
{
	return push_back_token_op_info(obj, op_info, type, data);
}

u8 bk_module_init(struct cmd_dispatcher *obj, struct phl_bk_module *module)
{
	if (TEST_STATUS_FLAG(module->status, MDL_INIT)) {
		PHL_TRACE(COMP_PHL_CMDDISP, _PHL_ERR_, "%s[%d] module_id:%d already init\n",
		          __func__, obj->idx, module->id);
		return false;
	}

	if (module->ops.init((void*)obj->phl_info, (void*)obj,
			     &(module->priv)) == MDL_RET_SUCCESS) {
		SET_STATUS_FLAG(module->status, MDL_INIT);
		return true;
	} else {
		PHL_TRACE(COMP_PHL_CMDDISP, _PHL_ERR_,
			"%s[%d] fail module_id: %d \n", __func__, obj->idx, module->id);
		return false;
	}
}

void bk_module_deinit(struct cmd_dispatcher *obj, struct phl_bk_module *module)
{
	if (TEST_STATUS_FLAG(module->status, MDL_INIT))
		module->ops.deinit((void*)obj, module->priv);
	CLEAR_STATUS_FLAG(module->status, MDL_INIT);
}

u8 bk_module_start(struct cmd_dispatcher *obj, struct phl_bk_module *module)
{
	if (!TEST_STATUS_FLAG(module->status, MDL_INIT) ||
	    TEST_STATUS_FLAG(module->status, MDL_STARTED)) {
		PHL_TRACE(COMP_PHL_CMDDISP, _PHL_ERR_, "%s[%d] module_id:%d already start\n",
		          __func__, obj->idx, module->id);
		return false;
	}

	if (module->ops.start((void*)obj, module->priv) == MDL_RET_SUCCESS) {
		SET_STATUS_FLAG(module->status, MDL_STARTED);
#ifdef CONFIG_CMD_DISP_SUPPORT_CUSTOM_SEQ
		SET_MDL_HANDLE(obj, module->id, module);
#endif
		return true;
	} else {
		PHL_TRACE(COMP_PHL_CMDDISP, _PHL_ERR_,
			"%s[%d] fail module_id: %d \n", __func__, obj->idx, module->id);
		return false;
	}
}

u8 bk_module_stop(struct cmd_dispatcher *obj, struct phl_bk_module *module)
{
	if (!TEST_STATUS_FLAG(module->status, MDL_STARTED))
		return false;
	CLEAR_STATUS_FLAG(module->status, MDL_STARTED);
#ifdef CONFIG_CMD_DISP_SUPPORT_CUSTOM_SEQ
	SET_MDL_HANDLE(obj, module->id, NULL);
#endif
	if (module->ops.stop((void*)obj, module->priv) == MDL_RET_SUCCESS) {
		return true;
	} else {
		PHL_TRACE(COMP_PHL_CMDDISP, _PHL_ERR_, "%s[%d] fail module_id: %d \n",
		          __func__, obj->idx, module->id);
		return false;
	}
}

void cur_req_hdl(struct cmd_dispatcher *obj, struct phl_dispr_msg_ex *ex)
{
	struct phl_cmd_token_req_ex *cur_req = obj->cur_cmd_req;

	if (cur_req == NULL)
		return;
	if (!TEST_STATUS_FLAG(cur_req->status, REQ_STATUS_RUN) ||
	    TEST_STATUS_FLAG(cur_req->status, REQ_STATUS_CANCEL))
		return;
	if (TEST_STATUS_FLAG(ex->status, MSG_STATUS_FOR_ABORT))
		return;
	cur_req->req.msg_hdlr((void*)obj, cur_req->req.priv, &(ex->msg));
}

void notify_msg_fail(struct cmd_dispatcher *obj,
                     struct phl_dispr_msg_ex *ex,
                     enum phl_mdl_ret_code ret)
{
	PHL_TRACE(COMP_PHL_CMDDISP, _PHL_INFO_, "%s[%d]\n", __func__, obj->idx);

	SET_STATUS_FLAG(ex->status, MSG_STATUS_FAIL);

	SET_MSG_INDC_FIELD(ex->msg.msg_id, MSG_INDC_FAIL);
	if (ret == MDL_RET_CANNOT_IO)
		SET_MSG_INDC_FIELD(ex->msg.msg_id, MSG_INDC_CANNOT_IO);

	if (TEST_STATUS_FLAG(ex->status, MSG_STATUS_OWNER_BK_MDL) &&
	   (IS_DISPR_CTRL(MSG_MDL_ID_FIELD(ex->msg.msg_id)) ||
	    _chk_bitmap_bit(obj->bitmap, MODL_MASK_LEN, ex->module->id))) {

		ex->module->ops.msg_hdlr(obj, ex->module->priv, &(ex->msg));
	}

	if (TEST_STATUS_FLAG(ex->status, MSG_STATUS_OWNER_REQ)) {
		cur_req_hdl(obj, ex);
	}
}

enum phl_mdl_ret_code feed_mdl_msg(struct cmd_dispatcher *obj,
				   struct phl_bk_module *mdl,
				   struct phl_dispr_msg_ex *ex)
{
	enum phl_mdl_ret_code ret = MDL_RET_FAIL;
	u8 *bitmap = NULL;

	PHL_TRACE(COMP_PHL_CMDDISP, _PHL_DEBUG_, "%s[%d], id:%d \n", __func__, obj->idx, mdl->id);
	ret = mdl->ops.msg_hdlr(obj, mdl->priv, &(ex->msg));
	if (ret == MDL_RET_FAIL || ret == MDL_RET_CANNOT_IO) {
		PHL_TRACE(COMP_PHL_CMDDISP, _PHL_INFO_, "id:%d evt:0x%x fail\n",
			 mdl->id, ex->msg.msg_id);
		ex->msg.rsvd[0].ptr = mdl;
		notify_msg_fail(obj, ex, ret);
	} else if (ret == MDL_RET_PENDING) {
		PHL_TRACE(COMP_PHL_CMDDISP, _PHL_INFO_, "id:%d evt:0x%x pending\n",
			 mdl->id, ex->msg.msg_id);
		SET_STATUS_FLAG(ex->status, MSG_STATUS_PENDING);
	} else {
		if (MSG_INDC_FIELD(ex->msg.msg_id) & MSG_INDC_PRE_PHASE)
			bitmap = ex->premap;
		else
			bitmap = ex->postmap;
		_clr_bitmap_bit(bitmap, MODL_MASK_LEN, &(mdl->id), 1);
	}
	return ret;
}

void msg_pre_phase_hdl(struct cmd_dispatcher *obj, struct phl_dispr_msg_ex *ex)
{
	s8 i = 0;
	void *d = phl_to_drvpriv(obj->phl_info);
	struct phl_bk_module *mdl = NULL;
	_os_list *node = NULL;
	struct phl_queue *q = NULL;
	enum phl_mdl_ret_code ret = MDL_RET_FAIL;
	u8 owner_id = (ex->module)?(ex->module->id):(PHL_MDL_ID_MAX);
	enum phl_bk_module_priority priority = PHL_MDL_PRI_MAX;

	if (owner_id < PHL_BK_MDL_END)
		priority = _get_mdl_priority(owner_id);

	for (i = PHL_MDL_PRI_MAX - 1 ; i >= PHL_MDL_PRI_ROLE ; i--) {
#ifdef CONFIG_CMD_DISP_SUPPORT_CUSTOM_SEQ
		ret = run_self_def_seq(obj, ex, i, true);
		if (STOP_DISPATCH_MSG(ret))
			return;
#endif
		if (priority == i && _chk_bitmap_bit(ex->premap, MODL_MASK_LEN, owner_id)) {
			ret = feed_mdl_msg(obj, ex->module, ex);
			if (STOP_DISPATCH_MSG(ret))
				return;
		}
		q = &(obj->module_q[(u8)i]);

		if (pq_get_front(d, q, &node, _bh) == false)
			continue;

		do {
			mdl = (struct phl_bk_module*)node;
			if (!_chk_bitmap_bit(ex->premap, MODL_MASK_LEN, mdl->id) ||
			    !TEST_STATUS_FLAG(mdl->status, MDL_STARTED))
				continue;
			ret = feed_mdl_msg(obj, mdl, ex);
			if (STOP_DISPATCH_MSG(ret))
				return;
		} while(pq_get_next(d, q, node, &node, _bh));
	}
}

void msg_post_phase_hdl(struct cmd_dispatcher *obj, struct phl_dispr_msg_ex *ex)
{
	s8 i = 0;
	void *d = phl_to_drvpriv(obj->phl_info);
	struct phl_bk_module *mdl = NULL;
	_os_list *node = NULL;
	struct phl_queue *q = NULL;
	enum phl_mdl_ret_code ret = MDL_RET_FAIL;
	u8 owner_id = (ex->module)?(ex->module->id):(PHL_MDL_ID_MAX);
	enum phl_bk_module_priority priority = PHL_MDL_PRI_MAX;

	if (owner_id < PHL_BK_MDL_END)
		priority = _get_mdl_priority(owner_id);

	for (i = PHL_MDL_PRI_ROLE ; i < PHL_MDL_PRI_MAX ; i++) {
#ifdef CONFIG_CMD_DISP_SUPPORT_CUSTOM_SEQ
		ret = run_self_def_seq(obj, ex, i, false);
		if (STOP_DISPATCH_MSG(ret))
			return;
#endif
		if (priority == i && _chk_bitmap_bit(ex->postmap, MODL_MASK_LEN, owner_id)) {
			ret = feed_mdl_msg(obj, ex->module, ex);
			if (STOP_DISPATCH_MSG(ret))
				return;
		}
		q = &(obj->module_q[(u8)i]);
		if (pq_get_tail(d, q, &node, _bh) == false)
			continue;
		do {
			mdl = (struct phl_bk_module*)node;
			if (!_chk_bitmap_bit(ex->postmap, MODL_MASK_LEN, mdl->id)||
			    !TEST_STATUS_FLAG(mdl->status, MDL_STARTED))
				continue;
			ret = feed_mdl_msg(obj, mdl, ex);
			if (STOP_DISPATCH_MSG(ret))
				return;
		} while(pq_get_prev(d, q, node, &node, _bh));
	}
}

u8 get_cur_cmd_req_id(struct cmd_dispatcher *obj, u32 *req_status)
{
	struct phl_cmd_token_req_ex *cur_req = obj->cur_cmd_req;

	if(req_status)
		*req_status = 0;

	if (cur_req == NULL)
		return (u8)PHL_MDL_ID_MAX;

	if(req_status)
		*req_status = cur_req->status;

	if(!TEST_STATUS_FLAG(cur_req->status, REQ_STATUS_RUN) ||
	   TEST_STATUS_FLAG(cur_req->status, REQ_STATUS_CANCEL))
		return (u8)PHL_MDL_ID_MAX;
	else
		return cur_req->req.module_id;
}

#define MSG_REDIRECT_CHK(_ex) \
	if (TEST_STATUS_FLAG(ex->status, MSG_STATUS_FAIL)|| \
	    TEST_STATUS_FLAG(ex->status, MSG_STATUS_CANCEL)) \
		goto recycle;\
	if (TEST_STATUS_FLAG(ex->status, MSG_STATUS_PENDING)) \
		goto reschedule;

void msg_dispatch(struct cmd_dispatcher *obj, struct phl_dispr_msg_ex *ex)
{
	u8 *bitmap = get_msg_bitmap(ex);
	void *d = phl_to_drvpriv(obj->phl_info);

	PHL_TRACE(COMP_PHL_CMDDISP, _PHL_DEBUG_, "%s[%d], msg_id:0x%x status: 0x%x\n",
	          __func__, obj->idx, ex->msg.msg_id, ex->status);
	MSG_REDIRECT_CHK(ex);

	_notify_dispr_controller(obj, ex);

	MSG_REDIRECT_CHK(ex);

	if ((MSG_INDC_FIELD(ex->msg.msg_id) & MSG_INDC_PRE_PHASE) &&
	    _is_bitmap_empty(d, bitmap) == false) {
		msg_pre_phase_hdl(obj, ex);

		MSG_REDIRECT_CHK(ex);

		if (_is_bitmap_empty(d, bitmap)) {
			/* pre protocol phase done, switch to post protocol phase*/
			CLEAR_STATUS_FLAG(ex->status, MSG_STATUS_PRE_PHASE);
			bitmap = get_msg_bitmap(ex);
		} else {
			PHL_ERR("%s[%d], invalid bitmap state, msg status:0x%x \n",
			        __func__, obj->idx, ex->status);
			SET_STATUS_FLAG(ex->status, MSG_STATUS_CANCEL);
			goto recycle;
		}
	}
	if (_is_bitmap_empty(d, bitmap) == false)
		msg_post_phase_hdl(obj, ex);

	MSG_REDIRECT_CHK(ex);

	if (_is_bitmap_empty(d, bitmap)) {
		/* post protocol phase done */
		cur_req_hdl(obj, ex);
		goto recycle;
	} else {
		PHL_ERR("%s[%d], invalid bitmap state, msg status:0x%x \n",
		        __func__, obj->idx, ex->status);
		SET_STATUS_FLAG(ex->status, MSG_STATUS_CANCEL);
		goto recycle;
	}
reschedule:
	PHL_TRACE(COMP_PHL_CMDDISP, _PHL_INFO_, "%s[%d], msg:0x%x reschedule \n",
	          __func__, obj->idx, ex->msg.msg_id);
	if(TEST_STATUS_FLAG(ex->status, MSG_STATUS_PENDING))
		push_back_pending_msg(obj, ex);
	else
		push_back_wait_msg(obj, ex);
	return;
recycle:
	PHL_TRACE(COMP_PHL_CMDDISP, _PHL_DEBUG_, "%s[%d], msg:0x%x recycle \n",
	          __func__, obj->idx, ex->msg.msg_id);
	push_back_idle_msg(obj, ex);
}

void dispr_thread_loop_hdl(struct cmd_dispatcher *obj)
{
	struct phl_dispr_msg_ex *ex = NULL;

	/* check pending msg need in advance.
	* if pending msg is not empty before while loop breaks,
	* these msg would be cleared in deinit_dispr_msg_pool.
	*/
	clear_pending_msg(obj);
	/* token op Q in advance.
	* if req wait Q is not empty before while loop breaks,
	* these msg would be cleared in deinit_cmd_req_pool.
	*/
	token_op_hanler(obj);

	if (pop_front_wait_msg(obj, &ex)) {
		if (is_msg_canceled(obj, ex)) {
			push_back_idle_msg(obj, ex);
			return;
		}
		/* ensure all modules set in msg bitmap
			exists in cur dispatcher*/
		_and_bitmaps(obj->bitmap, ex->premap, MODL_MASK_LEN);
		_and_bitmaps(obj->bitmap, ex->postmap, MODL_MASK_LEN);
		msg_dispatch(obj, ex);
	}
}

void dispr_thread_leave_hdl(struct cmd_dispatcher *obj)
{
	deregister_cur_cmd_req(obj, true);
	/* clear remaining pending & waiting msg */
	clear_waiting_msg(obj);
	/* pop out all waiting cmd req and notify abort. */
	clear_wating_req(obj);
}

#ifdef CONFIG_CMD_DISP_SOLO_MODE
int background_thread_hdl(void *param)
{
	struct cmd_dispatcher *obj = (struct cmd_dispatcher *)param;
	void *d = phl_to_drvpriv(obj->phl_info);

	PHL_TRACE(COMP_PHL_CMDDISP, _PHL_INFO_, "%s[%d] enter\n", __func__, obj->idx);
	while (!_os_thread_check_stop(d, &(obj->bk_thread))) {

		_os_sema_down(d, &obj->msg_q_sema);

		if(_os_thread_check_stop(d, &(obj->bk_thread)))
			break;
		dispr_thread_loop_hdl(obj);
	}
	dispr_thread_leave_hdl(obj);
	_os_thread_wait_stop(d, &(obj->bk_thread));
	PHL_TRACE(COMP_PHL_CMDDISP, _PHL_INFO_, "%s[%d] down\n", __func__, obj->idx);
	return 0;
}
#endif
u8 search_mdl(void *d, void *mdl, void *priv)
{
	enum phl_module_id id = *(enum phl_module_id *)priv;
	struct phl_bk_module *module = NULL;

	module = (struct phl_bk_module *)mdl;
	if (module->id == id) {
		PHL_TRACE(COMP_PHL_CMDDISP, _PHL_DEBUG_, "%s:: id %d\n", __func__, id);
		return true;
	}
	else
		return false;
}

u8 get_module_by_id(struct cmd_dispatcher *obj, enum phl_module_id id,
		    struct phl_bk_module **mdl)
{
	void *d = phl_to_drvpriv(obj->phl_info);
	u8 i = 0;
	_os_list *node = NULL;

	if (mdl == NULL)
		return false;

	if (IS_DISPR_CTRL(id)) {
		if (!TEST_STATUS_FLAG(obj->status, DISPR_CTRL_PRESENT))
			return false;
		*mdl = &(obj->controller);
		return true;
	}

	if (!_chk_bitmap_bit(obj->bitmap, MODL_MASK_LEN, id))
		return false;

	for (i = 0; i < PHL_MDL_PRI_MAX; i++) {

		if(pq_search_node(d, &(obj->module_q[i]), &node, _bh, false, &id, search_mdl)) {
			*mdl = (struct phl_bk_module*)node;
			return true;
		}
	}
	*mdl = NULL;
	return false;
}

enum rtw_phl_status phl_dispr_get_idx(void *dispr, u8 *idx)
{
	struct cmd_dispatcher *obj = (struct cmd_dispatcher *)dispr;

	if (dispr == NULL)
		return RTW_PHL_STATUS_FAILURE;
	if (!TEST_STATUS_FLAG(obj->status, DISPR_INIT) || idx == NULL)
		return RTW_PHL_STATUS_FAILURE;
	*idx = obj->idx;
	return RTW_PHL_STATUS_SUCCESS;
}

/* Each dispr has a controller.
 * A dispr controller is designed for phl instance to interact with dispr modules that are belonged to a specific hw band,
 * phl instance can perform follwing actions via dedicated controller:
 * 1. allow (phl status/non-dispr phl modules) to monitor & drop msg
 * 2. allow dispr modules, that are belonged to same dispr, to sequentially communicate with phl instance & call phl api,
 *    and also allow (phl status/non-dispr phl modules) to notify dispr by hw band.
 * *Note*
 * 1. when cmd dispatch engine is in solo thread mode (each dispr has its own dedicated thread).
 *    phl instance might receive msg from different dispr simutaneously and
 *    currently using semaphore (dispr_ctrl_sema) to prevent multi-thread condition.
 * 2. when cmd dispatch engine is in share thread mode, msg from different dispr would pass to controller sequentially.

 * PS:
 * phl instance: means phl_info_t, which include phl mgnt status & non-dispr phl modules
 * dispr modules: all existing background & foreground modules.
 * non-dispr phl module : Data path (TX/Rx), etc
 * phl mgnt status : stop/surprise remove/cannot io
*/
static enum rtw_phl_status _register_dispr_controller(struct cmd_dispatcher *obj)
{
	struct phl_bk_module *ctrl = &(obj->controller);

	dispr_ctrl_hook_ops(obj, &(ctrl->ops));
	ctrl->id = PHL_MDL_PHY_MGNT;

	if(bk_module_init(obj, &(obj->controller)) == true)
		return RTW_PHL_STATUS_SUCCESS;
	else {
		PHL_TRACE(COMP_PHL_CMDDISP, _PHL_ERR_, "%s(): fail \n", __func__);
		return RTW_PHL_STATUS_FAILURE;
	}
}

static void _deregister_dispr_controller(struct cmd_dispatcher *obj)
{
	bk_module_deinit(obj, &(obj->controller));
}

static enum rtw_phl_status _start_dispr_controller(struct cmd_dispatcher *obj)
{
	if (bk_module_start(obj, &(obj->controller)) == true) {
		SET_STATUS_FLAG(obj->status, DISPR_CTRL_PRESENT);
		return RTW_PHL_STATUS_SUCCESS;
	}
	else {
		PHL_TRACE(COMP_PHL_CMDDISP, _PHL_ERR_, "%s(): fail \n", __func__);
		return RTW_PHL_STATUS_FAILURE;
	}
}

static enum rtw_phl_status _stop_dispr_controller(struct cmd_dispatcher *obj)
{
	CLEAR_STATUS_FLAG(obj->status, DISPR_CTRL_PRESENT);
	if (bk_module_stop(obj, &(obj->controller)) == true)
		return RTW_PHL_STATUS_SUCCESS;
	else {
		PHL_TRACE(COMP_PHL_CMDDISP, _PHL_ERR_, "%s(): fail \n", __func__);
		return RTW_PHL_STATUS_FAILURE;
	}
}

void _notify_dispr_controller(struct cmd_dispatcher *obj, struct phl_dispr_msg_ex *ex)
{
	if (!TEST_STATUS_FLAG(obj->status, DISPR_CTRL_PRESENT))
		return;
#ifdef CONFIG_CMD_DISP_SOLO_MODE
	dispr_ctrl_sema_down(obj->phl_info);
#endif
	feed_mdl_msg(obj, &(obj->controller), ex);
#ifdef CONFIG_CMD_DISP_SOLO_MODE
	dispr_ctrl_sema_up(obj->phl_info);
#endif

}

void dispr_thread_stop_prior_hdl(struct cmd_dispatcher *obj)
{
	CLEAR_STATUS_FLAG(obj->status, DISPR_STARTED);
	_stop_dispr_controller(obj);
	cancel_all_cmd_req(obj);
	cancel_running_msg(obj);
}

void dispr_thread_stop_post_hdl(struct cmd_dispatcher *obj)
{
	void *d = phl_to_drvpriv(obj->phl_info);

	/* have to wait for bk thread ends before deinit msg & req*/
	deinit_dispr_msg_pool(obj);
	deinit_cmd_req_pool(obj);
	_os_atomic_set(d, &(obj->token_cnt), 0);
	#ifdef CONFIG_CMD_DISP_SOLO_MODE
	_os_sema_free(d, &(obj->msg_q_sema));
	#endif
}

enum rtw_phl_status dispr_init(struct phl_info_t *phl_info, void **dispr, u8 idx)
{
	struct cmd_dispatcher *obj = NULL;
	void *d = phl_to_drvpriv(phl_info);
	u8 i = 0;

	(*dispr) = NULL;

	obj = (struct cmd_dispatcher *)_os_mem_alloc(d, sizeof(struct cmd_dispatcher));
	if (obj == NULL) {
		PHL_TRACE(COMP_PHL_CMDDISP, _PHL_ERR_, "%s[%d], alloc fail\n", __func__, obj->idx);
		return RTW_PHL_STATUS_RESOURCE;
	}

	obj->phl_info = phl_info;
	obj->idx = idx;
	_os_atomic_set(d, &(obj->token_cnt), 0);
	for (i = 0 ; i < PHL_MDL_PRI_MAX; i++)
		pq_init(d, &(obj->module_q[i]));

	(*dispr) = (void*)obj;
	_os_spinlock_init(d, &(obj->token_op_q_lock));
	SET_STATUS_FLAG(obj->status, DISPR_INIT);
	SET_STATUS_FLAG(obj->status, DISPR_NOTIFY_IDLE);
	_register_dispr_controller(obj);
	PHL_TRACE(COMP_PHL_CMDDISP, _PHL_INFO_, "%s[%d], size dispr(%d), msg_ex(%d), req_ex(%d) \n",
		 __func__, obj->idx, (int)sizeof(struct cmd_dispatcher),
		 (int)sizeof(struct phl_dispr_msg_ex),
		 (int)sizeof(struct phl_cmd_token_req_ex));
	return RTW_PHL_STATUS_SUCCESS;
}

enum rtw_phl_status dispr_deinit(struct phl_info_t *phl, void *dispr)
{
	struct cmd_dispatcher *obj = (struct cmd_dispatcher *)dispr;
	void *d = phl_to_drvpriv(obj->phl_info);
	u8 i = 0;

	if (!TEST_STATUS_FLAG(obj->status, DISPR_INIT))
		return RTW_PHL_STATUS_SUCCESS;
	dispr_stop(dispr);
	_deregister_dispr_controller(obj);
	for (i = 0 ; i < PHL_MDL_PRI_MAX; i++)
		pq_deinit(d, &(obj->module_q[i]));
	_os_spinlock_free(d, &(obj->token_op_q_lock));
	_os_mem_free(d, obj, sizeof(struct cmd_dispatcher));
	PHL_TRACE(COMP_PHL_CMDDISP, _PHL_INFO_, "%s[%d]\n", __func__, obj->idx);
	return RTW_PHL_STATUS_SUCCESS;
}

enum rtw_phl_status dispr_start(void *dispr)
{
	struct cmd_dispatcher *obj = (struct cmd_dispatcher *)dispr;
	void *d = phl_to_drvpriv(obj->phl_info);

	if (TEST_STATUS_FLAG(obj->status, DISPR_STARTED))
		return RTW_PHL_STATUS_UNEXPECTED_ERROR;

	init_dispr_msg_pool(obj);
	init_cmd_req_pool(obj);
	init_dispr_mdl_mgnt_info(obj);
	_os_mem_set(d, &(obj->renew_req_info), 0,
			    sizeof(struct phl_token_op_info));
	CLEAR_EXCL_MDL(obj);
	#ifdef CONFIG_CMD_DISP_SOLO_MODE
	_os_sema_init(d, &(obj->msg_q_sema), 0);
	if (disp_eng_is_solo_thread_mode(obj->phl_info)) {
		_os_thread_init(d, &(obj->bk_thread), background_thread_hdl, obj,
				"dispr_solo_thread");
		_os_thread_schedule(d, &(obj->bk_thread));
	}
	#endif
	SET_STATUS_FLAG(obj->status, DISPR_STARTED);
	_start_dispr_controller(obj);

	PHL_TRACE(COMP_PHL_CMDDISP, _PHL_INFO_, "%s[%d]\n", __func__, obj->idx);
	return RTW_PHL_STATUS_SUCCESS;
}

bool is_dispr_started(void *dispr)
{
	struct cmd_dispatcher *obj = (struct cmd_dispatcher *)dispr;

	if (TEST_STATUS_FLAG(obj->status, DISPR_STARTED))
		return true;
	return false;
}

enum rtw_phl_status dispr_stop(void *dispr)
{
	struct cmd_dispatcher *obj = (struct cmd_dispatcher *)dispr;
	#ifdef CONFIG_CMD_DISP_SOLO_MODE
	void *d = phl_to_drvpriv(obj->phl_info);
	#endif

	if (!TEST_STATUS_FLAG(obj->status, DISPR_STARTED))
		return RTW_PHL_STATUS_UNEXPECTED_ERROR;

	dispr_thread_stop_prior_hdl(obj);
	#ifdef CONFIG_CMD_DISP_SOLO_MODE
	if (disp_eng_is_solo_thread_mode(obj->phl_info)) {
		_os_thread_stop(d, &(obj->bk_thread));
		_os_sema_up(d, &(obj->msg_q_sema));
		_os_thread_deinit(d, &(obj->bk_thread));
	}
	#endif
	dispr_thread_stop_post_hdl(obj);
	PHL_TRACE(COMP_PHL_CMDDISP, _PHL_INFO_, "%s[%d]\n", __func__, obj->idx);
	return RTW_PHL_STATUS_SUCCESS;
}

enum rtw_phl_status dispr_register_module(void *dispr,
					  enum phl_module_id id,
					  struct phl_bk_module_ops *ops)
{
	struct cmd_dispatcher *obj = (struct cmd_dispatcher *)dispr;
	void *d = phl_to_drvpriv(obj->phl_info);
	struct phl_bk_module *module = NULL;
	u8 ret = true;
	enum phl_bk_module_priority priority = _get_mdl_priority(id);

	FUNCIN();

	if (!TEST_STATUS_FLAG(obj->status, DISPR_INIT)  ||
	    priority == PHL_MDL_PRI_MAX ||
	    chk_module_ops(ops) == false ||
	    _chk_bitmap_bit(obj->bitmap, MODL_MASK_LEN, id) == true) {
		PHL_TRACE(COMP_PHL_CMDDISP, _PHL_ERR_, "%s[%d], register fail\n", __func__, obj->idx);
		return RTW_PHL_STATUS_FAILURE;
	}

	module = (struct phl_bk_module *)_os_mem_alloc(d, sizeof(struct phl_bk_module));
	if (module == NULL) {
		PHL_TRACE(COMP_PHL_CMDDISP, _PHL_ERR_, "%s[%d], allocte fail\n", __func__, obj->idx);
		return RTW_PHL_STATUS_FAILURE;
	}

	module->id = id;
	_os_mem_cpy(d, &(module->ops), ops, sizeof(struct phl_bk_module_ops));
	pq_push(d, &(obj->module_q[priority]), &(module->list), _tail, _bh);

	ret = bk_module_init(obj, module);
	if (ret == true && TEST_STATUS_FLAG(obj->status, DISPR_STARTED)) {
		ret = bk_module_start(obj, module);
		if (ret == true)
			_add_bitmap_bit(obj->bitmap, MODL_MASK_LEN, &(module->id), 1);
		if (ret == true && priority != PHL_MDL_PRI_OPTIONAL)
			_add_bitmap_bit(obj->basemap, MODL_MASK_LEN, &(module->id), 1);
	}
	PHL_TRACE(COMP_PHL_CMDDISP, _PHL_INFO_, "%s[%d] id:%d, ret:%d\n",__func__, obj->idx, id, ret);
	if (ret == true) {
		return RTW_PHL_STATUS_SUCCESS;
	} else {
		bk_module_deinit(obj, module);
		_os_mem_free(d, module, sizeof(struct phl_bk_module));
		return RTW_PHL_STATUS_FAILURE;
	}
}

enum rtw_phl_status dispr_deregister_module(void *dispr,
					    enum phl_module_id id)
{
	struct cmd_dispatcher *obj = (struct cmd_dispatcher *)dispr;
	void *d = phl_to_drvpriv(obj->phl_info);
	struct phl_bk_module *module = NULL;
	_os_list *mdl = NULL;
	enum rtw_phl_status phl_stat = RTW_PHL_STATUS_FAILURE;
	enum phl_bk_module_priority priority = _get_mdl_priority(id);

	FUNCIN();

	if (!TEST_STATUS_FLAG(obj->status, DISPR_INIT) ||
	    priority == PHL_MDL_PRI_MAX)
		return phl_stat;

	if(pq_search_node(d, &(obj->module_q[priority]), &mdl, _bh, true, &id, search_mdl)) {
		module = (struct phl_bk_module *)mdl;
		_clr_bitmap_bit(obj->bitmap, MODL_MASK_LEN, &(module->id), 1);
		_clr_bitmap_bit(obj->basemap, MODL_MASK_LEN, &(module->id), 1);
		bk_module_stop(obj, module);
		bk_module_deinit(obj, module);
		_os_mem_free(d, module, sizeof(struct phl_bk_module));
		phl_stat = RTW_PHL_STATUS_SUCCESS;
	}

	PHL_TRACE(COMP_PHL_CMDDISP, _PHL_INFO_, "%s[%d], id: %d stat:%d\n",
	          __func__, obj->idx, id, phl_stat);
	return phl_stat;
}

enum rtw_phl_status dispr_module_init(void *dispr)
{
	struct cmd_dispatcher *obj = (struct cmd_dispatcher *)dispr;
	void *d = phl_to_drvpriv(obj->phl_info);
	_os_list *mdl = NULL;
	u8 i = 0;

	if (!TEST_STATUS_FLAG(obj->status, DISPR_INIT))
		return RTW_PHL_STATUS_FAILURE;

	for (i = 0; i < PHL_MDL_PRI_MAX; i++) {
		if (pq_get_front(d, &(obj->module_q[i]), &mdl, _bh) == false)
			continue;
		do {
			bk_module_init(obj, (struct phl_bk_module *)mdl);
		} while(pq_get_next(d, &(obj->module_q[i]), mdl, &mdl, _bh));
	}
	PHL_TRACE(COMP_PHL_CMDDISP, _PHL_INFO_, "%s[%d]\n", __func__, obj->idx);
	return RTW_PHL_STATUS_SUCCESS;
}

enum rtw_phl_status dispr_module_deinit(void *dispr)
{
	struct cmd_dispatcher *obj = (struct cmd_dispatcher *)dispr;
	void *d = phl_to_drvpriv(obj->phl_info);
	_os_list *mdl = NULL;
	u8 i = 0;

	if (!TEST_STATUS_FLAG(obj->status, DISPR_INIT))
		return RTW_PHL_STATUS_FAILURE;

	for (i = 0; i < PHL_MDL_PRI_MAX; i++) {
		while (pq_pop(d, &(obj->module_q[i]), &mdl, _first, _bh)) {
			bk_module_deinit(obj, (struct phl_bk_module *)mdl);
			_os_mem_free(d, mdl, sizeof(struct phl_bk_module));
		}
	}
	PHL_TRACE(COMP_PHL_CMDDISP, _PHL_INFO_, "%s[%d]\n", __func__, obj->idx);
	return RTW_PHL_STATUS_SUCCESS;
}

enum rtw_phl_status dispr_module_start(void *dispr)
{
	struct cmd_dispatcher *obj = (struct cmd_dispatcher *)dispr;
	void *d = phl_to_drvpriv(obj->phl_info);
	_os_list *mdl = NULL;
	struct phl_bk_module *module = NULL;
	u8 i = 0;
	u8 ret = false;

	if (!TEST_STATUS_FLAG(obj->status, DISPR_STARTED))
		return RTW_PHL_STATUS_UNEXPECTED_ERROR;

	for (i = 0; i < PHL_MDL_PRI_MAX; i++) {
		if (pq_get_front(d, &(obj->module_q[i]), &mdl, _bh) == false)
			continue;
		do {
			module = (struct phl_bk_module*)mdl;
			ret = bk_module_start(obj, module);
			if (ret == true)
				_add_bitmap_bit(obj->bitmap, MODL_MASK_LEN, &(module->id), 1);
			if (ret == true && i != PHL_MDL_PRI_OPTIONAL)
				_add_bitmap_bit(obj->basemap, MODL_MASK_LEN, &(module->id), 1);
		} while(pq_get_next(d, &(obj->module_q[i]), mdl, &mdl, _bh));
	}
	PHL_TRACE(COMP_PHL_CMDDISP, _PHL_INFO_, "%s[%d]\n", __func__, obj->idx);
	/*_print_bitmap(obj->bitmap);*/
	return RTW_PHL_STATUS_SUCCESS;
}

enum rtw_phl_status dispr_module_stop(void *dispr)
{
	struct cmd_dispatcher *obj = (struct cmd_dispatcher *)dispr;
	void *d = phl_to_drvpriv(obj->phl_info);
	_os_list *mdl = NULL;
	struct phl_bk_module *module = NULL;
	u8 i = 0;

	if (!TEST_STATUS_FLAG(obj->status, DISPR_STARTED))
		return RTW_PHL_STATUS_UNEXPECTED_ERROR;

	for (i = 0; i < PHL_MDL_PRI_MAX; i++) {
		if (pq_get_front(d, &(obj->module_q[i]), &mdl, _bh) == false)
			continue;
		do {
			module = (struct phl_bk_module *)mdl;
			_clr_bitmap_bit(obj->bitmap, MODL_MASK_LEN, &(module->id), 1);
			_clr_bitmap_bit(obj->basemap, MODL_MASK_LEN, &(module->id), 1);
			bk_module_stop(obj, module);
		} while(pq_get_next(d, &(obj->module_q[i]), mdl, &mdl, _bh));
	}
	PHL_TRACE(COMP_PHL_CMDDISP, _PHL_INFO_, "%s[%d]\n", __func__, obj->idx);
	/*_print_bitmap(obj->bitmap);*/
	return RTW_PHL_STATUS_SUCCESS;
}

/**
 * dispr_get_cur_cmd_req -- background module can call this function to
 * check cmd dispatcher is idle to know the risk or conflict for the I/O.
 * @dispr: dispatcher handler, get from _disp_eng_get_dispr_by_idx
 * @handle: get current cmd request, NULL means cmd dispatcher is idle

 * return RTW_PHL_STATUS_SUCCESS means cmd dispatcher is busy and can get
 * current cmd request from handle parameter
 * return RTW_PHL_STATUS_FAILURE means cmd dispatcher is idle
 */
enum rtw_phl_status
dispr_get_cur_cmd_req(void *dispr, void **handle)
{
	struct cmd_dispatcher *obj = (struct cmd_dispatcher *)dispr;
	struct phl_cmd_token_req_ex *cur_req = NULL;
	enum rtw_phl_status phl_stat = RTW_PHL_STATUS_FAILURE;

	if (!TEST_STATUS_FLAG(obj->status, DISPR_INIT|DISPR_STARTED) || handle == NULL) {
		phl_stat = RTW_PHL_STATUS_UNEXPECTED_ERROR;
		return phl_stat;
	}

	(*handle) = NULL;
	cur_req = obj->cur_cmd_req;

	if (cur_req == NULL ||
	    !TEST_STATUS_FLAG(cur_req->status, REQ_STATUS_RUN) ||
	    TEST_STATUS_FLAG(cur_req->status, REQ_STATUS_CANCEL))
		return phl_stat;

	*handle = (void *)cur_req;
	phl_stat = RTW_PHL_STATUS_SUCCESS;

	PHL_TRACE(COMP_PHL_CMDDISP, _PHL_DEBUG_, "%s[%d], req module id:%d phl_stat:%d\n",
	          __func__, obj->idx, cur_req->req.module_id, phl_stat);
	return phl_stat;
}

enum rtw_phl_status
dispr_set_cur_cmd_info(void *dispr,
			   struct phl_module_op_info *op_info)
{
	void *handle = NULL;
	struct phl_cmd_token_req_ex *cmd_req = NULL;
	struct phl_cmd_token_req *req = NULL;

	if (RTW_PHL_STATUS_SUCCESS != dispr_get_cur_cmd_req(dispr, &handle))
		return RTW_PHL_STATUS_FAILURE;

	cmd_req = (struct phl_cmd_token_req_ex *)handle;
	req = &(cmd_req->req);

	PHL_TRACE(COMP_PHL_CMDDISP, _PHL_INFO_, "%s, id:%d\n",
	          __func__, req->module_id);
	if (req->set_info(dispr, req->priv, op_info) == MDL_RET_SUCCESS)
		return RTW_PHL_STATUS_SUCCESS;
	else
		return RTW_PHL_STATUS_FAILURE;
}

enum rtw_phl_status
dispr_query_cur_cmd_info(void *dispr,
			     struct phl_module_op_info *op_info)
{
	void *handle = NULL;
	struct phl_cmd_token_req_ex *cmd_req = NULL;
	struct phl_cmd_token_req *req = NULL;

	if (RTW_PHL_STATUS_SUCCESS != dispr_get_cur_cmd_req(dispr, &handle))
		return RTW_PHL_STATUS_FAILURE;

	cmd_req = (struct phl_cmd_token_req_ex *)handle;
	req = &(cmd_req->req);

	PHL_TRACE(COMP_PHL_CMDDISP, _PHL_DEBUG_, "%s, id:%d\n", __func__, req->module_id);
	if (req->query_info(dispr, req->priv, op_info) == MDL_RET_SUCCESS)
		return RTW_PHL_STATUS_SUCCESS;
	else
		return RTW_PHL_STATUS_FAILURE;
}

enum rtw_phl_status
dispr_get_bk_module_handle(void *dispr,
			       enum phl_module_id id,
			       void **handle)
{
	struct cmd_dispatcher *obj = (struct cmd_dispatcher *)dispr;
	void *d = phl_to_drvpriv(obj->phl_info);
	_os_list *mdl = NULL;
	enum rtw_phl_status phl_stat = RTW_PHL_STATUS_FAILURE;
	enum phl_bk_module_priority priority = _get_mdl_priority(id);

	if (!TEST_STATUS_FLAG(obj->status, DISPR_INIT) ||
	    handle == NULL ||
	    priority == PHL_MDL_PRI_MAX ||
	    !_chk_bitmap_bit(obj->bitmap, MODL_MASK_LEN, id))
		return phl_stat;

	(*handle) = NULL;


	if(pq_search_node(d, &(obj->module_q[priority]), &mdl, _bh, false, &id, search_mdl)) {
		(*handle) = mdl;
		phl_stat = RTW_PHL_STATUS_SUCCESS;
	}
	PHL_TRACE(COMP_PHL_CMDDISP, _PHL_DEBUG_,
			"%s[%d], id:%d phl_stat:%d\n", __func__, obj->idx, id, phl_stat);
	return phl_stat;
}

enum rtw_phl_status
dispr_set_bk_module_info(void *dispr,
			     void *handle,
			     struct phl_module_op_info *op_info)
{
	struct phl_bk_module *module = (struct phl_bk_module *)handle;
	struct phl_bk_module_ops *ops = &(module->ops);

	if (!TEST_STATUS_FLAG(module->status, MDL_INIT))
		return RTW_PHL_STATUS_FAILURE;
	PHL_TRACE(COMP_PHL_CMDDISP, _PHL_INFO_, "%s, id:%d\n", __func__, module->id);
	if (ops->set_info(dispr, module->priv, op_info) == MDL_RET_SUCCESS)
		return RTW_PHL_STATUS_SUCCESS;
	else
		return RTW_PHL_STATUS_FAILURE;
}

enum rtw_phl_status
dispr_query_bk_module_info(void *dispr,
			       void *handle,
			       struct phl_module_op_info *op_info)
{
	struct phl_bk_module *module = (struct phl_bk_module *)handle;
	struct phl_bk_module_ops *ops = &(module->ops);

	if (!TEST_STATUS_FLAG(module->status, MDL_INIT))
		return RTW_PHL_STATUS_FAILURE;
	PHL_TRACE(COMP_PHL_CMDDISP, _PHL_INFO_, "%s, id:%d\n", __func__, module->id);
	if (ops->query_info(dispr, module->priv, op_info) == MDL_RET_SUCCESS)
		return RTW_PHL_STATUS_SUCCESS;
	else
		return RTW_PHL_STATUS_FAILURE;
}

enum rtw_phl_status
dispr_set_src_info(void *dispr,
		       struct phl_msg *msg,
		       struct phl_module_op_info *op_info)
{
	struct cmd_dispatcher *obj = (struct cmd_dispatcher *)dispr;
	enum rtw_phl_status phl_stat = RTW_PHL_STATUS_FAILURE;
	u8 id = MSG_MDL_ID_FIELD(msg->msg_id);
	struct phl_cmd_token_req_ex *cur_req = obj->cur_cmd_req;
	enum phl_mdl_ret_code ret = MDL_RET_FAIL;
	struct phl_dispr_msg_ex *ex = (struct phl_dispr_msg_ex *)msg;
	u8 cur_req_id = get_cur_cmd_req_id(obj, NULL);

	if (!TEST_STATUS_FLAG(obj->status, DISPR_INIT) ||
	    (!_chk_bitmap_bit(obj->bitmap, MODL_MASK_LEN, id) &&
	    cur_req_id != id))
		return phl_stat;

	if (cur_req_id == id) {
		ret = cur_req->req.set_info(dispr, cur_req->req.priv, op_info);
	} else if (TEST_STATUS_FLAG(ex->status, MSG_STATUS_OWNER_BK_MDL)) {
		ret = ex->module->ops.set_info(dispr, ex->module->priv,
					       op_info);
	}
	PHL_TRACE(COMP_PHL_CMDDISP, _PHL_INFO_, "%s[%d], id:%d phl_stat:%d\n",
	          __func__, obj->idx, id, phl_stat);
	if (ret == MDL_RET_FAIL)
		return RTW_PHL_STATUS_FAILURE;
	else
		return RTW_PHL_STATUS_SUCCESS;
}

enum rtw_phl_status
dispr_query_src_info(void *dispr,
			 struct phl_msg *msg,
			 struct phl_module_op_info *op_info)
{
	struct cmd_dispatcher *obj = (struct cmd_dispatcher *)dispr;
	enum rtw_phl_status phl_stat = RTW_PHL_STATUS_FAILURE;
	u8 id = MSG_MDL_ID_FIELD(msg->msg_id);
	struct phl_cmd_token_req_ex *cur_req = obj->cur_cmd_req;
	struct phl_dispr_msg_ex *ex = (struct phl_dispr_msg_ex *)msg;
	enum phl_mdl_ret_code ret = MDL_RET_FAIL;
	u8 cur_req_id = get_cur_cmd_req_id(obj, NULL);

	if (!TEST_STATUS_FLAG(obj->status, DISPR_INIT) ||
	    (!_chk_bitmap_bit(obj->bitmap, MODL_MASK_LEN, id) &&
	    cur_req_id != id))
		return phl_stat;

	if (cur_req_id == id) {
		ret = cur_req->req.query_info(dispr, cur_req->req.priv, op_info);
	} else if (TEST_STATUS_FLAG(ex->status, MSG_STATUS_OWNER_BK_MDL)) {
		ret = ex->module->ops.query_info(dispr, ex->module->priv,
						 op_info);
	}
	PHL_TRACE(COMP_PHL_CMDDISP, _PHL_INFO_, "%s[%d], id:%d phl_stat:%d\n",
	          __func__, obj->idx, id, phl_stat);
	if (ret == MDL_RET_FAIL)
		return RTW_PHL_STATUS_FAILURE;
	else
		return RTW_PHL_STATUS_SUCCESS;
}

enum rtw_phl_status
dispr_send_msg(void *dispr,
               struct phl_msg *msg,
               struct phl_msg_attribute *attr,
               u32 *msg_hdl)
{
	struct cmd_dispatcher *obj = (struct cmd_dispatcher *)dispr;
	void *d = phl_to_drvpriv(obj->phl_info);
	struct phl_dispr_msg_ex *msg_ex = NULL;
	u8 module_id = MSG_MDL_ID_FIELD(msg->msg_id); /* msg src */
	u32 req_status = 0;
	u8 cur_req_id = get_cur_cmd_req_id(obj, &req_status);
	enum rtw_phl_status sts = RTW_PHL_STATUS_FAILURE;

	if (!TEST_STATUS_FLAG(obj->status, DISPR_STARTED)) {
		sts = RTW_PHL_STATUS_UNEXPECTED_ERROR;
		goto err;
	}

	if (TEST_STATUS_FLAG(obj->status, DISPR_SHALL_STOP)){
		PHL_TRACE(COMP_PHL_CMDDISP, _PHL_INFO_,"%s[%d]: dispr shall stop\n", __func__, obj->idx);
		sts = RTW_PHL_STATUS_UNEXPECTED_ERROR;
		goto err;
	}

	if(attr && attr->notify.id_arr == NULL && attr->notify.len) {
		PHL_TRACE(COMP_PHL_CMDDISP, _PHL_ERR_, "%s[%d]: attribute err\n", __func__, obj->idx);
		sts = RTW_PHL_STATUS_INVALID_PARAM;
		goto err;
	}

	if (!IS_DISPR_CTRL(module_id) &&
	    !_chk_bitmap_bit(obj->bitmap, MODL_MASK_LEN, module_id) &&
	    ((cur_req_id != PHL_MDL_ID_MAX  && cur_req_id != module_id) ||
	     (cur_req_id == PHL_MDL_ID_MAX && req_status == 0) ||
	     (cur_req_id == PHL_MDL_ID_MAX && !TEST_STATUS_FLAG(req_status,REQ_STATUS_LAST_PERMIT)))) {
		PHL_TRACE(COMP_PHL_CMDDISP, _PHL_ERR_,
			"%s[%d] module not allow to send\n", __func__, obj->idx);
		sts = RTW_PHL_STATUS_INVALID_PARAM;
		goto err;
	}

	if (!pop_front_idle_msg(obj, &msg_ex)) {
		PHL_TRACE(COMP_PHL_CMDDISP, _PHL_ERR_, "%s[%d] idle msg empty\n", __func__, obj->idx);
		sts = RTW_PHL_STATUS_RESOURCE;
		goto err;
	}

	if (msg_hdl)
		*msg_hdl = 0;

	_os_mem_cpy(d, &msg_ex->msg, msg, sizeof(struct phl_msg));

	set_msg_bitmap(obj, msg_ex, module_id);
	if (attr) {
#ifdef CONFIG_CMD_DISP_SUPPORT_CUSTOM_SEQ
		msg_ex->attr = (struct dispr_msg_attr *)attr->dispr_attr;
		attr->dispr_attr = NULL;
#endif
		set_msg_custom_bitmap(obj, msg_ex, attr->opt,
			       attr->notify.id_arr, attr->notify.len, module_id);
		if (attr->completion.completion) {
			SET_STATUS_FLAG(msg_ex->status, MSG_STATUS_NOTIFY_COMPLETE);
			msg_ex->completion.completion = attr->completion.completion;
			msg_ex->completion.priv = attr->completion.priv;
		}
		if (TEST_STATUS_FLAG(attr->opt, MSG_OPT_CLR_SNDR_MSG_IF_PENDING))
			SET_STATUS_FLAG(msg_ex->status, MSG_STATUS_CLR_SNDR_MSG_IF_PENDING);

		if (TEST_STATUS_FLAG(attr->opt, MSG_OPT_PENDING_DURING_CANNOT_IO))
			SET_STATUS_FLAG(msg_ex->status, MSG_STATUS_PENDING_DURING_CANNOT_IO);
		PHL_TRACE(COMP_PHL_CMDDISP, _PHL_INFO_, "%s[%d], opt:0x%x\n", __func__, obj->idx, attr->opt);
	}

	if (get_module_by_id(obj, module_id, &(msg_ex->module)) == true) {
		SET_STATUS_FLAG(msg_ex->status, MSG_STATUS_OWNER_BK_MDL);
		PHL_TRACE(COMP_PHL_CMDDISP, _PHL_INFO_,
			"%s[%d] module(%d) found\n", __func__, obj->idx, module_id);
	} else 	if ((cur_req_id == module_id) ||
		    (cur_req_id == PHL_MDL_ID_MAX && TEST_STATUS_FLAG(req_status,REQ_STATUS_LAST_PERMIT))) {
		SET_STATUS_FLAG(msg_ex->status, MSG_STATUS_OWNER_REQ);
	}

	if(TEST_STATUS_FLAG(msg_ex->status, MSG_STATUS_OWNER_REQ) &&
	   TEST_STATUS_FLAG(req_status,REQ_STATUS_LAST_PERMIT) &&
	   (attr == NULL || !TEST_STATUS_FLAG(attr->opt, MSG_OPT_SEND_IN_ABORT))) {
		PHL_TRACE(COMP_PHL_CMDDISP, _PHL_ERR_,
			"%s[%d] msg not allow since cur req is going to unload\n", __func__, obj->idx);
		SET_MSG_INDC_FIELD(msg_ex->msg.msg_id, MSG_INDC_FAIL);
		push_back_idle_msg(obj, msg_ex);
		sts = RTW_PHL_STATUS_FAILURE;
		goto exit;
	}

	if (TEST_STATUS_FLAG(msg_ex->status, MSG_STATUS_OWNER_REQ) &&
	    TEST_STATUS_FLAG(req_status,REQ_STATUS_LAST_PERMIT)) {
		SET_STATUS_FLAG(msg_ex->status, MSG_STATUS_FOR_ABORT);
		SET_STATUS_FLAG(obj->status, DISPR_WAIT_ABORT_MSG_DONE);
	}

	SET_STATUS_FLAG(msg_ex->status, MSG_STATUS_PRE_PHASE);

	if (IS_DISPR_CTRL(module_id))
		insert_msg_by_priority(obj, msg_ex);
	else
		push_back_wait_msg(obj, msg_ex);

	PHL_TRACE(COMP_PHL_CMDDISP, _PHL_INFO_, "%s[%d], status:0x%x\n",
	          __func__, obj->idx, msg_ex->status);
	if(msg_hdl)
		*msg_hdl = GEN_VALID_HDL(msg_ex->idx);
	PHL_TRACE(COMP_PHL_CMDDISP, _PHL_INFO_, "%s[%d], msg_id:0x%x\n", __func__, obj->idx, msg->msg_id);
	return RTW_PHL_STATUS_SUCCESS;
err:
#ifdef CONFIG_CMD_DISP_SUPPORT_CUSTOM_SEQ
	if(attr)
		free_dispr_attr(d,(struct dispr_msg_attr **) &(attr->dispr_attr));
#endif
exit:
	return sts;
}

enum rtw_phl_status dispr_cancel_msg(void *dispr, u32 *msg_hdl)
{
	struct cmd_dispatcher *obj = (struct cmd_dispatcher *)dispr;
	struct phl_dispr_msg_ex *msg_ex = NULL;

	if (!TEST_STATUS_FLAG(obj->status, DISPR_STARTED) || msg_hdl == NULL)
		return RTW_PHL_STATUS_UNEXPECTED_ERROR;

	if (!IS_HDL_VALID(*msg_hdl) ||
	    GET_MSG_IDX_FROM_HDL(*msg_hdl) >= MAX_PHL_MSG_NUM) {
		PHL_TRACE(COMP_PHL_CMDDISP, _PHL_ERR_, "%s[%d], HDL invalid\n", __func__, obj->idx);
		return RTW_PHL_STATUS_FAILURE;
	}

	msg_ex = &(obj->msg_ex_pool[GET_MSG_IDX_FROM_HDL(*msg_hdl)]);
	*msg_hdl = 0;
	if (!TEST_STATUS_FLAG(msg_ex->status, MSG_STATUS_ENQ) &&
	    !TEST_STATUS_FLAG(msg_ex->status, MSG_STATUS_RUN)) {
		PHL_TRACE(COMP_PHL_CMDDISP, _PHL_ERR_, "%s[%d], HDL status err\n", __func__, obj->idx);
		return RTW_PHL_STATUS_FAILURE;
	}

	cancel_msg(obj, msg_ex);
	PHL_TRACE(COMP_PHL_CMDDISP, _PHL_INFO_, "%s[%d]\n", __func__, obj->idx);
	return RTW_PHL_STATUS_SUCCESS;
}

enum rtw_phl_status dispr_clr_pending_msg(void *dispr)
{
	struct cmd_dispatcher *obj = (struct cmd_dispatcher *)dispr;

	SET_STATUS_FLAG(obj->status, DISPR_CLR_PEND_MSG);
	notify_dispr_thread(obj);
	PHL_TRACE(COMP_PHL_CMDDISP, _PHL_INFO_, "%s[%d]\n", __func__, obj->idx);
	return RTW_PHL_STATUS_SUCCESS;
}

enum rtw_phl_status
dispr_add_token_req(void *dispr,
			struct phl_cmd_token_req *req,
			u32 *req_hdl)
{
	struct cmd_dispatcher *obj = (struct cmd_dispatcher *)dispr;
	void *d = phl_to_drvpriv(obj->phl_info);
	struct phl_cmd_token_req_ex *req_ex = NULL;
	enum rtw_phl_status stat = RTW_PHL_STATUS_SUCCESS;
	_os_list *node = NULL;

	if (!TEST_STATUS_FLAG(obj->status, DISPR_STARTED) ||
	    req_hdl == NULL ||
	    chk_cmd_req_ops(req) == false)
		return RTW_PHL_STATUS_UNEXPECTED_ERROR;

	if (TEST_STATUS_FLAG(obj->status, DISPR_SHALL_STOP)){
		PHL_TRACE(COMP_PHL_CMDDISP, _PHL_INFO_,"%s[%d]: dispr shall stop\n", __func__, obj->idx);
		return RTW_PHL_STATUS_UNEXPECTED_ERROR;
	}

	if (!pop_front_idle_req(obj, &req_ex)) {
		PHL_TRACE(COMP_PHL_CMDDISP, _PHL_ERR_, "%s[%d] idle req empty\n", __func__, obj->idx);
		return RTW_PHL_STATUS_RESOURCE;
	}
	_os_mem_cpy(d, &(req_ex->req), req, sizeof(struct phl_cmd_token_req));

	push_back_wait_req(obj, req_ex);
	*req_hdl = GEN_VALID_HDL(req_ex->idx);
	PHL_TRACE(COMP_PHL_DBG, _PHL_INFO_, "%s[%d], id:%d, hdl:0x%x token_cnt:%d\n",
	          __func__, obj->idx, req->module_id, *req_hdl, _os_atomic_read(d, &(obj->token_cnt)));

	if (pq_get_front(d, &(obj->token_op_q), &node, _bh) == false &&
	    _os_atomic_read(d, &(obj->token_cnt)) == 0)
		stat = RTW_PHL_STATUS_SUCCESS;
	else
		stat = RTW_PHL_STATUS_PENDING;
	dispr_enqueue_token_op_info(obj, &req_ex->add_req_info, TOKEN_OP_ADD_CMD_REQ, req_ex->idx);
	return stat;
}

enum rtw_phl_status dispr_cancel_token_req(void *dispr, u32 *req_hdl)
{
	struct cmd_dispatcher *obj = (struct cmd_dispatcher *)dispr;
	struct phl_cmd_token_req_ex *req_ex = NULL;

	if (!TEST_STATUS_FLAG(obj->status, DISPR_STARTED) || req_hdl == NULL)
		return RTW_PHL_STATUS_UNEXPECTED_ERROR;

	if (!IS_HDL_VALID(*req_hdl) ||
	    GET_REQ_IDX_FROM_HDL(*req_hdl) >= MAX_CMD_REQ_NUM) {
		PHL_TRACE(COMP_PHL_CMDDISP, _PHL_ERR_,
			"%s[%d], HDL(0x%x) invalid\n", __func__, obj->idx, *req_hdl);
		return RTW_PHL_STATUS_FAILURE;
	}
	req_ex = &(obj->token_req_ex_pool[GET_REQ_IDX_FROM_HDL(*req_hdl)]);
	if (!TEST_STATUS_FLAG(req_ex->status, REQ_STATUS_ENQ) &&
	    !TEST_STATUS_FLAG(req_ex->status, REQ_STATUS_RUN) &&
	    !TEST_STATUS_FLAG(req_ex->status, REQ_STATUS_PREPARE)) {
		PHL_TRACE(COMP_PHL_CMDDISP, _PHL_ERR_,
			"%s[%d], HDL(0x%x) status err\n", __func__, obj->idx, *req_hdl);
		return RTW_PHL_STATUS_FAILURE;
	}

	if (dispr_enqueue_token_op_info(obj, &req_ex->free_req_info, TOKEN_OP_CANCEL_CMD_REQ, req_ex->idx))
		return RTW_PHL_STATUS_SUCCESS;
	else
		return RTW_PHL_STATUS_FAILURE;
}

enum rtw_phl_status dispr_free_token(void *dispr, u32 *req_hdl)
{
	struct cmd_dispatcher *obj = (struct cmd_dispatcher *)dispr;
	void *d = phl_to_drvpriv(obj->phl_info);
	struct phl_cmd_token_req_ex *req_ex = NULL;

	if (!TEST_STATUS_FLAG(obj->status, DISPR_STARTED) || req_hdl == NULL)
		return RTW_PHL_STATUS_UNEXPECTED_ERROR;

	if (obj->cur_cmd_req == NULL ||
	    _os_atomic_read(d, &(obj->token_cnt)) == 0  ||
	    !IS_HDL_VALID(*req_hdl) ||
	    GET_REQ_IDX_FROM_HDL(*req_hdl) >= MAX_CMD_REQ_NUM) {
		PHL_TRACE(COMP_PHL_CMDDISP, _PHL_ERR_,
			"%s[%d], HDL(0x%x) invalid\n", __func__, obj->idx, *req_hdl);
		return RTW_PHL_STATUS_FAILURE;
	}
	req_ex = &(obj->token_req_ex_pool[GET_REQ_IDX_FROM_HDL(*req_hdl)]);
	if (!TEST_STATUS_FLAG(req_ex->status, REQ_STATUS_RUN) &&
	    !TEST_STATUS_FLAG(req_ex->status, REQ_STATUS_PREPARE)) {
		PHL_TRACE(COMP_PHL_CMDDISP, _PHL_ERR_,
			"%s[%d], HDL(0x%x) mismatch\n", __func__, obj->idx, *req_hdl);
		return RTW_PHL_STATUS_FAILURE;
	}
	SET_STATUS_FLAG(req_ex->status, REQ_STATUS_CANCEL);
	if (dispr_enqueue_token_op_info(obj, &req_ex->free_req_info, TOKEN_OP_FREE_CMD_REQ, req_ex->idx))
		return RTW_PHL_STATUS_SUCCESS;
	else
		return RTW_PHL_STATUS_FAILURE;
}

void dispr_clearance_acquire(void *dispr)
{
	struct cmd_dispatcher *obj = (struct cmd_dispatcher *)dispr;

	SET_STATUS_FLAG(obj->status, DISPR_CLEARANCE);
	PHL_TRACE(COMP_PHL_CMDDISP, _PHL_INFO_, "%s[%d]\n", __func__, obj->idx);
}

void dispr_clearance_release(void *dispr)
{
	struct cmd_dispatcher *obj = (struct cmd_dispatcher *)dispr;

	CLEAR_STATUS_FLAG(obj->status, DISPR_CLEARANCE);
	dispr_enqueue_token_op_info(obj, &obj->renew_req_info, TOKEN_OP_RENEW_CMD_REQ, 0xff);
	PHL_TRACE(COMP_PHL_CMDDISP, _PHL_INFO_, "%s[%d]\n", __func__, obj->idx);
}

void dispr_exclusive_ready(void *dispr, bool renew_req)
{
	struct cmd_dispatcher *obj = (struct cmd_dispatcher *)dispr;

	if (!TEST_STATUS_FLAG(obj->status, DISPR_EXCLUSIVE_USE)) {
		SET_STATUS_FLAG(obj->status, DISPR_EXCLUSIVE_USE);
		if (renew_req)
			dispr_enqueue_token_op_info(obj, &obj->renew_req_info, TOKEN_OP_RENEW_CMD_REQ, 0xff);
		PHL_TRACE(COMP_PHL_CMDDISP, _PHL_INFO_, "%s[%d]\n", __func__, obj->idx);
	}
}

enum rtw_phl_status dispr_notify_dev_io_status(void *dispr, enum phl_module_id mdl_id, bool allow_io)
{
	struct cmd_dispatcher *obj = (struct cmd_dispatcher *)dispr;
	enum rtw_phl_status status = RTW_PHL_STATUS_SUCCESS;

	if (allow_io == false) {
		if (!TEST_STATUS_FLAG(obj->status, DISPR_CANNOT_IO)) {
			SET_STATUS_FLAG(obj->status, DISPR_CANNOT_IO);
			SET_EXCL_MDL(obj, mdl_id);
			PHL_TRACE(COMP_PHL_CMDDISP, _PHL_INFO_,
				"%s[%d], mdl_id(%d) notify cannot io\n", __func__, obj->idx, mdl_id);
			status = send_dev_io_status_change(obj, allow_io);
		}
	}
	else {
		if (TEST_STATUS_FLAG(obj->status, DISPR_CANNOT_IO)) {
			CLEAR_STATUS_FLAG(obj->status, DISPR_CANNOT_IO);
			CLEAR_EXCL_MDL(obj);
			status = send_dev_io_status_change(obj, allow_io);
			dispr_clr_pending_msg(dispr);
			PHL_TRACE(COMP_PHL_CMDDISP, _PHL_INFO_,
				"%s[%d], mdl_id(%d) notify io resume\n", __func__, obj->idx, mdl_id);
		}
	}
	return status;
}

void dispr_notify_shall_stop(void *dispr)
{
	struct cmd_dispatcher *obj = (struct cmd_dispatcher *)dispr;

	if (!TEST_STATUS_FLAG(obj->status, DISPR_SHALL_STOP)) {
		SET_STATUS_FLAG(obj->status, DISPR_SHALL_STOP);
		dispr_clr_pending_msg(dispr);
		PHL_TRACE(COMP_PHL_CMDDISP, _PHL_INFO_,
		          "%s[%d], notify shall stop\n", __func__, obj->idx);
	}
}

u8 dispr_is_fg_empty(void *dispr)
{
	struct cmd_dispatcher *obj = (struct cmd_dispatcher *)dispr;
	bool is_empty = true;
	void *drv = phl_to_drvpriv(obj->phl_info);
	struct phl_queue *q = NULL;
	_os_list *node = NULL;

	do {
		/* shall check wait queue first then check token op queue
		 * to avoid to get the incorrect empty state of fg cmd
		 */
		q = &(obj->token_req_wait_q);
		_os_spinlock(drv, &(q->lock), _bh, NULL);
		if(!list_empty(&q->queue) && (q->cnt > 0)) {
			is_empty = false;
			_os_spinunlock(drv, &(q->lock), _bh, NULL);
			break;
		}
		_os_spinunlock(drv, &(q->lock), _bh, NULL);

		if (pq_get_front(drv, &(obj->token_op_q), &node, _bh) == true ||
		    _os_atomic_read(drv, &(obj->token_cnt)) > 0) {
			is_empty = false;
			break;
		}
	} while(false);

	return is_empty;
}

enum rtw_phl_status dispr_process_token_req(struct cmd_dispatcher *obj)
{
	void *d = phl_to_drvpriv(obj->phl_info);
	struct phl_cmd_token_req_ex *ex = NULL;
	u8 ntfyidx = HW_BAND_0;

	do {
		if (!TEST_STATUS_FLAG(obj->status, DISPR_STARTED))
			return RTW_PHL_STATUS_UNEXPECTED_ERROR;

		if (TEST_STATUS_FLAG(obj->status, DISPR_SHALL_STOP)) {
			PHL_TRACE(COMP_PHL_CMDDISP, _PHL_INFO_,
			          "%s[%d]: dispr shall stop\n", __func__, obj->idx);

			return RTW_PHL_STATUS_FAILURE;
		}

		if (_os_atomic_read(d, &(obj->token_cnt)) > 0)
			return RTW_PHL_STATUS_FAILURE;

		if (TEST_STATUS_FLAG(obj->status, DISPR_WAIT_ABORT_MSG_DONE)) {
			PHL_TRACE(COMP_PHL_CMDDISP, _PHL_INFO_,
				"%s[%d], wait for abort msg sent from prev req finish before register next req \n", __func__, obj->idx);
			return RTW_PHL_STATUS_FAILURE;
		}

		/* no current token */
		if (TEST_STATUS_FLAG(obj->status, DISPR_CLEARANCE)) {
			if(obj->idx == HW_BAND_0)
				ntfyidx = HW_BAND_1;
			phl_disp_eng_exclusive_ready(obj->phl_info, ntfyidx);

			PHL_TRACE(COMP_PHL_CMDDISP, _PHL_INFO_,
				"%s[%d]: BLOCK!! Exclusive cmd in the other dispr \n", __func__, obj->idx);
			return RTW_PHL_STATUS_FAILURE;
		}

		/* FG_CMD_OPT_EXCLUSIVE */
		if (front_req_with_exclusive_opt(obj)) {
			if (!TEST_STATUS_FLAG(obj->status, DISPR_EXCLUSIVE_USE)) {
				if(obj->idx == HW_BAND_0)
					ntfyidx = HW_BAND_1;
				phl_disp_eng_clearance_acquire(obj->phl_info, ntfyidx);

				PHL_TRACE(COMP_PHL_CMDDISP, _PHL_INFO_,
					"%s[%d]: wait for the other dispr clearance \n", __func__, obj->idx);
				return RTW_PHL_STATUS_FAILURE;
			}
		}
		if (pop_front_wait_req(obj, &ex) == false) {
			if (!TEST_STATUS_FLAG(obj->status, DISPR_NOTIFY_IDLE)) {
				SET_STATUS_FLAG(obj->status, DISPR_NOTIFY_IDLE);
				send_bk_msg_phy_idle(obj);
			}
			return RTW_PHL_STATUS_SUCCESS;
		}

		if (TEST_STATUS_FLAG(obj->status, DISPR_NOTIFY_IDLE)) {
			CLEAR_STATUS_FLAG(obj->status, DISPR_NOTIFY_IDLE);
			send_bk_msg_phy_on(obj);
		}

	}while(!register_cur_cmd_req(obj, ex));

	return RTW_PHL_STATUS_SUCCESS;
}

#if !defined(CONFIG_CMD_DISP_SOLO_MODE)
void dispr_share_thread_loop_hdl(void *dispr)
{
	dispr_thread_loop_hdl( (struct cmd_dispatcher *)dispr);
}
void dispr_share_thread_leave_hdl(void *dispr)
{
	dispr_thread_leave_hdl((struct cmd_dispatcher *)dispr);
}
void dispr_share_thread_stop_prior_hdl(void *dispr)
{
	dispr_thread_stop_prior_hdl((struct cmd_dispatcher *)dispr);
}
void dispr_share_thread_stop_post_hdl(void *dispr)
{
	dispr_thread_stop_post_hdl((struct cmd_dispatcher *)dispr);
}
#endif
u8 disp_query_mdl_id(struct phl_info_t *phl, void *bk_mdl)
{
	struct phl_bk_module *mdl = NULL;

	if (bk_mdl != NULL) {
		mdl = (struct phl_bk_module *)bk_mdl;
		return mdl->id;
	} else {
		return PHL_MDL_ID_MAX;
	}
}

void send_bk_msg_phy_on(struct cmd_dispatcher *obj)
{
	struct phl_msg msg = {0};
	struct phl_msg_attribute attr = {0};

	SET_MSG_MDL_ID_FIELD(msg.msg_id, PHL_MDL_POWER_MGNT);
	SET_MSG_EVT_ID_FIELD(msg.msg_id, MSG_EVT_PHY_ON);
	dispr_send_msg((void*)obj, &msg, &attr, NULL);
}

void send_bk_msg_phy_idle(struct cmd_dispatcher *obj)
{
	struct phl_msg msg = {0};
	struct phl_msg_attribute attr = {0};

	SET_MSG_MDL_ID_FIELD(msg.msg_id, PHL_MDL_POWER_MGNT);
	SET_MSG_EVT_ID_FIELD(msg.msg_id, MSG_EVT_PHY_IDLE);
	dispr_send_msg((void*)obj, &msg, &attr, NULL);
}

enum rtw_phl_status send_dev_io_status_change(struct cmd_dispatcher *obj, u8 allow_io)
{
	struct phl_msg msg = {0};
	struct phl_msg_attribute attr = {0};
	u16 event = (allow_io == true) ? (MSG_EVT_DEV_RESUME_IO) : (MSG_EVT_DEV_CANNOT_IO);

	SET_MSG_MDL_ID_FIELD(msg.msg_id, PHL_MDL_PHY_MGNT);
	SET_MSG_EVT_ID_FIELD(msg.msg_id, event);
	return dispr_send_msg((void*)obj, &msg, &attr, NULL);
}

#ifdef CONFIG_CMD_DISP_SUPPORT_CUSTOM_SEQ
enum phl_mdl_ret_code loop_through_map(struct cmd_dispatcher *obj, struct phl_dispr_msg_ex *ex,
					enum phl_bk_module_priority priority, struct msg_notify_map *map, u8 pre_prot_phase)
{
	u8 i = 0;
	struct phl_bk_module *mdl = NULL;
	enum phl_mdl_ret_code ret = MDL_RET_IGNORE;
	u8 *bitmap = (pre_prot_phase == true) ? (ex->premap) : (ex->postmap);

	for (i = 0 ; i < map->len; i++) {
		if (map->id_arr[i] >= PHL_FG_MDL_START) {
			PHL_TRACE(COMP_PHL_CMDDISP, _PHL_WARNING_,
			"%s[%d], cmd req does not take precedence over bk module\n", __func__, obj->idx);
			continue;
		}
		mdl = (struct phl_bk_module *)GET_MDL_HANDLE(obj, map->id_arr[i]);
		if (mdl == NULL || !_chk_bitmap_bit(bitmap, MODL_MASK_LEN, mdl->id))
			continue;
		/*only allow sequence rearrange for modules at the same priority*/
		if ( _get_mdl_priority(mdl->id) != priority)
			continue;
		ret = feed_mdl_msg(obj, mdl, ex);
		if (STOP_DISPATCH_MSG(ret))
			return ret;
	}
	return ret;
}

static enum phl_mdl_ret_code run_self_def_seq(struct cmd_dispatcher *obj, struct phl_dispr_msg_ex *ex,
					enum phl_bk_module_priority priority, u8 pre_prot_phase)
{
	struct msg_notify_map *map = NULL;
	enum phl_mdl_ret_code ret = MDL_RET_IGNORE;
	struct msg_dispatch_seq* seq = NULL;

	if (ex->attr == NULL)
		return ret;
	/*MANDATORY modules cannot change dispatch order*/
	if (pre_prot_phase == true)
		seq = &(ex->attr->self_def_seq.pre_prot_phase);
	else
		seq = &(ex->attr->self_def_seq.post_prot_phase);

	return loop_through_map(obj, ex, priority, &(seq->map[priority]), pre_prot_phase);
}
void reset_self_def_seq(void *d, struct msg_self_def_seq* self_def_seq)
{
	u8 i = 0;
	u8 cnt = 0;
	struct msg_dispatch_seq *seq = (struct msg_dispatch_seq *)self_def_seq;

	while (cnt++ < 2) {
		for (i = 0; i < PHL_MDL_PRI_MAX; i++) {
			if (seq->map[i].len)
				_os_kmem_free(d, seq->map[i].id_arr, seq->map[i].len);
			seq->map[i].id_arr = NULL;
			seq->map[i].len = 0;
		}
		seq++;
	}

}

u8 copy_self_def_seq(void *d, struct msg_self_def_seq* self_def_dest, struct msg_self_def_seq* self_def_src)
{
	u8 i = 0;
	u8 cnt = 0;
	struct msg_dispatch_seq *dest = (struct msg_dispatch_seq *)self_def_dest;
	struct msg_dispatch_seq *src = (struct msg_dispatch_seq *)self_def_src;

	while (cnt++ < 2) {
		for (i = 0; i < PHL_MDL_PRI_MAX; i++) {
			if (src->map[i].len) {
				dest->map[i].id_arr = _os_kmem_alloc(d, src->map[i].len);
				if (dest->map[i].id_arr == NULL)
					return false;
				dest->map[i].len = src->map[i].len;
				_os_mem_cpy(d, dest->map[i].id_arr, src->map[i].id_arr, dest->map[i].len);
			}
		}
		dest++;
		src++;
	}
	return true;
}

inline static u8 alloc_dispr_attr(void *d, struct phl_msg_attribute *attr)
{
	if (attr->dispr_attr == NULL)
		attr->dispr_attr = _os_kmem_alloc(d, sizeof(struct dispr_msg_attr));
	if (attr->dispr_attr != NULL)
		_os_mem_set(d, attr->dispr_attr, sizeof(struct dispr_msg_attr));
	return (attr->dispr_attr == NULL) ? (false) : (true);
}

enum rtw_phl_status dispr_set_dispatch_seq(void *dispr, struct phl_msg_attribute *attr,
							struct msg_self_def_seq* seq)
{
	struct cmd_dispatcher *obj = (struct cmd_dispatcher *)dispr;
	void *d = phl_to_drvpriv(obj->phl_info);
	struct dispr_msg_attr *dispr_attr = NULL;

	if (attr == NULL || seq == NULL)
		return RTW_PHL_STATUS_INVALID_PARAM;

	if (alloc_dispr_attr(d, attr) == false)
		goto err_attr_alloc;

	dispr_attr = attr->dispr_attr;
	reset_self_def_seq(d, &(dispr_attr->self_def_seq));

	if (copy_self_def_seq(d, &(dispr_attr->self_def_seq), seq) == false)
		goto err_seq_copy;
	return RTW_PHL_STATUS_SUCCESS;
err_seq_copy:
	free_dispr_attr(d, &(attr->dispr_attr));
err_attr_alloc:
	PHL_TRACE(COMP_PHL_CMDDISP, _PHL_WARNING_,
			"%s[%d], err\n", __func__, obj->idx);
	return RTW_PHL_STATUS_RESOURCE;
}

static void free_dispr_attr(void *d, struct dispr_msg_attr **dispr_attr)
{
	struct dispr_msg_attr *attr = NULL;

	if (dispr_attr == NULL || *dispr_attr == NULL)
		return;
	PHL_TRACE(COMP_PHL_CMDDISP, _PHL_INFO_, "%s[%d]\n", __func__, obj->idx);
	attr = *dispr_attr;
	reset_self_def_seq(d, &(attr->self_def_seq));
	_os_kmem_free(d, attr, sizeof(struct dispr_msg_attr));
	*dispr_attr = NULL;
}
#endif
#endif

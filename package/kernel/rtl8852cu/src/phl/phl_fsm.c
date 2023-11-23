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

#ifdef CONFIG_FSM

#define PHL_DEBUG_FSM

#define CLOCK_NUM 10
#define CLOCK_UNIT 10
#define IS_CLK_OFF(clk) (clk->remain < 0) /* Negative value means disabled */
#define IS_CLK_ON(clk) (clk->remain >= 0)
#define IS_CLK_EXP(clk) (clk->remain < (CLOCK_UNIT >> 1)) /* expire */

#define MIN(X, Y) (((X) < (Y)) ? (X) : (Y))
#define MAX(X, Y) (((X) > (Y)) ? (X) : (Y))
#define pstr(s) (s +_os_strlen((u8 *)s))
#define lstr(s, l) (size_t)(l - _os_strlen((u8 *)s))

#define FSM_INITIAL_STATE 0

#ifdef FSM_DBG_MEM_OVERWRITE
void *fsm_kmalloc(u32 sz)
{
	char *ptr;

	ptr = kmalloc(sz+4, GFP_KERNEL);
	memset(ptr+sz, 0xff, 4);
	PHL_INFO("+AA %p %d\n", ptr, sz);
	return ptr;
}

void fsm_kfree(void *ptr, u32 sz)
{
	u32 ptn = 0xffffffff;
	u32 *p = (u32 *)(ptr+sz);

	PHL_INFO("-AA %p %d", ptr, sz);
	if ((*p&ptn) != ptn) {
		PHL_ERR("- %p %d", ptr, sz);
		PHL_ERR("OVER WRITE %x\n", ptn);
	}
	kfree(ptr);
}
#define _os_kmem_alloc(a, b) fsm_kmalloc(b)
#define _os_kmem_free(a, b, c) fsm_kfree(b, c)
#endif

struct fsm_event_ent int_event_tbl[] = {
	EV_ENT(FSM_INT_EV_MASK),
	EV_ENT(FSM_EV_CANCEL),
	EV_ENT(FSM_EV_TIMER_EXPIRE),
	EV_ENT(FSM_EV_END),

	EV_ENT(FSM_EV_SWITCH_IN),
	EV_ENT(FSM_EV_SWITCH_OUT),
	EV_ENT(FSM_EV_STATE_IN),
	EV_ENT(FSM_EV_STATE_OUT),

	/* Global event for announcement */
	/* BE CAUREFUL the EVENT ORDER
	 * please also modify enum FSM_EV_ID{} in phl_fsm.h
	 */
	EV_ENT(FSM_GB_SCAN_START),
	EV_ENT(FSM_GB_SCAN_COMPLETE),

	EV_ENT(FSM_EV_MAX)
};

/*
 * FSM status
 */
enum FSM_STATUS {
	FSM_STATUS_NONE,	/* default value */

	FSM_STATUS_INITIALIZED, /* insert module ok,
				 * all mem/queue/timer were allocated
				 * has a pending thread
				 * phl_fsm_new_fsm()
				 * phl_fsm_stop_fsm()
				 */

	FSM_STATUS_READY,	/* interface up, schedule thread, timer.
				 * Does NOT receive message
				 * phl_fsm_start_fsm()
				 */

	FSM_STATUS_ENABLE,	/* Normal running; Reack msg
				 * Internal use
				 * fsm_enable()
				 */

	FSM_STATUS_DISABLE,	/* Does NOT reack msg, able to receiving msg
				 * Internal use
				 * fsm_disable()
				 */
};

/* @obj: obj that will be infomred to when time's up
 * @counter: clock time period
 * @event: event that will delivered when time's up
 * @end: end time
 * @pause: stop countdown
 */
struct fsm_clock {
	u16 event;
	void *priv;
	u8 pause;
	u32 start;
	u32 end;
	int remain; /* ms */
};

struct fsm_queue {
	struct	list_head q;
	_os_lock lock;
};

struct fsm_main {
	struct list_head list;
	char name[FSM_NAME_LEN];
	u8 status;
	u8 obj_cnt;
	u8 oid_seq; /* starts from 1 */
	u8 en_clock_num;
	_os_lock clock_lock;

	_os_thread thread;
	_os_timer fsm_timer; /* unit in ms */

	struct fsm_root *root;
	struct fsm_queue obj_queue;
	struct fsm_queue msg_queue;
	struct fsm_queue evt_queue;
	_os_sema msg_ready;
	bool should_stop;

	/* extra custom queue; for fsm private */
	struct fsm_queue ext_queue;

	struct phl_info_t *phl_info; /* phl_info */
	struct rtw_phl_fsm_tb tb;
};

/*
 * @event: event id
 * @param: additional param of the event
 * @param_sz: param size
 */
struct fsm_evt {
	struct list_head list;
	u16 event; /* event id */
	struct fsm_main *fsm;

	void *param;
	int param_sz;
};

/* @obj_id: object id
 * @state: current state
 * @prive: object's private date
 */
struct fsm_obj {
	struct list_head list;
	u8 oid;
	u8 state;
	char name[FSM_NAME_LEN];
	struct fsm_clock clock[CLOCK_NUM];
	struct fsm_main *fsm;

	void *custom_obj;
	int custom_len; /* custom obj length */

	/* Global event use */
	struct gbl_param my_gbl_req; /* my announcemnt to all */
	struct fsm_queue gbl_queue; /* all received global events */
	u16 gbl_q_len; /* number of received global event */
};

/* Main structure to handle all standalone fsm */
struct fsm_root {
	_os_thread thread;
	struct list_head list;
	struct fsm_queue q_share_thd;
	struct fsm_queue q_alone_thd;
	struct phl_info_t *phl_info;
	u8 gbl_seq;

	_os_sema msg_ready;

	u32 status; /* refer to enum FSM_ROOT_STATUS_FLAGS */
};

/* Static function porto type */
static int fsm_handler(struct fsm_main *fsm);
static char *fsm_state_name(struct fsm_main *fsm, u8 state);
static u8 fsm_get_evt_level(struct fsm_main *fsm, u16 event);

static void fsm_status_set(struct fsm_main *fsm, enum FSM_STATUS status)
{
	fsm->status = status;
}

static enum FSM_STATUS fsm_status(struct fsm_main *fsm)
{
	return fsm->status;
}

/* unit ms */
u32 phl_fsm_time_pass(u32 start)
{
	u32 now = _os_get_cur_time_ms();
	u32 pass;

	if (start <= now)
		pass = now - start;
	else
		pass = 0xffffffff - start + now;

	return pass;
}

u32 phl_fsm_time_left(u32 start, u32 end)
{
	u32 total, pass;
	int left = 0;

	pass = phl_fsm_time_pass(start);

	if (end >= start)
		total = end - start;
	else
		total = 0xffffffff - start + end;

	left = total - pass;

	if (left < 0)
		left = 0;

	return (u32)left;
}

#if 0
static struct fsm_main *fsm_dequeue_fsm(struct fsm_root *root, u8 fsm_mode)
{
	void *d = phl_to_drvpriv(root->phl_info);
	struct fsm_main *fsm;
	struct fsm_queue *queue = (fsm_mode == FSM_ALONE_THREAD) ?
					(&root->q_alone_thd) : (&root->q_share_thd);

	if (list_empty(&queue->q))
		return NULL;

	_os_spinlock(d, &queue->lock, _bh, NULL);
	fsm = list_first_entry(&queue->q, struct fsm_main, list);
	list_del(&fsm->list);
	_os_spinunlock(d, &queue->lock, _bh, NULL);
	return fsm;
}

static struct fsm_obj *fsm_dequeue_obj(struct fsm_main *fsm)
{
	void *d = phl_to_drvpriv(fsm->phl_info);
	struct fsm_obj *obj;

	if (list_empty(&fsm->obj_queue.q))
		return NULL;

	_os_spinlock(d, &fsm->obj_queue.lock, _bh, NULL);
	obj = list_first_entry(&fsm->obj_queue.q, struct fsm_obj, list);
	list_del(&obj->list);
	_os_spinunlock(d, &fsm->obj_queue.lock, _bh, NULL);
	return obj;
}
#endif

static struct fsm_msg *fsm_dequeue_msg(struct fsm_main *fsm)
{
	void *d = phl_to_drvpriv(fsm->phl_info);
	struct fsm_msg *msg;

	if (list_empty(&fsm->msg_queue.q))
		return NULL;

	_os_spinlock(d, &fsm->msg_queue.lock, _bh, NULL);
	msg = list_first_entry(&fsm->msg_queue.q, struct fsm_msg, list);
	list_del(&msg->list);
	_os_spinunlock(d, &fsm->msg_queue.lock, _bh, NULL);
	return msg;
}

static struct fsm_evt *fsm_dequeue_evt(struct fsm_main *fsm)
{
	void *d = phl_to_drvpriv(fsm->phl_info);
	struct fsm_evt *evt;

	if (list_empty(&fsm->evt_queue.q))
		return NULL;

	_os_spinlock(d, &fsm->evt_queue.lock, _bh, NULL);
	evt = list_first_entry(&fsm->evt_queue.q, struct fsm_evt, list);
	list_del(&evt->list);
	_os_spinunlock(d, &fsm->evt_queue.lock, _bh, NULL);
	return evt;
}

/* For EXTERNAL application to enqueue message to extra queue (expose)
 *
 * @fsm: fsm that object belonged to
 * @msg: message to be enqueued
 * @to_head: enqueue message to the head
 */
int phl_fsm_enqueue_ext(struct fsm_main *fsm, struct fsm_msg *msg, u8 to_head)
{
	void *d = phl_to_drvpriv(fsm->phl_info);
	struct fsm_queue *queue = &fsm->ext_queue;

	_os_spinlock(d, &queue->lock, _bh, NULL);
	if (to_head)
		list_add(&msg->list, &queue->q);
	else
		list_add_tail(&msg->list, &queue->q);
	_os_spinunlock(d, &queue->lock, _bh, NULL);

	return 0;
}

/* For EXTERNAL application to dequeue message from extra queue (expose)
 *
 * @fsm: fsm that object belonged to
 */
struct fsm_msg *phl_fsm_dequeue_ext(struct fsm_main *fsm)
{
	void *d = phl_to_drvpriv(fsm->phl_info);
	struct fsm_msg *msg;

	if (list_empty(&fsm->ext_queue.q))
		return NULL;

	_os_spinlock(d, &fsm->ext_queue.lock, _bh, NULL);
	msg = list_first_entry(&fsm->ext_queue.q, struct fsm_msg, list);
	list_del(&msg->list);
	_os_spinunlock(d, &fsm->ext_queue.lock, _bh, NULL);
	return msg;
}

/* For EXTERNAL application to dequeue message from extra queue (expose)
 *
 * @fsm: fsm that object belonged to
 */
int phl_fsm_is_ext_queue_empty(struct fsm_main *fsm)
{
	return list_empty(&fsm->ext_queue.q);
}

static int fsm_new_oid(struct fsm_main *fsm)
{
	u8 oid = fsm->oid_seq++;

	if (fsm->oid_seq == 0xFF) {
		PHL_WARN("%s: reach MAX object ID 0x%x\n",
			fsm->name, oid);
	}
	return oid;
}

static int fsm_enqueue_list(void *d, struct fsm_main *fsm,
	struct fsm_queue *queue, struct list_head *list)
{
	_os_spinlock(d, &queue->lock, _bh, NULL);
	list_add_tail(list, &queue->q);
	_os_spinunlock(d, &queue->lock, _bh, NULL);
	return 0;
}

static enum fsm_run_rtn fsm_state_run(struct fsm_obj *obj,
	u16 event, void *param)
{
	struct fsm_main *fsm = obj->fsm;

	/* TODO protect incorrect event */

	FSM_EV_MSG(fsm, fsm_get_evt_level(fsm, event),
		"%s-%d %-18s    %s\n", fsm->name, obj->oid,
		fsm_state_name(fsm, obj->state), phl_fsm_evt_name(obj, event));

	return fsm->tb.state_tbl[obj->state].fsm_func(obj->custom_obj,
		event, param);
}

static void fsm_remove_all_queuing_msg(struct fsm_main *fsm)
{
	struct fsm_msg *msg;
	struct fsm_evt *evt;
	void *d = phl_to_drvpriv(fsm->phl_info);

	/* go through msg queue and free everything */
	while ((msg = fsm_dequeue_msg(fsm)) != NULL) {
		if (msg->param)
			_os_kmem_free(d, (void *)msg->param, msg->param_sz);
		_os_kmem_free(d, (void *)msg, sizeof(*msg));
	}

	/* go through event queue and free everything */
	while ((evt = fsm_dequeue_evt(fsm)) != NULL) {
		if (evt->param)
			_os_kmem_free(d, (void *)evt->param, evt->param_sz);
		_os_kmem_free(d, (void *)evt, sizeof(*evt));
	}

	/* go through ext queue and free everything */
	while ((msg = phl_fsm_dequeue_ext(fsm)) != NULL) {
		if (msg->param)
			_os_kmem_free(d, (void *)msg->param, msg->param_sz);
		_os_kmem_free(d, (void *)msg, sizeof(*msg));
	}
}

static int fsm_cancel_all_running_obj(struct fsm_main *fsm)
{
	struct fsm_obj *obj;

	phl_list_for_loop(obj, struct fsm_obj, &fsm->obj_queue.q, list) {
		phl_fsm_gen_msg(fsm->phl_info, obj, NULL, 0, FSM_EV_CANCEL);
	}

	return 0;
}

u8 phl_fsm_dbg_level(struct fsm_main *fsm, u8 level)
{
	if (fsm->tb.dbg_level >= level)
		return fsm->tb.dbg_level;

	return 0;
}

u8 phl_fsm_evt_level(struct fsm_main *fsm, u8 level)
{
	if (fsm->tb.evt_level >= level)
		return fsm->tb.evt_level;

	return 0;
}

static u8 fsm_get_evt_level(struct fsm_main *fsm, u16 event)
{
	u16 ev;

	/* fsm internal event */
	if (event & FSM_INT_EV_MASK) {
		ev = (u8)(event & ~(FSM_EV_MASK));
		return int_event_tbl[ev].evt_level;
	}

	if (event == FSM_EV_UNKNOWN)
		return FSM_DBG_INFO;

	if (event > fsm->tb.max_event)
		return FSM_DBG_INFO;

	/* user event */
	return fsm->tb.evt_tbl[event].evt_level;
}

static void fsm_init_queue(void *d, struct fsm_queue *queue)
{
	INIT_LIST_HEAD(&queue->q);
	_os_spinlock_init(d, &queue->lock);
}

static void fsm_deinit_queue(void *d, struct fsm_queue *queue)
{
	_os_spinlock_free(d, &queue->lock);
}

/* For External obj to check sould stop status
 *
 * @fsm: fsm to get state
 */
bool phl_fsm_should_stop(struct fsm_main *fsm)
{
	return fsm->should_stop;
}

int fsm_thread_share(void *param)
{
	struct fsm_main *fsm, *fsm_t;
	struct fsm_root *root = (struct fsm_root *)param;
	void *d = phl_to_drvpriv(root->phl_info);

	while (1) {

		_os_sema_down(d, &root->msg_ready);
		if (_os_thread_check_stop(d, &(root->thread)))
			break;
		phl_list_for_loop_safe(fsm, fsm_t,
			struct fsm_main, &root->q_share_thd.q, list) {
			if (fsm_status(fsm) == FSM_STATUS_ENABLE)
				fsm_handler(fsm);
		}
	}
	_os_thread_wait_stop(d, &root->thread);
	PHL_INFO("fsm: [root] thread down\n");

	return 0;
}

int fsm_thread_alone(void *param)
{
	struct fsm_main *fsm = (struct fsm_main *)param;
	void *d = phl_to_drvpriv(fsm->phl_info);

	while (1) {

		_os_sema_down(d, &fsm->msg_ready);
		if (_os_thread_check_stop(d, &(fsm->thread)))
			break;

		if (fsm_status(fsm) == FSM_STATUS_ENABLE)
			fsm_handler(fsm);
	}
	_os_thread_wait_stop(d, &fsm->thread);
	FSM_INFO(fsm, "fsm: [%s] thread down\n", fsm->name);

	return 0;
}

static struct fsm_obj *fsm_get_obj(struct fsm_main *fsm, u8 oid)
{
	struct fsm_obj *obj, *obj_t;
	void *d = phl_to_drvpriv(fsm->phl_info);


	_os_spinlock(d, &fsm->obj_queue.lock, _bh, NULL);
	phl_list_for_loop_safe(obj, obj_t,
		struct fsm_obj, &fsm->obj_queue.q, list) {
		if (oid == (obj->oid)) {
			_os_spinunlock(d, &fsm->obj_queue.lock, _bh, NULL);
			return obj;
		}
	}
	_os_spinunlock(d, &fsm->obj_queue.lock, _bh, NULL);
	return NULL;
}

struct fsm_msg *phl_fsm_new_msg(struct fsm_obj *obj, u16 event)
{
#ifdef PHL_INCLUDE_FSM
	struct fsm_msg *msg = NULL;
	void *d = phl_to_drvpriv(obj->fsm->phl_info);

	if (fsm_status(obj->fsm) != FSM_STATUS_ENABLE) {
		PHL_ERR("%s: is out of service, ignore message %s!\n",
			obj->fsm->name, phl_fsm_evt_name(obj, event));
		return NULL;
	}

	msg = (struct fsm_msg *)_os_kmem_alloc(d, sizeof(*msg));

	if (msg == NULL)
		return NULL;

	_os_mem_set(d, msg, 0, sizeof(*msg));
	msg->event = event;

	if (obj) {
		msg->fsm = obj->fsm;
		msg->oid = obj->oid;
	}
	return msg;
#else
	PHL_WARN("fsm: %s exclude FSM\n", __func__);
	return NULL;
#endif
}

enum rtw_phl_status phl_fsm_sent_msg(struct fsm_obj *obj, struct fsm_msg *msg)
{
	void *d = phl_to_drvpriv(obj->fsm->phl_info);

	if (fsm_status(obj->fsm) != FSM_STATUS_ENABLE) {
		PHL_ERR("fsm: %s is out of service, ignore message %s!\n",
			obj->fsm->name, phl_fsm_evt_name(obj, msg->event));
		return RTW_PHL_STATUS_RESOURCE;
	}
	fsm_enqueue_list(d, obj->fsm, &obj->fsm->msg_queue, &msg->list);

	if (obj->fsm->tb.mode == FSM_ALONE_THREAD)
		_os_sema_up(d, &obj->fsm->msg_ready);
	else
		_os_sema_up(d, &obj->fsm->root->msg_ready);

	return RTW_PHL_STATUS_SUCCESS;
}

static struct fsm_msg *fsm_new_timer_msg(struct fsm_obj *obj,
	u16 event, void *priv)
{
	struct fsm_msg *msg = NULL;
	void *d = phl_to_drvpriv(obj->fsm->phl_info);

	msg = (struct fsm_msg *)_os_kmem_alloc(d, sizeof(*msg));
	if (msg == NULL)
		return msg;

	_os_mem_set(d, msg, 0, sizeof(*msg));
	msg->event = event;
	msg->oid = obj->oid;
	msg->param = priv;

	return msg;
}

static int fsm_post_message(struct fsm_obj *obj, u16 event, void *priv)
{
	struct fsm_msg *msg;
	struct fsm_main *fsm = obj->fsm;
	void *d = phl_to_drvpriv(obj->fsm->phl_info);

	msg = fsm_new_timer_msg(obj, event, priv);
	if (msg == NULL)
		return -1;

	fsm_enqueue_list(d, fsm, &fsm->msg_queue, &msg->list);
	if (obj->fsm->tb.mode == FSM_ALONE_THREAD)
		_os_sema_up(d, &fsm->msg_ready);
	else
		_os_sema_up(d, &fsm->root->msg_ready);
	return 0;
}

void fsm_timer_callback(void *context)
{
	struct fsm_main *fsm = (struct fsm_main *)context;
	void *d = phl_to_drvpriv(fsm->phl_info);
	struct fsm_obj *obj;
	struct fsm_clock *clk;
	int i;

	_os_set_timer(d, &fsm->fsm_timer, CLOCK_UNIT);

	if (fsm->en_clock_num == 0)
		return;

	/* go through clock and descrease timer
	 * if timer was expired, issue event
	 */
	phl_list_for_loop(obj, struct fsm_obj, &fsm->obj_queue.q, list) {

		_os_spinlock(d, &obj->fsm->clock_lock, _bh, NULL);
		for (i = 0; i < CLOCK_NUM; i++) {

			clk = &obj->clock[i];

			if (IS_CLK_OFF(clk) || clk->pause)
				continue;

			clk->remain = (int)phl_fsm_time_left(clk->start,
				clk->end);
			//(clk->remain < 0 ) ? 0 : clk->remain;

			/* timer expired */
			if (!IS_CLK_EXP(clk))
				continue;
#ifdef PHL_DBG_FSM
			FSM_DBG(obj->fsm, "%s: expire in %d ms\n",
				phl_fsm_evt_name(obj, clk->event),
				phl_fsm_time_pass(clk->start));
#endif
			clk->end = 0;
			clk->remain = -1;
			/* send message to obj */

			/* check fsm status before posting */
			if (fsm_status(fsm) != FSM_STATUS_INITIALIZED &&
			    fsm_status(fsm) != FSM_STATUS_DISABLE)
				fsm_post_message(obj, clk->event, clk->priv);

			fsm->en_clock_num--;
		}
		_os_spinunlock(d, &obj->fsm->clock_lock, _bh, NULL);
	}
}

/* allocate and init fsm resource */
struct fsm_main *phl_fsm_init_fsm(struct fsm_root *root, const char *name,
	void *priv, struct rtw_phl_fsm_tb *tb)
{
#ifdef PHL_INCLUDE_FSM
	struct fsm_main *fsm;
	struct phl_info_t *phl_info = (struct phl_info_t *)priv;
	void *d = phl_to_drvpriv(phl_info);
	//char name_t[FSM_NAME_LEN+10];

	/* check event table */
	if (tb->evt_tbl[tb->max_event-1].event != tb->max_event-1) {
		PHL_ERR("Event mismatch ? Is max event = %d != %d ?\n",
			tb->evt_tbl[tb->max_event-1].event,
			tb->max_event-1);
		return NULL;
	}

	/* check state table */
	if (tb->state_tbl[tb->max_state-1].state != tb->max_state-1) {
		PHL_ERR("State mismatch ? Is max state = %d != %d) ?\n",
			tb->state_tbl[tb->max_state-1].state,
			tb->max_state-1);
		return NULL;
	}

	fsm = (struct fsm_main *)_os_kmem_alloc(d, sizeof(*fsm));

	if (fsm == NULL)
		return NULL;

	_os_mem_set(d, fsm, 0, sizeof(*fsm));
	_os_mem_cpy(d, &fsm->tb, (void *)tb, sizeof(*tb));
	_os_mem_cpy(d, &fsm->name, (void *)name,
		MIN(FSM_NAME_LEN-1, _os_strlen((u8 *)name)));

	fsm->root = root;
	fsm->phl_info = phl_info;

	fsm_init_queue(d, &(fsm->obj_queue));
	fsm_init_queue(d, &(fsm->msg_queue));
	fsm_init_queue(d, &(fsm->evt_queue));
	fsm_init_queue(d, &(fsm->ext_queue));
	_os_spinlock_init(d, &fsm->clock_lock);

	_os_init_timer(d, &fsm->fsm_timer, fsm_timer_callback, fsm, "fsm");
	fsm->oid_seq = 1;

	/* link fsm_main to fsm_root */
	if (tb->mode == FSM_ALONE_THREAD) {
		_os_sema_init(d, &fsm->msg_ready, 0);
		fsm_enqueue_list(d, fsm, &root->q_alone_thd, &fsm->list);
	} else
		fsm_enqueue_list(d, fsm, &root->q_share_thd, &fsm->list);

	FSM_INFO(fsm, "fsm: [%s] initialized\n", fsm->name);
	fsm_status_set(fsm, FSM_STATUS_INITIALIZED);
	return fsm;
#else
	PHL_WARN("fsm: %s exclude FSM\n", __func__);
	return NULL;
#endif /* PHL_INCLUDE_FSM */
}

/* For EXTERNAL application to deinit fsm (expose)
 * @fsm: see struct fsm_main
 */
enum rtw_phl_status phl_fsm_deinit_fsm(struct fsm_main *fsm)
{
	void *d = phl_to_drvpriv(fsm->phl_info);
	struct fsm_obj *obj, *obj_t;

	_os_release_timer(d, &fsm->fsm_timer);

	/* remove fsm form link list */
	list_del(&fsm->list);

	phl_list_for_loop_safe(obj, obj_t,
		struct fsm_obj, &fsm->obj_queue.q, list) {

		list_del(&obj->list);
		phl_fsm_flush_gbl(obj);
		fsm_deinit_queue(d, &(obj->gbl_queue));
		/* free custom_obj */
		_os_kmem_free(d, obj->custom_obj, obj->custom_len);

		/* free fsm_obj */
		_os_kmem_free(d, obj, sizeof(*obj));
	}
	fsm_deinit_queue(d, &(fsm->obj_queue));
	fsm_deinit_queue(d, &(fsm->msg_queue));
	fsm_deinit_queue(d, &(fsm->evt_queue));
	fsm_deinit_queue(d, &(fsm->ext_queue));
	_os_spinlock_free(d, &fsm->clock_lock);

	if (fsm->tb.mode == FSM_ALONE_THREAD)
		_os_sema_free(d, &fsm->msg_ready);

	FSM_INFO(fsm, "fsm: [%s] uninitilized\n", fsm->name);
	_os_kmem_free(d, fsm, sizeof(*fsm));

	return RTW_PHL_STATUS_SUCCESS;
}

char *phl_fsm_evt_name(struct fsm_obj *obj, u16 event)
{
	struct fsm_main *fsm = obj->fsm;
	u8 ev;

	/* TODO handle global, internal, user event */
	/* global event */
	if (event & FSM_GBL_EV_MASK)
		return "global";

	/* fsm internal event */
	if (event & FSM_INT_EV_MASK) {
		ev = (u8)(event & ~(FSM_EV_MASK));
		return int_event_tbl[ev].name;
	}

	if (event == FSM_EV_UNKNOWN)
		return "FSM_EV_UNKNOWN";

	if (event > fsm->tb.max_event)
		return "undefine";

	/* user event */
	return fsm->tb.evt_tbl[event].name;
}

static char *fsm_state_name(struct fsm_main *fsm, u8 state)
{
	if (state > fsm->tb.max_state)
		return "unknown";

	return fsm->tb.state_tbl[state].name;
}

/* For EXTERNAL application to get state id (expose)
 *
 * @obj: obj to get state
 */
u8 phl_fsm_state_id(struct fsm_obj *obj)
{
	return obj->state;
}

/**  init obj internal variable
 *
 * @fsm: fsm that object belonged to
 * default init to the 1st state in state_tbl

 */
static void fsm_obj_switch_in(struct fsm_obj *obj)
{
	struct fsm_main *fsm = obj->fsm;
	//void *d = phl_to_drvpriv(fsm->phl_info);

	/* default init to the 1st state in state_tbl */
	obj->state = fsm->tb.state_tbl[0].state;
	FSM_INFO(fsm, "%s-%d %-18s -> %s\n", fsm->name, obj->oid,
		"switch in", fsm_state_name(fsm, obj->state));

	/* make it alive! Hello OBJ! */
	fsm_state_run(obj, FSM_EV_SWITCH_IN, NULL);
}

/**  deinit obj internal variable
 *
 * @fsm: fsm that object belonged to
 * default init to the 1st state in state_tbl

 */
static void fsm_obj_switch_out(struct fsm_obj *obj)
{
	struct fsm_main *fsm = obj->fsm;
	//void *d = phl_to_drvpriv(fsm->phl_info);

	/* default init to the 1st state in state_tbl */
	obj->state = fsm->tb.state_tbl[0].state;
	FSM_INFO(fsm, "%s-%d %-18s -> %s\n", fsm->name, obj->oid,
		"switch out", fsm_state_name(fsm, obj->state));

	/* make it alive! Hello OBJ! */
	fsm_state_run(obj, FSM_EV_SWITCH_OUT, NULL);
}

/* For EXTERNAL application to new a fsm object (expose)
 *
 * @fsm: fsm that object belonged to
 * @fsm_obj: obj param when calling FSM framework function
 * @priv_len: custom obj length
 *
 * return value: NULL	:fail
 *		 other	:cusomer obj handler (success)
 */
void *phl_fsm_new_obj(struct fsm_main *fsm,
	void **fsm_obj, int sz)
{
#ifdef PHL_INCLUDE_FSM
	void *d = phl_to_drvpriv(fsm->phl_info);
	struct fsm_obj *obj;
	int i;

	obj = (struct fsm_obj *)_os_kmem_alloc(d, sizeof(*obj));
	if (obj == NULL)
		return NULL;

	_os_mem_set(d, obj, 0, sizeof(*obj));
	obj->custom_obj = _os_kmem_alloc(d, sz);

	if (obj->custom_obj == NULL) {
		_os_kmem_free(d, obj, sizeof(*obj));
		return NULL;
	}
	_os_mem_set(d, obj->custom_obj, 0, sz);

	for (i = 0; i < CLOCK_NUM; i++)
		obj->clock[i].remain = -1; /* Negative means disable */

	fsm_init_queue(d, &(obj->gbl_queue));
	obj->custom_len = sz;
	obj->oid = (u8)fsm_new_oid(fsm);
	obj->fsm = fsm;

	_os_mem_set(d, obj->name, 0, FSM_NAME_LEN);
	_os_snprintf(obj->name, FSM_NAME_LEN,
		"%s-%d", obj->fsm->name, obj->oid);
	*fsm_obj = obj;
	fsm_enqueue_list(d, fsm, &fsm->obj_queue, &obj->list);

	return obj->custom_obj;
#else
	PHL_WARN("fsm: %s exclude FSM\n", __func__);
	return NULL;
#endif /* PHL_INCLUDE_FSM */
}

/* For EXTERNAL application to destory a fsm object (expose)
 *
 * @fsm_obj: obj param when calling FSM framework function
 */
void phl_fsm_destory_obj(struct fsm_obj *obj)
{
	struct fsm_main *fsm = obj->fsm;
	void *d = phl_to_drvpriv(fsm->phl_info);

	list_del(&obj->list);
	phl_fsm_flush_gbl(obj);
	fsm_deinit_queue(d, &(obj->gbl_queue));

	/* free custom_obj */
	_os_kmem_free(d, obj->custom_obj, obj->custom_len);

	/* free fsm_obj */
	_os_kmem_free(d, obj, sizeof(*obj));
}

bool phl_fsm_is_alarm_off_ext(struct fsm_obj *obj, u8 id)
{
	struct fsm_clock *clock = &obj->clock[id];

	return IS_CLK_OFF(clock);
}

bool phl_fsm_is_alarm_off(struct fsm_obj *obj)
{
	struct fsm_clock *clock = &obj->clock[0];

	return IS_CLK_OFF(clock);
}

static void fsm_set_alarm(struct fsm_obj *obj, int ms,
	u16 event, u8 id, void *priv)
{
	void *d = phl_to_drvpriv(obj->fsm->phl_info);
	struct fsm_clock *clock = &obj->clock[id];
	u32 now;

	if (ms == 0)
		fsm_post_message(obj, event, priv);

	_os_spinlock(d, &obj->fsm->clock_lock, _bh, NULL);
	/* turn on clock from off */
	if (IS_CLK_OFF(clock))
		obj->fsm->en_clock_num++;

	now = _os_get_cur_time_ms();
	clock->event = event;
	clock->priv = priv;
	clock->start = now;
	clock->end = now + ms;
	clock->remain = (int)phl_fsm_time_left(clock->start, clock->end);
	_os_spinunlock(d, &obj->fsm->clock_lock, _bh, NULL);

#ifdef PHL_DBG_FSM
	FSM_DBG(obj->fsm, "%s:%s now=0x%08x, end=0x%08x, remain=0x%08x\n",
		phl_fsm_obj_name(obj), phl_fsm_evt_name(obj, event),
		clock->start, clock->end, clock->remain);
#endif
}

/* For EXTERNAL application to extend alarm time (expose)
 *
 * @obj: obj param when calling FSM framework function
 * @event: alarm will issue this event while timer expired
 * @ms: time period for the alarm
 *	remain time does not less than 'ms'
 * @id: alarm id; start from 1
 */
void phl_fsm_extend_alarm_ext(struct fsm_obj *obj, int ms, u8 id)

{
	struct fsm_clock *clk = &obj->clock[id];
	int remain = ms;

	if (id == 0 || id >= CLOCK_NUM) {
		PHL_ERR("%s: %s_%d fail\n",
			phl_fsm_obj_name(obj), __func__, id);
		return;
	}

	if (IS_CLK_OFF(clk))
		return;

	remain = MAX((int)phl_fsm_time_left(clk->start, clk->end), ms);
	phl_fsm_set_alarm_ext(obj, remain, clk->event, id, clk->priv);
}

/* For EXTERNAL application to setup alarm (expose)
 *
 * @obj: obj param when calling FSM framework function
 * @event: alarm will issue this event while timer expired
 * @ms: time period for the alarm
 * @id: alarm id; start from 1
 */
void phl_fsm_set_alarm(struct fsm_obj *obj, int ms, u16 event)
{
	fsm_set_alarm(obj, ms, event, 0, NULL);
}

/* For EXTERNAL application to setup alarm_ext (expose)
 *
 * @obj: obj param when calling FSM framework function
 * @event: alarm will issue this event while timer expired
 * @ms: time period for the alarm
 * @id: alarm id; start from 1
 * @priv: priv from caller
 */
void phl_fsm_set_alarm_ext(struct fsm_obj *obj,
	int ms, u16 event, u8 id, void *priv)
{
	if (id >= CLOCK_NUM) {
		PHL_ERR("%s: set alarm_ext_%d to %d ms fail\n",
			phl_fsm_obj_name(obj), id, ms);
		return;
	}
	fsm_set_alarm(obj, ms, event, id, priv);
}

static void fsm_cancel_alarm(struct fsm_obj *obj, u8 id)
{
	void *d = phl_to_drvpriv(obj->fsm->phl_info);
	struct fsm_clock *clock = &obj->clock[id];

	_os_spinlock(d, &obj->fsm->clock_lock, _bh, NULL);
	/* turn off clock from on */
	if (IS_CLK_ON(clock))
		obj->fsm->en_clock_num--;

	//obj->clock[id].counter = -1;
	obj->clock[id].end = 0;
	obj->clock[id].remain = -1;
	obj->clock[id].pause = 0;
	_os_spinunlock(d, &obj->fsm->clock_lock, _bh, NULL);
}

/* For EXTERNAL application to cancel alarm (expose)
 *
 * @obj: obj param when calling FSM framework function
 */
void phl_fsm_cancel_alarm(struct fsm_obj *obj)
{
	fsm_cancel_alarm(obj, 0);
}

/* For EXTERNAL application to cancel alarm_ext (expose)
 *
 * @obj: obj param when calling FSM framework function
 * @id: alarm id; start from 1
 */
void phl_fsm_cancel_alarm_ext(struct fsm_obj *obj, u8 id)
{
	if (id == 0 || id >= CLOCK_NUM) {
		PHL_ERR("%s: cancel alarm_ext_%d fail\n",
			phl_fsm_obj_name(obj), id);
		return;
	}
	fsm_cancel_alarm(obj, id);
}

static void fsm_pause_alarm(struct fsm_obj *obj, u8 id)
{
	void *d = phl_to_drvpriv(obj->fsm->phl_info);

	_os_spinlock(d, &obj->fsm->clock_lock, _bh, NULL);
	obj->clock[id].pause = 1;
	_os_spinunlock(d, &obj->fsm->clock_lock, _bh, NULL);
}

/* For EXTERNAL application to pause alarm (expose)
 *
 * @obj: obj param when calling FSM framework function
 */
void phl_fsm_pause_alarm(struct fsm_obj *obj)
{
	fsm_pause_alarm(obj, 0);
}

/* For EXTERNAL application to pause alarm_ext (expose)
 *
 * @obj: obj param when calling FSM framework function
 * @id: alarm id; start from 1
 */
void phl_fsm_pause_alarm_ext(struct fsm_obj *obj, u8 id)
{
	if (id == 0 || id >= CLOCK_NUM) {
		PHL_ERR("%s: pause alarm_%d fail\n", phl_fsm_obj_name(obj), id);
		return;
	}
	fsm_pause_alarm(obj, id);
}

static void fsm_resume_alarm(struct fsm_obj *obj, u8 id)
{
	void *d = phl_to_drvpriv(obj->fsm->phl_info);
	u32 cur = _os_get_cur_time_ms();

	/* extrend end time */
	_os_spinlock(d, &obj->fsm->clock_lock, _bh, NULL);
	obj->clock[id].end = cur + obj->clock[id].remain;
	obj->clock[id].pause = 0;
	_os_spinunlock(d, &obj->fsm->clock_lock, _bh, NULL);
}

/* For EXTERNAL application to resume alarm (expose)
 *
 * @obj: obj param when calling FSM framework function
 */
void phl_fsm_resume_alarm(struct fsm_obj *obj)
{
	fsm_resume_alarm(obj, 0);
}

/* For EXTERNAL application to resume alarm_ext (expose)
 *
 * @obj: obj param when calling FSM framework function
 * @id: alarm id; start from 1
 */
void phl_fsm_resume_alarm_ext(struct fsm_obj *obj, u8 id)
{
	if (id == 0 || id >= CLOCK_NUM) {
		PHL_ERR("%s: resume alarm_ext_%d fail\n",
			phl_fsm_obj_name(obj), id);
		return;
	}
	fsm_resume_alarm(obj, id);
}

/* For EXTERNAL application to change state (expose)
 *
 * @obj: obj that changes state
 * @new_state: new state
 */
void phl_fsm_state_goto(struct fsm_obj *obj, u8 new_state)
{
	struct fsm_main *fsm = NULL;

	if (obj->state == new_state)
		return;

	fsm = obj->fsm;

	fsm_state_run(obj, FSM_EV_STATE_OUT, NULL);

	FSM_MSG(fsm, FSM_DBG_DBG, "\n");
	FSM_MSG(fsm, FSM_DBG_DBG, "%s-%d %-18s -> %s\n", fsm->name, obj->oid,
		fsm_state_name(fsm, obj->state),
		fsm_state_name(fsm, new_state));

	obj->state = new_state; /* new state */
	fsm_state_run(obj, FSM_EV_STATE_IN, NULL);
}

static void fsm_user_evt_handler(struct fsm_main *fsm)
{
	void *d = phl_to_drvpriv(fsm->phl_info);
	struct fsm_msg *msg;
	struct fsm_obj *obj;
	int rtn = FSM_FREE_PARAM;


	while ((msg = fsm_dequeue_msg(fsm)) != NULL) {

		rtn = FSM_FREE_PARAM;
		obj = fsm_get_obj(fsm, msg->oid);

		if (obj == NULL) {
			PHL_WARN("%s-%d: obj not found\n",
				fsm->name, msg->oid);
			goto obj_not_found;
		}

		/* DO NOT deliver event when fsm->should_stop is true */
		if ((fsm->should_stop == true) &&
			(obj->state == FSM_INITIAL_STATE) &&
			(msg->event < FSM_INT_EV_MASK)) {

			PHL_INFO("%s: should stop skip msg %s\n",
				phl_fsm_obj_name(obj),
				phl_fsm_evt_name(obj, msg->event));
				goto skip_msg;
		}

		/* run state machine */
		rtn = fsm_state_run(obj, msg->event, msg->param);
skip_msg:
obj_not_found:
		if ((rtn == FSM_FREE_PARAM) &&
			(msg->param_sz > 0) &&
			(msg->param != NULL))
			_os_kmem_free(d, (void *)msg->param, msg->param_sz);
		_os_kmem_free(d, (void *)msg, sizeof(*msg));
	}
}

static int fsm_update_status(struct fsm_main *fsm)
{
	struct fsm_obj *obj;

	phl_list_for_loop(obj, struct fsm_obj, &fsm->obj_queue.q, list) {
		if (obj->state != FSM_INITIAL_STATE) {
			PHL_INFO("%s: state %s\n",
				phl_fsm_obj_name(obj),
				fsm_state_name(fsm, obj->state));
			return 0;
		}
	}

	/* all objs are at INITAL_STATE
	 * fsm module is ready to stop
	 */
	fsm_status_set(fsm, FSM_STATUS_INITIALIZED);

	return 0;
}

static int fsm_handler(struct fsm_main *fsm)
{
	/* USER EVENT */
	fsm_user_evt_handler(fsm);

	if (fsm->should_stop == true)
		fsm_update_status(fsm);

	return 0;
}

/* For EXTERNAL application to get fsm name (expose)
 * @fsm: fsm to be get name
 */
char *phl_fsm_fsm_name(struct fsm_main *fsm)
{
	return fsm->name;
}

/* For EXTERNAL application to get obj name (expose)
 * @obj: obj to be get name
 * For example: scan-1 (sacn obj with object id 1)
 */
char *phl_fsm_obj_name(struct fsm_obj *obj)
{
	return obj->name;
}

/* For EXTERNAL application to cancel sma (expose)
 * @obj: obj job will be cancelled
 */
enum rtw_phl_status phl_fsm_cancel_obj(struct fsm_obj *obj)
{
	void *d = phl_to_drvpriv(obj->fsm->phl_info);
	struct fsm_msg *msg;
	int rtn;

	/* NEW message to cancel obj task */
	msg = phl_fsm_new_msg(obj, FSM_EV_CANCEL);
	if (msg == NULL) {
		PHL_ERR("%s: alloc msg fail\n", obj->fsm->name);
		return RTW_PHL_STATUS_RESOURCE;
	}
	rtn = phl_fsm_sent_msg(obj, msg);

	if (rtn != RTW_PHL_STATUS_SUCCESS)
		_os_kmem_free(d, msg, sizeof(*msg));

	return rtn;
}

/* For EXTERNAL application to init FSM framework (expose) */
/* @obj: obj job will be cancelled
 */
struct fsm_root *phl_fsm_init_root(void *priv)
{
#ifdef PHL_INCLUDE_FSM
	struct fsm_root *root;
	struct phl_info_t *phl_info = (struct phl_info_t *)priv;
	void *d = phl_to_drvpriv(phl_info);
	int max, size;

	/* check size of internal event table */
	max = FSM_EV_MAX & ~(int_event_tbl[0].event);
	size = sizeof(int_event_tbl)/sizeof(int_event_tbl)[0];
	if (size != max + 1) {
		PHL_ERR("fsm: int_event_tbl[%d] != %d size mismatch!!",
			size, max);
		return NULL;
	}
	root = (struct fsm_root *)_os_kmem_alloc(d, sizeof(*root));
	if (root == NULL)
		return NULL;

	_os_mem_set(d, root, 0, sizeof(*root));
	fsm_init_queue(d, &(root->q_share_thd));
	fsm_init_queue(d, &(root->q_alone_thd));

	_os_sema_init(d, &root->msg_ready, 0);
	root->phl_info = phl_info;

	PHL_INFO("fsm: [root] initialized\n");
	return root;
#else
	PHL_WARN("fsm: %s exclude FSM\n", __func__);
	return 0;
#endif /* PHL_INCLUDE_FSM */
}

/* For EXTERNAL application to deinit FSM framework (expose)
 * @root: FSM framework handler
 */
void phl_fsm_deinit_root(struct fsm_root *root)
{
#ifdef PHL_INCLUDE_FSM
	void *d = phl_to_drvpriv(root->phl_info);
	void *c = NULL;

	fsm_deinit_queue(d, &(root->q_alone_thd));
	fsm_deinit_queue(d, &(root->q_share_thd));

	_os_sema_free(d, &root->msg_ready);

	/* free fsm_root */
	_os_kmem_free(d, root, sizeof(*root));

	FSM_INFO(c, "fsm: [root] uninitilized\n");
#else
	PHL_WARN("fsm: %s exclude FSM\n", __func__);
#endif /* PHL_INCLUDE_FSM */
}

/* For EXTERNAL application to start fsm root (expose)
 * @fsm: see struct fsm_main
 */
enum rtw_phl_status phl_fsm_start_root(struct fsm_root *root)
{
	void *d = phl_to_drvpriv(root->phl_info);

#ifdef CONFIG_LINUX_THREAD
	root->thread = kthread_create(fsm_thread_share, root,
			"fsm_thread_share");
	wake_up_process(root->thread);
#else
	_os_thread_init(d, &(root->thread), fsm_thread_share, root,
			"fsm_thread_share");
	_os_thread_schedule(d, &(root->thread));
#endif
	return RTW_PHL_STATUS_SUCCESS;
}

/* For EXTERNAL application to stop fsm root (expose)
 * @fsm: see struct fsm_main
 */
enum rtw_phl_status phl_fsm_stop_root(struct fsm_root *root)
{
	void *d = phl_to_drvpriv(root->phl_info);
	void *c = NULL;

	_os_thread_stop(d, &(root->thread));
	_os_sema_up(d, &root->msg_ready);
	_os_thread_deinit(d, &(root->thread));

	FSM_INFO(c, "fsm: [root] stopped\n");

	return RTW_PHL_STATUS_SUCCESS;
}

/* For EXTERNAL application to start fsm (expose)
 * @fsm: see struct fsm_main
 */
enum rtw_phl_status phl_fsm_start_fsm(struct fsm_main *fsm)
{
	void *d = phl_to_drvpriv(fsm->phl_info);
	struct fsm_obj *obj;

	phl_list_for_loop(obj, struct fsm_obj, &fsm->obj_queue.q, list) {
		fsm_obj_switch_in(obj);
	}
	if (fsm->tb.mode == FSM_ALONE_THREAD) {
		_os_thread_init(d, &(fsm->thread), fsm_thread_alone, fsm,
				"fsm_thread_alone");
		_os_thread_schedule(d, &(fsm->thread));
	}

	_os_set_timer(d, &fsm->fsm_timer, CLOCK_UNIT);
	fsm->status = FSM_STATUS_READY;

	fsm_status_set(fsm, FSM_STATUS_ENABLE);
	FSM_INFO(fsm, "fsm: [%s] started\n", fsm->name);

	return RTW_PHL_STATUS_SUCCESS;
}

#define WAIT_DUR 10
#define WAIT_TIMES 20
/* For EXTERNAL application to stop fsm (expose)
 * @fsm: see struct fsm_main
 */
enum rtw_phl_status phl_fsm_stop_fsm(struct fsm_main *fsm)
{
	void *d = phl_to_drvpriv(fsm->phl_info);
	struct fsm_obj *obj;
	int wait = WAIT_TIMES;

	fsm->should_stop = true;

	/* CANCEL all objs within fsm */
	fsm_cancel_all_running_obj(fsm);

	/* wait fsm module finish its task elegantly */
	while ((fsm->status != FSM_STATUS_INITIALIZED) && --wait)
		_os_sleep_ms(d, WAIT_DUR);

	if (wait < (WAIT_TIMES >> 1))
		FSM_INFO(fsm, "%s: take %dms to disable\n",
			fsm->name, (WAIT_TIMES-wait)*WAIT_DUR);

	fsm_status_set(fsm, FSM_STATUS_DISABLE);

	_os_spinlock(d, &fsm->clock_lock, _bh, NULL);
	_os_cancel_timer(d, &fsm->fsm_timer);
	_os_spinunlock(d, &fsm->clock_lock, _bh, NULL);

	phl_list_for_loop(obj, struct fsm_obj, &fsm->obj_queue.q, list) {
		fsm_obj_switch_out(obj);
		phl_fsm_flush_gbl(obj);
	}
	fsm_remove_all_queuing_msg(fsm);

	if (fsm->tb.mode == FSM_ALONE_THREAD) {
		_os_thread_stop(d, &(fsm->thread));
		_os_sema_up(d, &fsm->msg_ready);
		_os_thread_deinit(d, &(fsm->thread));
	}
	fsm->should_stop = false;

	FSM_INFO(fsm, "fsm: [%s] stopped\n", fsm->name);

	return RTW_PHL_STATUS_SUCCESS;
}

/* For EXTERNAL application to generate message buffer (expose)
 * Generate message quickly and simply
 * @phl: phl_info_t
 * @obj: fsm_obj (msg receiver)
 * @pbuf: message parameter
 * @sz: message parameter size
 * @event: event for the message
 */
enum rtw_phl_status phl_fsm_gen_msg(void *phl, struct fsm_obj *obj,
	void *pbuf, u32 sz, u16 event)
{
#ifdef PHL_INCLUDE_FSM
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;
	struct fsm_msg *msg;
	void *d = phl_to_drvpriv(phl_info);
	void *param = NULL;
	int rtn = RTW_PHL_STATUS_RESOURCE;

	/* NEW mem for message */
	msg = phl_fsm_new_msg(obj, event);
	if (msg == NULL) {
		FSM_ERR(obj->fsm, "%s: alloc msg %s fail\n",
			phl_fsm_obj_name(obj),
			phl_fsm_evt_name(obj, event));
		goto msg_fail;
	}

	/* NEW mem for param */
	if (pbuf && sz) {
		param = _os_kmem_alloc(d, sz);
		if (param == NULL) {
			FSM_ERR(obj->fsm,
				"%s: alloc param %s fail\n",
				phl_fsm_obj_name(obj),
				phl_fsm_evt_name(obj, event));
			goto param_fail;
		}
		_os_mem_cpy(d, param, pbuf, sz);
	}
	msg->param = (void *)param;
	msg->param_sz = sz;

	rtn = phl_fsm_sent_msg(obj, msg);

	if (rtn != RTW_PHL_STATUS_SUCCESS)
		goto send_fail;

	return rtn;

send_fail:
	if (msg->param && msg->param_sz)
		_os_kmem_free(d, msg->param, msg->param_sz);
param_fail:
	_os_kmem_free(d, msg, sizeof(*msg));
msg_fail:

	return rtn;
#else
	PHL_WARN("fsm: %s exclude FSM\n", __func__);
	return RTW_PHL_STATUS_FAILURE;
#endif /* PHL_INCLUDE_FSM */
}

enum rtw_phl_status phl_fsm_flush_gbl(struct fsm_obj *obj)
{
	void *d = phl_to_drvpriv(obj->fsm->phl_info);
	struct gbl_param *p, *n;

	_os_mem_set(d, &obj->my_gbl_req, 0, sizeof(obj->my_gbl_req));

	/* flush obj->gbl_queue */
	phl_list_for_loop_safe(p, n,
		struct gbl_param, &obj->gbl_queue.q, list) {

		list_del(&p->list);
		FSM_WARN(obj->fsm, "%s: del non replied %s:%s #%d\n",
			phl_fsm_obj_name(obj),
			phl_fsm_obj_name(p->obj_from),
			phl_fsm_evt_name(obj, p->event), p->seq);

		_os_kmem_free(d, (void *)p, sizeof(*p));
	}
	obj->gbl_q_len = 0;

	return RTW_PHL_STATUS_SUCCESS;
}

/* For EXTERNAL fsm module to announce global msg (expose)
 *
 * !!! ONLY ALLOW fsm MODULE to call !!!
 * !!! Otherwise will have reaing issue !!!
 *
 * Global msg will go throughs all fsm modules
 * Limitation:
 *	Only supports ONE Glboal announcement at a time
 *	The latest one always overwrite previous one
 *
 * reference: phl_fsm_gbl_not_reply_num()
 *
 * @obj: fsm_obj
 * @gbl_evt: Global event to be announced
 * @cb_evt: call back event when things was done
 * return: wait time(ms); 0: success, waiting is not necessary
 *	when wait > 0; callee will reply event to caller within ms
 *	negative value: fail
 */
int phl_fsm_gbl_msg_announce(struct fsm_obj *obj, u16 gbl_evt, u16 cb_evt)
{
	struct fsm_root *root = obj->fsm->root;
	void *d = phl_to_drvpriv(root->phl_info);
	struct fsm_main *fsm = obj->fsm;
	struct fsm_main *fsm_t;
	struct fsm_obj *obj_t;
	int i;

	if (obj->my_gbl_req.count > 0) {
		/* Should not happen!!
		 * Have ongoing announcement
		 * We are waiting for some GBL event reply
		 */
		for (i = 0; i < PHL_FSM_MAX_WAIT_OCUNT; i++) {
			if (obj->my_gbl_req.wait_list[i])
				FSM_WARN(fsm,
					"%s: drop not replied %s:%s #%d\n",
					phl_fsm_obj_name(obj),
					phl_fsm_obj_name(
						obj->my_gbl_req.wait_list[i]),
					phl_fsm_evt_name(obj,
						obj->my_gbl_req.event),
					obj->my_gbl_req.seq);
		}
	}

	/* create param for announcement */
	_os_mem_set(d, &obj->my_gbl_req, 0, sizeof(obj->my_gbl_req));
	obj->my_gbl_req.event = gbl_evt;
	obj->my_gbl_req.cb_evt = cb_evt;
	obj->my_gbl_req.obj_from = obj;
	if (obj->fsm->root->gbl_seq == 0) /* 0 reserved */
		obj->fsm->root->gbl_seq = 1;
	obj->my_gbl_req.seq = obj->fsm->root->gbl_seq++;

	/* GLOBAL EVENT will go through all fsms */
	phl_list_for_loop(fsm_t, struct fsm_main, &root->q_share_thd.q, list) {
		if (fsm_status(fsm_t) != FSM_STATUS_ENABLE) {
			FSM_INFO(fsm_t, "fsm: [%s] disabled, skip %s\n",
				phl_fsm_fsm_name(fsm_t),
				phl_fsm_evt_name(obj, gbl_evt));
				continue;
		}
		/* go through objs */
		phl_list_for_loop(obj_t, struct fsm_obj,
			&fsm_t->obj_queue.q, list) {

			/* skip myself */
			if (obj_t == obj)
				continue;

			fsm_state_run(obj_t, gbl_evt, &obj->my_gbl_req);

			if (obj->my_gbl_req.result < 0) {
				FSM_ERR(fsm_t,
					"%s: announce %s to %s fail(%d)\n",
					phl_fsm_obj_name(obj),
					phl_fsm_evt_name(obj_t, gbl_evt),
					phl_fsm_obj_name(obj_t),
					obj->my_gbl_req.result);

				return obj->my_gbl_req.result;
			}
		}
	}
	return obj->my_gbl_req.wait_ms;
}

/** For GBL announcer to get the number of un-replied fsm (espose)
 *
 * !!! ONLY ALLOW fsm MODULE to call !!!
 *
 * reference: phl_fsm_gbl_msg_announce()
 * @obj: fsm_obj
 * @param: see gbl_param
 * return: 0 means there is no non-reply reqest, it's ready to go;
 *	   otherwise yet ready
 */
int phl_fsm_gbl_not_reply_num(struct fsm_obj *obj, struct gbl_param *param)
{
	if (param == NULL)
		return obj->my_gbl_req.count;

	/* we don't have any waitting reply; GBL may be cancelled earlier */
	if (obj->my_gbl_req.obj_from == NULL) {
		FSM_WARN(obj->fsm, "%s: doesn't expect reply %s:%s #%d\n",
			phl_fsm_obj_name(obj),
			phl_fsm_obj_name(param->obj_to),
			phl_fsm_evt_name(obj, param->event), param->seq);
		return -1;
	}
	/* Are we looking for receiving event ? */
	if (param->event != obj->my_gbl_req.event)
		return -2;

	if (param->seq != obj->my_gbl_req.seq)
		return -3;

	FSM_INFO(obj->fsm, "%s: got reply %s:%s #%d\n",
		phl_fsm_obj_name(obj),
		phl_fsm_obj_name(param->obj_to),
		phl_fsm_evt_name(obj, param->event), param->seq);

	/* clear incoming reporter from waitting list */
	param->wait_list[param->count] = NULL;

	return --obj->my_gbl_req.count;
}

/** For Global event reciver to inform announcer to wait confirmation (espose)
 *
 * !!! ONLY ALLOW fsm MODULE to call !!!
 *
 * Call the function if Global receiver know that it can't finish task in time
 * Global event receiver expect FSM_EV_GBL_REPLY to confirm task is finish
 * reference : phl_fsm_gbl_msg_release()
 *
 * @obj: see fsm_obj
 * @param: see gbl_param
 * @ms: How long(max) can finish task according to received Global event
 *	caller will set an alarm to react if we can't finish the job on time
 * return: negative value : fail
 *	   postive value : seq number of this GBL event
 */
int phl_fsm_gbl_msg_hold(struct fsm_obj *obj,
	struct gbl_param *param, u32 ms)
{
	void *d = phl_to_drvpriv(obj->fsm->phl_info);
	struct gbl_param *p;

	if (param->count >= PHL_FSM_MAX_WAIT_OCUNT) {
		param->result = -(GBL_ST_WAIT_REACH_MAX);
		FSM_ERR(obj->fsm, "%s: hold %s reach max counter %d (%d)",
			phl_fsm_obj_name(obj),
			phl_fsm_evt_name(obj, param->event), param->count,
			param->result);
		return param->result;
	}

	if (obj->gbl_q_len >= PHL_FSM_MAX_WAIT_OCUNT) {
		param->result = -(GBL_ST_REPLY_REACH_MAX);
		FSM_ERR(obj->fsm, "%s: reply %s reach max counter %d (%d)",
			phl_fsm_obj_name(obj),
			phl_fsm_evt_name(obj, param->event),
			obj->gbl_q_len, param->result);
		return param->result;
	}

	p = (struct gbl_param *)_os_kmem_alloc(d, sizeof(*p));

	if (p == NULL) {
		param->result = -GBL_ST_ALLOC_MEM_FAIL;
		FSM_ERR(obj->fsm, "%s: reply %s, alloc mem fail (%d)",
			phl_fsm_obj_name(obj),
			phl_fsm_evt_name(obj, param->event),
			param->result);
		return param->result;
	}

	/* fill info to inform caller that we need time to process */
	param->obj_to = obj;
	param->wait_list[param->count] = obj;
	param->wait_ms = MAX(param->wait_ms, ms);
	param->count++;

	/* save param for replying later */
	_os_mem_cpy(d, p, (void *)param, sizeof(*param));
	fsm_enqueue_list(d, obj->fsm, &obj->gbl_queue, &p->list);

	FSM_DBG(obj->fsm, "%s: require %d ms to handle %s:%s #%d\n",
			phl_fsm_obj_name(obj), ms,
			phl_fsm_obj_name(param->obj_from),
			phl_fsm_evt_name(obj, param->event),
			param->seq);

	return p->seq;
}

/** For Global event reciver to inform announcer that task was done (espose)
 *
 * !!! ONLY ALLOW fsm MODULE to call !!!
 *
 * Call the function when Global receiver finish the task
 * This is a ASYNC confirmation to Global event announcer
 * Global event announcer will receive FSM_EV_GBL_REPLY when function is called
 * reference: phl_fsm_gbl_msg_hold()
 *
 * @obj: see fsm_obj
 * @param: see gbl_param
 * @obj: see fsm_obj
 * @event: event to be replied
 * @seq: event to be replied
 * @result: result to be replied
 */
enum rtw_phl_status phl_fsm_gbl_msg_release(struct fsm_obj *obj,
	u16 event, u32 seq, enum gbl_evt_result result)
{
	void *d = phl_to_drvpriv(obj->fsm->phl_info);
	struct gbl_param *p, *p_t;

	/* handle multiple Global event requests
	 * go through link list to get reply param according to event
	 */

	phl_list_for_loop_safe(p, p_t,
		struct gbl_param, &obj->gbl_queue.q, list) {
		if ((event == p->event) && (seq == p->seq)) {

			p->result = result;
			FSM_INFO(obj->fsm, "%s: reply %s:%s #%d, result %d\n",
				phl_fsm_obj_name(obj),
				phl_fsm_obj_name(p->obj_from),
				phl_fsm_evt_name(obj, event), p->seq, result);

			phl_fsm_gen_msg(obj->fsm->phl_info, p->obj_from,
				p, sizeof(*p), p->cb_evt);

			list_del(&p->list);
			_os_kmem_free(d, (void *)p, sizeof(*p));
			break;
		}
	}
	return RTW_PHL_STATUS_SUCCESS;
}

/** Debug funcitons
 *
 */
#ifdef PHL_DEBUG_FSM
static void fsm_dbg_dump_fsm_queue(struct fsm_queue *fsmq,
	char *s, int *sz,bool detail)
{
	struct fsm_main *fsm, *fsm_t;

	char *ptr = s;
	int len = *sz;

	phl_list_for_loop_safe(fsm, fsm_t,
		struct fsm_main, &fsmq->q, list) {
		_os_snprintf(pstr(ptr), lstr(ptr, len), "\t%4s : %s\n", fsm->name,
			fsm->tb.mode ? "STANDALONE":"SHARE");

		if (fsm->tb.dump_fsm && detail) {
			len = lstr(ptr, len);
			ptr = pstr(ptr);
			fsm->tb.dump_fsm(fsm, ptr, &len);
		}
	}
	*sz = len;
}

static void fsm_dbg_help(struct fsm_main *fsm, char *s, int *sz, bool detail);
static void fsm_dbg_dump_fsm(struct fsm_main *fsm,
	char *s, int *sz, bool detail)
{
	int len = *sz;
	char *ptr = s;

	_os_snprintf(pstr(ptr), lstr(ptr, len), "\t%4s : %s\n", fsm->name,
		fsm->tb.mode ? "STANDALONE":"SHARE");

	if (fsm->tb.dump_fsm && detail) {
		len = lstr(ptr, len);
		ptr = pstr(ptr);
		fsm->tb.dump_fsm(fsm, ptr, &len);
	}

}

static void fsm_dbg_dump_state(struct fsm_main *fsm,
	char *s, int *sz, bool detail)
{
	int i;
	int len = *sz;

	_os_snprintf(pstr(s), lstr(s, len),
		"[%s] state table\n", fsm->name);
	for (i = 0; i < fsm->tb.max_state; i++)
		_os_snprintf(pstr(s), lstr(s, len), "\t%4d : %s\n",
			i, fsm->tb.state_tbl[i].name);
	*sz = len;
}

static void fsm_dbg_dump_event(struct fsm_main *fsm,
	char *s, int *sz, bool detail)
{
	int i, max;
	int len = *sz;

	/* internal event */
	_os_snprintf(pstr(s), lstr(s, len), "[Internal] event table\n");

	max = FSM_EV_END & ~(int_event_tbl[0].event); /* FSM_INT_EV_MASK */
	for (i = 1; i < max; i++)
		_os_snprintf(pstr(s), lstr(s, len), "\t0x%4x : %s\n",
			int_event_tbl[i].event, int_event_tbl[i].name);

	/* user event */
	_os_snprintf(pstr(s), lstr(s, len), "\n[%s] event table max %d\n", fsm->name, fsm->tb.max_event);
	for (i = 0; i < fsm->tb.max_event-1; i++)
		_os_snprintf(pstr(s), lstr(s, len), "\t0x%4x : %s\n",
			fsm->tb.evt_tbl[i].event, fsm->tb.evt_tbl[i].name);
	*sz = len;
}

static void fsm_dbg_dump_obj(struct fsm_main *fsm,
	char *s, int *sz, bool detail)
{
	struct fsm_obj *obj, *obj_t;
	int len = *sz;
	char *ptr = s;

	phl_list_for_loop_safe(obj, obj_t,
		struct fsm_obj, &fsm->obj_queue.q, list) {

		_os_snprintf(pstr(ptr), lstr(ptr, len), "%s-%d : state %s",
			fsm->name, obj->oid, fsm_state_name(fsm, obj->state));

		if (fsm->tb.dump_obj && detail) {
			len = lstr(ptr, len);
			ptr = pstr(ptr);
			fsm->tb.dump_obj(obj->custom_obj, ptr, &len);
		}
	}
	*sz = len;
}

static void fsm_dbg_max(struct fsm_main *fsm, char *s, int *sz, bool detail)
{
	int len = *sz;

	_os_snprintf(pstr(s), lstr(s, len),
		"ERR: fsm %s sould not run to here!!\n", __func__);
	*sz = len;
}

struct fsm_debug_ent {
	char *opt;
	void (*func)(struct fsm_main *fsm, char *s, int *sz, bool detail);
	char *desc;
};

struct fsm_debug_ent debug_opt[] = {
	{"help", fsm_dbg_help, "help message"},
	{"fsm", fsm_dbg_dump_fsm, "all fsm name"},
	{"st", fsm_dbg_dump_state, "state name"},
	{"ev", fsm_dbg_dump_event, "event name"},
	{"obj", fsm_dbg_dump_obj, "obj detail"},
	{"max", fsm_dbg_max, "max_opt"}
};

static void _fsm_dbg_help(struct fsm_root *root, char *s, int *sz, bool detail)
{
	int i, max_opt;
	int len = *sz;
	char *ptr = s;

	_os_snprintf(pstr(ptr), lstr(ptr, len),
		"usage:\tfsm d <fsm_name> <option>\n");
	_os_snprintf(pstr(ptr), lstr(ptr, len),
		"\tfsm p,<obj_name> <priv_dbg_cmd> ....\n");
	_os_snprintf(pstr(ptr), lstr(ptr, len),
		"\tfsm s,<obj_name> <EVENT>\n");
	_os_snprintf(pstr(ptr), lstr(ptr, len),
		"\tfsm w,<fsm_name> <dbg_level|ev_level> <0-5(dbg)>\n");

	_os_snprintf(pstr(s), lstr(ptr, len), "\nfsm_name:\n");

	len = lstr(ptr, len);
	ptr = pstr(ptr);
	fsm_dbg_dump_fsm_queue(&root->q_share_thd, ptr, &len, detail);

	len = lstr(ptr, len);
	ptr = pstr(ptr);
	fsm_dbg_dump_fsm_queue(&root->q_alone_thd, ptr, &len, detail);

	_os_snprintf(pstr(ptr), lstr(ptr, len), "\noption:\n");
	max_opt = sizeof(debug_opt)/sizeof(debug_opt[0]);
	for (i = 0; i < max_opt-1; i++)
		_os_snprintf(pstr(ptr), lstr(ptr, len), "%12s : %s\n",
			debug_opt[i].opt, debug_opt[i].desc);
	*sz = len;
}

static void fsm_dbg_help(struct fsm_main *fsm, char *s, int *sz, bool detail)
{
	_fsm_dbg_help(fsm->root, s, sz, false);
}

struct fsm_main *get_fsm_by_name(struct fsm_root *root, char *name)
{
	void *d = phl_to_drvpriv(root->phl_info);
	struct fsm_main *fsm, *fsm_t;
	u32 len = _os_strlen((u8 *)name);

	if (len > FSM_NAME_LEN)
		return NULL;

	phl_list_for_loop_safe(fsm, fsm_t,
		struct fsm_main, &root->q_share_thd.q, list) {
		if (_os_strlen((u8 *)fsm->name) == len &&
			_os_mem_cmp(d, fsm->name, name, len) == 0)
			return fsm;
	}

	phl_list_for_loop_safe(fsm, fsm_t,
		struct fsm_main, &root->q_alone_thd.q, list) {
		if (_os_strlen((u8 *)fsm->name) == len &&
			_os_mem_cmp(d, fsm->name, name, len) == 0)
			return fsm;
	}
	return NULL;
}

static u16 fsm_get_evt_id(struct fsm_main *fsm, char *event)
{
	void *d = phl_to_drvpriv(fsm->phl_info);
	int i;
	u32 len = _os_strlen((u8 *)event);

	/* internal event */
	for (i = 0; i < (sizeof(int_event_tbl)/sizeof(int_event_tbl[0])); i++) {
		if (_os_strlen((u8 *)int_event_tbl[i].name) == len &&
			_os_mem_cmp(d, int_event_tbl[i].name, event, len) == 0)
			return int_event_tbl[i].event;
	}

	/* user event */
	for (i = 0; i < fsm->tb.max_event; i++) {
		if (_os_strlen((u8 *)fsm->tb.evt_tbl[i].name) == len &&
			_os_mem_cmp(d,
				fsm->tb.evt_tbl[i].name, event, len) == 0)
			return fsm->tb.evt_tbl[i].event;
	}
	return FSM_EV_UNKNOWN;
}
#endif /* PHL_DEBUG_FSM */

/* For EXTERNAL application to debug fsm (expose)
 * @phl_info: phl main struct
 * @input: input cmd
 * @input_num: num of cmd param
 * @output: output buffer
 * @out_len: MAX output buffer len
 *
 * d: dump fsm info
 *	fsm <d> <fsm_name> <fsm|st|ev|obj>
 * p: private cmd to fsm module
 *	fsm <p> <obj_name> <cmd to fsm module>
 * s: send event to fsm
 *	fsm <s> <obj_name> <ev>
 * w: write debug level
 *	fsm <w> <fsm_name> <dbg_level|evt_level> <0-5>
 */
void phl_fsm_dbg(struct phl_info_t *phl_info, char input[][MAX_ARGV],
		      u32 input_num, char *output, u32 out_len)
{
#ifdef PHL_DEBUG_FSM
	struct phl_info_t *phl = (struct phl_info_t *)phl_info;
	void *d = phl_to_drvpriv(phl);
	struct fsm_root *root = phl->fsm_root;
	struct fsm_main *fsm = NULL;
	struct fsm_obj *obj = NULL;
	struct fsm_msg *msg;
	int i, max_opt, len = out_len;
	char fsm_name[FSM_NAME_LEN], opt[FSM_NAME_LEN], cmd[FSM_NAME_LEN];
	char c, *ptr, *sp;
	u8 obj_id = 0;
	u16 ev_id;

	ptr = output;
	/* fsm <cmd> <fsm_name> <opt> : fsm d cmd ev
	 * fsm <cmd> <fsm_name> <evt> : fsm s cmd-1 FSM_EV_CANCEL
	 */
	if (input_num < 4)
		goto help;

	_os_mem_set(d, cmd, 0, FSM_NAME_LEN);
	_os_mem_cpy(d, cmd, input[1],
		MIN(_os_strlen((u8 *)input[1]), FSM_NAME_LEN));

	_os_mem_set(d, fsm_name, 0, FSM_NAME_LEN);
	_os_mem_cpy(d, fsm_name, input[2],
		MIN(_os_strlen((u8 *)input[2]), FSM_NAME_LEN));

	_os_mem_set(d, opt, 0, FSM_NAME_LEN);
	_os_mem_cpy(d, opt, input[3],
		MIN(_os_strlen((u8 *)input[3]), FSM_NAME_LEN));

	c = (char)*cmd;
	/* read obj_id
	 * if fsm_name is "cmd-1" then obj number is "1"
	 */
	sp = _os_strchr((const char *)fsm_name, '-');

	if (sp != NULL) {
		*sp = '\0';
		if (_os_sscanf(sp+1, "%hhd", &obj_id) != 1) {
			_os_snprintf(pstr(ptr), lstr(ptr, len),
				"ERR: fsm[%s] miss obj_id\n", fsm_name);
			return;
		}
	} else
		obj_id = 1; /* assume obj-1 */

	/* search fsm by name */
	fsm = get_fsm_by_name(root, (char *)fsm_name);
	if (fsm == NULL) {
		_os_snprintf(pstr(ptr), lstr(ptr, len),
			"ERR: fsm[%s] not found\n", fsm_name);
		return;
	}

	obj = fsm_get_obj(fsm, obj_id);
	if (obj == NULL) {
		_os_snprintf(pstr(ptr), lstr(ptr, len),
			"ERR: fsm[%s] miss obj_%d\n", fsm_name, obj_id);
		return;
	}

	switch (c) {
	case 'd':
		/* dump status */
		max_opt = sizeof(debug_opt)/sizeof(debug_opt)[0];
		for (i = 0; i < max_opt-1; i++) {
			if (_os_strlen((u8 *)debug_opt[i].opt) == \
				_os_strlen((u8 *)opt) &&
				_os_mem_cmp(d, debug_opt[i].opt, opt,
				_os_strlen((u8 *)opt)) == 0) {

				len = lstr(ptr, len);
				ptr = pstr(ptr);
				debug_opt[i].func(fsm, ptr, &len, true);
				break;
			}
		}
		break;

	case 'p':
		/* call fsm private degug function */
		if ((fsm != NULL) && (obj != NULL) && (fsm->tb.debug != NULL)){
			len = lstr(ptr, len);
			ptr = pstr(ptr);
			fsm->tb.debug(obj->custom_obj, &input[3],
				(input_num - 3), ptr, (u32 *)&len);
		}
		break;

	case 's':
		/* get event id */
		ev_id = fsm_get_evt_id(fsm, (char *)opt);

		if (ev_id == FSM_EV_UNKNOWN) {
			_os_snprintf(pstr(ptr), lstr(ptr, len),
				"\n\nERR: fsm[%s] unknown event %s\n",
				fsm_name, opt);
			len = lstr(ptr, len);
			ptr = pstr(ptr);
			fsm_dbg_dump_event(fsm, ptr, &len, false);
			break;
		}

		if (obj != NULL) {
			msg = phl_fsm_new_msg(obj, ev_id);

			/* send event */
			if (phl_fsm_sent_msg(obj, msg) != RTW_PHL_STATUS_SUCCESS)
				_os_kmem_free(d, msg, sizeof(*msg));
		}

		break;

	case 'w':
		/* write cfg */
		/* fsm w,<fsm_name>,<dbg_level|ev_level>,<0-5(dbg)> */

		sp = _os_strchr((const char *)opt, ',');
		if (sp == NULL)
			goto help;

		*sp = '\0';
		if (_os_sscanf(sp+1, "%d", &i) != 1)
			goto help;

		if ((i<0) || (i>5))
			goto help;

		if (!_os_strcmp(opt, "dbg_level")) {
			fsm->tb.dbg_level = (u8)i;
			_os_snprintf(pstr(ptr), lstr(ptr, len),
				"\n%s: set debug level to %d\n",
				phl_fsm_fsm_name(fsm), i);
		} else if (!_os_strcmp(opt, "evt_level")) {
			_os_snprintf(pstr(ptr), lstr(ptr, len),
				"\n%s: set event level to %d\n",
				phl_fsm_fsm_name(fsm), i);
			//fsm->tb.evt_level = (u8)i;
		} else
			goto help;
		break;

	default:
		goto help;
	}
	return;
help:
	len = lstr(ptr, len);
	ptr = pstr(ptr);
	_fsm_dbg_help(fsm->root, ptr, &len, false);
#endif /* PHL_DEBUG_FSM */
}
#endif /*CONFIG_FSM*/

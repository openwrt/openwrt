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
 *****************************************************************************/
#ifndef __PHL_FSM_H__
#define __PHL_FSM_H__

#define PHL_INCLUDE_FSM

/* #define PHL_DEBUG_FSM */
/* #define FSM_DBG_MEM_OVERWRITE */

#ifdef FSM_DBG_MEM_OVERWRITE
void *fsm_kmalloc(u32 sz);
void fsm_kfree(void *ptr, u32 sz);
#endif
#define FSM_NAME_LEN		32
#define CLOCK_UNIT              10 /* ms */

struct fsm_root;
struct fsm_main;
struct fsm_obj;

/* event map
 */
#define FSM_EV_MASK	0xff00
#define FSM_USR_EV_MASK	0x0100
#define FSM_INT_EV_MASK	0x0200
#define FSM_GBL_EV_MASK	0x0400
#define FSM_EV_UNKNOWN	0xffff

/* FSM EVENT */
enum FSM_EV_ID {
	/* Expose to all FSM service */
	FSM_INT_EV_MASK_ = FSM_INT_EV_MASK,
	FSM_EV_CANCEL,
	FSM_EV_TIMER_EXPIRE,
	FSM_EV_END, /* for reference */

	FSM_EV_SWITCH_IN,
	FSM_EV_SWITCH_OUT,
	FSM_EV_STATE_IN,
	FSM_EV_STATE_OUT,

	/* Global Events for announcement */
	/* BE CAUREFUL the EVENT ORDER
	 * please also modify int_event_tbl[] in phl_fsm.c
	 */
	FSM_GB_SCAN_START,
	FSM_GB_SCAN_COMPLETE,

	FSM_EV_MAX
};

enum fsm_mode {
	FSM_SHARE_THREAD, /* fsm shares root_fsm thread */
	FSM_ALONE_THREAD /* fsm has its own thread */
};

enum fsm_run_rtn {
	FSM_FREE_PARAM,
	FSM_KEEP_PARAM
};

/* @oid: object id
 * @event: event id
 * @msg: additional message of the event
 * @msg_sz: message size
 */
struct fsm_msg {
	_os_list list;
	u8 oid; /* receiver */
	u16 event; /* event id */
	struct fsm_main *fsm;

	void *param;
	int param_sz;
};

enum fsm_dbg_level {
	FSM_DBG_NONE,
	FSM_DBG_PRINT,
	FSM_DBG_ERR,
	FSM_DBG_WARN,
	FSM_DBG_INFO, /* dbg_level: dump normal info msg */
	FSM_DBG_DBG, /* dbg_level: dump state change info */
	FSM_DBG_MAX
};

#define EV_ENT(ev) {ev, #ev, FSM_DBG_INFO}
#define EV_WRN(ev) {ev, #ev, FSM_DBG_WARN}
#define EV_INF(ev) {ev, #ev, FSM_DBG_INFO}
#define EV_DBG(ev) {ev, #ev, FSM_DBG_DBG}

struct fsm_event_ent {
	u16 event;
	char *name;
	u8 evt_level;
};

#define ST_ENT(st, hdl) {st, #st, hdl}
struct fsm_state_ent {
	u8 state;
	char *name;
	int (*fsm_func)(void *priv, u16 event, void *param);
};

/* struct of phl_fsm_init_fsm() */
struct rtw_phl_fsm_tb {
	u8 mode; /* 0/1: Share/Standalone thread mode */
	u8 dbg_level;
	u8 evt_level;
	u8 max_state;
	u16 max_event;
	struct fsm_state_ent *state_tbl;
	struct fsm_event_ent *evt_tbl;

	/* debug function */
	void (*dump_obj)(void *obj, char *p, int *sz); /* optional */
	void (*dump_fsm)(void *fsm, char *p, int *sz); /* optional */
	void (*debug)(void *custom_obj, char input[][MAX_ARGV],
		      u32 input_num, char *output, u32 *out_len);
};

enum gbl_evt_result {
	GBL_ST_NOT_FINISH,
	GBL_ST_SUCCESS,
	GBL_ST_ABORT,
	GBL_ST_FAIL,
	GBL_ST_WAIT_REACH_MAX,
	GBL_ST_REPLY_REACH_MAX,
	GBL_ST_ALLOC_MEM_FAIL
};

#define PHL_FSM_MAX_WAIT_OCUNT 16
struct gbl_param {
	struct list_head list;
	u16 event;
	u16 cb_evt;
	u16 count;
	u32 wait_ms;
	u32 seq;
	struct fsm_obj *obj_from; /* GBL event original issuer */
	struct fsm_obj *obj_to; /* GBL event original receiver */
	struct fsm_obj *wait_list[PHL_FSM_MAX_WAIT_OCUNT];
	int result;
};

/* GBL event caller use */
int phl_fsm_gbl_msg_announce(struct fsm_obj *obj, u16 gbl_evt, u16 cb_evt);
int phl_fsm_gbl_not_reply_num(struct fsm_obj *obj, struct gbl_param *param);
enum rtw_phl_status phl_fsm_flush_gbl(struct fsm_obj *obj);

/* GBL event callee use */
int phl_fsm_gbl_msg_hold(struct fsm_obj *obj,
	struct gbl_param *param, u32 ms);
enum rtw_phl_status phl_fsm_gbl_msg_release(struct fsm_obj *obj,
	u16 event, u32 seq, enum gbl_evt_result result);

/* fsm init funciton */
struct fsm_root *phl_fsm_init_root(void *phl_info);
void phl_fsm_deinit_root(struct fsm_root *root);
enum rtw_phl_status phl_fsm_start_root(struct fsm_root *root);
enum rtw_phl_status phl_fsm_stop_root(struct fsm_root *root);


struct fsm_main *phl_fsm_init_fsm(struct fsm_root *root,
	const char *name, void *phl_info, struct rtw_phl_fsm_tb *tb);
enum rtw_phl_status phl_fsm_deinit_fsm(struct fsm_main *fsm);

enum rtw_phl_status phl_fsm_start_fsm(struct fsm_main *fsm);
enum rtw_phl_status phl_fsm_stop_fsm(struct fsm_main *fsm);
void *phl_fsm_new_obj(struct fsm_main *fsm, void **fsm_obj, int obj_sz);
void phl_fsm_destory_obj(struct fsm_obj *obj);
void phl_fsm_dbg(struct phl_info_t *phl_info, char input[][MAX_ARGV],
		      u32 input_num, char *output, u32 out_len);

/* fsm operating funciton */
struct fsm_msg *phl_fsm_new_msg(struct fsm_obj *obj, u16 event);
enum rtw_phl_status phl_fsm_sent_msg(struct fsm_obj *obj, struct fsm_msg *msg);
enum rtw_phl_status phl_fsm_cancel_obj(struct fsm_obj *obj);
void phl_fsm_state_goto(struct fsm_obj *obj, u8 new_state);
void phl_fsm_set_alarm(struct fsm_obj *obj, int ms, u16 event);
void phl_fsm_set_alarm_ext(struct fsm_obj *obj,
	int ms, u16 event, u8 id, void *priv);
void phl_fsm_cancel_alarm(struct fsm_obj *obj);
void phl_fsm_cancel_alarm_ext(struct fsm_obj *obj, u8 id);
void phl_fsm_pause_alarm(struct fsm_obj *obj);
void phl_fsm_pause_alarm_ext(struct fsm_obj *obj, u8 id);
void phl_fsm_resume_alarm(struct fsm_obj *obj);
void phl_fsm_resume_alarm_ext(struct fsm_obj *obj, u8 id);
bool phl_fsm_is_alarm_off(struct fsm_obj *obj);
bool phl_fsm_is_alarm_off_ext(struct fsm_obj *obj, u8 id);
void phl_fsm_extend_alarm_ext(struct fsm_obj *obj, int ms, u8 id);
u8 phl_fsm_dbg_level(struct fsm_main *fsm, u8 level);
u8 phl_fsm_evt_level(struct fsm_main *fsm, u8 level);
enum rtw_phl_status phl_fsm_gen_msg(void *phl, struct fsm_obj *obj,
	void *pbuf, u32 sz, u16 event);

/* function to manipulate extra queue */
int phl_fsm_enqueue_ext(struct fsm_main *fsm, struct fsm_msg *msg, u8 to_head);
struct fsm_msg *phl_fsm_dequeue_ext(struct fsm_main *fsm);
int phl_fsm_is_ext_queue_empty(struct fsm_main *fsm);

/* util function */
u8 phl_fsm_state_id(struct fsm_obj *obj);
char *phl_fsm_obj_name(struct fsm_obj *obj);
char *phl_fsm_evt_name(struct fsm_obj *obj, u16 event);
u32 phl_fsm_time_pass(u32 start);
u32 phl_fsm_time_left(u32 start, u32 end);


#ifndef CONFIG_PHL_WPP
#define FSM_PRINT(fsm, fmt, ...) \
	do {\
		if (!fsm || phl_fsm_dbg_level(fsm, FSM_DBG_PRINT)) \
			PHL_TRACE(COMP_PHL_FSM, _PHL_ALWAYS_, fmt, ##__VA_ARGS__); \
	} while (0)

#define FSM_ERR(fsm, fmt, ...) \
	do {\
		if (!fsm || phl_fsm_dbg_level(fsm, FSM_DBG_ERR)) \
			PHL_TRACE(COMP_PHL_FSM, _PHL_ERR_, fmt, ##__VA_ARGS__); \
	} while (0)

#define FSM_WARN(fsm, fmt, ...) \
	do {\
		if (!fsm || phl_fsm_dbg_level(fsm, FSM_DBG_WARN)) \
			PHL_TRACE(COMP_PHL_FSM, _PHL_WARNING_, fmt, ##__VA_ARGS__); \
	} while (0)

#define FSM_INFO(fsm, fmt, ...) \
	do {\
		if (!fsm || phl_fsm_dbg_level(fsm, FSM_DBG_INFO)) \
			PHL_TRACE(COMP_PHL_FSM, _PHL_INFO_, fmt, ##__VA_ARGS__); \
	} while (0)

#define FSM_DBG(fsm, fmt, ...) \
	do {\
		if (!fsm || phl_fsm_dbg_level(fsm, FSM_DBG_DBG)) \
			PHL_TRACE(COMP_PHL_FSM, _PHL_DEBUG_, fmt, ##__VA_ARGS__); \
	} while (0)

#define FSM_MSG(fsm, level_, fmt, ...) \
	do {\
		if (!fsm || phl_fsm_dbg_level(fsm, level_)) \
			PHL_TRACE(COMP_PHL_FSM, _PHL_INFO_, fmt, ##__VA_ARGS__); \
	} while (0)

#define FSM_EV_MSG(fsm, level_, fmt, ...) \
	do {\
		if (!fsm || phl_fsm_evt_level(fsm, level_)) \
			PHL_TRACE(COMP_PHL_FSM, _PHL_INFO_, fmt, ##__VA_ARGS__); \
	} while (0)
#else
#undef FSM_PRINT
#define FSM_PRINT(fsm, fmt, ...)
#undef FSM_ERR
#define FSM_ERR(fsm, fmt, ...)
#undef FSM_WARN
#define FSM_WARN(fsm, fmt, ...)
#undef FSM_INFO
#define FSM_INFO(fsm, fmt, ...)
#undef FSM_DBG
#define FSM_DBG(fsm, fmt, ...)
#undef FSM_MSG
#define FSM_MSG(fsm, level, fmt, ...)
#undef FSM_EV_MSG
#define FSM_EV_MSG(fsm, level, fmt, ...)
#endif  /* CONFIG_PHL_WPP */

#endif /* __PHL_FSM_H__ */

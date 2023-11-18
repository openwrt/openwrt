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
#ifndef __PHL_CMD_JOB_H__
#define __PHL_CMD_JOB_H__

#define RTW_PHL_JOB_NAME_LEN 32
#ifndef MIN
#define MIN(X, Y) (((X) < (Y)) ? (X) : (Y))
#endif
struct fsm_msg;
struct _cmd_obj;

/* command job */
enum JOB_ID {
	JOB_RUN_FUNC,
	JOB_ADD_STA_ENTRY,
	JOB_ADD_MACID,
	JOB_MAX,
};

struct job_hdl_ent {
	u16 job_id;
	char name[RTW_PHL_JOB_NAME_LEN];
	int (*job_func)(struct _cmd_obj *pcmd, void *param);
};

enum JOB_RESULT {
	JOB_SUCCESS,
	/* caller thread will be schedule out until job is completed */
	JOB_TIMEOUT,
	JOB_FAILURE
};

struct wait_completion {

	u8 sync;
	u32 submit_time;

	/* <0: not synchronous,
	 *  0: wait forever,
	 * >0: max waitting time (ms)
	 */
	int max_wait_time;

	_os_event done;

	/* JOB_WAIT_COMPLETION use */
	int rtn; /* handle return value */
	enum JOB_RESULT result; /* operation result */
};

struct phl_cmd_job {
	_os_list list;
	u16 id;
	struct wait_completion wait;
	u8 pwr_level;
	union {
		struct { /* JOB_RUN_FUNC */
			int (*func)(void *priv, void *param); /* TODO remove */
			int (*fptr)(void *priv, void *param, bool discard);
			void *priv;
			void *parm;
			int parm_sz;
			char name[RTW_PHL_JOB_NAME_LEN];
		} cmd;

		struct { /* JOB_ADD_STA_ENTRY */
			struct rtw_phl_stainfo_t *sta;
		} sta;
	} u;
};

enum PWR_LEVEL {
	PWR_DONT_CARE, /* able to run in any kind of power mode */
	PWR_NO_IO, /* without register read write */
	PWR_BASIC_IO /* leave 32K and PG */
};

enum JOB_SYNC {
	JOB_ASYNC,
	/* caller thread will be schedule out until job is completed */
	JOB_WAIT_COMPLETION
};

char *job_name(struct _cmd_obj *pcmd, u8 id);
void cmd_set_job_tbl(struct _cmd_obj *pcmd);
void cmd_discard_msg_job(struct _cmd_obj *pcmd, struct fsm_msg *msg);
int phl_cmd_do_job(struct _cmd_obj *pcmd, void *param);

/* command thread jobs */
/* TODO remove below when no reference */
enum rtw_phl_status rtw_phl_job_add_fptr(void *phl, void *func,
	void *priv, void *parm, char *name, enum PWR_LEVEL pwr);

enum rtw_phl_status rtw_phl_job_fill_fptr(void *phl, struct phl_cmd_job *job,
	void *func, void *priv, void *parm, char *name, enum PWR_LEVEL pwr);

enum rtw_phl_status phl_cmd_enqueue_and_wait_job(struct _cmd_obj *pcmd,
	struct fsm_msg *msg);

enum rtw_phl_status phl_cmd_complete_job(void *phl, struct phl_cmd_job *job);

enum rtw_phl_status rtw_phl_job_reg_wdog(void *phl,
	int (*func)(void *priv, void *param, bool discard),
	void *priv, void *parm, int parm_sz, char *name, enum PWR_LEVEL pwr);
enum rtw_phl_status rtw_phl_cmd_pause_wdog(void *phl, char *reason);
enum rtw_phl_status rtw_phl_cmd_resume_wdog(void *phl, char *reason);

enum rtw_phl_status rtw_hal_add_sta_entry_job(void *phl,
	struct rtw_phl_stainfo_t *sta, enum JOB_SYNC sync);

#endif /* __PHL_CMD_JOB_H__ */

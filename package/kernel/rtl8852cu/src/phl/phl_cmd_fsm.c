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

/* #define DEBUG_CMD_FSM_MODULE */

#define MAX(X, Y) (((X) > (Y)) ? (X) : (Y))
#ifdef FSM_DBG_MEM_OVERWRITE
#define _os_kmem_alloc(a, b) fsm_kmalloc(b)
#define _os_kmem_free(a, b, c) fsm_kfree(b, c)
#endif

#define pstr(s) (s +_os_strlen((u8 *)s))
#define lstr(s, l) (size_t)(l - _os_strlen((u8 *)s))

#define MAX_POWER_ON_TIME	100
#define WDOG_ALARM_ID		1
#define WD_DURATION		2000 /* 2sec */

enum CMD_STATE_ST {
	CMD_ST_IDLE,
	CMD_ST_REQ_PWR,
	CMD_ST_SERVICE
};

enum CMD_EV_ID {
	CMD_EV_REQ_PWR,
	CMD_EV_DO_JOB,
	CMD_EV_JOB_NOTIFY,
	CMD_EV_PWR_ON_OK,
	CMD_EV_PWR_ON_TIMEOUT,
	CMD_EV_PWR_EXPIRE,
	CMD_EV_WD_EXPIRE,
	CMD_EV_WD_DO_JOB,
	CMD_EV_MAX
};

static int cmd_idle_st_hdl(void *obj, u16 event, void *param);
static int cmd_req_pwr_st_hdl(void *obj, u16 event, void *param);
static int cmd_service_st_hdl(void *obj, u16 event, void *param);

/* STATE table */
static struct fsm_state_ent cmd_state_tbl[] = {
	ST_ENT(CMD_ST_IDLE, cmd_idle_st_hdl),
	ST_ENT(CMD_ST_REQ_PWR, cmd_req_pwr_st_hdl),
	ST_ENT(CMD_ST_SERVICE, cmd_service_st_hdl),
};

/* EVENT table */
static struct fsm_event_ent cmd_event_tbl[] = {
	EV_ENT(CMD_EV_REQ_PWR),
	EV_ENT(CMD_EV_DO_JOB),
	EV_ENT(CMD_EV_JOB_NOTIFY),
	EV_ENT(CMD_EV_PWR_ON_OK),
	EV_ENT(CMD_EV_PWR_ON_TIMEOUT),
	EV_ENT(CMD_EV_PWR_EXPIRE),
	EV_DBG(CMD_EV_WD_EXPIRE),
	EV_ENT(CMD_EV_WD_DO_JOB),
	EV_ENT(CMD_EV_MAX) /* EV_MAX for fsm safety checking */
};

/*
 * command thread state sub function
 */

void cmd_pm_cb(void *phl, void *hdl, void *ctx, enum rtw_phl_status stat)
{
	struct _cmd_obj *pcmd = (struct _cmd_obj *)ctx;

	if (stat == RTW_PHL_STATUS_SUCCESS)
		phl_fsm_gen_msg(phl, pcmd->fsm_obj, NULL, 0, CMD_EV_PWR_ON_OK);
	else
		phl_fsm_gen_msg(phl, pcmd->fsm_obj, NULL, 0, FSM_EV_CANCEL);
}

int cmd_dequeue_job(struct _cmd_obj *pcmd)
{
	void *d = phl_to_drvpriv(pcmd->phl_info);
	struct fsm_msg *msg;

	msg = phl_fsm_dequeue_ext(pcmd->fsm);
	if (msg == NULL)
		return -1;

	if (phl_fsm_sent_msg(pcmd->fsm_obj, msg) != RTW_PHL_STATUS_SUCCESS) {
		if (msg->param && msg->param_sz)
			_os_kmem_free(d, (u8 *)msg->param, msg->param_sz);
		_os_kmem_free(d, (u8 *)msg, sizeof(*msg));
		return -1;
	}
	return 0;
}

static void cmd_do_wdog_job(struct _cmd_obj *pcmd)
{
#if defined(PHL_PLATFORM_LINUX)
	void *d = phl_to_drvpriv(pcmd->phl_info);
	struct phl_cmd_job *job;

	/* Tempoary test in Linux,
	 * make sure Windows doesn't run to here
	 */

	_os_mutex_lock(d, &pcmd->wd_q_lock);
	phl_list_for_loop(job, struct phl_cmd_job, &pcmd->wd_q, list) {
		job->u.cmd.fptr(job->u.cmd.priv, job->u.cmd.parm, false);

#ifdef DEBUG_CMD_FSM_MODULE
               FSM_INFO(pcmd->fsm, "%s: wdog %s:%s\n",
                       phl_fsm_obj_name(pcmd->fsm_obj),
                       job_name(pcmd, (u8)job->id),
                       (char *)job->u.cmd.name);
#endif
	}
	_os_mutex_unlock(d, &pcmd->wd_q_lock);
	rtw_phl_watchdog_callback(pcmd->phl_info);
#endif
}

static void cmd_add_wdog_event(struct _cmd_obj *pcmd)
{
	struct fsm_msg *msg;

	msg = phl_fsm_new_msg(pcmd->fsm_obj, CMD_EV_WD_DO_JOB);
	if (msg == NULL)
		return;
	/* Always enqueue msg to extra queue */
	phl_fsm_enqueue_ext(pcmd->fsm, msg, 0);
}

/*
 * CMD state handler
 */

/*
 * cmd idle handler
 * This state has no power; Able to run no_io job
 * For jobs don't need to request power (no_io job)
 */
static int cmd_idle_st_hdl(void *obj, u16 event, void *param)
{
	struct _cmd_obj *pcmd = (struct _cmd_obj *)obj;
	int rtn = FSM_FREE_PARAM;

	/* has no power */
	switch (event) {
	case FSM_EV_SWITCH_IN:
		phl_fsm_set_alarm_ext(pcmd->fsm_obj,
			WD_DURATION, CMD_EV_WD_EXPIRE, WDOG_ALARM_ID, NULL);
		break;

	case FSM_EV_STATE_IN:
		break;

	case CMD_EV_WD_EXPIRE:

		/* restart watchdog alarm */
		phl_fsm_set_alarm_ext(pcmd->fsm_obj,
			WD_DURATION, CMD_EV_WD_EXPIRE, WDOG_ALARM_ID, NULL);

		if (pcmd->wdog_pwr_level < PWR_BASIC_IO) {
			cmd_do_wdog_job(pcmd);
			break;
		}
		/* We need to request power */
		cmd_add_wdog_event(pcmd);

		/* fall through */

	case CMD_EV_REQ_PWR:
		phl_fsm_state_goto(pcmd->fsm_obj, CMD_ST_REQ_PWR);
		break;

	case CMD_EV_JOB_NOTIFY:
		cmd_dequeue_job(pcmd);
		break;

	case CMD_EV_DO_JOB:
		/* TODO check MUST BE no_io cmd */
		rtn = phl_cmd_do_job(pcmd, param);
		break;

	case FSM_EV_STATE_OUT:
		break;

	case FSM_EV_SWITCH_OUT:
		phl_fsm_cancel_alarm_ext(pcmd->fsm_obj, WDOG_ALARM_ID);
		break;

	default:
		break;
	}
	return rtn;
}

/* This stete is designed to request power */
static int cmd_req_pwr_st_hdl(void *obj, u16 event, void *param)
{
	struct _cmd_obj *pcmd = (struct _cmd_obj *)obj;
	struct fsm_msg *msg;
	int rtn = FSM_FREE_PARAM;
	enum rtw_phl_status phl_st;

	switch (event) {
	case FSM_EV_STATE_IN:

		/* TODO: request power */
		phl_st = RTW_PHL_STATUS_SUCCESS;

		if (phl_st == RTW_PHL_STATUS_PENDING) {
			/* we have to wait CMD_EV_PWR_ON_OK */
			phl_fsm_set_alarm(pcmd->fsm_obj,
				MAX_POWER_ON_TIME, CMD_EV_PWR_ON_TIMEOUT);
			break;
		}

		if (phl_st != RTW_PHL_STATUS_SUCCESS) {
			FSM_ERR(pcmd->fsm, "%s: power on fail(%d)\n",
				phl_fsm_obj_name(pcmd->fsm_obj), phl_st);
			phl_fsm_state_goto(pcmd->fsm_obj, CMD_ST_IDLE);

			/* drop fail cmd */
			msg = phl_fsm_dequeue_ext(pcmd->fsm);
			if (msg != NULL)
				cmd_discard_msg_job(pcmd, msg);
			break;
		}

		/* RTW_PHL_STATUS_SUCCESS */
		/* fall through */

	case CMD_EV_PWR_ON_OK:
		pcmd->has_power = true;
		phl_fsm_state_goto(pcmd->fsm_obj, CMD_ST_SERVICE);
		break;

	case CMD_EV_PWR_ON_TIMEOUT:
	case FSM_EV_CANCEL:
		phl_fsm_state_goto(pcmd->fsm_obj, CMD_ST_IDLE);
		break;

	case CMD_EV_WD_EXPIRE:
		/* restart watchdog alarm */
		phl_fsm_set_alarm_ext(pcmd->fsm_obj,
			WD_DURATION, CMD_EV_WD_EXPIRE, WDOG_ALARM_ID, NULL);

		/* enqueue watchdog job */
		cmd_add_wdog_event(pcmd);
		break;

	case FSM_EV_STATE_OUT:
		phl_fsm_cancel_alarm(pcmd->fsm_obj);
		break;

	default:
		break;
	}
	return rtn;
}

/* This state has basic power supply.
 * Able to do both PWR_BASIC_IO and PWR_NO_IO jobs.
 */

static int cmd_service_st_hdl(void *obj, u16 event, void *param)
{
	struct _cmd_obj *pcmd = (struct _cmd_obj *)obj;
	struct phl_cmd_job *job;
	int rtn = FSM_FREE_PARAM;

	switch (event) {
	case FSM_EV_STATE_IN:
	case CMD_EV_JOB_NOTIFY:
		cmd_dequeue_job(pcmd);
		break;

	case CMD_EV_DO_JOB:

		rtn = phl_cmd_do_job(pcmd, param);

		/* hold power for a while */
		job = (struct phl_cmd_job *)param;
		if (job->pwr_level >= PWR_BASIC_IO)
			phl_fsm_set_alarm(pcmd->fsm_obj,
				MAX_POWER_ON_TIME, CMD_EV_PWR_EXPIRE);

		/* dequeue jobs */
		cmd_dequeue_job(pcmd);
		break;

	case CMD_EV_WD_EXPIRE:
	case CMD_EV_WD_DO_JOB:

		cmd_do_wdog_job(pcmd);

		/* hold power for a while */
		phl_fsm_set_alarm(pcmd->fsm_obj,
			MAX_POWER_ON_TIME, CMD_EV_PWR_EXPIRE);

		/* restart watchdog alarm */
		phl_fsm_set_alarm_ext(pcmd->fsm_obj,
			WD_DURATION, CMD_EV_WD_EXPIRE, WDOG_ALARM_ID, NULL);
		break;

	case CMD_EV_PWR_EXPIRE:

		/* make sure no more commands */
		if (cmd_dequeue_job(pcmd) >= 0)
			break;

		/* no more commands */
		/* fall through */

	case FSM_EV_CANCEL:
		phl_fsm_state_goto(pcmd->fsm_obj, CMD_ST_IDLE);
		break;

	case FSM_EV_STATE_OUT:
		phl_fsm_cancel_alarm(pcmd->fsm_obj);
		pcmd->has_power = false;
		break;

	default:
		break;
	}
	return rtn;
}

static void cmd_dump_obj(void *obj, char *p, int *sz)
{
	/* nothing to do for now */
}

static void cmd_dump_fsm(void *fsm, char *p, int *sz)
{
	/* nothing to do for now */
}

static void cmd_dbg_help(struct _cmd_obj *pcmd, char *p, int *sz)
{
	int len = *sz;

	_os_snprintf(pstr(p), lstr(p, len),
		"usage:\n\t<%s> <wdog>,<pause|resume>\n",
		phl_fsm_obj_name(pcmd->fsm_obj));
	*sz = len;
}

static void cmd_debug(void *obj, char input[][MAX_ARGV], u32 input_num,
	char *output, u32 *out_len)
{
	struct _cmd_obj *pcmd = (struct _cmd_obj *)obj;
	char *ptr = output;
	int len = *out_len;

	if (input_num <  2) {
		cmd_dbg_help(pcmd, ptr, &len);
		goto done;
	}

	if (!_os_strcmp(input[0], "wdog")) {
		if (!_os_strcmp(input[1], "pause")) {
			/* wdog,pause */
			rtw_phl_cmd_pause_wdog(pcmd->phl_info, "debug cmd");

			_os_snprintf(pstr(ptr), lstr(ptr, len),
				"\n%s: pause watchdog\n",
				phl_fsm_obj_name(pcmd->fsm_obj));

		} else if (!_os_strcmp(input[1], "resume")) {
			/* wdog,resume */
			rtw_phl_cmd_resume_wdog(pcmd->phl_info, "debug cmd");
			_os_snprintf(pstr(ptr), lstr(ptr, len),
				"\n%s: resume watchdog\n",
				phl_fsm_obj_name(pcmd->fsm_obj));
		}
	} else
		cmd_dbg_help(pcmd, ptr, &len);
done:
	*out_len = len;
}

/* return value
 *	0: timeout
 *     >0: success
 */
static int wait_completion(void *d, struct phl_cmd_job *job, int m_sec)
{
	job->wait.max_wait_time = m_sec;
	job->wait.submit_time = _os_get_cur_time_ms();

	_os_event_init(d, &(job->wait.done));

	/* wait here */
	return _os_event_wait(d, &(job->wait.done), m_sec);
}

/* For EXTERNAL application to create a cmd FSM */
/* @root: FSM root structure
 * @phl_info: private data structure to invoke hal/phl function
 *
 * return
 * fsm_main: FSM main structure (Do NOT expose)
 */
struct fsm_main *phl_cmd_new_fsm(struct fsm_root *root,
	struct phl_info_t *phl_info)
{
	void *d = phl_to_drvpriv(phl_info);
	struct fsm_main *fsm = NULL;
	struct rtw_phl_fsm_tb tb;


	_os_mem_set(d, &tb, 0, sizeof(tb));
	tb.max_state = sizeof(cmd_state_tbl)/sizeof(cmd_state_tbl[0]);
	tb.max_event = sizeof(cmd_event_tbl)/sizeof(cmd_event_tbl[0]);
	tb.state_tbl = cmd_state_tbl;
	tb.evt_tbl = cmd_event_tbl;
	tb.dump_obj = cmd_dump_obj;
	tb.dump_fsm = cmd_dump_fsm;
	tb.dbg_level = FSM_DBG_WARN;
	tb.evt_level = FSM_DBG_WARN;
	tb.debug = cmd_debug;

	fsm = phl_fsm_init_fsm(root, "cmd", phl_info, &tb);

	return fsm;
}

/* For EXTERNAL application to destory cmd fsm */
/* @fsm: see fsm_main
 */
void phl_cmd_destory_fsm(struct fsm_main *fsm)
{
	if (fsm == NULL)
		return;

	/* deinit fsm local variable if has */

	/* call FSM Framewro to deinit fsm */
	phl_fsm_deinit_fsm(fsm);
}

/* For EXTERNAL application to create command object */
/* @fsm: FSM main structure which created by phl_cmd_new_fsm()
 * @phl_info: private data structure to invoke hal/phl function
 *
 * return
 * _cmd_obj: structure of command object (Do NOT expose)
 */
struct _cmd_obj *phl_cmd_new_obj(struct fsm_main *fsm,
	struct phl_info_t *phl_info)
{
	void *d = phl_to_drvpriv(phl_info);
	struct fsm_obj *obj;
	struct _cmd_obj *pcmd;


	pcmd = phl_fsm_new_obj(fsm, (void **)&obj, sizeof(*pcmd));

	if (pcmd == NULL) {
		FSM_ERR(fsm, "cmd: malloc obj fail\n");
		return NULL;
	}
	pcmd->fsm = fsm;
	pcmd->fsm_obj = obj;
	pcmd->phl_info = phl_info;
	pcmd->has_power = false;

	cmd_set_job_tbl(pcmd);
	INIT_LIST_HEAD(&pcmd->wd_q);
	_os_mutex_init(d, &pcmd->wd_q_lock);

	return pcmd;
}

/* For EXTERNAL application to destory command object */
/* @pcmd: local created command object
 *
 */
void phl_cmd_destory_obj(struct _cmd_obj *pcmd)
{
	void *d;
	struct phl_cmd_job *job, *job_t;

	if (pcmd == NULL)
		return;

	d = phl_to_drvpriv(pcmd->phl_info);

	/* deinit and free all local variables */

	/* watchdog job */
	/* TODO spin lock is not necessary */
	phl_list_for_loop_safe(job, job_t,
		struct phl_cmd_job, &pcmd->wd_q, list) {

		_os_mutex_lock(d, &pcmd->wd_q_lock);
		list_del(&job->list);
		_os_mutex_unlock(d, &pcmd->wd_q_lock);

		if (job->id == JOB_RUN_FUNC) {

			if (job->u.cmd.parm && job->u.cmd.parm_sz != 0)
				_os_kmem_free(d, (u8 *)job->u.cmd.parm,
					job->u.cmd.parm_sz);
			_os_kmem_free(d, (u8 *)job, sizeof(*job));

		} else {
			FSM_ERR(pcmd->fsm, "%s: free wdog %s fail\n",
				phl_fsm_obj_name(pcmd->fsm_obj),
				job_name(pcmd, (u8)job->id));
		}
	}
	_os_mutex_deinit(d, &pcmd->wd_q_lock);

	/* inform FSM framewory to recycle fsm_obj */
	phl_fsm_destory_obj(pcmd->fsm_obj);
}

/* For EXTERNAL application to pause all watchdog jobs (expose) */
/* @phl: phl private structure
 * @reason: reason for pause watchdog (option)
 */
enum rtw_phl_status rtw_phl_cmd_pause_wdog(void *phl, char *reason)
{
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;
	struct _cmd_obj *pcmd = phl_info->cmd_obj;

	phl_fsm_pause_alarm_ext(pcmd->fsm_obj, WDOG_ALARM_ID);
	pcmd->wdog_pause_num++;

	FSM_INFO(pcmd->fsm, "%s: pause wdog (%s) %d\n",
		phl_fsm_obj_name(pcmd->fsm_obj),
		(reason == NULL) ? "" : reason, pcmd->wdog_pause_num);

	return RTW_PHL_STATUS_SUCCESS;
}

/* For EXTERNAL application to resume all watchdog jobs (expose) */
/* @phl: phl private structure
 * @reason: reason for resume watchdog (option)
 */
enum rtw_phl_status rtw_phl_cmd_resume_wdog(void *phl, char *reason)
{
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;
	struct _cmd_obj *pcmd = phl_info->cmd_obj;

	FSM_INFO(pcmd->fsm, "%s: resume wdog (%s) %d\n",
		phl_fsm_obj_name(pcmd->fsm_obj),
		(reason == NULL) ? "" : reason, pcmd->wdog_pause_num);

	if (pcmd->wdog_pause_num == 0)
		return RTW_PHL_STATUS_FAILURE;

	if (--(pcmd->wdog_pause_num) != 0)
		return RTW_PHL_STATUS_SUCCESS;

	/* reset timer to original period */
	phl_fsm_set_alarm_ext(pcmd->fsm_obj,
		WD_DURATION, CMD_EV_WD_EXPIRE, WDOG_ALARM_ID, NULL);

	phl_fsm_resume_alarm_ext(pcmd->fsm_obj, WDOG_ALARM_ID);

	return RTW_PHL_STATUS_SUCCESS;
}

/* For EXTERNAL application to register watchdog job (expose) */
/* @phl: phl private structure
 * @func: function pointer to be runed
 * @priv: 1st param of function pointer
 * @parm: 2nd param of function pointer
 * @name: function name for debug message (optional)
 * @pwr: refers to enum PWR_LEVEL
 */
enum rtw_phl_status rtw_phl_job_reg_wdog(void *phl,
	int (*fptr)(void *priv, void *param, bool discard),
	void *priv, void *parm, int parm_sz, char *name, enum PWR_LEVEL pwr)
{
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;
	struct _cmd_obj *pcmd = phl_info->cmd_obj;
	struct phl_cmd_job *job;
	void *d = phl_to_drvpriv(phl_info);
	char wd[] = "wdog";

	if (phl_info->cmd_obj == NULL) {
		PHL_ERR("cmd: %s fsm module doesn't init \n", __func__);
		return -1;
	}

	/* allocate memory for job parameter */
	job = _os_kmem_alloc(d, sizeof(*job));
	if (job == NULL) {
		FSM_ERR(pcmd->fsm, "%s: alloc wdog job fail\n",
			phl_fsm_obj_name(pcmd->fsm_obj));
		return RTW_PHL_STATUS_RESOURCE;
	}

	_os_mem_set(d, job, 0, sizeof(job));
	job->id = JOB_RUN_FUNC;
	job->pwr_level = pwr;
	job->u.cmd.fptr = fptr;
	job->u.cmd.priv = priv;
	job->u.cmd.parm = parm;
	job->u.cmd.parm_sz = parm_sz;
	_os_mem_set(d, job->u.cmd.name, 0, RTW_PHL_JOB_NAME_LEN);
	if (name != NULL)
		_os_mem_cpy(d, job->u.cmd.name, name,
			MIN((RTW_PHL_JOB_NAME_LEN - 1),
			_os_strlen((u8 *)name)));
	else
		_os_mem_cpy(d, job->u.cmd.name, wd,
			MIN((RTW_PHL_JOB_NAME_LEN - 1),
			_os_strlen((u8 *)wd)));

	pcmd->wdog_pwr_level = (u8)MAX(pwr, pcmd->wdog_pwr_level);

	_os_mutex_lock(d, &pcmd->wd_q_lock);
	list_add_tail(&job->list, &pcmd->wd_q);
	_os_mutex_unlock(d, &pcmd->wd_q_lock);

	FSM_INFO(pcmd->fsm, "%s: wdog hooks %s, pwr_level = %d\n",
		phl_fsm_obj_name(pcmd->fsm_obj),
		job->u.cmd.name, pwr);

	return RTW_PHL_STATUS_SUCCESS;
}

/* For EXTERNAL application to invoke command service (expose) */
/* @pcmd: cmd object
 * @msg: refert to struct fsm_msg
 */
enum rtw_phl_status phl_cmd_enqueue_and_wait_job(struct _cmd_obj *pcmd,
	struct fsm_msg *msg)
{
	struct phl_cmd_job *job = (struct phl_cmd_job *)msg->param;
	void *d = phl_to_drvpriv(pcmd->phl_info);
	int max_wait_time = 1000;
	int remain = 0;

#if 0
	FSM_INFO(pcmd->fsm, "%s: enqueue %s:%s\n",
		phl_fsm_obj_name(pcmd->fsm_obj),
		job_name(pcmd, (u8)job->id), (char *)job->u.cmd.name);
#endif
	/* Always enqueue msg to extra queue */
	phl_fsm_enqueue_ext(pcmd->fsm, msg, 0);

	if ((pcmd->has_power == false) && (job->pwr_level >= PWR_BASIC_IO))
		/* request power */
		phl_fsm_gen_msg(pcmd->phl_info,
			pcmd->fsm_obj, NULL, 0, CMD_EV_REQ_PWR);
	else
		phl_fsm_gen_msg(pcmd->phl_info,
			pcmd->fsm_obj, NULL, 0, CMD_EV_JOB_NOTIFY);

	if (job->wait.sync == JOB_WAIT_COMPLETION) {

		job->wait.result = JOB_SUCCESS;
		/* WAIT completion; context switch */
		remain = wait_completion(d, job, max_wait_time);
		if (remain == 0) {
			job->wait.result = JOB_TIMEOUT;
			FSM_WARN(pcmd->fsm, "job: %s timeout %d ms\n",
				job_name(pcmd, (u8)job->id), max_wait_time);
			return RTW_PHL_STATUS_FAILURE;
		}
	}
	return RTW_PHL_STATUS_SUCCESS;
}

/* For EXTERNAL application to invoke command service (expose) */
/* @phl: refer to struct phl_info_t
 * @pjob: job to be completed
 */
enum rtw_phl_status phl_cmd_complete_job(void *phl, struct phl_cmd_job *pjob)
{
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;
	struct _cmd_obj *pcmd = phl_info->cmd_obj;
	struct phl_cmd_job *job;
	struct fsm_msg *msg;
	void *d = phl_to_drvpriv(pcmd->phl_info);

	if (pjob == NULL)
		return RTW_PHL_STATUS_FAILURE;

#ifdef HAS_NO_COMPLETION
	if (pjob->wait.sync == JOB_WAIT_COMPLETION) {
		pjob->wait.sync = JOB_ASYNC;
		FSM_ERR(pcmd->fsm,
			"cmd: %s doesn't support SYNC mdoe use ASYNC mode\n",
			job_name(pcmd, (u8)pjob->id));
	}
#endif
	/* NEW message to start cmd service */
	msg = phl_fsm_new_msg(pcmd->fsm_obj, CMD_EV_DO_JOB);
	if (msg == NULL) {
		FSM_ERR(pcmd->fsm, "cmd: alloc msg fail\n");
		return RTW_PHL_STATUS_RESOURCE;
	}

	/* allocate memory for command parameter */
	if (pjob->wait.sync == JOB_WAIT_COMPLETION) {
		/* TODO check interrupt context */
		/* SYNC mode, use call loacl job varable */
		job = pjob;
	} else {
		/* ASYNC mode */
		/* allocate memory for command parameter */
		job = _os_kmem_alloc(d, sizeof(*job));
		if (job == NULL) {
			FSM_ERR(pcmd->fsm, "cmd: alloc job fail\n");
			_os_kmem_free(d, (u8 *)msg, sizeof(*msg));
			return RTW_PHL_STATUS_RESOURCE;
		}
		_os_mem_cpy(d, job, pjob, sizeof(*job));
	}
	msg->param = (void *)job;
	msg->param_sz = sizeof(*job);

	return phl_cmd_enqueue_and_wait_job(pcmd, msg);
}

/* For EXTERNAL application to start command service (expose) */
/* @pcmd: cmd object
 */
enum rtw_phl_status phl_cmd_start(struct _cmd_obj *pcmd)
{
	/* Start FSM */
	return phl_fsm_start_fsm(pcmd->fsm);
}

/* For EXTERNAL application to stop cmd obj
 * @phl_info: private data structure to invoke hal/phl function
 *
 */
void phl_fsm_cmd_stop(struct phl_info_t *phl_info)
{
	struct _cmd_obj *pcmd = phl_info->cmd_obj;
	//void *d = phl_to_drvpriv(pcmd->phl_info);
	struct phl_cmd_job *job;
	struct fsm_msg *msg;

	while ((msg = phl_fsm_dequeue_ext(pcmd->fsm)) != NULL) {
		job = (struct phl_cmd_job *)msg->param;
		cmd_discard_msg_job(pcmd, msg);
	}

	phl_fsm_stop_fsm(phl_info->cmd_fsm);
}

/* For EXTERNAL application to stop cmd service (expose) */
/* @pcmd: cmd job will be cancelled
 */
enum rtw_phl_status phl_cmd_cancel(struct _cmd_obj *pcmd)
{
#ifdef PHL_INCLUDE_FSM
	return phl_fsm_cancel_obj(pcmd->fsm_obj);
#else
	return RTW_PHL_STATUS_FAILURE;
#endif /* PHL_INCLUDE_FSM */
}
#endif /*CONFIG_FSM*/


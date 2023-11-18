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

#define MIN(X, Y) (((X) < (Y)) ? (X) : (Y))

/**
 * Deal with original command thread comd
 */
char unknow_func[8] = "no_name";

void *cmd2hal(struct _cmd_obj *pcmd)
{
	return pcmd->phl_info->hal;
}

/* usually called when stopping fsm */
void cmd_discard_msg_job(struct _cmd_obj *pcmd, struct fsm_msg *msg)
{
	void *d = phl_to_drvpriv(pcmd->phl_info);
	struct phl_cmd_job *job = (struct phl_cmd_job *)msg->param;

	/* inform core to free param */
	if (job->id == JOB_RUN_FUNC &&
		(job->u.cmd.fptr || job->u.cmd.func)) {
		FSM_INFO(pcmd->fsm, "%s: discard %s:%s\n",
			phl_fsm_obj_name(pcmd->fsm_obj),
			job_name(pcmd, (u8)job->id),
			(char *)job->u.cmd.name);

		/* we have to callback to core to free job
		 * Also release wait_complition if has
		 */
		job->u.cmd.fptr(job->u.cmd.priv, job->u.cmd.parm, true);
	} else
		FSM_INFO(pcmd->fsm, "%s: discard %s\n",
			phl_fsm_obj_name(pcmd->fsm_obj),
			job_name(pcmd, (u8)job->id));

	_os_kmem_free(d, (u8 *)msg->param, msg->param_sz);
	_os_kmem_free(d, (u8 *)msg, sizeof(*msg));
}

enum rtw_phl_status rtw_phl_job_fill_fptr(void *phl, struct phl_cmd_job *job,
	void *func, void *priv, void *parm, char *name, enum PWR_LEVEL pwr)
{
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;
	void *d = phl_to_drvpriv(phl_info);

	job->id = JOB_RUN_FUNC;
	job->pwr_level = pwr;
	job->u.cmd.fptr = (int (*)(void *priv, void *param, bool discard))func;
	job->u.cmd.priv = priv;
	job->u.cmd.parm = parm;
	_os_mem_set(d, job->u.cmd.name, 0, RTW_PHL_JOB_NAME_LEN);
	if (name != NULL)
		_os_mem_cpy(d, job->u.cmd.name, name,
			MIN((RTW_PHL_JOB_NAME_LEN - 1),
			_os_strlen((u8 *)name)));
	else
		_os_mem_cpy(d, job->u.cmd.name, unknow_func,
			MIN((RTW_PHL_JOB_NAME_LEN - 1),
			_os_strlen((u8 *)unknow_func)));
	return RTW_PHL_STATUS_SUCCESS;
}

/**
 * @phl: phl private structure
 * @func: function pointer to be runed
 * @priv: 1st param of function pointer
 * @parm: 2nd param of function pointer
 * @name: function name for debug message (optional)
 * @pwr: refers to enum PWR_LEVEL
 */
/* TODO remove below when no reference */
enum rtw_phl_status rtw_phl_job_add_fptr(void *phl, void *func,
	void *priv, void *parm, char *name, enum PWR_LEVEL pwr)
{
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;
	struct phl_cmd_job job;

	_os_mem_set(phl_to_drvpriv(phl_info), &job, 0, sizeof(job));
	rtw_phl_job_fill_fptr(phl, &job, func, priv, parm, name, pwr);

	return phl_cmd_complete_job(phl, &job);
}

static int job_run_func_hdl(struct _cmd_obj *pcmd, void *param)
{
	struct phl_cmd_job *job = (struct phl_cmd_job *)param;

	if (job->u.cmd.fptr)
		return job->u.cmd.fptr(job->u.cmd.priv, job->u.cmd.parm, false);
	else /* TODO remove else */
		return job->u.cmd.func(job->u.cmd.priv, job->u.cmd.parm);
}

/** JOB_ADD_STA
 * For EXTERNAL application to add sta entry (expose)
 * @phl: phl private structure
 * @sta: see rtw_phl_stainfo_t
 * @sync: Async or SYNC mode
 */
enum rtw_phl_status rtw_hal_add_sta_entry_job(void *phl,
	struct rtw_phl_stainfo_t *sta, enum JOB_SYNC sync)
{
	struct phl_info_t *phl_info = (struct phl_info_t *)phl;
	void *d = phl_to_drvpriv(phl_info);
	struct phl_cmd_job job;

	_os_mem_set(d, &job, 0, sizeof(job));
	job.id = JOB_ADD_STA_ENTRY;
	job.u.sta.sta = sta;
	job.wait.sync = sync;

	return phl_cmd_complete_job(phl, &job);
}

static int job_add_sta_entry_hdl(struct _cmd_obj *pcmd, void *param)
{
	struct phl_cmd_job *job = (struct phl_cmd_job *)param;

	return rtw_hal_add_sta_entry(cmd2hal(pcmd), job->u.sta.sta);
}

static int job_add_macid_hdl(struct _cmd_obj *pcmd, void *param)
{
	FSM_INFO(pcmd->fsm, "job: %s()\n", __func__);
	return 0;
}

struct job_hdl_ent job_hdl_tbl[] = {
	{JOB_RUN_FUNC, "JOB_RUN_FUNC", job_run_func_hdl},
	{JOB_ADD_STA_ENTRY, "JOB_ADD_STA_ENTRY", job_add_sta_entry_hdl},
	{JOB_ADD_MACID, "JOB_ADD_MACID", job_add_macid_hdl}
};

void cmd_set_job_tbl(struct _cmd_obj *pcmd)
{
	pcmd->job_tbl = job_hdl_tbl;
}

char *job_name(struct _cmd_obj *pcmd, u8 id)
{
	char *name = NULL;
	if (id >= JOB_MAX)
		FSM_ERR(pcmd->fsm, "%s: job id %d not found\n",
			phl_fsm_obj_name(pcmd->fsm_obj), id);
	else
		name = job_hdl_tbl[id].name;

	return name;
}

int phl_cmd_do_job(struct _cmd_obj *pcmd, void *param)
{
	struct phl_cmd_job *job = (struct phl_cmd_job *)param;
	void *d = phl_to_drvpriv(pcmd->phl_info);
	int rtn = FSM_FREE_PARAM;


	if (job->id == JOB_RUN_FUNC)
		FSM_INFO(pcmd->fsm, "%s: %s (%s)\n",
			phl_fsm_obj_name(pcmd->fsm_obj),
			(char *)job_name(pcmd, (u8)job->id),
			(char *)job->u.cmd.name);
	else
		FSM_INFO(pcmd->fsm, "%s: %s\n",
			phl_fsm_obj_name(pcmd->fsm_obj),
			(char *)job_name(pcmd, (u8)job->id));

	job->wait.rtn = pcmd->job_tbl[job->id].job_func(pcmd, job);

	if (job->wait.sync == JOB_WAIT_COMPLETION) {

		_os_event_set(d, &(job->wait.done));
		/* Inform fsm framwork do NOT free param
		 * SYNC mode use caller's local job variable
		 */
		rtn = FSM_KEEP_PARAM;
	}
	return rtn;
}
#endif /*CONFIG_FSM*/


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
#ifndef __PHL_CMD_FSM_H__
#define __PHL_CMD_FSM_H__

#include "phl_cmd_job.h"

/* Header file for application to invoke command service */

struct phl_cmd_job;
struct job_hdl_ent;
struct fsm_root;
struct fsm_main;
struct _cmd_obj;

struct _cmd_obj {
	struct fsm_main *fsm;
	struct phl_info_t *phl_info;
	struct fsm_obj *fsm_obj;
	struct job_hdl_ent *job_tbl;

	u8 wdog_pwr_level;
	u32 wdog_pause_num;
	bool has_power;

	struct	list_head wd_q;
	_os_mutex wd_q_lock;
};

/* function form command service management */
struct fsm_main *phl_cmd_new_fsm(struct fsm_root *fsm_m,
	struct phl_info_t *phl_info);
void phl_cmd_destory_fsm(struct fsm_main *fsm);
struct _cmd_obj *phl_cmd_new_obj(struct fsm_main *fsm,
	struct phl_info_t *phl_info);
void phl_cmd_destory_obj(struct _cmd_obj *pcmd);
void phl_fsm_cmd_stop(struct phl_info_t *phl_info);

/* function form command service */
enum rtw_phl_status phl_cmd_start(struct _cmd_obj *pcmd);
enum rtw_phl_status phl_cmd_cancel(struct _cmd_obj *pcmd);

#endif /* __PHL_CMD_FSM_H__ */


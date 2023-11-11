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
#ifndef __PHL_SER_FSM_H__
#define __PHL_SER_FSM_H__

struct fsm_main *phl_ser_new_fsm(struct fsm_root *root,
	struct phl_info_t *phl_info);
void phl_ser_destory_fsm(struct fsm_main *fsm);
struct ser_obj *phl_ser_new_obj(struct fsm_main *fsm,
	struct phl_info_t *phl_info);
void phl_ser_destory_obj(struct ser_obj *pser);
enum rtw_phl_status phl_ser_cancel(void *phl);
enum rtw_phl_status phl_ser_event_to_fw(void *phl, u32 err);
enum rtw_phl_status rtw_phl_ser_dump_ple_buffer(void *phl);
enum rtw_phl_status phl_fw_watchdog_timeout_notify(void *phl);

enum rtw_phl_status
phl_ser_send_msg(void *phl, enum RTW_PHL_SER_NOTIFY_EVENT notify);

u8 phl_ser_inprogress(void *phl);
void phl_ser_clear_status(struct ser_obj *pser, u32 serstatus);
void phl_ser_set_status(struct ser_obj *pser, u32 serstatus);
void phl_ser_notify_from_upper_watchdog_status(void *phl, bool inprogress);

u8 rtw_phl_ser_inprogress(void *phl);
void rtw_phl_ser_clear_status(void *phl, u32 serstatus);
void rtw_phl_notify_watchdog_status(void *phl, bool inprogress);

#endif /* __PHL_SER_FSM_H__ */

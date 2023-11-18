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
#ifndef __PHL_CMD_SER_H__
#define __PHL_CMD_SER_H__

#ifndef CONFIG_FSM
enum rtw_phl_status phl_fw_watchdog_timeout_notify(void *phl);
enum rtw_phl_status rtw_phl_ser_dump_ple_buffer(void *phl);
#endif

enum rtw_phl_status phl_ser_send_msg(void *phl, enum RTW_PHL_SER_NOTIFY_EVENT notify);

#ifdef CONFIG_PHL_CMD_SER
enum rtw_phl_status
phl_register_ser_module(struct phl_info_t *phl_info);
#endif

#endif /* __PHL_CMD_SER_H__ */

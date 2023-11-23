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
#ifndef _PHL_CHAN_INFO_H_
#define _PHL_CHAN_INFO_H_

#ifdef CONFIG_PHL_CHANNEL_INFO
enum rtw_phl_status rtw_phl_cmd_cfg_chinfo(void *phl,
					   struct rtw_chinfo_action_parm *act_parm,
					   enum phl_cmd_type cmd_type,
					   u32 cmd_timeout);
enum rtw_phl_status phl_chaninfo_init(struct phl_info_t *phl_info);
void phl_chaninfo_deinit(struct phl_info_t *phl_info);

/* Channel info queue operation*/
u32 rtw_phl_get_chaninfo_idle_number(void *drvpriv, struct rtw_phl_com_t *phl_com);
u32 rtw_phl_get_chaninfo_busy_number(void *drvpriv, struct rtw_phl_com_t *phl_com);
struct chan_info_t *rtw_phl_query_idle_chaninfo(void *drvpriv, struct rtw_phl_com_t *phl_com);

struct chan_info_t *rtw_phl_query_busy_chaninfo(void *drvpriv, struct rtw_phl_com_t *phl_com);

struct chan_info_t *rtw_phl_query_busy_chaninfo_latest(void *drvpriv, struct rtw_phl_com_t *phl_com);


void rtw_phl_enqueue_idle_chaninfo(void *drvpriv, struct rtw_phl_com_t *phl_com,
				struct chan_info_t *chan_info_pkt);

struct chan_info_t * rtw_phl_recycle_busy_chaninfo(void *drvpriv, struct rtw_phl_com_t *phl_com,
				struct chan_info_t *chan_info_pkt);

enum rtw_phl_status
phl_cmd_cfg_chinfo_hdl(struct phl_info_t *phl_info, u8 *param);
#endif /* CONFIG_PHL_CHANNEL_INFO */
#endif /* _PHL_CHAN_INFO_H_ */

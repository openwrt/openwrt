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
#ifndef _PHL_CMD_GENERAL_H_
#define _PHL_CMD_GENERAL_H_

#ifdef CONFIG_MR_COEX_SUPPORT
#define RTW_DUMP_HAL_CR(_phl, _evt, _band) \
	do {\
		if (true == rtw_phl_mr_coex_query_inprogress(_phl,\
				_band, RTW_MR_COEX_CHK_INPROGRESS)) {\
			rtw_hal_notification_ex(_phl->hal, _evt,\
					true, true, false, _band);\
		} else {\
			rtw_hal_notification_ex(_phl->hal, _evt,\
					true, true, true, _band);\
		}\
	} while(0);
#else
#define RTW_DUMP_HAL_CR(_phl, _evt, _band) \
	do {\
		rtw_hal_notification_ex(_phl->hal, _evt,\
					true, true, true, _band);\
	} while(0);
#endif

enum rtw_phl_status
phl_cmd_get_cur_cmdinfo(struct phl_info_t *phl_info,
					u8 band_idx,
					struct phl_msg *msg,
					u8 **cmd, u32 *cmd_len);

enum rtw_phl_status
phl_cmd_get_cur_cmdinfo(struct phl_info_t *phl_info,
					u8 band_idx,
					struct phl_msg *msg,
					u8 **cmd, u32 *cmd_len);

enum rtw_phl_status phl_register_cmd_general(struct phl_info_t *phl_info);

enum rtw_phl_status
phl_cmd_enqueue(struct phl_info_t *phl_info,
                enum phl_band_idx band_idx,
                enum phl_msg_evt_id evt_id,
                u8 *cmd_buf,
                u32 cmd_len,
                void (*cmd_complete)(void *priv, u8 *cmd, u32 cmd_len, enum rtw_phl_status status),
                enum phl_cmd_type cmd_type,
                u32 cmd_timeout);
#endif /*_PHL_CMD_GENERAL_H_*/

/******************************************************************************
 *
 * Copyright(c) 2020 Realtek Corporation.
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
#ifndef _PHL_SOUND_CMD_H_
#define _PHL_SOUND_CMD_H_

#ifdef CONFIG_PHL_CMD_BF

enum snd_cmd_disp_ctrl {
	SND_CMD_DISP_CTRL_BFEE = 0,
	SND_CMD_DISP_CTRL_BFER,
	SND_CMD_DISP_CTRL_MAX
};

enum snd_cmd_set_info_opcode {
	SND_CMD_OP_NONE = 0,
	SND_CMD_OP_SET_AID = 1,
	SND_CMD_OP_MAX = 2,
};

enum snd_cmd_bfer_event {
	SND_CMD_BFER_PRECFG = 0,
	SND_CMD_BFER_MAC_CTRL_LOCK = 1,
	SND_CMD_BFER_SOUND = 2,
	SND_CMD_BFER_MAC_CTRL_FREE = 3,
	SND_CMD_BFER_POSTCFG = 4,
	SND_CMD_BFER_LEAVE = 5,
	SND_CMD_BFER_GROUPING = 6,
	SND_CMD_BFER_CHECK_NEXT = 7,
	SND_CMD_BFER_TERMINATE = 8,
	SND_CMD_BFER_MAX
};
struct snd_cmd_bfer {
	enum snd_cmd_bfer_event event;
	struct rtw_wifi_role_t *wrole;
	struct phl_snd_grp *snd_grp;
	enum phl_mdl_ret_code cmd_sts;
};

struct snd_cmd_set_aid {
	u16 aid;
	enum phl_mdl_ret_code cmd_sts;
	struct rtw_phl_stainfo_t *sta_info;
};

enum rtw_phl_status
rtw_phl_snd_cmd_set_aid(void *phl,
			struct rtw_wifi_role_t *wrole,
			struct rtw_phl_stainfo_t *sta,
			u16 aid);

enum rtw_phl_status
phl_snd_cmd_ntfy_ps(struct phl_info_t *phl,
		    struct rtw_wifi_role_t *wrole,
		    bool enter);


enum rtw_phl_status phl_snd_cmd_register_module(struct phl_info_t *phl_info);

enum rtw_phl_status
phl_snd_cmd_sound_evt(void *phl,
			  struct rtw_wifi_role_t *wrole,
			  struct phl_snd_grp *snd_grp,
			  enum snd_cmd_bfer_event event);


void phl_snd_cmd_sound_cancel_msg(struct phl_info_t *phl_info);
#else
#define phl_snd_cmd_register_module(_p) RTW_PHL_STATUS_SUCCESS
#define phl_snd_cmd_sound_cancel_msg(_p)
#define phl_snd_cmd_sound_evt(_p, _w, _g, _e) RTW_PHL_STATUS_SUCCESS
#define phl_snd_cmd_ntfy_ps(_p, _w, _e) RTW_PHL_STATUS_SUCCESS
#endif /* end of CONFIG_PHL_CMD_BF */
#endif
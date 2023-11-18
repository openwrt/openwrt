/******************************************************************************
 *
 * Copyright(c) 2021 Realtek Corporation.
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
#ifndef _PHL_PS_H_
#define _PHL_PS_H_
#ifdef CONFIG_POWER_SAVE
enum phl_ps_mode {
	PS_MODE_NONE,
	PS_MODE_LPS,
	PS_MODE_IPS
};

#define PS_MACID_NONE 0xFFFF

enum phl_ps_lps_int_cfg_step {
	LPS_ENTER_CFG_INT_PRE_PHASE,
	LPS_ENTER_CFG_INT_POST_PHASE,
	LPS_LEAVE_CFG_INT_PRE_PHASE,
	LPS_LEAVE_CFG_INT_POST_PHASE,
	LPS_LEAVE_CFG_INT_MAX
};

/* use to configure specific pwr mode along with pwr lvl and others */
struct ps_cfg {
	/* common */
	u8 ps_mode;
	u8 cur_pwr_lvl;
	u8 pwr_lvl;
	u16 macid;
	/* lps */
	bool pwr_cfg; /* whether to configure pwr lvl */
	bool proto_cfg; /* whether to configure protocol */
	u32 *token;
	u8 listen_bcn_mode;
	u8 awake_interval;
	u8 smart_ps_mode;
	u8 bcnnohit_en;
};

#define _get_ps_cap(_phl_info) (&_phl_info->phl_com->dev_cap.ps_cap)
#define _get_ps_sw_cap(_phl_info) (&_phl_info->phl_com->dev_sw_cap.ps_cap)

const char *phl_ps_op_mode_to_str(u8 op_mode);
const char *phl_ps_ps_mode_to_str(u8 ps_mode);
const char *phl_ps_pwr_lvl_to_str(u8 pwr_lvl);
u8 phl_ps_judge_pwr_lvl(u8 ps_cap, u8 ps_mode, u8 ps_en);

enum rtw_phl_status
phl_ps_ips_cfg(struct phl_info_t *phl_info, struct ps_cfg *cfg, u8 ips_en);
enum rtw_phl_status phl_ps_lps_cfg(struct phl_info_t *phl_info, struct ps_cfg *cfg, u8 lps_en);
enum rtw_phl_status phl_ps_cfg_pwr_lvl(struct phl_info_t *phl_info, u8 ps_mode, u8 cur_pwr_lvl, u8 req_pwr_lvl);

enum rtw_phl_status phl_ps_enter_ps(struct phl_info_t *phl_info, struct ps_cfg *cfg);
enum rtw_phl_status phl_ps_leave_ps(struct phl_info_t *phl_info, struct ps_cfg *cfg);
#endif
#endif /* _PHL_PS_H_ */

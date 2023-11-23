/******************************************************************************
 *
 * Copyright(c) 2007 - 2022 Realtek Corporation.
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
#ifndef __RTW_DFS_H__
#define __RTW_DFS_H__

#define CAC_TIME_MS (60*1000)
#define CAC_TIME_CE_MS (10*60*1000)
#define NON_OCP_TIME_MS (30*60*1000)

#define RTW_CAC_STOPPED 0 /* used by cac_start_time, cac_end_time time stamps */
#define RTW_NON_OCP_STOPPED 0 /* used by non_ocp_end_time time stamps */

bool rtw_chset_is_dfs_range(const struct rtw_chset *chset, u32 hi, u32 lo);
RTW_FUNC_2G_5G_ONLY bool rtw_chset_is_dfs_ch(const struct rtw_chset *chset, u8 ch);
RTW_FUNC_2G_5G_ONLY bool rtw_chset_is_dfs_chbw(const struct rtw_chset *chset, u8 ch, u8 bw, u8 offset);
bool rtw_chset_is_dfs_bch(const struct rtw_chset *chset, enum band_type band, u8 ch);
bool rtw_chset_is_dfs_bchbw(const struct rtw_chset *chset, enum band_type band, u8 ch, u8 bw, u8 offset);

enum rtw_dfs_regd rtw_rfctl_get_dfs_domain(struct rf_ctl_t *rfctl);
bool rtw_rfctl_radar_detect_supported(struct rf_ctl_t *rfctl);

#ifdef CONFIG_DFS_MASTER
struct rf_ctl_t;
enum phl_band_idx;

#define CH_IS_NON_OCP_STOPPED(rt_ch_info) ((rt_ch_info)->non_ocp_end_time == RTW_NON_OCP_STOPPED)
#define CH_IS_NON_OCP(rt_ch_info) (!CH_IS_NON_OCP_STOPPED(rt_ch_info) && rtw_time_after((rt_ch_info)->non_ocp_end_time, rtw_get_current_time()))

bool rtw_rfctl_overlap_radar_detect_ch(struct rf_ctl_t *rfctl, enum band_type band, u8 ch, u8 bw, u8 offset);
bool rtw_rfctl_hwband_is_tx_blocked_by_ch_waiting(struct rf_ctl_t *rfctl, enum phl_band_idx hwband);
bool adapter_is_tx_blocked_by_ch_waiting(_adapter *adapter);
bool alink_is_tx_blocked_by_ch_waiting(struct _ADAPTER_LINK *alink);

RTW_FUNC_2G_5G_ONLY bool rtw_chset_is_chbw_non_ocp(const struct rtw_chset *chset, u8 ch, u8 bw, u8 offset);
RTW_FUNC_2G_5G_ONLY bool rtw_chset_is_ch_non_ocp(const struct rtw_chset *chset, u8 ch);
bool rtw_chset_is_bchbw_non_ocp(const struct rtw_chset *chset, enum band_type band, u8 ch, u8 bw, u8 offset);
bool rtw_chset_is_bch_non_ocp(const struct rtw_chset *chset, enum band_type band, u8 ch);

void rtw_rfctl_chset_chk_non_ocp_finish(struct rf_ctl_t *rfctl);

void rtw_rfctl_force_update_non_ocp_ms(struct rf_ctl_t *rfctl, enum band_type band, u8 ch, u8 bw, u8 offset, int ms);

u32 rtw_get_ch_waiting_ms(struct rf_ctl_t *rfctl, enum band_type band, u8 ch, u8 bw, u8 offset, u32 *r_non_ocp_ms, u32 *r_cac_ms);

u32 rtw_force_stop_cac(struct rf_ctl_t *rfctl, u32 timeout_ms);

u8 rtw_dfs_rd_hdl(struct dvobj_priv *dvobj, enum phl_band_idx hwband, u8 radar_cch, enum channel_width radar_bw);

void rtw_dfs_rd_en_dec_on_mlme_act(_adapter *adapter, struct _ADAPTER_LINK *alink, u8 mlme_act, u8 excl_ifbmp);
void rtw_dfs_rd_en_dec_update(struct dvobj_priv *dvobj, enum phl_band_idx hwband);
u8 rtw_dfs_rd_en_decision_cmd(struct dvobj_priv *dvobj, enum phl_band_idx hwband);

void rtw_indicate_cac_state_on_bss_start(_adapter *adapter);
void rtw_indicate_cac_state_on_bss_stop(_adapter *adapter);

void dump_radar_detect_status(void *sel, struct rf_ctl_t *rfctl, const char *title);

#else
#define CH_IS_NON_OCP(rt_ch_info) 0
#define rtw_rfctl_hwband_is_tx_blocked_by_ch_waiting(rfctl, hwband) false
#define adapter_is_tx_blocked_by_ch_waiting(adapter) false
#define alink_is_tx_blocked_by_ch_waiting(alink) false

RTW_FUNC_2G_5G_ONLY static inline bool rtw_chset_is_chbw_non_ocp(const struct rtw_chset *chset, u8 ch, u8 bw, u8 offset) { return false; }
RTW_FUNC_2G_5G_ONLY static inline bool rtw_chset_is_ch_non_ocp(const struct rtw_chset *chset, u8 ch) { return false; }
#define rtw_chset_is_bchbw_non_ocp(chset, band, ch, bw, offset) false
#define rtw_chset_is_bch_non_ocp(chset, band, ch) false
#endif /* CONFIG_DFS_MASTER */

bool rtw_rfctl_choose_bchbw(struct rf_ctl_t *rfctl
	, enum band_type sel_band, u8 sel_ch, u8 max_bw
	, enum band_type cur_band, u8 cur_ch
	, enum band_type *band, u8 *ch, u8 *bw, u8 *offset
	, bool by_int_info, u8 mesh_only, const char *caller);

RTW_FUNC_2G_5G_ONLY bool rtw_rfctl_choose_chbw(struct rf_ctl_t *rfctl, u8 sel_ch, u8 max_bw, u8 cur_ch
	, u8 *ch, u8 *bw, u8 *offset, bool by_int_info, u8 mesh_only, const char *caller);

void rtw_rfctl_dfs_init(struct rf_ctl_t *rfctl, struct registry_priv *regsty);

#endif /* __RTW_DFS_H__ */

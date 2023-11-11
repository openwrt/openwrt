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
#ifndef __RTW_CHSET_H__
#define __RTW_CHSET_H__

enum rtw_ch_type {
	RTW_CHT_DIS		= 0,
	RTW_CHT_NO_IR		= 1,
	RTW_CHT_DFS		= 2,
	RTW_CHT_NO_HT40U	= 3,
	RTW_CHT_NO_HT40L	= 4,
	RTW_CHT_NO_80MHZ	= 5,
	RTW_CHT_NO_160MHZ	= 6,
	RTW_CHT_NUM,
};

extern const char *const _rtw_ch_type_str[];
#define rtw_ch_type_str(type) (((type) >= RTW_CHT_NUM) ? _rtw_ch_type_str[RTW_CHT_NUM] : _rtw_ch_type_str[(type)])

enum rtw_ch_type get_ch_type_from_str(const char *str, size_t str_len);

enum {
	RTW_CHF_DIS 		= BIT(RTW_CHT_DIS),
	RTW_CHF_NO_IR		= BIT(RTW_CHT_NO_IR),
	RTW_CHF_DFS		= BIT(RTW_CHT_DFS),
	RTW_CHF_NO_HT40U	= BIT(RTW_CHT_NO_HT40U),
	RTW_CHF_NO_HT40L	= BIT(RTW_CHT_NO_HT40L),
	RTW_CHF_NO_80MHZ	= BIT(RTW_CHT_NO_80MHZ),
	RTW_CHF_NO_160MHZ	= BIT(RTW_CHT_NO_160MHZ),
};

#define RTW_CH_FLAGS_STR_LEN (45)
char *rtw_get_ch_flags_str(char *buf, u8 flags, char delim);

/* The channel information about this channel including joining, scanning, and power constraints. */
typedef struct _RT_CHANNEL_INFO {
	u8 band; /* enum band_type */
	u8 ChannelNum; /* The channel number. */

	/*
	* Bitmap and its usage:
	* RTW_CHF_DIS, RTW_CHF_NO_IR, RTW_CHF_DFS: is used to check for status
	* RTW_CHF_NO_HT40U, RTW_CHF_NO_HT40L, RTW_CHF_NO_80MHZ, RTW_CHF_NO_160MHZ: extra bandwidth limitation (ex: from regulatory)
	*/
	u8 flags;

	bool reg_no_ir;
	systime bcn_hint_end_time;

#ifdef CONFIG_FIND_BEST_CHANNEL
	u32 rx_count;
#endif

#if CONFIG_IEEE80211_BAND_5GHZ && CONFIG_DFS
	#ifdef CONFIG_DFS_MASTER
	bool cac_done;
	systime non_ocp_end_time;
	#endif
#endif

	u8 hidden_bss_cnt; /* per scan count */

#ifdef CONFIG_IOCTL_CFG80211
	void *os_chan;
#endif
} RT_CHANNEL_INFO, *PRT_CHANNEL_INFO;

#define RTW_BCN_HINT_STOPPED 0 /* used by bcn_hint_end_time time stamps */
#define CH_IS_BCN_HINT_STOPPED(rt_ch_info) ((rt_ch_info)->bcn_hint_end_time == RTW_BCN_HINT_STOPPED)
#define CH_IS_BCN_HINT(rt_ch_info) (!CH_IS_BCN_HINT_STOPPED(rt_ch_info) && rtw_time_after((rt_ch_info)->bcn_hint_end_time, rtw_get_current_time()))

struct rtw_chset {
	RT_CHANNEL_INFO chs[MAX_CHANNEL_NUM];
	u8 chs_len;
	RT_CHANNEL_INFO *chs_of_band[BAND_MAX];
	u8 chs_len_of_band[BAND_MAX];
	u8 enable_ch_num;
};

int rtw_chset_init(struct rtw_chset *chset, u8 band_bmp);

RTW_FUNC_2G_5G_ONLY int rtw_chset_search_ch(const struct rtw_chset *chset, u32 ch);
RTW_FUNC_2G_5G_ONLY u8 rtw_chset_is_chbw_valid(const struct rtw_chset *chset, u8 ch, u8 bw, u8 offset
	, bool allow_primary_passive, bool allow_passive);
RTW_FUNC_2G_5G_ONLY void rtw_chset_sync_chbw(const struct rtw_chset *chset, u8 *req_ch, u8 *req_bw, u8 *req_offset
	, u8 *g_ch, u8 *g_bw, u8 *g_offset, bool allow_primary_passive, bool allow_passive);
int rtw_chset_search_bch(const struct rtw_chset *chset, enum band_type band, u32 ch);
RT_CHANNEL_INFO *rtw_chset_get_chinfo_by_bch(struct rtw_chset *chset, enum band_type band, u32 ch, bool include_dis);
u8 rtw_chset_is_bchbw_valid(const struct rtw_chset *chset, enum band_type band, u8 ch, u8 bw, u8 offset
	, bool allow_primary_passive, bool allow_passive);
void rtw_chset_sync_bchbw(const struct rtw_chset *chset, enum band_type *req_band, u8 *req_ch, u8 *req_bw, u8 *req_offset
	, enum band_type *g_band, u8 *g_ch, u8 *g_bw, u8 *g_offset, bool allow_primary_passive, bool allow_passive);

u8 *rtw_chset_set_spt_chs_ie(struct rtw_chset *chset, u8 *buf_pos, uint *buf_len);

#ifdef CONFIG_PROC_DEBUG
void dump_chinfos(void *sel, const RT_CHANNEL_INFO *chinfos, u8 chinfo_num);
#endif

#endif /* __RTW_CHSET_H__ */

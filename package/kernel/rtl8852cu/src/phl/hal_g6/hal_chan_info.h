/******************************************************************************
 *
 * Copyright(c)2019 Realtek Corporation.
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
#ifndef _HAL_CHAN_INFO_H_
#define _HAL_CHAN_INFO_H_

#ifdef CONFIG_PHL_CHANNEL_INFO

void
_hal_fill_csi_header_remain(void* hal, struct csi_header_t *csi_header
	, struct rtw_r_meta_data *mdata);

void
_hal_fill_csi_header_phy_info(void* hal, struct csi_header_t *csi_header
	, struct ch_rpt_hdr_info *ch_hdr_rpt, struct phy_info_rpt *phy_info);

void
rtw_hal_get_ch_info_physts(void *hal,
							struct rtw_r_meta_data *mdata,
							struct hal_ppdu_sts_usr *usr,
							struct rtw_phl_ppdu_phy_info *phy_info);


#ifdef CONFIG_PHL_WKARD_CHANNEL_INFO_ACK
u8 rtw_hal_ch_info_process_ack(struct rtw_r_meta_data *meta,
									struct rtw_phl_ppdu_sts_info *ppdu_info,
									struct rtw_chinfo_cur_parm *cur_parm,
									u16 macid);
#endif

#ifdef CONFIG_PHL_CHANNEL_INFO_DBG
void
hal_print_csi_raw_data(struct chan_info_t *chan_info);
#endif
#endif /* CONFIG_PHL_CHANNEL_INFO */
#endif /* _HAL_CHAN_INFO_H_ */


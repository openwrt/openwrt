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
#ifndef __HAL_CSI_BUFFER_H__
#define __HAL_CSI_BUFFER_H__

#define CSI_BUF_IDX_HW_MSK 0x7FF


#define CSI_BUF_SUB_IDX_FULL_BW 0b0
#define CSI_BUF_SUB_IDX_40_U 0b100100
#define CSI_BUF_SUB_IDX_40_L 0b101000
#define CSI_BUF_SUB_IDX_20_UU 0b000100
#define CSI_BUF_SUB_IDX_20_UL 0b001000
#define CSI_BUF_SUB_IDX_20_LU 0b001100
#define CSI_BUF_SUB_IDX_20_LL 0b010000
/* following is used in csi buffer size is 160Mhz */
#define CSI_BUF_SUB_IDX_160M_20_LL 0b010000
#define CSI_BUF_SUB_IDX_160M_20_LL 0b010000
#define CSI_BUF_SUB_IDX_160M_20_LL 0b010000
#define CSI_BUF_SUB_IDX_160M_20_LL 0b010000
#define CSI_BUF_SUB_IDX_160M_40_U 0b101100
#define CSI_BUF_SUB_IDX_160M_40_L 0b110000
#define CSI_BUF_SUB_IDX_160M_80_U 0b110100
#define CSI_BUF_SUB_IDX_160M_80_L 0b111000

#define CSI_BUF_SUB_IDX_NON 0b111111

#define CSI_BUF_STS_BUSY 1
#define CSI_BUF_STS_IDLE 0

#define IS_SUB20_BUSY(_csi, _idx) \
	(((_csi->sub_idx&BIT(_idx)) == BIT(_idx)) ? CSI_BUF_STS_BUSY : CSI_BUF_STS_IDLE)

#define SET_SUB20_BUSY(_csi, _idx) _csi->sub_idx |= BIT(_idx)

#define IS_40L_BUSY(_csi) \
	(((_csi->sub_idx & (BIT(0)|BIT(1)))==(BIT(0)|BIT(1))) ? CSI_BUF_STS_BUSY : CSI_BUF_STS_IDLE)
#define IS_40U_BUSY(_csi) \
	(((_csi->sub_idx & (BIT(2)|BIT(3)))==(BIT(2)|BIT(3))) ? CSI_BUF_STS_BUSY : CSI_BUF_STS_IDLE)

#define SET_40L_BUSY(_csi) _csi->sub_idx |= (BIT(0)|BIT(1))
#define SET_40U_BUSY(_csi) _csi->sub_idx |= (BIT(2)|BIT(3))

#define CLEAR_CSI_STS_BIT(_csi, _x) _csi->sub_idx &= ~(1 << _x)

enum hal_csi_buf_type {
	HAL_CSI_BUF_TYPE_SU = 0,
	HAL_CSI_BUF_TYPE_MU = 1
};

enum hal_csi_buf_size {
	HAL_CSI_BUF_SIZE_NONE = 0,
	HAL_CSI_BUF_SIZE_20,
	HAL_CSI_BUF_SIZE_40,
	HAL_CSI_BUF_SIZE_80,
	HAL_CSI_BUF_SIZE_160
};

struct hal_csi_buf {
	u16 sub_idx:6;
	u16 idx:5;
	u16 type:1;
	u16 b160mhz:1;
	u16 rsvd:3;
};

struct hal_csi_obj {
	u8 max_csi_buf_nr;
	u8 max_csi_buf_nr_su;
	u8 max_csi_buf_nr_mu;
	struct hal_csi_buf *csi_buf;
	_os_lock csi_lock;
};

enum rtw_hal_status 
hal_csi_init(struct hal_info_t *hal_info, u8 su_buf_nr, u8 mu_buf_nr);

void hal_csi_deinit(struct hal_info_t *hal_info);

enum rtw_hal_status hal_csi_query_idle_csi_buf(
	struct hal_info_t *hal_info, u8 mu, enum channel_width bw, void *buf);

enum rtw_hal_status hal_csi_release_csi_buf(
	struct hal_info_t *hal_info,
	void *buf);

u8 hal_is_csi_buf_valid(struct hal_info_t *hal_info, void *buf);

enum channel_width
rtw_hal_get_csi_buf_bw(void *buf);

bool
rtw_hal_get_csi_buf_type(void *buf);

#endif

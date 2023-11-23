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
#include "hal_headers.h"

/**
 * _dump_csi_buf_status
 * 	Dump all of the csi buffer status;
 * @csi_obj: (struct hal_csi_obj *)
 **/
void _dump_csi_buf_status(struct hal_csi_obj *csi_obj)
{
	struct hal_csi_buf *csi_buf = NULL;
	u8 i = 0;
	PHL_TRACE(COMP_PHL_DBG, _PHL_INFO_, "===> DUMP HAL CSI Buffer Status\n");
	for (i = 0; i < csi_obj->max_csi_buf_nr; i++) {
		csi_buf = &csi_obj->csi_buf[i];
		PHL_TRACE(COMP_PHL_DBG, _PHL_INFO_, "[CSI BUF][%d] status 0x%x \n",
				csi_buf->idx, csi_buf->sub_idx);
		PHL_TRACE(COMP_PHL_DBG, _PHL_INFO_, "[CSI BUF] 20UU (%d) \n",
				IS_SUB20_BUSY(csi_buf, 3));
		PHL_TRACE(COMP_PHL_DBG, _PHL_INFO_, "[CSI BUF] 20UL (%d) \n",
				IS_SUB20_BUSY(csi_buf, 2));
		PHL_TRACE(COMP_PHL_DBG, _PHL_INFO_, "[CSI BUF] 20LU (%d) \n",
				IS_SUB20_BUSY(csi_buf, 1));
		PHL_TRACE(COMP_PHL_DBG, _PHL_INFO_, "[CSI BUF] 20LL (%d) \n",
				IS_SUB20_BUSY(csi_buf, 0));
	}
	PHL_TRACE(COMP_PHL_DBG, _PHL_INFO_, "<=== DUMP HAL CSI Buffer Status\n");
}

/**
 * __query_avl_buf_idx_20
 * 	Get available sub 20MHz csi buffer
 * input :
 * @csi_buf: (struct hal_csi_buf *) 80MHz hal csi buffer for checking avaible
 * return
 * @sub_idx: (u8) csi buffer sub index
 **/
u8 __query_avl_buf_idx_20(struct hal_csi_buf *csi_buf)
{
	u8 sub_idx = CSI_BUF_SUB_IDX_NON;
	do {
		if (IS_SUB20_BUSY(csi_buf, 0) == CSI_BUF_STS_IDLE) {
			SET_SUB20_BUSY(csi_buf, 0);
			sub_idx = CSI_BUF_SUB_IDX_20_LL;
			break;
		}
		if (IS_SUB20_BUSY(csi_buf, 1) == CSI_BUF_STS_IDLE) {
			SET_SUB20_BUSY(csi_buf, 1);
			sub_idx = CSI_BUF_SUB_IDX_20_LU;
			break;
		}
		if (IS_SUB20_BUSY(csi_buf, 2) == CSI_BUF_STS_IDLE) {
			SET_SUB20_BUSY(csi_buf, 2);
			sub_idx = CSI_BUF_SUB_IDX_20_UL;
			break;
		}
		if (IS_SUB20_BUSY(csi_buf, 3) == CSI_BUF_STS_IDLE) {
			SET_SUB20_BUSY(csi_buf, 3);
			sub_idx = CSI_BUF_SUB_IDX_20_UU;
			break;
		}
	} while (0);

	return sub_idx;
}
/**
 * __query_avl_buf_idx_40
 * 	Get available sub 40MHz csi buffer
 * input :
 * @csi_buf: (struct hal_csi_buf *) 80MHz hal csi buffer for checking avaible
 * return
 * @sub_idx: (u8) csi buffer sub index
 **/
u8 __query_avl_buf_idx_40(struct hal_csi_buf *csi_buf)
{
	u8 sub_idx = CSI_BUF_SUB_IDX_NON;
	do {
		if(IS_40L_BUSY(csi_buf) == CSI_BUF_STS_IDLE) {
			SET_40L_BUSY(csi_buf);
			sub_idx = CSI_BUF_SUB_IDX_40_L;
			break;
		}
		if (IS_40U_BUSY(csi_buf) == CSI_BUF_STS_IDLE) {
			SET_40U_BUSY(csi_buf);
			sub_idx = CSI_BUF_SUB_IDX_40_U;
			break;
		}
	} while (0);

	return sub_idx;
}
/**
 * __query_avl_buf_idx_80
 * 	Get available sub 80MHz csi buffer
 * input :
 * @csi_buf: (struct hal_csi_buf *) 80MHz hal csi buffer for checking avaible
 * return
 * @sub_idx: (u8) csi buffer sub index
 **/
u8 __query_avl_buf_idx_80(struct hal_csi_buf *csi_buf)
{
	u8 sub_idx = CSI_BUF_SUB_IDX_NON;

	if (csi_buf->sub_idx == CSI_BUF_STS_IDLE) {
		sub_idx = CSI_BUF_SUB_IDX_FULL_BW;
		csi_buf->sub_idx = 0xF;
	}

	return sub_idx;
}

/**
 * __query_avl_buf_idx_160
 * 	Get available 160MHz csi buffer
 * input :
 * @csi_buf: (struct hal_csi_buf *) 160MHz hal csi buffer for checking avaible
 * return
 * @sub_idx: (u8) csi buffer sub index
 **/
u8 __query_avl_buf_idx_160(struct hal_csi_buf *csi_buf)
{
	u8 sub_idx = CSI_BUF_SUB_IDX_NON;

	if (csi_buf->sub_idx == CSI_BUF_STS_IDLE) {
		sub_idx = CSI_BUF_SUB_IDX_FULL_BW;
		csi_buf->sub_idx = 0xF;
	}

	return sub_idx;
}

/**
 * _query_csi_buf_su
 * 	Get available sub 40MHz csi buffer
 * input :
 * @hal_info: (struct hal_info_t *)
 * @size:enum hal_csi_buf_size 20/40/80 MHz
 * return
 * @sub_id: (u8) available csi buffer sub index
 * @csi_buf: (struct hal_csi_buf *) original hal csi buffer
 **/
struct hal_csi_buf *_query_csi_buf_su(
	struct hal_info_t *hal_info,
	enum hal_csi_buf_size size,
	u8 *sub_id)
{
	struct rtw_hal_com_t *hal_com = hal_info->hal_com;
	struct hal_csi_obj *csi_obj = (struct hal_csi_obj *)hal_com->csi_obj;
	struct hal_csi_buf *csi_buf = NULL;
	u8 i = 0;
	u8 e_idx_su = csi_obj->max_csi_buf_nr_su;

	*sub_id = CSI_BUF_SUB_IDX_NON;

	switch (size) {
	case HAL_CSI_BUF_SIZE_20:
		for (i = 0; i < e_idx_su; i++) {
			*sub_id = __query_avl_buf_idx_20(&csi_obj->csi_buf[i]);
			if(*sub_id != CSI_BUF_SUB_IDX_NON) {
				csi_buf = &csi_obj->csi_buf[i];
				break;
			}
		}
	break;
	case HAL_CSI_BUF_SIZE_40:
		for (i = 0; i < e_idx_su; i++) {
			*sub_id = __query_avl_buf_idx_40(&csi_obj->csi_buf[i]);
			if(*sub_id != CSI_BUF_SUB_IDX_NON) {
				csi_buf = &csi_obj->csi_buf[i];
				break;
			}
		}
	break;
	case HAL_CSI_BUF_SIZE_80:
		for (i = 0; i < e_idx_su; i++) {
			*sub_id = __query_avl_buf_idx_80(&csi_obj->csi_buf[i]);
			if(*sub_id != CSI_BUF_SUB_IDX_NON) {
				csi_buf = &csi_obj->csi_buf[i];
				break;
			}
		}
	break;
	case HAL_CSI_BUF_SIZE_160:
		for (i = 0; i < e_idx_su; i++) {
			*sub_id = __query_avl_buf_idx_160(&csi_obj->csi_buf[i]);
			if(*sub_id != CSI_BUF_SUB_IDX_NON) {
				csi_buf = &csi_obj->csi_buf[i];
				break;
			}
		}
	break;
	default:
	break;
	}
	if (*sub_id == CSI_BUF_SUB_IDX_NON) {
		PHL_TRACE(COMP_PHL_DBG, _PHL_INFO_, "warring :_query_csi_buf_su fail !!!\n");
	}
	return csi_buf;
}

/**
 * _query_csi_buf_mu
 * 	Get available sub 40MHz csi buffer
 * input :
 * @hal_info: (struct hal_info_t *)
 * @size:enum hal_csi_buf_size 20/40/80 MHz
 * return
 * @sub_id: (u8) available csi buffer sub index
 * @csi_buf: (struct hal_csi_buf *) original hal csi buffer
 **/
struct hal_csi_buf *_query_csi_buf_mu(
	struct hal_info_t *hal_info,
	enum hal_csi_buf_size size,
	u8 *sub_id)
{
	struct rtw_hal_com_t *hal_com = hal_info->hal_com;
	struct hal_csi_obj *csi_obj = (struct hal_csi_obj *)hal_com->csi_obj;
	struct hal_csi_buf *csi_buf = NULL;
	u8 i = 0;
	u8 s_idx_mu = csi_obj->max_csi_buf_nr_su;
	u8 e_idx_mu = csi_obj->max_csi_buf_nr;

	*sub_id = CSI_BUF_SUB_IDX_NON;

	switch (size) {
	case HAL_CSI_BUF_SIZE_20:
		for (i = s_idx_mu; i < e_idx_mu; i++) {
			*sub_id = __query_avl_buf_idx_20(&csi_obj->csi_buf[i]);
			if(*sub_id != CSI_BUF_SUB_IDX_NON) {
				csi_buf = &csi_obj->csi_buf[i];
				break;
			}
		}
	break;
	case HAL_CSI_BUF_SIZE_40:
		for (i = s_idx_mu; i < e_idx_mu; i++) {
			*sub_id = __query_avl_buf_idx_40(&csi_obj->csi_buf[i]);
			if(*sub_id != CSI_BUF_SUB_IDX_NON) {
				csi_buf = &csi_obj->csi_buf[i];
				break;
			}
		}
	break;
	case HAL_CSI_BUF_SIZE_80:
		for (i = s_idx_mu; i < e_idx_mu; i++) {
			*sub_id = __query_avl_buf_idx_80(&csi_obj->csi_buf[i]);
			if(*sub_id != CSI_BUF_SUB_IDX_NON) {
				csi_buf = &csi_obj->csi_buf[i];
				break;
			}
		}
	break;
	case HAL_CSI_BUF_SIZE_160:
		for (i = s_idx_mu; i < e_idx_mu; i++) {
			*sub_id = __query_avl_buf_idx_160(&csi_obj->csi_buf[i]);
			if(*sub_id != CSI_BUF_SUB_IDX_NON) {
				csi_buf = &csi_obj->csi_buf[i];
				break;
			}
		}
	break;
	default:
	break;
	}

	if (*sub_id == CSI_BUF_SUB_IDX_NON) {
		PHL_TRACE(COMP_PHL_DBG, _PHL_INFO_, "warring :_query_csi_buf_mu fail !!!\n");
	}
	return csi_buf;
}

/**
 * _bw2csi
 * 	phl channel width to hal csi buffer size
 * input :
 * @bw: (struct hal_info_t *)
 * return
 * @ret: enum hal_csi_buf_size
 **/
enum hal_csi_buf_size
_bw2csi(enum channel_width bw)
{
	enum hal_csi_buf_size ret = HAL_CSI_BUF_SIZE_NONE;
	switch (bw){
	case CHANNEL_WIDTH_20:
		ret = HAL_CSI_BUF_SIZE_20;
	break;
	case CHANNEL_WIDTH_40:
		ret = HAL_CSI_BUF_SIZE_40;
	break;
	case CHANNEL_WIDTH_80:
		ret = HAL_CSI_BUF_SIZE_80;
	break;
	case CHANNEL_WIDTH_160:
		ret = HAL_CSI_BUF_SIZE_160;
	break;
	default:
	break;
	}
	return ret;
}

enum rtw_hal_status _hal_csi_init_buf(struct hal_info_t *hal_info, u8 num)
{
	enum rtw_hal_status status = RTW_HAL_STATUS_SUCCESS;
	void *drv_priv = hal_to_drvpriv(hal_info);
	struct rtw_hal_com_t *hal_com = hal_info->hal_com;
	struct hal_csi_obj *csi_obj = hal_com->csi_obj;
	struct hal_csi_buf *csi_buf = NULL;
	u8 idx = 0;

	do {
		csi_obj->csi_buf = _os_mem_alloc(drv_priv,
					sizeof(*csi_buf) * num);
		_os_mem_set(drv_priv, csi_obj->csi_buf, 0,
					sizeof(*csi_buf) * num);

		if (NULL == csi_obj->csi_buf) {
			status = RTW_HAL_STATUS_RESOURCE;
			break;
		}
		csi_buf = csi_obj->csi_buf;
		for (idx = 0; idx < num ; idx++) {
			csi_buf[idx].idx = idx;
			csi_buf[idx].sub_idx = 0;
		}
	} while (0);

	return status;
}


/**
 * hal_csi_init
 * 	hal csi module initialize
 * input :
 * @hal_info: struct hal_info_t
 * @su_buf_nr: Number of HW SU CSI Buffer for HAL
 * @mu_buf_nr: Number of HW MU CSI Buffer for HAL
 **/
enum rtw_hal_status
hal_csi_init(struct hal_info_t *hal_info, u8 su_buf_nr, u8 mu_buf_nr)
{
	enum rtw_hal_status status = RTW_HAL_STATUS_SUCCESS;
	struct rtw_hal_com_t *hal_com = hal_info->hal_com;
	struct hal_csi_obj *csi_obj = NULL;
	void *drv_priv = hal_to_drvpriv(hal_info);
	u8 idx = 0;
	FUNCIN();

	do {
		csi_obj = _os_mem_alloc(drv_priv, sizeof(*csi_obj));

		if (csi_obj == NULL) {
			status = RTW_HAL_STATUS_RESOURCE;
			break;
		}
		hal_com->csi_obj = csi_obj;
		_os_spinlock_init(drv_priv, &csi_obj->csi_lock);

		if ((su_buf_nr + mu_buf_nr) == 0) {
			PHL_ERR("hal_csi_init fail : csi buffer number = 0\n");
			status = RTW_HAL_STATUS_RESOURCE;
			break;
		}
		_hal_csi_init_buf(hal_info, mu_buf_nr + su_buf_nr);

		csi_obj->max_csi_buf_nr_mu = mu_buf_nr;
		csi_obj->max_csi_buf_nr_su = su_buf_nr;
		csi_obj->max_csi_buf_nr = su_buf_nr + mu_buf_nr;
		for (idx = su_buf_nr; idx < csi_obj->max_csi_buf_nr; idx++){
			csi_obj->csi_buf[idx].type = HAL_CSI_BUF_TYPE_MU;
		}

	} while (0);

	if (RTW_HAL_STATUS_SUCCESS != status) {
		hal_csi_deinit(hal_info);
	}
	FUNCOUT();
	return status;
}
/**
 * hal_csi_deinit
 * 	hal csi module deinitialize
 * input :
 * @hal_info: struct hal_info_t
 **/
void hal_csi_deinit(struct hal_info_t *hal_info)
{
	struct rtw_hal_com_t *hal_com = hal_info->hal_com;
	struct hal_csi_obj *csi_obj = (struct hal_csi_obj *)hal_com->csi_obj;
	void *drv_priv = hal_to_drvpriv(hal_info);
	struct hal_csi_buf *csi_buf = csi_obj->csi_buf;

	if (csi_obj != NULL) {
		if (csi_buf != NULL) {
			_os_mem_free(hal_to_drvpriv(hal_info), csi_buf,
			sizeof(struct hal_csi_buf) * csi_obj->max_csi_buf_nr);
			csi_obj->csi_buf = NULL;
		}

		_os_spinlock_free(drv_priv, &csi_obj->csi_lock);
		/* bf obj need free as last */
		_os_mem_free(hal_to_drvpriv(hal_info), csi_obj,
					sizeof(struct hal_csi_obj));
		hal_com->csi_obj = NULL;
	}
}

/**
 * hal_csi_query_idle_csi_buf
 * 	Get available CSI buffer
 * input :
 * @hal_info: struct hal_info_t
 * @mu: Request CSI Type : 1 = MU / 0 = SU
 * @bw: Request CSI BW : enum channel_width (20/40/80MHz)
 * @buf: (struct hal_csi_buf *)input/return : memory for CSI buffer index+subindex
 * 	 (the call shall provied 1byte memory to save csi index)
 **/
enum rtw_hal_status hal_csi_query_idle_csi_buf(
	struct hal_info_t *hal_info,
	u8 mu,
	enum channel_width bw,
	void *buf)
{
	enum rtw_hal_status status = RTW_HAL_STATUS_FAILURE;
	struct rtw_hal_com_t *hal_com = hal_info->hal_com;
	struct hal_csi_obj *csi_obj = (struct hal_csi_obj *)hal_com->csi_obj;
	void *drv_priv = hal_to_drvpriv(hal_info);
	struct hal_csi_buf *csi_buf = (struct hal_csi_buf *)buf;
	struct hal_csi_buf *tmp_csi_buf = NULL;
	u8 sub_idx = CSI_BUF_SUB_IDX_NON;
	enum hal_csi_buf_size size = HAL_CSI_BUF_SIZE_NONE;
	enum hal_csi_buf_type type = HAL_CSI_BUF_TYPE_SU;

	FUNCIN();
	if (csi_buf == NULL) {
		return status;
	}

	size = _bw2csi(bw);
	type = mu ? HAL_CSI_BUF_TYPE_MU : HAL_CSI_BUF_TYPE_SU;
	PHL_TRACE(COMP_PHL_SOUND, _PHL_INFO_, "size = 0x%x , type = 0x%x \n",
		  size, type);

	switch (type) {
	case HAL_CSI_BUF_TYPE_SU:
	{
		_os_spinlock(drv_priv, &csi_obj->csi_lock, _ps, NULL);
		tmp_csi_buf = _query_csi_buf_su(hal_info, size, &sub_idx);
		_os_spinunlock(drv_priv, &csi_obj->csi_lock, _ps, NULL);
	}
	break;
	case HAL_CSI_BUF_TYPE_MU:
	{
		_os_spinlock(drv_priv, &csi_obj->csi_lock, _ps, NULL);
		tmp_csi_buf = _query_csi_buf_mu(hal_info, size, &sub_idx);
		_os_spinunlock(drv_priv, &csi_obj->csi_lock, _ps, NULL);
	}
	break;
	default:
	break;
	}
	if (tmp_csi_buf == NULL || sub_idx == CSI_BUF_SUB_IDX_NON) {
		status = RTW_HAL_STATUS_RESOURCE;
		PHL_TRACE(COMP_PHL_DBG, _PHL_INFO_, "hal_csi_query_idle_csi_buf Fail\n");
	} else {
		csi_buf->idx = tmp_csi_buf->idx;
		csi_buf->sub_idx = sub_idx;
		csi_buf->type = type;
		if (bw == CHANNEL_WIDTH_160)
			csi_buf->b160mhz = 1;
		else
			csi_buf->b160mhz = 0;
		status = RTW_HAL_STATUS_SUCCESS;
	}

	FUNCOUT();
	return status;
}

/**
 * hal_csi_release_csi_buf
 * 	Release the CSI buffer
 * input :
 * @hal_info: struct hal_info_t
 * @buf: (struct hal_csi_buf *)input/return : CSI buffer index to release
 **/
enum rtw_hal_status hal_csi_release_csi_buf(
	struct hal_info_t *hal_info,
	void *buf)
{
	struct rtw_hal_com_t *hal_com = hal_info->hal_com;
	struct hal_csi_obj *csi_obj = (struct hal_csi_obj *)hal_com->csi_obj;
	void *drv_priv = hal_to_drvpriv(hal_info);
	enum rtw_hal_status status = RTW_HAL_STATUS_FAILURE;
	struct hal_csi_buf *tmp_csi_buf = NULL;
	struct hal_csi_buf *csi_buf = (struct hal_csi_buf *)buf;

	if (csi_buf == NULL) {
		return status;
	}

	if (csi_buf->idx < csi_obj->max_csi_buf_nr) {
		tmp_csi_buf = &csi_obj->csi_buf[csi_buf->idx];

		_os_spinlock(drv_priv, &csi_obj->csi_lock, _ps, NULL);

		switch (csi_buf->sub_idx) {
		case CSI_BUF_SUB_IDX_FULL_BW:
			tmp_csi_buf->sub_idx = 0;
		break;
		case CSI_BUF_SUB_IDX_40_U:
			CLEAR_CSI_STS_BIT(tmp_csi_buf, 3);
			CLEAR_CSI_STS_BIT(tmp_csi_buf, 2);
		break;
		case CSI_BUF_SUB_IDX_40_L:
			CLEAR_CSI_STS_BIT(tmp_csi_buf, 1);
			CLEAR_CSI_STS_BIT(tmp_csi_buf, 0);
		break;
		case CSI_BUF_SUB_IDX_20_UU:
			CLEAR_CSI_STS_BIT(tmp_csi_buf, 3);
		break;
		case CSI_BUF_SUB_IDX_20_UL:
			CLEAR_CSI_STS_BIT(tmp_csi_buf, 2);
		break;
		case CSI_BUF_SUB_IDX_20_LU:
			CLEAR_CSI_STS_BIT(tmp_csi_buf, 1);
		break;
		case CSI_BUF_SUB_IDX_20_LL:
			CLEAR_CSI_STS_BIT(tmp_csi_buf, 0);
		break;
		default:
		break;
		}
		csi_buf->idx = 0;
		csi_buf->sub_idx = CSI_BUF_SUB_IDX_NON;
		_os_spinunlock(drv_priv, &csi_obj->csi_lock, _ps, NULL);
		status = RTW_HAL_STATUS_SUCCESS;
	}
	return status;
}

/**
 * hal_is_csi_buf_valid : check the csi buffer index is valid or not.
 * @hal_info: struct hal_info_t
 * @buf: (struct hal_csi_buf *)input/return : CSI buffer index to release
 **/
u8 hal_is_csi_buf_valid(struct hal_info_t *hal_info, void *buf)
{
	struct hal_csi_buf *csi_buf = (struct hal_csi_buf *)buf;
	struct rtw_hal_com_t *hal_com = hal_info->hal_com;
	struct hal_csi_obj *csi_obj = (struct hal_csi_obj *)hal_com->csi_obj;

	u8 ret = _SUCCESS;
	do {
		if(csi_buf->idx >= csi_obj->max_csi_buf_nr){
			ret = _FAIL;
			break;
		}
		if(csi_buf->sub_idx == CSI_BUF_SUB_IDX_NON) {
			ret = _FAIL;
			break;
		}
	} while(0);

	return ret;
}

/**
 * rtw_hal_get_csi_buf_bw : Get the csi buf bandwidth;
 * @buf: (struct hal_csi_buf *)
 * return
 * @ret: enum channel_width
 **/
enum channel_width
rtw_hal_get_csi_buf_bw(void *buf)
{
	struct hal_csi_buf *csi_buf = (struct hal_csi_buf *)buf;
	enum channel_width bw = CHANNEL_WIDTH_20;
	switch (csi_buf->sub_idx) {
	case CSI_BUF_SUB_IDX_FULL_BW:
		if (csi_buf->b160mhz)
			bw = CHANNEL_WIDTH_160;
		else
			bw = CHANNEL_WIDTH_80;
	break;
	case CSI_BUF_SUB_IDX_40_U:
	case CSI_BUF_SUB_IDX_40_L:
		bw = CHANNEL_WIDTH_40;
	break;
	case CSI_BUF_SUB_IDX_20_UU:
	case CSI_BUF_SUB_IDX_20_UL:
	case CSI_BUF_SUB_IDX_20_LU:
	case CSI_BUF_SUB_IDX_20_LL:
		bw = CHANNEL_WIDTH_20;
	break;
	case CSI_BUF_SUB_IDX_NON:
	default:
		bw = CHANNEL_WIDTH_MAX;
		break;
	}

	return bw;
}


/**
 * rtw_hal_get_csi_buf_type : check the csi buffer index is valid or not.
 * @buf: (struct hal_csi_buf *)
 * return:
 * @ret: true = TYPE MU ; false = TYPE SU
 **/
bool
rtw_hal_get_csi_buf_type(void *buf)
{
	struct hal_csi_buf *csi_buf = (struct hal_csi_buf *)buf;
	bool ret = false;

	if (csi_buf != NULL) {
		ret = (csi_buf->type == HAL_CSI_BUF_TYPE_MU) ? true : false;
	}

	return ret;
}

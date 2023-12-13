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
#define _HAL_TEST_MP_REG_C_
#include "../../hal_headers.h"
#include "../../../test/mp/phl_test_mp_def.h"

#ifdef CONFIG_HAL_TEST_MP
enum rtw_hal_status rtw_hal_mp_reg_read_macreg(
	struct mp_context *mp, struct mp_reg_arg *arg)
{
	struct hal_info_t *hal_info = (struct hal_info_t *)mp->hal;
	struct hal_ops_t *hal_ops = hal_get_ops(hal_info);
	u32 convert_mask = 0x0;

	PHL_INFO("%s: io_type = %d\n", __FUNCTION__, arg->io_type);
	PHL_INFO("%s: io_offset = %d\n", __FUNCTION__, arg->io_offset);

	switch(arg->io_type){
	case 1:
		convert_mask = 0x000000ff;
		break;
	case 2:
		convert_mask = 0x0000ffff;
		break;
	case 4:
		convert_mask = 0xffffffff;
		break;
	default:
		PHL_INFO("io_type error\n");
		return RTW_HAL_STATUS_FAILURE;
	}

	arg->io_value = hal_ops->read_macreg(mp->hal,
					    arg->io_offset,
					    convert_mask);
	PHL_INFO("%s: io_value = %x\n", __FUNCTION__, arg->io_value);

	return RTW_HAL_STATUS_SUCCESS;
}

enum rtw_hal_status rtw_hal_mp_reg_write_macreg(
	struct mp_context *mp, struct mp_reg_arg *arg)
{
	struct hal_info_t *hal_info = (struct hal_info_t *)mp->hal;
	struct hal_ops_t *hal_ops = hal_get_ops(hal_info);
	u32 convert_mask = 0x0;

	PHL_INFO("%s: io_type = %d\n", __FUNCTION__, arg->io_type);
	PHL_INFO("%s: io_offset = %d\n", __FUNCTION__, arg->io_offset);
	PHL_INFO("%s: io_value = %x\n", __FUNCTION__, arg->io_value);

	switch(arg->io_type){
	case 1:
		convert_mask = 0x000000ff;
		break;
	case 2:
		convert_mask = 0x0000ffff;
		break;
	case 4:
		convert_mask = 0xffffffff;
		break;
	default:
		PHL_INFO("io_type error\n");
		return RTW_HAL_STATUS_FAILURE;
	}

	hal_ops->write_macreg(mp->hal, arg->io_offset,
			     convert_mask, arg->io_value);

	return RTW_HAL_STATUS_SUCCESS;
}

enum rtw_hal_status rtw_hal_mp_reg_read_rfreg(
	struct mp_context *mp, struct mp_reg_arg *arg)
{
	struct hal_info_t *hal_info = (struct hal_info_t *)mp->hal;
	struct hal_ops_t *hal_ops = hal_get_ops(hal_info);
	u32 convert_mask = 0x0;

	PHL_INFO("%s: rfpath = %d\n", __FUNCTION__, arg->rfpath);
	PHL_INFO("%s: io_type = %d\n", __FUNCTION__, arg->io_type);
	PHL_INFO("%s: io_offset = %x\n", __FUNCTION__, arg->io_offset);

	switch(arg->io_type){
	case 1:
		convert_mask = 0x000000ff;
		break;
	case 2:
		convert_mask = 0x0000ffff;
		break;
	case 4:
		convert_mask = 0xffffffff;
		break;
	default:
		PHL_INFO("io_type error\n");
		return RTW_HAL_STATUS_FAILURE;
	}

	arg->io_value = hal_ops->read_rfreg(mp->hal, arg->rfpath,
					   arg->io_offset, convert_mask);
	PHL_INFO("%s: io_value = %x\n", __FUNCTION__, arg->io_value);

	return RTW_HAL_STATUS_SUCCESS;
}

enum rtw_hal_status rtw_hal_mp_reg_write_rfreg(
	struct mp_context *mp, struct mp_reg_arg *arg)
{
	struct hal_info_t *hal_info = (struct hal_info_t *)mp->hal;
	struct hal_ops_t *hal_ops = hal_get_ops(hal_info);
	u32 convert_mask = 0x0;

	PHL_INFO("%s: rfpath = %d\n", __FUNCTION__, arg->rfpath);
	PHL_INFO("%s: io_type = %d\n", __FUNCTION__, arg->io_type);
	PHL_INFO("%s: io_offset = %d\n", __FUNCTION__, arg->io_offset);
	PHL_INFO("%s: io_value = %x\n", __FUNCTION__, arg->io_value);

	switch(arg->io_type){
	case 1:
		convert_mask = 0x000000ff;
		break;
	case 2:
		convert_mask = 0x0000ffff;
		break;
	case 4:
		convert_mask = 0xffffffff;
		break;
	default:
		PHL_INFO("io_type error\n");
		return RTW_HAL_STATUS_FAILURE;
	}

	hal_ops->write_rfreg(mp->hal, arg->rfpath, arg->io_offset,
			    convert_mask, arg->io_value);

	return RTW_HAL_STATUS_SUCCESS;
}

enum rtw_hal_status rtw_hal_mp_reg_read_rf_syn(
	struct mp_context *mp, struct mp_reg_arg *arg)
{
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_FAILURE;

	PHL_INFO("%s: io_type = %d\n", __FUNCTION__, arg->io_type);
	PHL_INFO("%s: io_offset = %d\n", __FUNCTION__, arg->io_offset);

	PHL_INFO("[MP] call hal api: not ready\n");
	hal_status = RTW_HAL_STATUS_SUCCESS;
	/*
	PHL_INFO("%s: io_value = %x\n", __FUNCTION__, arg->io_value);
	*/

	return hal_status;
}

enum rtw_hal_status rtw_hal_mp_reg_write_rf_syn(
	struct mp_context *mp, struct mp_reg_arg *arg)
{
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_FAILURE;

	PHL_INFO("%s: io_type = %d\n", __FUNCTION__, arg->io_type);
	PHL_INFO("%s: io_offset = %d\n", __FUNCTION__, arg->io_offset);
	PHL_INFO("%s: io_value = %x\n", __FUNCTION__, arg->io_value);

	PHL_INFO("[MP] call hal api: not ready\n");
	hal_status = RTW_HAL_STATUS_SUCCESS;

	return hal_status;
}

enum rtw_hal_status rtw_hal_mp_reg_read_bbreg(
	struct mp_context *mp, struct mp_reg_arg *arg)
{
	struct hal_info_t *hal_info = (struct hal_info_t *)mp->hal;
	struct hal_ops_t *hal_ops = hal_get_ops(hal_info);
	u32 convert_mask = 0x0;

	PHL_INFO("%s: io_type = %d\n", __FUNCTION__, arg->io_type);
	PHL_INFO("%s: io_offset = %d\n", __FUNCTION__, arg->io_offset);

	switch(arg->io_type){
	case 1:
		convert_mask = 0x000000ff;
		break;
	case 2:
		convert_mask = 0x0000ffff;
		break;
	case 4:
		convert_mask = 0xffffffff;
		break;
	default:
		PHL_INFO("io_type error\n");
		return RTW_HAL_STATUS_FAILURE;
	}

	arg->io_value = hal_ops->read_bbreg(mp->hal,
					   arg->io_offset,
					   convert_mask);
	PHL_INFO("%s: io_value = %x\n", __FUNCTION__, arg->io_value);

	return RTW_HAL_STATUS_SUCCESS;
}

enum rtw_hal_status rtw_hal_mp_reg_write_bbreg(
	struct mp_context *mp, struct mp_reg_arg *arg)
{
	struct hal_info_t *hal_info = (struct hal_info_t *)mp->hal;
	struct hal_ops_t *hal_ops = hal_get_ops(hal_info);
	u32 convert_mask = 0x0;

	PHL_INFO("%s: io_type = %d\n", __FUNCTION__, arg->io_type);
	PHL_INFO("%s: io_offset = %d\n", __FUNCTION__, arg->io_offset);
	PHL_INFO("%s: io_value = %x\n", __FUNCTION__, arg->io_value);

	switch(arg->io_type){
	case 1:
		convert_mask = 0x000000ff;
		break;
	case 2:
		convert_mask = 0x0000ffff;
		break;
	case 4:
		convert_mask = 0xffffffff;
		break;
	default:
		PHL_INFO("io_type error\n");
		return RTW_HAL_STATUS_FAILURE;
	}

	hal_ops->write_bbreg(mp->hal, arg->io_offset,
			    convert_mask, arg->io_value);

	return RTW_HAL_STATUS_SUCCESS;
}

enum rtw_hal_status rtw_hal_mp_reg_get_xcap(
	struct mp_context *mp, struct mp_reg_arg *arg)
{
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_FAILURE;
	struct hal_info_t *hal_info = (struct hal_info_t *)mp->hal;

	PHL_INFO("%s\n", __FUNCTION__);

	hal_status = rtw_hal_mac_get_xcap(hal_info->hal_com, arg->sc_xo,
									  &arg->io_value);
	PHL_INFO("%s: status = %d\n", __FUNCTION__, hal_status);
	PHL_INFO("%s: xcap capability = %d\n", __FUNCTION__, arg->io_value);

	return hal_status;
}

enum rtw_hal_status rtw_hal_mp_reg_set_xcap(
	struct mp_context *mp, struct mp_reg_arg *arg)
{
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_FAILURE;
	struct hal_info_t *hal_info = (struct hal_info_t *)mp->hal;

	PHL_INFO("%s: xcap capability = %d\n", __FUNCTION__, arg->io_value);

	hal_status = rtw_hal_mac_set_xcap(hal_info->hal_com, arg->sc_xo,
									  arg->io_value);
	PHL_INFO("%s: status = %d\n", __FUNCTION__, hal_status);

	return hal_status;
}

enum rtw_hal_status rtw_hal_mp_reg_get_xsi(
	struct mp_context *mp, struct mp_reg_arg *arg)
{
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_FAILURE;
	struct hal_info_t *hal_info = (struct hal_info_t *)mp->hal;

	PHL_INFO("%s\n", __FUNCTION__);

	hal_status = rtw_hal_mac_get_xsi(hal_info->hal_com,
					arg->xsi_offset,
					&arg->xsi_value);
	PHL_INFO("%s: status = %d\n", __FUNCTION__, hal_status);
	PHL_INFO("%s: xsi = %d\n", __FUNCTION__, arg->io_value);

	return hal_status;
}

enum rtw_hal_status rtw_hal_mp_reg_set_xsi(
	struct mp_context *mp, struct mp_reg_arg *arg)
{
	enum rtw_hal_status hal_status = RTW_HAL_STATUS_FAILURE;
	struct hal_info_t *hal_info = (struct hal_info_t *)mp->hal;

	PHL_INFO("%s: xsi = %d\n", __FUNCTION__, arg->io_value);

	hal_status = rtw_hal_mac_set_xsi(hal_info->hal_com,
					arg->xsi_offset,
					arg->xsi_value);
	PHL_INFO("%s: status = %d\n", __FUNCTION__, hal_status);

	return hal_status;
}
#endif /* CONFIG_HAL_TEST_MP */

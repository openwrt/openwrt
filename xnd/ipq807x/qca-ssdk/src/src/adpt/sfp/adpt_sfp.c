/*
 * Copyright (c) 2019, The Linux Foundation. All rights reserved.
 * Permission to use, copy, modify, and/or distribute this software for
 * any purpose with or without fee is hereby granted, provided that the
 * above copyright notice and this permission notice appear in all copies.
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT
 * OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */


/**
 * @defgroup
 * @{
 */
#include "sw.h"
#include "adpt.h"
#include "sfp_reg.h"
#include "sfp.h"
#include "hsl_phy.h"


#define ADPT_RTN_ON_INVALID_DATA_OFFSET(offset) \
    do { if (offset > 0xff) return(SW_BAD_PARAM); } while(0);

#define ADPT_PORT_I2C_CAP_CHECK(dev_id, port_id) \
	do { if (PHY_I2C_ACCESS != hsl_port_phy_access_type_get(dev_id, port_id)) \
		return(SW_NOT_SUPPORTED); } while(0);

sw_error_t
adpt_sfp_diag_ctrl_status_get(a_uint32_t dev_id, a_uint32_t port_id,
		fal_sfp_ctrl_status_t *ctrl_status)
{
	sw_error_t rv = SW_OK;
	union sfp_diag_optional_ctrl_status_u sfp_diag_optional_ctrl_status;
	union sfp_diag_extended_ctrl_status_u sfp_diag_extended_ctrl_status;

	ADPT_DEV_ID_CHECK(dev_id);
	ADPT_PORT_I2C_CAP_CHECK(dev_id, port_id);
	ADPT_NULL_POINT_CHECK(ctrl_status);

	memset(&sfp_diag_optional_ctrl_status, 0, sizeof(sfp_diag_optional_ctrl_status));
	memset(&sfp_diag_extended_ctrl_status, 0, sizeof(sfp_diag_extended_ctrl_status));

	rv = sfp_diag_optional_ctrl_status_get(dev_id,
			port_id, &sfp_diag_optional_ctrl_status);
	SW_RTN_ON_ERROR(rv);

	rv = sfp_diag_extended_ctrl_status_get(dev_id,
			port_id, &sfp_diag_extended_ctrl_status);
	SW_RTN_ON_ERROR(rv);

	ctrl_status->data_ready = sfp_diag_optional_ctrl_status.bf.data_ready;
	ctrl_status->rx_los = sfp_diag_optional_ctrl_status.bf.rx_los;
	ctrl_status->tx_fault = sfp_diag_optional_ctrl_status.bf.tx_fault;
	ctrl_status->soft_rate_sel = sfp_diag_optional_ctrl_status.bf.soft_rate_sel;
	ctrl_status->rate_sel = sfp_diag_optional_ctrl_status.bf.rate_sel;
	ctrl_status->rs_state = sfp_diag_optional_ctrl_status.bf.rs;
	ctrl_status->soft_tx_disable = sfp_diag_optional_ctrl_status.bf.soft_tx_disable_sel;
	ctrl_status->tx_disable = sfp_diag_optional_ctrl_status.bf.tx_disable;

	ctrl_status->pwr_level_sel = sfp_diag_extended_ctrl_status.bf.pwr_level_sel;
	ctrl_status->pwr_level_op_state = sfp_diag_extended_ctrl_status.bf.pwr_level_op_state;
	ctrl_status->soft_rs_sel = sfp_diag_extended_ctrl_status.bf.soft_rs_sel;

	return SW_OK;
}

sw_error_t
adpt_sfp_diag_extenal_calibration_const_get(a_uint32_t dev_id, a_uint32_t port_id,
		fal_sfp_cal_const_t *cal_const)
{
	sw_error_t rv = SW_OK;
	union sfp_diag_cal_const_u sfp_diag_cal_const;

	ADPT_DEV_ID_CHECK(dev_id);
	ADPT_PORT_I2C_CAP_CHECK(dev_id, port_id);
	ADPT_NULL_POINT_CHECK(cal_const);

	memset(&sfp_diag_cal_const, 0, sizeof(sfp_diag_cal_const));

	rv = sfp_diag_cal_const_get(dev_id, port_id, &sfp_diag_cal_const);
	SW_RTN_ON_ERROR(rv);

	cal_const->rx_power4 = sfp_diag_cal_const.bf.rx_pwr_4_0 << 24
		| sfp_diag_cal_const.bf.rx_pwr_4_1 << 16
		| sfp_diag_cal_const.bf.rx_pwr_4_2 << 8
		| sfp_diag_cal_const.bf.rx_pwr_4_3;
	cal_const->rx_power3 = sfp_diag_cal_const.bf.rx_pwr_3_0 << 24
		| sfp_diag_cal_const.bf.rx_pwr_3_1 << 16
		| sfp_diag_cal_const.bf.rx_pwr_3_2 << 8
		| sfp_diag_cal_const.bf.rx_pwr_3_3;
	cal_const->rx_power2 = sfp_diag_cal_const.bf.rx_pwr_2_0 << 24
		| sfp_diag_cal_const.bf.rx_pwr_2_1 << 16
		| sfp_diag_cal_const.bf.rx_pwr_2_2 << 8
		| sfp_diag_cal_const.bf.rx_pwr_2_3;
	cal_const->rx_power1 = sfp_diag_cal_const.bf.rx_pwr_1_0 << 24
		| sfp_diag_cal_const.bf.rx_pwr_1_1 << 16
		| sfp_diag_cal_const.bf.rx_pwr_1_2 << 8
		| sfp_diag_cal_const.bf.rx_pwr_1_3;
	cal_const->rx_power0 = sfp_diag_cal_const.bf.rx_pwr_0_0 << 24
		| sfp_diag_cal_const.bf.rx_pwr_0_1 << 16
		| sfp_diag_cal_const.bf.rx_pwr_0_2 << 8
		| sfp_diag_cal_const.bf.rx_pwr_0_3;

	cal_const->tx_bias_slope = sfp_diag_cal_const.bf.tx_i_slope_0 << 8
		| sfp_diag_cal_const.bf.tx_i_slope_1;
	cal_const->tx_bias_offset = sfp_diag_cal_const.bf.tx_i_offset_0 << 8
		| sfp_diag_cal_const.bf.tx_i_offset_1;

	cal_const->tx_power_slope = sfp_diag_cal_const.bf.tx_pwr_slope_0 << 8
		| sfp_diag_cal_const.bf.tx_pwr_slope_1;
	cal_const->tx_power_offset = sfp_diag_cal_const.bf.tx_pwr_offset_0 << 8
		| sfp_diag_cal_const.bf.tx_pwr_offset_1;

	cal_const->temp_slope = sfp_diag_cal_const.bf.t_slope_0 << 8
		| sfp_diag_cal_const.bf.t_slope_1;
	cal_const->temp_offset = sfp_diag_cal_const.bf.t_offset_0 << 8
		| sfp_diag_cal_const.bf.t_offset_1;

	cal_const->vol_slope = sfp_diag_cal_const.bf.v_slope_0 << 8
		| sfp_diag_cal_const.bf.v_slope_1;
	cal_const->vol_offset = sfp_diag_cal_const.bf.v_offset_0 << 8
		| sfp_diag_cal_const.bf.v_offset_1;

	return SW_OK;
}

sw_error_t
adpt_sfp_link_length_get(a_uint32_t dev_id, a_uint32_t port_id,
		fal_sfp_link_length_t *link_len)
{
	sw_error_t rv = SW_OK;
	union sfp_link_len_u sfp_link_len;

	ADPT_DEV_ID_CHECK(dev_id);
	ADPT_PORT_I2C_CAP_CHECK(dev_id, port_id);
	ADPT_NULL_POINT_CHECK(link_len);

	memset(&sfp_link_len, 0, sizeof(sfp_link_len));

	rv = sfp_link_len_get(dev_id, port_id, &sfp_link_len);
	SW_RTN_ON_ERROR(rv);

	link_len->single_mode_length_km = sfp_link_len.bf.single_mode_km;
	link_len->single_mode_length_100m = sfp_link_len.bf.single_mode_100m;
	link_len->om2_mode_length_10m = sfp_link_len.bf.om2_mode_10m;
	link_len->om1_mode_length_10m = sfp_link_len.bf.om1_mode_10m;
	link_len->copper_mode_length_1m = sfp_link_len.bf.copper_mode_1m;
	link_len->om3_mode_length_1m = sfp_link_len.bf.om3_mode_1m;

	return SW_OK;
}

sw_error_t
adpt_sfp_diag_internal_threshold_get(a_uint32_t dev_id, a_uint32_t port_id,
		fal_sfp_internal_threshold_t *threshold)
{
	sw_error_t rv = SW_OK;
	union sfp_diag_threshold_u sfp_diag_threshold;

	ADPT_DEV_ID_CHECK(dev_id);
	ADPT_PORT_I2C_CAP_CHECK(dev_id, port_id);
	ADPT_NULL_POINT_CHECK(threshold);

	memset(&sfp_diag_threshold, 0, sizeof(sfp_diag_threshold));

	rv = sfp_diag_threshold_get(dev_id, port_id, &sfp_diag_threshold);
	SW_RTN_ON_ERROR(rv);

	threshold->temp_high_alarm = sfp_diag_threshold.bf.temp_high_alarm_0 << 8
		| sfp_diag_threshold.bf.temp_high_alarm_1;
	threshold->temp_low_alarm = sfp_diag_threshold.bf.temp_low_alarm_0 << 8
		| sfp_diag_threshold.bf.temp_low_alarm_1;
	threshold->temp_high_warning = sfp_diag_threshold.bf.temp_high_warning_0 << 8
		| sfp_diag_threshold.bf.temp_high_warning_1;
	threshold->temp_low_warning = sfp_diag_threshold.bf.temp_low_warning_0 << 8
		| sfp_diag_threshold.bf.temp_low_warning_1;
	threshold->vol_high_alarm = sfp_diag_threshold.bf.vol_high_alarm_0 << 8
		| sfp_diag_threshold.bf.vol_high_alarm_1;
	threshold->vol_low_alarm = sfp_diag_threshold.bf.vol_low_alarm_0 << 8
		| sfp_diag_threshold.bf.vol_low_alarm_1;
	threshold->vol_high_warning = sfp_diag_threshold.bf.vol_high_warning_0 << 8
		| sfp_diag_threshold.bf.vol_high_warning_1;
	threshold->vol_low_warning = sfp_diag_threshold.bf.vol_low_warning_0 << 8
		| sfp_diag_threshold.bf.vol_low_warning_1;
	threshold->bias_high_alarm = sfp_diag_threshold.bf.bias_high_alarm_0 << 8
		| sfp_diag_threshold.bf.bias_high_alarm_1;
	threshold->bias_low_alarm = sfp_diag_threshold.bf.bias_low_alarm_0 << 8
		| sfp_diag_threshold.bf.bias_low_alarm_1;
	threshold->bias_high_warning = sfp_diag_threshold.bf.bias_high_warning_0 << 8
		| sfp_diag_threshold.bf.bias_high_warning_1;
	threshold->bias_low_warning = sfp_diag_threshold.bf.bias_low_warning_0 << 8
		| sfp_diag_threshold.bf.bias_low_warning_1;
	threshold->tx_power_high_alarm = sfp_diag_threshold.bf.tx_pwr_high_alarm_0 << 8
		| sfp_diag_threshold.bf.tx_pwr_high_alarm_1;
	threshold->tx_power_low_alarm = sfp_diag_threshold.bf.tx_pwr_low_alarm_0 << 8
		| sfp_diag_threshold.bf.tx_pwr_low_alarm_1;
	threshold->tx_power_high_warning = sfp_diag_threshold.bf.tx_pwr_high_warning_0 << 8
		| sfp_diag_threshold.bf.tx_pwr_high_warning_1;
	threshold->tx_power_low_warning = sfp_diag_threshold.bf.tx_pwr_low_warning_0 << 8
		| sfp_diag_threshold.bf.tx_pwr_low_warning_1;
	threshold->rx_power_high_alarm = sfp_diag_threshold.bf.rx_pwr_high_alarm_0 << 8
		| sfp_diag_threshold.bf.rx_pwr_high_alarm_1;
	threshold->rx_power_low_alarm = sfp_diag_threshold.bf.rx_pwr_low_alarm_0 << 8
		| sfp_diag_threshold.bf.rx_pwr_low_alarm_1;
	threshold->rx_power_high_warning = sfp_diag_threshold.bf.rx_pwr_high_warning_0 << 8
		| sfp_diag_threshold.bf.rx_pwr_high_warning_1;
	threshold->rx_power_low_warning = sfp_diag_threshold.bf.rx_pwr_low_warning_0 << 8
		| sfp_diag_threshold.bf.rx_pwr_low_warning_1;

	return SW_OK;
}

sw_error_t
adpt_sfp_diag_realtime_get(a_uint32_t dev_id, a_uint32_t port_id,
		fal_sfp_realtime_diag_t *real_diag)
{
	sw_error_t rv = SW_OK;
	union sfp_diag_realtime_u sfp_diag_realtime;

	ADPT_DEV_ID_CHECK(dev_id);
	ADPT_PORT_I2C_CAP_CHECK(dev_id, port_id);
	ADPT_NULL_POINT_CHECK(real_diag);

	memset(&sfp_diag_realtime, 0, sizeof(sfp_diag_realtime));

	rv = sfp_diag_realtime_get(dev_id, port_id, &sfp_diag_realtime);
	SW_RTN_ON_ERROR(rv);

	real_diag->cur_temp = sfp_diag_realtime.bf.tmp_0 << 8
		| sfp_diag_realtime.bf.tmp_1;
	real_diag->cur_vol = sfp_diag_realtime.bf.vcc_0 << 8
		| sfp_diag_realtime.bf.vcc_1;
	real_diag->tx_cur_bias = sfp_diag_realtime.bf.tx_bias_0 << 8
		| sfp_diag_realtime.bf.tx_bias_1;
	real_diag->tx_cur_power = sfp_diag_realtime.bf.tx_pwr_0 << 8
		| sfp_diag_realtime.bf.tx_pwr_1;
	real_diag->rx_cur_power = sfp_diag_realtime.bf.rx_pwr_0 << 8
		| sfp_diag_realtime.bf.rx_pwr_1;

	return SW_OK;
}

sw_error_t
adpt_sfp_laser_wavelength_get(a_uint32_t dev_id, a_uint32_t port_id,
		fal_sfp_laser_wavelength_t *laser_wavelen)
{
	sw_error_t rv = SW_OK;
	union sfp_laser_u sfp_laser;

	ADPT_DEV_ID_CHECK(dev_id);
	ADPT_PORT_I2C_CAP_CHECK(dev_id, port_id);
	ADPT_NULL_POINT_CHECK(laser_wavelen);

	memset(&sfp_laser, 0, sizeof(sfp_laser));

	rv = sfp_laser_get(dev_id, port_id, &sfp_laser);
	SW_RTN_ON_ERROR(rv);

	laser_wavelen->laser_wavelength = sfp_laser.bf.wavelength_0 << 8
		| sfp_laser.bf.wavelength_1;

	return SW_OK;
}

sw_error_t
adpt_sfp_option_get(a_uint32_t dev_id, a_uint32_t port_id,
		fal_sfp_option_t *option)
{
	sw_error_t rv = SW_OK;
	union sfp_option_u sfp_option;

	ADPT_DEV_ID_CHECK(dev_id);
	ADPT_PORT_I2C_CAP_CHECK(dev_id, port_id);
	ADPT_NULL_POINT_CHECK(option);

	memset(&sfp_option, 0, sizeof(sfp_option));

	rv = sfp_option_get(dev_id, port_id, &sfp_option);
	SW_RTN_ON_ERROR(rv);

	option->linear_recv_output = sfp_option.bf.linear_recv_output;
	option->pwr_level_declar = sfp_option.bf.pwr_level_declar;
	option->cool_transc_declar = sfp_option.bf.cool_transc_declar;
	option->loss_signal = sfp_option.bf.loss_signal;
	option->loss_invert_signal = sfp_option.bf.loss_invert_signal;
	option->tx_fault_signal = sfp_option.bf.tx_fault_signal;
	option->tx_disable = sfp_option.bf.tx_disable;
	option->rate_sel = sfp_option.bf.rate_sel;

	return SW_OK;
}

sw_error_t
adpt_sfp_checkcode_get(a_uint32_t dev_id, a_uint32_t port_id,
		fal_sfp_cc_type_t cc_type, a_uint8_t *ccode)
{
	sw_error_t rv = SW_OK;
	union sfp_base_u sfp_base;
	union sfp_ext_u sfp_ext;
	union sfp_diag_dmi_u sfp_dmi;

	ADPT_DEV_ID_CHECK(dev_id);
	ADPT_PORT_I2C_CAP_CHECK(dev_id, port_id);
	ADPT_NULL_POINT_CHECK(ccode);

	memset(&sfp_base, 0, sizeof(sfp_base));
	memset(&sfp_ext, 0, sizeof(sfp_ext));
	memset(&sfp_dmi, 0, sizeof(sfp_dmi));

	switch (cc_type) {
		case FAL_SFP_CC_BASE:
			rv = sfp_base_get(dev_id, port_id, &sfp_base);
			SW_RTN_ON_ERROR(rv);
			*ccode = sfp_base.bf.check_code;
			break;
		case FAL_SFP_CC_EXT:
			rv = sfp_ext_get(dev_id, port_id, &sfp_ext);
			SW_RTN_ON_ERROR(rv);
			*ccode = sfp_ext.bf.check_code;
			break;
		case FAL_SFP_CC_DMI:
			rv = sfp_diag_dmi_get(dev_id,
					port_id, &sfp_dmi);
			SW_RTN_ON_ERROR(rv);
			*ccode = sfp_dmi.bf.check_code;
			break;
		default:
			return SW_BAD_PARAM;
	}

	return SW_OK;
}

sw_error_t
adpt_sfp_diag_alarm_warning_flag_get(a_uint32_t dev_id, a_uint32_t port_id,
		fal_sfp_alarm_warn_flag_t *alarm_warn_flag)
{
	sw_error_t rv = SW_OK;
	union sfp_diag_flag_u sfp_diag_flag;

	ADPT_DEV_ID_CHECK(dev_id);
	ADPT_PORT_I2C_CAP_CHECK(dev_id, port_id);
	ADPT_NULL_POINT_CHECK(alarm_warn_flag);

	memset(&sfp_diag_flag, 0, sizeof(sfp_diag_flag));

	rv = sfp_diag_flag_get(dev_id, port_id, &sfp_diag_flag);
	SW_RTN_ON_ERROR(rv);

	alarm_warn_flag->tx_pwr_low_alarm = sfp_diag_flag.bf.tx_pwr_low_alarm;
	alarm_warn_flag->tx_pwr_high_alarm = sfp_diag_flag.bf.tx_pwr_high_alarm;
	alarm_warn_flag->tx_bias_low_alarm = sfp_diag_flag.bf.tx_bias_low_alarm;
	alarm_warn_flag->tx_bias_high_alarm = sfp_diag_flag.bf.tx_bias_high_alarm;
	alarm_warn_flag->vcc_low_alarm = sfp_diag_flag.bf.vcc_low_alarm;
	alarm_warn_flag->vcc_high_alarm = sfp_diag_flag.bf.vcc_high_alarm;
	alarm_warn_flag->tmp_low_alarm = sfp_diag_flag.bf.tmp_low_alarm;
	alarm_warn_flag->tmp_high_alarm = sfp_diag_flag.bf.tmp_high_alarm;
	alarm_warn_flag->rx_pwr_low_alarm = sfp_diag_flag.bf.rx_pwr_low_alarm;
	alarm_warn_flag->rx_pwr_high_alarm = sfp_diag_flag.bf.rx_pwr_high_alarm;

	alarm_warn_flag->tx_pwr_low_warning = sfp_diag_flag.bf.tx_pwr_low_warning;
	alarm_warn_flag->tx_pwr_high_warning = sfp_diag_flag.bf.tx_pwr_high_warning;
	alarm_warn_flag->tx_bias_low_warning = sfp_diag_flag.bf.tx_bias_low_warning;
	alarm_warn_flag->tx_bias_high_warning = sfp_diag_flag.bf.tx_bias_high_warning;
	alarm_warn_flag->vcc_low_warning = sfp_diag_flag.bf.vcc_low_warning;
	alarm_warn_flag->vcc_high_warning = sfp_diag_flag.bf.vcc_high_warning;
	alarm_warn_flag->tmp_low_warning = sfp_diag_flag.bf.tmp_low_warning;
	alarm_warn_flag->tmp_high_warning = sfp_diag_flag.bf.tmp_high_warning;
	alarm_warn_flag->rx_pwr_low_warning = sfp_diag_flag.bf.rx_pwr_low_warning;
	alarm_warn_flag->rx_pwr_high_warning = sfp_diag_flag.bf.rx_pwr_high_warning;

	return SW_OK;
}

sw_error_t
adpt_sfp_device_type_get(a_uint32_t dev_id, a_uint32_t port_id,
		fal_sfp_dev_type_t *sfp_id)
{
	sw_error_t rv = SW_OK;
	union sfp_dev_type_u sfp_dev_type;
	union sfp_dev_type_ext_u sfp_dev_type_ext;
	union sfp_dev_connector_type_u sfp_dev_connector_type;

	ADPT_DEV_ID_CHECK(dev_id);
	ADPT_PORT_I2C_CAP_CHECK(dev_id, port_id);
	ADPT_NULL_POINT_CHECK(sfp_id);

	memset(&sfp_dev_type, 0, sizeof(sfp_dev_type));
	memset(&sfp_dev_type_ext, 0, sizeof(sfp_dev_type_ext));
	memset(&sfp_dev_connector_type, 0, sizeof(sfp_dev_connector_type));

	rv = sfp_dev_type_get(dev_id, port_id, &sfp_dev_type);
	SW_RTN_ON_ERROR(rv);

	rv = sfp_dev_type_ext_get(dev_id, port_id, &sfp_dev_type_ext);
	SW_RTN_ON_ERROR(rv);

	rv = sfp_dev_connector_type_get(dev_id, port_id, &sfp_dev_connector_type);
	SW_RTN_ON_ERROR(rv);

	sfp_id->identifier = sfp_dev_type.bf.id;
	sfp_id->ext_indentifier = sfp_dev_type_ext.bf.id;
	sfp_id->connector_type = sfp_dev_connector_type.bf.code;

	return SW_OK;
}

sw_error_t
adpt_sfp_vendor_info_get(a_uint32_t dev_id, a_uint32_t port_id,
		fal_sfp_vendor_info_t *vender_info)
{
	sw_error_t rv = SW_OK;
	a_uint32_t index, i;
	union sfp_vendor_u sfp_vendor;
	union sfp_vendor_ext_u sfp_vendor_ext;

	ADPT_DEV_ID_CHECK(dev_id);
	ADPT_PORT_I2C_CAP_CHECK(dev_id, port_id);
	ADPT_NULL_POINT_CHECK(vender_info);

	memset(&sfp_vendor, 0, sizeof(sfp_vendor));
	memset(&sfp_vendor_ext, 0, sizeof(sfp_vendor_ext));

	rv = sfp_vendor_get(dev_id, port_id, &sfp_vendor);
	SW_RTN_ON_ERROR(rv);

	rv = sfp_vendor_ext_get(dev_id, port_id, &sfp_vendor_ext);
	SW_RTN_ON_ERROR(rv);

	/* vendor basic info */
	for (index = 0, i = 0; i < sizeof(vender_info->vendor_name); index++) {
		vender_info->vendor_name[i++] = *(sfp_vendor.val + index);
	}

	/* skip Transceiver Code for electronic or optical compatibility */
	index++;

	for (i = 0; i < sizeof(vender_info->vendor_oui); index++) {
		vender_info->vendor_oui[i++] = *(sfp_vendor.val + index);
	}

	for (i = 0; i < sizeof(vender_info->vendor_pn); index++) {
		vender_info->vendor_pn[i++] = *(sfp_vendor.val + index);
	}

	for (i = 0; i < sizeof(vender_info->vendor_rev); index++) {
		vender_info->vendor_rev[i++] = *(sfp_vendor.val + index);
	}

	/* vendor extended info */
	for (index = 0, i = 0; i < sizeof(vender_info->vendor_sn); index++) {
		vender_info->vendor_sn[i++] = *(sfp_vendor_ext.val + index);
	}

	for (i = 0; i < sizeof(vender_info->vendor_date_code); index++) {
		vender_info->vendor_date_code[i++] = *(sfp_vendor_ext.val + index);
	}

	return SW_OK;
}

sw_error_t
adpt_sfp_transceiver_code_get(a_uint32_t dev_id, a_uint32_t port_id,
		fal_sfp_transc_code_t *transc_code)
{
	sw_error_t rv = SW_OK;
	union sfp_transc_u sfp_transc;

	ADPT_DEV_ID_CHECK(dev_id);
	ADPT_PORT_I2C_CAP_CHECK(dev_id, port_id);
	ADPT_NULL_POINT_CHECK(transc_code);

	memset(&sfp_transc, 0, sizeof(sfp_transc));

	rv = sfp_transc_get(dev_id, port_id, &sfp_transc);
	SW_RTN_ON_ERROR(rv);

	transc_code->eth_10g_ccode = sfp_transc.bf.eth_10g_ccode;
	transc_code->infiniband_ccode = sfp_transc.bf.infiniband_ccode;
	transc_code->escon_ccode = sfp_transc.bf.escon_ccode;
	transc_code->sonet_ccode = sfp_transc.bf.sonet_ccode_1 << 8
		| sfp_transc.bf.sonet_ccode_2;
	transc_code->eth_ccode = sfp_transc.bf.eth_ccode;
	transc_code->fibre_chan_link_length = sfp_transc.bf.fiber_ch_link_len;
	transc_code->fibre_chan_tech = sfp_transc.bf.fiber_ch_tech_1 << 4
		| sfp_transc.bf.fiber_ch_tech_2;
	transc_code->sfp_cable_tech = sfp_transc.bf.cable_tech;
	transc_code->fibre_chan_trans_md = sfp_transc.bf.fiber_chan_tm_media;
	transc_code->fibre_chan_speed = sfp_transc.bf.fiber_ch_speed;

	return SW_OK;
}

sw_error_t
adpt_sfp_ctrl_rate_get(a_uint32_t dev_id, a_uint32_t port_id,
		fal_sfp_rate_t *rate_limit)
{
	sw_error_t rv = SW_OK;
	union sfp_rate_ctrl_u sfp_rate_ctrl;

	ADPT_DEV_ID_CHECK(dev_id);
	ADPT_PORT_I2C_CAP_CHECK(dev_id, port_id);
	ADPT_NULL_POINT_CHECK(rate_limit);

	memset(&sfp_rate_ctrl, 0, sizeof(sfp_rate_ctrl));

	rv = sfp_rate_ctrl_get(dev_id, port_id, &sfp_rate_ctrl);
	SW_RTN_ON_ERROR(rv);

	rate_limit->upper_rate_limit = sfp_rate_ctrl.bf.upper;
	rate_limit->lower_rate_limit = sfp_rate_ctrl.bf.lower;

	return SW_OK;
}

sw_error_t
adpt_sfp_enhanced_cfg_get(a_uint32_t dev_id, a_uint32_t port_id,
		fal_sfp_enhanced_cfg_t *enhanced_feature)
{
	sw_error_t rv = SW_OK;
	union sfp_enhanced_u sfp_enhanced;

	ADPT_DEV_ID_CHECK(dev_id);
	ADPT_PORT_I2C_CAP_CHECK(dev_id, port_id);
	ADPT_NULL_POINT_CHECK(enhanced_feature);

	memset(&sfp_enhanced, 0, sizeof(sfp_enhanced));

	rv = sfp_enhanced_get(dev_id, port_id, &sfp_enhanced);
	SW_RTN_ON_ERROR(rv);

	enhanced_feature->addr_mode = sfp_enhanced.bf.addr_mode;
	enhanced_feature->rec_pwr_type = sfp_enhanced.bf.re_pwr_type;
	enhanced_feature->external_cal = sfp_enhanced.bf.external_cal;
	enhanced_feature->internal_cal = sfp_enhanced.bf.internal_cal;
	enhanced_feature->diag_mon_flag = sfp_enhanced.bf.diag_mon_flag;
	enhanced_feature->legacy_type = sfp_enhanced.bf.legacy_type;

	enhanced_feature->soft_rate_sel_op = sfp_enhanced.bf.soft_rate_sel_op;
	enhanced_feature->app_sel_op = sfp_enhanced.bf.app_sel_op;
	enhanced_feature->soft_rate_ctrl_op = sfp_enhanced.bf.soft_rate_ctrl_op;
	enhanced_feature->rx_los_op = sfp_enhanced.bf.rx_los_op;
	enhanced_feature->tx_fault_op = sfp_enhanced.bf.tx_fault_op;
	enhanced_feature-> tx_disable_ctrl_op = sfp_enhanced.bf.tx_disable_ctrl_op;
	enhanced_feature->alarm_warning_flag_op = sfp_enhanced.bf.alarm_warning_flag_op;

	enhanced_feature->compliance_feature = sfp_enhanced.bf.cmpl_feature;

	return SW_OK;
}

sw_error_t
adpt_sfp_rate_encode_get(a_uint32_t dev_id, a_uint32_t port_id,
		fal_sfp_rate_encode_t *encode)
{
	sw_error_t rv = SW_OK;
	union sfp_encoding_u sfp_encoding;
	union sfp_br_u sfp_br;
	union sfp_rate_u sfp_rate;

	ADPT_DEV_ID_CHECK(dev_id);
	ADPT_PORT_I2C_CAP_CHECK(dev_id, port_id);
	ADPT_NULL_POINT_CHECK(encode);

	memset(&sfp_encoding, 0, sizeof(sfp_encoding));
	memset(&sfp_br, 0, sizeof(sfp_br));
	memset(&sfp_rate, 0, sizeof(sfp_rate));

	rv = sfp_encoding_get(dev_id, port_id, &sfp_encoding);
	SW_RTN_ON_ERROR(rv);

	rv = sfp_br_get(dev_id, port_id, &sfp_br);
	SW_RTN_ON_ERROR(rv);

	rv = sfp_rate_get(dev_id, port_id, &sfp_rate);
	SW_RTN_ON_ERROR(rv);

	encode->encode = sfp_encoding.bf.code;
	encode->nominal_bit_rate = sfp_br.bf.bit;
	encode->rate_id = sfp_rate.bf.id;

	return SW_OK;
}

sw_error_t
adpt_sfp_eeprom_data_get(a_uint32_t dev_id, a_uint32_t port_id, fal_sfp_data_t *entry)
{
	sw_error_t rv = SW_OK;

	ADPT_DEV_ID_CHECK(dev_id);
	ADPT_PORT_I2C_CAP_CHECK(dev_id, port_id);
	ADPT_NULL_POINT_CHECK(entry);

	ADPT_RTN_ON_INVALID_DATA_OFFSET(entry->offset + entry->count);

	rv = sfp_eeprom_data_get(dev_id, port_id, entry->addr,
			entry->offset, entry->data, entry->count);
	SW_RTN_ON_ERROR(rv);

	return SW_OK;
}

sw_error_t
adpt_sfp_eeprom_data_set(a_uint32_t dev_id, a_uint32_t port_id, fal_sfp_data_t *entry)
{
	sw_error_t rv = SW_OK;

	ADPT_DEV_ID_CHECK(dev_id);
	ADPT_PORT_I2C_CAP_CHECK(dev_id, port_id);
	ADPT_NULL_POINT_CHECK(entry);

	ADPT_RTN_ON_INVALID_DATA_OFFSET(entry->offset + entry->count);

	rv = sfp_eeprom_data_set(dev_id, port_id, entry->addr,
			entry->offset, entry->data, entry->count);
	SW_RTN_ON_ERROR(rv);

	/* retrieve the data */
	rv = sfp_eeprom_data_get(dev_id, port_id, entry->addr,
			entry->offset, entry->data, entry->count);
	SW_RTN_ON_ERROR(rv);

	return SW_OK;
}

sw_error_t adpt_sfp_init(a_uint32_t dev_id)
{
	adpt_api_t *p_adpt_api = NULL;

	p_adpt_api = adpt_api_ptr_get(dev_id);

	if(p_adpt_api == NULL)
		return SW_FAIL;

	p_adpt_api->adpt_sfp_diag_ctrl_status_get = adpt_sfp_diag_ctrl_status_get;
	p_adpt_api->adpt_sfp_diag_extenal_calibration_const_get =
		adpt_sfp_diag_extenal_calibration_const_get;
	p_adpt_api->adpt_sfp_link_length_get = adpt_sfp_link_length_get;
	p_adpt_api->adpt_sfp_diag_internal_threshold_get =
		adpt_sfp_diag_internal_threshold_get;
	p_adpt_api->adpt_sfp_diag_realtime_get = adpt_sfp_diag_realtime_get;
	p_adpt_api->adpt_sfp_laser_wavelength_get = adpt_sfp_laser_wavelength_get;
	p_adpt_api->adpt_sfp_option_get = adpt_sfp_option_get;
	p_adpt_api->adpt_sfp_checkcode_get = adpt_sfp_checkcode_get;
	p_adpt_api->adpt_sfp_diag_alarm_warning_flag_get =
		adpt_sfp_diag_alarm_warning_flag_get;
	p_adpt_api->adpt_sfp_device_type_get = adpt_sfp_device_type_get;
	p_adpt_api->adpt_sfp_vendor_info_get = adpt_sfp_vendor_info_get;
	p_adpt_api->adpt_sfp_transceiver_code_get = adpt_sfp_transceiver_code_get;
	p_adpt_api->adpt_sfp_ctrl_rate_get = adpt_sfp_ctrl_rate_get;
	p_adpt_api->adpt_sfp_enhanced_cfg_get = adpt_sfp_enhanced_cfg_get;
	p_adpt_api->adpt_sfp_rate_encode_get = adpt_sfp_rate_encode_get;
	p_adpt_api->adpt_sfp_eeprom_data_get = adpt_sfp_eeprom_data_get;
	p_adpt_api->adpt_sfp_eeprom_data_set = adpt_sfp_eeprom_data_set;


	return SW_OK;
}

/**
 * @}
 */

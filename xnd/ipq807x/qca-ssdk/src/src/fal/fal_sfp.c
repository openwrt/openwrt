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

#include "sw.h"
#include "fal_sfp.h"
#include "adpt.h"
#include "hsl_api.h"

sw_error_t
_fal_sfp_diag_ctrl_status_get(a_uint32_t dev_id, a_uint32_t port_id,
				fal_sfp_ctrl_status_t *ctrl_status)
{
	adpt_api_t *p_api;
	sw_error_t rv = SW_OK;

	SW_RTN_ON_NULL(p_api = adpt_api_ptr_get(dev_id));

	if (NULL == p_api->adpt_sfp_diag_ctrl_status_get)
		return SW_NOT_SUPPORTED;

	rv = p_api->adpt_sfp_diag_ctrl_status_get(dev_id, port_id, ctrl_status);
	return rv;
}

sw_error_t
_fal_sfp_diag_extenal_calibration_const_get(a_uint32_t dev_id, a_uint32_t port_id,
				fal_sfp_cal_const_t *cal_const)
{
	adpt_api_t *p_api;
	sw_error_t rv = SW_OK;

	SW_RTN_ON_NULL(p_api = adpt_api_ptr_get(dev_id));

	if (NULL == p_api->adpt_sfp_diag_extenal_calibration_const_get)
		return SW_NOT_SUPPORTED;

	rv = p_api->adpt_sfp_diag_extenal_calibration_const_get(dev_id, port_id, cal_const);
	return rv;
}

sw_error_t
_fal_sfp_link_length_get(a_uint32_t dev_id, a_uint32_t port_id,
				fal_sfp_link_length_t *link_len)
{
	adpt_api_t *p_api;
	sw_error_t rv = SW_OK;

	SW_RTN_ON_NULL(p_api = adpt_api_ptr_get(dev_id));

	if (NULL == p_api->adpt_sfp_link_length_get)
		return SW_NOT_SUPPORTED;

	rv = p_api->adpt_sfp_link_length_get(dev_id, port_id, link_len);
	return rv;
}

sw_error_t
_fal_sfp_diag_internal_threshold_get(a_uint32_t dev_id, a_uint32_t port_id,
				fal_sfp_internal_threshold_t *threshold)
{
	adpt_api_t *p_api;
	sw_error_t rv = SW_OK;

	SW_RTN_ON_NULL(p_api = adpt_api_ptr_get(dev_id));

	if (NULL == p_api->adpt_sfp_diag_internal_threshold_get)
		return SW_NOT_SUPPORTED;

	rv = p_api->adpt_sfp_diag_internal_threshold_get(dev_id, port_id, threshold);
	return rv;
}

sw_error_t
_fal_sfp_diag_realtime_get(a_uint32_t dev_id, a_uint32_t port_id,
				fal_sfp_realtime_diag_t *real_diag)
{
	adpt_api_t *p_api;
	sw_error_t rv = SW_OK;

	SW_RTN_ON_NULL(p_api = adpt_api_ptr_get(dev_id));

	if (NULL == p_api->adpt_sfp_diag_realtime_get)
		return SW_NOT_SUPPORTED;

	rv = p_api->adpt_sfp_diag_realtime_get(dev_id, port_id, real_diag);
	return rv;
}

sw_error_t
_fal_sfp_laser_wavelength_get(a_uint32_t dev_id, a_uint32_t port_id,
				fal_sfp_laser_wavelength_t *laser_wavelen)
{
	adpt_api_t *p_api;
	sw_error_t rv = SW_OK;

	SW_RTN_ON_NULL(p_api = adpt_api_ptr_get(dev_id));

	if (NULL == p_api->adpt_sfp_laser_wavelength_get)
		return SW_NOT_SUPPORTED;

	rv = p_api->adpt_sfp_laser_wavelength_get(dev_id, port_id, laser_wavelen);
	return rv;
}

sw_error_t
_fal_sfp_option_get(a_uint32_t dev_id, a_uint32_t port_id,
				fal_sfp_option_t *option)
{
	adpt_api_t *p_api;
	sw_error_t rv = SW_OK;

	SW_RTN_ON_NULL(p_api = adpt_api_ptr_get(dev_id));

	if (NULL == p_api->adpt_sfp_option_get)
		return SW_NOT_SUPPORTED;

	rv = p_api->adpt_sfp_option_get(dev_id, port_id, option);
	return rv;
}

sw_error_t
_fal_sfp_checkcode_get(a_uint32_t dev_id, a_uint32_t port_id,
				fal_sfp_cc_type_t cc_type, a_uint8_t *ccode)
{
	adpt_api_t *p_api;
	sw_error_t rv = SW_OK;

	SW_RTN_ON_NULL(p_api = adpt_api_ptr_get(dev_id));

	if (NULL == p_api->adpt_sfp_checkcode_get)
		return SW_NOT_SUPPORTED;

	rv = p_api->adpt_sfp_checkcode_get(dev_id, port_id, cc_type, ccode);
	return rv;
}

sw_error_t
_fal_sfp_diag_alarm_warning_flag_get(a_uint32_t dev_id, a_uint32_t port_id,
				fal_sfp_alarm_warn_flag_t *alarm_warn_flag)
{
	adpt_api_t *p_api;
	sw_error_t rv = SW_OK;

	SW_RTN_ON_NULL(p_api = adpt_api_ptr_get(dev_id));

	if (NULL == p_api->adpt_sfp_diag_alarm_warning_flag_get)
		return SW_NOT_SUPPORTED;

	rv = p_api->adpt_sfp_diag_alarm_warning_flag_get(dev_id, port_id, alarm_warn_flag);
	return rv;
}

sw_error_t
_fal_sfp_device_type_get(a_uint32_t dev_id, a_uint32_t port_id,
				fal_sfp_dev_type_t *sfp_id)
{
	adpt_api_t *p_api;
	sw_error_t rv = SW_OK;

	SW_RTN_ON_NULL(p_api = adpt_api_ptr_get(dev_id));

	if (NULL == p_api->adpt_sfp_device_type_get)
		return SW_NOT_SUPPORTED;

	rv = p_api->adpt_sfp_device_type_get(dev_id, port_id, sfp_id);
	return rv;
}

sw_error_t
_fal_sfp_vendor_info_get(a_uint32_t dev_id, a_uint32_t port_id,
				fal_sfp_vendor_info_t *vender_info)
{
	adpt_api_t *p_api;
	sw_error_t rv = SW_OK;

	SW_RTN_ON_NULL(p_api = adpt_api_ptr_get(dev_id));

	if (NULL == p_api->adpt_sfp_vendor_info_get)
		return SW_NOT_SUPPORTED;

	rv = p_api->adpt_sfp_vendor_info_get(dev_id, port_id, vender_info);
	return rv;
}

sw_error_t
_fal_sfp_transceiver_code_get(a_uint32_t dev_id, a_uint32_t port_id,
				fal_sfp_transc_code_t *transc_code)
{
	adpt_api_t *p_api;
	sw_error_t rv = SW_OK;

	SW_RTN_ON_NULL(p_api = adpt_api_ptr_get(dev_id));

	if (NULL == p_api->adpt_sfp_transceiver_code_get)
		return SW_NOT_SUPPORTED;

	rv = p_api->adpt_sfp_transceiver_code_get(dev_id, port_id, transc_code);
	return rv;
}

sw_error_t
_fal_sfp_ctrl_rate_get(a_uint32_t dev_id, a_uint32_t port_id,
				fal_sfp_rate_t *rate_limit)
{
	adpt_api_t *p_api;
	sw_error_t rv = SW_OK;

	SW_RTN_ON_NULL(p_api = adpt_api_ptr_get(dev_id));

	if (NULL == p_api->adpt_sfp_ctrl_rate_get)
		return SW_NOT_SUPPORTED;

	rv = p_api->adpt_sfp_ctrl_rate_get(dev_id, port_id, rate_limit);
	return rv;
}

sw_error_t
_fal_sfp_enhanced_cfg_get(a_uint32_t dev_id, a_uint32_t port_id,
				fal_sfp_enhanced_cfg_t *enhanced_feature)
{
	adpt_api_t *p_api;
	sw_error_t rv = SW_OK;

	SW_RTN_ON_NULL(p_api = adpt_api_ptr_get(dev_id));

	if (NULL == p_api->adpt_sfp_enhanced_cfg_get)
		return SW_NOT_SUPPORTED;

	rv = p_api->adpt_sfp_enhanced_cfg_get(dev_id, port_id, enhanced_feature);
	return rv;
}

sw_error_t
_fal_sfp_rate_encode_get(a_uint32_t dev_id, a_uint32_t port_id,
				fal_sfp_rate_encode_t *encode)
{
	adpt_api_t *p_api;
	sw_error_t rv = SW_OK;

	SW_RTN_ON_NULL(p_api = adpt_api_ptr_get(dev_id));

	if (NULL == p_api->adpt_sfp_rate_encode_get)
		return SW_NOT_SUPPORTED;

	rv = p_api->adpt_sfp_rate_encode_get(dev_id, port_id, encode);
	return rv;
}

sw_error_t
_fal_sfp_eeprom_data_get(a_uint32_t dev_id, a_uint32_t port_id, fal_sfp_data_t *entry)
{
	adpt_api_t *p_api;
	sw_error_t rv = SW_OK;

	SW_RTN_ON_NULL(p_api = adpt_api_ptr_get(dev_id));

	if (NULL == p_api->adpt_sfp_eeprom_data_get)
		return SW_NOT_SUPPORTED;

	rv = p_api->adpt_sfp_eeprom_data_get(dev_id, port_id, entry);
	return rv;
}

sw_error_t
_fal_sfp_eeprom_data_set(a_uint32_t dev_id, a_uint32_t port_id, fal_sfp_data_t *entry)
{
	adpt_api_t *p_api;
	sw_error_t rv = SW_OK;

	SW_RTN_ON_NULL(p_api = adpt_api_ptr_get(dev_id));

	if (NULL == p_api->adpt_sfp_eeprom_data_set)
		return SW_NOT_SUPPORTED;

	rv = p_api->adpt_sfp_eeprom_data_set(dev_id, port_id, entry);
	return rv;
}

sw_error_t
fal_sfp_diag_ctrl_status_get(a_uint32_t dev_id, a_uint32_t port_id,
				fal_sfp_ctrl_status_t *ctrl_status)
{
	sw_error_t rv = SW_OK;

	FAL_API_LOCK;
	rv = _fal_sfp_diag_ctrl_status_get(dev_id, port_id, ctrl_status);
	FAL_API_UNLOCK;
	return rv;
}

sw_error_t
fal_sfp_diag_extenal_calibration_const_get(a_uint32_t dev_id, a_uint32_t port_id,
				fal_sfp_cal_const_t *cal_const)
{
	sw_error_t rv = SW_OK;

	FAL_API_LOCK;
	rv = _fal_sfp_diag_extenal_calibration_const_get(dev_id, port_id, cal_const);
	FAL_API_UNLOCK;
	return rv;
}

sw_error_t
fal_sfp_link_length_get(a_uint32_t dev_id, a_uint32_t port_id,
				fal_sfp_link_length_t *link_len)
{
	sw_error_t rv = SW_OK;

	FAL_API_LOCK;
	rv = _fal_sfp_link_length_get(dev_id, port_id, link_len);
	FAL_API_UNLOCK;
	return rv;
}

sw_error_t
fal_sfp_diag_internal_threshold_get(a_uint32_t dev_id, a_uint32_t port_id,
				fal_sfp_internal_threshold_t *threshold)
{
	sw_error_t rv = SW_OK;

	FAL_API_LOCK;
	rv = _fal_sfp_diag_internal_threshold_get(dev_id, port_id, threshold);
	FAL_API_UNLOCK;
	return rv;
}

sw_error_t
fal_sfp_diag_realtime_get(a_uint32_t dev_id, a_uint32_t port_id,
				fal_sfp_realtime_diag_t *real_diag)
{
	sw_error_t rv = SW_OK;

	FAL_API_LOCK;
	rv = _fal_sfp_diag_realtime_get(dev_id, port_id, real_diag);
	FAL_API_UNLOCK;
	return rv;
}

sw_error_t
fal_sfp_laser_wavelength_get(a_uint32_t dev_id, a_uint32_t port_id,
				fal_sfp_laser_wavelength_t *laser_wavelen)
{
	sw_error_t rv = SW_OK;

	FAL_API_LOCK;
	rv = _fal_sfp_laser_wavelength_get(dev_id, port_id, laser_wavelen);
	FAL_API_UNLOCK;
	return rv;
}

sw_error_t
fal_sfp_option_get(a_uint32_t dev_id, a_uint32_t port_id,
				fal_sfp_option_t *option)
{
	sw_error_t rv = SW_OK;

	FAL_API_LOCK;
	rv = _fal_sfp_option_get(dev_id, port_id, option);
	FAL_API_UNLOCK;
	return rv;
}

sw_error_t
fal_sfp_checkcode_get(a_uint32_t dev_id, a_uint32_t port_id,
				fal_sfp_cc_type_t cc_type, a_uint8_t *ccode)
{
	sw_error_t rv = SW_OK;

	FAL_API_LOCK;
	rv = _fal_sfp_checkcode_get(dev_id, port_id, cc_type, ccode);
	FAL_API_UNLOCK;
	return rv;
}

sw_error_t
fal_sfp_diag_alarm_warning_flag_get(a_uint32_t dev_id, a_uint32_t port_id,
				fal_sfp_alarm_warn_flag_t *alarm_warn_flag)
{
	sw_error_t rv = SW_OK;

	FAL_API_LOCK;
	rv = _fal_sfp_diag_alarm_warning_flag_get(dev_id, port_id, alarm_warn_flag);
	FAL_API_UNLOCK;
	return rv;
}

sw_error_t
fal_sfp_device_type_get(a_uint32_t dev_id, a_uint32_t port_id,
				fal_sfp_dev_type_t *sfp_id)
{
	sw_error_t rv = SW_OK;

	FAL_API_LOCK;
	rv = _fal_sfp_device_type_get(dev_id, port_id, sfp_id);
	FAL_API_UNLOCK;
	return rv;
}

sw_error_t
fal_sfp_vendor_info_get(a_uint32_t dev_id, a_uint32_t port_id,
				fal_sfp_vendor_info_t *vender_info)
{
	sw_error_t rv = SW_OK;

	FAL_API_LOCK;
	rv = _fal_sfp_vendor_info_get(dev_id, port_id, vender_info);
	FAL_API_UNLOCK;
	return rv;
}

sw_error_t
fal_sfp_transceiver_code_get(a_uint32_t dev_id, a_uint32_t port_id,
				fal_sfp_transc_code_t *transc_code)
{
	sw_error_t rv = SW_OK;

	FAL_API_LOCK;
	rv = _fal_sfp_transceiver_code_get(dev_id, port_id, transc_code);
	FAL_API_UNLOCK;
	return rv;
}

sw_error_t
fal_sfp_ctrl_rate_get(a_uint32_t dev_id, a_uint32_t port_id,
				fal_sfp_rate_t *rate_limit)
{
	sw_error_t rv = SW_OK;

	FAL_API_LOCK;
	rv = _fal_sfp_ctrl_rate_get(dev_id, port_id, rate_limit);
	FAL_API_UNLOCK;
	return rv;
}

sw_error_t
fal_sfp_enhanced_cfg_get(a_uint32_t dev_id, a_uint32_t port_id,
				fal_sfp_enhanced_cfg_t *enhanced_feature)
{
	sw_error_t rv = SW_OK;

	FAL_API_LOCK;
	rv = _fal_sfp_enhanced_cfg_get(dev_id, port_id, enhanced_feature);
	FAL_API_UNLOCK;
	return rv;
}

sw_error_t
fal_sfp_rate_encode_get(a_uint32_t dev_id, a_uint32_t port_id,
				fal_sfp_rate_encode_t *encode)
{
	sw_error_t rv = SW_OK;

	FAL_API_LOCK;
	rv = _fal_sfp_rate_encode_get(dev_id, port_id, encode);
	FAL_API_UNLOCK;
	return rv;
}

sw_error_t
fal_sfp_eeprom_data_get(a_uint32_t dev_id, a_uint32_t port_id, fal_sfp_data_t *entry)
{
	sw_error_t rv = SW_OK;

	FAL_API_LOCK;
	rv = _fal_sfp_eeprom_data_get(dev_id, port_id, entry);
	FAL_API_UNLOCK;
	return rv;
}

sw_error_t
fal_sfp_eeprom_data_set(a_uint32_t dev_id, a_uint32_t port_id, fal_sfp_data_t *entry)
{
	sw_error_t rv = SW_OK;

	FAL_API_LOCK;
	rv = _fal_sfp_eeprom_data_set(dev_id, port_id, entry);
	FAL_API_UNLOCK;
	return rv;
}

EXPORT_SYMBOL(fal_sfp_diag_ctrl_status_get);
EXPORT_SYMBOL(fal_sfp_diag_extenal_calibration_const_get);
EXPORT_SYMBOL(fal_sfp_link_length_get);
EXPORT_SYMBOL(fal_sfp_diag_internal_threshold_get);
EXPORT_SYMBOL(fal_sfp_diag_realtime_get);
EXPORT_SYMBOL(fal_sfp_laser_wavelength_get);
EXPORT_SYMBOL(fal_sfp_option_get);
EXPORT_SYMBOL(fal_sfp_checkcode_get);
EXPORT_SYMBOL(fal_sfp_diag_alarm_warning_flag_get);
EXPORT_SYMBOL(fal_sfp_device_type_get);
EXPORT_SYMBOL(fal_sfp_vendor_info_get);
EXPORT_SYMBOL(fal_sfp_transceiver_code_get);
EXPORT_SYMBOL(fal_sfp_ctrl_rate_get);
EXPORT_SYMBOL(fal_sfp_enhanced_cfg_get);
EXPORT_SYMBOL(fal_sfp_rate_encode_get);
EXPORT_SYMBOL(fal_sfp_eeprom_data_get);
EXPORT_SYMBOL(fal_sfp_eeprom_data_set);

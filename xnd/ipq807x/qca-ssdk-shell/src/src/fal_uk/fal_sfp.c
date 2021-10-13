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
#include "sw_ioctl.h"
#include "fal_sfp.h"
#include "fal_uk_if.h"

sw_error_t
fal_sfp_eeprom_data_get(a_uint32_t dev_id, a_uint32_t port_id, fal_sfp_data_t *entry)
{
	sw_error_t rv;

	rv = sw_uk_exec(SW_API_SFP_DATA_GET, dev_id, port_id, entry);
	return rv;
}

sw_error_t
fal_sfp_eeprom_data_set(a_uint32_t dev_id, a_uint32_t port_id, fal_sfp_data_t *entry)
{
	sw_error_t rv;

	rv = sw_uk_exec(SW_API_SFP_DATA_SET, dev_id, port_id, entry);

	return rv;
}

sw_error_t
fal_sfp_device_type_get(a_uint32_t dev_id, a_uint32_t port_id,
		fal_sfp_dev_type_t *sfp_id)
{
	sw_error_t rv;

	rv = sw_uk_exec(SW_API_SFP_DEV_TYPE_GET, dev_id, port_id,
			sfp_id);
	return rv;
}

sw_error_t
fal_sfp_transceiver_code_get(a_uint32_t dev_id, a_uint32_t port_id,
		fal_sfp_transc_code_t *transc_code)
{
	sw_error_t rv;

	rv = sw_uk_exec(SW_API_SFP_TRANSC_CODE_GET, dev_id, port_id,
			transc_code);
	return rv;
}

sw_error_t
fal_sfp_rate_encode_get(a_uint32_t dev_id, a_uint32_t port_id,
		fal_sfp_rate_encode_t *encode)
{
	sw_error_t rv;

	rv = sw_uk_exec(SW_API_SFP_RATE_ENCODE_GET, dev_id, port_id,
			encode);
	return rv;
}

sw_error_t
fal_sfp_link_length_get(a_uint32_t dev_id, a_uint32_t port_id,
		fal_sfp_link_length_t *link_len)
{
	sw_error_t rv;

	rv = sw_uk_exec(SW_API_SFP_LINK_LENGTH_GET, dev_id, port_id,
			link_len);
	return rv;
}

sw_error_t
fal_sfp_vendor_info_get(a_uint32_t dev_id, a_uint32_t port_id,
		fal_sfp_vendor_info_t *vender_info)
{
	sw_error_t rv;

	rv = sw_uk_exec(SW_API_SFP_VENDOR_INFO_GET, dev_id, port_id,
			vender_info);
	return rv;
}

sw_error_t
fal_sfp_laser_wavelength_get(a_uint32_t dev_id, a_uint32_t port_id,
		fal_sfp_laser_wavelength_t *laser_wavelen)
{
	sw_error_t rv;

	rv = sw_uk_exec(SW_API_SFP_LASER_WAVELENGTH_GET, dev_id, port_id,
			laser_wavelen);
	return rv;
}

sw_error_t
fal_sfp_option_get(a_uint32_t dev_id, a_uint32_t port_id,
		fal_sfp_option_t *option)
{
	sw_error_t rv;

	rv = sw_uk_exec(SW_API_SFP_OPTION_GET, dev_id, port_id,
			option);
	return rv;
}

sw_error_t
fal_sfp_ctrl_rate_get(a_uint32_t dev_id, a_uint32_t port_id,
		fal_sfp_rate_t *rate_limit)

{
	sw_error_t rv;

	rv = sw_uk_exec(SW_API_SFP_CTRL_RATE_GET, dev_id, port_id,
			rate_limit);
	return rv;
}
sw_error_t
fal_sfp_enhanced_cfg_get(a_uint32_t dev_id, a_uint32_t port_id,
		fal_sfp_enhanced_cfg_t *enhanced_feature)
{
	sw_error_t rv;

	rv = sw_uk_exec(SW_API_SFP_ENHANCED_CFG_GET, dev_id, port_id,
			enhanced_feature);
	return rv;
}

sw_error_t
fal_sfp_diag_internal_threshold_get(a_uint32_t dev_id, a_uint32_t port_id,
		fal_sfp_internal_threshold_t *threshold)
{
	sw_error_t rv;

	rv = sw_uk_exec(SW_API_SFP_DIAG_THRESHOLD_GET, dev_id, port_id,
			threshold);
	return rv;
}

sw_error_t
fal_sfp_diag_extenal_calibration_const_get(a_uint32_t dev_id, a_uint32_t port_id,
		fal_sfp_cal_const_t *cal_const)
{
	sw_error_t rv;

	rv = sw_uk_exec(SW_API_SFP_DIAG_CAL_CONST_GET, dev_id, port_id,
			cal_const);
	return rv;
}

sw_error_t
fal_sfp_diag_realtime_get(a_uint32_t dev_id, a_uint32_t port_id,
		fal_sfp_realtime_diag_t *real_diag)
{
	sw_error_t rv;

	rv = sw_uk_exec(SW_API_SFP_DIAG_REALTIME_GET, dev_id, port_id,
			real_diag);
	return rv;
}

sw_error_t
fal_sfp_diag_ctrl_status_get(a_uint32_t dev_id, a_uint32_t port_id,
		fal_sfp_ctrl_status_t *ctrl_status)
{
	sw_error_t rv;

	rv = sw_uk_exec(SW_API_SFP_DIAG_CTRL_STATUS_GET, dev_id, port_id,
			ctrl_status);
	return rv;
}

sw_error_t
fal_sfp_diag_alarm_warning_flag_get(a_uint32_t dev_id, a_uint32_t port_id,
		fal_sfp_alarm_warn_flag_t *alarm_warn_flag)
{
	sw_error_t rv;

	rv = sw_uk_exec(SW_API_SFP_DIAG_ALARM_WARN_FLAG_GET, dev_id, port_id,
			alarm_warn_flag);
	return rv;
}

sw_error_t
fal_sfp_checkcode_get(a_uint32_t dev_id, a_uint32_t port_id,
		fal_sfp_cc_type_t cc_type, a_uint8_t *ccode)
{
	sw_error_t rv;

	rv = sw_uk_exec(SW_API_SFP_CHECKCODE_GET, dev_id, port_id, (a_ulong_t)cc_type,
			ccode);
	return rv;
}

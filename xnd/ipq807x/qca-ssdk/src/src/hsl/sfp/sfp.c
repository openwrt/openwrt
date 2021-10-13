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
#include "hsl.h"
#include "sfp_access.h"
#include "sfp_reg.h"

sw_error_t
sfp_eeprom_data_get(a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t i2c_addr,
		a_uint32_t offset, a_uint8_t *buf, a_uint32_t count)
{
	return sfp_data_tbl_get(
			dev_id, index, i2c_addr,
			offset,
			buf, count);

}

sw_error_t
sfp_eeprom_data_set(a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t i2c_addr,
		a_uint32_t offset, a_uint8_t *buf, a_uint32_t count)
{
	return sfp_data_tbl_set(
			dev_id, index, i2c_addr,
			offset,
			buf, count);

}

sw_error_t
sfp_dev_type_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union sfp_dev_type_u *value)
{
	return sfp_data_get(
			dev_id, index, SFP_EEPROM_BASE_A0,
			SFP_BASE_ADDR + SFP_DEV_TYPE_ADDRESS,
			&value->val);
}

sw_error_t
sfp_dev_type_ext_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union sfp_dev_type_ext_u *value)
{
	return sfp_data_get(
			dev_id, index, SFP_EEPROM_BASE_A0,
			SFP_BASE_ADDR + SFP_DEV_TYPE_EXT_ADDRESS,
			&value->val);
}

sw_error_t
sfp_dev_connector_type_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union sfp_dev_connector_type_u *value)
{
	return sfp_data_get(
			dev_id, index, SFP_EEPROM_BASE_A0,
			SFP_BASE_ADDR + SFP_DEV_CONNECTOR_TYPE_ADDRESS,
			&value->val);
}

sw_error_t
sfp_transc_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union sfp_transc_u *value)
{
	return sfp_data_tbl_get(
			dev_id, index, SFP_EEPROM_BASE_A0,
			SFP_BASE_ADDR + SFP_TRANSC_ADDRESS,
			value->val, 8);
}

sw_error_t
sfp_encoding_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union sfp_encoding_u *value)
{
	return sfp_data_get(
			dev_id, index, SFP_EEPROM_BASE_A0,
			SFP_BASE_ADDR + SFP_ENCODING_ADDRESS,
			&value->val);
}

sw_error_t
sfp_br_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union sfp_br_u *value)
{
	return sfp_data_get(
			dev_id, index, SFP_EEPROM_BASE_A0,
			SFP_BASE_ADDR + SFP_BR_ADDRESS,
			&value->val);
}

sw_error_t
sfp_rate_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union sfp_rate_u *value)
{
	return sfp_data_get(
			dev_id, index, SFP_EEPROM_BASE_A0,
			SFP_BASE_ADDR + SFP_RATE_ADDRESS,
			&value->val);
}

sw_error_t
sfp_link_len_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union sfp_link_len_u *value)
{
	return sfp_data_tbl_get(
			dev_id, index, SFP_EEPROM_BASE_A0,
			SFP_BASE_ADDR + SFP_LINK_LEN_ADDRESS,
			value->val, 6);
}

sw_error_t
sfp_vendor_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union sfp_vendor_u *value)
{
	return sfp_data_tbl_get(
			dev_id, index, SFP_EEPROM_BASE_A0,
			SFP_BASE_ADDR + SFP_VENDOR_ADDRESS,
			value->val, 40);
}

sw_error_t
sfp_laser_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union sfp_laser_u *value)
{
	return sfp_data_tbl_get(
			dev_id, index, SFP_EEPROM_BASE_A0,
			SFP_BASE_ADDR + SFP_LASER_ADDRESS,
			value->val, 2);
}

sw_error_t
sfp_base_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union sfp_base_u *value)
{
	return sfp_data_get(
			dev_id, index, SFP_EEPROM_BASE_A0,
			SFP_BASE_ADDR + SFP_BASE_ADDRESS,
			&value->val);
}

sw_error_t
sfp_option_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union sfp_option_u *value)
{
	return sfp_data_tbl_get(
			dev_id, index, SFP_EEPROM_BASE_A0,
			SFP_BASE_ADDR + SFP_OPTION_ADDRESS,
			value->val, 2);
}

sw_error_t
sfp_rate_ctrl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union sfp_rate_ctrl_u *value)
{
	return sfp_data_tbl_get(
			dev_id, index, SFP_EEPROM_BASE_A0,
			SFP_BASE_ADDR + SFP_RATE_CTRL_ADDRESS,
			value->val, 2);
}

sw_error_t
sfp_vendor_ext_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union sfp_vendor_ext_u *value)
{
	return sfp_data_tbl_get(
			dev_id, index, SFP_EEPROM_BASE_A0,
			SFP_BASE_ADDR + SFP_VENDOR_EXT_ADDRESS,
			value->val, 24);
}

sw_error_t
sfp_enhanced_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union sfp_enhanced_u *value)
{
	return sfp_data_tbl_get(
			dev_id, index, SFP_EEPROM_BASE_A0,
			SFP_BASE_ADDR + SFP_ENHANCED_ADDRESS,
			value->val, 3);
}

sw_error_t
sfp_ext_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union sfp_ext_u *value)
{
	return sfp_data_get(
			dev_id, index, SFP_EEPROM_BASE_A0,
			SFP_BASE_ADDR + SFP_EXT_ADDRESS,
			&value->val);
}

sw_error_t
sfp_dev_type_id_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union sfp_dev_type_u reg_val;
	sw_error_t ret = SW_OK;

	ret = sfp_dev_type_get(dev_id, index, &reg_val);
	*value = reg_val.bf.id;
	return ret;
}

sw_error_t
sfp_dev_type_ext_id_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union sfp_dev_type_ext_u reg_val;
	sw_error_t ret = SW_OK;

	ret = sfp_dev_type_ext_get(dev_id, index, &reg_val);
	*value = reg_val.bf.id;
	return ret;
}

sw_error_t
sfp_dev_connector_type_code_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union sfp_dev_connector_type_u reg_val;
	sw_error_t ret = SW_OK;

	ret = sfp_dev_connector_type_get(dev_id, index, &reg_val);
	*value = reg_val.bf.code;
	return ret;
}

sw_error_t
sfp_transc_sonet_ccode_2_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union sfp_transc_u reg_val;
	sw_error_t ret = SW_OK;

	ret = sfp_transc_get(dev_id, index, &reg_val);
	*value = reg_val.bf.sonet_ccode_2;
	return ret;
}

sw_error_t
sfp_transc_fiber_ch_tech_1_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union sfp_transc_u reg_val;
	sw_error_t ret = SW_OK;

	ret = sfp_transc_get(dev_id, index, &reg_val);
	*value = reg_val.bf.fiber_ch_tech_1;
	return ret;
}

sw_error_t
sfp_transc_sonet_ccode_1_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union sfp_transc_u reg_val;
	sw_error_t ret = SW_OK;

	ret = sfp_transc_get(dev_id, index, &reg_val);
	*value = reg_val.bf.sonet_ccode_1;
	return ret;
}

sw_error_t
sfp_transc_fiber_ch_speed_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union sfp_transc_u reg_val;
	sw_error_t ret = SW_OK;

	ret = sfp_transc_get(dev_id, index, &reg_val);
	*value = reg_val.bf.fiber_ch_speed;
	return ret;
}

sw_error_t
sfp_transc_fiber_ch_tech_2_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union sfp_transc_u reg_val;
	sw_error_t ret = SW_OK;

	ret = sfp_transc_get(dev_id, index, &reg_val);
	*value = reg_val.bf.fiber_ch_tech_2;
	return ret;
}

sw_error_t
sfp_transc_cable_tech_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union sfp_transc_u reg_val;
	sw_error_t ret = SW_OK;

	ret = sfp_transc_get(dev_id, index, &reg_val);
	*value = reg_val.bf.cable_tech;
	return ret;
}

sw_error_t
sfp_transc_fiber_ch_link_len_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union sfp_transc_u reg_val;
	sw_error_t ret = SW_OK;

	ret = sfp_transc_get(dev_id, index, &reg_val);
	*value = reg_val.bf.fiber_ch_link_len;
	return ret;
}

sw_error_t
sfp_transc_unallocated_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union sfp_transc_u reg_val;
	sw_error_t ret = SW_OK;

	ret = sfp_transc_get(dev_id, index, &reg_val);
	*value = reg_val.bf.unallocated;
	return ret;
}

sw_error_t
sfp_transc_fiber_chan_tm_media_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union sfp_transc_u reg_val;
	sw_error_t ret = SW_OK;

	ret = sfp_transc_get(dev_id, index, &reg_val);
	*value = reg_val.bf.fiber_chan_tm_media;
	return ret;
}

sw_error_t
sfp_transc_escon_ccode_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union sfp_transc_u reg_val;
	sw_error_t ret = SW_OK;

	ret = sfp_transc_get(dev_id, index, &reg_val);
	*value = reg_val.bf.escon_ccode;
	return ret;
}

sw_error_t
sfp_transc_infiniband_ccode_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union sfp_transc_u reg_val;
	sw_error_t ret = SW_OK;

	ret = sfp_transc_get(dev_id, index, &reg_val);
	*value = reg_val.bf.infiniband_ccode;
	return ret;
}

sw_error_t
sfp_transc_eth_ccode_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union sfp_transc_u reg_val;
	sw_error_t ret = SW_OK;

	ret = sfp_transc_get(dev_id, index, &reg_val);
	*value = reg_val.bf.eth_ccode;
	return ret;
}

sw_error_t
sfp_transc_eth_10g_ccode_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union sfp_transc_u reg_val;
	sw_error_t ret = SW_OK;

	ret = sfp_transc_get(dev_id, index, &reg_val);
	*value = reg_val.bf.eth_10g_ccode;
	return ret;
}

sw_error_t
sfp_encoding_code_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union sfp_encoding_u reg_val;
	sw_error_t ret = SW_OK;

	ret = sfp_encoding_get(dev_id, index, &reg_val);
	*value = reg_val.bf.code;
	return ret;
}

sw_error_t
sfp_br_bit_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union sfp_br_u reg_val;
	sw_error_t ret = SW_OK;

	ret = sfp_br_get(dev_id, index, &reg_val);
	*value = reg_val.bf.bit;
	return ret;
}

sw_error_t
sfp_rate_id_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union sfp_rate_u reg_val;
	sw_error_t ret = SW_OK;

	ret = sfp_rate_get(dev_id, index, &reg_val);
	*value = reg_val.bf.id;
	return ret;
}

sw_error_t
sfp_link_len_om3_mode_1m_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union sfp_link_len_u reg_val;
	sw_error_t ret = SW_OK;

	ret = sfp_link_len_get(dev_id, index, &reg_val);
	*value = reg_val.bf.om3_mode_1m;
	return ret;
}

sw_error_t
sfp_link_len_single_mode_100m_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union sfp_link_len_u reg_val;
	sw_error_t ret = SW_OK;

	ret = sfp_link_len_get(dev_id, index, &reg_val);
	*value = reg_val.bf.single_mode_100m;
	return ret;
}

sw_error_t
sfp_link_len_om2_mode_10m_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union sfp_link_len_u reg_val;
	sw_error_t ret = SW_OK;

	ret = sfp_link_len_get(dev_id, index, &reg_val);
	*value = reg_val.bf.om2_mode_10m;
	return ret;
}

sw_error_t
sfp_link_len_copper_mode_1m_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union sfp_link_len_u reg_val;
	sw_error_t ret = SW_OK;

	ret = sfp_link_len_get(dev_id, index, &reg_val);
	*value = reg_val.bf.copper_mode_1m;
	return ret;
}

sw_error_t
sfp_link_len_om1_mode_10m_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union sfp_link_len_u reg_val;
	sw_error_t ret = SW_OK;

	ret = sfp_link_len_get(dev_id, index, &reg_val);
	*value = reg_val.bf.om1_mode_10m;
	return ret;
}

sw_error_t
sfp_link_len_single_mode_km_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union sfp_link_len_u reg_val;
	sw_error_t ret = SW_OK;

	ret = sfp_link_len_get(dev_id, index, &reg_val);
	*value = reg_val.bf.single_mode_km;
	return ret;
}

sw_error_t
sfp_vendor_rev_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint8_t value[4])
{
	union sfp_vendor_u reg_val;
	sw_error_t ret = SW_OK;
	a_uint8_t i;

	ret = sfp_vendor_get(dev_id, index, &reg_val);

	for (i = 0; i < 4; i++) {
		value[i] = reg_val.val[i];
	}

	return ret;
}

sw_error_t
sfp_vendor_name_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint8_t value[16])
{
	union sfp_vendor_u reg_val;
	sw_error_t ret = SW_OK;
	a_uint8_t i;

	ret = sfp_vendor_get(dev_id, index, &reg_val);

	for (i = 0; i < 16; i++) {
		value[i] = reg_val.val[i];
	}

	return ret;
}

sw_error_t
sfp_vendor_oui_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint8_t value[3])
{
	union sfp_vendor_u reg_val;
	sw_error_t ret = SW_OK;
	a_uint8_t i;

	ret = sfp_vendor_get(dev_id, index, &reg_val);

	for (i = 0; i < 3; i++) {
		value[i] = reg_val.val[i];
	}

	return ret;
}

sw_error_t
sfp_vendor_pn_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint8_t value[16])
{
	union sfp_vendor_u reg_val;
	sw_error_t ret = SW_OK;
	a_uint8_t i;

	ret = sfp_vendor_get(dev_id, index, &reg_val);

	for (i = 0; i < 16; i++) {
		value[i] = reg_val.val[i];
	}

	return ret;
}
sw_error_t
sfp_laser_wavelength_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union sfp_laser_u reg_val;
	sw_error_t ret = SW_OK;

	ret = sfp_laser_get(dev_id, index, &reg_val);
	*value = reg_val.bf.wavelength_0 << 8
		| reg_val.bf.wavelength_1;
	return ret;
}

sw_error_t
sfp_base_check_code_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union sfp_base_u reg_val;
	sw_error_t ret = SW_OK;

	ret = sfp_base_get(dev_id, index, &reg_val);
	*value = reg_val.bf.check_code;
	return ret;
}

sw_error_t
sfp_option_linear_recv_output_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union sfp_option_u reg_val;
	sw_error_t ret = SW_OK;

	ret = sfp_option_get(dev_id, index, &reg_val);
	*value = reg_val.bf.linear_recv_output;
	return ret;
}

sw_error_t
sfp_option_pwr_level_declar_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union sfp_option_u reg_val;
	sw_error_t ret = SW_OK;

	ret = sfp_option_get(dev_id, index, &reg_val);
	*value = reg_val.bf.pwr_level_declar;
	return ret;
}

sw_error_t
sfp_option_unallocated_1_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union sfp_option_u reg_val;
	sw_error_t ret = SW_OK;

	ret = sfp_option_get(dev_id, index, &reg_val);
	*value = reg_val.bf.unallocated_1;
	return ret;
}

sw_error_t
sfp_option_unallocated_3_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union sfp_option_u reg_val;
	sw_error_t ret = SW_OK;

	ret = sfp_option_get(dev_id, index, &reg_val);
	*value = reg_val.bf.unallocated_3;
	return ret;
}

sw_error_t
sfp_option_loss_signal_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union sfp_option_u reg_val;
	sw_error_t ret = SW_OK;

	ret = sfp_option_get(dev_id, index, &reg_val);
	*value = reg_val.bf.loss_signal;
	return ret;
}

sw_error_t
sfp_option_rate_sel_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union sfp_option_u reg_val;
	sw_error_t ret = SW_OK;

	ret = sfp_option_get(dev_id, index, &reg_val);
	*value = reg_val.bf.rate_sel;
	return ret;
}

sw_error_t
sfp_option_unallocated_2_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union sfp_option_u reg_val;
	sw_error_t ret = SW_OK;

	ret = sfp_option_get(dev_id, index, &reg_val);
	*value = reg_val.bf.unallocated_2;
	return ret;
}

sw_error_t
sfp_option_loss_invert_signal_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union sfp_option_u reg_val;
	sw_error_t ret = SW_OK;

	ret = sfp_option_get(dev_id, index, &reg_val);
	*value = reg_val.bf.loss_invert_signal;
	return ret;
}

sw_error_t
sfp_option_tx_disable_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union sfp_option_u reg_val;
	sw_error_t ret = SW_OK;

	ret = sfp_option_get(dev_id, index, &reg_val);
	*value = reg_val.bf.tx_disable;
	return ret;
}

sw_error_t
sfp_option_cool_transc_declar_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union sfp_option_u reg_val;
	sw_error_t ret = SW_OK;

	ret = sfp_option_get(dev_id, index, &reg_val);
	*value = reg_val.bf.cool_transc_declar;
	return ret;
}

sw_error_t
sfp_option_tx_fault_signal_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union sfp_option_u reg_val;
	sw_error_t ret = SW_OK;

	ret = sfp_option_get(dev_id, index, &reg_val);
	*value = reg_val.bf.tx_fault_signal;
	return ret;
}

sw_error_t
sfp_rate_ctrl_upper_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union sfp_rate_ctrl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = sfp_rate_ctrl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.upper;
	return ret;
}

sw_error_t
sfp_rate_ctrl_lower_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union sfp_rate_ctrl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = sfp_rate_ctrl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.lower;
	return ret;
}

sw_error_t
sfp_vendor_ext_date_code_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint8_t value[8])
{
	union sfp_vendor_ext_u reg_val;
	sw_error_t ret = SW_OK;
	a_uint8_t i;

	ret = sfp_vendor_ext_get(dev_id, index, &reg_val);

	for (i = 0; i < 8; i++) {
		value[i] = reg_val.val[i];
	}

	return ret;
}

sw_error_t
sfp_vendor_ext_sn_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint8_t value[16])
{
	union sfp_vendor_ext_u reg_val;
	sw_error_t ret = SW_OK;
	a_uint8_t i;

	ret = sfp_vendor_ext_get(dev_id, index, &reg_val);

	for (i = 0; i < 16; i++) {
		value[i] = reg_val.val[i];
	}

	return ret;
}

sw_error_t
sfp_enhanced_diag_mon_flag_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union sfp_enhanced_u reg_val;
	sw_error_t ret = SW_OK;

	ret = sfp_enhanced_get(dev_id, index, &reg_val);
	*value = reg_val.bf.diag_mon_flag;
	return ret;
}

sw_error_t
sfp_enhanced_rx_los_op_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union sfp_enhanced_u reg_val;
	sw_error_t ret = SW_OK;

	ret = sfp_enhanced_get(dev_id, index, &reg_val);
	*value = reg_val.bf.rx_los_op;
	return ret;
}

sw_error_t
sfp_enhanced_cmpl_feature_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union sfp_enhanced_u reg_val;
	sw_error_t ret = SW_OK;

	ret = sfp_enhanced_get(dev_id, index, &reg_val);
	*value = reg_val.bf.cmpl_feature;
	return ret;
}

sw_error_t
sfp_enhanced_tx_disable_ctrl_op_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union sfp_enhanced_u reg_val;
	sw_error_t ret = SW_OK;

	ret = sfp_enhanced_get(dev_id, index, &reg_val);
	*value = reg_val.bf.tx_disable_ctrl_op;
	return ret;
}

sw_error_t
sfp_enhanced_alarm_warning_flag_op_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union sfp_enhanced_u reg_val;
	sw_error_t ret = SW_OK;

	ret = sfp_enhanced_get(dev_id, index, &reg_val);
	*value = reg_val.bf.alarm_warning_flag_op;
	return ret;
}

sw_error_t
sfp_enhanced_addr_mode_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union sfp_enhanced_u reg_val;
	sw_error_t ret = SW_OK;

	ret = sfp_enhanced_get(dev_id, index, &reg_val);
	*value = reg_val.bf.addr_mode;
	return ret;
}

sw_error_t
sfp_enhanced_unallocated_op_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union sfp_enhanced_u reg_val;
	sw_error_t ret = SW_OK;

	ret = sfp_enhanced_get(dev_id, index, &reg_val);
	*value = reg_val.bf.unallocated_op;
	return ret;
}

sw_error_t
sfp_enhanced_soft_rate_sel_op_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union sfp_enhanced_u reg_val;
	sw_error_t ret = SW_OK;

	ret = sfp_enhanced_get(dev_id, index, &reg_val);
	*value = reg_val.bf.soft_rate_sel_op;
	return ret;
}

sw_error_t
sfp_enhanced_external_cal_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union sfp_enhanced_u reg_val;
	sw_error_t ret = SW_OK;

	ret = sfp_enhanced_get(dev_id, index, &reg_val);
	*value = reg_val.bf.external_cal;
	return ret;
}

sw_error_t
sfp_enhanced_internal_cal_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union sfp_enhanced_u reg_val;
	sw_error_t ret = SW_OK;

	ret = sfp_enhanced_get(dev_id, index, &reg_val);
	*value = reg_val.bf.internal_cal;
	return ret;
}

sw_error_t
sfp_enhanced_re_pwr_type_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union sfp_enhanced_u reg_val;
	sw_error_t ret = SW_OK;

	ret = sfp_enhanced_get(dev_id, index, &reg_val);
	*value = reg_val.bf.re_pwr_type;
	return ret;
}

sw_error_t
sfp_enhanced_soft_rate_ctrl_op_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union sfp_enhanced_u reg_val;
	sw_error_t ret = SW_OK;

	ret = sfp_enhanced_get(dev_id, index, &reg_val);
	*value = reg_val.bf.soft_rate_ctrl_op;
	return ret;
}

sw_error_t
sfp_enhanced_app_sel_op_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union sfp_enhanced_u reg_val;
	sw_error_t ret = SW_OK;

	ret = sfp_enhanced_get(dev_id, index, &reg_val);
	*value = reg_val.bf.app_sel_op;
	return ret;
}

sw_error_t
sfp_enhanced_legacy_type_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union sfp_enhanced_u reg_val;
	sw_error_t ret = SW_OK;

	ret = sfp_enhanced_get(dev_id, index, &reg_val);
	*value = reg_val.bf.legacy_type;
	return ret;
}

sw_error_t
sfp_enhanced_tx_fault_op_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union sfp_enhanced_u reg_val;
	sw_error_t ret = SW_OK;

	ret = sfp_enhanced_get(dev_id, index, &reg_val);
	*value = reg_val.bf.tx_fault_op;
	return ret;
}

sw_error_t
sfp_enhanced_unallocated_type_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union sfp_enhanced_u reg_val;
	sw_error_t ret = SW_OK;

	ret = sfp_enhanced_get(dev_id, index, &reg_val);
	*value = reg_val.bf.unallocated_type;
	return ret;
}

sw_error_t
sfp_ext_check_code_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union sfp_ext_u reg_val;
	sw_error_t ret = SW_OK;

	ret = sfp_ext_get(dev_id, index, &reg_val);
	*value = reg_val.bf.check_code;
	return ret;
}

sw_error_t
sfp_diag_threshold_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union sfp_diag_threshold_u *value)
{
	return sfp_data_tbl_get(
			dev_id, index, SFP_EEPROM_DIAG_A2,
			SFP_DIAG_BASE_ADDR + SFP_DIAG_THRESHOLD_ADDRESS,
			value->val, 40);
}

sw_error_t
sfp_diag_cal_const_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union sfp_diag_cal_const_u *value)
{
	return sfp_data_tbl_get(
			dev_id, index, SFP_EEPROM_DIAG_A2,
			SFP_DIAG_BASE_ADDR + SFP_DIAG_CAL_CONST_ADDRESS,
			value->val, 36);
}

sw_error_t
sfp_diag_dmi_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union sfp_diag_dmi_u *value)
{
	return sfp_data_get(
			dev_id, index, SFP_EEPROM_DIAG_A2,
			SFP_DIAG_BASE_ADDR + SFP_DIAG_DMI_ADDRESS,
			&value->val);
}

sw_error_t
sfp_diag_realtime_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union sfp_diag_realtime_u *value)
{
	return sfp_data_tbl_get(
			dev_id, index, SFP_EEPROM_DIAG_A2,
			SFP_DIAG_BASE_ADDR + SFP_DIAG_REALTIME_ADDRESS,
			value->val, 10);
}

sw_error_t
sfp_diag_optional_ctrl_status_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union sfp_diag_optional_ctrl_status_u *value)
{
	return sfp_data_get(
			dev_id, index, SFP_EEPROM_DIAG_A2,
			SFP_DIAG_BASE_ADDR + SFP_DIAG_OPTIONAL_CTRL_STATUS_ADDRESS,
			value->val);
}

sw_error_t
sfp_diag_flag_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union sfp_diag_flag_u *value)
{
	return sfp_data_tbl_get(
			dev_id, index, SFP_EEPROM_DIAG_A2,
			SFP_DIAG_BASE_ADDR + SFP_DIAG_FLAG_ADDRESS,
			value->val, 6);
}

sw_error_t
sfp_diag_extended_ctrl_status_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union sfp_diag_extended_ctrl_status_u *value)
{
	return sfp_data_tbl_get(
			dev_id, index, SFP_EEPROM_DIAG_A2,
			SFP_DIAG_BASE_ADDR + SFP_DIAG_EXTENDED_CTRL_STATUS_ADDRESS,
			value->val, 2);
}

sw_error_t
sfp_diag_threshold_rx_pwr_low_alarm_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union sfp_diag_threshold_u reg_val;
	sw_error_t ret = SW_OK;

	ret = sfp_diag_threshold_get(dev_id, index, &reg_val);
	*value = reg_val.bf.rx_pwr_low_alarm_0 << 8 | \
		 reg_val.bf.rx_pwr_low_alarm_1;
	return ret;
}

sw_error_t
sfp_diag_threshold_rx_pwr_high_warning_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union sfp_diag_threshold_u reg_val;
	sw_error_t ret = SW_OK;

	ret = sfp_diag_threshold_get(dev_id, index, &reg_val);
	*value = reg_val.bf.rx_pwr_high_warning_0 << 8 | \
		 reg_val.bf.rx_pwr_high_warning_1;
	return ret;
}

sw_error_t
sfp_diag_threshold_temp_low_alarm_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union sfp_diag_threshold_u reg_val;
	sw_error_t ret = SW_OK;

	ret = sfp_diag_threshold_get(dev_id, index, &reg_val);
	*value = reg_val.bf.temp_low_alarm_0 << 8 | \
		 reg_val.bf.temp_low_alarm_1;
	return ret;
}

sw_error_t
sfp_diag_threshold_vol_high_alarm_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union sfp_diag_threshold_u reg_val;
	sw_error_t ret = SW_OK;

	ret = sfp_diag_threshold_get(dev_id, index, &reg_val);
	*value = reg_val.bf.vol_high_alarm_0 << 8 | \
		 reg_val.bf.vol_high_alarm_1;
	return ret;
}

sw_error_t
sfp_diag_threshold_tx_pwr_high_alarm_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union sfp_diag_threshold_u reg_val;
	sw_error_t ret = SW_OK;

	ret = sfp_diag_threshold_get(dev_id, index, &reg_val);
	*value = reg_val.bf.tx_pwr_high_alarm_0 << 8 | \
		 reg_val.bf.tx_pwr_high_alarm_1;
	return ret;
}

sw_error_t
sfp_diag_threshold_bias_low_alarm_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union sfp_diag_threshold_u reg_val;
	sw_error_t ret = SW_OK;

	ret = sfp_diag_threshold_get(dev_id, index, &reg_val);
	*value = reg_val.bf.bias_low_alarm_0 << 8 | \
		 reg_val.bf.bias_low_alarm_1;
	return ret;
}

sw_error_t
sfp_diag_threshold_bias_high_alarm_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union sfp_diag_threshold_u reg_val;
	sw_error_t ret = SW_OK;

	ret = sfp_diag_threshold_get(dev_id, index, &reg_val);
	*value = reg_val.bf.bias_high_alarm_0 << 8 | \
		 reg_val.bf.bias_high_alarm_1;
	return ret;
}

sw_error_t
sfp_diag_threshold_vol_low_alarm_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union sfp_diag_threshold_u reg_val;
	sw_error_t ret = SW_OK;

	ret = sfp_diag_threshold_get(dev_id, index, &reg_val);
	*value = reg_val.bf.vol_low_alarm_0 << 8 | \
		 reg_val.bf.vol_low_alarm_1;
	return ret;
}

sw_error_t
sfp_diag_threshold_bias_high_warning_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union sfp_diag_threshold_u reg_val;
	sw_error_t ret = SW_OK;

	ret = sfp_diag_threshold_get(dev_id, index, &reg_val);
	*value = reg_val.bf.bias_high_warning_0 << 8 | \
		 reg_val.bf.bias_high_warning_1;
	return ret;
}

sw_error_t
sfp_diag_threshold_temp_high_warning_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union sfp_diag_threshold_u reg_val;
	sw_error_t ret = SW_OK;

	ret = sfp_diag_threshold_get(dev_id, index, &reg_val);
	*value = reg_val.bf.temp_high_warning_0 << 8 | \
		 reg_val.bf.temp_high_warning_1;
	return ret;
}

sw_error_t
sfp_diag_threshold_rx_pwr_low_warning_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union sfp_diag_threshold_u reg_val;
	sw_error_t ret = SW_OK;

	ret = sfp_diag_threshold_get(dev_id, index, &reg_val);
	*value = reg_val.bf.rx_pwr_low_warning_0 << 8 | \
		 reg_val.bf.rx_pwr_low_warning_1;
	return ret;
}

sw_error_t
sfp_diag_threshold_vol_low_warning_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union sfp_diag_threshold_u reg_val;
	sw_error_t ret = SW_OK;

	ret = sfp_diag_threshold_get(dev_id, index, &reg_val);
	*value = reg_val.bf.vol_low_warning_0 << 8 | \
		 reg_val.bf.vol_low_warning_1;
	return ret;
}

sw_error_t
sfp_diag_threshold_tx_pwr_low_alarm_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union sfp_diag_threshold_u reg_val;
	sw_error_t ret = SW_OK;

	ret = sfp_diag_threshold_get(dev_id, index, &reg_val);
	*value = reg_val.bf.tx_pwr_low_alarm_0 << 8 | \
		 reg_val.bf.tx_pwr_low_alarm_1;
	return ret;
}

sw_error_t
sfp_diag_threshold_bias_low_warning_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union sfp_diag_threshold_u reg_val;
	sw_error_t ret = SW_OK;

	ret = sfp_diag_threshold_get(dev_id, index, &reg_val);
	*value = reg_val.bf.bias_low_warning_0 << 8 | \
		 reg_val.bf.bias_low_warning_1;
	return ret;
}

sw_error_t
sfp_diag_threshold_temp_high_alarm_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union sfp_diag_threshold_u reg_val;
	sw_error_t ret = SW_OK;

	ret = sfp_diag_threshold_get(dev_id, index, &reg_val);
	*value = reg_val.bf.temp_high_alarm_0 << 8 | \
		 reg_val.bf.temp_high_alarm_1;
	return ret;
}

sw_error_t
sfp_diag_threshold_tx_pwr_high_warning_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union sfp_diag_threshold_u reg_val;
	sw_error_t ret = SW_OK;

	ret = sfp_diag_threshold_get(dev_id, index, &reg_val);
	*value = reg_val.bf.tx_pwr_high_warning_0 << 8 | \
		 reg_val.bf.tx_pwr_high_warning_1;
	return ret;
}

sw_error_t
sfp_diag_threshold_vol_high_warning_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union sfp_diag_threshold_u reg_val;
	sw_error_t ret = SW_OK;

	ret = sfp_diag_threshold_get(dev_id, index, &reg_val);
	*value = reg_val.bf.vol_high_warning_0 << 8 | \
		 reg_val.bf.vol_high_warning_1;
	return ret;
}

sw_error_t
sfp_diag_threshold_temp_low_warning_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union sfp_diag_threshold_u reg_val;
	sw_error_t ret = SW_OK;

	ret = sfp_diag_threshold_get(dev_id, index, &reg_val);
	*value = reg_val.bf.temp_low_warning_0 << 8 | \
		 reg_val.bf.temp_low_warning_1;
	return ret;
}

sw_error_t
sfp_diag_threshold_rx_pwr_high_alarm_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union sfp_diag_threshold_u reg_val;
	sw_error_t ret = SW_OK;

	ret = sfp_diag_threshold_get(dev_id, index, &reg_val);
	*value = reg_val.bf.rx_pwr_high_alarm_0 << 8 | \
		 reg_val.bf.rx_pwr_high_alarm_1;
	return ret;
}

sw_error_t
sfp_diag_threshold_tx_pwr_low_warning_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union sfp_diag_threshold_u reg_val;
	sw_error_t ret = SW_OK;

	ret = sfp_diag_threshold_get(dev_id, index, &reg_val);
	*value = reg_val.bf.tx_pwr_low_warning_0 << 8 | \
		 reg_val.bf.tx_pwr_low_warning_1;
	return ret;
}

sw_error_t
sfp_diag_cal_const_rx_pwr_1_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union sfp_diag_cal_const_u reg_val;
	sw_error_t ret = SW_OK;

	ret = sfp_diag_cal_const_get(dev_id, index, &reg_val);
	*value = reg_val.bf.rx_pwr_1_0 << 24 | \
		 reg_val.bf.rx_pwr_1_1 << 16 | \
		 reg_val.bf.rx_pwr_1_2 << 8 | \
		 reg_val.bf.rx_pwr_1_3;
	return ret;
}

sw_error_t
sfp_diag_cal_const_t_slope_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union sfp_diag_cal_const_u reg_val;
	sw_error_t ret = SW_OK;

	ret = sfp_diag_cal_const_get(dev_id, index, &reg_val);
	*value = reg_val.bf.t_slope_0 << 8 | \
		 reg_val.bf.t_slope_1;
	return ret;
}

sw_error_t
sfp_diag_cal_const_rx_pwr_3_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union sfp_diag_cal_const_u reg_val;
	sw_error_t ret = SW_OK;

	ret = sfp_diag_cal_const_get(dev_id, index, &reg_val);
	*value = reg_val.bf.rx_pwr_3_0 << 24 | \
		 reg_val.bf.rx_pwr_3_1 << 16 | \
		 reg_val.bf.rx_pwr_3_2 << 8 | \
		 reg_val.bf.rx_pwr_3_3;
	return ret;
}

sw_error_t
sfp_diag_cal_const_v_offset_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union sfp_diag_cal_const_u reg_val;
	sw_error_t ret = SW_OK;

	ret = sfp_diag_cal_const_get(dev_id, index, &reg_val);
	*value = reg_val.bf.v_offset_0 << 8 | \
		 reg_val.bf.v_offset_1;
	return ret;
}

sw_error_t
sfp_diag_cal_const_rx_pwr_2_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union sfp_diag_cal_const_u reg_val;
	sw_error_t ret = SW_OK;

	ret = sfp_diag_cal_const_get(dev_id, index, &reg_val);
	*value = reg_val.bf.rx_pwr_2_0 << 24 | \
		 reg_val.bf.rx_pwr_2_1 << 16 | \
		 reg_val.bf.rx_pwr_2_2 << 8 | \
		 reg_val.bf.rx_pwr_2_3;
	return ret;
}

sw_error_t
sfp_diag_cal_const_tx_i_slope_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union sfp_diag_cal_const_u reg_val;
	sw_error_t ret = SW_OK;

	ret = sfp_diag_cal_const_get(dev_id, index, &reg_val);
	*value = reg_val.bf.tx_i_slope_0 << 8 | \
		 reg_val.bf.tx_i_slope_1;
	return ret;
}

sw_error_t
sfp_diag_cal_const_tx_i_offset_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union sfp_diag_cal_const_u reg_val;
	sw_error_t ret = SW_OK;

	ret = sfp_diag_cal_const_get(dev_id, index, &reg_val);
	*value = reg_val.bf.tx_i_offset_0 << 8 | \
		 reg_val.bf.tx_i_offset_1;
	return ret;
}

sw_error_t
sfp_diag_cal_const_v_slope_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union sfp_diag_cal_const_u reg_val;
	sw_error_t ret = SW_OK;

	ret = sfp_diag_cal_const_get(dev_id, index, &reg_val);
	*value = reg_val.bf.v_slope_0 << 8 | \
		 reg_val.bf.v_slope_1;
	return ret;
}

sw_error_t
sfp_diag_cal_const_t_offset_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union sfp_diag_cal_const_u reg_val;
	sw_error_t ret = SW_OK;

	ret = sfp_diag_cal_const_get(dev_id, index, &reg_val);
	*value = reg_val.bf.t_offset_0 << 8 | \
		 reg_val.bf.t_offset_1;
	return ret;
}

sw_error_t
sfp_diag_cal_const_tx_pwr_offset_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union sfp_diag_cal_const_u reg_val;
	sw_error_t ret = SW_OK;

	ret = sfp_diag_cal_const_get(dev_id, index, &reg_val);
	*value = reg_val.bf.tx_pwr_offset_0 << 8 | \
		 reg_val.bf.tx_pwr_offset_1;
	return ret;
}

sw_error_t
sfp_diag_cal_const_tx_pwr_slope_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union sfp_diag_cal_const_u reg_val;
	sw_error_t ret = SW_OK;

	ret = sfp_diag_cal_const_get(dev_id, index, &reg_val);
	*value = reg_val.bf.tx_pwr_slope_0 << 8 | \
		 reg_val.bf.tx_pwr_slope_1;
	return ret;
}

sw_error_t
sfp_diag_cal_const_rx_pwr_4_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union sfp_diag_cal_const_u reg_val;
	sw_error_t ret = SW_OK;

	ret = sfp_diag_cal_const_get(dev_id, index, &reg_val);
	*value = reg_val.bf.rx_pwr_4_0 << 24 | \
		 reg_val.bf.rx_pwr_4_1 << 16 | \
		 reg_val.bf.rx_pwr_4_2 << 8 | \
		 reg_val.bf.rx_pwr_4_3;
	return ret;
}

sw_error_t
sfp_diag_cal_const_rx_pwr_0_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union sfp_diag_cal_const_u reg_val;
	sw_error_t ret = SW_OK;

	ret = sfp_diag_cal_const_get(dev_id, index, &reg_val);
	*value = reg_val.bf.rx_pwr_0_0 << 24 | \
		 reg_val.bf.rx_pwr_0_1 << 16 | \
		 reg_val.bf.rx_pwr_0_2 << 8 | \
		 reg_val.bf.rx_pwr_0_3;
	return ret;
}

sw_error_t
sfp_diag_dmi_check_code_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union sfp_diag_dmi_u reg_val;
	sw_error_t ret = SW_OK;

	ret = sfp_diag_dmi_get(dev_id, index, &reg_val);
	*value = reg_val.bf.check_code;
	return ret;
}

sw_error_t
sfp_diag_realtime_vcc_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union sfp_diag_realtime_u reg_val;
	sw_error_t ret = SW_OK;

	ret = sfp_diag_realtime_get(dev_id, index, &reg_val);
	*value = reg_val.bf.vcc_1 << 8 | \
		 reg_val.bf.vcc_0;
	return ret;
}

sw_error_t
sfp_diag_realtime_tx_pwr_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union sfp_diag_realtime_u reg_val;
	sw_error_t ret = SW_OK;

	ret = sfp_diag_realtime_get(dev_id, index, &reg_val);
	*value = reg_val.bf.tx_pwr_1 << 8 | \
		 reg_val.bf.tx_pwr_0;
	return ret;
}

sw_error_t
sfp_diag_realtime_tx_bias_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union sfp_diag_realtime_u reg_val;
	sw_error_t ret = SW_OK;

	ret = sfp_diag_realtime_get(dev_id, index, &reg_val);
	*value = reg_val.bf.tx_bias_1 << 8 | \
		 reg_val.bf.tx_bias_0;
	return ret;
}

sw_error_t
sfp_diag_realtime_rx_pwr_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union sfp_diag_realtime_u reg_val;
	sw_error_t ret = SW_OK;

	ret = sfp_diag_realtime_get(dev_id, index, &reg_val);
	*value = reg_val.bf.rx_pwr_1 << 8 | \
		 reg_val.bf.rx_pwr_0;
	return ret;
}

sw_error_t
sfp_diag_realtime_tmp_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union sfp_diag_realtime_u reg_val;
	sw_error_t ret = SW_OK;

	ret = sfp_diag_realtime_get(dev_id, index, &reg_val);
	*value = reg_val.bf.tmp_1 << 8 | \
		 reg_val.bf.tmp_0;
	return ret;
}

sw_error_t
sfp_diag_optional_ctrl_status_rs_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union sfp_diag_optional_ctrl_status_u reg_val;
	sw_error_t ret = SW_OK;

	ret = sfp_diag_optional_ctrl_status_get(dev_id, index, &reg_val);
	*value = reg_val.bf.rs;
	return ret;
}

sw_error_t
sfp_diag_optional_ctrl_status_tx_fault_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union sfp_diag_optional_ctrl_status_u reg_val;
	sw_error_t ret = SW_OK;

	ret = sfp_diag_optional_ctrl_status_get(dev_id, index, &reg_val);
	*value = reg_val.bf.tx_fault;
	return ret;
}

sw_error_t
sfp_diag_optional_ctrl_status_rx_los_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union sfp_diag_optional_ctrl_status_u reg_val;
	sw_error_t ret = SW_OK;

	ret = sfp_diag_optional_ctrl_status_get(dev_id, index, &reg_val);
	*value = reg_val.bf.rx_los;
	return ret;
}

sw_error_t
sfp_diag_optional_ctrl_status_data_ready_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union sfp_diag_optional_ctrl_status_u reg_val;
	sw_error_t ret = SW_OK;

	ret = sfp_diag_optional_ctrl_status_get(dev_id, index, &reg_val);
	*value = reg_val.bf.data_ready;
	return ret;
}

sw_error_t
sfp_diag_optional_ctrl_status_soft_rate_sel_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union sfp_diag_optional_ctrl_status_u reg_val;
	sw_error_t ret = SW_OK;

	ret = sfp_diag_optional_ctrl_status_get(dev_id, index, &reg_val);
	*value = reg_val.bf.soft_rate_sel;
	return ret;
}

sw_error_t
sfp_diag_optional_ctrl_status_soft_tx_disable_sel_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union sfp_diag_optional_ctrl_status_u reg_val;
	sw_error_t ret = SW_OK;

	ret = sfp_diag_optional_ctrl_status_get(dev_id, index, &reg_val);
	*value = reg_val.bf.soft_tx_disable_sel;
	return ret;
}

sw_error_t
sfp_diag_optional_ctrl_status_rate_sel_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union sfp_diag_optional_ctrl_status_u reg_val;
	sw_error_t ret = SW_OK;

	ret = sfp_diag_optional_ctrl_status_get(dev_id, index, &reg_val);
	*value = reg_val.bf.rate_sel;
	return ret;
}

sw_error_t
sfp_diag_optional_ctrl_status_tx_disable_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union sfp_diag_optional_ctrl_status_u reg_val;
	sw_error_t ret = SW_OK;

	ret = sfp_diag_optional_ctrl_status_get(dev_id, index, &reg_val);
	*value = reg_val.bf.tx_disable;
	return ret;
}

sw_error_t
sfp_diag_flag_rx_pwr_low_alarm_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union sfp_diag_flag_u reg_val;
	sw_error_t ret = SW_OK;

	ret = sfp_diag_flag_get(dev_id, index, &reg_val);
	*value = reg_val.bf.rx_pwr_low_alarm;
	return ret;
}

sw_error_t
sfp_diag_flag_rx_pwr_high_warning_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union sfp_diag_flag_u reg_val;
	sw_error_t ret = SW_OK;

	ret = sfp_diag_flag_get(dev_id, index, &reg_val);
	*value = reg_val.bf.rx_pwr_high_warning;
	return ret;
}

sw_error_t
sfp_diag_flag_tx_bias_high_alarm_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union sfp_diag_flag_u reg_val;
	sw_error_t ret = SW_OK;

	ret = sfp_diag_flag_get(dev_id, index, &reg_val);
	*value = reg_val.bf.tx_bias_high_alarm;
	return ret;
}

sw_error_t
sfp_diag_flag_tmp_high_alarm_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union sfp_diag_flag_u reg_val;
	sw_error_t ret = SW_OK;

	ret = sfp_diag_flag_get(dev_id, index, &reg_val);
	*value = reg_val.bf.tmp_high_alarm;
	return ret;
}

sw_error_t
sfp_diag_flag_tmp_low_alarm_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union sfp_diag_flag_u reg_val;
	sw_error_t ret = SW_OK;

	ret = sfp_diag_flag_get(dev_id, index, &reg_val);
	*value = reg_val.bf.tmp_low_alarm;
	return ret;
}

sw_error_t
sfp_diag_flag_tx_pwr_high_alarm_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union sfp_diag_flag_u reg_val;
	sw_error_t ret = SW_OK;

	ret = sfp_diag_flag_get(dev_id, index, &reg_val);
	*value = reg_val.bf.tx_pwr_high_alarm;
	return ret;
}

sw_error_t
sfp_diag_flag_vcc_low_warning_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union sfp_diag_flag_u reg_val;
	sw_error_t ret = SW_OK;

	ret = sfp_diag_flag_get(dev_id, index, &reg_val);
	*value = reg_val.bf.vcc_low_warning;
	return ret;
}

sw_error_t
sfp_diag_flag_vcc_high_warning_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union sfp_diag_flag_u reg_val;
	sw_error_t ret = SW_OK;

	ret = sfp_diag_flag_get(dev_id, index, &reg_val);
	*value = reg_val.bf.vcc_high_warning;
	return ret;
}

sw_error_t
sfp_diag_flag_rx_pwr_low_warning_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union sfp_diag_flag_u reg_val;
	sw_error_t ret = SW_OK;

	ret = sfp_diag_flag_get(dev_id, index, &reg_val);
	*value = reg_val.bf.rx_pwr_low_warning;
	return ret;
}

sw_error_t
sfp_diag_flag_unallocated_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union sfp_diag_flag_u reg_val;
	sw_error_t ret = SW_OK;

	ret = sfp_diag_flag_get(dev_id, index, &reg_val);
	*value = reg_val.bf.unallocated_1 << 8 | \
		 reg_val.bf.unallocated_0;
	return ret;
}

sw_error_t
sfp_diag_flag_tx_bias_high_warning_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union sfp_diag_flag_u reg_val;
	sw_error_t ret = SW_OK;

	ret = sfp_diag_flag_get(dev_id, index, &reg_val);
	*value = reg_val.bf.tx_bias_high_warning;
	return ret;
}

sw_error_t
sfp_diag_flag_vcc_low_alarm_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union sfp_diag_flag_u reg_val;
	sw_error_t ret = SW_OK;

	ret = sfp_diag_flag_get(dev_id, index, &reg_val);
	*value = reg_val.bf.vcc_low_alarm;
	return ret;
}

sw_error_t
sfp_diag_flag_tx_pwr_low_alarm_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union sfp_diag_flag_u reg_val;
	sw_error_t ret = SW_OK;

	ret = sfp_diag_flag_get(dev_id, index, &reg_val);
	*value = reg_val.bf.tx_pwr_low_alarm;
	return ret;
}

sw_error_t
sfp_diag_flag_vcc_high_alarm_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union sfp_diag_flag_u reg_val;
	sw_error_t ret = SW_OK;

	ret = sfp_diag_flag_get(dev_id, index, &reg_val);
	*value = reg_val.bf.vcc_high_alarm;
	return ret;
}

sw_error_t
sfp_diag_flag_tmp_low_warning_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union sfp_diag_flag_u reg_val;
	sw_error_t ret = SW_OK;

	ret = sfp_diag_flag_get(dev_id, index, &reg_val);
	*value = reg_val.bf.tmp_low_warning;
	return ret;
}

sw_error_t
sfp_diag_flag_tx_bias_low_warning_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union sfp_diag_flag_u reg_val;
	sw_error_t ret = SW_OK;

	ret = sfp_diag_flag_get(dev_id, index, &reg_val);
	*value = reg_val.bf.tx_bias_low_warning;
	return ret;
}

sw_error_t
sfp_diag_flag_tx_bias_low_alarm_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union sfp_diag_flag_u reg_val;
	sw_error_t ret = SW_OK;

	ret = sfp_diag_flag_get(dev_id, index, &reg_val);
	*value = reg_val.bf.tx_bias_low_alarm;
	return ret;
}

sw_error_t
sfp_diag_flag_tmp_high_warning_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union sfp_diag_flag_u reg_val;
	sw_error_t ret = SW_OK;

	ret = sfp_diag_flag_get(dev_id, index, &reg_val);
	*value = reg_val.bf.tmp_high_warning;
	return ret;
}

sw_error_t
sfp_diag_flag_tx_pwr_high_warning_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union sfp_diag_flag_u reg_val;
	sw_error_t ret = SW_OK;

	ret = sfp_diag_flag_get(dev_id, index, &reg_val);
	*value = reg_val.bf.tx_pwr_high_warning;
	return ret;
}

sw_error_t
sfp_diag_flag_rx_pwr_high_alarm_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union sfp_diag_flag_u reg_val;
	sw_error_t ret = SW_OK;

	ret = sfp_diag_flag_get(dev_id, index, &reg_val);
	*value = reg_val.bf.rx_pwr_high_alarm;
	return ret;
}

sw_error_t
sfp_diag_flag_tx_pwr_low_warning_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union sfp_diag_flag_u reg_val;
	sw_error_t ret = SW_OK;

	ret = sfp_diag_flag_get(dev_id, index, &reg_val);
	*value = reg_val.bf.tx_pwr_low_warning;
	return ret;
}

sw_error_t
sfp_diag_extended_ctrl_status_unallocated_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union sfp_diag_extended_ctrl_status_u reg_val;
	sw_error_t ret = SW_OK;

	ret = sfp_diag_extended_ctrl_status_get(dev_id, index, &reg_val);
	*value = reg_val.bf.unallocated_0 << 8
		| reg_val.bf.unallocated_1;
	return ret;
}

sw_error_t
sfp_diag_extended_ctrl_status_pwr_level_op_state_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union sfp_diag_extended_ctrl_status_u reg_val;
	sw_error_t ret = SW_OK;

	ret = sfp_diag_extended_ctrl_status_get(dev_id, index, &reg_val);
	*value = reg_val.bf.pwr_level_op_state;
	return ret;
}

sw_error_t
sfp_diag_extended_ctrl_status_soft_rs_sel_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union sfp_diag_extended_ctrl_status_u reg_val;
	sw_error_t ret = SW_OK;

	ret = sfp_diag_extended_ctrl_status_get(dev_id, index, &reg_val);
	*value = reg_val.bf.soft_rs_sel;
	return ret;
}

sw_error_t
sfp_diag_extended_ctrl_status_pwr_level_sel_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union sfp_diag_extended_ctrl_status_u reg_val;
	sw_error_t ret = SW_OK;

	ret = sfp_diag_extended_ctrl_status_get(dev_id, index, &reg_val);
	*value = reg_val.bf.pwr_level_sel;
	return ret;
}


/*
 * Copyright (c) 2020, The Linux Foundation. All rights reserved.
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */
#include "sw.h"
#include "hsl.h"
#include "scomphy_reg_access.h"
#include "mp_portctrl_reg.h"
#include "mp_portctrl.h"

sw_error_t
mp_mac_configuration_get(
		 a_uint32_t dev_id,
		 a_uint32_t index,
		 union mac_configuration_u *value)
{
	 if (index >= MAC_CONFIGURATION_MAX_ENTRY)
		 return SW_OUT_OF_RANGE;
	 return scomphy_reg_get(
				dev_id,
				MAC_CONFIGURATION_ADDRESS + \
				index * MAC_CONFIGURATION_INC,
				(a_uint8_t *)&value->val, 4);
}

sw_error_t
mp_mac_configuration_set(
		 a_uint32_t dev_id,
		 a_uint32_t index,
		 union mac_configuration_u *value)
{
	 if (index >= MAC_CONFIGURATION_MAX_ENTRY)
		 return SW_OUT_OF_RANGE;
	 return scomphy_reg_set(
				dev_id,
				MAC_CONFIGURATION_ADDRESS + \
				index * MAC_CONFIGURATION_INC,
				(a_uint8_t *)&value->val, 4);
}

sw_error_t
mp_mac_frame_filter_get(
		 a_uint32_t dev_id,
		 a_uint32_t index,
		 union mac_frame_filter_u *value)
{
	 if (index >= MAC_FRAME_FILTER_MAX_ENTRY)
		 return SW_OUT_OF_RANGE;
	 return scomphy_reg_get(
				dev_id,
				MAC_FRAME_FILTER_ADDRESS + \
				index * MAC_FRAME_FILTER_INC,
				(a_uint8_t *)&value->val, 4);
}

sw_error_t
mp_mac_frame_filter_set(
		 a_uint32_t dev_id,
		 a_uint32_t index,
		 union mac_frame_filter_u *value)
{
	 if (index >= MAC_FRAME_FILTER_MAX_ENTRY)
		 return SW_OUT_OF_RANGE;
	 return scomphy_reg_set(
				dev_id,
				MAC_FRAME_FILTER_ADDRESS + \
				index * MAC_FRAME_FILTER_INC,
				(a_uint8_t *)&value->val, 4);
}

sw_error_t
mp_mac_flowctrl_get(
		 a_uint32_t dev_id,
		 a_uint32_t index,
		 union mac_flow_ctrl_u *value)
{
	 if (index >= MAC_FLOW_CTRL_MAX_ENTRY)
		 return SW_OUT_OF_RANGE;
	 return scomphy_reg_get(
				dev_id,
				MAC_FLOW_CTRL_ADDRESS + \
				index * MAC_FLOW_CTRL_INC,
				(a_uint8_t *)&value->val, 4);
}

sw_error_t
mp_mac_flowctrl_set(
		 a_uint32_t dev_id,
		 a_uint32_t index,
		 union mac_flow_ctrl_u *value)
{
	 if (index >= MAC_FLOW_CTRL_MAX_ENTRY)
		 return SW_OUT_OF_RANGE;
	 return scomphy_reg_set(
				dev_id,
				MAC_FLOW_CTRL_ADDRESS + \
				index * MAC_FLOW_CTRL_INC,
				(a_uint8_t *)&value->val, 4);
}

sw_error_t
mp_mac_lpi_ctrl_status_get(
		 a_uint32_t dev_id,
		 a_uint32_t index,
		 union mac_lpi_ctrl_status_u *value)
{
	 if (index >= MAC_LPI_CTRL_STATUS_MAX_ENTRY)
		 return SW_OUT_OF_RANGE;
	 return scomphy_reg_get(
				dev_id,
				MAC_LPI_CTRL_STATUS_ADDRESS + \
				index * MAC_LPI_CTRL_STATUS_INC,
				(a_uint8_t *)&value->val, 4);
}

sw_error_t
mp_mac_lpi_ctrl_status_set(
		 a_uint32_t dev_id,
		 a_uint32_t index,
		 union mac_lpi_ctrl_status_u *value)
{
	 if (index >= MAC_LPI_CTRL_STATUS_MAX_ENTRY)
		 return SW_OUT_OF_RANGE;
	 return scomphy_reg_set(
				dev_id,
				MAC_LPI_CTRL_STATUS_ADDRESS + \
				index * MAC_LPI_CTRL_STATUS_INC,
				(a_uint8_t *)&value->val, 4);
}

sw_error_t
mp_mac_lpi_timer_ctrl_get(
		 a_uint32_t dev_id,
		 a_uint32_t index,
		 union mac_lpi_timer_ctrl_u *value)
{
	 if (index >= MAC_LPI_TIMER_CTRL_MAX_ENTRY)
		 return SW_OUT_OF_RANGE;
	 return scomphy_reg_get(
				dev_id,
				MAC_LPI_TIMER_CTRL_ADDRESS + \
				index * MAC_LPI_TIMER_CTRL_INC,
				(a_uint8_t *)&value->val, 4);
}

sw_error_t
mp_mac_lpi_timer_ctrl_set(
		 a_uint32_t dev_id,
		 a_uint32_t index,
		 union mac_lpi_timer_ctrl_u *value)
{
	 if (index >= MAC_LPI_TIMER_CTRL_MAX_ENTRY)
		 return SW_OUT_OF_RANGE;
	 return scomphy_reg_set(
				dev_id,
				MAC_LPI_TIMER_CTRL_ADDRESS + \
				index * MAC_LPI_TIMER_CTRL_INC,
				(a_uint8_t *)&value->val, 4);
}

sw_error_t
mp_mac_max_frame_ctrl_get(
		 a_uint32_t dev_id,
		 a_uint32_t index,
		 union mac_max_frame_ctrl_u *value)
{
	 if (index >= MAC_MAX_FRAME_CTRL_MAX_ENTRY)
		 return SW_OUT_OF_RANGE;
	 return scomphy_reg_get(
				dev_id,
				MAC_MAX_FRAME_CTRL_ADDRESS + \
				index * MAC_MAX_FRAME_CTRL_INC,
				(a_uint8_t *)&value->val, 4);
}

sw_error_t
mp_mac_max_frame_ctrl_set(
		 a_uint32_t dev_id,
		 a_uint32_t index,
		 union mac_max_frame_ctrl_u *value)
{
	 if (index >= MAC_MAX_FRAME_CTRL_MAX_ENTRY)
		 return SW_OUT_OF_RANGE;
	 return scomphy_reg_set(
				dev_id,
				MAC_MAX_FRAME_CTRL_ADDRESS + \
				index * MAC_MAX_FRAME_CTRL_INC,
				(a_uint8_t *)&value->val, 4);
}

sw_error_t
mp_mac_operation_mode_ctrl_get(
		 a_uint32_t dev_id,
		 a_uint32_t index,
		 union mac_operation_mode_ctrl_u *value)
{
	 if (index >= MAC_OPERATION_MODE_CTRL_MAX_ENTRY)
		 return SW_OUT_OF_RANGE;
	 return scomphy_reg_get(
				dev_id,
				MAC_OPERATION_MODE_CTRL_ADDRESS + \
				index * MAC_OPERATION_MODE_CTRL_INC,
				(a_uint8_t *)&value->val, 4);
}

sw_error_t
mp_mac_operation_mode_ctrl_set(
		 a_uint32_t dev_id,
		 a_uint32_t index,
		 union mac_operation_mode_ctrl_u *value)
{
	 if (index >= MAC_OPERATION_MODE_CTRL_MAX_ENTRY)
		 return SW_OUT_OF_RANGE;
	 return scomphy_reg_set(
				dev_id,
				MAC_OPERATION_MODE_CTRL_ADDRESS + \
				index * MAC_OPERATION_MODE_CTRL_INC,
				(a_uint8_t *)&value->val, 4);
}


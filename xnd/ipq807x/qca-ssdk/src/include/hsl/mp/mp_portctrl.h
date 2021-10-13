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

#ifndef MAPLE_PORTCTRL_H
#define MAPLE_PORTCTRL_H

#define MAC_CONFIGURATION_MAX_ENTRY          2
#define MAC_FRAME_FILTER_MAX_ENTRY           2
#define MAC_FLOW_CTRL_MAX_ENTRY              2
#define MAC_LPI_CTRL_STATUS_MAX_ENTRY        2
#define MAC_LPI_TIMER_CTRL_MAX_ENTRY         2
#define MAC_MAX_FRAME_CTRL_MAX_ENTRY         2
#define MAC_OPERATION_MODE_CTRL_MAX_ENTRY    2

sw_error_t
mp_mac_configuration_get(
		 a_uint32_t dev_id,
		 a_uint32_t index,
		 union mac_configuration_u *value);

sw_error_t
mp_mac_configuration_set(
		 a_uint32_t dev_id,
		 a_uint32_t index,
		 union mac_configuration_u *value);


sw_error_t
mp_mac_frame_filter_get(
		 a_uint32_t dev_id,
		 a_uint32_t index,
		 union mac_frame_filter_u *value);

sw_error_t
mp_mac_frame_filter_set(
		 a_uint32_t dev_id,
		 a_uint32_t index,
		 union mac_frame_filter_u *value);

sw_error_t
mp_mac_flowctrl_get(
		 a_uint32_t dev_id,
		 a_uint32_t index,
		 union mac_flow_ctrl_u *value);

sw_error_t
mp_mac_flowctrl_set(
		 a_uint32_t dev_id,
		 a_uint32_t index,
		 union mac_flow_ctrl_u *value);

sw_error_t
mp_mac_lpi_ctrl_status_get(
		 a_uint32_t dev_id,
		 a_uint32_t index,
		 union mac_lpi_ctrl_status_u *value);

sw_error_t
mp_mac_lpi_ctrl_status_set(
		 a_uint32_t dev_id,
		 a_uint32_t index,
		 union mac_lpi_ctrl_status_u *value);

sw_error_t
mp_mac_lpi_timer_ctrl_get(
		 a_uint32_t dev_id,
		 a_uint32_t index,
		 union mac_lpi_timer_ctrl_u *value);

sw_error_t
mp_mac_lpi_timer_ctrl_set(
		 a_uint32_t dev_id,
		 a_uint32_t index,
		 union mac_lpi_timer_ctrl_u *value);

sw_error_t
mp_mac_max_frame_ctrl_get(
		 a_uint32_t dev_id,
		 a_uint32_t index,
		 union mac_max_frame_ctrl_u *value);

sw_error_t
mp_mac_max_frame_ctrl_set(
		 a_uint32_t dev_id,
		 a_uint32_t index,
		 union mac_max_frame_ctrl_u *value);

sw_error_t
mp_mac_operation_mode_ctrl_get(
		 a_uint32_t dev_id,
		 a_uint32_t index,
		 union mac_operation_mode_ctrl_u *value);

sw_error_t
mp_mac_operation_mode_ctrl_set(
		 a_uint32_t dev_id,
		 a_uint32_t index,
		 union mac_operation_mode_ctrl_u *value);
#endif


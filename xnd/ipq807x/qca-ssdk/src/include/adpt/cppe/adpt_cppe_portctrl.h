/*
 * Copyright (c) 2018-2019, The Linux Foundation. All rights reserved.
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
#ifndef _ADPT_CPPE_PORTCTRLH_
#define _ADPT_CPPE_PORTCTRLH_

#ifdef __cplusplus
extern "C" {
#endif                          /* __cplusplus */

#define CPPE_PORT3_PCS_SEL_PCS0_CHANNEL2          0x0
#define CPPE_PORT3_PCS_SEL_PCS0_CHANNEL4          0x1
#define CPPE_PORT4_PCS_SEL_PCS0_CHANNEL3          0x0
#define CPPE_PORT4_PCS_SEL_PCS0_SGMIIPLUS         0x1
#define CPPE_PORT5_PCS_SEL_PCS0_CHANNEL4          0x0
#define CPPE_PORT5_PCS_SEL_PCS1_CHANNEL0          0x1
#define CPPE_PORT5_GMAC_SEL_GMAC                  0x0
#define CPPE_PORT5_GMAC_SEL_XGMAC                 0x1
#define CPPE_PCS0_CHANNEL4_SEL_PORT5_CLOCK        0x0
#define CPPE_PCS0_CHANNEL4_SEL_PORT3_CLOCK        0x1
#define CPPE_PCS0_CHANNEL0_SEL_PSGMII             0x0
#define CPPE_PCS0_CHANNEL0_SEL_SGMIIPLUS          0x1
#define CPPE_DETECTION_PHY_FAILURE                0xFFFF
#define CPPE_LOOPBACK_PORT_RATE_FREQUENCY         300  /* 300MHZ*/
#define CPPE_LOOPBACK_PORT_NUM                    0x1

sw_error_t
_adpt_cppe_port_mux_mac_set(a_uint32_t dev_id, fal_port_t port_id,
		a_uint32_t port_type);
sw_error_t
adpt_cppe_port_mru_set(a_uint32_t dev_id, fal_port_t port_id,
		fal_mru_ctrl_t *ctrl);
sw_error_t
adpt_cppe_port_mru_get(a_uint32_t dev_id, fal_port_t port_id,
		fal_mru_ctrl_t *ctrl);
#ifndef IN_PORTCONTROL_MINI
sw_error_t
adpt_cppe_port_mtu_set(a_uint32_t dev_id, fal_port_t port_id,
		fal_mtu_ctrl_t *ctrl);
sw_error_t
adpt_cppe_port_mtu_get(a_uint32_t dev_id, fal_port_t port_id,
		fal_mtu_ctrl_t *ctrl);
sw_error_t
adpt_cppe_port_source_filter_set(a_uint32_t dev_id,
		fal_port_t port_id, a_bool_t enable);
sw_error_t
adpt_cppe_port_source_filter_get(a_uint32_t dev_id,
		fal_port_t port_id, a_bool_t * enable);
sw_error_t
adpt_cppe_port_source_filter_config_set(
		a_uint32_t dev_id, fal_port_t port_id,
		fal_src_filter_config_t *src_filter_config);
sw_error_t
adpt_cppe_port_source_filter_config_get
		(a_uint32_t dev_id, fal_port_t port_id,
		fal_src_filter_config_t* src_filter_config);
#endif
sw_error_t
adpt_cppe_port_to_channel_convert(a_uint32_t dev_id,
		a_uint32_t port_id, a_uint32_t *channel_id);
sw_error_t
adpt_cppe_switch_port_loopback_set(a_uint32_t dev_id,
	fal_port_t port_id, fal_loopback_config_t *loopback_cfg);

sw_error_t
adpt_cppe_switch_port_loopback_get(a_uint32_t dev_id,
	fal_port_t port_id, fal_loopback_config_t *loopback_cfg);

sw_error_t
adpt_cppe_switch_port_loopback_flowctrl_set(a_uint32_t dev_id,
	fal_port_t port_id, a_bool_t enable);

sw_error_t
adpt_cppe_switch_port_loopback_flowctrl_get(a_uint32_t dev_id,
	fal_port_t port_id, a_bool_t *enable);

sw_error_t
adpt_cppe_lpbk_max_frame_size_get(a_uint32_t dev_id, fal_port_t port_id,
	a_uint32_t *max_frame);

sw_error_t
adpt_cppe_lpbk_max_frame_size_set(a_uint32_t dev_id, fal_port_t port_id,
	a_uint32_t max_frame);
#ifdef __cplusplus
}
#endif                          /* __cplusplus */
#endif

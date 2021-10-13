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
#include "sw.h"
#include "hppe_global_reg.h"
#include "hppe_global.h"
#include "hppe_portctrl_reg.h"
#include "hppe_portctrl.h"
#include "cppe_portctrl_reg.h"
#include "cppe_portctrl.h"
#include "hppe_fdb_reg.h"
#include "hppe_fdb.h"
#include "cppe_loopback_reg.h"
#include "cppe_loopback.h"
#include "hsl.h"
#include "hsl_dev.h"
#include "hsl_phy.h"
#include "hsl_port_prop.h"
#include "hppe_init.h"
#include "adpt.h"
#include "adpt_hppe.h"
#include "adpt_cppe_portctrl.h"

sw_error_t
_adpt_cppe_port_mux_mac_set(a_uint32_t dev_id, fal_port_t port_id,
	a_uint32_t port_type)
{
	sw_error_t rv = SW_OK;
	a_uint32_t mode0, mode1;
	union cppe_port_mux_ctrl_u cppe_port_mux_ctrl;

	ADPT_DEV_ID_CHECK(dev_id);
	memset(&cppe_port_mux_ctrl, 0, sizeof(cppe_port_mux_ctrl));

	rv = cppe_port_mux_ctrl_get(dev_id, &cppe_port_mux_ctrl);
	SW_RTN_ON_ERROR (rv);

	mode0 = ssdk_dt_global_get_mac_mode(dev_id, SSDK_UNIPHY_INSTANCE0);
	mode1 = ssdk_dt_global_get_mac_mode(dev_id, SSDK_UNIPHY_INSTANCE1);

	switch (port_id) {
		case SSDK_PHYSICAL_PORT3:
		case SSDK_PHYSICAL_PORT4:
			if (mode0 == PORT_WRAPPER_PSGMII) {
				if (hsl_port_phyid_get(dev_id,
					SSDK_PHYSICAL_PORT3) == MALIBU2PORT_PHY) {
					cppe_port_mux_ctrl.bf.port3_pcs_sel =
						CPPE_PORT3_PCS_SEL_PCS0_CHANNEL4;
					cppe_port_mux_ctrl.bf.port4_pcs_sel =
						CPPE_PORT4_PCS_SEL_PCS0_CHANNEL3;
				} else {
					cppe_port_mux_ctrl.bf.port3_pcs_sel =
						CPPE_PORT3_PCS_SEL_PCS0_CHANNEL2;
					cppe_port_mux_ctrl.bf.port4_pcs_sel =
						CPPE_PORT4_PCS_SEL_PCS0_CHANNEL3;
				}
			} else if (mode0 == PORT_WRAPPER_QSGMII) {
				cppe_port_mux_ctrl.bf.port3_pcs_sel =
					CPPE_PORT3_PCS_SEL_PCS0_CHANNEL2;
				cppe_port_mux_ctrl.bf.port4_pcs_sel =
					CPPE_PORT4_PCS_SEL_PCS0_CHANNEL3;
			} else if (mode0 == PORT_WRAPPER_SGMII_PLUS) {
				cppe_port_mux_ctrl.bf.port3_pcs_sel =
					CPPE_PORT3_PCS_SEL_PCS0_CHANNEL2;
				cppe_port_mux_ctrl.bf.port4_pcs_sel =
					CPPE_PORT4_PCS_SEL_PCS0_SGMIIPLUS;
			} else if (mode0 ==PORT_WRAPPER_SGMII_CHANNEL0) {
				if (hsl_port_prop_check(dev_id, SSDK_PHYSICAL_PORT4,
					HSL_PP_EXCL_CPU) == A_TRUE) {
					cppe_port_mux_ctrl.bf.port3_pcs_sel =
						CPPE_PORT3_PCS_SEL_PCS0_CHANNEL2;
					cppe_port_mux_ctrl.bf.port4_pcs_sel =
						CPPE_PORT4_PCS_SEL_PCS0_SGMIIPLUS;
				} else {
					cppe_port_mux_ctrl.bf.port3_pcs_sel =
						CPPE_PORT3_PCS_SEL_PCS0_CHANNEL2;
					cppe_port_mux_ctrl.bf.port4_pcs_sel =
						CPPE_PORT4_PCS_SEL_PCS0_CHANNEL3;
				}
			} else if ((mode0 == PORT_WRAPPER_SGMII_CHANNEL4) ||
				(mode0 == PORT_WRAPPER_SGMII0_RGMII4)) {
				cppe_port_mux_ctrl.bf.port3_pcs_sel =
					CPPE_PORT3_PCS_SEL_PCS0_CHANNEL2;
				cppe_port_mux_ctrl.bf.port4_pcs_sel =
					CPPE_PORT4_PCS_SEL_PCS0_CHANNEL3;
				cppe_port_mux_ctrl.bf.port5_pcs_sel =
					CPPE_PORT5_PCS_SEL_PCS0_CHANNEL4;
				cppe_port_mux_ctrl.bf.port5_gmac_sel =
					CPPE_PORT5_GMAC_SEL_GMAC;
			} else if ((mode0 == PORT_WRAPPER_SGMII_CHANNEL1) ||
				(mode0 == PORT_WRAPPER_SGMII1_RGMII4)) {
				cppe_port_mux_ctrl.bf.port3_pcs_sel =
					CPPE_PORT3_PCS_SEL_PCS0_CHANNEL2;
				cppe_port_mux_ctrl.bf.port4_pcs_sel =
					CPPE_PORT4_PCS_SEL_PCS0_CHANNEL3;
			}
			break;
		case SSDK_PHYSICAL_PORT5:
			if (mode0 == PORT_WRAPPER_PSGMII) {
				if (hsl_port_phyid_get(dev_id,
					SSDK_PHYSICAL_PORT3) != MALIBU2PORT_PHY) {
					cppe_port_mux_ctrl.bf.port5_pcs_sel =
						CPPE_PORT5_PCS_SEL_PCS0_CHANNEL4;
					cppe_port_mux_ctrl.bf.port5_gmac_sel =
						CPPE_PORT5_GMAC_SEL_GMAC;
				}
			}
			if ((mode1 == PORT_WRAPPER_SGMII_PLUS) ||
				(mode1 == PORT_WRAPPER_SGMII0_RGMII4) ||
				(mode1 == PORT_WRAPPER_SGMII_CHANNEL0) ||
				(mode1 == PORT_WRAPPER_SGMII_FIBER)) {
				cppe_port_mux_ctrl.bf.port5_pcs_sel =
					CPPE_PORT5_PCS_SEL_PCS1_CHANNEL0;
				cppe_port_mux_ctrl.bf.port5_gmac_sel =
					CPPE_PORT5_GMAC_SEL_GMAC;
			} else if ((mode1 == PORT_WRAPPER_USXGMII) ||
				(mode1 == PORT_WRAPPER_10GBASE_R)) {
				cppe_port_mux_ctrl.bf.port5_pcs_sel =
					CPPE_PORT5_PCS_SEL_PCS1_CHANNEL0;
				cppe_port_mux_ctrl.bf.port5_gmac_sel =
					CPPE_PORT5_GMAC_SEL_XGMAC;
			}
			break;
		default:
			break;
	}

	rv = cppe_port_mux_ctrl_set(dev_id, &cppe_port_mux_ctrl);

	return rv;
}

sw_error_t
adpt_cppe_port_mru_set(a_uint32_t dev_id, fal_port_t port_id,
		fal_mru_ctrl_t *ctrl)
{
	sw_error_t rv = SW_OK;
	union cppe_mru_mtu_ctrl_tbl_u mru_mtu_ctrl_tbl;

	memset(&mru_mtu_ctrl_tbl, 0, sizeof(mru_mtu_ctrl_tbl));
	ADPT_DEV_ID_CHECK(dev_id);
	ADPT_NULL_POINT_CHECK(ctrl);

	rv = cppe_mru_mtu_ctrl_tbl_get(dev_id, port_id, &mru_mtu_ctrl_tbl);
	SW_RTN_ON_ERROR (rv);

	mru_mtu_ctrl_tbl.bf.mru = ctrl->mru_size;
	mru_mtu_ctrl_tbl.bf.mru_cmd = (a_uint32_t)ctrl->action;
	rv = cppe_mru_mtu_ctrl_tbl_set(dev_id, port_id, &mru_mtu_ctrl_tbl);

	return rv;
}

sw_error_t
adpt_cppe_port_mru_get(a_uint32_t dev_id, fal_port_t port_id,
		fal_mru_ctrl_t *ctrl)
{
	sw_error_t rv = SW_OK;
	union cppe_mru_mtu_ctrl_tbl_u mru_mtu_ctrl_tbl;

	memset(&mru_mtu_ctrl_tbl, 0, sizeof(mru_mtu_ctrl_tbl));
	ADPT_DEV_ID_CHECK(dev_id);
	ADPT_NULL_POINT_CHECK(ctrl);

	rv = cppe_mru_mtu_ctrl_tbl_get(dev_id, port_id, &mru_mtu_ctrl_tbl);
	SW_RTN_ON_ERROR (rv);

	ctrl->mru_size = mru_mtu_ctrl_tbl.bf.mru;
	ctrl->action = (fal_fwd_cmd_t)mru_mtu_ctrl_tbl.bf.mru_cmd;

	return SW_OK;
}

#ifndef IN_PORTCONTROL_MINI
sw_error_t
adpt_cppe_port_mtu_set(a_uint32_t dev_id, fal_port_t port_id,
		fal_mtu_ctrl_t *ctrl)
{
	sw_error_t rv = SW_OK;
	union cppe_mru_mtu_ctrl_tbl_u mru_mtu_ctrl_tbl;

	memset(&mru_mtu_ctrl_tbl, 0, sizeof(mru_mtu_ctrl_tbl));
	ADPT_DEV_ID_CHECK(dev_id);
	ADPT_NULL_POINT_CHECK(ctrl);

	rv = cppe_mru_mtu_ctrl_tbl_get(dev_id, port_id, &mru_mtu_ctrl_tbl);
	SW_RTN_ON_ERROR (rv);

	mru_mtu_ctrl_tbl.bf.mtu = ctrl->mtu_size;
	mru_mtu_ctrl_tbl.bf.mtu_cmd = (a_uint32_t)ctrl->action;
	rv = cppe_mru_mtu_ctrl_tbl_set(dev_id, port_id, &mru_mtu_ctrl_tbl);
	SW_RTN_ON_ERROR (rv);

	if ((port_id >= SSDK_PHYSICAL_PORT0) && (port_id <= SSDK_PHYSICAL_PORT7))
	{
		rv = hppe_mc_mtu_ctrl_tbl_mtu_set(dev_id, port_id, ctrl->mtu_size);
		SW_RTN_ON_ERROR (rv);
		rv = hppe_mc_mtu_ctrl_tbl_mtu_cmd_set(dev_id, port_id, (a_uint32_t)ctrl->action);
		SW_RTN_ON_ERROR (rv);
	}

	return rv;
}

sw_error_t
adpt_cppe_port_mtu_get(a_uint32_t dev_id, fal_port_t port_id,
		fal_mtu_ctrl_t *ctrl)
{
	sw_error_t rv = SW_OK;
	union cppe_mru_mtu_ctrl_tbl_u mru_mtu_ctrl_tbl;

	memset(&mru_mtu_ctrl_tbl, 0, sizeof(mru_mtu_ctrl_tbl));
	ADPT_DEV_ID_CHECK(dev_id);
	ADPT_NULL_POINT_CHECK(ctrl);

	rv = cppe_mru_mtu_ctrl_tbl_get(dev_id, port_id, &mru_mtu_ctrl_tbl);
	SW_RTN_ON_ERROR (rv);

	ctrl->mtu_size = mru_mtu_ctrl_tbl.bf.mtu;
	ctrl->action = (fal_fwd_cmd_t)mru_mtu_ctrl_tbl.bf.mtu_cmd;

	return SW_OK;
}
#endif

sw_error_t
adpt_cppe_port_to_channel_convert(a_uint32_t dev_id, a_uint32_t port_id,
	a_uint32_t *channel_id)
{
	ADPT_DEV_ID_CHECK(dev_id);
	ADPT_NULL_POINT_CHECK(channel_id);

	*channel_id = port_id;

	if (port_id == SSDK_PHYSICAL_PORT3) {
		if (hsl_port_phyid_get(dev_id,
				port_id) == MALIBU2PORT_PHY) {
			*channel_id = SSDK_PHYSICAL_PORT5;
		}
	}
	return SW_OK;
}

#ifndef IN_PORTCONTROL_MINI
sw_error_t
adpt_cppe_port_source_filter_set(a_uint32_t dev_id,
	fal_port_t port_id, a_bool_t enable)
{
	sw_error_t rv = SW_OK;
	fal_src_filter_config_t src_filter_config;

	ADPT_DEV_ID_CHECK(dev_id);

	rv = adpt_cppe_port_source_filter_config_get(dev_id,
			port_id, &src_filter_config);
	SW_RTN_ON_ERROR(rv);
	src_filter_config.src_filter_enable = enable;
	rv = adpt_cppe_port_source_filter_config_set(dev_id, port_id,
			&src_filter_config);

	return rv;
}

sw_error_t
adpt_cppe_port_source_filter_get(a_uint32_t dev_id,
	fal_port_t port_id, a_bool_t *enable)
{
	sw_error_t rv = SW_OK;
	fal_src_filter_config_t src_filter_config;

	ADPT_DEV_ID_CHECK(dev_id);
	ADPT_NULL_POINT_CHECK(enable);

	rv = adpt_cppe_port_source_filter_config_get(dev_id, port_id,
			&src_filter_config);
	SW_RTN_ON_ERROR(rv);
	*enable = src_filter_config.src_filter_enable;

	return rv;
}

sw_error_t
adpt_cppe_port_source_filter_config_set(a_uint32_t dev_id,
	fal_port_t port_id, fal_src_filter_config_t *src_filter_config)
{
	sw_error_t rv = SW_OK;
	a_bool_t src_filter_bypass;

	ADPT_DEV_ID_CHECK(dev_id);
	ADPT_NULL_POINT_CHECK(src_filter_config);

	port_id = FAL_PORT_ID_VALUE(port_id);
	if(src_filter_config->src_filter_enable == A_TRUE)
	{
		src_filter_bypass = A_FALSE;
	}
	else
	{
		src_filter_bypass = A_TRUE;
	}
	rv = cppe_mru_mtu_ctrl_tbl_source_filter_set(dev_id, port_id,
			src_filter_bypass);
	SW_RTN_ON_ERROR(rv);
	rv = cppe_mru_mtu_ctrl_tbl_source_filter_mode_set(dev_id, port_id,
			src_filter_config->src_filter_mode);

	return rv;
}

sw_error_t
adpt_cppe_port_source_filter_config_get(a_uint32_t dev_id,
	fal_port_t port_id, fal_src_filter_config_t *src_filter_config)
{
	sw_error_t rv = SW_OK;
	a_bool_t src_filter_bypass;

	ADPT_DEV_ID_CHECK(dev_id);
	ADPT_NULL_POINT_CHECK(src_filter_config);

	port_id = FAL_PORT_ID_VALUE(port_id);
	rv = cppe_mru_mtu_ctrl_tbl_source_filter_get(dev_id, port_id,
			&src_filter_bypass);
	SW_RTN_ON_ERROR(rv);
	if(src_filter_bypass == A_TRUE)
	{
		src_filter_config->src_filter_enable = A_FALSE;
	}
	else
	{
		src_filter_config->src_filter_enable = A_TRUE;
	}

	rv = cppe_mru_mtu_ctrl_tbl_source_filter_mode_get(dev_id,
			port_id, &(src_filter_config->src_filter_mode));

	return rv;
}
#endif

static a_uint32_t port_loopback_rate[SW_MAX_NR_DEV][CPPE_LOOPBACK_PORT_NUM] = {
	{14},
	{14},
	{14},
}; /* unit is Mpps*/

sw_error_t
adpt_cppe_switch_port_loopback_set(a_uint32_t dev_id, fal_port_t port_id,
	fal_loopback_config_t *loopback_cfg)
{
	sw_error_t rv = SW_OK;
	union lpbk_enable_u loopback_cfg_tbl;
	union lpbk_pps_ctrl_u loopback_rate_ctrl_tbl;
	union port_bridge_ctrl_u port_bridge_ctrl;
	a_uint32_t physical_port = 0;

	if (port_id != SSDK_PHYSICAL_PORT6) {
		return SW_BAD_PARAM;
	}

	memset(&loopback_cfg_tbl, 0, sizeof(loopback_cfg_tbl));
	memset(&loopback_rate_ctrl_tbl, 0, sizeof(loopback_rate_ctrl_tbl));
	memset(&port_bridge_ctrl, 0, sizeof(port_bridge_ctrl));
	ADPT_DEV_ID_CHECK(dev_id);
	ADPT_NULL_POINT_CHECK(loopback_cfg);

	physical_port = port_id;
	rv = hppe_port_bridge_ctrl_get(dev_id, physical_port, &port_bridge_ctrl);
	SW_RTN_ON_ERROR (rv);
	port_bridge_ctrl.bf.txmac_en = loopback_cfg->enable;

	port_id = HPPE_TO_GMAC_PORT_ID(port_id);
	rv = cppe_lpbk_pps_ctrl_get(dev_id, port_id, &loopback_rate_ctrl_tbl);
	SW_RTN_ON_ERROR (rv);

	loopback_rate_ctrl_tbl.bf.lpbk_pps_threshold =
		CPPE_LOOPBACK_PORT_RATE_FREQUENCY / loopback_cfg->loopback_rate;

	rv = cppe_lpbk_enable_get(dev_id, port_id, &loopback_cfg_tbl);
	SW_RTN_ON_ERROR (rv);
	loopback_cfg_tbl.bf.lpbk_en = loopback_cfg->enable;
	loopback_cfg_tbl.bf.crc_strip_en = loopback_cfg->crc_stripped;

	if (loopback_cfg->enable == A_TRUE) {
		rv = cppe_lpbk_pps_ctrl_set(dev_id, port_id, &loopback_rate_ctrl_tbl);
		SW_RTN_ON_ERROR (rv);
		rv = cppe_lpbk_enable_set(dev_id, port_id, &loopback_cfg_tbl);
		SW_RTN_ON_ERROR (rv);
		msleep(100);
		rv = hppe_port_bridge_ctrl_set(dev_id, physical_port,
			&port_bridge_ctrl);
		SW_RTN_ON_ERROR (rv);
	} else {
		rv = hppe_port_bridge_ctrl_set(dev_id, physical_port,
			&port_bridge_ctrl);
		SW_RTN_ON_ERROR (rv);
		msleep(100);
		rv = cppe_lpbk_pps_ctrl_set(dev_id, port_id, &loopback_rate_ctrl_tbl);
		SW_RTN_ON_ERROR (rv);
		rv = cppe_lpbk_enable_set(dev_id, port_id, &loopback_cfg_tbl);
		SW_RTN_ON_ERROR (rv);
	}

	port_loopback_rate[dev_id][CPPE_LOOPBACK_PORT_NUM - 1] =
		loopback_cfg->loopback_rate;
	return rv;
}

sw_error_t
adpt_cppe_switch_port_loopback_get(a_uint32_t dev_id, fal_port_t port_id,
	fal_loopback_config_t *loopback_cfg)
{
	sw_error_t rv = SW_OK;
	union lpbk_enable_u loopback_cfg_tbl;
	union lpbk_pps_ctrl_u loopback_rate_ctrl_tbl;

	if (port_id != SSDK_PHYSICAL_PORT6) {
		return SW_BAD_PARAM;
	}

	memset(&loopback_cfg_tbl, 0, sizeof(loopback_cfg_tbl));
	memset(&loopback_rate_ctrl_tbl, 0, sizeof(loopback_rate_ctrl_tbl));
	ADPT_DEV_ID_CHECK(dev_id);
	ADPT_NULL_POINT_CHECK(loopback_cfg);

	port_id = HPPE_TO_GMAC_PORT_ID(port_id);
	rv = cppe_lpbk_enable_get(dev_id, port_id, &loopback_cfg_tbl);
	SW_RTN_ON_ERROR (rv);
	rv = cppe_lpbk_pps_ctrl_get(dev_id, port_id, &loopback_rate_ctrl_tbl);
	SW_RTN_ON_ERROR (rv);

	loopback_cfg->enable = loopback_cfg_tbl.bf.lpbk_en;
	loopback_cfg->crc_stripped = loopback_cfg_tbl.bf.crc_strip_en;
	loopback_cfg->loopback_rate =
		port_loopback_rate[dev_id][CPPE_LOOPBACK_PORT_NUM - 1];

	return rv;
}

sw_error_t
adpt_cppe_switch_port_loopback_flowctrl_set(a_uint32_t dev_id,
	fal_port_t port_id, a_bool_t enable)
{
	sw_error_t rv = SW_OK;
	union lpbk_enable_u loopback_cfg_tbl;

	memset(&loopback_cfg_tbl, 0, sizeof(loopback_cfg_tbl));
	ADPT_DEV_ID_CHECK(dev_id);

	if (port_id != SSDK_PHYSICAL_PORT6) {
		return SW_BAD_PARAM;
	}

	port_id = HPPE_TO_GMAC_PORT_ID(port_id);
	rv = cppe_lpbk_enable_get(dev_id, port_id, &loopback_cfg_tbl);
	SW_RTN_ON_ERROR (rv);
	loopback_cfg_tbl.bf.flowctrl_en = enable;
	rv = cppe_lpbk_enable_set(dev_id, port_id, &loopback_cfg_tbl);

	return rv;
}

sw_error_t
adpt_cppe_switch_port_loopback_flowctrl_get(a_uint32_t dev_id,
	fal_port_t port_id, a_bool_t *enable)
{
	sw_error_t rv = SW_OK;
	union lpbk_enable_u loopback_cfg_tbl;

	memset(&loopback_cfg_tbl, 0, sizeof(loopback_cfg_tbl));
	ADPT_DEV_ID_CHECK(dev_id);
	ADPT_NULL_POINT_CHECK(enable);

	if (port_id != SSDK_PHYSICAL_PORT6) {
		return SW_BAD_PARAM;
	}

	port_id = HPPE_TO_GMAC_PORT_ID(port_id);
	rv = cppe_lpbk_enable_get(dev_id, port_id, &loopback_cfg_tbl);
	SW_RTN_ON_ERROR (rv);
	*enable = loopback_cfg_tbl.bf.flowctrl_en;

	return rv;
}

sw_error_t
adpt_cppe_lpbk_max_frame_size_get(a_uint32_t dev_id, fal_port_t port_id,
	a_uint32_t *max_frame)
{
	sw_error_t rv = SW_OK;
	union lpbk_mac_junmo_size_u lpbk_mac_junmo_size;

	ADPT_DEV_ID_CHECK(dev_id);
	ADPT_NULL_POINT_CHECK(max_frame);
	port_id = HPPE_TO_GMAC_PORT_ID(port_id);
	rv = cppe_lpbk_mac_junmo_size_get(dev_id, port_id, &lpbk_mac_junmo_size);
	SW_RTN_ON_ERROR(rv);
	*max_frame = lpbk_mac_junmo_size.bf.lpbk_mac_jumbo_size;

	return rv;
}

sw_error_t
adpt_cppe_lpbk_max_frame_size_set(a_uint32_t dev_id, fal_port_t port_id,
	a_uint32_t max_frame)
{
	sw_error_t rv = SW_OK;
	union lpbk_mac_junmo_size_u lpbk_mac_junmo_size;

	ADPT_DEV_ID_CHECK(dev_id);
	memset(&lpbk_mac_junmo_size, 0, sizeof(lpbk_mac_junmo_size));
	if (max_frame > SSDK_MAX_FRAME_SIZE)
	{
		return SW_BAD_VALUE;
	}
	port_id = HPPE_TO_GMAC_PORT_ID(port_id);
	rv = cppe_lpbk_mac_junmo_size_get(dev_id, port_id, &lpbk_mac_junmo_size);
	SW_RTN_ON_ERROR(rv);
	lpbk_mac_junmo_size.bf.lpbk_mac_jumbo_size = max_frame;
	rv = cppe_lpbk_mac_junmo_size_set(dev_id, port_id, &lpbk_mac_junmo_size);

	return rv;
}
/**
 * @}
 */

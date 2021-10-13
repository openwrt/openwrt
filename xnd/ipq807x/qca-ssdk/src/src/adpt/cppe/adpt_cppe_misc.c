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
#include "hppe_portctrl_reg.h"
#include "hppe_portctrl.h"
#include "hppe_portvlan_reg.h"
#include "hppe_portvlan.h"
#include "cppe_portctrl_reg.h"
#include "cppe_portctrl.h"
#include "adpt.h"

sw_error_t
adpt_cppe_debug_port_counter_enable(a_uint32_t dev_id, fal_port_t port_id,
		fal_counter_en_t *cnt_en)
{
	union cppe_mru_mtu_ctrl_tbl_u mru_mtu_ctrl_tbl;
	union mc_mtu_ctrl_tbl_u mc_mtu_ctrl_tbl;
	union port_eg_vlan_u port_eg_vlan;
	sw_error_t rv = SW_OK;

	ADPT_DEV_ID_CHECK(dev_id);
	ADPT_NULL_POINT_CHECK(cnt_en);

	port_id = FAL_PORT_ID_VALUE(port_id);

	if (port_id < SSDK_MAX_PORT_NUM) {
		rv = cppe_mru_mtu_ctrl_tbl_get(dev_id, port_id, &mru_mtu_ctrl_tbl);
		SW_RTN_ON_ERROR(rv);
		rv = hppe_mc_mtu_ctrl_tbl_get(dev_id, port_id, &mc_mtu_ctrl_tbl);
		SW_RTN_ON_ERROR(rv);
		rv = hppe_port_eg_vlan_get(dev_id, port_id, &port_eg_vlan);
		SW_RTN_ON_ERROR(rv);

		mru_mtu_ctrl_tbl.bf.rx_cnt_en = cnt_en->rx_counter_en;
		mru_mtu_ctrl_tbl.bf.tx_cnt_en = cnt_en->vp_uni_tx_counter_en;
		mc_mtu_ctrl_tbl.bf.tx_cnt_en = cnt_en->port_mc_tx_counter_en;
		port_eg_vlan.bf.tx_counting_en = cnt_en->port_tx_counter_en;

		rv = cppe_mru_mtu_ctrl_tbl_set(dev_id, port_id, &mru_mtu_ctrl_tbl);
		SW_RTN_ON_ERROR(rv);
		rv = hppe_mc_mtu_ctrl_tbl_set(dev_id, port_id, &mc_mtu_ctrl_tbl);
		SW_RTN_ON_ERROR(rv);
		rv = hppe_port_eg_vlan_set(dev_id, port_id, &port_eg_vlan);
		SW_RTN_ON_ERROR(rv);
	} else if (port_id >= SSDK_MAX_PORT_NUM &&
			port_id < SSDK_MAX_VIRTUAL_PORT_NUM) {
		rv = cppe_mru_mtu_ctrl_tbl_get(dev_id, port_id, &mru_mtu_ctrl_tbl);
		SW_RTN_ON_ERROR(rv);

		mru_mtu_ctrl_tbl.bf.rx_cnt_en = cnt_en->rx_counter_en;
		mru_mtu_ctrl_tbl.bf.tx_cnt_en = cnt_en->vp_uni_tx_counter_en;

		rv = cppe_mru_mtu_ctrl_tbl_set(dev_id, port_id, &mru_mtu_ctrl_tbl);
		SW_RTN_ON_ERROR(rv);
	} else {
		return SW_OUT_OF_RANGE;
	}

	return rv;
}

sw_error_t
adpt_cppe_debug_port_counter_status_get(a_uint32_t dev_id, fal_port_t port_id,
		fal_counter_en_t *cnt_en)
{
	union cppe_mru_mtu_ctrl_tbl_u mru_mtu_ctrl_tbl;
	union mc_mtu_ctrl_tbl_u mc_mtu_ctrl_tbl;
	union port_eg_vlan_u port_eg_vlan;
	sw_error_t rv = SW_OK;

	ADPT_DEV_ID_CHECK(dev_id);
	ADPT_NULL_POINT_CHECK(cnt_en);

	port_id = FAL_PORT_ID_VALUE(port_id);

	if (port_id < SSDK_MAX_PORT_NUM) {
		rv = cppe_mru_mtu_ctrl_tbl_get(dev_id, port_id, &mru_mtu_ctrl_tbl);
		SW_RTN_ON_ERROR(rv);
		rv = hppe_mc_mtu_ctrl_tbl_get(dev_id, port_id, &mc_mtu_ctrl_tbl);
		SW_RTN_ON_ERROR(rv);
		rv = hppe_port_eg_vlan_get(dev_id, port_id, &port_eg_vlan);
		SW_RTN_ON_ERROR(rv);

		cnt_en->rx_counter_en = mru_mtu_ctrl_tbl.bf.rx_cnt_en;
		cnt_en->vp_uni_tx_counter_en = mru_mtu_ctrl_tbl.bf.tx_cnt_en;
		cnt_en->port_mc_tx_counter_en = mc_mtu_ctrl_tbl.bf.tx_cnt_en;
		cnt_en->port_tx_counter_en = port_eg_vlan.bf.tx_counting_en;
	} else if (port_id >= SSDK_MAX_PORT_NUM &&
			port_id < SSDK_MAX_VIRTUAL_PORT_NUM) {
		rv = cppe_mru_mtu_ctrl_tbl_get(dev_id, port_id, &mru_mtu_ctrl_tbl);
		SW_RTN_ON_ERROR(rv);

		cnt_en->rx_counter_en = mru_mtu_ctrl_tbl.bf.rx_cnt_en;
		cnt_en->vp_uni_tx_counter_en = mru_mtu_ctrl_tbl.bf.tx_cnt_en;
	} else {
		return SW_OUT_OF_RANGE;
	}

	return rv;
}

/**
 * @}
 */

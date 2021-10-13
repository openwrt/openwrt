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
#include "mp_portctrl_reg.h"
#include "mp_portctrl.h"
#include "adpt.h"
#include "adpt_mp.h"
#include "adpt_mp_portctrl.h"
#include "adpt_mp_uniphy.h"
#include "hsl_port_prop.h"
#include "hsl_phy.h"
#include "ssdk_dts.h"
#include "ssdk_clk.h"

static a_uint32_t port_lpi_status[SW_MAX_NR_DEV] = {0};

static sw_error_t
_adpt_mp_gcc_mac_clock_set(a_uint32_t dev_id,
	a_uint32_t port_id, a_bool_t enable)
{
	sw_error_t rv = 0;

	qca_gcc_mac_port_clock_set(dev_id, port_id, enable);

	return rv;
}

static a_bool_t
_adpt_mp_port_phy_connected (a_uint32_t dev_id, fal_port_t port_id)
{
	a_bool_t force_port = 0;

	ADPT_DEV_ID_CHECK(dev_id);

	/* force port which connect s17c or other device chip*/
	force_port = ssdk_port_feature_get(dev_id, port_id, PHY_F_FORCE);
	if (force_port == A_TRUE) {
		SSDK_DEBUG("port_id %d is a force port!\n", port_id);
		return A_FALSE;
	} else {
		return A_TRUE;
	}
}

static sw_error_t
_adpt_mp_port_gcc_speed_clock_set(
	a_uint32_t dev_id,
	a_uint32_t port_id,
	fal_port_speed_t phy_speed)
{
	sw_error_t rv = 0;

	switch (phy_speed) {
		case FAL_SPEED_10:
			ssdk_port_speed_clock_set(dev_id,
					port_id, SGMII_SPEED_10M_CLK);
			break;
		case FAL_SPEED_100:
			ssdk_port_speed_clock_set(dev_id,
					port_id, SGMII_SPEED_100M_CLK);
			break;
		case FAL_SPEED_1000:
			ssdk_port_speed_clock_set(dev_id,
					port_id, SGMII_SPEED_1000M_CLK);
			break;
		case FAL_SPEED_2500:
			ssdk_port_speed_clock_set(dev_id,
					port_id, SGMII_PLUS_SPEED_2500M_CLK);
			break;
		default:
			break;
               }

	return rv;
}

static sw_error_t
adpt_mp_port_reset_set(a_uint32_t dev_id, a_uint32_t port_id)
{
	sw_error_t rv = 0;
	a_uint32_t phy_addr;
	hsl_phy_ops_t *phy_drv;

	ADPT_DEV_ID_CHECK(dev_id);

	if (port_id == SSDK_PHYSICAL_PORT1) {
		/*internal gephy reset*/
		SW_RTN_ON_NULL (phy_drv = hsl_phy_api_ops_get(dev_id,
				port_id));
		if (NULL == phy_drv->phy_function_reset)
			return SW_NOT_SUPPORTED;
		rv = hsl_port_prop_get_phyid (dev_id, port_id, &phy_addr);
		SW_RTN_ON_ERROR (rv);
		rv = phy_drv->phy_function_reset(dev_id, phy_addr, PHY_FIFO_RESET);
		SW_RTN_ON_ERROR (rv);
	} else if (port_id == SSDK_PHYSICAL_PORT2) {
		rv = adpt_mp_uniphy_adapter_port_reset(dev_id, port_id);
	} else {
		return SW_NOT_SUPPORTED;
	}

	return rv;
}

static sw_error_t
adpt_mp_port_txmac_status_set(a_uint32_t dev_id, fal_port_t port_id,
		a_bool_t enable)
{
	sw_error_t rv = SW_OK;
	a_uint32_t gmac_id = 0;
	union mac_configuration_u configuration;

	ADPT_DEV_ID_CHECK(dev_id);
	MP_PORT_ID_CHECK(port_id);

	memset(&configuration, 0, sizeof(configuration));
	gmac_id = MP_PORT_TO_GMAC_ID(port_id);

	rv = mp_mac_configuration_get(dev_id, gmac_id, &configuration);
	SW_RTN_ON_ERROR(rv);

	configuration.bf.tx_enable = enable;
	rv = mp_mac_configuration_set(dev_id, gmac_id, &configuration);

	return rv;
}

#ifndef IN_PORTCONTROL_MINI
static sw_error_t
adpt_mp_port_txmac_status_get(a_uint32_t dev_id, fal_port_t port_id,
		a_bool_t *enable)
{
	sw_error_t rv = SW_OK;
	a_uint32_t gmac_id = 0;
	union mac_configuration_u configuration;

	ADPT_DEV_ID_CHECK(dev_id);
	MP_PORT_ID_CHECK(port_id);
	ADPT_NULL_POINT_CHECK(enable);

	memset(&configuration, 0, sizeof(configuration));
	gmac_id = MP_PORT_TO_GMAC_ID(port_id);

	rv = mp_mac_configuration_get(dev_id, gmac_id, &configuration);
	SW_RTN_ON_ERROR(rv);

	*enable = configuration.bf.tx_enable;

	return rv;
}
#endif

static sw_error_t
adpt_mp_port_rxmac_status_set(a_uint32_t dev_id, fal_port_t port_id,
		a_bool_t enable)
{
	sw_error_t rv = SW_OK;
	a_uint32_t gmac_id = 0;
	union mac_configuration_u configuration;

	ADPT_DEV_ID_CHECK(dev_id);
	MP_PORT_ID_CHECK(port_id);

	memset(&configuration, 0, sizeof(configuration));
	gmac_id = MP_PORT_TO_GMAC_ID(port_id);

	rv = mp_mac_configuration_get(dev_id, gmac_id, &configuration);
	SW_RTN_ON_ERROR(rv);

	configuration.bf.rx_enable = enable;
	rv = mp_mac_configuration_set(dev_id, gmac_id, &configuration);

	return rv;
}

#ifndef IN_PORTCONTROL_MINI
static sw_error_t
adpt_mp_port_rxmac_status_get(a_uint32_t dev_id, fal_port_t port_id,
		a_bool_t *enable)
{
	sw_error_t rv = SW_OK;
	a_uint32_t gmac_id = 0;
	union mac_configuration_u configuration;

	ADPT_DEV_ID_CHECK(dev_id);
	MP_PORT_ID_CHECK(port_id);
	ADPT_NULL_POINT_CHECK(enable);

	memset(&configuration, 0, sizeof(configuration));
	gmac_id = MP_PORT_TO_GMAC_ID(port_id);

	rv = mp_mac_configuration_get(dev_id, gmac_id, &configuration);
	SW_RTN_ON_ERROR(rv);

	*enable = configuration.bf.rx_enable;

	return rv;
}
#endif

static sw_error_t
adpt_mp_port_txfc_status_set(a_uint32_t dev_id, fal_port_t port_id,
	a_bool_t enable)
{
	sw_error_t rv = SW_OK;
	a_uint32_t gmac_id = 0;
	union mac_flow_ctrl_u mac_flow_ctrl;
	struct qca_phy_priv *priv = ssdk_phy_priv_data_get(dev_id);
	union mac_operation_mode_ctrl_u mac_operation_mode_ctrl;

	ADPT_DEV_ID_CHECK(dev_id);
	MP_PORT_ID_CHECK(port_id);
	ADPT_NULL_POINT_CHECK(priv);

	memset(&mac_flow_ctrl, 0, sizeof(mac_flow_ctrl));
	memset(&mac_operation_mode_ctrl, 0, sizeof(mac_operation_mode_ctrl));
	gmac_id = MP_PORT_TO_GMAC_ID(port_id);

	rv = mp_mac_flowctrl_get(dev_id, gmac_id, &mac_flow_ctrl);
	SW_RTN_ON_ERROR(rv);

	rv = mp_mac_operation_mode_ctrl_get(dev_id, gmac_id,
			&mac_operation_mode_ctrl);
	SW_RTN_ON_ERROR(rv);

	if (A_TRUE == enable) {
		mac_flow_ctrl.bf.flowctrl_tx_enable = 1;
		mac_flow_ctrl.bf.pause_time = GMAC_PAUSE_TIME;
		mac_flow_ctrl.bf.disable_zero_quanta_pause =
			GMAC_PAUSE_ZERO_QUANTA_ENABLE;
		mac_operation_mode_ctrl.bf.enable_hw_flowctrl =
			GMAC_HW_FLOWCTRL_ENABLE;
		mac_operation_mode_ctrl.bf.disable_flushing_receiving_frame =
			GMAC_FLUSH_RECEIVED_FRAMES_DISABLE;
		/*activate flowctrl when 6KB FIFO is available*/
		mac_operation_mode_ctrl.val &= ~(GMAC_ACTIVATE_FLOWCTRL_MASK);
		mac_operation_mode_ctrl.val |= GMAC_ACTIVATE_FLOWCTRL_WITH_6KB;
		/*dactivate flowctrl when 7KB FIFO is available*/
		mac_operation_mode_ctrl.val &= ~(GMAC_DACTIVATE_FLOWCTRL_MASK);
		mac_operation_mode_ctrl.val |= GMAC_DACTIVATE_FLOWCTRL_WITH_7KB;
	} else {
		mac_flow_ctrl.bf.flowctrl_tx_enable = 0;
		mac_operation_mode_ctrl.bf.enable_hw_flowctrl =
			GMAC_HW_FLOWCTRL_DISABLE;
	}

	rv = mp_mac_operation_mode_ctrl_set(dev_id, gmac_id,
			&mac_operation_mode_ctrl);
	SW_RTN_ON_ERROR(rv);

	rv = mp_mac_flowctrl_set(dev_id, gmac_id, &mac_flow_ctrl);
	SW_RTN_ON_ERROR(rv);

	priv->port_old_tx_flowctrl[port_id - 1] = enable;

	return rv;
}

#ifndef IN_PORTCONTROL_MINI
static sw_error_t
adpt_mp_port_txfc_status_get(a_uint32_t dev_id, fal_port_t port_id,
	a_bool_t * enable)
{
	sw_error_t rv = SW_OK;
	a_uint32_t gmac_id = 0;
	union mac_flow_ctrl_u mac_flow_ctrl;

	ADPT_DEV_ID_CHECK(dev_id);
	MP_PORT_ID_CHECK(port_id);
	ADPT_NULL_POINT_CHECK(enable);

	memset(&mac_flow_ctrl, 0, sizeof(mac_flow_ctrl));
	gmac_id = MP_PORT_TO_GMAC_ID(port_id);

	rv = mp_mac_flowctrl_get(dev_id, gmac_id, &mac_flow_ctrl);
	SW_RTN_ON_ERROR(rv);

	if (mac_flow_ctrl.bf.flowctrl_tx_enable) {
		*enable = A_TRUE;
	} else {
		*enable = A_FALSE;
	}

	return rv;
}
#endif

static sw_error_t
adpt_mp_port_rxfc_status_set(a_uint32_t dev_id, fal_port_t port_id,
	a_bool_t enable)
{
	sw_error_t rv = SW_OK;
	a_uint32_t gmac_id = 0;
	union mac_flow_ctrl_u mac_flow_ctrl;
	struct qca_phy_priv *priv = ssdk_phy_priv_data_get(dev_id);

	ADPT_DEV_ID_CHECK(dev_id);
	MP_PORT_ID_CHECK(port_id);
	ADPT_NULL_POINT_CHECK(priv);

	memset(&mac_flow_ctrl, 0, sizeof(mac_flow_ctrl));
	gmac_id = MP_PORT_TO_GMAC_ID(port_id);

	rv = mp_mac_flowctrl_get(dev_id, gmac_id, &mac_flow_ctrl);
	SW_RTN_ON_ERROR(rv);

	if (A_TRUE == enable) {
		mac_flow_ctrl.bf.flowctrl_rx_enable = 1;
	} else {
		mac_flow_ctrl.bf.flowctrl_rx_enable = 0;
	}

	rv = mp_mac_flowctrl_set(dev_id, gmac_id, &mac_flow_ctrl);

	priv->port_old_rx_flowctrl[port_id - 1] = enable;

	return rv;
}

#ifndef IN_PORTCONTROL_MINI
static sw_error_t
adpt_mp_port_rxfc_status_get(a_uint32_t dev_id, fal_port_t port_id,
	a_bool_t * enable)
{
	sw_error_t rv = SW_OK;
	a_uint32_t gmac_id = 0;
	union mac_flow_ctrl_u mac_flow_ctrl;

	ADPT_DEV_ID_CHECK(dev_id);
	MP_PORT_ID_CHECK(port_id);
	ADPT_NULL_POINT_CHECK(enable);

	memset(&mac_flow_ctrl, 0, sizeof(mac_flow_ctrl));
	gmac_id = MP_PORT_TO_GMAC_ID(port_id);

	rv = mp_mac_flowctrl_get(dev_id, gmac_id, &mac_flow_ctrl);
	SW_RTN_ON_ERROR(rv);

	if (mac_flow_ctrl.bf.flowctrl_rx_enable) {
		*enable = A_TRUE;
	} else {
		*enable = A_FALSE;
	}

	return rv;
}
#endif

static sw_error_t
adpt_mp_port_flowctrl_set(a_uint32_t dev_id, fal_port_t port_id,
	a_bool_t enable)
{
	sw_error_t rv = SW_OK;
	struct qca_phy_priv *priv = ssdk_phy_priv_data_get(dev_id);

	ADPT_DEV_ID_CHECK(dev_id);
	MP_PORT_ID_CHECK(port_id);
	ADPT_NULL_POINT_CHECK(priv);

	rv = adpt_mp_port_txfc_status_set(dev_id, port_id, enable);
	SW_RTN_ON_ERROR(rv);

	rv = adpt_mp_port_rxfc_status_set(dev_id, port_id, enable);
	SW_RTN_ON_ERROR(rv);

	priv->port_old_tx_flowctrl[port_id - 1] = enable;
	priv->port_old_rx_flowctrl[port_id - 1] = enable;

	return rv;
}

#ifndef IN_PORTCONTROL_MINI
static sw_error_t
adpt_mp_port_flowctrl_get(a_uint32_t dev_id, fal_port_t port_id,
	a_bool_t * enable)
{
	sw_error_t rv = SW_OK;
	a_bool_t txfc_enable, rxfc_enable;

	ADPT_DEV_ID_CHECK(dev_id);
	MP_PORT_ID_CHECK(port_id);
	ADPT_NULL_POINT_CHECK(enable);

	rv = adpt_mp_port_txfc_status_get(dev_id, port_id,  &txfc_enable);
	SW_RTN_ON_ERROR(rv);
	rv = adpt_mp_port_rxfc_status_get(dev_id, port_id,  &rxfc_enable);
	SW_RTN_ON_ERROR(rv);

	*enable = txfc_enable & rxfc_enable;

	return rv;
}
#endif

static sw_error_t
adpt_mp_port_flowctrl_forcemode_set(a_uint32_t dev_id,
	fal_port_t port_id, a_bool_t enable)
{
	sw_error_t rv = SW_OK;
	struct qca_phy_priv *priv = ssdk_phy_priv_data_get(dev_id);

	ADPT_DEV_ID_CHECK(dev_id);
	MP_PORT_ID_CHECK(port_id);
	ADPT_NULL_POINT_CHECK(priv);

	priv->port_tx_flowctrl_forcemode[port_id - 1] = enable;
	priv->port_rx_flowctrl_forcemode[port_id - 1] = enable;

	return rv;
}

#ifndef IN_PORTCONTROL_MINI
static sw_error_t
adpt_mp_port_flowctrl_forcemode_get(a_uint32_t dev_id,
	fal_port_t port_id, a_bool_t *enable)
{
	sw_error_t rv = SW_OK;
	struct qca_phy_priv *priv = ssdk_phy_priv_data_get(dev_id);


	ADPT_DEV_ID_CHECK(dev_id);
	MP_PORT_ID_CHECK(port_id);
	ADPT_NULL_POINT_CHECK(enable);
	ADPT_NULL_POINT_CHECK(priv);

	*enable = (priv->port_tx_flowctrl_forcemode[port_id - 1] &
		priv->port_rx_flowctrl_forcemode[port_id - 1]);

	return rv;
}
#endif

static sw_error_t
adpt_mp_port_mac_status_get(a_uint32_t dev_id, a_uint32_t port_id,
	struct port_phy_status *port_mac_status)
{
	sw_error_t rv = SW_OK;
	a_uint32_t gmac_id = 0;
	union mac_configuration_u configuration;

	ADPT_DEV_ID_CHECK(dev_id);
	MP_PORT_ID_CHECK(port_id);
	ADPT_NULL_POINT_CHECK(port_mac_status);

	memset(&configuration, 0, sizeof(configuration));
	gmac_id = MP_PORT_TO_GMAC_ID(port_id);

	rv = mp_mac_configuration_get(dev_id, gmac_id, &configuration);
	SW_RTN_ON_ERROR(rv);

	if (configuration.bf.port_select == GMAC_SPEED_1000M) {
		port_mac_status->speed = FAL_SPEED_1000;
	} else {
		if (configuration.bf.mii_speed == GMAC_SPEED_100M) {
			port_mac_status->speed = FAL_SPEED_100;
		} else {
			port_mac_status->speed = FAL_SPEED_10;
		}
	}

	if (configuration.bf.duplex == GMAC_FULL_DUPLEX) {
		port_mac_status->duplex = FAL_FULL_DUPLEX;
	} else {
		port_mac_status->duplex = FAL_HALF_DUPLEX;
	}

	return rv;

}

static sw_error_t
adpt_mp_port_mac_speed_set(a_uint32_t dev_id, a_uint32_t port_id,
	fal_port_speed_t speed)
{
	sw_error_t rv = SW_OK;
	a_uint32_t gmac_id = 0;
	union mac_configuration_u configuration;
	a_bool_t force_port;

	ADPT_DEV_ID_CHECK(dev_id);
	MP_PORT_ID_CHECK(port_id);

	memset(&configuration, 0, sizeof(configuration));
	gmac_id = MP_PORT_TO_GMAC_ID(port_id);

	rv = mp_mac_configuration_get(dev_id, gmac_id, &configuration);
	SW_RTN_ON_ERROR(rv);

	if ((FAL_SPEED_1000 == speed) || (FAL_SPEED_2500 == speed)) {
		 configuration.bf.port_select = GMAC_SPEED_1000M;
	} else if (FAL_SPEED_100 == speed) {
		configuration.bf.port_select = (~GMAC_SPEED_1000M) & 0x1;
		configuration.bf.mii_speed = GMAC_SPEED_100M;
	} else if (FAL_SPEED_10== speed) {
		configuration.bf.port_select = (~GMAC_SPEED_1000M) & 0x1;
		configuration.bf.mii_speed = GMAC_SPEED_10M;
	}

	rv = mp_mac_configuration_set(dev_id, gmac_id, &configuration);
	SW_RTN_ON_ERROR(rv);

	force_port = ssdk_port_feature_get(dev_id, port_id, PHY_F_FORCE);
	/* enable force port configuration */
	if (force_port == A_TRUE) {
		rv = _adpt_mp_port_gcc_speed_clock_set(dev_id,
			port_id, speed);
		SW_RTN_ON_ERROR(rv);
		rv = adpt_mp_gcc_uniphy_port_clock_set(dev_id,
			port_id, A_TRUE);
		SW_RTN_ON_ERROR(rv);
		rv = _adpt_mp_gcc_mac_clock_set(dev_id,
			port_id, A_TRUE);
		SW_RTN_ON_ERROR(rv);
		rv = adpt_mp_port_reset_set(dev_id, port_id);
		SW_RTN_ON_ERROR(rv);
	}
	return rv;

}

static sw_error_t
adpt_mp_port_mac_duplex_set(a_uint32_t dev_id, a_uint32_t port_id,
	fal_port_duplex_t duplex)
{
	sw_error_t rv = SW_OK;
	a_uint32_t gmac_id = 0;
	union mac_configuration_u configuration;

	ADPT_DEV_ID_CHECK(dev_id);
	MP_PORT_ID_CHECK(port_id);

	memset(&configuration, 0, sizeof(configuration));
	gmac_id = MP_PORT_TO_GMAC_ID(port_id);

	rv = mp_mac_configuration_get(dev_id, gmac_id, &configuration);
	SW_RTN_ON_ERROR(rv);

	if (FAL_FULL_DUPLEX == duplex) {
		configuration.bf.duplex = GMAC_FULL_DUPLEX;
	} else {
		configuration.bf.duplex = GMAC_HALF_DUPLEX;
	}

	rv = mp_mac_configuration_set(dev_id, gmac_id, &configuration);

	return rv;
}

static sw_error_t
adpt_mp_port_promisc_mode_set(a_uint32_t dev_id, fal_port_t port_id,
	a_bool_t enable)
{
	sw_error_t rv = SW_OK;
	a_uint32_t gmac_id = 0;
	union mac_frame_filter_u mac_frame_filter;

	ADPT_DEV_ID_CHECK(dev_id);
	MP_PORT_ID_CHECK(port_id);

	memset(&mac_frame_filter, 0, sizeof(mac_frame_filter));
	gmac_id = MP_PORT_TO_GMAC_ID(port_id);

	rv = mp_mac_frame_filter_get(dev_id, gmac_id, &mac_frame_filter);
	SW_RTN_ON_ERROR(rv);

	mac_frame_filter.bf.promiscuous_mode = enable;

	rv = mp_mac_frame_filter_set(dev_id, gmac_id, &mac_frame_filter);

	return rv;
}

#ifndef IN_PORTCONTROL_MINI
static sw_error_t
adpt_mp_port_promisc_mode_get(a_uint32_t dev_id, fal_port_t port_id,
	a_bool_t *enable)
{
	sw_error_t rv = SW_OK;
	a_uint32_t gmac_id = 0;
	union mac_frame_filter_u mac_frame_filter;

	ADPT_DEV_ID_CHECK(dev_id);
	MP_PORT_ID_CHECK(port_id);
	ADPT_NULL_POINT_CHECK(enable);

	memset(&mac_frame_filter, 0, sizeof(mac_frame_filter));
	gmac_id = MP_PORT_TO_GMAC_ID(port_id);

	rv = mp_mac_frame_filter_get(dev_id, gmac_id, &mac_frame_filter);
	SW_RTN_ON_ERROR(rv);

	*enable = mac_frame_filter.bf.promiscuous_mode;

	return rv;
}
#endif

static sw_error_t
adpt_mp_port_max_frame_size_set(a_uint32_t dev_id, fal_port_t port_id,
	a_uint32_t max_frame)
{
	sw_error_t rv = SW_OK;
	a_uint32_t gmac_id = 0;
	union mac_max_frame_ctrl_u mac_max_frame_ctrl;
	union mac_configuration_u configuration;
	union mac_operation_mode_ctrl_u mac_operation_mode_ctrl;

	ADPT_DEV_ID_CHECK(dev_id);
	MP_PORT_ID_CHECK(port_id);

	memset(&configuration, 0, sizeof(configuration));
	memset(&mac_max_frame_ctrl, 0, sizeof(mac_max_frame_ctrl));
	memset(&mac_operation_mode_ctrl, 0, sizeof(mac_operation_mode_ctrl));
	gmac_id = MP_PORT_TO_GMAC_ID(port_id);

	rv = mp_mac_max_frame_ctrl_get(dev_id, gmac_id, &mac_max_frame_ctrl);
	SW_RTN_ON_ERROR(rv);
	rv = mp_mac_configuration_get(dev_id, gmac_id, &configuration);
	SW_RTN_ON_ERROR(rv);

	configuration.bf.jabber_disable = GMAC_JD_ENABLE;
	configuration.bf.watchdog_disable = GMAC_WD_DISABLE;
	configuration.bf.jumbo_frame_enable = GMAC_JUMBO_FRAME_ENABLE;
	configuration.bf.frame_burst_enable = GMAC_FRAME_BURST_ENABLE;
	rv = mp_mac_configuration_set(dev_id, gmac_id, &configuration);
	SW_RTN_ON_ERROR(rv);

	mac_max_frame_ctrl.bf.max_frame_ctrl_enable = GMAC_MAX_FRAME_CTRL_ENABLE;
	/* default max_frame 1518 byte doesn't include vlan tag */
	mac_max_frame_ctrl.bf.max_frame_ctrl = max_frame + 8;
	rv = mp_mac_max_frame_ctrl_set(dev_id, gmac_id, &mac_max_frame_ctrl);

	rv = mp_mac_operation_mode_ctrl_get(dev_id, gmac_id,
			&mac_operation_mode_ctrl);
	mac_operation_mode_ctrl.bf.receive_store_and_foward =
		GMAC_RX_STORE_FORWAD_ENABLE;
	mac_operation_mode_ctrl.bf.transmit_store_and_foward =
		GMAC_TX_STORE_FORWAD_ENABLE;
	mac_operation_mode_ctrl.bf.forward_error_frame =
		GMAC_FORWARD_ERROR_FRAME_DISABLE;
	mac_operation_mode_ctrl.bf.drop_gaint_frame =
		GMAC_DROP_GAINT_FRAME_DISABLE;
	rv = mp_mac_operation_mode_ctrl_set(dev_id, gmac_id,
			&mac_operation_mode_ctrl);

	return rv;
}

static sw_error_t
adpt_mp_port_max_frame_size_get(a_uint32_t dev_id, fal_port_t port_id,
	a_uint32_t *max_frame)
{
	sw_error_t rv = SW_OK;
	a_uint32_t gmac_id = 0;
	union mac_max_frame_ctrl_u mac_max_frame_ctrl;

	ADPT_DEV_ID_CHECK(dev_id);
	MP_PORT_ID_CHECK(port_id);
	ADPT_NULL_POINT_CHECK(max_frame);

	memset(&mac_max_frame_ctrl, 0, sizeof(mac_max_frame_ctrl));
	gmac_id = MP_PORT_TO_GMAC_ID(port_id);

	rv = mp_mac_max_frame_ctrl_get(dev_id, gmac_id, &mac_max_frame_ctrl);
	SW_RTN_ON_ERROR(rv);

	*max_frame = mac_max_frame_ctrl.bf.max_frame_ctrl;

	return rv;
}

static sw_error_t
adpt_mp_port_mac_eee_enable_set(a_uint32_t dev_id, fal_port_t port_id,
	a_bool_t enable)
{
	sw_error_t rv = 0;
	a_uint32_t gmac_id = 0;
	union mac_lpi_ctrl_status_u mac_lpi_ctrl_status;

	ADPT_DEV_ID_CHECK(dev_id);
	MP_PORT_ID_CHECK(port_id);

	memset(&mac_lpi_ctrl_status, 0, sizeof(mac_lpi_ctrl_status));
	gmac_id = MP_PORT_TO_GMAC_ID(port_id);

	rv = mp_mac_lpi_ctrl_status_get(dev_id, gmac_id, &mac_lpi_ctrl_status);
	SW_RTN_ON_ERROR(rv);
	mac_lpi_ctrl_status.bf.lpi_enable = enable;

	rv = mp_mac_lpi_ctrl_status_set(dev_id, gmac_id, &mac_lpi_ctrl_status);

	return rv;
}

#ifndef IN_PORTCONTROL_MINI
static sw_error_t
adpt_mp_port_mac_eee_enable_get(a_uint32_t dev_id, fal_port_t port_id,
	a_bool_t *enable)
{
	sw_error_t rv = 0;
	a_uint32_t gmac_id = 0;
	union mac_lpi_ctrl_status_u mac_lpi_ctrl_status;

	ADPT_DEV_ID_CHECK(dev_id);
	MP_PORT_ID_CHECK(port_id);
	ADPT_NULL_POINT_CHECK(enable);

	memset(&mac_lpi_ctrl_status, 0, sizeof(mac_lpi_ctrl_status));
	gmac_id = MP_PORT_TO_GMAC_ID(port_id);

	rv = mp_mac_lpi_ctrl_status_get(dev_id, gmac_id, &mac_lpi_ctrl_status);
	SW_RTN_ON_ERROR(rv);

	*enable = mac_lpi_ctrl_status.bf.lpi_enable;

	return rv;

}
#endif

static sw_error_t
adpt_mp_port_interface_eee_cfg_set(a_uint32_t dev_id, fal_port_t port_id,
	fal_port_eee_cfg_t *port_eee_cfg)
{
	sw_error_t rv = 0;
	union mac_lpi_timer_ctrl_u mac_lpi_timer_ctrl;
	union mac_lpi_ctrl_status_u mac_lpi_ctrl_status;

	a_uint32_t phy_addr = 0, gmac_id = 0;
	a_uint32_t adv;
	hsl_phy_ops_t *phy_drv;
	struct qca_phy_priv *priv;

	ADPT_DEV_ID_CHECK(dev_id);
	MP_PORT_ID_CHECK(port_id);

	memset(&mac_lpi_timer_ctrl, 0, sizeof(mac_lpi_timer_ctrl));
	memset(&mac_lpi_ctrl_status, 0, sizeof(mac_lpi_ctrl_status));
	gmac_id = MP_PORT_TO_GMAC_ID(port_id);

	priv = ssdk_phy_priv_data_get(dev_id);
	SW_RTN_ON_NULL(priv);

	if (port_eee_cfg->enable) {
		adv = port_eee_cfg->advertisement;
	} else {
		adv = 0;
	}

	if (port_eee_cfg->lpi_tx_enable) {
		port_lpi_status[dev_id] |= BIT(port_id-1);
	} else {
		port_lpi_status[dev_id] &= ~BIT(port_id-1);
	}

	SW_RTN_ON_NULL(phy_drv = hsl_phy_api_ops_get(dev_id, port_id));
	if (NULL == phy_drv->phy_eee_adv_set) {
		return SW_NOT_SUPPORTED;
	}

	rv = hsl_port_prop_get_phyid(dev_id, port_id, &phy_addr);
	SW_RTN_ON_ERROR (rv);

	rv = phy_drv->phy_eee_adv_set(dev_id, phy_addr, adv);
	SW_RTN_ON_ERROR (rv);

	rv = mp_mac_lpi_timer_ctrl_get(dev_id, gmac_id, &mac_lpi_timer_ctrl);
	SW_RTN_ON_ERROR (rv);
	mac_lpi_timer_ctrl.bf.lpi_tw_timer = port_eee_cfg->lpi_wakeup_timer;
	rv = mp_mac_lpi_timer_ctrl_set(dev_id, gmac_id, &mac_lpi_timer_ctrl);
	SW_RTN_ON_ERROR (rv);

	rv = mp_mac_lpi_ctrl_status_get(dev_id, gmac_id, &mac_lpi_ctrl_status);
	SW_RTN_ON_ERROR (rv);
	mac_lpi_ctrl_status.bf.lpi_tx_auto_enable = GMAC_LPI_AUTO_MODE;
	mac_lpi_ctrl_status.bf.link_status = GMAC_LPI_LINK_UP;
	mac_lpi_ctrl_status.bf.lpi_enable = port_eee_cfg->lpi_tx_enable;
	rv = mp_mac_lpi_ctrl_status_set(dev_id, gmac_id, &mac_lpi_ctrl_status);
	SW_RTN_ON_ERROR (rv);

	if (port_lpi_status[dev_id] & PORT_LPI_ENABLE_STATUS) {
		if (!(port_lpi_status[dev_id] & PORT_LPI_TASK_RUNNING)) {
			if (!(port_lpi_status[dev_id] & PORT_LPI_TASK_START)) {
				qca_mac_sw_sync_work_start(priv);
				port_lpi_status[dev_id] |= PORT_LPI_TASK_START;
			} else {
				qca_mac_sw_sync_work_resume(priv);
			}
			port_lpi_status[dev_id] |= PORT_LPI_TASK_RUNNING;
		}
	} else {
		qca_mac_sw_sync_work_stop(priv);
		port_lpi_status[dev_id] &= ~PORT_LPI_TASK_RUNNING;
	}

	return rv;
}
static sw_error_t
adpt_mp_port_interface_eee_cfg_get(a_uint32_t dev_id, fal_port_t port_id,
	fal_port_eee_cfg_t *port_eee_cfg)
{
	sw_error_t rv = 0;
	union mac_lpi_timer_ctrl_u mac_lpi_timer_ctrl;
	union mac_lpi_ctrl_status_u mac_lpi_ctrl_status;
	a_uint32_t phy_addr = 0, gmac_id = 0;
	a_uint32_t adv, lp_adv, cap, status;
	hsl_phy_ops_t *phy_drv;

	ADPT_DEV_ID_CHECK(dev_id);
	MP_PORT_ID_CHECK(port_id);
	ADPT_NULL_POINT_CHECK(port_eee_cfg);

	memset(&mac_lpi_timer_ctrl, 0, sizeof(mac_lpi_timer_ctrl));
	memset(&mac_lpi_ctrl_status, 0, sizeof(mac_lpi_ctrl_status));
	memset(port_eee_cfg, 0, sizeof(*port_eee_cfg));
	gmac_id = MP_PORT_TO_GMAC_ID(port_id);

	SW_RTN_ON_NULL(phy_drv =hsl_phy_api_ops_get (dev_id, port_id));
	if ((NULL == phy_drv->phy_eee_adv_get) ||
		(NULL == phy_drv->phy_eee_partner_adv_get) ||
		(NULL == phy_drv->phy_eee_cap_get) ||
		(NULL == phy_drv->phy_eee_status_get)) {
		return SW_NOT_SUPPORTED;
	}
	rv = hsl_port_prop_get_phyid(dev_id, port_id, &phy_addr);
	SW_RTN_ON_ERROR (rv);
	rv = phy_drv->phy_eee_adv_get(dev_id, phy_addr, &adv);
	SW_RTN_ON_ERROR (rv);
	port_eee_cfg->advertisement = adv;
	rv = phy_drv->phy_eee_partner_adv_get(dev_id, phy_addr, &lp_adv);
	SW_RTN_ON_ERROR (rv);
	port_eee_cfg->link_partner_advertisement = lp_adv;
	rv = phy_drv->phy_eee_cap_get(dev_id, phy_addr, &cap);
	SW_RTN_ON_ERROR (rv);
	port_eee_cfg->capability = cap;
	rv = phy_drv->phy_eee_status_get(dev_id, phy_addr, &status);
	SW_RTN_ON_ERROR (rv);
	port_eee_cfg->eee_status = status;

	if (port_eee_cfg->advertisement) {
		port_eee_cfg->enable = A_TRUE;
	} else {
		port_eee_cfg->enable = A_FALSE;
	}
	rv = mp_mac_lpi_ctrl_status_get(dev_id, gmac_id, &mac_lpi_ctrl_status);
	SW_RTN_ON_ERROR (rv);
	port_eee_cfg->lpi_tx_enable = mac_lpi_ctrl_status.bf.lpi_enable;

	rv = mp_mac_lpi_timer_ctrl_get(dev_id, gmac_id, &mac_lpi_timer_ctrl);
	SW_RTN_ON_ERROR (rv);
	port_eee_cfg->lpi_wakeup_timer = mac_lpi_timer_ctrl.bf.lpi_tw_timer;

	return rv;
}

static sw_error_t
adpt_mp_port_interface_mode_status_get(a_uint32_t dev_id,
	a_uint32_t port_id, fal_port_interface_mode_t * mode)
{
	sw_error_t rv = SW_OK;
	a_uint32_t phy_id = 0;
	hsl_phy_ops_t *phy_drv;

	ADPT_DEV_ID_CHECK(dev_id);
	ADPT_NULL_POINT_CHECK(mode);

	SW_RTN_ON_NULL(phy_drv = hsl_phy_api_ops_get(dev_id, port_id));
	SW_RTN_ON_NULL(phy_drv->phy_interface_mode_status_get);

	rv = hsl_port_prop_get_phyid(dev_id, port_id, &phy_id);
	SW_RTN_ON_ERROR (rv);

	rv = phy_drv->phy_interface_mode_status_get(dev_id, phy_id,mode);

	return rv;
}

static sw_error_t
adpt_mp_port_interface_mode_switch(a_uint32_t dev_id, a_uint32_t port_id)
{
	sw_error_t rv = SW_OK;
	fal_port_interface_mode_t port_mode_new = PORT_INTERFACE_MODE_MAX;
	a_uint32_t uniphy_mode_old = PORT_WRAPPER_MAX;
	a_uint32_t uniphy_mode_new = PORT_WRAPPER_MAX;
	a_bool_t force_port;

	force_port = ssdk_port_feature_get(dev_id, port_id, PHY_F_FORCE);
	if ((port_id == SSDK_PHYSICAL_PORT1) || (force_port == A_TRUE)) {
		return SW_OK;
	}
	rv = adpt_mp_port_interface_mode_status_get(dev_id,
		port_id, &port_mode_new);
	SW_RTN_ON_ERROR(rv);

	if (port_mode_new == PHY_SGMII_BASET) {
		uniphy_mode_new = PORT_WRAPPER_SGMII_CHANNEL0;
	} else if (port_mode_new == PORT_SGMII_PLUS) {
		uniphy_mode_new = PORT_WRAPPER_SGMII_PLUS;
	} else {
		return SW_NOT_SUPPORTED;
	}
	uniphy_mode_old = ssdk_dt_global_get_mac_mode(dev_id,
		SSDK_UNIPHY_INSTANCE0);
	if (uniphy_mode_new != uniphy_mode_old) {
		rv = adpt_mp_uniphy_mode_configure(dev_id,
			SSDK_UNIPHY_INSTANCE0, uniphy_mode_new);
		SW_RTN_ON_ERROR(rv);
		ssdk_dt_global_set_mac_mode(dev_id,
			SSDK_UNIPHY_INSTANCE0, uniphy_mode_new);
	}

	return rv;
}

static sw_error_t
_adpt_mp_port_phy_status_get(a_uint32_t dev_id, a_uint32_t port_id,
	struct port_phy_status *phy_status)
{
	sw_error_t rv = 0;
	a_uint32_t phy_addr;
	hsl_phy_ops_t *phy_drv;

	ADPT_DEV_ID_CHECK(dev_id);
	ADPT_NULL_POINT_CHECK(phy_status);

	SW_RTN_ON_NULL (phy_drv = hsl_phy_api_ops_get (dev_id, port_id));
	SW_RTN_ON_NULL (phy_drv->phy_get_status);

	rv = hsl_port_prop_get_phyid (dev_id, port_id, &phy_addr);
	SW_RTN_ON_ERROR (rv);
	rv = phy_drv->phy_get_status (dev_id, phy_addr, phy_status);
	SW_RTN_ON_ERROR (rv);

	return rv;
}

static sw_error_t
_adpt_mp_port_link_down_update(struct qca_phy_priv *priv,
	a_uint32_t port_id)
{
	sw_error_t rv = 0;

	/* disable rx mac, gcc uniphy port and gcc mac port status */
	rv = adpt_mp_port_rxmac_status_set(priv->device_id, port_id, A_FALSE);
	SW_RTN_ON_ERROR (rv);
	rv = adpt_mp_gcc_uniphy_port_clock_set(priv->device_id, port_id, A_FALSE);
	SW_RTN_ON_ERROR (rv);
	_adpt_mp_gcc_mac_clock_set(priv->device_id, port_id, A_FALSE);
	SW_RTN_ON_ERROR (rv);

	/* switch interface mode if necessary under link down*/
	rv = adpt_mp_port_interface_mode_switch(priv->device_id, port_id);
	SW_RTN_ON_ERROR (rv);
	SSDK_DEBUG("MP port %d interface mode switch under link down!\n",
			port_id);
	return rv;
}

static a_bool_t
_adpt_mp_port_status_change(struct qca_phy_priv *priv, a_uint32_t port_id,
	struct port_phy_status phy_status)
{
	if ((a_uint32_t)phy_status.speed != priv->port_old_speed[port_id - 1])
		return A_TRUE;
	if ((a_uint32_t)phy_status.duplex != priv->port_old_duplex[port_id - 1])
		return A_TRUE;
	if (phy_status.tx_flowctrl != priv->port_old_tx_flowctrl[port_id - 1])
		return A_TRUE;
	if (phy_status.rx_flowctrl != priv->port_old_rx_flowctrl[port_id - 1])
		return A_TRUE;
	return A_FALSE;
}

sw_error_t
adpt_mp_port_link_up_change_update(struct qca_phy_priv *priv,
	a_uint32_t port_id, struct port_phy_status phy_status)
{
	sw_error_t rv = 0;

	if ((a_uint32_t)phy_status.speed !=
		priv->port_old_speed[port_id - 1]) {

		/* configure gcc speed clock frequency */
		rv = _adpt_mp_port_gcc_speed_clock_set(priv->device_id,
			port_id, phy_status.speed);
		SW_RTN_ON_ERROR (rv);

		/* config mac speed */
		rv = adpt_mp_port_mac_speed_set(priv->device_id,
			port_id, phy_status.speed);
		SW_RTN_ON_ERROR (rv);

		priv->port_old_speed[port_id - 1] =
			(a_uint32_t)phy_status.speed;

		SSDK_DEBUG("Port %d up and speed is %d\n", port_id,
			priv->port_old_speed[port_id - 1]);
	}
	/* link up duplex change configuration */
	if ((a_uint32_t)phy_status.duplex !=
		priv->port_old_duplex[port_id - 1]) {

		rv = adpt_mp_port_mac_duplex_set(priv->device_id,
			port_id, phy_status.duplex);

		priv->port_old_duplex[port_id - 1] =
			(a_uint32_t)phy_status.duplex;
		SW_RTN_ON_ERROR (rv);

		SSDK_DEBUG("Port %d up and duplex is %d\n", port_id,
			priv->port_old_duplex[port_id - 1]);
	}
	/* tx flowctrl configuration*/
	if (priv->port_tx_flowctrl_forcemode[port_id - 1] != A_TRUE) {
		if (phy_status.duplex == FAL_HALF_DUPLEX) {
			phy_status.tx_flowctrl = A_TRUE;
		}
		if (phy_status.tx_flowctrl !=
			priv->port_old_tx_flowctrl[port_id - 1]) {
			rv = adpt_mp_port_txfc_status_set(priv->device_id,
				port_id, phy_status.tx_flowctrl);
			SW_RTN_ON_ERROR (rv);
			priv->port_old_tx_flowctrl[port_id - 1] =
				phy_status.tx_flowctrl;

			SSDK_DEBUG("Port %d up and tx flowctrl is %d\n",
				port_id,
				priv->port_old_tx_flowctrl[port_id - 1]);
		}
	}
	/*rx flowctrl configuration*/
	if (priv->port_rx_flowctrl_forcemode[port_id - 1] != A_TRUE) {
		if (phy_status.duplex == FAL_HALF_DUPLEX) {
			phy_status.rx_flowctrl = A_TRUE;
		}
		if (phy_status.rx_flowctrl !=
			priv->port_old_rx_flowctrl[port_id - 1]) {
			rv = adpt_mp_port_rxfc_status_set(priv->device_id,
				port_id, phy_status.rx_flowctrl);
			SW_RTN_ON_ERROR (rv);
			priv->port_old_rx_flowctrl[port_id - 1] =
				phy_status.rx_flowctrl;

			SSDK_DEBUG("Port %d up and rx flowctrl is %d\n",
				port_id,
				priv->port_old_rx_flowctrl[port_id-1]);
		}
	}

	return rv;
}

sw_error_t
adpt_mp_port_link_up_update(struct qca_phy_priv *priv,
	a_uint32_t port_id, struct port_phy_status phy_status)
{
	sw_error_t rv = 0;
	a_bool_t change;

	/* port phy status change check*/
	change = _adpt_mp_port_status_change(priv, port_id,
			phy_status);

	rv = adpt_mp_port_txmac_status_set(priv->device_id, port_id,
				A_FALSE);
	SW_RTN_ON_ERROR (rv);

	/* switch interface mode if necessary under link up */
	rv = adpt_mp_port_interface_mode_switch(priv->device_id, port_id);
	SW_RTN_ON_ERROR (rv);
	SSDK_DEBUG("MP port %d interface mode switch under link up!\n",
			port_id);
	/* link up status change*/
	if (change == A_TRUE) {
		rv = adpt_mp_port_link_up_change_update(priv,
			port_id, phy_status);
		SW_RTN_ON_ERROR (rv);
	}

	rv = adpt_mp_gcc_uniphy_port_clock_set(priv->device_id,
			port_id, A_TRUE);
	SW_RTN_ON_ERROR (rv);
	rv =_adpt_mp_gcc_mac_clock_set(priv->device_id,
			port_id, A_TRUE);
	SW_RTN_ON_ERROR (rv);

	msleep(50);

	rv = adpt_mp_port_reset_set(priv->device_id, port_id);
	SW_RTN_ON_ERROR (rv);
	rv = adpt_mp_port_txmac_status_set(priv->device_id,
			port_id, A_TRUE);
	SW_RTN_ON_ERROR (rv);
	rv = adpt_mp_port_rxmac_status_set(priv->device_id,
		port_id, A_TRUE);
	SW_RTN_ON_ERROR (rv);

	return rv;
}

sw_error_t
adpt_mp_port_netdev_change_notify(struct qca_phy_priv *priv,
	a_uint32_t port_id)
{
	sw_error_t rv = 0;
	struct port_phy_status phy_status = {0};
	a_uint32_t portbmp[SW_MAX_NR_DEV] = {0};

	portbmp[priv->device_id] = qca_ssdk_port_bmp_get(priv->device_id);

	if(!(portbmp[priv->device_id] & (0x1 << port_id))) {
		SSDK_ERROR("netdev change notify with incorrect port %d\n",
			port_id);
		return SW_BAD_VALUE;
	}

	rv = _adpt_mp_port_phy_status_get(priv->device_id, port_id,
		&phy_status);
	if (rv != SW_OK) {
		SSDK_ERROR("failed to get port %d status return value is %d\n",
			port_id, rv);
		return rv;
	}
	/* link status from up to down*/
	if ((phy_status.link_status == PORT_LINK_DOWN) &&
		(priv->port_old_link[port_id - 1] == PORT_LINK_UP)) {
		SSDK_DEBUG("MP port %d change to link down status\n", port_id);
		/* link down configuration*/
		rv = _adpt_mp_port_link_down_update(priv, port_id);
		SW_RTN_ON_ERROR (rv);
		priv->port_old_link[port_id - 1] = phy_status.link_status ;
	}
	/* link status from down to up */
	if ((phy_status.link_status == PORT_LINK_UP) &&
		(priv->port_old_link[port_id - 1] == PORT_LINK_DOWN)) {
		SSDK_DEBUG("Port %d change to link up status\n", port_id);
		rv = adpt_mp_port_link_up_update(priv, port_id, phy_status);
		SW_RTN_ON_ERROR (rv);
		priv->port_old_link[port_id - 1] = phy_status.link_status;
	}
	SSDK_DEBUG("MP port %d link is %d speed is %d duplex is %d"
		" tx_flowctrl is %d rx_flowctrl is %d\n",
	port_id, priv->port_old_link[port_id - 1],
	priv->port_old_speed[port_id - 1],
	priv->port_old_duplex[port_id - 1],
	priv->port_old_tx_flowctrl[port_id - 1],
	priv->port_old_rx_flowctrl[port_id - 1]);

	return rv;
}

static sw_error_t
adpt_mp_port_lpi_polling_task(struct qca_phy_priv *priv)
{
	a_uint32_t port_id;
	a_uint32_t portbmp[SW_MAX_NR_DEV] = {0};
	sw_error_t rv = SW_OK;

	portbmp[priv->device_id] = qca_ssdk_port_bmp_get(priv->device_id);

	for (port_id = SSDK_PHYSICAL_PORT1; port_id < SW_MAX_NR_PORT; port_id ++) {

		if(!(portbmp[priv->device_id] & BIT(port_id)))
			continue;
		if (port_lpi_status[priv->device_id] & BIT(port_id-1)) {
			rv = adpt_mp_port_mac_eee_enable_set(priv->device_id,
				port_id, A_TRUE);
			SW_RTN_ON_ERROR(rv);
		}
	}
	return SW_OK;
}

static sw_error_t
adpt_mp_port_speed_get(a_uint32_t dev_id, fal_port_t port_id,
	fal_port_speed_t * pspeed)
{
	sw_error_t rv = 0;
	a_uint32_t phy_id = 0;
	hsl_phy_ops_t *phy_drv;
	struct port_phy_status port_mac_status = {0};

	ADPT_DEV_ID_CHECK(dev_id);
	ADPT_NULL_POINT_CHECK(pspeed);

	if (A_TRUE != hsl_port_prop_check (dev_id, port_id, HSL_PP_PHY)) {
		return SW_BAD_PARAM;
	}

	/* for those ports without PHY device should be s17c port */
	if (A_FALSE == _adpt_mp_port_phy_connected (dev_id, port_id)) {
		rv = adpt_mp_port_mac_status_get(dev_id, port_id, &port_mac_status);
		SW_RTN_ON_ERROR (rv);
		*pspeed= port_mac_status.speed;
	} else {
		SW_RTN_ON_NULL (phy_drv = hsl_phy_api_ops_get (dev_id,
				port_id));
		if (NULL == phy_drv->phy_speed_get) {
			return SW_NOT_SUPPORTED;
		}

		rv = hsl_port_prop_get_phyid (dev_id, port_id, &phy_id);
		SW_RTN_ON_ERROR (rv);
		rv = phy_drv->phy_speed_get (dev_id, phy_id, pspeed);
		SW_RTN_ON_ERROR (rv);
	}

	return rv;
}

static sw_error_t
adpt_mp_port_duplex_get(a_uint32_t dev_id, fal_port_t port_id,
	fal_port_duplex_t * pduplex)
{
	sw_error_t rv = 0;
	a_uint32_t phy_id = 0;
	hsl_phy_ops_t *phy_drv;
	struct port_phy_status port_mac_status = {0};

	ADPT_DEV_ID_CHECK(dev_id);
	ADPT_NULL_POINT_CHECK(pduplex);

	if (A_TRUE != hsl_port_prop_check (dev_id, port_id, HSL_PP_PHY)) {
		return SW_BAD_PARAM;
	}

	/* for those ports without PHY device should be s17c port */
	if (A_FALSE == _adpt_mp_port_phy_connected (dev_id, port_id)) {
		rv = adpt_mp_port_mac_status_get(dev_id, port_id, &port_mac_status);
		SW_RTN_ON_ERROR (rv);
		*pduplex = port_mac_status.duplex;
	} else {
		SW_RTN_ON_NULL (phy_drv = hsl_phy_api_ops_get (dev_id,
				port_id));
		if (NULL == phy_drv->phy_duplex_get) {
			return SW_NOT_SUPPORTED;
		}

		rv = hsl_port_prop_get_phyid (dev_id, port_id, &phy_id);
		SW_RTN_ON_ERROR (rv);
		rv = phy_drv->phy_duplex_get (dev_id, phy_id, pduplex);
		SW_RTN_ON_ERROR (rv);
	}

	return rv;
}

static sw_error_t
adpt_mp_port_link_status_get(a_uint32_t dev_id, fal_port_t port_id,
	a_bool_t * status)
{
	sw_error_t rv = 0;
	a_uint32_t phy_id = 0;
	hsl_phy_ops_t *phy_drv;

	ADPT_DEV_ID_CHECK(dev_id);
	ADPT_NULL_POINT_CHECK(status);

	if (A_TRUE != hsl_port_prop_check (dev_id, port_id, HSL_PP_PHY)) {
		return SW_BAD_PARAM;
	}

	/* for those ports without PHY device should be s17c port */
	if (A_FALSE == _adpt_mp_port_phy_connected (dev_id, port_id)) {
		*status = A_TRUE;
	} else {
		SW_RTN_ON_NULL (phy_drv = hsl_phy_api_ops_get (dev_id,
				port_id));
		if (NULL == phy_drv->phy_link_status_get) {
			return SW_NOT_SUPPORTED;
		}

		rv = hsl_port_prop_get_phyid (dev_id, port_id, &phy_id);
		SW_RTN_ON_ERROR (rv);

		*status = phy_drv->phy_link_status_get (dev_id, phy_id);
	}

	return SW_OK;

}

sw_error_t 
adpt_mp_portctrl_init(a_uint32_t dev_id)
{
	adpt_api_t *p_adpt_api = NULL;

	p_adpt_api = adpt_api_ptr_get(dev_id);

	if(p_adpt_api == NULL)
	{
		return SW_FAIL;
	}
#ifndef IN_PORTCONTROL_MINI
	p_adpt_api->adpt_port_txmac_status_get = adpt_mp_port_txmac_status_get;
	p_adpt_api->adpt_port_rxmac_status_get = adpt_mp_port_rxmac_status_get;
	p_adpt_api->adpt_port_rxfc_status_get = adpt_mp_port_rxfc_status_get;
	p_adpt_api->adpt_port_txfc_status_get = adpt_mp_port_txfc_status_get;
	p_adpt_api->adpt_port_flowctrl_get = adpt_mp_port_flowctrl_get;
	p_adpt_api->adpt_port_flowctrl_forcemode_get =
		adpt_mp_port_flowctrl_forcemode_get;
	p_adpt_api->adpt_port_promisc_mode_get = adpt_mp_port_promisc_mode_get;
	p_adpt_api->adpt_port_interface_3az_status_get = adpt_mp_port_mac_eee_enable_get;
#endif
	p_adpt_api->adpt_port_txmac_status_set = adpt_mp_port_txmac_status_set;
	p_adpt_api->adpt_port_rxmac_status_set = adpt_mp_port_rxmac_status_set;
	p_adpt_api->adpt_port_rxfc_status_set = adpt_mp_port_rxfc_status_set;
	p_adpt_api->adpt_port_txfc_status_set = adpt_mp_port_txfc_status_set;
	p_adpt_api->adpt_port_flowctrl_set = adpt_mp_port_flowctrl_set;
	p_adpt_api->adpt_port_flowctrl_forcemode_set =
		adpt_mp_port_flowctrl_forcemode_set;
	p_adpt_api->adpt_port_max_frame_size_set = adpt_mp_port_max_frame_size_set;
	p_adpt_api->adpt_port_max_frame_size_get = adpt_mp_port_max_frame_size_get;
	p_adpt_api->adpt_port_promisc_mode_set = adpt_mp_port_promisc_mode_set;
	p_adpt_api->adpt_port_mac_speed_set = adpt_mp_port_mac_speed_set;
	p_adpt_api->adpt_port_speed_get = adpt_mp_port_speed_get;
	p_adpt_api->adpt_port_mac_duplex_set = adpt_mp_port_mac_duplex_set;
	p_adpt_api->adpt_port_duplex_get = adpt_mp_port_duplex_get;
	p_adpt_api->adpt_port_link_status_get = adpt_mp_port_link_status_get;
	p_adpt_api->adpt_port_interface_3az_status_set = adpt_mp_port_mac_eee_enable_set;
	p_adpt_api->adpt_port_interface_eee_cfg_set = adpt_mp_port_interface_eee_cfg_set;
	p_adpt_api->adpt_port_interface_eee_cfg_get = adpt_mp_port_interface_eee_cfg_get;
	p_adpt_api->adpt_port_netdev_notify_set = adpt_mp_port_netdev_change_notify;
	p_adpt_api->adpt_port_polling_sw_sync_set = adpt_mp_port_lpi_polling_task;

	return SW_OK;
}
/**
 * @}
 */

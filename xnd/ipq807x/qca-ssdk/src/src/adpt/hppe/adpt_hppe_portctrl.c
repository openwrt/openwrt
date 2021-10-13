/*
 * Copyright (c) 2016-2020, The Linux Foundation. All rights reserved.
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
#include "hppe_xgportctrl_reg.h"
#include "hppe_xgportctrl.h"
#include "hppe_uniphy_reg.h"
#include "hppe_uniphy.h"
#include "hppe_fdb_reg.h"
#include "hppe_fdb.h"
#include "hppe_global_reg.h"
#include "hppe_global.h"
#include "adpt.h"
#include "hsl.h"
#include "hsl_dev.h"
#include "hsl_port_prop.h"
#include "hsl_phy.h"
#include "hppe_init.h"
#include "ssdk_init.h"
#include "ssdk_dts.h"
#include "ssdk_clk.h"
#include "adpt_hppe.h"
#if defined(CPPE)
#include "adpt_cppe_portctrl.h"
#include "cppe_portctrl.h"
#endif
#include "sfp_phy.h"

#define PORT4_PCS_SEL_GMII_FROM_PCS0 1
#define PORT4_PCS_SEL_RGMII 0

#define PORT5_PCS_SEL_RGMII 0
#define PORT5_PCS_SEL_GMII_FROM_PCS0 1
#define PORT5_PCS_SEL_GMII_FROM_PCS1 2
#define PORT5_GMAC_SEL_GMAC 1
#define PORT5_GMAC_SEL_XGMAC 0

#define PORT6_PCS_SEL_RGMII 0
#define PORT6_PCS_SEL_GMII_FROM_PCS2 1
#define PORT6_GMAC_SEL_GMAC 1
#define PORT6_GMAC_SEL_XGMAC 0

#define MAC_SPEED_10M 0
#define MAC_SPEED_100M 1
#define MAC_SPEED_1000M 2
#define MAC_SPEED_10000M 3

#define XGMAC_USXGMII_ENABLE 1
#define XGMAC_USXGMII_CLEAR 0

#define XGMAC_SPEED_SELECT_10000M 0
#define XGMAC_SPEED_SELECT_5000M 1
#define XGMAC_SPEED_SELECT_2500M 2
#define XGMAC_SPEED_SELECT_1000M 3
#define LPI_WAKEUP_TIMER	0x20
#define LPI_SLEEP_TIMER	0x100
#define PROMISCUOUS_MODE 0x1
#define PASS_CONTROL_PACKET 0x2
#define XGMAC_PAUSE_TIME	0xffff
#define CARRIER_SENSE_SIGNAL_FROM_MAC        0x0

#define PHY_PORT_TO_BM_PORT(port)	(port + 7)
#define GMAC_IPG_CHECK 0xc
#define LPI_EEE_TIMER_FREQUENCY 300  /* 300MHZ*/
#define LPI_EEE_TIMER_UNIT 256

/* This register is used to adjust the write timing for reserving
 * some bandwidth of the memory to read operation.
 */
#define GMAC_TX_THD    0x1

static a_uint32_t port_interface_mode[SW_MAX_NR_DEV][SW_MAX_NR_PORT] = {0};

static a_bool_t
_adpt_hppe_port_phy_connected (a_uint32_t dev_id, fal_port_t port_id)
{
	a_uint32_t mode1, mode2;
	a_bool_t force_port = 0;

	ADPT_DEV_ID_CHECK(dev_id);

	/* force port which connect s17c or other device chip*/
	force_port = ssdk_port_feature_get(dev_id, port_id, PHY_F_FORCE);
	if (force_port == A_TRUE) {
		SSDK_DEBUG("port_id %d is a force port!\n", port_id);
		return A_FALSE;
	}

	mode1 = ssdk_dt_global_get_mac_mode(dev_id, SSDK_UNIPHY_INSTANCE1);
	mode2 = ssdk_dt_global_get_mac_mode(dev_id, SSDK_UNIPHY_INSTANCE2);

	if ((SSDK_PHYSICAL_PORT0 == port_id) || (SSDK_PHYSICAL_PORT7 == port_id)||
	((SSDK_PHYSICAL_PORT5 == port_id) && ((mode1 == PORT_WRAPPER_10GBASE_R) ||
		mode1 == PORT_WRAPPER_SGMII_FIBER)) ||
	((SSDK_PHYSICAL_PORT6 == port_id) && ((mode2 == PORT_WRAPPER_10GBASE_R) ||
		mode2 == PORT_WRAPPER_SGMII_FIBER)))
		return A_FALSE;
	else
		return hppe_mac_port_valid_check (dev_id, port_id);
}

static sw_error_t
_adpt_phy_status_get_from_ppe(a_uint32_t dev_id, a_uint32_t port_id,
		struct port_phy_status *phy_status)
{
	sw_error_t rv = SW_OK;
	a_uint32_t reg_field = 0;

	ADPT_DEV_ID_CHECK(dev_id);

	if (port_id == SSDK_PHYSICAL_PORT5)
	{

		if (adpt_hppe_chip_revision_get(dev_id)
			== HPPE_REVISION)
		{
			rv = hppe_port_phy_status_1_port5_1_phy_status_get(dev_id,
							&reg_field);
			SW_RTN_ON_ERROR (rv);
		}
#ifdef CPPE
		else
		{
			rv = cppe_port5_pcs1_phy_status_get(dev_id,
					&reg_field);
			SW_RTN_ON_ERROR (rv);
		}
#endif
	}
	else
	{
		rv = hppe_port_phy_status_1_port6_phy_status_get(dev_id,
						&reg_field);
		SW_RTN_ON_ERROR (rv);
	}

	phy_status->tx_flowctrl = A_TRUE;
	phy_status->rx_flowctrl = A_TRUE;

	if ((reg_field >> 7) & 0x1)
	{
		phy_status->link_status = PORT_LINK_UP;
		switch (reg_field & 0x7)
		{
			case MAC_SPEED_10M:
				phy_status->speed = FAL_SPEED_10;
				break;
			case MAC_SPEED_100M:
				phy_status->speed = FAL_SPEED_100;
				break;
			case MAC_SPEED_1000M:
				phy_status->speed = FAL_SPEED_1000;
				break;
			case MAC_SPEED_10000M:
				phy_status->speed = FAL_SPEED_10000;
				break;
			default:
				phy_status->speed = FAL_SPEED_BUTT;
				break;
		}
		phy_status->duplex = FAL_FULL_DUPLEX;
	}
	else
	{
		phy_status->link_status = PORT_LINK_DOWN;
		phy_status->speed = FAL_SPEED_BUTT;
		phy_status->duplex = FAL_DUPLEX_BUTT;
	}

	return rv;
}
#ifndef IN_PORTCONTROL_MINI
static sw_error_t
_adpt_hppe_port_xgmac_loopback_get(a_uint32_t dev_id, fal_port_t port_id,
				 a_bool_t * enable)
{
	sw_error_t rv = SW_OK;

	port_id = HPPE_TO_XGMAC_PORT_ID(port_id);
	rv = hppe_mac_rx_configuration_lm_get(dev_id, port_id, (a_uint32_t*)enable);

	return rv;
}

static sw_error_t
_adpt_hppe_port_gmac_loopback_get(a_uint32_t dev_id, fal_port_t port_id,
				 a_bool_t * enable)
{
	sw_error_t rv = SW_OK;

	port_id = HPPE_TO_GMAC_PORT_ID(port_id);
	rv = hppe_mac_ctrl2_mac_loop_back_get(dev_id, port_id, (a_uint32_t*)enable);

	return rv;
}

static sw_error_t
_adpt_hppe_port_xgmac_loopback_set(a_uint32_t dev_id, fal_port_t port_id,
				 a_bool_t enable)
{
	sw_error_t rv = SW_OK;

	port_id = HPPE_TO_XGMAC_PORT_ID(port_id);
	rv = hppe_mac_rx_configuration_lm_set(dev_id, port_id, (a_uint32_t)enable);

	return rv;
}

static sw_error_t
_adpt_hppe_port_gmac_loopback_set(a_uint32_t dev_id, fal_port_t port_id,
				 a_bool_t enable)
{
	sw_error_t rv = SW_OK;

	port_id = HPPE_TO_GMAC_PORT_ID(port_id);
	rv = hppe_mac_ctrl2_mac_loop_back_set(dev_id, port_id, (a_uint32_t)enable);

	return rv;
}
#endif
static sw_error_t
_adpt_hppe_port_jumbo_size_set(a_uint32_t dev_id, fal_port_t port_id,
		a_uint32_t jumbo_size)
{
	sw_error_t rv = SW_OK;

	port_id = HPPE_TO_GMAC_PORT_ID(port_id);
	rv = hppe_mac_jumbo_size_mac_jumbo_size_set(dev_id, port_id, jumbo_size);

	return rv;
}

static sw_error_t
_adpt_xgmac_port_max_frame_size_get(a_uint32_t dev_id, fal_port_t port_id,
		a_uint32_t *max_frame)
{
	sw_error_t rv = SW_OK;

	port_id = HPPE_TO_XGMAC_PORT_ID(port_id);
	rv = hppe_mac_rx_configuration_gpsl_get(dev_id,port_id, max_frame);

	return rv;
}

static sw_error_t
_adpt_gmac_port_max_frame_size_get(a_uint32_t dev_id, fal_port_t port_id,
		a_uint32_t *max_frame)
{
	sw_error_t rv = SW_OK;

	port_id = HPPE_TO_GMAC_PORT_ID(port_id);
	rv = hppe_mac_jumbo_size_mac_jumbo_size_get(dev_id, port_id, max_frame);

	return rv;
}

static sw_error_t
_adpt_xgmac_port_max_frame_size_set(a_uint32_t dev_id, fal_port_t port_id,
		a_uint32_t max_frame)
{
	sw_error_t rv = SW_OK;

	port_id = HPPE_TO_XGMAC_PORT_ID(port_id);
	rv |= hppe_mac_tx_configuration_jd_set(dev_id, port_id, (a_uint32_t)A_TRUE);
	rv |= hppe_mac_rx_configuration_gpsl_set(dev_id, port_id, max_frame);
	rv |= hppe_mac_rx_configuration_wd_set(dev_id, port_id, 1);
	rv |= hppe_mac_rx_configuration_gmpslce_set(dev_id, port_id, 1);
	rv |= hppe_mac_packet_filter_pr_set(dev_id, port_id, PROMISCUOUS_MODE);
	rv |= hppe_mac_packet_filter_pcf_set(dev_id, port_id, PASS_CONTROL_PACKET);

	return rv;
}

static sw_error_t
_adpt_gmac_port_max_frame_size_set(a_uint32_t dev_id, fal_port_t port_id,
		a_uint32_t max_frame)
{
	sw_error_t rv = SW_OK;

	port_id = HPPE_TO_GMAC_PORT_ID(port_id);
	rv |= hppe_mac_ctrl2_maxfr_set(dev_id, port_id, max_frame);
	rv |= hppe_mac_ctrl2_crs_sel_set(dev_id, port_id, CARRIER_SENSE_SIGNAL_FROM_MAC);
	rv |= hppe_mac_dbg_ctrl_hihg_ipg_set(dev_id, port_id, GMAC_IPG_CHECK);
	rv |= hppe_mac_ctrl2_mac_tx_thd_set(dev_id, port_id, GMAC_TX_THD);

	return rv;
}
#ifndef IN_PORTCONTROL_MINI
static sw_error_t
_adpt_xgmac_port_rx_status_get(a_uint32_t dev_id, fal_port_t port_id, a_uint32_t* port_rxmac_status)
{
	sw_error_t rv = SW_OK;
	union mac_rx_configuration_u xgmac_rx_enable;

	memset(&xgmac_rx_enable, 0, sizeof(xgmac_rx_enable));
	port_id = HPPE_TO_XGMAC_PORT_ID(port_id);
	rv = hppe_mac_rx_configuration_get(dev_id, port_id,  &xgmac_rx_enable);
	if( rv != SW_OK )
		return rv;
	*port_rxmac_status = xgmac_rx_enable.bf.re;

	return rv;
}
static sw_error_t
_adpt_gmac_port_rx_status_get(a_uint32_t dev_id, fal_port_t port_id, a_uint32_t* port_rxmac_status)
{
	sw_error_t rv = SW_OK;
	union mac_enable_u gmac_rx_enable;

	memset(&gmac_rx_enable, 0, sizeof(gmac_rx_enable));
	port_id = HPPE_TO_GMAC_PORT_ID(port_id);
	rv = hppe_mac_enable_get(dev_id, port_id, &gmac_rx_enable);
	if( rv != SW_OK )
		return rv;
	* port_rxmac_status = gmac_rx_enable.bf.rxmac_en;

	return rv;
}
#endif
static sw_error_t
_adpt_xgmac_port_rx_status_set(a_uint32_t dev_id, fal_port_t port_id, a_bool_t enable)
{
	sw_error_t rv = SW_OK;
	union mac_rx_configuration_u xgmac_rx_enable;

	memset(&xgmac_rx_enable, 0, sizeof(xgmac_rx_enable));
	port_id = HPPE_TO_XGMAC_PORT_ID(port_id);
	rv |=  hppe_mac_rx_configuration_get(dev_id, port_id,  &xgmac_rx_enable);

	xgmac_rx_enable.bf.acs = 1;
	xgmac_rx_enable.bf.cst = 1;
	if (A_TRUE == enable)
	{
		xgmac_rx_enable.bf.re = 1;
	}
	else {
		xgmac_rx_enable.bf.re = 0;
	}
	rv |= hppe_mac_rx_configuration_set(dev_id, port_id, &xgmac_rx_enable);

	return rv;
}

static sw_error_t
_adpt_gmac_port_rx_status_set(a_uint32_t dev_id, fal_port_t port_id, a_bool_t enable)
{
	sw_error_t rv = SW_OK;
	union mac_enable_u gmac_rx_enable;

	memset(&gmac_rx_enable, 0, sizeof(gmac_rx_enable));
	port_id = HPPE_TO_GMAC_PORT_ID(port_id);
	rv |= hppe_mac_enable_get(dev_id, port_id, &gmac_rx_enable);
	if (A_TRUE == enable)
		gmac_rx_enable.bf.rxmac_en = 1;
	if (A_FALSE == enable)
		gmac_rx_enable.bf.rxmac_en = 0;
	rv |= hppe_mac_enable_set(dev_id, port_id, &gmac_rx_enable);

	return rv;
}
#ifndef IN_PORTCONTROL_MINI
static sw_error_t
_adpt_xgmac_port_tx_status_get(a_uint32_t dev_id, fal_port_t port_id, a_uint32_t *port_txmac_status)
{
	sw_error_t rv = SW_OK;
	union mac_tx_configuration_u xgmac_tx_enable;

	memset(&xgmac_tx_enable, 0, sizeof(xgmac_tx_enable));
	port_id = HPPE_TO_XGMAC_PORT_ID(port_id);
	rv = hppe_mac_tx_configuration_get(dev_id, port_id,  &xgmac_tx_enable);
	if( rv != SW_OK )
		return  rv;
	*port_txmac_status = xgmac_tx_enable.bf.te;

	return SW_OK;
}

static sw_error_t
_adpt_gmac_port_tx_status_get(a_uint32_t dev_id, fal_port_t port_id, a_uint32_t *port_txmac_status)
{
	sw_error_t rv = SW_OK;
	union mac_enable_u gmac_tx_enable;

	memset(&gmac_tx_enable, 0, sizeof(gmac_tx_enable));
	port_id = HPPE_TO_GMAC_PORT_ID(port_id);
	rv = hppe_mac_enable_get(dev_id, port_id, &gmac_tx_enable);
	if( rv != SW_OK )
		return  rv;
	*port_txmac_status = gmac_tx_enable.bf.txmac_en;

	return SW_OK;
}
#endif
static sw_error_t
_adpt_xgmac_port_tx_status_set (a_uint32_t dev_id, fal_port_t port_id, a_bool_t enable)
{
	sw_error_t rv = SW_OK;
	union mac_tx_configuration_u xgmac_tx_enable;

	memset(&xgmac_tx_enable, 0, sizeof(xgmac_tx_enable));
	port_id = HPPE_TO_XGMAC_PORT_ID(port_id);
	 rv |=hppe_mac_tx_configuration_get(dev_id, port_id,  &xgmac_tx_enable);
	 if (A_TRUE == enable)
		 xgmac_tx_enable.bf.te = 1;
	 if (A_FALSE == enable)
		 xgmac_tx_enable.bf.te = 0;
	 rv |= hppe_mac_tx_configuration_set(dev_id, port_id, &xgmac_tx_enable);

	 return SW_OK;
}

static sw_error_t
_adpt_gmac_port_tx_status_set (a_uint32_t dev_id, fal_port_t port_id, a_bool_t enable)
{
	sw_error_t rv = SW_OK;
	union mac_enable_u gmac_tx_enable;

	memset(&gmac_tx_enable, 0, sizeof(gmac_tx_enable));
	port_id = HPPE_TO_GMAC_PORT_ID(port_id);
	rv |= hppe_mac_enable_get(dev_id, port_id, &gmac_tx_enable);
	if (A_TRUE == enable)
		gmac_tx_enable.bf.txmac_en = 1;
	if (A_FALSE == enable)
		gmac_tx_enable.bf.txmac_en = 0;
	rv |= hppe_mac_enable_set(dev_id, port_id, &gmac_tx_enable);

	return SW_OK;
}

static sw_error_t
_adpt_xgmac_port_txfc_status_get(a_uint32_t dev_id, fal_port_t port_id, a_uint32_t* txfc_status)
{
	sw_error_t rv = SW_OK;
	union mac_q0_tx_flow_ctrl_u xgmac_txfc_enable;

	memset(&xgmac_txfc_enable, 0, sizeof(xgmac_txfc_enable));
	port_id = HPPE_TO_XGMAC_PORT_ID(port_id);
	rv = hppe_mac_q0_tx_flow_ctrl_get(dev_id, port_id, &xgmac_txfc_enable);
	if( rv != SW_OK )
		return rv;
	*txfc_status = xgmac_txfc_enable.bf.tfe;

	return  SW_OK;
}

static sw_error_t
_adpt_gmac_port_txfc_status_get(a_uint32_t dev_id, fal_port_t port_id, a_uint32_t* txfc_status)
{
	sw_error_t rv = SW_OK;
	union mac_enable_u gmac_txfc_enable;

	memset(&gmac_txfc_enable, 0, sizeof(gmac_txfc_enable));
	port_id = HPPE_TO_GMAC_PORT_ID(port_id);
	rv = hppe_mac_enable_get(dev_id, port_id, &gmac_txfc_enable);
	if( rv != SW_OK )
		return rv;
	*txfc_status = gmac_txfc_enable.bf.tx_flow_en;

	return  SW_OK;
}

static sw_error_t
_adpt_xgmac_port_txfc_status_set(a_uint32_t dev_id, fal_port_t port_id,  a_bool_t enable)
{
	sw_error_t rv = SW_OK;
	union mac_q0_tx_flow_ctrl_u xgmac_txfc_enable;

	memset(&xgmac_txfc_enable, 0, sizeof(xgmac_txfc_enable));
	port_id = HPPE_TO_XGMAC_PORT_ID(port_id);
	rv |= hppe_mac_q0_tx_flow_ctrl_get(dev_id, port_id,  &xgmac_txfc_enable);
	if (A_TRUE == enable)
	{
		xgmac_txfc_enable.bf.tfe = 1;
		xgmac_txfc_enable.bf.pt = XGMAC_PAUSE_TIME;
	}
	if (A_FALSE == enable)
		xgmac_txfc_enable.bf.tfe = 0;
	 rv |= hppe_mac_q0_tx_flow_ctrl_set(dev_id, port_id,  &xgmac_txfc_enable);

	 return SW_OK;
}

static sw_error_t
_adpt_gmac_port_txfc_status_set(a_uint32_t dev_id, fal_port_t port_id,  a_bool_t enable)
{
	sw_error_t rv = SW_OK;
	union mac_enable_u gmac_txfc_enable;

	memset(&gmac_txfc_enable, 0, sizeof(gmac_txfc_enable));
	port_id = HPPE_TO_GMAC_PORT_ID(port_id);
	rv |= hppe_mac_enable_get(dev_id, port_id, &gmac_txfc_enable);
	if (A_TRUE == enable)
		gmac_txfc_enable.bf.tx_flow_en = 1;
	if (A_FALSE == enable)
		gmac_txfc_enable.bf.tx_flow_en = 0;
	rv |= hppe_mac_enable_set(dev_id, port_id, &gmac_txfc_enable);

	return SW_OK;
}

static sw_error_t
_adpt_xgmac_port_rxfc_status_get(a_uint32_t dev_id, fal_port_t port_id,  a_uint32_t* rxfc_status)
{
	sw_error_t rv = SW_OK;
	union mac_rx_flow_ctrl_u xgmac_rxfc_enable;

	memset(&xgmac_rxfc_enable, 0, sizeof(xgmac_rxfc_enable));
	port_id = HPPE_TO_XGMAC_PORT_ID(port_id);
	rv = hppe_mac_rx_flow_ctrl_get(dev_id, port_id, &xgmac_rxfc_enable);
	if(rv != SW_OK)
		return rv;
	*rxfc_status = xgmac_rxfc_enable.bf.rfe;

	return  SW_OK;
}

static sw_error_t
_adpt_gmac_port_rxfc_status_get(a_uint32_t dev_id, fal_port_t port_id, a_uint32_t* rxfc_status)
{
	sw_error_t rv = SW_OK;
	union mac_enable_u gmac_rxfc_enable;

	memset(&gmac_rxfc_enable, 0, sizeof(gmac_rxfc_enable));
	port_id = HPPE_TO_GMAC_PORT_ID(port_id);
	rv = hppe_mac_enable_get(dev_id, port_id, &gmac_rxfc_enable);
	if( rv != SW_OK)
		return rv;
	*rxfc_status = gmac_rxfc_enable.bf.rx_flow_en;

	return SW_OK;
}

static sw_error_t
_adpt_xgmac_port_rxfc_status_set(a_uint32_t dev_id,fal_port_t port_id, a_bool_t enable)
{
	sw_error_t rv = SW_OK;
	union mac_rx_flow_ctrl_u xgmac_rxfc_enable;

	memset(&xgmac_rxfc_enable, 0, sizeof(xgmac_rxfc_enable));
	port_id = HPPE_TO_XGMAC_PORT_ID(port_id);
	rv |= hppe_mac_rx_flow_ctrl_get(dev_id, port_id, &xgmac_rxfc_enable);
	if (A_TRUE == enable)
		xgmac_rxfc_enable.bf.rfe= 1;
	if (A_FALSE == enable)
		xgmac_rxfc_enable.bf.rfe = 0;
	rv |= hppe_mac_rx_flow_ctrl_set(dev_id, port_id, &xgmac_rxfc_enable);

	return SW_OK;
}

static sw_error_t
_adpt_gmac_port_rxfc_status_set(a_uint32_t dev_id,fal_port_t port_id, a_bool_t enable)
{
	sw_error_t rv = SW_OK;
	union mac_enable_u gmac_rxfc_enable;

	memset(&gmac_rxfc_enable, 0, sizeof(gmac_rxfc_enable));
	port_id = HPPE_TO_GMAC_PORT_ID(port_id);
	rv |= hppe_mac_enable_get(dev_id, port_id, &gmac_rxfc_enable);
	if (A_TRUE == enable)
		gmac_rxfc_enable.bf.rx_flow_en = 1;
	if (A_FALSE == enable)
		gmac_rxfc_enable.bf.rx_flow_en = 0;
	rv |= hppe_mac_enable_set(dev_id, port_id, &gmac_rxfc_enable);

	return SW_OK;
}

#ifndef IN_PORTCONTROL_MINI
sw_error_t
adpt_hppe_port_local_loopback_get(a_uint32_t dev_id, fal_port_t port_id,
					a_bool_t * enable)
{

	sw_error_t rv = 0;
	a_uint32_t phy_id = 0;
	hsl_phy_ops_t *phy_drv;

	ADPT_DEV_ID_CHECK(dev_id);
	ADPT_NULL_POINT_CHECK(enable);

	if (A_TRUE != hsl_port_prop_check (dev_id, port_id, HSL_PP_PHY))
	  {
		return SW_BAD_PARAM;
	  }
	if (A_FALSE == _adpt_hppe_port_phy_connected (dev_id, port_id))
		return SW_NOT_SUPPORTED;

	SW_RTN_ON_NULL (phy_drv = hsl_phy_api_ops_get (dev_id, port_id));
	if (NULL == phy_drv->phy_local_loopback_get)
		return SW_NOT_SUPPORTED;

	rv = hsl_port_prop_get_phyid (dev_id, port_id, &phy_id);
	SW_RTN_ON_ERROR (rv);

	rv = phy_drv->phy_local_loopback_get (dev_id, phy_id, enable);

	return rv;

}
#endif
sw_error_t
adpt_hppe_port_autoneg_restart(a_uint32_t dev_id, fal_port_t port_id)
{

	sw_error_t rv = 0;
	a_uint32_t phy_id;
	hsl_phy_ops_t *phy_drv;

	ADPT_DEV_ID_CHECK(dev_id);

	if (A_TRUE != hsl_port_prop_check (dev_id, port_id, HSL_PP_PHY))
	  {
		return SW_BAD_PARAM;
	  }
	if (A_FALSE == _adpt_hppe_port_phy_connected (dev_id, port_id))
		return SW_NOT_SUPPORTED;

	SW_RTN_ON_NULL (phy_drv = hsl_phy_api_ops_get (dev_id, port_id));
	if (NULL == phy_drv->phy_restart_autoneg)
		return SW_NOT_SUPPORTED;

	rv = hsl_port_prop_get_phyid (dev_id, port_id, &phy_id);
	SW_RTN_ON_ERROR (rv);

	rv = phy_drv->phy_restart_autoneg (dev_id, phy_id);
	return rv;

}
sw_error_t
adpt_hppe_port_duplex_set(a_uint32_t dev_id, fal_port_t port_id,
				fal_port_duplex_t duplex)
{
	union mac_enable_u mac_enable;
	sw_error_t rv = 0;
	a_uint32_t phy_id = 0;
	hsl_phy_ops_t *phy_drv;

	memset(&mac_enable, 0, sizeof(mac_enable));
	ADPT_DEV_ID_CHECK(dev_id);

	if (A_TRUE != hsl_port_prop_check (dev_id, port_id, HSL_PP_INCL_CPU))
	  {
		return SW_BAD_PARAM;
	  }
	if (A_FALSE == _adpt_hppe_port_phy_connected (dev_id, port_id))
		return SW_NOT_SUPPORTED;

	SW_RTN_ON_NULL (phy_drv = hsl_phy_api_ops_get (dev_id, port_id));
	if (NULL == phy_drv->phy_duplex_set)
		return SW_NOT_SUPPORTED;

	if (FAL_DUPLEX_BUTT <= duplex)
	{
		return SW_BAD_PARAM;
	}
	rv = hsl_port_prop_get_phyid (dev_id, port_id, &phy_id);
	SW_RTN_ON_ERROR (rv);

	rv = phy_drv->phy_duplex_set (dev_id, phy_id, duplex);
	SW_RTN_ON_ERROR (rv);

#if 0
	port_id = port_id - 1;
	hppe_mac_enable_get(dev_id, port_id, &mac_enable);

	if (FAL_HALF_DUPLEX == duplex)
		mac_enable.bf.duplex = 0;
	if (FAL_FULL_DUPLEX == duplex)
		mac_enable.bf.duplex = 1;

	hppe_mac_enable_set(dev_id, port_id, &mac_enable);

#endif

	return rv;
}
#ifndef IN_PORTCONTROL_MINI
sw_error_t
adpt_hppe_port_rxmac_status_get(a_uint32_t dev_id, fal_port_t port_id,
				      a_bool_t * enable)
{
	sw_error_t rv = SW_OK;
	a_uint32_t port_rxmac_status = 0, port_mac_type;

	ADPT_DEV_ID_CHECK(dev_id);
	ADPT_NULL_POINT_CHECK(enable);

	port_mac_type = qca_hppe_port_mac_type_get(dev_id, port_id);
	if (port_mac_type == PORT_XGMAC_TYPE)
		_adpt_xgmac_port_rx_status_get( dev_id, port_id, &port_rxmac_status);
	else if (port_mac_type == PORT_GMAC_TYPE)
		 _adpt_gmac_port_rx_status_get( dev_id, port_id, &port_rxmac_status);
	else
		return SW_BAD_VALUE;

	if (port_rxmac_status)
		*enable = A_TRUE;
	else
		*enable = A_FALSE;

	return rv;
}

sw_error_t
adpt_hppe_port_cdt(a_uint32_t dev_id, fal_port_t port_id,
		a_uint32_t mdi_pair, fal_cable_status_t * cable_status,
		a_uint32_t * cable_len)
{

	sw_error_t rv = 0;
	a_uint32_t phy_id = 0;
	hsl_phy_ops_t *phy_drv;

	ADPT_DEV_ID_CHECK(dev_id);
	ADPT_NULL_POINT_CHECK(cable_status);
	ADPT_NULL_POINT_CHECK(cable_len);

	if (A_TRUE != hsl_port_prop_check (dev_id, port_id, HSL_PP_PHY))
	  {
		return SW_BAD_PARAM;
	  }
	if (A_FALSE == _adpt_hppe_port_phy_connected (dev_id, port_id))
		return SW_NOT_SUPPORTED;

	SW_RTN_ON_NULL (phy_drv = hsl_phy_api_ops_get (dev_id, port_id));
	if (NULL == phy_drv->phy_cdt)
		return SW_NOT_SUPPORTED;

	rv = hsl_port_prop_get_phyid (dev_id, port_id, &phy_id);
	SW_RTN_ON_ERROR (rv);

	rv = phy_drv->phy_cdt (dev_id, phy_id, mdi_pair, cable_status, cable_len);

	return rv;

}
#endif
sw_error_t
adpt_hppe_port_txmac_status_set(a_uint32_t dev_id, fal_port_t port_id,
				      a_bool_t enable)
{
	a_uint32_t port_mac_type;

	ADPT_DEV_ID_CHECK(dev_id);

	port_mac_type =qca_hppe_port_mac_type_get(dev_id, port_id);
	if (port_mac_type == PORT_XGMAC_TYPE)
		_adpt_xgmac_port_tx_status_set( dev_id, port_id, enable);
	else if (port_mac_type == PORT_GMAC_TYPE)
		_adpt_gmac_port_tx_status_set( dev_id, port_id, enable);
	else
		return SW_BAD_VALUE;

	return SW_OK;
}
#ifndef IN_PORTCONTROL_MINI
sw_error_t
adpt_hppe_port_combo_fiber_mode_set(a_uint32_t dev_id,
						  a_uint32_t port_id,
						  fal_port_fiber_mode_t mode)
{

	sw_error_t rv = 0;
	a_uint32_t phy_id = 0;
	hsl_phy_ops_t *phy_drv;

	ADPT_DEV_ID_CHECK(dev_id);

	if (A_TRUE != hsl_port_prop_check (dev_id, port_id, HSL_PP_PHY))
	  {
		return SW_BAD_PARAM;
	  }
	if (A_FALSE == _adpt_hppe_port_phy_connected (dev_id, port_id))
		return SW_NOT_SUPPORTED;

	SW_RTN_ON_NULL (phy_drv = hsl_phy_api_ops_get (dev_id, port_id));
	if (NULL == phy_drv->phy_combo_fiber_mode_set)
		return SW_NOT_SUPPORTED;

	rv = hsl_port_prop_get_phyid (dev_id, port_id, &phy_id);
	SW_RTN_ON_ERROR (rv);

	rv = phy_drv->phy_combo_fiber_mode_set (dev_id, phy_id, mode);

	return rv;

}
sw_error_t
adpt_hppe_port_combo_medium_status_get(a_uint32_t dev_id,
						     a_uint32_t port_id,
						     fal_port_medium_t *
						     medium)
{

	sw_error_t rv = 0;
	a_uint32_t phy_id = 0;
	hsl_phy_ops_t *phy_drv;

	ADPT_DEV_ID_CHECK(dev_id);
	ADPT_NULL_POINT_CHECK(medium);

	if (A_TRUE != hsl_port_prop_check (dev_id, port_id, HSL_PP_PHY))
	  {
		return SW_BAD_PARAM;
	  }
	if (A_FALSE == _adpt_hppe_port_phy_connected (dev_id, port_id))
		return SW_NOT_SUPPORTED;

	SW_RTN_ON_NULL (phy_drv = hsl_phy_api_ops_get (dev_id, port_id));
	if (NULL == phy_drv->phy_combo_medium_status_get)
		return SW_NOT_SUPPORTED;

	rv = hsl_port_prop_get_phyid (dev_id, port_id, &phy_id);
	SW_RTN_ON_ERROR (rv);

	rv = phy_drv->phy_combo_medium_status_get (dev_id, phy_id, medium);

	return rv;

}

sw_error_t
adpt_hppe_port_magic_frame_mac_set(a_uint32_t dev_id, fal_port_t port_id,
				   fal_mac_addr_t * mac)
{
	sw_error_t rv;
	a_uint32_t phy_id = 0;
	hsl_phy_ops_t *phy_drv;

	ADPT_DEV_ID_CHECK(dev_id);
	ADPT_NULL_POINT_CHECK(mac);

	if (A_TRUE != hsl_port_prop_check (dev_id, port_id, HSL_PP_PHY))
	  {
		return SW_BAD_PARAM;
	  }
	if (A_FALSE == _adpt_hppe_port_phy_connected (dev_id, port_id))
		return SW_NOT_SUPPORTED;

	SW_RTN_ON_NULL (phy_drv = hsl_phy_api_ops_get (dev_id, port_id));
	if (NULL == phy_drv->phy_magic_frame_mac_set)
		return SW_NOT_SUPPORTED;

	rv = hsl_port_prop_get_phyid (dev_id, port_id, &phy_id);
	SW_RTN_ON_ERROR (rv);

	rv = phy_drv->phy_magic_frame_mac_set (dev_id, phy_id,mac);

	return rv;

}
sw_error_t
adpt_hppe_port_powersave_set(a_uint32_t dev_id, fal_port_t port_id,
			  a_bool_t enable)
{
	sw_error_t rv = 0;
	a_uint32_t phy_id = 0;
	hsl_phy_ops_t *phy_drv;

	ADPT_DEV_ID_CHECK(dev_id);

	if (A_TRUE != hsl_port_prop_check (dev_id, port_id, HSL_PP_PHY))
	  {
		return SW_BAD_PARAM;
	  }
	if (A_FALSE == _adpt_hppe_port_phy_connected (dev_id, port_id))
		return SW_NOT_SUPPORTED;

	SW_RTN_ON_NULL (phy_drv = hsl_phy_api_ops_get (dev_id, port_id));
	if (NULL == phy_drv->phy_powersave_set)
		return SW_NOT_SUPPORTED;

	rv = hsl_port_prop_get_phyid (dev_id, port_id, &phy_id);
	SW_RTN_ON_ERROR (rv);

	rv = phy_drv->phy_powersave_set (dev_id, phy_id, enable);

	return rv;

}
sw_error_t
adpt_hppe_port_hibernate_set(a_uint32_t dev_id, fal_port_t port_id,
			  a_bool_t enable)
{

	sw_error_t rv = 0;
	a_uint32_t phy_id = 0;
	hsl_phy_ops_t *phy_drv;

	ADPT_DEV_ID_CHECK(dev_id);

	if (A_TRUE != hsl_port_prop_check (dev_id, port_id, HSL_PP_PHY))
	  {
		return SW_BAD_PARAM;
	  }
	if (A_FALSE == _adpt_hppe_port_phy_connected (dev_id, port_id))
		return SW_NOT_SUPPORTED;

	SW_RTN_ON_NULL (phy_drv = hsl_phy_api_ops_get (dev_id, port_id));
	if (NULL == phy_drv->phy_hibernation_set)
		return SW_NOT_SUPPORTED;

	rv = hsl_port_prop_get_phyid (dev_id, port_id, &phy_id);
	SW_RTN_ON_ERROR (rv);

	rv = phy_drv->phy_hibernation_set (dev_id, phy_id, enable);

	return rv;

}

sw_error_t
adpt_hppe_port_mru_set(a_uint32_t dev_id, fal_port_t port_id,
		fal_mru_ctrl_t *ctrl)
{
	ADPT_DEV_ID_CHECK(dev_id);
	ADPT_NULL_POINT_CHECK(ctrl);

	hppe_mru_mtu_ctrl_tbl_mru_set(dev_id, port_id, ctrl->mru_size);
	hppe_mru_mtu_ctrl_tbl_mru_cmd_set(dev_id, port_id, (a_uint32_t)ctrl->action);

	return SW_OK;
}

sw_error_t
adpt_ppe_port_mru_set(a_uint32_t dev_id, fal_port_t port_id,
		fal_mru_ctrl_t *ctrl)
{
	a_uint32_t chip_ver = 0;

	ADPT_DEV_ID_CHECK(dev_id);
	ADPT_NULL_POINT_CHECK(ctrl);

	chip_ver = adpt_hppe_chip_revision_get(dev_id);
	if (chip_ver == CPPE_REVISION) {
#if defined(CPPE)
		return adpt_cppe_port_mru_set(dev_id, port_id, ctrl);
#endif
	} else {
		return adpt_hppe_port_mru_set(dev_id, port_id, ctrl);
	}

	return SW_NOT_SUPPORTED;
}

sw_error_t
adpt_hppe_port_mtu_set(a_uint32_t dev_id, fal_port_t port_id,
		fal_mtu_ctrl_t *ctrl)
{
	union mru_mtu_ctrl_tbl_u mru_mtu_ctrl_tbl;

	memset(&mru_mtu_ctrl_tbl, 0, sizeof(mru_mtu_ctrl_tbl));
	ADPT_DEV_ID_CHECK(dev_id);
	ADPT_NULL_POINT_CHECK(ctrl);

	hppe_mru_mtu_ctrl_tbl_get(dev_id, port_id, &mru_mtu_ctrl_tbl);
	mru_mtu_ctrl_tbl.bf.mtu = ctrl->mtu_size;
	mru_mtu_ctrl_tbl.bf.mtu_cmd = (a_uint32_t)ctrl->action;
	hppe_mru_mtu_ctrl_tbl_set(dev_id, port_id, &mru_mtu_ctrl_tbl);

	if ((port_id >= SSDK_PHYSICAL_PORT0) && (port_id <= SSDK_PHYSICAL_PORT7))
	{
		hppe_mc_mtu_ctrl_tbl_mtu_set(dev_id, port_id, ctrl->mtu_size);
		hppe_mc_mtu_ctrl_tbl_mtu_cmd_set(dev_id, port_id, (a_uint32_t)ctrl->action);
	}

	return SW_OK;
}

sw_error_t
adpt_ppe_port_mtu_set(a_uint32_t dev_id, fal_port_t port_id,
		fal_mtu_ctrl_t *ctrl)
{
	a_uint32_t chip_ver = 0;

	ADPT_DEV_ID_CHECK(dev_id);
	ADPT_NULL_POINT_CHECK(ctrl);

	chip_ver = adpt_hppe_chip_revision_get(dev_id);
	if (chip_ver == CPPE_REVISION) {
#if defined(CPPE)
		return adpt_cppe_port_mtu_set(dev_id, port_id, ctrl);
#endif
	} else {
		return adpt_hppe_port_mtu_set(dev_id, port_id, ctrl);
	}

	return SW_NOT_SUPPORTED;
}
#endif

sw_error_t
adpt_hppe_port_max_frame_size_set(a_uint32_t dev_id, fal_port_t port_id,
		a_uint32_t max_frame)
{
	a_uint32_t port_mac_type = 0;
	sw_error_t rv = SW_OK;

	ADPT_DEV_ID_CHECK(dev_id);

	if (max_frame > SSDK_MAX_FRAME_SIZE) {
		return SW_BAD_VALUE;
	}

	port_mac_type =qca_hppe_port_mac_type_get(dev_id, port_id);
	if (port_mac_type == PORT_XGMAC_TYPE)
		rv |= _adpt_xgmac_port_max_frame_size_set( dev_id, port_id, max_frame);
	else if (port_mac_type == PORT_GMAC_TYPE)
	{
		/*for gmac, rxtoolong have counters when package length is longer than jumbo size and shorter than max frame size,
		   when package length is longer than max frame size, the rxbadbyte have counters.*/
		rv |= _adpt_hppe_port_jumbo_size_set(dev_id, port_id, max_frame);
		rv |= _adpt_gmac_port_max_frame_size_set( dev_id, port_id, max_frame);
	}
	else
		return SW_BAD_VALUE;

	return rv;
}

sw_error_t
adpt_ppe_port_max_frame_size_set(a_uint32_t dev_id, fal_port_t port_id,
		a_uint32_t max_frame)
{
#ifdef CPPE
	if (adpt_hppe_chip_revision_get(dev_id) == CPPE_REVISION &&
		port_id == SSDK_PHYSICAL_PORT6)
	{
		return adpt_cppe_lpbk_max_frame_size_set(dev_id, port_id, max_frame);
	}
#endif
	return adpt_hppe_port_max_frame_size_set(dev_id, port_id, max_frame);
}

#ifndef IN_PORTCONTROL_MINI
sw_error_t
adpt_hppe_port_8023az_get(a_uint32_t dev_id, fal_port_t port_id,
				a_bool_t * enable)
{

	sw_error_t rv = 0;
	a_uint32_t phy_id = 0;
	hsl_phy_ops_t *phy_drv;

	ADPT_DEV_ID_CHECK(dev_id);
	ADPT_NULL_POINT_CHECK(enable);

	if (A_TRUE != hsl_port_prop_check (dev_id, port_id, HSL_PP_PHY))
	  {
		return SW_BAD_PARAM;
	  }
	if (A_FALSE == _adpt_hppe_port_phy_connected (dev_id, port_id))
		return SW_NOT_SUPPORTED;

	SW_RTN_ON_NULL (phy_drv = hsl_phy_api_ops_get (dev_id, port_id));
	if (NULL == phy_drv->phy_8023az_get)
		return SW_NOT_SUPPORTED;

	rv = hsl_port_prop_get_phyid (dev_id, port_id, &phy_id);
	SW_RTN_ON_ERROR (rv);

	rv = phy_drv->phy_8023az_get (dev_id, phy_id, enable);

	return rv;

}
#endif
sw_error_t
adpt_hppe_port_rxfc_status_get(a_uint32_t dev_id, fal_port_t port_id,
				     a_bool_t * enable)
{
	sw_error_t rv = SW_OK;
	a_uint32_t rxfc_status = 0, port_mac_type;

	ADPT_DEV_ID_CHECK(dev_id);
	ADPT_NULL_POINT_CHECK(enable);

	port_mac_type =qca_hppe_port_mac_type_get(dev_id, port_id);
	if (port_mac_type == PORT_XGMAC_TYPE)
		_adpt_xgmac_port_rxfc_status_get( dev_id, port_id, &rxfc_status);
	else if (port_mac_type == PORT_GMAC_TYPE)
		_adpt_gmac_port_rxfc_status_get( dev_id, port_id, &rxfc_status);
	else
		return SW_BAD_VALUE;

	if (rxfc_status)
		*enable = A_TRUE;
	else
		*enable = A_FALSE;

	return rv;
}

sw_error_t
adpt_hppe_port_txfc_status_get(a_uint32_t dev_id, fal_port_t port_id,
				     a_bool_t * enable)
{
	a_uint32_t txfc_status = 0, port_mac_type;

	ADPT_DEV_ID_CHECK(dev_id);
	ADPT_NULL_POINT_CHECK(enable);

	port_mac_type =qca_hppe_port_mac_type_get(dev_id, port_id);
	if (port_mac_type == PORT_XGMAC_TYPE)
		_adpt_xgmac_port_txfc_status_get( dev_id, port_id, &txfc_status);
	else if (port_mac_type == PORT_GMAC_TYPE)
		_adpt_gmac_port_txfc_status_get( dev_id, port_id, &txfc_status);
	else
		return SW_BAD_VALUE;

	if (txfc_status)
		*enable = A_TRUE;
	else
		*enable = A_FALSE;

	return SW_OK;
}
#ifndef IN_PORTCONTROL_MINI
sw_error_t
adpt_hppe_port_remote_loopback_set(a_uint32_t dev_id, fal_port_t port_id,
					 a_bool_t enable)
{

	sw_error_t rv = 0;
	a_uint32_t phy_id = 0;
	hsl_phy_ops_t *phy_drv;

	ADPT_DEV_ID_CHECK(dev_id);

	if (A_TRUE != hsl_port_prop_check (dev_id, port_id, HSL_PP_PHY))
	  {
		return SW_BAD_PARAM;
	  }
	if (A_FALSE == _adpt_hppe_port_phy_connected (dev_id, port_id))
		return SW_NOT_SUPPORTED;

	SW_RTN_ON_NULL (phy_drv = hsl_phy_api_ops_get (dev_id, port_id));
	if (NULL == phy_drv->phy_remote_loopback_set)
		return SW_NOT_SUPPORTED;

	rv = hsl_port_prop_get_phyid (dev_id, port_id, &phy_id);
	SW_RTN_ON_ERROR (rv);

	rv = phy_drv->phy_remote_loopback_set (dev_id, phy_id, enable);

	return rv;

}
#endif
sw_error_t
adpt_hppe_port_autoneg_status_get(a_uint32_t dev_id, fal_port_t port_id,
					a_bool_t * status)
{

	a_uint32_t phy_id = 0;
	sw_error_t rv = 0;
	hsl_phy_ops_t *phy_drv;

	ADPT_DEV_ID_CHECK(dev_id);
	ADPT_NULL_POINT_CHECK(status);

	if (A_TRUE != hsl_port_prop_check (dev_id, port_id, HSL_PP_PHY))
	  {
		return SW_BAD_PARAM;
	  }
	if (A_FALSE == _adpt_hppe_port_phy_connected (dev_id, port_id))
		return SW_NOT_SUPPORTED;

	SW_RTN_ON_NULL (phy_drv = hsl_phy_api_ops_get (dev_id, port_id));
	if (NULL == phy_drv->phy_autoneg_status_get)
		return SW_NOT_SUPPORTED;

	rv = hsl_port_prop_get_phyid (dev_id, port_id, &phy_id);
	SW_RTN_ON_ERROR (rv);

	*status = phy_drv->phy_autoneg_status_get (dev_id, phy_id);

	return SW_OK;

}
#ifndef IN_PORTCONTROL_MINI
sw_error_t
adpt_hppe_port_txmac_status_get(a_uint32_t dev_id, fal_port_t port_id,
				      a_bool_t * enable)
{
	sw_error_t rv = SW_OK;
	a_uint32_t port_txmac_status = 0, port_mac_type;

	ADPT_DEV_ID_CHECK(dev_id);
	ADPT_NULL_POINT_CHECK(enable);

	port_mac_type =qca_hppe_port_mac_type_get(dev_id, port_id);
	if (port_mac_type == PORT_XGMAC_TYPE)
		_adpt_xgmac_port_tx_status_get( dev_id, port_id, &port_txmac_status);
	else if (port_mac_type == PORT_GMAC_TYPE)
		_adpt_gmac_port_tx_status_get( dev_id, port_id, &port_txmac_status);
	else
		return SW_BAD_VALUE;

	if (port_txmac_status)
		*enable = A_TRUE;
	else
		*enable = A_FALSE;

	return rv;
}

sw_error_t
adpt_hppe_port_mdix_get(a_uint32_t dev_id, fal_port_t port_id,
			      fal_port_mdix_mode_t * mode)
{

	sw_error_t rv = 0;
	a_uint32_t phy_id = 0;
	hsl_phy_ops_t *phy_drv;

	ADPT_DEV_ID_CHECK(dev_id);
	ADPT_NULL_POINT_CHECK(mode);

	if (A_TRUE != hsl_port_prop_check (dev_id, port_id, HSL_PP_PHY))
	  {
		return SW_BAD_PARAM;
	  }
	if (A_FALSE == _adpt_hppe_port_phy_connected (dev_id, port_id))
		return SW_NOT_SUPPORTED;

	SW_RTN_ON_NULL (phy_drv = hsl_phy_api_ops_get (dev_id, port_id));
	if (NULL == phy_drv->phy_mdix_get)
		return SW_NOT_SUPPORTED;

	rv = hsl_port_prop_get_phyid (dev_id, port_id, &phy_id);
	SW_RTN_ON_ERROR (rv);

	rv = phy_drv->phy_mdix_get (dev_id, phy_id, mode);

	return rv;

}

sw_error_t
adpt_hppe_ports_link_status_get(a_uint32_t dev_id, a_uint32_t * status)
{

	sw_error_t rv = 0;
	a_uint32_t port_id;
	a_uint32_t phy_id;
	hsl_dev_t *pdev = NULL;
	hsl_phy_ops_t *phy_drv;
	struct port_phy_status phy_status = {0};

	ADPT_DEV_ID_CHECK(dev_id);
	ADPT_NULL_POINT_CHECK(status);

	pdev = hsl_dev_ptr_get(dev_id);
	if (pdev == NULL)
		return SW_NOT_INITIALIZED;

	*status = 0x0;
	for (port_id = 0; port_id < SW_MAX_NR_PORT; port_id++)
	{
		/* for those ports without PHY device should be sfp port */
		if (A_FALSE == _adpt_hppe_port_phy_connected(dev_id, port_id))
		{
			if (hsl_port_prop_check(dev_id, port_id, HSL_PP_CPU) ||
				hsl_port_prop_check(dev_id, port_id, HSL_PP_INNER))
			{
				*status |= (0x1 << port_id);
			}
			else
			{
				if(!hsl_port_prop_check(dev_id, port_id, HSL_PP_PHY))
				{
					continue;
				}
				rv = _adpt_phy_status_get_from_ppe(dev_id,
					port_id, &phy_status);
				SW_RTN_ON_ERROR (rv);

				if (phy_status.link_status == PORT_LINK_UP)
				{
					*status |= (0x1 << port_id);
				}
				else
				{
					*status &= ~(0x1 << port_id);
				}
			}
		}
		else
		{
			SW_RTN_ON_NULL (phy_drv = hsl_phy_api_ops_get (dev_id, port_id));
			if (NULL == phy_drv->phy_link_status_get)
			{
				return SW_NOT_SUPPORTED;
			}
			rv = hsl_port_prop_get_phyid(dev_id, port_id, &phy_id);
			SW_RTN_ON_ERROR(rv);

			if (A_TRUE == phy_drv->phy_link_status_get (dev_id, phy_id))
			{
				*status |= (0x1 << port_id);
			}
			else
			{
				*status &= ~(0x1 << port_id);
			}
		}
	}
	return SW_OK;

}

sw_error_t
adpt_hppe_port_mac_loopback_set(a_uint32_t dev_id, fal_port_t port_id,
				 a_bool_t enable)
{
	sw_error_t rv = SW_OK;
	a_uint32_t port_mac_type = 0;

	ADPT_DEV_ID_CHECK(dev_id);
	port_mac_type = qca_hppe_port_mac_type_get(dev_id, port_id);
	if (port_mac_type == PORT_XGMAC_TYPE)
		rv = _adpt_hppe_port_xgmac_loopback_set( dev_id, port_id, enable);
	else if(port_mac_type == PORT_GMAC_TYPE)
		rv =  _adpt_hppe_port_gmac_loopback_set( dev_id, port_id, enable);
	else
		return SW_BAD_VALUE;

	return rv;
}

sw_error_t
adpt_hppe_port_phy_id_get(a_uint32_t dev_id, fal_port_t port_id,
		      a_uint16_t * org_id, a_uint16_t * rev_id)
{
	sw_error_t rv = 0;
	a_uint32_t phy_id = 0;
	hsl_phy_ops_t *phy_drv;
	a_uint32_t phy_data;

	ADPT_DEV_ID_CHECK(dev_id);
	ADPT_NULL_POINT_CHECK(org_id);
	ADPT_NULL_POINT_CHECK(rev_id);

	if (A_TRUE != hsl_port_prop_check (dev_id, port_id, HSL_PP_PHY))
	{
		return SW_BAD_PARAM;
	}
	if (A_FALSE == _adpt_hppe_port_phy_connected (dev_id, port_id))
		return SW_NOT_SUPPORTED;

	SW_RTN_ON_NULL (phy_drv = hsl_phy_api_ops_get (dev_id, port_id));
	if (NULL == phy_drv->phy_id_get)
		return SW_NOT_SUPPORTED;

	rv = hsl_port_prop_get_phyid (dev_id, port_id, &phy_id);
	SW_RTN_ON_ERROR (rv);

	rv = phy_drv->phy_id_get (dev_id, phy_id, &phy_data);
	SW_RTN_ON_ERROR (rv);

	*org_id = (phy_data >> 16) & 0xffff;
	*rev_id = phy_data & 0xffff;

	return rv;

}
sw_error_t
adpt_hppe_port_mru_get(a_uint32_t dev_id, fal_port_t port_id,
		fal_mru_ctrl_t *ctrl)
{
	sw_error_t rv = SW_OK;
	union mru_mtu_ctrl_tbl_u mru_mtu_ctrl_tbl;

	memset(&mru_mtu_ctrl_tbl, 0, sizeof(mru_mtu_ctrl_tbl));
	ADPT_DEV_ID_CHECK(dev_id);
	ADPT_NULL_POINT_CHECK(ctrl);

	rv = hppe_mru_mtu_ctrl_tbl_get(dev_id, port_id, &mru_mtu_ctrl_tbl);

	if( rv != SW_OK )
		return rv;

	ctrl->mru_size = mru_mtu_ctrl_tbl.bf.mru;
	ctrl->action = (fal_fwd_cmd_t)mru_mtu_ctrl_tbl.bf.mru_cmd;

	return SW_OK;
}

sw_error_t
adpt_ppe_port_mru_get(a_uint32_t dev_id, fal_port_t port_id,
		fal_mru_ctrl_t *ctrl)
{
	a_uint32_t chip_ver = 0;

	ADPT_DEV_ID_CHECK(dev_id);
	ADPT_NULL_POINT_CHECK(ctrl);

	chip_ver = adpt_hppe_chip_revision_get(dev_id);
	if (chip_ver == CPPE_REVISION) {
#if defined(CPPE)
		return adpt_cppe_port_mru_get(dev_id, port_id, ctrl);
#endif
	} else {
		return adpt_hppe_port_mru_get(dev_id, port_id, ctrl);
	}

	return SW_NOT_SUPPORTED;
}
#endif

sw_error_t
adpt_hppe_port_power_on(a_uint32_t dev_id, fal_port_t port_id)
{

	sw_error_t rv = 0;
	a_uint32_t phy_id = 0;
	hsl_phy_ops_t *phy_drv;

	ADPT_DEV_ID_CHECK(dev_id);

	if (A_TRUE != hsl_port_prop_check (dev_id, port_id, HSL_PP_PHY))
	  {
		return SW_BAD_PARAM;
	  }
	if (A_FALSE == _adpt_hppe_port_phy_connected (dev_id, port_id))
		return SW_NOT_SUPPORTED;

	SW_RTN_ON_NULL (phy_drv = hsl_phy_api_ops_get (dev_id, port_id));
	if (NULL == phy_drv->phy_power_on)
		return SW_NOT_SUPPORTED;

	rv = hsl_port_prop_get_phyid (dev_id, port_id, &phy_id);
	SW_RTN_ON_ERROR (rv);

	rv = phy_drv->phy_power_on(dev_id, phy_id);

	return rv;

}
sw_error_t
adpt_hppe_port_speed_set(a_uint32_t dev_id, fal_port_t port_id,
			       fal_port_speed_t speed)
{
	sw_error_t rv = 0;
	a_uint32_t phy_id = 0;
	hsl_phy_ops_t *phy_drv;

	ADPT_DEV_ID_CHECK(dev_id);

	if (A_TRUE != hsl_port_prop_check (dev_id, port_id, HSL_PP_INCL_CPU))
	  {
		return SW_BAD_PARAM;
	  }
	if (A_FALSE == _adpt_hppe_port_phy_connected (dev_id, port_id))
		return SW_NOT_SUPPORTED;

	SW_RTN_ON_NULL (phy_drv = hsl_phy_api_ops_get (dev_id, port_id));
	if (NULL == phy_drv->phy_speed_set)
		return SW_NOT_SUPPORTED;

	rv = hsl_port_prop_get_phyid (dev_id, port_id, &phy_id);
	SW_RTN_ON_ERROR (rv);

	rv = phy_drv->phy_speed_set (dev_id, phy_id, speed);
	SW_RTN_ON_ERROR (rv);

	return rv;
}
sw_error_t
adpt_hppe_port_interface_mode_get(a_uint32_t dev_id, fal_port_t port_id,
			      fal_port_interface_mode_t * mode)
{
	sw_error_t rv = 0;
	if (A_TRUE != hsl_port_prop_check (dev_id, port_id, HSL_PP_EXCL_CPU))
	{
		return SW_BAD_PARAM;
	}
	*mode = port_interface_mode[dev_id][port_id];

	return rv;
}

sw_error_t
adpt_hppe_port_duplex_get(a_uint32_t dev_id, fal_port_t port_id,
				fal_port_duplex_t * pduplex)
{
	sw_error_t rv = 0;
	a_uint32_t phy_id = 0;
	hsl_phy_ops_t *phy_drv;
	struct port_phy_status phy_status = {0};

	ADPT_DEV_ID_CHECK(dev_id);
	ADPT_NULL_POINT_CHECK(pduplex);

	if (port_id == SSDK_PHYSICAL_PORT0 || port_id == SSDK_PHYSICAL_PORT7)
	{
		*pduplex = FAL_FULL_DUPLEX;
		return SW_OK;
	}

	if (A_TRUE != hsl_port_prop_check (dev_id, port_id, HSL_PP_INCL_CPU))
	{
		return SW_BAD_PARAM;
	}

	/* for those ports without PHY device should be sfp port */
	if (A_FALSE == _adpt_hppe_port_phy_connected (dev_id, port_id))
	{

		rv = _adpt_phy_status_get_from_ppe(dev_id,
			port_id, &phy_status);
		SW_RTN_ON_ERROR (rv);
		*pduplex = phy_status.duplex;

	}
	else
	{
		SW_RTN_ON_NULL (phy_drv = hsl_phy_api_ops_get (dev_id, port_id));
		if (NULL == phy_drv->phy_duplex_get)
			return SW_NOT_SUPPORTED;

		rv = hsl_port_prop_get_phyid (dev_id, port_id, &phy_id);
		SW_RTN_ON_ERROR (rv);
		rv = phy_drv->phy_duplex_get (dev_id, phy_id, pduplex);
		SW_RTN_ON_ERROR (rv);
	}

	return rv;
}

sw_error_t
adpt_hppe_port_autoneg_adv_get(a_uint32_t dev_id, fal_port_t port_id,
				     a_uint32_t * autoadv)
{

	sw_error_t rv = 0;
	a_uint32_t phy_id;
	hsl_phy_ops_t *phy_drv;

	ADPT_DEV_ID_CHECK(dev_id);
	ADPT_NULL_POINT_CHECK(autoadv);

	if (A_TRUE != hsl_port_prop_check (dev_id, port_id, HSL_PP_PHY))
	  {
		return SW_BAD_PARAM;
	  }
	if (A_FALSE == _adpt_hppe_port_phy_connected (dev_id, port_id))
		return SW_NOT_SUPPORTED;

	SW_RTN_ON_NULL (phy_drv = hsl_phy_api_ops_get (dev_id, port_id));
	if (NULL == phy_drv->phy_autoneg_adv_get)
		return SW_NOT_SUPPORTED;

	rv = hsl_port_prop_get_phyid (dev_id, port_id, &phy_id);
	SW_RTN_ON_ERROR (rv);

	*autoadv = 0;
	rv = phy_drv->phy_autoneg_adv_get (dev_id, phy_id, autoadv);
	SW_RTN_ON_ERROR (rv);

	return SW_OK;

}
#ifndef IN_PORTCONTROL_MINI
sw_error_t
adpt_hppe_port_mdix_status_get(a_uint32_t dev_id, fal_port_t port_id,
				     fal_port_mdix_status_t * mode)
{

	sw_error_t rv = 0;
	a_uint32_t phy_id = 0;
	hsl_phy_ops_t *phy_drv;

	ADPT_DEV_ID_CHECK(dev_id);
	ADPT_NULL_POINT_CHECK(mode);

	if (A_TRUE != hsl_port_prop_check (dev_id, port_id, HSL_PP_PHY))
	  {
		return SW_BAD_PARAM;
	  }
	if (A_FALSE == _adpt_hppe_port_phy_connected (dev_id, port_id))
		return SW_NOT_SUPPORTED;

	SW_RTN_ON_NULL (phy_drv = hsl_phy_api_ops_get (dev_id, port_id));
	if (NULL == phy_drv->phy_mdix_status_get)
		return SW_NOT_SUPPORTED;

	rv = hsl_port_prop_get_phyid (dev_id, port_id, &phy_id);
	SW_RTN_ON_ERROR (rv);

	rv = phy_drv->phy_mdix_status_get (dev_id, phy_id, mode);

	return rv;

}
#endif
sw_error_t
adpt_hppe_port_link_status_get(a_uint32_t dev_id, fal_port_t port_id,
				     a_bool_t * status)
{

	sw_error_t rv = 0;
	a_uint32_t phy_id;
	hsl_phy_ops_t *phy_drv;
	struct port_phy_status phy_status = {0};

	ADPT_DEV_ID_CHECK(dev_id);
	ADPT_NULL_POINT_CHECK(status);

	if (port_id == SSDK_PHYSICAL_PORT0 || port_id == SSDK_PHYSICAL_PORT7)
	{
		*status = A_TRUE;
		return SW_OK;
	}
	if (A_TRUE != hsl_port_prop_check (dev_id, port_id, HSL_PP_INCL_CPU))
	{
		return SW_BAD_PARAM;
	}

	/* for those ports without PHY device should be sfp port */
	if (A_FALSE == _adpt_hppe_port_phy_connected (dev_id, port_id))
	{
		rv = _adpt_phy_status_get_from_ppe(dev_id,
			port_id, &phy_status);
		SW_RTN_ON_ERROR (rv);
		*status = (a_bool_t) phy_status.link_status;
	}
	else
	{
		SW_RTN_ON_NULL (phy_drv = hsl_phy_api_ops_get (dev_id, port_id));
		if (NULL == phy_drv->phy_link_status_get)
			return SW_NOT_SUPPORTED;

		rv = hsl_port_prop_get_phyid (dev_id, port_id, &phy_id);
		SW_RTN_ON_ERROR (rv);

		if (A_TRUE == phy_drv->phy_link_status_get (dev_id, phy_id))
		{
			*status = A_TRUE;
		}
		else
		{
			*status = A_FALSE;
		}
	}

	return SW_OK;

}
#ifndef IN_PORTCONTROL_MINI
sw_error_t
adpt_hppe_port_8023az_set(a_uint32_t dev_id, fal_port_t port_id,
				a_bool_t enable)
{

	sw_error_t rv = 0;
	a_uint32_t phy_id = 0;
	hsl_phy_ops_t *phy_drv;

	ADPT_DEV_ID_CHECK(dev_id);

	if (A_TRUE != hsl_port_prop_check (dev_id, port_id, HSL_PP_PHY))
	  {
		return SW_BAD_PARAM;
	  }
	if (A_FALSE == _adpt_hppe_port_phy_connected (dev_id, port_id))
		return SW_NOT_SUPPORTED;

	SW_RTN_ON_NULL (phy_drv = hsl_phy_api_ops_get (dev_id, port_id));
	if (NULL == phy_drv->phy_8023az_set)
		return SW_NOT_SUPPORTED;

	rv = hsl_port_prop_get_phyid (dev_id, port_id, &phy_id);
	SW_RTN_ON_ERROR (rv);

	rv = phy_drv->phy_8023az_set (dev_id, phy_id, enable);

	return rv;

}
sw_error_t
adpt_hppe_port_powersave_get(a_uint32_t dev_id, fal_port_t port_id,
			  a_bool_t * enable)
{

	sw_error_t rv = 0;
	a_uint32_t phy_id = 0;
	hsl_phy_ops_t *phy_drv;

	ADPT_DEV_ID_CHECK(dev_id);
	ADPT_NULL_POINT_CHECK(enable);

	if (A_TRUE != hsl_port_prop_check (dev_id, port_id, HSL_PP_PHY))
	  {
		return SW_BAD_PARAM;
	  }
	if (A_FALSE == _adpt_hppe_port_phy_connected (dev_id, port_id))
		return SW_NOT_SUPPORTED;

	SW_RTN_ON_NULL (phy_drv = hsl_phy_api_ops_get (dev_id, port_id));
	if (NULL == phy_drv->phy_powersave_get)
		return SW_NOT_SUPPORTED;

	rv = hsl_port_prop_get_phyid (dev_id, port_id, &phy_id);
	SW_RTN_ON_ERROR (rv);

	rv = phy_drv->phy_powersave_get (dev_id, phy_id, enable);

	return rv;

}

sw_error_t
adpt_hppe_port_combo_prefer_medium_get(a_uint32_t dev_id,
						     a_uint32_t port_id,
						     fal_port_medium_t *
						     medium)
{
	sw_error_t rv = 0;
	a_uint32_t phy_id = 0;
	hsl_phy_ops_t *phy_drv;

	ADPT_DEV_ID_CHECK(dev_id);
	ADPT_NULL_POINT_CHECK(medium);

	if (A_TRUE != hsl_port_prop_check (dev_id, port_id, HSL_PP_PHY))
	  {
		return SW_BAD_PARAM;
	  }
	if (A_FALSE == _adpt_hppe_port_phy_connected (dev_id, port_id))
		return SW_NOT_SUPPORTED;

	SW_RTN_ON_NULL (phy_drv = hsl_phy_api_ops_get (dev_id, port_id));
	if (NULL == phy_drv->phy_combo_prefer_medium_get)
		return SW_NOT_SUPPORTED;

	rv = hsl_port_prop_get_phyid (dev_id, port_id, &phy_id);
	SW_RTN_ON_ERROR (rv);

	rv = phy_drv->phy_combo_prefer_medium_get (dev_id, phy_id, medium);

	return rv;

}
#endif
sw_error_t
adpt_hppe_port_max_frame_size_get(a_uint32_t dev_id, fal_port_t port_id,
		a_uint32_t *max_frame)
{
	sw_error_t rv = SW_OK;
	a_uint32_t port_mac_type = 0;

	ADPT_DEV_ID_CHECK(dev_id);
	ADPT_NULL_POINT_CHECK(max_frame);

	port_mac_type =qca_hppe_port_mac_type_get(dev_id, port_id);
	if (port_mac_type == PORT_XGMAC_TYPE)
	{
		rv = _adpt_xgmac_port_max_frame_size_get( dev_id, port_id, max_frame);
	}
	else if (port_mac_type == PORT_GMAC_TYPE)
	{
		rv = _adpt_gmac_port_max_frame_size_get( dev_id, port_id, max_frame);
	}
	else
	{
		return SW_BAD_VALUE;
	}

	return rv;
}

sw_error_t
adpt_ppe_port_max_frame_size_get(a_uint32_t dev_id, fal_port_t port_id,
		a_uint32_t *max_frame)
{
#ifdef CPPE
	if (adpt_hppe_chip_revision_get(dev_id) == CPPE_REVISION &&
		port_id == SSDK_PHYSICAL_PORT6)
	{
		return adpt_cppe_lpbk_max_frame_size_get(dev_id, port_id, max_frame);
	}
#endif
	return adpt_hppe_port_max_frame_size_get(dev_id, port_id, max_frame);

}

#ifndef IN_PORTCONTROL_MINI
sw_error_t
adpt_hppe_port_combo_prefer_medium_set(a_uint32_t dev_id,
					     a_uint32_t port_id,
					     fal_port_medium_t medium)
{

	sw_error_t rv = 0;
	a_uint32_t phy_id = 0;
	hsl_phy_ops_t *phy_drv;

	ADPT_DEV_ID_CHECK(dev_id);

	if (A_TRUE != hsl_port_prop_check (dev_id, port_id, HSL_PP_PHY))
	  {
		return SW_BAD_PARAM;
	  }
	if (A_FALSE == _adpt_hppe_port_phy_connected (dev_id, port_id))
		return SW_NOT_SUPPORTED;

	SW_RTN_ON_NULL (phy_drv = hsl_phy_api_ops_get (dev_id, port_id));
	if (NULL == phy_drv->phy_combo_prefer_medium_set)
		return SW_NOT_SUPPORTED;

	rv = hsl_port_prop_get_phyid (dev_id, port_id, &phy_id);
	SW_RTN_ON_ERROR (rv);

	rv = phy_drv->phy_combo_prefer_medium_set (dev_id, phy_id, medium);

	return rv;

}
#endif
sw_error_t
adpt_hppe_port_power_off(a_uint32_t dev_id, fal_port_t port_id)
{

	sw_error_t rv = 0;
	a_uint32_t phy_id = 0;
	hsl_phy_ops_t *phy_drv;

	ADPT_DEV_ID_CHECK(dev_id);

	if (A_TRUE != hsl_port_prop_check (dev_id, port_id, HSL_PP_PHY))
	  {
		return SW_BAD_PARAM;
	  }
	if (A_FALSE == _adpt_hppe_port_phy_connected (dev_id, port_id))
		return SW_NOT_SUPPORTED;

	SW_RTN_ON_NULL (phy_drv = hsl_phy_api_ops_get (dev_id, port_id));
	if (NULL == phy_drv->phy_power_off)
		return SW_NOT_SUPPORTED;

	rv = hsl_port_prop_get_phyid (dev_id, port_id, &phy_id);
	SW_RTN_ON_ERROR (rv);

	rv = phy_drv->phy_power_off(dev_id, phy_id);

	return rv;

}
sw_error_t
adpt_hppe_port_txfc_status_set(a_uint32_t dev_id, fal_port_t port_id,
				     a_bool_t enable)
{
	sw_error_t rv = SW_OK;
	a_uint32_t port_mac_type;
	struct qca_phy_priv *priv = ssdk_phy_priv_data_get(dev_id);
	adpt_api_t *p_adpt_api;

	ADPT_DEV_ID_CHECK(dev_id);

	if (!priv)
		return SW_FAIL;

	if ((port_id < SSDK_PHYSICAL_PORT1) || (port_id > SSDK_PHYSICAL_PORT6))
		return SW_BAD_VALUE;

	port_mac_type =qca_hppe_port_mac_type_get(dev_id, port_id);
	if (port_mac_type == PORT_XGMAC_TYPE)
		rv = _adpt_xgmac_port_txfc_status_set( dev_id, port_id, enable);
	else if (port_mac_type == PORT_GMAC_TYPE)
		rv = _adpt_gmac_port_txfc_status_set( dev_id, port_id, enable);
	else
		return SW_BAD_VALUE;

	if (rv != SW_OK)
		return rv;

	priv->port_old_tx_flowctrl[port_id - 1] = enable;

	/*keep bm status same with port*/
	p_adpt_api = adpt_api_ptr_get(dev_id);
	if (p_adpt_api && p_adpt_api->adpt_port_bm_ctrl_set)
		rv = p_adpt_api->adpt_port_bm_ctrl_set(dev_id,
					PHY_PORT_TO_BM_PORT(port_id),
					enable);

	return rv;
}

sw_error_t
adpt_hppe_port_rxfc_status_set(a_uint32_t dev_id, fal_port_t port_id,
				     a_bool_t enable)
{
	sw_error_t rv = SW_OK;
	a_uint32_t port_mac_type;
	struct qca_phy_priv *priv = ssdk_phy_priv_data_get(dev_id);

	ADPT_DEV_ID_CHECK(dev_id);

	if (!priv)
		return SW_FAIL;

	if ((port_id < SSDK_PHYSICAL_PORT1) || (port_id > SSDK_PHYSICAL_PORT6))
		return SW_BAD_VALUE;

	port_mac_type =qca_hppe_port_mac_type_get(dev_id, port_id);
	if(port_mac_type == PORT_XGMAC_TYPE)
		rv = _adpt_xgmac_port_rxfc_status_set( dev_id, port_id, enable);
	else if (port_mac_type == PORT_GMAC_TYPE)
		rv = _adpt_gmac_port_rxfc_status_set( dev_id, port_id, enable);
	else
		return SW_BAD_VALUE;

	if (rv != SW_OK)
		return rv;

	priv->port_old_rx_flowctrl[port_id - 1] = enable;

	return SW_OK;
}

#ifndef IN_PORTCONTROL_MINI
sw_error_t
adpt_hppe_port_counter_set(a_uint32_t dev_id, fal_port_t port_id,
		   a_bool_t enable)
{

	sw_error_t rv = 0;
	a_uint32_t phy_id = 0;
	hsl_phy_ops_t *phy_drv;

	ADPT_DEV_ID_CHECK(dev_id);

	if (A_TRUE != hsl_port_prop_check (dev_id, port_id, HSL_PP_PHY))
	  {
		return SW_BAD_PARAM;
	  }
	if (A_FALSE == _adpt_hppe_port_phy_connected (dev_id, port_id))
		return SW_NOT_SUPPORTED;

	SW_RTN_ON_NULL (phy_drv = hsl_phy_api_ops_get (dev_id, port_id));
	if (NULL == phy_drv->phy_counter_set)
		return SW_NOT_SUPPORTED;

	rv = hsl_port_prop_get_phyid (dev_id, port_id, &phy_id);
	SW_RTN_ON_ERROR (rv);

	rv = phy_drv->phy_counter_set (dev_id, phy_id, enable);

	return rv;
}
sw_error_t
adpt_hppe_port_combo_fiber_mode_get(a_uint32_t dev_id,
						  a_uint32_t port_id,
						  fal_port_fiber_mode_t * mode)
{
	sw_error_t rv = 0;
	a_uint32_t phy_id = 0;
	hsl_phy_ops_t *phy_drv;

	ADPT_DEV_ID_CHECK(dev_id);
	ADPT_NULL_POINT_CHECK(mode);

	if (A_TRUE != hsl_port_prop_check (dev_id, port_id, HSL_PP_PHY))
	  {
	  	return SW_BAD_PARAM;
	  }
	if (A_FALSE == _adpt_hppe_port_phy_connected (dev_id, port_id))
		return SW_NOT_SUPPORTED;

	SW_RTN_ON_NULL (phy_drv = hsl_phy_api_ops_get (dev_id, port_id));
	if (NULL == phy_drv->phy_combo_fiber_mode_get)
		return SW_NOT_SUPPORTED;

	rv = hsl_port_prop_get_phyid (dev_id, port_id, &phy_id);
	SW_RTN_ON_ERROR (rv);

	rv = phy_drv->phy_combo_fiber_mode_get (dev_id, phy_id, mode);

	return rv;

}

sw_error_t
adpt_hppe_port_local_loopback_set(a_uint32_t dev_id,
						fal_port_t port_id,
						a_bool_t enable)
{

	sw_error_t rv = 0;
	a_uint32_t phy_id = 0;
	hsl_phy_ops_t *phy_drv;

	ADPT_DEV_ID_CHECK(dev_id);

	if (A_TRUE != hsl_port_prop_check (dev_id, port_id, HSL_PP_PHY))
	  {
		return SW_BAD_PARAM;
	  }
	if (A_FALSE == _adpt_hppe_port_phy_connected (dev_id, port_id))
		return SW_NOT_SUPPORTED;

	SW_RTN_ON_NULL (phy_drv = hsl_phy_api_ops_get (dev_id, port_id));
	if (NULL == phy_drv->phy_local_loopback_set)
		return SW_NOT_SUPPORTED;

	rv = hsl_port_prop_get_phyid (dev_id, port_id, &phy_id);
	SW_RTN_ON_ERROR (rv);

	rv = phy_drv->phy_local_loopback_set (dev_id, phy_id, enable);

	return rv;

}
sw_error_t
adpt_hppe_port_wol_status_set(a_uint32_t dev_id, fal_port_t port_id,
			      a_bool_t enable)
{

	sw_error_t rv = 0;
	a_uint32_t phy_id = 0;
	hsl_phy_ops_t *phy_drv;

	ADPT_DEV_ID_CHECK(dev_id);

	if (A_TRUE != hsl_port_prop_check (dev_id, port_id, HSL_PP_PHY))
	{
		return SW_BAD_PARAM;
	}
	if (A_FALSE == _adpt_hppe_port_phy_connected (dev_id, port_id))
		return SW_NOT_SUPPORTED;

	SW_RTN_ON_NULL (phy_drv = hsl_phy_api_ops_get (dev_id, port_id));
	if (NULL == phy_drv->phy_wol_status_set)
		return SW_NOT_SUPPORTED;

	rv = hsl_port_prop_get_phyid (dev_id, port_id, &phy_id);
	SW_RTN_ON_ERROR (rv);

	rv = phy_drv->phy_wol_status_set (dev_id, phy_id, enable);

	return rv;


}
sw_error_t
adpt_hppe_port_magic_frame_mac_get(a_uint32_t dev_id, fal_port_t port_id,
				   fal_mac_addr_t * mac)
{

	sw_error_t rv;
	a_uint32_t phy_id = 0;
	hsl_phy_ops_t *phy_drv;

	ADPT_DEV_ID_CHECK(dev_id);
	ADPT_NULL_POINT_CHECK(mac);

	if (A_TRUE != hsl_port_prop_check (dev_id, port_id, HSL_PP_PHY))
	  {
		return SW_BAD_PARAM;
	  }
	if (A_FALSE == _adpt_hppe_port_phy_connected (dev_id, port_id))
		return SW_NOT_SUPPORTED;

	SW_RTN_ON_NULL (phy_drv = hsl_phy_api_ops_get (dev_id, port_id));
	if (NULL == phy_drv->phy_magic_frame_mac_get)
		return SW_NOT_SUPPORTED;

	rv = hsl_port_prop_get_phyid (dev_id, port_id, &phy_id);
	SW_RTN_ON_ERROR (rv);

	rv = phy_drv->phy_magic_frame_mac_get (dev_id, phy_id,mac);

	return rv;

}

sw_error_t
adpt_hppe_port_flowctrl_get(a_uint32_t dev_id, fal_port_t port_id,
				  a_bool_t * enable)
{
	sw_error_t rv = SW_OK;
	a_bool_t txfc_enable, rxfc_enable;

#if defined(CPPE)
	if ((adpt_hppe_chip_revision_get(dev_id) == CPPE_REVISION) &&
		(port_id == SSDK_PHYSICAL_PORT6)) {
		return adpt_cppe_switch_port_loopback_flowctrl_get(dev_id,
				port_id, enable);
	}
#endif
	rv = adpt_hppe_port_txfc_status_get(dev_id, port_id,  &txfc_enable);
	rv |= adpt_hppe_port_rxfc_status_get(dev_id, port_id,  &rxfc_enable);
	if(rv != SW_OK)
		return rv;
	*enable = txfc_enable & rxfc_enable;

	return SW_OK;
}
#endif
sw_error_t
adpt_hppe_port_rxmac_status_set(a_uint32_t dev_id, fal_port_t port_id,
				      a_bool_t enable)
{
	a_uint32_t port_mac_type;

	ADPT_DEV_ID_CHECK(dev_id);

	port_mac_type =qca_hppe_port_mac_type_get(dev_id, port_id);
	if (port_mac_type == PORT_XGMAC_TYPE)
		_adpt_xgmac_port_rx_status_set(dev_id, port_id, enable);
	else if (port_mac_type == PORT_GMAC_TYPE)
		_adpt_gmac_port_rx_status_set(dev_id, port_id, enable);
	else
		return SW_BAD_VALUE;

	return SW_OK;
}
#ifndef IN_PORTCONTROL_MINI
sw_error_t
adpt_hppe_port_counter_get(a_uint32_t dev_id, fal_port_t port_id,
		   a_bool_t * enable)
{

	sw_error_t rv;
	a_uint32_t phy_id = 0;
	hsl_phy_ops_t *phy_drv;

	ADPT_DEV_ID_CHECK(dev_id);
	ADPT_NULL_POINT_CHECK(enable);

	 if (A_TRUE != hsl_port_prop_check (dev_id, port_id, HSL_PP_PHY))
	   {
		return SW_BAD_PARAM;
	   }
	 if (A_FALSE == _adpt_hppe_port_phy_connected (dev_id, port_id))
		 return SW_NOT_SUPPORTED;

	 SW_RTN_ON_NULL (phy_drv = hsl_phy_api_ops_get (dev_id, port_id));
	 if (NULL == phy_drv->phy_counter_get)
		return SW_NOT_SUPPORTED;

	 rv = hsl_port_prop_get_phyid (dev_id, port_id, &phy_id);
	 SW_RTN_ON_ERROR (rv);

	 rv = phy_drv->phy_counter_get (dev_id, phy_id, enable);

	 return rv;

}
#endif

static sw_error_t
_adpt_hppe_port_interface_mode_set(a_uint32_t dev_id, fal_port_t port_id,
			      fal_port_interface_mode_t mode)
{
	sw_error_t rv = SW_OK;

	ADPT_DEV_ID_CHECK(dev_id);
	if (A_TRUE != hsl_port_prop_check (dev_id, port_id, HSL_PP_EXCL_CPU) &&
		mode != PORT_INTERFACE_MODE_MAX)
	{
		return SW_BAD_PARAM;
	}

	port_interface_mode[dev_id][port_id] = mode;

	return rv;
}

sw_error_t
adpt_hppe_port_interface_mode_set(a_uint32_t dev_id, fal_port_t port_id,
			      fal_port_interface_mode_t mode)
{
	sw_error_t rv = SW_OK;
	struct qca_phy_priv *priv;

	priv = ssdk_phy_priv_data_get(dev_id);
	SW_RTN_ON_NULL(priv);
	qca_mac_sw_sync_work_stop(priv);

	rv = _adpt_hppe_port_interface_mode_set(dev_id, port_id, mode);

	return rv;
}

static sw_error_t
_adpt_hppe_gmac_speed_set(a_uint32_t dev_id, a_uint32_t port_id, fal_port_speed_t speed)
{
	sw_error_t rv = SW_OK;
	union mac_speed_u mac_speed;

	memset(&mac_speed, 0, sizeof(mac_speed));
	ADPT_DEV_ID_CHECK(dev_id);

	port_id = HPPE_TO_GMAC_PORT_ID(port_id);
	hppe_mac_speed_get(dev_id, port_id, &mac_speed);

	if(FAL_SPEED_10 == speed)
		mac_speed.bf.mac_speed = MAC_SPEED_10M;
	else if(FAL_SPEED_100 == speed)
		mac_speed.bf.mac_speed = MAC_SPEED_100M;
	else if(FAL_SPEED_1000 == speed || FAL_SPEED_2500 == speed)
		mac_speed.bf.mac_speed = MAC_SPEED_1000M;

	rv = hppe_mac_speed_set(dev_id, port_id, &mac_speed);

	return rv;
}

static sw_error_t
_adpt_hppe_xgmac_speed_set(a_uint32_t dev_id, a_uint32_t port_id, fal_port_speed_t speed)
{
	sw_error_t rv = SW_OK;
	union mac_tx_configuration_u mac_tx_configuration;
	a_uint32_t mode = 0;

	memset(&mac_tx_configuration, 0, sizeof(mac_tx_configuration));
	ADPT_DEV_ID_CHECK(dev_id);

	port_id = HPPE_TO_XGMAC_PORT_ID(port_id);
	hppe_mac_tx_configuration_get(dev_id, port_id, &mac_tx_configuration);

	if(FAL_SPEED_1000 == speed)
	{
		mac_tx_configuration.bf.uss= XGMAC_USXGMII_CLEAR;
		mac_tx_configuration.bf.ss= XGMAC_SPEED_SELECT_1000M;
	}
	else if(FAL_SPEED_10000 == speed)
	{
		if (port_id == SSDK_PHYSICAL_PORT0)
		{
			mode = ssdk_dt_global_get_mac_mode(dev_id, SSDK_UNIPHY_INSTANCE1);
		}
		else
		{
			mode = ssdk_dt_global_get_mac_mode(dev_id, SSDK_UNIPHY_INSTANCE2);
		}
		if (mode == PORT_WRAPPER_USXGMII)
		{
			mac_tx_configuration.bf.uss= XGMAC_USXGMII_ENABLE;
			mac_tx_configuration.bf.ss= XGMAC_SPEED_SELECT_10000M;
		}
		else
		{
			mac_tx_configuration.bf.uss= XGMAC_USXGMII_CLEAR;
			mac_tx_configuration.bf.ss= XGMAC_SPEED_SELECT_10000M;
		}
	}
	else if(FAL_SPEED_5000 == speed)
	{
		mac_tx_configuration.bf.uss= XGMAC_USXGMII_ENABLE;
		mac_tx_configuration.bf.ss= XGMAC_SPEED_SELECT_5000M;
	}
	else if(FAL_SPEED_2500 == speed)
	{
		if (port_id == SSDK_PHYSICAL_PORT0)
		{
			mode = ssdk_dt_global_get_mac_mode(dev_id, SSDK_UNIPHY_INSTANCE1);
		}
		else
		{
			mode = ssdk_dt_global_get_mac_mode(dev_id, SSDK_UNIPHY_INSTANCE2);
		}
		if (mode == PORT_WRAPPER_USXGMII)
		{
			mac_tx_configuration.bf.uss= XGMAC_USXGMII_ENABLE;
			mac_tx_configuration.bf.ss= XGMAC_SPEED_SELECT_2500M;
		}
		else
		{
			mac_tx_configuration.bf.uss= XGMAC_USXGMII_CLEAR;
			mac_tx_configuration.bf.ss= XGMAC_SPEED_SELECT_2500M;
		}
	}
	else if(FAL_SPEED_100 == speed)
	{
		mac_tx_configuration.bf.uss= XGMAC_USXGMII_CLEAR;
		mac_tx_configuration.bf.ss= XGMAC_SPEED_SELECT_1000M;
	}
	else if(FAL_SPEED_10 == speed)
	{
		mac_tx_configuration.bf.uss= XGMAC_USXGMII_CLEAR;
		mac_tx_configuration.bf.ss= XGMAC_SPEED_SELECT_1000M;
	}

	rv = hppe_mac_tx_configuration_set(dev_id, port_id, &mac_tx_configuration);

	return rv;
}

static sw_error_t
_adpt_hppe_gmac_duplex_set(a_uint32_t dev_id, a_uint32_t port_id, fal_port_duplex_t duplex)
{
	sw_error_t rv = SW_OK;
	union mac_enable_u mac_enable;

	memset(&mac_enable, 0, sizeof(mac_enable));
	ADPT_DEV_ID_CHECK(dev_id);

	port_id = HPPE_TO_GMAC_PORT_ID(port_id);
	hppe_mac_enable_get(dev_id, port_id, &mac_enable);

	if (FAL_FULL_DUPLEX == duplex)
		mac_enable.bf.duplex = 1;
	else
		mac_enable.bf.duplex = 0;

	rv = hppe_mac_enable_set(dev_id, port_id, &mac_enable);

	return rv;
}

sw_error_t
adpt_hppe_port_mac_duplex_set(a_uint32_t dev_id, a_uint32_t port_id, fal_port_duplex_t duplex)
{
	sw_error_t rv = SW_OK;
	a_uint32_t port_mac_type;

	port_mac_type = qca_hppe_port_mac_type_get(dev_id, port_id);
	if (port_mac_type == PORT_XGMAC_TYPE)
	{
		return rv;
	}
	else if (port_mac_type == PORT_GMAC_TYPE)
	{
		rv = _adpt_hppe_gmac_duplex_set(dev_id, port_id, duplex);
	}
	else
	{
		return SW_BAD_VALUE;
	}

	return rv;
}

static sw_error_t
_adpt_hppe_port_mux_mac_set(a_uint32_t dev_id, fal_port_t port_id, a_uint32_t port_type)
{
	sw_error_t rv = SW_OK;
	union port_mux_ctrl_u port_mux_ctrl;
	a_uint32_t mode0, mode1;

	memset(&port_mux_ctrl, 0, sizeof(port_mux_ctrl));
	ADPT_DEV_ID_CHECK(dev_id);

	rv = hppe_port_mux_ctrl_get(dev_id, &port_mux_ctrl);
	port_mux_ctrl.bf.port4_pcs_sel = PORT4_PCS_SEL_GMII_FROM_PCS0;

	if (port_id == HPPE_MUX_PORT1)
	{
		if (port_type == PORT_GMAC_TYPE)
		{
			mode0 = ssdk_dt_global_get_mac_mode(dev_id, SSDK_UNIPHY_INSTANCE0);
			mode1 = ssdk_dt_global_get_mac_mode(dev_id, SSDK_UNIPHY_INSTANCE1);
			if ((mode0 == PORT_WRAPPER_PSGMII) ||
				(mode0 == PORT_WRAPPER_PSGMII_FIBER) ||
				(mode0 == PORT_WRAPPER_SGMII4_RGMII4) ||
				(mode0 == PORT_WRAPPER_SGMII_CHANNEL4))
			{
				port_mux_ctrl.bf.port5_pcs_sel = PORT5_PCS_SEL_GMII_FROM_PCS0;
				port_mux_ctrl.bf.port5_gmac_sel = PORT5_GMAC_SEL_GMAC;
			}
			if (mode1 == PORT_WRAPPER_SGMII0_RGMII4 ||
					mode1 == PORT_WRAPPER_SGMII_CHANNEL0 ||
					mode1 == PORT_WRAPPER_SGMII_PLUS ||
					mode1 == PORT_WRAPPER_SGMII_FIBER)
			{
				port_mux_ctrl.bf.port5_pcs_sel = PORT5_PCS_SEL_GMII_FROM_PCS1;
				port_mux_ctrl.bf.port5_gmac_sel = PORT5_GMAC_SEL_GMAC;
			}
		}
		else if (port_type == PORT_XGMAC_TYPE)
		{
			port_mux_ctrl.bf.port5_pcs_sel = PORT5_PCS_SEL_GMII_FROM_PCS1;
			port_mux_ctrl.bf.port5_gmac_sel = PORT5_GMAC_SEL_XGMAC;
		}
		else
			return SW_NOT_SUPPORTED;
	}
	else if (port_id == HPPE_MUX_PORT2)
	{
		if (port_type == PORT_GMAC_TYPE)
		{
			port_mux_ctrl.bf.port6_pcs_sel = PORT6_PCS_SEL_GMII_FROM_PCS2;
			port_mux_ctrl.bf.port6_gmac_sel = PORT6_GMAC_SEL_GMAC;
		}
		else if (port_type == PORT_XGMAC_TYPE)
		{
			port_mux_ctrl.bf.port6_pcs_sel = PORT6_PCS_SEL_GMII_FROM_PCS2;
			port_mux_ctrl.bf.port6_gmac_sel = PORT6_GMAC_SEL_XGMAC;
		}
		else
			return SW_NOT_SUPPORTED;
	}
	else
		return SW_OK;

	rv = hppe_port_mux_ctrl_set(dev_id, &port_mux_ctrl);

	return rv;
}

static sw_error_t
adpt_hppe_port_xgmac_promiscuous_mode_set(a_uint32_t dev_id,
	a_uint32_t port_id)
{
	sw_error_t rv = 0;

	port_id = HPPE_TO_XGMAC_PORT_ID(port_id);

	rv = hppe_mac_packet_filter_pr_set(dev_id, port_id, PROMISCUOUS_MODE);

	SW_RTN_ON_ERROR (rv);

	rv = hppe_mac_packet_filter_pcf_set(dev_id, port_id, PASS_CONTROL_PACKET);

	return rv;
}
static sw_error_t
adpt_hppe_port_speed_change_mac_reset(a_uint32_t dev_id, a_uint32_t port_id)
{
	a_uint32_t uniphy_index = 0, mode = 0;
	a_uint32_t rxfc_status = 0, txfc_status = 0;
	sw_error_t rv = 0;

	if (port_id == HPPE_MUX_PORT1) {
		uniphy_index = SSDK_UNIPHY_INSTANCE1;
	} else if (port_id == HPPE_MUX_PORT2) {
		uniphy_index = SSDK_UNIPHY_INSTANCE2;
	} else {
		return SW_OK;
	}
	mode = ssdk_dt_global_get_mac_mode(dev_id, uniphy_index);
	if (mode == PORT_WRAPPER_USXGMII) {
		ssdk_port_mac_clock_reset(dev_id, port_id);
		/*restore xgmac's pr and pcf setting after reset operation*/
		rv = adpt_hppe_port_xgmac_promiscuous_mode_set(dev_id,
			port_id);
		SW_RTN_ON_ERROR(rv);
		/*flowctrl need to be configured when reset XGMAC*/
		rv = _adpt_xgmac_port_rxfc_status_get(dev_id, port_id,
			&rxfc_status);
		SW_RTN_ON_ERROR(rv);
		rv = _adpt_xgmac_port_rxfc_status_set(dev_id, port_id,
			rxfc_status);
		SW_RTN_ON_ERROR(rv);

		rv = _adpt_xgmac_port_txfc_status_get(dev_id, port_id,
			&txfc_status);
		SW_RTN_ON_ERROR(rv);
		rv = _adpt_xgmac_port_txfc_status_set(dev_id, port_id,
			txfc_status);
	}
	return rv;
}
static sw_error_t
adpt_hppe_port_interface_mode_switch_mac_reset(a_uint32_t dev_id,
	a_uint32_t port_id)
{
	a_uint32_t uniphy_index = 0, mode = 0;
	sw_error_t rv = 0;
	phy_type_t phy_type;
	a_uint32_t port_mac_type;

	phy_type = hsl_phy_type_get(dev_id, port_id);
	if (phy_type != AQUANTIA_PHY_CHIP && phy_type != SFP_PHY_CHIP) {
		return SW_OK;
	}

	if (port_id == HPPE_MUX_PORT1) {
		uniphy_index = SSDK_UNIPHY_INSTANCE1;
	} else if (port_id == HPPE_MUX_PORT2) {
		uniphy_index = SSDK_UNIPHY_INSTANCE2;
	} else {
		return SW_OK;
	}

	mode = ssdk_dt_global_get_mac_mode(dev_id, uniphy_index);
	if ((mode == PORT_WRAPPER_USXGMII) ||
		(mode == PORT_WRAPPER_SGMII_CHANNEL0) ||
		(mode == PORT_WRAPPER_SGMII0_RGMII4) ||
		(mode == PORT_WRAPPER_SGMII_FIBER) ||
		(mode == PORT_WRAPPER_10GBASE_R)) {
		ssdk_port_mac_clock_reset(dev_id, port_id);
		port_mac_type = qca_hppe_port_mac_type_get(dev_id, port_id);
		if (port_mac_type == PORT_XGMAC_TYPE) {
			/*restore xgmac's pr and pcf setting after reset operation*/
			rv = adpt_hppe_port_xgmac_promiscuous_mode_set(dev_id,
			port_id);
		}
	}
	return rv;
}

sw_error_t
adpt_hppe_port_mac_speed_set(a_uint32_t dev_id, a_uint32_t port_id,
				fal_port_speed_t speed)
{
	sw_error_t rv = SW_OK;
	a_uint32_t port_mac_type;

	port_mac_type = qca_hppe_port_mac_type_get(dev_id, port_id);
	if (port_mac_type == PORT_XGMAC_TYPE)
	{
		rv = _adpt_hppe_xgmac_speed_set(dev_id, port_id, speed);

	}
	else if (port_mac_type == PORT_GMAC_TYPE)
	{
		rv = _adpt_hppe_gmac_speed_set(dev_id, port_id, speed);
	}
	else
	{
		return SW_BAD_VALUE;
	}
	return rv;
}
static sw_error_t
_adpt_hppe_port_mux_set(a_uint32_t dev_id, fal_port_t port_id,
	a_uint32_t mode1, a_uint32_t mode2)
{
	a_uint32_t  mode = 0, port_type;
	sw_error_t rv = SW_OK;

	port_type = qca_hppe_port_mac_type_get(dev_id, port_id);
	if (port_type == PORT_GMAC_TYPE)
	{
		rv = adpt_hppe_port_mac_speed_set(dev_id, port_id, FAL_SPEED_1000);
		rv = adpt_hppe_port_mac_duplex_set(dev_id, port_id, FAL_FULL_DUPLEX);
	}
	else if (port_type == PORT_XGMAC_TYPE)
	{
		if (port_id == HPPE_MUX_PORT1)
			mode = mode1;
		else if (port_id == HPPE_MUX_PORT2)
			mode = mode2;
		else
			return SW_NOT_SUPPORTED;
		if (mode == PORT_WRAPPER_SGMII_PLUS)
		{
			rv = adpt_hppe_port_mac_speed_set(dev_id, port_id, FAL_SPEED_2500);
			rv = adpt_hppe_port_mac_duplex_set(dev_id, port_id, FAL_FULL_DUPLEX);
		}
		else
		{
			rv = adpt_hppe_port_mac_speed_set(dev_id, port_id, FAL_SPEED_10000);
			rv = adpt_hppe_port_mac_duplex_set(dev_id, port_id, FAL_FULL_DUPLEX);
		}
	}
	if ((port_type == PORT_GMAC_TYPE) ||(port_type == PORT_XGMAC_TYPE)) {
		if (adpt_hppe_chip_revision_get(dev_id) == HPPE_REVISION) {
		 	rv = _adpt_hppe_port_mux_mac_set(dev_id, port_id, port_type);
		} else if (adpt_hppe_chip_revision_get(dev_id) == CPPE_REVISION) {
#if defined(CPPE)
			rv = _adpt_cppe_port_mux_mac_set(dev_id, port_id, port_type);
#endif
		}
	}

	if (port_id >= HPPE_MUX_PORT1) {
		if (port_type == PORT_GMAC_TYPE) {
			rv = _adpt_xgmac_port_txfc_status_set( dev_id, port_id, A_FALSE);
			SW_RTN_ON_ERROR(rv);
			rv = _adpt_xgmac_port_rxfc_status_set( dev_id, port_id, A_FALSE);
			SW_RTN_ON_ERROR(rv);
		} else if (port_type == PORT_XGMAC_TYPE) {
			rv = _adpt_gmac_port_txfc_status_set( dev_id, port_id, A_FALSE);
			SW_RTN_ON_ERROR(rv);
			rv = _adpt_gmac_port_rxfc_status_set( dev_id, port_id, A_FALSE);
			SW_RTN_ON_ERROR(rv);
		} else {
			return SW_NOT_SUPPORTED;
		}
		rv = adpt_hppe_port_interface_mode_switch_mac_reset(dev_id, port_id);
	}

	return rv;
}

sw_error_t
adpt_hppe_port_mux_mac_type_set(a_uint32_t dev_id, fal_port_t port_id,
	a_uint32_t mode0, a_uint32_t mode1, a_uint32_t mode2)
{
	sw_error_t rv = SW_OK;
	a_uint32_t mode_tmp;

	/*init the port interface mode before set it according to three mac modes*/
	rv = _adpt_hppe_port_interface_mode_set(dev_id, port_id, PORT_INTERFACE_MODE_MAX);
	SW_RTN_ON_ERROR(rv);

	switch (mode0) {
		case PORT_WRAPPER_PSGMII_FIBER:
			if(port_id >= SSDK_PHYSICAL_PORT1  && port_id <= SSDK_PHYSICAL_PORT5)
			{
				qca_hppe_port_mac_type_set(dev_id, port_id, PORT_GMAC_TYPE);
				if (port_id == SSDK_PHYSICAL_PORT5) {
					_adpt_hppe_port_interface_mode_set(dev_id,
							SSDK_PHYSICAL_PORT5, PHY_PSGMII_FIBER);
				} else {
					_adpt_hppe_port_interface_mode_set(dev_id,
							port_id, PHY_PSGMII_BASET);
				}
			}
			break;
		case PORT_WRAPPER_PSGMII:
			if((port_id >= SSDK_PHYSICAL_PORT1 && port_id <= SSDK_PHYSICAL_PORT4) ||
					(port_id == SSDK_PHYSICAL_PORT5 &&
					 mode1 == PORT_WRAPPER_MAX))
			{
				qca_hppe_port_mac_type_set(dev_id, port_id, PORT_GMAC_TYPE);
				_adpt_hppe_port_interface_mode_set(dev_id, port_id, PHY_PSGMII_BASET);
			}
			break;
		case PORT_WRAPPER_QSGMII:
			if(port_id >= SSDK_PHYSICAL_PORT1 && port_id <= SSDK_PHYSICAL_PORT4)
			{
				qca_hppe_port_mac_type_set(dev_id, port_id, PORT_GMAC_TYPE);
				_adpt_hppe_port_interface_mode_set(dev_id, port_id, PORT_QSGMII);
			}
			break;
		case PORT_WRAPPER_SGMII0_RGMII4:
		case PORT_WRAPPER_SGMII_CHANNEL0:
		case PORT_WRAPPER_SGMII_FIBER:
#ifdef CPPE
		case PORT_WRAPPER_SGMII_PLUS:
			if(adpt_hppe_chip_revision_get(dev_id) == CPPE_REVISION
				&& port_id == SSDK_PHYSICAL_PORT4)
			{
				qca_hppe_port_mac_type_set(dev_id, port_id,
					PORT_GMAC_TYPE);
				if((mode0 == PORT_WRAPPER_SGMII0_RGMII4 ||
					mode0 == PORT_WRAPPER_SGMII_CHANNEL0))
				{
					if(hsl_port_prop_check (dev_id, port_id,
						HSL_PP_EXCL_CPU))
					{
						_adpt_hppe_port_interface_mode_set(dev_id, port_id,
							PHY_SGMII_BASET);
					}
					else
					{
						SSDK_ERROR("Port bitmap is incorrect when port 4"
							"support sgmii for CPPE\n");
						return SW_NOT_SUPPORTED;
					}
				}
				else if(mode0 == PORT_WRAPPER_SGMII_PLUS)
				{
					_adpt_hppe_port_interface_mode_set(dev_id, port_id,
						PORT_SGMII_PLUS);
				}
				else
				{
					SSDK_ERROR("CPPE doesn't support mode0 : %x\n",
						mode0);
					return SW_NOT_SUPPORTED;
				}
				break;
			}
#endif
			if(port_id == SSDK_PHYSICAL_PORT1)
			{
				qca_hppe_port_mac_type_set(dev_id, SSDK_PHYSICAL_PORT1,
						PORT_GMAC_TYPE);
				if(mode0 == PORT_WRAPPER_SGMII_FIBER)
				{
					_adpt_hppe_port_interface_mode_set(dev_id,
							SSDK_PHYSICAL_PORT1, PORT_SGMII_FIBER);
				}
				else
				{
					_adpt_hppe_port_interface_mode_set(dev_id,
							SSDK_PHYSICAL_PORT1, PHY_SGMII_BASET);
				}
			}
			break;
		case PORT_WRAPPER_SGMII1_RGMII4:
		case PORT_WRAPPER_SGMII_CHANNEL1:
			if(port_id == SSDK_PHYSICAL_PORT2)
			{
				qca_hppe_port_mac_type_set(dev_id, SSDK_PHYSICAL_PORT2,
						PORT_GMAC_TYPE);
				_adpt_hppe_port_interface_mode_set(dev_id,
						SSDK_PHYSICAL_PORT2, PHY_SGMII_BASET);
			}
			break;
		case PORT_WRAPPER_SGMII4_RGMII4:
		case PORT_WRAPPER_SGMII_CHANNEL4:
			if(port_id == SSDK_PHYSICAL_PORT5)
			{
				qca_hppe_port_mac_type_set(dev_id, SSDK_PHYSICAL_PORT5,
						PORT_GMAC_TYPE);
				_adpt_hppe_port_interface_mode_set(dev_id,
						SSDK_PHYSICAL_PORT5, PHY_SGMII_BASET);
			}
			break;
		default:
			break;
	}
	if(port_id == SSDK_PHYSICAL_PORT5 ||port_id == SSDK_PHYSICAL_PORT6)
	{
		if(port_id == SSDK_PHYSICAL_PORT5)
			mode_tmp = mode1;
		else
			mode_tmp = mode2;

		switch(mode_tmp)
		{
			case PORT_WRAPPER_SGMII_CHANNEL0:
			case PORT_WRAPPER_SGMII0_RGMII4:
			case PORT_WRAPPER_SGMII_FIBER:
				qca_hppe_port_mac_type_set(dev_id, port_id, PORT_GMAC_TYPE);
				if(mode_tmp == PORT_WRAPPER_SGMII_FIBER)
				{
					_adpt_hppe_port_interface_mode_set(dev_id, port_id,
						PORT_SGMII_FIBER);
				}
				else
				{
					_adpt_hppe_port_interface_mode_set(dev_id, port_id,
						PHY_SGMII_BASET);
				}
				break;
			case PORT_WRAPPER_SGMII_PLUS:
				if (ssdk_port_feature_get(dev_id, port_id, PHY_F_QGMAC)) {
					qca_hppe_port_mac_type_set(dev_id, port_id,
							PORT_GMAC_TYPE);
				} else {
					qca_hppe_port_mac_type_set(dev_id, port_id,
							PORT_XGMAC_TYPE);
				}
				_adpt_hppe_port_interface_mode_set(dev_id, port_id, PORT_SGMII_PLUS);
				break;
			case PORT_WRAPPER_USXGMII:
				qca_hppe_port_mac_type_set(dev_id, port_id, PORT_XGMAC_TYPE);
				_adpt_hppe_port_interface_mode_set(dev_id, port_id, PORT_USXGMII);
				break;
			case PORT_WRAPPER_10GBASE_R:
				qca_hppe_port_mac_type_set(dev_id, port_id, PORT_XGMAC_TYPE);
				_adpt_hppe_port_interface_mode_set(dev_id, port_id, PORT_10GBASE_R);
				break;
			default:
				break;
		}
	}

	rv = _adpt_hppe_port_mux_set(dev_id, port_id,mode1, mode2);

	return rv;
}

static sw_error_t
_adpt_hppe_instance0_mode_get(a_uint32_t dev_id, a_uint32_t *mode0)
{
	a_uint32_t port_id = 0;

	for(port_id = SSDK_PHYSICAL_PORT1; port_id <= SSDK_PHYSICAL_PORT5; port_id++)
	{
		if (A_TRUE != hsl_port_prop_check (dev_id, port_id, HSL_PP_EXCL_CPU))
		{
			continue;
		}
		SSDK_DEBUG("port_id:%d, port_interface_mode:%d\n", port_id,
			port_interface_mode[dev_id][port_id]);
		if(port_interface_mode[dev_id][port_id] == PHY_PSGMII_BASET)
		{
			if(*mode0 != PORT_WRAPPER_MAX && *mode0 != PORT_WRAPPER_PSGMII)
			{
				SSDK_ERROR("when the port_interface_mode of port %d is %d, "
					"mode0:%d cannot be supported\n",
					port_id, port_interface_mode[dev_id][port_id], *mode0);
				return SW_NOT_SUPPORTED;
			}
			*mode0 = PORT_WRAPPER_PSGMII;
		}

		if(port_interface_mode[dev_id][port_id] == PHY_PSGMII_FIBER &&
			port_id == SSDK_PHYSICAL_PORT5)
		{
			*mode0 = PORT_WRAPPER_PSGMII_FIBER;
		}

		if(port_interface_mode[dev_id][port_id] == PORT_QSGMII)
		{
			if((*mode0 != PORT_WRAPPER_MAX && *mode0 != PORT_WRAPPER_QSGMII) ||
				port_id == SSDK_PHYSICAL_PORT5)
			{
				SSDK_ERROR("when the port_interface_mode of port %d is %d, "
					"mode0:%d cannot be supported\n",
					port_id, port_interface_mode[dev_id][port_id], *mode0);
				return SW_NOT_SUPPORTED;
			}
			*mode0 = PORT_WRAPPER_QSGMII;
		}

		if(port_interface_mode[dev_id][port_id] == PHY_SGMII_BASET ||
			port_interface_mode[dev_id][port_id] == PORT_SGMII_FIBER)
		{
			if(*mode0 !=PORT_WRAPPER_MAX)
			{
				if(port_id != SSDK_PHYSICAL_PORT5)
				{
					SSDK_ERROR("when the port_interface_mode of port %d is %d, "
						"mode0:%d cannot be supported\n",
						port_id, port_interface_mode[dev_id][port_id],
						*mode0);
					return SW_NOT_SUPPORTED;
				}
				else
				{
					return SW_OK;
				}
			}
			switch(port_id)
			{
				case SSDK_PHYSICAL_PORT1:
					if(port_interface_mode[dev_id][port_id] == PORT_SGMII_FIBER)
					{
						*mode0 = PORT_WRAPPER_SGMII_FIBER;
					}
					else
					{
						*mode0 = PORT_WRAPPER_SGMII_CHANNEL0;
					}
					break;
				case SSDK_PHYSICAL_PORT2:
					*mode0 = PORT_WRAPPER_SGMII_CHANNEL1;
					break;
#ifdef CPPE
				case SSDK_PHYSICAL_PORT4:
					if(adpt_hppe_chip_revision_get(dev_id) == CPPE_REVISION)
					{
						*mode0 = PORT_WRAPPER_SGMII_CHANNEL0;
					}
					break;
#endif
				case SSDK_PHYSICAL_PORT5:
					if(ssdk_dt_global_get_mac_mode(dev_id,
						SSDK_UNIPHY_INSTANCE1) == PORT_WRAPPER_MAX)
					{
						*mode0 = PORT_WRAPPER_SGMII_CHANNEL4;
					}
					break;
				default:
					SSDK_ERROR("port %d doesn't support "
						"port_interface_mode %d\n",
						port_id, port_interface_mode[dev_id][port_id]);
					return SW_NOT_SUPPORTED;
			}
		}
		if(port_id != SSDK_PHYSICAL_PORT5 &&
				(port_interface_mode[dev_id][port_id] == PORT_SGMII_PLUS ||
				 port_interface_mode[dev_id][port_id] ==PORT_USXGMII ||
				 port_interface_mode[dev_id][port_id] == PORT_10GBASE_R))
		{
#ifdef CPPE
			if(port_interface_mode[dev_id][port_id] == PORT_SGMII_PLUS
				&& adpt_hppe_chip_revision_get(dev_id) == CPPE_REVISION
				&& port_id == SSDK_PHYSICAL_PORT4)
			{
				*mode0 = PORT_WRAPPER_SGMII_PLUS;
				continue;
			}
#endif
			SSDK_ERROR("port %d doesn't support port_interface_mode %d\n",
				port_id, port_interface_mode[dev_id][port_id]);
			return SW_NOT_SUPPORTED;
		}
	}

	return SW_OK;
}

static sw_error_t
_adpt_hppe_instance1_mode_get(a_uint32_t dev_id, a_uint32_t port_id,  a_uint32_t *mode)
{
	if ((A_TRUE != hsl_port_prop_check (dev_id, port_id, HSL_PP_EXCL_CPU)) ||
		A_TRUE == hsl_port_prop_check (dev_id, port_id, HSL_PP_INNER))
	{
		return SW_OK;
	}
	SSDK_DEBUG("port_id:%x: %x\n", port_id, port_interface_mode[dev_id][port_id]);
	switch(port_interface_mode[dev_id][port_id])
	{
		case PHY_SGMII_BASET:
			*mode = PORT_WRAPPER_SGMII_CHANNEL0;
			break;
		case PORT_SGMII_PLUS:
			*mode = PORT_WRAPPER_SGMII_PLUS;
			break;
		case PORT_USXGMII:
			*mode = PORT_WRAPPER_USXGMII;
			break;
		case PORT_10GBASE_R:
			*mode = PORT_WRAPPER_10GBASE_R;
			break;
		case PORT_SGMII_FIBER:
			*mode = PORT_WRAPPER_SGMII_FIBER;
			break;
		case PHY_PSGMII_BASET:
		case PHY_PSGMII_FIBER:
			if(port_id == SSDK_PHYSICAL_PORT6)
			{
				SSDK_ERROR("port %d doesn't support port_interface_mode %d\n",
					port_id, port_interface_mode[dev_id][port_id]);
				return SW_NOT_SUPPORTED;
			}
			*mode = PORT_INTERFACE_MODE_MAX;
			break;
		case PORT_INTERFACE_MODE_MAX:
			break;
		default:
			SSDK_ERROR("port %d doesn't support port_interface_mode %d\n",
				port_id, port_interface_mode[dev_id][port_id]);
			return SW_NOT_SUPPORTED;
	}

	return SW_OK;
}

static sw_error_t
_adpt_hppe_instance_mode_get(a_uint32_t dev_id, a_uint32_t uniphy_index,
	a_uint32_t *interface_mode)
{
	sw_error_t rv = SW_OK;

	switch(uniphy_index)
	{
		case SSDK_UNIPHY_INSTANCE0:
			rv = _adpt_hppe_instance0_mode_get(dev_id, interface_mode);
			SW_RTN_ON_ERROR(rv);
			break;
		case SSDK_UNIPHY_INSTANCE1:
			rv =_adpt_hppe_instance1_mode_get(dev_id, SSDK_PHYSICAL_PORT5,
				interface_mode);
			SW_RTN_ON_ERROR(rv);
			break;
		case SSDK_UNIPHY_INSTANCE2:
			rv =_adpt_hppe_instance1_mode_get(dev_id, SSDK_PHYSICAL_PORT6,
				interface_mode);
			SW_RTN_ON_ERROR(rv);
			break;
		default:
			return SW_NOT_SUPPORTED;
	}

	return rv;
}

extern sw_error_t
adpt_hppe_uniphy_mode_set(a_uint32_t dev_id, a_uint32_t index, a_uint32_t mode);

static sw_error_t
_adpt_hppe_port_interface_mode_phy_config(a_uint32_t dev_id, a_uint32_t port_id,
	fal_port_interface_mode_t mode)
{
	sw_error_t rv;
	a_uint32_t phy_id = 0;
	hsl_phy_ops_t *phy_drv;

	ADPT_DEV_ID_CHECK(dev_id);

	if (A_TRUE != hsl_port_prop_check (dev_id, port_id, HSL_PP_PHY))
	{
		SSDK_ERROR("port %d is not in bitmap\n", port_id);
		return SW_BAD_PARAM;
	}
	SW_RTN_ON_NULL (phy_drv = hsl_phy_api_ops_get (dev_id, port_id));
	if (NULL == phy_drv->phy_interface_mode_set)
	{
		SSDK_ERROR("the phy_interface_mode_set api is null for port %d\n",
			port_id);
		return SW_NOT_SUPPORTED;
	}

	rv = hsl_port_prop_get_phyid (dev_id, port_id, &phy_id);
	SW_RTN_ON_ERROR (rv);
	SSDK_DEBUG("port_id:%d, phy_id:%d, mode:%d\n", port_id, phy_id, mode);
	rv = phy_drv->phy_interface_mode_set (dev_id, phy_id,mode);

	return rv;
}

static sw_error_t _adpt_hppe_port_mac_set(a_uint32_t dev_id, a_uint32_t port_id, a_bool_t enable)
{
	sw_error_t rv = SW_OK;

	rv = adpt_hppe_port_txmac_status_set(dev_id, port_id, enable);
	SW_RTN_ON_ERROR(rv);
	rv = adpt_hppe_port_rxmac_status_set(dev_id, port_id, enable);

	return rv;
}

static sw_error_t
_adpt_hppe_port_phyaddr_update(a_uint32_t dev_id, a_uint32_t port_id,
	a_uint32_t mode)
{
	sw_error_t rv = SW_OK;
	a_uint32_t phy_addr = 0, mode0 = PORT_WRAPPER_MAX;
	ssdk_port_phyinfo *port_phyinfo = NULL;

	if(port_id != SSDK_PHYSICAL_PORT5)
	{
		return rv;
	}
	if(mode == PORT_WRAPPER_10GBASE_R)
	{
		port_phyinfo = ssdk_port_phyinfo_get(dev_id, port_id);
		if (!port_phyinfo)
		{
			SSDK_ERROR("port_phyinfo of port%d is null\n", port_id);
			return SW_FAIL;
		}
		phy_addr = port_phyinfo->phy_addr;
		qca_ssdk_phy_address_set(dev_id, port_id, phy_addr);
		hsl_port_phy_access_type_set(dev_id, port_id, PHY_I2C_ACCESS);
		SSDK_DEBUG("port %x phy_addr is %x\n", port_id, phy_addr);
	}
	else
	{
		mode0 = ssdk_dt_global_get_mac_mode(dev_id, SSDK_UNIPHY_INSTANCE0);
		if((mode0 == PORT_WRAPPER_PSGMII || mode0 == PORT_WRAPPER_PSGMII_FIBER) &&
			mode == PORT_WRAPPER_MAX)
		{
			rv = hsl_port_prop_get_phyid (dev_id, SSDK_PHYSICAL_PORT4, &phy_addr);
			SW_RTN_ON_ERROR (rv);
			phy_addr++;
			qca_ssdk_phy_address_set(dev_id, port_id, phy_addr);
			hsl_port_phy_access_type_set(dev_id, port_id, PHY_MDIO_ACCESS);
			SSDK_DEBUG("port %x phy_addr is %x\n", port_id, phy_addr);
		}
	}

	return rv;
}

static sw_error_t
_adpt_hppe_sfp_copper_phydriver_switch(a_uint32_t dev_id, a_uint32_t port_id,
	a_uint32_t mode)
{
	sw_error_t rv = SW_OK;

	rv = _adpt_hppe_port_phyaddr_update(dev_id, port_id, mode);
	SW_RTN_ON_ERROR(rv);
	rv = hsl_phydriver_update(dev_id, port_id, mode);

	return rv;
}

static sw_error_t
_adpt_hppe_port_phy_config(a_uint32_t dev_id, a_uint32_t index, a_uint32_t mode)
{
	sw_error_t rv = SW_OK;

	switch(mode)
	{
		case PORT_WRAPPER_PSGMII:
			/*interface mode changed form psgmii+usxgmii to psgmii+10gbase-r+usxgmii, cannot
			use port 5 to configure malibu*/
			rv = _adpt_hppe_port_interface_mode_phy_config(dev_id,
				SSDK_PHYSICAL_PORT4, PHY_PSGMII_BASET);
			break;
		case PORT_WRAPPER_PSGMII_FIBER:
			rv = _adpt_hppe_port_interface_mode_phy_config(dev_id,
				SSDK_PHYSICAL_PORT4, PHY_PSGMII_FIBER);
			break;
		case PORT_WRAPPER_SGMII_CHANNEL4:
			rv = _adpt_hppe_port_interface_mode_phy_config(dev_id,
				SSDK_PHYSICAL_PORT5, PHY_SGMII_BASET);
			break;
		case PORT_WRAPPER_QSGMII:
			rv = _adpt_hppe_port_interface_mode_phy_config(dev_id,
				SSDK_PHYSICAL_PORT4, PORT_QSGMII);
			break;
		case PORT_WRAPPER_SGMII_CHANNEL0:
#ifdef CPPE
			if(index == SSDK_UNIPHY_INSTANCE0 &&
				adpt_hppe_chip_revision_get(dev_id) == CPPE_REVISION &&
				(hsl_port_prop_check (dev_id, SSDK_PHYSICAL_PORT4,
					HSL_PP_EXCL_CPU)))
			{
					rv = _adpt_hppe_port_interface_mode_phy_config(dev_id,
						SSDK_PHYSICAL_PORT4, PHY_SGMII_BASET);
			}
#endif
			if(index == SSDK_UNIPHY_INSTANCE1)
			{
				rv = _adpt_hppe_port_interface_mode_phy_config(dev_id,
					SSDK_PHYSICAL_PORT5, PHY_SGMII_BASET);
			}
			if(index == SSDK_UNIPHY_INSTANCE2)
			{
				rv = _adpt_hppe_port_interface_mode_phy_config(dev_id,
					SSDK_PHYSICAL_PORT6, PHY_SGMII_BASET);
			}
			break;
		case PORT_WRAPPER_USXGMII:
			if(index == SSDK_UNIPHY_INSTANCE1)
			{
				rv = _adpt_hppe_port_interface_mode_phy_config(dev_id,
					SSDK_PHYSICAL_PORT5, PORT_USXGMII);
			}
			if(index == SSDK_UNIPHY_INSTANCE2)
			{
				rv = _adpt_hppe_port_interface_mode_phy_config(dev_id,
					SSDK_PHYSICAL_PORT6, PORT_USXGMII);
			}
			break;
		case PORT_WRAPPER_SGMII_PLUS:
#ifdef CPPE
			if(index == SSDK_UNIPHY_INSTANCE0 &&
				adpt_hppe_chip_revision_get(dev_id) == CPPE_REVISION)
			{
				rv = _adpt_hppe_port_interface_mode_phy_config(dev_id,
					SSDK_PHYSICAL_PORT4, PORT_SGMII_PLUS);
			}
#endif
			if(index == SSDK_UNIPHY_INSTANCE1)
			{
				rv = _adpt_hppe_port_interface_mode_phy_config(dev_id,
					SSDK_PHYSICAL_PORT5, PORT_SGMII_PLUS);
			}
			if(index == SSDK_UNIPHY_INSTANCE2)
			{
				rv = _adpt_hppe_port_interface_mode_phy_config(dev_id,
					SSDK_PHYSICAL_PORT6, PORT_SGMII_PLUS);
			}
			break;
		default:
			break;
	}

	return rv;
}

static sw_error_t
adpt_hppe_port_mac_uniphy_phy_config(a_uint32_t dev_id, a_uint32_t mode_index,
	a_uint32_t mode[], a_bool_t force_switch)
{
	sw_error_t rv = SW_OK;
	a_uint32_t port_id = 0, port_id_from = 0, port_id_end = 0;

	if(mode_index == SSDK_UNIPHY_INSTANCE0)
	{
		switch (mode[SSDK_UNIPHY_INSTANCE0])
		{
			case PORT_WRAPPER_PSGMII:
			case PORT_WRAPPER_PSGMII_FIBER:
				port_id_from = SSDK_PHYSICAL_PORT1;
				/*qsgmii+10gbase-r+usxgmii --> psgmii+10gbase-r+usxgmii*/
				if(mode[SSDK_UNIPHY_INSTANCE1] != PORT_WRAPPER_MAX)
				{
					port_id_end = SSDK_PHYSICAL_PORT4;
				}
				else
				{
					port_id_end = SSDK_PHYSICAL_PORT5;
				}
				break;
			case PORT_WRAPPER_QSGMII:
				port_id_from = SSDK_PHYSICAL_PORT1;
				port_id_end = SSDK_PHYSICAL_PORT4;
				break;
			case PORT_WRAPPER_SGMII0_RGMII4:
			case PORT_WRAPPER_SGMII_CHANNEL0:
			case PORT_WRAPPER_SGMII_FIBER:
#ifdef CPPE
			case PORT_WRAPPER_SGMII_PLUS:
				if(adpt_hppe_chip_revision_get(dev_id) == CPPE_REVISION)
				{
					a_uint32_t mode_tmp = 0;
					mode_tmp = mode[SSDK_UNIPHY_INSTANCE0];
					if (mode_tmp == PORT_WRAPPER_SGMII_PLUS ||
						((mode_tmp == PORT_WRAPPER_SGMII0_RGMII4 ||
						mode_tmp == PORT_WRAPPER_SGMII_CHANNEL0) &&
						(hsl_port_prop_check (dev_id, SSDK_PHYSICAL_PORT4,
							HSL_PP_EXCL_CPU))))
					{
						port_id_from = SSDK_PHYSICAL_PORT4;
						port_id_end = SSDK_PHYSICAL_PORT4;
						break;
					}
				}
#endif
				port_id_from = SSDK_PHYSICAL_PORT1;
				port_id_end = SSDK_PHYSICAL_PORT1;
				break;
			case PORT_WRAPPER_SGMII1_RGMII4:
			case PORT_WRAPPER_SGMII_CHANNEL1:
				port_id_from = SSDK_PHYSICAL_PORT2;
				port_id_end = SSDK_PHYSICAL_PORT2;
				break;
			case PORT_WRAPPER_SGMII4_RGMII4:
			case PORT_WRAPPER_SGMII_CHANNEL4:
				port_id_from = SSDK_PHYSICAL_PORT5;
				port_id_end = SSDK_PHYSICAL_PORT5;
				break;
			default:
				SSDK_INFO ("uniphy %d interface mode is 0x%x\n",
					mode_index, mode[SSDK_UNIPHY_INSTANCE0]);
				return SW_OK;
		}
	}
	else if(mode_index == SSDK_UNIPHY_INSTANCE1)
	{
		port_id_from = SSDK_PHYSICAL_PORT5;
		port_id_end = SSDK_PHYSICAL_PORT5;
	}
	else if(mode_index == SSDK_UNIPHY_INSTANCE2)
	{
		port_id_from = SSDK_PHYSICAL_PORT6;
		port_id_end = SSDK_PHYSICAL_PORT6;
	}
	else
	{
		return SW_NOT_SUPPORTED;
	}
	/*disable mac tx and rx for special ports*/
	for(port_id = port_id_from; port_id <= port_id_end; port_id++)
	{
		rv = _adpt_hppe_port_mac_set(dev_id, port_id, A_FALSE);
	}

	/*configure the uniphy*/
	rv = adpt_hppe_uniphy_mode_set(dev_id, mode_index, mode[mode_index]);
	SSDK_DEBUG("configure uniphy mode_index:%x, mode:%x, rv:%x\n",
		mode_index, mode[mode_index], rv);
	SW_RTN_ON_ERROR(rv);

	/*configure the mac*/
	for(port_id = port_id_from; port_id <= port_id_end; port_id++)
	{
		rv = adpt_hppe_port_mux_mac_type_set(dev_id, port_id, mode[SSDK_UNIPHY_INSTANCE0],
			mode[SSDK_UNIPHY_INSTANCE1], mode[SSDK_UNIPHY_INSTANCE2]);
		SSDK_DEBUG("configure mac, port_id is %x,mode0:%x,mode1:%x,mode2:%x, rv:%x\n",
			port_id, mode[SSDK_UNIPHY_INSTANCE0], mode[SSDK_UNIPHY_INSTANCE1],
			mode[SSDK_UNIPHY_INSTANCE2], rv);
		SW_RTN_ON_ERROR(rv);
	}

	/*configure the phy*/
	if(SSDK_PHYSICAL_PORT5 >= port_id_from && SSDK_PHYSICAL_PORT5 <= port_id_end)
	{
		rv = _adpt_hppe_sfp_copper_phydriver_switch(dev_id, SSDK_PHYSICAL_PORT5,
			mode[SSDK_UNIPHY_INSTANCE1]);
	}
	SW_RTN_ON_ERROR(rv);
	if (force_switch != A_FALSE) {
		rv = _adpt_hppe_port_phy_config(dev_id, mode_index, mode[mode_index]);
		SSDK_DEBUG("configure phy, mode_index:%x,interface_mode:%x, rv:%x\n",
			mode_index, mode[mode_index], rv);
	}

	/*init port status for special ports to triger polling*/
	for(port_id = port_id_from; port_id <= port_id_end; port_id++)
	{
		adpt_hppe_port_txfc_status_set(dev_id, port_id, A_FALSE);
		adpt_hppe_port_rxfc_status_set(dev_id, port_id, A_FALSE);
		qca_mac_port_status_init(dev_id, port_id);
	}

	return rv;
}

sw_error_t
_adpt_hppe_port_interface_mode_apply(a_uint32_t dev_id, a_bool_t force_switch)
{
	sw_error_t rv = SW_OK;
	a_uint32_t mode_index = 0, mode_old[3] = {0};
	a_uint32_t mode_new[3] = {PORT_WRAPPER_MAX,PORT_WRAPPER_MAX,PORT_WRAPPER_MAX};

	/*get three intances mode, include old mode and new mode*/
	for(mode_index = SSDK_UNIPHY_INSTANCE0; mode_index <= SSDK_UNIPHY_INSTANCE2; mode_index++)
	{
		mode_old[mode_index] = ssdk_dt_global_get_mac_mode(dev_id, mode_index);
		if(mode_index == SSDK_UNIPHY_INSTANCE1 &&
			mode_new[SSDK_UNIPHY_INSTANCE0] == PORT_WRAPPER_SGMII_CHANNEL4)
		{
			mode_new[SSDK_UNIPHY_INSTANCE1] = PORT_WRAPPER_MAX;
		}
		else
		{
			rv = _adpt_hppe_instance_mode_get(dev_id, mode_index, &mode_new[mode_index]);
			SW_RTN_ON_ERROR(rv);
		}
	}
	SSDK_DEBUG("mode0_old: %x, mode1_old:%x, mode2_old:%x\n",
			mode_old[0], mode_old[1], mode_old[2]);
	SSDK_DEBUG("mode0_new: %x, mode1_new:%x, mode2_new:%x\n",
			mode_new[0], mode_new[1], mode_new[2]);
	/*set three new intances mode*/
	for(mode_index = SSDK_UNIPHY_INSTANCE0; mode_index <= SSDK_UNIPHY_INSTANCE2; mode_index++)
	{
		ssdk_dt_global_set_mac_mode(dev_id, mode_index, mode_new[mode_index]);
	}

	for (mode_index = SSDK_UNIPHY_INSTANCE0; mode_index <= SSDK_UNIPHY_INSTANCE2; mode_index++)
	{
		ssdk_gcc_uniphy_sys_set(dev_id, mode_index, A_TRUE);
	}
	ssdk_uniphy_port5_clock_source_set();

	/*configure the mode according to mode_new*/
	for(mode_index = SSDK_UNIPHY_INSTANCE0; mode_index <= SSDK_UNIPHY_INSTANCE2; mode_index++)
	{
		if(mode_new[mode_index] != mode_old[mode_index])
		{
			SSDK_DEBUG("need to configure instance%x\n", mode_index);
			rv = adpt_hppe_port_mac_uniphy_phy_config(dev_id,
				mode_index, mode_new, force_switch);
			if(rv)
			{
				SSDK_ERROR("config instance%x, rv:%x faild\n", mode_index,rv);
				return rv;
			}
		}
		if (mode_new[mode_index] == PORT_WRAPPER_MAX) {
			ssdk_gcc_uniphy_sys_set(dev_id, mode_index, A_FALSE);
		}
	}

	return rv;
}
#ifndef IN_PORTCONTROL_MINI
sw_error_t
adpt_hppe_port_interface_mode_apply(a_uint32_t dev_id)
{
	struct qca_phy_priv *priv;
	sw_error_t rv = SW_OK;

	ADPT_DEV_ID_CHECK(dev_id);
	priv = ssdk_phy_priv_data_get(dev_id);
	if (!priv)
	{
		return SW_FAIL;
	}

	mutex_lock(&priv->mac_sw_sync_lock);
	rv = _adpt_hppe_port_interface_mode_apply(dev_id, A_TRUE);
	mutex_unlock(&priv->mac_sw_sync_lock);

	qca_mac_sw_sync_work_resume(priv);

	return rv;
}

sw_error_t
adpt_hppe_port_mac_loopback_get(a_uint32_t dev_id, fal_port_t port_id,
				 a_bool_t * enable)
{
	sw_error_t rv = SW_OK;
	a_uint32_t port_mac_type = 0;

	ADPT_DEV_ID_CHECK(dev_id);
	ADPT_NULL_POINT_CHECK(enable);

	port_mac_type = qca_hppe_port_mac_type_get(dev_id, port_id);
	if (port_mac_type == PORT_XGMAC_TYPE)
		rv = _adpt_hppe_port_xgmac_loopback_get( dev_id, port_id, enable);
	else if (port_mac_type == PORT_GMAC_TYPE)
		rv = _adpt_hppe_port_gmac_loopback_get( dev_id, port_id, enable);
	else
		return SW_BAD_VALUE;

	return rv;
}

sw_error_t
adpt_hppe_port_hibernate_get(a_uint32_t dev_id, fal_port_t port_id,
			  a_bool_t * enable)
{

	sw_error_t rv = 0;
	a_uint32_t phy_id = 0;
	hsl_phy_ops_t *phy_drv;

	ADPT_DEV_ID_CHECK(dev_id);
	ADPT_NULL_POINT_CHECK(enable);

	if (A_TRUE != hsl_port_prop_check (dev_id, port_id, HSL_PP_PHY))
	  {
		return SW_BAD_PARAM;
	  }
	if (A_FALSE == _adpt_hppe_port_phy_connected (dev_id, port_id))
		return SW_NOT_SUPPORTED;

	SW_RTN_ON_NULL (phy_drv = hsl_phy_api_ops_get (dev_id, port_id));
	if (NULL == phy_drv->phy_hibernation_get)
		return SW_NOT_SUPPORTED;

	rv = hsl_port_prop_get_phyid (dev_id, port_id, &phy_id);
	SW_RTN_ON_ERROR (rv);

	rv = phy_drv->phy_hibernation_get (dev_id, phy_id, enable);

	return rv;

}
#endif
sw_error_t
adpt_hppe_port_autoneg_adv_set(a_uint32_t dev_id, fal_port_t port_id,
				     a_uint32_t autoadv)
{
	sw_error_t rv = 0;
	a_uint32_t phy_id;
	hsl_phy_ops_t *phy_drv;

	ADPT_DEV_ID_CHECK(dev_id);

	if (A_TRUE != hsl_port_prop_check (dev_id, port_id, HSL_PP_PHY))
	  {
		return SW_BAD_PARAM;
	  }
	if (A_FALSE == _adpt_hppe_port_phy_connected (dev_id, port_id))
		return SW_NOT_SUPPORTED;

	SW_RTN_ON_NULL (phy_drv = hsl_phy_api_ops_get (dev_id, port_id));
	if (NULL == phy_drv->phy_autoneg_adv_set)
		return SW_NOT_SUPPORTED;

	rv = hsl_port_prop_get_phyid (dev_id, port_id, &phy_id);
	SW_RTN_ON_ERROR (rv);

	rv = phy_drv->phy_autoneg_adv_set (dev_id, phy_id, autoadv);
	SW_RTN_ON_ERROR (rv);
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(5, 0, 0))
	rv = hsl_port_phydev_adv_update (dev_id, port_id, autoadv);
	SW_RTN_ON_ERROR (rv);
#endif

	return SW_OK;

}
#ifndef IN_PORTCONTROL_MINI
sw_error_t
adpt_hppe_port_remote_loopback_get(a_uint32_t dev_id, fal_port_t port_id,
					 a_bool_t * enable)
{

	sw_error_t rv = 0;
	a_uint32_t phy_id = 0;
	hsl_phy_ops_t *phy_drv;

	ADPT_DEV_ID_CHECK(dev_id);
	ADPT_NULL_POINT_CHECK(enable);

	if (A_TRUE != hsl_port_prop_check (dev_id, port_id, HSL_PP_PHY))
	  {
		return SW_BAD_PARAM;
	  }
	if (A_FALSE == _adpt_hppe_port_phy_connected (dev_id, port_id))
		return SW_NOT_SUPPORTED;

	SW_RTN_ON_NULL (phy_drv = hsl_phy_api_ops_get (dev_id, port_id));
	if (NULL == phy_drv->phy_remote_loopback_get)
		return SW_NOT_SUPPORTED;

	rv = hsl_port_prop_get_phyid (dev_id, port_id, &phy_id);
	SW_RTN_ON_ERROR (rv);

	rv = phy_drv->phy_remote_loopback_get (dev_id, phy_id, enable);

	return rv;

}

sw_error_t
adpt_hppe_port_counter_show(a_uint32_t dev_id, fal_port_t port_id,
				 fal_port_counter_info_t * counter_info)
{

	sw_error_t rv = 0;
	a_uint32_t phy_id = 0;
	hsl_phy_ops_t *phy_drv;

	ADPT_DEV_ID_CHECK(dev_id);
	ADPT_NULL_POINT_CHECK(counter_info);

	if (A_TRUE != hsl_port_prop_check (dev_id, port_id, HSL_PP_PHY))
	  {
		return SW_BAD_PARAM;
	  }
	if (A_FALSE == _adpt_hppe_port_phy_connected (dev_id, port_id))
		return SW_NOT_SUPPORTED;

	SW_RTN_ON_NULL (phy_drv = hsl_phy_api_ops_get (dev_id, port_id));
	if (NULL == phy_drv->phy_counter_show)
	  return SW_NOT_SUPPORTED;

	rv = hsl_port_prop_get_phyid (dev_id, port_id, &phy_id);
	SW_RTN_ON_ERROR (rv);

	rv = phy_drv->phy_counter_show (dev_id, phy_id,counter_info);

	return rv;

}
sw_error_t
adpt_hppe_port_mtu_get(a_uint32_t dev_id, fal_port_t port_id,
		fal_mtu_ctrl_t *ctrl)
{
	sw_error_t rv = SW_OK;
	union mru_mtu_ctrl_tbl_u mru_mtu_ctrl_tbl;

	memset(&mru_mtu_ctrl_tbl, 0, sizeof(mru_mtu_ctrl_tbl));
	ADPT_DEV_ID_CHECK(dev_id);
	ADPT_NULL_POINT_CHECK(ctrl);

	rv = hppe_mru_mtu_ctrl_tbl_get(dev_id, port_id, &mru_mtu_ctrl_tbl);

	if( rv != SW_OK )
		return rv;

	ctrl->mtu_size = mru_mtu_ctrl_tbl.bf.mtu;
	ctrl->action = (fal_fwd_cmd_t)mru_mtu_ctrl_tbl.bf.mtu_cmd;

	return SW_OK;
}

#endif
sw_error_t
adpt_hppe_port_autoneg_enable(a_uint32_t dev_id, fal_port_t port_id)
{

	sw_error_t rv = 0;
	a_uint32_t phy_id;
	hsl_phy_ops_t *phy_drv;

	ADPT_DEV_ID_CHECK(dev_id);

	if (A_TRUE != hsl_port_prop_check (dev_id, port_id, HSL_PP_PHY))
	  {
		return SW_BAD_PARAM;
	  }
	if (A_FALSE == _adpt_hppe_port_phy_connected (dev_id, port_id))
		return SW_NOT_SUPPORTED;

	SW_RTN_ON_NULL (phy_drv = hsl_phy_api_ops_get (dev_id, port_id));
	if (NULL == phy_drv->phy_autoneg_enable_set)
		return SW_NOT_SUPPORTED;

	rv = hsl_port_prop_get_phyid (dev_id, port_id, &phy_id);
	SW_RTN_ON_ERROR (rv);

	rv = phy_drv->phy_autoneg_enable_set (dev_id, phy_id);
	return rv;

}

#ifndef IN_PORTCONTROL_MINI
sw_error_t
adpt_ppe_port_mtu_get(a_uint32_t dev_id, fal_port_t port_id,
		fal_mtu_ctrl_t *ctrl)
{
	a_uint32_t chip_ver = 0;

	ADPT_DEV_ID_CHECK(dev_id);
	ADPT_NULL_POINT_CHECK(ctrl);

	chip_ver = adpt_hppe_chip_revision_get(dev_id);
	if (chip_ver == CPPE_REVISION) {
#if defined(CPPE)
		return adpt_cppe_port_mtu_get(dev_id, port_id, ctrl);
#endif
	} else {
		return adpt_hppe_port_mtu_get(dev_id, port_id, ctrl);
	}

	return SW_NOT_SUPPORTED;
}
#endif

sw_error_t
adpt_hppe_port_interface_mode_status_get(a_uint32_t dev_id, fal_port_t port_id,
			      fal_port_interface_mode_t * mode)
{

	sw_error_t rv = SW_OK;
	a_uint32_t phy_id = 0;
	hsl_phy_ops_t *phy_drv;

	ADPT_DEV_ID_CHECK(dev_id);
	ADPT_NULL_POINT_CHECK(mode);

	if (A_TRUE != hsl_port_prop_check (dev_id, port_id, HSL_PP_PHY))
	  {
		return SW_BAD_PARAM;
	  }

	/* for those ports without PHY device should be sfp port */
	if (A_FALSE == _adpt_hppe_port_phy_connected (dev_id, port_id)) {
		rv = sfp_phy_interface_get_mode_status(dev_id, port_id, mode);
		SW_RTN_ON_ERROR (rv);
	} else {
		SW_RTN_ON_NULL(phy_drv = hsl_phy_api_ops_get (dev_id, port_id));
		if (NULL == phy_drv->phy_interface_mode_status_get)
			return SW_NOT_SUPPORTED;

		rv = hsl_port_prop_get_phyid (dev_id, port_id, &phy_id);
		SW_RTN_ON_ERROR (rv);

		rv = phy_drv->phy_interface_mode_status_get (dev_id, phy_id,mode);
	}
	return rv;
}
#ifndef IN_PORTCONTROL_MINI
sw_error_t
adpt_hppe_port_reset(a_uint32_t dev_id, fal_port_t port_id)
{

	sw_error_t rv;
	a_uint32_t phy_id = 0;
	hsl_phy_ops_t *phy_drv;

	ADPT_DEV_ID_CHECK(dev_id);

	if (A_TRUE != hsl_port_prop_check (dev_id, port_id, HSL_PP_PHY))
	  {
		return SW_BAD_PARAM;
	  }
	if (A_FALSE == _adpt_hppe_port_phy_connected (dev_id, port_id))
		return SW_NOT_SUPPORTED;

	SW_RTN_ON_NULL (phy_drv = hsl_phy_api_ops_get (dev_id, port_id));
	if (NULL == phy_drv->phy_reset)
		return SW_NOT_SUPPORTED;

	rv = hsl_port_prop_get_phyid (dev_id, port_id, &phy_id);
	SW_RTN_ON_ERROR (rv);

	rv = phy_drv->phy_reset(dev_id, phy_id);

	return rv;

}
#endif

sw_error_t
adpt_hppe_port_flowctrl_set(a_uint32_t dev_id, fal_port_t port_id,
				  a_bool_t enable)
{
	sw_error_t rv = SW_OK;
	struct qca_phy_priv *priv = ssdk_phy_priv_data_get(dev_id);

	if (!priv)
		return SW_FAIL;

	if ((port_id < SSDK_PHYSICAL_PORT1) || (port_id > SSDK_PHYSICAL_PORT6))
		return SW_BAD_VALUE;
#if defined(CPPE)
	if ((adpt_hppe_chip_revision_get(dev_id) == CPPE_REVISION) &&
		(port_id == SSDK_PHYSICAL_PORT6)) {
		return adpt_cppe_switch_port_loopback_flowctrl_set(dev_id,
				port_id, enable);
	}
#endif
	rv = adpt_hppe_port_txfc_status_set(dev_id, port_id, enable);
	rv |= adpt_hppe_port_rxfc_status_set(dev_id, port_id, enable);

	if(rv != SW_OK)
		return rv;

	priv->port_old_tx_flowctrl[port_id - 1] = enable;
	priv->port_old_rx_flowctrl[port_id - 1] = enable;

	return SW_OK;
}
sw_error_t
adpt_hppe_port_speed_get(a_uint32_t dev_id, fal_port_t port_id,
			       fal_port_speed_t * pspeed)
{
	sw_error_t rv = 0;
	a_uint32_t phy_id;
	hsl_phy_ops_t *phy_drv;
	struct port_phy_status phy_status = {0};

	ADPT_DEV_ID_CHECK(dev_id);
	ADPT_NULL_POINT_CHECK(pspeed);

	if (A_TRUE != hsl_port_prop_check (dev_id, port_id, HSL_PP_INCL_CPU))
	{
		return SW_BAD_PARAM;
	}

	/* for those ports without PHY device should be sfp port */
	if (A_FALSE == _adpt_hppe_port_phy_connected (dev_id, port_id))
	{
		if (port_id == SSDK_PHYSICAL_PORT0)
			*pspeed = FAL_SPEED_10000;
		else {
			rv = _adpt_phy_status_get_from_ppe(dev_id,
				port_id, &phy_status);
			SW_RTN_ON_ERROR (rv);
			*pspeed= phy_status.speed;
		}
	}
	else
	{
		SW_RTN_ON_NULL (phy_drv = hsl_phy_api_ops_get (dev_id, port_id));
		if (NULL == phy_drv->phy_speed_get)
			return SW_NOT_SUPPORTED;

		rv = hsl_port_prop_get_phyid (dev_id, port_id, &phy_id);
		SW_RTN_ON_ERROR (rv);
		rv = phy_drv->phy_speed_get (dev_id, phy_id, pspeed);
		SW_RTN_ON_ERROR (rv);
	}

	return rv;
}
#ifndef IN_PORTCONTROL_MINI
sw_error_t
adpt_hppe_port_mdix_set(a_uint32_t dev_id, fal_port_t port_id,
			      fal_port_mdix_mode_t mode)
{

	sw_error_t rv = 0;
	a_uint32_t phy_id = 0;
	hsl_phy_ops_t *phy_drv;

	ADPT_DEV_ID_CHECK(dev_id);

	if (A_TRUE != hsl_port_prop_check (dev_id, port_id, HSL_PP_PHY))
	  {
		return SW_BAD_PARAM;
	  }
	if (A_FALSE == _adpt_hppe_port_phy_connected (dev_id, port_id))
		return SW_NOT_SUPPORTED;

	SW_RTN_ON_NULL (phy_drv = hsl_phy_api_ops_get (dev_id, port_id));
	if (NULL == phy_drv->phy_mdix_set)
		return SW_NOT_SUPPORTED;

	rv = hsl_port_prop_get_phyid (dev_id, port_id, &phy_id);
	SW_RTN_ON_ERROR (rv);

	rv = phy_drv->phy_mdix_set (dev_id, phy_id, mode);

	return rv;

}
sw_error_t
adpt_hppe_port_wol_status_get(a_uint32_t dev_id, fal_port_t port_id,
			      a_bool_t * enable)
{

	sw_error_t rv = 0;
	a_uint32_t phy_id = 0;
	hsl_phy_ops_t *phy_drv;

	ADPT_DEV_ID_CHECK(dev_id);
	ADPT_NULL_POINT_CHECK(enable);

	if (A_TRUE != hsl_port_prop_check (dev_id, port_id, HSL_PP_PHY))
	{
		return SW_BAD_PARAM;
	}
	if (A_FALSE == _adpt_hppe_port_phy_connected (dev_id, port_id))
		return SW_NOT_SUPPORTED;

	SW_RTN_ON_NULL (phy_drv = hsl_phy_api_ops_get (dev_id, port_id));
	if (NULL == phy_drv->phy_wol_status_get)
	  return SW_NOT_SUPPORTED;

	rv = hsl_port_prop_get_phyid (dev_id, port_id, &phy_id);
	SW_RTN_ON_ERROR (rv);

	rv = phy_drv->phy_wol_status_get (dev_id, phy_id, enable);

	return rv;

}

sw_error_t
adpt_hppe_port_source_filter_get(a_uint32_t dev_id,
				fal_port_t port_id, a_bool_t * enable)
{
	sw_error_t rv = SW_OK;
	union port_in_forward_u port_in_forward = {0};

	ADPT_DEV_ID_CHECK(dev_id);

	rv = hppe_port_in_forward_get(dev_id, port_id, &port_in_forward);

	if (rv != SW_OK)
		return rv;

	if (port_in_forward.bf.source_filtering_bypass == A_TRUE)
		*enable = A_FALSE;
	else
		*enable = A_TRUE;

	return SW_OK;
}

sw_error_t
adpt_hppe_port_source_filter_set(a_uint32_t dev_id,
				fal_port_t port_id, a_bool_t enable)
{
	union port_in_forward_u port_in_forward = {0};

	ADPT_DEV_ID_CHECK(dev_id);

	if (enable == A_TRUE)
		port_in_forward.bf.source_filtering_bypass = A_FALSE;
	else
		port_in_forward.bf.source_filtering_bypass = A_TRUE;

	return hppe_port_in_forward_set(dev_id, port_id, &port_in_forward);
}

sw_error_t
adpt_ppe_port_source_filter_get(a_uint32_t dev_id,
				fal_port_t port_id, a_bool_t * enable)
{
	ADPT_DEV_ID_CHECK(dev_id);
#ifdef CPPE
	if (adpt_hppe_chip_revision_get(dev_id) == CPPE_REVISION) {
		return adpt_cppe_port_source_filter_get(dev_id, port_id, enable);
	}
#endif
	return adpt_hppe_port_source_filter_get(dev_id, port_id, enable);
}

sw_error_t
adpt_ppe_port_source_filter_set(a_uint32_t dev_id,
				fal_port_t port_id, a_bool_t enable)

{
	ADPT_DEV_ID_CHECK(dev_id);
#ifdef CPPE
	if (adpt_hppe_chip_revision_get(dev_id) == CPPE_REVISION) {
		return adpt_cppe_port_source_filter_set(dev_id, port_id, enable);
	}
#endif
	return adpt_hppe_port_source_filter_set(dev_id, port_id, enable);
}

sw_error_t
adpt_ppe_port_source_filter_config_get(a_uint32_t dev_id,
				fal_port_t port_id, fal_src_filter_config_t* src_filter_config)
{
	ADPT_DEV_ID_CHECK(dev_id);
#ifdef CPPE
	if (adpt_hppe_chip_revision_get(dev_id) == CPPE_REVISION) {
		return adpt_cppe_port_source_filter_config_get(dev_id, port_id,
				src_filter_config);
	}
#endif
	return SW_NOT_SUPPORTED;
}

sw_error_t
adpt_ppe_port_source_filter_config_set(a_uint32_t dev_id,
				fal_port_t port_id, fal_src_filter_config_t *src_filter_config)

{
	ADPT_DEV_ID_CHECK(dev_id);
#ifdef CPPE
	if (adpt_hppe_chip_revision_get(dev_id) == CPPE_REVISION) {
		return adpt_cppe_port_source_filter_config_set(dev_id, port_id, src_filter_config);
	}
#endif
	return SW_NOT_SUPPORTED;
}

static sw_error_t
adpt_hppe_port_interface_3az_set(a_uint32_t dev_id, fal_port_t port_id, a_bool_t enable)
{
	sw_error_t rv = 0;
	union lpi_enable_u lpi_enable = {0};
	union lpi_port_timer_u lpi_port_timer = {0};
	a_uint32_t phy_id = 0;
	hsl_phy_ops_t *phy_drv;

	ADPT_DEV_ID_CHECK(dev_id);

	if (A_TRUE != hsl_port_prop_check(dev_id, port_id, HSL_PP_PHY))
	{
		return SW_BAD_PARAM;
	}
	if (A_FALSE == _adpt_hppe_port_phy_connected (dev_id, port_id))
		return SW_NOT_SUPPORTED;

	SW_RTN_ON_NULL(phy_drv = hsl_phy_api_ops_get(dev_id, port_id));
	if (NULL == phy_drv->phy_8023az_set)
		return SW_NOT_SUPPORTED;

	rv = hsl_port_prop_get_phyid(dev_id, port_id, &phy_id);
	SW_RTN_ON_ERROR (rv);

	rv = phy_drv->phy_8023az_set(dev_id, phy_id, enable);
	SW_RTN_ON_ERROR (rv);

	hppe_lpi_enable_get(dev_id, port_id, &lpi_enable);

	lpi_enable.val &= ~(0x1 << (port_id - 1));
	lpi_enable.val |= (((a_uint32_t)enable) << (port_id - 1));
	hppe_lpi_enable_set(dev_id, port_id, &lpi_enable);

	lpi_port_timer.bf.lpi_port_wakeup_timer = LPI_WAKEUP_TIMER;
	lpi_port_timer.bf.lpi_port_sleep_timer = LPI_SLEEP_TIMER;
	hppe_lpi_timer_set(dev_id, port_id, &lpi_port_timer);

	return SW_OK;
}
static sw_error_t
adpt_hppe_port_interface_3az_get(a_uint32_t dev_id, fal_port_t port_id, a_bool_t *enable)
{
	sw_error_t rv = 0;
	union lpi_enable_u lpi_enable = {0};
	a_uint32_t phy_id = 0;
	hsl_phy_ops_t *phy_drv;
	a_bool_t phy_status = 0, mac_status = 0;

	ADPT_DEV_ID_CHECK(dev_id);
	ADPT_NULL_POINT_CHECK(enable);

	if (A_TRUE != hsl_port_prop_check(dev_id, port_id, HSL_PP_PHY))
	{
		return SW_BAD_PARAM;
	}
	if (A_FALSE == _adpt_hppe_port_phy_connected (dev_id, port_id))
		return SW_NOT_SUPPORTED;

	SW_RTN_ON_NULL (phy_drv =hsl_phy_api_ops_get (dev_id, port_id));
	if (NULL == phy_drv->phy_8023az_get)
		return SW_NOT_SUPPORTED;

	rv = hsl_port_prop_get_phyid(dev_id, port_id, &phy_id);
	SW_RTN_ON_ERROR (rv);

	rv = phy_drv->phy_8023az_get(dev_id, phy_id, &phy_status);
	SW_RTN_ON_ERROR (rv);

	rv = hppe_lpi_enable_get(dev_id, port_id, &lpi_enable);

	if(((lpi_enable.val >> (port_id - 1)) & 0x1) == A_TRUE)
		mac_status = A_TRUE;
	else
		mac_status = A_FALSE;

	*enable = (phy_status & mac_status);

	return SW_OK;
}

sw_error_t
adpt_hppe_port_promisc_mode_get(a_uint32_t dev_id, fal_port_t port_id, a_bool_t *enable)
{
	sw_error_t rv = SW_OK;
	union port_bridge_ctrl_u port_bridge_ctrl = {0};

	ADPT_DEV_ID_CHECK(dev_id);
	ADPT_NULL_POINT_CHECK(enable);

	rv = hppe_port_bridge_ctrl_get(dev_id, port_id, &port_bridge_ctrl);

	if( rv != SW_OK )
		return rv;

	*enable = port_bridge_ctrl.bf.promisc_en;

	return SW_OK;
}
#endif

sw_error_t
adpt_hppe_port_promisc_mode_set(a_uint32_t dev_id, fal_port_t port_id, a_bool_t enable)
{
	sw_error_t rv = SW_OK;
	union port_bridge_ctrl_u port_bridge_ctrl = {0};

	ADPT_DEV_ID_CHECK(dev_id);

	rv = hppe_port_bridge_ctrl_get(dev_id, port_id, &port_bridge_ctrl);

	if( rv != SW_OK )
		return rv;

	port_bridge_ctrl.bf.promisc_en = enable;

	return hppe_port_bridge_ctrl_set(dev_id, port_id, &port_bridge_ctrl);
}

static sw_error_t
adpt_hppe_port_bridge_txmac_set(a_uint32_t dev_id, fal_port_t port_id, a_bool_t enable)
{
	sw_error_t rv = SW_OK;
	union port_bridge_ctrl_u port_bridge_ctrl = {0};

	ADPT_DEV_ID_CHECK(dev_id);

	rv = hppe_port_bridge_ctrl_get(dev_id, port_id, &port_bridge_ctrl);

	if( rv != SW_OK )
		return rv;

	if(enable == A_TRUE)
		port_bridge_ctrl.bf.txmac_en= 1;
	else
		port_bridge_ctrl.bf.txmac_en= 0;

	return hppe_port_bridge_ctrl_set(dev_id, port_id, &port_bridge_ctrl);
}

sw_error_t
adpt_hppe_port_mux_set(a_uint32_t dev_id, fal_port_t port_id, a_uint32_t port_type)
{
	sw_error_t rv = SW_OK;
	union port_mux_ctrl_u port_mux_ctrl;
	a_uint32_t mode, mode1;

	memset(&port_mux_ctrl, 0, sizeof(port_mux_ctrl));
	ADPT_DEV_ID_CHECK(dev_id);

	rv = hppe_port_mux_ctrl_get(dev_id, &port_mux_ctrl);
	port_mux_ctrl.bf.port4_pcs_sel = 1;

	if (port_id == HPPE_MUX_PORT1)
	{
		if (port_type == PORT_GMAC_TYPE)
		{
			mode = ssdk_dt_global_get_mac_mode(dev_id, SSDK_UNIPHY_INSTANCE0);
			mode1 = ssdk_dt_global_get_mac_mode(dev_id, SSDK_UNIPHY_INSTANCE1);
			if ((((mode == PORT_WRAPPER_PSGMII) ||
			      (mode == PORT_WRAPPER_PSGMII_FIBER)) &&
			     (mode1 == PORT_WRAPPER_MAX)) ||
			    ((mode == PORT_WRAPPER_SGMII4_RGMII4) && (mode1 == PORT_WRAPPER_MAX)))
			{
				port_mux_ctrl.bf.port5_pcs_sel = 1;
				port_mux_ctrl.bf.port5_gmac_sel = 1;
			}
			else if (mode1 == PORT_WRAPPER_SGMII0_RGMII4 ||
					mode1 == PORT_WRAPPER_SGMII_CHANNEL0 ||
					mode1 == PORT_WRAPPER_SGMII_FIBER)
			{
				port_mux_ctrl.bf.port5_pcs_sel = 2;
				port_mux_ctrl.bf.port5_gmac_sel = 1;
			}
		}
		else if (port_type == PORT_XGMAC_TYPE)
		{
			port_mux_ctrl.bf.port5_pcs_sel = 2;
			port_mux_ctrl.bf.port5_gmac_sel = 0;
		}
	}
	else if (port_id == HPPE_MUX_PORT2)
	{
		if (port_type == PORT_GMAC_TYPE)
		{
			port_mux_ctrl.bf.port6_pcs_sel = 1;
			port_mux_ctrl.bf.port6_gmac_sel = 1;
		}
		else if (port_type == PORT_XGMAC_TYPE)
		{
			port_mux_ctrl.bf.port6_pcs_sel = 1;
			port_mux_ctrl.bf.port6_gmac_sel = 0;
		}
	}
	else
	{
		return SW_OK;
	}
	rv = hppe_port_mux_ctrl_set(dev_id, &port_mux_ctrl);

	return rv;
}

static sw_error_t
adpt_hppe_port_flowctrl_forcemode_set(a_uint32_t dev_id,
		fal_port_t port_id, a_bool_t enable)
{
	sw_error_t rv = SW_OK;
	struct qca_phy_priv *priv = ssdk_phy_priv_data_get(dev_id);

	ADPT_DEV_ID_CHECK(dev_id);

	if (!priv)
		return SW_FAIL;

	if ((port_id < SSDK_PHYSICAL_PORT1) || (port_id > SSDK_PHYSICAL_PORT6))
		return SW_BAD_VALUE;

	priv->port_tx_flowctrl_forcemode[port_id - 1] = enable;
	priv->port_rx_flowctrl_forcemode[port_id - 1] = enable;

	return rv;
}
#ifndef IN_PORTCONTROL_MINI
static sw_error_t
adpt_hppe_port_flowctrl_forcemode_get(a_uint32_t dev_id,
		fal_port_t port_id, a_bool_t *enable)
{
	sw_error_t rv = SW_OK;
	struct qca_phy_priv *priv = ssdk_phy_priv_data_get(dev_id);


	ADPT_DEV_ID_CHECK(dev_id);
	ADPT_NULL_POINT_CHECK(enable);

	if ((port_id < SSDK_PHYSICAL_PORT1) || (port_id > SSDK_PHYSICAL_PORT6))
		return SW_BAD_VALUE;

	if (!priv)
		return SW_FAIL;

	*enable = (priv->port_tx_flowctrl_forcemode[port_id - 1] &
		priv->port_rx_flowctrl_forcemode[port_id - 1]);

	return rv;
}
#endif
static a_uint32_t port_lpi_sleep_timer[SW_MAX_NR_DEV][SSDK_PHYSICAL_PORT6] = {
	{218, 218, 218, 218, 218, 218},
	{218, 218, 218, 218, 218, 218},
	{218, 218, 218, 218, 218, 218},
}; /* unit is us*/
static a_uint32_t port_lpi_wakeup_timer[SW_MAX_NR_DEV][SSDK_PHYSICAL_PORT6] = {
	{27, 27, 27, 27, 27, 27},
	{27, 27, 27, 27, 27, 27},
	{27, 27, 27, 27, 27, 27},
}; /* unit is us*/
static sw_error_t
adpt_hppe_port_interface_eee_cfg_set(a_uint32_t dev_id, fal_port_t port_id,
	fal_port_eee_cfg_t *port_eee_cfg)
{
	sw_error_t rv = 0;
	union lpi_enable_u lpi_enable = {0};
	union lpi_port_timer_u lpi_port_timer = {0};
	a_uint32_t phy_id = 0;
	a_uint32_t adv, enable;
	hsl_phy_ops_t *phy_drv;

	ADPT_DEV_ID_CHECK(dev_id);

	if ((port_id < SSDK_PHYSICAL_PORT1) || (port_id > SSDK_PHYSICAL_PORT6)) {
		return SW_BAD_PARAM;
	}
	if (port_eee_cfg->enable) {
		adv = port_eee_cfg->advertisement;
	} else {
		adv = 0;
	}
	if (A_TRUE != hsl_port_prop_check(dev_id, port_id, HSL_PP_PHY)) {
		return SW_BAD_PARAM;
	}
	if (A_FALSE == _adpt_hppe_port_phy_connected (dev_id, port_id)) {
		return SW_NOT_SUPPORTED;
	}

	SW_RTN_ON_NULL(phy_drv = hsl_phy_api_ops_get(dev_id, port_id));
	if (NULL == phy_drv->phy_eee_adv_set) {
		return SW_NOT_SUPPORTED;
	}

	rv = hsl_port_prop_get_phyid(dev_id, port_id, &phy_id);
	SW_RTN_ON_ERROR (rv);

	rv = phy_drv->phy_eee_adv_set(dev_id, phy_id, adv);
	SW_RTN_ON_ERROR (rv);

	hppe_lpi_enable_get(dev_id, port_id, &lpi_enable);

	enable = port_eee_cfg->lpi_tx_enable;
	lpi_enable.val &= ~(0x1 << (port_id - 1));
	lpi_enable.val |= (enable << (port_id - 1));
	hppe_lpi_enable_set(dev_id, port_id, &lpi_enable);

	lpi_port_timer.bf.lpi_port_wakeup_timer =
		(port_eee_cfg->lpi_wakeup_timer * LPI_EEE_TIMER_FREQUENCY) /LPI_EEE_TIMER_UNIT;
	lpi_port_timer.bf.lpi_port_sleep_timer =
		(port_eee_cfg->lpi_sleep_timer * LPI_EEE_TIMER_FREQUENCY) /LPI_EEE_TIMER_UNIT;
	rv = hppe_lpi_timer_set(dev_id, port_id, &lpi_port_timer);
	SW_RTN_ON_ERROR (rv);
	port_lpi_wakeup_timer[dev_id][port_id - 1] = port_eee_cfg->lpi_wakeup_timer;
	port_lpi_sleep_timer[dev_id][port_id - 1] = port_eee_cfg->lpi_sleep_timer;

	return rv;
}
static sw_error_t
adpt_hppe_port_interface_eee_cfg_get(a_uint32_t dev_id, fal_port_t port_id,
	fal_port_eee_cfg_t *port_eee_cfg)
{
	sw_error_t rv = 0;
	union lpi_enable_u lpi_enable = {0};
	a_uint32_t phy_id = 0;
	hsl_phy_ops_t *phy_drv;
	a_uint32_t adv, lp_adv, cap, status;

	if ((port_id < SSDK_PHYSICAL_PORT1) || (port_id > SSDK_PHYSICAL_PORT6)) {
		return SW_BAD_PARAM;
	}

	ADPT_DEV_ID_CHECK(dev_id);
	ADPT_NULL_POINT_CHECK(port_eee_cfg);
	memset(port_eee_cfg, 0, sizeof(*port_eee_cfg));

	if (A_TRUE != hsl_port_prop_check(dev_id, port_id, HSL_PP_PHY)) {
		return SW_BAD_PARAM;
	}
	if (A_FALSE == _adpt_hppe_port_phy_connected (dev_id, port_id)) {
		return SW_NOT_SUPPORTED;
	}

	SW_RTN_ON_NULL (phy_drv =hsl_phy_api_ops_get (dev_id, port_id));
	if ((NULL == phy_drv->phy_eee_adv_get) || (NULL == phy_drv->phy_eee_partner_adv_get) ||
		(NULL == phy_drv->phy_eee_cap_get) || (NULL == phy_drv->phy_eee_status_get)) {
		return SW_NOT_SUPPORTED;
	}

	rv = hsl_port_prop_get_phyid(dev_id, port_id, &phy_id);
	SW_RTN_ON_ERROR (rv);

	rv = phy_drv->phy_eee_adv_get(dev_id, phy_id, &adv);
	SW_RTN_ON_ERROR (rv);
	port_eee_cfg->advertisement = adv;
	rv = phy_drv->phy_eee_partner_adv_get(dev_id, phy_id, &lp_adv);
	SW_RTN_ON_ERROR (rv);
	port_eee_cfg->link_partner_advertisement = lp_adv;
	rv = phy_drv->phy_eee_cap_get(dev_id, phy_id, &cap);
	SW_RTN_ON_ERROR (rv);
	port_eee_cfg->capability = cap;
	rv = phy_drv->phy_eee_status_get(dev_id, phy_id, &status);
	SW_RTN_ON_ERROR (rv);
	port_eee_cfg->eee_status = status;

	if (port_eee_cfg->advertisement) {
		port_eee_cfg->enable = A_TRUE;
	} else {
		port_eee_cfg->enable = A_FALSE;
	}
	rv = hppe_lpi_enable_get(dev_id, port_id, &lpi_enable);
	SW_RTN_ON_ERROR (rv);

	if(((lpi_enable.val >> (port_id - 1)) & 0x1) == A_TRUE) {
		port_eee_cfg->lpi_tx_enable = A_TRUE;
	} else {
		port_eee_cfg->lpi_tx_enable = A_FALSE;
	}
	port_eee_cfg->lpi_wakeup_timer =  port_lpi_wakeup_timer[dev_id][port_id - 1];
	port_eee_cfg->lpi_sleep_timer = port_lpi_sleep_timer[dev_id][port_id - 1];

	return rv;
}

static sw_error_t
adpt_hppe_port_phy_status_get(a_uint32_t dev_id, a_uint32_t port_id,
				struct port_phy_status *phy_status)
{
	sw_error_t rv = 0;
	a_uint32_t phy_id;
	hsl_phy_ops_t *phy_drv;

	ADPT_DEV_ID_CHECK(dev_id);
	ADPT_NULL_POINT_CHECK(phy_status);

	/* for those ports without PHY device should be sfp port or a internal port*/
	if (A_FALSE == _adpt_hppe_port_phy_connected (dev_id, port_id)) {
		if (port_id != SSDK_PHYSICAL_PORT0) {
			rv = _adpt_phy_status_get_from_ppe(dev_id,
				port_id, phy_status);
			SW_RTN_ON_ERROR (rv);
		} else {
			return SW_NOT_SUPPORTED;
		}
	} else {
		SW_RTN_ON_NULL (phy_drv = hsl_phy_api_ops_get (dev_id, port_id));
		if (NULL == phy_drv->phy_get_status)
			return SW_NOT_SUPPORTED;

		rv = hsl_port_prop_get_phyid (dev_id, port_id, &phy_id);
		SW_RTN_ON_ERROR (rv);
		rv = phy_drv->phy_get_status (dev_id, phy_id, phy_status);
		SW_RTN_ON_ERROR (rv);
	}

	return rv;

}
static void
adpt_hppe_uniphy_psgmii_port_reset(a_uint32_t dev_id, a_uint32_t uniphy_index,
			a_uint32_t port_id)
{
	union uniphy_channel0_input_output_4_u uniphy_channel0_input_output_4;
	union uniphy_channel1_input_output_4_u uniphy_channel1_input_output_4;
	union uniphy_channel2_input_output_4_u uniphy_channel2_input_output_4;
	union uniphy_channel3_input_output_4_u uniphy_channel3_input_output_4;
	union uniphy_channel4_input_output_4_u uniphy_channel4_input_output_4;

	memset(&uniphy_channel0_input_output_4, 0, sizeof(uniphy_channel0_input_output_4));
	memset(&uniphy_channel1_input_output_4, 0, sizeof(uniphy_channel1_input_output_4));
	memset(&uniphy_channel2_input_output_4, 0, sizeof(uniphy_channel2_input_output_4));
	memset(&uniphy_channel3_input_output_4, 0, sizeof(uniphy_channel3_input_output_4));
	memset(&uniphy_channel4_input_output_4, 0, sizeof(uniphy_channel4_input_output_4));

	if (port_id == SSDK_PHYSICAL_PORT1)
	{
		hppe_uniphy_channel0_input_output_4_get(dev_id, uniphy_index,
			&uniphy_channel0_input_output_4);
		uniphy_channel0_input_output_4.bf.newaddedfromhere_ch0_adp_sw_rstn = 0;
		hppe_uniphy_channel0_input_output_4_set(dev_id, uniphy_index,
			&uniphy_channel0_input_output_4);
		uniphy_channel0_input_output_4.bf.newaddedfromhere_ch0_adp_sw_rstn = 1;
		hppe_uniphy_channel0_input_output_4_set(dev_id, uniphy_index,
			&uniphy_channel0_input_output_4);
	}
	else if (port_id == SSDK_PHYSICAL_PORT2)
	{
		hppe_uniphy_channel1_input_output_4_get(dev_id, uniphy_index,
			&uniphy_channel1_input_output_4);
		uniphy_channel1_input_output_4.bf.newaddedfromhere_ch1_adp_sw_rstn = 0;
		hppe_uniphy_channel1_input_output_4_set(dev_id, uniphy_index,
			&uniphy_channel1_input_output_4);
		uniphy_channel1_input_output_4.bf.newaddedfromhere_ch1_adp_sw_rstn = 1;
		hppe_uniphy_channel1_input_output_4_set(dev_id, uniphy_index,
			&uniphy_channel1_input_output_4);
	}
	else if (port_id == SSDK_PHYSICAL_PORT3)
	{
		hppe_uniphy_channel2_input_output_4_get(dev_id, uniphy_index,
			&uniphy_channel2_input_output_4);
		uniphy_channel2_input_output_4.bf.newaddedfromhere_ch2_adp_sw_rstn = 0;
		hppe_uniphy_channel2_input_output_4_set(dev_id, uniphy_index,
			&uniphy_channel2_input_output_4);
		uniphy_channel2_input_output_4.bf.newaddedfromhere_ch2_adp_sw_rstn = 1;
		hppe_uniphy_channel2_input_output_4_set(dev_id, uniphy_index,
			&uniphy_channel2_input_output_4);
	}
	else if (port_id == SSDK_PHYSICAL_PORT4)
	{
		hppe_uniphy_channel3_input_output_4_get(dev_id, uniphy_index,
			&uniphy_channel3_input_output_4);
		uniphy_channel3_input_output_4.bf.newaddedfromhere_ch3_adp_sw_rstn = 0;
		hppe_uniphy_channel3_input_output_4_set(dev_id, uniphy_index,
			&uniphy_channel3_input_output_4);
		uniphy_channel3_input_output_4.bf.newaddedfromhere_ch3_adp_sw_rstn = 1;
		hppe_uniphy_channel3_input_output_4_set(dev_id, uniphy_index,
			&uniphy_channel3_input_output_4);
	}
	else if (port_id == SSDK_PHYSICAL_PORT5)
	{
		hppe_uniphy_channel4_input_output_4_get(dev_id, uniphy_index,
			&uniphy_channel4_input_output_4);
		uniphy_channel4_input_output_4.bf.newaddedfromhere_ch4_adp_sw_rstn = 0;
		hppe_uniphy_channel4_input_output_4_set(dev_id, uniphy_index,
			&uniphy_channel4_input_output_4);
		uniphy_channel4_input_output_4.bf.newaddedfromhere_ch4_adp_sw_rstn = 1;
		hppe_uniphy_channel4_input_output_4_set(dev_id, uniphy_index,
			&uniphy_channel4_input_output_4);
	}

	return;
}
static void
adpt_hppe_uniphy_usxgmii_port_reset(a_uint32_t dev_id, a_uint32_t uniphy_index,
			a_uint32_t port_id)
{
	union vr_xs_pcs_dig_ctrl1_u vr_xs_pcs_dig_ctrl1;

	memset(&vr_xs_pcs_dig_ctrl1, 0, sizeof(vr_xs_pcs_dig_ctrl1));

	hppe_vr_xs_pcs_dig_ctrl1_get(dev_id, uniphy_index, &vr_xs_pcs_dig_ctrl1);
	vr_xs_pcs_dig_ctrl1.bf.usra_rst = 1;
	hppe_vr_xs_pcs_dig_ctrl1_set(dev_id, uniphy_index, &vr_xs_pcs_dig_ctrl1);

	return;
}

static void
adpt_hppe_uniphy_port_adapter_reset(a_uint32_t dev_id, a_uint32_t port_id)
{
	a_uint32_t uniphy_index, mode, mode1;
#if defined(CPPE)
	a_uint32_t channel_id = 0;
#endif

	if (port_id < HPPE_MUX_PORT1)
	{
		uniphy_index = SSDK_UNIPHY_INSTANCE0;
#if defined(CPPE)
		adpt_cppe_port_to_channel_convert(dev_id, port_id,
				&channel_id);
		port_id = channel_id;
#endif
		adpt_hppe_uniphy_psgmii_port_reset(dev_id, uniphy_index,
						port_id);
	}
	else
	{
		if (port_id == HPPE_MUX_PORT1)
		{
			mode = ssdk_dt_global_get_mac_mode(dev_id, SSDK_UNIPHY_INSTANCE0);
			mode1 = ssdk_dt_global_get_mac_mode(dev_id, SSDK_UNIPHY_INSTANCE1);
			if ((((mode == PORT_WRAPPER_PSGMII) ||
			      (mode == PORT_WRAPPER_PSGMII_FIBER)) &&
			     (mode1 == PORT_WRAPPER_MAX)) ||
			    ((mode == PORT_WRAPPER_SGMII4_RGMII4) && (mode1 == PORT_WRAPPER_MAX)))
			{
				uniphy_index = SSDK_UNIPHY_INSTANCE0;
				adpt_hppe_uniphy_psgmii_port_reset(dev_id, uniphy_index,
						port_id);
				return;
			}
			else
				uniphy_index = SSDK_UNIPHY_INSTANCE1;
		}
		else
			uniphy_index = SSDK_UNIPHY_INSTANCE2;

		mode = ssdk_dt_global_get_mac_mode(dev_id, uniphy_index);

		if ((mode == PORT_WRAPPER_SGMII_PLUS) || (mode == PORT_WRAPPER_SGMII0_RGMII4)
				|| (mode == PORT_WRAPPER_SGMII_CHANNEL0)
				|| (mode == PORT_WRAPPER_SGMII_FIBER))
		{
			/* only reset channel 0 */
			adpt_hppe_uniphy_psgmii_port_reset(dev_id, uniphy_index, 1);
		}
		else if (mode == PORT_WRAPPER_USXGMII)
		{
			adpt_hppe_uniphy_usxgmii_port_reset(dev_id, uniphy_index,
						port_id);
		}
	}

	return;
}
static void
adpt_hppe_uniphy_usxgmii_speed_set(a_uint32_t dev_id, a_uint32_t uniphy_index,
				fal_port_speed_t speed)
{
	union sr_mii_ctrl_u sr_mii_ctrl;
	memset(&sr_mii_ctrl, 0, sizeof(sr_mii_ctrl));

	hppe_sr_mii_ctrl_get(0, uniphy_index, &sr_mii_ctrl);
	sr_mii_ctrl.bf.duplex_mode = 1;
	if (speed == FAL_SPEED_10)
	{
		sr_mii_ctrl.bf.ss5 = 0;
		sr_mii_ctrl.bf.ss6 = 0;
		sr_mii_ctrl.bf.ss13 = 0;
	}
	else if (speed == FAL_SPEED_100)
	{
		sr_mii_ctrl.bf.ss5 = 0;
		sr_mii_ctrl.bf.ss6 = 0;
		sr_mii_ctrl.bf.ss13 = 1;
	}
	else if (speed == FAL_SPEED_1000)
	{
		sr_mii_ctrl.bf.ss5 = 0;
		sr_mii_ctrl.bf.ss6 = 1;
		sr_mii_ctrl.bf.ss13 = 0;
	}
	else if (speed == FAL_SPEED_10000)
	{
		sr_mii_ctrl.bf.ss5 = 0;
		sr_mii_ctrl.bf.ss6 = 1;
		sr_mii_ctrl.bf.ss13 = 1;
	}
	else if (speed == FAL_SPEED_2500)
	{
		sr_mii_ctrl.bf.ss5 = 1;
		sr_mii_ctrl.bf.ss6 = 0;
		sr_mii_ctrl.bf.ss13 = 0;
	}
	else if (speed == FAL_SPEED_5000)
	{
		sr_mii_ctrl.bf.ss5 = 1;
		sr_mii_ctrl.bf.ss6 = 0;
		sr_mii_ctrl.bf.ss13 = 1;
	}
	hppe_sr_mii_ctrl_set(0, uniphy_index, &sr_mii_ctrl);

	return;
}
static void
adpt_hppe_uniphy_usxgmii_duplex_set(a_uint32_t dev_id, a_uint32_t uniphy_index,
				fal_port_duplex_t duplex)
{
	union sr_mii_ctrl_u sr_mii_ctrl;
	memset(&sr_mii_ctrl, 0, sizeof(sr_mii_ctrl));

	hppe_sr_mii_ctrl_get(dev_id, uniphy_index, &sr_mii_ctrl);

	if (duplex == FAL_FULL_DUPLEX)
		sr_mii_ctrl.bf.duplex_mode = 1;
	else
		sr_mii_ctrl.bf.duplex_mode = 0;

	hppe_sr_mii_ctrl_set(dev_id, uniphy_index, &sr_mii_ctrl);

	return;
}
sw_error_t
adpt_hppe_uniphy_usxgmii_autoneg_completed(a_uint32_t dev_id,
				a_uint32_t uniphy_index)
{
	a_uint32_t autoneg_complete = 0, retries = 100;
	union vr_mii_an_intr_sts_u vr_mii_an_intr_sts;

	memset(&vr_mii_an_intr_sts, 0, sizeof(vr_mii_an_intr_sts));

	// swith uniphy xpcs auto-neg complete and clear interrupt
	while (autoneg_complete != 0x1) {
		mdelay(1);
		if (retries-- == 0)
		{
			printk("uniphy autoneg time out!\n");
			return SW_TIMEOUT;
		}
		hppe_vr_mii_an_intr_sts_get(dev_id, uniphy_index, &vr_mii_an_intr_sts);
		autoneg_complete = vr_mii_an_intr_sts.bf.cl37_ancmplt_intr;
	}

	vr_mii_an_intr_sts.bf.cl37_ancmplt_intr = 0;
	hppe_vr_mii_an_intr_sts_set(dev_id, uniphy_index, &vr_mii_an_intr_sts);

	return SW_OK;
}
static void
adpt_hppe_uniphy_speed_set(a_uint32_t dev_id, a_uint32_t port_id, fal_port_speed_t speed)
{
	a_uint32_t uniphy_index = 0, mode = 0;

	if (port_id == HPPE_MUX_PORT1)
		uniphy_index = SSDK_UNIPHY_INSTANCE1;
	else if (port_id == HPPE_MUX_PORT2)
		uniphy_index = SSDK_UNIPHY_INSTANCE2;

	mode = ssdk_dt_global_get_mac_mode(dev_id, uniphy_index);
	if (mode == PORT_WRAPPER_USXGMII)
	{
		/* adpt_hppe_uniphy_usxgmii_autoneg_completed(dev_id,uniphy_index); */
		/* configure xpcs speed at usxgmii mode */
		adpt_hppe_uniphy_usxgmii_speed_set(dev_id, uniphy_index, speed);
	}

	return;
}
static void
adpt_hppe_uniphy_duplex_set(a_uint32_t dev_id, a_uint32_t port_id, fal_port_duplex_t duplex)
{
	a_uint32_t uniphy_index = 0, mode = 0;

	if (port_id == HPPE_MUX_PORT1)
		uniphy_index = SSDK_UNIPHY_INSTANCE1;
	else if (port_id == HPPE_MUX_PORT2)
		uniphy_index = SSDK_UNIPHY_INSTANCE2;

	mode = ssdk_dt_global_get_mac_mode(dev_id, uniphy_index);
	if (mode == PORT_WRAPPER_USXGMII)
	{
		/* adpt_hppe_uniphy_usxgmii_autoneg_completed(0,uniphy_index); */
		/* configure xpcs duplex at usxgmii mode */
		adpt_hppe_uniphy_usxgmii_duplex_set(dev_id, uniphy_index, duplex);
	}

	return;
}
static void
adpt_hppe_uniphy_autoneg_status_check(a_uint32_t dev_id, a_uint32_t port_id)
{
	a_uint32_t uniphy_index = 0, mode = 0;

	if (port_id == HPPE_MUX_PORT1)
		uniphy_index = SSDK_UNIPHY_INSTANCE1;
	else if (port_id == HPPE_MUX_PORT2)
		uniphy_index = SSDK_UNIPHY_INSTANCE2;

	mode = ssdk_dt_global_get_mac_mode(dev_id, uniphy_index);
	if (mode == PORT_WRAPPER_USXGMII)
	{
		adpt_hppe_uniphy_usxgmii_autoneg_completed(dev_id,uniphy_index);;
	}
	return;
}

static void
adpt_hppe_sgmii_speed_clock_set(
	a_uint32_t dev_id,
	a_uint32_t port_id,
	fal_port_speed_t phy_speed)
{
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
		default:
			break;
               }
}
static void
adpt_hppe_pqsgmii_speed_clock_set(
	a_uint32_t dev_id,
	a_uint32_t port_id,
	fal_port_speed_t phy_speed)
{
	switch (phy_speed) {
		case FAL_SPEED_10:
			ssdk_port_speed_clock_set(dev_id,
					port_id, PQSGMII_SPEED_10M_CLK);
			break;
		case FAL_SPEED_100:
			ssdk_port_speed_clock_set(dev_id,
					port_id, PQSGMII_SPEED_100M_CLK);
			break;
		case FAL_SPEED_1000:
			ssdk_port_speed_clock_set(dev_id,
					port_id, PQSGMII_SPEED_1000M_CLK);
			break;
		default:
			break;
	}
}

static void
adpt_hppe_usxgmii_speed_clock_set(
	a_uint32_t dev_id,
	a_uint32_t port_id,
	fal_port_speed_t phy_speed)
{
	switch (phy_speed) {
		case FAL_SPEED_10:
			ssdk_port_speed_clock_set(dev_id,
					port_id, USXGMII_SPEED_10M_CLK);
			break;
		case FAL_SPEED_100:
			ssdk_port_speed_clock_set(dev_id,
					port_id, USXGMII_SPEED_100M_CLK);
			break;
		case FAL_SPEED_1000:
			ssdk_port_speed_clock_set(dev_id,
					port_id, USXGMII_SPEED_1000M_CLK);
			break;
		case FAL_SPEED_2500:
			ssdk_port_speed_clock_set(dev_id,
					port_id, USXGMII_SPEED_2500M_CLK);
			break;
		case FAL_SPEED_5000:
			ssdk_port_speed_clock_set(dev_id,
					port_id, USXGMII_SPEED_5000M_CLK);
			break;
		case FAL_SPEED_10000:
			ssdk_port_speed_clock_set(dev_id,
					port_id, USXGMII_SPEED_10000M_CLK);
			break;
		default:
			break;
               }

}

static void
adpt_hppe_sgmiiplus_speed_clock_set(
	a_uint32_t dev_id,
	a_uint32_t port_id,
	fal_port_speed_t phy_speed)
{
               ssdk_port_speed_clock_set(dev_id, port_id, SGMII_PLUS_SPEED_2500M_CLK);
}


void
adpt_hppe_gcc_port_speed_clock_set(a_uint32_t dev_id, a_uint32_t port_id,
				fal_port_speed_t phy_speed)
{
	a_uint32_t  mode = 0, uniphy_index = 0, mode1 = 0;

	if (port_id < HPPE_MUX_PORT1)
	{
#if defined(CPPE)
		if (port_id == SSDK_PHYSICAL_PORT4) {
			mode = ssdk_dt_global_get_mac_mode(dev_id, SSDK_UNIPHY_INSTANCE0);
			if (mode == PORT_WRAPPER_SGMII_PLUS) {
				adpt_hppe_sgmiiplus_speed_clock_set(dev_id, port_id, phy_speed);
				return;
			}
		}
#endif
		adpt_hppe_pqsgmii_speed_clock_set(dev_id, port_id, phy_speed);
	}
	else
	{
		if (port_id == HPPE_MUX_PORT1)
		{
			uniphy_index = SSDK_UNIPHY_INSTANCE0;
			mode = ssdk_dt_global_get_mac_mode(dev_id, uniphy_index);
			mode1 = ssdk_dt_global_get_mac_mode(dev_id, SSDK_UNIPHY_INSTANCE1);
			if ((((mode == PORT_WRAPPER_PSGMII) ||
			      (mode == PORT_WRAPPER_PSGMII_FIBER)) &&
			     (mode1 == PORT_WRAPPER_MAX)) ||
			    ((mode == PORT_WRAPPER_SGMII4_RGMII4 ||
			        mode == PORT_WRAPPER_SGMII_CHANNEL4)
			        && (mode1 == PORT_WRAPPER_MAX)))
			{
				adpt_hppe_pqsgmii_speed_clock_set(dev_id, port_id, phy_speed);
				return;
			}
		}
		if (port_id == HPPE_MUX_PORT1)
			uniphy_index = SSDK_UNIPHY_INSTANCE1;
		else
			uniphy_index = SSDK_UNIPHY_INSTANCE2;

		mode = ssdk_dt_global_get_mac_mode(dev_id, uniphy_index);
		if (mode == PORT_WRAPPER_SGMII0_RGMII4 || mode == PORT_WRAPPER_SGMII_CHANNEL0
			|| mode == PORT_WRAPPER_SGMII_FIBER)
			adpt_hppe_sgmii_speed_clock_set(dev_id, port_id, phy_speed);
		else if (mode == PORT_WRAPPER_SGMII_PLUS)
			adpt_hppe_sgmiiplus_speed_clock_set(dev_id, port_id, phy_speed);
		else if ((mode == PORT_WRAPPER_USXGMII) || (mode == PORT_WRAPPER_10GBASE_R))
			adpt_hppe_usxgmii_speed_clock_set(dev_id,port_id, phy_speed);
	}
	return;
}

void
adpt_hppe_gcc_uniphy_clock_status_set(a_uint32_t dev_id, a_uint32_t port_id,
				a_bool_t enable)
{
	a_uint32_t mode = 0, uniphy_index = 0, mode1 = 0;
#if defined(CPPE)
	a_uint32_t channel_id = 0;
#endif

	if (port_id < HPPE_MUX_PORT1)
	{
		uniphy_index = SSDK_UNIPHY_INSTANCE0;
#if defined(CPPE)
		adpt_cppe_port_to_channel_convert(dev_id, port_id,
				&channel_id);
		port_id = channel_id;
#endif
		qca_gcc_uniphy_port_clock_set(dev_id, uniphy_index,
				port_id, enable);
	}
	else
	{
		if (port_id == HPPE_MUX_PORT1)
		{
			uniphy_index = SSDK_UNIPHY_INSTANCE0;
			mode = ssdk_dt_global_get_mac_mode(dev_id, uniphy_index);
			mode1 = ssdk_dt_global_get_mac_mode(dev_id, SSDK_UNIPHY_INSTANCE1);
			if ((((mode == PORT_WRAPPER_PSGMII) ||
			      (mode == PORT_WRAPPER_PSGMII_FIBER)) &&
			     (mode1 == PORT_WRAPPER_MAX)) ||
			    ((mode == PORT_WRAPPER_SGMII4_RGMII4 ||
			        mode == PORT_WRAPPER_SGMII_CHANNEL4)
			        && (mode1 == PORT_WRAPPER_MAX)))
			{
				qca_gcc_uniphy_port_clock_set(dev_id, uniphy_index,
				port_id, enable);
				return;
			}
		}
		if (port_id == HPPE_MUX_PORT1)
			uniphy_index = SSDK_UNIPHY_INSTANCE1;
		else
			uniphy_index = SSDK_UNIPHY_INSTANCE2;

		qca_gcc_uniphy_port_clock_set(dev_id, uniphy_index,
				1, enable);
	}
	return;
}

static sw_error_t
adpt_hppe_sfp_interface_mode_switch(a_uint32_t dev_id,
	a_uint32_t port_id)
{
	sw_error_t rv = SW_OK;
	fal_port_interface_mode_t port_mode_old = PORT_INTERFACE_MODE_MAX,
		port_mode_new= PORT_INTERFACE_MODE_MAX;

	if (A_FALSE == _adpt_hppe_port_phy_connected(dev_id, port_id))
	{
		rv = adpt_hppe_port_interface_mode_get(dev_id, port_id, &port_mode_old);
		SW_RTN_ON_ERROR(rv);
		rv = adpt_hppe_port_interface_mode_status_get(dev_id, port_id,
			&port_mode_new);
		SW_RTN_ON_ERROR (rv);
		if(port_mode_old != port_mode_new)
		{
			SSDK_DEBUG("Port %d change interface mode to %d from %d\n", port_id,
				port_mode_new, port_mode_old);
			rv = _adpt_hppe_port_interface_mode_set(dev_id, port_id, port_mode_new);
			SW_RTN_ON_ERROR(rv);
			rv = _adpt_hppe_port_interface_mode_apply(dev_id, A_FALSE);
			SW_RTN_ON_ERROR(rv);
		}
	}

	return rv;
}

static sw_error_t
adpt_hppe_port_interface_mode_switch(a_uint32_t dev_id, a_uint32_t port_id)
{
	sw_error_t rv = SW_OK;
	fal_port_interface_mode_t port_mode_old = PORT_INTERFACE_MODE_MAX;
	fal_port_interface_mode_t port_mode_new = PORT_INTERFACE_MODE_MAX;

	rv = adpt_hppe_port_interface_mode_status_get(dev_id,
		port_id, &port_mode_new);
	SW_RTN_ON_ERROR(rv);

	rv = adpt_hppe_port_interface_mode_get(dev_id, port_id, &port_mode_old);
	SW_RTN_ON_ERROR(rv);

	if (port_mode_new != port_mode_old) {
		rv = _adpt_hppe_port_interface_mode_set(dev_id,
			port_id, port_mode_new);
		SW_RTN_ON_ERROR(rv);
		rv = _adpt_hppe_port_interface_mode_apply(dev_id, A_FALSE);
	}
	return rv;
}

void
adpt_hppe_gcc_mac_clock_status_set(a_uint32_t dev_id, a_uint32_t port_id,
				a_bool_t enable)
{

	qca_gcc_mac_port_clock_set(dev_id, port_id, enable);

	return;
}
a_bool_t
adpt_hppe_port_phy_status_change(struct qca_phy_priv *priv, a_uint32_t port_id,
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
qca_hppe_mac_sw_sync_task(struct qca_phy_priv *priv)
{
	a_uint32_t port_id;
	struct port_phy_status phy_status = {0};
	a_bool_t status;
	a_uint32_t portbmp[SW_MAX_NR_DEV] = {0};
	sw_error_t rv = SW_OK;

	portbmp[priv->device_id] = qca_ssdk_port_bmp_get(priv->device_id);

	for (port_id = 1; port_id < SW_MAX_NR_PORT; port_id ++) {

		if(!(portbmp[priv->device_id] & (0x1 << port_id)))
			continue;

		rv = adpt_hppe_port_phy_status_get(priv->device_id,
				port_id, &phy_status);
		if (rv != SW_OK) {
			SSDK_DEBUG("failed to get port %d status return value is %d\n",
					port_id, rv);
			continue;
		}
		rv = adpt_hppe_sfp_interface_mode_switch(priv->device_id, port_id);
		if(rv) {
			SSDK_DEBUG("port %d sfp interface mode change failed\n", port_id);
		}
		SSDK_DEBUG("polling task external phy %d link status is %d and speed is %d\n",
				port_id, phy_status.link_status, phy_status.speed);
		/* link status from up to down */
		if ((phy_status.link_status == PORT_LINK_DOWN) &&
			(priv->port_old_link[port_id - 1] == PORT_LINK_UP))
		{
			SSDK_DEBUG("Port %d change to link down status\n", port_id);
			/* disable ppe port bridge txmac */
			adpt_hppe_port_bridge_txmac_set(priv->device_id, port_id, A_FALSE);
			/* disable rx mac */
			adpt_hppe_port_rxmac_status_set(priv->device_id, port_id, A_FALSE);
			priv->port_old_link[port_id - 1] = phy_status.link_status;
			/* switch interface mode if necessary */
			if (adpt_hppe_port_interface_mode_switch(priv->device_id,
					port_id) == SW_OK) {
				SSDK_DEBUG("Port %d the interface mode switched\n",
						port_id);
			}
#ifdef IN_FDB
			adpt_hppe_fdb_del_by_port(priv->device_id, port_id, !(FAL_FDB_DEL_STATIC));
#endif
			continue;
		}
		/* link status from down to up*/
		if ((phy_status.link_status == PORT_LINK_UP) &&
			(priv->port_old_link[port_id - 1] == PORT_LINK_DOWN))
		{
			SSDK_DEBUG("Port %d change to link up status\n", port_id);
			status = adpt_hppe_port_phy_status_change(priv, port_id, phy_status);
			/*disable tx mac*/
			adpt_hppe_port_txmac_status_set(priv->device_id, port_id, A_FALSE);
			/* switch interface mode if necessary */
			if (adpt_hppe_port_interface_mode_switch(priv->device_id,
					port_id) == SW_OK) {
				SSDK_DEBUG("Port %d the interface mode switched\n",
						port_id);
			}
			/* first check uniphy auto-neg complete interrupt to usxgmii */
			adpt_hppe_uniphy_autoneg_status_check(priv->device_id, port_id);
			if (status == A_TRUE)
			{
				adpt_hppe_gcc_uniphy_clock_status_set(priv->device_id,
						port_id, A_FALSE);
				if ((a_uint32_t)phy_status.speed !=
						priv->port_old_speed[port_id - 1])
				{
					/* configure gcc speed clock according to current speed */
					adpt_hppe_gcc_port_speed_clock_set(priv->device_id, port_id,
							phy_status.speed);

					/* config uniphy speed to usxgmii mode */
					adpt_hppe_uniphy_speed_set(priv->device_id, port_id,
							phy_status.speed);

					/* reset port mac when speed change under usxgmii mode */
					adpt_hppe_port_speed_change_mac_reset(priv->device_id, port_id);

					/* config mac speed */
					adpt_hppe_port_mac_speed_set(priv->device_id, port_id,
							phy_status.speed);
					priv->port_old_speed[port_id - 1] =
						(a_uint32_t)phy_status.speed;

					SSDK_DEBUG("Port %d is link up and speed change to %d\n",
							port_id, priv->port_old_speed[port_id - 1]);
				}
				if ((a_uint32_t)phy_status.duplex !=
						priv->port_old_duplex[port_id - 1])
				{
					adpt_hppe_uniphy_duplex_set(priv->device_id, port_id,
							phy_status.duplex);
					adpt_hppe_port_mac_duplex_set(priv->device_id, port_id,
							phy_status.duplex);
					priv->port_old_duplex[port_id - 1] =
						(a_uint32_t)phy_status.duplex;

					SSDK_DEBUG("Port %d is link up and duplex change to %d\n",
							port_id,
							priv->port_old_duplex[port_id - 1]);
				}
				if (priv->port_tx_flowctrl_forcemode[port_id - 1] != A_TRUE)
				{
					if (phy_status.duplex == FAL_HALF_DUPLEX)
					{
						phy_status.tx_flowctrl = A_TRUE;
					}
					if (phy_status.tx_flowctrl !=
							priv->port_old_tx_flowctrl[port_id - 1])
					{
						adpt_hppe_port_txfc_status_set(priv->device_id,
								port_id, phy_status.tx_flowctrl);
						priv->port_old_tx_flowctrl[port_id - 1] =
							phy_status.tx_flowctrl;

						SSDK_DEBUG("Port %d is link up and tx flowctrl"
							" change to %d\n", port_id,
							priv->port_old_tx_flowctrl[port_id - 1]);
					}
				}
				if (priv->port_rx_flowctrl_forcemode[port_id - 1] != A_TRUE)
				{
					if (phy_status.duplex == FAL_HALF_DUPLEX)
					{
						phy_status.rx_flowctrl = A_TRUE;
					}
					if (phy_status.rx_flowctrl !=
							priv->port_old_rx_flowctrl[port_id - 1])
					{
						adpt_hppe_port_rxfc_status_set(priv->device_id,
								port_id, phy_status.rx_flowctrl);
						priv->port_old_rx_flowctrl[port_id - 1] =
							phy_status.rx_flowctrl;

						SSDK_DEBUG("Port %d is link up and rx flowctrl"
							" change to %d\n", port_id,
							priv->port_old_rx_flowctrl[port_id-1]);
					}
				}
				adpt_hppe_gcc_uniphy_clock_status_set(priv->device_id,
						port_id, A_TRUE);
				adpt_hppe_uniphy_port_adapter_reset(priv->device_id, port_id);
			}
			/* enable mac and ppe txmac*/
			adpt_hppe_port_txmac_status_set(priv->device_id, port_id, A_TRUE);
			adpt_hppe_port_rxmac_status_set(priv->device_id, port_id, A_TRUE);
			adpt_hppe_port_bridge_txmac_set(priv->device_id, port_id, A_TRUE);
			priv->port_old_link[port_id - 1] = phy_status.link_status;
		}
		SSDK_DEBUG("polling task PPE port %d link status is %d and speed is %d\n",
				port_id, priv->port_old_link[port_id - 1],
				priv->port_old_speed[port_id - 1]);
	}
	return 0;
}

void adpt_hppe_port_ctrl_func_bitmap_init(a_uint32_t dev_id)
{
	adpt_api_t *p_adpt_api = NULL;

	p_adpt_api = adpt_api_ptr_get(dev_id);

	if(p_adpt_api == NULL)
		return;

	p_adpt_api->adpt_port_ctrl_func_bitmap[0] = \
		((1 << FUNC_ADPT_PORT_LOCAL_LOOPBACK_GET)|
		(1 << FUNC_ADPT_PORT_AUTONEG_RESTART)|
		(1 << FUNC_ADPT_PORT_DUPLEX_SET)|
		(1 << FUNC_ADPT_PORT_RXMAC_STATUS_GET)|
		(1 << FUNC_ADPT_PORT_CDT)|
		(1 << FUNC_ADPT_PORT_TXMAC_STATUS_SET)|
		(1 << FUNC_ADPT_PORT_COMBO_FIBER_MODE_SET)|
		(1 << FUNC_ADPT_PORT_COMBO_MEDIUM_STATUS_GET)|
		(1 << FUNC_ADPT_PORT_MAGIC_FRAME_MAC_SET)|
		(1 << FUNC_ADPT_PORT_POWERSAVE_SET)|
		(1 << FUNC_ADPT_PORT_HIBERNATE_SET)|
		(1 << FUNC_ADPT_PORT_8023AZ_GET)|
		(1 << FUNC_ADPT_PORT_RXFC_STATUS_GET)|
		(1 << FUNC_ADPT_PORT_TXFC_STATUS_GET)|
		(1 << FUNC_ADPT_PORT_REMOTE_LOOPBACK_SET)|
		(1 << FUNC_ADPT_PORT_FLOWCTRL_SET)|
		(1 << FUNC_ADPT_PORT_MRU_SET)|
		(1 << FUNC_ADPT_PORT_AUTONEG_STATUS_GET)|
		(1 << FUNC_ADPT_PORT_TXMAC_STATUS_GET)|
		(1 << FUNC_ADPT_PORT_MDIX_GET)|
		(1 << FUNC_ADPT_PORTS_LINK_STATUS_GET)|
		(1 << FUNC_ADPT_PORT_MAC_LOOPBACK_SET)|
		(1 << FUNC_ADPT_PORT_PHY_ID_GET)|
		(1 << FUNC_ADPT_PORT_MRU_GET)|
		(1 << FUNC_ADPT_PORT_POWER_ON)|
		(1 << FUNC_ADPT_PORT_SPEED_SET)|
		(1 << FUNC_ADPT_PORT_INTERFACE_MODE_GET)|
		(1 << FUNC_ADPT_PORT_DUPLEX_GET)|
		(1 << FUNC_ADPT_PORT_AUTONEG_ADV_GET)|
		(1 << FUNC_ADPT_PORT_MDIX_STATUS_GET)|
		(1 << FUNC_ADPT_PORT_MTU_SET)|
		(1 << FUNC_ADPT_PORT_LINK_STATUS_GET));

	p_adpt_api->adpt_port_ctrl_func_bitmap[1] = \
		((1 << (FUNC_ADPT_PORT_8023AZ_SET % 32))|
		(1 << (FUNC_ADPT_PORT_POWERSAVE_GET % 32))|
		(1 << (FUNC_ADPT_PORT_COMBO_PREFER_MEDIUM_GET % 32))|
		(1 << (FUNC_ADPT_PORT_COMBO_PREFER_MEDIUM_SET % 32))|
		(1 << (FUNC_ADPT_PORT_POWER_OFF % 32))|
		(1 << (FUNC_ADPT_PORT_TXFC_STATUS_SET % 32))|
		(1 << (FUNC_ADPT_PORT_COUNTER_SET % 32))|
		(1 << (FUNC_ADPT_PORT_COMBO_FIBER_MODE_GET % 32))|
		(1 << (FUNC_ADPT_PORT_LOCAL_LOOPBACK_SET % 32))|
		(1 << (FUNC_ADPT_PORT_WOL_STATUS_SET % 32))|
		(1 << (FUNC_ADPT_PORT_MAGIC_FRAME_MAC_GET % 32))|
		(1 << (FUNC_ADPT_PORT_FLOWCTRL_GET % 32))|
		(1 << (FUNC_ADPT_PORT_RXMAC_STATUS_SET % 32))|
		(1 << (FUNC_ADPT_PORT_COUNTER_GET % 32))|
		(1 << (FUNC_ADPT_PORT_INTERFACE_MODE_SET % 32))|
		(1 << (FUNC_ADPT_PORT_MAC_LOOPBACK_GET % 32))|
		(1 << (FUNC_ADPT_PORT_HIBERNATE_GET % 32))|
		(1 << (FUNC_ADPT_PORT_AUTONEG_ADV_SET % 32))|
		(1 << (FUNC_ADPT_PORT_REMOTE_LOOPBACK_GET % 32))|
		(1 << (FUNC_ADPT_PORT_COUNTER_SHOW % 32))|
		(1 << (FUNC_ADPT_PORT_AUTONEG_ENABLE % 32))|
		(1 << (FUNC_ADPT_PORT_MTU_GET % 32))|
		(1 << (FUNC_ADPT_PORT_INTERFACE_MODE_STATUS_GET % 32))|
		(1 << (FUNC_ADPT_PORT_RESET % 32))|
		(1 << (FUNC_ADPT_PORT_RXFC_STATUS_SET % 32))|
		(1 << (FUNC_ADPT_PORT_SPEED_GET % 32))|
		(1 << (FUNC_ADPT_PORT_MDIX_SET % 32))|
		(1 << (FUNC_ADPT_PORT_WOL_STATUS_GET % 32))|
		(1 << (FUNC_ADPT_PORT_MAX_FRAME_SIZE_SET % 32))|
		(1 << (FUNC_ADPT_PORT_MAX_FRAME_SIZE_GET % 32))|
		(1 << (FUNC_ADPT_PORT_SOURCE_FILTER_GET % 32))|
		(1 << (FUNC_ADPT_PORT_SOURCE_FILTER_SET % 32)));

	p_adpt_api->adpt_port_ctrl_func_bitmap[2] = \
		((1 << (FUNC_ADPT_PORT_INTERFACE_MODE_APPLY% 32))|
		(1 << (FUNC_ADPT_PORT_INTERFACE_3AZ_STATUS_SET% 32))|
		(1 << (FUNC_ADPT_PORT_INTERFACE_3AZ_STATUS_GET% 32))|
		(1 << (FUNC_ADPT_PORT_PROMISC_MODE_SET% 32))|
		(1 << (FUNC_ADPT_PORT_PROMISC_MODE_GET% 32))|
		(1 << (FUNC_ADPT_PORT_FLOWCTRL_FORCEMODE_SET% 32))|
		(1 << (FUNC_ADPT_PORT_FLOWCTRL_FORCEMODE_GET% 32)));

	return;

}

static void adpt_hppe_port_ctrl_func_unregister(a_uint32_t dev_id, adpt_api_t *p_adpt_api)
{
	if(p_adpt_api == NULL)
		return;

	p_adpt_api->adpt_port_local_loopback_get = NULL;
	p_adpt_api->adpt_port_autoneg_restart = NULL;
	p_adpt_api->adpt_port_duplex_set = NULL;
	p_adpt_api->adpt_port_rxmac_status_get = NULL;
	p_adpt_api->adpt_port_cdt = NULL;
	p_adpt_api->adpt_port_txmac_status_set = NULL;
	p_adpt_api->adpt_port_combo_fiber_mode_set = NULL;
	p_adpt_api->adpt_port_combo_medium_status_get = NULL;
	p_adpt_api->adpt_port_magic_frame_mac_set = NULL;
	p_adpt_api->adpt_port_powersave_set = NULL;
	p_adpt_api->adpt_port_hibernate_set = NULL;
	p_adpt_api->adpt_port_8023az_get = NULL;
	p_adpt_api->adpt_port_rxfc_status_get = NULL;
	p_adpt_api->adpt_port_txfc_status_get = NULL;
	p_adpt_api->adpt_port_remote_loopback_set = NULL;
	p_adpt_api->adpt_port_flowctrl_set = NULL;
	p_adpt_api->adpt_port_mru_set = NULL;
	p_adpt_api->adpt_port_autoneg_status_get = NULL;
	p_adpt_api->adpt_port_txmac_status_get = NULL;
	p_adpt_api->adpt_port_mdix_get = NULL;
	p_adpt_api->adpt_ports_link_status_get = NULL;
	p_adpt_api->adpt_port_mac_loopback_set = NULL;
	p_adpt_api->adpt_port_phy_id_get = NULL;
	p_adpt_api->adpt_port_mru_get = NULL;
	p_adpt_api->adpt_port_power_on = NULL;
	p_adpt_api->adpt_port_speed_set = NULL;
	p_adpt_api->adpt_port_interface_mode_get = NULL;
	p_adpt_api->adpt_port_duplex_get = NULL;
	p_adpt_api->adpt_port_autoneg_adv_get = NULL;
	p_adpt_api->adpt_port_mdix_status_get = NULL;
	p_adpt_api->adpt_port_mtu_set = NULL;
	p_adpt_api->adpt_port_link_status_get = NULL;
	p_adpt_api->adpt_port_8023az_set = NULL;
	p_adpt_api->adpt_port_powersave_get = NULL;
	p_adpt_api->adpt_port_combo_prefer_medium_get = NULL;
	p_adpt_api->adpt_port_combo_prefer_medium_set = NULL;
	p_adpt_api->adpt_port_power_off = NULL;
	p_adpt_api->adpt_port_txfc_status_set = NULL;
	p_adpt_api->adpt_port_counter_set = NULL;
	p_adpt_api->adpt_port_combo_fiber_mode_get = NULL;
	p_adpt_api->adpt_port_local_loopback_set = NULL;
	p_adpt_api->adpt_port_wol_status_set = NULL;
	p_adpt_api->adpt_port_magic_frame_mac_get = NULL;
	p_adpt_api->adpt_port_flowctrl_get = NULL;
	p_adpt_api->adpt_port_rxmac_status_set = NULL;
	p_adpt_api->adpt_port_counter_get = NULL;
	p_adpt_api->adpt_port_interface_mode_set = NULL;
	p_adpt_api->adpt_port_interface_mode_apply = NULL;
	p_adpt_api->adpt_port_mac_loopback_get = NULL;
	p_adpt_api->adpt_port_hibernate_get = NULL;
	p_adpt_api->adpt_port_autoneg_adv_set = NULL;
	p_adpt_api->adpt_port_remote_loopback_get = NULL;
	p_adpt_api->adpt_port_counter_show = NULL;
	p_adpt_api->adpt_port_autoneg_enable = NULL;
	p_adpt_api->adpt_port_mtu_get = NULL;
	p_adpt_api->adpt_port_interface_mode_status_get = NULL;
	p_adpt_api->adpt_port_reset = NULL;
	p_adpt_api->adpt_port_rxfc_status_set = NULL;
	p_adpt_api->adpt_port_speed_get = NULL;
	p_adpt_api->adpt_port_mdix_set = NULL;
	p_adpt_api->adpt_port_wol_status_get = NULL;
	p_adpt_api->adpt_port_max_frame_size_set = NULL;
	p_adpt_api->adpt_port_max_frame_size_get = NULL;
	p_adpt_api->adpt_port_source_filter_get = NULL;
	p_adpt_api->adpt_port_source_filter_set = NULL;
	p_adpt_api->adpt_port_interface_3az_status_set = NULL;
	p_adpt_api->adpt_port_interface_3az_status_get = NULL;
	p_adpt_api->adpt_port_promisc_mode_set = NULL;
	p_adpt_api->adpt_port_promisc_mode_get = NULL;
	p_adpt_api->adpt_port_flowctrl_forcemode_set = NULL;
	p_adpt_api->adpt_port_flowctrl_forcemode_get = NULL;

	return;

}

sw_error_t adpt_hppe_port_ctrl_init(a_uint32_t dev_id)
{
	adpt_api_t *p_adpt_api = NULL;

	p_adpt_api = adpt_api_ptr_get(dev_id);

	if(p_adpt_api == NULL)
		return SW_FAIL;

	adpt_hppe_port_ctrl_func_unregister(dev_id, p_adpt_api);
#ifndef IN_PORTCONTROL_MINI
	if(p_adpt_api->adpt_port_ctrl_func_bitmap[0] & (1 << FUNC_ADPT_PORT_LOCAL_LOOPBACK_GET))
	{
		p_adpt_api->adpt_port_local_loopback_get = adpt_hppe_port_local_loopback_get;
	}
#endif
	if(p_adpt_api->adpt_port_ctrl_func_bitmap[0] & (1 << FUNC_ADPT_PORT_AUTONEG_RESTART))
	{
		p_adpt_api->adpt_port_autoneg_restart = adpt_hppe_port_autoneg_restart;
	}
	if(p_adpt_api->adpt_port_ctrl_func_bitmap[0] & (1 << FUNC_ADPT_PORT_DUPLEX_SET))
	{
		p_adpt_api->adpt_port_duplex_set = adpt_hppe_port_duplex_set;
	}
#ifndef IN_PORTCONTROL_MINI
	if(p_adpt_api->adpt_port_ctrl_func_bitmap[0] & (1 << FUNC_ADPT_PORT_RXMAC_STATUS_GET))
	{
		p_adpt_api->adpt_port_rxmac_status_get = adpt_hppe_port_rxmac_status_get;
	}
	if(p_adpt_api->adpt_port_ctrl_func_bitmap[0] & (1 << FUNC_ADPT_PORT_CDT))
	{
		p_adpt_api->adpt_port_cdt = adpt_hppe_port_cdt;
	}
#endif
	if(p_adpt_api->adpt_port_ctrl_func_bitmap[0] & (1 << FUNC_ADPT_PORT_TXMAC_STATUS_SET))
	{
		p_adpt_api->adpt_port_txmac_status_set = adpt_hppe_port_txmac_status_set;
	}
#ifndef IN_PORTCONTROL_MINI
	if(p_adpt_api->adpt_port_ctrl_func_bitmap[0] & (1 << FUNC_ADPT_PORT_COMBO_FIBER_MODE_SET))
	{
		p_adpt_api->adpt_port_combo_fiber_mode_set = adpt_hppe_port_combo_fiber_mode_set;
	}
	if(p_adpt_api->adpt_port_ctrl_func_bitmap[0] &
		(1 << FUNC_ADPT_PORT_COMBO_MEDIUM_STATUS_GET))
	{
		p_adpt_api->adpt_port_combo_medium_status_get =
			adpt_hppe_port_combo_medium_status_get;
	}
	if(p_adpt_api->adpt_port_ctrl_func_bitmap[0] & (1 << FUNC_ADPT_PORT_MAGIC_FRAME_MAC_SET))
	{
		p_adpt_api->adpt_port_magic_frame_mac_set = adpt_hppe_port_magic_frame_mac_set;
	}
	if(p_adpt_api->adpt_port_ctrl_func_bitmap[0] & (1 << FUNC_ADPT_PORT_POWERSAVE_SET))
	{
		p_adpt_api->adpt_port_powersave_set = adpt_hppe_port_powersave_set;
	}
	if(p_adpt_api->adpt_port_ctrl_func_bitmap[0] & (1 << FUNC_ADPT_PORT_HIBERNATE_SET))
	{
		p_adpt_api->adpt_port_hibernate_set = adpt_hppe_port_hibernate_set;
	}
	if(p_adpt_api->adpt_port_ctrl_func_bitmap[0] & (1 << FUNC_ADPT_PORT_8023AZ_GET))
	{
		p_adpt_api->adpt_port_8023az_get = adpt_hppe_port_8023az_get;
	}
#endif
	if(p_adpt_api->adpt_port_ctrl_func_bitmap[0] & (1 << FUNC_ADPT_PORT_RXFC_STATUS_GET))
	{
		p_adpt_api->adpt_port_rxfc_status_get = adpt_hppe_port_rxfc_status_get;
	}
	if(p_adpt_api->adpt_port_ctrl_func_bitmap[0] & (1 << FUNC_ADPT_PORT_TXFC_STATUS_GET))
	{
		p_adpt_api->adpt_port_txfc_status_get = adpt_hppe_port_txfc_status_get;
	}
#ifndef IN_PORTCONTROL_MINI
	if(p_adpt_api->adpt_port_ctrl_func_bitmap[0] & (1 << FUNC_ADPT_PORT_REMOTE_LOOPBACK_SET))
	{
		p_adpt_api->adpt_port_remote_loopback_set = adpt_hppe_port_remote_loopback_set;
	}
#endif
	if(p_adpt_api->adpt_port_ctrl_func_bitmap[0] & (1 << FUNC_ADPT_PORT_FLOWCTRL_SET))
	{
		p_adpt_api->adpt_port_flowctrl_set = adpt_hppe_port_flowctrl_set;
	}
#ifndef IN_PORTCONTROL_MINI
	if(p_adpt_api->adpt_port_ctrl_func_bitmap[0] & (1 << FUNC_ADPT_PORT_MRU_SET))
	{
		p_adpt_api->adpt_port_mru_set = adpt_ppe_port_mru_set;
	}
#endif
	if(p_adpt_api->adpt_port_ctrl_func_bitmap[0] & (1 << FUNC_ADPT_PORT_AUTONEG_STATUS_GET))
	{
		p_adpt_api->adpt_port_autoneg_status_get = adpt_hppe_port_autoneg_status_get;
	}
#ifndef IN_PORTCONTROL_MINI
	if(p_adpt_api->adpt_port_ctrl_func_bitmap[0] & (1 << FUNC_ADPT_PORT_TXMAC_STATUS_GET))
	{
		p_adpt_api->adpt_port_txmac_status_get = adpt_hppe_port_txmac_status_get;
	}
	if(p_adpt_api->adpt_port_ctrl_func_bitmap[0] & (1 << FUNC_ADPT_PORT_MDIX_GET))
	{
		p_adpt_api->adpt_port_mdix_get = adpt_hppe_port_mdix_get;
	}
	if(p_adpt_api->adpt_port_ctrl_func_bitmap[0] & (1 << FUNC_ADPT_PORTS_LINK_STATUS_GET))
	{
		p_adpt_api->adpt_ports_link_status_get = adpt_hppe_ports_link_status_get;
	}
	if(p_adpt_api->adpt_port_ctrl_func_bitmap[0] & (1 << FUNC_ADPT_PORT_MAC_LOOPBACK_SET))
	{
		p_adpt_api->adpt_port_mac_loopback_set = adpt_hppe_port_mac_loopback_set;
	}
	if(p_adpt_api->adpt_port_ctrl_func_bitmap[0] & (1 << FUNC_ADPT_PORT_PHY_ID_GET))
	{
		p_adpt_api->adpt_port_phy_id_get = adpt_hppe_port_phy_id_get;
	}
	if(p_adpt_api->adpt_port_ctrl_func_bitmap[0] & (1 << FUNC_ADPT_PORT_MRU_GET))
	{
		p_adpt_api->adpt_port_mru_get = adpt_ppe_port_mru_get;
	}
#endif
	if(p_adpt_api->adpt_port_ctrl_func_bitmap[0] & (1 << FUNC_ADPT_PORT_POWER_ON))
	{
		p_adpt_api->adpt_port_power_on = adpt_hppe_port_power_on;
	}
	if(p_adpt_api->adpt_port_ctrl_func_bitmap[0] & (1 << FUNC_ADPT_PORT_SPEED_SET))
	{
		p_adpt_api->adpt_port_speed_set = adpt_hppe_port_speed_set;
	}
	if(p_adpt_api->adpt_port_ctrl_func_bitmap[0] & (1 << FUNC_ADPT_PORT_INTERFACE_MODE_GET))
	{
		p_adpt_api->adpt_port_interface_mode_get = adpt_hppe_port_interface_mode_get;
	}
	if(p_adpt_api->adpt_port_ctrl_func_bitmap[0] & (1 << FUNC_ADPT_PORT_DUPLEX_GET))
	{
		p_adpt_api->adpt_port_duplex_get = adpt_hppe_port_duplex_get;
	}
	if(p_adpt_api->adpt_port_ctrl_func_bitmap[0] & (1 << FUNC_ADPT_PORT_AUTONEG_ADV_GET))
	{
		p_adpt_api->adpt_port_autoneg_adv_get = adpt_hppe_port_autoneg_adv_get;
	}
#ifndef IN_PORTCONTROL_MINI
	if(p_adpt_api->adpt_port_ctrl_func_bitmap[0] & (1 << FUNC_ADPT_PORT_MDIX_STATUS_GET))
	{
		p_adpt_api->adpt_port_mdix_status_get = adpt_hppe_port_mdix_status_get;
	}
	if(p_adpt_api->adpt_port_ctrl_func_bitmap[0] & (1 << FUNC_ADPT_PORT_MTU_SET))
	{
		p_adpt_api->adpt_port_mtu_set = adpt_ppe_port_mtu_set;
	}
#endif
	if(p_adpt_api->adpt_port_ctrl_func_bitmap[0] & (1 << FUNC_ADPT_PORT_LINK_STATUS_GET))
	{
		p_adpt_api->adpt_port_link_status_get = adpt_hppe_port_link_status_get;
	}
#ifndef IN_PORTCONTROL_MINI
	if(p_adpt_api->adpt_port_ctrl_func_bitmap[1] & (1 <<  (FUNC_ADPT_PORT_8023AZ_SET % 32)))
	{
		p_adpt_api->adpt_port_8023az_set = adpt_hppe_port_8023az_set;
	}
	if(p_adpt_api->adpt_port_ctrl_func_bitmap[1] & (1 <<  (FUNC_ADPT_PORT_POWERSAVE_GET % 32)))
	{
		p_adpt_api->adpt_port_powersave_get = adpt_hppe_port_powersave_get;
	}
	if(p_adpt_api->adpt_port_ctrl_func_bitmap[1] &
		(1 <<  (FUNC_ADPT_PORT_COMBO_PREFER_MEDIUM_GET % 32)))
	{
		p_adpt_api->adpt_port_combo_prefer_medium_get =
			adpt_hppe_port_combo_prefer_medium_get;
	}
	if(p_adpt_api->adpt_port_ctrl_func_bitmap[1] &
		(1 <<  (FUNC_ADPT_PORT_COMBO_PREFER_MEDIUM_SET % 32)))
	{
		p_adpt_api->adpt_port_combo_prefer_medium_set =
			adpt_hppe_port_combo_prefer_medium_set;
	}
#endif
	if(p_adpt_api->adpt_port_ctrl_func_bitmap[1] & (1 <<  (FUNC_ADPT_PORT_POWER_OFF % 32)))
	{
		p_adpt_api->adpt_port_power_off = adpt_hppe_port_power_off;
	}
	if(p_adpt_api->adpt_port_ctrl_func_bitmap[1] &
		(1 <<  (FUNC_ADPT_PORT_TXFC_STATUS_SET  % 32)))
	{
		p_adpt_api->adpt_port_txfc_status_set = adpt_hppe_port_txfc_status_set;
	}
#ifndef IN_PORTCONTROL_MINI
	if(p_adpt_api->adpt_port_ctrl_func_bitmap[1] & (1 <<  (FUNC_ADPT_PORT_COUNTER_SET % 32)))
	{
		p_adpt_api->adpt_port_counter_set = adpt_hppe_port_counter_set;
	}
	if(p_adpt_api->adpt_port_ctrl_func_bitmap[1] &
		(1 <<  (FUNC_ADPT_PORT_COMBO_FIBER_MODE_GET % 32)))
	{
		p_adpt_api->adpt_port_combo_fiber_mode_get = adpt_hppe_port_combo_fiber_mode_get;
	}
	if(p_adpt_api->adpt_port_ctrl_func_bitmap[1] &
		(1 <<  (FUNC_ADPT_PORT_LOCAL_LOOPBACK_SET % 32)))
	{
		p_adpt_api->adpt_port_local_loopback_set = adpt_hppe_port_local_loopback_set;
	}
	if(p_adpt_api->adpt_port_ctrl_func_bitmap[1] & (1 <<  (FUNC_ADPT_PORT_WOL_STATUS_SET % 32)))
	{
		p_adpt_api->adpt_port_wol_status_set = adpt_hppe_port_wol_status_set;
	}
	if(p_adpt_api->adpt_port_ctrl_func_bitmap[1] &
		(1 <<  (FUNC_ADPT_PORT_MAGIC_FRAME_MAC_GET % 32)))
	{
		p_adpt_api->adpt_port_magic_frame_mac_get = adpt_hppe_port_magic_frame_mac_get;
	}
	if(p_adpt_api->adpt_port_ctrl_func_bitmap[1] & (1 <<  (FUNC_ADPT_PORT_FLOWCTRL_GET % 32)))
	{
		p_adpt_api->adpt_port_flowctrl_get = adpt_hppe_port_flowctrl_get;
	}
#endif
	if(p_adpt_api->adpt_port_ctrl_func_bitmap[1] &
		(1 <<  (FUNC_ADPT_PORT_RXMAC_STATUS_SET % 32)))
	{
		p_adpt_api->adpt_port_rxmac_status_set = adpt_hppe_port_rxmac_status_set;
	}
#ifndef IN_PORTCONTROL_MINI
	if(p_adpt_api->adpt_port_ctrl_func_bitmap[1] & (1 <<  (FUNC_ADPT_PORT_COUNTER_GET % 32)))
	{
		p_adpt_api->adpt_port_counter_get = adpt_hppe_port_counter_get;
	}
#endif
	if(p_adpt_api->adpt_port_ctrl_func_bitmap[1] &
		(1 <<  (FUNC_ADPT_PORT_INTERFACE_MODE_SET % 32)))
	{
		p_adpt_api->adpt_port_interface_mode_set = adpt_hppe_port_interface_mode_set;
	}
#ifndef IN_PORTCONTROL_MINI
	if(p_adpt_api->adpt_port_ctrl_func_bitmap[1] &
		(1 <<  (FUNC_ADPT_PORT_MAC_LOOPBACK_GET % 32)))
	{
		p_adpt_api->adpt_port_mac_loopback_get = adpt_hppe_port_mac_loopback_get;
	}
	if(p_adpt_api->adpt_port_ctrl_func_bitmap[1] & (1 <<  (FUNC_ADPT_PORT_HIBERNATE_GET % 32)))
	{
		p_adpt_api->adpt_port_hibernate_get = adpt_hppe_port_hibernate_get;
	}
#endif
	if(p_adpt_api->adpt_port_ctrl_func_bitmap[1] &
		(1 <<  (FUNC_ADPT_PORT_AUTONEG_ADV_SET % 32)))
	{
		p_adpt_api->adpt_port_autoneg_adv_set = adpt_hppe_port_autoneg_adv_set;
	}
#ifndef IN_PORTCONTROL_MINI
	if(p_adpt_api->adpt_port_ctrl_func_bitmap[1] &
		(1 <<  (FUNC_ADPT_PORT_REMOTE_LOOPBACK_GET % 32)))
	{
		p_adpt_api->adpt_port_remote_loopback_get = adpt_hppe_port_remote_loopback_get;
	}
	if(p_adpt_api->adpt_port_ctrl_func_bitmap[1] & (1 <<  (FUNC_ADPT_PORT_COUNTER_SHOW % 32)))
	{
		p_adpt_api->adpt_port_counter_show = adpt_hppe_port_counter_show;
	}
#endif
	if(p_adpt_api->adpt_port_ctrl_func_bitmap[1] & (1 <<  (FUNC_ADPT_PORT_AUTONEG_ENABLE % 32)))
	{
		p_adpt_api->adpt_port_autoneg_enable = adpt_hppe_port_autoneg_enable;
	}
#ifndef IN_PORTCONTROL_MINI
	if(p_adpt_api->adpt_port_ctrl_func_bitmap[1] & (1 <<  (FUNC_ADPT_PORT_MTU_GET % 32)))
	{
		p_adpt_api->adpt_port_mtu_get = adpt_ppe_port_mtu_get;
	}
#endif
	if(p_adpt_api->adpt_port_ctrl_func_bitmap[1] &
		(1 <<  (FUNC_ADPT_PORT_INTERFACE_MODE_STATUS_GET % 32)))
	{
		p_adpt_api->adpt_port_interface_mode_status_get =
			adpt_hppe_port_interface_mode_status_get;
	}
#ifndef IN_PORTCONTROL_MINI
	if(p_adpt_api->adpt_port_ctrl_func_bitmap[1] & (1 <<  (FUNC_ADPT_PORT_RESET % 32)))
	{
		p_adpt_api->adpt_port_reset = adpt_hppe_port_reset;
	}
#endif
	if(p_adpt_api->adpt_port_ctrl_func_bitmap[1] &
		(1 <<  (FUNC_ADPT_PORT_RXFC_STATUS_SET % 32)))
	{
		p_adpt_api->adpt_port_rxfc_status_set = adpt_hppe_port_rxfc_status_set;
	}
	if(p_adpt_api->adpt_port_ctrl_func_bitmap[1] & (1 <<  (FUNC_ADPT_PORT_SPEED_GET % 32)))
	{
		p_adpt_api->adpt_port_speed_get = adpt_hppe_port_speed_get;
	}
#ifndef IN_PORTCONTROL_MINI
	if(p_adpt_api->adpt_port_ctrl_func_bitmap[1] & (1 <<  (FUNC_ADPT_PORT_MDIX_SET % 32)))
	{
		p_adpt_api->adpt_port_mdix_set = adpt_hppe_port_mdix_set;
	}
	if(p_adpt_api->adpt_port_ctrl_func_bitmap[1] & (1 <<  (FUNC_ADPT_PORT_WOL_STATUS_GET % 32)))
	{
		p_adpt_api->adpt_port_wol_status_get = adpt_hppe_port_wol_status_get;
	}
#endif
	if(p_adpt_api->adpt_port_ctrl_func_bitmap[1] &
		(1 <<  (FUNC_ADPT_PORT_MAX_FRAME_SIZE_SET % 32)))
	{
		p_adpt_api->adpt_port_max_frame_size_set = adpt_ppe_port_max_frame_size_set;
	}
	if(p_adpt_api->adpt_port_ctrl_func_bitmap[1] &
		(1 <<  (FUNC_ADPT_PORT_MAX_FRAME_SIZE_GET % 32)))
	{
		p_adpt_api->adpt_port_max_frame_size_get = adpt_ppe_port_max_frame_size_get;
	}
#ifndef IN_PORTCONTROL_MINI
	if(p_adpt_api->adpt_port_ctrl_func_bitmap[1] &
		(1 <<  (FUNC_ADPT_PORT_SOURCE_FILTER_GET % 32)))
	{
		p_adpt_api->adpt_port_source_filter_get = adpt_ppe_port_source_filter_get;
	}
	if(p_adpt_api->adpt_port_ctrl_func_bitmap[1] &
		(1 <<  (FUNC_ADPT_PORT_SOURCE_FILTER_SET % 32)))
	{
		p_adpt_api->adpt_port_source_filter_set = adpt_ppe_port_source_filter_set;
	}
	if(p_adpt_api->adpt_port_ctrl_func_bitmap[2] &
		(1 <<  (FUNC_ADPT_PORT_INTERFACE_MODE_APPLY% 32)))
	{
		p_adpt_api->adpt_port_interface_mode_apply = adpt_hppe_port_interface_mode_apply;
	}
	if(p_adpt_api->adpt_port_ctrl_func_bitmap[2] &
		(1 <<  (FUNC_ADPT_PORT_INTERFACE_3AZ_STATUS_SET% 32)))
	{
		p_adpt_api->adpt_port_interface_3az_status_set = adpt_hppe_port_interface_3az_set;
	}
	if(p_adpt_api->adpt_port_ctrl_func_bitmap[2] &
		(1 <<  (FUNC_ADPT_PORT_INTERFACE_3AZ_STATUS_GET% 32)))
	{
		p_adpt_api->adpt_port_interface_3az_status_get = adpt_hppe_port_interface_3az_get;
	}
#endif
	if(p_adpt_api->adpt_port_ctrl_func_bitmap[2] &
		(1 <<  (FUNC_ADPT_PORT_PROMISC_MODE_SET% 32)))
	{
		p_adpt_api->adpt_port_promisc_mode_set = adpt_hppe_port_promisc_mode_set;
	}
#ifndef IN_PORTCONTROL_MINI

	if(p_adpt_api->adpt_port_ctrl_func_bitmap[2] &
		(1 <<  (FUNC_ADPT_PORT_PROMISC_MODE_GET% 32)))
	{
		p_adpt_api->adpt_port_promisc_mode_get = adpt_hppe_port_promisc_mode_get;
	}
#endif
	if(p_adpt_api->adpt_port_ctrl_func_bitmap[2] &
		(1 <<  (FUNC_ADPT_PORT_FLOWCTRL_FORCEMODE_SET% 32)))
	{
		p_adpt_api->adpt_port_flowctrl_forcemode_set =
			adpt_hppe_port_flowctrl_forcemode_set;
	}
#ifndef IN_PORTCONTROL_MINI
	if(p_adpt_api->adpt_port_ctrl_func_bitmap[2] &
		(1 <<  (FUNC_ADPT_PORT_FLOWCTRL_FORCEMODE_GET% 32)))
	{
		p_adpt_api->adpt_port_flowctrl_forcemode_get =
			adpt_hppe_port_flowctrl_forcemode_get;
	}
	p_adpt_api->adpt_port_source_filter_config_get = adpt_ppe_port_source_filter_config_get;
	p_adpt_api->adpt_port_source_filter_config_set = adpt_ppe_port_source_filter_config_set;
#endif
	p_adpt_api->adpt_port_mux_mac_type_set = adpt_hppe_port_mux_mac_type_set;
	p_adpt_api->adpt_port_mac_speed_set = adpt_hppe_port_mac_speed_set;
	p_adpt_api->adpt_port_mac_duplex_set = adpt_hppe_port_mac_duplex_set;
	p_adpt_api->adpt_port_polling_sw_sync_set = qca_hppe_mac_sw_sync_task;
	p_adpt_api->adpt_port_bridge_txmac_set = adpt_hppe_port_bridge_txmac_set;
	p_adpt_api->adpt_port_interface_eee_cfg_set = adpt_hppe_port_interface_eee_cfg_set;
	p_adpt_api->adpt_port_interface_eee_cfg_get = adpt_hppe_port_interface_eee_cfg_get;
	p_adpt_api->adpt_port_phy_status_get = adpt_hppe_port_phy_status_get;
#if defined(CPPE)
	if (adpt_hppe_chip_revision_get(dev_id) == CPPE_REVISION) {
		p_adpt_api->adpt_switch_port_loopback_set = adpt_cppe_switch_port_loopback_set;
		p_adpt_api->adpt_switch_port_loopback_get = adpt_cppe_switch_port_loopback_get;
	}
#endif
	return SW_OK;
}

/**
 * @}
 */

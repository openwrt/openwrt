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
#include "hppe_global_reg.h"
#include "hppe_global.h"
#include "hppe_uniphy_reg.h"
#include "hppe_uniphy.h"
#include "hppe_init.h"
#include "ssdk_init.h"
#include "ssdk_clk.h"
#include "adpt_hppe.h"
#include "adpt.h"
#include "hppe_reg_access.h"
#include "hsl_phy.h"
#include "adpt_cppe_portctrl.h"
#include "adpt_cppe_uniphy.h"

static sw_error_t
__adpt_cppe_uniphy_reset(a_uint32_t dev_id, a_uint32_t uniphy_index)
{
	sw_error_t rv = SW_OK;
	union pll_power_on_and_reset_u pll_software_reset;

	memset(&pll_software_reset, 0, sizeof(pll_software_reset));
	ADPT_DEV_ID_CHECK(dev_id);

	rv = hppe_uniphy_pll_reset_ctrl_get(dev_id, uniphy_index,
		&pll_software_reset);
	SW_RTN_ON_ERROR (rv);
	pll_software_reset.bf.software_reset_analog_reset = 0;
	rv = hppe_uniphy_pll_reset_ctrl_set(dev_id, uniphy_index,
		&pll_software_reset);
	SW_RTN_ON_ERROR (rv);
	msleep(500);
	pll_software_reset.bf.software_reset_analog_reset = 1;
	rv = hppe_uniphy_pll_reset_ctrl_set(dev_id, uniphy_index,
		&pll_software_reset);
	SW_RTN_ON_ERROR (rv);
	msleep(500);

	return SW_OK;
}

static sw_error_t
__adpt_cppe_uniphy_port_disable(a_uint32_t dev_id, a_uint32_t uniphy_index,
	a_uint32_t port_id)
{
	enum unphy_rst_type rst_type = 0;

	if (uniphy_index != SSDK_UNIPHY_INSTANCE0) {
		return SW_BAD_VALUE;
	}

	switch (port_id) {
		case SSDK_PHYSICAL_PORT1:
			rst_type = UNIPHY0_PORT1_DISABLE_E;
			break;
		case SSDK_PHYSICAL_PORT2:
			rst_type = UNIPHY0_PORT2_DISABLE_E;
			break;
		case SSDK_PHYSICAL_PORT3:
			rst_type = UNIPHY0_PORT3_DISABLE_E;
			break;
		case SSDK_PHYSICAL_PORT4:
			rst_type = UNIPHY0_PORT4_DISABLE_E;
			break;
		case SSDK_PHYSICAL_PORT5:
			rst_type = UNIPHY0_PORT5_DISABLE_E;
			break;
		default:
			break;
	}
	ssdk_uniphy_reset(dev_id, rst_type, SSDK_RESET_ASSERT);

	return SW_OK;
}

void
__adpt_cppe_gcc_uniphy_software_reset(a_uint32_t dev_id,
	a_uint32_t uniphy_index)
{
	a_uint32_t phy_type = 0;
	enum unphy_rst_type rst_type = 0;

	if (uniphy_index >= SSDK_UNIPHY_INSTANCE2) {
		return;
	}

	if (uniphy_index == SSDK_UNIPHY_INSTANCE0) {
		phy_type = hsl_port_phyid_get(dev_id, SSDK_PHYSICAL_PORT4);
		if (phy_type == MALIBU2PORT_PHY) {
			rst_type = UNIPHY0_PORT_4_5_RESET_E;
		} else if (phy_type == QCA8081_PHY_V1_1) {
			rst_type = UNIPHY0_PORT_4_RESET_E;
		} else {
			rst_type = UNIPHY0_SOFT_RESET_E;
		}
	} else {
		rst_type = UNIPHY1_SOFT_RESET_E;
	}

	ssdk_uniphy_reset(dev_id, rst_type, SSDK_RESET_ASSERT);

	msleep(100);

	ssdk_uniphy_reset(dev_id, rst_type, SSDK_RESET_DEASSERT);

	return;
}

sw_error_t
__adpt_cppe_uniphy_channel_selection_set(a_uint32_t dev_id,
	a_uint32_t ch0_selection, a_uint32_t ch4_selection)
{
	sw_error_t rv = SW_OK;
	union cppe_port_mux_ctrl_u cppe_port_mux_ctrl;

	memset(&cppe_port_mux_ctrl, 0, sizeof(cppe_port_mux_ctrl));
	ADPT_DEV_ID_CHECK(dev_id);

	rv = cppe_port_mux_ctrl_get(dev_id, &cppe_port_mux_ctrl);
	SW_RTN_ON_ERROR (rv);
	cppe_port_mux_ctrl.bf.pcs0_ch0_sel = ch0_selection;
	cppe_port_mux_ctrl.bf.pcs0_ch4_sel = ch4_selection;
	rv = cppe_port_mux_ctrl_set(dev_id, &cppe_port_mux_ctrl);
	SW_RTN_ON_ERROR (rv);

	return rv;
}

static sw_error_t
__adpt_cppe_uniphy_mode_ctrl_set(a_uint32_t dev_id,
	a_uint32_t uniphy_index, a_uint32_t mode)
{
	sw_error_t rv = SW_OK;
	union uniphy_mode_ctrl_u uniphy_mode_ctrl;

	memset(&uniphy_mode_ctrl, 0, sizeof(uniphy_mode_ctrl));

	/* configure uniphy mode ctrl to psgmii/sgmii/sgmiiplus */
	rv = hppe_uniphy_mode_ctrl_get(dev_id, uniphy_index, &uniphy_mode_ctrl);
	SW_RTN_ON_ERROR (rv);
	uniphy_mode_ctrl.bf.newaddedfromhere_ch0_autoneg_mode =
		UNIPHY_ATHEROS_NEGOTIATION;
	if (mode == PORT_WRAPPER_PSGMII) {
		uniphy_mode_ctrl.bf.newaddedfromhere_ch0_psgmii_qsgmii =
			UNIPHY_CH0_PSGMII_MODE;
		uniphy_mode_ctrl.bf.newaddedfromhere_sg_mode =
			UNIPHY_SGMII_MODE_DISABLE;
		uniphy_mode_ctrl.bf.newaddedfromhere_sgplus_mode =
			UNIPHY_SGMIIPLUS_MODE_DISABLE;
	} else if (mode == PORT_WRAPPER_SGMII_CHANNEL0) {
		uniphy_mode_ctrl.bf.newaddedfromhere_ch0_psgmii_qsgmii =
			UNIPHY_CH0_QSGMII_SGMII_MODE;
		uniphy_mode_ctrl.bf.newaddedfromhere_sg_mode =
			UNIPHY_SGMII_MODE_ENABLE;
		uniphy_mode_ctrl.bf.newaddedfromhere_sgplus_mode =
			UNIPHY_SGMIIPLUS_MODE_DISABLE;
	} else {
		uniphy_mode_ctrl.bf.newaddedfromhere_ch0_psgmii_qsgmii =
			UNIPHY_CH0_QSGMII_SGMII_MODE;
		uniphy_mode_ctrl.bf.newaddedfromhere_sg_mode =
			UNIPHY_SGMII_MODE_DISABLE;
		uniphy_mode_ctrl.bf.newaddedfromhere_sgplus_mode =
			UNIPHY_SGMIIPLUS_MODE_ENABLE;
	}
	uniphy_mode_ctrl.bf.newaddedfromhere_ch0_qsgmii_sgmii =
		UNIPHY_CH0_SGMII_MODE;
	uniphy_mode_ctrl.bf.newaddedfromhere_xpcs_mode =
		UNIPHY_XPCS_MODE_DISABLE;
	rv = hppe_uniphy_mode_ctrl_set(dev_id, uniphy_index, &uniphy_mode_ctrl);

	return rv;
}

sw_error_t
__adpt_cppe_uniphy_mode_set(a_uint32_t dev_id,
		a_uint32_t uniphy_index, a_uint32_t mode)
{
	sw_error_t rv = SW_OK;
	a_uint32_t i = 0;
	union uniphy_misc2_phy_mode_u uniphy_misc2_phy_mode;

	memset(&uniphy_misc2_phy_mode, 0, sizeof(uniphy_misc2_phy_mode));

	ADPT_DEV_ID_CHECK(dev_id);

	if (uniphy_index != SSDK_UNIPHY_INSTANCE0) {
		SSDK_ERROR("uniphy index is %d\n", uniphy_index);
		return SW_BAD_VALUE;
	}

	if ((mode == PORT_WRAPPER_SGMII_CHANNEL0) ||
		(mode == PORT_WRAPPER_SGMII_PLUS)) {
		/*set the PHY mode to SGMII or SGMIIPLUS*/
		rv = hppe_uniphy_phy_mode_ctrl_get(dev_id, uniphy_index,
			&uniphy_misc2_phy_mode);
		SW_RTN_ON_ERROR (rv);
		if (mode == PORT_WRAPPER_SGMII_CHANNEL0) {
			uniphy_misc2_phy_mode.bf.phy_mode =
				UNIPHY_PHY_SGMII_MODE;
		} else {
			uniphy_misc2_phy_mode.bf.phy_mode =
				UNIPHY_PHY_SGMIIPLUS_MODE;
		}
		rv = hppe_uniphy_phy_mode_ctrl_set(dev_id, uniphy_index,
			&uniphy_misc2_phy_mode);
		SW_RTN_ON_ERROR (rv);

		/*reset uniphy*/
		rv = __adpt_cppe_uniphy_reset(dev_id, uniphy_index);
		SW_RTN_ON_ERROR (rv);
	}
	/* keep xpcs to reset status */
	__adpt_hppe_gcc_uniphy_xpcs_reset(dev_id, uniphy_index, A_TRUE);

	SW_RTN_ON_ERROR (rv);
	if (mode == PORT_WRAPPER_PSGMII) {
		/* disable GCC_UNIPHY0_MISC port 1, 2 and 3*/
		for (i = SSDK_PHYSICAL_PORT1; i < SSDK_PHYSICAL_PORT4; i++) {
		 	rv = __adpt_cppe_uniphy_port_disable(dev_id, uniphy_index, i);
			SW_RTN_ON_ERROR (rv);
		}

		/* disable instance0 port 4 and 5 clock */
		for (i = SSDK_PHYSICAL_PORT4; i < SSDK_PHYSICAL_PORT6; i++) {
			qca_gcc_uniphy_port_clock_set(dev_id, uniphy_index,
				i, A_FALSE);
		}
		rv = __adpt_cppe_uniphy_channel_selection_set(dev_id,
			CPPE_PCS0_CHANNEL0_SEL_PSGMII,
			CPPE_PCS0_CHANNEL4_SEL_PORT3_CLOCK);
		SW_RTN_ON_ERROR (rv);
	} else {
		/* disable GCC_UNIPHY0_MISC port 2, 3 and 5*/
		for (i = SSDK_PHYSICAL_PORT1; i < SSDK_PHYSICAL_PORT6; i++) {
			if ((i == SSDK_PHYSICAL_PORT1) || (i == SSDK_PHYSICAL_PORT4)) {
				continue;
			}
			rv = __adpt_cppe_uniphy_port_disable(dev_id, uniphy_index, i);
			SW_RTN_ON_ERROR (rv);
		}
		/* disable instance0 port 4 clock */
		qca_gcc_uniphy_port_clock_set(dev_id, uniphy_index,
					SSDK_PHYSICAL_PORT4, A_FALSE);
		rv = __adpt_cppe_uniphy_channel_selection_set(dev_id,
			CPPE_PCS0_CHANNEL0_SEL_SGMIIPLUS,
			CPPE_PCS0_CHANNEL4_SEL_PORT5_CLOCK);
		SW_RTN_ON_ERROR (rv);
	}

	rv = __adpt_cppe_uniphy_mode_ctrl_set(dev_id, uniphy_index, mode);
	SW_RTN_ON_ERROR (rv);

	__adpt_cppe_gcc_uniphy_software_reset(dev_id, uniphy_index);

	/* wait uniphy calibration done */
	rv = __adpt_hppe_uniphy_calibrate(dev_id, uniphy_index);
	SW_RTN_ON_ERROR (rv);

	if (mode == PORT_WRAPPER_PSGMII) {
		rv = hsl_port_phy_serdes_reset(dev_id);
		SW_RTN_ON_ERROR (rv);
		/* enable instance0 clock */
		for (i = SSDK_PHYSICAL_PORT4; i < SSDK_PHYSICAL_PORT6; i++) {
			qca_gcc_uniphy_port_clock_set(dev_id, uniphy_index,
				i, A_TRUE);
		}
	} else {
		/* enable instance clock */
		qca_gcc_uniphy_port_clock_set(dev_id, uniphy_index,
				SSDK_PHYSICAL_PORT4, A_TRUE);
	}
	if (mode == PORT_WRAPPER_PSGMII) {
		SSDK_DEBUG("cypress uniphy %d psgmii configuration is done!\n", uniphy_index);
	} else if (mode == PORT_WRAPPER_SGMII_CHANNEL0) {
		SSDK_DEBUG("cypress uniphy %d sgmii configuration is done!\n", uniphy_index);
	} else {
		SSDK_DEBUG("cypress uniphy %d sgmiiplus configuration is done!\n", uniphy_index);
	}

	return rv;
}

/**
 * @}
 */

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

/**
 * @defgroup
 * @{
 */
#include "sw.h"
#include "hppe_uniphy_reg.h"
#include "hppe_uniphy.h"
#include "hppe_init.h"
#include "ssdk_init.h"
#include "ssdk_clk.h"
#include "ssdk_dts.h"
#include "adpt.h"
#include "mp_uniphy_reg.h"
#include "mp_uniphy.h"
#include "hsl_phy.h"

static sw_error_t
_adpt_mp_uniphy_calibrate(a_uint32_t dev_id, a_uint32_t uniphy_index)
{
	a_uint32_t reg_value = 0;
	a_uint32_t retries = 100, calibration_done = 0;
	union uniphy_offset_calib_4_u uniphy_offset_calib_4;

	memset(&uniphy_offset_calib_4, 0, sizeof(uniphy_offset_calib_4));
	ADPT_DEV_ID_CHECK(dev_id);

	if(ssdk_is_emulation(dev_id)){
		SSDK_INFO("uniphy_index %d on emulation platform\n", uniphy_index);
		return SW_OK;
	}
	/*wait calibration done to uniphy*/
	while (calibration_done != UNIPHY_CALIBRATION_DONE) {
		mdelay(1);
		if (retries-- == 0)
		{
			SSDK_ERROR("uniphy callibration time out!\n");
			return SW_TIMEOUT;
		}
		reg_value = 0;
		hppe_uniphy_offset_calib_4_get(dev_id, uniphy_index, &uniphy_offset_calib_4);
		reg_value = uniphy_offset_calib_4.bf.mmd1_reg_calibration_done_reg;

		calibration_done = (reg_value & UNIPHY_CALIBRATION_DONE);
	}

	return SW_OK;
}

sw_error_t
adpt_mp_uniphy_adapter_port_reset(a_uint32_t dev_id,
	a_uint32_t port_id)
{
	sw_error_t rv = SW_OK;
	a_uint32_t uniphy_index = 0;
	union uniphy_channel0_input_output_4_u uniphy_channel0_input_output_4;

	memset(&uniphy_channel0_input_output_4, 0, sizeof(uniphy_channel0_input_output_4));

	if (port_id == SSDK_PHYSICAL_PORT2) {
		uniphy_index = SSDK_UNIPHY_INSTANCE0;
	} else {
		SSDK_ERROR("uniphy adapter reset port_id is %d\n", port_id);
		return SW_BAD_VALUE;
	}

	rv = hppe_uniphy_channel0_input_output_4_get(dev_id, uniphy_index,
		&uniphy_channel0_input_output_4);
	SW_RTN_ON_ERROR (rv);
	uniphy_channel0_input_output_4.bf.newaddedfromhere_ch0_adp_sw_rstn = 0;
	rv = hppe_uniphy_channel0_input_output_4_set(dev_id, uniphy_index,
		&uniphy_channel0_input_output_4);
	SW_RTN_ON_ERROR (rv);
	uniphy_channel0_input_output_4.bf.newaddedfromhere_ch0_adp_sw_rstn = 1;
	rv = hppe_uniphy_channel0_input_output_4_set(dev_id, uniphy_index,
		&uniphy_channel0_input_output_4);
	SW_RTN_ON_ERROR (rv);

	return rv;
}

sw_error_t
adpt_mp_gcc_uniphy_port_clock_set(a_uint32_t dev_id, a_uint32_t port_id,
	a_bool_t enable)
{
	sw_error_t rv = SW_OK;

	if (port_id == SSDK_PHYSICAL_PORT1) {
		qca_gcc_uniphy_port_clock_set(dev_id, SSDK_UNIPHY_INSTANCE0,
				port_id, enable);
	} else if (port_id == SSDK_PHYSICAL_PORT2) {
		qca_gcc_uniphy_port_clock_set(dev_id, SSDK_UNIPHY_INSTANCE1,
				port_id, enable);
	} else {
		return SW_BAD_VALUE;
	}

	return rv;
}

void
adpt_mp_gcc_uniphy_port_set(a_uint32_t dev_id, a_uint32_t port_id,
	a_bool_t enable)
{
	enum unphy_rst_type rst_type;

	if (port_id == SSDK_PHYSICAL_PORT2) {
		rst_type = UNIPHY1_SOFT_RESET_E;
	} else {
		return;
	}

	if (enable == A_TRUE) {
		ssdk_uniphy_reset(dev_id, rst_type, SSDK_RESET_DEASSERT);
	} else {
		ssdk_uniphy_reset(dev_id, rst_type, SSDK_RESET_ASSERT);
	}

	return;
}

void
adpt_mp_gcc_uniphy_port_reset(a_uint32_t dev_id, a_uint32_t port_id)
{
	adpt_mp_gcc_uniphy_port_set(dev_id, port_id, A_FALSE);

	msleep(100);

	adpt_mp_gcc_uniphy_port_set(dev_id, port_id, A_TRUE);

	return;
}

static sw_error_t
adpt_mp_uniphy_reset(a_uint32_t dev_id, a_uint32_t uniphy_index)
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
adpt_mp_uniphy_mode_ctrl_set(a_uint32_t dev_id,
	a_uint32_t uniphy_index, a_uint32_t mode)
{
	sw_error_t rv = SW_OK;
	union uniphy_mode_ctrl_u uniphy_mode_ctrl;
	union uniphy_channel0_input_output_4_u uniphy_force_ctrl;
	a_bool_t force_port = 0;

	memset(&uniphy_mode_ctrl, 0, sizeof(uniphy_mode_ctrl));
	memset(&uniphy_force_ctrl, 0, sizeof(uniphy_force_ctrl));

	force_port = ssdk_port_feature_get(dev_id, SSDK_PHYSICAL_PORT2,
				PHY_F_FORCE);
	/* configure uniphy mode ctrl to sgmii/sgmiiplus */
	rv = hppe_uniphy_mode_ctrl_get(dev_id, uniphy_index, &uniphy_mode_ctrl);
	SW_RTN_ON_ERROR (rv);
	if (mode == PORT_WRAPPER_SGMII_CHANNEL0) {
		uniphy_mode_ctrl.bf.newaddedfromhere_sg_mode =
			UNIPHY_SGMII_MODE_ENABLE;
		uniphy_mode_ctrl.bf.newaddedfromhere_sgplus_mode =
			UNIPHY_SGMIIPLUS_MODE_DISABLE;
		if (force_port == A_TRUE) {
			rv = hppe_uniphy_channel0_input_output_4_get(dev_id,
				uniphy_index, &uniphy_force_ctrl);
			SW_RTN_ON_ERROR (rv);
			uniphy_force_ctrl.bf.newaddedfromhere_ch0_force_speed_25m =
				UNIPHY_FORCE_SPEED_ENABLE;
			rv = hppe_uniphy_channel0_input_output_4_set(dev_id,
				uniphy_index, &uniphy_force_ctrl);
			SW_RTN_ON_ERROR (rv);
		}
	} else {
		uniphy_mode_ctrl.bf.newaddedfromhere_sg_mode =
			UNIPHY_SGMII_MODE_DISABLE;
		uniphy_mode_ctrl.bf.newaddedfromhere_sgplus_mode =
			UNIPHY_SGMIIPLUS_MODE_ENABLE;
	}
	uniphy_mode_ctrl.bf.newaddedfromhere_ch0_autoneg_mode =
		UNIPHY_ATHEROS_NEGOTIATION;
	uniphy_mode_ctrl.bf.newaddedfromhere_ch0_psgmii_qsgmii =
		UNIPHY_CH0_QSGMII_SGMII_MODE;
	uniphy_mode_ctrl.bf.newaddedfromhere_ch0_qsgmii_sgmii =
		UNIPHY_CH0_SGMII_MODE;
	uniphy_mode_ctrl.bf.newaddedfromhere_xpcs_mode =
		UNIPHY_XPCS_MODE_DISABLE;
	rv = hppe_uniphy_mode_ctrl_set(dev_id, uniphy_index, &uniphy_mode_ctrl);

	return rv;
}

static sw_error_t
_adpt_mp_uniphy_clk_output_ctrl_set(a_uint32_t dev_id, a_uint32_t index,
	a_uint32_t clk_rate)
{
	sw_error_t rv = SW_OK;
	union uniphy_clock_output_control_u clock_output;

	memset(&clock_output, 0, sizeof(union uniphy_clock_output_control_u));
	clock_output.bf.ref_clk_output_drv = UNIPHY_CLK_DRV_1;
	clock_output.bf.ref_clk_output_en = A_TRUE;
	SSDK_INFO("uniphy will output clock as %dHz\n", clk_rate);
	if(clk_rate == UNIPHY_CLK_RATE_25M)
	{
		clock_output.bf.ref_clk_output_div = UNIPHY_CLK_DIV_25M;
	}
	else if(clk_rate == UNIPHY_CLK_RATE_50M)
	{
		clock_output.bf.ref_clk_output_div = UNIPHY_CLK_DIV_50M;
	}
	else
	{
		return SW_NOT_SUPPORTED;
	}
	rv = mp_uniphy_clock_output_control_set(dev_id, index,
			&clock_output);

	return rv;
}

static void
_adpt_mp_uniphy_clk_output_set(a_uint32_t dev_id, a_uint32_t index)
{
	a_uint32_t phy_id =0;
	a_bool_t force_port = A_FALSE;
	a_uint32_t force_speed = 0;

	/*when MP connect s17c or qca803x, need to reconfigure reference clock
	as 25M for port 2*/
	force_port = ssdk_port_feature_get(dev_id, SSDK_PHYSICAL_PORT2, PHY_F_FORCE);
	force_speed = ssdk_port_force_speed_get(dev_id, SSDK_PHYSICAL_PORT2);

	if ((force_port) && (force_speed == FAL_SPEED_1000))
	{
		_adpt_mp_uniphy_clk_output_ctrl_set(dev_id, index, UNIPHY_CLK_RATE_25M);
	}
	phy_id = hsl_port_phyid_get(dev_id, SSDK_PHYSICAL_PORT2);
	if (phy_id == QCA8030_PHY || phy_id == QCA8033_PHY || phy_id == QCA8035_PHY)
	{
		_adpt_mp_uniphy_clk_output_ctrl_set(dev_id, index, UNIPHY_CLK_RATE_25M);
	}

	return;
}

sw_error_t
adpt_mp_uniphy_mode_configure(a_uint32_t dev_id, a_uint32_t index, a_uint32_t mode)
{
	sw_error_t rv = SW_OK;
	a_uint32_t clock = UNIPHY_CLK_RATE_125M;

	union uniphy_misc2_phy_mode_u uniphy_misc2_phy_mode;

	memset(&uniphy_misc2_phy_mode, 0, sizeof(uniphy_misc2_phy_mode));

	ADPT_DEV_ID_CHECK(dev_id);

	if (index != SSDK_UNIPHY_INSTANCE0) {
		SSDK_ERROR("uniphy index is %d\n", index);
		return SW_BAD_VALUE;
	}

	if (mode == PORT_WRAPPER_MAX) {
		adpt_mp_gcc_uniphy_port_set(dev_id, SSDK_PHYSICAL_PORT2,
			A_FALSE);
		return SW_OK;
	} else if ((mode == PORT_WRAPPER_SGMII_CHANNEL0) ||
		(mode == PORT_WRAPPER_SGMII_PLUS)) {
		adpt_mp_gcc_uniphy_port_set(dev_id, SSDK_PHYSICAL_PORT2,
			A_TRUE);
	} else {
		return SW_NOT_SUPPORTED;
	}

	/*set the PHY mode to SGMII or SGMIIPLUS*/
	rv = hppe_uniphy_phy_mode_ctrl_get(dev_id, index,
		&uniphy_misc2_phy_mode);
	SW_RTN_ON_ERROR (rv);
	if (mode == PORT_WRAPPER_SGMII_CHANNEL0) {
		uniphy_misc2_phy_mode.bf.phy_mode =
			UNIPHY_PHY_SGMII_MODE;
		clock = UNIPHY_CLK_RATE_125M;
	} else {
		uniphy_misc2_phy_mode.bf.phy_mode =
			UNIPHY_PHY_SGMIIPLUS_MODE;
		clock = UNIPHY_CLK_RATE_312M;
	}
	rv = hppe_uniphy_phy_mode_ctrl_set(dev_id, index,
		&uniphy_misc2_phy_mode);
	SW_RTN_ON_ERROR (rv);

	/* reset uniphy */
	rv = adpt_mp_uniphy_reset(dev_id, index);
	SW_RTN_ON_ERROR (rv);

	/* disable uniphy port clock */
	rv = adpt_mp_gcc_uniphy_port_clock_set(dev_id, SSDK_PHYSICAL_PORT2,
		A_FALSE);
	SW_RTN_ON_ERROR (rv);

	rv = adpt_mp_uniphy_mode_ctrl_set(dev_id, index, mode);
	SW_RTN_ON_ERROR (rv);

	adpt_mp_gcc_uniphy_port_reset(dev_id, SSDK_PHYSICAL_PORT2);

	/* wait uniphy calibration done */
	rv = _adpt_mp_uniphy_calibrate(dev_id, index);
	SW_RTN_ON_ERROR (rv);

	/* enable instance clock */
	rv = adpt_mp_gcc_uniphy_port_clock_set(dev_id, SSDK_PHYSICAL_PORT2,
		A_TRUE);
	SW_RTN_ON_ERROR (rv);

	if (SW_OK == rv) {
		/* index + 1 point to mp uniphy clock */
		ssdk_mp_raw_clock_set(index + 1, UNIPHY_RX, clock);
		ssdk_mp_raw_clock_set(index + 1, UNIPHY_TX, clock);
	}

	if (mode == PORT_WRAPPER_SGMII_CHANNEL0) {
		SSDK_DEBUG("mp uniphy %d sgmii configuration is done!\n", index);
	} else {
		SSDK_DEBUG("mp uniphy %d sgmiiplus configuration is done!\n", index);
	}

	return rv;
}

sw_error_t
adpt_mp_uniphy_mode_set(a_uint32_t dev_id, a_uint32_t index, a_uint32_t mode)
{
	sw_error_t rv = SW_OK;

	rv = adpt_mp_uniphy_mode_configure(dev_id, index, mode);
	SW_RTN_ON_ERROR(rv);
	_adpt_mp_uniphy_clk_output_set(dev_id, index);

	/*port2 is connected with PHY, need gpio reset*/
	if(!ssdk_port_feature_get(dev_id, SSDK_PHYSICAL_PORT2, PHY_F_FORCE))
	{
		hsl_port_phy_gpio_reset(dev_id, SSDK_PHYSICAL_PORT2);
		msleep(100);
		hsl_port_phy_hw_init(dev_id, SSDK_PHYSICAL_PORT2);
	}

	return rv;
}

sw_error_t adpt_mp_uniphy_init(a_uint32_t dev_id)
{
	adpt_api_t *p_adpt_api = NULL;

	ADPT_DEV_ID_CHECK(dev_id);
	SW_RTN_ON_NULL(p_adpt_api = adpt_api_ptr_get(dev_id));

	p_adpt_api->adpt_uniphy_mode_set = adpt_mp_uniphy_mode_set;

	return SW_OK;
}

/**
 * @}
 */

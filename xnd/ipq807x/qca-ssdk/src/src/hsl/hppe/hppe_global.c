/*
 * Copyright (c) 2016-2017, The Linux Foundation. All rights reserved.
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
#include "hppe_reg_access.h"
#include "hppe_global_reg.h"
#include "hppe_global.h"

sw_error_t
hppe_switch_id_get(
		a_uint32_t dev_id,
		union switch_id_u *value)
{
	return hppe_reg_get(
				dev_id,
				NSS_GLOBAL_BASE_ADDR + SWITCH_ID_ADDRESS,
				&value->val);
}

sw_error_t
hppe_switch_id_set(
		a_uint32_t dev_id,
		union switch_id_u *value)
{
	return hppe_reg_set(
				dev_id,
				NSS_GLOBAL_BASE_ADDR + SWITCH_ID_ADDRESS,
				value->val);
}

sw_error_t
hppe_rgmii_ctrl_get(
		a_uint32_t dev_id,
		union rgmii_ctrl_u *value)
{
	return hppe_reg_get(
				dev_id,
				NSS_GLOBAL_BASE_ADDR + RGMII_CTRL_ADDRESS,
				&value->val);
}

sw_error_t
hppe_rgmii_ctrl_set(
		a_uint32_t dev_id,
		union rgmii_ctrl_u *value)
{
	return hppe_reg_set(
				dev_id,
				NSS_GLOBAL_BASE_ADDR + RGMII_CTRL_ADDRESS,
				value->val);
}

sw_error_t
hppe_clk_gating_ctrl_get(
		a_uint32_t dev_id,
		union clk_gating_ctrl_u *value)
{
	return hppe_reg_get(
				dev_id,
				NSS_GLOBAL_BASE_ADDR + CLK_GATING_CTRL_ADDRESS,
				&value->val);
}

sw_error_t
hppe_clk_gating_ctrl_set(
		a_uint32_t dev_id,
		union clk_gating_ctrl_u *value)
{
	return hppe_reg_set(
				dev_id,
				NSS_GLOBAL_BASE_ADDR + CLK_GATING_CTRL_ADDRESS,
				value->val);
}

sw_error_t
hppe_port_mux_ctrl_get(
		a_uint32_t dev_id,
		union port_mux_ctrl_u *value)
{
	return hppe_reg_get(
				dev_id,
				NSS_GLOBAL_BASE_ADDR + PORT_MUX_CTRL_ADDRESS,
				&value->val);
}

sw_error_t
hppe_port_mux_ctrl_set(
		a_uint32_t dev_id,
		union port_mux_ctrl_u *value)
{
	return hppe_reg_set(
				dev_id,
				NSS_GLOBAL_BASE_ADDR + PORT_MUX_CTRL_ADDRESS,
				value->val);
}

sw_error_t
cppe_port_mux_ctrl_get(
		a_uint32_t dev_id,
		union cppe_port_mux_ctrl_u *value)
{
	return hppe_reg_get(
				dev_id,
				NSS_GLOBAL_BASE_ADDR + PORT_MUX_CTRL_ADDRESS,
				&value->val);
}
sw_error_t
cppe_port_mux_ctrl_set(
		a_uint32_t dev_id,
		union cppe_port_mux_ctrl_u *value)
{
	return hppe_reg_set(
				dev_id,
				NSS_GLOBAL_BASE_ADDR + PORT_MUX_CTRL_ADDRESS,
				value->val);
}
sw_error_t
hppe_module_ini_done_int_get(
		a_uint32_t dev_id,
		union module_ini_done_int_u *value)
{
	return hppe_reg_get(
				dev_id,
				NSS_GLOBAL_BASE_ADDR + MODULE_INI_DONE_INT_ADDRESS,
				&value->val);
}

sw_error_t
hppe_module_ini_done_int_set(
		a_uint32_t dev_id,
		union module_ini_done_int_u *value)
{
	return hppe_reg_set(
				dev_id,
				NSS_GLOBAL_BASE_ADDR + MODULE_INI_DONE_INT_ADDRESS,
				value->val);
}

sw_error_t
hppe_module_cpu_done_int_get(
		a_uint32_t dev_id,
		union module_cpu_done_int_u *value)
{
	return hppe_reg_get(
				dev_id,
				NSS_GLOBAL_BASE_ADDR + MODULE_CPU_DONE_INT_ADDRESS,
				&value->val);
}

sw_error_t
hppe_module_cpu_done_int_set(
		a_uint32_t dev_id,
		union module_cpu_done_int_u *value)
{
	return hppe_reg_set(
				dev_id,
				NSS_GLOBAL_BASE_ADDR + MODULE_CPU_DONE_INT_ADDRESS,
				value->val);
}

sw_error_t
hppe_port_link_int_get(
		a_uint32_t dev_id,
		union port_link_int_u *value)
{
	return hppe_reg_get(
				dev_id,
				NSS_GLOBAL_BASE_ADDR + PORT_LINK_INT_ADDRESS,
				&value->val);
}

sw_error_t
hppe_port_link_int_set(
		a_uint32_t dev_id,
		union port_link_int_u *value)
{
	return hppe_reg_set(
				dev_id,
				NSS_GLOBAL_BASE_ADDR + PORT_LINK_INT_ADDRESS,
				value->val);
}

sw_error_t
hppe_module_ini_done_int_mask_get(
		a_uint32_t dev_id,
		union module_ini_done_int_mask_u *value)
{
	return hppe_reg_get(
				dev_id,
				NSS_GLOBAL_BASE_ADDR + MODULE_INI_DONE_INT_MASK_ADDRESS,
				&value->val);
}

sw_error_t
hppe_module_ini_done_int_mask_set(
		a_uint32_t dev_id,
		union module_ini_done_int_mask_u *value)
{
	return hppe_reg_set(
				dev_id,
				NSS_GLOBAL_BASE_ADDR + MODULE_INI_DONE_INT_MASK_ADDRESS,
				value->val);
}

sw_error_t
hppe_module_cpu_done_int_mask_get(
		a_uint32_t dev_id,
		union module_cpu_done_int_mask_u *value)
{
	return hppe_reg_get(
				dev_id,
				NSS_GLOBAL_BASE_ADDR + MODULE_CPU_DONE_INT_MASK_ADDRESS,
				&value->val);
}

sw_error_t
hppe_module_cpu_done_int_mask_set(
		a_uint32_t dev_id,
		union module_cpu_done_int_mask_u *value)
{
	return hppe_reg_set(
				dev_id,
				NSS_GLOBAL_BASE_ADDR + MODULE_CPU_DONE_INT_MASK_ADDRESS,
				value->val);
}

sw_error_t
hppe_port_link_int_mask_get(
		a_uint32_t dev_id,
		union port_link_int_mask_u *value)
{
	return hppe_reg_get(
				dev_id,
				NSS_GLOBAL_BASE_ADDR + PORT_LINK_INT_MASK_ADDRESS,
				&value->val);
}

sw_error_t
hppe_port_link_int_mask_set(
		a_uint32_t dev_id,
		union port_link_int_mask_u *value)
{
	return hppe_reg_set(
				dev_id,
				NSS_GLOBAL_BASE_ADDR + PORT_LINK_INT_MASK_ADDRESS,
				value->val);
}

sw_error_t
hppe_port_phy_status_0_get(
		a_uint32_t dev_id,
		union port_phy_status_0_u *value)
{
	return hppe_reg_get(
				dev_id,
				NSS_GLOBAL_BASE_ADDR + PORT_PHY_STATUS_0_ADDRESS,
				&value->val);
}

sw_error_t
hppe_port_phy_status_0_set(
		a_uint32_t dev_id,
		union port_phy_status_0_u *value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_port_phy_status_1_get(
		a_uint32_t dev_id,
		union port_phy_status_1_u *value)
{
	return hppe_reg_get(
				dev_id,
				NSS_GLOBAL_BASE_ADDR + PORT_PHY_STATUS_1_ADDRESS,
				&value->val);
}

sw_error_t
hppe_port_phy_status_1_set(
		a_uint32_t dev_id,
		union port_phy_status_1_u *value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_port1_status_get(
		a_uint32_t dev_id,
		union port1_status_u *value)
{
	return hppe_reg_get(
				dev_id,
				NSS_GLOBAL_BASE_ADDR + PORT1_STATUS_ADDRESS,
				&value->val);
}

sw_error_t
hppe_port1_status_set(
		a_uint32_t dev_id,
		union port1_status_u *value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_port2_status_get(
		a_uint32_t dev_id,
		union port2_status_u *value)
{
	return hppe_reg_get(
				dev_id,
				NSS_GLOBAL_BASE_ADDR + PORT2_STATUS_ADDRESS,
				&value->val);
}

sw_error_t
hppe_port2_status_set(
		a_uint32_t dev_id,
		union port2_status_u *value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_port3_status_get(
		a_uint32_t dev_id,
		union port3_status_u *value)
{
	return hppe_reg_get(
				dev_id,
				NSS_GLOBAL_BASE_ADDR + PORT3_STATUS_ADDRESS,
				&value->val);
}

sw_error_t
hppe_port3_status_set(
		a_uint32_t dev_id,
		union port3_status_u *value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_port4_status_get(
		a_uint32_t dev_id,
		union port4_status_u *value)
{
	return hppe_reg_get(
				dev_id,
				NSS_GLOBAL_BASE_ADDR + PORT4_STATUS_ADDRESS,
				&value->val);
}

sw_error_t
hppe_port4_status_set(
		a_uint32_t dev_id,
		union port4_status_u *value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_port5_status_get(
		a_uint32_t dev_id,
		union port5_status_u *value)
{
	return hppe_reg_get(
				dev_id,
				NSS_GLOBAL_BASE_ADDR + PORT5_STATUS_ADDRESS,
				&value->val);
}

sw_error_t
hppe_port5_status_set(
		a_uint32_t dev_id,
		union port5_status_u *value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_port6_status_get(
		a_uint32_t dev_id,
		union port6_status_u *value)
{
	return hppe_reg_get(
				dev_id,
				NSS_GLOBAL_BASE_ADDR + PORT6_STATUS_ADDRESS,
				&value->val);
}

sw_error_t
hppe_port6_status_set(
		a_uint32_t dev_id,
		union port6_status_u *value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_reserved_regs_0_get(
		a_uint32_t dev_id,
		union reserved_regs_0_u *value)
{
	return hppe_reg_get(
				dev_id,
				NSS_GLOBAL_BASE_ADDR + RESERVED_REGS_0_ADDRESS,
				&value->val);
}

sw_error_t
hppe_reserved_regs_0_set(
		a_uint32_t dev_id,
		union reserved_regs_0_u *value)
{
	return hppe_reg_set(
				dev_id,
				NSS_GLOBAL_BASE_ADDR + RESERVED_REGS_0_ADDRESS,
				value->val);
}

sw_error_t
hppe_reserved_regs_1_get(
		a_uint32_t dev_id,
		union reserved_regs_1_u *value)
{
	return hppe_reg_get(
				dev_id,
				NSS_GLOBAL_BASE_ADDR + RESERVED_REGS_1_ADDRESS,
				&value->val);
}

sw_error_t
hppe_reserved_regs_1_set(
		a_uint32_t dev_id,
		union reserved_regs_1_u *value)
{
	return hppe_reg_set(
				dev_id,
				NSS_GLOBAL_BASE_ADDR + RESERVED_REGS_1_ADDRESS,
				value->val);
}

sw_error_t
hppe_reserved_regs_2_get(
		a_uint32_t dev_id,
		union reserved_regs_2_u *value)
{
	return hppe_reg_get(
				dev_id,
				NSS_GLOBAL_BASE_ADDR + RESERVED_REGS_2_ADDRESS,
				&value->val);
}

sw_error_t
hppe_reserved_regs_2_set(
		a_uint32_t dev_id,
		union reserved_regs_2_u *value)
{
	return hppe_reg_set(
				dev_id,
				NSS_GLOBAL_BASE_ADDR + RESERVED_REGS_2_ADDRESS,
				value->val);
}

sw_error_t
hppe_reserved_regs_3_get(
		a_uint32_t dev_id,
		union reserved_regs_3_u *value)
{
	return hppe_reg_get(
				dev_id,
				NSS_GLOBAL_BASE_ADDR + RESERVED_REGS_3_ADDRESS,
				&value->val);
}

sw_error_t
hppe_reserved_regs_3_set(
		a_uint32_t dev_id,
		union reserved_regs_3_u *value)
{
	return hppe_reg_set(
				dev_id,
				NSS_GLOBAL_BASE_ADDR + RESERVED_REGS_3_ADDRESS,
				value->val);
}

sw_error_t
hppe_dbg_data_sel_get(
		a_uint32_t dev_id,
		union dbg_data_sel_u *value)
{
	return hppe_reg_get(
				dev_id,
				NSS_GLOBAL_BASE_ADDR + DBG_DATA_SEL_ADDRESS,
				&value->val);
}

sw_error_t
hppe_dbg_data_sel_set(
		a_uint32_t dev_id,
		union dbg_data_sel_u *value)
{
	return hppe_reg_set(
				dev_id,
				NSS_GLOBAL_BASE_ADDR + DBG_DATA_SEL_ADDRESS,
				value->val);
}

sw_error_t
hppe_switch_id_dev_id_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union switch_id_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_switch_id_get(dev_id, &reg_val);
	*value = reg_val.bf.dev_id;
	return ret;
}

sw_error_t
hppe_switch_id_dev_id_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union switch_id_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_switch_id_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.dev_id = value;
	ret = hppe_switch_id_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_switch_id_rev_id_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union switch_id_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_switch_id_get(dev_id, &reg_val);
	*value = reg_val.bf.rev_id;
	return ret;
}

sw_error_t
hppe_switch_id_rev_id_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union switch_id_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_switch_id_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.rev_id = value;
	ret = hppe_switch_id_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_rgmii_ctrl_rgmii_ctrl_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union rgmii_ctrl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_rgmii_ctrl_get(dev_id, &reg_val);
	*value = reg_val.bf.rgmii_ctrl;
	return ret;
}

sw_error_t
hppe_rgmii_ctrl_rgmii_ctrl_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union rgmii_ctrl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_rgmii_ctrl_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.rgmii_ctrl = value;
	ret = hppe_rgmii_ctrl_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_clk_gating_ctrl_clk_gating_ctrl_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union clk_gating_ctrl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_clk_gating_ctrl_get(dev_id, &reg_val);
	*value = reg_val.bf.clk_gating_ctrl;
	return ret;
}

sw_error_t
hppe_clk_gating_ctrl_clk_gating_ctrl_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union clk_gating_ctrl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_clk_gating_ctrl_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.clk_gating_ctrl = value;
	ret = hppe_clk_gating_ctrl_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_port_mux_ctrl_port6_pcs_sel_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union port_mux_ctrl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_port_mux_ctrl_get(dev_id, &reg_val);
	*value = reg_val.bf.port6_pcs_sel;
	return ret;
}

sw_error_t
hppe_port_mux_ctrl_port6_pcs_sel_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union port_mux_ctrl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_port_mux_ctrl_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.port6_pcs_sel = value;
	ret = hppe_port_mux_ctrl_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_port_mux_ctrl_port5_gmac_sel_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union port_mux_ctrl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_port_mux_ctrl_get(dev_id, &reg_val);
	*value = reg_val.bf.port5_gmac_sel;
	return ret;
}

sw_error_t
hppe_port_mux_ctrl_port5_gmac_sel_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union port_mux_ctrl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_port_mux_ctrl_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.port5_gmac_sel = value;
	ret = hppe_port_mux_ctrl_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_port_mux_ctrl_port5_pcs_sel_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union port_mux_ctrl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_port_mux_ctrl_get(dev_id, &reg_val);
	*value = reg_val.bf.port5_pcs_sel;
	return ret;
}

sw_error_t
hppe_port_mux_ctrl_port5_pcs_sel_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union port_mux_ctrl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_port_mux_ctrl_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.port5_pcs_sel = value;
	ret = hppe_port_mux_ctrl_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_port_mux_ctrl_port4_pcs_sel_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union port_mux_ctrl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_port_mux_ctrl_get(dev_id, &reg_val);
	*value = reg_val.bf.port4_pcs_sel;
	return ret;
}

sw_error_t
hppe_port_mux_ctrl_port4_pcs_sel_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union port_mux_ctrl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_port_mux_ctrl_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.port4_pcs_sel = value;
	ret = hppe_port_mux_ctrl_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_port_mux_ctrl_port6_gmac_sel_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union port_mux_ctrl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_port_mux_ctrl_get(dev_id, &reg_val);
	*value = reg_val.bf.port6_gmac_sel;
	return ret;
}

sw_error_t
hppe_port_mux_ctrl_port6_gmac_sel_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union port_mux_ctrl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_port_mux_ctrl_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.port6_gmac_sel = value;
	ret = hppe_port_mux_ctrl_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_module_ini_done_int_iv_ini_done_int_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union module_ini_done_int_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_module_ini_done_int_get(dev_id, &reg_val);
	*value = reg_val.bf.iv_ini_done_int;
	return ret;
}

sw_error_t
hppe_module_ini_done_int_iv_ini_done_int_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union module_ini_done_int_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_module_ini_done_int_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.iv_ini_done_int = value;
	ret = hppe_module_ini_done_int_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_module_ini_done_int_qm_ini_done_int_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union module_ini_done_int_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_module_ini_done_int_get(dev_id, &reg_val);
	*value = reg_val.bf.qm_ini_done_int;
	return ret;
}

sw_error_t
hppe_module_ini_done_int_qm_ini_done_int_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union module_ini_done_int_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_module_ini_done_int_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.qm_ini_done_int = value;
	ret = hppe_module_ini_done_int_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_module_ini_done_int_l3_ini_done_int_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union module_ini_done_int_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_module_ini_done_int_get(dev_id, &reg_val);
	*value = reg_val.bf.l3_ini_done_int;
	return ret;
}

sw_error_t
hppe_module_ini_done_int_l3_ini_done_int_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union module_ini_done_int_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_module_ini_done_int_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.l3_ini_done_int = value;
	ret = hppe_module_ini_done_int_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_module_ini_done_int_bm_ini_done_int_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union module_ini_done_int_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_module_ini_done_int_get(dev_id, &reg_val);
	*value = reg_val.bf.bm_ini_done_int;
	return ret;
}

sw_error_t
hppe_module_ini_done_int_bm_ini_done_int_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union module_ini_done_int_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_module_ini_done_int_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.bm_ini_done_int = value;
	ret = hppe_module_ini_done_int_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_module_ini_done_int_ptx_ini_done_int_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union module_ini_done_int_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_module_ini_done_int_get(dev_id, &reg_val);
	*value = reg_val.bf.ptx_ini_done_int;
	return ret;
}

sw_error_t
hppe_module_ini_done_int_ptx_ini_done_int_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union module_ini_done_int_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_module_ini_done_int_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ptx_ini_done_int = value;
	ret = hppe_module_ini_done_int_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_module_ini_done_int_tm_ini_done_int_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union module_ini_done_int_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_module_ini_done_int_get(dev_id, &reg_val);
	*value = reg_val.bf.tm_ini_done_int;
	return ret;
}

sw_error_t
hppe_module_ini_done_int_tm_ini_done_int_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union module_ini_done_int_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_module_ini_done_int_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.tm_ini_done_int = value;
	ret = hppe_module_ini_done_int_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_module_ini_done_int_l2_ini_done_int_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union module_ini_done_int_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_module_ini_done_int_get(dev_id, &reg_val);
	*value = reg_val.bf.l2_ini_done_int;
	return ret;
}

sw_error_t
hppe_module_ini_done_int_l2_ini_done_int_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union module_ini_done_int_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_module_ini_done_int_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.l2_ini_done_int = value;
	ret = hppe_module_ini_done_int_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_module_ini_done_int_acl_ini_done_int_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union module_ini_done_int_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_module_ini_done_int_get(dev_id, &reg_val);
	*value = reg_val.bf.acl_ini_done_int;
	return ret;
}

sw_error_t
hppe_module_ini_done_int_acl_ini_done_int_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union module_ini_done_int_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_module_ini_done_int_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.acl_ini_done_int = value;
	ret = hppe_module_ini_done_int_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_module_ini_done_int_ing_rate_ini_done_int_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union module_ini_done_int_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_module_ini_done_int_get(dev_id, &reg_val);
	*value = reg_val.bf.ing_rate_ini_done_int;
	return ret;
}

sw_error_t
hppe_module_ini_done_int_ing_rate_ini_done_int_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union module_ini_done_int_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_module_ini_done_int_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ing_rate_ini_done_int = value;
	ret = hppe_module_ini_done_int_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_module_cpu_done_int_l3_flow_wr_cmd_overflow_int_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union module_cpu_done_int_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_module_cpu_done_int_get(dev_id, &reg_val);
	*value = reg_val.bf.l3_flow_wr_cmd_overflow_int;
	return ret;
}

sw_error_t
hppe_module_cpu_done_int_l3_flow_wr_cmd_overflow_int_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union module_cpu_done_int_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_module_cpu_done_int_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.l3_flow_wr_cmd_overflow_int = value;
	ret = hppe_module_cpu_done_int_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_module_cpu_done_int_l2_fdb_rd_cmd_overflow_int_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union module_cpu_done_int_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_module_cpu_done_int_get(dev_id, &reg_val);
	*value = reg_val.bf.l2_fdb_rd_cmd_overflow_int;
	return ret;
}

sw_error_t
hppe_module_cpu_done_int_l2_fdb_rd_cmd_overflow_int_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union module_cpu_done_int_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_module_cpu_done_int_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.l2_fdb_rd_cmd_overflow_int = value;
	ret = hppe_module_cpu_done_int_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_module_cpu_done_int_qm_cpu_op_done_int_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union module_cpu_done_int_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_module_cpu_done_int_get(dev_id, &reg_val);
	*value = reg_val.bf.qm_cpu_op_done_int;
	return ret;
}

sw_error_t
hppe_module_cpu_done_int_qm_cpu_op_done_int_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union module_cpu_done_int_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_module_cpu_done_int_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.qm_cpu_op_done_int = value;
	ret = hppe_module_cpu_done_int_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_module_cpu_done_int_l3_flow_rd_cmd_overflow_int_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union module_cpu_done_int_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_module_cpu_done_int_get(dev_id, &reg_val);
	*value = reg_val.bf.l3_flow_rd_cmd_overflow_int;
	return ret;
}

sw_error_t
hppe_module_cpu_done_int_l3_flow_rd_cmd_overflow_int_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union module_cpu_done_int_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_module_cpu_done_int_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.l3_flow_rd_cmd_overflow_int = value;
	ret = hppe_module_cpu_done_int_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_module_cpu_done_int_l2_fdb_wr_result_vld_int_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union module_cpu_done_int_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_module_cpu_done_int_get(dev_id, &reg_val);
	*value = reg_val.bf.l2_fdb_wr_result_vld_int;
	return ret;
}

sw_error_t
hppe_module_cpu_done_int_l2_fdb_wr_result_vld_int_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union module_cpu_done_int_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_module_cpu_done_int_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.l2_fdb_wr_result_vld_int = value;
	ret = hppe_module_cpu_done_int_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_module_cpu_done_int_l2_fdb_rd_result_vld_int_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union module_cpu_done_int_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_module_cpu_done_int_get(dev_id, &reg_val);
	*value = reg_val.bf.l2_fdb_rd_result_vld_int;
	return ret;
}

sw_error_t
hppe_module_cpu_done_int_l2_fdb_rd_result_vld_int_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union module_cpu_done_int_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_module_cpu_done_int_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.l2_fdb_rd_result_vld_int = value;
	ret = hppe_module_cpu_done_int_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_module_cpu_done_int_l2_fdb_wr_cmd_overflow_int_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union module_cpu_done_int_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_module_cpu_done_int_get(dev_id, &reg_val);
	*value = reg_val.bf.l2_fdb_wr_cmd_overflow_int;
	return ret;
}

sw_error_t
hppe_module_cpu_done_int_l2_fdb_wr_cmd_overflow_int_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union module_cpu_done_int_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_module_cpu_done_int_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.l2_fdb_wr_cmd_overflow_int = value;
	ret = hppe_module_cpu_done_int_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_module_cpu_done_int_l3_flow_rd_result_vld_int_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union module_cpu_done_int_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_module_cpu_done_int_get(dev_id, &reg_val);
	*value = reg_val.bf.l3_flow_rd_result_vld_int;
	return ret;
}

sw_error_t
hppe_module_cpu_done_int_l3_flow_rd_result_vld_int_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union module_cpu_done_int_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_module_cpu_done_int_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.l3_flow_rd_result_vld_int = value;
	ret = hppe_module_cpu_done_int_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_module_cpu_done_int_l3_host_wr_cmd_overflow_int_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union module_cpu_done_int_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_module_cpu_done_int_get(dev_id, &reg_val);
	*value = reg_val.bf.l3_host_wr_cmd_overflow_int;
	return ret;
}

sw_error_t
hppe_module_cpu_done_int_l3_host_wr_cmd_overflow_int_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union module_cpu_done_int_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_module_cpu_done_int_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.l3_host_wr_cmd_overflow_int = value;
	ret = hppe_module_cpu_done_int_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_module_cpu_done_int_l3_host_rd_cmd_overflow_int_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union module_cpu_done_int_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_module_cpu_done_int_get(dev_id, &reg_val);
	*value = reg_val.bf.l3_host_rd_cmd_overflow_int;
	return ret;
}

sw_error_t
hppe_module_cpu_done_int_l3_host_rd_cmd_overflow_int_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union module_cpu_done_int_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_module_cpu_done_int_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.l3_host_rd_cmd_overflow_int = value;
	ret = hppe_module_cpu_done_int_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_module_cpu_done_int_l3_host_rd_result_vld_int_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union module_cpu_done_int_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_module_cpu_done_int_get(dev_id, &reg_val);
	*value = reg_val.bf.l3_host_rd_result_vld_int;
	return ret;
}

sw_error_t
hppe_module_cpu_done_int_l3_host_rd_result_vld_int_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union module_cpu_done_int_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_module_cpu_done_int_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.l3_host_rd_result_vld_int = value;
	ret = hppe_module_cpu_done_int_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_module_cpu_done_int_l3_host_wr_result_vld_int_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union module_cpu_done_int_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_module_cpu_done_int_get(dev_id, &reg_val);
	*value = reg_val.bf.l3_host_wr_result_vld_int;
	return ret;
}

sw_error_t
hppe_module_cpu_done_int_l3_host_wr_result_vld_int_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union module_cpu_done_int_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_module_cpu_done_int_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.l3_host_wr_result_vld_int = value;
	ret = hppe_module_cpu_done_int_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_module_cpu_done_int_l3_flow_wr_result_vld_int_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union module_cpu_done_int_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_module_cpu_done_int_get(dev_id, &reg_val);
	*value = reg_val.bf.l3_flow_wr_result_vld_int;
	return ret;
}

sw_error_t
hppe_module_cpu_done_int_l3_flow_wr_result_vld_int_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union module_cpu_done_int_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_module_cpu_done_int_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.l3_flow_wr_result_vld_int = value;
	ret = hppe_module_cpu_done_int_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_port_link_int_port6_link_chg_int_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union port_link_int_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_port_link_int_get(dev_id, &reg_val);
	*value = reg_val.bf.port6_link_chg_int;
	return ret;
}

sw_error_t
hppe_port_link_int_port6_link_chg_int_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union port_link_int_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_port_link_int_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.port6_link_chg_int = value;
	ret = hppe_port_link_int_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_port_link_int_xgmac0_an_done_int_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union port_link_int_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_port_link_int_get(dev_id, &reg_val);
	*value = reg_val.bf.xgmac0_an_done_int;
	return ret;
}

sw_error_t
hppe_port_link_int_xgmac0_an_done_int_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union port_link_int_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_port_link_int_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.xgmac0_an_done_int = value;
	ret = hppe_port_link_int_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_port_link_int_port5_1_link_chg_int_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union port_link_int_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_port_link_int_get(dev_id, &reg_val);
	*value = reg_val.bf.port5_1_link_chg_int;
	return ret;
}

sw_error_t
hppe_port_link_int_port5_1_link_chg_int_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union port_link_int_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_port_link_int_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.port5_1_link_chg_int = value;
	ret = hppe_port_link_int_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_port_link_int_port5_0_link_chg_int_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union port_link_int_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_port_link_int_get(dev_id, &reg_val);
	*value = reg_val.bf.port5_0_link_chg_int;
	return ret;
}

sw_error_t
hppe_port_link_int_port5_0_link_chg_int_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union port_link_int_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_port_link_int_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.port5_0_link_chg_int = value;
	ret = hppe_port_link_int_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_port_link_int_port4_link_chg_int_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union port_link_int_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_port_link_int_get(dev_id, &reg_val);
	*value = reg_val.bf.port4_link_chg_int;
	return ret;
}

sw_error_t
hppe_port_link_int_port4_link_chg_int_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union port_link_int_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_port_link_int_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.port4_link_chg_int = value;
	ret = hppe_port_link_int_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_port_link_int_port3_link_chg_int_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union port_link_int_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_port_link_int_get(dev_id, &reg_val);
	*value = reg_val.bf.port3_link_chg_int;
	return ret;
}

sw_error_t
hppe_port_link_int_port3_link_chg_int_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union port_link_int_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_port_link_int_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.port3_link_chg_int = value;
	ret = hppe_port_link_int_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_port_link_int_port2_link_chg_int_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union port_link_int_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_port_link_int_get(dev_id, &reg_val);
	*value = reg_val.bf.port2_link_chg_int;
	return ret;
}

sw_error_t
hppe_port_link_int_port2_link_chg_int_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union port_link_int_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_port_link_int_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.port2_link_chg_int = value;
	ret = hppe_port_link_int_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_port_link_int_xgmac1_an_done_int_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union port_link_int_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_port_link_int_get(dev_id, &reg_val);
	*value = reg_val.bf.xgmac1_an_done_int;
	return ret;
}

sw_error_t
hppe_port_link_int_xgmac1_an_done_int_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union port_link_int_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_port_link_int_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.xgmac1_an_done_int = value;
	ret = hppe_port_link_int_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_port_link_int_port1_link_chg_int_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union port_link_int_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_port_link_int_get(dev_id, &reg_val);
	*value = reg_val.bf.port1_link_chg_int;
	return ret;
}

sw_error_t
hppe_port_link_int_port1_link_chg_int_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union port_link_int_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_port_link_int_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.port1_link_chg_int = value;
	ret = hppe_port_link_int_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_module_ini_done_int_mask_tm_ini_done_int_mask_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union module_ini_done_int_mask_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_module_ini_done_int_mask_get(dev_id, &reg_val);
	*value = reg_val.bf.tm_ini_done_int_mask;
	return ret;
}

sw_error_t
hppe_module_ini_done_int_mask_tm_ini_done_int_mask_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union module_ini_done_int_mask_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_module_ini_done_int_mask_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.tm_ini_done_int_mask = value;
	ret = hppe_module_ini_done_int_mask_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_module_ini_done_int_mask_bm_ini_done_int_mask_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union module_ini_done_int_mask_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_module_ini_done_int_mask_get(dev_id, &reg_val);
	*value = reg_val.bf.bm_ini_done_int_mask;
	return ret;
}

sw_error_t
hppe_module_ini_done_int_mask_bm_ini_done_int_mask_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union module_ini_done_int_mask_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_module_ini_done_int_mask_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.bm_ini_done_int_mask = value;
	ret = hppe_module_ini_done_int_mask_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_module_ini_done_int_mask_iv_ini_done_int_mask_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union module_ini_done_int_mask_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_module_ini_done_int_mask_get(dev_id, &reg_val);
	*value = reg_val.bf.iv_ini_done_int_mask;
	return ret;
}

sw_error_t
hppe_module_ini_done_int_mask_iv_ini_done_int_mask_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union module_ini_done_int_mask_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_module_ini_done_int_mask_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.iv_ini_done_int_mask = value;
	ret = hppe_module_ini_done_int_mask_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_module_ini_done_int_mask_acl_ini_done_int_mask_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union module_ini_done_int_mask_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_module_ini_done_int_mask_get(dev_id, &reg_val);
	*value = reg_val.bf.acl_ini_done_int_mask;
	return ret;
}

sw_error_t
hppe_module_ini_done_int_mask_acl_ini_done_int_mask_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union module_ini_done_int_mask_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_module_ini_done_int_mask_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.acl_ini_done_int_mask = value;
	ret = hppe_module_ini_done_int_mask_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_module_ini_done_int_mask_qm_ini_done_int_mask_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union module_ini_done_int_mask_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_module_ini_done_int_mask_get(dev_id, &reg_val);
	*value = reg_val.bf.qm_ini_done_int_mask;
	return ret;
}

sw_error_t
hppe_module_ini_done_int_mask_qm_ini_done_int_mask_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union module_ini_done_int_mask_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_module_ini_done_int_mask_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.qm_ini_done_int_mask = value;
	ret = hppe_module_ini_done_int_mask_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_module_ini_done_int_mask_l2_ini_done_int_mask_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union module_ini_done_int_mask_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_module_ini_done_int_mask_get(dev_id, &reg_val);
	*value = reg_val.bf.l2_ini_done_int_mask;
	return ret;
}

sw_error_t
hppe_module_ini_done_int_mask_l2_ini_done_int_mask_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union module_ini_done_int_mask_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_module_ini_done_int_mask_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.l2_ini_done_int_mask = value;
	ret = hppe_module_ini_done_int_mask_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_module_ini_done_int_mask_ptx_ini_done_int_mask_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union module_ini_done_int_mask_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_module_ini_done_int_mask_get(dev_id, &reg_val);
	*value = reg_val.bf.ptx_ini_done_int_mask;
	return ret;
}

sw_error_t
hppe_module_ini_done_int_mask_ptx_ini_done_int_mask_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union module_ini_done_int_mask_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_module_ini_done_int_mask_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ptx_ini_done_int_mask = value;
	ret = hppe_module_ini_done_int_mask_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_module_ini_done_int_mask_ing_rate_ini_done_int_mask_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union module_ini_done_int_mask_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_module_ini_done_int_mask_get(dev_id, &reg_val);
	*value = reg_val.bf.ing_rate_ini_done_int_mask;
	return ret;
}

sw_error_t
hppe_module_ini_done_int_mask_ing_rate_ini_done_int_mask_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union module_ini_done_int_mask_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_module_ini_done_int_mask_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ing_rate_ini_done_int_mask = value;
	ret = hppe_module_ini_done_int_mask_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_module_ini_done_int_mask_l3_ini_done_int_mask_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union module_ini_done_int_mask_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_module_ini_done_int_mask_get(dev_id, &reg_val);
	*value = reg_val.bf.l3_ini_done_int_mask;
	return ret;
}

sw_error_t
hppe_module_ini_done_int_mask_l3_ini_done_int_mask_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union module_ini_done_int_mask_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_module_ini_done_int_mask_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.l3_ini_done_int_mask = value;
	ret = hppe_module_ini_done_int_mask_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_module_cpu_done_int_mask_l3_host_wr_result_vld_int_mask_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union module_cpu_done_int_mask_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_module_cpu_done_int_mask_get(dev_id, &reg_val);
	*value = reg_val.bf.l3_host_wr_result_vld_int_mask;
	return ret;
}

sw_error_t
hppe_module_cpu_done_int_mask_l3_host_wr_result_vld_int_mask_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union module_cpu_done_int_mask_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_module_cpu_done_int_mask_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.l3_host_wr_result_vld_int_mask = value;
	ret = hppe_module_cpu_done_int_mask_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_module_cpu_done_int_mask_l3_flow_wr_result_vld_int_mask_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union module_cpu_done_int_mask_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_module_cpu_done_int_mask_get(dev_id, &reg_val);
	*value = reg_val.bf.l3_flow_wr_result_vld_int_mask;
	return ret;
}

sw_error_t
hppe_module_cpu_done_int_mask_l3_flow_wr_result_vld_int_mask_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union module_cpu_done_int_mask_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_module_cpu_done_int_mask_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.l3_flow_wr_result_vld_int_mask = value;
	ret = hppe_module_cpu_done_int_mask_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_module_cpu_done_int_mask_l3_host_rd_cmd_overflow_int_mask_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union module_cpu_done_int_mask_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_module_cpu_done_int_mask_get(dev_id, &reg_val);
	*value = reg_val.bf.l3_host_rd_cmd_overflow_int_mask;
	return ret;
}

sw_error_t
hppe_module_cpu_done_int_mask_l3_host_rd_cmd_overflow_int_mask_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union module_cpu_done_int_mask_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_module_cpu_done_int_mask_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.l3_host_rd_cmd_overflow_int_mask = value;
	ret = hppe_module_cpu_done_int_mask_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_module_cpu_done_int_mask_l3_flow_wr_cmd_overflow_int_mask_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union module_cpu_done_int_mask_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_module_cpu_done_int_mask_get(dev_id, &reg_val);
	*value = reg_val.bf.l3_flow_wr_cmd_overflow_int_mask;
	return ret;
}

sw_error_t
hppe_module_cpu_done_int_mask_l3_flow_wr_cmd_overflow_int_mask_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union module_cpu_done_int_mask_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_module_cpu_done_int_mask_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.l3_flow_wr_cmd_overflow_int_mask = value;
	ret = hppe_module_cpu_done_int_mask_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_module_cpu_done_int_mask_l3_host_rd_result_vld_int_mask_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union module_cpu_done_int_mask_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_module_cpu_done_int_mask_get(dev_id, &reg_val);
	*value = reg_val.bf.l3_host_rd_result_vld_int_mask;
	return ret;
}

sw_error_t
hppe_module_cpu_done_int_mask_l3_host_rd_result_vld_int_mask_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union module_cpu_done_int_mask_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_module_cpu_done_int_mask_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.l3_host_rd_result_vld_int_mask = value;
	ret = hppe_module_cpu_done_int_mask_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_module_cpu_done_int_mask_l2_fdb_rd_cmd_overflow_int_mask_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union module_cpu_done_int_mask_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_module_cpu_done_int_mask_get(dev_id, &reg_val);
	*value = reg_val.bf.l2_fdb_rd_cmd_overflow_int_mask;
	return ret;
}

sw_error_t
hppe_module_cpu_done_int_mask_l2_fdb_rd_cmd_overflow_int_mask_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union module_cpu_done_int_mask_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_module_cpu_done_int_mask_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.l2_fdb_rd_cmd_overflow_int_mask = value;
	ret = hppe_module_cpu_done_int_mask_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_module_cpu_done_int_mask_l2_fdb_wr_cmd_overflow_int_mask_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union module_cpu_done_int_mask_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_module_cpu_done_int_mask_get(dev_id, &reg_val);
	*value = reg_val.bf.l2_fdb_wr_cmd_overflow_int_mask;
	return ret;
}

sw_error_t
hppe_module_cpu_done_int_mask_l2_fdb_wr_cmd_overflow_int_mask_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union module_cpu_done_int_mask_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_module_cpu_done_int_mask_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.l2_fdb_wr_cmd_overflow_int_mask = value;
	ret = hppe_module_cpu_done_int_mask_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_module_cpu_done_int_mask_l3_host_wr_cmd_overflow_int_mask_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union module_cpu_done_int_mask_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_module_cpu_done_int_mask_get(dev_id, &reg_val);
	*value = reg_val.bf.l3_host_wr_cmd_overflow_int_mask;
	return ret;
}

sw_error_t
hppe_module_cpu_done_int_mask_l3_host_wr_cmd_overflow_int_mask_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union module_cpu_done_int_mask_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_module_cpu_done_int_mask_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.l3_host_wr_cmd_overflow_int_mask = value;
	ret = hppe_module_cpu_done_int_mask_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_module_cpu_done_int_mask_l2_fdb_rd_result_vld_int_mask_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union module_cpu_done_int_mask_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_module_cpu_done_int_mask_get(dev_id, &reg_val);
	*value = reg_val.bf.l2_fdb_rd_result_vld_int_mask;
	return ret;
}

sw_error_t
hppe_module_cpu_done_int_mask_l2_fdb_rd_result_vld_int_mask_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union module_cpu_done_int_mask_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_module_cpu_done_int_mask_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.l2_fdb_rd_result_vld_int_mask = value;
	ret = hppe_module_cpu_done_int_mask_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_module_cpu_done_int_mask_l2_fdb_wr_result_vld_int_mask_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union module_cpu_done_int_mask_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_module_cpu_done_int_mask_get(dev_id, &reg_val);
	*value = reg_val.bf.l2_fdb_wr_result_vld_int_mask;
	return ret;
}

sw_error_t
hppe_module_cpu_done_int_mask_l2_fdb_wr_result_vld_int_mask_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union module_cpu_done_int_mask_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_module_cpu_done_int_mask_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.l2_fdb_wr_result_vld_int_mask = value;
	ret = hppe_module_cpu_done_int_mask_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_module_cpu_done_int_mask_qm_cpu_op_done_int_mask_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union module_cpu_done_int_mask_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_module_cpu_done_int_mask_get(dev_id, &reg_val);
	*value = reg_val.bf.qm_cpu_op_done_int_mask;
	return ret;
}

sw_error_t
hppe_module_cpu_done_int_mask_qm_cpu_op_done_int_mask_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union module_cpu_done_int_mask_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_module_cpu_done_int_mask_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.qm_cpu_op_done_int_mask = value;
	ret = hppe_module_cpu_done_int_mask_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_module_cpu_done_int_mask_l3_flow_rd_result_vld_int_mask_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union module_cpu_done_int_mask_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_module_cpu_done_int_mask_get(dev_id, &reg_val);
	*value = reg_val.bf.l3_flow_rd_result_vld_int_mask;
	return ret;
}

sw_error_t
hppe_module_cpu_done_int_mask_l3_flow_rd_result_vld_int_mask_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union module_cpu_done_int_mask_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_module_cpu_done_int_mask_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.l3_flow_rd_result_vld_int_mask = value;
	ret = hppe_module_cpu_done_int_mask_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_module_cpu_done_int_mask_l3_flow_rd_cmd_overflow_int_mask_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union module_cpu_done_int_mask_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_module_cpu_done_int_mask_get(dev_id, &reg_val);
	*value = reg_val.bf.l3_flow_rd_cmd_overflow_int_mask;
	return ret;
}

sw_error_t
hppe_module_cpu_done_int_mask_l3_flow_rd_cmd_overflow_int_mask_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union module_cpu_done_int_mask_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_module_cpu_done_int_mask_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.l3_flow_rd_cmd_overflow_int_mask = value;
	ret = hppe_module_cpu_done_int_mask_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_port_link_int_mask_xgmac0_an_done_int_mask_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union port_link_int_mask_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_port_link_int_mask_get(dev_id, &reg_val);
	*value = reg_val.bf.xgmac0_an_done_int_mask;
	return ret;
}

sw_error_t
hppe_port_link_int_mask_xgmac0_an_done_int_mask_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union port_link_int_mask_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_port_link_int_mask_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.xgmac0_an_done_int_mask = value;
	ret = hppe_port_link_int_mask_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_port_link_int_mask_port2_link_chg_int_mask_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union port_link_int_mask_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_port_link_int_mask_get(dev_id, &reg_val);
	*value = reg_val.bf.port2_link_chg_int_mask;
	return ret;
}

sw_error_t
hppe_port_link_int_mask_port2_link_chg_int_mask_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union port_link_int_mask_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_port_link_int_mask_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.port2_link_chg_int_mask = value;
	ret = hppe_port_link_int_mask_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_port_link_int_mask_port4_link_chg_int_mask_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union port_link_int_mask_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_port_link_int_mask_get(dev_id, &reg_val);
	*value = reg_val.bf.port4_link_chg_int_mask;
	return ret;
}

sw_error_t
hppe_port_link_int_mask_port4_link_chg_int_mask_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union port_link_int_mask_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_port_link_int_mask_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.port4_link_chg_int_mask = value;
	ret = hppe_port_link_int_mask_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_port_link_int_mask_port3_link_chg_int_mask_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union port_link_int_mask_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_port_link_int_mask_get(dev_id, &reg_val);
	*value = reg_val.bf.port3_link_chg_int_mask;
	return ret;
}

sw_error_t
hppe_port_link_int_mask_port3_link_chg_int_mask_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union port_link_int_mask_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_port_link_int_mask_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.port3_link_chg_int_mask = value;
	ret = hppe_port_link_int_mask_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_port_link_int_mask_port5_1_link_chg_int_mask_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union port_link_int_mask_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_port_link_int_mask_get(dev_id, &reg_val);
	*value = reg_val.bf.port5_1_link_chg_int_mask;
	return ret;
}

sw_error_t
hppe_port_link_int_mask_port5_1_link_chg_int_mask_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union port_link_int_mask_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_port_link_int_mask_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.port5_1_link_chg_int_mask = value;
	ret = hppe_port_link_int_mask_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_port_link_int_mask_xgmac1_an_done_int_mask_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union port_link_int_mask_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_port_link_int_mask_get(dev_id, &reg_val);
	*value = reg_val.bf.xgmac1_an_done_int_mask;
	return ret;
}

sw_error_t
hppe_port_link_int_mask_xgmac1_an_done_int_mask_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union port_link_int_mask_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_port_link_int_mask_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.xgmac1_an_done_int_mask = value;
	ret = hppe_port_link_int_mask_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_port_link_int_mask_port1_link_chg_int_mask_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union port_link_int_mask_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_port_link_int_mask_get(dev_id, &reg_val);
	*value = reg_val.bf.port1_link_chg_int_mask;
	return ret;
}

sw_error_t
hppe_port_link_int_mask_port1_link_chg_int_mask_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union port_link_int_mask_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_port_link_int_mask_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.port1_link_chg_int_mask = value;
	ret = hppe_port_link_int_mask_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_port_link_int_mask_port5_0_link_chg_int_mask_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union port_link_int_mask_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_port_link_int_mask_get(dev_id, &reg_val);
	*value = reg_val.bf.port5_0_link_chg_int_mask;
	return ret;
}

sw_error_t
hppe_port_link_int_mask_port5_0_link_chg_int_mask_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union port_link_int_mask_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_port_link_int_mask_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.port5_0_link_chg_int_mask = value;
	ret = hppe_port_link_int_mask_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_port_link_int_mask_port6_link_chg_int_mask_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union port_link_int_mask_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_port_link_int_mask_get(dev_id, &reg_val);
	*value = reg_val.bf.port6_link_chg_int_mask;
	return ret;
}

sw_error_t
hppe_port_link_int_mask_port6_link_chg_int_mask_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union port_link_int_mask_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_port_link_int_mask_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.port6_link_chg_int_mask = value;
	ret = hppe_port_link_int_mask_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_port_phy_status_0_port3_phy_status_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union port_phy_status_0_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_port_phy_status_0_get(dev_id, &reg_val);
	*value = reg_val.bf.port3_phy_status;
	return ret;
}

sw_error_t
hppe_port_phy_status_0_port3_phy_status_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_port_phy_status_0_port4_phy_status_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union port_phy_status_0_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_port_phy_status_0_get(dev_id, &reg_val);
	*value = reg_val.bf.port4_phy_status;
	return ret;
}

sw_error_t
hppe_port_phy_status_0_port4_phy_status_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_port_phy_status_0_port2_phy_status_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union port_phy_status_0_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_port_phy_status_0_get(dev_id, &reg_val);
	*value = reg_val.bf.port2_phy_status;
	return ret;
}

sw_error_t
hppe_port_phy_status_0_port2_phy_status_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_port_phy_status_0_port1_phy_status_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union port_phy_status_0_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_port_phy_status_0_get(dev_id, &reg_val);
	*value = reg_val.bf.port1_phy_status;
	return ret;
}

sw_error_t
hppe_port_phy_status_0_port1_phy_status_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_port_phy_status_1_port6_phy_status_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union port_phy_status_1_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_port_phy_status_1_get(dev_id, &reg_val);
	*value = reg_val.bf.port6_phy_status;
	return ret;
}

sw_error_t
hppe_port_phy_status_1_port6_phy_status_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_port_phy_status_1_port5_0_phy_status_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union port_phy_status_1_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_port_phy_status_1_get(dev_id, &reg_val);
	*value = reg_val.bf.port5_0_phy_status;
	return ret;
}

sw_error_t
hppe_port_phy_status_1_port5_0_phy_status_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_port_phy_status_1_port5_1_phy_status_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union port_phy_status_1_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_port_phy_status_1_get(dev_id, &reg_val);
	*value = reg_val.bf.port5_1_phy_status;
	return ret;
}

sw_error_t
hppe_port_phy_status_1_port5_1_phy_status_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_port1_status_port1_status_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union port1_status_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_port1_status_get(dev_id, &reg_val);
	*value = reg_val.bf.port1_status;
	return ret;
}

sw_error_t
hppe_port1_status_port1_status_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_port2_status_port2_status_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union port2_status_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_port2_status_get(dev_id, &reg_val);
	*value = reg_val.bf.port2_status;
	return ret;
}

sw_error_t
hppe_port2_status_port2_status_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_port3_status_port3_status_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union port3_status_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_port3_status_get(dev_id, &reg_val);
	*value = reg_val.bf.port3_status;
	return ret;
}

sw_error_t
hppe_port3_status_port3_status_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_port4_status_port4_status_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union port4_status_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_port4_status_get(dev_id, &reg_val);
	*value = reg_val.bf.port4_status;
	return ret;
}

sw_error_t
hppe_port4_status_port4_status_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_port5_status_port3_mac_speed_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union port5_status_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_port5_status_get(dev_id, &reg_val);
	*value = reg_val.bf.port3_mac_speed;
	return ret;
}

sw_error_t
hppe_port5_status_port3_mac_speed_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_port5_status_port2_mac_speed_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union port5_status_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_port5_status_get(dev_id, &reg_val);
	*value = reg_val.bf.port2_mac_speed;
	return ret;
}

sw_error_t
hppe_port5_status_port2_mac_speed_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_port5_status_port1_mac_speed_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union port5_status_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_port5_status_get(dev_id, &reg_val);
	*value = reg_val.bf.port1_mac_speed;
	return ret;
}

sw_error_t
hppe_port5_status_port1_mac_speed_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_port5_status_port5_status_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union port5_status_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_port5_status_get(dev_id, &reg_val);
	*value = reg_val.bf.port5_status;
	return ret;
}

sw_error_t
hppe_port5_status_port5_status_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_port5_status_port4_mac_speed_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union port5_status_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_port5_status_get(dev_id, &reg_val);
	*value = reg_val.bf.port4_mac_speed;
	return ret;
}

sw_error_t
hppe_port5_status_port4_mac_speed_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_port6_status_port6_status_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union port6_status_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_port6_status_get(dev_id, &reg_val);
	*value = reg_val.bf.port6_status;
	return ret;
}

sw_error_t
hppe_port6_status_port6_status_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_reserved_regs_0_spare_regs_0_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union reserved_regs_0_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_reserved_regs_0_get(dev_id, &reg_val);
	*value = reg_val.bf.spare_regs_0;
	return ret;
}

sw_error_t
hppe_reserved_regs_0_spare_regs_0_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union reserved_regs_0_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_reserved_regs_0_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.spare_regs_0 = value;
	ret = hppe_reserved_regs_0_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_reserved_regs_1_spare_regs_1_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union reserved_regs_1_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_reserved_regs_1_get(dev_id, &reg_val);
	*value = reg_val.bf.spare_regs_1;
	return ret;
}

sw_error_t
hppe_reserved_regs_1_spare_regs_1_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union reserved_regs_1_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_reserved_regs_1_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.spare_regs_1 = value;
	ret = hppe_reserved_regs_1_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_reserved_regs_2_spare_regs_2_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union reserved_regs_2_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_reserved_regs_2_get(dev_id, &reg_val);
	*value = reg_val.bf.spare_regs_2;
	return ret;
}

sw_error_t
hppe_reserved_regs_2_spare_regs_2_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union reserved_regs_2_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_reserved_regs_2_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.spare_regs_2 = value;
	ret = hppe_reserved_regs_2_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_reserved_regs_3_spare_regs_3_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union reserved_regs_3_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_reserved_regs_3_get(dev_id, &reg_val);
	*value = reg_val.bf.spare_regs_3;
	return ret;
}

sw_error_t
hppe_reserved_regs_3_spare_regs_3_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union reserved_regs_3_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_reserved_regs_3_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.spare_regs_3 = value;
	ret = hppe_reserved_regs_3_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_dbg_data_sel_dbg_data_sel_desp_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union dbg_data_sel_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_dbg_data_sel_get(dev_id, &reg_val);
	*value = reg_val.bf.dbg_data_sel_desp;
	return ret;
}

sw_error_t
hppe_dbg_data_sel_dbg_data_sel_desp_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union dbg_data_sel_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_dbg_data_sel_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.dbg_data_sel_desp = value;
	ret = hppe_dbg_data_sel_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_dbg_data_sel_dbg_data_sel_switch_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union dbg_data_sel_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_dbg_data_sel_get(dev_id, &reg_val);
	*value = reg_val.bf.dbg_data_sel_switch;
	return ret;
}

sw_error_t
hppe_dbg_data_sel_dbg_data_sel_switch_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union dbg_data_sel_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_dbg_data_sel_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.dbg_data_sel_switch = value;
	ret = hppe_dbg_data_sel_set(dev_id, &reg_val);
	return ret;
}


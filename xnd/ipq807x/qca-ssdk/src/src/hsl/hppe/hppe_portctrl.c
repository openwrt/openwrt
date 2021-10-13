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
#include "hppe_portctrl_reg.h"
#include "hppe_portctrl.h"

sw_error_t
hppe_mac_enable_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union mac_enable_u *value)
{
	if (index >= MAC_ENABLE_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				NSS_MAC_CSR_BASE_ADDR + MAC_ENABLE_ADDRESS + \
				index * MAC_ENABLE_INC,
				&value->val);
}

sw_error_t
hppe_mac_enable_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union mac_enable_u *value)
{
	return hppe_reg_set(
				dev_id,
				NSS_MAC_CSR_BASE_ADDR + MAC_ENABLE_ADDRESS + \
				index * MAC_ENABLE_INC,
				value->val);
}

sw_error_t
hppe_mac_speed_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union mac_speed_u *value)
{
	if (index >= MAC_SPEED_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				NSS_MAC_CSR_BASE_ADDR + MAC_SPEED_ADDRESS + \
				index * MAC_SPEED_INC,
				&value->val);
}

sw_error_t
hppe_mac_speed_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union mac_speed_u *value)
{
	return hppe_reg_set(
				dev_id,
				NSS_MAC_CSR_BASE_ADDR + MAC_SPEED_ADDRESS + \
				index * MAC_SPEED_INC,
				value->val);
}
#ifndef IN_PORTCONTROL_MINI
sw_error_t
hppe_gol_mac_addr0_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union gol_mac_addr0_u *value)
{
	if (index >= GOL_MAC_ADDR0_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				NSS_MAC_CSR_BASE_ADDR + GOL_MAC_ADDR0_ADDRESS + \
				index * GOL_MAC_ADDR0_INC,
				&value->val);
}

sw_error_t
hppe_gol_mac_addr0_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union gol_mac_addr0_u *value)
{
	return hppe_reg_set(
				dev_id,
				NSS_MAC_CSR_BASE_ADDR + GOL_MAC_ADDR0_ADDRESS + \
				index * GOL_MAC_ADDR0_INC,
				value->val);
}

sw_error_t
hppe_gol_mac_addr1_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union gol_mac_addr1_u *value)
{
	if (index >= GOL_MAC_ADDR1_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				NSS_MAC_CSR_BASE_ADDR + GOL_MAC_ADDR1_ADDRESS + \
				index * GOL_MAC_ADDR1_INC,
				&value->val);
}

sw_error_t
hppe_gol_mac_addr1_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union gol_mac_addr1_u *value)
{
	return hppe_reg_set(
				dev_id,
				NSS_MAC_CSR_BASE_ADDR + GOL_MAC_ADDR1_ADDRESS + \
				index * GOL_MAC_ADDR1_INC,
				value->val);
}

sw_error_t
hppe_mac_ctrl0_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union mac_ctrl0_u *value)
{
	if (index >= MAC_CTRL0_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				NSS_MAC_CSR_BASE_ADDR + MAC_CTRL0_ADDRESS + \
				index * MAC_CTRL0_INC,
				&value->val);
}

sw_error_t
hppe_mac_ctrl0_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union mac_ctrl0_u *value)
{
	return hppe_reg_set(
				dev_id,
				NSS_MAC_CSR_BASE_ADDR + MAC_CTRL0_ADDRESS + \
				index * MAC_CTRL0_INC,
				value->val);
}

sw_error_t
hppe_mac_ctrl1_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union mac_ctrl1_u *value)
{
	if (index >= MAC_CTRL1_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				NSS_MAC_CSR_BASE_ADDR + MAC_CTRL1_ADDRESS + \
				index * MAC_CTRL1_INC,
				&value->val);
}

sw_error_t
hppe_mac_ctrl1_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union mac_ctrl1_u *value)
{
	return hppe_reg_set(
				dev_id,
				NSS_MAC_CSR_BASE_ADDR + MAC_CTRL1_ADDRESS + \
				index * MAC_CTRL1_INC,
				value->val);
}
#endif
sw_error_t
hppe_mac_ctrl2_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union mac_ctrl2_u *value)
{
	if (index >= MAC_CTRL2_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				NSS_MAC_CSR_BASE_ADDR + MAC_CTRL2_ADDRESS + \
				index * MAC_CTRL2_INC,
				&value->val);
}

sw_error_t
hppe_mac_ctrl2_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union mac_ctrl2_u *value)
{
	return hppe_reg_set(
				dev_id,
				NSS_MAC_CSR_BASE_ADDR + MAC_CTRL2_ADDRESS + \
				index * MAC_CTRL2_INC,
				value->val);
}

sw_error_t
hppe_mac_dbg_ctrl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union mac_dbg_ctrl_u *value)
{
	if (index >= MAC_DBG_CTRL_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				NSS_MAC_CSR_BASE_ADDR + MAC_DBG_CTRL_ADDRESS + \
				index * MAC_DBG_CTRL_INC,
				&value->val);
}

sw_error_t
hppe_mac_dbg_ctrl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union mac_dbg_ctrl_u *value)
{
	return hppe_reg_set(
				dev_id,
				NSS_MAC_CSR_BASE_ADDR + MAC_DBG_CTRL_ADDRESS + \
				index * MAC_DBG_CTRL_INC,
				value->val);
}
#ifndef IN_PORTCONTROL_MINI
sw_error_t
hppe_mac_dbg_addr_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union mac_dbg_addr_u *value)
{
	if (index >= MAC_DBG_ADDR_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				NSS_MAC_CSR_BASE_ADDR + MAC_DBG_ADDR_ADDRESS + \
				index * MAC_DBG_ADDR_INC,
				&value->val);
}

sw_error_t
hppe_mac_dbg_addr_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union mac_dbg_addr_u *value)
{
	return hppe_reg_set(
				dev_id,
				NSS_MAC_CSR_BASE_ADDR + MAC_DBG_ADDR_ADDRESS + \
				index * MAC_DBG_ADDR_INC,
				value->val);
}

sw_error_t
hppe_mac_dbg_data_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union mac_dbg_data_u *value)
{
	if (index >= MAC_DBG_DATA_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				NSS_MAC_CSR_BASE_ADDR + MAC_DBG_DATA_ADDRESS + \
				index * MAC_DBG_DATA_INC,
				&value->val);
}

sw_error_t
hppe_mac_dbg_data_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union mac_dbg_data_u *value)
{
	return SW_NOT_SUPPORTED;
}
#endif

sw_error_t
hppe_mac_jumbo_size_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union mac_jumbo_size_u *value)
{
	if (index >= MAC_JUMBO_SIZE_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				NSS_MAC_CSR_BASE_ADDR + MAC_JUMBO_SIZE_ADDRESS + \
				index * MAC_JUMBO_SIZE_INC,
				&value->val);
}

sw_error_t
hppe_mac_jumbo_size_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union mac_jumbo_size_u *value)
{
	return hppe_reg_set(
				dev_id,
				NSS_MAC_CSR_BASE_ADDR + MAC_JUMBO_SIZE_ADDRESS + \
				index * MAC_JUMBO_SIZE_INC,
				value->val);
}

sw_error_t
hppe_mru_mtu_ctrl_tbl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union mru_mtu_ctrl_tbl_u *value)
{
	return hppe_reg_tbl_get(
				dev_id,
				IPE_L2_BASE_ADDR + MRU_MTU_CTRL_TBL_ADDRESS + \
				index * MRU_MTU_CTRL_TBL_INC,
				value->val,
				2);
}

sw_error_t
hppe_mru_mtu_ctrl_tbl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union mru_mtu_ctrl_tbl_u *value)
{
	return hppe_reg_tbl_set(
				dev_id,
				IPE_L2_BASE_ADDR + MRU_MTU_CTRL_TBL_ADDRESS + \
				index * MRU_MTU_CTRL_TBL_INC,
				value->val,
				2);
}

#if ((!defined(IN_PORTCONTROL_MINI)) || (!defined(IN_MISC_MINI)))
sw_error_t
hppe_mc_mtu_ctrl_tbl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union mc_mtu_ctrl_tbl_u *value)
{
	if (index >= MC_MTU_CTRL_TBL_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				IPE_L2_BASE_ADDR + MC_MTU_CTRL_TBL_ADDRESS + \
				index * MC_MTU_CTRL_TBL_INC,
				&value->val);
}

sw_error_t
hppe_mc_mtu_ctrl_tbl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union mc_mtu_ctrl_tbl_u *value)
{
	return hppe_reg_set(
				dev_id,
				IPE_L2_BASE_ADDR + MC_MTU_CTRL_TBL_ADDRESS + \
				index * MC_MTU_CTRL_TBL_INC,
				value->val);
}
#endif

sw_error_t
hppe_tdm_ctrl_get(
		a_uint32_t dev_id,
		union tdm_ctrl_u *value)
{
	return hppe_reg_get(
				dev_id,
				NSS_PRX_CSR_BASE_ADDR + TDM_CTRL_ADDRESS,
				&value->val);
}

sw_error_t
hppe_tdm_ctrl_set(
		a_uint32_t dev_id,
		union tdm_ctrl_u *value)
{
	return hppe_reg_set(
				dev_id,
				NSS_PRX_CSR_BASE_ADDR + TDM_CTRL_ADDRESS,
				value->val);
}
#ifndef IN_PORTCONTROL_MINI
sw_error_t
hppe_rx_fifo_cfg_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rx_fifo_cfg_u *value)
{
	if (index >= RX_FIFO_CFG_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				NSS_PRX_CSR_BASE_ADDR + RX_FIFO_CFG_ADDRESS + \
				index * RX_FIFO_CFG_INC,
				&value->val);
}

sw_error_t
hppe_rx_fifo_cfg_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rx_fifo_cfg_u *value)
{
	return hppe_reg_set(
				dev_id,
				NSS_PRX_CSR_BASE_ADDR + RX_FIFO_CFG_ADDRESS + \
				index * RX_FIFO_CFG_INC,
				value->val);
}

sw_error_t
hppe_tdm_cfg_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union tdm_cfg_u *value)
{
	if (index >= TDM_CFG_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				NSS_PRX_CSR_BASE_ADDR + TDM_CFG_ADDRESS + \
				index * TDM_CFG_INC,
				&value->val);
}
#endif

sw_error_t
hppe_tdm_cfg_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union tdm_cfg_u *value)
{
	return hppe_reg_set(
				dev_id,
				NSS_PRX_CSR_BASE_ADDR + TDM_CFG_ADDRESS + \
				index * TDM_CFG_INC,
				value->val);
}

sw_error_t
hppe_drop_stat_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union drop_stat_u *value)
{
	return hppe_reg_tbl_get(
				dev_id,
				NSS_PRX_CSR_BASE_ADDR + DROP_STAT_ADDRESS + \
				index * DROP_STAT_INC,
				value->val,
				3);
}

#ifndef IN_MISC_MINI
sw_error_t
hppe_drop_stat_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union drop_stat_u *value)
{
	return hppe_reg_tbl_set(
				dev_id,
				NSS_PRX_CSR_BASE_ADDR + DROP_STAT_ADDRESS + \
				index * DROP_STAT_INC,
				value->val,
				3);
}
#endif

#ifndef IN_PORTCONTROL_MINI
sw_error_t
hppe_mac_enable_txmac_en_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mac_enable_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_enable_get(dev_id, index, &reg_val);
	*value = reg_val.bf.txmac_en;
	return ret;
}

sw_error_t
hppe_mac_enable_txmac_en_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union mac_enable_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_enable_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.txmac_en = value;
	ret = hppe_mac_enable_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_mac_enable_rxmac_en_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mac_enable_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_enable_get(dev_id, index, &reg_val);
	*value = reg_val.bf.rxmac_en;
	return ret;
}

sw_error_t
hppe_mac_enable_rxmac_en_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union mac_enable_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_enable_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.rxmac_en = value;
	ret = hppe_mac_enable_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_mac_enable_tx_flow_en_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mac_enable_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_enable_get(dev_id, index, &reg_val);
	*value = reg_val.bf.tx_flow_en;
	return ret;
}

sw_error_t
hppe_mac_enable_tx_flow_en_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union mac_enable_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_enable_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.tx_flow_en = value;
	ret = hppe_mac_enable_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_mac_enable_rx_flow_en_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mac_enable_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_enable_get(dev_id, index, &reg_val);
	*value = reg_val.bf.rx_flow_en;
	return ret;
}

sw_error_t
hppe_mac_enable_rx_flow_en_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union mac_enable_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_enable_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.rx_flow_en = value;
	ret = hppe_mac_enable_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_mac_enable_duplex_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mac_enable_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_enable_get(dev_id, index, &reg_val);
	*value = reg_val.bf.duplex;
	return ret;
}

sw_error_t
hppe_mac_enable_duplex_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union mac_enable_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_enable_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.duplex = value;
	ret = hppe_mac_enable_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_mac_speed_mac_speed_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mac_speed_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_speed_get(dev_id, index, &reg_val);
	*value = reg_val.bf.mac_speed;
	return ret;
}

sw_error_t
hppe_mac_speed_mac_speed_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union mac_speed_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_speed_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.mac_speed = value;
	ret = hppe_mac_speed_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_gol_mac_addr0_mac_addr_byte4_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union gol_mac_addr0_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_gol_mac_addr0_get(dev_id, index, &reg_val);
	*value = reg_val.bf.mac_addr_byte4;
	return ret;
}

sw_error_t
hppe_gol_mac_addr0_mac_addr_byte4_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union gol_mac_addr0_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_gol_mac_addr0_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.mac_addr_byte4 = value;
	ret = hppe_gol_mac_addr0_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_gol_mac_addr0_mac_addr_byte5_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union gol_mac_addr0_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_gol_mac_addr0_get(dev_id, index, &reg_val);
	*value = reg_val.bf.mac_addr_byte5;
	return ret;
}

sw_error_t
hppe_gol_mac_addr0_mac_addr_byte5_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union gol_mac_addr0_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_gol_mac_addr0_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.mac_addr_byte5 = value;
	ret = hppe_gol_mac_addr0_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_gol_mac_addr1_mac_addr_byte1_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union gol_mac_addr1_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_gol_mac_addr1_get(dev_id, index, &reg_val);
	*value = reg_val.bf.mac_addr_byte1;
	return ret;
}

sw_error_t
hppe_gol_mac_addr1_mac_addr_byte1_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union gol_mac_addr1_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_gol_mac_addr1_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.mac_addr_byte1 = value;
	ret = hppe_gol_mac_addr1_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_gol_mac_addr1_mac_addr_byte2_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union gol_mac_addr1_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_gol_mac_addr1_get(dev_id, index, &reg_val);
	*value = reg_val.bf.mac_addr_byte2;
	return ret;
}

sw_error_t
hppe_gol_mac_addr1_mac_addr_byte2_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union gol_mac_addr1_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_gol_mac_addr1_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.mac_addr_byte2 = value;
	ret = hppe_gol_mac_addr1_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_gol_mac_addr1_mac_addr_byte0_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union gol_mac_addr1_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_gol_mac_addr1_get(dev_id, index, &reg_val);
	*value = reg_val.bf.mac_addr_byte0;
	return ret;
}

sw_error_t
hppe_gol_mac_addr1_mac_addr_byte0_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union gol_mac_addr1_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_gol_mac_addr1_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.mac_addr_byte0 = value;
	ret = hppe_gol_mac_addr1_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_gol_mac_addr1_mac_addr_byte3_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union gol_mac_addr1_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_gol_mac_addr1_get(dev_id, index, &reg_val);
	*value = reg_val.bf.mac_addr_byte3;
	return ret;
}

sw_error_t
hppe_gol_mac_addr1_mac_addr_byte3_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union gol_mac_addr1_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_gol_mac_addr1_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.mac_addr_byte3 = value;
	ret = hppe_gol_mac_addr1_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_mac_ctrl0_amaxc_en_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mac_ctrl0_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_ctrl0_get(dev_id, index, &reg_val);
	*value = reg_val.bf.amaxc_en;
	return ret;
}

sw_error_t
hppe_mac_ctrl0_amaxc_en_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union mac_ctrl0_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_ctrl0_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.amaxc_en = value;
	ret = hppe_mac_ctrl0_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_mac_ctrl0_ipgt_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mac_ctrl0_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_ctrl0_get(dev_id, index, &reg_val);
	*value = reg_val.bf.ipgt;
	return ret;
}

sw_error_t
hppe_mac_ctrl0_ipgt_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union mac_ctrl0_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_ctrl0_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ipgt = value;
	ret = hppe_mac_ctrl0_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_mac_ctrl0_nobo_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mac_ctrl0_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_ctrl0_get(dev_id, index, &reg_val);
	*value = reg_val.bf.nobo;
	return ret;
}

sw_error_t
hppe_mac_ctrl0_nobo_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union mac_ctrl0_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_ctrl0_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.nobo = value;
	ret = hppe_mac_ctrl0_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_mac_ctrl0_half_thdf_ctrl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mac_ctrl0_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_ctrl0_get(dev_id, index, &reg_val);
	*value = reg_val.bf.half_thdf_ctrl;
	return ret;
}

sw_error_t
hppe_mac_ctrl0_half_thdf_ctrl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union mac_ctrl0_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_ctrl0_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.half_thdf_ctrl = value;
	ret = hppe_mac_ctrl0_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_mac_ctrl0_hugen_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mac_ctrl0_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_ctrl0_get(dev_id, index, &reg_val);
	*value = reg_val.bf.hugen;
	return ret;
}

sw_error_t
hppe_mac_ctrl0_hugen_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union mac_ctrl0_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_ctrl0_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.hugen = value;
	ret = hppe_mac_ctrl0_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_mac_ctrl0_bpnb_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mac_ctrl0_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_ctrl0_get(dev_id, index, &reg_val);
	*value = reg_val.bf.bpnb;
	return ret;
}

sw_error_t
hppe_mac_ctrl0_bpnb_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union mac_ctrl0_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_ctrl0_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.bpnb = value;
	ret = hppe_mac_ctrl0_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_mac_ctrl0_flchk_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mac_ctrl0_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_ctrl0_get(dev_id, index, &reg_val);
	*value = reg_val.bf.flchk;
	return ret;
}

sw_error_t
hppe_mac_ctrl0_flchk_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union mac_ctrl0_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_ctrl0_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.flchk = value;
	ret = hppe_mac_ctrl0_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_mac_ctrl0_ipgr2_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mac_ctrl0_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_ctrl0_get(dev_id, index, &reg_val);
	*value = reg_val.bf.ipgr2;
	return ret;
}

sw_error_t
hppe_mac_ctrl0_ipgr2_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union mac_ctrl0_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_ctrl0_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ipgr2 = value;
	ret = hppe_mac_ctrl0_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_mac_ctrl0_drbnib_rxok_en_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mac_ctrl0_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_ctrl0_get(dev_id, index, &reg_val);
	*value = reg_val.bf.drbnib_rxok_en;
	return ret;
}

sw_error_t
hppe_mac_ctrl0_drbnib_rxok_en_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union mac_ctrl0_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_ctrl0_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.drbnib_rxok_en = value;
	ret = hppe_mac_ctrl0_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_mac_ctrl0_huge_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mac_ctrl0_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_ctrl0_get(dev_id, index, &reg_val);
	*value = reg_val.bf.huge;
	return ret;
}

sw_error_t
hppe_mac_ctrl0_huge_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union mac_ctrl0_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_ctrl0_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.huge = value;
	ret = hppe_mac_ctrl0_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_mac_ctrl0_abebe_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mac_ctrl0_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_ctrl0_get(dev_id, index, &reg_val);
	*value = reg_val.bf.abebe;
	return ret;
}

sw_error_t
hppe_mac_ctrl0_abebe_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union mac_ctrl0_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_ctrl0_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.abebe = value;
	ret = hppe_mac_ctrl0_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_mac_ctrl1_povr_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mac_ctrl1_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_ctrl1_get(dev_id, index, &reg_val);
	*value = reg_val.bf.povr;
	return ret;
}

sw_error_t
hppe_mac_ctrl1_povr_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union mac_ctrl1_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_ctrl1_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.povr = value;
	ret = hppe_mac_ctrl1_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_mac_ctrl1_simr_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mac_ctrl1_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_ctrl1_get(dev_id, index, &reg_val);
	*value = reg_val.bf.simr;
	return ret;
}

sw_error_t
hppe_mac_ctrl1_simr_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union mac_ctrl1_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_ctrl1_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.simr = value;
	ret = hppe_mac_ctrl1_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_mac_ctrl1_jam_ipg_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mac_ctrl1_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_ctrl1_get(dev_id, index, &reg_val);
	*value = reg_val.bf.jam_ipg;
	return ret;
}

sw_error_t
hppe_mac_ctrl1_jam_ipg_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union mac_ctrl1_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_ctrl1_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.jam_ipg = value;
	ret = hppe_mac_ctrl1_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_mac_ctrl1_lcol_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mac_ctrl1_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_ctrl1_get(dev_id, index, &reg_val);
	*value = reg_val.bf.lcol;
	return ret;
}

sw_error_t
hppe_mac_ctrl1_lcol_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union mac_ctrl1_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_ctrl1_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.lcol = value;
	ret = hppe_mac_ctrl1_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_mac_ctrl1_tctl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mac_ctrl1_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_ctrl1_get(dev_id, index, &reg_val);
	*value = reg_val.bf.tctl;
	return ret;
}

sw_error_t
hppe_mac_ctrl1_tctl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union mac_ctrl1_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_ctrl1_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.tctl = value;
	ret = hppe_mac_ctrl1_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_mac_ctrl1_retry_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mac_ctrl1_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_ctrl1_get(dev_id, index, &reg_val);
	*value = reg_val.bf.retry;
	return ret;
}

sw_error_t
hppe_mac_ctrl1_retry_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union mac_ctrl1_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_ctrl1_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.retry = value;
	ret = hppe_mac_ctrl1_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_mac_ctrl1_prlen_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mac_ctrl1_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_ctrl1_get(dev_id, index, &reg_val);
	*value = reg_val.bf.prlen;
	return ret;
}

sw_error_t
hppe_mac_ctrl1_prlen_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union mac_ctrl1_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_ctrl1_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.prlen = value;
	ret = hppe_mac_ctrl1_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_mac_ctrl1_ppad_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mac_ctrl1_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_ctrl1_get(dev_id, index, &reg_val);
	*value = reg_val.bf.ppad;
	return ret;
}

sw_error_t
hppe_mac_ctrl1_ppad_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union mac_ctrl1_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_ctrl1_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ppad = value;
	ret = hppe_mac_ctrl1_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_mac_ctrl1_long_jam_en_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mac_ctrl1_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_ctrl1_get(dev_id, index, &reg_val);
	*value = reg_val.bf.long_jam_en;
	return ret;
}

sw_error_t
hppe_mac_ctrl1_long_jam_en_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union mac_ctrl1_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_ctrl1_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.long_jam_en = value;
	ret = hppe_mac_ctrl1_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_mac_ctrl1_phug_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mac_ctrl1_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_ctrl1_get(dev_id, index, &reg_val);
	*value = reg_val.bf.phug;
	return ret;
}

sw_error_t
hppe_mac_ctrl1_phug_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union mac_ctrl1_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_ctrl1_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.phug = value;
	ret = hppe_mac_ctrl1_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_mac_ctrl1_sstct_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mac_ctrl1_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_ctrl1_get(dev_id, index, &reg_val);
	*value = reg_val.bf.sstct;
	return ret;
}

sw_error_t
hppe_mac_ctrl1_sstct_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union mac_ctrl1_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_ctrl1_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.sstct = value;
	ret = hppe_mac_ctrl1_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_mac_ctrl1_mbof_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mac_ctrl1_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_ctrl1_get(dev_id, index, &reg_val);
	*value = reg_val.bf.mbof;
	return ret;
}

sw_error_t
hppe_mac_ctrl1_mbof_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union mac_ctrl1_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_ctrl1_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.mbof = value;
	ret = hppe_mac_ctrl1_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_mac_ctrl1_tpause_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mac_ctrl1_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_ctrl1_get(dev_id, index, &reg_val);
	*value = reg_val.bf.tpause;
	return ret;
}

sw_error_t
hppe_mac_ctrl1_tpause_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union mac_ctrl1_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_ctrl1_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.tpause = value;
	ret = hppe_mac_ctrl1_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_mac_ctrl2_ipg_dec_en_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mac_ctrl2_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_ctrl2_get(dev_id, index, &reg_val);
	*value = reg_val.bf.ipg_dec_en;
	return ret;
}

sw_error_t
hppe_mac_ctrl2_ipg_dec_en_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union mac_ctrl2_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_ctrl2_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ipg_dec_en = value;
	ret = hppe_mac_ctrl2_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_mac_ctrl2_mac_rsv_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mac_ctrl2_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_ctrl2_get(dev_id, index, &reg_val);
	*value = reg_val.bf.mac_rsv;
	return ret;
}

sw_error_t
hppe_mac_ctrl2_mac_rsv_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union mac_ctrl2_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_ctrl2_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.mac_rsv = value;
	ret = hppe_mac_ctrl2_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_mac_ctrl2_mac_tx_thd_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mac_ctrl2_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_ctrl2_get(dev_id, index, &reg_val);
	*value = reg_val.bf.mac_tx_thd;
	return ret;
}
#endif
sw_error_t
hppe_mac_ctrl2_mac_tx_thd_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union mac_ctrl2_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_ctrl2_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.mac_tx_thd = value;
	ret = hppe_mac_ctrl2_set(dev_id, index, &reg_val);
	return ret;
}
#ifndef IN_PORTCONTROL_MINI
sw_error_t
hppe_mac_ctrl2_crc_rsv_en_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mac_ctrl2_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_ctrl2_get(dev_id, index, &reg_val);
	*value = reg_val.bf.crc_rsv_en;
	return ret;
}

sw_error_t
hppe_mac_ctrl2_crc_rsv_en_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union mac_ctrl2_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_ctrl2_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.crc_rsv_en = value;
	ret = hppe_mac_ctrl2_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_mac_ctrl2_crs_sel_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mac_ctrl2_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_ctrl2_get(dev_id, index, &reg_val);
	*value = reg_val.bf.crs_sel;
	return ret;
}
#endif
sw_error_t
hppe_mac_ctrl2_crs_sel_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union mac_ctrl2_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_ctrl2_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.crs_sel = value;
	ret = hppe_mac_ctrl2_set(dev_id, index, &reg_val);
	return ret;
}
#ifndef IN_PORTCONTROL_MINI
sw_error_t
hppe_mac_ctrl2_ipg_dec_len_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mac_ctrl2_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_ctrl2_get(dev_id, index, &reg_val);
	*value = reg_val.bf.ipg_dec_len;
	return ret;
}

sw_error_t
hppe_mac_ctrl2_ipg_dec_len_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union mac_ctrl2_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_ctrl2_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ipg_dec_len = value;
	ret = hppe_mac_ctrl2_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_mac_ctrl2_maxfr_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mac_ctrl2_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_ctrl2_get(dev_id, index, &reg_val);
	*value = reg_val.bf.maxfr;
	return ret;
}
#endif
sw_error_t
hppe_mac_ctrl2_maxfr_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union mac_ctrl2_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_ctrl2_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.maxfr = value;
	ret = hppe_mac_ctrl2_set(dev_id, index, &reg_val);
	return ret;
}
#ifndef IN_PORTCONTROL_MINI

sw_error_t
hppe_mac_ctrl2_mac_lpi_tx_idle_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mac_ctrl2_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_ctrl2_get(dev_id, index, &reg_val);
	*value = reg_val.bf.mac_lpi_tx_idle;
	return ret;
}

sw_error_t
hppe_mac_ctrl2_mac_lpi_tx_idle_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union mac_ctrl2_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_ctrl2_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.mac_lpi_tx_idle = value;
	ret = hppe_mac_ctrl2_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_mac_ctrl2_mac_loop_back_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mac_ctrl2_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_ctrl2_get(dev_id, index, &reg_val);
	*value = reg_val.bf.mac_loop_back;
	return ret;
}

sw_error_t
hppe_mac_ctrl2_mac_loop_back_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union mac_ctrl2_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_ctrl2_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.mac_loop_back = value;
	ret = hppe_mac_ctrl2_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_mac_ctrl2_test_pause_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mac_ctrl2_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_ctrl2_get(dev_id, index, &reg_val);
	*value = reg_val.bf.test_pause;
	return ret;
}

sw_error_t
hppe_mac_ctrl2_test_pause_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union mac_ctrl2_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_ctrl2_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.test_pause = value;
	ret = hppe_mac_ctrl2_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_mac_dbg_ctrl_edxsdfr_transmit_en_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mac_dbg_ctrl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_dbg_ctrl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.edxsdfr_transmit_en;
	return ret;
}

sw_error_t
hppe_mac_dbg_ctrl_edxsdfr_transmit_en_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union mac_dbg_ctrl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_dbg_ctrl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.edxsdfr_transmit_en = value;
	ret = hppe_mac_dbg_ctrl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_mac_dbg_ctrl_hihg_ipg_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mac_dbg_ctrl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_dbg_ctrl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.hihg_ipg;
	return ret;
}
#endif
sw_error_t
hppe_mac_dbg_ctrl_hihg_ipg_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union mac_dbg_ctrl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_dbg_ctrl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.hihg_ipg = value;
	ret = hppe_mac_dbg_ctrl_set(dev_id, index, &reg_val);
	return ret;
}
#ifndef IN_PORTCONTROL_MINI
sw_error_t
hppe_mac_dbg_ctrl_mac_ipg_ctrl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mac_dbg_ctrl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_dbg_ctrl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.mac_ipg_ctrl;
	return ret;
}

sw_error_t
hppe_mac_dbg_ctrl_mac_ipg_ctrl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union mac_dbg_ctrl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_dbg_ctrl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.mac_ipg_ctrl = value;
	ret = hppe_mac_dbg_ctrl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_mac_dbg_ctrl_mac_len_ctrl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mac_dbg_ctrl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_dbg_ctrl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.mac_len_ctrl;
	return ret;
}

sw_error_t
hppe_mac_dbg_ctrl_mac_len_ctrl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union mac_dbg_ctrl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_dbg_ctrl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.mac_len_ctrl = value;
	ret = hppe_mac_dbg_ctrl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_mac_dbg_ctrl_ipgr1_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mac_dbg_ctrl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_dbg_ctrl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.ipgr1;
	return ret;
}

sw_error_t
hppe_mac_dbg_ctrl_ipgr1_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union mac_dbg_ctrl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_dbg_ctrl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ipgr1 = value;
	ret = hppe_mac_dbg_ctrl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_mac_dbg_addr_mac_debug_addr_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mac_dbg_addr_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_dbg_addr_get(dev_id, index, &reg_val);
	*value = reg_val.bf.mac_debug_addr;
	return ret;
}

sw_error_t
hppe_mac_dbg_addr_mac_debug_addr_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union mac_dbg_addr_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_dbg_addr_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.mac_debug_addr = value;
	ret = hppe_mac_dbg_addr_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_mac_dbg_data_mac_debug_data_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mac_dbg_data_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_dbg_data_get(dev_id, index, &reg_val);
	*value = reg_val.bf.mac_debug_data;
	return ret;
}

sw_error_t
hppe_mac_dbg_data_mac_debug_data_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}
#endif
sw_error_t
hppe_mac_jumbo_size_mac_jumbo_size_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mac_jumbo_size_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_jumbo_size_get(dev_id, index, &reg_val);
	*value = reg_val.bf.mac_jumbo_size;
	return ret;
}

sw_error_t
hppe_mac_jumbo_size_mac_jumbo_size_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union mac_jumbo_size_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_jumbo_size_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.mac_jumbo_size = value;
	ret = hppe_mac_jumbo_size_set(dev_id, index, &reg_val);
	return ret;
}
#ifndef IN_PORTCONTROL_MINI
sw_error_t
hppe_mru_mtu_ctrl_tbl_mtu_cmd_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mru_mtu_ctrl_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mru_mtu_ctrl_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.mtu_cmd;
	return ret;
}

sw_error_t
hppe_mru_mtu_ctrl_tbl_mtu_cmd_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union mru_mtu_ctrl_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mru_mtu_ctrl_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.mtu_cmd = value;
	ret = hppe_mru_mtu_ctrl_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_mru_mtu_ctrl_tbl_rx_cnt_en_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mru_mtu_ctrl_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mru_mtu_ctrl_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.rx_cnt_en;
	return ret;
}

sw_error_t
hppe_mru_mtu_ctrl_tbl_rx_cnt_en_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union mru_mtu_ctrl_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mru_mtu_ctrl_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.rx_cnt_en = value;
	ret = hppe_mru_mtu_ctrl_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_mru_mtu_ctrl_tbl_tx_cnt_en_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mru_mtu_ctrl_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mru_mtu_ctrl_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.tx_cnt_en;
	return ret;
}

sw_error_t
hppe_mru_mtu_ctrl_tbl_tx_cnt_en_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union mru_mtu_ctrl_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mru_mtu_ctrl_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.tx_cnt_en = value;
	ret = hppe_mru_mtu_ctrl_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_mru_mtu_ctrl_tbl_mru_cmd_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mru_mtu_ctrl_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mru_mtu_ctrl_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.mru_cmd;
	return ret;
}

sw_error_t
hppe_mru_mtu_ctrl_tbl_mru_cmd_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union mru_mtu_ctrl_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mru_mtu_ctrl_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.mru_cmd = value;
	ret = hppe_mru_mtu_ctrl_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_mru_mtu_ctrl_tbl_mru_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mru_mtu_ctrl_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mru_mtu_ctrl_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.mru;
	return ret;
}

sw_error_t
hppe_mru_mtu_ctrl_tbl_mru_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union mru_mtu_ctrl_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mru_mtu_ctrl_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.mru = value;
	ret = hppe_mru_mtu_ctrl_tbl_set(dev_id, index, &reg_val);
	return ret;
}
#endif

sw_error_t
hppe_mru_mtu_ctrl_tbl_src_profile_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mru_mtu_ctrl_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mru_mtu_ctrl_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.src_profile;
	return ret;
}

sw_error_t
hppe_mru_mtu_ctrl_tbl_src_profile_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union mru_mtu_ctrl_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mru_mtu_ctrl_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.src_profile = value;
	ret = hppe_mru_mtu_ctrl_tbl_set(dev_id, index, &reg_val);
	return ret;
}

#ifndef IN_PORTCONTROL_MINI
sw_error_t
hppe_mc_mtu_ctrl_tbl_mtu_cmd_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mc_mtu_ctrl_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mc_mtu_ctrl_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.mtu_cmd;
	return ret;
}

sw_error_t
hppe_mc_mtu_ctrl_tbl_mtu_cmd_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union mc_mtu_ctrl_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mc_mtu_ctrl_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.mtu_cmd = value;
	ret = hppe_mc_mtu_ctrl_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_mc_mtu_ctrl_tbl_tx_cnt_en_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mc_mtu_ctrl_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mc_mtu_ctrl_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.tx_cnt_en;
	return ret;
}

sw_error_t
hppe_mc_mtu_ctrl_tbl_tx_cnt_en_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union mc_mtu_ctrl_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mc_mtu_ctrl_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.tx_cnt_en = value;
	ret = hppe_mc_mtu_ctrl_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_mc_mtu_ctrl_tbl_mtu_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mc_mtu_ctrl_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mc_mtu_ctrl_tbl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.mtu;
	return ret;
}

sw_error_t
hppe_mc_mtu_ctrl_tbl_mtu_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union mc_mtu_ctrl_tbl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mc_mtu_ctrl_tbl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.mtu = value;
	ret = hppe_mc_mtu_ctrl_tbl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_tdm_ctrl_tdm_en_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union tdm_ctrl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_tdm_ctrl_get(dev_id, &reg_val);
	*value = reg_val.bf.tdm_en;
	return ret;
}

sw_error_t
hppe_tdm_ctrl_tdm_en_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union tdm_ctrl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_tdm_ctrl_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.tdm_en = value;
	ret = hppe_tdm_ctrl_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_tdm_ctrl_tdm_offset_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union tdm_ctrl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_tdm_ctrl_get(dev_id, &reg_val);
	*value = reg_val.bf.tdm_offset;
	return ret;
}

sw_error_t
hppe_tdm_ctrl_tdm_offset_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union tdm_ctrl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_tdm_ctrl_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.tdm_offset = value;
	ret = hppe_tdm_ctrl_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_tdm_ctrl_tdm_depth_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union tdm_ctrl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_tdm_ctrl_get(dev_id, &reg_val);
	*value = reg_val.bf.tdm_depth;
	return ret;
}

sw_error_t
hppe_tdm_ctrl_tdm_depth_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union tdm_ctrl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_tdm_ctrl_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.tdm_depth = value;
	ret = hppe_tdm_ctrl_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_rx_fifo_cfg_rx_fifo_thres_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union rx_fifo_cfg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_rx_fifo_cfg_get(dev_id, index, &reg_val);
	*value = reg_val.bf.rx_fifo_thres;
	return ret;
}

sw_error_t
hppe_rx_fifo_cfg_rx_fifo_thres_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union rx_fifo_cfg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_rx_fifo_cfg_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.rx_fifo_thres = value;
	ret = hppe_rx_fifo_cfg_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_tdm_cfg_port_num_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union tdm_cfg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_tdm_cfg_get(dev_id, index, &reg_val);
	*value = reg_val.bf.port_num;
	return ret;
}

sw_error_t
hppe_tdm_cfg_port_num_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union tdm_cfg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_tdm_cfg_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.port_num = value;
	ret = hppe_tdm_cfg_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_tdm_cfg_valid_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union tdm_cfg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_tdm_cfg_get(dev_id, index, &reg_val);
	*value = reg_val.bf.valid;
	return ret;
}

sw_error_t
hppe_tdm_cfg_valid_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union tdm_cfg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_tdm_cfg_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.valid = value;
	ret = hppe_tdm_cfg_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_tdm_cfg_dir_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union tdm_cfg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_tdm_cfg_get(dev_id, index, &reg_val);
	*value = reg_val.bf.dir;
	return ret;
}

sw_error_t
hppe_tdm_cfg_dir_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union tdm_cfg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_tdm_cfg_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.dir = value;
	ret = hppe_tdm_cfg_set(dev_id, index, &reg_val);
	return ret;
}
#endif
sw_error_t
hppe_port_in_forward_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union port_in_forward_u *value)
{
	if (index >= PORT_IN_FORWARD_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				IPE_L2_BASE_ADDR + PORT_IN_FORWARD_ADDRESS + \
				index * PORT_IN_FORWARD_INC,
				&value->val);
}

sw_error_t
hppe_port_in_forward_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union port_in_forward_u *value)
{
	return hppe_reg_set(
				dev_id,
				IPE_L2_BASE_ADDR + PORT_IN_FORWARD_ADDRESS + \
				index * PORT_IN_FORWARD_INC,
				value->val);
}
#ifndef IN_PORTCONTROL_MINI
sw_error_t
hppe_port_in_forward_source_filtering_bypass_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union port_in_forward_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_port_in_forward_get(dev_id, index, &reg_val);
	*value = reg_val.bf.source_filtering_bypass;
	return ret;
}

sw_error_t
hppe_port_in_forward_source_filtering_bypass_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union port_in_forward_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_port_in_forward_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.source_filtering_bypass = value;
	ret = hppe_port_in_forward_set(dev_id, index, &reg_val);
	return ret;
}
#endif

#ifndef IN_MISC_MINI
sw_error_t
hppe_drop_stat_bytes_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint64_t *value)
{
	union drop_stat_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_drop_stat_get(dev_id, index, &reg_val);
	*value = (a_uint64_t)reg_val.bf.bytes_1 << 32 | \
		reg_val.bf.bytes_0;
	return ret;
}

sw_error_t
hppe_drop_stat_bytes_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint64_t value)
{
	union drop_stat_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_drop_stat_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.bytes_1 = value >> 32;
	reg_val.bf.bytes_0 = value & (((a_uint64_t)1<<32)-1);
	ret = hppe_drop_stat_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_drop_stat_pkts_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union drop_stat_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_drop_stat_get(dev_id, index, &reg_val);
	*value = reg_val.bf.pkts;
	return ret;
}

sw_error_t
hppe_drop_stat_pkts_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union drop_stat_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_drop_stat_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.pkts = value;
	ret = hppe_drop_stat_set(dev_id, index, &reg_val);
	return ret;
}
#endif

#if ((!defined(IN_PORTCONTROL_MINI)) || (!defined(IN_MISC_MINI)))
sw_error_t
hppe_port_tx_counter_tbl_reg_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union port_tx_counter_tbl_reg_u *value)
{
	return hppe_reg_tbl_get(
				dev_id,
				NSS_PTX_CSR_BASE_ADDR + PORT_TX_COUNTER_TBL_REG_ADDRESS + \
				index * PORT_TX_COUNTER_TBL_REG_INC,
				value->val,
				3);
}

sw_error_t
hppe_port_tx_counter_tbl_reg_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union port_tx_counter_tbl_reg_u *value)
{
	return hppe_reg_tbl_set(
				dev_id,
				NSS_PTX_CSR_BASE_ADDR + PORT_TX_COUNTER_TBL_REG_ADDRESS + \
				index * PORT_TX_COUNTER_TBL_REG_INC,
				value->val,
				3);
}

sw_error_t
hppe_vp_tx_counter_tbl_reg_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union vp_tx_counter_tbl_reg_u *value)
{
	return hppe_reg_tbl_get(
				dev_id,
				NSS_PTX_CSR_BASE_ADDR + VP_TX_COUNTER_TBL_REG_ADDRESS + \
				index * VP_TX_COUNTER_TBL_REG_INC,
				value->val,
				3);
}

sw_error_t
hppe_vp_tx_counter_tbl_reg_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union vp_tx_counter_tbl_reg_u *value)
{
	return hppe_reg_tbl_set(
				dev_id,
				NSS_PTX_CSR_BASE_ADDR + VP_TX_COUNTER_TBL_REG_ADDRESS + \
				index * VP_TX_COUNTER_TBL_REG_INC,
				value->val,
				3);
}

sw_error_t
hppe_epe_dbg_in_cnt_reg_get(
		a_uint32_t dev_id,
		union epe_dbg_in_cnt_reg_u *value)
{
	return hppe_reg_get(
				dev_id,
				NSS_PTX_CSR_BASE_ADDR + EPE_DBG_IN_CNT_REG_ADDRESS,
				&value->val);
}

sw_error_t
hppe_epe_dbg_in_cnt_reg_set(
		a_uint32_t dev_id,
		union epe_dbg_in_cnt_reg_u *value)
{
	return hppe_reg_set(
				dev_id,
				NSS_PTX_CSR_BASE_ADDR + EPE_DBG_IN_CNT_REG_ADDRESS,
				value->val);
}

sw_error_t
hppe_epe_dbg_out_cnt_reg_set(
		a_uint32_t dev_id,
		union epe_dbg_out_cnt_reg_u *value)
{
	return hppe_reg_set(
				dev_id,
				NSS_PTX_CSR_BASE_ADDR + EPE_DBG_OUT_CNT_REG_ADDRESS,
				value->val);
}
#endif

#ifndef IN_PORTCONTROL_MINI
sw_error_t
hppe_epe_dbg_out_cnt_reg_get(
		a_uint32_t dev_id,
		union epe_dbg_out_cnt_reg_u *value)
{
	return hppe_reg_get(
				dev_id,
				NSS_PTX_CSR_BASE_ADDR + EPE_DBG_OUT_CNT_REG_ADDRESS,
				&value->val);
}

sw_error_t
hppe_port_tx_counter_tbl_reg_tx_bytes_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint64_t *value)
{
	union port_tx_counter_tbl_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_port_tx_counter_tbl_reg_get(dev_id, index, &reg_val);
	*value = (a_uint64_t)reg_val.bf.tx_bytes_1 << 32 | \
		reg_val.bf.tx_bytes_0;
	return ret;
}

sw_error_t
hppe_port_tx_counter_tbl_reg_tx_bytes_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint64_t value)
{
	union port_tx_counter_tbl_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_port_tx_counter_tbl_reg_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.tx_bytes_1 = value >> 32;
	reg_val.bf.tx_bytes_0 = value & (((a_uint64_t)1<<32)-1);
	ret = hppe_port_tx_counter_tbl_reg_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_port_tx_counter_tbl_reg_tx_packets_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union port_tx_counter_tbl_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_port_tx_counter_tbl_reg_get(dev_id, index, &reg_val);
	*value = reg_val.bf.tx_packets;
	return ret;
}

sw_error_t
hppe_port_tx_counter_tbl_reg_tx_packets_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union port_tx_counter_tbl_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_port_tx_counter_tbl_reg_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.tx_packets = value;
	ret = hppe_port_tx_counter_tbl_reg_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_vp_tx_counter_tbl_reg_tx_bytes_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint64_t *value)
{
	union vp_tx_counter_tbl_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_vp_tx_counter_tbl_reg_get(dev_id, index, &reg_val);
	*value = (a_uint64_t)reg_val.bf.tx_bytes_1 << 32 | \
		reg_val.bf.tx_bytes_0;
	return ret;
}

sw_error_t
hppe_vp_tx_counter_tbl_reg_tx_bytes_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint64_t value)
{
	union vp_tx_counter_tbl_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_vp_tx_counter_tbl_reg_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.tx_bytes_1 = value >> 32;
	reg_val.bf.tx_bytes_0 = value & (((a_uint64_t)1<<32)-1);
	ret = hppe_vp_tx_counter_tbl_reg_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_vp_tx_counter_tbl_reg_tx_packets_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union vp_tx_counter_tbl_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_vp_tx_counter_tbl_reg_get(dev_id, index, &reg_val);
	*value = reg_val.bf.tx_packets;
	return ret;
}

sw_error_t
hppe_vp_tx_counter_tbl_reg_tx_packets_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union vp_tx_counter_tbl_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_vp_tx_counter_tbl_reg_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.tx_packets = value;
	ret = hppe_vp_tx_counter_tbl_reg_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_epe_dbg_in_cnt_reg_counter_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union epe_dbg_in_cnt_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_epe_dbg_in_cnt_reg_get(dev_id, &reg_val);
	*value = reg_val.bf.counter;
	return ret;
}

sw_error_t
hppe_epe_dbg_in_cnt_reg_counter_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union epe_dbg_in_cnt_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_epe_dbg_in_cnt_reg_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.counter = value;
	ret = hppe_epe_dbg_in_cnt_reg_set(dev_id, &reg_val);
	return ret;
}

sw_error_t
hppe_epe_dbg_out_cnt_reg_counter_get(
		a_uint32_t dev_id,
		a_uint32_t *value)
{
	union epe_dbg_out_cnt_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_epe_dbg_out_cnt_reg_get(dev_id, &reg_val);
	*value = reg_val.bf.counter;
	return ret;
}

sw_error_t
hppe_epe_dbg_out_cnt_reg_counter_set(
		a_uint32_t dev_id,
		a_uint32_t value)
{
	union epe_dbg_out_cnt_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_epe_dbg_out_cnt_reg_get(dev_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.counter = value;
	ret = hppe_epe_dbg_out_cnt_reg_set(dev_id, &reg_val);
	return ret;
}
#endif

sw_error_t
hppe_lpi_enable_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union lpi_enable_u *value)
{
	return hppe_reg_get(
				dev_id,
				NSS_LPI_BASE_ADDR + LPI_ENABLE_ADDRESS + \
				index * LPI_ENABLE_INC,
				&value->val);
}

sw_error_t
hppe_lpi_enable_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union lpi_enable_u *value)
{
	return hppe_reg_set(
				dev_id,
				NSS_LPI_BASE_ADDR + LPI_ENABLE_ADDRESS + \
				index * LPI_ENABLE_INC,
				value->val);
}

sw_error_t
hppe_lpi_timer_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union lpi_port_timer_u *value)
{
	return hppe_reg_get(
				dev_id,
				NSS_LPI_BASE_ADDR + LPI_PORT_TIMER_ADDRESS + \
				index * LPI_PORT_TIMER_INC,
				&value->val);
}

sw_error_t
hppe_lpi_timer_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union lpi_port_timer_u *value)
{
	return hppe_reg_set(
				dev_id,
				NSS_LPI_BASE_ADDR + LPI_PORT_TIMER_ADDRESS + \
				index * LPI_PORT_TIMER_INC,
				value->val);
}
#ifndef IN_PORTCONTROL_MINI
sw_error_t
hppe_lpi_dbg_addr_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union lpi_dbg_addr_u *value)
{
	return hppe_reg_get(
				dev_id,
				NSS_LPI_BASE_ADDR + LPI_DBG_ADDR_ADDRESS + \
				index * LPI_DBG_ADDR_INC,
				&value->val);
}

sw_error_t
hppe_lpi_dbg_addr_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union lpi_dbg_addr_u *value)
{
	return hppe_reg_set(
				dev_id,
				NSS_LPI_BASE_ADDR + LPI_DBG_ADDR_ADDRESS + \
				index * LPI_DBG_ADDR_INC,
				value->val);
}

sw_error_t
hppe_lpi_dbg_data_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union lpi_dbg_data_u *value)
{
	return hppe_reg_get(
				dev_id,
				NSS_LPI_BASE_ADDR + LPI_DBG_DATA_ADDRESS + \
				index * LPI_DBG_DATA_INC,
				&value->val);
}

sw_error_t
hppe_lpi_dbg_data_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union lpi_dbg_addr_u *value)
{
	return hppe_reg_set(
				dev_id,
				NSS_LPI_BASE_ADDR + LPI_DBG_DATA_ADDRESS + \
				index * LPI_DBG_DATA_INC,
				value->val);
}
sw_error_t
hppe_lpi_cnt_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union lpi_cnt_u *value)
{
	return hppe_reg_get(
				dev_id,
				NSS_LPI_BASE_ADDR + LPI_CNT_ADDRESS + \
				index * LPI_CNT_INC,
				&value->val);
}

sw_error_t
hppe_lpi_cnt_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union lpi_cnt_u *value)
{
	return hppe_reg_set(
				dev_id,
				NSS_LPI_BASE_ADDR + LPI_CNT_ADDRESS + \
				index * LPI_CNT_INC,
				value->val);
}
#endif

#ifndef IN_MISC_MINI
sw_error_t
hppe_drop_cnt_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union drop_cnt_u *value)
{
	if (index >= DROP_CNT_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				NSS_PRX_CSR_BASE_ADDR + DROP_CNT_ADDRESS + \
				index * DROP_CNT_INC,
				&value->val);
}

sw_error_t
hppe_drop_cnt_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union drop_cnt_u *value)
{
	return hppe_reg_set(
				dev_id,
				NSS_PRX_CSR_BASE_ADDR + DROP_CNT_ADDRESS + \
				index * DROP_CNT_INC,
				value->val);
}

sw_error_t
hppe_drop_cnt_drop_cnt_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union drop_cnt_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_drop_cnt_get(dev_id, index, &reg_val);
	*value = reg_val.bf.drop_cnt;
	return ret;
}

sw_error_t
hppe_drop_cnt_drop_cnt_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union drop_cnt_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_drop_cnt_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.drop_cnt = value;
	ret = hppe_drop_cnt_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_ipr_pkt_num_tbl_reg_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union ipr_pkt_num_tbl_reg_u *value)
{
	if (index >= IPR_PKT_NUM_TBL_REG_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				IPR_CSR_BASE_ADDR + IPR_PKT_NUM_TBL_REG_ADDRESS + \
				index * IPR_PKT_NUM_TBL_REG_INC,
				&value->val);
}

sw_error_t
hppe_ipr_pkt_num_tbl_reg_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union ipr_pkt_num_tbl_reg_u *value)
{
	return hppe_reg_set(
				dev_id,
				IPR_CSR_BASE_ADDR + IPR_PKT_NUM_TBL_REG_ADDRESS + \
				index * IPR_PKT_NUM_TBL_REG_INC,
				value->val);
}

sw_error_t
hppe_ipr_byte_low_reg_reg_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union ipr_byte_low_reg_reg_u *value)
{
	if (index >= IPR_BYTE_LOW_REG_REG_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				IPR_CSR_BASE_ADDR + IPR_BYTE_LOW_REG_REG_ADDRESS + \
				index * IPR_BYTE_LOW_REG_REG_INC,
				&value->val);
}

sw_error_t
hppe_ipr_byte_low_reg_reg_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union ipr_byte_low_reg_reg_u *value)
{
	return hppe_reg_set(
				dev_id,
				IPR_CSR_BASE_ADDR + IPR_BYTE_LOW_REG_REG_ADDRESS + \
				index * IPR_BYTE_LOW_REG_REG_INC,
				value->val);
}

sw_error_t
hppe_ipr_byte_high_reg_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union ipr_byte_high_reg_u *value)
{
	if (index >= IPR_BYTE_HIGH_REG_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				IPR_CSR_BASE_ADDR + IPR_BYTE_HIGH_REG_ADDRESS + \
				index * IPR_BYTE_HIGH_REG_INC,
				&value->val);
}

sw_error_t
hppe_ipr_byte_high_reg_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union ipr_byte_high_reg_u *value)
{
	return hppe_reg_set(
				dev_id,
				IPR_CSR_BASE_ADDR + IPR_BYTE_HIGH_REG_ADDRESS + \
				index * IPR_BYTE_HIGH_REG_INC,
				value->val);
}

sw_error_t
hppe_ipr_pkt_num_tbl_reg_packets_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union ipr_pkt_num_tbl_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ipr_pkt_num_tbl_reg_get(dev_id, index, &reg_val);
	*value = reg_val.bf.packets;
	return ret;
}

sw_error_t
hppe_ipr_pkt_num_tbl_reg_packets_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union ipr_pkt_num_tbl_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ipr_pkt_num_tbl_reg_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.packets = value;
	ret = hppe_ipr_pkt_num_tbl_reg_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_ipr_byte_low_reg_reg_bytes_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union ipr_byte_low_reg_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ipr_byte_low_reg_reg_get(dev_id, index, &reg_val);
	*value = reg_val.bf.bytes;
	return ret;
}

sw_error_t
hppe_ipr_byte_low_reg_reg_bytes_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union ipr_byte_low_reg_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ipr_byte_low_reg_reg_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.bytes = value;
	ret = hppe_ipr_byte_low_reg_reg_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_ipr_byte_high_reg_bytes_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union ipr_byte_high_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ipr_byte_high_reg_get(dev_id, index, &reg_val);
	*value = reg_val.bf.bytes;
	return ret;
}

sw_error_t
hppe_ipr_byte_high_reg_bytes_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union ipr_byte_high_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_ipr_byte_high_reg_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.bytes = value;
	ret = hppe_ipr_byte_high_reg_set(dev_id, index, &reg_val);
	return ret;
}
#endif


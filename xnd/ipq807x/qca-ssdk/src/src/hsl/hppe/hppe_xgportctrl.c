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
#include "hppe_xgportctrl_reg.h"
#include "hppe_xgportctrl.h"

sw_error_t
hppe_mac_tx_configuration_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union mac_tx_configuration_u *value)
{
	if (index >= MAC_TX_CONFIGURATION_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				NSS_XGMAC_CSR_BASE_ADDR + MAC_TX_CONFIGURATION_ADDRESS + \
				index * MAC_TX_CONFIGURATION_INC,
				&value->val);
}

sw_error_t
hppe_mac_tx_configuration_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union mac_tx_configuration_u *value)
{
	return hppe_reg_set(
				dev_id,
				NSS_XGMAC_CSR_BASE_ADDR + MAC_TX_CONFIGURATION_ADDRESS + \
				index * MAC_TX_CONFIGURATION_INC,
				value->val);
}

sw_error_t
hppe_mac_rx_configuration_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union mac_rx_configuration_u *value)
{
	if (index >= MAC_RX_CONFIGURATION_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				NSS_XGMAC_CSR_BASE_ADDR + MAC_RX_CONFIGURATION_ADDRESS + \
				index * MAC_RX_CONFIGURATION_INC,
				&value->val);
}

sw_error_t
hppe_mac_rx_configuration_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union mac_rx_configuration_u *value)
{
	return hppe_reg_set(
				dev_id,
				NSS_XGMAC_CSR_BASE_ADDR + MAC_RX_CONFIGURATION_ADDRESS + \
				index * MAC_RX_CONFIGURATION_INC,
				value->val);
}

sw_error_t
hppe_mac_packet_filter_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union mac_packet_filter_u *value)
{
	if (index >= MAC_PACKET_FILTER_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				NSS_XGMAC_CSR_BASE_ADDR + MAC_PACKET_FILTER_ADDRESS + \
				index * MAC_PACKET_FILTER_INC,
				&value->val);
}

sw_error_t
hppe_mac_packet_filter_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union mac_packet_filter_u *value)
{
	return hppe_reg_set(
				dev_id,
				NSS_XGMAC_CSR_BASE_ADDR + MAC_PACKET_FILTER_ADDRESS + \
				index * MAC_PACKET_FILTER_INC,
				value->val);
}
#ifndef IN_PORTCONTROL_MINI

sw_error_t
hppe_mac_watchdog_timeout_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union mac_watchdog_timeout_u *value)
{
	if (index >= MAC_WATCHDOG_TIMEOUT_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				NSS_XGMAC_CSR_BASE_ADDR + MAC_WATCHDOG_TIMEOUT_ADDRESS + \
				index * MAC_WATCHDOG_TIMEOUT_INC,
				&value->val);
}

sw_error_t
hppe_mac_watchdog_timeout_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union mac_watchdog_timeout_u *value)
{
	return hppe_reg_set(
				dev_id,
				NSS_XGMAC_CSR_BASE_ADDR + MAC_WATCHDOG_TIMEOUT_ADDRESS + \
				index * MAC_WATCHDOG_TIMEOUT_INC,
				value->val);
}

sw_error_t
hppe_mac_vlan_tag_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union mac_vlan_tag_u *value)
{
	if (index >= MAC_VLAN_TAG_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				NSS_XGMAC_CSR_BASE_ADDR + MAC_VLAN_TAG_ADDRESS + \
				index * MAC_VLAN_TAG_INC,
				&value->val);
}

sw_error_t
hppe_mac_vlan_tag_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union mac_vlan_tag_u *value)
{
	return hppe_reg_set(
				dev_id,
				NSS_XGMAC_CSR_BASE_ADDR + MAC_VLAN_TAG_ADDRESS + \
				index * MAC_VLAN_TAG_INC,
				value->val);
}

sw_error_t
hppe_mac_rx_eth_type_match_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union mac_rx_eth_type_match_u *value)
{
	if (index >= MAC_RX_ETH_TYPE_MATCH_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				NSS_XGMAC_CSR_BASE_ADDR + MAC_RX_ETH_TYPE_MATCH_ADDRESS + \
				index * MAC_RX_ETH_TYPE_MATCH_INC,
				&value->val);
}

sw_error_t
hppe_mac_rx_eth_type_match_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union mac_rx_eth_type_match_u *value)
{
	return hppe_reg_set(
				dev_id,
				NSS_XGMAC_CSR_BASE_ADDR + MAC_RX_ETH_TYPE_MATCH_ADDRESS + \
				index * MAC_RX_ETH_TYPE_MATCH_INC,
				value->val);
}
#endif
sw_error_t
hppe_mac_q0_tx_flow_ctrl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union mac_q0_tx_flow_ctrl_u *value)
{
	if (index >= MAC_Q0_TX_FLOW_CTRL_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				NSS_XGMAC_CSR_BASE_ADDR + MAC_Q0_TX_FLOW_CTRL_ADDRESS + \
				index * MAC_Q0_TX_FLOW_CTRL_INC,
				&value->val);
}

sw_error_t
hppe_mac_q0_tx_flow_ctrl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union mac_q0_tx_flow_ctrl_u *value)
{
	return hppe_reg_set(
				dev_id,
				NSS_XGMAC_CSR_BASE_ADDR + MAC_Q0_TX_FLOW_CTRL_ADDRESS + \
				index * MAC_Q0_TX_FLOW_CTRL_INC,
				value->val);
}

sw_error_t
hppe_mac_rx_flow_ctrl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union mac_rx_flow_ctrl_u *value)
{
	if (index >= MAC_RX_FLOW_CTRL_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				NSS_XGMAC_CSR_BASE_ADDR + MAC_RX_FLOW_CTRL_ADDRESS + \
				index * MAC_RX_FLOW_CTRL_INC,
				&value->val);
}

sw_error_t
hppe_mac_rx_flow_ctrl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union mac_rx_flow_ctrl_u *value)
{
	return hppe_reg_set(
				dev_id,
				NSS_XGMAC_CSR_BASE_ADDR + MAC_RX_FLOW_CTRL_ADDRESS + \
				index * MAC_RX_FLOW_CTRL_INC,
				value->val);
}
#ifndef IN_PORTCONTROL_MINI

sw_error_t
hppe_mac_interrupt_status_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union mac_interrupt_status_u *value)
{
	if (index >= MAC_INTERRUPT_STATUS_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				NSS_XGMAC_CSR_BASE_ADDR + MAC_INTERRUPT_STATUS_ADDRESS + \
				index * MAC_INTERRUPT_STATUS_INC,
				&value->val);
}

sw_error_t
hppe_mac_interrupt_status_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union mac_interrupt_status_u *value)
{
	return SW_NOT_SUPPORTED;

}

sw_error_t
hppe_mac_interrupt_enable_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union mac_interrupt_enable_u *value)
{
	if (index >= MAC_INTERRUPT_ENABLE_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				NSS_XGMAC_CSR_BASE_ADDR + MAC_INTERRUPT_ENABLE_ADDRESS + \
				index * MAC_INTERRUPT_ENABLE_INC,
				&value->val);
}

sw_error_t
hppe_mac_interrupt_enable_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union mac_interrupt_enable_u *value)
{
	return hppe_reg_set(
				dev_id,
				NSS_XGMAC_CSR_BASE_ADDR + MAC_INTERRUPT_ENABLE_ADDRESS + \
				index * MAC_INTERRUPT_ENABLE_INC,
				value->val);
}

sw_error_t
hppe_mac_rx_tx_status_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union mac_rx_tx_status_u *value)
{
	if (index >= MAC_RX_TX_STATUS_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				NSS_XGMAC_CSR_BASE_ADDR + MAC_RX_TX_STATUS_ADDRESS + \
				index * MAC_RX_TX_STATUS_INC,
				&value->val);
}

sw_error_t
hppe_mac_rx_tx_status_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union mac_rx_tx_status_u *value)
{
	return SW_NOT_SUPPORTED;

}

sw_error_t
hppe_mac_lpi_control_status_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union mac_lpi_control_status_u *value)
{
	if (index >= MAC_LPI_CONTROL_STATUS_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				NSS_XGMAC_CSR_BASE_ADDR + MAC_LPI_CONTROL_STATUS_ADDRESS + \
				index * MAC_LPI_CONTROL_STATUS_INC,
				&value->val);
}

sw_error_t
hppe_mac_lpi_control_status_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union mac_lpi_control_status_u *value)
{
	return hppe_reg_set(
				dev_id,
				NSS_XGMAC_CSR_BASE_ADDR + MAC_LPI_CONTROL_STATUS_ADDRESS + \
				index * MAC_LPI_CONTROL_STATUS_INC,
				value->val);
}

sw_error_t
hppe_mac_lpi_timers_control_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union mac_lpi_timers_control_u *value)
{
	if (index >= MAC_LPI_TIMERS_CONTROL_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				NSS_XGMAC_CSR_BASE_ADDR + MAC_LPI_TIMERS_CONTROL_ADDRESS + \
				index * MAC_LPI_TIMERS_CONTROL_INC,
				&value->val);
}

sw_error_t
hppe_mac_lpi_timers_control_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union mac_lpi_timers_control_u *value)
{
	return hppe_reg_set(
				dev_id,
				NSS_XGMAC_CSR_BASE_ADDR + MAC_LPI_TIMERS_CONTROL_ADDRESS + \
				index * MAC_LPI_TIMERS_CONTROL_INC,
				value->val);
}

sw_error_t
hppe_mac_lpi_auto_entry_timer_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union mac_lpi_auto_entry_timer_u *value)
{
	if (index >= MAC_LPI_AUTO_ENTRY_TIMER_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				NSS_XGMAC_CSR_BASE_ADDR + MAC_LPI_AUTO_ENTRY_TIMER_ADDRESS + \
				index * MAC_LPI_AUTO_ENTRY_TIMER_INC,
				&value->val);
}

sw_error_t
hppe_mac_lpi_auto_entry_timer_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union mac_lpi_auto_entry_timer_u *value)
{
	return hppe_reg_set(
				dev_id,
				NSS_XGMAC_CSR_BASE_ADDR + MAC_LPI_AUTO_ENTRY_TIMER_ADDRESS + \
				index * MAC_LPI_AUTO_ENTRY_TIMER_INC,
				value->val);
}

sw_error_t
hppe_mac_1us_tic_counter_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union mac_1us_tic_counter_u *value)
{
	if (index >= MAC_1US_TIC_COUNTER_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				NSS_XGMAC_CSR_BASE_ADDR + MAC_1US_TIC_COUNTER_ADDRESS + \
				index * MAC_1US_TIC_COUNTER_INC,
				&value->val);
}

sw_error_t
hppe_mac_1us_tic_counter_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union mac_1us_tic_counter_u *value)
{
	return hppe_reg_set(
				dev_id,
				NSS_XGMAC_CSR_BASE_ADDR + MAC_1US_TIC_COUNTER_ADDRESS + \
				index * MAC_1US_TIC_COUNTER_INC,
				value->val);
}

sw_error_t
hppe_mac_address0_high_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union mac_address0_high_u *value)
{
	if (index >= MAC_ADDRESS0_HIGH_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				NSS_XGMAC_CSR_BASE_ADDR + MAC_ADDRESS0_HIGH_ADDRESS + \
				index * MAC_ADDRESS0_HIGH_INC,
				&value->val);
}

sw_error_t
hppe_mac_address0_high_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union mac_address0_high_u *value)
{
	return hppe_reg_set(
				dev_id,
				NSS_XGMAC_CSR_BASE_ADDR + MAC_ADDRESS0_HIGH_ADDRESS + \
				index * MAC_ADDRESS0_HIGH_INC,
				value->val);
}

sw_error_t
hppe_mac_address0_low_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union mac_address0_low_u *value)
{
	if (index >= MAC_ADDRESS0_LOW_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				NSS_XGMAC_CSR_BASE_ADDR + MAC_ADDRESS0_LOW_ADDRESS + \
				index * MAC_ADDRESS0_LOW_INC,
				&value->val);
}

sw_error_t
hppe_mac_address0_low_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union mac_address0_low_u *value)
{
	return hppe_reg_set(
				dev_id,
				NSS_XGMAC_CSR_BASE_ADDR + MAC_ADDRESS0_LOW_ADDRESS + \
				index * MAC_ADDRESS0_LOW_INC,
				value->val);
}

sw_error_t
hppe_mmc_receive_interrupt_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union mmc_receive_interrupt_u *value)
{
	if (index >= MMC_RECEIVE_INTERRUPT_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				NSS_XGMAC_CSR_BASE_ADDR + MMC_RECEIVE_INTERRUPT_ADDRESS + \
				index * MMC_RECEIVE_INTERRUPT_INC,
				&value->val);
}

sw_error_t
hppe_mmc_receive_interrupt_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union mmc_receive_interrupt_u *value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_mmc_transmit_interrupt_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union mmc_transmit_interrupt_u *value)
{
	if (index >= MMC_TRANSMIT_INTERRUPT_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				NSS_XGMAC_CSR_BASE_ADDR + MMC_TRANSMIT_INTERRUPT_ADDRESS + \
				index * MMC_TRANSMIT_INTERRUPT_INC,
				&value->val);
}

sw_error_t
hppe_mmc_transmit_interrupt_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union mmc_transmit_interrupt_u *value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_mmc_receive_interrupt_enable_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union mmc_receive_interrupt_enable_u *value)
{
	if (index >= MMC_RECEIVE_INTERRUPT_ENABLE_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				NSS_XGMAC_CSR_BASE_ADDR + MMC_RECEIVE_INTERRUPT_ENABLE_ADDRESS + \
				index * MMC_RECEIVE_INTERRUPT_ENABLE_INC,
				&value->val);
}

sw_error_t
hppe_mmc_receive_interrupt_enable_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union mmc_receive_interrupt_enable_u *value)
{
	return hppe_reg_set(
				dev_id,
				NSS_XGMAC_CSR_BASE_ADDR + MMC_RECEIVE_INTERRUPT_ENABLE_ADDRESS + \
				index * MMC_RECEIVE_INTERRUPT_ENABLE_INC,
				value->val);
}

sw_error_t
hppe_mmc_transmit_interrupt_enable_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union mmc_transmit_interrupt_enable_u *value)
{
	if (index >= MMC_TRANSMIT_INTERRUPT_ENABLE_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				NSS_XGMAC_CSR_BASE_ADDR + MMC_TRANSMIT_INTERRUPT_ENABLE_ADDRESS + \
				index * MMC_TRANSMIT_INTERRUPT_ENABLE_INC,
				&value->val);
}

sw_error_t
hppe_mmc_transmit_interrupt_enable_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union mmc_transmit_interrupt_enable_u *value)
{
	return hppe_reg_set(
				dev_id,
				NSS_XGMAC_CSR_BASE_ADDR + MMC_TRANSMIT_INTERRUPT_ENABLE_ADDRESS + \
				index * MMC_TRANSMIT_INTERRUPT_ENABLE_INC,
				value->val);
}

sw_error_t
hppe_mac_tx_configuration_vne_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mac_tx_configuration_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_tx_configuration_get(dev_id, index, &reg_val);
	*value = reg_val.bf.vne;
	return ret;
}

sw_error_t
hppe_mac_tx_configuration_vne_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union mac_tx_configuration_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_tx_configuration_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.vne = value;
	ret = hppe_mac_tx_configuration_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_mac_tx_configuration_ddic_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mac_tx_configuration_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_tx_configuration_get(dev_id, index, &reg_val);
	*value = reg_val.bf.ddic;
	return ret;
}

sw_error_t
hppe_mac_tx_configuration_ddic_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union mac_tx_configuration_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_tx_configuration_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ddic = value;
	ret = hppe_mac_tx_configuration_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_mac_tx_configuration_te_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mac_tx_configuration_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_tx_configuration_get(dev_id, index, &reg_val);
	*value = reg_val.bf.te;
	return ret;
}

sw_error_t
hppe_mac_tx_configuration_te_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union mac_tx_configuration_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_tx_configuration_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.te = value;
	ret = hppe_mac_tx_configuration_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_mac_tx_configuration_ipg_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mac_tx_configuration_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_tx_configuration_get(dev_id, index, &reg_val);
	*value = reg_val.bf.ipg;
	return ret;
}

sw_error_t
hppe_mac_tx_configuration_ipg_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union mac_tx_configuration_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_tx_configuration_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ipg = value;
	ret = hppe_mac_tx_configuration_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_mac_tx_configuration_ism_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mac_tx_configuration_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_tx_configuration_get(dev_id, index, &reg_val);
	*value = reg_val.bf.ism;
	return ret;
}

sw_error_t
hppe_mac_tx_configuration_ism_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union mac_tx_configuration_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_tx_configuration_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ism = value;
	ret = hppe_mac_tx_configuration_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_mac_tx_configuration_ifp_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mac_tx_configuration_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_tx_configuration_get(dev_id, index, &reg_val);
	*value = reg_val.bf.ifp;
	return ret;
}

sw_error_t
hppe_mac_tx_configuration_ifp_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union mac_tx_configuration_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_tx_configuration_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ifp = value;
	ret = hppe_mac_tx_configuration_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_mac_tx_configuration_sarc_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mac_tx_configuration_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_tx_configuration_get(dev_id, index, &reg_val);
	*value = reg_val.bf.sarc;
	return ret;
}

sw_error_t
hppe_mac_tx_configuration_sarc_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union mac_tx_configuration_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_tx_configuration_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.sarc = value;
	ret = hppe_mac_tx_configuration_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_mac_tx_configuration_isr_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mac_tx_configuration_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_tx_configuration_get(dev_id, index, &reg_val);
	*value = reg_val.bf.isr;
	return ret;
}

sw_error_t
hppe_mac_tx_configuration_isr_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union mac_tx_configuration_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_tx_configuration_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.isr = value;
	ret = hppe_mac_tx_configuration_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_mac_tx_configuration_ss_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mac_tx_configuration_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_tx_configuration_get(dev_id, index, &reg_val);
	*value = reg_val.bf.ss;
	return ret;
}

sw_error_t
hppe_mac_tx_configuration_ss_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union mac_tx_configuration_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_tx_configuration_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ss = value;
	ret = hppe_mac_tx_configuration_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_mac_tx_configuration_g9991en_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mac_tx_configuration_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_tx_configuration_get(dev_id, index, &reg_val);
	*value = reg_val.bf.g9991en;
	return ret;
}

sw_error_t
hppe_mac_tx_configuration_g9991en_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union mac_tx_configuration_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_tx_configuration_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.g9991en = value;
	ret = hppe_mac_tx_configuration_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_mac_tx_configuration_uss_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mac_tx_configuration_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_tx_configuration_get(dev_id, index, &reg_val);
	*value = reg_val.bf.uss;
	return ret;
}

sw_error_t
hppe_mac_tx_configuration_uss_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union mac_tx_configuration_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_tx_configuration_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.uss = value;
	ret = hppe_mac_tx_configuration_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_mac_tx_configuration_vnm_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mac_tx_configuration_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_tx_configuration_get(dev_id, index, &reg_val);
	*value = reg_val.bf.vnm;
	return ret;
}

sw_error_t
hppe_mac_tx_configuration_vnm_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union mac_tx_configuration_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_tx_configuration_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.vnm = value;
	ret = hppe_mac_tx_configuration_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_mac_tx_configuration_jd_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mac_tx_configuration_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_tx_configuration_get(dev_id, index, &reg_val);
	*value = reg_val.bf.jd;
	return ret;
}
#endif
sw_error_t
hppe_mac_tx_configuration_jd_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union mac_tx_configuration_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_tx_configuration_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.jd = value;
	ret = hppe_mac_tx_configuration_set(dev_id, index, &reg_val);
	return ret;
}
#ifndef IN_PORTCONTROL_MINI

sw_error_t
hppe_mac_rx_configuration_lm_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mac_rx_configuration_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_rx_configuration_get(dev_id, index, &reg_val);
	*value = reg_val.bf.lm;
	return ret;
}

sw_error_t
hppe_mac_rx_configuration_lm_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union mac_rx_configuration_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_rx_configuration_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.lm = value;
	ret = hppe_mac_rx_configuration_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_mac_rx_configuration_je_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mac_rx_configuration_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_rx_configuration_get(dev_id, index, &reg_val);
	*value = reg_val.bf.je;
	return ret;
}

sw_error_t
hppe_mac_rx_configuration_je_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union mac_rx_configuration_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_rx_configuration_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.je = value;
	ret = hppe_mac_rx_configuration_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_mac_rx_configuration_arpen_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mac_rx_configuration_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_rx_configuration_get(dev_id, index, &reg_val);
	*value = reg_val.bf.arpen;
	return ret;
}

sw_error_t
hppe_mac_rx_configuration_arpen_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union mac_rx_configuration_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_rx_configuration_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.arpen = value;
	ret = hppe_mac_rx_configuration_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_mac_rx_configuration_elen_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mac_rx_configuration_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_rx_configuration_get(dev_id, index, &reg_val);
	*value = reg_val.bf.elen;
	return ret;
}

sw_error_t
hppe_mac_rx_configuration_elen_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union mac_rx_configuration_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_rx_configuration_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.elen = value;
	ret = hppe_mac_rx_configuration_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_mac_rx_configuration_gmpslce_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mac_rx_configuration_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_rx_configuration_get(dev_id, index, &reg_val);
	*value = reg_val.bf.gmpslce;
	return ret;
}
#endif
sw_error_t
hppe_mac_rx_configuration_gmpslce_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union mac_rx_configuration_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_rx_configuration_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.gmpslce = value;
	ret = hppe_mac_rx_configuration_set(dev_id, index, &reg_val);
	return ret;
}
#ifndef IN_PORTCONTROL_MINI

sw_error_t
hppe_mac_rx_configuration_hdsms_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mac_rx_configuration_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_rx_configuration_get(dev_id, index, &reg_val);
	*value = reg_val.bf.hdsms;
	return ret;
}

sw_error_t
hppe_mac_rx_configuration_hdsms_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union mac_rx_configuration_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_rx_configuration_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.hdsms = value;
	ret = hppe_mac_rx_configuration_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_mac_rx_configuration_spen_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mac_rx_configuration_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_rx_configuration_get(dev_id, index, &reg_val);
	*value = reg_val.bf.spen;
	return ret;
}

sw_error_t
hppe_mac_rx_configuration_spen_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union mac_rx_configuration_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_rx_configuration_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.spen = value;
	ret = hppe_mac_rx_configuration_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_mac_rx_configuration_usp_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mac_rx_configuration_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_rx_configuration_get(dev_id, index, &reg_val);
	*value = reg_val.bf.usp;
	return ret;
}

sw_error_t
hppe_mac_rx_configuration_usp_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union mac_rx_configuration_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_rx_configuration_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.usp = value;
	ret = hppe_mac_rx_configuration_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_mac_rx_configuration_ipc_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mac_rx_configuration_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_rx_configuration_get(dev_id, index, &reg_val);
	*value = reg_val.bf.ipc;
	return ret;
}

sw_error_t
hppe_mac_rx_configuration_ipc_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union mac_rx_configuration_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_rx_configuration_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ipc = value;
	ret = hppe_mac_rx_configuration_set(dev_id, index, &reg_val);
	return ret;
}
#endif
sw_error_t
hppe_mac_rx_configuration_gpsl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mac_rx_configuration_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_rx_configuration_get(dev_id, index, &reg_val);
	*value = reg_val.bf.gpsl;
	return ret;
}

sw_error_t
hppe_mac_rx_configuration_gpsl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union mac_rx_configuration_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_rx_configuration_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.gpsl = value;
	ret = hppe_mac_rx_configuration_set(dev_id, index, &reg_val);
	return ret;
}
#ifndef IN_PORTCONTROL_MINI

sw_error_t
hppe_mac_rx_configuration_re_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mac_rx_configuration_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_rx_configuration_get(dev_id, index, &reg_val);
	*value = reg_val.bf.re;
	return ret;
}

sw_error_t
hppe_mac_rx_configuration_re_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union mac_rx_configuration_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_rx_configuration_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.re = value;
	ret = hppe_mac_rx_configuration_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_mac_rx_configuration_cst_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mac_rx_configuration_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_rx_configuration_get(dev_id, index, &reg_val);
	*value = reg_val.bf.cst;
	return ret;
}

sw_error_t
hppe_mac_rx_configuration_cst_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union mac_rx_configuration_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_rx_configuration_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.cst = value;
	ret = hppe_mac_rx_configuration_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_mac_rx_configuration_dcrcc_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mac_rx_configuration_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_rx_configuration_get(dev_id, index, &reg_val);
	*value = reg_val.bf.dcrcc;
	return ret;
}

sw_error_t
hppe_mac_rx_configuration_dcrcc_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union mac_rx_configuration_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_rx_configuration_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.dcrcc = value;
	ret = hppe_mac_rx_configuration_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_mac_rx_configuration_wd_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mac_rx_configuration_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_rx_configuration_get(dev_id, index, &reg_val);
	*value = reg_val.bf.wd;
	return ret;
}
#endif
sw_error_t
hppe_mac_rx_configuration_wd_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union mac_rx_configuration_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_rx_configuration_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.wd = value;
	ret = hppe_mac_rx_configuration_set(dev_id, index, &reg_val);
	return ret;
}
#ifndef IN_PORTCONTROL_MINI

sw_error_t
hppe_mac_rx_configuration_acs_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mac_rx_configuration_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_rx_configuration_get(dev_id, index, &reg_val);
	*value = reg_val.bf.acs;
	return ret;
}

sw_error_t
hppe_mac_rx_configuration_acs_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union mac_rx_configuration_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_rx_configuration_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.acs = value;
	ret = hppe_mac_rx_configuration_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_mac_rx_configuration_s2kp_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mac_rx_configuration_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_rx_configuration_get(dev_id, index, &reg_val);
	*value = reg_val.bf.s2kp;
	return ret;
}

sw_error_t
hppe_mac_rx_configuration_s2kp_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union mac_rx_configuration_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_rx_configuration_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.s2kp = value;
	ret = hppe_mac_rx_configuration_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_mac_packet_filter_pcf_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mac_packet_filter_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_packet_filter_get(dev_id, index, &reg_val);
	*value = reg_val.bf.pcf;
	return ret;
}
#endif
sw_error_t
hppe_mac_packet_filter_pcf_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union mac_packet_filter_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_packet_filter_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.pcf = value;
	ret = hppe_mac_packet_filter_set(dev_id, index, &reg_val);
	return ret;
}
#ifndef IN_PORTCONTROL_MINI

sw_error_t
hppe_mac_packet_filter_hmc_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mac_packet_filter_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_packet_filter_get(dev_id, index, &reg_val);
	*value = reg_val.bf.hmc;
	return ret;
}

sw_error_t
hppe_mac_packet_filter_hmc_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union mac_packet_filter_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_packet_filter_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.hmc = value;
	ret = hppe_mac_packet_filter_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_mac_packet_filter_dntu_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mac_packet_filter_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_packet_filter_get(dev_id, index, &reg_val);
	*value = reg_val.bf.dntu;
	return ret;
}

sw_error_t
hppe_mac_packet_filter_dntu_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union mac_packet_filter_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_packet_filter_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.dntu = value;
	ret = hppe_mac_packet_filter_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_mac_packet_filter_saf_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mac_packet_filter_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_packet_filter_get(dev_id, index, &reg_val);
	*value = reg_val.bf.saf;
	return ret;
}

sw_error_t
hppe_mac_packet_filter_saf_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union mac_packet_filter_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_packet_filter_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.saf = value;
	ret = hppe_mac_packet_filter_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_mac_packet_filter_dbf_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mac_packet_filter_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_packet_filter_get(dev_id, index, &reg_val);
	*value = reg_val.bf.dbf;
	return ret;
}

sw_error_t
hppe_mac_packet_filter_dbf_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union mac_packet_filter_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_packet_filter_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.dbf = value;
	ret = hppe_mac_packet_filter_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_mac_packet_filter_huc_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mac_packet_filter_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_packet_filter_get(dev_id, index, &reg_val);
	*value = reg_val.bf.huc;
	return ret;
}

sw_error_t
hppe_mac_packet_filter_huc_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union mac_packet_filter_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_packet_filter_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.huc = value;
	ret = hppe_mac_packet_filter_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_mac_packet_filter_vtfe_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mac_packet_filter_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_packet_filter_get(dev_id, index, &reg_val);
	*value = reg_val.bf.vtfe;
	return ret;
}

sw_error_t
hppe_mac_packet_filter_vtfe_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union mac_packet_filter_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_packet_filter_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.vtfe = value;
	ret = hppe_mac_packet_filter_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_mac_packet_filter_daif_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mac_packet_filter_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_packet_filter_get(dev_id, index, &reg_val);
	*value = reg_val.bf.daif;
	return ret;
}

sw_error_t
hppe_mac_packet_filter_daif_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union mac_packet_filter_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_packet_filter_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.daif = value;
	ret = hppe_mac_packet_filter_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_mac_packet_filter_ra_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mac_packet_filter_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_packet_filter_get(dev_id, index, &reg_val);
	*value = reg_val.bf.ra;
	return ret;
}

sw_error_t
hppe_mac_packet_filter_ra_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union mac_packet_filter_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_packet_filter_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ra = value;
	ret = hppe_mac_packet_filter_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_mac_packet_filter_hpf_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mac_packet_filter_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_packet_filter_get(dev_id, index, &reg_val);
	*value = reg_val.bf.hpf;
	return ret;
}

sw_error_t
hppe_mac_packet_filter_hpf_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union mac_packet_filter_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_packet_filter_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.hpf = value;
	ret = hppe_mac_packet_filter_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_mac_packet_filter_pm_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mac_packet_filter_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_packet_filter_get(dev_id, index, &reg_val);
	*value = reg_val.bf.pm;
	return ret;
}

sw_error_t
hppe_mac_packet_filter_pm_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union mac_packet_filter_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_packet_filter_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.pm = value;
	ret = hppe_mac_packet_filter_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_mac_packet_filter_vucc_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mac_packet_filter_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_packet_filter_get(dev_id, index, &reg_val);
	*value = reg_val.bf.vucc;
	return ret;
}

sw_error_t
hppe_mac_packet_filter_vucc_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union mac_packet_filter_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_packet_filter_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.vucc = value;
	ret = hppe_mac_packet_filter_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_mac_packet_filter_pr_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mac_packet_filter_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_packet_filter_get(dev_id, index, &reg_val);
	*value = reg_val.bf.pr;
	return ret;
}
#endif
sw_error_t
hppe_mac_packet_filter_pr_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union mac_packet_filter_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_packet_filter_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.pr = value;
	ret = hppe_mac_packet_filter_set(dev_id, index, &reg_val);
	return ret;
}
#ifndef IN_PORTCONTROL_MINI

sw_error_t
hppe_mac_packet_filter_ipfe_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mac_packet_filter_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_packet_filter_get(dev_id, index, &reg_val);
	*value = reg_val.bf.ipfe;
	return ret;
}

sw_error_t
hppe_mac_packet_filter_ipfe_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union mac_packet_filter_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_packet_filter_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ipfe = value;
	ret = hppe_mac_packet_filter_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_mac_packet_filter_saif_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mac_packet_filter_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_packet_filter_get(dev_id, index, &reg_val);
	*value = reg_val.bf.saif;
	return ret;
}

sw_error_t
hppe_mac_packet_filter_saif_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union mac_packet_filter_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_packet_filter_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.saif = value;
	ret = hppe_mac_packet_filter_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_mac_watchdog_timeout_pwe_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mac_watchdog_timeout_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_watchdog_timeout_get(dev_id, index, &reg_val);
	*value = reg_val.bf.pwe;
	return ret;
}

sw_error_t
hppe_mac_watchdog_timeout_pwe_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union mac_watchdog_timeout_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_watchdog_timeout_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.pwe = value;
	ret = hppe_mac_watchdog_timeout_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_mac_watchdog_timeout_wto_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mac_watchdog_timeout_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_watchdog_timeout_get(dev_id, index, &reg_val);
	*value = reg_val.bf.wto;
	return ret;
}

sw_error_t
hppe_mac_watchdog_timeout_wto_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union mac_watchdog_timeout_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_watchdog_timeout_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.wto = value;
	ret = hppe_mac_watchdog_timeout_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_mac_vlan_tag_eivls_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mac_vlan_tag_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_vlan_tag_get(dev_id, index, &reg_val);
	*value = reg_val.bf.eivls;
	return ret;
}

sw_error_t
hppe_mac_vlan_tag_eivls_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union mac_vlan_tag_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_vlan_tag_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.eivls = value;
	ret = hppe_mac_vlan_tag_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_mac_vlan_tag_vthm_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mac_vlan_tag_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_vlan_tag_get(dev_id, index, &reg_val);
	*value = reg_val.bf.vthm;
	return ret;
}

sw_error_t
hppe_mac_vlan_tag_vthm_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union mac_vlan_tag_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_vlan_tag_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.vthm = value;
	ret = hppe_mac_vlan_tag_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_mac_vlan_tag_vl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mac_vlan_tag_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_vlan_tag_get(dev_id, index, &reg_val);
	*value = reg_val.bf.vl;
	return ret;
}

sw_error_t
hppe_mac_vlan_tag_vl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union mac_vlan_tag_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_vlan_tag_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.vl = value;
	ret = hppe_mac_vlan_tag_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_mac_vlan_tag_dovltc_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mac_vlan_tag_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_vlan_tag_get(dev_id, index, &reg_val);
	*value = reg_val.bf.dovltc;
	return ret;
}

sw_error_t
hppe_mac_vlan_tag_dovltc_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union mac_vlan_tag_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_vlan_tag_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.dovltc = value;
	ret = hppe_mac_vlan_tag_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_mac_vlan_tag_etv_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mac_vlan_tag_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_vlan_tag_get(dev_id, index, &reg_val);
	*value = reg_val.bf.etv;
	return ret;
}

sw_error_t
hppe_mac_vlan_tag_etv_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union mac_vlan_tag_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_vlan_tag_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.etv = value;
	ret = hppe_mac_vlan_tag_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_mac_vlan_tag_erivlt_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mac_vlan_tag_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_vlan_tag_get(dev_id, index, &reg_val);
	*value = reg_val.bf.erivlt;
	return ret;
}

sw_error_t
hppe_mac_vlan_tag_erivlt_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union mac_vlan_tag_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_vlan_tag_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.erivlt = value;
	ret = hppe_mac_vlan_tag_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_mac_vlan_tag_eivlrxs_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mac_vlan_tag_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_vlan_tag_get(dev_id, index, &reg_val);
	*value = reg_val.bf.eivlrxs;
	return ret;
}

sw_error_t
hppe_mac_vlan_tag_eivlrxs_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union mac_vlan_tag_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_vlan_tag_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.eivlrxs = value;
	ret = hppe_mac_vlan_tag_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_mac_vlan_tag_vtim_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mac_vlan_tag_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_vlan_tag_get(dev_id, index, &reg_val);
	*value = reg_val.bf.vtim;
	return ret;
}

sw_error_t
hppe_mac_vlan_tag_vtim_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union mac_vlan_tag_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_vlan_tag_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.vtim = value;
	ret = hppe_mac_vlan_tag_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_mac_vlan_tag_edvlp_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mac_vlan_tag_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_vlan_tag_get(dev_id, index, &reg_val);
	*value = reg_val.bf.edvlp;
	return ret;
}

sw_error_t
hppe_mac_vlan_tag_edvlp_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union mac_vlan_tag_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_vlan_tag_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.edvlp = value;
	ret = hppe_mac_vlan_tag_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_mac_vlan_tag_evlrxs_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mac_vlan_tag_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_vlan_tag_get(dev_id, index, &reg_val);
	*value = reg_val.bf.evlrxs;
	return ret;
}

sw_error_t
hppe_mac_vlan_tag_evlrxs_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union mac_vlan_tag_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_vlan_tag_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.evlrxs = value;
	ret = hppe_mac_vlan_tag_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_mac_vlan_tag_evls_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mac_vlan_tag_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_vlan_tag_get(dev_id, index, &reg_val);
	*value = reg_val.bf.evls;
	return ret;
}

sw_error_t
hppe_mac_vlan_tag_evls_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union mac_vlan_tag_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_vlan_tag_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.evls = value;
	ret = hppe_mac_vlan_tag_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_mac_vlan_tag_esvl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mac_vlan_tag_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_vlan_tag_get(dev_id, index, &reg_val);
	*value = reg_val.bf.esvl;
	return ret;
}

sw_error_t
hppe_mac_vlan_tag_esvl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union mac_vlan_tag_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_vlan_tag_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.esvl = value;
	ret = hppe_mac_vlan_tag_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_mac_vlan_tag_ersvlm_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mac_vlan_tag_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_vlan_tag_get(dev_id, index, &reg_val);
	*value = reg_val.bf.ersvlm;
	return ret;
}

sw_error_t
hppe_mac_vlan_tag_ersvlm_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union mac_vlan_tag_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_vlan_tag_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ersvlm = value;
	ret = hppe_mac_vlan_tag_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_mac_rx_eth_type_match_et_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mac_rx_eth_type_match_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_rx_eth_type_match_get(dev_id, index, &reg_val);
	*value = reg_val.bf.et;
	return ret;
}

sw_error_t
hppe_mac_rx_eth_type_match_et_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union mac_rx_eth_type_match_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_rx_eth_type_match_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.et = value;
	ret = hppe_mac_rx_eth_type_match_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_mac_q0_tx_flow_ctrl_pt_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mac_q0_tx_flow_ctrl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_q0_tx_flow_ctrl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.pt;
	return ret;
}

sw_error_t
hppe_mac_q0_tx_flow_ctrl_pt_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union mac_q0_tx_flow_ctrl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_q0_tx_flow_ctrl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.pt = value;
	ret = hppe_mac_q0_tx_flow_ctrl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_mac_q0_tx_flow_ctrl_plt_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mac_q0_tx_flow_ctrl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_q0_tx_flow_ctrl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.plt;
	return ret;
}

sw_error_t
hppe_mac_q0_tx_flow_ctrl_plt_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union mac_q0_tx_flow_ctrl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_q0_tx_flow_ctrl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.plt = value;
	ret = hppe_mac_q0_tx_flow_ctrl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_mac_q0_tx_flow_ctrl_tfe_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mac_q0_tx_flow_ctrl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_q0_tx_flow_ctrl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.tfe;
	return ret;
}

sw_error_t
hppe_mac_q0_tx_flow_ctrl_tfe_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union mac_q0_tx_flow_ctrl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_q0_tx_flow_ctrl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.tfe = value;
	ret = hppe_mac_q0_tx_flow_ctrl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_mac_q0_tx_flow_ctrl_fcb_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mac_q0_tx_flow_ctrl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_q0_tx_flow_ctrl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.fcb;
	return ret;
}

sw_error_t
hppe_mac_q0_tx_flow_ctrl_fcb_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union mac_q0_tx_flow_ctrl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_q0_tx_flow_ctrl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.fcb = value;
	ret = hppe_mac_q0_tx_flow_ctrl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_mac_q0_tx_flow_ctrl_dapq_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mac_q0_tx_flow_ctrl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_q0_tx_flow_ctrl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.dapq;
	return ret;
}

sw_error_t
hppe_mac_q0_tx_flow_ctrl_dapq_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union mac_q0_tx_flow_ctrl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_q0_tx_flow_ctrl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.dapq = value;
	ret = hppe_mac_q0_tx_flow_ctrl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_mac_rx_flow_ctrl_pfce_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mac_rx_flow_ctrl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_rx_flow_ctrl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.pfce;
	return ret;
}

sw_error_t
hppe_mac_rx_flow_ctrl_pfce_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union mac_rx_flow_ctrl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_rx_flow_ctrl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.pfce = value;
	ret = hppe_mac_rx_flow_ctrl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_mac_rx_flow_ctrl_up_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mac_rx_flow_ctrl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_rx_flow_ctrl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.up;
	return ret;
}

sw_error_t
hppe_mac_rx_flow_ctrl_up_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union mac_rx_flow_ctrl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_rx_flow_ctrl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.up = value;
	ret = hppe_mac_rx_flow_ctrl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_mac_rx_flow_ctrl_rfe_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mac_rx_flow_ctrl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_rx_flow_ctrl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.rfe;
	return ret;
}

sw_error_t
hppe_mac_rx_flow_ctrl_rfe_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union mac_rx_flow_ctrl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_rx_flow_ctrl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.rfe = value;
	ret = hppe_mac_rx_flow_ctrl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_mac_interrupt_status_txesis_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mac_interrupt_status_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_interrupt_status_get(dev_id, index, &reg_val);
	*value = reg_val.bf.txesis;
	return ret;
}

sw_error_t
hppe_mac_interrupt_status_txesis_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union mac_interrupt_status_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_interrupt_status_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.txesis = value;
	ret = hppe_mac_interrupt_status_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_mac_interrupt_status_gpiis_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mac_interrupt_status_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_interrupt_status_get(dev_id, index, &reg_val);
	*value = reg_val.bf.gpiis;
	return ret;
}

sw_error_t
hppe_mac_interrupt_status_gpiis_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union mac_interrupt_status_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_interrupt_status_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.gpiis = value;
	ret = hppe_mac_interrupt_status_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_mac_interrupt_status_tsis_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mac_interrupt_status_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_interrupt_status_get(dev_id, index, &reg_val);
	*value = reg_val.bf.tsis;
	return ret;
}

sw_error_t
hppe_mac_interrupt_status_tsis_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union mac_interrupt_status_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_interrupt_status_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.tsis = value;
	ret = hppe_mac_interrupt_status_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_mac_interrupt_status_mmctxis_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mac_interrupt_status_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_interrupt_status_get(dev_id, index, &reg_val);
	*value = reg_val.bf.mmctxis;
	return ret;
}

sw_error_t
hppe_mac_interrupt_status_mmctxis_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union mac_interrupt_status_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_interrupt_status_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.mmctxis = value;
	ret = hppe_mac_interrupt_status_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_mac_interrupt_status_ls_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mac_interrupt_status_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_interrupt_status_get(dev_id, index, &reg_val);
	*value = reg_val.bf.ls;
	return ret;
}

sw_error_t
hppe_mac_interrupt_status_ls_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union mac_interrupt_status_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_interrupt_status_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ls = value;
	ret = hppe_mac_interrupt_status_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_mac_interrupt_status_mmcrxis_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mac_interrupt_status_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_interrupt_status_get(dev_id, index, &reg_val);
	*value = reg_val.bf.mmcrxis;
	return ret;
}

sw_error_t
hppe_mac_interrupt_status_mmcrxis_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union mac_interrupt_status_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_interrupt_status_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.mmcrxis = value;
	ret = hppe_mac_interrupt_status_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_mac_interrupt_status_smi_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mac_interrupt_status_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_interrupt_status_get(dev_id, index, &reg_val);
	*value = reg_val.bf.smi;
	return ret;
}

sw_error_t
hppe_mac_interrupt_status_smi_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union mac_interrupt_status_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_interrupt_status_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.smi = value;
	ret = hppe_mac_interrupt_status_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_mac_interrupt_status_pmtis_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mac_interrupt_status_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_interrupt_status_get(dev_id, index, &reg_val);
	*value = reg_val.bf.pmtis;
	return ret;
}

sw_error_t
hppe_mac_interrupt_status_pmtis_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union mac_interrupt_status_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_interrupt_status_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.pmtis = value;
	ret = hppe_mac_interrupt_status_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_mac_interrupt_status_rxesis_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mac_interrupt_status_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_interrupt_status_get(dev_id, index, &reg_val);
	*value = reg_val.bf.rxesis;
	return ret;
}

sw_error_t
hppe_mac_interrupt_status_rxesis_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union mac_interrupt_status_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_interrupt_status_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.rxesis = value;
	ret = hppe_mac_interrupt_status_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_mac_interrupt_status_lpiis_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mac_interrupt_status_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_interrupt_status_get(dev_id, index, &reg_val);
	*value = reg_val.bf.lpiis;
	return ret;
}

sw_error_t
hppe_mac_interrupt_status_lpiis_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union mac_interrupt_status_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_interrupt_status_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.lpiis = value;
	ret = hppe_mac_interrupt_status_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_mac_interrupt_enable_tsie_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mac_interrupt_enable_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_interrupt_enable_get(dev_id, index, &reg_val);
	*value = reg_val.bf.tsie;
	return ret;
}

sw_error_t
hppe_mac_interrupt_enable_tsie_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_mac_interrupt_enable_lpiie_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mac_interrupt_enable_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_interrupt_enable_get(dev_id, index, &reg_val);
	*value = reg_val.bf.lpiie;
	return ret;
}

sw_error_t
hppe_mac_interrupt_enable_lpiie_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_mac_interrupt_enable_txesie_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mac_interrupt_enable_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_interrupt_enable_get(dev_id, index, &reg_val);
	*value = reg_val.bf.txesie;
	return ret;
}

sw_error_t
hppe_mac_interrupt_enable_txesie_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_mac_interrupt_enable_pmtie_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mac_interrupt_enable_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_interrupt_enable_get(dev_id, index, &reg_val);
	*value = reg_val.bf.pmtie;
	return ret;
}

sw_error_t
hppe_mac_interrupt_enable_pmtie_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_mac_interrupt_enable_rxesie_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mac_interrupt_enable_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_interrupt_enable_get(dev_id, index, &reg_val);
	*value = reg_val.bf.rxesie;
	return ret;
}

sw_error_t
hppe_mac_interrupt_enable_rxesie_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_mac_rx_tx_status_tjt_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mac_rx_tx_status_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_rx_tx_status_get(dev_id, index, &reg_val);
	*value = reg_val.bf.tjt;
	return ret;
}

sw_error_t
hppe_mac_rx_tx_status_tjt_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union mac_rx_tx_status_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_rx_tx_status_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.tjt = value;
	ret = hppe_mac_rx_tx_status_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_mac_rx_tx_status_rwt_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mac_rx_tx_status_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_rx_tx_status_get(dev_id, index, &reg_val);
	*value = reg_val.bf.rwt;
	return ret;
}

sw_error_t
hppe_mac_rx_tx_status_rwt_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union mac_rx_tx_status_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_rx_tx_status_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.rwt = value;
	ret = hppe_mac_rx_tx_status_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_mac_lpi_control_status_tlpien_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mac_lpi_control_status_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_lpi_control_status_get(dev_id, index, &reg_val);
	*value = reg_val.bf.tlpien;
	return ret;
}

sw_error_t
hppe_mac_lpi_control_status_tlpien_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union mac_lpi_control_status_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_lpi_control_status_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.tlpien = value;
	ret = hppe_mac_lpi_control_status_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_mac_lpi_control_status_lpitcse_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mac_lpi_control_status_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_lpi_control_status_get(dev_id, index, &reg_val);
	*value = reg_val.bf.lpitcse;
	return ret;
}

sw_error_t
hppe_mac_lpi_control_status_lpitcse_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union mac_lpi_control_status_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_lpi_control_status_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.lpitcse = value;
	ret = hppe_mac_lpi_control_status_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_mac_lpi_control_status_rxrstp_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mac_lpi_control_status_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_lpi_control_status_get(dev_id, index, &reg_val);
	*value = reg_val.bf.rxrstp;
	return ret;
}

sw_error_t
hppe_mac_lpi_control_status_rxrstp_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union mac_lpi_control_status_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_lpi_control_status_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.rxrstp = value;
	ret = hppe_mac_lpi_control_status_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_mac_lpi_control_status_lpite_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mac_lpi_control_status_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_lpi_control_status_get(dev_id, index, &reg_val);
	*value = reg_val.bf.lpite;
	return ret;
}

sw_error_t
hppe_mac_lpi_control_status_lpite_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union mac_lpi_control_status_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_lpi_control_status_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.lpite = value;
	ret = hppe_mac_lpi_control_status_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_mac_lpi_control_status_pls_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mac_lpi_control_status_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_lpi_control_status_get(dev_id, index, &reg_val);
	*value = reg_val.bf.pls;
	return ret;
}

sw_error_t
hppe_mac_lpi_control_status_pls_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union mac_lpi_control_status_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_lpi_control_status_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.pls = value;
	ret = hppe_mac_lpi_control_status_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_mac_lpi_control_status_rlpiex_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mac_lpi_control_status_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_lpi_control_status_get(dev_id, index, &reg_val);
	*value = reg_val.bf.rlpiex;
	return ret;
}

sw_error_t
hppe_mac_lpi_control_status_rlpiex_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union mac_lpi_control_status_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_lpi_control_status_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.rlpiex = value;
	ret = hppe_mac_lpi_control_status_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_mac_lpi_control_status_rlpien_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mac_lpi_control_status_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_lpi_control_status_get(dev_id, index, &reg_val);
	*value = reg_val.bf.rlpien;
	return ret;
}

sw_error_t
hppe_mac_lpi_control_status_rlpien_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union mac_lpi_control_status_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_lpi_control_status_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.rlpien = value;
	ret = hppe_mac_lpi_control_status_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_mac_lpi_control_status_rlpist_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mac_lpi_control_status_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_lpi_control_status_get(dev_id, index, &reg_val);
	*value = reg_val.bf.rlpist;
	return ret;
}

sw_error_t
hppe_mac_lpi_control_status_rlpist_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union mac_lpi_control_status_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_lpi_control_status_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.rlpist = value;
	ret = hppe_mac_lpi_control_status_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_mac_lpi_control_status_tlpist_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mac_lpi_control_status_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_lpi_control_status_get(dev_id, index, &reg_val);
	*value = reg_val.bf.tlpist;
	return ret;
}

sw_error_t
hppe_mac_lpi_control_status_tlpist_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union mac_lpi_control_status_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_lpi_control_status_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.tlpist = value;
	ret = hppe_mac_lpi_control_status_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_mac_lpi_control_status_txrstp_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mac_lpi_control_status_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_lpi_control_status_get(dev_id, index, &reg_val);
	*value = reg_val.bf.txrstp;
	return ret;
}

sw_error_t
hppe_mac_lpi_control_status_txrstp_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union mac_lpi_control_status_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_lpi_control_status_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.txrstp = value;
	ret = hppe_mac_lpi_control_status_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_mac_lpi_control_status_plsdis_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mac_lpi_control_status_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_lpi_control_status_get(dev_id, index, &reg_val);
	*value = reg_val.bf.plsdis;
	return ret;
}

sw_error_t
hppe_mac_lpi_control_status_plsdis_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union mac_lpi_control_status_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_lpi_control_status_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.plsdis = value;
	ret = hppe_mac_lpi_control_status_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_mac_lpi_control_status_lpitxa_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mac_lpi_control_status_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_lpi_control_status_get(dev_id, index, &reg_val);
	*value = reg_val.bf.lpitxa;
	return ret;
}

sw_error_t
hppe_mac_lpi_control_status_lpitxa_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union mac_lpi_control_status_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_lpi_control_status_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.lpitxa = value;
	ret = hppe_mac_lpi_control_status_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_mac_lpi_control_status_tlpiex_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mac_lpi_control_status_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_lpi_control_status_get(dev_id, index, &reg_val);
	*value = reg_val.bf.tlpiex;
	return ret;
}

sw_error_t
hppe_mac_lpi_control_status_tlpiex_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union mac_lpi_control_status_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_lpi_control_status_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.tlpiex = value;
	ret = hppe_mac_lpi_control_status_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_mac_lpi_control_status_lpitxen_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mac_lpi_control_status_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_lpi_control_status_get(dev_id, index, &reg_val);
	*value = reg_val.bf.lpitxen;
	return ret;
}

sw_error_t
hppe_mac_lpi_control_status_lpitxen_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union mac_lpi_control_status_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_lpi_control_status_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.lpitxen = value;
	ret = hppe_mac_lpi_control_status_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_mac_lpi_timers_control_lst_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mac_lpi_timers_control_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_lpi_timers_control_get(dev_id, index, &reg_val);
	*value = reg_val.bf.lst;
	return ret;
}

sw_error_t
hppe_mac_lpi_timers_control_lst_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_mac_lpi_timers_control_twt_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mac_lpi_timers_control_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_lpi_timers_control_get(dev_id, index, &reg_val);
	*value = reg_val.bf.twt;
	return ret;
}

sw_error_t
hppe_mac_lpi_timers_control_twt_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_mac_lpi_auto_entry_timer_lpiet_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mac_lpi_auto_entry_timer_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_lpi_auto_entry_timer_get(dev_id, index, &reg_val);
	*value = reg_val.bf.lpiet;
	return ret;
}

sw_error_t
hppe_mac_lpi_auto_entry_timer_lpiet_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_mac_1us_tic_counter_tic_1us_cntr_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mac_1us_tic_counter_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_1us_tic_counter_get(dev_id, index, &reg_val);
	*value = reg_val.bf.tic_1us_cntr;
	return ret;
}

sw_error_t
hppe_mac_1us_tic_counter_tic_1us_cntr_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_mac_address0_high_addrhi_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mac_address0_high_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_address0_high_get(dev_id, index, &reg_val);
	*value = reg_val.bf.addrhi;
	return ret;
}

sw_error_t
hppe_mac_address0_high_addrhi_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_mac_address0_high_ae_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mac_address0_high_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_address0_high_get(dev_id, index, &reg_val);
	*value = reg_val.bf.ae;
	return ret;
}

sw_error_t
hppe_mac_address0_high_ae_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_mac_address0_high_dcs_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mac_address0_high_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_address0_high_get(dev_id, index, &reg_val);
	*value = reg_val.bf.dcs;
	return ret;
}

sw_error_t
hppe_mac_address0_high_dcs_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_mac_address0_low_addrlo_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mac_address0_low_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_address0_low_get(dev_id, index, &reg_val);
	*value = reg_val.bf.addrlo;
	return ret;
}

sw_error_t
hppe_mac_address0_low_addrlo_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_mmc_receive_interrupt_rxorangefis_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mmc_receive_interrupt_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mmc_receive_interrupt_get(dev_id, index, &reg_val);
	*value = reg_val.bf.rxorangefis;
	return ret;
}

sw_error_t
hppe_mmc_receive_interrupt_rxorangefis_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_mmc_receive_interrupt_rxlenerfis_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mmc_receive_interrupt_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mmc_receive_interrupt_get(dev_id, index, &reg_val);
	*value = reg_val.bf.rxlenerfis;
	return ret;
}

sw_error_t
hppe_mmc_receive_interrupt_rxlenerfis_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_mmc_receive_interrupt_rx65t127octgbfis_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mmc_receive_interrupt_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mmc_receive_interrupt_get(dev_id, index, &reg_val);
	*value = reg_val.bf.rx65t127octgbfis;
	return ret;
}

sw_error_t
hppe_mmc_receive_interrupt_rx65t127octgbfis_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_mmc_receive_interrupt_rxprmmcis_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mmc_receive_interrupt_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mmc_receive_interrupt_get(dev_id, index, &reg_val);
	*value = reg_val.bf.rxprmmcis;
	return ret;
}

sw_error_t
hppe_mmc_receive_interrupt_rxprmmcis_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_mmc_receive_interrupt_rx512t1023octgbfis_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mmc_receive_interrupt_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mmc_receive_interrupt_get(dev_id, index, &reg_val);
	*value = reg_val.bf.rx512t1023octgbfis;
	return ret;
}

sw_error_t
hppe_mmc_receive_interrupt_rx512t1023octgbfis_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_mmc_receive_interrupt_rxgboctis_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mmc_receive_interrupt_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mmc_receive_interrupt_get(dev_id, index, &reg_val);
	*value = reg_val.bf.rxgboctis;
	return ret;
}

sw_error_t
hppe_mmc_receive_interrupt_rxgboctis_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_mmc_receive_interrupt_rxlpiuscis_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mmc_receive_interrupt_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mmc_receive_interrupt_get(dev_id, index, &reg_val);
	*value = reg_val.bf.rxlpiuscis;
	return ret;
}

sw_error_t
hppe_mmc_receive_interrupt_rxlpiuscis_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_mmc_receive_interrupt_rxjaberfis_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mmc_receive_interrupt_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mmc_receive_interrupt_get(dev_id, index, &reg_val);
	*value = reg_val.bf.rxjaberfis;
	return ret;
}

sw_error_t
hppe_mmc_receive_interrupt_rxjaberfis_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_mmc_receive_interrupt_rxvlangbfis_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mmc_receive_interrupt_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mmc_receive_interrupt_get(dev_id, index, &reg_val);
	*value = reg_val.bf.rxvlangbfis;
	return ret;
}

sw_error_t
hppe_mmc_receive_interrupt_rxvlangbfis_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_mmc_receive_interrupt_rxpausfis_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mmc_receive_interrupt_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mmc_receive_interrupt_get(dev_id, index, &reg_val);
	*value = reg_val.bf.rxpausfis;
	return ret;
}

sw_error_t
hppe_mmc_receive_interrupt_rxpausfis_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_mmc_receive_interrupt_rxcrcerfis_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mmc_receive_interrupt_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mmc_receive_interrupt_get(dev_id, index, &reg_val);
	*value = reg_val.bf.rxcrcerfis;
	return ret;
}

sw_error_t
hppe_mmc_receive_interrupt_rxcrcerfis_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_mmc_receive_interrupt_rxdisocgbis_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mmc_receive_interrupt_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mmc_receive_interrupt_get(dev_id, index, &reg_val);
	*value = reg_val.bf.rxdisocgbis;
	return ret;
}

sw_error_t
hppe_mmc_receive_interrupt_rxdisocgbis_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_mmc_receive_interrupt_rxwdogfis_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mmc_receive_interrupt_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mmc_receive_interrupt_get(dev_id, index, &reg_val);
	*value = reg_val.bf.rxwdogfis;
	return ret;
}

sw_error_t
hppe_mmc_receive_interrupt_rxwdogfis_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_mmc_receive_interrupt_rx128t255octgbfis_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mmc_receive_interrupt_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mmc_receive_interrupt_get(dev_id, index, &reg_val);
	*value = reg_val.bf.rx128t255octgbfis;
	return ret;
}

sw_error_t
hppe_mmc_receive_interrupt_rx128t255octgbfis_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_mmc_receive_interrupt_rxdisfcgbis_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mmc_receive_interrupt_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mmc_receive_interrupt_get(dev_id, index, &reg_val);
	*value = reg_val.bf.rxdisfcgbis;
	return ret;
}

sw_error_t
hppe_mmc_receive_interrupt_rxdisfcgbis_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_mmc_receive_interrupt_rxosizegfis_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mmc_receive_interrupt_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mmc_receive_interrupt_get(dev_id, index, &reg_val);
	*value = reg_val.bf.rxosizegfis;
	return ret;
}

sw_error_t
hppe_mmc_receive_interrupt_rxosizegfis_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_mmc_receive_interrupt_rx1024tmaxoctgbfis_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mmc_receive_interrupt_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mmc_receive_interrupt_get(dev_id, index, &reg_val);
	*value = reg_val.bf.rx1024tmaxoctgbfis;
	return ret;
}

sw_error_t
hppe_mmc_receive_interrupt_rx1024tmaxoctgbfis_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_mmc_receive_interrupt_rxruntfis_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mmc_receive_interrupt_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mmc_receive_interrupt_get(dev_id, index, &reg_val);
	*value = reg_val.bf.rxruntfis;
	return ret;
}

sw_error_t
hppe_mmc_receive_interrupt_rxruntfis_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_mmc_receive_interrupt_rxmcgfis_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mmc_receive_interrupt_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mmc_receive_interrupt_get(dev_id, index, &reg_val);
	*value = reg_val.bf.rxmcgfis;
	return ret;
}

sw_error_t
hppe_mmc_receive_interrupt_rxmcgfis_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_mmc_receive_interrupt_rx256t511octgbfis_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mmc_receive_interrupt_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mmc_receive_interrupt_get(dev_id, index, &reg_val);
	*value = reg_val.bf.rx256t511octgbfis;
	return ret;
}

sw_error_t
hppe_mmc_receive_interrupt_rx256t511octgbfis_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_mmc_receive_interrupt_rx64octgbfis_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mmc_receive_interrupt_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mmc_receive_interrupt_get(dev_id, index, &reg_val);
	*value = reg_val.bf.rx64octgbfis;
	return ret;
}

sw_error_t
hppe_mmc_receive_interrupt_rx64octgbfis_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_mmc_receive_interrupt_rxfovfis_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mmc_receive_interrupt_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mmc_receive_interrupt_get(dev_id, index, &reg_val);
	*value = reg_val.bf.rxfovfis;
	return ret;
}

sw_error_t
hppe_mmc_receive_interrupt_rxfovfis_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_mmc_receive_interrupt_rxgoctis_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mmc_receive_interrupt_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mmc_receive_interrupt_get(dev_id, index, &reg_val);
	*value = reg_val.bf.rxgoctis;
	return ret;
}

sw_error_t
hppe_mmc_receive_interrupt_rxgoctis_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_mmc_receive_interrupt_rxgbfrmis_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mmc_receive_interrupt_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mmc_receive_interrupt_get(dev_id, index, &reg_val);
	*value = reg_val.bf.rxgbfrmis;
	return ret;
}

sw_error_t
hppe_mmc_receive_interrupt_rxgbfrmis_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_mmc_receive_interrupt_rxlpitrcis_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mmc_receive_interrupt_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mmc_receive_interrupt_get(dev_id, index, &reg_val);
	*value = reg_val.bf.rxlpitrcis;
	return ret;
}

sw_error_t
hppe_mmc_receive_interrupt_rxlpitrcis_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_mmc_receive_interrupt_rxbcgfis_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mmc_receive_interrupt_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mmc_receive_interrupt_get(dev_id, index, &reg_val);
	*value = reg_val.bf.rxbcgfis;
	return ret;
}

sw_error_t
hppe_mmc_receive_interrupt_rxbcgfis_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_mmc_receive_interrupt_rxusizegfis_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mmc_receive_interrupt_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mmc_receive_interrupt_get(dev_id, index, &reg_val);
	*value = reg_val.bf.rxusizegfis;
	return ret;
}

sw_error_t
hppe_mmc_receive_interrupt_rxusizegfis_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_mmc_receive_interrupt_rxucgfis_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mmc_receive_interrupt_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mmc_receive_interrupt_get(dev_id, index, &reg_val);
	*value = reg_val.bf.rxucgfis;
	return ret;
}

sw_error_t
hppe_mmc_receive_interrupt_rxucgfis_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_mmc_transmit_interrupt_txgbfrmis_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mmc_transmit_interrupt_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mmc_transmit_interrupt_get(dev_id, index, &reg_val);
	*value = reg_val.bf.txgbfrmis;
	return ret;
}

sw_error_t
hppe_mmc_transmit_interrupt_txgbfrmis_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_mmc_transmit_interrupt_txprmmcis_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mmc_transmit_interrupt_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mmc_transmit_interrupt_get(dev_id, index, &reg_val);
	*value = reg_val.bf.txprmmcis;
	return ret;
}

sw_error_t
hppe_mmc_transmit_interrupt_txprmmcis_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_mmc_transmit_interrupt_tx1024tmaxoctgbfis_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mmc_transmit_interrupt_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mmc_transmit_interrupt_get(dev_id, index, &reg_val);
	*value = reg_val.bf.tx1024tmaxoctgbfis;
	return ret;
}

sw_error_t
hppe_mmc_transmit_interrupt_tx1024tmaxoctgbfis_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_mmc_transmit_interrupt_tx256t511octgbfis_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mmc_transmit_interrupt_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mmc_transmit_interrupt_get(dev_id, index, &reg_val);
	*value = reg_val.bf.tx256t511octgbfis;
	return ret;
}

sw_error_t
hppe_mmc_transmit_interrupt_tx256t511octgbfis_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_mmc_transmit_interrupt_txlpitrcis_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mmc_transmit_interrupt_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mmc_transmit_interrupt_get(dev_id, index, &reg_val);
	*value = reg_val.bf.txlpitrcis;
	return ret;
}

sw_error_t
hppe_mmc_transmit_interrupt_txlpitrcis_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_mmc_transmit_interrupt_txbcgfis_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mmc_transmit_interrupt_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mmc_transmit_interrupt_get(dev_id, index, &reg_val);
	*value = reg_val.bf.txbcgfis;
	return ret;
}

sw_error_t
hppe_mmc_transmit_interrupt_txbcgfis_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_mmc_transmit_interrupt_tx64octgbfis_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mmc_transmit_interrupt_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mmc_transmit_interrupt_get(dev_id, index, &reg_val);
	*value = reg_val.bf.tx64octgbfis;
	return ret;
}

sw_error_t
hppe_mmc_transmit_interrupt_tx64octgbfis_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_mmc_transmit_interrupt_txlpiuscis_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mmc_transmit_interrupt_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mmc_transmit_interrupt_get(dev_id, index, &reg_val);
	*value = reg_val.bf.txlpiuscis;
	return ret;
}

sw_error_t
hppe_mmc_transmit_interrupt_txlpiuscis_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_mmc_transmit_interrupt_txuflowerfis_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mmc_transmit_interrupt_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mmc_transmit_interrupt_get(dev_id, index, &reg_val);
	*value = reg_val.bf.txuflowerfis;
	return ret;
}

sw_error_t
hppe_mmc_transmit_interrupt_txuflowerfis_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_mmc_transmit_interrupt_txbcgbfis_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mmc_transmit_interrupt_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mmc_transmit_interrupt_get(dev_id, index, &reg_val);
	*value = reg_val.bf.txbcgbfis;
	return ret;
}

sw_error_t
hppe_mmc_transmit_interrupt_txbcgbfis_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_mmc_transmit_interrupt_txpausfis_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mmc_transmit_interrupt_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mmc_transmit_interrupt_get(dev_id, index, &reg_val);
	*value = reg_val.bf.txpausfis;
	return ret;
}

sw_error_t
hppe_mmc_transmit_interrupt_txpausfis_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_mmc_transmit_interrupt_txvlangfis_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mmc_transmit_interrupt_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mmc_transmit_interrupt_get(dev_id, index, &reg_val);
	*value = reg_val.bf.txvlangfis;
	return ret;
}

sw_error_t
hppe_mmc_transmit_interrupt_txvlangfis_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_mmc_transmit_interrupt_txgboctis_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mmc_transmit_interrupt_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mmc_transmit_interrupt_get(dev_id, index, &reg_val);
	*value = reg_val.bf.txgboctis;
	return ret;
}

sw_error_t
hppe_mmc_transmit_interrupt_txgboctis_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_mmc_transmit_interrupt_txgfrmis_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mmc_transmit_interrupt_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mmc_transmit_interrupt_get(dev_id, index, &reg_val);
	*value = reg_val.bf.txgfrmis;
	return ret;
}

sw_error_t
hppe_mmc_transmit_interrupt_txgfrmis_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_mmc_transmit_interrupt_tx512t1023octgbfis_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mmc_transmit_interrupt_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mmc_transmit_interrupt_get(dev_id, index, &reg_val);
	*value = reg_val.bf.tx512t1023octgbfis;
	return ret;
}

sw_error_t
hppe_mmc_transmit_interrupt_tx512t1023octgbfis_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_mmc_transmit_interrupt_txmcgfis_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mmc_transmit_interrupt_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mmc_transmit_interrupt_get(dev_id, index, &reg_val);
	*value = reg_val.bf.txmcgfis;
	return ret;
}

sw_error_t
hppe_mmc_transmit_interrupt_txmcgfis_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_mmc_transmit_interrupt_txucgbfis_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mmc_transmit_interrupt_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mmc_transmit_interrupt_get(dev_id, index, &reg_val);
	*value = reg_val.bf.txucgbfis;
	return ret;
}

sw_error_t
hppe_mmc_transmit_interrupt_txucgbfis_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_mmc_transmit_interrupt_tx65t127octgbfis_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mmc_transmit_interrupt_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mmc_transmit_interrupt_get(dev_id, index, &reg_val);
	*value = reg_val.bf.tx65t127octgbfis;
	return ret;
}

sw_error_t
hppe_mmc_transmit_interrupt_tx65t127octgbfis_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_mmc_transmit_interrupt_txmcgbfis_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mmc_transmit_interrupt_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mmc_transmit_interrupt_get(dev_id, index, &reg_val);
	*value = reg_val.bf.txmcgbfis;
	return ret;
}

sw_error_t
hppe_mmc_transmit_interrupt_txmcgbfis_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_mmc_transmit_interrupt_tx128t255octgbfis_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mmc_transmit_interrupt_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mmc_transmit_interrupt_get(dev_id, index, &reg_val);
	*value = reg_val.bf.tx128t255octgbfis;
	return ret;
}

sw_error_t
hppe_mmc_transmit_interrupt_tx128t255octgbfis_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_mmc_transmit_interrupt_txgoctis_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mmc_transmit_interrupt_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mmc_transmit_interrupt_get(dev_id, index, &reg_val);
	*value = reg_val.bf.txgoctis;
	return ret;
}

sw_error_t
hppe_mmc_transmit_interrupt_txgoctis_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_mmc_receive_interrupt_enable_rxprmmcise_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mmc_receive_interrupt_enable_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mmc_receive_interrupt_enable_get(dev_id, index, &reg_val);
	*value = reg_val.bf.rxprmmcise;
	return ret;
}

sw_error_t
hppe_mmc_receive_interrupt_enable_rxprmmcise_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_mmc_receive_interrupt_enable_rx65t127octgbfie_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mmc_receive_interrupt_enable_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mmc_receive_interrupt_enable_get(dev_id, index, &reg_val);
	*value = reg_val.bf.rx65t127octgbfie;
	return ret;
}

sw_error_t
hppe_mmc_receive_interrupt_enable_rx65t127octgbfie_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_mmc_receive_interrupt_enable_rxruntfie_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mmc_receive_interrupt_enable_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mmc_receive_interrupt_enable_get(dev_id, index, &reg_val);
	*value = reg_val.bf.rxruntfie;
	return ret;
}

sw_error_t
hppe_mmc_receive_interrupt_enable_rxruntfie_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_mmc_receive_interrupt_enable_rxcrcerfie_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mmc_receive_interrupt_enable_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mmc_receive_interrupt_enable_get(dev_id, index, &reg_val);
	*value = reg_val.bf.rxcrcerfie;
	return ret;
}

sw_error_t
hppe_mmc_receive_interrupt_enable_rxcrcerfie_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_mmc_receive_interrupt_enable_rx256t511octgbfie_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mmc_receive_interrupt_enable_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mmc_receive_interrupt_enable_get(dev_id, index, &reg_val);
	*value = reg_val.bf.rx256t511octgbfie;
	return ret;
}

sw_error_t
hppe_mmc_receive_interrupt_enable_rx256t511octgbfie_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_mmc_receive_interrupt_enable_rxlenerfie_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mmc_receive_interrupt_enable_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mmc_receive_interrupt_enable_get(dev_id, index, &reg_val);
	*value = reg_val.bf.rxlenerfie;
	return ret;
}

sw_error_t
hppe_mmc_receive_interrupt_enable_rxlenerfie_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_mmc_receive_interrupt_enable_rxusizegfie_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mmc_receive_interrupt_enable_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mmc_receive_interrupt_enable_get(dev_id, index, &reg_val);
	*value = reg_val.bf.rxusizegfie;
	return ret;
}

sw_error_t
hppe_mmc_receive_interrupt_enable_rxusizegfie_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_mmc_receive_interrupt_enable_rxosizegfie_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mmc_receive_interrupt_enable_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mmc_receive_interrupt_enable_get(dev_id, index, &reg_val);
	*value = reg_val.bf.rxosizegfie;
	return ret;
}

sw_error_t
hppe_mmc_receive_interrupt_enable_rxosizegfie_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_mmc_receive_interrupt_enable_rxfovfie_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mmc_receive_interrupt_enable_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mmc_receive_interrupt_enable_get(dev_id, index, &reg_val);
	*value = reg_val.bf.rxfovfie;
	return ret;
}

sw_error_t
hppe_mmc_receive_interrupt_enable_rxfovfie_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_mmc_receive_interrupt_enable_rxmcgfie_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mmc_receive_interrupt_enable_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mmc_receive_interrupt_enable_get(dev_id, index, &reg_val);
	*value = reg_val.bf.rxmcgfie;
	return ret;
}

sw_error_t
hppe_mmc_receive_interrupt_enable_rxmcgfie_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_mmc_receive_interrupt_enable_rxvlangbfie_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mmc_receive_interrupt_enable_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mmc_receive_interrupt_enable_get(dev_id, index, &reg_val);
	*value = reg_val.bf.rxvlangbfie;
	return ret;
}

sw_error_t
hppe_mmc_receive_interrupt_enable_rxvlangbfie_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_mmc_receive_interrupt_enable_rxwdogfie_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mmc_receive_interrupt_enable_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mmc_receive_interrupt_enable_get(dev_id, index, &reg_val);
	*value = reg_val.bf.rxwdogfie;
	return ret;
}

sw_error_t
hppe_mmc_receive_interrupt_enable_rxwdogfie_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_mmc_receive_interrupt_enable_rxdisocie_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mmc_receive_interrupt_enable_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mmc_receive_interrupt_enable_get(dev_id, index, &reg_val);
	*value = reg_val.bf.rxdisocie;
	return ret;
}

sw_error_t
hppe_mmc_receive_interrupt_enable_rxdisocie_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_mmc_receive_interrupt_enable_rxgbfrmie_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mmc_receive_interrupt_enable_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mmc_receive_interrupt_enable_get(dev_id, index, &reg_val);
	*value = reg_val.bf.rxgbfrmie;
	return ret;
}

sw_error_t
hppe_mmc_receive_interrupt_enable_rxgbfrmie_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_mmc_receive_interrupt_enable_rxjaberfie_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mmc_receive_interrupt_enable_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mmc_receive_interrupt_enable_get(dev_id, index, &reg_val);
	*value = reg_val.bf.rxjaberfie;
	return ret;
}

sw_error_t
hppe_mmc_receive_interrupt_enable_rxjaberfie_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_mmc_receive_interrupt_enable_rxlpiuscie_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mmc_receive_interrupt_enable_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mmc_receive_interrupt_enable_get(dev_id, index, &reg_val);
	*value = reg_val.bf.rxlpiuscie;
	return ret;
}

sw_error_t
hppe_mmc_receive_interrupt_enable_rxlpiuscie_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_mmc_receive_interrupt_enable_rxucgfie_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mmc_receive_interrupt_enable_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mmc_receive_interrupt_enable_get(dev_id, index, &reg_val);
	*value = reg_val.bf.rxucgfie;
	return ret;
}

sw_error_t
hppe_mmc_receive_interrupt_enable_rxucgfie_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_mmc_receive_interrupt_enable_rx1024tmaxoctgbfie_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mmc_receive_interrupt_enable_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mmc_receive_interrupt_enable_get(dev_id, index, &reg_val);
	*value = reg_val.bf.rx1024tmaxoctgbfie;
	return ret;
}

sw_error_t
hppe_mmc_receive_interrupt_enable_rx1024tmaxoctgbfie_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_mmc_receive_interrupt_enable_rxpausfie_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mmc_receive_interrupt_enable_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mmc_receive_interrupt_enable_get(dev_id, index, &reg_val);
	*value = reg_val.bf.rxpausfie;
	return ret;
}

sw_error_t
hppe_mmc_receive_interrupt_enable_rxpausfie_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_mmc_receive_interrupt_enable_rxdisfcie_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mmc_receive_interrupt_enable_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mmc_receive_interrupt_enable_get(dev_id, index, &reg_val);
	*value = reg_val.bf.rxdisfcie;
	return ret;
}

sw_error_t
hppe_mmc_receive_interrupt_enable_rxdisfcie_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_mmc_receive_interrupt_enable_rxgoctie_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mmc_receive_interrupt_enable_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mmc_receive_interrupt_enable_get(dev_id, index, &reg_val);
	*value = reg_val.bf.rxgoctie;
	return ret;
}

sw_error_t
hppe_mmc_receive_interrupt_enable_rxgoctie_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_mmc_receive_interrupt_enable_rxgboctie_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mmc_receive_interrupt_enable_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mmc_receive_interrupt_enable_get(dev_id, index, &reg_val);
	*value = reg_val.bf.rxgboctie;
	return ret;
}

sw_error_t
hppe_mmc_receive_interrupt_enable_rxgboctie_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_mmc_receive_interrupt_enable_rx128t255octgbfie_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mmc_receive_interrupt_enable_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mmc_receive_interrupt_enable_get(dev_id, index, &reg_val);
	*value = reg_val.bf.rx128t255octgbfie;
	return ret;
}

sw_error_t
hppe_mmc_receive_interrupt_enable_rx128t255octgbfie_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_mmc_receive_interrupt_enable_rxlpitrcie_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mmc_receive_interrupt_enable_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mmc_receive_interrupt_enable_get(dev_id, index, &reg_val);
	*value = reg_val.bf.rxlpitrcie;
	return ret;
}

sw_error_t
hppe_mmc_receive_interrupt_enable_rxlpitrcie_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_mmc_receive_interrupt_enable_rxbcgfie_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mmc_receive_interrupt_enable_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mmc_receive_interrupt_enable_get(dev_id, index, &reg_val);
	*value = reg_val.bf.rxbcgfie;
	return ret;
}

sw_error_t
hppe_mmc_receive_interrupt_enable_rxbcgfie_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_mmc_receive_interrupt_enable_rx64octgbfie_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mmc_receive_interrupt_enable_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mmc_receive_interrupt_enable_get(dev_id, index, &reg_val);
	*value = reg_val.bf.rx64octgbfie;
	return ret;
}

sw_error_t
hppe_mmc_receive_interrupt_enable_rx64octgbfie_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_mmc_receive_interrupt_enable_rxorangefie_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mmc_receive_interrupt_enable_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mmc_receive_interrupt_enable_get(dev_id, index, &reg_val);
	*value = reg_val.bf.rxorangefie;
	return ret;
}

sw_error_t
hppe_mmc_receive_interrupt_enable_rxorangefie_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_mmc_receive_interrupt_enable_rx512t1023octgbfie_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mmc_receive_interrupt_enable_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mmc_receive_interrupt_enable_get(dev_id, index, &reg_val);
	*value = reg_val.bf.rx512t1023octgbfie;
	return ret;
}

sw_error_t
hppe_mmc_receive_interrupt_enable_rx512t1023octgbfie_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_mmc_transmit_interrupt_enable_txucgbfie_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mmc_transmit_interrupt_enable_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mmc_transmit_interrupt_enable_get(dev_id, index, &reg_val);
	*value = reg_val.bf.txucgbfie;
	return ret;
}

sw_error_t
hppe_mmc_transmit_interrupt_enable_txucgbfie_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_mmc_transmit_interrupt_enable_tx64octgbfie_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mmc_transmit_interrupt_enable_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mmc_transmit_interrupt_enable_get(dev_id, index, &reg_val);
	*value = reg_val.bf.tx64octgbfie;
	return ret;
}

sw_error_t
hppe_mmc_transmit_interrupt_enable_tx64octgbfie_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_mmc_transmit_interrupt_enable_txbcgfie_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mmc_transmit_interrupt_enable_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mmc_transmit_interrupt_enable_get(dev_id, index, &reg_val);
	*value = reg_val.bf.txbcgfie;
	return ret;
}

sw_error_t
hppe_mmc_transmit_interrupt_enable_txbcgfie_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_mmc_transmit_interrupt_enable_txgbfrmie_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mmc_transmit_interrupt_enable_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mmc_transmit_interrupt_enable_get(dev_id, index, &reg_val);
	*value = reg_val.bf.txgbfrmie;
	return ret;
}

sw_error_t
hppe_mmc_transmit_interrupt_enable_txgbfrmie_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_mmc_transmit_interrupt_enable_txgfrmie_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mmc_transmit_interrupt_enable_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mmc_transmit_interrupt_enable_get(dev_id, index, &reg_val);
	*value = reg_val.bf.txgfrmie;
	return ret;
}

sw_error_t
hppe_mmc_transmit_interrupt_enable_txgfrmie_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_mmc_transmit_interrupt_enable_txgoctie_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mmc_transmit_interrupt_enable_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mmc_transmit_interrupt_enable_get(dev_id, index, &reg_val);
	*value = reg_val.bf.txgoctie;
	return ret;
}

sw_error_t
hppe_mmc_transmit_interrupt_enable_txgoctie_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_mmc_transmit_interrupt_enable_txbcgbfie_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mmc_transmit_interrupt_enable_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mmc_transmit_interrupt_enable_get(dev_id, index, &reg_val);
	*value = reg_val.bf.txbcgbfie;
	return ret;
}

sw_error_t
hppe_mmc_transmit_interrupt_enable_txbcgbfie_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_mmc_transmit_interrupt_enable_txlpitrcie_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mmc_transmit_interrupt_enable_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mmc_transmit_interrupt_enable_get(dev_id, index, &reg_val);
	*value = reg_val.bf.txlpitrcie;
	return ret;
}

sw_error_t
hppe_mmc_transmit_interrupt_enable_txlpitrcie_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_mmc_transmit_interrupt_enable_txvlangfie_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mmc_transmit_interrupt_enable_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mmc_transmit_interrupt_enable_get(dev_id, index, &reg_val);
	*value = reg_val.bf.txvlangfie;
	return ret;
}

sw_error_t
hppe_mmc_transmit_interrupt_enable_txvlangfie_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_mmc_transmit_interrupt_enable_txpausfie_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mmc_transmit_interrupt_enable_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mmc_transmit_interrupt_enable_get(dev_id, index, &reg_val);
	*value = reg_val.bf.txpausfie;
	return ret;
}

sw_error_t
hppe_mmc_transmit_interrupt_enable_txpausfie_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_mmc_transmit_interrupt_enable_txgboctie_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mmc_transmit_interrupt_enable_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mmc_transmit_interrupt_enable_get(dev_id, index, &reg_val);
	*value = reg_val.bf.txgboctie;
	return ret;
}

sw_error_t
hppe_mmc_transmit_interrupt_enable_txgboctie_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_mmc_transmit_interrupt_enable_txmcgfie_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mmc_transmit_interrupt_enable_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mmc_transmit_interrupt_enable_get(dev_id, index, &reg_val);
	*value = reg_val.bf.txmcgfie;
	return ret;
}

sw_error_t
hppe_mmc_transmit_interrupt_enable_txmcgfie_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_mmc_transmit_interrupt_enable_txuflowerfie_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mmc_transmit_interrupt_enable_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mmc_transmit_interrupt_enable_get(dev_id, index, &reg_val);
	*value = reg_val.bf.txuflowerfie;
	return ret;
}

sw_error_t
hppe_mmc_transmit_interrupt_enable_txuflowerfie_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_mmc_transmit_interrupt_enable_txlpiuscie_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mmc_transmit_interrupt_enable_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mmc_transmit_interrupt_enable_get(dev_id, index, &reg_val);
	*value = reg_val.bf.txlpiuscie;
	return ret;
}

sw_error_t
hppe_mmc_transmit_interrupt_enable_txlpiuscie_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_mmc_transmit_interrupt_enable_tx256t511octgbfie_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mmc_transmit_interrupt_enable_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mmc_transmit_interrupt_enable_get(dev_id, index, &reg_val);
	*value = reg_val.bf.tx256t511octgbfie;
	return ret;
}

sw_error_t
hppe_mmc_transmit_interrupt_enable_tx256t511octgbfie_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_mmc_transmit_interrupt_enable_tx65t127octgbfie_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mmc_transmit_interrupt_enable_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mmc_transmit_interrupt_enable_get(dev_id, index, &reg_val);
	*value = reg_val.bf.tx65t127octgbfie;
	return ret;
}

sw_error_t
hppe_mmc_transmit_interrupt_enable_tx65t127octgbfie_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_mmc_transmit_interrupt_enable_tx128t255octgbfie_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mmc_transmit_interrupt_enable_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mmc_transmit_interrupt_enable_get(dev_id, index, &reg_val);
	*value = reg_val.bf.tx128t255octgbfie;
	return ret;
}

sw_error_t
hppe_mmc_transmit_interrupt_enable_tx128t255octgbfie_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_mmc_transmit_interrupt_enable_tx512t1023octgbfie_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mmc_transmit_interrupt_enable_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mmc_transmit_interrupt_enable_get(dev_id, index, &reg_val);
	*value = reg_val.bf.tx512t1023octgbfie;
	return ret;
}

sw_error_t
hppe_mmc_transmit_interrupt_enable_tx512t1023octgbfie_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_mmc_transmit_interrupt_enable_tx1024tmaxoctgbfie_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mmc_transmit_interrupt_enable_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mmc_transmit_interrupt_enable_get(dev_id, index, &reg_val);
	*value = reg_val.bf.tx1024tmaxoctgbfie;
	return ret;
}

sw_error_t
hppe_mmc_transmit_interrupt_enable_tx1024tmaxoctgbfie_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_mmc_transmit_interrupt_enable_txprmmcise_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mmc_transmit_interrupt_enable_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mmc_transmit_interrupt_enable_get(dev_id, index, &reg_val);
	*value = reg_val.bf.txprmmcise;
	return ret;
}

sw_error_t
hppe_mmc_transmit_interrupt_enable_txprmmcise_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_mmc_transmit_interrupt_enable_txmcgbfie_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mmc_transmit_interrupt_enable_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mmc_transmit_interrupt_enable_get(dev_id, index, &reg_val);
	*value = reg_val.bf.txmcgbfie;
	return ret;
}

sw_error_t
hppe_mmc_transmit_interrupt_enable_txmcgbfie_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}
#endif

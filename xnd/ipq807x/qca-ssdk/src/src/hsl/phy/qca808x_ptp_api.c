/*
 * Copyright (c) 2018, The Linux Foundation. All rights reserved.
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
#include "hsl_api.h"
#include "hsl.h"
#include "hsl_phy.h"
#include "ssdk_plat.h"
#include "qca808x_ptp_reg.h"
#include "qca808x_ptp_api.h"
#include "qca808x_phy.h"


sw_error_t
qca808x_phy_ptp_reg_read(a_uint32_t dev_id, a_uint32_t phy_id, a_uint32_t reg, a_uint32_t * val)
{
	*val = qca808x_phy_reg_read(dev_id, phy_id, reg);
	return SW_OK;
}

sw_error_t
qca808x_phy_ptp_reg_write(a_uint32_t dev_id, a_uint32_t phy_id, a_uint32_t reg, a_uint32_t val)
{
	return qca808x_phy_reg_write(dev_id, phy_id, reg, (a_uint16_t)val);
}

sw_error_t
qca808x_phy_ptp_mmd_read(a_uint32_t dev_id, a_uint32_t phy_id, a_uint16_t mmd_num,
		a_uint32_t reg, a_uint32_t * val)
{
	*val = qca808x_phy_mmd_read(dev_id, phy_id, mmd_num, (a_uint16_t)reg);
	return SW_OK;
}

sw_error_t
qca808x_phy_ptp_mmd_write(a_uint32_t dev_id, a_uint32_t phy_id, a_uint16_t mmd_num,
		a_uint32_t reg, a_uint32_t val)
{
	return qca808x_phy_mmd_write(dev_id, phy_id, mmd_num, (a_uint16_t)reg, (a_uint16_t)val);
}

sw_error_t
qca808x_ptp_imr_reg_get(a_uint32_t dev_id, a_uint32_t phy_id, union ptp_imr_reg_u *value)
{
	return qca808x_phy_ptp_reg_read(
			dev_id,
			phy_id, PTP_IMR_REG_ADDRESS,
			&value->val);
}

sw_error_t
qca808x_ptp_imr_reg_set(a_uint32_t dev_id, a_uint32_t phy_id, union ptp_imr_reg_u *value)
{
	return qca808x_phy_ptp_reg_write(
			dev_id,
			phy_id, PTP_IMR_REG_ADDRESS,
			value->val);
}

sw_error_t
qca808x_ptp_isr_reg_get(a_uint32_t dev_id, a_uint32_t phy_id, union ptp_isr_reg_u *value)
{
	return qca808x_phy_ptp_reg_read(
			dev_id,
			phy_id, PTP_ISR_REG_ADDRESS,
			&value->val);
}

sw_error_t
qca808x_ptp_isr_reg_set(a_uint32_t dev_id, a_uint32_t phy_id, union ptp_isr_reg_u *value)
{
	return qca808x_phy_ptp_reg_write(
			dev_id,
			phy_id, PTP_ISR_REG_ADDRESS,
			value->val);
}

sw_error_t
qca808x_ptp_hw_enable_reg_get(a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_hw_enable_reg_u *value)
{
	return qca808x_phy_ptp_mmd_read(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_HW_ENABLE_REG_ADDRESS,
			&value->val);
}

sw_error_t
qca808x_ptp_hw_enable_reg_set(a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_hw_enable_reg_u *value)
{
	return qca808x_phy_ptp_mmd_write(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_HW_ENABLE_REG_ADDRESS,
			value->val);
}

sw_error_t
qca808x_ptp_main_conf_reg_get(a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_main_conf_reg_u *value)
{
	return qca808x_phy_ptp_mmd_read(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_MAIN_CONF_REG_ADDRESS,
			&value->val);
}

sw_error_t
qca808x_ptp_main_conf_reg_set(a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_main_conf_reg_u *value)
{
	return qca808x_phy_ptp_mmd_write(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_MAIN_CONF_REG_ADDRESS,
			value->val);
}

sw_error_t
qca808x_ptp_rx_seqid0_reg_get(a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_rx_seqid0_reg_u *value)
{
	return qca808x_phy_ptp_mmd_read(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_RX_SEQID0_REG_ADDRESS,
			&value->val);
}

sw_error_t
qca808x_ptp_rx_seqid0_reg_set(a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_rx_seqid0_reg_u *value)
{
	return qca808x_phy_ptp_mmd_write(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_RX_SEQID0_REG_ADDRESS,
			value->val);
}

sw_error_t
qca808x_ptp_rx_portid0_0_reg_get(a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_rx_portid0_0_reg_u *value)
{
	return qca808x_phy_ptp_mmd_read(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_RX_PORTID0_0_REG_ADDRESS,
			&value->val);
}

sw_error_t
qca808x_ptp_rx_portid0_0_reg_set(a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_rx_portid0_0_reg_u *value)
{
	return qca808x_phy_ptp_mmd_write(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_RX_PORTID0_0_REG_ADDRESS,
			value->val);
}

sw_error_t
qca808x_ptp_rx_portid0_1_reg_get(a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_rx_portid0_1_reg_u *value)
{
	return qca808x_phy_ptp_mmd_read(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_RX_PORTID0_1_REG_ADDRESS,
			&value->val);
}

sw_error_t
qca808x_ptp_rx_portid0_1_reg_set(a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_rx_portid0_1_reg_u *value)
{
	return qca808x_phy_ptp_mmd_write(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_RX_PORTID0_1_REG_ADDRESS,
			value->val);
}

sw_error_t
qca808x_ptp_rx_portid0_2_reg_get(a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_rx_portid0_2_reg_u *value)
{
	return qca808x_phy_ptp_mmd_read(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_RX_PORTID0_2_REG_ADDRESS,
			&value->val);
}

sw_error_t
qca808x_ptp_rx_portid0_2_reg_set(a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_rx_portid0_2_reg_u *value)
{
	return qca808x_phy_ptp_mmd_write(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_RX_PORTID0_2_REG_ADDRESS,
			value->val);
}

sw_error_t
qca808x_ptp_rx_portid0_3_reg_get(a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_rx_portid0_3_reg_u *value)
{
	return qca808x_phy_ptp_mmd_read(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_RX_PORTID0_3_REG_ADDRESS,
			&value->val);
}

sw_error_t
qca808x_ptp_rx_portid0_3_reg_set(a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_rx_portid0_3_reg_u *value)
{
	return qca808x_phy_ptp_mmd_write(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_RX_PORTID0_3_REG_ADDRESS,
			value->val);
}

sw_error_t
qca808x_ptp_rx_portid0_4_reg_get(a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_rx_portid0_4_reg_u *value)
{
	return qca808x_phy_ptp_mmd_read(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_RX_PORTID0_4_REG_ADDRESS,
			&value->val);
}

sw_error_t
qca808x_ptp_rx_portid0_4_reg_set(a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_rx_portid0_4_reg_u *value)
{
	return qca808x_phy_ptp_mmd_write(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_RX_PORTID0_4_REG_ADDRESS,
			value->val);
}

sw_error_t
qca808x_ptp_rtc_clk_reg_get(a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_rtc_clk_reg_u *value)
{
	return qca808x_phy_ptp_mmd_read(
			dev_id,
			phy_id, QCA808X_PHY_MMD7_NUM, PTP_RTC_CLK_REG_ADDRESS,
			&value->val);
}

sw_error_t
qca808x_ptp_rtc_clk_reg_set(a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_rtc_clk_reg_u *value)
{
	return qca808x_phy_ptp_mmd_write(
			dev_id,
			phy_id, QCA808X_PHY_MMD7_NUM, PTP_RTC_CLK_REG_ADDRESS,
			value->val);
}

sw_error_t
qca808x_ptp_rx_ts0_0_reg_get(a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_rx_ts0_0_reg_u *value)
{
	return qca808x_phy_ptp_mmd_read(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_RX_TS0_0_REG_ADDRESS,
			&value->val);
}

sw_error_t
qca808x_ptp_rx_ts0_0_reg_set(a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_rx_ts0_0_reg_u *value)
{
	return qca808x_phy_ptp_mmd_write(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_RX_TS0_0_REG_ADDRESS,
			value->val);
}

sw_error_t
qca808x_ptp_rx_ts0_1_reg_get(a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_rx_ts0_1_reg_u *value)
{
	return qca808x_phy_ptp_mmd_read(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_RX_TS0_1_REG_ADDRESS,
			&value->val);
}

sw_error_t
qca808x_ptp_rx_ts0_1_reg_set(a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_rx_ts0_1_reg_u *value)
{
	return qca808x_phy_ptp_mmd_write(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_RX_TS0_1_REG_ADDRESS,
			value->val);
}

sw_error_t
qca808x_ptp_rx_ts0_2_reg_get(a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_rx_ts0_2_reg_u *value)
{
	return qca808x_phy_ptp_mmd_read(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_RX_TS0_2_REG_ADDRESS,
			&value->val);
}

sw_error_t
qca808x_ptp_rx_ts0_2_reg_set(a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_rx_ts0_2_reg_u *value)
{
	return qca808x_phy_ptp_mmd_write(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_RX_TS0_2_REG_ADDRESS,
			value->val);
}

sw_error_t
qca808x_ptp_rx_ts0_3_reg_get(a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_rx_ts0_3_reg_u *value)
{
	return qca808x_phy_ptp_mmd_read(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_RX_TS0_3_REG_ADDRESS,
			&value->val);
}

sw_error_t
qca808x_ptp_rx_ts0_3_reg_set(a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_rx_ts0_3_reg_u *value)
{
	return qca808x_phy_ptp_mmd_write(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_RX_TS0_3_REG_ADDRESS,
			value->val);
}

sw_error_t
qca808x_ptp_rx_ts0_4_reg_get(a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_rx_ts0_4_reg_u *value)
{
	return qca808x_phy_ptp_mmd_read(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_RX_TS0_4_REG_ADDRESS,
			&value->val);
}

sw_error_t
qca808x_ptp_rx_ts0_4_reg_set(a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_rx_ts0_4_reg_u *value)
{
	return qca808x_phy_ptp_mmd_write(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_RX_TS0_4_REG_ADDRESS,
			value->val);
}

sw_error_t
qca808x_ptp_rx_ts0_5_reg_get(a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_rx_ts0_5_reg_u *value)
{
	return qca808x_phy_ptp_mmd_read(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_RX_TS0_5_REG_ADDRESS,
			&value->val);
}

sw_error_t
qca808x_ptp_rx_ts0_5_reg_set(a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_rx_ts0_5_reg_u *value)
{
	return qca808x_phy_ptp_mmd_write(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_RX_TS0_5_REG_ADDRESS,
			value->val);
}

sw_error_t
qca808x_ptp_rx_ts0_6_reg_get(a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_rx_ts0_6_reg_u *value)
{
	return qca808x_phy_ptp_mmd_read(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_RX_TS0_6_REG_ADDRESS,
			&value->val);
}

sw_error_t
qca808x_ptp_rx_ts0_6_reg_set(a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_rx_ts0_6_reg_u *value)
{
	return qca808x_phy_ptp_mmd_write(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_RX_TS0_6_REG_ADDRESS,
			value->val);
}

sw_error_t
qca808x_ptp_tx_seqid_reg_get(a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_tx_seqid_reg_u *value)
{
	return qca808x_phy_ptp_mmd_read(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_TX_SEQID_REG_ADDRESS,
			&value->val);
}

sw_error_t
qca808x_ptp_tx_seqid_reg_set(a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_tx_seqid_reg_u *value)
{
	return qca808x_phy_ptp_mmd_write(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_TX_SEQID_REG_ADDRESS,
			value->val);
}

sw_error_t
qca808x_ptp_tx_portid0_reg_get(a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_tx_portid0_reg_u *value)
{
	return qca808x_phy_ptp_mmd_read(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_TX_PORTID0_REG_ADDRESS,
			&value->val);
}

sw_error_t
qca808x_ptp_tx_portid0_reg_set(a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_tx_portid0_reg_u *value)
{
	return qca808x_phy_ptp_mmd_write(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_TX_PORTID0_REG_ADDRESS,
			value->val);
}

sw_error_t
qca808x_ptp_tx_portid1_reg_get(a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_tx_portid1_reg_u *value)
{
	return qca808x_phy_ptp_mmd_read(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_TX_PORTID1_REG_ADDRESS,
			&value->val);
}

sw_error_t
qca808x_ptp_tx_portid1_reg_set(a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_tx_portid1_reg_u *value)
{
	return qca808x_phy_ptp_mmd_write(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_TX_PORTID1_REG_ADDRESS,
			value->val);
}

sw_error_t
qca808x_ptp_tx_portid2_reg_get(a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_tx_portid2_reg_u *value)
{
	return qca808x_phy_ptp_mmd_read(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_TX_PORTID2_REG_ADDRESS,
			&value->val);
}

sw_error_t
qca808x_ptp_tx_portid2_reg_set(a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_tx_portid2_reg_u *value)
{
	return qca808x_phy_ptp_mmd_write(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_TX_PORTID2_REG_ADDRESS,
			value->val);
}

sw_error_t
qca808x_ptp_tx_portid3_reg_get(a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_tx_portid3_reg_u *value)
{
	return qca808x_phy_ptp_mmd_read(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_TX_PORTID3_REG_ADDRESS,
			&value->val);
}

sw_error_t
qca808x_ptp_tx_portid3_reg_set(a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_tx_portid3_reg_u *value)
{
	return qca808x_phy_ptp_mmd_write(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_TX_PORTID3_REG_ADDRESS,
			value->val);
}

sw_error_t
qca808x_ptp_tx_portid4_reg_get(a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_tx_portid4_reg_u *value)
{
	return qca808x_phy_ptp_mmd_read(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_TX_PORTID4_REG_ADDRESS,
			&value->val);
}

sw_error_t
qca808x_ptp_tx_portid4_reg_set(a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_tx_portid4_reg_u *value)
{
	return qca808x_phy_ptp_mmd_write(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_TX_PORTID4_REG_ADDRESS,
			value->val);
}

sw_error_t
qca808x_ptp_tx_ts0_reg_get(a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_tx_ts0_reg_u *value)
{
	return qca808x_phy_ptp_mmd_read(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_TX_TS0_REG_ADDRESS,
			&value->val);
}

sw_error_t
qca808x_ptp_tx_ts0_reg_set(a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_tx_ts0_reg_u *value)
{
	return qca808x_phy_ptp_mmd_write(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_TX_TS0_REG_ADDRESS,
			value->val);
}

sw_error_t
qca808x_ptp_tx_ts1_reg_get(a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_tx_ts1_reg_u *value)
{
	return qca808x_phy_ptp_mmd_read(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_TX_TS1_REG_ADDRESS,
			&value->val);
}

sw_error_t
qca808x_ptp_tx_ts1_reg_set(a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_tx_ts1_reg_u *value)
{
	return qca808x_phy_ptp_mmd_write(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_TX_TS1_REG_ADDRESS,
			value->val);
}

sw_error_t
qca808x_ptp_tx_ts2_reg_get(a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_tx_ts2_reg_u *value)
{
	return qca808x_phy_ptp_mmd_read(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_TX_TS2_REG_ADDRESS,
			&value->val);
}

sw_error_t
qca808x_ptp_tx_ts2_reg_set(a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_tx_ts2_reg_u *value)
{
	return qca808x_phy_ptp_mmd_write(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_TX_TS2_REG_ADDRESS,
			value->val);
}

sw_error_t
qca808x_ptp_tx_ts3_reg_get(a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_tx_ts3_reg_u *value)
{
	return qca808x_phy_ptp_mmd_read(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_TX_TS3_REG_ADDRESS,
			&value->val);
}

sw_error_t
qca808x_ptp_tx_ts3_reg_set(a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_tx_ts3_reg_u *value)
{
	return qca808x_phy_ptp_mmd_write(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_TX_TS3_REG_ADDRESS,
			value->val);
}

sw_error_t
qca808x_ptp_tx_ts4_reg_get(a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_tx_ts4_reg_u *value)
{
	return qca808x_phy_ptp_mmd_read(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_TX_TS4_REG_ADDRESS,
			&value->val);
}

sw_error_t
qca808x_ptp_tx_ts4_reg_set(a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_tx_ts4_reg_u *value)
{
	return qca808x_phy_ptp_mmd_write(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_TX_TS4_REG_ADDRESS,
			value->val);
}

sw_error_t
qca808x_ptp_tx_ts5_reg_get(a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_tx_ts5_reg_u *value)
{
	return qca808x_phy_ptp_mmd_read(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_TX_TS5_REG_ADDRESS,
			&value->val);
}

sw_error_t
qca808x_ptp_tx_ts5_reg_set(a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_tx_ts5_reg_u *value)
{
	return qca808x_phy_ptp_mmd_write(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_TX_TS5_REG_ADDRESS,
			value->val);
}

sw_error_t
qca808x_ptp_tx_ts6_reg_get(a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_tx_ts6_reg_u *value)
{
	return qca808x_phy_ptp_mmd_read(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_TX_TS6_REG_ADDRESS,
			&value->val);
}

sw_error_t
qca808x_ptp_tx_ts6_reg_set(a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_tx_ts6_reg_u *value)
{
	return qca808x_phy_ptp_mmd_write(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_TX_TS6_REG_ADDRESS,
			value->val);
}

sw_error_t
qca808x_ptp_orig_corr0_reg_get(a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_orig_corr0_reg_u *value)
{
	return qca808x_phy_ptp_mmd_read(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_ORIG_CORR0_REG_ADDRESS,
			&value->val);
}

sw_error_t
qca808x_ptp_orig_corr0_reg_set(a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_orig_corr0_reg_u *value)
{
	return qca808x_phy_ptp_mmd_write(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_ORIG_CORR0_REG_ADDRESS,
			value->val);
}

sw_error_t
qca808x_ptp_orig_corr1_reg_get(a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_orig_corr1_reg_u *value)
{
	return qca808x_phy_ptp_mmd_read(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_ORIG_CORR1_REG_ADDRESS,
			&value->val);
}

sw_error_t
qca808x_ptp_orig_corr1_reg_set(a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_orig_corr1_reg_u *value)
{
	return qca808x_phy_ptp_mmd_write(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_ORIG_CORR1_REG_ADDRESS,
			value->val);
}

sw_error_t
qca808x_ptp_orig_corr2_reg_get(a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_orig_corr2_reg_u *value)
{
	return qca808x_phy_ptp_mmd_read(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_ORIG_CORR2_REG_ADDRESS,
			&value->val);
}

sw_error_t
qca808x_ptp_orig_corr2_reg_set(a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_orig_corr2_reg_u *value)
{
	return qca808x_phy_ptp_mmd_write(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_ORIG_CORR2_REG_ADDRESS,
			value->val);
}

sw_error_t
qca808x_ptp_orig_corr3_reg_get(a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_orig_corr3_reg_u *value)
{
	return qca808x_phy_ptp_mmd_read(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_ORIG_CORR3_REG_ADDRESS,
			&value->val);
}

sw_error_t
qca808x_ptp_orig_corr3_reg_set(a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_orig_corr3_reg_u *value)
{
	return qca808x_phy_ptp_mmd_write(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_ORIG_CORR3_REG_ADDRESS,
			value->val);
}

sw_error_t
qca808x_ptp_in_trig0_reg_get(a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_in_trig0_reg_u *value)
{
	return qca808x_phy_ptp_mmd_read(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_IN_TRIG0_REG_ADDRESS,
			&value->val);
}

sw_error_t
qca808x_ptp_in_trig0_reg_set(a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_in_trig0_reg_u *value)
{
	return qca808x_phy_ptp_mmd_write(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_IN_TRIG0_REG_ADDRESS,
			value->val);
}

sw_error_t
qca808x_ptp_in_trig1_reg_get(a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_in_trig1_reg_u *value)
{
	return qca808x_phy_ptp_mmd_read(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_IN_TRIG1_REG_ADDRESS,
			&value->val);
}

sw_error_t
qca808x_ptp_in_trig1_reg_set(a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_in_trig1_reg_u *value)
{
	return qca808x_phy_ptp_mmd_write(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_IN_TRIG1_REG_ADDRESS,
			value->val);
}

sw_error_t
qca808x_ptp_in_trig2_reg_get(a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_in_trig2_reg_u *value)
{
	return qca808x_phy_ptp_mmd_read(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_IN_TRIG2_REG_ADDRESS,
			&value->val);
}

sw_error_t
qca808x_ptp_in_trig2_reg_set(a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_in_trig2_reg_u *value)
{
	return qca808x_phy_ptp_mmd_write(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_IN_TRIG2_REG_ADDRESS,
			value->val);
}

sw_error_t
qca808x_ptp_in_trig3_reg_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_in_trig3_reg_u *value)
{
	return qca808x_phy_ptp_mmd_read(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_IN_TRIG3_REG_ADDRESS,
			&value->val);
}

sw_error_t
qca808x_ptp_in_trig3_reg_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_in_trig3_reg_u *value)
{
	return qca808x_phy_ptp_mmd_write(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_IN_TRIG3_REG_ADDRESS,
			value->val);
}

sw_error_t
qca808x_ptp_tx_latency_reg_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_tx_latency_reg_u *value)
{
	return qca808x_phy_ptp_mmd_read(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_TX_LATENCY_REG_ADDRESS,
			&value->val);
}

sw_error_t
qca808x_ptp_tx_latency_reg_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_tx_latency_reg_u *value)
{
	return qca808x_phy_ptp_mmd_write(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_TX_LATENCY_REG_ADDRESS,
			value->val);
}

sw_error_t
qca808x_ptp_rtc_inc0_reg_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_rtc_inc0_reg_u *value)
{
	return qca808x_phy_ptp_mmd_read(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_RTC_INC0_REG_ADDRESS,
			&value->val);
}

sw_error_t
qca808x_ptp_rtc_inc0_reg_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_rtc_inc0_reg_u *value)
{
	return qca808x_phy_ptp_mmd_write(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_RTC_INC0_REG_ADDRESS,
			value->val);
}

sw_error_t
qca808x_ptp_rtc_inc1_reg_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_rtc_inc1_reg_u *value)
{
	return qca808x_phy_ptp_mmd_read(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_RTC_INC1_REG_ADDRESS,
			&value->val);
}

sw_error_t
qca808x_ptp_rtc_inc1_reg_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_rtc_inc1_reg_u *value)
{
	return qca808x_phy_ptp_mmd_write(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_RTC_INC1_REG_ADDRESS,
			value->val);
}

sw_error_t
qca808x_ptp_rtcoffs0_reg_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_rtcoffs0_reg_u *value)
{
	return qca808x_phy_ptp_mmd_read(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_RTCOFFS0_REG_ADDRESS,
			&value->val);
}

sw_error_t
qca808x_ptp_rtcoffs0_reg_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_rtcoffs0_reg_u *value)
{
	return qca808x_phy_ptp_mmd_write(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_RTCOFFS0_REG_ADDRESS,
			value->val);
}

sw_error_t
qca808x_ptp_rtcoffs1_reg_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_rtcoffs1_reg_u *value)
{
	return qca808x_phy_ptp_mmd_read(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_RTCOFFS1_REG_ADDRESS,
			&value->val);
}

sw_error_t
qca808x_ptp_rtcoffs1_reg_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_rtcoffs1_reg_u *value)
{
	return qca808x_phy_ptp_mmd_write(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_RTCOFFS1_REG_ADDRESS,
			value->val);
}

sw_error_t
qca808x_ptp_rtcoffs2_reg_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_rtcoffs2_reg_u *value)
{
	return qca808x_phy_ptp_mmd_read(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_RTCOFFS2_REG_ADDRESS,
			&value->val);
}

sw_error_t
qca808x_ptp_rtcoffs2_reg_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_rtcoffs2_reg_u *value)
{
	return qca808x_phy_ptp_mmd_write(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_RTCOFFS2_REG_ADDRESS,
			value->val);
}

sw_error_t
qca808x_ptp_rtcoffs3_reg_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_rtcoffs3_reg_u *value)
{
	return qca808x_phy_ptp_mmd_read(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_RTCOFFS3_REG_ADDRESS,
			&value->val);
}

sw_error_t
qca808x_ptp_rtcoffs3_reg_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_rtcoffs3_reg_u *value)
{
	return qca808x_phy_ptp_mmd_write(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_RTCOFFS3_REG_ADDRESS,
			value->val);
}

sw_error_t
qca808x_ptp_rtcoffs4_reg_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_rtcoffs4_reg_u *value)
{
	return qca808x_phy_ptp_mmd_read(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_RTCOFFS4_REG_ADDRESS,
			&value->val);
}

sw_error_t
qca808x_ptp_rtcoffs4_reg_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_rtcoffs4_reg_u *value)
{
	return qca808x_phy_ptp_mmd_write(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_RTCOFFS4_REG_ADDRESS,
			value->val);
}

sw_error_t
qca808x_ptp_rtc0_reg_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_rtc0_reg_u *value)
{
	return qca808x_phy_ptp_mmd_read(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_RTC0_REG_ADDRESS,
			&value->val);
}

sw_error_t
qca808x_ptp_rtc0_reg_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_rtc0_reg_u *value)
{
	return qca808x_phy_ptp_mmd_write(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_RTC0_REG_ADDRESS,
			value->val);
}

sw_error_t
qca808x_ptp_rtc1_reg_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_rtc1_reg_u *value)
{
	return qca808x_phy_ptp_mmd_read(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_RTC1_REG_ADDRESS,
			&value->val);
}

sw_error_t
qca808x_ptp_rtc1_reg_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_rtc1_reg_u *value)
{
	return qca808x_phy_ptp_mmd_write(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_RTC1_REG_ADDRESS,
			value->val);
}

sw_error_t
qca808x_ptp_rtc2_reg_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_rtc2_reg_u *value)
{
	return qca808x_phy_ptp_mmd_read(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_RTC2_REG_ADDRESS,
			&value->val);
}

sw_error_t
qca808x_ptp_rtc2_reg_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_rtc2_reg_u *value)
{
	return qca808x_phy_ptp_mmd_write(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_RTC2_REG_ADDRESS,
			value->val);
}

sw_error_t
qca808x_ptp_rtc3_reg_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_rtc3_reg_u *value)
{
	return qca808x_phy_ptp_mmd_read(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_RTC3_REG_ADDRESS,
			&value->val);
}

sw_error_t
qca808x_ptp_rtc3_reg_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_rtc3_reg_u *value)
{
	return qca808x_phy_ptp_mmd_write(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_RTC3_REG_ADDRESS,
			value->val);
}

sw_error_t
qca808x_ptp_rtc4_reg_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_rtc4_reg_u *value)
{
	return qca808x_phy_ptp_mmd_read(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_RTC4_REG_ADDRESS,
			&value->val);
}

sw_error_t
qca808x_ptp_rtc4_reg_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_rtc4_reg_u *value)
{
	return qca808x_phy_ptp_mmd_write(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_RTC4_REG_ADDRESS,
			value->val);
}

sw_error_t
qca808x_ptp_rtc5_reg_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_rtc5_reg_u *value)
{
	return qca808x_phy_ptp_mmd_read(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_RTC5_REG_ADDRESS,
			&value->val);
}

sw_error_t
qca808x_ptp_rtc5_reg_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_rtc5_reg_u *value)
{
	return qca808x_phy_ptp_mmd_write(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_RTC5_REG_ADDRESS,
			value->val);
}

sw_error_t
qca808x_ptp_rtc6_reg_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_rtc6_reg_u *value)
{
	return qca808x_phy_ptp_mmd_read(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_RTC6_REG_ADDRESS,
			&value->val);
}

sw_error_t
qca808x_ptp_rtc6_reg_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_rtc6_reg_u *value)
{
	return qca808x_phy_ptp_mmd_write(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_RTC6_REG_ADDRESS,
			value->val);
}

sw_error_t
qca808x_ptp_rtcoffs_valid_reg_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_rtcoffs_valid_reg_u *value)
{
	return qca808x_phy_ptp_mmd_read(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_RTCOFFS_VALID_REG_ADDRESS,
			&value->val);
}

sw_error_t
qca808x_ptp_rtcoffs_valid_reg_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_rtcoffs_valid_reg_u *value)
{
	return qca808x_phy_ptp_mmd_write(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_RTCOFFS_VALID_REG_ADDRESS,
			value->val);
}

sw_error_t
qca808x_ptp_misc_config_reg_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_misc_config_reg_u *value)
{
	return qca808x_phy_ptp_mmd_read(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_MISC_CONFIG_REG_ADDRESS,
			&value->val);
}

sw_error_t
qca808x_ptp_misc_config_reg_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_misc_config_reg_u *value)
{
	return qca808x_phy_ptp_mmd_write(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_MISC_CONFIG_REG_ADDRESS,
			value->val);
}

sw_error_t
qca808x_ptp_ext_imr_reg_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_ext_imr_reg_u *value)
{
	return qca808x_phy_ptp_mmd_read(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_EXT_IMR_REG_ADDRESS,
			&value->val);
}

sw_error_t
qca808x_ptp_ext_imr_reg_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_ext_imr_reg_u *value)
{
	return qca808x_phy_ptp_mmd_write(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_EXT_IMR_REG_ADDRESS,
			value->val);
}

sw_error_t
qca808x_ptp_ext_isr_reg_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_ext_isr_reg_u *value)
{
	return qca808x_phy_ptp_mmd_read(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_EXT_ISR_REG_ADDRESS,
			&value->val);
}

sw_error_t
qca808x_ptp_ext_isr_reg_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_ext_isr_reg_u *value)
{
	return qca808x_phy_ptp_mmd_write(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_EXT_ISR_REG_ADDRESS,
			value->val);
}

sw_error_t
qca808x_ptp_rtc_ext_conf_reg_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_rtc_ext_conf_reg_u *value)
{
	return qca808x_phy_ptp_mmd_read(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_RTC_EXT_CONF_REG_ADDRESS,
			&value->val);
}

sw_error_t
qca808x_ptp_rtc_ext_conf_reg_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_rtc_ext_conf_reg_u *value)
{
	return qca808x_phy_ptp_mmd_write(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_RTC_EXT_CONF_REG_ADDRESS,
			value->val);
}

sw_error_t
qca808x_ptp_rtc_preloaded0_reg_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_rtc_preloaded0_reg_u *value)
{
	return qca808x_phy_ptp_mmd_read(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_RTC_PRELOADED0_REG_ADDRESS,
			&value->val);
}

sw_error_t
qca808x_ptp_rtc_preloaded0_reg_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_rtc_preloaded0_reg_u *value)
{
	return qca808x_phy_ptp_mmd_write(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_RTC_PRELOADED0_REG_ADDRESS,
			value->val);
}

sw_error_t
qca808x_ptp_rtc_preloaded1_reg_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_rtc_preloaded1_reg_u *value)
{
	return qca808x_phy_ptp_mmd_read(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_RTC_PRELOADED1_REG_ADDRESS,
			&value->val);
}

sw_error_t
qca808x_ptp_rtc_preloaded1_reg_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_rtc_preloaded1_reg_u *value)
{
	return qca808x_phy_ptp_mmd_write(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_RTC_PRELOADED1_REG_ADDRESS,
			value->val);
}

sw_error_t
qca808x_ptp_rtc_preloaded2_reg_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_rtc_preloaded2_reg_u *value)
{
	return qca808x_phy_ptp_mmd_read(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_RTC_PRELOADED2_REG_ADDRESS,
			&value->val);
}

sw_error_t
qca808x_ptp_rtc_preloaded2_reg_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_rtc_preloaded2_reg_u *value)
{
	return qca808x_phy_ptp_mmd_write(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_RTC_PRELOADED2_REG_ADDRESS,
			value->val);
}

sw_error_t
qca808x_ptp_rtc_preloaded3_reg_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_rtc_preloaded3_reg_u *value)
{
	return qca808x_phy_ptp_mmd_read(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_RTC_PRELOADED3_REG_ADDRESS,
			&value->val);
}

sw_error_t
qca808x_ptp_rtc_preloaded3_reg_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_rtc_preloaded3_reg_u *value)
{
	return qca808x_phy_ptp_mmd_write(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_RTC_PRELOADED3_REG_ADDRESS,
			value->val);
}

sw_error_t
qca808x_ptp_rtc_preloaded4_reg_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_rtc_preloaded4_reg_u *value)
{
	return qca808x_phy_ptp_mmd_read(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_RTC_PRELOADED4_REG_ADDRESS,
			&value->val);
}

sw_error_t
qca808x_ptp_rtc_preloaded4_reg_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_rtc_preloaded4_reg_u *value)
{
	return qca808x_phy_ptp_mmd_write(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_RTC_PRELOADED4_REG_ADDRESS,
			value->val);
}

sw_error_t
qca808x_ptp_gm_conf0_reg_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_gm_conf0_reg_u *value)
{
	return qca808x_phy_ptp_mmd_read(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_GM_CONF0_REG_ADDRESS,
			&value->val);
}

sw_error_t
qca808x_ptp_gm_conf0_reg_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_gm_conf0_reg_u *value)
{
	return qca808x_phy_ptp_mmd_write(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_GM_CONF0_REG_ADDRESS,
			value->val);
}

sw_error_t
qca808x_ptp_gm_conf1_reg_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_gm_conf1_reg_u *value)
{
	return qca808x_phy_ptp_mmd_read(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_GM_CONF1_REG_ADDRESS,
			&value->val);
}

sw_error_t
qca808x_ptp_gm_conf1_reg_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_gm_conf1_reg_u *value)
{
	return qca808x_phy_ptp_mmd_write(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_GM_CONF1_REG_ADDRESS,
			value->val);
}

sw_error_t
qca808x_ptp_ppsin_ts0_reg_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_ppsin_ts0_reg_u *value)
{
	return qca808x_phy_ptp_mmd_read(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_PPSIN_TS0_REG_ADDRESS,
			&value->val);
}

sw_error_t
qca808x_ptp_ppsin_ts0_reg_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_ppsin_ts0_reg_u *value)
{
	return qca808x_phy_ptp_mmd_write(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_PPSIN_TS0_REG_ADDRESS,
			value->val);
}

sw_error_t
qca808x_ptp_ppsin_ts1_reg_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_ppsin_ts1_reg_u *value)
{
	return qca808x_phy_ptp_mmd_read(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_PPSIN_TS1_REG_ADDRESS,
			&value->val);
}

sw_error_t
qca808x_ptp_ppsin_ts1_reg_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_ppsin_ts1_reg_u *value)
{
	return qca808x_phy_ptp_mmd_write(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_PPSIN_TS1_REG_ADDRESS,
			value->val);
}

sw_error_t
qca808x_ptp_ppsin_ts2_reg_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_ppsin_ts2_reg_u *value)
{
	return qca808x_phy_ptp_mmd_read(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_PPSIN_TS2_REG_ADDRESS,
			&value->val);
}

sw_error_t
qca808x_ptp_ppsin_ts2_reg_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_ppsin_ts2_reg_u *value)
{
	return qca808x_phy_ptp_mmd_write(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_PPSIN_TS2_REG_ADDRESS,
			value->val);
}

sw_error_t
qca808x_ptp_ppsin_ts3_reg_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_ppsin_ts3_reg_u *value)
{
	return qca808x_phy_ptp_mmd_read(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_PPSIN_TS3_REG_ADDRESS,
			&value->val);
}

sw_error_t
qca808x_ptp_ppsin_ts3_reg_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_ppsin_ts3_reg_u *value)
{
	return qca808x_phy_ptp_mmd_write(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_PPSIN_TS3_REG_ADDRESS,
			value->val);
}

sw_error_t
qca808x_ptp_ppsin_ts4_reg_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_ppsin_ts4_reg_u *value)
{
	return qca808x_phy_ptp_mmd_read(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_PPSIN_TS4_REG_ADDRESS,
			&value->val);
}

sw_error_t
qca808x_ptp_ppsin_ts4_reg_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_ppsin_ts4_reg_u *value)
{
	return qca808x_phy_ptp_mmd_write(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_PPSIN_TS4_REG_ADDRESS,
			value->val);
}

sw_error_t
qca808x_ptp_hwpll_inc0_reg_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_hwpll_inc0_reg_u *value)
{
	return qca808x_phy_ptp_mmd_read(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_HWPLL_INC0_REG_ADDRESS,
			&value->val);
}

sw_error_t
qca808x_ptp_hwpll_inc0_reg_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_hwpll_inc0_reg_u *value)
{
	return qca808x_phy_ptp_mmd_write(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_HWPLL_INC0_REG_ADDRESS,
			value->val);
}

sw_error_t
qca808x_ptp_hwpll_inc1_reg_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_hwpll_inc1_reg_u *value)
{
	return qca808x_phy_ptp_mmd_read(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_HWPLL_INC1_REG_ADDRESS,
			&value->val);
}

sw_error_t
qca808x_ptp_hwpll_inc1_reg_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_hwpll_inc1_reg_u *value)
{
	return qca808x_phy_ptp_mmd_write(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_HWPLL_INC1_REG_ADDRESS,
			value->val);
}

sw_error_t
qca808x_ptp_ppsin_latency_reg_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_ppsin_latency_reg_u *value)
{
	return qca808x_phy_ptp_mmd_read(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_PPSIN_LATENCY_REG_ADDRESS,
			&value->val);
}

sw_error_t
qca808x_ptp_ppsin_latency_reg_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_ppsin_latency_reg_u *value)
{
	return qca808x_phy_ptp_mmd_write(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_PPSIN_LATENCY_REG_ADDRESS,
			value->val);
}

sw_error_t
qca808x_ptp_trigger0_config_reg_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_trigger0_config_reg_u *value)
{
	return qca808x_phy_ptp_mmd_read(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_TRIGGER0_CONFIG_REG_ADDRESS,
			&value->val);
}

sw_error_t
qca808x_ptp_trigger0_config_reg_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_trigger0_config_reg_u *value)
{
	return qca808x_phy_ptp_mmd_write(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_TRIGGER0_CONFIG_REG_ADDRESS,
			value->val);
}

sw_error_t
qca808x_ptp_trigger0_status_reg_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_trigger0_status_reg_u *value)
{
	return qca808x_phy_ptp_mmd_read(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_TRIGGER0_STATUS_REG_ADDRESS,
			&value->val);
}

sw_error_t
qca808x_ptp_trigger0_status_reg_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_trigger0_status_reg_u *value)
{
	return qca808x_phy_ptp_mmd_write(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_TRIGGER0_STATUS_REG_ADDRESS,
			value->val);
}

sw_error_t
qca808x_ptp_trigger1_config_reg_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_trigger1_config_reg_u *value)
{
	return qca808x_phy_ptp_mmd_read(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_TRIGGER1_CONFIG_REG_ADDRESS,
			&value->val);
}

sw_error_t
qca808x_ptp_trigger1_config_reg_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_trigger1_config_reg_u *value)
{
	return qca808x_phy_ptp_mmd_write(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_TRIGGER1_CONFIG_REG_ADDRESS,
			value->val);
}

sw_error_t
qca808x_ptp_trigger1_status_reg_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_trigger1_status_reg_u *value)
{
	return qca808x_phy_ptp_mmd_read(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_TRIGGER1_STATUS_REG_ADDRESS,
			&value->val);
}

sw_error_t
qca808x_ptp_trigger1_status_reg_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_trigger1_status_reg_u *value)
{
	return qca808x_phy_ptp_mmd_write(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_TRIGGER1_STATUS_REG_ADDRESS,
			value->val);
}

sw_error_t
qca808x_ptp_trigger0_timestamp0_reg_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_trigger0_timestamp0_reg_u *value)
{
	return qca808x_phy_ptp_mmd_read(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_TRIGGER0_TIMESTAMP0_REG_ADDRESS,
			&value->val);
}

sw_error_t
qca808x_ptp_trigger0_timestamp0_reg_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_trigger0_timestamp0_reg_u *value)
{
	return qca808x_phy_ptp_mmd_write(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_TRIGGER0_TIMESTAMP0_REG_ADDRESS,
			value->val);
}

sw_error_t
qca808x_ptp_trigger0_timestamp1_reg_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_trigger0_timestamp1_reg_u *value)
{
	return qca808x_phy_ptp_mmd_read(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_TRIGGER0_TIMESTAMP1_REG_ADDRESS,
			&value->val);
}

sw_error_t
qca808x_ptp_trigger0_timestamp1_reg_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_trigger0_timestamp1_reg_u *value)
{
	return qca808x_phy_ptp_mmd_write(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_TRIGGER0_TIMESTAMP1_REG_ADDRESS,
			value->val);
}

sw_error_t
qca808x_ptp_trigger0_timestamp2_reg_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_trigger0_timestamp2_reg_u *value)
{
	return qca808x_phy_ptp_mmd_read(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_TRIGGER0_TIMESTAMP2_REG_ADDRESS,
			&value->val);
}

sw_error_t
qca808x_ptp_trigger0_timestamp2_reg_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_trigger0_timestamp2_reg_u *value)
{
	return qca808x_phy_ptp_mmd_write(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_TRIGGER0_TIMESTAMP2_REG_ADDRESS,
			value->val);
}

sw_error_t
qca808x_ptp_trigger0_timestamp3_reg_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_trigger0_timestamp3_reg_u *value)
{
	return qca808x_phy_ptp_mmd_read(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_TRIGGER0_TIMESTAMP3_REG_ADDRESS,
			&value->val);
}

sw_error_t
qca808x_ptp_trigger0_timestamp3_reg_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_trigger0_timestamp3_reg_u *value)
{
	return qca808x_phy_ptp_mmd_write(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_TRIGGER0_TIMESTAMP3_REG_ADDRESS,
			value->val);
}

sw_error_t
qca808x_ptp_trigger0_timestamp4_reg_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_trigger0_timestamp4_reg_u *value)
{
	return qca808x_phy_ptp_mmd_read(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_TRIGGER0_TIMESTAMP4_REG_ADDRESS,
			&value->val);
}

sw_error_t
qca808x_ptp_trigger0_timestamp4_reg_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_trigger0_timestamp4_reg_u *value)
{
	return qca808x_phy_ptp_mmd_write(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_TRIGGER0_TIMESTAMP4_REG_ADDRESS,
			value->val);
}

sw_error_t
qca808x_ptp_trigger1_timestamp0_reg_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_trigger1_timestamp0_reg_u *value)
{
	return qca808x_phy_ptp_mmd_read(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_TRIGGER1_TIMESTAMP0_REG_ADDRESS,
			&value->val);
}

sw_error_t
qca808x_ptp_trigger1_timestamp0_reg_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_trigger1_timestamp0_reg_u *value)
{
	return qca808x_phy_ptp_mmd_write(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_TRIGGER1_TIMESTAMP0_REG_ADDRESS,
			value->val);
}

sw_error_t
qca808x_ptp_trigger1_timestamp1_reg_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_trigger1_timestamp1_reg_u *value)
{
	return qca808x_phy_ptp_mmd_read(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_TRIGGER1_TIMESTAMP1_REG_ADDRESS,
			&value->val);
}

sw_error_t
qca808x_ptp_trigger1_timestamp1_reg_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_trigger1_timestamp1_reg_u *value)
{
	return qca808x_phy_ptp_mmd_write(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_TRIGGER1_TIMESTAMP1_REG_ADDRESS,
			value->val);
}

sw_error_t
qca808x_ptp_trigger1_timestamp2_reg_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_trigger1_timestamp2_reg_u *value)
{
	return qca808x_phy_ptp_mmd_read(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_TRIGGER1_TIMESTAMP2_REG_ADDRESS,
			&value->val);
}

sw_error_t
qca808x_ptp_trigger1_timestamp2_reg_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_trigger1_timestamp2_reg_u *value)
{
	return qca808x_phy_ptp_mmd_write(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_TRIGGER1_TIMESTAMP2_REG_ADDRESS,
			value->val);
}

sw_error_t
qca808x_ptp_trigger1_timestamp3_reg_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_trigger1_timestamp3_reg_u *value)
{
	return qca808x_phy_ptp_mmd_read(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_TRIGGER1_TIMESTAMP3_REG_ADDRESS,
			&value->val);
}

sw_error_t
qca808x_ptp_trigger1_timestamp3_reg_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_trigger1_timestamp3_reg_u *value)
{
	return qca808x_phy_ptp_mmd_write(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_TRIGGER1_TIMESTAMP3_REG_ADDRESS,
			value->val);
}

sw_error_t
qca808x_ptp_trigger1_timestamp4_reg_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_trigger1_timestamp4_reg_u *value)
{
	return qca808x_phy_ptp_mmd_read(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_TRIGGER1_TIMESTAMP4_REG_ADDRESS,
			&value->val);
}

sw_error_t
qca808x_ptp_trigger1_timestamp4_reg_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_trigger1_timestamp4_reg_u *value)
{
	return qca808x_phy_ptp_mmd_write(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_TRIGGER1_TIMESTAMP4_REG_ADDRESS,
			value->val);
}

sw_error_t
qca808x_ptp_event0_config_reg_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_event0_config_reg_u *value)
{
	return qca808x_phy_ptp_mmd_read(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_EVENT0_CONFIG_REG_ADDRESS,
			&value->val);
}

sw_error_t
qca808x_ptp_event0_config_reg_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_event0_config_reg_u *value)
{
	return qca808x_phy_ptp_mmd_write(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_EVENT0_CONFIG_REG_ADDRESS,
			value->val);
}

sw_error_t
qca808x_ptp_event0_status_reg_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_event0_status_reg_u *value)
{
	return qca808x_phy_ptp_mmd_read(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_EVENT0_STATUS_REG_ADDRESS,
			&value->val);
}

sw_error_t
qca808x_ptp_event0_status_reg_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_event0_status_reg_u *value)
{
	return qca808x_phy_ptp_mmd_write(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_EVENT0_STATUS_REG_ADDRESS,
			value->val);
}

sw_error_t
qca808x_ptp_event1_config_reg_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_event1_config_reg_u *value)
{
	return qca808x_phy_ptp_mmd_read(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_EVENT1_CONFIG_REG_ADDRESS,
			&value->val);
}

sw_error_t
qca808x_ptp_event1_config_reg_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_event1_config_reg_u *value)
{
	return qca808x_phy_ptp_mmd_write(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_EVENT1_CONFIG_REG_ADDRESS,
			value->val);
}

sw_error_t
qca808x_ptp_event1_status_reg_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_event1_status_reg_u *value)
{
	return qca808x_phy_ptp_mmd_read(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_EVENT1_STATUS_REG_ADDRESS,
			&value->val);
}

sw_error_t
qca808x_ptp_event1_status_reg_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_event1_status_reg_u *value)
{
	return qca808x_phy_ptp_mmd_write(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_EVENT1_STATUS_REG_ADDRESS,
			value->val);
}

sw_error_t
qca808x_ptp_event0_timestamp0_reg_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_event0_timestamp0_reg_u *value)
{
	return qca808x_phy_ptp_mmd_read(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_EVENT0_TIMESTAMP0_REG_ADDRESS,
			&value->val);
}

sw_error_t
qca808x_ptp_event0_timestamp0_reg_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_event0_timestamp0_reg_u *value)
{
	return qca808x_phy_ptp_mmd_write(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_EVENT0_TIMESTAMP0_REG_ADDRESS,
			value->val);
}

sw_error_t
qca808x_ptp_event0_timestamp1_reg_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_event0_timestamp1_reg_u *value)
{
	return qca808x_phy_ptp_mmd_read(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_EVENT0_TIMESTAMP1_REG_ADDRESS,
			&value->val);
}

sw_error_t
qca808x_ptp_event0_timestamp1_reg_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_event0_timestamp1_reg_u *value)
{
	return qca808x_phy_ptp_mmd_write(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_EVENT0_TIMESTAMP1_REG_ADDRESS,
			value->val);
}

sw_error_t
qca808x_ptp_event0_timestamp2_reg_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_event0_timestamp2_reg_u *value)
{
	return qca808x_phy_ptp_mmd_read(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_EVENT0_TIMESTAMP2_REG_ADDRESS,
			&value->val);
}

sw_error_t
qca808x_ptp_event0_timestamp2_reg_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_event0_timestamp2_reg_u *value)
{
	return qca808x_phy_ptp_mmd_write(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_EVENT0_TIMESTAMP2_REG_ADDRESS,
			value->val);
}

sw_error_t
qca808x_ptp_event0_timestamp3_reg_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_event0_timestamp3_reg_u *value)
{
	return qca808x_phy_ptp_mmd_read(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_EVENT0_TIMESTAMP3_REG_ADDRESS,
			&value->val);
}

sw_error_t
qca808x_ptp_event0_timestamp3_reg_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_event0_timestamp3_reg_u *value)
{
	return qca808x_phy_ptp_mmd_write(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_EVENT0_TIMESTAMP3_REG_ADDRESS,
			value->val);
}

sw_error_t
qca808x_ptp_event0_timestamp4_reg_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_event0_timestamp4_reg_u *value)
{
	return qca808x_phy_ptp_mmd_read(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_EVENT0_TIMESTAMP4_REG_ADDRESS,
			&value->val);
}

sw_error_t
qca808x_ptp_event0_timestamp4_reg_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_event0_timestamp4_reg_u *value)
{
	return qca808x_phy_ptp_mmd_write(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_EVENT0_TIMESTAMP4_REG_ADDRESS,
			value->val);
}

sw_error_t
qca808x_ptp_event1_timestamp0_reg_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_event1_timestamp0_reg_u *value)
{
	return qca808x_phy_ptp_mmd_read(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_EVENT1_TIMESTAMP0_REG_ADDRESS,
			&value->val);
}

sw_error_t
qca808x_ptp_event1_timestamp0_reg_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_event1_timestamp0_reg_u *value)
{
	return qca808x_phy_ptp_mmd_write(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_EVENT1_TIMESTAMP0_REG_ADDRESS,
			value->val);
}

sw_error_t
qca808x_ptp_event1_timestamp1_reg_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_event1_timestamp1_reg_u *value)
{
	return qca808x_phy_ptp_mmd_read(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_EVENT1_TIMESTAMP1_REG_ADDRESS,
			&value->val);
}

sw_error_t
qca808x_ptp_event1_timestamp1_reg_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_event1_timestamp1_reg_u *value)
{
	return qca808x_phy_ptp_mmd_write(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_EVENT1_TIMESTAMP1_REG_ADDRESS,
			value->val);
}

sw_error_t
qca808x_ptp_event1_timestamp2_reg_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_event1_timestamp2_reg_u *value)
{
	return qca808x_phy_ptp_mmd_read(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_EVENT1_TIMESTAMP2_REG_ADDRESS,
			&value->val);
}

sw_error_t
qca808x_ptp_event1_timestamp2_reg_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_event1_timestamp2_reg_u *value)
{
	return qca808x_phy_ptp_mmd_write(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_EVENT1_TIMESTAMP2_REG_ADDRESS,
			value->val);
}

sw_error_t
qca808x_ptp_event1_timestamp3_reg_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_event1_timestamp3_reg_u *value)
{
	return qca808x_phy_ptp_mmd_read(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_EVENT1_TIMESTAMP3_REG_ADDRESS,
			&value->val);
}

sw_error_t
qca808x_ptp_event1_timestamp3_reg_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_event1_timestamp3_reg_u *value)
{
	return qca808x_phy_ptp_mmd_write(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_EVENT1_TIMESTAMP3_REG_ADDRESS,
			value->val);
}

sw_error_t
qca808x_ptp_event1_timestamp4_reg_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_event1_timestamp4_reg_u *value)
{
	return qca808x_phy_ptp_mmd_read(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_EVENT1_TIMESTAMP4_REG_ADDRESS,
			&value->val);
}

sw_error_t
qca808x_ptp_event1_timestamp4_reg_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_event1_timestamp4_reg_u *value)
{
	return qca808x_phy_ptp_mmd_write(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_EVENT1_TIMESTAMP4_REG_ADDRESS,
			value->val);
}

sw_error_t
qca808x_ptp_rx_seqid1_reg_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_rx_seqid1_reg_u *value)
{
	return qca808x_phy_ptp_mmd_read(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_RX_SEQID1_REG_ADDRESS,
			&value->val);
}

sw_error_t
qca808x_ptp_rx_seqid1_reg_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_rx_seqid1_reg_u *value)
{
	return qca808x_phy_ptp_mmd_write(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_RX_SEQID1_REG_ADDRESS,
			value->val);
}

sw_error_t
qca808x_ptp_rx_portid1_0_reg_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_rx_portid1_0_reg_u *value)
{
	return qca808x_phy_ptp_mmd_read(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_RX_PORTID1_0_REG_ADDRESS,
			&value->val);
}

sw_error_t
qca808x_ptp_rx_portid1_0_reg_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_rx_portid1_0_reg_u *value)
{
	return qca808x_phy_ptp_mmd_write(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_RX_PORTID1_0_REG_ADDRESS,
			value->val);
}

sw_error_t
qca808x_ptp_rx_portid1_1_reg_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_rx_portid1_1_reg_u *value)
{
	return qca808x_phy_ptp_mmd_read(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_RX_PORTID1_1_REG_ADDRESS,
			&value->val);
}

sw_error_t
qca808x_ptp_rx_portid1_1_reg_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_rx_portid1_1_reg_u *value)
{
	return qca808x_phy_ptp_mmd_write(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_RX_PORTID1_1_REG_ADDRESS,
			value->val);
}

sw_error_t
qca808x_ptp_rx_portid1_2_reg_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_rx_portid1_2_reg_u *value)
{
	return qca808x_phy_ptp_mmd_read(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_RX_PORTID1_2_REG_ADDRESS,
			&value->val);
}

sw_error_t
qca808x_ptp_rx_portid1_2_reg_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_rx_portid1_2_reg_u *value)
{
	return qca808x_phy_ptp_mmd_write(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_RX_PORTID1_2_REG_ADDRESS,
			value->val);
}

sw_error_t
qca808x_ptp_rx_portid1_3_reg_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_rx_portid1_3_reg_u *value)
{
	return qca808x_phy_ptp_mmd_read(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_RX_PORTID1_3_REG_ADDRESS,
			&value->val);
}

sw_error_t
qca808x_ptp_rx_portid1_3_reg_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_rx_portid1_3_reg_u *value)
{
	return qca808x_phy_ptp_mmd_write(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_RX_PORTID1_3_REG_ADDRESS,
			value->val);
}

sw_error_t
qca808x_ptp_rx_portid1_4_reg_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_rx_portid1_4_reg_u *value)
{
	return qca808x_phy_ptp_mmd_read(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_RX_PORTID1_4_REG_ADDRESS,
			&value->val);
}

sw_error_t
qca808x_ptp_rx_portid1_4_reg_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_rx_portid1_4_reg_u *value)
{
	return qca808x_phy_ptp_mmd_write(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_RX_PORTID1_4_REG_ADDRESS,
			value->val);
}

sw_error_t
qca808x_ptp_rx_ts1_0_reg_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_rx_ts1_0_reg_u *value)
{
	return qca808x_phy_ptp_mmd_read(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_RX_TS1_0_REG_ADDRESS,
			&value->val);
}

sw_error_t
qca808x_ptp_rx_ts1_0_reg_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_rx_ts1_0_reg_u *value)
{
	return qca808x_phy_ptp_mmd_write(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_RX_TS1_0_REG_ADDRESS,
			value->val);
}

sw_error_t
qca808x_ptp_rx_ts1_1_reg_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_rx_ts1_1_reg_u *value)
{
	return qca808x_phy_ptp_mmd_read(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_RX_TS1_1_REG_ADDRESS,
			&value->val);
}

sw_error_t
qca808x_ptp_rx_ts1_1_reg_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_rx_ts1_1_reg_u *value)
{
	return qca808x_phy_ptp_mmd_write(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_RX_TS1_1_REG_ADDRESS,
			value->val);
}

sw_error_t
qca808x_ptp_rx_ts1_2_reg_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_rx_ts1_2_reg_u *value)
{
	return qca808x_phy_ptp_mmd_read(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_RX_TS1_2_REG_ADDRESS,
			&value->val);
}

sw_error_t
qca808x_ptp_rx_ts1_2_reg_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_rx_ts1_2_reg_u *value)
{
	return qca808x_phy_ptp_mmd_write(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_RX_TS1_2_REG_ADDRESS,
			value->val);
}

sw_error_t
qca808x_ptp_rx_ts1_3_reg_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_rx_ts1_3_reg_u *value)
{
	return qca808x_phy_ptp_mmd_read(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_RX_TS1_3_REG_ADDRESS,
			&value->val);
}

sw_error_t
qca808x_ptp_rx_ts1_3_reg_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_rx_ts1_3_reg_u *value)
{
	return qca808x_phy_ptp_mmd_write(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_RX_TS1_3_REG_ADDRESS,
			value->val);
}

sw_error_t
qca808x_ptp_rx_ts1_4_reg_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_rx_ts1_4_reg_u *value)
{
	return qca808x_phy_ptp_mmd_read(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_RX_TS1_4_REG_ADDRESS,
			&value->val);
}

sw_error_t
qca808x_ptp_rx_ts1_4_reg_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_rx_ts1_4_reg_u *value)
{
	return qca808x_phy_ptp_mmd_write(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_RX_TS1_4_REG_ADDRESS,
			value->val);
}

sw_error_t
qca808x_ptp_rx_ts1_5_reg_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_rx_ts1_5_reg_u *value)
{
	return qca808x_phy_ptp_mmd_read(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_RX_TS1_5_REG_ADDRESS,
			&value->val);
}

sw_error_t
qca808x_ptp_rx_ts1_5_reg_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_rx_ts1_5_reg_u *value)
{
	return qca808x_phy_ptp_mmd_write(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_RX_TS1_5_REG_ADDRESS,
			value->val);
}

sw_error_t
qca808x_ptp_rx_ts1_6_reg_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_rx_ts1_6_reg_u *value)
{
	return qca808x_phy_ptp_mmd_read(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_RX_TS1_6_REG_ADDRESS,
			&value->val);
}

sw_error_t
qca808x_ptp_rx_ts1_6_reg_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_rx_ts1_6_reg_u *value)
{
	return qca808x_phy_ptp_mmd_write(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_RX_TS1_6_REG_ADDRESS,
			value->val);
}

sw_error_t
qca808x_ptp_rx_seqid2_reg_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_rx_seqid2_reg_u *value)
{
	return qca808x_phy_ptp_mmd_read(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_RX_SEQID2_REG_ADDRESS,
			&value->val);
}

sw_error_t
qca808x_ptp_rx_seqid2_reg_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_rx_seqid2_reg_u *value)
{
	return qca808x_phy_ptp_mmd_write(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_RX_SEQID2_REG_ADDRESS,
			value->val);
}

sw_error_t
qca808x_ptp_rx_portid2_0_reg_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_rx_portid2_0_reg_u *value)
{
	return qca808x_phy_ptp_mmd_read(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_RX_PORTID2_0_REG_ADDRESS,
			&value->val);
}

sw_error_t
qca808x_ptp_rx_portid2_0_reg_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_rx_portid2_0_reg_u *value)
{
	return qca808x_phy_ptp_mmd_write(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_RX_PORTID2_0_REG_ADDRESS,
			value->val);
}

sw_error_t
qca808x_ptp_rx_portid2_1_reg_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_rx_portid2_1_reg_u *value)
{
	return qca808x_phy_ptp_mmd_read(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_RX_PORTID2_1_REG_ADDRESS,
			&value->val);
}

sw_error_t
qca808x_ptp_rx_portid2_1_reg_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_rx_portid2_1_reg_u *value)
{
	return qca808x_phy_ptp_mmd_write(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_RX_PORTID2_1_REG_ADDRESS,
			value->val);
}

sw_error_t
qca808x_ptp_rx_portid2_2_reg_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_rx_portid2_2_reg_u *value)
{
	return qca808x_phy_ptp_mmd_read(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_RX_PORTID2_2_REG_ADDRESS,
			&value->val);
}

sw_error_t
qca808x_ptp_rx_portid2_2_reg_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_rx_portid2_2_reg_u *value)
{
	return qca808x_phy_ptp_mmd_write(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_RX_PORTID2_2_REG_ADDRESS,
			value->val);
}

sw_error_t
qca808x_ptp_rx_portid2_3_reg_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_rx_portid2_3_reg_u *value)
{
	return qca808x_phy_ptp_mmd_read(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_RX_PORTID2_3_REG_ADDRESS,
			&value->val);
}

sw_error_t
qca808x_ptp_rx_portid2_3_reg_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_rx_portid2_3_reg_u *value)
{
	return qca808x_phy_ptp_mmd_write(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_RX_PORTID2_3_REG_ADDRESS,
			value->val);
}

sw_error_t
qca808x_ptp_rx_portid2_4_reg_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_rx_portid2_4_reg_u *value)
{
	return qca808x_phy_ptp_mmd_read(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_RX_PORTID2_4_REG_ADDRESS,
			&value->val);
}

sw_error_t
qca808x_ptp_rx_portid2_4_reg_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_rx_portid2_4_reg_u *value)
{
	return qca808x_phy_ptp_mmd_write(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_RX_PORTID2_4_REG_ADDRESS,
			value->val);
}

sw_error_t
qca808x_ptp_rx_ts2_0_reg_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_rx_ts2_0_reg_u *value)
{
	return qca808x_phy_ptp_mmd_read(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_RX_TS2_0_REG_ADDRESS,
			&value->val);
}

sw_error_t
qca808x_ptp_rx_ts2_0_reg_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_rx_ts2_0_reg_u *value)
{
	return qca808x_phy_ptp_mmd_write(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_RX_TS2_0_REG_ADDRESS,
			value->val);
}

sw_error_t
qca808x_ptp_rx_ts2_1_reg_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_rx_ts2_1_reg_u *value)
{
	return qca808x_phy_ptp_mmd_read(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_RX_TS2_1_REG_ADDRESS,
			&value->val);
}

sw_error_t
qca808x_ptp_rx_ts2_1_reg_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_rx_ts2_1_reg_u *value)
{
	return qca808x_phy_ptp_mmd_write(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_RX_TS2_1_REG_ADDRESS,
			value->val);
}

sw_error_t
qca808x_ptp_rx_ts2_2_reg_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_rx_ts2_2_reg_u *value)
{
	return qca808x_phy_ptp_mmd_read(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_RX_TS2_2_REG_ADDRESS,
			&value->val);
}

sw_error_t
qca808x_ptp_rx_ts2_2_reg_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_rx_ts2_2_reg_u *value)
{
	return qca808x_phy_ptp_mmd_write(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_RX_TS2_2_REG_ADDRESS,
			value->val);
}

sw_error_t
qca808x_ptp_rx_ts2_3_reg_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_rx_ts2_3_reg_u *value)
{
	return qca808x_phy_ptp_mmd_read(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_RX_TS2_3_REG_ADDRESS,
			&value->val);
}

sw_error_t
qca808x_ptp_rx_ts2_3_reg_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_rx_ts2_3_reg_u *value)
{
	return qca808x_phy_ptp_mmd_write(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_RX_TS2_3_REG_ADDRESS,
			value->val);
}

sw_error_t
qca808x_ptp_rx_ts2_4_reg_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_rx_ts2_4_reg_u *value)
{
	return qca808x_phy_ptp_mmd_read(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_RX_TS2_4_REG_ADDRESS,
			&value->val);
}

sw_error_t
qca808x_ptp_rx_ts2_4_reg_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_rx_ts2_4_reg_u *value)
{
	return qca808x_phy_ptp_mmd_write(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_RX_TS2_4_REG_ADDRESS,
			value->val);
}

sw_error_t
qca808x_ptp_rx_ts2_5_reg_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_rx_ts2_5_reg_u *value)
{
	return qca808x_phy_ptp_mmd_read(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_RX_TS2_5_REG_ADDRESS,
			&value->val);
}

sw_error_t
qca808x_ptp_rx_ts2_5_reg_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_rx_ts2_5_reg_u *value)
{
	return qca808x_phy_ptp_mmd_write(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_RX_TS2_5_REG_ADDRESS,
			value->val);
}

sw_error_t
qca808x_ptp_rx_ts2_6_reg_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_rx_ts2_6_reg_u *value)
{
	return qca808x_phy_ptp_mmd_read(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_RX_TS2_6_REG_ADDRESS,
			&value->val);
}

sw_error_t
qca808x_ptp_rx_ts2_6_reg_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_rx_ts2_6_reg_u *value)
{
	return qca808x_phy_ptp_mmd_write(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_RX_TS2_6_REG_ADDRESS,
			value->val);
}

sw_error_t
qca808x_ptp_rx_seqid3_reg_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_rx_seqid3_reg_u *value)
{
	return qca808x_phy_ptp_mmd_read(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_RX_SEQID3_REG_ADDRESS,
			&value->val);
}

sw_error_t
qca808x_ptp_rx_seqid3_reg_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_rx_seqid3_reg_u *value)
{
	return qca808x_phy_ptp_mmd_write(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_RX_SEQID3_REG_ADDRESS,
			value->val);
}

sw_error_t
qca808x_ptp_rx_portid3_0_reg_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_rx_portid3_0_reg_u *value)
{
	return qca808x_phy_ptp_mmd_read(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_RX_PORTID3_0_REG_ADDRESS,
			&value->val);
}

sw_error_t
qca808x_ptp_rx_portid3_0_reg_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_rx_portid3_0_reg_u *value)
{
	return qca808x_phy_ptp_mmd_write(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_RX_PORTID3_0_REG_ADDRESS,
			value->val);
}

sw_error_t
qca808x_ptp_rx_portid3_1_reg_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_rx_portid3_1_reg_u *value)
{
	return qca808x_phy_ptp_mmd_read(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_RX_PORTID3_1_REG_ADDRESS,
			&value->val);
}

sw_error_t
qca808x_ptp_rx_portid3_1_reg_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_rx_portid3_1_reg_u *value)
{
	return qca808x_phy_ptp_mmd_write(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_RX_PORTID3_1_REG_ADDRESS,
			value->val);
}

sw_error_t
qca808x_ptp_rx_portid3_2_reg_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_rx_portid3_2_reg_u *value)
{
	return qca808x_phy_ptp_mmd_read(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_RX_PORTID3_2_REG_ADDRESS,
			&value->val);
}

sw_error_t
qca808x_ptp_rx_portid3_2_reg_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_rx_portid3_2_reg_u *value)
{
	return qca808x_phy_ptp_mmd_write(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_RX_PORTID3_2_REG_ADDRESS,
			value->val);
}

sw_error_t
qca808x_ptp_rx_portid3_3_reg_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_rx_portid3_3_reg_u *value)
{
	return qca808x_phy_ptp_mmd_read(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_RX_PORTID3_3_REG_ADDRESS,
			&value->val);
}

sw_error_t
qca808x_ptp_rx_portid3_3_reg_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_rx_portid3_3_reg_u *value)
{
	return qca808x_phy_ptp_mmd_write(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_RX_PORTID3_3_REG_ADDRESS,
			value->val);
}

sw_error_t
qca808x_ptp_rx_portid3_4_reg_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_rx_portid3_4_reg_u *value)
{
	return qca808x_phy_ptp_mmd_read(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_RX_PORTID3_4_REG_ADDRESS,
			&value->val);
}

sw_error_t
qca808x_ptp_rx_portid3_4_reg_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_rx_portid3_4_reg_u *value)
{
	return qca808x_phy_ptp_mmd_write(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_RX_PORTID3_4_REG_ADDRESS,
			value->val);
}

sw_error_t
qca808x_ptp_rx_ts3_0_reg_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_rx_ts3_0_reg_u *value)
{
	return qca808x_phy_ptp_mmd_read(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_RX_TS3_0_REG_ADDRESS,
			&value->val);
}

sw_error_t
qca808x_ptp_rx_ts3_0_reg_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_rx_ts3_0_reg_u *value)
{
	return qca808x_phy_ptp_mmd_write(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_RX_TS3_0_REG_ADDRESS,
			value->val);
}

sw_error_t
qca808x_ptp_rx_ts3_1_reg_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_rx_ts3_1_reg_u *value)
{
	return qca808x_phy_ptp_mmd_read(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_RX_TS3_1_REG_ADDRESS,
			&value->val);
}

sw_error_t
qca808x_ptp_rx_ts3_1_reg_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_rx_ts3_1_reg_u *value)
{
	return qca808x_phy_ptp_mmd_write(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_RX_TS3_1_REG_ADDRESS,
			value->val);
}

sw_error_t
qca808x_ptp_rx_ts3_2_reg_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_rx_ts3_2_reg_u *value)
{
	return qca808x_phy_ptp_mmd_read(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_RX_TS3_2_REG_ADDRESS,
			&value->val);
}

sw_error_t
qca808x_ptp_rx_ts3_2_reg_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_rx_ts3_2_reg_u *value)
{
	return qca808x_phy_ptp_mmd_write(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_RX_TS3_2_REG_ADDRESS,
			value->val);
}

sw_error_t
qca808x_ptp_rx_ts3_3_reg_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_rx_ts3_3_reg_u *value)
{
	return qca808x_phy_ptp_mmd_read(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_RX_TS3_3_REG_ADDRESS,
			&value->val);
}

sw_error_t
qca808x_ptp_rx_ts3_3_reg_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_rx_ts3_3_reg_u *value)
{
	return qca808x_phy_ptp_mmd_write(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_RX_TS3_3_REG_ADDRESS,
			value->val);
}

sw_error_t
qca808x_ptp_rx_ts3_4_reg_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_rx_ts3_4_reg_u *value)
{
	return qca808x_phy_ptp_mmd_read(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_RX_TS3_4_REG_ADDRESS,
			&value->val);
}

sw_error_t
qca808x_ptp_rx_ts3_4_reg_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_rx_ts3_4_reg_u *value)
{
	return qca808x_phy_ptp_mmd_write(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_RX_TS3_4_REG_ADDRESS,
			value->val);
}

sw_error_t
qca808x_ptp_rx_ts3_5_reg_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_rx_ts3_5_reg_u *value)
{
	return qca808x_phy_ptp_mmd_read(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_RX_TS3_5_REG_ADDRESS,
			&value->val);
}

sw_error_t
qca808x_ptp_rx_ts3_5_reg_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_rx_ts3_5_reg_u *value)
{
	return qca808x_phy_ptp_mmd_write(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_RX_TS3_5_REG_ADDRESS,
			value->val);
}

sw_error_t
qca808x_ptp_rx_ts3_6_reg_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_rx_ts3_6_reg_u *value)
{
	return qca808x_phy_ptp_mmd_read(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_RX_TS3_6_REG_ADDRESS,
			&value->val);
}

sw_error_t
qca808x_ptp_rx_ts3_6_reg_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_rx_ts3_6_reg_u *value)
{
	return qca808x_phy_ptp_mmd_write(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_RX_TS3_6_REG_ADDRESS,
			value->val);
}

sw_error_t
qca808x_ptp_imr_reg_mask_bmp_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_imr_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_imr_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.mask_bmp;
	return ret;
}

sw_error_t
qca808x_ptp_imr_reg_mask_bmp_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_imr_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_imr_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.mask_bmp = value;
	ret = qca808x_ptp_imr_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_isr_reg_status_bmp_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_isr_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_isr_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.status_bmp;
	return ret;
}

sw_error_t
qca808x_ptp_isr_reg_status_bmp_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_isr_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_isr_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.status_bmp = value;
	ret = qca808x_ptp_isr_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_hw_enable_reg_ptp_hw_enable_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_hw_enable_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_hw_enable_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.ptp_hw_enable;
	return ret;
}

sw_error_t
qca808x_ptp_hw_enable_reg_ptp_hw_enable_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_hw_enable_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_hw_enable_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ptp_hw_enable = value;
	ret = qca808x_ptp_hw_enable_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_main_conf_reg_ts_attach_mode_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_main_conf_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_main_conf_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.ts_attach_mode;
	return ret;
}

sw_error_t
qca808x_ptp_main_conf_reg_ts_attach_mode_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_main_conf_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_main_conf_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ts_attach_mode = value;
	ret = qca808x_ptp_main_conf_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_main_conf_reg_ptp_clk_sel_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_main_conf_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_main_conf_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.ptp_clk_sel;
	return ret;
}

sw_error_t
qca808x_ptp_main_conf_reg_ptp_clk_sel_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_main_conf_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_main_conf_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ptp_clk_sel = value;
	ret = qca808x_ptp_main_conf_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_main_conf_reg_disable_1588_phy_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_main_conf_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_main_conf_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.disable_1588_phy;
	return ret;
}

sw_error_t
qca808x_ptp_main_conf_reg_disable_1588_phy_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_main_conf_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_main_conf_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.disable_1588_phy = value;
	ret = qca808x_ptp_main_conf_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_main_conf_reg_attach_crc_recal_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_main_conf_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_main_conf_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.attach_crc_recal;
	return ret;
}

sw_error_t
qca808x_ptp_main_conf_reg_attach_crc_recal_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_main_conf_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_main_conf_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.attach_crc_recal = value;
	ret = qca808x_ptp_main_conf_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_main_conf_reg_ipv4_force_checksum_zero_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_main_conf_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_main_conf_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.ipv4_force_checksum_zero;
	return ret;
}

sw_error_t
qca808x_ptp_main_conf_reg_ipv4_force_checksum_zero_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_main_conf_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_main_conf_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ipv4_force_checksum_zero = value;
	ret = qca808x_ptp_main_conf_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_main_conf_reg_ipv6_embed_force_checksum_zero_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_main_conf_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_main_conf_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.ipv6_embed_force_checksum_zero;
	return ret;
}

sw_error_t
qca808x_ptp_main_conf_reg_ipv6_embed_force_checksum_zero_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_main_conf_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_main_conf_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ipv6_embed_force_checksum_zero = value;
	ret = qca808x_ptp_main_conf_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_main_conf_reg_ptp_bypass_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_main_conf_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_main_conf_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.ptp_bypass;
	return ret;
}

sw_error_t
qca808x_ptp_main_conf_reg_ptp_bypass_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_main_conf_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_main_conf_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ptp_bypass = value;
	ret = qca808x_ptp_main_conf_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_main_conf_reg_wol_en_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_main_conf_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_main_conf_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.wol_en;
	return ret;
}

sw_error_t
qca808x_ptp_main_conf_reg_wol_en_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_main_conf_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_main_conf_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.wol_en = value;
	ret = qca808x_ptp_main_conf_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_main_conf_reg_ptp_clock_mode_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_main_conf_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_main_conf_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.ptp_clock_mode;
	return ret;
}

sw_error_t
qca808x_ptp_main_conf_reg_ptp_clock_mode_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_main_conf_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_main_conf_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ptp_clock_mode = value;
	ret = qca808x_ptp_main_conf_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_rx_seqid0_reg_rx_seqid_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_rx_seqid0_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rx_seqid0_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.rx_seqid;
	return ret;
}

sw_error_t
qca808x_ptp_rx_seqid0_reg_rx_seqid_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_rx_seqid0_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rx_seqid0_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.rx_seqid = value;
	ret = qca808x_ptp_rx_seqid0_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_rx_portid0_0_reg_rx_portid_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_rx_portid0_0_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rx_portid0_0_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.rx_portid;
	return ret;
}

sw_error_t
qca808x_ptp_rx_portid0_0_reg_rx_portid_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_rx_portid0_0_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rx_portid0_0_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.rx_portid = value;
	ret = qca808x_ptp_rx_portid0_0_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_rx_portid0_1_reg_rx_portid_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_rx_portid0_1_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rx_portid0_1_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.rx_portid;
	return ret;
}

sw_error_t
qca808x_ptp_rx_portid0_1_reg_rx_portid_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_rx_portid0_1_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rx_portid0_1_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.rx_portid = value;
	ret = qca808x_ptp_rx_portid0_1_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_rx_portid0_2_reg_rx_portid_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_rx_portid0_2_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rx_portid0_2_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.rx_portid;
	return ret;
}

sw_error_t
qca808x_ptp_rx_portid0_2_reg_rx_portid_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_rx_portid0_2_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rx_portid0_2_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.rx_portid = value;
	ret = qca808x_ptp_rx_portid0_2_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_rx_portid0_3_reg_rx_portid_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_rx_portid0_3_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rx_portid0_3_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.rx_portid;
	return ret;
}

sw_error_t
qca808x_ptp_rx_portid0_3_reg_rx_portid_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_rx_portid0_3_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rx_portid0_3_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.rx_portid = value;
	ret = qca808x_ptp_rx_portid0_3_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_rx_portid0_4_reg_rx_portid_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_rx_portid0_4_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rx_portid0_4_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.rx_portid;
	return ret;
}

sw_error_t
qca808x_ptp_rx_portid0_4_reg_rx_portid_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_rx_portid0_4_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rx_portid0_4_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.rx_portid = value;
	ret = qca808x_ptp_rx_portid0_4_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_rtc_clk_reg_rtc_clk_selection_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_rtc_clk_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rtc_clk_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.rtc_clk_selection;
	return ret;
}

sw_error_t
qca808x_ptp_rtc_clk_reg_rtc_clk_selection_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_rtc_clk_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rtc_clk_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.rtc_clk_selection = value;
	ret = qca808x_ptp_rtc_clk_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_rx_ts0_0_reg_rx_ts_sec_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_rx_ts0_0_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rx_ts0_0_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.rx_ts_sec;
	return ret;
}

sw_error_t
qca808x_ptp_rx_ts0_0_reg_rx_ts_sec_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_rx_ts0_0_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rx_ts0_0_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.rx_ts_sec = value;
	ret = qca808x_ptp_rx_ts0_0_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_rx_ts0_1_reg_rx_ts_sec_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_rx_ts0_1_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rx_ts0_1_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.rx_ts_sec;
	return ret;
}

sw_error_t
qca808x_ptp_rx_ts0_1_reg_rx_ts_sec_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_rx_ts0_1_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rx_ts0_1_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.rx_ts_sec = value;
	ret = qca808x_ptp_rx_ts0_1_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_rx_ts0_2_reg_rx_ts_sec_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_rx_ts0_2_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rx_ts0_2_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.rx_ts_sec;
	return ret;
}

sw_error_t
qca808x_ptp_rx_ts0_2_reg_rx_ts_sec_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_rx_ts0_2_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rx_ts0_2_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.rx_ts_sec = value;
	ret = qca808x_ptp_rx_ts0_2_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_rx_ts0_3_reg_rx_ts_nsec_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_rx_ts0_3_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rx_ts0_3_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.rx_ts_nsec;
	return ret;
}

sw_error_t
qca808x_ptp_rx_ts0_3_reg_rx_ts_nsec_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_rx_ts0_3_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rx_ts0_3_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.rx_ts_nsec = value;
	ret = qca808x_ptp_rx_ts0_3_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_rx_ts0_4_reg_rx_ts_nsec_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_rx_ts0_4_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rx_ts0_4_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.rx_ts_nsec;
	return ret;
}

sw_error_t
qca808x_ptp_rx_ts0_4_reg_rx_ts_nsec_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_rx_ts0_4_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rx_ts0_4_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.rx_ts_nsec = value;
	ret = qca808x_ptp_rx_ts0_4_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_rx_ts0_5_reg_rx_ts_nfsec_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_rx_ts0_5_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rx_ts0_5_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.rx_ts_nfsec;
	return ret;
}

sw_error_t
qca808x_ptp_rx_ts0_5_reg_rx_ts_nfsec_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_rx_ts0_5_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rx_ts0_5_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.rx_ts_nfsec = value;
	ret = qca808x_ptp_rx_ts0_5_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_rx_ts0_5_reg_rx_msg_type_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_rx_ts0_5_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rx_ts0_5_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.rx_msg_type;
	return ret;
}

sw_error_t
qca808x_ptp_rx_ts0_5_reg_rx_msg_type_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_rx_ts0_5_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rx_ts0_5_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.rx_msg_type = value;
	ret = qca808x_ptp_rx_ts0_5_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_rx_ts0_6_reg_rx_ts_nfsec_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_rx_ts0_6_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rx_ts0_6_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.rx_ts_nfsec;
	return ret;
}

sw_error_t
qca808x_ptp_rx_ts0_6_reg_rx_ts_nfsec_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_rx_ts0_6_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rx_ts0_6_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.rx_ts_nfsec = value;
	ret = qca808x_ptp_rx_ts0_6_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_tx_seqid_reg_tx_seqid_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_tx_seqid_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_tx_seqid_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.tx_seqid;
	return ret;
}

sw_error_t
qca808x_ptp_tx_seqid_reg_tx_seqid_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_tx_seqid_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_tx_seqid_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.tx_seqid = value;
	ret = qca808x_ptp_tx_seqid_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_tx_portid0_reg_tx_portid_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_tx_portid0_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_tx_portid0_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.tx_portid;
	return ret;
}

sw_error_t
qca808x_ptp_tx_portid0_reg_tx_portid_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_tx_portid0_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_tx_portid0_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.tx_portid = value;
	ret = qca808x_ptp_tx_portid0_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_tx_portid1_reg_tx_portid_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_tx_portid1_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_tx_portid1_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.tx_portid;
	return ret;
}

sw_error_t
qca808x_ptp_tx_portid1_reg_tx_portid_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_tx_portid1_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_tx_portid1_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.tx_portid = value;
	ret = qca808x_ptp_tx_portid1_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_tx_portid2_reg_tx_portid_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_tx_portid2_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_tx_portid2_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.tx_portid;
	return ret;
}

sw_error_t
qca808x_ptp_tx_portid2_reg_tx_portid_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_tx_portid2_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_tx_portid2_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.tx_portid = value;
	ret = qca808x_ptp_tx_portid2_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_tx_portid3_reg_tx_portid_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_tx_portid3_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_tx_portid3_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.tx_portid;
	return ret;
}

sw_error_t
qca808x_ptp_tx_portid3_reg_tx_portid_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_tx_portid3_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_tx_portid3_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.tx_portid = value;
	ret = qca808x_ptp_tx_portid3_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_tx_portid4_reg_tx_portid_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_tx_portid4_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_tx_portid4_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.tx_portid;
	return ret;
}

sw_error_t
qca808x_ptp_tx_portid4_reg_tx_portid_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_tx_portid4_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_tx_portid4_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.tx_portid = value;
	ret = qca808x_ptp_tx_portid4_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_tx_ts0_reg_tx_ts_sec_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_tx_ts0_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_tx_ts0_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.tx_ts_sec;
	return ret;
}

sw_error_t
qca808x_ptp_tx_ts0_reg_tx_ts_sec_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_tx_ts0_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_tx_ts0_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.tx_ts_sec = value;
	ret = qca808x_ptp_tx_ts0_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_tx_ts1_reg_tx_ts_sec_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_tx_ts1_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_tx_ts1_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.tx_ts_sec;
	return ret;
}

sw_error_t
qca808x_ptp_tx_ts1_reg_tx_ts_sec_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_tx_ts1_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_tx_ts1_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.tx_ts_sec = value;
	ret = qca808x_ptp_tx_ts1_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_tx_ts2_reg_tx_ts_sec_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_tx_ts2_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_tx_ts2_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.tx_ts_sec;
	return ret;
}

sw_error_t
qca808x_ptp_tx_ts2_reg_tx_ts_sec_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_tx_ts2_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_tx_ts2_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.tx_ts_sec = value;
	ret = qca808x_ptp_tx_ts2_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_tx_ts3_reg_tx_ts_nsec_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_tx_ts3_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_tx_ts3_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.tx_ts_nsec;
	return ret;
}

sw_error_t
qca808x_ptp_tx_ts3_reg_tx_ts_nsec_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_tx_ts3_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_tx_ts3_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.tx_ts_nsec = value;
	ret = qca808x_ptp_tx_ts3_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_tx_ts4_reg_tx_ts_nsec_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_tx_ts4_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_tx_ts4_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.tx_ts_nsec;
	return ret;
}

sw_error_t
qca808x_ptp_tx_ts4_reg_tx_ts_nsec_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_tx_ts4_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_tx_ts4_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.tx_ts_nsec = value;
	ret = qca808x_ptp_tx_ts4_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_tx_ts5_reg_tx_ts_nfsec_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_tx_ts5_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_tx_ts5_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.tx_ts_nfsec;
	return ret;
}

sw_error_t
qca808x_ptp_tx_ts5_reg_tx_ts_nfsec_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_tx_ts5_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_tx_ts5_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.tx_ts_nfsec = value;
	ret = qca808x_ptp_tx_ts5_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_tx_ts5_reg_tx_msg_type_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_tx_ts5_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_tx_ts5_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.tx_msg_type;
	return ret;
}

sw_error_t
qca808x_ptp_tx_ts5_reg_tx_msg_type_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_tx_ts5_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_tx_ts5_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.tx_msg_type = value;
	ret = qca808x_ptp_tx_ts5_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_tx_ts6_reg_tx_ts_nfsec_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_tx_ts6_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_tx_ts6_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.tx_ts_nfsec;
	return ret;
}

sw_error_t
qca808x_ptp_tx_ts6_reg_tx_ts_nfsec_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_tx_ts6_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_tx_ts6_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.tx_ts_nfsec = value;
	ret = qca808x_ptp_tx_ts6_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_orig_corr0_reg_ptp_orig_corr_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_orig_corr0_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_orig_corr0_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.ptp_orig_corr;
	return ret;
}

sw_error_t
qca808x_ptp_orig_corr0_reg_ptp_orig_corr_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_orig_corr0_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_orig_corr0_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ptp_orig_corr = value;
	ret = qca808x_ptp_orig_corr0_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_orig_corr1_reg_ptp_orig_corr_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_orig_corr1_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_orig_corr1_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.ptp_orig_corr;
	return ret;
}

sw_error_t
qca808x_ptp_orig_corr1_reg_ptp_orig_corr_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_orig_corr1_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_orig_corr1_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ptp_orig_corr = value;
	ret = qca808x_ptp_orig_corr1_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_orig_corr2_reg_ptp_orig_corr_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_orig_corr2_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_orig_corr2_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.ptp_orig_corr;
	return ret;
}

sw_error_t
qca808x_ptp_orig_corr2_reg_ptp_orig_corr_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_orig_corr2_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_orig_corr2_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ptp_orig_corr = value;
	ret = qca808x_ptp_orig_corr2_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_orig_corr3_reg_ptp_orig_corr_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_orig_corr3_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_orig_corr3_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.ptp_orig_corr;
	return ret;
}

sw_error_t
qca808x_ptp_orig_corr3_reg_ptp_orig_corr_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_orig_corr3_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_orig_corr3_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ptp_orig_corr = value;
	ret = qca808x_ptp_orig_corr3_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_in_trig0_reg_ptp_in_trig_nisec_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_in_trig0_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_in_trig0_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.ptp_in_trig_nisec;
	return ret;
}

sw_error_t
qca808x_ptp_in_trig0_reg_ptp_in_trig_nisec_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_in_trig0_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_in_trig0_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ptp_in_trig_nisec = value;
	ret = qca808x_ptp_in_trig0_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_in_trig1_reg_ptp_in_trig_nisec_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_in_trig1_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_in_trig1_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.ptp_in_trig_nisec;
	return ret;
}

sw_error_t
qca808x_ptp_in_trig1_reg_ptp_in_trig_nisec_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_in_trig1_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_in_trig1_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ptp_in_trig_nisec = value;
	ret = qca808x_ptp_in_trig1_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_in_trig2_reg_ptp_in_trig_nisec_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_in_trig2_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_in_trig2_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.ptp_in_trig_nisec;
	return ret;
}

sw_error_t
qca808x_ptp_in_trig2_reg_ptp_in_trig_nisec_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_in_trig2_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_in_trig2_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ptp_in_trig_nisec = value;
	ret = qca808x_ptp_in_trig2_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_in_trig3_reg_ptp_in_trig_nisec_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_in_trig3_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_in_trig3_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.ptp_in_trig_nisec;
	return ret;
}

sw_error_t
qca808x_ptp_in_trig3_reg_ptp_in_trig_nisec_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_in_trig3_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_in_trig3_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ptp_in_trig_nisec = value;
	ret = qca808x_ptp_in_trig3_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_tx_latency_reg_ptp_tx_latency_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_tx_latency_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_tx_latency_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.ptp_tx_latency;
	return ret;
}

sw_error_t
qca808x_ptp_tx_latency_reg_ptp_tx_latency_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_tx_latency_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_tx_latency_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ptp_tx_latency = value;
	ret = qca808x_ptp_tx_latency_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_rtc_inc0_reg_ptp_rtc_inc_nis_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_rtc_inc0_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rtc_inc0_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.ptp_rtc_inc_nis;
	return ret;
}

sw_error_t
qca808x_ptp_rtc_inc0_reg_ptp_rtc_inc_nis_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_rtc_inc0_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rtc_inc0_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ptp_rtc_inc_nis = value;
	ret = qca808x_ptp_rtc_inc0_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_rtc_inc0_reg_ptp_rtc_inc_nfs_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_rtc_inc0_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rtc_inc0_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.ptp_rtc_inc_nfs;
	return ret;
}

sw_error_t
qca808x_ptp_rtc_inc0_reg_ptp_rtc_inc_nfs_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_rtc_inc0_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rtc_inc0_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ptp_rtc_inc_nfs = value;
	ret = qca808x_ptp_rtc_inc0_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_rtc_inc1_reg_ptp_rtc_inc_nfs_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_rtc_inc1_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rtc_inc1_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.ptp_rtc_inc_nfs;
	return ret;
}

sw_error_t
qca808x_ptp_rtc_inc1_reg_ptp_rtc_inc_nfs_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_rtc_inc1_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rtc_inc1_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ptp_rtc_inc_nfs = value;
	ret = qca808x_ptp_rtc_inc1_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_rtcoffs0_reg_ptp_rtcoffs_nsec_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_rtcoffs0_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rtcoffs0_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.ptp_rtcoffs_nsec;
	return ret;
}

sw_error_t
qca808x_ptp_rtcoffs0_reg_ptp_rtcoffs_nsec_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_rtcoffs0_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rtcoffs0_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ptp_rtcoffs_nsec = value;
	ret = qca808x_ptp_rtcoffs0_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_rtcoffs1_reg_ptp_rtcoffs_nsec_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_rtcoffs1_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rtcoffs1_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.ptp_rtcoffs_nsec;
	return ret;
}

sw_error_t
qca808x_ptp_rtcoffs1_reg_ptp_rtcoffs_nsec_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_rtcoffs1_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rtcoffs1_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ptp_rtcoffs_nsec = value;
	ret = qca808x_ptp_rtcoffs1_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_rtcoffs2_reg_ptp_rtcoffs_sec_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_rtcoffs2_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rtcoffs2_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.ptp_rtcoffs_sec;
	return ret;
}

sw_error_t
qca808x_ptp_rtcoffs2_reg_ptp_rtcoffs_sec_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_rtcoffs2_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rtcoffs2_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ptp_rtcoffs_sec = value;
	ret = qca808x_ptp_rtcoffs2_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_rtcoffs3_reg_ptp_rtcoffs_sec_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_rtcoffs3_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rtcoffs3_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.ptp_rtcoffs_sec;
	return ret;
}

sw_error_t
qca808x_ptp_rtcoffs3_reg_ptp_rtcoffs_sec_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_rtcoffs3_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rtcoffs3_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ptp_rtcoffs_sec = value;
	ret = qca808x_ptp_rtcoffs3_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_rtcoffs4_reg_ptp_rtcoffs_sec_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_rtcoffs4_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rtcoffs4_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.ptp_rtcoffs_sec;
	return ret;
}

sw_error_t
qca808x_ptp_rtcoffs4_reg_ptp_rtcoffs_sec_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_rtcoffs4_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rtcoffs4_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ptp_rtcoffs_sec = value;
	ret = qca808x_ptp_rtcoffs4_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_rtc0_reg_ptp_rtc_sec_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_rtc0_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rtc0_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.ptp_rtc_sec;
	return ret;
}

sw_error_t
qca808x_ptp_rtc0_reg_ptp_rtc_sec_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_rtc0_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rtc0_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ptp_rtc_sec = value;
	ret = qca808x_ptp_rtc0_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_rtc1_reg_ptp_rtc_sec_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_rtc1_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rtc1_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.ptp_rtc_sec;
	return ret;
}

sw_error_t
qca808x_ptp_rtc1_reg_ptp_rtc_sec_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_rtc1_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rtc1_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ptp_rtc_sec = value;
	ret = qca808x_ptp_rtc1_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_rtc2_reg_ptp_rtc_sec_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_rtc2_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rtc2_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.ptp_rtc_sec;
	return ret;
}

sw_error_t
qca808x_ptp_rtc2_reg_ptp_rtc_sec_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_rtc2_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rtc2_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ptp_rtc_sec = value;
	ret = qca808x_ptp_rtc2_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_rtc3_reg_ptp_rtc_nisec_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_rtc3_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rtc3_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.ptp_rtc_nisec;
	return ret;
}

sw_error_t
qca808x_ptp_rtc3_reg_ptp_rtc_nisec_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_rtc3_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rtc3_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ptp_rtc_nisec = value;
	ret = qca808x_ptp_rtc3_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_rtc4_reg_ptp_rtc_nisec_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_rtc4_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rtc4_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.ptp_rtc_nisec;
	return ret;
}

sw_error_t
qca808x_ptp_rtc4_reg_ptp_rtc_nisec_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_rtc4_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rtc4_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ptp_rtc_nisec = value;
	ret = qca808x_ptp_rtc4_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_rtc5_reg_ptp_rtc_nfsec_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_rtc5_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rtc5_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.ptp_rtc_nfsec;
	return ret;
}

sw_error_t
qca808x_ptp_rtc5_reg_ptp_rtc_nfsec_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_rtc5_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rtc5_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ptp_rtc_nfsec = value;
	ret = qca808x_ptp_rtc5_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_rtc6_reg_ptp_rtc_nfsec_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_rtc6_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rtc6_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.ptp_rtc_nfsec;
	return ret;
}

sw_error_t
qca808x_ptp_rtc6_reg_ptp_rtc_nfsec_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_rtc6_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rtc6_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ptp_rtc_nfsec = value;
	ret = qca808x_ptp_rtc6_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_rtcoffs_valid_reg_ptp_rtcoffs_valid_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_rtcoffs_valid_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rtcoffs_valid_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.ptp_rtcoffs_valid;
	return ret;
}

sw_error_t
qca808x_ptp_rtcoffs_valid_reg_ptp_rtcoffs_valid_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_rtcoffs_valid_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rtcoffs_valid_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ptp_rtcoffs_valid = value;
	ret = qca808x_ptp_rtcoffs_valid_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_misc_config_reg_ptp_ver_chk_en_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_misc_config_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_misc_config_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.ptp_ver_chk_en;
	return ret;
}

sw_error_t
qca808x_ptp_misc_config_reg_ptp_ver_chk_en_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_misc_config_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_misc_config_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ptp_ver_chk_en = value;
	ret = qca808x_ptp_misc_config_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_misc_config_reg_ipv6_udp_chk_en_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_misc_config_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_misc_config_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.ipv6_udp_chk_en;
	return ret;
}

sw_error_t
qca808x_ptp_misc_config_reg_ipv6_udp_chk_en_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_misc_config_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_misc_config_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ipv6_udp_chk_en = value;
	ret = qca808x_ptp_misc_config_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_misc_config_reg_cf_from_pkt_en_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_misc_config_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_misc_config_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.cf_from_pkt_en;
	return ret;
}

sw_error_t
qca808x_ptp_misc_config_reg_cf_from_pkt_en_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_misc_config_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_misc_config_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.cf_from_pkt_en = value;
	ret = qca808x_ptp_misc_config_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_misc_config_reg_embed_ingress_time_en_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_misc_config_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_misc_config_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.embed_ingress_time_en;
	return ret;
}

sw_error_t
qca808x_ptp_misc_config_reg_embed_ingress_time_en_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_misc_config_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_misc_config_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.embed_ingress_time_en = value;
	ret = qca808x_ptp_misc_config_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_misc_config_reg_ptp_addr_chk_en_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_misc_config_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_misc_config_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.ptp_addr_chk_en;
	return ret;
}

sw_error_t
qca808x_ptp_misc_config_reg_ptp_addr_chk_en_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_misc_config_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_misc_config_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ptp_addr_chk_en = value;
	ret = qca808x_ptp_misc_config_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_misc_config_reg_crc_validate_en_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_misc_config_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_misc_config_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.crc_validate_en;
	return ret;
}

sw_error_t
qca808x_ptp_misc_config_reg_crc_validate_en_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_misc_config_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_misc_config_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.crc_validate_en = value;
	ret = qca808x_ptp_misc_config_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_misc_config_reg_pkt_one_step_en_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_misc_config_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_misc_config_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.pkt_one_step_en;
	return ret;
}

sw_error_t
qca808x_ptp_misc_config_reg_pkt_one_step_en_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_misc_config_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_misc_config_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.pkt_one_step_en = value;
	ret = qca808x_ptp_misc_config_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_misc_config_reg_ptp_version_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_misc_config_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_misc_config_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.ptp_version;
	return ret;
}

sw_error_t
qca808x_ptp_misc_config_reg_ptp_version_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_misc_config_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_misc_config_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ptp_version = value;
	ret = qca808x_ptp_misc_config_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_misc_config_reg_appended_timestamp_size_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_misc_config_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_misc_config_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.appended_timestamp_size;
	return ret;
}

sw_error_t
qca808x_ptp_misc_config_reg_appended_timestamp_size_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_misc_config_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_misc_config_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.appended_timestamp_size = value;
	ret = qca808x_ptp_misc_config_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_misc_config_reg_ts_rtc_select_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_misc_config_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_misc_config_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.ts_rtc_select;
	return ret;
}

sw_error_t
qca808x_ptp_misc_config_reg_ts_rtc_select_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_misc_config_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_misc_config_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ts_rtc_select = value;
	ret = qca808x_ptp_misc_config_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_ext_imr_reg_mask_bmp_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_ext_imr_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_ext_imr_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.mask_bmp;
	return ret;
}

sw_error_t
qca808x_ptp_ext_imr_reg_mask_bmp_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_ext_imr_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_ext_imr_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.mask_bmp = value;
	ret = qca808x_ptp_ext_imr_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_ext_isr_reg_status_bmp_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_ext_isr_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_ext_isr_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.status_bmp;
	return ret;
}

sw_error_t
qca808x_ptp_ext_isr_reg_status_bmp_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_ext_isr_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_ext_isr_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.status_bmp = value;
	ret = qca808x_ptp_ext_isr_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_rtc_ext_conf_reg_rtc_snapshot_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_rtc_ext_conf_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rtc_ext_conf_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.rtc_snapshot;
	return ret;
}

sw_error_t
qca808x_ptp_rtc_ext_conf_reg_rtc_snapshot_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_rtc_ext_conf_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rtc_ext_conf_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.rtc_snapshot = value;
	ret = qca808x_ptp_rtc_ext_conf_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_rtc_ext_conf_reg_set_incval_mode_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_rtc_ext_conf_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rtc_ext_conf_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.set_incval_mode;
	return ret;
}

sw_error_t
qca808x_ptp_rtc_ext_conf_reg_set_incval_mode_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_rtc_ext_conf_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rtc_ext_conf_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.set_incval_mode = value;
	ret = qca808x_ptp_rtc_ext_conf_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_rtc_ext_conf_reg_load_rtc_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_rtc_ext_conf_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rtc_ext_conf_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.load_rtc;
	return ret;
}

sw_error_t
qca808x_ptp_rtc_ext_conf_reg_load_rtc_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_rtc_ext_conf_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rtc_ext_conf_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.load_rtc = value;
	ret = qca808x_ptp_rtc_ext_conf_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_rtc_ext_conf_reg_clear_rtc_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_rtc_ext_conf_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rtc_ext_conf_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.clear_rtc;
	return ret;
}

sw_error_t
qca808x_ptp_rtc_ext_conf_reg_clear_rtc_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_rtc_ext_conf_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rtc_ext_conf_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.clear_rtc = value;
	ret = qca808x_ptp_rtc_ext_conf_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_rtc_ext_conf_reg_rtc_read_mode_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_rtc_ext_conf_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rtc_ext_conf_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.rtc_read_mode;
	return ret;
}

sw_error_t
qca808x_ptp_rtc_ext_conf_reg_rtc_read_mode_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_rtc_ext_conf_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rtc_ext_conf_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.rtc_read_mode = value;
	ret = qca808x_ptp_rtc_ext_conf_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_rtc_ext_conf_reg_select_output_waveform_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_rtc_ext_conf_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rtc_ext_conf_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.select_output_waveform;
	return ret;
}

sw_error_t
qca808x_ptp_rtc_ext_conf_reg_select_output_waveform_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_rtc_ext_conf_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rtc_ext_conf_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.select_output_waveform = value;
	ret = qca808x_ptp_rtc_ext_conf_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_rtc_ext_conf_reg_set_incval_valid_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_rtc_ext_conf_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rtc_ext_conf_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.set_incval_valid;
	return ret;
}

sw_error_t
qca808x_ptp_rtc_ext_conf_reg_set_incval_valid_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_rtc_ext_conf_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rtc_ext_conf_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.set_incval_valid = value;
	ret = qca808x_ptp_rtc_ext_conf_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_rtc_preloaded0_reg_ptp_rtc_preloaded_sec_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_rtc_preloaded0_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rtc_preloaded0_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.ptp_rtc_preloaded_sec;
	return ret;
}

sw_error_t
qca808x_ptp_rtc_preloaded0_reg_ptp_rtc_preloaded_sec_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_rtc_preloaded0_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rtc_preloaded0_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ptp_rtc_preloaded_sec = value;
	ret = qca808x_ptp_rtc_preloaded0_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_rtc_preloaded1_reg_ptp_rtc_preloaded_sec_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_rtc_preloaded1_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rtc_preloaded1_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.ptp_rtc_preloaded_sec;
	return ret;
}

sw_error_t
qca808x_ptp_rtc_preloaded1_reg_ptp_rtc_preloaded_sec_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_rtc_preloaded1_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rtc_preloaded1_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ptp_rtc_preloaded_sec = value;
	ret = qca808x_ptp_rtc_preloaded1_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_rtc_preloaded2_reg_ptp_rtc_preloaded_sec_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_rtc_preloaded2_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rtc_preloaded2_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.ptp_rtc_preloaded_sec;
	return ret;
}

sw_error_t
qca808x_ptp_rtc_preloaded2_reg_ptp_rtc_preloaded_sec_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_rtc_preloaded2_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rtc_preloaded2_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ptp_rtc_preloaded_sec = value;
	ret = qca808x_ptp_rtc_preloaded2_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_rtc_preloaded3_reg_ptp_rtc_preloaded_nisec_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_rtc_preloaded3_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rtc_preloaded3_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.ptp_rtc_preloaded_nisec;
	return ret;
}

sw_error_t
qca808x_ptp_rtc_preloaded3_reg_ptp_rtc_preloaded_nisec_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_rtc_preloaded3_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rtc_preloaded3_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ptp_rtc_preloaded_nisec = value;
	ret = qca808x_ptp_rtc_preloaded3_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_rtc_preloaded4_reg_ptp_rtc_preloaded_nisec_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_rtc_preloaded4_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rtc_preloaded4_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.ptp_rtc_preloaded_nisec;
	return ret;
}

sw_error_t
qca808x_ptp_rtc_preloaded4_reg_ptp_rtc_preloaded_nisec_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_rtc_preloaded4_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rtc_preloaded4_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ptp_rtc_preloaded_nisec = value;
	ret = qca808x_ptp_rtc_preloaded4_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_gm_conf0_reg_gm_pps_sync_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_gm_conf0_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_gm_conf0_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.gm_pps_sync;
	return ret;
}

sw_error_t
qca808x_ptp_gm_conf0_reg_gm_pps_sync_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_gm_conf0_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_gm_conf0_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.gm_pps_sync = value;
	ret = qca808x_ptp_gm_conf0_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_gm_conf0_reg_gm_pll_mode_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_gm_conf0_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_gm_conf0_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.gm_pll_mode;
	return ret;
}

sw_error_t
qca808x_ptp_gm_conf0_reg_gm_pll_mode_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_gm_conf0_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_gm_conf0_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.gm_pll_mode = value;
	ret = qca808x_ptp_gm_conf0_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_gm_conf0_reg_gm_maxfreq_offset_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_gm_conf0_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_gm_conf0_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.gm_maxfreq_offset;
	return ret;
}

sw_error_t
qca808x_ptp_gm_conf0_reg_gm_maxfreq_offset_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_gm_conf0_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_gm_conf0_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.gm_maxfreq_offset = value;
	ret = qca808x_ptp_gm_conf0_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_gm_conf0_reg_grandmaster_mode_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_gm_conf0_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_gm_conf0_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.grandmaster_mode;
	return ret;
}

sw_error_t
qca808x_ptp_gm_conf0_reg_grandmaster_mode_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_gm_conf0_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_gm_conf0_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.grandmaster_mode = value;
	ret = qca808x_ptp_gm_conf0_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_gm_conf1_reg_gm_kp_ldn_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_gm_conf1_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_gm_conf1_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.gm_kp_ldn;
	return ret;
}

sw_error_t
qca808x_ptp_gm_conf1_reg_gm_kp_ldn_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_gm_conf1_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_gm_conf1_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.gm_kp_ldn = value;
	ret = qca808x_ptp_gm_conf1_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_gm_conf1_reg_gm_ki_ldn_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_gm_conf1_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_gm_conf1_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.gm_ki_ldn;
	return ret;
}

sw_error_t
qca808x_ptp_gm_conf1_reg_gm_ki_ldn_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_gm_conf1_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_gm_conf1_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.gm_ki_ldn = value;
	ret = qca808x_ptp_gm_conf1_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_ppsin_ts0_reg_rx_ts_sec_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_ppsin_ts0_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_ppsin_ts0_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.rx_ts_sec;
	return ret;
}

sw_error_t
qca808x_ptp_ppsin_ts0_reg_rx_ts_sec_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_ppsin_ts0_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_ppsin_ts0_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.rx_ts_sec = value;
	ret = qca808x_ptp_ppsin_ts0_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_ppsin_ts1_reg_rx_ts_sec_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_ppsin_ts1_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_ppsin_ts1_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.rx_ts_sec;
	return ret;
}

sw_error_t
qca808x_ptp_ppsin_ts1_reg_rx_ts_sec_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_ppsin_ts1_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_ppsin_ts1_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.rx_ts_sec = value;
	ret = qca808x_ptp_ppsin_ts1_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_ppsin_ts2_reg_rx_ts_sec_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_ppsin_ts2_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_ppsin_ts2_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.rx_ts_sec;
	return ret;
}

sw_error_t
qca808x_ptp_ppsin_ts2_reg_rx_ts_sec_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_ppsin_ts2_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_ppsin_ts2_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.rx_ts_sec = value;
	ret = qca808x_ptp_ppsin_ts2_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_ppsin_ts3_reg_rx_ts_nsec_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_ppsin_ts3_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_ppsin_ts3_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.rx_ts_nsec;
	return ret;
}

sw_error_t
qca808x_ptp_ppsin_ts3_reg_rx_ts_nsec_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_ppsin_ts3_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_ppsin_ts3_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.rx_ts_nsec = value;
	ret = qca808x_ptp_ppsin_ts3_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_ppsin_ts4_reg_rx_ts_nsec_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_ppsin_ts4_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_ppsin_ts4_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.rx_ts_nsec;
	return ret;
}

sw_error_t
qca808x_ptp_ppsin_ts4_reg_rx_ts_nsec_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_ppsin_ts4_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_ppsin_ts4_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.rx_ts_nsec = value;
	ret = qca808x_ptp_ppsin_ts4_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_hwpll_inc0_reg_ptp_rtc_inc_nfs1_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_hwpll_inc0_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_hwpll_inc0_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.ptp_rtc_inc_nfs1;
	return ret;
}

sw_error_t
qca808x_ptp_hwpll_inc0_reg_ptp_rtc_inc_nfs1_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_hwpll_inc0_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_hwpll_inc0_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ptp_rtc_inc_nfs1 = value;
	ret = qca808x_ptp_hwpll_inc0_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_hwpll_inc0_reg_ptp_rtc_inc_nis_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_hwpll_inc0_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_hwpll_inc0_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.ptp_rtc_inc_nis;
	return ret;
}

sw_error_t
qca808x_ptp_hwpll_inc0_reg_ptp_rtc_inc_nis_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_hwpll_inc0_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_hwpll_inc0_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ptp_rtc_inc_nis = value;
	ret = qca808x_ptp_hwpll_inc0_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_hwpll_inc1_reg_ptp_rtc_inc_nfs0_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_hwpll_inc1_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_hwpll_inc1_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.ptp_rtc_inc_nfs0;
	return ret;
}

sw_error_t
qca808x_ptp_hwpll_inc1_reg_ptp_rtc_inc_nfs0_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_hwpll_inc1_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_hwpll_inc1_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ptp_rtc_inc_nfs0 = value;
	ret = qca808x_ptp_hwpll_inc1_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_ppsin_latency_reg_ptp_ppsin_latency_sign_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_ppsin_latency_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_ppsin_latency_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.ptp_ppsin_latency_sign;
	return ret;
}

sw_error_t
qca808x_ptp_ppsin_latency_reg_ptp_ppsin_latency_sign_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_ppsin_latency_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_ppsin_latency_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ptp_ppsin_latency_sign = value;
	ret = qca808x_ptp_ppsin_latency_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_ppsin_latency_reg_ptp_ppsin_latency_value_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_ppsin_latency_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_ppsin_latency_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.ptp_ppsin_latency_value;
	return ret;
}

sw_error_t
qca808x_ptp_ppsin_latency_reg_ptp_ppsin_latency_value_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_ppsin_latency_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_ppsin_latency_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ptp_ppsin_latency_value = value;
	ret = qca808x_ptp_ppsin_latency_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_trigger0_config_reg_force_value_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_trigger0_config_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_trigger0_config_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.force_value;
	return ret;
}

sw_error_t
qca808x_ptp_trigger0_config_reg_force_value_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_trigger0_config_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_trigger0_config_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.force_value = value;
	ret = qca808x_ptp_trigger0_config_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_trigger0_config_reg_pattern_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_trigger0_config_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_trigger0_config_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.pattern;
	return ret;
}

sw_error_t
qca808x_ptp_trigger0_config_reg_pattern_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_trigger0_config_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_trigger0_config_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.pattern = value;
	ret = qca808x_ptp_trigger0_config_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_trigger0_config_reg_status_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_trigger0_config_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_trigger0_config_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.status;
	return ret;
}

sw_error_t
qca808x_ptp_trigger0_config_reg_status_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_trigger0_config_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_trigger0_config_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.status = value;
	ret = qca808x_ptp_trigger0_config_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_trigger0_config_reg_force_en_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_trigger0_config_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_trigger0_config_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.force_en;
	return ret;
}

sw_error_t
qca808x_ptp_trigger0_config_reg_force_en_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_trigger0_config_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_trigger0_config_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.force_en = value;
	ret = qca808x_ptp_trigger0_config_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_trigger0_config_reg_setting_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_trigger0_config_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_trigger0_config_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.setting;
	return ret;
}

sw_error_t
qca808x_ptp_trigger0_config_reg_setting_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_trigger0_config_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_trigger0_config_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.setting = value;
	ret = qca808x_ptp_trigger0_config_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_trigger0_config_reg_notify_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_trigger0_config_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_trigger0_config_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.notify;
	return ret;
}

sw_error_t
qca808x_ptp_trigger0_config_reg_notify_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_trigger0_config_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_trigger0_config_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.notify = value;
	ret = qca808x_ptp_trigger0_config_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_trigger0_config_reg_if_late_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_trigger0_config_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_trigger0_config_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.if_late;
	return ret;
}

sw_error_t
qca808x_ptp_trigger0_config_reg_if_late_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_trigger0_config_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_trigger0_config_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.if_late = value;
	ret = qca808x_ptp_trigger0_config_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_trigger0_status_reg_error_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_trigger0_status_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_trigger0_status_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.error;
	return ret;
}

sw_error_t
qca808x_ptp_trigger0_status_reg_error_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_trigger0_status_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_trigger0_status_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.error = value;
	ret = qca808x_ptp_trigger0_status_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_trigger0_status_reg_active_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_trigger0_status_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_trigger0_status_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.active;
	return ret;
}

sw_error_t
qca808x_ptp_trigger0_status_reg_active_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_trigger0_status_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_trigger0_status_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.active = value;
	ret = qca808x_ptp_trigger0_status_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_trigger0_status_reg_finished_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_trigger0_status_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_trigger0_status_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.finished;
	return ret;
}

sw_error_t
qca808x_ptp_trigger0_status_reg_finished_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_trigger0_status_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_trigger0_status_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.finished = value;
	ret = qca808x_ptp_trigger0_status_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_trigger1_config_reg_force_value_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_trigger1_config_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_trigger1_config_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.force_value;
	return ret;
}

sw_error_t
qca808x_ptp_trigger1_config_reg_force_value_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_trigger1_config_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_trigger1_config_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.force_value = value;
	ret = qca808x_ptp_trigger1_config_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_trigger1_config_reg_pattern_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_trigger1_config_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_trigger1_config_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.pattern;
	return ret;
}

sw_error_t
qca808x_ptp_trigger1_config_reg_pattern_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_trigger1_config_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_trigger1_config_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.pattern = value;
	ret = qca808x_ptp_trigger1_config_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_trigger1_config_reg_status_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_trigger1_config_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_trigger1_config_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.status;
	return ret;
}

sw_error_t
qca808x_ptp_trigger1_config_reg_status_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_trigger1_config_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_trigger1_config_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.status = value;
	ret = qca808x_ptp_trigger1_config_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_trigger1_config_reg_force_en_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_trigger1_config_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_trigger1_config_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.force_en;
	return ret;
}

sw_error_t
qca808x_ptp_trigger1_config_reg_force_en_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_trigger1_config_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_trigger1_config_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.force_en = value;
	ret = qca808x_ptp_trigger1_config_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_trigger1_config_reg_setting_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_trigger1_config_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_trigger1_config_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.setting;
	return ret;
}

sw_error_t
qca808x_ptp_trigger1_config_reg_setting_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_trigger1_config_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_trigger1_config_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.setting = value;
	ret = qca808x_ptp_trigger1_config_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_trigger1_config_reg_notify_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_trigger1_config_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_trigger1_config_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.notify;
	return ret;
}

sw_error_t
qca808x_ptp_trigger1_config_reg_notify_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_trigger1_config_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_trigger1_config_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.notify = value;
	ret = qca808x_ptp_trigger1_config_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_trigger1_config_reg_if_late_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_trigger1_config_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_trigger1_config_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.if_late;
	return ret;
}

sw_error_t
qca808x_ptp_trigger1_config_reg_if_late_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_trigger1_config_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_trigger1_config_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.if_late = value;
	ret = qca808x_ptp_trigger1_config_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_trigger1_status_reg_error_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_trigger1_status_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_trigger1_status_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.error;
	return ret;
}

sw_error_t
qca808x_ptp_trigger1_status_reg_error_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_trigger1_status_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_trigger1_status_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.error = value;
	ret = qca808x_ptp_trigger1_status_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_trigger1_status_reg_active_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_trigger1_status_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_trigger1_status_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.active;
	return ret;
}

sw_error_t
qca808x_ptp_trigger1_status_reg_active_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_trigger1_status_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_trigger1_status_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.active = value;
	ret = qca808x_ptp_trigger1_status_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_trigger1_status_reg_finished_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_trigger1_status_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_trigger1_status_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.finished;
	return ret;
}

sw_error_t
qca808x_ptp_trigger1_status_reg_finished_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_trigger1_status_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_trigger1_status_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.finished = value;
	ret = qca808x_ptp_trigger1_status_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_trigger0_timestamp0_reg_ts_sec_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_trigger0_timestamp0_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_trigger0_timestamp0_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.ts_sec;
	return ret;
}

sw_error_t
qca808x_ptp_trigger0_timestamp0_reg_ts_sec_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_trigger0_timestamp0_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_trigger0_timestamp0_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ts_sec = value;
	ret = qca808x_ptp_trigger0_timestamp0_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_trigger0_timestamp1_reg_ts_sec_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_trigger0_timestamp1_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_trigger0_timestamp1_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.ts_sec;
	return ret;
}

sw_error_t
qca808x_ptp_trigger0_timestamp1_reg_ts_sec_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_trigger0_timestamp1_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_trigger0_timestamp1_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ts_sec = value;
	ret = qca808x_ptp_trigger0_timestamp1_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_trigger0_timestamp2_reg_ts_sec_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_trigger0_timestamp2_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_trigger0_timestamp2_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.ts_sec;
	return ret;
}

sw_error_t
qca808x_ptp_trigger0_timestamp2_reg_ts_sec_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_trigger0_timestamp2_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_trigger0_timestamp2_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ts_sec = value;
	ret = qca808x_ptp_trigger0_timestamp2_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_trigger0_timestamp3_reg_ts_nsec_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_trigger0_timestamp3_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_trigger0_timestamp3_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.ts_nsec;
	return ret;
}

sw_error_t
qca808x_ptp_trigger0_timestamp3_reg_ts_nsec_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_trigger0_timestamp3_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_trigger0_timestamp3_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ts_nsec = value;
	ret = qca808x_ptp_trigger0_timestamp3_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_trigger0_timestamp4_reg_ts_nsec_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_trigger0_timestamp4_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_trigger0_timestamp4_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.ts_nsec;
	return ret;
}

sw_error_t
qca808x_ptp_trigger0_timestamp4_reg_ts_nsec_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_trigger0_timestamp4_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_trigger0_timestamp4_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ts_nsec = value;
	ret = qca808x_ptp_trigger0_timestamp4_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_trigger1_timestamp0_reg_ts_sec_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_trigger1_timestamp0_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_trigger1_timestamp0_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.ts_sec;
	return ret;
}

sw_error_t
qca808x_ptp_trigger1_timestamp0_reg_ts_sec_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_trigger1_timestamp0_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_trigger1_timestamp0_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ts_sec = value;
	ret = qca808x_ptp_trigger1_timestamp0_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_trigger1_timestamp1_reg_ts_sec_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_trigger1_timestamp1_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_trigger1_timestamp1_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.ts_sec;
	return ret;
}

sw_error_t
qca808x_ptp_trigger1_timestamp1_reg_ts_sec_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_trigger1_timestamp1_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_trigger1_timestamp1_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ts_sec = value;
	ret = qca808x_ptp_trigger1_timestamp1_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_trigger1_timestamp2_reg_ts_sec_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_trigger1_timestamp2_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_trigger1_timestamp2_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.ts_sec;
	return ret;
}

sw_error_t
qca808x_ptp_trigger1_timestamp2_reg_ts_sec_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_trigger1_timestamp2_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_trigger1_timestamp2_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ts_sec = value;
	ret = qca808x_ptp_trigger1_timestamp2_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_trigger1_timestamp3_reg_ts_nsec_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_trigger1_timestamp3_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_trigger1_timestamp3_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.ts_nsec;
	return ret;
}

sw_error_t
qca808x_ptp_trigger1_timestamp3_reg_ts_nsec_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_trigger1_timestamp3_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_trigger1_timestamp3_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ts_nsec = value;
	ret = qca808x_ptp_trigger1_timestamp3_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_trigger1_timestamp4_reg_ts_nsec_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_trigger1_timestamp4_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_trigger1_timestamp4_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.ts_nsec;
	return ret;
}

sw_error_t
qca808x_ptp_trigger1_timestamp4_reg_ts_nsec_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_trigger1_timestamp4_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_trigger1_timestamp4_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ts_nsec = value;
	ret = qca808x_ptp_trigger1_timestamp4_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_event0_config_reg_rise_en_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_event0_config_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_event0_config_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.rise_en;
	return ret;
}

sw_error_t
qca808x_ptp_event0_config_reg_rise_en_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_event0_config_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_event0_config_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.rise_en = value;
	ret = qca808x_ptp_event0_config_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_event0_config_reg_single_cap_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_event0_config_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_event0_config_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.single_cap;
	return ret;
}

sw_error_t
qca808x_ptp_event0_config_reg_single_cap_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_event0_config_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_event0_config_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.single_cap = value;
	ret = qca808x_ptp_event0_config_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_event0_config_reg_fall_en_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_event0_config_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_event0_config_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.fall_en;
	return ret;
}

sw_error_t
qca808x_ptp_event0_config_reg_fall_en_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_event0_config_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_event0_config_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.fall_en = value;
	ret = qca808x_ptp_event0_config_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_event0_config_reg_notify_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_event0_config_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_event0_config_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.notify;
	return ret;
}

sw_error_t
qca808x_ptp_event0_config_reg_notify_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_event0_config_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_event0_config_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.notify = value;
	ret = qca808x_ptp_event0_config_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_event0_config_reg_clear_stat_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_event0_config_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_event0_config_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.clear_stat;
	return ret;
}

sw_error_t
qca808x_ptp_event0_config_reg_clear_stat_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_event0_config_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_event0_config_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.clear_stat = value;
	ret = qca808x_ptp_event0_config_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_event0_status_reg_mul_event_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_event0_status_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_event0_status_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.mul_event;
	return ret;
}

sw_error_t
qca808x_ptp_event0_status_reg_mul_event_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_event0_status_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_event0_status_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.mul_event = value;
	ret = qca808x_ptp_event0_status_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_event0_status_reg_missed_count_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_event0_status_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_event0_status_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.missed_count;
	return ret;
}

sw_error_t
qca808x_ptp_event0_status_reg_missed_count_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_event0_status_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_event0_status_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.missed_count = value;
	ret = qca808x_ptp_event0_status_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_event0_status_reg_dir_detected_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_event0_status_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_event0_status_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.dir_detected;
	return ret;
}

sw_error_t
qca808x_ptp_event0_status_reg_dir_detected_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_event0_status_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_event0_status_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.dir_detected = value;
	ret = qca808x_ptp_event0_status_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_event0_status_reg_detected_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_event0_status_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_event0_status_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.detected;
	return ret;
}

sw_error_t
qca808x_ptp_event0_status_reg_detected_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_event0_status_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_event0_status_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.detected = value;
	ret = qca808x_ptp_event0_status_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_event1_config_reg_rise_en_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_event1_config_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_event1_config_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.rise_en;
	return ret;
}

sw_error_t
qca808x_ptp_event1_config_reg_rise_en_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_event1_config_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_event1_config_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.rise_en = value;
	ret = qca808x_ptp_event1_config_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_event1_config_reg_single_cap_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_event1_config_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_event1_config_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.single_cap;
	return ret;
}

sw_error_t
qca808x_ptp_event1_config_reg_single_cap_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_event1_config_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_event1_config_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.single_cap = value;
	ret = qca808x_ptp_event1_config_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_event1_config_reg_fall_en_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_event1_config_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_event1_config_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.fall_en;
	return ret;
}

sw_error_t
qca808x_ptp_event1_config_reg_fall_en_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_event1_config_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_event1_config_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.fall_en = value;
	ret = qca808x_ptp_event1_config_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_event1_config_reg_notify_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_event1_config_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_event1_config_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.notify;
	return ret;
}

sw_error_t
qca808x_ptp_event1_config_reg_notify_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_event1_config_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_event1_config_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.notify = value;
	ret = qca808x_ptp_event1_config_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_event1_config_reg_clear_stat_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_event1_config_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_event1_config_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.clear_stat;
	return ret;
}

sw_error_t
qca808x_ptp_event1_config_reg_clear_stat_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_event1_config_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_event1_config_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.clear_stat = value;
	ret = qca808x_ptp_event1_config_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_event1_status_reg_mul_event_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_event1_status_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_event1_status_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.mul_event;
	return ret;
}

sw_error_t
qca808x_ptp_event1_status_reg_mul_event_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_event1_status_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_event1_status_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.mul_event = value;
	ret = qca808x_ptp_event1_status_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_event1_status_reg_missed_count_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_event1_status_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_event1_status_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.missed_count;
	return ret;
}

sw_error_t
qca808x_ptp_event1_status_reg_missed_count_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_event1_status_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_event1_status_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.missed_count = value;
	ret = qca808x_ptp_event1_status_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_event1_status_reg_dir_detected_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_event1_status_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_event1_status_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.dir_detected;
	return ret;
}

sw_error_t
qca808x_ptp_event1_status_reg_dir_detected_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_event1_status_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_event1_status_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.dir_detected = value;
	ret = qca808x_ptp_event1_status_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_event1_status_reg_detected_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_event1_status_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_event1_status_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.detected;
	return ret;
}

sw_error_t
qca808x_ptp_event1_status_reg_detected_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_event1_status_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_event1_status_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.detected = value;
	ret = qca808x_ptp_event1_status_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_event0_timestamp0_reg_ts_nsec_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_event0_timestamp0_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_event0_timestamp0_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.ts_nsec;
	return ret;
}

sw_error_t
qca808x_ptp_event0_timestamp0_reg_ts_nsec_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_event0_timestamp0_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_event0_timestamp0_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ts_nsec = value;
	ret = qca808x_ptp_event0_timestamp0_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_event0_timestamp1_reg_ts_nsec_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_event0_timestamp1_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_event0_timestamp1_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.ts_nsec;
	return ret;
}

sw_error_t
qca808x_ptp_event0_timestamp1_reg_ts_nsec_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_event0_timestamp1_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_event0_timestamp1_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ts_nsec = value;
	ret = qca808x_ptp_event0_timestamp1_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_event0_timestamp2_reg_ts_nsec_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_event0_timestamp2_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_event0_timestamp2_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.ts_nsec;
	return ret;
}

sw_error_t
qca808x_ptp_event0_timestamp2_reg_ts_nsec_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_event0_timestamp2_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_event0_timestamp2_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ts_nsec = value;
	ret = qca808x_ptp_event0_timestamp2_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_event0_timestamp3_reg_ts_nsec_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_event0_timestamp3_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_event0_timestamp3_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.ts_nsec;
	return ret;
}

sw_error_t
qca808x_ptp_event0_timestamp3_reg_ts_nsec_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_event0_timestamp3_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_event0_timestamp3_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ts_nsec = value;
	ret = qca808x_ptp_event0_timestamp3_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_event0_timestamp4_reg_ts_nsec_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_event0_timestamp4_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_event0_timestamp4_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.ts_nsec;
	return ret;
}

sw_error_t
qca808x_ptp_event0_timestamp4_reg_ts_nsec_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_event0_timestamp4_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_event0_timestamp4_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ts_nsec = value;
	ret = qca808x_ptp_event0_timestamp4_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_event1_timestamp0_reg_ts_nsec_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_event1_timestamp0_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_event1_timestamp0_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.ts_nsec;
	return ret;
}

sw_error_t
qca808x_ptp_event1_timestamp0_reg_ts_nsec_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_event1_timestamp0_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_event1_timestamp0_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ts_nsec = value;
	ret = qca808x_ptp_event1_timestamp0_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_event1_timestamp1_reg_ts_nsec_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_event1_timestamp1_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_event1_timestamp1_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.ts_nsec;
	return ret;
}

sw_error_t
qca808x_ptp_event1_timestamp1_reg_ts_nsec_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_event1_timestamp1_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_event1_timestamp1_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ts_nsec = value;
	ret = qca808x_ptp_event1_timestamp1_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_event1_timestamp2_reg_ts_nsec_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_event1_timestamp2_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_event1_timestamp2_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.ts_nsec;
	return ret;
}

sw_error_t
qca808x_ptp_event1_timestamp2_reg_ts_nsec_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_event1_timestamp2_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_event1_timestamp2_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ts_nsec = value;
	ret = qca808x_ptp_event1_timestamp2_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_event1_timestamp3_reg_ts_nsec_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_event1_timestamp3_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_event1_timestamp3_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.ts_nsec;
	return ret;
}

sw_error_t
qca808x_ptp_event1_timestamp3_reg_ts_nsec_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_event1_timestamp3_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_event1_timestamp3_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ts_nsec = value;
	ret = qca808x_ptp_event1_timestamp3_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_event1_timestamp4_reg_ts_nsec_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_event1_timestamp4_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_event1_timestamp4_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.ts_nsec;
	return ret;
}

sw_error_t
qca808x_ptp_event1_timestamp4_reg_ts_nsec_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_event1_timestamp4_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_event1_timestamp4_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ts_nsec = value;
	ret = qca808x_ptp_event1_timestamp4_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_rx_seqid1_reg_rx_seqid_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_rx_seqid1_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rx_seqid1_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.rx_seqid;
	return ret;
}

sw_error_t
qca808x_ptp_rx_seqid1_reg_rx_seqid_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_rx_seqid1_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rx_seqid1_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.rx_seqid = value;
	ret = qca808x_ptp_rx_seqid1_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_rx_portid1_0_reg_rx_portid_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_rx_portid1_0_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rx_portid1_0_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.rx_portid;
	return ret;
}

sw_error_t
qca808x_ptp_rx_portid1_0_reg_rx_portid_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_rx_portid1_0_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rx_portid1_0_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.rx_portid = value;
	ret = qca808x_ptp_rx_portid1_0_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_rx_portid1_1_reg_rx_portid_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_rx_portid1_1_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rx_portid1_1_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.rx_portid;
	return ret;
}

sw_error_t
qca808x_ptp_rx_portid1_1_reg_rx_portid_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_rx_portid1_1_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rx_portid1_1_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.rx_portid = value;
	ret = qca808x_ptp_rx_portid1_1_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_rx_portid1_2_reg_rx_portid_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_rx_portid1_2_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rx_portid1_2_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.rx_portid;
	return ret;
}

sw_error_t
qca808x_ptp_rx_portid1_2_reg_rx_portid_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_rx_portid1_2_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rx_portid1_2_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.rx_portid = value;
	ret = qca808x_ptp_rx_portid1_2_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_rx_portid1_3_reg_rx_portid_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_rx_portid1_3_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rx_portid1_3_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.rx_portid;
	return ret;
}

sw_error_t
qca808x_ptp_rx_portid1_3_reg_rx_portid_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_rx_portid1_3_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rx_portid1_3_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.rx_portid = value;
	ret = qca808x_ptp_rx_portid1_3_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_rx_portid1_4_reg_rx_portid_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_rx_portid1_4_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rx_portid1_4_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.rx_portid;
	return ret;
}

sw_error_t
qca808x_ptp_rx_portid1_4_reg_rx_portid_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_rx_portid1_4_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rx_portid1_4_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.rx_portid = value;
	ret = qca808x_ptp_rx_portid1_4_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_rx_ts1_0_reg_rx_ts_sec_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_rx_ts1_0_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rx_ts1_0_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.rx_ts_sec;
	return ret;
}

sw_error_t
qca808x_ptp_rx_ts1_0_reg_rx_ts_sec_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_rx_ts1_0_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rx_ts1_0_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.rx_ts_sec = value;
	ret = qca808x_ptp_rx_ts1_0_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_rx_ts1_1_reg_rx_ts_sec_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_rx_ts1_1_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rx_ts1_1_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.rx_ts_sec;
	return ret;
}

sw_error_t
qca808x_ptp_rx_ts1_1_reg_rx_ts_sec_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_rx_ts1_1_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rx_ts1_1_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.rx_ts_sec = value;
	ret = qca808x_ptp_rx_ts1_1_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_rx_ts1_2_reg_rx_ts_sec_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_rx_ts1_2_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rx_ts1_2_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.rx_ts_sec;
	return ret;
}

sw_error_t
qca808x_ptp_rx_ts1_2_reg_rx_ts_sec_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_rx_ts1_2_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rx_ts1_2_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.rx_ts_sec = value;
	ret = qca808x_ptp_rx_ts1_2_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_rx_ts1_3_reg_rx_ts_nsec_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_rx_ts1_3_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rx_ts1_3_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.rx_ts_nsec;
	return ret;
}

sw_error_t
qca808x_ptp_rx_ts1_3_reg_rx_ts_nsec_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_rx_ts1_3_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rx_ts1_3_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.rx_ts_nsec = value;
	ret = qca808x_ptp_rx_ts1_3_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_rx_ts1_4_reg_rx_ts_nsec_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_rx_ts1_4_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rx_ts1_4_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.rx_ts_nsec;
	return ret;
}

sw_error_t
qca808x_ptp_rx_ts1_4_reg_rx_ts_nsec_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_rx_ts1_4_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rx_ts1_4_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.rx_ts_nsec = value;
	ret = qca808x_ptp_rx_ts1_4_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_rx_ts1_5_reg_rx_ts_nfsec_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_rx_ts1_5_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rx_ts1_5_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.rx_ts_nfsec;
	return ret;
}

sw_error_t
qca808x_ptp_rx_ts1_5_reg_rx_ts_nfsec_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_rx_ts1_5_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rx_ts1_5_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.rx_ts_nfsec = value;
	ret = qca808x_ptp_rx_ts1_5_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_rx_ts1_5_reg_rx_msg_type_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_rx_ts1_5_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rx_ts1_5_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.rx_msg_type;
	return ret;
}

sw_error_t
qca808x_ptp_rx_ts1_5_reg_rx_msg_type_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_rx_ts1_5_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rx_ts1_5_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.rx_msg_type = value;
	ret = qca808x_ptp_rx_ts1_5_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_rx_ts1_6_reg_rx_ts_nfsec_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_rx_ts1_6_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rx_ts1_6_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.rx_ts_nfsec;
	return ret;
}

sw_error_t
qca808x_ptp_rx_ts1_6_reg_rx_ts_nfsec_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_rx_ts1_6_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rx_ts1_6_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.rx_ts_nfsec = value;
	ret = qca808x_ptp_rx_ts1_6_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_rx_seqid2_reg_rx_seqid_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_rx_seqid2_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rx_seqid2_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.rx_seqid;
	return ret;
}

sw_error_t
qca808x_ptp_rx_seqid2_reg_rx_seqid_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_rx_seqid2_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rx_seqid2_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.rx_seqid = value;
	ret = qca808x_ptp_rx_seqid2_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_rx_portid2_0_reg_rx_portid_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_rx_portid2_0_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rx_portid2_0_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.rx_portid;
	return ret;
}

sw_error_t
qca808x_ptp_rx_portid2_0_reg_rx_portid_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_rx_portid2_0_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rx_portid2_0_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.rx_portid = value;
	ret = qca808x_ptp_rx_portid2_0_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_rx_portid2_1_reg_rx_portid_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_rx_portid2_1_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rx_portid2_1_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.rx_portid;
	return ret;
}

sw_error_t
qca808x_ptp_rx_portid2_1_reg_rx_portid_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_rx_portid2_1_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rx_portid2_1_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.rx_portid = value;
	ret = qca808x_ptp_rx_portid2_1_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_rx_portid2_2_reg_rx_portid_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_rx_portid2_2_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rx_portid2_2_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.rx_portid;
	return ret;
}

sw_error_t
qca808x_ptp_rx_portid2_2_reg_rx_portid_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_rx_portid2_2_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rx_portid2_2_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.rx_portid = value;
	ret = qca808x_ptp_rx_portid2_2_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_rx_portid2_3_reg_rx_portid_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_rx_portid2_3_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rx_portid2_3_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.rx_portid;
	return ret;
}

sw_error_t
qca808x_ptp_rx_portid2_3_reg_rx_portid_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_rx_portid2_3_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rx_portid2_3_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.rx_portid = value;
	ret = qca808x_ptp_rx_portid2_3_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_rx_portid2_4_reg_rx_portid_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_rx_portid2_4_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rx_portid2_4_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.rx_portid;
	return ret;
}

sw_error_t
qca808x_ptp_rx_portid2_4_reg_rx_portid_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_rx_portid2_4_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rx_portid2_4_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.rx_portid = value;
	ret = qca808x_ptp_rx_portid2_4_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_rx_ts2_0_reg_rx_ts_sec_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_rx_ts2_0_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rx_ts2_0_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.rx_ts_sec;
	return ret;
}

sw_error_t
qca808x_ptp_rx_ts2_0_reg_rx_ts_sec_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_rx_ts2_0_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rx_ts2_0_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.rx_ts_sec = value;
	ret = qca808x_ptp_rx_ts2_0_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_rx_ts2_1_reg_rx_ts_sec_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_rx_ts2_1_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rx_ts2_1_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.rx_ts_sec;
	return ret;
}

sw_error_t
qca808x_ptp_rx_ts2_1_reg_rx_ts_sec_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_rx_ts2_1_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rx_ts2_1_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.rx_ts_sec = value;
	ret = qca808x_ptp_rx_ts2_1_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_rx_ts2_2_reg_rx_ts_sec_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_rx_ts2_2_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rx_ts2_2_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.rx_ts_sec;
	return ret;
}

sw_error_t
qca808x_ptp_rx_ts2_2_reg_rx_ts_sec_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_rx_ts2_2_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rx_ts2_2_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.rx_ts_sec = value;
	ret = qca808x_ptp_rx_ts2_2_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_rx_ts2_3_reg_rx_ts_nsec_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_rx_ts2_3_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rx_ts2_3_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.rx_ts_nsec;
	return ret;
}

sw_error_t
qca808x_ptp_rx_ts2_3_reg_rx_ts_nsec_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_rx_ts2_3_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rx_ts2_3_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.rx_ts_nsec = value;
	ret = qca808x_ptp_rx_ts2_3_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_rx_ts2_4_reg_rx_ts_nsec_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_rx_ts2_4_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rx_ts2_4_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.rx_ts_nsec;
	return ret;
}

sw_error_t
qca808x_ptp_rx_ts2_4_reg_rx_ts_nsec_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_rx_ts2_4_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rx_ts2_4_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.rx_ts_nsec = value;
	ret = qca808x_ptp_rx_ts2_4_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_rx_ts2_5_reg_rx_ts_nfsec_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_rx_ts2_5_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rx_ts2_5_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.rx_ts_nfsec;
	return ret;
}

sw_error_t
qca808x_ptp_rx_ts2_5_reg_rx_ts_nfsec_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_rx_ts2_5_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rx_ts2_5_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.rx_ts_nfsec = value;
	ret = qca808x_ptp_rx_ts2_5_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_rx_ts2_5_reg_rx_msg_type_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_rx_ts2_5_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rx_ts2_5_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.rx_msg_type;
	return ret;
}

sw_error_t
qca808x_ptp_rx_ts2_5_reg_rx_msg_type_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_rx_ts2_5_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rx_ts2_5_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.rx_msg_type = value;
	ret = qca808x_ptp_rx_ts2_5_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_rx_ts2_6_reg_rx_ts_nfsec_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_rx_ts2_6_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rx_ts2_6_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.rx_ts_nfsec;
	return ret;
}

sw_error_t
qca808x_ptp_rx_ts2_6_reg_rx_ts_nfsec_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_rx_ts2_6_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rx_ts2_6_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.rx_ts_nfsec = value;
	ret = qca808x_ptp_rx_ts2_6_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_rx_seqid3_reg_rx_seqid_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_rx_seqid3_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rx_seqid3_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.rx_seqid;
	return ret;
}

sw_error_t
qca808x_ptp_rx_seqid3_reg_rx_seqid_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_rx_seqid3_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rx_seqid3_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.rx_seqid = value;
	ret = qca808x_ptp_rx_seqid3_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_rx_portid3_0_reg_rx_portid_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_rx_portid3_0_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rx_portid3_0_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.rx_portid;
	return ret;
}

sw_error_t
qca808x_ptp_rx_portid3_0_reg_rx_portid_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_rx_portid3_0_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rx_portid3_0_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.rx_portid = value;
	ret = qca808x_ptp_rx_portid3_0_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_rx_portid3_1_reg_rx_portid_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_rx_portid3_1_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rx_portid3_1_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.rx_portid;
	return ret;
}

sw_error_t
qca808x_ptp_rx_portid3_1_reg_rx_portid_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_rx_portid3_1_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rx_portid3_1_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.rx_portid = value;
	ret = qca808x_ptp_rx_portid3_1_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_rx_portid3_2_reg_rx_portid_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_rx_portid3_2_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rx_portid3_2_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.rx_portid;
	return ret;
}

sw_error_t
qca808x_ptp_rx_portid3_2_reg_rx_portid_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_rx_portid3_2_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rx_portid3_2_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.rx_portid = value;
	ret = qca808x_ptp_rx_portid3_2_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_rx_portid3_3_reg_rx_portid_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_rx_portid3_3_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rx_portid3_3_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.rx_portid;
	return ret;
}

sw_error_t
qca808x_ptp_rx_portid3_3_reg_rx_portid_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_rx_portid3_3_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rx_portid3_3_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.rx_portid = value;
	ret = qca808x_ptp_rx_portid3_3_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_rx_portid3_4_reg_rx_portid_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_rx_portid3_4_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rx_portid3_4_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.rx_portid;
	return ret;
}

sw_error_t
qca808x_ptp_rx_portid3_4_reg_rx_portid_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_rx_portid3_4_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rx_portid3_4_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.rx_portid = value;
	ret = qca808x_ptp_rx_portid3_4_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_rx_ts3_0_reg_rx_ts_sec_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_rx_ts3_0_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rx_ts3_0_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.rx_ts_sec;
	return ret;
}

sw_error_t
qca808x_ptp_rx_ts3_0_reg_rx_ts_sec_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_rx_ts3_0_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rx_ts3_0_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.rx_ts_sec = value;
	ret = qca808x_ptp_rx_ts3_0_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_rx_ts3_1_reg_rx_ts_sec_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_rx_ts3_1_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rx_ts3_1_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.rx_ts_sec;
	return ret;
}

sw_error_t
qca808x_ptp_rx_ts3_1_reg_rx_ts_sec_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_rx_ts3_1_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rx_ts3_1_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.rx_ts_sec = value;
	ret = qca808x_ptp_rx_ts3_1_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_rx_ts3_2_reg_rx_ts_sec_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_rx_ts3_2_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rx_ts3_2_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.rx_ts_sec;
	return ret;
}

sw_error_t
qca808x_ptp_rx_ts3_2_reg_rx_ts_sec_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_rx_ts3_2_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rx_ts3_2_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.rx_ts_sec = value;
	ret = qca808x_ptp_rx_ts3_2_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_rx_ts3_3_reg_rx_ts_nsec_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_rx_ts3_3_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rx_ts3_3_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.rx_ts_nsec;
	return ret;
}

sw_error_t
qca808x_ptp_rx_ts3_3_reg_rx_ts_nsec_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_rx_ts3_3_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rx_ts3_3_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.rx_ts_nsec = value;
	ret = qca808x_ptp_rx_ts3_3_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_rx_ts3_4_reg_rx_ts_nsec_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_rx_ts3_4_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rx_ts3_4_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.rx_ts_nsec;
	return ret;
}

sw_error_t
qca808x_ptp_rx_ts3_4_reg_rx_ts_nsec_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_rx_ts3_4_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rx_ts3_4_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.rx_ts_nsec = value;
	ret = qca808x_ptp_rx_ts3_4_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_rx_ts3_5_reg_rx_ts_nfsec_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_rx_ts3_5_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rx_ts3_5_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.rx_ts_nfsec;
	return ret;
}

sw_error_t
qca808x_ptp_rx_ts3_5_reg_rx_ts_nfsec_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_rx_ts3_5_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rx_ts3_5_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.rx_ts_nfsec = value;
	ret = qca808x_ptp_rx_ts3_5_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_rx_ts3_5_reg_rx_msg_type_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_rx_ts3_5_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rx_ts3_5_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.rx_msg_type;
	return ret;
}

sw_error_t
qca808x_ptp_rx_ts3_5_reg_rx_msg_type_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_rx_ts3_5_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rx_ts3_5_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.rx_msg_type = value;
	ret = qca808x_ptp_rx_ts3_5_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_rx_ts3_6_reg_rx_ts_nfsec_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_rx_ts3_6_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rx_ts3_6_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.rx_ts_nfsec;
	return ret;
}

sw_error_t
qca808x_ptp_rx_ts3_6_reg_rx_ts_nfsec_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_rx_ts3_6_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rx_ts3_6_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.rx_ts_nfsec = value;
	ret = qca808x_ptp_rx_ts3_6_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_phase_adjust_0_reg_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_phase_adjust_0_reg_u *value)
{
	return qca808x_phy_ptp_mmd_read(
				dev_id,
				phy_id, QCA808X_PHY_MMD3_NUM, PTP_PHASE_ADJUST_0_REG_ADDRESS,
				&value->val);
}

sw_error_t
qca808x_ptp_phase_adjust_0_reg_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_phase_adjust_0_reg_u *value)
{
	return qca808x_phy_ptp_mmd_write(
				dev_id,
				phy_id, QCA808X_PHY_MMD3_NUM, PTP_PHASE_ADJUST_0_REG_ADDRESS,
				value->val);
}

sw_error_t
qca808x_ptp_phase_adjust_0_reg_phase_value_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_phase_adjust_0_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_phase_adjust_0_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.phase_value;
	return ret;
}

sw_error_t
qca808x_ptp_phase_adjust_0_reg_phase_value_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_phase_adjust_0_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_phase_adjust_0_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.phase_value = value;
	ret = qca808x_ptp_phase_adjust_0_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_phase_adjust_1_reg_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_phase_adjust_1_reg_u *value)
{
	return qca808x_phy_ptp_mmd_read(
				dev_id,
				phy_id, QCA808X_PHY_MMD3_NUM, PTP_PHASE_ADJUST_1_REG_ADDRESS,
				&value->val);
}

sw_error_t
qca808x_ptp_phase_adjust_1_reg_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_phase_adjust_1_reg_u *value)
{
	return qca808x_phy_ptp_mmd_write(
				dev_id,
				phy_id, QCA808X_PHY_MMD3_NUM, PTP_PHASE_ADJUST_1_REG_ADDRESS,
				value->val);
}

sw_error_t
qca808x_ptp_phase_adjust_1_reg_phase_value_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_phase_adjust_1_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_phase_adjust_1_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.phase_value;
	return ret;
}

sw_error_t
qca808x_ptp_phase_adjust_1_reg_phase_value_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_phase_adjust_1_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_phase_adjust_1_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.phase_value = value;
	ret = qca808x_ptp_phase_adjust_1_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_pps_pul_width_0_reg_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_pps_pul_width_0_reg_u *value)
{
	return qca808x_phy_ptp_mmd_read(
				dev_id,
				phy_id, QCA808X_PHY_MMD3_NUM, PTP_PPS_PUL_WIDTH_0_REG_ADDRESS,
				&value->val);
}

sw_error_t
qca808x_ptp_pps_pul_width_0_reg_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_pps_pul_width_0_reg_u *value)
{
	return qca808x_phy_ptp_mmd_write(
				dev_id,
				phy_id, QCA808X_PHY_MMD3_NUM, PTP_PPS_PUL_WIDTH_0_REG_ADDRESS,
				value->val);
}

sw_error_t
qca808x_ptp_pps_pul_width_0_reg_pul_value_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_pps_pul_width_0_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_pps_pul_width_0_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.pul_value;
	return ret;
}

sw_error_t
qca808x_ptp_pps_pul_width_0_reg_pul_value_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_pps_pul_width_0_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_pps_pul_width_0_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.pul_value = value;
	ret = qca808x_ptp_pps_pul_width_0_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_pps_pul_width_1_reg_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_pps_pul_width_1_reg_u *value)
{
	return qca808x_phy_ptp_mmd_read(
				dev_id,
				phy_id, QCA808X_PHY_MMD3_NUM, PTP_PPS_PUL_WIDTH_1_REG_ADDRESS,
				&value->val);
}

sw_error_t
qca808x_ptp_pps_pul_width_1_reg_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_pps_pul_width_1_reg_u *value)
{
	return qca808x_phy_ptp_mmd_write(
				dev_id,
				phy_id, QCA808X_PHY_MMD3_NUM, PTP_PPS_PUL_WIDTH_1_REG_ADDRESS,
				value->val);
}

sw_error_t
qca808x_ptp_pps_pul_width_1_reg_pul_value_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_pps_pul_width_1_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_pps_pul_width_1_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.pul_value;
	return ret;
}

sw_error_t
qca808x_ptp_pps_pul_width_1_reg_pul_value_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_pps_pul_width_1_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_pps_pul_width_1_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.pul_value = value;
	ret = qca808x_ptp_pps_pul_width_1_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}


sw_error_t
qca808x_ptp_freq_waveform_period_0_reg_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_freq_waveform_period_0_reg_u *value)
{
	return qca808x_phy_ptp_mmd_read(
				dev_id,
				phy_id, QCA808X_PHY_MMD3_NUM, PTP_FREQ_WAVEFORM_PERIOD_0_REG_ADDRESS,
				&value->val);
}

sw_error_t
qca808x_ptp_freq_waveform_period_0_reg_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_freq_waveform_period_0_reg_u *value)
{
	return qca808x_phy_ptp_mmd_write(
				dev_id,
				phy_id, QCA808X_PHY_MMD3_NUM, PTP_FREQ_WAVEFORM_PERIOD_0_REG_ADDRESS,
				value->val);
}

sw_error_t
qca808x_ptp_freq_waveform_period_0_reg_wave_period_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_freq_waveform_period_0_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_freq_waveform_period_0_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.wave_period;
	return ret;
}

sw_error_t
qca808x_ptp_freq_waveform_period_0_reg_wave_period_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_freq_waveform_period_0_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_freq_waveform_period_0_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.wave_period = value;
	ret = qca808x_ptp_freq_waveform_period_0_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_freq_waveform_period_0_reg_phase_ali_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_freq_waveform_period_0_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_freq_waveform_period_0_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.phase_ali;
	return ret;
}

sw_error_t
qca808x_ptp_freq_waveform_period_0_reg_phase_ali_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_freq_waveform_period_0_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_freq_waveform_period_0_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.phase_ali = value;
	ret = qca808x_ptp_freq_waveform_period_0_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_freq_waveform_period_1_reg_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_freq_waveform_period_1_reg_u *value)
{
	return qca808x_phy_ptp_mmd_read(
				dev_id,
				phy_id, QCA808X_PHY_MMD3_NUM, PTP_FREQ_WAVEFORM_PERIOD_1_REG_ADDRESS,
				&value->val);
}

sw_error_t
qca808x_ptp_freq_waveform_period_1_reg_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_freq_waveform_period_1_reg_u *value)
{
	return qca808x_phy_ptp_mmd_write(
				dev_id,
				phy_id, QCA808X_PHY_MMD3_NUM, PTP_FREQ_WAVEFORM_PERIOD_1_REG_ADDRESS,
				value->val);
}

sw_error_t
qca808x_ptp_freq_waveform_period_1_reg_wave_period_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_freq_waveform_period_1_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_freq_waveform_period_1_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.wave_period;
	return ret;
}

sw_error_t
qca808x_ptp_freq_waveform_period_1_reg_wave_period_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_freq_waveform_period_1_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_freq_waveform_period_1_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.wave_period = value;
	ret = qca808x_ptp_freq_waveform_period_1_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_freq_waveform_period_2_reg_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_freq_waveform_period_2_reg_u *value)
{
	return qca808x_phy_ptp_mmd_read(
				dev_id,
				phy_id, QCA808X_PHY_MMD3_NUM, PTP_FREQ_WAVEFORM_PERIOD_2_REG_ADDRESS,
				&value->val);
}

sw_error_t
qca808x_ptp_freq_waveform_period_2_reg_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_freq_waveform_period_2_reg_u *value)
{
	return qca808x_phy_ptp_mmd_write(
				dev_id,
				phy_id, QCA808X_PHY_MMD3_NUM, PTP_FREQ_WAVEFORM_PERIOD_2_REG_ADDRESS,
				value->val);
}

sw_error_t
qca808x_ptp_freq_waveform_period_2_reg_wave_period_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_freq_waveform_period_2_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_freq_waveform_period_2_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.wave_period;
	return ret;
}

sw_error_t
qca808x_ptp_freq_waveform_period_2_reg_wave_period_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_freq_waveform_period_2_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_freq_waveform_period_2_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.wave_period = value;
	ret = qca808x_ptp_freq_waveform_period_2_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_rx_com_ts_ctrl_reg_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_rx_com_ts_ctrl_reg_u *value)
{
	return qca808x_phy_ptp_mmd_read(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_RX_COM_TS_CTRL_REG_ADDRESS,
			&value->val);
}

sw_error_t
qca808x_ptp_rx_com_ts_ctrl_reg_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_rx_com_ts_ctrl_reg_u *value)
{
	return qca808x_phy_ptp_mmd_write(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_RX_COM_TS_CTRL_REG_ADDRESS,
			value->val);
}

sw_error_t
qca808x_ptp_rx_filt_mac_da0_reg_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_rx_filt_mac_da0_reg_u *value)
{
	return qca808x_phy_ptp_mmd_read(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_RX_FILT_MAC_DA0_REG_ADDRESS,
			&value->val);
}

sw_error_t
qca808x_ptp_rx_filt_mac_da0_reg_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_rx_filt_mac_da0_reg_u *value)
{
	return qca808x_phy_ptp_mmd_write(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_RX_FILT_MAC_DA0_REG_ADDRESS,
			value->val);
}

sw_error_t
qca808x_ptp_rx_filt_mac_da1_reg_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_rx_filt_mac_da1_reg_u *value)
{
	return qca808x_phy_ptp_mmd_read(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_RX_FILT_MAC_DA1_REG_ADDRESS,
			&value->val);
}

sw_error_t
qca808x_ptp_rx_filt_mac_da1_reg_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_rx_filt_mac_da1_reg_u *value)
{
	return qca808x_phy_ptp_mmd_write(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_RX_FILT_MAC_DA1_REG_ADDRESS,
			value->val);
}

sw_error_t
qca808x_ptp_rx_filt_mac_da2_reg_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_rx_filt_mac_da2_reg_u *value)
{
	return qca808x_phy_ptp_mmd_read(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_RX_FILT_MAC_DA2_REG_ADDRESS,
			&value->val);
}

sw_error_t
qca808x_ptp_rx_filt_mac_da2_reg_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_rx_filt_mac_da2_reg_u *value)
{
	return qca808x_phy_ptp_mmd_write(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_RX_FILT_MAC_DA2_REG_ADDRESS,
			value->val);
}

sw_error_t
qca808x_ptp_rx_filt_ipv4_da0_reg_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_rx_filt_ipv4_da0_reg_u *value)
{
	return qca808x_phy_ptp_mmd_read(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_RX_FILT_IPV4_DA0_REG_ADDRESS,
			&value->val);
}

sw_error_t
qca808x_ptp_rx_filt_ipv4_da0_reg_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_rx_filt_ipv4_da0_reg_u *value)
{
	return qca808x_phy_ptp_mmd_write(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_RX_FILT_IPV4_DA0_REG_ADDRESS,
			value->val);
}

sw_error_t
qca808x_ptp_rx_filt_ipv4_da1_reg_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_rx_filt_ipv4_da1_reg_u *value)
{
	return qca808x_phy_ptp_mmd_read(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_RX_FILT_IPV4_DA1_REG_ADDRESS,
			&value->val);
}

sw_error_t
qca808x_ptp_rx_filt_ipv4_da1_reg_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_rx_filt_ipv4_da1_reg_u *value)
{
	return qca808x_phy_ptp_mmd_write(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_RX_FILT_IPV4_DA1_REG_ADDRESS,
			value->val);
}

sw_error_t
qca808x_ptp_rx_filt_ipv6_da0_reg_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_rx_filt_ipv6_da0_reg_u *value)
{
	return qca808x_phy_ptp_mmd_read(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_RX_FILT_IPV6_DA0_REG_ADDRESS,
			&value->val);
}

sw_error_t
qca808x_ptp_rx_filt_ipv6_da0_reg_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_rx_filt_ipv6_da0_reg_u *value)
{
	return qca808x_phy_ptp_mmd_write(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_RX_FILT_IPV6_DA0_REG_ADDRESS,
			value->val);
}

sw_error_t
qca808x_ptp_rx_filt_ipv6_da1_reg_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_rx_filt_ipv6_da1_reg_u *value)
{
	return qca808x_phy_ptp_mmd_read(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_RX_FILT_IPV6_DA1_REG_ADDRESS,
			&value->val);
}

sw_error_t
qca808x_ptp_rx_filt_ipv6_da1_reg_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_rx_filt_ipv6_da1_reg_u *value)
{
	return qca808x_phy_ptp_mmd_write(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_RX_FILT_IPV6_DA1_REG_ADDRESS,
			value->val);
}

sw_error_t
qca808x_ptp_rx_filt_ipv6_da2_reg_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_rx_filt_ipv6_da2_reg_u *value)
{
	return qca808x_phy_ptp_mmd_read(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_RX_FILT_IPV6_DA2_REG_ADDRESS,
			&value->val);
}

sw_error_t
qca808x_ptp_rx_filt_ipv6_da2_reg_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_rx_filt_ipv6_da2_reg_u *value)
{
	return qca808x_phy_ptp_mmd_write(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_RX_FILT_IPV6_DA2_REG_ADDRESS,
			value->val);
}

sw_error_t
qca808x_ptp_rx_filt_ipv6_da3_reg_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_rx_filt_ipv6_da3_reg_u *value)
{
	return qca808x_phy_ptp_mmd_read(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_RX_FILT_IPV6_DA3_REG_ADDRESS,
			&value->val);
}

sw_error_t
qca808x_ptp_rx_filt_ipv6_da3_reg_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_rx_filt_ipv6_da3_reg_u *value)
{
	return qca808x_phy_ptp_mmd_write(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_RX_FILT_IPV6_DA3_REG_ADDRESS,
			value->val);
}

sw_error_t
qca808x_ptp_rx_filt_ipv6_da4_reg_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_rx_filt_ipv6_da4_reg_u *value)
{
	return qca808x_phy_ptp_mmd_read(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_RX_FILT_IPV6_DA4_REG_ADDRESS,
			&value->val);
}

sw_error_t
qca808x_ptp_rx_filt_ipv6_da4_reg_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_rx_filt_ipv6_da4_reg_u *value)
{
	return qca808x_phy_ptp_mmd_write(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_RX_FILT_IPV6_DA4_REG_ADDRESS,
			value->val);
}

sw_error_t
qca808x_ptp_rx_filt_ipv6_da5_reg_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_rx_filt_ipv6_da5_reg_u *value)
{
	return qca808x_phy_ptp_mmd_read(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_RX_FILT_IPV6_DA5_REG_ADDRESS,
			&value->val);
}

sw_error_t
qca808x_ptp_rx_filt_ipv6_da5_reg_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_rx_filt_ipv6_da5_reg_u *value)
{
	return qca808x_phy_ptp_mmd_write(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_RX_FILT_IPV6_DA5_REG_ADDRESS,
			value->val);
}

sw_error_t
qca808x_ptp_rx_filt_ipv6_da6_reg_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_rx_filt_ipv6_da6_reg_u *value)
{
	return qca808x_phy_ptp_mmd_read(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_RX_FILT_IPV6_DA6_REG_ADDRESS,
			&value->val);
}

sw_error_t
qca808x_ptp_rx_filt_ipv6_da6_reg_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_rx_filt_ipv6_da6_reg_u *value)
{
	return qca808x_phy_ptp_mmd_write(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_RX_FILT_IPV6_DA6_REG_ADDRESS,
			value->val);
}

sw_error_t
qca808x_ptp_rx_filt_ipv6_da7_reg_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_rx_filt_ipv6_da7_reg_u *value)
{
	return qca808x_phy_ptp_mmd_read(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_RX_FILT_IPV6_DA7_REG_ADDRESS,
			&value->val);
}

sw_error_t
qca808x_ptp_rx_filt_ipv6_da7_reg_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_rx_filt_ipv6_da7_reg_u *value)
{
	return qca808x_phy_ptp_mmd_write(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_RX_FILT_IPV6_DA7_REG_ADDRESS,
			value->val);
}

sw_error_t
qca808x_ptp_rx_filt_mac_lengthtype_reg_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_rx_filt_mac_lengthtype_reg_u *value)
{
	return qca808x_phy_ptp_mmd_read(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_RX_FILT_MAC_LENGTHTYPE_REG_ADDRESS,
			&value->val);
}

sw_error_t
qca808x_ptp_rx_filt_mac_lengthtype_reg_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_rx_filt_mac_lengthtype_reg_u *value)
{
	return qca808x_phy_ptp_mmd_write(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_RX_FILT_MAC_LENGTHTYPE_REG_ADDRESS,
			value->val);
}

sw_error_t
qca808x_ptp_rx_filt_layer4_protocol_reg_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_rx_filt_layer4_protocol_reg_u *value)
{
	return qca808x_phy_ptp_mmd_read(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_RX_FILT_LAYER4_PROTOCOL_REG_ADDRESS,
			&value->val);
}

sw_error_t
qca808x_ptp_rx_filt_layer4_protocol_reg_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_rx_filt_layer4_protocol_reg_u *value)
{
	return qca808x_phy_ptp_mmd_write(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_RX_FILT_LAYER4_PROTOCOL_REG_ADDRESS,
			value->val);
}

sw_error_t
qca808x_ptp_rx_filt_udp_port_reg_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_rx_filt_udp_port_reg_u *value)
{
	return qca808x_phy_ptp_mmd_read(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_RX_FILT_UDP_PORT_REG_ADDRESS,
			&value->val);
}

sw_error_t
qca808x_ptp_rx_filt_udp_port_reg_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_rx_filt_udp_port_reg_u *value)
{
	return qca808x_phy_ptp_mmd_write(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_RX_FILT_UDP_PORT_REG_ADDRESS,
			value->val);
}

sw_error_t
qca808x_ptp_rx_com_ts_status_reg_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_rx_com_ts_status_reg_u *value)
{
	return qca808x_phy_ptp_mmd_read(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_RX_COM_TS_STATUS_REG_ADDRESS,
			&value->val);
}

sw_error_t
qca808x_ptp_rx_com_ts_status_reg_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_rx_com_ts_status_reg_u *value)
{
	return qca808x_phy_ptp_mmd_write(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_RX_COM_TS_STATUS_REG_ADDRESS,
			value->val);
}

sw_error_t
qca808x_ptp_rx_com_timestamp0_reg_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_rx_com_timestamp0_reg_u *value)
{
	return qca808x_phy_ptp_mmd_read(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_RX_COM_TIMESTAMP0_REG_ADDRESS,
			&value->val);
}

sw_error_t
qca808x_ptp_rx_com_timestamp0_reg_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_rx_com_timestamp0_reg_u *value)
{
	return qca808x_phy_ptp_mmd_write(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_RX_COM_TIMESTAMP0_REG_ADDRESS,
			value->val);
}

sw_error_t
qca808x_ptp_rx_com_timestamp1_reg_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_rx_com_timestamp1_reg_u *value)
{
	return qca808x_phy_ptp_mmd_read(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_RX_COM_TIMESTAMP1_REG_ADDRESS,
			&value->val);
}

sw_error_t
qca808x_ptp_rx_com_timestamp1_reg_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_rx_com_timestamp1_reg_u *value)
{
	return qca808x_phy_ptp_mmd_write(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_RX_COM_TIMESTAMP1_REG_ADDRESS,
			value->val);
}

sw_error_t
qca808x_ptp_rx_com_timestamp2_reg_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_rx_com_timestamp2_reg_u *value)
{
	return qca808x_phy_ptp_mmd_read(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_RX_COM_TIMESTAMP2_REG_ADDRESS,
			&value->val);
}

sw_error_t
qca808x_ptp_rx_com_timestamp2_reg_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_rx_com_timestamp2_reg_u *value)
{
	return qca808x_phy_ptp_mmd_write(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_RX_COM_TIMESTAMP2_REG_ADDRESS,
			value->val);
}

sw_error_t
qca808x_ptp_rx_com_timestamp3_reg_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_rx_com_timestamp3_reg_u *value)
{
	return qca808x_phy_ptp_mmd_read(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_RX_COM_TIMESTAMP3_REG_ADDRESS,
			&value->val);
}

sw_error_t
qca808x_ptp_rx_com_timestamp3_reg_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_rx_com_timestamp3_reg_u *value)
{
	return qca808x_phy_ptp_mmd_write(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_RX_COM_TIMESTAMP3_REG_ADDRESS,
			value->val);
}

sw_error_t
qca808x_ptp_rx_com_timestamp4_reg_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_rx_com_timestamp4_reg_u *value)
{
	return qca808x_phy_ptp_mmd_read(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_RX_COM_TIMESTAMP4_REG_ADDRESS,
			&value->val);
}

sw_error_t
qca808x_ptp_rx_com_timestamp4_reg_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_rx_com_timestamp4_reg_u *value)
{
	return qca808x_phy_ptp_mmd_write(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_RX_COM_TIMESTAMP4_REG_ADDRESS,
			value->val);
}

sw_error_t
qca808x_ptp_rx_com_frac_nano_reg_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_rx_com_frac_nano_reg_u *value)
{
	return qca808x_phy_ptp_mmd_read(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_RX_COM_FRAC_NANO_REG_ADDRESS,
			&value->val);
}

sw_error_t
qca808x_ptp_rx_com_frac_nano_reg_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_rx_com_frac_nano_reg_u *value)
{
	return qca808x_phy_ptp_mmd_write(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_RX_COM_FRAC_NANO_REG_ADDRESS,
			value->val);
}

sw_error_t
qca808x_ptp_rx_com_timestamp_pre0_reg_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_rx_com_timestamp_pre0_reg_u *value)
{
	return qca808x_phy_ptp_mmd_read(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_RX_COM_TIMESTAMP_PRE0_REG_ADDRESS,
			&value->val);
}

sw_error_t
qca808x_ptp_rx_com_timestamp_pre0_reg_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_rx_com_timestamp_pre0_reg_u *value)
{
	return qca808x_phy_ptp_mmd_write(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_RX_COM_TIMESTAMP_PRE0_REG_ADDRESS,
			value->val);
}

sw_error_t
qca808x_ptp_rx_com_timestamp_pre1_reg_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_rx_com_timestamp_pre1_reg_u *value)
{
	return qca808x_phy_ptp_mmd_read(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_RX_COM_TIMESTAMP_PRE1_REG_ADDRESS,
			&value->val);
}

sw_error_t
qca808x_ptp_rx_com_timestamp_pre1_reg_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_rx_com_timestamp_pre1_reg_u *value)
{
	return qca808x_phy_ptp_mmd_write(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_RX_COM_TIMESTAMP_PRE1_REG_ADDRESS,
			value->val);
}

sw_error_t
qca808x_ptp_rx_com_timestamp_pre2_reg_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_rx_com_timestamp_pre2_reg_u *value)
{
	return qca808x_phy_ptp_mmd_read(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_RX_COM_TIMESTAMP_PRE2_REG_ADDRESS,
			&value->val);
}

sw_error_t
qca808x_ptp_rx_com_timestamp_pre2_reg_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_rx_com_timestamp_pre2_reg_u *value)
{
	return qca808x_phy_ptp_mmd_write(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_RX_COM_TIMESTAMP_PRE2_REG_ADDRESS,
			value->val);
}

sw_error_t
qca808x_ptp_rx_com_timestamp_pre3_reg_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_rx_com_timestamp_pre3_reg_u *value)
{
	return qca808x_phy_ptp_mmd_read(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_RX_COM_TIMESTAMP_PRE3_REG_ADDRESS,
			&value->val);
}

sw_error_t
qca808x_ptp_rx_com_timestamp_pre3_reg_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_rx_com_timestamp_pre3_reg_u *value)
{
	return qca808x_phy_ptp_mmd_write(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_RX_COM_TIMESTAMP_PRE3_REG_ADDRESS,
			value->val);
}

sw_error_t
qca808x_ptp_rx_com_timestamp_pre4_reg_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_rx_com_timestamp_pre4_reg_u *value)
{
	return qca808x_phy_ptp_mmd_read(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_RX_COM_TIMESTAMP_PRE4_REG_ADDRESS,
			&value->val);
}

sw_error_t
qca808x_ptp_rx_com_timestamp_pre4_reg_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_rx_com_timestamp_pre4_reg_u *value)
{
	return qca808x_phy_ptp_mmd_write(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_RX_COM_TIMESTAMP_PRE4_REG_ADDRESS,
			value->val);
}

sw_error_t
qca808x_ptp_rx_com_frac_nano_pre_reg_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_rx_com_frac_nano_pre_reg_u *value)
{
	return qca808x_phy_ptp_mmd_read(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_RX_COM_FRAC_NANO_PRE_REG_ADDRESS,
			&value->val);
}

sw_error_t
qca808x_ptp_rx_com_frac_nano_pre_reg_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_rx_com_frac_nano_pre_reg_u *value)
{
	return qca808x_phy_ptp_mmd_write(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_RX_COM_FRAC_NANO_PRE_REG_ADDRESS,
			value->val);
}

sw_error_t
qca808x_ptp_rx_y1731_identify_reg_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_rx_y1731_identify_reg_u *value)
{
	return qca808x_phy_ptp_mmd_read(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_RX_Y1731_IDENTIFY_REG_ADDRESS,
			&value->val);
}

sw_error_t
qca808x_ptp_rx_y1731_identify_reg_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_rx_y1731_identify_reg_u *value)
{
	return qca808x_phy_ptp_mmd_write(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_RX_Y1731_IDENTIFY_REG_ADDRESS,
			value->val);
}

sw_error_t
qca808x_ptp_rx_y1731_identify_pre_reg_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_rx_y1731_identify_pre_reg_u *value)
{
	return qca808x_phy_ptp_mmd_read(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_RX_Y1731_IDENTIFY_PRE_REG_ADDRESS,
			&value->val);
}

sw_error_t
qca808x_ptp_rx_y1731_identify_pre_reg_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_rx_y1731_identify_pre_reg_u *value)
{
	return qca808x_phy_ptp_mmd_write(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_RX_Y1731_IDENTIFY_PRE_REG_ADDRESS,
			value->val);
}

sw_error_t
qca808x_ptp_tx_com_ts_ctrl_reg_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_tx_com_ts_ctrl_reg_u *value)
{
	return qca808x_phy_ptp_mmd_read(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_TX_COM_TS_CTRL_REG_ADDRESS,
			&value->val);
}

sw_error_t
qca808x_ptp_tx_com_ts_ctrl_reg_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_tx_com_ts_ctrl_reg_u *value)
{
	return qca808x_phy_ptp_mmd_write(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_TX_COM_TS_CTRL_REG_ADDRESS,
			value->val);
}

sw_error_t
qca808x_ptp_tx_filt_mac_da0_reg_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_tx_filt_mac_da0_reg_u *value)
{
	return qca808x_phy_ptp_mmd_read(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_TX_FILT_MAC_DA0_REG_ADDRESS,
			&value->val);
}

sw_error_t
qca808x_ptp_tx_filt_mac_da0_reg_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_tx_filt_mac_da0_reg_u *value)
{
	return qca808x_phy_ptp_mmd_write(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_TX_FILT_MAC_DA0_REG_ADDRESS,
			value->val);
}

sw_error_t
qca808x_ptp_tx_filt_mac_da1_reg_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_tx_filt_mac_da1_reg_u *value)
{
	return qca808x_phy_ptp_mmd_read(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_TX_FILT_MAC_DA1_REG_ADDRESS,
			&value->val);
}

sw_error_t
qca808x_ptp_tx_filt_mac_da1_reg_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_tx_filt_mac_da1_reg_u *value)
{
	return qca808x_phy_ptp_mmd_write(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_TX_FILT_MAC_DA1_REG_ADDRESS,
			value->val);
}

sw_error_t
qca808x_ptp_tx_filt_mac_da2_reg_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_tx_filt_mac_da2_reg_u *value)
{
	return qca808x_phy_ptp_mmd_read(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_TX_FILT_MAC_DA2_REG_ADDRESS,
			&value->val);
}

sw_error_t
qca808x_ptp_tx_filt_mac_da2_reg_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_tx_filt_mac_da2_reg_u *value)
{
	return qca808x_phy_ptp_mmd_write(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_TX_FILT_MAC_DA2_REG_ADDRESS,
			value->val);
}

sw_error_t
qca808x_ptp_tx_filt_ipv4_da0_reg_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_tx_filt_ipv4_da0_reg_u *value)
{
	return qca808x_phy_ptp_mmd_read(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_TX_FILT_IPV4_DA0_REG_ADDRESS,
			&value->val);
}

sw_error_t
qca808x_ptp_tx_filt_ipv4_da0_reg_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_tx_filt_ipv4_da0_reg_u *value)
{
	return qca808x_phy_ptp_mmd_write(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_TX_FILT_IPV4_DA0_REG_ADDRESS,
			value->val);
}

sw_error_t
qca808x_ptp_tx_filt_ipv4_da1_reg_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_tx_filt_ipv4_da1_reg_u *value)
{
	return qca808x_phy_ptp_mmd_read(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_TX_FILT_IPV4_DA1_REG_ADDRESS,
			&value->val);
}

sw_error_t
qca808x_ptp_tx_filt_ipv4_da1_reg_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_tx_filt_ipv4_da1_reg_u *value)
{
	return qca808x_phy_ptp_mmd_write(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_TX_FILT_IPV4_DA1_REG_ADDRESS,
			value->val);
}

sw_error_t
qca808x_ptp_tx_filt_ipv6_da0_reg_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_tx_filt_ipv6_da0_reg_u *value)
{
	return qca808x_phy_ptp_mmd_read(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_TX_FILT_IPV6_DA0_REG_ADDRESS,
			&value->val);
}

sw_error_t
qca808x_ptp_tx_filt_ipv6_da0_reg_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_tx_filt_ipv6_da0_reg_u *value)
{
	return qca808x_phy_ptp_mmd_write(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_TX_FILT_IPV6_DA0_REG_ADDRESS,
			value->val);
}

sw_error_t
qca808x_ptp_tx_filt_ipv6_da1_reg_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_tx_filt_ipv6_da1_reg_u *value)
{
	return qca808x_phy_ptp_mmd_read(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_TX_FILT_IPV6_DA1_REG_ADDRESS,
			&value->val);
}

sw_error_t
qca808x_ptp_tx_filt_ipv6_da1_reg_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_tx_filt_ipv6_da1_reg_u *value)
{
	return qca808x_phy_ptp_mmd_write(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_TX_FILT_IPV6_DA1_REG_ADDRESS,
			value->val);
}

sw_error_t
qca808x_ptp_tx_filt_ipv6_da2_reg_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_tx_filt_ipv6_da2_reg_u *value)
{
	return qca808x_phy_ptp_mmd_read(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_TX_FILT_IPV6_DA2_REG_ADDRESS,
			&value->val);
}

sw_error_t
qca808x_ptp_tx_filt_ipv6_da2_reg_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_tx_filt_ipv6_da2_reg_u *value)
{
	return qca808x_phy_ptp_mmd_write(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_TX_FILT_IPV6_DA2_REG_ADDRESS,
			value->val);
}

sw_error_t
qca808x_ptp_tx_filt_ipv6_da3_reg_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_tx_filt_ipv6_da3_reg_u *value)
{
	return qca808x_phy_ptp_mmd_read(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_TX_FILT_IPV6_DA3_REG_ADDRESS,
			&value->val);
}

sw_error_t
qca808x_ptp_tx_filt_ipv6_da3_reg_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_tx_filt_ipv6_da3_reg_u *value)
{
	return qca808x_phy_ptp_mmd_write(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_TX_FILT_IPV6_DA3_REG_ADDRESS,
			value->val);
}

sw_error_t
qca808x_ptp_tx_filt_ipv6_da4_reg_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_tx_filt_ipv6_da4_reg_u *value)
{
	return qca808x_phy_ptp_mmd_read(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_TX_FILT_IPV6_DA4_REG_ADDRESS,
			&value->val);
}

sw_error_t
qca808x_ptp_tx_filt_ipv6_da4_reg_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_tx_filt_ipv6_da4_reg_u *value)
{
	return qca808x_phy_ptp_mmd_write(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_TX_FILT_IPV6_DA4_REG_ADDRESS,
			value->val);
}

sw_error_t
qca808x_ptp_tx_filt_ipv6_da5_reg_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_tx_filt_ipv6_da5_reg_u *value)
{
	return qca808x_phy_ptp_mmd_read(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_TX_FILT_IPV6_DA5_REG_ADDRESS,
			&value->val);
}

sw_error_t
qca808x_ptp_tx_filt_ipv6_da5_reg_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_tx_filt_ipv6_da5_reg_u *value)
{
	return qca808x_phy_ptp_mmd_write(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_TX_FILT_IPV6_DA5_REG_ADDRESS,
			value->val);
}

sw_error_t
qca808x_ptp_tx_filt_ipv6_da6_reg_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_tx_filt_ipv6_da6_reg_u *value)
{
	return qca808x_phy_ptp_mmd_read(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_TX_FILT_IPV6_DA6_REG_ADDRESS,
			&value->val);
}

sw_error_t
qca808x_ptp_tx_filt_ipv6_da6_reg_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_tx_filt_ipv6_da6_reg_u *value)
{
	return qca808x_phy_ptp_mmd_write(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_TX_FILT_IPV6_DA6_REG_ADDRESS,
			value->val);
}

sw_error_t
qca808x_ptp_tx_filt_ipv6_da7_reg_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_tx_filt_ipv6_da7_reg_u *value)
{
	return qca808x_phy_ptp_mmd_read(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_TX_FILT_IPV6_DA7_REG_ADDRESS,
			&value->val);
}

sw_error_t
qca808x_ptp_tx_filt_ipv6_da7_reg_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_tx_filt_ipv6_da7_reg_u *value)
{
	return qca808x_phy_ptp_mmd_write(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_TX_FILT_IPV6_DA7_REG_ADDRESS,
			value->val);
}

sw_error_t
qca808x_ptp_tx_filt_mac_lengthtype_reg_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_tx_filt_mac_lengthtype_reg_u *value)
{
	return qca808x_phy_ptp_mmd_read(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_TX_FILT_MAC_LENGTHTYPE_REG_ADDRESS,
			&value->val);
}

sw_error_t
qca808x_ptp_tx_filt_mac_lengthtype_reg_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_tx_filt_mac_lengthtype_reg_u *value)
{
	return qca808x_phy_ptp_mmd_write(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_TX_FILT_MAC_LENGTHTYPE_REG_ADDRESS,
			value->val);
}

sw_error_t
qca808x_ptp_tx_filt_layer4_protocol_reg_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_tx_filt_layer4_protocol_reg_u *value)
{
	return qca808x_phy_ptp_mmd_read(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_TX_FILT_LAYER4_PROTOCOL_REG_ADDRESS,
			&value->val);
}

sw_error_t
qca808x_ptp_tx_filt_layer4_protocol_reg_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_tx_filt_layer4_protocol_reg_u *value)
{
	return qca808x_phy_ptp_mmd_write(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_TX_FILT_LAYER4_PROTOCOL_REG_ADDRESS,
			value->val);
}

sw_error_t
qca808x_ptp_tx_filt_udp_port_reg_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_tx_filt_udp_port_reg_u *value)
{
	return qca808x_phy_ptp_mmd_read(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_TX_FILT_UDP_PORT_REG_ADDRESS,
			&value->val);
}

sw_error_t
qca808x_ptp_tx_filt_udp_port_reg_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_tx_filt_udp_port_reg_u *value)
{
	return qca808x_phy_ptp_mmd_write(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_TX_FILT_UDP_PORT_REG_ADDRESS,
			value->val);
}

sw_error_t
qca808x_ptp_tx_com_ts_status_reg_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_tx_com_ts_status_reg_u *value)
{
	return qca808x_phy_ptp_mmd_read(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_TX_COM_TS_STATUS_REG_ADDRESS,
			&value->val);
}

sw_error_t
qca808x_ptp_tx_com_ts_status_reg_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_tx_com_ts_status_reg_u *value)
{
	return qca808x_phy_ptp_mmd_write(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_TX_COM_TS_STATUS_REG_ADDRESS,
			value->val);
}

sw_error_t
qca808x_ptp_tx_com_timestamp0_reg_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_tx_com_timestamp0_reg_u *value)
{
	return qca808x_phy_ptp_mmd_read(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_TX_COM_TIMESTAMP0_REG_ADDRESS,
			&value->val);
}

sw_error_t
qca808x_ptp_tx_com_timestamp0_reg_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_tx_com_timestamp0_reg_u *value)
{
	return qca808x_phy_ptp_mmd_write(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_TX_COM_TIMESTAMP0_REG_ADDRESS,
			value->val);
}

sw_error_t
qca808x_ptp_tx_com_timestamp1_reg_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_tx_com_timestamp1_reg_u *value)
{
	return qca808x_phy_ptp_mmd_read(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_TX_COM_TIMESTAMP1_REG_ADDRESS,
			&value->val);
}

sw_error_t
qca808x_ptp_tx_com_timestamp1_reg_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_tx_com_timestamp1_reg_u *value)
{
	return qca808x_phy_ptp_mmd_write(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_TX_COM_TIMESTAMP1_REG_ADDRESS,
			value->val);
}

sw_error_t
qca808x_ptp_tx_com_timestamp2_reg_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_tx_com_timestamp2_reg_u *value)
{
	return qca808x_phy_ptp_mmd_read(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_TX_COM_TIMESTAMP2_REG_ADDRESS,
			&value->val);
}

sw_error_t
qca808x_ptp_tx_com_timestamp2_reg_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_tx_com_timestamp2_reg_u *value)
{
	return qca808x_phy_ptp_mmd_write(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_TX_COM_TIMESTAMP2_REG_ADDRESS,
			value->val);
}

sw_error_t
qca808x_ptp_tx_com_timestamp3_reg_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_tx_com_timestamp3_reg_u *value)
{
	return qca808x_phy_ptp_mmd_read(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_TX_COM_TIMESTAMP3_REG_ADDRESS,
			&value->val);
}

sw_error_t
qca808x_ptp_tx_com_timestamp3_reg_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_tx_com_timestamp3_reg_u *value)
{
	return qca808x_phy_ptp_mmd_write(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_TX_COM_TIMESTAMP3_REG_ADDRESS,
			value->val);
}

sw_error_t
qca808x_ptp_tx_com_timestamp4_reg_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_tx_com_timestamp4_reg_u *value)
{
	return qca808x_phy_ptp_mmd_read(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_TX_COM_TIMESTAMP4_REG_ADDRESS,
			&value->val);
}

sw_error_t
qca808x_ptp_tx_com_timestamp4_reg_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_tx_com_timestamp4_reg_u *value)
{
	return qca808x_phy_ptp_mmd_write(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_TX_COM_TIMESTAMP4_REG_ADDRESS,
			value->val);
}

sw_error_t
qca808x_ptp_tx_com_frac_nano_reg_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_tx_com_frac_nano_reg_u *value)
{
	return qca808x_phy_ptp_mmd_read(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_TX_COM_FRAC_NANO_REG_ADDRESS,
			&value->val);
}

sw_error_t
qca808x_ptp_tx_com_frac_nano_reg_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_tx_com_frac_nano_reg_u *value)
{
	return qca808x_phy_ptp_mmd_write(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_TX_COM_FRAC_NANO_REG_ADDRESS,
			value->val);
}

sw_error_t
qca808x_ptp_tx_y1731_identify_reg_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_tx_y1731_identify_reg_u *value)
{
	return qca808x_phy_ptp_mmd_read(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_TX_Y1731_IDENTIFY_REG_ADDRESS,
			&value->val);
}

sw_error_t
qca808x_ptp_tx_y1731_identify_reg_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_tx_y1731_identify_reg_u *value)
{
	return qca808x_phy_ptp_mmd_write(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_TX_Y1731_IDENTIFY_REG_ADDRESS,
			value->val);
}

sw_error_t
qca808x_ptp_y1731_dm_control_reg_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_y1731_dm_control_reg_u *value)
{
	return qca808x_phy_ptp_mmd_read(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_Y1731_DM_CONTROL_REG_ADDRESS,
			&value->val);
}

sw_error_t
qca808x_ptp_y1731_dm_control_reg_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_y1731_dm_control_reg_u *value)
{
	return qca808x_phy_ptp_mmd_write(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_Y1731_DM_CONTROL_REG_ADDRESS,
			value->val);
}

sw_error_t
qca808x_ptp_rx_com_ts_status_pre_reg_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_rx_com_ts_status_pre_reg_u *value)
{
	return qca808x_phy_ptp_mmd_read(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_RX_COM_TS_STATUS_PRE_REG_ADDRESS,
			&value->val);
}

sw_error_t
qca808x_ptp_rx_com_ts_status_pre_reg_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_rx_com_ts_status_pre_reg_u *value)
{
	return qca808x_phy_ptp_mmd_write(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_RX_COM_TS_STATUS_PRE_REG_ADDRESS,
			value->val);
}

sw_error_t
qca808x_ptp_baud_config_reg_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_baud_config_reg_u *value)
{
	return qca808x_phy_ptp_mmd_read(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_BAUD_CONFIG_REG_ADDRESS,
			&value->val);
}

sw_error_t
qca808x_ptp_baud_config_reg_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_baud_config_reg_u *value)
{
	return qca808x_phy_ptp_mmd_write(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_BAUD_CONFIG_REG_ADDRESS,
			value->val);
}

sw_error_t
qca808x_ptp_uart_configuration_reg_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_uart_configuration_reg_u *value)
{
	return qca808x_phy_ptp_mmd_read(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_UART_CONFIGURATION_REG_ADDRESS,
			&value->val);
}

sw_error_t
qca808x_ptp_uart_configuration_reg_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_uart_configuration_reg_u *value)
{
	return qca808x_phy_ptp_mmd_write(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_UART_CONFIGURATION_REG_ADDRESS,
			value->val);
}

sw_error_t
qca808x_ptp_reset_buffer_reg_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_reset_buffer_reg_u *value)
{
	return qca808x_phy_ptp_mmd_read(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_RESET_BUFFER_REG_ADDRESS,
			&value->val);
}

sw_error_t
qca808x_ptp_reset_buffer_reg_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_reset_buffer_reg_u *value)
{
	return qca808x_phy_ptp_mmd_write(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_RESET_BUFFER_REG_ADDRESS,
			value->val);
}

sw_error_t
qca808x_ptp_buffer_status_reg_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_buffer_status_reg_u *value)
{
	return qca808x_phy_ptp_mmd_read(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_BUFFER_STATUS_REG_ADDRESS,
			&value->val);
}

sw_error_t
qca808x_ptp_buffer_status_reg_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_buffer_status_reg_u *value)
{
	return qca808x_phy_ptp_mmd_write(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_BUFFER_STATUS_REG_ADDRESS,
			value->val);
}

sw_error_t
qca808x_ptp_tx_buffer_write_reg_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_tx_buffer_write_reg_u *value)
{
	return qca808x_phy_ptp_mmd_read(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_TX_BUFFER_WRITE_REG_ADDRESS,
			&value->val);
}

sw_error_t
qca808x_ptp_tx_buffer_write_reg_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_tx_buffer_write_reg_u *value)
{
	return qca808x_phy_ptp_mmd_write(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_TX_BUFFER_WRITE_REG_ADDRESS,
			value->val);
}

sw_error_t
qca808x_ptp_rx_buffer_read_reg_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_rx_buffer_read_reg_u *value)
{
	return qca808x_phy_ptp_mmd_read(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_RX_BUFFER_READ_REG_ADDRESS,
			&value->val);
}

sw_error_t
qca808x_ptp_rx_buffer_read_reg_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_rx_buffer_read_reg_u *value)
{
	return qca808x_phy_ptp_mmd_write(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_RX_BUFFER_READ_REG_ADDRESS,
			value->val);
}

sw_error_t
qca808x_ptp_rx_com_ts_ctrl_reg_ipv4_da_en_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_rx_com_ts_ctrl_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rx_com_ts_ctrl_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.ipv4_da_en;
	return ret;
}

sw_error_t
qca808x_ptp_rx_com_ts_ctrl_reg_ipv4_da_en_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_rx_com_ts_ctrl_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rx_com_ts_ctrl_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ipv4_da_en = value;
	ret = qca808x_ptp_rx_com_ts_ctrl_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_rx_com_ts_ctrl_reg_mac_lengthtype_en_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_rx_com_ts_ctrl_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rx_com_ts_ctrl_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.mac_lengthtype_en;
	return ret;
}

sw_error_t
qca808x_ptp_rx_com_ts_ctrl_reg_mac_lengthtype_en_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_rx_com_ts_ctrl_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rx_com_ts_ctrl_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.mac_lengthtype_en = value;
	ret = qca808x_ptp_rx_com_ts_ctrl_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_rx_com_ts_ctrl_reg_y1731_da_chk_en_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_rx_com_ts_ctrl_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rx_com_ts_ctrl_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.y1731_da_chk_en;
	return ret;
}

sw_error_t
qca808x_ptp_rx_com_ts_ctrl_reg_y1731_da_chk_en_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_rx_com_ts_ctrl_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rx_com_ts_ctrl_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.y1731_da_chk_en = value;
	ret = qca808x_ptp_rx_com_ts_ctrl_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_rx_com_ts_ctrl_reg_pw_mac_en_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_rx_com_ts_ctrl_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rx_com_ts_ctrl_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.pw_mac_en;
	return ret;
}

sw_error_t
qca808x_ptp_rx_com_ts_ctrl_reg_pw_mac_en_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_rx_com_ts_ctrl_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rx_com_ts_ctrl_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.pw_mac_en = value;
	ret = qca808x_ptp_rx_com_ts_ctrl_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_rx_com_ts_ctrl_reg_udp_ptp_event_filt_en_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_rx_com_ts_ctrl_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rx_com_ts_ctrl_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.udp_ptp_event_filt_en;
	return ret;
}

sw_error_t
qca808x_ptp_rx_com_ts_ctrl_reg_udp_ptp_event_filt_en_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_rx_com_ts_ctrl_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rx_com_ts_ctrl_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.udp_ptp_event_filt_en = value;
	ret = qca808x_ptp_rx_com_ts_ctrl_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_rx_com_ts_ctrl_reg_udp_dport_en_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_rx_com_ts_ctrl_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rx_com_ts_ctrl_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.udp_dport_en;
	return ret;
}

sw_error_t
qca808x_ptp_rx_com_ts_ctrl_reg_udp_dport_en_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_rx_com_ts_ctrl_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rx_com_ts_ctrl_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.udp_dport_en = value;
	ret = qca808x_ptp_rx_com_ts_ctrl_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_rx_com_ts_ctrl_reg_mac_ptp_filt_en_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_rx_com_ts_ctrl_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rx_com_ts_ctrl_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.mac_ptp_filt_en;
	return ret;
}

sw_error_t
qca808x_ptp_rx_com_ts_ctrl_reg_mac_ptp_filt_en_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_rx_com_ts_ctrl_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rx_com_ts_ctrl_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.mac_ptp_filt_en = value;
	ret = qca808x_ptp_rx_com_ts_ctrl_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_rx_com_ts_ctrl_reg_ipv6_da_filt_en_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_rx_com_ts_ctrl_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rx_com_ts_ctrl_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.ipv6_da_filt_en;
	return ret;
}

sw_error_t
qca808x_ptp_rx_com_ts_ctrl_reg_ipv6_da_filt_en_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_rx_com_ts_ctrl_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rx_com_ts_ctrl_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ipv6_da_filt_en = value;
	ret = qca808x_ptp_rx_com_ts_ctrl_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_rx_com_ts_ctrl_reg_ipv6_next_header_en_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_rx_com_ts_ctrl_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rx_com_ts_ctrl_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.ipv6_next_header_en;
	return ret;
}

sw_error_t
qca808x_ptp_rx_com_ts_ctrl_reg_ipv6_next_header_en_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_rx_com_ts_ctrl_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rx_com_ts_ctrl_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ipv6_next_header_en = value;
	ret = qca808x_ptp_rx_com_ts_ctrl_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_rx_com_ts_ctrl_reg_ipv4_layer4_protocol_en_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_rx_com_ts_ctrl_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rx_com_ts_ctrl_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.ipv4_layer4_protocol_en;
	return ret;
}

sw_error_t
qca808x_ptp_rx_com_ts_ctrl_reg_ipv4_layer4_protocol_en_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_rx_com_ts_ctrl_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rx_com_ts_ctrl_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ipv4_layer4_protocol_en = value;
	ret = qca808x_ptp_rx_com_ts_ctrl_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_rx_com_ts_ctrl_reg_y1731_en_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_rx_com_ts_ctrl_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rx_com_ts_ctrl_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.y1731_en;
	return ret;
}

sw_error_t
qca808x_ptp_rx_com_ts_ctrl_reg_y1731_en_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_rx_com_ts_ctrl_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rx_com_ts_ctrl_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.y1731_en = value;
	ret = qca808x_ptp_rx_com_ts_ctrl_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_rx_com_ts_ctrl_reg_filt_en_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_rx_com_ts_ctrl_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rx_com_ts_ctrl_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.filt_en;
	return ret;
}

sw_error_t
qca808x_ptp_rx_com_ts_ctrl_reg_filt_en_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_rx_com_ts_ctrl_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rx_com_ts_ctrl_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.filt_en = value;
	ret = qca808x_ptp_rx_com_ts_ctrl_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_rx_com_ts_ctrl_reg_ipv6_ptp_filt_en_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_rx_com_ts_ctrl_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rx_com_ts_ctrl_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.ipv6_ptp_filt_en;
	return ret;
}

sw_error_t
qca808x_ptp_rx_com_ts_ctrl_reg_ipv6_ptp_filt_en_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_rx_com_ts_ctrl_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rx_com_ts_ctrl_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ipv6_ptp_filt_en = value;
	ret = qca808x_ptp_rx_com_ts_ctrl_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_rx_com_ts_ctrl_reg_ipv4_ptp_filt_en_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_rx_com_ts_ctrl_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rx_com_ts_ctrl_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.ipv4_ptp_filt_en;
	return ret;
}

sw_error_t
qca808x_ptp_rx_com_ts_ctrl_reg_ipv4_ptp_filt_en_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_rx_com_ts_ctrl_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rx_com_ts_ctrl_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ipv4_ptp_filt_en = value;
	ret = qca808x_ptp_rx_com_ts_ctrl_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_rx_com_ts_ctrl_reg_y1731_insert_ts_en_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_rx_com_ts_ctrl_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rx_com_ts_ctrl_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.y1731_insert_ts_en;
	return ret;
}

sw_error_t
qca808x_ptp_rx_com_ts_ctrl_reg_y1731_insert_ts_en_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_rx_com_ts_ctrl_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rx_com_ts_ctrl_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.y1731_insert_ts_en = value;
	ret = qca808x_ptp_rx_com_ts_ctrl_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_rx_com_ts_ctrl_reg_mac_da_en_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_rx_com_ts_ctrl_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rx_com_ts_ctrl_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.mac_da_en;
	return ret;
}

sw_error_t
qca808x_ptp_rx_com_ts_ctrl_reg_mac_da_en_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_rx_com_ts_ctrl_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rx_com_ts_ctrl_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.mac_da_en = value;
	ret = qca808x_ptp_rx_com_ts_ctrl_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_rx_filt_mac_da0_reg_mac_addr_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_rx_filt_mac_da0_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rx_filt_mac_da0_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.mac_addr;
	return ret;
}

sw_error_t
qca808x_ptp_rx_filt_mac_da0_reg_mac_addr_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_rx_filt_mac_da0_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rx_filt_mac_da0_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.mac_addr = value;
	ret = qca808x_ptp_rx_filt_mac_da0_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_rx_filt_mac_da1_reg_mac_addr_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_rx_filt_mac_da1_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rx_filt_mac_da1_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.mac_addr;
	return ret;
}

sw_error_t
qca808x_ptp_rx_filt_mac_da1_reg_mac_addr_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_rx_filt_mac_da1_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rx_filt_mac_da1_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.mac_addr = value;
	ret = qca808x_ptp_rx_filt_mac_da1_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_rx_filt_mac_da2_reg_mac_addr_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_rx_filt_mac_da2_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rx_filt_mac_da2_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.mac_addr;
	return ret;
}

sw_error_t
qca808x_ptp_rx_filt_mac_da2_reg_mac_addr_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_rx_filt_mac_da2_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rx_filt_mac_da2_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.mac_addr = value;
	ret = qca808x_ptp_rx_filt_mac_da2_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_rx_filt_ipv4_da0_reg_ip_addr_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_rx_filt_ipv4_da0_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rx_filt_ipv4_da0_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.ip_addr;
	return ret;
}

sw_error_t
qca808x_ptp_rx_filt_ipv4_da0_reg_ip_addr_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_rx_filt_ipv4_da0_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rx_filt_ipv4_da0_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ip_addr = value;
	ret = qca808x_ptp_rx_filt_ipv4_da0_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_rx_filt_ipv4_da1_reg_ip_addr_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_rx_filt_ipv4_da1_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rx_filt_ipv4_da1_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.ip_addr;
	return ret;
}

sw_error_t
qca808x_ptp_rx_filt_ipv4_da1_reg_ip_addr_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_rx_filt_ipv4_da1_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rx_filt_ipv4_da1_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ip_addr = value;
	ret = qca808x_ptp_rx_filt_ipv4_da1_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_rx_filt_ipv6_da0_reg_ip_addr_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_rx_filt_ipv6_da0_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rx_filt_ipv6_da0_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.ip_addr;
	return ret;
}

sw_error_t
qca808x_ptp_rx_filt_ipv6_da0_reg_ip_addr_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_rx_filt_ipv6_da0_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rx_filt_ipv6_da0_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ip_addr = value;
	ret = qca808x_ptp_rx_filt_ipv6_da0_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_rx_filt_ipv6_da1_reg_ip_addr_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_rx_filt_ipv6_da1_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rx_filt_ipv6_da1_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.ip_addr;
	return ret;
}

sw_error_t
qca808x_ptp_rx_filt_ipv6_da1_reg_ip_addr_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_rx_filt_ipv6_da1_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rx_filt_ipv6_da1_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ip_addr = value;
	ret = qca808x_ptp_rx_filt_ipv6_da1_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_rx_filt_ipv6_da2_reg_ip_addr_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_rx_filt_ipv6_da2_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rx_filt_ipv6_da2_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.ip_addr;
	return ret;
}

sw_error_t
qca808x_ptp_rx_filt_ipv6_da2_reg_ip_addr_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_rx_filt_ipv6_da2_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rx_filt_ipv6_da2_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ip_addr = value;
	ret = qca808x_ptp_rx_filt_ipv6_da2_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_rx_filt_ipv6_da3_reg_ip_addr_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_rx_filt_ipv6_da3_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rx_filt_ipv6_da3_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.ip_addr;
	return ret;
}

sw_error_t
qca808x_ptp_rx_filt_ipv6_da3_reg_ip_addr_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_rx_filt_ipv6_da3_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rx_filt_ipv6_da3_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ip_addr = value;
	ret = qca808x_ptp_rx_filt_ipv6_da3_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_rx_filt_ipv6_da4_reg_ip_addr_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_rx_filt_ipv6_da4_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rx_filt_ipv6_da4_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.ip_addr;
	return ret;
}

sw_error_t
qca808x_ptp_rx_filt_ipv6_da4_reg_ip_addr_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_rx_filt_ipv6_da4_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rx_filt_ipv6_da4_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ip_addr = value;
	ret = qca808x_ptp_rx_filt_ipv6_da4_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_rx_filt_ipv6_da5_reg_ip_addr_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_rx_filt_ipv6_da5_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rx_filt_ipv6_da5_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.ip_addr;
	return ret;
}

sw_error_t
qca808x_ptp_rx_filt_ipv6_da5_reg_ip_addr_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_rx_filt_ipv6_da5_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rx_filt_ipv6_da5_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ip_addr = value;
	ret = qca808x_ptp_rx_filt_ipv6_da5_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_rx_filt_ipv6_da6_reg_ip_addr_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_rx_filt_ipv6_da6_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rx_filt_ipv6_da6_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.ip_addr;
	return ret;
}

sw_error_t
qca808x_ptp_rx_filt_ipv6_da6_reg_ip_addr_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_rx_filt_ipv6_da6_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rx_filt_ipv6_da6_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ip_addr = value;
	ret = qca808x_ptp_rx_filt_ipv6_da6_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_rx_filt_ipv6_da7_reg_ip_addr_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_rx_filt_ipv6_da7_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rx_filt_ipv6_da7_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.ip_addr;
	return ret;
}

sw_error_t
qca808x_ptp_rx_filt_ipv6_da7_reg_ip_addr_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_rx_filt_ipv6_da7_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rx_filt_ipv6_da7_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ip_addr = value;
	ret = qca808x_ptp_rx_filt_ipv6_da7_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_rx_filt_mac_lengthtype_reg_length_type_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_rx_filt_mac_lengthtype_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rx_filt_mac_lengthtype_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.length_type;
	return ret;
}

sw_error_t
qca808x_ptp_rx_filt_mac_lengthtype_reg_length_type_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_rx_filt_mac_lengthtype_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rx_filt_mac_lengthtype_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.length_type = value;
	ret = qca808x_ptp_rx_filt_mac_lengthtype_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_rx_filt_layer4_protocol_reg_l4_protocol_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_rx_filt_layer4_protocol_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rx_filt_layer4_protocol_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.l4_protocol;
	return ret;
}

sw_error_t
qca808x_ptp_rx_filt_layer4_protocol_reg_l4_protocol_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_rx_filt_layer4_protocol_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rx_filt_layer4_protocol_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.l4_protocol = value;
	ret = qca808x_ptp_rx_filt_layer4_protocol_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_rx_filt_udp_port_reg_udp_port_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_rx_filt_udp_port_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rx_filt_udp_port_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.udp_port;
	return ret;
}

sw_error_t
qca808x_ptp_rx_filt_udp_port_reg_udp_port_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_rx_filt_udp_port_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rx_filt_udp_port_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.udp_port = value;
	ret = qca808x_ptp_rx_filt_udp_port_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_rx_com_ts_status_reg_mac_ptp_pdelay_addr_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_rx_com_ts_status_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rx_com_ts_status_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.mac_ptp_pdelay_addr;
	return ret;
}

sw_error_t
qca808x_ptp_rx_com_ts_status_reg_mac_ptp_pdelay_addr_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_rx_com_ts_status_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rx_com_ts_status_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.mac_ptp_pdelay_addr = value;
	ret = qca808x_ptp_rx_com_ts_status_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_rx_com_ts_status_reg_mac_da_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_rx_com_ts_status_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rx_com_ts_status_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.mac_da;
	return ret;
}

sw_error_t
qca808x_ptp_rx_com_ts_status_reg_mac_da_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_rx_com_ts_status_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rx_com_ts_status_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.mac_da = value;
	ret = qca808x_ptp_rx_com_ts_status_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_rx_com_ts_status_reg_ipv4_ptp_pdelay_addr_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_rx_com_ts_status_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rx_com_ts_status_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.ipv4_ptp_pdelay_addr;
	return ret;
}

sw_error_t
qca808x_ptp_rx_com_ts_status_reg_ipv4_ptp_pdelay_addr_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_rx_com_ts_status_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rx_com_ts_status_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ipv4_ptp_pdelay_addr = value;
	ret = qca808x_ptp_rx_com_ts_status_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_rx_com_ts_status_reg_mac_lengthtype_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_rx_com_ts_status_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rx_com_ts_status_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.mac_lengthtype;
	return ret;
}

sw_error_t
qca808x_ptp_rx_com_ts_status_reg_mac_lengthtype_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_rx_com_ts_status_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rx_com_ts_status_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.mac_lengthtype = value;
	ret = qca808x_ptp_rx_com_ts_status_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_rx_com_ts_status_reg_udp_dport_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_rx_com_ts_status_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rx_com_ts_status_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.udp_dport;
	return ret;
}

sw_error_t
qca808x_ptp_rx_com_ts_status_reg_udp_dport_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_rx_com_ts_status_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rx_com_ts_status_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.udp_dport = value;
	ret = qca808x_ptp_rx_com_ts_status_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_rx_com_ts_status_reg_ipv4_layer4_protocol_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_rx_com_ts_status_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rx_com_ts_status_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.ipv4_layer4_protocol;
	return ret;
}

sw_error_t
qca808x_ptp_rx_com_ts_status_reg_ipv4_layer4_protocol_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_rx_com_ts_status_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rx_com_ts_status_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ipv4_layer4_protocol = value;
	ret = qca808x_ptp_rx_com_ts_status_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_rx_com_ts_status_reg_udp_ptp_event_dport_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_rx_com_ts_status_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rx_com_ts_status_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.udp_ptp_event_dport;
	return ret;
}

sw_error_t
qca808x_ptp_rx_com_ts_status_reg_udp_ptp_event_dport_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_rx_com_ts_status_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rx_com_ts_status_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.udp_ptp_event_dport = value;
	ret = qca808x_ptp_rx_com_ts_status_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_rx_com_ts_status_reg_ipv6_da_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_rx_com_ts_status_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rx_com_ts_status_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.ipv6_da;
	return ret;
}

sw_error_t
qca808x_ptp_rx_com_ts_status_reg_ipv6_da_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_rx_com_ts_status_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rx_com_ts_status_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ipv6_da = value;
	ret = qca808x_ptp_rx_com_ts_status_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_rx_com_ts_status_reg_ipv4_ptp_prim_addr_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_rx_com_ts_status_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rx_com_ts_status_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.ipv4_ptp_prim_addr;
	return ret;
}

sw_error_t
qca808x_ptp_rx_com_ts_status_reg_ipv4_ptp_prim_addr_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_rx_com_ts_status_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rx_com_ts_status_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ipv4_ptp_prim_addr = value;
	ret = qca808x_ptp_rx_com_ts_status_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_rx_com_ts_status_reg_ipv4_da_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_rx_com_ts_status_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rx_com_ts_status_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.ipv4_da;
	return ret;
}

sw_error_t
qca808x_ptp_rx_com_ts_status_reg_ipv4_da_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_rx_com_ts_status_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rx_com_ts_status_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ipv4_da = value;
	ret = qca808x_ptp_rx_com_ts_status_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_rx_com_ts_status_reg_ipv6_ptp_pdelay_addr_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_rx_com_ts_status_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rx_com_ts_status_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.ipv6_ptp_pdelay_addr;
	return ret;
}

sw_error_t
qca808x_ptp_rx_com_ts_status_reg_ipv6_ptp_pdelay_addr_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_rx_com_ts_status_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rx_com_ts_status_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ipv6_ptp_pdelay_addr = value;
	ret = qca808x_ptp_rx_com_ts_status_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_rx_com_ts_status_reg_ipv6_ptp_prim_addr_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_rx_com_ts_status_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rx_com_ts_status_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.ipv6_ptp_prim_addr;
	return ret;
}

sw_error_t
qca808x_ptp_rx_com_ts_status_reg_ipv6_ptp_prim_addr_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_rx_com_ts_status_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rx_com_ts_status_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ipv6_ptp_prim_addr = value;
	ret = qca808x_ptp_rx_com_ts_status_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_rx_com_ts_status_reg_ipv6_next_header_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_rx_com_ts_status_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rx_com_ts_status_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.ipv6_next_header;
	return ret;
}

sw_error_t
qca808x_ptp_rx_com_ts_status_reg_ipv6_next_header_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_rx_com_ts_status_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rx_com_ts_status_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ipv6_next_header = value;
	ret = qca808x_ptp_rx_com_ts_status_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_rx_com_ts_status_reg_y1731_mach_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_rx_com_ts_status_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rx_com_ts_status_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.y1731_mach;
	return ret;
}

sw_error_t
qca808x_ptp_rx_com_ts_status_reg_y1731_mach_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_rx_com_ts_status_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rx_com_ts_status_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.y1731_mach = value;
	ret = qca808x_ptp_rx_com_ts_status_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_rx_com_ts_status_reg_mac_ptp_prim_addr_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_rx_com_ts_status_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rx_com_ts_status_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.mac_ptp_prim_addr;
	return ret;
}

sw_error_t
qca808x_ptp_rx_com_ts_status_reg_mac_ptp_prim_addr_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_rx_com_ts_status_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rx_com_ts_status_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.mac_ptp_prim_addr = value;
	ret = qca808x_ptp_rx_com_ts_status_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_rx_com_timestamp0_reg_com_ts_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_rx_com_timestamp0_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rx_com_timestamp0_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.com_ts;
	return ret;
}

sw_error_t
qca808x_ptp_rx_com_timestamp0_reg_com_ts_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_rx_com_timestamp0_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rx_com_timestamp0_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.com_ts = value;
	ret = qca808x_ptp_rx_com_timestamp0_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_rx_com_timestamp1_reg_com_ts_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_rx_com_timestamp1_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rx_com_timestamp1_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.com_ts;
	return ret;
}

sw_error_t
qca808x_ptp_rx_com_timestamp1_reg_com_ts_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_rx_com_timestamp1_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rx_com_timestamp1_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.com_ts = value;
	ret = qca808x_ptp_rx_com_timestamp1_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_rx_com_timestamp2_reg_com_ts_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_rx_com_timestamp2_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rx_com_timestamp2_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.com_ts;
	return ret;
}

sw_error_t
qca808x_ptp_rx_com_timestamp2_reg_com_ts_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_rx_com_timestamp2_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rx_com_timestamp2_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.com_ts = value;
	ret = qca808x_ptp_rx_com_timestamp2_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_rx_com_timestamp3_reg_com_ts_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_rx_com_timestamp3_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rx_com_timestamp3_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.com_ts;
	return ret;
}

sw_error_t
qca808x_ptp_rx_com_timestamp3_reg_com_ts_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_rx_com_timestamp3_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rx_com_timestamp3_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.com_ts = value;
	ret = qca808x_ptp_rx_com_timestamp3_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_rx_com_timestamp4_reg_com_ts_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_rx_com_timestamp4_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rx_com_timestamp4_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.com_ts;
	return ret;
}

sw_error_t
qca808x_ptp_rx_com_timestamp4_reg_com_ts_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_rx_com_timestamp4_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rx_com_timestamp4_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.com_ts = value;
	ret = qca808x_ptp_rx_com_timestamp4_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_rx_com_frac_nano_reg_frac_nano_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_rx_com_frac_nano_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rx_com_frac_nano_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.frac_nano;
	return ret;
}

sw_error_t
qca808x_ptp_rx_com_frac_nano_reg_frac_nano_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_rx_com_frac_nano_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rx_com_frac_nano_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.frac_nano = value;
	ret = qca808x_ptp_rx_com_frac_nano_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_rx_com_timestamp_pre0_reg_com_ts_pre_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_rx_com_timestamp_pre0_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rx_com_timestamp_pre0_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.com_ts_pre;
	return ret;
}

sw_error_t
qca808x_ptp_rx_com_timestamp_pre0_reg_com_ts_pre_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_rx_com_timestamp_pre0_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rx_com_timestamp_pre0_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.com_ts_pre = value;
	ret = qca808x_ptp_rx_com_timestamp_pre0_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_rx_com_timestamp_pre1_reg_com_ts_pre_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_rx_com_timestamp_pre1_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rx_com_timestamp_pre1_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.com_ts_pre;
	return ret;
}

sw_error_t
qca808x_ptp_rx_com_timestamp_pre1_reg_com_ts_pre_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_rx_com_timestamp_pre1_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rx_com_timestamp_pre1_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.com_ts_pre = value;
	ret = qca808x_ptp_rx_com_timestamp_pre1_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_rx_com_timestamp_pre2_reg_com_ts_pre_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_rx_com_timestamp_pre2_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rx_com_timestamp_pre2_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.com_ts_pre;
	return ret;
}

sw_error_t
qca808x_ptp_rx_com_timestamp_pre2_reg_com_ts_pre_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_rx_com_timestamp_pre2_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rx_com_timestamp_pre2_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.com_ts_pre = value;
	ret = qca808x_ptp_rx_com_timestamp_pre2_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_rx_com_timestamp_pre3_reg_com_ts_pre_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_rx_com_timestamp_pre3_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rx_com_timestamp_pre3_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.com_ts_pre;
	return ret;
}

sw_error_t
qca808x_ptp_rx_com_timestamp_pre3_reg_com_ts_pre_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_rx_com_timestamp_pre3_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rx_com_timestamp_pre3_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.com_ts_pre = value;
	ret = qca808x_ptp_rx_com_timestamp_pre3_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_rx_com_timestamp_pre4_reg_com_ts_pre_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_rx_com_timestamp_pre4_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rx_com_timestamp_pre4_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.com_ts_pre;
	return ret;
}

sw_error_t
qca808x_ptp_rx_com_timestamp_pre4_reg_com_ts_pre_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_rx_com_timestamp_pre4_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rx_com_timestamp_pre4_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.com_ts_pre = value;
	ret = qca808x_ptp_rx_com_timestamp_pre4_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_rx_com_frac_nano_pre_reg_frac_nano_pre_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_rx_com_frac_nano_pre_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rx_com_frac_nano_pre_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.frac_nano_pre;
	return ret;
}

sw_error_t
qca808x_ptp_rx_com_frac_nano_pre_reg_frac_nano_pre_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_rx_com_frac_nano_pre_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rx_com_frac_nano_pre_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.frac_nano_pre = value;
	ret = qca808x_ptp_rx_com_frac_nano_pre_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_rx_y1731_identify_reg_identify_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_rx_y1731_identify_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rx_y1731_identify_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.identify;
	return ret;
}

sw_error_t
qca808x_ptp_rx_y1731_identify_reg_identify_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_rx_y1731_identify_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rx_y1731_identify_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.identify = value;
	ret = qca808x_ptp_rx_y1731_identify_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_rx_y1731_identify_pre_reg_identify_pre_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_rx_y1731_identify_pre_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rx_y1731_identify_pre_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.identify_pre;
	return ret;
}

sw_error_t
qca808x_ptp_rx_y1731_identify_pre_reg_identify_pre_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_rx_y1731_identify_pre_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rx_y1731_identify_pre_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.identify_pre = value;
	ret = qca808x_ptp_rx_y1731_identify_pre_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_tx_com_ts_ctrl_reg_ipv4_da_en_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_tx_com_ts_ctrl_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_tx_com_ts_ctrl_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.ipv4_da_en;
	return ret;
}

sw_error_t
qca808x_ptp_tx_com_ts_ctrl_reg_ipv4_da_en_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_tx_com_ts_ctrl_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_tx_com_ts_ctrl_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ipv4_da_en = value;
	ret = qca808x_ptp_tx_com_ts_ctrl_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_tx_com_ts_ctrl_reg_mac_lengthtype_en_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_tx_com_ts_ctrl_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_tx_com_ts_ctrl_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.mac_lengthtype_en;
	return ret;
}

sw_error_t
qca808x_ptp_tx_com_ts_ctrl_reg_mac_lengthtype_en_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_tx_com_ts_ctrl_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_tx_com_ts_ctrl_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.mac_lengthtype_en = value;
	ret = qca808x_ptp_tx_com_ts_ctrl_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_tx_com_ts_ctrl_reg_ipv6_da_en_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_tx_com_ts_ctrl_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_tx_com_ts_ctrl_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.ipv6_da_en;
	return ret;
}

sw_error_t
qca808x_ptp_tx_com_ts_ctrl_reg_ipv6_da_en_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_tx_com_ts_ctrl_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_tx_com_ts_ctrl_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ipv6_da_en = value;
	ret = qca808x_ptp_tx_com_ts_ctrl_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_tx_com_ts_ctrl_reg_pw_mac_en_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_tx_com_ts_ctrl_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_tx_com_ts_ctrl_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.pw_mac_en;
	return ret;
}

sw_error_t
qca808x_ptp_tx_com_ts_ctrl_reg_pw_mac_en_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_tx_com_ts_ctrl_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_tx_com_ts_ctrl_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.pw_mac_en = value;
	ret = qca808x_ptp_tx_com_ts_ctrl_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_tx_com_ts_ctrl_reg_udp_ptp_event_filt_en_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_tx_com_ts_ctrl_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_tx_com_ts_ctrl_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.udp_ptp_event_filt_en;
	return ret;
}

sw_error_t
qca808x_ptp_tx_com_ts_ctrl_reg_udp_ptp_event_filt_en_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_tx_com_ts_ctrl_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_tx_com_ts_ctrl_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.udp_ptp_event_filt_en = value;
	ret = qca808x_ptp_tx_com_ts_ctrl_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_tx_com_ts_ctrl_reg_udp_dport_en_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_tx_com_ts_ctrl_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_tx_com_ts_ctrl_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.udp_dport_en;
	return ret;
}

sw_error_t
qca808x_ptp_tx_com_ts_ctrl_reg_udp_dport_en_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_tx_com_ts_ctrl_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_tx_com_ts_ctrl_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.udp_dport_en = value;
	ret = qca808x_ptp_tx_com_ts_ctrl_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_tx_com_ts_ctrl_reg_mac_ptp_filt_en_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_tx_com_ts_ctrl_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_tx_com_ts_ctrl_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.mac_ptp_filt_en;
	return ret;
}

sw_error_t
qca808x_ptp_tx_com_ts_ctrl_reg_mac_ptp_filt_en_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_tx_com_ts_ctrl_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_tx_com_ts_ctrl_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.mac_ptp_filt_en = value;
	ret = qca808x_ptp_tx_com_ts_ctrl_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_tx_com_ts_ctrl_reg_ipv6_next_header_en_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_tx_com_ts_ctrl_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_tx_com_ts_ctrl_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.ipv6_next_header_en;
	return ret;
}

sw_error_t
qca808x_ptp_tx_com_ts_ctrl_reg_ipv6_next_header_en_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_tx_com_ts_ctrl_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_tx_com_ts_ctrl_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ipv6_next_header_en = value;
	ret = qca808x_ptp_tx_com_ts_ctrl_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_tx_com_ts_ctrl_reg_ipv4_layer4_protocol_en_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_tx_com_ts_ctrl_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_tx_com_ts_ctrl_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.ipv4_layer4_protocol_en;
	return ret;
}

sw_error_t
qca808x_ptp_tx_com_ts_ctrl_reg_ipv4_layer4_protocol_en_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_tx_com_ts_ctrl_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_tx_com_ts_ctrl_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ipv4_layer4_protocol_en = value;
	ret = qca808x_ptp_tx_com_ts_ctrl_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_tx_com_ts_ctrl_reg_y1731_en_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_tx_com_ts_ctrl_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_tx_com_ts_ctrl_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.y1731_en;
	return ret;
}

sw_error_t
qca808x_ptp_tx_com_ts_ctrl_reg_y1731_en_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_tx_com_ts_ctrl_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_tx_com_ts_ctrl_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.y1731_en = value;
	ret = qca808x_ptp_tx_com_ts_ctrl_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_tx_com_ts_ctrl_reg_filt_en_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_tx_com_ts_ctrl_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_tx_com_ts_ctrl_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.filt_en;
	return ret;
}

sw_error_t
qca808x_ptp_tx_com_ts_ctrl_reg_filt_en_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_tx_com_ts_ctrl_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_tx_com_ts_ctrl_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.filt_en = value;
	ret = qca808x_ptp_tx_com_ts_ctrl_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_tx_com_ts_ctrl_reg_ipv6_ptp_filt_en_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_tx_com_ts_ctrl_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_tx_com_ts_ctrl_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.ipv6_ptp_filt_en;
	return ret;
}

sw_error_t
qca808x_ptp_tx_com_ts_ctrl_reg_ipv6_ptp_filt_en_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_tx_com_ts_ctrl_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_tx_com_ts_ctrl_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ipv6_ptp_filt_en = value;
	ret = qca808x_ptp_tx_com_ts_ctrl_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_tx_com_ts_ctrl_reg_ipv4_ptp_filt_en_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_tx_com_ts_ctrl_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_tx_com_ts_ctrl_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.ipv4_ptp_filt_en;
	return ret;
}

sw_error_t
qca808x_ptp_tx_com_ts_ctrl_reg_ipv4_ptp_filt_en_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_tx_com_ts_ctrl_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_tx_com_ts_ctrl_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ipv4_ptp_filt_en = value;
	ret = qca808x_ptp_tx_com_ts_ctrl_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_tx_com_ts_ctrl_reg_y1731_insert_ts_en_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_tx_com_ts_ctrl_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_tx_com_ts_ctrl_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.y1731_insert_ts_en;
	return ret;
}

sw_error_t
qca808x_ptp_tx_com_ts_ctrl_reg_y1731_insert_ts_en_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_tx_com_ts_ctrl_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_tx_com_ts_ctrl_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.y1731_insert_ts_en = value;
	ret = qca808x_ptp_tx_com_ts_ctrl_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_tx_com_ts_ctrl_reg_mac_da_en_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_tx_com_ts_ctrl_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_tx_com_ts_ctrl_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.mac_da_en;
	return ret;
}

sw_error_t
qca808x_ptp_tx_com_ts_ctrl_reg_mac_da_en_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_tx_com_ts_ctrl_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_tx_com_ts_ctrl_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.mac_da_en = value;
	ret = qca808x_ptp_tx_com_ts_ctrl_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_tx_com_ts_ctrl_reg_y1731_sa_chk_en_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_tx_com_ts_ctrl_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_tx_com_ts_ctrl_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.y1731_sa_chk_en;
	return ret;
}

sw_error_t
qca808x_ptp_tx_com_ts_ctrl_reg_y1731_sa_chk_en_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_tx_com_ts_ctrl_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_tx_com_ts_ctrl_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.y1731_sa_chk_en = value;
	ret = qca808x_ptp_tx_com_ts_ctrl_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_tx_filt_mac_da0_reg_mac_addr_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_tx_filt_mac_da0_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_tx_filt_mac_da0_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.mac_addr;
	return ret;
}

sw_error_t
qca808x_ptp_tx_filt_mac_da0_reg_mac_addr_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_tx_filt_mac_da0_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_tx_filt_mac_da0_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.mac_addr = value;
	ret = qca808x_ptp_tx_filt_mac_da0_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_tx_filt_mac_da1_reg_mac_addr_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_tx_filt_mac_da1_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_tx_filt_mac_da1_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.mac_addr;
	return ret;
}

sw_error_t
qca808x_ptp_tx_filt_mac_da1_reg_mac_addr_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_tx_filt_mac_da1_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_tx_filt_mac_da1_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.mac_addr = value;
	ret = qca808x_ptp_tx_filt_mac_da1_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_tx_filt_mac_da2_reg_mac_addr_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_tx_filt_mac_da2_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_tx_filt_mac_da2_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.mac_addr;
	return ret;
}

sw_error_t
qca808x_ptp_tx_filt_mac_da2_reg_mac_addr_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_tx_filt_mac_da2_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_tx_filt_mac_da2_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.mac_addr = value;
	ret = qca808x_ptp_tx_filt_mac_da2_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_tx_filt_ipv4_da0_reg_ip_addr_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_tx_filt_ipv4_da0_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_tx_filt_ipv4_da0_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.ip_addr;
	return ret;
}

sw_error_t
qca808x_ptp_tx_filt_ipv4_da0_reg_ip_addr_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_tx_filt_ipv4_da0_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_tx_filt_ipv4_da0_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ip_addr = value;
	ret = qca808x_ptp_tx_filt_ipv4_da0_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_tx_filt_ipv4_da1_reg_ip_addr_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_tx_filt_ipv4_da1_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_tx_filt_ipv4_da1_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.ip_addr;
	return ret;
}

sw_error_t
qca808x_ptp_tx_filt_ipv4_da1_reg_ip_addr_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_tx_filt_ipv4_da1_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_tx_filt_ipv4_da1_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ip_addr = value;
	ret = qca808x_ptp_tx_filt_ipv4_da1_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_tx_filt_ipv6_da0_reg_ip_addr_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_tx_filt_ipv6_da0_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_tx_filt_ipv6_da0_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.ip_addr;
	return ret;
}

sw_error_t
qca808x_ptp_tx_filt_ipv6_da0_reg_ip_addr_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_tx_filt_ipv6_da0_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_tx_filt_ipv6_da0_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ip_addr = value;
	ret = qca808x_ptp_tx_filt_ipv6_da0_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_tx_filt_ipv6_da1_reg_ip_addr_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_tx_filt_ipv6_da1_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_tx_filt_ipv6_da1_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.ip_addr;
	return ret;
}

sw_error_t
qca808x_ptp_tx_filt_ipv6_da1_reg_ip_addr_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_tx_filt_ipv6_da1_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_tx_filt_ipv6_da1_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ip_addr = value;
	ret = qca808x_ptp_tx_filt_ipv6_da1_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_tx_filt_ipv6_da2_reg_ip_addr_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_tx_filt_ipv6_da2_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_tx_filt_ipv6_da2_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.ip_addr;
	return ret;
}

sw_error_t
qca808x_ptp_tx_filt_ipv6_da2_reg_ip_addr_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_tx_filt_ipv6_da2_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_tx_filt_ipv6_da2_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ip_addr = value;
	ret = qca808x_ptp_tx_filt_ipv6_da2_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_tx_filt_ipv6_da3_reg_ip_addr_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_tx_filt_ipv6_da3_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_tx_filt_ipv6_da3_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.ip_addr;
	return ret;
}

sw_error_t
qca808x_ptp_tx_filt_ipv6_da3_reg_ip_addr_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_tx_filt_ipv6_da3_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_tx_filt_ipv6_da3_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ip_addr = value;
	ret = qca808x_ptp_tx_filt_ipv6_da3_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_tx_filt_ipv6_da4_reg_ip_addr_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_tx_filt_ipv6_da4_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_tx_filt_ipv6_da4_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.ip_addr;
	return ret;
}

sw_error_t
qca808x_ptp_tx_filt_ipv6_da4_reg_ip_addr_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_tx_filt_ipv6_da4_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_tx_filt_ipv6_da4_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ip_addr = value;
	ret = qca808x_ptp_tx_filt_ipv6_da4_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_tx_filt_ipv6_da5_reg_ip_addr_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_tx_filt_ipv6_da5_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_tx_filt_ipv6_da5_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.ip_addr;
	return ret;
}

sw_error_t
qca808x_ptp_tx_filt_ipv6_da5_reg_ip_addr_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_tx_filt_ipv6_da5_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_tx_filt_ipv6_da5_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ip_addr = value;
	ret = qca808x_ptp_tx_filt_ipv6_da5_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_tx_filt_ipv6_da6_reg_ip_addr_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_tx_filt_ipv6_da6_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_tx_filt_ipv6_da6_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.ip_addr;
	return ret;
}

sw_error_t
qca808x_ptp_tx_filt_ipv6_da6_reg_ip_addr_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_tx_filt_ipv6_da6_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_tx_filt_ipv6_da6_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ip_addr = value;
	ret = qca808x_ptp_tx_filt_ipv6_da6_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_tx_filt_ipv6_da7_reg_ip_addr_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_tx_filt_ipv6_da7_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_tx_filt_ipv6_da7_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.ip_addr;
	return ret;
}

sw_error_t
qca808x_ptp_tx_filt_ipv6_da7_reg_ip_addr_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_tx_filt_ipv6_da7_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_tx_filt_ipv6_da7_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ip_addr = value;
	ret = qca808x_ptp_tx_filt_ipv6_da7_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_tx_filt_mac_lengthtype_reg_length_type_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_tx_filt_mac_lengthtype_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_tx_filt_mac_lengthtype_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.length_type;
	return ret;
}

sw_error_t
qca808x_ptp_tx_filt_mac_lengthtype_reg_length_type_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_tx_filt_mac_lengthtype_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_tx_filt_mac_lengthtype_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.length_type = value;
	ret = qca808x_ptp_tx_filt_mac_lengthtype_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_tx_filt_layer4_protocol_reg_l4_protocol_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_tx_filt_layer4_protocol_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_tx_filt_layer4_protocol_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.l4_protocol;
	return ret;
}

sw_error_t
qca808x_ptp_tx_filt_layer4_protocol_reg_l4_protocol_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_tx_filt_layer4_protocol_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_tx_filt_layer4_protocol_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.l4_protocol = value;
	ret = qca808x_ptp_tx_filt_layer4_protocol_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_tx_filt_udp_port_reg_udp_port_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_tx_filt_udp_port_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_tx_filt_udp_port_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.udp_port;
	return ret;
}

sw_error_t
qca808x_ptp_tx_filt_udp_port_reg_udp_port_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_tx_filt_udp_port_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_tx_filt_udp_port_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.udp_port = value;
	ret = qca808x_ptp_tx_filt_udp_port_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_tx_com_ts_status_reg_mac_ptp_pdelay_addr_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_tx_com_ts_status_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_tx_com_ts_status_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.mac_ptp_pdelay_addr;
	return ret;
}

sw_error_t
qca808x_ptp_tx_com_ts_status_reg_mac_ptp_pdelay_addr_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_tx_com_ts_status_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_tx_com_ts_status_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.mac_ptp_pdelay_addr = value;
	ret = qca808x_ptp_tx_com_ts_status_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_tx_com_ts_status_reg_mac_da_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_tx_com_ts_status_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_tx_com_ts_status_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.mac_da;
	return ret;
}

sw_error_t
qca808x_ptp_tx_com_ts_status_reg_mac_da_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_tx_com_ts_status_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_tx_com_ts_status_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.mac_da = value;
	ret = qca808x_ptp_tx_com_ts_status_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_tx_com_ts_status_reg_ipv4_ptp_pdelay_addr_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_tx_com_ts_status_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_tx_com_ts_status_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.ipv4_ptp_pdelay_addr;
	return ret;
}

sw_error_t
qca808x_ptp_tx_com_ts_status_reg_ipv4_ptp_pdelay_addr_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_tx_com_ts_status_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_tx_com_ts_status_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ipv4_ptp_pdelay_addr = value;
	ret = qca808x_ptp_tx_com_ts_status_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_tx_com_ts_status_reg_mac_lengthtype_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_tx_com_ts_status_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_tx_com_ts_status_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.mac_lengthtype;
	return ret;
}

sw_error_t
qca808x_ptp_tx_com_ts_status_reg_mac_lengthtype_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_tx_com_ts_status_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_tx_com_ts_status_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.mac_lengthtype = value;
	ret = qca808x_ptp_tx_com_ts_status_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_tx_com_ts_status_reg_udp_dport_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_tx_com_ts_status_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_tx_com_ts_status_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.udp_dport;
	return ret;
}

sw_error_t
qca808x_ptp_tx_com_ts_status_reg_udp_dport_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_tx_com_ts_status_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_tx_com_ts_status_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.udp_dport = value;
	ret = qca808x_ptp_tx_com_ts_status_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_tx_com_ts_status_reg_ipv4_layer4_protocol_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_tx_com_ts_status_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_tx_com_ts_status_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.ipv4_layer4_protocol;
	return ret;
}

sw_error_t
qca808x_ptp_tx_com_ts_status_reg_ipv4_layer4_protocol_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_tx_com_ts_status_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_tx_com_ts_status_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ipv4_layer4_protocol = value;
	ret = qca808x_ptp_tx_com_ts_status_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_tx_com_ts_status_reg_udp_ptp_event_dport_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_tx_com_ts_status_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_tx_com_ts_status_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.udp_ptp_event_dport;
	return ret;
}

sw_error_t
qca808x_ptp_tx_com_ts_status_reg_udp_ptp_event_dport_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_tx_com_ts_status_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_tx_com_ts_status_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.udp_ptp_event_dport = value;
	ret = qca808x_ptp_tx_com_ts_status_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_tx_com_ts_status_reg_ipv6_da_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_tx_com_ts_status_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_tx_com_ts_status_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.ipv6_da;
	return ret;
}

sw_error_t
qca808x_ptp_tx_com_ts_status_reg_ipv6_da_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_tx_com_ts_status_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_tx_com_ts_status_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ipv6_da = value;
	ret = qca808x_ptp_tx_com_ts_status_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_tx_com_ts_status_reg_ipv4_ptp_prim_addr_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_tx_com_ts_status_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_tx_com_ts_status_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.ipv4_ptp_prim_addr;
	return ret;
}

sw_error_t
qca808x_ptp_tx_com_ts_status_reg_ipv4_ptp_prim_addr_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_tx_com_ts_status_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_tx_com_ts_status_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ipv4_ptp_prim_addr = value;
	ret = qca808x_ptp_tx_com_ts_status_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_tx_com_ts_status_reg_ipv4_da_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_tx_com_ts_status_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_tx_com_ts_status_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.ipv4_da;
	return ret;
}

sw_error_t
qca808x_ptp_tx_com_ts_status_reg_ipv4_da_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_tx_com_ts_status_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_tx_com_ts_status_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ipv4_da = value;
	ret = qca808x_ptp_tx_com_ts_status_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_tx_com_ts_status_reg_ipv6_ptp_pdelay_addr_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_tx_com_ts_status_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_tx_com_ts_status_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.ipv6_ptp_pdelay_addr;
	return ret;
}

sw_error_t
qca808x_ptp_tx_com_ts_status_reg_ipv6_ptp_pdelay_addr_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_tx_com_ts_status_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_tx_com_ts_status_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ipv6_ptp_pdelay_addr = value;
	ret = qca808x_ptp_tx_com_ts_status_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_tx_com_ts_status_reg_ipv6_ptp_prim_addr_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_tx_com_ts_status_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_tx_com_ts_status_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.ipv6_ptp_prim_addr;
	return ret;
}

sw_error_t
qca808x_ptp_tx_com_ts_status_reg_ipv6_ptp_prim_addr_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_tx_com_ts_status_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_tx_com_ts_status_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ipv6_ptp_prim_addr = value;
	ret = qca808x_ptp_tx_com_ts_status_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_tx_com_ts_status_reg_ipv6_next_header_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_tx_com_ts_status_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_tx_com_ts_status_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.ipv6_next_header;
	return ret;
}

sw_error_t
qca808x_ptp_tx_com_ts_status_reg_ipv6_next_header_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_tx_com_ts_status_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_tx_com_ts_status_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.ipv6_next_header = value;
	ret = qca808x_ptp_tx_com_ts_status_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_tx_com_ts_status_reg_y1731_mach_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_tx_com_ts_status_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_tx_com_ts_status_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.y1731_mach;
	return ret;
}

sw_error_t
qca808x_ptp_tx_com_ts_status_reg_y1731_mach_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_tx_com_ts_status_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_tx_com_ts_status_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.y1731_mach = value;
	ret = qca808x_ptp_tx_com_ts_status_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_tx_com_ts_status_reg_mac_ptp_prim_addr_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_tx_com_ts_status_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_tx_com_ts_status_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.mac_ptp_prim_addr;
	return ret;
}

sw_error_t
qca808x_ptp_tx_com_ts_status_reg_mac_ptp_prim_addr_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_tx_com_ts_status_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_tx_com_ts_status_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.mac_ptp_prim_addr = value;
	ret = qca808x_ptp_tx_com_ts_status_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_tx_com_timestamp0_reg_com_ts_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_tx_com_timestamp0_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_tx_com_timestamp0_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.com_ts;
	return ret;
}

sw_error_t
qca808x_ptp_tx_com_timestamp0_reg_com_ts_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_tx_com_timestamp0_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_tx_com_timestamp0_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.com_ts = value;
	ret = qca808x_ptp_tx_com_timestamp0_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_tx_com_timestamp1_reg_com_ts_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_tx_com_timestamp1_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_tx_com_timestamp1_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.com_ts;
	return ret;
}

sw_error_t
qca808x_ptp_tx_com_timestamp1_reg_com_ts_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_tx_com_timestamp1_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_tx_com_timestamp1_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.com_ts = value;
	ret = qca808x_ptp_tx_com_timestamp1_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_tx_com_timestamp2_reg_com_ts_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_tx_com_timestamp2_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_tx_com_timestamp2_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.com_ts;
	return ret;
}

sw_error_t
qca808x_ptp_tx_com_timestamp2_reg_com_ts_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_tx_com_timestamp2_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_tx_com_timestamp2_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.com_ts = value;
	ret = qca808x_ptp_tx_com_timestamp2_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_tx_com_timestamp3_reg_com_ts_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_tx_com_timestamp3_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_tx_com_timestamp3_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.com_ts;
	return ret;
}

sw_error_t
qca808x_ptp_tx_com_timestamp3_reg_com_ts_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_tx_com_timestamp3_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_tx_com_timestamp3_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.com_ts = value;
	ret = qca808x_ptp_tx_com_timestamp3_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_tx_com_timestamp4_reg_com_ts_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_tx_com_timestamp4_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_tx_com_timestamp4_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.com_ts;
	return ret;
}

sw_error_t
qca808x_ptp_tx_com_timestamp4_reg_com_ts_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_tx_com_timestamp4_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_tx_com_timestamp4_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.com_ts = value;
	ret = qca808x_ptp_tx_com_timestamp4_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_tx_com_frac_nano_reg_frac_nano_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_tx_com_frac_nano_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_tx_com_frac_nano_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.frac_nano;
	return ret;
}

sw_error_t
qca808x_ptp_tx_com_frac_nano_reg_frac_nano_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_tx_com_frac_nano_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_tx_com_frac_nano_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.frac_nano = value;
	ret = qca808x_ptp_tx_com_frac_nano_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_tx_y1731_identify_reg_identify_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_tx_y1731_identify_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_tx_y1731_identify_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.identify;
	return ret;
}

sw_error_t
qca808x_ptp_tx_y1731_identify_reg_identify_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_tx_y1731_identify_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_tx_y1731_identify_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.identify = value;
	ret = qca808x_ptp_tx_y1731_identify_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_y1731_dm_control_reg_y1731_dmm_lpbk_en_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_y1731_dm_control_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_y1731_dm_control_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.y1731_dmm_lpbk_en;
	return ret;
}

sw_error_t
qca808x_ptp_y1731_dm_control_reg_y1731_dmm_lpbk_en_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_y1731_dm_control_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_y1731_dm_control_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.y1731_dmm_lpbk_en = value;
	ret = qca808x_ptp_y1731_dm_control_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_y1731_dm_control_reg_valid_msg_lev_bmp_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_y1731_dm_control_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_y1731_dm_control_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.valid_msg_lev_bmp;
	return ret;
}

sw_error_t
qca808x_ptp_y1731_dm_control_reg_valid_msg_lev_bmp_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_y1731_dm_control_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_y1731_dm_control_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.valid_msg_lev_bmp = value;
	ret = qca808x_ptp_y1731_dm_control_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_baud_config_reg_baud_rate_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_baud_config_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_baud_config_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.baud_rate;
	return ret;
}

sw_error_t
qca808x_ptp_baud_config_reg_baud_rate_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_baud_config_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_baud_config_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.baud_rate = value;
	ret = qca808x_ptp_baud_config_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_uart_configuration_reg_start_polarity_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		unsigned int *value)
{
	union ptp_uart_configuration_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_uart_configuration_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.start_polarity;
	return ret;
}

sw_error_t
qca808x_ptp_uart_configuration_reg_start_polarity_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_uart_configuration_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_uart_configuration_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.start_polarity = value;
	ret = qca808x_ptp_uart_configuration_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_uart_configuration_reg_msb_first_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		unsigned int *value)
{
	union ptp_uart_configuration_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_uart_configuration_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.msb_first;
	return ret;
}

sw_error_t
qca808x_ptp_uart_configuration_reg_msb_first_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_uart_configuration_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_uart_configuration_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.msb_first = value;
	ret = qca808x_ptp_uart_configuration_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_uart_configuration_reg_parity_en_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		unsigned int *value)
{
	union ptp_uart_configuration_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_uart_configuration_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.parity_en;
	return ret;
}

sw_error_t
qca808x_ptp_uart_configuration_reg_parity_en_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_uart_configuration_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_uart_configuration_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.parity_en = value;
	ret = qca808x_ptp_uart_configuration_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_uart_configuration_reg_auto_tod_out_en_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		unsigned int *value)
{
	union ptp_uart_configuration_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_uart_configuration_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.auto_tod_out_en;
	return ret;
}

sw_error_t
qca808x_ptp_uart_configuration_reg_auto_tod_out_en_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_uart_configuration_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_uart_configuration_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.auto_tod_out_en = value;
	ret = qca808x_ptp_uart_configuration_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_uart_configuration_reg_auto_tod_in_en_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		unsigned int *value)
{
	union ptp_uart_configuration_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_uart_configuration_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.auto_tod_in_en;
	return ret;
}

sw_error_t
qca808x_ptp_uart_configuration_reg_auto_tod_in_en_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_uart_configuration_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_uart_configuration_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.auto_tod_in_en = value;
	ret = qca808x_ptp_uart_configuration_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_reset_buffer_reg_reset_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_reset_buffer_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_reset_buffer_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.reset;
	return ret;
}

sw_error_t
qca808x_ptp_reset_buffer_reg_reset_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_reset_buffer_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_reset_buffer_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.reset = value;
	ret = qca808x_ptp_reset_buffer_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_buffer_status_reg_tx_buffer_almost_empty_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_buffer_status_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_buffer_status_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.tx_buffer_almost_empty;
	return ret;
}

sw_error_t
qca808x_ptp_buffer_status_reg_tx_buffer_almost_empty_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_buffer_status_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_buffer_status_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.tx_buffer_almost_empty = value;
	ret = qca808x_ptp_buffer_status_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_buffer_status_reg_tx_buffer_almost_full_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_buffer_status_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_buffer_status_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.tx_buffer_almost_full;
	return ret;
}

sw_error_t
qca808x_ptp_buffer_status_reg_tx_buffer_almost_full_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_buffer_status_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_buffer_status_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.tx_buffer_almost_full = value;
	ret = qca808x_ptp_buffer_status_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_buffer_status_reg_tx_buffer_half_full_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_buffer_status_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_buffer_status_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.tx_buffer_half_full;
	return ret;
}

sw_error_t
qca808x_ptp_buffer_status_reg_tx_buffer_half_full_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_buffer_status_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_buffer_status_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.tx_buffer_half_full = value;
	ret = qca808x_ptp_buffer_status_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_buffer_status_reg_tx_buffer_full_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_buffer_status_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_buffer_status_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.tx_buffer_full;
	return ret;
}

sw_error_t
qca808x_ptp_buffer_status_reg_tx_buffer_full_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_buffer_status_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_buffer_status_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.tx_buffer_full = value;
	ret = qca808x_ptp_buffer_status_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_buffer_status_reg_rx_buffer_almost_empty_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_buffer_status_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_buffer_status_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.rx_buffer_almost_empty;
	return ret;
}

sw_error_t
qca808x_ptp_buffer_status_reg_rx_buffer_almost_empty_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_buffer_status_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_buffer_status_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.rx_buffer_almost_empty = value;
	ret = qca808x_ptp_buffer_status_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_buffer_status_reg_rx_buffer_almost_full_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_buffer_status_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_buffer_status_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.rx_buffer_almost_full;
	return ret;
}

sw_error_t
qca808x_ptp_buffer_status_reg_rx_buffer_almost_full_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_buffer_status_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_buffer_status_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.rx_buffer_almost_full = value;
	ret = qca808x_ptp_buffer_status_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_buffer_status_reg_rx_buffer_half_full_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_buffer_status_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_buffer_status_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.rx_buffer_half_full;
	return ret;
}

sw_error_t
qca808x_ptp_buffer_status_reg_rx_buffer_half_full_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_buffer_status_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_buffer_status_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.rx_buffer_half_full = value;
	ret = qca808x_ptp_buffer_status_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_buffer_status_reg_rx_buffer_full_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_buffer_status_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_buffer_status_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.rx_buffer_full;
	return ret;
}

sw_error_t
qca808x_ptp_buffer_status_reg_rx_buffer_full_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_buffer_status_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_buffer_status_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.rx_buffer_full = value;
	ret = qca808x_ptp_buffer_status_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_buffer_status_reg_rx_buffer_data_present_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_buffer_status_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_buffer_status_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.rx_buffer_data_present;
	return ret;
}

sw_error_t
qca808x_ptp_buffer_status_reg_rx_buffer_data_present_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_buffer_status_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_buffer_status_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.rx_buffer_data_present = value;
	ret = qca808x_ptp_buffer_status_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_tx_buffer_write_reg_tx_buffer_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_tx_buffer_write_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_tx_buffer_write_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.tx_buffer;
	return ret;
}

sw_error_t
qca808x_ptp_tx_buffer_write_reg_tx_buffer_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_tx_buffer_write_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_tx_buffer_write_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.tx_buffer = value;
	ret = qca808x_ptp_tx_buffer_write_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_rx_buffer_read_reg_rx_data_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_rx_buffer_read_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rx_buffer_read_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.rx_data;
	return ret;
}

sw_error_t
qca808x_ptp_rx_buffer_read_reg_rx_data_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_rx_buffer_read_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rx_buffer_read_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.rx_data = value;
	ret = qca808x_ptp_rx_buffer_read_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_rx_buffer_read_reg_rx_buffer_almost_empty_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_rx_buffer_read_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rx_buffer_read_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.rx_buffer_almost_empty;
	return ret;
}

sw_error_t
qca808x_ptp_rx_buffer_read_reg_rx_buffer_almost_empty_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_rx_buffer_read_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rx_buffer_read_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.rx_buffer_almost_empty = value;
	ret = qca808x_ptp_rx_buffer_read_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_rx_buffer_read_reg_rx_buffer_almost_full_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_rx_buffer_read_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rx_buffer_read_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.rx_buffer_almost_full;
	return ret;
}

sw_error_t
qca808x_ptp_rx_buffer_read_reg_rx_buffer_almost_full_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_rx_buffer_read_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rx_buffer_read_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.rx_buffer_almost_full = value;
	ret = qca808x_ptp_rx_buffer_read_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_rx_buffer_read_reg_rx_buffer_half_full_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_rx_buffer_read_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rx_buffer_read_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.rx_buffer_half_full;
	return ret;
}

sw_error_t
qca808x_ptp_rx_buffer_read_reg_rx_buffer_half_full_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_rx_buffer_read_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rx_buffer_read_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.rx_buffer_half_full = value;
	ret = qca808x_ptp_rx_buffer_read_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_rx_buffer_read_reg_rx_buffer_full_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_rx_buffer_read_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rx_buffer_read_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.rx_buffer_full;
	return ret;
}

sw_error_t
qca808x_ptp_rx_buffer_read_reg_rx_buffer_full_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_rx_buffer_read_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rx_buffer_read_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.rx_buffer_full = value;
	ret = qca808x_ptp_rx_buffer_read_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_rx_buffer_read_reg_rx_buffer_data_present_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_rx_buffer_read_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rx_buffer_read_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.rx_buffer_data_present;
	return ret;
}

sw_error_t
qca808x_ptp_rx_buffer_read_reg_rx_buffer_data_present_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_rx_buffer_read_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_rx_buffer_read_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.rx_buffer_data_present = value;
	ret = qca808x_ptp_rx_buffer_read_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

//
sw_error_t
qca808x_ptp_loc_mac_addr_0_reg_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_loc_mac_addr_0_reg_u *value)
{
	return qca808x_phy_ptp_mmd_read(
				dev_id,
				phy_id, QCA808X_PHY_MMD3_NUM, PTP_LOC_MAC_ADDR_0_REG_ADDRESS,
				&value->val);
}

sw_error_t
qca808x_ptp_loc_mac_addr_0_reg_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_loc_mac_addr_0_reg_u *value)
{
	return qca808x_phy_ptp_mmd_write(
				dev_id,
				phy_id, QCA808X_PHY_MMD3_NUM, PTP_LOC_MAC_ADDR_0_REG_ADDRESS,
				value->val);
}

sw_error_t
qca808x_ptp_loc_mac_addr_1_reg_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_loc_mac_addr_1_reg_u *value)
{
	return qca808x_phy_ptp_mmd_read(
				dev_id,
				phy_id, QCA808X_PHY_MMD3_NUM, PTP_LOC_MAC_ADDR_1_REG_ADDRESS,
				&value->val);
}

sw_error_t
qca808x_ptp_loc_mac_addr_1_reg_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_loc_mac_addr_1_reg_u *value)
{
	return qca808x_phy_ptp_mmd_write(
				dev_id,
				phy_id, QCA808X_PHY_MMD3_NUM, PTP_LOC_MAC_ADDR_1_REG_ADDRESS,
				value->val);
}

sw_error_t
qca808x_ptp_loc_mac_addr_2_reg_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_loc_mac_addr_2_reg_u *value)
{
	return qca808x_phy_ptp_mmd_read(
				dev_id,
				phy_id, QCA808X_PHY_MMD3_NUM, PTP_LOC_MAC_ADDR_2_REG_ADDRESS,
				&value->val);
}

sw_error_t
qca808x_ptp_loc_mac_addr_2_reg_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_loc_mac_addr_2_reg_u *value)
{
	return qca808x_phy_ptp_mmd_write(
				dev_id,
				phy_id, QCA808X_PHY_MMD3_NUM, PTP_LOC_MAC_ADDR_2_REG_ADDRESS,
				value->val);
}

sw_error_t
qca808x_ptp_loc_mac_addr_0_reg_mac_addr_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_loc_mac_addr_0_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_loc_mac_addr_0_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.mac_addr;
	return ret;
}

sw_error_t
qca808x_ptp_loc_mac_addr_0_reg_mac_addr_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_loc_mac_addr_0_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_loc_mac_addr_0_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.mac_addr = value;
	ret = qca808x_ptp_loc_mac_addr_0_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_loc_mac_addr_1_reg_mac_addr_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_loc_mac_addr_1_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_loc_mac_addr_1_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.mac_addr;
	return ret;
}

sw_error_t
qca808x_ptp_loc_mac_addr_1_reg_mac_addr_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_loc_mac_addr_1_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_loc_mac_addr_1_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.mac_addr = value;
	ret = qca808x_ptp_loc_mac_addr_1_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_loc_mac_addr_2_reg_mac_addr_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_loc_mac_addr_2_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_loc_mac_addr_2_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.mac_addr;
	return ret;
}

sw_error_t
qca808x_ptp_loc_mac_addr_2_reg_mac_addr_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_loc_mac_addr_2_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_loc_mac_addr_2_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.mac_addr = value;
	ret = qca808x_ptp_loc_mac_addr_2_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_link_delay_0_reg_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_link_delay_0_reg_u *value)
{
	return qca808x_phy_ptp_mmd_read(
				dev_id,
				phy_id, QCA808X_PHY_MMD3_NUM, PTP_LINK_DELAY_0_REG_ADDRESS,
				&value->val);
}

sw_error_t
qca808x_ptp_link_delay_0_reg_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_link_delay_0_reg_u *value)
{
	return qca808x_phy_ptp_mmd_write(
				dev_id,
				phy_id, QCA808X_PHY_MMD3_NUM, PTP_LINK_DELAY_0_REG_ADDRESS,
				value->val);
}

sw_error_t
qca808x_ptp_link_delay_1_reg_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_link_delay_1_reg_u *value)
{
	return qca808x_phy_ptp_mmd_read(
				dev_id,
				phy_id, QCA808X_PHY_MMD3_NUM, PTP_LINK_DELAY_1_REG_ADDRESS,
				&value->val);
}

sw_error_t
qca808x_ptp_link_delay_1_reg_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_link_delay_1_reg_u *value)
{
	return qca808x_phy_ptp_mmd_write(
				dev_id,
				phy_id, QCA808X_PHY_MMD3_NUM, PTP_LINK_DELAY_1_REG_ADDRESS,
				value->val);
}

sw_error_t
qca808x_ptp_link_delay_0_reg_link_delay_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_link_delay_0_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_link_delay_0_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.link_delay;
	return ret;
}

sw_error_t
qca808x_ptp_link_delay_0_reg_link_delay_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_link_delay_0_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_link_delay_0_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.link_delay = value;
	ret = qca808x_ptp_link_delay_0_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_link_delay_1_reg_link_delay_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t *value)
{
	union ptp_link_delay_1_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_link_delay_1_reg_get(dev_id, phy_id, &reg_val);
	*value = reg_val.bf.link_delay;
	return ret;
}

sw_error_t
qca808x_ptp_link_delay_1_reg_link_delay_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		a_uint32_t value)
{
	union ptp_link_delay_1_reg_u reg_val;
	sw_error_t ret = SW_OK;

	ret = qca808x_ptp_link_delay_1_reg_get(dev_id, phy_id, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.link_delay = value;
	ret = qca808x_ptp_link_delay_1_reg_set(dev_id, phy_id, &reg_val);
	return ret;
}

sw_error_t
qca808x_ptp_misc_control_reg_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_misc_control_reg_u *value)
{
	return qca808x_phy_ptp_mmd_read(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_MISC_CONTROL_REG_ADDRESS,
			&value->val);
}

sw_error_t
qca808x_ptp_misc_control_reg_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_misc_control_reg_u *value)
{
	return qca808x_phy_ptp_mmd_write(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_MISC_CONTROL_REG_ADDRESS,
			value->val);
}

sw_error_t
qca808x_ptp_ingress_asymmetry_0_reg_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_ingress_asymmetry_0_reg_u *value)
{
	return qca808x_phy_ptp_mmd_read(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_INGRESS_ASYMMETRY_0_REG_ADDRESS,
			&value->val);
}

sw_error_t
qca808x_ptp_ingress_asymmetry_0_reg_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_ingress_asymmetry_0_reg_u *value)
{
	return qca808x_phy_ptp_mmd_write(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_INGRESS_ASYMMETRY_0_REG_ADDRESS,
			value->val);
}

sw_error_t
qca808x_ptp_ingress_asymmetry_1_reg_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_ingress_asymmetry_1_reg_u *value)
{
	return qca808x_phy_ptp_mmd_read(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_INGRESS_ASYMMETRY_1_REG_ADDRESS,
			&value->val);
}

sw_error_t
qca808x_ptp_ingress_asymmetry_1_reg_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_ingress_asymmetry_1_reg_u *value)
{
	return qca808x_phy_ptp_mmd_write(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_INGRESS_ASYMMETRY_1_REG_ADDRESS,
			value->val);
}

sw_error_t
qca808x_ptp_egress_asymmetry_0_reg_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_egress_asymmetry_0_reg_u *value)
{
	return qca808x_phy_ptp_mmd_read(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_EGRESS_ASYMMETRY_0_REG_ADDRESS,
			&value->val);
}

sw_error_t
qca808x_ptp_egress_asymmetry_0_reg_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_egress_asymmetry_0_reg_u *value)
{
	return qca808x_phy_ptp_mmd_write(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_EGRESS_ASYMMETRY_0_REG_ADDRESS,
			value->val);
}

sw_error_t
qca808x_ptp_egress_asymmetry_1_reg_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_egress_asymmetry_1_reg_u *value)
{
	return qca808x_phy_ptp_mmd_read(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_EGRESS_ASYMMETRY_1_REG_ADDRESS,
			&value->val);
}

sw_error_t
qca808x_ptp_egress_asymmetry_1_reg_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_egress_asymmetry_1_reg_u *value)
{
	return qca808x_phy_ptp_mmd_write(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_EGRESS_ASYMMETRY_1_REG_ADDRESS,
			value->val);
}

sw_error_t
qca808x_ptp_backup_reg_get(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_backup_reg_u *value)
{
	return qca808x_phy_ptp_mmd_read(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_BACKUP_REG_ADDRESS,
			&value->val);
}

sw_error_t
qca808x_ptp_backup_reg_set(
		a_uint32_t dev_id, a_uint32_t phy_id,
		union ptp_backup_reg_u *value)
{
	return qca808x_phy_ptp_mmd_write(
			dev_id,
			phy_id, QCA808X_PHY_MMD3_NUM, PTP_BACKUP_REG_ADDRESS,
			value->val);
}


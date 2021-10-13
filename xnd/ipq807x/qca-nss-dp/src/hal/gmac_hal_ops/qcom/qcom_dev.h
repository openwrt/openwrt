/*
 **************************************************************************
 * Copyright (c) 2016-2017,2020 The Linux Foundation. All rights reserved.
 *
 * Permission to use, copy, modify, and/or distribute this software for
 * any purpose with or without fee is hereby granted, provided that the
 * above copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF0
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT
 * OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 **************************************************************************
 */

#ifndef __QCOM_DEV_H__
#define __QCOM_DEV_H__

#include <nss_dp_hal_if.h>
#include "qcom_reg.h"
#include <fal/fal_mib.h>
#include <fal/fal_port_ctrl.h>

/*
 * Subclass for base nss_gmac_haldev
 */
struct qcom_hal_dev {
	struct nss_gmac_hal_dev nghd;	/* Base class */
	fal_mib_counter_t stats;	/* Stats structure */
};
/*
 * qcom_set_rx_flow_ctrl()
 */
static inline void qcom_set_rx_flow_ctrl(struct nss_gmac_hal_dev *nghd)
{
	hal_set_reg_bits(nghd, QCOM_MAC_ENABLE, QCOM_RX_FLOW_ENABLE);
}

/*
 * qcom_clear_rx_flow_ctrl()
 */
static inline void qcom_clear_rx_flow_ctrl(struct nss_gmac_hal_dev *nghd)
{
	hal_clear_reg_bits(nghd, QCOM_MAC_ENABLE, QCOM_RX_FLOW_ENABLE);
}

/*
 * qcom_set_tx_flow_ctrl()
 */
static inline void qcom_set_tx_flow_ctrl(struct nss_gmac_hal_dev *nghd)
{
	hal_set_reg_bits(nghd, QCOM_MAC_ENABLE, QCOM_TX_FLOW_ENABLE);
}

/*
 * qcom_clear_tx_flow_ctrl()
 */
static inline void qcom_clear_tx_flow_ctrl(struct nss_gmac_hal_dev *nghd)
{
	hal_clear_reg_bits(nghd, QCOM_MAC_ENABLE, QCOM_TX_FLOW_ENABLE);
}

/*
 * qcom_clear_mac_ctrl0()
 */
static inline void qcom_clear_mac_ctrl0(struct nss_gmac_hal_dev *nghd)
{
	hal_write_reg(nghd->mac_base, QCOM_MAC_CTRL0, 0);
}

/*
 * qcom_rx_enable()
 */
static inline void qcom_rx_enable(struct nss_gmac_hal_dev *nghd)
{
	hal_set_reg_bits(nghd, QCOM_MAC_ENABLE, QCOM_RX_MAC_ENABLE);
}

/*
 * qcom_rx_disable()
 *	Disable the reception of frames on GMII/MII.
 *	GMAC receive state machine is disabled after completion of reception of
 *	current frame.
 */
static inline void qcom_rx_disable(struct nss_gmac_hal_dev *nghd)
{
	hal_clear_reg_bits(nghd, QCOM_MAC_ENABLE, QCOM_RX_MAC_ENABLE);
}

/*
 * qcom_tx_enable()
 */
static inline void qcom_tx_enable(struct nss_gmac_hal_dev *nghd)
{
	hal_set_reg_bits(nghd, QCOM_MAC_ENABLE, QCOM_TX_MAC_ENABLE);
}

/*
 * qcom_tx_disable()
 *	Disable the transmission of frames on GMII/MII.
 *	GMAC transmit state machine is disabled after completion of
 *	transmission of current frame.
 */
static inline void qcom_tx_disable(struct nss_gmac_hal_dev *nghd)
{
	hal_clear_reg_bits(nghd, QCOM_MAC_ENABLE, QCOM_TX_MAC_ENABLE);
}

/*
 * qcom_set_full_duplex()
 */
static inline void qcom_set_full_duplex(struct nss_gmac_hal_dev *nghd)
{
	hal_set_reg_bits(nghd, QCOM_MAC_ENABLE, QCOM_DUPLEX);
}

/*
 * qcom_set_half_duplex()
 */
static inline void qcom_set_half_duplex(struct nss_gmac_hal_dev *nghd)
{
	hal_clear_reg_bits(nghd, QCOM_MAC_ENABLE, QCOM_DUPLEX);
}

/*
 * qcom_set_ipgt()
 */
static inline void qcom_set_ipgt(struct nss_gmac_hal_dev *nghd, uint32_t ipgt)
{
	uint32_t data;

	data = hal_read_reg(nghd->mac_base, QCOM_MAC_CTRL0);
	data &= ~QCOM_IPGT_POS;
	ipgt = ipgt << QCOM_IPGT_LSB;
	data |= ipgt;
	hal_write_reg(nghd->mac_base, QCOM_MAC_CTRL0, data);
}

/*
 * qcom_set_ipgr()
 */
static inline void qcom_set_ipgr(struct nss_gmac_hal_dev *nghd, uint32_t ipgr)
{
	uint32_t data;

	data = hal_read_reg(nghd->mac_base, QCOM_MAC_CTRL0);
	data &= ~QCOM_IPGR2_POS;
	ipgr = ipgr << QCOM_IPGR2_LSB;
	data |= ipgr;
	hal_write_reg(nghd->mac_base, QCOM_MAC_CTRL0, data);
}

/*
 * qcom_set_half_thdf_ctrl()
 */
static inline void qcom_set_half_thdf_ctrl(struct nss_gmac_hal_dev *nghd)
{
	hal_set_reg_bits(nghd, QCOM_MAC_CTRL0, QCOM_HALF_THDF_CTRL);
}

/*
 * qcom_reset_half_thdf_ctrl()
 */
static inline void qcom_reset_half_thdf_ctrl(struct nss_gmac_hal_dev *nghd)
{
	hal_clear_reg_bits(nghd, QCOM_MAC_CTRL0, QCOM_HALF_THDF_CTRL);
}

/*
 * qcom_set_frame_len_chk()
 */
static inline void qcom_set_frame_len_chk(struct nss_gmac_hal_dev *nghd)
{
	hal_set_reg_bits(nghd, QCOM_MAC_CTRL0, QCOM_FLCHK);
}

/*
 * qcom_reset_frame_len_chk()
 */
static inline void qcom_reset_frame_len_chk(struct nss_gmac_hal_dev *nghd)
{
	hal_clear_reg_bits(nghd, QCOM_MAC_CTRL0, QCOM_FLCHK);
}

/*
 * qcom_set_abebe()
 */
static inline void qcom_set_abebe(struct nss_gmac_hal_dev *nghd)
{
	hal_set_reg_bits(nghd, QCOM_MAC_CTRL0, QCOM_ABEBE);
}

/*
 * qcom_reset_abebe()
 */
static inline void qcom_reset_abebe(struct nss_gmac_hal_dev *nghd)
{
	hal_clear_reg_bits(nghd, QCOM_MAC_CTRL0, QCOM_ABEBE);
}

/*
 * qcom_set_amaxe()
 */
static inline void qcom_set_amaxe(struct nss_gmac_hal_dev *nghd)
{
	hal_set_reg_bits(nghd, QCOM_MAC_CTRL0, QCOM_AMAXE);
}

/*
 * qcom_reset_amaxe()
 */
static inline void qcom_reset_amaxe(struct nss_gmac_hal_dev *nghd)
{
	hal_clear_reg_bits(nghd, QCOM_MAC_CTRL0, QCOM_AMAXE);
}

/*
 * qcom_set_bpnb()
 */
static inline void qcom_set_bpnb(struct nss_gmac_hal_dev *nghd)
{
	hal_set_reg_bits(nghd, QCOM_MAC_CTRL0, QCOM_BPNB);
}

/*
 * qcom_reset_bpnb()
 */
static inline void qcom_reset_bpnb(struct nss_gmac_hal_dev *nghd)
{
	hal_clear_reg_bits(nghd, QCOM_MAC_CTRL0, QCOM_BPNB);
}

/*
 * qcom_set_nobo()
 */
static inline void qcom_set_nobo(struct nss_gmac_hal_dev *nghd)
{
	hal_set_reg_bits(nghd, QCOM_MAC_CTRL0, QCOM_NOBO);
}

/*
 * qcom_reset_nobo()
 */
static inline void qcom_reset_nobo(struct nss_gmac_hal_dev *nghd)
{
	hal_clear_reg_bits(nghd, QCOM_MAC_CTRL0, QCOM_NOBO);
}

/*
 * qcom_set_drbnib_rxok()
 */
static inline void qcom_set_drbnib_rxok(struct nss_gmac_hal_dev *nghd)
{
	hal_set_reg_bits(nghd, QCOM_MAC_CTRL0, QCOM_DRBNIB_RXOK);
}

/*
 * qcom_reset_drbnib_rxok()
 */
static inline void qcom_reset_drbnib_rxok(struct nss_gmac_hal_dev *nghd)
{
	hal_clear_reg_bits(nghd, QCOM_MAC_CTRL0, QCOM_DRBNIB_RXOK);
}

/*
 * qcom_set_jam_ipg()
 */
static inline void qcom_set_jam_ipg(struct nss_gmac_hal_dev *nghd,
							uint32_t jam_ipg)
{
	uint32_t data;

	data = hal_read_reg(nghd->mac_base, QCOM_MAC_CTRL1);
	data &= ~QCOM_JAM_IPG_POS;
	jam_ipg = jam_ipg << QCOM_JAM_IPG_LSB;
	data |= jam_ipg;
	hal_write_reg(nghd->mac_base, QCOM_MAC_CTRL1, data);
}

/*
 * qcom_set_ctrl1_test_pause()
 */
static inline void qcom_set_ctrl1_test_pause(struct nss_gmac_hal_dev *nghd)
{
	hal_set_reg_bits(nghd, QCOM_MAC_CTRL1, QCOM_TPAUSE);
}

/*
 * qcom_reset_ctrl1_test_pause()
 */
static inline void qcom_reset_ctrl1_test_pause(struct nss_gmac_hal_dev *nghd)
{
	hal_clear_reg_bits(nghd, QCOM_MAC_CTRL1, QCOM_TPAUSE);
}

/*
 * qcom_reset_ctrl1_test_pause()
 */
static inline void qcom_set_tctl(struct nss_gmac_hal_dev *nghd)
{
	hal_set_reg_bits(nghd, QCOM_MAC_CTRL1, QCOM_TCTL);
}

/*
 * qcom_reset_tctl()
 */
static inline void qcom_reset_tctl(struct nss_gmac_hal_dev *nghd)
{
	hal_clear_reg_bits(nghd, QCOM_MAC_CTRL1, QCOM_TCTL);
}

/*
 * qcom_set_sstct()
 */
static inline void qcom_set_sstct(struct nss_gmac_hal_dev *nghd)
{
	hal_set_reg_bits(nghd, QCOM_MAC_CTRL1, QCOM_SSTCT);
}

/*
 * qcom_reset_sstct()
 */
static inline void qcom_reset_sstct(struct nss_gmac_hal_dev *nghd)
{
	hal_clear_reg_bits(nghd, QCOM_MAC_CTRL1, QCOM_SSTCT);
}

/*
 * qcom_set_simr()
 */
static inline void qcom_set_simr(struct nss_gmac_hal_dev *nghd)
{
	hal_set_reg_bits(nghd, QCOM_MAC_CTRL1, QCOM_SIMR);
}

/*
 * qcom_reset_simr()
 */
static inline void qcom_reset_simr(struct nss_gmac_hal_dev *nghd)
{
	hal_clear_reg_bits(nghd, QCOM_MAC_CTRL1, QCOM_SIMR);
}

/*
 * qcom_set_retry()
 */
static inline void qcom_set_retry(struct nss_gmac_hal_dev *nghd, uint32_t retry)
{
	uint32_t data;

	data = hal_read_reg(nghd->mac_base, QCOM_MAC_CTRL1);
	data &= ~QCOM_RETRY_POS;
	retry = retry << QCOM_RETRY_LSB;
	data |= retry;
	hal_write_reg(nghd->mac_base, QCOM_MAC_CTRL1, data);
}

/*
 * qcom_set_prlen()
 */
static inline void qcom_set_prlen(struct nss_gmac_hal_dev *nghd, uint32_t prlen)
{
	uint32_t data;

	data = hal_read_reg(nghd->mac_base, QCOM_MAC_CTRL1);
	data &= ~QCOM_PRLEN_POS;
	prlen = prlen << QCOM_PRLEN_LSB;
	data |= prlen;
	hal_write_reg(nghd->mac_base, QCOM_MAC_CTRL1, data);
}

/*
 * qcom_set_ppad()
 */
static inline void qcom_set_ppad(struct nss_gmac_hal_dev *nghd)
{
	hal_set_reg_bits(nghd, QCOM_MAC_CTRL1, QCOM_PPAD);
}

/*
 * qcom_reset_ppad()
 */
static inline void qcom_reset_ppad(struct nss_gmac_hal_dev *nghd)
{
	hal_clear_reg_bits(nghd, QCOM_MAC_CTRL1, QCOM_PPAD);
}

/*
 * qcom_set_povr()
 */
static inline void qcom_set_povr(struct nss_gmac_hal_dev *nghd)
{
	hal_set_reg_bits(nghd, QCOM_MAC_CTRL1, QCOM_POVR);
}

/*
 * qcom_reset_povr()
 */
static inline void qcom_reset_povr(struct nss_gmac_hal_dev *nghd)
{
	hal_clear_reg_bits(nghd, QCOM_MAC_CTRL1, QCOM_POVR);
}

/*
 * qcom_set_phug()
 */
static inline void qcom_set_phug(struct nss_gmac_hal_dev *nghd)
{
	hal_set_reg_bits(nghd, QCOM_MAC_CTRL1, QCOM_PHUG);
}

/*
 * qcom_reset_phug()
 */
static inline void qcom_reset_phug(struct nss_gmac_hal_dev *nghd)
{
	hal_clear_reg_bits(nghd, QCOM_MAC_CTRL1, QCOM_PHUG);
}

/*
 * qcom_set_mbof()
 */
static inline void qcom_set_mbof(struct nss_gmac_hal_dev *nghd)
{
	hal_set_reg_bits(nghd, QCOM_MAC_CTRL1, QCOM_MBOF);
}

/*
 * qcom_reset_mbof()
 */
static inline void qcom_reset_mbof(struct nss_gmac_hal_dev *nghd)
{
	hal_clear_reg_bits(nghd, QCOM_MAC_CTRL1, QCOM_MBOF);
}

/*
 * qcom_set_lcol()
 */
static inline void qcom_set_lcol(struct nss_gmac_hal_dev *nghd, uint32_t lcol)
{
	uint32_t data;

	data = hal_read_reg(nghd->mac_base, QCOM_MAC_CTRL1);
	data &= ~QCOM_LCOL_POS;
	lcol = lcol << QCOM_LCOL_LSB;
	data |= lcol;
	hal_write_reg(nghd->mac_base, QCOM_MAC_CTRL1, data);
}

/*
 * qcom_set_long_jam()
 */
static inline void qcom_set_long_jam(struct nss_gmac_hal_dev *nghd)
{
	hal_set_reg_bits(nghd, QCOM_MAC_CTRL1, QCOM_LONG_JAM);
}

/*
 * qcom_reset_long_jam()
 */
static inline void qcom_reset_long_jam(struct nss_gmac_hal_dev *nghd)
{
	hal_clear_reg_bits(nghd, QCOM_MAC_CTRL1, QCOM_LONG_JAM);
}

/*
 * qcom_set_ipg_dec_len()
 */
static inline void qcom_set_ipg_dec_len(struct nss_gmac_hal_dev *nghd)
{
	hal_set_reg_bits(nghd, QCOM_MAC_CTRL2, QCOM_IPG_DEC_LEN);
}

/*
 * qcom_reset_ipg_dec_len()
 */
static inline void qcom_reset_ipg_dec_len(struct nss_gmac_hal_dev *nghd)
{
	hal_clear_reg_bits(nghd, QCOM_MAC_CTRL2, QCOM_IPG_DEC_LEN);
}

/*
 * qcom_set_ctrl2_test_pause()
 */
static inline void qcom_set_ctrl2_test_pause(struct nss_gmac_hal_dev *nghd)
{
	hal_set_reg_bits(nghd, QCOM_MAC_CTRL2, QCOM_TEST_PAUSE);
}

/*
 * qcom_reset_ctrl2_test_pause()
 */
static inline void qcom_reset_ctrl2_test_pause(struct nss_gmac_hal_dev *nghd)
{
	hal_clear_reg_bits(nghd, QCOM_MAC_CTRL2, QCOM_TEST_PAUSE);
}

/*
 * qcom_set_mac_loopback()
 */
static inline void qcom_set_mac_loopback(struct nss_gmac_hal_dev *nghd)
{
	hal_set_reg_bits(nghd, QCOM_MAC_CTRL2, QCOM_MAC_LOOPBACK);
}

/*
 * qcom_reset_mac_loopback()
 */
static inline void qcom_reset_mac_loopback(struct nss_gmac_hal_dev *nghd)
{
	hal_clear_reg_bits(nghd, QCOM_MAC_CTRL2, QCOM_MAC_LOOPBACK);
}

/*
 * qcom_set_ipg_dec()
 */
static inline void qcom_set_ipg_dec(struct nss_gmac_hal_dev *nghd)
{
	hal_set_reg_bits(nghd, QCOM_MAC_CTRL2, QCOM_IPG_DEC);
}

/*
 * qcom_reset_ipg_dec()
 */
static inline void qcom_reset_ipg_dec(struct nss_gmac_hal_dev *nghd)
{
	hal_clear_reg_bits(nghd, QCOM_MAC_CTRL2, QCOM_IPG_DEC);
}

/*
 * qcom_set_crs_sel()
 */
static inline void qcom_set_crs_sel(struct nss_gmac_hal_dev *nghd)
{
	hal_set_reg_bits(nghd, QCOM_MAC_CTRL2, QCOM_SRS_SEL);
}

/*
 * qcom_reset_crs_sel()
 */
static inline void qcom_reset_crs_sel(struct nss_gmac_hal_dev *nghd)
{
	hal_clear_reg_bits(nghd, QCOM_MAC_CTRL2, QCOM_SRS_SEL);
}

/*
 * qcom_set_crc_rsv()
 */
static inline void qcom_set_crc_rsv(struct nss_gmac_hal_dev *nghd)
{
	hal_set_reg_bits(nghd, QCOM_MAC_CTRL2, QCOM_CRC_RSV);
}

/*
 * qcom_reset_crc_rsv()
 */
static inline void qcom_reset_crc_rsv(struct nss_gmac_hal_dev *nghd)
{
	hal_clear_reg_bits(nghd, QCOM_MAC_CTRL2, QCOM_CRC_RSV);
}

/*
 * qcom_set_ipgr1()
 */
static inline void qcom_set_ipgr1(struct nss_gmac_hal_dev *nghd, uint32_t ipgr1)
{
	uint32_t data;

	data = hal_read_reg(nghd->mac_base, QCOM_MAC_DBG_CTRL);
	data &= ~QCOM_DBG_IPGR1_POS;
	ipgr1 = ipgr1 << QCOM_DBG_IPGR1_LSB;
	data |= ipgr1;
	hal_write_reg(nghd->mac_base, QCOM_MAC_DBG_CTRL, data);
}

/*
 * qcom_set_hihg_ipg()
 */
static inline void qcom_set_hihg_ipg(struct nss_gmac_hal_dev *nghd,
			uint32_t hihg_ipg)
{
	uint32_t data;

	data = hal_read_reg(nghd->mac_base, QCOM_MAC_DBG_CTRL);
	data &= ~QCOM_DBG_HIHG_IPG_POS;
	data |= hihg_ipg << QCOM_DBG_HIHG_IPG_LSB;
	hal_write_reg(nghd->mac_base, QCOM_MAC_DBG_CTRL, data);
}

/*
 * qcom_set_mac_ipg_ctrl()
 */
static inline void qcom_set_mac_ipg_ctrl(struct nss_gmac_hal_dev *nghd,
			uint32_t mac_ipg_ctrl)
{
	uint32_t data;

	data = hal_read_reg(nghd->mac_base, QCOM_MAC_DBG_CTRL);
	data &= ~QCOM_DBG_MAC_IPG_CTRL_POS;
	data |= mac_ipg_ctrl << QCOM_DBG_MAC_IPG_CTRL_LSB;
	hal_write_reg(nghd->mac_base, QCOM_MAC_DBG_CTRL, data);
}

/*
 * qcom_set_mac_len_ctrl()
 */
static inline void qcom_set_mac_len_ctrl(struct nss_gmac_hal_dev *nghd)
{
	hal_set_reg_bits(nghd, QCOM_MAC_DBG_CTRL, QCOM_DBG_MAC_LEN_CTRL);
}

/*
 * qcom_reset_mac_len_ctrl()
 */
static inline void qcom_reset_mac_len_ctrl(struct nss_gmac_hal_dev *nghd)
{
	hal_clear_reg_bits(nghd, QCOM_MAC_DBG_CTRL, QCOM_DBG_MAC_LEN_CTRL);
}

/*
 * qcom_set_edxsdfr_transmit()
 */
static inline void qcom_set_edxsdfr_transmit(struct nss_gmac_hal_dev *nghd)
{
	hal_set_reg_bits(nghd, QCOM_MAC_DBG_CTRL, QCOM_DBG_EDxSDFR_TRANS);
}

/*
 * qcom_reset_edxsdfr_transmit()
 */
static inline void qcom_reset_edxsdfr_transmit(struct nss_gmac_hal_dev *nghd)
{
	hal_clear_reg_bits(nghd, QCOM_MAC_DBG_CTRL, QCOM_DBG_EDxSDFR_TRANS);
}

/*
 * qcom_set_mac_dbg_addr()
 */
static inline void qcom_set_mac_dbg_addr(struct nss_gmac_hal_dev *nghd,
						uint8_t mac_dbg_addr)
{
	hal_write_reg(nghd->mac_base, QCOM_MAC_DBG_ADDR, mac_dbg_addr);
}

/*
 * qcom_set_mac_dbg_data()
 */
static inline void qcom_set_mac_dbg_data(struct nss_gmac_hal_dev *nghd,
						uint32_t mac_dbg_data)
{
	hal_write_reg(nghd->mac_base, QCOM_MAC_DBG_DATA, mac_dbg_data);
}

/*
 * qcom_set_mac_jumbosize()
 */
static inline void qcom_set_mac_jumbosize(struct nss_gmac_hal_dev *nghd,
						uint16_t mac_jumbo_size)
{
	hal_write_reg(nghd->mac_base, QCOM_MAC_JMB_SIZE, mac_jumbo_size);
}

/*
 * qcom_clear_mib_ctrl()
 */
static inline void qcom_clear_mib_ctrl(struct nss_gmac_hal_dev *nghd)
{
	hal_write_reg(nghd->mac_base, QCOM_MAC_MIB_CTRL, 0);
}

/*
 * qcom_set_mib_ctrl()
 */
static inline void qcom_set_mib_ctrl(struct nss_gmac_hal_dev *nghd,
						int mib_settings)
{
	hal_set_reg_bits(nghd, QCOM_MAC_MIB_CTRL,
			mib_settings);
}

/*
 * qcom_get_stats()
 */
static int qcom_get_stats(struct nss_gmac_hal_dev *nghd)
{
	struct qcom_hal_dev *qhd = (struct qcom_hal_dev *)nghd;
	fal_mib_counter_t *stats = &(qhd->stats);

	if (fal_mib_counter_get(0, nghd->mac_id, stats) < 0)
		return -1;

	return 0;
}
#endif /* __QCOM_DEV_H__ */

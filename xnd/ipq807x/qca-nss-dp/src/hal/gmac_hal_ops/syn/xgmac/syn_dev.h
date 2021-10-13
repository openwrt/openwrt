/*
 **************************************************************************
 * Copyright (c) 2016,2020 The Linux Foundation. All rights reserved.
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

#ifndef __SYN_DEV_H__
#define __SYN_DEV_H__

#include "syn_reg.h"
#include <fal/fal_mib.h>
#include <fal/fal_port_ctrl.h>

/*
 * Subclass for base nss_gmac_haldev
 */
struct syn_hal_dev {
	struct nss_gmac_hal_dev nghd;	/* Base class */
	fal_xgmib_info_t stats;		/* Stats structure */
};

/*
 * syn_set_rx_flow_ctrl()
 */
static inline void syn_set_rx_flow_ctrl(
		struct nss_gmac_hal_dev *nghd)
{
	hal_set_reg_bits(nghd, SYN_MAC_RX_FLOW_CTL,
			SYN_MAC_RX_FLOW_ENABLE);
}

/*
 * syn_clear_rx_flow_ctrl()
 */
static inline void syn_clear_rx_flow_ctrl(
		struct nss_gmac_hal_dev *nghd)
{
	hal_clear_reg_bits(nghd, SYN_MAC_RX_FLOW_CTL,
			SYN_MAC_RX_FLOW_ENABLE);
}

/*
 * syn_set_tx_flow_ctrl()
 */
static inline void syn_set_tx_flow_ctrl(
		struct nss_gmac_hal_dev *nghd)
{
	hal_set_reg_bits(nghd, SYN_MAC_Q0_TX_FLOW_CTL,
			SYN_MAC_TX_FLOW_ENABLE);
}

/*
 * syn_send_tx_pause_frame()
 */
static inline void syn_send_tx_pause_frame(
		struct nss_gmac_hal_dev *nghd)
{
	hal_set_reg_bits(nghd, SYN_MAC_Q0_TX_FLOW_CTL,
			SYN_MAC_TX_FLOW_ENABLE);
	hal_set_reg_bits(nghd, SYN_MAC_Q0_TX_FLOW_CTL,
			SYN_MAC_TX_PAUSE_SEND);
}

/*
 * syn_clear_tx_flow_ctrl()
 */
static inline void syn_clear_tx_flow_ctrl(
		struct nss_gmac_hal_dev *nghd)
{
	hal_clear_reg_bits(nghd, SYN_MAC_Q0_TX_FLOW_CTL,
			SYN_MAC_TX_FLOW_ENABLE);
}

/*
 * syn_clear_mac_ctrl()
 */
static inline void syn_clear_mac_ctrl(
		struct nss_gmac_hal_dev *nghd)
{
	hal_write_reg(nghd->mac_base, SYN_MAC_TX_CONFIG, 0);
	hal_write_reg(nghd->mac_base, SYN_MAC_RX_CONFIG, 0);
}

/*
 * syn_rx_enable()
 */
static inline void syn_rx_enable(struct nss_gmac_hal_dev *nghd)
{
	hal_set_reg_bits(nghd, SYN_MAC_RX_CONFIG, SYN_MAC_RX_ENABLE);
	hal_set_reg_bits(nghd, SYN_MAC_PACKET_FILTER, SYN_MAC_RX_ENABLE);
}

/*
 * syn_rx_disable()
 */
static inline void syn_rx_disable(struct nss_gmac_hal_dev *nghd)
{
	hal_clear_reg_bits(nghd, SYN_MAC_RX_CONFIG, SYN_MAC_RX_ENABLE);
}

/*
 * syn_tx_enable()
 */
static inline void syn_tx_enable(struct nss_gmac_hal_dev *nghd)
{
	hal_set_reg_bits(nghd, SYN_MAC_TX_CONFIG, SYN_MAC_TX_ENABLE);
}

/*
 * syn_tx_disable()
 */
static inline void syn_tx_disable(struct nss_gmac_hal_dev *nghd)
{
	hal_clear_reg_bits(nghd, SYN_MAC_TX_CONFIG,
			SYN_MAC_TX_ENABLE);
}

/*
 * syn_set_mmc_stats()
 */
static inline void syn_set_mmc_stats(struct nss_gmac_hal_dev *nghd)
{
	hal_set_reg_bits(nghd, SYN_MAC_MMC_CTL,
			SYN_MAC_MMC_RSTONRD);
}

/*
 * syn_rx_jumbo_frame_enable()
 */
static inline void syn_rx_jumbo_frame_enable(
		struct nss_gmac_hal_dev *nghd)
{
	hal_set_reg_bits(nghd, SYN_MAC_RX_CONFIG,
			SYN_MAC_JUMBO_FRAME_ENABLE);
}

/*
 * syn_rx_jumbo_frame_disable()
 */
static inline void syn_rx_jumbo_frame_disable(
		struct nss_gmac_hal_dev *nghd)
{
	hal_clear_reg_bits(nghd, SYN_MAC_RX_CONFIG,
			SYN_MAC_JUMBO_FRAME_ENABLE);
}

/*
 * syn_set_full_duplex()
 */
static inline void syn_set_full_duplex(
		struct nss_gmac_hal_dev *nghd)
{
	/* TBD */
	return;
}

/*
 * syn_set_half_duplex()
 */
static inline void syn_set_half_duplex(
		struct nss_gmac_hal_dev *nghd)
{
	/* TBD */
	return;
}

static int syn_get_stats(struct nss_gmac_hal_dev *nghd)
{
	struct syn_hal_dev *shd = (struct syn_hal_dev *)nghd;
	fal_xgmib_info_t *stats = &(shd->stats);

	if (fal_get_xgmib_info(0, nghd->mac_id, stats))
		return -1;

	return 0;
}
#endif /*__SYN_DEV_H__*/

// SPDX-License-Identifier: GPL-2.0-only OR BSD-2-Clause
/* Copyright (C) 2013 Annapurna Labs Ltd. */

/**
 * @file   al_hal_udma_config.c
 *
 * @brief  Universal DMA HAL driver for configurations
 *
 */

#include <mach/al_hal_common.h>
#include <mach/al_hal_udma_regs.h>
#include <mach/al_hal_udma_config.h>

/** M2S packet len configuration */
int al_udma_m2s_packet_size_cfg_set(struct al_udma *udma,
				struct al_udma_m2s_pkt_len_conf *conf)
{
	uint32_t reg = al_reg_read32(&udma->udma_regs->m2s.m2s.cfg_len);
	uint32_t max_supported_size = UDMA_M2S_CFG_LEN_MAX_PKT_SIZE_MASK;

	al_assert(udma->type == UDMA_TX);

	if (conf->encode_64k_as_zero == AL_TRUE)
		max_supported_size += 1;	/* 64K */

	if (conf->max_pkt_size > max_supported_size) {
		al_err("udma [%s]: requested max_pkt_size (0x%x) exceeds the"
			"supported limit (0x%x)\n", udma->name,
			 conf->max_pkt_size, max_supported_size);
		return -EINVAL;
	}

	reg &= ~UDMA_M2S_CFG_LEN_ENCODE_64K;
	if (conf->encode_64k_as_zero == AL_TRUE)
		reg |= UDMA_M2S_CFG_LEN_ENCODE_64K;
	else
		reg &= ~UDMA_M2S_CFG_LEN_ENCODE_64K;

	reg &= ~UDMA_M2S_CFG_LEN_MAX_PKT_SIZE_MASK;
	reg |= conf->max_pkt_size;

	al_reg_write32(&udma->udma_regs->m2s.m2s.cfg_len, reg);
	return 0;
}

/** Report Error - to be used for abort */
void al_udma_err_report(struct al_udma *udma __attribute__((__unused__)))
{
	return;
}

/** Statistics - TBD */
void al_udma_stats_get(struct al_udma *udma __attribute__((__unused__)))
{
	return;
}

/* set max descriptors */
int al_udma_m2s_max_descs_set(struct al_udma *udma, uint8_t max_descs)
{
	uint32_t pref_thr = max_descs;
	uint32_t min_burst_above_thr = 4;
	al_assert(max_descs <= AL_UDMA_M2S_MAX_ALLOWED_DESCS_PER_PACKET);
	al_assert(max_descs > 0);

	/* increase min_burst_above_thr so larger burst can be used to fetch
	 * descriptors */
	if (pref_thr >= 8)
		min_burst_above_thr = 8;
	else {
	/* don't set prefetch threshold too low so we can have the
	 * min_burst_above_thr >= 4 */
		pref_thr = 4;
	}

	al_reg_write32_masked(&udma->udma_regs->m2s.m2s_rd.desc_pref_cfg_2,
			      UDMA_M2S_RD_DESC_PREF_CFG_2_MAX_DESC_PER_PKT_MASK,
			      max_descs << UDMA_M2S_RD_DESC_PREF_CFG_2_MAX_DESC_PER_PKT_SHIFT);

	al_reg_write32_masked(&udma->udma_regs->m2s.m2s_rd.desc_pref_cfg_3,
			      UDMA_M2S_RD_DESC_PREF_CFG_3_PREF_THR_MASK |
			      UDMA_M2S_RD_DESC_PREF_CFG_3_MIN_BURST_ABOVE_THR_MASK,
			      (max_descs << UDMA_M2S_RD_DESC_PREF_CFG_3_PREF_THR_SHIFT) |
			      (min_burst_above_thr << UDMA_M2S_RD_DESC_PREF_CFG_3_MIN_BURST_ABOVE_THR_SHIFT));

	return 0;
}

/* set s2m max descriptors */
int al_udma_s2m_max_descs_set(struct al_udma *udma, uint8_t max_descs)
{
	uint32_t pref_thr = max_descs;
	uint32_t min_burst_above_thr = 4;
	al_assert(max_descs <= AL_UDMA_S2M_MAX_ALLOWED_DESCS_PER_PACKET);
	al_assert(max_descs > 0);

	/* increase min_burst_above_thr so larger burst can be used to fetch
	 * descriptors */
	if (pref_thr >= 8)
		min_burst_above_thr = 8;
	else
	/* don't set prefetch threshold too low so we can have the
	 * min_burst_above_thr >= 4 */
		pref_thr = 4;

	al_reg_write32_masked(&udma->udma_regs->s2m.s2m_rd.desc_pref_cfg_3,
			      UDMA_S2M_RD_DESC_PREF_CFG_3_PREF_THR_MASK |
			      UDMA_S2M_RD_DESC_PREF_CFG_3_MIN_BURST_ABOVE_THR_MASK,
			      (max_descs << UDMA_S2M_RD_DESC_PREF_CFG_3_PREF_THR_SHIFT) |
			      (min_burst_above_thr << UDMA_S2M_RD_DESC_PREF_CFG_3_MIN_BURST_ABOVE_THR_SHIFT));

	return 0;
}

/* S2M UDMA configure a queue's completion descriptors coalescing */
int al_udma_s2m_q_compl_coal_config(struct al_udma_q *udma_q, al_bool enable, uint32_t
		coal_timeout)
{
	uint32_t reg = al_reg_read32(&udma_q->q_regs->s2m_q.comp_cfg);

	if (enable == AL_TRUE)
		reg &= ~UDMA_S2M_Q_COMP_CFG_DIS_COMP_COAL;
	else
		reg |= UDMA_S2M_Q_COMP_CFG_DIS_COMP_COAL;

	al_reg_write32(&udma_q->q_regs->s2m_q.comp_cfg, reg);

	al_reg_write32(&udma_q->q_regs->s2m_q.comp_cfg_2, coal_timeout);
	return 0;
}

/* S2M UDMA configure completion descriptors write burst parameters */
int al_udma_s2m_compl_desc_burst_config(struct al_udma *udma, uint16_t
		burst_size)
{
	if ((burst_size != 64) && (burst_size != 128) && (burst_size != 256)) {
		al_err("%s: invalid burst_size value (%d)\n", __func__,
				burst_size);
		return -EINVAL;
	}

	/* convert burst size from bytes to beats (16 byte) */
	burst_size = burst_size / 16;
	al_reg_write32_masked(&udma->udma_regs->s2m.axi_s2m.desc_wr_cfg_1,
			UDMA_AXI_S2M_DESC_WR_CFG_1_MIN_AXI_BEATS_MASK |
			UDMA_AXI_S2M_DESC_WR_CFG_1_MAX_AXI_BEATS_MASK,
			burst_size << UDMA_AXI_S2M_DESC_WR_CFG_1_MIN_AXI_BEATS_SHIFT |
			burst_size << UDMA_AXI_S2M_DESC_WR_CFG_1_MAX_AXI_BEATS_SHIFT);
	return 0;
}

/* UDMA VMID control advanced Tx queue configuration */
void al_udma_gen_vmid_advanced_tx_q_conf(
	struct al_udma_q				*q,
	struct al_udma_gen_vmid_advanced_tx_q_conf	*conf)
{
	struct unit_regs *unit_regs = (struct unit_regs *)q->udma->udma_regs;
	struct udma_gen_vmpr *vmpr = &unit_regs->gen.vmpr[q->qid];

	al_reg_write32_masked(
		&vmpr->cfg_vmpr_0,
		UDMA_GEN_VMPR_CFG_VMPR_0_TX_Q_HISEL_MASK |
		UDMA_GEN_VMPR_CFG_VMPR_0_TX_Q_DATA_VMID_EN |
		UDMA_GEN_VMPR_CFG_VMPR_0_TX_Q_PREF_VMID_EN |
		UDMA_GEN_VMPR_CFG_VMPR_0_TX_Q_CMPL_VMID_EN,
		conf->tx_q_addr_hi_sel |
		((conf->tx_q_data_vmid_en == AL_TRUE) ?
		 UDMA_GEN_VMPR_CFG_VMPR_0_TX_Q_DATA_VMID_EN : 0) |
		((conf->tx_q_prefetch_vmid_en == AL_TRUE) ?
		 UDMA_GEN_VMPR_CFG_VMPR_0_TX_Q_PREF_VMID_EN : 0) |
		((conf->tx_q_compl_vmid_en == AL_TRUE) ?
		 UDMA_GEN_VMPR_CFG_VMPR_0_TX_Q_CMPL_VMID_EN : 0));

	al_reg_write32(
		&vmpr->cfg_vmpr_1,
		conf->tx_q_addr_hi);

	al_reg_write32_masked(
		&vmpr->cfg_vmpr_2,
		UDMA_GEN_VMPR_CFG_VMPR_2_TX_Q_PREF_VMID_MASK |
		UDMA_GEN_VMPR_CFG_VMPR_2_TX_Q_CMPL_VMID_MASK,
		(conf->tx_q_prefetch_vmid <<
		 UDMA_GEN_VMPR_CFG_VMPR_2_TX_Q_PREF_VMID_SHIFT) |
		(conf->tx_q_compl_vmid <<
		 UDMA_GEN_VMPR_CFG_VMPR_2_TX_Q_CMPL_VMID_SHIFT));

	al_reg_write32_masked(
		&vmpr->cfg_vmpr_3,
		UDMA_GEN_VMPR_CFG_VMPR_3_TX_Q_DATA_VMID_MASK |
		UDMA_GEN_VMPR_CFG_VMPR_3_TX_Q_DATA_VMID_SEL_MASK,
		(conf->tx_q_data_vmid <<
		 UDMA_GEN_VMPR_CFG_VMPR_3_TX_Q_DATA_VMID_SHIFT) |
		(conf->tx_q_data_vmid_mask <<
		 UDMA_GEN_VMPR_CFG_VMPR_3_TX_Q_DATA_VMID_SEL_SHIFT));
}

/** UDMA VMID control advanced Rx queue configuration */
void al_udma_gen_vmid_advanced_rx_q_conf(
	struct al_udma_q				*q,
	struct al_udma_gen_vmid_advanced_rx_q_conf	*conf)
{
	struct unit_regs *unit_regs = (struct unit_regs *)q->udma->udma_regs;
	struct udma_gen_vmpr *vmpr = &unit_regs->gen.vmpr[q->qid];

	al_reg_write32_masked(
		&vmpr->cfg_vmpr_4,
		UDMA_GEN_VMPR_CFG_VMPR_4_RX_Q_BUF1_HISEL_MASK |
		UDMA_GEN_VMPR_CFG_VMPR_4_RX_Q_BUF1_VMID_EN |
		UDMA_GEN_VMPR_CFG_VMPR_4_RX_Q_BUF2_HISEL_MASK |
		UDMA_GEN_VMPR_CFG_VMPR_4_RX_Q_BUF2_VMID_EN |
		UDMA_GEN_VMPR_CFG_VMPR_4_RX_Q_DDP_HISEL_MASK |
		UDMA_GEN_VMPR_CFG_VMPR_4_RX_Q_DDP_VMID_EN |
		UDMA_GEN_VMPR_CFG_VMPR_4_RX_Q_PREF_VMID_EN |
		UDMA_GEN_VMPR_CFG_VMPR_4_RX_Q_CMPL_VMID_EN,
		(conf->rx_q_addr_hi_sel <<
		 UDMA_GEN_VMPR_CFG_VMPR_4_RX_Q_BUF1_HISEL_SHIFT) |
		((conf->rx_q_data_vmid_en == AL_TRUE) ?
		 UDMA_GEN_VMPR_CFG_VMPR_4_RX_Q_BUF1_VMID_EN : 0) |
		(conf->rx_q_data_buff2_addr_hi_sel <<
		 UDMA_GEN_VMPR_CFG_VMPR_4_RX_Q_BUF2_HISEL_SHIFT) |
		((conf->rx_q_data_buff2_vmid_en == AL_TRUE) ?
		 UDMA_GEN_VMPR_CFG_VMPR_4_RX_Q_BUF2_VMID_EN : 0) |
		(conf->rx_q_ddp_addr_hi_sel <<
		 UDMA_GEN_VMPR_CFG_VMPR_4_RX_Q_DDP_HISEL_SHIFT) |
		((conf->rx_q_ddp_vmid_en == AL_TRUE) ?
		 UDMA_GEN_VMPR_CFG_VMPR_4_RX_Q_DDP_VMID_EN : 0) |
		((conf->rx_q_prefetch_vmid_en == AL_TRUE) ?
		 UDMA_GEN_VMPR_CFG_VMPR_4_RX_Q_PREF_VMID_EN : 0) |
		((conf->rx_q_compl_vmid_en == AL_TRUE) ?
		 UDMA_GEN_VMPR_CFG_VMPR_4_RX_Q_CMPL_VMID_EN : 0));

	al_reg_write32_masked(
		&vmpr->cfg_vmpr_6,
		UDMA_GEN_VMPR_CFG_VMPR_6_RX_Q_PREF_VMID_MASK |
		UDMA_GEN_VMPR_CFG_VMPR_6_RX_Q_CMPL_VMID_MASK,
		(conf->rx_q_prefetch_vmid <<
		 UDMA_GEN_VMPR_CFG_VMPR_6_RX_Q_PREF_VMID_SHIFT) |
		(conf->rx_q_compl_vmid <<
		 UDMA_GEN_VMPR_CFG_VMPR_6_RX_Q_CMPL_VMID_SHIFT));

	al_reg_write32_masked(
		&vmpr->cfg_vmpr_7,
		UDMA_GEN_VMPR_CFG_VMPR_7_RX_Q_BUF1_VMID_MASK |
		UDMA_GEN_VMPR_CFG_VMPR_7_RX_Q_BUF1_VMID_SEL_MASK,
		(conf->rx_q_data_vmid <<
		 UDMA_GEN_VMPR_CFG_VMPR_7_RX_Q_BUF1_VMID_SHIFT) |
		(conf->rx_q_data_vmid_mask <<
		 UDMA_GEN_VMPR_CFG_VMPR_7_RX_Q_BUF1_VMID_SEL_SHIFT));

	al_reg_write32_masked(
		&vmpr->cfg_vmpr_8,
		UDMA_GEN_VMPR_CFG_VMPR_8_RX_Q_BUF2_VMID_MASK |
		UDMA_GEN_VMPR_CFG_VMPR_8_RX_Q_BUF2_VMID_SEL_MASK,
		(conf->rx_q_data_vmid <<
		 UDMA_GEN_VMPR_CFG_VMPR_8_RX_Q_BUF2_VMID_SHIFT) |
		(conf->rx_q_data_vmid_mask <<
		 UDMA_GEN_VMPR_CFG_VMPR_8_RX_Q_BUF2_VMID_SEL_SHIFT));

	al_reg_write32_masked(
		&vmpr->cfg_vmpr_9,
		UDMA_GEN_VMPR_CFG_VMPR_9_RX_Q_DDP_VMID_MASK |
		UDMA_GEN_VMPR_CFG_VMPR_9_RX_Q_DDP_VMID_SEL_MASK,
		(conf->rx_q_data_vmid <<
		 UDMA_GEN_VMPR_CFG_VMPR_9_RX_Q_DDP_VMID_SHIFT) |
		(conf->rx_q_data_vmid_mask <<
		 UDMA_GEN_VMPR_CFG_VMPR_9_RX_Q_DDP_VMID_SEL_SHIFT));

	al_reg_write32(
		&vmpr->cfg_vmpr_10,
		conf->rx_q_addr_hi);

	al_reg_write32(
		&vmpr->cfg_vmpr_11,
		conf->rx_q_data_buff2_addr_hi);

	al_reg_write32(
		&vmpr->cfg_vmpr_12,
		conf->rx_q_ddp_addr_hi);
}

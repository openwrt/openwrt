/* SPDX-License-Identifier: GPL-2.0-only OR BSD-2-Clause */
/* Copyright (C) 2013 Annapurna Labs Ltd. */

/**
 * @defgroup group_udma_config UDMA Config
 * @ingroup group_udma_api
 *  UDMA Config API
 *  @{
 * @file   al_hal_udma_config.h
 *
 * @brief C Header file for the Universal DMA HAL driver for configuration APIs
 *
 */

#ifndef __AL_HAL_UDMA_CONFIG_H__
#define __AL_HAL_UDMA_CONFIG_H__

#include <mach/al_hal_udma.h>

/** M2S max packet size configuration */
struct al_udma_m2s_pkt_len_conf {
	uint32_t max_pkt_size;
	al_bool encode_64k_as_zero;
};

/** UDMA per Tx queue advanced VMID control configuration */
struct al_udma_gen_vmid_advanced_tx_q_conf {
	/**********************************************************************
	 * Tx Data VMID
	 **********************************************************************/
	/* Tx data VMID enable */
	al_bool tx_q_data_vmid_en;

	/*
	 * For Tx data reads, replacement bits for the original address.
	 * The number of bits replaced is determined according to
	 * 'tx_q_addr_hi_sel'
	 */
	unsigned int tx_q_addr_hi;

	/*
	 * For Tx data reads, 6 bits serving the number of bits taken from the
	 * extra register on account of bits coming from the original address
	 * field.
	 * When 'tx_q_addr_hi_sel'=32 all of 'tx_q_addr_hi' will be taken.
	 * When 'tx_q_addr_hi_sel'=0 none of it will be taken, and when any
	 * value in between, it will start from the MSB bit and sweep down as
	 * many bits as needed. For example if 'tx_q_addr_hi_sel'=8, the final
	 * address [63:56] will carry 'tx_q_addr_hi'[31:24] while [55:32] will
	 * carry the original buffer address[55:32].
	 */
	unsigned int tx_q_addr_hi_sel;

	/*
	 * Tx data read VMID
	 * Masked per bit with 'tx_q_data_vmid_mask'
	 */
	unsigned int tx_q_data_vmid;

	/*
	 * Tx data read VMID mask
	 * Each '1' selects from the buffer address, each '0' selects from
	 * 'tx_q_data_vmid'
	 */
	unsigned int tx_q_data_vmid_mask;

	/**********************************************************************
	 * Tx prefetch VMID
	 **********************************************************************/
	/* Tx prefetch VMID enable */
	al_bool tx_q_prefetch_vmid_en;

	/* Tx prefetch VMID */
	unsigned int tx_q_prefetch_vmid;

	/**********************************************************************
	 * Tx completion VMID
	 **********************************************************************/
	/* Tx completion VMID enable */
	al_bool tx_q_compl_vmid_en;

	/* Tx completion VMID */
	unsigned int tx_q_compl_vmid;
};

/** UDMA per Rx queue advanced VMID control configuration */
struct al_udma_gen_vmid_advanced_rx_q_conf {
	/**********************************************************************
	 * Rx Data VMID
	 **********************************************************************/
	/* Rx data VMID enable */
	al_bool rx_q_data_vmid_en;

	/*
	 * For Rx data writes, replacement bits for the original address.
	 * The number of bits replaced is determined according to
	 * 'rx_q_addr_hi_sel'
	 */
	unsigned int rx_q_addr_hi;

	/*
	 * For Rx data writes, 6 bits serving the number of bits taken from the
	 * extra register on account of bits coming from the original address
	 * field.
	 */
	unsigned int rx_q_addr_hi_sel;

	/*
	 * Rx data write VMID
	 * Masked per bit with 'rx_q_data_vmid_mask'
	 */
	unsigned int rx_q_data_vmid;

	/* Rx data write VMID mask */
	unsigned int rx_q_data_vmid_mask;

	/**********************************************************************
	 * Rx Data Buffer 2 VMID
	 **********************************************************************/
	/* Rx data buff2 VMID enable */
	al_bool rx_q_data_buff2_vmid_en;

	/*
	 * For Rx data buff2 writes, replacement bits for the original address.
	 * The number of bits replaced is determined according to
	 * 'rx_q_data_buff2_addr_hi_sel'
	 */
	unsigned int rx_q_data_buff2_addr_hi;

	/*
	 * For Rx data buff2 writes, 6 bits serving the number of bits taken
	 * from the extra register on account of bits coming from the original
	 * address field.
	 */
	unsigned int rx_q_data_buff2_addr_hi_sel;

	/*
	 * Rx data buff2 write VMID
	 * Masked per bit with 'rx_q_data_buff2_mask'
	 */
	unsigned int rx_q_data_buff2_vmid;

	/* Rx data buff2 write VMID mask */
	unsigned int rx_q_data_buff2_mask;

	/**********************************************************************
	 * Rx DDP VMID
	 **********************************************************************/
	/* Rx DDP write VMID enable */
	al_bool rx_q_ddp_vmid_en;

	/*
	 * For Rx DDP writes, replacement bits for the original address.
	 * The number of bits replaced is determined according to
	 * 'rx_q_ddp_addr_hi_sel'
	 */
	unsigned int rx_q_ddp_addr_hi;

	/*
	 * For Rx DDP writes, 6 bits serving the number of bits taken from the
	 * extra register on account of bits coming from the original address
	 * field.
	 */
	unsigned int rx_q_ddp_addr_hi_sel;

	/*
	 * Rx DDP write VMID
	 * Masked per bit with 'rx_q_ddp_mask'
	 */
	unsigned int rx_q_ddp_vmid;

	/* Rx DDP write VMID mask */
	unsigned int rx_q_ddp_mask;

	/**********************************************************************
	 * Rx prefetch VMID
	 **********************************************************************/
	/* Rx prefetch VMID enable */
	al_bool rx_q_prefetch_vmid_en;

	/* Rx prefetch VMID */
	unsigned int rx_q_prefetch_vmid;

	/**********************************************************************
	 * Rx completion VMID
	 **********************************************************************/
	/* Rx completion VMID enable */
	al_bool rx_q_compl_vmid_en;

	/* Rx completion VMID */
	unsigned int rx_q_compl_vmid;
};

/* Report Error - to be used for abort */
void al_udma_err_report(struct al_udma *udma);

/* Statistics - TBD */
void al_udma_stats_get(struct al_udma *udma);

/* Configure M2S packet len */
int al_udma_m2s_packet_size_cfg_set(struct al_udma *udma,
				    struct al_udma_m2s_pkt_len_conf *conf);

/* set m2s packet's max descriptors (including meta descriptors) */
#define AL_UDMA_M2S_MAX_ALLOWED_DESCS_PER_PACKET	31
int al_udma_m2s_max_descs_set(struct al_udma *udma, uint8_t max_descs);

/* set s2m packets' max descriptors */
#define AL_UDMA_S2M_MAX_ALLOWED_DESCS_PER_PACKET	31
int al_udma_s2m_max_descs_set(struct al_udma *udma, uint8_t max_descs);

/**
 * S2M UDMA configure a queue's completion descriptors coalescing
 *
 * @param q_udma
 * @param enable set to true to enable completion coalescing
 * @param coal_timeout in South Bridge cycles.
 *
 * @return 0 if no error found.
 */
int al_udma_s2m_q_compl_coal_config(struct al_udma_q *udma_q, al_bool enable, uint32_t coal_timeout);

/**
 * S2M UDMA configure completion descriptors write burst parameters
 *
 * @param udma
 * @param burst_size completion descriptors write burst size in bytes.
 *
 * @return 0 if no error found.
 */int al_udma_s2m_compl_desc_burst_config(struct al_udma *udma, uint16_t
		 burst_size);

/** UDMA VMID control advanced Tx queue configuration */
void al_udma_gen_vmid_advanced_tx_q_conf(
	struct al_udma_q				*q,
	struct al_udma_gen_vmid_advanced_tx_q_conf	*conf);

/** UDMA VMID control advanced Rx queue configuration */
void al_udma_gen_vmid_advanced_rx_q_conf(
	struct al_udma_q				*q,
	struct al_udma_gen_vmid_advanced_rx_q_conf	*conf);

/** @} end of UDMA config group */
#endif /* __AL_HAL_UDMA_CONFIG_H__ */

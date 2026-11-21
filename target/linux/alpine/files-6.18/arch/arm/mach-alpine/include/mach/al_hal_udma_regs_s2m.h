/* SPDX-License-Identifier: GPL-2.0-only OR BSD-2-Clause */
/* Copyright (C) 2013 Annapurna Labs Ltd. */

/**
 * @file   al_hal_udma_regs_s2m.h
 *
 * @brief C Header file for the UDMA S2M registers
 *
 */

#ifndef __AL_HAL_UDMA_S2M_REG_H
#define __AL_HAL_UDMA_S2M_REG_H

#include <mach/al_hal_plat_types.h>

/*
* Unit Registers
*/

struct udma_axi_s2m {
	/* [0x0] Data write master configuration */
	uint32_t data_wr_cfg_1;
	/* [0x4] Data write master configuration */
	uint32_t data_wr_cfg_2;
	/* [0x8] Descriptor read master configuration */
	uint32_t desc_rd_cfg_4;
	/* [0xc] Descriptor read master configuration */
	uint32_t desc_rd_cfg_5;
	/* [0x10] Completion  write master configuration */
	uint32_t comp_wr_cfg_1;
	/* [0x14] Completion  write master configuration */
	uint32_t comp_wr_cfg_2;
	/* [0x18] Data write master configuration */
	uint32_t data_wr_cfg;
	/* [0x1c] Descriptors read master configuration */
	uint32_t desc_rd_cfg_3;
	/* [0x20] Completion descriptors write master configuration */
	uint32_t desc_wr_cfg_1;
	/* [0x24] AXI outstanding read configuration */
	uint32_t ostand_cfg_rd;
	/* [0x28] AXI outstanding write configuration */
	uint32_t ostand_cfg_wr;
	uint32_t rsrvd[53];
};
struct udma_s2m {
	/*
	 * [0x0] DMA state
	 * 00  - No pending tasks
	 * 01 – Normal (active)
	 * 10 – Abort (error condition)
	 * 11 – Reserved
	 */
	uint32_t state;
	/* [0x4] CPU request to change DMA state */
	uint32_t change_state;
	uint32_t rsrvd_0;
	/*
	 * [0xc] S2M DMA error log mask.
	 * Each error has an interrupt controller cause bit.
	 * This register determines if these errors cause the S2M DMA to log the
	 * error condition.
	 * 0 - Log is enable
	 * 1 - Log is masked.
	 */
	uint32_t err_log_mask;
	uint32_t rsrvd_1;
	/*
	 * [0x14] DMA header log
	 * Sample the packet header that caused the error
	 */
	uint32_t log_0;
	/*
	 * [0x18] DMA header log
	 * Sample the packet header that caused the error.
	 */
	uint32_t log_1;
	/*
	 * [0x1c] DMA header log
	 * Sample the packet header that caused the error.
	 */
	uint32_t log_2;
	/*
	 * [0x20] DMA header log
	 * Sample the packet header that caused the error
	 */
	uint32_t log_3;
	/* [0x24] DMA clear error log */
	uint32_t clear_err_log;
	/* [0x28] S2M stream data FIFO status */
	uint32_t s_data_fifo_status;
	/* [0x2c] S2M stream header FIFO status */
	uint32_t s_header_fifo_status;
	/* [0x30] S2M AXI data FIFO status */
	uint32_t axi_data_fifo_status;
	/* [0x34] S2M unack FIFO status */
	uint32_t unack_fifo_status;
	/* [0x38] Select queue for debug */
	uint32_t indirect_ctrl;
	/*
	 * [0x3c] S2M prefetch FIFO status.
	 * Status of the selected queue in S2M_indirect_ctrl
	 */
	uint32_t sel_pref_fifo_status;
	/*
	 * [0x40] S2M completion FIFO status.
	 * Status of the selected queue in S2M_indirect_ctrl
	 */
	uint32_t sel_comp_fifo_status;
	/* [0x44] S2M state machine and FIFO clear control */
	uint32_t clear_ctrl;
	/* [0x48] S2M Misc Check enable */
	uint32_t check_en;
	/* [0x4c] S2M FIFO enable control, internal */
	uint32_t fifo_en;
	/* [0x50] Stream interface configuration */
	uint32_t stream_cfg;
	uint32_t rsrvd[43];
};
struct udma_s2m_rd {
	/* [0x0] S2M descriptor prefetch configuration */
	uint32_t desc_pref_cfg_1;
	/* [0x4] S2M descriptor prefetch configuration */
	uint32_t desc_pref_cfg_2;
	/* [0x8] S2M descriptor prefetch configuration */
	uint32_t desc_pref_cfg_3;
	/* [0xc] S2M descriptor prefetch configuration */
	uint32_t desc_pref_cfg_4;
	uint32_t rsrvd[12];
};
struct udma_s2m_wr {
	/* [0x0] Stream data FIFO configuration */
	uint32_t data_cfg_1;
	/* [0x4] Data write configuration */
	uint32_t data_cfg_2;
	uint32_t rsrvd[14];
};
struct udma_s2m_comp {
	/* [0x0] Completion controller configuration */
	uint32_t cfg_1c;
	/* [0x4] Completion controller configuration */
	uint32_t cfg_2c;
	uint32_t rsrvd_0;
	/* [0xc] Completion controller application acknowledge configuration */
	uint32_t cfg_application_ack;
	uint32_t rsrvd[12];
};
struct udma_s2m_stat {
	uint32_t rsrvd_0;
	/* [0x4] Number of dropped packets */
	uint32_t drop_pkt;
	/*
	 * [0x8] Counting the net length of the data buffers [64-bit]
	 * Should be read before rx_bytes_high
	 */
	uint32_t rx_bytes_low;
	/*
	 * [0xc] Counting the net length of the data buffers [64-bit]
	 * Should be read after tx_bytes_low (value is sampled when reading
	 * Should be read before rx_bytes_low
	 */
	uint32_t rx_bytes_high;
	/* [0x10] Total number of descriptors read from the host memory */
	uint32_t prefed_desc;
	/* [0x14] Number of packets written into the completion ring */
	uint32_t comp_pkt;
	/* [0x18] Number of descriptors written into the completion ring */
	uint32_t comp_desc;
	/*
	 * [0x1c] Number of acknowledged packets.
	 * (acknowledge sent to the stream interface)
	 */
	uint32_t ack_pkts;
	uint32_t rsrvd[56];
};
struct udma_s2m_feature {
	/*
	 * [0x0] S2M Feature register
	 * S2M instantiation parameters
	 */
	uint32_t reg_1;
	/* [0x4] Reserved S2M feature register */
	uint32_t reg_2;
	/*
	 * [0x8] S2M Feature register
	 * S2M instantiation parameters
	 */
	uint32_t reg_3;
	/*
	 * [0xc] S2M Feature register.
	 * S2M instantiation parameters.
	 */
	uint32_t reg_4;
	/*
	 * [0x10] S2M Feature register.
	 * S2M instantiation parameters.
	 */
	uint32_t reg_5;
	/* [0x14] S2M Feature register. S2M instantiation parameters. */
	uint32_t reg_6;
	uint32_t rsrvd[58];
};
struct udma_s2m_q {
	uint32_t rsrvd_0[8];
	/* [0x20] S2M Descriptor ring configuration */
	uint32_t cfg;
	/* [0x24] S2M Descriptor ring status and information */
	uint32_t status;
	/* [0x28] Rx Descriptor Ring Base Pointer [31:4] */
	uint32_t rdrbp_low;
	/* [0x2c] Rx Descriptor Ring Base Pointer [63:32] */
	uint32_t rdrbp_high;
	/*
	 * [0x30] Rx Descriptor Ring Length[23:2]
	 */
	uint32_t rdrl;
	/* [0x34] RX Descriptor Ring Head Pointer */
	uint32_t rdrhp;
	/* [0x38] Rx Descriptor Tail Pointer increment */
	uint32_t rdrtp_inc;
	/* [0x3c] Rx Descriptor Tail Pointer */
	uint32_t rdrtp;
	/* [0x40] RX Descriptor Current Pointer */
	uint32_t rdcp;
	/* [0x44] Rx Completion Ring Base Pointer [31:4] */
	uint32_t rcrbp_low;
	/* [0x48] Rx Completion Ring Base Pointer [63:32] */
	uint32_t rcrbp_high;
	/* [0x4c] Rx Completion Ring Head Pointer */
	uint32_t rcrhp;
	/*
	 * [0x50] RX Completion Ring Head Pointer internal.
	 * (Before the coalescing FIFO)
	 */
	uint32_t rcrhp_internal;
	/* [0x54] Completion controller configuration for the queue */
	uint32_t comp_cfg;
	/* [0x58] Completion controller configuration for the queue */
	uint32_t comp_cfg_2;
	/* [0x5c] Packet handler configuration */
	uint32_t pkt_cfg;
	/* [0x60] Queue QoS configuration */
	uint32_t qos_cfg;
	/* [0x64] DMB software control */
	uint32_t q_sw_ctrl;
	/* [0x68] Number of S2M Rx packets after completion  */
	uint32_t q_rx_pkt;
	uint32_t rsrvd[997];
};

struct udma_s2m_regs {
	uint32_t rsrvd_0[64];
	struct udma_axi_s2m axi_s2m;                     /* [0x100] */
	struct udma_s2m s2m;                             /* [0x200] */
	struct udma_s2m_rd s2m_rd;                       /* [0x300] */
	struct udma_s2m_wr s2m_wr;                       /* [0x340] */
	struct udma_s2m_comp s2m_comp;                   /* [0x380] */
	uint32_t rsrvd_1[80];
	struct udma_s2m_stat s2m_stat;                   /* [0x500] */
	struct udma_s2m_feature s2m_feature;             /* [0x600] */
	uint32_t rsrvd_2[576];
	struct udma_s2m_q s2m_q[4];                      /* [0x1000] */
};

/*
* Registers Fields
*/

/**** desc_wr_cfg_1 register ****/
/*
 * Defines the maximum number of AXI beats for a single AXI burst. This value is
 * used for the burst split decision.
 */
#define UDMA_AXI_S2M_DESC_WR_CFG_1_MAX_AXI_BEATS_MASK 0x000000FF
#define UDMA_AXI_S2M_DESC_WR_CFG_1_MAX_AXI_BEATS_SHIFT 0
/*
 * Minimum burst for writing completion descriptors.
 * (AXI beats).
 * Value must be aligned to cache lines (64 bytes).
 * Default value is 2 cache lines, 8 beats.
 */
#define UDMA_AXI_S2M_DESC_WR_CFG_1_MIN_AXI_BEATS_MASK 0x00FF0000
#define UDMA_AXI_S2M_DESC_WR_CFG_1_MIN_AXI_BEATS_SHIFT 16

/**** state register ****/

/*
 * Minimum descriptor burst size when prefetch FIFO level is above the
 * descriptor prefetch threshold
 */
#define UDMA_S2M_RD_DESC_PREF_CFG_3_MIN_BURST_ABOVE_THR_MASK 0x000000F0
#define UDMA_S2M_RD_DESC_PREF_CFG_3_MIN_BURST_ABOVE_THR_SHIFT 4
/*
 * Descriptor fetch threshold.
 * Used as a threshold to determine the allowed minimum descriptor burst size.
 * (Must be at least "max_desc_per_pkt")
 */
#define UDMA_S2M_RD_DESC_PREF_CFG_3_PREF_THR_MASK 0x0000FF00
#define UDMA_S2M_RD_DESC_PREF_CFG_3_PREF_THR_SHIFT 8

/**** cfg_1c register ****/
/*
 * Completion descriptor size.
 * (words)
 */
#define UDMA_S2M_COMP_CFG_1C_DESC_SIZE_MASK 0x0000000F

/* Disables the completion coalescing function. */
#define UDMA_S2M_Q_COMP_CFG_DIS_COMP_COAL (1 << 1)

#endif /* __AL_HAL_UDMA_S2M_REG_H */

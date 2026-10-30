/* SPDX-License-Identifier: GPL-2.0-only OR BSD-2-Clause */
/* Copyright (C) 2013 Annapurna Labs Ltd. */

/**
 * @file   al_hal_udma_regs_m2s.h
 *
 * @brief C Header file for the UDMA M2S registers
 *
 */

#ifndef __AL_HAL_UDMA_M2S_REG_H
#define __AL_HAL_UDMA_M2S_REG_H

#include <mach/al_hal_plat_types.h>

/*
* Unit Registers
*/

struct udma_axi_m2s {
	/* [0x0] Completion write master configuration */
	uint32_t comp_wr_cfg_1;
	/* [0x4] Completion write master configuration */
	uint32_t comp_wr_cfg_2;
	/* [0x8] Data read master configuration */
	uint32_t data_rd_cfg_1;
	/* [0xc] Data read master configuration */
	uint32_t data_rd_cfg_2;
	/* [0x10] Descriptor read master configuration */
	uint32_t desc_rd_cfg_1;
	/* [0x14] Descriptor read master configuration */
	uint32_t desc_rd_cfg_2;
	/* [0x18] Data read master configuration */
	uint32_t data_rd_cfg;
	/* [0x1c] Descriptors read master configuration */
	uint32_t desc_rd_cfg_3;
	/* [0x20] Descriptors write master configuration (completion) */
	uint32_t desc_wr_cfg_1;
	/* [0x24] AXI outstanding  configuration */
	uint32_t ostand_cfg;
	uint32_t rsrvd[54];
};
struct udma_m2s {
	/*
	 * [0x0] DMA state.
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
	 * [0xc] M2S DMA error log mask.
	 * Each error has an interrupt controller cause bit.
	 * This register determines if these errors cause the M2S DMA to log the
	 * error condition.
	 * 0 - Log is enabled.
	 * 1 - Log is masked.
	 */
	uint32_t err_log_mask;
	uint32_t rsrvd_1;
	/*
	 * [0x14] DMA header log.
	 * Sample the packet header that caused the error.
	 */
	uint32_t log_0;
	/*
	 * [0x18] DMA header log.
	 * Sample the packet header that caused the error.
	 */
	uint32_t log_1;
	/*
	 * [0x1c] DMA header log.
	 * Sample the packet header that caused the error.
	 */
	uint32_t log_2;
	/*
	 * [0x20] DMA header log.
	 * Sample the packet header that caused the error.
	 */
	uint32_t log_3;
	/* [0x24] DMA clear error log */
	uint32_t clear_err_log;
	/* [0x28] M2S data FIFO status */
	uint32_t data_fifo_status;
	/* [0x2c] M2S header FIFO status */
	uint32_t header_fifo_status;
	/* [0x30] M2S unack FIFO status */
	uint32_t unack_fifo_status;
	/* [0x34] Select queue for debug */
	uint32_t indirect_ctrl;
	/*
	 * [0x38] M2S prefetch FIFO status.
	 * Status of the selected queue in M2S_indirect_ctrl
	 */
	uint32_t sel_pref_fifo_status;
	/*
	 * [0x3c] M2S completion FIFO status.
	 * Status of the selected queue in M2S_indirect_ctrl
	 */
	uint32_t sel_comp_fifo_status;
	/*
	 * [0x40] M2S rate limit status.
	 * Status of the selected queue in M2S_indirect_ctrl
	 */
	uint32_t sel_rate_limit_status;
	/*
	 * [0x44] M2S DWRR scheduler status.
	 * Status of the selected queue in M2S_indirect_ctrl
	 */
	uint32_t sel_dwrr_status;
	/* [0x48] M2S state machine and FIFO clear control */
	uint32_t clear_ctrl;
	/* [0x4c] Misc Check enable */
	uint32_t check_en;
	/* [0x50] M2S FIFO enable control, internal */
	uint32_t fifo_en;
	/* [0x54] M2S packet length configuration */
	uint32_t cfg_len;
	/* [0x58] Stream interface configuration */
	uint32_t stream_cfg;
	uint32_t rsrvd[41];
};
struct udma_m2s_rd {
	/* [0x0] M2S descriptor prefetch configuration */
	uint32_t desc_pref_cfg_1;
	/* [0x4] M2S descriptor prefetch configuration */
	uint32_t desc_pref_cfg_2;
	/* [0x8] M2S descriptor prefetch configuration */
	uint32_t desc_pref_cfg_3;
	uint32_t rsrvd_0;
	/* [0x10] Data burst read configuration */
	uint32_t data_cfg;
	uint32_t rsrvd[11];
};
struct udma_m2s_dwrr {
	/* [0x0] Tx DMA DWRR scheduler configuration */
	uint32_t cfg_sched;
	/* [0x4] Token bucket rate limit control */
	uint32_t ctrl_deficit_cnt;
	uint32_t rsrvd[14];
};
struct udma_m2s_rate_limiter {
	/* [0x0] Token bucket rate limit configuration */
	uint32_t gen_cfg;
	/*
	 * [0x4] Token bucket rate limit control.
	 * Controls the cycle counters.
	 */
	uint32_t ctrl_cycle_cnt;
	/*
	 * [0x8] Token bucket rate limit control.
	 * Controls the token bucket counter.
	 */
	uint32_t ctrl_token;
	uint32_t rsrvd[13];
};

struct udma_rlimit_common {
	/* [0x0] Token bucket configuration */
	uint32_t cfg_1s;
	/* [0x4] Token bucket rate limit configuration */
	uint32_t cfg_cycle;
	/* [0x8] Token bucket rate limit configuration */
	uint32_t cfg_token_size_1;
	/* [0xc] Token bucket rate limit configuration */
	uint32_t cfg_token_size_2;
	/* [0x10] Token bucket rate limit configuration */
	uint32_t sw_ctrl;
	/*
	 * [0x14] Mask the different types of rate limiter.
	 * 0 - Rate limit is active.
	 * 1 - Rate limit is masked.
	 */
	uint32_t mask;
};

struct udma_m2s_stream_rate_limiter {
	struct udma_rlimit_common rlimit;
	uint32_t rsrvd[10];
};
struct udma_m2s_comp {
	/* [0x0] Completion controller configuration */
	uint32_t cfg_1c;
	/* [0x4] Completion controller coalescing configuration */
	uint32_t cfg_coal;
	/* [0x8] Completion controller application acknowledge configuration */
	uint32_t cfg_application_ack;
	uint32_t rsrvd[61];
};
struct udma_m2s_stat {
	/* [0x0] Statistics counters configuration */
	uint32_t cfg_st;
	/* [0x4] Counting number of descriptors with First-bit set. */
	uint32_t tx_pkt;
	/*
	 * [0x8] Counting the net length of the data buffers [64-bit]
	 * Should be read before tx_bytes_high
	 */
	uint32_t tx_bytes_low;
	/*
	 * [0xc] Counting the net length of the data buffers [64-bit],
	 * Should be read after tx_bytes_low (value is sampled when reading
	 * Should be read before tx_bytes_low
	 */
	uint32_t tx_bytes_high;
	/* [0x10] Total number of descriptors read from the host memory */
	uint32_t prefed_desc;
	/* [0x14] Number of packets read from the unack FIFO */
	uint32_t comp_pkt;
	/* [0x18] Number of descriptors written into the completion ring */
	uint32_t comp_desc;
	/*
	 * [0x1c] Number of acknowledged packets.
	 * (acknowledge received from the stream interface)
	 */
	uint32_t ack_pkts;
	uint32_t rsrvd[56];
};
struct udma_m2s_feature {
	/*
	 * [0x0] M2S Feature register.
	 * M2S instantiation parameters
	 */
	uint32_t reg_1;
	/* [0x4] Reserved M2S feature register */
	uint32_t reg_2;
	/*
	 * [0x8] M2S Feature register.
	 * M2S instantiation parameters
	 */
	uint32_t reg_3;
	/*
	 * [0xc] M2S Feature register.
	 * M2S instantiation parameters
	 */
	uint32_t reg_4;
	/*
	 * [0x10] M2S Feature register.
	 * M2S instantiation parameters
	 */
	uint32_t reg_5;
	uint32_t rsrvd[59];
};
struct udma_m2s_q {
	uint32_t rsrvd_0[8];
	/* [0x20] M2S descriptor ring configuration */
	uint32_t cfg;
	/* [0x24] M2S descriptor ring status and information */
	uint32_t status;
	/* [0x28] TX Descriptor Ring Base Pointer [31:4] */
	uint32_t tdrbp_low;
	/* [0x2c] TX Descriptor Ring Base Pointer [63:32] */
	uint32_t tdrbp_high;
	/*
	 * [0x30] TX Descriptor Ring Length[23:2]
	 */
	uint32_t tdrl;
	/* [0x34] TX Descriptor Ring Head Pointer */
	uint32_t tdrhp;
	/* [0x38] Tx Descriptor Tail Pointer increment */
	uint32_t tdrtp_inc;
	/* [0x3c] Tx Descriptor Tail Pointer */
	uint32_t tdrtp;
	/* [0x40] TX Descriptor Current Pointer */
	uint32_t tdcp;
	/* [0x44] Tx Completion Ring Base Pointer [31:4] */
	uint32_t tcrbp_low;
	/* [0x48] TX Completion Ring Base Pointer [63:32] */
	uint32_t tcrbp_high;
	/* [0x4c] TX Completion Ring Head Pointer */
	uint32_t tcrhp;
	/*
	 * [0x50] Tx Completion Ring Head Pointer internal (Before the
	 * coalescing FIFO)
	 */
	uint32_t tcrhp_internal;
	uint32_t rsrvd_1[3];
	/* [0x60] Rate limit configuration */
	struct udma_rlimit_common rlimit;
	uint32_t rsrvd_2[2];
	/* [0x80] DWRR scheduler configuration */
	uint32_t dwrr_cfg_1;
	/* [0x84] DWRR scheduler configuration */
	uint32_t dwrr_cfg_2;
	/* [0x88] DWRR scheduler configuration */
	uint32_t dwrr_cfg_3;
	/* [0x8c] DWRR scheduler software control */
	uint32_t dwrr_sw_ctrl;
	uint32_t rsrvd_3[4];
	/* [0xa0] Completion controller configuration */
	uint32_t comp_cfg;
	uint32_t rsrvd_4[3];
	/* [0xb0] SW control  */
	uint32_t q_sw_ctrl;
	uint32_t rsrvd_5[3];
	/* [0xc0] Number of M2S Tx packets after the scheduler */
	uint32_t q_tx_pkt;
	uint32_t rsrvd[975];
};

struct udma_m2s_regs {
	uint32_t rsrvd_0[64];
	struct udma_axi_m2s axi_m2s;                     /* [0x100] */
	struct udma_m2s m2s;                             /* [0x200] */
	struct udma_m2s_rd m2s_rd;                       /* [0x300] */
	struct udma_m2s_dwrr m2s_dwrr;                   /* [0x340] */
	struct udma_m2s_rate_limiter m2s_rate_limiter;   /* [0x380] */
	struct udma_m2s_stream_rate_limiter m2s_stream_rate_limiter; /* [0x3c0] */
	struct udma_m2s_comp m2s_comp;                   /* [0x400] */
	struct udma_m2s_stat m2s_stat;                   /* [0x500] */
	struct udma_m2s_feature m2s_feature;             /* [0x600] */
	uint32_t rsrvd_1[576];
	struct udma_m2s_q m2s_q[4];                      /* [0x1000] */
};

/*
* Registers Fields
*/

/**** desc_wr_cfg_1 register ****/
/*
 * Defines the maximum number of AXI beats for a single AXI burst.
 * This value is used for a burst split decision.
 */
#define UDMA_AXI_M2S_DESC_WR_CFG_1_MAX_AXI_BEATS_MASK 0x000000FF
#define UDMA_AXI_M2S_DESC_WR_CFG_1_MAX_AXI_BEATS_SHIFT 0

/**** state register ****/
/* Completion control */
#define UDMA_M2S_STATE_COMP_CTRL_MASK 0x00000003
#define UDMA_M2S_STATE_COMP_CTRL_SHIFT 0
/* Stream interface */
#define UDMA_M2S_STATE_STREAM_IF_MASK 0x00000030
#define UDMA_M2S_STATE_STREAM_IF_SHIFT 4
/* Data read control */
#define UDMA_M2S_STATE_DATA_RD_CTRL_MASK 0x00000300
#define UDMA_M2S_STATE_DATA_RD_CTRL_SHIFT 8
/* Descriptor prefetch */
#define UDMA_M2S_STATE_DESC_PREF_MASK 0x00003000
#define UDMA_M2S_STATE_DESC_PREF_SHIFT 12

/**** change_state register ****/
/* Start normal operation */
#define UDMA_M2S_CHANGE_STATE_NORMAL (1 << 0)
/* Stop normal operation */
#define UDMA_M2S_CHANGE_STATE_DIS    (1 << 1)
/*
 * Stop all machines.
 * (Prefetch, scheduling, completion and stream interface)
 */
#define UDMA_M2S_CHANGE_STATE_ABORT  (1 << 2)

/**** cfg_len register ****/
/* Maximum packet size for the M2S */
#define UDMA_M2S_CFG_LEN_MAX_PKT_SIZE_MASK 0x000FFFFF
/*
 * Length encoding for 64K.
 * 0 - length 0x0000 = 0
 * 1 - length 0x0000 = 64k
 */
#define UDMA_M2S_CFG_LEN_ENCODE_64K  (1 << 24)

/**** desc_pref_cfg_2 register ****/
/* Maximum number of descriptors per packet */
#define UDMA_M2S_RD_DESC_PREF_CFG_2_MAX_DESC_PER_PKT_MASK 0x0000001F
#define UDMA_M2S_RD_DESC_PREF_CFG_2_MAX_DESC_PER_PKT_SHIFT 0

/*
 * Minimum descriptor burst size when prefetch FIFO level is above the
 * descriptor prefetch threshold
 */
#define UDMA_M2S_RD_DESC_PREF_CFG_3_MIN_BURST_ABOVE_THR_MASK 0x000000F0
#define UDMA_M2S_RD_DESC_PREF_CFG_3_MIN_BURST_ABOVE_THR_SHIFT 4
/*
 * Descriptor fetch threshold.
 * Used as a threshold to determine the allowed minimum descriptor burst size.
 * (Must be at least max_desc_per_pkt)
 */
#define UDMA_M2S_RD_DESC_PREF_CFG_3_PREF_THR_MASK 0x0000FF00
#define UDMA_M2S_RD_DESC_PREF_CFG_3_PREF_THR_SHIFT 8

/*
 * Enable operation of this queue.
 * Start prefetch.
 */
#define UDMA_M2S_Q_CFG_EN_PREF       (1 << 16)
/*
 * Enable operation of this queue.
 * Start scheduling.
 */
#define UDMA_M2S_Q_CFG_EN_SCHEDULING (1 << 17)

/*
 * prefetch status
 * 0 – prefetch operation is stopped
 * 1 – prefetch is operational
 */
#define UDMA_M2S_Q_STATUS_PREFETCH   (1 << 28)
/*
 * Queue scheduler status
 * 0 – queue is not active and not participating in scheduling
 * 1 – queue is active and participating in the scheduling process
 */
#define UDMA_M2S_Q_STATUS_SCHEDULER  (1 << 29)
/*
 * M2S Descriptor Ring Base address [31:4].
 * Value of the base address of the M2S descriptor ring
 * [3:0] - 0 - 16B alignment is enforced
 * ([11:4] should be 0 for 4KB alignment)
 */
#define UDMA_M2S_Q_TDRBP_LOW_ADDR_MASK 0xFFFFFFF0

/*
 * M2S Descriptor Ring Base address [31:4].
 * Value of the base address of the M2S descriptor ring
 * [3:0] - 0 - 16B alignment is enforced
 * ([11:4] should be 0 for 4KB alignment)
 * NOTE:
 * Length of the descriptor ring (in descriptors) associated with the ring base
 * address. Ends at maximum burst size alignment.
 */
#define UDMA_M2S_Q_TCRBP_LOW_ADDR_MASK 0xFFFFFFF0

/*
 * Mask the internal pause mechanism for DMB.
 * (Data Memory Barrier).
 */
#define UDMA_M2S_Q_RATE_LIMIT_MASK_INTERNAL_PAUSE_DMB (1 << 2)

/**** comp_cfg register ****/
/* Enable writing to the completion ring */
#define UDMA_M2S_Q_COMP_CFG_EN_COMP_RING_UPDATE (1 << 0)
/* Disable the completion coalescing function. */
#define UDMA_M2S_Q_COMP_CFG_DIS_COMP_COAL (1 << 1)

/* Reset the queue */
#define UDMA_M2S_Q_SW_CTRL_RST_Q   (1 << 8)

#endif /* __AL_HAL_UDMA_M2S_REG_H */

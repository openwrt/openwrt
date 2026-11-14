/* SPDX-License-Identifier: GPL-2.0-only OR BSD-2-Clause */
/* Copyright (C) 2013 Annapurna Labs Ltd. */

/**
 * @file   al_hal_udma_regs_gen.h
 *
 * @brief C Header file for the UDMA general registers
 *
 */

#ifndef __AL_HAL_UDMA_GEN_REG_H
#define __AL_HAL_UDMA_GEN_REG_H

#include <mach/al_hal_udma_iofic_regs.h>

/*
* Unit Registers
*/

struct udma_gen_dma_misc {
	/* [0x0] Reserved register for the interrupt controller */
	uint32_t int_cfg;
	/* [0x4] Revision register */
	uint32_t revision;
	/* [0x8] Reserved for future use */
	uint32_t general_cfg_1;
	/* [0xc] Reserved for future use */
	uint32_t general_cfg_2;
	/* [0x10] Reserved for future use */
	uint32_t general_cfg_3;
	/* [0x14] Reserved for future use */
	uint32_t general_cfg_4;
	/* [0x18] General timer configuration */
	uint32_t general_cfg_5;
	uint32_t rsrvd[57];
};
struct udma_gen_mailbox {
	/*
	 * [0x0] Mailbox interrupt generator.
	 * Generates interrupt to neighbor DMA
	 */
	uint32_t interrupt;
	/* [0x4] Mailbox message data out */
	uint32_t msg_out;
	/* [0x8] Mailbox message data in */
	uint32_t msg_in;
	uint32_t rsrvd[13];
};
struct udma_gen_axi {
	/* [0x0] Configuration of the AXI masters */
	uint32_t cfg_1;
	/* [0x4] Configuration of the AXI masters */
	uint32_t cfg_2;
	/* [0x8] Configuration of the AXI masters. Endianess configuration */
	uint32_t endian_cfg;
	uint32_t rsrvd[61];
};
struct udma_gen_sram_ctrl {
	/* [0x0] Timing configuration */
	uint32_t timing;
};
struct udma_gen_vmid {
	/* [0x0] VMID control */
	uint32_t cfg_vmid_0;
	/* [0x4] TX queue 0/1 VMID */
	uint32_t cfg_vmid_1;
	/* [0x8] TX queue 2/3 VMID */
	uint32_t cfg_vmid_2;
	/* [0xc] RX queue 0/1 VMID */
	uint32_t cfg_vmid_3;
	/* [0x10] RX queue 2/3 VMID */
	uint32_t cfg_vmid_4;
};
struct udma_gen_vmaddr {
	/* [0x0] TX queue 0/1 VMADDR */
	uint32_t cfg_vmaddr_0;
	/* [0x4] TX queue 2/3 VMADDR */
	uint32_t cfg_vmaddr_1;
	/* [0x8] RX queue 0/1 VMADDR */
	uint32_t cfg_vmaddr_2;
	/* [0xc] RX queue 2/3 VMADDR */
	uint32_t cfg_vmaddr_3;
};
struct udma_gen_vmpr {
	/* [0x0] TX VMPR control */
	uint32_t cfg_vmpr_0;
	/* [0x4] TX VMPR Address High Regsiter */
	uint32_t cfg_vmpr_1;
	/* [0x8] TX queue VMID values */
	uint32_t cfg_vmpr_2;
	/* [0xc] TX queue VMID values */
	uint32_t cfg_vmpr_3;
	/* [0x10] RX VMPR control */
	uint32_t cfg_vmpr_4;
	/* [0x14] RX VMPR Buffer2 MSB address */
	uint32_t cfg_vmpr_5;
	/* [0x18] RX queue VMID values */
	uint32_t cfg_vmpr_6;
	/* [0x1c] RX queue BUF1 VMID values */
	uint32_t cfg_vmpr_7;
	/* [0x20] RX queue BUF2 VMID values */
	uint32_t cfg_vmpr_8;
	/* [0x24] RX queue Direct Data Placement VMID values */
	uint32_t cfg_vmpr_9;
	/* [0x28] RX VMPR BUF1 Address High Regsiter */
	uint32_t cfg_vmpr_10;
	/* [0x2c] RX VMPR BUF2 Address High Regsiter */
	uint32_t cfg_vmpr_11;
	/* [0x30] RX VMPR DDP Address High Regsiter */
	uint32_t cfg_vmpr_12;
	uint32_t rsrvd[3];
};

struct udma_gen_regs {
	struct udma_iofic_regs interrupt_regs;					/* [0x0000] */
	struct udma_gen_dma_misc dma_misc;                   /* [0x2080] */
	struct udma_gen_mailbox mailbox[4];                  /* [0x2180] */
	struct udma_gen_axi axi;                             /* [0x2280] */
	struct udma_gen_sram_ctrl sram_ctrl[25];             /* [0x2380] */
	uint32_t rsrvd_1[2];
	struct udma_gen_vmid vmid;                           /* [0x23ec] */
	struct udma_gen_vmaddr vmaddr;                       /* [0x2400] */
	uint32_t rsrvd_2[252];
	struct udma_gen_vmpr vmpr[4];                        /* [0x2800] */
};

/*
* Registers Fields
*/

/**** cfg_vmpr_0 register ****/
/* TX High Address Select Per Q */
#define UDMA_GEN_VMPR_CFG_VMPR_0_TX_Q_HISEL_MASK 0x0000003F
/* TX Data VMID Enable Per Q */
#define UDMA_GEN_VMPR_CFG_VMPR_0_TX_Q_DATA_VMID_EN (1 << 7)
/* TX Prefetch VMID Enable Per Q */
#define UDMA_GEN_VMPR_CFG_VMPR_0_TX_Q_PREF_VMID_EN (1 << 28)
/* TX Completions VMID Enable Per Q */
#define UDMA_GEN_VMPR_CFG_VMPR_0_TX_Q_CMPL_VMID_EN (1 << 29)

/**** cfg_vmpr_2 register ****/
/* TX queue Prefetch VMID */
#define UDMA_GEN_VMPR_CFG_VMPR_2_TX_Q_PREF_VMID_MASK 0x0000FFFF
#define UDMA_GEN_VMPR_CFG_VMPR_2_TX_Q_PREF_VMID_SHIFT 0
/* TX queue Completion VMID */
#define UDMA_GEN_VMPR_CFG_VMPR_2_TX_Q_CMPL_VMID_MASK 0xFFFF0000
#define UDMA_GEN_VMPR_CFG_VMPR_2_TX_Q_CMPL_VMID_SHIFT 16

/**** cfg_vmpr_3 register ****/
/* TX queue Data VMID */
#define UDMA_GEN_VMPR_CFG_VMPR_3_TX_Q_DATA_VMID_MASK 0x0000FFFF
#define UDMA_GEN_VMPR_CFG_VMPR_3_TX_Q_DATA_VMID_SHIFT 0
/* TX queue Data VMID select */
#define UDMA_GEN_VMPR_CFG_VMPR_3_TX_Q_DATA_VMID_SEL_MASK 0xFFFF0000
#define UDMA_GEN_VMPR_CFG_VMPR_3_TX_Q_DATA_VMID_SEL_SHIFT 16

/**** cfg_vmpr_4 register ****/
/* RX Data Buffer1 - High Address Select Per Q */
#define UDMA_GEN_VMPR_CFG_VMPR_4_RX_Q_BUF1_HISEL_MASK 0x0000003F
#define UDMA_GEN_VMPR_CFG_VMPR_4_RX_Q_BUF1_HISEL_SHIFT 0
/* RX Data Buffer1 VMID Enable Per Q */
#define UDMA_GEN_VMPR_CFG_VMPR_4_RX_Q_BUF1_VMID_EN (1 << 7)
/* RX Data Buffer2 - High Address Select Per Q */
#define UDMA_GEN_VMPR_CFG_VMPR_4_RX_Q_BUF2_HISEL_MASK 0x00003F00
#define UDMA_GEN_VMPR_CFG_VMPR_4_RX_Q_BUF2_HISEL_SHIFT 8
/* RX Data Buffer2 VMID Enable Per Q */
#define UDMA_GEN_VMPR_CFG_VMPR_4_RX_Q_BUF2_VMID_EN (1 << 15)
/* RX Direct Data Placement - High Address Select Per Q */
#define UDMA_GEN_VMPR_CFG_VMPR_4_RX_Q_DDP_HISEL_MASK 0x003F0000
#define UDMA_GEN_VMPR_CFG_VMPR_4_RX_Q_DDP_HISEL_SHIFT 16
/* RX Direct Data Placement VMID Enable Per Q */
#define UDMA_GEN_VMPR_CFG_VMPR_4_RX_Q_DDP_VMID_EN (1 << 23)
/* RX Prefetch VMID Enable Per Q */
#define UDMA_GEN_VMPR_CFG_VMPR_4_RX_Q_PREF_VMID_EN (1 << 28)
/* RX Completions VMID Enable Per Q */
#define UDMA_GEN_VMPR_CFG_VMPR_4_RX_Q_CMPL_VMID_EN (1 << 29)

/**** cfg_vmpr_6 register ****/
/* RX queue Prefetch VMID */
#define UDMA_GEN_VMPR_CFG_VMPR_6_RX_Q_PREF_VMID_MASK 0x0000FFFF
#define UDMA_GEN_VMPR_CFG_VMPR_6_RX_Q_PREF_VMID_SHIFT 0
/* RX queue Completion VMID */
#define UDMA_GEN_VMPR_CFG_VMPR_6_RX_Q_CMPL_VMID_MASK 0xFFFF0000
#define UDMA_GEN_VMPR_CFG_VMPR_6_RX_Q_CMPL_VMID_SHIFT 16

/**** cfg_vmpr_7 register ****/
/* RX queue Data Buffer 1 VMID */
#define UDMA_GEN_VMPR_CFG_VMPR_7_RX_Q_BUF1_VMID_MASK 0x0000FFFF
#define UDMA_GEN_VMPR_CFG_VMPR_7_RX_Q_BUF1_VMID_SHIFT 0
/* RX queue Data Buffer 1 VMID select */
#define UDMA_GEN_VMPR_CFG_VMPR_7_RX_Q_BUF1_VMID_SEL_MASK 0xFFFF0000
#define UDMA_GEN_VMPR_CFG_VMPR_7_RX_Q_BUF1_VMID_SEL_SHIFT 16

/**** cfg_vmpr_8 register ****/
/* RX queue Data Buffer 2 VMID */
#define UDMA_GEN_VMPR_CFG_VMPR_8_RX_Q_BUF2_VMID_MASK 0x0000FFFF
#define UDMA_GEN_VMPR_CFG_VMPR_8_RX_Q_BUF2_VMID_SHIFT 0
/* RX queue Data Buffer 2 VMID select */
#define UDMA_GEN_VMPR_CFG_VMPR_8_RX_Q_BUF2_VMID_SEL_MASK 0xFFFF0000
#define UDMA_GEN_VMPR_CFG_VMPR_8_RX_Q_BUF2_VMID_SEL_SHIFT 16

/**** cfg_vmpr_9 register ****/
/* RX queue DDP VMID */
#define UDMA_GEN_VMPR_CFG_VMPR_9_RX_Q_DDP_VMID_MASK 0x0000FFFF
#define UDMA_GEN_VMPR_CFG_VMPR_9_RX_Q_DDP_VMID_SHIFT 0
/* RX queue DDP VMID select */
#define UDMA_GEN_VMPR_CFG_VMPR_9_RX_Q_DDP_VMID_SEL_MASK 0xFFFF0000
#define UDMA_GEN_VMPR_CFG_VMPR_9_RX_Q_DDP_VMID_SEL_SHIFT 16

#endif /* __AL_HAL_UDMA_GEN_REG_H */

/* SPDX-License-Identifier: GPL-2.0-only OR BSD-2-Clause */
/* Copyright (C) 2013 Annapurna Labs Ltd. */

#ifndef __AL_HAL_NAND_REGS_H__
#define __AL_HAL_NAND_REGS_H__

struct al_nand_regs {
	uint32_t rsrvd0[0x44 / sizeof(uint32_t)];
	uint32_t flash_ctl_3;				/* 0x0044 */
	uint32_t rsrvd1[(0x400 - 0x48) / sizeof(uint32_t)];
	uint32_t mode_select_reg;			/* 0x0400 */
	uint32_t ctl_reg0;				/* 0x0404 */
	uint32_t bch_ctrl_reg_0;			/* 0x0408 */
	uint32_t bch_ctrl_reg_1;			/* 0x040c */
	uint32_t command_reg;				/* 0x0410 */
	uint32_t nflash_row_addr;			/* 0x0414 */
	uint32_t nflash_col_addr;			/* 0x0418 */
	uint32_t codeword_size_cnt_reg;			/* 0x041c */
	uint32_t command_buffer_reg;			/* 0x0420 */
	uint32_t data_buffer_reg;			/* 0x0424 */
	uint32_t nflash_spare_offset;			/* 0x0428 */
	uint32_t sdr_timing_params_0;			/* 0x042c */
	uint32_t sdr_timing_params_1;			/* 0x0430 */
	uint32_t ddr_timing_params_0;			/* 0x0434 */
	uint32_t ddr_timing_params_1;			/* 0x0438 */
	uint32_t ddr2_timing_params_0;			/* 0x043c */
	uint32_t ddr2_timing_params_1;			/* 0x0440 */
	uint32_t rdy_busy_wait_cnt_reg;			/* 0x0444 */
	uint32_t nfc_int_en;				/* 0x0448 */
	uint32_t rdy_busy_status_reg;			/* 0x044c */
	uint32_t nfc_int_stat;				/* 0x0450 */
	uint32_t flash_status_reg;			/* 0x0454 */
	uint32_t ddr_timing_params_2;			/* 0x0458 */
	uint32_t ddr2_timing_params_2;			/* 0x045c */
	uint32_t reset_reg;				/* 0x0460 */
	uint32_t xip_ctrl_reg_0;			/* 0x0464 */
	uint32_t xip_ctrl_reg_1;			/* 0x0468 */
	uint32_t xip_ctrl_reg_2;			/* 0x046c */
	uint32_t clk_select_reg;			/* 0x0470 */
	uint32_t bch_status_reg;			/* 0x0474 */
	uint32_t dma_sector_addr_reg;			/* 0x0478 */
	uint32_t dma_addr_reg;				/* 0x047c */
	uint32_t dma_ctrl_reg;				/* 0x0480 */
	uint32_t xip_ctrl_reg_3;			/* 0x0484 */
	uint32_t reset_status_reg;			/* 0x048c */
};

struct al_nand_wrap_regs {
	uint32_t rsrvd0;
	uint32_t code_word_size;			/* 0x0004 */
};

#define AL_NAND_FLASH_CTL_3_FLASH_TYPE_BNK_SHIFT(idx)	\
	((idx) * 2)
#define AL_NAND_FLASH_CTL_3_FLASH_TYPE_BNK_VAL_NUM_BNKS	6
#define AL_NAND_FLASH_CTL_3_FLASH_TYPE_BNK_VAL_NAND		1

/* Register fields: mode_select_reg */
#define AL_NAND_MODE_SELECT_MODE_SELECT_MASK		AL_FIELD_MASK(2, 0)
#define AL_NAND_MODE_SELECT_MODE_SELECT_SHIFT		0

#define AL_NAND_MODE_SELECT_SDR				0

#define AL_NAND_MODE_SELECT_SDR_TIM_MODE_MASK		AL_FIELD_MASK(6, 3)
#define AL_NAND_MODE_SELECT_SDR_TIM_MODE_SHIFT		3

/* Register fields: ctl_reg0 */
#define AL_NAND_CTL_REG0_CS1_MASK			AL_FIELD_MASK(2, 0)
#define AL_NAND_CTL_REG0_CS1_SHIFT			0

#define AL_NAND_CTL_REG0_WP				3

#define AL_NAND_CTL_REG0_DQ_WIDTH			4

#define AL_NAND_CTL_REG0_COL_ADDR_CYCLES_MASK		AL_FIELD_MASK(8, 5)
#define AL_NAND_CTL_REG0_COL_ADDR_CYCLES_SHIFT		5

#define AL_NAND_CTL_REG0_ROW_ADDR_CYCLES_MASK		AL_FIELD_MASK(12, 9)
#define AL_NAND_CTL_REG0_ROW_ADDR_CYCLES_SHIFT		9

#define AL_NAND_CTL_REG0_PAGE_SIZE_MASK			AL_FIELD_MASK(15, 13)
#define AL_NAND_CTL_REG0_PAGE_SIZE_SHIFT		13

#define AL_NAND_CTL_REG0_TX_MODE			16

#define AL_NAND_CTL_REG0_CS2_MASK			AL_FIELD_MASK(21, 19)
#define AL_NAND_CTL_REG0_CS2_SHIFT			19

/* Register fields: bch_ctrl_reg_0 */
#define AL_NAND_BCH_CTRL_REG_0_ECC_ON_OFF		0

#define AL_NAND_BCH_CTRL_REG_0_ECC_ALGORITHM		1

#define AL_NAND_BCH_CTRL_REG_0_BCH_T_MASK		AL_FIELD_MASK(5, 2)
#define AL_NAND_BCH_CTRL_REG_0_BCH_T_SHIFT		2

#define AL_NAND_BCH_CTRL_REG_0_BCH_N_MASK		AL_FIELD_MASK(21, 6)
#define AL_NAND_BCH_CTRL_REG_0_BCH_N_SHIFT		6

/* Register fields: bch_ctrl_reg_1 */
#define AL_NAND_BCH_CTRL_REG_1_BCH_K_MASK		AL_FIELD_MASK(12, 0)
#define AL_NAND_BCH_CTRL_REG_1_BCH_K_SHIFT		0

/* Register fields: codeword_size_cnt_reg */
#define AL_NAND_CODEWORD_SIZE_CNT_REG_SIZE_MASK		AL_FIELD_MASK(15, 0)
#define AL_NAND_CODEWORD_SIZE_CNT_REG_SIZE_SHIFT	0

#define AL_NAND_CODEWORD_SIZE_CNT_REG_COUNT_MASK	AL_FIELD_MASK(31, 16)
#define AL_NAND_CODEWORD_SIZE_CNT_REG_COUNT_SHIFT	16

#define AL_NAND_SDR_TIM_PARAMS_0_T_SETUP_SHIFT		0

#define AL_NAND_SDR_TIM_PARAMS_0_T_HOLD_SHIFT		6

#define AL_NAND_SDR_TIM_PARAMS_0_T_WH_SHIFT		12

#define AL_NAND_SDR_TIM_PARAMS_0_T_WRP_SHIFT		18

#define AL_NAND_SDR_TIM_PARAMS_0_T_INTCMD_SHIFT		24

#define AL_NAND_SDR_TIM_PARAMS_1_T_RR_SHIFT		0

#define AL_NAND_SDR_TIM_PARAMS_1_T_WB_MASK		AL_FIELD_MASK(11, 6)
#define AL_NAND_SDR_TIM_PARAMS_1_T_WB_SHIFT		6
#define AL_NAND_SDR_TIM_PARAMS_1_T_WB_WIDTH		6

#define AL_NAND_SDR_TIM_PARAMS_1_T_READ_DLY_SHIFT	12

#define AL_NAND_SDR_TIM_PARAMS_1_T_WB_MSB_SHIFT		14

/* Register fields: rdy_busy_wait_cnt_reg */
#define AL_NAND_RDYBSY_WAIT_CNT_REG_RDY_TOUT_CNT_MASK	AL_FIELD_MASK(15, 0)
#define AL_NAND_RDYBSY_WAIT_CNT_REG_RDY_TOUT_CNT_SHIFT	0

#define AL_NAND_RDYBSY_WAIT_CNT_REG_RDYBSYEN_MASK	AL_FIELD_MASK(16, 16)
#define AL_NAND_RDYBSY_WAIT_CNT_REG_RDYBSYEN_SHIFT	16

/* Register fields: nfc_int_stat */
#define AL_NAND_NFC_INT_STAT_CMD_BUF_EMPTY		0
#define AL_NAND_NFC_INT_STAT_CORR_ERR			4
#define AL_NAND_NFC_INT_STAT_UNCORR_ERR			5

#endif		/* __AL_HAL_NAND_REGS_H__ */

/*******************************************************************************
Copyright (C) 2013 Annapurna Labs Ltd.

This file may be licensed under the terms of the Annapurna Labs Commercial
License Agreement.

Alternatively, this file can be distributed under the terms of the GNU General
Public License V2 as published by the Free Software Foundation and can be
found at http://www.gnu.org/licenses/gpl-2.0.html

Alternatively, redistribution and use in source and binary forms, with or
without modification, are permitted provided that the following conditions are
met:

    *     Redistributions of source code must retain the above copyright notice,
	  this list of conditions and the following disclaimer.

    *     Redistributions in binary form must reproduce the above copyright
	  notice, this list of conditions and the following disclaimer in
	  the documentation and/or other materials provided with the
	  distribution.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
 ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*******************************************************************************/

#ifndef __AL_HAL_NAND_REGS_H__
#define __AL_HAL_NAND_REGS_H__

/* *INDENT-OFF* */
#ifdef __cplusplus
extern "C" {
#endif
/* *INDENT-ON* */

#define AL_NAND_CMD_FIFO_DEPTH			32

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

/* Register fields: flash_ctl_3 */
#define AL_NAND_FLASH_CTL_3_FLASH_TYPE_BNK_MASK(idx)	\
	AL_FIELD_MASK(((idx) * 2 + 1), ((idx) * 2))
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

#define AL_NAND_MODE_SELECT_DDR_TIM_MODE_MASK		AL_FIELD_MASK(10, 7)
#define AL_NAND_MODE_SELECT_DDR_TIM_MODE_SHIFT		7

#define AL_NAND_MODE_SELECT_DDR2_TIM_MODE_MASK		AL_FIELD_MASK(14, 11)
#define AL_NAND_MODE_SELECT_DDR2_TIM_MODE_SHIFT		11

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
#define AL_NAND_CTL_REG0_TX_MODE_VAL_RX			0
#define AL_NAND_CTL_REG0_TX_MODE_VAL_TX			1

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

/* Register fields: nflash_spare_offset */
#define	AL_NAND_NFLASH_SPR_OFF_SPR_OFF_MASK		AL_FIELD_MASK(15, 0)
#define	AL_NAND_NFLASH_SPR_OFF_SPR_OFF_SHIFT		0

/* Register fields: sdr_timing_params_0 */
#define AL_NAND_SDR_TIM_PARAMS_0_T_SETUP_MASK		AL_FIELD_MASK(5, 0)
#define AL_NAND_SDR_TIM_PARAMS_0_T_SETUP_SHIFT		0

#define AL_NAND_SDR_TIM_PARAMS_0_T_HOLD_MASK		AL_FIELD_MASK(11, 6)
#define AL_NAND_SDR_TIM_PARAMS_0_T_HOLD_SHIFT		6

#define AL_NAND_SDR_TIM_PARAMS_0_T_WH_MASK		AL_FIELD_MASK(17, 12)
#define AL_NAND_SDR_TIM_PARAMS_0_T_WH_SHIFT		12

#define AL_NAND_SDR_TIM_PARAMS_0_T_WRP_MASK		AL_FIELD_MASK(23, 18)
#define AL_NAND_SDR_TIM_PARAMS_0_T_WRP_SHIFT		18

#define AL_NAND_SDR_TIM_PARAMS_0_T_INTCMD_MASK		AL_FIELD_MASK(29, 24)
#define AL_NAND_SDR_TIM_PARAMS_0_T_INTCMD_SHIFT		24

/* Register fields: sdr_timing_params_1 */
#define AL_NAND_SDR_TIM_PARAMS_1_T_RR_MASK		AL_FIELD_MASK(5, 0)
#define AL_NAND_SDR_TIM_PARAMS_1_T_RR_SHIFT		0

#define AL_NAND_SDR_TIM_PARAMS_1_T_WB_MASK		AL_FIELD_MASK(11, 6)
#define AL_NAND_SDR_TIM_PARAMS_1_T_WB_SHIFT		6
#define AL_NAND_SDR_TIM_PARAMS_1_T_WB_WIDTH		6

#define AL_NAND_SDR_TIM_PARAMS_1_T_READ_DLY_MASK	AL_FIELD_MASK(13, 12)
#define AL_NAND_SDR_TIM_PARAMS_1_T_READ_DLY_SHIFT	12

#define AL_NAND_SDR_TIM_PARAMS_1_T_WB_MSB_MASK		AL_FIELD_MASK(14, 14)
#define AL_NAND_SDR_TIM_PARAMS_1_T_WB_MSB_SHIFT		14

/* Register fields: ddr_timing_params_0 */
#define AL_NAND_DDR_TIM_PARAMS_0_T_SETUP_MASK		AL_FIELD_MASK(5, 0)
#define AL_NAND_DDR_TIM_PARAMS_0_T_SETUP_SHIFT		0

#define AL_NAND_DDR_TIM_PARAMS_0_T_HOLD_MASK		AL_FIELD_MASK(11, 6)
#define AL_NAND_DDR_TIM_PARAMS_0_T_HOLD_SHIFT		6

#define AL_NAND_DDR_TIM_PARAMS_0_T_WH_MASK		AL_FIELD_MASK(17, 12)
#define AL_NAND_DDR_TIM_PARAMS_0_T_WH_SHIFT		12

#define AL_NAND_DDR_TIM_PARAMS_0_T_WRP_MASK		AL_FIELD_MASK(23, 18)
#define AL_NAND_DDR_TIM_PARAMS_0_T_WRP_SHIFT		18

#define AL_NAND_DDR_TIM_PARAMS_0_T_INTCMD_MASK		AL_FIELD_MASK(29, 24)
#define AL_NAND_DDR_TIM_PARAMS_0_T_INTCMD_SHIFT		24

/* Register fields: ddr_timing_params_1 */
#define AL_NAND_DDR_TIM_PARAMS_1_T_RR_MASK		AL_FIELD_MASK(5, 0)
#define AL_NAND_DDR_TIM_PARAMS_1_T_RR_SHIFT		0

#define AL_NAND_DDR_TIM_PARAMS_1_T_WB_MASK		AL_FIELD_MASK(11, 6)
#define AL_NAND_DDR_TIM_PARAMS_1_T_WB_SHIFT		6

#define AL_NAND_DDR_TIM_PARAMS_1_T_CLKDIV_MASK		AL_FIELD_MASK(17, 12)
#define AL_NAND_DDR_TIM_PARAMS_1_T_CLKDIV_SHIFT		12

#define AL_NAND_DDR_TIM_PARAMS_1_T_CE_N_MASK		AL_FIELD_MASK(23, 18)
#define AL_NAND_DDR_TIM_PARAMS_1_T_CE_N_SHIFT		18

#define AL_NAND_DDR_TIM_PARAMS_1_T_DQS_IN_MASK		AL_FIELD_MASK(28, 24)
#define AL_NAND_DDR_TIM_PARAMS_1_T_DQS_IN_SHIFT		24

/* Register fields: ddr2_timing_params_0 */
#define AL_NAND_DDR2_TIM_PARAMS_0_T_SETUP_MASK		AL_FIELD_MASK(5, 0)
#define AL_NAND_DDR2_TIM_PARAMS_0_T_SETUP_SHIFT		0

#define AL_NAND_DDR2_TIM_PARAMS_0_T_HOLD_MASK		AL_FIELD_MASK(11, 6)
#define AL_NAND_DDR2_TIM_PARAMS_0_T_HOLD_SHIFT		6

#define AL_NAND_DDR2_TIM_PARAMS_0_T_WH_MASK		AL_FIELD_MASK(17, 12)
#define AL_NAND_DDR2_TIM_PARAMS_0_T_WH_SHIFT		12

#define AL_NAND_DDR2_TIM_PARAMS_0_T_WRP_MASK		AL_FIELD_MASK(23, 18)
#define AL_NAND_DDR2_TIM_PARAMS_0_T_WRP_SHIFT		18

#define AL_NAND_DDR2_TIM_PARAMS_0_T_INTCMD_MASK		AL_FIELD_MASK(29, 24)
#define AL_NAND_DDR2_TIM_PARAMS_0_T_INTCMD_SHIFT	24

/* Register fields: ddr2_timing_params_1 */
#define AL_NAND_DDR2_TIM_PARAMS_1_T_RR_MASK		AL_FIELD_MASK(5, 0)
#define AL_NAND_DDR2_TIM_PARAMS_1_T_RR_SHIFT		0

#define AL_NAND_DDR2_TIM_PARAMS_1_T_WB_MASK		AL_FIELD_MASK(11, 6)
#define AL_NAND_DDR2_TIM_PARAMS_1_T_WB_SHIFT		6

#define AL_NAND_DDR2_TIM_PARAMS_1_T_CLKDIV_MASK		AL_FIELD_MASK(17, 12)
#define AL_NAND_DDR2_TIM_PARAMS_1_T_CLKDIV_SHIFT	12

#define AL_NAND_DDR2_TIM_PARAMS_1_T_CE_N_MASK		AL_FIELD_MASK(23, 18)
#define AL_NAND_DDR2_TIM_PARAMS_1_T_CE_N_SHIFT		18

#define AL_NAND_DDR2_TIM_PARAMS_1_T_DQS_IN_MASK		AL_FIELD_MASK(28, 24)
#define AL_NAND_DDR2_TIM_PARAMS_1_T_DQS_IN_SHIFT	24

/* Register fields: rdy_busy_wait_cnt_reg */
#define AL_NAND_RDYBSY_WAIT_CNT_REG_RDY_TOUT_CNT_MASK	AL_FIELD_MASK(15, 0)
#define AL_NAND_RDYBSY_WAIT_CNT_REG_RDY_TOUT_CNT_SHIFT	0

#define AL_NAND_RDYBSY_WAIT_CNT_REG_RDYBSYEN_MASK	AL_FIELD_MASK(16, 16)
#define AL_NAND_RDYBSY_WAIT_CNT_REG_RDYBSYEN_SHIFT	16

/* Register fields: rdy_busy_status_reg */
#define AL_NAND_RDYBSY_STATUS_REG_RDYBSY_STATUS_MASK	AL_FIELD_MASK(7, 0)
#define AL_NAND_RDYBSY_STATUS_REG_RDYBSY_STATUS_SHIFT	0

/* Register fields: nfc_int_stat */
#define AL_NAND_NFC_INT_STAT_CMD_BUF_EMPTY		0
#define AL_NAND_NFC_INT_STAT_CMD_BUF_FULL		1
#define AL_NAND_NFC_INT_STAT_DATA_BUF_EMPTY		2
#define AL_NAND_NFC_INT_STAT_DATA_BUF_FULL		3
#define AL_NAND_NFC_INT_STAT_CORR_ERR			4
#define AL_NAND_NFC_INT_STAT_UNCORR_ERR			5
#define AL_NAND_NFC_INT_STAT_BUF_WR_RDY			6
#define AL_NAND_NFC_INT_STAT_BUF_RD_RDY			7

/* Register fields: ddr_timing_params_2 */
#define AL_NAND_DDR_TIM_PARAMS_2_T_DQS_OUT_MASK		AL_FIELD_MASK(4, 0)
#define AL_NAND_DDR_TIM_PARAMS_2_T_DQS_OUT_SHIFT	0

/* Register fields: ddr2_timing_params_2 */
#define AL_NAND_DDR2_TIM_PARAMS_2_T_DQS_OUT_MASK	AL_FIELD_MASK(4, 0)
#define AL_NAND_DDR2_TIM_PARAMS_2_T_DQS_OUT_SHIFT	0

/* Register fields: reset_status_reg */
#define AL_NAND_RESET_STATUS_REG_TIM_ENG_RST_DN		0

#ifdef __cplusplus
}
#endif
/* *INDENT-ON* */
#endif		/* __AL_HAL_NAND_REGS_H__ */

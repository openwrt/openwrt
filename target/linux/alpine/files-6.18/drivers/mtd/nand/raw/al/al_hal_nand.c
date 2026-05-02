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

#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt

#include <mach/al_hal_pbs_regs.h>
#include "al_hal_nand.h"
#include "al_hal_nand_regs.h"
#include "al_hal_nand_coded_properties.h"

#define AL_NAND_DEVICE_CMD_PAGE_READ_FIRST	0x00
#define AL_NAND_DEVICE_CMD_PAGE_READ_SECOND	0x30
#define AL_NAND_DEVICE_CMD_CHG_READ_COL_FIRST	0x05
#define AL_NAND_DEVICE_CMD_CHG_READ_COL_SECOND	0xE0
#define AL_NAND_DEVICE_CMD_PAGE_PRG_FIRST	0x80
#define AL_NAND_DEVICE_CMD_PAGE_PRG_SECOND	0x10
#define AL_NAND_DEVICE_CMD_CHG_PRG_COL_FIRST	0x85

#define DATA_BUFF_OFFSET			0x000000
#define CMD_BUFF_OFFSET				0x100000
#define WRAP_BASE_OFFSET			0x200000
#define CTRL_BASE_OFFSET			0x201000

static int _al_nand_dev_config_modes(
	struct al_nand_ctrl_obj	*obj);

static void _al_nand_dev_config_ctl_reg_0(
	struct al_nand_ctrl_obj	*obj);

static void _al_nand_dev_config_sdr_timing_params(
	struct al_nand_ctrl_obj	*obj);

static void _al_nand_dev_config_rdy_busy_wait_cnt(
	struct al_nand_ctrl_obj	*obj);

static void _al_nand_dev_config_bch_ctl_reg_0(
	struct al_nand_ctrl_obj	*obj);

static void _al_nand_dev_config_bch_ctl_reg_1(
	struct al_nand_ctrl_obj	*obj);

static void _al_nand_data_buf_wait_for_cw_vacancy(
	struct al_nand_ctrl_obj	*obj);

static void _al_nand_cmd_seq_add_command(
	uint32_t			**cmd_seq_buff,
	int				*cmd_seq_buff_num_entries,
	enum al_nand_command_type	type,
	uint8_t				argument);

static void _al_nand_cmd_seq_add_command_cyc(
	uint32_t			**cmd_seq_buff,
	int				*cmd_seq_buff_num_entries,
	enum al_nand_command_type	type,
	int				argument,
	int				num_cyc);

static int _al_nand_get_bch_num_corr_bits(
	struct al_nand_ctrl_obj	*obj);

static int _al_nand_get_bch_cw_parity_size(
	struct al_nand_ctrl_obj	*obj);

static int _al_nand_get_cw_spare_cnt(
	struct al_nand_ctrl_obj	*obj);

/******************************************************************************/
/******************************************************************************/
int al_nand_init(
	struct al_nand_ctrl_obj	*obj,
	void __iomem		*nand_base,
	struct al_ssm_dma	*raid_dma,
	uint32_t		raid_dma_qid)
{
	int status = 0;

	uint32_t reg_val;

	al_assert(obj);
	al_assert(nand_base);

	obj->regs_base = (nand_base + CTRL_BASE_OFFSET);

	obj->wrap_regs_base = (nand_base + WRAP_BASE_OFFSET);
	obj->cmd_buff_base = (nand_base + CMD_BUFF_OFFSET);
	obj->data_buff_base = (nand_base + DATA_BUFF_OFFSET);

	obj->raid_dma = raid_dma;
	obj->raid_dma_qid = raid_dma_qid;

	obj->current_dev_index = -1;

#if (AL_NAND_FLASH_CTL_3_FLASH_TYPE_BNK_VAL_NUM_BNKS != 6)
	#error Assumption wrong!
#endif

	reg_val =
		AL_REG_BITS_FIELD(
			AL_NAND_FLASH_CTL_3_FLASH_TYPE_BNK_SHIFT(0),
			AL_NAND_FLASH_CTL_3_FLASH_TYPE_BNK_VAL_NAND) |
		AL_REG_BITS_FIELD(
			AL_NAND_FLASH_CTL_3_FLASH_TYPE_BNK_SHIFT(1),
			AL_NAND_FLASH_CTL_3_FLASH_TYPE_BNK_VAL_NAND) |
		AL_REG_BITS_FIELD(
			AL_NAND_FLASH_CTL_3_FLASH_TYPE_BNK_SHIFT(2),
			AL_NAND_FLASH_CTL_3_FLASH_TYPE_BNK_VAL_NAND) |
		AL_REG_BITS_FIELD(
			AL_NAND_FLASH_CTL_3_FLASH_TYPE_BNK_SHIFT(3),
			AL_NAND_FLASH_CTL_3_FLASH_TYPE_BNK_VAL_NAND) |
		AL_REG_BITS_FIELD(
			AL_NAND_FLASH_CTL_3_FLASH_TYPE_BNK_SHIFT(4),
			AL_NAND_FLASH_CTL_3_FLASH_TYPE_BNK_VAL_NAND) |
		AL_REG_BITS_FIELD(
			AL_NAND_FLASH_CTL_3_FLASH_TYPE_BNK_SHIFT(5),
			AL_NAND_FLASH_CTL_3_FLASH_TYPE_BNK_VAL_NAND);

	al_reg_write32(&obj->regs_base->flash_ctl_3, reg_val);

	return status;
}

/******************************************************************************/
/******************************************************************************/
void al_nand_terminate(
	struct al_nand_ctrl_obj	*obj)
{
	al_assert(obj);
}

/******************************************************************************/
/******************************************************************************/
void al_nand_reset(
	struct al_nand_ctrl_obj	*obj,
	int			reset_mask)
{
	al_assert(obj);

	al_reg_write32(&obj->regs_base->reset_reg, reset_mask);

	if (reset_mask & AL_NAND_RESET_MASK_TIMING_ENGINE) {
		uint32_t reg_val;

		do {
			reg_val = al_reg_read32(
				&obj->regs_base->reset_status_reg);
		} while (0 == AL_REG_BIT_GET(
			reg_val,
			AL_NAND_RESET_STATUS_REG_TIM_ENG_RST_DN));
	}
}

/******************************************************************************/
/******************************************************************************/
void al_nand_dev_select(
	struct al_nand_ctrl_obj	*obj,
	int			dev_index)
{
	uint32_t reg_val;

	al_assert(obj);
	al_assert(dev_index >= 0);
	al_assert(dev_index < 8);

	reg_val = al_reg_read32(&obj->regs_base->ctl_reg0);

	AL_REG_FIELD_SET(
		reg_val,
		AL_NAND_CTL_REG0_CS1_MASK,
		AL_NAND_CTL_REG0_CS1_SHIFT,
		dev_index);

	AL_REG_FIELD_SET(
		reg_val,
		AL_NAND_CTL_REG0_CS2_MASK,
		AL_NAND_CTL_REG0_CS2_SHIFT,
		dev_index);

	al_reg_write32(&obj->regs_base->ctl_reg0, reg_val);

	obj->current_dev_index = dev_index;
}

/******************************************************************************/
/******************************************************************************/
int al_nand_dev_config_basic(
	struct al_nand_ctrl_obj *obj)
{
	int status = 0;

	struct al_nand_dev_properties dev_properties;
	struct al_nand_ecc_config ecc_config;

	al_assert(obj);

	al_memset(&dev_properties, 0, sizeof(struct al_nand_dev_properties));
	dev_properties.timingMode = AL_NAND_DEVICE_TIMING_MODE_ONFI_0;

	al_memset(&ecc_config, 0, sizeof(struct al_nand_ecc_config));

	if (0 != al_nand_dev_config(
			obj,
			&dev_properties,
			&ecc_config)) {
		al_err("al_nand_dev_config failed!\n");
		status = -EIO;
		goto done;
	}

done:
	return status;
}

/******************************************************************************/
/******************************************************************************/
int al_nand_dev_config(
	struct al_nand_ctrl_obj	*obj,
	struct al_nand_dev_properties	*dev_properties,
	struct al_nand_ecc_config	*ecc_config)
{
	int status = 0;

	al_assert(obj);
	al_assert(dev_properties);
	al_assert(ecc_config);

	obj->dev_properties = *dev_properties;
	obj->ecc_config = *ecc_config;

	if (0 != _al_nand_dev_config_modes(obj)) {
		al_err("_al_nand_dev_config_modes failed!\n");
		status = -EIO;
		goto done;
	}

	_al_nand_dev_config_ctl_reg_0(obj);

	_al_nand_dev_config_sdr_timing_params(obj);

	_al_nand_dev_config_rdy_busy_wait_cnt(obj);

	_al_nand_dev_config_bch_ctl_reg_0(obj);

	_al_nand_dev_config_bch_ctl_reg_1(obj);

done:

	return status;
}
/******************************************************************************/
/******************************************************************************/
#if NAND_CODED_PROPERTIES_NUM_WORDS != 4
#error NAND_CODED_PROPERTIES_NUM_WORDS != 4
#endif

int al_nand_properties_decode(
	void __iomem				*pbs_regs_base,
	struct al_nand_dev_properties		*dev_properties,
	struct al_nand_ecc_config		*ecc_config,
	struct al_nand_extra_dev_properties	*dev_ext_props)
{
	int fieldVal;
	uint32_t nand_properties[NAND_CODED_PROPERTIES_NUM_WORDS];
	struct al_pbs_regs *pbs_regs = pbs_regs_base;

	nand_properties[0] = al_reg_read32(&pbs_regs->unit.cfg_nand_0);
	nand_properties[1] = al_reg_read32(&pbs_regs->unit.cfg_nand_1);
	nand_properties[2] = al_reg_read32(&pbs_regs->unit.cfg_nand_2);

	if (0 == AL_REG_FIELD_GET(nand_properties[0],
				NAND_CODED_PROPERTIES_WORD_0_IS_VALID_MASK,
				NAND_CODED_PROPERTIES_WORD_0_IS_VALID_SHIFT)) {
		al_err("nand properties is not valid\n");
		return -EINVAL;
	}

	al_memset(dev_properties, 0, sizeof(struct al_nand_dev_properties));
	al_memset(ecc_config, 0, sizeof(struct al_nand_ecc_config));
	al_memset(dev_ext_props, 0,
				sizeof(struct al_nand_extra_dev_properties));

	fieldVal = AL_REG_FIELD_GET(nand_properties[0],
				NAND_CODED_PROPERTIES_WORD_0_PAGE_SIZE_MASK,
				NAND_CODED_PROPERTIES_WORD_0_PAGE_SIZE_SHIFT);

	dev_ext_props->pageSize = 512 << fieldVal;

	if (0 == fieldVal)
		dev_properties->pageSize = AL_NAND_DEVICE_PAGE_SIZE_512;
	else
		dev_properties->pageSize = fieldVal - 2;

	dev_ext_props->blockSize =
		dev_ext_props->pageSize * 16 *
		(1 << AL_REG_FIELD_GET(nand_properties[0],
				NAND_CODED_PROPERTIES_WORD_0_BLOCK_SIZE_MASK,
				NAND_CODED_PROPERTIES_WORD_0_BLOCK_SIZE_SHIFT));

	dev_properties->sdrDataWidth =
		AL_REG_FIELD_GET(nand_properties[0],
				NAND_CODED_PROPERTIES_WORD_0_BUS_WIDTH_MASK,
				NAND_CODED_PROPERTIES_WORD_0_BUS_WIDTH_SHIFT);

	dev_ext_props->wordSize = (1 << dev_properties->sdrDataWidth);

	dev_properties->num_col_cyc =
		1 + AL_REG_FIELD_GET(nand_properties[0],
				NAND_CODED_PROPERTIES_WORD_0_COL_CYCLES_MASK,
				NAND_CODED_PROPERTIES_WORD_0_COL_CYCLES_SHIFT);

	dev_properties->num_row_cyc =
		1 + AL_REG_FIELD_GET(nand_properties[0],
				NAND_CODED_PROPERTIES_WORD_0_ROW_CYCLES_MASK,
				NAND_CODED_PROPERTIES_WORD_0_ROW_CYCLES_SHIFT);

	dev_ext_props->badBlockMarking.method =
		AL_REG_FIELD_GET(nand_properties[0],
				NAND_CODED_PROPERTIES_WORD_0_BBM_METHOD_MASK,
				NAND_CODED_PROPERTIES_WORD_0_BBM_METHOD_SHIFT);

	dev_ext_props->badBlockMarking.location1 =
		AL_REG_FIELD_GET(nand_properties[0],
				NAND_CODED_PROPERTIES_WORD_0_BBM_LOC1_MASK,
				NAND_CODED_PROPERTIES_WORD_0_BBM_LOC1_SHIFT);

	dev_ext_props->badBlockMarking.location2 =
		AL_REG_FIELD_GET(nand_properties[0],
				NAND_CODED_PROPERTIES_WORD_0_BBM_LOC2_MASK,
				NAND_CODED_PROPERTIES_WORD_0_BBM_LOC2_SHIFT);

	dev_properties->timingMode = AL_REG_FIELD_GET(nand_properties[0],
				NAND_CODED_PROPERTIES_WORD_0_TIMING_SET_MASK,
				NAND_CODED_PROPERTIES_WORD_0_TIMING_SET_SHIFT);

	fieldVal = AL_REG_FIELD_GET(nand_properties[0],
				NAND_CODED_PROPERTIES_WORD_0_ECC_ALG_MASK,
				NAND_CODED_PROPERTIES_WORD_0_ECC_ALG_SHIFT);

	dev_ext_props->eccIsEnabled = (0 != fieldVal) ? 1 : 0;

	if (dev_ext_props->eccIsEnabled)
			ecc_config->algorithm = fieldVal - 1;

	ecc_config->num_corr_bits =
		AL_REG_FIELD_GET(nand_properties[1],
			NAND_CODED_PROPERTIES_WORD_1_ECC_BCH_STRENGTH_MASK,
			NAND_CODED_PROPERTIES_WORD_1_ECC_BCH_STRENGTH_SHIFT);

	ecc_config->messageSize =
		AL_REG_FIELD_GET(nand_properties[1],
			NAND_CODED_PROPERTIES_WORD_1_ECC_BCH_CODEWORD_MASK,
			NAND_CODED_PROPERTIES_WORD_1_ECC_BCH_CODEWORD_SHIFT);

	ecc_config->spareAreaOffset =
		dev_ext_props->pageSize +
		AL_REG_FIELD_GET(nand_properties[1],
			NAND_CODED_PROPERTIES_WORD_1_ECC_LOC_MASK,
			NAND_CODED_PROPERTIES_WORD_1_ECC_LOC_SHIFT);

	dev_properties->timing.tSETUP =
		AL_REG_FIELD_GET(nand_properties[1],
			NAND_CODED_PROPERTIES_WORD_1_TIMING_TSETUP_MASK,
			NAND_CODED_PROPERTIES_WORD_1_TIMING_TSETUP_SHIFT);

	dev_properties->timing.tHOLD =
		AL_REG_FIELD_GET(nand_properties[1],
			NAND_CODED_PROPERTIES_WORD_1_TIMING_THOLD_MASK,
			NAND_CODED_PROPERTIES_WORD_1_TIMING_THOLD_SHIFT);

	dev_properties->timing.tWH =
		AL_REG_FIELD_GET(nand_properties[1],
			NAND_CODED_PROPERTIES_WORD_1_TIMING_TWH_MASK,
			NAND_CODED_PROPERTIES_WORD_1_TIMING_TWH_SHIFT);

	dev_properties->timing.tWRP =
		AL_REG_FIELD_GET(nand_properties[2],
			NAND_CODED_PROPERTIES_WORD_2_TIMING_TWRP_MASK,
			NAND_CODED_PROPERTIES_WORD_2_TIMING_TWRP_SHIFT);

	dev_properties->timing.tINTCMD =
		AL_REG_FIELD_GET(nand_properties[2],
			NAND_CODED_PROPERTIES_WORD_2_TIMING_TINTCMD_MASK,
			NAND_CODED_PROPERTIES_WORD_2_TIMING_TINTCMD_SHIFT);

	dev_properties->timing.tRR =
		AL_REG_FIELD_GET(nand_properties[2],
			NAND_CODED_PROPERTIES_WORD_2_TIMING_TREADYRE_MASK,
			NAND_CODED_PROPERTIES_WORD_2_TIMING_TREADYRE_SHIFT);

	dev_properties->timing.tWB =
		AL_REG_FIELD_GET(nand_properties[2],
			NAND_CODED_PROPERTIES_WORD_2_TIMING_TWB_MASK,
			NAND_CODED_PROPERTIES_WORD_2_TIMING_TWB_SHIFT);

	dev_properties->timing.readDelay =
		AL_REG_FIELD_GET(nand_properties[2],
			NAND_CODED_PROPERTIES_WORD_2_TIMING_TREADDLY_MASK,
			NAND_CODED_PROPERTIES_WORD_2_TIMING_TREADDLY_SHIFT);

	return 0;
}


/******************************************************************************/
/******************************************************************************/
void al_nand_cw_config(
	struct al_nand_ctrl_obj	*obj,
	uint32_t		cw_size,
	uint32_t		cw_count)
{
	uint32_t reg_val;

	al_assert(obj);

	reg_val = 0;

	AL_REG_FIELD_SET(
		reg_val,
		AL_NAND_CODEWORD_SIZE_CNT_REG_SIZE_MASK,
		AL_NAND_CODEWORD_SIZE_CNT_REG_SIZE_SHIFT,
		cw_size);

	AL_REG_FIELD_SET(
		reg_val,
		AL_NAND_CODEWORD_SIZE_CNT_REG_COUNT_MASK,
		AL_NAND_CODEWORD_SIZE_CNT_REG_COUNT_SHIFT,
		cw_count);

	al_reg_write32(&obj->regs_base->codeword_size_cnt_reg, reg_val);

	al_reg_write32(&obj->wrap_regs_base->code_word_size, cw_size);

	obj->cw_size = cw_size;
	obj->cw_count_remaining = obj->cw_count = cw_count;
	obj->cw_size_remaining = 0;
}

/******************************************************************************/
/******************************************************************************/
void al_nand_ecc_set_enabled(
	struct al_nand_ctrl_obj	*obj,
	int			enabled)
{
	uint32_t reg_val;

	al_assert(obj);

	reg_val = al_reg_read32(&obj->regs_base->bch_ctrl_reg_0);

	AL_REG_BIT_VAL_SET(
		reg_val,
		AL_NAND_BCH_CTRL_REG_0_ECC_ON_OFF,
		enabled);

	al_reg_write32(&obj->regs_base->bch_ctrl_reg_0, reg_val);
}

/******************************************************************************/
/******************************************************************************/
void al_nand_wp_set_enable(
	struct al_nand_ctrl_obj	*obj,
	int			enable)
{
	uint32_t reg_val;

	al_assert(obj);

	reg_val = al_reg_read32(&obj->regs_base->ctl_reg0);

	AL_REG_BIT_VAL_SET(reg_val, AL_NAND_CTL_REG0_WP, enable ? 0 : 1);

	al_reg_write32(&obj->regs_base->ctl_reg0, reg_val);
}

void al_nand_tx_set_enable(
	struct al_nand_ctrl_obj	*obj,
	int			enable)
{
	uint32_t reg_val;

	al_assert(obj);

	reg_val = al_reg_read32(&obj->regs_base->ctl_reg0);

	AL_REG_BIT_VAL_SET(reg_val, AL_NAND_CTL_REG0_TX_MODE, enable);

	al_reg_write32(&obj->regs_base->ctl_reg0, reg_val);
}

/******************************************************************************/
/******************************************************************************/
int al_nand_uncorr_err_get(
	struct al_nand_ctrl_obj	*obj)
{
	int reg_val;

	al_assert(obj);

	reg_val = al_reg_read32(&obj->regs_base->nfc_int_stat);

	return AL_REG_BIT_GET(reg_val, AL_NAND_NFC_INT_STAT_UNCORR_ERR);
}

/******************************************************************************/
/******************************************************************************/
void al_nand_uncorr_err_clear(
	struct al_nand_ctrl_obj	*obj)
{
	int reg_val = 0;

	al_assert(obj);

	AL_REG_BIT_SET(reg_val, AL_NAND_NFC_INT_STAT_UNCORR_ERR);

	al_reg_write32(&obj->regs_base->nfc_int_stat, reg_val);
}

/******************************************************************************/
/******************************************************************************/
int al_nand_corr_err_get(
	struct al_nand_ctrl_obj	*obj)
{
	int reg_val;

	al_assert(obj);

	reg_val = al_reg_read32(&obj->regs_base->nfc_int_stat);

	return AL_REG_BIT_GET(reg_val, AL_NAND_NFC_INT_STAT_CORR_ERR);
}

/******************************************************************************/
/******************************************************************************/
void al_nand_corr_err_clear(
	struct al_nand_ctrl_obj	*obj)
{
	int reg_val = 0;

	al_assert(obj);

	AL_REG_BIT_VAL_SET(reg_val, AL_NAND_NFC_INT_STAT_CORR_ERR, 1);

	al_reg_write32(&obj->regs_base->nfc_int_stat, reg_val);
}

/******************************************************************************/
/******************************************************************************/
int al_nand_dev_is_ready(
	struct al_nand_ctrl_obj	*obj)
{
	int is_ready;
	uint32_t reg_val;

	al_assert(obj);

	reg_val = al_reg_read32(&obj->regs_base->rdy_busy_status_reg);

	if (0 == (reg_val & (1 << obj->current_dev_index)))
		is_ready = 0;
	else
		is_ready = 1;

	return is_ready;
}

/******************************************************************************/
/******************************************************************************/
static void _al_nand_cmd_seq_size_page_read(
	struct al_nand_ctrl_obj	*obj,
	int			num_bytes,
	int			ecc_enabled,
	int			*cmd_seq_buff_num_entries)
{
	int code_word_size;
	int code_word_count;

	int num_required_entries;

	al_assert(obj);
	al_assert(cmd_seq_buff_num_entries);

	code_word_size = (512 << obj->ecc_config.messageSize);

	num_bytes = AL_ALIGN_UP(num_bytes, 4);

	if (num_bytes < code_word_size)
		code_word_size = num_bytes;

	if ((0 == ecc_enabled) && (0 != (num_bytes % code_word_size)))
		code_word_size = num_bytes / 4;

	al_assert(0 == (num_bytes % code_word_size));

	code_word_count = num_bytes / code_word_size;

	if (ecc_enabled) {
		num_required_entries =
			1 + /* first read command */
			obj->dev_properties.num_col_cyc + /* spare off */
			obj->dev_properties.num_row_cyc + /* row address */
			1 + /* second read command */
			1 + /* wait for ready */
			2 + /* spare count read */
			1 + /* change read column address first command */
			obj->dev_properties.num_col_cyc + /* column address*/
			1 + /* change read column address second command */
			1 + /* wait cycle count */
			2 * code_word_count; /* codeword size */
	} else {
		num_required_entries =
			1 + /* first read command */
			obj->dev_properties.num_col_cyc + /* column address*/
			obj->dev_properties.num_row_cyc + /* row address */
			1 + /* second read command */
			1 + /* wait for ready */
			2 * code_word_count; /* codeword size */
	}

	*cmd_seq_buff_num_entries = num_required_entries;
}

/******************************************************************************/
/******************************************************************************/
int al_nand_cmd_seq_gen_page_read(
	struct al_nand_ctrl_obj	*obj,
	int			column,
	int			row,
	int			num_bytes,
	int			ecc_enabled,
	uint32_t		*cmd_seq_buff,
	int			*cmd_seq_buff_num_entries,
	uint32_t		*cw_size,
	uint32_t		*cw_count)
{
	int status = 0;

	int code_word_size;
	int code_word_count;

	int num_required_entries;

	al_assert(obj);
	al_assert(cmd_seq_buff);
	al_assert(cmd_seq_buff_num_entries);

	code_word_size = (512 << obj->ecc_config.messageSize);

	num_bytes = AL_ALIGN_UP(num_bytes, 4);

	if (num_bytes < code_word_size)
		code_word_size = num_bytes;

	if ((0 == ecc_enabled) && (0 != (num_bytes % code_word_size)))
		code_word_size = num_bytes / 4;

	al_assert(0 == (num_bytes % code_word_size));

	code_word_count = num_bytes / code_word_size;

	*cw_size = code_word_size;
	*cw_count = code_word_count;

	_al_nand_cmd_seq_size_page_read(
			obj,
			num_bytes,
			ecc_enabled,
			&num_required_entries);

	if (*cmd_seq_buff_num_entries < num_required_entries) {
		al_err(
			"not enough entries provided - required %d!\n",
			num_required_entries);

		status = -EIO;
		goto done;
	}

	*cmd_seq_buff_num_entries = 0;

	if (ecc_enabled) {
		int spare_count =
			code_word_count * _al_nand_get_cw_spare_cnt(obj);

		_al_nand_cmd_seq_add_command(
			&cmd_seq_buff,
			cmd_seq_buff_num_entries,
			AL_NAND_COMMAND_TYPE_CMD,
			AL_NAND_DEVICE_CMD_PAGE_READ_FIRST);

		_al_nand_cmd_seq_add_command_cyc(
			&cmd_seq_buff,
			cmd_seq_buff_num_entries,
			AL_NAND_COMMAND_TYPE_ADDRESS,
			obj->ecc_config.spareAreaOffset /
			(obj->dev_properties.sdrDataWidth + 1),
			obj->dev_properties.num_col_cyc);

		_al_nand_cmd_seq_add_command_cyc(
			&cmd_seq_buff,
			cmd_seq_buff_num_entries,
			AL_NAND_COMMAND_TYPE_ADDRESS,
			row,
			obj->dev_properties.num_row_cyc);

		_al_nand_cmd_seq_add_command(
			&cmd_seq_buff,
			cmd_seq_buff_num_entries,
			AL_NAND_COMMAND_TYPE_CMD,
			AL_NAND_DEVICE_CMD_PAGE_READ_SECOND);

		_al_nand_cmd_seq_add_command(
			&cmd_seq_buff,
			cmd_seq_buff_num_entries,
			AL_NAND_COMMAND_TYPE_WAIT_FOR_READY,
			0);

		_al_nand_cmd_seq_add_command_cyc(
			&cmd_seq_buff,
			cmd_seq_buff_num_entries,
			AL_NAND_COMMAND_TYPE_SPARE_READ_COUNT,
			spare_count,
			2);

		_al_nand_cmd_seq_add_command(
			&cmd_seq_buff,
			cmd_seq_buff_num_entries,
			AL_NAND_COMMAND_TYPE_CMD,
			AL_NAND_DEVICE_CMD_CHG_READ_COL_FIRST);

		_al_nand_cmd_seq_add_command_cyc(
			&cmd_seq_buff,
			cmd_seq_buff_num_entries,
			AL_NAND_COMMAND_TYPE_ADDRESS,
			column / (obj->dev_properties.sdrDataWidth + 1),
			obj->dev_properties.num_col_cyc);

		_al_nand_cmd_seq_add_command(
			&cmd_seq_buff,
			cmd_seq_buff_num_entries,
			AL_NAND_COMMAND_TYPE_CMD,
			AL_NAND_DEVICE_CMD_CHG_READ_COL_SECOND);

		_al_nand_cmd_seq_add_command(
			&cmd_seq_buff,
			cmd_seq_buff_num_entries,
			AL_NAND_COMMAND_TYPE_WAIT_CYCLE_COUNT,
			0);

		while (num_bytes > 0) {
			_al_nand_cmd_seq_add_command_cyc(
				&cmd_seq_buff,
				cmd_seq_buff_num_entries,
				AL_NAND_COMMAND_TYPE_DATA_READ_COUNT,
				code_word_size,
				2);

			num_bytes -= code_word_size;
		}
	} else {
		_al_nand_cmd_seq_add_command(
			&cmd_seq_buff,
			cmd_seq_buff_num_entries,
			AL_NAND_COMMAND_TYPE_CMD,
			AL_NAND_DEVICE_CMD_PAGE_READ_FIRST);

		_al_nand_cmd_seq_add_command_cyc(
			&cmd_seq_buff,
			cmd_seq_buff_num_entries,
			AL_NAND_COMMAND_TYPE_ADDRESS,
			column / (obj->dev_properties.sdrDataWidth + 1),
			obj->dev_properties.num_col_cyc);

		_al_nand_cmd_seq_add_command_cyc(
			&cmd_seq_buff,
			cmd_seq_buff_num_entries,
			AL_NAND_COMMAND_TYPE_ADDRESS,
			row,
			obj->dev_properties.num_row_cyc);

		_al_nand_cmd_seq_add_command(
			&cmd_seq_buff,
			cmd_seq_buff_num_entries,
			AL_NAND_COMMAND_TYPE_CMD,
			AL_NAND_DEVICE_CMD_PAGE_READ_SECOND);

		_al_nand_cmd_seq_add_command(
			&cmd_seq_buff,
			cmd_seq_buff_num_entries,
			AL_NAND_COMMAND_TYPE_WAIT_FOR_READY,
			0);

		while (num_bytes > 0) {
			_al_nand_cmd_seq_add_command_cyc(
				&cmd_seq_buff,
				cmd_seq_buff_num_entries,
				AL_NAND_COMMAND_TYPE_DATA_READ_COUNT,
				code_word_size,
				2);

			num_bytes -= code_word_size;
		}
	}

	al_assert(*cmd_seq_buff_num_entries == num_required_entries);

done:

	return status;
}

/******************************************************************************/
/******************************************************************************/
void al_nand_cmd_seq_size_page_write(
	struct al_nand_ctrl_obj	*obj,
	int			num_bytes,
	int			ecc_enabled,
	int			*cmd_seq_buff_num_entries)
{
	int code_word_size;
	int code_word_count;

	int num_required_entries;

	al_assert(obj);
	al_assert(cmd_seq_buff_num_entries);

	code_word_size = (512 << obj->ecc_config.messageSize);

	if (num_bytes < code_word_size)
		code_word_size = num_bytes;

	if ((0 == ecc_enabled) && (0 != (num_bytes % code_word_size)))
		code_word_size = num_bytes / 4;

	al_assert(0 == (num_bytes % code_word_size));

	code_word_count = num_bytes / code_word_size;

	if (ecc_enabled) {
		num_required_entries =
			1 + /* first program command */
			obj->dev_properties.num_col_cyc + /* column address*/
			obj->dev_properties.num_row_cyc + /* row address */
			1 + /* NOP */
			1 + /* wait for cycle count */
			2 * code_word_count + /* codeword size */
			1 + /* change program column address first command */
			obj->dev_properties.num_col_cyc + /* spare off */
			1 + /* wait for cycle count */
			2 + /* spare count write */
			1 + /* second program command */
			1; /* wait for ready */
	} else {
		num_required_entries =
			1 + /* first program command */
			obj->dev_properties.num_col_cyc + /* column address*/
			obj->dev_properties.num_row_cyc + /* row address */
			1 + /* NOP */
			1 + /* wait for cycle count */
			2 * code_word_count + /* codeword size */
			1 + /* second program command */
			1; /* wait for ready */
	}

	*cmd_seq_buff_num_entries = num_required_entries;
}

/******************************************************************************/
/******************************************************************************/
int al_nand_cmd_seq_gen_page_write(
	struct al_nand_ctrl_obj	*obj,
	int			column,
	int			row,
	int			num_bytes,
	int			ecc_enabled,
	uint32_t		*cmd_seq_buff,
	int			*cmd_seq_buff_num_entries,
	uint32_t		*cw_size,
	uint32_t		*cw_count)
{
	int status = 0;

	int code_word_size;
	int code_word_count;

	int num_required_entries;

	al_assert(obj);
	al_assert(cmd_seq_buff);
	al_assert(cmd_seq_buff_num_entries);

	code_word_size = (512 << obj->ecc_config.messageSize);

	num_bytes = AL_ALIGN_UP(num_bytes, 4);

	if (num_bytes < code_word_size)
		code_word_size = num_bytes;

	if ((0 == ecc_enabled) && (0 != (num_bytes % code_word_size)))
		code_word_size = num_bytes / 4;

	al_assert(0 == (num_bytes % code_word_size));

	code_word_count = num_bytes / code_word_size;

	*cw_size = code_word_size;
	*cw_count = code_word_count;

	al_nand_cmd_seq_size_page_write(
		obj,
		num_bytes,
		ecc_enabled,
		&num_required_entries);

	if (*cmd_seq_buff_num_entries < num_required_entries) {
		al_err(
			"not enough entries provided - required %d!\n",
			num_required_entries);

		status = -EIO;
		goto done;
	}

	*cmd_seq_buff_num_entries = 0;

	if (ecc_enabled) {
		int spare_count =
			code_word_count * _al_nand_get_cw_spare_cnt(obj);

		_al_nand_cmd_seq_add_command(
			&cmd_seq_buff,
			cmd_seq_buff_num_entries,
			AL_NAND_COMMAND_TYPE_CMD,
			AL_NAND_DEVICE_CMD_PAGE_PRG_FIRST);

		_al_nand_cmd_seq_add_command_cyc(
			&cmd_seq_buff,
			cmd_seq_buff_num_entries,
			AL_NAND_COMMAND_TYPE_ADDRESS,
			column / (obj->dev_properties.sdrDataWidth + 1),
			obj->dev_properties.num_col_cyc);

		_al_nand_cmd_seq_add_command_cyc(
			&cmd_seq_buff,
			cmd_seq_buff_num_entries,
			AL_NAND_COMMAND_TYPE_ADDRESS,
			row,
			obj->dev_properties.num_row_cyc);

		_al_nand_cmd_seq_add_command(
			&cmd_seq_buff,
			cmd_seq_buff_num_entries,
			AL_NAND_COMMAND_TYPE_NOP,
			0);

		_al_nand_cmd_seq_add_command(
			&cmd_seq_buff,
			cmd_seq_buff_num_entries,
			AL_NAND_COMMAND_TYPE_WAIT_CYCLE_COUNT,
			0);

		while (num_bytes) {
			_al_nand_cmd_seq_add_command_cyc(
				&cmd_seq_buff,
				cmd_seq_buff_num_entries,
				AL_NAND_COMMAND_TYPE_DATA_WRITE_COUNT,
				code_word_size,
				2);

			num_bytes -= code_word_size;
		}

		_al_nand_cmd_seq_add_command(
			&cmd_seq_buff,
			cmd_seq_buff_num_entries,
			AL_NAND_COMMAND_TYPE_CMD,
			AL_NAND_DEVICE_CMD_CHG_PRG_COL_FIRST);

		_al_nand_cmd_seq_add_command_cyc(
			&cmd_seq_buff,
			cmd_seq_buff_num_entries,
			AL_NAND_COMMAND_TYPE_ADDRESS,
			obj->ecc_config.spareAreaOffset /
			(obj->dev_properties.sdrDataWidth + 1),
			obj->dev_properties.num_col_cyc);

		_al_nand_cmd_seq_add_command(
			&cmd_seq_buff,
			cmd_seq_buff_num_entries,
			AL_NAND_COMMAND_TYPE_WAIT_CYCLE_COUNT,
			0);

		_al_nand_cmd_seq_add_command_cyc(
			&cmd_seq_buff,
			cmd_seq_buff_num_entries,
			AL_NAND_COMMAND_TYPE_SPARE_WRITE_COUNT,
			spare_count,
			2);

		_al_nand_cmd_seq_add_command(
			&cmd_seq_buff,
			cmd_seq_buff_num_entries,
			AL_NAND_COMMAND_TYPE_CMD,
			AL_NAND_DEVICE_CMD_PAGE_PRG_SECOND);

		_al_nand_cmd_seq_add_command(
			&cmd_seq_buff,
			cmd_seq_buff_num_entries,
			AL_NAND_COMMAND_TYPE_WAIT_FOR_READY,
			0);
	} else {
		_al_nand_cmd_seq_add_command(
			&cmd_seq_buff,
			cmd_seq_buff_num_entries,
			AL_NAND_COMMAND_TYPE_CMD,
			AL_NAND_DEVICE_CMD_PAGE_PRG_FIRST);

		_al_nand_cmd_seq_add_command_cyc(
			&cmd_seq_buff,
			cmd_seq_buff_num_entries,
			AL_NAND_COMMAND_TYPE_ADDRESS,
			column / (obj->dev_properties.sdrDataWidth + 1),
			obj->dev_properties.num_col_cyc);

		_al_nand_cmd_seq_add_command_cyc(
			&cmd_seq_buff,
			cmd_seq_buff_num_entries,
			AL_NAND_COMMAND_TYPE_ADDRESS,
			row,
			obj->dev_properties.num_row_cyc);

		_al_nand_cmd_seq_add_command(
			&cmd_seq_buff,
			cmd_seq_buff_num_entries,
			AL_NAND_COMMAND_TYPE_NOP,
			0);

		_al_nand_cmd_seq_add_command(
			&cmd_seq_buff,
			cmd_seq_buff_num_entries,
			AL_NAND_COMMAND_TYPE_WAIT_CYCLE_COUNT,
			0);

		while (num_bytes) {
			_al_nand_cmd_seq_add_command_cyc(
				&cmd_seq_buff,
				cmd_seq_buff_num_entries,
				AL_NAND_COMMAND_TYPE_DATA_WRITE_COUNT,
				code_word_size,
				2);

			num_bytes -= code_word_size;
		}

		_al_nand_cmd_seq_add_command(
			&cmd_seq_buff,
			cmd_seq_buff_num_entries,
			AL_NAND_COMMAND_TYPE_CMD,
			AL_NAND_DEVICE_CMD_PAGE_PRG_SECOND);

		_al_nand_cmd_seq_add_command(
			&cmd_seq_buff,
			cmd_seq_buff_num_entries,
			AL_NAND_COMMAND_TYPE_WAIT_FOR_READY,
			0);
	}

	al_assert(*cmd_seq_buff_num_entries == num_required_entries);

done:

	return status;
}

/******************************************************************************/
/******************************************************************************/
void al_nand_cmd_single_execute(
	struct al_nand_ctrl_obj	*obj,
	uint32_t		cmd)
{
	al_assert(obj);

	al_reg_write32(
		obj->cmd_buff_base,
		cmd);
}

/******************************************************************************/
/******************************************************************************/
void al_nand_cmd_seq_execute(
	struct al_nand_ctrl_obj	*obj,
	uint32_t		*cmd_seq_buff,
	int			cmd_seq_buff_num_entries)
{
	al_assert(obj);
	al_assert(cmd_seq_buff);

	while (cmd_seq_buff_num_entries > 0) {
		al_reg_write32(
			obj->cmd_buff_base,
			*cmd_seq_buff);

		cmd_seq_buff++;
		cmd_seq_buff_num_entries--;
	}
}

/******************************************************************************/
/******************************************************************************/
int al_nand_cmd_buff_is_empty(
	struct al_nand_ctrl_obj	*obj)
{
	uint32_t reg_val;
	int empty;

	al_assert(obj);

	reg_val = 0;

	AL_REG_BIT_VAL_SET(
		reg_val,
		AL_NAND_NFC_INT_STAT_CMD_BUF_EMPTY,
		1);

	al_reg_write32(&obj->regs_base->nfc_int_stat, reg_val);

	reg_val = al_reg_read32(&obj->regs_base->nfc_int_stat);

	empty = AL_REG_BIT_GET(reg_val, AL_NAND_NFC_INT_STAT_CMD_BUF_EMPTY);

	return empty;
}

/******************************************************************************/
/******************************************************************************/
void __iomem *al_nand_data_buff_base_get(
			struct al_nand_ctrl_obj	*obj)
{
	al_assert(obj);

	return obj->data_buff_base;
}

int al_nand_data_buff_read(
	struct al_nand_ctrl_obj	*obj,
	int						num_bytes,
	int						num_bytes_skip_head,
	int						num_bytes_skip_tail,
	uint8_t					*buff)
{
	int status = 0;

	uint32_t cw_size;
	uint32_t cw_size_remaining;
	uint32_t cw_count_remaining;

	al_assert(obj);
	al_assert(buff);

	cw_size = obj->cw_size;
	cw_size_remaining = obj->cw_size_remaining;
	cw_count_remaining = obj->cw_count_remaining;

	if (((cw_count_remaining * cw_size) + cw_size_remaining) <
		(uint32_t)num_bytes) {
		al_err(
			"%s: Not enough CW data (%u, %u)!\n",
			__func__,
			cw_size_remaining,
			cw_count_remaining);

		status = -EIO;
		goto done;
	}

	while (num_bytes > 0) {
		uint32_t word;

		uint8_t byte;

		int word_bytes;

		if (0 == cw_size_remaining) {
			cw_size_remaining = cw_size;
			cw_count_remaining--;
		}

		word = al_reg_read32(
			obj->data_buff_base);

		for (
			word_bytes = 0;
			(word_bytes < (int)sizeof(word)) && (num_bytes > 0);
			word_bytes++, num_bytes--, word >>= 8) {
			byte = word & 0xFF;

			if (
				(0 == num_bytes_skip_head) &&
				(num_bytes > num_bytes_skip_tail)) {
				*buff = byte;
				buff++;
			}

			if (num_bytes_skip_head > 0)
				num_bytes_skip_head--;
		}

		cw_size_remaining -= 4;
	}

	obj->cw_size_remaining = cw_size_remaining;
	obj->cw_count_remaining = cw_count_remaining;

done:

	return status;
}

/******************************************************************************/
/******************************************************************************/
int al_nand_data_buff_write(
	struct al_nand_ctrl_obj	*obj,
	int			num_bytes,
	const uint8_t		*buff)
{
	int status = 0;

	uint32_t cw_size;
	uint32_t cw_size_remaining;
	uint32_t cw_count_remaining;

	al_assert(obj);
	al_assert(buff);

	cw_size = obj->cw_size;
	cw_size_remaining = obj->cw_size_remaining;
	cw_count_remaining = obj->cw_count_remaining;

	if (((cw_count_remaining * cw_size) + cw_size_remaining) <
		(uint32_t)num_bytes) {
		al_err(
			"%s: Not enough CW data (%u, %u)!\n",
			__func__,
			cw_size_remaining,
			cw_count_remaining);

		status = -EIO;
		goto done;
	}

	while (num_bytes > 0) {
		uint32_t word;
		int word_bytes;

		if (0 == cw_size_remaining) {
			_al_nand_data_buf_wait_for_cw_vacancy(obj);
			cw_size_remaining = cw_size;
			cw_count_remaining--;
		}

		for (
			word_bytes = 0, word = 0;
			(word_bytes < (int)sizeof(word)) && (num_bytes > 0);
			word_bytes++, num_bytes--, buff++) {
			word >>= 8;

			word |= (uint32_t)(*buff) << 24;
		}

		for (
			;
			word_bytes < (int)sizeof(word);
			word_bytes++) {
			word >>= 8;
		}

		al_reg_write32(
			&obj->regs_base->data_buffer_reg,
			word);

		cw_size_remaining -= 4;
	}

done:

	return status;
}

/******************************************************************************/
/******************************************************************************/
static int _al_nand_dev_config_modes(
	struct al_nand_ctrl_obj	*obj)
{
	int status = 0;

	uint32_t reg_val;

	struct al_nand_dev_properties *dev_properties =
		&obj->dev_properties;

	al_assert(obj);

	reg_val = 0;

	AL_REG_FIELD_SET(
		reg_val,
		AL_NAND_MODE_SELECT_MODE_SELECT_MASK,
		AL_NAND_MODE_SELECT_MODE_SELECT_SHIFT,
		AL_NAND_MODE_SELECT_SDR);

	AL_REG_FIELD_SET(
		reg_val,
		AL_NAND_MODE_SELECT_SDR_TIM_MODE_MASK,
		AL_NAND_MODE_SELECT_SDR_TIM_MODE_SHIFT,
		dev_properties->timingMode);

	al_reg_write32(&obj->regs_base->mode_select_reg, reg_val);

	return status;
}

static void _al_nand_dev_config_ctl_reg_0(
	struct al_nand_ctrl_obj	*obj)
{
	uint32_t reg_val;

	struct al_nand_dev_properties *dev_properties =
		&obj->dev_properties;

	al_assert(obj);

	reg_val = al_reg_read32(&obj->regs_base->ctl_reg0);

	AL_REG_BIT_VAL_SET(
		reg_val,
		AL_NAND_CTL_REG0_DQ_WIDTH,
		dev_properties->sdrDataWidth);

	AL_REG_FIELD_SET(
		reg_val,
		AL_NAND_CTL_REG0_COL_ADDR_CYCLES_MASK,
		AL_NAND_CTL_REG0_COL_ADDR_CYCLES_SHIFT,
		dev_properties->num_col_cyc);

	AL_REG_FIELD_SET(
		reg_val,
		AL_NAND_CTL_REG0_ROW_ADDR_CYCLES_MASK,
		AL_NAND_CTL_REG0_ROW_ADDR_CYCLES_SHIFT,
		dev_properties->num_row_cyc);

	AL_REG_FIELD_SET(
		reg_val,
		AL_NAND_CTL_REG0_PAGE_SIZE_MASK,
		AL_NAND_CTL_REG0_PAGE_SIZE_SHIFT,
		dev_properties->pageSize);

	al_reg_write32(&obj->regs_base->ctl_reg0, reg_val);
}

static void _al_nand_dev_config_sdr_timing_params(
	struct al_nand_ctrl_obj	*obj)
{
	uint32_t reg_val;

	struct al_nand_dev_properties *dev_properties =
		&obj->dev_properties;

	al_assert(obj);

	reg_val =

	AL_REG_BITS_FIELD(
		AL_NAND_SDR_TIM_PARAMS_0_T_SETUP_SHIFT,
		dev_properties->timing.tSETUP) |

	AL_REG_BITS_FIELD(
		AL_NAND_SDR_TIM_PARAMS_0_T_HOLD_SHIFT,
		dev_properties->timing.tHOLD) |

	AL_REG_BITS_FIELD(
		AL_NAND_SDR_TIM_PARAMS_0_T_WH_SHIFT,
		dev_properties->timing.tWH) |

	AL_REG_BITS_FIELD(
		AL_NAND_SDR_TIM_PARAMS_0_T_WRP_SHIFT,
		dev_properties->timing.tWRP) |

	AL_REG_BITS_FIELD(
		AL_NAND_SDR_TIM_PARAMS_0_T_INTCMD_SHIFT,
		dev_properties->timing.tINTCMD);

	al_reg_write32(&obj->regs_base->sdr_timing_params_0, reg_val);

	reg_val =

	AL_REG_BITS_FIELD(
		AL_NAND_SDR_TIM_PARAMS_1_T_RR_SHIFT,
		dev_properties->timing.tRR) |

	AL_REG_BITS_FIELD(
		AL_NAND_SDR_TIM_PARAMS_1_T_WB_SHIFT,
		dev_properties->timing.tWB &
		(AL_NAND_SDR_TIM_PARAMS_1_T_WB_MASK >>
		AL_NAND_SDR_TIM_PARAMS_1_T_WB_SHIFT)) |

	AL_REG_BITS_FIELD(
		AL_NAND_SDR_TIM_PARAMS_1_T_READ_DLY_SHIFT,
		dev_properties->timing.readDelay) |

	AL_REG_BITS_FIELD(
		AL_NAND_SDR_TIM_PARAMS_1_T_WB_MSB_SHIFT,
		dev_properties->timing.tWB >>
		AL_NAND_SDR_TIM_PARAMS_1_T_WB_WIDTH);

	al_reg_write32(&obj->regs_base->sdr_timing_params_1, reg_val);
}

static void _al_nand_dev_config_rdy_busy_wait_cnt(
	struct al_nand_ctrl_obj	*obj)
{
	uint32_t reg_val;

	struct al_nand_dev_properties *dev_properties =
		&obj->dev_properties;

	al_assert(obj);

	reg_val = 0;

	AL_REG_FIELD_SET(
		reg_val,
		AL_NAND_RDYBSY_WAIT_CNT_REG_RDY_TOUT_CNT_MASK,
		AL_NAND_RDYBSY_WAIT_CNT_REG_RDY_TOUT_CNT_SHIFT,
		dev_properties->readyBusyTimeout);

	AL_REG_FIELD_SET(
		reg_val,
		AL_NAND_RDYBSY_WAIT_CNT_REG_RDYBSYEN_MASK,
		AL_NAND_RDYBSY_WAIT_CNT_REG_RDYBSYEN_SHIFT,
		(dev_properties->readyBusyTimeout > 0) ? 0 : 1);

	al_reg_write32(&obj->regs_base->rdy_busy_wait_cnt_reg, reg_val);
}

static void _al_nand_dev_config_bch_ctl_reg_0(
	struct al_nand_ctrl_obj	*obj)
{
	uint32_t reg_val;

	struct al_nand_ecc_config *ecc_config =
		&obj->ecc_config;

	al_assert(obj);

	reg_val = al_reg_read32(&obj->regs_base->bch_ctrl_reg_0);

	AL_REG_BIT_VAL_SET(
		reg_val,
		AL_NAND_BCH_CTRL_REG_0_ECC_ALGORITHM,
		ecc_config->algorithm);

	AL_REG_FIELD_SET(
		reg_val,
		AL_NAND_BCH_CTRL_REG_0_BCH_T_MASK,
		AL_NAND_BCH_CTRL_REG_0_BCH_T_SHIFT,
		obj->ecc_config.num_corr_bits);

	AL_REG_FIELD_SET(
		reg_val,
		AL_NAND_BCH_CTRL_REG_0_BCH_N_MASK,
		AL_NAND_BCH_CTRL_REG_0_BCH_N_SHIFT,
		_al_nand_get_bch_cw_parity_size(obj));

	al_reg_write32(&obj->regs_base->bch_ctrl_reg_0, reg_val);
}

static void _al_nand_dev_config_bch_ctl_reg_1(
	struct al_nand_ctrl_obj	*obj)
{
	uint32_t reg_val;

	struct al_nand_ecc_config *ecc_config =
		&obj->ecc_config;

	al_assert(obj);

	reg_val = 0;

	AL_REG_FIELD_SET(
		reg_val,
		AL_NAND_BCH_CTRL_REG_1_BCH_K_MASK,
		AL_NAND_BCH_CTRL_REG_1_BCH_K_SHIFT,
		ecc_config->messageSize);

	al_reg_write32(&obj->regs_base->bch_ctrl_reg_1, reg_val);
}

static void _al_nand_data_buf_wait_for_cw_vacancy(
	struct al_nand_ctrl_obj	*obj)
{
	uint32_t reg_val;

	do {
		reg_val = 0;

		AL_REG_BIT_VAL_SET(
			reg_val,
			AL_NAND_NFC_INT_STAT_BUF_WR_RDY,
			1);

		al_reg_write32(
			&obj->regs_base->nfc_int_stat,
			reg_val);

		reg_val = al_reg_read32(&obj->regs_base->nfc_int_stat);

	} while (!AL_REG_BIT_GET(reg_val, AL_NAND_NFC_INT_STAT_BUF_WR_RDY));
}

static void _al_nand_cmd_seq_add_command(
	uint32_t			**cmd_seq_buff,
	int				*cmd_seq_buff_num_entries,
	enum al_nand_command_type	type,
	uint8_t				argument)
{
	**cmd_seq_buff = AL_NAND_CMD_SEQ_ENTRY(type, argument);

	(*cmd_seq_buff)++;
	(*cmd_seq_buff_num_entries)++;
}

static void _al_nand_cmd_seq_add_command_cyc(
	uint32_t			**cmd_seq_buff,
	int				*cmd_seq_buff_num_entries,
	enum al_nand_command_type	type,
	int				argument,
	int				num_cyc)
{
	while (num_cyc) {
		**cmd_seq_buff = AL_NAND_CMD_SEQ_ENTRY(type, argument & 0xFF);

		(*cmd_seq_buff)++;
		(*cmd_seq_buff_num_entries)++;
		num_cyc--;
		argument >>= 8;
	}
}

static int _al_nand_get_bch_num_corr_bits(
	struct al_nand_ctrl_obj	*obj)
{
	return 4 * (1 + obj->ecc_config.num_corr_bits);
}

static int _al_nand_get_bch_cw_parity_size(
	struct al_nand_ctrl_obj	*obj)
{
	return 15 * _al_nand_get_bch_num_corr_bits(obj);
}

static int _al_nand_get_cw_spare_cnt(
	struct al_nand_ctrl_obj	*obj)
{
	if (obj->ecc_config.algorithm == AL_NAND_ECC_ALGORITHM_HAMMING)	{
		return 4;
	} else {
		return AL_ALIGN_UP(
			((_al_nand_get_bch_cw_parity_size(obj) + 7) / 8), 4);
	}
}

/******************************************************************************/
/******************************************************************************/
uint32_t al_nand_int_status_get(
		struct al_nand_ctrl_obj	*obj)
{
	al_assert(obj);

	return al_reg_read32(&obj->regs_base->nfc_int_stat);
}

void al_nand_int_enable(
		struct al_nand_ctrl_obj	*obj,
		uint32_t int_mask)
{
	uint32_t val;

	al_assert(obj);

	/* before enabling the interrupt the status must be cleared cause the
	 * controller doesn't clear the status when it no longer valid */
	al_reg_write32(&obj->regs_base->nfc_int_stat, int_mask);

	val = al_reg_read32(&obj->regs_base->nfc_int_en);
	AL_REG_MASK_SET(val, int_mask);
	al_reg_write32(&obj->regs_base->nfc_int_en, val);
}

void al_nand_int_disable(
		struct al_nand_ctrl_obj	*obj,
		uint32_t int_mask)
{
	uint32_t val;

	al_assert(obj);

	val = al_reg_read32(&obj->regs_base->nfc_int_en);
	AL_REG_MASK_CLEAR(val, int_mask);
	al_reg_write32(&obj->regs_base->nfc_int_en, val);
}

void al_nand_int_clear(
		struct al_nand_ctrl_obj	*obj,
		uint32_t int_mask)
{
	al_assert(obj);

	al_reg_write32(&obj->regs_base->nfc_int_stat, int_mask);
}

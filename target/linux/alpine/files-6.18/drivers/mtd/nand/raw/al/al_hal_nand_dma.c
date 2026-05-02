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

#include "al_hal_nand.h"
#include "al_hal_nand_regs.h"

static int _al_nand_reg_write_dma(
	struct al_nand_ctrl_obj	*obj,
	uintptr_t		reg_addr,
	struct al_buf			*tx_buff,
	int			trigger_interrupt);

/******************************************************************************/
/******************************************************************************/
void al_nand_cw_config_buffs_prepare(
	struct al_nand_ctrl_obj	*obj,
	uint32_t		cw_size,
	uint32_t		cw_count,
	uint32_t		*buff_arr[2])
{
	al_assert(obj);
	al_assert(buff_arr);

	*(buff_arr[0]) =
		AL_REG_BITS_FIELD(
			AL_NAND_CODEWORD_SIZE_CNT_REG_SIZE_SHIFT,
			cw_size) |
		AL_REG_BITS_FIELD(
			AL_NAND_CODEWORD_SIZE_CNT_REG_COUNT_SHIFT,
			cw_count);

	*(buff_arr[1]) =
		cw_size;
}

/******************************************************************************/
/******************************************************************************/
int al_nand_cw_config_dma(
	struct al_nand_ctrl_obj	*obj,
	struct al_buf			tx_buff_arr[2],
	int						trigger_interrupt,
	int						*num_transactions)
{
	int status = 0;

	al_assert(obj);
	al_assert(tx_buff_arr);
	al_assert(num_transactions);

	if (0 != _al_nand_reg_write_dma(
		obj,
		(uintptr_t)&obj->regs_base->codeword_size_cnt_reg,
		&tx_buff_arr[0],
		0)) {

		al_err("_al_nand_reg_write_dma failed!\n");
		status = -EIO;
		goto done;
	}

	if (0 != _al_nand_reg_write_dma(
		obj,
		(uintptr_t)&obj->wrap_regs_base->code_word_size,
		&tx_buff_arr[1],
		trigger_interrupt)) {

		al_err("_al_nand_reg_write_dma failed!\n");
		status = -EIO;
		goto done;
	}

done:

	*num_transactions = 2;

	return status;
}

/******************************************************************************/
/******************************************************************************/
void al_nand_misc_ctrl_buffs_prepare(
	struct al_nand_ctrl_obj	*obj,
	int			wp_enable,
	int			tx_enable,
	uint32_t		*buff_arr[1])
{
	uint32_t reg_val;

	al_assert(obj);
	al_assert(buff_arr);

	reg_val = al_reg_read32(&obj->regs_base->ctl_reg0);

	AL_REG_BIT_VAL_SET(reg_val, AL_NAND_CTL_REG0_WP, wp_enable ? 0 : 1);

	AL_REG_BIT_VAL_SET(reg_val, AL_NAND_CTL_REG0_TX_MODE, tx_enable);

	*(buff_arr[0]) = reg_val;
}

/******************************************************************************/
/******************************************************************************/
int al_nand_misc_ctrl_dma(
	struct al_nand_ctrl_obj	*obj,
	struct al_buf			tx_buff_arr[1],
	int						trigger_interrupt,
	int						*num_transactions)
{
	int status = 0;

	al_assert(obj);
	al_assert(tx_buff_arr);
	al_assert(num_transactions);

	if (0 != _al_nand_reg_write_dma(
		obj,
		(uintptr_t)&obj->regs_base->ctl_reg0,
		&tx_buff_arr[0],
		trigger_interrupt)) {

		al_err("_al_nand_reg_write_dma failed!\n");
		status = -EIO;
		goto done;
	}

done:

	*num_transactions = 1;

	return status;
}

/******************************************************************************/
/******************************************************************************/
int al_nand_cmd_seq_execute_dma(
	struct al_nand_ctrl_obj	*obj,
	struct al_buf		*cmd_seq_buff,
	int			trigger_interrupt)
{
	int status = 0;

	struct al_buf transaction_dst_buf;
	struct al_block transaction_src_block;
	struct al_block transaction_dst_block;
	struct al_raid_transaction transaction;

	al_assert(obj);
	al_assert(cmd_seq_buff);

	transaction_src_block.bufs = cmd_seq_buff;
	transaction_src_block.num = 1;

	transaction_dst_buf.addr =
		(al_phys_addr_t)(uintptr_t)obj->cmd_buff_base;
	transaction_dst_buf.len = cmd_seq_buff->len;

	transaction_dst_block.bufs = &transaction_dst_buf;
	transaction_dst_block.num = 1;

	transaction.op = AL_RAID_OP_MEM_CPY;

	transaction.flags =
		AL_RAID_BARRIER | (trigger_interrupt ? AL_RAID_INTERRUPT : 0);

	transaction.srcs_blocks = &transaction_src_block;
	transaction.num_of_srcs = 1;
	transaction.total_src_bufs = 1;
	transaction.dsts_blocks = &transaction_dst_block;
	transaction.num_of_dsts = 1;
	transaction.total_dst_bufs = 1;

	if (0 != al_raid_dma_prepare(
		obj->raid_dma,
		obj->raid_dma_qid,
		&transaction)) {
		al_err("al_raid_dma_prepare failed!\n");
		status = -EIO;
		goto done;
	}

	if (0 != al_raid_dma_action(
		obj->raid_dma,
		obj->raid_dma_qid,
		transaction.tx_descs_count)) {
		al_err("al_raid_dma_action failed!\n");
		status = -EIO;
		goto done;
	}

done:

	return status;
}

/******************************************************************************/
/******************************************************************************/
void al_nand_cmd_seq_scion_buff_prepare(
	struct al_nand_ctrl_obj	*obj __attribute__((__unused__)),
	uint32_t		*buff)
{
	int i;

	for (i = 0; i < AL_NAND_CMD_FIFO_DEPTH; i++)
		buff[i] = AL_NAND_CMD_SEQ_ENTRY(AL_NAND_COMMAND_TYPE_NOP, 0);
}

int al_nand_cmd_seq_scion_dma(
	struct al_nand_ctrl_obj	*obj,
	struct al_buf			*tx_buff,
	int						trigger_interrupt,
	int						*num_transactions)
{
	int status = 0;

	struct al_buf transaction_dst_buf;
	struct al_block transaction_src_block;
	struct al_block transaction_dst_block;
	struct al_raid_transaction transaction;

	al_assert(obj);
	al_assert(tx_buff);
	al_assert(num_transactions);

	transaction_src_block.bufs = tx_buff;
	transaction_src_block.num = 1;

	transaction_dst_buf.addr =
		(al_phys_addr_t)(uintptr_t)obj->cmd_buff_base;
	transaction_dst_buf.len = tx_buff->len;

	transaction_dst_block.bufs = &transaction_dst_buf;
	transaction_dst_block.num = 1;

	transaction.op = AL_RAID_OP_MEM_CPY;

	transaction.flags =
		AL_RAID_BARRIER | (trigger_interrupt ? AL_RAID_INTERRUPT : 0);

	transaction.srcs_blocks = &transaction_src_block;
	transaction.num_of_srcs = 1;
	transaction.total_src_bufs = 1;
	transaction.dsts_blocks = &transaction_dst_block;
	transaction.num_of_dsts = 1;
	transaction.total_dst_bufs = 1;

	if (0 != al_raid_dma_prepare(
		obj->raid_dma,
		obj->raid_dma_qid,
		&transaction)) {
		al_err("al_raid_dma_prepare failed!\n");
		status = -EIO;
		goto done;
	}

	if (0 != al_raid_dma_action(
		obj->raid_dma,
		obj->raid_dma_qid,
		transaction.tx_descs_count)) {
		al_err("al_raid_dma_action failed!\n");
		status = -EIO;
		goto done;
	}

	*num_transactions = 1;

done:

	return status;
}

/******************************************************************************/
/******************************************************************************/
int al_nand_data_buff_read_dma(
	struct al_nand_ctrl_obj	*obj,
	struct al_buf		*buff,
	int			trigger_interrupt)
{
	int status = 0;

	struct al_buf transaction_src_buf;
	struct al_block transaction_src_block;
	struct al_block transaction_dst_block;
	struct al_raid_transaction transaction;

	al_assert(obj);
	al_assert(buff);

	transaction_dst_block.bufs = buff;
	transaction_dst_block.num = 1;

	transaction_src_buf.addr =
		(al_phys_addr_t)(uintptr_t)obj->data_buff_base;
	transaction_src_buf.len = buff->len;

	transaction_src_block.bufs = &transaction_src_buf;
	transaction_src_block.num = 1;

	transaction.op = AL_RAID_OP_MEM_CPY;

	transaction.flags =
		AL_RAID_BARRIER | (trigger_interrupt ? AL_RAID_INTERRUPT : 0);

	transaction.srcs_blocks = &transaction_src_block;
	transaction.num_of_srcs = 1;
	transaction.total_src_bufs = 1;
	transaction.dsts_blocks = &transaction_dst_block;
	transaction.num_of_dsts = 1;
	transaction.total_dst_bufs = 1;

	if (0 != al_raid_dma_prepare(
		obj->raid_dma,
		obj->raid_dma_qid,
		&transaction)) {
		al_err("al_raid_dma_prepare failed!\n");
		status = -EIO;
		goto done;
	}

	if (0 != al_raid_dma_action(
		obj->raid_dma,
		obj->raid_dma_qid,
		transaction.tx_descs_count)) {
		al_err("al_raid_dma_action failed!\n");
		status = -EIO;
		goto done;
	}

done:

	return status;
}

/******************************************************************************/
/******************************************************************************/
int al_nand_data_buff_write_dma(
	struct al_nand_ctrl_obj	*obj,
	struct al_buf			*buff,
	int						trigger_interrupt)
{
	int status = 0;

	struct al_buf transaction_dst_buf;
	struct al_block transaction_dst_block;
	struct al_block transaction_src_block;
	struct al_raid_transaction transaction;

	al_assert(obj);
	al_assert(buff);

	transaction_src_block.bufs = buff;
	transaction_src_block.num = 1;

	transaction_dst_buf.addr =
		(al_phys_addr_t)(uintptr_t)obj->data_buff_base;
	transaction_dst_buf.len = buff->len;

	transaction_dst_block.bufs = &transaction_dst_buf;
	transaction_dst_block.num = 1;

	transaction.op = AL_RAID_OP_MEM_CPY;

	transaction.flags =
		AL_RAID_BARRIER | (trigger_interrupt ? AL_RAID_INTERRUPT : 0);

	transaction.srcs_blocks = &transaction_src_block;
	transaction.num_of_srcs = 1;
	transaction.total_src_bufs = 1;
	transaction.dsts_blocks = &transaction_dst_block;
	transaction.num_of_dsts = 1;
	transaction.total_dst_bufs = 1;

	if (0 != al_raid_dma_prepare(
			obj->raid_dma,
			obj->raid_dma_qid,
			&transaction)) {
		al_err("al_raid_dma_prepare failed!\n");
		status = -EIO;
		goto done;
	}

	if (0 != al_raid_dma_action(
		obj->raid_dma,
		obj->raid_dma_qid,
		transaction.tx_descs_count)) {
		al_err("al_raid_dma_action failed!\n");
		status = -EIO;
		goto done;
	}

done:

	return status;
}

/******************************************************************************/
/******************************************************************************/
int al_nand_transaction_completion(
	struct al_nand_ctrl_obj	*obj,
	uint32_t		*comp_status)
{
	int retVal;

	al_assert(obj);

	retVal = al_raid_dma_completion(
		obj->raid_dma,
		obj->raid_dma_qid,
		comp_status);

	return retVal;
}

/******************************************************************************/
/******************************************************************************/
static int _al_nand_reg_write_dma(
	struct al_nand_ctrl_obj	*obj,
	uintptr_t		reg_addr,
	struct al_buf		*tx_buff,
	int			trigger_interrupt)
{
	int status = 0;

	struct al_buf transaction_dst_buf;
	struct al_block transaction_src_block;
	struct al_block transaction_dst_block;
	struct al_raid_transaction transaction;

	transaction_src_block.bufs = tx_buff;
	transaction_src_block.num = 1;

	transaction_dst_buf.addr = (al_phys_addr_t)reg_addr;
	transaction_dst_buf.len = tx_buff->len;

	transaction_dst_block.bufs = &transaction_dst_buf;
	transaction_dst_block.num = 1;

	transaction.op = AL_RAID_OP_MEM_CPY;

	transaction.flags =
		AL_RAID_BARRIER | (trigger_interrupt ? AL_RAID_INTERRUPT : 0);

	transaction.srcs_blocks = &transaction_src_block;
	transaction.num_of_srcs = 1;
	transaction.total_src_bufs = 1;
	transaction.dsts_blocks = &transaction_dst_block;
	transaction.num_of_dsts = 1;
	transaction.total_dst_bufs = 1;

	if (0 != al_raid_dma_prepare(
		obj->raid_dma,
		obj->raid_dma_qid,
		&transaction)) {
		al_err("al_raid_dma_prepare failed!\n");
		status = -EIO;
		goto done;
	}

	if (0 != al_raid_dma_action(
		obj->raid_dma,
		obj->raid_dma_qid,
		transaction.tx_descs_count)) {
		al_err("al_raid_dma_action failed!\n");
		status = -EIO;
		goto done;
	}

done:

	return status;
}

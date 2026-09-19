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

/**
 * @addtogroup group_nand NAND controller
 * @ingroup group_pbs
 *  @{
 * The NAND controller is activated mostly through executing command sequences
 * and reading/writing from/to its data buffer.
 * Both command sequence execution and data buffer reading/writing can either be
 * obtained directly through memory-mapped read/write access, or through a DMA
 * Command sequences for page reading/writing can be generated using dedicated
 * API functions.
 * Command sequences for other NAND operations can be manually constructed.
 *
 * A typical initialization flow:
 * - al_nand_init
 * - al_nand_dev_select
 * - al_nand_dev_config_basic
 * - al_nand_cmd_seq_execute(reset sequence)
 * - al_nand_cmd_seq_execute(read properties page / read id sequence)
 * - al_nand_data_buff_read
 * - al_nand_cmd_seq_execute(set features sequence)
 * - al_nand_data_buff_write
 * - al_nand_dev_config(device specific config)
 *
 * A typical non DMA page reading flow:
 * - al_nand_cmd_seq_size_page_read
 * - allocate appropriate sequence buffer
 * - al_nand_cmd_seq_gen_page_read
 * - al_nand_cw_config
 * - al_nand_cmd_seq_execute
 * - al_nand_data_buff_read
 *
 * A typical DMA page reading flow:
 * - al_nand_cmd_seq_size_page_read
 * - allocate appropriate sequence physical buffer
 * - al_nand_cmd_seq_gen_page_read
 * - al_nand_cw_config_dma
 * - al_nand_cmd_seq_execute_dma(no interrupt)
 * - al_nand_data_buff_read_dma(with interrupt)
 * - wait for interrupt
 * - al_nand_transaction_completion (for the command sequence)
 * - al_nand_transaction_completion (for the read transaction)
 *
 * @file   al_hal_nand.h
 *
 * @brief Header file for the NAND HAL driver
 *
 */

#ifndef __AL_HAL_NAND_H__
#define __AL_HAL_NAND_H__

#include <mach/al_hal_common.h>
#include <mach/al_hal_ssm.h>
#include "../../../../dma/al/al_hal_ssm_raid.h"
#include "al_hal_nand_defs.h"

/* *INDENT-OFF* */
#ifdef __cplusplus
extern "C" {
#endif
/* *INDENT-ON* */

/**
 * NAND controller initialization
 *
 * Initializes all resources required for operating the NAND controller.
 * This function should be called prior to any other attempt to access the
 * controller.
 * A handle to an object is initialized and shall be used in all other API
 * calls.
 *
 * @param  nand_base
 *             The base address for accessing to NAND buffs and regs.
 *
 * @param  raid_dma
 *             An allocated RAID DMA handle, or NULL if DMA is not required
 *
 * @param  raid_dma_qid
 *             An allocated RAID DMA queue handle (applicable if raid_dma !=
 *             NULL)
 *
 * @param obj
 *             The initialized object
 *
 * @return 0 if no error found.
 *
 */
int al_nand_init(
	struct al_nand_ctrl_obj	*obj,
	void __iomem		*nand_base,
	struct al_ssm_dma	*raid_dma,
	uint32_t		raid_dma_qid);

/**
 * NAND controller termination
 *
 * Releases all resources previously initialized for operating the NAND
 * controller.
 * No function besides 'al_nand_init' can be called after calling this
 * function.
 *
 * @param  obj
 *             The object context
 *
 */
void al_nand_terminate(
	struct al_nand_ctrl_obj	*obj);

/**
 * NAND controller reset
 *
 * Resets various sub-units of the NAND controller according to a mask provided
 * by the caller.
 *
 * @param  obj
 *             The object context
 *
 * @param  reset_mask
 *             A bitwise OR combination of one or more sub-units.
 *
 * @see AL_NAND_RESET_MASK_SOFT
 * @see AL_NAND_RESET_MASK_CMD_FIFO
 * @see AL_NAND_RESET_MASK_DATA_FIFO
 * @see AL_NAND_RESET_MASK_DDRRX_FIFO
 * @see AL_NAND_RESET_MASK_CMD_ENGINE
 * @see AL_NAND_RESET_MASK_TIMING_ENGINE
 *
 */
void al_nand_reset(
	struct al_nand_ctrl_obj	*obj,
	int				reset_mask);

/**
 * NAND device selection
 *
 * Selects one of the devices connected to the NAND controller as the active
 * device. Following device operations will act upon it.
 *
 * @param  obj
 *             The object context
 *
 * @param  device_index
 *             The index of the device to be selected
 *
 * @see AL_NAND_MAX_NUM_DEVICES
 *
 */
void al_nand_dev_select(
	struct al_nand_ctrl_obj	*obj,
	int			device_index);

/**
 * NAND device standard basic config
 *
 * Configures the currently selected NAND device with standard basic
 * config that can be used only for resetting the device and for reading
 * its ID and properties page.
 *
 * @param  obj
 *             The object context
 *
 * @return 0 if no error found.
 *
 */
int al_nand_dev_config_basic(
	struct al_nand_ctrl_obj *obj);

/**
 * NAND device config
 *
 * Configures the currently selected NAND device. The config involves
 * both setting the device properties and ECC config.
 *
 * @param  obj
 *             The object context
 *
 * @param  dev_properties
 *             NAND device properties (device specific)
 *
 * @param  ecc_config
 *             ECC config (application requirements)
 *
 * @return 0 if no error found.
 *
 */
int al_nand_dev_config(
	struct al_nand_ctrl_obj		*obj,
	struct al_nand_dev_properties	*dev_properties,
	struct al_nand_ecc_config	*ecc_config);

/**
 * NAND decode properties
 *
 * Read properties from pbs registers and parse them.
 *
 * @param  pbs_regs_base
 *             PBS regs base address
 *
 * @param dev_properties
 *             NAND device properties (device specific)
 *
 * @param ecc_config
 *             ECC config (application requirements)
 *
 * @param dev_ext_props
 *             NAND device extra properties
 *
 * @return 0 if no error found.
 *
 */
int al_nand_properties_decode(
	void __iomem				*pbs_regs_base,
	struct al_nand_dev_properties		*dev_properties,
	struct al_nand_ecc_config		*ecc_config,
	struct al_nand_extra_dev_properties	*dev_ext_props);

/**
 * NAND code word configuration
 *
 * Configures the code word settings for the next read/write sequence
 *
 * @param  obj
 *             The object context
 *
 * @param  cw_size
 *             Code word size [bytes]
 *
 * @param  cw_count
 *             Code word count
 *
 */
void al_nand_cw_config(
	struct al_nand_ctrl_obj	*obj,
	uint32_t		cw_size,
	uint32_t		cw_count);

/**
 * NAND code word configuration through DMA - preapre buffer
 *
 * Configures the code word settings for the next read/write sequence through
 * DMA - buffer preparation phase.
 *
 * @param  obj
 *             The object context
 *
 * @param  cw_size
 *             Code word size [bytes]
 *
 * @param  cw_count
 *             Code word count
 *
 * @param buff_arr
 *             An allocated temporary DMA transaction buffer array.
 *             The required number of elements in the array is 2.
 *             The required buffer size is 4 bytes.
 *
 */
void al_nand_cw_config_buffs_prepare(
	struct al_nand_ctrl_obj	*obj,
	uint32_t		cw_size,
	uint32_t		cw_count,
	uint32_t		*buff_arr[2]);

/**
 * NAND code word configuration through DMA - send buffer
 *
 * Configures the code word settings for the next read/write sequence through
 * DMA - buffer sending phase.
 * Call 'al_nand_transaction_completion' to check and acknowledge completion.
 *
 * @param  obj
 *             The object context
 *
 * @param tx_buff_arr
 *             A DMA transaction buffer array.
 *             The required number of elements in the array is 2.
 *             The required buffer size is 4 bytes.
 *             The buffers in the array can only be freed upon completion.
 *
 * @param  trigger_interrupt
 *             Whether or not interrupt should be triggered upon completion
 *             of this transaction
 *
 * @param num_transactions
 *             The number of DMA transactions generated by this request.
 *             Each should be acknowledged by calling
 *             'al_nand_transaction_completion'.
 *
 * @return 0 if no error found.
 *
 */
int al_nand_cw_config_dma(
	struct al_nand_ctrl_obj	*obj,
	struct al_buf		tx_buff_arr[2],
	int			trigger_interrupt,
	int			*num_transactions);

/**
 * NAND ECC enable state setting
 *
 * Enables/disables ECC
 *
 * @param  obj
 *             The object context
 *
 * @param  enabled
 *             The required ECC enable state
 *
 */
void al_nand_ecc_set_enabled(
	struct al_nand_ctrl_obj	*obj,
	int			enabled);

/**
 * Write protection enabling/disabling
 *
 * Enables or disables NAND device write protection (by controlling the WP
 * signal)
 *
 * @param  obj
 *             The object context
 *
 * @param  enable
 *             A flag for either enabling or disabling write protection
 *
 */
void al_nand_wp_set_enable(
	struct al_nand_ctrl_obj	*obj,
	int			enable);

/**
 * TX enabling/disabling
 *
 * Enables or disables NAND device TX mode
 *
 * @param  obj
 *             The object context
 *
 * @param  enable
 *             A flag for either enabling or disabling the TX mode
 *
 */
void al_nand_tx_set_enable(
	struct al_nand_ctrl_obj	*obj,
	int			enable);

/**
 * Miscellaneous control through DMA - buffer prepare phase
 *
 * Miscellaneous control through DMA:
 * - Enables or disables NAND device write protection (by controlling the WP
 * signal)
 * - Enables or disables NAND device TX mode
 *
 * @param  obj
 *             The object context
 *
 * @param  wp_enable
 *             A flag for either enabling or disabling write protection
 *
 * @param  tx_enable
 *             A flag for either enabling or disabling the TX mode
 *
 * @param tx_buff_arr
 *             An allocated temporary DMA transaction buffer array.
 *             The required number of elements in the array is 1.
 *             The required buffer size is 4 bytes.
 *
 */
void al_nand_misc_ctrl_buffs_prepare(
	struct al_nand_ctrl_obj	*obj,
	int			wp_enable,
	int			tx_enable,
	uint32_t		*tx_buff_arr[1]);

/**
 * Miscellaneous control through DMA - buffer sending phase
 *
 * Miscellaneous control through DMA:
 * - Enables or disables NAND device write protection (by controlling the WP
 * signal)
 * - Enables or disables NAND device TX mode
 *
 * @param  obj
 *             The object context
 *
 * @param tx_buff_arr
 *             An allocated DMA transaction buffer array.
 *             The required number of elements in the array is 1.
 *             The required buffer size is 4 bytes.
 *             The buffers in the array can only be freed upon completion.
 *
 * @param  trigger_interrupt
 *             Whether or not interrupt should be triggered upon completion
 *             of this request
 *
 * @param num_transactions
 *             The number of DMA transactions generated by this request.
 *             Each should be acknowledged by calling
 *             'al_nand_transaction_completion'.
 *
 * @return 0 if no error found.
 *
 */
int al_nand_misc_ctrl_dma(
	struct al_nand_ctrl_obj	*obj,
	struct al_buf		tx_buff_arr[1],
	int			trigger_interrupt,
	int			*num_transactions);

/**
 * Uncorrectable Error Status Getting
 *
 * Gets current status of uncorrectable errors - whether happened or not
 *
 * @param  obj
 *             The object context
 *
 * @return 0 if no uncorrectable errors
 * @return 1 if uncorrectable errors
 *
 */
int al_nand_uncorr_err_get(
	struct al_nand_ctrl_obj	*obj);

/**
 * Uncorrectable Error Status Clearing
 *
 * Clears current status of uncorrectable errors
 *
 * @param  obj
 *             The object context
 *
 */
void al_nand_uncorr_err_clear(
	struct al_nand_ctrl_obj	*obj);

/**
 * Correctable Error Status Getting
 *
 * Gets current status of correctable errors - whether happened or not
 *
 * @param  obj
 *             The object context
 *
 * @return 0 if no correctable errors
 * @return 1 if correctable errors
 *
 * @return 0 if no error found.
 *
 */
int al_nand_corr_err_get(
	struct al_nand_ctrl_obj	*obj);

/**
 * Correctable Error Status Clearing
 *
 * Clears current status of correctable errors
 *
 * @param  obj
 *             The object context
 *
 */
void al_nand_corr_err_clear(
	struct al_nand_ctrl_obj	*obj);

/**
 * NAND device testing for being ready
 *
 * Checks the state of the NAND device ready/busy# signal
 *
 * @param  obj
 *             The object context
 *
 * @return A flag indicating whether the device is ready of busy.
 *
 */
int al_nand_dev_is_ready(
	struct al_nand_ctrl_obj	*obj);

/**
 * NAND device page reading command sequence size obtaining
 *
 * Obtains the required size for a command sequence for reading a NAND device
 * page.
 *
 * @param  obj
 *             The object context
 *
 * @param  num_bytes
 *             The number of bytes to read
 *
 * @param  ecc_enabled
 *             Whether or not to enable HW ECC
 *
 * @param cmd_seq_buff_num_entries
 *             The required number of entries
 *
 * @return 0 if no error found.
 *
 */
int al_nand_cmd_seq_size_page_read(
	struct al_nand_ctrl_obj	*obj,
	int			num_bytes,
	int			ecc_enabled,
	int			*cmd_seq_buff_num_entries);

/**
 * NAND device page reading command sequence generation
 *
 * Generates a command sequence for reading a a NAND device page.
 *
 * @param  obj
 *             The object context
 *
 * @param  column
 *             The byte address within the page
 *
 * @param  row
 *             The page address
 *
 * @param  num_bytes
 *             The number of bytes to read
 *
 * @param  ecc_enabled
 *             Whether or not to enable HW ECC
 *
 * @param  cmd_seq_buff
 *             An allocated command sequence buffer
 *
 * @param cmd_seq_buff_num_entries
 *             in: the number of entries in the command sequence buffer
 *             out: the number of used entries in the command sequence buffer
 *
 * @param cw_size
 *             The code word size to be configured prior to executing the
 *             sequence
 *
 * @param cw_count
 *             The code word count to be configured prior to executing the
 *             sequence
 *
 * @return 0 if no error found.
 *
 */
int al_nand_cmd_seq_gen_page_read(
	struct al_nand_ctrl_obj	*obj,
	int			column,
	int			row,
	int			num_bytes,
	int			ecc_enabled,
	uint32_t		*cmd_seq_buff,
	int			*cmd_seq_buff_num_entries,
	uint32_t		*cw_size,
	uint32_t		*cw_count);

/**
 * NAND device page writing command sequence size obtaining
 *
 * Obtains the required size for a command sequence for writing a NAND device
 * page.
 *
 * @param  obj
 *             The object context
 *
 * @param  num_bytes
 *             The number of bytes to write
 *
 * @param  ecc_enabled
 *             Whether or not to enable HW ECC
 *
 * @param cmd_seq_buff_num_entries
 *             The required number of entries
 *
 */
void al_nand_cmd_seq_size_page_write(
	struct al_nand_ctrl_obj	*obj,
	int			num_bytes,
	int			ecc_enabled,
	int			*cmd_seq_buff_num_entries);

/**
 * NAND device page writing command sequence generation
 *
 * Generates a command sequence for writing a a NAND device page.
 *
 * @param  obj
 *             The object context
 *
 * @param  column
 *             The byte address within the page
 *
 * @param  row
 *             The page address
 *
 * @param  num_bytes
 *             The number of bytes to write
 *
 * @param  ecc_enabled
 *             Whether or not to enable HW ECC
 *
 * @param  cmd_seq_buff
 *             An allocated command sequence buffer
 *
 * @param cmd_seq_buff_num_entries
 *             in: the number of entries in the command sequence buffer
 *             out: the number of used entries in the command sequence buffer
 *
 * @param cw_size
 *             The code word size to be configured prior to executing the
 *             sequence
 *
 * @param cw_count
 *             The code word count to be configured prior to executing the
 *             sequence
 *
 * @return 0 if no error found.
 *
 */
int al_nand_cmd_seq_gen_page_write(
	struct al_nand_ctrl_obj	*obj,
	int			column,
	int			row,
	int			num_bytes,
	int			ecc_enabled,
	uint32_t		*cmd_seq_buff,
	int			*cmd_seq_buff_num_entries,
	uint32_t		*cw_size,
	uint32_t		*cw_count);

/**
 * NAND controller command constructor
 *
 * Constructs a NAND controller command
 *
 * @param  type
 *             The command type
 *
 * @param  arg
 *             The command argument
 *
 * @see al_nand_command_type
 *
 * @return 0 if no error found.
 *
 */
#define AL_NAND_CMD_SEQ_ENTRY(type, arg)	\
	(((type) << 8) | (arg))

/**
 * NAND controller single command execution
 *
 * Executes a single NAND controller command.
 *
 * @param  obj
 *             The object context
 *
 * @param  cmd
 *             The command to be executed
 *
 */
void al_nand_cmd_single_execute(
	struct al_nand_ctrl_obj	*obj,
	uint32_t		cmd);

/**
 * NAND controller command sequence execution
 *
 * Executes a NAND controller command sequence.
 *
 * @param  obj
 *             The object context
 *
 * @param  cmd_seq_buff
 *             The command sequence buffer
 *
 * @param  cmd_seq_buff_num_entries
 *             The command sequence buffer number of entries
 *
 */
void al_nand_cmd_seq_execute(
	struct al_nand_ctrl_obj	*obj,
	uint32_t		*cmd_seq_buff,
	int			cmd_seq_buff_num_entries);

/**
 * NAND controller command sequence execution
 *
 * Executes a NAND controller command sequence.
 *
 * @param  obj
 *             The object context
 *
 * @return An indication of whether the command buffer is empty
 *
 */
int al_nand_cmd_buff_is_empty(
	struct al_nand_ctrl_obj	*obj);

/**
 * NAND controller command sequence execution through DMA
 *
 * Begins executing a NAND controller command sequence using a DMA.
 * Call 'al_nand_transaction_completion' to check and acknowledge completion.
 *
 * @param  obj
 *             The object context
 *
 * @param  cmd_seq_buff
 *             The command sequence buffer
 *             The buffer shall remain allocated until transaction completion
 *
 * @param  trigger_interrupt
 *             Whether or not interrupt should be triggered upon completion
 *             of this transaction
 *
 * @return 0 if no error found.
 *
 */
int al_nand_cmd_seq_execute_dma(
	struct al_nand_ctrl_obj	*obj,
	struct al_buf		*cmd_seq_buff,
	int			trigger_interrupt);

/**
 * NAND controller command FIFO scion sending through DMA - buffer preparation
 * phase
 *
 * Sends NAND controller command FIFO scion using a DMA.
 *
 * @param  obj
 *             The object context
 *
 * @param buff
 *             An allocated DMA transaction buffer.
 *             The required buffer size is 128 bytes.
 *
 * @return 0 if no error found.
 *
 */
void al_nand_cmd_seq_scion_buff_prepare(
	struct al_nand_ctrl_obj	*obj,
	uint32_t		*buff);

/**
 * NAND controller command FIFO scion sending through DMA - buffer sending
 * phase
 *
 * Sends NAND controller command FIFO scion using a DMA.
 * Call 'al_nand_transaction_completion' to check and acknowledge completion.
 *
 * @param  obj
 *             The object context
 *
 * @param tx_buff
 *             An allocated temporary DMA transaction buffer.
 *             The required buffer size is 128 bytes.
 *             The buffers in the array can only be freed upon completion.
 *
 * @param  trigger_interrupt
 *             Whether or not interrupt should be triggered upon completion
 *             of this transaction
 *
 * @param num_transactions
 *             The number of DMA transactions generated by this request.
 *             Each should be acknowledged by calling
 *             'al_nand_transaction_completion'.
 *
 * @return 0 if no error found.
 *
 */
int al_nand_cmd_seq_scion_dma(
	struct al_nand_ctrl_obj	*obj,
	struct al_buf		*tx_buff,
	int			trigger_interrupt,
	int			*num_transactions);

/**
 * Get the data buff address.
 *
 * @param  obj
 *             The object context
 *
 * @return the data buff address
 *
 */
void __iomem *al_nand_data_buff_base_get(
			struct al_nand_ctrl_obj	*obj);

/**
 * NAND controller data buffer reading
 *
 * Reads from the NAND controller data buffer.
 * Data become available in the data buffer according to prior commands being
 * written to the controller command FIFO.
 * This function is blocking.
 *
 * @param  obj
 *             The object context
 *
 * @param  num_bytes
 *             The number of bytes to read
 *
 * @param  num_bytes_skip_head
 *             The number of bytes to skip at the beginning of reading
 *
 * @param  num_bytes_skip_tail
 *             The number of bytes to skip at the end of reading
 *
 * @param buff
 *             The read data
 *
 * @return 0 if no error found.
 *
 */
int al_nand_data_buff_read(
	struct al_nand_ctrl_obj	*obj,
	int			num_bytes,
	int			num_bytes_skip_head,
	int			num_bytes_skip_tail,
	uint8_t			*buff);

/**
 * NAND controller data buffer reading through DMA
 *
 * Begins reading from the NAND controller data buffer using a DMA.
 * Call 'al_nand_transaction_completion' to check and acknowledge completion.
 *
 * @param  obj
 *             The object context
 *
 * @param  buff
 *             The read data
 *             The buffer shall remain allocated until transaction completion
 *
 * @param  trigger_interrupt
 *             Whether or not interrupt should be triggered upon completion
 *             of this transaction
 *
 * @return 0 if no error found.
 *
 */
int al_nand_data_buff_read_dma(
	struct al_nand_ctrl_obj	*obj,
	struct al_buf		*buff,
	int			trigger_interrupt);

/**
 * NAND controller data buffer writing
 *
 * Writes to the NAND controller data buffer.
 *
 * @param  obj
 *             The object context
 *
 * @param  num_bytes
 *             The number of bytes to write
 *
 * @param  buff
 *             The data to write
 *
 * @return 0 if no error found.
 *
 */
int al_nand_data_buff_write(
	struct al_nand_ctrl_obj	*obj,
	int			num_bytes,
	const uint8_t		*buff);

/**
 * NAND controller data buffer writing through DMA
 *
 * Begins writing to the NAND controller data buffer using a DMA.
 * Call 'al_nand_transaction_completion' to check and acknowledge completion.
 *
 * @param  obj
 *             The object context
 *
 * @param  buff
 *             The data to write
 *             The buffer shall remain allocated until transaction completion
 *
 * @param  trigger_interrupt
 *             Whether or not interrupt should be triggered upon completion
 *             of this transaction
 *
 * @return 0 if no error found.
 *
 */
int al_nand_data_buff_write_dma(
	struct al_nand_ctrl_obj	*obj,
	struct al_buf		*buff,
	int			trigger_interrupt);

/**
 * Check and cleanup completed transaction
 *
 * @param  obj
 *             The object context
 *
 * @param comp_status
 *             The status reported by rx completion descriptor
 *
 * @return 1 if a transaction was completed. 0 otherwise
 *
 */
int al_nand_transaction_completion(
	struct al_nand_ctrl_obj	*obj,
	uint32_t		*comp_status);

/**
 * Get the interrupt status register
 *
 * @param  obj
 *             The object context
 *
 * @return the interrupt status register value
 *
 */
uint32_t al_nand_int_status_get(
		struct al_nand_ctrl_obj	*obj);

/**
 * Enable interrupts for the mask status
 *
 * @param  obj
 *             The object context
 *
 * @param  int_mask
 *             the interrupt's status mask to enable
 *
 */
void al_nand_int_enable(
		struct al_nand_ctrl_obj	*obj,
		uint32_t int_mask);

/**
 * Disable interrupts for the mask status
 *
 * @param  obj
 *             The object context
 *
 * @param  int_mask
 *             the interrupt's status mask to disable
 *
 */
void al_nand_int_disable(
		struct al_nand_ctrl_obj	*obj,
		uint32_t int_mask);

/**
 * Clear interrupts for the mask status
 *
 * @param  obj
 *             The object context
 *
 * @param  int_mask
 *             the interrupt's status mask to clear
 *
 */
void al_nand_int_clear(
		struct al_nand_ctrl_obj	*obj,
		uint32_t int_mask);

/* *INDENT-OFF* */
#ifdef __cplusplus
}
#endif
/* *INDENT-ON* */
/** @} end of NAND group */
#endif		/* __AL_HAL_NAND_H__ */

/* SPDX-License-Identifier: GPL-2.0-only OR BSD-2-Clause */
/* Copyright (C) 2013 Annapurna Labs Ltd. */

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
#include "al_hal_nand_defs.h"

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
 * @param obj
 *             The initialized object
 *
 * @return 0 if no error found.
 *
 */
int al_nand_init(
	struct al_nand_ctrl_obj	*obj,
	void __iomem		*nand_base);

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

/** @} end of NAND group */
#endif		/* __AL_HAL_NAND_H__ */

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
 * @file   al_hal_nand_defs.h
 *
 * @brief Header file for the NAND HAL driver definitions
 *
 */

#ifndef __AL_HAL_NAND_DEFS_H__
#define __AL_HAL_NAND_DEFS_H__

/* *INDENT-OFF* */
#ifdef __cplusplus
extern "C" {
#endif
/* *INDENT-ON* */

/** Soft Reset
 */
#define AL_NAND_RESET_MASK_SOFT			(1 << 0)

/** Command FIFO Reset
 */
#define AL_NAND_RESET_MASK_CMD_FIFO		(1 << 1)

/** Data FIFO Reset
 */
#define AL_NAND_RESET_MASK_DATA_FIFO		(1 << 2)

/** DDR receive FIFO reset
 */
#define AL_NAND_RESET_MASK_DDRRX_FIFO		(1 << 3)

/** CMD Engine module reset
 * It will reset command engine after finishing currently executing
 * instruction.
 */
#define AL_NAND_RESET_MASK_CMD_ENGINE		(1 << 4)

/** Timing Engine reset module
 * It waits for current operation to finish on Nand Bus before applying reset
 * to avoid timing glitches on Nand Bus. Later this also clears command buffer.
 */
#define AL_NAND_RESET_MASK_TIMING_ENGINE	(1 << 5)

/** Title
 * Description
 */
#define AL_NAND_MAX_NUM_DEVICES	8

/*** interrupts status ***/
/* Command buffer empty status */
#define AL_NAND_INTR_STATUS_CMD_BUF_EMPTY		(1 << 0)
/* Command buffer full status */
#define AL_NAND_INTR_STATUS_CMD_BUF_FULL	        (1 << 1)
/* Data buffer empty status */
#define AL_NAND_INTR_STATUS_DATA_BUF_EMPTY	        (1 << 2)
/* Data buffer full status */
#define AL_NAND_INTR_STATUS_DATA_BUF_FULL	        (1 << 3)
/* Correctable error status */
#define AL_NAND_INTR_STATUS_CORR_ERROR                  (1 << 4)
/* Uncorrectable error status */
#define AL_NAND_INTR_STATUS_UNCORR_ERROR                (1 << 5)
/* Indicate if PKT_SIZE amount of space
 * is available in Buffer */
#define AL_NAND_INTR_STATUS_BUF_WRRDY                   (1 << 6)
/* Indicate if PKT_SIZE amount of data
 * is available in Buffer. */
#define AL_NAND_INTR_STATUS_BUF_RDRDY                   (1 << 7)
/* set when the specified operation mentioned
 * in the COMMAND_REG is completed */
#define AL_NAND_INTR_STATUS_WRRD_DONE                   (1 << 8)
/* set when the DMA Based write/read is completed.
 * Asserted high for both Write and Read operation
 * on every page buffer boundary for DMA. */
#define AL_NAND_INTR_STATUS_DMA_DONE                    (1 << 9)
/* Set high for both Write and Read operation
 * on end of every DMA Transaction */
#define AL_NAND_INTR_STATUS_TRANS_COMP                  (1 << 10)
/* Command buffer overflow status */
#define AL_NAND_INTR_STATUS_CMD_BUF_OVERFLOW            (1 << 11)
/* Command buffer underflow status */
#define AL_NAND_INTR_STATUS_CMD_BUF_UNDERFLOW           (1 << 12)
/* Data buffer overflow status */
#define AL_NAND_INTR_STATUS_DATA_BUF_OVERFLOW           (1 << 13)
/* Data buffer underflow status */
#define AL_NAND_INTR_STATUS_DATA_BUF_UNDERFLOW          (1 << 14)
/* DMA transaction done interrupt status */
#define AL_NAND_INTR_STATUS_DMA_TRANS_DONE              (1 << 15)
/* DMA Buffer boundary Cross interrupt status */
#define AL_NAND_INTR_STATUS_DMA_BOUNDARY_CROSS		(1 << 16)
/* Slave error interrupt during DMA operations */
#define AL_NAND_INTR_STATUS_SLAVE_ERROR			(1 << 17)

/** Title
 * Description
 */
enum al_nand_device_timing_mode {
	AL_NAND_DEVICE_TIMING_MODE_ONFI_0	= 0,
	AL_NAND_DEVICE_TIMING_MODE_ONFI_1	= 1,
	AL_NAND_DEVICE_TIMING_MODE_ONFI_2	= 2,
	AL_NAND_DEVICE_TIMING_MODE_ONFI_3	= 3,
	AL_NAND_DEVICE_TIMING_MODE_ONFI_4	= 4,
	AL_NAND_DEVICE_TIMING_MODE_ONFI_5	= 5,
	AL_NAND_DEVICE_TIMING_MODE_MANUAL	= 6,
};

/** NAND device SDR timing - read delay
 * Delay that is used for sampling flash_data after re_n goes high (number of
 * cycles after re_n).
 */
enum al_nand_device_timing_sdr_read_delay {
	AL_NAND_DEVIE_TIMING_READ_DELAY_1 = 2,
	AL_NAND_DEVIE_TIMING_READ_DELAY_2 = 3,
};

/** Title
 * Description
 */
struct al_nand_device_timing {

	/** Setup time [controller cycles]
	 * The time between assertion of the ALE, CLE, I/O signals and WE_L
	 * assertion.
	 */
	uint8_t tSETUP;

	/** Hold time [controller cycles]
	 * The time ALE, CLE or I/O is held asserted after the desertion of the
	 * WE_L signal.
	 */
	uint8_t tHOLD;

	/** WE_n high pulse width [controller cycles]
	 */
	uint8_t tWH;

	/** WE_L and RE_L assertions duration [controller cycles]
	 */
	uint8_t tWRP;

	/** Inter command gap [controller cycles]
	 */
	uint8_t tINTCMD;

	/** Ready to data output cycle [controller cycles]
	 */
	uint8_t tRR;

	/** The waiting time for specific command operations [controller cycles]
	 * This timing should be provided whenever cmd1 present is set.
	 * For example, Read Command requires the waiting time (tWB) to specify
	 * the wait duration between the last Command Latch Cycle and R/~B.
	 */
	uint8_t tWB;

	/** Read delay (SDR only)
	 */
	uint8_t readDelay;

	/** Clock divider value [1 - 64] (DDR/DDR2 only)
	 */
	int tCLKDiv;

	/** CE_n setup time [controller cycles] (DDR/DDR2 only)
	 */
	int tCE_n;

	/** tDQS in delay tap value (DDR/DDR2 only)
	 */
	int tDQS_in;

	/** tDQS out delay (DDR/DDR2 only)
	 */
	int tDQS_out;
};

/** Title
 * Description
 */
enum al_nand_device_sdr_data_width {
	AL_NAND_DEVICE_SDR_DATA_WIDTH_8		= 0,
	AL_NAND_DEVICE_SDR_DATA_WIDTH_16	= 1,	/* Not supported */
};

/** Title
 * Description
 */
enum al_nand_device_page_size {
	AL_NAND_DEVICE_PAGE_SIZE_2K	= 0,
	AL_NAND_DEVICE_PAGE_SIZE_4K	= 1,
	AL_NAND_DEVICE_PAGE_SIZE_8K	= 2,
	AL_NAND_DEVICE_PAGE_SIZE_16K	= 3,

	/* TODO: Check if really supported */
	AL_NAND_DEVICE_PAGE_SIZE_512	= 4,
};

/** Title
 * Description
 */
struct al_nand_dev_properties {
	enum al_nand_device_timing_mode		timingMode;

	enum al_nand_device_sdr_data_width	sdrDataWidth;

	struct al_nand_device_timing		timing;

	/** Timeout value for ready busy signal
	 * 0 - r_b_n pin value is used for checking ready busy
	 */
	int					readyBusyTimeout;

	int					num_col_cyc;

	int					num_row_cyc;

	enum al_nand_device_page_size		pageSize;
};

/** Title
 * Description
 */
enum al_nand_ecc_algorithm {
	AL_NAND_ECC_ALGORITHM_HAMMING	= 0,
	AL_NAND_ECC_ALGORITHM_BCH	= 1,
};

/** Title
 * Description
 */
enum al_nand_ecc_bch_num_corr_bits {
	AL_NAND_ECC_BCH_NUM_CORR_BITS_4		= 0,
	AL_NAND_ECC_BCH_NUM_CORR_BITS_8		= 1,
	AL_NAND_ECC_BCH_NUM_CORR_BITS_12	= 2,
	AL_NAND_ECC_BCH_NUM_CORR_BITS_16	= 3,
	AL_NAND_ECC_BCH_NUM_CORR_BITS_20	= 4,
	AL_NAND_ECC_BCH_NUM_CORR_BITS_24	= 5,
	AL_NAND_ECC_BCH_NUM_CORR_BITS_28	= 6,
	AL_NAND_ECC_BCH_NUM_CORR_BITS_32	= 7,
	AL_NAND_ECC_BCH_NUM_CORR_BITS_36	= 8,
	AL_NAND_ECC_BCH_NUM_CORR_BITS_40	= 9,
};

/** Title
 * Description
 */
enum al_nand_ecc_bch_message_size {
	AL_NAND_ECC_BCH_MESSAGE_SIZE_512	= 0,
	AL_NAND_ECC_BCH_MESSAGE_SIZE_1024	= 1,
};

/** Title
 * Description
 */
struct al_nand_ecc_config {
	enum al_nand_ecc_algorithm		algorithm;

	enum al_nand_ecc_bch_num_corr_bits	num_corr_bits;

	enum al_nand_ecc_bch_message_size	messageSize;

	int					spareAreaOffset;
};

enum al_nand_bad_block_marking_method {
	NAND_BAD_BLOCK_MARKING_METHOD_DISABLED = 0,
	NAND_BAD_BLOCK_MARKING_CHECK_1ST_PAGE,
	NAND_BAD_BLOCK_MARKING_CHECK_1ST_PAGES,
	NAND_BAD_BLOCK_MARKING_CHECK_LAST_PAGE,
	NAND_BAD_BLOCK_MARKING_CHECK_LAST_PAGES,
};

struct al_nand_bad_block_marking {
	enum al_nand_bad_block_marking_method	method;
	int					location1;
	int					location2;
};

struct al_nand_extra_dev_properties {
	unsigned int				pageSize;
	unsigned int				blockSize;
	unsigned int				wordSize;

	struct al_nand_bad_block_marking	badBlockMarking;

	int					eccIsEnabled;
};

/** Nand Controller Object */
struct al_nand_ctrl_obj {
	struct al_nand_regs		*regs_base;

	struct al_nand_wrap_regs	*wrap_regs_base;
	void				*cmd_buff_base;
	void				*data_buff_base;

	struct al_ssm_dma		*raid_dma;
	uint32_t			raid_dma_qid;

	struct al_nand_dev_properties	dev_properties;
	struct al_nand_ecc_config	ecc_config;
	int				current_dev_index;

	uint32_t			cw_size;
	uint32_t			cw_count;

	uint32_t			cw_size_remaining;
	uint32_t			cw_count_remaining;
};

enum al_nand_command_type {
	/** no operation
	 */
	AL_NAND_COMMAND_TYPE_NOP		= 0,

	/** Command
	 * Activate CLE and write a command byte to the NAND device
	 * Argument: 1 byte command
	 */
	AL_NAND_COMMAND_TYPE_CMD		= 2,

	/** Address
	 * Activate ALE and write an address byte to the NAND device
	 * Argument: 1 byte address (repeat for several address cycles)
	 */
	AL_NAND_COMMAND_TYPE_ADDRESS		= 3,

	/** Wait cycle count
	 */
	AL_NAND_COMMAND_TYPE_WAIT_CYCLE_COUNT	= 4,

	/** Wait for ready
	 * Wait for the NAND device to become ready
	 */
	AL_NAND_COMMAND_TYPE_WAIT_FOR_READY	= 5,

	/** Count for read data
	 * Requests reading of bytes from the NAND device.
	 * This field is 2 bytes (LSB first).
	 * Argumet: LSB upon first occurance, MSB upon second occurance
	 */
	AL_NAND_COMMAND_TYPE_DATA_READ_COUNT	= 6,

	/** Count for write data
	 * Requests writing of bytes to the NAND device.
	 * This field is 2 bytes (LSB first).
	 * Argumet: LSB upon first occurance, MSB upon second occurance
	 */
	AL_NAND_COMMAND_TYPE_DATA_WRITE_COUNT	= 7,

	/** Status read command
	 * Requests reading of NAND device status.
	 * Argumet: Number of status bytes to read (1-4)
	 */
	AL_NAND_COMMAND_TYPE_STATUS_READ	= 8,

	/** Spare read
	 */
	AL_NAND_COMMAND_TYPE_SPARE_READ_COUNT	= 9,

	/** Spare write
	 */
	AL_NAND_COMMAND_TYPE_SPARE_WRITE_COUNT	= 10,

	/** Status write command
	 * (Write command with direct data in cmd)
	 */
	AL_NAND_COMMAND_TYPE_STATUS_WRITE	= 11,
};

struct al_nand_command {
	enum al_nand_command_type	type;
	uint8_t				argument;
};

#ifdef __cplusplus
}
#endif
/* *INDENT-ON* */
/** @} end of NAND group */
#endif		/* __AL_HAL_NAND_DEFS_H__ */

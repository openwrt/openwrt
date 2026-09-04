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
 * @defgroup groupddr DDR controller & PHY hardrware abstraction layer
 *  @{
 * @file   al_hal_ddr.h
 *
 * @brief Header file for the DDR HAL driver
 */

#ifndef __AL_HAL_DDR_H__
#define __AL_HAL_DDR_H__

#include <mach/al_hal_common.h>
#include "al_hal_ddr_cfg.h"

/* *INDENT-OFF* */
#ifdef __cplusplus
extern "C" {
#endif
/* *INDENT-ON* */

/**
 * DDR address mapping - not connected bit
 * See explanation about al_ddr_addrmap below.
 */
#define AL_DDR_ADDRMAP_NC	0xff

/* Data Width */
enum al_ddr_data_width {
	AL_DDR_DATA_WIDTH_32_BITS,
	AL_DDR_DATA_WIDTH_64_BITS,
};

/**
 * Address mapping:
 * Read and write requests are provided to the DDR controller with a system
 * address.  The system address is the command address of a transaction as
 * presented on one of the data ports. The DDR controller is responsible for
 * mapping this system address to rank, bank, row, and column addresses to the
 * SDRAM. It converts the system address to a physical address.
 * For each CS/bank/column/row bit assign a system memory address bit index.
 * Set to AL_DDR_ADDRMAP_NC if not connected.
 * CS minimal supported memory address bit index is 10.
 * Bank minimal supported memory address bit index is 6.
 * Column minimal supported memory address bit index is 4.
 * Row minimal supported memory address bit index is 10.
 *
 * Address mapping might affect the system performance and should be optimized
 * according to the specific application nature. The basic guideline is keeping
 * as much open pages as possible and avoiding frequent closing of pages and
 * opening new ones.
 *
 * Example:
 * Mapping of 16GB memory device with 64 bits data width, 1KB page
 *
 * System address bit index |	SDRAM required mapping
 * ----------------------------------------------------
 * 33:32			cs[1:0]
 * 31:16			row[15:0]
 * 15:13			bank[2:0]
 * 12:3				col[9:0]
 * 2:0				N/A since 8 bytes are accessed at a time
 *
 * In this case the following setting is required:
 * col_b3_9_b11_13 = { 6, 7, 8, 9, 10, 11, 12, AL_DDR_ADDRMAP_NC, ... }
 * bank_b0_2 = { 13, 14, 15 }
 * row_b0_2_10 = { 16, 17, 18 }
 * row_b11_15 = { 27, 28, 29, 30, 31 }
 * cs_b0_1 = { 32, 33 }
 */
struct al_ddr_addrmap {
	/**
	 * Column bits 3 - 9, 11 - 13
	 * Bit 3 relevant only for 64 bits data bus
	 * Bit 13 relevant only for 32 bits data bus
	 */
	uint8_t col_b3_9_b11_13[10];

	/* Bank bits 0 - 2 */
	uint8_t bank_b0_2[3];

	/**
	 * Row bits 0 - 2
	 * Bits 3 - 10 are following bit 2
	 */
	uint8_t row_b0_2_10[3];

	/* Column bits 11 - 15 */
	uint8_t row_b11_15[5];

	/* CS bits 0 - 1 */
	uint8_t	cs_b0_1[2];
};


/* Data BIST mode */
enum al_ddr_bist_mode {
	/**
	 * Loopback mode:
	 * Address, commands and data loop back at the PHY I/Os
	 */
	AL_DDR_BIST_MODE_LOOPBACK,

	/**
	 * DRAM mode:
	 * Address, commands and data go to DRAM for normal memory accesses.
	 */
	AL_DDR_BIST_MODE_DRAM,
};

/* Data/AC BIST pattern */
enum al_ddr_bist_pat {
	/* Walking '0' */
	AL_DDR_BIST_PATTERN_WALK_0,

	/* Walking '1' */
	AL_DDR_BIST_PATTERN_WALK_1,

	/* LFSR-based pseudo-random */
	AL_DDR_BIST_PATTERN_LFSR,

	/* User programmable (Not valid for AC loopback) */
	AL_DDR_BIST_PATTERN_USER,
};

/* Data BIST parameters */
struct al_ddr_bist_params {
	/* Mode */
	enum al_ddr_bist_mode	mode;

	/* Pattern */
	enum al_ddr_bist_pat	pat;

	/**
	 * User Data Pattern 0:
	 * Data to be applied on even DQ pins during BIST.
	 * Valid values: 0x0000 - 0xffff
	 */
	unsigned int		user_pat_even;

	/**
	 * User Data Pattern 1:
	 * Data to be applied on odd DQ pins during BIST.
	 * Valid values: 0x0000 - 0xffff
	 */
	unsigned int		user_pat_odd;

	/** Word count
	 * Indicates the number of words to generate during BIST.
	 * Valid values are 4, 8, 12, 16, and so on.
	 * Maximal value: 0xfffc
	 */
	unsigned int		wc;

	/** Address increment
	 * Selects the value by which the SDRAM address is incremented for each
	 * write/read access. This value must be at the beginning of a burst
	 * boundary, i.e. the lower bits must be "000".
	 * Maximal value: 0xff8
	 */
	unsigned int		inc;

	/**
	 * BIST Column Address:
	 * Selects the SDRAM column address to be used during BIST. The lower
	 * bits of this address must be "000".
	 */
	unsigned int		col_min;

	/**
	 * BIST Maximum Column Address:
	 * Specifies the maximum SDRAM column address to be used during BIST
	 * before the address increments to the next row.
	 */
	unsigned int		col_max;

	/**
	 * BIST Row Address:
	 * Selects the SDRAM row address to be used during BIST.
	 */
	unsigned int		row_min;

	/**
	 * BIST Maximum Row Address:
	 * Specifies the maximum SDRAM row address to be used during BIST
	 * before the address increments to the next bank.
	 */
	unsigned int		row_max;

	/**
	 * BIST Bank Address:
	 * Selects the SDRAM bank address to be used during BIST.
	 */
	unsigned int		bank_min;

	/**
	 * BIST Maximum Bank Address:
	 * Specifies the maximum SDRAM bank address to be used during BIST
	 * before the address increments to the next rank.
	 */
	unsigned int		bank_max;

	/**
	 * BIST Rank:
	 * Selects the SDRAM rank to be used during BIST.
	 */
	unsigned int		rank_min;

	/**
	 * BIST Maximum Rank:
	 * Specifies the maximum SDRAM rank to be used during BIST.
	 */
	unsigned int		rank_max;

	/**
	 * Active byte lanes to have the BIST applied upon.
	 * Lanes 0-3 can always have BIST applied upon.
	 * Lane 4 - only if ECC is supported by the DDR device.
	 * Lanes 5-8 - only for 64 bits data bus width.
	 */
	uint8_t			active_byte_lanes[AL_DDR_PHY_NUM_BYTE_LANES];
};

/* ECC status parameters */
struct al_ddr_ecc_status {
	/* Number of ECC errors detected */
	unsigned int err_cnt;

	/* Rank number of a read resulting in an ECC error */
	unsigned int rank;

	/* Bank number of a read resulting in an ECC error */
	unsigned int bank;

	/* Row number of a read resulting in an ECC error */
	unsigned int row;

	/* Collumn number of a read resulting in an ECC error */
	unsigned int col;

	/* Data pattern that resulted in a corrected error */
	uint32_t syndromes_31_0;
	uint32_t syndromes_63_32;	/* For 32-bit ECC - not used. */
	uint32_t syndromes_ecc;		/* ECC lane */

	/**
	 * Mask for the corrected data portion
	 * 1 on any bit indicates that the bit has been corrected by the ECC
	 * logic
	 * 0 on any bit indicates that the bit has not been corrected by the
	 * ECC logic
	 * This register accumulates data over multiple ECC errors, to give an
	 * overall indication of which bits are being fixed. It is cleared by
	 * calling al_ddr_ecc_corr_int_clear.
	 */
	uint32_t corr_bit_mask_31_0;
	uint32_t corr_bit_mask_63_32;	/* For 32-bit ECC - not used. */
	uint32_t corr_bit_mask_ecc;	/* ECC lane */

	/* Bit number corrected by single-bit ECC error */
	unsigned int ecc_corrected_bit_num;
};

struct al_ddr_ecc_cfg {
	/* ECC mode indicator */
	al_bool ecc_enabled;

	/* Enable ECC scrubs - applicable only when ecc is enabled */
	al_bool scrub_enabled;
};

/* DDR controller power modes */
enum al_ddr_power_mode {
	/* No power mode enabled */
	AL_DDR_POWERMODE_OFF,

	/**
	 * Self refresh:
	 * Puts the SDRAM into self refresh when no active transactions
	 */
	AL_DDR_POWERMODE_SELF_REFRESH,

	/**
	 * Power down:
	 * The DDR controller goes into power-down after a
	 * programmable number of idle cycles (Multiples of 32 clocks)
	 */
	AL_DDR_POWERMODE_POWER_DOWN,

};

/* DDR operating modes */
enum al_ddr_operating_mode {
	/* Initialiazation */
	AL_DDR_OPERATING_MODE_INIT,

	/* Normal operation */
	AL_DDR_OPERATING_MODE_NORMAL,

	/* Power down */
	AL_DDR_OPERATING_MODE_POWER_DOWN,

	/* Self refresh */
	AL_DDR_OPERATING_MODE_SELF_REFRESH,

};

int al_ddr_phy_datx_bist(
	void __iomem			*ddr_ctrl_regs_base,
	void __iomem			*ddr_phy_regs_base,
	struct al_ddr_bist_params	*params);

int al_ddr_phy_ac_bist(
	void __iomem		*ddr_phy_regs_base,
	enum al_ddr_bist_pat	pat);

/**
 * @brief Get current data bus width
 *
 * @param	ddr_ctrl_regs_base
 *		Address of the DDR controller register base
 *
 * @returns	The data bus width
 */
enum al_ddr_data_width al_ddr_data_width_get(
	void __iomem		*ddr_ctrl_regs_base);

/**
 * @brief Get the current number of available ranks
 *
 * @param	ddr_phy_regs_base
 *		Address of the DDR controller register base
 *
 * @returns	The number of available ranks
 */
unsigned int al_ddr_active_ranks_get(
	void __iomem		*ddr_ctrl_regs_base);

/**
 * @brief Get the current corrected/uncorrected error status
 *
 * @param	ddr_ctrl_regs_base
 *		Address of the DDR controller register base
 * @param	corr_status
 *		The corrected error status (use NULL if no status is required)
 * @param	uncorr_status
 *		The uncorrected error status (use NULL if no status is required)
 *
 * @returns	0 if successful
 *		<0 otherwise
 */
int al_ddr_ecc_status_get(
	void __iomem			*ddr_ctrl_regs_base,
	struct al_ddr_ecc_status	*corr_status,
	struct al_ddr_ecc_status	*uncorr_status);

/**
 * @brief Get the current ECC configuration
 *
 * @param	ddr_ctrl_regs_base
 *		Address of the DDR controller register base
 * @param	cfg
 *		The ECC configuration
 */
void al_ddr_ecc_cfg_get(
	void __iomem			*ddr_ctrl_regs_base,
	struct al_ddr_ecc_cfg		*cfg);

int al_ddr_ecc_corr_count_clear(
	void __iomem		*ddr_ctrl_regs_base);

/**
 * @brief Clear the correctable error interrupt
 *
 * @param	nb_regs_base
 *		Address of the NB register base, used i.o. to clear NB interrupt
 *		(use NULL if no clearing is required)
 * @param	ddr_ctrl_regs_base
 *		Address of the DDR controller register base
 *
 * @returns	0 if successful
 *		<0 otherwise
 */
int al_ddr_ecc_corr_int_clear(
	void __iomem		*nb_regs_base,
	void __iomem		*ddr_ctrl_regs_base);

int al_ddr_ecc_uncorr_count_clear(
	void __iomem		*ddr_ctrl_regs_base);

/**
 * @brief Clear the uncorrectable error interrupt
 *
 * @param	nb_regs_base
 *		Address of the NB register base, used i.o. to clear NB interrupt
 *		(use NULL if no clearing is required)
 * @param	ddr_ctrl_regs_base
 *		Address of the DDR controller register base
 *
 * @returns	0 if successful
 *		<0 otherwise
 */
int al_ddr_ecc_uncorr_int_clear(
	void __iomem		*nb_regs_base,
	void __iomem		*ddr_ctrl_regs_base);

int al_ddr_ecc_data_poison_enable(
	void __iomem		*ddr_ctrl_regs_base,
	unsigned int		rank,
	unsigned int		bank,
	unsigned int		col,
	unsigned int		row);

int al_ddr_ecc_data_poison_disable(
	void __iomem		*ddr_ctrl_regs_base);

unsigned int al_ddr_parity_count_get(
	void __iomem		*ddr_ctrl_regs_base);

void al_ddr_parity_count_clear(
	void __iomem		*ddr_ctrl_regs_base);

void al_ddr_parity_int_clear(
	void __iomem		*nb_regs_base,
	void __iomem		*ddr_ctrl_regs_base);

int al_ddr_power_mode_set(
	void __iomem		*ddr_ctrl_regs_base,
	enum al_ddr_power_mode	power_mode,
	unsigned int		timer_x32);

enum al_ddr_operating_mode al_ddr_operating_mode_get(
	void __iomem		*ddr_ctrl_regs_base);

int al_ddr_address_translate_sys2dram(
	void __iomem			*ddr_ctrl_regs_base,
	al_phys_addr_t			sys_address,
	unsigned int			*rank,
	unsigned int			*bank,
	unsigned int			*col,
	unsigned int			*row);

int al_ddr_address_translate_dram2sys(
	void __iomem			*ddr_ctrl_regs_base,
	al_phys_addr_t			*sys_address,
	unsigned int			rank,
	unsigned int			bank,
	unsigned int			col,
	unsigned int			row);

/**
 * @brief Get the amount of connected address bits
 *
 * User can use these bits i.o. to calculate the memory device's rank size
 *
 * @param	ddr_ctrl_regs_base
 *		Address of the DDR controller register base
 *
 * @returns	Num of connected address bits (rank size == 1 << active_bits)
 */
unsigned int al_ddr_bits_per_rank_get(
	void __iomem			*ddr_ctrl_regs_base);

/* *INDENT-OFF* */
#ifdef __cplusplus
}
#endif
/* *INDENT-ON* */
/** @} end of DDR group */
#endif


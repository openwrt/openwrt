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
 * @file   al_hal_nand_coded_properties.h
 *
 * @brief Header file for the NAND coded properties
 *
 */

#ifndef __NAND_CODED_PROPERTIES_H__
#define __NAND_CODED_PROPERTIES_H__

/* *INDENT-OFF* */
#ifdef __cplusplus
extern "C" {
#endif
/* *INDENT-ON* */

#define NAND_CODED_PROPERTIES_NUM_WORDS			4

/*******************************************************************************
 * Word 0
*******************************************************************************/

/*
 * Is valid:
 *	0 - not valid
 *	1 - valid
 */
#define NAND_CODED_PROPERTIES_WORD_0_IS_VALID_SHIFT	31
#define NAND_CODED_PROPERTIES_WORD_0_IS_VALID_MASK	0x80000000

/*
 * Page size:
 *	000 - 512 bytes
 *	001 - reserved
 *	010 - 2048 bytes
 *	011 - 4096 bytes
 *	100 - 8192 bytes
 *	101 - 16384 bytes
 *	110 - reserved
 *	111 - reserved
 */
#define NAND_CODED_PROPERTIES_WORD_0_PAGE_SIZE_SHIFT	28
#define NAND_CODED_PROPERTIES_WORD_0_PAGE_SIZE_MASK	0x70000000

/*
 * Block size:
 *	000 - 16 pages
 *	001 - 32 pages
 *	010 - 64 pages
 *	011 - 128 pages
 *	100 - 256 pages
 *	101 - 512 pages
 *	110 - 1024 pages
 *	111 - 2048 pages
 */
#define NAND_CODED_PROPERTIES_WORD_0_BLOCK_SIZE_SHIFT	25
#define NAND_CODED_PROPERTIES_WORD_0_BLOCK_SIZE_MASK	0x0E000000

/*
 * Bus width:
 *	0 - 8 bits
 *	1 - 16 bits
 */
#define NAND_CODED_PROPERTIES_WORD_0_BUS_WIDTH_SHIFT	24
#define NAND_CODED_PROPERTIES_WORD_0_BUS_WIDTH_MASK	0x01000000

/*
 * Num column cycles:
 *	00 - 1 cycle
 *	01 - 2 cycles
 *	10 - 3 cycles
 *	11 - 4 cycles
 */
#define NAND_CODED_PROPERTIES_WORD_0_COL_CYCLES_SHIFT	22
#define NAND_CODED_PROPERTIES_WORD_0_COL_CYCLES_MASK	0x00C00000

/*
 * Num row cycles:
 *	000 - 1 cycle
 *	001 - 2 cycles
 *	010 - 3 cycles
 *	011 - 4 cycles
 *	100 - 5 cycles
 *	101 - 6 cycles
 *	110 - 7 cycles
 *	111 - 8 cycles
 */
#define NAND_CODED_PROPERTIES_WORD_0_ROW_CYCLES_SHIFT	19
#define NAND_CODED_PROPERTIES_WORD_0_ROW_CYCLES_MASK	0x00380000

/*
 * Bad block marking method:
 *	000 - Disabled
 *	001 - Check only first page of each block
 *	010 - Check first and second page of each block
 *	011 - Check only last page of each block
 *	100 - Check last and last-2 page of each block
 *	101 - reserved
 *	110 - reserved
 *	111 - reserved
 */
#define NAND_CODED_PROPERTIES_WORD_0_BBM_METHOD_SHIFT	16
#define NAND_CODED_PROPERTIES_WORD_0_BBM_METHOD_MASK	0x00070000

/*
 * Bad block marking location 1:
 *	0000 - 1111 - word (depending on bus width) location within spare area
 */
#define NAND_CODED_PROPERTIES_WORD_0_BBM_LOC1_SHIFT	12
#define NAND_CODED_PROPERTIES_WORD_0_BBM_LOC1_MASK	0x0000F000

/*
 *  Bad block marking location 2:
 *	0000 - 1111 - word (depending on bus width) location within spare area
 */
#define NAND_CODED_PROPERTIES_WORD_0_BBM_LOC2_SHIFT	8
#define NAND_CODED_PROPERTIES_WORD_0_BBM_LOC2_MASK	0x00000F00

/* Reserved */
#define NAND_CODED_PROPERTIES_WORD_0_RESERVED_SHIFT	5
#define NAND_CODED_PROPERTIES_WORD_0_RESERVED_MASK	0x000000E0

/*
 * Timing parameter set:
 *	000 - ONFI 0
 *	001 - ONFI 1
 *	010 - ONFI 2
 *	011 - ONFI 3
 *	100 - ONFI 4
 *	101 - ONFI 5
 *	110 - manual
 */
#define NAND_CODED_PROPERTIES_WORD_0_TIMING_SET_SHIFT	2
#define NAND_CODED_PROPERTIES_WORD_0_TIMING_SET_MASK	0x0000001C

/*
 * ECC algorithm:
 *	00 - ECC disabled
 *	01 - Hamming (1 bit per 512 bytes)
 *	10 - BCH
 */
#define NAND_CODED_PROPERTIES_WORD_0_ECC_ALG_SHIFT	0
#define NAND_CODED_PROPERTIES_WORD_0_ECC_ALG_MASK	0x00000003

/*******************************************************************************
 * Word 1
*******************************************************************************/

/*
 * BCH required strength:
 *	0000 - 4 bits
 *	0001 - 8 bits
 *	0010 - 12 bits
 *	0011 - 16 bits
 *	0100 - 20 bits
 *	0101 - 24 bits
 *	0110 - 28 bits
 *	0111 - 32 bits
 *	1000 - 36 bits
 *	1001 - 40 bits
 */
#define NAND_CODED_PROPERTIES_WORD_1_ECC_BCH_STRENGTH_SHIFT	28
#define NAND_CODED_PROPERTIES_WORD_1_ECC_BCH_STRENGTH_MASK	0xF0000000

/*
 * BCH code word size:
 *	0 - 512 bytes
 *	1 - 1024 bytes
 */
#define NAND_CODED_PROPERTIES_WORD_1_ECC_BCH_CODEWORD_SHIFT	27
#define NAND_CODED_PROPERTIES_WORD_1_ECC_BCH_CODEWORD_MASK	0X08000000

/*
 * ECC location in spare area:
 *	9 bits - location within spare area (0 - 511)
 */
#define NAND_CODED_PROPERTIES_WORD_1_ECC_LOC_SHIFT		18
#define NAND_CODED_PROPERTIES_WORD_1_ECC_LOC_MASK		0X07FC0000

/* Timing - tSETUP - 6 bits */
#define NAND_CODED_PROPERTIES_WORD_1_TIMING_TSETUP_SHIFT	12
#define NAND_CODED_PROPERTIES_WORD_1_TIMING_TSETUP_MASK		0x0003F000

/* Timing - tHOLD - 6 bits */
#define NAND_CODED_PROPERTIES_WORD_1_TIMING_THOLD_SHIFT		6
#define NAND_CODED_PROPERTIES_WORD_1_TIMING_THOLD_MASK		0x00000FC0

/* Timing - tWH - 6 bits */
#define NAND_CODED_PROPERTIES_WORD_1_TIMING_TWH_SHIFT		0
#define NAND_CODED_PROPERTIES_WORD_1_TIMING_TWH_MASK		0x0000003F

/*******************************************************************************
 * Word 2
*******************************************************************************/

/* Timing - tWRP - 6 bits */
#define NAND_CODED_PROPERTIES_WORD_2_TIMING_TWRP_SHIFT		26
#define NAND_CODED_PROPERTIES_WORD_2_TIMING_TWRP_MASK		0xFC000000

/* Timing - tINTCMD - 6 bits */
#define NAND_CODED_PROPERTIES_WORD_2_TIMING_TINTCMD_SHIFT	20
#define NAND_CODED_PROPERTIES_WORD_2_TIMING_TINTCMD_MASK	0x03F00000

/* Timing - tReadyRE - 6 bits */
#define NAND_CODED_PROPERTIES_WORD_2_TIMING_TREADYRE_SHIFT	14
#define NAND_CODED_PROPERTIES_WORD_2_TIMING_TREADYRE_MASK	0x000FC000

/* Timing - tWB - 7 bits */
#define NAND_CODED_PROPERTIES_WORD_2_TIMING_TWB_SHIFT		7
#define NAND_CODED_PROPERTIES_WORD_2_TIMING_TWB_MASK		0x00003F80

/* Timing - tReadDly - 2 bits - SDR only */
#define NAND_CODED_PROPERTIES_WORD_2_TIMING_TREADDLY_SHIFT	5
#define NAND_CODED_PROPERTIES_WORD_2_TIMING_TREADDLY_MASK	0x00000060

/*******************************************************************************
 * Word 3
*******************************************************************************/

/* Reserved */
#define NAND_CODED_PROPERTIES_WORD_3_RESERVED_SHIFT		0
#define NAND_CODED_PROPERTIES_WORD_3_RESERVED_MASK		0xFFFFFFFF

/*

Examples of bad block marking:

Micron
------
Before NAND Flash devices are shipped from Micron, they are erased.
The factory identifies invalid blocks before shipping by programming data other
than FFh (x8) or FFFFh (x16) into the first spare location
(column address 2,048 for x8 devices, or 1,024 for x16 devices) of the first or
second page of each bad block.

Toshiba
-------
Read Check: Read column 517 of the 1st page in the block.
If the column is not FFh, define the block as a bad block.

Read Check : Read either column 0 or 2048 of the 1st page or the 2nd page
of each block. If the data of the column is not FF (Hex),
define the block as a bad block.

Numonyx
-------
The devices are supplied with all the locations inside valid blocks
erased (FFh). The bad block information is written prior to shipping.
Any block where the 6th byte (x8 devices)/1st word (x16 devices),
in the spare area of the 1st page, does not contain FFh is a bad block.

Samsung
-------
All device locations are erased(FFh) except locations where the initial
invalid block(s) information is written prior to shipping.
The initial invalid block(s) status is defined by the 1st byte(1st word) in the
spare area. Samsung makes sure that Both of 1st and 2nd page of every initial
invalid block has non-FFh data at the column address of 4096(x16:2048).
Since the initial invalid block information is also erasable in most cases,
it is impossible to recover the information once it has been erased.
Therefore, the system must be able to recognize the initial invalid block(s)
based on the original initial invalid block information and create the initial
invalid block table via the following suggested flow chart(Figure 5).
Any intentional erasure of the original initial invalid block information
is prohibited.

*/

#ifdef __cplusplus
}
#endif
/* *INDENT-ON* */
/** @} end of NAND group */
#endif

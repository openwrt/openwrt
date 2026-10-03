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
 * This file contains the default I/O mappings for Annapurna Labs
 */

#ifndef __AL_HAL_IOMAP_H__
#define __AL_HAL_IOMAP_H__

/* Primary Windows */
#define AL_PCIE_0_BASE		0xe0000000
#define AL_PCIE_0_SIZE		SZ_128M
#define AL_PCIE_1_BASE		0xe8000000
#define AL_PCIE_1_SIZE		SZ_64M
#define AL_PCIE_2_BASE		0xec000000
#define AL_PCIE_2_SIZE		SZ_64M
#define AL_NOR_BASE		0xf4000000
#define AL_SPI_BASE		0xf8000000
#define AL_NAND_BASE		0xfa100000
#define AL_SB_BASE		0xfc000000
#define AL_SB_SIZE		SZ_32M
#define AL_NB_BASE		0xfb000000
#define AL_NB_SIZE		SZ_2M
#define AL_PCIE_0_ECAM_BASE	0xfb600000
#define AL_PCIE_0_ECAM_SIZE	SZ_2M
#define AL_PCIE_1_ECAM_BASE	0xfb800000
#define AL_PCIE_1_ECAM_SIZE	SZ_2M
#define AL_PCIE_2_ECAM_BASE	0xfba00000
#define AL_PCIE_2_ECAM_SIZE	SZ_2M
#define AL_PCIE_INT_ECAM_BASE	0xfbc00000
#define AL_PCIE_INT_ECAM_SIZE	SZ_1M
#define AL_PCIE_INT_BASE	0xfe000000
#define AL_PCIE_INT_SIZE	SZ_16M

#define AL_MSIX_SPACE_BASE_LOW	0xfbe00000
#define AL_MSIX_SPACE_BASE_HIGH	0x0

#define AL_PBS_INT_MEM_BASE	0xfbff0000

#define AL_DRAM_2_HIGH		0x00000001	/* 4GB - 8GB */
#define AL_DRAM_3_HIGH		0x00000002	/* 3GB - 4GB */

/******************/
/* SB Sub Windows */
/******************/
#define AL_SB_PCIE_BASE(idx)	(AL_SB_BASE + 0x01800000 + \
				((idx) * 0x20000))

#define AL_SB_PCIE_NUM		3

#define AL_SB_RING_BASE		(AL_SB_BASE + 0x01860000)

#define AL_SB_PBS_BASE		(AL_SB_BASE + 0x01880000)

#define AL_SB_SERDES_BASE	(AL_SB_BASE + 0x018c0000)

#define AL_SB_DFX_BASE		(AL_SB_BASE + 0x018e0000)

/******************/
/* NB Sub Windows */
/******************/

/* NB main / secondary GICs and their Sub Windows*/
#define AL_NB_GIC_MAIN		0
#define AL_NB_GIC_SECONDARY	1

#define AL_NB_GIC_BASE(id)		(AL_NB_BASE + (id)*0x8000)

#define AL_NB_GIC_DIST_BASE(id)		(AL_NB_GIC_BASE(id) + 0x00001000)
#define AL_NB_GIC_CPU_BASE(id)		(AL_NB_GIC_BASE(id) + 0x00002000)

#define AL_NB_IOMMU_BASE(idx)		(AL_NB_BASE + 0x30000 + (idx)*0x10000)

#define AL_NB_IOMMU_NUM		2

/* NB service registers */
#define AL_NB_SERVICE_BASE	(AL_NB_BASE + 0x00070000)

/* DDR Controller */
#define AL_NB_DDR_CTL_BASE	(AL_NB_BASE + 0x00080000)

/* DDR PHY */
#define AL_NB_DDR_PHY_BASE	(AL_NB_BASE + 0x00088000)

/* CCI Controller */
#define AL_NB_CCI_BASE		(AL_NB_BASE + 0x00090000)

/* SB PBS Sub Windows */
#define AL_I2C_PLD_BASE		(AL_SB_PBS_BASE + 0x00000000)
#define AL_SPI_SLAVE_BASE	(AL_SB_PBS_BASE + 0x00001000)
#define AL_SPI_MASTER_BASE	(AL_SB_PBS_BASE + 0x00002000)

#define AL_UART_BASE(idx)	(AL_SB_PBS_BASE + 0x00003000 \
				+ ((idx) * 0x1000))
#define AL_UART_NUM		4

#define AL_GPIO_BASE(idx)	(AL_SB_PBS_BASE + \
		((idx != 5) ? 0x00007000 + ((idx) * 0x1000) : 0x17000))
#define AL_GPIO_NUM		6

#define AL_WD_BASE(idx)		(AL_SB_PBS_BASE + 0x0000c000 \
				+ ((idx) * 0x1000))
#define AL_WD_NUM		4

#define AL_TIMER_BASE(idx, sub_idx) \
				(AL_SB_PBS_BASE + 0x00010000 \
				 + ((idx) * 0x1000) + ((sub_idx) * 0x20))

#define AL_TIMER_NUM		4
#define AL_TIMER_SUB_TIMERS_NUM	2

#define AL_I2C_GEN_BASE		(AL_SB_PBS_BASE + 0x00014000)
#define AL_PBS_UFC_WRAP_BASE	(AL_SB_PBS_BASE + 0x00015000)
#define AL_PBS_UFC_CNTL_BASE	(AL_SB_PBS_BASE + 0x00015800)
#define AL_PBS_OTP_BASE		(AL_SB_PBS_BASE + 0x00016000)
#define AL_PBS_BOOT_ROM_BASE	(AL_SB_PBS_BASE + 0x00020000)
#define AL_PBS_SRAM_BASE	(AL_SB_PBS_BASE + 0x00024000)
#define AL_PBS_REGFILE_BASE	(AL_SB_PBS_BASE + 0x00028000)

/* SB Ring Sub Windows */
#define AL_CMOS_NUM_GROUPS	10

#define AL_CMOS_GROUP_BASE(idx)	(AL_SB_RING_BASE + (idx) * 0x100)

#define AL_TEMP_SENSOR_BASE	(AL_SB_RING_BASE + 0xa00)

#define AL_PLL_SB		0
#define AL_PLL_NB		1
#define AL_PLL_CPU		2

#define AL_PLL_BASE(id)		(AL_SB_RING_BASE + 0xb00 + (id) * 0x100)

/* SB DFX Sub Windows */
#define AL_DFX_CTRL_BASE	(AL_SB_DFX_BASE + 0x0)
#define AL_DAP2JTAG_BASE	(AL_SB_DFX_BASE + 0x8000)

/***************************/
/* PBS int mem sub windows */
/***************************/
#define AL_PBS_INT_MEM_BOOT_ROM_BASE	(AL_PBS_INT_MEM_BASE + 0x0)
#define AL_PBS_INT_MEM_SRAM_BASE	(AL_PBS_INT_MEM_BASE + 0x4000)

#endif

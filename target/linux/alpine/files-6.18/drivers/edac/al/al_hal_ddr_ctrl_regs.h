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
 * @addtogroup groupddr
 *
 *  @{
 * @file   al_hal_ddr_ctrl_regs.h
 *
 * @brief  DDR controller registers 
 *
 */

#ifndef __AL_HAL_DDR_CTRL_REGS_H__
#define __AL_HAL_DDR_CTRL_REGS_H__

#ifdef __cplusplus
extern "C" {
#endif
/*
* Unit Registers
*/
struct al_dwc_ddr_umctl2_regs {
	uint32_t mstr;                  /* Master Register */
	uint32_t stat;                  /* Operating Mode Status Regi ... */
	uint32_t rsrvd_0[2];
	uint32_t mrctrl0;               /* Mode Register Read/Write C ... */
	uint32_t mrctrl1;               /* Mode Register Read/Write C ... */
	uint32_t mrstat;                /* Mode Register Read/Write S ... */
	uint32_t rsrvd_1[5];
	uint32_t pwrctl;                /* Low Power Control Register ... */
	uint32_t pwrtmg;                /* Low Power Timing Register */
	uint32_t rsrvd_2[6];
	uint32_t rfshctl0;              /* Refresh Control Register 0 ... */
	uint32_t rfshctl1;              /* Refresh Control Register 1 ... */
	uint32_t rfshctl2;              /* Refresh Control Register 2 ... */
	uint32_t rsrvd_3;
	uint32_t rfshctl3;              /* Refresh Control Register 0 ... */
	uint32_t rfshtmg;               /* Refresh Timing Register */
	uint32_t rsrvd_4[2];
	uint32_t ecccfg0;               /* ECC Configuration Register ... */
	uint32_t ecccfg1;               /* ECC Configuration Register ... */
	uint32_t eccstat;               /* ECC Status Register */
	uint32_t eccclr;                /* ECC Clear Register */
	uint32_t eccerrcnt;             /* ECC Error Counter Register ... */
	uint32_t ecccaddr0;             /* ECC Corrected Error Addres ... */
	uint32_t ecccaddr1;             /* ECC Corrected Error Addres ... */
	uint32_t ecccsyn0;              /* ECC Corrected Syndrome Reg ... */
	uint32_t ecccsyn1;              /* ECC Corrected Syndrome Reg ... */
	uint32_t ecccsyn2;              /* ECC Corrected Syndrome Reg ... */
	uint32_t eccbitmask0;           /* ECC Corrected Data Bit Mas ... */
	uint32_t eccbitmask1;           /* ECC Corrected Data Bit Mas ... */
	uint32_t eccbitmask2;           /* ECC Corrected Data Bit Mas ... */
	uint32_t eccuaddr0;             /* ECC Uncorrected Error Addr ... */
	uint32_t eccuaddr1;             /* ECC Unorrected Error Addre ... */
	uint32_t eccusyn0;              /* ECC Unorrected Syndrome Re ... */
	uint32_t eccusyn1;              /* ECC Uncorrected Syndrome R ... */
	uint32_t eccusyn2;              /* ECC Uncorrected Syndrome R ... */
	uint32_t eccpoisonaddr0;        /* ECC Data Poisoning Address ... */
	uint32_t eccpoisonaddr1;        /* ECC Data Poisoning Address ... */
	uint32_t parctl;                /* Parity Control Register */
	uint32_t parstat;               /* Parity Status Register */
	uint32_t rsrvd_5[2];
	uint32_t init0;                 /* SDRAM Initialization Regis ... */
	uint32_t init1;                 /* SDRAM Initialization Regis ... */
	uint32_t rsrvd_6;
	uint32_t init3;                 /* SDRAM Initialization Regis ... */
	uint32_t init4;                 /* SDRAM Initialization Regis ... */
	uint32_t init5;                 /* SDRAM Initialization Regis ... */
	uint32_t rsrvd_7[2];
	uint32_t dimmctl;               /* DIMM Control Register */
	uint32_t rankctl;               /* Rank Control Register */
	uint32_t rsrvd_8[2];
	uint32_t dramtmg0;              /* SDRAM Timing Register 0 */
	uint32_t dramtmg1;              /* SDRAM Timing Register 1 */
	uint32_t dramtmg2;              /* SDRAM Timing Register 2 */
	uint32_t dramtmg3;              /* SDRAM Timing Register 3 */
	uint32_t dramtmg4;              /* SDRAM Timing Register 4 */
	uint32_t dramtmg5;              /* SDRAM Timing Register 5 */
	uint32_t rsrvd_9[2];
	uint32_t dramtmg8;              /* SDRAM Timing Register 8 */
	uint32_t rsrvd_10[23];
	uint32_t zqctl0;                /* ZQ Control Register 0 */
	uint32_t zqctl1;                /* ZQ Control Register 1 */
	uint32_t rsrvd_11[2];
	uint32_t dfitmg0;               /* DFI Timing Register 0 */
	uint32_t dfitmg1;               /* DFI Timing Register 1 */
	uint32_t rsrvd_12[2];
	uint32_t dfiupd0;               /* DFI Update Register 0 */
	uint32_t dfiupd1;               /* DFI Update Register 1 */
	uint32_t dfiupd2;               /* DFI Update Register 2 */
	uint32_t dfiupd3;               /* DFI Update Register 3 */
	uint32_t dfimisc;               /* DFI Miscellaneous Control  ... */
	uint32_t rsrvd_13[19];
	uint32_t addrmap0;              /* Address Map Register 0 */
	uint32_t addrmap1;              /* Address Map Register 1 */
	uint32_t addrmap2;              /* Address Map Register 2 */
	uint32_t addrmap3;              /* Address Map Register 3 */
	uint32_t addrmap4;              /* Address Map Register 4 */
	uint32_t addrmap5;              /* Address Map Register 5 */
	uint32_t addrmap6;              /* Address Map Register 6 */
	uint32_t rsrvd_14[9];
	uint32_t odtcfg;                /* ODT Configuration Register ... */
	uint32_t odtmap;                /* ODT/Rank Map Register */
	uint32_t rsrvd_15[2];
	uint32_t sched;                 /* Scheduler Control Register ... */
	uint32_t rsrvd_16;
	uint32_t perfhpr0;              /* High Priority Read CAM Reg ... */
	uint32_t perfhpr1;              /* High Priority Read CAM Reg ... */
	uint32_t perflpr0;              /* Low Priority Read CAM Regi ... */
	uint32_t perflpr1;              /* Low Priority Read CAM Regi ... */
	uint32_t perfwr0;               /* Write CAM Register 0 */
	uint32_t perfwr1;               /* Write CAM Register 1 */
	uint32_t rsrvd_17[36];
	uint32_t dbg0;                  /* Debug Register 0 */
	uint32_t dbg1;                  /* Debug Register 1 */
	uint32_t dbgcam;                /* CAM Debug Register */
	uint32_t rsrvd[61];
};
struct al_dwc_ddr_umctl2_mp {
	uint32_t pccfg;                 /* Port Common Configuration  ... */
	uint32_t pcfgr_0;               /* Port 0 Configuration Read  ... */
	uint32_t pcfgw_0;               /* Port 0 Configuration Write ... */
	uint32_t pcfgidmaskch0_0;       /* Port 0 Channel 0 Configura ... */
	uint32_t pcfgidvaluech0_0;      /* Port 0 Channel 0 Configura ... */
	uint32_t rsrvd[1787];
};

struct al_ddr_ctrl_regs {
	struct al_dwc_ddr_umctl2_regs umctl2_regs;
	struct al_dwc_ddr_umctl2_mp umctl2_mp;
};


/*
* Registers Fields
*/


/**** MSTR register ****/
/*  Select DDR3 SDRAM - 1 - DDR3 operating mode - 0 - DDR2 opera ... */
#define DWC_DDR_UMCTL2_REGS_MSTR_DDR3    (1 << 0)

#define DWC_DDR_UMCTL2_REGS_MSTR_RESERVED_MASK 0x000000FE
#define DWC_DDR_UMCTL2_REGS_MSTR_RESERVED_SHIFT 1
/* Indicates burst mode */
#define DWC_DDR_UMCTL2_REGS_MSTR_BURST_MODE (1 << 8)
/* When set, enable burst-chop in DDR3. */
#define DWC_DDR_UMCTL2_REGS_MSTR_BURSTCHOP (1 << 9)
/* If 1, then uMCTL2 uses 2T timing */
#define DWC_DDR_UMCTL2_REGS_MSTR_EN_2T_TIMING_MODE (1 << 10)

#define DWC_DDR_UMCTL2_REGS_MSTR_RESERVED1 (1 << 11)
/*  Selects proportion of DQ bus width that is used by the SDRAM ... */
#define DWC_DDR_UMCTL2_REGS_MSTR_DATA_BUS_WIDTH_MASK 0x00003000
#define DWC_DDR_UMCTL2_REGS_MSTR_DATA_BUS_WIDTH_SHIFT 12

#define DWC_DDR_UMCTL2_REGS_MSTR_DATA_BUS_WIDTH_64	\
	(0 << DWC_DDR_UMCTL2_REGS_MSTR_DATA_BUS_WIDTH_SHIFT)
#define DWC_DDR_UMCTL2_REGS_MSTR_DATA_BUS_WIDTH_32	\
	(1 << DWC_DDR_UMCTL2_REGS_MSTR_DATA_BUS_WIDTH_SHIFT)

#define DWC_DDR_UMCTL2_REGS_MSTR_RESERVED2_MASK 0x0000C000
#define DWC_DDR_UMCTL2_REGS_MSTR_RESERVED2_SHIFT 14
/*  SDRAM burst length used: - 0001 - Burst length of 2 (only su ... */
#define DWC_DDR_UMCTL2_REGS_MSTR_BURST_RDWR_MASK 0x000F0000
#define DWC_DDR_UMCTL2_REGS_MSTR_BURST_RDWR_SHIFT 16

#define DWC_DDR_UMCTL2_REGS_MSTR_RESERVED3_MASK 0x00F00000
#define DWC_DDR_UMCTL2_REGS_MSTR_RESERVED3_SHIFT 20
/* Only present for multi-rank configurations */
#define DWC_DDR_UMCTL2_REGS_MSTR_ACTIVE_RANKS_MASK 0x0F000000
#define DWC_DDR_UMCTL2_REGS_MSTR_ACTIVE_RANKS_SHIFT 24

#define DWC_DDR_UMCTL2_REGS_MSTR_RESERVED4_MASK 0xF0000000
#define DWC_DDR_UMCTL2_REGS_MSTR_RESERVED4_SHIFT 28

/**** STAT register ****/
/* Operating mode */
#define DWC_DDR_UMCTL2_REGS_STAT_OPERATING_MODE_MASK 0x00000003
#define DWC_DDR_UMCTL2_REGS_STAT_OPERATING_MODE_SHIFT 0

#define DWC_DDR_UMCTL2_REGS_STAT_OPERATING_MODE_INIT		\
	(0 << DWC_DDR_UMCTL2_REGS_STAT_OPERATING_MODE_SHIFT)
#define DWC_DDR_UMCTL2_REGS_STAT_OPERATING_MODE_NORMAL		\
	(1 << DWC_DDR_UMCTL2_REGS_STAT_OPERATING_MODE_SHIFT)
#define DWC_DDR_UMCTL2_REGS_STAT_OPERATING_MODE_POWER_DOWN	\
	(2 << DWC_DDR_UMCTL2_REGS_STAT_OPERATING_MODE_SHIFT)
#define DWC_DDR_UMCTL2_REGS_STAT_OPERATING_MODE_SELF_REFRESH	\
	(3 << DWC_DDR_UMCTL2_REGS_STAT_OPERATING_MODE_SHIFT)

#define DWC_DDR_UMCTL2_REGS_STAT_RESERVED_MASK 0xFFFFFFFC
#define DWC_DDR_UMCTL2_REGS_STAT_RESERVED_SHIFT 2

/**** MRCTRL0 register ****/

#define DWC_DDR_UMCTL2_REGS_MRCTRL0_RESERVED_MASK 0x0000000F
#define DWC_DDR_UMCTL2_REGS_MRCTRL0_RESERVED_SHIFT 0
/* Controls which rank is accessed by MRCTRL0 */
#define DWC_DDR_UMCTL2_REGS_MRCTRL0_MR_RANK_MASK 0x000000F0
#define DWC_DDR_UMCTL2_REGS_MRCTRL0_MR_RANK_SHIFT 4

#define DWC_DDR_UMCTL2_REGS_MRCTRL0_RESERVED1_MASK 0x00000F00
#define DWC_DDR_UMCTL2_REGS_MRCTRL0_RESERVED1_SHIFT 8
/* Address of the mode register that is to be written to */
#define DWC_DDR_UMCTL2_REGS_MRCTRL0_MR_ADDR_MASK 0x00007000
#define DWC_DDR_UMCTL2_REGS_MRCTRL0_MR_ADDR_SHIFT 12

#define DWC_DDR_UMCTL2_REGS_MRCTRL0_RESERVED2_MASK 0x7FFF8000
#define DWC_DDR_UMCTL2_REGS_MRCTRL0_RESERVED2_SHIFT 15
/* Setting this register bit to 1 triggers a mode register read  ... */
#define DWC_DDR_UMCTL2_REGS_MRCTRL0_MR_WR (1 << 31)

/**** MRCTRL1 register ****/
/* Mode register write data for all non-LPDDR2 modes */
#define DWC_DDR_UMCTL2_REGS_MRCTRL1_MR_DATA_MASK 0x0000FFFF
#define DWC_DDR_UMCTL2_REGS_MRCTRL1_MR_DATA_SHIFT 0

#define DWC_DDR_UMCTL2_REGS_MRCTRL1_RESERVED_MASK 0xFFFF0000
#define DWC_DDR_UMCTL2_REGS_MRCTRL1_RESERVED_SHIFT 16

/**** MRSTAT register ****/
/* SoC core may initiate a MR write operation only if this signa ... */
#define DWC_DDR_UMCTL2_REGS_MRSTAT_MR_WR_BUSY (1 << 0)

#define DWC_DDR_UMCTL2_REGS_MRSTAT_RESERVED_MASK 0xFFFFFFFE
#define DWC_DDR_UMCTL2_REGS_MRSTAT_RESERVED_SHIFT 1

/**** PWRCTL register ****/
/* If set, then the uMCTL2 puts the SDRAM into self refresh when ... */
#define DWC_DDR_UMCTL2_REGS_PWRCTL_SELFREF_EN (1 << 0)
/* If true then the uMCTL2 goes into power-down after a programm ... */
#define DWC_DDR_UMCTL2_REGS_PWRCTL_POWERDOWN_EN (1 << 1)

#define DWC_DDR_UMCTL2_REGS_PWRCTL_RESERVED (1 << 2)
/* Enable the assertion of dfi_dram_clk_disable whenever a clock ... */
#define DWC_DDR_UMCTL2_REGS_PWRCTL_EN_DFI_DRAM_CLK_DISABLE (1 << 3)

#define DWC_DDR_UMCTL2_REGS_PWRCTL_RESERVED1_MASK 0xFFFFFFF0
#define DWC_DDR_UMCTL2_REGS_PWRCTL_RESERVED1_SHIFT 4

/**** PWRTMG register ****/
/* After this many clocks of NOP or deselect the uMCTL2 puts the ... */
#define DWC_DDR_UMCTL2_REGS_PWRTMG_POWERDOWN_TO_X32_MASK 0x0000001F
#define DWC_DDR_UMCTL2_REGS_PWRTMG_POWERDOWN_TO_X32_SHIFT 0

#define DWC_DDR_UMCTL2_REGS_PWRTMG_RESERVED_MASK 0xFFFFFFE0
#define DWC_DDR_UMCTL2_REGS_PWRTMG_RESERVED_SHIFT 5

/**** RFSHCTL0 register ****/

#define DWC_DDR_UMCTL2_REGS_RFSHCTL0_RESERVED_MASK 0x000000FF
#define DWC_DDR_UMCTL2_REGS_RFSHCTL0_RESERVED_SHIFT 0
/* The programmed value + 1 is the number of refresh timeouts th ... */
#define DWC_DDR_UMCTL2_REGS_RFSHCTL0_REFRESH_BURST_MASK 0x00000700
#define DWC_DDR_UMCTL2_REGS_RFSHCTL0_REFRESH_BURST_SHIFT 8

#define DWC_DDR_UMCTL2_REGS_RFSHCTL0_REFRESH_BURST(n)	\
	(((n) - 1) << DWC_DDR_UMCTL2_REGS_RFSHCTL0_REFRESH_BURST_SHIFT)

#define DWC_DDR_UMCTL2_REGS_RFSHCTL0_RESERVED1 (1 << 11)
/* If the refresh timer (t<sub>RFC</sub>nom, also known as t<sub ... */
#define DWC_DDR_UMCTL2_REGS_RFSHCTL0_REFRESH_TO_X32_MASK 0x0001F000
#define DWC_DDR_UMCTL2_REGS_RFSHCTL0_REFRESH_TO_X32_SHIFT 12

#define DWC_DDR_UMCTL2_REGS_RFSHCTL0_RESERVED2_MASK 0x000E0000
#define DWC_DDR_UMCTL2_REGS_RFSHCTL0_RESERVED2_SHIFT 17
/* Threshold value in number of clock cycles before the critical ... */
#define DWC_DDR_UMCTL2_REGS_RFSHCTL0_REFRESH_MARGIN_MASK 0x00F00000
#define DWC_DDR_UMCTL2_REGS_RFSHCTL0_REFRESH_MARGIN_SHIFT 20

#define DWC_DDR_UMCTL2_REGS_RFSHCTL0_RESERVED3_MASK 0xFF000000
#define DWC_DDR_UMCTL2_REGS_RFSHCTL0_RESERVED3_SHIFT 24

/**** RFSHCTL1 register ****/
/* Refresh timer start for rank 0 (only present in multi-rank co ... */
#define DWC_DDR_UMCTL2_REGS_RFSHCTL1_REFRESH_TIMER0_START_VALUE_X32_MASK 0x00000FFF
#define DWC_DDR_UMCTL2_REGS_RFSHCTL1_REFRESH_TIMER0_START_VALUE_X32_SHIFT 0

#define DWC_DDR_UMCTL2_REGS_RFSHCTL1_RESERVED_MASK 0x0000F000
#define DWC_DDR_UMCTL2_REGS_RFSHCTL1_RESERVED_SHIFT 12
/* Refresh timer start for rank 1 (only present in multi-rank co ... */
#define DWC_DDR_UMCTL2_REGS_RFSHCTL1_REFRESH_TIMER1_START_VALUE_X32_MASK 0x0FFF0000
#define DWC_DDR_UMCTL2_REGS_RFSHCTL1_REFRESH_TIMER1_START_VALUE_X32_SHIFT 16

#define DWC_DDR_UMCTL2_REGS_RFSHCTL1_RESERVED1_MASK 0xF0000000
#define DWC_DDR_UMCTL2_REGS_RFSHCTL1_RESERVED1_SHIFT 28

/**** RFSHCTL2 register ****/
/* Refresh timer start for rank 2 (only present in 4-rank config ... */
#define DWC_DDR_UMCTL2_REGS_RFSHCTL2_REFRESH_TIMER2_START_VALUE_X32_MASK 0x00000FFF
#define DWC_DDR_UMCTL2_REGS_RFSHCTL2_REFRESH_TIMER2_START_VALUE_X32_SHIFT 0

#define DWC_DDR_UMCTL2_REGS_RFSHCTL2_RESERVED_MASK 0x0000F000
#define DWC_DDR_UMCTL2_REGS_RFSHCTL2_RESERVED_SHIFT 12
/* Refresh timer start for rank 3 (only present in 4-rank config ... */
#define DWC_DDR_UMCTL2_REGS_RFSHCTL2_REFRESH_TIMER3_START_VALUE_X32_MASK 0x0FFF0000
#define DWC_DDR_UMCTL2_REGS_RFSHCTL2_REFRESH_TIMER3_START_VALUE_X32_SHIFT 16

#define DWC_DDR_UMCTL2_REGS_RFSHCTL2_RESERVED1_MASK 0xF0000000
#define DWC_DDR_UMCTL2_REGS_RFSHCTL2_RESERVED1_SHIFT 28

/**** RFSHCTL3 register ****/
/* When '1', disable auto-refresh generated by the uMCTL2 */
#define DWC_DDR_UMCTL2_REGS_RFSHCTL3_DIS_AUTO_REFRESH (1 << 0)
/* Toggle this signal to indicate that the refresh register(s) h ... */
#define DWC_DDR_UMCTL2_REGS_RFSHCTL3_REFRESH_UPDATE_LEVEL (1 << 1)

#define DWC_DDR_UMCTL2_REGS_RFSHCTL3_RESERVED_MASK 0xFFFFFFFC
#define DWC_DDR_UMCTL2_REGS_RFSHCTL3_RESERVED_SHIFT 2

/**** RFSHTMG register ****/
/* t<sub>RFC</sub> (min):  Minimum time from refresh to refresh  ... */
#define DWC_DDR_UMCTL2_REGS_RFSHTMG_T_RFC_MIN_MASK 0x000001FF
#define DWC_DDR_UMCTL2_REGS_RFSHTMG_T_RFC_MIN_SHIFT 0

#define DWC_DDR_UMCTL2_REGS_RFSHTMG_RESERVED_MASK 0x0000FE00
#define DWC_DDR_UMCTL2_REGS_RFSHTMG_RESERVED_SHIFT 9
/* t<sub>REFI</sub>: Average time interval between refreshes per ... */
#define DWC_DDR_UMCTL2_REGS_RFSHTMG_T_RFC_NOM_X32_MASK 0x0FFF0000
#define DWC_DDR_UMCTL2_REGS_RFSHTMG_T_RFC_NOM_X32_SHIFT 16

#define DWC_DDR_UMCTL2_REGS_RFSHTMG_RESERVED1_MASK 0xF0000000
#define DWC_DDR_UMCTL2_REGS_RFSHTMG_RESERVED1_SHIFT 28

/**** ECCCFG0 register ****/
/* ECC mode indicator */
#define DWC_DDR_UMCTL2_REGS_ECCCFG0_ECC_MODE_MASK 0x00000007
#define DWC_DDR_UMCTL2_REGS_ECCCFG0_ECC_MODE_SHIFT 0

#define DWC_DDR_UMCTL2_REGS_ECCCFG0_ECC_MODE_DIS	\
	(0 << DWC_DDR_UMCTL2_REGS_ECCCFG0_ECC_MODE_SHIFT)
#define DWC_DDR_UMCTL2_REGS_ECCCFG0_ECC_MODE_EN		\
	(4 << DWC_DDR_UMCTL2_REGS_ECCCFG0_ECC_MODE_SHIFT)

#define DWC_DDR_UMCTL2_REGS_ECCCFG0_RESERVED (1 << 3)
/* Disable ECC scrubs */
#define DWC_DDR_UMCTL2_REGS_ECCCFG0_DIS_SCRUB (1 << 4)

#define DWC_DDR_UMCTL2_REGS_ECCCFG0_RESERVED1_MASK 0xFFFFFFE0
#define DWC_DDR_UMCTL2_REGS_ECCCFG0_RESERVED1_SHIFT 5

/**** ECCCFG1 register ****/
/* Enable ECC data poisoning - introduces ECC errors on writes t ... */
#define DWC_DDR_UMCTL2_REGS_ECCCFG1_DATA_POISON (1 << 0)

#define DWC_DDR_UMCTL2_REGS_ECCCFG1_RESERVED_MASK 0xFFFFFFFE
#define DWC_DDR_UMCTL2_REGS_ECCCFG1_RESERVED_SHIFT 1

/**** ECCSTAT register ****/
/* Bit number corrected by single-bit ECC error */
#define DWC_DDR_UMCTL2_REGS_ECCSTAT_ECC_CORRECTED_BIT_NUM_MASK 0x0000007F
#define DWC_DDR_UMCTL2_REGS_ECCSTAT_ECC_CORRECTED_BIT_NUM_SHIFT 0

#define DWC_DDR_UMCTL2_REGS_ECCSTAT_RESERVED (1 << 7)
/* Single-bit error indicators, 1 per ECC lane */
#define DWC_DDR_UMCTL2_REGS_ECCSTAT_ECC_CORRECTED_ERR_MASK 0x00000300
#define DWC_DDR_UMCTL2_REGS_ECCSTAT_ECC_CORRECTED_ERR_SHIFT 8

#define DWC_DDR_UMCTL2_REGS_ECCSTAT_RESERVED1_MASK 0x0000FC00
#define DWC_DDR_UMCTL2_REGS_ECCSTAT_RESERVED1_SHIFT 10
/* Double-bit error indicators, 1 per ECC lane */
#define DWC_DDR_UMCTL2_REGS_ECCSTAT_ECC_UNCORRECTED_ERR_MASK 0x00030000
#define DWC_DDR_UMCTL2_REGS_ECCSTAT_ECC_UNCORRECTED_ERR_SHIFT 16

#define DWC_DDR_UMCTL2_REGS_ECCSTAT_RESERVED2_MASK 0xFFFC0000
#define DWC_DDR_UMCTL2_REGS_ECCSTAT_RESERVED2_SHIFT 18

/**** ECCCLR register ****/
/* Setting this regsiter bit to 1 clears the currently stored co ... */
#define DWC_DDR_UMCTL2_REGS_ECCCLR_ECC_CLR_CORR_ERR (1 << 0)
/* Setting this regsiter bit to 1 clears the currently stored un ... */
#define DWC_DDR_UMCTL2_REGS_ECCCLR_ECC_CLR_UNCORR_ERR (1 << 1)
/* Setting this regsiter bit to 1 clears the currently stored co ... */
#define DWC_DDR_UMCTL2_REGS_ECCCLR_ECC_CLR_CORR_ERR_CNT (1 << 2)
/* Setting this regsiter bit to 1 clears the currently stored un ... */
#define DWC_DDR_UMCTL2_REGS_ECCCLR_ECC_CLR_UNCORR_ERR_CNT (1 << 3)

#define DWC_DDR_UMCTL2_REGS_ECCCLR_RESERVED_MASK 0xFFFFFFF0
#define DWC_DDR_UMCTL2_REGS_ECCCLR_RESERVED_SHIFT 4

/**** ECCERRCNT register ****/
/* Number of correctable ECC errors detected */
#define DWC_DDR_UMCTL2_REGS_ECCERRCNT_ECC_CORR_ERR_CNT_MASK 0x0000FFFF
#define DWC_DDR_UMCTL2_REGS_ECCERRCNT_ECC_CORR_ERR_CNT_SHIFT 0
/* Number of uncorrectable ECC errors detected */
#define DWC_DDR_UMCTL2_REGS_ECCERRCNT_ECC_UNCORR_ERR_CNT_MASK 0xFFFF0000
#define DWC_DDR_UMCTL2_REGS_ECCERRCNT_ECC_UNCORR_ERR_CNT_SHIFT 16

/**** ECCCADDR0 register ****/
/* Page/row number of a read resulting in a corrected ECC error */
#define DWC_DDR_UMCTL2_REGS_ECCCADDR0_ECC_CORR_ROW_MASK 0x0000FFFF
#define DWC_DDR_UMCTL2_REGS_ECCCADDR0_ECC_CORR_ROW_SHIFT 0

#define DWC_DDR_UMCTL2_REGS_ECCCADDR0_RESERVED_MASK 0x00FF0000
#define DWC_DDR_UMCTL2_REGS_ECCCADDR0_RESERVED_SHIFT 16
/* Rank number of a read resulting in a corrected ECC error */
#define DWC_DDR_UMCTL2_REGS_ECCCADDR0_ECC_CORR_RANK_MASK 0x03000000
#define DWC_DDR_UMCTL2_REGS_ECCCADDR0_ECC_CORR_RANK_SHIFT 24

#define DWC_DDR_UMCTL2_REGS_ECCCADDR0_RESERVED1_MASK 0xFC000000
#define DWC_DDR_UMCTL2_REGS_ECCCADDR0_RESERVED1_SHIFT 26

/**** ECCCADDR1 register ****/
/* Block number of a read resulting in a corrected ECC error (lo ... */
#define DWC_DDR_UMCTL2_REGS_ECCCADDR1_ECC_CORR_COL_MASK 0x00000FFF
#define DWC_DDR_UMCTL2_REGS_ECCCADDR1_ECC_CORR_COL_SHIFT 0

#define DWC_DDR_UMCTL2_REGS_ECCCADDR1_RESERVED_MASK 0x0000F000
#define DWC_DDR_UMCTL2_REGS_ECCCADDR1_RESERVED_SHIFT 12
/* Bank number of a read resulting in a corrected ECC error */
#define DWC_DDR_UMCTL2_REGS_ECCCADDR1_ECC_CORR_BANK_MASK 0x00070000
#define DWC_DDR_UMCTL2_REGS_ECCCADDR1_ECC_CORR_BANK_SHIFT 16

#define DWC_DDR_UMCTL2_REGS_ECCCADDR1_RESERVED1_MASK 0xFFF80000
#define DWC_DDR_UMCTL2_REGS_ECCCADDR1_RESERVED1_SHIFT 19

/**** ECCCSYN2 register ****/
/* Data pattern that resulted in a corrected error one for each  ... */
#define DWC_DDR_UMCTL2_REGS_ECCCSYN2_ECC_CORR_SYNDROMES_71_64_MASK 0x000000FF
#define DWC_DDR_UMCTL2_REGS_ECCCSYN2_ECC_CORR_SYNDROMES_71_64_SHIFT 0

#define DWC_DDR_UMCTL2_REGS_ECCCSYN2_RESERVED_MASK 0xFFFFFF00
#define DWC_DDR_UMCTL2_REGS_ECCCSYN2_RESERVED_SHIFT 8

/**** ECCBITMASK2 register ****/
/* Mask for the corrected data portion    - 1 on any bit indicat ... */
#define DWC_DDR_UMCTL2_REGS_ECCBITMASK2_ECC_CORR_BIT_MASK_71_64_MASK 0x000000FF
#define DWC_DDR_UMCTL2_REGS_ECCBITMASK2_ECC_CORR_BIT_MASK_71_64_SHIFT 0

#define DWC_DDR_UMCTL2_REGS_ECCBITMASK2_RESERVED_MASK 0xFFFFFF00
#define DWC_DDR_UMCTL2_REGS_ECCBITMASK2_RESERVED_SHIFT 8

/**** ECCUADDR0 register ****/
/* Page/row number of a read resulting in an uncorrected ECC err ... */
#define DWC_DDR_UMCTL2_REGS_ECCUADDR0_ECC_UNCORR_ROW_MASK 0x0000FFFF
#define DWC_DDR_UMCTL2_REGS_ECCUADDR0_ECC_UNCORR_ROW_SHIFT 0

#define DWC_DDR_UMCTL2_REGS_ECCUADDR0_RESERVED_MASK 0x00FF0000
#define DWC_DDR_UMCTL2_REGS_ECCUADDR0_RESERVED_SHIFT 16
/* Rank number of a read resulting in an uncorrected ECC error */
#define DWC_DDR_UMCTL2_REGS_ECCUADDR0_ECC_UNCORR_RANK_MASK 0x03000000
#define DWC_DDR_UMCTL2_REGS_ECCUADDR0_ECC_UNCORR_RANK_SHIFT 24

#define DWC_DDR_UMCTL2_REGS_ECCUADDR0_RESERVED1_MASK 0xFC000000
#define DWC_DDR_UMCTL2_REGS_ECCUADDR0_RESERVED1_SHIFT 26

/**** ECCUADDR1 register ****/
/* Block number of a read resulting in an uncorrected ECC error  ... */
#define DWC_DDR_UMCTL2_REGS_ECCUADDR1_ECC_UNCORR_COL_MASK 0x00000FFF
#define DWC_DDR_UMCTL2_REGS_ECCUADDR1_ECC_UNCORR_COL_SHIFT 0

#define DWC_DDR_UMCTL2_REGS_ECCUADDR1_RESERVED_MASK 0x0000F000
#define DWC_DDR_UMCTL2_REGS_ECCUADDR1_RESERVED_SHIFT 12
/* Bank number of a read resulting in an uncorrected ECC error */
#define DWC_DDR_UMCTL2_REGS_ECCUADDR1_ECC_UNCORR_BANK_MASK 0x00070000
#define DWC_DDR_UMCTL2_REGS_ECCUADDR1_ECC_UNCORR_BANK_SHIFT 16

#define DWC_DDR_UMCTL2_REGS_ECCUADDR1_RESERVED1_MASK 0xFFF80000
#define DWC_DDR_UMCTL2_REGS_ECCUADDR1_RESERVED1_SHIFT 19

/**** ECCUSYN2 register ****/
/* Data pattern that resulted in an uncorrected error one for ea ... */
#define DWC_DDR_UMCTL2_REGS_ECCUSYN2_ECC_UNCORR_SYNDROMES_71_64_MASK 0x000000FF
#define DWC_DDR_UMCTL2_REGS_ECCUSYN2_ECC_UNCORR_SYNDROMES_71_64_SHIFT 0

#define DWC_DDR_UMCTL2_REGS_ECCUSYN2_RESERVED_MASK 0xFFFFFF00
#define DWC_DDR_UMCTL2_REGS_ECCUSYN2_RESERVED_SHIFT 8

/**** ECCPOISONADDR0 register ****/
/* Column address for ECC poisoning */
#define DWC_DDR_UMCTL2_REGS_ECCPOISONADDR0_ECC_POISON_COL_MASK 0x00000FFF
#define DWC_DDR_UMCTL2_REGS_ECCPOISONADDR0_ECC_POISON_COL_SHIFT 0

#define DWC_DDR_UMCTL2_REGS_ECCPOISONADDR0_RESERVED_MASK 0x00FFF000
#define DWC_DDR_UMCTL2_REGS_ECCPOISONADDR0_RESERVED_SHIFT 12
/* Rank address for ECC poisoning */
#define DWC_DDR_UMCTL2_REGS_ECCPOISONADDR0_ECC_POISON_RANK_MASK 0x03000000
#define DWC_DDR_UMCTL2_REGS_ECCPOISONADDR0_ECC_POISON_RANK_SHIFT 24

#define DWC_DDR_UMCTL2_REGS_ECCPOISONADDR0_RESERVED1_MASK 0xFC000000
#define DWC_DDR_UMCTL2_REGS_ECCPOISONADDR0_RESERVED1_SHIFT 26

/**** ECCPOISONADDR1 register ****/
/* Row address for ECC poisoning */
#define DWC_DDR_UMCTL2_REGS_ECCPOISONADDR1_ECC_POISON_ROW_MASK 0x0000FFFF
#define DWC_DDR_UMCTL2_REGS_ECCPOISONADDR1_ECC_POISON_ROW_SHIFT 0

#define DWC_DDR_UMCTL2_REGS_ECCPOISONADDR1_RESERVED_MASK 0x00FF0000
#define DWC_DDR_UMCTL2_REGS_ECCPOISONADDR1_RESERVED_SHIFT 16
/* Bank address for ECC poisoning */
#define DWC_DDR_UMCTL2_REGS_ECCPOISONADDR1_ECC_POISON_BANK_MASK 0x07000000
#define DWC_DDR_UMCTL2_REGS_ECCPOISONADDR1_ECC_POISON_BANK_SHIFT 24

#define DWC_DDR_UMCTL2_REGS_ECCPOISONADDR1_RESERVED1_MASK 0xF8000000
#define DWC_DDR_UMCTL2_REGS_ECCPOISONADDR1_RESERVED1_SHIFT 27

/**** PARCTL register ****/
/* Interrupt enable bit for DFI parity error */
#define DWC_DDR_UMCTL2_REGS_PARCTL_DFI_PARITY_ERR_INT_EN (1 << 0)
/* Interrupt clear bit for DFI parity error */
#define DWC_DDR_UMCTL2_REGS_PARCTL_DFI_PARITY_ERR_INT_CLR (1 << 1)
/* DFI parity error count clear */
#define DWC_DDR_UMCTL2_REGS_PARCTL_DFI_PARITY_ERR_CNT_CLR (1 << 2)

#define DWC_DDR_UMCTL2_REGS_PARCTL_RESERVED_MASK 0xFFFFFFF8
#define DWC_DDR_UMCTL2_REGS_PARCTL_RESERVED_SHIFT 3

/**** PARSTAT register ****/
/* DFI parity error count */
#define DWC_DDR_UMCTL2_REGS_PARSTAT_DFI_PARITY_ERR_CNT_MASK 0x0000FFFF
#define DWC_DDR_UMCTL2_REGS_PARSTAT_DFI_PARITY_ERR_CNT_SHIFT 0
/* DFI parity error interrupt */
#define DWC_DDR_UMCTL2_REGS_PARSTAT_DFI_PARITY_ERR_INT (1 << 16)

#define DWC_DDR_UMCTL2_REGS_PARSTAT_RESERVED_MASK 0xFFFE0000
#define DWC_DDR_UMCTL2_REGS_PARSTAT_RESERVED_SHIFT 17

/**** INIT0 register ****/
/* Cycles to wait after reset before driving CKE high to start t ... */
#define DWC_DDR_UMCTL2_REGS_INIT0_PRE_CKE_X1024_MASK 0x000003FF
#define DWC_DDR_UMCTL2_REGS_INIT0_PRE_CKE_X1024_SHIFT 0

#define DWC_DDR_UMCTL2_REGS_INIT0_RESERVED_MASK 0x0000FC00
#define DWC_DDR_UMCTL2_REGS_INIT0_RESERVED_SHIFT 10
/* Cycles to wait after driving CKE high to start the SDRAM init ... */
#define DWC_DDR_UMCTL2_REGS_INIT0_POST_CKE_X1024_MASK 0x03FF0000
#define DWC_DDR_UMCTL2_REGS_INIT0_POST_CKE_X1024_SHIFT 16

#define DWC_DDR_UMCTL2_REGS_INIT0_RESERVED1_MASK 0xFC000000
#define DWC_DDR_UMCTL2_REGS_INIT0_RESERVED1_SHIFT 26

/**** INIT1 register ****/
/* Wait period before driving the OCD complete command to SDRAM */
#define DWC_DDR_UMCTL2_REGS_INIT1_PRE_OCD_X32_MASK 0x0000000F
#define DWC_DDR_UMCTL2_REGS_INIT1_PRE_OCD_X32_SHIFT 0

#define DWC_DDR_UMCTL2_REGS_INIT1_RESERVED_MASK 0x000000F0
#define DWC_DDR_UMCTL2_REGS_INIT1_RESERVED_SHIFT 4
/* Cycles to wait after completing the SDRAM initialization sequ ... */
#define DWC_DDR_UMCTL2_REGS_INIT1_FINAL_WAIT_X32_MASK 0x00007F00
#define DWC_DDR_UMCTL2_REGS_INIT1_FINAL_WAIT_X32_SHIFT 8

#define DWC_DDR_UMCTL2_REGS_INIT1_RESERVED1 (1 << 15)
/* Number of cycles to assert SDRAM reset signal during init seq ... */
#define DWC_DDR_UMCTL2_REGS_INIT1_DRAM_RSTN_X1024_MASK 0x00FF0000
#define DWC_DDR_UMCTL2_REGS_INIT1_DRAM_RSTN_X1024_SHIFT 16

#define DWC_DDR_UMCTL2_REGS_INIT1_RESERVED2_MASK 0xFF000000
#define DWC_DDR_UMCTL2_REGS_INIT1_RESERVED2_SHIFT 24

/**** INIT3 register ****/
/* Non LPDDR2-Value to be loaded into SDRAM EMR registers */
#define DWC_DDR_UMCTL2_REGS_INIT3_EMR_MASK 0x0000FFFF
#define DWC_DDR_UMCTL2_REGS_INIT3_EMR_SHIFT 0
/* Non LPDDR2-Value to be loaded into the SDRAM Mode register */
#define DWC_DDR_UMCTL2_REGS_INIT3_MR_MASK 0xFFFF0000
#define DWC_DDR_UMCTL2_REGS_INIT3_MR_SHIFT 16

/**** INIT4 register ****/
/* Non LPDDR2- Value to be loaded into SDRAM EMR3 registers */
#define DWC_DDR_UMCTL2_REGS_INIT4_EMR3_MASK 0x0000FFFF
#define DWC_DDR_UMCTL2_REGS_INIT4_EMR3_SHIFT 0
/* Non LPDDR2- Value to be loaded into SDRAM EMR2 registers */
#define DWC_DDR_UMCTL2_REGS_INIT4_EMR2_MASK 0xFFFF0000
#define DWC_DDR_UMCTL2_REGS_INIT4_EMR2_SHIFT 16

/**** INIT5 register ****/

#define DWC_DDR_UMCTL2_REGS_INIT5_RESERVED_MASK 0x0000FFFF
#define DWC_DDR_UMCTL2_REGS_INIT5_RESERVED_SHIFT 0
/* ZQ initial calibration, t<sub>ZQINIT</sub> */
#define DWC_DDR_UMCTL2_REGS_INIT5_DEV_ZQINIT_X32_MASK 0x00FF0000
#define DWC_DDR_UMCTL2_REGS_INIT5_DEV_ZQINIT_X32_SHIFT 16

#define DWC_DDR_UMCTL2_REGS_INIT5_RESERVED1_MASK 0xFF000000
#define DWC_DDR_UMCTL2_REGS_INIT5_RESERVED1_SHIFT 24

/**** DIMMCTL register ****/
/* Staggering enable for multi-rank accesses (for multi-rank UDI ... */
#define DWC_DDR_UMCTL2_REGS_DIMMCTL_DIMM_STAGGER_CS_EN (1 << 0)
/* Address Mirroring Enable (for multi-rank UDIMM implementation ... */
#define DWC_DDR_UMCTL2_REGS_DIMMCTL_DIMM_ADDR_MIRR_EN (1 << 1)

#define DWC_DDR_UMCTL2_REGS_DIMMCTL_RESERVED_MASK 0xFFFFFFFC
#define DWC_DDR_UMCTL2_REGS_DIMMCTL_RESERVED_SHIFT 2

/**** RANKCTL register ****/
/* Only present for multi-rank configurations */
#define DWC_DDR_UMCTL2_REGS_RANKCTL_MAX_RANK_RD_MASK 0x0000000F
#define DWC_DDR_UMCTL2_REGS_RANKCTL_MAX_RANK_RD_SHIFT 0
/* Only present for multi-rank configurations */
#define DWC_DDR_UMCTL2_REGS_RANKCTL_DIFF_RANK_RD_GAP_MASK 0x000000F0
#define DWC_DDR_UMCTL2_REGS_RANKCTL_DIFF_RANK_RD_GAP_SHIFT 4
/* Only present for multi-rank configurations */
#define DWC_DDR_UMCTL2_REGS_RANKCTL_DIFF_RANK_WR_GAP_MASK 0x00000F00
#define DWC_DDR_UMCTL2_REGS_RANKCTL_DIFF_RANK_WR_GAP_SHIFT 8

#define DWC_DDR_UMCTL2_REGS_RANKCTL_RESERVED_MASK 0xFFFFF000
#define DWC_DDR_UMCTL2_REGS_RANKCTL_RESERVED_SHIFT 12

/**** DRAMTMG0 register ****/
/* t<sub>RAS</sub>(min):  Minimum time between activate and prec ... */
#define DWC_DDR_UMCTL2_REGS_DRAMTMG0_T_RAS_MIN_MASK 0x0000003F
#define DWC_DDR_UMCTL2_REGS_DRAMTMG0_T_RAS_MIN_SHIFT 0

#define DWC_DDR_UMCTL2_REGS_DRAMTMG0_RESERVED_MASK 0x000000C0
#define DWC_DDR_UMCTL2_REGS_DRAMTMG0_RESERVED_SHIFT 6
/* t<sub>RAS</sub>(max):  Maximum time between activate and prec ... */
#define DWC_DDR_UMCTL2_REGS_DRAMTMG0_T_RAS_MAX_MASK 0x00003F00
#define DWC_DDR_UMCTL2_REGS_DRAMTMG0_T_RAS_MAX_SHIFT 8

#define DWC_DDR_UMCTL2_REGS_DRAMTMG0_RESERVED1_MASK 0x0000C000
#define DWC_DDR_UMCTL2_REGS_DRAMTMG0_RESERVED1_SHIFT 14
/* t<sub>FAW</sub> Valid only when 8 banks are present */
#define DWC_DDR_UMCTL2_REGS_DRAMTMG0_T_FAW_MASK 0x003F0000
#define DWC_DDR_UMCTL2_REGS_DRAMTMG0_T_FAW_SHIFT 16

#define DWC_DDR_UMCTL2_REGS_DRAMTMG0_RESERVED2_MASK 0x00C00000
#define DWC_DDR_UMCTL2_REGS_DRAMTMG0_RESERVED2_SHIFT 22
/* Minimum time between write and precharge to same bank */
#define DWC_DDR_UMCTL2_REGS_DRAMTMG0_WR2PRE_MASK 0x3F000000
#define DWC_DDR_UMCTL2_REGS_DRAMTMG0_WR2PRE_SHIFT 24

#define DWC_DDR_UMCTL2_REGS_DRAMTMG0_RESERVED3_MASK 0xC0000000
#define DWC_DDR_UMCTL2_REGS_DRAMTMG0_RESERVED3_SHIFT 30

/**** DRAMTMG1 register ****/
/* t<sub>RC</sub>:  Minimum time between activates to same bank */
#define DWC_DDR_UMCTL2_REGS_DRAMTMG1_T_RC_MASK 0x0000003F
#define DWC_DDR_UMCTL2_REGS_DRAMTMG1_T_RC_SHIFT 0

#define DWC_DDR_UMCTL2_REGS_DRAMTMG1_RESERVED_MASK 0x000000C0
#define DWC_DDR_UMCTL2_REGS_DRAMTMG1_RESERVED_SHIFT 6
/* t<sub>RTP</sub>:  Minimum time from read to precharge of same ... */
#define DWC_DDR_UMCTL2_REGS_DRAMTMG1_RD2PRE_MASK 0x00001F00
#define DWC_DDR_UMCTL2_REGS_DRAMTMG1_RD2PRE_SHIFT 8

#define DWC_DDR_UMCTL2_REGS_DRAMTMG1_RESERVED1_MASK 0x0000E000
#define DWC_DDR_UMCTL2_REGS_DRAMTMG1_RESERVED1_SHIFT 13
/* t<sub>XP</sub>: Minimum time after power-down exit to any ope ... */
#define DWC_DDR_UMCTL2_REGS_DRAMTMG1_T_XP_MASK 0x001F0000
#define DWC_DDR_UMCTL2_REGS_DRAMTMG1_T_XP_SHIFT 16

#define DWC_DDR_UMCTL2_REGS_DRAMTMG1_RESERVED2_MASK 0xFFE00000
#define DWC_DDR_UMCTL2_REGS_DRAMTMG1_RESERVED2_SHIFT 21

/**** DRAMTMG2 register ****/
/* WL + BL/2 + t<sub>WTR</sub><br>Minimum time from write comman ... */
#define DWC_DDR_UMCTL2_REGS_DRAMTMG2_WR2RD_MASK 0x0000003F
#define DWC_DDR_UMCTL2_REGS_DRAMTMG2_WR2RD_SHIFT 0

#define DWC_DDR_UMCTL2_REGS_DRAMTMG2_RESERVED_MASK 0x000000C0
#define DWC_DDR_UMCTL2_REGS_DRAMTMG2_RESERVED_SHIFT 6
/* DDR2/3/mDDR: RL + BL/2 + 2 - WL<br>LPDDR2: RL + BL/2 + RU(t<s ... */
#define DWC_DDR_UMCTL2_REGS_DRAMTMG2_RD2WR_MASK 0x00001F00
#define DWC_DDR_UMCTL2_REGS_DRAMTMG2_RD2WR_SHIFT 8

#define DWC_DDR_UMCTL2_REGS_DRAMTMG2_RESERVED1_MASK 0xFFFFE000
#define DWC_DDR_UMCTL2_REGS_DRAMTMG2_RESERVED1_SHIFT 13

/**** DRAMTMG3 register ****/
/* t<sub>MOD</sub>: Present if MEMC_DDR3 = 1 only */
#define DWC_DDR_UMCTL2_REGS_DRAMTMG3_T_MOD_MASK 0x000003FF
#define DWC_DDR_UMCTL2_REGS_DRAMTMG3_T_MOD_SHIFT 0

#define DWC_DDR_UMCTL2_REGS_DRAMTMG3_RESERVED_MASK 0x00000C00
#define DWC_DDR_UMCTL2_REGS_DRAMTMG3_RESERVED_SHIFT 10
/* t<sub>MRD</sub>: Cycles between load mode commands */
#define DWC_DDR_UMCTL2_REGS_DRAMTMG3_T_MRD_MASK 0x00007000
#define DWC_DDR_UMCTL2_REGS_DRAMTMG3_T_MRD_SHIFT 12

#define DWC_DDR_UMCTL2_REGS_DRAMTMG3_RESERVED1_MASK 0xFFFF8000
#define DWC_DDR_UMCTL2_REGS_DRAMTMG3_RESERVED1_SHIFT 15

/**** DRAMTMG4 register ****/
/* t<sub>RP</sub>:  Minimum time from precharge to activate of s ... */
#define DWC_DDR_UMCTL2_REGS_DRAMTMG4_T_RP_MASK 0x0000000F
#define DWC_DDR_UMCTL2_REGS_DRAMTMG4_T_RP_SHIFT 0

#define DWC_DDR_UMCTL2_REGS_DRAMTMG4_RESERVED_MASK 0x000000F0
#define DWC_DDR_UMCTL2_REGS_DRAMTMG4_RESERVED_SHIFT 4
/* t<sub>RRD</sub>:  Minimum time between activates from bank "a ... */
#define DWC_DDR_UMCTL2_REGS_DRAMTMG4_T_RRD_MASK 0x00000700
#define DWC_DDR_UMCTL2_REGS_DRAMTMG4_T_RRD_SHIFT 8

#define DWC_DDR_UMCTL2_REGS_DRAMTMG4_RESERVED1_MASK 0x0000F800
#define DWC_DDR_UMCTL2_REGS_DRAMTMG4_RESERVED1_SHIFT 11
/* t<sub>CCD</sub>:  This is the minimum time between two reads  ... */
#define DWC_DDR_UMCTL2_REGS_DRAMTMG4_T_CCD_MASK 0x00070000
#define DWC_DDR_UMCTL2_REGS_DRAMTMG4_T_CCD_SHIFT 16

#define DWC_DDR_UMCTL2_REGS_DRAMTMG4_RESERVED2_MASK 0x00F80000
#define DWC_DDR_UMCTL2_REGS_DRAMTMG4_RESERVED2_SHIFT 19
/* t<sub>RCD</sub> - t<sub>AL</sub>: Minimum time from activate  ... */
#define DWC_DDR_UMCTL2_REGS_DRAMTMG4_T_RCD_MASK 0x0F000000
#define DWC_DDR_UMCTL2_REGS_DRAMTMG4_T_RCD_SHIFT 24

#define DWC_DDR_UMCTL2_REGS_DRAMTMG4_RESERVED3_MASK 0xF0000000
#define DWC_DDR_UMCTL2_REGS_DRAMTMG4_RESERVED3_SHIFT 28

/**** DRAMTMG5 register ****/
/* Minimum number of cycles of CKE HIGH/LOW during power-down an ... */
#define DWC_DDR_UMCTL2_REGS_DRAMTMG5_T_CKE_MASK 0x0000000F
#define DWC_DDR_UMCTL2_REGS_DRAMTMG5_T_CKE_SHIFT 0

#define DWC_DDR_UMCTL2_REGS_DRAMTMG5_RESERVED_MASK 0x000000F0
#define DWC_DDR_UMCTL2_REGS_DRAMTMG5_RESERVED_SHIFT 4
/* Minimum CKE low width for Self refresh entry to exit timing i ... */
#define DWC_DDR_UMCTL2_REGS_DRAMTMG5_T_CKESR_MASK 0x00003F00
#define DWC_DDR_UMCTL2_REGS_DRAMTMG5_T_CKESR_SHIFT 8

#define DWC_DDR_UMCTL2_REGS_DRAMTMG5_RESERVED1_MASK 0x0000C000
#define DWC_DDR_UMCTL2_REGS_DRAMTMG5_RESERVED1_SHIFT 14
/* Specifies the number of DFI clock cycles from the de-assertio ... */
#define DWC_DDR_UMCTL2_REGS_DRAMTMG5_T_CKSRE_MASK 0x000F0000
#define DWC_DDR_UMCTL2_REGS_DRAMTMG5_T_CKSRE_SHIFT 16

#define DWC_DDR_UMCTL2_REGS_DRAMTMG5_RESERVED2_MASK 0x00F00000
#define DWC_DDR_UMCTL2_REGS_DRAMTMG5_RESERVED2_SHIFT 20
/* This is the time before Self Refresh Exit that CK is maintain ... */
#define DWC_DDR_UMCTL2_REGS_DRAMTMG5_T_CKSRX_MASK 0x0F000000
#define DWC_DDR_UMCTL2_REGS_DRAMTMG5_T_CKSRX_SHIFT 24

#define DWC_DDR_UMCTL2_REGS_DRAMTMG5_RESERVED3_MASK 0xF0000000
#define DWC_DDR_UMCTL2_REGS_DRAMTMG5_RESERVED3_SHIFT 28

/**** DRAMTMG8 register ****/
/* Minimum time to wait after coming out of self refresh before  ... */
#define DWC_DDR_UMCTL2_REGS_DRAMTMG8_POST_SELFREF_GAP_X32_MASK 0x0000007F
#define DWC_DDR_UMCTL2_REGS_DRAMTMG8_POST_SELFREF_GAP_X32_SHIFT 0

#define DWC_DDR_UMCTL2_REGS_DRAMTMG8_RESERVED_MASK 0xFFFFFF80
#define DWC_DDR_UMCTL2_REGS_DRAMTMG8_RESERVED_SHIFT 7

/**** ZQCTL0 register ****/
/* t<sub>ZQCS</sub>: Number of cycles of NOP required after a ZQ ... */
#define DWC_DDR_UMCTL2_REGS_ZQCTL0_T_ZQ_SHORT_NOP_MASK 0x000003FF
#define DWC_DDR_UMCTL2_REGS_ZQCTL0_T_ZQ_SHORT_NOP_SHIFT 0

#define DWC_DDR_UMCTL2_REGS_ZQCTL0_RESERVED_MASK 0x0000FC00
#define DWC_DDR_UMCTL2_REGS_ZQCTL0_RESERVED_SHIFT 10
/* t<sub>ZQOPER</sub> for DDR3, t<sub>ZQCL</sub> for LPDDR2: Num ... */
#define DWC_DDR_UMCTL2_REGS_ZQCTL0_T_ZQ_LONG_NOP_MASK 0x03FF0000
#define DWC_DDR_UMCTL2_REGS_ZQCTL0_T_ZQ_LONG_NOP_SHIFT 16

#define DWC_DDR_UMCTL2_REGS_ZQCTL0_RESERVED1_MASK 0x3C000000
#define DWC_DDR_UMCTL2_REGS_ZQCTL0_RESERVED1_SHIFT 26
/*  - 1 - Disable issuing of ZQCL command at Self-Refresh exit */
#define DWC_DDR_UMCTL2_REGS_ZQCTL0_DIS_SRX_ZQCL (1 << 30)
/*  - 1 - Disable uMCTL2 generation of ZQCS command */
#define DWC_DDR_UMCTL2_REGS_ZQCTL0_DIS_AUTO_ZQ (1 << 31)

/**** ZQCTL1 register ****/
/* Average interval to wait between automatically issuing ZQCS ( ... */
#define DWC_DDR_UMCTL2_REGS_ZQCTL1_T_ZQ_SHORT_INTERVAL_X1024_MASK 0x000FFFFF
#define DWC_DDR_UMCTL2_REGS_ZQCTL1_T_ZQ_SHORT_INTERVAL_X1024_SHIFT 0

#define DWC_DDR_UMCTL2_REGS_ZQCTL1_RESERVED_MASK 0xFFF00000
#define DWC_DDR_UMCTL2_REGS_ZQCTL1_RESERVED_SHIFT 20

/**** DFITMG0 register ****/
/* Write latency<br>Number of clocks from the write command to w ... */
#define DWC_DDR_UMCTL2_REGS_DFITMG0_WRITE_LATENCY_MASK 0x0000001F
#define DWC_DDR_UMCTL2_REGS_DFITMG0_WRITE_LATENCY_SHIFT 0

#define DWC_DDR_UMCTL2_REGS_DFITMG0_RESERVED_MASK 0x000000E0
#define DWC_DDR_UMCTL2_REGS_DFITMG0_RESERVED_SHIFT 5
/* Specifies the number of clock cycles between when dfi_wrdata_ ... */
#define DWC_DDR_UMCTL2_REGS_DFITMG0_DFI_TPHY_WRDATA_MASK 0x00001F00
#define DWC_DDR_UMCTL2_REGS_DFITMG0_DFI_TPHY_WRDATA_SHIFT 8

#define DWC_DDR_UMCTL2_REGS_DFITMG0_RESERVED1_MASK 0x0000E000
#define DWC_DDR_UMCTL2_REGS_DFITMG0_RESERVED1_SHIFT 13
/* Time from the assertion of a read command on the DFI interfac ... */
#define DWC_DDR_UMCTL2_REGS_DFITMG0_DFI_T_RDDATA_EN_MASK 0x001F0000
#define DWC_DDR_UMCTL2_REGS_DFITMG0_DFI_T_RDDATA_EN_SHIFT 16

#define DWC_DDR_UMCTL2_REGS_DFITMG0_RESERVED2_MASK 0x00E00000
#define DWC_DDR_UMCTL2_REGS_DFITMG0_RESERVED2_SHIFT 21
/* Specifies the number of DFI clock cycles after an assertion o ... */
#define DWC_DDR_UMCTL2_REGS_DFITMG0_DFI_T_CTRL_DELAY_MASK 0x0F000000
#define DWC_DDR_UMCTL2_REGS_DFITMG0_DFI_T_CTRL_DELAY_SHIFT 24

#define DWC_DDR_UMCTL2_REGS_DFITMG0_RESERVED3_MASK 0xF0000000
#define DWC_DDR_UMCTL2_REGS_DFITMG0_RESERVED3_SHIFT 28

/**** DFITMG1 register ****/
/* Specifies the number of DFI clock cycles from the de-assertio ... */
#define DWC_DDR_UMCTL2_REGS_DFITMG1_DFI_T_DRAM_CLK_ENABLE_MASK 0x0000000F
#define DWC_DDR_UMCTL2_REGS_DFITMG1_DFI_T_DRAM_CLK_ENABLE_SHIFT 0

#define DWC_DDR_UMCTL2_REGS_DFITMG1_RESERVED_MASK 0x000000F0
#define DWC_DDR_UMCTL2_REGS_DFITMG1_RESERVED_SHIFT 4
/* Specifies the number of DFI clock cycles from the assertion o ... */
#define DWC_DDR_UMCTL2_REGS_DFITMG1_DFI_T_DRAM_CLK_DISABLE_MASK 0x00000F00
#define DWC_DDR_UMCTL2_REGS_DFITMG1_DFI_T_DRAM_CLK_DISABLE_SHIFT 8

#define DWC_DDR_UMCTL2_REGS_DFITMG1_RESERVED1_MASK 0xFFFFF000
#define DWC_DDR_UMCTL2_REGS_DFITMG1_RESERVED1_SHIFT 12

/**** DFIUPD0 register ****/
/* Specifies the minimum number of clock cycles that the dfi_ctr ... */
#define DWC_DDR_UMCTL2_REGS_DFIUPD0_DFI_T_CTRLUP_MIN_MASK 0x000003FF
#define DWC_DDR_UMCTL2_REGS_DFIUPD0_DFI_T_CTRLUP_MIN_SHIFT 0

#define DWC_DDR_UMCTL2_REGS_DFIUPD0_RESERVED_MASK 0x0000FC00
#define DWC_DDR_UMCTL2_REGS_DFIUPD0_RESERVED_SHIFT 10
/* Specifies the maximum number of clock cycles that the dfi_ctr ... */
#define DWC_DDR_UMCTL2_REGS_DFIUPD0_DFI_T_CTRLUP_MAX_MASK 0x03FF0000
#define DWC_DDR_UMCTL2_REGS_DFIUPD0_DFI_T_CTRLUP_MAX_SHIFT 16

#define DWC_DDR_UMCTL2_REGS_DFIUPD0_RESERVED1_MASK 0x7C000000
#define DWC_DDR_UMCTL2_REGS_DFIUPD0_RESERVED1_SHIFT 26
/* When '1', disable co_gs_dll_calib generated by the uMCTL2 */
#define DWC_DDR_UMCTL2_REGS_DFIUPD0_DIS_DLL_CALIB (1 << 31)

/**** DFIUPD1 register ****/
/* This is the maximum amount of time between uMCTL2 initiated D ... */
#define DWC_DDR_UMCTL2_REGS_DFIUPD1_DFI_T_CTRLUPD_INTERVAL_MAX_X1024_MASK 0x000000FF
#define DWC_DDR_UMCTL2_REGS_DFIUPD1_DFI_T_CTRLUPD_INTERVAL_MAX_X1024_SHIFT 0

#define DWC_DDR_UMCTL2_REGS_DFIUPD1_RESERVED_MASK 0x0000FF00
#define DWC_DDR_UMCTL2_REGS_DFIUPD1_RESERVED_SHIFT 8
/* This is the minimum amount of time between uMCTL2 initiated D ... */
#define DWC_DDR_UMCTL2_REGS_DFIUPD1_DFI_T_CTRLUPD_INTERVAL_MIN_X1024_MASK 0x00FF0000
#define DWC_DDR_UMCTL2_REGS_DFIUPD1_DFI_T_CTRLUPD_INTERVAL_MIN_X1024_SHIFT 16

#define DWC_DDR_UMCTL2_REGS_DFIUPD1_RESERVED1_MASK 0xFF000000
#define DWC_DDR_UMCTL2_REGS_DFIUPD1_RESERVED1_SHIFT 24

/**** DFIUPD2 register ****/
/* Specifies the maximum number of DFI clock cycles that the dfi ... */
#define DWC_DDR_UMCTL2_REGS_DFIUPD2_DFI_PHYUPD_TYPE0_MASK 0x00000FFF
#define DWC_DDR_UMCTL2_REGS_DFIUPD2_DFI_PHYUPD_TYPE0_SHIFT 0

#define DWC_DDR_UMCTL2_REGS_DFIUPD2_RESERVED_MASK 0x0000F000
#define DWC_DDR_UMCTL2_REGS_DFIUPD2_RESERVED_SHIFT 12
/* Specifies the maximum number of DFI clock cycles that the dfi ... */
#define DWC_DDR_UMCTL2_REGS_DFIUPD2_DFI_PHYUPD_TYPE1_MASK 0x0FFF0000
#define DWC_DDR_UMCTL2_REGS_DFIUPD2_DFI_PHYUPD_TYPE1_SHIFT 16

#define DWC_DDR_UMCTL2_REGS_DFIUPD2_RESERVED1_MASK 0x70000000
#define DWC_DDR_UMCTL2_REGS_DFIUPD2_RESERVED1_SHIFT 28
/* Enables the support for acknowledging PHY-initiated updates:  ... */
#define DWC_DDR_UMCTL2_REGS_DFIUPD2_DFI_PHYUPD_EN (1 << 31)

/**** DFIUPD3 register ****/
/* Specifies the maximum number of DFI clock cycles that the dfi ... */
#define DWC_DDR_UMCTL2_REGS_DFIUPD3_DFI_PHYUPD_TYPE2_MASK 0x00000FFF
#define DWC_DDR_UMCTL2_REGS_DFIUPD3_DFI_PHYUPD_TYPE2_SHIFT 0

#define DWC_DDR_UMCTL2_REGS_DFIUPD3_RESERVED_MASK 0x0000F000
#define DWC_DDR_UMCTL2_REGS_DFIUPD3_RESERVED_SHIFT 12
/* Specifies the maximum number of DFI clock cycles that the dfi ... */
#define DWC_DDR_UMCTL2_REGS_DFIUPD3_DFI_PHYUPD_TYPE3_MASK 0x0FFF0000
#define DWC_DDR_UMCTL2_REGS_DFIUPD3_DFI_PHYUPD_TYPE3_SHIFT 16

#define DWC_DDR_UMCTL2_REGS_DFIUPD3_RESERVED1_MASK 0xF0000000
#define DWC_DDR_UMCTL2_REGS_DFIUPD3_RESERVED1_SHIFT 28

/**** DFIMISC register ****/
/* PHY initialization complete enable signal */
#define DWC_DDR_UMCTL2_REGS_DFIMISC_DFI_INIT_COMPLETE_EN (1 << 0)

#define DWC_DDR_UMCTL2_REGS_DFIMISC_RESERVED_MASK 0xFFFFFFFE
#define DWC_DDR_UMCTL2_REGS_DFIMISC_RESERVED_SHIFT 1

/* Address mapping bases */
#define AL_DDR_ADDR_MAP_CS_0_BASE	6

#define AL_DDR_ADDR_MAP_CS_DISABLED	31

#define AL_DDR_ADDR_MAP_BANK_0_BASE	2

#define AL_DDR_ADDR_MAP_BANK_DISABLED	15

#define AL_DDR_ADDR_MAP_COL_2_BASE	2

#define AL_DDR_ADDR_MAP_COL_DISABLED	15

#define AL_DDR_ADDR_MAP_ROW_0_BASE	6
#define AL_DDR_ADDR_MAP_ROW_11_BASE	17

#define AL_DDR_ADDR_MAP_ROW_DISABLED	15

#define AL_DDR_ADDR_MAP_OFFSET		4

/**** ADDRMAP0 register ****/
/* Selects the HIF address bit used as rank address bit 0 */
#define DWC_DDR_UMCTL2_REGS_ADDRMAP0_ADDRMAP_CS_BIT0_MASK 0x0000001F
#define DWC_DDR_UMCTL2_REGS_ADDRMAP0_ADDRMAP_CS_BIT0_SHIFT 0

#define DWC_DDR_UMCTL2_REGS_ADDRMAP0_RESERVED_MASK 0x000000E0
#define DWC_DDR_UMCTL2_REGS_ADDRMAP0_RESERVED_SHIFT 5
/* Selects the HIF address bit used as rank address bit 1 */
#define DWC_DDR_UMCTL2_REGS_ADDRMAP0_ADDRMAP_CS_BIT1_MASK 0x00001F00
#define DWC_DDR_UMCTL2_REGS_ADDRMAP0_ADDRMAP_CS_BIT1_SHIFT 8

#define DWC_DDR_UMCTL2_REGS_ADDRMAP0_RESERVED1_MASK 0xFFFFE000
#define DWC_DDR_UMCTL2_REGS_ADDRMAP0_RESERVED1_SHIFT 13

/**** ADDRMAP1 register ****/
/* Selects the HIF address bits used as bank address bit 0 */
#define DWC_DDR_UMCTL2_REGS_ADDRMAP1_ADDRMAP_BANK_B0_MASK 0x0000000F
#define DWC_DDR_UMCTL2_REGS_ADDRMAP1_ADDRMAP_BANK_B0_SHIFT 0

#define DWC_DDR_UMCTL2_REGS_ADDRMAP1_RESERVED_MASK 0x000000F0
#define DWC_DDR_UMCTL2_REGS_ADDRMAP1_RESERVED_SHIFT 4
/* Selects the HIF address bits used as bank address bit 1 */
#define DWC_DDR_UMCTL2_REGS_ADDRMAP1_ADDRMAP_BANK_B1_MASK 0x00000F00
#define DWC_DDR_UMCTL2_REGS_ADDRMAP1_ADDRMAP_BANK_B1_SHIFT 8

#define DWC_DDR_UMCTL2_REGS_ADDRMAP1_RESERVED1_MASK 0x0000F000
#define DWC_DDR_UMCTL2_REGS_ADDRMAP1_RESERVED1_SHIFT 12
/* Selects the HIF address bit used as bank address bit 2 */
#define DWC_DDR_UMCTL2_REGS_ADDRMAP1_ADDRMAP_BANK_B2_MASK 0x000F0000
#define DWC_DDR_UMCTL2_REGS_ADDRMAP1_ADDRMAP_BANK_B2_SHIFT 16

#define DWC_DDR_UMCTL2_REGS_ADDRMAP1_RESERVED2_MASK 0xFFF00000
#define DWC_DDR_UMCTL2_REGS_ADDRMAP1_RESERVED2_SHIFT 20

/**** ADDRMAP2 register ****/
/* Full bus width mode: Selects the HIF address bit used as colu ... */
#define DWC_DDR_UMCTL2_REGS_ADDRMAP2_ADDRMAP_COL_B2_MASK 0x0000000F
#define DWC_DDR_UMCTL2_REGS_ADDRMAP2_ADDRMAP_COL_B2_SHIFT 0

#define DWC_DDR_UMCTL2_REGS_ADDRMAP2_RESERVED_MASK 0x000000F0
#define DWC_DDR_UMCTL2_REGS_ADDRMAP2_RESERVED_SHIFT 4
/* Full bus width mode: Selects the HIF address bit used as colu ... */
#define DWC_DDR_UMCTL2_REGS_ADDRMAP2_ADDRMAP_COL_B3_MASK 0x00000F00
#define DWC_DDR_UMCTL2_REGS_ADDRMAP2_ADDRMAP_COL_B3_SHIFT 8

#define DWC_DDR_UMCTL2_REGS_ADDRMAP2_RESERVED1_MASK 0x0000F000
#define DWC_DDR_UMCTL2_REGS_ADDRMAP2_RESERVED1_SHIFT 12
/* Full bus width mode: Selects the HIF address bit used as colu ... */
#define DWC_DDR_UMCTL2_REGS_ADDRMAP2_ADDRMAP_COL_B4_MASK 0x000F0000
#define DWC_DDR_UMCTL2_REGS_ADDRMAP2_ADDRMAP_COL_B4_SHIFT 16

#define DWC_DDR_UMCTL2_REGS_ADDRMAP2_RESERVED2_MASK 0x00F00000
#define DWC_DDR_UMCTL2_REGS_ADDRMAP2_RESERVED2_SHIFT 20
/* Full bus width mode: Selects the HIF address bit used as colu ... */
#define DWC_DDR_UMCTL2_REGS_ADDRMAP2_ADDRMAP_COL_B5_MASK 0x0F000000
#define DWC_DDR_UMCTL2_REGS_ADDRMAP2_ADDRMAP_COL_B5_SHIFT 24

#define DWC_DDR_UMCTL2_REGS_ADDRMAP2_RESERVED3_MASK 0xF0000000
#define DWC_DDR_UMCTL2_REGS_ADDRMAP2_RESERVED3_SHIFT 28

/**** ADDRMAP3 register ****/
/* Full bus width mode: Selects the HIF address bit used as colu ... */
#define DWC_DDR_UMCTL2_REGS_ADDRMAP3_ADDRMAP_COL_B6_MASK 0x0000000F
#define DWC_DDR_UMCTL2_REGS_ADDRMAP3_ADDRMAP_COL_B6_SHIFT 0

#define DWC_DDR_UMCTL2_REGS_ADDRMAP3_RESERVED_MASK 0x000000F0
#define DWC_DDR_UMCTL2_REGS_ADDRMAP3_RESERVED_SHIFT 4
/* Full bus width mode: Selects the HIF address bit used as colu ... */
#define DWC_DDR_UMCTL2_REGS_ADDRMAP3_ADDRMAP_COL_B7_MASK 0x00000F00
#define DWC_DDR_UMCTL2_REGS_ADDRMAP3_ADDRMAP_COL_B7_SHIFT 8

#define DWC_DDR_UMCTL2_REGS_ADDRMAP3_RESERVED1_MASK 0x0000F000
#define DWC_DDR_UMCTL2_REGS_ADDRMAP3_RESERVED1_SHIFT 12
/* Full bus width mode: Selects the HIF address bit used as colu ... */
#define DWC_DDR_UMCTL2_REGS_ADDRMAP3_ADDRMAP_COL_B8_MASK 0x000F0000
#define DWC_DDR_UMCTL2_REGS_ADDRMAP3_ADDRMAP_COL_B8_SHIFT 16

#define DWC_DDR_UMCTL2_REGS_ADDRMAP3_RESERVED2_MASK 0x00F00000
#define DWC_DDR_UMCTL2_REGS_ADDRMAP3_RESERVED2_SHIFT 20
/* Full bus width mode: Selects the HIF address bit used as colu ... */
#define DWC_DDR_UMCTL2_REGS_ADDRMAP3_ADDRMAP_COL_B9_MASK 0x0F000000
#define DWC_DDR_UMCTL2_REGS_ADDRMAP3_ADDRMAP_COL_B9_SHIFT 24

#define DWC_DDR_UMCTL2_REGS_ADDRMAP3_RESERVED3_MASK 0xF0000000
#define DWC_DDR_UMCTL2_REGS_ADDRMAP3_RESERVED3_SHIFT 28

/**** ADDRMAP4 register ****/
/* Full bus width mode: Selects the HIF address bit used as colu ... */
#define DWC_DDR_UMCTL2_REGS_ADDRMAP4_ADDRMAP_COL_B10_MASK 0x0000000F
#define DWC_DDR_UMCTL2_REGS_ADDRMAP4_ADDRMAP_COL_B10_SHIFT 0

#define DWC_DDR_UMCTL2_REGS_ADDRMAP4_RESERVED_MASK 0x000000F0
#define DWC_DDR_UMCTL2_REGS_ADDRMAP4_RESERVED_SHIFT 4
/* Full bus width mode: Selects the HIF address bit used as colu ... */
#define DWC_DDR_UMCTL2_REGS_ADDRMAP4_ADDRMAP_COL_B11_MASK 0x00000F00
#define DWC_DDR_UMCTL2_REGS_ADDRMAP4_ADDRMAP_COL_B11_SHIFT 8

#define DWC_DDR_UMCTL2_REGS_ADDRMAP4_RESERVED1_MASK 0xFFFFF000
#define DWC_DDR_UMCTL2_REGS_ADDRMAP4_RESERVED1_SHIFT 12

/**** ADDRMAP5 register ****/
/* Selects the HIF address bits used as row address bit 0 */
#define DWC_DDR_UMCTL2_REGS_ADDRMAP5_ADDRMAP_ROW_B0_MASK 0x0000000F
#define DWC_DDR_UMCTL2_REGS_ADDRMAP5_ADDRMAP_ROW_B0_SHIFT 0

#define DWC_DDR_UMCTL2_REGS_ADDRMAP5_RESERVED_MASK 0x000000F0
#define DWC_DDR_UMCTL2_REGS_ADDRMAP5_RESERVED_SHIFT 4
/* Selects the HIF address bits used as row address bit 1 */
#define DWC_DDR_UMCTL2_REGS_ADDRMAP5_ADDRMAP_ROW_B1_MASK 0x00000F00
#define DWC_DDR_UMCTL2_REGS_ADDRMAP5_ADDRMAP_ROW_B1_SHIFT 8

#define DWC_DDR_UMCTL2_REGS_ADDRMAP5_RESERVED1_MASK 0x0000F000
#define DWC_DDR_UMCTL2_REGS_ADDRMAP5_RESERVED1_SHIFT 12
/* Selects the HIF address bits used as row address bits 2 to 10 */
#define DWC_DDR_UMCTL2_REGS_ADDRMAP5_ADDRMAP_ROW_B2_10_MASK 0x000F0000
#define DWC_DDR_UMCTL2_REGS_ADDRMAP5_ADDRMAP_ROW_B2_10_SHIFT 16

#define DWC_DDR_UMCTL2_REGS_ADDRMAP5_RESERVED2_MASK 0x00F00000
#define DWC_DDR_UMCTL2_REGS_ADDRMAP5_RESERVED2_SHIFT 20
/* Selects the HIF address bit used as row address bit 11 */
#define DWC_DDR_UMCTL2_REGS_ADDRMAP5_ADDRMAP_ROW_B11_MASK 0x0F000000
#define DWC_DDR_UMCTL2_REGS_ADDRMAP5_ADDRMAP_ROW_B11_SHIFT 24

#define DWC_DDR_UMCTL2_REGS_ADDRMAP5_RESERVED3_MASK 0xF0000000
#define DWC_DDR_UMCTL2_REGS_ADDRMAP5_RESERVED3_SHIFT 28

/**** ADDRMAP6 register ****/
/* Selects the HIF address bit used as row address bit 12 */
#define DWC_DDR_UMCTL2_REGS_ADDRMAP6_ADDRMAP_ROW_B12_MASK 0x0000000F
#define DWC_DDR_UMCTL2_REGS_ADDRMAP6_ADDRMAP_ROW_B12_SHIFT 0

#define DWC_DDR_UMCTL2_REGS_ADDRMAP6_RESERVED_MASK 0x000000F0
#define DWC_DDR_UMCTL2_REGS_ADDRMAP6_RESERVED_SHIFT 4
/* Selects the HIF address bit used as row address bit 13 */
#define DWC_DDR_UMCTL2_REGS_ADDRMAP6_ADDRMAP_ROW_B13_MASK 0x00000F00
#define DWC_DDR_UMCTL2_REGS_ADDRMAP6_ADDRMAP_ROW_B13_SHIFT 8

#define DWC_DDR_UMCTL2_REGS_ADDRMAP6_RESERVED1_MASK 0x0000F000
#define DWC_DDR_UMCTL2_REGS_ADDRMAP6_RESERVED1_SHIFT 12
/* Selects the HIF address bit used as row address bit 14 */
#define DWC_DDR_UMCTL2_REGS_ADDRMAP6_ADDRMAP_ROW_B14_MASK 0x000F0000
#define DWC_DDR_UMCTL2_REGS_ADDRMAP6_ADDRMAP_ROW_B14_SHIFT 16

#define DWC_DDR_UMCTL2_REGS_ADDRMAP6_RESERVED2_MASK 0x00F00000
#define DWC_DDR_UMCTL2_REGS_ADDRMAP6_RESERVED2_SHIFT 20
/* Selects the HIF address bit used as row address bit 15 */
#define DWC_DDR_UMCTL2_REGS_ADDRMAP6_ADDRMAP_ROW_B15_MASK 0x0F000000
#define DWC_DDR_UMCTL2_REGS_ADDRMAP6_ADDRMAP_ROW_B15_SHIFT 24

#define DWC_DDR_UMCTL2_REGS_ADDRMAP6_RESERVED3_MASK 0xF0000000
#define DWC_DDR_UMCTL2_REGS_ADDRMAP6_RESERVED3_SHIFT 28

/**** ODTCFG register ****/
/* Controls blocking of commands for ODT - 00 - Block read/write ... */
#define DWC_DDR_UMCTL2_REGS_ODTCFG_WR_ODT_BLOCK_MASK 0x00000003
#define DWC_DDR_UMCTL2_REGS_ODTCFG_WR_ODT_BLOCK_SHIFT 0

/*  The delay, in clock cycles, from issuing a read command to  ... */
#define DWC_DDR_UMCTL2_REGS_ODTCFG_RD_ODT_DELAY_MASK 0x0000003C
#define DWC_DDR_UMCTL2_REGS_ODTCFG_RD_ODT_DELAY_SHIFT 2

#define DWC_DDR_UMCTL2_REGS_ODTCFG_RESERVED_MASK 0x000000C0
#define DWC_DDR_UMCTL2_REGS_ODTCFG_RESERVED_SHIFT 6

/* Cycles to hold ODT for a read command */
#define DWC_DDR_UMCTL2_REGS_ODTCFG_RD_ODT_HOLD_MASK 0x00000F00
#define DWC_DDR_UMCTL2_REGS_ODTCFG_RD_ODT_HOLD_SHIFT 8

/*  The delay, in clock cycles, from issuing a write command to  ... */
#define DWC_DDR_UMCTL2_REGS_ODTCFG_WR_ODT_DELAY_MASK 0x000F0000
#define DWC_DDR_UMCTL2_REGS_ODTCFG_WR_ODT_DELAY_SHIFT 16

#define DWC_DDR_UMCTL2_REGS_ODTCFG_RESERVED1_MASK 0x00F00000
#define DWC_DDR_UMCTL2_REGS_ODTCFG_RESERVED1_SHIFT 20
/* Cycles to hold ODT for a write command */
#define DWC_DDR_UMCTL2_REGS_ODTCFG_WR_ODT_HOLD_MASK 0x0F000000
#define DWC_DDR_UMCTL2_REGS_ODTCFG_WR_ODT_HOLD_SHIFT 24

#define DWC_DDR_UMCTL2_REGS_ODTCFG_RESERVED2_MASK 0xF0000000
#define DWC_DDR_UMCTL2_REGS_ODTCFG_RESERVED2_SHIFT 28

/**** ODTMAP register ****/
/* Indicates which remote ODTs must be turned on during a write  ... */
#define DWC_DDR_UMCTL2_REGS_ODTMAP_RANK0_WR_ODT_MASK 0x0000000F
#define DWC_DDR_UMCTL2_REGS_ODTMAP_RANK0_WR_ODT_SHIFT 0

/* Indicates which remote ODTs must be turned on during a read  ... */
#define DWC_DDR_UMCTL2_REGS_ODTMAP_RANK0_RD_ODT_MASK 0x000000F0
#define DWC_DDR_UMCTL2_REGS_ODTMAP_RANK0_RD_ODT_SHIFT 4

/* Indicates which remote ODTs must be turned on during a write  ... */
#define DWC_DDR_UMCTL2_REGS_ODTMAP_RANK1_WR_ODT_MASK 0x00000F00
#define DWC_DDR_UMCTL2_REGS_ODTMAP_RANK1_WR_ODT_SHIFT 8

/* Indicates which remote ODTs must be turned on during a read  ... */
#define DWC_DDR_UMCTL2_REGS_ODTMAP_RANK1_RD_ODT_MASK 0x0000F000
#define DWC_DDR_UMCTL2_REGS_ODTMAP_RANK1_RD_ODT_SHIFT 12

/* Indicates which remote ODTs must be turned on during a write  ... */
#define DWC_DDR_UMCTL2_REGS_ODTMAP_RANK2_WR_ODT_MASK 0x000F0000
#define DWC_DDR_UMCTL2_REGS_ODTMAP_RANK2_WR_ODT_SHIFT 16

/* Indicates which remote ODTs must be turned on during a read  ... */
#define DWC_DDR_UMCTL2_REGS_ODTMAP_RANK2_RD_ODT_MASK 0x00F00000
#define DWC_DDR_UMCTL2_REGS_ODTMAP_RANK2_RD_ODT_SHIFT 20

/* Indicates which remote ODTs must be turned on during a write  ... */
#define DWC_DDR_UMCTL2_REGS_ODTMAP_RANK3_WR_ODT_MASK 0x0F000000
#define DWC_DDR_UMCTL2_REGS_ODTMAP_RANK3_WR_ODT_SHIFT 24

/* Indicates which remote ODTs must be turned on during a read  ... */
#define DWC_DDR_UMCTL2_REGS_ODTMAP_RANK3_RD_ODT_MASK 0xF0000000
#define DWC_DDR_UMCTL2_REGS_ODTMAP_RANK3_RD_ODT_SHIFT 28

/**** SCHED register ****/
/* Active low signal */
#define DWC_DDR_UMCTL2_REGS_SCHED_FORCE_LOW_PRI_N (1 << 0)
/* If set then the bank selector prefers writes over reads */
#define DWC_DDR_UMCTL2_REGS_SCHED_PREFER_WRITE (1 << 1)
/* If true, bank is closed until transactions are available for  ... */
#define DWC_DDR_UMCTL2_REGS_SCHED_PAGECLOSE (1 << 2)

#define DWC_DDR_UMCTL2_REGS_SCHED_RESERVED_MASK 0x000000F8
#define DWC_DDR_UMCTL2_REGS_SCHED_RESERVED_SHIFT 3
/* Number of entries in the low priority transaction store is th ... */
#define DWC_DDR_UMCTL2_REGS_SCHED_LPR_NUM_ENTRIES_MASK 0x00001F00
#define DWC_DDR_UMCTL2_REGS_SCHED_LPR_NUM_ENTRIES_SHIFT 8

#define DWC_DDR_UMCTL2_REGS_SCHED_RESERVED1_MASK 0x0000E000
#define DWC_DDR_UMCTL2_REGS_SCHED_RESERVED1_SHIFT 13
/* Describes the number of cycles that co_gs_go2critical_rd or c ... */
#define DWC_DDR_UMCTL2_REGS_SCHED_GO2CRITICAL_HYSTERESIS_MASK 0x00FF0000
#define DWC_DDR_UMCTL2_REGS_SCHED_GO2CRITICAL_HYSTERESIS_SHIFT 16
/* When the preferred transaction store is empty for these many  ... */
#define DWC_DDR_UMCTL2_REGS_SCHED_RDWR_IDLE_GAP_MASK 0x7F000000
#define DWC_DDR_UMCTL2_REGS_SCHED_RDWR_IDLE_GAP_SHIFT 24

#define DWC_DDR_UMCTL2_REGS_SCHED_RESERVED2 (1 << 31)

/**** PERFHPR0 register ****/
/* Number of clocks that the HPR queue is guaranteed to stay in  ... */
#define DWC_DDR_UMCTL2_REGS_PERFHPR0_HPR_MIN_NON_CRITICAL_MASK 0x0000FFFF
#define DWC_DDR_UMCTL2_REGS_PERFHPR0_HPR_MIN_NON_CRITICAL_SHIFT 0

#define DWC_DDR_UMCTL2_REGS_PERFHPR0_RESERVED_MASK 0xFFFF0000
#define DWC_DDR_UMCTL2_REGS_PERFHPR0_RESERVED_SHIFT 16

/**** PERFHPR1 register ****/
/* Number of clocks that the HPR queue can be starved before it  ... */
#define DWC_DDR_UMCTL2_REGS_PERFHPR1_HPR_MAX_STARVE_MASK 0x0000FFFF
#define DWC_DDR_UMCTL2_REGS_PERFHPR1_HPR_MAX_STARVE_SHIFT 0

#define DWC_DDR_UMCTL2_REGS_PERFHPR1_RESERVED_MASK 0x00FF0000
#define DWC_DDR_UMCTL2_REGS_PERFHPR1_RESERVED_SHIFT 16
/* Number of transactions that are serviced once the HPR queue g ... */
#define DWC_DDR_UMCTL2_REGS_PERFHPR1_HPR_XACT_RUN_LENGTH_MASK 0xFF000000
#define DWC_DDR_UMCTL2_REGS_PERFHPR1_HPR_XACT_RUN_LENGTH_SHIFT 24

/**** PERFLPR0 register ****/
/* Number of clocks that the LPR queue is guaranteed to be non-c ... */
#define DWC_DDR_UMCTL2_REGS_PERFLPR0_LPR_MIN_NON_CRITICAL_MASK 0x0000FFFF
#define DWC_DDR_UMCTL2_REGS_PERFLPR0_LPR_MIN_NON_CRITICAL_SHIFT 0

#define DWC_DDR_UMCTL2_REGS_PERFLPR0_RESERVED_MASK 0xFFFF0000
#define DWC_DDR_UMCTL2_REGS_PERFLPR0_RESERVED_SHIFT 16

/**** PERFLPR1 register ****/
/* Number of clocks that the LPR queue can be starved before it  ... */
#define DWC_DDR_UMCTL2_REGS_PERFLPR1_LPR_MAX_STARVE_MASK 0x0000FFFF
#define DWC_DDR_UMCTL2_REGS_PERFLPR1_LPR_MAX_STARVE_SHIFT 0

#define DWC_DDR_UMCTL2_REGS_PERFLPR1_RESERVED_MASK 0x00FF0000
#define DWC_DDR_UMCTL2_REGS_PERFLPR1_RESERVED_SHIFT 16
/* Number of transactions that are serviced once the LPR queue g ... */
#define DWC_DDR_UMCTL2_REGS_PERFLPR1_LPR_XACT_RUN_LENGTH_MASK 0xFF000000
#define DWC_DDR_UMCTL2_REGS_PERFLPR1_LPR_XACT_RUN_LENGTH_SHIFT 24

/**** PERFWR0 register ****/
/* Number of clocks that the write queue is guaranteed to be non ... */
#define DWC_DDR_UMCTL2_REGS_PERFWR0_W_MIN_NON_CRITICAL_MASK 0x0000FFFF
#define DWC_DDR_UMCTL2_REGS_PERFWR0_W_MIN_NON_CRITICAL_SHIFT 0

#define DWC_DDR_UMCTL2_REGS_PERFWR0_RESERVED_MASK 0xFFFF0000
#define DWC_DDR_UMCTL2_REGS_PERFWR0_RESERVED_SHIFT 16

/**** PERFWR1 register ****/
/* Number of clocks that the write queue can be starved before i ... */
#define DWC_DDR_UMCTL2_REGS_PERFWR1_W_MAX_STARVE_MASK 0x0000FFFF
#define DWC_DDR_UMCTL2_REGS_PERFWR1_W_MAX_STARVE_SHIFT 0

#define DWC_DDR_UMCTL2_REGS_PERFWR1_RESERVED_MASK 0x00FF0000
#define DWC_DDR_UMCTL2_REGS_PERFWR1_RESERVED_SHIFT 16
/* Number of transactions that are serviced once the WR queue go ... */
#define DWC_DDR_UMCTL2_REGS_PERFWR1_W_XACT_RUN_LENGTH_MASK 0xFF000000
#define DWC_DDR_UMCTL2_REGS_PERFWR1_W_XACT_RUN_LENGTH_SHIFT 24

/**** DBG0 register ****/
/* When 1, disable write combine.<br>FOR DEBUG ONLY */
#define DWC_DDR_UMCTL2_REGS_DBG0_DIS_WC  (1 << 0)
/* Only present in designs supporting read bypass */
#define DWC_DDR_UMCTL2_REGS_DBG0_DIS_RD_BYPASS (1 << 1)
/* Only present in designs supporting activate bypass */
#define DWC_DDR_UMCTL2_REGS_DBG0_DIS_ACT_BYPASS (1 << 2)

#define DWC_DDR_UMCTL2_REGS_DBG0_RESERVED (1 << 3)
/* When this is set to '0', auto-precharge is disabled for the f ... */
#define DWC_DDR_UMCTL2_REGS_DBG0_DIS_COLLISION_PAGE_OPT (1 << 4)

#define DWC_DDR_UMCTL2_REGS_DBG0_RESERVED1_MASK 0xFFFFFFE0
#define DWC_DDR_UMCTL2_REGS_DBG0_RESERVED1_SHIFT 5

/**** DBG1 register ****/
/* When 1, uMCTL2 will not de-queue any transactions from the CA ... */
#define DWC_DDR_UMCTL2_REGS_DBG1_DIS_DQ  (1 << 0)

#define DWC_DDR_UMCTL2_REGS_DBG1_RESERVED_MASK 0xFFFFFFFE
#define DWC_DDR_UMCTL2_REGS_DBG1_RESERVED_SHIFT 1

/**** DBGCAM register ****/
/* High priority read queue depth<br>FOR DEBUG ONLY */
#define DWC_DDR_UMCTL2_REGS_DBGCAM_DBG_HPR_Q_DEPTH_MASK 0x0000003F
#define DWC_DDR_UMCTL2_REGS_DBGCAM_DBG_HPR_Q_DEPTH_SHIFT 0

#define DWC_DDR_UMCTL2_REGS_DBGCAM_RESERVED_MASK 0x000000C0
#define DWC_DDR_UMCTL2_REGS_DBGCAM_RESERVED_SHIFT 6
/* Low priority read queue depth<br>FOR DEBUG ONLY */
#define DWC_DDR_UMCTL2_REGS_DBGCAM_DBG_LPR_Q_DEPTH_MASK 0x00003F00
#define DWC_DDR_UMCTL2_REGS_DBGCAM_DBG_LPR_Q_DEPTH_SHIFT 8

#define DWC_DDR_UMCTL2_REGS_DBGCAM_RESERVED1_MASK 0x0000C000
#define DWC_DDR_UMCTL2_REGS_DBGCAM_RESERVED1_SHIFT 14
/* Write queue depth<br>FOR DEBUG ONLY */
#define DWC_DDR_UMCTL2_REGS_DBGCAM_DBG_W_Q_DEPTH_MASK 0x003F0000
#define DWC_DDR_UMCTL2_REGS_DBGCAM_DBG_W_Q_DEPTH_SHIFT 16

#define DWC_DDR_UMCTL2_REGS_DBGCAM_RESERVED2_MASK 0x00C00000
#define DWC_DDR_UMCTL2_REGS_DBGCAM_RESERVED2_SHIFT 22
/* Stall<br>FOR DEBUG ONLY */
#define DWC_DDR_UMCTL2_REGS_DBGCAM_DBG_STALL (1 << 24)

#define DWC_DDR_UMCTL2_REGS_DBGCAM_RESERVED3_MASK 0xFE000000
#define DWC_DDR_UMCTL2_REGS_DBGCAM_RESERVED3_SHIFT 25

/**** PCCFG register ****/
/* If set to 1 (enabled), sets co_gs_go2critical_wr and co_gs_go ... */
#define DWC_DDR_UMCTL2_MP_PCCFG_GO2CRITICAL_EN (1 << 0)

#define DWC_DDR_UMCTL2_MP_PCCFG_RESERVED_MASK 0x0000000E
#define DWC_DDR_UMCTL2_MP_PCCFG_RESERVED_SHIFT 1
/* Page match four limit */
#define DWC_DDR_UMCTL2_MP_PCCFG_PAGEMATCH_LIMIT (1 << 4)

#define DWC_DDR_UMCTL2_MP_PCCFG_RESERVED1_MASK 0xFFFFFFE0
#define DWC_DDR_UMCTL2_MP_PCCFG_RESERVED1_SHIFT 5

/**** PCFGR_0 register ****/
/* Determines the initial load value of read aging counters */
#define DWC_DDR_UMCTL2_MP_PCFGR_0_RD_PORT_PRIORITY_MASK 0x000003FF
#define DWC_DDR_UMCTL2_MP_PCFGR_0_RD_PORT_PRIORITY_SHIFT 0

#define DWC_DDR_UMCTL2_MP_PCFGR_0_RESERVED (1 << 10)
/* If set to 1, read transactions with ID not covered by any of  ... */
#define DWC_DDR_UMCTL2_MP_PCFGR_0_READ_REORDER_BYPASS_EN (1 << 11)
/* If set to 1, enables aging function for the read channel of t ... */
#define DWC_DDR_UMCTL2_MP_PCFGR_0_RD_PORT_AGING_EN (1 << 12)
/* If set to 1, enables the AXI urgent sideband signal (arurgent ... */
#define DWC_DDR_UMCTL2_MP_PCFGR_0_RD_PORT_URGENT_EN (1 << 13)
/* If set to 1, enables the Page Match feature */
#define DWC_DDR_UMCTL2_MP_PCFGR_0_RD_PORT_PAGEMATCH_EN (1 << 14)
/* If set to 1, enables reads to be generated as "High Priority  ... */
#define DWC_DDR_UMCTL2_MP_PCFGR_0_RD_PORT_HPR_EN (1 << 15)

#define DWC_DDR_UMCTL2_MP_PCFGR_0_RESERVED1_MASK 0xFFFF0000
#define DWC_DDR_UMCTL2_MP_PCFGR_0_RESERVED1_SHIFT 16

/**** PCFGW_0 register ****/
/* Determines the initial load value of write aging counters */
#define DWC_DDR_UMCTL2_MP_PCFGW_0_WR_PORT_PRIORITY_MASK 0x000003FF
#define DWC_DDR_UMCTL2_MP_PCFGW_0_WR_PORT_PRIORITY_SHIFT 0

#define DWC_DDR_UMCTL2_MP_PCFGW_0_RESERVED_MASK 0x00000C00
#define DWC_DDR_UMCTL2_MP_PCFGW_0_RESERVED_SHIFT 10
/* If set to 1, enables aging function for the write channel of  ... */
#define DWC_DDR_UMCTL2_MP_PCFGW_0_WR_PORT_AGING_EN (1 << 12)
/* If set to 1, enables the AXI urgent sideband signal (awurgent ... */
#define DWC_DDR_UMCTL2_MP_PCFGW_0_WR_PORT_URGENT_EN (1 << 13)
/* If set to 1, enables the Page Match feature */
#define DWC_DDR_UMCTL2_MP_PCFGW_0_WR_PORT_PAGEMATCH_EN (1 << 14)

#define DWC_DDR_UMCTL2_MP_PCFGW_0_RESERVED1_MASK 0xFFFF8000
#define DWC_DDR_UMCTL2_MP_PCFGW_0_RESERVED1_SHIFT 15

/**** PCFGIDMASKCH0_0 register ****/
/* Determines the mask used in the ID mapping function for virtu ... */
#define DWC_DDR_UMCTL2_MP_PCFGIDMASKCH0_0_ID_MASK_MASK 0x003FFFFF
#define DWC_DDR_UMCTL2_MP_PCFGIDMASKCH0_0_ID_MASK_SHIFT 0

#define DWC_DDR_UMCTL2_MP_PCFGIDMASKCH0_0_RESERVED_MASK 0xFFC00000
#define DWC_DDR_UMCTL2_MP_PCFGIDMASKCH0_0_RESERVED_SHIFT 22

/**** PCFGIDVALUECH0_0 register ****/
/* Determines the value used in the ID mapping function for virt ... */
#define DWC_DDR_UMCTL2_MP_PCFGIDVALUECH0_0_ID_VALUE_MASK 0x003FFFFF
#define DWC_DDR_UMCTL2_MP_PCFGIDVALUECH0_0_ID_VALUE_SHIFT 0

#define DWC_DDR_UMCTL2_MP_PCFGIDVALUECH0_0_RESERVED_MASK 0xFFC00000
#define DWC_DDR_UMCTL2_MP_PCFGIDVALUECH0_0_RESERVED_SHIFT 22

#ifdef __cplusplus
}
#endif

#endif

/** @} end of DDR group */

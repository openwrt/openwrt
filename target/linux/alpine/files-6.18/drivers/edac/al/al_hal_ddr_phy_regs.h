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
 * @file   al_hal_ddr_phy_regs.h
 *
 * @brief  DDR PHY registers
 *
 */
#ifndef __AL_HAL_DDR_PHY_REGS_REGS_H__
#define __AL_HAL_DDR_PHY_REGS_REGS_H__

#include "al_hal_ddr_cfg.h"

#ifdef __cplusplus
extern "C" {
#endif

/* The index of the ECC byte lane */
#define AL_DDR_PHY_ECC_BYTE_LANE_INDEX		4

/* The clock frequency on which the PLL frequency select need to be changed */
#define AL_DDR_PHY_PLL_FREQ_SEL_MHZ		700

#define AL_DDR_PHY_NUM_ZQ_SEGMANTS		3

struct al_ddr_phy_zq_regs {
	uint32_t PR;		/* ZQ Impedance Control Program Register */
	uint32_t DR;		/* ZQ Impedance Control Data Register */
	uint32_t SR;		/* ZQ Impedance Control Status Register */
	uint32_t reserved;
};

struct al_ddr_phy_datx8_regs {
	uint32_t GCR[4];	/* General Configuration Registers 0-3 */
	uint32_t GSR[3];	/* General Status Registers 0-2 */
	uint32_t BDLR[7];	/* Bit Delay Line Registers 0-6 */
	uint32_t LCDLR[3];	/* Local Calibrated Delay Line Registers 0-2 */
	uint32_t MDLR;		/* Master Delay Line Register */
	uint32_t GTR;		/* General Timing Register */
	uint32_t reserved[13];
};

struct al_ddr_phy_regs {
	uint32_t RIDR;		/* Revision Identification Reg */
	uint32_t PIR;		/* PHY Initialization Reg */
	uint32_t PGCR[4];	/* PHY General Configuration Regs 0-3 */
	uint32_t PGSR[2];	/* PHY General Status Regs 0-1 */
	uint32_t PLLCR;		/* PLL Control Reg */
	uint32_t PTR[5];	/* PHY Timing Regs 0-4 */
	uint32_t ACMDLR;	/* AC Master Delay Line Reg */
	uint32_t ACLCDLR;	/* AC Local Calibrated Delay Line Reg */
	uint32_t ACBDLR[10];	/* AC Bit Delay Line Regs 0-9 */
	uint32_t ACIOCR[6];	/* AC I/O Configuration Regs 0-5 */
	uint32_t DXCCR;		/* DATX8 Common Configuration Reg */
	uint32_t DSGCR;		/* DDR System General Configuration Reg */
	uint32_t DCR;		/* DRAM Configuration Reg */
	uint32_t DTPR[4];	/* DRAM Timing Parameters Registesr 0-3 */
	uint32_t MR[4];		/* Mode Regs 0-3 */
	uint32_t ODTCR;		/* ODT Configuration Reg */
	uint32_t DTCR;		/* Data Training Configuration Reg */
	uint32_t DTAR[4];	/* Data Training Address Register 0-3 */
	uint32_t DTDR[2];	/* Data Training Data Register 0-1 */
	uint32_t DTEDR[2];	/* Data Training Eye Data Register 0-1 */
	uint32_t RDIMMGCR[2];	/* RDIMM General Configuration Register 0-1 */
	uint32_t RDIMMCR[2];	/* RDIMM Control Register 0-1 */
	uint32_t reserved1[0x3D - 0x39];
	uint32_t ODTCTLR;	/* ODT Control Reg */
	uint32_t reserved2[0x70 - 0x3E];
	uint32_t BISTRR;	/* BIST Run Register */
	uint32_t BISTWCR;	/* BIST Word Count Register */
	uint32_t BISTMSKR[3];	/* BIST Mask Register 0-2 */
	uint32_t BISTLSR;	/* BIST LFSR Seed Register */
	uint32_t BISTAR[3];	/* BIST Address Register 0-2 */
	uint32_t BISTUDPR;	/* BIST User Data Pattern Register */
	uint32_t BISTGSR;	/* BIST General Status Register */
	uint32_t BISTWER;	/* BIST Word Error Register */
	uint32_t BISTBER[4];	/* BIST Bit Error Register 0-3 */
	uint32_t BISTWCSR;	/* BIST Word Count Status Register */
	uint32_t BISTFWR[3];	/* BIST Fail Word Register 0-2 */
	uint32_t reserved3[0x8E - 0x84];
	uint32_t IOVCR[2];	/* IO VREF Control Register 0-1 */
	uint32_t ZQCR;		/* ZQ Impedance Control Register */
	struct al_ddr_phy_zq_regs ZQ[AL_DDR_PHY_NUM_ZQ_SEGMANTS];
	uint32_t reserved4[0xA0 - 0x9D];
	struct al_ddr_phy_datx8_regs DATX8[AL_DDR_PHY_NUM_BYTE_LANES];
};

/* Register PGSR0 field iDONE */
/**
 * Initialization Done: Indicates if set that the DDR system initialization has
 * completed. This bit is set after all the selected initialization routines in
 * PIR register have completed.
 */
#define DWC_DDR_PHY_REGS_PGSR0_IDONE			0x00000001

/* Register PGSR0 field ZCERR */
/**
 * Impedance Calibration Error: Indicates if set that there is an error in
 * impedance calibration.
 */
#define DWC_DDR_PHY_REGS_PGSR0_ZCERR			0x00100000

/* Register PGSR0 field WLERR */
/**
 * Write Leveling Error: Indicates if set that there is an error in write
 * leveling.
 */
#define DWC_DDR_PHY_REGS_PGSR0_WLERR			0x00200000

/* Register PGSR0 field QSGERR */
/**
 * DQS Gate Training Error: Indicates if set that there is an error in DQS gate
 * training.
 */
#define DWC_DDR_PHY_REGS_PGSR0_QSGERR			0x00400000

/* Register PGSR0 field WLAERR */
/**
 * Write Leveling Adjustment Error: Indicates if set that there is an error in
 * write leveling adjustment.
 */
#define DWC_DDR_PHY_REGS_PGSR0_WLAERR			0x00800000

/* Register PGSR0 field RDERR */
/**
 * Read Bit Deskew Error: Indicates if set that there is an error in read bit
 * deskew.
 */
#define DWC_DDR_PHY_REGS_PGSR0_RDERR			0x01000000

/* Register PGSR0 field WDERR */
/**
 * Write Bit Deskew Error: Indicates if set that there is an error in write bit
 * deskew.
 */
#define DWC_DDR_PHY_REGS_PGSR0_WDERR			0x02000000

/* Register PGSR0 field REERR */
/**
 * Read Eye Training Error: Indicates if set that there is an error in read eye
 * training.
 */
#define DWC_DDR_PHY_REGS_PGSR0_REERR			0x04000000

/* Register PGSR0 field WEERR */
/**
 * Write Eye Training Error: Indicates if set that there is an error in write
 * eye training.
 */
#define DWC_DDR_PHY_REGS_PGSR0_WEERR			0x08000000

/* Register PGSR0 field VTDONE */
/**
 * AC VT Done: Indicates if set that VT compensation calculation has
 * been completed for all enabled AC BDLs and LCDL.
 */
#define DWC_DDR_PHY_REGS_PGSR0_VTDONE			0x40000000

/* Register PGSR1 field VTSTOP */
/**
 * VT Stop: Indicates if set that the VT calculation logic has stopped
 * computing the next values for the VT compensated delay line values. After
 * assertion of the PGCR.INHVT, the VTSTOP bit should be read to ensure all VT
 * compensation logic has stopped computations before writing to the delay line
 * registers.
 */
#define DWC_DDR_PHY_REGS_PGSR1_VTSTOP			0x40000000

/* Register PGCR0 field PHYFRST */
/**
 * A write of ‘0’ to this bit resets the AC and DATX8 FIFOs without
 * resetting PUB RTL logic. This bit is not self-clearing and a ‘1’
 * must be written to deassert the reset.
 */
#define DWC_DDR_PHY_REGS_PGCR0_PHYFRST			0x04000000

/* Register PGCR1 field DLBYPMODE */
/**
 * Controls DDL Bypass Modes. Valid values are:
 * 00 = Normal dynamic control
 * 01 = All DDLs bypassed
 * 10 = No DDLs bypassed
 * 11 = Reserved
 */
#define DWC_DDR_PHY_REGS_PGCR1_DLBYPMODE_MASK		0x00000030
#define DWC_DDR_PHY_REGS_PGCR1_DLBYPMODE_SHIFT		4

#define DWC_DDR_PHY_REGS_PGCR1_DLBYPMODE_DYNAMIC	\
	(0 << DWC_DDR_PHY_REGS_PGCR1_DLBYPMODE_SHIFT)

#define DWC_DDR_PHY_REGS_PGCR1_DLBYPMODE_BYPASS	\
	(1 << DWC_DDR_PHY_REGS_PGCR1_DLBYPMODE_SHIFT)

#define DWC_DDR_PHY_REGS_PGCR1_DLBYPMODE_NO_BYPASS	\
	(2 << DWC_DDR_PHY_REGS_PGCR1_DLBYPMODE_SHIFT)

/* Register PGCR1 field IODDRM */
/**
 * I/O DDR Mode (D3F I/O Only): Selects the DDR mode for the I/Os.
 * These bits connect to bits [2:1] of the IOM pin of the SSTL I/O.
 * I/O Mode: I/O Mode select
 * 000 = DDR2 mode
 * 001 = LVCMOS mode
 * 010 = DDR3 mode
 * 011 = Reserved
 * 100 = DDR3L mode
 * 101 = Reserved
 * 110 = Reserved
 * 111 = Reserved
 */
#define DWC_DDR_PHY_REGS_PGCR1_IODDRM_MASK		AL_FIELD_MASK(8, 7)
#define DWC_DDR_PHY_REGS_PGCR1_IODDRM_SHIFT		7
#define DWC_DDR_PHY_REGS_PGCR1_IODDRM_VAL_DDR3		\
	(1 << DWC_DDR_PHY_REGS_PGCR1_IODDRM_SHIFT)

/* Register PGCR1 field PHYHRST */
/**
 * PHY High-Speed Reset: A write of ‘0’ to this bit resets the AC and DATX8
 * macros without resetting PUBm2 RTL logic. This bit is not self-clearing
 * and a ‘1’ must be written to de-assert the reset.
 */
#define DWC_DDR_PHY_REGS_PGCR1_PHYHRST			0x02000000

/* Register PGCR1 field INHVT */
/**
 * VT Calculation Inhibit: Inhibits calculation of the next VT compensated
 * delay line values. A value of 1 will initiate a stop of the VT compensation
 * logic. The bit PGSR1[30] (VSTOP) will be set to a logic 1 when VT
 * compensation has stopped. This bit should be set to 1 during writes to the
 * delay line registers. A value of 0 will re-enable the VT compensation
 * logic.
 */
#define DWC_DDR_PHY_REGS_PGCR1_INHVT			0x04000000

/* Register PGCR1 field IOLB */
/**
 * I/O Loop-Back Select: Selects where inside the I/O the loop-back of signals
 * happens. Valid values are:
 * 0 = Loopback is after output buffer; output enable must be asserted
 * 1 = Loopback is before output buffer; output enable is don’t care
 */
#define DWC_DDR_PHY_REGS_PGCR1_IOLB			0x08000000

/* Register PGCR3 field RDMODE */
/**
 */
#define DWC_DDR_PHY_REGS_PGCR3_RDMODE_MASK		0x00000018
#define DWC_DDR_PHY_REGS_PGCR3_RDMODE_SHIFT		3

#define DWC_DDR_PHY_REGS_PGCR3_RDMODE_STATIC_RD_RSP_OFF	\
	(0 << DWC_DDR_PHY_REGS_PGCR3_RDMODE_SHIFT)

#define DWC_DDR_PHY_REGS_PGCR3_RDMODE_STATIC_RD_RSP_CMP	\
	(2 << DWC_DDR_PHY_REGS_PGCR3_RDMODE_SHIFT)

#define DWC_DDR_PHY_REGS_PGCR3_RDMODE_STATIC_RD_RSP_PRG	\
	(3 << DWC_DDR_PHY_REGS_PGCR3_RDMODE_SHIFT)

/* Register PGCR3 field RDDLY */
/**
 */
#define DWC_DDR_PHY_REGS_PGCR3_RDDLY_MASK		0x000001e0
#define DWC_DDR_PHY_REGS_PGCR3_RDDLY_SHIFT		5

/* Register PGCR3 field GATEDXCTLCLK */
/**
Enable Clock Gating for DX ctl_clk: Enables, when set, clock gating for power
saving. Valid values are:
0 = Clock gating is disabled .
1 = Clock gating is enabled
 */
#define DWC_DDR_PHY_REGS_PGCR3_GATEDXCTLCLK			0x00002000

/* Register PGCR3 field GATEDXDDRCLK */
/**
Enable Clock Gating for DX ddr_clk: Enables, when set, clock gating for power
saving. Valid values are:
0 = Clock gating is disabled .
1 = Clock gating is enabled
 */
#define DWC_DDR_PHY_REGS_PGCR3_GATEDXDDRCLK			0x00004000

/* Register PGCR3 field GATEDXRDCLK */
/**
Enable Clock Gating for DX rd_clk: Enables, when set, clock gating for power
saving. Valid values are:
0 = Clock gating is disabled .
1 = Clock gating is enabled
 */
#define DWC_DDR_PHY_REGS_PGCR3_GATEDXRDCLK			0x00008000

/* Register PLLCR field FRQSEL */
/**
 * PLL Frequency Select: Selects the operating range of the PLL.
 * 00 = PLL reference clock (ctl_clk/REF_CLK) ranges from 335MHz to 533MHz
 * 01 = PLL reference clock (ctl_clk/REF_CLK) ranges from 225MHz to 385MHz
 * 10 = Reserved
 * 11 = PLL reference clock (ctl_clk/REF_CLK) ranges from 166MHz to 275MHz
 */
#define DWC_DDR_PHY_REGS_PLLCR_FRQSEL_MASK			0x00180000
#define DWC_DDR_PHY_REGS_PLLCR_FRQSEL_SHIFT			19
#define DWC_DDR_PHY_REGS_PLLCR_FRQSEL_335MHZ_533MHz		\
	(0x0 << DWC_DDR_PHY_REGS_PLLCR_FRQSEL_SHIFT)
#define DWC_DDR_PHY_REGS_PLLCR_FRQSEL_225MHZ_385MHz		\
	(0x1 << DWC_DDR_PHY_REGS_PLLCR_FRQSEL_SHIFT)
#define DWC_DDR_PHY_REGS_PLLCR_FRQSEL_166MHZ_275MHz		\
	(0x3 << DWC_DDR_PHY_REGS_PLLCR_FRQSEL_SHIFT)

/* Register ACIOCR0 field ACPDR */
/**
 * AC Power Down Receiver: Powers down, when set, the input receiver on the I/O for
 * RAS#, CAS#, WE#, BA[2:0], and A[15:0] pins.
 */
#define DWC_DDR_PHY_REGS_ACIOCR0_ACPDR			0x00000010

/* Register ACIOCR0 field CKPDR */
/**
 * CK Power Down Receiver: Powers down, when set, the input receiver on the I/O for
 * CK[0], CK[1], CK[2], and CK[3] pins, respectively.
 */
#define DWC_DDR_PHY_REGS_ACIOCR0_CKPDR_MASK			0x00003c00
#define DWC_DDR_PHY_REGS_ACIOCR0_CKPDR_SHIFT			10

/* Register ACIOCR0 field RANKPDR */
/**
 * Rank Power Down Receiver: Powers down, when set, the input receiver on the I/O
 * CKE[3:0], ODT[3:0], and CS#[3:0] pins. RANKPDR[0] controls the power down for
 * CKE[0], ODT[0], and CS#[0], RANKPDR[1] controls the power down for CKE[1],
 * ODT[1], and CS#[1], and so on.
 */
#define DWC_DDR_PHY_REGS_ACIOCR0_RANKPDR_MASK			0x03c00000
#define DWC_DDR_PHY_REGS_ACIOCR0_RANKPDR_SHIFT			22

/* Register ACIOCR0 field RSTPDR */
/**
 * SDRAM Reset Power Down Receiver: Powers down, when set, the input receiver
 * on the I/O for SDRAM RST# pin.
 */
#define DWC_DDR_PHY_REGS_ACIOCR0_RSTPDR			0x10000000

/* Register DSGCR field PUREN */
/**
 * PHY Update Request Enable: Specifies if set, that the PHY should issue
 * PHY-initiated update request when there is DDL VT drift.
 */
#define DWC_DDR_PHY_REGS_DSGCR_PUREN			0x00000001


/* Register DSGCR field DQSGX */
/**
 * DQSGX DQS Gate Extension: Specifies if set that the read DQS gating mustgate
 * will be extended. This should be set ONLY when used with DQS
 * pulldown and then re-centered, i.e. one clock cycle extension on either
 * side.DQSn pullup. Allowable settings are:
 * 00 = do not extend the gate
 * 01 = extend the gate by ½ tCK in both directions (but never earlier than
 * zero read latency)
 * 10 = extend the gate earlier by ½ tCK and later by 2 * tCK (to facilitate
 * LPDDR2/LPDDR3 usage without training for systems supporting upto
 * 800Mbps)
 * 11 = extend the gate earlier by ½ tCK and later by 3 * tCK (to facilitate
 * LPDDR2/LPDDR3 usage without training for systems supporting upto
 * 1600Mbps))
 */
#define DWC_DDR_PHY_REGS_DSGCR_DQSGX_MASK		0x000000c0
#define DWC_DDR_PHY_REGS_DSGCR_DQSGX_SHIFT		6

/* Register DSGCR field RRMODE */
/**
 * Rise-to-Rise Mode: Indicates if set that the PHY mission mode is configured
 * to run in rise-to-rise mode. Otherwise if not set the PHY mission mode is
 * running in rise-to-fall mode.
 */
#define DWC_DDR_PHY_REGS_DSGCR_RRMODE			0x00040000

/* Register DCR field NOSRA */
/**
 * No Simultaneous Rank Access: Specifies if set that simultaneous rank access
 * on the same clock cycle is not allowed. This means that multiple chip select
 * signals should not be asserted at the same time. This may be required on
 * some DIMM systems.
 */
#define DWC_DDR_PHY_REGS_DCR_NOSRA			0x08000000

/* Register DCR field DDR2T */
/**
 * DDR 2T Timing: Indicates if set that 2T timing should be used by PUBm2
 * internally generated SDRAM transactions.
 */
#define DWC_DDR_PHY_REGS_DCR_DDR2T			0x10000000

/* Register DCR field UDIMM */
/**
 * Un-buffered DIMM Address Mirroring: Indicates if set that there is address
 * mirroring on the second rank of an un-buffered DIMM (the rank connected to
 * CS#[1]). In this case, the PUBm2 re-scrambles the bank and address when
 * sending mode register commands to the second rank. This only applies to
 * PUBm2 internal SDRAM transactions. Transactions generated by the controller
 * must make its own adjustments when using an un-buffered DIMM. DCR[NOSRA]
 * must be set if address mirroring is enabled.
 */
#define DWC_DDR_PHY_REGS_DCR_UDIMM			0x20000000

/* Register DTPR0 field t_rtp */
/* Internal read to precharge command delay. Valid values are 2 to 15 */
#define DWC_DDR_PHY_REGS_DTPR0_T_RTP_MASK		0x0000000f
#define DWC_DDR_PHY_REGS_DTPR0_T_RTP_SHIFT		0

/* Register DTPR0 field t_wtr */
/* Internal write to read command delay. Valid values are 1 to 15 */
#define DWC_DDR_PHY_REGS_DTPR0_T_WTR_MASK		0x000000f0
#define DWC_DDR_PHY_REGS_DTPR0_T_WTR_SHIFT		4

/* Register DTPR0 field t_rp */
/* Precharge command period: The minimum time between a precharge command
and any other command. Note that the Controller automatically derives tRPA for
8-bank DDR2 devices by adding 1 to tRP. Valid values are 2 to 15.
In LPDDR3 mode, PUBm2 adds an offset of 8 to the register value, so valid range
is 8 to 2 */
#define DWC_DDR_PHY_REGS_DTPR0_T_RP_MASK		0x00000f00
#define DWC_DDR_PHY_REGS_DTPR0_T_RP_SHIFT		8

/* Register DTPR0 field t_rcd */
/* Activate to read or write delay. Minimum time from when an activate command
 * is issued to when a read or write to the activated row can be issued. Valid
 * values are 2 to 15. In LPDDR3 mode, PUBm2 adds an offset of 8 to the register
 * value, so valid range is 8 to 23.
 */
#define DWC_DDR_PHY_REGS_DTPR0_T_RCD_MASK		0x0000f000
#define DWC_DDR_PHY_REGS_DTPR0_T_RCD_SHIFT		12

/* Register DTPR0 field t_ras_min */
/* Activate to precharge command delay. Valid values are 2 to 63 */
#define DWC_DDR_PHY_REGS_DTPR0_T_RAS_MIN_MASK		0x003f0000
#define DWC_DDR_PHY_REGS_DTPR0_T_RAS_MIN_SHIFT		16

/* Register DTPR0 field t_rrd */
/* Activate to activate command delay (different banks). Valid values are 1 to
 * 15
 */
#define DWC_DDR_PHY_REGS_DTPR0_T_RRD_MASK		0x03c00000
#define DWC_DDR_PHY_REGS_DTPR0_T_RRD_SHIFT		22

/* Register DTPR0 field t_rc */
/* Activate to activate command delay (same bank). Valid values are 2 to 63 */
#define DWC_DDR_PHY_REGS_DTPR0_T_RC_MASK		0xfc000000
#define DWC_DDR_PHY_REGS_DTPR0_T_RC_SHIFT		26

/* Register DTPR1 field T_AOND */
/* Read ODT turn-on delay */
#define DWC_DDR_PHY_REGS_DTPR1_T_AOND_MASK		0xc0000000
#define DWC_DDR_PHY_REGS_DTPR1_T_AOND_SHIFT		30

/* Register DTPR3 field T_OFDX */
/* ODT turn-on length (read and write) */
#define DWC_DDR_PHY_REGS_DTPR3_T_OFDX_MASK		0xe0000000
#define DWC_DDR_PHY_REGS_DTPR3_T_OFDX_SHIFT		29

/* Register ODTCR field RDODT0 */
/**
 * Read ODT: Specifies whether ODT should be enabled (‘1’) or disabled (‘0’) on
 * each of the up to four ranks when a read command is sent to rank n. RDODT0,
 * RDODT1, RDODT2, and RDODT3 specify ODT settings when a read is to rank 0,
 * rank 1, rank 2, and rank 3, respectively. The four bits of each field each
 * represent a rank, the LSB being rank 0 and the MSB being rank 3.
 * Default is to disable ODT during reads.
*/
#define DWC_DDR_PHY_REGS_ODTCR_RDODT0_MASK		0x0000000F
#define DWC_DDR_PHY_REGS_ODTCR_RDODT0_SHIFT		0

#define DWC_DDR_PHY_REGS_ODTCR_RDODT1_MASK		0x000000F0
#define DWC_DDR_PHY_REGS_ODTCR_RDODT1_SHIFT		4

#define DWC_DDR_PHY_REGS_ODTCR_RDODT2_MASK		0x00000F00
#define DWC_DDR_PHY_REGS_ODTCR_RDODT2_SHIFT		8

#define DWC_DDR_PHY_REGS_ODTCR_RDODT3_MASK		0x0000F000
#define DWC_DDR_PHY_REGS_ODTCR_RDODT3_SHIFT		12

/* Register ODTCR field WRODT0 */
/**
 * Write ODT: Specifies whether ODT should be enabled (‘1’) or disabled (‘0’) on
 * each of the up to four ranks when a write command is sent to rank n. WRODT0,
 * WRODT1, WRODT2, and WRODT3 specify ODT settings when a write is to rank 0,
 * rank 1, rank 2, and rank 3, respectively. The four bits of each field each
 * represent a rank, the LSB being rank 0 and the MSB being rank 3.
 * Default is to enable ODT only on rank being written to.
*/
#define DWC_DDR_PHY_REGS_ODTCR_WRODT0_MASK		0x000F0000
#define DWC_DDR_PHY_REGS_ODTCR_WRODT0_SHIFT		16

#define DWC_DDR_PHY_REGS_ODTCR_WRODT1_MASK		0x00F00000
#define DWC_DDR_PHY_REGS_ODTCR_WRODT1_SHIFT		20

#define DWC_DDR_PHY_REGS_ODTCR_WRODT2_MASK		0x0F000000
#define DWC_DDR_PHY_REGS_ODTCR_WRODT2_SHIFT		24

#define DWC_DDR_PHY_REGS_ODTCR_WRODT3_MASK		0xF0000000
#define DWC_DDR_PHY_REGS_ODTCR_WRODT3_SHIFT		28

/* Register DTCR field DTRPTN */
/**
 * Data Training Repeat Number: Repeat number used to confirm stability of DDR
 * write or read
*/
#define DWC_DDR_PHY_REGS_DTCR_DTRPTN_MASK		0x0000000f
#define DWC_DDR_PHY_REGS_DTCR_DTRPTN_SHIFT		0

/* Register DTCR field DTRANK */
/**
 * Data Training Rank: Selects the SDRAM rank to be used during data bit deskew
 * and eye centering.
*/
#define DWC_DDR_PHY_REGS_DTCR_DTRANK_MASK		0x00000030
#define DWC_DDR_PHY_REGS_DTCR_DTRANK_SHIFT		4

/* Register DTCR field DTMPR */
/**
 * Data Training Using MPR (DDR3 Only): Specifies, if set, that DQS gate
 * training should use the SDRAM Multi-Purpose Register (MPR) register.
 * Otherwise datatraining is performed by first writing to some locations in
 * the SDRAM and then reading them back.
 */
#define DWC_DDR_PHY_REGS_DTCR_DTMPR			0x00000040

/* Register DTCR field DTDBS */
/**
 * Data Training Debug Byte Select: Selects the byte during data training debug
 * mode.
 */
#define DWC_DDR_PHY_REGS_DTCR_DTDBS_MASK		0x000f0000
#define DWC_DDR_PHY_REGS_DTCR_DTDBS_SHIFT		16
#define DWC_DDR_PHY_REGS_DTCR_DTDBS(i)			\
	((i) << DWC_DDR_PHY_REGS_DTCR_DTDBS_SHIFT)

/* Register DTCR field DTEXG */
/**
 * Data Training with Early/Extended Gate: Specifies if set that the DQS gate
 * training should be performed with an early/extended gate as specified in
 * DSGCR.DQSGX.
 */
#define DWC_DDR_PHY_REGS_DTCR_DTEXG			0x00800000

/* Register DTCR field RANKEN */
/**
 * Rank Enable: Specifies the ranks that are enabled for data-training. Bit 0
 * controls rank 0, bit 1 controls rank 1, bit 2 controls rank 2, and bit 3
 * controls rank 3. Setting the bit to '1' enables the rank, and setting it to
 * '0' disables the rank.
 */
#define DWC_DDR_PHY_REGS_DTCR_RANKEN_MASK		0x0f000000
#define DWC_DDR_PHY_REGS_DTCR_RANKEN_SHIFT		24

/* Register DTEDR1 field DTRLMN */
/* Data Training RDQS LCDL Minimum */
#define DWC_DDR_PHY_REGS_DTEDR1_DTRLMN_MASK		0x000000ff
#define DWC_DDR_PHY_REGS_DTEDR1_DTRLMN_SHIFT		0

/* Register DTEDR1 field DTRLMX */
/* Data Training RDQS LCDL Maximum */
#define DWC_DDR_PHY_REGS_DTEDR1_DTRLMX_MASK		0x0000ff00
#define DWC_DDR_PHY_REGS_DTEDR1_DTRLMX_SHIFT		8

/* Register DTEDR1 field DTRBMN */
/* Data Training Read BDL Shift Minimum */
#define DWC_DDR_PHY_REGS_DTEDR1_DTRBMN_MASK		0x00ff0000
#define DWC_DDR_PHY_REGS_DTEDR1_DTRBMN_SHIFT		16

/* Register DTEDR1 field DTRBMX */
/* Data Training Read BDL Shift Minimum */
#define DWC_DDR_PHY_REGS_DTEDR1_DTRBMX_MASK		0xff000000
#define DWC_DDR_PHY_REGS_DTEDR1_DTRBMX_SHIFT		24

/* Register RDIMMGCR0 field RDIMM */
/**
 * Registered DIMM: Indicates if set that a registered DIMM is used. In this
 * case, the PUBm2 increases the SDRAM write and read latencies (WL/RL) by 1
 * and also enforces that accesses adhere to RDIMM buffer chip. This only
 * applies to PUBm2 internal SDRAM transactions. Transactions generated by the
 * controller must make its own adjustments to WL/RL when using a registered
 * DIMM. The DCR.NOSRA register bit must be set to '1' if using the standard
 * RDIMM buffer chip so that normal DRAM accesses do not assert multiple chip
 * select bits at the same time.
 */
#define DWC_DDR_PHY_REGS_RDIMMGCR0_RDIMM		0x00000001

/* Register ODTCTLR field FRCEN */
/**
 * ODT force value enable : when this field is set, the ODT
 * value is taken from the FRCVAL field.
 * One bit for each rank.
 */
#define DWC_DDR_PHY_REGS_ODTCTLR_FRCEN_MASK		0x0000000f
#define DWC_DDR_PHY_REGS_ODTCTLR_FRCEN_SHIFT		0

/* Register ODTCTLR field FRCVAL */
/**
 * ODT force value : when FRCEN field is set, the ODT
 * value is taken from this field.
 * One bit for each rank.
 */
#define DWC_DDR_PHY_REGS_ODTCTLR_FRCVAL_MASK		0x000000f0
#define DWC_DDR_PHY_REGS_ODTCTLR_FRCVAL_SHIFT		4

/* Register BISTRR field BINST */
/**
Selects the BIST instruction to be executed: Valid values are:
000 = NOP: No operation
001 = Run: Triggers the running of the BIST.
010 = Stop: Stops the running of the BIST.
011 = Reset: Resets all BIST run-time registers, such as error counters.
100 – 111 Reserved
 */
#define DWC_DDR_PHY_REGS_BISTRR_BINST_MASK		0x00000007
#define DWC_DDR_PHY_REGS_BISTRR_BINST_SHIFT		0
#define DWC_DDR_PHY_REGS_BISTRR_BINST_NOP		\
	(0x0 << DWC_DDR_PHY_REGS_BISTRR_BINST_SHIFT)
#define DWC_DDR_PHY_REGS_BISTRR_BINST_RUN		\
	(0x1 << DWC_DDR_PHY_REGS_BISTRR_BINST_SHIFT)
#define DWC_DDR_PHY_REGS_BISTRR_BINST_STOP		\
	(0x2 << DWC_DDR_PHY_REGS_BISTRR_BINST_SHIFT)
#define DWC_DDR_PHY_REGS_BISTRR_BINST_RESET		\
	(0x3 << DWC_DDR_PHY_REGS_BISTRR_BINST_SHIFT)

/* Register BISTRR field BMODE */
/**
BIST Mode: Selects the mode in which BIST is run. Valid values are:
0 = Loopback mode: Address, commands and data loop back at the PHY I/Os.
1 = DRAM mode: Address, commands and data go to DRAM for normal memory
accesses.
*/
#define DWC_DDR_PHY_REGS_BISTRR_BMODE_MASK		0x00000008
#define DWC_DDR_PHY_REGS_BISTRR_BMODE_SHIFT		3
#define DWC_DDR_PHY_REGS_BISTRR_BMODE_LOOPBACK		\
	(0x0 << DWC_DDR_PHY_REGS_BISTRR_BMODE_SHIFT)
#define DWC_DDR_PHY_REGS_BISTRR_BMODE_DRAM		\
	(0x1 << DWC_DDR_PHY_REGS_BISTRR_BMODE_SHIFT)

/* Register BISTRR field BDXEN */
/**
 * DATX8 Enable: Enables the running of BIST on the data byte lane PHYs.
 * This bit is exclusive with BACEN, i.e. both cannot be set to '1' at the same
 * time.
 */
#define DWC_DDR_PHY_REGS_BISTRR_BDXEN			0x00004000

/* Register BISTRR field BACEN */
/**
 * BIST AC Enable: Enables the running of BIST on the address/command lane PHY.
 * This bit is exclusive with BDXEN, i.e. both cannot be set to '1' at the same
 * time.
 */
#define DWC_DDR_PHY_REGS_BISTRR_BACEN			0x00008000

/* Register BISTRR field BDPAT */
/**
BIST Data Pattern: Selects the data pattern used during BIST. Valid values are:
00 = Walking 0
01 = Walking 1
10 = LFSR-based pseudo-random
11 = User programmable (Not valid for AC loopback).
*/
#define DWC_DDR_PHY_REGS_BISTRR_BDPAT_MASK		0x00060000
#define DWC_DDR_PHY_REGS_BISTRR_BDPAT_SHIFT		17
#define DWC_DDR_PHY_REGS_BISTRR_BDPAT_WALKING_0		\
	(0x0 << DWC_DDR_PHY_REGS_BISTRR_BDPAT_SHIFT)
#define DWC_DDR_PHY_REGS_BISTRR_BDPAT_WALKING_1		\
	(0x1 << DWC_DDR_PHY_REGS_BISTRR_BDPAT_SHIFT)
#define DWC_DDR_PHY_REGS_BISTRR_BDPAT_LFSR		\
	(0x2 << DWC_DDR_PHY_REGS_BISTRR_BDPAT_SHIFT)
#define DWC_DDR_PHY_REGS_BISTRR_BDPAT_USER		\
	(0x3 << DWC_DDR_PHY_REGS_BISTRR_BDPAT_SHIFT)

/* Register BISTRR field BDXSEL */
/**
BIST DATX8 Select: Select the byte lane for comparison of loopback/read data.
Valid values are 0 to 8.
*/
#define DWC_DDR_PHY_REGS_BISTRR_BDXSEL_MASK		0x00780000
#define DWC_DDR_PHY_REGS_BISTRR_BDXSEL_SHIFT		19
#define DWC_DDR_PHY_REGS_BISTRR_BDXSEL(val)		\
	((val) << DWC_DDR_PHY_REGS_BISTRR_BDXSEL_SHIFT)

/* Register BISTWCR field BWCNT */
/**
BIST Word Count: Indicates the number of words to generate during BIST. This
must be a multiple of DRAM burst length (BL) divided by 2, e.g. for BL=8, valid
values are 4, 8, 12, 16, and so on.
*/
#define DWC_DDR_PHY_REGS_BISTWCR_BWCNT_MASK		0x0000ffff
#define DWC_DDR_PHY_REGS_BISTWCR_BWCNT_SHIFT		0
#define DWC_DDR_PHY_REGS_BISTWCR_BWCNT(cnt)	\
	((cnt) << DWC_DDR_PHY_REGS_BISTWCR_BWCNT_SHIFT)

/* Register BISTAR0 field BCOL */
/**
 * BIST Column Address: Selects the SDRAM column address to be used during
 * BIST. The lower bits of this address must be "0000" for BL16, "000" for BL8,
 * "00" for BL4 and "0" for BL2.
 */
#define DWC_DDR_PHY_REGS_BISTAR0_BCOL_MASK		0x00000fff
#define DWC_DDR_PHY_REGS_BISTAR0_BCOL_SHIFT		0
#define DWC_DDR_PHY_REGS_BISTAR0_BCOL(val)		\
	((val) << DWC_DDR_PHY_REGS_BISTAR0_BCOL_SHIFT)

/* Register BISTAR0 field BROW */
/**
BIST Row Address: Selects the SDRAM row address to be used during BIST
*/
#define DWC_DDR_PHY_REGS_BISTAR0_BROW_MASK		0x0ffff000
#define DWC_DDR_PHY_REGS_BISTAR0_BROW_SHIFT		12
#define DWC_DDR_PHY_REGS_BISTAR0_BROW(val)		\
	((val) << DWC_DDR_PHY_REGS_BISTAR0_BROW_SHIFT)

/* Register BISTAR0 field BBANK */
/**
BIST Bank Address: Selects the SDRAM bank address to be used during BIST.
*/
#define DWC_DDR_PHY_REGS_BISTAR0_BBANK_MASK		0x70000000
#define DWC_DDR_PHY_REGS_BISTAR0_BBANK_SHIFT		28
#define DWC_DDR_PHY_REGS_BISTAR0_BBANK(val)		\
	((val) << DWC_DDR_PHY_REGS_BISTAR0_BBANK_SHIFT)

/* Register BISTAR1 field BRANK */
/**
BIST Rank: Selects the SDRAM rank to be used during BIST. Valid values range
from 0 to maximum ranks minus 1.
*/
#define DWC_DDR_PHY_REGS_BISTAR1_BRANK_MASK		0x00000003
#define DWC_DDR_PHY_REGS_BISTAR1_BRANK_SHIFT		0
#define DWC_DDR_PHY_REGS_BISTAR1_BRANK(val)		\
	((val) << DWC_DDR_PHY_REGS_BISTAR1_BRANK_SHIFT)

/* Register BISTAR1 field BMRANK */
/**
BIST Maximum Rank: Specifies the maximum SDRAM rank to be used during BIST.
The default value is set to maximum ranks minus 1.
*/
#define DWC_DDR_PHY_REGS_BISTAR1_BMRANK_MASK		0x0000000c
#define DWC_DDR_PHY_REGS_BISTAR1_BMRANK_SHIFT		2
#define DWC_DDR_PHY_REGS_BISTAR1_BMRANK(val)		\
	((val) << DWC_DDR_PHY_REGS_BISTAR1_BMRANK_SHIFT)

/* Register BISTAR1 field BAINC */
/**
 * BIST Address Increment: Selects the value by which the SDRAM address is
 * incremented for each write/read access. This value must be at the beginning
 * of a burst boundary, i.e. the lower bits must be "0000" for BL16, "000" for
 * BL8, "00" for BL4 and "0" for BL2.
*/
#define DWC_DDR_PHY_REGS_BISTAR1_BAINC_MASK		0x0000fff0
#define DWC_DDR_PHY_REGS_BISTAR1_BAINC_SHIFT		4
#define DWC_DDR_PHY_REGS_BISTAR1_BAINC(val)		\
	((val) << DWC_DDR_PHY_REGS_BISTAR1_BAINC_SHIFT)

/* Register BISTAR2 field BMCOL */
/**
BIST Maximum Column Address: Specifies the maximum SDRAM column address
to be used during BIST before the address increments to the next row.
*/
#define DWC_DDR_PHY_REGS_BISTAR2_BMCOL_MASK		0x00000fff
#define DWC_DDR_PHY_REGS_BISTAR2_BMCOL_SHIFT		0
#define DWC_DDR_PHY_REGS_BISTAR2_BMCOL(val)		\
	((val) << DWC_DDR_PHY_REGS_BISTAR2_BMCOL_SHIFT)

/* Register BISTAR2 field BMROW */
/**
BIST Maximum Row Address: Specifies the maximum SDRAM row address to be
used during BIST before the address increments to the next bank.
*/
#define DWC_DDR_PHY_REGS_BISTAR2_BMROW_MASK		0x0ffff000
#define DWC_DDR_PHY_REGS_BISTAR2_BMROW_SHIFT		12
#define DWC_DDR_PHY_REGS_BISTAR2_BMROW(val)		\
	((val) << DWC_DDR_PHY_REGS_BISTAR2_BMROW_SHIFT)

/* Register BISTAR2 field BMBANK */
/**
BIST Maximum Bank Address: Specifies the maximum SDRAM bank address to be
used during BIST before the address increments to the next rank.
*/
#define DWC_DDR_PHY_REGS_BISTAR2_BMBANK_MASK		0x70000000
#define DWC_DDR_PHY_REGS_BISTAR2_BMBANK_SHIFT		28
#define DWC_DDR_PHY_REGS_BISTAR2_BMBANK(val)		\
	((val) << DWC_DDR_PHY_REGS_BISTAR2_BMBANK_SHIFT)

/* Register BISTUDPR field BUDP0 */
/**
 * BIST User Data Pattern 0: Data to be applied on even DQ pins during BIST.
 */
#define DWC_DDR_PHY_REGS_BISTUDPR_BUDP0_MASK		0x0000ffff
#define DWC_DDR_PHY_REGS_BISTUDPR_BUDP0_SHIFT		0
#define DWC_DDR_PHY_REGS_BISTUDPR_BUDP0(val)		\
	((val) << DWC_DDR_PHY_REGS_BISTUDPR_BUDP0_SHIFT)

/* Register BISTUDPR field BUDP1 */
/**
 * BIST User Data Pattern 1: Data to be applied on odd DQ pins during BIST.
 */
#define DWC_DDR_PHY_REGS_BISTUDPR_BUDP1_MASK		0xffff0000
#define DWC_DDR_PHY_REGS_BISTUDPR_BUDP1_SHIFT		16
#define DWC_DDR_PHY_REGS_BISTUDPR_BUDP1(val)		\
	((val) << DWC_DDR_PHY_REGS_BISTUDPR_BUDP1_SHIFT)

/* Register BISTGSR field BDONE */
/**
BIST Done: Indicates if set that the BIST has finished executing. This bit is reset to
zero when BIST is triggered.
*/
#define DWC_DDR_PHY_REGS_BISTGSR_BDONE			0x00000001

/* Register BISTGSR field BACERR */
/**
BIST Address/Command Error: indicates if set that there is a data comparison error
in the address/command lane.
*/
#define DWC_DDR_PHY_REGS_BISTGSR_BACERR			0x00000002

/* Register BISTGSR field BDXERR */
/**
BIST Data Error: indicates if set that there is a data comparison error in the byte
lane.
*/
#define DWC_DDR_PHY_REGS_BISTGSR_BDXERR			0x00000004

/* Register DXnGCR0 field DXEN */
/**
 * Data Byte Enable: Enables if set the data byte. Setting this bit to ‘0’
 * disables the byte, i.e. the byte is not used in PHY initialization or
 * training and is ignored during SDRAM read/write operations.
 */
#define DWC_DDR_PHY_REGS_DXNGCR0_DXEN			0x00000001

/* Register DXnGCR0 field DQSGOE */
/**
DQSG Output Enable: Enables, when set, the output driver (OE pin)on the I/O for
DQS gate.
 */
#define DWC_DDR_PHY_REGS_DXNGCR0_DQSGOE			0x00000004

/* Register DXnGCR0 field DQSGODT */
/**
DQSG On-Die Termination: Enables, when set, the on-die termination (TE pin)on
the I/O for DQS gate. Note that in typical usage, DQSGOE will always be on,
rendering this control bit meaningless.
 */
#define DWC_DDR_PHY_REGS_DXNGCR0_DQSGODT		0x00000008

/* Register DXnGCR0 field DQSGPDD */
/**
DQSG Power Down Driver: Powers down, if set, the output driver on the I/O for
DQS gate. This bit is ORed with the common PDD configuration bit.
 */
#define DWC_DDR_PHY_REGS_DXNGCR0_DQSGPDD		0x00000010

/* Register DXnGCR0 field DQSGPDR */
/**
DQSG Power Down Receiver: Powers down, if set, the input receiver on the I/O for
DQS gate. This bit is ORed with the common PDR configuration bit.
 */
#define DWC_DDR_PHY_REGS_DXNGCR0_DQSGPDR		0x00000020

/* Register DXnGCR0 field DQSRPD */
/**
DQSR Power Down: Powers down, if set, the PDQSR cell. This bit is ORed with the
common PDR configuration bit
 */
#define DWC_DDR_PHY_REGS_DXNGCR0_DQSRPD		0x00000040

/* Register DXnGCR0 field PLLPD */
/**
 * PLL Power Down: Puts the byte PLL in power down mode by driving the PLL
 * power down pin. This bit is not self-clearing and a '0' must be written to
 * de-assert the power-down. This bit is ORed with the global PLLPD
 * configuration bit
 */
#define DWC_DDR_PHY_REGS_DXNGCR0_PLLPD		0x00020000

/* Register DXnGCR0 field WLRKEN */
/* Write Level Rank Enable: Specifies the ranks that should be write leveled
 * for this byte. Write leveling responses from ranks that are not enabled for
 * write leveling for a particular byte are ignored and write leveling is
 * flagged as done for these ranks. WLRKEN[0] enables rank 0, [1] enables rank
 * 1, [2] enables rank 2, and [3] enables rank 3.
 */
#define DWC_DDR_PHY_REGS_DXNGCR0_WLRKEN_MASK		0x3c000000
#define DWC_DDR_PHY_REGS_DXNGCR0_WLRKEN_SHIFT		26

/* Register DXnGCR0 field MDLEN */
/**
Master Delay Line Enable: Enables, if set, the DATX8 master delay line calibration
to perform subsequent period measurements following the initial period
measurements that are performed after reset or on when calibration is manually
triggered. These additional measurements are accumulated and filtered as long as
this bit remains high. This bit is combined with the common DATX8 MDL enable bit
 */
#define DWC_DDR_PHY_REGS_DXNGCR0_MDLEN		0x40000000

/* Register DXnGCR0 field CALBYP */
/**
Calibration Bypass: Prevents, if set, period measurement calibration from
automatically triggering after PHY initialization.
 */
#define DWC_DDR_PHY_REGS_DXNGCR0_CALBYP		0x80000000

/* Register DXnGCR3 field DSPDRMODE */
/**
 * Enables the PDR mode values for DQS.
 * 00 : PDR Dynamic
 * 01 : PDR always ON
 * 10 : PDR always OFF
 * 11 : Reserved
 */
#define DWC_DDR_PHY_REGS_DXNGCR3_DSPDRMODE_MASK		0x0000000c
#define DWC_DDR_PHY_REGS_DXNGCR3_DSPDRMODE_SHIFT	2
#define DWC_DDR_PHY_REGS_DXNGCR3_DSPDRMODE_PDR_DYNAMIC		\
	(0x0 << DWC_DDR_PHY_REGS_DXNGCR3_DSPDRMODE_SHIFT)
#define DWC_DDR_PHY_REGS_DXNGCR3_DSPDRMODE_PDR_ALWAYS_ON	\
	(0x1 << DWC_DDR_PHY_REGS_DXNGCR3_DSPDRMODE_SHIFT)
#define DWC_DDR_PHY_REGS_DXNGCR3_DSPDRMODE_PDR_ALWAYS_OFF	\
	(0x2 << DWC_DDR_PHY_REGS_DXNGCR3_DSPDRMODE_SHIFT)

/* Register DXnLCDLR0 field R0WLD */
/** Rank 0 Write Leveling Delay: Rank 0 delay select for the write leveling
 * (WL) LCDL
 */
#define DWC_DDR_PHY_REGS_DXNLCDLR0_R0WLD_MASK		0x000000ff
#define DWC_DDR_PHY_REGS_DXNLCDLR0_R0WLD_SHIFT		0

/* Register DXnLCDLR0 field R1WLD */
/** Rank 0 Write Leveling Delay: Rank 1 delay select for the write leveling
 * (WL) LCDL
 */
#define DWC_DDR_PHY_REGS_DXNLCDLR0_R1WLD_MASK		0x0000ff00
#define DWC_DDR_PHY_REGS_DXNLCDLR0_R1WLD_SHIFT		8

/* Register DXnLCDLR0 field R2WLD */
/** Rank 0 Write Leveling Delay: Rank 2 delay select for the write leveling
 * (WL) LCDL
 */
#define DWC_DDR_PHY_REGS_DXNLCDLR0_R2WLD_MASK		0x00ff0000
#define DWC_DDR_PHY_REGS_DXNLCDLR0_R2WLD_SHIFT		16

/* Register DXnLCDLR0 field R3WLD */
/** Rank 0 Write Leveling Delay: Rank 3 delay select for the write leveling
 * (WL) LCDL
 */
#define DWC_DDR_PHY_REGS_DXNLCDLR0_R3WLD_MASK		0xff000000
#define DWC_DDR_PHY_REGS_DXNLCDLR0_R3WLD_SHIFT		24

/* Register DXnLCDLR1 field WDQD */
/* Write Data Delay: Delay select for the write data (WDQ) LCDL. */
#define DWC_DDR_PHY_REGS_DXNLCDLR1_WDQD_MASK		0x000000ff
#define DWC_DDR_PHY_REGS_DXNLCDLR1_WDQD_SHIFT		0

/* Register DXnLCDLR1 field RDQSD */
/* Read DQS Delay: Delay select for the read DQS (RDQS) LCDL. */
#define DWC_DDR_PHY_REGS_DXNLCDLR1_RDQSD_MASK		0x0000ff00
#define DWC_DDR_PHY_REGS_DXNLCDLR1_RDQSD_SHIFT		8

/* Register DXnLCDLR1 field RDQSND */
/* Read DQSN Delay: Delay select for the read DQSN (RDQS) LCDL. */
#define DWC_DDR_PHY_REGS_DXNLCDLR1_RDQSND_MASK		0x00ff0000
#define DWC_DDR_PHY_REGS_DXNLCDLR1_RDQSND_SHIFT		16

/* Register DXnLCDLR2 field R0DQSGD */
/** Rank 0 Read DQS Gating Delay: Rank 0 delay select for the read DQS gating
 * (DQSG) LCDL.
 */
#define DWC_DDR_PHY_REGS_DXNLCDLR2_R0DQSGD_MASK		0x000000ff
#define DWC_DDR_PHY_REGS_DXNLCDLR2_R0DQSGD_SHIFT	0

/* Register DXnLCDLR2 field R1DQSGD */
/** Rank 1 Read DQS Gating Delay: Rank 1 delay select for the read DQS gating
 * (DQSG) LCDL.
 */
#define DWC_DDR_PHY_REGS_DXNLCDLR2_R1DQSGD_MASK		0x0000ff00
#define DWC_DDR_PHY_REGS_DXNLCDLR2_R1DQSGD_SHIFT	8

/* Register DXnLCDLR2 field R2DQSGD */
/** Rank 2 Read DQS Gating Delay: Rank 2 delay select for the read DQS gating
 * (DQSG) LCDL.
 */
#define DWC_DDR_PHY_REGS_DXNLCDLR2_R2DQSGD_MASK		0x00ff0000
#define DWC_DDR_PHY_REGS_DXNLCDLR2_R2DQSGD_SHIFT	16

/* Register DXnLCDLR2 field R3DQSGD */
/** Rank 3 Read DQS Gating Delay: Rank 3 delay select for the read DQS gating
 * (DQSG) LCDL.
 */
#define DWC_DDR_PHY_REGS_DXNLCDLR2_R3DQSGD_MASK		0xff000000
#define DWC_DDR_PHY_REGS_DXNLCDLR2_R3DQSGD_SHIFT	24

/* Register DXnGTR field R0DGSL */
/** Rank n DQS Gating System Latency: This is used to increase the number of clock
 * cycles needed to expect valid DDR read data by up to seven extra clock cycles.
 * This is used to compensate for board delays and other system delays. Power-up
 * default is 000 (i.e. no extra clock cycles required). The SL fields are initially set by
 * the PUBm2 during automatic DQS data training but these values can be
 * overwritten by a direct write to this register. Every three bits of this register control
 * the latency of each of the (up to) four ranks. R0DGSL controls the latency of rank
 * 0, R1DGSL controls rank 1, and so on. Valid values are 0 to 7:
 */
#define DWC_DDR_PHY_REGS_DXNGTR_R0DGSL_MASK		0x00000007
#define DWC_DDR_PHY_REGS_DXNGTR_R0DGSL_SHIFT		0

/* Register DXnGTR field R1DGSL */
/** Rank n DQS Gating System Latency: This is used to increase the number of clock
 * cycles needed to expect valid DDR read data by up to seven extra clock cycles.
 * This is used to compensate for board delays and other system delays. Power-up
 * default is 000 (i.e. no extra clock cycles required). The SL fields are initially set by
 * the PUBm2 during automatic DQS data training but these values can be
 * overwritten by a direct write to this register. Every three bits of this register control
 * the latency of each of the (up to) four ranks. R0DGSL controls the latency of rank
 * 0, R1DGSL controls rank 1, and so on. Valid values are 0 to 7:
 */
#define DWC_DDR_PHY_REGS_DXNGTR_R1DGSL_MASK		0x00000038
#define DWC_DDR_PHY_REGS_DXNGTR_R1DGSL_SHIFT		3

/* Register DXnGTR field R2DGSL */
/** Rank n DQS Gating System Latency: This is used to increase the number of clock
 * cycles needed to expect valid DDR read data by up to seven extra clock cycles.
 * This is used to compensate for board delays and other system delays. Power-up
 * default is 000 (i.e. no extra clock cycles required). The SL fields are initially set by
 * the PUBm2 during automatic DQS data training but these values can be
 * overwritten by a direct write to this register. Every three bits of this register control
 * the latency of each of the (up to) four ranks. R0DGSL controls the latency of rank
 * 0, R1DGSL controls rank 1, and so on. Valid values are 0 to 7:
 */
#define DWC_DDR_PHY_REGS_DXNGTR_R2DGSL_MASK		0x000001c0
#define DWC_DDR_PHY_REGS_DXNGTR_R2DGSL_SHIFT		6

/* Register DXnGTR field R3DGSL */
/** Rank n DQS Gating System Latency: This is used to increase the number of clock
 * cycles needed to expect valid DDR read data by up to seven extra clock cycles.
 * This is used to compensate for board delays and other system delays. Power-up
 * default is 000 (i.e. no extra clock cycles required). The SL fields are initially set by
 * the PUBm2 during automatic DQS data training but these values can be
 * overwritten by a direct write to this register. Every three bits of this register control
 * the latency of each of the (up to) four ranks. R0DGSL controls the latency of rank
 * 0, R1DGSL controls rank 1, and so on. Valid values are 0 to 7:
 */
#define DWC_DDR_PHY_REGS_DXNGTR_R3DGSL_MASK		0x00000e00
#define DWC_DDR_PHY_REGS_DXNGTR_R3DGSL_SHIFT		9

/* Register DXnGTR field R0WLSL */
/** Rank n Write Leveling System Latency: This is used to adjust the write latency
 * after write leveling. Power-up default is 01 (i.e. no extra clock cycles required). The
 * SL fields are initially set by the PUBm2 during automatic write leveling but these
 * values can be overwritten by a direct write to this register. Every two bits of this
 * register control the latency of each of the (up to) four ranks. R0WLSL controls the
 * latency of rank 0, R1WLSL controls rank 1, and so on. Valid values:
 * 00 = Write latency = WL - 1
 * 01 = Write latency = WL
 * 10 = Write latency = WL + 1
 * 11 = Reserved
 */
#define DWC_DDR_PHY_REGS_DXNGTR_R0WLSL_MASK		0x00003000
#define DWC_DDR_PHY_REGS_DXNGTR_R0WLSL_SHIFT		12

/* Register DXnGTR field R1WLSL */
/** Rank n Write Leveling System Latency: This is used to adjust the write latency
 * after write leveling. Power-up default is 01 (i.e. no extra clock cycles required). The
 * SL fields are initially set by the PUBm2 during automatic write leveling but these
 * values can be overwritten by a direct write to this register. Every two bits of this
 * register control the latency of each of the (up to) four ranks. R0WLSL controls the
 * latency of rank 0, R1WLSL controls rank 1, and so on. Valid values:
 * 00 = Write latency = WL - 1
 * 01 = Write latency = WL
 * 10 = Write latency = WL + 1
 * 11 = Reserved
 */
#define DWC_DDR_PHY_REGS_DXNGTR_R1WLSL_MASK		0x0000c000
#define DWC_DDR_PHY_REGS_DXNGTR_R1WLSL_SHIFT		14

/* Register DXnGTR field R2WLSL */
/** Rank n Write Leveling System Latency: This is used to adjust the write latency
 * after write leveling. Power-up default is 01 (i.e. no extra clock cycles required). The
 * SL fields are initially set by the PUBm2 during automatic write leveling but these
 * values can be overwritten by a direct write to this register. Every two bits of this
 * register control the latency of each of the (up to) four ranks. R0WLSL controls the
 * latency of rank 0, R1WLSL controls rank 1, and so on. Valid values:
 * 00 = Write latency = WL - 1
 * 01 = Write latency = WL
 * 10 = Write latency = WL + 1
 * 11 = Reserved
 */
#define DWC_DDR_PHY_REGS_DXNGTR_R2WLSL_MASK		0x00030000
#define DWC_DDR_PHY_REGS_DXNGTR_R2WLSL_SHIFT		16

/* Register DXnGTR field R3WLSL */
/** Rank n Write Leveling System Latency: This is used to adjust the write latency
 * after write leveling. Power-up default is 01 (i.e. no extra clock cycles required). The
 * SL fields are initially set by the PUBm2 during automatic write leveling but these
 * values can be overwritten by a direct write to this register. Every two bits of this
 * register control the latency of each of the (up to) four ranks. R0WLSL controls the
 * latency of rank 0, R1WLSL controls rank 1, and so on. Valid values:
 * 00 = Write latency = WL - 1
 * 01 = Write latency = WL
 * 10 = Write latency = WL + 1
 * 11 = Reserved
 */
#define DWC_DDR_PHY_REGS_DXNGTR_R3WLSL_MASK		0x000c0000
#define DWC_DDR_PHY_REGS_DXNGTR_R3WLSL_SHIFT		18

#ifdef __cplusplus
}
#endif

#endif

/** @} end of DDR group */


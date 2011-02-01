/*
 *  Memory sub-system initialization code for Danube board.
 *  Andre Messerschmidt
 *  Copyright (c) 2005	Infineon Technologies AG
 *
 *  Based on Inca-IP code
 *  Copyright (c) 2003	Wolfgang Denk <wd@denx.de>
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */
/* History:
      peng liu May 25, 2006, for PLL setting after reset, 05252006
 */
#include <config.h>
#include <version.h>
#include <asm/regdef.h>

#if 0

#if defined(CONFIG_USE_DDR_RAM)

#if defined(CONFIG_USE_DDR_RAM_CFG_111M)
#include "ddr_settings_r111.h"
#define DDR111
#elif defined(CONFIG_USE_DDR_RAM_CFG_166M)
#include "ddr_settings_r166.h"
#define DDR166
#elif defined(CONFIG_USE_DDR_RAM_CFG_e111M)
#include "ddr_settings_e111.h"
#define DDR111
#elif defined(CONFIG_USE_DDR_RAM_CFG_e166M)
#include "ddr_settings_e166.h"
#define DDR166
#elif defined(CONFIG_USE_DDR_RAM_CFG_promos400)
#include "ddr_settings_PROMOSDDR400.h"
#define DDR166
#elif defined(CONFIG_USE_DDR_RAM_CFG_samsung166)
#include "ddr_settings_Samsung_166.h"
#define DDR166
#elif defined(CONFIG_USE_DDR_RAM_CFG_psc166)
#include "ddr_settings_psc_166.h"
#define DDR166
#else
#warning "missing definition for ddr_settings.h, use default!"
#include "ddr_settings.h"
#endif
#endif /* CONFIG_USE_DDR_RAM */

#else

#include "ddr_settings_psc_166.h"
#define DDR166

#endif

#if defined(CONFIG_USE_DDR_RAM) &&  !defined(MC_DC0_VALUE)
#error "missing include of ddr_settings.h"
#endif

#define EBU_MODUL_BASE		0xBE105300
#define EBU_CLC(value)		0x0000(value)
#define EBU_CON(value)		0x0010(value)
#define EBU_ADDSEL0(value)	0x0020(value)
#define EBU_ADDSEL1(value)	0x0024(value)
#define EBU_ADDSEL2(value)	0x0028(value)
#define EBU_ADDSEL3(value)	0x002C(value)
#define EBU_BUSCON0(value)	0x0060(value)
#define EBU_BUSCON1(value)	0x0064(value)
#define EBU_BUSCON2(value)	0x0068(value)
#define EBU_BUSCON3(value)	0x006C(value)

#define MC_MODUL_BASE		0xBF800000
#define MC_ERRCAUSE(value)	0x0010(value)
#define MC_ERRADDR(value)	0x0020(value)
#define MC_CON(value)		0x0060(value)

#define MC_SRAM_ENABLE		0x00000004
#define MC_SDRAM_ENABLE		0x00000002
#define MC_DDRRAM_ENABLE	0x00000001

#define MC_SDR_MODUL_BASE	0xBF800200
#define MC_IOGP(value)		0x0000(value)
#define MC_CTRLENA(value)	0x0010(value)
#define MC_MRSCODE(value)	0x0020(value)
#define MC_CFGDW(value)		0x0030(value)
#define MC_CFGPB0(value)	0x0040(value)
#define MC_LATENCY(value)	0x0080(value)
#define MC_TREFRESH(value)	0x0090(value)
#define MC_SELFRFSH(value)	0x00A0(value)

#define MC_DDR_MODUL_BASE	0xBF801000
#define MC_DC00(value)		0x0000(value)
#define MC_DC01(value)		0x0010(value)
#define MC_DC02(value)		0x0020(value)
#define MC_DC03(value)		0x0030(value)
#define MC_DC04(value)		0x0040(value)
#define MC_DC05(value)		0x0050(value)
#define MC_DC06(value)		0x0060(value)
#define MC_DC07(value)		0x0070(value)
#define MC_DC08(value)		0x0080(value)
#define MC_DC09(value)		0x0090(value)
#define MC_DC10(value)		0x00A0(value)
#define MC_DC11(value)		0x00B0(value)
#define MC_DC12(value)		0x00C0(value)
#define MC_DC13(value)		0x00D0(value)
#define MC_DC14(value)		0x00E0(value)
#define MC_DC15(value)		0x00F0(value)
#define MC_DC16(value)		0x0100(value)
#define MC_DC17(value)		0x0110(value)
#define MC_DC18(value)		0x0120(value)
#define MC_DC19(value)		0x0130(value)
#define MC_DC20(value)		0x0140(value)
#define MC_DC21(value)		0x0150(value)
#define MC_DC22(value)		0x0160(value)
#define MC_DC23(value)		0x0170(value)
#define MC_DC24(value)		0x0180(value)
#define MC_DC25(value)		0x0190(value)
#define MC_DC26(value)		0x01A0(value)
#define MC_DC27(value)		0x01B0(value)
#define MC_DC28(value)		0x01C0(value)
#define MC_DC29(value)		0x01D0(value)
#define MC_DC30(value)		0x01E0(value)
#define MC_DC31(value)		0x01F0(value)
#define MC_DC32(value)		0x0200(value)
#define MC_DC33(value)		0x0210(value)
#define MC_DC34(value)		0x0220(value)
#define MC_DC35(value)		0x0230(value)
#define MC_DC36(value)		0x0240(value)
#define MC_DC37(value)		0x0250(value)
#define MC_DC38(value)		0x0260(value)
#define MC_DC39(value)		0x0270(value)
#define MC_DC40(value)		0x0280(value)
#define MC_DC41(value)		0x0290(value)
#define MC_DC42(value)		0x02A0(value)
#define MC_DC43(value)		0x02B0(value)
#define MC_DC44(value)		0x02C0(value)
#define MC_DC45(value)		0x02D0(value)
#define MC_DC46(value)		0x02E0(value)

#define RCU_OFFSET  0xBF203000
#define RCU_RST_REQ      (RCU_OFFSET + 0x0010)
#define RCU_STS          (RCU_OFFSET + 0x0014)

#define CGU_OFFSET  0xBF103000
#define  PLL0_CFG     (CGU_OFFSET + 0x0004)
#define  PLL1_CFG     (CGU_OFFSET + 0x0008)
#define  PLL2_CFG     (CGU_OFFSET + 0x000C)
#define  CGU_SYS      (CGU_OFFSET + 0x0010)
#define  CGU_UPDATE   (CGU_OFFSET + 0x0014)
#define  IF_CLK       (CGU_OFFSET + 0x0018)
#define  CGU_SMD      (CGU_OFFSET + 0x0020)
#define  CGU_CT1SR    (CGU_OFFSET + 0x0028)
#define  CGU_CT2SR    (CGU_OFFSET + 0x002C)
#define  CGU_PCMCR    (CGU_OFFSET + 0x0030)
#define  PCI_CR_PCI   (CGU_OFFSET + 0x0034)
#define  CGU_OSC_CTRL (CGU_OFFSET + 0x001C)
#define  CGU_MIPS_PWR_DWN (CGU_OFFSET + 0x0038)
#define  CLK_MEASURE  (CGU_OFFSET + 0x003C)

//05252006
#define  pll0_35MHz_CONFIG 0x9D861059
#define  pll1_35MHz_CONFIG 0x1A260CD9
#define  pll2_35MHz_CONFIG 0x8000f1e5
#define  pll0_36MHz_CONFIG 0x1000125D
#define  pll1_36MHz_CONFIG 0x1B1E0C99
#define  pll2_36MHz_CONFIG 0x8002f2a1
//05252006

//06063001-joelin disable the PCI CFRAME mask -start
/*CFRAME is an I/O signal, in the chip, the output CFRAME is selected via GPIO altsel pins, so if you select MII1 RXD1, the CFRAME will not come out.
But the CFRAME input still take the signal from the pad and not disabled when altsel choose other function. So when MII1_RXD1 is low from other device, the EBU interface will be disabled.

The chip function in such a way that disable the CFRAME mask mean EBU not longer check CFRAME to be the device using the bus.
The side effect is the entire PCI block will see CFRAME low all the time meaning PCI cannot use the bus at all so no more PCI function.
*/
#define PCI_CR_PR_OFFSET  0xBE105400
#define PCI_CR_PCI_MOD_REG          (PCI_CR_PR_OFFSET + 0x0030)
#define PCI_CONFIG_SPACE  0xB7000000
#define CS_CFM		(PCI_CONFIG_SPACE + 0x6C)
//06063001-joelin disable the PCI CFRAME mask -end
	.set	noreorder


/*
 * void ebu_init(void)
 */
	.globl	ebu_init
	.ent	ebu_init
ebu_init:

#if defined(CONFIG_EBU_ADDSEL0) || defined(CONFIG_EBU_ADDSEL1) || \
	defined(CONFIG_EBU_ADDSEL2) || defined(CONFIG_EBU_ADDSEL3) || \
	defined(CONFIG_EBU_BUSCON0) || defined(CONFIG_EBU_BUSCON1) || \
	defined(CONFIG_EBU_BUSCON2) || defined(CONFIG_EBU_BUSCON3)

	li	t1, EBU_MODUL_BASE
#if defined(CONFIG_EBU_ADDSEL0)
	li	t2, CONFIG_EBU_ADDSEL0
	sw	t2, EBU_ADDSEL0(t1)
#endif
#if defined(CONFIG_EBU_ADDSEL1)
	li	t2, CONFIG_EBU_ADDSEL1
	sw	t2, EBU_ADDSEL1(t1)
#endif
#if defined(CONFIG_EBU_ADDSEL2)
	li	t2, CONFIG_EBU_ADDSEL2
	sw	t2, EBU_ADDSEL2(t1)
#endif
#if defined(CONFIG_EBU_ADDSEL3)
	li	t2, CONFIG_EBU_ADDSEL3
	sw	t2, EBU_ADDSEL3(t1)
#endif

#if defined(CONFIG_EBU_BUSCON0)
	li	t2, CONFIG_EBU_BUSCON0
	sw	t2, EBU_BUSCON0(t1)
#endif
#if defined(CONFIG_EBU_BUSCON1)
	li	t2, CONFIG_EBU_BUSCON1
	sw	t2, EBU_BUSCON1(t1)
#endif
#if defined(CONFIG_EBU_BUSCON2)
	li	t2, CONFIG_EBU_BUSCON2
	sw	t2, EBU_BUSCON2(t1)
#endif
#if defined(CONFIG_EBU_BUSCON3)
	li	t2, CONFIG_EBU_BUSCON3
	sw	t2, EBU_BUSCON3(t1)
#endif

#endif

	j	ra
	nop

	.end	ebu_init


/*
 * void cgu_init(long)
 *
 * a0 has the clock value
 */
	.globl	cgu_init
	.ent	cgu_init
cgu_init:
	li  t2, CGU_SYS
	lw  t2,0(t2)
	beq t2,a0,freq_up2date
	nop

	li  t2, RCU_STS
	lw  t2, 0(t2)
	and t2,0x00020000
	beq t2,0x00020000,boot_36MHZ
	nop
//05252006
	li  t1, PLL0_CFG
	li  t2, pll0_35MHz_CONFIG
	sw	t2,0(t1)
	li  t1, PLL1_CFG
	li  t2, pll1_35MHz_CONFIG
	sw	t2,0(t1)
	li  t1, PLL2_CFG
	li  t2, pll2_35MHz_CONFIG
	sw	t2,0(t1)
	li  t1, CGU_SYS
	sw	a0,0(t1)
	li  t1, RCU_RST_REQ
	li  t2, 0x40000008
	sw	t2,0(t1)
	b   wait_reset
	nop
boot_36MHZ:
	li  t1, PLL0_CFG
	li  t2, pll0_36MHz_CONFIG
	sw	t2,0(t1)
	li  t1, PLL1_CFG
	li  t2, pll1_36MHz_CONFIG
	sw	t2,0(t1)
	li  t1, PLL2_CFG
	li  t2, pll2_36MHz_CONFIG
	sw	t2,0(t1)
	li  t1, CGU_SYS
	sw	a0,0(t1)
	li  t1, RCU_RST_REQ
	li  t2, 0x40000008
	sw	t2,0(t1)
//05252006

wait_reset:
	b   wait_reset
	nop
freq_up2date:
	j ra
	nop

	.end	cgu_init

#ifndef CONFIG_USE_DDR_RAM
/*
 * void sdram_init(long)
 *
 * a0 has the clock value
 */
	.globl	sdram_init
	.ent	sdram_init
sdram_init:

	/* SDRAM Initialization
	 */
	li	t1, MC_MODUL_BASE

	/* Clear Error log registers */
	sw	zero, MC_ERRCAUSE(t1)
	sw	zero, MC_ERRADDR(t1)

	/* Enable SDRAM module in memory controller */
	li	t3, MC_SDRAM_ENABLE
	lw	t2, MC_CON(t1)
	or	t3, t2, t3
	sw	t3, MC_CON(t1)

	li	t1, MC_SDR_MODUL_BASE

	/* disable the controller */
	li	t2, 0
	sw	t2, MC_CTRLENA(t1)

	li	t2, 0x822
	sw	t2, MC_IOGP(t1)

	li	t2, 0x2
	sw	t2, MC_CFGDW(t1)

	/* Set CAS Latency */
	li	t2, 0x00000020
	sw	t2, MC_MRSCODE(t1)

	/* Set CS0 to SDRAM parameters */
	li	t2, 0x000014d8
	sw	t2, MC_CFGPB0(t1)

	/* Set SDRAM latency parameters */
	li  	t2, 0x00036325;   /* BC PC100 */
	sw	t2, MC_LATENCY(t1)

	/* Set SDRAM refresh rate */
	li	t2, 0x00000C30
	sw	t2, MC_TREFRESH(t1)

	/* Clear Power-down registers */
	sw	zero, MC_SELFRFSH(t1)

	/* Finally enable the controller */
	li	t2, 1
	sw	t2, MC_CTRLENA(t1)

	j	ra
	nop

	.end	sdram_init

#endif /* !CONFIG_USE_DDR_RAM */

#ifdef CONFIG_USE_DDR_RAM
/*
 * void ddrram_init(long)
 *
 * a0 has the clock value
 */
	.globl	ddrram_init
	.ent	ddrram_init
ddrram_init:

	/* DDR-DRAM Initialization
	 */
	li	t1, MC_MODUL_BASE

	/* Clear Error log registers */
	sw	zero, MC_ERRCAUSE(t1)
	sw	zero, MC_ERRADDR(t1)

	/* Enable DDR module in memory controller */
	li	t3, MC_DDRRAM_ENABLE
	lw	t2, MC_CON(t1)
	or	t3, t2, t3
	sw	t3, MC_CON(t1)

	li	t1, MC_DDR_MODUL_BASE

	/* Write configuration to DDR controller registers */
	li	t2, MC_DC0_VALUE
	sw	t2, MC_DC00(t1)

	li	t2, MC_DC1_VALUE
	sw	t2, MC_DC01(t1)

	li	t2, MC_DC2_VALUE
	sw	t2, MC_DC02(t1)

	li	t2, MC_DC3_VALUE
	sw	t2, MC_DC03(t1)

	li	t2, MC_DC4_VALUE
	sw	t2, MC_DC04(t1)

	li	t2, MC_DC5_VALUE
	sw	t2, MC_DC05(t1)

	li	t2, MC_DC6_VALUE
	sw	t2, MC_DC06(t1)

	li	t2, MC_DC7_VALUE
	sw	t2, MC_DC07(t1)

	li	t2, MC_DC8_VALUE
	sw	t2, MC_DC08(t1)

	li	t2, MC_DC9_VALUE
	sw	t2, MC_DC09(t1)

	li	t2, MC_DC10_VALUE
	sw	t2, MC_DC10(t1)

	li	t2, MC_DC11_VALUE
	sw	t2, MC_DC11(t1)

	li	t2, MC_DC12_VALUE
	sw	t2, MC_DC12(t1)

	li	t2, MC_DC13_VALUE
	sw	t2, MC_DC13(t1)

	li	t2, MC_DC14_VALUE
	sw	t2, MC_DC14(t1)

	li	t2, MC_DC15_VALUE
	sw	t2, MC_DC15(t1)

	li	t2, MC_DC16_VALUE
	sw	t2, MC_DC16(t1)

	li	t2, MC_DC17_VALUE
	sw	t2, MC_DC17(t1)

	li	t2, MC_DC18_VALUE
	sw	t2, MC_DC18(t1)

	li	t2, MC_DC19_VALUE
	sw	t2, MC_DC19(t1)

	li	t2, MC_DC20_VALUE
	sw	t2, MC_DC20(t1)

	li	t2, MC_DC21_VALUE
	sw	t2, MC_DC21(t1)

	li	t2, MC_DC22_VALUE
	sw	t2, MC_DC22(t1)

	li	t2, MC_DC23_VALUE
	sw	t2, MC_DC23(t1)

	li	t2, MC_DC24_VALUE
	sw	t2, MC_DC24(t1)

	li	t2, MC_DC25_VALUE
	sw	t2, MC_DC25(t1)

	li	t2, MC_DC26_VALUE
	sw	t2, MC_DC26(t1)

	li	t2, MC_DC27_VALUE
	sw	t2, MC_DC27(t1)

	li	t2, MC_DC28_VALUE
	sw	t2, MC_DC28(t1)

	li	t2, MC_DC29_VALUE
	sw	t2, MC_DC29(t1)

	li	t2, MC_DC30_VALUE
	sw	t2, MC_DC30(t1)

	li	t2, MC_DC31_VALUE
	sw	t2, MC_DC31(t1)

	li	t2, MC_DC32_VALUE
	sw	t2, MC_DC32(t1)

	li	t2, MC_DC33_VALUE
	sw	t2, MC_DC33(t1)

	li	t2, MC_DC34_VALUE
	sw	t2, MC_DC34(t1)

	li	t2, MC_DC35_VALUE
	sw	t2, MC_DC35(t1)

	li	t2, MC_DC36_VALUE
	sw	t2, MC_DC36(t1)

	li	t2, MC_DC37_VALUE
	sw	t2, MC_DC37(t1)

	li	t2, MC_DC38_VALUE
	sw	t2, MC_DC38(t1)

	li	t2, MC_DC39_VALUE
	sw	t2, MC_DC39(t1)

	li	t2, MC_DC40_VALUE
	sw	t2, MC_DC40(t1)

	li	t2, MC_DC41_VALUE
	sw	t2, MC_DC41(t1)

	li	t2, MC_DC42_VALUE
	sw	t2, MC_DC42(t1)

	li	t2, MC_DC43_VALUE
	sw	t2, MC_DC43(t1)

	li	t2, MC_DC44_VALUE
	sw	t2, MC_DC44(t1)

	li	t2, MC_DC45_VALUE
	sw	t2, MC_DC45(t1)

	li	t2, MC_DC46_VALUE
	sw	t2, MC_DC46(t1)

	li	t2, 0x00000100
	sw	t2, MC_DC03(t1)

	j	ra
	nop

	.end	ddrram_init
#endif /* CONFIG_USE_DDR_RAM */

	.globl	lowlevel_init
	.ent	lowlevel_init
lowlevel_init:
	/* EBU, CGU and SDRAM/DDR-RAM Initialization.
	 */
	move	t0, ra
	/* We rely on the fact that non of the following ..._init() functions
	 * modify t0
	 */
#if defined(CONFIG_SYS_EBU_BOOT)
#if defined(DDR166)
	/* 0xe8 means CPU0/CPU1 333M, DDR 167M, FPI 83M, PPE 240M */
	li  a0,0xe8
#elif defined(DDR133)
	/* 0xe9 means CPU0/CPU1 333M, DDR 133M, FPI 83M, PPE 240M */
	li  a0,0xe9
#else /* defined(DDR111) */
	/* 0xea means CPU0/CPU1 333M, DDR 111M, FPI 83M, PPE 240M */
	li  a0,0xea
#endif
	bal	cgu_init
	nop
#endif /* CONFIG_SYS_EBU_BOOT */

	bal	ebu_init
	nop

//06063001-joelin disable the PCI CFRAME mask-start
#ifdef DISABLE_CFRAME
	li  t1, PCI_CR_PCI	//mw bf103034 80000000
	li  t2, 0x80000000
	sw	t2,0(t1)

	li  t1, PCI_CR_PCI_MOD_REG	//mw be105430 103
	li  t2, 0x103
	sw  t2,0(t1)

	li  t1, CS_CFM			//mw b700006c 0
	li  t2, 0x00
	sw  t2, 0(t1)

	li  t1, PCI_CR_PCI_MOD_REG	//mw be105430 103
	li  t2, 0x1000103
	sw  t2, 0(t1)
#endif
//06063001-joelin disable the PCI CFRAME mask-end

#ifdef CONFIG_SYS_EBU_BOOT
#ifndef CONFIG_SYS_RAMBOOT
#ifdef CONFIG_USE_DDR_RAM
	bal	ddrram_init
	nop
#else
	bal	sdram_init
	nop
#endif
#endif /* CONFIG_SYS_RAMBOOT */
#endif /* CONFIG_SYS_EBU_BOOT */

	move	ra, t0
	j	ra
	nop

	.end	lowlevel_init

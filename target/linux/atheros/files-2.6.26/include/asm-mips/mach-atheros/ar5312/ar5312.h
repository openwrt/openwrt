/*
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file "COPYING" in the main directory of this archive
 * for more details.
 *
 * Copyright (C) 2003 Atheros Communications, Inc.,  All Rights Reserved.
 * Copyright (C) 2006 Imre Kaloz <kaloz@openwrt.org>
 * Copyright (C) 2006 Felix Fietkau <nbd@openwrt.org>
 */

#ifndef AR5312_H
#define AR5312_H

#include <asm/addrspace.h>

/*
 * IRQs
 */

#define AR5312_IRQ_WLAN0_INTRS  MIPS_CPU_IRQ_BASE+2 /* C0_CAUSE: 0x0400 */
#define AR5312_IRQ_ENET0_INTRS  MIPS_CPU_IRQ_BASE+3 /* C0_CAUSE: 0x0800 */
#define AR5312_IRQ_ENET1_INTRS  MIPS_CPU_IRQ_BASE+4 /* C0_CAUSE: 0x1000 */
#define AR5312_IRQ_WLAN1_INTRS  MIPS_CPU_IRQ_BASE+5 /* C0_CAUSE: 0x2000 */
#define AR5312_IRQ_MISC_INTRS   MIPS_CPU_IRQ_BASE+6 /* C0_CAUSE: 0x4000 */


/* Address Map */
#define AR531X_WLAN0            0x18000000
#define AR531X_WLAN1            0x18500000
#define AR531X_ENET0            0x18100000
#define AR531X_ENET1            0x18200000
#define AR531X_SDRAMCTL         0x18300000
#define AR531X_FLASHCTL         0x18400000
#define AR531X_APBBASE		0x1c000000
#define AR531X_FLASH            0x1e000000
#define AR531X_UART0            0xbc000003      /* UART MMR */

/*
 * AR531X_NUM_ENET_MAC defines the number of ethernet MACs that
 * should be considered available.  The AR5312 supports 2 enet MACS,
 * even though many reference boards only actually use 1 of them
 * (i.e. Only MAC 0 is actually connected to an enet PHY or PHY switch.
 * The AR2312 supports 1 enet MAC.
 */
#define AR531X_NUM_ENET_MAC             2

/*
 * Need these defines to determine true number of ethernet MACs
 */
#define AR5212_AR5312_REV2      0x0052          /* AR5312 WMAC (AP31) */
#define AR5212_AR5312_REV7      0x0057          /* AR5312 WMAC (AP30-040) */
#define AR5212_AR2313_REV8      0x0058          /* AR2313 WMAC (AP43-030) */
#define AR531X_RADIO_MASK_OFF  0xc8
#define AR531X_RADIO0_MASK     0x0003
#define AR531X_RADIO1_MASK     0x000c
#define AR531X_RADIO1_S        2

/*
 * AR531X_NUM_WMAC defines the number of Wireless MACs that\
 * should be considered available.
 */
#define AR531X_NUM_WMAC                 2

/* Reset/Timer Block Address Map */
#define AR531X_RESETTMR		(AR531X_APBBASE  + 0x3000)
#define AR531X_TIMER		(AR531X_RESETTMR + 0x0000) /* countdown timer */
#define AR531X_WD_CTRL          (AR531X_RESETTMR + 0x0008) /* watchdog cntrl */
#define AR531X_WD_TIMER         (AR531X_RESETTMR + 0x000c) /* watchdog timer */
#define AR531X_ISR		(AR531X_RESETTMR + 0x0010) /* Intr Status Reg */
#define AR531X_IMR		(AR531X_RESETTMR + 0x0014) /* Intr Mask Reg */
#define AR531X_RESET		(AR531X_RESETTMR + 0x0020)
#define AR5312_CLOCKCTL1	(AR531X_RESETTMR + 0x0064)
#define AR5312_SCRATCH   	(AR531X_RESETTMR + 0x006c)
#define AR531X_PROCADDR		(AR531X_RESETTMR + 0x0070)
#define AR531X_PROC1		(AR531X_RESETTMR + 0x0074)
#define AR531X_DMAADDR		(AR531X_RESETTMR + 0x0078)
#define AR531X_DMA1		(AR531X_RESETTMR + 0x007c)
#define AR531X_ENABLE           (AR531X_RESETTMR + 0x0080) /* interface enb */
#define AR531X_REV		(AR531X_RESETTMR + 0x0090) /* revision */

/* AR531X_WD_CTRL register bit field definitions */
#define AR531X_WD_CTRL_IGNORE_EXPIRATION 0x0000
#define AR531X_WD_CTRL_NMI               0x0001
#define AR531X_WD_CTRL_RESET             0x0002

/* AR531X_ISR register bit field definitions */
#define AR531X_ISR_NONE		0x0000
#define AR531X_ISR_TIMER	0x0001
#define AR531X_ISR_AHBPROC	0x0002
#define AR531X_ISR_AHBDMA	0x0004
#define AR531X_ISR_GPIO		0x0008
#define AR531X_ISR_UART0	0x0010
#define AR531X_ISR_UART0DMA	0x0020
#define AR531X_ISR_WD		0x0040
#define AR531X_ISR_LOCAL	0x0080

/* AR531X_RESET register bit field definitions */
#define AR531X_RESET_SYSTEM     0x00000001  /* cold reset full system */
#define AR531X_RESET_PROC       0x00000002  /* cold reset MIPS core */
#define AR531X_RESET_WLAN0      0x00000004  /* cold reset WLAN MAC and BB */
#define AR531X_RESET_EPHY0      0x00000008  /* cold reset ENET0 phy */
#define AR531X_RESET_EPHY1      0x00000010  /* cold reset ENET1 phy */
#define AR531X_RESET_ENET0      0x00000020  /* cold reset ENET0 mac */
#define AR531X_RESET_ENET1      0x00000040  /* cold reset ENET1 mac */
#define AR531X_RESET_UART0      0x00000100  /* cold reset UART0 (high speed) */
#define AR531X_RESET_WLAN1      0x00000200  /* cold reset WLAN MAC/BB */
#define AR531X_RESET_APB        0x00000400  /* cold reset APB (ar5312) */
#define AR531X_RESET_WARM_PROC  0x00001000  /* warm reset MIPS core */
#define AR531X_RESET_WARM_WLAN0_MAC 0x00002000  /* warm reset WLAN0 MAC */
#define AR531X_RESET_WARM_WLAN0_BB  0x00004000  /* warm reset WLAN0 BaseBand */
#define AR531X_RESET_NMI        0x00010000  /* send an NMI to the processor */
#define AR531X_RESET_WARM_WLAN1_MAC 0x00020000  /* warm reset WLAN1 mac */
#define AR531X_RESET_WARM_WLAN1_BB  0x00040000  /* warm reset WLAN1 baseband */
#define AR531X_RESET_LOCAL_BUS  0x00080000  /* reset local bus */
#define AR531X_RESET_WDOG       0x00100000  /* last reset was a watchdog */

#define AR531X_RESET_WMAC0_BITS \
        AR531X_RESET_WLAN0 |\
        AR531X_RESET_WARM_WLAN0_MAC |\
        AR531X_RESET_WARM_WLAN0_BB

#define AR531X_RESERT_WMAC1_BITS \
        AR531X_RESET_WLAN1 |\
        AR531X_RESET_WARM_WLAN1_MAC |\
        AR531X_RESET_WARM_WLAN1_BB

/* AR5312_CLOCKCTL1 register bit field definitions */
#define AR5312_CLOCKCTL1_PREDIVIDE_MASK    0x00000030
#define AR5312_CLOCKCTL1_PREDIVIDE_SHIFT            4
#define AR5312_CLOCKCTL1_MULTIPLIER_MASK   0x00001f00
#define AR5312_CLOCKCTL1_MULTIPLIER_SHIFT           8
#define AR5312_CLOCKCTL1_DOUBLER_MASK      0x00010000

/* Valid for AR5312 and AR2312 */
#define AR5312_CLOCKCTL1_PREDIVIDE_MASK    0x00000030
#define AR5312_CLOCKCTL1_PREDIVIDE_SHIFT            4
#define AR5312_CLOCKCTL1_MULTIPLIER_MASK   0x00001f00
#define AR5312_CLOCKCTL1_MULTIPLIER_SHIFT           8
#define AR5312_CLOCKCTL1_DOUBLER_MASK      0x00010000

/* Valid for AR2313 */
#define AR2313_CLOCKCTL1_PREDIVIDE_MASK    0x00003000
#define AR2313_CLOCKCTL1_PREDIVIDE_SHIFT           12
#define AR2313_CLOCKCTL1_MULTIPLIER_MASK   0x001f0000
#define AR2313_CLOCKCTL1_MULTIPLIER_SHIFT          16
#define AR2313_CLOCKCTL1_DOUBLER_MASK      0x00000000


/* AR531X_ENABLE register bit field definitions */
#define AR531X_ENABLE_WLAN0              0x0001
#define AR531X_ENABLE_ENET0              0x0002
#define AR531X_ENABLE_ENET1              0x0004
#define AR531X_ENABLE_UART_AND_WLAN1_PIO 0x0008   /* UART, and WLAN1 PIOs */
#define AR531X_ENABLE_WLAN1_DMA          0x0010   /* WLAN1 DMAs */
#define AR531X_ENABLE_WLAN1 \
            (AR531X_ENABLE_UART_AND_WLAN1_PIO | AR531X_ENABLE_WLAN1_DMA)

/* AR531X_REV register bit field definitions */
#define AR531X_REV_WMAC_MAJ    0xf000
#define AR531X_REV_WMAC_MAJ_S  12
#define AR531X_REV_WMAC_MIN    0x0f00
#define AR531X_REV_WMAC_MIN_S  8
#define AR531X_REV_MAJ         0x00f0
#define AR531X_REV_MAJ_S       4
#define AR531X_REV_MIN         0x000f
#define AR531X_REV_MIN_S       0
#define AR531X_REV_CHIP        (AR531X_REV_MAJ|AR531X_REV_MIN)

/* Major revision numbers, bits 7..4 of Revision ID register */
#define AR531X_REV_MAJ_AR5312          0x4
#define AR531X_REV_MAJ_AR2313          0x5

/* Minor revision numbers, bits 3..0 of Revision ID register */
#define AR5312_REV_MIN_DUAL     0x0     /* Dual WLAN version */
#define AR5312_REV_MIN_SINGLE   0x1     /* Single WLAN version */

/* AR531X_FLASHCTL register bit field definitions */
#define FLASHCTL_IDCY   0x0000000f      /* Idle cycle turn around time */
#define FLASHCTL_IDCY_S 0
#define FLASHCTL_WST1   0x000003e0      /* Wait state 1 */
#define FLASHCTL_WST1_S 5
#define FLASHCTL_RBLE   0x00000400      /* Read byte lane enable */
#define FLASHCTL_WST2   0x0000f800      /* Wait state 2 */
#define FLASHCTL_WST2_S 11
#define FLASHCTL_AC     0x00070000      /* Flash address check (added) */
#define FLASHCTL_AC_S   16
#define FLASHCTL_AC_128K 0x00000000
#define FLASHCTL_AC_256K 0x00010000
#define FLASHCTL_AC_512K 0x00020000
#define FLASHCTL_AC_1M   0x00030000
#define FLASHCTL_AC_2M   0x00040000
#define FLASHCTL_AC_4M   0x00050000
#define FLASHCTL_AC_8M   0x00060000
#define FLASHCTL_AC_RES  0x00070000     /* 16MB is not supported */
#define FLASHCTL_E      0x00080000      /* Flash bank enable (added) */
#define FLASHCTL_BUSERR 0x01000000      /* Bus transfer error status flag */
#define FLASHCTL_WPERR  0x02000000      /* Write protect error status flag */
#define FLASHCTL_WP     0x04000000      /* Write protect */
#define FLASHCTL_BM     0x08000000      /* Burst mode */
#define FLASHCTL_MW     0x30000000      /* Memory width */
#define FLASHCTL_MWx8   0x00000000      /* Memory width x8 */
#define FLASHCTL_MWx16  0x10000000      /* Memory width x16 */
#define FLASHCTL_MWx32  0x20000000      /* Memory width x32 (not supported) */
#define FLASHCTL_ATNR   0x00000000      /* Access type == no retry */
#define FLASHCTL_ATR    0x80000000      /* Access type == retry every */
#define FLASHCTL_ATR4   0xc0000000      /* Access type == retry every 4 */

/* ARM Flash Controller -- 3 flash banks with either x8 or x16 devices.  */
#define AR531X_FLASHCTL0        (AR531X_FLASHCTL + 0x00)
#define AR531X_FLASHCTL1        (AR531X_FLASHCTL + 0x04)
#define AR531X_FLASHCTL2        (AR531X_FLASHCTL + 0x08)

/* ARM SDRAM Controller -- just enough to determine memory size */
#define AR531X_MEM_CFG1 (AR531X_SDRAMCTL + 0x04)
#define MEM_CFG1_AC0    0x00000700      /* bank 0: SDRAM addr check (added) */
#define MEM_CFG1_AC0_S  8
#define MEM_CFG1_AC1    0x00007000      /* bank 1: SDRAM addr check (added) */
#define MEM_CFG1_AC1_S  12

/* GPIO Address Map */
#define AR531X_GPIO         (AR531X_APBBASE  + 0x2000)
#define AR531X_GPIO_DO      (AR531X_GPIO + 0x00)        /* output register */
#define AR531X_GPIO_DI      (AR531X_GPIO + 0x04)        /* intput register */
#define AR531X_GPIO_CR      (AR531X_GPIO + 0x08)        /* control register */

/* GPIO Control Register bit field definitions */
#define AR531X_GPIO_CR_M(x)    (1 << (x))                      /* mask for i/o */
#define AR531X_GPIO_CR_O(x)    (0 << (x))                      /* mask for output */
#define AR531X_GPIO_CR_I(x)    (1 << (x))                      /* mask for input */
#define AR531X_GPIO_CR_INT(x)  (1 << ((x)+8))                  /* mask for interrupt */
#define AR531X_GPIO_CR_UART(x) (1 << ((x)+16))                 /* uart multiplex */

#endif


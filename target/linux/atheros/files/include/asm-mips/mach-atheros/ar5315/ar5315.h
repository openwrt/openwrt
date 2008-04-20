/*
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file "COPYING" in the main directory of this archive
 * for more details.
 *
 * Copyright (C) 2003 Atheros Communications, Inc.,  All Rights Reserved.
 * Copyright (C) 2006 FON Technology, SL.
 * Copyright (C) 2006 Imre Kaloz <kaloz@openwrt.org>
 * Copyright (C) 2006 Felix Fietkau <nbd@openwrt.org>
 */

#ifndef AR5315_H
#define AR5315_H

/*
 * IRQs
 */
#define AR5315_IRQ_MISC_INTRS   MIPS_CPU_IRQ_BASE+2 /* C0_CAUSE: 0x0400 */
#define AR5315_IRQ_WLAN0_INTRS  MIPS_CPU_IRQ_BASE+3 /* C0_CAUSE: 0x0800 */
#define AR5315_IRQ_ENET0_INTRS  MIPS_CPU_IRQ_BASE+4 /* C0_CAUSE: 0x1000 */
#define AR5315_IRQ_LCBUS_PCI    MIPS_CPU_IRQ_BASE+5 /* C0_CAUSE: 0x2000 */
#define AR5315_IRQ_WLAN0_POLL   MIPS_CPU_IRQ_BASE+6 /* C0_CAUSE: 0x4000 */


/*
 * Address map
 */
#define AR5315_SDRAM0           0x00000000      /* DRAM */
#define AR5315_SPI_READ         0x08000000      /* SPI FLASH */
#define AR5315_WLAN0            0xB0000000      /* Wireless MMR */
#define AR5315_PCI              0xB0100000      /* PCI MMR */
#define AR5315_SDRAMCTL         0xB0300000      /* SDRAM MMR */
#define AR5315_LOCAL            0xB0400000      /* LOCAL BUS MMR */
#define AR5315_ENET0            0xB0500000      /* ETHERNET MMR */
#define AR5315_DSLBASE          0xB1000000      /* RESET CONTROL MMR */
#define AR5315_UART0            0xB1100003      /* UART MMR */
#define AR5315_SPI              0xB1300000      /* SPI FLASH MMR */
#define AR5315_FLASHBT          0xBfc00000      /* ro boot alias to FLASH */
#define AR5315_RAM1             0x40000000      /* ram alias */
#define AR5315_PCIEXT           0x80000000      /* pci external */
#define AR5315_RAM2             0xc0000000      /* ram alias */
#define AR5315_RAM3             0xe0000000      /* ram alias */

/*
 * Reset Register
 */
#define AR5315_COLD_RESET       (AR5315_DSLBASE + 0x0000)

/* Cold Reset */
#define RESET_COLD_AHB              0x00000001
#define RESET_COLD_APB              0x00000002
#define RESET_COLD_CPU              0x00000004
#define RESET_COLD_CPUWARM          0x00000008
#define RESET_SYSTEM                (RESET_COLD_CPU | RESET_COLD_APB | RESET_COLD_AHB)      /* full system */

#define AR5317_RESET_SYSTEM	    0x00000010

/* Warm Reset */

#define AR5315_RESET            (AR5315_DSLBASE + 0x0004)

#define AR5315_RESET_WARM_WLAN0_MAC        0x00000001      /* warm reset WLAN0 MAC */
#define AR5315_RESET_WARM_WLAN0_BB         0x00000002      /* warm reset WLAN0 BaseBand */
#define AR5315_RESET_MPEGTS_RSVD           0x00000004      /* warm reset MPEG-TS */
#define AR5315_RESET_PCIDMA                0x00000008      /* warm reset PCI ahb/dma */
#define AR5315_RESET_MEMCTL                0x00000010      /* warm reset memory controller */
#define AR5315_RESET_LOCAL                 0x00000020      /* warm reset local bus */
#define AR5315_RESET_I2C_RSVD              0x00000040      /* warm reset I2C bus */
#define AR5315_RESET_SPI                   0x00000080      /* warm reset SPI interface */
#define AR5315_RESET_UART0                 0x00000100      /* warm reset UART0 */
#define AR5315_RESET_IR_RSVD               0x00000200      /* warm reset IR interface */
#define AR5315_RESET_EPHY0                 0x00000400      /* cold reset ENET0 phy */
#define AR5315_RESET_ENET0                 0x00000800      /* cold reset ENET0 mac */

/*
 * AHB master arbitration control
 */
#define AR5315_AHB_ARB_CTL      (AR5315_DSLBASE + 0x0008)

#define ARB_CPU                     0x00000001      /* CPU, default */
#define ARB_WLAN                    0x00000002      /* WLAN */
#define ARB_MPEGTS_RSVD             0x00000004      /* MPEG-TS */
#define ARB_LOCAL                   0x00000008      /* LOCAL */
#define ARB_PCI                     0x00000010      /* PCI */
#define ARB_ETHERNET                0x00000020      /* Ethernet */
#define ARB_RETRY                   0x00000100      /* retry policy, debug only */

/*
 * Config Register
 */
#define AR5315_ENDIAN_CTL       (AR5315_DSLBASE + 0x000c)

#define AR5315_CONFIG_AHB                  0x00000001      /* EC - AHB bridge endianess */
#define AR5315_CONFIG_WLAN                 0x00000002      /* WLAN byteswap */
#define AR5315_CONFIG_MPEGTS_RSVD          0x00000004      /* MPEG-TS byteswap */
#define AR5315_CONFIG_PCI                  0x00000008      /* PCI byteswap */
#define AR5315_CONFIG_MEMCTL               0x00000010      /* Memory controller endianess */
#define AR5315_CONFIG_LOCAL                0x00000020      /* Local bus byteswap */
#define AR5315_CONFIG_ETHERNET             0x00000040      /* Ethernet byteswap */

#define AR5315_CONFIG_MERGE                0x00000200      /* CPU write buffer merge */
#define AR5315_CONFIG_CPU                  0x00000400      /* CPU big endian */
#define AR5315_CONFIG_PCIAHB               0x00000800
#define AR5315_CONFIG_PCIAHB_BRIDGE        0x00001000
#define AR5315_CONFIG_SPI                  0x00008000      /* SPI byteswap */
#define AR5315_CONFIG_CPU_DRAM             0x00010000
#define AR5315_CONFIG_CPU_PCI              0x00020000
#define AR5315_CONFIG_CPU_MMR              0x00040000
#define AR5315_CONFIG_BIG                  0x00000400


/*
 * NMI control
 */
#define AR5315_NMI_CTL          (AR5315_DSLBASE + 0x0010)

#define NMI_EN  1

/*
 * Revision Register - Initial value is 0x3010 (WMAC 3.0, AR531X 1.0).
 */
#define AR5315_SREV             (AR5315_DSLBASE + 0x0014)

#define AR5315_REV_MAJ                     0x00f0
#define AR5315_REV_MAJ_S                   4
#define AR5315_REV_MIN                     0x000f
#define AR5315_REV_MIN_S                   0
#define AR5315_REV_CHIP                    (AR5315_REV_MAJ|AR5315_REV_MIN)

/*
 * Interface Enable
 */
#define AR5315_IF_CTL           (AR5315_DSLBASE + 0x0018)

#define IF_MASK                     0x00000007
#define IF_DISABLED                 0
#define IF_PCI                      1
#define IF_TS_LOCAL                 2
#define IF_ALL                      3   /* only for emulation with separate pins */
#define IF_LOCAL_HOST               0x00000008
#define IF_PCI_HOST                 0x00000010
#define IF_PCI_INTR                 0x00000020
#define IF_PCI_CLK_MASK             0x00030000
#define IF_PCI_CLK_INPUT            0
#define IF_PCI_CLK_OUTPUT_LOW       1
#define IF_PCI_CLK_OUTPUT_CLK       2
#define IF_PCI_CLK_OUTPUT_HIGH      3
#define IF_PCI_CLK_SHIFT            16


/* Major revision numbers, bits 7..4 of Revision ID register */
#define REV_MAJ_AR5311              0x01
#define REV_MAJ_AR5312              0x04
#define REV_MAJ_AR5315              0x0B

/*
 * APB Interrupt control
 */

#define AR5315_ISR              (AR5315_DSLBASE + 0x0020)
#define AR5315_IMR              (AR5315_DSLBASE + 0x0024)
#define AR5315_GISR             (AR5315_DSLBASE + 0x0028)

#define AR5315_ISR_UART0                   0x0001           /* high speed UART */
#define AR5315_ISR_I2C_RSVD                0x0002           /* I2C bus */
#define AR5315_ISR_SPI                     0x0004           /* SPI bus */
#define AR5315_ISR_AHB                     0x0008           /* AHB error */
#define AR5315_ISR_APB                     0x0010           /* APB error */
#define AR5315_ISR_TIMER                   0x0020           /* timer */
#define AR5315_ISR_GPIO                    0x0040           /* GPIO */
#define AR5315_ISR_WD                      0x0080           /* watchdog */
#define AR5315_ISR_IR_RSVD                 0x0100           /* IR */

#define AR5315_GISR_MISC                   0x0001
#define AR5315_GISR_WLAN0                  0x0002
#define AR5315_GISR_MPEGTS_RSVD            0x0004
#define AR5315_GISR_LOCALPCI               0x0008
#define AR5315_GISR_WMACPOLL               0x0010
#define AR5315_GISR_TIMER                  0x0020
#define AR5315_GISR_ETHERNET               0x0040

/*
 * Interrupt routing from IO to the processor IP bits
 * Define our inter mask and level
 */
#define AR5315_INTR_MISCIO      SR_IBIT3
#define AR5315_INTR_WLAN0       SR_IBIT4
#define AR5315_INTR_ENET0       SR_IBIT5
#define AR5315_INTR_LOCALPCI    SR_IBIT6
#define AR5315_INTR_WMACPOLL    SR_IBIT7
#define AR5315_INTR_COMPARE     SR_IBIT8

/*
 * Timers
 */
#define AR5315_TIMER            (AR5315_DSLBASE + 0x0030)
#define AR5315_RELOAD           (AR5315_DSLBASE + 0x0034)
#define AR5315_WD               (AR5315_DSLBASE + 0x0038)
#define AR5315_WDC              (AR5315_DSLBASE + 0x003c)

#define WDC_RESET                   0x00000002               /* reset on watchdog */
#define WDC_NMI                     0x00000001               /* NMI on watchdog */
#define WDC_IGNORE_EXPIRATION       0x00000000

/*
 * CPU Performance Counters
 */
#define AR5315_PERFCNT0         (AR5315_DSLBASE + 0x0048)
#define AR5315_PERFCNT1         (AR5315_DSLBASE + 0x004c)

#define PERF_DATAHIT                0x0001  /* Count Data Cache Hits */
#define PERF_DATAMISS               0x0002  /* Count Data Cache Misses */
#define PERF_INSTHIT                0x0004  /* Count Instruction Cache Hits */
#define PERF_INSTMISS               0x0008  /* Count Instruction Cache Misses */
#define PERF_ACTIVE                 0x0010  /* Count Active Processor Cycles */
#define PERF_WBHIT                  0x0020  /* Count CPU Write Buffer Hits */
#define PERF_WBMISS                 0x0040  /* Count CPU Write Buffer Misses */

#define PERF_EB_ARDY                0x0001  /* Count EB_ARdy signal */
#define PERF_EB_AVALID              0x0002  /* Count EB_AValid signal */
#define PERF_EB_WDRDY               0x0004  /* Count EB_WDRdy signal */
#define PERF_EB_RDVAL               0x0008  /* Count EB_RdVal signal */
#define PERF_VRADDR                 0x0010  /* Count valid read address cycles */
#define PERF_VWADDR                 0x0020  /* Count valid write address cycles */
#define PERF_VWDATA                 0x0040  /* Count valid write data cycles */

/*
 * AHB Error Reporting.
 */
#define AR5315_AHB_ERR0         (AR5315_DSLBASE + 0x0050)  /* error  */
#define AR5315_AHB_ERR1         (AR5315_DSLBASE + 0x0054)  /* haddr  */
#define AR5315_AHB_ERR2         (AR5315_DSLBASE + 0x0058)  /* hwdata */
#define AR5315_AHB_ERR3         (AR5315_DSLBASE + 0x005c)  /* hrdata */
#define AR5315_AHB_ERR4         (AR5315_DSLBASE + 0x0060)  /* status */

#define AHB_ERROR_DET               1   /* AHB Error has been detected,          */
                                        /* write 1 to clear all bits in ERR0     */
#define AHB_ERROR_OVR               2   /* AHB Error overflow has been detected  */
#define AHB_ERROR_WDT               4   /* AHB Error due to wdt instead of hresp */

#define PROCERR_HMAST               0x0000000f
#define PROCERR_HMAST_DFLT          0
#define PROCERR_HMAST_WMAC          1
#define PROCERR_HMAST_ENET          2
#define PROCERR_HMAST_PCIENDPT      3
#define PROCERR_HMAST_LOCAL         4
#define PROCERR_HMAST_CPU           5
#define PROCERR_HMAST_PCITGT        6

#define PROCERR_HMAST_S             0
#define PROCERR_HWRITE              0x00000010
#define PROCERR_HSIZE               0x00000060
#define PROCERR_HSIZE_S             5
#define PROCERR_HTRANS              0x00000180
#define PROCERR_HTRANS_S            7
#define PROCERR_HBURST              0x00000e00
#define PROCERR_HBURST_S            9



/*
 * Clock Control
 */
#define AR5315_PLLC_CTL         (AR5315_DSLBASE + 0x0064)
#define AR5315_PLLV_CTL         (AR5315_DSLBASE + 0x0068)
#define AR5315_CPUCLK           (AR5315_DSLBASE + 0x006c)
#define AR5315_AMBACLK          (AR5315_DSLBASE + 0x0070)
#define AR5315_SYNCCLK          (AR5315_DSLBASE + 0x0074)
#define AR5315_DSL_SLEEP_CTL    (AR5315_DSLBASE + 0x0080)
#define AR5315_DSL_SLEEP_DUR    (AR5315_DSLBASE + 0x0084)

/* PLLc Control fields */
#define PLLC_REF_DIV_M              0x00000003
#define PLLC_REF_DIV_S              0
#define PLLC_FDBACK_DIV_M           0x0000007C
#define PLLC_FDBACK_DIV_S           2
#define PLLC_ADD_FDBACK_DIV_M       0x00000080
#define PLLC_ADD_FDBACK_DIV_S       7
#define PLLC_CLKC_DIV_M             0x0001c000
#define PLLC_CLKC_DIV_S             14
#define PLLC_CLKM_DIV_M             0x00700000
#define PLLC_CLKM_DIV_S             20

/* CPU CLK Control fields */
#define CPUCLK_CLK_SEL_M            0x00000003
#define CPUCLK_CLK_SEL_S            0
#define CPUCLK_CLK_DIV_M            0x0000000c
#define CPUCLK_CLK_DIV_S            2

/* AMBA CLK Control fields */
#define AMBACLK_CLK_SEL_M           0x00000003
#define AMBACLK_CLK_SEL_S           0
#define AMBACLK_CLK_DIV_M           0x0000000c
#define AMBACLK_CLK_DIV_S           2

#if defined(COBRA_EMUL)
#define AR5315_AMBA_CLOCK_RATE  20000000
#define AR5315_CPU_CLOCK_RATE   40000000
#else
#if defined(DEFAULT_PLL)
#define AR5315_AMBA_CLOCK_RATE  40000000
#define AR5315_CPU_CLOCK_RATE   40000000
#else
#define AR5315_AMBA_CLOCK_RATE  92000000
#define AR5315_CPU_CLOCK_RATE   184000000
#endif /* ! DEFAULT_PLL */
#endif /* ! COBRA_EMUL */

#define AR5315_UART_CLOCK_RATE  AR5315_AMBA_CLOCK_RATE
#define AR5315_SDRAM_CLOCK_RATE AR5315_AMBA_CLOCK_RATE

/*
 * The UART computes baud rate as:
 *   baud = clock / (16 * divisor)
 * where divisor is specified as a High Byte (DLM) and a Low Byte (DLL).
 */
#define DESIRED_BAUD_RATE           38400


#define CLOCKCTL_UART0  0x0010  /* enable UART0 external clock */


 /*
 * Applicable "PCICFG" bits for WLAN(s).  Assoc status and LED mode.
 */
#define ASSOC_STATUS_M              0x00000003
#define ASSOC_STATUS_NONE           0
#define ASSOC_STATUS_PENDING        1
#define ASSOC_STATUS_ASSOCIATED     2
#define LED_MODE_M                  0x0000001c
#define LED_BLINK_THRESHOLD_M       0x000000e0
#define LED_SLOW_BLINK_MODE         0x00000100

/*
 * GPIO
 */

#define AR5315_GPIO_DI          (AR5315_DSLBASE + 0x0088)
#define AR5315_GPIO_DO          (AR5315_DSLBASE + 0x0090)
#define AR5315_GPIO_CR          (AR5315_DSLBASE + 0x0098)
#define AR5315_GPIO_INT         (AR5315_DSLBASE + 0x00a0)

#define AR5315_GPIO_CR_M(x)                (1 << (x))                  /* mask for i/o */
#define AR5315_GPIO_CR_O(x)                (1 << (x))                  /* output */
#define AR5315_GPIO_CR_I(x)                (0)                         /* input */

#define AR5315_GPIO_INT_S(x)               (x)                         /* interrupt enable */
#define AR5315_GPIO_INT_M                  (0x3F)                      /* mask for int */
#define AR5315_GPIO_INT_LVL(x)             ((x) << 6)                  /* interrupt level */
#define AR5315_GPIO_INT_LVL_M              ((0x3) << 6)                /* mask for int level */

#define AR5315_GPIO_INT_MAX_Y				1   /* Maximum value of Y for AR5313_GPIO_INT_* macros */
#define AR5315_GPIO_INT_LVL_OFF				0   /* Triggerring off */
#define AR5315_GPIO_INT_LVL_LOW				1   /* Low Level Triggered */
#define AR5315_GPIO_INT_LVL_HIGH			2   /* High Level Triggered */
#define AR5315_GPIO_INT_LVL_EDGE			3   /* Edge Triggered */

#define AR5315_RESET_GPIO       5
#define AR5315_NUM_GPIO         22


/*
 *  PCI Clock Control
 */

#define AR5315_PCICLK           (AR5315_DSLBASE + 0x00a4)

#define AR5315_PCICLK_INPUT_M              0x3
#define AR5315_PCICLK_INPUT_S              0

#define AR5315_PCICLK_PLLC_CLKM            0
#define AR5315_PCICLK_PLLC_CLKM1           1
#define AR5315_PCICLK_PLLC_CLKC            2
#define AR5315_PCICLK_REF_CLK              3

#define AR5315_PCICLK_DIV_M                0xc
#define AR5315_PCICLK_DIV_S                2

#define AR5315_PCICLK_IN_FREQ              0
#define AR5315_PCICLK_IN_FREQ_DIV_6        1
#define AR5315_PCICLK_IN_FREQ_DIV_8        2
#define AR5315_PCICLK_IN_FREQ_DIV_10       3

/*
 * Observation Control Register
 */
#define AR5315_OCR              (AR5315_DSLBASE + 0x00b0)
#define OCR_GPIO0_IRIN              0x0040
#define OCR_GPIO1_IROUT             0x0080
#define OCR_GPIO3_RXCLR             0x0200

/*
 *  General Clock Control
 */

#define AR5315_MISCCLK          (AR5315_DSLBASE + 0x00b4)
#define MISCCLK_PLLBYPASS_EN        0x00000001
#define MISCCLK_PROCREFCLK          0x00000002

/*
 * SDRAM Controller
 *   - No read or write buffers are included.
 */
#define AR5315_MEM_CFG          (AR5315_SDRAMCTL + 0x00)
#define AR5315_MEM_CTRL         (AR5315_SDRAMCTL + 0x0c)
#define AR5315_MEM_REF          (AR5315_SDRAMCTL + 0x10)

#define SDRAM_DATA_WIDTH_M          0x00006000
#define SDRAM_DATA_WIDTH_S          13

#define SDRAM_COL_WIDTH_M           0x00001E00
#define SDRAM_COL_WIDTH_S           9

#define SDRAM_ROW_WIDTH_M           0x000001E0
#define SDRAM_ROW_WIDTH_S           5

#define SDRAM_BANKADDR_BITS_M       0x00000018
#define SDRAM_BANKADDR_BITS_S       3

/*
 * SPI Flash Interface Registers
 */

#define AR5315_SPI_CTL      (AR5315_SPI + 0x00)
#define AR5315_SPI_OPCODE   (AR5315_SPI + 0x04)
#define AR5315_SPI_DATA     (AR5315_SPI + 0x08)

#define SPI_CTL_START           0x00000100
#define SPI_CTL_BUSY            0x00010000
#define SPI_CTL_TXCNT_MASK      0x0000000f
#define SPI_CTL_RXCNT_MASK      0x000000f0
#define SPI_CTL_TX_RX_CNT_MASK  0x000000ff
#define SPI_CTL_SIZE_MASK       0x00060000

#define SPI_CTL_CLK_SEL_MASK    0x03000000
#define SPI_OPCODE_MASK         0x000000ff

/*
 * PCI-MAC Configuration registers
 */
#define PCI_MAC_RC              (AR5315_PCI + 0x4000)
#define PCI_MAC_SCR             (AR5315_PCI + 0x4004)
#define PCI_MAC_INTPEND         (AR5315_PCI + 0x4008)
#define PCI_MAC_SFR             (AR5315_PCI + 0x400C)
#define PCI_MAC_PCICFG          (AR5315_PCI + 0x4010)
#define PCI_MAC_SREV            (AR5315_PCI + 0x4020)

#define PCI_MAC_RC_MAC          0x00000001
#define PCI_MAC_RC_BB           0x00000002

#define PCI_MAC_SCR_SLMODE_M    0x00030000
#define PCI_MAC_SCR_SLMODE_S    16
#define PCI_MAC_SCR_SLM_FWAKE   0
#define PCI_MAC_SCR_SLM_FSLEEP  1
#define PCI_MAC_SCR_SLM_NORMAL  2

#define PCI_MAC_SFR_SLEEP       0x00000001

#define PCI_MAC_PCICFG_SPWR_DN  0x00010000


/*
 * PCI Bus Interface Registers
 */
#define AR5315_PCI_1MS_REG      (AR5315_PCI + 0x0008)
#define AR5315_PCI_1MS_MASK     0x3FFFF         /* # of AHB clk cycles in 1ms */

#define AR5315_PCI_MISC_CONFIG  (AR5315_PCI + 0x000c)
#define AR5315_PCIMISC_TXD_EN   0x00000001      /* Enable TXD for fragments */
#define AR5315_PCIMISC_CFG_SEL  0x00000002      /* mem or config cycles */
#define AR5315_PCIMISC_GIG_MASK 0x0000000C      /* bits 31-30 for pci req */
#define AR5315_PCIMISC_RST_MODE 0x00000030
#define AR5315_PCIRST_INPUT     0x00000000      /* 4:5=0 rst is input */
#define AR5315_PCIRST_LOW       0x00000010      /* 4:5=1 rst to GND */
#define AR5315_PCIRST_HIGH      0x00000020      /* 4:5=2 rst to VDD */
#define AR5315_PCIGRANT_EN      0x00000000      /* 6:7=0 early grant en */
#define AR5315_PCIGRANT_FRAME   0x00000040      /* 6:7=1 grant waits 4 frame */
#define AR5315_PCIGRANT_IDLE    0x00000080      /* 6:7=2 grant waits 4 idle */
#define AR5315_PCIGRANT_GAP     0x00000000      /* 6:7=2 grant waits 4 idle */
#define AR5315_PCICACHE_DIS     0x00001000      /* PCI external access cache disable */

#define AR5315_PCI_OUT_TSTAMP   (AR5315_PCI + 0x0010)

#define AR5315_PCI_UNCACHE_CFG  (AR5315_PCI + 0x0014)

#define AR5315_PCI_IN_EN        (AR5315_PCI + 0x0100)
#define AR5315_PCI_IN_EN0       0x01            /* Enable chain 0 */
#define AR5315_PCI_IN_EN1       0x02            /* Enable chain 1 */
#define AR5315_PCI_IN_EN2       0x04            /* Enable chain 2 */
#define AR5315_PCI_IN_EN3       0x08            /* Enable chain 3 */

#define AR5315_PCI_IN_DIS       (AR5315_PCI + 0x0104)
#define AR5315_PCI_IN_DIS0      0x01            /* Disable chain 0 */
#define AR5315_PCI_IN_DIS1      0x02            /* Disable chain 1 */
#define AR5315_PCI_IN_DIS2      0x04            /* Disable chain 2 */
#define AR5315_PCI_IN_DIS3      0x08            /* Disable chain 3 */

#define AR5315_PCI_IN_PTR       (AR5315_PCI + 0x0200)

#define AR5315_PCI_OUT_EN       (AR5315_PCI + 0x0400)
#define AR5315_PCI_OUT_EN0      0x01            /* Enable chain 0 */

#define AR5315_PCI_OUT_DIS      (AR5315_PCI + 0x0404)
#define AR5315_PCI_OUT_DIS0     0x01            /* Disable chain 0 */

#define AR5315_PCI_OUT_PTR      (AR5315_PCI + 0x0408)

#define AR5315_PCI_INT_STATUS   (AR5315_PCI + 0x0500)   /* write one to clr */
#define AR5315_PCI_TXINT        0x00000001      /* Desc In Completed */
#define AR5315_PCI_TXOK         0x00000002      /* Desc In OK */
#define AR5315_PCI_TXERR        0x00000004      /* Desc In ERR */
#define AR5315_PCI_TXEOL        0x00000008      /* Desc In End-of-List */
#define AR5315_PCI_RXINT        0x00000010      /* Desc Out Completed */
#define AR5315_PCI_RXOK         0x00000020      /* Desc Out OK */
#define AR5315_PCI_RXERR        0x00000040      /* Desc Out ERR */
#define AR5315_PCI_RXEOL        0x00000080      /* Desc Out EOL */
#define AR5315_PCI_TXOOD        0x00000200      /* Desc In Out-of-Desc */
#define AR5315_PCI_MASK         0x0000FFFF      /* Desc Mask */
#define AR5315_PCI_EXT_INT      0x02000000
#define AR5315_PCI_ABORT_INT    0x04000000

#define AR5315_PCI_INT_MASK     (AR5315_PCI + 0x0504)   /* same as INT_STATUS */

#define AR5315_PCI_INTEN_REG    (AR5315_PCI + 0x0508)
#define AR5315_PCI_INT_DISABLE  0x00            /* disable pci interrupts */
#define AR5315_PCI_INT_ENABLE   0x01            /* enable pci interrupts */

#define AR5315_PCI_HOST_IN_EN   (AR5315_PCI + 0x0800)
#define AR5315_PCI_HOST_IN_DIS  (AR5315_PCI + 0x0804)
#define AR5315_PCI_HOST_IN_PTR  (AR5315_PCI + 0x0810)
#define AR5315_PCI_HOST_OUT_EN  (AR5315_PCI + 0x0900)
#define AR5315_PCI_HOST_OUT_DIS (AR5315_PCI + 0x0904)
#define AR5315_PCI_HOST_OUT_PTR (AR5315_PCI + 0x0908)


/*
 * Local Bus Interface Registers
 */
#define AR5315_LB_CONFIG        (AR5315_LOCAL + 0x0000)
#define AR5315_LBCONF_OE        0x00000001      /* =1 OE is low-true */
#define AR5315_LBCONF_CS0       0x00000002      /* =1 first CS is low-true */
#define AR5315_LBCONF_CS1       0x00000004      /* =1 2nd CS is low-true */
#define AR5315_LBCONF_RDY       0x00000008      /* =1 RDY is low-true */
#define AR5315_LBCONF_WE        0x00000010      /* =1 Write En is low-true */
#define AR5315_LBCONF_WAIT      0x00000020      /* =1 WAIT is low-true */
#define AR5315_LBCONF_ADS       0x00000040      /* =1 Adr Strobe is low-true */
#define AR5315_LBCONF_MOT       0x00000080      /* =0 Intel, =1 Motorola */
#define AR5315_LBCONF_8CS       0x00000100      /* =1 8 bits CS, 0= 16bits */
#define AR5315_LBCONF_8DS       0x00000200      /* =1 8 bits Data S, 0=16bits */
#define AR5315_LBCONF_ADS_EN    0x00000400      /* =1 Enable ADS */
#define AR5315_LBCONF_ADR_OE    0x00000800      /* =1 Adr cap on OE, WE or DS */
#define AR5315_LBCONF_ADDT_MUX  0x00001000      /* =1 Adr and Data share bus */
#define AR5315_LBCONF_DATA_OE   0x00002000      /* =1 Data cap on OE, WE, DS */
#define AR5315_LBCONF_16DATA    0x00004000      /* =1 Data is 16 bits wide */
#define AR5315_LBCONF_SWAPDT    0x00008000      /* =1 Byte swap data */
#define AR5315_LBCONF_SYNC      0x00010000      /* =1 Bus synchronous to clk */
#define AR5315_LBCONF_INT       0x00020000      /* =1 Intr is low true */
#define AR5315_LBCONF_INT_CTR0  0x00000000      /* GND high-Z, Vdd is high-Z */
#define AR5315_LBCONF_INT_CTR1  0x00040000      /* GND drive, Vdd is high-Z */
#define AR5315_LBCONF_INT_CTR2  0x00080000      /* GND high-Z, Vdd drive */
#define AR5315_LBCONF_INT_CTR3  0x000C0000      /* GND drive, Vdd drive */
#define AR5315_LBCONF_RDY_WAIT  0x00100000      /* =1 RDY is negative of WAIT */
#define AR5315_LBCONF_INT_PULSE 0x00200000      /* =1 Interrupt is a pulse */
#define AR5315_LBCONF_ENABLE    0x00400000      /* =1 Falcon respond to LB */

#define AR5315_LB_CLKSEL        (AR5315_LOCAL + 0x0004)
#define AR5315_LBCLK_EXT        0x0001          /* use external clk for lb */

#define AR5315_LB_1MS           (AR5315_LOCAL + 0x0008)
#define AR5315_LB1MS_MASK       0x3FFFF         /* # of AHB clk cycles in 1ms */

#define AR5315_LB_MISCCFG       (AR5315_LOCAL + 0x000C)
#define AR5315_LBM_TXD_EN       0x00000001      /* Enable TXD for fragments */
#define AR5315_LBM_RX_INTEN     0x00000002      /* Enable LB ints on RX ready */
#define AR5315_LBM_MBOXWR_INTEN 0x00000004      /* Enable LB ints on mbox wr */
#define AR5315_LBM_MBOXRD_INTEN 0x00000008      /* Enable LB ints on mbox rd */
#define AR5315_LMB_DESCSWAP_EN  0x00000010      /* Byte swap desc enable */
#define AR5315_LBM_TIMEOUT_MASK 0x00FFFF80
#define AR5315_LBM_TIMEOUT_SHFT 7
#define AR5315_LBM_PORTMUX      0x07000000


#define AR5315_LB_RXTSOFF       (AR5315_LOCAL + 0x0010)

#define AR5315_LB_TX_CHAIN_EN   (AR5315_LOCAL + 0x0100)
#define AR5315_LB_TXEN_0        0x01
#define AR5315_LB_TXEN_1        0x02
#define AR5315_LB_TXEN_2        0x04
#define AR5315_LB_TXEN_3        0x08

#define AR5315_LB_TX_CHAIN_DIS  (AR5315_LOCAL + 0x0104)
#define AR5315_LB_TX_DESC_PTR   (AR5315_LOCAL + 0x0200)

#define AR5315_LB_RX_CHAIN_EN   (AR5315_LOCAL + 0x0400)
#define AR5315_LB_RXEN          0x01

#define AR5315_LB_RX_CHAIN_DIS  (AR5315_LOCAL + 0x0404)
#define AR5315_LB_RX_DESC_PTR   (AR5315_LOCAL + 0x0408)

#define AR5315_LB_INT_STATUS    (AR5315_LOCAL + 0x0500)
#define AR5315_INT_TX_DESC      0x0001
#define AR5315_INT_TX_OK        0x0002
#define AR5315_INT_TX_ERR       0x0004
#define AR5315_INT_TX_EOF       0x0008
#define AR5315_INT_RX_DESC      0x0010
#define AR5315_INT_RX_OK        0x0020
#define AR5315_INT_RX_ERR       0x0040
#define AR5315_INT_RX_EOF       0x0080
#define AR5315_INT_TX_TRUNC     0x0100
#define AR5315_INT_TX_STARVE    0x0200
#define AR5315_INT_LB_TIMEOUT   0x0400
#define AR5315_INT_LB_ERR       0x0800
#define AR5315_INT_MBOX_WR      0x1000
#define AR5315_INT_MBOX_RD      0x2000

/* Bit definitions for INT MASK are the same as INT_STATUS */
#define AR5315_LB_INT_MASK      (AR5315_LOCAL + 0x0504)

#define AR5315_LB_INT_EN        (AR5315_LOCAL + 0x0508)
#define AR5315_LB_MBOX          (AR5315_LOCAL + 0x0600)



/*
 * IR Interface Registers
 */
#define AR5315_IR_PKTDATA                   (AR5315_IR + 0x0000)

#define AR5315_IR_PKTLEN                    (AR5315_IR + 0x07fc) /* 0 - 63 */

#define AR5315_IR_CONTROL                   (AR5315_IR + 0x0800)
#define AR5315_IRCTL_TX                     0x00000000  /* use as tranmitter */
#define AR5315_IRCTL_RX                     0x00000001  /* use as receiver   */
#define AR5315_IRCTL_SAMPLECLK_MASK         0x00003ffe  /* Sample clk divisor mask */
#define AR5315_IRCTL_SAMPLECLK_SHFT                  1
#define AR5315_IRCTL_OUTPUTCLK_MASK         0x03ffc000  /* Output clk divisor mask */
#define AR5315_IRCTL_OUTPUTCLK_SHFT                 14

#define AR5315_IR_STATUS                    (AR5315_IR + 0x0804)
#define AR5315_IRSTS_RX                     0x00000001  /* receive in progress */
#define AR5315_IRSTS_TX                     0x00000002  /* transmit in progress */

#define AR5315_IR_CONFIG                    (AR5315_IR + 0x0808)
#define AR5315_IRCFG_INVIN                  0x00000001  /* invert input polarity */
#define AR5315_IRCFG_INVOUT                 0x00000002  /* invert output polarity */
#define AR5315_IRCFG_SEQ_START_WIN_SEL      0x00000004  /* 1 => 28, 0 => 7 */
#define AR5315_IRCFG_SEQ_START_THRESH       0x000000f0  /*  */
#define AR5315_IRCFG_SEQ_END_UNIT_SEL       0x00000100  /*  */
#define AR5315_IRCFG_SEQ_END_UNIT_THRESH    0x00007e00  /*  */
#define AR5315_IRCFG_SEQ_END_WIN_SEL        0x00008000  /*  */
#define AR5315_IRCFG_SEQ_END_WIN_THRESH     0x001f0000  /*  */
#define AR5315_IRCFG_NUM_BACKOFF_WORDS      0x01e00000  /*  */

/*
 * PCI memory constants: Memory area 1 and 2 are the same size -
 * (twice the PCI_TLB_PAGE_SIZE). The definition of
 * CPU_TO_PCI_MEM_SIZE is coupled with the TLB setup routine
 * sysLib.c/sysTlbInit(), in that it assumes that 2 pages of size
 * PCI_TLB_PAGE_SIZE are set up in the TLB for each PCI memory space.
 */

#define CPU_TO_PCI_MEM_BASE1    0xE0000000
#define CPU_TO_PCI_MEM_SIZE1    (2*PCI_TLB_PAGE_SIZE)


/* TLB attributes for PCI transactions */

#define PCI_MMU_PAGEMASK        0x00003FFF
#define MMU_PAGE_UNCACHED       0x00000010
#define MMU_PAGE_DIRTY          0x00000004
#define MMU_PAGE_VALID          0x00000002
#define MMU_PAGE_GLOBAL         0x00000001
#define PCI_MMU_PAGEATTRIB      (MMU_PAGE_UNCACHED|MMU_PAGE_DIRTY|\
                                 MMU_PAGE_VALID|MMU_PAGE_GLOBAL)
#define PCI_MEMORY_SPACE1_VIRT  0xE0000000      /* Used for non-prefet  mem   */
#define PCI_MEMORY_SPACE1_PHYS  0x80000000
#define PCI_TLB_PAGE_SIZE       0x01000000
#define TLB_HI_MASK             0xFFFFE000
#define TLB_LO_MASK             0x3FFFFFFF
#define PAGEMASK_SHIFT          11
#define TLB_LO_SHIFT            6

#define PCI_MAX_LATENCY         0xFFF           /* Max PCI latency            */

#define HOST_PCI_DEV_ID         3
#define HOST_PCI_MBAR0          0x10000000
#define HOST_PCI_MBAR1          0x20000000
#define HOST_PCI_MBAR2          0x30000000

#define HOST_PCI_SDRAM_BASEADDR HOST_PCI_MBAR1
#define PCI_DEVICE_MEM_SPACE    0x800000

#endif


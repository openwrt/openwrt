/*
 * arch/ubicom32/include/asm/ip5000.h
 *   Specific details for the Ubicom IP5000 processor.
 *
 * (C) Copyright 2009, Ubicom, Inc.
 *
 * This file is part of the Ubicom32 Linux Kernel Port.
 *
 * The Ubicom32 Linux Kernel Port is free software: you can redistribute
 * it and/or modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation, either version 2 of the
 * License, or (at your option) any later version.
 *
 * The Ubicom32 Linux Kernel Port is distributed in the hope that it
 * will be useful, but WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See
 * the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with the Ubicom32 Linux Kernel Port.  If not,
 * see <http://www.gnu.org/licenses/>.
 *
 * Ubicom32 implementation derived from (with many thanks):
 *   arch/m68knommu
 *   arch/blackfin
 *   arch/parisc
 */

#ifndef _ASM_UBICOM32_IP5000_H
#define _ASM_UBICOM32_IP5000_H

#include <asm/memory_map.h>

/*
 * Inline assembly define
 */
#define S(arg) #arg
#define D(arg) S(arg)

/*
 * Assembler include file
 */
#include <asm/ip5000-asm.h>

/*
 * Timing
 */
#define JMPT_PENALTY 3
#define JMPF_PENALTY 7
#define RET_PENALTY 7

/*
 * Threads
 */
#if defined(IP5000) || defined(IP5000_REV2)
#define THREAD_COUNT 10
#elif defined(IP7000) || defined(IP7000_REV2)
#define THREAD_COUNT 12
#else
#error "Unknown IP5K silicon"
#endif

/*
 * Arch
 */
#if defined(IP5000) || defined(IP5000_REV2)
#define UBICOM32_ARCH_VERSION 3
#elif defined(IP7000) || defined(IP7000_REV2)
#define UBICOM32_ARCH_VERSION 4
#else
#error "Unknown IP5K silicon"
#endif


/*
 * Registers
 */
#define ROSR_INT (1 << 0)

/* Interrupts */
#define INT_CHIP(reg, bit) (((reg) << 5) | (bit))
#define INT_REG(interrupt) (((interrupt) >> 5) * 4)
#define INT_SET(interrupt) 0x0114 + INT_REG(interrupt)
#define INT_CLR(interrupt) 0x0124 + INT_REG(interrupt)
#define INT_STAT(interrupt) 0x0104 + INT_REG(interrupt)
#define INT_MASK(interrupt) 0x00C0 + INT_REG(interrupt)
#define INT_BIT(interrupt) ((interrupt) & 0x1F)
#define INT_BIT_MASK(interrupt) (1 << INT_BIT(interrupt))

/*
 * The LOCK_INT and THREAD_INT are used to wake up corresponding thread. They are sharing
 * the same set of SW interrupt resource.
 *
 * LOCK_INT(n): One SW INT per NRT thread that can participate lock operation.
 *	The threads that can participate lock are application threads and DSR thread.
 *	(Lock locks - numbers are hard-coded in lock.h)
 * THREAD_INT(n):   One SW INT per HRT thread for wake up trigger.
 */
#define LOCK_INT(thread)	INT_CHIP(0, (thread))
#define THREAD_INT(thread)	INT_CHIP(0, (thread))

/*
 * The SYSTEM_INT and DSR_INT are sharing the same set of SW interrupt resource.
 *
 * SYSTEM_INT(n): One SW INT per NRT threads (application threads) as system queue interrupt,
 *	and for DSR as self-trigger interrupt.
 *	(The application threads include at least thread 0)
 * DSR_INT(n):    One SW INT per HRT thread to request DSR service.
 */
#define SYSTEM_INT(thread)	INT_CHIP(0, THREAD_COUNT + (thread))
#define DSR_INT(thread)		INT_CHIP(0, THREAD_COUNT + (thread))

/* GLOBAL_CTRL */
#define GLOBAL_CTRL_TRAP_RST_EN (1 << 9)
#define GLOBAL_CTRL_AERROR_RST_EN (1 << 8)
#define GLOBAL_CTRL_MT_MIN_DELAY(x) ((x) << 3)
#define GLOBAL_CTRL_HRT_BANK_SELECT (1 << 2)
#define GLOBAL_CTRL_INT_EN (1 << 0)

/*
 * HRT Tables
 */
#define HRT_TABLE0_BASE 0x0800
#define HRT_TABLE1_BASE 0x0900
#define HRT_TABLE_SIZE 64

/*
 * Break Point Trap Register
 */
#define ASYNCERROR_INT INT_CHIP(0, 31)
#define BREAKPOINT_INT INT_CHIP(1, 31)

/*
 * Port interrupts
 *	The non-existing FIFO INTs are mapped to INT2 for the ports.
 */
#define IO_PORT_PTR_TO_NUM(port) (((port) & 0x0000ffff) >> 12)
#define RX_FIFO_INT(port) \
	((IO_PORT_PTR_TO_NUM(port) == 0) ? INT_CHIP(0, 25) : \
	((IO_PORT_PTR_TO_NUM(port) == 1) ? INT_CHIP(0, 26) : \
	((IO_PORT_PTR_TO_NUM(port) == 2) ? INT_CHIP(0, 29) : \
	((IO_PORT_PTR_TO_NUM(port) == 3) ? INT_CHIP(1, 24) : \
	((IO_PORT_PTR_TO_NUM(port) == 4) ? INT_CHIP(1, 27) : \
	((IO_PORT_PTR_TO_NUM(port) == 5) ? INT_CHIP(1, 16) : \
	((IO_PORT_PTR_TO_NUM(port) == 6) ? INT_CHIP(1, 19) : \
	((IO_PORT_PTR_TO_NUM(port) == 7) ? INT_CHIP(1, 20) : \
	((IO_PORT_PTR_TO_NUM(port) == 8) ? INT_CHIP(1, 21) : \
	INT_CHIP(1, 15))))))))))
#define TX_FIFO_INT(port) \
	((IO_PORT_PTR_TO_NUM(port) == 0) ? INT_CHIP(0, 24) : \
	((IO_PORT_PTR_TO_NUM(port) == 1) ? INT_CHIP(0, 27) : \
	((IO_PORT_PTR_TO_NUM(port) == 2) ? INT_CHIP(0, 29) : \
	((IO_PORT_PTR_TO_NUM(port) == 3) ? INT_CHIP(1, 25) : \
	((IO_PORT_PTR_TO_NUM(port) == 4) ? INT_CHIP(1, 28) : \
	((IO_PORT_PTR_TO_NUM(port) == 5) ? INT_CHIP(1, 17) : \
	((IO_PORT_PTR_TO_NUM(port) == 6) ? INT_CHIP(1, 19) : \
	((IO_PORT_PTR_TO_NUM(port) == 7) ? INT_CHIP(1, 20) : \
	((IO_PORT_PTR_TO_NUM(port) == 8) ? INT_CHIP(1, 22) : \
	INT_CHIP(1, 15))))))))))
#define PORT_OTHER_INT(port) \
	((IO_PORT_PTR_TO_NUM(port) == 0) ? INT_CHIP(0, 25) : \
	((IO_PORT_PTR_TO_NUM(port) == 1) ? INT_CHIP(0, 28) : \
	((IO_PORT_PTR_TO_NUM(port) == 2) ? INT_CHIP(0, 29) : \
	((IO_PORT_PTR_TO_NUM(port) == 3) ? INT_CHIP(1, 26) : \
	((IO_PORT_PTR_TO_NUM(port) == 4) ? INT_CHIP(1, 29) : \
	((IO_PORT_PTR_TO_NUM(port) == 5) ? INT_CHIP(1, 18) : \
	((IO_PORT_PTR_TO_NUM(port) == 6) ? INT_CHIP(1, 19) : \
	((IO_PORT_PTR_TO_NUM(port) == 7) ? INT_CHIP(1, 20) : \
	((IO_PORT_PTR_TO_NUM(port) == 8) ? INT_CHIP(1, 23) : \
	INT_CHIP(1, 15))))))))))

/*
 * On Chip Peripherals Base.
 */
#define OCP_BASE	0x01000000
#define OCP_GENERAL	0x000
#define OCP_TIMERS	0x100
#define OCP_TRNG	0x200	/* True Random Number Generator Control Reigsters */
#define OCP_DEBUG	0x300
#define OCP_SECURITY	0x400
#define OCP_ICCR	0x500	/* I-Cache Control Registers */
#define OCP_DCCR	0x600	/* D-Cache Control Registers */
#define OCP_OCMC	0x700	/* On Chip Memory Control Registers */
#define OCP_STATISTICS	0x800	/* Statistics Counters */
#define OCP_MTEST	0x900	/* Memory Test Registers */
#define OCP_MCFG	0xa00	/* Memory Configuration Registers -- IP7000 only */
#define OCP_DEBUG_INST	0x000	/* Up to 16M */

/*
 * General Configuration Registers (PLL)
 */
#define GENERAL_CFG_BASE (OCP_BASE + OCP_GENERAL)
#define GEN_CLK_CORE_CFG 0x00
#define GEN_CLK_IO_CFG 0x04
#define GEN_CLK_DDR_CFG 0x08
#define GEN_CLK_DDRDS_CFG 0x0c
#define GEN_CLK_SLIP_CLR 0x10
#define GEN_CLK_SLIP_START 0x14
#define GEN_CLK_SERDES_SEL 0x18	/* IP7000 only */
#define GEN_CLK_DDR_CFG2 0x1c	/* IP7000 only */
#define GEN_DDR_CAL_CTRL 0x30	/* IP5000 only */
#define GEN_DDR_CAL_STAT 0x34	/* IP5000 only */
#define GEN_USB_DFT_CTRL 0x38	/* IP5000 only */
#define GEN_USB_DFT_STAT 0x3c	/* IP5000 only */
#define GEN_USB_PHY_CFG 0x40	/* IP7000 only */
#define GEN_USB_PHY_TEST 0x44	/* IP7000 only */
#define GEN_USB_PHY_STAT 0x48	/* IP7000 only */
#define GEN_SW_RESET 0x80
#define GEN_RESET_REASON 0x84
#define GEN_BOND_CFG 0x88
#define GEN_IO_PU_CFG 0x8c
#define GEN_MEM_RM_CFG 0x90
#define GEN_IO_CONFIG 0x94

#define GEN_CLK_PLL_SECURITY_BIT_NO 31
#define GEN_CLK_PLL_SECURITY (1 << GEN_CLK_PLL_SECURITY_BIT_NO)
#define GEN_CLK_PLL_ENSAT (1 << 30)
#define GEN_CLK_PLL_FASTEN (1 << 29)
#define GEN_CLK_PLL_NR(v) (((v) - 1) << 23)
#define GEN_CLK_PLL_NF(v) (((v) - 1) << 11)
#define GEN_CLK_PLL_OD(v) (((v) - 1) << 8)
#define GEN_CLK_PLL_RESET (1 << 7)
#define GEN_CLK_PLL_BYPASS (1 << 6)
#define GEN_CLK_PLL_POWERDOWN (1 << 5)
#define GEN_CLK_PLL_SELECT (1 << 4)

#define GEN_GET_CLK_PLL_NR(v) ((((v) >> 23) & 0x003f) + 1)
#define GEN_GET_CLK_PLL_NF(v) ((((v) >> 11) & 0x0fff) + 1)
#define GEN_GET_CLK_PLL_OD(v) ((((v) >> 8) & 0x7) + 1)


#define RESET_FLAG_DST_MEM_ERROR (1 << 18)
#define RESET_FLAG_SRC1_MEM_ERROR (1 << 17)
#define RESET_FLAG_WRITE_ADDR (1 << 16)
#define RESET_FLAG_DST_SYNC_ERROR (1 << 15)
#define RESET_FLAG_SRC1_SYNC_ERROR (1 << 14)
#define RESET_FLAG_DST_ALGN_ERROR (1 << 13)
#define RESET_FLAG_SRC1_ALGN_ERROR (1 << 12)
#define RESET_FLAG_DST_ADDR_ERROR (1 << 11)
#define RESET_FLAG_SRC1_ADDR_ERROR (1 << 10)
#define RESET_FLAG_ILLEGAL_INST (1 << 9)
#define RESET_FLAG_INST_SYNC_ERROR (1 << 8)
#define RESET_FLAG_INST_ADDR_ERROR (1 << 7)
#define RESET_FLAG_DATA_PORT_ERROR (1 << 6)
#define RESET_FLAG_INST_PORT_ERROR (1 << 5)
#define RESET_FLAG_SW_RESET (1 << 4)
#define RESET_FLAG_DEBUG (1 << 3)
#define RESET_FLAG_WATCHDOG (1 << 2)
#define RESET_FLAG_POWER_ON (1 << 1)
#define RESET_FLAG_EXTERNAL (1 << 0)

/*
 * Timer block
 */
#define TIMER_BASE (OCP_BASE + OCP_TIMERS)
#define TIMER_MPTVAL 0x00
#define TIMER_RTCOM 0x04
#define TIMER_TKEY 0x08
#define TIMER_WDCOM 0x0c
#define TIMER_WDCFG 0x10
#define TIMER_SYSVAL 0x14
#define TIMER_SYSCOM(tmr) (0x18 + (tmr) * 4)
#define TIMER_TRN_CFG 0x100
#define TIMER_TRN 0x104

#define TIMER_COUNT 10
#define TIMER_INT(tmr) INT_CHIP(1, (tmr))
#define TIMER_TKEYVAL 0xa1b2c3d4
#define TIMER_WATCHDOG_DISABLE 0x4d3c2b1a
#define TIMER_TRN_CFG_ENABLE_OSC 0x00000007

#ifndef __ASSEMBLY__
/*
 * ubicom32_io_timer
 */
struct ubicom32_io_timer {
	volatile u32_t mptval;
	volatile u32_t rtcom;
	volatile u32_t tkey;
	volatile u32_t wdcom;
	volatile u32_t wdcfg;
	volatile u32_t sysval;
	volatile u32_t syscom[TIMER_COUNT];
	volatile u32_t reserved[64 - 6 - TIMER_COUNT];	// skip all the way to OCP-TRNG section
	volatile u32_t rsgcfg;
	volatile u32_t trn;
};

#define UBICOM32_IO_TIMER ((struct ubicom32_io_timer *)TIMER_BASE)
#endif

#define UBICOM32_VECTOR_TO_TIMER_INDEX(vector) (vector - TIMER_INT(0))

/*
 * OCP-Debug Module (Mailbox)
 */
#define ISD_MAILBOX_BASE (OCP_BASE + OCP_DEBUG)
#define ISD_MAILBOX_IN 0x00
#define ISD_MAILBOX_OUT 0x04
#define ISD_MAILBOX_STATUS 0x08

#define ISD_MAILBOX_INT INT_CHIP(1, 30)

#define ISD_MAILBOX_STATUS_IN_FULL (1 << 31)
#define ISD_MAILBOX_STATUS_IN_EMPTY (1 << 30)
#define ISD_MAILBOX_STATUS_OUT_FULL (1 << 29)
#define ISD_MAILBOX_STATUS_OUT_EMPTY (1 << 28)

/*
 * OCP-Security
 */
#define SECURITY_BASE (OCP_BASE + OCP_SECURITY)
#define SECURITY_BASE_EFFECTIVE_ADDRESS (SECURITY_BASE >> 7) // To load the base address in a single instruction
#define SECURITY_CTRL 0x00
#define SECURITY_CTRL_BYTE_OFFSET(x) ((x) << 16)
#define SECURITY_CTRL_KEY_SIZE(x) ((x) << 8)
#define SECURITY_CTRL_HASH_ALG_NONE (0 << 4)
#define SECURITY_CTRL_HASH_ALG_MD5 (1 << 4)
#define SECURITY_CTRL_HASH_ALG_SHA1 (2 << 4)
#define SECURITY_CTRL_CBC (1 << 3)
#define SECURITY_CTRL_CIPHER_ALG_AES (0 << 1)
#define SECURITY_CTRL_CIPHER_ALG_NONE (1 << 1)
#define SECURITY_CTRL_CIPHER_ALG_DES (2 << 1)
#define SECURITY_CTRL_CIPHER_ALG_3DES (3 << 1)
#define SECURITY_CTRL_ENCIPHER (1 << 0)
#define SECURITY_CTRL_DECIPHER (0 << 0)
#define SECURITY_STAT 0x04
#define SECURITY_STAT_BUSY (1 << 0)
#define SECURITY_KEY_VALUE(x) (0x10 + (x) * 4)
#define SECURITY_KEY_IN(x) (0x30 + (x) * 4)
#define SECURITY_KEY_OUT(x) (0x50 + (x) * 4)
#define SECURITY_KEY_HASH(x) (0x70 + (x) * 4)

/*
 * OCP-ICCR
 */
#define ICCR_BASE (OCP_BASE + OCP_ICCR)
#define ICACHE_TOTAL_SIZE 16384			/* in bytes */

/*
 * OCP-DCCR
 */
#define DCCR_BASE (OCP_BASE + OCP_DCCR)
#if defined(IP5000) || defined(IP5000_REV2)
#define DCACHE_TOTAL_SIZE 8192			/* in bytes */
#elif defined(IP7000) || defined(IP7000_REV2)
#define DCACHE_TOTAL_SIZE 16384			/* in bytes */
#endif

#if defined(IP5000) || defined(IP5000_REV2) || defined(IP7000) || defined(IP7000_REV2)
#define DCACHE_WRITE_QUEUE_LENGTH 6
#else
#error "Unknown IP5K silicon"
#endif

#define CACHE_LINE_SIZE 32			/* in bytes */

#define CCR_ADDR 0x00
#define CCR_RDD 0x04
#define CCR_WRD 0x08
#define CCR_STAT 0x0c
#define CCR_CTRL 0x10

#define CCR_STAT_MCBE 0
#define CCR_STAT_WIDEL 1			/* D-cache only */

#define CCR_CTRL_DONE 0
#define CCR_CTRL_RESET 2
#define CCR_CTRL_VALID 3
#define CCR_CTRL_RD_DATA (1 << 4)
#define CCR_CTRL_RD_TAG (2 << 4)
#define CCR_CTRL_WR_DATA (3 << 4)
#define CCR_CTRL_WR_TAG (4 << 4)
#define CCR_CTRL_INV_INDEX (5 << 4)
#define CCR_CTRL_INV_ADDR (6 << 4)
#define CCR_CTRL_FLUSH_INDEX (7 << 4)		/* D-cache only */
#define CCR_CTRL_FLUSH_INV_INDEX (8 << 4)	/* D-cache only */
#define CCR_CTRL_FLUSH_ADDR (9 << 4)		/* D-cache only */
#define CCR_CTRL_FLUSH_INV_ADDR (10 << 4)	/* D-cache only */

/*
 * OCP-OCMC
 */
#define OCMC_BASE (OCP_BASE + OCP_OCMC)
#define OCMC_BANK_MASK 0x00
#define OCMC_BIST_CNTL 0x04	/* IP5000 only */
#define OCMC_BIST_STAT 0x08	/* IP5000 only */

#define OCMC_BANK_PROG(n) ((1<<(n))-1)

#define OCMC_BIST_WRCK (1 << 7)
#define OCMC_BIST_RESET (1 << 5)
#define OCMC_BIST_SMART (1 << 4)
#define OCMC_BIST_RUN (1 << 3)
#define OCMC_BIST_REPAIR (1 << 2)

#define OCMC_BIST_READY (1 << 3)
#define OCMC_BIST_FAIL (1 << 2)

/*
 * OCP-STATISTICS
 */
#define STATISTICS_BASE (OCP_BASE + OCP_STATISTICS)
#define STAT_COUNTER_CTRL(n) ((n)*8)
#define STAT_COUNTER(n) ((n)*8 + 4)

#define STAT_EVENT_MP_INST 0
#define STAT_EVENT_OCM_ACCESS 4
#define STAT_EVENT_OCM_REQ 5
#define STAT_EVENT_IC_REQ_INVAL 13
#define STAT_EVENT_IC_MISS_INVAL 14
#define STAT_EVENT_IC_REQ_INVAL_NACK 15
#define STAT_EVENT_IC_REQ_VAL 16
#define STAT_EVENT_IC_MISS_VAL 17
#define STAT_EVENT_IC_REQ_VAL_NACK 18
#define STAT_EVENT_IC_MISS_Q 19
#define STAT_EVENT_DC_RD_REQ 20
#define STAT_EVENT_DC_RD_MISS 21
#define STAT_EVENT_DC_WR_REQ 22
#define STAT_EVENT_DC_WR_MISS 23
#define STAT_EVENT_DC_MISS_Q 24
#define STAT_EVENT_DC_WB_FULL 25
#define STAT_EVENT_DC_REQ_NACK 26
#define STAT_EVENT_DC_CORE_REQ 27
#define STAT_EVENT_DC_MISS 28
#define STAT_EVENT_DC_EVICT 29
#define STAT_EVENT_TRUE 30
#define STAT_EVENT_FALSE 31

/*
 * OCP_MTEST
 */
#define MTEST_BASE (OCP_BASE + OCP_MTEST)
#define MTEST_ADDR 0x00
#define MTEST_WR 0x04
#define MTEST_RD 0x08
#define MTEST_CTRL 0x0c

/*
 * OCP_MCFG (IP7000 only)
 */
#define MCFG_BASE (OCP_BASE + OCP_MCFG)
#define MCFG_CTRL 0x00
#define MCFG_WCFG 0x04
#define MCFG_RCFG 0x08

/*
 * Port registers
 */
#define IO_BASE 0x02000000
#define RA (IO_BASE + 0x00000000)
#define RB (IO_BASE + 0x00001000)
#define RC (IO_BASE + 0x00002000)
#define RD (IO_BASE + 0x00003000)
#define RE (IO_BASE + 0x00004000)
#define RF (IO_BASE + 0x00005000)
#define RG (IO_BASE + 0x00006000)
#define RH (IO_BASE + 0x00007000)
#define RI (IO_BASE + 0x00008000)
#define RJ (IO_BASE + 0x00009000)
#define RLATCH (IO_BASE + 0x00ff0000)	// For latched output only
#define IO_PORT_BR_OFFSET 0x00000800

/*
 * General I/O Register Map (per port)
 */
#define IO_FUNC 0x00
#define IO_GPIO_CTL 0x04
#define IO_GPIO_OUT 0x08
#define IO_GPIO_IN 0x0C
#define IO_INT_STATUS 0x10
#define IO_INT_MASK 0x14
#define IO_INT_SET 0x18
#define IO_INT_CLR 0x1C
#define IO_TX_FIFO 0x20
#define IO_TX_FIFO_HI 0x24
#define IO_RX_FIFO 0x28
#define IO_RX_FIFO_HI 0x2c
#define IO_CTL0 0x30
#define IO_CTL1 0x34
#define IO_CTL2 0x38
#define IO_STATUS0 0x3c
#define IO_STATUS1 0x40
#define IO_STATUS2 0x44
#define IO_FIFO_WATER 0x48
#define IO_FIFO_LEVEL 0x4c
#define IO_GPIO_MASK 0x50

#define IO_FUNC_FUNCTION_RESET(func) ((1 << ((func) - 1)) << 4)	/* Function 0 doesn't need reset */
#define IO_FUNC_RX_FIFO (1 << 3)
#define IO_FUNC_SELECT(func) ((func) << 0)

/*
 * External interrupt pins.
 */
#define EXT_INT_IO_BIT(pin) ((pin) + 5)	// Interrupt pin number -> I/O INT bit
#define EXT_INT_RISING_EDGE(pin) (0x2 << (2*(pin) + 7))
#define EXT_INT_FALLING_EDGE(pin) (0x1 << (2*(pin) + 7))

/*
 * Flash
 */
#define IO_XFL_BASE RA

#define IO_XFL_INT_START (1 << 16)
#define IO_XFL_INT_ERR (1 << 8)
#define IO_XFL_INT_DONE (1 << 0)

#define IO_XFL_CTL0_MASK (0xffe07fff)
#define IO_XFL_CTL0_RD_CMD(cmd) (((cmd) & 0xff) << 24)
#define IO_XFL_CTL0_RD_DUMMY(n) (((n) & 0x7) << 21)
#define IO_XFL_CTL0_CLK_WIDTH(core_cycles) ((((core_cycles) + 1) & 0x7e) << 8)	/* must be even number */
#define IO_XFL_CTL0_CE_WAIT(spi_cycles) (((spi_cycles) & 0x3f) << 2)
#define IO_XFL_CTL0_MCB_LOCK (1 << 1)
#define IO_XFL_CTL0_ENABLE (1 << 0)
#define IO_XFL_CTL0_FAST_VALUE(div, wait) (IO_XFL_CTL0_RD_CMD(0xb) | IO_XFL_CTL0_RD_DUMMY(1) | IO_XFL_CTL0_CLK_WIDTH(div) | IO_XFL_CTL0_CE_WAIT(wait) | IO_XFL_CTL0_ENABLE)
#define IO_XFL_CTL0_VALUE(div, wait) (IO_XFL_CTL0_RD_CMD(3) | IO_XFL_CTL0_CLK_WIDTH(div) | IO_XFL_CTL0_CE_WAIT(wait) | IO_XFL_CTL0_ENABLE)

#define IO_XFL_CTL1_MASK (0xc0003fff)
#define IO_XFL_CTL1_FC_INST(inst) (((inst) & 0x3) << 30)
#define IO_XFL_CTL1_FC_DATA(n) (((n) & 0x3ff) << 4)
#define IO_XFL_CTL1_FC_DUMMY(n) (((n) & 0x7) << 1)
#define IO_XFL_CTL1_FC_ADDR (1 << 0)

#define IO_XFL_CTL2_FC_CMD(cmd) (((cmd) & 0xff) << 24)
#define IO_XFL_CTL2_FC_ADDR(addr) ((addr) & 0x00ffffff)	/* Only up to 24 bits */

#define IO_XFL_STATUS0_MCB_ACTIVE (1 << 0)
#define IO_XFL_STATUS0_IOPCS_ACTIVE (1 << 1)

/*
 * SDRAM
 */
#define IO_SDRAM_DATA_BASE RG
#define IO_SDRAM_CNTL_BASE RH

#define IO_SDRAM_CTRL0_EN_REF (1 << 0)

/*
 * Port function code (common fucntion codes for all I/O ports)
 */
#define IO_PORTX_FUNC_GPIO 0x00
#define IO_PORTX_FUNC_XFL 0x01
#define IO_PORTX_FUNC_PCI 0x01
#define IO_PORTX_FUNC_SERDES 0x01
#define IO_PORTX_FUNC_GMII 0x01
#define IO_PORTX_FUNC_DDR 0x01
#define IO_PORTX_FUNC_PCIX 0x01
#define IO_PORTX_FUNC_USB2_0 0x01
#define IO_PORTX_FUNC_GPIO_INT_CLK 0x02
#define IO_PORTX_FUNC_PLIO 0x02
#define IO_PORTX_FUNC_GPIO_INT 0x03
#define IO_PORTX_FUNC_MII 0x03

/*
 * Port 0
 */
#define IO_PORT0_FUNC_GPIO IO_PORTX_FUNC_GPIO
#define IO_PORT0_FUNC_XFL_INT_CLK IO_PORTX_FUNC_XFL	// Default mode after reset
#define IO_PORT0_FUNC_GPIO_INT_CLK IO_PORTX_FUNC_GPIO_INT_CLK
#define IO_PORT0_FUNC_GPIO_INT IO_PORTX_FUNC_GPIO_INT

/*
 * Port 1
 */
#define IO_PORT1_FUNC_GPIO IO_PORTX_FUNC_GPIO
#define IO_PORT1_FUNC_PCI IO_PORTX_FUNC_PCI		// PCI control
#define IO_PORT1_FUNC_MII IO_PORTX_FUNC_MII		// port 4 MII extension

/*
 * Port 2
 */
#define IO_PORT2_FUNC_GPIO IO_PORTX_FUNC_GPIO
#define IO_PORT2_FUNC_PCI IO_PORTX_FUNC_PCI		// PCI data I/O
#define IO_PORT2_FUNC_PLIO IO_PORTX_FUNC_PLIO		// Extended LM

/*
 * Port 3
 */
#define IO_PORT3_FUNC_GPIO IO_PORTX_FUNC_GPIO
#define IO_PORT3_FUNC_SERDES IO_PORTX_FUNC_SERDES
#define IO_PORT3_FUNC_PLIO IO_PORTX_FUNC_PLIO

/*
 * Port 4
 */
#define IO_PORT4_FUNC_GPIO IO_PORTX_FUNC_GPIO
#define IO_PORT4_FUNC_SERDES IO_PORTX_FUNC_SERDES
#define IO_PORT4_FUNC_PLIO IO_PORTX_FUNC_PLIO		// Extended LM
#define IO_PORT4_FUNC_MII IO_PORTX_FUNC_MII

/*
 * Port 5
 */
#define IO_PORT5_FUNC_GPIO IO_PORTX_FUNC_GPIO
#define IO_PORT5_FUNC_GMII IO_PORTX_FUNC_GMII

/*
 * Port 6
 */
#define IO_PORT6_FUNC_GPIO IO_PORTX_FUNC_GPIO
#define IO_PORT6_FUNC_DDR IO_PORTX_FUNC_DDR

/*
 * Port 7
 */
#define IO_PORT7_FUNC_GPIO IO_PORTX_FUNC_GPIO
#define IO_PORT7_FUNC_DDR IO_PORTX_FUNC_DDR

/*
 * Port 8
 */
#define IO_PORT8_FUNC_GPIO IO_PORTX_FUNC_GPIO
#define IO_PORT8_FUNC_PCIX IO_PORTX_FUNC_PCIX
#define IO_PORT8_FUNC_PLIO IO_PORTX_FUNC_PLIO		// Extended LM
#define IO_PORT8_FUNC_MII IO_PORTX_FUNC_MII		// port 4 MII extension

/*
 * Port 9
 */
#define IO_PORT9_FUNC_USB2_0 IO_PORTX_FUNC_USB2_0

/*
 * FIFO
 */
#define IO_PORTX_INT_FIFO_TX_RESET (1 << 31)
#define IO_PORTX_INT_FIFO_RX_RESET (1 << 30)
#define IO_PORTX_INT_FIFO_TX_UF (1 << 15)
#define IO_PORTX_INT_FIFO_TX_WM (1 << 14)
#define IO_PORTX_INT_FIFO_RX_OF (1 << 13)
#define IO_PORTX_INT_FIFO_RX_WM (1 << 12)

#define IO_PORTX_FUNC_FIFO_TX_WM(n) ((n) << 16)
#define IO_PORTX_FUNC_FIFO_RX_WM(n) ((n) << 0)

/*
 * MII
 */
#define IO_PORTX_INT_MII_TX_ERR_SEND (1 << 18)
#define IO_PORTX_INT_MII_TX_HALT (1 << 17)
#define IO_PORTX_INT_MII_TX_START (1 << 16)
#define IO_PORTX_INT_MII_THRESHOLD (1 << 8)
#define IO_PORTX_INT_MII_RX_EOP (1 << 7)
#define IO_PORTX_INT_MII_RX_SFD (1 << 6)
#define IO_PORTX_INT_MII_RX_ERR (1 << 5)
#define IO_PORTX_INT_MII_TX_EOP (1 << 4)
#define IO_PORTX_INT_MII_COL (1 << 3)
#define IO_PORTX_INT_MII_CRS (1 << 2)
#define IO_PORTX_INT_MII_ODD_NIB_ERR (1 << 1)
#define IO_PORTX_INT_MII_FALSE_CARRIER (1 << 0)

/*
 * SerDes
 */
#define IO_PORTX_INT_SERDES_TXBUF_VALID (1 << 16)
#define IO_PORTX_INT_SERDES_RXERR (1 << 7)
#define IO_PORTX_INT_SERDES_RXEOP (1 << 6)
#define IO_PORTX_INT_SERDES_SYND (1 << 5)
#define IO_PORTX_INT_SERDES_TXBE (1 << 4)
#define IO_PORTX_INT_SERDES_TXEOP (1 << 3)
#define IO_PORTX_INT_SERDES_SXLP (1 << 2)
#define IO_PORTX_INT_SERDES_RXBF (1 << 1)
#define IO_PORTX_INT_SERDES_RXCRS (1 << 0)

#ifndef __ASSEMBLY__
struct ubicom32_io_port {
	volatile u32_t function;
	volatile u32_t gpio_ctl;
	volatile u32_t gpio_out;
	volatile u32_t gpio_in;
	volatile u32_t int_status;
	volatile u32_t int_mask;
	volatile u32_t int_set;
	volatile u32_t int_clr;
	volatile u32_t tx_fifo;
	volatile u32_t tx_fifo_hi;
	volatile u32_t rx_fifo;
	volatile u32_t rx_fifo_hi;
	volatile u32_t ctl0;
	volatile u32_t ctl1;
	volatile u32_t ctl2;
	volatile u32_t status0;
	volatile u32_t status1;
	volatile u32_t status2;
	volatile u32_t fifo_watermark;
	volatile u32_t fifo_level;
	volatile u32_t gpio_mask;
};

#define UBICOM32_IO_PORT(port) ((struct ubicom32_io_port *)((port)))
#endif

#ifndef __ASSEMBLY__
/*
 * ubicom32_set_interrupt()
 */
extern inline void ubicom32_set_interrupt(u8_t interrupt)
{
	u32_t ibit = INT_BIT_MASK(interrupt);

	if (INT_REG(interrupt) == INT_REG(INT_CHIP(0, 0))) {
		asm volatile (
			"move.4		"D(INT_SET(INT_CHIP(0, 0)))", %0\n\t"
			:
			: "r" (ibit)
		);

		return;
	}

	asm volatile (
		"move.4		"D(INT_SET(INT_CHIP(1, 0)))", %0\n\t"
		:
		: "r" (ibit)
	);
}

/*
 * ubicom32_clear_interrupt()
 */
extern inline void ubicom32_clear_interrupt(u8_t interrupt)
{
	u32_t ibit = INT_BIT_MASK(interrupt);

	if (INT_REG(interrupt) == INT_REG(INT_CHIP(0, 0))) {
		asm volatile (
			"move.4		"D(INT_CLR(INT_CHIP(0, 0)))", %0\n\t"
			:
			: "r" (ibit)
		);

		return;
	}

	asm volatile (
		"move.4		"D(INT_CLR(INT_CHIP(1, 0)))", %0\n\t"
		:
		: "r" (ibit)
	);
}

/*
 * ubicom32_enable_interrupt()
 */
extern inline void ubicom32_enable_interrupt(u8_t interrupt)
{
	u32_t ibit = INT_BIT_MASK(interrupt);

	if (INT_REG(interrupt) == INT_REG(INT_CHIP(0, 0))) {
		asm volatile (
			"or.4		"D(INT_MASK(INT_CHIP(0, 0)))", "D(INT_MASK(INT_CHIP(0, 0)))", %0\n\t"
			:
			: "d" (ibit)
		);

		return;
	}

	asm volatile (
		"or.4		"D(INT_MASK(INT_CHIP(1, 0)))", "D(INT_MASK(INT_CHIP(1, 0)))", %0\n\t"
		:
		: "d" (ibit)
	);
}

/*
 * ubicom32_disable_interrupt()
 */
extern inline void ubicom32_disable_interrupt(u8_t interrupt)
{
	u32_t ibit = ~INT_BIT_MASK(interrupt);

	if (INT_REG(interrupt) == INT_REG(INT_CHIP(0, 0))) {
		asm volatile (
			"and.4		"D(INT_MASK(INT_CHIP(0, 0)))", "D(INT_MASK(INT_CHIP(0, 0)))", %0\n\t"
			:
			: "d" (ibit)
		);

		return;
	}

	asm volatile (
		"and.4		"D(INT_MASK(INT_CHIP(1, 0)))", "D(INT_MASK(INT_CHIP(1, 0)))", %0\n\t"
		:
		: "d" (ibit)
	);
}

/*
 * ubicom32_enable_global_interrupts()
 */
extern inline void ubicom32_enable_global_interrupts(void)
{
	asm volatile(
		"bset		GLOBAL_CTRL, GLOBAL_CTRL, #%bit("D(GLOBAL_CTRL_INT_EN)")"
	);
}

/*
 * ubicom32_disable_global_interrupts()
 */
extern inline void ubicom32_disable_global_interrupts(void)
{
	asm volatile(
		"bclr		GLOBAL_CTRL, GLOBAL_CTRL, #%bit("D(GLOBAL_CTRL_INT_EN)")"
	);
}

/*
 * ubicom32_get_reset_reason()
 */
extern inline u32_t ubicom32_get_reset_reason(void)
{
	return *(u32_t *)(GENERAL_CFG_BASE + GEN_RESET_REASON);
}

/*
 * ubicom32_read_reg()
 */
extern inline u32_t ubicom32_read_reg(volatile void *reg)
{
	u32_t v;
	asm volatile (
		"move.4		%[dest], %[src]	\n\t"
		: [dest] "=r" (v)
		: [src] "m" (*(u32_t *)reg)
	);
	return v;
}

/*
 * ubicom32_write_reg()
 */
extern inline void ubicom32_write_reg(volatile void *reg, u32_t v)
{
	asm volatile (
		"move.4		%[dest], %[src]	\n\t"
		:
		: [src] "r" (v), [dest] "m" (*(u32_t *)reg)
	);
}

#endif /* __ASSEMBLY__ */
#endif /* _ASM_UBICOM32_IP5000_H */

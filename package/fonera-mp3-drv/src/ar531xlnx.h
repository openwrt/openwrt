/*
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file "COPYING" in the main directory of this archive
 * for more details.
 *
 * Copyright © 2003 Atheros Communications, Inc.,  All Rights Reserved.
 */

/*
 * This file contains definitions needed in order to compile
 * AR531X products for linux.  Definitions that are largely
 * AR531X-specific and independent of operating system belong
 * in ar531x.h rather than this file.
 */
#ifndef __AR531XLNX_H
#define __AR531XLNX_H
#include "ar531x.h"

#define AR531X_HIGH_PRIO                0x10
#define AR531X_MISC_IRQ_BASE		0x20
#define AR531X_GPIO_IRQ_BASE            0x30

/* Software's idea of interrupts handled by "CPU Interrupt Controller" */
#ifndef CONFIG_AR531X_COBRA
#define CONFIG_AR531X_COBRA 0
#endif

#if CONFIG_AR531X_COBRA
#define AR531X_IRQ_NONE		MIPS_CPU_IRQ_BASE+0
#define AR531X_IRQ_MISC_INTRS	MIPS_CPU_IRQ_BASE+2 /* C0_CAUSE: 0x0400 */
#define AR531X_IRQ_WLAN0_INTRS	MIPS_CPU_IRQ_BASE+3 /* C0_CAUSE: 0x0800 */
#define AR531X_IRQ_ENET0_INTRS	MIPS_CPU_IRQ_BASE+4 /* C0_CAUSE: 0x1000 */
#define AR531X_IRQ_LCBUS_PCI	MIPS_CPU_IRQ_BASE+6 /* C0_CAUSE: 0x4000 */
#define AR531X_IRQ_WLAN0_POLL	MIPS_CPU_IRQ_BASE+6 /* C0_CAUSE: 0x4000 */
#define AR531X_IRQ_CPU_CLOCK	MIPS_CPU_IRQ_BASE+7 /* C0_CAUSE: 0x8000 */
#else
#define AR531X_IRQ_NONE		MIPS_CPU_IRQ_BASE+0
#define AR531X_IRQ_WLAN0_INTRS	MIPS_CPU_IRQ_BASE+2 /* C0_CAUSE: 0x0400 */
#define AR531X_IRQ_ENET0_INTRS	MIPS_CPU_IRQ_BASE+3 /* C0_CAUSE: 0x0800 */
#define AR531X_IRQ_ENET1_INTRS	MIPS_CPU_IRQ_BASE+4 /* C0_CAUSE: 0x1000 */
#define AR531X_IRQ_WLAN1_INTRS	MIPS_CPU_IRQ_BASE+5 /* C0_CAUSE: 0x2000 */
#define AR531X_IRQ_MISC_INTRS	MIPS_CPU_IRQ_BASE+6 /* C0_CAUSE: 0x4000 */
#define AR531X_IRQ_CPU_CLOCK	MIPS_CPU_IRQ_BASE+7 /* C0_CAUSE: 0x8000 */
#endif

/* Miscellaneous interrupts, which share IP6 or IP2 */
#define AR531X_MISC_IRQ_NONE		AR531X_MISC_IRQ_BASE+0
#define AR531X_MISC_IRQ_TIMER		AR531X_MISC_IRQ_BASE+1
#define AR531X_MISC_IRQ_AHB_PROC	AR531X_MISC_IRQ_BASE+2
#define AR531X_MISC_IRQ_AHB_DMA		AR531X_MISC_IRQ_BASE+3
#define AR531X_MISC_IRQ_GPIO		AR531X_MISC_IRQ_BASE+4
#define AR531X_MISC_IRQ_UART0		AR531X_MISC_IRQ_BASE+5
#define AR531X_MISC_IRQ_UART0_DMA	AR531X_MISC_IRQ_BASE+6
#define AR531X_MISC_IRQ_WATCHDOG	AR531X_MISC_IRQ_BASE+7
#define AR531X_MISC_IRQ_LOCAL		AR531X_MISC_IRQ_BASE+8
#define AR531X_MISC_IRQ_COUNT		9

/* GPIO Interrupts [0..7], share AR531X_MISC_IRQ_GPIO */
#define AR531X_GPIO_IRQ_NONE            AR531X_MISC_IRQ_BASE+0
#define AR531X_GPIO_IRQ(n)              AR531X_MISC_IRQ_BASE+(n)+1
#define AR531X_GPIO_IRQ_COUNT           9

#define PHYS_TO_K1(physaddr) KSEG1ADDR(physaddr)
//#define PHYS_TO_K0(physaddr) KSEG0ADDR(physaddr)
#define UNMAPPED_TO_PHYS(vaddr)  PHYSADDR(vaddr)
#define IS_UNMAPPED_VADDR(vaddr) \
    ((KSEGX(vaddr) == KSEG0) || (KSEGX(vaddr) == KSEG1))

/* IOCTL commands for /proc/ar531x */
#define AR531X_CTRL_DO_BREAKPOINT       1
#define AR531X_CTRL_DO_MADWIFI          2

/*
 * Definitions for operating system portability.
 * These are vxWorks-->Linux translations.
 */
#define LOCAL static
#define BOOL int
#define TRUE 1
#define FALSE 0
#define UINT8 u8
#define UINT16 u16
#define UINT32 u32
#define PRINTF printk
#if /* DEBUG */ 1
#define DEBUG_PRINTF printk
#define printf printk
#define INLINE
#else
DEBUG_PRINTF while (0) printk
#define INLINE inline
#endif
#define sysUDelay(usecs) udelay(usecs)
#define sysMsDelay(msecs) mdelay(msecs)
typedef volatile UINT8 *VIRT_ADDR;
#define MALLOC(sz) kmalloc(sz, GFP_KERNEL)
#define MALLOC_NOSLEEP(sz) kmalloc(sz, GFP_ATOMIC)
#define FREE(ptr) kfree((void *)ptr)
#define BSP_BUG() do { printk("kernel BSP BUG at %s:%d!\n", __FILE__, __LINE__); *(int *)0=0; } while (0)
#define BSP_BUG_ON(condition) do { if (unlikely((condition)!=0)) BSP_BUG(); } while(0)
#define ASSERT(x) BSP_BUG_ON(!(x))

extern struct ar531x_boarddata *ar531x_board_configuration;
extern char *ar531x_radio_configuration;
extern char *enet_mac_address_get(int MACUnit);

extern void kgdbInit(void);
extern int kgdbEnabled(void);
extern void breakpoint(void);
extern int kgdbInterrupt(void);
extern unsigned int ar531x_cpu_frequency(void);
extern unsigned int ar531x_sys_frequency(void);

/* GPIO support */
extern struct irqaction spurious_gpio;
extern unsigned int gpioIntMask;
extern void ar531x_gpio_intr_init(int irq_base);
extern void ar531x_gpio_ctrl_output(int gpio);
extern void ar531x_gpio_ctrl_input(int gpio);
extern void ar531x_gpio_set(int gpio, int val);
extern int  ar531x_gpio_get(int gpio);
extern void ar531x_gpio_intr_enable(unsigned int irq);
extern void ar531x_gpio_intr_disable(unsigned int irq);

/* Watchdog Timer support */
extern int watchdog_start(unsigned int milliseconds);
extern int watchdog_stop(void);
extern int watchdog_is_enabled(void);
extern unsigned int watchdog_min_timer_reached(void);
extern void watchdog_notify_alive(void);

#define A_DATA_CACHE_INVAL(start, length) \
        dma_cache_inv((UINT32)(start),(length))

#define sysWbFlush() mb()

#define intDisable(x) cli()
#define intEnable(x) sti()

#endif   /* __AR531XLNX_H */

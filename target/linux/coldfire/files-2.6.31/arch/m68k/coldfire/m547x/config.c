/*
 *  linux/arch/m68k/coldfire/config.c
 *
 *  Copyright 2007-2009 Freescale Semiconductor, Inc. All Rights Reserved.
 *  Kurt Mahan kmahan@freescale.com
 *  Matt Waddel Matt.Waddel@freescale.com
 *  Shrek Wu b16972@freescale.com
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 */

#include <linux/module.h>
#include <linux/init.h>
#include <linux/string.h>
#include <linux/kernel.h>
#include <linux/console.h>
#include <linux/bootmem.h>
#include <linux/mm.h>
#include <linux/clockchips.h>
#include <asm/bootinfo.h>
#include <asm/machdep.h>
#include <asm/coldfire.h>
#include <asm/cfcache.h>
#include <asm/cacheflush.h>
#include <linux/io.h>
#include <asm/cfmmu.h>
#include <asm/setup.h>
#include <asm/irq.h>
#include <asm/traps.h>
#include <asm/movs.h>
#include <asm/movs.h>
#include <asm/page.h>
#include <asm/pgalloc.h>

#include <asm/mcfsim.h>

#define UBOOT_PCI
#include <asm/bootinfo.h>
#include <asm/m5485gpt.h>

extern int get_irq_list(struct seq_file *p, void *v);
extern char _text, _end;
extern char _etext, _edata, __init_begin, __init_end;
extern struct console mcfrs_console;
extern char m68k_command_line[CL_SIZE];
extern unsigned long availmem;

static int irq_enable[NR_IRQS];
unsigned long num_pages;

/* cf dma physical addresses */
unsigned long cf_dma_base;
unsigned long cf_dma_end;
unsigned long cf_dma_size;
EXPORT_SYMBOL(cf_dma_base);
EXPORT_SYMBOL(cf_dma_end);
EXPORT_SYMBOL(cf_dma_size);

/* ethernet mac addresses from uboot */
unsigned char uboot_enet0[6];
unsigned char uboot_enet1[6];

void coldfire_sort_memrec(void)
{
	int i, j;

	/* Sort the m68k_memory records by address */
	for (i = 0; i < m68k_num_memory; ++i) {
		for (j = i + 1; j < m68k_num_memory; ++j) {
			if (m68k_memory[i].addr > m68k_memory[j].addr) {
				struct mem_info tmp;
				tmp = m68k_memory[i];
				m68k_memory[i] = m68k_memory[j];
				m68k_memory[j] = tmp;
			}
		}
	}
	/* Trim off discontiguous bits */
	for (i = 1; i < m68k_num_memory; ++i) {
		if ((m68k_memory[i-1].addr + m68k_memory[i-1].size) !=
			m68k_memory[i].addr) {
			printk(KERN_DEBUG "m68k_parse_bootinfo: "
				"addr gap between 0x%lx & 0x%lx\n",
				m68k_memory[i-1].addr+m68k_memory[i-1].size,
				m68k_memory[i].addr);
			m68k_num_memory = i;
			break;
		}
	}
}

/*
 * UBoot Handler
 */
int __init uboot_commandline(char *bootargs)
{
	int len = 0, cmd_line_len;
	static struct uboot_record uboot_info;
	u32 offset = PAGE_OFFSET_RAW - PHYS_OFFSET;

	extern unsigned long uboot_info_stk;

	/* validate address */
	if ((uboot_info_stk < PAGE_OFFSET_RAW) ||
	    (uboot_info_stk >= (PAGE_OFFSET_RAW + CONFIG_SDRAM_SIZE)))
		return 0;

	/* Add offset to get post-remapped kernel memory location */
	uboot_info.bdi = (struct bd_info *)((*(u32 *)(uboot_info_stk))
							+ offset);
	uboot_info.initrd_start = (*(u32 *)(uboot_info_stk+4)) + offset;
	uboot_info.initrd_end = (*(u32 *)(uboot_info_stk+8)) + offset;
	uboot_info.cmd_line_start = (*(u32 *)(uboot_info_stk+12)) + offset;
	uboot_info.cmd_line_stop = (*(u32 *)(uboot_info_stk+16)) + offset;

	/* copy over mac addresses */
	memcpy(uboot_enet0, uboot_info.bdi->bi_enet0addr, 6);
	memcpy(uboot_enet1, uboot_info.bdi->bi_enet1addr, 6);

	/* copy command line */
	cmd_line_len = uboot_info.cmd_line_stop - uboot_info.cmd_line_start;
	if ((cmd_line_len > 0) && (cmd_line_len < CL_SIZE-1))
		len = (int)strncpy(bootargs, (char *)uboot_info.cmd_line_start,\
				   cmd_line_len);

	return len;
}

/*
 * This routine does things not done in the bootloader.
 */
#define DEFAULT_COMMAND_LINE \
	"debug root=/dev/nfs rw \
	nfsroot=172.27.155.1:/tftpboot/rigo/rootfs/ \
	ip=172.27.155.75:172.27.155.1"

asmlinkage void __init cf_early_init(void)
{
	struct bi_record *record = (struct bi_record *) &_end;

	extern char _end;

	SET_VBR((void *)MCF_RAMBAR0);

	/* Mask all interrupts */
	MCF_IMRL = 0xFFFFFFFF;
	MCF_IMRH = 0xFFFFFFFF;

	m68k_machtype = MACH_CFMMU;
	m68k_fputype = FPU_CFV4E;
	m68k_mmutype = MMU_CFV4E;
	m68k_cputype = CPU_CFV4E;

	m68k_num_memory = 0;
	m68k_memory[m68k_num_memory].addr = CONFIG_SDRAM_BASE;
	m68k_memory[m68k_num_memory++].size = CONFIG_SDRAM_SIZE;

	if (!uboot_commandline(m68k_command_line)) {
#if defined(CONFIG_BOOTPARAM)
		strncpy(m68k_command_line, CONFIG_BOOTPARAM_STRING, CL_SIZE-1);
#else
		strcpy(m68k_command_line, DEFAULT_COMMAND_LINE);
#endif
	}

#if defined(CONFIG_BLK_DEV_INITRD)
	/* add initrd image */
	record = (struct bi_record *) ((void *)record + record->size);
	record->tag = BI_RAMDISK;
	record->size =  sizeof(record->tag) + sizeof(record->size)
		+ sizeof(record->data[0]) + sizeof(record->data[1]);
#endif

	/* Mark end of tags. */
	record = (struct bi_record *) ((void *) record + record->size);
	record->tag = 0;
	record->data[0] = 0;
	record->data[1] = 0;
	record->size = sizeof(record->tag) + sizeof(record->size)
		+ sizeof(record->data[0]) + sizeof(record->data[1]);

	/* Invalidate caches via CACR */
	flush_bcache();
	cacr_set(CACHE_DISABLE_MODE);

	/* Turn on caches via CACR, enable EUSP */
	cacr_set(CACHE_INITIAL_MODE);

}

/* Assembler routines */
asmlinkage void buserr(void);
asmlinkage void trap(void);
asmlinkage void system_call(void);
asmlinkage void inthandler(void);

void __init coldfire_trap_init(void)
{
	int i = 0;
	e_vector *vectors;

	vectors = (e_vector *)MCF_RAMBAR0;
	/*
	 * There is a common trap handler and common interrupt
	 * handler that handle almost every vector. We treat
	 * the system call and bus error special, they get their
	 * own first level handlers.
	 */
	for (i = 3; (i <= 23); i++)
		vectors[i] = trap;
	for (i = 33; (i <= 63); i++)
		vectors[i] = trap;
	for (i = 24; (i <= 31); i++)
		vectors[i] = inthandler;
	for (i = 64; (i < 255); i++)
		vectors[i] = inthandler;

	vectors[255] = 0;
	vectors[2] = buserr;
	vectors[32] = system_call;
}

#ifndef CONFIG_GENERIC_CLOCKEVENTS
void coldfire_tick(void)
{
	/* Reset the ColdFire timer */
	MCF_SSR(0) = MCF_SSR_ST;
}

void __init coldfire_sched_init(irq_handler_t handler)
{
	int irq = ISC_SLTn(0);

	MCF_SCR(0) = 0;
	MCF_ICR(irq) = ILP_SLT0;
	request_irq(64 + irq, handler, IRQF_DISABLED, "ColdFire Timer 0", NULL);
	MCF_SLTCNT(0) = MCF_BUSCLK / HZ;
	MCF_SCR(0) |=  MCF_SCR_TEN | MCF_SCR_IEN | MCF_SCR_RUN;
}

unsigned long coldfire_gettimeoffset(void)
{
	volatile unsigned long trr, tcn, offset;
	trr = MCF_SLTCNT(0);
	tcn = MCF_SCNT(0);

	offset = (trr - tcn) * ((1000000 >> 3) / HZ) / (trr >> 3);
	if (MCF_SSR(0) & MCF_SSR_ST)
		offset += 1000000 / HZ;

	return offset;
}
#else
static unsigned long long sched_dtim_clk_val;

unsigned long long sched_clock(void)
{
	unsigned long flags;
	unsigned long long cycles;
	volatile unsigned long trr, tcn, offset;

	local_irq_save(flags);
	trr = MCF_SLTCNT(0);
	tcn = MCF_SCNT(0);
	offset = (trr - tcn);
	cycles = sched_dtim_clk_val;
	local_irq_restore(flags);

	return cycles + offset;
}

unsigned long long sys_dtim2_read(void)
{
	unsigned long flags;
	unsigned long long cycles;
	volatile unsigned long trr, tcn, offset;

	local_irq_save(flags);
	trr = MCF_SLTCNT(0);
	tcn = MCF_SCNT(0);
	offset = (trr - tcn);
	cycles = sched_dtim_clk_val;
	local_irq_restore(flags);

	return cycles + offset;
}

static irqreturn_t coldfire_dtim_clk_irq(int irq, void *dev)
{
	struct clock_event_device *evt =
		(struct clock_event_device *)dev;

	MCF_SSR(0) = MCF_SSR_ST;
	sched_dtim_clk_val +=  (MCF_BUSCLK) / HZ;;
	evt->event_handler(evt);
	return IRQ_HANDLED;
}

void sys_dtim2_init(struct clock_event_device *evt)
{
	int irq = ISC_SLTn(0);

	sched_dtim_clk_val = 0;
	MCF_SCR(0) = 0;
	MCF_ICR(irq) = ILP_SLT0;
	request_irq(64 + irq, coldfire_dtim_clk_irq, IRQF_DISABLED,
		"ColdFire Timer 0", (void *)evt);
	MCF_SLTCNT(0) = MCF_BUSCLK / HZ;
	MCF_SCR(0) |=  MCF_SCR_TEN | MCF_SCR_IEN | MCF_SCR_RUN;
}
#endif

void coldfire_reboot(void)
{
	/* disable interrupts and enable the watchdog */
	printk(KERN_INFO "Rebooting\n");
	asm("movew #0x2700, %sr\n");
	MCF_GPT_GMS0 = MCF_GPT_GMS_WDEN | MCF_GPT_GMS_CE | MCF_GPT_GMS_TMS(4);
}

static void coldfire_get_model(char *model)
{
	sprintf(model, "Version 4 ColdFire");
}

static void __init
coldfire_bootmem_alloc(unsigned long memory_start, unsigned long memory_end)
{
	unsigned long base_pfn;

	/* compute total pages in system */
	num_pages = PAGE_ALIGN(memory_end - PAGE_OFFSET) >> PAGE_SHIFT;

	/* align start/end to page boundries */
	memory_start = PAGE_ALIGN(memory_start);
	memory_end = memory_end & PAGE_MASK;

	/* page numbers */
	base_pfn = __pa(PAGE_OFFSET) >> PAGE_SHIFT;
	min_low_pfn = __pa(memory_start) >> PAGE_SHIFT;
	max_low_pfn = __pa(memory_end) >> PAGE_SHIFT;

	high_memory = (void *)memory_end;
	availmem = memory_start;

	/* setup bootmem data */
	m68k_setup_node(0);
	availmem += init_bootmem_node(NODE_DATA(0), min_low_pfn,
		base_pfn, max_low_pfn);
	availmem = PAGE_ALIGN(availmem);

	printk(KERN_INFO "** availmem=0x%lx  pa(am)=0x%lx\n",
			availmem, __pa(availmem));
	printk(KERN_INFO "** mstart=0x%lx  mend=0x%lx\n",
			memory_start, memory_end);
	printk(KERN_INFO "bpfn=0x%lx minpfn=0x%lx maxpfn=0x%lx\n",
			base_pfn, min_low_pfn, max_low_pfn);

	/* turn over physram */
	free_bootmem(__pa(availmem), memory_end - (availmem));

	/* configure physical dma area */
	cf_dma_base = __pa(PAGE_ALIGN(memory_start));
	cf_dma_size = CONFIG_DMA_SIZE;
	cf_dma_end = CONFIG_SDRAM_BASE + cf_dma_size - 1;

	printk(KERN_INFO "dma: phys base=0x%lx  phys end=0x%lx  virt base=0x%x\n",
	       cf_dma_base, cf_dma_end, CONFIG_DMA_BASE);

	printk(KERN_INFO "mdma=0x%x  pa(mdma)=0x%lx\n",
			MAX_DMA_ADDRESS, __pa(MAX_DMA_ADDRESS));
}

void __init config_coldfire(void)
{
	unsigned long endmem, startmem;
	int i;

	/*
	 * Calculate endmem from m68k_memory, assume all are contiguous
	 */
	startmem = ((((int) &_end) + (PAGE_SIZE - 1)) & PAGE_MASK);
	endmem = PAGE_OFFSET;
	for (i = 0; i < m68k_num_memory; ++i)
		endmem += m68k_memory[i].size;

	printk(KERN_INFO "starting up linux startmem 0x%lx, endmem 0x%lx, \
		size %luMB\n", startmem,  endmem, (endmem - startmem) >> 20);

	memset(irq_enable, 0, sizeof(irq_enable));

	/*
	 * Setup coldfire mach-specific handlers
	 */
	mach_max_dma_address 	= 0xffffffff;
#ifndef CONFIG_GENERIC_CLOCKEVENTS
	mach_sched_init 	= coldfire_sched_init;
	mach_tick		= coldfire_tick;
	mach_gettimeoffset 	= coldfire_gettimeoffset;
#endif
	mach_reset 		= coldfire_reboot;
/*	mach_hwclk 		= coldfire_hwclk; to be done */
	mach_get_model 		= coldfire_get_model;

	coldfire_bootmem_alloc(startmem, endmem-1);

	/*
	 * initrd setup
	 */
/* #ifdef CONFIG_BLK_DEV_INITRD
	if (m68k_ramdisk.size)  {
		reserve_bootmem (__pa(m68k_ramdisk.addr), m68k_ramdisk.size);
		initrd_start = (unsigned long) m68k_ramdisk.addr;
		initrd_end = initrd_start + m68k_ramdisk.size;
		printk (KERN_DEBUG "initrd: %08lx - %08lx\n", initrd_start,
			initrd_end);
	}
#endif */

#if defined(CONFIG_DUMMY_CONSOLE) || defined(CONFIG_FRAMEBUFFER_CONSOLE)
	conswitchp = &dummy_con;
#endif

#if defined(CONFIG_SERIAL_COLDFIRE)
	/*
	 * This causes trouble when it is re-registered later.
	 * Currently this is fixed by conditionally commenting
	 * out the register_console in mcf_serial.c
	 */
	register_console(&mcfrs_console);
#endif
}

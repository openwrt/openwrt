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

/*
 * Platform devices for Atheros SoCs
 */

#include <linux/autoconf.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/types.h>
#include <linux/string.h>
#include <linux/platform_device.h>
#include <linux/kernel.h>
#include <linux/serial.h>
#include <linux/serial_core.h>
#include <asm/bootinfo.h>
#include <asm/irq_cpu.h>
#include <asm/io.h>
#include "ar531x.h"

char *board_config, *radio_config;

static u8 *find_board_config(char *flash_limit)
{
	char *addr;
	int found = 0;

	for (addr = (char *) (flash_limit - 0x1000);
		addr >= (char *) (flash_limit - 0x30000);
		addr -= 0x1000) {

		if ( *(int *)addr == 0x35333131) {
			/* config magic found */
			found = 1;
			break;
		}
	}

	if (!found) {
		printk("WARNING: No board configuration data found!\n");
		addr = NULL;
	}
	
	return addr;
}

static u8 *find_radio_config(char *flash_limit, char *board_config)
{
	int dataFound;
	u32 radio_config;
	
	/* 
	 * Now find the start of Radio Configuration data, using heuristics:
	 * Search forward from Board Configuration data by 0x1000 bytes
	 * at a time until we find non-0xffffffff.
	 */
	dataFound = 0;
	for (radio_config = (u32) board_config + 0x1000;
	     (radio_config < (u32) flash_limit);
	     radio_config += 0x1000) {
		if (*(int *)radio_config != 0xffffffff) {
			dataFound = 1;
			break;
		}
	}

#ifdef CONFIG_ATHEROS_AR5315
	if (!dataFound) { /* AR2316 relocates radio config to new location */
	    for (radio_config = (u32) board_config + 0xf8;
	     	(radio_config < (u32) flash_limit - 0x1000 + 0xf8);
			 radio_config += 0x1000) {
			if (*(int *)radio_config != 0xffffffff) {
				dataFound = 1;
				break;
			}
	    }
	}
#endif

	if (!dataFound) {
		printk("Could not find Radio Configuration data\n");
		radio_config = 0;
	}

	return (u8 *) radio_config;
}

int __init ar531x_find_config(char *flash_limit)
{
	unsigned int rcfg_size;
	char *bcfg, *rcfg;

	/* Copy the board and radio data to RAM, because with the new
	 * spiflash driver, accessing the mapped memory directly is no
	 * longer safe */

	bcfg = find_board_config(flash_limit);
	if (!bcfg)
		return -ENODEV;

	board_config = kzalloc(BOARD_CONFIG_BUFSZ, GFP_KERNEL);
	memcpy(board_config, bcfg, 0x100);

	/* Radio config starts 0x100 bytes after board config, regardless
	 * of what the physical layout on the flash chip looks like */

	rcfg = find_radio_config(flash_limit, bcfg);
	if (!rcfg)
		return -ENODEV;

	radio_config = board_config + 0x100 + ((rcfg - bcfg) & 0xfff);
	printk("Radio config found at offset 0x%x(0x%x)\n", rcfg - bcfg, radio_config - board_config);
	rcfg_size = BOARD_CONFIG_BUFSZ - ((rcfg - bcfg) & (BOARD_CONFIG_BUFSZ - 1));
	memcpy(radio_config, rcfg, rcfg_size);
	
	return 0;
}

void __init serial_setup(unsigned long mapbase, unsigned int uartclk)
{
	struct uart_port s;
	
	memset(&s, 0, sizeof(s));

	s.flags = UPF_BOOT_AUTOCONF | UPF_SKIP_TEST;
	s.iotype = UPIO_MEM;
	s.irq = AR531X_MISC_IRQ_UART0;
	s.regshift = 2;
	s.mapbase = mapbase;
	s.uartclk = uartclk;
	s.membase = (void __iomem *)s.mapbase;

	early_serial_setup(&s);
}

void __init plat_mem_setup(void)
{
	DO_AR5312(ar5312_plat_setup();)
	DO_AR5315(ar5315_plat_setup();)

	/* Disable data watchpoints */
	write_c0_watchlo0(0);
}

const char *get_system_type(void)
{
	switch (mips_machtype) {
#ifdef CONFIG_ATHEROS_AR5312
	case MACH_ATHEROS_AR5312:
		return "Atheros AR5312";

	case MACH_ATHEROS_AR2312:
		return "Atheros AR2312";
		
	case MACH_ATHEROS_AR2313:
		return "Atheros AR2313";
#endif
#ifdef CONFIG_ATHEROS_AR5315
	case MACH_ATHEROS_AR2315:
		return "Atheros AR2315";
	case MACH_ATHEROS_AR2316:
		return "Atheros AR2316";
	case MACH_ATHEROS_AR2317:
		return "Atheros AR2317";
	case MACH_ATHEROS_AR2318:
		return "Atheros AR2318";
#endif
	}
	return "Atheros (unknown)";
}

void __init plat_timer_setup(struct irqaction *irq)
{
	unsigned int count;

	/* Usually irq is timer_irqaction (timer_interrupt) */
	setup_irq(AR531X_IRQ_CPU_CLOCK, irq);

	/* to generate the first CPU timer interrupt */
	count = read_c0_count();
	write_c0_compare(count + 1000);
}

asmlinkage void plat_irq_dispatch(void)
{
	DO_AR5312(ar5312_irq_dispatch();)
	DO_AR5315(ar5315_irq_dispatch();)
}

void __init arch_init_irq(void)
{
	clear_c0_status(ST0_IM);
	mips_cpu_irq_init();

	/* Initialize interrupt controllers */
	DO_AR5312(ar5312_misc_intr_init(AR531X_MISC_IRQ_BASE);)
	DO_AR5315(ar5315_misc_intr_init(AR531X_MISC_IRQ_BASE);)
}

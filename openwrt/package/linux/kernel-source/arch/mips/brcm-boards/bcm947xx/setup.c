/*
 * Generic setup routines for Broadcom MIPS boards
 *
 * Copyright 2004, Broadcom Corporation
 * All Rights Reserved.
 * 
 * THIS SOFTWARE IS OFFERED "AS IS", AND BROADCOM GRANTS NO WARRANTIES OF ANY
 * KIND, EXPRESS OR IMPLIED, BY STATUTE, COMMUNICATION OR OTHERWISE. BROADCOM
 * SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A SPECIFIC PURPOSE OR NONINFRINGEMENT CONCERNING THIS SOFTWARE.
 *
 * $Id$
 */

#include <linux/config.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/serialP.h>
#include <linux/ide.h>
#include <asm/bootinfo.h>
#include <asm/time.h>
#include <asm/reboot.h>

#ifdef CONFIG_MTD_PARTITIONS
#include <linux/mtd/mtd.h>
#include <linux/mtd/partitions.h>
#endif

#include <typedefs.h>
#include <bcmutils.h>
#include <bcmnvram.h>
#include <sbmips.h>
#include <sbutils.h>
#include <trxhdr.h>

extern void bcm947xx_time_init(void);
extern void bcm947xx_timer_setup(struct irqaction *irq);

#ifdef CONFIG_REMOTE_DEBUG
extern void set_debug_traps(void);
extern void rs_kgdb_hook(struct serial_state *);
extern void breakpoint(void);
#endif

#if defined(CONFIG_BLK_DEV_IDE) || defined(CONFIG_BLK_DEV_IDE_MODULE)
extern struct ide_ops std_ide_ops;
#endif

/* Global SB handle */
void *bcm947xx_sbh = NULL;
spinlock_t bcm947xx_sbh_lock = SPIN_LOCK_UNLOCKED;
EXPORT_SYMBOL(bcm947xx_sbh);
EXPORT_SYMBOL(bcm947xx_sbh_lock);

/* Convenience */
#define sbh bcm947xx_sbh
#define sbh_lock bcm947xx_sbh_lock

/* Kernel command line */
char arcs_cmdline[CL_SIZE] __initdata = CONFIG_CMDLINE;

void
bcm947xx_machine_restart(char *command)
{
	printk("Please stand by while rebooting the system...\n");

	/* Set the watchdog timer to reset immediately */
	__cli();
	sb_watchdog(sbh, 1);
	while (1);
}

void
bcm947xx_machine_halt(void)
{
	printk("System halted\n");

	/* Disable interrupts and watchdog and spin forever */
	__cli();
	sb_watchdog(sbh, 0);
	while (1);
}

#ifdef CONFIG_SERIAL

static struct serial_struct rs = {
	line: 0,
	flags: ASYNC_BOOT_AUTOCONF,
	io_type: SERIAL_IO_MEM,
};

static void __init
serial_add(void *regs, uint irq, uint baud_base, uint reg_shift)
{
	rs.iomem_base = regs;
	rs.irq = irq + 2;
	rs.baud_base = baud_base / 16;
	rs.iomem_reg_shift = reg_shift;

	early_serial_setup(&rs);

	rs.line++;
}

static void __init
serial_setup(void *sbh)
{
	sb_serial_init(sbh, serial_add);

#ifdef CONFIG_REMOTE_DEBUG
	/* Use the last port for kernel debugging */
	if (rs.iomem_base)
		rs_kgdb_hook(&rs);
#endif
}

#endif /* CONFIG_SERIAL */

void __init
brcm_setup(void)
{
	char *value;

	/* Get global SB handle */
	sbh = sb_kattach();

	/* Initialize clocks and interrupts */
	sb_mips_init(sbh);

#ifdef CONFIG_SERIAL
	/* Initialize UARTs */
	serial_setup(sbh);
#endif

#if defined(CONFIG_BLK_DEV_IDE) || defined(CONFIG_BLK_DEV_IDE_MODULE)
	ide_ops = &std_ide_ops;
#endif

	/* Override default command line arguments */
	value = nvram_get("kernel_args");
	if (value && strlen(value) && strncmp(value, "empty", 5))
		strncpy(arcs_cmdline, value, sizeof(arcs_cmdline));


	/* Generic setup */
	_machine_restart = bcm947xx_machine_restart;
	_machine_halt = bcm947xx_machine_halt;
	_machine_power_off = bcm947xx_machine_halt;

	board_time_init = bcm947xx_time_init;
	board_timer_setup = bcm947xx_timer_setup;
}

const char *
get_system_type(void)
{
	return "Broadcom BCM947XX";
}

void __init
bus_error_init(void)
{
}

#ifdef CONFIG_MTD_PARTITIONS

static struct mtd_partition bcm947xx_parts[] = {
	{ name: "pmon",	offset: 0, size: 0, mask_flags: MTD_WRITEABLE, },
	{ name: "linux", offset: 0, size: 0, },
	{ name: "rootfs", offset: 0, size: 0, },
	{ name: "nvram", offset: 0, size: 0, },
	{ name: "OpenWrt", offset: 0, size: 0, },
	{ name: NULL, },
};

static int __init
find_root(struct mtd_info *mtd, size_t size, struct mtd_partition *part)
{
	struct trx_header *trx;
	unsigned char buf[512];
	int off;
	size_t len;

	trx = (struct trx_header *) buf;

	for (off = (256*1024); off < size; off += mtd->erasesize) {
		memset(buf, 0xe5, sizeof(buf));

		/*
		 * Read into buffer 
		 */
		if (MTD_READ(mtd, off, sizeof(buf), &len, buf) ||
		    len != sizeof(buf))
			continue;

		/* found a TRX header */
		if (le32_to_cpu(trx->magic) == TRX_MAGIC) {
			part->offset = le32_to_cpu(trx->offsets[1]);
			part->size = le32_to_cpu(trx->len); 

			part->size -= part->offset;
			part->offset += off;

			goto done;
		}
	}

	printk(KERN_NOTICE
	       "%s: Couldn't find root filesystem\n",
	       mtd->name);
	return -1;

 done:
		return part->size;
}

struct mtd_partition * __init
init_mtd_partitions(struct mtd_info *mtd, size_t size)
{

	/* boot loader */
	bcm947xx_parts[0].offset=0;
	bcm947xx_parts[0].size=256*1024;

	/* nvram */
	bcm947xx_parts[3].offset = size - ROUNDUP(NVRAM_SPACE, mtd->erasesize);
	bcm947xx_parts[3].size   = size - bcm947xx_parts[3].offset;

	/* Size linux (kernel and rootfs) */
	bcm947xx_parts[1].offset = bcm947xx_parts[0].size;
	bcm947xx_parts[1].size   = bcm947xx_parts[3].offset - bcm947xx_parts[1].offset;

	/* Find and size rootfs */
	if (find_root(mtd,size,&bcm947xx_parts[2])==0) {
		/* entirely jffs2 */
		bcm947xx_parts[2].size = bcm947xx_parts[3].offset - bcm947xx_parts[2].offset; 
		bcm947xx_parts[4].name = NULL;
	} else {
		/* legacy setup */
		/* calculate leftover flash, and assign it to the jffs2 partition */
		bcm947xx_parts[4].offset = bcm947xx_parts[2].offset + bcm947xx_parts[2].size;
		bcm947xx_parts[4].offset = ROUNDUP(bcm947xx_parts[4].offset, mtd->erasesize);
		bcm947xx_parts[4].size = bcm947xx_parts[3].offset - bcm947xx_parts[4].offset;
	}

	return bcm947xx_parts;
}

EXPORT_SYMBOL(init_mtd_partitions);

#endif

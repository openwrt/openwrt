/*
 * setup.c - boot time setup code
 */

#include <linux/init.h>
#include <linux/mm.h>
#include <linux/sched.h>
#include <linux/irq.h>
#include <linux/ioport.h>
#include <linux/pm.h>
#include <asm/bootinfo.h>
#include <asm/mipsregs.h>
#include <asm/pgtable.h>
#include <asm/reboot.h>
#include <asm/addrspace.h>     /* for KSEG1ADDR() */
#include <asm/time.h>
#include <asm/io.h>
#include <asm/rc32434/rc32434.h>
#include <asm/rc32434/pci.h>

#ifdef CONFIG_PCI
extern void rc32434_time_init(void);
extern int __init rc32434_pcibridge_init(void);
#endif

#define epldMask ((volatile unsigned char *)0xB900000d)

static void rb_machine_restart(char *command)
{
	/* just jump to the reset vector */
	* (volatile unsigned *) KSEG1ADDR(0x18008000) = 0x80000001;
	((void (*)(void))KSEG1ADDR(0x1FC00000u))();
}

static void rb_machine_halt(void)
{
	for(;;) continue;
}

#ifdef CONFIG_CPU_HAS_WB
void (*__wbflush) (void);

static void rb_write_buffer_flush(void)
{
	__asm__ __volatile__
	    ("sync\n\t" "nop\n\t" "loop: bc0f loop\n\t" "nop\n\t");
}
#endif

void __init plat_mem_setup(void)
{
	unsigned int pciCntlVal;

	board_time_init = rc32434_time_init;

#ifdef CONFIG_CPU_HAS_WB
	__wbflush = rb_write_buffer_flush;
#endif
	_machine_restart = rb_machine_restart;
	_machine_halt = rb_machine_halt;
	/*_machine_power_off = rb_machine_power_halt;*/
	pm_power_off = rb_machine_halt;

	set_io_port_base(KSEG1);

	pciCntlVal=rc32434_pci->pcic;
	pciCntlVal &= 0xFFFFFF7;
	rc32434_pci->pcic = pciCntlVal;

#ifdef CONFIG_PCI
	/* Enable PCI interrupts in EPLD Mask register */
	*epldMask = 0x0;
	*(epldMask + 1) = 0x0;
#endif
	write_c0_wired(0);
}

const char *get_system_type(void)
{
	return "MIPS RB500";
}

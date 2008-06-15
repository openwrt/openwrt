/*
 *  Generic setup routines for Broadcom MIPS boards
 *
 *  Copyright (C) 2005 Felix Fietkau <nbd@openwrt.org>
 *
 *  This program is free software; you can redistribute  it and/or modify it
 *  under  the terms of  the GNU General  Public License as published by the
 *  Free Software Foundation;  either version 2 of the  License, or (at your
 *  option) any later version.
 *
 *  THIS  SOFTWARE  IS PROVIDED   ``AS  IS'' AND   ANY  EXPRESS OR IMPLIED
 *  WARRANTIES,   INCLUDING, BUT NOT  LIMITED  TO, THE IMPLIED WARRANTIES OF
 *  MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN
 *  NO  EVENT  SHALL   THE AUTHOR  BE    LIABLE FOR ANY   DIRECT, INDIRECT,
 *  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 *  NOT LIMITED   TO, PROCUREMENT OF  SUBSTITUTE GOODS  OR SERVICES; LOSS OF
 *  USE, DATA,  OR PROFITS; OR  BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 *  ANY THEORY OF LIABILITY, WHETHER IN  CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 *  THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *  You should have received a copy of the  GNU General Public License along
 *  with this program; if not, write  to the Free Software Foundation, Inc.,
 *  675 Mass Ave, Cambridge, MA 02139, USA.
 *
 *
 * Copyright 2005, Broadcom Corporation
 * All Rights Reserved.
 * 
 * THIS SOFTWARE IS OFFERED "AS IS", AND BROADCOM GRANTS NO WARRANTIES OF ANY
 * KIND, EXPRESS OR IMPLIED, BY STATUTE, COMMUNICATION OR OTHERWISE. BROADCOM
 * SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A SPECIFIC PURPOSE OR NONINFRINGEMENT CONCERNING THIS SOFTWARE.
 *
 */

#include <linux/config.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/serialP.h>
#include <linux/ide.h>
#include <asm/bootinfo.h>
#include <asm/cpu.h>
#include <asm/time.h>
#include <asm/reboot.h>

#include <typedefs.h>
#include <osl.h>
#include <sbutils.h>
#include <bcmnvram.h>
#include <bcmdevs.h>
#include <sbhndmips.h>
#include <hndmips.h>
#include <trxhdr.h>

/* Virtual IRQ base, after last hw IRQ */
#define SBMIPS_VIRTIRQ_BASE 6

/* # IRQs, hw and sw IRQs */
#define SBMIPS_NUMIRQS  8

/* Global SB handle */
sb_t *bcm947xx_sbh = NULL;
spinlock_t bcm947xx_sbh_lock = SPIN_LOCK_UNLOCKED;

/* Convenience */
#define sbh bcm947xx_sbh
#define sbh_lock bcm947xx_sbh_lock

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

/* Kernel command line */
char arcs_cmdline[CL_SIZE] __initdata = CONFIG_CMDLINE;
extern void sb_serial_init(sb_t *sbh, void (*add)(void *regs, uint irq, uint baud_base, uint reg_shift));

void
bcm947xx_machine_restart(char *command)
{
	printk("Please stand by while rebooting the system...\n");

	if (sb_chip(sbh) == BCM4785_CHIP_ID)
		MTC0(C0_BROADCOM, 4, (1 << 22));

	/* Set the watchdog timer to reset immediately */
	__cli();
	sb_watchdog(sbh, 1);

	if (sb_chip(sbh) == BCM4785_CHIP_ID) {
		__asm__ __volatile__(
			".set\tmips3\n\t"
			"sync\n\t"
			"wait\n\t"
			".set\tmips0");
	}

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

static int ser_line = 0;

typedef struct {
        void *regs;
        uint irq;
        uint baud_base;
        uint reg_shift;
} serial_port;

static serial_port ports[4];
static int num_ports = 0;

static void
serial_add(void *regs, uint irq, uint baud_base, uint reg_shift)
{
        ports[num_ports].regs = regs;
        ports[num_ports].irq = irq;
        ports[num_ports].baud_base = baud_base;
        ports[num_ports].reg_shift = reg_shift;
        num_ports++;
}

static void
do_serial_add(serial_port *port)
{
        void *regs;
        uint irq;
        uint baud_base;
        uint reg_shift;
        struct serial_struct s;
        
        regs = port->regs;
        irq = port->irq;
        baud_base = port->baud_base;
        reg_shift = port->reg_shift;

        memset(&s, 0, sizeof(s));

        s.line = ser_line++;
        s.iomem_base = regs;
        s.irq = irq + 2;
        s.baud_base = baud_base / 16;
        s.flags = ASYNC_BOOT_AUTOCONF;
        s.io_type = SERIAL_IO_MEM;
        s.iomem_reg_shift = reg_shift;

        if (early_serial_setup(&s) != 0) {
                printk(KERN_ERR "Serial setup failed!\n");
        }
}

#endif /* CONFIG_SERIAL */

void __init
brcm_setup(void)
{
	char *s;
	int i;
	char *value;

	/* Get global SB handle */
	sbh = sb_kattach(SB_OSH);

	/* Initialize clocks and interrupts */
	sb_mips_init(sbh, SBMIPS_VIRTIRQ_BASE);

	if (BCM330X(current_cpu_data.processor_id) &&
		(read_c0_diag() & BRCM_PFC_AVAIL)) {
		/* 
		 * Now that the sbh is inited set the  proper PFC value 
		 */	
		printk("Setting the PFC to its default value\n");
		enable_pfc(PFC_AUTO);
	}


#ifdef CONFIG_SERIAL
	sb_serial_init(sbh, serial_add);

	/* reverse serial ports if nvram variable starts with console=ttyS1 */
	/* Initialize UARTs */
	s = nvram_get("kernel_args");
	if (!s) s = "";
	if (!strncmp(s, "console=ttyS1", 13)) {
		for (i = num_ports; i; i--)
			do_serial_add(&ports[i - 1]);
	} else {
		for (i = 0; i < num_ports; i++)
			do_serial_add(&ports[i]);
	}
#endif

#if defined(CONFIG_BLK_DEV_IDE) || defined(CONFIG_BLK_DEV_IDE_MODULE)
	ide_ops = &std_ide_ops;
#endif

	/* Override default command line arguments */
	value = nvram_get("kernel_cmdline");
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
	static char s[32];

	if (bcm947xx_sbh) {
		sprintf(s, "Broadcom BCM%X chip rev %d", sb_chip(bcm947xx_sbh),
			sb_chiprev(bcm947xx_sbh));
		return s;
	}
	else
		return "Broadcom BCM947XX";
}

void __init
bus_error_init(void)
{
}


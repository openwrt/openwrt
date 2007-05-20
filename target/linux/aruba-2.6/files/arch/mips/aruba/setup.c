/**************************************************************************
 *
 *  BRIEF MODULE DESCRIPTION
 *     setup routines for IDT EB434 boards
 *
 *  Copyright 2004 IDT Inc. (rischelp@idt.com)
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
 **************************************************************************
 * May 2004 rkt, neb
 *
 * Initial Release
 *
 * 
 *
 **************************************************************************
 */

#include <linux/init.h>
#include <linux/module.h>
#include <linux/mm.h>
#include <linux/sched.h>
#include <linux/irq.h>
#include <asm/bootinfo.h>
#include <asm/io.h>
#include <linux/ioport.h>
#include <asm/mipsregs.h>
#include <asm/pgtable.h>
#include <asm/reboot.h>
#include <asm/addrspace.h>	/* for KSEG1ADDR() */
#include <asm/idt-boards/rc32434/rc32434.h>
#include <linux/pm.h>

extern char *__init prom_getcmdline(void);

extern void (*board_time_init) (void);
extern void aruba_time_init(void);
extern void aruba_reset(void);

#define epldMask ((volatile unsigned char *)0xB900000d)

static void aruba_machine_restart(char *command)
{
	switch (mips_machtype) {
		case MACH_ARUBA_AP70:
			*(volatile u32 *)KSEG1ADDR(0x18008000) = 0x80000001;
			break;
		case MACH_ARUBA_AP65:
		case MACH_ARUBA_AP60:
		default:
			/* Reset*/
			*((volatile u32 *)KSEG1ADDR(0x1c003020)) = 0x00080350; // reset everything in sight
			udelay(100);
			*((volatile u32 *)KSEG1ADDR(0x1c003020)) = 0; // reset everything in sight
			udelay(100);
			*((volatile u32 *)KSEG1ADDR(0x1c003020)) = 0x3; // cold reset the cpu & system
			break;
	}
}

static void aruba_machine_halt(void)
{
	for (;;) continue;
}

extern char * getenv(char *e);
extern void unlock_ap60_70_flash(void);

void __init plat_mem_setup(void)
{
	board_time_init = aruba_time_init;

	_machine_restart = aruba_machine_restart;
	_machine_halt = aruba_machine_halt;
	pm_power_off = aruba_machine_halt;

	set_io_port_base(KSEG1);

	/* Enable PCI interrupts in EPLD Mask register */
	*epldMask = 0x0;
	*(epldMask + 1) = 0x0;

	write_c0_wired(0);
	unlock_ap60_70_flash();

	printk("BOARD - %s\n",getenv("boardname"));
}

int page_is_ram(unsigned long pagenr)
{
	return 1;
}

const char *get_system_type(void)
{
	switch (mips_machtype) {
		case MACH_ARUBA_AP70:
			return "Aruba AP70";
		case MACH_ARUBA_AP65:
			return "Aruba AP65";
		case MACH_ARUBA_AP60:
			return "Aruba AP60/AP61";
		default:
			return "Aruba UNKNOWN";
	}
}

EXPORT_SYMBOL(get_system_type);

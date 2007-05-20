/**************************************************************************
 *
 *  BRIEF MODULE DESCRIPTION
 *     prom interface routines
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

#include <linux/autoconf.h>
#include <linux/init.h>
#include <linux/mm.h>
#include <linux/module.h>
#include <linux/string.h>
#include <linux/console.h>
#include <asm/bootinfo.h>
#include <linux/bootmem.h>
#include <linux/ioport.h>
#include <linux/serial.h>
#include <linux/serialP.h>
#include <asm/serial.h>
#include <linux/ioport.h>

unsigned int idt_cpu_freq;
EXPORT_SYMBOL(idt_cpu_freq);

unsigned int arch_has_pci=0;

/* Kernel Boot parameters */
static unsigned char bootparm[] = 
	"init=/etc/preinit noinitrd "
	"mtdparts=physmap-flash.0:3520k@0x080000(kernel),2752k@0x140000(rootfs),8k@0x3f8000(NVRAM) "
	"console=ttyS0,9600 root=/dev/mtdblock1 rootfstype=squashfs,jffs2 ";

extern unsigned long mips_machgroup;
extern unsigned long mips_machtype;

extern void setup_serial_port(void);
extern char * getenv(char *e);

/* IDT 79EB434 memory map -- we really should be auto sizing it */
#define RAM_SIZE        32*1024*1024

char *__init prom_getcmdline(void)
{
	return &(arcs_cmdline[0]);
}

void __init prom_init(void)
{
	char *boardname;
	sprintf(arcs_cmdline, "%s", bootparm);

	/* set our arch type */
	mips_machgroup = MACH_GROUP_ARUBA;
	mips_machtype = MACH_ARUBA_UNKNOWN;

	boardname=getenv("boardname");

	if (!strcmp(boardname,"Muscat")) {
		mips_machtype = MACH_ARUBA_AP70;
		idt_cpu_freq = 133000000;
		arch_has_pci=1;
	} else if (!strcmp(boardname,"Mataro")) {
		mips_machtype = MACH_ARUBA_AP65;
		idt_cpu_freq = 110000000;
	} else if (!strcmp(boardname,"Merlot")) {
		mips_machtype = MACH_ARUBA_AP60;
		idt_cpu_freq = 90000000;
	}

	/* turn on the console */
	setup_serial_port();

	/*
	 * give all RAM to boot allocator,
	 * except where the kernel was loaded
	 */
	add_memory_region(0,RAM_SIZE,BOOT_MEM_RAM);
}

void prom_free_prom_memory(void)
{
	printk("stubbed prom_free_prom_memory()\n");
}

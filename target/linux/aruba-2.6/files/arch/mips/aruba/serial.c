/**************************************************************************
 *
 *  BRIEF MODULE DESCRIPTION
 *     Serial port initialisation.
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
#include <linux/sched.h>
#include <linux/pci.h>
#include <linux/interrupt.h>
#include <linux/tty.h>
#include <linux/serial.h>
#include <linux/serial_core.h>

#include <asm/time.h>
#include <asm/cpu.h>
#include <asm/bootinfo.h>
#include <asm/irq.h>
#include <asm/serial.h>

#include <asm/idt-boards/rc32434/rc32434.h>

extern int __init early_serial_setup(struct uart_port *port);

#define BASE_BAUD (1843200 / 16)

extern unsigned int idt_cpu_freq;

extern int __init setup_serial_port(void)
{
	static struct uart_port serial_req[2];
	
	memset(serial_req, 0, sizeof(serial_req));
	serial_req[0].type       = PORT_16550A;
	serial_req[0].line       = 0;
	serial_req[0].flags      = STD_COM_FLAGS;
	serial_req[0].iotype     = SERIAL_IO_MEM;
	serial_req[0].regshift   = 2;
	
	switch (mips_machtype) {
		case MACH_ARUBA_AP70:
			serial_req[0].irq        = 104;
			serial_req[0].mapbase    = KSEG1ADDR(0x18058003);
			serial_req[0].membase    = (char *) KSEG1ADDR(0x18058003);
			serial_req[0].uartclk    = idt_cpu_freq;
			break;
		case MACH_ARUBA_AP65:
		case MACH_ARUBA_AP60:
		default:
			serial_req[0].irq        = 12;
			serial_req[0].mapbase    = KSEG1ADDR(0xbc000003);
			serial_req[0].membase    = (char *) KSEG1ADDR(0xbc000003);
			serial_req[0].uartclk    = idt_cpu_freq / 2;
			break;
	}

	early_serial_setup(&serial_req[0]);
	
	return(0);
}

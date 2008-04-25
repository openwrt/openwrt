/*
 *  Copyright (C) 2004 Florian Schirmer (jolt@tuxbox.org)
 *  Copyright (C) 2005 Waldemar Brodkorb <wbx@openwrt.org>
 *  Copyright (C) 2006 Felix Fietkau <nbd@openwrt.org>
 *  Copyright (C) 2006-2008 Michael Buesch <mb@bu3sch.de>
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
 */

#include <linux/init.h>
#include <linux/types.h>
#include <linux/tty.h>
#include <linux/serial.h>
#include <linux/serial_core.h>
#include <linux/serial_reg.h>
#include <linux/serial_8250.h>
#include <asm/bootinfo.h>
#include <asm/time.h>
#include <asm/reboot.h>
#include <asm/cfe.h>
#include <linux/pm.h>
#include <linux/ssb/ssb.h>
#include <linux/ssb/ssb_embedded.h>

#include <nvram.h>

extern void bcm47xx_pci_init(void);
extern void bcm47xx_time_init(void);

struct ssb_bus ssb;


int pcibios_plat_dev_init(struct pci_dev *dev)
{
	int err;

	err = ssb_pcibios_plat_dev_init(dev);
	if (err) {
		printk(KERN_ALERT "PCI: Failed to init device %s\n",
		       pci_name(dev));
	}

	return err;
}

static void bcm47xx_machine_restart(char *command)
{
	printk(KERN_ALERT "Please stand by while rebooting the system...\n");
	local_irq_disable();
	/* CFE has a reboot callback, but that does not work.
	 * Oopses with: Reserved instruction in kernel code.
	 */

	/* Set the watchdog timer to reset immediately */
	if (ssb_watchdog_timer_set(&ssb, 1))
		printk(KERN_EMERG "SSB watchdog-triggered reboot failed!\n");
	while (1)
		cpu_relax();
}

static void bcm47xx_machine_halt(void)
{
	/* Disable interrupts and watchdog and spin forever */
	local_irq_disable();
	if (ssb_watchdog_timer_set(&ssb, 0))
		printk(KERN_EMERG "Failed to disable SSB watchdog!\n");
	while (1)
		cpu_relax();
}

static void e_aton(char *str, char *dest)
{
	int i = 0;

	if (str == NULL) {
		memset(dest, 0, 6);
		return;
	}
	
	for (;;) {
		dest[i++] = (char) simple_strtoul(str, NULL, 16);
		str += 2;
		if (!*str++ || i == 6)
			break;
	}
}

static void bcm47xx_fill_sprom(struct ssb_sprom *sprom)
{
	char *s;

	memset(sprom, 0xFF, sizeof(struct ssb_sprom));

	sprom->revision = 1;
	if ((s = nvram_get("il0macaddr")))
		e_aton(s, sprom->il0mac);
	if ((s = nvram_get("et0macaddr")))
		e_aton(s, sprom->et0mac);
	if ((s = nvram_get("et1macaddr")))
		e_aton(s, sprom->et1mac);
	if ((s = nvram_get("et0phyaddr")))
		sprom->et0phyaddr = simple_strtoul(s, NULL, 0);
	if ((s = nvram_get("et1phyaddr")))
		sprom->et1phyaddr = simple_strtoul(s, NULL, 0);
	if ((s = nvram_get("et0mdcport")))
		sprom->et0mdcport = !!simple_strtoul(s, NULL, 10);
	if ((s = nvram_get("et1mdcport")))
		sprom->et1mdcport = !!simple_strtoul(s, NULL, 10);
	if ((s = nvram_get("pa0b0")))
		sprom->pa0b0 = simple_strtoul(s, NULL, 0);
	if ((s = nvram_get("pa0b1")))
		sprom->pa0b1 = simple_strtoul(s, NULL, 0);
	if ((s = nvram_get("pa0b2")))
		sprom->pa0b2 = simple_strtoul(s, NULL, 0);
	if ((s = nvram_get("pa1b0")))
		sprom->pa1b0 = simple_strtoul(s, NULL, 0);
	if ((s = nvram_get("pa1b1")))
		sprom->pa1b1 = simple_strtoul(s, NULL, 0);
	if ((s = nvram_get("pa1b2")))
		sprom->pa1b2 = simple_strtoul(s, NULL, 0);
	if ((s = nvram_get("wl0gpio0")))
		sprom->gpio0 = simple_strtoul(s, NULL, 0);
	if ((s = nvram_get("wl0gpio1")))
		sprom->gpio1 = simple_strtoul(s, NULL, 0);
	if ((s = nvram_get("wl0gpio2")))
		sprom->gpio2 = simple_strtoul(s, NULL, 0);
	if ((s = nvram_get("wl0gpio3")))
		sprom->gpio3 = simple_strtoul(s, NULL, 0);
	if ((s = nvram_get("pa0maxpwr")))
		sprom->maxpwr_bg = simple_strtoul(s, NULL, 0);
	if ((s = nvram_get("pa1maxpwr")))
		sprom->maxpwr_a = simple_strtoul(s, NULL, 0);
	if ((s = nvram_get("pa0itssit")))
		sprom->itssi_bg = simple_strtoul(s, NULL, 0);
	if ((s = nvram_get("pa1itssit")))
		sprom->itssi_a = simple_strtoul(s, NULL, 0);
	sprom->boardflags_lo = 0;
	if ((s = nvram_get("boardflags")))
		sprom->boardflags_lo = simple_strtoul(s, NULL, 0);
	sprom->boardflags_hi = 0;
	if ((s = nvram_get("boardflags2")))
		sprom->boardflags_hi = simple_strtoul(s, NULL, 0);
}

static int bcm47xx_get_invariants(struct ssb_bus *bus, struct ssb_init_invariants *iv)
{
	char *s;

	iv->boardinfo.vendor = SSB_BOARDVENDOR_BCM;
	if ((s = nvram_get("boardtype")))
		iv->boardinfo.type = (u16)simple_strtoul(s, NULL, 0);
	if ((s = nvram_get("boardrev")))
		iv->boardinfo.rev = (u16)simple_strtoul(s, NULL, 0);

	bcm47xx_fill_sprom(&iv->sprom);

	if ((s = nvram_get("cardbus")))
		iv->has_cardbus_slot = !!simple_strtoul(s, NULL, 10);

	return 0;
}

void __init plat_mem_setup(void)
{
	int i, err;
	char *s;
	struct ssb_mipscore *mcore;

	err = ssb_bus_ssbbus_register(&ssb, SSB_ENUM_BASE, bcm47xx_get_invariants);
	if (err) {
		const char *msg = "Failed to initialize SSB bus (err %d)\n";
		cfe_printk(msg, err); /* Make sure the message gets out of the box. */
		panic(msg, err);
	}
	mcore = &ssb.mipscore;

	s = nvram_get("kernel_args");
	if (s && !strncmp(s, "console=ttyS1", 13)) {
		struct ssb_serial_port port;

		cfe_printk("Swapping serial ports!\n");
		/* swap serial ports */
		memcpy(&port, &mcore->serial_ports[0], sizeof(port));
		memcpy(&mcore->serial_ports[0], &mcore->serial_ports[1], sizeof(port));
		memcpy(&mcore->serial_ports[1], &port, sizeof(port));
	}

	for (i = 0; i < mcore->nr_serial_ports; i++) {
		struct ssb_serial_port *port = &(mcore->serial_ports[i]);
		struct uart_port s;
	
		memset(&s, 0, sizeof(s));
		s.line = i;
		s.membase = port->regs;
		s.irq = port->irq + 2;
		s.uartclk = port->baud_base;
		s.flags = UPF_BOOT_AUTOCONF | UPF_SHARE_IRQ;
		s.iotype = SERIAL_IO_MEM;
		s.regshift = port->reg_shift;

		early_serial_setup(&s);
	}
	cfe_printk("Serial init done.\n");

	_machine_restart = bcm47xx_machine_restart;
	_machine_halt = bcm47xx_machine_halt;
	pm_power_off = bcm47xx_machine_halt;
	board_time_init = bcm47xx_time_init;
}

static int __init bcm47xx_register_gpiodev(void)
{
	static struct resource res = {
		.start = 0xFFFFFFFF,
	};
	struct platform_device *pdev;

	pdev = platform_device_register_simple("GPIODEV", 0, &res, 1);
	if (!pdev) {
		printk(KERN_ERR "bcm47xx: GPIODEV init failed\n");
		return -ENODEV;
	}

	return 0;
}
device_initcall(bcm47xx_register_gpiodev);

EXPORT_SYMBOL(ssb);

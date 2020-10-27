// SPDX-License-Identifier: GPL-2.0-only
/*
 * 8250 serial console setup for the Realtek RTL838X SoC
 *
 * based on the original BSP by
 * Copyright (C) 2006-2012 Tony Wu (tonywu@realtek.com)
 *
 * Copyright (C) 2020 B. Koblitz
 *
 */

#include <linux/types.h>
#include <linux/ctype.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/version.h>
#include <linux/serial.h>
#include <linux/serial_core.h>
#include <linux/serial_8250.h>
#include <linux/serial_reg.h>
#include <linux/tty.h>
#include <linux/clk.h>

#include <asm/mach-rtl838x/mach-rtl838x.h>

extern char arcs_cmdline[];

int __init rtl838x_serial_init(void)
{
#ifdef CONFIG_SERIAL_8250
	int ret;
	struct uart_port p;
	unsigned long baud = 0;
	int err;
	char parity = '\0', bits = '\0', flow = '\0';
	char *s;
	struct device_node *dn;

	dn = of_find_compatible_node(NULL, NULL, "ns16550a");
	if (dn) {
		pr_info("Found NS16550a: %s (%s)\n", dn->name, dn->full_name);
		dn = of_find_compatible_node(dn, NULL, "ns16550a");
		if (dn && of_device_is_available(dn) && soc_info.family == RTL8380_FAMILY_ID) {
			/* Enable UART1 on RTL838x */
			pr_info("Enabling uart1\n");
			sw_w32(0x10, RTL838X_GMII_INTF_SEL);
		}
	} else {
		pr_err("No NS16550a UART found!");
		return -ENODEV;
	}

	s = strstr(arcs_cmdline, "console=ttyS0,");
	if (s) {
		s += 14;
		err = kstrtoul(s, 10, &baud);
		if (err)
			baud = 0;
		while (isdigit(*s))
			s++;
		if (*s == ',')
			s++;
		if (*s)
			parity = *s++;
		if (*s == ',')
			s++;
		if (*s)
			bits = *s++;
		if (*s == ',')
			s++;
		if (*s == 'h')
			flow = 'r';
	}

	if (baud == 0) {
		baud = 38400;
		pr_warn("Using default baud rate: %lu\n", baud);
	}
	if (parity != 'n' && parity != 'o' && parity != 'e')
		parity = 'n';
	if (bits != '7' && bits != '8')
		bits = '8';

	memset(&p, 0, sizeof(p));

	p.type = PORT_16550A;
	p.membase = (unsigned char *) RTL838X_UART0_BASE;
	p.irq = RTL838X_UART0_IRQ;
	p.uartclk = SYSTEM_FREQ - (24 * baud);
	p.flags = UPF_SKIP_TEST | UPF_LOW_LATENCY | UPF_FIXED_TYPE;
	p.iotype = UPIO_MEM;
	p.regshift = 2;
	p.fifosize = 1;

	/* Call early_serial_setup() here, to set up 8250 console driver */
	if (early_serial_setup(&p) != 0)
		ret = 1;
#endif
	return 0;
}

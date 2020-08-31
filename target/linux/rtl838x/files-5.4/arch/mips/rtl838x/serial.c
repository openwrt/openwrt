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
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/version.h>
#include <linux/serial.h>
#include <linux/serial_core.h>
#include <linux/serial_8250.h>
#include <linux/serial_reg.h>
#include <linux/tty.h>
#include <linux/clk.h>

#include "mach-rtl838x.h"

extern char arcs_cmdline[];

int __init rtl838x_serial_init(void)
{
#ifdef CONFIG_SERIAL_8250
	int ret;
	struct uart_port p;
	int baud = 0;
	char parity = '\0', bits = '\0', flow = '\0';
	char *s, *n;

	/* Enable UART1 */
	rtl838x_w32(0x10, RTL838X_GMII_INTF_SEL);

	s = strstr(arcs_cmdline, "console=ttyS0,");
	if (s) {
		s += 14;
		baud = simple_strtoul(s, &n, 10);
		s = n;
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
		pr_warn("Using default baud rate: %d\n", baud);
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

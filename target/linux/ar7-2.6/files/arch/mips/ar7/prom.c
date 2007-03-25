/*
 * $Id$
 * 
 * Copyright (C) 2006, 2007 OpenWrt.org
 * 
 * Carsten Langgaard, carstenl@mips.com
 * Copyright (C) 1999,2000 MIPS Technologies, Inc.  All rights reserved.
 *
 *  This program is free software; you can distribute it and/or modify it
 *  under the terms of the GNU General Public License (Version 2) as
 *  published by the Free Software Foundation.
 *
 *  This program is distributed in the hope it will be useful, but WITHOUT
 *  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 *  FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 *  for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  59 Temple Place - Suite 330, Boston MA 02111-1307, USA.
 *
 * Putting things on the screen/serial line using YAMONs facilities.
 */
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/serial_reg.h>
#include <linux/spinlock.h>
#include <linux/module.h>
#include <asm/io.h>
#include <asm/bootinfo.h>
#include <asm/mips-boards/prom.h>

#include <asm/ar7/ar7.h>

#define MAX_ENTRY 80

struct env_var {
	char *name;
	char *value;
};

struct psp_chip_map {
	u16 chip;
	char *names[50];
};

/* I hate this. No. *I* *HATE* *THIS* */
static __initdata struct psp_chip_map psp_chip_map[] = {
	{
		.chip = AR7_CHIP_7100,
		.names = {
			"dummy", "cpufrequency", "memsize",
			"flashsize", "modetty0", "modetty1", "prompt",
			"bootcfg", "maca", "macb", "usb_rndis_mac",
			"macap", "my_ipaddress", "server_ipaddress",
			"bline_maca", "bline_macb", "bline_rndis",
			"bline_atm", "usb_pid", "usb_vid",
			"usb_eppolli", "gateway_ipaddress",
			"subnet_mask", "usb_serial", "usb_board_mac",
			"remote_user", "remote_pass", "remote_dir",
			"sysfrequency", "link_timeout", "mac_port",
			"path", "hostname", "tftpcfg", "buildops",
			"tftp_fo_fname", "tftp_fo_ports",
			"console_state", "mipsfrequency", 
		},
	},
	{
		.chip = AR7_CHIP_7200,
		.names = {
			"dummy", "cpufrequency", "memsize",
			"flashsize", "modetty0", "modetty1", "prompt",
			"bootcfg", "maca", "macb", "usb_rndis_mac",
			"macap", "my_ipaddress", "server_ipaddress",
			"bline_maca", "bline_macb", "bline_rndis",
			"bline_atm", "usb_pid", "usb_vid",
			"usb_eppolli", "gateway_ipaddress",
			"subnet_mask", "usb_serial", "usb_board_mac",
			"remote_user", "remote_pass", "remote_dir",
			"sysfrequency", "link_timeout", "mac_port",
			"path", "hostname", "tftpcfg", "buildops",
			"tftp_fo_fname", "tftp_fo_ports",
			"console_state", "mipsfrequency", 
		},
	},
	{
		.chip = AR7_CHIP_7300,
		.names = {
			"dummy", "cpufrequency", "memsize",
			"flashsize", "modetty0", "modetty1", "prompt",
			"bootcfg", "maca", "macb", "usb_rndis_mac",
			"macap", "my_ipaddress", "server_ipaddress",
			"bline_maca", "bline_macb", "bline_rndis",
			"bline_atm", "usb_pid", "usb_vid",
			"usb_eppolli", "gateway_ipaddress",
			"subnet_mask", "usb_serial", "usb_board_mac",
			"remote_user", "remote_pass", "remote_dir",
			"sysfrequency", "link_timeout", "mac_port",
			"path", "hostname", "tftpcfg", "buildops",
			"tftp_fo_fname", "tftp_fo_ports",
			"console_state", "mipsfrequency", 
		},
	},
	{
		.chip = 0x0,
	},
};

static struct env_var adam2_env[MAX_ENTRY] = { { 0, }, };

char * __init prom_getenv(char *name)
{
	int i;
	for (i = 0; (i < MAX_ENTRY) && adam2_env[i].name; i++)
		if (!strcmp(name, adam2_env[i].name))
			return adam2_env[i].value;

	return NULL;
}

char * __init prom_getcmdline(void)
{
	return &(arcs_cmdline[0]);
}

static void  __init ar7_init_cmdline(int argc, char *argv[])
{
	char *cp;
	int actr;

	actr = 1; /* Always ignore argv[0] */

	cp = &(arcs_cmdline[0]);
	while(actr < argc) {
	        strcpy(cp, argv[actr]);
		cp += strlen(argv[actr]);
		*cp++ = ' ';
		actr++;
	}
	if (cp != &(arcs_cmdline[0])) {
		/* get rid of trailing space */
		--cp;
		*cp = '\0';
	}
}

struct psbl_rec {
	u32 psbl_size;
	u32 env_base;
	u32 env_size;
	u32 ffs_base;
	u32 ffs_size;
};

static __initdata char psp_env_version[] = "TIENV0.8";

struct psp_env_var {
	unsigned char num;
	unsigned char ctrl;
	unsigned short csum;
	unsigned char len;
	unsigned char data[11];
};

static char psp_env_data[2048] = { 0, };

static void __init add_adam2_var(char *name, char *value)
{
	int i;
	for (i = 0; i < MAX_ENTRY; i++) {
		if (!adam2_env[i].name) {
			adam2_env[i].name = name;
			adam2_env[i].value = value;
			return;
		} else if (!strcmp(adam2_env[i].name, name)) {
			adam2_env[i].value = value;
			return;
		}
	}
}

static int __init parse_psp_env(void *start)
{
	int i, j;
	u16 chip;
	struct psp_chip_map *map;
	char *src, *dest, *name, *value;
	struct psp_env_var *vars = start;

	chip = ar7_chip_id();
	for (map = psp_chip_map; map->chip; map++)
		if (map->chip == chip)
			break;

	if (!map->chip)
		return -EINVAL;

	i = 1;
	j = 0;
	dest = psp_env_data;
	while (vars[i].num < 0xff) {
		src = vars[i].data;
		if (vars[i].num) {
			strcpy(dest, map->names[vars[i].num]);
			name = dest;
		} else {
			strcpy(dest, src);
			name = dest;
			src += strlen(src) + 1;
		}			
		dest += strlen(dest) + 1;
		strcpy(dest, src);
		value = dest;
		dest += strlen(dest) + 1;
		add_adam2_var(name, value);
		i += vars[i].len;
	}
	return 0;
}

static void __init ar7_init_env(struct env_var *env)
{
	int i;
	struct psbl_rec *psbl = (struct psbl_rec *)(KSEG1ADDR(0x14000300));
	void *psp_env = (void *)KSEG1ADDR(psbl->env_base);

	if(strcmp(psp_env, psp_env_version) == 0) {
		parse_psp_env(psp_env);
	} else {
		for (i = 0; i < MAX_ENTRY; i++, env++)
			if (env->name)
				add_adam2_var(env->name, env->value);
	}
}

void __init prom_init(void)
{
	prom_printf("\nLINUX running...\n");
	ar7_init_cmdline(fw_arg0, (char **)fw_arg1);
	ar7_init_env((struct env_var *)fw_arg2);
}

#define PORT(offset) (KSEG1ADDR(AR7_REGS_UART0 + (offset * 4)))
static inline unsigned int serial_in(int offset)
{
	return readb((void *)PORT(offset));
}

static inline void serial_out(int offset, int value)
{
	writeb(value, (void *)PORT(offset));
}

char prom_getchar(void)
{
	while (!(serial_in(UART_LSR) & UART_LSR_DR));
	return serial_in(UART_RX);
}

int prom_putchar(char c)
{
	while ((serial_in(UART_LSR) & UART_LSR_TEMT) == 0);
	serial_out(UART_TX, c);
	return 1;
}

EXPORT_SYMBOL(prom_getenv);

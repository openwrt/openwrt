/*
 *  $Id$
 *
 *  ADM5120 specific setup
 *
 *  Copyright (C) ADMtek Incorporated.
 *  Copyright (C) 2005 Jeroen Vreeken (pe1rxq@amsat.org)
 *  Copyright (C) 2007 Gabor Juhos <juhosg at openwrt.org>
 *  Copyright (C) 2007 OpenWrt.org
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License
 *  as published by the Free Software Foundation; either version 2
 *  of the License, or (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the
 *  Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 *  Boston, MA  02110-1301, USA.
 *
 */

#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/io.h>

#include <asm/reboot.h>
#include <asm/time.h>

#include <adm5120_info.h>
#include <adm5120_defs.h>
#include <adm5120_switch.h>
#include <adm5120_board.h>

static char *prom_names[ADM5120_PROM_LAST+1] __initdata = {
	[ADM5120_PROM_GENERIC]		= "Generic",
	[ADM5120_PROM_CFE]		= "CFE",
	[ADM5120_PROM_UBOOT]		= "U-Boot",
	[ADM5120_PROM_MYLOADER]		= "MyLoader",
	[ADM5120_PROM_ROUTERBOOT]	= "RouterBOOT",
	[ADM5120_PROM_BOOTBASE]		= "Bootbase"
};

static void __init adm5120_report(void)
{
	printk(KERN_INFO "SoC      : ADM%04X%s revision %d, running "
		"at %ldMHz\n",
		adm5120_product_code,
		adm5120_package_bga() ? "" : "P",
		adm5120_revision, (adm5120_speed / 1000000));
	printk(KERN_INFO "Bootdev  : %s flash\n",
		adm5120_nand_boot ? "NAND":"NOR");
	printk(KERN_INFO "Prom     : %s\n", prom_names[adm5120_prom_type]);
}

void __init plat_mem_setup(void)
{
	adm5120_soc_init();
	adm5120_mem_init();
	adm5120_report();

	board_time_init = adm5120_time_init;

	_machine_restart = adm5120_restart;
	_machine_halt = adm5120_halt;
	pm_power_off = adm5120_halt;

	set_io_port_base(KSEG1);
}

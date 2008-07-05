/*
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307, USA.
 *
 *   Copyright (C) 2007 John Crispin <blogic@openwrt.org> 
 */
#ifndef _IFXMIPS_CGU_H__
#define _IFXMIPS_CGU_H__

unsigned int cgu_get_mips_clock(int cpu);
unsigned int cgu_get_io_region_clock(void);
unsigned int cgu_get_fpi_bus_clock(int fpi);
void cgu_setup_pci_clk(int internal_clock);
unsigned int ifxmips_get_ddr_hz(void);
unsigned int ifxmips_get_fpi_hz(void);
unsigned int ifxmips_get_cpu_hz(void);

#endif

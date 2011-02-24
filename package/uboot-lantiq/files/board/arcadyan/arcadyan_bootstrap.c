/*
 * (C) Copyright 2010 Industrie Dial Face S.p.A.
 * Luigi 'Comio' Mantellini, luigi.mantellini@idf-hit.com
 *
 * (C) Copyright 2007
 * Vlad Lungu vlad.lungu@windriver.com
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include <common.h>
#include <command.h>
#include <asm/mipsregs.h>
#include <asm/io.h>

phys_size_t bootstrap_initdram(int board_type)
{
	/* Sdram is setup by assembler code */
	/* If memory could be changed, we should return the true value here */
	return CONFIG_SYS_MAX_RAM;
}

int bootstrap_checkboard(void)
{
	return 0;
}

int bootstrap_misc_init_r(void)
{
	set_io_port_base(0);
	return 0;
}

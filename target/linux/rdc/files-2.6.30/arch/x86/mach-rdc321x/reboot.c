/*
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

#include <asm/reboot.h>
#include <asm/io.h>

static void rdc321x_reset(void)
{
	unsigned i;

	/* write to southbridge config register 0x41
	   enable pci reset on cpu reset, make internal port 0x92 writeable
	   and switch port 0x92 to internal */
	outl(0x80003840, 0xCF8);
	i = inl(0xCFC);
	i |= 0x1600;
	outl(i, 0xCFC);

	/* soft reset */
	outb(1, 0x92);
}

static int __init rdc_setup_reset(void)
{
	machine_ops.emergency_restart = rdc321x_reset;
	return 0;
}

arch_initcall(rdc_setup_reset);

/*
 * (C) Copyright 2003
 * Wolfgang Denk Engineering, <wd@denx.de>
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

/*
OUTPUT_FORMAT("elf32-bigmips", "elf32-bigmips", "elf32-bigmips")
*/
OUTPUT_FORMAT("elf32-tradbigmips", "elf32-tradbigmips", "elf32-tradbigmips")
OUTPUT_ARCH(mips)
ENTRY(_start)
SECTIONS
{
        . = 0x00000000;

        . = ALIGN(4);
	.text       :
	{
	  *(.text)
	}

        . = ALIGN(4);
        .rodata  : { *(.rodata) }

        . = ALIGN(4);
        .data  : { *(.data) }

	. = ALIGN(4);
	.sdata  : { *(.sdata) }

	_gp = ALIGN(16);

	__got_start = .;
	.got  : { *(.got) }
	__got_end = .;

	.sdata  : { *(.sdata) }

	. = .;
        __u_boot_cmd_start = .;
        .u_boot_cmd : { *(.u_boot_cmd) }
        __u_boot_cmd_end = .;

	uboot_end_data = .;
	num_got_entries = (__got_end - __got_start) >> 2;

        . = ALIGN(4);
	.sbss  : { *(.sbss) }
        .bss  : { *(.bss) }
	uboot_end = .;
}

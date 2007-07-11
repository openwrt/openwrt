/*
 * Support for IBM PPC 405EP-based MagicBox board
 *
 * Heavily based on bubinga.h
 *
 *
 * Author: SAW (IBM), derived from walnut.h.
 *         Maintained by MontaVista Software <source@mvista.com>
 *
 * 2003 (c) MontaVista Softare Inc.  This file is licensed under the
 * terms of the GNU General Public License version 2. This program is
 * licensed "as is" without any warranty of any kind, whether express
 * or implied.
 */

#ifdef __KERNEL__
#ifndef __MAGICBOX_H__
#define __MAGICBOX_H__

#include <platforms/4xx/ibm405ep.h>
#include <asm/ppcboot.h>

/* Memory map for the "MagicBox" 405EP evaluation board -- generic 4xx. */

/* The UART clock is based off an internal clock -
 * define BASE_BAUD based on the internal clock and divider(s).
 * Since BASE_BAUD must be a constant, we will initialize it
 * using clock/divider values which OpenBIOS initializes
 * for typical configurations at various CPU speeds.
 * The base baud is calculated as (FWDA / EXT UART DIV / 16)
 */
#define BASE_BAUD		0

/* Flash */
#define PPC40x_FPGA_BASE	0xF0300000
#define PPC40x_FPGA_REG_OFFS	1	/* offset to flash map reg */
#define PPC40x_FLASH_ONBD_N(x)	(x & 0x02)
#define PPC40x_FLASH_SRAM_SEL(x) (x & 0x01)
#define PPC40x_FLASH_LOW	0xFFF00000
#define PPC40x_FLASH_HIGH	0xFFF80000
#define PPC40x_FLASH_SIZE	0x80000

#define PPC4xx_MACHINE_NAME	"MagicBox"

#endif /* __MAGICBOX_H__ */
#endif /* __KERNEL__ */

#ifndef __CONFIG_QI_LB60_H
#define __CONFIG_QI_LB60_H

#include <configs/nanonote.h>

#define CONFIG_QI_LB60 1

#define CONFIG_BOOTARGS		"mem=32M console=ttyS0,57600n8 ubi.mtd=2 rootfstype=ubifs root=ubi0:rootfs rw rootwait"
#define CONFIG_BOOTARGSFROMSD	"mem=32M console=ttyS0,57600n8 rootfstype=ext2 root=/dev/mmcblk0p2 rw rootwait"
#define CONFIG_BOOTCOMMAND	"nand read 0x80600000 0x400000 0x200000;bootm"
#define CONFIG_BOOTCOMMANDFROMSD	"mmc init; fatload mmc 0 0x80600000 uImage; bootm"

/* SDRAM paramters */
#define SDRAM_BW16		1	/* Data bus width: 0-32bit, 1-16bit */
#define SDRAM_BANK4		1	/* Banks each chip: 0-2bank, 1-4bank */
#define SDRAM_ROW		13	/* Row address: 11 to 13 */
#define SDRAM_COL		9	/* Column address: 8 to 12 */
#define SDRAM_CASL		2	/* CAS latency: 2 or 3 */

/* SDRAM Timings, unit: ns */
#define SDRAM_TRAS		45	/* RAS# Active Time */
#define SDRAM_RCD		20	/* RAS# to CAS# Delay */
#define SDRAM_TPC		20	/* RAS# Precharge Time */
#define SDRAM_TRWL		7	/* Write Latency Time */
#define SDRAM_TREF	        15625	/* Refresh period: 8192 cycles/64ms */

#endif

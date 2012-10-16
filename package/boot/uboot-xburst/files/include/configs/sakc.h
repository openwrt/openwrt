/*
 * Authors: Xiangfu Liu <xiangfu.z@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 3 of the License, or (at your option) any later version.
 */

/*
 * This file contains the configuration parameters for SAKC.
 */
#ifndef __CONFIG_H
#define __CONFIG_H

#define DEBUG
#define CONFIG_MIPS32		1  /* MIPS32 CPU core */
#define CONFIG_JzRISC		1  /* JzRISC core */
#define CONFIG_JZSOC		1  /* Jz SoC */
#define CONFIG_JZ4725		1  /* Jz4725 SoC */
#define CONFIG_JZ4740		1  /* Jz4740 SoC */
#define CONFIG_SAKC			1  /* SAKC board */

#define MMC_BUS_WIDTH_1BIT	1  /* 1 for MMC  1Bit Bus Width */

//#define CONFIG_LCD		1  /* LCD support */
//#define LCD_BPP			LCD_COLOR32	/*5:18,24,32 bits per pixel */
//#define CONFIG_SYS_WHITE_ON_BLACK	1

#define CONFIG_SYS_CPU_SPEED	336000000	/* CPU clock: 336 MHz */
#define CONFIG_SYS_EXTAL		12000000	/* EXTAL freq: 12 MHz */
#define CONFIG_SYS_HZ		(CONFIG_SYS_EXTAL / 256) /* incrementer freq */
#define CONFIG_SYS_MIPS_TIMER_FREQ	CONFIG_SYS_CPU_SPEED

#define CONFIG_SYS_UART_BASE	UART0_BASE	/* Base of the UART channel */
#define CONFIG_BAUDRATE		57600
#define CONFIG_SYS_BAUDRATE_TABLE	{ 9600, 19200, 38400, 57600, 115200 }

#define CONFIG_MMC      	1
#define CONFIG_FAT      	1  
#define CONFIG_DOS_PARTITION	1
#define CONFIG_SKIP_LOWLEVEL_INIT	1
#define CONFIG_BOARD_EARLY_INIT_F	1
#define CONFIG_SYS_NO_FLASH	1
#define CONFIG_ENV_OVERWRITE	1

#define CONFIG_BOOTP_MASK	(CONFIG_BOOTP_DEFAUL)
#define CONFIG_BOOTDELAY	3
#define CONFIG_BOOTFILE		"uImage"	/* file to load */
#define CONFIG_BOOTARGS		"mem=32M console=ttyS0,57600n8 ubi.mtd=2 rootfstype=ubifs root=ubi0:rootfs rw rootwait"
#define CONFIG_EXTRA_ENV_SETTINGS	1
#define CONFIG_BOOTARGSFROMSD	"mem=32M console=ttyS0,57600n8 rootfstype=ext2 root=/dev/mmcblk0p1 rw rootwait"
#define CONFIG_BOOTCOMMAND	"nand read 0x80600000 0x400000 0x200000;bootm"

/*
 * Command line configuration.
 */
#define CONFIG_CMD_BDI		/* bdinfo			*/
#define CONFIG_CMD_BOOTD	/* bootd			*/
#define CONFIG_CMD_CONSOLE	/* coninfo			*/
#define CONFIG_CMD_ECHO		/* echo arguments		*/
#define CONFIG_CMD_IMI		/* iminfo			*/
#define CONFIG_CMD_ITEST	/* Integer (and string) test	*/

#define CONFIG_CMD_LOADB	/* loadb			*/
#define CONFIG_CMD_LOADS	/* loads			*/
#define CONFIG_CMD_MEMORY	/* md mm nm mw cp cmp crc base loop mtest */
#define CONFIG_CMD_MISC		/* Misc functions like sleep etc*/
#define CONFIG_CMD_RUN		/* run command in env variable	*/
#define CONFIG_CMD_SAVEENV	/* saveenv			*/
#define CONFIG_CMD_SETGETDCR	/* DCR support on 4xx		*/
#define CONFIG_CMD_SOURCE	/* "source" command support	*/
#define CONFIG_CMD_XIMG		/* Load part of Multi Image	*/

#define CONFIG_CMD_NAND
#define CONFIG_CMD_MMC
#define CONFIG_CMD_FAT

/*
 * Serial download configuration
 */
#define CONFIG_LOADS_ECHO	1	/* echo on for serial download */
#define CONFIG_LOADS_BAUD_CHANGE	1	/* allow baudrate change */

/*
 * Miscellaneous configurable options
 */
#define	CONFIG_SYS_LONGHELP			/* undef to save memory */
#define	CONFIG_SYS_PROMPT		"SAKC# "	/* Monitor Command Prompt */
#define	CONFIG_SYS_CBSIZE		256	/* Console I/O Buffer Size */
#define	CONFIG_SYS_PBSIZE		(CONFIG_SYS_CBSIZE + sizeof(CONFIG_SYS_PROMPT) + 16)
/* Print Buffer Size */
#define	CONFIG_SYS_MAXARGS		16	/* max number of command args*/

#define CONFIG_SYS_MALLOC_LEN		128 * 1024
#define CONFIG_SYS_BOOTPARAMS_LEN	128 * 1024

#define CONFIG_SYS_SDRAM_BASE		0x80000000     /* Cached addr */
#define CONFIG_SYS_INIT_SP_OFFSET	0x400000
#define CONFIG_SYS_LOAD_ADDR		0x80600000     /* default load address */
#define CONFIG_SYS_MEMTEST_START	0x80100000
#define CONFIG_SYS_MEMTEST_END		0x80800000

/*
 * Environment
 */
#define CONFIG_ENV_IS_IN_NAND	1	/* use NAND for environment vars */

/*
 * NAND FLASH configuration
 */
/* NAND Boot config code */
#define JZ4740_NANDBOOT_CFG	JZ4740_NANDBOOT_B8R3 

#define SAKC_NAND_SIZE	1 /* if board nand flash is 1GB, set to 1
				   * if board nand flash is 2GB, set to 2
				   * for change the PAGE_SIZE and BLOCK_SIZE
				   * will delete when there is no 1GB flash
				   */

#define CONFIG_SYS_NAND_PAGE_SIZE	(2048 * SAKC_NAND_SIZE)
/* nand chip block size	*/
#define CONFIG_SYS_NAND_BLOCK_SIZE	(256 * SAKC_NAND_SIZE << 10)	
/* nand bad block was marked at this page in a block, start from 0 */
#define CONFIG_SYS_NAND_BADBLOCK_PAGE	127 
/* ECC offset position in oob area, default value is 6 if it isn't defined */
#define CONFIG_SYS_NAND_ECC_POS	(6 * SAKC_NAND_SIZE)
#define CONFIG_SYS_MAX_NAND_DEVICE     1
#define NAND_MAX_CHIPS          1
#define CONFIG_SYS_NAND_BASE           0xB8000000
#define CONFIG_SYS_NAND_SELECT_DEVICE  1 /* nand driver supports mutipl.*/
#define CONFIG_SYS_ONENAND_BASE	CONFIG_SYS_NAND_BASE

/*
 * IPL (Initial Program Loader, integrated inside CPU)
 * Will load first 8k from NAND (SPL) into cache and execute it from there.
 *
 * SPL (Secondary Program Loader)
 * Will load special U-Boot version (NUB) from NAND and execute it. This SPL
 * has to fit into 8kByte. It sets up the CPU and configures the SDRAM
 * controller and the NAND controller so that the special U-Boot image can be
 * loaded from NAND to SDRAM.
 *
 * NUB (NAND U-Boot)
 * This NAND U-Boot (NUB) is a special U-Boot version which can be started
 * from RAM. Therefore it mustn't (re-)configure the SDRAM controller.
 *
 */
#define CONFIG_SYS_NAND_U_BOOT_DST		0x80100000	/* Load NUB to this addr */
#define CONFIG_SYS_NAND_U_BOOT_START	CONFIG_SYS_NAND_U_BOOT_DST 
/* Start NUB from this addr*/

/*
 * Define the partitioning of the NAND chip (only RAM U-Boot is needed here)
 */
#define CONFIG_SYS_NAND_U_BOOT_OFFS	(256 << 10)	/* Offset to RAM U-Boot image */
#define CONFIG_SYS_NAND_U_BOOT_SIZE	(512 << 10)	/* Size of RAM U-Boot image */

#define CONFIG_ENV_SIZE		CONFIG_SYS_NAND_BLOCK_SIZE
#define CONFIG_ENV_OFFSET	(CONFIG_SYS_NAND_BLOCK_SIZE + CONFIG_SYS_NAND_U_BOOT_SIZE + CONFIG_SYS_NAND_BLOCK_SIZE)
/* environment starts here  */
#define CONFIG_ENV_OFFSET_REDUND	(CONFIG_ENV_OFFSET + CONFIG_ENV_SIZE)

/* in qi_lb60.h/config.mk TEXT_BAS = 0x88000000 */
#define CONFIG_SYS_MONITOR_BASE	TEXT_BASE

/*
 * SDRAM Info.
 */
#define CONFIG_NR_DRAM_BANKS	1

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

/*
 * Cache Configuration
 */
#define CONFIG_SYS_DCACHE_SIZE	16384
#define CONFIG_SYS_ICACHE_SIZE	16384
#define CONFIG_SYS_CACHELINE_SIZE	32

/*
 * GPIO definition
 */  
#define	GPIO_SD_DETECT		(2 * 32 + 27)
#define GPIO_SD_CD_N		GPIO_SD_DETECT	/* SD Card insert detect */
#define GPIO_SD_VCC_EN_N	GPIO_SDPW_EN	/* SD Card Power Enable */

#endif	/* __CONFIG_H */

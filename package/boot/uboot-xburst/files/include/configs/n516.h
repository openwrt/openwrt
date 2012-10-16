/*
 * (C) Copyright 2006
 * Ingenic Semiconductor, <jlwei@ingenic.cn>
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
 * This file contains the configuration parameters for the pavo board.
 */

#ifndef __CONFIG_H
#define __CONFIG_H

//#define DEBUG
//#define DEBUG_SHELL

#define CONFIG_MIPS32		1  /* MIPS32 CPU core */
#define CONFIG_JzRISC		1  /* JzRISC core */
#define CONFIG_JZSOC		1  /* Jz SoC */
#define CONFIG_JZ4740		1  /* Jz4740 SoC */
#define CONFIG_PAVO		1  /* PAVO validation board */

#define CONFIG_BOARD_NAME	"n516"
#define CONFIG_BOARD_HWREV	"1.0"
#define CONFIG_FIRMWARE_EPOCH	"0"
#define CONFIG_UPDATE_TMPBUF	0x80600000
#define CONFIG_UPDATE_CHUNKSIZE	0x800000
#define CONFIG_UPDATE_FILENAME	"update.oifw"
#define CONFIG_UPDATE_FILEEXT	".oifw"
#define CONFIG_UBI_PARTITION	"UBI"

#define CONFIG_SKIP_LOWLEVEL_INIT	1
#undef  CONFIG_SKIP_RELOCATE_UBOOT

#if 0
#define CONFIG_LCD                 /* LCD support */
#define CONFIG_JZLCD_METRONOME_800x600
#define LCD_BPP			LCD_COLOR8

#define WFM_DATA_SIZE  ( 1 << 14 )
#define CONFIG_METRONOME_WF_LEN (64 * (1 << 10))
#define CONFIG_METRONOME_WF_NAND_OFFSET (0x100000)
#define BMP_LOGO_HEIGHT 0
#define CONFIG_UBI_WF_VOLUME "waveforms"
#define CONFIG_UBI_BOOTSPLASH_VOLUME "bootsplash"
#define CONFIG_METRONOME_BOOTSPLASH_LEN 480000
#endif

#if 0
#define CONFIG_JZSOC_I2C
#define CONFIG_HARD_I2C
#define CONFIG_SYS_I2C_SPEED	100000
#define CONFIG_SYS_I2C_SLAVE	0
#define CONFIG_LPC_I2C_ADDR	0x54
#endif

#define JZ4740_NORBOOT_CFG	JZ4740_NORBOOT_16BIT	/* NOR Boot config code */
#define JZ4740_NANDBOOT_CFG	JZ4740_NANDBOOT_B8R3	/* NAND Boot config code */

#define CONFIG_SYS_CPU_SPEED		336000000	/* CPU clock: 336 MHz */
#define CONFIG_SYS_EXTAL		12000000	/* EXTAL freq: 12 MHz */
#define	CONFIG_SYS_HZ			(CONFIG_SYS_EXTAL/256) /* incrementer freq */

#define CONFIG_SYS_UART_BASE  		UART0_BASE	/* Base of the UART channel */

#define CONFIG_BAUDRATE		57600
#define CONFIG_SYS_BAUDRATE_TABLE	{ 9600, 19200, 38400, 57600, 115200 }


#define CONFIG_MMC      	1
#define CONFIG_GENERIC_MMC	1
#define CONFIG_JZ_MMC		1
#define CONFIG_FAT		1

#define CONFIG_SYS_HUSH_PARSER
#define CONFIG_SYS_PROMPT_HUSH_PS2 ">"
#define CONFIG_CMDLINE_EDITING

/* allow to overwrite serial and ethaddr */
#define CONFIG_ENV_OVERWRITE

#include <config_cmd_default.h>

#undef CONFIG_CMD_BDI		/* bdinfo			*/
#undef CONFIG_CMD_FPGA
#undef CONFIG_CMD_ECHO		/* echo arguments		*/
#undef CONFIG_CMD_FLASH	/* flinfo, erase, protect	*/
#undef CONFIG_CMD_FPGA		/* FPGA configuration Support	*/
#undef CONFIG_CMD_IMI		/* iminfo			*/
#undef CONFIG_CMD_ITEST	/* Integer (and string) test	*/
#undef CONFIG_CMD_LOADB	/* loadb			*/
#undef CONFIG_CMD_LOADS	/* loads			*/
#undef CONFIG_CMD_NFS		/* NFS support			*/
#undef CONFIG_CMD_SETGETDCR	/* DCR support on 4xx		*/
#undef CONFIG_CMD_SOURCE	/* "source" command support	*/
#undef CONFIG_CMD_XIMG		/* Load part of Multi Image	*/
#undef CONFIG_CMD_NET

//#define CONFIG_CMD_ASKENV
//#define CONFIG_CMD_DHCP
//#define CONFIG_CMD_PING
#define CONFIG_CMD_NAND
#define CONFIG_CMD_MMC
#define CONFIG_CMD_FAT
/*#define CONFIG_CMD_UBI*/
/*#define CONFIG_CMD_MTDPARTS*/
//#define CONFIG_CMD_JFFS2
//#define CONFIG_JFFS2_NAND
//#define CONFIG_JFFS2_CMDLINE
#define CONFIG_CMD_UPDATE

#define CONFIG_DOS_PARTITION

/*#define CONFIG_MTD_PARTITIONS*/
#define CONFIG_RBTREE

#define CONFIG_BOOTP_MASK	( CONFIG_BOOTP_DEFAUL )

/* this must be included AFTER the definition of CONFIG_COMMANDS (if any) */
#if 0
#define CONFIG_ZERO_BOOTDELAY_CHECK
#define CONFIG_BOOTDELAY	0
#define CONFIG_BOOTFILE	        uImage	/* file to load */
#define CONFIG_BOOTARGS		"mem=64M console=ttyS0,57600n8 ip=off rootfstype=ubifs root=ubi:rootfs ubi.mtd=UBI rw panic=5 " MTDPARTS_DEFAULT
#define CONFIG_BOOTCOMMAND	"check_and_update; setenv bootargs $bootargs $batt_level_param; ubi read 0x80600000 bootsplash && show_image 0x80600000; ubi read 0x80600000 kernel; bootm 0x80600000; ubi read 0x80600000 errorsplash && show_image 0x80600000; while test 0 = 0; do check_and_update; done"
#define CONFIG_SYS_AUTOLOAD	"n"		/* No autoload */
#define CONFIG_IPADDR		192.168.111.1
#define CONFIG_SERVERIP		192.168.111.2
#define MTDIDS_DEFAULT		"nand0=jz4740-nand"
#define MTDPARTS_DEFAULT	"mtdparts=jz4740-nand:1M@0(uboot)ro,-@1M(UBI)"
#define CONFIG_EXTRA_ENV_SETTINGS "mtdids=nand0=jz4740-nand\0mtdparts=mtdparts=jz4740-nand:1M@0(uboot)ro,-@1M(UBI)\0" \
				"stdout=serial\0stderr=lcd\0"
#endif

#define CONFIG_BOOTP_MASK	(CONFIG_BOOTP_DEFAUL)
#define CONFIG_BOOTDELAY	0
#define CONFIG_BOOTFILE		"uImage"	/* file to load */
#define CONFIG_BOOTARGS		"mem=64M console=ttyS0,57600n8 ubi.mtd=2 rootfstype=ubifs root=ubi0:rootfs rw rootwait"
#define CONFIG_BOOTCOMMAND	"nand read 0x80600000 0x400000 0x200000;bootm"



#define CONFIG_SYS_CONSOLE_IS_IN_ENV

/*
 * Serial download configuration
 *
 */
#define CONFIG_LOADS_ECHO	1	/* echo on for serial download	*/
#define CONFIG_SYS_LOADS_BAUD_CHANGE	1	/* allow baudrate change	*/

/*
 * Miscellaneous configurable options
 */
#define	CONFIG_SYS_LONGHELP				/* undef to save memory      */
#define	CONFIG_SYS_PROMPT		"n516 # "	/* Monitor Command Prompt    */
#define	CONFIG_SYS_CBSIZE		256		/* Console I/O Buffer Size   */
#define	CONFIG_SYS_PBSIZE (CONFIG_SYS_CBSIZE+sizeof(CONFIG_SYS_PROMPT)+16)  /* Print Buffer Size */
#define	CONFIG_SYS_MAXARGS		16		/* max number of command args*/

#define CONFIG_SYS_MALLOC_LEN		1024*1024*2
#define CONFIG_SYS_BOOTPARAMS_LEN	128*1024

#define CONFIG_SYS_SDRAM_BASE		0x80000000     /* Cached addr */

#define CONFIG_SYS_INIT_SP_OFFSET	0x400000

#define	CONFIG_SYS_LOAD_ADDR		0x80600000     /* default load address	*/

#define CONFIG_SYS_MEMTEST_START	0x80100000
#define CONFIG_SYS_MEMTEST_END		0x80800000

/*-----------------------------------------------------------------------
 * Environment
 *----------------------------------------------------------------------*/
#if !defined(CONFIG_NAND_U_BOOT) && !defined(CONFIG_NAND_SPL)
#define CONFIG_ENV_IS_IN_FLASH     1	/* use FLASH for environment vars	*/
#else
#define CONFIG_ENV_IS_IN_NAND	1	/* use NAND for environment vars	*/
#endif

/*-----------------------------------------------------------------------
 * NAND FLASH configuration
 */
#define CONFIG_SYS_MAX_NAND_DEVICE     1
#define NAND_MAX_CHIPS          1
#define CONFIG_SYS_NAND_BASE           0xB8000000
#define CONFIG_SYS_NAND_SELECT_DEVICE  1       /* nand driver supports mutipl. chips   */

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
#define CONFIG_SYS_NAND_U_BOOT_DST	0x80100000	/* Load NUB to this addr	*/
#define CONFIG_SYS_NAND_U_BOOT_START	CONFIG_SYS_NAND_U_BOOT_DST /* Start NUB from this addr	*/

/*
 * Define the partitioning of the NAND chip (only RAM U-Boot is needed here)
 */
#define CONFIG_SYS_NAND_U_BOOT_OFFS	(256 << 10)	/* Offset to RAM U-Boot image	*/
#define CONFIG_SYS_NAND_U_BOOT_SIZE	(512 << 10)	/* Size of RAM U-Boot image	*/

#define CONFIG_SYS_NAND_PAGE_SIZE 2048
#define CONFIG_SYS_NAND_BLOCK_SIZE	(128 << 10)	/* NAND chip block size		*/
#define CONFIG_SYS_NAND_BADBLOCK_PAGE	63		/* NAND bad block was marked at this page in a block, starting from 0 */
#define CONFIG_SYS_NAND_ECC_POS		6

#ifdef CONFIG_ENV_IS_IN_NAND
//#define CONFIG_ENV_SIZE			CONFIG_SYS_NAND_BLOCK_SIZE
#define CONFIG_ENV_SIZE			(128 * 1024)
//#define CONFIG_ENV_OFFSET		(CONFIG_SYS_NAND_BLOCK_SIZE + CONFIG_SYS_NAND_U_BOOT_SIZE + CONFIG_SYS_NAND_BLOCK_SIZE)	/* environment starts here  */
#define CONFIG_ENV_OFFSET		(CONFIG_SYS_NAND_U_BOOT_SIZE + CONFIG_SYS_NAND_U_BOOT_OFFS)
//#define CONFIG_ENV_OFFSET_REDUND	(CONFIG_ENV_OFFSET + CONFIG_ENV_SIZE)
#endif


/*-----------------------------------------------------------------------
 * NOR FLASH and environment organization
 */
#define CONFIG_SYS_MAX_FLASH_BANKS	1	/* max number of memory banks */
#define CONFIG_SYS_MAX_FLASH_SECT	(128)	/* max number of sectors on one chip */

#define PHYS_FLASH_1		0xa8000000 /* Flash Bank #1 */

/* The following #defines are needed to get flash environment right */
#define	CONFIG_SYS_MONITOR_BASE	TEXT_BASE   /* in pavo/config.mk TEXT_BASE=0x88000000*/ 
#define	CONFIG_SYS_SYS_MONITOR_BASE	TEXT_BASE   /* in pavo/config.mk TEXT_BASE=0x88000000*/ 
#define	CONFIG_SYS_MONITOR_LEN		(256*1024)  /* Reserve 256 kB for Monitor */

#define CONFIG_SYS_FLASH_BASE		PHYS_FLASH_1

/* timeout values are in ticks */
#define CONFIG_SYS_FLASH_ERASE_TOUT	(2 * CONFIG_SYS_HZ) /* Timeout for Flash Erase */
#define CONFIG_SYS_FLASH_WRITE_TOUT	(2 * CONFIG_SYS_HZ) /* Timeout for Flash Write */

#ifdef CONFIG_ENV_IS_IN_FLASH
#define	CONFIG_ENV_IS_NOWHERE	1
#define CONFIG_ENV_ADDR		0xa8040000
#define CONFIG_ENV_SIZE		0x20000
#endif

/*-----------------------------------------------------------------------
 * SDRAM Info.
 */
#define CONFIG_NR_DRAM_BANKS	1

// SDRAM paramters
#define SDRAM_BW16		0	/* Data bus width: 0-32bit, 1-16bit */
#define SDRAM_BANK4		1	/* Banks each chip: 0-2bank, 1-4bank */
#define SDRAM_ROW		13	/* Row address: 11 to 13 */
#define SDRAM_COL		9	/* Column address: 8 to 12 */
#define SDRAM_CASL		2	/* CAS latency: 2 or 3 */

// SDRAM Timings, unit: ns
#define SDRAM_TRAS		45	/* RAS# Active Time */
#define SDRAM_RCD		20	/* RAS# to CAS# Delay */
#define SDRAM_TPC		20	/* RAS# Precharge Time */
#define SDRAM_TRWL		7	/* Write Latency Time */
#define SDRAM_TREF	        15625	/* Refresh period: 4096 refresh cycles/64ms */

/*-----------------------------------------------------------------------
 * Cache Configuration
 */
#define CONFIG_SYS_DCACHE_SIZE		16384
#define CONFIG_SYS_ICACHE_SIZE		16384
#define CONFIG_SYS_CACHELINE_SIZE	32

/*-----------------------------------------------------------------------
 * GPIO definition
 */
#define GPIO_SD_VCC_EN_N	113 /* GPD17 */
#define GPIO_SD_CD_N           103 /* GPD7 */
#define GPIO_SD_WP             111 /* GPD15 */
#define GPIO_USB_DETE          115 /* GPD6 */
//#define GPIO_DC_DETE_N               103 /* GPD7 */
#define GPIO_CHARG_STAT_N      112 /* GPD15 */
#define GPIO_DISP_OFF_N                97 /* GPD1 */
#define GPIO_UDC_HOTPLUG       100 /* GPD4 */
#define GPIO_LED_EN            124 /* GPD28 */

#define GPIO_RST_L          50   /* GPB18 LCD_SPL */
#define GPIO_LCDRDY         49   /* GPB17 LCD_CLS */
#define GPIO_STBY           86   /* GPC22 LCD_PS */
#define GPIO_ERR            87   /* GPC23 LCD_REV */

#endif	/* __CONFIG_H */

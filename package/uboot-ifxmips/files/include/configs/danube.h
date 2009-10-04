/*
 * (C) Copyright 2003
 * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
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
 * This file contains the configuration parameters for the danube board.
 */

#ifndef __CONFIG_H
#define __CONFIG_H

#define  USE_REFERENCE_BOARD
//#define   USE_EVALUATION_BOARD

//#define   DANUBE_BOOT_FROM_EBU
#define   DANUBE_USE_DDR_RAM

#ifdef DANUBE_USE_DDR_RAM
//#define  DANUBE_DDR_RAM_111M
#define DANUBE_DDR_RAM_166M
//#define PROMOSDDR400
//#define DDR_SAMSUNG_166M
//#define DDR_PSC_166M
//#define DANUBE_DDR_RAM_133M
#define DANUBE_DDR_RAM_SIZE	32	/* 32M DDR-DRAM for reference board */
#endif

#define CONFIG_LZMA		1	/* use LZMA for compression */

#define CLK_OUT2_25MHZ
#define CONFIG_MIPS32		1	/* MIPS 4Kc CPU core	*/
#define CONFIG_IFX_MIPS		1	/* in an Infineon chip	*/
#define CONFIG_DANUBE		1	/* on a danube Board	*/
#define RAM_SIZE                0x2000000 /*32M ram*/

#define CPU_CLOCK_RATE		235000000   /* 235 MHz clock for the MIPS core */

#define INFINEON_EBU_BOOTCFG	0x688C688C	/* CMULT = 8 for 150 MHz */

#define CONFIG_BOOTDELAY	3	/* autoboot after 3 seconds	*/

#define CONFIG_BAUDRATE		115200

#define DEBUG_PARSER		2

/* valid baudrates */
#define CFG_BAUDRATE_TABLE	{ 300, 9600, 19200, 38400, 57600, 115200 }

#ifndef CFG_BOOTSTRAP_CODE
#define	CONFIG_TIMESTAMP		/* Print image info with timestamp */
#endif

#define CONFIG_PREBOOT	"echo;"	\
	"echo Type \"run flash_nfs\" to mount root filesystem over NFS;" \
	"echo"

#undef	CONFIG_BOOTARGS
/* by MarsLin 2005/05/10, to support different hardware configuations */
//#define CONFIG_EXTRA_ENV_SETTINGS	<configs/ifx_extra_env.h>
#define CONFIG_EXTRA_ENV_SETTINGS \
	"ethaddr=11:22:33:44:55:66\0" \
	"serverip=192.168.45.100\0" \
	"ipaddr=192.168.45.108\0"  \
	"ram_addr=0x80500000\0" \
	"kernel_addr=0xb0030000\0" \
	"flashargs=setenv bootargs rootfstype=squashfs,jffs2 init=/etc/preinit\0" \
	"nfsargs=setenv bootargs root=/dev/nfs rw nfsroot=${serverip}:${rootpath} init=/etc/preinit\0" \
	"addip=setenv bootargs ${bootargs} ip=${ipaddr}:${serverip}:${gatewayip}:${netmask}:${hostname}:${netdev}:off\0" \
	"addmisc=setenv bootargs ${bootargs} console=ttyS1,115200 ethaddr=${ethaddr} ${mtdparts}\0" \
	"flash_flash=run flashargs addip addmisc;bootm ${kernel_addr}\0" \
	"flash_nfs=run nfsargs addip addmisc;bootm ${kernel_addr}\0" \
	"net_flash=run load_kernel flashargs addip addmisc;bootm ${ram_addr}\0" \
	"net_nfs=run load_kernel nfsargs addip addmisc;bootm ${ram_addr}\0" \
	"load_kernel=tftp ${ram_addr} ${tftppath}openwrt-ifxmips-uImage\0" \
	"update_uboot=tftp 0x80500000 u-boot.ifx;era 1:0-10; cp.b 0x80500000 0xb0000000 0x10000\0" \
	"update_openwrt=tftp ${ram_addr} ${tftppath}openwrt-ifxmips-squashfs.image; era ${kernel_addr} +${filesize} 0; cp.b ${ram_addr} ${kernel_addr} ${filesize}\0" 

#define CONFIG_BOOTCOMMAND	"run flash_flash"

#define CONFIG_COMMANDS_YES	(CONFIG_CMD_DFL	| \
				 CFG_CMD_ASKENV		| \
				 CFG_CMD_NET	)

#define CONFIG_COMMANDS_NO	(CFG_CMD_NFS		| \
				 CFG_CMD_FPGA		| \
				 CFG_CMD_IMLS		| \
				 CFG_CMD_ITEST		| \
				 CFG_CMD_XING		| \
				 CFG_CMD_IMI		| \
				 CFG_CMD_BMP		| \
				 CFG_CMD_BOOTD		| \
				 CFG_CMD_CONSOLE	| \
				 CFG_CMD_LOADS		| \
				 CFG_CMD_LOADB		)

#define CONFIG_COMMANDS		(CONFIG_COMMANDS_YES & ~CONFIG_COMMANDS_NO)

#if 0
				 CFG_CMD_DHCP
				 CFG_CMD_ELF
				 CFG_CMD_NAND
#endif

#include <cmd_confdefs.h>

/*
 * Miscellaneous configurable options
 */
#define	CFG_LONGHELP				/* undef to save memory      */
#define	CFG_PROMPT		"DANUBE # "	/* Monitor Command Prompt    */
#define	CFG_CBSIZE		256		/* Console I/O Buffer Size   */
#define	CFG_PBSIZE (CFG_CBSIZE+sizeof(CFG_PROMPT)+16)  /* Print Buffer Size */
#define	CFG_MAXARGS		16		/* max number of command args*/

#define CFG_MALLOC_LEN		128*1024

#define CFG_BOOTPARAMS_LEN	128*1024

#define CFG_HZ       (CPU_CLOCK_RATE / 2)

#define	CFG_LOAD_ADDR		0x80100000	/* default load address	*/

#define CFG_MEMTEST_START	0x80100000
#define CFG_MEMTEST_END		0x80400000

/*-----------------------------------------------------------------------
 * FLASH and environment organization
 */
#define CFG_MAX_FLASH_BANKS	1	/* max number of memory banks */
#define CFG_MAX_FLASH_SECT	(135)	/* max number of sectors on one chip */

#define PHYS_FLASH_1		0xB0000000 /* Flash Bank #1 */
#define PHYS_FLASH_2		0xB4000000 /* Flash Bank #2 */

#define BOOTSTRAP_TEXT_BASE 0xb0000000 	

/* The following #defines are needed to get flash environment right */
#define	CFG_MONITOR_BASE	UBOOT_RAM_TEXT_BASE 	/* board/danube/config.mk. = 0xA0800000 */
#define	BOOTSTRAP_CFG_MONITOR_BASE	BOOTSTRAP_TEXT_BASE 	/* board/danube/config.mk. = 0xA0800000 */
#define	CFG_MONITOR_LEN		(256 << 10)

#define CFG_INIT_SP_OFFSET	0x400000

#define CFG_FLASH_BASE		PHYS_FLASH_1

/* timeout values are in ticks */
#define CFG_FLASH_ERASE_TOUT	(20 * CFG_HZ) /* Timeout for Flash Erase */
#define CFG_FLASH_WRITE_TOUT	(20 * CFG_HZ) /* Timeout for Flash Write */

#define	CFG_ENV_IS_IN_FLASH	1
//#define CFG_ENV_IS_NOWHERE	1
//#define CFG_ENV_IS_IN_NVRAM	1
/* Address and size of Primary Environment Sector	*/
#define CFG_ENV_ADDR		0xB0020000
#define CFG_ENV_SIZE		0x10000

#define CONFIG_FLASH_16BIT

#define CONFIG_NR_DRAM_BANKS	1

#define CONFIG_DANUBE_SWITCH
#define CONFIG_NET_MULTI
#define CONFIG_ENV_OVERWRITE

#define EXCEPTION_BASE	0x200

/**
 *\brief definition for nand
 *
 */
#define CFG_MAX_NAND_DEVICE	1	/* Max number of NAND devices		*/
#define NAND_ChipID_UNKNOWN 	0x00
#define SECTORSIZE 512
#define NAND_MAX_FLOORS 1
#define NAND_MAX_CHIPS 1


#define ADDR_COLUMN 1
#define ADDR_PAGE 2
#define ADDR_COLUMN_PAGE 3


#define AT91_SMART_MEDIA_ALE (1 << 22)  /* our ALE is AD22 */
#define AT91_SMART_MEDIA_CLE (1 << 21)  /* our CLE is AD21 */

#define NAND_DISABLE_CE(nand) 
#define NAND_ENABLE_CE(nand)
#define NAND_WAIT_READY(nand)
#define WRITE_NAND_COMMAND(d, adr) 
#define WRITE_NAND_ADDRESS(d, adr) 
#define WRITE_NAND(d, adr) 
#define READ_NAND(adr) 
/* the following are NOP's in our implementation */
#define NAND_CTL_CLRALE(nandptr)
#define NAND_CTL_SETALE(nandptr)
#define NAND_CTL_CLRCLE(nandptr)
#define NAND_CTL_SETCLE(nandptr)



#define NAND_BASE_ADDRESS  0xB4000000

#define NAND_WRITE(addr, val)     *((u8*)(NAND_BASE_ADDRESS | (addr))) = val;while((*EBU_NAND_WAIT & 0x08) == 0);
#define NAND_READ(addr, val)      val = *((u8*)(NAND_BASE_ADDRESS | (addr)))
#define NAND_CE_SET 
#define NAND_CE_CLEAR
#define NAND_READY       ( ((*EBU_NAND_WAIT)&0x07) == 7)
#define NAND_READY_CLEAR  *EBU_NAND_WAIT = 0;
#define WRITE_CMD    0x18
#define WRITE_ADDR   0x14
#define WRITE_LADDR  0x10
#define WRITE_DATA  0x10
#define READ_DATA    0x10
#define READ_LDATA   0x00
#define ACCESS_WAIT
#define IFX_ATC_NAND 0xc176
#define IFX_BTC_NAND 0xc166
#define ST_512WB2_NAND 0x2076

#define NAND_OK              0x00000000    /* Bootstrap succesful, start address in BOOT_RVEC */
#define NAND_ERR             0x80000000
#define NAND_ACC_TIMEOUT     (NAND_ERR | 0x00000001)
#define NAND_ACC_ERR         (NAND_ERR | 0x00000002)


/*****************************************************************************
 * DANUBE
 *****************************************************************************/
/* lock cache for C program stack */
/* points to ROM */
/* stack size is 16K */
#define LOCK_DCACHE_ADDR       	0x9FC00000
#define LOCK_DCACHE_SIZE       	0x1000

/*
 * Memory layout
 */
#define CFG_SDRAM_BASE		0x80000000
#define CFG_SDRAM_BASE_UNCACHE	0xA0000000
#define CFG_CACHE_LOCK_SIZE  LOCK_DCACHE_SIZE

/*
 * Cache settings
 */
#define CFG_CACHE_SIZE   16384
#define CFG_CACHE_LINES  32
#define CFG_CACHE_WAYS   4
#define CFG_CACHE_SETS   128

#define CFG_ICACHE_SIZE   CFG_CACHE_SIZE
#define CFG_DCACHE_SIZE   CFG_CACHE_SIZE
#define CFG_CACHELINE_SIZE  CFG_CACHE_LINES

#endif	/* __CONFIG_H */

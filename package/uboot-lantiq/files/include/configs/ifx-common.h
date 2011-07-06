/*
 * (C) Copyright 2008
 * Stefan Roese, DENX Software Engineering, sr@denx.de.
 *
 * Common configuration options for all AMCC boards
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

#ifndef __IFX_COMMON_H
#define __IFX_COMMON_H

#define CONFIG_BOOTDELAY	2	/* autoboot after 5 seconds	*/

#define CONFIG_BAUDRATE		115200

/* valid baudrates */
#define CONFIG_SYS_BAUDRATE_TABLE	{ 9600, 19200, 38400, 57600, 115200 }

#define CONFIG_TIMESTAMP		/* Print image info with timestamp */

#undef CONFIG_PREBOOT

#undef	CONFIG_BOOTARGS
#define CONFIG_EXTRA_ENV_SETTINGS					\
	"ram_addr=0x80500000\0"						\
	"kernel_addr=0xb0020000\0"					\
	"mtdparts=mtdparts=ifx-nor:256k(uboot)ro,64k(uboot_env)ro,64k(kernel),-(rootfs)\0" \
	"flashargs=setenv bootargs rootfstype=squashfs,jffs2\0"		\
	"nfsargs=setenv bootargs root=/dev/nfs rw "			\
		"nfsroot=${serverip}:${rootpath} \0"			\
	"addip=setenv bootargs ${bootargs} "				\
		"ip=${ipaddr}:${serverip}:${gatewayip}:${netmask}"	\
		":${hostname}:${netdev}:off\0"				\
	"addmisc=setenv bootargs ${bootargs} init=/etc/preinit "	\
		"console=ttyS1,115200 ethaddr=${ethaddr} "		\
		"${mtdparts}\0"						\
	"flash_flash=run flashargs addip addmisc;"			\
		"bootm ${kernel_addr}\0"				\
	"flash_nfs=run nfsargs addip addmisc;bootm ${kernel_addr}\0"	\
	"net_flash=run load_kernel flashargs addip addmisc;"		\
		"bootm ${ram_addr}\0"					\
	"net_nfs=run load_kernel nfsargs addip addmisc;"		\
		"bootm ${ram_addr}\0"					\
	"load_kernel=tftp ${ram_addr} "					\
		"${tftppath}openwrt-ifxmips-uImage\0"			\
	"update_uboot=tftp 0x80500000 ${tftppath}u-boot.bin;era 0xb0000000 +${filesize};" \
		"cp.b 0x80500000 0xb0000000 ${filesize}\0" \
	"update_openwrt=tftp ${ram_addr} "				\
		"${tftppath}openwrt-ifxmips-squashfs.image;"		\
		"era ${kernel_addr} +${filesize};"			\
		"cp.b ${ram_addr} ${kernel_addr} ${filesize}\0"

#define CONFIG_BOOTCOMMAND	"run flash_flash"

/*
 * TFTP is using fragmented packets
*/
#define CONFIG_IP_DEFRAG

/*
 * BOOTP options
 */
#define CONFIG_BOOTP_BOOTFILESIZE
#define CONFIG_BOOTP_BOOTPATH
#define CONFIG_BOOTP_GATEWAY
#define CONFIG_BOOTP_HOSTNAME


/*
 * Command line configuration.
 */

#define CONFIG_CMD_FLASH        /* flinfo, erase, protect       */
#define CONFIG_CMD_MEMORY       /* md mm nm mw cp cmp crc base loop mtest */
#define CONFIG_CMD_NET          /* bootp, tftpboot, rarpboot    */
#define CONFIG_CMD_RUN          /* run command in env variable  */
#define CONFIG_CMD_SAVEENV      /* saveenv                      */
#define CONFIG_CMD_IMI
#undef CONFIG_CMD_PING
#undef CONFIG_ZLIB
#undef CONFIG_GZIP
#undef CONFIG_SYS_HUSH_PARSER

/*
 * Miscellaneous configurable options
 */

#define CONFIG_LZMA

#undef CONFIG_SYS_LONGHELP				/* undef to save memory */
#ifndef CONFIG_SYS_PROMPT
#define CONFIG_SYS_PROMPT		"=> "		/* Monitor Command Prompt */
#endif
#define CONFIG_SYS_CBSIZE		512		/* Console I/O Buffer Size */
#define CONFIG_SYS_PBSIZE (CONFIG_SYS_CBSIZE+sizeof(CONFIG_SYS_PROMPT)+16)  /* Print Buffer Size */
#define CONFIG_SYS_MAXARGS		16		/* max number of command args */

#define CONFIG_SYS_MALLOC_LEN		1024*1024
#define CONFIG_SYS_BOOTPARAMS_LEN	128*1024

#define CONFIG_SYS_MIPS_TIMER_FREQ	(CPU_CLOCK_RATE/2)
#define CONFIG_SYS_HZ			1000

#define CONFIG_SYS_SDRAM_BASE		0x80000000
#define CONFIG_SYS_LOAD_ADDR		0x80100000	/* default load address	*/
#define CONFIG_SYS_MEMTEST_START	0x80100000
#define CONFIG_SYS_MEMTEST_END		0x80800000

#define CONFIG_CMDLINE_EDITING		/* add command line history	*/
#undef CONFIG_AUTO_COMPLETE		/* add autocompletion support	*/

#define CONFIG_ZERO_BOOTDELAY_CHECK	/* check for keypress on bootdelay==0 */
#define CONFIG_VERSION_VARIABLE		/* include version env variable */
#define CONFIG_SYS_CONSOLE_INFO_QUIET	/* don't print console @ startup*/

#ifdef CONFIG_SYS_HUSH_PARSER
#define CONFIG_SYS_PROMPT_HUSH_PS2	"> "
#endif

#define CONFIG_LOADS_ECHO		/* echo on for serial download	*/
#define CONFIG_SYS_LOADS_BAUD_CHANGE	/* allow baudrate change	*/

/*-----------------------------------------------------------------------
 * FLASH and environment organization
 */
 
#define CONFIG_SYS_MAX_FLASH_BANKS_DETECT	1

#define CONFIG_SYS_MAX_FLASH_BANKS	1	/* max number of memory banks */
#define CONFIG_SYS_MAX_FLASH_SECT	(140)	/* max number of sectors on one chip */

#define PHYS_FLASH_1			0xB0000000 /* Flash Bank #1 */
#define PHYS_FLASH_2			0xB0800000 /* Flash Bank #2 */

/* The following #defines are needed to get flash environment right */
#define CONFIG_SYS_MONITOR_BASE		TEXT_BASE
#define CONFIG_SYS_MONITOR_LEN		(192 << 10)

#define CONFIG_SYS_INIT_SP_OFFSET	0x400000

#define CONFIG_SYS_FLASH_BASE		PHYS_FLASH_1

#define CONFIG_ENV_OVERWRITE		1
#define CONFIG_ENV_IS_IN_FLASH		1

/* Address and size of Primary Environment Sector	*/
#define CONFIG_ENV_ADDR			0xB0010000
#define CONFIG_ENV_SIZE			0x10000

#ifdef CONFIG_FLASH_CFI_DRIVER
#define CONFIG_SYS_FLASH_CFI
#define CONFIG_SYS_FLASH_SWAP_ADDR
#define CONFIG_FLASH_SHOW_PROGRESS	45

#define CONFIG_SYS_FLASH_CFI_WIDTH	FLASH_CFI_16BIT

#define FLASH_FIXUP_ADDR_8(addr)	((void*)((ulong)(addr)^2))
#define FLASH_FIXUP_ADDR_16(addr)	((void*)((ulong)(addr)^2))

#endif

#define CONFIG_NR_DRAM_BANKS		1

#ifdef CONFIG_SYS_EBU_BOOT
#ifndef INFINEON_EBU_BOOTCFG
#error Please define INFINEON_EBU_BOOTCFG
#endif
#endif

#ifdef CONFIG_BOOTSTRAP
#define CONFIG_BOOTSTRAP_BASE			CONFIG_BOOTSTRAP_TEXT_BASE
#define CONFIG_BOOTSTRAP_BAUDRATE		CONFIG_BAUDRATE
#define CONFIG_SKIP_LOWLEVEL_INIT
#define CONFIG_BOOTSTRAP_LZMA
//#define CONFIG_BOOTSTRAP_SERIAL
#endif

#endif /* __IFX_COMMON_H */

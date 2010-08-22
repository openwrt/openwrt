/*
 * (C) Copyright 2009
 * Marvell Semiconductor <www.marvell.com>
 * Written-by: Prafulla Wadaskar <prafulla@marvell.com>
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301 USA
 */

#ifndef _CONFIG_SHEEVAPLUG_H
#define _CONFIG_SHEEVAPLUG_H

/*
 * Version number information
 */
#define CONFIG_IDENT_STRING	"\nMarvell-Sheevaplug"

/*
 * High Level Configuration Options (easy to change)
 */
#define CONFIG_MARVELL		1
#define CONFIG_ARM926EJS	1	/* Basic Architecture */
#define CONFIG_FEROCEON_88FR131	1	/* CPU Core subversion */
#define CONFIG_KIRKWOOD		1	/* SOC Family Name */
#define CONFIG_KW88F6281	1	/* SOC Name */
#define CONFIG_MACH_SHEEVAPLUG	/* Machine type */

#define CONFIG_MD5	/* get_random_hex on krikwood needs MD5 support */
#define CONFIG_SKIP_LOWLEVEL_INIT	/* disable board lowlevel_init */
#define CONFIG_KIRKWOOD_EGIGA_INIT	/* Enable GbePort0/1 for kernel */
#define CONFIG_KIRKWOOD_RGMII_PAD_1V8	/* Set RGMII Pad voltage to 1.8V */

/*
 * CLKs configurations
 */
#define CONFIG_SYS_HZ		1000

/*
 * NS16550 Configuration
 */
#define CONFIG_SYS_NS16550
#define CONFIG_SYS_NS16550_SERIAL
#define CONFIG_SYS_NS16550_REG_SIZE	(-4)
#define CONFIG_SYS_NS16550_CLK		CONFIG_SYS_TCLK
#define CONFIG_SYS_NS16550_COM1		KW_UART0_BASE

/*
 * Serial Port configuration
 * The following definitions let you select what serial you want to use
 * for your console driver.
 */

#define CONFIG_CONS_INDEX	1	/*Console on UART0 */
#define CONFIG_BAUDRATE			115200
#define CONFIG_SYS_BAUDRATE_TABLE	{ 9600, 19200, 38400, 57600, \
					  115200,230400, 460800, 921600 }
/* auto boot */
#define CONFIG_BOOTDELAY	3	/* default enable autoboot */

/*
 * For booting Linux, the board info and command line data
 * have to be in the first 8 MB of memory, since this is
 * the maximum mapped by the Linux kernel during initialization.
 */
#define CONFIG_CMDLINE_TAG	1	/* enable passing of ATAGs  */
#define CONFIG_INITRD_TAG	1	/* enable INITRD tag */
#define CONFIG_SETUP_MEMORY_TAGS 1	/* enable memory tag */

#define	CONFIG_SYS_PROMPT	"Marvell>> "	/* Command Prompt */
#define	CONFIG_SYS_CBSIZE	1024	/* Console I/O Buff Size */
#define	CONFIG_SYS_PBSIZE	(CONFIG_SYS_CBSIZE \
		+sizeof(CONFIG_SYS_PROMPT) + 16)	/* Print Buff */
/*
 * Commands configuration
 */
#define CONFIG_SYS_NO_FLASH		/* Declare no flash (NOR/SPI) */
#include <config_cmd_default.h>
#define CONFIG_CMD_AUTOSCRIPT
#define CONFIG_CMD_DHCP
#define CONFIG_CMD_ENV
#define CONFIG_CMD_FAT
#define CONFIG_CMD_NAND
#define CONFIG_CMD_PING
#define CONFIG_CMD_USB

/*
 * NAND configuration
 */
#ifdef CONFIG_CMD_NAND
#define CONFIG_NAND_KIRKWOOD
#define CONFIG_SYS_MAX_NAND_DEVICE	1
#define NAND_MAX_CHIPS			1
#define CONFIG_SYS_NAND_BASE		0xD8000000	/* KW_DEFADR_NANDF */
#define NAND_ALLOW_ERASE_ALL		1
#endif

/*
 *  Environment variables configurations
 */
#ifdef CONFIG_CMD_NAND
#define CONFIG_ENV_IS_IN_NAND		1
#define CONFIG_ENV_SECT_SIZE		0x20000	/* 128K */
#else
#define CONFIG_ENV_IS_NOWHERE		1	/* if env in SDRAM */
#endif
/*
 * max 4k env size is enough, but in case of nand
 * it has to be rounded to sector size
 */
#define CONFIG_ENV_SIZE			0x20000	/* 128k */
#define CONFIG_ENV_ADDR			0x140000
#define CONFIG_ENV_OFFSET		0x140000	/* env starts here */

/*
 * Default environment variables
 */
#define CONFIG_BOOTCOMMAND		"${x_bootcmd_kernel}; "	\
	"setenv bootargs ${x_bootargs} ${x_bootargs_root}; "	\
	"${x_bootcmd_usb}; bootm 0x6400000;"

#define CONFIG_MTDPARTS		"orion_nand:1M(u-boot),"	\
	"1M@1M(second_stage_u-boot)," \
	"3M@2M(kernel),64M@5M(rootfs),187M@69M(data) rw\0"

#define CONFIG_EXTRA_ENV_SETTINGS	"x_bootargs=console"	\
	"=ttyS0,115200 mtdparts="CONFIG_MTDPARTS	\
	"x_bootcmd_kernel=nand read 0x6400000 0x200000 0x300000\0" \
	"x_bootcmd_usb=usb start\0" \
	"x_bootargs_root=root=/dev/mtdblock3 rw rootfstype=jffs2\0"

/*
 * Size of malloc() pool
 */
#define CONFIG_SYS_MALLOC_LEN	(1024 * 128) /* 128kB for malloc() */
/* size in bytes reserved for initial data */
#define CONFIG_SYS_GBL_DATA_SIZE	128

/*
 * Other required minimal configurations
 */
#define CONFIG_CONSOLE_INFO_QUIET	/* some code reduction */
#define CONFIG_ARCH_CPU_INIT	/* call arch_cpu_init() */
#define CONFIG_ARCH_MISC_INIT	/* call arch_misc_init() */
#define CONFIG_DISPLAY_CPUINFO	/* Display cpu info */
#define CONFIG_NR_DRAM_BANKS	4
#define CONFIG_STACKSIZE	0x00100000	/* regular stack- 1M */
#define CONFIG_SYS_LOAD_ADDR	0x00800000	/* default load adr- 8M */
#define CONFIG_SYS_MEMTEST_START 0x00400000	/* 4M */
#define CONFIG_SYS_MEMTEST_END	0x007fffff	/*(_8M -1) */
#define CONFIG_SYS_RESET_ADDRESS 0xffff0000	/* Rst Vector Adr */
#define CONFIG_SYS_MAXARGS	16	/* max number of command args */

/*
 * Ethernet Driver configuration
 */
#ifdef CONFIG_CMD_NET
#define CONFIG_NETCONSOLE	/* include NetConsole support   */
#define CONFIG_NET_MULTI	/* specify more that one ports available */
#define	CONFIG_MII		/* expose smi ove miiphy interface */
#define CONFIG_KIRKWOOD_EGIGA	/* Enable kirkwood Gbe Controller Driver */
#define CONFIG_SYS_FAULT_ECHO_LINK_DOWN	/* detect link using phy */
#define CONFIG_KIRKWOOD_EGIGA_PORTS	{1,0}	/* enable port 0 only */
#define CONFIG_PHY_BASE_ADR	0
#define CONFIG_ENV_OVERWRITE	/* ethaddr can be reprogrammed */
#define CONFIG_RESET_PHY_R	/* use reset_phy() to init mv8831116 PHY */
#endif /* CONFIG_CMD_NET */

/*
 * USB/EHCI
 */
#ifdef CONFIG_CMD_USB
#define CONFIG_USB_EHCI			/* Enable EHCI USB support */
#define CONFIG_USB_EHCI_KIRKWOOD	/* on Kirkwood platform	*/
#define CONFIG_EHCI_IS_TDI
#define CONFIG_USB_STORAGE
#define CONFIG_DOS_PARTITION
#define CONFIG_ISO_PARTITION
#define CONFIG_SUPPORT_VFAT
#endif /* CONFIG_CMD_USB */

#define CONFIG_SERVERIP	169.254.254.254
#define CONFIG_IPADDR	169.254.254.243
#endif /* _CONFIG_SHEEVAPLUG_H */

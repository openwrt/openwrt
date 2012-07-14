/*
 * (C) Copyright 2009-2012
 * Wojciech Dubowik <wojciech.dubowik@neratec.com>
 * Luka Perkov <uboot@lukaperkov.net>
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
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _CONFIG_ICONNECT_H
#define _CONFIG_ICONNECT_H

/*
 * Version number information
 */
#define CONFIG_IDENT_STRING	" Iomega iConnect Wireless"

/*
 * High level configuration options
 */
#define CONFIG_FEROCEON_88FR131		/* CPU Core subversion */
#define CONFIG_KIRKWOOD			/* SOC Family Name */
#define CONFIG_KW88F6281		/* SOC Name */
#define CONFIG_SKIP_LOWLEVEL_INIT	/* disable board lowlevel_init */
#define CONFIG_KIRKWOOD_EGIGA_INIT	/* Enable GbePort0/1 for kernel */

/*
 * Machine type
 */
#define CONFIG_MACH_TYPE	MACH_TYPE_ICONNECT


/*
 * Commands configuration
 */
#define CONFIG_SYS_NO_FLASH		/* declare no flash (NOR/SPI) */
#define CONFIG_SYS_MVFS
#include <config_cmd_default.h>
#define CONFIG_CMD_ENV
#define CONFIG_CMD_MII
#define CONFIG_CMD_NAND
#define CONFIG_CMD_PING
#define CONFIG_CMD_USB

/*
 * mv-common.h should be defined after CMD configs since it used them
 * to enable certain macros
 */
#include "mv-common.h"

#undef CONFIG_SYS_PROMPT
#define CONFIG_SYS_PROMPT	"iconnect => "

/*
 * Environment variables configuration
 */
#ifdef CONFIG_CMD_NAND
#define CONFIG_ENV_IS_IN_NAND
#define CONFIG_ENV_SECT_SIZE	0x20000
#else
#define CONFIG_ENV_IS_NOWHERE
#endif
#define CONFIG_ENV_SIZE			0x20000
#define CONFIG_ENV_OFFSET		0xc0000

/*
 * Default environment variables
 */
#define CONFIG_BOOTCOMMAND	"${x_bootcmd_kernel}; "		\
	"setenv bootargs ${x_bootargs} ${x_bootargs_root}; "	\
	"${x_bootcmd_usb}; bootm 0x6400000;"

#define CONFIG_MTDPARTS		"orion_nand:1M(u-boot),"	\
	"3M@1M(kernel),32M@4M(rootfs),475M@36M(data)\0"

#define CONFIG_EXTRA_ENV_SETTINGS	"x_bootargs=console"	\
	"=ttyS0,115200 mtdparts="CONFIG_MTDPARTS		\
	"x_bootcmd_kernel=nand read 0x6400000 0x100000 0x300000\0" \
	"x_bootcmd_usb=usb start\0" \
	"x_bootargs_root=root=/dev/mtdblock2 rw rootfstype=jffs2\0"

/*
 * Ethernet driver configuration
 */
#ifdef CONFIG_CMD_NET
#define CONFIG_MVGBE_PORTS	{1, 0}	/* enable port 0 only */
#define CONFIG_PHY_BASE_ADR	11
#endif /* CONFIG_CMD_NET */

/*
 * SATA driver configuration
 */
#ifdef CONFIG_CMD_IDE
#define CONFIG_SYS_ATA_IDE0_OFFSET	MV_SATA_PORT0_OFFSET
#define CONFIG_SYS_ATA_IDE1_OFFSET	MV_SATA_PORT1_OFFSET
#endif /* CONFIG_CMD_IDE */

/*
 * File system
 */
#define CONFIG_CMD_EXT2
#define CONFIG_CMD_FAT
#define CONFIG_CMD_JFFS2
#define CONFIG_CMD_UBI
#define CONFIG_CMD_UBIFS
#define CONFIG_RBTREE
#define CONFIG_MTD_DEVICE
#define CONFIG_MTD_PARTITIONS
#define CONFIG_CMD_MTDPARTS

#endif /* _CONFIG_ICONNECT_H */

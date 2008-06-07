/* ============================================================================
 * Copyright (C) 2003[- 2004] ? Infineon Technologies AG.
 *
 * All rights reserved.
 * ============================================================================
 *
 * ============================================================================
 *
 * This document contains proprietary information belonging to Infineon 
 * Technologies AG. Passing on and copying of this document, and communication
 * of its contents is not permitted without prior written authorisation.
 * 
 * ============================================================================
 *
 * File Name: ifx_cfg.h
 * Author : Mars Lin (mars.lin@infineon.com)
 * Date: 
 *
 * ===========================================================================
 *
 * Project:
 * Block:
 *
 * ===========================================================================
 * Contents:  This file contains the data structures and definitions used 
 * 	      by the core iptables and the sip alg modules. 
 * ===========================================================================
 * References:
 */

/*
 * This file contains the configuration parameters for the IFX board.
 */
#ifndef _DANUBE_CFG_H_
#define _DANUBE_CFG_H_

/*-----------------------------------------------------------------------
 * U-Boot/Kernel configurations
 */
#define IFX_CFG_UBOOT_DEFAULT_CFG_IPADDR		"172.20.80.100"
#define IFX_CFG_UBOOT_DEFAULT_CFG_SERVERIP		"172.20.80.2"
#define IFX_CFG_UBOOT_DEFAULT_CFG_ETHADDR		"00:E0:92:00:01:40"
#define IFX_CFG_UBOOT_DEFAULT_CFG_NETDEV		"eth1"
#define IFX_CFG_UBOOT_DEFAULT_CFG_BAUDRATE		"115200"
#define IFX_CFG_UBOOT_LOAD_ADDRESS                      "0x80800000"

/* End of U-Boot/Kernel configurations
 *-----------------------------------------------------------------------
 */

/*-----------------------------------------------------------------------
 * Board specific configurations
 */
#ifdef IFX_CONFIG_MEMORY_SIZE
	#define IFX_CFG_MEM_SIZE	31
#else
	#error "ERROR!! Define memory size first!"
#endif

//2MB flash partition
#if (IFX_CONFIG_FLASH_SIZE == 2)
#define IFX_CFG_FLASH_PARTITIONS_INFO                                   \
        "part0_begin=0xB0000000\0"                                      \
        "part1_begin=0xB0010000\0"                                      \
        "part2_begin=0xB0050000\0"                                      \
        "total_part=3\0"

#define IFX_CFG_FLASH_DATA_BLOCKS_INFO					\
	"data_block0=" IFX_CFG_FLASH_UBOOT_IMAGE_BLOCK_NAME "\0"	\
	"data_block1=" IFX_CFG_FLASH_FIRMWARE_IMAGE_BLOCK_NAME "\0"	\
	"data_block2=" IFX_CFG_FLASH_ROOTFS_IMAGE_BLOCK_NAME "\0"	\
	"data_block3=" IFX_CFG_FLASH_KERNEL_IMAGE_BLOCK_NAME "\0"	\
	"data_block4=" IFX_CFG_FLASH_SYSTEM_CFG_BLOCK_NAME "\0"		\
	"data_block5=" IFX_CFG_FLASH_UBOOT_CFG_BLOCK_NAME "\0"		\
	"data_block6=" IFX_CFG_FLASH_FIRMWARE_DIAG_BLOCK_NAME "\0"	\
	"data_block7=" IFX_CFG_FLASH_CALIBRATION_CFG_BLOCK_NAME "\0"	\
	"total_db=8\0"

	#define IFX_CFG_FLASH_UBOOT_IMAGE_BLOCK_NAME		"uboot"
	#define IFX_CFG_FLASH_UBOOT_IMAGE_START_ADDR		0xB0000000
	#define IFX_CFG_FLASH_UBOOT_IMAGE_SIZE			0
	#define IFX_CFG_FLASH_UBOOT_IMAGE_MTDBLOCK_NAME		"/dev/mtdblock0"

	#define IFX_CFG_FLASH_FIRMWARE_IMAGE_BLOCK_NAME		"firmware"
	#define IFX_CFG_FLASH_FIRMWARE_IMAGE_START_ADDR         0xB0010000
	#define IFX_CFG_FLASH_FIRMWARE_IMAGE_SIZE		0
	#define IFX_CFG_FLASH_FIRMWARE_IMAGE_MTDBLOCK_NAME	"/dev/mtdblock1"

	#define IFX_CFG_FLASH_ROOTFS_IMAGE_BLOCK_NAME		"rootfs"
	#define IFX_CFG_FLASH_ROOTFS_IMAGE_START_ADDR           0xB0050000
	#define IFX_CFG_FLASH_ROOTFS_IMAGE_SIZE			0
	#define IFX_CFG_FLASH_ROOTFS_IMAGE_MTDBLOCK_NAME	"/dev/mtdblock2"

	#define IFX_CFG_FLASH_KERNEL_IMAGE_BLOCK_NAME		"kernel"
	#define IFX_CFG_FLASH_KERNEL_IMAGE_START_ADDR		0xB01FCFFF
	#define IFX_CFG_FLASH_KERNEL_IMAGE_SIZE			0

	#define IFX_CFG_FLASH_SYSTEM_CFG_BLOCK_NAME		"sysconfig"
	#define IFX_CFG_FLASH_SYSTEM_CFG_START_ADDR		0xB01FD000
	#define IFX_CFG_FLASH_SYSTEM_CFG_SIZE			0
	#define IFX_CFG_FLASH_SYSTEM_CFG_END_ADDR		0xB01FEFFF

	#define IFX_CFG_FLASH_UBOOT_CFG_BLOCK_NAME		"ubootconfig"
	#define IFX_CFG_FLASH_UBOOT_CFG_START_ADDR		0xB01FF000
	#define IFX_CFG_FLASH_UBOOT_CFG_SIZE                    0x0C00
	#define IFX_CFG_FLASH_UBOOT_CFG_END_ADDR		0xB01FFBFF

	#define IFX_CFG_FLASH_FIRMWARE_DIAG_BLOCK_NAME		"fwdiag"
	#define IFX_CFG_FLASH_FIRMWARE_DIAG_START_ADDR		0xB31FFC00
	#define IFX_CFG_FLASH_FIRMWARE_DIAG_SIZE		0x0200
	#define IFX_CFG_FLASH_FIRMWARE_DIAG_END_ADDR		0xB01FFDFF

	#define IFX_CFG_FLASH_CALIBRATION_CFG_BLOCK_NAME	"calibration"
	#define IFX_CFG_FLASH_CALIBRATION_CFG_START_ADDR	0xB01FFE00
	#define IFX_CFG_FLASH_CALIBRATION_CFG_SIZE		0x0200
	#define IFX_CFG_FLASH_CALIBRATION_CFG_END_ADDR		0xB01FFFFF

	#define IFX_CFG_FLASH_END_ADDR				0xB01FFFFF

//4MB flash partition
#elif (IFX_CONFIG_FLASH_SIZE == 4)
#define IFX_CFG_FLASH_PARTITIONS_INFO                                   \
        "part0_begin=0xB0000000\0"                                      \
        "part1_begin=0xB0020000\0"                                      \
        "part2_begin=0xB0060000\0"                                      \
        "total_part=3\0"

#define IFX_CFG_FLASH_DATA_BLOCKS_INFO                                  \
        "data_block0=" IFX_CFG_FLASH_UBOOT_IMAGE_BLOCK_NAME "\0"        \
        "data_block1=" IFX_CFG_FLASH_FIRMWARE_IMAGE_BLOCK_NAME "\0"     \
        "data_block2=" IFX_CFG_FLASH_ROOTFS_IMAGE_BLOCK_NAME "\0"       \
        "data_block3=" IFX_CFG_FLASH_KERNEL_IMAGE_BLOCK_NAME "\0"       \
        "data_block4=" IFX_CFG_FLASH_SYSTEM_CFG_BLOCK_NAME "\0"         \
	"data_block5=" IFX_CFG_FLASH_UBOOT_CFG_BLOCK_NAME "\0"          \
        "data_block6=" IFX_CFG_FLASH_VOIP_CFG_BLOCK_NAME "\0"           \
        "data_block7=" IFX_CFG_FLASH_FIRMWARE_DIAG_BLOCK_NAME "\0"	\
	"data_block8=" IFX_CFG_FLASH_CALIBRATION_CFG_BLOCK_NAME "\0"	\
	"total_db=9\0"

	#define IFX_CFG_FLASH_UBOOT_IMAGE_BLOCK_NAME		"uboot"
	#define IFX_CFG_FLASH_UBOOT_IMAGE_START_ADDR		0xB0000000
	#define IFX_CFG_FLASH_UBOOT_IMAGE_SIZE			0
	#define IFX_CFG_FLASH_UBOOT_IMAGE_MTDBLOCK_NAME		"/dev/mtdblock0"

	#define IFX_CFG_FLASH_FIRMWARE_IMAGE_BLOCK_NAME		"firmware"
	#define IFX_CFG_FLASH_FIRMWARE_IMAGE_START_ADDR         0xB0020000
	#define IFX_CFG_FLASH_FIRMWARE_IMAGE_SIZE		0
	#define IFX_CFG_FLASH_FIRMWARE_IMAGE_MTDBLOCK_NAME	"/dev/mtdblock1"

	#define IFX_CFG_FLASH_ROOTFS_IMAGE_BLOCK_NAME		"rootfs"
	#define IFX_CFG_FLASH_ROOTFS_IMAGE_START_ADDR           0xB0060000
	#define IFX_CFG_FLASH_ROOTFS_IMAGE_SIZE			0
	#define IFX_CFG_FLASH_ROOTFS_IMAGE_MTDBLOCK_NAME	"/dev/mtdblock2"

	#define IFX_CFG_FLASH_KERNEL_IMAGE_BLOCK_NAME		"kernel"
	#define IFX_CFG_FLASH_KERNEL_IMAGE_START_ADDR		0xB03F4FFF
	#define IFX_CFG_FLASH_KERNEL_IMAGE_SIZE			0

	#define IFX_CFG_FLASH_SYSTEM_CFG_BLOCK_NAME		"sysconfig"
	#define IFX_CFG_FLASH_SYSTEM_CFG_START_ADDR		0xB03F5000
	#define IFX_CFG_FLASH_SYSTEM_CFG_SIZE			0x2000
	#define IFX_CFG_FLASH_SYSTEM_CFG_END_ADDR		0xB03F6FFF

	#define IFX_CFG_FLASH_UBOOT_CFG_BLOCK_NAME		"ubootconfig"
	#define IFX_CFG_FLASH_UBOOT_CFG_START_ADDR		0xB03F7000
	#define IFX_CFG_FLASH_UBOOT_CFG_SIZE			0x0C00
	#define IFX_CFG_FLASH_UBOOT_CFG_END_ADDR		0xB03F7BFF

	#define IFX_CFG_FLASH_VOIP_CFG_BLOCK_NAME	        "voip"
	#define IFX_CFG_FLASH_VOIP_CFG_START_ADDR	        0xB03F7C00
	#define IFX_CFG_FLASH_VOIP_CFG_SIZE		        0x8000
	#define IFX_CFG_FLASH_VOIP_CFG_END_ADDR		        0xB03FFBFF

	#define IFX_CFG_FLASH_FIRMWARE_DIAG_BLOCK_NAME		"fwdiag"
	#define IFX_CFG_FLASH_FIRMWARE_DIAG_START_ADDR		0xB03FFC00
	#define IFX_CFG_FLASH_FIRMWARE_DIAG_SIZE		0x0200
	#define IFX_CFG_FLASH_FIRMWARE_DIAG_END_ADDR		0xB03FFDFF

	#define IFX_CFG_FLASH_CALIBRATION_CFG_BLOCK_NAME	"calibration"
	#define IFX_CFG_FLASH_CALIBRATION_CFG_START_ADDR	0xB03FFE00
	#define IFX_CFG_FLASH_CALIBRATION_CFG_SIZE		0x0200
	#define IFX_CFG_FLASH_CALIBRATION_CFG_END_ADDR		0xB03FFFFF

	#define IFX_CFG_FLASH_END_ADDR				0xB03FFFFF
//8MB flash definition
#elif (IFX_CONFIG_FLASH_SIZE == 8)
#define IFX_CFG_FLASH_PARTITIONS_INFO                                   \
        "part0_begin=0xB0000000\0"                                      \
        "part1_begin=0xB0080000\0"                                      \
        "part2_begin=0xB0280000\0"                                      \
        "part3_begin=0xB0790000\0"                                      \
        "part4_begin=0xB07A0000\0"                                      \
        "part5_begin=0xB07E0000\0"                                      \
        "total_part=6\0"

#define IFX_CFG_FLASH_DATA_BLOCKS_INFO                                  \
        "data_block0=" IFX_CFG_FLASH_UBOOT_IMAGE_BLOCK_NAME "\0"        \
        "data_block1=" IFX_CFG_FLASH_KERNEL_IMAGE_BLOCK_NAME "\0"       \
        "data_block2=" IFX_CFG_FLASH_ROOTFS_IMAGE_BLOCK_NAME "\0"       \
        "data_block3=" IFX_CFG_FLASH_SYSTEM_CFG_BLOCK_NAME "\0"         \
        "data_block4=" IFX_CFG_FLASH_FIRMWARE_IMAGE_BLOCK_NAME "\0"     \
	"data_block5=" IFX_CFG_FLASH_UBOOT_CFG_BLOCK_NAME "\0"		\
        "total_db=6\0"

        #define IFX_CFG_FLASH_UBOOT_IMAGE_BLOCK_NAME            "uboot"
        #define IFX_CFG_FLASH_UBOOT_IMAGE_START_ADDR            0xB0000000
        #define IFX_CFG_FLASH_UBOOT_IMAGE_END_ADDR              0xB007FFFF
        #define IFX_CFG_FLASH_UBOOT_IMAGE_SIZE                  0x00080000
        #define IFX_CFG_FLASH_UBOOT_IMAGE_MTDBLOCK_NAME         "/dev/mtdblock0"

        #define IFX_CFG_FLASH_KERNEL_IMAGE_BLOCK_NAME           "kernel"
        #define IFX_CFG_FLASH_KERNEL_IMAGE_START_ADDR           0xB0080000
        #define IFX_CFG_FLASH_KERNEL_IMAGE_SIZE                 0x200000
        #define IFX_CFG_FLASH_KERNEL_IMAGE_END_ADDR             0xB017FFFF
        #define IFX_CFG_FLASH_KERNEL_IMAGE_MTDBLOCK_NAME        "/dev/mtdblock1"

        #define IFX_CFG_FLASH_ROOTFS_IMAGE_BLOCK_NAME           "rootfs"
        #define IFX_CFG_FLASH_ROOTFS_IMAGE_START_ADDR           0xB0280000
        #define IFX_CFG_FLASH_ROOTFS_IMAGE_SIZE                 0x00510000
        #define IFX_CFG_FLASH_ROOTFS_IMAGE_END_ADDR             0xB078FFFF
        #define IFX_CFG_FLASH_ROOTFS_IMAGE_MTDBLOCK_NAME        "/dev/mtdblock2"

	#define IFX_CFG_FLASH_SYSTEM_CFG_BLOCK_NAME		"sysconfig"
	#define IFX_CFG_FLASH_SYSTEM_CFG_START_ADDR		0xB0790000
	#define IFX_CFG_FLASH_SYSTEM_CFG_SIZE			0x10000
	#define IFX_CFG_FLASH_SYSTEM_CFG_END_ADDR		0xB079FFFF
	#define IFX_CFG_FLASH_SYSTEM_CFG_MTDBLOCK_NAME		"/dev/mtdblock3"

	#define IFX_CFG_FLASH_FIRMWARE_IMAGE_BLOCK_NAME		"firmware"
	#define IFX_CFG_FLASH_FIRMWARE_IMAGE_START_ADDR         0xB07A0000
	#define IFX_CFG_FLASH_FIRMWARE_IMAGE_SIZE		0x40000
	#define IFX_CFG_FLASH_FIRMWARE_IMAGE_END_ADDR		0xB07DFFFF
	#define IFX_CFG_FLASH_FIRMWARE_IMAGE_MTDBLOCK_NAME	"/dev/mtdblock4"

        #define IFX_CFG_FLASH_UBOOT_CFG_BLOCK_NAME              "ubootconfig"
        #define IFX_CFG_FLASH_UBOOT_CFG_START_ADDR              0xB0020000
        #define IFX_CFG_FLASH_UBOOT_CFG_END_ADDR                0xB002FFFF
        #define IFX_CFG_FLASH_UBOOT_CFG_SIZE                    0x10000
        #define IFX_CFG_FLASH_UBOOT_CFG_MTDBLOCK_NAME           "/dev/mtdblock5"

        #define IFX_CFG_FLASH_END_ADDR                          0xB07FFFFF
#else
	#error "ERROR!! Define flash size first!"
#endif
/* End of Board specific configurations
 *-----------------------------------------------------------------------
 */

#endif

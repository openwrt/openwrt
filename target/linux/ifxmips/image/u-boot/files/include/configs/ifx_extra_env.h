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
 * File Name: ifx_extra_env.h
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
	"mem=" MK_STR(IFX_CONFIG_MEMORY_SIZE) "M\0"
   	"ipaddr=" IFX_CFG_UBOOT_DEFAULT_CFG_IPADDR "\0"
	"serverip=" IFX_CFG_UBOOT_DEFAULT_CFG_SERVERIP "\0"
	"ethaddr=" IFX_CFG_UBOOT_DEFAULT_CFG_ETHADDR "\0"
	"netdev=eth0\0"
	"baudrate=" IFX_CFG_UBOOT_DEFAULT_CFG_BAUDRATE "\0"
	"loadaddr=" IFX_CFG_UBOOT_LOAD_ADDRESS "\0"
	"rootpath=/tftpboot/nfsrootfs\0"
	"nfsargs=setenv bootargs root=/dev/nfs rw nfsroot=$(serverip):$(rootpath)\0"
	"ramargs=setenv bootargs root=/dev/ram rw\0"
	"addip=setenv bootargs $(bootargs) ip=$(ipaddr):$(serverip):$(gatewayip):$(netmask):$(hostname):$(netdev):on\0"
	"addmisc=setenv bootargs $(bootargs) console=ttyS1,$(baudrate) ethaddr=$(ethaddr) mem=$(mem) panic=1\0"
	"flash_nfs=run nfsargs addip addmisc;bootm $(kernel_addr)\0"
	"ramdisk_addr=B0100000\0"
	"flash_self=run ramargs addip addmisc;bootm $(kernel_addr) $(ramdisk_addr)\0"
	"bootfile=uImage\0"
	"net_nfs=tftp $(loadaddr) $(bootfile);run nfsargs addip addmisc;bootm\0"
	"net_flash=tftp $(loadaddr) $(bootfile); run flashargs addip addmisc; bootm\0"
	"u-boot=u-boot.ifx\0"
	"jffs2fs=jffs2.img\0"
        "rootfs=rootfs.img\0"
	"firmware=firmware.img\0"
	"load=tftp $(loadaddr) $(u-boot)\0"
	"update=protect off 1:0-2;era 1:0-2;cp.b $(loadaddr) B0000000 $(filesize)\0"
	"flashargs=setenv bootargs root=/dev/mtdblock2 ro rootfstype=squashfs\0"
	"mtdargs=setenv bootargs root=/dev/mtdblock2 rw rootfstype=jffs2\0"
	"flash_flash=run flashargs addip addmisc; bootm $(f_kernel_addr)\0"
	"net_mtd=tftp $(loadaddr) $(bootfile); run mtdargs addip addmisc; bootm\0"
	"flash_mtd=run mtdargs addip addmisc; bootm $(f_kernel_addr)\0"
	"update_uboot=tftpboot $(loadaddr) $(u-boot);upgrade uboot $(loadaddr) $(filesize) 0\0"
	"update_kernel=tftpboot $(loadaddr) $(bootfile);upgrade kernel $(loadaddr) $(filesize) 0\0"
	"update_rootfs=tftpboot $(loadaddr) $(rootfs); upgrade rootfs $(loadaddr) $(filesize) 0\0"
	"update_rootfs_1=tftpboot $(loadaddr) $(rootfs); erase 1:47-132; cp.b $(loadaddr) $(f_rootfs_addr) $(filesize)\0"
	"update_jffs2=tftpboot $(loadaddr) $(rootfs); upgrade rootfs $(loadaddr) $(filesize) 0\0"
	"update_jffs2_1=tftpboot $(loadaddr) $(jffs2fs); erase 1:47-132; cp.b $(loadaddr) $(f_rootfs_addr) $(filesize)\0"
	"update_firmware=tftpboot $(loadaddr) $(firmware);upgrade firmware $(loadaddr) $(filesize) 0\0"
	"reset_uboot_config=erase " MK_STR(IFX_CFG_FLASH_UBOOT_CFG_START_ADDR) " " MK_STR(IFX_CFG_FLASH_UBOOT_CFG_END_ADDR) "\0" 
	IFX_CFG_FLASH_PARTITIONS_INFO
	"flash_end=" MK_STR(IFX_CFG_FLASH_END_ADDR) "\0"
	IFX_CFG_FLASH_DATA_BLOCKS_INFO
	"f_uboot_addr=" MK_STR(IFX_CFG_FLASH_UBOOT_IMAGE_START_ADDR) "\0"
	"f_uboot_size=" MK_STR(IFX_CFG_FLASH_UBOOT_IMAGE_SIZE) "\0"
	"f_ubootconfig_addr=" MK_STR(IFX_CFG_FLASH_UBOOT_CFG_START_ADDR) "\0"
	"f_ubootconfig_size=" MK_STR(IFX_CFG_FLASH_UBOOT_CFG_SIZE) "\0"
	"f_ubootconfig_end=" MK_STR(IFX_CFG_FLASH_UBOOT_CFG_END_ADDR) "\0"
	"f_kernel_addr=" MK_STR(IFX_CFG_FLASH_KERNEL_IMAGE_START_ADDR) "\0"
	"f_kernel_size=" MK_STR(IFX_CFG_FLASH_KERNEL_IMAGE_SIZE) "\0"
	"f_kernel_end=" MK_STR(IFX_CFG_FLASH_KERNEL_IMAGE_END_ADDR) "\0"
	"f_rootfs_addr=" MK_STR(IFX_CFG_FLASH_ROOTFS_IMAGE_START_ADDR) "\0"
	"f_rootfs_size=" MK_STR(IFX_CFG_FLASH_ROOTFS_IMAGE_SIZE) "\0"
	"f_rootfs_end=" MK_STR(IFX_CFG_FLASH_ROOTFS_IMAGE_END_ADDR) "\0" 
	"f_firmware_addr=" MK_STR(IFX_CFG_FLASH_FIRMWARE_IMAGE_START_ADDR) "\0"
	"f_firmware_size=" MK_STR(IFX_CFG_FLASH_FIRMWARE_IMAGE_SIZE) "\0"
	"f_sysconfig_addr=" MK_STR(IFX_CFG_FLASH_SYSTEM_CFG_START_ADDR) "\0"
	"f_sysconfig_size=" MK_STR(IFX_CFG_FLASH_SYSTEM_CFG_SIZE) "\0"
	/*
	"f_fwdiag_addr=" MK_STR(IFX_CFG_FLASH_FIRMWARE_DIAG_START_ADDR) "\0"
	"f_fwdiag_size=" MK_STR(IFX_CFG_FLASH_FIRMWARE_DIAG_SIZE) "\0"
	"f_calibration_addr=" MK_STR(IFX_CFG_FLASH_CALIBRATION_CFG_START_ADDR) "\0"
	"f_calibration_size=" MK_STR(IFX_CFG_FLASH_CALIBRATION_CFG_SIZE) "\0"
#if (IFX_CONFIG_FLASH_SIZE == 4) || (IFX_CONFIG_FLASH_SIZE == 8) 
	"f_voip_addr=" MK_STR(IFX_CFG_FLASH_VOIP_CFG_START_ADDR) "\0" 
	"f_voip_size=" MK_STR(IFX_CFG_FLASH_VOIP_CFG_SIZE) "\0" 
#endif
	*/

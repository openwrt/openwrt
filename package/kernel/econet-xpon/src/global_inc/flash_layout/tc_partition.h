/************************************************************************
 *
 *	Copyright (C) 2010 Trendchip Technologies, Corp.
 *	All Rights Reserved.
 *
 * Trendchip Confidential; Need to Know only.
 * Protected as an unpublished work.
 *
 * The computer program listings, specifications and documentation
 * herein are the property of Trendchip Technologies, Co. and shall
 * not be reproduced, copied, disclosed, or used in whole or in part
 * for any reason without the prior express written permission of
 * Trendchip Technologeis, Co.
 *
 *************************************************************************/
#ifndef TC_PARTITION_H
#define TC_PARTITION_H
 #ifdef TCSUPPORT_MTD_ENCHANCEMENT
/*
note:		
	1.the read base address of reserve area need to compute by (flash total size -flash erase size * reverse block num) 
		-the base addrass can be obtained by mtd ioctl
		-for example :flash total size is 32 m,flash erase size is 64k,reverse block num is 1,so the read base address of 
		reserve area is 0x3fff0000
	2.reverse block num is according to compile option TCSUPPORT_RESERVEAREA_BLOCK
	TCSUPPORT_RESERVEAREA_BLOCK==1 --------reverse block num is 1
	TCSUPPORT_RESERVEAREA_BLOCK==2 --------reverse block num is 2
	TCSUPPORT_RESERVEAREA_BLOCK==3 --------reverse block num is 3
	TCSUPPORT_RESERVEAREA_BLOCK==4 --------reverse block num is 4
	3.if you modify the size or offset of the certain sector or you add a new sector,you must modify the reserve area table in order to let
	other people be clear.
*/

//********************************
//	operation of  reserve area 	       //
//********************************
#if defined(TCSUPPORT_CPU_EN7512) || defined(TCSUPPORT_CPU_EN7521)
#ifndef __KERNEL__
#include <stdio.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <mtd/mtd-user.h>
#endif
#endif

#define TC_FLASH_READ_CMD		"/userfs/bin/mtd readflash %s %lu %lu %s"
#define TC_FLASH_WRITE_CMD	"/userfs/bin/mtd writeflash %s %lu %lu %s"

#define TC_FLASH_ERASE_SECTOR_CMD "/userfs/bin/mtd erasesector %lu %s"

#define RESERVEAREA_NAME "reservearea"

#if defined(TCSUPPORT_CPU_EN7512) || defined(TCSUPPORT_CPU_EN7521)
#ifndef __KERNEL__
static inline int mtd_reservearea_open(const char *mtd, int flags)
{
	FILE *fp = NULL;
	char dev[128];
	int i;

	if ((fp = fopen("/proc/mtd", "r"))) {
		while (fgets(dev, sizeof(dev), fp)) {
			if (sscanf(dev, "mtd%d:", &i) && strstr(dev, mtd)) {
				snprintf(dev, sizeof(dev), "/dev/mtd%d", i);
				fclose(fp);
				return open(dev, flags);
			}
		}
		fclose(fp);
	}

	return -1;
}
#endif

/* the original NAND_FLASH_BLOCK_SIZE is 0x20000 */
#define ORIGINAL_NAND_FLASH_BLOCK_SIZE (0x20000)
static inline unsigned int runtime_flash_block_size(void)
{
#ifndef __KERNEL__
	int fd, ret;
	mtd_info_t mtd_info;
	

	fd = mtd_reservearea_open(RESERVEAREA_NAME, O_RDONLY);
	if(fd < 0) {
		fprintf(stderr, "Couldn't open mtd:%s!\n", RESERVEAREA_NAME);
		return ORIGINAL_NAND_FLASH_BLOCK_SIZE;
	}

	ret = ioctl(fd, MEMGETINFO, &mtd_info);
	close(fd);
	
	if(ret == 0) {
		return mtd_info.erasesize;
	} else {
		fprintf(stderr, "Couldn't ioctl to %s!\n", RESERVEAREA_NAME);
		return ORIGINAL_NAND_FLASH_BLOCK_SIZE;
	}
#else /* __KERNEL__ */
	return ORIGINAL_NAND_FLASH_BLOCK_SIZE;
#endif /* __KERNEL__ */
}
#define NAND_FLASH_BLOCK_SIZE runtime_flash_block_size()  //this define can be changed baccording to type of nandflash (0x10000/0x20000/0x40000)
#else /* defined(TCSUPPORT_CPU_EN7512) || defined(TCSUPPORT_CPU_EN7521) */
#define NAND_FLASH_BLOCK_SIZE (0x20000)  //this define can be changed baccording to type of nandflash (0x10000/0x20000/0x40000)
#endif /* defined(TCSUPPORT_CPU_EN7512) || defined(TCSUPPORT_CPU_EN7521) */

#if defined(TCSUPPORT_CT)
//********************************
//	sectors define of reserve area 	//
//********************************
#define EEPROM_RA_AC_SIZE 			512
#define EEPROM_RA_AC_RESERVE_SIZE	512
#ifdef TCSUPPORT_NAND_FLASH
#if defined(TCSUPPORT_CT_PON)
#define RESERVEAREA_ERASE_SIZE 0x40000 //this define should be changed baccording to  flash erase size
#else
#define RESERVEAREA_ERASE_SIZE 0x10000 //this define should be changed baccording to  flash erase size
#endif
#else
#define RESERVEAREA_ERASE_SIZE 0x10000 //this define should be changed baccording to  flash erase size
#endif
#define RESERVEAREA_BLOCK_BASE 0

//#if defined( TCSUPPORT_RESERVEAREA_1_BLOCK)

//#elif defined( TCSUPPORT_RESERVEAREA_2_BLOCK)

//#elif defined( TCSUPPORT_RESERVEAREA_3_BLOCK)

#if (TCSUPPORT_RESERVEAREA_BLOCK==1)
/*
---------------------------------------------------------------------------------
@reserve area table@
|sector		name				cover area				note		
------------------------------------------------------------------------------------
*/
#elif (TCSUPPORT_RESERVEAREA_BLOCK==2)
/*
---------------------------------------------------------------------------------
@reserve area table@
|sector		name				cover area				note			
------------------------------------------------------------------------------------
*/
#elif (TCSUPPORT_RESERVEAREA_BLOCK==3)
/*
---------------------------------------------------------------------------------
@reserve area table@
|sector		name				cover area				note		
------------------------------------------------------------------------------------
*/
#elif (TCSUPPORT_RESERVEAREA_BLOCK==4)
//defined( TCSUPPORT_RESERVEAREA_BLOCK == 4)
#if defined(TCSUPPORT_CT_E8B_ADSL) && defined(TCSUPPORT_CPU_MT7505)
/*
---------------------------------------------------------------------------------
@reserve area table@
|sector		name				cover area				note
|0			backupromfile			0~0xffff					64k
|1			temp				0x10000~0x1ffff			64k
|a			cerm1				0x10400~0x113ff			4k
|b			cerm2				0x11400~0x123ff			4k
|c			cerm3				0x12400~0x133ff			4k
|d			cerm4				0x13400~0x143ff			4k
|e			username/passwd		0x14400~0x147ff			1K
|2			syslog				0x20000~0x2ffff			64k
|3			eeprom				0x30000~0x303ff			1k(reserve 1k,no use 256 bytes)
|a			proline				0x30400~0x313ff			4k
------------------------------------------------------------------------------------
*/
#define RESERVEAREA_TOTAL_SIZE RESERVEAREA_ERASE_SIZE*4
/*backupromfile*/
#define BACKUPROMFILE_RA_SIZE 0x10000
#define BACKUPROMFILE_RA_OFFSET RESERVEAREA_BLOCK_BASE
/*temp*/
#define TEMP_RA_SIZE 0x10000
#define TEMP_RA_OFFSET  (BACKUPROMFILE_RA_SIZE+BACKUPROMFILE_RA_OFFSET)
/*cerm1*/
#define CERM1_RA_SIZE 0x1000
#define CERM1_RA_OFFSET (TEMP_RA_OFFSET)
/*cerm2*/
#define CERM2_RA_SIZE 0x1000
#define CERM2_RA_OFFSET (CERM1_RA_SIZE+CERM1_RA_OFFSET)
/*cerm3*/
#define CERM3_RA_SIZE 0x1000
#define CERM3_RA_OFFSET (CERM2_RA_SIZE+CERM2_RA_OFFSET)
/*cerm4*/
#define CERM4_RA_SIZE 0x1000
#define CERM4_RA_OFFSET (CERM3_RA_SIZE+CERM3_RA_OFFSET)
#if defined(TCSUPPORT_CT_BOOTLOADER_UPGRADE)
/*username/passwd*/
#define USERNAMEPASSWD_RA_SIZE 0x400
#define USERNAMEPASSWD_RA_OFFSET (CERM4_RA_SIZE+CERM4_RA_OFFSET)
#else
#define USERNAMEPASSWD_RA_SIZE 0x0
#define USERNAMEPASSWD_RA_OFFSET (CERM4_RA_SIZE+CERM4_RA_OFFSET)
#endif
/*syslog*/
#define SYSLOG_RA_SIZE 0x10000
#define SYSLOG_RA_OFFSET (TEMP_RA_SIZE+TEMP_RA_OFFSET)
/*eeprom*/
#define EEPROM_RA_SIZE 0x400
#define EEPROM_RA_OFFSET (SYSLOG_RA_SIZE+SYSLOG_RA_OFFSET)
#ifdef TCSUPPORT_PRODUCTIONLINE
#define PROLINE_CWMPPARA_RA_SIZE 0x1000
#define PROLINE_CWMPPARA_RA_OFFSET (EEPROM_RA_SIZE+EEPROM_RA_OFFSET)
#else
#define PROLINE_CWMPPARA_RA_SIZE 0x0
#define PROLINE_CWMPPARA_RA_OFFSET (EEPROM_RA_SIZE+EEPROM_RA_OFFSET)
#endif
#else
/*
---------------------------------------------------------------------------------
@reserve area table@
|sector		name				cover area				note
|0			backupromfile			0~0xffff					64k
|1			defaultromfile			0x10000~0x1ffff			64k
|2			syslog				0x20000~0x2ffff			64k
|3			eeprom				0x30000~0x303ff			1k(reserve 1k,no use 256 bytes)
|4			cerm1				0x30400~0x313ff			4k
|5			cerm2				0x31400~0x323ff			4k
|6			cerm3				0x32400~0x333ff			4k
|7			cerm4				0x33400~0x343ff			4k
|8			username/passwd		0x34400~0x347ff			1K
------------------------------------------------------------------------------------
*/
#define RESERVEAREA_TOTAL_SIZE RESERVEAREA_ERASE_SIZE*4
/*backupromfile*/
#define BACKUPROMFILE_RA_SIZE 0x10000
#define BACKUPROMFILE_RA_OFFSET RESERVEAREA_BLOCK_BASE
/*defaultromfile*/
#define DEFAULTROMFILE_RA_SIZE 0x10000
#define DEFAULTROMFILE_RA_OFFSET (BACKUPROMFILE_RA_OFFSET+BACKUPROMFILE_RA_SIZE)
/*syslog*/
#define SYSLOG_RA_SIZE 0x10000
#define SYSLOG_RA_OFFSET (DEFAULTROMFILE_RA_OFFSET+DEFAULTROMFILE_RA_SIZE)
/*eeprom*/
#define EEPROM_RA_SIZE 0x400
#define EEPROM_RA_OFFSET (SYSLOG_RA_OFFSET+SYSLOG_RA_SIZE)
/*cerm1*/
#define CERM1_RA_SIZE 0x1000
#define CERM1_RA_OFFSET (EEPROM_RA_SIZE+EEPROM_RA_OFFSET)
/*cerm2*/
#define CERM2_RA_SIZE 0x1000
#define CERM2_RA_OFFSET (CERM1_RA_SIZE+CERM1_RA_OFFSET)
/*cerm3*/
#define CERM3_RA_SIZE 0x1000
#define CERM3_RA_OFFSET (CERM2_RA_SIZE+CERM2_RA_OFFSET)
/*cerm4*/
#define CERM4_RA_SIZE 0x1000
#define CERM4_RA_OFFSET (CERM3_RA_SIZE+CERM3_RA_OFFSET)
#if defined(TCSUPPORT_CT_BOOTLOADER_UPGRADE)
/*username/passwd*/
#define USERNAMEPASSWD_RA_SIZE 0x400
#define USERNAMEPASSWD_RA_OFFSET (CERM4_RA_SIZE+CERM4_RA_OFFSET)
#else
#define USERNAMEPASSWD_RA_SIZE 0x0
#define USERNAMEPASSWD_RA_OFFSET (CERM4_RA_SIZE+CERM4_RA_OFFSET)
#endif
#ifdef TCSUPPORT_PRODUCTIONLINE
#if defined(TCSUPPORT_C7)
#define PROLINE_CWMPPARA_RA_SIZE 0x1000
#else
#define PROLINE_CWMPPARA_RA_SIZE 0x180
#endif
#define PROLINE_CWMPPARA_RA_OFFSET (USERNAMEPASSWD_RA_SIZE+USERNAMEPASSWD_RA_OFFSET)
#else
#define PROLINE_CWMPPARA_RA_SIZE 0x0
#define PROLINE_CWMPPARA_RA_OFFSET (USERNAMEPASSWD_RA_SIZE+USERNAMEPASSWD_RA_OFFSET)
#endif
#endif
#else
//defined( TCSUPPORT_RESERVEAREA_BLOCK ==7)
#ifdef TCSUPPORT_NAND_FLASH
#if defined(TCSUPPORT_CT_PON)
/*
---------------------------------------------------------------------------------
@reserve area table 7 BLOCK@
|sector		name				cover area				note
|1			backupromfile			0~0x3ffff					256k
|2			defaultromfile			0x40000~0x7ffff			256k
|3			syslog				0x80000~0xBffff			256k
|4			proline      			0xc0000~0xfffff			256k
|5			temp				0x100000~0x13ffff			256k
|5.1			cerm1				0x100000~0x100fff		4k
|5.2			cerm2				0x101000~0x101fff		4k
|5.3			cerm3				0x102000~0x102fff		4k
|5.4			cerm4				0x103000~0x103fff		4k
|6			block6				0x140000~0x17ffff			256k
|6.1			eeprom				0x140000~0x1403ff		1k(reserve 1k,no use 256 bytes)
|6.2			bob.conf				0x140400~0x14049f		160bytes
|7			block7				0x180000~0x1bffff			256k
|7.1			imgbootflag			0x180000~0x18003f		64bytes
|7.2			11ac					0x180040~0x18023f		512bytes
|7.3			11ac	 reserved			0x180240~0x18043f		512bytes
------------------------------------------------------------------------------------
*/
#define RESERVEAREA_TOTAL_SIZE RESERVEAREA_ERASE_SIZE*7
/*backupromfile*/
#define BACKUPROMFILE_RA_SIZE 0x40000
#define BACKUPROMFILE_RA_OFFSET RESERVEAREA_BLOCK_BASE
/*defaultromfile*/
#define DEFAULTROMFILE_RA_SIZE 0x40000
#define DEFAULTROMFILE_RA_OFFSET (BACKUPROMFILE_RA_OFFSET+BACKUPROMFILE_RA_SIZE)
/*syslog*/
#define SYSLOG_RA_SIZE 0x40000
#define SYSLOG_RA_OFFSET (DEFAULTROMFILE_RA_OFFSET+DEFAULTROMFILE_RA_SIZE)
/*product para*/
#define PROLINE_CWMPPARA_RA_SIZE 0x40000
#define PROLINE_CWMPPARA_RA_OFFSET (SYSLOG_RA_OFFSET+SYSLOG_RA_SIZE)
/*temp data*/
#define TEMP_RA_SIZE 0x40000
#define TEMP_RA_OFFSET (PROLINE_CWMPPARA_RA_OFFSET+PROLINE_CWMPPARA_RA_SIZE)
/*cerm1*/
#define CERM1_RA_SIZE 0x1000
#define CERM1_RA_OFFSET (TEMP_RA_OFFSET)
/*cerm2*/
#define CERM2_RA_SIZE 0x1000
#define CERM2_RA_OFFSET (CERM1_RA_OFFSET+CERM1_RA_SIZE)
/*cerm3*/
#define CERM3_RA_SIZE 0x1000
#define CERM3_RA_OFFSET (CERM2_RA_OFFSET+CERM2_RA_SIZE)
/*cerm4*/
#define CERM4_RA_SIZE 0x1000
#define CERM4_RA_OFFSET (CERM3_RA_OFFSET+CERM3_RA_SIZE)
/*eeprom*/
#define EEPROM_RA_SIZE 0x400
#define EEPROM_RA_OFFSET (TEMP_RA_OFFSET+TEMP_RA_SIZE)
/*bob info*/
#define BOB_RA_SIZE 0xa0
#define BOB_RA_OFFSET  (EEPROM_RA_OFFSET+EEPROM_RA_SIZE)
/*image boot flag*/
#define IMG_BOOT_FLAG_SIZE 	1
#define IMG_BOOT_FLAG_OFFSET  	(EEPROM_RA_OFFSET + RESERVEAREA_ERASE_SIZE)
#define IMG_BOOT_FLAG_RESERVE_SIZE 		63
#define IMG_BOOT_FLAG_RESERVE_OFFSET	(IMG_BOOT_FLAG_OFFSET + IMG_BOOT_FLAG_SIZE)
#define EEPROM_RA_AC_OFFSET				(IMG_BOOT_FLAG_RESERVE_OFFSET+IMG_BOOT_FLAG_RESERVE_SIZE)
#define EEPROM_RA_AC_RESERVE_OFFSET		(EEPROM_RA_AC_OFFSET+EEPROM_RA_AC_SIZE)
#else
/*
---------------------------------------------------------------------------------
@reserve area table@
|sector		name				cover area				note
|0			backupromfile			0~0xffff					64k
|1			defaultromfile			0x10000~0x1ffff			64k
|2			syslog				0x20000~0x2ffff			64k
|3			proline      			0x30000~0x3ffff			64k
|4			temp				0x40000~0x4ffff			64k
|5			cerm1				0x40000~0x40fff			4k
|6			cerm2				0x41000~0x41fff			4k
|7			cerm3				0x42000~0x42fff			4k
|8			cerm4				0x43000~0x43fff			4k
|9			eeprom				0x50000~0x503ff			1k(reserve 1k,no use 256 bytes)
|10			imgbootflag			0x60000~0x6ffff			64k
------------------------------------------------------------------------------------
*/
#define RESERVEAREA_TOTAL_SIZE RESERVEAREA_ERASE_SIZE*7
/*backupromfile*/
#define BACKUPROMFILE_RA_SIZE 0x10000
#define BACKUPROMFILE_RA_OFFSET RESERVEAREA_BLOCK_BASE
/*defaultromfile*/
#define DEFAULTROMFILE_RA_SIZE 0x10000
#define DEFAULTROMFILE_RA_OFFSET (BACKUPROMFILE_RA_OFFSET+BACKUPROMFILE_RA_SIZE)
/*syslog*/
#define SYSLOG_RA_SIZE 0x10000
#define SYSLOG_RA_OFFSET (DEFAULTROMFILE_RA_OFFSET+DEFAULTROMFILE_RA_SIZE)
/*product para*/
#define PROLINE_CWMPPARA_RA_SIZE 0x10000
#define PROLINE_CWMPPARA_RA_OFFSET (SYSLOG_RA_OFFSET+SYSLOG_RA_SIZE)
/*temp data*/
#define TEMP_RA_SIZE 0x10000
#define TEMP_RA_OFFSET (PROLINE_CWMPPARA_RA_OFFSET+PROLINE_CWMPPARA_RA_SIZE)
/*cerm1*/
#define CERM1_RA_SIZE 0x1000
#define CERM1_RA_OFFSET (TEMP_RA_OFFSET)
/*cerm2*/
#define CERM2_RA_SIZE 0x1000
#define CERM2_RA_OFFSET (CERM1_RA_OFFSET+CERM1_RA_SIZE)
/*cerm3*/
#define CERM3_RA_SIZE 0x1000
#define CERM3_RA_OFFSET (CERM2_RA_OFFSET+CERM2_RA_SIZE)
/*cerm4*/
#define CERM4_RA_SIZE 0x1000
#define CERM4_RA_OFFSET (CERM3_RA_OFFSET+CERM3_RA_SIZE)
/*eeprom*/
#define EEPROM_RA_SIZE 0x400
#define EEPROM_RA_OFFSET (TEMP_RA_OFFSET+TEMP_RA_SIZE)
/*bob info*/
#define BOB_RA_SIZE 0xa0
#define BOB_RA_OFFSET  (0x600)
/*image boot flag*/
#define IMG_BOOT_FLAG_SIZE 	1
#define IMG_BOOT_FLAG_OFFSET  	(EEPROM_RA_OFFSET + RESERVEAREA_ERASE_SIZE)
#endif
#else
/*
---------------------------------------------------------------------------------
@reserve area table@
|sector		name				cover area				note
|0			backupromfile			0~0xffff					64k
|1			defaultromfile			0x10000~0x1ffff			64k
|2			syslog				0x20000~0x2ffff			64k
|3			proline      			0x30000~0x3ffff			64k
|4			temp				0x40000~0x4ffff			64k
|5			cerm1				0x40000~0x40fff			4k
|6			cerm2				0x41000~0x41fff			4k
|7			cerm3				0x42000~0x42fff			4k
|8			cerm4				0x43000~0x43fff			4k
|9			eeprom				0x50000~0x503ff			1k(reserve 1k,no use 256 bytes)
|10			imgbootflag			0x60000~0x6ffff			64k
------------------------------------------------------------------------------------
*/
#define RESERVEAREA_TOTAL_SIZE RESERVEAREA_ERASE_SIZE*7
/*backupromfile*/
#define BACKUPROMFILE_RA_SIZE 0x10000
#define BACKUPROMFILE_RA_OFFSET RESERVEAREA_BLOCK_BASE
/*defaultromfile*/
#define DEFAULTROMFILE_RA_SIZE 0x10000
#define DEFAULTROMFILE_RA_OFFSET (BACKUPROMFILE_RA_OFFSET+BACKUPROMFILE_RA_SIZE)
/*syslog*/
#define SYSLOG_RA_SIZE 0x10000
#define SYSLOG_RA_OFFSET (DEFAULTROMFILE_RA_OFFSET+DEFAULTROMFILE_RA_SIZE)
/*product para*/
#define PROLINE_CWMPPARA_RA_SIZE 0x10000
#define PROLINE_CWMPPARA_RA_OFFSET (SYSLOG_RA_OFFSET+SYSLOG_RA_SIZE)
/*temp data*/
#define TEMP_RA_SIZE 0x10000
#define TEMP_RA_OFFSET (PROLINE_CWMPPARA_RA_OFFSET+PROLINE_CWMPPARA_RA_SIZE)
/*cerm1*/
#define CERM1_RA_SIZE 0x1000
#define CERM1_RA_OFFSET (TEMP_RA_OFFSET)
/*cerm2*/
#define CERM2_RA_SIZE 0x1000
#define CERM2_RA_OFFSET (CERM1_RA_OFFSET+CERM1_RA_SIZE)
/*cerm3*/
#define CERM3_RA_SIZE 0x1000
#define CERM3_RA_OFFSET (CERM2_RA_OFFSET+CERM2_RA_SIZE)
/*cerm4*/
#define CERM4_RA_SIZE 0x1000
#define CERM4_RA_OFFSET (CERM3_RA_OFFSET+CERM3_RA_SIZE)
/*eeprom*/
#define EEPROM_RA_SIZE 0x400
#define EEPROM_RA_OFFSET (TEMP_RA_OFFSET+TEMP_RA_SIZE)
/*bob info*/
#define BOB_RA_SIZE 0xa0
#define BOB_RA_OFFSET  (EEPROM_RA_OFFSET+EEPROM_RA_SIZE)
/*image boot flag*/
#define IMG_BOOT_FLAG_SIZE 	1
#define IMG_BOOT_FLAG_OFFSET  	(EEPROM_RA_OFFSET + RESERVEAREA_ERASE_SIZE)
#endif
#endif
#else
//********************************
//	sectors define of reserve area 	//
//********************************
#ifdef TCSUPPORT_NAND_BMT
#define RESERVEAREA_ERASE_SIZE NAND_FLASH_BLOCK_SIZE //this define should be changed baccording to  flash erase size  
#else
#define RESERVEAREA_ERASE_SIZE 0x10000 //this define should be changed baccording to  flash erase size
#endif
#define RESERVEAREA_BLOCK_BASE 0

//#if defined( TCSUPPORT_RESERVEAREA_1_BLOCK)

//#elif defined( TCSUPPORT_RESERVEAREA_2_BLOCK)

//#elif defined( TCSUPPORT_RESERVEAREA_3_BLOCK)

#if (TCSUPPORT_RESERVEAREA_BLOCK==1)
/*
---------------------------------------------------------------------------------
@reserve area table@
|sector		name				cover area				note		
|0			eeprom				0x00000~0x003ff			1k(reserve 1k,no use 256 bytes)
|1			cwmppara			0x00400~0x00600			512 Bytes
|2			not use now			0x00601~0x00fff			(4k-1k-512 Bytes)
|3			mrd					0x01000~0x01fff			4k
|4			rom-t				0x02000~0x0ffff			56k
------------------------------------------------------------------------------------
*/
#define RESERVEAREA_TOTAL_SIZE RESERVEAREA_ERASE_SIZE*1

/*eeprom*/
#define EEPROM_RA_SIZE 0x400
#define EEPROM_RA_OFFSET RESERVEAREA_BLOCK_BASE 

/*device information*/
#ifdef TCSUPPORT_PRODUCTIONLINE
#define PROLINE_CWMPPARA_RA_SIZE 0x200
#define PROLINE_CWMPPARA_RA_OFFSET (EEPROM_RA_OFFSET+EEPROM_RA_SIZE)
#else
#define PROLINE_CWMPPARA_RA_SIZE 0x0
#define PROLINE_CWMPPARA_RA_OFFSET (EEPROM_RA_OFFSET+EEPROM_RA_SIZE)
#endif

#define MRD_RA_SIZE 0x1000
#define MRD_RA_OFFSET (EEPROM_RA_OFFSET + 0x1000) 

#define ROM_T_RA_SIZE 0xe000
#define ROM_T_RA_OFFSET (MRD_RA_OFFSET+MRD_RA_SIZE)

#elif (TCSUPPORT_RESERVEAREA_BLOCK==2)
/*
---------------------------------------------------------------------------------
@reserve area table@
|sector		name				cover area				note
|0			backupromfile 		0~0xffff					64k
|1			syslog				0x10000~0x10000			
|2			eeprom				0x10000~0x103ff			1k(reserve 1k,no use 256 bytes)
|3			cwmppara			0x00400~0x00600			512 Bytes
|4			not use now			0x00601~0x00fff			(4k-1k-512 Bytes)
|5			mrd				0x01000~0x01fff			4k
|6			rom-t				0x02000~0x0ffff			56k
------------------------------------------------------------------------------------
*/
#define RESERVEAREA_TOTAL_SIZE RESERVEAREA_ERASE_SIZE*2
/*backupromfile*/
#define BACKUPROMFILE_RA_SIZE 0x10000
#define BACKUPROMFILE_RA_OFFSET RESERVEAREA_BLOCK_BASE
/*defaultromfile*/
//#define DEFAULTROMFILE_RA_SIZE 0x0
//#define DEFAULTROMFILE_RA_OFFSET (BACKUPROMFILE_RA_OFFSET+BACKUPROMFILE_RA_SIZE)
/*syslog*/
#define SYSLOG_RA_SIZE 0x0
//#define SYSLOG_RA_OFFSET (DEFAULTROMFILE_RA_OFFSET+DEFAULTROMFILE_RA_SIZE)
#define SYSLOG_RA_OFFSET (BACKUPROMFILE_RA_OFFSET+BACKUPROMFILE_RA_SIZE)
/*eeprom*/
#define EEPROM_RA_SIZE 0x400
#define EEPROM_RA_OFFSET (SYSLOG_RA_OFFSET+SYSLOG_RA_SIZE)

/*device information*/
#ifdef TCSUPPORT_PRODUCTIONLINE
#define PROLINE_CWMPPARA_RA_SIZE 0x200
#define PROLINE_CWMPPARA_RA_OFFSET (EEPROM_RA_OFFSET+EEPROM_RA_SIZE)
#else
#define PROLINE_CWMPPARA_RA_SIZE 0x0
#define PROLINE_CWMPPARA_RA_OFFSET (EEPROM_RA_OFFSET+EEPROM_RA_SIZE)
#endif

#define MRD_RA_SIZE 0x1000
#define MRD_RA_OFFSET (EEPROM_RA_OFFSET + 0x1000)

#define ROM_T_RA_SIZE 0xe000
#define ROM_T_RA_OFFSET (MRD_RA_OFFSET+MRD_RA_SIZE)

#elif (TCSUPPORT_RESERVEAREA_BLOCK==3)
/*
---------------------------------------------------------------------------------
@reserve area table@
|sector		name				cover area				note
|0			backupromfile 		0~0xffff					64k
|1			defaultromfile		0x10000~0x1ffff			64k
|2			eeprom			0x20000~0x203ff			1k(reserve 1k,no use 256 bytes)
|3			cwmppara			0x00400~0x00600			384 Bytes
|4			not use now			0x00601~0x00fff			(4k-1k-512 Bytes)
|5			mrd					0x01000~0x01fff			4k
|6			rom-t				0x02000~0x0ffff			56k
------------------------------------------------------------------------------------
*/
#define RESERVEAREA_TOTAL_SIZE RESERVEAREA_ERASE_SIZE*3
/*backupromfile*/
#define BACKUPROMFILE_RA_SIZE 0x10000
#define BACKUPROMFILE_RA_OFFSET RESERVEAREA_BLOCK_BASE
/*defaultromfile*/
#define DEFAULTROMFILE_RA_SIZE 0x10000
#define DEFAULTROMFILE_RA_OFFSET (BACKUPROMFILE_RA_OFFSET+BACKUPROMFILE_RA_SIZE)
/*eeprom*/
#define EEPROM_RA_SIZE 0x400
#define EEPROM_RA_OFFSET (DEFAULTROMFILE_RA_SIZE+DEFAULTROMFILE_RA_OFFSET)

/*device information*/
#ifdef TCSUPPORT_PRODUCTIONLINE
#define PROLINE_CWMPPARA_RA_SIZE 0x200
#define PROLINE_CWMPPARA_RA_OFFSET (EEPROM_RA_OFFSET+EEPROM_RA_SIZE)
#else
#define PROLINE_CWMPPARA_RA_SIZE 0x0
#define PROLINE_CWMPPARA_RA_OFFSET (EEPROM_RA_OFFSET+EEPROM_RA_SIZE)
#endif

#define MRD_RA_SIZE 0x1000
#define MRD_RA_OFFSET (EEPROM_RA_OFFSET + 0x1000)

#define ROM_T_RA_SIZE 0xe000
#define ROM_T_RA_OFFSET (MRD_RA_OFFSET+MRD_RA_SIZE)

//#else
#elif (TCSUPPORT_RESERVEAREA_BLOCK==4)
//defined( TCSUPPORT_RESERVEAREA_BLOCK == 4)
#ifdef TCSUPPORT_NAND_BMT

/*
---------------------------------------------------------------------------------
@reserve area table@
|sector		name				cover area				note
|0			backupromfile 		0~0x1ffff					128k
|1			defaultromfile			0x20000~0x3ffff			128k
|2			syslog				0x40000~0x5ffff			128k
|3			eeprom				0x60000~0x603ff			1k(reserve 1k,no use 256 bytes)
|4			cwmppara			0x60400~0x60600			512 Bytes
|5			not use now			0x60601~0x60fff			(4k-1k-512 Bytes)
|6			mrd					0x61000~0x61fff			4k
|7			rom-t				0x62000~0x6ffff			56k
------------------------------------------------------------------------------------
*/
#define RESERVEAREA_TOTAL_SIZE RESERVEAREA_ERASE_SIZE*4
#ifdef TCSUPPORT_WLAN_AC
#define MAX_EEPROM_BIN_FILE_SIZE	512
#endif
/*backupromfile*/
#define BACKUPROMFILE_RA_SIZE RESERVEAREA_ERASE_SIZE
#define BACKUPROMFILE_RA_OFFSET RESERVEAREA_BLOCK_BASE
/*defaultromfile*/
#define DEFAULTROMFILE_RA_SIZE RESERVEAREA_ERASE_SIZE
#define DEFAULTROMFILE_RA_OFFSET (BACKUPROMFILE_RA_OFFSET+BACKUPROMFILE_RA_SIZE)
/*syslog*/
#define SYSLOG_RA_SIZE RESERVEAREA_ERASE_SIZE
#define SYSLOG_RA_OFFSET (DEFAULTROMFILE_RA_OFFSET+DEFAULTROMFILE_RA_SIZE)
/*eeprom*/
#define EEPROM_RA_SIZE 0x400
#define EEPROM_RA_OFFSET (SYSLOG_RA_OFFSET+SYSLOG_RA_SIZE)
#ifdef TCSUPPORT_WLAN_AC
#ifndef TCSUPPORT_EEPROM_ACEXT
#define EEPROM_RA_AC_OFFSET (EEPROM_RA_OFFSET+MAX_EEPROM_BIN_FILE_SIZE)
#endif
#endif

/*device information*/
#ifdef TCSUPPORT_PRODUCTIONLINE
#define PROLINE_CWMPPARA_RA_SIZE 0x200
#define PROLINE_CWMPPARA_RA_OFFSET (EEPROM_RA_OFFSET+EEPROM_RA_SIZE)
#else
#define PROLINE_CWMPPARA_RA_SIZE 0x0
#define PROLINE_CWMPPARA_RA_OFFSET (EEPROM_RA_OFFSET+EEPROM_RA_SIZE)
#endif
/*for MT7615 use cause the size of MT7615 EEPROM is 1k*/
#ifdef TCSUPPORT_WLAN_AC
#ifdef TCSUPPORT_EEPROM_ACEXT   
#define EEPROM_RA_AC_OFFSET (PROLINE_CWMPPARA_RA_OFFSET+PROLINE_CWMPPARA_RA_SIZE)
#endif
#endif

#define MRD_RA_SIZE 0x1000
#define MRD_RA_OFFSET (EEPROM_RA_OFFSET + 0x1000)

#define ROM_T_RA_SIZE 0xe000
#define ROM_T_RA_OFFSET (MRD_RA_OFFSET+MRD_RA_SIZE)


#else

/*
---------------------------------------------------------------------------------
@reserve area table@
|sector		name				cover area				note
|0			backupromfile 		0~0xffff					64k
|1			defaultromfile			0x10000~0x1ffff			64k
|2			syslog				0x20000~0x2ffff			64k
|3			eeprom				0x30000~0x303ff			1k(reserve 1k,no use 256 bytes)
|4			cwmppara			0x00400~0x00600			512 Bytes
|5			not use now			0x00601~0x00fff			(4k-1k-512 Bytes)
|6			mrd					0x01000~0x01fff			4k
|7			rom-t				0x02000~0x0ffff			56k
------------------------------------------------------------------------------------
*/
#define RESERVEAREA_TOTAL_SIZE RESERVEAREA_ERASE_SIZE*4
#ifdef TCSUPPORT_WLAN_AC
#define MAX_EEPROM_BIN_FILE_SIZE	512
#endif
/*backupromfile*/
#define BACKUPROMFILE_RA_SIZE 0x10000
#define BACKUPROMFILE_RA_OFFSET RESERVEAREA_BLOCK_BASE
/*defaultromfile*/
#define DEFAULTROMFILE_RA_SIZE 0x10000
#define DEFAULTROMFILE_RA_OFFSET (BACKUPROMFILE_RA_OFFSET+BACKUPROMFILE_RA_SIZE)
/*syslog*/
#define SYSLOG_RA_SIZE 0x10000
#define SYSLOG_RA_OFFSET (DEFAULTROMFILE_RA_OFFSET+DEFAULTROMFILE_RA_SIZE)
/*eeprom*/
#define EEPROM_RA_SIZE 0x400
#define EEPROM_RA_OFFSET (SYSLOG_RA_OFFSET+SYSLOG_RA_SIZE)
#ifdef TCSUPPORT_WLAN_AC
#ifndef TCSUPPORT_EEPROM_ACEXT
#define EEPROM_RA_AC_OFFSET (EEPROM_RA_OFFSET+MAX_EEPROM_BIN_FILE_SIZE)
#endif
#endif

/*device information*/
#ifdef TCSUPPORT_PRODUCTIONLINE
#define PROLINE_CWMPPARA_RA_SIZE 0x200
#define PROLINE_CWMPPARA_RA_OFFSET (EEPROM_RA_OFFSET+EEPROM_RA_SIZE)
#else
#define PROLINE_CWMPPARA_RA_SIZE 0x0
#define PROLINE_CWMPPARA_RA_OFFSET (EEPROM_RA_OFFSET+EEPROM_RA_SIZE)
#endif

/*for MT7615 use cause the size of MT7615 EEPROM is 1k*/
#ifdef TCSUPPORT_WLAN_AC
#ifdef TCSUPPORT_EEPROM_ACEXT   
#define EEPROM_RA_AC_OFFSET (PROLINE_CWMPPARA_RA_OFFSET+PROLINE_CWMPPARA_RA_SIZE)
#endif
#endif

#define MRD_RA_SIZE 0x1000
#define MRD_RA_OFFSET (EEPROM_RA_OFFSET + 0x1000)

#define ROM_T_RA_SIZE 0xe000
#define ROM_T_RA_OFFSET (MRD_RA_OFFSET+MRD_RA_SIZE)
#endif


#else
//defined( TCSUPPORT_RESERVEAREA_BLOCK == 5)
#ifdef TCSUPPORT_NAND_BMT
/*
---------------------------------------------------------------------------------
@reserve area table@
|sector		name				cover area				note
|0			backupromfile			0~0x3ffff					256k
|1			gpon_bob			0x40000~0x5ffff			128k
|2			syslog				0x60000~0x7ffff			128k
|3			eeprom				0x80000~0x803ff			1k(reserve 1k,no use 256 bytes)
|4			cerm1				0x80400~0x813ff			4k
|5			cerm2				0x81400~0x823ff			4k
|6			cerm3				0x82400~0x833ff			4k
|7			cerm4				0x83400~0x843ff			4k
|8			username/passwd		0x84400~0x847ff			1K
|9			mrd					0x80000~0x80fff			4k
|10			imgbootflag			0x85800~0x85801			

------------------------------------------------------------------------------------
*/
#define RESERVEAREA_TOTAL_SIZE RESERVEAREA_ERASE_SIZE*5
/*backupromfile*/
#define BACKUPROMFILE_RA_SIZE NAND_FLASH_BLOCK_SIZE*2
#define BACKUPROMFILE_RA_OFFSET RESERVEAREA_BLOCK_BASE
/*defaultromfile*/
#define GPON_BOB_SIZE NAND_FLASH_BLOCK_SIZE
#define GPON_BOB_OFFSET (BACKUPROMFILE_RA_OFFSET+BACKUPROMFILE_RA_SIZE)
/*syslog*/
#define SYSLOG_RA_SIZE NAND_FLASH_BLOCK_SIZE
#define SYSLOG_RA_OFFSET (GPON_BOB_OFFSET+GPON_BOB_SIZE)
/*eeprom*/
#define EEPROM_RA_SIZE 0x400
#define EEPROM_RA_OFFSET (SYSLOG_RA_OFFSET+SYSLOG_RA_SIZE)
/*cerm1*/
#define CERM1_RA_SIZE 0x1000
#define CERM1_RA_OFFSET (EEPROM_RA_SIZE+EEPROM_RA_OFFSET)
/*cerm2*/
#define CERM2_RA_SIZE 0x1000
#define CERM2_RA_OFFSET (CERM1_RA_SIZE+CERM1_RA_OFFSET)
/*cerm3*/
#define CERM3_RA_SIZE 0x1000
#define CERM3_RA_OFFSET (CERM2_RA_SIZE+CERM2_RA_OFFSET)
/*cerm4*/
#define CERM4_RA_SIZE 0x1000
#define CERM4_RA_OFFSET (CERM3_RA_SIZE+CERM3_RA_OFFSET)
#if defined(TCSUPPORT_CT_BOOTLOADER_UPGRADE)
/*username/passwd*/
#define USERNAMEPASSWD_RA_SIZE 0x400
#define USERNAMEPASSWD_RA_OFFSET (CERM4_RA_SIZE+CERM4_RA_OFFSET)
#else
#define USERNAMEPASSWD_RA_SIZE 0x0
#define USERNAMEPASSWD_RA_OFFSET (CERM4_RA_SIZE+CERM4_RA_OFFSET)
#endif
#ifdef TCSUPPORT_PRODUCTIONLINE
#define PROLINE_CWMPPARA_RA_SIZE 0x180
#define PROLINE_CWMPPARA_RA_OFFSET (USERNAMEPASSWD_RA_SIZE+USERNAMEPASSWD_RA_OFFSET)
#else
#define PROLINE_CWMPPARA_RA_SIZE 0x0
#define PROLINE_CWMPPARA_RA_OFFSET (USERNAMEPASSWD_RA_SIZE+USERNAMEPASSWD_RA_OFFSET)
#endif

#define MRD_RA_SIZE 0x1000
#define MRD_RA_OFFSET (EEPROM_RA_OFFSET + 0x1000)

/*image boot flag*/
#define IMG_BOOT_FLAG_SIZE 	1
#define IMG_BOOT_FLAG_OFFSET  	(PROLINE_CWMPPARA_RA_SIZE+PROLINE_CWMPPARA_RA_OFFSET)

#endif



#endif

#ifdef TCSUPPORT_NAND_BADBLOCK_CHECK
/*syslog*/
#define SYSLOG_RA_SIZE 0x20000
#define SYSLOG_RA_OFFSET RESERVEAREA_BLOCK_BASE

/*backupromfile*/
#define BACKUPROMFILE_RA_SIZE 0x20000
#define BACKUPROMFILE_RA_OFFSET (SYSLOG_RA_OFFSET + SYSLOG_RA_SIZE * 11)

/*eeprom*/
#define EEPROM_RA_SIZE 0x400
#define EEPROM_RA_OFFSET (BACKUPROMFILE_RA_OFFSET + BACKUPROMFILE_RA_SIZE * 15)
#endif

#endif
#endif
#endif

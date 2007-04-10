/*
<:copyright-gpl 
 Copyright 2002 Broadcom Corp. All Rights Reserved. 
 
 This program is free software; you can distribute it and/or modify it 
 under the terms of the GNU General Public License (Version 2) as 
 published by the Free Software Foundation. 
 
 This program is distributed in the hope it will be useful, but WITHOUT 
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or 
 FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License 
 for more details. 
 
 You should have received a copy of the GNU General Public License along 
 with this program; if not, write to the Free Software Foundation, Inc., 
 59 Temple Place - Suite 330, Boston MA 02111-1307, USA. 
:>
*/
/***********************************************************************/
/*                                                                     */
/*   MODULE:  board.h                                                  */
/*   DATE:    97/02/18                                                 */
/*   PURPOSE: Board specific information.  This module should include  */
/*            all base device addresses and board specific macros.     */
/*                                                                     */
/***********************************************************************/
#ifndef _BOARD_H
#define _BOARD_H

/*****************************************************************************/
/*                    Misc board definitions                                 */
/*****************************************************************************/

#define	DYING_GASP_API

/*****************************************************************************/
/*                    Physical Memory Map                                    */
/*****************************************************************************/

#define PHYS_DRAM_BASE           0x00000000     /* Dynamic RAM Base */
#define PHYS_FLASH_BASE          0x1FC00000     /* Flash Memory         */

/*****************************************************************************/
/* Note that the addresses above are physical addresses and that programs    */
/* have to use converted addresses defined below:                            */
/*****************************************************************************/
#define DRAM_BASE           (0x80000000 | PHYS_DRAM_BASE)   /* cached DRAM */
#define DRAM_BASE_NOCACHE   (0xA0000000 | PHYS_DRAM_BASE)   /* uncached DRAM */
#define FLASH_BASE          (0xA0000000 | PHYS_FLASH_BASE)  /* uncached Flash  */

/*****************************************************************************/
/*  Select the PLL value to get the desired CPU clock frequency.             */
/*                                                                           */
/*                                                                           */
/*****************************************************************************/
#define FPERIPH            50000000

#define ONEK                            1024
#define BLK64K                          (64*ONEK)
#define FLASH45_BLKS_BOOT_ROM           1
#define FLASH45_LENGTH_BOOT_ROM         (FLASH45_BLKS_BOOT_ROM * BLK64K)
#define FLASH_RESERVED_AT_END           (64*ONEK) /*reserved for PSI, scratch pad*/
    
/*****************************************************************************/
/* Note that the addresses above are physical addresses and that programs    */
/* have to use converted addresses defined below:                            */
/*****************************************************************************/
#define DRAM_BASE           (0x80000000 | PHYS_DRAM_BASE)   /* cached DRAM */
#define DRAM_BASE_NOCACHE   (0xA0000000 | PHYS_DRAM_BASE)   /* uncached DRAM */
#define FLASH_BASE          (0xA0000000 | PHYS_FLASH_BASE)  /* uncached Flash  */

/*****************************************************************************/
/*  Select the PLL value to get the desired CPU clock frequency.             */
/*                                                                           */
/*                                                                           */
/*****************************************************************************/
#define FPERIPH            50000000
    
#define SDRAM_TYPE_ADDRESS_OFFSET   16
#define NVRAM_DATA_OFFSET           0x0580
#define NVRAM_DATA_ID               0x0f1e2d3c
#define BOARD_SDRAM_TYPE            *(unsigned long *) \
                                    (FLASH_BASE + SDRAM_TYPE_ADDRESS_OFFSET)

#define ONEK                1024
#define BLK64K              (64*ONEK)

// nvram and psi flash definitions for 45
#define FLASH45_LENGTH_NVRAM            ONEK            // 1k nvram 
#define NVRAM_PSI_DEFAULT               24              // default psi in K byes

/*****************************************************************************/
/*       NVRAM Offset and definition                                         */
/*****************************************************************************/

#define NVRAM_VERSION_NUMBER            2
#define NVRAM_VERSION_NUMBER_ADDRESS    0

#define NVRAM_BOOTLINE_LEN              256
#define NVRAM_BOARD_ID_STRING_LEN       16
#define NVRAM_MAC_ADDRESS_LEN           6
#define NVRAM_MAC_COUNT_MAX             32

/*****************************************************************************/
/*       Misc Offsets                                                        */
/*****************************************************************************/

#define CFE_VERSION_OFFSET           0x0570
#define CFE_VERSION_MARK_SIZE        5
#define CFE_VERSION_SIZE             5

typedef struct
{
    unsigned long ulVersion;
    char szBootline[NVRAM_BOOTLINE_LEN];
    char szBoardId[NVRAM_BOARD_ID_STRING_LEN];
    unsigned long ulReserved1[2];
    unsigned long ulNumMacAddrs;
    unsigned char ucaBaseMacAddr[NVRAM_MAC_ADDRESS_LEN];
    char chReserved[2];
    unsigned long ulCheckSum;
} NVRAM_DATA, *PNVRAM_DATA;


/*****************************************************************************/
/*          board ioctl calls for flash, led and some other utilities        */
/*****************************************************************************/


/* Defines. for board driver */
#define BOARD_IOCTL_MAGIC       'B'
#define BOARD_DRV_MAJOR          206

#define MAC_ADDRESS_ANY         (unsigned long) -1

#define BOARD_IOCTL_FLASH_INIT \
    _IOWR(BOARD_IOCTL_MAGIC, 0, BOARD_IOCTL_PARMS)

#define BOARD_IOCTL_FLASH_WRITE \
    _IOWR(BOARD_IOCTL_MAGIC, 1, BOARD_IOCTL_PARMS)

#define BOARD_IOCTL_FLASH_READ \
    _IOWR(BOARD_IOCTL_MAGIC, 2, BOARD_IOCTL_PARMS)

#define BOARD_IOCTL_GET_NR_PAGES \
    _IOWR(BOARD_IOCTL_MAGIC, 3, BOARD_IOCTL_PARMS)

#define BOARD_IOCTL_DUMP_ADDR \
    _IOWR(BOARD_IOCTL_MAGIC, 4, BOARD_IOCTL_PARMS)

#define BOARD_IOCTL_SET_MEMORY \
    _IOWR(BOARD_IOCTL_MAGIC, 5, BOARD_IOCTL_PARMS)

#define BOARD_IOCTL_MIPS_SOFT_RESET \
    _IOWR(BOARD_IOCTL_MAGIC, 6, BOARD_IOCTL_PARMS)

#define BOARD_IOCTL_LED_CTRL \
    _IOWR(BOARD_IOCTL_MAGIC, 7, BOARD_IOCTL_PARMS)

#define BOARD_IOCTL_GET_ID \
    _IOWR(BOARD_IOCTL_MAGIC, 8, BOARD_IOCTL_PARMS)

#define BOARD_IOCTL_GET_MAC_ADDRESS \
    _IOWR(BOARD_IOCTL_MAGIC, 9, BOARD_IOCTL_PARMS)

#define BOARD_IOCTL_RELEASE_MAC_ADDRESS \
    _IOWR(BOARD_IOCTL_MAGIC, 10, BOARD_IOCTL_PARMS)

#define BOARD_IOCTL_GET_PSI_SIZE \
    _IOWR(BOARD_IOCTL_MAGIC, 11, BOARD_IOCTL_PARMS)

#define BOARD_IOCTL_GET_SDRAM_SIZE \
    _IOWR(BOARD_IOCTL_MAGIC, 12, BOARD_IOCTL_PARMS)

#define BOARD_IOCTL_SET_MONITOR_FD \
    _IOWR(BOARD_IOCTL_MAGIC, 13, BOARD_IOCTL_PARMS)
    
#define BOARD_IOCTL_WAKEUP_MONITOR_TASK \
    _IOWR(BOARD_IOCTL_MAGIC, 14, BOARD_IOCTL_PARMS)

#define BOARD_IOCTL_GET_BOOTLINE \
    _IOWR(BOARD_IOCTL_MAGIC, 15, BOARD_IOCTL_PARMS)

#define BOARD_IOCTL_SET_BOOTLINE \
    _IOWR(BOARD_IOCTL_MAGIC, 16, BOARD_IOCTL_PARMS)

#define BOARD_IOCTL_GET_BASE_MAC_ADDRESS \
    _IOWR(BOARD_IOCTL_MAGIC, 17, BOARD_IOCTL_PARMS)

#define BOARD_IOCTL_GET_CHIP_ID \
    _IOWR(BOARD_IOCTL_MAGIC, 18, BOARD_IOCTL_PARMS)

#define BOARD_IOCTL_GET_NUM_ENET \
    _IOWR(BOARD_IOCTL_MAGIC, 19, BOARD_IOCTL_PARMS)

#define BOARD_IOCTL_GET_CFE_VER \
    _IOWR(BOARD_IOCTL_MAGIC, 20, BOARD_IOCTL_PARMS)

#define BOARD_IOCTL_GET_ENET_CFG \
    _IOWR(BOARD_IOCTL_MAGIC, 21, BOARD_IOCTL_PARMS)

#define BOARD_IOCTL_GET_WLAN_ANT_INUSE \
    _IOWR(BOARD_IOCTL_MAGIC, 22, BOARD_IOCTL_PARMS)
    
#define BOARD_IOCTL_SET_TRIGGER_EVENT \
    _IOWR(BOARD_IOCTL_MAGIC, 23, BOARD_IOCTL_PARMS)        

#define BOARD_IOCTL_GET_TRIGGER_EVENT \
    _IOWR(BOARD_IOCTL_MAGIC, 24, BOARD_IOCTL_PARMS)        

#define BOARD_IOCTL_UNSET_TRIGGER_EVENT \
    _IOWR(BOARD_IOCTL_MAGIC, 25, BOARD_IOCTL_PARMS) 

#define BOARD_IOCTL_SET_SES_LED \
    _IOWR(BOARD_IOCTL_MAGIC, 26, BOARD_IOCTL_PARMS)

//<<JUNHON, 2004/09/15, get reset button status , tim hou , 05/04/12
#define RESET_BUTTON_UP           1
#define RESET_BUTTON_PRESSDOWN    0
#define BOARD_IOCTL_GET_RESETHOLD \
    _IOWR(BOARD_IOCTL_MAGIC, 27, BOARD_IOCTL_PARMS)
//>>JUNHON, 2004/09/15    
    
// for the action in BOARD_IOCTL_PARMS for flash operation
typedef enum 
{
    PERSISTENT,
    NVRAM,
    BCM_IMAGE_CFE,
    BCM_IMAGE_FS,
    BCM_IMAGE_KERNEL,
    BCM_IMAGE_WHOLE,
    SCRATCH_PAD,
    FLASH_SIZE,
} BOARD_IOCTL_ACTION;
    
    
typedef struct boardIoctParms
{
    char *string;
    char *buf;
    int strLen;
    int offset;
    BOARD_IOCTL_ACTION  action;        /* flash read/write: nvram, persistent, bcm image */
    int result;
} BOARD_IOCTL_PARMS;


// LED defines 
typedef enum
{   
    kLedAdsl,
    kLedWireless,
    kLedUsb,
    kLedHpna,
    kLedWanData,
    kLedPPP,
    kLedVoip,
    kLedSes,
    kLedLan,
    kLedSelfTest,
    kLedEnd,                // NOTE: Insert the new led name before this one.  Alway stay at the end.
} BOARD_LED_NAME;

typedef enum
{
    kLedStateOff,                        /* turn led off */
    kLedStateOn,                         /* turn led on */
    kLedStateFail,                       /* turn led on red */
    kLedStateBlinkOnce,                  /* blink once, ~100ms and ignore the same call during the 100ms period */
    kLedStateSlowBlinkContinues,         /* slow blink continues at ~600ms interval */
    kLedStateFastBlinkContinues,         /* fast blink continues at ~200ms interval */
} BOARD_LED_STATE;


// virtual and physical map pair defined in board.c
typedef struct ledmappair
{
    BOARD_LED_NAME ledName;         // virtual led name
    BOARD_LED_STATE ledInitState;   // initial led state when the board boots.
    unsigned short ledMask;         // physical GPIO pin mask
    unsigned short ledActiveLow;    // reset bit to turn on LED
    unsigned short ledMaskFail;     // physical GPIO pin mask for state failure
    unsigned short ledActiveLowFail;// reset bit to turn on LED
} LED_MAP_PAIR, *PLED_MAP_PAIR;

typedef void (*HANDLE_LED_FUNC)(BOARD_LED_NAME ledName, BOARD_LED_STATE ledState);

/* Flash storage address information that is determined by the flash driver. */
typedef struct flashaddrinfo
{
    int flash_persistent_start_blk;
    int flash_persistent_number_blk;
    int flash_persistent_length;
    unsigned long flash_persistent_blk_offset;
    int flash_scratch_pad_start_blk;         // start before psi (SP_BUF_LEN)
    int flash_scratch_pad_number_blk;
    int flash_scratch_pad_length;
    unsigned long flash_scratch_pad_blk_offset;
    int flash_nvram_start_blk;
    int flash_nvram_number_blk;
    int flash_nvram_length;
    unsigned long flash_nvram_blk_offset;
} FLASH_ADDR_INFO, *PFLASH_ADDR_INFO;

// scratch pad defines
/* SP - Persisten Scratch Pad format:
       sp header        : 32 bytes
       tokenId-1        : 8 bytes
       tokenId-1 len    : 4 bytes
       tokenId-1 data    
       ....
       tokenId-n        : 8 bytes
       tokenId-n len    : 4 bytes
       tokenId-n data    
*/

#define MAGIC_NUM_LEN       8
#define MAGIC_NUMBER        "gOGoBrCm"
#define TOKEN_NAME_LEN      16
#define SP_VERSION          1
#define SP_MAX_LEN          8 * 1024            // 8k buf before psi
#define SP_RESERVERD        16

typedef struct _SP_HEADER
{
    char SPMagicNum[MAGIC_NUM_LEN];             // 8 bytes of magic number
    int SPVersion;                              // version number
    int SPUsedLen;                              // used sp len   
    char SPReserved[SP_RESERVERD];              // reservied, total 32 bytes
} SP_HEADER, *PSP_HEADER;

typedef struct _TOKEN_DEF
{
    char tokenName[TOKEN_NAME_LEN];
    int tokenLen;
} SP_TOKEN, *PSP_TOKEN;


/*****************************************************************************/
/*          Function Prototypes                                              */
/*****************************************************************************/
#if !defined(__ASM_ASM_H)
void dumpaddr( unsigned char *pAddr, int nLen );

int kerSysNvRamGet(char *string, int strLen, int offset);
int kerSysNvRamSet(char *string, int strLen, int offset);
int kerSysPersistentGet(char *string, int strLen, int offset);
int kerSysPersistentSet(char *string, int strLen, int offset);
int kerSysScratchPadGet(char *tokName, char *tokBuf, int tokLen);
int kerSysScratchPadSet(char *tokName, char *tokBuf, int tokLen);
int kerSysBcmImageSet( int flash_start_addr, char *string, int size);
int kerSysGetMacAddress( unsigned char *pucaAddr, unsigned long ulId );
int kerSysReleaseMacAddress( unsigned char *pucaAddr );
int kerSysGetSdramSize( void );
void kerSysGetBootline(char *string, int strLen);
void kerSysSetBootline(char *string, int strLen);
void kerSysMipsSoftReset(void);
void kerSysLedCtrl(BOARD_LED_NAME, BOARD_LED_STATE);
void kerSysLedRegisterHwHandler( BOARD_LED_NAME, HANDLE_LED_FUNC, int );
int kerSysFlashSizeGet(void);
void kerSysRegisterDyingGaspHandler(char *devname, void *cbfn, void *context);
void kerSysDeregisterDyingGaspHandler(char *devname);    
void kerSysWakeupMonitorTask( void );
#endif

#define BOOT_CFE     0
#define BOOT_REDBOOT 1

extern int boot_loader_type;

#endif /* _BOARD_H */


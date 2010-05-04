/*******************************************************************************
Copyright (C) Marvell International Ltd. and its affiliates

This software file (the "File") is owned and distributed by Marvell
International Ltd. and/or its affiliates ("Marvell") under the following
alternative licensing terms.  Once you have made an election to distribute the
File under one of the following license alternatives, please (i) delete this
introductory statement regarding license alternatives, (ii) delete the two
license alternatives that you have not elected to use and (iii) preserve the
Marvell copyright notice above.

********************************************************************************
Marvell Commercial License Option

If you received this File from Marvell and you have entered into a commercial
license agreement (a "Commercial License") with Marvell, the File is licensed
to you under the terms of the applicable Commercial License.

********************************************************************************
Marvell GPL License Option

If you received this File from Marvell, you may opt to use, redistribute and/or
modify this File in accordance with the terms and conditions of the General
Public License Version 2, June 1991 (the "GPL License"), a copy of which is
available along with the File in the license.txt file or by writing to the Free
Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 or
on the worldwide web at http://www.gnu.org/licenses/gpl.txt.

THE FILE IS DISTRIBUTED AS-IS, WITHOUT WARRANTY OF ANY KIND, AND THE IMPLIED
WARRANTIES OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE ARE EXPRESSLY
DISCLAIMED.  The GPL License provides additional details about this warranty
disclaimer.
********************************************************************************
Marvell BSD License Option

If you received this File from Marvell, you may opt to use, redistribute and/or
modify this File under the following licensing terms.
Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

    *   Redistributions of source code must retain the above copyright notice,
	    this list of conditions and the following disclaimer.

    *   Redistributions in binary form must reproduce the above copyright
        notice, this list of conditions and the following disclaimer in the
        documentation and/or other materials provided with the distribution.

    *   Neither the name of Marvell nor the names of its contributors may be
        used to endorse or promote products derived from this software without
        specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*******************************************************************************/

#ifndef __INCmvSFlashH
#define __INCmvSFlashH

#include "mvTypes.h"

/* MCAROS */
#define MV_SFLASH_PAGE_ALLIGN_MASK(pgSz)    (pgSz-1)
#define MV_ARRAY_SIZE(a)                    ((sizeof(a)) / (sizeof(a[0])))

/* Constants */
#define MV_INVALID_DEVICE_NUMBER            0xFFFFFFFF
/* 10 MHz is the minimum possible SPI frequency when tclk is set 200MHz*/
#define MV_SFLASH_BASIC_SPI_FREQ            10000000
/* enumerations */
typedef enum
{
	MV_WP_NONE,             /* Unprotect the whole chip */
    MV_WP_UPR_1OF128,       /* Write protect the upper 1/128 part */
    MV_WP_UPR_1OF64,        /* Write protect the upper 1/64 part */
	MV_WP_UPR_1OF32,        /* Write protect the upper 1/32 part */
	MV_WP_UPR_1OF16,        /* Write protect the upper 1/16 part */
	MV_WP_UPR_1OF8,         /* Write protect the upper 1/8 part */
	MV_WP_UPR_1OF4,         /* Write protect the upper 1/4 part */
	MV_WP_UPR_1OF2,         /* Write protect the upper 1/2 part */
	MV_WP_ALL               /* Write protect the whole chip */
} MV_SFLASH_WP_REGION;

/* Type Definitions */
typedef struct
{
    MV_U8   opcdWREN;       /* Write enable opcode */
    MV_U8   opcdWRDI;       /* Write disable opcode */
    MV_U8   opcdRDID;       /* Read ID opcode */
    MV_U8   opcdRDSR;       /* Read Status Register opcode */
    MV_U8   opcdWRSR;       /* Write Status register opcode */
    MV_U8   opcdREAD;       /* Read opcode */
    MV_U8   opcdFSTRD;      /* Fast Read opcode */
    MV_U8   opcdPP;         /* Page program opcode */
    MV_U8   opcdSE;         /* Sector erase opcode */
    MV_U8   opcdBE;         /* Bulk erase opcode */
    MV_U8   opcdRES;        /* Read electronic signature */
    MV_U8   opcdPwrSave;    /* Go into power save mode */
    MV_U32  sectorSize;     /* Size of each sector */
    MV_U32  sectorNumber;   /* Number of sectors */
    MV_U32  pageSize;       /* size of each page */
    const char * deviceModel;    /* string with the device model */
    MV_U32  manufacturerId; /* The manufacturer ID */
    MV_U32  deviceId;       /* Device ID */
    MV_U32  spiMaxFreq;     /* The MAX frequency that can be used with the device */
    MV_U32  spiMaxFastFreq; /* The MAX frequency that can be used with the device for fast reads */
    MV_U32  spiFastRdDummyBytes; /* Number of dumy bytes to read before real data when working in fast read mode. */
} MV_SFLASH_DEVICE_PARAMS;

typedef struct
{
    MV_U32					baseAddr;       /* Flash Base Address used in fast mode */
	MV_U8	                manufacturerId;	/* Manufacturer ID */
    MV_U16	                deviceId;	    /* Device ID */
    MV_U32                  sectorSize;     /* Size of each sector - all the same */
    MV_U32                  sectorNumber;   /* Number of sectors */
    MV_U32                  pageSize;       /* Page size - affect allignment */
    MV_U32                  index;          /* index of the device in the sflash table (internal parameter) */
} MV_SFLASH_INFO;

/* Function Prototypes */
/* Init */
MV_STATUS	mvSFlashInit		(MV_SFLASH_INFO * pFlinfo);

/* erase */
MV_STATUS 	mvSFlashSectorErase (MV_SFLASH_INFO * pFlinfo, MV_U32 secNumber);
MV_STATUS 	mvSFlashChipErase   (MV_SFLASH_INFO * pFlinfo);

/* Read */
MV_STATUS	mvSFlashBlockRd  	(MV_SFLASH_INFO * pFlinfo, MV_U32 offset,
							     MV_U8* pReadBuff, MV_U32 buffSize);
MV_STATUS mvSFlashFastBlockRd (MV_SFLASH_INFO * pFlinfo, MV_U32 offset,
							     MV_U8* pReadBuff, MV_U32 buffSize);

/* write regardless of the page boundaries and size limit per Page program command */
MV_STATUS	mvSFlashBlockWr		(MV_SFLASH_INFO * pFlinfo, MV_U32 offset,
							     MV_U8* pWriteBuff, MV_U32 buffSize);
/* Get IDs */
MV_STATUS 	mvSFlashIdGet      	(MV_SFLASH_INFO * pFlinfo, MV_U8* pManId, MV_U16* pDevId);

/* Set and Get the Write Protection region - if the Status register is not locked */
MV_STATUS   mvSFlashWpRegionSet (MV_SFLASH_INFO * pFlinfo, MV_SFLASH_WP_REGION wpRegion);
MV_STATUS   mvSFlashWpRegionGet (MV_SFLASH_INFO * pFlinfo, MV_SFLASH_WP_REGION * pWpRegion);

/* Lock the status register for writing - W/Vpp pin should be low to take effect */
MV_STATUS   mvSFlashStatRegLock (MV_SFLASH_INFO * pFlinfo, MV_BOOL srLock);

/* Get the regions sizes */
MV_U32      mvSFlashSizeGet     (MV_SFLASH_INFO * pFlinfo);

/* Cause the falsh device to go into power save mode */
MV_STATUS   mvSFlashPowerSaveEnter(MV_SFLASH_INFO * pFlinfo);
MV_STATUS   mvSFlashPowerSaveExit (MV_SFLASH_INFO * pFlinfo);

/* Retreive the string with the device manufacturer and model */
const MV_8 *     mvSFlashModelGet    (MV_SFLASH_INFO * pFlinfo);

#endif /* __INCmvSFlashH */

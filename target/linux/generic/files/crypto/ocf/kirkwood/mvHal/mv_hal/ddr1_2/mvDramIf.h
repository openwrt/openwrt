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


#ifndef __INCmvDramIfh
#define __INCmvDramIfh

/* includes */
#include "ddr1_2/mvDramIfRegs.h"
#include "ddr1_2/mvDramIfConfig.h"
#include "ctrlEnv/mvCtrlEnvLib.h"

/* defines  */
/* DRAM Timing parameters */
#define SDRAM_TWR                    15  /* ns tWr */
#define SDRAM_TRFC_64_512M_AT_200MHZ 70  /* ns tRfc for dens 64-512 @ 200MHz */
#define SDRAM_TRFC_64_512M           75  /* ns tRfc for dens 64-512          */
#define SDRAM_TRFC_1G                120 /* ns tRfc for dens 1GB             */
#define SDRAM_TR2R_CYC               1   /* cycle for tR2r                   */
#define SDRAM_TR2WW2R_CYC            1   /* cycle for tR2wW2r                */

/* typedefs */

/* enumeration for memory types */
typedef enum _mvMemoryType
{
    MEM_TYPE_SDRAM,
    MEM_TYPE_DDR1,
    MEM_TYPE_DDR2
}MV_MEMORY_TYPE;

/* enumeration for DDR1 supported CAS Latencies */
typedef enum _mvDimmDdr1Cas
{
    DDR1_CL_1_5  = 0x02, 
    DDR1_CL_2    = 0x04, 
    DDR1_CL_2_5  = 0x08, 
    DDR1_CL_3    = 0x10, 
    DDR1_CL_4    = 0x40, 
    DDR1_CL_FAULT
} MV_DIMM_DDR1_CAS;

/* enumeration for DDR2 supported CAS Latencies */
typedef enum _mvDimmDdr2Cas
{
    DDR2_CL_3    = 0x08, 
    DDR2_CL_4    = 0x10, 
    DDR2_CL_5    = 0x20, 
    DDR2_CL_FAULT
} MV_DIMM_DDR2_CAS;


typedef struct _mvDramBankInfo
{
    MV_MEMORY_TYPE  memoryType; 	/* DDR1, DDR2 or SDRAM */

    /* DIMM dimensions */
    MV_U32  numOfRowAddr;
    MV_U32  numOfColAddr;
    MV_U32  dataWidth;
    MV_U32  errorCheckType;             /* ECC , PARITY..*/
    MV_U32  sdramWidth;                 /* 4,8,16 or 32 */
    MV_U32  errorCheckDataWidth;        /* 0 - no, 1 - Yes */
    MV_U32  burstLengthSupported;
    MV_U32  numOfBanksOnEachDevice;
    MV_U32  suportedCasLatencies;
    MV_U32  refreshInterval;

    /* DIMM timing parameters */
    MV_U32  minCycleTimeAtMaxCasLatPs;	
    MV_U32  minCycleTimeAtMaxCasLatMinus1Ps;
    MV_U32  minCycleTimeAtMaxCasLatMinus2Ps;
	MV_U32  minRowPrechargeTime;
	MV_U32  minRowActiveToRowActive;
	MV_U32  minRasToCasDelay;
	MV_U32  minRasPulseWidth;
    MV_U32  minWriteRecoveryTime;   /* DDR2 only */
    MV_U32  minWriteToReadCmdDelay; /* DDR2 only */
    MV_U32  minReadToPrechCmdDelay; /* DDR2 only */
    MV_U32  minRefreshToActiveCmd;  /* DDR2 only */
                      
    /* Parameters calculated from the extracted DIMM information */
    MV_U32  size;
    MV_U32  deviceDensity;           	/* 16,64,128,256 or 512 Mbit */
    MV_U32  numberOfDevices;

    /* DIMM attributes (MV_TRUE for yes) */
    MV_BOOL registeredAddrAndControlInputs;
     
}MV_DRAM_BANK_INFO;

/* This structure describes CPU interface address decode window               */
typedef struct _mvDramIfDecWin 
{
	MV_ADDR_WIN   addrWin;    /* An address window*/
	MV_BOOL       enable;     /* Address decode window is enabled/disabled    */
}MV_DRAM_DEC_WIN;

#include "ddr1_2/mvDram.h"

/* mvDramIf.h API list */
MV_VOID   mvDramIfBasicAsmInit(MV_VOID);
MV_STATUS mvDramIfDetect(MV_U32 forcedCl);
MV_VOID   _mvDramIfConfig(MV_VOID);

MV_STATUS mvDramIfWinSet(MV_TARGET target, MV_DRAM_DEC_WIN *pAddrDecWin);
MV_STATUS mvDramIfWinGet(MV_TARGET target, MV_DRAM_DEC_WIN *pAddrDecWin);
MV_STATUS mvDramIfWinEnable(MV_TARGET target,MV_BOOL enable);
MV_32 mvDramIfBankSizeGet(MV_U32 bankNum);
MV_32 mvDramIfBankBaseGet(MV_U32 bankNum);
MV_32 mvDramIfSizeGet(MV_VOID);

#if 0
MV_STATUS mvDramIfMbusCtrlSet(MV_XBAR_TARGET *pPizzaArbArray);
MV_STATUS mvDramIfMbusToutSet(MV_U32 timeout, MV_BOOL enable);
#endif

#endif /* __INCmvDramIfh */

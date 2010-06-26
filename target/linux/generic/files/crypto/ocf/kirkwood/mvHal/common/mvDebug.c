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



/* includes */
#include "mvOs.h"
#include "mv802_3.h"
#include "mvCommon.h"
#include "mvDebug.h"

/* Global variables effect on behave MV_DEBUG_PRINT and MV_DEBUG_CODE macros
 * mvDebug  - map of bits (one for each module) bit=1 means enable 
 *          debug code and messages for this module
 * mvModuleDebug - array of 32 bits varables one for each module
 */
MV_U32    mvDebug = 0;
MV_U32    mvDebugModules[MV_MODULE_MAX];
 
/* Init mvModuleDebug array to default values */
void    mvDebugInit(void)
{
    int     bit;

    mvDebug = 0;
    for(bit=0; bit<MV_MODULE_MAX; bit++)
    {
        mvDebugModules[bit] = MV_DEBUG_FLAG_ERR | MV_DEBUG_FLAG_STATS;
        mvDebug |= MV_BIT_MASK(bit);
    }
}
 
void    mvDebugModuleEnable(MV_MODULE_ID module, MV_BOOL isEnable)
{    
    if (isEnable)
    {
       MV_BIT_SET(mvDebug, module);               
    }
    else
       MV_BIT_CLEAR(mvDebug, module);                   
}
 
void    mvDebugModuleSetFlags(MV_MODULE_ID module, MV_U32 flags)
{ 
    mvDebugModules[module] |= flags;
}

void    mvDebugModuleClearFlags(MV_MODULE_ID module, MV_U32 flags)
{ 
    mvDebugModules[module] &= ~flags;
}

/* Dump memory in specific format: 
 * address: X1X1X1X1 X2X2X2X2 ... X8X8X8X8 
 */
void mvDebugMemDump(void* addr, int size, int access)
{
    int     i, j;
    MV_U32  memAddr = (MV_U32)addr;

    if(access == 0)
        access = 1;

    if( (access != 4) && (access != 2) && (access != 1) )
    {
        mvOsPrintf("%d wrong access size. Access must be 1 or 2 or 4\n",
                    access);
        return;
    }
    memAddr = MV_ALIGN_DOWN( (unsigned int)addr, 4);
    size = MV_ALIGN_UP(size, 4);
    addr = (void*)MV_ALIGN_DOWN( (unsigned int)addr, access);
    while(size > 0)
    {
        mvOsPrintf("%08x: ", memAddr);
        i = 0;
        /* 32 bytes in the line */
        while(i < 32)
        {
            if(memAddr >= (MV_U32)addr)
            {
                switch(access)
                {
                    case 1:
                        if( memAddr == CPU_PHY_MEM(memAddr) )
                        {
                            mvOsPrintf("%02x ", MV_MEMIO8_READ(memAddr));
                        }
                        else
                        {
                            mvOsPrintf("%02x ", *((MV_U8*)memAddr));
                        }
                        break;

                    case 2:
                        if( memAddr == CPU_PHY_MEM(memAddr) )
                        {
                            mvOsPrintf("%04x ", MV_MEMIO16_READ(memAddr));
                        }
                        else
                        {
                            mvOsPrintf("%04x ", *((MV_U16*)memAddr));
                        }
                        break;

                    case 4:
                        if( memAddr == CPU_PHY_MEM(memAddr) )
                        {
                            mvOsPrintf("%08x ", MV_MEMIO32_READ(memAddr));
                        }
                        else
                        {
                            mvOsPrintf("%08x ", *((MV_U32*)memAddr));
                        }
                        break;
                }
            }
            else
            {
                for(j=0; j<(access*2+1); j++)
                    mvOsPrintf(" ");
            }
            i += access;
            memAddr += access;
            size -= access;
            if(size <= 0)
                break;
        }
        mvOsPrintf("\n");
    }
}

void mvDebugPrintBufInfo(BUF_INFO* pBufInfo, int size, int access)
{
	if(pBufInfo == NULL)
	{
		mvOsPrintf("\n!!! pBufInfo = NULL\n");
		return;
	}
    mvOsPrintf("\n*** pBufInfo=0x%x, cmdSts=0x%08x, pBuf=0x%x, bufSize=%d\n",
               (unsigned int)pBufInfo, 
			   (unsigned int)pBufInfo->cmdSts, 
			   (unsigned int)pBufInfo->pBuff, 
			   (unsigned int)pBufInfo->bufSize);
    mvOsPrintf("pData=0x%x, byteCnt=%d, pNext=0x%x, uInfo1=0x%x, uInfo2=0x%x\n",
               (unsigned int)pBufInfo->pData, 
			   (unsigned int)pBufInfo->byteCnt, 
			   (unsigned int)pBufInfo->pNextBufInfo,
               (unsigned int)pBufInfo->userInfo1, 
			   (unsigned int)pBufInfo->userInfo2);
    if(pBufInfo->pData != NULL)
    {
        if(size > pBufInfo->byteCnt)
            size = pBufInfo->byteCnt;
        mvDebugMemDump(pBufInfo->pData, size, access);
    }
}

void mvDebugPrintPktInfo(MV_PKT_INFO* pPktInfo, int size, int access)
{
    int frag, len;

	if(pPktInfo == NULL)
	{
		mvOsPrintf("\n!!! pPktInfo = NULL\n");
		return;
	}
    mvOsPrintf("\npPkt=%p, stat=0x%08x, numFr=%d, size=%d, pFr=%p, osInfo=0x%lx\n",
                pPktInfo, pPktInfo->status, pPktInfo->numFrags, pPktInfo->pktSize, 
                pPktInfo->pFrags, pPktInfo->osInfo);
    
    for(frag=0; frag<pPktInfo->numFrags; frag++)
    {
        mvOsPrintf("#%2d. bufVirt=%p, bufSize=%d\n", 
                    frag, pPktInfo->pFrags[frag].bufVirtPtr, 
                    pPktInfo->pFrags[frag].bufSize);
        if(size > 0)
        {
            len = MV_MIN((int)pPktInfo->pFrags[frag].bufSize, size);
            mvDebugMemDump(pPktInfo->pFrags[frag].bufVirtPtr, len, access);
            size -= len;
        }
    }
    
}

void    mvDebugPrintIpAddr(MV_U32 ipAddr)
{
    mvOsPrintf("%d.%d.%d.%d", ((ipAddr >> 24) & 0xFF), ((ipAddr >> 16) & 0xFF),
                              ((ipAddr >> 8) & 0xFF), ((ipAddr >> 0) & 0xFF));
}

void    mvDebugPrintMacAddr(const MV_U8* pMacAddr)
{
    int     i;

    mvOsPrintf("%02x", (unsigned int)pMacAddr[0]);
    for(i=1; i<MV_MAC_ADDR_SIZE; i++)
    {
        mvOsPrintf(":%02x", pMacAddr[i]);
    }
    /* mvOsPrintf("\n");*/
}


/******* There are three functions deals with MV_DEBUG_TIMES structure ********/

/* Reset MV_DEBUG_TIMES entry */
void mvDebugResetTimeEntry(MV_DEBUG_TIMES* pTimeEntry, int count, char* pName)
{
    pTimeEntry->begin = 0;
    pTimeEntry->count = count;
    pTimeEntry->end = 0;
    pTimeEntry->left = pTimeEntry->count;
    pTimeEntry->total = 0;
    pTimeEntry->min = 0xFFFFFFFF;
    pTimeEntry->max = 0x0;
    strncpy(pTimeEntry->name, pName, sizeof(pTimeEntry->name)-1);
    pTimeEntry->name[sizeof(pTimeEntry->name)-1] = '\0';
}

/* Print out MV_DEBUG_TIMES entry */
void mvDebugPrintTimeEntry(MV_DEBUG_TIMES* pTimeEntry, MV_BOOL isTitle)
{
    int     num;

    if(isTitle == MV_TRUE)
        mvOsPrintf("Event         NumOfEvents       TotalTime         Average       Min       Max\n");

    num = pTimeEntry->count-pTimeEntry->left;
    if(num > 0)
    {
        mvOsPrintf("%-11s     %6u          0x%08lx        %6lu     %6lu    %6lu\n",
                pTimeEntry->name, num, pTimeEntry->total, pTimeEntry->total/num,
                pTimeEntry->min, pTimeEntry->max);
    }   
}

/* Update MV_DEBUG_TIMES entry */
void mvDebugUpdateTimeEntry(MV_DEBUG_TIMES* pTimeEntry)
{
    MV_U32  delta;

    if(pTimeEntry->left > 0)                                        
    {                                                                           
        if(pTimeEntry->end <= pTimeEntry->begin)       
        {                                                                       
            delta = pTimeEntry->begin - pTimeEntry->end;
        }
        else
        {
            delta = ((MV_U32)0x10000 - pTimeEntry->end) + pTimeEntry->begin;
        }
        pTimeEntry->total += delta;             

        if(delta < pTimeEntry->min)    
            pTimeEntry->min = delta;    

        if(delta > pTimeEntry->max)    
            pTimeEntry->max = delta;      

        pTimeEntry->left--;                                                                                                                 
    }
}


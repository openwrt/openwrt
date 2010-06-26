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

#include "cesa/mvCesa.h"

#include "ctrlEnv/mvCtrlEnvLib.h"
#undef CESA_DEBUG


/********** Global variables **********/

/*  If request size is more than MV_CESA_MAX_BUF_SIZE the
 *  request is processed as fragmented request.
 */

MV_CESA_STATS           cesaStats;

MV_BUF_INFO             cesaSramSaBuf;
short                   cesaLastSid = -1;
MV_CESA_SA*             pCesaSAD = NULL;
MV_U16                  cesaMaxSA = 0;

MV_CESA_REQ*            pCesaReqFirst = NULL;
MV_CESA_REQ*            pCesaReqLast = NULL;
MV_CESA_REQ*            pCesaReqEmpty = NULL;
MV_CESA_REQ*            pCesaReqProcess = NULL;
int                     cesaQueueDepth = 0;
int                     cesaReqResources = 0;

MV_CESA_SRAM_MAP*       cesaSramVirtPtr = NULL;
MV_U32                  cesaCryptEngBase = 0;
void			*cesaOsHandle = NULL;
#if (MV_CESA_VERSION >= 3)
MV_U32			cesaChainLength = 0;
int                     chainReqNum = 0;
MV_U32			chainIndex = 0;
MV_CESA_REQ*  		pNextActiveChain = 0;
MV_CESA_REQ*		pEndCurrChain = 0;
MV_BOOL			isFirstReq = MV_TRUE;
#endif

static INLINE MV_U8*  mvCesaSramAddrGet(void)
{
#ifdef MV_CESA_NO_SRAM
    return (MV_U8*)cesaSramVirtPtr;
#else
    return (MV_U8*)cesaCryptEngBase;
#endif /* MV_CESA_NO_SRAM */
}

static INLINE MV_ULONG    mvCesaSramVirtToPhys(void* pDev, MV_U8* pSramVirt)
{
#ifdef MV_CESA_NO_SRAM
    return (MV_ULONG)mvOsIoVirtToPhy(NULL, pSramVirt);
#else
    return (MV_ULONG)pSramVirt;
#endif /* MV_CESA_NO_SRAM */
}

/* Internal Function prototypes */

static INLINE void      mvCesaSramDescrBuild(MV_U32 config, int frag,
                                 int cryptoOffset, int ivOffset, int cryptoLength,
                                 int macOffset, int digestOffset, int macLength, int macTotalLen,
                                 MV_CESA_REQ *pCesaReq, MV_DMA_DESC* pDmaDesc);

static INLINE void      mvCesaSramSaUpdate(short sid, MV_DMA_DESC *pDmaDesc);

static INLINE int       mvCesaDmaCopyPrepare(MV_CESA_MBUF* pMbuf, MV_U8* pSramBuf, 
                                MV_DMA_DESC* pDmaDesc, MV_BOOL isToMbuf,
                                int offset, int copySize, MV_BOOL skipFlush);

static void        mvCesaHmacIvGet(MV_CESA_MAC_MODE macMode, unsigned char key[], int keyLength,
                                    unsigned char innerIV[], unsigned char outerIV[]);

static MV_STATUS   mvCesaFragAuthComplete(MV_CESA_REQ* pReq, MV_CESA_SA* pSA,
                                          int macDataSize);

static MV_CESA_COMMAND*   mvCesaCtrModeInit(void);

static MV_STATUS   mvCesaCtrModePrepare(MV_CESA_COMMAND *pCtrModeCmd, MV_CESA_COMMAND *pCmd);
static MV_STATUS   mvCesaCtrModeComplete(MV_CESA_COMMAND *pOrgCmd, MV_CESA_COMMAND *pCmd);
static void        mvCesaCtrModeFinish(MV_CESA_COMMAND *pCmd);

static INLINE MV_STATUS mvCesaReqProcess(MV_CESA_REQ* pReq);
static MV_STATUS   mvCesaFragReqProcess(MV_CESA_REQ* pReq, MV_U8 frag);

static INLINE MV_STATUS mvCesaParamCheck(MV_CESA_SA* pSA, MV_CESA_COMMAND *pCmd, MV_U8* pFixOffset);
static INLINE MV_STATUS mvCesaFragParamCheck(MV_CESA_SA* pSA, MV_CESA_COMMAND *pCmd);

static INLINE void      mvCesaFragSizeFind(MV_CESA_SA* pSA, MV_CESA_REQ* pReq, 
                               int cryptoOffset, int macOffset,
                               int* pCopySize, int* pCryptoDataSize, int* pMacDataSize);
static MV_STATUS        mvCesaMbufCacheUnmap(MV_CESA_MBUF* pMbuf, int offset, int size);


/* Go to the next request in the request queue */
static INLINE MV_CESA_REQ* MV_CESA_REQ_NEXT_PTR(MV_CESA_REQ* pReq)
{
    if(pReq == pCesaReqLast)
        return pCesaReqFirst;

    return pReq+1;
}

#if (MV_CESA_VERSION >= 3)
/* Go to the previous request in the request queue */
static INLINE MV_CESA_REQ* MV_CESA_REQ_PREV_PTR(MV_CESA_REQ* pReq)
{
    if(pReq == pCesaReqFirst)
        return pCesaReqLast;

    return pReq-1;
}

#endif


static INLINE void mvCesaReqProcessStart(MV_CESA_REQ* pReq)
{
    int frag;

#if (MV_CESA_VERSION >= 3)
    pReq->state = MV_CESA_CHAIN;
#else
    pReq->state = MV_CESA_PROCESS;
#endif
    cesaStats.startCount++;

    if(pReq->fragMode == MV_CESA_FRAG_NONE)
    {
        frag = 0;
    }
    else
    {
        frag = pReq->frags.nextFrag;
        pReq->frags.nextFrag++;
    }
#if (MV_CESA_VERSION >= 2)
    /* Enable TDMA engine */
    MV_REG_WRITE(MV_CESA_TDMA_CURR_DESC_PTR_REG, 0);
    MV_REG_WRITE(MV_CESA_TDMA_NEXT_DESC_PTR_REG, 
            (MV_U32)mvCesaVirtToPhys(&pReq->dmaDescBuf, pReq->dma[frag].pDmaFirst));
#else
    /* Enable IDMA engine */
    MV_REG_WRITE(IDMA_CURR_DESC_PTR_REG(0), 0);
    MV_REG_WRITE(IDMA_NEXT_DESC_PTR_REG(0), 
            (MV_U32)mvCesaVirtToPhys(&pReq->dmaDescBuf, pReq->dma[frag].pDmaFirst));
#endif /* MV_CESA_VERSION >= 2 */

#if defined(MV_BRIDGE_SYNC_REORDER) 
    mvOsBridgeReorderWA();
#endif

    /* Start Accelerator */
    MV_REG_WRITE(MV_CESA_CMD_REG, MV_CESA_CMD_CHAN_ENABLE_MASK);
}


/*******************************************************************************
* mvCesaHalInit - Initialize the CESA driver
*
* DESCRIPTION:
*       This function initialize the CESA driver.
*       1) Session database
*       2) Request queue
*       4) DMA descriptor lists - one list per request. Each list
*           has MV_CESA_MAX_DMA_DESC descriptors.
*
* INPUT:
*       numOfSession    - maximum number of supported sessions
*       queueDepth      - number of elements in the request queue.
*       pSramBase       - virtual address of Sram
*	osHandle	- A handle used by the OS to allocate memory for the 
*			  module (Passed to the OS Services layer)
*
* RETURN:
*       MV_OK           - Success
*       MV_NO_RESOURCE  - Fail, can't allocate resources:
*                         Session database, request queue,
*                         DMA descriptors list, LRU cache database.
*       MV_NOT_ALIGNED  - Sram base address is not 8 byte aligned.
*
*******************************************************************************/
MV_STATUS mvCesaHalInit (int numOfSession, int queueDepth, char* pSramBase, MV_U32 cryptEngBase,
			 void *osHandle)
{
    int     i, req;
    MV_U32  descOffsetReg, configReg;
    MV_CESA_SRAM_SA *pSramSA;


    mvOsPrintf("mvCesaInit: sessions=%d, queue=%d, pSram=%p\n",
                numOfSession, queueDepth, pSramBase);

    cesaOsHandle = osHandle;
    /* Create Session database */
    pCesaSAD = mvOsMalloc(sizeof(MV_CESA_SA)*numOfSession);
    if(pCesaSAD == NULL)
    {
        mvOsPrintf("mvCesaInit: Can't allocate %u bytes for %d SAs\n",
                    sizeof(MV_CESA_SA)*numOfSession, numOfSession);
        mvCesaFinish();
        return MV_NO_RESOURCE;
    }
    memset(pCesaSAD, 0, sizeof(MV_CESA_SA)*numOfSession);
    cesaMaxSA = numOfSession;

    /* Allocate imag of sramSA in the DRAM */
    cesaSramSaBuf.bufSize = sizeof(MV_CESA_SRAM_SA)*numOfSession + 
                                    CPU_D_CACHE_LINE_SIZE;

    cesaSramSaBuf.bufVirtPtr = mvOsIoCachedMalloc(osHandle,cesaSramSaBuf.bufSize,
						  &cesaSramSaBuf.bufPhysAddr,
						  &cesaSramSaBuf.memHandle);
        
    if(cesaSramSaBuf.bufVirtPtr == NULL)
    {
        mvOsPrintf("mvCesaInit: Can't allocate %d bytes for sramSA structures\n", 
                    cesaSramSaBuf.bufSize);
        mvCesaFinish();
        return MV_NO_RESOURCE;
    }
    memset(cesaSramSaBuf.bufVirtPtr, 0, cesaSramSaBuf.bufSize); 
    pSramSA = (MV_CESA_SRAM_SA*)MV_ALIGN_UP((MV_ULONG)cesaSramSaBuf.bufVirtPtr, 
                                                       CPU_D_CACHE_LINE_SIZE);
    for(i=0; i<numOfSession; i++)
    {
        pCesaSAD[i].pSramSA = &pSramSA[i];
    }

    /* Create request queue */
    pCesaReqFirst = mvOsMalloc(sizeof(MV_CESA_REQ)*queueDepth);
    if(pCesaReqFirst == NULL)
    {
        mvOsPrintf("mvCesaInit: Can't allocate %u bytes for %d requests\n",
                    sizeof(MV_CESA_REQ)*queueDepth, queueDepth);
        mvCesaFinish();
        return MV_NO_RESOURCE;
    }
    memset(pCesaReqFirst, 0, sizeof(MV_CESA_REQ)*queueDepth);
    pCesaReqEmpty = pCesaReqFirst;
    pCesaReqLast = pCesaReqFirst + (queueDepth-1);
    pCesaReqProcess = pCesaReqEmpty;
    cesaQueueDepth = queueDepth;
    cesaReqResources = queueDepth;
#if (MV_CESA_VERSION >= 3)
    cesaChainLength = MAX_CESA_CHAIN_LENGTH; 
#endif
    /* pSramBase must be 8 byte aligned */
    if( MV_IS_NOT_ALIGN((MV_ULONG)pSramBase, 8) )
    {
        mvOsPrintf("mvCesaInit: pSramBase (%p) must be 8 byte aligned\n",
                pSramBase);
        mvCesaFinish();
        return MV_NOT_ALIGNED;
    }
    cesaSramVirtPtr = (MV_CESA_SRAM_MAP*)pSramBase;
    
    cesaCryptEngBase = cryptEngBase;

    /*memset(cesaSramVirtPtr, 0, sizeof(MV_CESA_SRAM_MAP));*/

    /* Clear registers */
    MV_REG_WRITE( MV_CESA_CFG_REG, 0);
    MV_REG_WRITE( MV_CESA_ISR_CAUSE_REG, 0);
    MV_REG_WRITE( MV_CESA_ISR_MASK_REG, 0);

    /* Initialize DMA descriptor lists for all requests in Request queue */
    descOffsetReg = configReg = 0;
    for(req=0; req<queueDepth; req++)
    {
        int             frag;
        MV_CESA_REQ*    pReq;
        MV_DMA_DESC*    pDmaDesc;

        pReq = &pCesaReqFirst[req];

        pReq->cesaDescBuf.bufSize = sizeof(MV_CESA_DESC)*MV_CESA_MAX_REQ_FRAGS + 
                                        CPU_D_CACHE_LINE_SIZE;

	pReq->cesaDescBuf.bufVirtPtr = 
		mvOsIoCachedMalloc(osHandle,pReq->cesaDescBuf.bufSize,
				   &pReq->cesaDescBuf.bufPhysAddr,
				   &pReq->cesaDescBuf.memHandle);

        if(pReq->cesaDescBuf.bufVirtPtr == NULL)
        {
            mvOsPrintf("mvCesaInit: req=%d, Can't allocate %d bytes for CESA descriptors\n", 
                        req, pReq->cesaDescBuf.bufSize);
                mvCesaFinish();
                return MV_NO_RESOURCE;
            }
        memset(pReq->cesaDescBuf.bufVirtPtr, 0, pReq->cesaDescBuf.bufSize); 
        pReq->pCesaDesc = (MV_CESA_DESC*)MV_ALIGN_UP((MV_ULONG)pReq->cesaDescBuf.bufVirtPtr, 
                                                                CPU_D_CACHE_LINE_SIZE);

        pReq->dmaDescBuf.bufSize = sizeof(MV_DMA_DESC)*MV_CESA_MAX_DMA_DESC*MV_CESA_MAX_REQ_FRAGS + 
                                    CPU_D_CACHE_LINE_SIZE;

        pReq->dmaDescBuf.bufVirtPtr = 
		mvOsIoCachedMalloc(osHandle,pReq->dmaDescBuf.bufSize,
				   &pReq->dmaDescBuf.bufPhysAddr,
				   &pReq->dmaDescBuf.memHandle);

        if(pReq->dmaDescBuf.bufVirtPtr == NULL)
        {
            mvOsPrintf("mvCesaInit: req=%d, Can't allocate %d bytes for DMA descriptor list\n", 
                        req, pReq->dmaDescBuf.bufSize);
            mvCesaFinish();
            return MV_NO_RESOURCE;
        }
        memset(pReq->dmaDescBuf.bufVirtPtr, 0, pReq->dmaDescBuf.bufSize); 
        pDmaDesc = (MV_DMA_DESC*)MV_ALIGN_UP((MV_ULONG)pReq->dmaDescBuf.bufVirtPtr, 
                                                       CPU_D_CACHE_LINE_SIZE);

        for(frag=0; frag<MV_CESA_MAX_REQ_FRAGS; frag++)
        {
            MV_CESA_DMA*    pDma = &pReq->dma[frag];

            pDma->pDmaFirst = pDmaDesc;
            pDma->pDmaLast = NULL;
            
            for(i=0; i<MV_CESA_MAX_DMA_DESC-1; i++)
            {
                /* link all DMA descriptors together */
                pDma->pDmaFirst[i].phyNextDescPtr = 
                        MV_32BIT_LE(mvCesaVirtToPhys(&pReq->dmaDescBuf, &pDmaDesc[i+1]));
            }
            pDma->pDmaFirst[i].phyNextDescPtr = 0;
            mvOsCacheFlush(NULL, &pDma->pDmaFirst[0], MV_CESA_MAX_DMA_DESC*sizeof(MV_DMA_DESC));        

            pDmaDesc += MV_CESA_MAX_DMA_DESC;
        }        
    }
    /*mvCesaCryptoIvSet(NULL, MV_CESA_MAX_IV_LENGTH);*/
    descOffsetReg = (MV_U16)((MV_U8*)&cesaSramVirtPtr->desc - mvCesaSramAddrGet());
    MV_REG_WRITE(MV_CESA_CHAN_DESC_OFFSET_REG, descOffsetReg);

    configReg |= (MV_CESA_CFG_WAIT_DMA_MASK | MV_CESA_CFG_ACT_DMA_MASK);
#if (MV_CESA_VERSION >= 3)
    configReg |= MV_CESA_CFG_CHAIN_MODE_MASK;
#endif

#if (MV_CESA_VERSION >= 2)
    /* Initialize TDMA engine */
    MV_REG_WRITE(MV_CESA_TDMA_CTRL_REG, MV_CESA_TDMA_CTRL_VALUE);
    MV_REG_WRITE(MV_CESA_TDMA_BYTE_COUNT_REG, 0);
    MV_REG_WRITE(MV_CESA_TDMA_CURR_DESC_PTR_REG, 0);
#else
    /* Initialize IDMA #0 engine */
    MV_REG_WRITE(IDMA_CTRL_LOW_REG(0), 0);
    MV_REG_WRITE(IDMA_BYTE_COUNT_REG(0), 0);
    MV_REG_WRITE(IDMA_CURR_DESC_PTR_REG(0), 0);
    MV_REG_WRITE(IDMA_CTRL_HIGH_REG(0), ICCHR_ENDIAN_LITTLE
#ifdef MV_CPU_LE
      		| ICCHR_DESC_BYTE_SWAP_EN
#endif
		 );
    /* Clear Cause Byte of IDMA channel to be used */
    MV_REG_WRITE( IDMA_CAUSE_REG, ~ICICR_CAUSE_MASK_ALL(0));
    MV_REG_WRITE(IDMA_CTRL_LOW_REG(0), MV_CESA_IDMA_CTRL_LOW_VALUE);
#endif /* (MV_CESA_VERSION >= 2) */

    /* Set CESA configuration registers */
    MV_REG_WRITE( MV_CESA_CFG_REG, configReg);
    mvCesaDebugStatsClear();

    return MV_OK;
}

/*******************************************************************************
* mvCesaFinish - Shutdown the CESA driver
*
* DESCRIPTION:
*       This function shutdown the CESA driver and free all allocted resources.
*
* INPUT:    None
*
* RETURN:
*       MV_OK   - Success
*       Other   - Fail
*
*******************************************************************************/
MV_STATUS   mvCesaFinish (void)
{
    int             req;
    MV_CESA_REQ*    pReq;

    mvOsPrintf("mvCesaFinish: \n");

    cesaSramVirtPtr = NULL;

    /* Free all resources: DMA list, etc. */
    for(req=0; req<cesaQueueDepth; req++)
    {
    	pReq = &pCesaReqFirst[req];
        if(pReq->dmaDescBuf.bufVirtPtr != NULL)
        {
        	mvOsIoCachedFree(cesaOsHandle,pReq->dmaDescBuf.bufSize,
				 pReq->dmaDescBuf.bufPhysAddr,
				 pReq->dmaDescBuf.bufVirtPtr,
				 pReq->dmaDescBuf.memHandle);
        }
        if(pReq->cesaDescBuf.bufVirtPtr != NULL)
        {
                mvOsIoCachedFree(cesaOsHandle,pReq->cesaDescBuf.bufSize,
				 pReq->cesaDescBuf.bufPhysAddr,
				 pReq->cesaDescBuf.bufVirtPtr,
				 pReq->cesaDescBuf.memHandle);
        }
    }
#if (MV_CESA_VERSION < 2)
    MV_REG_WRITE(IDMA_CTRL_LOW_REG(0), 0);
#endif /* (MV_CESA_VERSION < 2) */

    /* Free request queue */
    if(pCesaReqFirst != NULL)
    {
        mvOsFree(pCesaReqFirst);
        pCesaReqFirst = pCesaReqLast = NULL;
        pCesaReqEmpty = pCesaReqProcess = NULL;
        cesaQueueDepth = cesaReqResources = 0;
    }
    /* Free SA database */
    if(pCesaSAD != NULL)
    {
        mvOsFree(pCesaSAD);
        pCesaSAD = NULL;
        cesaMaxSA = 0;
    }
    MV_REG_WRITE( MV_CESA_CFG_REG, 0);
    MV_REG_WRITE( MV_CESA_ISR_CAUSE_REG, 0);
    MV_REG_WRITE( MV_CESA_ISR_MASK_REG, 0);

    return MV_OK;
}

/*******************************************************************************
* mvCesaCryptoIvSet - Set IV value for Crypto algorithm working in CBC mode
*
* DESCRIPTION:
*    This function set IV value using by Crypto algorithms in CBC mode.
*   Each channel has its own IV value.
*   This function gets IV value from the caller. If no IV value passed from
*   the caller or only part of IV passed, the function will init the rest part
*   of IV value (or the whole IV) by random value.
*
* INPUT:
*       MV_U8*  pIV     - Pointer to IV value supplied by user. If pIV==NULL
*                       the function will generate random IV value.
*       int     ivSize  - size (in bytes) of IV provided by user. If ivSize is
*                       smaller than maximum IV size, the function will complete
*                       IV by random value.
*
* RETURN:
*       MV_OK   - Success
*       Other   - Fail
*
*******************************************************************************/
MV_STATUS   mvCesaCryptoIvSet(MV_U8* pIV, int ivSize)
{
    MV_U8*  pSramIV;
#if defined(MV646xx)
    mvOsPrintf("mvCesaCryptoIvSet: ERR. shouldn't use this call on MV64660\n");
#endif
    pSramIV = cesaSramVirtPtr->cryptoIV;
    if(ivSize > MV_CESA_MAX_IV_LENGTH)
    {
        mvOsPrintf("mvCesaCryptoIvSet: ivSize (%d) is too large\n", ivSize);
        ivSize = MV_CESA_MAX_IV_LENGTH;
    }
    if(pIV != NULL)
    {
        memcpy(pSramIV, pIV, ivSize);
        ivSize = MV_CESA_MAX_IV_LENGTH - ivSize;
        pSramIV += ivSize;
    }

    while(ivSize > 0)
    {
        int size, mv_random = mvOsRand();

        size = MV_MIN(ivSize, sizeof(mv_random));
        memcpy(pSramIV, (void*)&mv_random, size);

        pSramIV += size;
        ivSize -= size;
    }
/*
    mvOsCacheFlush(NULL, cesaSramVirtPtr->cryptoIV,
                                MV_CESA_MAX_IV_LENGTH);
    mvOsCacheInvalidate(NULL, cesaSramVirtPtr->cryptoIV,
                              MV_CESA_MAX_IV_LENGTH);
*/
    return MV_OK;
}

/*******************************************************************************
* mvCesaSessionOpen - Open new uni-directional crypto session
*
* DESCRIPTION:
*       This function open new session.
*
* INPUT:
*       MV_CESA_OPEN_SESSION *pSession - pointer to new session input parameters
*
* OUTPUT:
*       short           *pSid  - session ID, should be used for all future
*                                   requests over this session.
*
* RETURN:
*       MV_OK           - Session opend successfully.
*       MV_FULL         - All sessions are in use, no free place in
*                       SA database.
*       MV_BAD_PARAM    - One of session input parameters is invalid.
*
*******************************************************************************/
MV_STATUS   mvCesaSessionOpen(MV_CESA_OPEN_SESSION *pSession, short* pSid)
{
    short       sid;
    MV_U32      config = 0;
    int         digestSize;

    cesaStats.openedCount++;
    
    /* Find free entry in SAD */
    for(sid=0; sid<cesaMaxSA; sid++)
    {
        if(pCesaSAD[sid].valid == 0)
        {
            break;
        }
    }
    if(sid == cesaMaxSA)
    {
        mvOsPrintf("mvCesaSessionOpen: SA Database is FULL\n");
        return MV_FULL;
    }

    /* Check Input parameters for Open session */
    if (pSession->operation >= MV_CESA_MAX_OPERATION)
    {
        mvOsPrintf("mvCesaSessionOpen: Unexpected operation %d\n",
                    pSession->operation);
        return MV_BAD_PARAM;
    }
    config |= (pSession->operation << MV_CESA_OPERATION_OFFSET);

    if( (pSession->direction != MV_CESA_DIR_ENCODE) &&
        (pSession->direction != MV_CESA_DIR_DECODE) )
    {
        mvOsPrintf("mvCesaSessionOpen: Unexpected direction %d\n",
                    pSession->direction);
        return MV_BAD_PARAM;
    }
    config |= (pSession->direction << MV_CESA_DIRECTION_BIT);
    /* Clear SA entry */
    /* memset(&pCesaSAD[sid], 0, sizeof(pCesaSAD[sid])); */

    /* Check AUTH parameters and update SA entry */
    if(pSession->operation != MV_CESA_CRYPTO_ONLY)
    {
        /* For HMAC (MD5 and SHA1) - Maximum Key size is 64 bytes */
        if( (pSession->macMode == MV_CESA_MAC_HMAC_MD5) ||
            (pSession->macMode == MV_CESA_MAC_HMAC_SHA1) )
        {
            if(pSession->macKeyLength > MV_CESA_MAX_MAC_KEY_LENGTH)
            {
                mvOsPrintf("mvCesaSessionOpen: macKeyLength %d is too large\n",
                            pSession->macKeyLength);
                return MV_BAD_PARAM;
            }
            mvCesaHmacIvGet(pSession->macMode, pSession->macKey, pSession->macKeyLength,
                            pCesaSAD[sid].pSramSA->macInnerIV, 
                            pCesaSAD[sid].pSramSA->macOuterIV);
            pCesaSAD[sid].macKeyLength = pSession->macKeyLength;
        }
        switch(pSession->macMode)
        {
            case MV_CESA_MAC_MD5:
            case MV_CESA_MAC_HMAC_MD5:
                digestSize = MV_CESA_MD5_DIGEST_SIZE;
                break;

            case MV_CESA_MAC_SHA1:
            case MV_CESA_MAC_HMAC_SHA1:
                digestSize = MV_CESA_SHA1_DIGEST_SIZE;
                break;

            default:
                mvOsPrintf("mvCesaSessionOpen: Unexpected macMode %d\n",
                            pSession->macMode);
                return MV_BAD_PARAM;
        }
        config |= (pSession->macMode << MV_CESA_MAC_MODE_OFFSET);

        /* Supported digest sizes: MD5 - 16 bytes (128 bits), */
        /* SHA1 - 20 bytes (160 bits) or 12 bytes (96 bits) for both */
        if( (pSession->digestSize != digestSize) && (pSession->digestSize != 12))
        {
            mvOsPrintf("mvCesaSessionOpen: Unexpected digest size %d\n",
                        pSession->digestSize);
            mvOsPrintf("\t Valid values [bytes]: MD5-16, SHA1-20, Both-12\n");
            return MV_BAD_PARAM;
        }
        pCesaSAD[sid].digestSize = pSession->digestSize;

        if(pCesaSAD[sid].digestSize == 12)
        {
            /* Set MV_CESA_MAC_DIGEST_SIZE_BIT if digest size is 96 bits */
            config |= (MV_CESA_MAC_DIGEST_96B << MV_CESA_MAC_DIGEST_SIZE_BIT);
        }
    }

    /* Check CRYPTO parameters and update SA entry */
    if(pSession->operation != MV_CESA_MAC_ONLY)
    {
        switch(pSession->cryptoAlgorithm)
        {
            case MV_CESA_CRYPTO_DES:
                pCesaSAD[sid].cryptoKeyLength = MV_CESA_DES_KEY_LENGTH;
                pCesaSAD[sid].cryptoBlockSize = MV_CESA_DES_BLOCK_SIZE;
                break;

            case MV_CESA_CRYPTO_3DES:
                pCesaSAD[sid].cryptoKeyLength = MV_CESA_3DES_KEY_LENGTH;
                pCesaSAD[sid].cryptoBlockSize = MV_CESA_DES_BLOCK_SIZE;
                /* Only EDE mode is supported */
                config |= (MV_CESA_CRYPTO_3DES_EDE <<
                            MV_CESA_CRYPTO_3DES_MODE_BIT);
                break;

            case MV_CESA_CRYPTO_AES:
                switch(pSession->cryptoKeyLength)
                {
                    case 16:
                        pCesaSAD[sid].cryptoKeyLength = MV_CESA_AES_128_KEY_LENGTH;
                        config |= (MV_CESA_CRYPTO_AES_KEY_128 <<
                                       MV_CESA_CRYPTO_AES_KEY_LEN_OFFSET);
                        break;

                    case 24:
                        pCesaSAD[sid].cryptoKeyLength = MV_CESA_AES_192_KEY_LENGTH;
                        config |= (MV_CESA_CRYPTO_AES_KEY_192 <<
                                       MV_CESA_CRYPTO_AES_KEY_LEN_OFFSET);
                        break;

                    case 32:
                    default:
                        pCesaSAD[sid].cryptoKeyLength = MV_CESA_AES_256_KEY_LENGTH;
                        config |= (MV_CESA_CRYPTO_AES_KEY_256 <<
                                       MV_CESA_CRYPTO_AES_KEY_LEN_OFFSET);
                        break;
                }
                pCesaSAD[sid].cryptoBlockSize = MV_CESA_AES_BLOCK_SIZE;
                break;

            default:
                mvOsPrintf("mvCesaSessionOpen: Unexpected cryptoAlgorithm %d\n",
                            pSession->cryptoAlgorithm);
                return MV_BAD_PARAM;
        }
        config |= (pSession->cryptoAlgorithm << MV_CESA_CRYPTO_ALG_OFFSET);

        if(pSession->cryptoKeyLength != pCesaSAD[sid].cryptoKeyLength)
        {
            mvOsPrintf("cesaSessionOpen: Wrong CryptoKeySize %d != %d\n",
                        pSession->cryptoKeyLength, pCesaSAD[sid].cryptoKeyLength);
            return MV_BAD_PARAM;
        }
        
        /* Copy Crypto key */
        if( (pSession->cryptoAlgorithm == MV_CESA_CRYPTO_AES) &&
            (pSession->direction == MV_CESA_DIR_DECODE))
        {
            /* Crypto Key for AES decode is computed from original key material */
            /* and depend on cryptoKeyLength (128/192/256 bits) */
            aesMakeKey(pCesaSAD[sid].pSramSA->cryptoKey, pSession->cryptoKey, 
                        pSession->cryptoKeyLength*8, MV_CESA_AES_BLOCK_SIZE*8);
        }
        else
        {
                /*panic("mvCesaSessionOpen2");*/
                memcpy(pCesaSAD[sid].pSramSA->cryptoKey, pSession->cryptoKey, 
                    pCesaSAD[sid].cryptoKeyLength);
            
        }
        
        switch(pSession->cryptoMode)
        {
            case MV_CESA_CRYPTO_ECB:
                pCesaSAD[sid].cryptoIvSize = 0;
                break;

            case MV_CESA_CRYPTO_CBC:
                pCesaSAD[sid].cryptoIvSize = pCesaSAD[sid].cryptoBlockSize;
                break;

            case MV_CESA_CRYPTO_CTR:
                /* Supported only for AES algorithm */
                if(pSession->cryptoAlgorithm != MV_CESA_CRYPTO_AES)
                {
                    mvOsPrintf("mvCesaSessionOpen: CRYPTO CTR mode supported for AES only\n");
                    return MV_BAD_PARAM;
                }
                pCesaSAD[sid].cryptoIvSize = 0;
                pCesaSAD[sid].ctrMode = 1;
                /* Replace to ECB mode for HW */
                pSession->cryptoMode = MV_CESA_CRYPTO_ECB;
                break;

            default:
                mvOsPrintf("mvCesaSessionOpen: Unexpected cryptoMode %d\n",
                            pSession->cryptoMode);
                return MV_BAD_PARAM;
        }
        
        config |= (pSession->cryptoMode << MV_CESA_CRYPTO_MODE_BIT);
    }
    pCesaSAD[sid].config = config;
    
    mvOsCacheFlush(NULL, pCesaSAD[sid].pSramSA, sizeof(MV_CESA_SRAM_SA));
    if(pSid != NULL)
        *pSid = sid;

    pCesaSAD[sid].valid = 1;
    return MV_OK;
}

/*******************************************************************************
* mvCesaSessionClose - Close active crypto session
*
* DESCRIPTION:
*       This function closes existing session
*
* INPUT:
*       short sid   - Unique identifier of the session to be closed
*
* RETURN:
*       MV_OK        - Session closed successfully.
*       MV_BAD_PARAM - Session identifier is out of valid range.
*       MV_NOT_FOUND - There is no active session with such ID.
*
*******************************************************************************/
MV_STATUS mvCesaSessionClose(short sid)
{
    cesaStats.closedCount++;

    if(sid >= cesaMaxSA)
    {
        mvOsPrintf("CESA Error: sid (%d) is too big\n", sid);
        return MV_BAD_PARAM;
    }
    if(pCesaSAD[sid].valid == 0)
    {
        mvOsPrintf("CESA Warning: Session (sid=%d) is invalid\n", sid);
        return MV_NOT_FOUND;
    }
    if(cesaLastSid == sid)
        cesaLastSid = -1;

    pCesaSAD[sid].valid = 0;
    return MV_OK;
}

/*******************************************************************************
* mvCesaAction - Perform crypto operation
*
* DESCRIPTION:
*       This function set new CESA request FIFO queue for further HW processing.
*       The function checks request parameters before set new request to the queue.
*       If one of the CESA channels is ready for processing the request will be
*       passed to HW. When request processing is finished the CESA interrupt will
*       be generated by HW. The caller should call mvCesaReadyGet() function to
*       complete request processing and get result.
*
* INPUT:
*       MV_CESA_COMMAND *pCmd   - pointer to new CESA request.
*                               It includes pointers to Source and Destination
*                               buffers, session identifier get from
*                               mvCesaSessionOpen() function, pointer to caller
*                               private data and all needed crypto parameters.
*
* RETURN:
*       MV_OK             - request successfully added to request queue
*                         and will be processed.
*       MV_NO_MORE        - request successfully added to request queue and will
*                         be processed, but request queue became Full and next
*                         request will not be accepted.
*       MV_NO_RESOURCE    - request queue is FULL and the request can not
*                         be processed.
*       MV_OUT_OF_CPU_MEM - memory allocation needed for request processing is
*                         failed. Request can not be processed.
*       MV_NOT_ALLOWED    - This mixed request (CRYPTO+MAC) can not be processed
*                         as one request and should be splitted for two requests:
*                         CRYPTO_ONLY and MAC_ONLY.
*       MV_BAD_PARAM      - One of the request parameters is out of valid range.
*                         The request can not be processed.
*
*******************************************************************************/
MV_STATUS   mvCesaAction (MV_CESA_COMMAND *pCmd)
{
    MV_STATUS       status;
    MV_CESA_REQ*    pReq = pCesaReqEmpty;
    int             sid = pCmd->sessionId;
    MV_CESA_SA*     pSA = &pCesaSAD[sid];
#if (MV_CESA_VERSION >= 3)
     MV_CESA_REQ*   pFromReq;
     MV_CESA_REQ*   pToReq;
#endif
    cesaStats.reqCount++;

    /* Check that the request queue is not FULL */
    if(cesaReqResources == 0)
        return MV_NO_RESOURCE;

    if( (sid >= cesaMaxSA) || (!pSA->valid) )
    {
        mvOsPrintf("CESA Action Error: Session sid=%d is INVALID\n", sid);
        return MV_BAD_PARAM;
    }
    pSA->count++;

    if(pSA->ctrMode)
    {
        /* AES in CTR mode can't be mixed with Authentication */
        if( (pSA->config & MV_CESA_OPERATION_MASK) !=
            (MV_CESA_CRYPTO_ONLY << MV_CESA_OPERATION_OFFSET) )
        {
            mvOsPrintf("mvCesaAction : CRYPTO CTR mode can't be mixed with AUTH\n");
            return MV_NOT_ALLOWED;
        }
        /* All other request parameters should not be checked because key stream */
        /* (not user data) processed by AES HW engine */
        pReq->pOrgCmd = pCmd;
        /* Allocate temporary pCmd structure for Key stream */
        pCmd = mvCesaCtrModeInit();
        if(pCmd == NULL)
            return MV_OUT_OF_CPU_MEM;

        /* Prepare Key stream */
        mvCesaCtrModePrepare(pCmd, pReq->pOrgCmd);
        pReq->fixOffset = 0;
    }
    else
    {
        /* Check request parameters and calculae fixOffset */
        status = mvCesaParamCheck(pSA, pCmd, &pReq->fixOffset);
        if(status != MV_OK)
        {
            return status;
        }
    }
    pReq->pCmd = pCmd;

    /* Check if the packet need fragmentation */
    if(pCmd->pSrc->mbufSize <= sizeof(cesaSramVirtPtr->buf) )
    {
        /* request size is smaller than single buffer size */
        pReq->fragMode = MV_CESA_FRAG_NONE;

        /* Prepare NOT fragmented packets */
        status = mvCesaReqProcess(pReq);
        if(status != MV_OK)
        {
            mvOsPrintf("CesaReady: ReqProcess error: pReq=%p, status=0x%x\n",
                        pReq, status);
        }
#if (MV_CESA_VERSION >= 3)
	pReq->frags.numFrag = 1;
#endif
    }
    else
    {
        MV_U8     frag = 0;

        /* request size is larger than buffer size - needs fragmentation */

        /* Check restrictions for processing fragmented packets */
        status = mvCesaFragParamCheck(pSA, pCmd);
        if(status != MV_OK)
            return status;

        pReq->fragMode = MV_CESA_FRAG_FIRST;
        pReq->frags.nextFrag = 0;

        /* Prepare Process Fragmented packets */
        while(pReq->fragMode != MV_CESA_FRAG_LAST)
        {
            if(frag >= MV_CESA_MAX_REQ_FRAGS)
            {
                mvOsPrintf("mvCesaAction Error: Too large request frag=%d\n", frag);
                return MV_OUT_OF_CPU_MEM;
            }
            status = mvCesaFragReqProcess(pReq, frag);
            if(status == MV_OK) {
#if (MV_CESA_VERSION >= 3)
		if(frag) {
			pReq->dma[frag-1].pDmaLast->phyNextDescPtr = 
				MV_32BIT_LE(mvCesaVirtToPhys(&pReq->dmaDescBuf, pReq->dma[frag].pDmaFirst));
			mvOsCacheFlush(NULL, pReq->dma[frag-1].pDmaLast, sizeof(MV_DMA_DESC));
		}
#endif
                frag++;
        }
        }
        pReq->frags.numFrag = frag;
#if (MV_CESA_VERSION >= 3)
	if(chainReqNum) {	
		chainReqNum += pReq->frags.numFrag;
		if(chainReqNum >= MAX_CESA_CHAIN_LENGTH)
			chainReqNum = MAX_CESA_CHAIN_LENGTH;
	}		
#endif
    }

    pReq->state = MV_CESA_PENDING;

    pCesaReqEmpty = MV_CESA_REQ_NEXT_PTR(pReq);
    cesaReqResources -= 1;

/* #ifdef CESA_DEBUG */
    if( (cesaQueueDepth - cesaReqResources) > cesaStats.maxReqCount)
        cesaStats.maxReqCount = (cesaQueueDepth - cesaReqResources);
/* #endif CESA_DEBUG */

    cesaLastSid = sid;

#if (MV_CESA_VERSION >= 3)
    /* Are we within chain bounderies and follows the first request ? */
    if((chainReqNum > 0) && (chainReqNum < MAX_CESA_CHAIN_LENGTH)) {
	if(chainIndex) {
		pFromReq = MV_CESA_REQ_PREV_PTR(pReq); 
		pToReq = pReq;
		pReq->state = MV_CESA_CHAIN;
		/* assume concatenating is possible */
		pFromReq->dma[pFromReq->frags.numFrag-1].pDmaLast->phyNextDescPtr = 
			MV_32BIT_LE(mvCesaVirtToPhys(&pToReq->dmaDescBuf, pToReq->dma[0].pDmaFirst));
		mvOsCacheFlush(NULL, pFromReq->dma[pFromReq->frags.numFrag-1].pDmaLast, sizeof(MV_DMA_DESC));
			
		/* align active & next pointers */
		if(pNextActiveChain->state != MV_CESA_PENDING)
			pEndCurrChain = pNextActiveChain = MV_CESA_REQ_NEXT_PTR(pReq);
	}
	else { /* we have only one chain, start new one */
		chainReqNum = 0;
		chainIndex++;
		/* align active & next pointers  */
		if(pNextActiveChain->state != MV_CESA_PENDING)
			pEndCurrChain = pNextActiveChain = pReq;
	}
    }
    else {
		/* In case we concatenate full chain */
		if(chainReqNum == MAX_CESA_CHAIN_LENGTH) {
			chainIndex++;
			if(pNextActiveChain->state != MV_CESA_PENDING)
				pEndCurrChain = pNextActiveChain = pReq;
			chainReqNum = 0;
		}
		
		pReq = pCesaReqProcess;
		if(pReq->state == MV_CESA_PENDING) {
			pNextActiveChain = pReq;
			pEndCurrChain = MV_CESA_REQ_NEXT_PTR(pReq);
        		/* Start Process new request */
 	      		mvCesaReqProcessStart(pReq);
    		}
    }

    chainReqNum++;

    if((chainIndex < MAX_CESA_CHAIN_LENGTH) && (chainReqNum > cesaStats.maxChainUsage))
	cesaStats.maxChainUsage = chainReqNum;

#else

    /* Check status of CESA channels and process requests if possible */
    pReq = pCesaReqProcess;
    if(pReq->state == MV_CESA_PENDING)
    {
        /* Start Process new request */
        mvCesaReqProcessStart(pReq);
    }
#endif
    /* If request queue became FULL - return MV_NO_MORE */
    if(cesaReqResources == 0)
        return MV_NO_MORE;

    return MV_OK;

}

/*******************************************************************************
* mvCesaReadyGet - Get crypto request that processing is finished
*
* DESCRIPTION:
*       This function complete request processing and return ready request to
*       caller. To don't miss interrupts the caller must call this function
*       while MV_OK or MV_TERMINATE values returned.
*
* INPUT:
*   MV_U32          chanMap  - map of CESA channels finished thier job
*                              accordingly with CESA Cause register.
*   MV_CESA_RESULT* pResult  - pointer to structure contains information
*                            about ready request. It includes pointer to
*                            user private structure "pReqPrv", session identifier
*                            for this request "sessionId" and return code.
*                            Return code set to MV_FAIL if calculated digest value
*                            on decode direction is different than digest value
*                            in the packet.
*
* RETURN:
*       MV_OK           - Success, ready request is returned.
*       MV_NOT_READY    - Next request is not ready yet. New interrupt will
*                       be generated for futher request processing.
*       MV_EMPTY        - There is no more request for processing.
*       MV_BUSY         - Fragmented request is not ready yet.
*       MV_TERMINATE    - Call this function once more to complete processing
*                       of fragmented request.
*
*******************************************************************************/
MV_STATUS mvCesaReadyGet(MV_CESA_RESULT* pResult)
{
    MV_STATUS       status, readyStatus = MV_NOT_READY;
    MV_U32          statusReg;
    MV_CESA_REQ*    pReq;
    MV_CESA_SA*     pSA;

#if (MV_CESA_VERSION >= 3)
    if(isFirstReq == MV_TRUE) {
	if(chainIndex == 0)
		chainReqNum = 0;
	
	isFirstReq = MV_FALSE;

	if(pNextActiveChain->state == MV_CESA_PENDING) {
		/* Start request Process */
		mvCesaReqProcessStart(pNextActiveChain);
		pEndCurrChain = pNextActiveChain;
		if(chainIndex > 0)
			chainIndex--;
		/* Update pNextActiveChain to next chain head */
		   while(pNextActiveChain->state == MV_CESA_CHAIN)
			pNextActiveChain = MV_CESA_REQ_NEXT_PTR(pNextActiveChain);
    	}    
    }
    
    /* Check if there are more processed requests - can we remove pEndCurrChain ??? */
    if(pCesaReqProcess == pEndCurrChain) {
		isFirstReq = MV_TRUE;
		pEndCurrChain = pNextActiveChain;
#else
    if(pCesaReqProcess->state != MV_CESA_PROCESS) {
#endif
        return MV_EMPTY;
    }

#ifdef CESA_DEBUG
    statusReg = MV_REG_READ(MV_CESA_STATUS_REG);
    if( statusReg & MV_CESA_STATUS_ACTIVE_MASK )
    {
        mvOsPrintf("mvCesaReadyGet: Not Ready, Status = 0x%x\n", statusReg);
        cesaStats.notReadyCount++;
        return MV_NOT_READY;
    }
#endif /* CESA_DEBUG */

    cesaStats.readyCount++;

    pReq = pCesaReqProcess;
    pSA = &pCesaSAD[pReq->pCmd->sessionId];

    pResult->retCode = MV_OK;
    if(pReq->fragMode != MV_CESA_FRAG_NONE)
    {
        MV_U8*          pNewDigest;
      int             frag;
#if (MV_CESA_VERSION >= 3)
      pReq->frags.nextFrag = 1;
      while(pReq->frags.nextFrag <= pReq->frags.numFrag) {
#endif
	frag = (pReq->frags.nextFrag - 1);

        /* Restore DMA descriptor list */
        pReq->dma[frag].pDmaLast->phyNextDescPtr = 
                MV_32BIT_LE(mvCesaVirtToPhys(&pReq->dmaDescBuf, &pReq->dma[frag].pDmaLast[1]));
        pReq->dma[frag].pDmaLast = NULL;

        /* Special processing for finished fragmented request */
        if(pReq->frags.nextFrag >= pReq->frags.numFrag)
        {
            mvCesaMbufCacheUnmap(pReq->pCmd->pDst, 0, pReq->pCmd->pDst->mbufSize);

            /* Fragmented packet is ready */
            if( (pSA->config & MV_CESA_OPERATION_MASK) !=
                (MV_CESA_CRYPTO_ONLY << MV_CESA_OPERATION_OFFSET) )
            {
                int  macDataSize = pReq->pCmd->macLength - pReq->frags.macSize;

                if(macDataSize != 0)
                {
                    /* Calculate all other blocks by SW */
                    mvCesaFragAuthComplete(pReq, pSA, macDataSize);
                }

                /* Copy new digest from SRAM to the Destination buffer */
                pNewDigest = cesaSramVirtPtr->buf + pReq->frags.newDigestOffset;
                status = mvCesaCopyToMbuf(pNewDigest, pReq->pCmd->pDst,
                                   pReq->pCmd->digestOffset, pSA->digestSize);

                /* For decryption: Compare new digest value with original one */
                if((pSA->config & MV_CESA_DIRECTION_MASK) ==
                            (MV_CESA_DIR_DECODE << MV_CESA_DIRECTION_BIT))
                {
                    if( memcmp(pNewDigest, pReq->frags.orgDigest, pSA->digestSize) != 0)
                    {
/*
                        mvOsPrintf("Digest error: chan=%d, newDigest=%p, orgDigest=%p, status = 0x%x\n",
                            chan, pNewDigest, pReq->frags.orgDigest, MV_REG_READ(MV_CESA_STATUS_REG));
*/
                        /* Signiture verification is failed */
                        pResult->retCode = MV_FAIL;
                    }
                }
            }
            readyStatus = MV_OK;
        }
#if (MV_CESA_VERSION >= 3)
	pReq->frags.nextFrag++;
      }
#endif
    }
    else
    {
        mvCesaMbufCacheUnmap(pReq->pCmd->pDst, 0, pReq->pCmd->pDst->mbufSize);

        /* Restore DMA descriptor list */
        pReq->dma[0].pDmaLast->phyNextDescPtr = 
                MV_32BIT_LE(mvCesaVirtToPhys(&pReq->dmaDescBuf, &pReq->dma[0].pDmaLast[1]));
        pReq->dma[0].pDmaLast = NULL;
        if( ((pSA->config & MV_CESA_OPERATION_MASK) !=
                (MV_CESA_CRYPTO_ONLY << MV_CESA_OPERATION_OFFSET) ) &&
            ((pSA->config & MV_CESA_DIRECTION_MASK) ==
                        (MV_CESA_DIR_DECODE << MV_CESA_DIRECTION_BIT)) )
        {
            /* For AUTH on decode : Check Digest result in Status register */
            statusReg = MV_REG_READ(MV_CESA_STATUS_REG);
            if(statusReg & MV_CESA_STATUS_DIGEST_ERR_MASK)
            {
/*
                mvOsPrintf("Digest error: chan=%d, status = 0x%x\n",
                        chan, statusReg);
*/
                /* Signiture verification is failed */
                pResult->retCode = MV_FAIL;
            }
        }
        readyStatus = MV_OK;
    }

    if(readyStatus == MV_OK)
    {
        /* If Request is ready - Prepare pResult structure */
        pResult->pReqPrv = pReq->pCmd->pReqPrv;
        pResult->sessionId = pReq->pCmd->sessionId;

        pReq->state = MV_CESA_IDLE;
        pCesaReqProcess = MV_CESA_REQ_NEXT_PTR(pReq);
        cesaReqResources++;

        if(pSA->ctrMode)
        {
            /* For AES CTR mode - complete processing and free allocated resources */
            mvCesaCtrModeComplete(pReq->pOrgCmd, pReq->pCmd);
            mvCesaCtrModeFinish(pReq->pCmd);
            pReq->pOrgCmd = NULL;
        }
    }

#if (MV_CESA_VERSION < 3)
    if(pCesaReqProcess->state == MV_CESA_PROCESS)
    {
        /* Start request Process */
        mvCesaReqProcessStart(pCesaReqProcess);
        if(readyStatus == MV_NOT_READY)
            readyStatus = MV_BUSY;
    }
    else if(pCesaReqProcess != pCesaReqEmpty)
    {
        /* Start process new request from the queue */
        mvCesaReqProcessStart(pCesaReqProcess);
    }
#endif 
    return readyStatus;
}

/***************** Functions to work with CESA_MBUF structure ******************/

/*******************************************************************************
* mvCesaMbufOffset - Locate offset in the Mbuf structure
*
* DESCRIPTION:
*       This function locates offset inside Multi-Bufeer structure.
*       It get fragment number and place in the fragment where the offset
*       is located.
*
*
* INPUT:
*   MV_CESA_MBUF* pMbuf  - Pointer to multi-buffer structure
*   int           offset - Offset from the beginning of the data presented by
*                        the Mbuf structure.
*
* OUTPUT:
*   int*        pBufOffset  - Offset from the beginning of the fragment where
*                           the offset is located.
*
* RETURN:
*       int - Number of fragment, where the offset is located\
*
*******************************************************************************/
int     mvCesaMbufOffset(MV_CESA_MBUF* pMbuf, int offset, int* pBufOffset)
{
    int frag = 0;

    while(offset > 0)
    {
        if(frag >= pMbuf->numFrags)
        {
            mvOsPrintf("mvCesaMbufOffset: Error: frag (%d) > numFrags (%d)\n",
                    frag, pMbuf->numFrags);
            return MV_INVALID;
        }
        if(offset < pMbuf->pFrags[frag].bufSize)
        {
            break;
        }
        offset -= pMbuf->pFrags[frag].bufSize;
        frag++;
    }
    if(pBufOffset != NULL)
        *pBufOffset = offset;

    return frag;
}

/*******************************************************************************
* mvCesaCopyFromMbuf - Copy data from the Mbuf structure to continuous buffer
*
* DESCRIPTION:
*
*
* INPUT:
*   MV_U8*          pDstBuf  - Pointer to continuous buffer, where data is
*                              copied to.
*   MV_CESA_MBUF*   pSrcMbuf - Pointer to multi-buffer structure where data is
*                              copied from.
*   int             offset   - Offset in the Mbuf structure where located first
*                            byte of data should be copied.
*   int             size     - Size of data should be copied
*
* RETURN:
*       MV_OK           - Success, all data is copied successfully.
*       MV_OUT_OF_RANGE - Failed, offset is out of Multi-buffer data range.
*                         No data is copied.
*       MV_EMPTY        - Multi-buffer structure has not enough data to copy
*                       Data from the offset to end of Mbuf data is copied.
*
*******************************************************************************/
MV_STATUS   mvCesaCopyFromMbuf(MV_U8* pDstBuf, MV_CESA_MBUF* pSrcMbuf,
                               int offset, int size)
{
    int     frag, fragOffset, bufSize;
    MV_U8*  pBuf;

    if(size == 0)
        return MV_OK;

    frag = mvCesaMbufOffset(pSrcMbuf, offset, &fragOffset);
    if(frag == MV_INVALID)
    {
        mvOsPrintf("CESA Mbuf Error: offset (%d) out of range\n", offset);
        return MV_OUT_OF_RANGE;
    }

    bufSize = pSrcMbuf->pFrags[frag].bufSize - fragOffset;
    pBuf = pSrcMbuf->pFrags[frag].bufVirtPtr + fragOffset;
    while(MV_TRUE)
    {
        if(size <= bufSize)
        {
            memcpy(pDstBuf, pBuf, size);
            return MV_OK;
        }
        memcpy(pDstBuf, pBuf, bufSize);
        size -= bufSize;
        frag++;
        pDstBuf += bufSize;
        if(frag >= pSrcMbuf->numFrags)
            break;

        bufSize = pSrcMbuf->pFrags[frag].bufSize;
        pBuf = pSrcMbuf->pFrags[frag].bufVirtPtr;
    }
    mvOsPrintf("mvCesaCopyFromMbuf: Mbuf is EMPTY - %d bytes isn't copied\n",
                size);
    return MV_EMPTY;
}

/*******************************************************************************
* mvCesaCopyToMbuf - Copy data from continuous buffer to the Mbuf structure
*
* DESCRIPTION:
*
*
* INPUT:
*   MV_U8*          pSrcBuf  - Pointer to continuous buffer, where data is
*                              copied from.
*   MV_CESA_MBUF*   pDstMbuf - Pointer to multi-buffer structure where data is
*                              copied to.
*   int             offset   - Offset in the Mbuf structure where located first
*                            byte of data should be copied.
*   int             size     - Size of data should be copied
*
* RETURN:
*       MV_OK           - Success, all data is copied successfully.
*       MV_OUT_OF_RANGE - Failed, offset is out of Multi-buffer data range.
*                         No data is copied.
*       MV_FULL         - Multi-buffer structure has not enough place to copy
*                       all data. Data from the offset to end of Mbuf data
*                       is copied.
*
*******************************************************************************/
MV_STATUS   mvCesaCopyToMbuf(MV_U8* pSrcBuf, MV_CESA_MBUF* pDstMbuf,
                               int offset, int size)
{
    int     frag, fragOffset, bufSize;
    MV_U8*  pBuf;

    if(size == 0)
        return MV_OK;

    frag = mvCesaMbufOffset(pDstMbuf, offset, &fragOffset);
    if(frag == MV_INVALID)
    {
        mvOsPrintf("CESA Mbuf Error: offset (%d) out of range\n", offset);
        return MV_OUT_OF_RANGE;
    }

    bufSize = pDstMbuf->pFrags[frag].bufSize - fragOffset;
    pBuf = pDstMbuf->pFrags[frag].bufVirtPtr + fragOffset;
    while(MV_TRUE)
    {
        if(size <= bufSize)
        {
            memcpy(pBuf, pSrcBuf, size);
            return MV_OK;
        }
        memcpy(pBuf, pSrcBuf, bufSize);
        size -= bufSize;
        frag++;
        pSrcBuf += bufSize;
        if(frag >= pDstMbuf->numFrags)
            break;

        bufSize = pDstMbuf->pFrags[frag].bufSize;
        pBuf = pDstMbuf->pFrags[frag].bufVirtPtr;
    }
    mvOsPrintf("mvCesaCopyToMbuf: Mbuf is FULL - %d bytes isn't copied\n",
                size);
    return MV_FULL;
}

/*******************************************************************************
* mvCesaMbufCopy - Copy data from one Mbuf structure to the other Mbuf structure
*
* DESCRIPTION:
*
*
* INPUT:
*
*   MV_CESA_MBUF*   pDstMbuf - Pointer to multi-buffer structure where data is
*                              copied to.
*   int      dstMbufOffset   - Offset in the dstMbuf structure where first byte
*                            of data should be copied to.
*   MV_CESA_MBUF*   pSrcMbuf - Pointer to multi-buffer structure where data is
*                              copied from.
*   int      srcMbufOffset   - Offset in the srcMbuf structure where first byte
*                            of data should be copied from.
*   int             size     - Size of data should be copied
*
* RETURN:
*       MV_OK           - Success, all data is copied successfully.
*       MV_OUT_OF_RANGE - Failed, srcMbufOffset or dstMbufOffset is out of
*                       srcMbuf or dstMbuf structure correspondently.
*                       No data is copied.
*       MV_BAD_SIZE     - srcMbuf or dstMbuf structure is too small to copy
*                       all data. Partial data is copied
*
*******************************************************************************/
MV_STATUS   mvCesaMbufCopy(MV_CESA_MBUF* pMbufDst, int dstMbufOffset,
                           MV_CESA_MBUF* pMbufSrc, int srcMbufOffset, int size)
{
    int     srcFrag, dstFrag, srcSize, dstSize, srcOffset, dstOffset;
    int     copySize;
    MV_U8   *pSrc, *pDst;

    if(size == 0)
        return MV_OK;

    srcFrag = mvCesaMbufOffset(pMbufSrc, srcMbufOffset, &srcOffset);
    if(srcFrag == MV_INVALID)
    {
        mvOsPrintf("CESA srcMbuf Error: offset (%d) out of range\n", srcMbufOffset);
        return MV_OUT_OF_RANGE;
    }
    pSrc = pMbufSrc->pFrags[srcFrag].bufVirtPtr + srcOffset;
    srcSize = pMbufSrc->pFrags[srcFrag].bufSize - srcOffset;

    dstFrag = mvCesaMbufOffset(pMbufDst, dstMbufOffset, &dstOffset);
    if(dstFrag == MV_INVALID)
    {
        mvOsPrintf("CESA dstMbuf Error: offset (%d) out of range\n", dstMbufOffset);
        return MV_OUT_OF_RANGE;
    }
    pDst = pMbufDst->pFrags[dstFrag].bufVirtPtr + dstOffset;
    dstSize = pMbufDst->pFrags[dstFrag].bufSize - dstOffset;

    while(size > 0)
    {
        copySize = MV_MIN(srcSize, dstSize);
        if(size <= copySize)
        {
            memcpy(pDst, pSrc, size);
            return MV_OK;
        }
        memcpy(pDst, pSrc, copySize);
        size -= copySize;
        srcSize -= copySize;
        dstSize -= copySize;

        if(srcSize == 0)
        {
            srcFrag++;
            if(srcFrag >= pMbufSrc->numFrags)
                break;

            pSrc = pMbufSrc->pFrags[srcFrag].bufVirtPtr;
            srcSize = pMbufSrc->pFrags[srcFrag].bufSize;
        }

        if(dstSize == 0)
        {
            dstFrag++;
            if(dstFrag >= pMbufDst->numFrags)
                break;

            pDst = pMbufDst->pFrags[dstFrag].bufVirtPtr;
            dstSize = pMbufDst->pFrags[dstFrag].bufSize;
        }
    }
    mvOsPrintf("mvCesaMbufCopy: BAD size - %d bytes isn't copied\n",
                size);

    return MV_BAD_SIZE;
}

static MV_STATUS   mvCesaMbufCacheUnmap(MV_CESA_MBUF* pMbuf, int offset, int size)
{
    int     frag, fragOffset, bufSize;
    MV_U8*  pBuf;

    if(size == 0)
        return MV_OK;

    frag = mvCesaMbufOffset(pMbuf, offset, &fragOffset);
    if(frag == MV_INVALID)
    {
        mvOsPrintf("CESA Mbuf Error: offset (%d) out of range\n", offset);
        return MV_OUT_OF_RANGE;
    }

    bufSize = pMbuf->pFrags[frag].bufSize - fragOffset;
    pBuf = pMbuf->pFrags[frag].bufVirtPtr + fragOffset;
    while(MV_TRUE)
    {
        if(size <= bufSize)
        {
            mvOsCacheUnmap(NULL, mvOsIoVirtToPhy(NULL, pBuf), size);
            return MV_OK;
        }

        mvOsCacheUnmap(NULL, mvOsIoVirtToPhy(NULL, pBuf), bufSize);
        size -= bufSize;
        frag++;
        if(frag >= pMbuf->numFrags)
            break;

        bufSize = pMbuf->pFrags[frag].bufSize;
        pBuf = pMbuf->pFrags[frag].bufVirtPtr;
    }
    mvOsPrintf("%s: Mbuf is FULL - %d bytes isn't Unmapped\n",
                __FUNCTION__, size);
    return MV_FULL;
}


/*************************************** Local Functions ******************************/

/*******************************************************************************
* mvCesaFragReqProcess - Process fragmented request
*
* DESCRIPTION:
*       This function processes a fragment of fragmented request (First, Middle or Last)
*
*
* INPUT:
*       MV_CESA_REQ* pReq   - Pointer to the request in the request queue.
*
* RETURN:
*       MV_OK        - The fragment is successfully passed to HW for processing.
*       MV_TERMINATE - Means, that HW finished its work on this packet and no more
*                    interrupts will be generated for this request.
*                    Function mvCesaReadyGet() must be called to complete request
*                    processing and get request result.
*
*******************************************************************************/
static MV_STATUS   mvCesaFragReqProcess(MV_CESA_REQ* pReq, MV_U8 frag)
{
    int                     i, copySize, cryptoDataSize, macDataSize, sid;
    int                     cryptoIvOffset, digestOffset;
    MV_U32                  config;
    MV_CESA_COMMAND*        pCmd = pReq->pCmd;
    MV_CESA_SA*             pSA;
    MV_CESA_MBUF*           pMbuf;
    MV_DMA_DESC*            pDmaDesc = pReq->dma[frag].pDmaFirst;
    MV_U8*                  pSramBuf = cesaSramVirtPtr->buf;
    int                     macTotalLen = 0;
    int                     fixOffset, cryptoOffset, macOffset;

    cesaStats.fragCount++;

    sid = pReq->pCmd->sessionId;

    pSA = &pCesaSAD[sid];

    cryptoIvOffset = digestOffset = 0;
    i = macDataSize = 0;
    cryptoDataSize = 0;

    /* First fragment processing */
    if(pReq->fragMode == MV_CESA_FRAG_FIRST)
    {
        /* pReq->frags monitors processing of fragmented request between fragments */
        pReq->frags.bufOffset = 0;
        pReq->frags.cryptoSize = 0;
        pReq->frags.macSize = 0;

        config = pSA->config | (MV_CESA_FRAG_FIRST << MV_CESA_FRAG_MODE_OFFSET);

        /* fixOffset can be not equal to zero only for FIRST fragment */
        fixOffset = pReq->fixOffset;
        /* For FIRST fragment crypto and mac offsets are taken from pCmd */
        cryptoOffset = pCmd->cryptoOffset;
        macOffset = pCmd->macOffset;

        copySize = sizeof(cesaSramVirtPtr->buf) - pReq->fixOffset;

        /* Find fragment size: Must meet all requirements for CRYPTO and MAC
         * cryptoDataSize   - size of data will be encrypted/decrypted in this fragment
         * macDataSize      - size of data will be signed/verified in this fragment
         * copySize         - size of data will be copied from srcMbuf to SRAM and
         *                  back to dstMbuf for this fragment
         */
        mvCesaFragSizeFind(pSA, pReq, cryptoOffset, macOffset,
                        &copySize, &cryptoDataSize, &macDataSize);

        if( (pSA->config & MV_CESA_OPERATION_MASK) !=
                (MV_CESA_MAC_ONLY << MV_CESA_OPERATION_OFFSET))
        {
            /* CryptoIV special processing */
            if( (pSA->config & MV_CESA_CRYPTO_MODE_MASK) ==
                (MV_CESA_CRYPTO_CBC << MV_CESA_CRYPTO_MODE_BIT) )
            {
                /* In CBC mode for encode direction when IV from user */
                if( (pCmd->ivFromUser) &&
                    ((pSA->config & MV_CESA_DIRECTION_MASK) ==
                        (MV_CESA_DIR_ENCODE << MV_CESA_DIRECTION_BIT)) )
                {

                    /* For Crypto Encode in CBC mode HW always takes IV from SRAM IVPointer,
                    * (not from IVBufPointer). So when ivFromUser==1, we should copy IV from user place
                    * in the buffer to SRAM IVPointer
                    */
                    i += mvCesaDmaCopyPrepare(pCmd->pSrc, cesaSramVirtPtr->cryptoIV, &pDmaDesc[i],
                                    MV_FALSE, pCmd->ivOffset, pSA->cryptoIvSize, pCmd->skipFlush);
                }

                /* Special processing when IV is not located in the first fragment */
                if(pCmd->ivOffset > (copySize - pSA->cryptoIvSize))
                {
                    /* Prepare dummy place for cryptoIV in SRAM */
                    cryptoIvOffset = cesaSramVirtPtr->tempCryptoIV - mvCesaSramAddrGet();

                    /* For Decryption: Copy IV value from pCmd->ivOffset to Special SRAM place */
                    if((pSA->config & MV_CESA_DIRECTION_MASK) ==
                            (MV_CESA_DIR_DECODE << MV_CESA_DIRECTION_BIT))
                    {
                        i += mvCesaDmaCopyPrepare(pCmd->pSrc, cesaSramVirtPtr->tempCryptoIV, &pDmaDesc[i],
                                    MV_FALSE, pCmd->ivOffset, pSA->cryptoIvSize, pCmd->skipFlush);
                    }
                    else
                    {
                        /* For Encryption when IV is NOT from User: */
                        /* Copy IV from SRAM to buffer (pCmd->ivOffset) */
                        if(pCmd->ivFromUser == 0)
                        {
                            /* copy IV value from cryptoIV to Buffer (pCmd->ivOffset) */
                            i += mvCesaDmaCopyPrepare(pCmd->pSrc, cesaSramVirtPtr->cryptoIV, &pDmaDesc[i],
                                    MV_TRUE, pCmd->ivOffset, pSA->cryptoIvSize, pCmd->skipFlush);
                        }
                    }
                }
                else
                {
                    cryptoIvOffset = pCmd->ivOffset;
                }
            }
        }

        if( (pSA->config & MV_CESA_OPERATION_MASK) !=
                (MV_CESA_CRYPTO_ONLY << MV_CESA_OPERATION_OFFSET) )
        {
            /* MAC digest special processing on Decode direction */
            if((pSA->config & MV_CESA_DIRECTION_MASK) ==
                        (MV_CESA_DIR_DECODE << MV_CESA_DIRECTION_BIT))
            {
                /* Save digest from pCmd->digestOffset */
                mvCesaCopyFromMbuf(pReq->frags.orgDigest,
                               pCmd->pSrc, pCmd->digestOffset, pSA->digestSize);

                /* If pCmd->digestOffset is not located on the first */
                if(pCmd->digestOffset > (copySize - pSA->digestSize))
                {
                    MV_U8  digestZero[MV_CESA_MAX_DIGEST_SIZE];

                    /* Set zeros to pCmd->digestOffset (DRAM) */
                    memset(digestZero, 0, MV_CESA_MAX_DIGEST_SIZE);
                    mvCesaCopyToMbuf(digestZero, pCmd->pSrc, pCmd->digestOffset, pSA->digestSize);

                    /* Prepare dummy place for digest in SRAM */
                    digestOffset = cesaSramVirtPtr->tempDigest - mvCesaSramAddrGet();
                }
                else
                {
                    digestOffset = pCmd->digestOffset;
                }
            }
        }
        /* Update SA in SRAM */
        if(cesaLastSid != sid)
        {
            mvCesaSramSaUpdate(sid, &pDmaDesc[i]);
            i++;
        }

        pReq->fragMode = MV_CESA_FRAG_MIDDLE;
    }
    else
    {
        /* Continue fragment */
        fixOffset = 0;
        cryptoOffset = 0;
        macOffset = 0;
        if( (pCmd->pSrc->mbufSize - pReq->frags.bufOffset) <= sizeof(cesaSramVirtPtr->buf))
        {
            /* Last fragment */
            config = pSA->config | (MV_CESA_FRAG_LAST << MV_CESA_FRAG_MODE_OFFSET);
            pReq->fragMode = MV_CESA_FRAG_LAST;
            copySize = pCmd->pSrc->mbufSize - pReq->frags.bufOffset;

            if( (pSA->config & MV_CESA_OPERATION_MASK) !=
                (MV_CESA_CRYPTO_ONLY << MV_CESA_OPERATION_OFFSET) )
            {
                macDataSize = pCmd->macLength - pReq->frags.macSize;

                /* If pCmd->digestOffset is not located on last fragment */
                if(pCmd->digestOffset < pReq->frags.bufOffset)
                {
                    /* Prepare dummy place for digest in SRAM */
                    digestOffset = cesaSramVirtPtr->tempDigest - mvCesaSramAddrGet();
                }
                else
                {
                    digestOffset = pCmd->digestOffset - pReq->frags.bufOffset;
                }
                pReq->frags.newDigestOffset = digestOffset;
                macTotalLen = pCmd->macLength;

                /* HW can't calculate the Digest correctly for fragmented packets
                 * in the following cases:
                 *  - MV88F5182                                           ||
                 *  - MV88F5181L when total macLength more that 16 Kbytes ||
                 *  - total macLength more that 64 Kbytes
                 */
                if( (mvCtrlModelGet() == MV_5182_DEV_ID) ||
                    ( (mvCtrlModelGet() == MV_5181_DEV_ID) &&
                      (mvCtrlRevGet() >= MV_5181L_A0_REV)  &&
                      (pCmd->macLength >= (1 << 14)) ) )
                {
                    return MV_TERMINATE;
                }
            }
            if( (pSA->config & MV_CESA_OPERATION_MASK) !=
                (MV_CESA_MAC_ONLY << MV_CESA_OPERATION_OFFSET) )
            {
                cryptoDataSize = pCmd->cryptoLength - pReq->frags.cryptoSize;
            }

            /* cryptoIvOffset - don't care */
        }
        else
        {
            /* WA for MV88F5182 SHA1 and MD5 fragmentation mode */
            if( (mvCtrlModelGet() == MV_5182_DEV_ID) &&
                (((pSA->config & MV_CESA_MAC_MODE_MASK) ==
                    (MV_CESA_MAC_MD5 << MV_CESA_MAC_MODE_OFFSET)) ||
                ((pSA->config & MV_CESA_MAC_MODE_MASK) ==
                    (MV_CESA_MAC_SHA1 << MV_CESA_MAC_MODE_OFFSET))) )
            {
                pReq->frags.newDigestOffset = cesaSramVirtPtr->tempDigest - mvCesaSramAddrGet();
                pReq->fragMode = MV_CESA_FRAG_LAST;

                return MV_TERMINATE;
            }
            /* Middle fragment */
            config = pSA->config | (MV_CESA_FRAG_MIDDLE << MV_CESA_FRAG_MODE_OFFSET);
            copySize = sizeof(cesaSramVirtPtr->buf);
            /* digestOffset and cryptoIvOffset - don't care */

            /* Find fragment size */
            mvCesaFragSizeFind(pSA, pReq, cryptoOffset, macOffset,
                            &copySize, &cryptoDataSize, &macDataSize);
        }
    }
    /********* Prepare DMA descriptors to copy from pSrc to SRAM *********/
    pMbuf = pCmd->pSrc;
    i += mvCesaDmaCopyPrepare(pMbuf, pSramBuf + fixOffset, &pDmaDesc[i],
                                MV_FALSE, pReq->frags.bufOffset, copySize, pCmd->skipFlush);

    /* Prepare CESA descriptor to copy from DRAM to SRAM by DMA */
    mvCesaSramDescrBuild(config, frag, 
                cryptoOffset + fixOffset, cryptoIvOffset + fixOffset,
                cryptoDataSize, macOffset + fixOffset,
                digestOffset + fixOffset, macDataSize, macTotalLen,
                pReq, &pDmaDesc[i]);
    i++;

   /* Add special descriptor Ownership for CPU */
    pDmaDesc[i].byteCnt = 0;
    pDmaDesc[i].phySrcAdd = 0;
    pDmaDesc[i].phyDestAdd = 0;
    i++;

    /********* Prepare DMA descriptors to copy from SRAM to pDst *********/
    pMbuf = pCmd->pDst;
    i += mvCesaDmaCopyPrepare(pMbuf, pSramBuf + fixOffset, &pDmaDesc[i],
                                MV_TRUE, pReq->frags.bufOffset, copySize, pCmd->skipFlush);

    /* Next field of Last DMA descriptor must be NULL */
    pDmaDesc[i-1].phyNextDescPtr = 0;
    pReq->dma[frag].pDmaLast = &pDmaDesc[i-1];
    mvOsCacheFlush(NULL, pReq->dma[frag].pDmaFirst, 
                    i*sizeof(MV_DMA_DESC));

    /*mvCesaDebugDescriptor(&cesaSramVirtPtr->desc[frag]);*/

    pReq->frags.bufOffset += copySize;
    pReq->frags.cryptoSize += cryptoDataSize;
    pReq->frags.macSize += macDataSize;

    return MV_OK;
}


/*******************************************************************************
* mvCesaReqProcess - Process regular (Non-fragmented) request
*
* DESCRIPTION:
*       This function processes the whole (not fragmented) request
*
* INPUT:
*       MV_CESA_REQ* pReq   - Pointer to the request in the request queue.
*
* RETURN:
*       MV_OK   - The request is successfully passed to HW for processing.
*       Other   - Failure. The request will not be processed
*
*******************************************************************************/
static MV_STATUS   mvCesaReqProcess(MV_CESA_REQ* pReq)
{
    MV_CESA_MBUF    *pMbuf;
    MV_DMA_DESC     *pDmaDesc;
    MV_U8           *pSramBuf;
    int             sid, i, fixOffset;
    MV_CESA_SA      *pSA;
    MV_CESA_COMMAND *pCmd = pReq->pCmd;

    cesaStats.procCount++;

    sid = pCmd->sessionId;
    pSA = &pCesaSAD[sid];
    pDmaDesc = pReq->dma[0].pDmaFirst;
    pSramBuf = cesaSramVirtPtr->buf;
    fixOffset = pReq->fixOffset;

/*
    mvOsPrintf("mvCesaReqProcess: sid=%d, pSA=%p, pDmaDesc=%p, pSramBuf=%p\n",
                sid, pSA, pDmaDesc, pSramBuf);
*/
    i = 0;

    /* Crypto IV Special processing in CBC mode for Encryption direction */
    if( ((pSA->config & MV_CESA_OPERATION_MASK) != (MV_CESA_MAC_ONLY << MV_CESA_OPERATION_OFFSET)) &&
        ((pSA->config & MV_CESA_CRYPTO_MODE_MASK) == (MV_CESA_CRYPTO_CBC << MV_CESA_CRYPTO_MODE_BIT)) &&
        ((pSA->config & MV_CESA_DIRECTION_MASK) == (MV_CESA_DIR_ENCODE << MV_CESA_DIRECTION_BIT)) &&
        (pCmd->ivFromUser) )
    {
        /* For Crypto Encode in CBC mode HW always takes IV from SRAM IVPointer,
         * (not from IVBufPointer). So when ivFromUser==1, we should copy IV from user place
         * in the buffer to SRAM IVPointer
         */
        i += mvCesaDmaCopyPrepare(pCmd->pSrc, cesaSramVirtPtr->cryptoIV, &pDmaDesc[i],
                                    MV_FALSE, pCmd->ivOffset, pSA->cryptoIvSize, pCmd->skipFlush);
    }

    /* Update SA in SRAM */
    if(cesaLastSid != sid)
    {
        mvCesaSramSaUpdate(sid, &pDmaDesc[i]);
        i++;
    }

    /********* Prepare DMA descriptors to copy from pSrc to SRAM *********/
    pMbuf = pCmd->pSrc;
    i += mvCesaDmaCopyPrepare(pMbuf, pSramBuf + fixOffset, &pDmaDesc[i],
                                MV_FALSE, 0, pMbuf->mbufSize, pCmd->skipFlush);

    /* Prepare Security Accelerator descriptor to SRAM words 0 - 7 */
    mvCesaSramDescrBuild(pSA->config, 0, pCmd->cryptoOffset + fixOffset, 
                        pCmd->ivOffset + fixOffset, pCmd->cryptoLength,
                        pCmd->macOffset + fixOffset, pCmd->digestOffset + fixOffset,
                        pCmd->macLength, pCmd->macLength, pReq, &pDmaDesc[i]);
    i++;

   /* Add special descriptor Ownership for CPU */
    pDmaDesc[i].byteCnt = 0;
    pDmaDesc[i].phySrcAdd = 0;
    pDmaDesc[i].phyDestAdd = 0;
    i++;

    /********* Prepare DMA descriptors to copy from SRAM to pDst *********/
    pMbuf = pCmd->pDst;
    i += mvCesaDmaCopyPrepare(pMbuf, pSramBuf + fixOffset, &pDmaDesc[i],
                                MV_TRUE, 0, pMbuf->mbufSize, pCmd->skipFlush);

    /* Next field of Last DMA descriptor must be NULL */
    pDmaDesc[i-1].phyNextDescPtr = 0;
    pReq->dma[0].pDmaLast = &pDmaDesc[i-1];
    mvOsCacheFlush(NULL, pReq->dma[0].pDmaFirst, i*sizeof(MV_DMA_DESC));
            
    return MV_OK;
}


/*******************************************************************************
* mvCesaSramDescrBuild - Set CESA descriptor in SRAM
*
* DESCRIPTION:
*       This function builds CESA descriptor in SRAM from all Command parameters
*
*
* INPUT:
*       int     chan            - CESA channel uses the descriptor
*       MV_U32  config          - 32 bits of WORD_0 in CESA descriptor structure
*       int     cryptoOffset    - Offset from the beginning of SRAM buffer where
*                               data for encryption/decription is started.
*       int     ivOffset        - Offset of crypto IV from the SRAM base. Valid only
*                               for first fragment.
*       int     cryptoLength    - Size (in bytes) of data for encryption/descryption
*                               operation on this fragment.
*       int     macOffset       - Offset from the beginning of SRAM buffer where
*                               data for Authentication is started
*       int     digestOffset    - Offset from the beginning of SRAM buffer where
*                               digest is located. Valid for first and last fragments.
*       int     macLength       - Size (in bytes) of data for Authentication
*                               operation on this fragment.
*       int     macTotalLen     - Toatl size (in bytes) of data for Authentication
*                               operation on the whole request (packet). Valid for
*                               last fragment only.
*
* RETURN:   None
*
*******************************************************************************/
static void    mvCesaSramDescrBuild(MV_U32 config, int frag, 
                             int cryptoOffset, int ivOffset, int cryptoLength,
                             int macOffset, int digestOffset, int macLength,
                             int macTotalLen, MV_CESA_REQ* pReq, MV_DMA_DESC* pDmaDesc)
{    
    MV_CESA_DESC*   pCesaDesc = &pReq->pCesaDesc[frag];
    MV_CESA_DESC*   pSramDesc = pSramDesc = &cesaSramVirtPtr->desc;
    MV_U16          sramBufOffset = (MV_U16)((MV_U8*)cesaSramVirtPtr->buf - mvCesaSramAddrGet());

    pCesaDesc->config = MV_32BIT_LE(config);

    if( (config & MV_CESA_OPERATION_MASK) !=
         (MV_CESA_MAC_ONLY << MV_CESA_OPERATION_OFFSET) )
    {
        /* word 1 */
        pCesaDesc->cryptoSrcOffset = MV_16BIT_LE(sramBufOffset + cryptoOffset);
        pCesaDesc->cryptoDstOffset = MV_16BIT_LE(sramBufOffset + cryptoOffset);
        /* word 2 */
        pCesaDesc->cryptoDataLen = MV_16BIT_LE(cryptoLength);
        /* word 3 */
        pCesaDesc->cryptoKeyOffset = MV_16BIT_LE((MV_U16)(cesaSramVirtPtr->sramSA.cryptoKey - 
                                                            mvCesaSramAddrGet()));
        /* word 4 */
        pCesaDesc->cryptoIvOffset  = MV_16BIT_LE((MV_U16)(cesaSramVirtPtr->cryptoIV -
                                                            mvCesaSramAddrGet()));
        pCesaDesc->cryptoIvBufOffset = MV_16BIT_LE(sramBufOffset + ivOffset);
    }

    if( (config & MV_CESA_OPERATION_MASK) !=
         (MV_CESA_CRYPTO_ONLY << MV_CESA_OPERATION_OFFSET) )
    {
        /* word 5 */
        pCesaDesc->macSrcOffset = MV_16BIT_LE(sramBufOffset + macOffset);
        pCesaDesc->macTotalLen = MV_16BIT_LE(macTotalLen);

        /* word 6 */
        pCesaDesc->macDigestOffset = MV_16BIT_LE(sramBufOffset + digestOffset);
        pCesaDesc->macDataLen = MV_16BIT_LE(macLength);

        /* word 7 */
        pCesaDesc->macInnerIvOffset = MV_16BIT_LE((MV_U16)(cesaSramVirtPtr->sramSA.macInnerIV - 
                                 mvCesaSramAddrGet()));
        pCesaDesc->macOuterIvOffset = MV_16BIT_LE((MV_U16)(cesaSramVirtPtr->sramSA.macOuterIV - 
                                 mvCesaSramAddrGet()));
    }
    /* Prepare DMA descriptor to CESA descriptor from DRAM to SRAM */
    pDmaDesc->phySrcAdd = MV_32BIT_LE(mvCesaVirtToPhys(&pReq->cesaDescBuf, pCesaDesc));
    pDmaDesc->phyDestAdd = MV_32BIT_LE(mvCesaSramVirtToPhys(NULL, (MV_U8*)pSramDesc));
    pDmaDesc->byteCnt = MV_32BIT_LE(sizeof(MV_CESA_DESC) | BIT31);
                                
    /* flush Source buffer */
    mvOsCacheFlush(NULL, pCesaDesc, sizeof(MV_CESA_DESC));
}

/*******************************************************************************
* mvCesaSramSaUpdate - Move required SA information to SRAM if needed.
*
* DESCRIPTION:
*   Copy to SRAM values of the required SA.
*
*
* INPUT:
*       short       sid          - Session ID needs SRAM Cache update
*       MV_DMA_DESC *pDmaDesc   - Pointer to DMA descriptor used to
*                                copy SA values from DRAM to SRAM.
*
* RETURN:
*       MV_OK           - Cache entry for this SA copied to SRAM.
*       MV_NO_CHANGE    - Cache entry for this SA already exist in SRAM
*
*******************************************************************************/
static INLINE void   mvCesaSramSaUpdate(short sid, MV_DMA_DESC *pDmaDesc)
{
    MV_CESA_SA      *pSA = &pCesaSAD[sid];

    /* Prepare DMA descriptor to Copy CACHE_SA from SA database in DRAM to SRAM */
     pDmaDesc->byteCnt = MV_32BIT_LE(sizeof(MV_CESA_SRAM_SA) | BIT31);
    pDmaDesc->phySrcAdd = MV_32BIT_LE(mvCesaVirtToPhys(&cesaSramSaBuf, pSA->pSramSA));
     pDmaDesc->phyDestAdd =
          MV_32BIT_LE(mvCesaSramVirtToPhys(NULL, (MV_U8*)&cesaSramVirtPtr->sramSA));

    /* Source buffer is already flushed during OpenSession*/
    /*mvOsCacheFlush(NULL, &pSA->sramSA, sizeof(MV_CESA_SRAM_SA));*/
}

/*******************************************************************************
* mvCesaDmaCopyPrepare - prepare DMA descriptor list to copy data presented by
*                       Mbuf structure from DRAM to SRAM
*
* DESCRIPTION:
*
*
* INPUT:
*       MV_CESA_MBUF*   pMbuf       - pointer to Mbuf structure contains request
*                                   data in DRAM
*       MV_U8*          pSramBuf    - pointer to buffer in SRAM where data should
*                                   be copied to.
*       MV_DMA_DESC*    pDmaDesc   - pointer to first DMA descriptor for this copy.
*                                   The function set number of DMA descriptors needed
*                                   to copy the copySize bytes from Mbuf.
*       MV_BOOL         isToMbuf    - Copy direction.
*                                   MV_TRUE means copy from SRAM buffer to Mbuf in DRAM.
*                                   MV_FALSE means copy from Mbuf in DRAM to SRAM buffer.
*       int             offset      - Offset in the Mbuf structure that copy should be
*                                   started from.
*       int             copySize    - Size of data should be copied.
*
* RETURN:
*       int  - number of DMA descriptors used for the copy.
*
*******************************************************************************/
#ifndef MV_NETBSD
static INLINE int    mvCesaDmaCopyPrepare(MV_CESA_MBUF* pMbuf, MV_U8* pSramBuf, 
                        MV_DMA_DESC* pDmaDesc, MV_BOOL isToMbuf,
                        int offset, int copySize, MV_BOOL skipFlush)
{
    int     bufOffset, bufSize, size, frag, i;
    MV_U8*  pBuf;

    i = 0;

    /* Calculate start place for copy: fragment number and offset in the fragment */
    frag = mvCesaMbufOffset(pMbuf, offset, &bufOffset);
    bufSize = pMbuf->pFrags[frag].bufSize - bufOffset;
    pBuf = pMbuf->pFrags[frag].bufVirtPtr + bufOffset;

    /* Size accumulate total copy size */
    size = 0;

    /* Create DMA lists to copy mBuf from pSrc to SRAM */
    while(size < copySize)
    {
        /* Find copy size for each DMA descriptor */
        bufSize = MV_MIN(bufSize, (copySize - size));
        pDmaDesc[i].byteCnt = MV_32BIT_LE(bufSize | BIT31);
        if(isToMbuf)
        {
            pDmaDesc[i].phyDestAdd = MV_32BIT_LE(mvOsIoVirtToPhy(NULL, pBuf));
            pDmaDesc[i].phySrcAdd  =
                MV_32BIT_LE(mvCesaSramVirtToPhys(NULL, (pSramBuf + size)));
            /* invalidate the buffer */
	    if(skipFlush == MV_FALSE)
            	mvOsCacheInvalidate(NULL, pBuf, bufSize);
        }
        else
        {
            pDmaDesc[i].phySrcAdd = MV_32BIT_LE(mvOsIoVirtToPhy(NULL, pBuf));
            pDmaDesc[i].phyDestAdd =
                MV_32BIT_LE(mvCesaSramVirtToPhys(NULL, (pSramBuf + size)));
            /* flush the buffer */
	    if(skipFlush == MV_FALSE)
            	mvOsCacheFlush(NULL, pBuf, bufSize);
        }

        /* Count number of used DMA descriptors */
        i++;
        size += bufSize;

        /* go to next fragment in the Mbuf */
        frag++;
        pBuf = pMbuf->pFrags[frag].bufVirtPtr;
        bufSize = pMbuf->pFrags[frag].bufSize;
    }
    return i;
}
#else /* MV_NETBSD */
static int    mvCesaDmaCopyPrepare(MV_CESA_MBUF* pMbuf, MV_U8* pSramBuf,
                        MV_DMA_DESC* pDmaDesc, MV_BOOL isToMbuf,
                        int offset, int copySize, MV_BOOL skipFlush)
{
	int bufOffset, bufSize, thisSize, size, frag, i;
	MV_ULONG bufPhys, sramPhys;
    MV_U8*  pBuf;

	/*
	 * Calculate start place for copy: fragment number and offset in
	 * the fragment
	 */
    frag = mvCesaMbufOffset(pMbuf, offset, &bufOffset);

	/*
	 * Get SRAM physical address only once. We can update it in-place
	 * as we build the descriptor chain.
	 */
	sramPhys = mvCesaSramVirtToPhys(NULL, pSramBuf);

	/*
	 * 'size' accumulates total copy size, 'i' counts desccriptors.
	 */
	size = i = 0;

	/* Create DMA lists to copy mBuf from pSrc to SRAM */
	while (size < copySize) {
		/*
		 * Calculate # of bytes to copy from the current fragment,
		 * and the pointer to the start of data
		 */
    bufSize = pMbuf->pFrags[frag].bufSize - bufOffset;
    pBuf = pMbuf->pFrags[frag].bufVirtPtr + bufOffset;
		bufOffset = 0;	/* First frag may be non-zero */
		frag++;

		/*
		 * As long as there is data in the current fragment...
		 */
		while (bufSize > 0) {
			/*
			 * Ensure we don't cross an MMU page boundary.
			 * XXX: This is NetBSD-specific, but it is a
			 * quick and dirty way to fix the problem.
			 * A true HAL would rely on the OS-specific
			 * driver to do this...
			 */
			thisSize = PAGE_SIZE -
			    (((MV_ULONG)pBuf) & (PAGE_SIZE - 1));
			thisSize = MV_MIN(bufSize, thisSize);
			/*
			 * Make sure we don't copy more than requested
			 */
			if (thisSize > (copySize - size)) {
				thisSize = copySize - size;
				bufSize = 0;
			}

			/*
			 * Physicall address of this fragment
			 */
			bufPhys = MV_32BIT_LE(mvOsIoVirtToPhy(NULL, pBuf));

			/*
			 * Set up the descriptor
			 */
		        pDmaDesc[i].byteCnt = MV_32BIT_LE(thisSize | BIT31);
		        if(isToMbuf) {
				pDmaDesc[i].phyDestAdd = bufPhys;
				pDmaDesc[i].phySrcAdd  = MV_32BIT_LE(sramPhys);
            /* invalidate the buffer */
				if(skipFlush == MV_FALSE)
					mvOsCacheInvalidate(NULL, pBuf, thisSize);
			} else {
				pDmaDesc[i].phySrcAdd = bufPhys;
				pDmaDesc[i].phyDestAdd = MV_32BIT_LE(sramPhys);
            /* flush the buffer */
				if(skipFlush == MV_FALSE)
					mvOsCacheFlush(NULL, pBuf, thisSize);
			}

			pDmaDesc[i].phyNextDescPtr =
			    MV_32BIT_LE(mvOsIoVirtToPhy(NULL,(&pDmaDesc[i+1])));

        /* flush the DMA desc */
        mvOsCacheFlush(NULL, &pDmaDesc[i], sizeof(MV_DMA_DESC));

			/* Update state */
			bufSize -= thisSize;
			sramPhys += thisSize;
			pBuf += thisSize;
			size += thisSize;
        i++;
		}
	}

    return i;
}
#endif /* MV_NETBSD */
/*******************************************************************************
* mvCesaHmacIvGet - Calculate Inner and Outter values from HMAC key
*
* DESCRIPTION:
*       This function calculate Inner and Outer values used for HMAC algorithm.
*       This operation allows improve performance fro the whole HMAC processing.
*
* INPUT:
*       MV_CESA_MAC_MODE    macMode     - Authentication mode: HMAC_MD5 or HMAC_SHA1.
*       unsigned char       key[]       - Pointer to HMAC key.
*       int                 keyLength   - Size of HMAC key (maximum 64 bytes)
*
* OUTPUT:
*       unsigned char       innerIV[]   - HASH(key^inner)
*       unsigned char       outerIV[]   - HASH(key^outter)
*
* RETURN:   None
*
*******************************************************************************/
static void    mvCesaHmacIvGet(MV_CESA_MAC_MODE macMode, unsigned char key[], int keyLength,
                     unsigned char innerIV[], unsigned char outerIV[])
{
    unsigned char   inner[MV_CESA_MAX_MAC_KEY_LENGTH];
    unsigned char   outer[MV_CESA_MAX_MAC_KEY_LENGTH];
    int             i, digestSize = 0;
#if defined(MV_CPU_LE) || defined(MV_PPC)
    MV_U32          swapped32, val32, *pVal32;
#endif 
    for(i=0; i<keyLength; i++)
    {
        inner[i] = 0x36 ^ key[i];
        outer[i] = 0x5c ^ key[i];
    }

    for(i=keyLength; i<MV_CESA_MAX_MAC_KEY_LENGTH; i++)
    {
        inner[i] = 0x36;
        outer[i] = 0x5c;
    }
    if(macMode == MV_CESA_MAC_HMAC_MD5)
    {
        MV_MD5_CONTEXT  ctx;

        mvMD5Init(&ctx);
        mvMD5Update(&ctx, inner, MV_CESA_MAX_MAC_KEY_LENGTH);

        memcpy(innerIV, ctx.buf, MV_CESA_MD5_DIGEST_SIZE);
        memset(&ctx, 0, sizeof(ctx));

        mvMD5Init(&ctx);
        mvMD5Update(&ctx, outer, MV_CESA_MAX_MAC_KEY_LENGTH);
        memcpy(outerIV, ctx.buf, MV_CESA_MD5_DIGEST_SIZE);
        memset(&ctx, 0, sizeof(ctx));
        digestSize = MV_CESA_MD5_DIGEST_SIZE;
    }
    else if(macMode == MV_CESA_MAC_HMAC_SHA1)
    {
        MV_SHA1_CTX  ctx;

        mvSHA1Init(&ctx);
        mvSHA1Update(&ctx, inner, MV_CESA_MAX_MAC_KEY_LENGTH);
        memcpy(innerIV, ctx.state, MV_CESA_SHA1_DIGEST_SIZE);
        memset(&ctx, 0, sizeof(ctx));

        mvSHA1Init(&ctx);
        mvSHA1Update(&ctx, outer, MV_CESA_MAX_MAC_KEY_LENGTH);
        memcpy(outerIV, ctx.state, MV_CESA_SHA1_DIGEST_SIZE);
        memset(&ctx, 0, sizeof(ctx));
        digestSize = MV_CESA_SHA1_DIGEST_SIZE;
    }
    else
    {
        mvOsPrintf("hmacGetIV: Unexpected macMode %d\n", macMode);
    }
#if defined(MV_CPU_LE) || defined(MV_PPC)
    /* 32 bits Swap of Inner and Outer values */
    pVal32 = (MV_U32*)innerIV;
    for(i=0; i<digestSize/4; i++)
    {
        val32 = *pVal32;
        swapped32 = MV_BYTE_SWAP_32BIT(val32);
        *pVal32 = swapped32;
        pVal32++;
    }
    pVal32 = (MV_U32*)outerIV;
    for(i=0; i<digestSize/4; i++)
    {
        val32 = *pVal32;
        swapped32 = MV_BYTE_SWAP_32BIT(val32);
        *pVal32 = swapped32;
        pVal32++;
    }
#endif  /* defined(MV_CPU_LE) || defined(MV_PPC) */
}


/*******************************************************************************
* mvCesaFragSha1Complete - Complete SHA1 authentication started by HW using SW
*
* DESCRIPTION:
*
*
* INPUT:
*       MV_CESA_MBUF*   pMbuf           - Pointer to Mbuf structure where data
*                                       for SHA1 is placed.
*       int             offset          - Offset in the Mbuf structure where
*                                       unprocessed data for SHA1 is started.
*       MV_U8*          pOuterIV        - Pointer to OUTER for this session.
*                                       If pOuterIV==NULL - MAC mode is HASH_SHA1
*                                       If pOuterIV!=NULL - MAC mode is HMAC_SHA1
*       int             macLeftSize     - Size of unprocessed data for SHA1.
*       int             macTotalSize    - Total size of data for SHA1 in the
*                                       request (processed + unprocessed)
*
* OUTPUT:
*       MV_U8*     pDigest  - Pointer to place where calculated Digest will
*                           be stored.
*
* RETURN:   None
*
*******************************************************************************/
static void    mvCesaFragSha1Complete(MV_CESA_MBUF* pMbuf, int offset,
                                      MV_U8* pOuterIV, int macLeftSize,
                                      int macTotalSize, MV_U8* pDigest)
{
    MV_SHA1_CTX     ctx;
    MV_U8           *pData;
    int             i, frag, fragOffset, size;

    /* Read temporary Digest from HW */
    for(i=0; i<MV_CESA_SHA1_DIGEST_SIZE/4; i++)
    {
        ctx.state[i] = MV_REG_READ(MV_CESA_AUTH_INIT_VAL_DIGEST_REG(i));
    }
    /* Initialize MV_SHA1_CTX structure */
    memset(ctx.buffer, 0, 64);
    /* Set count[0] in bits. 32 bits is enough for 512 MBytes */
    /* so count[1] is always 0 */
    ctx.count[0] = ((macTotalSize - macLeftSize) * 8);
    ctx.count[1] = 0;

    /* If HMAC - add size of Inner block (64 bytes) ro count[0] */
    if(pOuterIV != NULL)
        ctx.count[0] += (64 * 8);

    /* Get place of unprocessed data in the Mbuf structure */
    frag = mvCesaMbufOffset(pMbuf, offset, &fragOffset);
    if(frag == MV_INVALID)
    {
        mvOsPrintf("CESA Mbuf Error: offset (%d) out of range\n", offset);
        return;
    }

    pData = pMbuf->pFrags[frag].bufVirtPtr + fragOffset;
    size = pMbuf->pFrags[frag].bufSize - fragOffset;

    /* Complete Inner part */
    while(macLeftSize > 0)
    {
        if(macLeftSize <= size)
        {
            mvSHA1Update(&ctx, pData, macLeftSize);
            break;
        }
        mvSHA1Update(&ctx, pData, size);
        macLeftSize -= size;
        frag++;
        pData = pMbuf->pFrags[frag].bufVirtPtr;
        size = pMbuf->pFrags[frag].bufSize;
    }
    mvSHA1Final(pDigest, &ctx);
/*
    mvOsPrintf("mvCesaFragSha1Complete: pOuterIV=%p, macLeftSize=%d, macTotalSize=%d\n",
                pOuterIV, macLeftSize, macTotalSize);
    mvDebugMemDump(pDigest, MV_CESA_SHA1_DIGEST_SIZE, 1);
*/

    if(pOuterIV != NULL)
    {
        /* If HMAC - Complete Outer part */
        for(i=0; i<MV_CESA_SHA1_DIGEST_SIZE/4; i++)
        {
#if defined(MV_CPU_LE) || defined(MV_ARM)
            ctx.state[i] = MV_BYTE_SWAP_32BIT(((MV_U32*)pOuterIV)[i]);
#else
	    ctx.state[i] = ((MV_U32*)pOuterIV)[i];
#endif
	}
        memset(ctx.buffer, 0, 64);

        ctx.count[0] = 64*8;
        ctx.count[1] = 0;
        mvSHA1Update(&ctx, pDigest, MV_CESA_SHA1_DIGEST_SIZE);
        mvSHA1Final(pDigest, &ctx);
    }
}

/*******************************************************************************
* mvCesaFragMd5Complete - Complete MD5 authentication started by HW using SW
*
* DESCRIPTION:
*
*
* INPUT:
*       MV_CESA_MBUF*   pMbuf           - Pointer to Mbuf structure where data
*                                       for SHA1 is placed.
*       int             offset          - Offset in the Mbuf structure where
*                                       unprocessed data for MD5 is started.
*       MV_U8*          pOuterIV        - Pointer to OUTER for this session.
*                                       If pOuterIV==NULL - MAC mode is HASH_MD5
*                                       If pOuterIV!=NULL - MAC mode is HMAC_MD5
*       int             macLeftSize     - Size of unprocessed data for MD5.
*       int             macTotalSize    - Total size of data for MD5 in the
*                                       request (processed + unprocessed)
*
* OUTPUT:
*       MV_U8*     pDigest  - Pointer to place where calculated Digest will
*                           be stored.
*
* RETURN:   None
*
*******************************************************************************/
static void    mvCesaFragMd5Complete(MV_CESA_MBUF* pMbuf, int offset,
                                     MV_U8* pOuterIV, int macLeftSize,
                                     int macTotalSize, MV_U8* pDigest)
{
    MV_MD5_CONTEXT  ctx;
    MV_U8           *pData;
    int             i, frag, fragOffset, size;

    /* Read temporary Digest from HW */
    for(i=0; i<MV_CESA_MD5_DIGEST_SIZE/4; i++)
    {
        ctx.buf[i] = MV_REG_READ(MV_CESA_AUTH_INIT_VAL_DIGEST_REG(i));
    }
    memset(ctx.in, 0, 64);

    /* Set count[0] in bits. 32 bits is enough for 512 MBytes */
    /* so count[1] is always 0 */
    ctx.bits[0] = ((macTotalSize - macLeftSize) * 8);
    ctx.bits[1] = 0;

    /* If HMAC - add size of Inner block (64 bytes) ro count[0] */
    if(pOuterIV != NULL)
        ctx.bits[0] += (64 * 8);

    frag = mvCesaMbufOffset(pMbuf, offset, &fragOffset);
    if(frag == MV_INVALID)
    {
        mvOsPrintf("CESA Mbuf Error: offset (%d) out of range\n", offset);
        return;
    }

    pData = pMbuf->pFrags[frag].bufVirtPtr + fragOffset;
    size = pMbuf->pFrags[frag].bufSize - fragOffset;

    /* Complete Inner part */
    while(macLeftSize > 0)
    {
        if(macLeftSize <= size)
        {
            mvMD5Update(&ctx, pData, macLeftSize);
            break;
        }
        mvMD5Update(&ctx, pData, size);
        macLeftSize -= size;
        frag++;
        pData = pMbuf->pFrags[frag].bufVirtPtr;
        size = pMbuf->pFrags[frag].bufSize;
    }
    mvMD5Final(pDigest, &ctx);

/*
    mvOsPrintf("mvCesaFragMd5Complete: pOuterIV=%p, macLeftSize=%d, macTotalSize=%d\n",
                pOuterIV, macLeftSize, macTotalSize);
    mvDebugMemDump(pDigest, MV_CESA_MD5_DIGEST_SIZE, 1);
*/
    if(pOuterIV != NULL)
    {
        /* Complete Outer part */
        for(i=0; i<MV_CESA_MD5_DIGEST_SIZE/4; i++)
        {
#if defined(MV_CPU_LE) || defined(MV_ARM)
            ctx.buf[i] = MV_BYTE_SWAP_32BIT(((MV_U32*)pOuterIV)[i]);
#else
	    ctx.buf[i] = ((MV_U32*)pOuterIV)[i];
#endif
	}
        memset(ctx.in, 0, 64);

        ctx.bits[0] = 64*8;
        ctx.bits[1] = 0;
        mvMD5Update(&ctx, pDigest, MV_CESA_MD5_DIGEST_SIZE);
        mvMD5Final(pDigest, &ctx);
    }
}

/*******************************************************************************
* mvCesaFragAuthComplete -
*
* DESCRIPTION:
*
*
* INPUT:
*       MV_CESA_REQ*    pReq,
*       MV_CESA_SA*     pSA,
*       int             macDataSize
*
* RETURN:
*       MV_STATUS
*
*******************************************************************************/
static MV_STATUS   mvCesaFragAuthComplete(MV_CESA_REQ* pReq, MV_CESA_SA* pSA,
                               int macDataSize)
{
    MV_CESA_COMMAND*        pCmd = pReq->pCmd;
    MV_U8*                  pDigest;
    MV_CESA_MAC_MODE        macMode;
    MV_U8*                  pOuterIV = NULL;

    /* Copy data from Source fragment to Destination */
    if(pCmd->pSrc != pCmd->pDst)
    {
        mvCesaMbufCopy(pCmd->pDst, pReq->frags.bufOffset,
                       pCmd->pSrc, pReq->frags.bufOffset, macDataSize);
    }

/*
    mvCesaCopyFromMbuf(cesaSramVirtPtr->buf[0], pCmd->pSrc, pReq->frags.bufOffset, macDataSize);
    mvCesaCopyToMbuf(cesaSramVirtPtr->buf[0], pCmd->pDst, pReq->frags.bufOffset, macDataSize);
*/
    pDigest = (mvCesaSramAddrGet() + pReq->frags.newDigestOffset);

    macMode = (pSA->config & MV_CESA_MAC_MODE_MASK) >> MV_CESA_MAC_MODE_OFFSET;
/*
    mvOsPrintf("macDataSize=%d, macLength=%d, digestOffset=%d, macMode=%d\n",
            macDataSize, pCmd->macLength, pCmd->digestOffset, macMode);
*/
    switch(macMode)
    {
        case MV_CESA_MAC_HMAC_MD5:
            pOuterIV = pSA->pSramSA->macOuterIV;

        case MV_CESA_MAC_MD5:
            mvCesaFragMd5Complete(pCmd->pDst, pReq->frags.bufOffset, pOuterIV,
                               macDataSize, pCmd->macLength, pDigest);
        break;

        case MV_CESA_MAC_HMAC_SHA1:
            pOuterIV = pSA->pSramSA->macOuterIV;

        case MV_CESA_MAC_SHA1:
            mvCesaFragSha1Complete(pCmd->pDst, pReq->frags.bufOffset, pOuterIV,
                               macDataSize, pCmd->macLength, pDigest);
        break;

        default:
            mvOsPrintf("mvCesaFragAuthComplete: Unexpected macMode %d\n", macMode);
            return MV_BAD_PARAM;
    }
    return MV_OK;
}

/*******************************************************************************
* mvCesaCtrModeInit -
*
* DESCRIPTION:
*
*
* INPUT: NONE
*
*
* RETURN:
*       MV_CESA_COMMAND*
*
*******************************************************************************/
static MV_CESA_COMMAND*    mvCesaCtrModeInit(void)
{
    MV_CESA_MBUF    *pMbuf;
    MV_U8           *pBuf;
    MV_CESA_COMMAND *pCmd;

    pBuf = mvOsMalloc(sizeof(MV_CESA_COMMAND) +
                      sizeof(MV_CESA_MBUF) + sizeof(MV_BUF_INFO) + 100);
    if(pBuf == NULL)
    {
        mvOsPrintf("mvCesaSessionOpen: Can't allocate %u bytes for CTR Mode\n",
                    sizeof(MV_CESA_COMMAND) + sizeof(MV_CESA_MBUF) + sizeof(MV_BUF_INFO) );
        return NULL;
    }
    pCmd = (MV_CESA_COMMAND*)pBuf;
    pBuf += sizeof(MV_CESA_COMMAND);

    pMbuf = (MV_CESA_MBUF*)pBuf;
    pBuf += sizeof(MV_CESA_MBUF);

    pMbuf->pFrags = (MV_BUF_INFO*)pBuf;

    pMbuf->numFrags = 1;
    pCmd->pSrc = pMbuf;
    pCmd->pDst = pMbuf;
/*
    mvOsPrintf("CtrModeInit: pCmd=%p, pSrc=%p, pDst=%p, pFrags=%p\n",
                pCmd, pCmd->pSrc, pCmd->pDst,
                pMbuf->pFrags);
*/
    return pCmd;
}

/*******************************************************************************
* mvCesaCtrModePrepare -
*
* DESCRIPTION:
*
*
* INPUT:
*       MV_CESA_COMMAND *pCtrModeCmd, MV_CESA_COMMAND *pCmd
*
* RETURN:
*       MV_STATUS
*
*******************************************************************************/
static MV_STATUS    mvCesaCtrModePrepare(MV_CESA_COMMAND *pCtrModeCmd, MV_CESA_COMMAND *pCmd)
{
    MV_CESA_MBUF    *pMbuf;
    MV_U8           *pBuf, *pIV;
    MV_U32          counter, *pCounter;
    int             cryptoSize = MV_ALIGN_UP(pCmd->cryptoLength, MV_CESA_AES_BLOCK_SIZE);
/*
    mvOsPrintf("CtrModePrepare: pCmd=%p, pCtrSrc=%p, pCtrDst=%p, pOrgCmd=%p, pOrgSrc=%p, pOrgDst=%p\n",
                pCmd, pCmd->pSrc, pCmd->pDst,
                pCtrModeCmd, pCtrModeCmd->pSrc, pCtrModeCmd->pDst);
*/
    pMbuf = pCtrModeCmd->pSrc;

    /* Allocate buffer for Key stream */
    pBuf = mvOsIoCachedMalloc(cesaOsHandle,cryptoSize, 
			      &pMbuf->pFrags[0].bufPhysAddr,
			      &pMbuf->pFrags[0].memHandle);
    if(pBuf == NULL)
    {
        mvOsPrintf("mvCesaCtrModePrepare: Can't allocate %d bytes\n", cryptoSize);
        return MV_OUT_OF_CPU_MEM;
    }
    memset(pBuf, 0, cryptoSize);
    mvOsCacheFlush(NULL, pBuf, cryptoSize);

    pMbuf->pFrags[0].bufVirtPtr = pBuf;
    pMbuf->mbufSize = cryptoSize;
    pMbuf->pFrags[0].bufSize = cryptoSize;

    pCtrModeCmd->pReqPrv = pCmd->pReqPrv;
    pCtrModeCmd->sessionId = pCmd->sessionId;

    /* ivFromUser and ivOffset are don't care */
    pCtrModeCmd->cryptoOffset = 0;
    pCtrModeCmd->cryptoLength = cryptoSize;

    /* digestOffset, macOffset and macLength are don't care */

    mvCesaCopyFromMbuf(pBuf, pCmd->pSrc, pCmd->ivOffset, MV_CESA_AES_BLOCK_SIZE);
    pCounter = (MV_U32*)(pBuf + (MV_CESA_AES_BLOCK_SIZE - sizeof(counter)));
    counter = *pCounter;
    counter = MV_32BIT_BE(counter);
    pIV = pBuf;
    cryptoSize -= MV_CESA_AES_BLOCK_SIZE;

    /* fill key stream */
    while(cryptoSize > 0)
    {
        pBuf += MV_CESA_AES_BLOCK_SIZE;
        memcpy(pBuf, pIV, MV_CESA_AES_BLOCK_SIZE - sizeof(counter));
        pCounter = (MV_U32*)(pBuf + (MV_CESA_AES_BLOCK_SIZE - sizeof(counter)));
        counter++;
        *pCounter = MV_32BIT_BE(counter);
        cryptoSize -= MV_CESA_AES_BLOCK_SIZE;
    }

    return MV_OK;
}

/*******************************************************************************
* mvCesaCtrModeComplete -
*
* DESCRIPTION:
*
*
* INPUT:
*       MV_CESA_COMMAND *pOrgCmd, MV_CESA_COMMAND *pCmd
*
* RETURN:
*       MV_STATUS
*
*******************************************************************************/
static MV_STATUS   mvCesaCtrModeComplete(MV_CESA_COMMAND *pOrgCmd, MV_CESA_COMMAND *pCmd)
{
    int         srcFrag, dstFrag, srcOffset, dstOffset, keyOffset, srcSize, dstSize;
    int         cryptoSize = pCmd->cryptoLength;
    MV_U8       *pSrc, *pDst, *pKey;
    MV_STATUS   status = MV_OK;
/*
    mvOsPrintf("CtrModeComplete: pCmd=%p, pCtrSrc=%p, pCtrDst=%p, pOrgCmd=%p, pOrgSrc=%p, pOrgDst=%p\n",
                pCmd, pCmd->pSrc, pCmd->pDst,
                pOrgCmd, pOrgCmd->pSrc, pOrgCmd->pDst);
*/
    /* XOR source data with key stream to destination data */
    pKey = pCmd->pDst->pFrags[0].bufVirtPtr;
    keyOffset = 0;

    if( (pOrgCmd->pSrc != pOrgCmd->pDst) &&
        (pOrgCmd->cryptoOffset > 0) )
    {
        /* Copy Prefix from source buffer to destination buffer */

        status = mvCesaMbufCopy(pOrgCmd->pDst, 0,
                                pOrgCmd->pSrc, 0, pOrgCmd->cryptoOffset);
/*
        status = mvCesaCopyFromMbuf(tempBuf, pOrgCmd->pSrc,
                       0, pOrgCmd->cryptoOffset);
        status = mvCesaCopyToMbuf(tempBuf, pOrgCmd->pDst,
                       0, pOrgCmd->cryptoOffset);
*/
    }

    srcFrag = mvCesaMbufOffset(pOrgCmd->pSrc, pOrgCmd->cryptoOffset, &srcOffset);
    pSrc = pOrgCmd->pSrc->pFrags[srcFrag].bufVirtPtr;
    srcSize = pOrgCmd->pSrc->pFrags[srcFrag].bufSize;

    dstFrag = mvCesaMbufOffset(pOrgCmd->pDst, pOrgCmd->cryptoOffset, &dstOffset);
    pDst = pOrgCmd->pDst->pFrags[dstFrag].bufVirtPtr;
    dstSize = pOrgCmd->pDst->pFrags[dstFrag].bufSize;

    while(cryptoSize > 0)
    {
        pDst[dstOffset] = (pSrc[srcOffset] ^ pKey[keyOffset]);

        cryptoSize--;
        dstOffset++;
        srcOffset++;
        keyOffset++;

        if(srcOffset >= srcSize)
        {
            srcFrag++;
            srcOffset = 0;
            pSrc = pOrgCmd->pSrc->pFrags[srcFrag].bufVirtPtr;
            srcSize = pOrgCmd->pSrc->pFrags[srcFrag].bufSize;
        }

        if(dstOffset >= dstSize)
        {
            dstFrag++;
            dstOffset = 0;
            pDst = pOrgCmd->pDst->pFrags[dstFrag].bufVirtPtr;
            dstSize = pOrgCmd->pDst->pFrags[dstFrag].bufSize;
        }
    }

    if(pOrgCmd->pSrc != pOrgCmd->pDst)
    {
        /* Copy Suffix from source buffer to destination buffer */
        srcOffset = pOrgCmd->cryptoOffset + pOrgCmd->cryptoLength;

        if( (pOrgCmd->pDst->mbufSize - srcOffset) > 0)
        {
            status = mvCesaMbufCopy(pOrgCmd->pDst, srcOffset,
                                    pOrgCmd->pSrc, srcOffset,
                                    pOrgCmd->pDst->mbufSize - srcOffset);
        }

/*
        status = mvCesaCopyFromMbuf(tempBuf, pOrgCmd->pSrc,
                                srcOffset, pOrgCmd->pSrc->mbufSize - srcOffset);
        status = mvCesaCopyToMbuf(tempBuf, pOrgCmd->pDst,
                       srcOffset, pOrgCmd->pDst->mbufSize - srcOffset);
*/
    }

    /* Free buffer used for Key stream */
    mvOsIoCachedFree(cesaOsHandle,pCmd->pDst->pFrags[0].bufSize,
		     pCmd->pDst->pFrags[0].bufPhysAddr,
                     pCmd->pDst->pFrags[0].bufVirtPtr,
		     pCmd->pDst->pFrags[0].memHandle);

    return MV_OK;
}

/*******************************************************************************
* mvCesaCtrModeFinish -
*
* DESCRIPTION:
*
*
* INPUT:
*       MV_CESA_COMMAND* pCmd
*
* RETURN:
*       MV_STATUS
*
*******************************************************************************/
static void    mvCesaCtrModeFinish(MV_CESA_COMMAND* pCmd)
{
    mvOsFree(pCmd);
}

/*******************************************************************************
* mvCesaParamCheck -
*
* DESCRIPTION:
*
*
* INPUT:
*       MV_CESA_SA* pSA, MV_CESA_COMMAND *pCmd, MV_U8* pFixOffset
*
* RETURN:
*       MV_STATUS
*
*******************************************************************************/
static MV_STATUS   mvCesaParamCheck(MV_CESA_SA* pSA, MV_CESA_COMMAND *pCmd,
                                    MV_U8* pFixOffset)
{
    MV_U8   fixOffset = 0xFF;

    /* Check AUTH operation parameters */
    if( ((pSA->config & MV_CESA_OPERATION_MASK) !=
                (MV_CESA_CRYPTO_ONLY << MV_CESA_OPERATION_OFFSET)) )
    {
        /* MAC offset should be at least 4 byte aligned */
        if( MV_IS_NOT_ALIGN(pCmd->macOffset, 4) )
        {
            mvOsPrintf("mvCesaAction: macOffset %d must be 4 byte aligned\n",
                    pCmd->macOffset);
            return MV_BAD_PARAM;
        }
        /* Digest offset must be 4 byte aligned */
        if( MV_IS_NOT_ALIGN(pCmd->digestOffset, 4) )
        {
            mvOsPrintf("mvCesaAction: digestOffset %d must be 4 byte aligned\n",
                    pCmd->digestOffset);
            return MV_BAD_PARAM;
        }
        /* In addition all offsets should be the same alignment: 8 or 4 */
        if(fixOffset == 0xFF)
        {
            fixOffset = (pCmd->macOffset % 8);
        }
        else
        {
            if( (pCmd->macOffset % 8) != fixOffset)
            {
                mvOsPrintf("mvCesaAction: macOffset %d mod 8 must be equal %d\n",
                                pCmd->macOffset, fixOffset);
                return MV_BAD_PARAM;
            }
        }
        if( (pCmd->digestOffset % 8) != fixOffset)
        {
            mvOsPrintf("mvCesaAction: digestOffset %d mod 8 must be equal %d\n",
                                pCmd->digestOffset, fixOffset);
            return MV_BAD_PARAM;
        }
    }
    /* Check CRYPTO operation parameters */
    if( ((pSA->config & MV_CESA_OPERATION_MASK) !=
                (MV_CESA_MAC_ONLY << MV_CESA_OPERATION_OFFSET)) )
    {
        /* CryptoOffset should be at least 4 byte aligned */
        if( MV_IS_NOT_ALIGN(pCmd->cryptoOffset, 4)  )
        {
            mvOsPrintf("CesaAction: cryptoOffset=%d must be 4 byte aligned\n",
                        pCmd->cryptoOffset);
            return MV_BAD_PARAM;
        }
        /* cryptoLength should be the whole number of blocks */
        if( MV_IS_NOT_ALIGN(pCmd->cryptoLength, pSA->cryptoBlockSize) )
        {
            mvOsPrintf("mvCesaAction: cryptoLength=%d must be %d byte aligned\n",
                        pCmd->cryptoLength, pSA->cryptoBlockSize);
            return MV_BAD_PARAM;
        }
        if(fixOffset == 0xFF)
        {
            fixOffset = (pCmd->cryptoOffset % 8);
        }
        else
        {
            /* In addition all offsets should be the same alignment: 8 or 4 */
            if( (pCmd->cryptoOffset % 8) != fixOffset)
            {
                mvOsPrintf("mvCesaAction: cryptoOffset %d mod 8 must be equal %d \n",
                                pCmd->cryptoOffset, fixOffset);
                return MV_BAD_PARAM;
            }
        }

        /* check for CBC mode */
        if(pSA->cryptoIvSize > 0)
        {
            /* cryptoIV must not be part of CryptoLength */
            if( ((pCmd->ivOffset + pSA->cryptoIvSize) > pCmd->cryptoOffset) &&
                (pCmd->ivOffset < (pCmd->cryptoOffset + pCmd->cryptoLength)) )
            {
                mvOsPrintf("mvCesaFragParamCheck: cryptoIvOffset (%d) is part of cryptoLength (%d+%d)\n",
                        pCmd->ivOffset, pCmd->macOffset, pCmd->macLength);
                return MV_BAD_PARAM;
            }

            /* ivOffset must be 4 byte aligned */
            if( MV_IS_NOT_ALIGN(pCmd->ivOffset, 4) )
            {
                mvOsPrintf("CesaAction: ivOffset=%d must be 4 byte aligned\n",
                            pCmd->ivOffset);
                return MV_BAD_PARAM;
            }
            /* In addition all offsets should be the same alignment: 8 or 4 */
            if( (pCmd->ivOffset % 8) != fixOffset)
            {
                mvOsPrintf("mvCesaAction: ivOffset %d mod 8 must be %d\n",
                                pCmd->ivOffset, fixOffset);
                return MV_BAD_PARAM;
            }
        }
    }
    return MV_OK;
}

/*******************************************************************************
* mvCesaFragParamCheck -
*
* DESCRIPTION:
*
*
* INPUT:
*       MV_CESA_SA* pSA, MV_CESA_COMMAND *pCmd
*
* RETURN:
*       MV_STATUS
*
*******************************************************************************/
static MV_STATUS   mvCesaFragParamCheck(MV_CESA_SA* pSA, MV_CESA_COMMAND *pCmd)
{
    int     offset;

    if( ((pSA->config & MV_CESA_OPERATION_MASK) !=
                (MV_CESA_CRYPTO_ONLY << MV_CESA_OPERATION_OFFSET)) )
    {
        /* macOffset must be less that SRAM buffer size */
        if(pCmd->macOffset > (sizeof(cesaSramVirtPtr->buf) - MV_CESA_AUTH_BLOCK_SIZE))
        {
            mvOsPrintf("mvCesaFragParamCheck: macOffset is too large (%d)\n",
                        pCmd->macOffset);
            return MV_BAD_PARAM;
        }
        /* macOffset+macSize must be more than mbufSize - SRAM buffer size */
        if( ((pCmd->macOffset + pCmd->macLength) > pCmd->pSrc->mbufSize) ||
            ((pCmd->pSrc->mbufSize - (pCmd->macOffset + pCmd->macLength)) >=
             sizeof(cesaSramVirtPtr->buf)) )
        {
            mvOsPrintf("mvCesaFragParamCheck: macLength is too large (%d), mbufSize=%d\n",
                        pCmd->macLength, pCmd->pSrc->mbufSize);
            return MV_BAD_PARAM;
        }
    }

    if( ((pSA->config & MV_CESA_OPERATION_MASK) !=
                (MV_CESA_MAC_ONLY << MV_CESA_OPERATION_OFFSET)) )
    {
        /* cryptoOffset must be less that SRAM buffer size */
        /* 4 for possible fixOffset */
        if( (pCmd->cryptoOffset + 4) > (sizeof(cesaSramVirtPtr->buf) - pSA->cryptoBlockSize))
        {
            mvOsPrintf("mvCesaFragParamCheck: cryptoOffset is too large (%d)\n",
                        pCmd->cryptoOffset);
            return MV_BAD_PARAM;
        }

        /* cryptoOffset+cryptoSize must be more than mbufSize - SRAM buffer size */
        if( ((pCmd->cryptoOffset + pCmd->cryptoLength) > pCmd->pSrc->mbufSize) ||
            ((pCmd->pSrc->mbufSize - (pCmd->cryptoOffset + pCmd->cryptoLength)) >=
             (sizeof(cesaSramVirtPtr->buf) - pSA->cryptoBlockSize)) )
        {
            mvOsPrintf("mvCesaFragParamCheck: cryptoLength is too large (%d), mbufSize=%d\n",
                        pCmd->cryptoLength, pCmd->pSrc->mbufSize);
            return MV_BAD_PARAM;
        }
    }

    /* When MAC_THEN_CRYPTO or CRYPTO_THEN_MAC */
    if( ((pSA->config & MV_CESA_OPERATION_MASK) ==
            (MV_CESA_MAC_THEN_CRYPTO << MV_CESA_OPERATION_OFFSET)) ||
        ((pSA->config & MV_CESA_OPERATION_MASK) ==
            (MV_CESA_CRYPTO_THEN_MAC << MV_CESA_OPERATION_OFFSET)) )
    {
        if( (mvCtrlModelGet() == MV_5182_DEV_ID) ||
            ( (mvCtrlModelGet() == MV_5181_DEV_ID) &&
              (mvCtrlRevGet() >= MV_5181L_A0_REV)  &&
              (pCmd->macLength >= (1 << 14)) ) )
        {
            return MV_NOT_ALLOWED;
        }

        /* abs(cryptoOffset-macOffset) must be aligned cryptoBlockSize */
        if(pCmd->cryptoOffset > pCmd->macOffset)
        {
            offset = pCmd->cryptoOffset - pCmd->macOffset;
        }
        else
        {
            offset = pCmd->macOffset - pCmd->cryptoOffset;
        }

        if( MV_IS_NOT_ALIGN(offset,  pSA->cryptoBlockSize) )
        {
/*
            mvOsPrintf("mvCesaFragParamCheck: (cryptoOffset - macOffset) must be %d byte aligned\n",
                        pSA->cryptoBlockSize);
*/
            return MV_NOT_ALLOWED;
        }
        /* Digest must not be part of CryptoLength */
        if( ((pCmd->digestOffset + pSA->digestSize) > pCmd->cryptoOffset) &&
            (pCmd->digestOffset < (pCmd->cryptoOffset + pCmd->cryptoLength)) )
        {
/*
            mvOsPrintf("mvCesaFragParamCheck: digestOffset (%d) is part of cryptoLength (%d+%d)\n",
                        pCmd->digestOffset, pCmd->cryptoOffset, pCmd->cryptoLength);
*/
            return MV_NOT_ALLOWED;
        }
    }
    return MV_OK;
}

/*******************************************************************************
* mvCesaFragSizeFind -
*
* DESCRIPTION:
*
*
* INPUT:
*       MV_CESA_SA* pSA, MV_CESA_COMMAND *pCmd,
*       int cryptoOffset, int macOffset,
*
* OUTPUT:
*       int* pCopySize, int* pCryptoDataSize, int* pMacDataSize
*
* RETURN:
*       MV_STATUS
*
*******************************************************************************/
static void   mvCesaFragSizeFind(MV_CESA_SA* pSA, MV_CESA_REQ* pReq,
                                 int cryptoOffset, int macOffset,
                          int* pCopySize, int* pCryptoDataSize, int* pMacDataSize)
{
    MV_CESA_COMMAND *pCmd = pReq->pCmd;
    int             cryptoDataSize, macDataSize, copySize;

    cryptoDataSize = macDataSize = 0;
    copySize = *pCopySize;

    if( (pSA->config & MV_CESA_OPERATION_MASK) !=
                (MV_CESA_MAC_ONLY << MV_CESA_OPERATION_OFFSET) )
    {
        cryptoDataSize = MV_MIN( (copySize - cryptoOffset),
                                 (pCmd->cryptoLength - (pReq->frags.cryptoSize + 1)) );

        /* cryptoSize for each fragment must be the whole number of blocksSize */
        if( MV_IS_NOT_ALIGN(cryptoDataSize, pSA->cryptoBlockSize) )
        {
            cryptoDataSize = MV_ALIGN_DOWN(cryptoDataSize, pSA->cryptoBlockSize);
            copySize = cryptoOffset + cryptoDataSize;
        }
    }
    if( (pSA->config & MV_CESA_OPERATION_MASK) !=
             (MV_CESA_CRYPTO_ONLY << MV_CESA_OPERATION_OFFSET) )
    {
        macDataSize = MV_MIN( (copySize - macOffset),
                              (pCmd->macLength - (pReq->frags.macSize + 1)));

        /* macSize for each fragment (except last) must be the whole number of blocksSize */
        if( MV_IS_NOT_ALIGN(macDataSize, MV_CESA_AUTH_BLOCK_SIZE) )
        {
            macDataSize = MV_ALIGN_DOWN(macDataSize, MV_CESA_AUTH_BLOCK_SIZE);
            copySize = macOffset + macDataSize;
        }
        cryptoDataSize = copySize - cryptoOffset;
    }
    *pCopySize = copySize;

    if(pCryptoDataSize != NULL)
        *pCryptoDataSize = cryptoDataSize;

    if(pMacDataSize != NULL)
        *pMacDataSize = macDataSize;
}

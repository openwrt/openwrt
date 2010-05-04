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

/*******************************************************************************
* mvEth.h - Header File for : Marvell Gigabit Ethernet Controller
*
* DESCRIPTION:
*       This header file contains macros typedefs and function declaration specific to 
*       the Marvell Gigabit Ethernet Controller.
*
* DEPENDENCIES:
*       None.
*
*******************************************************************************/

#ifndef __mvEthGbe_h__
#define __mvEthGbe_h__

extern MV_BOOL         ethDescInSram;
extern MV_BOOL         ethDescSwCoher;
extern ETH_PORT_CTRL*  ethPortCtrl[];

static INLINE MV_ULONG  ethDescVirtToPhy(ETH_QUEUE_CTRL* pQueueCtrl, MV_U8* pDesc)
{
#if defined (ETH_DESCR_IN_SRAM)
    if( ethDescInSram )
        return mvSramVirtToPhy(pDesc);
    else
#endif /* ETH_DESCR_IN_SRAM */
        return (pQueueCtrl->descBuf.bufPhysAddr + (pDesc - pQueueCtrl->descBuf.bufVirtPtr));
}
/* Return port handler */
#define mvEthPortHndlGet(port)  ethPortCtrl[port]

/* Used as WA for HW/SW race on TX */
static INLINE int      mvEthPortTxEnable(void* pPortHndl, int queue, int max_deep)
{
    int                 deep = 0;
    MV_U32              txCurrReg, txEnReg;
    ETH_TX_DESC*        pTxLastDesc;
    ETH_QUEUE_CTRL*     pQueueCtrl;
    ETH_PORT_CTRL*      pPortCtrl = (ETH_PORT_CTRL*)pPortHndl;

    txEnReg = MV_REG_VALUE(ETH_TX_QUEUE_COMMAND_REG(pPortCtrl->portNo));
    if( (txEnReg & MV_32BIT_LE_FAST(ETH_TXQ_ENABLE_MASK)) == 0)
    {
        MV_REG_VALUE(ETH_TX_QUEUE_COMMAND_REG(pPortCtrl->portNo)) = pPortCtrl->portTxQueueCmdReg;
        return 0;
    }

    pQueueCtrl = &pPortCtrl->txQueue[queue];
    pTxLastDesc = pQueueCtrl->pCurrentDescr;
    txCurrReg = MV_REG_READ(ETH_TX_CUR_DESC_PTR_REG(pPortCtrl->portNo, queue));
    if(ethDescVirtToPhy(pQueueCtrl, (MV_U8*)pTxLastDesc) == txCurrReg)
    {
        /* All descriptors are processed, no chance for race */
        return 0;
    }

    /* Check distance betwee HW and SW location: */
    /* If distance between HW and SW pointers is less than max_deep descriptors */
    /* Race condition is possible, so wait end of TX and restart TXQ */
    while(deep < max_deep)
    {
        pTxLastDesc = TX_PREV_DESC_PTR(pTxLastDesc, pQueueCtrl);
        if(ethDescVirtToPhy(pQueueCtrl, (MV_U8*)pTxLastDesc) == txCurrReg)
        {
            int count = 0;

            while( (txEnReg & MV_32BIT_LE_FAST(ETH_TXQ_ENABLE_MASK)) != 0)
            {   
                count++;
                if(count > 10000)
                {
                    mvOsPrintf("mvEthPortTxEnable: timeout - TXQ_CMD=0x%08x\n", 
                                MV_REG_READ(ETH_TX_QUEUE_COMMAND_REG(pPortCtrl->portNo)) );
                    break;
                }
                txEnReg = MV_REG_VALUE(ETH_TX_QUEUE_COMMAND_REG(pPortCtrl->portNo));
            }

            MV_REG_VALUE(ETH_TX_QUEUE_COMMAND_REG(pPortCtrl->portNo)) = pPortCtrl->portTxQueueCmdReg;
            return count;
        }
        deep++;
    }
    /* Distance between HW and SW pointers is more than max_deep descriptors, */
    /* So NO race condition - do nothing */
    return -1;
}


/* defines  */
#define ETH_CSUM_MIN_BYTE_COUNT     72

/* Tailgate and Kirwood have only 2K TX FIFO */
#if (MV_ETH_VERSION == 2) || (MV_ETH_VERSION == 4)
#define ETH_CSUM_MAX_BYTE_COUNT     1600
#else
#define ETH_CSUM_MAX_BYTE_COUNT     9*1024
#endif /* MV_ETH_VERSION */

#define ETH_MV_HEADER_SIZE	    2
#define ETH_MV_TX_EN

/* An offest in Tx descriptors to store data for buffers less than 8 Bytes */
#define MIN_TX_BUFF_LOAD            8
#define TX_BUF_OFFSET_IN_DESC       (ETH_TX_DESC_ALIGNED_SIZE - MIN_TX_BUFF_LOAD)

/* Default port configuration value */
#define PORT_CONFIG_VALUE                       \
             ETH_DEF_RX_QUEUE_MASK(0)       |   \
             ETH_DEF_RX_ARP_QUEUE_MASK(0)   |   \
             ETH_DEF_RX_TCP_QUEUE_MASK(0)   |   \
             ETH_DEF_RX_UDP_QUEUE_MASK(0)   |   \
             ETH_DEF_RX_BPDU_QUEUE_MASK(0)  |   \
             ETH_RX_CHECKSUM_WITH_PSEUDO_HDR

/* Default port extend configuration value */
#define PORT_CONFIG_EXTEND_VALUE            0

#define PORT_SERIAL_CONTROL_VALUE                           \
            ETH_DISABLE_FC_AUTO_NEG_MASK                |   \
            BIT9                                        |   \
            ETH_DO_NOT_FORCE_LINK_FAIL_MASK             |   \
            ETH_MAX_RX_PACKET_1552BYTE                  |   \
            ETH_SET_FULL_DUPLEX_MASK

#define PORT_SERIAL_CONTROL_100MB_FORCE_VALUE               \
            ETH_FORCE_LINK_PASS_MASK                    |   \
            ETH_DISABLE_DUPLEX_AUTO_NEG_MASK            |   \
            ETH_DISABLE_FC_AUTO_NEG_MASK                |   \
            BIT9                                        |   \
            ETH_DO_NOT_FORCE_LINK_FAIL_MASK             |   \
            ETH_DISABLE_SPEED_AUTO_NEG_MASK             |   \
            ETH_SET_FULL_DUPLEX_MASK                    |   \
            ETH_SET_MII_SPEED_100_MASK                  |   \
            ETH_MAX_RX_PACKET_1552BYTE


#define PORT_SERIAL_CONTROL_1000MB_FORCE_VALUE              \
            ETH_FORCE_LINK_PASS_MASK                    |   \
            ETH_DISABLE_DUPLEX_AUTO_NEG_MASK            |   \
            ETH_DISABLE_FC_AUTO_NEG_MASK                |   \
            BIT9                                        |   \
            ETH_DO_NOT_FORCE_LINK_FAIL_MASK             |   \
            ETH_DISABLE_SPEED_AUTO_NEG_MASK             |   \
            ETH_SET_FULL_DUPLEX_MASK                    |   \
            ETH_SET_GMII_SPEED_1000_MASK                |   \
            ETH_MAX_RX_PACKET_1552BYTE

#define PORT_SERIAL_CONTROL_SGMII_IBAN_VALUE                \
            ETH_DISABLE_FC_AUTO_NEG_MASK                |   \
            BIT9                                        |   \
            ETH_IN_BAND_AN_EN_MASK                      |   \
            ETH_DO_NOT_FORCE_LINK_FAIL_MASK             |   \
            ETH_MAX_RX_PACKET_1552BYTE                  

/* Function headers: */
MV_VOID     mvEthSetSpecialMcastTable(int portNo, int queue);
MV_STATUS   mvEthArpRxQueue(void* pPortHandle, int arpQueue);
MV_STATUS   mvEthUdpRxQueue(void* pPortHandle, int udpQueue);
MV_STATUS   mvEthTcpRxQueue(void* pPortHandle, int tcpQueue);
MV_STATUS   mvEthMacAddrGet(int portNo, unsigned char *pAddr);
MV_VOID     mvEthSetOtherMcastTable(int portNo, int queue);
MV_STATUS   mvEthHeaderModeSet(void* pPortHandle, MV_ETH_HEADER_MODE headerMode);
/* Interrupt Coalesting functions */
MV_U32      mvEthRxCoalSet(void* pPortHndl, MV_U32 uSec);
MV_U32      mvEthTxCoalSet(void* pPortHndl, MV_U32 uSec);
MV_STATUS   mvEthCoalGet(void* pPortHndl, MV_U32* pRxCoal, MV_U32* pTxCoal);

/******************************************************************************/
/*                          Data Flow functions                               */
/******************************************************************************/
static INLINE void      mvEthPortTxRestart(void* pPortHndl)
{
    ETH_PORT_CTRL*      pPortCtrl = (ETH_PORT_CTRL*)pPortHndl;

    MV_REG_VALUE(ETH_TX_QUEUE_COMMAND_REG(pPortCtrl->portNo)) = pPortCtrl->portTxQueueCmdReg;
}

/* Get number of Free resources in specific TX queue */
static INLINE int     mvEthTxResourceGet(void* pPortHndl, int txQueue)
{
    ETH_PORT_CTRL*  pPortCtrl = (ETH_PORT_CTRL*)pPortHndl;

    return (pPortCtrl->txQueue[txQueue].resource);      
}

/* Get number of Free resources in specific RX queue */
static INLINE int     mvEthRxResourceGet(void* pPortHndl, int rxQueue)
{
    ETH_PORT_CTRL*  pPortCtrl = (ETH_PORT_CTRL*)pPortHndl;

    return (pPortCtrl->rxQueue[rxQueue].resource);      
}

static INLINE int     mvEthTxQueueIsFull(void* pPortHndl, int txQueue)
{
    ETH_PORT_CTRL*  pPortCtrl = (ETH_PORT_CTRL*)pPortHndl;

    if(pPortCtrl->txQueue[txQueue].resource == 0)
        return MV_TRUE;

    return MV_FALSE;
}

/* Get number of Free resources in specific RX queue */
static INLINE int     mvEthRxQueueIsFull(void* pPortHndl, int rxQueue)
{
    ETH_PORT_CTRL*  pPortCtrl = (ETH_PORT_CTRL*)pPortHndl;
    ETH_QUEUE_CTRL* pQueueCtrl = &pPortCtrl->rxQueue[rxQueue];

    if( (pQueueCtrl->pUsedDescr == pQueueCtrl->pCurrentDescr) && 
        (pQueueCtrl->resource != 0) )
        return MV_TRUE;

    return MV_FALSE;
}

static INLINE int     mvEthTxQueueIsEmpty(void* pPortHndl, int txQueue)
{
    ETH_PORT_CTRL*  pPortCtrl = (ETH_PORT_CTRL*)pPortHndl;
    ETH_QUEUE_CTRL* pQueueCtrl = &pPortCtrl->txQueue[txQueue];

    if( (pQueueCtrl->pUsedDescr == pQueueCtrl->pCurrentDescr) &&
        (pQueueCtrl->resource != 0) )
    {
        return MV_TRUE;
    }
    return MV_FALSE;
}

/* Get number of Free resources in specific RX queue */
static INLINE int     mvEthRxQueueIsEmpty(void* pPortHndl, int rxQueue)
{
    ETH_PORT_CTRL*  pPortCtrl = (ETH_PORT_CTRL*)pPortHndl;

    if(pPortCtrl->rxQueue[rxQueue].resource == 0)
        return MV_TRUE;

    return MV_FALSE;
}

/*******************************************************************************
* mvEthPortTx - Send an Ethernet packet
*
* DESCRIPTION:
*       This routine send a given packet described by pPktInfo parameter. 
*       Single buffer only.
*
* INPUT:
*       void*       pEthPortHndl  - Ethernet Port handler.
*       int         txQueue       - Number of Tx queue.
*       MV_PKT_INFO *pPktInfo     - User packet to send.
*
* RETURN:
*       MV_NO_RESOURCE  - No enough resources to send this packet.
*       MV_ERROR        - Unexpected Fatal error.
*       MV_OK           - Packet send successfully.
*
*******************************************************************************/
static INLINE MV_STATUS   mvEthPortTx(void* pEthPortHndl, int txQueue, MV_PKT_INFO* pPktInfo)
{
    ETH_TX_DESC*    pTxCurrDesc;
    ETH_PORT_CTRL*  pPortCtrl = (ETH_PORT_CTRL*)pEthPortHndl;
    ETH_QUEUE_CTRL* pQueueCtrl;
    int             portNo;
    MV_BUF_INFO*    pBufInfo = pPktInfo->pFrags;

#ifdef ETH_DEBUG
    if(pPortCtrl->portState != MV_ACTIVE)
        return MV_BAD_STATE;
#endif /* ETH_DEBUG */

    portNo = pPortCtrl->portNo;
    pQueueCtrl = &pPortCtrl->txQueue[txQueue];

    /* Get the Tx Desc ring indexes */
    pTxCurrDesc = pQueueCtrl->pCurrentDescr;

    /* Check if there is enough resources to send the packet */
    if(pQueueCtrl->resource == 0)
        return MV_NO_RESOURCE;

    pTxCurrDesc->byteCnt = pBufInfo->dataSize;

    /* Flash Buffer */
    if(pPktInfo->pktSize != 0)
    {
#ifdef MV_NETBSD
        pTxCurrDesc->bufPtr = pBufInfo->bufPhysAddr;
        ETH_PACKET_CACHE_FLUSH(pBufInfo->bufVirtPtr, pPktInfo->pktSize);
#else
        pTxCurrDesc->bufPtr = ETH_PACKET_CACHE_FLUSH(pBufInfo->bufVirtPtr, pPktInfo->pktSize);
#endif
        pPktInfo->pktSize = 0;
    }
    else
        pTxCurrDesc->bufPtr = pBufInfo->bufPhysAddr;

    pTxCurrDesc->returnInfo = (MV_ULONG)pPktInfo;

    /* There is only one buffer in the packet */
    /* The OSG might set some bits for checksum offload, so add them to first descriptor */
    pTxCurrDesc->cmdSts = pPktInfo->status              |
                          ETH_BUFFER_OWNED_BY_DMA       |
                          ETH_TX_GENERATE_CRC_MASK      |
                          ETH_TX_ENABLE_INTERRUPT_MASK  |
                          ETH_TX_ZERO_PADDING_MASK      |
                          ETH_TX_FIRST_DESC_MASK        |
                          ETH_TX_LAST_DESC_MASK;

    ETH_DESCR_FLUSH_INV(pPortCtrl, pTxCurrDesc);

    pQueueCtrl->resource--;
    pQueueCtrl->pCurrentDescr = TX_NEXT_DESC_PTR(pTxCurrDesc, pQueueCtrl);

    /* Apply send command */
    MV_REG_VALUE(ETH_TX_QUEUE_COMMAND_REG(portNo)) = pPortCtrl->portTxQueueCmdReg;

    return MV_OK;
}


/*******************************************************************************
* mvEthPortSgTx - Send an Ethernet packet
*
* DESCRIPTION:
*       This routine send a given packet described by pBufInfo parameter. It
*       supports transmitting of a packet spaned over multiple buffers. 
*
* INPUT:
*       void*       pEthPortHndl  - Ethernet Port handler.
*       int         txQueue       - Number of Tx queue.
*       MV_PKT_INFO *pPktInfo     - User packet to send.
*
* RETURN:
*       MV_NO_RESOURCE  - No enough resources to send this packet.
*       MV_ERROR        - Unexpected Fatal error.
*       MV_OK           - Packet send successfully.
*
*******************************************************************************/
static INLINE MV_STATUS   mvEthPortSgTx(void* pEthPortHndl, int txQueue, MV_PKT_INFO* pPktInfo)
{
    ETH_TX_DESC*    pTxFirstDesc;
    ETH_TX_DESC*    pTxCurrDesc;
    ETH_PORT_CTRL*  pPortCtrl = (ETH_PORT_CTRL*)pEthPortHndl;
    ETH_QUEUE_CTRL* pQueueCtrl;
    int             portNo, bufCount;
    MV_BUF_INFO*    pBufInfo = pPktInfo->pFrags;
    MV_U8*          pTxBuf;

#ifdef ETH_DEBUG
    if(pPortCtrl->portState != MV_ACTIVE)
        return MV_BAD_STATE;
#endif /* ETH_DEBUG */

    portNo = pPortCtrl->portNo;
    pQueueCtrl = &pPortCtrl->txQueue[txQueue];

    /* Get the Tx Desc ring indexes */
    pTxCurrDesc = pQueueCtrl->pCurrentDescr;

    /* Check if there is enough resources to send the packet */
    if(pQueueCtrl->resource < pPktInfo->numFrags)
        return MV_NO_RESOURCE;

    /* Remember first desc */
    pTxFirstDesc  = pTxCurrDesc;

    bufCount = 0;
    while(MV_TRUE)
    {   
        if(pBufInfo[bufCount].dataSize <= MIN_TX_BUFF_LOAD)
        {
            /* Buffers with a payload smaller than MIN_TX_BUFF_LOAD (8 bytes) must be aligned    */
            /* to 64-bit boundary. Two options here:                                             */
            /* 1) Usually, copy the payload to the reserved 8 bytes inside descriptor.           */
            /* 2) In the Half duplex workaround, the reserved 8 bytes inside descriptor are used */ 
            /*    as a pointer to the aligned buffer, copy the small payload to this buffer.     */
            pTxBuf = ((MV_U8*)pTxCurrDesc)+TX_BUF_OFFSET_IN_DESC;
            mvOsBCopy(pBufInfo[bufCount].bufVirtPtr, pTxBuf, pBufInfo[bufCount].dataSize);
            pTxCurrDesc->bufPtr = ethDescVirtToPhy(pQueueCtrl, pTxBuf);
        }
        else
        {
            /* Flash Buffer */
#ifdef MV_NETBSD
            pTxCurrDesc->bufPtr = pBufInfo[bufCount].bufPhysAddr;
	    ETH_PACKET_CACHE_FLUSH(pBufInfo[bufCount].bufVirtPtr, pBufInfo[bufCount].dataSize);
#else
            pTxCurrDesc->bufPtr = ETH_PACKET_CACHE_FLUSH(pBufInfo[bufCount].bufVirtPtr, pBufInfo[bufCount].dataSize);
#endif
        }

        pTxCurrDesc->byteCnt = pBufInfo[bufCount].dataSize;
        bufCount++;

        if(bufCount >= pPktInfo->numFrags)
            break;

        if(bufCount > 1)
        {
            /* There is middle buffer of the packet Not First and Not Last */
            pTxCurrDesc->cmdSts = ETH_BUFFER_OWNED_BY_DMA;
            ETH_DESCR_FLUSH_INV(pPortCtrl, pTxCurrDesc);
        }
        /* Go to next descriptor and next buffer */
        pTxCurrDesc = TX_NEXT_DESC_PTR(pTxCurrDesc, pQueueCtrl);
    }
    /* Set last desc with DMA ownership and interrupt enable. */
    pTxCurrDesc->returnInfo = (MV_ULONG)pPktInfo;
    if(bufCount == 1) 
    {
        /* There is only one buffer in the packet */
        /* The OSG might set some bits for checksum offload, so add them to first descriptor */
        pTxCurrDesc->cmdSts = pPktInfo->status              |
                              ETH_BUFFER_OWNED_BY_DMA       |
                              ETH_TX_GENERATE_CRC_MASK      |
                              ETH_TX_ENABLE_INTERRUPT_MASK  |
                              ETH_TX_ZERO_PADDING_MASK      |
                              ETH_TX_FIRST_DESC_MASK        |
                              ETH_TX_LAST_DESC_MASK;

        ETH_DESCR_FLUSH_INV(pPortCtrl, pTxCurrDesc);
    }
    else
    {
        /* Last but not First */
        pTxCurrDesc->cmdSts = ETH_BUFFER_OWNED_BY_DMA       |
                              ETH_TX_ENABLE_INTERRUPT_MASK  |
                              ETH_TX_ZERO_PADDING_MASK      |
                              ETH_TX_LAST_DESC_MASK;

        ETH_DESCR_FLUSH_INV(pPortCtrl, pTxCurrDesc);

        /* Update First when more than one buffer in the packet */
        /* The OSG might set some bits for checksum offload, so add them to first descriptor */
        pTxFirstDesc->cmdSts = pPktInfo->status             |
                               ETH_BUFFER_OWNED_BY_DMA      |
                               ETH_TX_GENERATE_CRC_MASK     |
                               ETH_TX_FIRST_DESC_MASK;

        ETH_DESCR_FLUSH_INV(pPortCtrl, pTxFirstDesc);
    }
    /* Update txQueue state */
    pQueueCtrl->resource -= bufCount;
    pQueueCtrl->pCurrentDescr = TX_NEXT_DESC_PTR(pTxCurrDesc, pQueueCtrl);

    /* Apply send command */
    MV_REG_VALUE(ETH_TX_QUEUE_COMMAND_REG(portNo)) = pPortCtrl->portTxQueueCmdReg;

    return MV_OK;
}

/*******************************************************************************
* mvEthPortTxDone - Free all used Tx descriptors and mBlks.
*
* DESCRIPTION:
*       This routine returns the transmitted packet information to the caller.
*
* INPUT:
*       void*       pEthPortHndl    - Ethernet Port handler.
*       int         txQueue         - Number of Tx queue.
*
* OUTPUT:
*       MV_PKT_INFO *pPktInfo       - Pointer to packet was sent.
*
* RETURN:
*       MV_NOT_FOUND    - No transmitted packets to return. Transmit in progress.
*       MV_EMPTY        - No transmitted packets to return. TX Queue is empty.
*       MV_ERROR        - Unexpected Fatal error.
*       MV_OK           - There is transmitted packet in the queue, 
*                       'pPktInfo' filled with relevant information.
*
*******************************************************************************/
static INLINE MV_PKT_INFO*    mvEthPortTxDone(void* pEthPortHndl, int txQueue)
{
    ETH_TX_DESC*    pTxCurrDesc;
    ETH_TX_DESC*    pTxUsedDesc;
    ETH_QUEUE_CTRL* pQueueCtrl;
    ETH_PORT_CTRL*  pPortCtrl = (ETH_PORT_CTRL*)pEthPortHndl;
    MV_PKT_INFO*    pPktInfo;
    MV_U32          commandStatus;

    pQueueCtrl = &pPortCtrl->txQueue[txQueue];

    pTxUsedDesc = pQueueCtrl->pUsedDescr;
    pTxCurrDesc = pQueueCtrl->pCurrentDescr;

    while(MV_TRUE)
    {
        /* No more used descriptors */
        commandStatus = pTxUsedDesc->cmdSts;
        if (commandStatus  & (ETH_BUFFER_OWNED_BY_DMA))
        {    
            ETH_DESCR_INV(pPortCtrl, pTxUsedDesc);
            return NULL;
        }
        if( (pTxUsedDesc == pTxCurrDesc) &&
            (pQueueCtrl->resource != 0) )
        {
            return NULL;
        }
        pQueueCtrl->resource++;
        pQueueCtrl->pUsedDescr = TX_NEXT_DESC_PTR(pTxUsedDesc, pQueueCtrl);
        if(commandStatus & (ETH_TX_LAST_DESC_MASK)) 
        {
            pPktInfo = (MV_PKT_INFO*)pTxUsedDesc->returnInfo;
            pPktInfo->status  = commandStatus;            
            return pPktInfo;
        }
        pTxUsedDesc = pQueueCtrl->pUsedDescr;
    }
}

/*******************************************************************************
* mvEthPortRx - Get new received packets from Rx queue.
*
* DESCRIPTION:
*       This routine returns the received data to the caller. There is no
*       data copying during routine operation. All information is returned
*       using pointer to packet information struct passed from the caller.
*
* INPUT:
*       void*       pEthPortHndl    - Ethernet Port handler.
*       int         rxQueue         - Number of Rx queue.
*
* OUTPUT:
*       MV_PKT_INFO *pPktInfo       - Pointer to received packet.
*
* RETURN:
*       MV_NO_RESOURCE  - No free resources in RX queue.
*       MV_ERROR        - Unexpected Fatal error.
*       MV_OK           - New packet received and 'pBufInfo' structure filled
*                       with relevant information.
*
*******************************************************************************/
static INLINE MV_PKT_INFO*    mvEthPortRx(void* pEthPortHndl, int rxQueue)
{
    ETH_RX_DESC     *pRxCurrDesc;
    MV_U32          commandStatus;
    ETH_PORT_CTRL*  pPortCtrl = (ETH_PORT_CTRL*)pEthPortHndl;
    ETH_QUEUE_CTRL* pQueueCtrl;
    MV_PKT_INFO*    pPktInfo;

    pQueueCtrl = &(pPortCtrl->rxQueue[rxQueue]);

    /* Check resources */
    if(pQueueCtrl->resource == 0)
    {
        mvOsPrintf("ethPortRx: no more resources\n");
        return NULL;
    }
    while(MV_TRUE)
    {
        /* Get the Rx Desc ring 'curr and 'used' indexes */
        pRxCurrDesc = pQueueCtrl->pCurrentDescr;

	commandStatus   = pRxCurrDesc->cmdSts;
        if (commandStatus & (ETH_BUFFER_OWNED_BY_DMA))
        {
            /* Nothing to receive... */
            ETH_DESCR_INV(pPortCtrl, pRxCurrDesc);
            return NULL;
        }

        /* Valid RX only if FIRST and LAST bits are set */
        if( (commandStatus & (ETH_RX_LAST_DESC_MASK | ETH_RX_FIRST_DESC_MASK)) == 
                             (ETH_RX_LAST_DESC_MASK | ETH_RX_FIRST_DESC_MASK) )
        {
            pPktInfo = (MV_PKT_INFO*)pRxCurrDesc->returnInfo;
            pPktInfo->pFrags->dataSize  = pRxCurrDesc->byteCnt - 4;
            pPktInfo->status            = commandStatus;
            pPktInfo->fragIP            = pRxCurrDesc->bufSize & ETH_RX_IP_FRAGMENTED_FRAME_MASK;

            pQueueCtrl->resource--;
            /* Update 'curr' in data structure */
            pQueueCtrl->pCurrentDescr = RX_NEXT_DESC_PTR(pRxCurrDesc, pQueueCtrl);

#ifdef INCLUDE_SYNC_BARR
            mvCpuIfSyncBarr(DRAM_TARGET);
#endif
            return pPktInfo;
        }
        else
        {
            ETH_RX_DESC*    pRxUsedDesc = pQueueCtrl->pUsedDescr;

#ifdef ETH_DEBUG
            mvOsPrintf("ethDrv: Unexpected Jumbo frame: "
                       "status=0x%08x, byteCnt=%d, pData=0x%x\n", 
                        commandStatus, pRxCurrDesc->byteCnt, pRxCurrDesc->bufPtr);
#endif /* ETH_DEBUG */

            /* move buffer from pCurrentDescr position to pUsedDescr position */
            pRxUsedDesc->bufPtr     = pRxCurrDesc->bufPtr;
            pRxUsedDesc->returnInfo = pRxCurrDesc->returnInfo;
            pRxUsedDesc->bufSize    = pRxCurrDesc->bufSize & ETH_RX_BUFFER_MASK;

            /* Return the descriptor to DMA ownership */
            pRxUsedDesc->cmdSts = ETH_BUFFER_OWNED_BY_DMA | 
                                  ETH_RX_ENABLE_INTERRUPT_MASK;

            /* Flush descriptor and CPU pipe */
            ETH_DESCR_FLUSH_INV(pPortCtrl, pRxUsedDesc);

            /* Move the used descriptor pointer to the next descriptor */
            pQueueCtrl->pUsedDescr = RX_NEXT_DESC_PTR(pRxUsedDesc, pQueueCtrl);
            pQueueCtrl->pCurrentDescr = RX_NEXT_DESC_PTR(pRxCurrDesc, pQueueCtrl);            
        }
    }
}

/*******************************************************************************
* mvEthPortRxDone - Returns a Rx buffer back to the Rx ring.
*
* DESCRIPTION:
*       This routine returns a Rx buffer back to the Rx ring. 
*
* INPUT:
*       void*       pEthPortHndl    - Ethernet Port handler.
*       int         rxQueue         - Number of Rx queue.
*       MV_PKT_INFO *pPktInfo       - Pointer to received packet.
*
* RETURN:
*       MV_ERROR        - Unexpected Fatal error.
*       MV_OUT_OF_RANGE - RX queue is already FULL, so this buffer can't be 
*                       returned to this queue.
*       MV_FULL         - Buffer returned successfully and RX queue became full.
*                       More buffers should not be returned at the time.
*       MV_OK           - Buffer returned successfully and there are more free 
*                       places in the queue.
*
*******************************************************************************/
static INLINE MV_STATUS   mvEthPortRxDone(void* pEthPortHndl, int rxQueue, MV_PKT_INFO *pPktInfo)
{
    ETH_RX_DESC*    pRxUsedDesc;
    ETH_QUEUE_CTRL* pQueueCtrl;
    ETH_PORT_CTRL*  pPortCtrl = (ETH_PORT_CTRL*)pEthPortHndl;
            
    pQueueCtrl = &pPortCtrl->rxQueue[rxQueue];

    /* Get 'used' Rx descriptor */
    pRxUsedDesc = pQueueCtrl->pUsedDescr;

    /* Check that ring is not FULL */
    if( (pQueueCtrl->pUsedDescr == pQueueCtrl->pCurrentDescr) && 
        (pQueueCtrl->resource != 0) )
    {
        mvOsPrintf("%s %d: out of range Error resource=%d, curr=%p, used=%p\n", 
                    __FUNCTION__, pPortCtrl->portNo, pQueueCtrl->resource, 
                    pQueueCtrl->pCurrentDescr, pQueueCtrl->pUsedDescr);
        return MV_OUT_OF_RANGE;
    }

    pRxUsedDesc->bufPtr     = pPktInfo->pFrags->bufPhysAddr;
    pRxUsedDesc->returnInfo = (MV_ULONG)pPktInfo;
    pRxUsedDesc->bufSize    = pPktInfo->pFrags->bufSize & ETH_RX_BUFFER_MASK;

    /* Invalidate data buffer accordingly with pktSize */
    if(pPktInfo->pktSize != 0)
    {
        ETH_PACKET_CACHE_INVALIDATE(pPktInfo->pFrags->bufVirtPtr, pPktInfo->pktSize);
        pPktInfo->pktSize = 0;
    }

    /* Return the descriptor to DMA ownership */
    pRxUsedDesc->cmdSts = ETH_BUFFER_OWNED_BY_DMA | ETH_RX_ENABLE_INTERRUPT_MASK;

    /* Flush descriptor and CPU pipe */
    ETH_DESCR_FLUSH_INV(pPortCtrl, pRxUsedDesc);

    pQueueCtrl->resource++;

    /* Move the used descriptor pointer to the next descriptor */
    pQueueCtrl->pUsedDescr = RX_NEXT_DESC_PTR(pRxUsedDesc, pQueueCtrl);
    
    /* If ring became Full return MV_FULL */
    if(pQueueCtrl->pUsedDescr == pQueueCtrl->pCurrentDescr) 
        return MV_FULL;

    return MV_OK;
}


#endif /* __mvEthGbe_h__ */



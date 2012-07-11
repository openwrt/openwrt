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
* mvEthDebug.c - Source file for user friendly debug functions
*
* DESCRIPTION:
*
* DEPENDENCIES:
*       None.
*
*******************************************************************************/

#include "mvOs.h"
#include "mvCommon.h"
#include "mvTypes.h"
#include "mv802_3.h"
#include "mvDebug.h"
#include "ctrlEnv/mvCtrlEnvLib.h"
#include "eth-phy/mvEthPhy.h"
#include "eth/mvEth.h"
#include "eth/gbe/mvEthDebug.h"

/* #define mvOsPrintf printf */

void    mvEthPortShow(void* pHndl);
void    mvEthQueuesShow(void* pHndl, int rxQueue, int txQueue, int mode);

/******************************************************************************/
/*                          Debug functions                                   */
/******************************************************************************/
void    ethRxCoal(int port, int usec)
{
    void*   pHndl;

    pHndl = mvEthPortHndlGet(port);
    if(pHndl != NULL)
    {
        mvEthRxCoalSet(pHndl, usec);
    }
}

void    ethTxCoal(int port, int usec)
{
    void*   pHndl;

    pHndl = mvEthPortHndlGet(port);
    if(pHndl != NULL)
    {
        mvEthTxCoalSet(pHndl, usec);
    }
}

#if (MV_ETH_VERSION >= 4)
void     ethEjpModeSet(int port, int mode)
{
    void*   pHndl;

    pHndl = mvEthPortHndlGet(port);
    if(pHndl != NULL)
    {
        mvEthEjpModeSet(pHndl, mode);
    }
}
#endif /* (MV_ETH_VERSION >= 4) */

void    ethBpduRxQ(int port, int bpduQueue)
{
    void*   pHndl;

    pHndl = mvEthPortHndlGet(port);
    if(pHndl != NULL)
    {
        mvEthBpduRxQueue(pHndl, bpduQueue);
    }
}

void    ethArpRxQ(int port, int arpQueue)
{
    void*   pHndl;

    pHndl = mvEthPortHndlGet(port);
    if(pHndl != NULL)
    {
        mvEthArpRxQueue(pHndl, arpQueue);
    }
}

void    ethTcpRxQ(int port, int tcpQueue)
{
    void*   pHndl;

    pHndl = mvEthPortHndlGet(port);
    if(pHndl != NULL)
    {
        mvEthTcpRxQueue(pHndl, tcpQueue);
    }
}

void    ethUdpRxQ(int port, int udpQueue)
{
    void*   pHndl;

    pHndl = mvEthPortHndlGet(port);
    if(pHndl != NULL)
    {
        mvEthUdpRxQueue(pHndl, udpQueue);
    }
}

void    ethTxPolicyRegs(int port)
{
    int             queue;
    ETH_PORT_CTRL*  pPortCtrl = (ETH_PORT_CTRL*)mvEthPortHndlGet(port);

    if(pPortCtrl == NULL) 
    {
        return;
    }
    mvOsPrintf("Port #%d TX Policy: EJP=%d, TXQs: ", 
                port, pPortCtrl->portConfig.ejpMode);
    for(queue=0; queue<MV_ETH_TX_Q_NUM; queue++)
    {
        if(pPortCtrl->txQueueConfig[queue].descrNum > 0)
            mvOsPrintf("%d, ", queue);
    }
    mvOsPrintf("\n");

    mvOsPrintf("\n\t TX policy Port #%d configuration registers\n", port);

    mvOsPrintf("ETH_TX_QUEUE_COMMAND_REG            : 0x%X = 0x%08x\n", 
                ETH_TX_QUEUE_COMMAND_REG(port), 
                MV_REG_READ( ETH_TX_QUEUE_COMMAND_REG(port) ) );    
    
    mvOsPrintf("ETH_TX_FIXED_PRIO_CFG_REG           : 0x%X = 0x%08x\n", 
                ETH_TX_FIXED_PRIO_CFG_REG(port), 
                MV_REG_READ( ETH_TX_FIXED_PRIO_CFG_REG(port) ) );

    mvOsPrintf("ETH_TX_TOKEN_RATE_CFG_REG           : 0x%X = 0x%08x\n", 
                ETH_TX_TOKEN_RATE_CFG_REG(port), 
                MV_REG_READ( ETH_TX_TOKEN_RATE_CFG_REG(port) ) );

    mvOsPrintf("ETH_MAX_TRANSMIT_UNIT_REG           : 0x%X = 0x%08x\n", 
                ETH_MAX_TRANSMIT_UNIT_REG(port), 
                MV_REG_READ( ETH_MAX_TRANSMIT_UNIT_REG(port) ) );

    mvOsPrintf("ETH_TX_TOKEN_BUCKET_SIZE_REG        : 0x%X = 0x%08x\n", 
                ETH_TX_TOKEN_BUCKET_SIZE_REG(port), 
                MV_REG_READ( ETH_TX_TOKEN_BUCKET_SIZE_REG(port) ) );

    mvOsPrintf("ETH_TX_TOKEN_BUCKET_COUNT_REG       : 0x%X = 0x%08x\n", 
                ETH_TX_TOKEN_BUCKET_COUNT_REG(port), 
                MV_REG_READ( ETH_TX_TOKEN_BUCKET_COUNT_REG(port) ) );

    for(queue=0; queue<MV_ETH_MAX_TXQ; queue++)
    {
        mvOsPrintf("\n\t TX policy Port #%d, Queue #%d configuration registers\n", port, queue);

        mvOsPrintf("ETH_TXQ_TOKEN_COUNT_REG             : 0x%X = 0x%08x\n", 
                ETH_TXQ_TOKEN_COUNT_REG(port, queue), 
                MV_REG_READ( ETH_TXQ_TOKEN_COUNT_REG(port, queue) ) );

        mvOsPrintf("ETH_TXQ_TOKEN_CFG_REG               : 0x%X = 0x%08x\n", 
                ETH_TXQ_TOKEN_CFG_REG(port, queue), 
                MV_REG_READ( ETH_TXQ_TOKEN_CFG_REG(port, queue) ) );

        mvOsPrintf("ETH_TXQ_ARBITER_CFG_REG             : 0x%X = 0x%08x\n", 
                ETH_TXQ_ARBITER_CFG_REG(port, queue), 
                MV_REG_READ( ETH_TXQ_ARBITER_CFG_REG(port, queue) ) );
    }
    mvOsPrintf("\n");
}

/* Print important registers of Ethernet port */
void    ethPortRegs(int port)
{
    mvOsPrintf("\n\t ethGiga #%d port Registers:\n", port);

    mvOsPrintf("ETH_PORT_STATUS_REG                 : 0x%X = 0x%08x\n", 
                ETH_PORT_STATUS_REG(port), 
                MV_REG_READ( ETH_PORT_STATUS_REG(port) ) );

    mvOsPrintf("ETH_PORT_SERIAL_CTRL_REG            : 0x%X = 0x%08x\n", 
                ETH_PORT_SERIAL_CTRL_REG(port), 
                MV_REG_READ( ETH_PORT_SERIAL_CTRL_REG(port) ) );

    mvOsPrintf("ETH_PORT_CONFIG_REG                 : 0x%X = 0x%08x\n", 
                ETH_PORT_CONFIG_REG(port), 
                MV_REG_READ( ETH_PORT_CONFIG_REG(port) ) );    

    mvOsPrintf("ETH_PORT_CONFIG_EXTEND_REG          : 0x%X = 0x%08x\n", 
                ETH_PORT_CONFIG_EXTEND_REG(port), 
                MV_REG_READ( ETH_PORT_CONFIG_EXTEND_REG(port) ) );    

    mvOsPrintf("ETH_SDMA_CONFIG_REG                 : 0x%X = 0x%08x\n", 
                ETH_SDMA_CONFIG_REG(port), 
                MV_REG_READ( ETH_SDMA_CONFIG_REG(port) ) );    

    mvOsPrintf("ETH_TX_FIFO_URGENT_THRESH_REG       : 0x%X = 0x%08x\n", 
                ETH_TX_FIFO_URGENT_THRESH_REG(port), 
                MV_REG_READ( ETH_TX_FIFO_URGENT_THRESH_REG(port) ) );    

    mvOsPrintf("ETH_RX_QUEUE_COMMAND_REG            : 0x%X = 0x%08x\n", 
                ETH_RX_QUEUE_COMMAND_REG(port), 
                MV_REG_READ( ETH_RX_QUEUE_COMMAND_REG(port) ) );    

    mvOsPrintf("ETH_TX_QUEUE_COMMAND_REG            : 0x%X = 0x%08x\n", 
                ETH_TX_QUEUE_COMMAND_REG(port), 
                MV_REG_READ( ETH_TX_QUEUE_COMMAND_REG(port) ) );    

    mvOsPrintf("ETH_INTR_CAUSE_REG                  : 0x%X = 0x%08x\n", 
                ETH_INTR_CAUSE_REG(port), 
                MV_REG_READ( ETH_INTR_CAUSE_REG(port) ) );    

    mvOsPrintf("ETH_INTR_EXTEND_CAUSE_REG           : 0x%X = 0x%08x\n", 
                ETH_INTR_CAUSE_EXT_REG(port), 
                MV_REG_READ( ETH_INTR_CAUSE_EXT_REG(port) ) );    

    mvOsPrintf("ETH_INTR_MASK_REG                   : 0x%X = 0x%08x\n", 
                ETH_INTR_MASK_REG(port), 
                MV_REG_READ( ETH_INTR_MASK_REG(port) ) );    

    mvOsPrintf("ETH_INTR_EXTEND_MASK_REG            : 0x%X = 0x%08x\n", 
                ETH_INTR_MASK_EXT_REG(port), 
                MV_REG_READ( ETH_INTR_MASK_EXT_REG(port) ) );    

    mvOsPrintf("ETH_RX_DESCR_STAT_CMD_REG           : 0x%X = 0x%08x\n", 
                ETH_RX_DESCR_STAT_CMD_REG(port, 0), 
                MV_REG_READ( ETH_RX_DESCR_STAT_CMD_REG(port, 0) ) );    

    mvOsPrintf("ETH_RX_BYTE_COUNT_REG               : 0x%X = 0x%08x\n", 
                ETH_RX_BYTE_COUNT_REG(port, 0), 
                MV_REG_READ( ETH_RX_BYTE_COUNT_REG(port, 0) ) );    

    mvOsPrintf("ETH_RX_BUF_PTR_REG                  : 0x%X = 0x%08x\n", 
                ETH_RX_BUF_PTR_REG(port, 0), 
                MV_REG_READ( ETH_RX_BUF_PTR_REG(port, 0) ) );    

    mvOsPrintf("ETH_RX_CUR_DESC_PTR_REG             : 0x%X = 0x%08x\n", 
                ETH_RX_CUR_DESC_PTR_REG(port, 0), 
                MV_REG_READ( ETH_RX_CUR_DESC_PTR_REG(port, 0) ) );    
}


/* Print Giga Ethernet UNIT registers */
void    ethRegs(int port)
{
    mvOsPrintf("ETH_PHY_ADDR_REG               : 0x%X = 0x%08x\n", 
                ETH_PHY_ADDR_REG(port), 
                MV_REG_READ(ETH_PHY_ADDR_REG(port)) );    

    mvOsPrintf("ETH_UNIT_INTR_CAUSE_REG        : 0x%X = 0x%08x\n", 
                ETH_UNIT_INTR_CAUSE_REG(port), 
                MV_REG_READ( ETH_UNIT_INTR_CAUSE_REG(port)) );    

    mvOsPrintf("ETH_UNIT_INTR_MASK_REG         : 0x%X = 0x%08x\n", 
                ETH_UNIT_INTR_MASK_REG(port), 
                MV_REG_READ( ETH_UNIT_INTR_MASK_REG(port)) );    

    mvOsPrintf("ETH_UNIT_ERROR_ADDR_REG        : 0x%X = 0x%08x\n", 
                ETH_UNIT_ERROR_ADDR_REG(port), 
                MV_REG_READ(ETH_UNIT_ERROR_ADDR_REG(port)) );    

    mvOsPrintf("ETH_UNIT_INT_ADDR_ERROR_REG    : 0x%X = 0x%08x\n", 
                ETH_UNIT_INT_ADDR_ERROR_REG(port), 
                MV_REG_READ(ETH_UNIT_INT_ADDR_ERROR_REG(port)) );    
    
}

/******************************************************************************/
/*                      MIB Counters functions                                */
/******************************************************************************/

/*******************************************************************************
* ethClearMibCounters - Clear all MIB counters
*
* DESCRIPTION:
*       This function clears all MIB counters of a specific ethernet port.
*       A read from the MIB counter will reset the counter.
*
* INPUT:
*       int    port -  Ethernet Port number.
*
* RETURN: None
*
*******************************************************************************/
void ethClearCounters(int port)
{
    void*   pHndl;

    pHndl = mvEthPortHndlGet(port);
    if(pHndl != NULL)
        mvEthMibCountersClear(pHndl);

    return;
}


/* Print counters of the Ethernet port */
void    ethPortCounters(int port)
{
    MV_U32  regValue, regValHigh;
    void*   pHndl;

    pHndl = mvEthPortHndlGet(port);
    if(pHndl == NULL)
        return;

    mvOsPrintf("\n\t Port #%d MIB Counters\n\n", port);

    mvOsPrintf("GoodFramesReceived          = %u\n", 
              mvEthMibCounterRead(pHndl, ETH_MIB_GOOD_FRAMES_RECEIVED, NULL));
    mvOsPrintf("BadFramesReceived           = %u\n", 
              mvEthMibCounterRead(pHndl, ETH_MIB_BAD_FRAMES_RECEIVED, NULL));
    mvOsPrintf("BroadcastFramesReceived     = %u\n", 
              mvEthMibCounterRead(pHndl, ETH_MIB_BROADCAST_FRAMES_RECEIVED, NULL));
    mvOsPrintf("MulticastFramesReceived     = %u\n", 
              mvEthMibCounterRead(pHndl, ETH_MIB_MULTICAST_FRAMES_RECEIVED, NULL));

    regValue = mvEthMibCounterRead(pHndl, ETH_MIB_GOOD_OCTETS_RECEIVED_LOW, 
                                 &regValHigh);
    mvOsPrintf("GoodOctetsReceived          = 0x%08x%08x\n", 
               regValHigh, regValue);

    mvOsPrintf("\n");
    mvOsPrintf("GoodFramesSent              = %u\n", 
              mvEthMibCounterRead(pHndl, ETH_MIB_GOOD_FRAMES_SENT, NULL));
    mvOsPrintf("BroadcastFramesSent         = %u\n", 
              mvEthMibCounterRead(pHndl, ETH_MIB_BROADCAST_FRAMES_SENT, NULL));
    mvOsPrintf("MulticastFramesSent         = %u\n", 
              mvEthMibCounterRead(pHndl, ETH_MIB_MULTICAST_FRAMES_SENT, NULL));

    regValue = mvEthMibCounterRead(pHndl, ETH_MIB_GOOD_OCTETS_SENT_LOW, 
                                 &regValHigh);
    mvOsPrintf("GoodOctetsSent              = 0x%08x%08x\n", regValHigh, regValue);


    mvOsPrintf("\n\t FC Control Counters\n");

    regValue = mvEthMibCounterRead(pHndl, ETH_MIB_UNREC_MAC_CONTROL_RECEIVED, NULL);
    mvOsPrintf("UnrecogMacControlReceived   = %u\n", regValue);

    regValue = mvEthMibCounterRead(pHndl, ETH_MIB_GOOD_FC_RECEIVED, NULL);
    mvOsPrintf("GoodFCFramesReceived        = %u\n", regValue);

    regValue = mvEthMibCounterRead(pHndl, ETH_MIB_BAD_FC_RECEIVED, NULL);
    mvOsPrintf("BadFCFramesReceived         = %u\n", regValue);

    regValue = mvEthMibCounterRead(pHndl, ETH_MIB_FC_SENT, NULL);
    mvOsPrintf("FCFramesSent                = %u\n", regValue);


    mvOsPrintf("\n\t RX Errors\n");

    regValue = mvEthMibCounterRead(pHndl, ETH_MIB_BAD_OCTETS_RECEIVED, NULL);
    mvOsPrintf("BadOctetsReceived           = %u\n", regValue);

    regValue = mvEthMibCounterRead(pHndl, ETH_MIB_UNDERSIZE_RECEIVED, NULL);
    mvOsPrintf("UndersizeFramesReceived     = %u\n", regValue);

    regValue = mvEthMibCounterRead(pHndl, ETH_MIB_FRAGMENTS_RECEIVED, NULL);
    mvOsPrintf("FragmentsReceived           = %u\n", regValue);

    regValue = mvEthMibCounterRead(pHndl, ETH_MIB_OVERSIZE_RECEIVED, NULL);
    mvOsPrintf("OversizeFramesReceived      = %u\n", regValue);
    
    regValue = mvEthMibCounterRead(pHndl, ETH_MIB_JABBER_RECEIVED, NULL);
    mvOsPrintf("JabbersReceived             = %u\n", regValue);

    regValue = mvEthMibCounterRead(pHndl, ETH_MIB_MAC_RECEIVE_ERROR, NULL);
    mvOsPrintf("MacReceiveErrors            = %u\n", regValue);

    regValue = mvEthMibCounterRead(pHndl, ETH_MIB_BAD_CRC_EVENT, NULL);
    mvOsPrintf("BadCrcReceived              = %u\n", regValue);

    mvOsPrintf("\n\t TX Errors\n");

    regValue = mvEthMibCounterRead(pHndl, ETH_MIB_INTERNAL_MAC_TRANSMIT_ERR, NULL);
    mvOsPrintf("TxMacErrors                 = %u\n", regValue);

    regValue = mvEthMibCounterRead(pHndl, ETH_MIB_EXCESSIVE_COLLISION, NULL);
    mvOsPrintf("TxExcessiveCollisions       = %u\n", regValue);

    regValue = mvEthMibCounterRead(pHndl, ETH_MIB_COLLISION, NULL);
    mvOsPrintf("TxCollisions                = %u\n", regValue);

    regValue = mvEthMibCounterRead(pHndl, ETH_MIB_LATE_COLLISION, NULL);
    mvOsPrintf("TxLateCollisions            = %u\n", regValue);


    mvOsPrintf("\n");
    regValue = MV_REG_READ( ETH_RX_DISCARD_PKTS_CNTR_REG(port));
    mvOsPrintf("Rx Discard packets counter  = %u\n", regValue);

    regValue = MV_REG_READ(ETH_RX_OVERRUN_PKTS_CNTR_REG(port));
    mvOsPrintf("Rx Overrun packets counter  = %u\n", regValue);
}

/* Print RMON counters of the Ethernet port */
void    ethPortRmonCounters(int port)
{
    void*   pHndl;

    pHndl = mvEthPortHndlGet(port);
    if(pHndl == NULL)
        return;

    mvOsPrintf("\n\t Port #%d RMON MIB Counters\n\n", port);

    mvOsPrintf("64 ByteFramesReceived           = %u\n", 
              mvEthMibCounterRead(pHndl, ETH_MIB_FRAMES_64_OCTETS, NULL));
    mvOsPrintf("65...127 ByteFramesReceived     = %u\n", 
              mvEthMibCounterRead(pHndl, ETH_MIB_FRAMES_65_TO_127_OCTETS, NULL));
    mvOsPrintf("128...255 ByteFramesReceived    = %u\n", 
              mvEthMibCounterRead(pHndl, ETH_MIB_FRAMES_128_TO_255_OCTETS, NULL));
    mvOsPrintf("256...511 ByteFramesReceived    = %u\n", 
              mvEthMibCounterRead(pHndl, ETH_MIB_FRAMES_256_TO_511_OCTETS, NULL));
    mvOsPrintf("512...1023 ByteFramesReceived   = %u\n", 
              mvEthMibCounterRead(pHndl, ETH_MIB_FRAMES_512_TO_1023_OCTETS, NULL));
    mvOsPrintf("1024...Max ByteFramesReceived   = %u\n", 
              mvEthMibCounterRead(pHndl, ETH_MIB_FRAMES_1024_TO_MAX_OCTETS, NULL));
}

/* Print port information */
void    ethPortStatus(int port)
{
    void*   pHndl;

    pHndl = mvEthPortHndlGet(port);
    if(pHndl != NULL)
    {
        mvEthPortShow(pHndl);
    }
}

/* Print port queues information */
void    ethPortQueues(int port, int rxQueue, int txQueue, int mode)  
{
    void*   pHndl;

    pHndl = mvEthPortHndlGet(port);
    if(pHndl != NULL)
    {
        mvEthQueuesShow(pHndl, rxQueue, txQueue, mode);
    }
}

void    ethUcastSet(int port, char* macStr, int queue)
{
    void*   pHndl;
    MV_U8   macAddr[MV_MAC_ADDR_SIZE];

    pHndl = mvEthPortHndlGet(port);
    if(pHndl != NULL)
    {
        mvMacStrToHex(macStr, macAddr);
        mvEthMacAddrSet(pHndl, macAddr, queue);
    }
}


void    ethPortUcastShow(int port)
{
    MV_U32  unicastReg, macL, macH;
    int     i, j;

    macL = MV_REG_READ(ETH_MAC_ADDR_LOW_REG(port));
    macH = MV_REG_READ(ETH_MAC_ADDR_HIGH_REG(port));
 
    mvOsPrintf("\n\t Port #%d Unicast MAC table: %02x:%02x:%02x:%02x:%02x:%02x\n\n", 
                port, ((macH >> 24) & 0xff), ((macH >> 16) & 0xff),
                      ((macH >> 8) & 0xff), (macH  & 0xff), 
                      ((macL >> 8) & 0xff), (macL  & 0xff) );

    for (i=0; i<4; i++)
    {
        unicastReg = MV_REG_READ( (ETH_DA_FILTER_UCAST_BASE(port) + i*4));
        for(j=0; j<4; j++)
        {
            MV_U8   macEntry = (unicastReg >> (8*j)) & 0xFF;

            mvOsPrintf("%X: %8s, Q = %d\n", i*4+j, 
                (macEntry & BIT0) ? "Accept" : "Reject", (macEntry >> 1) & 0x7);
        }
    }
} 

void    ethMcastAdd(int port, char* macStr, int queue)
{
    void*   pHndl;
    MV_U8   macAddr[MV_MAC_ADDR_SIZE];

    pHndl = mvEthPortHndlGet(port);
    if(pHndl != NULL)
    {
        mvMacStrToHex(macStr, macAddr);
        mvEthMcastAddrSet(pHndl, macAddr, queue);
    }
}

void    ethPortMcast(int port)
{
    int     tblIdx, regIdx;
    MV_U32  regVal;

    mvOsPrintf("\n\t Port #%d Special (IP) Multicast table: 01:00:5E:00:00:XX\n\n", 
                port);

    for(tblIdx=0; tblIdx<(256/4); tblIdx++)
    {
        regVal = MV_REG_READ((ETH_DA_FILTER_SPEC_MCAST_BASE(port) + tblIdx*4));
        for(regIdx=0; regIdx<4; regIdx++)
        {
            if((regVal & (0x01 << (regIdx*8))) != 0)
            {
                mvOsPrintf("0x%02X: Accepted, rxQ = %d\n", 
                    tblIdx*4+regIdx, ((regVal >> (regIdx*8+1)) & 0x07));
            }
        }
    }
    mvOsPrintf("\n\t Port #%d Other Multicast table\n\n", port);
    for(tblIdx=0; tblIdx<(256/4); tblIdx++)
    {
        regVal = MV_REG_READ((ETH_DA_FILTER_OTH_MCAST_BASE(port) + tblIdx*4));
        for(regIdx=0; regIdx<4; regIdx++)
        {
            if((regVal & (0x01 << (regIdx*8))) != 0)
            {
                mvOsPrintf("Crc8=0x%02X: Accepted, rxQ = %d\n", 
                    tblIdx*4+regIdx, ((regVal >> (regIdx*8+1)) & 0x07));
            }
        }
    }
}


/* Print status of Ethernet port */
void    mvEthPortShow(void* pHndl)
{
    MV_U32              regValue, rxCoal, txCoal;
    int                 speed, queue, port;
    ETH_PORT_CTRL*      pPortCtrl = (ETH_PORT_CTRL*)pHndl;

    port = pPortCtrl->portNo;

    regValue = MV_REG_READ( ETH_PORT_STATUS_REG(port) );

    mvOsPrintf("\n\t ethGiga #%d port Status: 0x%04x = 0x%08x\n\n", 
                port, ETH_PORT_STATUS_REG(port), regValue);

    mvOsPrintf("descInSram=%d, descSwCoher=%d\n", 
                ethDescInSram, ethDescSwCoher);

    if(regValue & ETH_GMII_SPEED_1000_MASK)
        speed = 1000;
    else if(regValue & ETH_MII_SPEED_100_MASK)
        speed = 100;
    else
        speed = 10;

    mvEthCoalGet(pPortCtrl, &rxCoal, &txCoal);

    /* Link, Speed, Duplex, FlowControl */
    mvOsPrintf("Link=%s, Speed=%d, Duplex=%s, RxFlowControl=%s",
                (regValue & ETH_LINK_UP_MASK) ? "UP" : "DOWN",
                speed, 
                (regValue & ETH_FULL_DUPLEX_MASK) ? "FULL" : "HALF",
                (regValue & ETH_ENABLE_RCV_FLOW_CTRL_MASK) ? "ENABLE" : "DISABLE");

    mvOsPrintf("\n");

    mvOsPrintf("RxCoal = %d usec, TxCoal = %d usec\n", 
                rxCoal, txCoal);

    mvOsPrintf("rxDefQ=%d, arpQ=%d, bpduQ=%d, tcpQ=%d, udpQ=%d\n\n",
                pPortCtrl->portConfig.rxDefQ, pPortCtrl->portConfig.rxArpQ, 
                pPortCtrl->portConfig.rxBpduQ, 
                pPortCtrl->portConfig.rxTcpQ, pPortCtrl->portConfig.rxUdpQ); 

    /* Print all RX and TX queues */
    for(queue=0; queue<MV_ETH_RX_Q_NUM; queue++)
    {
        mvOsPrintf("RX Queue #%d: base=0x%lx, free=%d\n", 
                    queue, (MV_ULONG)pPortCtrl->rxQueue[queue].pFirstDescr,
                    mvEthRxResourceGet(pPortCtrl, queue) );
    }
    mvOsPrintf("\n");
    for(queue=0; queue<MV_ETH_TX_Q_NUM; queue++)
    {
        mvOsPrintf("TX Queue #%d: base=0x%lx, free=%d\n", 
                queue, (MV_ULONG)pPortCtrl->txQueue[queue].pFirstDescr,
                mvEthTxResourceGet(pPortCtrl, queue) );
    }
}

/* Print RX and TX queue of the Ethernet port */
void    mvEthQueuesShow(void* pHndl, int rxQueue, int txQueue, int mode)  
{
    ETH_PORT_CTRL   *pPortCtrl = (ETH_PORT_CTRL*)pHndl;
    ETH_QUEUE_CTRL  *pQueueCtrl;
    MV_U32          regValue;
    ETH_RX_DESC     *pRxDescr;
    ETH_TX_DESC     *pTxDescr;
    int             i, port = pPortCtrl->portNo;

    if( (rxQueue >=0) && (rxQueue < MV_ETH_RX_Q_NUM) )
    {
        pQueueCtrl = &(pPortCtrl->rxQueue[rxQueue]);
        mvOsPrintf("Port #%d, RX Queue #%d\n\n", port, rxQueue);

        mvOsPrintf("CURR_RX_DESC_PTR        : 0x%X = 0x%08x\n", 
            ETH_RX_CUR_DESC_PTR_REG(port, rxQueue), 
            MV_REG_READ( ETH_RX_CUR_DESC_PTR_REG(port, rxQueue)));


        if(pQueueCtrl->pFirstDescr != NULL)
        {
            mvOsPrintf("pFirstDescr=0x%lx, pLastDescr=0x%lx, numOfResources=%d\n",
                (MV_ULONG)pQueueCtrl->pFirstDescr, (MV_ULONG)pQueueCtrl->pLastDescr, 
                pQueueCtrl->resource);
            mvOsPrintf("pCurrDescr: 0x%lx, pUsedDescr: 0x%lx\n",
                (MV_ULONG)pQueueCtrl->pCurrentDescr, 
                (MV_ULONG)pQueueCtrl->pUsedDescr);

            if(mode == 1)
            {
                pRxDescr = (ETH_RX_DESC*)pQueueCtrl->pFirstDescr;
                i = 0; 
                do 
                {
                    mvOsPrintf("%3d. desc=%08x (%08x), cmd=%08x, data=%4d, buf=%4d, buf=%08x, pkt=%lx, os=%lx\n", 
                                i, (MV_U32)pRxDescr, (MV_U32)ethDescVirtToPhy(pQueueCtrl, (MV_U8*)pRxDescr), 
                                pRxDescr->cmdSts, pRxDescr->byteCnt, (MV_U32)pRxDescr->bufSize, 
                                (unsigned int)pRxDescr->bufPtr, (MV_ULONG)pRxDescr->returnInfo,
                                ((MV_PKT_INFO*)pRxDescr->returnInfo)->osInfo);

                    ETH_DESCR_INV(pPortCtrl, pRxDescr);
                    pRxDescr = RX_NEXT_DESC_PTR(pRxDescr, pQueueCtrl);
                    i++;
                } while (pRxDescr != pQueueCtrl->pFirstDescr);
            }
        }
        else
            mvOsPrintf("RX Queue #%d is NOT CREATED\n", rxQueue);
    }

    if( (txQueue >=0) && (txQueue < MV_ETH_TX_Q_NUM) )
    {
        pQueueCtrl = &(pPortCtrl->txQueue[txQueue]);
        mvOsPrintf("Port #%d, TX Queue #%d\n\n", port, txQueue);

        regValue = MV_REG_READ( ETH_TX_CUR_DESC_PTR_REG(port, txQueue));
        mvOsPrintf("CURR_TX_DESC_PTR        : 0x%X = 0x%08x\n", 
                    ETH_TX_CUR_DESC_PTR_REG(port, txQueue), regValue);

        if(pQueueCtrl->pFirstDescr != NULL)
        {
            mvOsPrintf("pFirstDescr=0x%lx, pLastDescr=0x%lx, numOfResources=%d\n",
                       (MV_ULONG)pQueueCtrl->pFirstDescr, 
                       (MV_ULONG)pQueueCtrl->pLastDescr, 
                        pQueueCtrl->resource);
            mvOsPrintf("pCurrDescr: 0x%lx, pUsedDescr: 0x%lx\n",
                       (MV_ULONG)pQueueCtrl->pCurrentDescr, 
                       (MV_ULONG)pQueueCtrl->pUsedDescr);

            if(mode == 1)
            {
                pTxDescr = (ETH_TX_DESC*)pQueueCtrl->pFirstDescr;
                i = 0; 
                do 
                {
                    mvOsPrintf("%3d. desc=%08x (%08x), cmd=%08x, data=%4d, buf=%08x, pkt=%lx, os=%lx\n", 
                                i, (MV_U32)pTxDescr, (MV_U32)ethDescVirtToPhy(pQueueCtrl, (MV_U8*)pTxDescr), 
                                pTxDescr->cmdSts, pTxDescr->byteCnt, 
                                (MV_U32)pTxDescr->bufPtr, (MV_ULONG)pTxDescr->returnInfo,
                                pTxDescr->returnInfo ? (((MV_PKT_INFO*)pTxDescr->returnInfo)->osInfo) : 0x0);

                    ETH_DESCR_INV(pPortCtrl, pTxDescr);
                    pTxDescr = TX_NEXT_DESC_PTR(pTxDescr, pQueueCtrl);
                    i++;
                } while (pTxDescr != pQueueCtrl->pFirstDescr);
            }
        }
        else
            mvOsPrintf("TX Queue #%d is NOT CREATED\n", txQueue);
    }
}

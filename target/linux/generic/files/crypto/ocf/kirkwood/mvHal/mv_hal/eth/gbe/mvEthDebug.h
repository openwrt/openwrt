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
#ifndef __MV_ETH_DEBUG_H__
#define __MV_ETH_DEBUG_H__

#if 0
/*
 ** Externs
 */
void     ethBpduRxQ(int port, int bpduQueue);
void     ethArpRxQ(int port, int bpduQueue);
void     ethTcpRxQ(int port, int bpduQueue);
void     ethUdpRxQ(int port, int bpduQueue);
void     ethMcastAdd(int port, char* macStr, int queue);

#ifdef INCLUDE_MULTI_QUEUE
void   	ethRxPolicy( int port);
void   	ethTxPolicy( int port);
void   	ethTxPolDA(int port, char* macStr, int txQ, char* headerHexStr);
void   	ethRxPolMode(int port, MV_ETH_PRIO_MODE prioMode);
void    ethRxPolQ(int port, int rxQueue, int rxQuota);
#endif /* INCLUDE_MULTI_QUEUE */

void    print_egiga_stat(void *sc, unsigned int port);
void    ethPortStatus (int port);
void    ethPortQueues( int port, int rxQueue, int txQueue, int mode);
void    ethPortMcast(int port);
void    ethPortRegs(int port);
void    ethPortCounters(int port);
void 	ethPortRmonCounters(int port);
void    ethRxCoal(int port, int usec);
void    ethTxCoal(int port, int usec);

void    ethRegs(int port);
void	ethClearCounters(int port);
void    ethUcastSet(int port, char* macStr, int queue);
void    ethPortUcastShow(int port);

#ifdef CONFIG_MV_ETH_HEADER
void	run_com_header(const char *buffer);
#endif

#ifdef INCLUDE_MULTI_QUEUE
void    ethRxPolMode(int port, MV_ETH_PRIO_MODE prioMode);
void    ethRxPolQ(int port, int queue, int quota);
void    ethRxPolicy(int port);
void    ethTxPolDef(int port, int txQ, char* headerHexStr);
void    ethTxPolDA(int port, char* macStr, int txQ, char* headerHexStr);
void    ethTxPolicy(int port);
#endif /* INCLUDE_MULTI_QUEUE */

#if (MV_ETH_VERSION >= 4)
void     ethEjpModeSet(int port, int mode)
#endif
#endif /* 0 */




void    ethRxCoal(int port, int usec);
void    ethTxCoal(int port, int usec);
#if (MV_ETH_VERSION >= 4)
void     ethEjpModeSet(int port, int mode);
#endif /* (MV_ETH_VERSION >= 4) */

void    ethBpduRxQ(int port, int bpduQueue);
void    ethArpRxQ(int port, int arpQueue);
void    ethTcpRxQ(int port, int tcpQueue);
void    ethUdpRxQ(int port, int udpQueue);
void    ethTxPolicyRegs(int port);
void    ethPortRegs(int port);
void    ethRegs(int port);
void ethClearCounters(int port);
void    ethPortCounters(int port);
void    ethPortRmonCounters(int port);
void    ethPortStatus(int port);
void    ethPortQueues(int port, int rxQueue, int txQueue, int mode);
void    ethUcastSet(int port, char* macStr, int queue);
void    ethPortUcastShow(int port);
void    ethMcastAdd(int port, char* macStr, int queue);
void    ethPortMcast(int port);
void    mvEthPortShow(void* pHndl);
void    mvEthQueuesShow(void* pHndl, int rxQueue, int txQueue, int mode);

#endif

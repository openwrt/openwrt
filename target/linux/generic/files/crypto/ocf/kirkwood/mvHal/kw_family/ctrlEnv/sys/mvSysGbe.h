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

#ifndef __INCmvSysGbeh
#define __INCmvSysGbeh

#include "mvCommon.h"
#include "eth/mvEth.h"
#include "ctrlEnv/mvCtrlEnvSpec.h"
#include "ctrlEnv/sys/mvCpuIf.h"

#define ETH_WIN_BASE_REG(port, win)         (MV_ETH_REG_BASE(port) + 0x200 + ((win)<<3))
#define ETH_WIN_SIZE_REG(port, win)         (MV_ETH_REG_BASE(port) + 0x204 + ((win)<<3))
#define ETH_WIN_REMAP_REG(port, win)        (MV_ETH_REG_BASE(port) + 0x280 + ((win)<<2))
#define ETH_BASE_ADDR_ENABLE_REG(port)      (MV_ETH_REG_BASE(port) + 0x290)
#define ETH_ACCESS_PROTECT_REG(port)        (MV_ETH_REG_BASE(port) + 0x294)

/**** Address decode parameters ****/

/* Ethernet Base Address Register bits */
#define ETH_MAX_DECODE_WIN              6
#define ETH_MAX_HIGH_ADDR_REMAP_WIN     4

/* Ethernet Port Access Protect (EPAP) register */

/* The target associated with this window*/
#define ETH_WIN_TARGET_OFFS             0 
#define ETH_WIN_TARGET_MASK             (0xf << ETH_WIN_TARGET_OFFS)
/* The target attributes Associated with window */
#define ETH_WIN_ATTR_OFFS               8 
#define ETH_WIN_ATTR_MASK               (0xff << ETH_WIN_ATTR_OFFS)

/* Ethernet Port Access Protect Register (EPAPR) */
#define ETH_PROT_NO_ACCESS              NO_ACCESS_ALLOWED
#define ETH_PROT_READ_ONLY              READ_ONLY
#define ETH_PROT_FULL_ACCESS            FULL_ACCESS
#define ETH_PROT_WIN_OFFS(winNum)       (2 * (winNum))
#define ETH_PROT_WIN_MASK(winNum)       (0x3 << ETH_PROT_WIN_OFFS(winNum))

MV_STATUS   mvEthWinInit (int port);
MV_STATUS   mvEthWinEnable(int port, MV_U32 winNum, MV_BOOL enable);
MV_U32      mvEthWinTargetGet(int port, MV_TARGET target);
MV_STATUS mvEthProtWinSet(MV_U32 portNo, MV_U32 winNum, MV_ACCESS_RIGHTS
		access);

void        mvEthPortAddrDecShow(int port);

MV_VOID     mvEthAddrDecShow(MV_VOID);

void        mvEthInit(void);

#endif

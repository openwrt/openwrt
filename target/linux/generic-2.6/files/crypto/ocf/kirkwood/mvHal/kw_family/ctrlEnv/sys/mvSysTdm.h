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

#ifndef __INCmvSysTdmh
#define __INCmvSysTdmh

#include "ctrlEnv/sys/mvCpuIf.h"
#include "ctrlEnv/mvCtrlEnvLib.h"
#include "ctrlEnv/mvCtrlEnvAddrDec.h"

typedef struct _mvTdmDecWin
{
        MV_TARGET     target;
        MV_ADDR_WIN   addrWin; /* An address window*/
        MV_BOOL       enable;  /* Address decode window is enabled/disabled */ 
} MV_TDM_DEC_WIN;

MV_STATUS mvTdmWinInit(MV_VOID);
MV_STATUS mvTdmWinSet(MV_U32 winNum, MV_TDM_DEC_WIN *pAddrDecWin);
MV_STATUS mvTdmWinGet(MV_U32 winNum, MV_TDM_DEC_WIN *pAddrDecWin);
MV_STATUS mvTdmWinEnable(int winNum, MV_BOOL enable);
MV_VOID mvTdmAddrDecShow(MV_VOID);


#define TDM_MBUS_MAX_WIN	4
#define TDM_WIN_CTRL_REG(win)	((TDM_REG_BASE + 0x4030) + (win<<4))
#define TDM_WIN_BASE_REG(win)	((TDM_REG_BASE +0x4034) + (win<<4))

/* TDM_WIN_CTRL_REG bits */
#define TDM_WIN_ENABLE_OFFS	0
#define TDM_WIN_ENABLE_MASK	(1<<TDM_WIN_ENABLE_OFFS)
#define TDM_WIN_ENABLE		1
#define TDM_WIN_TARGET_OFFS	4
#define TDM_WIN_TARGET_MASK	(0xf<<TDM_WIN_TARGET_OFFS)
#define TDM_WIN_ATTRIB_OFFS	8
#define TDM_WIN_ATTRIB_MASK	(0xff<<TDM_WIN_ATTRIB_OFFS)
#define TDM_WIN_SIZE_OFFS	16
#define TDM_WIN_SIZE_MASK	(0xffff<<TDM_WIN_SIZE_OFFS)

/* TDM_WIN_BASE_REG bits */
#define TDM_BASE_OFFS		16
#define TDM_BASE_MASK		(0xffff<<TDM_BASE_OFFS)

#endif /*__INCmvSysTdmh*/


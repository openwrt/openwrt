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


#ifndef __INCmvAhbToMbush
#define __INCmvAhbToMbush

/* includes */
#include "ctrlEnv/mvCtrlEnvLib.h"
#include "ctrlEnv/sys/mvAhbToMbusRegs.h"
#include "ctrlEnv/mvCtrlEnvAddrDec.h"

/* defines  */

#if defined(MV_88F1181)
/* This enumerator defines the Marvell controller possible MBUS arbiter     */
/* target ports. It is used to define crossbar priority scheame (pizza)     */ 
typedef enum _mvMBusArbTargetId
{
    DRAM_MBUS_ARB_TARGET = 0,    /* Port 0 -> DRAM interface         */
    TWSI_MBUS_ARB_TARGET  = 1,     /* Port 1 -> TWSI 		    */
    ARM_MBUS_ARB_TARGET   = 2,     /* Port 2 -> ARM		    */
	PEX1_MBUS_ARB_TARGET  = 3,    /* Port 3 -> PCI Express 1		    */
    PEX0_MBUS_ARB_TARGET  = 4,    /* Port 4 -> PCI Express0 		    */
	MAX_MBUS_ARB_TARGETS
}MV_MBUS_ARB_TARGET;

typedef struct _mvMBusArbCtrl
{
	MV_BOOL starvEn;
	MV_BOOL highPrio;
	MV_BOOL fixedRoundRobin;

}MV_MBUS_ARB_CTRL;

#endif /* #if defined(MV_88F1181) */

typedef struct _mvAhbtoMbusDecWin
{
	MV_TARGET	  target;
	MV_ADDR_WIN   addrWin;    /* An address window*/
	MV_BOOL       enable;     /* Address decode window is enabled/disabled    */

}MV_AHB_TO_MBUS_DEC_WIN;

/* mvAhbToMbus.h API list */

MV_STATUS mvAhbToMbusInit(MV_VOID);
MV_STATUS mvAhbToMbusWinSet(MV_U32 winNum, MV_AHB_TO_MBUS_DEC_WIN *pAddrDecWin);
MV_STATUS mvAhbToMbusWinGet(MV_U32 winNum, MV_AHB_TO_MBUS_DEC_WIN *pAddrDecWin);
MV_STATUS mvAhbToMbusWinEnable(MV_U32 winNum,MV_BOOL enable);
MV_U32    mvAhbToMbusWinRemap(MV_U32 winNum, MV_ADDR_WIN *pAddrDecWin);
MV_U32	  mvAhbToMbusWinTargetGet(MV_TARGET target);
MV_U32    mvAhbToMbusWinAvailGet(MV_VOID);
MV_STATUS mvAhbToMbusWinTargetSwap(MV_TARGET target1,MV_TARGET target2);

#if defined(MV_88F1181)

MV_STATUS mvMbusArbSet(MV_MBUS_ARB_TARGET *pPizzaArbArray);
MV_STATUS mvMbusArbCtrlSet(MV_MBUS_ARB_CTRL *ctrl);
MV_STATUS mvMbusArbCtrlGet(MV_MBUS_ARB_CTRL *ctrl);

#endif /* #if defined(MV_88F1181) */


MV_VOID   mvAhbToMbusAddDecShow(MV_VOID);


#endif /* __INCmvAhbToMbush */

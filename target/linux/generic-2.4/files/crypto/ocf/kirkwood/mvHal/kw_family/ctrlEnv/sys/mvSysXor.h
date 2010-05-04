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

#ifndef __INCMVSysXorh
#define __INCMVSysXorh


#ifdef __cplusplus
extern "C" {
#endif

#include "ctrlEnv/sys/mvCpuIf.h"

#include "ctrlEnv/mvCtrlEnvLib.h"
#include "ctrlEnv/mvCtrlEnvAddrDec.h"

#define XOR_MAX_ADDR_DEC_WIN	8	/* Maximum address decode windows		*/
#define XOR_MAX_REMAP_WIN       4	/* Maximum address arbiter windows		*/

/* XOR Engine Address Decoding Register Map */                  
#define XOR_WINDOW_CTRL_REG(unit,chan)     (XOR_UNIT_BASE(unit)+(0x240 + ((chan) * 4)))
#define XOR_BASE_ADDR_REG(unit,winNum)     (XOR_UNIT_BASE(unit)+(0x250 + ((winNum) * 4)))
#define XOR_SIZE_MASK_REG(unit,winNum)     (XOR_UNIT_BASE(unit)+(0x270 + ((winNum) * 4)))
#define XOR_HIGH_ADDR_REMAP_REG(unit,winNum) (XOR_UNIT_BASE(unit)+(0x290 + ((winNum) * 4)))

/* XOR Engine [0..1] Window Control Registers (XExWCR) */
#define XEXWCR_WIN_EN_OFFS(winNum)          (winNum)
#define XEXWCR_WIN_EN_MASK(winNum)          (1 << (XEXWCR_WIN_EN_OFFS(winNum)))
#define XEXWCR_WIN_EN_ENABLE(winNum)        (1 << (XEXWCR_WIN_EN_OFFS(winNum)))
#define XEXWCR_WIN_EN_DISABLE(winNum)       (0 << (XEXWCR_WIN_EN_OFFS(winNum)))

#define XEXWCR_WIN_ACC_OFFS(winNum)         ((2 * winNum) + 16)
#define XEXWCR_WIN_ACC_MASK(winNum)         (3 << (XEXWCR_WIN_ACC_OFFS(winNum)))
#define XEXWCR_WIN_ACC_NO_ACC(winNum)       (0 << (XEXWCR_WIN_ACC_OFFS(winNum)))
#define XEXWCR_WIN_ACC_RO(winNum)           (1 << (XEXWCR_WIN_ACC_OFFS(winNum)))
#define XEXWCR_WIN_ACC_RW(winNum)           (3 << (XEXWCR_WIN_ACC_OFFS(winNum)))

/* XOR Engine Base Address Registers (XEBARx) */
#define XEBARX_TARGET_OFFS                  (0)
#define XEBARX_TARGET_MASK                  (0xF << XEBARX_TARGET_OFFS)
#define XEBARX_ATTR_OFFS                    (8)
#define XEBARX_ATTR_MASK                    (0xFF << XEBARX_ATTR_OFFS)
#define XEBARX_BASE_OFFS                    (16)
#define XEBARX_BASE_MASK                    (0xFFFF << XEBARX_BASE_OFFS)

/* XOR Engine Size Mask Registers (XESMRx) */
#define XESMRX_SIZE_MASK_OFFS               (16)
#define XESMRX_SIZE_MASK_MASK               (0xFFFF << XESMRX_SIZE_MASK_OFFS)

/* XOR Engine High Address Remap Register (XEHARRx1) */
#define XEHARRX_REMAP_OFFS                  (0)
#define XEHARRX_REMAP_MASK                  (0xFFFFFFFF << XEHARRX_REMAP_OFFS)

typedef struct _mvXorDecWin
{
    MV_TARGET     target;
    MV_ADDR_WIN   addrWin; /* An address window*/
    MV_BOOL       enable;  /* Address decode window is enabled/disabled */
 
}MV_XOR_DEC_WIN;

MV_STATUS   mvXorInit (MV_VOID);
MV_STATUS   mvXorTargetWinSet(MV_U32 unit, MV_U32 winNum,
			      MV_XOR_DEC_WIN *pAddrDecWin);
MV_STATUS   mvXorTargetWinGet(MV_U32 unit, MV_U32 winNum,
			      MV_XOR_DEC_WIN *pAddrDecWin);
MV_STATUS   mvXorTargetWinEnable(MV_U32 unit, 
			      MV_U32 winNum, MV_BOOL enable);
MV_STATUS   mvXorProtWinSet (MV_U32 unit,MV_U32 chan, MV_U32 winNum, MV_BOOL access, 
                             MV_BOOL write);
MV_STATUS   mvXorPciRemap(MV_U32 unit, MV_U32 winNum, MV_U32 addrHigh);

MV_VOID     mvXorAddrDecShow(MV_VOID);

#ifdef __cplusplus
}
#endif

#endif

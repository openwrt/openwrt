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

#ifndef __INCmvTmrwtdgRegsh
#define __INCmvTmrwtdgRegsh

/*******************************************/
/* ARM Timers Registers Map                */
/*******************************************/

#define CNTMR_RELOAD_REG(tmrNum)	(CNTMR_BASE + 0x10 + (tmrNum)*8 + \
						(((tmrNum) <= 3)?0:8))
#define CNTMR_VAL_REG(tmrNum)		(CNTMR_BASE + 0x14 + (tmrNum)*8 + \
						(((tmrNum) <= 3)?0:8))
#define CNTMR_CTRL_REG			(CNTMR_BASE)

/*For MV78XX0*/
#define CNTMR_CAUSE_REG		(CPU_AHB_MBUS_CAUSE_INT_REG(whoAmI()))
#define CNTMR_MASK_REG		(CPU_AHB_MBUS_MASK_INT_REG(whoAmI()))

/* ARM Timers Registers Map                */
/*******************************************/


/* ARM Timers Control Register */
/* CPU_TIMERS_CTRL_REG (CTCR) */

#define TIMER0_NUM				0
#define TIMER1_NUM				1
#define WATCHDOG_NUM			2
#define TIMER2_NUM				3
#define TIMER3_NUM				4
	
#define CTCR_ARM_TIMER_EN_OFFS(cntr)	(cntr * 2)
#define CTCR_ARM_TIMER_EN_MASK(cntr)	(1 << CTCR_ARM_TIMER_EN_OFFS)
#define CTCR_ARM_TIMER_EN(cntr)			(1 << CTCR_ARM_TIMER_EN_OFFS(cntr))
#define CTCR_ARM_TIMER_DIS(cntr)		(0 << CTCR_ARM_TIMER_EN_OFFS(cntr))
	
#define CTCR_ARM_TIMER_AUTO_OFFS(cntr)	((cntr * 2) + 1)
#define CTCR_ARM_TIMER_AUTO_MASK(cntr)	BIT1
#define CTCR_ARM_TIMER_AUTO_EN(cntr)	(1 << CTCR_ARM_TIMER_AUTO_OFFS(cntr))
#define CTCR_ARM_TIMER_AUTO_DIS(cntr)	(0 << CTCR_ARM_TIMER_AUTO_OFFS(cntr))


/* ARM Timer\Watchdog Reload Register */
/* CNTMR_RELOAD_REG (TRR) */

#define TRG_ARM_TIMER_REL_OFFS			0
#define TRG_ARM_TIMER_REL_MASK			0xffffffff

/* ARM Timer\Watchdog Register */
/* CNTMR_VAL_REG (TVRG) */

#define TVR_ARM_TIMER_OFFS			0
#define TVR_ARM_TIMER_MASK			0xffffffff
#define TVR_ARM_TIMER_MAX			0xffffffff



#endif /* __INCmvTmrwtdgRegsh */

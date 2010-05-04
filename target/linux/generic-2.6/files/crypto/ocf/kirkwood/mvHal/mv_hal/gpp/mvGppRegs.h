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

#ifndef __INCmvGppRegsH
#define __INCmvGppRegsH

#define    MV_GPP0  BIT0
#define    MV_GPP1  BIT1
#define    MV_GPP2  BIT2
#define    MV_GPP3  BIT3
#define    MV_GPP4  BIT4
#define    MV_GPP5  BIT5
#define    MV_GPP6  BIT6
#define    MV_GPP7  BIT7
#define    MV_GPP8  BIT8
#define    MV_GPP9  BIT9
#define    MV_GPP10 BIT10
#define    MV_GPP11 BIT11
#define    MV_GPP12 BIT12
#define    MV_GPP13 BIT13
#define    MV_GPP14 BIT14
#define    MV_GPP15 BIT15
#define    MV_GPP16 BIT16
#define    MV_GPP17 BIT17
#define    MV_GPP18 BIT18
#define    MV_GPP19 BIT19
#define    MV_GPP20 BIT20
#define    MV_GPP21 BIT21
#define    MV_GPP22 BIT22
#define    MV_GPP23 BIT23
#define    MV_GPP24 BIT24
#define    MV_GPP25 BIT25
#define    MV_GPP26 BIT26
#define    MV_GPP27 BIT27
#define    MV_GPP28 BIT28
#define    MV_GPP29 BIT29
#define    MV_GPP30 BIT30
#define    MV_GPP31 BIT31


/* registers offsets */
   
#define GPP_DATA_OUT_REG(grp)			((grp == 0) ? 0x10100 : 0x10140)
#define GPP_DATA_OUT_EN_REG(grp)		((grp == 0) ? 0x10104 : 0x10144)
#define GPP_BLINK_EN_REG(grp)			((grp == 0) ? 0x10108 : 0x10148)
#define GPP_DATA_IN_POL_REG(grp)		((grp == 0) ? 0x1010C : 0x1014c)
#define GPP_DATA_IN_REG(grp)			((grp == 0) ? 0x10110 : 0x10150)
#define GPP_INT_CAUSE_REG(grp)			((grp == 0) ? 0x10114 : 0x10154)
#define GPP_INT_MASK_REG(grp)			((grp == 0) ? 0x10118 : 0x10158)
#define GPP_INT_LVL_REG(grp)			((grp == 0) ? 0x1011c : 0x1015c)

#define GPP_DATA_OUT_SET_REG			0x10120
#define GPP_DATA_OUT_CLEAR_REG			0x10124

#endif /* #ifndef __INCmvGppRegsH */

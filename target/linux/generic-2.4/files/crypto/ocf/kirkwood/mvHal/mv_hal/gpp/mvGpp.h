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

#ifndef __INCmvGppH
#define __INCmvGppH

#include "mvCommon.h"
#include "mvOs.h"
#include "ctrlEnv/mvCtrlEnvSpec.h"
#include "gpp/mvGppRegs.h"

/* These macros describes the GPP type. Each of the GPPs pins can        	*/
/* be assigned to act as a general purpose input or output pin.             */
#define	MV_GPP_IN	0xFFFFFFFF      			/* GPP input   */
#define MV_GPP_OUT  0    						/* GPP output  */


/* These macros describes the GPP Out Enable. */
#define	MV_GPP_OUT_DIS	0xFFFFFFFF         	/* Out pin disabled*/
#define MV_GPP_OUT_EN   0    			 	/* Out pin enabled*/

/* These macros describes the GPP Out Blinking. */
/* When set and the corresponding bit in GPIO Data Out Enable Control 		*/
/* Register is enabled, the GPIO pin blinks every ~100 ms (a period of		*/
/* 2^24 TCLK clocks).														*/
#define	MV_GPP_OUT_BLINK	0xFFFFFFFF         	/* Out pin blinking*/
#define MV_GPP_OUT_STATIC   0    			 	/* Out pin static*/


/* These macros describes the GPP Polarity. */
/* When set to 1 GPIO Data In Register reflects the inverted value of the	*/
/* corresponding pin.														*/

#define	MV_GPP_IN_INVERT	0xFFFFFFFF         	/* Inverted value is got*/
#define MV_GPP_IN_ORIGIN    0    			 	/* original value is got*/

/* mvGppTypeSet - Set PP pin mode (IN or OUT) */
MV_STATUS mvGppTypeSet(MV_U32 group, MV_U32 mask, MV_U32 value);

/* mvGppBlinkEn - Set a GPP (IN) Pin list to blink every ~100ms */
MV_STATUS mvGppBlinkEn(MV_U32 group, MV_U32 mask, MV_U32 value);

/* mvGppPolaritySet - Set a GPP (IN) Pin list Polarity mode. */
MV_STATUS mvGppPolaritySet(MV_U32 group, MV_U32 mask, MV_U32 value);

/* mvGppPolarityGet - Get the Polarity of a GPP Pin */
MV_U32  mvGppPolarityGet(MV_U32 group, MV_U32 mask);

/* mvGppValueGet - Get a GPP Pin list value.*/
MV_U32 mvGppValueGet(MV_U32 group, MV_U32 mask);


/* mvGppValueSet - Set a GPP Pin list value. */
MV_STATUS mvGppValueSet (MV_U32 group, MV_U32 mask, MV_U32 value);

#endif /* #ifndef __INCmvGppH */ 


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


#ifndef __INCmvAhbToMbusRegsh
#define __INCmvAhbToMbusRegsh

/******************************/
/* ARM Address Map Registers  */
/******************************/

#define MAX_AHB_TO_MBUS_WINS	9
#define MV_AHB_TO_MBUS_INTREG_WIN	8


#define AHB_TO_MBUS_WIN_CTRL_REG(winNum)		(0x20000 + (winNum)*0x10)
#define AHB_TO_MBUS_WIN_BASE_REG(winNum)		(0x20004 + (winNum)*0x10)
#define AHB_TO_MBUS_WIN_REMAP_LOW_REG(winNum)	(0x20008 + (winNum)*0x10)
#define AHB_TO_MBUS_WIN_REMAP_HIGH_REG(winNum)	(0x2000C + (winNum)*0x10)
#define AHB_TO_MBUS_WIN_INTEREG_REG				 0x20080

/* Window Control Register      */
/* AHB_TO_MBUS_WIN_CTRL_REG (ATMWCR)*/
#define ATMWCR_WIN_ENABLE					BIT0	/* Window Enable */

#define ATMWCR_WIN_TARGET_OFFS			4 /* The target interface associated 
											 with this window*/
#define ATMWCR_WIN_TARGET_MASK			(0xf << ATMWCR_WIN_TARGET_OFFS)

#define ATMWCR_WIN_ATTR_OFFS				8 /* The target interface attributes
											 Associated with this window */
#define ATMWCR_WIN_ATTR_MASK				(0xff << ATMWCR_WIN_ATTR_OFFS)

											 
/*
Used with the Base register to set the address window size and location
Must be programed from LSB to MSB as sequence of 1’s followed
by sequence of 0’s. The number of 1’s specifies the size of the window
in 64 KB granularity (e.g. a value of 0x00FF specifies 256 = 16 MB).

NOTE: A value of 0x0 specifies 64KB size.
*/
#define ATMWCR_WIN_SIZE_OFFS				16 /* Window Size */
#define ATMWCR_WIN_SIZE_MASK				(0xffff << ATMWCR_WIN_SIZE_OFFS)
#define ATMWCR_WIN_SIZE_ALIGNMENT			0x10000

/*  Window Base Register     */
/* AHB_TO_MBUS_WIN_BASE_REG (ATMWBR) */

/*
Used with the size field to set the address window size and location.
Corresponds to transaction address[31:16]
*/
#define ATMWBR_BASE_OFFS					16 /* Base Address */
#define ATMWBR_BASE_MASK					(0xffff << 	ATMWBR_BASE_OFFS)
#define ATMWBR_BASE_ALIGNMENT				0x10000

/*  Window Remap Low Register   */
/* AHB_TO_MBUS_WIN_REMAP_LOW_REG (ATMWRLR) */

/*
Used with the size field to specifies address bits[31:0] to be driven to
the target interface.:
target_addr[31:16] = (addr[31:16] & size[15:0]) | (remap[31:16] & ~size[15:0])
*/
#define ATMWRLR_REMAP_LOW_OFFS			16 /* Remap Address */
#define ATMWRLR_REMAP_LOW_MASK			(0xffff << ATMWRLR_REMAP_LOW_OFFS)
#define ATMWRLR_REMAP_LOW_ALIGNMENT		0x10000

/* Window Remap High Register   */
/* AHB_TO_MBUS_WIN_REMAP_HIGH_REG (ATMWRHR) */

/*
Specifies address bits[63:32] to be driven to the target interface.
target_addr[63:32] = (RemapHigh[31:0]
*/
#define ATMWRHR_REMAP_HIGH_OFFS			0 /* Remap Address */
#define ATMWRHR_REMAP_HIGH_MASK			(0xffffffff << ATMWRHR_REMAP_HIGH_OFFS)


#endif /* __INCmvAhbToMbusRegsh */


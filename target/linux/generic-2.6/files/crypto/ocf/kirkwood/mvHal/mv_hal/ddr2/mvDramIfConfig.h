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


#ifndef __INCmvDramIfConfigh
#define __INCmvDramIfConfigh

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* includes */

/* defines  */

/* registers defaults values */

#define SDRAM_CONFIG_DV 	(SDRAM_SRMODE_DRAM | BIT25 | BIT30)

#define SDRAM_DUNIT_CTRL_LOW_DDR2_DV			\
		(SDRAM_SRCLK_KEPT		|	\
		 SDRAM_CLK1DRV_NORMAL		|	\
		 (BIT28 | BIT29))

#define SDRAM_ADDR_CTRL_DV	    2
		
#define SDRAM_TIMING_CTRL_LOW_REG_DV 	\
		((0x2 << SDRAM_TRCD_OFFS) | 	\
		 (0x2 << SDRAM_TRP_OFFS)  | 	\
		 (0x1 << SDRAM_TWR_OFFS)  | 	\
		 (0x0 << SDRAM_TWTR_OFFS) | 	\
		 (0x5 << SDRAM_TRAS_OFFS) | 	\
		 (0x1 << SDRAM_TRRD_OFFS))

/* Note: value of 0 in register means one cycle, 1 means two and so on  */
#define SDRAM_TIMING_CTRL_HIGH_REG_DV 	\
		((0x0 << SDRAM_TR2R_OFFS)	|	\
		 (0x0 << SDRAM_TR2W_W2R_OFFS)	|	\
		 (0x1 << SDRAM_TW2W_OFFS))

#define SDRAM_OPEN_PAGES_CTRL_REG_DV 	SDRAM_OPEN_PAGE_EN	

/* Presence	     Ctrl Low    Ctrl High  Dunit Ctrl   Ext Mode     */
/* CS0              0x84210000  0x00000000  0x0000780F  0x00000440    */
/* CS0+CS1          0x84210000  0x00000000  0x0000780F  0x00000440    */
/* CS0+CS2          0x030C030C  0x00000000  0x0000740F  0x00000404    */
/* CS0+CS1+CS2      0x030C030C  0x00000000  0x0000740F  0x00000404    */
/* CS0+CS2+CS3      0x030C030C  0x00000000  0x0000740F  0x00000404    */
/* CS0+CS1+CS2+CS3  0x030C030C  0x00000000  0x0000740F  0x00000404    */

#define DDR2_ODT_CTRL_LOW_CS0_CS1_DV		0x84210000
#define DDR2_ODT_CTRL_HIGH_CS0_CS1_DV		0x00000000
#define DDR2_DUNIT_ODT_CTRL_CS0_CS1_DV		0x0000E80F
#ifdef MV78XX0
#define DDR_SDRAM_EXT_MODE_CS0_CS1_DV		0x00000040
#else
#define DDR_SDRAM_EXT_MODE_CS0_CS1_DV		0x00000440
#endif

#define DDR2_ODT_CTRL_LOW_CS0_CS1_CS2_CS3_DV	0x030C030C
#define DDR2_ODT_CTRL_HIGH_CS0_CS1_CS2_CS3_DV	0x00000000
#define DDR2_DUNIT_ODT_CTRL_CS0_CS1_CS2_CS3_DV	0x0000F40F
#ifdef MV78XX0
#define DDR_SDRAM_EXT_MODE_CS0_CS1_CS2_CS3_DV	0x00000004
#define DDR_SDRAM_EXT_MODE_FAST_CS0_CS1_CS2_CS3_DV	0x00000044
#else
#define DDR_SDRAM_EXT_MODE_CS0_CS1_CS2_CS3_DV	0x00000404
#define DDR_SDRAM_EXT_MODE_FAST_CS0_CS1_CS2_CS3_DV	0x00000444
#endif

/* DDR SDRAM Adderss/Control and Data Pads Calibration default values */
#define DDR2_ADDR_CTRL_PAD_STRENGTH_TYPICAL_DV	\
		(3 << SDRAM_PRE_DRIVER_STRENGTH_OFFS)
		
#define DDR2_DATA_PAD_STRENGTH_TYPICAL_DV		\
		(3 << SDRAM_PRE_DRIVER_STRENGTH_OFFS)

/* DDR SDRAM Mode Register default value */
#define DDR2_MODE_REG_DV		(SDRAM_BURST_LEN_4 | SDRAM_WR_3_CYC)
/* DDR SDRAM Timing parameter default values */
#define SDRAM_TIMING_CTRL_LOW_REG_DEFAULT  	0x33136552
#define SDRAM_TRFC_DEFAULT_VALUE		0x34
#define SDRAM_TRFC_DEFAULT		SDRAM_TRFC_DEFAULT_VALUE
#define SDRAM_TW2W_DEFALT		(0x1 << SDRAM_TW2W_OFFS)

#define SDRAM_TIMING_CTRL_HIGH_REG_DEFAULT  (SDRAM_TRFC_DEFAULT | SDRAM_TW2W_DEFALT)

#define SDRAM_FTDLL_REG_DEFAULT_LEFT  		0x88C800
#define SDRAM_FTDLL_REG_DEFAULT_RIGHT  		0x88C800
#define SDRAM_FTDLL_REG_DEFAULT_UP  		0x88C800

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __INCmvDramIfh */

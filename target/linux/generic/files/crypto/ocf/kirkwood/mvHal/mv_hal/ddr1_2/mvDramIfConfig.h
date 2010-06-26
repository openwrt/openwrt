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

/* includes */

/* defines  */

/* registers defaults values */

#define SDRAM_CONFIG_DV 				\
		(SDRAM_PERR_WRITE			|	\
		 SDRAM_SRMODE				|	\
		 SDRAM_SRCLK_GATED)

#define SDRAM_DUNIT_CTRL_LOW_DV			\
		(SDRAM_CTRL_POS_RISE		|	\
		 SDRAM_CLK1DRV_NORMAL		|	\
		 SDRAM_LOCKEN_ENABLE)

#define SDRAM_ADDR_CTRL_DV	    0
		
#define SDRAM_TIMING_CTRL_LOW_REG_DV 	\
		((0x2 << SDRAM_TRCD_OFFS)	|	\
		 (0x2 << SDRAM_TRP_OFFS)	|	\
		 (0x1 << SDRAM_TWR_OFFS)	|	\
		 (0x0 << SDRAM_TWTR_OFFS)	|	\
		 (0x5 << SDRAM_TRAS_OFFS)	|	\
		 (0x1 << SDRAM_TRRD_OFFS))
/* TRFC 0x27, TW2W 0x1 */
#define SDRAM_TIMING_CTRL_HIGH_REG_DV	(( 0x7 << SDRAM_TRFC_OFFS )	|\
					( 0x2 << SDRAM_TRFC_EXT_OFFS)	|\
					( 0x1 << SDRAM_TW2W_OFFS))

#define SDRAM_OPEN_PAGES_CTRL_REG_DV	SDRAM_OPEN_PAGE_EN	

/* DDR2 ODT default register values */

/* Presence	     Ctrl Low    Ctrl High  Dunit Ctrl   Ext Mode     */
/*	CS0	         0x84210000  0x00000000  0x0000780F  0x00000440 */
/*	CS0+CS1          0x84210000  0x00000000  0x0000780F  0x00000440 */
/*	CS0+CS2	    	 0x030C030C  0x00000000  0x0000740F  0x00000404 */
/*	CS0+CS1+CS2	 0x030C030C  0x00000000  0x0000740F  0x00000404 */
/*	CS0+CS2+CS3	 0x030C030C  0x00000000  0x0000740F  0x00000404 */
/*	CS0+CS1+CS2+CS3  0x030C030C  0x00000000  0x0000740F  0x00000404 */

#define DDR2_ODT_CTRL_LOW_CS0_DV	0x84210000
#define DDR2_ODT_CTRL_HIGH_CS0_DV	0x00000000
#define DDR2_DUNIT_ODT_CTRL_CS0_DV	0x0000780F
#define DDR_SDRAM_EXT_MODE_CS0_DV	0x00000440

#define DDR2_ODT_CTRL_LOW_CS0_CS2_DV	0x030C030C
#define DDR2_ODT_CTRL_HIGH_CS0_CS2_DV	0x00000000
#define DDR2_DUNIT_ODT_CTRL_CS0_CS2_DV	0x0000740F
#define DDR_SDRAM_EXT_MODE_CS0_CS2_DV	0x00000404


/* DDR SDRAM Adderss/Control and Data Pads Calibration default values */
#define DDR1_ADDR_CTRL_PAD_STRENGTH_TYPICAL_DV	\
		(1 << SDRAM_PRE_DRIVER_STRENGTH_OFFS)
#define DDR2_ADDR_CTRL_PAD_STRENGTH_TYPICAL_DV	\
		(3 << SDRAM_PRE_DRIVER_STRENGTH_OFFS)
		
		
#define DDR1_DATA_PAD_STRENGTH_TYPICAL_DV		\
		(1 << SDRAM_PRE_DRIVER_STRENGTH_OFFS)
#define DDR2_DATA_PAD_STRENGTH_TYPICAL_DV		\
		(3 << SDRAM_PRE_DRIVER_STRENGTH_OFFS)

/* DDR SDRAM Mode Register default value */
#define DDR1_MODE_REG_DV			0x00000000
#define DDR2_MODE_REG_DV			0x00000400

/* DDR SDRAM Timing parameter default values */
#define DDR1_TIMING_LOW_DV           0x11602220
#define DDR1_TIMING_HIGH_DV          0x0000000d

#define DDR2_TIMING_LOW_DV           0x11812220
#define DDR2_TIMING_HIGH_DV          0x0000030f

/* For Guideline (GL# MEM-4) DQS Reference Delay Tuning */
#define FTDLL_DDR1_166MHZ           ((0x1 << 0)    | \
                                     (0x7F<< 12)   | \
                                     (0x1 << 22))

#define FTDLL_DDR1_133MHZ           FTDLL_DDR1_166MHZ

#define FTDLL_DDR1_200MHZ           ((0x1 << 0)    | \
                                     (0x1 << 12)   | \
                                     (0x3 << 14)   | \
                                     (0x1 << 18)   | \
                                     (0x1 << 22))


#define FTDLL_DDR2_166MHZ           ((0x1 << 0)    | \
                                     (0x1 << 12)   | \
                                     (0x1 << 14)   | \
                                     (0x1 << 16)   | \
                                     (0x1 << 19)   | \
                                     (0xF << 20))

#define FTDLL_DDR2_133MHZ           FTDLL_DDR2_166MHZ

#define FTDLL_DDR2_200MHZ           ((0x1 << 0)    | \
                                     (0x1 << 12)   | \
                                     (0x1 << 14)   | \
                                     (0x1 << 16)   | \
                                     (0x1 << 19)   | \
                                     (0xF << 20))

#define FTDLL_DDR2_250MHZ            0x445001

/* Orion 1 B1 and above */
#define FTDLL_DDR1_166MHZ_5181_B1    0x45D001

/* Orion nas */
#define FTDLL_DDR2_166MHZ_5182       0x597001

/* Orion 2 D0 and above */
#define FTDLL_DDR1_166MHZ_5281_D0    0x8D0001
#define FTDLL_DDR1_200MHZ_5281_D0    0x8D0001
#define FTDLL_DDR2_166MHZ_5281_D0    0x485001
#define FTDLL_DDR2_200MHZ_5281_D0    0x485001
#define FTDLL_DDR2_250MHZ_5281_D0    0x445001
#define FTDLL_DDR2_200MHZ_5281_D1    0x995001
#define FTDLL_DDR2_250MHZ_5281_D1    0x984801

#endif /* __INCmvDramIfh */

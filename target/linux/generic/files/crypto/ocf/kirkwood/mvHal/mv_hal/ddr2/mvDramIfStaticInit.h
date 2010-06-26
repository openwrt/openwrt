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


#ifndef __INCmvDramIfStaticInith
#define __INCmvDramIfStaticInith

#ifdef MV_STATIC_DRAM_ON_BOARD
#define STATIC_DRAM_BANK_1
#undef	STATIC_DRAM_BANK_2             
#undef	STATIC_DRAM_BANK_3                         
#undef 	STATIC_DRAM_BANK_4             


#ifdef MV_DIMM_TS256MLQ72V5U
#define	STATIC_DRAM_BANK_2             
#define	STATIC_DRAM_BANK_3                         
#undef 	STATIC_DRAM_BANK_4             

#define STATIC_SDRAM_CONFIG_REG		    0x4724481A  /* offset 0x1400 - DMA reg-0xf1000814 */ 
#define STATIC_SDRAM_DUNIT_CTRL_REG         0x37707450  /* offset 0x1404 - DMA reg-0xf100081c */ 
#define STATIC_SDRAM_TIMING_CTRL_LOW_REG    0x11A13330  /* offset 0x1408 - DMA reg-0xf1000824 */ 
#define STATIC_SDRAM_TIMING_CTRL_HIGH_REG   0x00000601  /* offset 0x140c - DMA reg-0xf1000828 */ 
#define STATIC_SDRAM_ADDR_CTRL_REG          0x00001CB2  /* offset 0x1410 - DMA reg-0xf1000820 */ 
#define STATIC_SDRAM_MODE_REG               0x00000642  /* offset 0x141c - DMA reg-0xf1000818 */ 
#define STATIC_SDRAM_ODT_CTRL_LOW	    0x030C030C /*   0x1494  */  
#define STATIC_SDRAM_ODT_CTRL_HI	    0x00000000 /*   0x1498  */  
#define STATIC_SDRAM_DUNIT_ODT_CTRL    	    0x0000740F /*   0x149c  */  
#define STATIC_SDRAM_EXT_MODE          	    0x00000404 /*   0x1420  */  
#define STATIC_SDRAM_DDR2_TIMING_LO         0x00074410 /*   0x1428  */  
#define STATIC_SDRAM_DDR2_TIMING_HI         0x00007441 /*   0x147C  */  

#define STATIC_SDRAM_RANK0_SIZE_DIMM0       0x3FFF /* size bank0 dimm0   - DMA reg-0xf1000810 */ 
#define STATIC_SDRAM_RANK1_SIZE_DIMM0       0x3FFF /* size bank1 dimm0   */ 
#define STATIC_SDRAM_RANK0_SIZE_DIMM1       0x3FFF /* size bank0 dimm1   */ 
#define STATIC_SDRAM_RANK1_SIZE_DIMM1       0x0	   /* size bank1 dimm1   */ 

#endif /* TS256MLQ72V5U */


#ifdef MV_MT9VDDT3272AG
/* one DIMM 256M  */
#define STATIC_SDRAM_CONFIG_REG		    0x5820040d  /* offset 0x1400 - DMA reg-0xf1000814 */ 
#define STATIC_SDRAM_DUNIT_CTRL_REG         0xC4000540  /* offset 0x1404 - DMA reg-0xf100081c */ 
#define STATIC_SDRAM_TIMING_CTRL_LOW_REG    0x01602220  /* offset 0x1408 - DMA reg-0xf1000824 */ 
#define STATIC_SDRAM_TIMING_CTRL_HIGH_REG   0x0000000b  /* offset 0x140c - DMA reg-0xf1000828 */ 
#define STATIC_SDRAM_ADDR_CTRL_REG          0x00000012  /* offset 0x1410 - DMA reg-0xf1000820 */ 
#define STATIC_SDRAM_MODE_REG               0x00000062  /* offset 0x141c - DMA reg-0xf1000818 */ 
#define STATIC_SDRAM_RANK0_SIZE_DIMM0       0x0fff /* size bank0 dimm0   - DMA reg-0xf1000810 */ 
#define STATIC_SDRAM_RANK0_SIZE_DIMM1       0x0    /* size bank0 dimm1   */ 

#endif /* MV_MT9VDDT3272AG */



#ifdef MV_D27RB12P
/* 
Two DIMM 512M + ECC enabled, Registered DIMM  CAS Latency 2.5
*/

#define STATIC_SDRAM_CONFIG_REG		    0x6826081E  /* offset 0x1400 - DMA reg-0xf1000814 */ 
#define STATIC_SDRAM_DUNIT_CTRL_REG         0xC5000540  /* offset 0x1404 - DMA reg-0xf100081c */ 
#define STATIC_SDRAM_TIMING_CTRL_LOW_REG    0x01501220  /* offset 0x1408 - DMA reg-0xf1000824 */ 
#define STATIC_SDRAM_TIMING_CTRL_HIGH_REG   0x00000009  /* offset 0x140c - DMA reg-0xf1000828 */ 
#define STATIC_SDRAM_ADDR_CTRL_REG          0x00000012  /* offset 0x1410 - DMA reg-0xf1000820 */ 
#define STATIC_SDRAM_MODE_REG               0x00000062  /* offset 0x141c - DMA reg-0xf1000818 */ 
#define STATIC_SDRAM_RANK0_SIZE_DIMM0       0x0FFF /* size bank0 dimm0   - DMA reg-0xf1000810 */ 
#define STATIC_SDRAM_RANK0_SIZE_DIMM1       0x0FFF    /* size bank0 dimm1   */ 

#define STATIC_DRAM_BANK_2             

#define STATIC_DRAM_BANK_3                         
#define STATIC_DRAM_BANK_4             

#endif /*  mv_D27RB12P  */

#ifdef RD_MV645XX

#define STATIC_MEM_TYPE				MEM_TYPE_DDR2
#define STATIC_DIMM_INFO_BANK0_SIZE		256
/* DDR2 boards 256 MB*/

#define STATIC_SDRAM_RANK0_SIZE_DIMM0       	0x00000fff /* size bank0 dimm0   - DMA reg-0xf1000810 */ 
#define STATIC_SDRAM_CONFIG_REG	     		0x07190618	
#define STATIC_SDRAM_MODE_REG	     		0x00000432	
#define STATIC_SDRAM_DUNIT_CTRL_REG     	0xf4a03440
#define STATIC_SDRAM_ADDR_CTRL_REG	     	0x00000022
#define STATIC_SDRAM_TIMING_CTRL_LOW_REG    	0x11712220
#define STATIC_SDRAM_TIMING_CTRL_HIGH_REG	0x00000504
#define STATIC_SDRAM_ODT_CTRL_LOW	     	0x84210000
#define STATIC_SDRAM_ODT_CTRL_HI	     	0x00000000
#define STATIC_SDRAM_DUNIT_ODT_CTRL    		0x0000780f
#define STATIC_SDRAM_EXT_MODE          		0x00000440
#define STATIC_SDRAM_DDR2_TIMING_LO         	0x00063300
#define STATIC_SDRAM_DDR2_TIMING_HI         	0x00006330
#endif /* RD_MV645XX */

#if MV_DIMM_M3783354CZ3_CE6 

#define STATIC_SDRAM_RANK0_SIZE_DIMM0		0x00000FFF /* 0x2010 size bank0 dimm0   - DMA reg-0xf1000810 */ 
#define STATIC_SDRAM_CONFIG_REG	     		0x07190618 /*   0x1400  */ 
#define STATIC_SDRAM_MODE_REG	     		0x00000432 /*   0x141c  */  
#define STATIC_SDRAM_DUNIT_CTRL_REG     	0xf4a03440 /*   0x1404  */  
#define STATIC_SDRAM_ADDR_CTRL_REG	     	0x00000022 /*   0x1410  */  
#define STATIC_SDRAM_TIMING_CTRL_LOW_REG	0x11712220 /*   0x1408  */  
#define STATIC_SDRAM_TIMING_CTRL_HIGH_REG	0x00000504 /*   0x140c  */  
#define STATIC_SDRAM_ODT_CTRL_LOW	     	0x84210000 /*   0x1494  */  
#define STATIC_SDRAM_ODT_CTRL_HI	     	0x00000000 /*   0x1498  */  
#define STATIC_SDRAM_DUNIT_ODT_CTRL    		0x0000780f /*   0x149c  */  
#define STATIC_SDRAM_EXT_MODE          		0x00000440 /*   0x1420  */  
#define STATIC_SDRAM_DDR2_TIMING_LO		0x00063300 /*   0x1428  */  
#define STATIC_SDRAM_DDR2_TIMING_HI		0x00006330 /*   0x147C  */  

#endif /* MV_DIMM_M3783354CZ3_CE6 */

#endif /* MV_STATIC_DRAM_ON_BOARD */
#endif /* __INCmvDramIfStaticInith */


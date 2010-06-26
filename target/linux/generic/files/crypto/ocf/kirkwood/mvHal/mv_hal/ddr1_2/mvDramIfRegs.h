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

#ifndef __INCmvDramIfRegsh
#define __INCmvDramIfRegsh


/* DDR SDRAM Controller Address Decode Registers */
/* SDRAM CSn Base Address Register (SCBAR) */
#define SDRAM_BASE_ADDR_REG(csNum)	(0x1500 + (csNum * 8))
#define SCBAR_BASE_OFFS				16 
#define SCBAR_BASE_MASK				(0xffff << SCBAR_BASE_OFFS)
#define SCBAR_BASE_ALIGNMENT		0x10000 

/* SDRAM CSn Size Register (SCSR) */		  
#define SDRAM_SIZE_REG(csNum)	(0x1504 + (csNum * 8))
#define SCSR_WIN_EN					BIT0
#define SCSR_SIZE_OFFS				16
#define SCSR_SIZE_MASK				(0xffff << SCSR_SIZE_OFFS)
#define SCSR_SIZE_ALIGNMENT			0x10000

/* configuration register */
#define SDRAM_CONFIG_REG   		0x1400
#define SDRAM_REFRESH_OFFS 		0
#define SDRAM_REFRESH_MAX  		0x3000
#define SDRAM_REFRESH_MASK 		(SDRAM_REFRESH_MAX << SDRAM_REFRESH_OFFS)
#define SDRAM_DWIDTH_OFFS       14
#define SDRAM_DWIDTH_MASK       (3 << SDRAM_DWIDTH_OFFS)
#define SDRAM_DWIDTH_16BIT      (1 << SDRAM_DWIDTH_OFFS)
#define SDRAM_DWIDTH_32BIT      (2 << SDRAM_DWIDTH_OFFS)
#define SDRAM_DTYPE_OFFS        16
#define SDRAM_DTYPE_MASK        (1 << SDRAM_DTYPE_OFFS)
#define SDRAM_DTYPE_DDR1        (0 << SDRAM_DTYPE_OFFS)
#define SDRAM_DTYPE_DDR2        (1 << SDRAM_DTYPE_OFFS)
#define SDRAM_REGISTERED   		(1 << 17)
#define SDRAM_PERR_OFFS    		18
#define SDRAM_PERR_MASK    		(1 << SDRAM_PERR_OFFS)
#define SDRAM_PERR_NO_WRITE     (0 << SDRAM_PERR_OFFS)
#define SDRAM_PERR_WRITE        (1 << SDRAM_PERR_OFFS)
#define SDRAM_DCFG_OFFS     	20
#define SDRAM_DCFG_MASK     	(0x3 << SDRAM_DCFG_OFFS)
#define SDRAM_DCFG_X16_DEV   	(1 << SDRAM_DCFG_OFFS)
#define SDRAM_DCFG_X8_DEV   	(2 << SDRAM_DCFG_OFFS)
#define SDRAM_SRMODE			(1 << 24)
#define SDRAM_SRCLK_OFFS		25
#define SDRAM_SRCLK_MASK		(1 << SDRAM_SRCLK_OFFS)
#define SDRAM_SRCLK_KEPT		(0 << SDRAM_SRCLK_OFFS)
#define SDRAM_SRCLK_GATED		(1 << SDRAM_SRCLK_OFFS)
#define SDRAM_CATTH_OFFS		26
#define SDRAM_CATTHR_EN			(1 << SDRAM_CATTH_OFFS)


/* dunit control register */
#define SDRAM_DUNIT_CTRL_REG  	0x1404
#define SDRAM_CTRL_POS_OFFS	   	6
#define SDRAM_CTRL_POS_FALL	   	(0 << SDRAM_CTRL_POS_OFFS)
#define SDRAM_CTRL_POS_RISE	   	(1 << SDRAM_CTRL_POS_OFFS)
#define SDRAM_CLK1DRV_OFFS      12
#define SDRAM_CLK1DRV_MASK      (1 << SDRAM_CLK1DRV_OFFS)
#define SDRAM_CLK1DRV_HIGH_Z    (0 << SDRAM_CLK1DRV_OFFS)
#define SDRAM_CLK1DRV_NORMAL    (1 << SDRAM_CLK1DRV_OFFS)
#define SDRAM_LOCKEN_OFFS       18
#define SDRAM_LOCKEN_MASK       (1 << SDRAM_LOCKEN_OFFS)
#define SDRAM_LOCKEN_DISABLE    (0 << SDRAM_LOCKEN_OFFS)
#define SDRAM_LOCKEN_ENABLE     (1 << SDRAM_LOCKEN_OFFS)
#define SDRAM_ST_BURST_DEL_OFFS 	24
#define SDRAM_ST_BURST_DEL_MAX 	0xf
#define SDRAM_ST_BURST_DEL_MASK (SDRAM_ST_BURST_DEL_MAX<<SDRAM_ST_BURST_DEL_OFFS)

/* sdram timing control low register */
#define SDRAM_TIMING_CTRL_LOW_REG	0x1408
#define SDRAM_TRCD_OFFS 		4
#define SDRAM_TRCD_MASK 		(0xF << SDRAM_TRCD_OFFS)
#define SDRAM_TRP_OFFS 			8
#define SDRAM_TRP_MASK 			(0xF << SDRAM_TRP_OFFS)
#define SDRAM_TWR_OFFS 			12
#define SDRAM_TWR_MASK 			(0xF << SDRAM_TWR_OFFS)
#define SDRAM_TWTR_OFFS 		16
#define SDRAM_TWTR_MASK 		(0xF << SDRAM_TWTR_OFFS)
#define SDRAM_TRAS_OFFS 		20
#define SDRAM_TRAS_MASK 		(0xF << SDRAM_TRAS_OFFS)
#define SDRAM_TRRD_OFFS 		24
#define SDRAM_TRRD_MASK 		(0xF << SDRAM_TRRD_OFFS)
#define SDRAM_TRTP_OFFS 		28
#define SDRAM_TRTP_MASK 		(0xF << SDRAM_TRTP_OFFS)

/* sdram timing control high register */
#define SDRAM_TIMING_CTRL_HIGH_REG	0x140c
#define SDRAM_TRFC_OFFS 		0
#define SDRAM_TRFC_MASK 		(0xF << SDRAM_TRFC_OFFS)
#define SDRAM_TR2R_OFFS 		4
#define SDRAM_TR2R_MASK 		(0x3 << SDRAM_TR2R_OFFS)
#define SDRAM_TR2W_W2R_OFFS 		6
#define SDRAM_TR2W_W2R_MASK 		(0x3 << SDRAM_TR2W_W2R_OFFS)
#define SDRAM_TRFC_EXT_OFFS		8
#define SDRAM_TRFC_EXT_MASK		(0x1 << SDRAM_TRFC_EXT_OFFS)
#define SDRAM_TW2W_OFFS		    10
#define SDRAM_TW2W_MASK		    (0x1 << SDRAM_TW2W_OFFS)

/* address control register */
#define SDRAM_ADDR_CTRL_REG		0x1410
#define SDRAM_DSIZE_OFFS   	    4
#define SDRAM_DSIZE_MASK   	    (0x3 << SDRAM_DSIZE_OFFS)
#define SDRAM_DSIZE_128Mb 	    (0x0 << SDRAM_DSIZE_OFFS)
#define SDRAM_DSIZE_256Mb 	    (0x1 << SDRAM_DSIZE_OFFS)
#define SDRAM_DSIZE_512Mb  	    (0x2 << SDRAM_DSIZE_OFFS)

/* SDRAM Open Pages Control registers */
#define SDRAM_OPEN_PAGE_CTRL_REG	0x1414
#define SDRAM_OPEN_PAGE_EN		    (0 << 0)
#define SDRAM_OPEN_PAGE_DIS		    (1 << 0)

/* sdram opertion register */
#define SDRAM_OPERATION_REG 	0x1418
#define SDRAM_CMD_OFFS  		0
#define SDRAM_CMD_MASK   		(0x7 << SDRAM_CMD_OFFS)
#define SDRAM_CMD_NORMAL 		(0x0 << SDRAM_CMD_OFFS)
#define SDRAM_CMD_PRECHARGE_ALL (0x1 << SDRAM_CMD_OFFS)
#define SDRAM_CMD_REFRESH_ALL 	(0x2 << SDRAM_CMD_OFFS)
#define SDRAM_CMD_REG_SET_CMD 	(0x3 << SDRAM_CMD_OFFS)
#define SDRAM_CMD_EXT_MODE_SET 	(0x4 << SDRAM_CMD_OFFS)
#define SDRAM_CMD_NOP 			(0x5 << SDRAM_CMD_OFFS)
#define SDRAM_CMD_SLF_RFRSH 	(0x7 << SDRAM_CMD_OFFS)
#define SDRAM_CMD_EMRS2_CMD  	(0x8 << SDRAM_CMD_OFFS)
#define SDRAM_CMD_EMRS3_CMD  	(0x9 << SDRAM_CMD_OFFS)

/* sdram mode register */
#define SDRAM_MODE_REG 			0x141c
#define SDRAM_BURST_LEN_OFFS 	0
#define SDRAM_BURST_LEN_MASK 	(0x7 << SDRAM_BURST_LEN_OFFS)
#define SDRAM_BURST_LEN_4    	(0x2 << SDRAM_BURST_LEN_OFFS)
#define SDRAM_CL_OFFS   		4
#define SDRAM_CL_MASK   		(0x7 << SDRAM_CL_OFFS)
#define SDRAM_DDR1_CL_2	     	(0x2 << SDRAM_CL_OFFS)
#define SDRAM_DDR1_CL_3      	(0x3 << SDRAM_CL_OFFS)
#define SDRAM_DDR1_CL_4      	(0x4 << SDRAM_CL_OFFS)
#define SDRAM_DDR1_CL_1_5    	(0x5 << SDRAM_CL_OFFS)
#define SDRAM_DDR1_CL_2_5    	(0x6 << SDRAM_CL_OFFS)
#define SDRAM_DDR2_CL_3      	(0x3 << SDRAM_CL_OFFS)
#define SDRAM_DDR2_CL_4      	(0x4 << SDRAM_CL_OFFS)
#define SDRAM_DDR2_CL_5    		(0x5 << SDRAM_CL_OFFS)
#define SDRAM_TM_OFFS           7
#define SDRAM_TM_MASK           (1 << SDRAM_TM_OFFS)
#define SDRAM_TM_NORMAL         (0 << SDRAM_TM_OFFS)
#define SDRAM_TM_TEST_MODE      (1 << SDRAM_TM_OFFS)
#define SDRAM_DLL_OFFS          8
#define SDRAM_DLL_MASK          (1 << SDRAM_DLL_OFFS)
#define SDRAM_DLL_NORMAL        (0 << SDRAM_DLL_OFFS)
#define SDRAM_DLL_RESET 	(1 << SDRAM_DLL_OFFS)
#define SDRAM_WR_OFFS		11
#define SDRAM_WR_MAX		7
#define SDRAM_WR_MASK		(SDRAM_WR_MAX << SDRAM_WR_OFFS)
#define SDRAM_PD_OFFS		12
#define SDRAM_PD_MASK		(1 << SDRAM_PD_OFFS) 
#define SDRAM_PD_FAST_EXIT	(0 << SDRAM_PD_OFFS) 
#define SDRAM_PD_SLOW_EXIT	(1 << SDRAM_PD_OFFS) 

/* DDR SDRAM Extended Mode register (DSEMR) */
#define SDRAM_EXTENDED_MODE_REG	0x1420
#define DSEMR_DLL_ENABLE		(1 << 0)
#define DSEMR_DS_OFFS			1
#define DSEMR_DS_MASK			(1 << DSEMR_DS_OFFS)
#define DSEMR_DS_NORMAL			(0 << DSEMR_DS_OFFS)
#define DSEMR_DS_REDUCED		(1 << DSEMR_DS_OFFS)
#define DSEMR_RTT0_OFFS			2
#define DSEMR_RTT1_OFFS			6
#define DSEMR_RTT_ODT_DISABLE	((0 << DSEMR_RTT0_OFFS)||(0 << DSEMR_RTT1_OFFS))
#define DSEMR_RTT_ODT_75_OHM	((1 << DSEMR_RTT0_OFFS)||(0 << DSEMR_RTT1_OFFS))
#define DSEMR_RTT_ODT_150_OHM	((0 << DSEMR_RTT0_OFFS)||(1 << DSEMR_RTT1_OFFS))
#define DSEMR_OCD_OFFS			7
#define DSEMR_OCD_MASK			(0x7 << DSEMR_OCD_OFFS)
#define DSEMR_OCD_EXIT_CALIB	(0 << DSEMR_OCD_OFFS)
#define DSEMR_OCD_DRIVE1		(1 << DSEMR_OCD_OFFS)
#define DSEMR_OCD_DRIVE0		(2 << DSEMR_OCD_OFFS)
#define DSEMR_OCD_ADJUST_MODE	(4 << DSEMR_OCD_OFFS)
#define DSEMR_OCD_CALIB_DEFAULT	(7 << DSEMR_OCD_OFFS)
#define DSEMR_DQS_OFFS			10
#define DSEMR_DQS_MASK			(1 << DSEMR_DQS_OFFS)
#define DSEMR_DQS_DIFFERENTIAL	(0 << DSEMR_DQS_OFFS)
#define DSEMR_DQS_SINGLE_ENDED	(0 << DSEMR_DQS_OFFS)
#define DSEMR_RDQS_ENABLE		(1 << 11)
#define DSEMR_QOFF_OUTPUT_BUFF_EN	(1 << 12)

/* DDR SDRAM Operation Control Register */
#define SDRAM_OPERATION_CTRL_REG	0x142c

/* Dunit FTDLL Configuration Register */
#define SDRAM_FTDLL_CONFIG_REG			0x1484
  
/* Pads Calibration register */
#define SDRAM_ADDR_CTRL_PADS_CAL_REG	0x14c0
#define SDRAM_DATA_PADS_CAL_REG		0x14c4
#define SDRAM_DRVN_OFFS 				0
#define SDRAM_DRVN_MASK 				(0x3F << SDRAM_DRVN_OFFS)
#define SDRAM_DRVP_OFFS 				6
#define SDRAM_DRVP_MASK 				(0x3F << SDRAM_DRVP_OFFS)
#define SDRAM_PRE_DRIVER_STRENGTH_OFFS	12
#define SDRAM_PRE_DRIVER_STRENGTH_MASK	(3 << SDRAM_PRE_DRIVER_STRENGTH_OFFS)
#define SDRAM_TUNE_EN   		BIT16
#define SDRAM_LOCK_OFFS 		17
#define SDRAM_LOCK_MAKS 		(0x1F << SDRAM_LOCK_OFFS)
#define SDRAM_LOCKN_OFFS 		17
#define SDRAM_LOCKN_MAKS 		(0x3F << SDRAM_LOCKN_OFFS)
#define SDRAM_LOCKP_OFFS 		23
#define SDRAM_LOCKP_MAKS 		(0x3F << SDRAM_LOCKP_OFFS)
#define SDRAM_WR_EN     		(1 << 31)

/* DDR2 SDRAM ODT Control (Low) Register (DSOCLR) */
#define DDR2_SDRAM_ODT_CTRL_LOW_REG  0x1494
#define DSOCLR_ODT_RD_OFFS(odtNum)   (odtNum * 4)
#define DSOCLR_ODT_RD_MASK(odtNum)   (0xf << DSOCLR_ODT_RD_OFFS(odtNum))
#define DSOCLR_ODT_RD(odtNum, bank)  ((1 << bank) << DSOCLR_ODT_RD_OFFS(odtNum))
#define DSOCLR_ODT_WR_OFFS(odtNum)   (16 + (odtNum * 4))
#define DSOCLR_ODT_WR_MASK(odtNum)   (0xf << DSOCLR_ODT_WR_OFFS(odtNum))
#define DSOCLR_ODT_WD(odtNum, bank)  ((1 << bank) << DSOCLR_ODT_WR_OFFS(odtNum))

/* DDR2 SDRAM ODT Control (High) Register (DSOCHR) */
#define DDR2_SDRAM_ODT_CTRL_HIGH_REG    0x1498
/* Optional control values to DSOCHR_ODT_EN macro */
#define DDR2_ODT_CTRL_DUNIT         0
#define DDR2_ODT_CTRL_NEVER         1
#define DDR2_ODT_CTRL_ALWAYS        3
#define DSOCHR_ODT_EN_OFFS(odtNum)  (odtNum * 2)
#define DSOCHR_ODT_EN_MASK(odtNum)  (0x3 << DSOCHR_ODT_EN_OFFS(odtNum))
#define DSOCHR_ODT_EN(odtNum, ctrl) ((1 << ctrl) << DSOCHR_ODT_RD_OFFS(odtNum))

/* DDR2 Dunit ODT Control Register (DDOCR)*/
#define DDR2_DUNIT_ODT_CONTROL_REG  0x149c
#define DDOCR_ODT_RD_OFFS           0
#define DDOCR_ODT_RD_MASK           (0xf << DDOCR_ODT_RD_OFFS)
#define DDOCR_ODT_RD(bank)          ((1 << bank) << DDOCR_ODT_RD_OFFS)
#define DDOCR_ODT_WR_OFFS           4
#define DDOCR_ODT_WR_MASK           (0xf << DDOCR_ODT_WR_OFFS)
#define DDOCR_ODT_WR(bank)          ((1 << bank) << DDOCR_ODT_WR_OFFS)
#define DSOCR_ODT_EN_OFFS           8
#define DSOCR_ODT_EN_MASK           (0x3 << DSOCR_ODT_EN_OFFS)
#define DSOCR_ODT_EN(ctrl)          ((1 << ctrl) << DSOCR_ODT_EN_OFFS)
#define DSOCR_ODT_SEL_OFFS          10
#define DSOCR_ODT_SEL_MASK          (0x3 << DSOCR_ODT_SEL_OFFS)

/* DDR SDRAM Initialization Control Register (DSICR) */
#define DDR_SDRAM_INIT_CTRL_REG		0x1480
#define DSICR_INIT_EN				(1 << 0)

#endif /* __INCmvDramIfRegsh */

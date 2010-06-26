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

#ifndef __INCSysPEXH
#define __INCSysPEXH

#include "mvCommon.h"
#include "ctrlEnv/sys/mvCpuIf.h"
#include "ctrlEnv/mvCtrlEnvLib.h"
#include "ctrlEnv/mvCtrlEnvAddrDec.h"

/* 4KB granularity */
#define MINIMUM_WINDOW_SIZE     0x1000
#define MINIMUM_BAR_SIZE        0x1000
#define MINIMUM_BAR_SIZE_MASK	0xFFFFF000
#define BAR_SIZE_OFFS			12
#define BAR_SIZE_MASK			(0xFFFFF << BAR_SIZE_OFFS)



#define MV_PEX_WIN_DEFAULT		6
#define MV_PEX_WIN_EXP_ROM		7
#define PEX_MAX_TARGET_WIN		8


#define PEX_MAX_BARS			3
#define PEX_INTER_REGS_BAR		0
#define PEX_DRAM_BAR			1
#define PEX_DEVICE_BAR			2

/*************************************/
/* PCI Express BAR Control Registers */
/*************************************/
#define PEX_BAR_CTRL_REG(pexIf,bar)		(0x41804 + (bar-1)*4- (pexIf)*0x10000)
#define PEX_EXP_ROM_BAR_CTRL_REG(pexIf)	(0x4180C - (pexIf)*0x10000)


/* PCI Express BAR Control Register */
/* PEX_BAR_CTRL_REG (PXBCR) */

#define PXBCR_BAR_EN				BIT0
#define PXBCR_BAR_SIZE_OFFS			16
#define PXBCR_BAR_SIZE_MASK			(0xffff << PXBCR_BAR_SIZE_OFFS)
#define PXBCR_BAR_SIZE_ALIGNMENT	0x10000



/* PCI Express Expansion ROM BAR Control Register */
/* PEX_EXP_ROM_BAR_CTRL_REG (PXERBCR) */

#define PXERBCR_EXPROM_EN			BIT0
#define PXERBCR_EXPROMSZ_OFFS		19
#define PXERBCR_EXPROMSZ_MASK		(0xf << PXERBCR_EXPROMSZ_OFFS)
#define PXERBCR_EXPROMSZ_512KB		(0x0 << PXERBCR_EXPROMSZ_OFFS)
#define PXERBCR_EXPROMSZ_1024KB		(0x1 << PXERBCR_EXPROMSZ_OFFS)
#define PXERBCR_EXPROMSZ_2048KB		(0x3 << PXERBCR_EXPROMSZ_OFFS)
#define PXERBCR_EXPROMSZ_4096KB		(0x7 << PXERBCR_EXPROMSZ_OFFS)

/************************************************/
/* PCI Express Address Window Control Registers */
/************************************************/
#define PEX_WIN0_3_CTRL_REG(pexIf,winNum)       \
                                (0x41820 + (winNum) * 0x10 - (pexIf) * 0x10000)
#define PEX_WIN0_3_BASE_REG(pexIf,winNum)       \
                                (0x41824 + (winNum) * 0x10 - (pexIf) * 0x10000)
#define PEX_WIN0_3_REMAP_REG(pexIf,winNum)      \
                                (0x4182C + (winNum) * 0x10 - (pexIf) * 0x10000)
#define PEX_WIN4_5_CTRL_REG(pexIf,winNum)       \
                            (0x41860 + (winNum - 4) * 0x20 - (pexIf) * 0x10000)
#define PEX_WIN4_5_BASE_REG(pexIf,winNum)       \
                            (0x41864 + (winNum - 4) * 0x20 - (pexIf) * 0x10000)
#define PEX_WIN4_5_REMAP_REG(pexIf,winNum)      \
                            (0x4186C + (winNum - 4) * 0x20 - (pexIf) * 0x10000)
#define PEX_WIN4_5_REMAP_HIGH_REG(pexIf,winNum) \
                            (0x41870 + (winNum - 4) * 0x20 - (pexIf) * 0x10000)

#define PEX_WIN_DEFAULT_CTRL_REG(pexIf)         (0x418B0 - (pexIf) * 0x10000)
#define PEX_WIN_EXP_ROM_CTRL_REG(pexIf)         (0x418C0 - (pexIf) * 0x10000)
#define PEX_WIN_EXP_ROM_REMAP_REG(pexIf)        (0x418C4 - (pexIf) * 0x10000)

/* PCI Express Window Control Register */
/* PEX_WIN_CTRL_REG (PXWCR) */

#define	PXWCR_WIN_EN					BIT0 /* Window Enable.*/

#define	PXWCR_WIN_BAR_MAP_OFFS			1    /* Mapping to BAR.*/
#define	PXWCR_WIN_BAR_MAP_MASK			BIT1
#define	PXWCR_WIN_BAR_MAP_BAR1			(0 << PXWCR_WIN_BAR_MAP_OFFS)
#define	PXWCR_WIN_BAR_MAP_BAR2			(1 << PXWCR_WIN_BAR_MAP_OFFS)

#define	PXWCR_TARGET_OFFS				4  /*Unit ID */
#define	PXWCR_TARGET_MASK				(0xf << PXWCR_TARGET_OFFS)

#define	PXWCR_ATTRIB_OFFS				8  /* target attributes */
#define	PXWCR_ATTRIB_MASK				(0xff << PXWCR_ATTRIB_OFFS)

#define	PXWCR_SIZE_OFFS					16 /* size */
#define	PXWCR_SIZE_MASK					(0xffff << PXWCR_SIZE_OFFS)
#define	PXWCR_SIZE_ALIGNMENT			0x10000

/* PCI Express Window Base Register */
/* PEX_WIN_BASE_REG (PXWBR)*/

#define PXWBR_BASE_OFFS					16 /* address[31:16] */
#define PXWBR_BASE_MASK					(0xffff << PXWBR_BASE_OFFS)
#define PXWBR_BASE_ALIGNMENT			0x10000

/* PCI Express Window Remap Register */
/* PEX_WIN_REMAP_REG (PXWRR)*/

#define PXWRR_REMAP_EN					BIT0
#define PXWRR_REMAP_OFFS				16
#define PXWRR_REMAP_MASK				(0xffff << PXWRR_REMAP_OFFS)
#define PXWRR_REMAP_ALIGNMENT			0x10000

/* PCI Express Window Remap (High) Register */
/* PEX_WIN_REMAP_HIGH_REG (PXWRHR)*/

#define PXWRHR_REMAP_HIGH_OFFS			0
#define PXWRHR_REMAP_HIGH_MASK			(0xffffffff << PXWRHR_REMAP_HIGH_OFFS)

/* PCI Express Default Window Control Register */
/* PEX_WIN_DEFAULT_CTRL_REG (PXWDCR) */

#define	PXWDCR_TARGET_OFFS				4  /*Unit ID */
#define	PXWDCR_TARGET_MASK				(0xf << PXWDCR_TARGET_OFFS)
#define	PXWDCR_ATTRIB_OFFS				8  /* target attributes */
#define	PXWDCR_ATTRIB_MASK				(0xff << PXWDCR_ATTRIB_OFFS)

/* PCI Express Expansion ROM Window Control Register */
/* PEX_WIN_EXP_ROM_CTRL_REG (PXWERCR)*/

#define	PXWERCR_TARGET_OFFS				4  /*Unit ID */
#define	PXWERCR_TARGET_MASK				(0xf << PXWERCR_TARGET_OFFS)
#define	PXWERCR_ATTRIB_OFFS				8  /* target attributes */
#define	PXWERCR_ATTRIB_MASK				(0xff << PXWERCR_ATTRIB_OFFS)

/* PCI Express Expansion ROM Window Remap Register */
/* PEX_WIN_EXP_ROM_REMAP_REG (PXWERRR)*/

#define PXWERRR_REMAP_EN				BIT0
#define PXWERRR_REMAP_OFFS				16
#define PXWERRR_REMAP_MASK				(0xffff << PXWERRR_REMAP_OFFS)
#define PXWERRR_REMAP_ALIGNMENT			0x10000



/*PEX_MEMORY_BAR_BASE_ADDR(barNum) (PXMBBA)*/ 
/* PCI Express BAR0 Internal Register*/
/*PEX BAR0_INTER_REG (PXBIR)*/

#define PXBIR_IOSPACE			BIT0	/* Memory Space Indicator */

#define PXBIR_TYPE_OFFS			1	   /* BAR Type/Init Val. */ 
#define PXBIR_TYPE_MASK			(0x3 << PXBIR_TYPE_OFFS)
#define PXBIR_TYPE_32BIT_ADDR	(0x0 << PXBIR_TYPE_OFFS)
#define PXBIR_TYPE_64BIT_ADDR	(0x2 << PXBIR_TYPE_OFFS)

#define PXBIR_PREFETCH_EN		BIT3 	/* Prefetch Enable */

#define PXBIR_BASE_OFFS		20		/* Base address. Address bits [31:20] */
#define PXBIR_BASE_MASK		(0xfff << PXBIR_BASE_OFFS)
#define PXBIR_BASE_ALIGNMET	(1 << PXBIR_BASE_OFFS)


/* PCI Express BAR0 Internal (High) Register*/
/*PEX BAR0_INTER_REG_HIGH (PXBIRH)*/      

#define PXBIRH_BASE_OFFS			0		/* Base address. Bits [63:32] */
#define PXBIRH_BASE_MASK			(0xffffffff << PBBHR_BASE_OFFS)


#define PEX_BAR_DEFAULT_ATTRIB		0xc /* Memory - Prefetch - 64 bit address */
#define PEX_BAR0_DEFAULT_ATTRIB	    PEX_BAR_DEFAULT_ATTRIB  
#define PEX_BAR1_DEFAULT_ATTRIB		PEX_BAR_DEFAULT_ATTRIB
#define PEX_BAR2_DEFAULT_ATTRIB		PEX_BAR_DEFAULT_ATTRIB


/* PCI Express BAR1 Register */
/*  PCI Express BAR2 Register*/
/*PEX BAR1_REG (PXBR)*/
/*PEX BAR2_REG (PXBR)*/

#define PXBR_IOSPACE			BIT0	/* Memory Space Indicator */

#define PXBR_TYPE_OFFS			1	   /* BAR Type/Init Val. */ 
#define PXBR_TYPE_MASK			(0x3 << PXBR_TYPE_OFFS)
#define PXBR_TYPE_32BIT_ADDR	(0x0 << PXBR_TYPE_OFFS)
#define PXBR_TYPE_64BIT_ADDR	(0x2 << PXBR_TYPE_OFFS)

#define PXBR_PREFETCH_EN		BIT3 	/* Prefetch Enable */

#define PXBR_BASE_OFFS		16		/* Base address. Address bits [31:16] */
#define PXBR_BASE_MASK		(0xffff << PXBR_BASE_OFFS)
#define PXBR_BASE_ALIGNMET	(1 << PXBR_BASE_OFFS)


/* PCI Express BAR1 (High) Register*/
/* PCI Express BAR2 (High) Register*/
/*PEX BAR1_REG_HIGH (PXBRH)*/
/*PEX BAR2_REG_HIGH (PXBRH)*/

#define PXBRH_BASE_OFFS			0		/* Base address. Address bits [63:32] */
#define PXBRH_BASE_MASK			(0xffffffff << PXBRH_BASE_OFFS)

/* PCI Express Expansion ROM BAR Register*/
/*PEX_EXPANSION_ROM_BASE_ADDR_REG (PXERBAR)*/

#define PXERBAR_EXPROMEN		BIT0	/* Expansion ROM Enable */

#define PXERBAR_BASE_512K_OFFS		19		/* Expansion ROM Base Address */
#define PXERBAR_BASE_512K_MASK		(0x1fff << PXERBAR_BASE_512K_OFFS) 	

#define PXERBAR_BASE_1MB_OFFS		20		/* Expansion ROM Base Address */
#define PXERBAR_BASE_1MB_MASK		(0xfff << PXERBAR_BASE_1MB_OFFS) 	

#define PXERBAR_BASE_2MB_OFFS		21		/* Expansion ROM Base Address */
#define PXERBAR_BASE_2MB_MASK		(0x7ff << PXERBAR_BASE_2MB_OFFS) 	

#define PXERBAR_BASE_4MB_OFFS		22		/* Expansion ROM Base Address */
#define PXERBAR_BASE_4MB_MASK		(0x3ff << PXERBAR_BASE_4MB_OFFS) 	

/* PEX Bar attributes */
typedef struct _mvPexBar
{
	MV_ADDR_WIN   addrWin;    /* An address window*/
	MV_BOOL       enable;     /* Address decode window is enabled/disabled    */

}MV_PEX_BAR;

/* PEX Remap Window attributes */
typedef struct _mvPexRemapWin
{
	MV_ADDR_WIN   addrWin;    /* An address window*/
	MV_BOOL       enable;     /* Address decode window is enabled/disabled    */

}MV_PEX_REMAP_WIN;

/* PEX Remap Window attributes */
typedef struct _mvPexDecWin
{
	MV_TARGET	  target;
	MV_ADDR_WIN   addrWin;    /* An address window*/
	MV_U32		  targetBar;
	MV_U8			attrib;			/* chip select attributes */
	MV_TARGET_ID 	targetId; 		/* Target Id of this MV_TARGET */
	MV_BOOL       enable;     /* Address decode window is enabled/disabled    */

}MV_PEX_DEC_WIN;

/* Global Functions prototypes */
/* mvPexHalInit - Initialize PEX interfaces*/
MV_STATUS mvPexInit(MV_U32 pexIf, MV_PEX_TYPE pexType);


/* mvPexTargetWinSet - Set PEX to peripheral target address window BAR*/
MV_STATUS mvPexTargetWinSet(MV_U32 pexIf, MV_U32 winNum, 
                            MV_PEX_DEC_WIN *pAddrDecWin);

/* mvPexTargetWinGet - Get PEX to peripheral target address window*/
MV_STATUS mvPexTargetWinGet(MV_U32 pexIf, MV_U32 winNum, 
                            MV_PEX_DEC_WIN *pAddrDecWin);

/* mvPexTargetWinEnable - Enable/disable a PEX BAR window*/
MV_STATUS mvPexTargetWinEnable(MV_U32 pexIf,MV_U32 winNum, MV_BOOL enable);

/* mvPexTargetWinRemap - Set PEX to target address window remap.*/
MV_STATUS mvPexTargetWinRemap(MV_U32 pexIf, MV_U32 winNum, 
                           MV_PEX_REMAP_WIN *pAddrWin);

/* mvPexTargetWinRemapEnable -enable\disable a PEX Window remap.*/
MV_STATUS mvPexTargetWinRemapEnable(MV_U32 pexIf, MV_U32 winNum, 
                           MV_BOOL enable);

/* mvPexBarSet - Set PEX bar address and size */
MV_STATUS mvPexBarSet(MV_U32 pexIf, MV_U32 barNum, MV_PEX_BAR *addrWin);

/* mvPexBarGet - Get PEX bar address and size */
MV_STATUS mvPexBarGet(MV_U32 pexIf, MV_U32 barNum, MV_PEX_BAR *addrWin);

/* mvPexBarEnable - enable\disable a PEX bar*/
MV_STATUS mvPexBarEnable(MV_U32 pexIf, MV_U32 barNum, MV_BOOL enable);

/* mvPexAddrDecShow - Display address decode windows attributes */
MV_VOID mvPexAddrDecShow(MV_VOID);

#endif

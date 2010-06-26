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

#ifndef __INCmvCtrlEnvRegsh
#define __INCmvCtrlEnvRegsh

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* CV Support */
#define PEX0_MEM0 	PEX0_MEM
#define PCI0_MEM0	PEX0_MEM

/* Controller revision info */
#define PCI_CLASS_CODE_AND_REVISION_ID			    0x008
#define PCCRIR_REVID_OFFS				    0		/* Revision ID */
#define PCCRIR_REVID_MASK				    (0xff << PCCRIR_REVID_OFFS)

/* Controler environment registers offsets */

/* Power Managment Control */
#define POWER_MNG_MEM_CTRL_REG			0x20118

#define PMC_GESTOPMEM_OFFS(port)		((port)? 13 : 0)
#define PMC_GESTOPMEM_MASK(port)		(1 << PMC_GESTOPMEM_OFFS(port))
#define PMC_GESTOPMEM_EN(port)			(0 << PMC_GESTOPMEM_OFFS(port))
#define PMC_GESTOPMEM_STOP(port)		(1 << PMC_GESTOPMEM_OFFS(port))

#define PMC_PEXSTOPMEM_OFFS			1
#define PMC_PEXSTOPMEM_MASK			(1 << PMC_PEXSTOPMEM_OFFS)
#define PMC_PEXSTOPMEM_EN			(0 << PMC_PEXSTOPMEM_OFFS)
#define PMC_PEXSTOPMEM_STOP			(1 << PMC_PEXSTOPMEM_OFFS)

#define PMC_USBSTOPMEM_OFFS			2
#define PMC_USBSTOPMEM_MASK			(1 << PMC_USBSTOPMEM_OFFS)
#define PMC_USBSTOPMEM_EN			(0 << PMC_USBSTOPMEM_OFFS)
#define PMC_USBSTOPMEM_STOP			(1 << PMC_USBSTOPMEM_OFFS)

#define PMC_DUNITSTOPMEM_OFFS			3
#define PMC_DUNITSTOPMEM_MASK			(1 << PMC_DUNITSTOPMEM_OFFS)
#define PMC_DUNITSTOPMEM_EN			(0 << PMC_DUNITSTOPMEM_OFFS)
#define PMC_DUNITSTOPMEM_STOP			(1 << PMC_DUNITSTOPMEM_OFFS)

#define PMC_RUNITSTOPMEM_OFFS			4
#define PMC_RUNITSTOPMEM_MASK			(1 << PMC_RUNITSTOPMEM_OFFS)
#define PMC_RUNITSTOPMEM_EN			(0 << PMC_RUNITSTOPMEM_OFFS)
#define PMC_RUNITSTOPMEM_STOP			(1 << PMC_RUNITSTOPMEM_OFFS)

#define PMC_XORSTOPMEM_OFFS(port)		(5+(port*2))
#define PMC_XORSTOPMEM_MASK(port)		(1 << PMC_XORSTOPMEM_OFFS(port))
#define PMC_XORSTOPMEM_EN(port)			(0 << PMC_XORSTOPMEM_OFFS(port))
#define PMC_XORSTOPMEM_STOP(port)		(1 << PMC_XORSTOPMEM_OFFS(port))

#define PMC_SATASTOPMEM_OFFS(port)		(6+(port*5))
#define PMC_SATASTOPMEM_MASK(port)		(1 << PMC_SATASTOPMEM_OFFS(port))
#define PMC_SATASTOPMEM_EN(port)		(0 << PMC_SATASTOPMEM_OFFS(port))
#define PMC_SATASTOPMEM_STOP(port)		(1 << PMC_SATASTOPMEM_OFFS(port))

#define PMC_SESTOPMEM_OFFS			8
#define PMC_SESTOPMEM_MASK			(1 << PMC_SESTOPMEM_OFFS)
#define PMC_SESTOPMEM_EN			(0 << PMC_SESTOPMEM_OFFS)
#define PMC_SESTOPMEM_STOP			(1 << PMC_SESTOPMEM_OFFS)

#define PMC_AUDIOSTOPMEM_OFFS			9
#define PMC_AUDIOSTOPMEM_MASK			(1 << PMC_AUDIOSTOPMEM_OFFS)
#define PMC_AUDIOSTOPMEM_EN			(0 << PMC_AUDIOSTOPMEM_OFFS)
#define PMC_AUDIOSTOPMEM_STOP			(1 << PMC_AUDIOSTOPMEM_OFFS)

#define POWER_MNG_CTRL_REG			0x2011C

#define PMC_GESTOPCLOCK_OFFS(port)		((port)? 19 : 0)
#define PMC_GESTOPCLOCK_MASK(port)		(1 << PMC_GESTOPCLOCK_OFFS(port))
#define PMC_GESTOPCLOCK_EN(port)		(1 << PMC_GESTOPCLOCK_OFFS(port))
#define PMC_GESTOPCLOCK_STOP(port)		(0 << PMC_GESTOPCLOCK_OFFS(port))

#define PMC_PEXPHYSTOPCLOCK_OFFS		1
#define PMC_PEXPHYSTOPCLOCK_MASK		(1 << PMC_PEXPHYSTOPCLOCK_OFFS)
#define PMC_PEXPHYSTOPCLOCK_EN			(1 << PMC_PEXPHYSTOPCLOCK_OFFS)
#define PMC_PEXPHYSTOPCLOCK_STOP		(0 << PMC_PEXPHYSTOPCLOCK_OFFS)

#define PMC_PEXSTOPCLOCK_OFFS			2
#define PMC_PEXSTOPCLOCK_MASK			(1 << PMC_PEXSTOPCLOCK_OFFS)
#define PMC_PEXSTOPCLOCK_EN			(1 << PMC_PEXSTOPCLOCK_OFFS)
#define PMC_PEXSTOPCLOCK_STOP			(0 << PMC_PEXSTOPCLOCK_OFFS)

#define PMC_USBSTOPCLOCK_OFFS			3
#define PMC_USBSTOPCLOCK_MASK			(1 << PMC_USBSTOPCLOCK_OFFS)
#define PMC_USBSTOPCLOCK_EN			(1 << PMC_USBSTOPCLOCK_OFFS)
#define PMC_USBSTOPCLOCK_STOP			(0 << PMC_USBSTOPCLOCK_OFFS)

#define PMC_SDIOSTOPCLOCK_OFFS			4
#define PMC_SDIOSTOPCLOCK_MASK			(1 << PMC_SDIOSTOPCLOCK_OFFS)
#define PMC_SDIOSTOPCLOCK_EN			(1 << PMC_SDIOSTOPCLOCK_OFFS)
#define PMC_SDIOSTOPCLOCK_STOP			(0 << PMC_SDIOSTOPCLOCK_OFFS)

#define PMC_TSSTOPCLOCK_OFFS			5
#define PMC_TSSTOPCLOCK_MASK			(1 << PMC_TSSTOPCLOCK_OFFS)
#define PMC_TSSTOPCLOCK_EN			(1 << PMC_TSSTOPCLOCK_OFFS)
#define PMC_TSSTOPCLOCK_STOP			(0 << PMC_TSSTOPCLOCK_OFFS)

#define PMC_AUDIOSTOPCLOCK_OFFS			9
#define PMC_AUDIOSTOPCLOCK_MASK			(1 << PMC_AUDIOSTOPCLOCK_OFFS)
#define PMC_AUDIOSTOPCLOCK_EN			(1 << PMC_AUDIOSTOPCLOCK_OFFS)
#define PMC_AUDIOSTOPCLOCK_STOP			(0 << PMC_AUDIOSTOPCLOCK_OFFS)

#define PMC_POWERSAVE_OFFS			11
#define PMC_POWERSAVE_MASK			(1 << PMC_POWERSAVE_OFFS)
#define PMC_POWERSAVE_EN			(1 << PMC_POWERSAVE_OFFS)
#define PMC_POWERSAVE_STOP			(0 << PMC_POWERSAVE_OFFS)




#define PMC_SATASTOPCLOCK_OFFS(port)		(14+(port))
#define PMC_SATASTOPCLOCK_MASK(port)		(1 << PMC_SATASTOPCLOCK_OFFS(port))
#define PMC_SATASTOPCLOCK_EN(port)		(1 << PMC_SATASTOPCLOCK_OFFS(port))
#define PMC_SATASTOPCLOCK_STOP(port)		(0 << PMC_SATASTOPCLOCK_OFFS(port))

#define PMC_SESTOPCLOCK_OFFS			17
#define PMC_SESTOPCLOCK_MASK			(1 << PMC_SESTOPCLOCK_OFFS)
#define PMC_SESTOPCLOCK_EN			(1 << PMC_SESTOPCLOCK_OFFS)
#define PMC_SESTOPCLOCK_STOP			(0 << PMC_SESTOPCLOCK_OFFS)

#define PMC_TDMSTOPCLOCK_OFFS			20
#define PMC_TDMSTOPCLOCK_MASK			(1 << PMC_TDMSTOPCLOCK_OFFS)
#define PMC_TDMSTOPCLOCK_EN			(1 << PMC_TDMSTOPCLOCK_OFFS)
#define PMC_TDMSTOPCLOCK_STOP			(0 << PMC_TDMSTOPCLOCK_OFFS)


/* Controler environment registers offsets */
#define MPP_CONTROL_REG0			0x10000
#define MPP_CONTROL_REG1			0x10004
#define MPP_CONTROL_REG2			0x10008
#define MPP_CONTROL_REG3			0x1000C
#define MPP_CONTROL_REG4			0x10010
#define MPP_CONTROL_REG5			0x10014
#define MPP_CONTROL_REG6			0x10018
#define MPP_SAMPLE_AT_RESET			0x10030
#define CHIP_BOND_REG				0x10034
#define SYSRST_LENGTH_COUNTER_REG		0x10050
#define SLCR_COUNT_OFFS				0
#define SLCR_COUNT_MASK				(0x1FFFFFFF << SLCR_COUNT_OFFS)
#define SLCR_CLR_OFFS				31
#define SLCR_CLR_MASK				(1 << SLCR_CLR_OFFS)			
#define PCKG_OPT_MASK				0x3
#define MPP_OUTPUT_DRIVE_REG			0x100E0
#define MPP_RGMII0_OUTPUT_DRIVE_OFFS            7
#define MPP_3_3_RGMII0_OUTPUT_DRIVE		(0x0 << MPP_RGMII0_OUTPUT_DRIVE_OFFS)
#define MPP_1_8_RGMII0_OUTPUT_DRIVE		(0x1 << MPP_RGMII0_OUTPUT_DRIVE_OFFS)
#define MPP_RGMII1_OUTPUT_DRIVE_OFFS            15
#define MPP_3_3_RGMII1_OUTPUT_DRIVE		(0x0 << MPP_RGMII1_OUTPUT_DRIVE_OFFS)
#define MPP_1_8_RGMII1_OUTPUT_DRIVE		(0x1 << MPP_RGMII1_OUTPUT_DRIVE_OFFS)

#define MSAR_BOOT_MODE_OFFS                     12
#define MSAR_BOOT_MODE_MASK                     (0x7 << MSAR_BOOT_MODE_OFFS)
#define MSAR_BOOT_NAND_WITH_BOOTROM		        (0x5 << MSAR_BOOT_MODE_OFFS)
#define MSAR_BOOT_SPI_WITH_BOOTROM              (0x4 << MSAR_BOOT_MODE_OFFS)
#define MSAR_BOOT_SPI_USE_NAND_WITH_BOOTROM		(0x2 << MSAR_BOOT_MODE_OFFS)

#define MSAR_BOOT_MODE_6180(X)                  (((X & 0x3000) >> 12) | \
                                                ((X & 0x2) << 1))
#define MSAR_BOOT_SPI_WITH_BOOTROM_6180         0x1
#define MSAR_BOOT_NAND_WITH_BOOTROM_6180        0x5

#define MSAR_TCLCK_OFFS				21
#define MSAR_TCLCK_MASK				(0x1 << MSAR_TCLCK_OFFS)
#define MSAR_TCLCK_166				(0x1 << MSAR_TCLCK_OFFS)
#define MSAR_TCLCK_200				(0x0 << MSAR_TCLCK_OFFS)


#define MSAR_CPUCLCK_EXTRACT(X)     (((X & 0x2) >> 1) | ((X & 0x400000) >> 21) | \
                                    ((X & 0x18) >> 1))

#define MSAR_CPUCLCK_OFFS_6180		2
#define MSAR_CPUCLCK_MASK_6180		(0x7 << MSAR_CPUCLCK_OFFS_6180)

#define MSAR_DDRCLCK_RTIO_OFFS		5
#define MSAR_DDRCLCK_RTIO_MASK		(0xF << MSAR_DDRCLCK_RTIO_OFFS)

#define MSAR_L2CLCK_EXTRACT(X)      (((X & 0x600) >> 9) | ((X & 0x80000) >> 17))

#ifndef MV_ASMLANGUAGE
/* CPU clock for 6281,6192  0->Resereved */
#define MV_CPU_CLCK_TBL { 	0,		0, 		0, 		0,	\
			     	600000000, 	0,		800000000,	1000000000,	\
			     	0,	 	1200000000,	0,		0,		\
			     	1500000000,	0,		0,		0}

/* DDR clock RATIO for 6281,6192 {0,0}->Reserved */
#define MV_DDR_CLCK_RTIO_TBL	{\
	{0, 0}, {0, 0}, {2, 1}, {0, 0}, \
	{3, 1}, {0, 0}, {4, 1}, {9, 2}, \
	{5, 1}, {6, 1}, {0, 0}, {0, 0}, \
	{0, 0}, {0, 0}, {0, 0}, {0, 0} \
}

/* L2 clock RATIO for 6281,6192 {1,1}->Reserved */
#define MV_L2_CLCK_RTIO_TBL	{\
	{0, 0}, {2, 1}, {0, 0}, {3, 1}, \
	{0, 0}, {0, 0}, {0, 0}, {0, 0} \
}

/* 6180 have different clk reset sampling 		*/
/* ARM CPU, DDR, L2 clock for 6180 {0,0,0}->Reserved 	*/
#define MV_CPU6180_DDR_L2_CLCK_TBL    { \
	{0,   		0,   		0		},\
	{0,   		0,   		0		},\
	{0,   		0,   		0		},\
	{0,   		0,   		0		},\
	{0,   		0,   		0		},\
	{600000000, 	200000000, 	300000000	},\
	{800000000, 	200000000, 	400000000	},\
	{0,   		0,   		0		}\
}



/* These macros help units to identify a target Mbus Arbiter group */
#define MV_TARGET_IS_DRAM(target)   \
                            ((target >= SDRAM_CS0) && (target <= SDRAM_CS3))

#define MV_TARGET_IS_PEX0(target)   \
                            ((target >= PEX0_MEM) && (target <= PEX0_IO))

#define MV_TARGET_IS_PEX1(target)   0

#define MV_TARGET_IS_PEX(target) (MV_TARGET_IS_PEX0(target) || MV_TARGET_IS_PEX1(target))

#define MV_TARGET_IS_DEVICE(target) \
                            ((target >= DEVICE_CS0) && (target <= DEVICE_CS3))

#define MV_PCI_DRAM_BAR_TO_DRAM_TARGET(bar)   0

#define	MV_TARGET_IS_AS_BOOT(target) ((target) == (sampleAtResetTargetArray[ \
                     (mvCtrlModelGet() == MV_6180_DEV_ID)? MSAR_BOOT_MODE_6180 \
                     (MV_REG_READ(MPP_SAMPLE_AT_RESET)):((MV_REG_READ(MPP_SAMPLE_AT_RESET)\
						 & MSAR_BOOT_MODE_MASK) >> MSAR_BOOT_MODE_OFFS)]))


#define MV_CHANGE_BOOT_CS(target)	(((target) == DEV_BOOCS)?\
					sampleAtResetTargetArray[(mvCtrlModelGet() == MV_6180_DEV_ID)? \
                    MSAR_BOOT_MODE_6180(MV_REG_READ(MPP_SAMPLE_AT_RESET)): \
                    ((MV_REG_READ(MPP_SAMPLE_AT_RESET) & MSAR_BOOT_MODE_MASK)\
                     >> MSAR_BOOT_MODE_OFFS)]:(target))
					
#define TCLK_TO_COUNTER_RATIO   1   /* counters running in Tclk */

#define BOOT_TARGETS_NAME_ARRAY {       \
    TBL_TERM,         	\
    TBL_TERM,         	\
    BOOT_ROM_CS,          	\
    TBL_TERM,         	\
    BOOT_ROM_CS,          	\
    BOOT_ROM_CS,          \
    TBL_TERM,         	\
    TBL_TERM           \
}

#define BOOT_TARGETS_NAME_ARRAY_6180 {       \
    TBL_TERM,         	\
    BOOT_ROM_CS,          	\
    TBL_TERM,           \
    TBL_TERM,           \
    TBL_TERM,           \
    BOOT_ROM_CS,          \
    TBL_TERM,           \
    TBL_TERM           \
}


/* For old competability */
#define DEVICE_CS0		NFLASH_CS  
#define DEVICE_CS1  		SPI_CS 
#define DEVICE_CS2  		BOOT_ROM_CS 
#define DEVICE_CS3  		DEV_BOOCS
#define MV_BOOTDEVICE_INDEX   	0

#define START_DEV_CS   		DEV_CS0
#define DEV_TO_TARGET(dev)	((dev) + DEVICE_CS0)

#define PCI_IF0_MEM0		PEX0_MEM
#define PCI_IF0_IO		PEX0_IO


/* This enumerator defines the Marvell controller target ID      */ 
typedef enum _mvTargetId
{
    DRAM_TARGET_ID  = 0 ,    /* Port 0 -> DRAM interface         */
    DEV_TARGET_ID   = 1,     /* Port 1 -> Nand/SPI 		*/
    PEX0_TARGET_ID  = 4 ,    /* Port 4 -> PCI Express0 		*/
    CRYPT_TARGET_ID = 3 ,    /* Port 3 --> Crypto Engine 	*/
    SAGE_TARGET_ID = 12 ,    /* Port 12 -> SAGE Unit 	*/
    MAX_TARGETS_ID
}MV_TARGET_ID;


/* This enumerator described the possible Controller paripheral targets.    */
/* Controller peripherals are designated memory/IO address spaces that the  */
/* controller can access. They are also refered as "targets"                */
typedef enum _mvTarget
{
    TBL_TERM = -1, 	/* none valid target, used as targets list terminator*/
    SDRAM_CS0,      	/* SDRAM chip select 0                          */  
    SDRAM_CS1,      	/* SDRAM chip select 1                          */  
    SDRAM_CS2,      	/* SDRAM chip select 2                          */  
    SDRAM_CS3,      	/* SDRAM chip select 3                          */  
    PEX0_MEM,		/* PCI Express 0 Memory				*/
    PEX0_IO,		/* PCI Express 0 IO				*/
    INTER_REGS,     	/* Internal registers                           */  
    NFLASH_CS,     	/* NFLASH_CS					*/  
    SPI_CS,     	/* SPI_CS					*/  
    BOOT_ROM_CS,        /* BOOT_ROM_CS                                  */  
    DEV_BOOCS,     	/* DEV_BOOCS					*/  
    CRYPT_ENG,      	/* Crypto Engine				*/  
#ifdef MV_INCLUDE_SAGE
    SAGE_UNIT,      	/* SAGE Unit					*/  
#endif
    MAX_TARGETS

}MV_TARGET;

#define TARGETS_DEF_ARRAY	{			\
    {0x0E, DRAM_TARGET_ID }, /* SDRAM_CS0 */		\
    {0x0D, DRAM_TARGET_ID }, /* SDRAM_CS1 */		\
    {0x0B, DRAM_TARGET_ID }, /* SDRAM_CS0 */		\
    {0x07, DRAM_TARGET_ID }, /* SDRAM_CS1 */		\
    {0xE8, PEX0_TARGET_ID }, /* PEX0_MEM */			\
    {0xE0, PEX0_TARGET_ID }, /* PEX0_IO */			\
    {0xFF, 0xFF           }, /* INTER_REGS */		\
    {0x2F, DEV_TARGET_ID  },  /* NFLASH_CS */		\
    {0x1E, DEV_TARGET_ID  },  /* SPI_CS */		 	\
    {0x1D, DEV_TARGET_ID  },  /* BOOT_ROM_CS */     \
    {0x1E, DEV_TARGET_ID  },  /* DEV_BOOCS */		\
    {0x01, CRYPT_TARGET_ID}, /* CRYPT_ENG */        \
    {0x00, SAGE_TARGET_ID }  						\
}


#define TARGETS_NAME_ARRAY	{	\
    "SDRAM_CS0",    /* SDRAM_CS0 */	\
    "SDRAM_CS1",    /* SDRAM_CS1 */	\
    "SDRAM_CS2",    /* SDRAM_CS2 */	\
    "SDRAM_CS3",    /* SDRAM_CS3 */	\
    "PEX0_MEM",	    /* PEX0_MEM */	\
    "PEX0_IO",	    /* PEX0_IO */	\
    "INTER_REGS",   /* INTER_REGS */	\
    "NFLASH_CS",    /* NFLASH_CS */	\
    "SPI_CS",	    /* SPI_CS */	\
    "BOOT_ROM_CS",  /* BOOT_ROM_CS */ \
    "DEV_BOOTCS",   /* DEV_BOOCS */	\
    "CRYPT_ENG",    /* CRYPT_ENG */  \
    "SAGE_UNIT"	   /* SAGE_UNIT */	\
}
#endif /* MV_ASMLANGUAGE */


#endif

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

#define	_ASMLANGUAGE
#define MV_ASMLANGUAGE
#include "mvSysHwConfig.h"
#include "mvOsAsm.h"
#include "boardEnv/mvBoardEnvSpec.h"
#include "ctrlEnv/sys/mvCpuIfRegs.h"
#include "mvDramIfConfig.h"
#include "mvDramIfRegs.h"
#include "pex/mvPexRegs.h"
#include "ctrlEnv/mvCtrlEnvSpec.h"
#include "ctrlEnv/mvCtrlEnvAsm.h"
#include "mvCommon.h"

/* defines */

#if defined(MV_STATIC_DRAM_ON_BOARD) 
.globl dramBoot1
dramBoot1:
        .word   0

/******************************************************************************
*
*
*
*
*******************************************************************************/
#if defined(DB_MV78XX0) || defined(DB_MV88F632X)
/* DDR2 boards 512MB 333MHz */
#define STATIC_SDRAM0_BANK0_SIZE		0x1ffffff1 /*	0x1504	*/ 
#define STATIC_SDRAM_CONFIG	     		0x43048C30 /*	0x1400  */	
#define STATIC_SDRAM_MODE	     		0x00000652 /*	0x141c  */	
#define STATIC_DUNIT_CTRL_LOW			0x38543000 /*   0x1404  */  
#define STATIC_DUNIT_CTRL_HI			0x0000FFFF /*   0x1424  */  
#define STATIC_SDRAM_ADDR_CTRL			0x00000088 /*   0x1410  */  
#define STATIC_SDRAM_TIME_CTRL_LOW		0x22125441 /*   0x1408  */  
#define STATIC_SDRAM_TIME_CTRL_HI		0x00000A29 /*   0x140c  */  
#define STATIC_SDRAM_ODT_CTRL_LOW	     	0x84210000 /*   0x1494  */  
#define STATIC_SDRAM_ODT_CTRL_HI	     	0x00000000 /*   0x1498  */  
#define STATIC_SDRAM_DUNIT_ODT_CTRL    		0x0000E80F /*   0x149c  */  
#define STATIC_SDRAM_EXT_MODE          		0x00000040 /*   0x1420  */  
#define STATIC_SDRAM_DDR2_TIMING_LO		0x00085520 /*   0x1428  */  
#define STATIC_SDRAM_DDR2_TIMING_HI		0x00008552 /*   0x147C  */  

#elif defined(RD_MV78XX0_AMC)
/* On board DDR2 512MB 400MHz CL5 */
#define STATIC_SDRAM0_BANK0_SIZE		0x1ffffff1 /*	0x1504	*/ 
#define STATIC_SDRAM_CONFIG	     		0x43008C30 /*	0x1400  */	
#define STATIC_SDRAM_MODE	     		0x00000652 /*	0x141c  */	
#define STATIC_DUNIT_CTRL_LOW			0x38543000 /*   0x1404  */  
#define STATIC_DUNIT_CTRL_HI			0x0000F07F /*   0x1424  */  
#define STATIC_SDRAM_ADDR_CTRL			0x000000DD /*   0x1410  */  
#define STATIC_SDRAM_TIME_CTRL_LOW		0x23135441 /*   0x1408  */  
#define STATIC_SDRAM_TIME_CTRL_HI		0x00000A32 /*   0x140c  */  
#define STATIC_SDRAM_ODT_CTRL_LOW	     	0x84210000 /*   0x1494  */  
#define STATIC_SDRAM_ODT_CTRL_HI	     	0x00000000 /*   0x1498  */  
#define STATIC_SDRAM_DUNIT_ODT_CTRL    		0x0000EB0F /*   0x149c  */  
#define STATIC_SDRAM_EXT_MODE          		0x00000040 /*   0x1420  */  
#define STATIC_SDRAM_DDR2_TIMING_LO		0x00085520 /*   0x1428  */  
#define STATIC_SDRAM_DDR2_TIMING_HI		0x00008552 /*   0x147C  */  

#elif defined(RD_MV78XX0_H3C)
/* DDR2 boards 512MB 333MHz */
#define STATIC_SDRAM0_BANK0_SIZE		0x1ffffff1 /*	0x1504	*/ 
#define STATIC_SDRAM_CONFIG	     		0x43048a25 /*	0x1400  */	
#define STATIC_SDRAM_MODE	     		0x00000652 /*	0x141c  */	
#define STATIC_DUNIT_CTRL_LOW			0x38543000 /*   0x1404  */  
#define STATIC_DUNIT_CTRL_HI			0x0000F07F /*   0x1424  */  
#define STATIC_SDRAM_ADDR_CTRL			0x00000088 /*   0x1410  */  
#define STATIC_SDRAM_TIME_CTRL_LOW		0x2202444e /*   0x1408  */  
#define STATIC_SDRAM_TIME_CTRL_HI		0x00000A22 /*   0x140c  */  
#define STATIC_SDRAM_ODT_CTRL_LOW	     	0x84210000 /*   0x1494  */  
#define STATIC_SDRAM_ODT_CTRL_HI	     	0x00000000 /*   0x1498  */  
#define STATIC_SDRAM_DUNIT_ODT_CTRL    		0x0000EB0F /*   0x149c  */  
#define STATIC_SDRAM_EXT_MODE          		0x00000040 /*   0x1420  */  
#define STATIC_SDRAM_DDR2_TIMING_LO		0x00085520 /*   0x1428  */  
#define STATIC_SDRAM_DDR2_TIMING_HI		0x00008552 /*   0x147C  */  

#elif defined(RD_MV78XX0_PCAC)
/* DDR2 boards 256MB 200MHz */
#define STATIC_SDRAM0_BANK0_SIZE		0x0ffffff1 /*	0x1504	*/ 
#define STATIC_SDRAM_CONFIG	     		0x43000a25 /*	0x1400  */	
#define STATIC_SDRAM_MODE	     		0x00000652 /*	0x141c  */	
#define STATIC_DUNIT_CTRL_LOW			0x38543000 /*   0x1404  */  
#define STATIC_DUNIT_CTRL_HI			0x0000F07F /*   0x1424  */  
#define STATIC_SDRAM_ADDR_CTRL			0x000000DD /*   0x1410  */  
#define STATIC_SDRAM_TIME_CTRL_LOW		0x2202444e /*   0x1408  */  
#define STATIC_SDRAM_TIME_CTRL_HI		0x00000822 /*   0x140c  */  
#define STATIC_SDRAM_ODT_CTRL_LOW	     	0x84210000 /*   0x1494  */  
#define STATIC_SDRAM_ODT_CTRL_HI	     	0x00000000 /*   0x1498  */  
#define STATIC_SDRAM_DUNIT_ODT_CTRL    		0x0000EB0F /*   0x149c  */  
#define STATIC_SDRAM_EXT_MODE          		0x00000040 /*   0x1420  */  
#define STATIC_SDRAM_DDR2_TIMING_LO		0x00085520 /*   0x1428  */  
#define STATIC_SDRAM_DDR2_TIMING_HI		0x00008552 /*   0x147C  */  

#else
/* DDR2 MV88F6281 boards 256MB 400MHz */
#define STATIC_SDRAM0_BANK0_SIZE		0x0FFFFFF1 /*	0x1504	*/ 
#define STATIC_SDRAM_CONFIG	     		0x43000c30 /*	0x1400  */	
#define STATIC_SDRAM_MODE	     		0x00000C52 /*	0x141c  */	
#define STATIC_DUNIT_CTRL_LOW			0x39543000 /*   0x1404  */  
#define STATIC_DUNIT_CTRL_HI			0x0000F1FF /*   0x1424  */  
#define STATIC_SDRAM_ADDR_CTRL			0x000000cc /*   0x1410  */  
#define STATIC_SDRAM_TIME_CTRL_LOW		0x22125451 /*   0x1408  */  
#define STATIC_SDRAM_TIME_CTRL_HI		0x00000A33 /*   0x140c  */  
#define STATIC_SDRAM_ODT_CTRL_LOW	     	0x003C0000 /*   0x1494  */  
#define STATIC_SDRAM_ODT_CTRL_HI	     	0x00000000 /*   0x1498  */  
#define STATIC_SDRAM_DUNIT_ODT_CTRL    		0x0000F80F /*   0x149c  */  
#define STATIC_SDRAM_EXT_MODE          		0x00000042 /*   0x1420  */  
#define STATIC_SDRAM_DDR2_TIMING_LO		0x00085520 /*   0x1428  */  
#define STATIC_SDRAM_DDR2_TIMING_HI		0x00008552 /*   0x147C  */  
#endif /* MV78XX0 */

	.globl _mvDramIfStaticInit
_mvDramIfStaticInit:

	mov     r11, LR     		/* Save link register */
	mov	r10, r2

#ifdef MV78XX0
        MV_REG_READ_ASM (r6, r5, SDRAM_DUNIT_CTRL_REG)
	orr	r6, r6, #BIT4	/* Enable 2T mode */
	bic	r6, r6, #BIT6	/* clear ctrlPos */
        MV_REG_WRITE_ASM (r6, r5, SDRAM_DUNIT_CTRL_REG)
#endif

     	/*DDR SDRAM Initialization Control */
	ldr	r6, =DSICR_INIT_EN
	MV_REG_WRITE_ASM (r6, r1, DDR_SDRAM_INIT_CTRL_REG)
2:	MV_REG_READ_ASM (r6, r1, DDR_SDRAM_INIT_CTRL_REG)
         and    r6, r6, #DSICR_INIT_EN
         cmp    r6, #0
         bne 2b

        /* If we boot from NAND jump to DRAM address */
        mov     r5, #1
        ldr     r6, =dramBoot1
        str     r5, [r6]                /* We started executing from DRAM */

        ldr     r6, dramBoot1
        cmp     r6, #0
        bne     1f

	/* set all dram windows to 0 */
	mov	r6, #0
	MV_REG_WRITE_ASM(r6, r5, SDRAM_SIZE_REG(0,0))
	MV_REG_WRITE_ASM(r6, r5, SDRAM_SIZE_REG(0,1))
	MV_REG_WRITE_ASM(r6, r5, SDRAM_SIZE_REG(0,2))
	MV_REG_WRITE_ASM(r6, r5, SDRAM_SIZE_REG(0,3))
	ldr	r6, = STATIC_SDRAM0_BANK0_SIZE
	MV_REG_WRITE_ASM(r6, r5, SDRAM_SIZE_REG(0,0))


	/* set all dram configuration in temp registers */
	ldr	r6, = STATIC_SDRAM0_BANK0_SIZE
	MV_REG_WRITE_ASM(r6, r5, DRAM_BUF_REG0)
	ldr	r6, = STATIC_SDRAM_CONFIG
	MV_REG_WRITE_ASM(r6, r5, DRAM_BUF_REG1)
	ldr	r6, = STATIC_SDRAM_MODE
	MV_REG_WRITE_ASM(r6, r5, DRAM_BUF_REG2)
	ldr	r6, = STATIC_DUNIT_CTRL_LOW
	MV_REG_WRITE_ASM(r6, r5, DRAM_BUF_REG3)
	ldr	r6, = STATIC_SDRAM_ADDR_CTRL
	MV_REG_WRITE_ASM(r6, r5, DRAM_BUF_REG4)
	ldr	r6, = STATIC_SDRAM_TIME_CTRL_LOW
	MV_REG_WRITE_ASM(r6, r5, DRAM_BUF_REG5)
	ldr	r6, = STATIC_SDRAM_TIME_CTRL_HI
	MV_REG_WRITE_ASM(r6, r5, DRAM_BUF_REG6)
	ldr	r6, = STATIC_SDRAM_ODT_CTRL_LOW
	MV_REG_WRITE_ASM(r6, r5, DRAM_BUF_REG7)
	ldr	r6, = STATIC_SDRAM_ODT_CTRL_HI
	MV_REG_WRITE_ASM(r6, r5, DRAM_BUF_REG8)
	ldr	r6, = STATIC_SDRAM_DUNIT_ODT_CTRL
	MV_REG_WRITE_ASM(r6, r5, DRAM_BUF_REG9)
	ldr	r6, = STATIC_SDRAM_EXT_MODE
	MV_REG_WRITE_ASM(r6, r5, DRAM_BUF_REG10)
	ldr	r6, = STATIC_SDRAM_DDR2_TIMING_LO
	MV_REG_WRITE_ASM(r6, r5, DRAM_BUF_REG11)
	ldr	r6, = STATIC_SDRAM_DDR2_TIMING_HI
	MV_REG_WRITE_ASM(r6, r5, DRAM_BUF_REG12)
#ifndef MV_NAND_BOOT
	ldr	r6, = STATIC_DUNIT_CTRL_HI
	MV_REG_WRITE_ASM(r6, r5, DRAM_BUF_REG13)
#endif

	ldr	sp,=0
	bl	_mvDramIfConfig
	ldr	r0, =0
#ifdef MV78XX0
	bl	_mvDramIfEccMemInit 
#endif
1:
	mov 	r2, r10
	mov     PC, r11         	/* r11 is saved link register */

#else  /* #if defined(MV_STATIC_DRAM_ON_BOARD) */

.globl dramBoot1
dramBoot1:
        .word   0

/*******************************************************************************
* mvDramIfBasicInit - Basic initialization of DRAM interface
*
* DESCRIPTION:
*       The function will initialize the DRAM for basic usage. The function
*       will use the TWSI assembly API to extract DIMM parameters according
*       to which DRAM interface will be initialized.
*       The function referes to the following DRAM parameters:
*       1) DIMM is registered or not.
*       2) DIMM width detection.
*       3) DIMM density.
*
* INPUT:
*       r3 - required size for initial DRAM.
*
* OUTPUT:
*       None.
*
* RETURN:
*       None.
*
*       Note:
*       r4 holds I2C EEPROM address
*       r5 holds SDRAM register base address
*	r7 holds returned values
*       r8 holds SDRAM various configuration registers value.
*       r11 holds return function address.
*******************************************************************************/
/* Setting the offsets of the I2C registers */
#define DIMM_TYPE_OFFSET	      2
#define NUM_OF_ROWS_OFFSET            3
#define NUM_OF_COLS_OFFSET            4
#define NUM_OF_RANKS		      5
#define DIMM_CONFIG_TYPE             11
#define SDRAM_WIDTH_OFFSET           13
#define NUM_OF_BANKS_OFFSET          17
#define SUPPORTED_CL_OFFSET          18
#define DIMM_TYPE_INFO_OFFSET        20         /* DDR2 only    */
#define SDRAM_MODULES_ATTR_OFFSET    21
#define RANK_SIZE_OFFSET             31

#define DRAM_DEV_DENSITY_128M         128
#define DRAM_DEV_DENSITY_256M         256
#define DRAM_DEV_DENSITY_512M         512
#define DRAM_DEV_DENSITY_1G          1024
#define DRAM_DEV_DENSITY_2G          2048

#define DRAM_RANK_DENSITY_128M       0x20
#define DRAM_RANK_DENSITY_256M       0x40
#define DRAM_RANK_DENSITY_512M       0x80
#define DRAM_RANK_DENSITY_1G	     0x1
#define DRAM_RANK_DENSITY_2G	     0x2

       .globl _mvDramIfBasicInit
       .extern _i2cInit
_mvDramIfBasicInit:

        mov     r11, LR     		/* Save link register */

	/* Set Dunit high control register	      */
        MV_REG_READ_ASM (r6, r5, SDRAM_DUNIT_CTRL_HI_REG)
	orr	r6, r6, #BIT7 /* SDRAM__D2P_EN */
	orr	r6, r6, #BIT8 /* SDRAM__P2D_EN */
#ifdef MV78XX0
	orr	r6, r6, #BIT9 /* SDRAM__ADD_HALF_FCC_EN */
	orr	r6, r6, #BIT10 /* SDRAM__PUP_ZERO_SKEW_EN */
	orr	r6, r6, #BIT11 /* SDRAM__WR_MASH_DELAY_EN */
#endif
        MV_REG_WRITE_ASM (r6, r5, SDRAM_DUNIT_CTRL_HI_REG)

#ifdef MV78XX0
        MV_REG_READ_ASM (r6, r5, SDRAM_DUNIT_CTRL_REG)
	orr	r6, r6, #BIT4	/* Enable 2T mode */
	bic	r6, r6, #BIT6	/* clear ctrlPos */
        MV_REG_WRITE_ASM (r6, r5, SDRAM_DUNIT_CTRL_REG)
#endif

     	/*DDR SDRAM Initialization Control */
	ldr	r6, =DSICR_INIT_EN
	MV_REG_WRITE_ASM (r6, r1, DDR_SDRAM_INIT_CTRL_REG)
2:	MV_REG_READ_ASM (r6, r1, DDR_SDRAM_INIT_CTRL_REG)
         and    r6, r6, #DSICR_INIT_EN
         cmp    r6, #0
         bne 2b

        mov     r5, #1
        ldr     r8, =dramBoot1
        str     r5, [r8]                /* We started executing from DRAM */

        /* If we boot from NAND jump to DRAM address */
        ldr     r8, dramBoot1
        cmp     r8, #0
        movne   pc, r11

        bl      _i2cInit                /* Initialize TWSI master             */

        /* Check if we have more then 1 dimm */
	ldr	r6, =0
	MV_REG_WRITE_ASM (r6, r1, DRAM_BUF_REG14)
#ifdef MV78XX0
	bl _is_Second_Dimm_Exist
	beq single_dimm
	ldr	r6, =1
	MV_REG_WRITE_ASM (r6, r1, DRAM_BUF_REG14)
single_dimm:
        bl      _i2cInit                /* Initialize TWSI master             */
#endif

        /* Get default SDRAM Config values */
        MV_REG_READ_ASM (r8, r5, SDRAM_CONFIG_REG)

        /* Get registered/non registered info from DIMM */
	bl  	_is_Registered
        beq     nonRegistered

setRegistered:
        orr     r8, r8, #SDRAM_REGISTERED   /* Set registered bit(17)         */
nonRegistered:
#ifdef MV78XX0
        /* Get ECC/non ECC info from DIMM */
	bl  	_is_Ecc
        beq     setConfigReg

setEcc:        
        orr     r8, r8, #SDRAM_ECC_EN   /* Set ecc bit(18)         */
#endif
setConfigReg:
        MV_REG_WRITE_ASM (r8, r5, DRAM_BUF_REG1)

        /* Set maximum CL supported by DIMM */
	bl	_get_CAL

        /* r7 is DIMM supported CAS (e.g: 3 --> 0x1C)                         */
        clz     r6, r7
        rsb     r6, r6, #31     /* r6 = the bit number of MAX CAS supported   */

casDdr2:
	ldr     r7, =0x41        /* stBurstInDel|stBurstOutDel field value     */
	ldr     r3, =0x53       /* stBurstInDel|stBurstOutDel registered value*/
	ldr     r8, =0x32      /* Assuming MAX CL = 3           */
        cmp     r6, #3          /* If CL = 3 break              */
        beq     casDdr2Cont

	ldr     r7, =0x53        /* stBurstInDel|stBurstOutDel field value     */
	ldr     r3, =0x65       /* stBurstInDel|stBurstOutDel registered value*/
	ldr     r8, =0x42      /* Assuming MAX CL = 4           */
        cmp     r6, #4          /* If CL = 4 break              */
        beq     casDdr2Cont

	ldr     r7, =0x65        /* stBurstInDel|stBurstOutDel field value     */
	ldr     r3, =0x77       /* stBurstInDel|stBurstOutDel registered value*/
	ldr     r8, =0x52      /* Assuming MAX CL = 5           */
        cmp     r6, #5          /* If CL = 5 break              */
        beq     casDdr2Cont

	ldr     r7, =0x77        /* stBurstInDel|stBurstOutDel field value     */
	ldr     r3, =0x89       /* stBurstInDel|stBurstOutDel registered value*/
	ldr     r8, =0x62      /* Assuming MAX CL = 6           */
        cmp     r6, #6          /* If CL = 5 break              */
        beq     casDdr2Cont                

        /* This is an error. return */
        b       exit_ddrAutoConfig      /* This is an error !!  */
casDdr2Cont:

        /* Get default SDRAM Mode values */
        MV_REG_READ_ASM (r6, r5, SDRAM_MODE_REG)
        bic     r6, r6, #(BIT6 | BIT5 | BIT4) /* Clear CL filed */
	orr	r6, r6, r8
        MV_REG_WRITE_ASM (r6, r5, DRAM_BUF_REG2)

	/* Set Dunit control register according to max CL detected	      */
        MV_REG_READ_ASM (r6, r5, DRAM_BUF_REG1)
	tst	r6, #SDRAM_REGISTERED
	beq	setDunitReg
	mov	r7, r3

setDunitReg:
#ifdef MV78XX0
        /* Set SDRAM Extended Mode register for double DIMM */
	/* Check DRAM frequency for more then 267MHz set ODT Rtt to 50ohm */

        MV_REG_READ_ASM (r4, r5, CPU_RESET_SAMPLE_L_REG)
	ldr	r5, =MSAR_SYSCLCK_MASK
	and	r4, r4, r5	
	ldr	r5, =MSAR_SYSCLCK_333
	cmp	r4, r5
	ble	Clock333
	add r7, r7, #0x10
Clock333:
#endif

        MV_REG_READ_ASM (r6, r5, SDRAM_DUNIT_CTRL_REG)
	bic	r6, r6, #(0xff << 20) /* Clear SBout and SBin */
	orr	r6, r6, #BIT4	/* Enable 2T mode */
	bic	r6, r6, #BIT6	/* clear ctrlPos */
	orr	r6, r6, r7, LSL #20
        MV_REG_WRITE_ASM (r6, r5, DRAM_BUF_REG3)

	/* Set Dunit high control register	      */
        MV_REG_READ_ASM (r6, r5, SDRAM_DUNIT_CTRL_HI_REG)
	orr	r6, r6, #BIT7 /* SDRAM__D2P_EN */
	orr	r6, r6, #BIT8 /* SDRAM__P2D_EN */
#ifdef MV78XX0
	orr	r6, r6, #BIT9 /* SDRAM__ADD_HALF_FCC_EN */
	orr	r6, r6, #BIT10 /* SDRAM__PUP_ZERO_SKEW_EN */
	orr	r6, r6, #BIT11 /* SDRAM__WR_MASH_DELAY_EN */
#endif
        MV_REG_WRITE_ASM (r6, r5, DRAM_BUF_REG13)

        /* DIMM density configuration*/
        /* Density = (1 << (rowNum + colNum)) * dramWidth * dramBankNum       */
Density:
	/* Get bank 0 and 1 density */
	ldr	r6, =0
	bl 	_getDensity

	mov 	r8, r7
        mov     r8, r8, LSR #20 /* Move density 20 bits to the right  */
                                /* For example 0x10000000 --> 0x1000 */

        mov     r3, #(SDRAM_DSIZE_256Mb(0) | SDRAM_DSIZE_256Mb(1))
        cmp     r8, #DRAM_DEV_DENSITY_256M
        beq     get_bank_2_density

        mov     r3, #(SDRAM_DSIZE_512Mb(0) | SDRAM_DSIZE_512Mb(1))
        cmp     r8, #DRAM_DEV_DENSITY_512M
        beq     get_bank_2_density

        mov     r3, #(SDRAM_DSIZE_1Gb(0) | SDRAM_DSIZE_1Gb(1))
        cmp     r8, #DRAM_DEV_DENSITY_1G
        beq     get_bank_2_density

        mov     r3, #(SDRAM_DSIZE_2Gb(0) | SDRAM_DSIZE_2Gb(1))
        cmp     r8, #DRAM_DEV_DENSITY_2G
        beq     get_bank_2_density

        /* This is an error. return */
        b       exit_ddrAutoConfig

get_bank_2_density:
	/* Check for second dimm */
	MV_REG_READ_ASM (r6, r1, DRAM_BUF_REG14)
	cmp	r6, #1
	bne 	get_width

	/* Get bank 2 and 3 density */
	ldr	r6, =2
	bl 	_getDensity

	mov 	r8, r7
        mov     r8, r8, LSR #20 /* Move density 20 bits to the right  */
                                /* For example 0x10000000 --> 0x1000 */

        orr     r3, r3, #(SDRAM_DSIZE_256Mb(2) | SDRAM_DSIZE_256Mb(3))
        cmp     r8, #DRAM_DEV_DENSITY_256M
        beq     get_width

        and     r3, r3, #~(SDRAM_DSIZE_MASK(2) | SDRAM_DSIZE_MASK(3))
        orr     r3, r3, #(SDRAM_DSIZE_512Mb(2) | SDRAM_DSIZE_512Mb(3))
        cmp     r8, #DRAM_DEV_DENSITY_512M
        beq     get_width

        and     r3, r3, #~(SDRAM_DSIZE_MASK(2) | SDRAM_DSIZE_MASK(3))
        orr     r3, r3, #(SDRAM_DSIZE_1Gb(2) | SDRAM_DSIZE_1Gb(3))
        cmp     r8, #DRAM_DEV_DENSITY_1G
        beq     get_width

        and     r3, r3, #~(SDRAM_DSIZE_MASK(2) | SDRAM_DSIZE_MASK(3))
        orr     r3, r3, #(SDRAM_DSIZE_2Gb(2) | SDRAM_DSIZE_2Gb(3))
        cmp     r8, #DRAM_DEV_DENSITY_2G
        beq     get_width

        /* This is an error. return */
        b       exit_ddrAutoConfig

	/* Get SDRAM width */
get_width: 
	/* Get bank 0 and 1 width */
	ldr	r6, =0
	bl 	_get_width

        cmp     r7, #8           /* x8 devices   */  
        beq     get_bank_2_width

        orr     r3, r3, #(SDRAM_ADDRSEL_X16(0) | SDRAM_ADDRSEL_X16(1)) /* x16 devices  */
        cmp     r7, #16
        beq     get_bank_2_width

        /* This is an error. return */
        b       exit_ddrAutoConfig

get_bank_2_width:
	/* Check for second dimm */
	MV_REG_READ_ASM (r6, r1, DRAM_BUF_REG14)
	cmp	r6, #1
	bne 	densCont

	/* Get bank 2 and 3 width */
	ldr	r6, =2
	bl 	_get_width

        cmp     r7, #8           /* x8 devices   */  
        beq     densCont

        orr     r3, r3, #(SDRAM_ADDRSEL_X16(2) | SDRAM_ADDRSEL_X16(3)) /* x16 devices  */
        cmp     r7, #16
        beq     densCont

        /* This is an error. return */
        b       exit_ddrAutoConfig

densCont:
        MV_REG_WRITE_ASM (r3, r5, DRAM_BUF_REG4)

        /* Set SDRAM timing control low register */
	ldr	r4, =SDRAM_TIMING_CTRL_LOW_REG_DEFAULT
        /* MV_REG_READ_ASM (r4, r5, SDRAM_TIMING_CTRL_LOW_REG) */
        MV_REG_WRITE_ASM(r4, r5, DRAM_BUF_REG5)

        /* Set SDRAM timing control high register */
	ldr	r6, =SDRAM_TIMING_CTRL_HIGH_REG_DEFAULT

    MV_REG_READ_ASM (r4, r5, CPU_RESET_SAMPLE_L_REG)
	ldr	r5, =MSAR_SYSCLCK_MASK
	and	r4, r4, r5	
	ldr	r5, =MSAR_SYSCLCK_333
	cmp	r4, r5
	blt	timingHighClock333
    orr r6, r6, #BIT9

timingHighClock333:
    /* MV_REG_READ_ASM (r6, r5, SDRAM_TIMING_CTRL_HIGH_REG) */
    MV_REG_WRITE_ASM(r6, r5, DRAM_BUF_REG6)

	/* Check for second dimm */
	MV_REG_READ_ASM (r6, r1, DRAM_BUF_REG14)
	cmp	r6, #1
	bne 	single_dimm_odt

        /* Set SDRAM ODT control low register for double DIMM*/        
        ldr	r4, =DDR2_ODT_CTRL_LOW_CS0_CS1_CS2_CS3_DV
        MV_REG_WRITE_ASM(r4, r5, DRAM_BUF_REG7)

        /* Set DUNIT ODT control register for double DIMM */
        ldr	r4, =DDR2_DUNIT_ODT_CTRL_CS0_CS1_CS2_CS3_DV
        MV_REG_WRITE_ASM(r4, r5, DRAM_BUF_REG9)

#ifdef MV78XX0
        /* Set SDRAM Extended Mode register for double DIMM */
	/* Check DRAM frequency for more then 267MHz set ODT Rtt to 50ohm */

        MV_REG_READ_ASM (r4, r5, CPU_RESET_SAMPLE_L_REG)
	ldr	r5, =MSAR_SYSCLCK_MASK
	and	r4, r4, r5	
	ldr	r5, =MSAR_SYSCLCK_267
	cmp	r4, r5
	beq	slow_dram_clock_rtt
	ldr	r5, =MSAR_SYSCLCK_300
	cmp	r4, r5
	beq	slow_dram_clock_rtt
	ldr	r5, =MSAR_SYSCLCK_333
	cmp	r4, r5
	beq	fast_dram_clock_rtt
	ldr	r5, =MSAR_SYSCLCK_400
	cmp	r4, r5
	beq	fast_dram_clock_rtt

	b	slow_dram_clock_rtt

fast_dram_clock_rtt:
        ldr	r4, =DDR_SDRAM_EXT_MODE_FAST_CS0_CS1_CS2_CS3_DV
        MV_REG_WRITE_ASM(r4, r5, DRAM_BUF_REG10)
	b odt_config_end
#endif
slow_dram_clock_rtt:
        ldr	r4, =DDR_SDRAM_EXT_MODE_CS0_CS1_CS2_CS3_DV
        MV_REG_WRITE_ASM(r4, r5, DRAM_BUF_REG10)
	b odt_config_end

single_dimm_odt:
        /* Set SDRAM ODT control low register */        
        ldr	r4, =DDR2_ODT_CTRL_LOW_CS0_CS1_DV
        MV_REG_WRITE_ASM(r4, r5, DRAM_BUF_REG7)

        /* Set DUNIT ODT control register */
        ldr	r4, =DDR2_DUNIT_ODT_CTRL_CS0_CS1_DV
        MV_REG_WRITE_ASM(r4, r5, DRAM_BUF_REG9)

        /* Set SDRAM Extended Mode register */
        ldr	r4, =DDR_SDRAM_EXT_MODE_CS0_CS1_DV
        MV_REG_WRITE_ASM(r4, r5, DRAM_BUF_REG10)

odt_config_end:
        /* SDRAM ODT control high register is left as default */
        MV_REG_READ_ASM (r4, r5, DDR2_SDRAM_ODT_CTRL_HIGH_REG)
        MV_REG_WRITE_ASM(r4, r5, DRAM_BUF_REG8)

        /*Read CL and set the DDR2 registers accordingly */
        MV_REG_READ_ASM (r6, r5, DRAM_BUF_REG2)
        and r6, r6, #SDRAM_CL_MASK
        mov r4, r6
        orr r4, r4, r6, LSL #4
        orr r4, r4, r6, LSL #8
        orr r4, r4, r6, LSL #12
        mov r5, #0x30000
        add r4, r4, r5
        sub r4, r4, #0x30
        /* Set SDRAM Ddr2 Timing Low register */
        MV_REG_WRITE_ASM(r4, r5, DRAM_BUF_REG11)

        /* Set SDRAM Ddr2 Timing High register */
        mov r4, r4, LSR #4
        MV_REG_WRITE_ASM(r4, r5, DRAM_BUF_REG12)

timeParamDone:        
	/* Close all windows */
        MV_REG_READ_ASM (r6, r5, SDRAM_SIZE_REG(0,0))
        and	r6, r6,#~SCSR_SIZE_MASK
        and	r6, r6,#~1
        MV_REG_WRITE_ASM (r6, r5, SDRAM_SIZE_REG(0,0))
        MV_REG_READ_ASM (r6, r5, SDRAM_SIZE_REG(0,1))
        and	r6, r6,#~SCSR_SIZE_MASK
        and	r6, r6,#~1
        MV_REG_WRITE_ASM (r6, r5, SDRAM_SIZE_REG(0,1))
        MV_REG_READ_ASM (r6, r5, SDRAM_SIZE_REG(0,2))
        and	r6, r6,#~SCSR_SIZE_MASK
        and	r6, r6,#~1
        MV_REG_WRITE_ASM (r6, r5, SDRAM_SIZE_REG(0,2))
        MV_REG_READ_ASM (r6, r5, SDRAM_SIZE_REG(0,3))
        and	r6, r6,#~SCSR_SIZE_MASK
        and	r6, r6,#~1
        MV_REG_WRITE_ASM (r6, r5, SDRAM_SIZE_REG(0,3))

        /* Set sdram bank 0 size and enable it */
	ldr	r6, =0
	bl _mvDramIfGetDimmSizeFromSpd
#ifdef MV78XX0
	/* Check DRAM width */
        MV_REG_READ_ASM (r4, r5, SDRAM_CONFIG_REG)
	ldr	r5, =SDRAM_DWIDTH_MASK
	and	r4, r4, r5	
	ldr	r5, =SDRAM_DWIDTH_64BIT
	cmp	r4, r5
	beq	dram_64bit_width
	/* Utilize only 32bit width */
	mov	r8, r8, LSR #1
#else
	/* Utilize only 16bit width */
	mov	r8, r8, LSR #2
#endif
dram_64bit_width:
	/* Update first dimm size return value R8 */
        MV_REG_READ_ASM (r5, r6, SDRAM_SIZE_REG(0,0))
        ldr	r6, =~SCSR_SIZE_MASK	
	and	r5, r5, r6
	orr	r5, r5, r8
        MV_REG_WRITE_ASM(r5, r8, SDRAM_SIZE_REG(0,0))

	/* Clear bank 2 size */
        MV_REG_READ_ASM (r6, r5, SDRAM_SIZE_REG(0,2))
        and	r6, r6,#~SCSR_SIZE_MASK	
        MV_REG_WRITE_ASM (r6, r5, SDRAM_SIZE_REG(0,2))

	/* Check for second dimm */
	MV_REG_READ_ASM (r6, r1, DRAM_BUF_REG14)
	cmp	r6, #1
	bne 	defualt_order

        /* Set sdram bank 2 size */
	ldr	r6, =2
	bl _mvDramIfGetDimmSizeFromSpd
#ifdef MV78XX0
	/* Check DRAM width */
        MV_REG_READ_ASM (r4, r5, SDRAM_CONFIG_REG)
	ldr	r5, =SDRAM_DWIDTH_MASK
	and	r4, r4, r5	
	ldr	r5, =SDRAM_DWIDTH_64BIT
	cmp	r4, r5
	beq	dram_64bit_width2
	/* Utilize only 32bit width */
	mov	r8, r8, LSR #1
#else
	/* Utilize only 16bit width */
	mov	r8, r8, LSR #2
#endif
dram_64bit_width2:
	/* Update first dimm size return value R8 */
        MV_REG_READ_ASM (r5, r6, SDRAM_SIZE_REG(0,2))
        ldr	r6, =~SCSR_SIZE_MASK	
	and	r5, r5, r6
	orr	r5, r5, r8
        MV_REG_WRITE_ASM(r5, r8, SDRAM_SIZE_REG(0,2))

	/* Close windows 1 and 3 */
        MV_REG_READ_ASM (r6, r5, SDRAM_SIZE_REG(0,1))
        and	r6, r6,#~1
        MV_REG_WRITE_ASM (r6, r5, SDRAM_SIZE_REG(0,1))
        MV_REG_READ_ASM (r6, r5, SDRAM_SIZE_REG(0,3))
        and	r6, r6,#~1
        MV_REG_WRITE_ASM (r6, r5, SDRAM_SIZE_REG(0,3))

	/* Check dimm size for setting dram bank order */
        MV_REG_READ_ASM (r6, r5, SDRAM_SIZE_REG(0,0))
        MV_REG_READ_ASM (r4, r5, SDRAM_SIZE_REG(0,2))
        and	r6, r6,#SCSR_SIZE_MASK	
        and	r4, r4,#SCSR_SIZE_MASK	
	cmp	r6, r4
	bge	defualt_order

	/* Bank 2 is biger then bank 0 */
	ldr	r6,=0
        MV_REG_WRITE_ASM (r6, r5, SDRAM_BASE_ADDR_REG(0,2))

	/* Open win 2 */
        MV_REG_READ_ASM (r6, r5, SDRAM_SIZE_REG(0,2))
        orr	r6, r6,#1
        MV_REG_WRITE_ASM (r6, r5, SDRAM_SIZE_REG(0,2))

	ldr	sp,=0
	bl	_mvDramIfConfig
#ifdef MV78XX0
	/* Init ECC on CS 2 */
	ldr	r0, =2
	bl	_mvDramIfEccMemInit
#endif
        mov     PC, r11         /* r11 is saved link register */

defualt_order:

	/* Open win 0 */
        MV_REG_READ_ASM (r6, r5, SDRAM_SIZE_REG(0,0))
        orr	r6, r6,#1
        MV_REG_WRITE_ASM (r6, r5, SDRAM_SIZE_REG(0,0))

	ldr	sp,=0
	bl	_mvDramIfConfig
#ifdef MV78XX0
	/* Init ECC on CS 0 */
	ldr	r0, =0
	bl	_mvDramIfEccMemInit
#endif
exit_ddrAutoConfig:
        mov     PC, r11         /* r11 is saved link register */


/***************************************************************************************/
/*       r4 holds I2C EEPROM address
 *       r7 holds I2C EEPROM offset parameter for i2cRead and its --> returned value
 *       r8 holds SDRAM various configuration registers value.
 *	r13 holds Link register
 */
/**************************/
_getDensity:
	mov     r13, LR                            /* Save link register */

	/* Read SPD rank size from DIMM0 */
        mov     r4, #MV_BOARD_DIMM0_I2C_ADDR       /* reading from DIMM0      */

	cmp	r6, #0
	beq	1f

	/* Read SPD rank size from DIMM1 */
        mov     r4, #MV_BOARD_DIMM1_I2C_ADDR /* reading from DIMM1            */

1:
        mov     r7, #NUM_OF_ROWS_OFFSET            /* offset  3               */
        bl      _i2cRead
        mov     r8, r7                             /* r8 save number of rows  */

        mov     r7, #NUM_OF_COLS_OFFSET            /* offset  4               */
        bl      _i2cRead
        add     r8, r8, r7                         /* r8 = number of rows + number of col */

        mov     r7, #0x1
        mov     r8, r7, LSL r8                     /* r8 = (1 << r8)          */

        mov     r7, #SDRAM_WIDTH_OFFSET            /* offset 13 */
        bl      _i2cRead
        mul     r8, r7, r8

        mov     r7, #NUM_OF_BANKS_OFFSET           /* offset 17               */
        bl      _i2cRead
        mul     r7, r8, r7

	mov     PC, r13

/**************************/
_get_width:
	mov     r13, LR                 /* Save link register */

	/* Read SPD rank size from DIMM0 */
        mov     r4, #MV_BOARD_DIMM0_I2C_ADDR /* reading from DIMM0            */

	cmp	r6, #0
	beq	1f

	/* Read SPD rank size from DIMM1 */
        mov     r4, #MV_BOARD_DIMM1_I2C_ADDR /* reading from DIMM1            */

1:
        /* Get SDRAM width (SPD offset 13) */
        mov     r7, #SDRAM_WIDTH_OFFSET
        bl      _i2cRead                /* result in r7                       */

	mov     PC, r13

/**************************/
_get_CAL:
	mov     r13, LR                 /* Save link register */

        /* Set maximum CL supported by DIMM */
        mov     r4, #MV_BOARD_DIMM0_I2C_ADDR /* reading from DIMM0            */
        mov     r7, #SUPPORTED_CL_OFFSET     /* offset  18 */
        bl      _i2cRead

	mov     PC, r13

/**************************/
/* R8 - sdram configuration register.
 * Return value in flag if no-registered then Z-flag is set
 */
_is_Registered:
	mov     r13, LR                 /* Save link register */
#if defined(MV645xx)
        /* Get registered/non registered info from DIMM */
        tst     r8, #SDRAM_DTYPE_DDR2
        bne     regDdr2

regDdr1:
        mov     r4, #MV_BOARD_DIMM0_I2C_ADDR /* reading from DIMM0            */
        mov     r7, #SDRAM_MODULES_ATTR_OFFSET
        bl      _i2cRead                /* result in r7                       */

        tst     r7, #0x2
	b	exit
#endif
regDdr2:
        mov     r4, #MV_BOARD_DIMM0_I2C_ADDR /* reading from DIMM0            */
        mov     r7, #DIMM_TYPE_INFO_OFFSET
        bl      _i2cRead                /* result in r7                       */

        tst     r7, #0x11               /* DIMM type = regular RDIMM (0x01)   */
                                        /* or Mini-RDIMM (0x10)               */
exit:
        mov     PC, r13


/**************************/
/* Return value in flag if no-Ecc then Z-flag is set */
_is_Ecc:
	mov     r13, LR                 /* Save link register */

        mov     r4, #MV_BOARD_DIMM0_I2C_ADDR /* reading from DIMM0            */
        mov     r7, #DIMM_CONFIG_TYPE
        bl      _i2cRead                /* result in r7                       */

        tst     r7, #0x2               /* bit 1 -> Data ECC */
        mov     PC, r13

/**************************/
/* Return value in flag if no second DIMM then Z-flag is set */
_is_Second_Dimm_Exist:
	mov     r13, LR                 /* Save link register */

        mov     r4, #MV_BOARD_DIMM1_I2C_ADDR /* reading from DIMM0            */
        mov     r7, #DIMM_TYPE_OFFSET
        bl      _i2cRead                /* result in r7                       */

     	tst     r7, #0x8               /* bit3 is '1' -> DDR 2 */
        mov     PC, r13

/*******************************************************************************
* _mvDramIfGetDimmSizeFromSpd  - read bank 0 dram's size
*
* DESCRIPTION:
*       The function will read the bank 0 dram size(SPD version 1.0 and above )  
*
* INPUT:
*       r6 - dram bank number.
*
* OUTPUT:
*	none
*/
_mvDramIfGetDimmSizeFromSpd:

	mov     r13, LR                 /* Save link register */
	
	/* Read SPD rank size from DIMM0 */
        mov     r4, #MV_BOARD_DIMM0_I2C_ADDR /* reading from DIMM0            */

	cmp	r6, #0
	beq	1f

	/* Read SPD rank size from DIMM1 */
        mov     r4, #MV_BOARD_DIMM1_I2C_ADDR /* reading from DIMM1            */

1:
        mov     r7, #RANK_SIZE_OFFSET	/* offset  31 */
        bl      _i2cRead  
	
pass_read:
      	ldr     r8, =(0x7 << SCSR_SIZE_OFFS)
        cmp	r7, #DRAM_RANK_DENSITY_128M
        beq     endDimmSize

      	ldr     r8, =(0xf << SCSR_SIZE_OFFS)
        cmp	r7, #DRAM_RANK_DENSITY_256M
        beq     endDimmSize
        
        ldr     r8, =(0x1f << SCSR_SIZE_OFFS)
        cmp	r7, #DRAM_RANK_DENSITY_512M
        beq     endDimmSize
        
        ldr     r8, =(0x3f << SCSR_SIZE_OFFS)
        cmp	r7, #DRAM_RANK_DENSITY_1G
        beq     endDimmSize

        ldr     r8, =(0x7f  << SCSR_SIZE_OFFS)     /* DRAM_RANK_DENSITY_2G */
endDimmSize:
        mov     PC, r13
#endif

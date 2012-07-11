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

/*******************************************************************************
* mvDramIfBasicAsm.s
*
* DESCRIPTION:
*       Memory full detection and best timing configuration is done in 
*       C code. C runtime environment requires a stack. This module API 
*       initialize DRAM interface chip select 0 for basic functionality for 
*       the use of stack.
*       The module API assumes DRAM information is stored in I2C EEPROM reside
*       in a given I2C address MV_BOARD_DIMM0_I2C_ADDR. The I2C EEPROM 
*       internal data structure is assumed to be orgenzied in common DRAM 
*       vendor SPD structure.
*       NOTE: DFCDL values are assumed to be already initialized prior to 
*       this module API activity.
*       
*
* DEPENDENCIES:
*       None.
*
*******************************************************************************/

/* includes */
#define	_ASMLANGUAGE
#define MV_ASMLANGUAGE
#include "mvOsAsm.h"
#include "mvSysHwConfig.h"
#include "mvDramIfRegs.h"
#include "mvDramIfConfig.h"
#include "ctrlEnv/sys/mvCpuIfRegs.h"
#include "pex/mvPexRegs.h"
#include "ctrlEnv/mvCtrlEnvSpec.h"
#include "mvCommon.h"

/* defines  */

/* locals   */
.data
.globl _mvDramIfConfig
.text
.globl _mvDramIfMemInit

/*******************************************************************************
* _mvDramIfConfig - Basic DRAM interface initialization.
*
* DESCRIPTION:
*       The function will initialize the following DRAM parameters using the
*       values prepared by mvDramIfDetect routine. Values are located
*       in predefined registers.
*
* INPUT:
*       None.
*
* OUTPUT:
*       None.
*
* RETURN:
*       None.
*
*******************************************************************************/

_mvDramIfConfig:      
        
        /* Save register on stack */
	cmp	sp, #0
	beq	no_stack_s
save_on_stack:
        stmdb	sp!, {r1, r2, r3, r4}
no_stack_s:

	/* Dunit FTDLL Configuration Register */
	/* 0) Write to SDRAM FTDLL coniguration register */
        ldr     r4, = SDRAM_FTDLL_REG_DEFAULT_LEFT;
        ldr     r1, =(INTER_REGS_BASE + SDRAM_FTDLL_CONFIG_LEFT_REG)
        str     r4, [r1]
        ldr     r4, = SDRAM_FTDLL_REG_DEFAULT_RIGHT;
        ldr     r1, =(INTER_REGS_BASE + SDRAM_FTDLL_CONFIG_RIGHT_REG)
        str     r4, [r1]
        ldr     r4, = SDRAM_FTDLL_REG_DEFAULT_UP;
        ldr     r1, =(INTER_REGS_BASE + SDRAM_FTDLL_CONFIG_UP_REG)
        str     r4, [r1]

	/* 1) Write to SDRAM coniguration register */
        ldr     r1, =(INTER_REGS_BASE + DRAM_BUF_REG1)
        ldr     r4, [r1]
        ldr     r1, =(INTER_REGS_BASE + SDRAM_CONFIG_REG)
        str     r4, [r1]
        
	/* 2) Write Dunit control low register */ 
        ldr     r1, =(INTER_REGS_BASE + DRAM_BUF_REG3)
        ldr     r4, [r1]
        ldr     r1, =(INTER_REGS_BASE + SDRAM_DUNIT_CTRL_REG)
        str     r4, [r1]
        
	/* 2) Write Dunit control high register */ 
        ldr     r1, =(INTER_REGS_BASE + DRAM_BUF_REG13)
        ldr     r4, [r1]
        ldr     r1, =(INTER_REGS_BASE + SDRAM_DUNIT_CTRL_HI_REG)
        str     r4, [r1]
        
        /* 3) Write SDRAM address control register */ 
        ldr     r1, =(INTER_REGS_BASE + DRAM_BUF_REG4)
        ldr     r4, [r1]
        ldr     r1, =(INTER_REGS_BASE + SDRAM_ADDR_CTRL_REG)
        str     r4, [r1]
#if defined(MV_STATIC_DRAM_ON_BOARD)
        /* 4) Write SDRAM bank 0 size register */
        ldr     r1, =(INTER_REGS_BASE + DRAM_BUF_REG0)
        ldr     r4, [r1]
        ldr     r1, =(INTER_REGS_BASE + SDRAM_SIZE_REG(0,0))
        str     r4, [r1]
#endif
                          
        /* 5) Write SDRAM open pages control register */
        ldr     r1, =(INTER_REGS_BASE + SDRAM_OPEN_PAGE_CTRL_REG)
        ldr     r4, =SDRAM_OPEN_PAGES_CTRL_REG_DV
        str     r4, [r1]
                          
        /* 6) Write SDRAM timing Low register */
        ldr     r1, =(INTER_REGS_BASE + DRAM_BUF_REG5)
        ldr     r4, [r1]
        ldr     r1, =(INTER_REGS_BASE + SDRAM_TIMING_CTRL_LOW_REG)
        str     r4, [r1]
        
        /* 7) Write SDRAM timing High register */
        ldr     r1, =(INTER_REGS_BASE + DRAM_BUF_REG6)
        ldr     r4, [r1]
        ldr     r1, =(INTER_REGS_BASE + SDRAM_TIMING_CTRL_HIGH_REG)
        str     r4, [r1]
                
        /* Config DDR2 On Die Termination (ODT) registers */
        /* Write SDRAM DDR2 ODT control low register */
        ldr     r1, =(INTER_REGS_BASE + DRAM_BUF_REG7)
        ldr     r4, [r1]
        ldr     r1, =(INTER_REGS_BASE + DDR2_SDRAM_ODT_CTRL_LOW_REG)
        str     r4, [r1]
        
        /* Write SDRAM DDR2 ODT control high register */
        ldr     r1, =(INTER_REGS_BASE + DRAM_BUF_REG8)
        ldr     r4, [r1]
        ldr     r1, =(INTER_REGS_BASE + DDR2_SDRAM_ODT_CTRL_HIGH_REG)
        str     r4, [r1]
        
        /* Write SDRAM DDR2 Dunit ODT control register */
        ldr     r1, =(INTER_REGS_BASE + DRAM_BUF_REG9)
        ldr     r4, [r1]
        ldr     r1, =(INTER_REGS_BASE + DDR2_DUNIT_ODT_CONTROL_REG)
        str     r4, [r1]
        
        /* Write DDR2 SDRAM timing Low register */
        ldr     r1, =(INTER_REGS_BASE + DRAM_BUF_REG11)
        ldr     r4, [r1]
        ldr     r1, =(INTER_REGS_BASE + SDRAM_DDR2_TIMING_LO_REG)
        str     r4, [r1]
                                   
        /* Write DDR2 SDRAM timing High register */
        ldr     r1, =(INTER_REGS_BASE + DRAM_BUF_REG12)
        ldr     r4, [r1]
        ldr     r1, =(INTER_REGS_BASE + SDRAM_DDR2_TIMING_HI_REG)
        str     r4, [r1]

        /* 8) Write SDRAM mode register */ 
        /* The CPU must not attempt to change the SDRAM Mode register setting */
        /* prior to DRAM controller completion of the DRAM initialization     */
        /* sequence. To guarantee this restriction, it is recommended that    */
        /* the CPU sets the SDRAM Operation register to NOP command, performs */
        /* read polling until the register is back in Normal operation value, */
        /* and then sets SDRAM Mode register to its new value.               */
        
	/* 8.1 write 'nop' to SDRAM operation */
        mov     r4, #0x5                 /* 'NOP' command              */
        MV_REG_WRITE_ASM(r4, r1, SDRAM_OPERATION_REG)
       
        /* 8.2 poll SDRAM operation. Make sure its back to normal operation   */
_sdramOpPoll1:        
        ldr     r4, [r1]
        cmp     r4, #0                          /* '0' = Normal SDRAM Mode    */
        bne     _sdramOpPoll1

        /* 8.3 Now its safe to write new value to SDRAM Mode register         */
        ldr     r1, =(INTER_REGS_BASE + DRAM_BUF_REG2)
        ldr     r4, [r1]
        ldr     r1, =(INTER_REGS_BASE + SDRAM_MODE_REG)
        str     r4, [r1]

        /* 8.4 Make the Dunit write the DRAM its new mode                     */       
        mov     r4, #0x3                 /* Mode Register Set command  */
        MV_REG_WRITE_ASM (r4, r1, SDRAM_OPERATION_REG)
               
        /* 8.5 poll SDRAM operation. Make sure its back to normal operation   */
_sdramOpPoll2:        
        ldr     r4, [r1]                     
        cmp     r4, #0                          /* '0' = Normal SDRAM Mode    */
        bne     _sdramOpPoll2

        /* Now its safe to write new value to SDRAM Extended Mode regist */
        ldr     r1, =(INTER_REGS_BASE + DRAM_BUF_REG10)
        ldr     r4, [r1]
        ldr     r1, =(INTER_REGS_BASE + SDRAM_EXTENDED_MODE_REG)
        str     r4, [r1]

        /* 9) Write SDRAM Extended mode register This operation should be     */
        /*    done for each memory bank                                       */ 
        /* write 'nop' to SDRAM operation */
        mov     r4, #0x5                 /* 'NOP' command              */
        MV_REG_WRITE_ASM (r4, r1, SDRAM_OPERATION_REG)
       
        /* poll SDRAM operation. Make sure its back to normal operation   */
_sdramOpPoll3:        
        ldr     r4, [r1]
        cmp     r4, #0                          /* '0' = Normal SDRAM Mode    */
        bne     _sdramOpPoll3
        /* Go over each of the Banks */
        ldr     r3, =0          /* r3 = DRAM bank Num */

extModeLoop:        
        /* Set the SDRAM Operation Control to each of the DRAM banks          */
        mov     r4, r3   /* Do not swap the bank counter value */
        MV_REG_WRITE_ASM (r4, r1, SDRAM_OPERATION_CTRL_REG)
        
        /* Make the Dunit write the DRAM its new mode                     */       
        mov     r4, #0x4        /* Extended Mode Register Set command  */
        MV_REG_WRITE_ASM (r4, r1, SDRAM_OPERATION_REG)
               
        /* poll SDRAM operation. Make sure its back to normal operation   */
_sdramOpPoll4:        
        ldr     r4, [r1]                     
        cmp     r4, #0                          /* '0' = Normal SDRAM Mode    */
        bne     _sdramOpPoll4

        add     r3, r3, #1
        cmp     r3, #4         /* 4 = Number of banks */
        bne     extModeLoop
        
extModeEnd:	
cmp	sp, #0
	beq	no_stack_l
	mov     r1, LR                        	/* Save link register */
#if defined(MV78XX0)
	bl   	_mvDramIfMemInit 
#endif
	mov	LR,r1				/* restore link register */
load_from_stack:
	/* Restore registers */
        ldmia	sp!, {r1, r2, r3, r4}
no_stack_l:
           
        mov     pc, lr


/*******************************************************************************
* _mvDramIfEccMemInit - Basic DRAM ECC initialization.
*
* DESCRIPTION:
*
* INPUT:
*       None.
*
* OUTPUT:
*       None.
*
* RETURN:
*       None.
*
*******************************************************************************/
#define XOR_CHAN0         0   /* XOR channel 0 used for memory initialization */
#define XOR_UNIT0         0   /* XOR unit 0 used for memory initialization */
#define XOR_ADDR_DEC_WIN0 0   /* Enable DRAM access using XOR decode window 0 */ 
/* XOR engine register offsets macros */
#define XOR_CONFIG_REG(chan)                (XOR_UNIT_BASE(0) + 0x10 + ((chan)    * 4))
#define XOR_ACTIVATION_REG(chan)            (XOR_UNIT_BASE(0) + 0x20 + ((chan)    * 4))
#define XOR_CAUSE_REG			            (XOR_UNIT_BASE(0) + 0x30)
#define XOR_ERROR_CAUSE_REG                 (XOR_UNIT_BASE(0) + 0x50)             
#define XOR_ERROR_ADDR_REG                  (XOR_UNIT_BASE(0) + 0x60)             
#define XOR_INIT_VAL_LOW_REG                (XOR_UNIT_BASE(0) + 0x2E0)
#define XOR_INIT_VAL_HIGH_REG               (XOR_UNIT_BASE(0) + 0x2E4)
#define XOR_DST_PTR_REG(chan)               (XOR_UNIT_BASE(0) + 0x2B0 + ((chan)    * 4))
#define XOR_BLOCK_SIZE_REG(chan)            (XOR_UNIT_BASE(0) + 0x2C0 + ((chan)    * 4))

/* XOR Engine Address Decoding Register Map */                  
#define XOR_WINDOW_CTRL_REG(unit,chan)     (XOR_UNIT_BASE(unit)+(0x240 + ((chan) * 4)))
#define XOR_BASE_ADDR_REG(unit,winNum)     (XOR_UNIT_BASE(unit)+(0x250 + ((winNum) * 4)))
#define XOR_SIZE_MASK_REG(unit,winNum)     (XOR_UNIT_BASE(unit)+(0x270 + ((winNum) * 4)))

.globl _mvDramIfEccMemInit
/*******************************************************************************
* _mvDramIfEccMemInit  - mem init for dram cs
*
* DESCRIPTION:
*       This function will clean the cs by ussing the XOR mem init.  
*
* INPUT:
*       r0 - dram bank number.
*
* OUTPUT:
*	none
*/
_mvDramIfEccMemInit:      
        
        /* Save register on stack */
	cmp	sp, #0
	beq	no_stack_s1
save_on_stack1:
        stmdb	sp!, {r0,r1, r2, r3, r4, r5, r6}
no_stack_s1:

	ldr	r1, = 0             

        /* Disable all XOR address decode windows to avoid possible overlap */
        MV_REG_WRITE_ASM (r1, r5, (XOR_WINDOW_CTRL_REG(XOR_UNIT0,XOR_CHAN0)))

        /* Init r5 to first XOR_SIZE_MASK_REG */
		mov		r5, r0, LSL #3
        add     r5, r5,#0x1500
        add     r5, r5,#0x04
        add     r5, r5,#(INTER_REGS_BASE)
        ldr     r6, [r5]
        HTOLL(r6,r5)
        MV_REG_WRITE_ASM (r6, r5, XOR_SIZE_MASK_REG(XOR_UNIT0,XOR_ADDR_DEC_WIN0))
       
	mov		r5, r0, LSL #3
        add     r5, r5,#0x1500
        add     r5, r5,#(INTER_REGS_BASE)
        ldr     r6, [r5]
        /* Update destination & size */
        MV_REG_WRITE_ASM(r6, r5, XOR_DST_PTR_REG(XOR_CHAN0))
        HTOLL(r6,r5)
        /* Init r6 to first XOR_BASE_ADDR_REG */
	ldr	r4, = 0xf              
	ldr	r5, = 0x1
	mov	r5, r5, LSL r0
	bic	r4, r4, r5
	mov 	r4, r4, LSL #8

        orr	r6, r6, r4
        MV_REG_WRITE_ASM (r6, r5, XOR_BASE_ADDR_REG(XOR_UNIT0,XOR_ADDR_DEC_WIN0))
        
	ldr	r6, = 0xff0001              
        MV_REG_WRITE_ASM (r6, r5, XOR_WINDOW_CTRL_REG(XOR_UNIT0,XOR_CHAN0))        

        /* Configure XOR engine for memory init function.           */
        MV_REG_READ_ASM (r6, r5, XOR_CONFIG_REG(XOR_CHAN0))
        and	r6, r6, #~0x7        	/* Clear operation mode field      */
        orr     r6, r6, #0x4             /* Set operation to memory init    */
        MV_REG_WRITE_ASM(r6, r5, XOR_CONFIG_REG(XOR_CHAN0))
               
        /* Set initVal in the XOR Engine Initial Value Registers       */
	ldr	r6, = 0xfeedfeed              
        MV_REG_WRITE_ASM(r6, r5, XOR_INIT_VAL_LOW_REG)
	ldr	r6, = 0xfeedfeed              
        MV_REG_WRITE_ASM(r6, r5, XOR_INIT_VAL_HIGH_REG)

        /* Set block size using DRAM bank size  */

	mov	r5, r0, LSL #3
        add     r5, r5,#0x1500
        add     r5, r5,#0x04
        add     r5, r5,#(INTER_REGS_BASE)

        ldr     r6, [r5]
        HTOLL(r6,r5)
	and	r6, r6, #SCSR_SIZE_MASK
	mov	r5, r6, LSR #SCSR_SIZE_OFFS
        add	r5, r5, #1
	mov	r6, r5, LSL #SCSR_SIZE_OFFS
        MV_REG_WRITE_ASM(r6, r5, XOR_BLOCK_SIZE_REG(XOR_CHAN0))
        
        /* Clean interrupt cause*/
        MV_REG_WRITE_ASM(r1, r5, XOR_CAUSE_REG)

        /* Clean error interrupt cause*/
        MV_REG_READ_ASM(r6, r5, XOR_ERROR_CAUSE_REG)
        MV_REG_READ_ASM(r6, r5, XOR_ERROR_ADDR_REG)

        /* Start transfer */
        MV_REG_READ_ASM (r6, r5, XOR_ACTIVATION_REG(XOR_CHAN0))
        orr     r6, r6, #0x1 /* Preform start command      */
        MV_REG_WRITE_ASM(r6, r5, XOR_ACTIVATION_REG(XOR_CHAN0))

        /* Wait for engine to finish */
waitForComplete:        
        MV_REG_READ_ASM(r6, r5, XOR_CAUSE_REG)
        and   	r6, r6, #2
	cmp	r6, #0
        beq     waitForComplete

        /* Clear all error report registers */
        MV_REG_WRITE_ASM(r1, r5, SDRAM_SINGLE_BIT_ERR_CNTR_REG)
        MV_REG_WRITE_ASM(r1, r5, SDRAM_DOUBLE_BIT_ERR_CNTR_REG)

        MV_REG_WRITE_ASM(r1, r5, SDRAM_ERROR_CAUSE_REG)

	cmp	sp, #0
	beq	no_stack_l1
load_from_stack1:
        ldmia	sp!, {r0, r1, r2, r3, r4, r5, r6}
no_stack_l1:
        mov     pc, lr


/*******************************************************************************
* mvDramIfMemInit - Use XOR to clear all memory.
*
* DESCRIPTION:
*       Use assembler function _mvDramIfEccMemInit to fill all memory with FEADFEAD pattern.
* INPUT:
*       None.
*
* OUTPUT:
*       None.
*
* RETURN:
*       None.
*
*******************************************************************************/
#if defined(MV78XX0)

_mvDramIfMemInit:
        stmdb	sp!, {r0,r1, r2, r3, r4, r5, r6}
	mov     r6, LR                 /* Save link register */
    	/* Check if dram bank 0 has to be init for ECC */
	MV_REG_READ_ASM (r0, r5, SDRAM_SIZE_REG(0,0))
	and 	r3, r0, #SCSR_WIN_EN
        cmp     r3, #0 
	beq   	no_bank_0
	MV_REG_READ_ASM(r0, r5,  SDRAM_BASE_ADDR_REG(0,0))
        cmp     r0, #0 
	beq   	no_bank_0
	mov	r0,#0
	bl	_mvDramIfEccMemInit

no_bank_0:
    	/* Check if dram bank 1 has to be init for ECC */
        MV_REG_READ_ASM (r0, r5, SDRAM_SIZE_REG(0,1))
	and 	r0, r0, #SCSR_WIN_EN
        cmp     r0, #0 
	beq   	no_bank_1
	mov	r0,#1
	bl	_mvDramIfEccMemInit
no_bank_1:
    	/* Check if dram bank 2 has to be init for ECC */
    	MV_REG_READ_ASM (r0, r5, SDRAM_SIZE_REG(0,2))
	and 	r0, r0, #SCSR_WIN_EN
        cmp     r0, #0 
	beq   	no_bank_2
	MV_REG_READ_ASM(r0, r5,  SDRAM_BASE_ADDR_REG(0,2))
        cmp     r0, #0 
	beq   	no_bank_2
	mov	r0,#2
	bl	_mvDramIfEccMemInit

no_bank_2:
    	/* Check if dram bank 3 has to be init for ECC */
	MV_REG_READ_ASM (r0, r5, SDRAM_SIZE_REG(0,3))
	and 	r0, r0, #SCSR_WIN_EN
	cmp     r0, #0 
	beq   	no_bank_3
	mov	r0,#3
	bl	_mvDramIfEccMemInit
no_bank_3:
	mov     LR ,r6                /* restore link register */
	ldmia	sp!, {r0, r1, r2, r3, r4, r5, r6}
	mov     pc, lr
#endif


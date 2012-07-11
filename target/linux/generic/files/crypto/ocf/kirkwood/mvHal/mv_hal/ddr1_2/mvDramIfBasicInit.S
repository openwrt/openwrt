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

#define MV_ASMLANGUAGE
#include "mvSysHwConfig.h"
#include "mvOsAsm.h"
#include "mvBoardEnvSpec.h"
#include "mvCpuIfRegs.h"
#include "mvDramIfConfig.h"
#include "mvDramIfRegs.h"
#include "pex/mvPexRegs.h"
#include "pci/mvPciRegs.h"
#include "mvCtrlEnvSpec.h"
#include "mvCtrlEnvAsm.h"
#include "cpu/mvCpuArm.h"
#include "mvCommon.h"

/* defines */

#if !defined(MV_INC_BOARD_DDIM)
.globl dramBoot1
dramBoot1:
        .word   0

/******************************************************************************
*
*
*
*
*******************************************************************************/
#if defined(DB_PRPMC) || defined(DB_PEX_PCI) || defined(DB_MNG)

/* PEX_PCI and PRPMC boards 256 MB*/
#define STATIC_SDRAM0_BANK0_SIZE	     	0x0fff0001
#define STATIC_SDRAM_CONFIG	     		0x03248400
#define STATIC_SDRAM_MODE	     		0x62
#define STATIC_DUNIT_CTRL_LOW	     		0x4041000
#define STATIC_SDRAM_ADDR_CTRL	     		0x00000020
#define STATIC_SDRAM_TIME_CTRL_LOW     		0x11602220
#define STATIC_SDRAM_TIME_CTRL_HI	     	0x0000030F
#define STATIC_SDRAM_ODT_CTRL_LOW	     	0x0
#define STATIC_SDRAM_ODT_CTRL_HI	     	0x0
#define STATIC_SDRAM_DUNIT_ODT_CTRL    		0x0
#define STATIC_SDRAM_EXT_MODE          		0x0

#elif defined(DB_FPGA)

/* FPGA DC boards 256 MB*/
#define STATIC_SDRAM0_BANK0_SIZE	     	0x0fff0001
#define STATIC_SDRAM_CONFIG	     		0x03208400	/* 32bit */
#define STATIC_SDRAM_MODE	     		0x22
#define STATIC_DUNIT_CTRL_LOW	     		0x03041000
#define STATIC_SDRAM_ADDR_CTRL	     		0x00000020
#define STATIC_SDRAM_TIME_CTRL_LOW     		0x11112220
#define STATIC_SDRAM_TIME_CTRL_HI	     	0x0000000D
#define STATIC_SDRAM_ODT_CTRL_LOW	     	0x0
#define STATIC_SDRAM_ODT_CTRL_HI	     	0x0
#define STATIC_SDRAM_DUNIT_ODT_CTRL    		0x0
#define STATIC_SDRAM_EXT_MODE          		0x1

#elif  defined(RD_88F6183GP) || defined(DB_CUSTOMER)

/* Customer 1 DDR2 2 devices 512Mbit by 16 bit */
#define STATIC_SDRAM0_BANK0_SIZE	     	0x07ff0001
#define STATIC_SDRAM_CONFIG	     		0x03158400
#define STATIC_SDRAM_MODE	     		0x452
#define STATIC_DUNIT_CTRL_LOW	     		0x06041000
#define STATIC_SDRAM_ADDR_CTRL	     		0x00000020
#define STATIC_SDRAM_TIME_CTRL_LOW     		0x11912220
#define STATIC_SDRAM_TIME_CTRL_HI	     	0x00000502
#define STATIC_SDRAM_ODT_CTRL_LOW	     	0x00010000
#define STATIC_SDRAM_ODT_CTRL_HI	     	0x00000002
#define STATIC_SDRAM_DUNIT_ODT_CTRL    		0x00000601
#define STATIC_SDRAM_EXT_MODE          		0x00000440


#elif  defined(RD_88F6183AP)

/* DDR2 1 devices 512Mbit by 16 bit */
#define STATIC_SDRAM0_BANK0_SIZE	     	0x03ff0001
#define STATIC_SDRAM_CONFIG	     		0x1f154400
#define STATIC_SDRAM_MODE	     		0x432
#define STATIC_DUNIT_CTRL_LOW	     		0x04041000
#define STATIC_SDRAM_ADDR_CTRL	     		0x00000020
#define STATIC_SDRAM_TIME_CTRL_LOW     		0x11912220
#define STATIC_SDRAM_TIME_CTRL_HI	     	0x00000502
#define STATIC_SDRAM_ODT_CTRL_LOW	     	0x00010000
#define STATIC_SDRAM_ODT_CTRL_HI	     	0x00000002
#define STATIC_SDRAM_DUNIT_ODT_CTRL    		0x00000601
#define STATIC_SDRAM_EXT_MODE          		0x00000440

/* 6082L MARVELL DIMM */
#elif  defined(DB_88F6082LBP)
#define STATIC_SDRAM0_BANK0_SIZE	     	0x07ff0001
#define STATIC_SDRAM_CONFIG	     		0x7f158400
#define STATIC_SDRAM_MODE	     		0x432
#define STATIC_DUNIT_CTRL_LOW	     		0x04041040
#define STATIC_SDRAM_ADDR_CTRL	     		0x00000020
#define STATIC_SDRAM_TIME_CTRL_LOW     		0x11612220
#define STATIC_SDRAM_TIME_CTRL_HI	     	0x00000501
#define STATIC_SDRAM_ODT_CTRL_LOW	     	0x00010000
#define STATIC_SDRAM_ODT_CTRL_HI	     	0x00000002
#define STATIC_SDRAM_DUNIT_ODT_CTRL    		0x00000a01
#define STATIC_SDRAM_EXT_MODE          		0x00000440

#elif  defined(RD_88W8660_AP82S)

/* Shark RD */

#if defined(MV_DRAM_32M)
#define STATIC_SDRAM0_BANK0_SIZE	     	0x01ff0001
#define STATIC_SDRAM_ADDR_CTRL	     		0x00000010
#elif defined(MV_DRAM_16M)

#define STATIC_SDRAM0_BANK0_SIZE	     	0x00ff0001
#define STATIC_SDRAM_ADDR_CTRL	     		0x00000000

#else
#error "NO DDR size selected"
#endif

#define STATIC_SDRAM_CONFIG	     		0x03144400
#define STATIC_SDRAM_MODE	     		0x62
#define STATIC_DUNIT_CTRL_LOW	     		0x4041000

#define STATIC_SDRAM_TIME_CTRL_LOW     		0x11602220
#define STATIC_SDRAM_TIME_CTRL_HI	     	0x0000040b
#define STATIC_SDRAM_ODT_CTRL_LOW	     	0x0
#define STATIC_SDRAM_ODT_CTRL_HI	     	0x0
#define STATIC_SDRAM_DUNIT_ODT_CTRL    		0x0
#define STATIC_SDRAM_EXT_MODE          		0x0

#elif defined(RD_88W8660)

/* Shark RD */
#define STATIC_SDRAM0_BANK0_SIZE	     	0x03ff0001
#define STATIC_SDRAM_CONFIG	     		0x03144400
#define STATIC_SDRAM_MODE	     		0x62
#define STATIC_DUNIT_CTRL_LOW	     		0x4041000
#define STATIC_SDRAM_ADDR_CTRL	     		0x00000010
#define STATIC_SDRAM_TIME_CTRL_LOW     		0x11602220
#define STATIC_SDRAM_TIME_CTRL_HI	     	0x0000040b
#define STATIC_SDRAM_ODT_CTRL_LOW	     	0x0
#define STATIC_SDRAM_ODT_CTRL_HI	     	0x0
#define STATIC_SDRAM_DUNIT_ODT_CTRL    		0x0
#define STATIC_SDRAM_EXT_MODE          		0x0

#else /* NAS */


#if defined(RD_88F5182)

#if defined(MV_88F5082)
#define STATIC_SDRAM0_BANK0_SIZE	     	0x3ff0001
#define STATIC_SDRAM_ADDR_CTRL	    	 	0x20
#else
#define STATIC_SDRAM0_BANK0_SIZE	     	0x7ff0001
#define STATIC_SDRAM_ADDR_CTRL	    	 	0x20
#endif

#elif defined(RD_88F5182_3)

#if defined(MV_88F5082)
#define STATIC_SDRAM0_BANK0_SIZE	     	0x3ff0001
#define STATIC_SDRAM_ADDR_CTRL	    	 	0x20
#else
#define STATIC_SDRAM0_BANK0_SIZE	     	0x7ff0001
#define STATIC_SDRAM_ADDR_CTRL	    	 	0x20
#endif

#else

#define STATIC_SDRAM0_BANK0_SIZE	     	0x1ff0001
#define STATIC_SDRAM_ADDR_CTRL	    	 	0x0

#endif

#if defined(MV_88F5082)
#define STATIC_SDRAM_CONFIG	     		0x3144400
#else
#define STATIC_SDRAM_CONFIG	     		0x3148400
#endif
#define STATIC_SDRAM_MODE	     		0x62
#define STATIC_DUNIT_CTRL_LOW	     		0x4041000
#define STATIC_SDRAM_TIME_CTRL_LOW     		0x11602220
#define STATIC_SDRAM_TIME_CTRL_HI	     	0x40c
#define STATIC_SDRAM_ODT_CTRL_LOW	     	0x0
#define STATIC_SDRAM_ODT_CTRL_HI	     	0x0
#define STATIC_SDRAM_DUNIT_ODT_CTRL    		0x0
#define STATIC_SDRAM_EXT_MODE          		0x0

#endif

	.globl _mvDramIfStaticInit
_mvDramIfStaticInit:

	mov     r11, LR     		/* Save link register */
	mov	r10, r2

        /* If we boot from NAND jump to DRAM sddress */

        mov     r5, #1
        ldr     r6, =dramBoot1
        str     r5, [r6]                /* We started executing from DRAM */

        ldr     r6, dramBoot1
        cmp     r6, #0
        bne     1f


	/* set all dram windows to 0 */
	mov	r6, #0
	MV_REG_WRITE_ASM(r6, r5, 0x1504)
	MV_REG_WRITE_ASM(r6, r5, 0x150c)
	MV_REG_WRITE_ASM(r6, r5, 0x1514)
	MV_REG_WRITE_ASM(r6, r5, 0x151c)

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

	mov 	sp, #0
	bl	_mvDramIfConfig
1:
	mov 	r2, r10
	mov     PC, r11         	/* r11 is saved link register */

#else  /* #if !defined(MV_INC_BOARD_DDIM) */

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
#define NUM_OF_ROWS_OFFSET            3
#define NUM_OF_COLS_OFFSET            4
#define NUM_OF_RANKS		      5
#define SDRAM_WIDTH_OFFSET           13
#define NUM_OF_BANKS_OFFSET          17
#define SUPPORTED_CL_OFFSET          18
#define DIMM_TYPE_INFO_OFFSET        20         /* DDR2 only    */
#define SDRAM_MODULES_ATTR_OFFSET    21

#define DRAM_DEV_DENSITY_128M        0x080
#define DRAM_DEV_DENSITY_256M        0x100
#define DRAM_DEV_DENSITY_512M        0x200
       .globl _mvDramIfBasicInit
       .extern _i2cInit

_mvDramIfBasicInit:

        mov     r11, LR     		/* Save link register */

        mov     r5, #1
        ldr     r8, =dramBoot1
        str     r5, [r8]                /* We started executing from DRAM */

        /* If we boot from NAND jump to DRAM sddress */
        ldr     r8, dramBoot1
        cmp     r8, #0
        movne   pc, r11



        bl      _i2cInit                /* Initialize TWSI master             */

        /* Get default SDRAM Config values */
        MV_REG_READ_ASM (r8, r5, SDRAM_CONFIG_REG)
        bic     r8, r8, #SDRAM_DCFG_MASK


        /* Read device ID  */
	MV_CTRL_MODEL_GET_ASM(r4, r5);

        /* Return if OrionN */
        ldr     r5, =MV_5180_DEV_ID
        cmp     r4, r5
        beq     cat_through_end

        /* Return if Orion1 */
        ldr     r5, =MV_5181_DEV_ID
        cmp     r4, r5
        beq     cat_through_end

        /* Return if Nas */
        ldr     r5, =MV_5182_DEV_ID
        cmp     r4, r5
        beq     cat_through_end

        /* Return if Shark */
        ldr     r5, =MV_8660_DEV_ID
        cmp     r4, r5
        beq     cat_through_end

        /* goto calcConfigReg if bigger than Orion2*/
        ldr     r5, =MV_5281_DEV_ID
        cmp     r4, r5
        bne     cat_through

cat_through:
        /* set cat through - for better performance - in orion2 b0 and higher*/
        orr     r8, r8, #SDRAM_CATTHR_EN

cat_through_end:


        /* Get registered/non registered info from DIMM */
	bl  	_is_Registered
        beq     nonRegistered

setRegistered:
        orr     r8, r8, #SDRAM_REGISTERED   /* Set registered bit(17)         */

nonRegistered:
	/* Get SDRAM width */
	bl 	_get_width

        orr     r6, r8, #SDRAM_DCFG_X16_DEV /* x16 devices  */
        cmp     r7, #16
        beq     setConfigReg

        orr     r6, r8, #SDRAM_DCFG_X8_DEV  /* x8 devices   */
        cmp     r7, #8
        beq     setConfigReg

        /* This is an error. return */
        b       exit_ddrAutoConfig

setConfigReg:
        mov     r8, r6
        ldr     r6, =SDRAM_CONFIG_DV
        orr     r8, r8, r6              /* Add default settings */
        mov     r6, r8                  /* Do not swap r8 content */
        MV_REG_WRITE_ASM (r6, r5, SDRAM_CONFIG_REG)

        /* Set maximum CL supported by DIMM */
	bl	_get_CAL

        /* r7 is DIMM supported CAS (e.g: 3 --> 0x1C)                         */
        clz     r6, r7
        rsb     r6, r6, #31     /* r6 = the bit number of MAX CAS supported   */

        /* Check the DDR version */
        tst     r8, #SDRAM_DTYPE_DDR2
        bne     casDdr2

casDdr1:
        ldr	r7, =3		/* stBurstDel field value	*/
	ldr     r8, =0x52       /* Assuming MAX CL = 1.5        */
        cmp     r6, #1          /* If CL = 1.5 break            */
        beq     setModeReg

        ldr	r7, =3		/* stBurstDel field value	*/
	ldr     r8, =0x22       /* Assuming MAX CL = 2          */
        cmp     r6, #2          /* If CL = 2 break              */
        beq     setModeReg

        ldr	r7, =4		/* stBurstDel field value	*/
	ldr     r8, =0x62       /* Assuming MAX CL = 2.5        */
        cmp     r6, #3          /* If CL = 2.5 break            */
        beq     setModeReg

        ldr	r7, =4		/* stBurstDel field value	*/
	ldr     r8, =0x32       /* Assuming MAX CL = 3          */
        cmp     r6, #4          /* If CL = 3 break              */
        beq     setModeReg

        ldr	r7, =5		/* stBurstDel field value	*/
	ldr     r8, =0x42       /* Assuming MAX CL = 4          */
        cmp     r6, #6          /* If CL = 4 break              */
        b       setModeReg

        b       exit_ddrAutoConfig      /* This is an error !!  */

casDdr2:
        ldr	r7, =4		/* stBurstDel field value	*/
	ldr     r8, =0x32      /* Assuming MAX CL = 3           */
        cmp     r6, #3          /* If CL = 3 break              */
        beq     casDdr2Cont

        ldr	r7, =5		/* stBurstDel field value	*/
	ldr     r8, =0x42      /* Assuming MAX CL = 4           */
        cmp     r6, #4          /* If CL = 4 break              */
        beq     casDdr2Cont

        /* CL 5 currently unsupported. We use CL 4 instead      */
        ldr	r7, =5		/* stBurstDel field value	*/
	ldr     r8, =0x42      /* Assuming MAX CL = 5           */
        cmp     r6, #5          /* If CL = 5 break              */
        beq     casDdr2Cont

        b       exit_ddrAutoConfig      /* This is an error !!  */
casDdr2Cont:
        /* Write recovery for auto-precharge relevant only in DDR2 */
        orr     r8, r8, #0x400   /* Default value */

setModeReg:
        /* The CPU must not attempt to change the SDRAM Mode register setting */
        /* prior to DRAM controller completion of the DRAM initialization     */
        /* sequence. To guarantee this restriction, it is recommended that    */
        /* the CPU sets the SDRAM Operation register to NOP command, performs */
        /* read polling until the register is back in Normal operation value, */
        /* and then sets SDRAM Mode register to it's new value.               */

	/* write 'nop' to SDRAM operation */
        mov     r6, #0x5                 /* 'NOP' command              */
        MV_REG_WRITE_ASM (r6, r5, SDRAM_OPERATION_REG)

        /* poll SDRAM operation. Make sure its back to normal operation       */
_sdramOpPoll1:
        ldr     r6, [r5]
        cmp     r6, #0                          /* '0' = Normal SDRAM Mode    */
        bne     _sdramOpPoll1

        /* Now its safe to write new value to SDRAM Mode register             */
        MV_REG_WRITE_ASM (r8, r5, SDRAM_MODE_REG)

        /* Make the Dunit write the DRAM its new mode                         */
        mov     r6, #0x3                 /* Mode Register Set command  */
        MV_REG_WRITE_ASM (r6, r5, SDRAM_OPERATION_REG)

        /* poll SDRAM operation. Make sure its back to normal operation       */
_sdramOpPoll2:
        ldr     r6, [r5]
        cmp     r6, #0                          /* '0' = Normal SDRAM Mode    */
        bne     _sdramOpPoll2

	/* Set Dunit control register according to max CL detected	      */
	/* If we use registered DIMM, add 1 to stBurstDel		      */
        MV_REG_READ_ASM (r6, r5, SDRAM_CONFIG_REG)
	tst	r6, #SDRAM_REGISTERED
	beq	setDunitReg
	add	r7, r7, #1

setDunitReg:
        ldr     r6, =SDRAM_DUNIT_CTRL_LOW_DV
        orr	r6, r6, r7, LSL #SDRAM_ST_BURST_DEL_OFFS
        MV_REG_WRITE_ASM (r6, r5, SDRAM_DUNIT_CTRL_REG)


        /* DIMM density configuration*/
        /* Density = (1 << (rowNum + colNum)) * dramWidth * dramBankNum       */
Density:
	bl 	_getDensity
	mov 	r8, r7
        mov     r8, r8, LSR #20 /* Move density 20 bits to the right  */
                                /* For example 0x10000000 --> 0x1000 */

        mov     r6, #0x00
        cmp     r8, #DRAM_DEV_DENSITY_128M
        beq     densCont

        mov     r6, #0x10
        cmp     r8, #DRAM_DEV_DENSITY_256M
        beq     densCont

        mov     r6, #0x20
        cmp     r8, #DRAM_DEV_DENSITY_512M
        beq     densCont

        /* This is an error. return */
        b       exit_ddrAutoConfig

densCont:
        MV_REG_WRITE_ASM (r6, r5, SDRAM_ADDR_CTRL_REG)

        /* Config DDR2 registers (Extended mode, ODTs and pad calibration)    */
        MV_REG_READ_ASM (r8, r5, SDRAM_CONFIG_REG)
        tst     r8, #SDRAM_DTYPE_DDR2
        beq     _extModeODTEnd


	/* Set DDR Extended Mode register for working with CS[0]	      */
        /* write 'nop' to SDRAM operation */
        mov     r6, #0x5                 /* 'NOP' command              */
        MV_REG_WRITE_ASM (r6, r5, SDRAM_OPERATION_REG)

        /* poll SDRAM operation. Make sure its back to normal operation       */
_sdramOpPoll3:
        ldr     r6, [r5]
        cmp     r6, #0                          /* '0' = Normal SDRAM Mode    */
        bne     _sdramOpPoll3

        /* Now its safe to write new value to SDRAM Extended Mode register    */
        ldr	r6, =DDR_SDRAM_EXT_MODE_CS0_DV
        MV_REG_WRITE_ASM (r6, r5, SDRAM_EXTENDED_MODE_REG)

        /* Make the Dunit write the DRAM its new extended mode                */
        mov     r6, #0x4                /* Extended Mode Register Set command */
        MV_REG_WRITE_ASM (r6, r5, SDRAM_OPERATION_REG)

        /* poll SDRAM operation. Make sure its back to normal operation       */
_sdramOpPoll4:
        ldr     r6, [r5]
        cmp     r6, #0                          /* '0' = Normal SDRAM Mode    */
        bne     _sdramOpPoll4

	/* ODT configuration is done for single bank CS[0] only		      */
        /* Config DDR2 On Die Termination (ODT) registers */
        ldr     r6, =DDR2_ODT_CTRL_LOW_CS0_DV
        MV_REG_WRITE_ASM (r6, r5, DDR2_SDRAM_ODT_CTRL_LOW_REG)

        ldr     r6, =DDR2_ODT_CTRL_HIGH_CS0_DV
        MV_REG_WRITE_ASM (r6, r5, DDR2_SDRAM_ODT_CTRL_HIGH_REG)

        ldr     r6, =DDR2_DUNIT_ODT_CTRL_CS0_DV
        MV_REG_WRITE_ASM (r6, r5, DDR2_DUNIT_ODT_CONTROL_REG)


        /* we will check what device we are running and perform
        Initialization according to device value */

_extModeODTEnd:

        /* Implement Guideline (GL# MEM-2) P_CAL Automatic Calibration  */
        /* Does Not Work for Address/Control and Data Pads.             */
        /* Relevant for: 88F5181-A1/B0 and 88F5281-A0                   */

	/* Read device ID  */
	MV_CTRL_MODEL_GET_ASM(r6, r5);
        /* Read device revision */
	MV_CTRL_REV_GET_ASM(r8, r5);

	/* Continue if OrionN */
        ldr     r5, =MV_5180_DEV_ID
        cmp     r6, r5
        bne     1f
        b     glMem2End
1:

	/* Continue if Orion1 and device revision B1 */
        ldr     r5, =MV_5181_DEV_ID
        cmp     r6, r5
        bne     1f

        cmp     r8, #MV_5181_B1_REV
        bge     glMem2End
        b       glMem2Start
1:

        /* Orion NAS */
        ldr     r5, =MV_5182_DEV_ID
        cmp     r6, r5
        beq     glMem2Start

        /* Orion Shark */
        ldr     r5, =MV_8660_DEV_ID
        cmp     r6, r5
        beq     glMem2Start

	b	glMem2End

glMem2Start:

        /* DDR SDRAM Address/Control Pads Calibration                   */
        MV_REG_READ_ASM (r6, r5, SDRAM_ADDR_CTRL_PADS_CAL_REG)

        /* Set Bit [31] to make the register writable                   */
        orr   r8, r6, #SDRAM_WR_EN

        MV_REG_WRITE_ASM (r8, r5, SDRAM_ADDR_CTRL_PADS_CAL_REG)

        bic   r6, r6, #SDRAM_WR_EN      /* Make register read-only      */
        bic   r6, r6, #SDRAM_TUNE_EN    /* Disable auto calibration     */
        bic   r6, r6, #SDRAM_DRVN_MASK  /* Clear r5[5:0]<DrvN>          */
        bic   r6, r6, #SDRAM_DRVP_MASK  /* Clear r5[11:6]<DrvP>         */

        /* Get the final N locked value of driving strength [22:17]     */
        mov   r5, r6
        mov   r5, r5, LSL #9
        mov   r5, r5, LSR #26    /* r5[5:0]<DrvN>  = r6[22:17]<LockN>   */
        orr   r5, r5, r5, LSL #6 /* r5[11:6]<DrvP> = r5[5:0]<DrvN>      */

        /* Write to both <DrvN> bits [5:0] and <DrvP> bits [11:6]       */
        orr   r6, r6, r5

        MV_REG_WRITE_ASM (r6, r5, SDRAM_ADDR_CTRL_PADS_CAL_REG)


        /* DDR SDRAM Data Pads Calibration                              */
        MV_REG_READ_ASM (r6, r5, SDRAM_DATA_PADS_CAL_REG)

        /* Set Bit [31] to make the register writable                   */
        orr   r8, r6, #SDRAM_WR_EN

        MV_REG_WRITE_ASM (r8, r5, SDRAM_DATA_PADS_CAL_REG)

        bic   r6, r6, #SDRAM_WR_EN      /* Make register read-only      */
        bic   r6, r6, #SDRAM_TUNE_EN    /* Disable auto calibration     */
        bic   r6, r6, #SDRAM_DRVN_MASK  /* Clear r5[5:0]<DrvN>          */
        bic   r6, r6, #SDRAM_DRVP_MASK  /* Clear r5[11:6]<DrvP>         */

        /* Get the final N locked value of driving strength [22:17]     */
        mov   r5, r6
        mov   r5, r5, LSL #9
        mov   r5, r5, LSR #26
        orr   r5, r5, r5, LSL #6 /* r5[5:0] = r6[22:17]<LockN>  */

        /* Write to both <DrvN> bits [5:0] and <DrvP> bits [11:6]       */
        orr   r6, r6, r5

        MV_REG_WRITE_ASM (r6, r5, SDRAM_DATA_PADS_CAL_REG)

glMem2End:
        /* Implement Guideline (GL# MEM-3) Drive Strength Value         */
        /* Relevant for: 88F5181-A1/B0/B1 and 88F5281-A0/B0             */

        /* Get SDRAM Config value */
        MV_REG_READ_ASM (r8, r5, SDRAM_CONFIG_REG)

        /* Get DIMM type */
        tst     r8, #SDRAM_DTYPE_DDR2
        beq     ddr1StrengthVal

ddr2StrengthVal:
        ldr     r4, =DDR2_ADDR_CTRL_PAD_STRENGTH_TYPICAL_DV
        ldr     r8, =DDR2_DATA_PAD_STRENGTH_TYPICAL_DV
        b       setDrvStrength
ddr1StrengthVal:
        ldr     r4, =DDR1_ADDR_CTRL_PAD_STRENGTH_TYPICAL_DV
        ldr     r8, =DDR1_DATA_PAD_STRENGTH_TYPICAL_DV

setDrvStrength:
        /* DDR SDRAM Address/Control Pads Calibration                   */
        MV_REG_READ_ASM (r6, r5, SDRAM_ADDR_CTRL_PADS_CAL_REG)

        orr   r6, r6, #SDRAM_WR_EN      /* Make register writeable      */

        MV_REG_WRITE_ASM (r6, r5, SDRAM_ADDR_CTRL_PADS_CAL_REG)
        HTOLL(r6,r5)

        bic   r6, r6, #SDRAM_WR_EN      /* Make register read-only      */
        bic   r6, r6, #SDRAM_PRE_DRIVER_STRENGTH_MASK
        orr   r6, r4, r6                /* Set default value for DDR    */

        MV_REG_WRITE_ASM (r6, r5, SDRAM_ADDR_CTRL_PADS_CAL_REG)


        /* DDR SDRAM Data Pads Calibration                              */
        MV_REG_READ_ASM (r6, r5, SDRAM_DATA_PADS_CAL_REG)

        orr   r6, r6, #SDRAM_WR_EN      /* Make register writeable      */

        MV_REG_WRITE_ASM (r6, r5, SDRAM_DATA_PADS_CAL_REG)
        HTOLL(r6,r5)

        bic   r6, r6, #SDRAM_WR_EN      /* Make register read-only      */
        bic   r6, r6, #SDRAM_PRE_DRIVER_STRENGTH_MASK
        orr   r6, r8, r6                /* Set default value for DDR    */

        MV_REG_WRITE_ASM (r6, r5, SDRAM_DATA_PADS_CAL_REG)


        /* Implement Guideline (GL# MEM-4) DQS Reference Delay Tuning   */
        /* Relevant for: 88F5181-A1/B0/B1 and 88F5281-A0/B0             */
        /* Get the "sample on reset" register for the DDR frequancy     */

#if defined(MV_RUN_FROM_FLASH)
	/* Calc the absolute address of the _cpuARMDDRCLK[] in the boot flash */
	ldr   r7, = _cpuARMDDRCLK
	ldr	r4, =_start
	ldr	r4, [r4]
	sub   r7, r7, r4
         ldr   r4, = Lrom_start_of_data
         ldr	r4, [r4]
         add   r7, r4, r7
#else
	/* Calc the absolute address of the _cpuARMDDRCLK[] in the boot flash */
	ldr     r7, = _cpuARMDDRCLK
	ldr	r4, =_start
	sub     r7, r7, r4
	add	r7, r7, #CFG_MONITOR_BASE
#endif
        /* Get the "sample on reset" register for the DDR frequancy     */
        MV_REG_READ_ASM (r4, r5, MPP_SAMPLE_AT_RESET)
        ldr     r5, =MSAR_ARMDDRCLCK_MASK
        and     r5, r4, r5
#if 0 /* YOTAM TO BE FIX */
	mov    r5, r5, LSR #MSAR_ARMDDRCLCK_OFFS
#endif

	/* Read device ID  */
	MV_CTRL_MODEL_GET_ASM(r6, r8);

        /* Continue if TC90 */
        ldr     r8, =MV_1281_DEV_ID
        cmp     r6, r6
        beq     armClkMsb

        /* Continue if Orion2 */
        ldr     r8, =MV_5281_DEV_ID
        cmp     r6, r8
#if 0 /* YOTAM TO BE FIX */
        bne     1f
#endif

armClkMsb:
#if 0 /* YOTAM TO BE FIX */
        tst    r4, #MSAR_ARMDDRCLCK_H_MASK
        beq    1f
        orr    r5, r5, #BIT4
1:
	ldr    r4, =MV_CPU_ARM_CLK_ELM_SIZE
	mul    r5, r4, r5
	add    r7, r7, r5
	add    r7, r7, #MV_CPU_ARM_CLK_DDR_OFF
	ldr    r5, [r7]
#endif

        /* Get SDRAM Config value */
        MV_REG_READ_ASM (r8, r4, SDRAM_CONFIG_REG)

        /* Get DIMM type */
        tst     r8, #SDRAM_DTYPE_DDR2
        beq     ddr1FtdllVal

ddr2FtdllVal:
        ldr    r4, =FTDLL_DDR2_250MHZ
	ldr    r7, =_250MHz
        cmp    r5, r7
        beq    setFtdllReg
        ldr     r4, =FTDLL_DDR2_200MHZ
	ldr    r7, =_200MHz
        cmp    r5, r7
        beq    setFtdllReg
        ldr     r4, =FTDLL_DDR2_166MHZ
	ldr    r7, =_166MHz
        cmp    r5, r7
        beq    setFtdllReg
        ldr     r4, =FTDLL_DDR2_133MHZ
        b       setFtdllReg

ddr1FtdllVal:
        ldr    r4, =FTDLL_DDR1_200MHZ
	ldr    r7, =_200MHz
        cmp    r5, r7
        beq    setFtdllReg
        ldr    r4, =FTDLL_DDR1_166MHZ
	ldr    r7, =_166MHz
        cmp    r5, r7
        beq    setFtdllReg
        ldr    r4, =FTDLL_DDR1_133MHZ
	ldr    r7, =_133MHz
        cmp    r5, r7
        beq    setFtdllReg
        ldr    r4, =0

setFtdllReg:

#if !defined(MV_88W8660) && !defined(MV_88F6183) && !defined(MV_88F6183L)
        MV_REG_READ_ASM (r8, r5, SDRAM_FTDLL_CONFIG_REG)
        orr    r8, r8, r4
        MV_REG_WRITE_ASM (r8, r5, SDRAM_FTDLL_CONFIG_REG)
        bic   r8, r8, #1
        MV_REG_WRITE_ASM (r8, r5, SDRAM_FTDLL_CONFIG_REG)
#endif /* !defined(MV_88W8660) && !defined(MV_88F6183) && !defined(MV_88F6183L)*/


setTimingReg:
        /* Set default Timing parameters */
        MV_REG_READ_ASM (r8, r5, SDRAM_CONFIG_REG)
        tst     r8, #SDRAM_DTYPE_DDR2
        bne     ddr2TimeParam

ddr1TimeParam:
        ldr     r6, =DDR1_TIMING_LOW_DV
        MV_REG_WRITE_ASM (r6, r5, SDRAM_TIMING_CTRL_LOW_REG)
        ldr     r6, =DDR1_TIMING_HIGH_DV
        MV_REG_WRITE_ASM (r6, r5, SDRAM_TIMING_CTRL_HIGH_REG)
        b       timeParamDone

ddr2TimeParam:
        ldr     r6, =DDR2_TIMING_LOW_DV
        MV_REG_WRITE_ASM (r6, r5, SDRAM_TIMING_CTRL_LOW_REG)
        ldr     r6, =DDR2_TIMING_HIGH_DV
        MV_REG_WRITE_ASM (r6, r5, SDRAM_TIMING_CTRL_HIGH_REG)

timeParamDone:
        /* Open CS[0] window to requested size and enable it. Disable other   */
	/* windows 							      */
        ldr	r6, =SCBAR_BASE_MASK
        sub     r3, r3, #1
        and	r3, r3, r6
	orr	r3, r3, #1	/* Enable bank */
        MV_REG_WRITE_ASM (r3, r5, SDRAM_SIZE_REG(0))
        ldr	r6, =0
        MV_REG_WRITE_ASM (r6, r5, SDRAM_SIZE_REG(1))
        MV_REG_WRITE_ASM (r6, r5, SDRAM_SIZE_REG(2))
        MV_REG_WRITE_ASM (r6, r5, SDRAM_SIZE_REG(3))

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

        mov     r4, #MV_BOARD_DIMM0_I2C_ADDR       /* reading from DIMM0      */
        mov     r7, #NUM_OF_ROWS_OFFSET            /* offset  3               */
        bl      _i2cRead
        mov     r8, r7                             /* r8 save number of rows  */

        mov     r4, #MV_BOARD_DIMM0_I2C_ADDR       /* reading from DIMM0      */
        mov     r7, #NUM_OF_COLS_OFFSET            /* offset  4               */
        bl      _i2cRead
        add     r8, r8, r7                         /* r8 = number of rows + number of col */

        mov     r7, #0x1
        mov     r8, r7, LSL r8                     /* r8 = (1 << r8)          */

        mov     r4, #MV_BOARD_DIMM0_I2C_ADDR       /* reading from DIMM0      */
        mov     r7, #SDRAM_WIDTH_OFFSET            /* offset 13 */
        bl      _i2cRead
        mul     r8, r7, r8

        mov     r4, #MV_BOARD_DIMM0_I2C_ADDR       /* reading from DIMM0      */
        mov     r7, #NUM_OF_BANKS_OFFSET           /* offset 17               */
        bl      _i2cRead
        mul     r7, r8, r7

	mov     PC, r13

/**************************/
_get_width:
	mov     r13, LR                 /* Save link register */

        /* Get SDRAM width (SPD offset 13) */
        mov     r4, #MV_BOARD_DIMM0_I2C_ADDR /* reading from DIMM0            */
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

        /* Get registered/non registered info from DIMM */
        tst     r8, #SDRAM_DTYPE_DDR2
        bne     regDdr2

regDdr1:
        mov     r4, #MV_BOARD_DIMM0_I2C_ADDR /* reading from DIMM0            */
        mov     r7, #SDRAM_MODULES_ATTR_OFFSET
        bl      _i2cRead                /* result in r7                       */
        tst     r7, #0x2
	b	exit
regDdr2:
        mov     r4, #MV_BOARD_DIMM0_I2C_ADDR /* reading from DIMM0            */
        mov     r7, #DIMM_TYPE_INFO_OFFSET
        bl      _i2cRead                /* result in r7                       */
        tst     r7, #0x11               /* DIMM type = regular RDIMM (0x01)   */
                                        /* or Mini-RDIMM (0x10)               */
exit:
        mov     PC, r13


#endif

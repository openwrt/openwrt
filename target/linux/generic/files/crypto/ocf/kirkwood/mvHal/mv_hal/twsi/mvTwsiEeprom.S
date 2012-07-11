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


/* includes */
#define MV_ASMLANGUAGE
#include "ctrlEnv/mvCtrlEnvSpec.h"
#include "boardEnv/mvBoardEnvSpec.h"
#include "mvOsAsm.h"
#include "mvTwsiSpec.h"
#include "mvSysHwConfig.h"
#include "ctrlEnv/sys/mvCpuIfRegs.h"
#include "mvCommon.h"

#define I2C_CH MV_BOARD_DIMM_I2C_CHANNEL

/* defines */
/* defines  */


        .data
        .global _i2cInit
        .global _i2cRead

        .text

/*******************************************************************************
* _i2cInit - Initialize TWSI interface
*
* DESCRIPTION:
*       The function performs TWSI interface initialization. It resets the 
*       TWSI state machine and initialize its clock to 100KHz assuming Tclock
*       of 133MHz.
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
_i2cInit:        
        mov     r9, LR     /* Save link register */
        mov     r0, #0     /* Make sure r0 is zero */        
       	
        /* Reset the i2c Mechanism first */
        MV_REG_WRITE_ASM (r0, r1, TWSI_SOFT_RESET_REG(I2C_CH))

        bl      _twsiDelay        
        bl      _twsiDelay              

        /* Initializing the I2C mechanism. Assuming Tclock frequency          */
        /* of 166MHz. The I2C frequency in that case will be 100KHz.          */
        /* For this settings, M = 9 and N = 3. Set the baud-rate with the     */
        /* value of 0x2b (freq of ==> 100KHz                                  */
        /* see spec for more details about the calculation of this value)     */
        mov     r6, #(9 << 3 | 3)
        MV_REG_WRITE_ASM (r6, r1, TWSI_STATUS_BAUDE_RATE_REG(I2C_CH))
        
        /* Enable the I2C master */
	/* Enable TWSI interrupt in main mask reg */
        mov     r6, #0xC4
        MV_REG_WRITE_ASM (r6, r1, TWSI_CONTROL_REG(I2C_CH))
        
        /* Let the slow TWSI machine get used to the idea that it is enabled  */
        bl      _twsiDelay
        
        
        mov     PC, r9         /* r9 is saved link register */

/*******************************************************************************
* _twsiDelay - Perform delay.
*
* DESCRIPTION:
*       The function performs a delay to enable TWSI logic to stable.
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
_twsiDelay:
        mov     r10, #0x100000 /*was 0x400*/ 
    
_twsiDelayLoop:
        subs    r10, r10, #1
        bne     _twsiDelayLoop
        
        mov     PC, LR

/*******************************************************************************
* _i2cRead - Read byte from I2C EEPROM device.
*
* DESCRIPTION:
*       The function returns a byte from I2C EEPROM device. 
*       The EEPROM device is 7-bit address type. 
*
* INPUT:
*       r4 has the DIMM0 base address with shift 1 bit to the left
*       r7 has the EEPROM offset
*
* OUTPUT:
*       None.
*
* RETURN:
*       r4 returns '0' if address can not be read.
*       r7 has byte value in case read is successful.
*
*******************************************************************************/
_i2cRead:
        mov     r9, LR     /* Save link register */
       
        /* Transmit the device address and desired offset within the EEPROM. */
        
        /* Generate Start Bit */
        MV_REG_READ_ASM (r6, r1, TWSI_CONTROL_REG(I2C_CH))
        orr     r6, r6, #TWSI_CONTROL_START_BIT
        MV_REG_WRITE_ASM (r6, r1, TWSI_CONTROL_REG(I2C_CH))

        /* Wait for the interrupt flag (bit3) to be set  */
        mov     r10, #0x50000        
loop_1:
        subs    r10, r10, #1
        beq     loop_1_timeout
#ifdef MV78XX0
        MV_REG_READ_ASM (r6, r1, CPU_INT_LOW_REG(I2C_CH))
        tst     r6, #BIT2
#else
        MV_REG_READ_ASM (r6, r1, CPU_MAIN_INT_CAUSE_REG) 
        tst     r6, #BIT5
#endif
        beq     loop_1
            
loop_1_timeout:        
            
        /* Wait for the start bit to be reset by HW */
        mov     r10, #0x50000 
loop_2:
        subs    r10, r10, #1
        beq     loop_2_timeout
        MV_REG_READ_ASM (r6, r1, TWSI_CONTROL_REG(I2C_CH))
        tst     r6, #TWSI_CONTROL_START_BIT
        bne     loop_2
           
loop_2_timeout:

        /* Wait for the status TWSI_START_CONDITION_TRA = 0x8 */
        mov     r10, #0x50000
loop_3:
        subs    r10, r10, #1
        beq     loop_3_timeout
        MV_REG_READ_ASM (r6, r1, TWSI_STATUS_BAUDE_RATE_REG(I2C_CH))
        cmp     r6, #0x08
        bne     loop_3
        
loop_3_timeout:
        
        /* writing the address of (DIMM0/1 << 1) with write indication */
        mov     r6, r4, LSL #1 /* Write operation address bit 0 must be 0 */
        MV_REG_WRITE_ASM (r6, r1, TWSI_DATA_REG(I2C_CH))
        
        bl      _twsiDelay
        /* Clear the interrupt flag */
        MV_REG_READ_ASM (r6, r1, TWSI_CONTROL_REG(I2C_CH))
        bic     r6, r6, #TWSI_CONTROL_INT_FLAG_SET
        MV_REG_WRITE_ASM (r6, r1, TWSI_CONTROL_REG(I2C_CH))
        bl      _twsiDelay
        
        /* Waiting for the interrupt flag to be set which means that the
           address has been transmitted                                  */
loop_4:
#ifdef MV78XX0
        MV_REG_READ_ASM (r6, r1, CPU_INT_LOW_REG(I2C_CH))
        tst     r6, #BIT2
#else
        MV_REG_READ_ASM (r6, r1, CPU_MAIN_INT_CAUSE_REG)
        tst     r6, #BIT5
#endif
        beq     loop_4       /* if tst = 0, then the bit is not set yet */
        
        /* Wait for status TWSI_ADDR_PLUS_WRITE_BIT_TRA_ACK_REC = 0x18 */
        mov     r10, #0x50000         /* Set r10 to 0x50000 =~ 328,000 */

loop_5:
        subs    r10, r10, #1          /* timeout count down         */
        bne     testStatus
        mov     r4, #0                /* r4 = 0 -> operation failed */  
        b       exit_i2cRead          /* Exit if timeout (No DIMM)  */

testStatus:    
        MV_REG_READ_ASM (r6, r1, TWSI_STATUS_BAUDE_RATE_REG(I2C_CH))
        cmp     r6, #0x18
        bne     loop_5
                

        /* check if the offset is bigger than 256 byte*/
        tst     r7, #0x80000000
        bne     great_than_256
                        
        /* Write the offset to be read from the DIMM EEPROM */
        MV_REG_WRITE_ASM (r7, r1, TWSI_DATA_REG(I2C_CH))
        
        b after_offset
        
great_than_256:
        mov     r10, r7, LSR #8
        and     r10, r10, #0xff
        /* Write the offset0 to be read from the  EEPROM */
        MV_REG_WRITE_ASM (r10, r1, TWSI_DATA_REG(I2C_CH))
        
        /* Clear the interrupt flag ==> signaling that the address can now
           be transmited                                                    */
        
        bl      _twsiDelay
        MV_REG_READ_ASM (r6, r1, TWSI_CONTROL_REG(I2C_CH))
        bic     r6, r6, #TWSI_CONTROL_INT_FLAG_SET
        MV_REG_WRITE_ASM (r6, r1, TWSI_CONTROL_REG(I2C_CH))
        bl      _twsiDelay
    
        /* Wait for the interrupt to be set again ==> address has transmited */
loop_6_1:
#ifdef MV78XX0
        MV_REG_READ_ASM (r6, r1, CPU_INT_LOW_REG(I2C_CH))
        tst     r6, #BIT2
#else
        MV_REG_READ_ASM (r6, r1, CPU_MAIN_INT_CAUSE_REG)
        tst     r6, #BIT5
#endif
        beq     loop_6_1
        
        /* Wait for status TWSI_MAS_TRAN_DATA_BYTE_ACK_REC = 0x28 */
loop_7_1:
        MV_REG_READ_ASM (r6, r1, TWSI_STATUS_BAUDE_RATE_REG(I2C_CH))
        cmp     r6, #0x28
        bne     loop_7_1
        
        
        mov     r10, r7
        and     r10, r10, #0xff
        /* Write the offset1 to be read from the  EEPROM */
        MV_REG_WRITE_ASM (r10, r1, TWSI_DATA_REG(I2C_CH))
        
        
        
after_offset:                
        
        /* Clear the interrupt flag ==> signaling that the address can now
           be transmited                                                    */
        
        bl      _twsiDelay
        MV_REG_READ_ASM (r6, r1, TWSI_CONTROL_REG(I2C_CH))
        bic     r6, r6, #TWSI_CONTROL_INT_FLAG_SET
        MV_REG_WRITE_ASM (r6, r1, TWSI_CONTROL_REG(I2C_CH))
        bl      _twsiDelay
    
        /* Wait for the interrupt to be set again ==> address has transmited */
loop_6:
#ifdef MV78XX0
        MV_REG_READ_ASM (r6, r1, CPU_INT_LOW_REG(I2C_CH))
        tst     r6, #BIT2
#else
        MV_REG_READ_ASM (r6, r1, CPU_MAIN_INT_CAUSE_REG)
        tst     r6, #BIT5
#endif
        beq     loop_6
        
        /* Wait for status TWSI_MAS_TRAN_DATA_BYTE_ACK_REC = 0x28 */
loop_7:
        MV_REG_READ_ASM (r6, r1, TWSI_STATUS_BAUDE_RATE_REG(I2C_CH))
        cmp     r6, #0x28
        bne     loop_7
        
        /* Retransmit the device address with read indication to get the data */
        
        /* generate a repeated start bit */
        MV_REG_READ_ASM (r6, r1, TWSI_CONTROL_REG(I2C_CH))
        orr     r6, r6, #TWSI_CONTROL_START_BIT
        MV_REG_WRITE_ASM (r6, r1, TWSI_CONTROL_REG(I2C_CH))

        
        /* Clear the interrupt flag ==> the start bit will be transmitted. */ 
        bl      _twsiDelay
        MV_REG_READ_ASM (r6, r1, TWSI_CONTROL_REG(I2C_CH))
        bic     r6, r6, #TWSI_CONTROL_INT_FLAG_SET
        MV_REG_WRITE_ASM (r6, r1, TWSI_CONTROL_REG(I2C_CH))
        bl      _twsiDelay
        
       /* Wait for the interrupt flag (bit3) to be set */
loop_9:
#ifdef MV78XX0
        MV_REG_READ_ASM (r6, r1, CPU_INT_LOW_REG(I2C_CH))
        tst     r6, #BIT2
#else
        MV_REG_READ_ASM (r6, r1, CPU_MAIN_INT_CAUSE_REG)
        tst     r6, #BIT5
#endif
        beq     loop_9

        /* Wait for the start bit to be reset by HW */
loop_8:
        MV_REG_READ_ASM (r6, r1, TWSI_CONTROL_REG(I2C_CH)) 
        tst     r6, #TWSI_CONTROL_START_BIT
        bne     loop_8
        
        /* Wait for status TWSI_REPEATED_START_CONDITION_TRA = 0x10 */
loop_10:
        MV_REG_READ_ASM (r6, r1, TWSI_STATUS_BAUDE_RATE_REG(I2C_CH))
        cmp     r6, #0x10
        bne     loop_10
        
        /* Writing the address of (DIMM0<<1) with read indication (bit0 is 1) */
        mov     r6, r4, LSL #1
        orr     r6, r6, #1     /* Read operation address bit 0 must be 1 */
        MV_REG_WRITE_ASM (r6, r1, TWSI_DATA_REG(I2C_CH))
        
        /* Clear the interrupt flag ==> the address will be transmitted */
        bl      _twsiDelay
        MV_REG_READ_ASM (r6, r1, TWSI_CONTROL_REG(I2C_CH))
        bic     r6, r6, #TWSI_CONTROL_INT_FLAG_SET
        MV_REG_WRITE_ASM (r6, r1, TWSI_CONTROL_REG(I2C_CH))
        bl      _twsiDelay
        
        /* Wait for the interrupt flag (bit3) to be set as a result of
           transmitting the address.                                     */
loop_11:
#ifdef MV78XX0
        MV_REG_READ_ASM (r6, r1, CPU_INT_LOW_REG(I2C_CH))
        tst     r6, #BIT2
#else
        MV_REG_READ_ASM (r6, r1, CPU_MAIN_INT_CAUSE_REG)
        tst     r6, #BIT5
#endif
        beq     loop_11
        
         /* Wait for status TWSI_ADDR_PLUS_READ_BIT_TRA_ACK_REC = 0x40 */
loop_12:
        MV_REG_READ_ASM (r6, r1, TWSI_STATUS_BAUDE_RATE_REG(I2C_CH))
        cmp     r6, #0x40
        bne     loop_12
        
        /* Clear the interrupt flag and the Acknoledge bit */
        bl      _twsiDelay
        MV_REG_READ_ASM (r6, r1, TWSI_CONTROL_REG(I2C_CH))
        bic     r6, r6, #(TWSI_CONTROL_INT_FLAG_SET | TWSI_CONTROL_ACK)
        MV_REG_WRITE_ASM (r6, r1, TWSI_CONTROL_REG(I2C_CH))
        bl      _twsiDelay
        
        /* Wait for the interrupt flag (bit3) to be set */
loop_14:
#ifdef MV78XX0
        MV_REG_READ_ASM (r6, r1, CPU_INT_LOW_REG(I2C_CH))
        tst     r6, #BIT2
#else
        MV_REG_READ_ASM (r6, r1, CPU_MAIN_INT_CAUSE_REG)
        tst     r6, #BIT5
#endif
        beq     loop_14
        
        /* Wait for status TWSI_MAS_REC_READ_DATA_ACK_NOT_TRA = 0x58 */
loop_15:
        MV_REG_READ_ASM (r6, r1, TWSI_STATUS_BAUDE_RATE_REG(I2C_CH))
        cmp     r6, #0x58
        bne     loop_15
        
        /* Store the data in r7. */
        MV_REG_READ_ASM (r7, r1, TWSI_DATA_REG(I2C_CH))
        
        /* Generate stop bit */
        MV_REG_READ_ASM (r6, r1, TWSI_CONTROL_REG(I2C_CH))
        orr     r6, r6, #TWSI_CONTROL_STOP_BIT
        MV_REG_WRITE_ASM (r6, r1, TWSI_CONTROL_REG(I2C_CH))

        
        /* Clear the interrupt flag  */
        bl      _twsiDelay
        MV_REG_READ_ASM (r6, r1, TWSI_CONTROL_REG(I2C_CH))
        bic     r6, r6, #TWSI_CONTROL_INT_FLAG_SET
        MV_REG_WRITE_ASM (r6, r1, TWSI_CONTROL_REG(I2C_CH))
        bl      _twsiDelay
        
        /* Wait for the stop bit to be reset by HW */
loop_16:
        MV_REG_READ_ASM (r6, r1, TWSI_CONTROL_REG(I2C_CH))
        tst     r6, #TWSI_CONTROL_INT_FLAG_SET
        bne     loop_16

exit_i2cRead:
        mov     PC, r9         /* r9 is saved link register */

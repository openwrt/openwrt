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
/****************************************/
/* TWSI Registers                        */
/****************************************/
#ifndef __INCmvTwsiSpech
#define __INCmvTwsiSpech

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* defines */
#define TWSI_SLAVE_ADDR_REG(chanNum)	(TWSI_SLAVE_BASE(chanNum)+ 0x00)

#define TWSI_SLAVE_ADDR_GCE_ENA		BIT0
#define TWSI_SLAVE_ADDR_7BIT_OFFS	0x1
#define TWSI_SLAVE_ADDR_7BIT_MASK 	(0xFF << TWSI_SLAVE_ADDR_7BIT_OFFS)
#define TWSI_SLAVE_ADDR_10BIT_OFFS	0x7
#define TWSI_SLAVE_ADDR_10BIT_MASK 	0x300
#define	TWSI_SLAVE_ADDR_10BIT_CONST 	0xF0


#define TWSI_EXTENDED_SLAVE_ADDR_REG(chanNum)	(TWSI_SLAVE_BASE(chanNum) + 0x10)
#define TWSI_EXTENDED_SLAVE_OFFS 	0
#define TWSI_EXTENDED_SLAVE_MASK	(0xFF << TWSI_EXTENDED_SLAVE_OFFS)


#define TWSI_DATA_REG(chanNum)		(TWSI_SLAVE_BASE(chanNum) + 0x04)
#define TWSI_DATA_COMMAND_OFFS		0x0
#define TWSI_DATA_COMMAND_MASK 		(0x1 << TWSI_DATA_COMMAND_OFFS)
#define TWSI_DATA_COMMAND_WR		(0x1 << TWSI_DATA_COMMAND_OFFS)
#define TWSI_DATA_COMMAND_RD		(0x0 << TWSI_DATA_COMMAND_OFFS)
#define TWSI_DATA_ADDR_7BIT_OFFS	0x1
#define TWSI_DATA_ADDR_7BIT_MASK 	(0xFF << TWSI_DATA_ADDR_7BIT_OFFS)
#define TWSI_DATA_ADDR_10BIT_OFFS	0x7
#define TWSI_DATA_ADDR_10BIT_MASK	0x300
#define TWSI_DATA_ADDR_10BIT_CONST	0xF0


#define TWSI_CONTROL_REG(chanNum)	(TWSI_SLAVE_BASE(chanNum) + 0x08)
#define TWSI_CONTROL_ACK            	BIT2
#define TWSI_CONTROL_INT_FLAG_SET   	BIT3
#define TWSI_CONTROL_STOP_BIT    	BIT4
#define TWSI_CONTROL_START_BIT 		BIT5 
#define TWSI_CONTROL_ENA     		BIT6
#define TWSI_CONTROL_INT_ENA    	BIT7


#define TWSI_STATUS_BAUDE_RATE_REG(chanNum)	(TWSI_SLAVE_BASE(chanNum) + 0x0c)
#define TWSI_BAUD_RATE_N_OFFS		0
#define TWSI_BAUD_RATE_N_MASK		(0x7 << TWSI_BAUD_RATE_N_OFFS)
#define TWSI_BAUD_RATE_M_OFFS   	3
#define TWSI_BAUD_RATE_M_MASK  		(0xF << TWSI_BAUD_RATE_M_OFFS)

#define TWSI_SOFT_RESET_REG(chanNum)	(TWSI_SLAVE_BASE(chanNum) + 0x1c)

/* defines */
#define TWSI_TIMEOUT_VALUE 		0x500 

/* TWSI status codes */
#define TWSI_BUS_ERROR                                            0x00
#define TWSI_START_CON_TRA                                        0x08
#define TWSI_REPEATED_START_CON_TRA                               0x10
#define TWSI_AD_PLS_WR_BIT_TRA_ACK_REC                            0x18
#define TWSI_AD_PLS_WR_BIT_TRA_ACK_NOT_REC                        0x20
#define TWSI_M_TRAN_DATA_BYTE_ACK_REC                             0x28
#define TWSI_M_TRAN_DATA_BYTE_ACK_NOT_REC                         0x30
#define TWSI_M_LOST_ARB_DUR_AD_OR_DATA_TRA                        0x38
#define TWSI_AD_PLS_RD_BIT_TRA_ACK_REC                            0x40
#define TWSI_AD_PLS_RD_BIT_TRA_ACK_NOT_REC                        0x48
#define TWSI_M_REC_RD_DATA_ACK_TRA                                0x50
#define TWSI_M_REC_RD_DATA_ACK_NOT_TRA                            0x58
#define TWSI_SLA_REC_AD_PLS_WR_BIT_ACK_TRA                        0x60
#define TWSI_M_LOST_ARB_DUR_AD_TRA_AD_IS_TRGT_TO_SLA_ACK_TRA_W    0x68
#define TWSI_GNL_CALL_REC_ACK_TRA                                 0x70
#define TWSI_M_LOST_ARB_DUR_AD_TRA_GNL_CALL_AD_REC_ACK_TRA        0x78
#define TWSI_SLA_REC_WR_DATA_AF_REC_SLA_AD_ACK_TRAN               0x80
#define TWSI_SLA_REC_WR_DATA_AF_REC_SLA_AD_ACK_NOT_TRAN           0x88
#define TWSI_SLA_REC_WR_DATA_AF_REC_GNL_CALL_ACK_TRAN             0x90
#define TWSI_SLA_REC_WR_DATA_AF_REC_GNL_CALL_ACK_NOT_TRAN         0x98
#define TWSI_SLA_REC_STOP_OR_REPEATED_STRT_CON                    0xA0
#define TWSI_SLA_REC_AD_PLS_RD_BIT_ACK_TRA                        0xA8
#define TWSI_M_LOST_ARB_DUR_AD_TRA_AD_IS_TRGT_TO_SLA_ACK_TRA_R    0xB0
#define TWSI_SLA_TRA_RD_DATA_ACK_REC                              0xB8
#define TWSI_SLA_TRA_RD_DATA_ACK_NOT_REC                          0xC0
#define TWSI_SLA_TRA_LAST_RD_DATA_ACK_REC                         0xC8
#define TWSI_SEC_AD_PLS_WR_BIT_TRA_ACK_REC                        0xD0
#define TWSI_SEC_AD_PLS_WR_BIT_TRA_ACK_NOT_REC                    0xD8
#define TWSI_SEC_AD_PLS_RD_BIT_TRA_ACK_REC                        0xE0
#define TWSI_SEC_AD_PLS_RD_BIT_TRA_ACK_NOT_REC                    0xE8
#define TWSI_NO_REL_STS_INT_FLAG_IS_KEPT_0                        0xF8


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __INCmvTwsiSpech */

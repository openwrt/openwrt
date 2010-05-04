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

#ifndef __INCmvSpiSpecH
#define __INCmvSpiSpecH

/* Constants */
#define		MV_SPI_WAIT_RDY_MAX_LOOP			100000
#define		MV_SPI_16_BIT_CHUNK_SIZE			2
#define		MV_SPI_DUMMY_WRITE_16BITS			0xFFFF
#define		MV_SPI_DUMMY_WRITE_8BITS			0xFF

/* Marvell Flash Device Controller Registers */
#define		MV_SPI_CTRLR_OFST					0x10600
#define		MV_SPI_IF_CTRL_REG					(MV_SPI_CTRLR_OFST + 0x00)
#define		MV_SPI_IF_CONFIG_REG				(MV_SPI_CTRLR_OFST + 0x04)
#define		MV_SPI_DATA_OUT_REG					(MV_SPI_CTRLR_OFST + 0x08)
#define		MV_SPI_DATA_IN_REG					(MV_SPI_CTRLR_OFST + 0x0c)
#define		MV_SPI_INT_CAUSE_REG				(MV_SPI_CTRLR_OFST + 0x10)
#define		MV_SPI_INT_CAUSE_MASK_REG			(MV_SPI_CTRLR_OFST + 0x14)

/* Serial Memory Interface Control Register Masks */
#define		MV_SPI_CS_ENABLE_OFFSET				0		/* bit 0 */
#define		MV_SPI_MEMORY_READY_OFFSET			1		/* bit 1 */
#define		MV_SPI_CS_ENABLE_MASK				(0x1  << MV_SPI_CS_ENABLE_OFFSET)
#define		MV_SPI_MEMORY_READY_MASK			(0x1  << MV_SPI_MEMORY_READY_OFFSET)

/* Serial Memory Interface Configuration Register Masks */
#define		MV_SPI_CLK_PRESCALE_OFFSET			0		/* bit 0-4 */
#define		MV_SPI_BYTE_LENGTH_OFFSET			5		/* bit 5 */
#define		MV_SPI_ADDRESS_BURST_LENGTH_OFFSET  8	    /* bit 8-9 */
#define		MV_SPI_CLK_PRESCALE_MASK			(0x1F << MV_SPI_CLK_PRESCALE_OFFSET)
#define		MV_SPI_BYTE_LENGTH_MASK				(0x1  << MV_SPI_BYTE_LENGTH_OFFSET)
#define		MV_SPI_ADDRESS_BURST_LENGTH_MASK	(0x3  << MV_SPI_ADDRESS_BURST_LENGTH_OFFSET)

#endif /* __INCmvSpiSpecH */


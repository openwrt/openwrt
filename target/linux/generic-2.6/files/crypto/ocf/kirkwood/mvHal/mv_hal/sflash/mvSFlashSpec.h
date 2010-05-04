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

#ifndef __INCmvSFlashSpecH
#define __INCmvSFlashSpecH

/* Constants */
#define		MV_SFLASH_READ_CMND_LENGTH		    4		/* 1B opcode + 3B address */
#define		MV_SFLASH_SE_CMND_LENGTH		    4		/* 1B opcode + 3B address */
#define		MV_SFLASH_BE_CMND_LENGTH		    1		/* 1B opcode */
#define		MV_SFLASH_PP_CMND_LENGTH		    4		/* 1B opcode + 3B address */
#define		MV_SFLASH_WREN_CMND_LENGTH		    1		/* 1B opcode */
#define		MV_SFLASH_WRDI_CMND_LENGTH		    1		/* 1B opcode */
#define		MV_SFLASH_RDID_CMND_LENGTH		    1		/* 1B opcode */
#define		MV_SFLASH_RDID_REPLY_LENGTH		    3		/* 1B manf ID and 2B device ID */
#define		MV_SFLASH_RDSR_CMND_LENGTH		    1		/* 1B opcode */
#define		MV_SFLASH_RDSR_REPLY_LENGTH		    1		/* 1B status */
#define		MV_SFLASH_WRSR_CMND_LENGTH		    2		/* 1B opcode + 1B status value */
#define		MV_SFLASH_DP_CMND_LENGTH		    1		/* 1B opcode */
#define		MV_SFLASH_RES_CMND_LENGTH		    1		/* 1B opcode */

/* Status Register Bit Masks */
#define		MV_SFLASH_STATUS_REG_WIP_OFFSET	    0	    /* bit 0; write in progress */
#define		MV_SFLASH_STATUS_REG_WP_OFFSET	    2       /* bit 2-4; write protect option */
#define		MV_SFLASH_STATUS_REG_SRWD_OFFSET	7	    /* bit 7; lock status register write */
#define		MV_SFLASH_STATUS_REG_WIP_MASK	    (0x1 << MV_SFLASH_STATUS_REG_WIP_OFFSET)
#define		MV_SFLASH_STATUS_REG_SRWD_MASK	    (0x1 << MV_SFLASH_STATUS_REG_SRWD_OFFSET)

#define		MV_SFLASH_MAX_WAIT_LOOP			    1000000
#define     MV_SFLASH_CHIP_ERASE_MAX_WAIT_LOOP  0x50000000

#define		MV_SFLASH_DEFAULT_RDID_OPCD		    0x9F	/* Default Read ID */
#define     MV_SFLASH_DEFAULT_WREN_OPCD         0x06	/* Default Write Enable */
#define     MV_SFLASH_NO_SPECIFIC_OPCD          0x00

/********************************/
/*  ST M25Pxxx Device Specific  */
/********************************/

/* Manufacturer IDs and Device IDs for SFLASHs supported by the driver */
#define     MV_M25PXXX_ST_MANF_ID               0x20
#define     MV_M25P32_DEVICE_ID                 0x2016
#define     MV_M25P32_MAX_SPI_FREQ              20000000    /* 20MHz */
#define     MV_M25P32_MAX_FAST_SPI_FREQ         50000000    /* 50MHz */
#define     MV_M25P32_FAST_READ_DUMMY_BYTES     1
#define     MV_M25P64_DEVICE_ID                 0x2017
#define     MV_M25P64_MAX_SPI_FREQ              20000000    /* 20MHz */
#define     MV_M25P64_MAX_FAST_SPI_FREQ         50000000    /* 50MHz */
#define     MV_M25P64_FAST_READ_DUMMY_BYTES     1
#define     MV_M25P128_DEVICE_ID                0x2018
#define     MV_M25P128_MAX_SPI_FREQ             20000000    /* 20MHz */
#define     MV_M25P128_MAX_FAST_SPI_FREQ        50000000    /* 50MHz */
#define     MV_M25P128_FAST_READ_DUMMY_BYTES    1


/* Sector Sizes and population per device model*/
#define     MV_M25P32_SECTOR_SIZE               0x10000 /* 64K */
#define     MV_M25P64_SECTOR_SIZE               0x10000 /* 64K */
#define     MV_M25P128_SECTOR_SIZE              0x40000 /* 256K */
#define     MV_M25P32_SECTOR_NUMBER             64
#define     MV_M25P64_SECTOR_NUMBER             128
#define     MV_M25P128_SECTOR_NUMBER            64
#define		MV_M25P_PAGE_SIZE				    0x100   /* 256 byte */

#define		MV_M25P_WREN_CMND_OPCD			    0x06	/* Write Enable */
#define		MV_M25P_WRDI_CMND_OPCD			    0x04	/* Write Disable */
#define		MV_M25P_RDID_CMND_OPCD			    0x9F	/* Read ID */
#define		MV_M25P_RDSR_CMND_OPCD			    0x05	/* Read Status Register */
#define		MV_M25P_WRSR_CMND_OPCD			    0x01	/* Write Status Register */
#define		MV_M25P_READ_CMND_OPCD			    0x03	/* Sequential Read */
#define		MV_M25P_FAST_RD_CMND_OPCD		    0x0B	/* Fast Read */
#define		MV_M25P_PP_CMND_OPCD			    0x02	/* Page Program */
#define		MV_M25P_SE_CMND_OPCD			    0xD8	/* Sector Erase */
#define		MV_M25P_BE_CMND_OPCD			    0xC7	/* Bulk Erase */
#define		MV_M25P_RES_CMND_OPCD			    0xAB	/* Read Electronic Signature */

/* Status Register Write Protect Bit Masks - 3bits */
#define		MV_M25P_STATUS_REG_WP_MASK	        (0x07 << MV_SFLASH_STATUS_REG_WP_OFFSET)
#define     MV_M25P_STATUS_BP_NONE              (0x00 << MV_SFLASH_STATUS_REG_WP_OFFSET)
#define     MV_M25P_STATUS_BP_1_OF_64           (0x01 << MV_SFLASH_STATUS_REG_WP_OFFSET)
#define     MV_M25P_STATUS_BP_1_OF_32           (0x02 << MV_SFLASH_STATUS_REG_WP_OFFSET)
#define     MV_M25P_STATUS_BP_1_OF_16           (0x03 << MV_SFLASH_STATUS_REG_WP_OFFSET)
#define     MV_M25P_STATUS_BP_1_OF_8            (0x04 << MV_SFLASH_STATUS_REG_WP_OFFSET)
#define     MV_M25P_STATUS_BP_1_OF_4            (0x05 << MV_SFLASH_STATUS_REG_WP_OFFSET)
#define     MV_M25P_STATUS_BP_1_OF_2            (0x06 << MV_SFLASH_STATUS_REG_WP_OFFSET)
#define     MV_M25P_STATUS_BP_ALL               (0x07 << MV_SFLASH_STATUS_REG_WP_OFFSET)

/************************************/
/*  MXIC MX25L6405 Device Specific  */
/************************************/

/* Manufacturer IDs and Device IDs for SFLASHs supported by the driver */
#define     MV_MXIC_MANF_ID                     0xC2
#define     MV_MX25L6405_DEVICE_ID              0x2017
#define     MV_MX25L6405_MAX_SPI_FREQ           20000000    /* 20MHz */
#define     MV_MX25L6405_MAX_FAST_SPI_FREQ      50000000    /* 50MHz */
#define     MV_MX25L6405_FAST_READ_DUMMY_BYTES  1
#define     MV_MXIC_DP_EXIT_DELAY               30          /* 30 ms */

/* Sector Sizes and population per device model*/
#define     MV_MX25L6405_SECTOR_SIZE            0x10000 /* 64K */
#define     MV_MX25L6405_SECTOR_NUMBER          128
#define		MV_MXIC_PAGE_SIZE			        0x100   /* 256 byte */

#define		MV_MX25L_WREN_CMND_OPCD			    0x06	/* Write Enable */
#define		MV_MX25L_WRDI_CMND_OPCD			    0x04	/* Write Disable */
#define		MV_MX25L_RDID_CMND_OPCD			    0x9F	/* Read ID */
#define		MV_MX25L_RDSR_CMND_OPCD			    0x05	/* Read Status Register */
#define		MV_MX25L_WRSR_CMND_OPCD			    0x01	/* Write Status Register */
#define		MV_MX25L_READ_CMND_OPCD			    0x03	/* Sequential Read */
#define		MV_MX25L_FAST_RD_CMND_OPCD		    0x0B	/* Fast Read */
#define		MV_MX25L_PP_CMND_OPCD			    0x02	/* Page Program */
#define		MV_MX25L_SE_CMND_OPCD			    0xD8	/* Sector Erase */
#define		MV_MX25L_BE_CMND_OPCD			    0xC7	/* Bulk Erase */
#define     MV_MX25L_DP_CMND_OPCD               0xB9    /* Deep Power Down */
#define		MV_MX25L_RES_CMND_OPCD			    0xAB	/* Read Electronic Signature */

/* Status Register Write Protect Bit Masks - 4bits */
#define		MV_MX25L_STATUS_REG_WP_MASK	        (0x0F << MV_SFLASH_STATUS_REG_WP_OFFSET)
#define     MV_MX25L_STATUS_BP_NONE             (0x00 << MV_SFLASH_STATUS_REG_WP_OFFSET)
#define     MV_MX25L_STATUS_BP_1_OF_128         (0x01 << MV_SFLASH_STATUS_REG_WP_OFFSET)
#define     MV_MX25L_STATUS_BP_1_OF_64          (0x02 << MV_SFLASH_STATUS_REG_WP_OFFSET)
#define     MV_MX25L_STATUS_BP_1_OF_32          (0x03 << MV_SFLASH_STATUS_REG_WP_OFFSET)
#define     MV_MX25L_STATUS_BP_1_OF_16          (0x04 << MV_SFLASH_STATUS_REG_WP_OFFSET)
#define     MV_MX25L_STATUS_BP_1_OF_8           (0x05 << MV_SFLASH_STATUS_REG_WP_OFFSET)
#define     MV_MX25L_STATUS_BP_1_OF_4           (0x06 << MV_SFLASH_STATUS_REG_WP_OFFSET)
#define     MV_MX25L_STATUS_BP_1_OF_2           (0x07 << MV_SFLASH_STATUS_REG_WP_OFFSET)
#define     MV_MX25L_STATUS_BP_ALL              (0x0F << MV_SFLASH_STATUS_REG_WP_OFFSET)

/************************************/
/*  SPANSION S25FL128P Device Specific  */
/************************************/

/* Manufacturer IDs and Device IDs for SFLASHs supported by the driver */
#define     MV_SPANSION_MANF_ID                     	0x01
#define     MV_S25FL128_DEVICE_ID              		0x2018
#define     MV_S25FL128_MAX_SPI_FREQ           		33000000    /* 33MHz */
#define     MV_S25FL128_MAX_FAST_SPI_FREQ        	104000000    /* 104MHz */
#define     MV_S25FL128_FAST_READ_DUMMY_BYTES    	1

/* Sector Sizes and population per device model*/
#define     MV_S25FL128_SECTOR_SIZE            			0x40000 /* 256K */
#define     MV_S25FL128_SECTOR_NUMBER          			64
#define	    MV_S25FL_PAGE_SIZE			        	0x100   /* 256 byte */

#define		MV_S25FL_WREN_CMND_OPCD			    0x06	/* Write Enable */
#define		MV_S25FL_WRDI_CMND_OPCD			    0x04	/* Write Disable */
#define		MV_S25FL_RDID_CMND_OPCD			    0x9F	/* Read ID */
#define		MV_S25FL_RDSR_CMND_OPCD			    0x05	/* Read Status Register */
#define		MV_S25FL_WRSR_CMND_OPCD			    0x01	/* Write Status Register */
#define		MV_S25FL_READ_CMND_OPCD			    0x03	/* Sequential Read */
#define		MV_S25FL_FAST_RD_CMND_OPCD		    0x0B	/* Fast Read */
#define		MV_S25FL_PP_CMND_OPCD			    0x02	/* Page Program */
#define		MV_S25FL_SE_CMND_OPCD			    0xD8	/* Sector Erase */
#define		MV_S25FL_BE_CMND_OPCD			    0xC7	/* Bulk Erase */
#define     	MV_S25FL_DP_CMND_OPCD               	    0xB9    	/* Deep Power Down */
#define		MV_S25FL_RES_CMND_OPCD			    0xAB	/* Read Electronic Signature */

/* Status Register Write Protect Bit Masks - 4bits */
#define		MV_S25FL_STATUS_REG_WP_MASK	        (0x0F << MV_SFLASH_STATUS_REG_WP_OFFSET)
#define     	MV_S25FL_STATUS_BP_NONE             	(0x00 << MV_SFLASH_STATUS_REG_WP_OFFSET)
#define     	MV_S25FL_STATUS_BP_1_OF_128         	(0x01 << MV_SFLASH_STATUS_REG_WP_OFFSET)
#define     	MV_S25FL_STATUS_BP_1_OF_64          	(0x02 << MV_SFLASH_STATUS_REG_WP_OFFSET)
#define     	MV_S25FL_STATUS_BP_1_OF_32          	(0x03 << MV_SFLASH_STATUS_REG_WP_OFFSET)
#define     	MV_S25FL_STATUS_BP_1_OF_16          	(0x04 << MV_SFLASH_STATUS_REG_WP_OFFSET)
#define     	MV_S25FL_STATUS_BP_1_OF_8           	(0x05 << MV_SFLASH_STATUS_REG_WP_OFFSET)
#define     	MV_S25FL_STATUS_BP_1_OF_4           	(0x06 << MV_SFLASH_STATUS_REG_WP_OFFSET)
#define     	MV_S25FL_STATUS_BP_1_OF_2           	(0x07 << MV_SFLASH_STATUS_REG_WP_OFFSET)
#define     	MV_S25FL_STATUS_BP_ALL              	(0x0F << MV_SFLASH_STATUS_REG_WP_OFFSET)

#endif /* __INCmvSFlashSpecH */


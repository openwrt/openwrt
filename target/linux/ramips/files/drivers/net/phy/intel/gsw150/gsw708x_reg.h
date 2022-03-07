/*
 This file is provided under a dual BSD/GPLv2 license.  When using or
 redistributing this file, you may do so under either license.

 GPL LICENSE SUMMARY

 Copyright(c) 2016 - 2017 Intel Corporation.

 This program is free software; you can redistribute it and/or modify
 it under the terms of version 2 of the GNU General Public License as
 published by the Free Software Foundation.

 This program is distributed in the hope that it will be useful, but
 WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 General Public License for more details.

 Contact Information:
  Intel Corporation
  2200 Mission College Blvd.
  Santa Clara, CA  97052

 BSD LICENSE

 Copyright(c) 2016 - 2017 Intel Corporation.

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions
 are met:

  * Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in
    the documentation and/or other materials provided with the
    distribution.
  * Neither the name of Intel Corporation nor the names of its
    contributors may be used to endorse or promote products derived
    from this software without specific prior written permission.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*/

/* ----------------------------------------------- */
/* xMII Register offset */
/* ----------------------------------------------- */
/* Register: 'xMII Port 5 ConfigurationRegister' */
/* Bit: 'RES' */
/* Description: 'Hardware Reset' */
#define MII_CFG_5_RST_OFFSET	0xF100
#define MII_CFG_5_RST_SHIFT 15
#define MII_CFG_5_RST_SIZE	 1
/* Bit: 'EN' */
/* Description: 'xMII Interface Enable' */
#define MII_CFG_5_EN_OFFSET 0xF100
#define MII_CFG_5_EN_SHIFT  14
#define MII_CFG_5_EN_SIZE   1
/* Bit: 'ISOL' */
/* Description: 'ISOLATE xMII Interface' */
#define MII_CFG_5_ISOL_OFFSET 0xF100
#define MII_CFG_5_ISOL_SHIFT  13
#define MII_CFG_5_ISOL_SIZE   1
/* Bit: 'LDCLKDIS' */
/* Description: 'Link Down Clock Disable' */
#define MII_CFG_5_CLKDIS_OFFSET 0xF100
#define MII_CFG_5_CLKDIS_SHIFT  12
#define MII_CFG_5_CLKDIS_SIZE   1
/* Bit: 'CRS' */
/* Description: 'CRS Sensitivity Configuration' */
#define MII_CFG_5_CRS_OFFSET 0xF100
#define MII_CFG_5_CRS_SHIFT  9
#define MII_CFG_5_CRS_SIZE   2
/* Bit: 'RGMII_IBS' */
/* Description: 'RGMII In Band Status' */
#define MII_CFG_5_RGMII_IBS_OFFSET 0xF100
#define MII_CFG_5_RGMII_IBS_SHIFT  8
#define MII_CFG_5_RGMII_IBS_SIZE   1
/* Bit: 'MIIRATE' */
/* Description: 'xMII Port Interface Clock Rate' */
#define MII_CFG_5_MIIRATE_OFFSET 0xF100
#define MII_CFG_5_MIIRATE_SHIFT  4
#define MII_CFG_5_MIIRATE_SIZE   3
/* Bit: 'MIIMODE' */
/* Description: 'xMII Interface Mode' */
#define MII_CFG_5_MIIMODE_OFFSET 0xF100
#define MII_CFG_5_MIIMODE_SHIFT  0
#define MII_CFG_5_MIIMODE_SIZE   4
/* ----------------------------------------------- */
/* Register: 'xMII Port 6 ConfigurationRegister' */
/* Bit: 'RES' */
/* Description: 'Hardware Reset' */
#define MII_CFG_6_RST_OFFSET	0xF10A
#define MII_CFG_6_RSTS_SHIFT 15
#define MII_CFG_6_RST_SIZE	 1
/* Bit: 'EN' */
/* Description: 'xMII Interface Enable' */
#define MII_CFG_6_EN_OFFSET 0xF10A
#define MII_CFG_6_EN_SHIFT  14
#define MII_CFG_6_EN_SIZE   1
/* Bit: 'ISOL' */
/* Description: 'ISOLATE xMII Interface' */
#define MII_CFG_6_ISOL_OFFSET 0xF10A
#define MII_CFG_6_ISOL_SHIFT  13
#define MII_CFG_6_ISOL_SIZE   1
/* Bit: 'LDCLKDIS' */
/* Description: 'Link Down Clock Disable' */
#define MII_CFG_6_CLKDIS_OFFSET 0xF10A
#define MII_CFG_6_CLKDIS_SHIFT  12
#define MII_CFG_6_CLKDIS_SIZE   1
/* Bit: 'CRS' */
/* Description: 'CRS Sensitivity Configuration' */
#define MII_CFG_6_CRS_OFFSET 0xF10A
#define MII_CFG_6_CRS_SHIFT  9
#define MII_CFG_6_CRS_SIZE   2
/* Bit: 'RGMII_IBS' */
/* Description: 'RGMII In Band Status' */
#define MII_CFG_6_RGMII_IBS_OFFSET 0xF10A
#define MII_CFG_6_RGMII_IBS_SHIFT  8
#define MII_CFG_6_RGMII_IBS_SIZE   1
/* Bit: 'MIIRATE' */
/* Description: 'xMII Port Interface Clock Rate' */
#define MII_CFG_6_MIIRATE_OFFSET 0xF10A
#define MII_CFG_6_MIIRATE_SHIFT  4
#define MII_CFG_6_MIIRATE_SIZE   3
/* Bit: 'MIIMODE' */
/* Description: 'xMII Interface Mode' */
#define MII_CFG_6_MIIMODE_OFFSET 0xF10A
#define MII_CFG_6_MIIMODE_SHIFT  0
#define MII_CFG_6_MIIMODE_SIZE   4
/* ----------------------------------------------- */
/* Register: 'RGMII 5 Clock Delay Configuration Register' */
/* Bit: 'DELMD' */
/* Description: 'PCDU Delay Setting Mode' */
#define PCDU_5_RXLOCK_OFFSET 0xF101
#define PCDU_5_RXLOCK_SHIFT  10
#define PCDU_5_RXLOCK_SIZE   1
/* Bit: 'RXDLY' */
/* Description: 'Configure Receive Clock Delay' */
#define PCDU_5_RXDLY_OFFSET 0xF101
#define PCDU_5_RXDLY_SHIFT  7
#define PCDU_5_RXDLY_SIZE   3
/* Bit: 'TXDLY' */
/* Description: 'Configure Transmit Clock Delay' */
#define PCDU_5_TXDLY_OFFSET 0xF101
#define PCDU_5_TXDLY_SHIFT 0
#define PCDU_5_TXDLY_SIZE	 3
/* ----------------------------------------------- */
/* Register: 'RGMII 6 Clock Delay Configuration Register' */
/* Bit: 'DELMD' */
/* Description: 'PCDU Delay Setting Mode' */
#define PCDU_6_RXLOCK_OFFSET 0xF10B
#define PCDU_6_RXLOCK_SHIFT  10
#define PCDU_6_RXLOCK_SIZE   1
/* Bit: 'RXDLY' */
/* Description: 'Configure Receive Clock Delay' */
#define PCDU_6_RXDLY_OFFSET 0xF10B
#define PCDU_6_RXDLY_SHIFT  7
#define PCDU_6_RXDLY_SIZE   3
/* Bit: 'TXDLY' */
/* Description: 'Configure Transmit Clock Delay' */
#define PCDU_6_TXDLY_OFFSET 0xF10B
#define PCDU_6_TXDLY_SHIFT 0
#define PCDU_6_TXDLY_SIZE	 3
/* ----------------------------------------------- */
/* Register: 'xMII5 Interface Receive Transmit Buffer Control Register' */
/* Bit: 'RBUF_UFL' */
/* Description: 'Transmit Buffer Underflow Indicator' */
#define RTXB_CTL_5_TBUF_UFL_OFFSET 0xF120
#define RTXB_CTL_5_TBUF_UFL_SHIFT  15
#define RTXB_CTL_5_TBUF_UFL_SIZE   1
/* Bit: 'RBUF_OFL' */
/* Description: 'Transmit Buffer Overflow Indicator' */
#define RTXB_CTL_5_TBUF_OFL_OFFSET 0xF120
#define RTXB_CTL_5_TBUF_OFL_SHIFT  14
#define RTXB_CTL_5_TBUF_OFL_SIZE   1
/* Bit: 'RBUF_UFL' */
/* Description: 'Receive Buffer Underflow Indicator' */
#define RTXB_CTL_5_RBUF_UFL_OFFSET 0xF120
#define RTXB_CTL_5_RBUF_UFL_SHIFT  13
#define RTXB_CTL_5_RBUF_UFL_SIZE   1
/* Bit: 'RBUF_OFL' */
/* Description: 'Receive Buffer Overflow Indicator' */
#define RTXB_CTL_5_RBUF_OFL_OFFSET 0xF120
#define RTXB_CTL_5_RBUF_OFL_SHIFT  12
#define RTXB_CTL_5_RBUF_OFL_SIZE   1
/* Bit: 'TBUF_DLY_WP' */
/* Description: 'TX Buffer Delay Write Pointer' */
#define RTXB_CTL_5_TBUF_DLY_WP_OFFSET 0xF120
#define RTXB_CTL_5_TBUF_DLY_WP_SHIFT  3
#define RTXB_CTL_5_TBUF_DLY_WP_SIZE   3
/* Bit: 'RBUF_DLY_WP' */
/* Description: 'RX Buffer Delay Write Pointer' */
#define RTXB_CTL_5_RBUF_DLY_WP_OFFSET 0xF120
#define RTXB_CTL_5_RBUF_DLY_WP_SHIFT  0
#define RTXB_CTL_5_RBUF_DLY_WP_SIZE   3
/* ----------------------------------------------- */
/* Register: 'xMII6 Interface Receive Transmit Buffer Control Register' */
/* Bit: 'RBUF_UFL' */
/* Description: 'Transmit Buffer Underflow Indicator' */
#define RTXB_CTL_6_TBUF_UFL_OFFSET 0xF125
#define RTXB_CTL_6_TBUF_UFL_SHIFT  15
#define RTXB_CTL_6_TBUF_UFL_SIZE   1
/* Bit: 'RBUF_OFL' */
/* Description: 'Transmit Buffer Overflow Indicator' */
#define RTXB_CTL_6_TBUF_OFL_OFFSET 0xF125
#define RTXB_CTL_6_TBUF_OFL_SHIFT  14
#define RTXB_CTL_6_TBUF_OFL_SIZE   1
/* Bit: 'RBUF_UFL' */
/* Description: 'Receive Buffer Underflow Indicator' */
#define RTXB_CTL_6_RBUF_UFL_OFFSET 0xF125
#define RTXB_CTL_6_RBUF_UFL_SHIFT  13
#define RTXB_CTL_6_RBUF_UFL_SIZE   1
/* Bit: 'RBUF_OFL' */
/* Description: 'Receive Buffer Overflow Indicator' */
#define RTXB_CTL_6_RBUF_OFL_OFFSET 0xF125
#define RTXB_CTL_6_RBUF_OFL_SHIFT  12
#define RTXB_CTL_6_RBUF_OFL_SIZE   1
/* Bit: 'TBUF_DLY_WP' */
/* Description: 'TX Buffer Delay Write Pointer' */
#define RTXB_CTL_6_TBUF_DLY_WP_OFFSET 0xF125
#define RTXB_CTL_6_TBUF_DLY_WP_SHIFT  3
#define RTXB_CTL_6_TBUF_DLY_WP_SIZE   3
/* Bit: 'RBUF_DLY_WP' */
/* Description: 'RX Buffer Delay Write Pointer' */
#define RTXB_CTL_6_RBUF_DLY_WP_OFFSET 0xF125
#define RTXB_CTL_6_RBUF_DLY_WP_SHIFT  0
#define RTXB_CTL_6_RBUF_DLY_WP_SIZE   3
/* ----------------------------------------------- */
/* Register: 'Pin and Port Multiplexing Configuration Register' */
/* Bit: 'PIE' */
/* Description: 'Packet Insertion and Extraction Mode' */
#define MII_MUX_CFG_PIE_OFFSET 0xF130
#define MII_MUX_CFG_PIE_SHIFT 9
#define MII_MUX_CFG_PIE_SIZE  1
/* Bit: 'CLKMD' */
/* Description: 'Clock Mode for GMII with Internal GPHY' */
#define MII_MUX_CFG_CLKMD_OFFSET 0xF130
#define MII_MUX_CFG_CLKMD_SHIFT 8
#define MII_MUX_CFG_CLKMD_SIZE  1
/* Bit: 'GPHY0_ISO' */
/* Description: 'Packet Insertion and Extraction Mode' */
#define MII_MUX_CFG_GPHY0_ISO_OFFSET 0xF130
#define MII_MUX_CFG_GPHY0_ISO_SHIFT 0
#define MII_MUX_CFG_GPHY0_ISO_SIZE  1
/* ----------------------------------------------- */
/* Register: 'Packet Insertion Register' */
/* Bit: 'INSCMD' */
/* Description: 'Packet Insertion Command' */
#define PKT_INS_INSCMD_OFFSET 0xF140
#define PKT_INS_INSCMD_SHIFT 15
#define PKT_INS_INSCMD_SIZE  1
/* Bit: 'RXVD' */
/* Description: 'RX Valid' */
#define PKT_INS_RXVD_OFFSET 0xF140
#define PKT_INS_RXVD_SHIFT 8
#define PKT_INS_RXVD_SIZE  1
/* Bit: 'RXD' */
/* Description: 'RX Datae' */
#define PKT_INS_RXD_OFFSET 0xF140
#define PKT_INS_RXD_SHIFT 0
#define PKT_INS_RXD_SIZE  8
/* ----------------------------------------------- */
/* Register: 'Packet Extraction Read Register' */
/* Bit: 'AVAIL' */
/* Description: 'Packet Available' */
#define PKT_EXT_READ_AVAIL_OFFSET 0xF141
#define PKT_EXT_READ_AVAIL_SHIFT 15
#define PKT_EXT_READ_AVAIL_SIZE  1
/* Bit: 'TXEN' */
/* Description: 'TX Data Valid' */
#define PKT_EXT_READ_TXEN_OFFSET 0xF141
#define PKT_EXT_READ_TXEN_SHIFT 8
#define PKT_EXT_READ_TXEN_SIZE  1
/* Bit: 'TXD' */
/* Description: 'TX Data' */
#define PKT_EXT_READ_TXD_OFFSET 0xF141
#define PKT_EXT_READ_TXD_SHIFT 0
#define PKT_EXT_READ_TXD_SIZE  8
/* ----------------------------------------------- */
/* Register: 'Packet Extraction Command Register' */
/* Bit: 'FLUSH' */
/* Description: 'Packet Extraction Flush Command' */
#define PKT_EXT_CMD_FLUSH_OFFSET 0xF142
#define PKT_EXT_CMD_FLUSH_SHIFT 0
#define PKT_EXT_CMD_FLUSH_SIZE  1
/* ----------------------------------------------- */
/* Register: 'PCDU5 TX K Value' */
/* Bit: 'KVAL' */
/* Description: 'K Value for TX Delay Path' */
#define PCDU5_TX_KVAL_OFFSET 0xF160
#define PCDU5_TX_KVAL__SHIFT 0
#define PCDU5_TX_KVAL_SIZE  16
/* ----------------------------------------------- */
/* Register: 'PCDU6 TX K Value' */
/* Bit: 'KVAL' */
/* Description: 'K Value for TX Delay Path' */
#define PCDU6_TX_KVAL_OFFSET 0xF170
#define PCDU6_TX_KVAL_SHIFT 0
#define PCDU6_TX_KVAL_SIZE  16
/* ----------------------------------------------- */
/* Register: 'PCDU5 TX M Required' */
/* Bit: 'MREQ' */
/* Description: 'M Required for TX Delay Path' */
#define PCDU5_TX_MREQ_OFFSET 0xF161
#define PCDU5_TX_MREQ_SHIFT 0
#define PCDU5_TX_MREQ_SIZE  16
/* ----------------------------------------------- */
/* Register: 'PCDU6 TX M Required' */
/* Bit: 'MREQ' */
/* Description: 'M Required for TX Delay Path' */
#define PCDU6_TX_MREQ_OFFSET 0xF171
#define PCDU6_TX_MREQ_SHIFT 0
#define PCDU6_TX_MREQ_SIZE  16
/* ----------------------------------------------- */
/* Register: 'PCDU5 TX M Blank' */
/* Bit: 'MBLK' */
/* Description: 'M Blank for TX Delay Path' */
#define PCDU5_TX_MBLK_OFFSET 0xF162
#define PCDU5_TX_MBLK_SHIFT 0
#define PCDU5_TX_MBLK_SIZE  16
/* ----------------------------------------------- */
/* Register: 'PCDU6 TX M Required' */
/* Bit: 'MBLK' */
/* Description: 'M Blank for TX Delay Path' */
#define PCDU6_TX_MBLK_OFFSET 0xF172
#define PCDU6_TX_MBLK_SHIFT 0
#define PCDU6_TX_MBLK_SIZE  16
/* ----------------------------------------------- */
/* Register: 'PCDU5 TX Delay Length' */
/* Bit: 'DEL_LEN' */
/* Description: 'Delay Length for TX Delay Path' */
#define PCDU5_TX_DELLEN_OFFSET 0xF163
#define PCDU5_TX_DELLEN_SHIFT 0
#define PCDU5_TX_DELLEN_SIZE  6
/* ----------------------------------------------- */
/* Register: 'PCDU6 TX Delay Length' */
/* Bit: 'DEL_LEN' */
/* Description: 'Delay Length for TX Delay Path' */
#define PCDU6_TX_DELLEN_OFFSET 0xF173
#define PCDU6_TX_DELLEN_SHIFT 0
#define PCDU6_TX_DELLEN_SIZE  6
/* ----------------------------------------------- */
/* Register: 'PCDU5 RX K Value' */
/* Bit: 'KVAL' */
/* Description: 'K Value for RX Delay Path' */
#define PCDU5_RX_KVAL_OFFSET 0xF168
#define PCDU5_RX_KVAL_SHIFT 0
#define PCDU5_RX_KVAL_SIZE  16
/* ----------------------------------------------- */
/* Register: 'PCDU6 RX K Value' */
/* Bit: 'KVAL' */
/* Description: 'K Value for RX Delay Path' */
#define PCDU6_RX_KVAL_OFFSET 0xF178
#define PCDU6_RX_KVAL_SHIFT 0
#define PCDU6_RX_KVAL_SIZE  16
/* ----------------------------------------------- */
/* Register: 'PCDU5 RX M Required' */
/* Bit: 'MREQ' */
/* Description: 'M Required for RX Delay Path' */
#define PCDU5_RX_MREQ_OFFSET 0xF169
#define PCDU5_RX_MREQ_SHIFT 0
#define PCDU5_RX_MREQ_SIZE  16
/* ----------------------------------------------- */
/* Register: 'PCDU6 RX M Required' */
/* Bit: 'MREQ' */
/* Description: 'M Required for RX Delay Path' */
#define PCDU6_RX_MREQ_OFFSET 0xF179
#define PCDU6_RX_MREQ_SHIFT 0
#define PCDU6_RX_MREQ_SIZE  16
/* ----------------------------------------------- */
/* Register: 'PCDU5 RX M Blank' */
/* Bit: 'MBLK' */
/* Description: 'M Blank for RX Delay Path' */
#define PCDU5_RX_MBLK_OFFSET 0xF16A
#define PCDU5_RX_MBLK_SHIFT 0
#define PCDU5_RX_MBLK_SIZE  16
/* ----------------------------------------------- */
/* Register: 'PCDU6 RX M Required' */
/* Bit: 'MBLK' */
/* Description: 'M Blank for RX Delay Path' */
#define PCDU6_RX_MBLK_OFFSET 0xF17A
#define PCDU6_RX_MBLK_SHIFT 0
#define PCDU6_RX_MBLK_SIZE  16
/* ----------------------------------------------- */
/* Register: 'PCDU5 RX Delay Length' */
/* Bit: 'DEL_LEN' */
/* Description: 'Delay Length for RX Delay Path' */
#define PCDU5_RX_DELLEN_OFFSET 0xF16B
#define PCDU5_RX_DELLEN_SHIFT 0
#define PCDU5_RX_DELLEN_SIZE  6
/* ----------------------------------------------- */
/* Register: 'PCDU6 RX Delay Length' */
/* Bit: 'DEL_LEN' */
/* Description: 'Delay Length for RX Delay Path' */
#define PCDU6_RX_DELLEN_OFFSET 0xF17B
#define PCDU6_RX_DELLEN_SHIFT 0
#define PCDU6_RX_DELLEN_SIZE  6
/* ----------------------------------------------- */
/* MMDIO register*/
/* ----------------------------------------------- */
/* Register: 'Global Control Register0' */
/* Bit: 'SE' */
/* Description: 'Global Switch Macro Enable' */
#define GSWIP_CFG_SE_OFFSET 0xF400
#define GSWIP_CFG_SE_SHIFT  15
#define GSWIP_CFG_SE_SIZE   1
/* Bit: 'P6' */
/* Description: 'Port 6 Disable Configuration' */
#define GSWIP_CFG_P6_OFFSET 0xF400
#define GSWIP_CFG_P6_SHIFT 14
#define GSWIP_CFG_P6_SIZE  1
/* Bit: 'P6' */
/* Description: 'Port 5 Disable Configuration' */
#define GSWIP_CFG_P5_OFFSET 0xF400
#define GSWIP_CFG_P5_SHIFT 13
#define GSWIP_CFG_P5_SIZE  1
/* Bit: 'P6' */
/* Description: 'Port 4 Disable Configuration' */
#define GSWIP_CFG_P4_OFFSET 0xF400
#define GSWIP_CFG_P4_SHIFT 11
#define GSWIP_CFG_P4_SIZE  1
/* Bit: 'P6' */
/* Description: 'Port 3 Disable Configuration' */
#define GSWIP_CFG_P3_OFFSET 0xF400
#define GSWIP_CFG_P3_SHIFT 11
#define GSWIP_CFG_P3_SIZE  1
/* Bit: 'P6' */
/* Description: 'Port 2 Disable Configuration' */
#define GSWIP_CFG_P2_OFFSET 0xF400
#define GSWIP_CFG_P2_SHIFT 10
#define GSWIP_CFG_P2_SIZE  1
/* Bit: 'P6' */
/* Description: 'Port 1 Disable Configuration' */
#define GSWIP_CFG_P1_OFFSET 0xF400
#define GSWIP_CFG_P1_SHIFT 9
#define GSWIP_CFG_P1_SIZE  1
/* Bit: 'P6' */
/* Description: 'Port 0 Disable Configuration' */
#define GSWIP_CFG_P0_OFFSET 0xF400
#define GSWIP_CFG_P0_SHIFT 8
#define GSWIP_CFG_P0_SIZE  1
/* Bit: 'HWRES' */
/* Description: 'Global Hardware Reset' */
#define GSWIP_CFG_HWRES_OFFSET	0xF400
#define GSWIP_CFG_HWRES_SHIFT		1
#define GSWIP_CFG_HWRES_SIZE		1
/* Bit: 'SWRES' */
/* Description: 'Global Software Reset' */
#define GSWIP_CFG_SWRES_OFFSET	0xF400
#define GSWIP_CFG_SWRES_SHIFT		0
#define GSWIP_CFG_SWRES_SIZE		1
/* ----------------------------------------------- */
/* Register: 'MDIO Control Register' */
/* Bit: 'MBUSY' */
/* Description: 'MDIO Busy' */
#define MMDIO_CTRL_MBUSY_OFFSET 0xF408
#define MMDIO_CTRL_MBUSY_SHIFT  12
#define MMDIO_CTRL_MBUSY_SIZE   1
/* Bit: 'OP' */
/* Description: 'Operation Code' */
#define MMDIO_CTRL_OP_OFFSET 0xF408
#define MMDIO_CTRL_OP_SHIFT  10
#define MMDIO_CTRL_OP_SIZE   2
/* Bit: 'PHYAD' */
/* Description: 'PHY Address' */
#define MMDIO_CTRL_PHYAD_OFFSET 0xF408
#define MMDIO_CTRL_PHYAD_SHIFT  5
#define MMDIO_CTRL_PHYAD_SIZE   5
/* Bit: 'REGAD' */
/* Description: 'Register Address' */
#define MMDIO_CTRL_REGAD_OFFSET 0xF408
#define MMDIO_CTRL_REGAD_SHIFT  0
#define MMDIO_CTRL_REGAD_SIZE   5
#define MMDIO_CTRL_REG_SIZE   16
/* ----------------------------------------------- */
/* Register: 'MDIO Read Data Register' */
/* Bit: 'RDATA' */
/* Description: 'Read Data' */
#define MMDIO_READ_RDATA_OFFSET 0xF409
#define MMDIO_READ_RDATA_SHIFT  0
#define MMDIO_READ_RDATA_SIZE   16
/* ----------------------------------------------- */
/* Register: 'MDIO Write Data Register' */
/* Bit: 'WDATA' */
/* Description: 'Write Data' */
#define MMDIO_WRITE_WDATA_OFFSET	0xF40A
#define MMDIO_WRITE_WDATA_SHIFT	0
#define MMDIO_WRITE_WDATA_SIZE	16
/* ----------------------------------------------- */
/* Register: 'MDC Clock ConfigurationRegister 0' */
/* Bit: 'PEN_6' */
/* Description: 'Polling State Machine Enable' */
#define MMDC_CFG_0_PEN_6_OFFSET 0xF40B
#define MMDC_CFG_0_PEN_6_SHIFT  6
#define MMDC_CFG_0_PEN_6_SIZE   1
/* Bit: 'PEN_5' */
/* Description: 'Polling State Machine Enable' */
#define MMDC_CFG_0_PEN_5_OFFSET 0xF40B
#define MMDC_CFG_0_PEN_5_SHIFT  5
#define MMDC_CFG_0_PEN_5_SIZE   1
/* Bit: 'PEN_4' */
/* Description: 'Polling State Machine Enable' */
#define MMDC_CFG_0_PEN_4_OFFSET 0x00B
#define MMDC_CFG_0_PEN_4_SHIFT  4
#define MMDC_CFG_0_PEN_4_SIZE   1
/* Bit: 'PEN_3' */
/* Description: 'Polling State Machine Enable' */
#define MMDC_CFG_0_PEN_3_OFFSET 0xF40B
#define MMDC_CFG_0_PEN_3_SHIFT  3
#define MMDC_CFG_0_PEN_3_SIZE   1
/* Bit: 'PEN_2' */
/* Description: 'Polling State Machine Enable' */
#define MMDC_CFG_0_PEN_2_OFFSET 0xF40B
#define MMDC_CFG_0_PEN_2_SHIFT  2
#define MMDC_CFG_0_PEN_2_SIZE   1
/* Bit: 'PEN_1' */
/* Description: 'Polling State Machine Enable' */
#define MMDC_CFG_0_PEN_1_OFFSET 0xF40B
#define MMDC_CFG_0_PEN_1_SHIFT  1
#define MMDC_CFG_0_PEN_1_SIZE   1
/* Bit: 'PEN_0' */
/* Description: 'Polling State Machine Enable' */
#define MMDC_CFG_0_PEN_0_OFFSET 0xF40B
#define MMDC_CFG_0_PEN_0_SHIFT  0
#define MMDC_CFG_0_PEN_0_SIZE   1
/* Bit: 'PEN_0~PEN_5' */
/* Description: 'Polling State Machine Enable' */
#define MMDC_CFG_0_PEN_ALL_OFFSET 0xF40B
#define MMDC_CFG_0_PEN_ALL_SHIFT  0
#define MMDC_CFG_0_PEN_ALL_SIZE   7
/* ----------------------------------------------- */
/* Register: 'MDC Clock ConfigurationRegister 1' */
/* Bit: 'RST' */
/* Description: 'MDIO Hardware Reset' */
#define MMDC_CFG_1_RST_OFFSET 0xF40C
#define MMDC_CFG_1_RST_SHIFT 15
#define MMDC_CFG_1_RST_SIZE  1
/* Bit: 'GAP' */
/* Description: 'Autopolling Gap' */
#define MMDC_CFG_1_GAP_OFFSET 0xF40C
#define MMDC_CFG_1_GAP_SHIFT 9
#define MMDC_CFG_1_GAP_SIZE  6
/* Bit: 'MCEN' */
/* Description: 'Management Clock Enable' */
#define MMDC_CFG_1_MCEN_OFFSET 0xF40C
#define MMDC_CFG_1_MCEN_SHIFT 8
#define MMDC_CFG_1_MCEN_SIZE  1
/* Bit: 'FREQ' */
/* Description: 'MDIO Interface Clock Rate' */
#define MMDC_CFG_1_FREQ_OFFSET 0xF40C
#define MMDC_CFG_1_FREQ_SHIFT 0
#define MMDC_CFG_1_FREQ_SIZE  8
/* ----------------------------------------------- */
/* Register: 'PHY Address RegisterPORT 2' */
/* Bit: 'FRE' */
/* Description: 'Link Status Control' */
#define PHY_ADDR_0_FRE_OFFSET 0xF415
#define PHY_ADDR_0_FRE_SHIFT 15
#define PHY_ADDR_0_FRE_SIZE  1
/* Bit: 'LNKST' */
/* Description: 'Link Status Control' */
#define PHY_ADDR_0_LNKST_OFFSET 0xF415
#define PHY_ADDR_0_LNKST_SHIFT 13
#define PHY_ADDR_0_LNKST_SIZE  2
/* Bit: 'SPEED' */
/* Description: 'Speed Control' */
#define PHY_ADDR_0_SPEED_OFFSET 0xF415
#define PHY_ADDR_0_SPEED_SHIFT 11
#define PHY_ADDR_0_SPEED_SIZE 2
/* Bit: 'FDUP' */
/* Description: 'Full Duplex Control' */
#define PHY_ADDR_0_FDUP_OFFSET 0xF415
#define PHY_ADDR_0_FDUP_SHIFT  9
#define PHY_ADDR_0_FDUP_SIZE   2
/* Bit: 'FCONTX' */
/* Description: 'Flow Control Mode TX' */
#define PHY_ADDR_0_FCONTX_OFFSET 0xF415
#define PHY_ADDR_0_FCONTX_SHIFT 7
#define PHY_ADDR_0_FCONTX_SIZE  2
/* Bit: 'FCONRX' */
/* Description: 'Flow Control Mode RX' */
#define PHY_ADDR_0_FCONRX_OFFSET 0xF415
#define PHY_ADDR_0_FCONRX_SHIFT  5
#define PHY_ADDR_0_FCONRX_SIZE   2
/* Bit: 'ADDR' */
/* Description: 'PHY Address' */
#define PHY_ADDR_0_ADDR_OFFSET 0xF415
#define PHY_ADDR_0_ADDR_SHIFT  0
#define PHY_ADDR_0_ADDR_SIZE  5
#define PHY_ADDR_0_REG_SIZE  16
/* ----------------------------------------------- */
/* Register: 'PHY Address RegisterPORT 1' */
/* Bit: 'FRE' */
/* Description: 'Link Status Control' */
#define PHY_ADDR_1_FRE_OFFSET 0xF414
#define PHY_ADDR_1_FRE_SHIFT 15
#define PHY_ADDR_1_FRE_SIZE  1
/* Bit: 'LNKST' */
/* Description: 'Link Status Control' */
#define PHY_ADDR_1_LNKST_OFFSET 0xF414
#define PHY_ADDR_1_LNKST_SHIFT 13
#define PHY_ADDR_1_LNKST_SIZE  2
/* Bit: 'SPEED' */
/* Description: 'Speed Control' */
#define PHY_ADDR_1_SPEED_OFFSET 0xF414
#define PHY_ADDR_1_SPEED_SHIFT 11
#define PHY_ADDR_1_SPEED_SIZE 2
/* Bit: 'FDUP' */
/* Description: 'Full Duplex Control' */
#define PHY_ADDR_1_FDUP_OFFSET 0xF414
#define PHY_ADDR_1_FDUP_SHIFT  9
#define PHY_ADDR_1_FDUP_SIZE   2
/* Bit: 'FCONTX' */
/* Description: 'Flow Control Mode TX' */
#define PHY_ADDR_1_FCONTX_OFFSET 0xF414
#define PHY_ADDR_1_FCONTX_SHIFT 7
#define PHY_ADDR_1_FCONTX_SIZE  2
/* Bit: 'FCONRX' */
/* Description: 'Flow Control Mode RX' */
#define PHY_ADDR_1_FCONRX_OFFSET 0xF414
#define PHY_ADDR_1_FCONRX_SHIFT  5
#define PHY_ADDR_1_FCONRX_SIZE   2
/* Bit: 'ADDR' */
/* Description: 'PHY Address' */
#define PHY_ADDR_1_ADDR_OFFSET 0xF414
#define PHY_ADDR_1_ADDR_SHIFT  0
#define PHY_ADDR_1_ADDR_SIZE  5
/* ----------------------------------------------- */
/* Register: 'PHY Address RegisterPORT 2' */
/* Bit: 'FRE' */
/* Description: 'Link Status Control' */
#define PHY_ADDR_2_FRE_OFFSET 0xF413
#define PHY_ADDR_2_FRE_SHIFT 15
#define PHY_ADDR_2_FRE_SIZE  1
/* Bit: 'LNKST' */
/* Description: 'Link Status Control' */
#define PHY_ADDR_2_LNKST_OFFSET 0xF413
#define PHY_ADDR_2_LNKST_SHIFT 13
#define PHY_ADDR_2_LNKST_SIZE  2
/* Bit: 'SPEED' */
/* Description: 'Speed Control' */
#define PHY_ADDR_2_SPEED_OFFSET 0xF413
#define PHY_ADDR_2_SPEED_SHIFT 11
#define PHY_ADDR_2_SPEED_SIZE 2
/* Bit: 'FDUP' */
/* Description: 'Full Duplex Control' */
#define PHY_ADDR_2_FDUP_OFFSET 0xF413
#define PHY_ADDR_2_FDUP_SHIFT  9
#define PHY_ADDR_2_FDUP_SIZE   2
/* Bit: 'FCONTX' */
/* Description: 'Flow Control Mode TX' */
#define PHY_ADDR_2_FCONTX_OFFSET 0xF413
#define PHY_ADDR_2_FCONTX_SHIFT 7
#define PHY_ADDR_2_FCONTX_SIZE  2
/* Bit: 'FCONRX' */
/* Description: 'Flow Control Mode RX' */
#define PHY_ADDR_2_FCONRX_OFFSET 0xF413
#define PHY_ADDR_2_FCONRX_SHIFT  5
#define PHY_ADDR_2_FCONRX_SIZE   2
/* Bit: 'ADDR' */
/* Description: 'PHY Address' */
#define PHY_ADDR_2_ADDR_OFFSET 0xF413
#define PHY_ADDR_2_ADDR_SHIFT  0
#define PHY_ADDR_2_ADDR_SIZE  5
/* ----------------------------------------------- */
/* Register: 'PHY Address RegisterPORT 3' */
/* Bit: 'FRE' */
/* Description: 'Link Status Control' */
#define PHY_ADDR_3_FRE_OFFSET 0xF412
#define PHY_ADDR_3_FRE_SHIFT 15
#define PHY_ADDR_3_FRE_SIZE  1
/* Bit: 'LNKST' */
/* Description: 'Link Status Control' */
#define PHY_ADDR_3_LNKST_OFFSET 0xF412
#define PHY_ADDR_3_LNKST_SHIFT 13
#define PHY_ADDR_3_LNKST_SIZE  2
/* Bit: 'SPEED' */
/* Description: 'Speed Control' */
#define PHY_ADDR_3_SPEED_OFFSET 0xF412
#define PHY_ADDR_3_SPEED_SHIFT 11
#define PHY_ADDR_3_SPEED_SIZE 2
/* Bit: 'FDUP' */
/* Description: 'Full Duplex Control' */
#define PHY_ADDR_3_FDUP_OFFSET 0xF412
#define PHY_ADDR_3_FDUP_SHIFT  9
#define PHY_ADDR_3_FDUP_SIZE   2
/* Bit: 'FCONTX' */
/* Description: 'Flow Control Mode TX' */
#define PHY_ADDR_3_FCONTX_OFFSET 0xF412
#define PHY_ADDR_3_FCONTX_SHIFT 7
#define PHY_ADDR_3_FCONTX_SIZE  2
/* Bit: 'FCONRX' */
/* Description: 'Flow Control Mode RX' */
#define PHY_ADDR_3_FCONRX_OFFSET 0xF412
#define PHY_ADDR_3_FCONRX_SHIFT  5
#define PHY_ADDR_3_FCONRX_SIZE   2
/* Bit: 'ADDR' */
/* Description: 'PHY Address' */
#define PHY_ADDR_3_ADDR_OFFSET 0xF412
#define PHY_ADDR_3_ADDR_SHIFT  0
#define PHY_ADDR_3_ADDR_SIZE  5
/* ----------------------------------------------- */
/* Register: 'PHY Address RegisterPORT 4' */
/* Bit: 'FRE' */
/* Description: 'Link Status Control' */
#define PHY_ADDR_4_FRE_OFFSET 0xF411
#define PHY_ADDR_4_FRE_SHIFT 15
#define PHY_ADDR_4_FRE_SIZE  1
/* Bit: 'LNKST' */
/* Description: 'Link Status Control' */
#define PHY_ADDR_4_LNKST_OFFSET 0xF411
#define PHY_ADDR_4_LNKST_SHIFT 13
#define PHY_ADDR_4_LNKST_SIZE  2
/* Bit: 'SPEED' */
/* Description: 'Speed Control' */
#define PHY_ADDR_4_SPEED_OFFSET 0xF411
#define PHY_ADDR_4_SPEED_SHIFT 11
#define PHY_ADDR_4_SPEED_SIZE 2
/* Bit: 'FDUP' */
/* Description: 'Full Duplex Control' */
#define PHY_ADDR_4_FDUP_OFFSET 0xF411
#define PHY_ADDR_4_FDUP_SHIFT  9
#define PHY_ADDR_4_FDUP_SIZE   2
/* Bit: 'FCONTX' */
/* Description: 'Flow Control Mode TX' */
#define PHY_ADDR_4_FCONTX_OFFSET 0xF411
#define PHY_ADDR_4_FCONTX_SHIFT 7
#define PHY_ADDR_4_FCONTX_SIZE  2
/* Bit: 'FCONRX' */
/* Description: 'Flow Control Mode RX' */
#define PHY_ADDR_4_FCONRX_OFFSET 0xF411
#define PHY_ADDR_4_FCONRX_SHIFT  5
#define PHY_ADDR_4_FCONRX_SIZE   2
/* Bit: 'ADDR' */
/* Description: 'PHY Address' */
#define PHY_ADDR_4_ADDR_OFFSET 0xF411
#define PHY_ADDR_4_ADDR_SHIFT  0
#define PHY_ADDR_4_ADDR_SIZE  5
/* ----------------------------------------------- */
/* Register: 'PHY Address RegisterPORT 5' */
/* Bit: 'FRE' */
/* Description: 'Link Status Control' */
#define PHY_ADDR_5_FRE_OFFSET 0xF410
#define PHY_ADDR_5_FRE_SHIFT 15
#define PHY_ADDR_5_FRE_SIZE  1
/* Bit: 'LNKST' */
/* Description: 'Link Status Control' */
#define PHY_ADDR_5_LNKST_OFFSET 0xF410
#define PHY_ADDR_5_LNKST_SHIFT 13
#define PHY_ADDR_5_LNKST_SIZE  2
/* Bit: 'SPEED' */
/* Description: 'Speed Control' */
#define PHY_ADDR_5_SPEED_OFFSET 0xF410
#define PHY_ADDR_5_SPEED_SHIFT 11
#define PHY_ADDR_5_SPEED_SIZE 2
/* Bit: 'FDUP' */
/* Description: 'Full Duplex Control' */
#define PHY_ADDR_5_FDUP_OFFSET 0xF410
#define PHY_ADDR_5_FDUP_SHIFT  9
#define PHY_ADDR_5_FDUP_SIZE   2
/* Bit: 'FCONTX' */
/* Description: 'Flow Control Mode TX' */
#define PHY_ADDR_5_FCONTX_OFFSET 0xF410
#define PHY_ADDR_5_FCONTX_SHIFT 7
#define PHY_ADDR_5_FCONTX_SIZE  2
/* Bit: 'FCONRX' */
/* Description: 'Flow Control Mode RX' */
#define PHY_ADDR_5_FCONRX_OFFSET 0xF410
#define PHY_ADDR_5_FCONRX_SHIFT  5
#define PHY_ADDR_5_FCONRX_SIZE   2
/* Bit: 'ADDR' */
/* Description: 'PHY Address' */
#define PHY_ADDR_5_ADDR_OFFSET 0xF410
#define PHY_ADDR_5_ADDR_SHIFT  0
#define PHY_ADDR_5_ADDR_SIZE  5
/* ----------------------------------------------- */
/* Register: 'PHY Address RegisterPORT 6' */
/* Bit: 'FRE' */
/* Description: 'Link Status Control' */
#define PHY_ADDR_6_FRE_OFFSET 0xF40F
#define PHY_ADDR_6_FRE_SHIFT 15
#define PHY_ADDR_6_FRE_SIZE  1
/* Bit: 'LNKST' */
/* Description: 'Link Status Control' */
#define PHY_ADDR_6_LNKST_OFFSET 0xF40F
#define PHY_ADDR_6_LNKST_SHIFT 13
#define PHY_ADDR_6_LNKST_SIZE  2
/* Bit: 'SPEED' */
/* Description: 'Speed Control' */
#define PHY_ADDR_6_SPEED_OFFSET 0xF40F
#define PHY_ADDR_6_SPEED_SHIFT 11
#define PHY_ADDR_6_SPEED_SIZE 2
/* Bit: 'FDUP' */
/* Description: 'Full Duplex Control' */
#define PHY_ADDR_6_FDUP_OFFSET 0xF40F
#define PHY_ADDR_6_FDUP_SHIFT  9
#define PHY_ADDR_6_FDUP_SIZE   2
/* Bit: 'FCONTX' */
/* Description: 'Flow Control Mode TX' */
#define PHY_ADDR_6_FCONTX_OFFSET 0xF40F
#define PHY_ADDR_6_FCONTX_SHIFT 7
#define PHY_ADDR_6_FCONTX_SIZE  2
/* Bit: 'FCONRX' */
/* Description: 'Flow Control Mode RX' */
#define PHY_ADDR_6_FCONRX_OFFSET 0xF40F
#define PHY_ADDR_6_FCONRX_SHIFT  5
#define PHY_ADDR_6_FCONRX_SIZE   2
/* Bit: 'ADDR' */
/* Description: 'PHY Address' */
#define PHY_ADDR_6_ADDR_OFFSET 0xF40F
#define PHY_ADDR_6_ADDR_SHIFT  0
#define PHY_ADDR_6_ADDR_SIZE  5
/* ----------------------------------------------- */
/* Register: 'PHY MDIO PollingStatus per PORT' */
/* Bit: 'CLK_STOP_CAPABLE' */
/* Description: 'PHY supports MAC turning of TX clk' */
#define MMDIO_STAT_0_CLK_STOP_CAPABLE_OFFSET 0xF416
#define MMDIO_STAT_0_CLK_STOP_CAPABLE_SHIFT 8
#define MMDIO_STAT_0_CLK_STOP_CAPABLE_SIZE  1
/* Bit: 'EEE_CAPABLE' */
/* Description: 'PHY and link partner support EEE for current speed' */
#define MMDIO_STAT_0_EEE_CAPABLE_OFFSET 0xF416
#define MMDIO_STAT_0_EEE_CAPABLE_SHIFT 7
#define MMDIO_STAT_0_EEE_CAPABLE_SIZE  1
/* Bit: 'PACT' */
/* Description: 'PHY Active Status' */
#define MMDIO_STAT_0_PACT_OFFSET 0xF416
#define MMDIO_STAT_0_PACT_SHIFT 6
#define MMDIO_STAT_0_PACT_SIZE  1
/* Bit: 'LSTAT' */
/* Description: 'Link Status' */
#define MMDIO_STAT_0_LSTAT_OFFSET 0xF416
#define MMDIO_STAT_0_LSTAT_SHIFT  5
#define MMDIO_STAT_0_LSTAT_SIZE   1
/* Bit: 'SPEED' */
/* Description: 'Speed Control' */
#define MMDIO_STAT_0_SPEED_OFFSET 0xF416
#define MMDIO_STAT_0_SPEED_SHIFT  3
#define MMDIO_STAT_0_SPEED_SIZE   2
/* Bit: 'FDUP' */
/* Description: 'Full Duplex Status' */
#define MMDIO_STAT_0_FDUP_OFFSET 0xF416
#define MMDIO_STAT_0_FDUP_SHIFT  2
#define MMDIO_STAT_0_FDUP_SIZE   1
/* Bit: 'RXPAUEN' */
/* Description: 'Receive Pause Enable Status' */
#define MMDIO_STAT_0_RXPAUEN_OFFSET 0xF416
#define MMDIO_STAT_0_RXPAUEN_SHIFT  1
#define MMDIO_STAT_0_RXPAUEN_SIZE   1
/* Bit: 'TXPAUEN' */
/* Description: 'Transmit Pause Enable Status' */
#define MMDIO_STAT_0_TXPAUEN_OFFSET 0xF416
#define MMDIO_STAT_0_TXPAUEN_SHIFT  0
#define MMDIO_STAT_0_TXPAUEN_SIZE   1
/* ----------------------------------------------- */
/* Register: 'PHY MDIO PollingStatus per PORT' */
/* Bit: 'CLK_STOP_CAPABLE' */
/* Description: 'PHY supports MAC turning of TX clk' */
#define MMDIO_STAT_1_CLK_STOP_CAPABLE_OFFSET 0xF417
#define MMDIO_STAT_1_CLK_STOP_CAPABLE_SHIFT 8
#define MMDIO_STAT_1_CLK_STOP_CAPABLE_SIZE  1
/* Bit: 'EEE_CAPABLE' */
/* Description: 'PHY and link partner support EEE for current speed' */
#define MMDIO_STAT_1_EEE_CAPABLE_OFFSET 0xF417
#define MMDIO_STAT_1_EEE_CAPABLE_SHIFT 7
#define MMDIO_STAT_1_EEE_CAPABLE_SIZE  1
/* Bit: 'PACT' */
/* Description: 'PHY Active Status' */
#define MMDIO_STAT_1_PACT_OFFSET 0xF417
#define MMDIO_STAT_1_PACT_SHIFT 6
#define MMDIO_STAT_1_PACT_SIZE  1
/* Bit: 'LSTAT' */
/* Description: 'Link Status' */
#define MMDIO_STAT_1_LSTAT_OFFSET 0xF417
#define MMDIO_STAT_1_LSTAT_SHIFT  5
#define MMDIO_STAT_1_LSTAT_SIZE   1
/* Bit: 'SPEED' */
/* Description: 'Speed Control' */
#define MMDIO_STAT_1_SPEED_OFFSET 0xF417
#define MMDIO_STAT_1_SPEED_SHIFT  3
#define MMDIO_STAT_1_SPEED_SIZE   2
/* Bit: 'FDUP' */
/* Description: 'Full Duplex Status' */
#define MMDIO_STAT_1_FDUP_OFFSET 0xF417
#define MMDIO_STAT_1_FDUP_SHIFT  2
#define MMDIO_STAT_1_FDUP_SIZE   1
/* Bit: 'RXPAUEN' */
/* Description: 'Receive Pause Enable Status' */
#define MMDIO_STAT_1_RXPAUEN_OFFSET 0xF417
#define MMDIO_STAT_1_RXPAUEN_SHIFT  1
#define MMDIO_STAT_1_RXPAUEN_SIZE   1
/* Bit: 'TXPAUEN' */
/* Description: 'Transmit Pause Enable Status' */
#define MMDIO_STAT_1_TXPAUEN_OFFSET 0xF417
#define MMDIO_STAT_1_TXPAUEN_SHIFT  0
#define MMDIO_STAT_1_TXPAUEN_SIZE   1
/* ----------------------------------------------- */
/* Register: 'PHY MDIO PollingStatus per PORT' */
/* Bit: 'CLK_STOP_CAPABLE' */
/* Description: 'PHY supports MAC turning of TX clk' */
#define MMDIO_STAT_2_CLK_STOP_CAPABLE_OFFSET 0xF418
#define MMDIO_STAT_2_CLK_STOP_CAPABLE_SHIFT 8
#define MMDIO_STAT_2_CLK_STOP_CAPABLE_SIZE  1
/* Bit: 'EEE_CAPABLE' */
/* Description: 'PHY and link partner support EEE for current speed' */
#define MMDIO_STAT_2_EEE_CAPABLE_OFFSET 0xF418
#define MMDIO_STAT_2_EEE_CAPABLE_SHIFT 7
#define MMDIO_STAT_2_EEE_CAPABLE_SIZE  1
/* Bit: 'PACT' */
/* Description: 'PHY Active Status' */
#define MMDIO_STAT_2_PACT_OFFSET 0xF418
#define MMDIO_STAT_2_PACT_SHIFT 6
#define MMDIO_STAT_2_PACT_SIZE  1
/* Bit: 'LSTAT' */
/* Description: 'Link Status' */
#define MMDIO_STAT_2_LSTAT_OFFSET 0xF418
#define MMDIO_STAT_2_LSTAT_SHIFT  5
#define MMDIO_STAT_2_LSTAT_SIZE   1
/* Bit: 'SPEED' */
/* Description: 'Speed Control' */
#define MMDIO_STAT_2_SPEED_OFFSET 0xF418
#define MMDIO_STAT_2_SPEED_SHIFT  3
#define MMDIO_STAT_2_SPEED_SIZE   2
/* Bit: 'FDUP' */
/* Description: 'Full Duplex Status' */
#define MMDIO_STAT_2_FDUP_OFFSET 0xF418
#define MMDIO_STAT_2_FDUP_SHIFT  2
#define MMDIO_STAT_2_FDUP_SIZE   1
/* Bit: 'RXPAUEN' */
/* Description: 'Receive Pause Enable Status' */
#define MMDIO_STAT_2_RXPAUEN_OFFSET 0xF418
#define MMDIO_STAT_2_RXPAUEN_SHIFT  1
#define MMDIO_STAT_2_RXPAUEN_SIZE   1
/* Bit: 'TXPAUEN' */
/* Description: 'Transmit Pause Enable Status' */
#define MMDIO_STAT_2_TXPAUEN_OFFSET 0xF418
#define MMDIO_STAT_2_TXPAUEN_SHIFT  0
#define MMDIO_STAT_2_TXPAUEN_SIZE   1
/* ----------------------------------------------- */
/* Register: 'PHY MDIO PollingStatus per PORT' */
/* Bit: 'CLK_STOP_CAPABLE' */
/* Description: 'PHY supports MAC turning of TX clk' */
#define MMDIO_STAT_3_CLK_STOP_CAPABLE_OFFSET 0xF419
#define MMDIO_STAT_3_CLK_STOP_CAPABLE_SHIFT 8
#define MMDIO_STAT_3_CLK_STOP_CAPABLE_SIZE  1
/* Bit: 'EEE_CAPABLE' */
/* Description: 'PHY and link partner support EEE for current speed' */
#define MMDIO_STAT_3_EEE_CAPABLE_OFFSET 0xF419
#define MMDIO_STAT_3_EEE_CAPABLE_SHIFT 7
#define MMDIO_STAT_3_EEE_CAPABLE_SIZE  1
/* Bit: 'PACT' */
/* Description: 'PHY Active Status' */
#define MMDIO_STAT_3_PACT_OFFSET 0xF419
#define MMDIO_STAT_3_PACT_SHIFT 6
#define MMDIO_STAT_3_PACT_SIZE  1
/* Bit: 'LSTAT' */
/* Description: 'Link Status' */
#define MMDIO_STAT_3_LSTAT_OFFSET 0xF419
#define MMDIO_STAT_3_LSTAT_SHIFT  5
#define MMDIO_STAT_3_LSTAT_SIZE   1
/* Bit: 'SPEED' */
/* Description: 'Speed Control' */
#define MMDIO_STAT_3_SPEED_OFFSET 0xF419
#define MMDIO_STAT_3_SPEED_SHIFT  3
#define MMDIO_STAT_3_SPEED_SIZE   2
/* Bit: 'FDUP' */
/* Description: 'Full Duplex Status' */
#define MMDIO_STAT_3_FDUP_OFFSET 0xF419
#define MMDIO_STAT_3_FDUP_SHIFT  2
#define MMDIO_STAT_3_FDUP_SIZE   1
/* Bit: 'RXPAUEN' */
/* Description: 'Receive Pause Enable Status' */
#define MMDIO_STAT_3_RXPAUEN_OFFSET 0xF419
#define MMDIO_STAT_3_RXPAUEN_SHIFT  1
#define MMDIO_STAT_3_RXPAUEN_SIZE   1
/* Bit: 'TXPAUEN' */
/* Description: 'Transmit Pause Enable Status' */
#define MMDIO_STAT_3_TXPAUEN_OFFSET 0xF419
#define MMDIO_STAT_3_TXPAUEN_SHIFT  0
#define MMDIO_STAT_3_TXPAUEN_SIZE   1
/* ----------------------------------------------- */
/* Register: 'PHY MDIO PollingStatus per PORT' */
/* Bit: 'CLK_STOP_CAPABLE' */
/* Description: 'PHY supports MAC turning of TX clk' */
#define MMDIO_STAT_4_CLK_STOP_CAPABLE_OFFSET 0xF41A
#define MMDIO_STAT_4_CLK_STOP_CAPABLE_SHIFT 8
#define MMDIO_STAT_4_CLK_STOP_CAPABLE_SIZE  1
/* Bit: 'EEE_CAPABLE' */
/* Description: 'PHY and link partner support EEE for current speed' */
#define MMDIO_STAT_4_EEE_CAPABLE_OFFSET 0xF41A
#define MMDIO_STAT_4_EEE_CAPABLE_SHIFT 7
#define MMDIO_STAT_4_EEE_CAPABLE_SIZE  1
/* Bit: 'PACT' */
/* Description: 'PHY Active Status' */
#define MMDIO_STAT_4_PACT_OFFSET 0xF41A
#define MMDIO_STAT_4_PACT_SHIFT 6
#define MMDIO_STAT_4_PACT_SIZE  1
/* Bit: 'LSTAT' */
/* Description: 'Link Status' */
#define MMDIO_STAT_4_LSTAT_OFFSET 0xF41A
#define MMDIO_STAT_4_LSTAT_SHIFT  5
#define MMDIO_STAT_4_LSTAT_SIZE   1
/* Bit: 'SPEED' */
/* Description: 'Speed Control' */
#define MMDIO_STAT_4_SPEED_OFFSET 0xF41A
#define MMDIO_STAT_4_SPEED_SHIFT  3
#define MMDIO_STAT_4_SPEED_SIZE   2
/* Bit: 'FDUP' */
/* Description: 'Full Duplex Status' */
#define MMDIO_STAT_4_FDUP_OFFSET 0xF41A
#define MMDIO_STAT_4_FDUP_SHIFT  2
#define MMDIO_STAT_4_FDUP_SIZE   1
/* Bit: 'RXPAUEN' */
/* Description: 'Receive Pause Enable Status' */
#define MMDIO_STAT_4_RXPAUEN_OFFSET 0xF41A
#define MMDIO_STAT_4_RXPAUEN_SHIFT  1
#define MMDIO_STAT_4_RXPAUEN_SIZE   1
/* Bit: 'TXPAUEN' */
/* Description: 'Transmit Pause Enable Status' */
#define MMDIO_STAT_4_TXPAUEN_OFFSET 0xF41A
#define MMDIO_STAT_4_TXPAUEN_SHIFT  0
#define MMDIO_STAT_4_TXPAUEN_SIZE   1
/* ----------------------------------------------- */
/* Register: 'PHY MDIO PollingStatus per PORT' */
/* Bit: 'CLK_STOP_CAPABLE' */
/* Description: 'PHY supports MAC turning of TX clk' */
#define MMDIO_STAT_5_CLK_STOP_CAPABLE_OFFSET 0xF41B
#define MMDIO_STAT_5_CLK_STOP_CAPABLE_SHIFT 8
#define MMDIO_STAT_5_CLK_STOP_CAPABLE_SIZE  1
/* Bit: 'EEE_CAPABLE' */
/* Description: 'PHY and link partner support EEE for current speed' */
#define MMDIO_STAT_5_EEE_CAPABLE_OFFSET 0xF41B
#define MMDIO_STAT_5_EEE_CAPABLE_SHIFT 7
#define MMDIO_STAT_5_EEE_CAPABLE_SIZE  1
/* Bit: 'PACT' */
/* Description: 'PHY Active Status' */
#define MMDIO_STAT_5_PACT_OFFSET 0xF41B
#define MMDIO_STAT_5_PACT_SHIFT 6
#define MMDIO_STAT_5_PACT_SIZE  1
/* Bit: 'LSTAT' */
/* Description: 'Link Status' */
#define MMDIO_STAT_5_LSTAT_OFFSET 0xF41B
#define MMDIO_STAT_5_LSTAT_SHIFT  5
#define MMDIO_STAT_5_LSTAT_SIZE   1
/* Bit: 'SPEED' */
/* Description: 'Speed Control' */
#define MMDIO_STAT_5_SPEED_OFFSET 0xF41B
#define MMDIO_STAT_5_SPEED_SHIFT  3
#define MMDIO_STAT_5_SPEED_SIZE   2
/* Bit: 'FDUP' */
/* Description: 'Full Duplex Status' */
#define MMDIO_STAT_5_FDUP_OFFSET 0xF41B
#define MMDIO_STAT_5_FDUP_SHIFT  2
#define MMDIO_STAT_5_FDUP_SIZE   1
/* Bit: 'RXPAUEN' */
/* Description: 'Receive Pause Enable Status' */
#define MMDIO_STAT_5_RXPAUEN_OFFSET 0xF41B
#define MMDIO_STAT_5_RXPAUEN_SHIFT  1
#define MMDIO_STAT_5_RXPAUEN_SIZE   1
/* Bit: 'TXPAUEN' */
/* Description: 'Transmit Pause Enable Status' */
#define MMDIO_STAT_5_TXPAUEN_OFFSET 0xF41B
#define MMDIO_STAT_5_TXPAUEN_SHIFT  0
#define MMDIO_STAT_5_TXPAUEN_SIZE   1
/* ----------------------------------------------- */
/* Register: 'PHY MDIO PollingStatus per PORT' */
/* Bit: 'CLK_STOP_CAPABLE' */
/* Description: 'PHY supports MAC turning of TX clk' */
#define MMDIO_STAT_6_CLK_STOP_CAPABLE_OFFSET 0xF41C
#define MMDIO_STAT_6_CLK_STOP_CAPABLE_SHIFT 8
#define MMDIO_STAT_6_CLK_STOP_CAPABLE_SIZE  1
/* Bit: 'EEE_CAPABLE' */
/* Description: 'PHY and link partner support EEE for current speed' */
#define MMDIO_STAT_6_EEE_CAPABLE_OFFSET 0xF41C
#define MMDIO_STAT_6_EEE_CAPABLE_SHIFT 7
#define MMDIO_STAT_6_EEE_CAPABLE_SIZE  1
/* Bit: 'PACT' */
/* Description: 'PHY Active Status' */
#define MMDIO_STAT_6_PACT_OFFSET 0xF41C
#define MMDIO_STAT_6_PACT_SHIFT 6
#define MMDIO_STAT_6_PACT_SIZE  1
/* Bit: 'LSTAT' */
/* Description: 'Link Status' */
#define MMDIO_STAT_6_LSTAT_OFFSET 0xF41C
#define MMDIO_STAT_6_LSTAT_SHIFT  5
#define MMDIO_STAT_6_LSTAT_SIZE   1
/* Bit: 'SPEED' */
/* Description: 'Speed Control' */
#define MMDIO_STAT_6_SPEED_OFFSET 0xF41C
#define MMDIO_STAT_6_SPEED_SHIFT  3
#define MMDIO_STAT_6_SPEED_SIZE   2
/* Bit: 'FDUP' */
/* Description: 'Full Duplex Status' */
#define MMDIO_STAT_6_FDUP_OFFSET 0xF41C
#define MMDIO_STAT_6_FDUP_SHIFT  2
#define MMDIO_STAT_6_FDUP_SIZE   1
/* Bit: 'RXPAUEN' */
/* Description: 'Receive Pause Enable Status' */
#define MMDIO_STAT_6_RXPAUEN_OFFSET 0xF41C
#define MMDIO_STAT_6_RXPAUEN_SHIFT  1
#define MMDIO_STAT_6_RXPAUEN_SIZE   1
/* Bit: 'TXPAUEN' */
/* Description: 'Transmit Pause Enable Status' */
#define MMDIO_STAT_6_TXPAUEN_OFFSET 0xF41C
#define MMDIO_STAT_6_TXPAUEN_SHIFT  0
#define MMDIO_STAT_6_TXPAUEN_SIZE   1
/* ----------------------------------------------- */
/* Register: 'EEE auto negotiationoverides' for PORT 0*/
/* Bit: 'CLK_STOP_CAPABLE' */
/* Description: 'clk stop capable' */
#define ANEG_EEE_0_CLK_STOP_CAPABLE_OFFSET 0xF41D
#define ANEG_EEE_0_CLK_STOP_CAPABLE_SHIFT  2
#define ANEG_EEE_0_CLK_STOP_CAPABLE_SIZE   2
/* Bit: 'EEE_CAPABLE' */
/* Description: 'EEE capable' */
#define ANEG_EEE_0_EEE_CAPABLE_OFFSET 0xF41D
#define ANEG_EEE_0_EEE_CAPABLE_SHIFT 0
#define ANEG_EEE_0_EEE_CAPABLE_SIZE  2
/* ----------------------------------------------- */
/* Register: 'EEE auto negotiationoverides' for PORT 1*/
/* Bit: 'CLK_STOP_CAPABLE' */
/* Description: 'clk stop capable' */
#define ANEG_EEE_1_CLK_STOP_CAPABLE_OFFSET 0xF41E
#define ANEG_EEE_1_CLK_STOP_CAPABLE_SHIFT  2
#define ANEG_EEE_1_CLK_STOP_CAPABLE_SIZE   2
/* Bit: 'EEE_CAPABLE' */
/* Description: 'EEE capable' */
#define ANEG_EEE_1_EEE_CAPABLE_OFFSET 0xF41E
#define ANEG_EEE_1_EEE_CAPABLE_SHIFT 0
#define ANEG_EEE_1_EEE_CAPABLE_SIZE  2
/* ----------------------------------------------- */
/* Register: 'EEE auto negotiationoverides' for PORT 2*/
/* Bit: 'CLK_STOP_CAPABLE' */
/* Description: 'clk stop capable' */
#define ANEG_EEE_2_CLK_STOP_CAPABLE_OFFSET 0xF41F
#define ANEG_EEE_2_CLK_STOP_CAPABLE_SHIFT  2
#define ANEG_EEE_2_CLK_STOP_CAPABLE_SIZE   2
/* Bit: 'EEE_CAPABLE' */
/* Description: 'EEE capable' */
#define ANEG_EEE_2_EEE_CAPABLE_OFFSET 0xF41F
#define ANEG_EEE_2_EEE_CAPABLE_SHIFT 0
#define ANEG_EEE_2_EEE_CAPABLE_SIZE  2
/* ----------------------------------------------- */
/* Register: 'EEE auto negotiationoverides' for PORT 3*/
/* Bit: 'CLK_STOP_CAPABLE' */
/* Description: 'clk stop capable' */
#define ANEG_EEE_3_CLK_STOP_CAPABLE_OFFSET 0xF420
#define ANEG_EEE_3_CLK_STOP_CAPABLE_SHIFT  2
#define ANEG_EEE_3_CLK_STOP_CAPABLE_SIZE   2
/* Bit: 'EEE_CAPABLE' */
/* Description: 'EEE capable' */
#define ANEG_EEE_3_EEE_CAPABLE_OFFSET 0xF420
#define ANEG_EEE_3_EEE_CAPABLE_SHIFT 0
#define ANEG_EEE_3_EEE_CAPABLE_SIZE  2
/* ----------------------------------------------- */
/* Register: 'EEE auto negotiationoverides' for PORT 4*/
/* Bit: 'CLK_STOP_CAPABLE' */
/* Description: 'clk stop capable' */
#define ANEG_EEE_4_CLK_STOP_CAPABLE_OFFSET 0xF421
#define ANEG_EEE_4_CLK_STOP_CAPABLE_SHIFT  2
#define ANEG_EEE_4_CLK_STOP_CAPABLE_SIZE   2
/* Bit: 'EEE_CAPABLE' */
/* Description: 'EEE capable' */
#define ANEG_EEE_4_EEE_CAPABLE_OFFSET 0xF421
#define ANEG_EEE_4_EEE_CAPABLE_SHIFT 0
#define ANEG_EEE_4_EEE_CAPABLE_SIZE  2
/* ----------------------------------------------- */
/* Register: 'EEE auto negotiationoverides' for PORT 5*/
/* Bit: 'CLK_STOP_CAPABLE' */
/* Description: 'clk stop capable' */
#define ANEG_EEE_5_CLK_STOP_CAPABLE_OFFSET 0xF422
#define ANEG_EEE_5_CLK_STOP_CAPABLE_SHIFT  2
#define ANEG_EEE_5_CLK_STOP_CAPABLE_SIZE   2
/* Bit: 'EEE_CAPABLE' */
/* Description: 'EEE capable' */
#define ANEG_EEE_5_EEE_CAPABLE_OFFSET 0xF422
#define ANEG_EEE_5_EEE_CAPABLE_SHIFT 0
#define ANEG_EEE_5_EEE_CAPABLE_SIZE  2
/* ----------------------------------------------- */
/* Register: 'EEE auto negotiationoverides' for PORT 6*/
/* Bit: 'CLK_STOP_CAPABLE' */
/* Description: 'clk stop capable' */
#define ANEG_EEE_6_CLK_STOP_CAPABLE_OFFSET 0xF423
#define ANEG_EEE_6_CLK_STOP_CAPABLE_SHIFT  2
#define ANEG_EEE_6_CLK_STOP_CAPABLE_SIZE   2
/* Bit: 'EEE_CAPABLE' */
/* Description: 'EEE capable' */
#define ANEG_EEE_6_EEE_CAPABLE_OFFSET 0xF423
#define ANEG_EEE_6_EEE_CAPABLE_SHIFT 0
#define ANEG_EEE_6_EEE_CAPABLE_SIZE  2
/* ----------------------------------------------- */
/*SMDIO Configuration */
/* ----------------------------------------------- */
/* Register: 'MDC Slave Configuration Register*/
/* Bit: 'RST' */
/* Description: 'MDIO Slave Hardware Reset' */
#define SMDIO_CFG_RST_OFFSET 0xF480
#define SMDIO_CFG_RST_SHIFT  15
#define SMDIO_CFG_RST_SIZE   1
/* Bit: 'ADDR' */
/* Description: 'SMDIO Address' */
#define SMDIO_CFG_ADDR_OFFSET 0xF480
#define SMDIO_CFG_ADDR_SHIFT 4
#define SMDIO_CFG_ADDR_SIZE  5
/* Bit: 'GPHYISO' */
/* Description: 'GPHY MDIO Isolation Mode' */
#define SMDIO_CFG_GPHYISO_OFFSET 0xF480
#define SMDIO_CFG_GPHYISO_SHIFT 2
#define SMDIO_CFG_GPHYISO_SIZE  2
/* Bit: 'EN' */
/* Description: 'SMDIO Interface Enable' */
#define SMDIO_CFG_EN_OFFSET 0xF480
#define SMDIO_CFG_EN_SHIFT 0
#define SMDIO_CFG_EN_SIZE  1
/* ----------------------------------------------- */
/* Register: 'MDC Slave Target Base Address Register*/
/* Bit: 'ADDR' */
/* Description: 'Target Base Address' */
#define SMDIO_BADR_ADDR_OFFSET 0xF481
#define SMDIO_BADR_ADDR_SHIFT  0
#define SMDIO_BADR_ADDR_SIZE   16
/* ----------------------------------------------- */
/* GPHY shell registers*/
/* ----------------------------------------------- */
/* Register: 'GPHY0 Firmware Address Offset Register' */
/* Bit: 'MEMSEL' */
/* Description: 'GPHY Code Memory Mode' */
#define GPHY0_FCR_MEMSEL_OFFSET 0xF700
#define GPHY0_FCR_MEMSEL_SHIFT 15
#define GPHY0_FCR_MEMSEL_SIZE  1
/* Bit: 'INV' */
/* Description: 'Firmware Address Inversion' */
#define GPHY0_FCR_INV_OFFSET 0xF700
#define GPHY0_FCR_INV_SHIFT 14
#define GPHY0_FCR_INV_SIZE  1
/* Bit: 'FCR' */
/* Description: 'Firmware Address Offset MSB' */
#define GPHY0_FCR_FCR_OFFSET 0xF700
#define GPHY0_FCR_FCR_SHIFT 0
#define GPHY0_FCR_FCR_SIZE  14
/* ----------------------------------------------- */
/* Register: 'GPHY0 General Configuration Register' */
/* Bit: 'IDCNMI' */
/* Description: 'IDC Non-maskable Interrupt' */
#define GPHY0_CFG_IDCNMI_OFFSET 0xF701
#define GPHY0_CFG_IDCNMI_SHIFT 1
#define GPHY0_CFG_IDCNMI_SIZE  1
/* Bit: 'MDINTP' */
/* Description: 'MDIO Interrupt Polarity' */
#define GPHY0_CFG_MDINTP_OFFSET 0xF701
#define GPHY0_CFG_MDINTP_SHIFT 0
#define GPHY0_CFG_MDINTP_SIZE  1
/* ----------------------------------------------- */
/* Register: 'GPHY0 AFE TX Path Control Register' */
/* Bit: 'AFETX' */
/* Description: 'Control for AFE TX' */
#define GPHY0_AFETX_CTRL_OFFSET 0xF702
#define GPHY0_AFETX_CTRL_SHIFT 0
#define GPHY0_AFETX_CTRL_SIZE  8
/* ----------------------------------------------- */
/* Register: 'GPHY0 Firmware Address Offset Shadow Register' */
/* Bit: 'MEMSEL' */
/* Description: 'GPHY Code Memory Mode' */
#define GPHY0_FCR_SD_MEMSEL_OFFSET 0xF703
#define GPHY0_FCR_SD_MEMSEL_SHIFT 15
#define GPHY0_FCR_SD_MEMSEL_SIZE  1
/* Bit: 'INV' */
/* Description: 'Firmware Address Inversion' */
#define GPHY0_FCR_SD_INV_OFFSET 0xF703
#define GPHY0_FCR_SD_INV_SHIFT 14
#define GPHY0_FCR_SD_INV_SIZE  1
/* Bit: 'FCR' */
/* Description: 'Firmware Address Offset MSB' */
#define GPHY0_FCR_SD_FCR_OFFSET 0xF703
#define GPHY0_FCR_SD_FCR_SHIFT 0
#define GPHY0_FCR_SD_FCR_SIZE  14
/* ----------------------------------------------- */
/* Register: 'GPHY0 General Pin Strapping Register' */
/* Bit: 'GPS' */
/* Description: 'General Pin Strapping' */
#define GPHY0_GPS_OFFSET 0xF708
#define GPHY0_GPS_SHIFT 0
#define GPHY0_GPS_SIZE  8
/* ----------------------------------------------- */
/* Register: 'GPHY0 Base Frequency Deviation Configuration Register' */
/* Bit: 'BFDEV' */
/* Description: 'Base Frequency Deviation' */
#define GPHY0_BFDEV_OFFSET 0xF709
#define GPHY0_BFDEV_SHIFT 0
#define GPHY0_BFDEV_SIZE  16
/* ----------------------------------------------- */
/* Register: 'GPHY0 General Status Register' */
/* Bit: 'IDCPWD' */
/* Description: 'IDC Power Down' */
#define GPHY0_STATUS_IDCPWD_OFFSET 0xF70F
#define GPHY0_STATUS_IDCPWD_SHIFT 1
#define GPHY0_STATUS_IDCPWD_SIZE  1
/* Bit: 'IDCIDLE' */
/* Description: 'IDC Idle' */
#define GPHY0_STATUS_IDCIDLE_OFFSET 0xF70F
#define GPHY0_STATUS_IDCIDLE_SHIFT 0
#define GPHY0_STATUS_IDCIDLE_SIZE  1
/* ----------------------------------------------- */

/* Register: 'GPHY1 Firmware Address Offset Register' */
/* Bit: 'MEMSEL' */
/* Description: 'GPHY Code Memory Mode' */
#define GPHY1_FCR_MEMSEL_OFFSET 0xF710
#define GPHY1_FCR_MEMSEL_SHIFT 15
#define GPHY1_FCR_MEMSEL_SIZE  1
/* Bit: 'INV' */
/* Description: 'Firmware Address Inversion' */
#define GPHY1_FCR_INV_OFFSET 0xF710
#define GPHY1_FCR_INV_SHIFT 14
#define GPHY1_FCR_INV_SIZE  1
/* Bit: 'FCR' */
/* Description: 'Firmware Address Offset MSB' */
#define GPHY1_FCR_FCR_OFFSET 0xF710
#define GPHY1_FCR_FCR_SHIFT 0
#define GPHY1_FCR_FCR_SIZE  14
/* ----------------------------------------------- */
/* Register: 'GPHY1 General Configuration Register' */
/* Bit: 'IDCNMI' */
/* Description: 'IDC Non-maskable Interrupt' */
#define GPHY1_CFG_IDCNMI_OFFSET 0xF711
#define GPHY1_CFG_IDCNMI_SHIFT 1
#define GPHY1_CFG_IDCNMI_SIZE  1
/* Bit: 'MDINTP' */
/* Description: 'MDIO Interrupt Polarity' */
#define GPHY1_CFG_MDINTP_OFFSET 0xF711
#define GPHY1_CFG_MDINTP_SHIFT 0
#define GPHY1_CFG_MDINTP_SIZE  1
/* ----------------------------------------------- */
/* Register: 'GPHY1 AFE TX Path Control Register' */
/* Bit: 'AFETX' */
/* Description: 'Control for AFE TX' */
#define GPHY1_AFETX_CTRL_OFFSET 0xF712
#define GPHY1_AFETX_CTRL_SHIFT 0
#define GPHY1_AFETX_CTRL_SIZE  8
/* ----------------------------------------------- */
/* Register: 'GPHY1 Firmware Address Offset Shadow Register' */
/* Bit: 'MEMSEL' */
/* Description: 'GPHY Code Memory Mode' */
#define GPHY1_FCR_SD_MEMSEL_OFFSET 0xF713
#define GPHY1_FCR_SD_MEMSEL_SHIFT 15
#define GPHY1_FCR_SD_MEMSEL_SIZE  1
/* Bit: 'INV' */
/* Description: 'Firmware Address Inversion' */
#define GPHY1_FCR_SD_INV_OFFSET 0xF713
#define GPHY1_FCR_SD_INV_SHIFT 14
#define GPHY1_FCR_SD_INV_SIZE  1
/* Bit: 'FCR' */
/* Description: 'Firmware Address Offset MSB' */
#define GPHY1_FCR_SD_FCR_OFFSET 0xF713
#define GPHY1_FCR_SD_FCR_SHIFT 0
#define GPHY1_FCR_SD_FCR_SIZE  14
/* ----------------------------------------------- */
/* Register: 'GPHY1 General Pin Strapping Register' */
/* Bit: 'GPS' */
/* Description: 'General Pin Strapping' */
#define GPHY1_GPS_OFFSET 0xF718
#define GPHY1_GPS_SHIFT 0
#define GPHY1_GPS_SIZE  8
/* ----------------------------------------------- */
/* Register: 'GPHY0 Base Frequency Deviation Configuration Register' */
/* Bit: 'BFDEV' */
/* Description: 'Base Frequency Deviation' */
#define GPHY1_BFDEV_OFFSET 0xF719
#define GPHY1_BFDEV_SHIFT 0
#define GPHY1_BFDEV_SIZE  16
/* ----------------------------------------------- */
/* Register: 'GPHY0 General Status Register' */
/* Bit: 'IDCPWD' */
/* Description: 'IDC Power Down' */
#define GPHY1_STATUS_IDCPWD_OFFSET 0xF71F
#define GPHY1_STATUS_IDCPWD_SHIFT 1
#define GPHY1_STATUS_IDCPWD_SIZE  1
/* Bit: 'IDCIDLE' */
/* Description: 'IDC Idle' */
#define GPHY1_STATUS_IDCIDLE_OFFSET 0xF71F
#define GPHY1_STATUS_IDCIDLE_SHIFT 0
#define GPHY1_STATUS_IDCIDLE_SIZE  1
/* ----------------------------------------------- */

/* Register: 'GPHY2 Firmware Address Offset Register' */
/* Bit: 'MEMSEL' */
/* Description: 'GPHY Code Memory Mode' */
#define GPHY2_FCR_MEMSEL_OFFSET 0xF720
#define GPHY2_FCR_MEMSEL_SHIFT 15
#define GPHY2_FCR_MEMSEL_SIZE  1
/* Bit: 'INV' */
/* Description: 'Firmware Address Inversion' */
#define GPHY2_FCR_INV_OFFSET 0xF720
#define GPHY2_FCR_INV_SHIFT 14
#define GPHY2_FCR_INV_SIZE  1
/* Bit: 'FCR' */
/* Description: 'Firmware Address Offset MSB' */
#define GPHY2_FCR_FCR_OFFSET 0xF720
#define GPHY2_FCR_FCR_SHIFT 0
#define GPHY2_FCR_FCR_SIZE  14
/* ----------------------------------------------- */
/* Register: 'GPHY2 General Configuration Register' */
/* Bit: 'IDCNMI' */
/* Description: 'IDC Non-maskable Interrupt' */
#define GPHY2_CFG_IDCNMI_OFFSET 0xF721
#define GPHY2_CFG_IDCNMI_SHIFT 1
#define GPHY2_CFG_IDCNMI_SIZE  1
/* Bit: 'MDINTP' */
/* Description: 'MDIO Interrupt Polarity' */
#define GPHY2_CFG_MDINTP_OFFSET 0xF721
#define GPHY2_CFG_MDINTP_SHIFT 0
#define GPHY2_CFG_MDINTP_SIZE  1
/* ----------------------------------------------- */
/* Register: 'GPHY2 AFE TX Path Control Register' */
/* Bit: 'AFETX' */
/* Description: 'Control for AFE TX' */
#define GPHY2_AFETX_CTRL_OFFSET 0xF722
#define GPHY2_AFETX_CTRL_SHIFT 0
#define GPHY2_AFETX_CTRL_SIZE  8
/* ----------------------------------------------- */
/* Register: 'GPHY2 Firmware Address Offset Shadow Register' */
/* Bit: 'MEMSEL' */
/* Description: 'GPHY Code Memory Mode' */
#define GPHY2_FCR_SD_MEMSEL_OFFSET 0xF723
#define GPHY2_FCR_SD_MEMSEL_SHIFT 15
#define GPHY2_FCR_SD_MEMSEL_SIZE  1
/* Bit: 'INV' */
/* Description: 'Firmware Address Inversion' */
#define GPHY2_FCR_SD_INV_OFFSET 0xF723
#define GPHY2_FCR_SD_INV_SHIFT 14
#define GPHY2_FCR_SD_INV_SIZE  1
/* Bit: 'FCR' */
/* Description: 'Firmware Address Offset MSB' */
#define GPHY2_FCR_SD_FCR_OFFSET 0xF723
#define GPHY2_FCR_SD_FCR_SHIFT 0
#define GPHY2_FCR_SD_FCR_SIZE  14
/* ----------------------------------------------- */
/* Register: 'GPHY2 General Pin Strapping Register' */
/* Bit: 'GPS' */
/* Description: 'General Pin Strapping' */
#define GPHY2_GPS_OFFSET 0xF728
#define GPHY2_GPS_SHIFT 0
#define GPHY2_GPS_SIZE  8
/* ----------------------------------------------- */
/* Register: 'GPHY0 Base Frequency Deviation Configuration Register' */
/* Bit: 'BFDEV' */
/* Description: 'Base Frequency Deviation' */
#define GPHY2_BFDEV_OFFSET 0xF729
#define GPHY2_BFDEV_SHIFT 0
#define GPHY2_BFDEV_SIZE  16
/* ----------------------------------------------- */
/* Register: 'GPHY0 General Status Register' */
/* Bit: 'IDCPWD' */
/* Description: 'IDC Power Down' */
#define GPHY2_STATUS_IDCPWD_OFFSET 0xF72F
#define GPHY2_STATUS_IDCPWD_SHIFT 1
#define GPHY2_STATUS_IDCPWD_SIZE  1
/* Bit: 'IDCIDLE' */
/* Description: 'IDC Idle' */
#define GPHY2_STATUS_IDCIDLE_OFFSET 0xF72F
#define GPHY2_STATUS_IDCIDLE_SHIFT 0
#define GPHY2_STATUS_IDCIDLE_SIZE  1
/* ----------------------------------------------- */

/* Register: 'GPHY3 Firmware Address Offset Register' */
/* Bit: 'MEMSEL' */
/* Description: 'GPHY Code Memory Mode' */
#define GPHY3_FCR_MEMSEL_OFFSET 0xF730
#define GPHY3_FCR_MEMSEL_SHIFT 15
#define GPHY3_FCR_MEMSEL_SIZE  1
/* Bit: 'INV' */
/* Description: 'Firmware Address Inversion' */
#define GPHY3_FCR_INV_OFFSET 0xF730
#define GPHY3_FCR_INV_SHIFT 14
#define GPHY3_FCR_INV_SIZE  1
/* Bit: 'FCR' */
/* Description: 'Firmware Address Offset MSB' */
#define GPHY3_FCR_FCR_OFFSET 0xF730
#define GPHY3_FCR_FCR_SHIFT 0
#define GPHY3_FCR_FCR_SIZE  14
/* ----------------------------------------------- */
/* Register: 'GPHY3 General Configuration Register' */
/* Bit: 'IDCNMI' */
/* Description: 'IDC Non-maskable Interrupt' */
#define GPHY3_CFG_IDCNMI_OFFSET 0xF731
#define GPHY3_CFG_IDCNMI_SHIFT 1
#define GPHY3_CFG_IDCNMI_SIZE  1
/* Bit: 'MDINTP' */
/* Description: 'MDIO Interrupt Polarity' */
#define GPHY3_CFG_MDINTP_OFFSET 0xF731
#define GPHY3_CFG_MDINTP_SHIFT 0
#define GPHY3_CFG_MDINTP_SIZE  1
/* ----------------------------------------------- */
/* Register: 'GPHY3 AFE TX Path Control Register' */
/* Bit: 'AFETX' */
/* Description: 'Control for AFE TX' */
#define GPHY3_AFETX_CTRL_OFFSET 0xF732
#define GPHY3_AFETX_CTRL_SHIFT 0
#define GPHY3_AFETX_CTRL_SIZE  8
/* ----------------------------------------------- */
/* Register: 'GPHY3 Firmware Address Offset Shadow Register' */
/* Bit: 'MEMSEL' */
/* Description: 'GPHY Code Memory Mode' */
#define GPHY3_FCR_SD_MEMSEL_OFFSET 0xF733
#define GPHY3_FCR_SD_MEMSEL_SHIFT 15
#define GPHY3_FCR_SD_MEMSEL_SIZE  1
/* Bit: 'INV' */
/* Description: 'Firmware Address Inversion' */
#define GPHY3_FCR_SD_INV_OFFSET 0xF733
#define GPHY3_FCR_SD_INV_SHIFT 14
#define GPHY3_FCR_SD_INV_SIZE  1
/* Bit: 'FCR' */
/* Description: 'Firmware Address Offset MSB' */
#define GPHY3_FCR_SD_FCR_OFFSET 0xF733
#define GPHY3_FCR_SD_FCR_SHIFT 0
#define GPHY3_FCR_SD_FCR_SIZE  14
/* ----------------------------------------------- */
/* Register: 'GPHY3 General Pin Strapping Register' */
/* Bit: 'GPS' */
/* Description: 'General Pin Strapping' */
#define GPHY3_GPS_OFFSET 0xF738
#define GPHY3_GPS_SHIFT 0
#define GPHY3_GPS_SIZE  8
/* ----------------------------------------------- */
/* Register: 'GPHY3 Base Frequency Deviation Configuration Register' */
/* Bit: 'BFDEV' */
/* Description: 'Base Frequency Deviation' */
#define GPHY3_BFDEV_OFFSET 0xF739
#define GPHY3_BFDEV_SHIFT 0
#define GPHY3_BFDEV_SIZE  16
/* ----------------------------------------------- */
/* Register: 'GPHY0 General Status Register' */
/* Bit: 'IDCPWD' */
/* Description: 'IDC Power Down' */
#define GPHY3_STATUS_IDCPWD_OFFSET 0xF73F
#define GPHY3_STATUS_IDCPWD_SHIFT 1
#define GPHY3_STATUS_IDCPWD_SIZE  1
/* Bit: 'IDCIDLE' */
/* Description: 'IDC Idle' */
#define GPHY3_STATUS_IDCIDLE_OFFSET 0xF73F
#define GPHY3_STATUS_IDCIDLE_SHIFT 0
#define GPHY3_STATUS_IDCIDLE_SIZE  1
/* ----------------------------------------------- */

/* Register: 'GPHY4 Firmware Address Offset Register' */
/* Bit: 'MEMSEL' */
/* Description: 'GPHY Code Memory Mode' */
#define GPHY4_FCR_MEMSEL_OFFSET 0xF740
#define GPHY4_FCR_MEMSEL_SHIFT 15
#define GPHY4_FCR_MEMSEL_SIZE  1
/* Bit: 'INV' */
/* Description: 'Firmware Address Inversion' */
#define GPHY4_FCR_INV_OFFSET 0xF740
#define GPHY4_FCR_INV_SHIFT 14
#define GPHY4_FCR_INV_SIZE  1
/* Bit: 'FCR' */
/* Description: 'Firmware Address Offset MSB' */
#define GPHY4_FCR_FCR_OFFSET 0xF740
#define GPHY4_FCR_FCR_SHIFT 0
#define GPHY4_FCR_FCR_SIZE  14
/* ----------------------------------------------- */
/* Register: 'GPHY4 General Configuration Register' */
/* Bit: 'IDCNMI' */
/* Description: 'IDC Non-maskable Interrupt' */
#define GPHY4_CFG_IDCNMI_OFFSET 0xF741
#define GPHY4_CFG_IDCNMI_SHIFT 1
#define GPHY4_CFG_IDCNMI_SIZE  1
/* Bit: 'MDINTP' */
/* Description: 'MDIO Interrupt Polarity' */
#define GPHY4_CFG_MDINTP_OFFSET 0xF741
#define GPHY4_CFG_MDINTP_SHIFT 0
#define GPHY4_CFG_MDINTP_SIZE  1
/* ----------------------------------------------- */
/* Register: 'GPHY4 AFE TX Path Control Register' */
/* Bit: 'AFETX' */
/* Description: 'Control for AFE TX' */
#define GPHY4_AFETX_CTRL_OFFSET 0xF742
#define GPHY4_AFETX_CTRL_SHIFT 0
#define GPHY4_AFETX_CTRL_SIZE  8
/* ----------------------------------------------- */
/* Register: 'GPHY4 Firmware Address Offset Shadow Register' */
/* Bit: 'MEMSEL' */
/* Description: 'GPHY Code Memory Mode' */
#define GPHY4_FCR_SD_MEMSEL_OFFSET 0xF743
#define GPHY4_FCR_SD_MEMSEL_SHIFT 15
#define GPHY4_FCR_SD_MEMSEL_SIZE  1
/* Bit: 'INV' */
/* Description: 'Firmware Address Inversion' */
#define GPHY4_FCR_SD_INV_OFFSET 0xF743
#define GPHY4_FCR_SD_INV_SHIFT 14
#define GPHY4_FCR_SD_INV_SIZE  1
/* Bit: 'FCR' */
/* Description: 'Firmware Address Offset MSB' */
#define GPHY4_FCR_SD_FCR_OFFSET 0xF743
#define GPHY4_FCR_SD_FCR_SHIFT 0
#define GPHY4_FCR_SD_FCR_SIZE  14
/* ----------------------------------------------- */
/* Register: 'GPHY4 General Pin Strapping Register' */
/* Bit: 'GPS' */
/* Description: 'General Pin Strapping' */
#define GPHY4_GPS_OFFSET 0xF748
#define GPHY4_GPS_SHIFT 0
#define GPHY4_GPS_SIZE  8
/* ----------------------------------------------- */
/* Register: 'GPHY4 Base Frequency Deviation Configuration Register' */
/* Bit: 'BFDEV' */
/* Description: 'Base Frequency Deviation' */
#define GPHY4_BFDEV_OFFSET 0xF749
#define GPHY4_BFDEV_SHIFT 0
#define GPHY4_BFDEV_SIZE  16
/* ----------------------------------------------- */
/* Register: 'GPHY4 General Status Register' */
/* Bit: 'IDCPWD' */
/* Description: 'IDC Power Down' */
#define GPHY4_STATUS_IDCPWD_OFFSET 0xF74F
#define GPHY4_STATUS_IDCPWD_SHIFT 1
#define GPHY4_STATUS_IDCPWD_SIZE  1
/* Bit: 'IDCIDLE' */
/* Description: 'IDC Idle' */
#define GPHY4_STATUS_IDCIDLE_OFFSET 0xF74F
#define GPHY4_STATUS_IDCIDLE_SHIFT 0
#define GPHY4_STATUS_IDCIDLE_SIZE  1
/* ----------------------------------------------- */

/* ----------------------------------------------- */
/* UART Configuration */
/* ----------------------------------------------- */
/* Register: 'UART Configuration Register' */
/* Bit: 'IDCPWD' */
/* Description: 'LF As Enter Disable' */
#define UART_CFG_LFDIS_OFFSET 0xF680
#define UART_CFG_LFDIS_SHIFT 9
#define UART_CFG_LFDIS_SIZE  1
/* Bit: 'CRDIS' */
/* Description: 'CR As Enter Disable' */
#define UART_CFG_CRDIS_OFFSET 0xF680
#define UART_CFG_CRDIS_SHIFT 8
#define UART_CFG_CRDIS_SIZE  1
/* Bit: 'STOP' */
/* Description: 'Additional Stop Bits' */
#define UART_CFG_STOP_OFFSET 0xF680
#define UART_CFG_STOP_SHIFT 4
#define UART_CFG_STOP_SIZE  4
/* Bit: 'PAREN' */
/* Description: 'UART Parity Enable' */
#define UART_CFG_PAREN_OFFSET 0xF680
#define UART_CFG_PAREN_SHIFT 1
#define UART_CFG_PAREN_SIZE  1
/* Bit: 'EN' */
/* Description: 'UART Interface Enable' */
#define UART_CFG_EN_OFFSET 0xF680
#define UART_CFG_EN_SHIFT 0
#define UART_CFG_EN_SIZE  1
/* ----------------------------------------------- */
/* Register: 'UART Baudrate Register' */
/* Bit: 'BD' */
/* Description: 'Baudrate Divider' */
#define UART_BD_OFFSET 0xF681
#define UART_BD_SHIFT 0
#define UART_BD_SIZE  16
/* ----------------------------------------------- */
/* Register: 'UART Baudrate Fractional Divider Register' */
/* Bit: 'FDIV' */
/* Description: 'Baudrate Fractional Divider' */
#define UART_FDIV_OFFSET 0xF682
#define UART_FDIV_SHIFT 0
#define UART_FDIV_SIZE  8
/* ----------------------------------------------- */
/* Register: 'UART PROMPT Register' */
/* Bit: 'PROMPT1' */
/* Description: 'Second Prompt Character' */
#define UART_PROMPT1_OFFSET 0xF683
#define UART_PROMPT1_SHIFT 8
#define UART_PROMPT1_SIZE  8
/* Bit: 'PROMPT0' */
/* Description: 'First Prompt Character' */
#define UART_PROMPT0_OFFSET 0xF683
#define UART_PROMPT0_SHIFT 0
#define UART_PROMPT0_SIZE  8
/* ----------------------------------------------- */
/* Register: 'UART Error Counter Register' */
/* Bit: 'CNT' */
/* Description: 'Error Counter' */
#define UART_ERRCNT_CNT_OFFSET 0xF684
#define UART_ERRCNT_CNT_SHIFT 0
#define UART_ERRCNT_CNT_SIZE  16
/* ----------------------------------------------- */

/* ----------------------------------------------- */
/* Clock Generation Unit Registers */
/* ----------------------------------------------- */
/* Register: 'GPC0 Configuration Register' */
/* Bit: 'SEL' */
/* Description: 'GPC0 Output Clock Selection' */
#define GPC0_CONF_SEL_OFFSET 0xF948
#define GPC0_CONF_SEL_SHIFT 0
#define GPC0_CONF_SEL_SIZE  3
/* ----------------------------------------------- */
/* Register: 'GPC1 Configuration Register' */
/* Bit: 'SEL' */
/* Description: 'GPC1 Output Clock Selection' */
#define GPC1_CONF_SEL_OFFSET 0xF94C
#define GPC1_CONF_SEL_SHIFT 0
#define GPC1_CONF_SEL_SIZE  3
/* ----------------------------------------------- */
/* Register: 'SYCCLK Configuration Register' */
/* Bit: 'CLK250_DIV' */
/* Description: 'CLK250_LC Clock Divider Selection' */
#define SYSCLK_CONF_CLK250_DIV_OFFSET 0xF950
#define SYSCLK_CONF_CLK250_DIV_SHIFT 0
#define SYSCLK_CONF_CLK250_DIV_SIZE  8
/* ----------------------------------------------- */
/* Register: 'SGMII Configuration Register' */
/* Bit: 'UNLOCK' */
/* Description: 'SGMII PLL Unlock Stick Bit' */
#define SGMII_CONF_UNLOCK_OFFSET 0xF954
#define SGMII_CONF_UNLOCK_SHIFT 6
#define SGMII_CONF_UNLOCK_SIZE  1
/* Bit: 'UNSYNC' */
/* Description: 'SGMII PMD/PMA Ready' */
#define SGMII_CONF_UNSYNC_OFFSET 0xF954
#define SGMII_CONF_UNSYNC_SHIFT 5
#define SGMII_CONF_UNSYNC_SIZE  1
/* Bit: 'SGMII_LOCK' */
/* Description: 'SGMII PLL Lock' */
#define SGMII_CONF_SGMII_LOCK_OFFSET 0xF954
#define SGMII_CONF_SGMII_LOCK_SHIFT 4
#define SGMII_CONF_SGMII_LOCK_SIZE  1
/* Bit: 'SGMII_SYNC' */
/* Description: 'SGMII PMD/PMA Ready' */
#define SGMII_CONF_SGMII_SYNC_OFFSET 0xF954
#define SGMII_CONF_SGMII_SYNC_SHIFT 3
#define SGMII_CONF_SGMII_SYNC_SIZE  1
/* Bit: 'SGMII_DIS' */
/* Description: 'SGMII Macro Disable' */
#define SGMII_CONF_SGMII_DIS_OFFSET 0xF954
#define SGMII_CONF_SGMII_DIS_SHIFT 0
#define SGMII_CONF_SGMII_DIS_SIZE  1
/* ----------------------------------------------- */
/* Register: 'NCO1 LSB Configuration Register' */
/* Bit: 'NCO1_LSB' */
/* Description: 'NCO1 LSB' */
#define NCO1_LSB_OFFSET 0xF958
#define NCO1_LSB_SHIFT 0
#define NCO1_LSB_SIZE  16
/* ----------------------------------------------- */
/* Register: 'NCO1 MSB Configuration Register' */
/* Bit: 'NCO1_MSB' */
/* Description: 'NCO1 MSB' */
#define NCO1_MSB_OFFSET 0xF95C
#define NCO1_MSB_SHIFT 0
#define NCO1_MSB_SIZE  8
/* ----------------------------------------------- */
/* Register: 'NCO2 LSB Configuration Register' */
/* Bit: 'NCO2 LSB' */
/* Description: 'NCO2 LSB' */
#define NCO2_LSB_OFFSET 0xF960
#define NCO2_LSB_SHIFT 0
#define NCO2_LSB_SIZE  16
/* ----------------------------------------------- */
/* Register: 'NCO2 MSB Configuration Register' */
/* Bit: 'NCO2_MSB' */
/* Description: 'NCO2 MSB' */
#define NCO2_MSB_OFFSET 0xF964
#define NCO2_MSB_SHIFT 0
#define NCO2_MSB_SIZE  8
/* ----------------------------------------------- */
/* Register: 'NCO Control' */
/* Bit: 'SGMII_HSP' */
/* Description: 'SGMII High Speed Selection' */
#define NCO_CTRL_SGMII_HSP_OFFSET 0xF968
#define NCO_CTRL_SGMII_HSP_SHIFT 2
#define NCO_CTRL_SGMII_HSP_SIZE  2
/* Bit: 'SGMIISEL' */
/* Description: 'SGMII Clock NCO Selection' */
#define NCO_CTRL_SGMIISEL_OFFSET 0xF968
#define NCO_CTRL_SGMIISEL_SHIFT 1
#define NCO_CTRL_SGMIISEL_SIZE  1
/* Bit: 'FORCE0' */
/* Description: 'Reset NCO1 and NCO2 counter to 0' */
#define NCO_CTRL_FORCE0_OFFSET 0xF968
#define NCO_CTRL_FORCE0_SHIFT 0
#define NCO_CTRL_FORCE0_SIZE  1
/* ----------------------------------------------- */
/* Register: 'RO PLL Configuration 0 Register' */
/* Bit: 'PLL_K' */
/* Description: 'PLL Fractional K Divider 6:0' */
#define ROPLL_CFG0_PLL_K_OFFSET 0xF980
#define ROPLL_CFG0_PLL_K_SHIFT 9
#define ROPLL_CFG0_PLL_K_SIZE  7
/* Bit: 'PLL_N' */
/* Description: 'PLL N Divider' */
#define ROPLL_CFG0_PLL_N_OFFSET 0xF980
#define ROPLL_CFG0_PLL_N_SHIFT 2
#define ROPLL_CFG0_PLL_N_SIZE  7
/* Bit: 'PLL_L' */
/* Description: 'PLL Lock Status' */
#define ROPLL_CFG0_PLL_L_OFFSET 0xF980
#define ROPLL_CFG0_PLL_L_SHIFT 1
#define ROPLL_CFG0_PLL_L_SIZE  1
/* Bit: 'PLL_RST' */
/* Description: 'PLL Reset' */
#define ROPLL_CFG0_PLL_RST_OFFSET 0xF980
#define ROPLL_CFG0_PLL_RST_SHIFT 0
#define ROPLL_CFG0_PLL_RST_SIZE  1
/* ----------------------------------------------- */
/* Register: 'RO PLL Configuration 1 Register' */
/* Bit: 'PLL_BUFOUT' */
/* Description: 'PLL CML input buffer' */
#define ROPLL_CFG1_PLL_BUFOUT_OFFSET 0xF984
#define ROPLL_CFG1_PLL_BUFOUT_SHIFT 15
#define ROPLL_CFG1_PLL_BUFOUT_SIZE  1
/* Bit: 'PLL_BP' */
/* Description: 'PLL Bypass' */
#define ROPLL_CFG1_PLL_BP_OFFSET 0xF984
#define ROPLL_CFG1_PLL_BP_SHIFT 14
#define ROPLL_CFG1_PLL_BP_SIZE  1
/* Bit: 'PLL_K' */
/* Description: 'PLL0 Fractional K Divider 20 to 7' */
#define ROPLL_CFG1_PLL_K_OFFSET 0xF984
#define ROPLL_CFG1_PLL_K_SHIFT 0
#define ROPLL_CFG1_PLL_K_SIZE  14
/* ----------------------------------------------- */
/* Register: 'RO PLL Configuration Register 2' */
/* Bit: 'PLL_CLK4' */
/* Description: 'PLL Clock Output 4' */
#define ROPLL_CFG2_PLL_CLK4_OFFSET 0xF988
#define ROPLL_CFG2_PLL_CLK4_SHIFT 12
#define ROPLL_CFG2_PLL_CLK4_SIZE  4
/* Bit: 'PLL_CLK3' */
/* Description: 'PLL Clock Output 3' */
#define ROPLL_CFG2_PLL_CLK3_OFFSET 0xF988
#define ROPLL_CFG2_PLL_CLK3_SHIFT 8
#define ROPLL_CFG2_PLL_CLK3_SIZE  4
/* Bit: 'PLL_CLK2' */
/* Description: 'PLL Clock Output 2' */
#define ROPLL_CFG2_PLL_CLK2_OFFSET 0xF988
#define ROPLL_CFG2_PLL_CLK2_SHIFT 4
#define ROPLL_CFG2_PLL_CLK2_SIZE  4
/* Bit: 'PLL_CLK1' */
/* Description: 'PLL Clock Output 1' */
#define ROPLL_CFG2_PLL_CLK1_OFFSET 0xF988
#define ROPLL_CFG2_PLL_CLK1_SHIFT 0
#define ROPLL_CFG2_PLL_CLK1_SIZE  4
/* ----------------------------------------------- */
/* Register: 'RO PLL Configuration Register 3' */
/* Bit: 'PLL_BW' */
/* Description: 'PLL Bandwidth Select' */
#define ROPLL_CFG3_PLL_BW_OFFSET 0xF98C
#define ROPLL_CFG3_PLL_BW_SHIFT 14
#define ROPLL_CFG3_PLL_BW_SIZE  2
/* Bit: 'PLL_INVCLK' */
/* Description: 'PLL Invert Clock Enable' */
#define ROPLL_CFG3_PLL_INVCLK_OFFSET 0xF98C
#define ROPLL_CFG3_PLL_INVCLK_SHIFT 9
#define ROPLL_CFG3_PLL_INVCLK_SIZE  4
/* Bit: 'PLL_SSC' */
/* Description: 'PLL Spread Spectrum Mode' */
#define ROPLL_CFG3_PLL_SSC_OFFSET 0xF98C
#define ROPLL_CFG3_PLL_SSC_SHIFT 8
#define ROPLL_CFG3_PLL_SSC_SIZE  1
/* Bit: 'PLL_N_MODE' */
/* Description: 'Integer N mode En' */
#define ROPLL_CFG3_PLL_N_MODE_OFFSET 0xF98C
#define ROPLL_CFG3_PLL_N_MODE_SHIFT 7
#define ROPLL_CFG3_PLL_N_MODE_SIZE  1
/* Bit: 'PLL_OPD5' */
/* Description: 'PLL CLK5 output buffer power down' */
#define ROPLL_CFG3_PLL_OPD5_OFFSET 0xF98C
#define ROPLL_CFG3_PLL_OPD5_SHIFT 6
#define ROPLL_CFG3_PLL_OPD5_SIZE  1
/* Bit: 'PLL_OPD4' */
/* Description: 'PLL CLK4 output buffer power down' */
#define ROPLL_CFG3_PLL_OPD4_OFFSET 0xF98C
#define ROPLL_CFG3_PLL_OPD4_SHIFT 5
#define ROPLL_CFG3_PLL_OPD4_SIZE  1
/* Bit: 'PLL_OPD3' */
/* Description: 'PLL CLK3 output buffer power down' */
#define ROPLL_CFG3_PLL_OPD3_OFFSET 0xF98C
#define ROPLL_CFG3_PLL_OPD3_SHIFT 4
#define ROPLL_CFG3_PLL_OPD3_SIZE  1
/* Bit: 'PLL_OPD2' */
/* Description: 'PLL CLK2 output buffer power down' */
#define ROPLL_CFG3_PLL_OPD2_OFFSET 0xF98C
#define ROPLL_CFG3_PLL_OPD2_SHIFT 3
#define ROPLL_CFG3_PLL_OPD2_SIZE  1
/* Bit: 'PLL_OPD1' */
/* Description: 'PLL CLK1 output buffer power down' */
#define ROPLL_CFG3_PLL_OPD1_OFFSET 0xF98C
#define ROPLL_CFG3_PLL_OPD1_SHIFT 2
#define ROPLL_CFG3_PLL_OPD1_SIZE  1
/* Bit: 'PLL_CLK5' */
/* Description: 'PLL CML CLK5 output buffer frequency selection' */
#define ROPLL_CFG3_PLL_CLK5_OFFSET 0xF98C
#define ROPLL_CFG3_PLL_CLK5_SHIFT 0
#define ROPLL_CFG3_PLL_CLK5_SIZE  2
/* ----------------------------------------------- */
/* Register: 'RO PLL Miscellaneous Control Register' */
/* Bit: 'PSOVR' */
/* Description: 'Pinstrap overwrite' */
#define ROPLL_MISC_PSOVR_OFFSET 0xF990
#define ROPLL_MISC_PSOVR_SHIFT 15
#define ROPLL_MISC_PSOVR_SIZE  1
/* Bit: 'UNLCK' */
/* Description: 'Sticky bit for unlock status' */
#define ROPLL_MISC_UNLCK_OFFSET 0xF990
#define ROPLL_MISC_UNLCK_SHIFT 14
#define ROPLL_MISC_UNLCK_SIZE  1
/* Bit: 'FORCE' */
/* Description: 'Force Latching Of Shadow Registers' */
#define ROPLL_MISC_FORCE_OFFSET 0xF990
#define ROPLL_MISC_FORCE_SHIFT 13
#define ROPLL_MISC_FORCE_SIZE  1
/* Bit: 'VEXT' */
/* Description: 'PLL output buffer power supply' */
#define ROPLL_MISC_VEXT_OFFSET 0xF990
#define ROPLL_MISC_VEXT_SHIFT 12
#define ROPLL_MISC_VEXT_SIZE  1
/* Bit: 'EXTREF' */
/* Description: 'Select External Reference Current' */
#define ROPLL_MISC_EXTREF_OFFSET 0xF990
#define ROPLL_MISC_EXTREF_SHIFT 11
#define ROPLL_MISC_EXTREF_SIZE  1
/* Bit: 'LCKOVR' */
/* Description: 'PLL Lock Overwrite' */
#define ROPLL_MISC_LCKOVR_OFFSET 0xF990
#define ROPLL_MISC_LCKOVR_SHIFT 10
#define ROPLL_MISC_LCKOVR_SIZE  1
/* Bit: 'IPOK' */
/* Description: 'Internal POK Override' */
#define ROPLL_MISC_IPOK_OFFSET 0xF990
#define ROPLL_MISC_IPOK_SHIFT 9
#define ROPLL_MISC_IPOK_SIZE  1
/* Bit: 'IOPFSEL' */
/* Description: 'Selects if PLL internal digital
	allocates the default output frequencies.' */
#define ROPLL_MISC_IOPFSEL_OFFSET 0xF990
#define ROPLL_MISC_IOPFSEL_SHIFT 8
#define ROPLL_MISC_IOPFSEL_SIZE  1
/* Bit: 'INKSEL' */
/* Description: 'Selects if PLL internal mapped
	N,K or based on PLL_CFG0/1 N,K' */
#define ROPLL_MISC_INKSEL_OFFSET 0xF990
#define ROPLL_MISC_INKSEL_SHIFT 7
#define ROPLL_MISC_INKSEL_SIZE  1
/* Bit: 'FPUP' */
/* Description: 'Force Power up of all Divider chains' */
#define ROPLL_MISC_FPUP_OFFSET 0xF990
#define ROPLL_MISC_FPUP_SHIFT 6
#define ROPLL_MISC_FPUP_SIZE  1
/* Bit: 'CLKSEL' */
/* Description: 'PLL input clock select' */
#define ROPLL_MISC_CLKSEL_OFFSET 0xF990
#define ROPLL_MISC_CLKSEL_SHIFT 4
#define ROPLL_MISC_CLKSEL_SIZE  2
/* Bit: 'MODE' */
/* Description: 'Selects CML/CMOS input Clock' */
#define ROPLL_MISC_MODE_OFFSET 0xF990
#define ROPLL_MISC_MODE_SHIFT 3
#define ROPLL_MISC_MODE_SIZE  1
/* Bit: 'MPROG' */
/* Description: 'PLL Mode Selection' */
#define ROPLL_MISC_MPROG_OFFSET 0xF990
#define ROPLL_MISC_MPROG_SHIFT 0
#define ROPLL_MISC_MPROG_SIZE  2
/* ----------------------------------------------- */

/* ----------------------------------------------- */
/* Reset Control Unit Registers*/
/* ----------------------------------------------- */
/* Register: 'Reset Status Register' */
/* Bit: 'HRST' */
/* Description: 'Hardware Reset Cause Flag' */
#define RESET_STATUS_HRST_OFFSET 0xFA00
#define RESET_STATUS_HRST_SHIFT 15
#define RESET_STATUS_HRST_SIZE  1
/* Bit: 'RECORD' */
/* Description: 'Last Reset Record' */
#define RESET_STATUS_RECORD_OFFSET 0xFA00
#define RESET_STATUS_RECORD_SHIFT 1
#define RESET_STATUS_RECORD_SIZE  14
/* Bit: 'INIT' */
/* Description: 'Initialization Done Flag' */
#define RESET_STATUS_INIT_OFFSET 0xFA00
#define RESET_STATUS_INIT_SHIFT 0
#define RESET_STATUS_INIT_SIZE  1
/* ----------------------------------------------- */
/* Register: 'Reset Status Register' */
/* Bit: 'SRST' */
/* Description: 'Enable Global Software Reset' */
#define RST_REQ_SRST_OFFSET 0xFA01
#define RST_REQ_SRST_SHIFT 15
#define RST_REQ_SRST_SIZE  1
/* Bit: 'RD14' */
/* Description: 'Reset Request for Reset Domain RD14' */
#define RST_REQ_RD14_OFFSET 0xFA01
#define RST_REQ_RD14_SHIFT 14
#define RST_REQ_RD14_SIZE  1
/* Bit: 'G0RST' */
/* Description: 'Enable GPHY0 Reset' */
#define RST_REQ_G0RST_OFFSET 0xFA01
#define RST_REQ_G0RST_SHIFT 12
#define RST_REQ_G0RST_SIZE  1
/* Bit: 'RD10' */
/* Description: 'Reset Request for Reset Domain RD10' */
#define RST_REQ_RD10_OFFSET 0xFA01
#define RST_REQ_RD10_SHIFT 10
#define RST_REQ_RD10_SIZE  1
/* Bit: 'RD9' */
/* Description: 'Reset Request for Reset Domain RD9' */
#define RST_REQ_RD9_OFFSET 0xFA01
#define RST_REQ_RD9_SHIFT 9
#define RST_REQ_RD9_SIZE  1
/* Bit: 'RD7' */
/* Description: 'Reset Request for Reset Domain RD7' */
#define RST_REQ_RD7_OFFSET 0xFA01
#define RST_REQ_RD7_SHIFT 7
#define RST_REQ_RD7_SIZE  1
/* Bit: 'RD6' */
/* Description: 'Reset Request for Reset Domain RD6' */
#define RST_REQ_RD6_OFFSET 0xFA01
#define RST_REQ_RD6_SHIFT 6
#define RST_REQ_RD6_SIZE  1
/* Bit: 'RD5' */
/* Description: 'Reset Request for Reset Domain RD5' */
#define RST_REQ_RD5_OFFSET 0xFA01
#define RST_REQ_RD5_SHIFT 5
#define RST_REQ_RD5_SIZE  1
/* Bit: 'RD4' */
/* Description: 'Reset Request for Reset Domain RD4' */
#define RST_REQ_RD4_OFFSET 0xFA01
#define RST_REQ_RD4_SHIFT 4
#define RST_REQ_RD4_SIZE  1
/* Bit: 'RD3' */
/* Description: 'Reset Request for Reset Domain RD3' */
#define RST_REQ_RD3_OFFSET 0xFA01
#define RST_REQ_RD3_SHIFT 3
#define RST_REQ_RD3_SIZE  1
/* Bit: 'RD2' */
/* Description: 'Reset Request for Reset Domain RD2' */
#define RST_REQ_RD2_OFFSET 0xFA01
#define RST_REQ_RD2_SHIFT 2
#define RST_REQ_RD2_SIZE  1
/* Bit: 'RD1' */
/* Description: 'Reset Request for Reset Domain RD1' */
#define RST_REQ_RD1_OFFSET 0xFA01
#define RST_REQ_RD1_SHIFT 1
#define RST_REQ_RD1_SIZE  1
/* Bit: 'RD0' */
/* Description: 'Reset Request for Reset Domain RD0' */
#define RST_REQ_RD0_OFFSET 0xFA01
#define RST_REQ_RD0_SHIFT 0
#define RST_REQ_RD0_SIZE  1
/* ----------------------------------------------- */
/* Register: 'MANU ID Register' */
/* Bit: 'PNUML' */
/* Description: 'Part Number LSB' */
#define MANU_ID_PNUML_OFFSET 0xFA10
#define MANU_ID_PNUML_SHIFT 12
#define MANU_ID_PNUML_SIZE  4
/* Bit: 'MANID' */
/* Description: 'Manufacturer ID' */
#define MANU_ID_MANID_OFFSET 0xFA10
#define MANU_ID_MANID_SHIFT 1
#define MANU_ID_MANID_SIZE  11
/* Bit: 'FIX1' */
/* Description: 'Fixed to 1' */
#define MANU_ID_FIX1_OFFSET 0xFA10
#define MANU_ID_FIX1_SHIFT 0
#define MANU_ID_FIX1_SIZE  1
/* ----------------------------------------------- */
/* Register: 'PNUM ID Register' */
/* Bit: 'VER' */
/* Description: 'Chip Version' */
#define PNUM_ID_VER_OFFSET 0xFA11
#define PNUM_ID_VER_SHIFT 12
#define PNUM_ID_VER_SIZE  4
/* Bit: 'PNUMM' */
/* Description: 'Part Number MSB' */
#define PNUM_ID_PNUMM_OFFSET 0xFA11
#define PNUM_ID_PNUMM_SHIFT 0
#define PNUM_ID_PNUMM_SIZE  12
/* ----------------------------------------------- */
/* Register: 'GPIO PAD Driver Strength 0 Control Register' */
/* Bit: 'GPIO' */
/* Description: 'GPIO PAD Drive Strength Bit 0' */
#define GPIO_DRIVE0_CFG_GPIO_OFFSET 0xFA70
#define GPIO_DRIVE0_CFG_GPIO_SHIFT 0
#define GPIO_DRIVE0_CFG_GPIO_SIZE  14
/* ----------------------------------------------- */
/* Register: 'GPIO PAD Driver Strength 1 Control Register' */
/* Bit: 'GPIO' */
/* Description: 'GPIO PAD Drive Strength Bit 1' */
#define GPIO_DRIVE1_CFG_GPIO_OFFSET 0xFA71
#define GPIO_DRIVE1_CFG_GPIO_SHIFT 0
#define GPIO_DRIVE1_CFG_GPIO_SIZE  14
/* ----------------------------------------------- */
/* Register: 'GPIO PAD Slew Control Register' */
/* Bit: 'PAD_VOL' */
/* Description: 'GPIO1 PAD Voltage Supply Level' */
#define GPIO_SLEW_CFG_PAD_VOL_OFFSET 0xFA72
#define GPIO_SLEW_CFG_PAD_VOL_SHIFT 15
#define GPIO_SLEW_CFG_PAD_VOL_SIZE  1
/* Bit: 'GPIO' */
/* Description: 'GPIO PAD Slew Control' */
#define GPIO_SLEW_CFG_GPIO_OFFSET 0xFA72
#define GPIO_SLEW_CFG_GPIO_SHIFT 0
#define GPIO_SLEW_CFG_GPIO_SIZE  14
/* ----------------------------------------------- */
/* Register: 'GPIO2 PAD Driver Strength 0 Control Register' */
/* Bit: 'GPIO2' */
/* Description: 'GPIO2 PAD Drive Strength Bit 0' */
#define GPIO2_DRIVE0_CFG_GPIO2_OFFSET 0xFA74
#define GPIO2_DRIVE0_CFG_GPIO2_SHIFT 0
#define GPIO2_DRIVE0_CFG_GPIO2_SIZE  15
/* ----------------------------------------------- */
/* Register: 'GPIO2 PAD Driver Strength 1 Control Register' */
/* Bit: 'GPIO2' */
/* Description: 'GPIO2 PAD Drive Strength Bit 1' */
#define GPIO2_DRIVE1_CFG_GPIO2_OFFSET 0xFA75
#define GPIO2_DRIVE1_CFG_GPIO2_SHIFT 0
#define GPIO2_DRIVE1_CFG_GPIO2_SIZE  15
/* ----------------------------------------------- */
/* Register: 'GPIO 2 PAD Slew Control Register' */
/* Bit: 'PAD_VOL' */
/* Description: 'GPIO2 PAD Voltage Supply Level' */
#define GPIO2_SLEW_CFG_PAD_VOL_OFFSET 0xFA76
#define GPIO2_SLEW_CFG_PAD_VOL_SHIFT 15
#define GPIO2_SLEW_CFG_PAD_VOL_SIZE  1
/* Bit: 'GPIO2' */
/* Description: 'GPIO PAD Slew Control' */
#define GPIO2_SLEW_CFG_GPIO2_OFFSET 0xFA76
#define GPIO2_SLEW_CFG_GPIO2_SHIFT 0
#define GPIO2_SLEW_CFG_GPIO2_SIZE  15
/* ----------------------------------------------- */
/* Register: 'RGMII PAD Slew Control Register' */
/* Bit: 'PAD_TX_PU' */
/* Description: 'RGMII TX PAD Pull Up' */
#define RGMII_SLEW_CFG_PAD_TX_PU_OFFSET 0xFA78
#define RGMII_SLEW_CFG_PAD_TX_PU_SHIFT 11
#define RGMII_SLEW_CFG_PAD_TX_PU_SIZE  1
/* Bit: 'PAD_TX_PD' */
/* Description: 'RGMII TX PAD Pull Down' */
#define RGMII_SLEW_CFG_PAD_TX_PD_OFFSET 0xFA78
#define RGMII_SLEW_CFG_PAD_TX_PD_SHIFT 10
#define RGMII_SLEW_CFG_PAD_TX_PD_SIZE  1
/* Bit: 'PAD_RX_PU' */
/* Description: 'RGMII RX PAD Pull Up' */
#define RGMII_SLEW_CFG_PAD_RX_PU_OFFSET 0xFA78
#define RGMII_SLEW_CFG_PAD_RX_PU_SHIFT 9
#define RGMII_SLEW_CFG_PAD_RX_PU_SIZE  1
/* Bit: 'PAD_RX_PD' */
/* Description: 'RGMII RX PAD Pull Down' */
#define RGMII_SLEW_CFG_PAD_RX_PD_OFFSET 0xFA78
#define RGMII_SLEW_CFG_PAD_RX_PD_SHIFT 8
#define RGMII_SLEW_CFG_PAD_RX_PD_SIZE  1
/* Bit: 'PAD_VOL_TX' */
/* Description: 'RGMII TX PAD Voltage Supply Level' */
#define RGMII_SLEW_CFG_PAD_VOL_TX_OFFSET 0xFA78
#define RGMII_SLEW_CFG_PAD_VOL_TX_SHIFT 5
#define RGMII_SLEW_CFG_PAD_VOL_TX_SIZE  1
/* Bit: 'PAD_VOL_RX' */
/* Description: 'RGMII RX PAD Voltage Supply Level' */
#define RGMII_SLEW_CFG_PAD_VOL_RX_OFFSET 0xFA78
#define RGMII_SLEW_CFG_PAD_VOL_RX_SHIFT 4
#define RGMII_SLEW_CFG_PAD_VOL_RX_SIZE  1
/* Bit: 'DRV_TXD' */
/* Description: 'RGMII TX Non-Clock PAD Slew Rate' */
#define RGMII_SLEW_CFG_DRV_TXD_OFFSET 0xFA78
#define RGMII_SLEW_CFG_DRV_TXD_SHIFT 3
#define RGMII_SLEW_CFG_DRV_TXD_SIZE  1
/* Bit: 'DRV_TXC' */
/* Description: 'RGMII TX Clock Slew Rate' */
#define RGMII_SLEW_CFG_DRV_TXC_OFFSET 0xFA78
#define RGMII_SLEW_CFG_DRV_TXC_SHIFT 2
#define RGMII_SLEW_CFG_DRV_TXC_SIZE  1
/* Bit: 'DRV_RXD' */
/* Description: 'RGMII RX Non-Clock PAD Slew Rate' */
#define RGMII_SLEW_CFG_DRV_RXD_OFFSET 0xFA78
#define RGMII_SLEW_CFG_DRV_RXD_SHIFT 1
#define RGMII_SLEW_CFG_DRV_RXD_SIZE  1
/* Bit: 'DRV_RXC' */
/* Description: 'RGMII RX Clock Slew Rate' */
#define RGMII_SLEW_CFG_DRV_RXC_OFFSET 0xFA78
#define RGMII_SLEW_CFG_DRV_RXC_SHIFT 0
#define RGMII_SLEW_CFG_DRV_RXC_SIZE  1
/* ----------------------------------------------- */
/* Register: 'Pin Strapping Register' */
/* Bit: 'PS' */
/* Description: 'Pin Strapping of GPIO0 to GPIO13' */
#define PS0_PS_OFFSET 0xFA80
#define PS0_PS_SHIFT 0
#define PS0_PS_SIZE  14
/* ----------------------------------------------- */
/* Register: 'Pin Strapping Register 1' */
/* Bit: 'PS' */
/* Description: 'Pin Strapping of GPIO0 to GPIO13' */
#define PS1_PS_OFFSET 0xFA81
#define PS1_PS_SHIFT 0
#define PS1_PS_SIZE  15
/* ----------------------------------------------- */

/* ----------------------------------------------- */
/* GPIO Registers */
/* ----------------------------------------------- */
/* Register: 'GPIO Data Output Register' */
/* Bit: 'GPIO' */
/* Description: 'GPIO Output Value' */
#define GPIO_OUT_GPIO_OFFSET 0xF380
#define GPIO_OUT_GPIO_SHIFT 0
#define GPIO_OUT_GPIO_SIZE  14
/* ----------------------------------------------- */
/* Register: 'GPIO Data Input Register' */
/* Bit: 'GPIO' */
/* Description: 'GPIO Input Value' */
#define GPIO_IN_GPIO_OFFSET 0xF381
#define GPIO_IN_GPIO_SHIFT 0
#define GPIO_IN_GPIO_SIZE  14
/* ----------------------------------------------- */
/* Register: 'GPIO Direction Register' */
/* Bit: 'GPIO' */
/* Description: 'GPIO Direction Control' */
#define GPIO_DIR_GPIO_OFFSET 0xF382
#define GPIO_DIR_GPIO_SHIFT 0
#define GPIO_DIR_GPIO_SIZE  14
/* ----------------------------------------------- */
/* Register: 'Port 0 Alternate Function Select Register 0' */
/* Bit: 'GPIO' */
/* Description: 'GPIO Alternate Function Selection LSB' */
#define GPIO_ALTSEL0_GPIO_OFFSET 0xF383
#define GPIO_ALTSEL0_GPIO_SHIFT 0
#define GPIO_ALTSEL0_GPIO_SIZE  14
/* ----------------------------------------------- */
/* Register: 'Port 0 Alternate Function Select Register 1' */
/* Bit: 'GPIO' */
/* Description: 'GPIO Alternate Function Selection MSB' */
#define GPIO_ALTSEL1_GPIO_OFFSET 0xF384
#define GPIO_ALTSEL1_GPIO_SHIFT 0
#define GPIO_ALTSEL1_GPIO_SIZE  14
/* ----------------------------------------------- */
/* Register: 'GPIO Open Drain Control Register' */
/* Bit: 'GPIO' */
/* Description: 'GPIO Open Drain Mode' */
#define GPIO_OD_GPIO_OFFSET 0xF385
#define GPIO_OD_GPIO_SHIFT 0
#define GPIO_OD_GPIO_SIZE  14
/* ----------------------------------------------- */
/* Register: 'GPIO Pull-Up/Pull-Down Select Register' */
/* Bit: 'GPIO' */
/* Description: 'GPIO Pull Up/Down Mode' */
#define GPIO_PUDSEL_GPIO_OFFSET 0xF386
#define GPIO_PUDSEL_GPIO_SHIFT 0
#define GPIO_PUDSEL_GPIO_SIZE  14
/* ----------------------------------------------- */
/* Register: 'GPIO Pull-Up/Pull-Down Enable Register' */
/* Bit: 'GPIO' */
/* Description: 'GPIO Pull Up/Down Enable' */
#define GPIO_PUDEN_GPIO_OFFSET 0xF387
#define GPIO_PUDEN_GPIO_SHIFT 0
#define GPIO_PUDEN_GPIO_SIZE  14
/* ----------------------------------------------- */
/* Register: 'GPIO2 Data Output Register' */
/* Bit: 'GPIO' */
/* Description: 'GPIO Output Value' */
#define GPIO2_OUT_GPIO_OFFSET 0xF390
#define GPIO2_OUT_GPIO_SHIFT 0
#define GPIO2_OUT_GPIO_SIZE  15
/* ----------------------------------------------- */
/* Register: 'GPIO2 Data Input Register' */
/* Bit: 'GPIO' */
/* Description: 'GPIO Input Value' */
#define GPIO2_IN_GPIO_OFFSET 0xF391
#define GPIO2_IN_GPIO_SHIFT 0
#define GPIO2_IN_GPIO_SIZE  15
/* ----------------------------------------------- */
/* Register: 'GPIO2 Direction Register' */
/* Bit: 'GPIO' */
/* Description: 'GPIO Direction Control' */
#define GPIO2_DIR_GPIO_OFFSET 0xF392
#define GPIO2_DIR_GPIO_SHIFT 0
#define GPIO2_DIR_GPIO_SIZE  15
/* ----------------------------------------------- */
/* Register: 'Port 2 Alternate Function Select Register 0' */
/* Bit: 'GPIO' */
/* Description: 'GPIO Alternate Function Selection LSB' */
#define GPIO2_ALTSEL0_GPIO_OFFSET 0xF393
#define GPIO2_ALTSEL0_GPIO_SHIFT 0
#define GPIO2_ALTSEL0_GPIO_SIZE  15
/* ----------------------------------------------- */
/* Register: 'Port 2 Alternate Function Select Register 1' */
/* Bit: 'GPIO' */
/* Description: 'GPIO Alternate Function Selection MSB' */
#define GPIO2_ALTSEL1_GPIO_OFFSET 0xF394
#define GPIO2_ALTSEL1_GPIO_SHIFT 0
#define GPIO2_ALTSEL1_GPIO_SIZE  15
/* ----------------------------------------------- */
/* Register: 'GPIO2 Open Drain Control Register' */
/* Bit: 'GPIO' */
/* Description: 'GPIO Open Drain Mode' */
#define GPIO2_OD_GPIO_OFFSET 0xF395
#define GPIO2_OD_GPIO_SHIFT 0
#define GPIO2_OD_GPIO_SIZE  15
/* ----------------------------------------------- */
/* Register: 'GPIO2 Pull-Up/Pull-Down Select Register' */
/* Bit: 'GPIO' */
/* Description: 'GPIO Pull Up/Down Mode' */
#define GPIO2_PUDSEL_GPIO_OFFSET 0xF396
#define GPIO2_PUDSEL_GPIO_SHIFT 0
#define GPIO2_PUDSEL_GPIO_SIZE  15
/* ----------------------------------------------- */
/* Register: 'GPIO2 Pull-Up/Pull-Down Enable Register' */
/* Bit: 'GPIO' */
/* Description: 'GPIO Pull Up/Down Enable' */
#define GPIO2_PUDEN_GPIO_OFFSET 0xF397
#define GPIO2_PUDEN_GPIO_SHIFT 0
#define GPIO2_PUDEN_GPIO_SIZE  15
/* ----------------------------------------------- */
/* ----------------------------------------------- */
/* ICU Registers */
/* ----------------------------------------------- */
/* Register: 'IM0 Interrupt Status Register' */
/* Bit: 'IR11' */
/* Description: 'Status of Interrupt Request SGMII' */
#define IM0_ISR_IR11_OFFSET 0xF3C0
#define IM0_ISR_IR11_SHIFT 11
#define IM0_ISR_IR11_SIZE  1
/* Bit: 'IR10' */
/* Description: 'Status of Interrupt Request Packet Extraction' */
#define IM0_ISR_IR10_OFFSET 0xF3C0
#define IM0_ISR_IR10_SHIFT 10
#define IM0_ISR_IR10_SIZE  1
/* Bit: 'IR7' */
/* Description: 'Status of Interrupt Request MGE' */
#define IM0_ISR_IR7_OFFSET 0xF3C0
#define IM0_ISR_IR7_SHIFT 7
#define IM0_ISR_IR7_SIZE  1
/* Bit: 'IR6' */
/* Description: 'Status of Interrupt Request xMII' */
#define IM0_ISR_IR6_OFFSET 0xF3C0
#define IM0_ISR_IR6_SHIFT 6
#define IM0_ISR_IR6_SIZE  1
/* Bit: 'IR5' */
/* Description: 'Status of Interrupt Request GSWIP' */
#define IM0_ISR_IR5_OFFSET 0xF3C0
#define IM0_ISR_IR5_SHIFT 5
#define IM0_ISR_IR5_SIZE  1
/* Bit: 'IR4' */
/* Description: 'Status of Interrupt Request PHY4' */
#define IM0_ISR_IR4_OFFSET 0xF3C0
#define IM0_ISR_IR4_SHIFT 4
#define IM0_ISR_IR4_SIZE  1
/* Bit: 'IR3' */
/* Description: 'Status of Interrupt Request PHY3' */
#define IM0_ISR_IR3_OFFSET 0xF3C0
#define IM0_ISR_IR3_SHIFT 3
#define IM0_ISR_IR3_SIZE  1
/* Bit: 'IR2' */
/* Description: 'Status of Interrupt Request PHY2' */
#define IM0_ISR_IR2_OFFSET 0xF3C0
#define IM0_ISR_IR2_SHIFT 2
#define IM0_ISR_IR2_SIZE  1
/* Bit: 'IR1' */
/* Description: 'Status of Interrupt Request PHY1' */
#define IM0_ISR_IR1_OFFSET 0xF3C0
#define IM0_ISR_IR1_SHIFT 1
#define IM0_ISR_IR1_SIZE  1
/* Bit: 'IR0' */
/* Description: 'Status of Interrupt Request PHY0' */
#define IM0_ISR_IR0_OFFSET 0xF3C0
#define IM0_ISR_IR0_SHIFT 0
#define IM0_ISR_IR0_SIZE  1
/* ----------------------------------------------- */
/* Register: 'IM0 EINT0 Interrupt Enable Register' */
/* Bit: 'IR11' */
/* Description: 'Interrupt Enable SGMII' */
#define IM0_EINT0_IER_IR11_OFFSET 0xF3C2
#define IM0_EINT0_IER_IR11_SHIFT 11
#define IM0_EINT0_IER_IR11_SIZE  1
/* Bit: 'IR10' */
/* Description: 'Interrupt Enable Packet Extraction' */
#define IM0_EINT0_IER_IR10_OFFSET 0xF3C2
#define IM0_EINT0_IER_IR10_SHIFT 10
#define IM0_EINT0_IER_IR10_SIZE  1
/* Bit: 'IR7' */
/* Description: 'Interrupt Enable MGE' */
#define IM0_EINT0_IER_IR7_OFFSET 0xF3C2
#define IM0_EINT0_IER_IR7_SHIFT 7
#define IM0_EINT0_IER_IR7_SIZE  1
/* Bit: 'IR6' */
/* Description: 'Interrupt Enable xMII' */
#define IM0_EINT0_IER_IR6_OFFSET 0xF3C2
#define IM0_EINT0_IER_IR6_SHIFT 6
#define IM0_EINT0_IER_IR6_SIZE  1
/* Bit: 'IR5' */
/* Description: 'Interrupt Enable GSWIP' */
#define IM0_EINT0_IER_IR5_OFFSET 0xF3C2
#define IM0_EINT0_IER_IR5_SHIFT 5
#define IM0_EINT0_IER_IR5_SIZE  1
/* Bit: 'IR4' */
/* Description: 'Interrupt Enable PHY4' */
#define IM0_EINT0_IER_IR4_OFFSET 0xF3C2
#define IM0_EINT0_IER_IR4_SHIFT 4
#define IM0_EINT0_IER_IR4_SIZE  1
/* Bit: 'IR3' */
/* Description: 'Interrupt Enable PHY3' */
#define IM0_EINT0_IER_IR3_OFFSET 0xF3C2
#define IM0_EINT0_IER_IR3_SHIFT 3
#define IM0_EINT0_IER_IR3_SIZE  1
/* Bit: 'IR2' */
/* Description: 'Interrupt Enable PHY2' */
#define IM0_EINT0_IER_IR2_OFFSET 0xF3C2
#define IM0_EINT0_IER_IR2_SHIFT 2
#define IM0_EINT0_IER_IR2_SIZE  1
/* Bit: 'IR1' */
/* Description: 'Interrupt Enable PHY1' */
#define IM0_EINT0_IER_IR1_OFFSET 0xF3C2
#define IM0_EINT0_IER_IR1_SHIFT 1
#define IM0_EINT0_IER_IR1_SIZE  1
/* Bit: 'IR0' */
/* Description: 'Interrupt Enable PHY0' */
#define IM0_EINT0_IER_IR0_OFFSET 0xF3C2
#define IM0_EINT0_IER_IR0_SHIFT 0
#define IM0_EINT0_IER_IR0_SIZE  1
/* ----------------------------------------------- */
/* Register: 'IM0 EINT1 Interrupt Enable Register' */
/* Bit: 'IR11' */
/* Description: 'Interrupt Enable SGMII' */
#define IM0_EINT1_IER_IR11_OFFSET 0xF3C3
#define IM0_EINT1_IER_IR11_SHIFT 11
#define IM0_EINT1_IER_IR11_SIZE  1
/* Bit: 'IR10' */
/* Description: 'Interrupt Enable Packet Extraction' */
#define IM0_EINT1_IER_IR10_OFFSET 0xF3C3
#define IM0_EINT1_IER_IR10_SHIFT 10
#define IM0_EINT1_IER_IR10_SIZE  1
/* Bit: 'IR7' */
/* Description: 'Interrupt Enable MGE' */
#define IM0_EINT1_IER_IR7_OFFSET 0xF3C3
#define IM0_EINT1_IER_IR7_SHIFT 7
#define IM0_EINT1_IER_IR7_SIZE  1
/* Bit: 'IR6' */
/* Description: 'Interrupt Enable xMII' */
#define IM0_EINT1_IER_IR6_OFFSET 0xF3C3
#define IM0_EINT1_IER_IR6_SHIFT 6
#define IM0_EINT1_IER_IR6_SIZE  1
/* Bit: 'IR5' */
/* Description: 'Interrupt Enable GSWIP' */
#define IM0_EINT1_IER_IR5_OFFSET 0xF3C3
#define IM0_EINT1_IER_IR5_SHIFT 5
#define IM0_EINT1_IER_IR5_SIZE  1
/* Bit: 'IR4' */
/* Description: 'Interrupt Enable PHY4' */
#define IM0_EINT1_IER_IR4_OFFSET 0xF3C3
#define IM0_EINT1_IER_IR4_SHIFT 4
#define IM0_EINT1_IER_IR4_SIZE  1
/* Bit: 'IR3' */
/* Description: 'Interrupt Enable PHY3' */
#define IM0_EINT1_IER_IR3_OFFSET 0xF3C3
#define IM0_EINT1_IER_IR3_SHIFT 3
#define IM0_EINT1_IER_IR3_SIZE  1
/* Bit: 'IR2' */
/* Description: 'Interrupt Enable PHY2' */
#define IM0_EINT1_IER_IR2_OFFSET 0xF3C3
#define IM0_EINT1_IER_IR2_SHIFT 2
#define IM0_EINT1_IER_IR2_SIZE  1
/* Bit: 'IR1' */
/* Description: 'Interrupt Enable PHY1' */
#define IM0_EINT1_IER_IR1_OFFSET 0xF3C3
#define IM0_EINT1_IER_IR1_SHIFT 1
#define IM0_EINT1_IER_IR1_SIZE  1
/* Bit: 'IR0' */
/* Description: 'Interrupt Enable PHY0' */
#define IM0_EINT1_IER_IR0_OFFSET 0xF3C3
#define IM0_EINT1_IER_IR0_SHIFT 0
#define IM0_EINT1_IER_IR0_SIZE  1
/* ----------------------------------------------- */
/* Register: 'EIU External Interrupt Controller Register' */
/* Bit: 'EOUT1' */
/* Description: 'External Interrupt Output EINT1' */
#define EIU_EXIN_CONF_EOUT1_OFFSET 0xF3C4
#define EIU_EXIN_CONF_EOUT1_SHIFT 9
#define EIU_EXIN_CONF_EOUT1_SIZE  1
/* Bit: 'EOUT0' */
/* Description: 'External Interrupt Output EINT0' */
#define EIU_EXIN_CONF_EOUT0_OFFSET 0xF3C4
#define EIU_EXIN_CONF_EOUT0_SHIFT 9
#define EIU_EXIN_CONF_EOUT0_SIZE  1
/* Bit: 'EIN1' */
/* Description: 'External Interrupt Input EINT1' */
#define EIU_EXIN_CONF_EIN1_OFFSET 0xF3C4
#define EIU_EXIN_CONF_EIN1_SHIFT 9
#define EIU_EXIN_CONF_EIN1_SIZE  1
/* Bit: 'EIN0' */
/* Description: 'External Interrupt Input EINT0' */
#define EIU_EXIN_CONF_EIN0_OFFSET 0xF3C4
#define EIU_EXIN_CONF_EIN0_SHIFT 9
#define EIU_EXIN_CONF_EIN0_SIZE  1
/* ----------------------------------------------- */

/* ----------------------------------------------- */
/* LED Registers */
/* ----------------------------------------------- */
/* Register: 'LED Single Color LED Mode Register' */
/* Bit: 'LED14' */
/* Description: 'LED Single Color Mode' */
#define LED_MD_CFG_LED14_OFFSET 0xF3E0
#define LED_MD_CFG_LED14_SHIFT 14
#define LED_MD_CFG_LED14_SIZE  1
/* Bit: 'LED13' */
/* Description: 'LED Single Color Mode' */
#define LED_MD_CFG_LED13_OFFSET 0xF3E0
#define LED_MD_CFG_LED13_SHIFT 13
#define LED_MD_CFG_LED13_SIZE  1
/* Bit: 'LED12' */
/* Description: 'LED Single Color Mode' */
#define LED_MD_CFG_LED12_OFFSET 0xF3E0
#define LED_MD_CFG_LED12_SHIFT 12
#define LED_MD_CFG_LED12_SIZE  1
/* Bit: 'LED11' */
/* Description: 'LED Single Color Mode' */
#define LED_MD_CFG_LED11_OFFSET 0xF3E0
#define LED_MD_CFG_LED11_SHIFT 11
#define LED_MD_CFG_LED11_SIZE  1
/* Bit: 'LED10' */
/* Description: 'LED Single Color Mode' */
#define LED_MD_CFG_LED10_OFFSET 0xF3E0
#define LED_MD_CFG_LED10_SHIFT 10
#define LED_MD_CFG_LED10_SIZE  1
/* Bit: 'LED9' */
/* Description: 'LED Single Color Mode' */
#define LED_MD_CFG_LED9_OFFSET 0xF3E0
#define LED_MD_CFG_LED9_SHIFT 9
#define LED_MD_CFG_LED9_SIZE  1
/* Bit: 'LED8' */
/* Description: 'LED Single Color Mode' */
#define LED_MD_CFG_LED8_OFFSET 0xF3E0
#define LED_MD_CFG_LED8_SHIFT 8
#define LED_MD_CFG_LED8_SIZE  1
/* Bit: 'LED7' */
/* Description: 'LED Single Color Mode' */
#define LED_MD_CFG_LED7_OFFSET 0xF3E0
#define LED_MD_CFG_LED7_SHIFT 7
#define LED_MD_CFG_LED7_SIZE  1
/* Bit: 'LED6' */
/* Description: 'LED Single Color Mode' */
#define LED_MD_CFG_LED6_OFFSET 0xF3E0
#define LED_MD_CFG_LED6_SHIFT 6
#define LED_MD_CFG_LED6_SIZE  1
/* Bit: 'LED5' */
/* Description: 'LED Single Color Mode' */
#define LED_MD_CFG_LED5_OFFSET 0xF3E0
#define LED_MD_CFG_LED5_SHIFT 5
#define LED_MD_CFG_LED5_SIZE  1
/* Bit: 'LED4' */
/* Description: 'LED Single Color Mode' */
#define LED_MD_CFG_LED4_OFFSET 0xF3E0
#define LED_MD_CFG_LED4_SHIFT 4
#define LED_MD_CFG_LED4_SIZE  1
/* Bit: 'LED3' */
/* Description: 'LED Single Color Mode' */
#define LED_MD_CFG_LED3_OFFSET 0xF3E0
#define LED_MD_CFG_LED3_SHIFT 3
#define LED_MD_CFG_LED3_SIZE  1
/* Bit: 'LED2' */
/* Description: 'LED Single Color Mode' */
#define LED_MD_CFG_LED2_OFFSET 0xF3E0
#define LED_MD_CFG_LED2_SHIFT 2
#define LED_MD_CFG_LED2_SIZE  1
/* Bit: 'LED5' */
/* Description: 'LED Single Color Mode' */
#define LED_MD_CFG_LED1_OFFSET 0xF3E0
#define LED_MD_CFG_LED1_SHIFT 1
#define LED_MD_CFG_LED1_SIZE  1
/* Bit: 'LED0' */
/* Description: 'LED Single Color Mode' */
#define LED_MD_CFG_LED0_OFFSET 0xF3E0
#define LED_MD_CFG_LED0_SHIFT 0
#define LED_MD_CFG_LED0_SIZE  1
/* ----------------------------------------------- */
/* Register: 'LED Brightness Control Register' */
/* Bit: 'MAXLEVEL' */
/* Description: 'Maximum LED Brightness Value' */
#define LED_BRT_CTRL_MAXLEVEL_OFFSET 0xF3E1
#define LED_BRT_CTRL_MAXLEVEL_SHIFT 12
#define LED_BRT_CTRL_MAXLEVEL_SIZE  4
/* Bit: 'MINLEVEL' */
/* Description: 'Minimum LED Brightness Value' */
#define LED_BRT_CTRL_MINLEVEL_OFFSET 0xF3E1
#define LED_BRT_CTRL_MINLEVEL_SHIFT 8
#define LED_BRT_CTRL_MINLEVEL_SIZE  4
/* Bit: 'EDGE' */
/* Description: 'LED Brightness Switch Edge Detection' */
#define LED_BRT_CTRL_EDGE_OFFSET 0xF3E1
#define LED_BRT_CTRL_EDGE_SHIFT 6
#define LED_BRT_CTRL_EDGE_SIZE  1
/* Bit: 'EN' */
/* Description: 'LED Brightness Control Enable' */
#define LED_BRT_CTRL_EN_OFFSET 0xF3E1
#define LED_BRT_CTRL_EN_SHIFT 5
#define LED_BRT_CTRL_EN_SIZE  1
/* Bit: '2SEWN' */
/* Description: 'LED Brightness 2 Level Switch Enable' */
#define LED_BRT_CTRL_2SEWN_OFFSET 0xF3E1
#define LED_BRT_CTRL_2SEWN_SHIFT 6
#define LED_BRT_CTRL_2SEWN_SIZE  1
/* ----------------------------------------------- */
/* Register: 'LED Light Sensing Control Register' */
/* Bit: 'TD' */
/* Description: 'The Number of Slots for Discharge' */
#define LED_LSENS_CTRL_TD_OFFSET 0xF3E2
#define LED_LSENS_CTRL_TD_SHIFT 8
#define LED_LSENS_CTRL_TD_SIZE  6
/* Bit: 'CURLEVEL' */
/* Description: 'Current Brightness Level' */
#define LED_LSENS_CTRL_CURLEVEL_OFFSET 0xF3E2
#define LED_LSENS_CTRL_CURLEVEL_SHIFT 4
#define LED_LSENS_CTRL_CURLEVEL_SIZE  4
/* Bit: 'SENS' */
/* Description: 'LED Sensing Enable' */
#define LED_LSENS_CTRL_SENS_OFFSET 0xF3E2
#define LED_LSENS_CTRL_SENS_SHIFT 3
#define LED_LSENS_CTRL_SENS_SIZE  1
/* Bit: 'PERIOD' */
/* Description: 'LED Sensing Period' */
#define LED_LSENS_CTRL_PERIOD_OFFSET 0xF3E2
#define LED_LSENS_CTRL_PERIOD_SHIFT 0
#define LED_LSENS_CTRL_PERIOD_SIZE  3
/* ----------------------------------------------- */

/* ----------------------------------------------- */
/* SGMII Registers */
/* ----------------------------------------------- */
/* SGMII_PHY Registers                             */
/* ----------------------------------------------- */
/* Register: 'SGMII PHY Reset' */
/* Bit: 'RESET_N' */
/* Description: 'Asynchronous active low reset' */
#define SGMII_PHY_RESET_N_OFFSET 0xD000
#define SGMII_PHY_RESET_N_SHIFT 14
#define SGMII_PHY_RESET_N_SIZE  1
/* ----------------------------------------------- */
/* Register: 'SGMII_PHY_MPLL_CFG1' */
/* Bit: 'REF_USE_PAD' */
/* Description: 'MPLL Input Clock Mode' */
#define SGMII_PHY_MPLL_CFG1_REF_USE_PAD 0xD001
#define SGMII_PHY_MPLL_CFG1_REF_USE_PAD_SHIFT 10
#define SGMII_PHY_MPLL_CFG1_REF_USE_PAD_SIZE  1

/* Bit: 'REF_CLKDIV2' */
/* Description: 'Input Clock Frequency Division Enable' */
#define SGMII_PHY_MPLL_CFG1_MPLL_REF_CLKDIV2 0xD001
#define SGMII_PHY_MPLL_CFG1_REF_CLKDIV2_SHIFT 8
#define SGMII_PHY_MPLL_CFG1_REF_CLKDIV2_SIZE  1

/* Bit: 'MPLL_MULTIPLIER' */
/* Description: 'MPLL Frequency Multiplier' */
#define SGMII_PHY_MPLL_CFG1_MPLL_MPLL_MULTIPLIER 0xD001
#define SGMII_PHY_MPLL_CFG1_MPLL_MULTIPLIER_SHIFT 1
#define SGMII_PHY_MPLL_CFG1_MPLL_MULTIPLIER_SIZE  7

/* Bit: 'MPLL_EN' */
/* Description: 'MPLL Enable' */
#define SGMII_PHY_MPLL_CFG1_MPLL_MPLL_EN 0xD001
#define SGMII_PHY_MPLL_CFG1_MPLL_EN_SHIFT 0
#define SGMII_PHY_MPLL_CFG1_MPLL_EN_SIZE  1
/* ----------------------------------------------- */
/* Register: 'SGMII_PHY_MPLL_CFG2' */
/* Bit: 'SSC_REF_CLK_SEL' */
/* Description: 'Spread Spectrum Reference Clock Config' */
#define SGMII_PHY_MPLL_CFG2_SSC_REF_CLK_SEL 0xD002
#define SGMII_PHY_MPLL_CFG2_SSC_REF_CLK_SEL_SHIFT 0
#define SGMII_PHY_MPLL_CFG2_SSC_REF_CLK_SEL_SIZE  9
/* ----------------------------------------------- */
/* Register: 'SGMII_PHY_RX0_CFG1' */
/* Bit: 'RX0_RESET' */
/* Description: 'Active High Receiver Reset' */
#define SGMII_PHY_RX0_CFG1_RX0_RESET 0xD003
#define SGMII_PHY_RX0_CFG1_RX0_RESET_SHIFT 5
#define SGMII_PHY_RX0_CFG1_RX0_RESET_SIZE  1
/* Bit: 'RX0_RATE' */
/* Description: 'RX Data Rate' */
#define SGMII_PHY_RX0_CFG1_RX0_RATE 0xD003
#define SGMII_PHY_RX0_CFG1_RX0_RATE_SHIFT 3
#define SGMII_PHY_RX0_CFG1_RX0_RATE_SIZE  2
/* Bit: 'RX0_PLL_EN' */
/* Description: 'RX PLL Enable' */
#define SGMII_PHY_RX0_CFG1_RX0_PLL_EN 0xD003
#define SGMII_PHY_RX0_CFG1_RX0_PLL_EN_SHIFT 2
#define SGMII_PHY_RX0_CFG1_RX0_PLL_EN_SIZE  1
/* Bit: 'RX0_DATA_EN' */
/* Description: 'RX Data Enable' */
#define SGMII_PHY_RX0_CFG1_RX0_DATA_EN 0xD003
#define SGMII_PHY_RX0_CFG1_RX0_DATA_EN_SHIFT 1
#define SGMII_PHY_RX0_CFG1_RX0_DATA_EN_SIZE  1
/* Bit: 'RX0_ALIGN_EN' */
/* Description: 'RX Align Enable' */
#define SGMII_PHY_RX0_CFG1_RX0_ALIGN_EN 0xD003
#define SGMII_PHY_RX0_CFG1_RX0_ALIGN_EN_SHIFT 0
#define SGMII_PHY_RX0_CFG1_RX0_ALIGN_EN_SIZE  1
/* ----------------------------------------------- */



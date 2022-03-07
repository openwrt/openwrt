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


#ifndef _GSWITCH_REG_H_
#define _GSWITCH_REG_H_
/* --------------------------------------------------- */
/* Register: 'Ethernet Switch ResetControl Register' */
/* Bit: 'R1' */
/* Description: 'GSWIP Software Reset' */
#define ETHSW_SWRES_R1_OFFSET	0x0000
#define ETHSW_SWRES_R1_SHIFT	1
#define ETHSW_SWRES_R1_SIZE		1
/* Bit: 'R0' */
/* Description: 'GSWIP Hardware Reset' */
#define ETHSW_SWRES_R0_OFFSET	0x0000
#define ETHSW_SWRES_R0_SHIFT	0
#define ETHSW_SWRES_R0_SIZE		1
/* --------------------------------------------------- */
/* Register: 'Ethernet Switch Clock ControlRegister ' */
/* Bit: 'CORACT_OVR' */
/* Description: 'Core Active Override' */
#define ETHSW_CLK_CORACT_OVR_OFFSET	0x0001
#define ETHSW_CLK_CORACT_OVR_SHIFT	1
#define ETHSW_CLK_CORACT_OVR_SIZE		1
/* Bit: 'LNKDWN_OVR' */
/* Description: 'Link Down Override' */
#define ETHSW_CLK_LNKDWN_OVR_OFFSET	0x0001
#define ETHSW_CLK_LNKDWN_OVR_SHIFT	0
#define ETHSW_CLK_LNKDWN_OVR_SIZE		1
/* --------------------------------------- */
/* Register: 'Ethernet Switch SharedSegment Buffer Mode Register' */
/* Bit: 'ADDE' */
/* Description: 'Memory Address' */
#define ETHSW_SSB_MODE_ADDE_OFFSET	0x0003
#define ETHSW_SSB_MODE_ADDE_SHIFT		2
#define ETHSW_SSB_MODE_ADDE_SIZE		4
/* Bit: 'MODE' */
/* Description: 'Memory Access Mode' */
#define ETHSW_SSB_MODE_MODE_OFFSET	0x0003
#define ETHSW_SSB_MODE_MODE_SHIFT		0
#define ETHSW_SSB_MODE_MODE_SIZE		2
/* ----------------------------------------- */
/* Register: 'Ethernet Switch SharedSegment Buffer Address Register' */
/* Bit: 'ADDE' */
/* Description: 'Memory Address' */
#define ETHSW_SSB_ADDR_ADDE_OFFSET	0x0004
#define ETHSW_SSB_ADDR_ADDE_SHIFT		0
#define ETHSW_SSB_ADDR_ADDE_SIZE		16
/* ------------------------------------------	*/
/* Register: 'Ethernet Switch SharedSegment Buffer Data Register' */
/* Bit: 'DATA' */
/* Description: 'Data Value' */
#define ETHSW_SSB_DATA_DATA_OFFSET	0x0005
#define ETHSW_SSB_DATA_DATA_SHIFT		0
#define ETHSW_SSB_DATA_DATA_SIZE		16
/* --------------------------------------------------- */
/* Register: 'Ethernet Switch CapabilityRegister 0' */
/* Bit: 'SPEED' */
/* Description: 'Clock frequency' */
#define ETHSW_CAP_0_SPEED_OFFSET	0x0006
#define ETHSW_CAP_0_SPEED_SHIFT		0
#define ETHSW_CAP_0_SPEED_SIZE		16
/* --------------------------------------------------- */
/* Register: 'Ethernet Switch CapabilityRegister 1' */
/* Bit: 'GMAC' */
/* Description: 'MAC operation mode' */
#define ETHSW_CAP_1_GMAC_OFFSET	0x0007
#define ETHSW_CAP_1_GMAC_SHIFT	15
#define ETHSW_CAP_1_GMAC_SIZE		1
/* Bit: 'QUEUE' */
/* Description: 'Number of queues' */
#define ETHSW_CAP_1_QUEUE_OFFSET	0x0007
#define ETHSW_CAP_1_QUEUE_SHIFT		8
#define ETHSW_CAP_1_QUEUE_SIZE		7
/* Bit: 'VPORTS' */
/* Description: 'Number of virtual ports' */
#define ETHSW_CAP_1_VPORTS_OFFSET	0x0007
#define ETHSW_CAP_1_VPORTS_SHIFT	4
#define ETHSW_CAP_1_VPORTS_SIZE		4
/* Bit: 'PPORTS' */
/* Description: 'Number of physical ports' */
#define ETHSW_CAP_1_PPORTS_OFFSET	0x0007
#define ETHSW_CAP_1_PPORTS_SHIFT	0
#define ETHSW_CAP_1_PPORTS_SIZE		4
/* ---------------------------------------------------*/
/* Register: 'Ethernet Switch CapabilityRegister 2' */
/* Bit: 'PACKETS' */
/* Description: 'Number of packets' */
#define ETHSW_CAP_2_PACKETS_OFFSET	0x0008
#define ETHSW_CAP_2_PACKETS_SHIFT		0
#define ETHSW_CAP_2_PACKETS_SIZE		11
/* ---------------------------------------------------*/
/* Register: 'Ethernet Switch CapabilityRegister 3' */
/* Bit: 'METERS' */
/* Description: 'Number of traffic meters' */
#define ETHSW_CAP_3_METERS_OFFSET	0x0009
#define ETHSW_CAP_3_METERS_SHIFT	8
#define ETHSW_CAP_3_METERS_SIZE		8
/* Bit: 'SHAPERS' */
/* Description: 'Number of traffic shapers' */
#define ETHSW_CAP_3_SHAPERS_OFFSET	0x0009
#define ETHSW_CAP_3_SHAPERS_SHIFT		0
#define ETHSW_CAP_3_SHAPERS_SIZE		8
/* --------------------------------------------------- */
/* Register: 'Ethernet Switch CapabilityRegister 4' */
/* Bit: 'PPPOE' */
/* Description: 'PPPoE table size' */
#define ETHSW_CAP_4_PPPOE_OFFSET	0x000A
#define ETHSW_CAP_4_PPPOE_SHIFT		8
#define ETHSW_CAP_4_PPPOE_SIZE		8
/* Bit: 'VLAN' */
/* Description: 'Active VLAN table size' */
#define ETHSW_CAP_4_VLAN_OFFSET	0x000A
#define ETHSW_CAP_4_VLAN_SHIFT	0
#define ETHSW_CAP_4_VLAN_SIZE		8
/* --------------------------------------------------- */
/* Register: 'Ethernet Switch CapabilityRegister 5' */
/* Bit: 'IPPLEN' */
/* Description: 'IP packet length table size' */
#define ETHSW_CAP_5_IPPLEN_OFFSET	0x000B
#define ETHSW_CAP_5_IPPLEN_SHIFT	8
#define ETHSW_CAP_5_IPPLEN_SIZE		8
/* Bit: 'PROT' */
/* Description: 'Protocol table size' */
#define ETHSW_CAP_5_PROT_OFFSET	0x000B
#define ETHSW_CAP_5_PROT_SHIFT	0
#define ETHSW_CAP_5_PROT_SIZE		8
/* --------------------------------------------------- */
/* Register: 'Ethernet Switch CapabilityRegister 6' */
/* Bit: 'MACDASA' */
/* Description: 'MAC DA/SA table size' */
#define ETHSW_CAP_6_MACDASA_OFFSET	0x000C
#define ETHSW_CAP_6_MACDASA_SHIFT		8
#define ETHSW_CAP_6_MACDASA_SIZE		8
/* Bit: 'APPL' */
/* Description: 'Application table size' */
#define ETHSW_CAP_6_APPL_OFFSET	0x000C
#define ETHSW_CAP_6_APPL_SHIFT	0
#define ETHSW_CAP_6_APPL_SIZE		8
/* ---------------------------------------------------*/
/* Register: 'Ethernet Switch CapabilityRegister 7' */
/* Bit: 'IPDASAM' */
/* Description: 'IP DA/SA MSB table size' */
#define ETHSW_CAP_7_IPDASAM_OFFSET	0x000D
#define ETHSW_CAP_7_IPDASAM_SHIFT		8
#define ETHSW_CAP_7_IPDASAM_SIZE		8
/* Bit: 'IPDASAL' */
/* Description: 'IP DA/SA LSB table size' */
#define ETHSW_CAP_7_IPDASAL_OFFSET	0x000D
#define ETHSW_CAP_7_IPDASAL_SHIFT		0
#define ETHSW_CAP_7_IPDASAL_SIZE		8
/* --------------------------------------------------- */
/* Register: 'Ethernet Switch CapabilityRegister 8' */
/* Bit: 'MCAST' */
/* Description: 'Multicast table size' */
#define ETHSW_CAP_8_MCAST_OFFSET	0x000E
#define ETHSW_CAP_8_MCAST_SHIFT		0
#define ETHSW_CAP_8_MCAST_SIZE		8
/* --------------------------------------------------- */
/* Register: 'Ethernet Switch CapabilityRegister 9' */
/** Bit: 'FLAGG'  (LTQ_GSWIP_3_0) */
/* Description: 'Flow Aggregation table size' */
#define ETHSW_CAP_9_FLAGG_OFFSET	0x000F
#define ETHSW_CAP_9_FLAGG_SHIFT		0
#define ETHSW_CAP_9_FLAGG_SIZE		10
/* --------------------------------------------------- */
/* Register: 'Ethernet Switch CapabilityRegister 10' */
/* Bit: 'MACBT' */
/* Description: 'MAC bridging table size' */
#define ETHSW_CAP_10_MACBT_OFFSET	0x0010
#define ETHSW_CAP_10_MACBT_SHIFT	0
#define ETHSW_CAP_10_MACBT_SIZE		13
/* --------------------------------------------------- */
/* Register: 'Ethernet Switch CapabilityRegister 11' */
/* Bit: 'BSIZEL' */
/* Description: 'Packet buffer size (lower part, in byte)' */
#define ETHSW_CAP_11_BSIZEL_OFFSET	0x0011
#define ETHSW_CAP_11_BSIZEL_SHIFT		0
#define ETHSW_CAP_11_BSIZEL_SIZE		16
/* --------------------------------------------------- */
/* Register: 'Ethernet Switch CapabilityRegister 12' */
/* Bit: 'BSIZEH' */
/* Description: 'Packet buffer size (higher part, in byte)' */
#define ETHSW_CAP_12_BSIZEH_OFFSET	0x0012
#define ETHSW_CAP_12_BSIZEH_SHIFT		0
#define ETHSW_CAP_12_BSIZEH_SIZE		3
/* --------------------------------------------------- */
/* Register: 'Ethernet Switch VersionRegister' */
/* Bit: 'MOD_ID' */
/* Description: 'Module Identification' */
#define ETHSW_VERSION_MOD_ID_OFFSET	0x0013
#define ETHSW_VERSION_MOD_ID_SHIFT	8
#define ETHSW_VERSION_MOD_ID_SIZE		8
/* Bit: 'REV_ID' */
/* Description: 'Hardware Revision Identification' */
#define ETHSW_VERSION_REV_ID_OFFSET	0x0013
#define ETHSW_VERSION_REV_ID_SHIFT	0
#define ETHSW_VERSION_REV_ID_SIZE		8
#define ETHSW_VERSION_REV_ID_REG_SIZE		16
/* --------------------------------------------------- */
/* Register: 'Interrupt Enable Register' */
/* Bit: 'FDMAIE' */
/* Description: 'Fetch DMA Interrupt Enable' */
#define ETHSW_IER_FDMAIE_OFFSET	0x0014
#define ETHSW_IER_FDMAIE_SHIFT	4
#define ETHSW_IER_FDMAIE_SIZE		1
/* Bit: 'SDMAIE' */
/* Description: 'Store DMA Interrupt Enable' */
#define ETHSW_IER_SDMAIE_OFFSET	0x0014
#define ETHSW_IER_SDMAIE_SHIFT	3
#define ETHSW_IER_SDMAIE_SIZE		1
/* Bit: 'MACIE' */
/* Description: 'Ethernet MAC Interrupt Enable' */
#define ETHSW_IER_MACIE_OFFSET	0x0014
#define ETHSW_IER_MACIE_SHIFT		2
#define ETHSW_IER_MACIE_SIZE		1
/* Bit: 'PCEIE' */
/* Description: 'Parser and Classification Engine Interrupt Enable' */
#define ETHSW_IER_PCEIE_OFFSET	0x0014
#define ETHSW_IER_PCEIE_SHIFT		1
#define ETHSW_IER_PCEIE_SIZE		1
/* Bit: 'BMIE' */
/* Description: 'Buffer Manager Interrupt Enable' */
#define ETHSW_IER_BMIE_OFFSET	0x0014
#define ETHSW_IER_BMIE_SHIFT	0
#define ETHSW_IER_BMIE_SIZE		1
/* --------------------------------------------------- */
/* Register: 'Interrupt Status Register' */
/* Bit: 'FDMAINT' */
/* Description: 'Fetch DMA Interrupt' */
#define ETHSW_ISR_FDMAINT_OFFSET	0x0015
#define ETHSW_ISR_FDMAINT_SHIFT		4
#define ETHSW_ISR_FDMAINT_SIZE		1
/* Bit: 'SDMAINT' */
/* Description: 'Store DMA Interrupt' */
#define ETHSW_ISR_SDMAINT_OFFSET	0x0015
#define ETHSW_ISR_SDMAINT_SHIFT		3
#define ETHSW_ISR_SDMAINT_SIZE		1
/* Bit: 'MACINT' */
/* Description: 'Ethernet MAC Interrupt' */
#define ETHSW_ISR_MACINT_OFFSET	0x0015
#define ETHSW_ISR_MACINT_SHIFT	2
#define ETHSW_ISR_MACINT_SIZE		1
/* Bit: 'PCEINT' */
/* Description: 'Parser and Classification Engine Interrupt' */
#define ETHSW_ISR_PCEINT_OFFSET	0x0015
#define ETHSW_ISR_PCEINT_SHIFT	1
#define ETHSW_ISR_PCEINT_SIZE		1
/* Bit: 'BMINT' */
/* Description: 'Buffer Manager Interrupt' */
#define ETHSW_ISR_BMINT_OFFSET	0x0015
#define ETHSW_ISR_BMINT_SHIFT		0
#define ETHSW_ISR_BMINT_SIZE		1
/* --------------------------------------------------- */
/* Register: 'Ethernet Switch CapabilityRegister 13 (LTQ_GSWIP_3_0)' */
/* Bit: 'PMAC' */
/* Description: 'Number of PMAC' */
#define ETHSW_CAP_13_PMAC_OFFSET	0x0016
#define ETHSW_CAP_13_PMAC_SHIFT	12
#define ETHSW_CAP_13_PMAC_SIZE		4
/* Bit: 'PAYLOAD' */
/* Description: 'Payload Table Size (2**PAYLOAD)' */
#define ETHSW_CAP_13_PAYLOAD_OFFSET	0x0016
#define ETHSW_CAP_13_PAYLOAD_SHIFT		8
#define ETHSW_CAP_13_PAYLOAD_SIZE		4
/* Bit: 'INTRMON' */
/* Description: 'Interface RMON Counter Table Size(2**INTRMON)' */
#define ETHSW_CAP_13_INTRMON_OFFSET	0x0016
#define ETHSW_CAP_13_INTRMON_SHIFT 4
#define ETHSW_CAP_13_INTRMON_SIZE  4
/* Bit: 'EVLAN' */
/* Description: 'Egress VLAN Treatment Table Size (2**EVLAN)' */
#define ETHSW_CAP_13_EVLAN_OFFSET	0x0016
#define ETHSW_CAP_13_EVLAN_SHIFT	0
#define ETHSW_CAP_13_EVLAN_SIZE		4
/* ---------------------------------------------------*/
/* Register: 'Ethernet Switch CapabilityRegister 14 (LTQ_GSWIP_3_0)' */
/* Bit: 'SMAC' */
/* Description: 'Routing MAC Table Size for Source MAC (2**SMAC.)' */
#define ETHSW_CAP_14_SMAC_OFFSET	0x0017
#define ETHSW_CAP_14_SMAC_SHIFT	12
#define ETHSW_CAP_14_SMAC_SIZE		4
/* Bit: 'DMAC' */
/* Description: 'Routing MAC Table Size for Destination MAC (2**DMAC)' */
#define ETHSW_CAP_14_DMAC_OFFSET	0x0017
#define ETHSW_CAP_14_DMAC_SHIFT	8
#define ETHSW_CAP_14_DMAC_SIZE	4
/* Bit: 'PPPoE' */
/* Description: 'Routing PPPoE Table Size(2**PPPoE)' */
#define ETHSW_CAP_14_PPPoE_OFFSET	0x0017
#define ETHSW_CAP_14_PPPoE_SHIFT	4
#define ETHSW_CAP_14_PPPoE_SIZE		4
/* Bit: 'NAT' */
/* Description: 'Routing NAT Table Size (2**NAT)' */
#define ETHSW_CAP_14_NAT_OFFSET	0x0017
#define ETHSW_CAP_14_NAT_SHIFT	0
#define ETHSW_CAP_14_NAT_SIZE		4
/* ---------------------------------------------------*/
/* Register: 'Ethernet Switch CapabilityRegister 15 (LTQ_GSWIP_3_0)' */
/* Bit: 'MTU' */
/* Description: 'Routing MTU Table Size (2**MTU)' */
#define ETHSW_CAP_15_MTU_OFFSET	0x0018
#define ETHSW_CAP_15_MTU_SHIFT	8
#define ETHSW_CAP_15_MTU_SIZE		4
/* Bit: 'TUNNEL' */
/* Description: 'Routing Tunnel Table Size (2**TUNNEL)' */
#define ETHSW_CAP_15_TUNNEL_OFFSET	0x0018
#define ETHSW_CAP_15_TUNNEL_SHIFT	4
#define ETHSW_CAP_15_TUNNEL_SIZE	4
/* Bit: 'RTP' */
/* Description: 'Routing RTP Table Size (2**RTP)' */
#define ETHSW_CAP_15_RTP_OFFSET	0x018
#define ETHSW_CAP_15_RTP_SHIFT		0
#define ETHSW_CAP_15_RTP_SIZE		4
/* ---------------------------------------------------*/
/* Bits: FSLSB	*/
/* Description: Timer Fractional Nano Second LSB Value (LTQ_GSWIP_2_2 ) */
#define TIMER_FS_LSB_FSLSB_OFFSET	0x0020
#define TIMER_FS_LSB_FSLSB_SHIFT	0
#define TIMER_FS_LSB_FSLSB_SIZE		15
/* --------------------------------------------------- */
/* Bits: FSMSB	*/
/* Description: Timer Fractional Nano Second MSB Value  */
#define TIMER_FS_MSB_FSMSB_OFFSET	0x0021
#define TIMER_FS_MSB_FSMSB_SHIFT	0
#define TIMER_FS_MSB_FSMSB_SIZE		16
/* --------------------------------------------------- */
/* Bits: NSLSB	*/
/* Description: Timer Nano Second LSB Register  */
#define TIMER_NS_LSB_NSLSB_OFFSET	0x0022
#define TIMER_NS_LSB_NSLSB_SHIFT	0
#define TIMER_NS_LSB_NSLSB_SIZE		16
/* --------------------------------------------------- */
/* Bits: NSMSB	*/
/* Description: Timer Nano Second MSB Register  */
#define TIMER_NS_MSB_NSMSB_OFFSET	0x0023
#define TIMER_NS_MSB_NSMSB_SHIFT	0
#define TIMER_NS_MSB_NSMSB_SIZE		16
/* --------------------------------------------------- */
/* Bits: SECLSB	*/
/* Description: Timer Second LSB Register  */
#define TIMER_SEC_LSB_SECLSB_OFFSET	0x0024
#define TIMER_SEC_LSB_SECLSB_SHIFT	0
#define TIMER_SEC_LSB_SECLSB_SIZE		16
/* --------------------------------------------------- */
/* Bits: SECMSB	*/
/* Description: Timer Second MSB Register  */
#define TIMER_SEC_MSB_SECMSB_OFFSET	0x0025
#define TIMER_SEC_MSB_SECMSB_SHIFT	0
#define TIMER_SEC_MSB_SECMSB_SIZE		16
/* --------------------------------------------------- */
/* Bit: WR	*/
/* Description: Write to Timer Command  (LTQ_GSWIP_2_2) */
#define TIMER_CTRL_WR_OFFSET	0x0026
#define TIMER_CTRL_WR_SHIFT		15
#define TIMER_CTRL_WR_SIZE		1
/* Bit: RD	*/
/* Description: Read from Timer Command  */
#define TIMER_CTRL_RD_OFFSET	0x0026
#define TIMER_CTRL_RD_SHIFT		14
#define TIMER_CTRL_RD_SIZE		1
/* Bit: ADJ	*/
/* Description: Adjust Timer Command  */
#define TIMER_CTRL_ADJ_OFFSET	0x0026
#define TIMER_CTRL_ADJ_SHIFT	13
#define TIMER_CTRL_ADJ_SIZE		1
/* --------------------------------------------------- */
/* Register: 'RAM Value Register 8' (LTQ_GSWIP_3_0)*/
/* Bit: 'VAL8' */
/* Description: 'Data value [15:0]' */
#define BM_RAM_VAL_8_VAL8_OFFSET	0x003B
#define BM_RAM_VAL_8_VAL8_SHIFT		0
#define BM_RAM_VAL_8_VAL8_SIZE		16
/* --------------------------------------- */
/* Register: 'RAM Value Register 7' (LTQ_GSWIP_3_0)*/
/* Bit: 'VAL7' */
/* Description: 'Data value [15:0]' */
#define BM_RAM_VAL_7_VAL7_OFFSET	0x003C
#define BM_RAM_VAL_7_VAL7_SHIFT		0
#define BM_RAM_VAL_7_VAL7_SIZE		16
/* --------------------------------------- */
/* Register: 'RAM Value Register 6' (LTQ_GSWIP_3_0)*/
/* Bit: 'VAL6' */
/* Description: 'Data value [15:0]' */
#define BM_RAM_VAL_6_VAL6_OFFSET	0x003D
#define BM_RAM_VAL_6_VAL6_SHIFT		0
#define BM_RAM_VAL_6_VAL6_SIZE		16
/* --------------------------------------- */
/* Register: 'RAM Value Register 5' (LTQ_GSWIP_3_0)*/
/* Bit: 'VAL5' */
/* Description: 'Data value [15:0]' */
#define BM_RAM_VAL_5_VAL5_OFFSET	0x003E
#define BM_RAM_VAL_5_VAL5_SHIFT		0
#define BM_RAM_VAL_5_VAL5_SIZE		16
/* --------------------------------------- */
/* Register: 'RAM Value Register 4' (LTQ_GSWIP_2_2)*/
/* Bit: 'VAL4' */
/* Description: 'Data value [15:0]' */
#define BM_RAM_VAL_4_VAL4_OFFSET	0x003F
#define BM_RAM_VAL_4_VAL4_SHIFT		0
#define BM_RAM_VAL_4_VAL4_SIZE		16
/* --------------------------------------- */
/* Register: 'RAM Value Register 3' */
/* Bit: 'VAL3' */
/* Description: 'Data value [15:0]' */
#define BM_RAM_VAL_3_VAL3_OFFSET	0x0040
#define BM_RAM_VAL_3_VAL3_SHIFT		0
#define BM_RAM_VAL_3_VAL3_SIZE		16
/* --------------------------------------------------- */
/* Register: 'RAM Value Register 2' */
/* Bit: 'VAL2' */
/* Description: 'Data value [15:0]' */
#define BM_RAM_VAL_2_VAL2_OFFSET	0x0041
#define BM_RAM_VAL_2_VAL2_SHIFT		0
#define BM_RAM_VAL_2_VAL2_SIZE		16
/* --------------------------------------------------- */
/* Register: 'RAM Value Register 1' */
/* Bit: 'VAL1' */
/* Description: 'Data value [15:0]' */
#define BM_RAM_VAL_1_VAL1_OFFSET	0x0042
#define BM_RAM_VAL_1_VAL1_SHIFT		0
#define BM_RAM_VAL_1_VAL1_SIZE		16
/* --------------------------------------------------- */
/* Register: 'RAM Value Register 0' */
/* Bit: 'VAL0' */
/* Description: 'Data value [15:0]' */
#define BM_RAM_VAL_0_VAL0_OFFSET	0x0043
#define BM_RAM_VAL_0_VAL0_SHIFT		0
#define BM_RAM_VAL_0_VAL0_SIZE		16
/* --------------------------------------------------- */
/* Register: 'RAM Address Register' */
/* Bit: 'ADDR' */
/* Description: 'RAM Address' */
#define BM_RAM_ADDR_ADDR_OFFSET	0x0044
#define BM_RAM_ADDR_ADDR_SHIFT	0
#define BM_RAM_ADDR_ADDR_SIZE		11
/* --------------------------------------------------- */
/* Register: 'RAM Access Control Register' */
/* Bit: 'BAS' */
/* Description: 'Access Busy/Access Start' */
#define BM_RAM_CTRL_BAS_OFFSET	0x0045
#define BM_RAM_CTRL_BAS_SHIFT		15
#define BM_RAM_CTRL_BAS_SIZE		1
/* Bit: 'OPMOD' */
/* Description: 'Lookup Table Access Operation Mode' */
#define BM_RAM_CTRL_OPMOD_OFFSET	0x0045
#define BM_RAM_CTRL_OPMOD_SHIFT		5
#define BM_RAM_CTRL_OPMOD_SIZE		1
/* Bit: 'ADDR' */
/* Description: 'Address for RAM selection' */
#define BM_RAM_CTRL_ADDR_OFFSET	0x0045
#define BM_RAM_CTRL_ADDR_SHIFT	0
#define BM_RAM_CTRL_ADDR_SIZE		5
/* --------------------------------------------------- */
/* Register: 'Free Segment Queue ManagerGlobal Control Register' */
/* Bit: 'SEGNUM' */
/* Description: 'Maximum Segment Number' */
#define BM_FSQM_GCTRL_SEGNUM_OFFSET	0x0046
#define BM_FSQM_GCTRL_SEGNUM_SHIFT	0
#define BM_FSQM_GCTRL_SEGNUM_SIZE		10
/* --------------------------------------------------- */
/* Register: 'Number of Consumed SegmentsRegister' */
/* Bit: 'FSEG' */
/* Description: 'Number of Consumed Segments' */
#define BM_CONS_SEG_FSEG_OFFSET	0x0047
#define BM_CONS_SEG_FSEG_SHIFT	0
#define BM_CONS_SEG_FSEG_SIZE		10
/* --------------------------------------------------- */
/* Register: 'Number of Consumed PacketPointers Register' */
/* Bit: 'FQP' */
/* Description: 'Number of Consumed Packet Pointers' */
#define BM_CONS_PKT_FQP_OFFSET	0x0048
#define BM_CONS_PKT_FQP_SHIFT		0
#define BM_CONS_PKT_FQP_SIZE		11
/* --------------------------------------------------- */
/* Register: 'Buffer Manager Global ControlRegister 0' */
/* Bit: 'FR_RERGC' (LTQ_GSWIP_3_0)*/
/* Description: 'Freeze RMON Redirection RX Byte 64 Bit Counter' */
#define BM_GCTRL_FR_RERGC_OFFSET	0x0049
#define BM_GCTRL_FR_RERGC_SHIFT		15
#define BM_GCTRL_FR_RERGC_SIZE		1
/* Bit: 'FR_RETGC' (LTQ_GSWIP_3_0)*/
/* Description: 'Freeze RMON Redirection TX Byte 64 Bit Counter' */
#define BM_GCTRL_FR_RETGC_OFFSET	0x0049
#define BM_GCTRL_FR_RETGC_SHIFT		14
#define BM_GCTRL_FR_RETGC_SIZE		1
/* Bit: 'BM_STA' */
/* Description: 'Buffer Manager Initialization Status Bit' */
#define BM_GCTRL_BM_STA_OFFSET	0x0049
#define BM_GCTRL_BM_STA_SHIFT		13
#define BM_GCTRL_BM_STA_SIZE		1
/* Bit: 'SAT' */
/* Description: 'RMON Counter Update Mode' */
#define BM_GCTRL_SAT_OFFSET	0x0049
#define BM_GCTRL_SAT_SHIFT	12
#define BM_GCTRL_SAT_SIZE		1
/* Bit: 'FR_RBC' */
/* Description: 'Freeze RMON RX Bad Byte 64 Bit Counter' */
#define BM_GCTRL_FR_RBC_OFFSET	0x0049
#define BM_GCTRL_FR_RBC_SHIFT		11
#define BM_GCTRL_FR_RBC_SIZE		1
/* Bit: 'FR_RGC' */
/* Description: 'Freeze RMON RX Good Byte 64 Bit Counter' */
#define BM_GCTRL_FR_RGC_OFFSET	0x0049
#define BM_GCTRL_FR_RGC_SHIFT		10
#define BM_GCTRL_FR_RGC_SIZE		1
/* Bit: 'FR_TGC' */
/* Description: 'Freeze RMON TX Good Byte 64 Bit Counter' */
#define BM_GCTRL_FR_TGC_OFFSET	0x0049
#define BM_GCTRL_FR_TGC_SHIFT		9
#define BM_GCTRL_FR_TGC_SIZE		1
/* Bit: 'I_FIN' */
/* Description: 'RAM initialization finished' */
#define BM_GCTRL_I_FIN_OFFSET	0x0049
#define BM_GCTRL_I_FIN_SHIFT	8
#define BM_GCTRL_I_FIN_SIZE		1
/* Bit: 'CX_INI' */
/* Description: 'PQM Context RAM initialization' */
#define BM_GCTRL_CX_INI_OFFSET	0x0049
#define BM_GCTRL_CX_INI_SHIFT		7
#define BM_GCTRL_CX_INI_SIZE		1
/* Bit: 'FP_INI' */
/* Description: 'FPQM RAM initialization' */
#define BM_GCTRL_FP_INI_OFFSET	0x0049
#define BM_GCTRL_FP_INI_SHIFT		6
#define BM_GCTRL_FP_INI_SIZE		1
/* Bit: 'FS_INI' */
/* Description: 'FSQM RAM initialization' */
#define BM_GCTRL_FS_INI_OFFSET	0x0049
#define BM_GCTRL_FS_INI_SHIFT		5
#define BM_GCTRL_FS_INI_SIZE		1
/* Bit: 'R_SRES' */
/* Description: 'Software Reset for RMON' */
#define BM_GCTRL_R_SRES_OFFSET	0x0049
#define BM_GCTRL_R_SRES_SHIFT		4
#define BM_GCTRL_R_SRES_SIZE		1
/* Bit: 'S_SRES' */
/* Description: 'Software Reset for Scheduler' */
#define BM_GCTRL_S_SRES_OFFSET	0x0049
#define BM_GCTRL_S_SRES_SHIFT		3
#define BM_GCTRL_S_SRES_SIZE		1
/* Bit: 'A_SRES' */
/* Description: 'Software Reset for AVG' */
#define BM_GCTRL_A_SRES_OFFSET	0x0049
#define BM_GCTRL_A_SRES_SHIFT		2
#define BM_GCTRL_A_SRES_SIZE		1
/* Bit: 'P_SRES' */
/* Description: 'Software Reset for PQM' */
#define BM_GCTRL_P_SRES_OFFSET	0x0049
#define BM_GCTRL_P_SRES_SHIFT		1
#define BM_GCTRL_P_SRES_SIZE		1
/* Bit: 'F_SRES' */
/* Description: 'Software Reset for FSQM' */
#define BM_GCTRL_F_SRES_OFFSET	0x0049
#define BM_GCTRL_F_SRES_SHIFT		0
#define BM_GCTRL_F_SRES_SIZE		1
/* --------------------------------------------------- */
/* Register: 'Queue Manager GlobalControl Register 0' */
/* Bit: 'BUFMOD' (LTQ_GSWIP_3_0) */
/* Description: 'Buffer Control Mode' */
#define BM_QUEUE_GCTRL_BUFMOD_OFFSET	0x004A
#define BM_QUEUE_GCTRL_BUFMOD_SHIFT		12
#define BM_QUEUE_GCTRL_BUFMOD_SIZE		1
/* Bit: 'GL_MOD' */
/* Description: 'WRED Mode Signal' */
#define BM_QUEUE_GCTRL_GL_MOD_OFFSET	0x004A
#define BM_QUEUE_GCTRL_GL_MOD_SHIFT		10
#define BM_QUEUE_GCTRL_GL_MOD_SIZE		1
/* Bit: 'AQUI' */
/* Description: 'Average Queue Update Interval' */
#define BM_QUEUE_GCTRL_AQUI_OFFSET	0x004A
#define BM_QUEUE_GCTRL_AQUI_SHIFT		7
#define BM_QUEUE_GCTRL_AQUI_SIZE		3
/* Bit: 'AQWF' */
/* Description: 'Average Queue Weight Factor' */
#define BM_QUEUE_GCTRL_AQWF_OFFSET	0x004A
#define BM_QUEUE_GCTRL_AQWF_SHIFT		3
#define BM_QUEUE_GCTRL_AQWF_SIZE		4
/* Bit: 'QAVGEN' */
/* Description: 'Queue Average Calculation Enable' */
#define BM_QUEUE_GCTRL_QAVGEN_OFFSET	0x004A
#define BM_QUEUE_GCTRL_QAVGEN_SHIFT		2
#define BM_QUEUE_GCTRL_QAVGEN_SIZE		1
/* Bit: 'DPROB' */
/* Description: 'Drop Probability Profile' */
#define BM_QUEUE_GCTRL_DPROB_OFFSET	0x004A
#define BM_QUEUE_GCTRL_DPROB_SHIFT	0
#define BM_QUEUE_GCTRL_DPROB_SIZE		2
/* --------------------------------------------------- */
/* Register: 'WRED Red Threshold Register0' */
/* Bit: 'MINTH' */
/* Description: 'Minimum Threshold' */
#define BM_WRED_RTH_0_MINTH_OFFSET	0x004B
#define BM_WRED_RTH_0_MINTH_SHIFT		0
#define BM_WRED_RTH_0_MINTH_SIZE		10
/* --------------------------------------------------- */
/* Register: 'WRED Red Threshold Register1' */
/* Bit: 'MAXTH' */
/* Description: 'Maximum Threshold' */
#define BM_WRED_RTH_1_MAXTH_OFFSET	0x004C
#define BM_WRED_RTH_1_MAXTH_SHIFT		0
#define BM_WRED_RTH_1_MAXTH_SIZE		10
/* --------------------------------------------------- */
/* Register: 'WRED Yellow ThresholdRegister 0' */
/* Bit: 'MINTH' */
/* Description: 'Minimum Threshold' */
#define BM_WRED_YTH_0_MINTH_OFFSET	0x004D
#define BM_WRED_YTH_0_MINTH_SHIFT		0
#define BM_WRED_YTH_0_MINTH_SIZE		10
/* --------------------------------------------------- */
/* Register: 'WRED Yellow ThresholdRegister 1' */
/* Bit: 'MAXTH' */
/* Description: 'Maximum Threshold' */
#define BM_WRED_YTH_1_MAXTH_OFFSET	0x004E
#define BM_WRED_YTH_1_MAXTH_SHIFT		0
#define BM_WRED_YTH_1_MAXTH_SIZE		10
/* --------------------------------------------------- */
/* Register: 'WRED Green ThresholdRegister 0' */
/* Bit: 'MINTH' */
/* Description: 'Minimum Threshold' */
#define BM_WRED_GTH_0_MINTH_OFFSET	0x004F
#define BM_WRED_GTH_0_MINTH_SHIFT		0
#define BM_WRED_GTH_0_MINTH_SIZE		10
/* --------------------------------------------------- */
/* Register: 'WRED Green ThresholdRegister 1' */
/* Bit: 'MAXTH' */
/* Description: 'Maximum Threshold' */
#define BM_WRED_GTH_1_MAXTH_OFFSET	0x0050
#define BM_WRED_GTH_1_MAXTH_SHIFT		0
#define BM_WRED_GTH_1_MAXTH_SIZE		10
/* --------------------------------------------------- */
/* Register: 'Drop Threshold ConfigurationRegister 0' */
/* Bit: 'THR_FQ' */
/* Description: 'Threshold for frames marked red' */
#define BM_DROP_GTH_0_THR_FQ_OFFSET	0x0051
#define BM_DROP_GTH_0_THR_FQ_SHIFT	0
#define BM_DROP_GTH_0_THR_FQ_SIZE		11
/* --------------------------------------------------- */
/* Register: 'Drop Threshold ConfigurationRegister 1' */
/* Bit: 'THY_FQ' */
/* Description: 'Threshold for frames marked yellow' */
#define BM_DROP_GTH_1_THY_FQ_OFFSET	0x0052
#define BM_DROP_GTH_1_THY_FQ_SHIFT	0
#define BM_DROP_GTH_1_THY_FQ_SIZE		11
/* --------------------------------------------------- */
/* Register: 'Drop Threshold ConfigurationRegister 2' */
/* Bit: 'THG_FQ' */
/* Description: 'Threshold for frames marked green' */
#define BM_DROP_GTH_2_THG_FQ_OFFSET	0x0053
#define BM_DROP_GTH_2_THG_FQ_SHIFT	0
#define BM_DROP_GTH_2_THG_FQ_SIZE		11
/* --------------------------------------------------- */
/* Register: 'Buffer Manager Global InterruptEnable Register' */
/* Bit: 'CNT4' */
/* Description: 'Counter Group 4 (RMON-CLASSIFICATION) Interrupt*/
/* Enable' */
#define BM_IER_CNT4_OFFSET	0x0054
#define BM_IER_CNT4_SHIFT		7
#define BM_IER_CNT4_SIZE		1
/* Bit: 'CNT3' */
/* Description: 'Counter Group 3 (RMON-PQM) Interrupt Enable' */
#define BM_IER_CNT3_OFFSET	0x0054
#define BM_IER_CNT3_SHIFT		6
#define BM_IER_CNT3_SIZE		1
/* Bit: 'CNT2' */
/* Description: 'Counter Group 2 (RMON-SCHEDULER) Interrupt Enable' */
#define BM_IER_CNT2_OFFSET	0x0054
#define BM_IER_CNT2_SHIFT		5
#define BM_IER_CNT2_SIZE		1
/* Bit: 'CNT1' */
/* Description: 'Counter Group 1 (RMON-QFETCH) Interrupt Enable' */
#define BM_IER_CNT1_OFFSET	0x0054
#define BM_IER_CNT1_SHIFT		4
#define BM_IER_CNT1_SIZE		1
/* Bit: 'CNT0' */
/* Description: 'Counter Group 0 (RMON-QSTOR) Interrupt Enable' */
#define BM_IER_CNT0_OFFSET	0x0054
#define BM_IER_CNT0_SHIFT		3
#define BM_IER_CNT0_SIZE		1
/* Bit: 'DEQ' */
/* Description: 'PQM dequeue Interrupt Enable' */
#define BM_IER_DEQ_OFFSET	0x0054
#define BM_IER_DEQ_SHIFT	2
#define BM_IER_DEQ_SIZE		1
/* Bit: 'ENQ' */
/* Description: 'PQM Enqueue Interrupt Enable' */
#define BM_IER_ENQ_OFFSET	0x0054
#define BM_IER_ENQ_SHIFT	1
#define BM_IER_ENQ_SIZE		1
/* Bit: 'FSQM' */
/* Description: 'Buffer Empty Interrupt Enable' */
#define BM_IER_FSQM_OFFSET	0x0054
#define BM_IER_FSQM_SHIFT		0
#define BM_IER_FSQM_SIZE		1
/* --------------------------------------------------- */
/* Register: 'Buffer Manager Global InterruptStatus Register'(LTQ_GSWIP_2_2) */
/* Bit: 'EQCGN' */
/* Description: 'Egress Queue Congestion Status Change Interrupt' */
#define BM_ISR_EQCGN_OFFSET	0x0055
#define BM_ISR_EQCGN_SHIFT	8
#define BM_ISR_EQCGN_SIZE		1
/* Bit: 'CNT4' */
/* Description: 'Counter Group 4 Interrupt' */
#define BM_ISR_CNT4_OFFSET	0x0055
#define BM_ISR_CNT4_SHIFT		7
#define BM_ISR_CNT4_SIZE		1
/* Bit: 'CNT3' */
/* Description: 'Counter Group 3 Interrupt' */
#define BM_ISR_CNT3_OFFSET	0x0055
#define BM_ISR_CNT3_SHIFT		6
#define BM_ISR_CNT3_SIZE		1
/* Bit: 'CNT2' */
/* Description: 'Counter Group 2 Interrupt' */
#define BM_ISR_CNT2_OFFSET	0x0055
#define BM_ISR_CNT2_SHIFT		5
#define BM_ISR_CNT2_SIZE		1
/* Bit: 'CNT1' */
/* Description: 'Counter Group 1 Interrupt' */
#define BM_ISR_CNT1_OFFSET	0x0055
#define BM_ISR_CNT1_SHIFT		4
#define BM_ISR_CNT1_SIZE		1
/* Bit: 'CNT0' */
/* Description: 'Counter Group 0 Interrupt' */
#define BM_ISR_CNT0_OFFSET	0x0055
#define BM_ISR_CNT0_SHIFT		3
#define BM_ISR_CNT0_SIZE		1
/* Bit: 'DEQ' */
/* Description: 'PQM dequeue Interrupt Enable' */
#define BM_ISR_DEQ_OFFSET		0x0055
#define BM_ISR_DEQ_SHIFT		2
#define BM_ISR_DEQ_SIZE			1
/* Bit: 'ENQ' */
/* Description: 'PQM Enqueue Interrupt' */
#define BM_ISR_ENQ_OFFSET		0x0055
#define BM_ISR_ENQ_SHIFT		1
#define BM_ISR_ENQ_SIZE			1
/* Bit: 'FSQM' */
/* Description: 'Buffer Empty Interrupt' */
#define BM_ISR_FSQM_OFFSET	0x0055
#define BM_ISR_FSQM_SHIFT		0
#define BM_ISR_FSQM_SIZE		1
/* --------------------------------------------------- */
/* Register: 'Buffer Manager RMON CounterInterrupt Select Register' */
/* Bit: 'PORT' */
/* Description: 'Port Number' */
#define BM_CISEL_PORT_OFFSET	0x056
#define BM_CISEL_PORT_SHIFT		0
#define BM_CISEL_PORT_SIZE		4
/* --------------------------------------------------- */
/* Register: 'Debug Control Register' */
/* Bit: 'DBG_SEL' */
/* Description: 'Select Signal for Debug Multiplexer' */
#define BM_DEBUG_CTRL_DBG_SEL_OFFSET	0x057
#define BM_DEBUG_CTRL_DBG_SEL_SHIFT		0
#define BM_DEBUG_CTRL_DBG_SEL_SIZE		8
/* --------------------------------------------------- */
/* Register: 'Debug Value Register' */
/* Bit: 'DBG_DAT' */
/* Description: 'Debug Data Value' */
#define BM_DEBUG_VAL_DBG_DAT_OFFSET	0x058
#define BM_DEBUG_VAL_DBG_DAT_SHIFT	0
#define BM_DEBUG_VAL_DBG_DAT_SIZE		16
/* --------------------------------------------------- */
/* Register: 'BM Egress Queue Congestion Notification */
/* Status Register 0'(LTQ_GSWIP_2_2) */
/* Bit: 'EQCGN0' */
/* Description: 'Egress Queue Congestion Notification Queue 15 to 0' */
#define BM_EQCGN0_EQCGN0_OFFSET	0x059
#define BM_EQCGN0_EQCGN0_SHIFT	0
#define BM_EQCGN0_EQCGN0_SIZE		16
/* --------------------------------------------------- */
/* Register: 'BM Egress Queue Congestion Notification Status */
/* Register 1'(LTQ_GSWIP_2_2) */
/* Bit: 'EQCGN1' */
/* Description: 'Egress Queue Congestion Notification Queue 31 to 16' */
#define BM_EQCGN1_EQCGN1_OFFSET	0x05A
#define BM_EQCGN1_EQCGN1_SHIFT	0
#define BM_EQCGN1_EQCGN1_SIZE		16
/* --------------------------------------------------- */
/* Register: 'BM Egress Queue Congestion Notification Status Register 2' */
/* Bit: 'EQCGN2' */
/* Description: 'Egress Queue Congestion Notification Queue 39 to 32' */
/*	 (LTQ_GSWIP_2_2)*/
#define BM_EQCGN2_EQCGN2_OFFSET	0x05B
#define BM_EQCGN2_EQCGN2_SHIFT	0
#define BM_EQCGN2_EQCGN2_SIZE		8
/* --------------------------------------------------- */
/* Register: 'BM WFQ and Shaper Overhead Byte Configuration' */
/* Bit: 'OVERHD' (LTQ_GSWIP_2_2) */
/* Description: 'BM WFQ and Shaper Overhead Byte' */
#define BM_OVERHD_OVERHD_OFFSET	0x061
#define BM_OVERHD_OVERHD_SHIFT	0
#define BM_OVERHD_OVERHD_SIZE		8
/* --------------------------------------------------- */
/* Register: 'BM RMON Global Control Configuration'(LTQ_GSWIP_3_0) */
/* Bit: 'INTMON' */
/* Description: 'Interface RMON Counter Mode' */
#define BM_RMON_GCTRL_INTMON_OFFSET	0x062
#define BM_RMON_GCTRL_INTMON_SHIFT	15
#define BM_RMON_GCTRL_INTMON_SIZE		1
/* Bit: 'MRMON' */
/* Description: 'Meter RMON Counter Mode' */
#define BM_RMON_GCTRL_MRMON_OFFSET	0x062
#define BM_RMON_GCTRL_MRMON_SHIFT		14
#define BM_RMON_GCTRL_MRMON_SIZE		1
/* Bit: 'MRMON' */
/* Description: 'Meter RMON Counter Mode' */
#define BM_RMON_GCTRL_PMAC_RES_OFFSET	0x062
#define BM_RMON_GCTRL_PMAC_RES_SHIFT	12
#define BM_RMON_GCTRL_PMAC_RES_SIZE		1
/* Bit: 'ITFID' */
/* Description: 'Interface ID to be Reset' */
#define BM_RMON_GCTRL_ITFID_OFFSET	0x062
#define BM_RMON_GCTRL_ITFID_SHIFT		4
#define BM_RMON_GCTRL_ITFID_SIZE		8
/* Bit: 'INT_RES' */
/* Description: 'Software Reset for a Single Interface RMON RAM' */
#define BM_RMON_GCTRL_INT_RES_OFFSET	0x062
#define BM_RMON_GCTRL_INT_RES_SHIFT		3
#define BM_RMON_GCTRL_INT_RES_SIZE		1
/* Bit: 'ALLITF_RES' */
/* Description: 'Software Reset for All Interface RMON RAM' */
#define BM_RMON_GCTRL_ALLITF_RES_OFFSET	0x062
#define BM_RMON_GCTRL_ALLITF_RES_SHIFT	2
#define BM_RMON_GCTRL_ALLITF_RES_SIZE		1
/* Bit: 'METER_RES' */
/* Description: 'Software Reset for Meter RMON RAM' */
#define BM_RMON_GCTRL_METER_RES_OFFSET	0x062
#define BM_RMON_GCTRL_METER_RES_SHIFT		1
#define BM_RMON_GCTRL_METER_RES_SIZE		1
/* Bit: 'RED_RES' */
/* Description: ' Software Reset for Redirection RMON RAM' */
#define BM_RMON_GCTRL_RED_RES_OFFSET	0x062
#define BM_RMON_GCTRL_RED_RES_SHIFT		0
#define BM_RMON_GCTRL_RED_RES_SIZE		1
/* --------------------------------------------------- */
/* Register: 'Buffer Manager PortConfiguration Register' */
/* Bit: 'IGCNT' */
/* Description: 'Ingress Special Tag RMON count' */
#define BM_PCFG_IGCNT_OFFSET	0x080
#define BM_PCFG_IGCNT_SHIFT		1
#define BM_PCFG_IGCNT_SIZE		1
/* Bit: 'CNTEN' */
/* Description: 'RMON Counter Enable' */
#define BM_PCFG_CNTEN_OFFSET	0x080
#define BM_PCFG_CNTEN_SHIFT		0
#define BM_PCFG_CNTEN_SIZE		1
/* --------------------------------------------------- */
/* Register: 'Buffer ManagerRMON Control Register' */
/* Bit: 'IFRMONFST' (LTQ_GSWIP_3_0)*/
/* Description: 'Interface RMON Counter First Entry Index' */
#define BM_RMON_CTRL_IFRMONFST_OFFSET	0x081
#define BM_RMON_CTRL_IFRMONFST_SHIFT	8
#define BM_RMON_CTRL_IFRMONFST_SIZE		8
/* Bit: 'IFRMONMD' (LTQ_GSWIP_3_0)*/
/* Description: 'Interface RMON Counter Control' */
#define BM_RMON_CTRL_IFRMONMD_OFFSET	0x081
#define BM_RMON_CTRL_IFRMONMD_SHIFT		6
#define BM_RMON_CTRL_IFRMONMD_SIZE		2
/* Bit: 'ROUT_RES' (LTQ_GSWIP_3_0)*/
/* Description: 'Software Reset for Routing RMON RAM' */
#define BM_RMON_CTRL_ROUT_RES_OFFSET	0x081
#define BM_RMON_CTRL_ROUT_RES_SHIFT		3
#define BM_RMON_CTRL_ROUT_RES_SIZE		1
/* Bit: 'BCAST_CNT' (LTQ_GSWIP_2_2)*/
/* Description: 'Broadcast Counter Control' */
#define BM_RMON_CTRL_BCAST_CNT_OFFSET	0x081
#define BM_RMON_CTRL_BCAST_CNT_SHIFT	2
#define BM_RMON_CTRL_BCAST_CNT_SIZE		1
/* Bit: 'RAM2_RES' */
/* Description: 'Software Reset for RMON RAM2' */
#define BM_RMON_CTRL_RAM2_RES_OFFSET	0x081
#define BM_RMON_CTRL_RAM2_RES_SHIFT		1
#define BM_RMON_CTRL_RAM2_RES_SIZE		1
/* Bit: 'RAM1_RES' */
/* Description: 'Software Reset for RMON RAM1' */
#define BM_RMON_CTRL_RAM1_RES_OFFSET	0x081
#define BM_RMON_CTRL_RAM1_RES_SHIFT		0
#define BM_RMON_CTRL_RAM1_RES_SIZE		1
/* --------------------------------------------------- */
/* Register: 'WRED Port Red Threshold Register 0' (LTQ_GSWIP_2_2) */
/* Bit: 'MINTH' */
/* Description: 'Minimum Threshold' */
#define BM_PWRED_RTH_0_MINTH_OFFSET	0x0A0
#define BM_PWRED_RTH_0_MINTH_SHIFT	0
#define BM_PWRED_RTH_0_MINTH_SIZE		10
/* --------------------------------------------------- */
/* Register: 'WRED Port Red Threshold Register 1' (LTQ_GSWIP_2_2)*/
/* Bit: 'MAXTH' */
/* Description: 'Minimum Threshold' */
#define BM_PWRED_RTH_1_MAXTH_OFFSET	0x0A1
#define BM_PWRED_RTH_1_MAXTH_SHIFT	0
#define BM_PWRED_RTH_1_MAXTH_SIZE		10
/* --------------------------------------------------- */
/* Register: 'WRED Port Yellow Threshold Register 0'(LTQ_GSWIP_2_2) */
/* Bit: 'MINTH' */
/* Description: 'Minimum Threshold' */
#define BM_PWRED_YTH_0_MINTH_OFFSET	0x0A2
#define BM_PWRED_YTH_0_MINTH_SHIFT	0
#define BM_PWRED_YTH_0_MINTH_SIZE		10
/* --------------------------------------------------- */
/* Register: 'WRED Port Yellow Threshold Register 1' (LTQ_GSWIP_2_2)*/
/* Bit: 'MAXTH' */
/* Description: 'Minimum Threshold' */
#define BM_PWRED_YTH_1_MAXTH_OFFSET	0x0A3
#define BM_PWRED_YTH_1_MAXTH_SHIFT	0
#define BM_PWRED_YTH_1_MAXTH_SIZE		10
/* --------------------------------------------------- */
/* Register: 'WRED Port Green Threshold Register 0' (LTQ_GSWIP_2_2)*/
/* Bit: 'MINTH' */
/* Description: 'Minimum Threshold' */
#define BM_PWRED_GTH_0_MINTH_OFFSET	0x0A4
#define BM_PWRED_GTH_0_MINTH_SHIFT	0
#define BM_PWRED_GTH_0_MINTH_SIZE		10
/* --------------------------------------------------- */
/* Register: 'WRED Port Green Threshold Register 1' (LTQ_GSWIP_2_2)*/
/* Bit: 'MAXTH' */
/* Description: 'Minimum Threshold' */
#define BM_PWRED_GTH_1_MAXTH_OFFSET	0x0A5
#define BM_PWRED_GTH_1_MAXTH_SHIFT	0
#define BM_PWRED_GTH_1_MAXTH_SIZE		10
/* --------------------------------------------------- */
/* Register: 'Packet Queue ManagerRate Shaper Assignment Register' */
/* Bit: 'EN2' */
/* Description: 'Rate Shaper 2 Enable' */
#define PQM_RS_EN2_OFFSET	0x101
#define PQM_RS_EN2_SHIFT	15
#define PQM_RS_EN2_SIZE		1
/* Bit: 'RS2' */
/* Description: 'Rate Shaper 2' */
#define PQM_RS_RS2_OFFSET	0x101
#define PQM_RS_RS2_SHIFT	8
#define PQM_RS_RS2_SIZE		4
/* Bit: 'EN1' */
/* Description: 'Rate Shaper 1 Enable' */
#define PQM_RS_EN1_OFFSET	0x101
#define PQM_RS_EN1_SHIFT	7
#define PQM_RS_EN1_SIZE		1
/* Bit: 'RS1' */
/* Description: 'Rate Shaper 1' */
#define PQM_RS_RS1_OFFSET	0x101
#define PQM_RS_RS1_SHIFT	0
#define PQM_RS_RS1_SIZE		4
/* --------------------------------------------------- */
/* Register: 'Rate Shaper ControlRegister' (LTQ_GSWIP_2_2) */
/* Bit: 'RSMOD' */
/* Description: 'Rate Shaper Mode' */
#define RS_CTRL_RSMOD_OFFSET	0x140
#define RS_CTRL_RSMOD_SHIFT		1
#define RS_CTRL_RSMOD_SIZE		1
/* Bit: 'RSEN' */
/* Description: 'Rate Shaper Enable' */
#define RS_CTRL_RSEN_OFFSET		0x140
#define RS_CTRL_RSEN_SHIFT		0
#define RS_CTRL_RSEN_SIZE			1
/* --------------------------------------------------- */
/* Register: 'Rate Shaper CommittedBurst Size Register' */
/* Bit: 'CBS' */
/* Description: 'Committed Burst Size' */
#define RS_CBS_CBS_OFFSET	0x141
#define RS_CBS_CBS_SHIFT	0
#define RS_CBS_CBS_SIZE		10
/* --------------------------------------------------- */
/* Register: 'Rate Shaper InstantaneousBurst Size Register' */
/* Bit: 'IBS' */
/* Description: 'Instantaneous Burst Size' */
#define RS_IBS_IBS_OFFSET	0x142
#define RS_IBS_IBS_SHIFT	0
#define RS_IBS_IBS_SIZE		2
/* --------------------------------------------------- */
/* Register: 'Rate Shaper RateExponent Register' */
/* Bit: 'EXP' */
/* Description: 'Exponent' */
#define RS_CIR_EXP_EXP_OFFSET	0x143
#define RS_CIR_EXP_EXP_SHIFT	0
#define RS_CIR_EXP_EXP_SIZE		4
/* --------------------------------------------------- */
/* Register: 'Rate Shaper RateMantissa Register' */
/* Bit: 'MANT' */
/* Description: 'Mantissa' */
#define RS_CIR_MANT_MANT_OFFSET	0x144
#define RS_CIR_MANT_MANT_SHIFT	0
#define RS_CIR_MANT_MANT_SIZE		10
/* --------------------------------------------------- */
/* Register: 'Table Mask Write Register3' (LTQ_GSWIP_3_0) */
/* Bit: 'MASK3' */
/* Description: 'Mask Pattern [15:0]' */
#define PCE_TBL_MASK_3_MASK3_OFFSET	0x42A
#define PCE_TBL_MASK_3_MASK3_SHIFT	0
#define PCE_TBL_MASK_3_MASK3_SIZE		16
/* --------------------------------------------------- */
/* Register: 'Table Mask Write Register2' (LTQ_GSWIP_3_0) */
/* Bit: 'MASK2' */
/* Description: 'Mask Pattern [15:0]' */
#define PCE_TBL_MASK_2_MASK2_OFFSET	0x42B
#define PCE_TBL_MASK_2_MASK2_SHIFT	0
#define PCE_TBL_MASK_2_MASK2_SIZE		16
/* --------------------------------------------------- */
/* Register: 'Table Mask Write Register1' (LTQ_GSWIP_3_0) */
/* Bit: 'MASK1' */
/* Description: 'Mask Pattern [15:0]' */
#define PCE_TBL_MASK_1_MASK1_OFFSET	0x42C
#define PCE_TBL_MASK_1_MASK1_SHIFT	0
#define PCE_TBL_MASK_1_MASK1_SIZE		16
/* --------------------------------------------------- */
/* Register: 'Table Value Register15' (LTQ_GSWIP_3_0)*/
/* Bit: 'VAL15' */
/* Description: 'Data value [15:0]' */
#define PCE_TBL_VAL_15_VAL15_OFFSET	0x42D
#define PCE_TBL_VAL_15_VAL15_SHIFT	0
#define PCE_TBL_VAL_15_VAL15_SIZE		16
/* --------------------------------------------------- */
/* Register: 'Table Value Register14' (LTQ_GSWIP_3_0)*/
/* Bit: 'VAL14' */
/* Description: 'Data value [15:0]' */
#define PCE_TBL_VAL_14_VAL14_OFFSET	0x42E
#define PCE_TBL_VAL_14_VAL14_SHIFT	0
#define PCE_TBL_VAL_14_VAL14_SIZE		16
/* --------------------------------------------------- */
/* Register: 'Table Value Register13' (LTQ_GSWIP_3_0)*/
/* Bit: 'VAL13' */
/* Description: 'Data value [15:0]' */
#define PCE_TBL_VAL_13_VAL13_OFFSET	0x42F
#define PCE_TBL_VAL_13_VAL13_SHIFT	0
#define PCE_TBL_VAL_13_VAL13_SIZE		16
/* --------------------------------------------------- */
/* Register: 'Table Value Register12' (LTQ_GSWIP_3_0)*/
/* Bit: 'VAL12' */
/* Description: 'Data value [15:0]' */
#define PCE_TBL_VAL_12_VAL12_OFFSET	0x430
#define PCE_TBL_VAL_12_VAL12_SHIFT	0
#define PCE_TBL_VAL_12_VAL12_SIZE		16
/* --------------------------------------------------- */
/* Register: 'Table Value Register11' (LTQ_GSWIP_3_0)*/
/* Bit: 'VAL11' */
/* Description: 'Data value [15:0]' */
#define PCE_TBL_VAL_11_VAL11_OFFSET	0x431
#define PCE_TBL_VAL_11_VAL11_SHIFT	0
#define PCE_TBL_VAL_11_VAL11_SIZE		16
/* --------------------------------------------------- */
/* Register: 'Table Value Register10' (LTQ_GSWIP_3_0)*/
/* Bit: 'VAL10' */
/* Description: 'Data value [15:0]' */
#define PCE_TBL_VAL_10_VAL10_OFFSET	0x432
#define PCE_TBL_VAL_10_VAL10_SHIFT	0
#define PCE_TBL_VAL_10_VAL10_SIZE		16
/* --------------------------------------------------- */
/* Register: 'Table Value Register9' (LTQ_GSWIP_3_0)*/
/* Bit: 'VAL9' */
/* Description: 'Data value [15:0]' */
#define PCE_TBL_VAL_9_VAL9_OFFSET	0x433
#define PCE_TBL_VAL_9_VAL9_SHIFT	0
#define PCE_TBL_VAL_9_VAL9_SIZE		16
/* --------------------------------------------------- */
/* Register: 'Table Value Register8' (LTQ_GSWIP_3_0)*/
/* Bit: 'VAL8' */
/* Description: 'Data value [15:0]' */
#define PCE_TBL_VAL_8_VAL8_OFFSET	0x434
#define PCE_TBL_VAL_8_VAL8_SHIFT	0
#define PCE_TBL_VAL_8_VAL8_SIZE		16
/* --------------------------------------------------- */
/* Register: 'Table Value Register7' (LTQ_GSWIP_3_0)*/
/* Bit: 'VAL7' */
/* Description: 'Data value [15:0]' */
#define PCE_TBL_VAL_7_VAL7_OFFSET	0x435
#define PCE_TBL_VAL_7_VAL7_SHIFT	0
#define PCE_TBL_VAL_7_VAL7_SIZE		16
/* --------------------------------------------------- */
/* Register: 'Table Value Register6' */
/* Bit: 'VAL6' */
/* Description: 'Data value [15:0]' */
#define PCE_TBL_VAL_6_VAL6_OFFSET	0x436
#define PCE_TBL_VAL_6_VAL6_SHIFT	0
#define PCE_TBL_VAL_6_VAL6_SIZE		16
/* --------------------------------------------------- */
/* Register: 'Table Value Register5' */
/* Bit: 'VAL5' */
/* Description: 'Data value [15:0]' */
#define PCE_TBL_VAL_5_VAL5_OFFSET	0x437
#define PCE_TBL_VAL_5_VAL5_SHIFT	0
#define PCE_TBL_VAL_5_VAL5_SIZE		16
/* --------------------------------------------------- */
/* Register: 'Table Key Data 15' */
/* Bit: 'KEY15' */
/* Description: 'Key Value[15:0]' */
#define PCE_TBL_KEY_15_KEY15_OFFSET	0x438
#define PCE_TBL_KEY_15_KEY15_SHIFT	0
#define PCE_TBL_KEY_15_KEY15_SIZE		16
/* --------------------------------------------------- */
/* Register: 'Table Key Data 14' */
/* Bit: 'KEY14' */
/* Description: 'Key Value[15:0]' */
#define PCE_TBL_KEY_14_KEY14_OFFSET	0x439
#define PCE_TBL_KEY_14_KEY14_SHIFT	0
#define PCE_TBL_KEY_14_KEY14_SIZE		16
/* --------------------------------------------------- */
/* Register: 'Table Key Data 13' */
/* Bit: 'KEY13' */
/* Description: 'Key Value[15:0]' */
#define PCE_TBL_KEY_13_KEY13_OFFSET	0x43A
#define PCE_TBL_KEY_13_KEY13_SHIFT	0
#define PCE_TBL_KEY_13_KEY13_SIZE		16
/* --------------------------------------------------- */
/* Register: 'Table Key Data 12' */
/* Bit: 'KEY12' */
/* Description: 'Key Value[15:0]' */
#define PCE_TBL_KEY_12_KEY12_OFFSET	0x43B
#define PCE_TBL_KEY_12_KEY12_SHIFT	0
#define PCE_TBL_KEY_12_KEY12_SIZE		16
/* --------------------------------------------------- */
/* Register: 'Table Key Data 11' */
/* Bit: 'KEY11' */
/* Description: 'Key Value[15:0]' */
#define PCE_TBL_KEY_11_KEY11_OFFSET	0x43C
#define PCE_TBL_KEY_11_KEY11_SHIFT	0
#define PCE_TBL_KEY_11_KEY11_SIZE		16
/* --------------------------------------------------- */
/* Register: 'Table Key Data 10' */
/* Bit: 'KEY10' */
/* Description: 'Key Value[15:0]' */
#define PCE_TBL_KEY_10_KEY10_OFFSET	0x43D
#define PCE_TBL_KEY_10_KEY10_SHIFT	0
#define PCE_TBL_KEY_10_KEY10_SIZE		16
/* --------------------------------------------------- */
/* Register: 'Table Key Data 9' */
/* Bit: 'KEY9' */
/* Description: 'Key Value[15:0]' */
#define PCE_TBL_KEY_9_KEY9_OFFSET	0x43E
#define PCE_TBL_KEY_9_KEY9_SHIFT	0
#define PCE_TBL_KEY_9_KEY9_SIZE		16
/* --------------------------------------------------- */
/* Register: 'Table Key Data 8' */
/* Bit: 'KEY8' */
/* Description: 'Key Value[15:0]' */
#define PCE_TBL_KEY_8_KEY8_OFFSET	0x43F
#define PCE_TBL_KEY_8_KEY8_SHIFT	0
#define PCE_TBL_KEY_8_KEY8_SIZE		16
/* --------------------------------------------------- */
/* Register: 'Table Key Data 7' */
/* Bit: 'KEY7' */
/* Description: 'Key Value[15:0]' */
#define PCE_TBL_KEY_7_KEY7_OFFSET	0x440
#define PCE_TBL_KEY_7_KEY7_SHIFT	0
#define PCE_TBL_KEY_7_KEY7_SIZE		16
/* --------------------------------------------------- */
/* Register: 'Table Key Data 6' */
/* Bit: 'KEY6' */
/* Description: 'Key Value[15:0]' */
#define PCE_TBL_KEY_6_KEY6_OFFSET	0x441
#define PCE_TBL_KEY_6_KEY6_SHIFT	0
#define PCE_TBL_KEY_6_KEY6_SIZE		16
/* --------------------------------------------------- */
/* Register: 'Table Key Data 5' */
/* Bit: 'KEY5' */
/* Description: 'Key Value[15:0]' */
#define PCE_TBL_KEY_5_KEY5_OFFSET	0x442
#define PCE_TBL_KEY_5_KEY5_SHIFT	0
#define PCE_TBL_KEY_5_KEY5_SIZE		16
/* --------------------------------------------------- */
/* Register: 'Table Key Data 4' */
/* Bit: 'KEY4' */
/* Description: 'Key Value[15:0]' */
#define PCE_TBL_KEY_4_KEY4_OFFSET	0x443
#define PCE_TBL_KEY_4_KEY4_SHIFT	0
#define PCE_TBL_KEY_4_KEY4_SIZE		16
/* --------------------------------------------------- */
/* Register: 'Table Key Data 3' */
/* Bit: 'KEY3' */
/* Description: 'Key Value[15:0]' */
#define PCE_TBL_KEY_3_KEY3_OFFSET	0x444
#define PCE_TBL_KEY_3_KEY3_SHIFT	0
#define PCE_TBL_KEY_3_KEY3_SIZE		16
/* --------------------------------------------------- */
/* Register: 'Table Key Data 2' */
/* Bit: 'KEY2' */
/* Description: 'Key Value[15:0]' */
#define PCE_TBL_KEY_2_KEY2_OFFSET	0x445
#define PCE_TBL_KEY_2_KEY2_SHIFT	0
#define PCE_TBL_KEY_2_KEY2_SIZE		16
/* --------------------------------------------------- */
/* Register: 'Table Key Data 1' */
/* Bit: 'KEY1' */
/* Description: 'Key Value[15:0]' */
#define PCE_TBL_KEY_1_KEY1_OFFSET	0x446
#define PCE_TBL_KEY_1_KEY1_SHIFT	0
#define PCE_TBL_KEY_1_KEY1_SIZE		16
/* --------------------------------------------------- */
/* Register: 'Table Key Data 0' */
/* Bit: 'KEY0' */
/* Description: 'Key Value[15:0]' */
#define PCE_TBL_KEY_0_KEY0_OFFSET	0x447
#define PCE_TBL_KEY_0_KEY0_SHIFT	0
#define PCE_TBL_KEY_0_KEY0_SIZE		16
/* --------------------------------------------------- */
/* Register: 'Table Mask Write Register0' */
/* Bit: 'MASK0' */
/* Description: 'Mask Pattern [15:0]' */
#define PCE_TBL_MASK_0_MASK0_OFFSET	0x448
#define PCE_TBL_MASK_0_MASK0_SHIFT	0
#define PCE_TBL_MASK_0_MASK0_SIZE		16
/* --------------------------------------------------- */
/* Register: 'Table Value Register4' */
/* Bit: 'VAL4' */
/* Description: 'Data value [15:0]' */
#define PCE_TBL_VAL_4_VAL4_OFFSET	0x449
#define PCE_TBL_VAL_4_VAL4_SHIFT	0
#define PCE_TBL_VAL_4_VAL4_SIZE		16
/* --------------------------------------------------- */
/* Register: 'Table Value Register3' */
/* Bit: 'VAL3' */
/* Description: 'Data value [15:0]' */
#define PCE_TBL_VAL_3_VAL3_OFFSET	0x44A
#define PCE_TBL_VAL_3_VAL3_SHIFT	0
#define PCE_TBL_VAL_3_VAL3_SIZE		16
/* --------------------------------------------------- */
/* Register: 'Table Value Register2' */
/* Bit: 'VAL2' */
/* Description: 'Data value [15:0]' */
#define PCE_TBL_VAL_2_VAL2_OFFSET	0x44B
#define PCE_TBL_VAL_2_VAL2_SHIFT	0
#define PCE_TBL_VAL_2_VAL2_SIZE		16
/* --------------------------------------------------- */
/* Register: 'Table Value Register1' */
/* Bit: 'VAL1' */
/* Description: 'Data value [15:0]' */
#define PCE_TBL_VAL_1_VAL1_OFFSET	0x44C
#define PCE_TBL_VAL_1_VAL1_SHIFT	0
#define PCE_TBL_VAL_1_VAL1_SIZE		16
/* --------------------------------------------------- */
/* Register: 'Table Value Register0' */
/* Bit: 'VAL0' */
/* Description: 'Data value [15:0]' */
#define PCE_TBL_VAL_0_VAL0_OFFSET	0x44D
#define PCE_TBL_VAL_0_VAL0_SHIFT	0
#define PCE_TBL_VAL_0_VAL0_SIZE		16
/* --------------------------------------------------- */
/* Register: 'Table Entry AddressRegister' */
/* Bit: 'ADDR' */
/* Description: 'Table Address' */
#define PCE_TBL_ADDR_ADDR_OFFSET	0x44E
#define PCE_TBL_ADDR_ADDR_SHIFT		0
#define PCE_TBL_ADDR_ADDR_SIZE		12
/* --------------------------------------------------- */
/* Register: 'Table Access ControlRegister' */
/* Bit: 'BAS' */
/* Description: 'Access Busy/Access Start' */
#define PCE_TBL_CTRL_BAS_OFFSET	0x44F
#define PCE_TBL_CTRL_BAS_SHIFT	15
#define PCE_TBL_CTRL_BAS_SIZE		1
/* Bit: 'EXTOP' */
/* Description: 'Lookup Table Access Extended Operation Mode' */
#define PCE_TBL_CTRL_EXTOP_OFFSET	0x44F
#define PCE_TBL_CTRL_EXTOP_SHIFT	14
#define PCE_TBL_CTRL_EXTOP_SIZE		1
/* Bit: 'TYPE' */
/* Description: 'Lookup Entry Type' */
#define PCE_TBL_CTRL_TYPE_OFFSET	0x44F
#define PCE_TBL_CTRL_TYPE_SHIFT		13
#define PCE_TBL_CTRL_TYPE_SIZE		1
/* Bit: 'VLD' */
/* Description: 'Lookup Entry Valid' */
#define PCE_TBL_CTRL_VLD_OFFSET	0x44F
#define PCE_TBL_CTRL_VLD_SHIFT	12
#define PCE_TBL_CTRL_VLD_SIZE		1
/* Bit: 'KEYFORM' */
/* Description: 'Key Format' */
#define PCE_TBL_CTRL_KEYFORM_OFFSET	0x44F
#define PCE_TBL_CTRL_KEYFORM_SHIFT	11
#define PCE_TBL_CTRL_KEYFORM_SIZE		1
/* Bit: 'GMAP' */
/* Description: 'Group Map' */
#define PCE_TBL_CTRL_GMAP_OFFSET	0x44F
#define PCE_TBL_CTRL_GMAP_SHIFT		7
#define PCE_TBL_CTRL_GMAP_SIZE		4
/* Bit: 'OPMOD' */
/* Description: 'Lookup Table Access Operation Mode' */
#define PCE_TBL_CTRL_OPMOD_OFFSET	0x44F
#define PCE_TBL_CTRL_OPMOD_SHIFT	5
#define PCE_TBL_CTRL_OPMOD_SIZE		2
/* Bit: 'ADDR' */
/* Description: 'Lookup Table Address' */
#define PCE_TBL_CTRL_ADDR_OFFSET	0x44F
#define PCE_TBL_CTRL_ADDR_SHIFT		0
#define PCE_TBL_CTRL_ADDR_SIZE		5
#define PCE_TBL_CTRL_ADDR_OPMOD_SIZE 7
#define PCE_TBL_CTRL_ADDR_REG_SIZE 16
/* --------------------------------------------------- */
/* Register: 'Table General StatusRegister' */
/* Bit: 'TBUSY' */
/* Description: 'Table Access Busy' */
#define PCE_TBL_STAT_TBUSY_OFFSET	0x450
#define PCE_TBL_STAT_TBUSY_SHIFT	2
#define PCE_TBL_STAT_TBUSY_SIZE		1
/* Bit: 'TEMPT' */
/* Description: 'Table Empty' */
#define PCE_TBL_STAT_TEMPT_OFFSET	0x450
#define PCE_TBL_STAT_TEMPT_SHIFT	1
#define PCE_TBL_STAT_TEMPT_SIZE		1
/* Bit: 'TFUL' */
/* Description: 'Table Full' */
#define PCE_TBL_STAT_TFUL_OFFSET	0x450
#define PCE_TBL_STAT_TFUL_SHIFT		0
#define PCE_TBL_STAT_TFUL_SIZE		1
/* --------------------------------------------------- */
/* Register: 'Aging Counter ConfigurationRegister 0' */
/* Bit: 'EXP' */
/* Description: 'Aging Counter Exponent Value ' */
#define PCE_AGE_0_EXP_OFFSET	0x451
#define PCE_AGE_0_EXP_SHIFT		0
#define PCE_AGE_0_EXP_SIZE		4
/* --------------------------------------------------- */
/* Register: 'Aging Counter ConfigurationRegister 1' */
/* Bit: 'MANT' */
/* Description: 'Aging Counter Mantissa Value ' */
#define PCE_AGE_1_MANT_OFFSET	0x452
#define PCE_AGE_1_MANT_SHIFT	0
#define PCE_AGE_1_MANT_SIZE		16
/* --------------------------------------------------- */
/* Register: 'Port Map Register 1' */
/* Bit: 'MPMAP' */
/* Description: 'Monitoring Port Map' */
#define PCE_PMAP_1_MPMAP_OFFSET	0x453
#define PCE_PMAP_1_MPMAP_SHIFT	0
#define PCE_PMAP_1_MPMAP_SIZE		16
/* --------------------------------------------------- */
/* Register: 'Port Map Register 2' */
/* Bit: 'DMCPMAP' */
/* Description: 'Default Multicast Port Map' */
#define PCE_PMAP_2_DMCPMAP_OFFSET	0x454
#define PCE_PMAP_2_DMCPMAP_SHIFT	0
#define PCE_PMAP_2_DMCPMAP_SIZE		16
/* --------------------------------------------------- */
/* Register: 'Port Map Register 3' */
/* Bit: 'UUCMAP' */
/* Description: 'Default Unknown Unicast Port Map' */
#define PCE_PMAP_3_UUCMAP_OFFSET	0x455
#define PCE_PMAP_3_UUCMAP_SHIFT		0
#define PCE_PMAP_3_UUCMAP_SIZE		16
/* --------------------------------------------------- */
/* Register: 'PCE Global Control Register0' */
/* Bit: 'IGMP' */
/* Description: 'IGMP Mode Selection' */
#define PCE_GCTRL_0_IGMP_OFFSET	0x456
#define PCE_GCTRL_0_IGMP_SHIFT	15
#define PCE_GCTRL_0_IGMP_SIZE		1
/* Bit: 'VLAN' */
/* Description: 'VLAN-aware Switching' */
#define PCE_GCTRL_0_VLAN_OFFSET	0x456
#define PCE_GCTRL_0_VLAN_SHIFT	14
#define PCE_GCTRL_0_VLAN_SIZE		1
/* Bit: 'NOPM' */
/* Description: 'No Port Map Forwarding' */
#define PCE_GCTRL_0_NOPM_OFFSET	0x456
#define PCE_GCTRL_0_NOPM_SHIFT	13
#define PCE_GCTRL_0_NOPM_SIZE		1
/* Bit: 'SCONUC' */
/* Description: 'Unknown Unicast Storm Control' */
#define PCE_GCTRL_0_SCONUC_OFFSET	0x456
#define PCE_GCTRL_0_SCONUC_SHIFT	12
#define PCE_GCTRL_0_SCONUC_SIZE		1
/* Bit: 'SCONMC' */
/* Description: 'Multicast Storm Control' */
#define PCE_GCTRL_0_SCONMC_OFFSET	0x456
#define PCE_GCTRL_0_SCONMC_SHIFT	11
#define PCE_GCTRL_0_SCONMC_SIZE		1
/* Bit: 'SCONBC' */
/* Description: 'Broadcast Storm Control' */
#define PCE_GCTRL_0_SCONBC_OFFSET	0x456
#define PCE_GCTRL_0_SCONBC_SHIFT	10
#define PCE_GCTRL_0_SCONBC_SIZE		1
/* Bit: 'SCONMOD' */
/* Description: 'Storm Control Mode' */
#define PCE_GCTRL_0_SCONMOD_OFFSET	0x456
#define PCE_GCTRL_0_SCONMOD_SHIFT		8
#define PCE_GCTRL_0_SCONMOD_SIZE		2
/* Bit: 'SCONMET' */
/* Description: 'Storm Control Metering Instance' */
#define PCE_GCTRL_0_SCONMET_OFFSET	0x456
#define PCE_GCTRL_0_SCONMET_SHIFT		4
#define PCE_GCTRL_0_SCONMET_SIZE		4
/* Bit: 'MC_VALID' */
/* Description: 'Access Request' */
#define PCE_GCTRL_0_MC_VALID_OFFSET	0x456
#define PCE_GCTRL_0_MC_VALID_SHIFT	3
#define PCE_GCTRL_0_MC_VALID_SIZE		1
/* Bit: 'PLCKMOD' */
/* Description: 'Port Lock Mode' */
#define PCE_GCTRL_0_PLCKMOD_OFFSET	0x456
#define PCE_GCTRL_0_PLCKMOD_SHIFT		2
#define PCE_GCTRL_0_PLCKMOD_SIZE		1
/* Bit: 'PLIMMOD' */
/* Description: 'MAC Address Learning Limitation Mode' */
#define PCE_GCTRL_0_PLIMMOD_OFFSET	0x456
#define PCE_GCTRL_0_PLIMMOD_SHIFT		1
#define PCE_GCTRL_0_PLIMMOD_SIZE		1
/* Bit: 'MTFL' */
/* Description: 'MAC Table Flushing' */
#define PCE_GCTRL_0_MTFL_OFFSET	0x456
#define PCE_GCTRL_0_MTFL_SHIFT	0
#define PCE_GCTRL_0_MTFL_SIZE		1
/* --------------------------------------------------- */
/* Register: 'PCE Global Control Register1' */
/* Bit: 'PARSER_DBG' */
/* Description: 'Parser Debug Selection' */
#define PCE_GCTRL_1_PARSER_DBG_OFFSET	0x457
#define PCE_GCTRL_1_PARSER_DBG_SHIFT	14
#define PCE_GCTRL_1_PARSER_DBG_SIZE		2
/* Bit: 'RSCNTMD' */
/* Description: 'Routing Session Counter Mode' (LTQ_GSWIP_3_0) */
#define PCE_GCTRL_1_RSCNTMD_OFFSET	0x457
#define PCE_GCTRL_1_RSCNTMD_SHIFT		11
#define PCE_GCTRL_1_RSCNTMD_SIZE		1
/* Bit: 'MKFIDEN' */
/* Description: 'IP Multicast Forwardng Software Mode FID Enable' */
/**	(LTQ_GSWIP_3_0) */
#define PCE_GCTRL_1_MKFIDEN_OFFSET	0x457
#define PCE_GCTRL_1_MKFIDEN_SHIFT		10
#define PCE_GCTRL_1_MKFIDEN_SIZE		1
/* Bit: 'VLANMD' */
/* Description: 'GSWIP2.2 VLAN Mode' */
#define PCE_GCTRL_1_VLANMD_OFFSET	0x457
#define PCE_GCTRL_1_VLANMD_SHIFT	9
#define PCE_GCTRL_1_VLANMD_SIZE		1
/* Bit: 'UKIPMC' */
/* Description: 'Unknown IP Multicast Forwardng Mode' */
#define PCE_GCTRL_1_UKIPMC_OFFSET	0x457
#define PCE_GCTRL_1_UKIPMC_SHIFT	8
#define PCE_GCTRL_1_UKIPMC_SIZE		1
/* Bit: 'SPFMOD' */
/* Description: 'Port Spoofing Detection Violation Forwarding Mode' */
#define PCE_GCTRL_1_SPFMOD_OFFSET	0x457
#define PCE_GCTRL_1_SPFMOD_SHIFT	7
#define PCE_GCTRL_1_SPFMOD_SIZE		1
/* Bit: 'FLOWPTR_LIST_EN' */
/* Description: 'Flow Pointer List enable' */
#define PCE_GCTRL_1_FLOWPTR_LIST_EN_OFFSET	0x457
#define PCE_GCTRL_1_FLOWPTR_LIST_EN_SHIFT		4
#define PCE_GCTRL_1_FLOWPTR_LIST_EN_SIZE		1
/* Bit: 'MAC_GLOCKMOD' */
/* Description: 'MAC Address Table Lock forwarding mode' */
#define PCE_GCTRL_1_MAC_GLOCKMOD_OFFSET	0x457
#define PCE_GCTRL_1_MAC_GLOCKMOD_SHIFT	3
#define PCE_GCTRL_1_MAC_GLOCKMOD_SIZE		1
/* Bit: 'MAC_GLOCK' */
/* Description: 'MAC Address Table Lock' */
#define PCE_GCTRL_1_MAC_GLOCK_OFFSET	0x457
#define PCE_GCTRL_1_MAC_GLOCK_SHIFT		2
#define PCE_GCTRL_1_MAC_GLOCK_SIZE		1
/* Bit: 'PCE_DIS' */
/* Description: 'PCE Disable after currently processed packet' */
#define PCE_GCTRL_1_PCE_DIS_OFFSET	0x457
#define PCE_GCTRL_1_PCE_DIS_SHIFT		1
#define PCE_GCTRL_1_PCE_DIS_SIZE		1
/* Bit: 'LRNMOD' */
/* Description: 'MAC Address Learning Mode' */
#define PCE_GCTRL_1_LRNMOD_OFFSET	0x457
#define PCE_GCTRL_1_LRNMOD_SHIFT	0
#define PCE_GCTRL_1_LRNMOD_SIZE		1
/* --------------------------------------------------- */
/* Register: 'Three-color MarkerGlobal Control Register' */
/* Bit: 'DPRED' */
/* Description: 'Re-marking Drop Precedence Red Encoding' */
#define PCE_TCM_GLOB_CTRL_DPRED_OFFSET	0x458
#define PCE_TCM_GLOB_CTRL_DPRED_SHIFT		6
#define PCE_TCM_GLOB_CTRL_DPRED_SIZE		3
/* Bit: 'DPYEL' */
/* Description: 'Re-marking Drop Precedence Yellow Encoding' */
#define PCE_TCM_GLOB_CTRL_DPYEL_OFFSET	0x458
#define PCE_TCM_GLOB_CTRL_DPYEL_SHIFT		3
#define PCE_TCM_GLOB_CTRL_DPYEL_SIZE		3
/* Bit: 'DPGRN' */
/* Description: 'Re-marking Drop Precedence Green Encoding' */
#define PCE_TCM_GLOB_CTRL_DPGRN_OFFSET	0x458
#define PCE_TCM_GLOB_CTRL_DPGRN_SHIFT		0
#define PCE_TCM_GLOB_CTRL_DPGRN_SIZE		3
/* --------------------------------------------------- */
/* Register: 'IGMP Control Register' */
/* Bit: 'FAGEEN' */
/* Description: 'Force Aging of Table Entries Enable' */
#define PCE_IGMP_CTRL_FAGEEN_OFFSET	0x459
#define PCE_IGMP_CTRL_FAGEEN_SHIFT	15
#define PCE_IGMP_CTRL_FAGEEN_SIZE		1
/* Bit: 'FLEAVE' */
/* Description: 'Fast Leave Enable' */
#define PCE_IGMP_CTRL_FLEAVE_OFFSET	0x459
#define PCE_IGMP_CTRL_FLEAVE_SHIFT	14
#define PCE_IGMP_CTRL_FLEAVE_SIZE		1
/* Bit: 'DMRTEN' */
/* Description: 'Default Maximum Response Time Enable' */
#define PCE_IGMP_CTRL_DMRTEN_OFFSET	0x459
#define PCE_IGMP_CTRL_DMRTEN_SHIFT	13
#define PCE_IGMP_CTRL_DMRTEN_SIZE		1
/* Bit: 'JASUP' */
/* Description: 'Join Aggregation Suppression Enable' */
#define PCE_IGMP_CTRL_JASUP_OFFSET	0x459
#define PCE_IGMP_CTRL_JASUP_SHIFT		12
#define PCE_IGMP_CTRL_JASUP_SIZE		1
/* Bit: 'REPSUP' */
/* Description: 'Report Suppression Enable' */
#define PCE_IGMP_CTRL_REPSUP_OFFSET	0x459
#define PCE_IGMP_CTRL_REPSUP_SHIFT	11
#define PCE_IGMP_CTRL_REPSUP_SIZE		1
/* Bit: 'SRPEN' */
/* Description: 'Snooping of Router Port Enable' */
#define PCE_IGMP_CTRL_SRPEN_OFFSET	0x459
#define PCE_IGMP_CTRL_SRPEN_SHIFT		10
#define PCE_IGMP_CTRL_SRPEN_SIZE		1
/* Bit: 'ROB' */
/* Description: 'Robustness Variable' */
#define PCE_IGMP_CTRL_ROB_OFFSET	0x459
#define PCE_IGMP_CTRL_ROB_SHIFT		8
#define PCE_IGMP_CTRL_ROB_SIZE		2
/* Bit: 'DMRT' */
/* Description: 'IGMP Default Maximum Response Time' */
#define PCE_IGMP_CTRL_DMRT_OFFSET	0x459
#define PCE_IGMP_CTRL_DMRT_SHIFT	0
#define PCE_IGMP_CTRL_DMRT_SIZE		8
/* --------------------------------------------------- */
/* Register: 'IGMP Default RouterPort Map Register' */
/* Bit: 'DRPM' */
/* Description: 'IGMP Default Router Port Map' */
#define PCE_IGMP_DRPM_DRPM_OFFSET	0x45A
#define PCE_IGMP_DRPM_DRPM_SHIFT	0
#define PCE_IGMP_DRPM_DRPM_SIZE		16
/* --------------------------------------------- */
/* Register: 'IGMP Aging Register0' */
/* Bit: 'MANT' */
/* Description: 'IGMP Group Aging Time Mantissa' */
#define PCE_IGMP_AGE_0_MANT_OFFSET	0x45B
#define PCE_IGMP_AGE_0_MANT_SHIFT		3
#define PCE_IGMP_AGE_0_MANT_SIZE		8
/* Bit: 'EXP' */
/* Description: 'IGMP Group Aging Time Exponent' */
#define PCE_IGMP_AGE_0_EXP_OFFSET	0x45B
#define PCE_IGMP_AGE_0_EXP_SHIFT	0
#define PCE_IGMP_AGE_0_EXP_SIZE		3
/* --------------------------------------------------- */
/* Register: 'IGMP Aging Register1' */
/* Bit: 'MANT' */
/* Description: 'IGMP Router Port Aging Time Mantissa' */
#define PCE_IGMP_AGE_1_MANT_OFFSET	0x45C
#define PCE_IGMP_AGE_1_MANT_SHIFT		0
#define PCE_IGMP_AGE_1_MANT_SIZE		12
/* --------------------------------------------------- */
/* Register: 'IGMP Status Register' */
/* Bit: 'IGPM' */
/* Description: 'IGMP Port Map' */
#define PCE_IGMP_STAT_IGPM_OFFSET	0x45D
#define PCE_IGMP_STAT_IGPM_SHIFT	0
#define PCE_IGMP_STAT_IGPM_SIZE		16
/* --------------------------------------------------- */
/* Register: 'Wake-on-LAN ControlRegister' */
/* Bit: 'PASSEN' */
/* Description: 'WoL Password Enable' */
#define WOL_GLB_CTRL_PASSEN_OFFSET	0x45E
#define WOL_GLB_CTRL_PASSEN_SHIFT		0
#define WOL_GLB_CTRL_PASSEN_SIZE		1
/* --------------------------------------------------- */
/* Register: 'Wake-on-LAN DestinationAddress Register 0' */
/* Bit: 'DA0' */
/* Description: 'WoL Destination Address [15:0]' */
#define WOL_DA_0_DA0_OFFSET	0x45F
#define WOL_DA_0_DA0_SHIFT	0
#define WOL_DA_0_DA0_SIZE		16
/* --------------------------------------------------- */
/* Register: 'Wake-on-LAN DestinationAddress Register 1' */
/* Bit: 'DA1' */
/* Description: 'WoL Destination Address [31:16]' */
#define WOL_DA_1_DA1_OFFSET	0x460
#define WOL_DA_1_DA1_SHIFT	0
#define WOL_DA_1_DA1_SIZE		16
/* --------------------------------------------------- */
/* Register: 'Wake-on-LAN DestinationAddress Register 2' */
/* Bit: 'DA2' */
/* Description: 'WoL Destination Address [47:32]' */
#define WOL_DA_2_DA2_OFFSET	0x461
#define WOL_DA_2_DA2_SHIFT	0
#define WOL_DA_2_DA2_SIZE		16
/* --------------------------------------------------- */
/* Register: 'Wake-on-LAN Password Register0' */
/* Bit: 'PW0' */
/* Description: 'WoL Password [15:0]' */
#define WOL_PW_0_PW0_OFFSET	0x462
#define WOL_PW_0_PW0_SHIFT	0
#define WOL_PW_0_PW0_SIZE		16
/* --------------------------------------------------- */
/* Register: 'Wake-on-LAN Password Register1' */
/* Bit: 'PW1' */
/* Description: 'WoL Password [31:16]' */
#define WOL_PW_1_PW1_OFFSET	0x463
#define WOL_PW_1_PW1_SHIFT	0
#define WOL_PW_1_PW1_SIZE		16
/* --------------------------------------------------- */
/* Register: 'Wake-on-LAN Password Register2' */
/* Bit: 'PW2' */
/* Description: 'WoL Password [47:32]' */
#define WOL_PW_2_PW2_OFFSET	0x464
#define WOL_PW_2_PW2_SHIFT	0
#define WOL_PW_2_PW2_SIZE		16
/* --------------------------------------------------- */
/* Register: 'Parser and ClassificationEngine Global */
/* Interrupt Enable Register 0' */
/* Bit: 'PINT_15' */
/* Description: 'Port Interrupt Enable' */
#define PCE_IER_0_PINT_15_OFFSET	0x465
#define PCE_IER_0_PINT_15_SHIFT		15
#define PCE_IER_0_PINT_15_SIZE		1
/* Bit: 'PINT_14' */
/* Description: 'Port Interrupt Enable' */
#define PCE_IER_0_PINT_14_OFFSET	0x465
#define PCE_IER_0_PINT_14_SHIFT		14
#define PCE_IER_0_PINT_14_SIZE		1
/* Bit: 'PINT_13' */
/* Description: 'Port Interrupt Enable' */
#define PCE_IER_0_PINT_13_OFFSET	0x465
#define PCE_IER_0_PINT_13_SHIFT		13
#define PCE_IER_0_PINT_13_SIZE		1
/* Bit: 'PINT_12' */
/* Description: 'Port Interrupt Enable' */
#define PCE_IER_0_PINT_12_OFFSET	0x465
#define PCE_IER_0_PINT_12_SHIFT		12
#define PCE_IER_0_PINT_12_SIZE		1
/* Bit: 'PINT_11' */
/* Description: 'Port Interrupt Enable' */
#define PCE_IER_0_PINT_11_OFFSET	0x465
#define PCE_IER_0_PINT_11_SHIFT		11
#define PCE_IER_0_PINT_11_SIZE		1
/* Bit: 'PINT_10' */
/* Description: 'Port Interrupt Enable' */
#define PCE_IER_0_PINT_10_OFFSET	0x465
#define PCE_IER_0_PINT_10_SHIFT		10
#define PCE_IER_0_PINT_10_SIZE		1
/* Bit: 'PINT_9' */
/* Description: 'Port Interrupt Enable' */
#define PCE_IER_0_PINT_9_OFFSET	0x465
#define PCE_IER_0_PINT_9_SHIFT	9
#define PCE_IER_0_PINT_9_SIZE		1
/* Bit: 'PINT_8' */
/* Description: 'Port Interrupt Enable' */
#define PCE_IER_0_PINT_8_OFFSET	0x465
#define PCE_IER_0_PINT_8_SHIFT	8
#define PCE_IER_0_PINT_8_SIZE		1
/* Bit: 'PINT_7' */
/* Description: 'Port Interrupt Enable' */
#define PCE_IER_0_PINT_7_OFFSET	0x465
#define PCE_IER_0_PINT_7_SHIFT	7
#define PCE_IER_0_PINT_7_SIZE		1
/* Bit: 'PINT_6' */
/* Description: 'Port Interrupt Enable' */
#define PCE_IER_0_PINT_6_OFFSET	0x465
#define PCE_IER_0_PINT_6_SHIFT	6
#define PCE_IER_0_PINT_6_SIZE		1
/* Bit: 'PINT_5' */
/* Description: 'Port Interrupt Enable' */
#define PCE_IER_0_PINT_5_OFFSET	0x465
#define PCE_IER_0_PINT_5_SHIFT	5
#define PCE_IER_0_PINT_5_SIZE		1
/* Bit: 'PINT_4' */
/* Description: 'Port Interrupt Enable' */
#define PCE_IER_0_PINT_4_OFFSET	0x465
#define PCE_IER_0_PINT_4_SHIFT	4
#define PCE_IER_0_PINT_4_SIZE		1
/* Bit: 'PINT_3' */
/* Description: 'Port Interrupt Enable' */
#define PCE_IER_0_PINT_3_OFFSET	0x465
#define PCE_IER_0_PINT_3_SHIFT	3
#define PCE_IER_0_PINT_3_SIZE		1
/* Bit: 'PINT_2' */
/* Description: 'Port Interrupt Enable' */
#define PCE_IER_0_PINT_2_OFFSET	0x465
#define PCE_IER_0_PINT_2_SHIFT	2
#define PCE_IER_0_PINT_2_SIZE		1
/* Bit: 'PINT_1' */
/* Description: 'Port Interrupt Enable' */
#define PCE_IER_0_PINT_1_OFFSET	0x465
#define PCE_IER_0_PINT_1_SHIFT	1
#define PCE_IER_0_PINT_1_SIZE		1
/* Bit: 'PINT_0' */
/* Description: 'Port Interrupt Enable' */
#define PCE_IER_0_PINT_0_OFFSET	0x465
#define PCE_IER_0_PINT_0_SHIFT	0
#define PCE_IER_0_PINT_0_SIZE		1
/* --------------------------------------------------- */
/*Register: 'Parser and ClassificationEngine Global Interrupt */
/* Enable Register 1' */
/* Bit: 'CHG' */
/* Description: 'MAC Table Changed Entry Interrupt Enable' */
#define PCE_IER_1_CHG_OFFSET	0x466
#define PCE_IER_1_CHG_SHIFT		7
#define PCE_IER_1_CHG_SIZE		1
/* Bit: 'FLOWINT' */
/* Description: 'Traffic Flow Table Interrupt Rule matched Interrupt */
/* Enable' */
#define PCE_IER_1_FLOWINT_OFFSET	0x466
#define PCE_IER_1_FLOWINT_SHIFT		6
#define PCE_IER_1_FLOWINT_SIZE		1
/* Bit: 'CPH2' */
/* Description: 'Classification Phase 2 Ready Interrupt Enable' */
#define PCE_IER_1_CPH2_OFFSET	0x466
#define PCE_IER_1_CPH2_SHIFT	5
#define PCE_IER_1_CPH2_SIZE		1
/* Bit: 'CPH1' */
/* Description: 'Classification Phase 1 Ready Interrupt Enable' */
#define PCE_IER_1_CPH1_OFFSET	0x466
#define PCE_IER_1_CPH1_SHIFT	4
#define PCE_IER_1_CPH1_SIZE		1
/* Bit: 'CPH0' */
/* Description: 'Classification Phase 0 Ready Interrupt Enable' */
#define PCE_IER_1_CPH0_OFFSET	0x466
#define PCE_IER_1_CPH0_SHIFT	3
#define PCE_IER_1_CPH0_SIZE		1
/* Bit: 'PRDY' */
/* Description: 'Parser Ready Interrupt Enable' */
#define PCE_IER_1_PRDY_OFFSET	0x466
#define PCE_IER_1_PRDY_SHIFT	2
#define PCE_IER_1_PRDY_SIZE		1
/* Bit: 'IGTF' */
/* Description: 'IGMP Table Full Interrupt Enable' */
#define PCE_IER_1_IGTF_OFFSET	0x466
#define PCE_IER_1_IGTF_SHIFT	1
#define PCE_IER_1_IGTF_SIZE		1
/* Bit: 'MTF' */
/* Description: 'MAC Table Full Interrupt Enable' */
#define PCE_IER_1_MTF_OFFSET	0x466
#define PCE_IER_1_MTF_SHIFT		0
#define PCE_IER_1_MTF_SIZE		1
/* --------------------------------------------------- */
/* Register: 'Parser and ClassificationEngine Global */
/* Interrupt Status Register 0' */
/* Bit: 'PINT_15' */
/* Description: 'Port Interrupt' */
#define PCE_ISR_0_PINT_15_OFFSET	0x467
#define PCE_ISR_0_PINT_15_SHIFT		15
#define PCE_ISR_0_PINT_15_SIZE		1
/* Bit: 'PINT_14' */
/* Description: 'Port Interrupt' */
#define PCE_ISR_0_PINT_14_OFFSET	0x467
#define PCE_ISR_0_PINT_14_SHIFT		14
#define PCE_ISR_0_PINT_14_SIZE		1
/* Bit: 'PINT_13' */
/* Description: 'Port Interrupt' */
#define PCE_ISR_0_PINT_13_OFFSET	0x467
#define PCE_ISR_0_PINT_13_SHIFT		13
#define PCE_ISR_0_PINT_13_SIZE		1
/* Bit: 'PINT_12' */
/* Description: 'Port Interrupt' */
#define PCE_ISR_0_PINT_12_OFFSET	0x467
#define PCE_ISR_0_PINT_12_SHIFT		12
#define PCE_ISR_0_PINT_12_SIZE		1
/* Bit: 'PINT_11' */
/* Description: 'Port Interrupt' */
#define PCE_ISR_0_PINT_11_OFFSET	0x467
#define PCE_ISR_0_PINT_11_SHIFT		11
#define PCE_ISR_0_PINT_11_SIZE		1
/* Bit: 'PINT_10' */
/* Description: 'Port Interrupt' */
#define PCE_ISR_0_PINT_10_OFFSET	0x467
#define PCE_ISR_0_PINT_10_SHIFT		10
#define PCE_ISR_0_PINT_10_SIZE		1
/* Bit: 'PINT_9' */
/* Description: 'Port Interrupt' */
#define PCE_ISR_0_PINT_9_OFFSET	0x467
#define PCE_ISR_0_PINT_9_SHIFT	9
#define PCE_ISR_0_PINT_9_SIZE		1
/* Bit: 'PINT_8' */
/* Description: 'Port Interrupt' */
#define PCE_ISR_0_PINT_8_OFFSET	0x467
#define PCE_ISR_0_PINT_8_SHIFT	8
#define PCE_ISR_0_PINT_8_SIZE		1
/* Bit: 'PINT_7' */
/* Description: 'Port Interrupt' */
#define PCE_ISR_0_PINT_7_OFFSET	0x467
#define PCE_ISR_0_PINT_7_SHIFT	7
#define PCE_ISR_0_PINT_7_SIZE		1
/* Bit: 'PINT_6' */
/* Description: 'Port Interrupt' */
#define PCE_ISR_0_PINT_6_OFFSET	0x467
#define PCE_ISR_0_PINT_6_SHIFT	6
#define PCE_ISR_0_PINT_6_SIZE		1
/* Bit: 'PINT_5' */
/* Description: 'Port Interrupt' */
#define PCE_ISR_0_PINT_5_OFFSET	0x467
#define PCE_ISR_0_PINT_5_SHIFT	5
#define PCE_ISR_0_PINT_5_SIZE		1
/* Bit: 'PINT_4' */
/* Description: 'Port Interrupt' */
#define PCE_ISR_0_PINT_4_OFFSET	0x467
#define PCE_ISR_0_PINT_4_SHIFT	4
#define PCE_ISR_0_PINT_4_SIZE		1
/* Bit: 'PINT_3' */
/* Description: 'Port Interrupt' */
#define PCE_ISR_0_PINT_3_OFFSET	0x467
#define PCE_ISR_0_PINT_3_SHIFT	3
#define PCE_ISR_0_PINT_3_SIZE		1
/* Bit: 'PINT_2' */
/* Description: 'Port Interrupt' */
#define PCE_ISR_0_PINT_2_OFFSET	0x467
#define PCE_ISR_0_PINT_2_SHIFT	2
#define PCE_ISR_0_PINT_2_SIZE		1
/* Bit: 'PINT_1' */
/* Description: 'Port Interrupt' */
#define PCE_ISR_0_PINT_1_OFFSET	0x467
#define PCE_ISR_0_PINT_1_SHIFT	1
#define PCE_ISR_0_PINT_1_SIZE		1
/* Bit: 'PINT_0' */
/* Description: 'Port Interrupt' */
#define PCE_ISR_0_PINT_0_OFFSET	0x467
#define PCE_ISR_0_PINT_0_SHIFT	0
#define PCE_ISR_0_PINT_0_SIZE		1
/* --------------------------------------------------- */
/* Register: 'Parser and ClassificationEngine Global */
/* Interrupt Status Register 1' */
/* Bit: 'CHG' */
/* Description: 'MAC Table Changed Entry Interrupt' */
#define PCE_ISR_1_CHG_OFFSET	0x468
#define PCE_ISR_1_CHG_SHIFT		7
#define PCE_ISR_1_CHG_SIZE		1
/* Bit: 'FLOWINT' */
/* Description: 'Traffic Flow Table Interrupt Rule matched' */
#define PCE_ISR_1_FLOWINT_OFFSET	0x468
#define PCE_ISR_1_FLOWINT_SHIFT		6
#define PCE_ISR_1_FLOWINT_SIZE		1
/* Bit: 'CPH2' */
/* Description: 'Classification Phase 2 Ready Interrupt' */
#define PCE_ISR_1_CPH2_OFFSET	0x468
#define PCE_ISR_1_CPH2_SHIFT	5
#define PCE_ISR_1_CPH2_SIZE		1
/* Bit: 'CPH1' */
/* Description: 'Classification Phase 1 Ready Interrupt' */
#define PCE_ISR_1_CPH1_OFFSET	0x468
#define PCE_ISR_1_CPH1_SHIFT	4
#define PCE_ISR_1_CPH1_SIZE		1
/* Bit: 'CPH0' */
/* Description: 'Classification Phase 0 Ready Interrupt' */
#define PCE_ISR_1_CPH0_OFFSET	0x468
#define PCE_ISR_1_CPH0_SHIFT	3
#define PCE_ISR_1_CPH0_SIZE		1
/* Bit: 'PRDY' */
/* Description: 'Parser Ready Interrupt' */
#define PCE_ISR_1_PRDY_OFFSET	0x468
#define PCE_ISR_1_PRDY_SHIFT	2
#define PCE_ISR_1_PRDY_SIZE		1
/* Bit: 'IGTF' */
/* Description: 'IGMP Table Full Interrupt' */
#define PCE_ISR_1_IGTF_OFFSET	0x468
#define PCE_ISR_1_IGTF_SHIFT	1
#define PCE_ISR_1_IGTF_SIZE		1
/* Bit: 'MTF' */
/* Description: 'MAC Table Full Interrupt' */
#define PCE_ISR_1_MTF_OFFSET	0x468
#define PCE_ISR_1_MTF_SHIFT		0
#define PCE_ISR_1_MTF_SIZE		1
/* --------------------------------------------------- */
/* Register: 'Parser Status Register' */
/* Bit: 'FSM_DAT_CNT' */
/* Description: 'Parser FSM Data Counter' */
#define PARSER_STAT_FSM_DAT_CNT_OFFSET	0x469
#define PARSER_STAT_FSM_DAT_CNT_SHIFT		8
#define PARSER_STAT_FSM_DAT_CNT_SIZE		8
/* Bit: 'FSM_STATE' */
/* Description: 'Parser FSM State' */
#define PARSER_STAT_FSM_STATE_OFFSET	0x469
#define PARSER_STAT_FSM_STATE_SHIFT		5
#define PARSER_STAT_FSM_STATE_SIZE		3
/* Bit: 'PKT_ERR' */
/* Description: 'Packet error detected' */
#define PARSER_STAT_PKT_ERR_OFFSET	0x469
#define PARSER_STAT_PKT_ERR_SHIFT		4
#define PARSER_STAT_PKT_ERR_SIZE		1
/* Bit: 'FSM_FIN' */
/* Description: 'Parser FSM finished' */
#define PARSER_STAT_FSM_FIN_OFFSET	0x469
#define PARSER_STAT_FSM_FIN_SHIFT		3
#define PARSER_STAT_FSM_FIN_SIZE		1
/* Bit: 'FSM_START' */
/* Description: 'Parser FSM start' */
#define PARSER_STAT_FSM_START_OFFSET	0x469
#define PARSER_STAT_FSM_START_SHIFT		2
#define PARSER_STAT_FSM_START_SIZE		1
/* Bit: 'FIFO_RDY' */
/* Description: 'Parser FIFO ready for read.' */
#define PARSER_STAT_FIFO_RDY_OFFSET	0x469
#define PARSER_STAT_FIFO_RDY_SHIFT	1
#define PARSER_STAT_FIFO_RDY_SIZE		1
/* Bit: 'FIFO_FULL' */
/* Description: 'Parser's FIFO full' */
#define PARSER_STAT_FIFO_FULL_OFFSET	0x469
#define PARSER_STAT_FIFO_FULL_SHIFT		0
#define PARSER_STAT_FIFO_FULL_SIZE		1
/* --------------------------------------------------- */
/* Register: 'PCE Changed MAC Entry Number' */
/* Bit: 'NUM' */
/* Description: 'PCE Changed MAC Entry Number' */
#define PCE_CGDMAC_NUM_OFFSET		0x46A
#define PCE_CGDMAC_NUM_SHIFT		0
#define PCE_CGDMAC_NUM_SIZE			12
/* --------------------------------------------------- */
/* Register: 'PCE Trunking Distribution Number' (LTQ_GSWIP_2_2)*/
/* Bit: 'DIP' */
/* Description: 'Destination IP Mask' */
#define PCE_TRUNK_CONF_DIP_OFFSET	0x46B
#define PCE_TRUNK_CONF_DIP_SHIFT	3
#define PCE_TRUNK_CONF_DIP_SIZE		1
/* Bit: 'SIP' */
/* Description: 'Source IP Mask' */
#define PCE_TRUNK_CONF_SIP_OFFSET	0x46B
#define PCE_TRUNK_CONF_SIP_SHIFT	2
#define PCE_TRUNK_CONF_SIP_SIZE		1
/* Bit: 'DA' */
/* Description: 'Destination MAC Mask' */
#define PCE_TRUNK_CONF_DA_OFFSET	0x46B
#define PCE_TRUNK_CONF_DA_SHIFT		1
#define PCE_TRUNK_CONF_DA_SIZE		1
/* Bit: 'SA' */
/* Description: 'Source MAC Mask' */
#define PCE_TRUNK_CONF_SA_OFFSET	0x46B
#define PCE_TRUNK_CONF_SA_SHIFT		0
#define PCE_TRUNK_CONF_SA_SIZE		1
/* --------------------------------------------------- */
/* Register: 'PCE Meter Shaper Overhead Byte Configuration' (LTQ_GSWIP_2_2)*/
/* Bit: 'OVERHD' */
/* Description: 'PCE Meter Overhead Byte' */
#define PCE_OVERHD_OVERHD_OFFSET	0x46C
#define PCE_OVERHD_OVERHD_SHIFT		0
#define PCE_OVERHD_OVERHD_SIZE		8
/* --------------------------------------------------- */
/* Register: 'Routing Table Access Control Register' (LTQ_GSWIP_3_0)*/
/* Bit: 'BAS' */
/* Description: 'Access Busy/Access Start' */
#define PCE_RTBL_CTRL_BAS_OFFSET	0x46D
#define PCE_RTBL_CTRL_BAS_SHIFT		15
#define PCE_RTBL_CTRL_BAS_SIZE		1
/* Bit: 'VLD' */
/* Description: 'Lookup Entry Valid' */
#define PCE_RTBL_CTRL_VLD_OFFSET	0x46D
#define PCE_RTBL_CTRL_VLD_SHIFT		12
#define PCE_RTBL_CTRL_VLD_SIZE		1
/* Bit: 'OPMOD' */
/* Description: 'Lookup Table Access Operation Mode' */
#define PCE_RTBL_CTRL_OPMOD_OFFSET	0x46D
#define PCE_RTBL_CTRL_OPMOD_SHIFT		5
#define PCE_RTBL_CTRL_OPMOD_SIZE		2
/* Bit: 'ADDR' */
/* Description: 'Lookup Table Address' */
#define PCE_RTBL_CTRL_ADDR_OFFSET	0x46D
#define PCE_RTBL_CTRL_ADDR_SHIFT	0
#define PCE_RTBL_CTRL_ADDR_SIZE		5
/* --------------------------------------------------- */
/* Register: 'PCE Flow Engine Common Region Start Index Register' */
/**	(LTQ_GSWIP_3_0)*/
/* Bit: 'INDEX' */
/* Description: 'PCE Flow Engine Common Region Start Index ' */
#define PCE_TFCR_ID_INDEX_OFFSET	0x46E
#define PCE_TFCR_ID_INDEX_SHIFT		2
#define PCE_TFCR_ID_INDEX_SIZE		7
/* --------------------------------------------------- */
/* Register: 'PCE Flow Engine Common Region Entry Numbers Register' */
/**	(LTQ_GSWIP_3_0)*/
/* Bit: 'NUM' */
/* Description: 'PCE Flow Engine Common Region Entry Numbers ' */
#define PCE_TFCR_NUM_NUM_OFFSET	0x46F
#define PCE_TFCR_NUM_NUM_SHIFT	0
#define PCE_TFCR_NUM_NUM_SIZE		8
/* --------------------------------------------------- */
/* Register: 'PCE CPU Meter Control Register'(LTQ_GSWIP_3_0)*/
/* Bit: 'REQ' */
/* Description: 'Meter Trigger Request' */
#define PCE_CPUMETER_CTRL_REQ_OFFSET	0x470
#define PCE_CPUMETER_CTRL_REQ_SHIFT		15
#define PCE_CPUMETER_CTRL_REQ_SIZE		1
/* Bit: 'MT1EN' */
/* Description: 'Metering Instance 1 Enable' */
#define PCE_CPUMETER_CTRL_MT1EN_OFFSET	0x470
#define PCE_CPUMETER_CTRL_MT1EN_SHIFT		14
#define PCE_CPUMETER_CTRL_MT1EN_SIZE		1
/* Bit: 'MT0EN' */
/* Description: 'Metering Instance 0 Enable' */
#define PCE_CPUMETER_CTRL_MT0EN_OFFSET	0x470
#define PCE_CPUMETER_CTRL_MT0EN_SHIFT		13
#define PCE_CPUMETER_CTRL_MT0EN_SIZE		1
/* Bit: 'AFTCOL' */
/* Description: 'Packet Color After Metering' */
#define PCE_CPUMETER_CTRL_AFTCOL_OFFSET	0x470
#define PCE_CPUMETER_CTRL_AFTCOL_SHIFT	2
#define PCE_CPUMETER_CTRL_AFTCOL_SIZE		2
/* Bit: 'PRECOL' */
/* Description: 'Packet Color Before Metering' */
#define PCE_CPUMETER_CTRL_PRECOL_OFFSET	0x470
#define PCE_CPUMETER_CTRL_PRECOL_SHIFT	0
#define PCE_CPUMETER_CTRL_PRECOL_SIZE		2
/* --------------------------------------------------- */
/* Register: 'PCE CPU Meter Size Register'(LTQ_GSWIP_3_0)*/
/* Bit: 'SIZE' */
/* Description: 'Packet Size' */
#define PCE_CPUMETER_SIZE_SIZE_OFFSET	0x471
#define PCE_CPUMETER_SIZE_SIZE_SHIFT	0
#define PCE_CPUMETER_SIZE_SIZE_SIZE		14
/* --------------------------------------------------- */
/* Register: 'PCE CPU Meter Instance 0 ID Register'(LTQ_GSWIP_3_0)*/
/* Bit: 'MID' */
/* Description: 'Meter Instance 0 ID' */
#define PCE_CPUMETER_MID0_MID_OFFSET	0x472
#define PCE_CPUMETER_MID0_MID_SHIFT		0
#define PCE_CPUMETER_MID0_MID_SIZE		6
/* --------------------------------------------------- */
/* Register: 'PCE CPU Meter Instance 1 ID Register'(LTQ_GSWIP_3_0)*/
/* Bit: 'MID' */
/* Description: 'Meter Instance 1 ID' */
#define PCE_CPUMETER_MID1_MID_OFFSET	0x473
#define PCE_CPUMETER_MID1_MID_SHIFT		0
#define PCE_CPUMETER_MID1_MID_SIZE		6
/* --------------------------------------------------- */
/* Register: 'PCE MPE Meter Control Register'(LTQ_GSWIP_3_0)*/
/* Bit: 'REQ' */
/* Description: 'Meter Trigger Request' */
#define PCE_MPEMETER_CTRL_REQ_OFFSET	0x478
#define PCE_MPEMETER_CTRL_REQ_SHIFT		15
#define PCE_MPEMETER_CTRL_REQ_SIZE		1
/* Bit: 'MT1EN' */
/* Description: 'Metering Instance 1 Enable' */
#define PCE_MPEMETER_CTRL_MT1EN_OFFSET	0x478
#define PCE_MPEMETER_CTRL_MT1EN_SHIFT		14
#define PCE_MPEMETER_CTRL_MT1EN_SIZE		1
/* Bit: 'MT0EN' */
/* Description: 'Metering Instance 0 Enable' */
#define PCE_MPEMETER_CTRL_MT0EN_OFFSET	0x478
#define PCE_MPEMETER_CTRL_MT0EN_SHIFT		13
#define PCE_MPEMETER_CTRL_MT0EN_SIZE		1
/* Bit: 'AFTCOL' */
/* Description: 'Packet Color After Metering' */
#define PCE_MPEMETER_CTRL_AFTCOL_OFFSET	0x478
#define PCE_MPEMETER_CTRL_AFTCOL_SHIFT	2
#define PCE_MPEMETER_CTRL_AFTCOL_SIZE		2
/* Bit: 'PRECOL' */
/* Description: 'Packet Color Before Metering' */
#define PCE_MPEMETER_CTRL_PRECOL_OFFSET	0x478
#define PCE_MPEMETER_CTRL_PRECOL_SHIFT	0
#define PCE_MPEMETER_CTRL_PRECOL_SIZE		2
/* --------------------------------------------------- */
/* Register: 'PCE MPE Meter Size Register'(LTQ_GSWIP_3_0)*/
/* Bit: 'SIZE' */
/* Description: 'Packet Size' */
#define PCE_MPEMETER_SIZE_SIZE_OFFSET	0x479
#define PCE_MPEMETER_SIZE_SIZE_SHIFT	0
#define PCE_MPEMETER_SIZE_SIZE_SIZE		14
/* --------------------------------------------------- */
/* Register: 'PCE MPE Meter Instance 0 ID Register'(LTQ_GSWIP_3_0)*/
/* Bit: 'MID' */
/* Description: 'Meter Instance 0 ID' */
#define PCE_MPEMETER_MID0_MID_OFFSET	0x47A
#define PCE_MPEMETER_MID0_MID_SHIFT		0
#define PCE_MPEMETER_MID0_MID_SIZE		6
/* --------------------------------------------------- */
/* Register: 'PCE MPE Meter Instance 1 ID Register'(LTQ_GSWIP_3_0)*/
/* Bit: 'MID' */
/* Description: 'Meter Instance 0 ID' */
#define PCE_MPEMETER_MID1_MID_OFFSET	0x47B
#define PCE_MPEMETER_MID1_MID_SHIFT		0
#define PCE_MPEMETER_MID1_MID_SIZE		6
/* --------------------------------------------------- */
/* Register: 'PCE Port Index Register'(LTQ_GSWIP_3_0)*/
/* Bit: 'CPU' */
/* Description: 'PCE CPU Port Index Register' */
#define PCE_PID_CPU_OFFSET	0x47C
#define PCE_PID_CPU_SHIFT		0
#define PCE_PID_CPU_SIZE		4
/* --------------------------------------------------- */
/* Register: 'PCE Port ControlRegister 0' */
/* Bit: 'MSTP' */
/* Description: 'Multiple STP Instance Enable' (LTQ_GSWIP_2_2) */
#define PCE_PCTRL_0_MSTP_OFFSET	0x480
#define PCE_PCTRL_0_MSTP_SHIFT	15
#define PCE_PCTRL_0_MSTP_SIZE		1
/* Bit: 'SPFDIS' */
/* Description: 'Port Spoofing Detection' (LTQ_GSWIP_2_2)*/
#define PCE_PCTRL_0_SPFDIS_OFFSET	0x480
#define PCE_PCTRL_0_SPFDIS_SHIFT	14
#define PCE_PCTRL_0_SPFDIS_SIZE		1
/* Bit: 'MCST' */
/* Description: 'Multicast Forwarding Mode Selection' */
#define PCE_PCTRL_0_MCST_OFFSET	0x480
#define PCE_PCTRL_0_MCST_SHIFT	13
#define PCE_PCTRL_0_MCST_SIZE		1
/* Bit: 'EGSTEN' */
/* Description: 'Table-based Egress Special Tag Enable' */
#define PCE_PCTRL_0_EGSTEN_OFFSET	0x480
#define PCE_PCTRL_0_EGSTEN_SHIFT	12
#define PCE_PCTRL_0_EGSTEN_SIZE		1
/* Bit: 'IGSTEN' */
/* Description: 'Ingress Special Tag Enable' */
#define PCE_PCTRL_0_IGSTEN_OFFSET	0x480
#define PCE_PCTRL_0_IGSTEN_SHIFT	11
#define PCE_PCTRL_0_IGSTEN_SIZE		1
/* Bit: 'PCPEN' */
/* Description: 'PCP Remarking Mode' */
#define PCE_PCTRL_0_PCPEN_OFFSET	0x480
#define PCE_PCTRL_0_PCPEN_SHIFT		10
#define PCE_PCTRL_0_PCPEN_SIZE		1
/* Bit: 'CLPEN' */
/* Description: 'Class Remarking Mode' */
#define PCE_PCTRL_0_CLPEN_OFFSET	0x480
#define PCE_PCTRL_0_CLPEN_SHIFT		9
#define PCE_PCTRL_0_CLPEN_SIZE		1
/* Bit: 'DPEN' */
/* Description: 'Drop Precedence Remarking Mode' */
#define PCE_PCTRL_0_DPEN_OFFSET	0x480
#define PCE_PCTRL_0_DPEN_SHIFT	8
#define PCE_PCTRL_0_DPEN_SIZE		1
/* Bit: 'CMOD' */
/* Description: 'Three-color Marker Color Mode' */
#define PCE_PCTRL_0_CMOD_OFFSET	0x480
#define PCE_PCTRL_0_CMOD_SHIFT	7
#define PCE_PCTRL_0_CMOD_SIZE		1
/* Bit: 'VREP' */
/* Description: 'VLAN Replacement Mode' */
#define PCE_PCTRL_0_VREP_OFFSET	0x480
#define PCE_PCTRL_0_VREP_SHIFT	6
#define PCE_PCTRL_0_VREP_SIZE		1
/* Bit: 'TVM' */
/* Description: 'Transparent VLAN Mode' */
#define PCE_PCTRL_0_TVM_OFFSET	0x480
#define PCE_PCTRL_0_TVM_SHIFT		5
#define PCE_PCTRL_0_TVM_SIZE		1
/* Bit: 'PLOCK' */
/* Description: 'Port Locking Enable' */
#define PCE_PCTRL_0_PLOCK_OFFSET	0x480
#define PCE_PCTRL_0_PLOCK_SHIFT		4
#define PCE_PCTRL_0_PLOCK_SIZE		1
/* Bit: 'AGEDIS' */
/* Description: 'Aging Disable' */
#define PCE_PCTRL_0_AGEDIS_OFFSET	0x480
#define PCE_PCTRL_0_AGEDIS_SHIFT	3
#define PCE_PCTRL_0_AGEDIS_SIZE		1
/* Bit: 'PSTATE' */
/* Description: 'Port State' */
#define PCE_PCTRL_0_PSTATE_OFFSET	0x480
#define PCE_PCTRL_0_PSTATE_SHIFT	0
#define PCE_PCTRL_0_PSTATE_SIZE		3
/* --------------------------------------------------- */
/* Register: 'PCE Port ControlRegister 1' */
/* Bit: 'LRNLIM' */
/* Description: 'MAC Address Learning Limit' */
#define PCE_PCTRL_1_LRNLIM_OFFSET	0x481
#define PCE_PCTRL_1_LRNLIM_SHIFT	0
#define PCE_PCTRL_1_LRNLIM_SIZE		8
/* --------------------------------------------------- */
/* Register: 'PCE Port ControlRegister 2' */
/* Bit: 'L2NAT' (LTQ_GSWIP_3_0)*/
/* Description: 'Enables the L2NAT' */
#define PCE_PCTRL_2_L2NAT_OFFSET	0x482
#define PCE_PCTRL_2_L2NAT_SHIFT		11
#define PCE_PCTRL_2_L2NAT_SIZE		1
/* Bit: 'SDEIEN' */
/* Description: 'STAG DEI Remarking Mode' (LTQ_GSWIP_2_2) */
#define PCE_PCTRL_2_SDEIEN_OFFSET	0x482
#define PCE_PCTRL_2_SDEIEN_SHIFT	10
#define PCE_PCTRL_2_SDEIEN_SIZE		1
/* Bit: 'SPCPEN' */
/* Description: 'STAG PCP Remarking Mode' (LTQ_GSWIP_2_2) */
#define PCE_PCTRL_2_SPCPEN_OFFSET	0x482
#define PCE_PCTRL_2_SPCPEN_SHIFT	9
#define PCE_PCTRL_2_SPCPEN_SIZE		1
/* Bit: 'SPCP' */
/* Description: 'Enable VLAN STAG PCP and DEI to select */
/**	the Class of Service' (LTQ_GSWIP_2_2) */
#define PCE_PCTRL_2_SPCP_OFFSET	0x482
#define PCE_PCTRL_2_SPCP_SHIFT	8
#define PCE_PCTRL_2_SPCP_SIZE		1
/* Bit: 'DSCPMOD' */
/* Description: 'DSCP Mode Selection' */
#define PCE_PCTRL_2_DSCPMOD_OFFSET	0x482
#define PCE_PCTRL_2_DSCPMOD_SHIFT		7
#define PCE_PCTRL_2_DSCPMOD_SIZE		1
/* Bit: 'DSCP' */
/* Description: 'Enable DSCP to select the Class of Service' */
#define PCE_PCTRL_2_DSCP_OFFSET	0x482
#define PCE_PCTRL_2_DSCP_SHIFT	5
#define PCE_PCTRL_2_DSCP_SIZE		2
/* Bit: 'PCP' */
/* Description: 'Enable VLAN PCP to select the Class of Service' */
#define PCE_PCTRL_2_PCP_OFFSET	0x482
#define PCE_PCTRL_2_PCP_SHIFT		4
#define PCE_PCTRL_2_PCP_SIZE		1
/* Bit: 'PCLASS' */
/* Description: 'Port-based Traffic Class' */
#define PCE_PCTRL_2_PCLASS_OFFSET	0x482
#define PCE_PCTRL_2_PCLASS_SHIFT	0
#define PCE_PCTRL_2_PCLASS_SIZE		4
/* --------------------------------------------------- */
/* Register: 'PCE Port ControlRegister 3'  */
/* Bit: 'LNDIS' */
/* Description: 'Learning Disable'(LTQ_GSWIP_2_2) */
#define PCE_PCTRL_3_LNDIS_OFFSET	0x483
#define PCE_PCTRL_3_LNDIS_SHIFT		15
#define PCE_PCTRL_3_LNDIS_SIZE		1
/* Bit: 'IGPTRM' */
/* Description: 'Ingress Port Removal Disable'(LTQ_GSWIP_2_2)*/
#define PCE_PCTRL_3_IGPTRM_OFFSET	0x483
#define PCE_PCTRL_3_IGPTRM_SHIFT	14
#define PCE_PCTRL_3_IGPTRM_SIZE		1
/* Bit: 'VIO_9' */
/* Description: 'New MAC-Port Association Mirroring Enable'(LTQ_GSWIP_2_2) */
#define PCE_PCTRL_3_VIO_9_OFFSET	0x483
#define PCE_PCTRL_3_VIO_9_SHIFT		13
#define PCE_PCTRL_3_VIO_9_SIZE		1
/* Bit: 'VIO_8' */
/* Description: 'Violation Type 8 Mirroring Enable' */
#define PCE_PCTRL_3_VIO_8_OFFSET	0x483
#define PCE_PCTRL_3_VIO_8_SHIFT		12
#define PCE_PCTRL_3_VIO_8_SIZE		1
/* Bit: 'EDIR' */
/* Description: 'Egress Redirection Mode' */
#define PCE_PCTRL_3_EDIR_OFFSET	0x483
#define PCE_PCTRL_3_EDIR_SHIFT	11
#define PCE_PCTRL_3_EDIR_SIZE		1
/* Bit: 'RXDMIR' */
/* Description: 'Receive Mirroring Enable for dropped frames' */
#define PCE_PCTRL_3_RXDMIR_OFFSET	0x483
#define PCE_PCTRL_3_RXDMIR_SHIFT	10
#define PCE_PCTRL_3_RXDMIR_SIZE		1
/* Bit: 'RXVMIR' */
/* Description: 'Receive Mirroring Enable for valid frames' */
#define PCE_PCTRL_3_RXVMIR_OFFSET	0x483
#define PCE_PCTRL_3_RXVMIR_SHIFT	9
#define PCE_PCTRL_3_RXVMIR_SIZE		1
/* Bit: 'TXMIR' */
/* Description: 'Transmit Mirroring Enable' */
#define PCE_PCTRL_3_TXMIR_OFFSET	0x483
#define PCE_PCTRL_3_TXMIR_SHIFT		8
#define PCE_PCTRL_3_TXMIR_SIZE		1
#define PCE_PCTRL_3_RX_TXMIR_SIZE	2
/* Bit: 'VIO_7' */
/* Description: 'Violation Type 7 Mirroring Enable' */
#define PCE_PCTRL_3_VIO_7_OFFSET	0x483
#define PCE_PCTRL_3_VIO_7_SHIFT		7
#define PCE_PCTRL_3_VIO_7_SIZE		1
/* Bit: 'VIO_6' */
/* Description: 'Violation Type 6 Mirroring Enable' */
#define PCE_PCTRL_3_VIO_6_OFFSET	0x483
#define PCE_PCTRL_3_VIO_6_SHIFT		6
#define PCE_PCTRL_3_VIO_6_SIZE		1
/* Bit: 'VIO_5' */
/* Description: 'Violation Type 5 Mirroring Enable' */
#define PCE_PCTRL_3_VIO_5_OFFSET	0x483
#define PCE_PCTRL_3_VIO_5_SHIFT		5
#define PCE_PCTRL_3_VIO_5_SIZE		1
/* Bit: 'VIO_4' */
/* Description: 'Violation Type 4 Mirroring Enable' */
#define PCE_PCTRL_3_VIO_4_OFFSET	0x483
#define PCE_PCTRL_3_VIO_4_SHIFT		4
#define PCE_PCTRL_3_VIO_4_SIZE		1
/* Bit: 'VIO_3' */
/* Description: 'Violation Type 3 Mirroring Enable' */
#define PCE_PCTRL_3_VIO_3_OFFSET	0x483
#define PCE_PCTRL_3_VIO_3_SHIFT		3
#define PCE_PCTRL_3_VIO_3_SIZE		1
/* Bit: 'VIO_2' */
/* Description: 'Violation Type 2 Mirroring Enable' */
#define PCE_PCTRL_3_VIO_2_OFFSET	0x483
#define PCE_PCTRL_3_VIO_2_SHIFT		2
#define PCE_PCTRL_3_VIO_2_SIZE		1
/* Bit: 'VIO_1' */
/* Description: 'Violation Type 1 Mirroring Enable' */
#define PCE_PCTRL_3_VIO_1_OFFSET	0x483
#define PCE_PCTRL_3_VIO_1_SHIFT		1
#define PCE_PCTRL_3_VIO_1_SIZE		1
/* Bit: 'VIO_0' */
/* Description: 'Violation Type 0 Mirroring Enable' */
#define PCE_PCTRL_3_VIO_0_OFFSET	0x483
#define PCE_PCTRL_3_VIO_0_SHIFT		0
#define PCE_PCTRL_3_VIO_0_SIZE		1
/* --------------------------------------------------- */
/* Register: 'Wake-on-LAN ControlRegister' */
/* Bit: 'PORT' */
/* Description: 'WoL Enable' */
#define WOL_CTRL_PORT_OFFSET	0x484
#define WOL_CTRL_PORT_SHIFT		0
#define WOL_CTRL_PORT_SIZE		1
/* --------------------------------------------------- */
/* Register: 'PCE VLAN ControlRegister'*/
/* Bit: 'MACEN' */
/* Description: 'MAC VLAN Enable' (LTQ_GSWIP_2_2) */
#define PCE_VCTRL_MACEN_OFFSET	0x485
#define PCE_VCTRL_MACEN_SHIFT		15
#define PCE_VCTRL_MACEN_SIZE		1
/* Bit: 'SVID0' */
/* Description: 'STAG Priority Tagged Rule' (LTQ_GSWIP_2_2) */
#define PCE_VCTRL_SVID0_OFFSET	0x485
#define PCE_VCTRL_SVID0_SHIFT		14
#define PCE_VCTRL_SVID0_SIZE		1
/* Bit: 'SVSR' */
/* Description: 'STAG VLAN Security Rule' (LTQ_GSWIP_2_2) */
#define PCE_VCTRL_SVSR_OFFSET		0x485
#define PCE_VCTRL_SVSR_SHIFT		13
#define PCE_VCTRL_SVSR_SIZE			1
/* Bit: 'SVEMR' (LTQ_GSWIP_2_2)*/
/* Description: 'STAG VLAN Egress Member Violation Rule' */
#define PCE_VCTRL_SVEMR_OFFSET	0x485
#define PCE_VCTRL_SVEMR_SHIFT		12
#define PCE_VCTRL_SVEMR_SIZE		1
/* Bit: 'SVIMR' (LTQ_GSWIP_2_2)*/
/* Description: 'STAG VLAN Ingress Member Violation Rule' */
#define PCE_VCTRL_SVIMR_OFFSET	0x485
#define PCE_VCTRL_SVIMR_SHIFT		11
#define PCE_VCTRL_SVIMR_SIZE		1
#define PCE_VCTRL_SVIMR_SVEMR_SIZE	2
/* Bit: 'SVINR' */
/* Description: 'STAG VLAN Ingress Tag Rule'(LTQ_GSWIP_2_2) */
#define PCE_VCTRL_SVINR_OFFSET	0x485
#define PCE_VCTRL_SVINR_SHIFT		9
#define PCE_VCTRL_SVINR_SIZE		2
/* Bit: 'STEN' */
/* Description: 'STAG VLAN Enable'(LTQ_GSWIP_2_2) */
#define PCE_VCTRL_STEN_OFFSET		0x485
#define PCE_VCTRL_STEN_SHIFT		8
#define PCE_VCTRL_STEN_SIZE			1
/* Bit: 'STVM' */
/* Description: 'STAG Transparent VLAN Mode'(LTQ_GSWIP_2_2) */
#define PCE_VCTRL_STVM_OFFSET		0x485
#define PCE_VCTRL_STVM_SHIFT		7
#define PCE_VCTRL_STVM_SIZE			1
/* Bit: 'VID0' */
/* Description: 'Priority Tagged Rule' */
#define PCE_VCTRL_VID0_OFFSET		0x485
#define PCE_VCTRL_VID0_SHIFT		6
#define PCE_VCTRL_VID0_SIZE			1
/* Bit: 'VSR' */
/* Description: 'VLAN Security Rule' */
#define PCE_VCTRL_VSR_OFFSET		0x485
#define PCE_VCTRL_VSR_SHIFT			5
#define PCE_VCTRL_VSR_SIZE			1
/* Bit: 'VEMR' */
/* Description: 'VLAN Egress Member Violation Rule' */
#define PCE_VCTRL_VEMR_OFFSET		0x485
#define PCE_VCTRL_VEMR_SHIFT		4
#define PCE_VCTRL_VEMR_SIZE			1
/* Bit: 'VIMR' */
/* Description: 'VLAN Ingress Member Violation Rule' */
#define PCE_VCTRL_VIMR_OFFSET		0x485
#define PCE_VCTRL_VIMR_SHIFT		3
#define PCE_VCTRL_VIMR_SIZE			1
#define PCE_VCTRL_VIMR_VEMR_SIZE	2
/* Bit: 'VINR' */
/* Description: 'VLAN Ingress Tag Rule' */
#define PCE_VCTRL_VINR_OFFSET		0x485
#define PCE_VCTRL_VINR_SHIFT		1
#define PCE_VCTRL_VINR_SIZE			2
/* Bit: 'UVR' */
/* Description: 'Unknown VLAN Rule' */
#define PCE_VCTRL_UVR_OFFSET		0x485
#define PCE_VCTRL_UVR_SHIFT			0
#define PCE_VCTRL_UVR_SIZE			1
/* --------------------------------------------------- */
/* Register: 'PCE Default PortVID Register' */
/* Bit: 'PVID' (LTQ_GSWIP_2_2) size changed from 6 to 12*/
/* Description: 'Default Port VID Index'  */
#define PCE_DEFPVID_PVID_OFFSET	0x486
#define PCE_DEFPVID_PVID_SHIFT	0
#define PCE_DEFPVID_PVID_SIZE		12
/* --------------------------------------------------- */
/* Register: 'PCE Port StatusRegister' */
/* Bit: 'LRNCNT' */
/* Description: 'Learning Count' */
#define PCE_PSTAT_LRNCNT_OFFSET	0x487
#define PCE_PSTAT_LRNCNT_SHIFT	0
#define PCE_PSTAT_LRNCNT_SIZE		16
/* --------------------------------------------------- */
/* Register: 'Parser and ClassificationEngine Port Interrupt Enable Register' */
/* Bit: 'MTEBP' (LTQ_GSWIP_2_2)*/
/* Description: 'Metering Based Backpressure Status Change Interrupt Enable' */
#define PCE_PIER_MTEBP_OFFSET	0x488
#define PCE_PIER_MTEBP_SHIFT	8
#define PCE_PIER_MTEBP_SIZE		1
/* Bit: 'SPFINT' (LTQ_GSWIP_2_2)*/
/* Description: 'Port Spoofing Alert Interrupt Enable' */
#define PCE_PIER_SPFINT_OFFSET	0x488
#define PCE_PIER_SPFINT_SHIFT		7
#define PCE_PIER_SPFINT_SIZE		1
/* Bit: 'FRZDRP' */
/* Description: 'MAC Table Freeze Drop Interrupt Enable' */
#define PCE_PIER_FRZDRP_OFFSET	0x488
#define PCE_PIER_FRZDRP_SHIFT		6
#define PCE_PIER_FRZDRP_SIZE		1
/* Bit: 'CLDRP' */
/* Description: 'Classification Drop Interrupt Enable' */
#define PCE_PIER_CLDRP_OFFSET	0x488
#define PCE_PIER_CLDRP_SHIFT	5
#define PCE_PIER_CLDRP_SIZE		1
/* Bit: 'PTDRP' */
/* Description: 'Port Drop Interrupt Enable' */
#define PCE_PIER_PTDRP_OFFSET	0x488
#define PCE_PIER_PTDRP_SHIFT	4
#define PCE_PIER_PTDRP_SIZE		1
/* Bit: 'VLAN' */
/* Description: 'VLAN Violation Interrupt Enable' */
#define PCE_PIER_VLAN_OFFSET	0x488
#define PCE_PIER_VLAN_SHIFT		3
#define PCE_PIER_VLAN_SIZE		1
/* Bit: 'WOL' */
/* Description: 'Wake-on-LAN Interrupt Enable' */
#define PCE_PIER_WOL_OFFSET		0x488
#define PCE_PIER_WOL_SHIFT		2
#define PCE_PIER_WOL_SIZE			1
/* Bit: 'LOCK' */
/* Description: 'Port Lock Alert Interrupt Enable' */
#define PCE_PIER_LOCK_OFFSET	0x488
#define PCE_PIER_LOCK_SHIFT		1
#define PCE_PIER_LOCK_SIZE		1
/* Bit: 'LIM' */
/* Description: 'Port Limit Alert Interrupt Enable' */
#define PCE_PIER_LIM_OFFSET	0x488
#define PCE_PIER_LIM_SHIFT	0
#define PCE_PIER_LIM_SIZE		1
#define PCE_PIER_REG_SHIFT	0
#define PCE_PIER_REG_SIZE		16
/* --------------------------------------------------- */
/* Register: 'Parser and ClassificationEngine Port Interrupt Status Register' */
/* Bit: 'MTEBP' (LTQ_GSWIP_2_2)*/
/* Description: 'Metering Based Backpressure Status Change Interrupt' */
#define PCE_PISR_MTEBP_OFFSET	0x489
#define PCE_PISR_MTEBP_SHIFT	8
#define PCE_PISR_MTEBP_SIZE		1
/* Bit: 'SPFINT'(LTQ_GSWIP_2_2) */
/* Description: 'Port Spoofing Alert Interrupt' */
#define PCE_PISR_SPFINT_OFFSET	0x489
#define PCE_PISR_SPFINT_SHIFT		7
#define PCE_PISR_SPFINT_SIZE		1
/* Bit: 'FRZDRP' */
/* Description: 'MAC Table Freeze Drop Interrupt' */
#define PCE_PISR_FRZDRP_OFFSET	0x489
#define PCE_PISR_FRZDRP_SHIFT		6
#define PCE_PISR_FRZDRP_SIZE		1
/* Bit: 'CLDRP' */
/* Description: 'Classification Drop Interrupt' */
#define PCE_PISR_CLDRP_OFFSET	0x489
#define PCE_PISR_CLDRP_SHIFT	5
#define PCE_PISR_CLDRP_SIZE		1
/* Bit: 'PTDRP' */
/* Description: 'Port Drop Interrupt' */
#define PCE_PISR_PTDRP_OFFSET	0x489
#define PCE_PISR_PTDRP_SHIFT	4
#define PCE_PISR_PTDRP_SIZE		1
/* Bit: 'VLAN' */
/* Description: 'VLAN Violation Interrupt' */
#define PCE_PISR_VLAN_OFFSET	0x489
#define PCE_PISR_VLAN_SHIFT		3
#define PCE_PISR_VLAN_SIZE		1
/* Bit: 'WOL' */
/* Description: 'Wake-on-LAN Interrupt' */
#define PCE_PISR_WOL_OFFSET	0x489
#define PCE_PISR_WOL_SHIFT	2
#define PCE_PISR_WOL_SIZE		1
/* Bit: 'LOCK' */
/* Description: 'Port Lock Alert Interrupt' */
#define PCE_PISR_LOCK_OFFSET	0x489
#define PCE_PISR_LOCK_SHIFT		1
#define PCE_PISR_LOCK_SIZE		1
/* Bit: 'LIMIT' */
/* Description: 'Port Limitation Alert Interrupt' */
#define PCE_PISR_LIMIT_OFFSET	0x489
#define PCE_PISR_LIMIT_SHIFT	0
#define PCE_PISR_LIMIT_SIZE		1
/* -------------------------------------------*/
/* Register: 'PCE Default Port STAG VID Register'(LTQ_GSWIP_2_2) */
/* Bit: 'PVID' */
/* Description: 'Default Port STAG VID Index' */
#define PCE_DEFPSVID_PVID_OFFSET	0x520
#define PCE_DEFPSVID_PVID_SHIFT		0
#define PCE_DEFPSVID_PVID_SIZE		13
/* ---------------------------------------------*/
/* Register: 'PCE Port Trunking Register' (LTQ_GSWIP_2_2)*/
/* Bit: 'EN' */
/* Description: 'Port Trunking Enable' */
#define PCE_PTRUNK_EN_OFFSET	0x521
#define PCE_PTRUNK_EN_SHIFT		15
#define PCE_PTRUNK_EN_SIZE		1
/* Bit: 'PARTER' */
/* Description: 'Port Trunking Partner Port ID' */
#define PCE_PTRUNK_PARTER_OFFSET	0x521
#define PCE_PTRUNK_PARTER_SHIFT		0
#define PCE_PTRUNK_PARTER_SIZE		4
/* --------------------------------------------------- */
/* Register: 'PCE Port Memmbership Register'(LTQ_GSWIP_3_0) */
/* Bit: 'MEMBER' */
/* Description: 'PCE Port Membership ' */
#define PCE_PPM_MEMBER_OFFSET	0x540
#define PCE_PPM_MEMBER_SHIFT	0
#define PCE_PPM_MEMBER_SIZE		13
/* ---------------------------------------------*/
/* Register: 'PCE Port Egress VLAN Treatment Configuration Register' */
/**	(LTQ_GSWIP_3_0)*/
/* Bit: 'EGVMD' */
/* Description: 'PCE Egress VLAN Treatment Access Selection' */
#define PCE_EVLANCFG_EGVMD_OFFSET	0x541
#define PCE_EVLANCFG_EGVMD_SHIFT		15
#define PCE_EVLANCFG_EGVMD_SIZE		1
/* Bit: 'EGVFST' */
/* Description: 'PCE Egress VLAN Treatment First Entry Index ' */
#define PCE_EVLANCFG_EGVFST_OFFSET	0x541
#define PCE_EVLANCFG_EGVFST_SHIFT		0
#define PCE_EVLANCFG_EGVFST_SIZE		8
/* --------------------------------------------------- */
/* Register:'PCE Flow Engine Port Region Start Index Register' */
/* (LTQ_GSWIP_3_0)*/
/* Bit: 'INDEX' */
/* Description: 'PCE Flow Engine Port Region Start Index  ' */
#define PCE_TFPR_INDEX_OFFSET	0x542
#define PCE_TFPR_INDEX_SHIFT	2
#define PCE_TFPR_INDEX_SIZE		6
/* ---------------------------------------------*/
/* Register:'PCE Flow Engine Port Region Entry Numbers Register' */
/* (LTQ_GSWIP_3_0)*/
/* Bit: 'NUMBER' */
/* Description: 'PCE Flow Engine Port Region Entry Numbers ' */
#define PCE_TFPR_NUMBER_OFFSET	0x543
#define PCE_TFPR_NUMBER_SHIFT		2
#define PCE_TFPR_NUMBER_SIZE		8
/* ---------------------------------------------*/
/* Register: 'PCE Ingress Port Removal Configuration Register'(LTQ_GSWIP_3_0) */
/* Bit: 'SUB15' */
/* Description: 'Sub-Interface Group 15 Ingress Port Removal' */
#define PCE_IGPTRM_SUB15_OFFSET	0x546
#define PCE_IGPTRM_SUB15_SHIFT	15
#define PCE_IGPTRM_SUB15_SIZE		1
/* Bit: 'SUB14' */
/* Description: 'Sub-Interface Group 14 Ingress Port Removal' */
#define PCE_IGPTRM_SUB14_OFFSET	0x546
#define PCE_IGPTRM_SUB14_SHIFT	14
#define PCE_IGPTRM_SUB14_SIZE		1
/* Bit: 'SUB13' */
/* Description: 'Sub-Interface Group 13 Ingress Port Removal' */
#define PCE_IGPTRM_SUB13_OFFSET	0x546
#define PCE_IGPTRM_SUB13_SHIFT	13
#define PCE_IGPTRM_SUB13_SIZE		1
/* Bit: 'SUB12' */
/* Description: 'Sub-Interface Group 12 Ingress Port Removal' */
#define PCE_IGPTRM_SUB12_OFFSET	0x546
#define PCE_IGPTRM_SUB12_SHIFT	12
#define PCE_IGPTRM_SUB12_SIZE		1
/* Bit: 'SUB11' */
/* Description: 'Sub-Interface Group 11 Ingress Port Removal' */
#define PCE_IGPTRM_SUB11_OFFSET	0x546
#define PCE_IGPTRM_SUB11_SHIFT	11
#define PCE_IGPTRM_SUB11_SIZE		1
/* Bit: 'SUB10' */
/* Description: 'Sub-Interface Group 10 Ingress Port Removal' */
#define PCE_IGPTRM_SUB10_OFFSET	0x546
#define PCE_IGPTRM_SUB10_SHIFT	10
#define PCE_IGPTRM_SUB10_SIZE		1
/* Bit: 'SUB9' */
/* Description: 'Sub-Interface Group 9 Ingress Port Removal' */
#define PCE_IGPTRM_SUB9_OFFSET	0x546
#define PCE_IGPTRM_SUB9_SHIFT		9
#define PCE_IGPTRM_SUB9_SIZE		1
/* Bit: 'SUB8' */
/* Description: 'Sub-Interface Group 8 Ingress Port Removal' */
#define PCE_IGPTRM_SUB8_OFFSET	0x546
#define PCE_IGPTRM_SUB8_SHIFT		8
#define PCE_IGPTRM_SUB8_SIZE		1
/* Bit: 'SUB7' */
/* Description: 'Sub-Interface Group 7 Ingress Port Removal' */
#define PCE_IGPTRM_SUB7_OFFSET	0x546
#define PCE_IGPTRM_SUB7_SHIFT		7
#define PCE_IGPTRM_SUB7_SIZE		1
/* Bit: 'SUB6' */
/* Description: 'Sub-Interface Group 6 Ingress Port Removal' */
#define PCE_IGPTRM_SUB6_OFFSET	0x546
#define PCE_IGPTRM_SUB6_SHIFT		6
#define PCE_IGPTRM_SUB6_SIZE		1
/* Bit: 'SUB5' */
/* Description: 'Sub-Interface Group 5 Ingress Port Removal' */
#define PCE_IGPTRM_SUB5_OFFSET	0x546
#define PCE_IGPTRM_SUB5_SHIFT		5
#define PCE_IGPTRM_SUB5_SIZE		1
/* Bit: 'SUB4' */
/* Description: 'Sub-Interface Group 4 Ingress Port Removal' */
#define PCE_IGPTRM_SUB4_OFFSET	0x546
#define PCE_IGPTRM_SUB4_SHIFT		4
#define PCE_IGPTRM_SUB4_SIZE		1
/* Bit: 'SUB3' */
/* Description: 'Sub-Interface Group 3 Ingress Port Removal' */
#define PCE_IGPTRM_SUB3_OFFSET	0x546
#define PCE_IGPTRM_SUB3_SHIFT		3
#define PCE_IGPTRM_SUB3_SIZE		1
/* Bit: 'SUB2' */
/* Description: 'Sub-Interface Group 2 Ingress Port Removal' */
#define PCE_IGPTRM_SUB2_OFFSET	0x546
#define PCE_IGPTRM_SUB2_SHIFT		2
#define PCE_IGPTRM_SUB2_SIZE		1
/* Bit: 'SUB1' */
/* Description: 'Sub-Interface Group 1 Ingress Port Removal' */
#define PCE_IGPTRM_SUB1_OFFSET	0x546
#define PCE_IGPTRM_SUB1_SHIFT		1
#define PCE_IGPTRM_SUB1_SIZE		1
/* Bit: 'SUB0' */
/* Description: 'Sub-Interface Group 0 Ingress Port Removal' */
#define PCE_IGPTRM_SUB0_OFFSET	0x546
#define PCE_IGPTRM_SUB0_SHIFT		0
#define PCE_IGPTRM_SUB0_SIZE		1
/* --------------------------------------------------- */
/* Register: 'PCE L2NAT MAC0 Register'(LTQ_GSWIP_3_0) */
/* Bit: 'MAC0' */
/* Description: 'L2NAT MAC Address Bit 15 to 0' */
#define PCE_L2NAT_MAC0_MAC0_OFFSET	0x545
#define PCE_L2NAT_MAC0_MAC0_SHIFT		0
#define PCE_L2NAT_MAC0_MAC0_SIZE		16
/* ---------------------------------------------*/
/* Register: 'PCE L2NAT MAC1 Register'(LTQ_GSWIP_3_0) */
/* Bit: 'MAC1' */
/* Description: 'L2NAT MAC Address Bit 31 to 16' */
#define PCE_L2NAT_MAC1_MAC1_OFFSET	0x546
#define PCE_L2NAT_MAC1_MAC1_SHIFT		0
#define PCE_L2NAT_MAC1_MAC1_SIZE		16
/* ---------------------------------------------*/
/* Register: 'PCE L2NAT MAC2 Register'(LTQ_GSWIP_3_0) */
/* Bit: 'MAC2' */
/* Description: 'L2NAT MAC Address Bit 47 to 32' */
#define PCE_L2NAT_MAC2_MAC2_OFFSET	0x547
#define PCE_L2NAT_MAC2_MAC2_SHIFT		0
#define PCE_L2NAT_MAC2_MAC2_SIZE		16
/* ---------------------------------------------*/
/* Register: 'Three-colorMarker Control Register' */
/* Bit: 'TEBYP' (LTQ_GSWIP_3_0)*/
/* Description: 'Te Bucket Check Bypass' */
#define PCE_TCM_CTRL_TEBYP_OFFSET	0x580
#define PCE_TCM_CTRL_TEBYP_SHIFT	1
#define PCE_TCM_CTRL_TEBYP_SIZE		1
/* Bit: 'TCMEN' */
/* Description: 'Three-color Marker metering instance enable' */
#define PCE_TCM_CTRL_TCMEN_OFFSET	0x580
#define PCE_TCM_CTRL_TCMEN_SHIFT	0
#define PCE_TCM_CTRL_TCMEN_SIZE		1
/* --------------------------------------------------- */
/* Register: 'Three-colorMarker Status Register' */
/* Bit: 'MTEBP' (LTQ_GSWIP_2_2)*/
/* Description: 'Metering Based Back Pressure Status' */
#define PCE_TCM_STAT_MTEBP_OFFSET	0x581
#define PCE_TCM_STAT_MTEBP_SHIFT	2
#define PCE_TCM_STAT_MTEBP_SIZE		1
/* Bit: 'AL1' */
/* Description: 'Three-color Marker Alert 1 Status' */
#define PCE_TCM_STAT_AL1_OFFSET	0x581
#define PCE_TCM_STAT_AL1_SHIFT	1
#define PCE_TCM_STAT_AL1_SIZE		1
/* Bit: 'AL0' */
/* Description: 'Three-color Marker Alert 0 Status' */
#define PCE_TCM_STAT_AL0_OFFSET	0x581
#define PCE_TCM_STAT_AL0_SHIFT	0
#define PCE_TCM_STAT_AL0_SIZE		1
/* --------------------------------------------------- */
/* Register: 'Three-color MarkerCommitted Burst Size Register' */
/* Bit: 'CBS' */
/* Description: 'Committed Burst Size' */
#define PCE_TCM_CBS_CBS_OFFSET	0x582
#define PCE_TCM_CBS_CBS_SHIFT		0
#define PCE_TCM_CBS_CBS_SIZE		10
/* --------------------------------------------------- */
/* Register: 'Three-color MarkerExcess Burst Size Register' */
/* Bit: 'EBS' */
/* Description: 'Excess Burst Size' */
#define PCE_TCM_EBS_EBS_OFFSET	0x583
#define PCE_TCM_EBS_EBS_SHIFT		0
#define PCE_TCM_EBS_EBS_SIZE		10
/* --------------------------------------------------- */
/* Register: 'Three-color MarkerInstantaneous Burst Size Register' */
/* Bit: 'IBS' */
/* Description: 'Instantaneous Burst Size' */
#define PCE_TCM_IBS_IBS_OFFSET	0x584
#define PCE_TCM_IBS_IBS_SHIFT		0
#define PCE_TCM_IBS_IBS_SIZE		2
/* --------------------------------------------------- */
/* Register: 'Three-colorMarker Constant Information Rate Mantissa Register' */
/* Bit: 'MANT' */
/* Description: 'Rate Counter Mantissa' */
#define PCE_TCM_CIR_MANT_MANT_OFFSET	0x585
#define PCE_TCM_CIR_MANT_MANT_SHIFT		0
#define PCE_TCM_CIR_MANT_MANT_SIZE		10
/* --------------------------------------------------- */
/* Register: 'Three-colorMarker Constant Information Rate Exponent Register' */
/* Bit: 'EXP' */
/* Description: 'Rate Counter Exponent' */
#define PCE_TCM_CIR_EXP_EXP_OFFSET	0x586
#define PCE_TCM_CIR_EXP_EXP_SHIFT		0
#define PCE_TCM_CIR_EXP_EXP_SIZE		4
/* --------------------------------------------------- */
#if 0
/* Register 'Instance Selection Register' ( LTQ_GSWIP_3_0 )*/
/* Bit: 'INST' */
/* Description: 'Instance Selection' */
#define GSW_INST_SEL_INST_OFFSET	0xE00
#define GSW_INST_SEL_INST_SHIFT		0
#define GSW_INST_SEL_INST_SIZE		8
/* --------------------------------------------------- */
/* Register: 'Three-colorMarker Control Register' ( LTQ_GSWIP_3_0 )*/
/* Bit: 'TMOD' (LTQ_GSWIP_3_0)*/
/* Description: 'Meter Mode' */
#define GSW_PCE_TCM_CTRL_TMOD_OFFSET	0xE10
#define GSW_PCE_TCM_CTRL_TMOD_SHIFT	2
#define GSW_PCE_TCM_CTRL_TMOD_SIZE	1
/* Bit: 'TEBYP' (LTQ_GSWIP_3_0)*/
/* Description: 'Te Bucket Check Bypass' */
#define GSW_PCE_TCM_CTRL_TEBYP_OFFSET	0xE10
#define GSW_PCE_TCM_CTRL_TEBYP_SHIFT	1
#define GSW_PCE_TCM_CTRL_TEBYP_SIZE		1
/* Bit: 'TCMEN' */
/* Description: 'Three-color Marker metering instance enable' */
#define GSW_PCE_TCM_CTRL_TCMEN_OFFSET	0xE10
#define GSW_PCE_TCM_CTRL_TCMEN_SHIFT	0
#define GSW_PCE_TCM_CTRL_TCMEN_SIZE		1
/* --------------------------------------------------- */
/* Register: 'Three-colorMarker Status Register' ( LTQ_GSWIP_3_0 )*/
/* Bit: 'MTEBP' (LTQ_GSWIP_2_2)*/
/* Description: 'Metering Based Back Pressure Status' */
#define GSW_PCE_TCM_STAT_MTEBP_OFFSET	0xE11
#define GSW_PCE_TCM_STAT_MTEBP_SHIFT	2
#define GSW_PCE_TCM_STAT_MTEBP_SIZE		1
/* Bit: 'AL1' */
/* Description: 'Three-color Marker Alert 1 Status' */
#define GSW_PCE_TCM_STAT_AL1_OFFSET	0xE11
#define GSW_PCE_TCM_STAT_AL1_SHIFT	1
#define GSW_PCE_TCM_STAT_AL1_SIZE		1
/* Bit: 'AL0' */
/* Description: 'Three-color Marker Alert 0 Status' */
#define GSW_PCE_TCM_STAT_AL0_OFFSET	0xE11
#define GSW_PCE_TCM_STAT_AL0_SHIFT	0
#define GSW_PCE_TCM_STAT_AL0_SIZE		1
/* --------------------------------------------------- */
/* Register: 'Three-color MarkerCommitted Burst Size Register' (LTQ_GSWIP_3_0)*/
/* Bit: 'CBS' */
/* Description: 'Committed Burst Size' */
#define GSW_PCE_TCM_CBS_CBS_OFFSET	0xE12
#define GSW_PCE_TCM_CBS_CBS_SHIFT		0
#define GSW_PCE_TCM_CBS_CBS_SIZE		10
/* --------------------------------------------------- */
/* Register: 'Three-color MarkerExcess Burst Size Register' (LTQ_GSWIP_3_0)*/
/* Bit: 'EBS' */
/* Description: 'Excess Burst Size' */
#define GSW_PCE_TCM_EBS_EBS_OFFSET	0xE13
#define GSW_PCE_TCM_EBS_EBS_SHIFT		0
#define GSW_PCE_TCM_EBS_EBS_SIZE		10
/* --------------------------------------------------- */
/* Register: 'Three-color MarkerInstantaneous Burst Size Register' */
/* (LTQ_GSWIP_3_0) */
/* Bit: 'IBS' */
/* Description: 'Instantaneous Burst Size' */
#define GSW_PCE_TCM_IBS_IBS_OFFSET	0xE14
#define GSW_PCE_TCM_IBS_IBS_SHIFT		0
#define GSW_PCE_TCM_IBS_IBS_SIZE		2
/* --------------------------------------------------- */
/* Register: 'Three-colorMarker Constant Information Rate Mantissa Register' */
/* (LTQ_GSWIP_3_0) */
/* Bit: 'MANT' */
/* Description: 'Rate Counter Mantissa' */
#define GSW_PCE_TCM_CIR_MANT_MANT_OFFSET	0xE15
#define GSW_PCE_TCM_CIR_MANT_MANT_SHIFT		0
#define GSW_PCE_TCM_CIR_MANT_MANT_SIZE		10
/* --------------------------------------------------- */
/* Register: 'Three-colorMarker Constant Information Rate */
/* Exponent Register' (LTQ_GSWIP_3_0)*/
/* Bit: 'EXP' */
/* Description: 'Rate Counter Exponent' */
#define GSW_PCE_TCM_CIR_EXP_EXP_OFFSET	0xE16
#define GSW_PCE_TCM_CIR_EXP_EXP_SHIFT		0
#define GSW_PCE_TCM_CIR_EXP_EXP_SIZE		4
/* --------------------------------------------------- */
/* Register: 'Three-colorMarker Peak Information Rate */
/* Mantissa Register'(LTQ_GSWIP_3_0) */
/* Bit: 'MANT' */
/* Description: 'Rate Counter Mantissa' */
#define GSW_PCE_TCM_PIR_MANT_MANT_OFFSET	0xE17
#define GSW_PCE_TCM_PIR_MANT_MANT_SHIFT		0
#define GSW_PCE_TCM_PIR_MANT_MANT_SIZE		10
/* --------------------------------------------------- */
/* Register: 'Three-colorMarker Peak Information Rate */
/* Exponent Register' (LTQ_GSWIP_3_0)*/
/* Bit: 'EXP' */
/* Description: 'Rate Counter Exponent' */
#define GSW_PCE_TCM_PIR_EXP_EXP_OFFSET	0xE18
#define GSW_PCE_TCM_PIR_EXP_EXP_SHIFT		0
#define GSW_PCE_TCM_PIR_EXP_EXP_SIZE		4
/* --------------------------------------------------- */
#endif //if 0
/* Register: 'MAC Test Register' */
/* Bit: 'JTP' */
/* Description: 'Jitter Test Pattern' */
#define MAC_TEST_JTP_OFFSET	0x8C0
#define MAC_TEST_JTP_SHIFT	0
#define MAC_TEST_JTP_SIZE		16
/* --------------------------------------------------- */
/* Register: 'MAC Pause FrameSource Address Configuration Register' */
/* Bit: 'SAMOD' */
/* Description: 'Source Address Mode' */
#define MAC_PFAD_CFG_SAMOD_OFFSET	0x8C1
#define MAC_PFAD_CFG_SAMOD_SHIFT	0
#define MAC_PFAD_CFG_SAMOD_SIZE		1
/* --------------------------------------------------- */
/* Register: 'Pause Frame SourceAddress Part 0 ' */
/* Bit: 'PFAD' */
/* Description: 'Pause Frame Source Address Part 0' */
#define MAC_PFSA_0_PFAD_OFFSET	0x8C2
#define MAC_PFSA_0_PFAD_SHIFT		0
#define MAC_PFSA_0_PFAD_SIZE		16
/* --------------------------------------------------- */
/* Register: 'Pause Frame SourceAddress Part 1 ' */
/* Bit: 'PFAD' */
/* Description: 'Pause Frame Source Address Part 1' */
#define MAC_PFSA_1_PFAD_OFFSET	0x8C3
#define MAC_PFSA_1_PFAD_SHIFT		0
#define MAC_PFSA_1_PFAD_SIZE		16
/* --------------------------------------------------- */
/* Register: 'Pause Frame SourceAddress Part 2 ' */
/* Bit: 'PFAD' */
/* Description: 'Pause Frame Source Address Part 2' */
#define MAC_PFSA_2_PFAD_OFFSET	0x8C4
#define MAC_PFSA_2_PFAD_SHIFT		0
#define MAC_PFSA_2_PFAD_SIZE		16
/* --------------------------------------------------- */
/* Register: 'MAC Frame Length Register' */
/* Bit: 'LEN' */
/* Description: 'Maximum Frame Length' */
#define MAC_FLEN_LEN_OFFSET	0x8C5
#define MAC_FLEN_LEN_SHIFT	0
#define MAC_FLEN_LEN_SIZE		14
/* --------------------------------------------------- */
/* Register: 'MAC VLAN EthertypeRegister 0' */
/* Bit: 'OUTER' */
/* Description: 'Ethertype' */
#define MAC_VLAN_ETYPE_0_OUTER_OFFSET	0x8C6
#define MAC_VLAN_ETYPE_0_OUTER_SHIFT	0
#define MAC_VLAN_ETYPE_0_OUTER_SIZE		16
/* --------------------------------------------------- */
/* Register: 'MAC VLAN EthertypeRegister 1' */
/* Bit: 'INNER' */
/* Description: 'Ethertype' */
#define MAC_VLAN_ETYPE_1_INNER_OFFSET	0x8C7
#define MAC_VLAN_ETYPE_1_INNER_SHIFT	0
#define MAC_VLAN_ETYPE_1_INNER_SIZE		16
/* --------------------------------------------------- */
/* Register: 'MAC Interrupt EnableRegister' */
/* Bit: 'MACIEN' */
/* Description: 'MAC Interrupt Enable' */
#define MAC_IER_MACIEN_OFFSET	0x8C8
#define MAC_IER_MACIEN_SHIFT	0
#define MAC_IER_MACIEN_SIZE		13
/* --------------------------------------------------- */
/* Register: 'MAC Interrupt StatusRegister' */
/* Bit: 'MACINT' */
/* Description: 'MAC Interrupt' */
#define MAC_ISR_MACINT_OFFSET	0x8C9
#define MAC_ISR_MACINT_SHIFT	0
#define MAC_ISR_MACINT_SIZE		13
/* --------------------------------------------------- */
/* Register: 'MAC Port Status Register' */
/* Bit: 'PACT' */
/* Description: 'PHY Active Status' */
#define MAC_PSTAT_PACT_OFFSET	0x900
#define MAC_PSTAT_PACT_SHIFT	11
#define MAC_PSTAT_PACT_SIZE		1
/* Bit: 'GBIT' */
/* Description: 'Gigabit Speed Status' */
#define MAC_PSTAT_GBIT_OFFSET	0x900
#define MAC_PSTAT_GBIT_SHIFT	10
#define MAC_PSTAT_GBIT_SIZE		1
/* Bit: 'MBIT' */
/* Description: 'Megabit Speed Status' */
#define MAC_PSTAT_MBIT_OFFSET	0x900
#define MAC_PSTAT_MBIT_SHIFT	9
#define MAC_PSTAT_MBIT_SIZE		1
/* Bit: 'FDUP' */
/* Description: 'Full Duplex Status' */
#define MAC_PSTAT_FDUP_OFFSET	0x900
#define MAC_PSTAT_FDUP_SHIFT	8
#define MAC_PSTAT_FDUP_SIZE		1
/* Bit: 'RXPAU' */
/* Description: 'Receive Pause Status' */
#define MAC_PSTAT_RXPAU_OFFSET	0x900
#define MAC_PSTAT_RXPAU_SHIFT		7
#define MAC_PSTAT_RXPAU_SIZE		1
/* Bit: 'TXPAU' */
/* Description: 'Transmit Pause Status' */
#define MAC_PSTAT_TXPAU_OFFSET	0x900
#define MAC_PSTAT_TXPAU_SHIFT		6
#define MAC_PSTAT_TXPAU_SIZE		1
/* Bit: 'RXPAUEN' */
/* Description: 'Receive Pause Enable Status' */
#define MAC_PSTAT_RXPAUEN_OFFSET	0x900
#define MAC_PSTAT_RXPAUEN_SHIFT		5
#define MAC_PSTAT_RXPAUEN_SIZE		1
/* Bit: 'TXPAUEN' */
/* Description: 'Transmit Pause Enable Status' */
#define MAC_PSTAT_TXPAUEN_OFFSET	0x900
#define MAC_PSTAT_TXPAUEN_SHIFT		4
#define MAC_PSTAT_TXPAUEN_SIZE		1
#define MAC_PSTAT_TXRXPAUEN_SIZE	2
/* Bit: 'LSTAT' */
/* Description: 'Link Status' */
#define MAC_PSTAT_LSTAT_OFFSET	0x900
#define MAC_PSTAT_LSTAT_SHIFT		3
#define MAC_PSTAT_LSTAT_SIZE		1
/* Bit: 'CRS' */
/* Description: 'Carrier Sense Status' */
#define MAC_PSTAT_CRS_OFFSET	0x900
#define MAC_PSTAT_CRS_SHIFT		2
#define MAC_PSTAT_CRS_SIZE		1
/* Bit: 'TXLPI' */
/* Description: 'Transmit Low-power Idle Status' */
#define MAC_PSTAT_TXLPI_OFFSET	0x900
#define MAC_PSTAT_TXLPI_SHIFT		1
#define MAC_PSTAT_TXLPI_SIZE		1
/* Bit: 'RXLPI' */
/* Description: 'Receive Low-power Idle Status' */
#define MAC_PSTAT_RXLPI_OFFSET	0x900
#define MAC_PSTAT_RXLPI_SHIFT		0
#define MAC_PSTAT_RXLPI_SIZE		1
/* --------------------------------------------------- */
/* Register: 'MAC Interrupt Status Register' */
/* Bit: 'PHYERR' */
/* Description: 'PHY Error Interrupt' */
#define MAC_PISR_PHYERR_OFFSET	0x901
#define MAC_PISR_PHYERR_SHIFT		15
#define MAC_PISR_PHYERR_SIZE		1
/* Bit: 'ALIGN' */
/* Description: 'Allignment Error Interrupt' */
#define MAC_PISR_ALIGN_OFFSET	0x901
#define MAC_PISR_ALIGN_SHIFT	14
#define MAC_PISR_ALIGN_SIZE		1
/* Bit: 'PACT' */
/* Description: 'PHY Active Status' */
#define MAC_PISR_PACT_OFFSET	0x901
#define MAC_PISR_PACT_SHIFT		13
#define MAC_PISR_PACT_SIZE		1
/* Bit: 'SPEED' */
/* Description: 'Megabit Speed Status' */
#define MAC_PISR_SPEED_OFFSET	0x901
#define MAC_PISR_SPEED_SHIFT	12
#define MAC_PISR_SPEED_SIZE		1
/* Bit: 'FDUP' */
/* Description: 'Full Duplex Status' */
#define MAC_PISR_FDUP_OFFSET	0x901
#define MAC_PISR_FDUP_SHIFT		11
#define MAC_PISR_FDUP_SIZE		1
/* Bit: 'RXPAUEN' */
/* Description: 'Receive Pause Enable Status' */
#define MAC_PISR_RXPAUEN_OFFSET	0x901
#define MAC_PISR_RXPAUEN_SHIFT	10
#define MAC_PISR_RXPAUEN_SIZE		1
/* Bit: 'TXPAUEN' */
/* Description: 'Transmit Pause Enable Status' */
#define MAC_PISR_TXPAUEN_OFFSET	0x901
#define MAC_PISR_TXPAUEN_SHIFT	9
#define MAC_PISR_TXPAUEN_SIZE		1
/* Bit: 'LPIOFF' */
/* Description: 'Receive Low-power Idle Mode is left' */
#define MAC_PISR_LPIOFF_OFFSET	0x901
#define MAC_PISR_LPIOFF_SHIFT		8
#define MAC_PISR_LPIOFF_SIZE		1
/* Bit: 'LPION' */
/* Description: 'Receive Low-power Idle Mode is entered' */
#define MAC_PISR_LPION_OFFSET	0x901
#define MAC_PISR_LPION_SHIFT	7
#define MAC_PISR_LPION_SIZE		1
/* Bit: 'JAM' */
/* Description: 'Jam Status Detected' */
#define MAC_PISR_JAM_OFFSET	0x901
#define MAC_PISR_JAM_SHIFT	6
#define MAC_PISR_JAM_SIZE		1
/* Bit: 'TOOSHORT' */
/* Description: 'Too Short Frame Error Detected' */
#define MAC_PISR_TOOSHORT_OFFSET	0x901
#define MAC_PISR_TOOSHORT_SHIFT		5
#define MAC_PISR_TOOSHORT_SIZE		1
/* Bit: 'TOOLONG' */
/* Description: 'Too Long Frame Error Detected' */
#define MAC_PISR_TOOLONG_OFFSET	0x901
#define MAC_PISR_TOOLONG_SHIFT	4
#define MAC_PISR_TOOLONG_SIZE		1
/* Bit: 'LENERR' */
/* Description: 'Length Mismatch Error Detected' */
#define MAC_PISR_LENERR_OFFSET	0x901
#define MAC_PISR_LENERR_SHIFT		3
#define MAC_PISR_LENERR_SIZE		1
/* Bit: 'FCSERR' */
/* Description: 'Frame Checksum Error Detected' */
#define MAC_PISR_FCSERR_OFFSET	0x901
#define MAC_PISR_FCSERR_SHIFT		2
#define MAC_PISR_FCSERR_SIZE		1
/* Bit: 'TXPAUSE' */
/* Description: 'Pause Frame Transmitted' */
#define MAC_PISR_TXPAUSE_OFFSET	0x901
#define MAC_PISR_TXPAUSE_SHIFT	1
#define MAC_PISR_TXPAUSE_SIZE		1
/* Bit: 'RXPAUSE' */
/* Description: 'Pause Frame Received' */
#define MAC_PISR_RXPAUSE_OFFSET	0x901
#define MAC_PISR_RXPAUSE_SHIFT	0
#define MAC_PISR_RXPAUSE_SIZE		1
/* --------------------------------------------------- */
/* Register: 'MAC Interrupt Enable Register' */
/* Bit: 'PHYERR' */
/* Description: 'PHY Error Interrupt' */
#define MAC_PIER_PHYERR_OFFSET	0x902
#define MAC_PIER_PHYERR_SHIFT		15
#define MAC_PIER_PHYERR_SIZE		1
/* Bit: 'ALIGN' */
/* Description: 'Allignment Error Interrupt' */
#define MAC_PIER_ALIGN_OFFSET	0x902
#define MAC_PIER_ALIGN_SHIFT	14
#define MAC_PIER_ALIGN_SIZE		1
/* Bit: 'PACT' */
/* Description: 'PHY Active Status' */
#define MAC_PIER_PACT_OFFSET	0x902
#define MAC_PIER_PACT_SHIFT		13
#define MAC_PIER_PACT_SIZE		1
/* Bit: 'SPEED' */
/* Description: 'Megabit Speed Status' */
#define MAC_PIER_SPEED_OFFSET	0x902
#define MAC_PIER_SPEED_SHIFT	12
#define MAC_PIER_SPEED_SIZE		1
/* Bit: 'FDUP' */
/* Description: 'Full Duplex Status' */
#define MAC_PIER_FDUP_OFFSET	0x902
#define MAC_PIER_FDUP_SHIFT		11
#define MAC_PIER_FDUP_SIZE		1
/* Bit: 'RXPAUEN' */
/* Description: 'Receive Pause Enable Status' */
#define MAC_PIER_RXPAUEN_OFFSET	0x902
#define MAC_PIER_RXPAUEN_SHIFT	10
#define MAC_PIER_RXPAUEN_SIZE		1
/* Bit: 'TXPAUEN' */
/* Description: 'Transmit Pause Enable Status' */
#define MAC_PIER_TXPAUEN_OFFSET	0x902
#define MAC_PIER_TXPAUEN_SHIFT	9
#define MAC_PIER_TXPAUEN_SIZE		1
/* Bit: 'LPIOFF' */
/* Description: 'Low-power Idle Off Interrupt Mask' */
#define MAC_PIER_LPIOFF_OFFSET	0x902
#define MAC_PIER_LPIOFF_SHIFT		8
#define MAC_PIER_LPIOFF_SIZE		1
/* Bit: 'LPION' */
/* Description: 'Low-power Idle On Interrupt Mask' */
#define MAC_PIER_LPION_OFFSET	0x902
#define MAC_PIER_LPION_SHIFT	7
#define MAC_PIER_LPION_SIZE		1
/* Bit: 'JAM' */
/* Description: 'Jam Status Interrupt Mask' */
#define MAC_PIER_JAM_OFFSET	0x902
#define MAC_PIER_JAM_SHIFT	6
#define MAC_PIER_JAM_SIZE		1
/* Bit: 'TOOSHORT' */
/* Description: 'Too Short Frame Error Interrupt Mask' */
#define MAC_PIER_TOOSHORT_OFFSET	0x902
#define MAC_PIER_TOOSHORT_SHIFT		5
#define MAC_PIER_TOOSHORT_SIZE		1
/* Bit: 'TOOLONG' */
/* Description: 'Too Long Frame Error Interrupt Mask' */
#define MAC_PIER_TOOLONG_OFFSET	0x902
#define MAC_PIER_TOOLONG_SHIFT	4
#define MAC_PIER_TOOLONG_SIZE		1
/* Bit: 'LENERR' */
/* Description: 'Length Mismatch Error Interrupt Mask' */
#define MAC_PIER_LENERR_OFFSET	0x902
#define MAC_PIER_LENERR_SHIFT		3
#define MAC_PIER_LENERR_SIZE		1
/* Bit: 'FCSERR' */
/* Description: 'Frame Checksum Error Interrupt Mask' */
#define MAC_PIER_FCSERR_OFFSET	0x902
#define MAC_PIER_FCSERR_SHIFT		2
#define MAC_PIER_FCSERR_SIZE		1
/* Bit: 'TXPAUSE' */
/* Description: 'Transmit Pause Frame Interrupt Mask' */
#define MAC_PIER_TXPAUSE_OFFSET	0x902
#define MAC_PIER_TXPAUSE_SHIFT	1
#define MAC_PIER_TXPAUSE_SIZE		1
/* Bit: 'RXPAUSE' */
/* Description: 'Receive Pause Frame Interrupt Mask' */
#define MAC_PIER_RXPAUSE_OFFSET	0x902
#define MAC_PIER_RXPAUSE_SHIFT	0
#define MAC_PIER_RXPAUSE_SIZE		1
/* --------------------------------------------------- */
/* Register: 'MAC Control Register0' */
/* Bit: 'BM' */
/* Description: 'Burst Mode Control' */
#define MAC_CTRL_0_BM_OFFSET	0x903
#define MAC_CTRL_0_BM_SHIFT		12
#define MAC_CTRL_0_BM_SIZE		1
/* Bit: 'APADEN' */
/* Description: 'Automatic VLAN Padding Enable' */
#define MAC_CTRL_0_APADEN_OFFSET	0x903
#define MAC_CTRL_0_APADEN_SHIFT		11
#define MAC_CTRL_0_APADEN_SIZE		1
/* Bit: 'VPAD2EN' */
/* Description: 'Stacked VLAN Padding Enable' */
#define MAC_CTRL_0_VPAD2EN_OFFSET	0x903
#define MAC_CTRL_0_VPAD2EN_SHIFT	10
#define MAC_CTRL_0_VPAD2EN_SIZE		1
/* Bit: 'VPADEN' */
/* Description: 'VLAN Padding Enable' */
#define MAC_CTRL_0_VPADEN_OFFSET	0x903
#define MAC_CTRL_0_VPADEN_SHIFT		9
#define MAC_CTRL_0_VPADEN_SIZE		1
/* Bit: 'PADEN' */
/* Description: 'Padding Enable' */
#define MAC_CTRL_0_PADEN_OFFSET	0x903
#define MAC_CTRL_0_PADEN_SHIFT	8
#define MAC_CTRL_0_PADEN_SIZE		1
/* Bit: 'FCS' */
/* Description: 'Transmit FCS Control' */
#define MAC_CTRL_0_FCS_OFFSET	0x903
#define MAC_CTRL_0_FCS_SHIFT	7
#define MAC_CTRL_0_FCS_SIZE		1
/* Bit: 'FCON' */
/* Description: 'Flow Control Mode' */
#define MAC_CTRL_0_FCON_OFFSET	0x903
#define MAC_CTRL_0_FCON_SHIFT		4
#define MAC_CTRL_0_FCON_SIZE		3
/* Bit: 'FDUP' */
/* Description: 'Full Duplex Control' */
#define MAC_CTRL_0_FDUP_OFFSET	0x903
#define MAC_CTRL_0_FDUP_SHIFT		2
#define MAC_CTRL_0_FDUP_SIZE		2
/* Bit: 'GMII' */
/* Description: 'GMII/MII interface mode selection' */
#define MAC_CTRL_0_GMII_OFFSET	0x903
#define MAC_CTRL_0_GMII_SHIFT		0
#define MAC_CTRL_0_GMII_SIZE		2
/* --------------------------------------------------- */
/* Register: 'MAC Control Register1' */
/* Bit: 'DEFERMODE' */
/* Description: 'Defer Model' */
#define MAC_CTRL_1_DEFERMODE_OFFSET	0x904
#define MAC_CTRL_1_DEFERMODE_SHIFT	15
#define MAC_CTRL_1_DEFERMODE_SIZE		1
/* Bit: 'SHORTPRE' */
/* Description: 'Short Preamble Control' */
#define MAC_CTRL_1_SHORTPRE_OFFSET	0x904
#define MAC_CTRL_1_SHORTPRE_SHIFT		8
#define MAC_CTRL_1_SHORTPRE_SIZE		1
/* Bit: 'IPG' */
/* Description: 'Minimum Inter Packet Gap Size' */
#define MAC_CTRL_1_IPG_OFFSET	0x904
#define MAC_CTRL_1_IPG_SHIFT	0
#define MAC_CTRL_1_IPG_SIZE		4
/* --------------------------------------------------- */
/* Register: 'MAC Control Register2' */
/* Bit: 'MLEN' */
/* Description: 'Maximum Untagged Frame Length' */
#define MAC_CTRL_2_MLEN_OFFSET	0x905
#define MAC_CTRL_2_MLEN_SHIFT		3
#define MAC_CTRL_2_MLEN_SIZE		1
/* Bit: 'LCHKL' */
/* Description: 'Frame Length Check Long Enable' */
#define MAC_CTRL_2_LCHKL_OFFSET	0x905
#define MAC_CTRL_2_LCHKL_SHIFT	2
#define MAC_CTRL_2_LCHKL_SIZE		1
/* Bit: 'LCHKS' */
/* Description: 'Frame Length Check Short Enable' */
#define MAC_CTRL_2_LCHKS_OFFSET	0x905
#define MAC_CTRL_2_LCHKS_SHIFT	0
#define MAC_CTRL_2_LCHKS_SIZE		2
/* --------------------------------------------------- */
/* Register: 'MAC Control Register3' */
/* Bit: 'RCNT' */
/* Description: 'Retry Count' */
#define MAC_CTRL_3_RCNT_OFFSET	0x906
#define MAC_CTRL_3_RCNT_SHIFT		0
#define MAC_CTRL_3_RCNT_SIZE		4
/* --------------------------------------------------- */
/* Register: 'MAC Control Register4' */
/* Bit: 'GWAIT' */
/* Description: 'LPI Wait Time for 1G' */
#define MAC_CTRL_4_GWAIT_OFFSET	0x907
#define MAC_CTRL_4_GWAIT_SHIFT	8
#define MAC_CTRL_4_GWAIT_SIZE		7
/* Bit: 'LPIEN' */
/* Description: 'LPI Mode Enable' */
#define MAC_CTRL_4_LPIEN_OFFSET	0x907
#define MAC_CTRL_4_LPIEN_SHIFT	7
#define MAC_CTRL_4_LPIEN_SIZE		1
/* Bit: 'WAIT' */
/* Description: 'LPI Wait Time for 100M' */
#define MAC_CTRL_4_WAIT_OFFSET	0x907
#define MAC_CTRL_4_WAIT_SHIFT		0
#define MAC_CTRL_4_WAIT_SIZE		7
/* --------------------------------------------------- */
/* Register: 'MAC Control Register5' */
/* Bit: 'PJPS_NOBP' */
/* Description: 'Prolonged Jam pattern size during no-backpressure */
/* state' */
#define MAC_CTRL_5_PJPS_NOBP_OFFSET	0x908
#define MAC_CTRL_5_PJPS_NOBP_SHIFT	1
#define MAC_CTRL_5_PJPS_NOBP_SIZE		1
/* Bit: 'PJPS_BP' */
/* Description: 'Prolonged Jam pattern size during backpressure state' */
#define MAC_CTRL_5_PJPS_BP_OFFSET	0x908
#define MAC_CTRL_5_PJPS_BP_SHIFT	0
#define MAC_CTRL_5_PJPS_BP_SIZE		1
/* --------------------------------------------------- */
/* Register: 'MAC LPI Timer Register 0' */
/* Bit: 'TMLSB' */
/* Description: 'MAC LPI Timer LSB' */
#define MAC_LPITMER0_TMLSB_OFFSET	0x909
#define MAC_LPITMER0_TMLSB_SHIFT	0
#define MAC_LPITMER0_TMLSB_SIZE		16
/* --------------------------------------------------- */
/* Register: 'MAC LPI Timer Register 1' */
/* Bit: 'TMMSB' */
/* Description: 'MAC LPI Timer MSB' */
#define MAC_LPITMER1_TMMSB_OFFSET	0x90A
#define MAC_LPITMER1_TMMSB_SHIFT	0
#define MAC_LPITMER1_TMMSB_SIZE		16
/* --------------------------------------------------- */
/* Register: 'MAC Test Enable Register' */
/* Bit: 'LPITM' */
/* Description: 'LPI Monitoring Mode' */
#define MAC_TESTEN_LPITM_OFFSET	0x90B
#define MAC_TESTEN_LPITM_SHIFT	8
#define MAC_TESTEN_LPITM_SIZE		2
/* Bit: 'JTEN' */
/* Description: 'Jitter Test Enable' */
#define MAC_TESTEN_JTEN_OFFSET	0x90B
#define MAC_TESTEN_JTEN_SHIFT		2
#define MAC_TESTEN_JTEN_SIZE		1
/* Bit: 'TXER' */
/* Description: 'Transmit Error Insertion' */
#define MAC_TESTEN_TXER_OFFSET	0x90B
#define MAC_TESTEN_TXER_SHIFT		1
#define MAC_TESTEN_TXER_SIZE		1
/* Bit: 'LOOP' */
/* Description: 'MAC Loopback Enable' */
#define MAC_TESTEN_LOOP_OFFSET	0x90B
#define MAC_TESTEN_LOOP_SHIFT		0
#define MAC_TESTEN_LOOP_SIZE		1
/* --------------------------------------------------- */
/* Register: 'Ethernet Switch FetchDMA Control Register' (LTQ_GSWIP_2_2_ETC)*/
/* Bit: 'RMMD' */
/* Description: 'Remarking Mode' */
#define FDMA_CTRL_RMMD_OFFSET	0xA40
#define FDMA_CTRL_RMMD_SHIFT	8
#define FDMA_CTRL_RMMD_SIZE		1
/* Bit: 'EGCNT' */
/* Description: 'Egress Special Tag RMON count' */
#define FDMA_CTRL_EGCNT_OFFSET	0xA40
#define FDMA_CTRL_EGCNT_SHIFT		7
#define FDMA_CTRL_EGCNT_SIZE		1
/* Bit: 'LPI_MODE' */
/* Description: 'Low Power Idle Mode' */
#define FDMA_CTRL_LPI_MODE_OFFSET	0xA40
#define FDMA_CTRL_LPI_MODE_SHIFT	4
#define FDMA_CTRL_LPI_MODE_SIZE		3
/* Bit: 'EGSTAG' */
/* Description: 'Egress Special Tag Size' */
#define FDMA_CTRL_EGSTAG_OFFSET	0xA40
#define FDMA_CTRL_EGSTAG_SHIFT	2
#define FDMA_CTRL_EGSTAG_SIZE		2
/* Bit: 'IGSTAG' */
/* Description: 'Ingress Special Tag Size' */
#define FDMA_CTRL_IGSTAG_OFFSET	0xA40
#define FDMA_CTRL_IGSTAG_SHIFT	1
#define FDMA_CTRL_IGSTAG_SIZE		1
/* --------------------------------------------------- */
/* Register: 'Special Tag EthertypeControl Register' */
/* Bit: 'ETYPE' */
/* Description: 'Special Tag Ethertype' */
#define FDMA_STETYPE_ETYPE_OFFSET	0xA41
#define FDMA_STETYPE_ETYPE_SHIFT	0
#define FDMA_STETYPE_ETYPE_SIZE		16
/* --------------------------------------------------- */
/* Register: 'VLAN Tag EthertypeControl Register' */
/* Bit: 'ETYPE' */
/* Description: 'VLAN Tag Ethertype' */
#define FDMA_VTETYPE_ETYPE_OFFSET	0xA42
#define FDMA_VTETYPE_ETYPE_SHIFT	0
#define FDMA_VTETYPE_ETYPE_SIZE		16
/* --------------------------------------------------- */
/* Register: 'FDMA Status Register0' */
/* Bit: 'FSMS' */
/* Description: 'FSM states status' */
#define FDMA_STAT_0_FSMS_OFFSET	0xA43
#define FDMA_STAT_0_FSMS_SHIFT	0
#define FDMA_STAT_0_FSMS_SIZE		16
/* --------------------------------------------------- */
/* Register: 'Fetch DMA Global InterruptEnable Register' */
/* Bit: 'PCKD' */
/* Description: 'Packet Drop Interrupt Enable' */
#define FDMA_IER_PCKD_OFFSET	0xA44
#define FDMA_IER_PCKD_SHIFT		14
#define FDMA_IER_PCKD_SIZE		1
/* Bit: 'PCKR' */
/* Description: 'Packet Ready Interrupt Enable' */
#define FDMA_IER_PCKR_OFFSET	0xA44
#define FDMA_IER_PCKR_SHIFT		13
#define FDMA_IER_PCKR_SIZE		1
/* Bit: 'PCKT' */
/* Description: 'Packet Sent Interrupt Enable' */
#define FDMA_IER_PCKT_OFFSET	0xA44
#define FDMA_IER_PCKT_SHIFT		0
#define FDMA_IER_PCKT_SIZE		13
/* --------------------------------------------------- */
/* Register: 'Fetch DMA Global InterruptStatus Register' */
/* Bit: 'PCKTD' */
/* Description: 'Packet Drop' */
#define FDMA_ISR_PCKTD_OFFSET	0xA45
#define FDMA_ISR_PCKTD_SHIFT	14
#define FDMA_ISR_PCKTD_SIZE		1
/* Bit: 'PCKR' */
/* Description: 'Packet is Ready for Transmission' */
#define FDMA_ISR_PCKR_OFFSET	0xA45
#define FDMA_ISR_PCKR_SHIFT		13
#define FDMA_ISR_PCKR_SIZE		1
/* Bit: 'PCKT' */
/* Description: 'Packet Sent Event' */
#define FDMA_ISR_PCKT_OFFSET	0xA45
#define FDMA_ISR_PCKT_SHIFT		0
#define FDMA_ISR_PCKT_SIZE		13
/* --------------------------------------------------- */
/* Register: 'Service VLAN Tag Ethertype Control Register' */
/* (LTQ_GSWIP_2_2_ETC)*/
/* Bit: 'ETYPE' */
/* Description: 'A VLAN tag can be inserted into egress frames, */
/* identified by this Ethertype value.' */
#define FDMA_SVTETYPE_OFFSET			0xA46
#define FDMA_SVTETYPE_ETYPE_SHIFT	0
#define FDMA_SVTETYPE_ETYPE_SIZE	16
/* --------------------------------------------------- */
/* Register: 'Fetch DMA Parsing Result Control Register' (LTQ_GSWIP_3_0)*/
/* Bit: 'MPE3' */
/* Description: 'Parsing Results to CPU Port MPE3.' */
#define FDMA_PASR_MPE3_OFFSET	0xA47
#define FDMA_PASR_MPE3_SHIFT	6
#define FDMA_PASR_MPE3_SIZE		2
/* Bit: 'MPE2' */
/* Description: 'Parsing Results to CPU Port MPE2.' */
#define FDMA_PASR_MPE2_OFFSET	0xA47
#define FDMA_PASR_MPE2_SHIFT	4
#define FDMA_PASR_MPE2_SIZE		2
/* Bit: 'MPE1' */
/* Description: 'Parsing Results to CPU Port MPE1.' */
#define FDMA_PASR_MPE1_OFFSET	0xA47
#define FDMA_PASR_MPE1_SHIFT	2
#define FDMA_PASR_MPE1_SIZE		2
/* Bit: 'CPU' */
/* Description: 'Parsing Results to CPU Port.' */
#define FDMA_PASR_CPU_OFFSET	0xA47
#define FDMA_PASR_CPU_SHIFT		0
#define FDMA_PASR_CPU_SIZE		2
/* --------------------------------------------------- */
/* Register:'Fetch DMA Global Interrupt Enable Register 1'(LTQ_GSWIP_3_0)*/
/* Bit: 'PCKD' */
/* Description: 'Packet Drop Interrupt Enable.' */
#define FDMA_IER_1_PCKD_OFFSET	0xA48
#define FDMA_IER_1_PCKD_SHIFT		14
#define FDMA_IER_1_PCKD_SIZE		1
/* Bit: 'PCKR' */
/* Description: 'Packet Ready Interrupt Enable.' */
#define FDMA_IER_1_PCKR_OFFSET	0xA48
#define FDMA_IER_1_PCKR_SHIFT		13
#define FDMA_IER_1_PCKR_SIZE		1
/* --------------------------------------------------- */
/* Register:'Fetch DMA Global Interrupt Status Register 1'(LTQ_GSWIP_3_0)*/
/* Bit: 'PCKTD' */
/* Description: 'Packet Drop.' */
#define FDMA_ISR_1_PCKTD_OFFSET	0xA49
#define FDMA_ISR_1_PCKTD_SHIFT	14
#define FDMA_ISR_1_PCKTD_SIZE		1
/* Bit: 'PCKR' */
/* Description: 'Packet is Ready for Transmission.' */
#define FDMA_ISR_1_PCKR_OFFSET	0xA49
#define FDMA_ISR_1_PCKR_SHIFT		13
#define FDMA_ISR_1_PCKR_SIZE		1
/* --------------------------------------------------- */
/* Register: 'Ethernet SwitchFetch DMA Port Control Register' */
/* Bit: 'DEIMOD' */
/* Description: 'Controls DEI remarking mode.' */
#define FDMA_PCTRL_DEIMOD_OFFSET	0xA80
#define FDMA_PCTRL_DEIMOD_SHIFT		9
#define FDMA_PCTRL_DEIMOD_SIZE		1
/* Bit: 'SFDSTB' */
/* Description: 'Controls selection of SFD strobe for time stamp capture..' */
#define FDMA_PCTRL_SFDSTB_OFFSET	0xA80
#define FDMA_PCTRL_SFDSTB_SHIFT		8
#define FDMA_PCTRL_SFDSTB_SIZE		1
/* Bit: 'SVLANMOD' */
/* Description: 'Controls the VLAN tag modification function on egress.' */
#define FDMA_PCTRL_SVLANMOD_OFFSET	0xA80
#define FDMA_PCTRL_SVLANMOD_SHIFT		6
#define FDMA_PCTRL_SVLANMOD_SIZE		2
/* Bit: 'ST_TYPE' */
/* Description: 'Special Tag Ethertype Mode' */
#define FDMA_PCTRL_ST_TYPE_OFFSET	0xA80
#define FDMA_PCTRL_ST_TYPE_SHIFT	5
#define FDMA_PCTRL_ST_TYPE_SIZE		1
/* Bit: 'VLANMOD' */
/* Description: 'VLAN Modification Control' */
#define FDMA_PCTRL_VLANMOD_OFFSET	0xA80
#define FDMA_PCTRL_VLANMOD_SHIFT	3
#define FDMA_PCTRL_VLANMOD_SIZE		2
/* Bit: 'DSCPRM' */
/* Description: 'DSCP Re-marking Enable' */
#define FDMA_PCTRL_DSCPRM_OFFSET	0xA80
#define FDMA_PCTRL_DSCPRM_SHIFT		2
#define FDMA_PCTRL_DSCPRM_SIZE		1
/* Bit: 'STEN' */
/* Description: 'Special Tag Insertion Enable' */
#define FDMA_PCTRL_STEN_OFFSET	0xA80
#define FDMA_PCTRL_STEN_SHIFT		1
#define FDMA_PCTRL_STEN_SIZE		1
/* Bit: 'EN' */
/* Description: 'FDMA Port Enable' */
#define FDMA_PCTRL_EN_OFFSET	0xA80
#define FDMA_PCTRL_EN_SHIFT		0
#define FDMA_PCTRL_EN_SIZE		1
/* --------------------------------------------------- */
/* Register: 'Ethernet SwitchFetch DMA Port Priority Register' */
/* Bit: 'PRIO' */
/* Description: 'FDMA PRIO' */
#define FDMA_PRIO_PRIO_OFFSET	0xA81
#define FDMA_PRIO_PRIO_SHIFT	0
#define FDMA_PRIO_PRIO_SIZE		2
/* --------------------------------------------------- */
/* Register: 'Ethernet SwitchFetch DMA Port Status Register 0' */
/* Bit: 'PKT_AVAIL' */
/* Description: 'Port Egress Packet Available' */
#define FDMA_PSTAT0_PKT_AVAIL_OFFSET	0xA82
#define FDMA_PSTAT0_PKT_AVAIL_SHIFT		15
#define FDMA_PSTAT0_PKT_AVAIL_SIZE		1
/* Bit: 'POK' */
/* Description: 'Port Status OK' */
#define FDMA_PSTAT0_POK_OFFSET	0xA82
#define FDMA_PSTAT0_POK_SHIFT		14
#define FDMA_PSTAT0_POK_SIZE		1
/* Bit: 'PSEG' */
/* Description: 'Port Egress Segment Count' */
#define FDMA_PSTAT0_PSEG_OFFSET	0xA82
#define FDMA_PSTAT0_PSEG_SHIFT	0
#define FDMA_PSTAT0_PSEG_SIZE		6
/* --------------------------------------------------- */
/* Register: 'Ethernet SwitchFetch DMA Port Status Register 1' */
/* Bit: 'HDR_PTR' */
/* Description: 'Header Pointer' */
#define FDMA_PSTAT1_HDR_PTR_OFFSET	0xA83
#define FDMA_PSTAT1_HDR_PTR_SHIFT		0
#define FDMA_PSTAT1_HDR_PTR_SIZE		10
/* --------------------------------------------------- */
/* Register: 'Egress TimeStamp Register 0' */
/* Bit: 'TSTL' */
/* Description: 'Time Stamp [15:0]' */
#define FDMA_TSTAMP0_TSTL_OFFSET	0xA84
#define FDMA_TSTAMP0_TSTL_SHIFT		0
#define FDMA_TSTAMP0_TSTL_SIZE		16
/* --------------------------------------------------- */
/* Register: 'Egress TimeStamp Register 1' */
/* Bit: 'TSTH' */
/* Description: 'Time Stamp [31:16]' */
#define FDMA_TSTAMP1_TSTH_OFFSET	0xA85
#define FDMA_TSTAMP1_TSTH_SHIFT		0
#define FDMA_TSTAMP1_TSTH_SIZE		16
/* --------------------------------------------------- */
/* Register: 'Ethernet Switch StoreDMA Control Register' */
/* Bit: 'RMON_ALIGN' */
/* Description: 'MUX to select what to count on align error rmon */
/* counter' */
#define SDMA_CTRL_RMON_ALIGN_OFFSET	0xB40
#define SDMA_CTRL_RMON_ALIGN_SHIFT	2
#define SDMA_CTRL_RMON_ALIGN_SIZE		2
/* Bit: 'ARBIT' */
/* Description: 'SIMPLE ARBITER FOR PARSER FILLING' */
#define SDMA_CTRL_ARBIT_OFFSET	0xB40
#define SDMA_CTRL_ARBIT_SHIFT		1
#define SDMA_CTRL_ARBIT_SIZE		1
/* Bit: 'TSTEN' */
/* Description: 'Time Stamp Enable' */
#define SDMA_CTRL_TSTEN_OFFSET	0xB40
#define SDMA_CTRL_TSTEN_SHIFT		0
#define SDMA_CTRL_TSTEN_SIZE		1
/* --------------------------------------------------- */
/* Register: 'SDMA Flow Control Threshold1 Register' */
/* Bit: 'THR1' */
/* Description: 'Threshold 1' */
#define SDMA_FCTHR1_THR1_OFFSET	0xB41
#define SDMA_FCTHR1_THR1_SHIFT	0
#define SDMA_FCTHR1_THR1_SIZE		10
/* --------------------------------------------------- */
/* Register: 'SDMA Flow Control Threshold2 Register' */
/* Bit: 'THR2' */
/* Description: 'Threshold 2' */
#define SDMA_FCTHR2_THR2_OFFSET	0xB42
#define SDMA_FCTHR2_THR2_SHIFT	0
#define SDMA_FCTHR2_THR2_SIZE		10
/* --------------------------------------------------- */
/* Register: 'SDMA Flow Control Threshold3 Register' */
/* Bit: 'THR3' */
/* Description: 'Threshold 3' */
#define SDMA_FCTHR3_THR3_OFFSET	0xB43
#define SDMA_FCTHR3_THR3_SHIFT	0
#define SDMA_FCTHR3_THR3_SIZE		10
/* --------------------------------------------------- */
/* Register: 'SDMA Flow Control Threshold4 Register' */
/* Bit: 'THR4' */
/* Description: 'Threshold 4' */
#define SDMA_FCTHR4_THR4_OFFSET	0xB44
#define SDMA_FCTHR4_THR4_SHIFT	0
#define SDMA_FCTHR4_THR4_SIZE		10
/* --------------------------------------------------- */
/* Register: 'SDMA Flow Control Threshold5 Register' */
/* Bit: 'THR5' */
/* Description: 'Threshold 5' */
#define SDMA_FCTHR5_THR5_OFFSET	0xB45
#define SDMA_FCTHR5_THR5_SHIFT	0
#define SDMA_FCTHR5_THR5_SIZE		10
/* --------------------------------------------------- */
/* Register: 'SDMA Flow Control Threshold6 Register' */
/* Bit: 'THR6' */
/* Description: 'Threshold 6' */
#define SDMA_FCTHR6_THR6_OFFSET	0xB46
#define SDMA_FCTHR6_THR6_SHIFT	0
#define SDMA_FCTHR6_THR6_SIZE		10
/* --------------------------------------------------- */
/* Register: 'SDMA Flow Control Threshold7 Register' */
/* Bit: 'THR7' */
/* Description: 'Threshold 7' */
#define SDMA_FCTHR7_THR7_OFFSET	0xB47
#define SDMA_FCTHR7_THR7_SHIFT	0
#define SDMA_FCTHR7_THR7_SIZE		11
/* --------------------------------------------------- */
/* Register: 'SDMA Status Register0' */
/* Bit: 'BPS_FILL' */
/* Description: 'Back Pressure Status' */
#define SDMA_STAT_0_BPS_FILL_OFFSET	0xB48
#define SDMA_STAT_0_BPS_FILL_SHIFT	4
#define SDMA_STAT_0_BPS_FILL_SIZE		3
/* Bit: 'BPS_PNT' */
/* Description: 'Back Pressure Status' */
#define SDMA_STAT_0_BPS_PNT_OFFSET	0xB48
#define SDMA_STAT_0_BPS_PNT_SHIFT	2
#define SDMA_STAT_0_BPS_PNT_SIZE	2
/* Bit: 'DROP' */
/* Description: 'Back Pressure Status' */
#define SDMA_STAT_0_DROP_OFFSET	0xB48
#define SDMA_STAT_0_DROP_SHIFT	0
#define SDMA_STAT_0_DROP_SIZE		2
/* --------------------------------------------------- */
/* Register: 'SDMA Status Register1' */
/* Bit: 'FILL' */
/* Description: 'Buffer Filling Level' */
#define SDMA_STAT_1_FILL_OFFSET	0xB49
#define SDMA_STAT_1_FILL_SHIFT	0
#define SDMA_STAT_1_FILL_SIZE		10
/* --------------------------------------------------- */
/* Register: 'SDMA Status Register2' */
/* Bit: 'FSMS' */
/* Description: 'FSM states status' */
#define SDMA_STAT_2_FSMS_OFFSET	0xB4A
#define SDMA_STAT_2_FSMS_SHIFT	0
#define SDMA_STAT_2_FSMS_SIZE		16
/* --------------------------------------------------- */
/* Register: 'SDMA Interrupt Enable Register' */
/* Bit: 'BPEX' */
/* Description: 'Buffer Pointers Exceeded' */
#define SDMA_IER_BPEX_OFFSET	0xB4B
#define SDMA_IER_BPEX_SHIFT		15
#define SDMA_IER_BPEX_SIZE		1
/* Bit: 'BFULL' */
/* Description: 'Buffer Full' */
#define SDMA_IER_BFULL_OFFSET	0xB4B
#define SDMA_IER_BFULL_SHIFT	14
#define SDMA_IER_BFULL_SIZE		1
/* Bit: 'FERR' */
/* Description: 'Frame Error' */
#define SDMA_IER_FERR_OFFSET	0xB4B
#define SDMA_IER_FERR_SHIFT		13
#define SDMA_IER_FERR_SIZE		1
/* Bit: 'FRX' */
/* Description: 'Frame Received Successfully' */
#define SDMA_IER_FRX_OFFSET		0xB4B
#define SDMA_IER_FRX_SHIFT		0
#define SDMA_IER_FRX_SIZE			13
/* --------------------------------------------------- */
/* Register: 'SDMA Interrupt Status Register' */
/* Bit: 'BPEX' */
/* Description: 'Packet Descriptors Exceeded' */
#define SDMA_ISR_BPEX_OFFSET	0xB4C
#define SDMA_ISR_BPEX_SHIFT		15
#define SDMA_ISR_BPEX_SIZE		1
/* Bit: 'BFULL' */
/* Description: 'Buffer Full' */
#define SDMA_ISR_BFULL_OFFSET	0xB4C
#define SDMA_ISR_BFULL_SHIFT	14
#define SDMA_ISR_BFULL_SIZE		1
/* Bit: 'FERR' */
/* Description: 'Frame Error' */
#define SDMA_ISR_FERR_OFFSET	0xB4C
#define SDMA_ISR_FERR_SHIFT		13
#define SDMA_ISR_FERR_SIZE		1
/* Bit: 'FRX' */
/* Description: 'Frame Received Successfully' */
#define SDMA_ISR_FRX_OFFSET	0xB4C
#define SDMA_ISR_FRX_SHIFT	0
#define SDMA_ISR_FRX_SIZE		13
/* --------------------------------------------------- */
/* Register: 'SDMA Interrupt Enable Register 1' (LTQ_GSWIP_2_2) */
/* Bit: 'CGNBP' */
/* Description: 'Ingress Port Congestion Based Back Pressure Status Change' */
#define SDMA_IER_1_CGNBP_OFFSET	0xB4D
#define SDMA_IER_1_CGNBP_SHIFT	0
#define SDMA_IER_1_CGNBP_SIZE		13
/* --------------------------------------------------- */
/* Register: 'SDMA Interrupt Enable Register 1' (LTQ_GSWIP_2_2)*/
/* Bit: 'CGNBP' */
/* Description: 'Ingress Port Congested Based Backpressure Status Change' */
#define SDMA_ISR_1_CGNBP_OFFSET	0xB4E
#define SDMA_ISR_1_CGNBP_SHIFT	0
#define SDMA_ISR_1_CGNBP_SIZE		13
/* --------------------------------------------------- */
/* Register: 'SDMA Congestion Backpressure Status Register' (LTQ_GSWIP_2_2)*/
/* Bit: 'CGNBP' */
/* Description: 'Ingress Port Congestion Based Back Pressure Status' */
#define SDMA_CGNBP_CGNBP_OFFSET	0xB4F
#define SDMA_CGNBP_CGNBP_SHIFT	0
#define SDMA_CGNBP_CGNBP_SIZE		13
/* --------------------------------------------------- */
/* Register: 'SDMA Interrupt Enable Register 2' (LTQ_GSWIP_3_0)*/
/* Bit: 'BPEX' */
/* Description: 'Buffer Pointers Exceeded' */
#define SDMA_IER_2_BPEX_OFFSET	0xB50
#define SDMA_IER_2_BPEX_SHIFT		15
#define SDMA_IER_2_BPEX_SIZE		1
/* Bit: 'BFULL' */
/* Description: 'Buffer Full' */
#define SDMA_IER_2_BFULL_OFFSET	0xB50
#define SDMA_IER_2_BFULL_SHIFT	14
#define SDMA_IER_2_BFULL_SIZE		1
/* Bit: 'FERR' */
/* Description: 'Frame Error' */
#define SDMA_IER_2_FERR_OFFSET	0xB50
#define SDMA_IER_2_FERR_SHIFT		13
#define SDMA_IER_2_FERR_SIZE		1
/* --------------------------------------------------- */
/* Register: 'SDMA Interrupt Status Register 2' (LTQ_GSWIP_3_0)*/
/* Bit: 'BPEX' */
/* Description: 'Packet Descriptors Exceeded' */
#define SDMA_ISR_2_BPEX_OFFSET	0xB51
#define SDMA_ISR_2_BPEX_SHIFT		15
#define SDMA_ISR_2_BPEX_SIZE		1
/* Bit: 'BFULL' */
/* Description: 'Buffer Full' */
#define SDMA_ISR_2_BFULL_OFFSET	0xB51
#define SDMA_ISR_2_BFULL_SHIFT	14
#define SDMA_ISR_2_BFULL_SIZE		1
/* Bit: 'FERR' */
/* Description: 'Frame Error' */
#define SDMA_ISR_2_FERR_OFFSET	0xB51
#define SDMA_ISR_2_FERR_SHIFT		13
#define SDMA_ISR_2_FERR_SIZE		1
/* --------------------------------------------------- */
/* Register: 'Ethernet SwitchStore DMA Port Control Register'(LTQ_GSWIP_2_2) */
/* Bit: 'SFDSTB' */
/* Description: 'Drop Threshold Selection' */
#define SDMA_PCTRL_SFDSTB_OFFSET	0xBC0
#define SDMA_PCTRL_SFDSTB_SHIFT		15
#define SDMA_PCTRL_SFDSTB_SIZE		1
/* Bit: 'DTHR' */
/* Description: 'Drop Threshold Selection' */
#define SDMA_PCTRL_DTHR_OFFSET	0xBC0
#define SDMA_PCTRL_DTHR_SHIFT		13
#define SDMA_PCTRL_DTHR_SIZE		2
/* Bit: 'PTHR' */
/* Description: 'Pause Threshold Selection' */
#define SDMA_PCTRL_PTHR_OFFSET	0xBC0
#define SDMA_PCTRL_PTHR_SHIFT		11
#define SDMA_PCTRL_PTHR_SIZE		2
/* Bit: 'PHYEFWD' */
/* Description: 'Forward PHY Error Frames' */
#define SDMA_PCTRL_PHYEFWD_OFFSET	0xBC0
#define SDMA_PCTRL_PHYEFWD_SHIFT	10
#define SDMA_PCTRL_PHYEFWD_SIZE		1
/* Bit: 'ALGFWD' */
/* Description: 'Forward Alignment Error Frames' */
#define SDMA_PCTRL_ALGFWD_OFFSET	0xBC0
#define SDMA_PCTRL_ALGFWD_SHIFT		9
#define SDMA_PCTRL_ALGFWD_SIZE		1
/* Bit: 'LENFWD' */
/* Description: 'Forward Length Errored Frames' */
#define SDMA_PCTRL_LENFWD_OFFSET	0xBC0
#define SDMA_PCTRL_LENFWD_SHIFT		8
#define SDMA_PCTRL_LENFWD_SIZE		1
/* Bit: 'OSFWD' */
/* Description: 'Forward Oversized Frames' */
#define SDMA_PCTRL_OSFWD_OFFSET	0xBC0
#define SDMA_PCTRL_OSFWD_SHIFT	7
#define SDMA_PCTRL_OSFWD_SIZE		1
/* Bit: 'USFWD' */
/* Description: 'Forward Undersized Frames' */
#define SDMA_PCTRL_USFWD_OFFSET 0xBC0
#define SDMA_PCTRL_USFWD_SHIFT	6
#define SDMA_PCTRL_USFWD_SIZE		1
/* Bit: 'FCSIGN' */
/* Description: 'Ignore FCS Errors' */
#define SDMA_PCTRL_FCSIGN_OFFSET	0xBC0
#define SDMA_PCTRL_FCSIGN_SHIFT		5
#define SDMA_PCTRL_FCSIGN_SIZE		1
/* Bit: 'FCSFWD' */
/* Description: 'Forward FCS Errored Frames' */
#define SDMA_PCTRL_FCSFWD_OFFSET	0xBC0
#define SDMA_PCTRL_FCSFWD_SHIFT		4
#define SDMA_PCTRL_FCSFWD_SIZE		1
/* Bit: 'PAUFWD' */
/* Description: 'Pause Frame Forwarding' */
#define SDMA_PCTRL_PAUFWD_OFFSET	0xBC0
#define SDMA_PCTRL_PAUFWD_SHIFT		3
#define SDMA_PCTRL_PAUFWD_SIZE		1
/* Bit: 'MFCEN' */
/* Description: 'Metering Flow Control Enable' */
#define SDMA_PCTRL_MFCEN_OFFSET	0xBC0
#define SDMA_PCTRL_MFCEN_SHIFT	2
#define SDMA_PCTRL_MFCEN_SIZE		1
/* Bit: 'FCEN' */
/* Description: 'Flow Control Enable' */
#define SDMA_PCTRL_FCEN_OFFSET	0xBC0
#define SDMA_PCTRL_FCEN_SHIFT		1
#define SDMA_PCTRL_FCEN_SIZE		1
/* Bit: 'PEN' */
/* Description: 'Port Enable' */
#define SDMA_PCTRL_PEN_OFFSET	0xBC0
#define SDMA_PCTRL_PEN_SHIFT	0
#define SDMA_PCTRL_PEN_SIZE		1
/* --------------------------------------------------- */
/* Register: 'Ethernet SwitchStore DMA Port Priority Register' */
/* Bit: 'MIN_IFG' */
/* Description: 'Minimum IFG,SFD and preamble' */
#define SDMA_PRIO_MIN_IFG_OFFSET	0xBC1
#define SDMA_PRIO_MIN_IFG_SHIFT		7
#define SDMA_PRIO_MIN_IFG_SIZE		5
/* Bit: 'PHYEIGN' */
/* Description: 'Ignore PHY Error Frames' */
#define SDMA_PRIO_PHYEIGN_OFFSET	0xBC1
#define SDMA_PRIO_PHYEIGN_SHIFT		6
#define SDMA_PRIO_PHYEIGN_SIZE		1
/* Bit: 'ALGIGN' */
/* Description: 'Ignore Alignment Error Frames' */
#define SDMA_PRIO_ALGIGN_OFFSET	0xBC1
#define SDMA_PRIO_ALGIGN_SHIFT	5
#define SDMA_PRIO_ALGIGN_SIZE		1
/* Bit: 'LENIGN' */
/* Description: 'Ignore Length Errored Frames' */
#define SDMA_PRIO_LENIGN_OFFSET	0xBC1
#define SDMA_PRIO_LENIGN_SHIFT	4
#define SDMA_PRIO_LENIGN_SIZE		1
/* Bit: 'OSIGN' */
/* Description: 'Ignore Oversized Frames' */
#define SDMA_PRIO_OSIGN_OFFSET	0xBC1
#define SDMA_PRIO_OSIGN_SHIFT		3
#define SDMA_PRIO_OSIGN_SIZE		1
/* Bit: 'USIGN' */
/* Description: 'Ignore Undersized Frames' */
#define SDMA_PRIO_USIGN_OFFSET	0xBC1
#define SDMA_PRIO_USIGN_SHIFT		2
#define SDMA_PRIO_USIGN_SIZE		1
/* Bit: 'PRIO' */
/* Description: 'SDMA PRIO' */
#define SDMA_PRIO_PRIO_OFFSET	0xBC1
#define SDMA_PRIO_PRIO_SHIFT	0
#define SDMA_PRIO_PRIO_SIZE		2
/* --------------------------------------------------- */
/* Register: 'Ethernet SwitchStore DMA Port Status Register 0' */
/* Bit: 'HDR_PTR' */
/* Description: 'Port Ingress Queue Header Pointer' */
#define SDMA_PSTAT0_HDR_PTR_OFFSET	0xBC2
#define SDMA_PSTAT0_HDR_PTR_SHIFT		0
#define SDMA_PSTAT0_HDR_PTR_SIZE		10
/* --------------------------------------------------- */
/* Register: 'Ethernet SwitchStore DMA Port Status Register 1' */
/* Bit: 'PPKT' */
/* Description: 'Port Ingress Packet Count' */
#define SDMA_PSTAT1_PPKT_OFFSET	0xBC3
#define SDMA_PSTAT1_PPKT_SHIFT	0
#define SDMA_PSTAT1_PPKT_SIZE		10
/* --------------------------------------------------- */
/* Register: 'Ingress TimeStamp Register 0' */
/* Bit: 'TSTL' */
/* Description: 'Time Stamp [15:0]' */
#define SDMA_TSTAMP0_TSTL_OFFSET	0xBC4
#define SDMA_TSTAMP0_TSTL_SHIFT		0
#define SDMA_TSTAMP0_TSTL_SIZE		16
/* --------------------------------------------------- */
/* Register: 'Ingress TimeStamp Register 1' */
/* Bit: 'TSTH' */
/* Description: 'Time Stamp [31:16]' */
#define SDMA_TSTAMP1_TSTH_OFFSET	0xBC5
#define SDMA_TSTAMP1_TSTH_SHIFT		0
#define SDMA_TSTAMP1_TSTH_SIZE		16
/* --------------------------------------------------- */
/* Register: 'Ingress Port Flow Control Threshold 8 Register' (LTQ_GSWIP_2_2)*/
/* Bit: 'THR8' */
/* Description: 'Threshold 8' */
#define SDMA_PFCTHR8_THR8_OFFSET22	0xC0E
#define SDMA_PFCTHR8_THR8_OFFSET30	0xB80  /* (LTQ_GSWIP_3_0) */
#define SDMA_PFCTHR8_THR8_SHIFT		0
#define SDMA_PFCTHR8_THR8_SIZE		11
/* --------------------------------------------------- */
/* Register: 'Ingress Port Flow Control Threshold 9 Register' (LTQ_GSWIP_2_2)*/
/* Bit: 'THR9' */
/* Description: 'Threshold 8' */
#define SDMA_PFCTHR9_THR9_OFFSET22	0xC0F
#define SDMA_PFCTHR9_THR9_OFFSET30	0xB81  /* (LTQ_GSWIP_3_0) */
#define SDMA_PFCTHR9_THR9_SHIFT		0
#define SDMA_PFCTHR9_THR9_SIZE		11
/* --------------------------------------------------- */
/******** Ethernet PMAC Registers ******************** */
/* --------------------------------------------------- */
/* Register: 'PMAC Interrupt Status Register' (LTQ_GSWIP_3_0)*/
/* Bit: 'EGCHKERR' */
/* Description: 'Egress IP/UDP/TCP Checksum Error Detected' */
#define PMAC_ISR_EGCHKERR_OFFSET	0xD01
#define PMAC_ISR_EGCHKERR_SHIFT		7
#define PMAC_ISR_EGCHKERR_SIZE		1
/* Bit: 'IGCHKERR' */
/* Description: 'Ingress IP/UDP/TCP Checksum Error Detected' */
#define PMAC_ISR_IGCHKERR_OFFSET	0xD01
#define PMAC_ISR_IGCHKERR_SHIFT		6
#define PMAC_ISR_IGCHKERR_SIZE		1
/* Bit: 'TOOSHORT' */
/* Description: 'Too Short Frame Error Detected' */
#define PMAC_ISR_TOOSHORT_OFFSET	0xD01
#define PMAC_ISR_TOOSHORT_SHIFT		5
#define PMAC_ISR_TOOSHORT_SIZE		1
/* Bit: 'TOOLONG' */
/* Description: 'Too Long Frame Error Detected' */
#define PMAC_ISR_TOOLONG_OFFSET	0xD01
#define PMAC_ISR_TOOLONG_SHIFT	4
#define PMAC_ISR_TOOLONG_SIZE		1
/* Bit: 'LENERR' */
/* Description: 'Length Mismatch Error Detected' */
#define PMAC_ISR_LENERR_OFFSET	0xD01
#define PMAC_ISR_LENERR_SHIFT		3
#define PMAC_ISR_LENERR_SIZE		1
/* Bit: 'FCSERR' */
/* Description: 'Frame Checksum Error Detected' */
#define PMAC_ISR_FCSERR_OFFSET	0xD01
#define PMAC_ISR_FCSERR_SHIFT		2
#define PMAC_ISR_FCSERR_SIZE		1
/* --------------------------------------------------- */
/* Register: 'PMAC Interrupt Enable Register' (LTQ_GSWIP_3_0)*/
/* Bit: 'EGCHKERR' */
/* Description: 'Egress Checksum Error Interrupt Mask' */
#define PMAC_IER_EGCHKERR_OFFSET	0xD02
#define PMAC_IER_EGCHKERR_SHIFT		7
#define PMAC_IER_EGCHKERR_SIZE		1
/* Bit: 'IGCHKERR' */
/* Description: 'Ingress Checksum Error Interrupt Mask' */
#define PMAC_IER_IGCHKERR_OFFSET	0xD02
#define PMAC_IER_IGCHKERR_SHIFT		6
#define PMAC_IER_IGCHKERR_SIZE		1
/* Bit: 'TOOSHORT' */
/* Description: 'Too Short Frame Error Interrupt Mask' */
#define PMAC_IER_TOOSHORT_OFFSET	0xD02
#define PMAC_IER_TOOSHORT_SHIFT		5
#define PMAC_IER_TOOSHORT_SIZE		1
/* Bit: 'TOOLONG' */
/* Description: 'Too Long Frame Error Interrupt Mask' */
#define PMAC_IER_TOOLONG_OFFSET	0xD02
#define PMAC_IER_TOOLONG_SHIFT	4
#define PMAC_IER_TOOLONG_SIZE		1
/* Bit: 'LENERR' */
/* Description: 'Length Mismatch Error Interrupt Mask' */
#define PMAC_IER_LENERR_OFFSET	0xD02
#define PMAC_IER_LENERR_SHIFT		3
#define PMAC_IER_LENERR_SIZE		1
/* Bit: 'FCSERR' */
/* Description: 'Frame Checksum Error Interrupt Mask' */
#define PMAC_IER_FCSERR_OFFSET	0xD02
#define PMAC_IER_FCSERR_SHIFT		2
#define PMAC_IER_FCSERR_SIZE		1
/* --------------------------------------------------- */
/* Register: 'PMAC Control Register 0' (LTQ_GSWIP_3_0)*/
/* Bit: 'APADEN' */
/* Description: 'Automatic VLAN Padding Enable' */
#define PMAC_CTRL_0_APADEN_OFFSET	0xD03
#define PMAC_CTRL_0_APADEN_SHIFT	11
#define PMAC_CTRL_0_APADEN_SIZE		1
/* Bit: 'VPAD2EN' */
/* Description: 'Stacked VLAN Padding Enable' */
#define PMAC_CTRL_0_VPAD2EN_OFFSET	0xD03
#define PMAC_CTRL_0_VPAD2EN_SHIFT		10
#define PMAC_CTRL_0_VPAD2EN_SIZE		1
/* Bit: 'VPADEN' */
/* Description: 'VLAN Padding Enable' */
#define PMAC_CTRL_0_VPADEN_OFFSET	0xD03
#define PMAC_CTRL_0_VPADEN_SHIFT	9
#define PMAC_CTRL_0_VPADEN_SIZE		1
/* Bit: 'PADEN' */
/* Description: 'Padding Enable' */
#define PMAC_CTRL_0_PADEN_OFFSET	0xD03
#define PMAC_CTRL_0_PADEN_SHIFT		8
#define PMAC_CTRL_0_PADEN_SIZE		1
/* Bit: 'FCS' */
/* Description: 'Transmit FCS Control' */
#define PMAC_CTRL_0_FCS_OFFSET	0xD03
#define PMAC_CTRL_0_FCS_SHIFT		7
#define PMAC_CTRL_0_FCS_SIZE		1
/* Bit: 'CHKREG' */
/* Description: 'IP/TCP/UDP Checksum Generation Control' */
#define PMAC_CTRL_0_CHKREG_OFFSET	0xD03
#define PMAC_CTRL_0_CHKREG_SHIFT	6
#define PMAC_CTRL_0_CHKREG_SIZE		1
/* Bit: 'CHKVER' */
/* Description: 'IP/TCP/UDP Checksum Verification Control' */
#define PMAC_CTRL_0_CHKVER_OFFSET	0xD03
#define PMAC_CTRL_0_CHKVER_SHIFT	5
#define PMAC_CTRL_0_CHKVER_SIZE		1
/* --------------------------------------------------- */
/* Register: 'PMAC Control Register 2' (LTQ_GSWIP_3_0)*/
/* Bit: 'MLEN' */
/* Description: 'Maximum Untagged Frame Length' */
#define PMAC_CTRL_2_MLEN_OFFSET	0xD05
#define PMAC_CTRL_2_MLEN_SHIFT	3
#define PMAC_CTRL_2_MLEN_SIZE		1
/* Bit: 'LCHKL' */
/* Description: 'Frame Length Check Long Enable' */
#define PMAC_CTRL_2_LCHKL_OFFSET	0xD05
#define PMAC_CTRL_2_LCHKL_SHIFT		2
#define PMAC_CTRL_2_LCHKL_SIZE		1
/* Bit: 'LCHKS' */
/* Description: 'Frame Length Check Short Enable' */
#define PMAC_CTRL_2_LCHKS_OFFSET	0xD05
#define PMAC_CTRL_2_LCHKS_SHIFT		0
#define PMAC_CTRL_2_LCHKS_SIZE		2
/* --------------------------------------------------- */
/* Register: 'PMAC Control Register 3' (LTQ_GSWIP_3_0)*/
/* Bit: 'JUMBO' */
/* Description: 'Frame Length Threshold for Jumbo Packet' */
#define PMAC_CTRL_3_JUMBO_OFFSET	0xD06
#define PMAC_CTRL_3_JUMBO_SHIFT		0
#define PMAC_CTRL_3_JUMBO_SIZE		14
/* --------------------------------------------------- */
/* Register: 'PMAC Control Register 4' (LTQ_GSWIP_3_0)*/
/* Bit: 'FLAGEN' */
/* Description: 'Select Processing Flags or Traffic Class For */
/*	Addressing Egress Configuration Table' */
#define PMAC_CTRL_4_FLAGEN_OFFSET	0xD07
#define PMAC_CTRL_4_FLAGEN_SHIFT	0
#define PMAC_CTRL_4_FLAGEN_SIZE		1
/* --------------------------------------------------- */
/* Register: 'Table Value Register 4' (LTQ_GSWIP_3_0)*/
/* Bit: 'VAL4' */
/* Description: 'Data value [15:0]' */
#define PMAC_TBL_VAL_4_VAL4_OFFSET	0xD40
#define PMAC_TBL_VAL_4_VAL4_SHIFT		0
#define PMAC_TBL_VAL_4_VAL4_SIZE		16
/* --------------------------------------------------- */
/* Register: 'Table Value Register 3' (LTQ_GSWIP_3_0)*/
/* Bit: 'VAL3' */
/* Description: 'Data value [15:0]' */
#define PMAC_TBL_VAL_3_VAL3_OFFSET	0xD41
#define PMAC_TBL_VAL_3_VAL3_SHIFT		0
#define PMAC_TBL_VAL_3_VAL3_SIZE		16
/* --------------------------------------------------- */
/* Register: 'Table Value Register 2' (LTQ_GSWIP_3_0)*/
/* Bit: 'VAL2' */
/* Description: 'Data value [15:0]' */
#define PMAC_TBL_VAL_2_VAL2_OFFSET	0xD42
#define PMAC_TBL_VAL_2_VAL2_SHIFT		0
#define PMAC_TBL_VAL_2_VAL2_SIZE		16
/* --------------------------------------------------- */
/* Register: 'Table Value Register 1' (LTQ_GSWIP_3_0)*/
/* Bit: 'VAL1' */
/* Description: 'Data value [15:0]' */
#define PMAC_TBL_VAL_1_VAL1_OFFSET	0xD43
#define PMAC_TBL_VAL_1_VAL1_SHIFT		0
#define PMAC_TBL_VAL_1_VAL1_SIZE		16
/* --------------------------------------------------- */
/* Register: 'Table Value Register 0' (LTQ_GSWIP_3_0)*/
/* Bit: 'VAL0' */
/* Description: 'Data value [15:0]' */
#define PMAC_TBL_VAL_0_VAL0_OFFSET	0xD44
#define PMAC_TBL_VAL_0_VAL0_SHIFT		0
#define PMAC_TBL_VAL_0_VAL0_SIZE		16
/* --------------------------------------------------- */
/* Register: 'Table Entry Address Register' (LTQ_GSWIP_3_0)*/
/* Bit: 'ADDR' */
/* Description: 'Table Address' */
#define PMAC_TBL_ADDR_ADDR_OFFSET	0xD45
#define PMAC_TBL_ADDR_ADDR_SHIFT	0
#define PMAC_TBL_ADDR_ADDR_SIZE		12
/* --------------------------------------------------- */
/* Register: 'Table Access Control Register' (LTQ_GSWIP_3_0)*/
/* Bit: 'BAS' */
/* Description: 'Access Busy/Access Start' */
#define PMAC_TBL_CTRL_BAS_OFFSET	0xD46
#define PMAC_TBL_CTRL_BAS_SHIFT		15
#define PMAC_TBL_CTRL_BAS_SIZE		1
/* Bit: 'OPMOD' */
/* Description: 'Lookup Table Access Operation Mode' */
#define PMAC_TBL_CTRL_OPMOD_OFFSET	0xD46
#define PMAC_TBL_CTRL_OPMOD_SHIFT		5
#define PMAC_TBL_CTRL_OPMOD_SIZE		1
/* Bit: 'ADDR' */
/* Description: 'Lookup Table Address' */
#define PMAC_TBL_CTRL_ADDR_OFFSET	0xD46
#define PMAC_TBL_CTRL_ADDR_SHIFT	0
#define PMAC_TBL_CTRL_ADDR_SIZE		3
/* --------------------------------------------------- */
#endif /* _GSWITCH_REG_H_ */

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

#ifndef _GSW_ETHSW_FLOW_H_
#define _GSW_ETHSW_FLOW_H_

/* =================================== */
/* Global typedef forward declarations */
/* =================================== */

/* ============================= */
/* Local Macros & Definitions    */
/* ============================= */
#include "f2x_swapi_types.h"

/* IOCTL MAGIC */
#define GSW_FLOW_MAGIC ('F')

/* Group definitions for Doxygen */
/** \defgroup GSW_FLOW_GROUP GSWIP Packet Classification Engine (PCE) related APIs
    \brief This chapter describes the GSWIP's PCE and other low level related APIs.
*/
/*@{*/

/** \defgroup GSW_IOCTL_DEBUG Debug Features
   \brief Switch Registers write and read for any low level operations including debug.
*/
/** \defgroup GSW_IOCTL_IRQ Interrupt Handling
    \brief Configure GSWIP support to configure and read out the interrupt mask and sources.
*/
/** \defgroup GSW_IOCTL_SYS GSWIP System Functions
    \brief This chapter summarizes the functions that are provided to handle system aspects of GSWIP.
*/
/** \defgroup GSW_IOCTL_CLASS Packet Classification Engine
    \brief Configures and controls the classification unit of the GSWIP.
*/

/*@}*/

/* -------------------------------------------------------------------------- */
/*                 Structure and Enumeration Type Defintions                  */
/* -------------------------------------------------------------------------- */

/** \addtogroup GSW_IOCTL_DEBUG */
/*@{*/

/** \brief Register access parameter to directly read or write switch
    internal registers.
    Used by \ref GSW_REGISTER_SET and \ref GSW_REGISTER_GET. */
typedef struct
{
	/** Register Address Offset for read or write access. */
	u16	nRegAddr;
	/** Value to write to or read from 'nRegAddr'. */
	u16	nData;
} GSW_register_t;

/*@}*/ /* GSW_IOCTL_DEBUG */

/** \addtogroup GSW_IOCTL_IRQ */
/*@{*/

/** \brief Interrupt Source Selector.
    Used by \ref GSW_irq_t. */
typedef enum
{
	/** Wake-on-LAN Interrupt. The configured packet flow will trigger WoL interrupt.
	    The parameter 'nPortId' specifies the relative MAC port. */
	GSW_IRQ_WOL	= 0,
	/** Port Limit Alert Interrupt. This interrupt is asserted when the number
	    of learned MAC addresses exceeds the configured limit for
	    the ingress port.
	    The parameter 'nPortId' specifies the relative MAC port. */
	GSW_IRQ_LIMIT_ALERT	= 1,
	/** Port Lock Alert Interrupt.
	    This interrupt is asserted when a source MAC address is learned on a
	    locked port and is received on another port.
	    The parameter 'nPortId' specifies the relative MAC port. */
	GSW_IRQ_LOCK_ALERT	= 2
} GSW_irqSrc_t;

/** \brief Interrupt bits. Depending on the hardware device type, not all interrupts might be available.
    Used by \ref GSW_IRQ_MASK_GET, \ref GSW_IRQ_MASK_SET,
    \ref GSW_IRQ_GET and \ref GSW_IRQ_STATUS_CLEAR. */
typedef struct
{
	/** Ethernet Port number (zero-based counting). The valid range is hardware dependent.
	    An error code is delivered if the selected port is not
	    available. This port parameter is needed for some interrupts
	    that are specified by 'nIrqSrc'. For all other interrupts, this
	    parameter is "don't care". */
	u32	nPortId;
	/** Interrupt Source Type. */
	GSW_irqSrc_t	eIrqSrc;
} GSW_irq_t;

/*@}*/ /* GSW_IOCTL_IRQ */

/** \addtogroup GSW_IOCTL_CLASS */
/*@{*/

/** \brief Rule selection for IPv4/IPv6.
    Used by \ref GSW_PCE_pattern_t. */
typedef enum
{
	/** Rule Pattern for IP selection disabled. */
	GSW_PCE_IP_DISABLED	= 0,
	/** Rule Pattern for IPv4. */
	GSW_PCE_IP_V4	= 1,
	/** Rule Pattern for IPv6. */
	GSW_PCE_IP_V6	= 2
} GSW_PCE_IP_t;

/** \brief Packet Classification Engine Pattern Configuration.
    Used by \ref GSW_PCE_rule_t. */
typedef struct
{
	/** PCE Rule Index (Upto 64 rules supported in GSWIP-2.x) */
	int	nIndex;

	/** Index is used (enabled) or set to unused (disabled) */
	ltq_bool_t	bEnable;

	/** Port ID used  for ingress packet classification */
	ltq_bool_t	bPortIdEnable;
	/** Port ID value of incoming packets used for classification */
	u8	nPortId;

	/** DSCP value used  */
	ltq_bool_t	bDSCP_Enable;
	/** DSCP value  */
	u8		nDSCP;

	/** CTAG VLAN PCP value used */
	ltq_bool_t	bPCP_Enable;
	/** CTAG VLAN PCP value */
	u8		nPCP;

	/** STAG VLAN PCP/DEI value used */
	ltq_bool_t	bSTAG_PCP_DEI_Enable;
	/** STAG VLAN PCP value */
	u8		nSTAG_PCP_DEI;

	/** Packet length used for classification */
	ltq_bool_t	bPktLngEnable;
	/** Packet length in bytes */
	u16		nPktLng;
	/** Packet length Range (from nPktLng to nPktLngRange) */
	u16		nPktLngRange;

	/** Destination MAC address used */
	ltq_bool_t	bMAC_DstEnable;
	/** Destination MAC address */
	u8		nMAC_Dst[6];
	/** Destination MAC address nibble mask.
	    Please clear the bits of the nibbles that are not marked out and set all other bits.
	    The LSB bit represents the lowest data nibble, the next bit the next nibble,
	    and so on. */
	u16	nMAC_DstMask;

	/** Source MAC address used */
	ltq_bool_t	bMAC_SrcEnable;
	/** Source MAC address */
	u8	nMAC_Src[6];
	/** Source MAC address nibble mask.
	    Please clear the bits of the nibbles that are not marked out and set all other bits.
	    The LSB bit represents the lowest data nibble, the next bit the next nibble,
	    and so on. */
	u16	nMAC_SrcMask;

	/** MSB Application field used */
	ltq_bool_t	bAppDataMSB_Enable;
	/** MSB Application field.
	    The first 2 bytes of the packet content following the IP header
	    for TCP/UDP packets (source port field), or the first 2 bytes of packet content
	    following the Ethertype for non-IP packets. Any part of this
	    content can be masked-out by a programmable bit
	    mask 'nAppMaskRangeMSB'. */
	u16	nAppDataMSB;
	/** MSB Application mask/range selection.
	    If set to LTQ_TRUE, the field 'nAppMaskRangeMSB' is used as a
	    range parameter, otherwise it is used as a nibble mask field. */
	ltq_bool_t	bAppMaskRangeMSB_Select;
	/** MSB Application mask/range. When used as a range parameter,
	    1 bit represents 1 nibble mask of the 'nAppDataMSB' field.
	    Please clear the bits of the nibbles that are not marked out and set all other bits.
	    The LSB bit represents the lowest data nibble, the next bit the next nibble,
	    and so on. */
	u16	nAppMaskRangeMSB;

	/** LSB Application used */
	ltq_bool_t	bAppDataLSB_Enable;
	/** LSB Application field.
	    The following 2 bytes of the packet behind the 'nAppDataMSB' field.
	    This is the destination port field for TCP/UDP packets,
	    or byte 3 and byte 4 of the packet content following the Ethertype
	    for non-IP packets. Any part of this content can be masked-out
	    by a programmable bit mask 'nAppMaskRangeLSB'. */
	u16	nAppDataLSB;
	/** LSB Application mask/range selection.
	    If set to LTQ_TRUE, the field 'nAppMaskRangeLSB' is used as
	    a range parameter, otherwise it is used as a nibble mask field. */
	ltq_bool_t	bAppMaskRangeLSB_Select;
	/** LSB Application mask/range. When used as a range parameter,
	    1 bit represents 1 nibble mask of the 'nAppDataLSB' field.
	    Please clear the bits of the nibbles that are not marked out and set all other bits.
	    The LSB bit represents the lowest data nibble, the next bit the next nibble,
	    and so on. */
	u16	nAppMaskRangeLSB;

	/** Destination IP Selection . */
	GSW_PCE_IP_t	eDstIP_Select;
	/** Destination IP  */
	GSW_IP_t	nDstIP;
	/** Destination IP Nibble Mask.
	    1 bit represents 1 nibble mask of the 'nDstIP' field.
	    Please clear the bits of the nibbles that are not marked out and set all other bits.
	    The LSB bit represents the lowest data nibble, the next bit the next nibble,
	    and so on. */
	u32	nDstIP_Mask;

	/** Source IP Selection . */
	GSW_PCE_IP_t	eSrcIP_Select;
	/** Source IP   */
	GSW_IP_t	nSrcIP;
	/** Source IP Nibble Mask .
	    1 bit represents 1 nibble mask of the 'nSrcIP' field.
	    Please clear the bits of the nibbles that are not marked out and set all other bits.
	    The LSB bit represents the lowest data nibble, the next bit the next nibble,
	    and so on. */
	u32	nSrcIP_Mask;

	/** Ethertype used. */
	ltq_bool_t	bEtherTypeEnable;
	/** Ethertype */
	u16	nEtherType;
	/** Ethertype Mask.
	    1 bit represents 1 nibble mask of the 'nEtherType' field.
	    Please clear the bits of the nibbles that are not marked out and set all other bits.
	    The LSB bit represents the lowest data nibble, the next bit the next nibble,
	    and so on. */
	u16	nEtherTypeMask;

	/** IP protocol used */
	ltq_bool_t	bProtocolEnable;
	/** IP protocol Value */
	u8	nProtocol;
	/** IP protocol Mask.
	    1 bit represents 1 nibble mask of the 'nProtocol' field.
	    Please clear the bits of the nibbles that are not marked out and set all other bits i.e. a set bit 1 indicates that bit is masked out (not compared).
	    The LSB bit represents the lowest data nibble, the next bit the next nibble,
	    and so on. */
	u8	nProtocolMask;

	/** PPPoE used. */
	ltq_bool_t	bSessionIdEnable;
	/** PPPoE Session Id */
	u16	nSessionId;
	/** VLAN ID (CVID) used. */
	ltq_bool_t	bVid;
	/** VLAN ID (CVID) */
	u16	nVid;
	/** VID mask/range selection.
	    If set to 1, the field 'nVidRange' is used as
	    a range parameter, otherwise it is used as a mask field. */
	ltq_bool_t	bVidRange_Select;
	/** VLAN ID Range (CVID). Gets used as mask to nVid in case bVidRange_Select is set to 0 */
	u16	nVidRange;
	/** STAG VLAN ID used. */
	ltq_bool_t	bSLAN_Vid;
	/** STAG VLAN ID */
	u16	nSLAN_Vid;
} GSW_PCE_pattern_t;

/** \brief IGMP Snooping Control.
    Used by \ref GSW_PCE_action_t. */
typedef enum
{
	/** Disabled. IGMP Snooping is disabled. */
	GSW_PCE_ACTION_IGMP_SNOOP_DISABLE	= 0,
	/** Default. Regular Packet. No IGMP Snooping action required. */
	GSW_PCE_ACTION_IGMP_SNOOP_REGULAR	= 1,
	/** IGMP Report/Join Message. */
	GSW_PCE_ACTION_IGMP_SNOOP_REPORT	= 2,
	/** IGMP Leave Message. */
	GSW_PCE_ACTION_IGMP_SNOOP_LEAVE	= 3,
	/**  Router Solicitation/Advertisement message. */
	GSW_PCE_ACTION_IGMP_SNOOP_AD	= 4,
	/** IGMP Query Message. */
	GSW_PCE_ACTION_IGMP_SNOOP_QUERY	= 5,
	/** IGMP Group Specific Query Message. */
	GSW_PCE_ACTION_IGMP_SNOOP_QUERY_GROUP	= 6,
	/** IGMP General Query message without Router Solicitation. */
	GSW_PCE_ACTION_IGMP_SNOOP_QUERY_NO_ROUTER = 7
} GSW_PCE_ActionIGMP_Snoop_t;

/** \brief MAC Address Learning control.
    Used by \ref GSW_PCE_action_t. */
typedef enum
{
	/** MAC Address Learning action is disabled. MAC address learning is
	    not influenced by this rule. */
	GSW_PCE_ACTION_LEARNING_DISABLE	= 0,
	/** Learning is based on the forwarding decision. If the packet is discarded,
	    the address is not learned. If the packet is forwarded to any egress port,
	    the address is learned. */
	GSW_PCE_ACTION_LEARNING_REGULAR	= 1,
	/** Force No Learning. The address is not learned; forwarding decision
	    ignored. */
	GSW_PCE_ACTION_LEARNING_FORCE_NOT	= 2,
	/** Force Learning. The address is learned, the forwarding decision ignored.
	    Note: The MAC Learning Control signals delivered to Port-Map filtering
	    and combined with Final Forwarding Decision. The result is used as a
	    feedback for MAC Address learning in the Bridging Table. */
	GSW_PCE_ACTION_LEARNING_FORCE	= 3
} GSW_PCE_ActionLearning_t;

/** \brief Flow Meter Assignment control.
    Used by \ref GSW_PCE_action_t. */
typedef enum
{
	/** Action Disable. */
	GSW_PCE_ACTION_METER_DISABLE	= 0,
	/** Action Enable.
	    The action is enabled but no dedicated metering instance is assigned by the rule. */
	GSW_PCE_ACTION_METER_REGULAR	= 1,
	/** Action Enable. Assign one meter instance as given in parameter "nMeterId". */
	GSW_PCE_ACTION_METER_1	= 2,
	/** Action Enable. Assign pair of meter instances.
	    These instances are "nMeterId" and the next following meter instance index. */
	GSW_PCE_ACTION_METER_1_2	= 3
} GSW_PCE_ActionMeter_t;

/** \brief Traffic Class Action Selector.
    Used by \ref GSW_PCE_action_t. */
typedef enum
{
	/** Disabled. Traffic class action is disabled. */
	GSW_PCE_ACTION_TRAFFIC_CLASS_DISABLE	= 0,
	/** Regular Class. Traffic class action is enabled and the CoS
	    classification traffic class is used. */
	GSW_PCE_ACTION_TRAFFIC_CLASS_REGULAR	= 1,
	/** Alternative Class. Traffic class action is enabled and the
	    class of the 'nTrafficClassAlter' field is used. */
	GSW_PCE_ACTION_TRAFFIC_CLASS_ALTERNATIVE	= 2,
} GSW_PCE_ActionTrafficClass_t;

/** \brief Interrupt Control Action Selector.
    Used by \ref GSW_PCE_action_t. */
typedef enum
{
	/** Disabled. Interrupt Control Action is disabled for this rule. */
	GSW_PCE_ACTION_IRQ_DISABLE	= 0,
	/** Regular Packet. The Interrupt Control Action is enabled, the packet is
	    treated as a regular packet and no interrupt event is generated. */
	GSW_PCE_ACTION_IRQ_REGULAR	= 1,
	/** Interrupt Event. The Interrupt Control Action is enabled and an
	    interrupt event is generated. */
	GSW_PCE_ACTION_IRQ_EVENT	= 2
} GSW_PCE_ActionIrq_t;

/** \brief Cross State Action Selector.
    Used by \ref GSW_PCE_action_t. */
typedef enum
{
	/** Disable. The Cross State Action is disabled. */
	GSW_PCE_ACTION_CROSS_STATE_DISABLE	= 0,
	/** Regular Packet. The Cross State Action is enabled and the packet is
	    treated as a non-Cross-State packet (regular packet). Therefore it does
	    not ignore Port-State filtering rules. */
	GSW_PCE_ACTION_CROSS_STATE_REGULAR	= 1,
	/** Cross-State packet. The Cross State Action is enabled and the packet is
	    treated as a Cross-State packet. It ignores the Port-State
	    filtering rules. */
	GSW_PCE_ACTION_CROSS_STATE_CROSS	= 2
} GSW_PCE_ActionCrossState_t;

/** \brief Critical Frame Action Selector.
    Used by \ref GSW_PCE_action_t. */
typedef enum
{
	/** Disable. The Critical Frame Action is disabled. */
	GSW_PCE_ACTION_CRITICAL_FRAME_DISABLE	= 0,
	/** Regular Packet. The Critical Frame Action is enabled and the packet is
	    treated as a non-Critical Frame. */
	GSW_PCE_ACTION_CRITICAL_FRAME_REGULAR	= 1,
	/** Critical Packet. The Critical Frame Action is enabled and the packet is
	    treated as a Critical Frame. */
	GSW_PCE_ACTION_CRITICAL_FRAME_CRITICAL	= 2
} GSW_PCE_ActionCriticalFrame_t;

/** \brief Timestamp Action Selector.
    Used by \ref GSW_PCE_action_t. */
typedef enum
{
	/** Disable. Timestamp Action is disabled for this rule. */
	GSW_PCE_ACTION_TIMESTAMP_DISABLE	= 0,
	/** Regular Packet. The Timestamp Action is enabled for this rule.
	    The packet is treated as a regular packet and no timing information
	    is stored. */
	GSW_PCE_ACTION_TIMESTAMP_REGULAR	= 1,
	/** Receive/Transmit Timing packet. Ingress and Egress Timestamps for
	    this packet should be stored. */
	GSW_PCE_ACTION_TIMESTAMP_STORED	= 2
} GSW_PCE_ActionTimestamp_t;

/** \brief Forwarding Group Action Selector.
    This flow table action and the 'bFlowID_Action' action
    can be used exclusively.
    Used by \ref GSW_PCE_action_t. */
typedef enum
{
	/** Disable. Forwarding Group Action is disabled. */
	GSW_PCE_ACTION_PORTMAP_DISABLE	= 0,
	/** Regular Packet. Forwarding Action enabled. Select Default
	    Port-Map (result of Default Forwarding Classification). */
	GSW_PCE_ACTION_PORTMAP_REGULAR	= 1,
	/** Discard. Discard the packets. */
	GSW_PCE_ACTION_PORTMAP_DISCARD	= 2,
	/** Forward to the CPU port. This requires that the CPU port is previously
	    set by calling \ref GSW_CPU_PORT_CFG_SET. */
	GSW_PCE_ACTION_PORTMAP_CPU	= 3,
	/** Forward to a portmap, selected by the parameter 'nForwardPortMap'.
	    Please note that this feature is not supported by all
	    hardware platforms. */
	GSW_PCE_ACTION_PORTMAP_ALTERNATIVE	= 4,
	/** The packet is treated as Multicast Router
	    Solicitation/Advertisement or Query packet. */
	GSW_PCE_ACTION_PORTMAP_MULTICAST_ROUTER	= 5,
	/** The packet is interpreted as Multicast packet and learned in the
	    multicast group table. */
	GSW_PCE_ACTION_PORTMAP_MULTICAST_HW_TABLE = 6,
	/** The CTAG VLAN portmap classification result is replaced by the
	    portmap parameter 'nForwardPortMap'. All other classification
	    results stay unchanged and will be combined together with
	    the overwritten portmap. */
	GSW_PCE_ACTION_PORTMAP_ALTERNATIVE_VLAN	= 7,
	/** Add STAG VLAN portmap 'nForwardPortMap' to the overall portmap
	    classification result (AND'ed with the portmap). */
	GSW_PCE_ACTION_PORTMAP_ALTERNATIVE_STAG_VLAN	= 8
} GSW_PCE_ActionPortmap_t;

/** \brief VLAN Group Action Selector.
    Used by \ref GSW_PCE_action_t. */
typedef enum
{
	/** Disabled. The VLAN Action is disabled. */
	GSW_PCE_ACTION_VLAN_DISABLE	= 0,
	/** Regular VLAN. VLAN Action enabled. Select Default VLAN ID. */
	GSW_PCE_ACTION_VLAN_REGULAR	= 1,
	/** Alternative VLAN. VLAN Action enabled.
	    Select Alternative VLAN as configured in 'nVLAN_Id'
	    or 'nSVLAN_Id'. For CTAG VLAN it requires that this VLAN ID
	    is configured by calling
	    \ref GSW_VLAN_ID_CREATE in advance.
	    This additional call is not required for STAG VLAN. */
	GSW_PCE_ACTION_VLAN_ALTERNATIVE	= 2
} GSW_PCE_ActionVLAN_t;

/** \brief Cross VLAN Action Selector.
    Used by \ref GSW_PCE_action_t. */
typedef enum
{
	/** Disabled. The Cross VLAN Action is disabled. */
	GSW_PCE_ACTION_CROSS_VLAN_DISABLE	= 0,
	/** Regular VLAN Packet. Do not ignore VLAN filtering rules. */
	GSW_PCE_ACTION_CROSS_VLAN_REGULAR	= 1,
	/** Cross-VLAN packet. Ignore VLAN filtering  rules.*/
	GSW_PCE_ACTION_CROSS_VLAN_CROSS	= 2
} GSW_PCE_ActionCrossVLAN_t;

/** \brief Packet Classification Engine Action Configuration.
    Used by \ref GSW_PCE_rule_t. */
typedef struct
{
	/** Action "Traffic Class" Group.
	    Traffic class action enable */
	GSW_PCE_ActionTrafficClass_t	eTrafficClassAction;
	/** Alternative Traffic class - used when eTrafficClassAction is set to 2. */
	u8	nTrafficClassAlternate;

	/** Action "IGMP Snooping" Group.
	    IGMP Snooping control and enable. Please note that the 'nPortMapAction'
	    configuration is ignored in case the IGMP snooping is enabled.
	    Here, on read operations,
	    'nPortMapAction = GSW_PCE_ACTION_PORTMAP_DISABLE' is returned. */
	GSW_PCE_ActionIGMP_Snoop_t	eSnoopingTypeAction;

	/** Action "Learning" Group.
	    Learning action control and enable */
	GSW_PCE_ActionLearning_t	eLearningAction;

	/** Action "Interrupt" Group.
	    Interrupt action generate and enable */
	GSW_PCE_ActionIrq_t	eIrqAction;

	/** Action "Cross State" Group.
	    Cross state action control and enable */
	GSW_PCE_ActionCrossState_t	eCrossStateAction;

	/** Action "Critical Frames" Group.
	    Critical Frame action control and enable */
	GSW_PCE_ActionCriticalFrame_t	eCritFrameAction;

	/** Action "Timestamp" Group. Time stamp action control and enable */
	GSW_PCE_ActionTimestamp_t	eTimestampAction;

	/** Action "Forwarding" Group.
	    Port map action enable. This port forwarding configuration is ignored
	    in case the action "IGMP Snooping" is enabled via the
	    parameter 'nSnoopingTypeAction'. */
	GSW_PCE_ActionPortmap_t	ePortMapAction;
	/** Target portmap for forwarded packets, only used if selected by
	    'ePortMapAction'. Forwarding is done
	    if 'ePortMapAction = GSW_PCE_ACTION_PORTMAP_ALTERNATIVE'.
	    Every bit in the portmap represents one port (port 0 = LSB bit). */
	u32	nForwardPortMap;

	/** Action "Remarking" Group. Remarking action enable */
	ltq_bool_t	bRemarkAction;
	/** CTAG VLAN PCP remarking enable.
	    Remarking enabling means that remarking is possible in case
	    the port configuration or metering enables remarking on that
	    packet. Disabling remarking means that it is forced to
	    not remarking this packet, independent of any port remarking of
	    metering configuration. */
	ltq_bool_t	bRemarkPCP;
	/** STAG VLAN PCP remarking enable.
	    Remarking enabling means that remarking is possible in case
	    the port configuration or metering enables remarking on that
	    packet. Disabling remarking means that it is forced to
	    not remarking this packet, independent of any port remarking of
	    metering configuration. */
	ltq_bool_t	bRemarkSTAG_PCP;
	/** STAG VLAN DEI remarking enable.
	    Remarking enabling means that remarking is possible in case
	    the port configuration or metering enables remarking on that
	    packet. Disabling remarking means that it is forced to
	    not remarking this packet, independent of any port remarking of
	    metering configuration. */
	ltq_bool_t	bRemarkSTAG_DEI;
	/** DSCP remarking enable
	    Remarking enabling means that remarking is possible in case
	    the port configuration or metering enables remarking on that
	    packet. Disabling remarking means that it is forced to
	    not remarking this packet, independent of any port remarking of
	    metering configuration. */
	ltq_bool_t	bRemarkDSCP;
	/** Class remarking enable
	    Remarking enabling means that remarking is possible in case
	    the port configuration or metering enables remarking on that
	    packet. Disabling remarking means that it is forced to
	    not remarking this packet, independent of any port remarking of
	    metering configuration. */
	ltq_bool_t	bRemarkClass;

	/** Action "Meter" Group. Meter action control and enable.
	    If metering action enabled, specified metering instance number
	    overrules any other metering assignment.
	    Up to two metering instances can be applied to a single packet. */
	GSW_PCE_ActionMeter_t	eMeterAction;
	/** Meter ID */
	u8	nMeterId;

	/** Action "RMON" Group. RMON action enable */
	ltq_bool_t	bRMON_Action;
	/** Counter ID (The index starts counting from zero). */
	u8	nRMON_Id;

	/** Action "CTAG VLAN" Group. VLAN action enable */
	GSW_PCE_ActionVLAN_t	eVLAN_Action;
	/** Alternative CTAG VLAN Id */
	u16	nVLAN_Id;
	/** Alternative FID */
	u8	nFId;
	/** Action "STAG VLAN" Group. VLAN action enable */
	GSW_PCE_ActionVLAN_t	eSVLAN_Action;
	/** Alternative STAG VLAN Id */
	u16	nSVLAN_Id;
	/** Action "Cross VLAN" Group. Cross VLAN action enable */
	GSW_PCE_ActionCrossVLAN_t	eVLAN_CrossAction;
	/**  CVLAN Ignore control */
	ltq_bool_t	bCVLAN_Ignore_Control;
	/** Port BitMap Mux control */
	ltq_bool_t	bPortBitMapMuxControl;
	/** Trunking action enable  */
	ltq_bool_t	bPortTrunkAction;
	/**  Port Link Selection control */
	ltq_bool_t	bPortLinkSelection;
	/** Action "Flow ID".
	 The Switch supports enhancing the egress packets by a device specific
	 special tag header. This header contains detailed switch classification
	 results. One header file is a 'Flow ID', which can be explicitly set as
	 flow table action when hitting a table rule.
	 If selected, the Flow ID is given by the parameter 'nFlowID'. */
	ltq_bool_t	bFlowID_Action;
	/** Flow ID */
	u16	nFlowID;
} GSW_PCE_action_t;

/** \brief Parameter to add/read a rule to/from the packet classification engine.
    Used by \ref GSW_PCE_RULE_WRITE and \ref GSW_PCE_RULE_READ. */
typedef struct
{
	/** PCE Rule Pattern Part. */
	GSW_PCE_pattern_t	pattern;
	/** PCE Rule Action Part. */
	GSW_PCE_action_t	action;
} GSW_PCE_rule_t;

/** \brief Parameter to delete a rule from the packet classification engine.
    Used by \ref GSW_PCE_RULE_DELETE. */
typedef struct
{
	/** Rule Index in the PCE Table. */
	u32	nIndex;
} GSW_PCE_ruleDelete_t;

/*@}*/ /* GSW_IOCTL_CLASS */

/** \addtogroup GSW_IOCTL_SYS */
/*@{*/

/** \brief Reset selection.
    Used by \ref GSW_reset_t. */
typedef enum
{
	/** Switch Macro reset */
	GSW_RESET_SWITCH	= 0,
	/** MDIO master interface reset */
	GSW_RESET_MDIO	= 1,
} GSW_resetMode_t;

/** \brief Reset selection.
    Used by \ref GSW_RESET. */
typedef struct
{
	/** Reset selection. */
	GSW_resetMode_t	eReset;
} GSW_reset_t;

/** Number of extended RMON counter. */
#define GSW_RMON_EXTEND_NUM	24

/**
   \brief Hardware platform extended RMON Counters.
   This structure contains additional RMON counters of one Ethernet Switch Port.
   These counters can be used by the packet classification engine and can be
   freely assigned to dedicated packet rules and flows.
    Used by \ref GSW_RMON_EXTEND_GET. */
typedef struct
{
	/** Ethernet Port number (zero-based counting). The valid range is hardware
	    dependent. An error code is delivered if the selected port is not
	    available. */
	u8		nPortId;
	/** Traffic flow counters */
	u32	nTrafficFlowCnt[GSW_RMON_EXTEND_NUM];
} GSW_RMON_extendGet_t;

/** \brief This structure describes the second, nano second and fractional
    nano second counterpart of the switch reference timer. This
    reference counter can be read by
    using \ref GSW_TIMESTAMP_TIMER_GET , and it can be modified
    by using \ref GSW_TIMESTAMP_TIMER_SET . */
typedef struct
{
	/** Second. Absolute second timer count. */
	u32	nSec;
	/** Nano Second. Absolute nanosecond timer count. */
	u32	nNanoSec;
	/** Fractional NanoSecond. Absolute fractional nanosecond timer count.
	    This counter specifis a 2^32 fractional 'nNanoSec'. */
	u32	nFractionalNanoSec;
} GSW_TIMESTAMP_Timer_t;

/** \brief This structure describes the port related time stamp.
    Used by \ref GSW_TIMESTAMP_PORT_READ.
*/
typedef struct
{
	/** Ethernet Port number (zero-based counting). The valid range is hardware
	    dependent. An error code is delivered if the selected port is not
	    available. */
	u8		nPortId;
	/** Second. Absolute second timer count. */
	u32	nIngressSec;
	/** Nano Second. Absolute nano second timer count. */
	u32	nIngressNanoSec;
	/** Second. Absolute second timer count. */
	u32	nEgressSec;
	/** Nano Second. Absolute nano second timer count. */
	u32	nEgressNanoSec;
} GSW_TIMESTAMP_PortRead_t;

/*@}*/ /* GSW_IOCTL_SYS */

/* -------------------------------------------------------------------------- */
/*                        IOCTL Command Definitions                           */
/* -------------------------------------------------------------------------- */

/** \addtogroup GSW_IOCTL_DEBUG */
/*@{*/

/**
   \brief Write to an internal register. The register offset defines which register to access in
   which table. This routine only accesses the M4599_PDI and
   the ETHSW_PDI of the switch. All PHY registers are accessed
   via \ref GSW_MDIO_DATA_WRITE and \ref GSW_MDIO_DATA_READ.
   Note that the switch API implementation checks whether the given address is
   inside the valid address range. It returns with an error in case an invalid
   address is given.

   \param GSW_register_t Pointer to \ref GSW_register_t.

   \remarks The function returns an error code in case an error occurs.
            The error code is described in \ref GSW_return_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
#define GSW_REGISTER_SET	GSW_IOW(GSW_FLOW_MAGIC, 0x01, GSW_register_t)

/**
   \brief Read an internal register. The register offset defines which register to access in
   which table. This routine only accesses the ETHSW_PDI of the switch.
   All PHY registers are accessed via \ref GSW_MDIO_DATA_WRITE and \ref GSW_MDIO_DATA_READ.
   Note that the switch API implementation checks whether the given address is
   inside the valid address range. It returns with an error in case an invalid
   address is given.

   \param GSW_register_t Pointer to \ref GSW_register_t.

   \remarks The function returns an error code in case an error occurs.
            The error code is described in \ref GSW_return_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
#define GSW_REGISTER_GET	GSW_IOWR(GSW_FLOW_MAGIC, 0x02, GSW_register_t)

/*@}*/ /* GSW_IOCTL_DEBUG */

/** \addtogroup GSW_IOCTL_IRQ */
/*@{*/
/**
   \brief Get the interrupt enable configuration. This assignment can be set using \ref GSW_IRQ_MASK_SET.

   \param GSW_irq_t Pointer to an \ref GSW_irq_t structure.

   \remarks The function returns an error code in case an error occurs.
            The error code is described in \ref GSW_return_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs

*/
#define GSW_IRQ_MASK_GET	GSW_IOWR(GSW_FLOW_MAGIC, 0x03, GSW_irq_t)

/**
   \brief Set the interrupt enable configuration. This assignment can be read using \ref GSW_IRQ_MASK_GET.
   Setting interrupts that are not supported by hardware results in an error response.

   \param GSW_irq_t Pointer to
      an \ref GSW_irq_t structure.

   \remarks The function returns an error code in case an error occurs.
            The error code is described in \ref GSW_return_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs

*/
#define GSW_IRQ_MASK_SET	GSW_IOW(GSW_FLOW_MAGIC, 0x04, GSW_irq_t)

/**
   \brief Read the interrupt status. Interrupt status indications can be cleared using \ref GSW_IRQ_STATUS_CLEAR.

   \param GSW_irq_t Pointer to
      an \ref GSW_irq_t structure.

   \remarks The function returns an error code in case an error occurs.
            The error code is described in \ref GSW_return_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs

*/
#define GSW_IRQ_GET	GSW_IOWR(GSW_FLOW_MAGIC, 0x05, GSW_irq_t)

/**
   \brief Clear individual interrupt status bits. Interrupt status indications can be read using \ref GSW_IRQ_GET.

   \param GSW_irq_t Pointer to
      an \ref GSW_irq_t structure.

   \remarks The function returns an error code in case an error occurs.
            The error code is described in \ref GSW_return_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs

*/
#define GSW_IRQ_STATUS_CLEAR GSW_IOW(GSW_FLOW_MAGIC, 0x06, GSW_irq_t)

/*@}*/ /* GSW_IOCTL_IRQ */

/** \addtogroup GSW_IOCTL_CLASS */
/*@{*/

/**
   \brief This command writes a rule pattern and action to the table of the packet
   classification engine. The pattern part describes the parameter to identify an
   incoming packet to which the dedicated actions should be applied.
   A rule can be read using the command \ref GSW_PCE_RULE_READ.

   \param GSW_PCE_rule_t Pointer to \ref GSW_PCE_rule_t.

   \remarks The function returns an error code in case an error occurs.
            The error code is described in \ref GSW_return_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
#define GSW_PCE_RULE_WRITE	GSW_IOW(GSW_FLOW_MAGIC, 0x07, GSW_PCE_rule_t)

/**
   \brief This command allows the reading out of a rule pattern and action of the
   packet classification engine.
   A rule can be written using the command \ref GSW_PCE_RULE_WRITE.

   \param GSW_PCE_rule_t Pointer to \ref GSW_PCE_rule_t.

   \remarks The function returns an error code in case an error occurs.
            The error code is described in \ref GSW_return_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
#define GSW_PCE_RULE_READ	GSW_IOWR(GSW_FLOW_MAGIC, 0x08, GSW_PCE_rule_t)

/**
   \brief This command deletes a complete rule from the packet classification engine.
   A delete operation is done on the rule of a dedicated index 'nIndex'.
   A rule can be written over using the command \ref GSW_PCE_RULE_WRITE.

   \param GSW_PCE_ruleDelete_t Pointer to \ref GSW_PCE_ruleDelete_t.

   \remarks The function returns an error code in case an error occurs.
            The error code is described in \ref GSW_return_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
#define GSW_PCE_RULE_DELETE  GSW_IOW(GSW_FLOW_MAGIC, 0x09, GSW_PCE_ruleDelete_t)


/*@}*/ /* GSW_IOCTL_CLASS */

/** \addtogroup GSW_IOCTL_SYS */
/*@{*/

/**
   \brief Forces a hardware reset of the switch device or switch macro. The device
   automatically comes back out of reset and contains the initial values.
   All previous configurations and statistics counters info are lost.

   \param GSW_reset_t Pointer to an \ref GSW_reset_t structure.

   \remarks The function returns an error code in case an error occurs.
            The error code is described in \ref GSW_return_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs

   \remarks Not supported for all devices
*/
#define GSW_RESET	GSW_IOW(GSW_FLOW_MAGIC, 0x0A, GSW_reset_t)

/**
   \brief Read out additional traffic flow (RMON) counters.
   The zero-based 'nPortId' structure element describes the physical switch
   port for the requested statistic information.

   \param GSW_RMON_extendGet_t  Pointer to a pre-allocated
   \ref GSW_RMON_extendGet_t structure. The structure element 'nPortId' is
   an input parameter that describes from which port to read the RMON counter.
   All remaining structure elements are filled with the counter values.
   The counter assignment needs to be done during the flow definition,
   for example in \ref GSW_PCE_RULE_WRITE.

   \remarks The function returns an error in case the given 'nPortId' is
   out of range.

   \remarks The function returns an error code in case an error occurs.
            The error code is described in \ref GSW_return_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
#define GSW_RMON_EXTEND_GET	GSW_IOWR(GSW_FLOW_MAGIC, 0x0B, GSW_RMON_extendGet_t)

/** \brief This command sets the packet timestamp reference counter.
    This reference timer is a switch global timer which is used by the
    packet timestamp capture when sending and receiving Ethernet port level.

    The command \ref GSW_TIMESTAMP_TIMER_GET allows to read out
    the current reference timer.

   \param GSW_TIMESTAMP_Timer_t Pointer to an \ref GSW_TIMESTAMP_Timer_t structure.

   \remarks The function returns an error code in case an error occurs.
            The error code is described in \ref GSW_return_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
#define GSW_TIMESTAMP_TIMER_SET GSW_IOW(GSW_FLOW_MAGIC, 0x0C, GSW_TIMESTAMP_Timer_t)

/** \brief This command reads out the switch global reference timer counter that is
    used by the packet timestamp capture when sending and receiving Ethernet port level.

    This reference timer is useful when processing the packet related
    timestamp information for delay calculation.

    The reference timer can be configured by \ref GSW_TIMESTAMP_TIMER_SET.

   \param GSW_TIMESTAMP_Timer_t Pointer to an \ref GSW_TIMESTAMP_Timer_t structure.

   \remarks The function returns an error code in case an error occurs.
            The error code is described in \ref GSW_return_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
#define GSW_TIMESTAMP_TIMER_GET GSW_IOWR(GSW_FLOW_MAGIC, 0x0D, GSW_TIMESTAMP_Timer_t)

/** \brief This command reads out the port related timestamp values. This timestamp is captured
    for ingress and egress packets triggered due to a traffic flow table timestamp action.
    A switch global reference timer is used for the timestamp capture operation.
    This global timer can be access by using \ref GSW_TIMESTAMP_TIMER_SET
    and \ref GSW_TIMESTAMP_TIMER_GET.

    A port related packet timestamp is triggered by the flow table
    action 'eTimestampAction'.

   \param GSW_TIMESTAMP_PortRead_t Pointer to
      an \ref GSW_TIMESTAMP_PortRead_t structure.

   \remarks The function returns an error code in case an error occurs.
            The error code is described in \ref GSW_return_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
#define GSW_TIMESTAMP_PORT_READ GSW_IOWR(GSW_FLOW_MAGIC, 0x0E, GSW_TIMESTAMP_PortRead_t)

/*@}*/ /* GSW_IOCTL_SYS */

#endif /* _GSW_ETHSW_FLOW_H_ */

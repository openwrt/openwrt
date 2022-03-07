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


#include "gsw_sw_init.h"

#ifdef GSW_SW_FKT
#undef GSW_SW_FKT
#endif /* GSW_SW_FKT */

#define GSW_SW_FKT(x, y) x ? (gsw_ll_fkt)y : NULL

gsw_ll_fkt ltq_fkt_ptr_tbl[] = {
	/* 0x00 */
	(gsw_ll_fkt) NULL,
	/* Command: GSW_MAC_TABLE_ENTRY_READ ; Index: 0x01 */
	(gsw_ll_fkt) GSW_MAC_TableEntryRead,
	/* Command: GSW_MAC_TABLE_ENTRY_QUERY ; Index: 0x02 */
	(gsw_ll_fkt) GSW_MAC_TableEntryQuery,
	/* Command: GSW_MAC_TABLE_ENTRY_ADD ; Index: 0x03 */
	(gsw_ll_fkt) GSW_MAC_TableEntryAdd,
	/* Command: GSW_MAC_TABLE_ENTRY_REMOVE ; Index: 0x04 */
	(gsw_ll_fkt) GSW_MAC_TableEntryRemove,
	/* Command: GSW_MAC_TABLE_CLEAR ; Index: 0x05 */
	(gsw_ll_fkt) GSW_MAC_TableClear,
	/* Command: GSW_STP_PORT_CFG_SET ; Index: 0x06 */
	(gsw_ll_fkt) GSW_SW_FKT(CONFIG_LTQ_STP, GSW_STP_PortCfgSet),
	/* Command: GSW_STP_PORT_CFG_GET ; Index: 0x07 */
	(gsw_ll_fkt) GSW_SW_FKT(CONFIG_LTQ_STP, GSW_STP_PortCfgGet),
	/* Command: GSW_STP_BPDU_RULE_SET ; Index: 0x08 */
	(gsw_ll_fkt) GSW_SW_FKT(CONFIG_LTQ_STP, GSW_STP_BPDU_RuleSet),
	/* Command: GSW_STP_BPDU_RULE_GET ; Index: 0x09 */
	(gsw_ll_fkt) GSW_SW_FKT(CONFIG_LTQ_STP, GSW_STP_BPDU_RuleGet),
	/* Command: GSW_8021X_EAPOL_RULE_GET ; Index: 0x0A */
	(gsw_ll_fkt) GSW_SW_FKT(CONFIG_LTQ_8021X, GSW_8021X_EAPOL_RuleGet),
	/* Command: GSW_8021X_EAPOL_RULE_SET ; Index: 0x0B */
	(gsw_ll_fkt) GSW_SW_FKT(CONFIG_LTQ_8021X, GSW_8021X_EAPOL_RuleSet),
	/* Command: GSW_8021X_PORT_CFG_GET ; Index: 0x0C */
	(gsw_ll_fkt) GSW_SW_FKT(CONFIG_LTQ_8021X, GSW_8021X_PortCfgGet),
	/* Command: GSW_8021X_PORT_CFG_SET ; Index: 0x0D */
	(gsw_ll_fkt) GSW_SW_FKT(CONFIG_LTQ_8021X, GSW_8021X_PortCfgSet),
	/* Command: GSW_VLAN_RESERVED_ADD ; Index: 0x0E */
	(gsw_ll_fkt) GSW_SW_FKT(CONFIG_LTQ_VLAN, GSW_VLAN_ReservedAdd),
	/* Command: GSW_VLAN_RESERVED_REMOVE ; Index: 0x0F */
	(gsw_ll_fkt) GSW_SW_FKT(CONFIG_LTQ_VLAN, GSW_VLAN_ReservedRemove),
	/* Command: GSW_VLAN_PORT_CFG_GET ; Index: 0x10 */
	(gsw_ll_fkt) GSW_SW_FKT(CONFIG_LTQ_VLAN, GSW_VLAN_PortCfgGet),
	/* Command: GSW_VLAN_PORT_CFG_SET ; Index: 0x11 */
	(gsw_ll_fkt) GSW_SW_FKT(CONFIG_LTQ_VLAN, GSW_VLAN_PortCfgSet),
	/* Command: GSW_VLAN_ID_CREATE ; Index: 0x12 */
	(gsw_ll_fkt) GSW_SW_FKT(CONFIG_LTQ_VLAN, GSW_VLAN_IdCreate),
	/* Command: GSW_VLAN_ID_DELETE ; Index: 0x13 */
	(gsw_ll_fkt) GSW_SW_FKT(CONFIG_LTQ_VLAN, GSW_VLAN_IdDelete),
	/* Command: GSW_VLAN_PORT_MEMBER_ADD ; Index: 0x14 */
	(gsw_ll_fkt) GSW_SW_FKT(CONFIG_LTQ_VLAN, GSW_VLAN_PortMemberAdd),
	/* Command: GSW_VLAN_PORT_MEMBER_REMOVE ; Index: 0x15 */
	(gsw_ll_fkt) GSW_SW_FKT(CONFIG_LTQ_VLAN, GSW_VLAN_PortMemberRemove),
	/* Command: GSW_VLAN_PORT_MEMBER_READ ; Index: 0x16 */
	(gsw_ll_fkt) GSW_SW_FKT(CONFIG_LTQ_VLAN, GSW_VLAN_PortMemberRead),
	/* Command: GSW_VLAN_ID_GET ; Index: 0x17 */
	(gsw_ll_fkt) GSW_SW_FKT(CONFIG_LTQ_VLAN, GSW_VLAN_IdGet),
	/* Command: GSW_QOS_PORT_CFG_SET ; Index: 0x18 */
	(gsw_ll_fkt) GSW_SW_FKT(CONFIG_LTQ_QOS, GSW_QoS_PortCfgSet),
	/* Command: GSW_QOS_PORT_CFG_GET ; Index: 0x19 */
	(gsw_ll_fkt) GSW_SW_FKT(CONFIG_LTQ_QOS, GSW_QoS_PortCfgGet),
	/* Command: GSW_QOS_DSCP_CLASS_SET ; Index: 0x1A */
	(gsw_ll_fkt) GSW_SW_FKT(CONFIG_LTQ_QOS, GSW_QoS_DSCP_ClassSet),
	/* Command: GSW_QOS_DSCP_CLASS_GET ; Index: 0x1B */
	(gsw_ll_fkt) GSW_SW_FKT(CONFIG_LTQ_QOS, GSW_QoS_DSCP_ClassGet),
	/* Command: GSW_QOS_PCP_CLASS_SET ; Index: 0x1C */
	(gsw_ll_fkt) GSW_SW_FKT(CONFIG_LTQ_QOS, GSW_QoS_PCP_ClassSet),
	/* Command: GSW_QOS_PCP_CLASS_GET ; Index: 0x1D */
	(gsw_ll_fkt) GSW_SW_FKT(CONFIG_LTQ_QOS, GSW_QoS_PCP_ClassGet),
	/* Command: GSW_QOS_DSCP_DROP_PRECEDENCE_CFG_SET ; Index: 0x1E */
	(gsw_ll_fkt) GSW_SW_FKT(CONFIG_LTQ_QOS, GSW_QoS_DSCP_DropPrecedenceCfgSet),
	/* Command: GSW_QOS_DSCP_DROP_PRECEDENCE_CFG_GET ; Index: 0x1F */
	(gsw_ll_fkt) GSW_SW_FKT(CONFIG_LTQ_QOS, GSW_QoS_DSCP_DropPrecedenceCfgGet),
	/* Command: GSW_QOS_PORT_REMARKING_CFG_SET ; Index: 0x20 */
	(gsw_ll_fkt) GSW_SW_FKT(CONFIG_LTQ_QOS, GSW_QoS_PortRemarkingCfgSet),
	/* Command: GSW_QOS_PORT_REMARKING_CFG_GET ; Index: 0x21 */
	(gsw_ll_fkt) GSW_SW_FKT(CONFIG_LTQ_QOS, GSW_QoS_PortRemarkingCfgGet),
	/* Command: GSW_QOS_CLASS_DSCP_SET ; Index: 0x22 */
	(gsw_ll_fkt) GSW_SW_FKT(CONFIG_LTQ_QOS, GSW_QoS_ClassDSCP_Set),
	/* Command: GSW_QOS_CLASS_DSCP_GET ; Index: 0x23 */
	(gsw_ll_fkt) GSW_SW_FKT(CONFIG_LTQ_QOS, GSW_QoS_ClassDSCP_Get),
	/* Command: GSW_QOS_CLASS_PCP_SET ; Index: 0x24 */
	(gsw_ll_fkt) GSW_SW_FKT(CONFIG_LTQ_QOS, GSW_QoS_ClassPCP_Set),
	/* Command: GSW_QOS_CLASS_PCP_GET ; Index: 0x25 */
	(gsw_ll_fkt) GSW_SW_FKT(CONFIG_LTQ_QOS, GSW_QoS_ClassPCP_Get),
	/* Command: GSW_QOS_SHAPER_CFG_SET ; Index: 0x26 */
	(gsw_ll_fkt) GSW_SW_FKT(CONFIG_LTQ_QOS, GSW_QoS_ShaperCfgSet),
	/* Command: GSW_QOS_SHAPER_CFG_GET ; Index: 0x27 */
	(gsw_ll_fkt) GSW_SW_FKT(CONFIG_LTQ_QOS, GSW_QoS_ShaperCfgGet),
	/* Command: GSW_QOS_SHAPER_QUEUE_ASSIGN ; Index: 0x28 */
	(gsw_ll_fkt) GSW_SW_FKT(CONFIG_LTQ_QOS, GSW_QoS_ShaperQueueAssign),
	/* Command: GSW_QOS_SHAPER_QUEUE_DEASSIGN ; Index: 0x29 */
	(gsw_ll_fkt) GSW_SW_FKT(CONFIG_LTQ_QOS, GSW_QoS_ShaperQueueDeassign),
	/* Command: GSW_QOS_SHAPER_QUEUE_GET ; Index: 0x2A */
	(gsw_ll_fkt) GSW_SW_FKT(CONFIG_LTQ_QOS, GSW_QoS_ShaperQueueGet),
	/* Command: GSW_QOS_WRED_CFG_SET ; Index: 0x2B */
	(gsw_ll_fkt) GSW_SW_FKT(CONFIG_LTQ_QOS, GSW_QoS_WredCfgSet),
	/* Command: GSW_QOS_WRED_CFG_GET ; Index: 0x2C */
	(gsw_ll_fkt) GSW_SW_FKT(CONFIG_LTQ_QOS, GSW_QoS_WredCfgGet),
	/* Command: GSW_QOS_WRED_QUEUE_CFG_SET ; Index: 0x2D */
	(gsw_ll_fkt) GSW_SW_FKT(CONFIG_LTQ_QOS, GSW_QoS_WredQueueCfgSet),
	/* Command: GSW_QOS_WRED_QUEUE_CFG_GET ; Index: 0x2E */
	(gsw_ll_fkt) GSW_SW_FKT(CONFIG_LTQ_QOS, GSW_QoS_WredQueueCfgGet),
	/* Command: GSW_QOS_METER_CFG_SET ; Index: 0x2F */
	(gsw_ll_fkt) GSW_SW_FKT(CONFIG_LTQ_QOS, GSW_QoS_MeterCfgSet),
	/* Command: GSW_QOS_METER_CFG_GET ; Index: 0x30 */
	(gsw_ll_fkt) GSW_SW_FKT(CONFIG_LTQ_QOS, GSW_QoS_MeterCfgGet),
	/* Command: GSW_QOS_METER_PORT_ASSIGN ; Index: 0x31 */
	(gsw_ll_fkt) GSW_SW_FKT(CONFIG_LTQ_QOS, GSW_QoS_MeterPortAssign),
	/* Command: GSW_QOS_METER_PORT_DEASSIGN ; Index: 0x32 */
	(gsw_ll_fkt) GSW_SW_FKT(CONFIG_LTQ_QOS, GSW_QoS_MeterPortDeassign),
	/* Command: GSW_QOS_METER_PORT_GET ; Index: 0x33 */
	(gsw_ll_fkt) GSW_SW_FKT(CONFIG_LTQ_QOS, GSW_QoS_MeterPortGet),
	/* Command: GSW_QOS_STORM_CFG_SET ; Index: 0x34 */
	(gsw_ll_fkt) GSW_SW_FKT(CONFIG_LTQ_QOS, GSW_QoS_StormCfgSet),
	/* Command: GSW_QOS_STORM_CFG_GET ; Index: 0x35 */
	(gsw_ll_fkt) GSW_SW_FKT(CONFIG_LTQ_QOS, GSW_QoS_StormCfgGet),
	/* Command: GSW_QOS_SCHEDULER_CFG_SET ; Index: 0x36 */
	(gsw_ll_fkt) GSW_SW_FKT(CONFIG_LTQ_QOS, GSW_QoS_SchedulerCfgSet),
	/* Command: GSW_QOS_SCHEDULER_CFG_GET ; Index: 0x37 */
	(gsw_ll_fkt) GSW_SW_FKT(CONFIG_LTQ_QOS, GSW_QoS_SchedulerCfgGet),
	/* Command: GSW_QOS_QUEUE_PORT_SET ; Index: 0x38 */
	(gsw_ll_fkt) GSW_SW_FKT(CONFIG_LTQ_QOS, GSW_QoS_QueuePortSet),
	/* Command: GSW_QOS_QUEUE_PORT_GET ; Index: 0x39 */
	(gsw_ll_fkt) GSW_SW_FKT(CONFIG_LTQ_QOS, GSW_QoS_QueuePortGet),
	/* Command: GSW_MULTICAST_SNOOP_CFG_SET ; Index: 0x3A */
	(gsw_ll_fkt) GSW_SW_FKT(CONFIG_LTQ_MULTICAST, GSW_MulticastSnoopCfgSet),
	/* Command: GSW_MULTICAST_SNOOP_CFG_GET ; Index: 0x3B */
	(gsw_ll_fkt) GSW_SW_FKT(CONFIG_LTQ_MULTICAST, GSW_MulticastSnoopCfgGet),
	/* Command: GSW_MULTICAST_ROUTER_PORT_ADD ; Index: 0x3C */
	(gsw_ll_fkt) GSW_SW_FKT(CONFIG_LTQ_MULTICAST, GSW_MulticastRouterPortAdd),
	/* Command: GSW_MULTICAST_ROUTER_PORT_REMOVE ; Index: 0x3D */
	(gsw_ll_fkt) GSW_SW_FKT(CONFIG_LTQ_MULTICAST, GSW_MulticastRouterPortRemove),
	/* Command: GSW_MULTICAST_ROUTER_PORT_READ ; Index: 0x3E */
	(gsw_ll_fkt) GSW_SW_FKT(CONFIG_LTQ_MULTICAST, GSW_MulticastRouterPortRead),
	/* Command: GSW_MULTICAST_TABLE_ENTRY_ADD ; Index: 0x3F */
	(gsw_ll_fkt) GSW_SW_FKT(CONFIG_LTQ_MULTICAST, GSW_MulticastTableEntryAdd),
	/* Command: GSW_MULTICAST_TABLE_ENTRY_REMOVE ; Index: 0x40 */
	(gsw_ll_fkt) GSW_SW_FKT(CONFIG_LTQ_MULTICAST, GSW_MulticastTableEntryRemove),
	/* Command: GSW_MULTICAST_TABLE_ENTRY_READ ; Index: 0x41 */
	(gsw_ll_fkt) GSW_SW_FKT(CONFIG_LTQ_MULTICAST, GSW_MulticastTableEntryRead),
	/* Command: GSW_HW_INIT ; Index: 0x42 */
	(gsw_ll_fkt) GSW_HW_Init,
	/* Command: GSW_VERSION_GET ; Index: 0x43 */
	(gsw_ll_fkt) GSW_VersionGet,
	/* Command: GSW_CAP_GET ; Index: 0x44 */
	(gsw_ll_fkt) GSW_CapGet,
	/* Command: GSW_CFG_SET ; Index: 0x45 */
	(gsw_ll_fkt) GSW_CfgSet,
	/* Command: GSW_CFG_GET ; Index: 0x46 */
	(gsw_ll_fkt) GSW_CfgGet,
	/* Command: GSW_ENABLE ; Index: 0x47 */
	(gsw_ll_fkt) GSW_Enable,
	/* Command: GSW_DISABLE ; Index: 0x48 */
	(gsw_ll_fkt) GSW_Disable,
	/* Command: GSW_PORT_CFG_GET ; Index: 0x49 */
	(gsw_ll_fkt) GSW_PortCfgGet,
	/* Command: GSW_PORT_CFG_SET ; Index: 0x4A */
	(gsw_ll_fkt) GSW_PortCfgSet,
	/* Command: GSW_CPU_PORT_CFG_SET ; Index: 0x4B */
	(gsw_ll_fkt) GSW_CPU_PortCfgSet,
	/* Command: GSW_CPU_PORT_CFG_GET ; Index: 0x4C */
	(gsw_ll_fkt) GSW_CPU_PortCfgGet,
	/* Command: GSW_CPU_PORT_EXTEND_CFG_SET ; Index: 0x4D */
	(gsw_ll_fkt) GSW_CPU_PortExtendCfgSet,
	/* Command: GSW_CPU_PORT_EXTEND_CFG_GET ; Index: 0x4E */
	(gsw_ll_fkt) GSW_CPU_PortExtendCfgGet,
	/* Command: GSW_PORT_LINK_CFG_GET ; Index: 0x4F */
	(gsw_ll_fkt) GSW_PortLinkCfgGet,
	/* Command: GSW_PORT_LINK_CFG_SET ; Index: 0x50 */
	(gsw_ll_fkt) GSW_PortLinkCfgSet,
	/* Command: GSW_PORT_RGMII_CLK_CFG_SET ; Index: 0x51 */
	(gsw_ll_fkt) GSW_PortRGMII_ClkCfgSet,
	/* Command: GSW_PORT_RGMII_CLK_CFG_GET ; Index: 0x52 */
	(gsw_ll_fkt) GSW_PortRGMII_ClkCfgGet,
	/* Command: GSW_PORT_PHY_QUERY ; Index: 0x53 */
	(gsw_ll_fkt) GSW_PortPHY_Query,
	/* Command: GSW_PORT_PHY_ADDR_GET ; Index: 0x54 */
	(gsw_ll_fkt) GSW_PortPHY_AddrGet,
	/* Command: GSW_PORT_REDIRECT_GET ; Index: 0x55 */
	(gsw_ll_fkt) GSW_PortRedirectGet,
	/* Command: GSW_PORT_REDIRECT_SET ; Index: 0x56 */
	(gsw_ll_fkt) GSW_PortRedirectSet,
	/* Command: GSW_MONITOR_PORT_CFG_GET ; Index: 0x57 */
	(gsw_ll_fkt) GSW_MonitorPortCfgGet,
	/* Command: GSW_MONITOR_PORT_CFG_SET ; Index: 0x58 */
	(gsw_ll_fkt) GSW_MonitorPortCfgSet,
	/* Command: GSW_RMON_PORT_GET ; Index: 0x59 */
	(gsw_ll_fkt) GSW_RMON_Port_Get,
	/* Command: GSW_RMON_CLEAR ; Index: 0x5A */
	(gsw_ll_fkt) GSW_RMON_Clear,
	/* Command: GSW_MDIO_CFG_GET ; Index: 0x5B */
	(gsw_ll_fkt) GSW_MDIO_CfgGet,
	/* Command: GSW_MDIO_CFG_SET ; Index: 0x5C */
	(gsw_ll_fkt) GSW_MDIO_CfgSet,
	/* Command: GSW_MDIO_DATA_READ ; Index: 0x5D */
	(gsw_ll_fkt) GSW_MDIO_DataRead,
	/* Command: GSW_MDIO_DATA_WRITE ; Index: 0x5E */
	(gsw_ll_fkt) GSW_MDIO_DataWrite,
	/* Command: GSW_MMD_DATA_READ ; Index: 0x5F */
	(gsw_ll_fkt) GSW_MmdDataRead,
	/* Command: GSW_MMD_DATA_WRITE ; Index: 0x60 */
	(gsw_ll_fkt) GSW_MmdDataWrite,
	/* Command: GSW_WOL_CFG_SET ; Index: 0x61 */
	(gsw_ll_fkt) GSW_SW_FKT(CONFIG_LTQ_WOL, GSW_WoL_CfgSet),
	/* Command: GSW_WOL_CFG_GET ; Index: 0x62 */
	(gsw_ll_fkt) GSW_SW_FKT(CONFIG_LTQ_WOL, GSW_WoL_CfgGet),
	/* Command: GSW_WOL_PORT_CFG_SET ; Index: 0x63 */
	(gsw_ll_fkt) GSW_SW_FKT(CONFIG_LTQ_WOL, GSW_WoL_PortCfgSet),
	/* Command: GSW_WOL_PORT_CFG_GET ; Index: 0x64 */
	(gsw_ll_fkt) GSW_SW_FKT(CONFIG_LTQ_WOL, GSW_WoL_PortCfgGet),
	/* Command: GSW_TRUNKING_CFG_GET ; Index: 0x65 */
	(gsw_ll_fkt) GSW_TrunkingCfgGet,
	/* Command: GSW_TRUNKING_CFG_SET ; Index: 0x66 */
	(gsw_ll_fkt) GSW_TrunkingCfgSet,
	/* Command: GSW_TRUNKING_PORT_CFG_GET ; Index: 0x67 */
	(gsw_ll_fkt) GSW_TrunkingPortCfgGet,
	/* Command: GSW_TRUNKING_PORT_CFG_SET ; Index: 0x68 */
	(gsw_ll_fkt) GSW_TrunkingPortCfgSet,
	/* Command: GSW_QOS_WRED_PORT_CFG_SET ; Index: 0x69 */
	(gsw_ll_fkt) GSW_SW_FKT(CONFIG_LTQ_QOS, GSW_QoS_WredPortCfgSet),
	/* Command: GSW_QOS_WRED_PORT_CFG_GET ; Index: 0x6a */
	(gsw_ll_fkt) GSW_SW_FKT(CONFIG_LTQ_QOS, GSW_QoS_WredPortCfgGet),
	/* Command: GSW_QOS_FLOWCTRL_CFG_SET ; Index: 0x6b */
	(gsw_ll_fkt) GSW_SW_FKT(CONFIG_LTQ_QOS, GSW_QoS_FlowctrlCfgSet),
	/* Command: GSW_QOS_FLOWCTRL_CFG_GET ; Index: 0x6c */
	(gsw_ll_fkt) GSW_SW_FKT(CONFIG_LTQ_QOS, GSW_QoS_FlowctrlCfgGet),
	/* Command: GSW_QOS_FLOWCTRL_PORT_CFG_SET ; Index: 0x6d */
	(gsw_ll_fkt) GSW_SW_FKT(CONFIG_LTQ_QOS, GSW_QoS_FlowctrlPortCfgSet),
	/* Command: GSW_QOS_FLOWCTRL_PORT_CFG_GET ; Index: 0x6e */
	(gsw_ll_fkt) GSW_SW_FKT(CONFIG_LTQ_QOS, GSW_QoS_FlowctrlPortCfgGet),
	/* Command: GSW_QOS_QUEUE_BUFFER_RESERVE_CFG_SET ; Index: 0x6f */
	(gsw_ll_fkt) GSW_SW_FKT(CONFIG_LTQ_QOS, GSW_QoS_QueueBufferReserveCfgSet),
	/* Command: GSW_QOS_QUEUE_BUFFER_RESERVE_CFG_GET ; Index: 0x70 */
	(gsw_ll_fkt) GSW_SW_FKT(CONFIG_LTQ_QOS, GSW_QoS_QueueBufferReserveCfgGet),
	/* Command: GSW_SVLAN_CFG_GET ; Index: 0x71 */
	(gsw_ll_fkt) GSW_SW_FKT(CONFIG_LTQ_VLAN, GSW_SVLAN_CfgGet),
	/* Command: GSW_SVLAN_CFG_SET ; Index: 0x72 */
	(gsw_ll_fkt) GSW_SW_FKT(CONFIG_LTQ_VLAN, GSW_SVLAN_CfgSet),
	/* Command: GSW_SVLAN_PORT_CFG_GET ; Index: 0x73 */
	(gsw_ll_fkt) GSW_SW_FKT(CONFIG_LTQ_VLAN, GSW_SVLAN_PortCfgGet),
	/* Command: GSW_SVLAN_PORT_CFG_SET ; Index: 0x74 */
	(gsw_ll_fkt) GSW_SW_FKT(CONFIG_LTQ_VLAN, GSW_SVLAN_PortCfgSet),
	/* Command: GSW_QOS_SVLAN_CLASS_PCP_PORT_SET ; Index: 0x75 */
	(gsw_ll_fkt) GSW_SW_FKT(CONFIG_LTQ_VLAN, GSW_QoS_SVLAN_ClassPCP_PortSet),
	/* Command: GSW_QOS_SVLAN_CLASS_PCP_PORT_GET ; Index: 0x76 */
	(gsw_ll_fkt) GSW_SW_FKT(CONFIG_LTQ_VLAN, GSW_QoS_SVLAN_ClassPCP_PortGet),
	/* Command: GSW_QOS_SVLAN_PCP_CLASS_SET ; Index: 0x77 */
	(gsw_ll_fkt) GSW_SW_FKT(CONFIG_LTQ_VLAN, GSW_QoS_SVLAN_PCP_ClassSet),
	/* Command: GSW_QOS_SVLAN_PCP_CLASS_GET ; Index: 0x78 */
	(gsw_ll_fkt) GSW_SW_FKT(CONFIG_LTQ_VLAN, GSW_QoS_SVLAN_PCP_ClassGet),
	/* Command: GSW_VLAN_MEMBER_INIT ; Index: 0x79 */
	(gsw_ll_fkt) GSW_SW_FKT(CONFIG_LTQ_VLAN, GSW_VLAN_Member_Init),
};

gsw_lowlevel_fkts_t GSW_FLOW_fkt_tbl = {
	NULL, /* pNext */
	(u16) GSW_MAGIC, /* nType */
	121, /* nNumFkts */
	ltq_fkt_ptr_tbl /* pFkts */
};

gsw_ll_fkt gsw_flow_fkt_ptr_tbl[] = {
	/* 0x00 */
	(gsw_ll_fkt) NULL,
	/* Command: GSW_REGISTER_SET ; Index: 0x01 */
	(gsw_ll_fkt) GSW_RegisterSet,
	/* Command: GSW_REGISTER_GET ; Index: 0x02 */
	(gsw_ll_fkt) GSW_RegisterGet,
	/* Command: GSW_IRQ_MASK_GET ; Index: 0x03 */
	(gsw_ll_fkt) GSW_IrqMaskGet,
	/* Command: GSW_IRQ_MASK_SET ; Index: 0x04 */
	(gsw_ll_fkt) GSW_IrqMaskSet,
	/* Command: GSW_IRQ_GET ; Index: 0x05 */
	(gsw_ll_fkt) GSW_IrqGet,
	/* Command: GSW_IRQ_STATUS_CLEAR ; Index: 0x06 */
	(gsw_ll_fkt) GSW_IrqStatusClear,
	/* Command: GSW_PCE_RULE_WRITE ; Index: 0x07 */
	(gsw_ll_fkt) GSW_PceRuleWrite,
	/* Command: GSW_PCE_RULE_READ ; Index: 0x08 */
	(gsw_ll_fkt) GSW_PceRuleRead,
	/* Command: GSW_PCE_RULE_DELETE ; Index: 0x09 */
	(gsw_ll_fkt) GSW_PceRuleDelete,
	/* Command: GSW_RESET ; Index: 0x0A */
	(gsw_ll_fkt) GSW_Reset,
	/* Command: GSW_RMON_EXTEND_GET ; Index: 0x0B */
	(gsw_ll_fkt) GSW_RMON_ExtendGet,
	/* Command: GSW_TIMESTAMP_TIMER_SET ; Index: 0x0C */
	(gsw_ll_fkt) GSW_TimestampTimerSet,
	/* Command: GSW_TIMESTAMP_TIMER_GET ; Index: 0x0D */
	(gsw_ll_fkt) GSW_TimestampTimerGet,
	/* Command: GSW_TIMESTAMP_PORT_READ ; Index: 0x0E */
	(gsw_ll_fkt) GSW_TimestampPortRead,
};

gsw_lowlevel_fkts_t flow_fkt_tbl = {
	&GSW_FLOW_fkt_tbl, /* pNext */
	(u16) GSW_FLOW_MAGIC, /* nType */
	15, /* nNumFkts */
	gsw_flow_fkt_ptr_tbl /* pFkts */
};


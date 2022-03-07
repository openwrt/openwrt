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


/* Group definitions for Doxygen */
/** \defgroup FLOW_LL Ethernet Switch Application Kernel Interface
    This chapter describes the entire interface to access and
    configure the services of the switch module in OS kernel space.*/
/*@{*/
/** \defgroup FLOW_LL_BRIDGE Ethernet Bridging Functions
    Ethernet bridging (or switching) is the basic task of the device. It
    provides individual configurations per port and standard global
    switch features.
*/
/** \defgroup FLOW_LL_CLASSUNIT Packet Classification Engine
    Configures and controls the classification unit of the XWAY VRX200
    and XWAY GRX200 Family hardware.
*/
/** \defgroup FLOW_LL_DEBUG Debug Features
    XWAY VRX200 and XWAY GRX200 Family specific features for system
    integration and debug sessions.
*/
/** \defgroup FLOW_LL_IRQ Interrupt Handling
    Configure XWAY VRX200 and XWAY GRX200 Family specific hardware
    support to generate interrupts
    and read out the interrupt sources.
*/
/** \defgroup FLOW_LL_MULTICAST Multicast Functions
    IGMP/MLD snooping configuration and support for IGMPv1, IGMPv2, IGMPv3,
    MLDv1, and MLDv2.
*/
/** \defgroup FLOW_LL_OAM Operation, Administration, and Management Functions
    This chapter summarizes the functions that are provided to monitor the
    data traffic passing through the device.
*/
/** \defgroup FLOW_LL_QOS Quality of Service Functions
    Switch and port configuration for Quality of Service (QoS).
*/
/** \defgroup FLOW_LL_VLAN VLAN Functions
    This chapter describes VLAN bridging functionality. This includes support for
    Customer VLAN Tags (CTAG VLAN) and also Service VLAN Tags (STAG VLAN/SVLAN).
*/
/** \defgroup GSWIP_ROUTE Operation, Administration, and Management Functions
    This chapter summarizes the functions that are provided to monitor the
    data traffic passing through the device.
*/
/*@}*/

/* ------------------------------------------------------------------------- */
/*                       Function Declaration                                */
/* ------------------------------------------------------------------------- */

/** \addtogroup FLOW_LL_BRIDGE */
/*@{*/
/**
   This is the switch API low-level function for
   the \ref GSW_8021X_EAPOL_RULE_GET command.

   \param cdev This parameter is a pointer to the device context
   which contains all information related to this special
   instance of the device.
   \param parm Pointer to \ref GSW_8021X_EAPOL_Rule_t.

	\remarks The function returns an error code in case an error occurs.
		The error code is described in \ref GSW_status_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
GSW_return_t GSW_8021X_EAPOL_RuleGet(void *cdev,
                                     GSW_8021X_EAPOL_Rule_t *parm);

/**
   This is the switch API low-level function for
   the \ref GSW_8021X_EAPOL_RULE_SET command.

   \param cdev This parameter is a pointer to the device context
   which contains all information related to this special
   instance of the device.
   \param parm Pointer to \ref GSW_8021X_EAPOL_Rule_t.

	\remarks The function returns an error code in case an error occurs.
			The error code is described in \ref GSW_status_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
GSW_return_t GSW_8021X_EAPOL_RuleSet(void *cdev,
                                     GSW_8021X_EAPOL_Rule_t *parm);

/**
   This is the switch API low-level function for
   the \ref GSW_8021X_PORT_CFG_GET command.

   \param cdev This parameter is a pointer to the device context
   which contains all information related to this special
   instance of the device.
   \param parm Pointer to a
      802.1x port authorized state port
      configuration \ref GSW_8021X_portCfg_t

	\remarks The function returns an error code in case an error occurs.
		The error code is described in \ref GSW_status_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
GSW_return_t GSW_8021X_PortCfgGet(void *cdev,
                                  GSW_8021X_portCfg_t *parm);

/**
   This is the switch API low-level function for
   the \ref GSW_8021X_PORT_CFG_SET command.

   \param cdev This parameter is a pointer to the device context
   which contains all information related to this special
   instance of the device.
   \param parm Pointer to a
      802.1x port authorized state port
      configuration \ref GSW_8021X_portCfg_t

	\remarks The function returns an error code in case an error occurs.
		The error code is described in \ref GSW_status_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
GSW_return_t GSW_8021X_PortCfgSet(void *cdev,
                                  GSW_8021X_portCfg_t *parm);

/**
   This is the switch API low-level function for
   the \ref GSW_MAC_TABLE_CLEAR command.

   \param cdev This parameter is a pointer to the device context
   which contains all information related to this special
   instance of the device.

	\remarks The function returns an error code in case an error occurs.
		The error code is described in \ref GSW_status_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
GSW_return_t GSW_MAC_TableClear(void *cdev);

/**
   This is the switch API low-level function for
   the \ref GSW_MAC_TABLE_ENTRY_ADD command.

   \param cdev This parameter is a pointer to the device context
   which contains all information related to this special
   instance of the device.
   \param parm Pointer to a MAC table entry
   \ref GSW_MAC_tableAdd_t structure.

	\remarks The function returns an error code in case an error occurs.
		The error code is described in \ref GSW_status_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
GSW_return_t GSW_MAC_TableEntryAdd(void *cdev,
                                   GSW_MAC_tableAdd_t *parm);

/**
   This is the switch API low-level function for
   the \ref GSW_MAC_TABLE_ENTRY_QUERY command.

   \param cdev This parameter is a pointer to the device context
   which contains all information related to this special
   instance of the device.
   \param parm Pointer to a MAC table entry
   \ref GSW_MAC_tableQuery_t structure that is filled out by the switch
   implementation.

	\remarks The function returns an error code in case an error occurs.
		The error code is described in \ref GSW_status_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
GSW_return_t GSW_MAC_TableEntryQuery(void *cdev,
                                     GSW_MAC_tableQuery_t *parm);

/**
   This is the switch API low-level function for
   the \ref GSW_MAC_TABLE_ENTRY_READ command.

   \param cdev This parameter is a pointer to the device context
   which contains all information related to this special
   instance of the device.
   \param parm Pointer to a MAC table entry
   \ref GSW_MAC_tableRead_t structure that is filled out by the switch
   implementation.

	\remarks The function returns an error code in case an error occurs.
		The error code is described in \ref GSW_status_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
GSW_return_t GSW_MAC_TableEntryRead(void *cdev,
                                    GSW_MAC_tableRead_t *parm);

/**
   This is the switch API low-level function for
   the \ref GSW_MAC_TABLE_ENTRY_REMOVE command.

   \param cdev This parameter is a pointer to the device context
   which contains all information related to this special
   instance of the device.
   \param parm Pointer to a MAC table entry
   \ref GSW_MAC_tableRemove_t structure.

	\remarks The function returns an error code in case an error occurs.
		The error code is described in \ref GSW_status_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
GSW_return_t GSW_MAC_TableEntryRemove(void *cdev,
                                      GSW_MAC_tableRemove_t *parm);

/**
   This is the switch API low-level function for
   the \ref GSW_STP_BPDU_RULE_GET command.

   \param cdev This parameter is a pointer to the device context
   which contains all information related to this special
   instance of the device.
   \param parm Pointer to \ref GSW_STP_BPDU_Rule_t.

	\remarks The function returns an error code in case an error occurs.
		The error code is described in \ref GSW_status_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
GSW_return_t GSW_STP_BPDU_RuleGet(void *cdev,
                                  GSW_STP_BPDU_Rule_t *parm);

/**
   This is the switch API low-level function for
   the \ref GSW_STP_BPDU_RULE_SET command.

   \param cdev This parameter is a pointer to the device context
   which contains all information related to this special
   instance of the device.
   \param parm Pointer to \ref GSW_STP_BPDU_Rule_t.

	\remarks The function returns an error code in case an error occurs.
		The error code is described in \ref GSW_status_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
GSW_return_t GSW_STP_BPDU_RuleSet(void *cdev,
                                  GSW_STP_BPDU_Rule_t *parm);

/**
   This is the switch API low-level function for
   the \ref GSW_STP_PORT_CFG_GET command.

   \param cdev This parameter is a pointer to the device context
   which contains all information related to this special
   instance of the device.
   \param parm Pointer to \ref GSW_STP_portCfg_t.

	\remarks The function returns an error code in case an error occurs.
		The error code is described in \ref GSW_status_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
GSW_return_t GSW_STP_PortCfgGet(void *cdev,
                                GSW_STP_portCfg_t *parm);

/**
   This is the switch API low-level function for
   the \ref GSW_STP_PORT_CFG_SET command.

   \param cdev This parameter is a pointer to the device context
   which contains all information related to this special
   instance of the device.
   \param parm Pointer to \ref GSW_STP_portCfg_t.

	\remarks The function returns an error code in case an error occurs.
		The error code is described in \ref GSW_status_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
GSW_return_t GSW_STP_PortCfgSet(void *cdev,
                                GSW_STP_portCfg_t *parm);

/**
   This is the switch API low-level function for
   the \ref GSW_TRUNKING_CFG_GET command.

   \param cdev This parameter is a pointer to the device context
   which contains all information related to this special
   instance of the device.
   \param parm Pointer to a
      configuration \ref GSW_trunkingCfg_t

	\remarks The function returns an error code in case an error occurs.
		The error code is described in \ref GSW_status_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
GSW_return_t GSW_TrunkingCfgGet(void *cdev,
                                GSW_trunkingCfg_t *parm);

/**
   This is the switch API low-level function for
   the \ref GSW_TRUNKING_CFG_SET command.

   \param cdev This parameter is a pointer to the device context
   which contains all information related to this special
   instance of the device.
   \param parm Pointer to a
      configuration \ref GSW_trunkingCfg_t

	\remarks The function returns an error code in case an error occurs.
		The error code is described in \ref GSW_status_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
GSW_return_t GSW_TrunkingCfgSet(void *cdev,
                                GSW_trunkingCfg_t *parm);

/**
   This is the switch API low-level function for
   the \ref GSW_TRUNKING_PORT_CFG_GET command.

   \param cdev This parameter is a pointer to the device context
   which contains all information related to this special
   instance of the device.
   \param parm Pointer to a
      configuration \ref GSW_trunkingPortCfg_t

	\remarks The function returns an error code in case an error occurs.
		The error code is described in \ref GSW_status_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
GSW_return_t GSW_TrunkingPortCfgGet(void *cdev,
                                    GSW_trunkingPortCfg_t *parm);

/**
   This is the switch API low-level function for
   the \ref GSW_TRUNKING_PORT_CFG_SET command.

   \param cdev This parameter is a pointer to the device context
   which contains all information related to this special
   instance of the device.
   \param parm Pointer to a
      configuration \ref GSW_trunkingPortCfg_t

	\remarks The function returns an error code in case an error occurs.
		The error code is described in \ref GSW_status_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
GSW_return_t GSW_TrunkingPortCfgSet(void *cdev,
                                    GSW_trunkingPortCfg_t *parm);

/*@}*/ /* FLOW_LL_BRIDGE */
/** \addtogroup FLOW_LL_VLAN */
/*@{*/
/**
   This is the switch API low-level function for
   the \ref GSW_SVLAN_CFG_GET command.

   \param cdev This parameter is a pointer to the device context
   which contains all information related to this special
   instance of the device.
   \param parm Pointer to an \ref GSW_SVLAN_cfg_t
      structure element.

	\remarks The function returns an error code in case an error occurs.
		The error code is described in \ref GSW_status_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
GSW_return_t GSW_SVLAN_CfgGet(void *cdev,
                              GSW_SVLAN_cfg_t *parm);

/**
   This is the switch API low-level function for
   the \ref GSW_SVLAN_CFG_SET command.

   \param cdev This parameter is a pointer to the device context
   which contains all information related to this special
   instance of the device.
   \param parm Pointer to an \ref GSW_SVLAN_cfg_t
      structure element.

	\remarks The function returns an error code in case an error occurs.
		The error code is described in \ref GSW_status_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
GSW_return_t GSW_SVLAN_CfgSet(void *cdev,
                              GSW_SVLAN_cfg_t *parm);

/**
   This is the switch API low-level function for
   the \ref GSW_SVLAN_PORT_CFG_GET command.

   \param cdev This parameter is a pointer to the device context
   which contains all information related to this special
   instance of the device.
   \param parm Pointer to an
      \ref GSW_SVLAN_portCfg_t structure element. Based on the parameter
      'nPortId', the switch API implementation fills out the remaining structure
      elements.

	\remarks The function returns an error code in case an error occurs.
		The error code is described in \ref GSW_status_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
GSW_return_t GSW_SVLAN_PortCfgGet(void *cdev,
                                  GSW_SVLAN_portCfg_t *parm);

/**
   This is the switch API low-level function for
   the \ref GSW_SVLAN_PORT_CFG_SET command.

   \param cdev This parameter is a pointer to the device context
   which contains all information related to this special
   instance of the device.
   \param parm Pointer to an \ref GSW_SVLAN_portCfg_t
      structure element.

	\remarks The function returns an error code in case an error occurs.
		The error code is described in \ref GSW_status_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
GSW_return_t GSW_SVLAN_PortCfgSet(void *cdev,
                                  GSW_SVLAN_portCfg_t *parm);
/**
   This is the switch API low-level function for
   the \ref GSW_VLAN_MEMBER_INIT command.

   \param cdev This parameter is a pointer to the device context
   which contains all information related to this special
   instance of the device.
   \param parm Pointer to
      an \ref GSW_VLAN_memberInit_t structure element.

	\remarks The function returns an error code in case an error occurs.
		The error code is described in \ref GSW_status_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
GSW_return_t GSW_VLAN_Member_Init(void *cdev,
                                  GSW_VLAN_memberInit_t *parm);
/**
   This is the switch API low-level function for
   the \ref GSW_VLAN_ID_CREATE command.

   \param cdev This parameter is a pointer to the device context
   which contains all information related to this special
   instance of the device.
   \param parm Pointer to
      an \ref GSW_VLAN_IdCreate_t structure element.

	\remarks The function returns an error code in case an error occurs.
		The error code is described in \ref GSW_status_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
GSW_return_t GSW_VLAN_IdCreate(void *cdev,
                               GSW_VLAN_IdCreate_t *parm);

/**
   This is the switch API low-level function for
   the \ref GSW_VLAN_ID_DELETE command.

   \param cdev This parameter is a pointer to the device context
   which contains all information related to this special
   instance of the device.
   \param parm Pointer to an \ref GSW_VLAN_IdDelete_t structure element.

	\remarks A VLAN ID can only be removed in case it was created by
		\ref GSW_VLAN_ID_CREATE and is currently not assigned
		to any Ethernet port (done using \ref GSW_VLAN_PORT_MEMBER_ADD).

	\remarks The function returns an error code in case an error occurs.
		The error code is described in \ref GSW_status_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
GSW_return_t GSW_VLAN_IdDelete(void *cdev,
                               GSW_VLAN_IdDelete_t *parm);

/**
   This is the switch API low-level function for
   the \ref GSW_VLAN_ID_GET command.

   \param cdev This parameter is a pointer to the device context
   which contains all information related to this special
   instance of the device.
   \param parm Pointer to \ref GSW_VLAN_IdGet_t.

	\remarks The function returns an error code in case an error occurs.
		The error code is described in \ref GSW_status_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
GSW_return_t GSW_VLAN_IdGet(void *cdev,
                            GSW_VLAN_IdGet_t *parm);

/**
   This is the switch API low-level function for
   the \ref GSW_VLAN_PORT_CFG_GET command.

   \param cdev This parameter is a pointer to the device context
   which contains all information related to this special
   instance of the device.
   \param parm Pointer to an
      \ref GSW_VLAN_portCfg_t structure element. Based on the parameter
      'nPortId', the switch API implementation fills out the remaining structure
      elements.

	\remarks The function returns an error code in case an error occurs.
		The error code is described in \ref GSW_status_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
GSW_return_t GSW_VLAN_PortCfgGet(void *cdev,
                                 GSW_VLAN_portCfg_t *parm);

/**
   This is the switch API low-level function for
   the \ref GSW_VLAN_PORT_CFG_SET command.

   \param cdev This parameter is a pointer to the device context
   which contains all information related to this special
   instance of the device.
   \param parm Pointer to an \ref GSW_VLAN_portCfg_t
      structure element.

	\remarks The function returns an error code in case an error occurs.
		The error code is described in \ref GSW_status_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
GSW_return_t GSW_VLAN_PortCfgSet(void *cdev,
                                 GSW_VLAN_portCfg_t *parm);

/**
   This is the switch API low-level function for
   the \ref GSW_VLAN_PORT_MEMBER_ADD command.

   \param cdev This parameter is a pointer to the device context
   which contains all information related to this special
   instance of the device.
   \param parm Pointer to
      an \ref GSW_VLAN_portMemberAdd_t structure element.

	\remarks The function returns an error code in case an error occurs.
		The error code is described in \ref GSW_status_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
GSW_return_t GSW_VLAN_PortMemberAdd(void *cdev,
                                    GSW_VLAN_portMemberAdd_t *parm);

/**
   This is the switch API low-level function for
   the \ref GSW_VLAN_PORT_MEMBER_READ command.

   \param cdev This parameter is a pointer to the device context
   which contains all information related to this special
   instance of the device.
   \param parm Pointer to
      an \ref GSW_VLAN_portMemberRead_t structure element.

	\remarks The function returns an error code in case an error occurs.
		The error code is described in \ref GSW_status_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
GSW_return_t GSW_VLAN_PortMemberRead(void *cdev,
                                     GSW_VLAN_portMemberRead_t *parm);

/**
   This is the switch API low-level function for
   the \ref GSW_VLAN_PORT_MEMBER_REMOVE command.

   \param cdev This parameter is a pointer to the device context
   which contains all information related to this special
   instance of the device.
   \param parm Pointer to
      an \ref GSW_VLAN_portMemberRemove_t structure element.

	\remarks The function returns an error code in case an error occurs.
		The error code is described in \ref GSW_status_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
GSW_return_t GSW_VLAN_PortMemberRemove(void *cdev,
                                       GSW_VLAN_portMemberRemove_t *parm);

/**
   This is the switch API low-level function for
   the \ref GSW_VLAN_RESERVED_ADD command.

   \param cdev This parameter is a pointer to the device context
   which contains all information related to this special
   instance of the device.
   \param parm Pointer to
      an \ref GSW_VLAN_reserved_t structure element.

	\remarks The function returns an error code in case an error occurs.
		The error code is described in \ref GSW_status_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
GSW_return_t GSW_VLAN_ReservedAdd(void *cdev,
                                  GSW_VLAN_reserved_t *parm);

/**
   This is the switch API low-level function for
   the \ref GSW_VLAN_RESERVED_REMOVE command.

   \param cdev This parameter is a pointer to the device context
   which contains all information related to this special
   instance of the device.
   \param parm Pointer to
      an \ref GSW_VLAN_reserved_t structure element.

	\remarks The function returns an error code in case an error occurs.
		The error code is described in \ref GSW_status_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
GSW_return_t GSW_VLAN_ReservedRemove(void *cdev,
                                     GSW_VLAN_reserved_t *parm);

/*@}*/ /* FLOW_LL_VLAN */
/** \addtogroup FLOW_LL_QOS */
/*@{*/
/**
   This is the switch API low-level function for
   the \ref GSW_QOS_CLASS_DSCP_GET command.

   \param cdev This parameter is a pointer to the device context
   which contains all information related to this special
   instance of the device.
   \param parm Pointer to the DSCP mapping parameter
		\ref GSW_QoS_ClassDSCP_Cfg_t.

	\remarks The function returns an error code in case an error occurs.
		The error code is described in \ref GSW_status_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
GSW_return_t GSW_QoS_ClassDSCP_Get(void *cdev,
                                   GSW_QoS_ClassDSCP_Cfg_t *parm);

/**
   This is the switch API low-level function for
   the \ref GSW_QOS_CLASS_DSCP_SET command.

   \param cdev This parameter is a pointer to the device context
   which contains all information related to this special
   instance of the device.
   \param parm Pointer to the DSCP mapping parameter
   \ref GSW_QoS_ClassDSCP_Cfg_t.

	\remarks The function returns an error code in case an error occurs.
		The error code is described in \ref GSW_status_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
GSW_return_t GSW_QoS_ClassDSCP_Set(void *cdev,
                                   GSW_QoS_ClassDSCP_Cfg_t *parm);

/**
   This is the switch API low-level function for
   the \ref GSW_QOS_CLASS_PCP_GET command.

   \param cdev This parameter is a pointer to the device context
   which contains all information related to this special
   instance of the device.
   \param parm Pointer to the PCP priority mapping parameter
   \ref GSW_QoS_ClassPCP_Cfg_t.

	\remarks The function returns an error code in case an error occurs.
		The error code is described in \ref GSW_status_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
GSW_return_t GSW_QoS_ClassPCP_Get(void *cdev,
                                  GSW_QoS_ClassPCP_Cfg_t *parm);

/**
	This is the switch API low-level function for
	the \ref GSW_QOS_CLASS_PCP_SET command.

	\param cdev This parameter is a pointer to the device context
   which contains all information related to this special
   instance of the device.
   \param parm Pointer to the PCP priority mapping parameter
   \ref GSW_QoS_ClassPCP_Cfg_t.

	\remarks The function returns an error code in case an error occurs.
		The error code is described in \ref GSW_status_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
GSW_return_t GSW_QoS_ClassPCP_Set(void *cdev,
                                  GSW_QoS_ClassPCP_Cfg_t *parm);

/**
   This is the switch API low-level function for
   the \ref GSW_QOS_DSCP_CLASS_GET command.

   \param cdev This parameter is a pointer to the device context
   which contains all information related to this special
   instance of the device.
   \param parm Pointer to the QoS filter parameters
   \ref GSW_QoS_DSCP_ClassCfg_t.

	\remarks The function returns an error code in case an error occurs.
		The error code is described in \ref GSW_status_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
GSW_return_t GSW_QoS_DSCP_ClassGet(void *cdev,
                                   GSW_QoS_DSCP_ClassCfg_t *parm);

/**
   This is the switch API low-level function for
   the \ref GSW_QOS_DSCP_CLASS_SET command.

   \param cdev This parameter is a pointer to the device context
   which contains all information related to this special
   instance of the device.
   \param parm Pointer to the QoS filter parameters
   \ref GSW_QoS_DSCP_ClassCfg_t.

	\remarks The function returns an error code in case an error occurs.
		The error code is described in \ref GSW_status_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
GSW_return_t GSW_QoS_DSCP_ClassSet(void *cdev,
                                   GSW_QoS_DSCP_ClassCfg_t *parm);

/**
   This is the switch API low-level function for
   the \ref GSW_QOS_DSCP_DROP_PRECEDENCE_CFG_GET command.

   \param cdev This parameter is a pointer to the device context
   which contains all information related to this special
   instance of the device.
   \param parm Pointer to the QoS
   DSCP drop precedence parameters
   \ref GSW_QoS_DSCP_DropPrecedenceCfg_t.

	\remarks The function returns an error code in case an error occurs.
		The error code is described in \ref GSW_status_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
GSW_return_t GSW_QoS_DSCP_DropPrecedenceCfgGet(void *cdev,
        GSW_QoS_DSCP_DropPrecedenceCfg_t *parm);

/**
   This is the switch API low-level function for
   the \ref GSW_QOS_DSCP_DROP_PRECEDENCE_CFG_SET command.

   \param cdev This parameter is a pointer to the device context
   which contains all information related to this special
   instance of the device.
   \param parm Pointer to the QoS
   DSCP drop precedence parameters
   \ref GSW_QoS_DSCP_DropPrecedenceCfg_t.

	\remarks The function returns an error code in case an error occurs.
		The error code is described in \ref GSW_status_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
GSW_return_t GSW_QoS_DSCP_DropPrecedenceCfgSet(void *cdev,
        GSW_QoS_DSCP_DropPrecedenceCfg_t *parm);

/**
   This is the switch API low-level function for
   the \ref GSW_QOS_FLOWCTRL_CFG_GET command.

   \param cdev This parameter is a pointer to the device context
   which contains all information related to this special
   instance of the device.
   \param parm Pointer to the parameters
   structure \ref GSW_QoS_FlowCtrlCfg_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
GSW_return_t GSW_QoS_FlowctrlCfgGet(void *cdev,
                                    GSW_QoS_FlowCtrlCfg_t *parm);

/**
   This is the switch API low-level function for
   the \ref GSW_QOS_FLOWCTRL_CFG_SET command.

   \param cdev This parameter is a pointer to the device context
   which contains all information related to this special
   instance of the device.
   \param parm Pointer to the parameters
   structure \ref GSW_QoS_FlowCtrlCfg_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
GSW_return_t GSW_QoS_FlowctrlCfgSet(void *cdev,
                                    GSW_QoS_FlowCtrlCfg_t *parm);

/**
   This is the switch API low-level function for
   the \ref GSW_QOS_FLOWCTRL_PORT_CFG_GET command.

   \param cdev This parameter is a pointer to the device context
   which contains all information related to this special
   instance of the device.
   \param parm Pointer to the parameters
   structure \ref GSW_QoS_FlowCtrlPortCfg_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
GSW_return_t GSW_QoS_FlowctrlPortCfgGet(void *cdev,
                                        GSW_QoS_FlowCtrlPortCfg_t *parm);

/**
   This is the switch API low-level function for
   the \ref GSW_QOS_FLOWCTRL_PORT_CFG_SET command.

   \param cdev This parameter is a pointer to the device context
   which contains all information related to this special
   instance of the device.
   \param parm Pointer to the parameters
   structure \ref GSW_QoS_FlowCtrlPortCfg_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
GSW_return_t GSW_QoS_FlowctrlPortCfgSet(void *cdev,
                                        GSW_QoS_FlowCtrlPortCfg_t *parm);
/**
   This is the switch API low-level function for
   the \ref GSW_QOS_METER_CFG_GET command.

   \param cdev This parameter is a pointer to the device context
   which contains all information related to this special
   instance of the device.
   \param parm Pointer to the parameters
   structure \ref GSW_QoS_meterCfg_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
GSW_return_t GSW_QoS_MeterCfgGet(void *cdev,
                                 GSW_QoS_meterCfg_t *parm);

/**
   This is the switch API low-level function for
   the \ref GSW_QOS_METER_CFG_SET command.

   \param cdev This parameter is a pointer to the device context
   which contains all information related to this special
   instance of the device.
   \param parm Pointer to the parameters
   structure \ref GSW_QoS_meterCfg_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
GSW_return_t GSW_QoS_MeterCfgSet(void *cdev,
                                 GSW_QoS_meterCfg_t *parm);

/**
   This is the switch API low-level function for
   the \ref GSW_QOS_METER_PORT_ASSIGN command.

   \param cdev This parameter is a pointer to the device context
   which contains all information related to this special
   instance of the device.
   \param parm Pointer to the parameters
   structure \ref GSW_QoS_meterPort_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
GSW_return_t GSW_QoS_MeterPortAssign(void *cdev,
                                     GSW_QoS_meterPort_t *parm);

/**
   This is the switch API low-level function for
   the \ref GSW_QOS_METER_PORT_DEASSIGN command.

   \param cdev This parameter is a pointer to the device context
   which contains all information related to this special
   instance of the device.
   \param parm Pointer to the parameters
   structure \ref GSW_QoS_meterPort_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
GSW_return_t GSW_QoS_MeterPortDeassign(void *cdev,
                                       GSW_QoS_meterPort_t *parm);

/**
   This is the switch API low-level function for
   the \ref GSW_QOS_METER_PORT_GET command.

   \param cdev This parameter is a pointer to the device context
   which contains all information related to this special
   instance of the device.
   \param parm Pointer to the parameters
   structure \ref GSW_QoS_meterPortGet_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
GSW_return_t GSW_QoS_MeterPortGet(void *cdev,
                                  GSW_QoS_meterPortGet_t *parm);

/**
   This is the switch API low-level function for
   the \ref GSW_QOS_PCP_CLASS_GET command.

   \param cdev This parameter is a pointer to the device context
   which contains all information related to this special
   instance of the device.
   \param parm Pointer to the QoS filter parameters
   \ref GSW_QoS_PCP_ClassCfg_t.

	\remarks The function returns an error code in case an error occurs.
		The error code is described in \ref GSW_status_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
GSW_return_t GSW_QoS_PCP_ClassGet(void *cdev,
                                  GSW_QoS_PCP_ClassCfg_t *parm);

/**
   This is the switch API low-level function for
   the \ref GSW_QOS_PCP_CLASS_SET command.

   \param cdev This parameter is a pointer to the device context
   which contains all information related to this special
   instance of the device.
   \param parm Pointer to the QoS filter parameters
   \ref GSW_QoS_PCP_ClassCfg_t.

	\remarks The function returns an error code in case an error occurs.
		The error code is described in \ref GSW_status_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
GSW_return_t GSW_QoS_PCP_ClassSet(void *cdev,
                                  GSW_QoS_PCP_ClassCfg_t *parm);

/**
   This is the switch API low-level function for
   the \ref GSW_QOS_PORT_CFG_GET command.

   \param cdev This parameter is a pointer to the device context
   which contains all information related to this special
   instance of the device.
   \param parm Pointer to a
      QOS port priority control configuration \ref GSW_QoS_portCfg_t.

	\remarks The function returns an error code in case an error occurs.
		The error code is described in \ref GSW_status_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
GSW_return_t GSW_QoS_PortCfgGet(void *cdev,
                                GSW_QoS_portCfg_t *parm);

/**
   This is the switch API low-level function for
   the \ref GSW_QOS_PORT_CFG_SET command.

   \param cdev This parameter is a pointer to the device context
   which contains all information related to this special
   instance of the device.
   \param parm Pointer to a
      QOS port priority control configuration \ref GSW_QoS_portCfg_t.

	\remarks The function returns an error code in case an error occurs.
		The error code is described in \ref GSW_status_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
GSW_return_t GSW_QoS_PortCfgSet(void *cdev,
                                GSW_QoS_portCfg_t *parm);

/**
   This is the switch API low-level function for
   the \ref GSW_QOS_PORT_REMARKING_CFG_GET command.

   \param cdev This parameter is a pointer to the device context
   which contains all information related to this special
   instance of the device.
   \param parm Pointer to the QoS filter parameters
   \ref GSW_QoS_portRemarkingCfg_t.

	\remarks The function returns an error code in case an error occurs.
		The error code is described in \ref GSW_status_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
GSW_return_t GSW_QoS_PortRemarkingCfgGet(void *cdev,
        GSW_QoS_portRemarkingCfg_t *parm);

/**
   This is the switch API low-level function for
   the \ref GSW_QOS_PORT_REMARKING_CFG_SET command.

   \param cdev This parameter is a pointer to the device context
   which contains all information related to this special
   instance of the device.
   \param parm Pointer to the remarking configuration
   \ref GSW_QoS_portRemarkingCfg_t.

	\remarks The function returns an error code in case an error occurs.
		The error code is described in \ref GSW_status_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
GSW_return_t GSW_QoS_PortRemarkingCfgSet(void *cdev,
        GSW_QoS_portRemarkingCfg_t *parm);

/**
   This is the switch API low-level function for
   the \ref GSW_QOS_QUEUE_BUFFER_RESERVE_CFG_GET command.

   \param cdev This parameter is a pointer to the device context
   which contains all information related to this special
   instance of the device.
   \param parm Pointer to the parameters
   structure \ref GSW_QoS_QueueBufferReserveCfg_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
GSW_return_t GSW_QoS_QueueBufferReserveCfgGet(void *cdev,
        GSW_QoS_QueueBufferReserveCfg_t *parm);

/**
   This is the switch API low-level function for
   the \ref GSW_QOS_QUEUE_BUFFER_RESERVE_CFG_SET command.

   \param cdev This parameter is a pointer to the device context
   which contains all information related to this special
   instance of the device.
   \param parm Pointer to the parameters
   structure \ref GSW_QoS_QueueBufferReserveCfg_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
GSW_return_t GSW_QoS_QueueBufferReserveCfgSet(void *cdev,
        GSW_QoS_QueueBufferReserveCfg_t *parm);
/**
   This is the switch API low-level function for
   the \ref GSW_QOS_QUEUE_PORT_GET command.

   \param cdev This parameter is a pointer to the device context
   which contains all information related to this special
   instance of the device.
   \param parm Pointer to the parameters
   structure \ref GSW_QoS_queuePort_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
GSW_return_t GSW_QoS_QueuePortGet(void *cdev,
                                  GSW_QoS_queuePort_t *parm);

/**
   This is the switch API low-level function for
   the \ref GSW_QOS_QUEUE_PORT_SET command.

   \param cdev This parameter is a pointer to the device context
   which contains all information related to this special
   instance of the device.
   \param parm Pointer to the parameters
   structure \ref GSW_QoS_queuePort_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
GSW_return_t GSW_QoS_QueuePortSet(void *cdev,
                                  GSW_QoS_queuePort_t *parm);

/**
   This is the switch API low-level function for
   the \ref GSW_QOS_SVLAN_CLASS_PCP_PORT_GET command.

   \param cdev This parameter is a pointer to the device context
   which contains all information related to this special
   instance of the device.
   \param parm Pointer to the PCP priority mapping parameter
   \ref GSW_QoS_SVLAN_ClassPCP_PortCfg_t.

	\remarks The function returns an error code in case an error occurs.
		The error code is described in \ref GSW_status_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
GSW_return_t GSW_QoS_SVLAN_ClassPCP_PortGet(void *cdev,
        GSW_QoS_SVLAN_ClassPCP_PortCfg_t *parm);

/**
   This is the switch API low-level function for
   the \ref GSW_QOS_SVLAN_CLASS_PCP_PORT_SET command.

   \param cdev This parameter is a pointer to the device context
   which contains all information related to this special
   instance of the device.
   \param parm Pointer to the PCP priority mapping parameter
   \ref GSW_QoS_SVLAN_ClassPCP_PortCfg_t.

	\remarks The function returns an error code in case an error occurs.
		The error code is described in \ref GSW_status_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
GSW_return_t GSW_QoS_SVLAN_ClassPCP_PortSet(void *cdev,
        GSW_QoS_SVLAN_ClassPCP_PortCfg_t *parm);

/**
   This is the switch API low-level function for
   the \ref GSW_QOS_SVLAN_PCP_CLASS_GET command.

   \param cdev This parameter is a pointer to the device context
   which contains all information related to this special
   instance of the device.
   \param parm Pointer to the QoS filter parameters
   \ref GSW_QoS_SVLAN_PCP_ClassCfg_t.

	\remarks The function returns an error code in case an error occurs.
		The error code is described in \ref GSW_status_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
GSW_return_t GSW_QoS_SVLAN_PCP_ClassGet(void *cdev,
                                        GSW_QoS_SVLAN_PCP_ClassCfg_t *parm);

/**
   This is the switch API low-level function for
   the \ref GSW_QOS_SVLAN_PCP_CLASS_SET command.

   \param cdev This parameter is a pointer to the device context
   which contains all information related to this special
   instance of the device.
   \param parm Pointer to the QoS filter parameters
   \ref GSW_QoS_SVLAN_PCP_ClassCfg_t.

	\remarks The function returns an error code in case an error occurs.
		The error code is described in \ref GSW_status_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
GSW_return_t GSW_QoS_SVLAN_PCP_ClassSet(void *cdev,
                                        GSW_QoS_SVLAN_PCP_ClassCfg_t *parm);
/**
   This is the switch API low-level function for
   the \ref GSW_QOS_SCHEDULER_CFG_GET command.

   \param cdev This parameter is a pointer to the device context
   which contains all information related to this special
   instance of the device.
   \param parm Pointer to the parameters
   structure \ref GSW_QoS_schedulerCfg_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
GSW_return_t GSW_QoS_SchedulerCfgGet(void *cdev,
                                     GSW_QoS_schedulerCfg_t *parm);

/**
   This is the switch API low-level function for
   the \ref GSW_QOS_SCHEDULER_CFG_SET command.

   \param cdev This parameter is a pointer to the device context
   which contains all information related to this special
   instance of the device.
   \param parm Pointer to the parameters
   structure \ref GSW_QoS_schedulerCfg_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
GSW_return_t GSW_QoS_SchedulerCfgSet(void *cdev,
                                     GSW_QoS_schedulerCfg_t *parm);

/**
   This is the switch API low-level function for
   the \ref GSW_QOS_SHAPER_CFG_GET command.

   \param cdev This parameter is a pointer to the device context
   which contains all information related to this special
   instance of the device.
   \param parm Pointer to the parameters
   structure \ref GSW_QoS_ShaperCfg_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
GSW_return_t GSW_QoS_ShaperCfgGet(void *cdev,
                                  GSW_QoS_ShaperCfg_t *parm);

/**
   This is the switch API low-level function for
   the \ref GSW_QOS_SHAPER_CFG_SET command.

   \param cdev This parameter is a pointer to the device context
   which contains all information related to this special
   instance of the device.
   \param parm Pointer to the parameters
   structure \ref GSW_QoS_ShaperCfg_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
GSW_return_t GSW_QoS_ShaperCfgSet(void *cdev,
                                  GSW_QoS_ShaperCfg_t *parm);

/**
   This is the switch API low-level function for
   the \ref GSW_QOS_SHAPER_QUEUE_ASSIGN command.

   \param cdev This parameter is a pointer to the device context
   which contains all information related to this special
   instance of the device.
   \param parm Pointer to the parameters
   structure \ref GSW_QoS_ShaperQueue_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
GSW_return_t GSW_QoS_ShaperQueueAssign(void *cdev,
                                       GSW_QoS_ShaperQueue_t *parm);

/**
   This is the switch API low-level function for
   the \ref GSW_QOS_SHAPER_QUEUE_DEASSIGN command.

   \param cdev This parameter is a pointer to the device context
   which contains all information related to this special
   instance of the device.
   \param parm Pointer to the parameters
   structure \ref GSW_QoS_ShaperQueue_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
GSW_return_t GSW_QoS_ShaperQueueDeassign(void *cdev,
        GSW_QoS_ShaperQueue_t *parm);

/**
   This is the switch API low-level function for
   the \ref GSW_QOS_SHAPER_QUEUE_GET command.

   \param cdev This parameter is a pointer to the device context
   which contains all information related to this special
   instance of the device.
   \param parm Pointer to the parameters
   structure \ref GSW_QoS_ShaperQueueGet_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
GSW_return_t GSW_QoS_ShaperQueueGet(void *cdev,
                                    GSW_QoS_ShaperQueueGet_t *parm);

/**
   This is the switch API low-level function for
   the \ref GSW_QOS_STORM_CFG_GET command.

   \param cdev This parameter is a pointer to the device context
   which contains all information related to this special
   instance of the device.
   \param parm Pointer to the parameters
   structure \ref GSW_QoS_stormCfg_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
GSW_return_t GSW_QoS_StormCfgGet(void *cdev, GSW_QoS_stormCfg_t *parm);

/**
   This is the switch API low-level function for
   the \ref GSW_QOS_STORM_CFG_SET command.

   \param cdev This parameter is a pointer to the device context
   which contains all information related to this special
   instance of the device.
   \param parm Pointer to the parameters
   structure \ref GSW_QoS_stormCfg_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
GSW_return_t GSW_QoS_StormCfgSet(void *cdev, GSW_QoS_stormCfg_t *parm);

/**
   This is the switch API low-level function for
   the \ref GSW_QOS_WRED_CFG_GET command.

   \param cdev This parameter is a pointer to the device context
   which contains all information related to this special
   instance of the device.
   \param parm Pointer to the parameters
   structure \ref GSW_QoS_WRED_Cfg_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
GSW_return_t GSW_QoS_WredCfgGet(void *cdev, GSW_QoS_WRED_Cfg_t *parm);

/**
   This is the switch API low-level function for
   the \ref GSW_QOS_WRED_CFG_SET command.

   \param cdev This parameter is a pointer to the device context
   which contains all information related to this special
   instance of the device.
   \param parm Pointer to the parameters
   structure \ref GSW_QoS_WRED_Cfg_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
GSW_return_t GSW_QoS_WredCfgSet(void *cdev, GSW_QoS_WRED_Cfg_t *parm);

/**
   This is the switch API low-level function for
   the \ref GSW_QOS_WRED_PORT_CFG_GET command.

   \param cdev This parameter is a pointer to the device context
   which contains all information related to this special
   instance of the device.
   \param parm Pointer to the parameters
   structure \ref GSW_QoS_WRED_PortCfg_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
GSW_return_t GSW_QoS_WredPortCfgGet(void *cdev,
                                    GSW_QoS_WRED_PortCfg_t *parm);

/**
   This is the switch API low-level function for
   the \ref GSW_QOS_WRED_PORT_CFG_SET command.

   \param cdev This parameter is a pointer to the device context
   which contains all information related to this special
   instance of the device.
   \param parm Pointer to the parameters
   structure \ref GSW_QoS_WRED_PortCfg_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
GSW_return_t GSW_QoS_WredPortCfgSet(void *cdev,
                                    GSW_QoS_WRED_PortCfg_t *parm);
/**
   This is the switch API low-level function for
   the \ref GSW_QOS_WRED_QUEUE_CFG_GET command.

   \param cdev This parameter is a pointer to the device context
   which contains all information related to this special
   instance of the device.
   \param parm Pointer to the parameters
   structure \ref GSW_QoS_WRED_QueueCfg_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
GSW_return_t GSW_QoS_WredQueueCfgGet(void *cdev,
                                     GSW_QoS_WRED_QueueCfg_t *parm);

/**
   This is the switch API low-level function for
   the \ref GSW_QOS_WRED_QUEUE_CFG_SET command.

   \param cdev This parameter is a pointer to the device context
   which contains all information related to this special
   instance of the device.
   \param parm Pointer to the parameters
   structure \ref GSW_QoS_WRED_QueueCfg_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
GSW_return_t GSW_QoS_WredQueueCfgSet(void *cdev,
                                     GSW_QoS_WRED_QueueCfg_t *parm);

/*@}*/ /* FLOW_LL_QOS */
/** \addtogroup FLOW_LL_MULTICAST */
/*@{*/
/**
   This is the switch API low-level function for
   the \ref GSW_MULTICAST_ROUTER_PORT_ADD command.

   \param cdev This parameter is a pointer to the device context
   which contains all information related to this special
   instance of the device.
   \param parm Pointer to \ref GSW_multicastRouter_t.

	\remarks The function returns an error code in case an error occurs.
		The error code is described in \ref GSW_status_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
GSW_return_t GSW_MulticastRouterPortAdd(void *cdev,
                                        GSW_multicastRouter_t *parm);

/**
   This is the switch API low-level function for
   the \ref GSW_MULTICAST_ROUTER_PORT_READ command.

   \param cdev This parameter is a pointer to the device context
   which contains all information related to this special
   instance of the device.
   \param parm Pointer to \ref GSW_multicastRouterRead_t.

	\remarks The function returns an error code in case an error occurs.
		The error code is described in \ref GSW_status_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
   (e.g. Ethernet port parameter out of range)
*/
GSW_return_t GSW_MulticastRouterPortRead(void *cdev,
        GSW_multicastRouterRead_t *parm);

/**
   This is the switch API low-level function for
   the \ref GSW_MULTICAST_ROUTER_PORT_REMOVE command.

   \param cdev This parameter is a pointer to the device context
   which contains all information related to this special
   instance of the device.
   \param parm Pointer to \ref GSW_multicastRouter_t.

	\remarks The function returns an error code in case an error occurs.
		The error code is described in \ref GSW_status_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
   (e.g. Ethernet port parameter out of range)
*/
GSW_return_t GSW_MulticastRouterPortRemove(void *cdev,
        GSW_multicastRouter_t *parm);

/**
   This is the switch API low-level function for
   the \ref GSW_MULTICAST_SNOOP_CFG_GET command.

   \param cdev This parameter is a pointer to the device context
   which contains all information related to this special
   instance of the device.
   \param parm Pointer to the
   multicast configuration \ref GSW_multicastSnoopCfg_t.

   \remarks IGMP/MLD snooping is disabled when
   'eIGMP_Mode = GSW_MULTICAST_SNOOP_MODE_SNOOPFORWARD'.
   Then all other structure parameters are unused.

	\remarks The function returns an error code in case an error occurs.
		The error code is described in \ref GSW_status_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
GSW_return_t GSW_MulticastSnoopCfgGet(void *cdev,
                                      GSW_multicastSnoopCfg_t *parm);

/**
   This is the switch API low-level function for
   the \ref GSW_MULTICAST_SNOOP_CFG_SET command.

   \param cdev This parameter is a pointer to the device context
   which contains all information related to this special
   instance of the device.
   \param parm Pointer to the
   multicast configuration \ref GSW_multicastSnoopCfg_t.

   \remarks IGMP/MLD snooping is disabled when
   'eIGMP_Mode = GSW_MULTICAST_SNOOP_MODE_SNOOPFORWARD'.
   Then all other structure parameters are unused.

	\remarks The function returns an error code in case an error occurs.
		The error code is described in \ref GSW_status_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
GSW_return_t GSW_MulticastSnoopCfgSet(void *cdev,
                                      GSW_multicastSnoopCfg_t *parm);

/**
   This is the switch API low-level function for
   the \ref GSW_MULTICAST_TABLE_ENTRY_ADD command.

   \param cdev This parameter is a pointer to the device context
   which contains all information related to this special
   instance of the device.
   \param parm Pointer
      to \ref GSW_multicastTable_t.

	\remarks The Source IP parameter is ignored in case IGMPv3 support is
		not enabled in the hardware.

	\remarks The function returns an error code in case an error occurs.
		The error code is described in \ref GSW_status_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
GSW_return_t GSW_MulticastTableEntryAdd(void *cdev,
                                        GSW_multicastTable_t *parm);

/**
   This is the switch API low-level function for
   the \ref GSW_MULTICAST_TABLE_ENTRY_READ command.

   \param cdev This parameter is a pointer to the device context
   which contains all information related to this special
   instance of the device.
   \param parm Pointer
      to \ref GSW_multicastTableRead_t.

	\remarks The 'bInitial' parameter is reset during the read operation.

	\remarks The function returns an error code in case an error occurs.
		The error code is described in \ref GSW_status_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
GSW_return_t GSW_MulticastTableEntryRead(void *cdev,
        GSW_multicastTableRead_t *parm);

/**
   This is the switch API low-level function for
   the \ref GSW_MULTICAST_TABLE_ENTRY_REMOVE command.

   \param cdev This parameter is a pointer to the device context
   which contains all information related to this special
   instance of the device.
   \param parm Pointer
      to \ref GSW_multicastTable_t.

	\remarks The Source IP parameter is ignored in case
	IGMPv3 support is not enabled in the hardware.

	\remarks The function returns an error code in case an error occurs.
		The error code is described in \ref GSW_status_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
GSW_return_t GSW_MulticastTableEntryRemove(void *cdev,
        GSW_multicastTable_t *parm);

/*@}*/ /* FLOW_LL_MULTICAST */
/** \addtogroup FLOW_LL_OAM */
/*@{*/
/**
   This is the switch API low-level function for
   the \ref GSW_CPU_PORT_CFG_GET command.

   \param cdev This parameter is a pointer to the device context
   which contains all information related to this specialinstance of the device.
   \param parm Pointer to
      an \ref GSW_CPU_PortCfg_t structure.

	\remarks The function returns an error code in case an error occurs.
		The error code is described in \ref GSW_status_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
GSW_return_t GSW_CPU_PortCfgGet(void *cdev, GSW_CPU_PortCfg_t *parm);

/**
   This is the switch API low-level function for
   the \ref GSW_CPU_PORT_CFG_SET command.

   \param cdev This parameter is a pointer to the device context
   which contains all information related to this special
   instance of the device.
   \param parm Pointer to
      an \ref GSW_CPU_PortCfg_t structure.

	\remarks The function returns an error code in case an error occurs.
		The error code is described in \ref GSW_status_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
GSW_return_t GSW_CPU_PortCfgSet(void *cdev, GSW_CPU_PortCfg_t *parm);

/**
   This is the switch API low-level function for
   the \ref GSW_CPU_PORT_EXTEND_CFG_GET command.

   \param cdev This parameter is a pointer to the device context
   which contains all information related to this special
   instance of the device.
   \param parm Pointer to
      an \ref GSW_CPU_PortExtendCfg_t structure.

	\remarks The function returns an error code in case an error occurs.
		The error code is described in \ref GSW_status_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs

*/
GSW_return_t GSW_CPU_PortExtendCfgGet(void *cdev,
                                      GSW_CPU_PortExtendCfg_t *parm);

/**
   This is the switch API low-level function for
   the \ref GSW_CPU_PORT_EXTEND_CFG_SET command.

   \param cdev This parameter is a pointer to the device context
   which contains all information related to this special
   instance of the device.
   \param parm Pointer to
      an \ref GSW_CPU_PortExtendCfg_t structure.

	\remarks The function returns an error code in case an error occurs.
		The error code is described in \ref GSW_status_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
GSW_return_t GSW_CPU_PortExtendCfgSet(void *cdev,
                                      GSW_CPU_PortExtendCfg_t *parm);

/**
   This is the switch API low-level function for
   the \ref GSW_CAP_GET command.

   \param cdev This parameter is a pointer to the device context
   which contains all information related to this special
   instance of the device.
   \param parm Pointer to pre-allocated capability
      list structure \ref GSW_cap_t.
      The switch API implementation fills out the structure with the supported
      features, based on the provided 'nCapType' parameter.

	\remarks The function returns an error code in case an error occurs.
		The error code is described in \ref GSW_status_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs

*/
GSW_return_t GSW_CapGet(void *cdev, GSW_cap_t *parm);

/**
   This is the switch API low-level function for
   the \ref GSW_CFG_GET command.

   \param cdev This parameter is a pointer to the device context
   which contains all information related to this special
   instance of the device.
   \param parm Pointer to an \ref GSW_cfg_t structure.
      The structure is filled out by the switch implementation.

	\remarks The function returns an error code in case an error occurs.
		The error code is described in \ref GSW_status_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
GSW_return_t GSW_CfgGet(void *cdev, GSW_cfg_t *parm);

/**
   This is the switch API low-level function for
   the \ref GSW_CFG_SET command.

   \param cdev This parameter is a pointer to the device context
   which contains all information related to this special
   instance of the device.
   \param parm Pointer to an \ref GSW_cfg_t structure.

	\remarks The function returns an error code in case an error occurs.
		The error code is described in \ref GSW_status_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
GSW_return_t GSW_CfgSet(void *cdev, GSW_cfg_t *parm);

/**
   This is the switch API low-level function for
   the \ref GSW_DISABLE command.

   \param cdev This parameter is a pointer to the device context
   which contains all information related to this special
   instance of the device.

	\remarks The function returns an error code in case an error occurs.
		The error code is described in \ref GSW_status_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
GSW_return_t GSW_Disable(void *cdev);

/**
   This is the switch API low-level function for
   the \ref GSW_ENABLE command.

   \param cdev This parameter is a pointer to the device context
   which contains all information related to this special
   instance of the device.

	\remarks The function returns an error code in case an error occurs.
		The error code is described in \ref GSW_status_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
GSW_return_t GSW_Enable(void *cdev);

/**
   This is the switch API low-level function for
   the \ref GSW_HW_INIT command.

   \param cdev This parameter is a pointer to the device context
   which contains all information related to this special
   instance of the device.
   \param parm Pointer to pre-allocated initialization structure
   \ref GSW_HW_Init_t.

	\remarks The function returns an error code in case an error occurs.
		The error code is described in \ref GSW_status_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
GSW_return_t GSW_HW_Init(void *cdev, GSW_HW_Init_t *parm);

/**
   This is the switch API low-level function for
   the \ref GSW_MDIO_CFG_GET command.

   \param cdev This parameter is a pointer to the device context
   which contains all information related to this special
   instance of the device.
   \param parm Pointer to \ref GSW_MDIO_cfg_t.

	\remarks The function returns an error code in case an error occurs.
		The error code is described in \ref GSW_status_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
GSW_return_t GSW_MDIO_CfgGet(void *cdev, GSW_MDIO_cfg_t *parm);

/**
   This is the switch API low-level function for
   the \ref GSW_MDIO_CFG_SET command.

   \param cdev This parameter is a pointer to the device context
   which contains all information related to this special
   instance of the device.
   \param parm Pointer to \ref GSW_MDIO_cfg_t.

	\remarks The function returns an error code in case an error occurs.
		The error code is described in \ref GSW_status_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
GSW_return_t GSW_MDIO_CfgSet(void *cdev, GSW_MDIO_cfg_t *parm);

/**
   This is the switch API low-level function for
   the \ref GSW_MDIO_DATA_READ command.

   \param cdev This parameter is a pointer to the device context
   which contains all information related to this special
   instance of the device.
   \param parm Pointer to \ref GSW_MDIO_data_t.

	\remarks The function returns an error code in case an error occurs.
		The error code is described in \ref GSW_status_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
GSW_return_t GSW_MDIO_DataRead(void *cdev, GSW_MDIO_data_t *parm);

/**
   This is the switch API low-level function for
   the \ref GSW_MDIO_DATA_WRITE command.

   \param cdev This parameter is a pointer to the device context
   which contains all information related to this special
   instance of the device.
   \param parm Pointer to \ref GSW_MDIO_data_t.

	\remarks The function returns an error code in case an error occurs.
			The error code is described in \ref GSW_status_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs

*/
GSW_return_t GSW_MDIO_DataWrite(void *cdev, GSW_MDIO_data_t *parm);

/**
   This is the switch API low-level function for
   the \ref GSW_MMD_DATA_READ command.

   \param cdev This parameter is a pointer to the device context
   which contains all information related to this special
   instance of the device.
   \param parm Pointer to \ref GSW_MMD_data_t.

	\remarks The function returns an error code in case an error occurs.
			The error code is described in \ref GSW_status_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
GSW_return_t GSW_MmdDataRead(void *cdev, GSW_MMD_data_t *parm);

/**
   This is the switch API low-level function for
   the \ref GSW_MMD_DATA_WRITE command.

   \param cdev This parameter is a pointer to the device context
   which contains all information related to this special
   instance of the device.
   \param parm Pointer to \ref GSW_MMD_data_t.

	\remarks The function returns an error code in case an error occurs.
			The error code is described in \ref GSW_status_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
GSW_return_t GSW_MmdDataWrite(void *cdev, GSW_MMD_data_t *parm);

/**
   This is the switch API low-level function for
   the \ref GSW_MONITOR_PORT_CFG_GET command.

   \param cdev This parameter is a pointer to the device context
   which contains all information related to this special
   instance of the device.
   \param parm Pointer to \ref GSW_monitorPortCfg_t.

	\remarks The function returns an error code in case an error occurs.
		The error code is described in \ref GSW_status_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
GSW_return_t GSW_MonitorPortCfgGet(void *cdev, GSW_monitorPortCfg_t *parm);

/**
   This is the switch API low-level function for
   the \ref GSW_MONITOR_PORT_CFG_SET command.

   \param cdev This parameter is a pointer to the device context
   which contains all information related to this special
   instance of the device.
   \param parm Pointer to \ref GSW_monitorPortCfg_t.

	\remarks The function returns an error code in case an error occurs.
		The error code is described in \ref GSW_status_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
GSW_return_t GSW_MonitorPortCfgSet(void *cdev, GSW_monitorPortCfg_t *parm);

/**
   This is the switch API low-level function for
   the \ref GSW_PORT_CFG_GET command.

   \param cdev This parameter is a pointer to the device context
   which contains all information related to this special
   instance of the device.
   \param parm Pointer to a port configuration
   \ref GSW_portCfg_t structure to fill out by the driver.
   The parameter 'nPortId' tells the driver which port parameter is requested.

	\remarks The function returns an error code in case an error occurs.
		The error code is described in \ref GSW_status_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
GSW_return_t GSW_PortCfgGet(void *cdev, GSW_portCfg_t *parm);

/**
   This is the switch API low-level function for
   the \ref GSW_PORT_CFG_SET command.

   \param cdev This parameter is a pointer to the device context
   which contains all information related to this special
   instance of the device.
   \param parm Pointer to an \ref GSW_portCfg_t structure
   to configure the switch port hardware.

	\remarks The function returns an error code in case an error occurs.
		The error code is described in \ref GSW_status_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
GSW_return_t GSW_PortCfgSet(void *cdev, GSW_portCfg_t *parm);

/**
   This is the switch API low-level function for
   the \ref GSW_PORT_LINK_CFG_GET command.

   \param cdev This parameter is a pointer to the device context
   which contains all information related to this special
   instance of the device.
   \param parm Pointer to
      an \ref GSW_portLinkCfg_t structure to read out the port status.

	\remarks The function returns an error code in case an error occurs.
		The error code is described in \ref GSW_status_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
GSW_return_t GSW_PortLinkCfgGet(void *cdev, GSW_portLinkCfg_t *parm);

/**
   This is the switch API low-level function for
   the \ref GSW_PORT_LINK_CFG_SET command.

   \param cdev This parameter is a pointer to the device context
   which contains all information related to this special
   instance of the device.
   \param parm Pointer to
      an \ref GSW_portLinkCfg_t structure to set the port configuration.

	\remarks The function returns an error code in case an error occurs.
		The error code is described in \ref GSW_status_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
GSW_return_t GSW_PortLinkCfgSet(void *cdev, GSW_portLinkCfg_t *parm);

/**
   This is the switch API low-level function for
   the \ref GSW_PORT_PHY_ADDR_GET command.

   \param cdev This parameter is a pointer to the device context
   which contains all information related to this special
   instance of the device.
   \param parm Pointer to \ref GSW_portPHY_Addr_t.

	\remarks The function returns an error code in case an error occurs.
		The error code is described in \ref GSW_status_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
GSW_return_t GSW_PortPHY_AddrGet(void *cdev, GSW_portPHY_Addr_t *parm);

/**
   This is the switch API low-level function for
   the \ref GSW_PORT_PHY_QUERY command.

   \param cdev This parameter is a pointer to the device context
   which contains all information related to this special
   instance of the device.
   \param parm Pointer to
      an \ref GSW_portPHY_Query_t structure to set the port configuration.

	\remarks The function returns an error code in case an error occurs.
		The error code is described in \ref GSW_status_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
GSW_return_t GSW_PortPHY_Query(void *cdev, GSW_portPHY_Query_t *parm);

/**
   This is the switch API low-level function for
   the \ref GSW_PORT_RGMII_CLK_CFG_GET command.

   \param cdev This parameter is a pointer to the device context
   which contains all information related to this special
   instance of the device.
   \param parm Pointer to
      an \ref GSW_portRGMII_ClkCfg_t structure to set the port configuration.

	\remarks The function returns an error code in case an error occurs.
		The error code is described in \ref GSW_status_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
GSW_return_t GSW_PortRGMII_ClkCfgGet(void *cdev, GSW_portRGMII_ClkCfg_t *parm);

/**
   This is the switch API low-level function for
   the \ref GSW_PORT_RGMII_CLK_CFG_SET command.

   \param cdev This parameter is a pointer to the device context
   which contains all information related to this special
   instance of the device.
   \param parm Pointer to
      an \ref GSW_portRGMII_ClkCfg_t structure to set the port configuration.

	\remarks The function returns an error code in case an error occurs.
		The error code is described in \ref GSW_status_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
GSW_return_t GSW_PortRGMII_ClkCfgSet(void *cdev, GSW_portRGMII_ClkCfg_t *parm);

/**
   This is the switch API low-level function for
   the \ref GSW_PORT_REDIRECT_GET command.

   \param cdev This parameter is a pointer to the device context
   which contains all information related to this special
   instance of the device.
   \param parm Pointer to \ref GSW_portRedirectCfg_t.

	\remarks The function returns an error code in case an error occurs.
		The error code is described in \ref GSW_status_t.
	\remarks Not all hardware platforms support this feature. The function
		returns an error if this feature is not supported.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
GSW_return_t GSW_PortRedirectGet(void *cdev, GSW_portRedirectCfg_t *parm);

/**
   This is the switch API low-level function for
   the \ref GSW_PORT_REDIRECT_SET command.

   \param cdev This parameter is a pointer to the device context
   which contains all information related to this special
   instance of the device.
   \param parm Pointer to \ref GSW_portRedirectCfg_t.

	\remarks The function returns an error code in case an error occurs.
		The error code is described in \ref GSW_status_t.
	\remarks Not all hardware platforms support this feature. The function
		returns an error if this feature is not supported.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
GSW_return_t GSW_PortRedirectSet(void *cdev, GSW_portRedirectCfg_t *parm);

/**
   This is the switch API low-level function for
   the \ref GSW_RMON_CLEAR command.

   \param cdev This parameter is a pointer to the device context
   which contains all information related to this special
   instance of the device.
   \param parm  Pointer to a pre-allocated
   \ref GSW_RMON_clear_t structure. The structure element 'nPortId' is
   an input parameter stating on which port to clear all RMON counters.

   \remarks The function returns an error in case the given 'nPortId' is
   out of range.

	\remarks The function returns an error code in case an error occurs.
		The error code is described in \ref GSW_status_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
GSW_return_t GSW_RMON_Clear(void *cdev, GSW_RMON_clear_t *parm);

/**
   This is the switch API low-level function for
   the \ref GSW_RMON_PORT_GET command.

   \param cdev This parameter is a pointer to the device context
   which contains all information related to this special
   instance of the device.
   \param parm  Pointer to pre-allocated
   \ref GSW_RMON_Port_cnt_t structure. The structure element 'nPortId' is
   an input parameter that describes from which port to read the RMON counter.
   All remaining structure elements are filled with the counter values.

   \remarks The function returns an error in case the given 'nPortId' is
   out of range.

	\remarks The function returns an error code in case an error occurs.
		The error code is described in \ref GSW_status_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
GSW_return_t GSW_RMON_Port_Get(void *cdev, GSW_RMON_Port_cnt_t *parm);

/**
   This is the switch API low-level function for
   the \ref GSW_VERSION_GET command.

   \param cdev This parameter is a pointer to the device context
   which contains all information related to this special
   instance of the device.
   \param parm* The parameter points to a
   \ref GSW_version_t structure.

   \return Returns value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs

*/
GSW_return_t GSW_VersionGet(void *cdev, GSW_version_t *parm);

/**
   This is the switch API low-level function for
   the \ref GSW_WOL_CFG_GET command.

   \param cdev This parameter is a pointer to the device context
   which contains all information related to this special
   instance of the device.
   \param parm Pointer to \ref GSW_WoL_Cfg_t.

   \remarks The function returns an error code in case an error occurs.
		The error code is described in \ref GSW_status_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
GSW_return_t GSW_WoL_CfgGet(void *cdev, GSW_WoL_Cfg_t *parm);

/**
   This is the switch API low-level function for
   the \ref GSW_WOL_CFG_SET command.

   \param cdev This parameter is a pointer to the device context
   which contains all information related to this special
   instance of the device.
   \param parm Pointer to \ref GSW_WoL_Cfg_t.

   \remarks The function returns an error code in case an error occurs.
		The error code is described in \ref GSW_status_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
GSW_return_t GSW_WoL_CfgSet(void *cdev, GSW_WoL_Cfg_t *parm);

/**
   This is the switch API low-level function for
   the \ref GSW_WOL_PORT_CFG_GET command.

   \param cdev This parameter is a pointer to the device context
   which contains all information related to this special
   instance of the device.
   \param parm Pointer to \ref GSW_WoL_PortCfg_t.

   \remarks The function returns an error code in case an error occurs.
		The error code is described in \ref GSW_status_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
GSW_return_t GSW_WoL_PortCfgGet(void *cdev, GSW_WoL_PortCfg_t *parm);

/**
   This is the switch API low-level function for
   the \ref GSW_WOL_PORT_CFG_SET command.

   \param cdev This parameter is a pointer to the device context
   which contains all information related to this special
   instance of the device.
   \param parm Pointer to \ref GSW_WoL_PortCfg_t.

	\remarks The function returns an error code in case an error occurs.
		The error code is described in \ref GSW_status_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
GSW_return_t GSW_WoL_PortCfgSet(void *cdev, GSW_WoL_PortCfg_t *parm);

/*@}*/ /* FLOW_LL_OAM */
/** \addtogroup FLOW_LL_DEBUG */
/*@{*/
/**
   This is the switch API low-level function for
   the \ref GSW_REGISTER_GET command.

   \param cdev This parameter is a pointer to the device context
   which contains all information related to this special
   instance of the device.
   \param parm Pointer to \ref GSW_register_t.

	\remarks The function returns an error code in case an error occurs.
		The error code is described in \ref GSW_status_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
GSW_return_t GSW_RegisterGet(void *cdev, GSW_register_t *parm);

/**
   This is the switch API low-level function for
   the \ref GSW_REGISTER_SET command.

   \param cdev This parameter is a pointer to the device context
   which contains all information related to this special
   instance of the device.
   \param parm Pointer to \ref GSW_register_t.

	\remarks The function returns an error code in case an error occurs.
		The error code is described in \ref GSW_status_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
GSW_return_t GSW_RegisterSet(void *cdev, GSW_register_t *parm);

/*@}*/ /* FLOW_LL_DEBUG */
/** \addtogroup FLOW_LL_IRQ */
/*@{*/
/**
   This is the switch API low-level function for
   the \ref GSW_IRQ_GET command.

   \param cdev This parameter is a pointer to the device context
   which contains all information related to this special
   instance of the device.
   \param parm Pointer to
      an \ref GSW_irq_t structure.

	\remarks The function returns an error code in case an error occurs.
		The error code is described in \ref GSW_status_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs

*/
GSW_return_t GSW_IrqGet(void *cdev, GSW_irq_t *parm);

/**
   This is the switch API low-level function for
   the \ref GSW_IRQ_MASK_GET command.

   \param cdev This parameter is a pointer to the device context
   which contains all information related to this special
   instance of the device.
   \param parm Pointer to
      an \ref GSW_irq_t structure.

	\remarks The function returns an error code in case an error occurs.
		The error code is described in \ref GSW_status_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs

*/
GSW_return_t GSW_IrqMaskGet(void *cdev, GSW_irq_t *parm);

/**
   This is the switch API low-level function for
   the \ref GSW_IRQ_MASK_SET command.

   \param cdev This parameter is a pointer to the device context
   which contains all information related to this special
   instance of the device.
   \param parm Pointer to
      an \ref GSW_irq_t structure.

	\remarks The function returns an error code in case an error occurs.
		The error code is described in \ref GSW_status_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs

*/
GSW_return_t GSW_IrqMaskSet(void *cdev, GSW_irq_t *parm);

/**
   This is the switch API low-level function for
   the \ref GSW_IRQ_STATUS_CLEAR command.

   \param cdev This parameter is a pointer to the device context
   which contains all information related to this special
   instance of the device.
   \param parm Pointer to
      an \ref GSW_irq_t structure.

	\remarks The function returns an error code in case an error occurs.
		The error code is described in \ref GSW_status_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs

*/
GSW_return_t GSW_IrqStatusClear(void *cdev, GSW_irq_t *parm);

/*@}*/ /* FLOW_LL_IRQ */
/** \addtogroup FLOW_LL_CLASSUNIT */
/*@{*/
/**
   This is the switch API low-level function for
   the \ref GSW_PCE_RULE_DELETE command.

   \param cdev This parameter is a pointer to the device context
   which contains all information related to this special
   instance of the device.
   \param parm Pointer to \ref GSW_PCE_ruleDelete_t.

	\remarks The function returns an error code in case an error occurs.
		The error code is described in \ref GSW_status_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
GSW_return_t GSW_PceRuleDelete(void *cdev, GSW_PCE_ruleDelete_t *parm);

/**
   This is the switch API low-level function for
   the \ref GSW_PCE_RULE_READ command.

   \param cdev This parameter is a pointer to the device context
   which contains all information related to this special
   instance of the device.
   \param parm Pointer to \ref GSW_PCE_rule_t.

	\remarks The function returns an error code in case an error occurs.
		The error code is described in \ref GSW_status_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
GSW_return_t GSW_PceRuleRead(void *cdev, GSW_PCE_rule_t *parm);

/**
   This is the switch API low-level function for
   the \ref GSW_PCE_RULE_WRITE command.

   \param cdev This parameter is a pointer to the device context
   which contains all information related to this special
   instance of the device.
   \param parm Pointer to \ref GSW_PCE_rule_t.

	\remarks The function returns an error code in case an error occurs.
		The error code is described in \ref GSW_status_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
GSW_return_t GSW_PceRuleWrite(void *cdev, GSW_PCE_rule_t *parm);

/*@}*/ /* FLOW_LL_CLASSUNIT */
/** \addtogroup FLOW_LL_OAM */
/*@{*/
/**
   This is the switch API low-level function for
   the \ref GSW_RMON_EXTEND_GET command.

   \param cdev This parameter is a pointer to the device context
   which contains all information related to this special
   instance of the device.
   \param parm  Pointer to a pre-allocated
   \ref GSW_RMON_extendGet_t structure. The structure element 'nPortId' is
   an input parameter that describes from which port to read the RMON counter.
   All remaining structure elements are filled with the counter values.
   The counter assignment needs to be done during the flow definition,
   for example in \ref GSW_PCE_RULE_WRITE.

	\remarks The function returns an error in case the given 'nPortId' is
		out of range.

	\remarks The function returns an error code in case an error occurs.
		The error code is described in \ref GSW_status_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
GSW_return_t GSW_RMON_ExtendGet(void *cdev, GSW_RMON_extendGet_t *parm);

/**
   This is the switch API low-level function for
   the \ref GSW_RESET command.

   \param cdev This parameter is a pointer to the device context
   which contains all information related to this special
   instance of the device.
   \param parm Pointer to an \ref GSW_reset_t structure.

	\remarks The function returns an error code in case an error occurs.
		The error code is described in \ref GSW_status_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs

   \remarks Not supported for all devices
*/
GSW_return_t GSW_Reset(void *cdev, GSW_reset_t *parm);

/**
   This is the switch API low-level function for
   the \ref GSW_TIMESTAMP_PORT_READ command.

   \param cdev This parameter is a pointer to the device context
   which contains all information related to this special
   instance of the device.
   \param parm Pointer to
      an \ref GSW_TIMESTAMP_PortRead_t structure.

	\remarks The function returns an error code in case an error occurs.
		The error code is described in \ref GSW_status_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
GSW_return_t GSW_TimestampPortRead(void *cdev, GSW_TIMESTAMP_PortRead_t *parm);

/**
   This is the switch API low-level function for
   the \ref GSW_TIMESTAMP_TIMER_GET command.

   \param cdev This parameter is a pointer to the device context
   which contains all information related to this special
   instance of the device.
   \param parm Pointer to an \ref GSW_TIMESTAMP_Timer_t structure.

	\remarks The function returns an error code in case an error occurs.
		The error code is described in \ref GSW_status_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
GSW_return_t GSW_TimestampTimerGet(void *cdev, GSW_TIMESTAMP_Timer_t *parm);

/**
   This is the switch API low-level function for
   the \ref GSW_TIMESTAMP_TIMER_SET command.

   \param cdev This parameter is a pointer to the device context
   which contains all information related to this special
   instance of the device.
   \param parm Pointer to an \ref GSW_TIMESTAMP_Timer_t structure.

	\remarks The function returns an error code in case an error occurs.
		The error code is described in \ref GSW_status_t.

   \return Return value as follows:
   - GSW_statusOk: if successful
   - An error code in case an error occurs
*/
GSW_return_t GSW_TimestampTimerSet(void *cdev, GSW_TIMESTAMP_Timer_t *parm);
/*@}*/ /* FLOW_LL_OAM */



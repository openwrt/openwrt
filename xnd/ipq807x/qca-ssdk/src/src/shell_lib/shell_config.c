/*
 * Copyright (c) 2013, 2015-2019, 2021, The Linux Foundation. All rights reserved.
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include "shell_config.h"
#include "shell_sw.h"

#if 0
/*cmdline tree descript*/
struct cmd_des_t gcmd_des[] =
{
    /*port ctrl*/
#ifdef IN_PORTCONTROL
    {
        "port", "config port control",
        {
            {"duplex", "set", "set duplex mode of a port", "<port_id> <half|full>", SW_API_PT_DUPLEX_SET, NULL},
            {"speed", "set", "set speed mode of a port", "<port_id> <10|100|1000>", SW_API_PT_SPEED_SET, NULL},
            {"autoAdv", "set", "set auto-negotiation advertisement of a port", "<port_id> <cap_bitmap>", SW_API_PT_AN_ADV_SET, NULL},
            {"autoNeg", "enable", "enable auto-negotiation of a port", "<port_id>", SW_API_PT_AN_ENABLE, NULL},
            {"autoNeg", "restart", "restart auto-negotiation process of a port", "<port_id>", SW_API_PT_AN_RESTART, NULL},
            {"autoNegenable", "set", "enable auto-negotiation of a port", "<port_id>", SW_API_PT_AN_ENABLE, NULL},
            {"autoNegrestart", "set", "restart auto-negotiation process of a port", "<port_id>", SW_API_PT_AN_RESTART, NULL},
            {"header", "set", "set atheros header/tag status of a port", "<port_id> <enable|disable>", SW_API_PT_HDR_SET, NULL},
            {"txhdr", "set", "set tx frame atheros header/tag status of a port", "<port_id> <noheader|onlymanagement|allframe>", SW_API_PT_TXHDR_SET, NULL},
            {"rxhdr", "set", "set rx frame atheros header/tag status of a port", "<port_id> <noheader|onlymanagement|allframe>", SW_API_PT_RXHDR_SET, NULL},
            {"hdrtype", "set", "set atheros header/tag type", "<enable|disable> <type 0x-0xffff>", SW_API_HEADER_TYPE_SET, NULL},
            {"flowCtrl", "set", "set flow control status of a port", "<port_id> <enable|disable>", SW_API_PT_FLOWCTRL_SET, NULL},
            {"flowCtrlforcemode", "set", "set flow control force mode of a port", "<port_id> <enable|disable>", SW_API_PT_FLOWCTRL_MODE_SET, NULL},
            {"powersave", "set", "set powersave status of a port", "<port_id> <enable|disable>", SW_API_PT_POWERSAVE_SET, NULL},
            {"hibernate", "set", "set hibernate status of a port", "<port_id> <enable|disable>", SW_API_PT_HIBERNATE_SET, NULL},
            {"cdt", "run", "run cable diagnostic test of a port", "<port_id> <mdi_pair>", SW_API_PT_CDT, NULL},
            {"txmacstatus", "set", "set txmac status of a port", "<port_id> <enable|disable>", SW_API_TXMAC_STATUS_SET, NULL},
            {"rxmacstatus", "set", "set rxmac status of a port", "<port_id> <enable|disable>", SW_API_RXMAC_STATUS_SET, NULL},
            {"txfcstatus", "set", "set tx flow control status of a port", "<port_id> <enable|disable>", SW_API_TXFC_STATUS_SET, NULL},
            {"rxfcstatus", "set", "set rx flow control status of a port", "<port_id> <enable|disable>", SW_API_RXFC_STATUS_SET, NULL},
            {"bpstatus", "set", "set back pressure status of a port", "<port_id> <enable|disable>", SW_API_BP_STATUS_SET, NULL},
            {"linkforcemode", "set", "set link force mode of a port", "<port_id> <enable|disable>", SW_API_PT_LINK_MODE_SET, NULL},
            {"macLoopback", "set", "set mac level loop back mode of port", "<port_id> <enable|disable>", SW_API_PT_MAC_LOOPBACK_SET, NULL},
	    {"congedrop", "set", "set congestion drop of port queue", "<port_id> <queue_id> <enable|disable>", SW_API_PT_CONGESTION_DROP_SET, NULL},
	    {"ringfcthresh", "set", "set flwo ctrl thres of ring", "<ring_id> <on_thres> <off_thres>", SW_API_PT_RING_FLOW_CTRL_THRES_SET, NULL},
	    {"Ieee8023az", "set", "set 8023az status of a port", "<port_id> <enable|disable>", SW_API_PT_8023AZ_SET, NULL},
	    {"crossover", "set", "set crossover mode of a port", "<port_id> <auto|mdi|mdix>", SW_API_PT_MDIX_SET, NULL},
            {"crossover", "get", "get crossover mode of a port", "<port_id>", SW_API_PT_MDIX_GET, NULL},
            {"crossover", "status", "get current crossover status of a port", "<port_id>", SW_API_PT_MDIX_STATUS_GET, NULL},
            {"preferMedium", "set", "set prefer medium of a combo port", "<port_id> <copper|fiber>", SW_API_PT_COMBO_PREFER_MEDIUM_SET, NULL},
            {"preferMedium", "get", "get prefer medium of a combo port", "<port_id>", SW_API_PT_COMBO_PREFER_MEDIUM_GET, NULL},
            {"mediumType", "get", "get current medium status of a combo port", "<port_id>", SW_API_PT_COMBO_MEDIUM_STATUS_GET, NULL},
            {"fiberMode", "set", "set fiber mode of a combo fiber port", "<port_id> <100fx|1000bx>", SW_API_PT_COMBO_FIBER_MODE_SET, NULL},
            {"fiberMode", "get", "get fiber mode of a combo fiber port", "<port_id>", SW_API_PT_COMBO_FIBER_MODE_GET, NULL},
            {"localLoopback", "set", "set local loopback of a port", "<port_id> <enable|disable>", SW_API_PT_LOCAL_LOOPBACK_SET, NULL},
            {"localLoopback", "get", "get local loopback of a port", "<port_id>", SW_API_PT_LOCAL_LOOPBACK_GET, NULL},
            {"remoteLoopback", "set", "set remote loopback of a port", "<port_id> <enable|disable>", SW_API_PT_REMOTE_LOOPBACK_SET, NULL},
            {"remoteLoopback", "get", "get remote loopback of a port", "<port_id>", SW_API_PT_REMOTE_LOOPBACK_GET, NULL},
            {"reset", "set", "reset phy of a port", "<port_id>", SW_API_PT_RESET, NULL},
            {"poweroff", "set", "power off  phy of a port", "<port_id>", SW_API_PT_POWER_OFF, NULL},
            {"poweron", "set", "power on phy of a port", "<port_id>", SW_API_PT_POWER_ON, NULL},
            {"magicFrameMac", "set", "set magic frame mac address  of a port", "<port_id> <mac_address>", SW_API_PT_MAGIC_FRAME_MAC_SET, NULL},
            {"magicFrameMac", "get", "get magic frame mac address  of a port", "<port_id>", SW_API_PT_MAGIC_FRAME_MAC_GET, NULL},
            {"phyId", "get", "get phy id of a port", "<port_id>", SW_API_PT_PHY_ID_GET, NULL},
            {"wolstatus", "set", "set wol status of a port", "<port_id> <enable|disable>", SW_API_PT_WOL_STATUS_SET, NULL},
            {"wolstatus", "get", "get wol status of a port", "<port_id>", SW_API_PT_WOL_STATUS_GET, NULL},
            {"interfaceMode", "set", "set interface mode of phy", "<port_id> <psgmii_baset|psgmii_bx1000|psgmii_fx100|psgmii_amdet|sgmii_baset>", SW_API_PT_INTERFACE_MODE_SET, NULL},
            {"interfaceMode", "get", "get interface mode of phy", "<port_id>", SW_API_PT_INTERFACE_MODE_GET, NULL},
            {"interfaceMode", "status", "get current interface mode of phy", "<port_id>", SW_API_PT_INTERFACE_MODE_STATUS_GET, NULL},
	        {"counter", "set", "set counter  status of a port", "<port_id> <enable|disable>", SW_API_PT_COUNTER_SET, NULL},
	        {"counter", "get", "get counter status of a port", "<port_id>", SW_API_PT_COUNTER_GET, NULL},
	        {"counter", "show", "show counter statistics of a port", "<port_id>", SW_API_PT_COUNTER_SHOW, NULL},
            {NULL, NULL, NULL, NULL, (int)NULL, NULL},/*end of desc*/
        },
    },
#endif

    /*vlan*/
#ifdef IN_VLAN
    {
        "vlan", "config VLAN table",
        {
            {"entry", "set", "create a VLAN entry", "<vlan_id>", SW_API_VLAN_ADD, NULL},
            {"entry", "create", "create a VLAN entry", "<vlan_id>", SW_API_VLAN_ADD, NULL},
            {"entry", "del", "delete a VLAN entryn", "<vlan_id>", SW_API_VLAN_DEL, NULL},
            {"entry", "update", "update port member of a VLAN entry", "<vlan_id> <member_bitmap> <0>", SW_API_VLAN_MEM_UPDATE, NULL},
            {"entry", "find", "find a VLAN entry by VLAN id", "<vlan_id>", SW_API_VLAN_FIND, NULL},
            {"entry", "next", "find next VLAN entry by VLAN id", "<vlan_id>",SW_API_VLAN_NEXT, NULL},
            {"entry", "append", "append a VLAN entry", "", SW_API_VLAN_APPEND, NULL},
            {"entry", "flush", "flush all VLAN entries", "",SW_API_VLAN_FLUSH, NULL},
            {"fid", "set", "set VLAN entry fid", "<vlan_id> <fid>",SW_API_VLAN_FID_SET, NULL},
            {"member", "set", "add VLAN entry member", "<vlan_id> <port_id> <unmodified|untagged|tagged>",SW_API_VLAN_MEMBER_ADD, NULL},
            {"member", "add", "add VLAN entry member", "<vlan_id> <port_id> <unmodified|untagged|tagged>",SW_API_VLAN_MEMBER_ADD, NULL},
            {"member", "del", "del VLAN entry member", "<vlan_id> <port_id>",SW_API_VLAN_MEMBER_DEL, NULL},
            {"learnsts", "set", "set VLAN entry learn status", "<vlan_id> <enable|disable>",SW_API_VLAN_LEARN_STATE_SET, NULL},
            {NULL, NULL, NULL, NULL, (int)NULL, NULL}/*end of desc*/
        },
    },
#endif

    /*portvlan*/
#ifdef IN_PORTVLAN
    {
        "portVlan", "config port base VLAN",
        {
            {"ingress", "set", "set ingress VLAN mode of a port", "<port_id> <disable|secure|check|fallback>", SW_API_PT_ING_MODE_SET, NULL},
            {"egress", "set", "set egress VLAN mode of a port", "<port_id> <unmodified|untagged|tagged|hybrid|untouched>", SW_API_PT_EG_MODE_SET, NULL},
            {"member", "set", "add a member to the port based VLAN of a port", "<port_id> <port_bitmap>", SW_API_PT_VLAN_MEM_UPDATE, NULL},
            {"member", "add", "add a member to the port based VLAN of a port", "<port_id> <memport_id>", SW_API_PT_VLAN_MEM_ADD, NULL},
            {"member", "del", "delete a member from the port based VLAN of a port", "<port_id> <memport_id>", SW_API_PT_VLAN_MEM_DEL, NULL},
            {"member", "update", "update members of the port based VLAN of a port", "<port_id> <port_bitmap>", SW_API_PT_VLAN_MEM_UPDATE, NULL},
            {"defaultVid", "set", "set default VLAN id of a port", "<port_id> <vid>", SW_API_PT_DEF_VID_SET, NULL},
            {"forceVid", "set", "set VLAN id enforcement status of a port", "<port_id> <enable|disable>", SW_API_PT_FORCE_DEF_VID_SET, NULL},
            {"forceMode", "set", "set port based VLAN enforcement status of a port", "<port_id> <enable|disable>", SW_API_PT_FORCE_PORTVLAN_SET, NULL},
            {"nestVlan", "set", "set nest VLAN status of a port", "<port_id> <enable|disable>", SW_API_PT_NESTVLAN_SET, NULL},
            {"sVlanTPID", "set", "set service VLAN tpid", "<tpid>", SW_API_NESTVLAN_TPID_SET, NULL},
            {"invlan", "set", "set port invlan mode", "<port_id> <admit_all|admit_tagged|admit_untagged>", SW_API_PT_IN_VLAN_MODE_SET, NULL},
            {"tlsMode", "set", "set TLS mode", "<port_id> <enable|disable>", SW_API_PT_TLS_SET, NULL},
            {"priPropagation", "set", "set priority propagation", "<port_id> <enable|disable>", SW_API_PT_PRI_PROPAGATION_SET, NULL},
            {"defaultSVid", "set", "set default SVID", "<port_id> <vlan_id>", SW_API_PT_DEF_SVID_SET, NULL},
            {"defaultCVid", "set", "set default CVID", "<port_id> <vlan_id>", SW_API_PT_DEF_CVID_SET, NULL},
            {"vlanPropagation", "set", "set vlan propagation", "<port_id> <disable|clone|replace>", SW_API_PT_VLAN_PROPAGATION_SET, NULL},
            {"translation", "set", "add vlan translation", "<port_id>", SW_API_PT_VLAN_TRANS_ADD, NULL},
            {"translation", "add", "add vlan translation", "<port_id>", SW_API_PT_VLAN_TRANS_ADD, NULL},
            {"translation", "del", "del vlan translation", "<port_id>", SW_API_PT_VLAN_TRANS_DEL, NULL},
            {"translation", "iterate", "iterate vlan translation tables", "<port_id> <iterator>", SW_API_PT_VLAN_TRANS_ITERATE, NULL},
            {"qinqMode", "set", "set qinq mode", "<ctag|stag>", SW_API_QINQ_MODE_SET, NULL},
            {"qinqRole", "set", "set qinq role", "<port_id> <edge|core>", SW_API_PT_QINQ_ROLE_SET, NULL},
            {"macvlanxlt", "set", "set mac vlan xlt status", "<port_id> <enable|disable>", SW_API_PT_MAC_VLAN_XLT_SET, NULL},
            {"netiso", "set", "enable public/private net isolate", "<enable|disable>", SW_API_NETISOLATE_SET, NULL},
            {"egbypass", "set", "enable egress translation filter bypass", "<enable|disable>", SW_API_EG_FLTR_BYPASS_EN_SET, NULL},
            {"ptvrfid", "set", "set port VRF ID", "<port_id> <vrf_id>", SW_API_PT_VRF_ID_SET, NULL},
            {NULL, NULL, NULL, NULL, (int)NULL, NULL}/*end of desc*/
        },
    },
#endif

    /*fdb*/
#ifdef IN_FDB
    {
        "fdb", "config FDB table",
        {
            {"entry", "set", "add a FDB entry", "", SW_API_FDB_ADD, NULL},
            {"entry", "add", "add a FDB entry", "", SW_API_FDB_ADD, NULL},
            {"entry", "del", "delete a FDB entry", "", SW_API_FDB_DELMAC, NULL},
            {"entry", "flush", "flush all FDB entries", "<0:dynamic only|1:dynamic and static>", SW_API_FDB_DELALL, NULL},
            {"entry", "find", "find a FDB entry", "", SW_API_FDB_FIND, NULL},
            {"entry", "iterate", "iterate all FDB entries", "<iterator>", SW_API_FDB_ITERATE, NULL},
            {"entry", "extendnext", "find next FDB entry in extend mode", "", SW_API_FDB_EXTEND_NEXT, NULL},
            {"entry", "extendfirst", "find first FDB entry in extend mode", "", SW_API_FDB_EXTEND_FIRST, NULL},
            {"entry", "transfer", "transfer port info in FDB entry", "<old port_id> <new port_id> <fid>", SW_API_FDB_TRANSFER, NULL},
            {"portEntry", "flush", "flush all FDB entries by a port", "<port_id> <0:dynamic only|1:dynamic and static>", SW_API_FDB_DELPORT, NULL},
            {"firstEntry", "find", "find the first FDB entry", "", SW_API_FDB_FIRST, NULL},
            {"nextEntry", "find", "find next FDB entry", "", SW_API_FDB_NEXT, NULL},
            {"portLearn", "set", "set FDB entry learning status of a port", "<port_id> <enable|disable>", SW_API_FDB_PT_LEARN_SET, NULL},
            {"ageCtrl", "set", "set FDB entry aging status", "<enable|disable>", SW_API_FDB_AGE_CTRL_SET, NULL},
            {"vlansmode", "set", "set FDB vlan search mode", "<ivl|svl>", SW_API_FDB_VLAN_IVL_SVL_SET, NULL},
            {"ageTime", "set", "set FDB entry aging time", "<time:s>", SW_API_FDB_AGE_TIME_SET, NULL},
            {"ptlearnlimit", "set", "set port FDB entry learn limit", "<port_id> <enable|disable> <limitcounter>", SW_API_PT_FDB_LEARN_LIMIT_SET, NULL},
            {"ptlearnexceedcmd", "set", "set port forwarding cmd when exceed learn limit", "<port_id> <forward|drop|cpycpu|rdtcpu>", SW_API_PT_FDB_LEARN_EXCEED_CMD_SET, NULL},
            {"learnlimit", "set", "set FDB entry learn limit", "<enable|disable> <limitcounter>", SW_API_FDB_LEARN_LIMIT_SET, NULL},
            {"learnexceedcmd", "set", "set forwarding cmd when exceed learn limit", "<forward|drop|cpycpu|rdtcpu>", SW_API_FDB_LEARN_EXCEED_CMD_SET, NULL},
            {"resventry", "set", "add a reserve FDB entry", "", SW_API_FDB_RESV_ADD, NULL},
            {"resventry", "add", "add a reserve FDB entry", "", SW_API_FDB_RESV_ADD, NULL},
            {"resventry", "del", "delete reserve a FDB entry", "", SW_API_FDB_RESV_DEL, NULL},
            {"resventry", "find", "find a reserve FDB entry", "", SW_API_FDB_RESV_FIND, NULL},
            {"resventry", "iterate", "iterate all reserve FDB entries", "<iterator>", SW_API_FDB_RESV_ITERATE, NULL},
            {"ptLearnstatic", "set", "set FDB entry learning static status of a port", "<port_id> <enable|disable>", SW_API_FDB_PT_LEARN_STATIC_SET, NULL},
            {"port", "add", "add one port to a FDB entry", "<fid> <macaddr> <port_id>", SW_API_FDB_PORT_ADD, NULL},
            {"port", "del", "del one port from a FDB entry", "<fid> <macaddr> <port_id>", SW_API_FDB_PORT_DEL, NULL},
            {NULL, NULL, NULL, NULL, (int)NULL, NULL}/*end of desc*/
        },
    },
#endif

    /*acl*/
#ifdef IN_ACL
    {
        "acl", "config ACL",
        {
            {"list", "create", "create an ACL list", "<list_id> <priority>", SW_API_ACL_LIST_CREAT, NULL},
            {"list", "destroy", "destroy an ACL list", "<list_id>", SW_API_ACL_LIST_DESTROY, NULL},
            {"list", "bind", "bind an ACL list to a port", "<list_id> <0-0:direction> <0-0:objtype> <objindex>", SW_API_ACL_LIST_BIND, NULL},
            {"list", "unbind", "unbind an ACL list from a port", "<list_id> <0-0:direction> <0-0:objtype> <objindex>", SW_API_ACL_LIST_UNBIND, NULL},
            {"rule", "add", "add ACL rules to an ACL list", "<list_id> <rule_id> <rule_nr>", SW_API_ACL_RULE_ADD, NULL},
            {"rule", "del", "delete ACL rules from an ACL list", "<list_id> <rule_id> <rule_nr>", SW_API_ACL_RULE_DELETE, NULL},
            {"rule", "query", "query a ACL rule", "<list_id> <rule_id>", SW_API_ACL_RULE_QUERY, NULL},
            {"rule", "active", "active ACL rules in an ACL list", "<list_id> <rule_id> <rule_nr>", SW_API_ACL_RULE_ACTIVE, NULL},
            {"rule", "deactive", "deactive ACL rules in an ACL list", "<list_id> <rule_id> <rule_nr>", SW_API_ACL_RULE_DEACTIVE, NULL},
            {"srcfiltersts", "set", "set status of ACL rules source filter", "<rule_id> <enable|disable>", SW_API_ACL_RULE_SRC_FILTER_STS_SET, NULL},
            {"status", "set", "set status of ACL engine", "<enable|disable>", SW_API_ACL_STATUS_SET, NULL},
            {"udfprofile", "set", "set port udf profile", "<port_id> <l2/l2snap/l3/l3plus/l4> <offset> <length>", SW_API_ACL_PT_UDF_PROFILE_SET, NULL},
            {NULL, NULL, NULL, NULL, (int)NULL, NULL}/*end of desc*/
        },
    },
#endif

    /*qos*/
#ifdef IN_QOS
    {
        "qos", "config Qos",
        {
#ifndef ISISC
            {"schMode", "set", "set traffic scheduling mode", "<sp|wrr|mix|mix_plus> <q0,q1,q3,q4>", SW_API_QOS_SCH_MODE_SET, NULL},
#endif
            {"qTxBufSts", "set", "set queue tx buffer counting status of a port", "<port_id> <enable|disable>", SW_API_QOS_QU_TX_BUF_ST_SET, NULL},
#ifdef ISISC
            {"qTxBufNr", "set", "set queue tx buffer number", "<port_id> <queueid:0-3> <number:0-120>", SW_API_QOS_QU_TX_BUF_NR_SET, NULL},
#else
            {"qTxBufNr", "set", "set queue tx buffer number", "<port_id> <queueid:0-3> <number:0-60>", SW_API_QOS_QU_TX_BUF_NR_SET, NULL},
#endif
            {"ptTxBufSts", "set", "set port tx buffer counting status of a port", "<port_id> <enable|disable>", SW_API_QOS_PT_TX_BUF_ST_SET, NULL},
            {"ptRedEn", "set", "set status of port wred of a port", "<port_id> <enable|disable>", SW_API_QOS_PT_RED_EN_SET, NULL},
#ifdef ISISC
            {"ptTxBufNr", "set", "set port tx buffer number", "<port_id> <number:0-504>", SW_API_QOS_PT_TX_BUF_NR_SET, NULL},
#else
            {"ptTxBufNr", "set", "set port tx buffer number", "<port_id> <number:0-252>", SW_API_QOS_PT_TX_BUF_NR_SET, NULL},
#endif
#ifdef ISISC
            {"ptRxBufNr", "set", "set port rx buffer number", "<port_id> <number:0-120>", SW_API_QOS_PT_RX_BUF_NR_SET, NULL},
#else
            {"ptRxBufNr", "set", "set port rx buffer number", "<port_id> <number:0-60>", SW_API_QOS_PT_RX_BUF_NR_SET, NULL},
#endif
#ifndef ISISC
            {"up2q", "set", "set user priority to queue mapping", "<up:0-7> <queueid:0-3>", SW_API_COSMAP_UP_QU_SET, NULL},
            {"dscp2q", "set", "set dscp to queue mapping", "<dscp:0-63> <queueid:0-3>", SW_API_COSMAP_DSCP_QU_SET, NULL},
#endif
#ifdef ISISC
            {"ptMode", "set", "set Qos mode of a port", "<port_id> <da|up|dscp> <enable|disable>", SW_API_QOS_PT_MODE_SET, NULL},
            {"ptModePri", "set", "set the priority of Qos modes of a port", "<port_id> <da|up|dscp> <priority:0-3>", SW_API_QOS_PT_MODE_PRI_SET, NULL},
#else
            {"ptMode", "set", "set Qos mode of a port", "<port_id> <da|up|dscp|port> <enable|disable>", SW_API_QOS_PT_MODE_SET, NULL},
            {"ptModePri", "set", "set the priority of Qos modes of a port", "<port_id> <da|up|dscp|port> <priority:0-3>", SW_API_QOS_PT_MODE_PRI_SET, NULL},
#endif
#ifndef ISISC
            {"ptDefaultUp", "set", "set default user priority for received frames of a port", "<port_id> <up:0-7>", SW_API_QOS_PORT_DEF_UP_SET, NULL},
#endif
            {"ptschMode", "set", "set port traffic scheduling mode", "<port_id> <sp|wrr|mix|mixplus> <q0,q1,q2,q3,q4,q5>", SW_API_QOS_PORT_SCH_MODE_SET, NULL},
            {"ptDefaultSpri", "set", "set default stag priority for received frames of a port", "<port_id> <spri:0-7>", SW_API_QOS_PT_DEF_SPRI_SET, NULL},
            {"ptDefaultCpri", "set", "set default ctag priority for received frames of a port", "<port_id> <cpri:0-7>", SW_API_QOS_PT_DEF_CPRI_SET, NULL},
            {"ptFSpriSts", "set", "set port force Stag priority status for received frames of a port", "<port_id> <enable|disable>", SW_API_QOS_PT_FORCE_SPRI_ST_SET, NULL},
            {"ptFCpriSts", "set", "set port force Ctag priority status for received frames of a port", "<port_id> <enable|disable>", SW_API_QOS_PT_FORCE_CPRI_ST_SET, NULL},
            {"ptQuRemark", "set", "set egress queue based remark", "<port_id> <queue_id> <table_id> <enable|disable>", SW_API_QOS_QUEUE_REMARK_SET, NULL},
            {NULL, NULL, NULL, NULL, (int)NULL, NULL}/*end of desc*/
        },
    },
#endif

    /*igmp*/
#ifdef IN_IGMP
    {
        "igmp", "config IGMP/MLD",
        {
            {"mode", "set", "set IGMP/MLD snooping status of a port", "<port_id> <enable|disable>", SW_API_PT_IGMPS_MODE_SET, NULL},
            {"cmd", "set", "set IGMP/MLD frames forwarding command", "<forward|drop|cpycpu|rdtcpu>", SW_API_IGMP_MLD_CMD_SET, NULL},
            {"portJoin", "set", "set IGMP/MLD hardware joining status", "<port_id> <enable|disable>", SW_API_IGMP_PT_JOIN_SET, NULL},
            {"portLeave", "set", "set IGMP/MLD hardware leaving status", "<port_id> <enable|disable>", SW_API_IGMP_PT_LEAVE_SET, NULL},
            {"rp", "set", "set IGMP/MLD router ports", "<port_bit_map>", SW_API_IGMP_RP_SET, NULL},
            {"createStatus", "set", "set IGMP/MLD ability for creating entry", "<enable|disable>", SW_API_IGMP_ENTRY_CREAT_SET, NULL},
            {"static", "set", "set IGMP/MLD static status for creating entry", "<enable|disable>", SW_API_IGMP_ENTRY_STATIC_SET, NULL},
            {"leaky", "set", "set IGMP/MLD leaky status for creating entry", "<enable|disable>", SW_API_IGMP_ENTRY_LEAKY_SET, NULL},
            {"version3", "set", "set IGMP v3/MLD v2 status for creating entry", "<enable|disable>", SW_API_IGMP_ENTRY_V3_SET, NULL},
            {"queue", "set", "set IGMP/MLD queue status for creating entry", "<enable|disable> <queue_id>", SW_API_IGMP_ENTRY_QUEUE_SET, NULL},
            {"ptlearnlimit", "set", "set port Multicast entry learn limit", "<port_id> <enable|disable> <limitcounter>", SW_API_PT_IGMP_LEARN_LIMIT_SET, NULL},
            {"ptlearnexceedcmd", "set", "set port forwarding cmd when exceed multicast learn limit", "<port_id> <forward|drop|cpycpu|rdtcpu>", SW_API_PT_IGMP_LEARN_EXCEED_CMD_SET, NULL},
            {"multi", "set", "set igmp/mld entry", "<entry>", SW_API_IGMP_SG_ENTRY_SET, NULL},
            {"multi", "clear", "clear igmp/mld entry", "<entry>", SW_API_IGMP_SG_ENTRY_CLEAR, NULL},
            {"multi", "show", "show all igmp/mld entry", "", SW_API_IGMP_SG_ENTRY_SHOW, NULL},
            {NULL, NULL, NULL, NULL, (int)NULL, NULL}/*end of desc*/
        },
    },
#endif

    /*leaky*/
#ifdef IN_LEAKY
    {
        "leaky", "config leaky",
        {
            {"ucMode", "set", "set unicast packets leaky mode", "<port|fdb>", SW_API_UC_LEAKY_MODE_SET, NULL},
            {"mcMode", "set", "set multicast packets leaky mode", "<port|fdb>", SW_API_MC_LEAKY_MODE_SET, NULL},
            {"arpMode", "set", "set arp packets leaky mode", "<port_id> <enable|disable>", SW_API_ARP_LEAKY_MODE_SET, NULL},
            {"ptUcMode", "set", "set unicast packets leaky status of a port", "<port_id> <enable|disable>", SW_API_PT_UC_LEAKY_MODE_SET, NULL},
            {"ptMcMode", "set", "set multicast packets leaky status of a port", "<port_id> <enable|disable>", SW_API_PT_MC_LEAKY_MODE_SET, NULL},
            {NULL, NULL, NULL, NULL, (int)NULL, NULL}/*end of desc*/
        },
    },
#endif

    /*mirror*/
#ifdef IN_MIRROR
    {
        "mirror", "config mirror",
        {
            {"analyPt", "set", "set mirror analysis port", "<port_id>", SW_API_MIRROR_ANALY_PT_SET, NULL},
            {"ptIngress", "set", "set ingress mirror status of a port", "<port_id> <enable|disable>", SW_API_MIRROR_IN_PT_SET, NULL},
            {"ptEgress", "set", "set egress mirror status of a port", "<port_id> <enable|disable>", SW_API_MIRROR_EG_PT_SET, NULL},
            {NULL, NULL, NULL, NULL, (int)NULL, NULL}/*end of desc*/
        },
    },
#endif

    /*rate*/
#ifdef IN_RATE
    {
        "rate", "config rate limit",
        {
#ifndef ISISC
            {"qEgress", "set", "set egress rate limit of a queue", "<port_id> <queueid:0-3> <speed:(kbps)> <enable|disable>", SW_API_RATE_QU_EGRL_SET, NULL},
            {"ptEgress", "set", "set egress rate limit of a port", "<port_id> <speed:(kbps)> <enable|disable>", SW_API_RATE_PT_EGRL_SET, NULL},
            {"ptIngress", "set", "set ingress rate limit of a port", "<port_id> <speed:(kbps)> <enable|disable>", SW_API_RATE_PT_INRL_SET, NULL},
            {"stormCtrl", "set", "set storm control status of a particular frame type", "<port_id> <unicast|multicast|broadcast> <enable|disable>", SW_API_STORM_CTRL_FRAME_SET, NULL},
            {"stormCtrlRate", "set", "set storm ctrl rate", "<port_id> <rate:(packets/s)>", SW_API_STORM_CTRL_RATE_SET, NULL},
#endif
            {"portpolicer", "set", "set port policer", "<port_id>", SW_API_RATE_PORT_POLICER_SET, NULL},
            {"portshaper", "set", "set port egress shaper", "<port_id> <enable|disable>", SW_API_RATE_PORT_SHAPER_SET, NULL},
            {"queueshaper", "set", "set queue egress shaper", "<port_id> <queue_id> <enable|disable>", SW_API_RATE_QUEUE_SHAPER_SET, NULL},
            {"aclpolicer", "set", "set acl policer", "<policer_id>", SW_API_RATE_ACL_POLICER_SET, NULL},
            {"ptAddRateByte", "set", "set add_rate_byte when cal rate ", "<port_id> <number:0-255>", SW_API_RATE_PT_ADDRATEBYTE_SET, NULL},
            {"ptgolflowen", "set", "set status of port globle flow control", "<port_id> <enable|disable>", SW_API_RATE_PT_GOL_FLOW_EN_SET, NULL},
            {NULL, NULL, NULL, NULL, (int)NULL, NULL}/*end of desc*/
        },
    },
#endif

#ifdef IN_SEC
#ifdef ISISC
    {
        "sec", "config security",
        {
            {"mac", "set", "set MAC layer related security", "<resv_vid/invalid_src_addr> <value>", SW_API_SEC_MAC_SET, NULL},
            {"ip", "set", "set IP layer related security", "<invalid_ver/same_addr/ttl_change_status/ttl_val> <value>", SW_API_SEC_IP_SET, NULL},
            {"ip4", "set", "set IP4 related security", "<invalid_hl/hdr_opts/invalid_df/frag_offset_min_len/frag_offset_min_size/frag_offset_max_len/invalid_frag_offset/invalid_sip/invalid_dip/invalid_chksum/invalid_pl/df_clear_status/ipid_random_status> <value>", SW_API_SEC_IP4_SET, NULL},
            {"ip6", "set", "set IP6 related security", "<invalid_dip/invalid_sip/invalid_pl> <value>", SW_API_SEC_IP6_SET, NULL},
            {"tcp", "set", "set TCP related security", "<blat/invalid_hl/min_hdr_size/invalid_syn/su_block/sp_block/sap_block/xmas_scan/null_scan/sr_block/sf_block/sar_block/rst_scan/rst_with_data/fa_block/pa_block/ua_block/invalid_chksum/invalid_urgptr/invalid_opts> <value>", SW_API_SEC_TCP_SET, NULL},
            {"udp", "set", "set UDP related security", "<blat/invalid_len/invalid_chksum> <value>", SW_API_SEC_UDP_SET, NULL},
            {"icmp4", "set", "set ICMP4 related security", "<ping_pl_exceed/ping_frag/ping_max_pl> <value>", SW_API_SEC_ICMP4_SET, NULL},
            {"icmp6", "set", "set ICMP6 related security", "<ping_pl_exceed/ping_frag/ping_max_pl> <value>", SW_API_SEC_ICMP6_SET, NULL},
            {NULL, NULL, NULL, NULL, (int)NULL, NULL}/*end of desc*/
        },
    },
#endif
#endif

    /*stp*/
#ifdef IN_STP
    {
        "stp", "config STP",
        {
            {"portState", "set", "set STP state of a port", "<st_id> <port_id> <disable|block|listen|learn|forward>", SW_API_STP_PT_STATE_SET, NULL},
            {NULL, NULL, NULL, NULL, (int)NULL, NULL}/*end of desc*/
        },
    },
#endif

    /*mib*/
#ifdef IN_MIB
    {
        "mib", "show MIB statistics information",
        {
            {"status", "set", "set mib status", "<enable|disable>",  SW_API_MIB_STATUS_SET, NULL},
            {"counters",  "flush", "flush counters of a port", "<port_id>", SW_API_PT_MIB_FLUSH_COUNTERS, NULL},
            {"cpuKeep", "set", "set cpu keep bit", "<enable|disable>",  SW_API_MIB_CPU_KEEP_SET, NULL},
            {NULL, NULL, NULL, NULL, (int)NULL, NULL}/*end of desc*/
        },
    },
#endif

    /* led */
#ifdef IN_LED
    {
        "led", "set/get led control pattern",
        {
            {"ctrlpattern", "set", "set led control pattern", "<group_id> <led_id>", SW_API_LED_PATTERN_SET, NULL},
            {NULL, NULL, NULL, NULL, (int)NULL, NULL}/*end of desc*/
        },
    },
#endif

    /* cosmap */
#ifdef IN_COSMAP
    {
        "cosmap", "set/get cosmap table",
        {
            {"dscp2pri", "set", "set dscp to priority map table", "<dscp> <priority>", SW_API_COSMAP_DSCP_TO_PRI_SET, NULL},
            {"dscp2dp", "set", "set dscp to dp map table", "<dscp> <dp>", SW_API_COSMAP_DSCP_TO_DP_SET, NULL},
            {"up2pri", "set", "set dot1p to priority map table", "<up> <priority>", SW_API_COSMAP_UP_TO_PRI_SET, NULL},
            {"up2dp", "set", "set dot1p to dp map table", "<up> <dp>", SW_API_COSMAP_UP_TO_DP_SET, NULL},
            {"dscp2ehpri", "set", "set dscp to priority map table for WAN port", "<dscp> <priority>", SW_API_COSMAP_DSCP_TO_EHPRI_SET, NULL},
            {"dscp2ehdp", "set", "set dscp to dp map table for WAN port", "<dscp> <dp>", SW_API_COSMAP_DSCP_TO_EHDP_SET, NULL},
            {"up2ehpri", "set", "set dot1p to priority map table for WAN port", "<up> <priority>", SW_API_COSMAP_UP_TO_EHPRI_SET, NULL},
            {"up2ehdp", "set", "set dot1p to dp map table for WAN port", "<up> <dp>", SW_API_COSMAP_UP_TO_EHDP_SET, NULL},
            {"pri2q", "set", "set priority to queue mapping", "<priority> <queueid>", SW_API_COSMAP_PRI_TO_QU_SET, NULL},
            {"pri2ehq", "set", "set priority to enhanced queue mapping", "<priority> <queueid>", SW_API_COSMAP_PRI_TO_EHQU_SET, NULL},
            {"egRemark", "set", "set egress remark table", "<tableid>", SW_API_COSMAP_EG_REMARK_SET, NULL},
            {NULL, NULL, NULL, NULL, (int)NULL, NULL}/*end of desc*/
        },
    },
#endif

    /*misc*/
#ifdef IN_MISC
    {
        "misc", "config miscellaneous",
        {
#ifndef ISISC
            {"arp", "set", "set arp packets hardware identification status", "<enable|disable>", SW_API_ARP_STATUS_SET, NULL},
#endif
            {"frameMaxSize", "set", "set the maximal received frame size of the device", "<size:byte>", SW_API_FRAME_MAX_SIZE_SET, NULL},
#ifndef ISISC
            {"ptUnkSaCmd", "set", "set forwarding command for frames with unknown source address", "<port_id> <forward|drop|cpycpu|rdtcpu>", SW_API_PT_UNK_SA_CMD_SET, NULL},
#endif
            {"ptUnkUcFilter", "set", "set flooding status of unknown unicast frames", "<port_id> <enable|disable>", SW_API_PT_UNK_UC_FILTER_SET, NULL},
            {"ptUnkMcFilter", "set", "set flooding status of unknown multicast frames", "<port_id> <enable|disable>", SW_API_PT_UNK_MC_FILTER_SET, NULL},
            {"ptBcFilter", "set", "set flooding status of broadcast frames", "<port_id> <enable|disable>", SW_API_PT_BC_FILTER_SET, NULL},
            {"cpuPort", "set", "set cpu port status", "<enable|disable>", SW_API_CPU_PORT_STATUS_SET, NULL},
#ifndef ISISC
            {"bctoCpu", "set", "set broadcast frames to Cpu port status", "<enable|disable>", SW_API_BC_TO_CPU_PORT_SET, NULL},
#endif
#ifdef ISISC
            {"PppoeCmd", "set", "set pppoe frames forwarding command", "<forward|rdtcpu>", SW_API_PPPOE_CMD_SET, NULL},
#else
            {"PppoeCmd", "set", "set pppoe frames forwarding command", "<forward|drop|cpycpu|rdtcpu>", SW_API_PPPOE_CMD_SET, NULL},
#endif
            {"Pppoe", "set", "set pppoe frames hardware identification status", "<enable|disable>", SW_API_PPPOE_STATUS_SET, NULL},
            {"ptDhcp", "set", "set dhcp frames hardware identification status", "<port_id> <enable|disable>", SW_API_PT_DHCP_SET, NULL},
#ifdef ISISC
            {"arpcmd", "set", "set arp packets forwarding command", "<forward|cpycpu|rdtcpu>", SW_API_ARP_CMD_SET, NULL},
#else
            {"arpcmd", "set", "set arp packets forwarding command", "<forward|drop|cpycpu|rdtcpu>", SW_API_ARP_CMD_SET, NULL},
#endif
#ifdef ISISC
            {"eapolcmd", "set", "set eapol packets forwarding command", "<cpycpu|rdtcpu>", SW_API_EAPOL_CMD_SET, NULL},
#else
            {"eapolcmd", "set", "set eapol packets forwarding command", "<forward|drop|cpycpu|rdtcpu>", SW_API_EAPOL_CMD_SET, NULL},
#endif
#ifndef ISISC
            {"pppoesession", "add", "add a pppoe session entry", "<session_id> <enable|disable>", SW_API_PPPOE_SESSION_ADD, NULL},
            {"pppoesession", "del", "del a pppoe session entry", "<session_id>", SW_API_PPPOE_SESSION_DEL, NULL},
#endif
            {"eapolstatus", "set", "set eapol frames hardware identification status", "<port_id> <enable|disable>", SW_API_EAPOL_STATUS_SET, NULL},
            {"rip", "set", "set rip packets copy to cpu status", "<enable|disable>", SW_API_RIPV1_STATUS_SET, NULL},
            {"ptarpreq", "set", "set arp request packets hardware identification status", "<port_id> <enable|disable>", SW_API_PT_ARP_REQ_STATUS_SET, NULL},
            {"ptarpack", "set", "set arp ack packets hardware identification status", "<port_id> <enable|disable>", SW_API_PT_ARP_ACK_STATUS_SET, NULL},
            {"extendpppoe", "set", "add a pppoe session entry", "", SW_API_PPPOE_SESSION_TABLE_ADD, NULL},
            {"extendpppoe", "add", "add a pppoe session entry", "", SW_API_PPPOE_SESSION_TABLE_ADD, NULL},
            {"extendpppoe", "del", "del a pppoe session entry", "", SW_API_PPPOE_SESSION_TABLE_DEL, NULL},
            {"pppoeid", "set", "set a pppoe session id entry", "<index> <id>", SW_API_PPPOE_SESSION_ID_SET, NULL},
            {"intrmask", "set", "set switch interrupt mask", "<intr_mask>", SW_API_INTR_MASK_SET, NULL},
            {"intrstatus", "clear", "clear switch interrupt status", "<intr_mask>", SW_API_INTR_STATUS_CLEAR, NULL},
            {"intrportlinkmask", "set", "set link interrupt mask of a port", "<port_id> <intr_mask>", SW_API_INTR_PORT_LINK_MASK_SET, NULL},
            {"intrmaskmaclinkchg", "set", "set switch interrupt mask for mac link change", "<port_id> <enable | disable>", SW_API_INTR_MASK_MAC_LINKCHG_SET, NULL},
            {"intrstatusmaclinkchg", "clear", "clear switch interrupt status for mac link change", "", SW_API_INTR_STATUS_MAC_LINKCHG_CLEAR, NULL},
            {"cpuVid", "set", "set to_cpu vid status", "<enable|disable>", SW_API_CPU_VID_EN_SET, NULL},
            {"rtdPppoe", "set", "set RM_RTD_PPPOE_EN status", "<enable|disable>", SW_API_RTD_PPPOE_EN_SET, NULL},
			{"glomacaddr", "set", "set global macaddr", "<macaddr>", SW_API_GLOBAL_MACADDR_SET, NULL},
			{"lldp", "set", "set lldp frames hardware identification status", "<enable|disable>", SW_API_LLDP_STATUS_SET, NULL},
			{"framecrc", "set", "set frame crc reserve enable", "<enable|disable>", SW_API_FRAME_CRC_RESERVE_SET, NULL},
            {NULL, NULL, NULL, NULL, (int)NULL, NULL}/*end of desc*/
        },
    },
#endif

    /* IP */
#ifdef IN_IP
    {
        "ip", "config ip",
        {
            {"hostentry", "set", "add host entry", "", SW_API_IP_HOST_ADD, NULL},
            {"hostentry", "add", "add host entry", "", SW_API_IP_HOST_ADD, NULL},
            {"hostentry", "del", "del host entry", "<del_mode>", SW_API_IP_HOST_DEL, NULL},
            {"hostentry", "next", "next host entry", "<next_mode>", SW_API_IP_HOST_NEXT, NULL},
            {"hostentry", "bindcnt", "bind counter to host entry", "<host entry id> <cnt id> <enable|disable>", SW_API_IP_HOST_COUNTER_BIND, NULL},
            {"hostentry", "bindpppoe", "bind pppoe to host entry", "<host entry id> <pppoe id> <enable|disable>", SW_API_IP_HOST_PPPOE_BIND, NULL},
            {"ptarplearn", "set", "set port arp learn flag, bit0 req bit1 ack", "<port_id> <flag>", SW_API_IP_PT_ARP_LEARN_SET, NULL},
            {"arplearn",   "set", "set arp learn mode", "<learnlocal|learnall>", SW_API_IP_ARP_LEARN_SET, NULL},
            {"ptipsrcguard",   "set", "set ip source guard mode", "<port_id> <mac_ip|mac_ip_port|mac_ip_vlan|mac_ip_port_vlan|no_guard>", SW_API_IP_SOURCE_GUARD_SET, NULL},
            {"ptarpsrcguard",  "set", "set arp source guard mode", "<port_id> <mac_ip|mac_ip_port|mac_ip_vlan|mac_ip_port_vlan|no_guard>", SW_API_IP_ARP_GUARD_SET, NULL},
            {"routestatus", "set", "set ip route status", "<enable|disable>", SW_API_IP_ROUTE_STATUS_SET, NULL},
            {"intfentry", "set", "add interface mac address", "", SW_API_IP_INTF_ENTRY_ADD, NULL},
            {"intfentry", "add", "add interface mac address", "", SW_API_IP_INTF_ENTRY_ADD, NULL},
            {"intfentry", "del", "del interface mac address", "", SW_API_IP_INTF_ENTRY_DEL, NULL},
            {"ipunksrc", "set", "set ip unkown source command", "<forward|drop|cpycpu|rdtcpu>", SW_API_IP_UNK_SOURCE_CMD_SET, NULL},
            {"arpunksrc", "set", "set arp unkown source command", "<forward|drop|cpycpu|rdtcpu>", SW_API_ARP_UNK_SOURCE_CMD_SET, NULL},
            {"ipagetime", "set", "set dynamic ip entry age time", "<time>", SW_API_IP_AGE_TIME_SET, NULL},
            {"wcmphashmode", "set", "set wcmp hash mode", "<hashmode>", SW_API_WCMP_HASH_MODE_SET, NULL},
            {"wcmpentry", "set", "set wcmp entry", "<wcmp_id>", SW_API_IP_WCMP_ENTRY_SET, NULL},
            {"vrfbaseaddr", "set", "set vrf prv base address", "<ip4 addr>", SW_API_IP_VRF_BASE_ADDR_SET, NULL},
            {"vrfbasemask", "set", "set vrf prv base mask", "<ip4 addr>", SW_API_IP_VRF_BASE_MASK_SET, NULL},
            {"defaultroute", "set", "set default route entry", "", SW_API_IP_DEFAULT_ROUTE_SET, NULL},
            {"hostroute", "set", "set host route entry", "", SW_API_IP_HOST_ROUTE_SET, NULL},
            {"rfsip4", "set", "set rfs ip4", "", SW_API_IP_RFS_IP4_SET, NULL},
            {"rfsip6", "set", "set rfs ip6", "", SW_API_IP_RFS_IP6_SET, NULL},
            {"defaultflowcmd", "set", "set default flow command", "<vrf id> <lan2lan|wan2lan|lan2wan|wan2wan> <forward|drop|rdtcpu|admit_all>", SW_API_IP_DEFAULT_FLOW_CMD_SET, NULL},
            {"defaultrtflowcmd", "set", "set default route flow command", "<vrf id> <lan2lan|wan2lan|lan2wan|wan2wan> <forward|drop|rdtcpu|admit_all>", SW_API_IP_DEFAULT_RT_FLOW_CMD_SET, NULL},
            {NULL, NULL, NULL, NULL, (int)NULL, NULL}/*end of desc*/
        },
    },
#endif

    /* NAT */
#ifdef IN_NAT
    {
        "nat", "config nat",
        {
            {"natentry", "set", "add nat entry", "", SW_API_NAT_ADD, NULL},
            {"natentry", "add", "add nat entry", "", SW_API_NAT_ADD, NULL},
            {"natentry", "del", "del nat entry", "<del_mode>", SW_API_NAT_DEL, NULL},
            {"natentry", "next", "next nat entry", "<next_mode>", SW_API_NAT_NEXT, NULL},
            {"natentry", "bindcnt", "bind counter to nat entry", "<nat entry id> <cnt id> <enable|disable>", SW_API_NAT_COUNTER_BIND, NULL},
            {"naptentry", "set", "add napt entry", "", SW_API_NAPT_ADD, NULL},
            {"naptentry", "add", "add napt entry", "", SW_API_NAPT_ADD, NULL},
            {"naptentry", "del", "del napt entry", "<del_mode>", SW_API_NAPT_DEL, NULL},
            {"naptentry", "next", "next napt entry", "<next_mode>", SW_API_NAPT_NEXT, NULL},
            {"naptentry", "bindcnt", "bind counter to napt entry", "<napt entry id> <cnt id> <enable|disable>", SW_API_NAPT_COUNTER_BIND, NULL},
            {"natstatus", "set", "set nat status", "<enable|disable>", SW_API_NAT_STATUS_SET, NULL},
            {"naptstatus", "set", "set napt status", "<enable|disable>", SW_API_NAPT_STATUS_SET, NULL},
            {"nathash", "set", "set nat hash mode", "<flag>", SW_API_NAT_HASH_MODE_SET, NULL},
            {"naptmode", "set", "set napt mode", "<fullcone|strictcone|portstrict|synmatric>", SW_API_NAPT_MODE_SET, NULL},
            {"prvbaseaddr", "set", "set nat prv base address", "<ip4 addr>", SW_API_PRV_BASE_ADDR_SET, NULL},
            {"prvaddrmode", "set", "set nat prv address map mode", "<enable|disable>", SW_API_PRV_ADDR_MODE_SET, NULL},
            {"pubaddr", "set", "add pub address", "", SW_API_PUB_ADDR_ENTRY_ADD, NULL},
            {"pubaddr", "add", "add pub address", "", SW_API_PUB_ADDR_ENTRY_ADD, NULL},
            {"pubaddr", "del", "del pub address", "<del_mode>", SW_API_PUB_ADDR_ENTRY_DEL, NULL},
            {"natunksess", "set", "set nat unkown session command", "<forward|drop|cpycpu|rdtcpu>", SW_API_NAT_UNK_SESSION_CMD_SET, NULL},
            {"prvbasemask", "set", "set nat prv base mask", "<ip4 mask>", SW_API_PRV_BASE_MASK_SET, NULL},
			{"global", "set", "set global nat function", "<enable|disable> <enable:sync counter|disable:unsync counter>", SW_API_NAT_GLOBAL_SET, NULL},
			{"flowentry", "set", "add flow entry", "", SW_API_FLOW_ADD, NULL},
			{"flowentry", "add", "add flow entry", "", SW_API_FLOW_ADD, NULL},
            {"flowentry", "del", "del flow entry", "<del_mode>", SW_API_FLOW_DEL, NULL},
			{"flowentry", "next", "next flow entry", "<next_mode>", SW_API_FLOW_NEXT, NULL},
			{"flowcookie", "set", "set flow cookie", "", SW_API_FLOW_COOKIE_SET, NULL},
			{"flowrfs", "set", "set flow rfs", "<action>", SW_API_FLOW_RFS_SET, NULL},
            {NULL, NULL, NULL, NULL, (int)NULL, NULL}/*end of desc*/
        },
    },
#endif

    /*Trunk*/
#ifdef IN_TRUNK
    {
        "trunk", "config trunk",
        {
            {"group", "set", "set trunk group member info", "<trunk_id> <disable|enable> <port_bitmap>", SW_API_TRUNK_GROUP_SET, NULL},
            {"hashmode", "set", "set trunk hash mode", "<hash_mode>", SW_API_TRUNK_HASH_SET, NULL},
            {"mansa", "set", "set trunk manipulable sa", "<macaddr>", SW_API_TRUNK_MAN_SA_SET, NULL},
            {NULL, NULL, NULL, NULL, (int)NULL, NULL}/*end of desc*/
        },
    },
#endif

    /*Interface Control*/
#ifdef IN_INTERFACECONTROL
    {
        "interface", "config interface",
        {
            {"macmode", "set", "set mac mode info", "<port_id>", SW_API_MAC_MODE_SET, NULL},
            {"pt3azstatus", "set", "get mac mode info", "<port_id> <enable/disable>", SW_API_PORT_3AZ_STATUS_SET, NULL},
            {"phymode", "set", "set phy mode info", "<phy_id>", SW_API_PHY_MODE_SET, NULL},
            {"fx100ctrl", "set", "set fx100 config", "", SW_API_FX100_CTRL_SET, NULL},
            {"mac06exch", "set", "set mac0 and mac6 exchange status", "<enable/disable>", SW_API_MAC06_EXCH_SET, NULL},
            {NULL, NULL, NULL, NULL, (int)NULL, NULL}/*end of desc*/
        },
    },
#endif

    /* debug */
    {
        "debug", "read/write register",
        {
            {"phy", "get", "read phy register", "<ph_id> <reg_addr>", SW_API_PHY_GET, NULL},
            {"phy", "set", "write phy register", "<ph_id> <reg_addr> <value>", SW_API_PHY_SET, NULL},
            {"reg", "get", "read switch register", "<reg_addr> <4>", SW_API_REG_GET, NULL},
            {"reg", "set", "write switch register", "<reg_addr> <value> <4>", SW_API_REG_SET, NULL},
			{"reg", "dump", "dump switch register group", "<group id> <0-6>", SW_API_REG_DUMP, NULL},
			{"dbgreg", "dump", "dump switch dbg register group", "", SW_API_DBG_REG_DUMP, NULL},
            {"field", "get", "read switch register field", "<reg_addr> <offset> <len> <4>", SW_API_REG_FIELD_GET, NULL},
            {"field", "set", "write switch register field", "<reg_addr> <offset> <len> <value> <4>", SW_API_REG_FIELD_SET, NULL},
            {"aclList", "dump", "dump all acl list", "", SW_API_ACL_LIST_DUMP, NULL},
            {"aclRule", "dump", "dump all acl rule", "", SW_API_ACL_RULE_DUMP, NULL},
            {"device",  "reset", "reset device",     "", SW_API_SWITCH_RESET, NULL},
            {"ssdk",  "config", "show ssdk configuration",     "", SW_API_SSDK_CFG, NULL},
            {"preg", "get", "read psgmii register", "<reg_addr> <4>", SW_API_PSGMII_REG_GET, NULL},
            {"preg", "set", "write psgmii register", "<reg_addr> <value> <4>", SW_API_PSGMII_REG_SET, NULL},
            {NULL, NULL, NULL, NULL, (int)NULL, NULL}/*end of desc*/
        },
    },

    /*debug*/
    {
        "device", "set device id",
        {
            {"id", "set", "set device id", "<dev_id>", SW_CMD_SET_DEVID, cmd_set_devid},
            {NULL, NULL, NULL, NULL, (int)NULL, NULL}/*end of desc*/
        },
    },

    {"help", "type ? get help", {{NULL, NULL, NULL, NULL, (int)NULL, NULL}/*end of desc*/}},

    {"quit", "type quit/q quit shell", {{NULL, NULL, NULL, NULL, (int)NULL, NULL}/*end of desc*/}},

    {NULL, NULL, {{NULL, NULL, NULL, NULL, (int)NULL, NULL}}} /*end of desc*/
};
#else

#ifdef IN_PORTCONTROL
struct sub_cmd_des_t g_port_des[] =
{
	{"duplex", "set",   SW_API_PT_DUPLEX_SET, NULL},
	{"speed", "set",   SW_API_PT_SPEED_SET, NULL},
	{"autoNeg", "enable",   SW_API_PT_AN_ENABLE, NULL},
	{"autoAdv", "set",   SW_API_PT_AN_ADV_SET, NULL},
	{"autoNeg", "restart",   SW_API_PT_AN_RESTART, NULL},
	{"autoNegenable", "set",   SW_API_PT_AN_ENABLE, NULL},
	{"autoNegrestart", "set",   SW_API_PT_AN_RESTART, NULL},
	#ifndef IN_PORTCONTROL_MINI
	{"header", "set",   SW_API_PT_HDR_SET, NULL},
	#endif
	{"txhdr", "set",   SW_API_PT_TXHDR_SET, NULL},
	{"rxhdr", "set",   SW_API_PT_RXHDR_SET, NULL},
	{"hdrtype", "set",   SW_API_HEADER_TYPE_SET, NULL},
	{"poweroff", "set",   SW_API_PT_POWER_OFF, NULL},
	{"poweron", "set",   SW_API_PT_POWER_ON, NULL},
	#ifndef IN_PORTCONTROL_MINI
	{"flowCtrl", "set",   SW_API_PT_FLOWCTRL_SET, NULL},
	{"flowCtrlforcemode", "set",   SW_API_PT_FLOWCTRL_MODE_SET, NULL},
	{"powersave", "set",   SW_API_PT_POWERSAVE_SET, NULL},
	{"hibernate", "set",   SW_API_PT_HIBERNATE_SET, NULL},
	{"cdt", "run",   SW_API_PT_CDT, NULL},
	{"txmacstatus", "set",   SW_API_TXMAC_STATUS_SET, NULL},
	{"rxmacstatus", "set",   SW_API_RXMAC_STATUS_SET, NULL},
	{"txfcstatus", "set",   SW_API_TXFC_STATUS_SET, NULL},
	{"rxfcstatus", "set",   SW_API_RXFC_STATUS_SET, NULL},
	{"bpstatus", "set",   SW_API_BP_STATUS_SET, NULL},
	{"linkforcemode", "set",   SW_API_PT_LINK_MODE_SET, NULL},
	{"macLoopback", "set",   SW_API_PT_MAC_LOOPBACK_SET, NULL},
	{"congedrop", "set",   SW_API_PT_CONGESTION_DROP_SET, NULL},
	{"ringfcthresh", "set",   SW_API_PT_RING_FLOW_CTRL_THRES_SET, NULL},
	{"Ieee8023az", "set",   SW_API_PT_8023AZ_SET, NULL},
	{"crossover", "set",   SW_API_PT_MDIX_SET, NULL},
	{"preferMedium", "set",   SW_API_PT_COMBO_PREFER_MEDIUM_SET, NULL},
	{"fiberMode", "set",   SW_API_PT_COMBO_FIBER_MODE_SET, NULL},
	{"localLoopback", "set",   SW_API_PT_LOCAL_LOOPBACK_SET, NULL},
	{"remoteLoopback", "set",   SW_API_PT_REMOTE_LOOPBACK_SET, NULL},
	{"reset", "set",   SW_API_PT_RESET, NULL},
	{"magicFrameMac", "set",   SW_API_PT_MAGIC_FRAME_MAC_SET, NULL},
	{"wolstatus", "set",   SW_API_PT_WOL_STATUS_SET, NULL},
	{"interfaceMode", "set",   SW_API_PT_INTERFACE_MODE_SET, NULL},
	{"InterfaceModeApply", "set",   SW_API_PT_INTERFACE_MODE_APPLY, NULL},
	{"mtu", "set",   SW_API_PT_MTU_SET, NULL},
	{"mru", "set",   SW_API_PT_MRU_SET, NULL},
	{"srcfilter", "set",   SW_API_PT_SOURCE_FILTER_SET, NULL},
	{"frameMaxSize", "set",   SW_API_PT_FRAME_MAX_SIZE_SET, NULL},
	{"interface3az", "set", SW_API_PT_INTERFACE_3AZ_STATUS_SET, NULL},
	{"promiscmode", "set",   SW_API_PT_PROMISC_MODE_SET, NULL},
	{"eeecfg", "set", SW_API_PT_INTERFACE_EEE_CFG_SET, NULL},
	{"srcfiltercfg", "set",   SW_API_PT_SOURCE_FILTER_CONFIG_SET, NULL},
	{"switchportloopback", "set", SW_API_PT_SWITCH_PORT_LOOPBACK_SET, NULL},
	#endif
	{NULL, NULL,  0, NULL},/*end of desc*/
};
#endif

#ifdef IN_VLAN
struct sub_cmd_des_t g_vlan_des[] =
{
	{"entry", "set",   SW_API_VLAN_ADD, NULL},
	{"entry", "create",   SW_API_VLAN_ADD, NULL},
	{"entry", "del",   SW_API_VLAN_DEL, NULL},
	{"entry", "update",   SW_API_VLAN_MEM_UPDATE, NULL},
	{"entry", "append",   SW_API_VLAN_APPEND, NULL},
	{"entry", "flush",  SW_API_VLAN_FLUSH, NULL},
	{"fid", "set",  SW_API_VLAN_FID_SET, NULL},
	{"member", "set",  SW_API_VLAN_MEMBER_ADD, NULL},
	{"member", "add",  SW_API_VLAN_MEMBER_ADD, NULL},
	{"member", "del",  SW_API_VLAN_MEMBER_DEL, NULL},
	#ifndef IN_VLAN_MINI
	{"learnsts", "set",  SW_API_VLAN_LEARN_STATE_SET, NULL},
	#endif
	{NULL, NULL,  0, NULL},/*end of desc*/
};
#endif

#ifdef IN_PORTVLAN
struct sub_cmd_des_t g_portvlan_des[] =
{
	{"ingress", "set",   SW_API_PT_ING_MODE_SET, NULL},
	{"egress", "set",   SW_API_PT_EG_MODE_SET, NULL},
	{"member", "set",   SW_API_PT_VLAN_MEM_UPDATE, NULL},
	{"member", "add",   SW_API_PT_VLAN_MEM_ADD, NULL},
	{"member", "del",   SW_API_PT_VLAN_MEM_DEL, NULL},
	{"member", "update",   SW_API_PT_VLAN_MEM_UPDATE, NULL},
	{"defaultVid", "set",   SW_API_PT_DEF_VID_SET, NULL},
	{"forceVid", "set",   SW_API_PT_FORCE_DEF_VID_SET, NULL},
	{"forceMode", "set",   SW_API_PT_FORCE_PORTVLAN_SET, NULL},
	{"nestVlan", "set",   SW_API_PT_NESTVLAN_SET, NULL},
	{"sVlanTPID", "set",   SW_API_NESTVLAN_TPID_SET, NULL},
	{"invlan", "set",   SW_API_PT_IN_VLAN_MODE_SET, NULL},
	{"globalQinQMode", "set", SW_API_GLOBAL_QINQ_MODE_SET, NULL},
	{"ptQinQMode", "set", SW_API_PORT_QINQ_MODE_SET, NULL},
#ifdef HPPE
	{"inTpid", "set", SW_API_TPID_SET, NULL},
	{"egTpid", "set", SW_API_EGRESS_TPID_SET, NULL},
	{"ingressFilter", "set", SW_API_PT_INGRESS_VLAN_FILTER_SET, NULL},
	{"defaultVlanTag", "set", SW_API_PT_DEFAULT_VLANTAG_SET, NULL},
	{"tagPropagation", "set", SW_API_PT_TAG_PROPAGATION_SET, NULL},
	{"translationMissAction", "set", SW_API_PT_VLAN_XLT_MISS_CMD_SET, NULL},
	{"egMode", "set", SW_API_PT_VLANTAG_EGMODE_SET, NULL},
	{"vsiEgMode", "set", SW_API_PT_VSI_EGMODE_SET, NULL},
	{"vsiEgModeEn", "set", SW_API_PT_VLANTAG_VSI_EGMODE_EN_SET, NULL},
	{"counter", "set", SW_API_PT_VLAN_COUNTER_CLEANUP, NULL},
	{"translationAdv", "set", SW_API_PT_VLAN_TRANS_ADV_ADD, NULL},
#endif
	#ifndef IN_PORTVLAN_MINI
	{"tlsMode", "set",   SW_API_PT_TLS_SET, NULL},
	{"priPropagation", "set",   SW_API_PT_PRI_PROPAGATION_SET, NULL},
	#endif
	{"defaultSVid", "set",   SW_API_PT_DEF_SVID_SET, NULL},
	{"defaultCVid", "set",   SW_API_PT_DEF_CVID_SET, NULL},
	#ifndef IN_PORTVLAN_MINI
	{"vlanPropagation", "set",   SW_API_PT_VLAN_PROPAGATION_SET, NULL},
	{"translation", "set",   SW_API_PT_VLAN_TRANS_ADD, NULL},
	{"translation", "add",   SW_API_PT_VLAN_TRANS_ADD, NULL},
	{"translation", "del",   SW_API_PT_VLAN_TRANS_DEL, NULL},
	{"qinqMode", "set",   SW_API_QINQ_MODE_SET, NULL},
	{"qinqRole", "set",   SW_API_PT_QINQ_ROLE_SET, NULL},
	{"macvlanxlt", "set",   SW_API_PT_MAC_VLAN_XLT_SET, NULL},
	{"netiso", "set",   SW_API_NETISOLATE_SET, NULL},
	{"egbypass", "set",   SW_API_EG_FLTR_BYPASS_EN_SET, NULL},
	{"ptvrfid", "set",   SW_API_PT_VRF_ID_SET, NULL},
	#endif
	{NULL, NULL,  0, NULL},/*end of desc*/
};
#endif

#ifdef IN_FDB
struct sub_cmd_des_t g_fdb_des[] =
{
#ifndef IN_FDB_MINI
	{"entry", "set",   SW_API_FDB_ADD, NULL},
	{"entry", "add",   SW_API_FDB_ADD, NULL},
	{"entry", "del",   SW_API_FDB_DELMAC, NULL},
	{"entry", "flush",   SW_API_FDB_DELALL, NULL},
	{"entry", "transfer",   SW_API_FDB_TRANSFER, NULL},
	{"portEntry", "flush",   SW_API_FDB_DELPORT, NULL},
	{"firstEntry", "find",   SW_API_FDB_FIRST, NULL},
	{"nextEntry", "find",   SW_API_FDB_NEXT, NULL},
#endif
	{"portLearn", "set",   SW_API_FDB_PT_LEARN_SET, NULL},
#ifndef IN_FDB_MINI
	{"ageCtrl", "set",   SW_API_FDB_AGE_CTRL_SET, NULL},
	{"vlansmode", "set",   SW_API_FDB_VLAN_IVL_SVL_SET, NULL},
	{"ageTime", "set",   SW_API_FDB_AGE_TIME_SET, NULL},
	{"ptlearnlimit", "set",   SW_API_PT_FDB_LEARN_LIMIT_SET, NULL},
	{"ptlearnexceedcmd", "set",   SW_API_PT_FDB_LEARN_EXCEED_CMD_SET, NULL},
	{"learnlimit", "set",   SW_API_FDB_LEARN_LIMIT_SET, NULL},
	{"learnexceedcmd", "set",   SW_API_FDB_LEARN_EXCEED_CMD_SET, NULL},
	{"resventry", "set",   SW_API_FDB_RESV_ADD, NULL},
	{"resventry", "add",   SW_API_FDB_RESV_ADD, NULL},
	{"resventry", "del",   SW_API_FDB_RESV_DEL, NULL},
	{"ptLearnstatic", "set",   SW_API_FDB_PT_LEARN_STATIC_SET, NULL},
	{"port", "add",   SW_API_FDB_PORT_ADD, NULL},
	{"port", "del",   SW_API_FDB_PORT_DEL, NULL},
	{"LearnCtrl", "set", SW_API_FDB_LEARN_CTRL_SET, NULL},
	{"PtLearnCtrl", "set", SW_API_FDB_PT_NEWADDR_LEARN_SET, NULL},
	{"PtStationMove", "set", SW_API_FDB_PT_STAMOVE_SET, NULL},
	{"PtMacLimitCtrl", "set", SW_API_FDB_PT_MACLIMIT_CTRL_SET, NULL},
        {"fidEntry", "flush", SW_API_FDB_DEL_BY_FID, NULL},
#endif
	{NULL, NULL,  0, NULL},/*end of desc*/
};
#endif

#ifdef IN_ACL
struct sub_cmd_des_t g_acl_des[] =
{
	{"list", "create",   SW_API_ACL_LIST_CREAT, NULL},
	{"list", "destroy",   SW_API_ACL_LIST_DESTROY, NULL},
	{"list", "bind",   SW_API_ACL_LIST_BIND, NULL},
	{"list", "unbind",   SW_API_ACL_LIST_UNBIND, NULL},
	{"rule", "add",   SW_API_ACL_RULE_ADD, NULL},
	{"rule", "del",   SW_API_ACL_RULE_DELETE, NULL},
	{"rule", "active",   SW_API_ACL_RULE_ACTIVE, NULL},
	{"rule", "deactive",   SW_API_ACL_RULE_DEACTIVE, NULL},
	{"srcfiltersts", "set",   SW_API_ACL_RULE_SRC_FILTER_STS_SET, NULL},
	{"status", "set",   SW_API_ACL_STATUS_SET, NULL},
	{"udfprofile", "set",   SW_API_ACL_PT_UDF_PROFILE_SET, NULL},
	{"udf", "set", SW_API_ACL_UDF_SET, NULL},
	{NULL, NULL,  0, NULL},/*end of desc*/
};
#endif

#ifdef IN_QOS
struct sub_cmd_des_t g_qos_des[] =
{
#ifndef IN_QOS_MINI
#ifndef ISISC
	{"schMode", "set", SW_API_QOS_SCH_MODE_SET, NULL},
#endif
	{"qTxBufSts", "set",  SW_API_QOS_QU_TX_BUF_ST_SET, NULL},
#ifdef ISISC
	{"qTxBufNr", "set",   SW_API_QOS_QU_TX_BUF_NR_SET, NULL},
#else
	{"qTxBufNr", "set",  SW_API_QOS_QU_TX_BUF_NR_SET, NULL},
#endif
	{"ptTxBufSts", "set",   SW_API_QOS_PT_TX_BUF_ST_SET, NULL},
	{"ptRedEn", "set",   SW_API_QOS_PT_RED_EN_SET, NULL},
#ifdef ISISC
	{"ptTxBufNr", "set",   SW_API_QOS_PT_TX_BUF_NR_SET, NULL},
#else
	{"ptTxBufNr", "set", SW_API_QOS_PT_TX_BUF_NR_SET, NULL},
#endif
#ifdef ISISC
	{"ptRxBufNr", "set",   SW_API_QOS_PT_RX_BUF_NR_SET, NULL},
#else
	{"ptRxBufNr", "set", SW_API_QOS_PT_RX_BUF_NR_SET, NULL},
#endif
#ifndef ISISC
	{"up2q", "set", SW_API_COSMAP_UP_QU_SET, NULL},
	{"dscp2q", "set", SW_API_COSMAP_DSCP_QU_SET, NULL},
#endif
#endif
#ifdef ISISC
	{"ptMode", "set",   SW_API_QOS_PT_MODE_SET, NULL},
	{"ptModePri", "set",   SW_API_QOS_PT_MODE_PRI_SET, NULL},
#else
	{"ptMode", "set", SW_API_QOS_PT_MODE_SET, NULL},
	{"ptModePri", "set", SW_API_QOS_PT_MODE_PRI_SET, NULL},
#endif
#ifndef IN_QOS_MINI
#ifndef ISISC
	{"ptDefaultUp", "set",   SW_API_QOS_PORT_DEF_UP_SET, NULL},
#endif
	{"ptschMode", "set",   SW_API_QOS_PORT_SCH_MODE_SET, NULL},
	{"ptDefaultSpri", "set",   SW_API_QOS_PT_DEF_SPRI_SET, NULL},
	{"ptDefaultCpri", "set",   SW_API_QOS_PT_DEF_CPRI_SET, NULL},
	{"ptFSpriSts", "set",   SW_API_QOS_PT_FORCE_SPRI_ST_SET, NULL},
	{"ptFCpriSts", "set",   SW_API_QOS_PT_FORCE_CPRI_ST_SET, NULL},
	{"ptQuRemark", "set",   SW_API_QOS_QUEUE_REMARK_SET, NULL},
	{"ptgroup", "set", SW_API_QOS_PORT_GROUP_SET, NULL},
	{"ptpriprece", "set", SW_API_QOS_PORT_PRI_SET, NULL},
	{"ptremark", "set", SW_API_QOS_PORT_REMARK_SET, NULL},
	{"pcpmap", "set", SW_API_QOS_PCP_MAP_SET, NULL},
	{"flowmap", "set", SW_API_QOS_FLOW_MAP_SET, NULL},
	{"dscpmap", "set", SW_API_QOS_DSCP_MAP_SET, NULL},
	{"qscheduler", "set", SW_API_QOS_QUEUE_SCHEDULER_SET, NULL},
	{"ringqueue", "set", SW_API_QOS_RING_QUEUE_MAP_SET, NULL},
	{"dequeue", "set", SW_API_QOS_SCHEDULER_DEQUEU_CTRL_SET, NULL},
	{"portscheduler", "set", SW_API_QOS_PORT_SCHEDULER_CFG_RESET, NULL},
#endif
	{NULL, NULL,  0, NULL},/*end of desc*/
};
#endif

#ifdef IN_IGMP
struct sub_cmd_des_t g_igmp_des[] =
{
	{"mode", "set", SW_API_PT_IGMPS_MODE_SET, NULL},
	{"cmd", "set", SW_API_IGMP_MLD_CMD_SET, NULL},
	{"portJoin", "set", SW_API_IGMP_PT_JOIN_SET, NULL},
	{"portLeave", "set", SW_API_IGMP_PT_LEAVE_SET, NULL},
	{"rp", "set", SW_API_IGMP_RP_SET, NULL},
	{"createStatus", "set", SW_API_IGMP_ENTRY_CREAT_SET, NULL},
	{"static", "set", SW_API_IGMP_ENTRY_STATIC_SET, NULL},
	{"leaky", "set", SW_API_IGMP_ENTRY_LEAKY_SET, NULL},
	{"version3", "set", SW_API_IGMP_ENTRY_V3_SET, NULL},
	{"queue", "set", SW_API_IGMP_ENTRY_QUEUE_SET, NULL},
	{"ptlearnlimit", "set", SW_API_PT_IGMP_LEARN_LIMIT_SET, NULL},
	{"ptlearnexceedcmd", "set", SW_API_PT_IGMP_LEARN_EXCEED_CMD_SET, NULL},
	{"multi", "set", SW_API_IGMP_SG_ENTRY_SET, NULL},
	{"multi", "clear", SW_API_IGMP_SG_ENTRY_CLEAR, NULL},
	{"multi", "show", SW_API_IGMP_SG_ENTRY_SHOW, NULL},
	{NULL, NULL,  0, NULL},/*end of desc*/
};
#endif

#ifdef IN_LEAKY
struct sub_cmd_des_t g_leaky_des[] =
{
	{"ucMode", "set", SW_API_UC_LEAKY_MODE_SET, NULL},
	{"mcMode", "set", SW_API_MC_LEAKY_MODE_SET, NULL},
	{"arpMode", "set", SW_API_ARP_LEAKY_MODE_SET, NULL},
	{"ptUcMode", "set", SW_API_PT_UC_LEAKY_MODE_SET, NULL},
	{"ptMcMode", "set", SW_API_PT_MC_LEAKY_MODE_SET, NULL},
	{NULL, NULL,  0, NULL},/*end of desc*/
};
#endif

#ifdef IN_MIRROR
struct sub_cmd_des_t g_mirror_des[] =
{
	{"analyPt", "set", SW_API_MIRROR_ANALY_PT_SET, NULL},
	{"ptIngress", "set", SW_API_MIRROR_IN_PT_SET, NULL},
	{"ptEgress", "set", SW_API_MIRROR_EG_PT_SET, NULL},
	{"analyCfg", "set", SW_API_MIRROR_ANALYSIS_CONFIG_SET, NULL},
	{NULL, NULL,  0, NULL},/*end of desc*/
};
#endif

#ifdef IN_RATE
struct sub_cmd_des_t g_rate_des[] =
{
#ifndef ISISC
	{"qEgress", "set", SW_API_RATE_QU_EGRL_SET, NULL},
	{"ptEgress", "set", SW_API_RATE_PT_EGRL_SET, NULL},
	{"ptIngress", "set", SW_API_RATE_PT_INRL_SET, NULL},
	{"stormCtrl", "set", SW_API_STORM_CTRL_FRAME_SET, NULL},
	{"stormCtrlRate", "set", SW_API_STORM_CTRL_RATE_SET, NULL},
#endif
	{"portpolicer", "set", SW_API_RATE_PORT_POLICER_SET, NULL},
	{"portshaper", "set", SW_API_RATE_PORT_SHAPER_SET, NULL},
	{"queueshaper", "set", SW_API_RATE_QUEUE_SHAPER_SET, NULL},
	{"aclpolicer", "set", SW_API_RATE_ACL_POLICER_SET, NULL},
	{"ptAddRateByte", "set", SW_API_RATE_PT_ADDRATEBYTE_SET, NULL},
	{"ptgolflowen", "set", SW_API_RATE_PT_GOL_FLOW_EN_SET, NULL},
	{NULL, NULL,  0, NULL},/*end of desc*/
};
#endif

#ifdef IN_SEC
#ifdef ISISC
struct sub_cmd_des_t g_sec_des[] =
{
	{"mac", "set", SW_API_SEC_MAC_SET, NULL},
	{"ip", "set", SW_API_SEC_IP_SET, NULL},
	{"ip4", "set", SW_API_SEC_IP4_SET, NULL},
	{"ip6", "set", SW_API_SEC_IP6_SET, NULL},
	{"tcp", "set", SW_API_SEC_TCP_SET, NULL},
	{"udp", "set", SW_API_SEC_UDP_SET, NULL},
	{"icmp4", "set", SW_API_SEC_ICMP4_SET, NULL},
	{"icmp6", "set", SW_API_SEC_ICMP6_SET, NULL},
	{"l3parser", "set", SW_API_SEC_L3_PARSER_CTRL_SET, NULL},
	{"l4parser", "set", SW_API_SEC_L4_PARSER_CTRL_SET, NULL},
	{"expctrl", "set", SW_API_SEC_EXP_CTRL_SET, NULL},
	{NULL, NULL,  0, NULL},/*end of desc*/
};
#endif
#endif

#ifdef IN_STP
struct sub_cmd_des_t g_stp_des[] =
{
	{"portState", "set", SW_API_STP_PT_STATE_SET, NULL},
	{NULL, NULL,  0, NULL},/*end of desc*/
};
#endif

#ifdef IN_MIB
struct sub_cmd_des_t g_mib_des[] =
{
	{"status", "set",    SW_API_MIB_STATUS_SET, NULL},
            {"counters",  "flush",   SW_API_PT_MIB_FLUSH_COUNTERS, NULL},
            {"cpuKeep", "set",    SW_API_MIB_CPU_KEEP_SET, NULL},
            {NULL, NULL,  0, NULL},/*end of desc*/
};
#endif

#ifdef IN_LED
struct sub_cmd_des_t g_led_des[] =
{
	{"ctrlpattern", "set", SW_API_LED_PATTERN_SET, NULL},
	{NULL, NULL,  0, NULL},/*end of desc*/
};
#endif


#ifdef IN_COSMAP
struct sub_cmd_des_t g_cosmap_des[] =
{
#ifndef IN_COSMAP_MINI
	{"dscp2pri", "set",   SW_API_COSMAP_DSCP_TO_PRI_SET, NULL},
	{"dscp2dp", "set",   SW_API_COSMAP_DSCP_TO_DP_SET, NULL},
	{"up2pri", "set",   SW_API_COSMAP_UP_TO_PRI_SET, NULL},
	{"up2dp", "set",   SW_API_COSMAP_UP_TO_DP_SET, NULL},
	{"dscp2ehpri", "set",   SW_API_COSMAP_DSCP_TO_EHPRI_SET, NULL},
	{"dscp2ehdp", "set",   SW_API_COSMAP_DSCP_TO_EHDP_SET, NULL},

	{"up2ehpri", "set",   SW_API_COSMAP_UP_TO_EHPRI_SET, NULL},
	{"up2ehdp", "set",   SW_API_COSMAP_UP_TO_EHDP_SET, NULL},
#endif
	{"pri2q", "set",   SW_API_COSMAP_PRI_TO_QU_SET, NULL},
	{"pri2ehq", "set",   SW_API_COSMAP_PRI_TO_EHQU_SET, NULL},
#ifndef IN_COSMAP_MINI
	{"egRemark", "set",   SW_API_COSMAP_EG_REMARK_SET, NULL},
#endif
	{NULL, NULL,  0, NULL},/*end of desc*/
};
#endif

#ifdef IN_MISC
struct sub_cmd_des_t g_misc_des[] =
{
#ifndef ISISC
	{"arp", "set",   SW_API_ARP_STATUS_SET, NULL},
#endif
	{"frameMaxSize", "set",   SW_API_FRAME_MAX_SIZE_SET, NULL},
#ifndef ISISC
	{"ptUnkSaCmd", "set", SW_API_PT_UNK_SA_CMD_SET, NULL},
#endif
	{"ptUnkUcFilter", "set",   SW_API_PT_UNK_UC_FILTER_SET, NULL},
	{"ptUnkMcFilter", "set",   SW_API_PT_UNK_MC_FILTER_SET, NULL},
	{"ptBcFilter", "set",   SW_API_PT_BC_FILTER_SET, NULL},
	{"cpuPort", "set",   SW_API_CPU_PORT_STATUS_SET, NULL},
#ifndef ISISC
	{"bctoCpu", "set", SW_API_BC_TO_CPU_PORT_SET, NULL},
#endif
#ifndef IN_MISC_MINI
	{"ptDhcp", "set",   SW_API_PT_DHCP_SET, NULL},
#ifdef ISISC
	{"arpcmd", "set",   SW_API_ARP_CMD_SET, NULL},
#else
	{"arpcmd", "set", SW_API_ARP_CMD_SET, NULL},
#endif
#endif
	{"eapolcmd", "set",   SW_API_EAPOL_CMD_SET, NULL},
	{"eapolstatus", "set",   SW_API_EAPOL_STATUS_SET, NULL},
#ifndef IN_MISC_MINI
	{"rip", "set",   SW_API_RIPV1_STATUS_SET, NULL},
	{"ptarpreq", "set",   SW_API_PT_ARP_REQ_STATUS_SET, NULL},
	{"ptarpack", "set",   SW_API_PT_ARP_ACK_STATUS_SET, NULL},
	{"intrmask", "set",   SW_API_INTR_MASK_SET, NULL},
	{"intrstatus", "clear",   SW_API_INTR_STATUS_CLEAR, NULL},
	{"intrportlinkmask", "set",   SW_API_INTR_PORT_LINK_MASK_SET, NULL},
	{"intrmaskmaclinkchg", "set",   SW_API_INTR_MASK_MAC_LINKCHG_SET, NULL},
	{"intrstatusmaclinkchg", "clear",   SW_API_INTR_STATUS_MAC_LINKCHG_CLEAR, NULL},
	{"cpuVid", "set",   SW_API_CPU_VID_EN_SET, NULL},
	{"glomacaddr", "set",   SW_API_GLOBAL_MACADDR_SET, NULL},
	{"lldp", "set",   SW_API_LLDP_STATUS_SET, NULL},
	{"framecrc", "set",   SW_API_FRAME_CRC_RESERVE_SET, NULL},
#endif
#ifdef IN_PPPOE
	{"pppoesession", "add", SW_API_PPPOE_SESSION_ADD, NULL},
	{"pppoesession", "del", SW_API_PPPOE_SESSION_DEL, NULL},
	{"extendpppoe", "set",   SW_API_PPPOE_SESSION_TABLE_ADD, NULL},
	{"extendpppoe", "add",   SW_API_PPPOE_SESSION_TABLE_ADD, NULL},
	{"extendpppoe", "del",   SW_API_PPPOE_SESSION_TABLE_DEL, NULL},
	{"pppoeid", "set",   SW_API_PPPOE_SESSION_ID_SET, NULL},
	{"PppoeCmd", "set",   SW_API_PPPOE_CMD_SET, NULL},
	{"PppoeCmd", "set",   SW_API_PPPOE_CMD_SET, NULL},
	{"rtdPppoe", "set",   SW_API_RTD_PPPOE_EN_SET, NULL},
	{"Pppoe", "set",   SW_API_PPPOE_STATUS_SET, NULL},
	{"pppoeen", "set", SW_API_PPPOE_EN_SET, NULL},
#endif
	{NULL, NULL,  0, NULL},/*end of desc*/
};
#endif

#ifdef IN_IP
struct sub_cmd_des_t g_ip_des[] =
{
#ifndef IN_IP_MINI
	{"hostentry", "set", SW_API_IP_HOST_ADD, NULL},
	{"hostentry", "add", SW_API_IP_HOST_ADD, NULL},
	{"hostentry", "del", SW_API_IP_HOST_DEL, NULL},
	{"hostentry", "next", SW_API_IP_HOST_NEXT, NULL},
	{"hostentry", "bindcnt", SW_API_IP_HOST_COUNTER_BIND, NULL},
	{"hostentry", "bindpppoe", SW_API_IP_HOST_PPPOE_BIND, NULL},
	{"ptarplearn", "set", SW_API_IP_PT_ARP_LEARN_SET, NULL},
	{"arplearn",   "set", SW_API_IP_ARP_LEARN_SET, NULL},
	{"ptipsrcguard",   "set", SW_API_IP_SOURCE_GUARD_SET, NULL},
	{"ptarpsrcguard",  "set", SW_API_IP_ARP_GUARD_SET, NULL},
	{"routestatus", "set", SW_API_IP_ROUTE_STATUS_SET, NULL},
	{"intfentry", "set", SW_API_IP_INTF_ENTRY_ADD, NULL},
	{"intfentry", "add", SW_API_IP_INTF_ENTRY_ADD, NULL},
	{"intfentry", "del", SW_API_IP_INTF_ENTRY_DEL, NULL},
	{"ipunksrc", "set", SW_API_IP_UNK_SOURCE_CMD_SET, NULL},
	{"arpunksrc", "set", SW_API_ARP_UNK_SOURCE_CMD_SET, NULL},
	{"ipagetime", "set", SW_API_IP_AGE_TIME_SET, NULL},
	{"wcmphashmode", "set", SW_API_WCMP_HASH_MODE_SET, NULL},
	{"wcmpentry", "set", SW_API_IP_WCMP_ENTRY_SET, NULL},
	{"vrfbaseaddr", "set", SW_API_IP_VRF_BASE_ADDR_SET, NULL},
	{"vrfbasemask", "set", SW_API_IP_VRF_BASE_MASK_SET, NULL},
	{"defaultroute", "set", SW_API_IP_DEFAULT_ROUTE_SET, NULL},
	{"hostroute", "set", SW_API_IP_HOST_ROUTE_SET, NULL},
	{"rfsip4", "set", SW_API_IP_RFS_IP4_SET, NULL},
	{"rfsip6", "set", SW_API_IP_RFS_IP6_SET, NULL},
	{"defaultflowcmd", "set", SW_API_IP_DEFAULT_FLOW_CMD_SET, NULL},
	{"defaultrtflowcmd", "set", SW_API_IP_DEFAULT_RT_FLOW_CMD_SET, NULL},
	{"vsiarpsg", "set",  SW_API_IP_VIS_ARP_SG_CFG_SET, NULL},
	{"networkroute", "set",  SW_API_IP_NETWORK_ROUTE_ADD, NULL},
	{"networkroute", "add",  SW_API_IP_NETWORK_ROUTE_ADD, NULL},
	{"intf", "set",  SW_API_IP_INTF_SET, NULL},
	{"vsiintf", "set",  SW_API_IP_VSI_INTF_SET, NULL},
	{"portintf", "set",  SW_API_IP_PORT_INTF_SET, NULL},
	{"nexthop", "set",  SW_API_IP_NEXTHOP_SET, NULL},
	{"portsg", "set",  SW_API_IP_PORT_SG_SET, NULL},
	{"vsisg", "set",  SW_API_IP_VSI_SG_SET, NULL},
	{"pubip", "set",  SW_API_IP_PUB_IP_SET, NULL},
	{"portmac", "set",  SW_API_IP_PORT_MAC_SET, NULL},
	{"routemiss", "set",  SW_API_IP_ROUTE_MISS_SET, NULL},
	{"portarpsg", "set",  SW_API_IP_PORT_ARP_SG_SET, NULL},
	{"mcmode", "set",  SW_API_IP_VSI_MC_MODE_SET, NULL},
	{"globalctrl", "set",  SW_API_GLOBAL_CTRL_SET, NULL},
#endif
	{NULL, NULL,  0, NULL},/*end of desc*/
};
#endif

#ifdef IN_FLOW
struct sub_cmd_des_t g_flow_des[] =
{
	{"status", "set", SW_API_FLOW_STATUS_SET, NULL},
	{"agetime", "set", SW_API_FLOW_AGE_TIMER_SET, NULL},
	{"mgmt", "set", SW_API_FLOW_CTRL_SET, NULL},
	{"entry", "add", SW_API_FLOW_ENTRY_ADD, NULL},
	{"entry", "set", SW_API_FLOW_ENTRY_ADD, NULL},
	{"entry", "del", SW_API_FLOW_ENTRY_DEL, NULL},
	{"host", "set", SW_API_FLOW_HOST_ADD, NULL},
	{"host", "add", SW_API_FLOW_HOST_ADD, NULL},
	{"host", "del", SW_API_FLOW_HOST_DEL, NULL},
	{"global", "set", SW_API_FLOW_GLOBAL_CFG_SET, NULL},
	{NULL, NULL,  0, NULL},/*end of desc*/
};
#endif

#ifdef IN_NAT
struct sub_cmd_des_t g_nat_des[] =
{
	{"natentry", "set", SW_API_NAT_ADD, NULL},
	{"natentry", "add", SW_API_NAT_ADD, NULL},
	{"natentry", "del", SW_API_NAT_DEL, NULL},
	{"natentry", "next", SW_API_NAT_NEXT, NULL},
	{"natentry", "bindcnt", SW_API_NAT_COUNTER_BIND, NULL},
	{"naptentry", "set", SW_API_NAPT_ADD, NULL},
	{"naptentry", "add", SW_API_NAPT_ADD, NULL},
	{"naptentry", "del", SW_API_NAPT_DEL, NULL},
	{"naptentry", "next", SW_API_NAPT_NEXT, NULL},
	{"naptentry", "bindcnt", SW_API_NAPT_COUNTER_BIND, NULL},
	{"natstatus", "set", SW_API_NAT_STATUS_SET, NULL},
	{"naptstatus", "set", SW_API_NAPT_STATUS_SET, NULL},
	{"nathash", "set", SW_API_NAT_HASH_MODE_SET, NULL},
	{"naptmode", "set", SW_API_NAPT_MODE_SET, NULL},
	{"prvbaseaddr", "set", SW_API_PRV_BASE_ADDR_SET, NULL},
	{"prvaddrmode", "set", SW_API_PRV_ADDR_MODE_SET, NULL},
	{"pubaddr", "set", SW_API_PUB_ADDR_ENTRY_ADD, NULL},
	{"pubaddr", "add", SW_API_PUB_ADDR_ENTRY_ADD, NULL},
	{"pubaddr", "del", SW_API_PUB_ADDR_ENTRY_DEL, NULL},
	{"natunksess", "set", SW_API_NAT_UNK_SESSION_CMD_SET, NULL},
	{"prvbasemask", "set", SW_API_PRV_BASE_MASK_SET, NULL},
	{"global", "set", SW_API_NAT_GLOBAL_SET, NULL},
	{"flowentry", "set", SW_API_FLOW_ADD, NULL},
	{"flowentry", "add", SW_API_FLOW_ADD, NULL},
	{"flowentry", "del", SW_API_FLOW_DEL, NULL},
	{"flowentry", "next", SW_API_FLOW_NEXT, NULL},
	{"flowcookie", "set", SW_API_FLOW_COOKIE_SET, NULL},
	{"flowrfs", "set", SW_API_FLOW_RFS_SET, NULL},
	{NULL, NULL,  0, NULL},/*end of desc*/
};
#endif

#ifdef IN_TRUNK
struct sub_cmd_des_t g_trunk_des[] =
{
	{"group", "set", SW_API_TRUNK_GROUP_SET, NULL},
	{"hashmode", "set", SW_API_TRUNK_HASH_SET, NULL},
	{"mansa", "set", SW_API_TRUNK_MAN_SA_SET, NULL},
	{"failover", "set", SW_API_TRUNK_FAILOVER_EN_SET, NULL},
	{NULL, NULL,  0, NULL},/*end of desc*/
};
#endif

#ifdef IN_INTERFACECONTROL
struct sub_cmd_des_t g_interfacecontrol_des[] =
{
	{"macmode", "set",   SW_API_MAC_MODE_SET, NULL},
	{"pt3azstatus", "set",   SW_API_PORT_3AZ_STATUS_SET, NULL},
	{"phymode", "set",   SW_API_PHY_MODE_SET, NULL},
	{"fx100ctrl", "set",   SW_API_FX100_CTRL_SET, NULL},
	{"mac06exch", "set",   SW_API_MAC06_EXCH_SET, NULL},
	{NULL, NULL,  0, NULL},/*end of desc*/
};
#endif

#ifdef IN_VSI
struct sub_cmd_des_t g_vsi_des[] =
{
	{"vsi", "alloc", SW_API_VSI_ALLOC, NULL},
	{"vsi", "free", SW_API_VSI_FREE, NULL},
	{"portbasedvsi", "set", SW_API_PORT_VSI_SET, NULL},
	{"vlanbasedvsi", "set", SW_API_PORT_VLAN_VSI_SET, NULL},
	{"learnctrl", "set", SW_API_VSI_NEWADDR_LRN_SET, NULL},
	{"stationmove", "set", SW_API_VSI_STAMOVE_SET, NULL},
	{"member", "set", SW_API_VSI_MEMBER_SET, NULL},
    {"counter", "cleanup", SW_API_VSI_COUNTER_CLEANUP, NULL},
	{NULL, NULL, 0, NULL}/*end of desc*/
};
#endif

#ifdef IN_POLICER
struct sub_cmd_des_t g_policer_des[] =
{
	{"timeslot", "set",   SW_API_POLICER_TIMESLOT_SET, NULL},
	{"fcscompensation", "set",   SW_API_POLICER_COMPENSATION_SET, NULL},
	{"portentry", "set",   SW_API_POLICER_PORT_ENTRY_SET, NULL},
	{"aclentry", "set",   SW_API_POLICER_ACL_ENTRY_SET, NULL},
	{"bypass", "set",  SW_API_POLICER_BYPASS_EN_SET, NULL},
	{NULL, NULL,  0, NULL},/*end of desc*/
};
#endif
#ifdef IN_SHAPER
struct sub_cmd_des_t g_shaper_des[] =
{
	{"porttimeslot", "set",   SW_API_PORT_SHAPER_TIMESLOT_SET, NULL},
	{"flowtimeslot", "set",   SW_API_FLOW_SHAPER_TIMESLOT_SET, NULL},
	{"queuetimeslot", "set",	 SW_API_QUEUE_SHAPER_TIMESLOT_SET, NULL},
	{"porttoken", "set",   SW_API_PORT_SHAPER_TOKEN_NUMBER_SET, NULL},
	{"flowtoken", "set",   SW_API_FLOW_SHAPER_TOKEN_NUMBER_SET, NULL},
	{"queuetoken", "set",   SW_API_QUEUE_SHAPER_TOKEN_NUMBER_SET, NULL},
	{"portshaper", "set",   SW_API_PORT_SHAPER_SET, NULL},
	{"flowshaper", "set",   SW_API_FLOW_SHAPER_SET, NULL},
	{"queueshaper", "set",   SW_API_QUEUE_SHAPER_SET, NULL},
	{"ipgcompensation", "set",   SW_API_SHAPER_IPG_PRE_SET, NULL},
	{NULL, NULL,  0, NULL},/*end of desc*/
};
#endif
    /*QM*/
#ifdef IN_QM
struct sub_cmd_des_t g_qm_des[] =
{

    {"ucastqbase", "set", SW_API_UCAST_QUEUE_BASE_PROFILE_SET, NULL},
    {"ucastpriclass", "set", SW_API_UCAST_PRIORITY_CLASS_SET, NULL},
    {"mcastpriclass", "set", SW_API_MCAST_PRIORITY_CLASS_SET, NULL},
    {"queue", "flush", SW_API_QUEUE_FLUSH, NULL},
    {"queue", "set", SW_API_QUEUE_FLUSH, NULL},
    {"ucasthash", "set", SW_API_UCAST_HASH_MAP_SET, NULL},
    {"ucastdflthash", "set", SW_API_UCAST_DFLT_HASH_MAP_SET, NULL},
    {"mcastcpucode", "set", SW_API_MCAST_CPUCODE_CLASS_SET, NULL},
    {"acctrl", "set", SW_API_AC_CTRL_SET, NULL},
    {"acprebuffer", "set", SW_API_AC_PRE_BUFFER_SET, NULL},
    {"acqgroup", "set", SW_API_QUEUE_GROUP_SET, NULL},
    {"acstaticthresh", "set", SW_API_STATIC_THRESH_SET, NULL},
    {"acdynamicthresh", "set", SW_API_DYNAMIC_THRESH_SET, NULL},
    {"acgroupbuff", "set", SW_API_GOURP_BUFFER_SET, NULL},
    {"cntctrl", "set", SW_API_QUEUE_CNT_CTRL_SET, NULL},
    {"cnt", "cleanup", SW_API_QUEUE_CNT_CLEANUP, NULL},
    {"cnt", "set", SW_API_QUEUE_CNT_CLEANUP, NULL},
    {"enqueue", "set", SW_API_QM_ENQUEUE_CTRL_SET, NULL},
    {"srcprofile", "set", SW_API_QM_SOURCE_PROFILE_SET, NULL},
    {NULL, NULL, 0, NULL},/*end of desc*/

};
#endif

/*BM*/
#ifdef IN_BM
struct sub_cmd_des_t g_bm_des[] =
{

    {"ctrl", "set", SW_API_BM_CTRL_SET, NULL},
    {"portgroupmap", "set", SW_API_BM_PORTGROUP_MAP_SET, NULL},
    {"groupbuff", "set", SW_API_BM_GROUP_BUFFER_SET, NULL},
    {"portrsvbuff", "set", SW_API_BM_PORT_RSVBUFFER_SET, NULL},
    {"portsthresh", "set", SW_API_BM_STATIC_THRESH_SET, NULL},
    {"portdthresh", "set", SW_API_BM_DYNAMIC_THRESH_SET, NULL},
    {NULL, NULL, 0, NULL},/*end of desc*/

};
#endif

/*Servcode*/
#ifdef IN_SERVCODE
struct sub_cmd_des_t g_servcode_des[] =
{
    {"config", "set", SW_API_SERVCODE_CONFIG_SET, NULL},
    {"loopcheck", "set", SW_API_SERVCODE_LOOPCHECK_EN, NULL},
    {NULL, NULL, 0, NULL},/*end of desc*/
};
#endif

/*rss hash*/
#ifdef IN_RSS_HASH
struct sub_cmd_des_t g_rss_hash_des[] =
{
    {"config", "set", SW_API_RSS_HASH_CONFIG_SET, NULL},
    {NULL, NULL, 0, NULL},/*end of desc*/
};
#endif

/*Ctrlpkt*/
#ifdef IN_CTRLPKT
struct sub_cmd_des_t g_ctrlpkt_des[] =
{
    {"ethernetType", "set", SW_API_MGMTCTRL_ETHTYPE_PROFILE_SET, NULL},
    {"rfdb", "set", SW_API_MGMTCTRL_RFDB_PROFILE_SET, NULL},
    {"appProfile", "set", SW_API_MGMTCTRL_CTRLPKT_PROFILE_ADD, NULL},
    {NULL, NULL, 0, NULL},/*end of desc*/
};
#endif

struct sub_cmd_des_t g_debug_des[] =
{
	{"module_func", "set", SW_API_MODULE_FUNC_CTRL_SET, NULL},
	{NULL, NULL, 0, NULL},/*end of desc*/
};

struct cmd_des_t gcmd_des[] =
{
    /*port ctrl*/
#ifdef IN_PORTCONTROL
    {
        "port", g_port_des,
    },
#endif

    /*vlan*/
#ifdef IN_VLAN
    {
        "vlan", g_vlan_des,
    },
#endif

    /*portvlan*/
#ifdef IN_PORTVLAN
    {
        "portVlan", g_portvlan_des,
    },
#endif

    /*fdb*/
#ifdef IN_FDB
    {
        "fdb", g_fdb_des,
    },
#endif

    /*acl*/
#ifdef IN_ACL
    {
        "acl", g_acl_des,
    },
#endif

    /*qos*/
#ifdef IN_QOS
    {
        "qos", g_qos_des,
    },
#endif

    /*igmp*/
#ifdef IN_IGMP
    {
        "igmp", g_igmp_des,
    },
#endif

    /*leaky*/
#ifdef IN_LEAKY
    {
        "leaky", g_leaky_des,
    },
#endif

    /*mirror*/
#ifdef IN_MIRROR
    {
        "mirror", g_mirror_des,
    },
#endif

    /*rate*/
#ifdef IN_RATE
    {
        "rate", g_rate_des,
    },
#endif

#ifdef IN_SEC
#ifdef ISISC
    {
        "sec", g_sec_des,
    },
#endif
#endif

    /*stp*/
#ifdef IN_STP
    {
        "stp", g_stp_des,
    },
#endif

    /*mib*/
#ifdef IN_MIB
    {
        "mib", g_mib_des,
    },
#endif

    /* led */
#ifdef IN_LED
    {
        "led", g_led_des,
    },
#endif

    /* cosmap */
#ifdef IN_COSMAP
    {
        "cosmap", g_cosmap_des,
    },
#endif

    /*misc*/
#ifdef IN_MISC
    {
        "misc", g_misc_des,
    },
#endif

    /* IP */
#ifdef IN_IP
    {
        "ip", g_ip_des,
    },
#endif

    /* FLOW */
#ifdef IN_FLOW
    {
        "flow", g_flow_des,
    },
#endif

    /* NAT */
#ifdef IN_NAT
    {
        "nat", g_nat_des,
    },
#endif

    /*Trunk*/
#ifdef IN_TRUNK
    {
        "trunk", g_trunk_des,
    },
#endif

    /*Interface Control*/
#ifdef IN_INTERFACECONTROL
    {
        "interface", g_interfacecontrol_des,
    },
#endif

#ifdef IN_VSI
    {
        "vsi", g_vsi_des,
    },
#endif

	/*policer*/
#ifdef IN_POLICER
		{
			"policer", g_policer_des,
		},
#endif

     /*shaper*/
#ifdef IN_SHAPER
    {
        "shaper", g_shaper_des,
    },
#endif
        /*qm Control*/
#ifdef IN_QM
    {
        "qm", g_qm_des,
    },
#endif

	/*bm Control*/
#ifdef IN_BM
    {
        "bm", g_bm_des,
    },
#endif

#ifdef IN_SERVCODE
    {
        "servcode", g_servcode_des,
    },
#endif

#ifdef IN_RSS_HASH
    {
        "rsshash", g_rss_hash_des,
    },
#endif

#ifdef IN_CTRLPKT
    {
        "ctrlpkt", g_ctrlpkt_des,
    },
#endif
    {
	"debug", g_debug_des,
    },

    {NULL, NULL} /*end of desc*/
};


#endif


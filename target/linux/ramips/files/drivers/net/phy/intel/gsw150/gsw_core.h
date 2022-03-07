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


#ifndef _GSWITCH_CORE_H_
#define _GSWITCH_CORE_H_

#define PORT_STATE_LISTENING 0
#define PS_RENABLE_TDISABLE	1
#define PS_RDISABLE_TENABLE	2
#define PORT_STATE_LEARNING 4
#define PORT_STATE_FORWARDING 7
#define REX_TFLOW_CNT_1	0x28

#define MAX_PORT_NUMBER 7
#define VLAN_ACTIVE_TABLE_SIZE 64
#define MC_PC_SIZE 64
#define GSW_2X_SOC_CPU_PORT 6
#define GSW_MEM_SEGMENT 256
#define VRX_PLATFORM_CAP_FID 64
#define RMON_COUNTER_OFFSET 64
#define GSW_TREG_OFFSET 0xF400
#define GSW_XMII_OFFSET 0xF100
#define MAX_PACKET_LENGTH 9600
#define PKG_LNG_TSIZE 16
#define DASA_MAC_TBL_SIZE 64
#define PCE_APPL_TBL_SIZE 64
#define IP_DASA_MSB_SIZE 16
#define IP_DASA_LSB_SIZE 64
#define PCE_PTCL_TBL_SIZE 32
#define PCE_PPPOE_TBL_SIZE 16
#define PCE_VLAN_ACT_TBL_SIZE 64
#define PCE_TABLE_SIZE 64
#define PCE_MICRO_TABLE_SIZE 64
/* Pore redirect PCE rules set or port 0 (30), port 1 (31),*/
/*	port 2 (32), port 3(33), port 4(34) and port 5(35) */
#define PRD_PRULE_INDEX 30
#define EAPOL_PCE_RULE_INDEX	60
#define BPDU_PCE_RULE_INDEX 61
#define MPCE_RULES_INDEX	10
#define LTQ_GSWIP_2_0 0x100
#define LTQ_GSWIP_2_1 0x021
#define LTQ_GSWIP_2_2 0x122
#define LTQ_GSWIP_2_2_ETC	0x022
#define LTQ_GSWIP_3_0 0x030


/*PHY Reg 0x4 */
#define PHY_AN_ADV_10HDX 0x20
#define PHY_AN_ADV_10FDX 0x40
#define PHY_AN_ADV_100HDX 0x80
#define PHY_AN_ADV_100FDX 0x100
/*PHY Reg 0x9 */
#define PHY_AN_ADV_1000HDX 0x100
#define PHY_AN_ADV_1000FDX 0x200

#define DEFAULT_AGING_TIMEOUT	300
/* Define Aging Counter Mantissa Value */
#define AGETIMER_1_DAY 0xFB75
#define AGETIMER_1_HOUR 0xA7BA
#define AGETIMER_300_SEC 0xDF84
#define AGETIMER_10_SEC 0x784
#define AGETIMER_1_SEC 0xBF

/* Define Duplex Mode */
#define DUPLEX_AUTO 0
#define DUPLEX_FULL 1
#define DUPLEX_HALF 3

#define INSTR 0
#define IPV6 1
#define LENACCU 2

/* parser's microcode output field type */
enum {
	OUT_MAC0 = 0,
	OUT_MAC1,
	OUT_MAC2,
	OUT_MAC3,
	OUT_MAC4,
	OUT_MAC5,
	OUT_ITAG0,
	OUT_ITAG1,
	OUT_ITAG2,
	OUT_ITAG3,
	OUT_1VTAG0, /* 10 */
	OUT_1VTAG1,
	OUT_2VTAG0,
	OUT_2VTAG1,
	OUT_3VTAG0,
	OUT_3VTAG1,
	OUT_4VTAG0,
	OUT_4VTAG1,
	OUT_ETYPE,
	OUT_PPPOE0,
	OUT_PPPOE1, /* 20 */
	OUT_PPPOE3,
	OUT_PPP,
	OUT_RES,
	OUT_1IP0,
	OUT_1IP1,
	OUT_1IP2,
	OUT_1IP3,
	OUT_1IP4,
	OUT_1IP5,
	OUT_1IP6, /* 30 */
	OUT_1IP7,
	OUT_1IP8,
	OUT_1IP9,
	OUT_1IP10,
	OUT_1IP11,
	OUT_1IP12,
	OUT_1IP13,
	OUT_1IP14,
	OUT_1IP15,
	OUT_1IP16, /* 40 */
	OUT_1IP17,
	OUT_1IP18,
	OUT_1IP19,
	OUT_2IP0,
	OUT_2IP1,
	OUT_2IP2,
	OUT_2IP3,
	OUT_2IP4,
	OUT_2IP5,
	OUT_2IP6, /* 50 */
	OUT_2IP7,
	OUT_2IP8,
	OUT_2IP9,
	OUT_2IP10,
	OUT_2IP11,
	OUT_2IP12,
	OUT_2IP13,
	OUT_2IP14,
	OUT_2IP15,
	OUT_2IP16, /* 60 */
	OUT_2IP17,
	OUT_2IP18,
	OUT_2IP19,
	OUT_APP0,
	OUT_APP1,
	OUT_APP2,
	OUT_APP3,
	OUT_APP4,
	OUT_APP5,
	OUT_APP6, /* 70 */
	OUT_APP7,
	OUT_APP8,
	OUT_APP9,
	OUT_1PL,
	OUT_2PL,
	OUT_1LNH,
	OUT_2LNH = 77,
	OUT_NONE = 127
};

/* parser's microcode flag type */
enum {
	FLAG_NO = 0,
	FLAG_END,
	FLAG_CAPWAP,
	FLAG_GRE,
	FLAG_LEN,
	FLAG_NN0,
	FLAG_NN1,
	FLAG_NN2,
	FLAG_ITAG,
	FLAG_1VLAN,
	FLAG_2VLAN,  /* 10 */
	FLAG_3VLAN,
	FLAG_4VLAN,
	FLAG_SNAP,
	FLAG_PPPOES,
	FLAG_1IPV4,
	FLAG_1IPV6,
	FLAG_2IPV4,
	FLAG_2IPV6,
	FLAG_ROUTEXP,
	FLAG_TCP,  /* 20 */
	FLAG_1UDP,
	FLAG_IGMP,
	FLAG_IPV4OPT,
	FLAG_1IPV6EXT,
	FLAG_TCPACK,
	FLAG_IPFRAG,
	FLAG_EAPOL,
	FLAG_2IPV6EXT,
	FLAG_2UDP,
	FLAG_L2TPNEXP,  /* 30 */
	FLAG_LROEXP,
	FLAG_L2TP,
	FLAG_NN4,
	FLAG_NN5,
	FLAG_NN6,
	FLAG_NN7,
	FLAG_NN8,
	FLAG_NN9,
	FLAG_NN10,
	FLAG_NN11,  /* 40 */
	FLAG_NN12,
	FLAG_NN13,
	FLAG_NN14,
	FLAG_NN15,
	FLAG_NN16,
	FLAG_NN17,
	FLAG_NN18,
	FLAG_NN19,
	FLAG_NN20,
	FLAG_NN21,  /* 50 */
	FLAG_NN22,
	FLAG_NN23,
	FLAG_NN24,
	FLAG_NN25,
	FLAG_NN26,
	FLAG_NN27,
	FLAG_NN28,
	FLAG_NN29,
	FLAG_NN30,
	FLAG_NN31,  /* 60 */
	FLAG_NN32,
	FLAG_NN33,
	FLAG_NN34,
};

/* GSWIP_2.X*/
enum {
	GOUT_MAC0 = 0,
	GOUT_MAC1,
	GOUT_MAC2,
	GOUT_MAC3,
	GOUT_MAC4,
	GOUT_MAC5,
	GOUT_ETHTYP,
	GOUT_VTAG0,
	GOUT_VTAG1,
	GOUT_ITAG0,
	GOUT_ITAG1,	/*10 */
	GOUT_ITAG2,
	GOUT_ITAG3,
	GOUT_IP0,
	GOUT_IP1,
	GOUT_IP2,
	GOUT_IP3,
	GOUT_SIP0,
	GOUT_SIP1,
	GOUT_SIP2,
	GOUT_SIP3,	/*20*/
	GOUT_SIP4,
	GOUT_SIP5,
	GOUT_SIP6,
	GOUT_SIP7,
	GOUT_DIP0,
	GOUT_DIP1,
	GOUT_DIP2,
	GOUT_DIP3,
	GOUT_DIP4,
	GOUT_DIP5,	/*30*/
	GOUT_DIP6,
	GOUT_DIP7,
	GOUT_SESID,
	GOUT_PROT,
	GOUT_APP0,
	GOUT_APP1,
	GOUT_IGMP0,
	GOUT_IGMP1,
	GOUT_STAG0 = 61,
	GOUT_STAG1 = 62,
	GOUT_NONE	=	63,
};

/* parser's microcode flag type */
enum {
	GFLAG_ITAG = 0,
	GFLAG_VLAN,
	GFLAG_SNAP,
	GFLAG_PPPOE,
	GFLAG_IPV6,
	GFLAG_IPV6FL,
	GFLAG_IPV4,
	GFLAG_IGMP,
	GFLAG_TU,
	GFLAG_HOP,
	GFLAG_NN1,	/*10 */
	GFLAG_NN2,
	GFLAG_END,
	GFLAG_NO,	/*13*/
	GFLAG_SVLAN,  /*14 */
};

typedef struct {
	u16 val_3;
	u16 val_2;
	u16 val_1;
	u16 val_0;
} pce_uc_row_t;

typedef pce_uc_row_t PCE_MICROCODE[PCE_MICRO_TABLE_SIZE];

/** Provides the address of the configured/fetched lookup table. */
typedef enum {
	/** Parser microcode table */
	PCE_PARS_INDEX = 0x00,
	PCE_ACTVLAN_INDEX = 0x01,
	PCE_VLANMAP_INDEX = 0x02,
	PCE_PPPOE_INDEX = 0x03,
	PCE_PROTOCOL_INDEX = 0x04,
	PCE_APPLICATION_INDEX	= 0x05,
	PCE_IP_DASA_MSB_INDEX	= 0x06,
	PCE_IP_DASA_LSB_INDEX	= 0x07,
	PCE_PACKET_INDEX = 0x08,
	PCE_PCP_INDEX = 0x09,
	PCE_DSCP_INDEX = 0x0A,
	PCE_MAC_BRIDGE_INDEX	= 0x0B,
	PCE_MAC_DASA_INDEX = 0x0C,
	PCE_MULTICAST_SW_INDEX = 0x0D,
	PCE_MULTICAST_HW_INDEX = 0x0E,
	PCE_TFLOW_INDEX = 0x0F,
	PCE_REMARKING_INDEX = 0x10,
	PCE_QUEUE_MAP_INDEX = 0x11,
	PCE_METER_INS_0_INDEX	= 0x12,
	PCE_METER_INS_1_INDEX	= 0x13,
	PCE_IPDALSB_INDEX = 0x14,
	PCE_IPSALSB_INDEX = 0x15,
	PCE_MACDA_INDEX = 0x16,
	PCE_MACSA_INDEX = 0x17,
	PCE_PARSER_FLAGS_INDEX = 0x18,
	PCE_PARS_INDIVIDUAL_INDEX	= 0x19,
	PCE_SPCP_INDEX = 0x1A,
	PCE_MSTP_INDEX = 0x1B,
	PCE_EGREMARK_INDEX = 0x1C,
	PCE_PAYLOAD_INDEX = 0x1D,
	PCE_EG_VLAN_INDEX = 0x1E,
} ptbl_cmds_t;

/** Description */
typedef enum {
	PCE_OPMODE_ADRD = 0,
	PCE_OPMODE_ADWR = 1,
	PCE_OPMODE_KSRD = 2,
	PCE_OPMODE_KSWR = 3
} ptbl_opcode_t;

typedef struct {
	u16 key[16];
	u16 mask[4];
	u16 val[16];
	u16 table;
	u16 pcindex;
	u16 opmode:2;
	u16 extop:1;
	u16 kformat:1;
	u16 type:1;
	u16 valid:1;
	u16 group:4;
} pctbl_prog_t;

typedef enum {
	BM_OPMODE_RD = 0,
	BM_OPMODE_WR = 1
} bm_tbl_opcode_t;

typedef struct {
	u16 bmval[5];
	u16 bmtable;
	u16 bmindex;
	u16 bmopmode:2;
} bm_tbl_prog_t;

typedef struct {
	u16 pkt_len;
	u16 pkt_len_range;
	u8 pkt_valid;
}	pkt_len_t;

typedef struct {
	u8 mac[6];
	u16 mmask;
	u8 mac_valid;
}	mac_tbl_t;

typedef struct {
	u16 app_data;
	u16 app_mr_data;
	u8 app_mr_sel:2;
	u8 app_valid:1;
} app_tbl_t;

typedef struct {
	u16 payload_data;
	u16 mask_range;
	u8 mask_range_type:2;
	u8 valid:1;
} payload_tbl_t;

/* IP DA/SA MSB Table */
typedef struct {
	u8 ip_val[8];
	u16 ip_mask[4];
	u8 ip_valid;
} ip_dasa_t;

/* programme the Protocol Table */
typedef struct {
	u16 protocol_type;
	u16 protocol_mask;
	u8 protocol_valid:1;
} protocol_tb_t;

typedef struct {
	u16 act_vid;
	u8	avlan_valid;
} avlan_tbl_t;

/* PPPoE Table  */
typedef struct {
	u16	ppsoe_sid;
	u8 pppoe_valid:1;
} ppoe_tbl_t;

typedef struct {
	u16 pkt_len_idx:8;
	u16 dst_mac_idx:8;
	u16 src_mac_idx:8;
	u16 dst_appl_idx:8;
	u16 src_appl_idx:8;
	u16 dip_msb_idx:8;
	u16 dip_lsb_idx:8;
	u16 sip_msb_idx:8;
	u16 sip_lsb_idx:8;
	u16 ip_prot_idx:8;
	u16 ethertype_idx:8;
	u16 pppoe_idx:8;
	u16 vlan_idx:8;
	u16 svlan_idx:8;
} sub_tbl_t;

typedef struct {
	/* table reference counter */
	u8 trc_pkg_lng[PKG_LNG_TSIZE];
	u8 src_mac_cnt[DASA_MAC_TBL_SIZE];
	u8 dst_mac_cnt[DASA_MAC_TBL_SIZE];

	u8 appl_tbl_cnt[PCE_APPL_TBL_SIZE];
	u8 ipmsbtcnt[IP_DASA_MSB_SIZE];
	u8 iplsbtcnt[IP_DASA_LSB_SIZE];
	u8 ptcl_tbl_cnt[PCE_PTCL_TBL_SIZE];
	u8 pppoe_tbl_cnt[PCE_PPPOE_TBL_SIZE];
	u8 vlan_act_tbl_cnt[PCE_VLAN_ACT_TBL_SIZE];
	/* cached tables */
	pkt_len_t	ct_pkg_lng[PKG_LNG_TSIZE];
	mac_tbl_t ct_src_mac[DASA_MAC_TBL_SIZE];
	avlan_tbl_t	avlan_tbl[PCE_VLAN_ACT_TBL_SIZE];
	mac_tbl_t da_mac_tbl[DASA_MAC_TBL_SIZE];
	app_tbl_t appl_tbl[PCE_APPL_TBL_SIZE];
	ip_dasa_t ip_dasa_msb_tbl[IP_DASA_MSB_SIZE];
	ip_dasa_t ip_dasa_lsb_tbl[IP_DASA_LSB_SIZE];
	protocol_tb_t ptcl_tbl[PCE_PTCL_TBL_SIZE];
	ppoe_tbl_t pppoe_tbl[PCE_PPPOE_TBL_SIZE];
} pcetbl_prog_t;

typedef struct {
	/* Parameter for the sub-tables */
	pcetbl_prog_t tsub_tbl;
	sub_tbl_t sub_tbl[PCE_TABLE_SIZE];
	GSW_PCE_action_t	pce_act[PCE_TABLE_SIZE];
	u8 ptblused[PCE_TABLE_SIZE];
} tft_tbl_t;

typedef struct {
	GSW_capType_t captype;
	/* Description String */
	char desci[GSW_CAP_STRING_LEN];
} gsw_capdesc_t;

typedef struct {
	GSW_STP_PortState_t psstate /*ifx_stp_state*/;
	GSW_8021X_portState_t	ps8021x /*ifx_8021_state*/;
	u8	pen_reg;
	u8	pstate_reg;
	u8	lrnlim;
} pstpstate_t;

typedef struct {
	/* Port Enable */
	ltq_bool_t penable;
	u16 llimit;
	/* Port State */
	u16 ptstate;
	/* Port State for STP */
	GSW_STP_PortState_t	pcstate;
	/* Port State for 8021.x */
	GSW_8021X_portState_t	p8021xs;
} port_config_t;

typedef struct {
	/* 8021x Port Forwarding State */
	GSW_portForward_t sfport;
	/* STP port State */
	GSW_portForward_t	spstate;
	/* 8021X Forwarding Port ID*/
	u8	fpid8021x;
	/* STP Port ID */
	u16	stppid;
} stp8021x_t;

typedef struct {
	u16 smsbindex;
	u16 dmsbindex;
	u16 slsbindex;
	u16 dlsbindex;
	u16 pmap;
	u16 subifid;
	u8 fid;
	u16 mcmode;
	ltq_bool_t valid;
	ltq_bool_t exclude;
} mcsw_table_t;

typedef struct {
	u16	igmode;
	ltq_bool_t igv3;
	u16	igfport;
	u8	igfpid;
	ltq_bool_t igxvlan;
	u8	igcos;
	mcsw_table_t mctable[MC_PC_SIZE];
	u16	igrport;
	u8	itblsize;
} gsw_igmp_t;

typedef struct {
	ethsw_core_init_t *ecint;
	gsw_devtype_t edev;
	gsw_igmp_t iflag;
	tft_tbl_t ptft;
	u8 mdio_id;
	u16 mdio_addr;
	u8 pnum;
	u8 tpnum; /* Total number of ports including vitual ports*/
	u8 mpnum; /* ManagementPortNumber */
	u32 matimer;
	u16 mac_rd_index; /* read mac table index */
	u8 mhw_rinx;
	u8 msw_rinx;
	u8 cport;
	u8 gsw_dev;
	/* multicast router port count */
	u8 mrtpcnt;
	u8 meter_cnt;
	u8 nqueues; /* Number of priority queues . */
	u8 nmeters;  /* Number of traffic meters */
	u8 nshapers; /* Number of traffic shapers */
	u8 npppoe; /* PPPoE table size  */
	u8 avlantsz; /* Active VLAN table size */
	u8 ip_pkt_lnt_size; /* IP packet length table size */
	u8 prot_table_size; /* Protocol table size */
	u8 mac_dasa_table_size; /* MAC DA/SA table size */
	u8 app_table_size;	/* Application table size */
	u8 idsmtblsize;	/* IP DA/SA MSB table size */
	u8 idsltblsize;	/*  IP DA/SA LSB table size*/
	u8 mctblsize;	/* Multicast table size */
	u8 tftblsize; /* Flow Aggregation table size */
	u16 mactblsize; /* MAC bridging table size */
	u16 gipver;
} ethsw_api_dev_t;
#endif /* _GSWITCH_CORE_H_ */


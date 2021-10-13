/*
 * Copyright (c) 2016-2017, The Linux Foundation. All rights reserved.
 * Permission to use, copy, modify, and/or distribute this software for
 * any purpose with or without fee is hereby granted, provided that the
 * above copyright notice and this permission notice appear in all copies.
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT
 * OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */


/**
 * @defgroup
 * @{
 */
#ifndef HPPE_PORTVLAN_REG_H
#define HPPE_PORTVLAN_REG_H

/*[register] PORT_PARSING_REG*/
#define PORT_PARSING_REG
#define PORT_PARSING_REG_ADDRESS 0x0
#define PORT_PARSING_REG_NUM     8
#define PORT_PARSING_REG_INC     0x4
#define PORT_PARSING_REG_TYPE    REG_TYPE_RW
#define PORT_PARSING_REG_DEFAULT 0x0
	/*[field] PORT_ROLE*/
	#define PORT_PARSING_REG_PORT_ROLE
	#define PORT_PARSING_REG_PORT_ROLE_OFFSET  0
	#define PORT_PARSING_REG_PORT_ROLE_LEN     1
	#define PORT_PARSING_REG_PORT_ROLE_DEFAULT 0x0

struct port_parsing_reg {
	a_uint32_t  port_role:1;
	a_uint32_t  _reserved0:31;
};

union port_parsing_reg_u {
	a_uint32_t val;
	struct port_parsing_reg bf;
};

/*[register] EDMA_VLAN_TPID_REG*/
#define EDMA_VLAN_TPID_REG
#define EDMA_VLAN_TPID_REG_ADDRESS 0x8
#define EDMA_VLAN_TPID_REG_NUM     1
#define EDMA_VLAN_TPID_REG_INC     0x4
#define EDMA_VLAN_TPID_REG_TYPE    REG_TYPE_RW
#define EDMA_VLAN_TPID_REG_DEFAULT 0x810088a8
	/*[field] STAG_TPID*/
	#define EDMA_VLAN_TPID_REG_STAG_TPID
	#define EDMA_VLAN_TPID_REG_STAG_TPID_OFFSET  0
	#define EDMA_VLAN_TPID_REG_STAG_TPID_LEN     16
	#define EDMA_VLAN_TPID_REG_STAG_TPID_DEFAULT 0x88a8
	/*[field] CTAG_TPID*/
	#define EDMA_VLAN_TPID_REG_CTAG_TPID
	#define EDMA_VLAN_TPID_REG_CTAG_TPID_OFFSET  16
	#define EDMA_VLAN_TPID_REG_CTAG_TPID_LEN     16
	#define EDMA_VLAN_TPID_REG_CTAG_TPID_DEFAULT 0x8100

struct edma_vlan_tpid_reg {
	a_uint32_t  stag_tpid:16;
	a_uint32_t  ctag_tpid:16;
};

union edma_vlan_tpid_reg_u {
	a_uint32_t val;
	struct edma_vlan_tpid_reg bf;
};

/*[register] VLAN_TPID_REG*/
#define VLAN_TPID_REG
#define VLAN_TPID_REG_ADDRESS 0x20
#define VLAN_TPID_REG_NUM     1
#define VLAN_TPID_REG_INC     0x4
#define VLAN_TPID_REG_TYPE    REG_TYPE_RW
#define VLAN_TPID_REG_DEFAULT 0x88a88100
	/*[field] CTAG_TPID*/
	#define VLAN_TPID_REG_CTAG_TPID
	#define VLAN_TPID_REG_CTAG_TPID_OFFSET  0
	#define VLAN_TPID_REG_CTAG_TPID_LEN     16
	#define VLAN_TPID_REG_CTAG_TPID_DEFAULT 0x8100
	/*[field] STAG_TPID*/
	#define VLAN_TPID_REG_STAG_TPID
	#define VLAN_TPID_REG_STAG_TPID_OFFSET  16
	#define VLAN_TPID_REG_STAG_TPID_LEN     16
	#define VLAN_TPID_REG_STAG_TPID_DEFAULT 0x88a8

struct vlan_tpid_reg {
	a_uint32_t  ctag_tpid:16;
	a_uint32_t  stag_tpid:16;
};

union vlan_tpid_reg_u {
	a_uint32_t val;
	struct vlan_tpid_reg bf;
};

/*[register] BRIDGE_CONFIG*/
#define BRIDGE_CONFIG
#define BRIDGE_CONFIG_ADDRESS 0x0
#define BRIDGE_CONFIG_NUM     1
#define BRIDGE_CONFIG_INC     0x4
#define BRIDGE_CONFIG_TYPE    REG_TYPE_RW
#define BRIDGE_CONFIG_DEFAULT 0x0
	/*[field] BRIDGE_TYPE*/
	#define BRIDGE_CONFIG_BRIDGE_TYPE
	#define BRIDGE_CONFIG_BRIDGE_TYPE_OFFSET  0
	#define BRIDGE_CONFIG_BRIDGE_TYPE_LEN     1
	#define BRIDGE_CONFIG_BRIDGE_TYPE_DEFAULT 0x0

struct bridge_config {
	a_uint32_t  bridge_type:1;
	a_uint32_t  _reserved0:31;
};

union bridge_config_u {
	a_uint32_t val;
	struct bridge_config bf;
};

/*[register] PORT_DEF_VID*/
#define PORT_DEF_VID
#define PORT_DEF_VID_ADDRESS 0x10
#define PORT_DEF_VID_NUM     1
#define PORT_DEF_VID_INC     0x4
#define PORT_DEF_VID_TYPE    REG_TYPE_RW
#define PORT_DEF_VID_DEFAULT 0x0
	/*[field] PORT_DEF_SVID*/
	#define PORT_DEF_VID_PORT_DEF_SVID
	#define PORT_DEF_VID_PORT_DEF_SVID_OFFSET  0
	#define PORT_DEF_VID_PORT_DEF_SVID_LEN     12
	#define PORT_DEF_VID_PORT_DEF_SVID_DEFAULT 0x0
	/*[field] PORT_DEF_SVID_EN*/
	#define PORT_DEF_VID_PORT_DEF_SVID_EN
	#define PORT_DEF_VID_PORT_DEF_SVID_EN_OFFSET  12
	#define PORT_DEF_VID_PORT_DEF_SVID_EN_LEN     1
	#define PORT_DEF_VID_PORT_DEF_SVID_EN_DEFAULT 0x0
	/*[field] PORT_DEF_CVID*/
	#define PORT_DEF_VID_PORT_DEF_CVID
	#define PORT_DEF_VID_PORT_DEF_CVID_OFFSET  16
	#define PORT_DEF_VID_PORT_DEF_CVID_LEN     12
	#define PORT_DEF_VID_PORT_DEF_CVID_DEFAULT 0x0
	/*[field] PORT_DEF_CVID_EN*/
	#define PORT_DEF_VID_PORT_DEF_CVID_EN
	#define PORT_DEF_VID_PORT_DEF_CVID_EN_OFFSET  28
	#define PORT_DEF_VID_PORT_DEF_CVID_EN_LEN     1
	#define PORT_DEF_VID_PORT_DEF_CVID_EN_DEFAULT 0x0

struct port_def_vid {
	a_uint32_t  port_def_svid:12;
	a_uint32_t  port_def_svid_en:1;
	a_uint32_t  _reserved0:3;
	a_uint32_t  port_def_cvid:12;
	a_uint32_t  port_def_cvid_en:1;
	a_uint32_t  _reserved1:3;
};

union port_def_vid_u {
	a_uint32_t val;
	struct port_def_vid bf;
};

/*[register] PORT_DEF_PCP*/
#define PORT_DEF_PCP
#define PORT_DEF_PCP_ADDRESS 0x30
#define PORT_DEF_PCP_NUM     1
#define PORT_DEF_PCP_INC     0x4
#define PORT_DEF_PCP_TYPE    REG_TYPE_RW
#define PORT_DEF_PCP_DEFAULT 0x0
	/*[field] PORT_DEF_SPCP*/
	#define PORT_DEF_PCP_PORT_DEF_SPCP
	#define PORT_DEF_PCP_PORT_DEF_SPCP_OFFSET  0
	#define PORT_DEF_PCP_PORT_DEF_SPCP_LEN     3
	#define PORT_DEF_PCP_PORT_DEF_SPCP_DEFAULT 0x0
	/*[field] PORT_DEF_SDEI*/
	#define PORT_DEF_PCP_PORT_DEF_SDEI
	#define PORT_DEF_PCP_PORT_DEF_SDEI_OFFSET  3
	#define PORT_DEF_PCP_PORT_DEF_SDEI_LEN     1
	#define PORT_DEF_PCP_PORT_DEF_SDEI_DEFAULT 0x0
	/*[field] PORT_DEF_CPCP*/
	#define PORT_DEF_PCP_PORT_DEF_CPCP
	#define PORT_DEF_PCP_PORT_DEF_CPCP_OFFSET  4
	#define PORT_DEF_PCP_PORT_DEF_CPCP_LEN     3
	#define PORT_DEF_PCP_PORT_DEF_CPCP_DEFAULT 0x0
	/*[field] PORT_DEF_CDEI*/
	#define PORT_DEF_PCP_PORT_DEF_CDEI
	#define PORT_DEF_PCP_PORT_DEF_CDEI_OFFSET  7
	#define PORT_DEF_PCP_PORT_DEF_CDEI_LEN     1
	#define PORT_DEF_PCP_PORT_DEF_CDEI_DEFAULT 0x0

struct port_def_pcp {
	a_uint32_t  port_def_spcp:3;
	a_uint32_t  port_def_sdei:1;
	a_uint32_t  port_def_cpcp:3;
	a_uint32_t  port_def_cdei:1;
	a_uint32_t  _reserved0:24;
};

union port_def_pcp_u {
	a_uint32_t val;
	struct port_def_pcp bf;
};

/*[register] PORT_VLAN_CONFIG*/
#define PORT_VLAN_CONFIG
#define PORT_VLAN_CONFIG_ADDRESS 0x50
#define PORT_VLAN_CONFIG_NUM     1
#define PORT_VLAN_CONFIG_INC     0x4
#define PORT_VLAN_CONFIG_TYPE    REG_TYPE_RW
#define PORT_VLAN_CONFIG_DEFAULT 0x0
	/*[field] PORT_IN_PCP_PROP_CMD*/
	#define PORT_VLAN_CONFIG_PORT_IN_PCP_PROP_CMD
	#define PORT_VLAN_CONFIG_PORT_IN_PCP_PROP_CMD_OFFSET  0
	#define PORT_VLAN_CONFIG_PORT_IN_PCP_PROP_CMD_LEN     1
	#define PORT_VLAN_CONFIG_PORT_IN_PCP_PROP_CMD_DEFAULT 0x0
	/*[field] PORT_IN_DEI_PROP_CMD*/
	#define PORT_VLAN_CONFIG_PORT_IN_DEI_PROP_CMD
	#define PORT_VLAN_CONFIG_PORT_IN_DEI_PROP_CMD_OFFSET  1
	#define PORT_VLAN_CONFIG_PORT_IN_DEI_PROP_CMD_LEN     1
	#define PORT_VLAN_CONFIG_PORT_IN_DEI_PROP_CMD_DEFAULT 0x0
	/*[field] PORT_UNTAG_FLTR_CMD*/
	#define PORT_VLAN_CONFIG_PORT_UNTAG_FLTR_CMD
	#define PORT_VLAN_CONFIG_PORT_UNTAG_FLTR_CMD_OFFSET  2
	#define PORT_VLAN_CONFIG_PORT_UNTAG_FLTR_CMD_LEN     1
	#define PORT_VLAN_CONFIG_PORT_UNTAG_FLTR_CMD_DEFAULT 0x0
	/*[field] PORT_PRI_TAG_FLTR_CMD*/
	#define PORT_VLAN_CONFIG_PORT_PRI_TAG_FLTR_CMD
	#define PORT_VLAN_CONFIG_PORT_PRI_TAG_FLTR_CMD_OFFSET  3
	#define PORT_VLAN_CONFIG_PORT_PRI_TAG_FLTR_CMD_LEN     1
	#define PORT_VLAN_CONFIG_PORT_PRI_TAG_FLTR_CMD_DEFAULT 0x0
	/*[field] PORT_TAG_FLTR_CMD*/
	#define PORT_VLAN_CONFIG_PORT_TAG_FLTR_CMD
	#define PORT_VLAN_CONFIG_PORT_TAG_FLTR_CMD_OFFSET  4
	#define PORT_VLAN_CONFIG_PORT_TAG_FLTR_CMD_LEN     1
	#define PORT_VLAN_CONFIG_PORT_TAG_FLTR_CMD_DEFAULT 0x0
	/*[field] PORT_VLAN_XLT_MISS_FWD_CMD*/
	#define PORT_VLAN_CONFIG_PORT_VLAN_XLT_MISS_FWD_CMD
	#define PORT_VLAN_CONFIG_PORT_VLAN_XLT_MISS_FWD_CMD_OFFSET  5
	#define PORT_VLAN_CONFIG_PORT_VLAN_XLT_MISS_FWD_CMD_LEN     2
	#define PORT_VLAN_CONFIG_PORT_VLAN_XLT_MISS_FWD_CMD_DEFAULT 0x0
	/*[field] PORT_IN_VLAN_FLTR_CMD*/
	#define PORT_VLAN_CONFIG_PORT_IN_VLAN_FLTR_CMD
	#define PORT_VLAN_CONFIG_PORT_IN_VLAN_FLTR_CMD_OFFSET  7
	#define PORT_VLAN_CONFIG_PORT_IN_VLAN_FLTR_CMD_LEN     1
	#define PORT_VLAN_CONFIG_PORT_IN_VLAN_FLTR_CMD_DEFAULT 0x0

struct port_vlan_config {
	a_uint32_t  port_in_pcp_prop_cmd:1;
	a_uint32_t  port_in_dei_prop_cmd:1;
	a_uint32_t  port_untag_fltr_cmd:1;
	a_uint32_t  port_pri_tag_fltr_cmd:1;
	a_uint32_t  port_tag_fltr_cmd:1;
	a_uint32_t  port_vlan_xlt_miss_fwd_cmd:2;
	a_uint32_t  port_in_vlan_fltr_cmd:1;
	a_uint32_t  _reserved0:24;
};

union port_vlan_config_u {
	a_uint32_t val;
	struct port_vlan_config bf;
};

/*[register] IV_DBG_ADDR*/
#define IV_DBG_ADDR
#define IV_DBG_ADDR_ADDRESS 0x70
#define IV_DBG_ADDR_NUM     1
#define IV_DBG_ADDR_INC     0x4
#define IV_DBG_ADDR_TYPE    REG_TYPE_RW
#define IV_DBG_ADDR_DEFAULT 0x0
	/*[field] DBG_ADDR*/
	#define IV_DBG_ADDR_DBG_ADDR
	#define IV_DBG_ADDR_DBG_ADDR_OFFSET  0
	#define IV_DBG_ADDR_DBG_ADDR_LEN     8
	#define IV_DBG_ADDR_DBG_ADDR_DEFAULT 0x0

struct iv_dbg_addr {
	a_uint32_t  dbg_addr:8;
	a_uint32_t  _reserved0:24;
};

union iv_dbg_addr_u {
	a_uint32_t val;
	struct iv_dbg_addr bf;
};

/*[register] IV_DBG_DATA*/
#define IV_DBG_DATA
#define IV_DBG_DATA_ADDRESS 0x74
#define IV_DBG_DATA_NUM     1
#define IV_DBG_DATA_INC     0x4
#define IV_DBG_DATA_TYPE    REG_TYPE_RO
#define IV_DBG_DATA_DEFAULT 0x0
	/*[field] DBG_DATA*/
	#define IV_DBG_DATA_DBG_DATA
	#define IV_DBG_DATA_DBG_DATA_OFFSET  0
	#define IV_DBG_DATA_DBG_DATA_LEN     32
	#define IV_DBG_DATA_DBG_DATA_DEFAULT 0x0

struct iv_dbg_data {
	a_uint32_t  dbg_data:32;
};

union iv_dbg_data_u {
	a_uint32_t val;
	struct iv_dbg_data bf;
};

/*[register] ECO_RESERVE*/
#define ECO_RESERVE
#define ECO_RESERVE_ADDRESS 0x78
#define ECO_RESERVE_NUM     1
#define ECO_RESERVE_INC     0x4
#define ECO_RESERVE_TYPE    REG_TYPE_RW
#define ECO_RESERVE_DEFAULT 0x0
	/*[field] ECO_RES*/
	#define ECO_RESERVE_ECO_RES
	#define ECO_RESERVE_ECO_RES_OFFSET  0
	#define ECO_RESERVE_ECO_RES_LEN     32
	#define ECO_RESERVE_ECO_RES_DEFAULT 0x0

struct eco_reserve {
	a_uint32_t  eco_res:32;
};

union eco_reserve_u {
	a_uint32_t val;
	struct eco_reserve bf;
};

/*[table] XLT_RULE_TBL*/
#define XLT_RULE_TBL
#define XLT_RULE_TBL_ADDRESS 0x2000
#define XLT_RULE_TBL_NUM     64
#define XLT_RULE_TBL_INC     0x10
#define XLT_RULE_TBL_TYPE    REG_TYPE_RW
#define XLT_RULE_TBL_DEFAULT 0x0
	/*[field] VALID*/
	#define XLT_RULE_TBL_VALID
	#define XLT_RULE_TBL_VALID_OFFSET  0
	#define XLT_RULE_TBL_VALID_LEN     1
	#define XLT_RULE_TBL_VALID_DEFAULT 0x0
	/*[field] PORT_BITMAP*/
	#define XLT_RULE_TBL_PORT_BITMAP
	#define XLT_RULE_TBL_PORT_BITMAP_OFFSET  1
	#define XLT_RULE_TBL_PORT_BITMAP_LEN     8
	#define XLT_RULE_TBL_PORT_BITMAP_DEFAULT 0x0
	/*[field] SKEY_FMT*/
	#define XLT_RULE_TBL_SKEY_FMT
	#define XLT_RULE_TBL_SKEY_FMT_OFFSET  9
	#define XLT_RULE_TBL_SKEY_FMT_LEN     3
	#define XLT_RULE_TBL_SKEY_FMT_DEFAULT 0x0
	/*[field] SKEY_VID_INCL*/
	#define XLT_RULE_TBL_SKEY_VID_INCL
	#define XLT_RULE_TBL_SKEY_VID_INCL_OFFSET  12
	#define XLT_RULE_TBL_SKEY_VID_INCL_LEN     1
	#define XLT_RULE_TBL_SKEY_VID_INCL_DEFAULT 0x0
	/*[field] SKEY_VID*/
	#define XLT_RULE_TBL_SKEY_VID
	#define XLT_RULE_TBL_SKEY_VID_OFFSET  13
	#define XLT_RULE_TBL_SKEY_VID_LEN     12
	#define XLT_RULE_TBL_SKEY_VID_DEFAULT 0x0
	/*[field] SKEY_PCP_INCL*/
	#define XLT_RULE_TBL_SKEY_PCP_INCL
	#define XLT_RULE_TBL_SKEY_PCP_INCL_OFFSET  25
	#define XLT_RULE_TBL_SKEY_PCP_INCL_LEN     1
	#define XLT_RULE_TBL_SKEY_PCP_INCL_DEFAULT 0x0
	/*[field] SKEY_PCP*/
	#define XLT_RULE_TBL_SKEY_PCP
	#define XLT_RULE_TBL_SKEY_PCP_OFFSET  26
	#define XLT_RULE_TBL_SKEY_PCP_LEN     3
	#define XLT_RULE_TBL_SKEY_PCP_DEFAULT 0x0
	/*[field] SKEY_DEI_INCL*/
	#define XLT_RULE_TBL_SKEY_DEI_INCL
	#define XLT_RULE_TBL_SKEY_DEI_INCL_OFFSET  29
	#define XLT_RULE_TBL_SKEY_DEI_INCL_LEN     1
	#define XLT_RULE_TBL_SKEY_DEI_INCL_DEFAULT 0x0
	/*[field] SKEY_DEI*/
	#define XLT_RULE_TBL_SKEY_DEI
	#define XLT_RULE_TBL_SKEY_DEI_OFFSET  30
	#define XLT_RULE_TBL_SKEY_DEI_LEN     1
	#define XLT_RULE_TBL_SKEY_DEI_DEFAULT 0x0
	/*[field] CKEY_FMT*/
	#define XLT_RULE_TBL_CKEY_FMT
	#define XLT_RULE_TBL_CKEY_FMT_OFFSET  31
	#define XLT_RULE_TBL_CKEY_FMT_LEN     3
	#define XLT_RULE_TBL_CKEY_FMT_DEFAULT 0x0
	/*[field] CKEY_VID_INCL*/
	#define XLT_RULE_TBL_CKEY_VID_INCL
	#define XLT_RULE_TBL_CKEY_VID_INCL_OFFSET  34
	#define XLT_RULE_TBL_CKEY_VID_INCL_LEN     1
	#define XLT_RULE_TBL_CKEY_VID_INCL_DEFAULT 0x0
	/*[field] CKEY_VID*/
	#define XLT_RULE_TBL_CKEY_VID
	#define XLT_RULE_TBL_CKEY_VID_OFFSET  35
	#define XLT_RULE_TBL_CKEY_VID_LEN     12
	#define XLT_RULE_TBL_CKEY_VID_DEFAULT 0x0
	/*[field] CKEY_PCP_INCL*/
	#define XLT_RULE_TBL_CKEY_PCP_INCL
	#define XLT_RULE_TBL_CKEY_PCP_INCL_OFFSET  47
	#define XLT_RULE_TBL_CKEY_PCP_INCL_LEN     1
	#define XLT_RULE_TBL_CKEY_PCP_INCL_DEFAULT 0x0
	/*[field] CKEY_PCP*/
	#define XLT_RULE_TBL_CKEY_PCP
	#define XLT_RULE_TBL_CKEY_PCP_OFFSET  48
	#define XLT_RULE_TBL_CKEY_PCP_LEN     3
	#define XLT_RULE_TBL_CKEY_PCP_DEFAULT 0x0
	/*[field] CKEY_DEI_INCL*/
	#define XLT_RULE_TBL_CKEY_DEI_INCL
	#define XLT_RULE_TBL_CKEY_DEI_INCL_OFFSET  51
	#define XLT_RULE_TBL_CKEY_DEI_INCL_LEN     1
	#define XLT_RULE_TBL_CKEY_DEI_INCL_DEFAULT 0x0
	/*[field] CKEY_DEI*/
	#define XLT_RULE_TBL_CKEY_DEI
	#define XLT_RULE_TBL_CKEY_DEI_OFFSET  52
	#define XLT_RULE_TBL_CKEY_DEI_LEN     1
	#define XLT_RULE_TBL_CKEY_DEI_DEFAULT 0x0
	/*[field] FRM_TYPE_INCL*/
	#define XLT_RULE_TBL_FRM_TYPE_INCL
	#define XLT_RULE_TBL_FRM_TYPE_INCL_OFFSET  53
	#define XLT_RULE_TBL_FRM_TYPE_INCL_LEN     1
	#define XLT_RULE_TBL_FRM_TYPE_INCL_DEFAULT 0x0
	/*[field] FRM_TYPE*/
	#define XLT_RULE_TBL_FRM_TYPE
	#define XLT_RULE_TBL_FRM_TYPE_OFFSET  54
	#define XLT_RULE_TBL_FRM_TYPE_LEN     2
	#define XLT_RULE_TBL_FRM_TYPE_DEFAULT 0x0
	/*[field] PROT_INCL*/
	#define XLT_RULE_TBL_PROT_INCL
	#define XLT_RULE_TBL_PROT_INCL_OFFSET  56
	#define XLT_RULE_TBL_PROT_INCL_LEN     1
	#define XLT_RULE_TBL_PROT_INCL_DEFAULT 0x0
	/*[field] PROT_VALUE*/
	#define XLT_RULE_TBL_PROT_VALUE
	#define XLT_RULE_TBL_PROT_VALUE_OFFSET  57
	#define XLT_RULE_TBL_PROT_VALUE_LEN     16
	#define XLT_RULE_TBL_PROT_VALUE_DEFAULT 0x0

struct xlt_rule_tbl {
	a_uint32_t  valid:1;
	a_uint32_t  port_bitmap:8;
	a_uint32_t  skey_fmt:3;
	a_uint32_t  skey_vid_incl:1;
	a_uint32_t  skey_vid:12;
	a_uint32_t  skey_pcp_incl:1;
	a_uint32_t  skey_pcp:3;
	a_uint32_t  skey_dei_incl:1;
	a_uint32_t  skey_dei:1;
	a_uint32_t  ckey_fmt_0:1;
	a_uint32_t  ckey_fmt_1:2;
	a_uint32_t  ckey_vid_incl:1;
	a_uint32_t  ckey_vid:12;
	a_uint32_t  ckey_pcp_incl:1;
	a_uint32_t  ckey_pcp:3;
	a_uint32_t  ckey_dei_incl:1;
	a_uint32_t  ckey_dei:1;
	a_uint32_t  frm_type_incl:1;
	a_uint32_t  frm_type:2;
	a_uint32_t  prot_incl:1;
	a_uint32_t  prot_value_0:7;
	a_uint32_t  prot_value_1:9;
	a_uint32_t  _reserved0:23;
};

union xlt_rule_tbl_u {
	a_uint32_t val[3];
	struct xlt_rule_tbl bf;
};

/*[table] XLT_ACTION_TBL*/
#define XLT_ACTION_TBL
#define XLT_ACTION_TBL_ADDRESS 0x4000
#define XLT_ACTION_TBL_NUM     64
#define XLT_ACTION_TBL_INC     0x10
#define XLT_ACTION_TBL_TYPE    REG_TYPE_RW
#define XLT_ACTION_TBL_DEFAULT 0x0
	/*[field] VID_SWAP_CMD*/
	#define XLT_ACTION_TBL_VID_SWAP_CMD
	#define XLT_ACTION_TBL_VID_SWAP_CMD_OFFSET  0
	#define XLT_ACTION_TBL_VID_SWAP_CMD_LEN     1
	#define XLT_ACTION_TBL_VID_SWAP_CMD_DEFAULT 0x0
	/*[field] XLT_SVID_CMD*/
	#define XLT_ACTION_TBL_XLT_SVID_CMD
	#define XLT_ACTION_TBL_XLT_SVID_CMD_OFFSET  1
	#define XLT_ACTION_TBL_XLT_SVID_CMD_LEN     2
	#define XLT_ACTION_TBL_XLT_SVID_CMD_DEFAULT 0x0
	/*[field] XLT_SVID*/
	#define XLT_ACTION_TBL_XLT_SVID
	#define XLT_ACTION_TBL_XLT_SVID_OFFSET  3
	#define XLT_ACTION_TBL_XLT_SVID_LEN     12
	#define XLT_ACTION_TBL_XLT_SVID_DEFAULT 0x0
	/*[field] XLT_CVID_CMD*/
	#define XLT_ACTION_TBL_XLT_CVID_CMD
	#define XLT_ACTION_TBL_XLT_CVID_CMD_OFFSET  15
	#define XLT_ACTION_TBL_XLT_CVID_CMD_LEN     2
	#define XLT_ACTION_TBL_XLT_CVID_CMD_DEFAULT 0x0
	/*[field] XLT_CVID*/
	#define XLT_ACTION_TBL_XLT_CVID
	#define XLT_ACTION_TBL_XLT_CVID_OFFSET  17
	#define XLT_ACTION_TBL_XLT_CVID_LEN     12
	#define XLT_ACTION_TBL_XLT_CVID_DEFAULT 0x0
	/*[field] PCP_SWAP_CMD*/
	#define XLT_ACTION_TBL_PCP_SWAP_CMD
	#define XLT_ACTION_TBL_PCP_SWAP_CMD_OFFSET  29
	#define XLT_ACTION_TBL_PCP_SWAP_CMD_LEN     1
	#define XLT_ACTION_TBL_PCP_SWAP_CMD_DEFAULT 0x0
	/*[field] XLT_SPCP_CMD*/
	#define XLT_ACTION_TBL_XLT_SPCP_CMD
	#define XLT_ACTION_TBL_XLT_SPCP_CMD_OFFSET  30
	#define XLT_ACTION_TBL_XLT_SPCP_CMD_LEN     1
	#define XLT_ACTION_TBL_XLT_SPCP_CMD_DEFAULT 0x0
	/*[field] XLT_SPCP*/
	#define XLT_ACTION_TBL_XLT_SPCP
	#define XLT_ACTION_TBL_XLT_SPCP_OFFSET  31
	#define XLT_ACTION_TBL_XLT_SPCP_LEN     3
	#define XLT_ACTION_TBL_XLT_SPCP_DEFAULT 0x0
	/*[field] XLT_CPCP_CMD*/
	#define XLT_ACTION_TBL_XLT_CPCP_CMD
	#define XLT_ACTION_TBL_XLT_CPCP_CMD_OFFSET  34
	#define XLT_ACTION_TBL_XLT_CPCP_CMD_LEN     1
	#define XLT_ACTION_TBL_XLT_CPCP_CMD_DEFAULT 0x0
	/*[field] XLT_CPCP*/
	#define XLT_ACTION_TBL_XLT_CPCP
	#define XLT_ACTION_TBL_XLT_CPCP_OFFSET  35
	#define XLT_ACTION_TBL_XLT_CPCP_LEN     3
	#define XLT_ACTION_TBL_XLT_CPCP_DEFAULT 0x0
	/*[field] DEI_SWAP_CMD*/
	#define XLT_ACTION_TBL_DEI_SWAP_CMD
	#define XLT_ACTION_TBL_DEI_SWAP_CMD_OFFSET  38
	#define XLT_ACTION_TBL_DEI_SWAP_CMD_LEN     1
	#define XLT_ACTION_TBL_DEI_SWAP_CMD_DEFAULT 0x0
	/*[field] XLT_SDEI_CMD*/
	#define XLT_ACTION_TBL_XLT_SDEI_CMD
	#define XLT_ACTION_TBL_XLT_SDEI_CMD_OFFSET  39
	#define XLT_ACTION_TBL_XLT_SDEI_CMD_LEN     1
	#define XLT_ACTION_TBL_XLT_SDEI_CMD_DEFAULT 0x0
	/*[field] XLT_SDEI*/
	#define XLT_ACTION_TBL_XLT_SDEI
	#define XLT_ACTION_TBL_XLT_SDEI_OFFSET  40
	#define XLT_ACTION_TBL_XLT_SDEI_LEN     1
	#define XLT_ACTION_TBL_XLT_SDEI_DEFAULT 0x0
	/*[field] XLT_CDEI_CMD*/
	#define XLT_ACTION_TBL_XLT_CDEI_CMD
	#define XLT_ACTION_TBL_XLT_CDEI_CMD_OFFSET  41
	#define XLT_ACTION_TBL_XLT_CDEI_CMD_LEN     1
	#define XLT_ACTION_TBL_XLT_CDEI_CMD_DEFAULT 0x0
	/*[field] XLT_CDEI*/
	#define XLT_ACTION_TBL_XLT_CDEI
	#define XLT_ACTION_TBL_XLT_CDEI_OFFSET  42
	#define XLT_ACTION_TBL_XLT_CDEI_LEN     1
	#define XLT_ACTION_TBL_XLT_CDEI_DEFAULT 0x0
	/*[field] VSI_CMD*/
	#define XLT_ACTION_TBL_VSI_CMD
	#define XLT_ACTION_TBL_VSI_CMD_OFFSET  43
	#define XLT_ACTION_TBL_VSI_CMD_LEN     1
	#define XLT_ACTION_TBL_VSI_CMD_DEFAULT 0x0
	/*[field] VSI*/
	#define XLT_ACTION_TBL_VSI
	#define XLT_ACTION_TBL_VSI_OFFSET  44
	#define XLT_ACTION_TBL_VSI_LEN     5
	#define XLT_ACTION_TBL_VSI_DEFAULT 0x0
	/*[field] COUNTER_EN*/
	#define XLT_ACTION_TBL_COUNTER_EN
	#define XLT_ACTION_TBL_COUNTER_EN_OFFSET  49
	#define XLT_ACTION_TBL_COUNTER_EN_LEN     1
	#define XLT_ACTION_TBL_COUNTER_EN_DEFAULT 0x0
	/*[field] COUNTER_ID*/
	#define XLT_ACTION_TBL_COUNTER_ID
	#define XLT_ACTION_TBL_COUNTER_ID_OFFSET  50
	#define XLT_ACTION_TBL_COUNTER_ID_LEN     6
	#define XLT_ACTION_TBL_COUNTER_ID_DEFAULT 0x0

struct xlt_action_tbl {
	a_uint32_t  vid_swap_cmd:1;
	a_uint32_t  xlt_svid_cmd:2;
	a_uint32_t  xlt_svid:12;
	a_uint32_t  xlt_cvid_cmd:2;
	a_uint32_t  xlt_cvid:12;
	a_uint32_t  pcp_swap_cmd:1;
	a_uint32_t  xlt_spcp_cmd:1;
	a_uint32_t  xlt_spcp_0:1;
	a_uint32_t  xlt_spcp_1:2;
	a_uint32_t  xlt_cpcp_cmd:1;
	a_uint32_t  xlt_cpcp:3;
	a_uint32_t  dei_swap_cmd:1;
	a_uint32_t  xlt_sdei_cmd:1;
	a_uint32_t  xlt_sdei:1;
	a_uint32_t  xlt_cdei_cmd:1;
	a_uint32_t  xlt_cdei:1;
	a_uint32_t  vsi_cmd:1;
	a_uint32_t  vsi:5;
	a_uint32_t  counter_en:1;
	a_uint32_t  counter_id:6;
	a_uint32_t  _reserved0:8;
};

union xlt_action_tbl_u {
	a_uint32_t val[2];
	struct xlt_action_tbl bf;
};

/*[table] EG_VLAN_XLT_RULE*/
#define EG_VLAN_XLT_RULE
#define EG_VLAN_XLT_RULE_ADDRESS 0x200
#define EG_VLAN_XLT_RULE_NUM     64
#define EG_VLAN_XLT_RULE_INC     0x8
#define EG_VLAN_XLT_RULE_TYPE    REG_TYPE_RW
#define EG_VLAN_XLT_RULE_DEFAULT 0x0
	/*[field] VALID*/
	#define EG_VLAN_XLT_RULE_VALID
	#define EG_VLAN_XLT_RULE_VALID_OFFSET  0
	#define EG_VLAN_XLT_RULE_VALID_LEN     1
	#define EG_VLAN_XLT_RULE_VALID_DEFAULT 0x0
	/*[field] PORT_BITMAP*/
	#define EG_VLAN_XLT_RULE_PORT_BITMAP
	#define EG_VLAN_XLT_RULE_PORT_BITMAP_OFFSET  1
	#define EG_VLAN_XLT_RULE_PORT_BITMAP_LEN     8
	#define EG_VLAN_XLT_RULE_PORT_BITMAP_DEFAULT 0x0
	/*[field] VSI_INCL*/
	#define EG_VLAN_XLT_RULE_VSI_INCL
	#define EG_VLAN_XLT_RULE_VSI_INCL_OFFSET  9
	#define EG_VLAN_XLT_RULE_VSI_INCL_LEN     1
	#define EG_VLAN_XLT_RULE_VSI_INCL_DEFAULT 0x0
	/*[field] VSI*/
	#define EG_VLAN_XLT_RULE_VSI
	#define EG_VLAN_XLT_RULE_VSI_OFFSET  10
	#define EG_VLAN_XLT_RULE_VSI_LEN     5
	#define EG_VLAN_XLT_RULE_VSI_DEFAULT 0x0
	/*[field] VSI_VALID*/
	#define EG_VLAN_XLT_RULE_VSI_VALID
	#define EG_VLAN_XLT_RULE_VSI_VALID_OFFSET  15
	#define EG_VLAN_XLT_RULE_VSI_VALID_LEN     1
	#define EG_VLAN_XLT_RULE_VSI_VALID_DEFAULT 0x0
	/*[field] SKEY_FMT*/
	#define EG_VLAN_XLT_RULE_SKEY_FMT
	#define EG_VLAN_XLT_RULE_SKEY_FMT_OFFSET  16
	#define EG_VLAN_XLT_RULE_SKEY_FMT_LEN     3
	#define EG_VLAN_XLT_RULE_SKEY_FMT_DEFAULT 0x0
	/*[field] SKEY_VID_INCL*/
	#define EG_VLAN_XLT_RULE_SKEY_VID_INCL
	#define EG_VLAN_XLT_RULE_SKEY_VID_INCL_OFFSET  19
	#define EG_VLAN_XLT_RULE_SKEY_VID_INCL_LEN     1
	#define EG_VLAN_XLT_RULE_SKEY_VID_INCL_DEFAULT 0x0
	/*[field] SKEY_VID*/
	#define EG_VLAN_XLT_RULE_SKEY_VID
	#define EG_VLAN_XLT_RULE_SKEY_VID_OFFSET  20
	#define EG_VLAN_XLT_RULE_SKEY_VID_LEN     12
	#define EG_VLAN_XLT_RULE_SKEY_VID_DEFAULT 0x0
	/*[field] SKEY_PCP_INCL*/
	#define EG_VLAN_XLT_RULE_SKEY_PCP_INCL
	#define EG_VLAN_XLT_RULE_SKEY_PCP_INCL_OFFSET  32
	#define EG_VLAN_XLT_RULE_SKEY_PCP_INCL_LEN     1
	#define EG_VLAN_XLT_RULE_SKEY_PCP_INCL_DEFAULT 0x0
	/*[field] SKEY_PCP*/
	#define EG_VLAN_XLT_RULE_SKEY_PCP
	#define EG_VLAN_XLT_RULE_SKEY_PCP_OFFSET  33
	#define EG_VLAN_XLT_RULE_SKEY_PCP_LEN     3
	#define EG_VLAN_XLT_RULE_SKEY_PCP_DEFAULT 0x0
	/*[field] SKEY_DEI_INCL*/
	#define EG_VLAN_XLT_RULE_SKEY_DEI_INCL
	#define EG_VLAN_XLT_RULE_SKEY_DEI_INCL_OFFSET  36
	#define EG_VLAN_XLT_RULE_SKEY_DEI_INCL_LEN     1
	#define EG_VLAN_XLT_RULE_SKEY_DEI_INCL_DEFAULT 0x0
	/*[field] SKEY_DEI*/
	#define EG_VLAN_XLT_RULE_SKEY_DEI
	#define EG_VLAN_XLT_RULE_SKEY_DEI_OFFSET  37
	#define EG_VLAN_XLT_RULE_SKEY_DEI_LEN     1
	#define EG_VLAN_XLT_RULE_SKEY_DEI_DEFAULT 0x0
	/*[field] CKEY_FMT*/
	#define EG_VLAN_XLT_RULE_CKEY_FMT
	#define EG_VLAN_XLT_RULE_CKEY_FMT_OFFSET  38
	#define EG_VLAN_XLT_RULE_CKEY_FMT_LEN     3
	#define EG_VLAN_XLT_RULE_CKEY_FMT_DEFAULT 0x0
	/*[field] CKEY_VID_INCL*/
	#define EG_VLAN_XLT_RULE_CKEY_VID_INCL
	#define EG_VLAN_XLT_RULE_CKEY_VID_INCL_OFFSET  41
	#define EG_VLAN_XLT_RULE_CKEY_VID_INCL_LEN     1
	#define EG_VLAN_XLT_RULE_CKEY_VID_INCL_DEFAULT 0x0
	/*[field] CKEY_VID*/
	#define EG_VLAN_XLT_RULE_CKEY_VID
	#define EG_VLAN_XLT_RULE_CKEY_VID_OFFSET  42
	#define EG_VLAN_XLT_RULE_CKEY_VID_LEN     12
	#define EG_VLAN_XLT_RULE_CKEY_VID_DEFAULT 0x0
	/*[field] CKEY_PCP_INCL*/
	#define EG_VLAN_XLT_RULE_CKEY_PCP_INCL
	#define EG_VLAN_XLT_RULE_CKEY_PCP_INCL_OFFSET  54
	#define EG_VLAN_XLT_RULE_CKEY_PCP_INCL_LEN     1
	#define EG_VLAN_XLT_RULE_CKEY_PCP_INCL_DEFAULT 0x0
	/*[field] CKEY_PCP*/
	#define EG_VLAN_XLT_RULE_CKEY_PCP
	#define EG_VLAN_XLT_RULE_CKEY_PCP_OFFSET  55
	#define EG_VLAN_XLT_RULE_CKEY_PCP_LEN     3
	#define EG_VLAN_XLT_RULE_CKEY_PCP_DEFAULT 0x0
	/*[field] CKEY_DEI_INCL*/
	#define EG_VLAN_XLT_RULE_CKEY_DEI_INCL
	#define EG_VLAN_XLT_RULE_CKEY_DEI_INCL_OFFSET  58
	#define EG_VLAN_XLT_RULE_CKEY_DEI_INCL_LEN     1
	#define EG_VLAN_XLT_RULE_CKEY_DEI_INCL_DEFAULT 0x0
	/*[field] CKEY_DEI*/
	#define EG_VLAN_XLT_RULE_CKEY_DEI
	#define EG_VLAN_XLT_RULE_CKEY_DEI_OFFSET  59
	#define EG_VLAN_XLT_RULE_CKEY_DEI_LEN     1
	#define EG_VLAN_XLT_RULE_CKEY_DEI_DEFAULT 0x0

struct eg_vlan_xlt_rule {
	a_uint32_t  valid:1;
	a_uint32_t  port_bitmap:8;
	a_uint32_t  vsi_incl:1;
	a_uint32_t  vsi:5;
	a_uint32_t  vsi_valid:1;
	a_uint32_t  skey_fmt:3;
	a_uint32_t  skey_vid_incl:1;
	a_uint32_t  skey_vid:12;
	a_uint32_t  skey_pcp_incl:1;
	a_uint32_t  skey_pcp:3;
	a_uint32_t  skey_dei_incl:1;
	a_uint32_t  skey_dei:1;
	a_uint32_t  ckey_fmt:3;
	a_uint32_t  ckey_vid_incl:1;
	a_uint32_t  ckey_vid:12;
	a_uint32_t  ckey_pcp_incl:1;
	a_uint32_t  ckey_pcp:3;
	a_uint32_t  ckey_dei_incl:1;
	a_uint32_t  ckey_dei:1;
	a_uint32_t  _reserved0:4;
};

union eg_vlan_xlt_rule_u {
	a_uint32_t val[2];
	struct eg_vlan_xlt_rule bf;
};

/*[register] EG_VSI_TAG*/
#define EG_VSI_TAG
#define EG_VSI_TAG_ADDRESS	0x0
#define EG_VSI_TAG_NUM		32
#define EG_VSI_TAG_INC		0x4
#define EG_VSI_TAG_TYPE		REG_TYPE_RW
#define EG_VSI_TAG_DEFAULT	0xaaaa
	/*[field] TAGGED_MODE_PORT_BITMAP*/
	#define EG_VSI_TAG_TAGGED_MODE_PORT_BITMAP
	#define EG_VSI_TAG_TAGGED_MODE_PORT_BITMAP_OFFSET	0
	#define EG_VSI_TAG_TAGGED_MODE_PORT_BITMAP_LEN		16
	#define EG_VSI_TAG_TAGGED_MODE_PORT_BITMAP_DEFAULT	0xaaaa

struct eg_vsi_tag {
	a_uint32_t tagged_mode_port_bitmap:16;
	a_uint32_t _reserved0:16;
};

union eg_vsi_tag_u {
	a_uint32_t val;
	struct eg_vsi_tag bf;
};

/*[register] PORT_EG_DEF_VID*/
#define PORT_EG_DEF_VID
#define PORT_EG_DEF_VID_ADDRESS 0x400
#define PORT_EG_DEF_VID_NUM     8
#define PORT_EG_DEF_VID_INC     0x4
#define PORT_EG_DEF_VID_TYPE    REG_TYPE_RW
#define PORT_EG_DEF_VID_DEFAULT 0x0
	/*[field] PORT_DEF_SVID*/
	#define PORT_EG_DEF_VID_PORT_DEF_SVID
	#define PORT_EG_DEF_VID_PORT_DEF_SVID_OFFSET  0
	#define PORT_EG_DEF_VID_PORT_DEF_SVID_LEN     12
	#define PORT_EG_DEF_VID_PORT_DEF_SVID_DEFAULT 0x0
	/*[field] PORT_DEF_SVID_EN*/
	#define PORT_EG_DEF_VID_PORT_DEF_SVID_EN
	#define PORT_EG_DEF_VID_PORT_DEF_SVID_EN_OFFSET  12
	#define PORT_EG_DEF_VID_PORT_DEF_SVID_EN_LEN     1
	#define PORT_EG_DEF_VID_PORT_DEF_SVID_EN_DEFAULT 0x0
	/*[field] PORT_DEF_CVID*/
	#define PORT_EG_DEF_VID_PORT_DEF_CVID
	#define PORT_EG_DEF_VID_PORT_DEF_CVID_OFFSET  16
	#define PORT_EG_DEF_VID_PORT_DEF_CVID_LEN     12
	#define PORT_EG_DEF_VID_PORT_DEF_CVID_DEFAULT 0x0
	/*[field] PORT_DEF_CVID_EN*/
	#define PORT_EG_DEF_VID_PORT_DEF_CVID_EN
	#define PORT_EG_DEF_VID_PORT_DEF_CVID_EN_OFFSET  28
	#define PORT_EG_DEF_VID_PORT_DEF_CVID_EN_LEN     1
	#define PORT_EG_DEF_VID_PORT_DEF_CVID_EN_DEFAULT 0x0

struct port_eg_def_vid {
	a_uint32_t  port_def_svid:12;
	a_uint32_t  port_def_svid_en:1;
	a_uint32_t  _reserved0:3;
	a_uint32_t  port_def_cvid:12;
	a_uint32_t  port_def_cvid_en:1;
	a_uint32_t  _reserved1:3;
};

union port_eg_def_vid_u {
	a_uint32_t val;
	struct port_eg_def_vid bf;
};

/*[register] PORT_EG_VLAN*/
#define PORT_EG_VLAN
#define PORT_EG_VLAN_ADDRESS 0x420
#define PORT_EG_VLAN_NUM     8
#define PORT_EG_VLAN_INC     0x4
#define PORT_EG_VLAN_TYPE    REG_TYPE_RW
#define PORT_EG_VLAN_DEFAULT 0x14
	/*[field] PORT_VLAN_TYPE*/
	#define PORT_EG_VLAN_PORT_VLAN_TYPE
	#define PORT_EG_VLAN_PORT_VLAN_TYPE_OFFSET  0
	#define PORT_EG_VLAN_PORT_VLAN_TYPE_LEN     1
	#define PORT_EG_VLAN_PORT_VLAN_TYPE_DEFAULT 0x0
	/*[field] PORT_EG_VLAN_CTAG_MODE*/
	#define PORT_EG_VLAN_PORT_EG_VLAN_CTAG_MODE
	#define PORT_EG_VLAN_PORT_EG_VLAN_CTAG_MODE_OFFSET  1
	#define PORT_EG_VLAN_PORT_EG_VLAN_CTAG_MODE_LEN     2
	#define PORT_EG_VLAN_PORT_EG_VLAN_CTAG_MODE_DEFAULT 0x2
	/*[field] PORT_EG_VLAN_STAG_MODE*/
	#define PORT_EG_VLAN_PORT_EG_VLAN_STAG_MODE
	#define PORT_EG_VLAN_PORT_EG_VLAN_STAG_MODE_OFFSET  3
	#define PORT_EG_VLAN_PORT_EG_VLAN_STAG_MODE_LEN     2
	#define PORT_EG_VLAN_PORT_EG_VLAN_STAG_MODE_DEFAULT 0x2
	/*[field] VSI_TAG_MODE_EN*/
	#define PORT_EG_VLAN_VSI_TAG_MODE_EN
	#define PORT_EG_VLAN_VSI_TAG_MODE_EN_OFFSET  5
	#define PORT_EG_VLAN_VSI_TAG_MODE_EN_LEN     1
	#define PORT_EG_VLAN_VSI_TAG_MODE_EN_DEFAULT 0x0
	/*[field] PORT_EG_PCP_PROP_CMD*/
	#define PORT_EG_VLAN_PORT_EG_PCP_PROP_CMD
	#define PORT_EG_VLAN_PORT_EG_PCP_PROP_CMD_OFFSET  6
	#define PORT_EG_VLAN_PORT_EG_PCP_PROP_CMD_LEN     1
	#define PORT_EG_VLAN_PORT_EG_PCP_PROP_CMD_DEFAULT 0x0
	/*[field] PORT_EG_DEI_PROP_CMD*/
	#define PORT_EG_VLAN_PORT_EG_DEI_PROP_CMD
	#define PORT_EG_VLAN_PORT_EG_DEI_PROP_CMD_OFFSET  7
	#define PORT_EG_VLAN_PORT_EG_DEI_PROP_CMD_LEN     1
	#define PORT_EG_VLAN_PORT_EG_DEI_PROP_CMD_DEFAULT 0x0
	/*[field] TX_COUNTING_EN*/
	#define PORT_EG_VLAN_TX_COUNTING_EN
	#define PORT_EG_VLAN_TX_COUNTING_EN_OFFSET  8
	#define PORT_EG_VLAN_TX_COUNTING_EN_LEN     1
	#define PORT_EG_VLAN_TX_COUNTING_EN_DEFAULT 0x0

struct port_eg_vlan {
	a_uint32_t  port_vlan_type:1;
	a_uint32_t  port_eg_vlan_ctag_mode:2;
	a_uint32_t  port_eg_vlan_stag_mode:2;
	a_uint32_t  vsi_tag_mode_en:1;
	a_uint32_t  port_eg_pcp_prop_cmd:1;
	a_uint32_t  port_eg_dei_prop_cmd:1;
	a_uint32_t  tx_counting_en:1;
	a_uint32_t  _reserved0:23;
};

union port_eg_vlan_u {
	a_uint32_t val;
	struct port_eg_vlan bf;
};

/*[register] EG_VLAN_TPID*/
#define EG_VLAN_TPID
#define EG_VLAN_TPID_ADDRESS 0x440
#define EG_VLAN_TPID_NUM     1
#define EG_VLAN_TPID_INC     0x4
#define EG_VLAN_TPID_TYPE    REG_TYPE_RW
#define EG_VLAN_TPID_DEFAULT 0x810088a8
	/*[field] STPID*/
	#define EG_VLAN_TPID_STPID
	#define EG_VLAN_TPID_STPID_OFFSET  0
	#define EG_VLAN_TPID_STPID_LEN     16
	#define EG_VLAN_TPID_STPID_DEFAULT 0x88a8
	/*[field] CTPID*/
	#define EG_VLAN_TPID_CTPID
	#define EG_VLAN_TPID_CTPID_OFFSET  16
	#define EG_VLAN_TPID_CTPID_LEN     16
	#define EG_VLAN_TPID_CTPID_DEFAULT 0x8100

struct eg_vlan_tpid {
	a_uint32_t  stpid:16;
	a_uint32_t  ctpid:16;
};

union eg_vlan_tpid_u {
	a_uint32_t val;
	struct eg_vlan_tpid bf;
};

/*[register] EG_BRIDGE_CONFIG*/
#define EG_BRIDGE_CONFIG
#define EG_BRIDGE_CONFIG_ADDRESS 0x6000
#define EG_BRIDGE_CONFIG_NUM     1
#define EG_BRIDGE_CONFIG_INC     0x4
#define EG_BRIDGE_CONFIG_TYPE    REG_TYPE_RW
#define EG_BRIDGE_CONFIG_DEFAULT 0x0
	/*[field] BRIDGE_TYPE*/
	#define EG_BRIDGE_CONFIG_BRIDGE_TYPE
	#define EG_BRIDGE_CONFIG_BRIDGE_TYPE_OFFSET  0
	#define EG_BRIDGE_CONFIG_BRIDGE_TYPE_LEN     1
	#define EG_BRIDGE_CONFIG_BRIDGE_TYPE_DEFAULT 0x0
	/*[field] PKT_L2_EDIT_EN*/
	#define EG_BRIDGE_CONFIG_PKT_L2_EDIT_EN
	#define EG_BRIDGE_CONFIG_PKT_L2_EDIT_EN_OFFSET  1
	#define EG_BRIDGE_CONFIG_PKT_L2_EDIT_EN_LEN     1
	#define EG_BRIDGE_CONFIG_PKT_L2_EDIT_EN_DEFAULT 0x0
	/*[field] QUEUE_CNT_EN*/
	#define EG_BRIDGE_CONFIG_QUEUE_CNT_EN
	#define EG_BRIDGE_CONFIG_QUEUE_CNT_EN_OFFSET  2
	#define EG_BRIDGE_CONFIG_QUEUE_CNT_EN_LEN     1
	#define EG_BRIDGE_CONFIG_QUEUE_CNT_EN_DEFAULT 0x0

struct eg_bridge_config {
	a_uint32_t  bridge_type:1;
	a_uint32_t  pkt_l2_edit_en:1;
	a_uint32_t  queue_cnt_en:1;
	a_uint32_t  _reserved0:29;
};

union eg_bridge_config_u {
	a_uint32_t val;
	struct eg_bridge_config bf;
};

/*[table] EG_VLAN_XLT_ACTION*/
#define EG_VLAN_XLT_ACTION
#define EG_VLAN_XLT_ACTION_ADDRESS 0xd000
#define EG_VLAN_XLT_ACTION_NUM     64
#define EG_VLAN_XLT_ACTION_INC     0x8
#define EG_VLAN_XLT_ACTION_TYPE    REG_TYPE_RW
#define EG_VLAN_XLT_ACTION_DEFAULT 0x0
	/*[field] VID_SWAP_CMD*/
	#define EG_VLAN_XLT_ACTION_VID_SWAP_CMD
	#define EG_VLAN_XLT_ACTION_VID_SWAP_CMD_OFFSET  0
	#define EG_VLAN_XLT_ACTION_VID_SWAP_CMD_LEN     1
	#define EG_VLAN_XLT_ACTION_VID_SWAP_CMD_DEFAULT 0x0
	/*[field] XLT_SVID_CMD*/
	#define EG_VLAN_XLT_ACTION_XLT_SVID_CMD
	#define EG_VLAN_XLT_ACTION_XLT_SVID_CMD_OFFSET  1
	#define EG_VLAN_XLT_ACTION_XLT_SVID_CMD_LEN     2
	#define EG_VLAN_XLT_ACTION_XLT_SVID_CMD_DEFAULT 0x0
	/*[field] XLT_SVID*/
	#define EG_VLAN_XLT_ACTION_XLT_SVID
	#define EG_VLAN_XLT_ACTION_XLT_SVID_OFFSET  3
	#define EG_VLAN_XLT_ACTION_XLT_SVID_LEN     12
	#define EG_VLAN_XLT_ACTION_XLT_SVID_DEFAULT 0x0
	/*[field] XLT_CVID_CMD*/
	#define EG_VLAN_XLT_ACTION_XLT_CVID_CMD
	#define EG_VLAN_XLT_ACTION_XLT_CVID_CMD_OFFSET  15
	#define EG_VLAN_XLT_ACTION_XLT_CVID_CMD_LEN     2
	#define EG_VLAN_XLT_ACTION_XLT_CVID_CMD_DEFAULT 0x0
	/*[field] XLT_CVID*/
	#define EG_VLAN_XLT_ACTION_XLT_CVID
	#define EG_VLAN_XLT_ACTION_XLT_CVID_OFFSET  17
	#define EG_VLAN_XLT_ACTION_XLT_CVID_LEN     12
	#define EG_VLAN_XLT_ACTION_XLT_CVID_DEFAULT 0x0
	/*[field] PCP_SWAP_CMD*/
	#define EG_VLAN_XLT_ACTION_PCP_SWAP_CMD
	#define EG_VLAN_XLT_ACTION_PCP_SWAP_CMD_OFFSET  29
	#define EG_VLAN_XLT_ACTION_PCP_SWAP_CMD_LEN     1
	#define EG_VLAN_XLT_ACTION_PCP_SWAP_CMD_DEFAULT 0x0
	/*[field] XLT_SPCP_CMD*/
	#define EG_VLAN_XLT_ACTION_XLT_SPCP_CMD
	#define EG_VLAN_XLT_ACTION_XLT_SPCP_CMD_OFFSET  30
	#define EG_VLAN_XLT_ACTION_XLT_SPCP_CMD_LEN     1
	#define EG_VLAN_XLT_ACTION_XLT_SPCP_CMD_DEFAULT 0x0
	/*[field] XLT_SPCP*/
	#define EG_VLAN_XLT_ACTION_XLT_SPCP
	#define EG_VLAN_XLT_ACTION_XLT_SPCP_OFFSET  31
	#define EG_VLAN_XLT_ACTION_XLT_SPCP_LEN     3
	#define EG_VLAN_XLT_ACTION_XLT_SPCP_DEFAULT 0x0
	/*[field] XLT_CPCP_CMD*/
	#define EG_VLAN_XLT_ACTION_XLT_CPCP_CMD
	#define EG_VLAN_XLT_ACTION_XLT_CPCP_CMD_OFFSET  34
	#define EG_VLAN_XLT_ACTION_XLT_CPCP_CMD_LEN     1
	#define EG_VLAN_XLT_ACTION_XLT_CPCP_CMD_DEFAULT 0x0
	/*[field] XLT_CPCP*/
	#define EG_VLAN_XLT_ACTION_XLT_CPCP
	#define EG_VLAN_XLT_ACTION_XLT_CPCP_OFFSET  35
	#define EG_VLAN_XLT_ACTION_XLT_CPCP_LEN     3
	#define EG_VLAN_XLT_ACTION_XLT_CPCP_DEFAULT 0x0
	/*[field] DEI_SWAP_CMD*/
	#define EG_VLAN_XLT_ACTION_DEI_SWAP_CMD
	#define EG_VLAN_XLT_ACTION_DEI_SWAP_CMD_OFFSET  38
	#define EG_VLAN_XLT_ACTION_DEI_SWAP_CMD_LEN     1
	#define EG_VLAN_XLT_ACTION_DEI_SWAP_CMD_DEFAULT 0x0
	/*[field] XLT_SDEI_CMD*/
	#define EG_VLAN_XLT_ACTION_XLT_SDEI_CMD
	#define EG_VLAN_XLT_ACTION_XLT_SDEI_CMD_OFFSET  39
	#define EG_VLAN_XLT_ACTION_XLT_SDEI_CMD_LEN     1
	#define EG_VLAN_XLT_ACTION_XLT_SDEI_CMD_DEFAULT 0x0
	/*[field] XLT_SDEI*/
	#define EG_VLAN_XLT_ACTION_XLT_SDEI
	#define EG_VLAN_XLT_ACTION_XLT_SDEI_OFFSET  40
	#define EG_VLAN_XLT_ACTION_XLT_SDEI_LEN     1
	#define EG_VLAN_XLT_ACTION_XLT_SDEI_DEFAULT 0x0
	/*[field] XLT_CDEI_CMD*/
	#define EG_VLAN_XLT_ACTION_XLT_CDEI_CMD
	#define EG_VLAN_XLT_ACTION_XLT_CDEI_CMD_OFFSET  41
	#define EG_VLAN_XLT_ACTION_XLT_CDEI_CMD_LEN     1
	#define EG_VLAN_XLT_ACTION_XLT_CDEI_CMD_DEFAULT 0x0
	/*[field] XLT_CDEI*/
	#define EG_VLAN_XLT_ACTION_XLT_CDEI
	#define EG_VLAN_XLT_ACTION_XLT_CDEI_OFFSET  42
	#define EG_VLAN_XLT_ACTION_XLT_CDEI_LEN     1
	#define EG_VLAN_XLT_ACTION_XLT_CDEI_DEFAULT 0x0
	/*[field] COUNTER_EN*/
	#define EG_VLAN_XLT_ACTION_COUNTER_EN
	#define EG_VLAN_XLT_ACTION_COUNTER_EN_OFFSET  43
	#define EG_VLAN_XLT_ACTION_COUNTER_EN_LEN     1
	#define EG_VLAN_XLT_ACTION_COUNTER_EN_DEFAULT 0x0
	/*[field] COUNTER_ID*/
	#define EG_VLAN_XLT_ACTION_COUNTER_ID
	#define EG_VLAN_XLT_ACTION_COUNTER_ID_OFFSET  44
	#define EG_VLAN_XLT_ACTION_COUNTER_ID_LEN     6
	#define EG_VLAN_XLT_ACTION_COUNTER_ID_DEFAULT 0x0

struct eg_vlan_xlt_action {
	a_uint32_t  vid_swap_cmd:1;
	a_uint32_t  xlt_svid_cmd:2;
	a_uint32_t  xlt_svid:12;
	a_uint32_t  xlt_cvid_cmd:2;
	a_uint32_t  xlt_cvid:12;
	a_uint32_t  pcp_swap_cmd:1;
	a_uint32_t  xlt_spcp_cmd:1;
	a_uint32_t  xlt_spcp_0:1;
	a_uint32_t  xlt_spcp_1:2;
	a_uint32_t  xlt_cpcp_cmd:1;
	a_uint32_t  xlt_cpcp:3;
	a_uint32_t  dei_swap_cmd:1;
	a_uint32_t  xlt_sdei_cmd:1;
	a_uint32_t  xlt_sdei:1;
	a_uint32_t  xlt_cdei_cmd:1;
	a_uint32_t  xlt_cdei:1;
	a_uint32_t  counter_en:1;
	a_uint32_t  counter_id:6;
	a_uint32_t  _reserved0:14;
};

union eg_vlan_xlt_action_u {
	a_uint32_t val[2];
	struct eg_vlan_xlt_action bf;
};

/*[table] VLAN_DEV_TX_COUNTER_TBL*/
#define VLAN_DEV_TX_COUNTER_TBL
#define VLAN_DEV_TX_COUNTER_TBL_ADDRESS 0x2000
#define VLAN_DEV_TX_COUNTER_TBL_NUM     64
#define VLAN_DEV_TX_COUNTER_TBL_INC     0x10
#define VLAN_DEV_TX_COUNTER_TBL_TYPE    REG_TYPE_RW
#define VLAN_DEV_TX_COUNTER_TBL_DEFAULT 0x0
	/*[field] TX_PKT_CNT*/
	#define VLAN_DEV_TX_COUNTER_TBL_TX_PKT_CNT
	#define VLAN_DEV_TX_COUNTER_TBL_TX_PKT_CNT_OFFSET  0
	#define VLAN_DEV_TX_COUNTER_TBL_TX_PKT_CNT_LEN     32
	#define VLAN_DEV_TX_COUNTER_TBL_TX_PKT_CNT_DEFAULT 0x0
	/*[field] TX_BYTE_CNT*/
	#define VLAN_DEV_TX_COUNTER_TBL_TX_BYTE_CNT
	#define VLAN_DEV_TX_COUNTER_TBL_TX_BYTE_CNT_OFFSET  32
	#define VLAN_DEV_TX_COUNTER_TBL_TX_BYTE_CNT_LEN     40
	#define VLAN_DEV_TX_COUNTER_TBL_TX_BYTE_CNT_DEFAULT 0x0

struct vlan_dev_tx_counter_tbl {
	a_uint32_t  tx_pkt_cnt:32;
	a_uint32_t  tx_byte_cnt_0:32;
	a_uint32_t  tx_byte_cnt_1:8;
	a_uint32_t  _reserved0:24;
};

union vlan_dev_tx_counter_tbl_u {
	a_uint32_t val[3];
	struct vlan_dev_tx_counter_tbl bf;
};

#endif

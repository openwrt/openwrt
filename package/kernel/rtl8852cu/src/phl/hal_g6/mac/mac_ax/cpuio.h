/** @file */
/******************************************************************************
 *
 * Copyright(c) 2019 Realtek Corporation. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 ******************************************************************************/

#ifndef _MAC_AX_CPUIO_H_
#define _MAC_AX_CPUIO_H_

#include "../type.h"
#include "trxcfg.h"
#include "role.h"
#include "hw.h"

/*--------------------Define ----------------------------------------*/
#define WDE_DLE_PID_C0 3
#define WDE_DLE_PID_C1 4
#define WDE_DLE_PID_WDRLS 7
#define WDE_DLE_QID_BE 0
#define WDE_DLE_QID_BK 1
#define WDE_DLE_QID_VI 2
#define WDE_DLE_QID_VO 3
#define WDE_DLE_QID_WDRLS_DROP 3
#define WDE_DLE_QID_BCN_C0 0x10
#define WDE_DLE_QID_HI_C0 0x11
#define WDE_DLE_QID_MG0_C0 0x12
#define WDE_DLE_QID_MG1_C0 0x13
#define WDE_DLE_QID_MG2_C0 0x14
#define WDE_DLE_QID_BCN_C1 0x18
#define WDE_DLE_QID_HI_C1 0x19
#define WDE_DLE_QID_MG0_C1 0x1A
#define WDE_DLE_QID_MG1_C1 0x1B
#define WDE_DLE_QID_MG2_C1 0x1C

#define WDE_DLE_SUBQID_PORT_SH	4

#define WDE_DLE_MAX_PKT_NUM 0xFFFF
#define WDE_DLE_NULL_PKTID 0xFFF

#define DLE_BUF_REQ_DLY_CNT 2000
#define DLE_BUF_REQ_DLY_US 1
#define SET_CPUIO_DLY_CNT 2000
#define SET_CPUIO_DLY_US 1

/*--------------------Define Enum------------------------------------*/

/**
 * @enum WDE_DLE_PORT_ID
 *
 * @brief WDE_DLE_PORT_ID
 *
 * @var WDE_DLE_PORT_ID::WDE_DLE_PORT_ID_DISPATCH
 * Please Place Description here.
 * @var WDE_DLE_PORT_ID::WDE_DLE_PORT_ID_PKTIN
 * Please Place Description here.
 * @var WDE_DLE_PORT_ID::WDE_DLE_PORT_ID_CMAC0
 * Please Place Description here.
 * @var WDE_DLE_PORT_ID::WDE_DLE_PORT_ID_CMAC1
 * Please Place Description here.
 * @var WDE_DLE_PORT_ID::WDE_DLE_PORT_ID_CPU_IO
 * Please Place Description here.
 * @var WDE_DLE_PORT_ID::WDE_DLE_PORT_ID_WDRLS
 * Please Place Description here.
 * @var WDE_DLE_PORT_ID::WDE_DLE_PORT_ID_END
 * Please Place Description here.
 */
enum WDE_DLE_PORT_ID {
	WDE_DLE_PORT_ID_DISPATCH = 0,
	WDE_DLE_PORT_ID_PKTIN = 1,
	WDE_DLE_PORT_ID_CMAC0 = 3,
	WDE_DLE_PORT_ID_CMAC1 = 4,
	WDE_DLE_PORT_ID_CPU_IO = 6,
	WDE_DLE_PORT_ID_WDRLS = 7,
	WDE_DLE_PORT_ID_END = 8
};

/**
 * @enum PLE_DLE_PORT_ID
 *
 * @brief PLE_DLE_PORT_ID
 *
 * @var PLE_DLE_PORT_ID::PLE_DLE_PORT_ID_DISPATCH
 * Please Place Description here.
 * @var PLE_DLE_PORT_ID::PLE_DLE_PORT_ID_MPDU
 * Please Place Description here.
 * @var PLE_DLE_PORT_ID::PLE_DLE_PORT_ID_SEC
 * Please Place Description here.
 * @var PLE_DLE_PORT_ID::PLE_DLE_PORT_ID_CMAC0
 * Please Place Description here.
 * @var PLE_DLE_PORT_ID::PLE_DLE_PORT_ID_CMAC1
 * Please Place Description here.
 * @var PLE_DLE_PORT_ID::PLE_DLE_PORT_ID_WDRLS
 * Please Place Description here.
 * @var PLE_DLE_PORT_ID::PLE_DLE_PORT_ID_CPU_IO
 * Please Place Description here.
 * @var PLE_DLE_PORT_ID::PLE_DLE_PORT_ID_PLRLS
 * Please Place Description here.
 * @var PLE_DLE_PORT_ID::PLE_DLE_PORT_ID_END
 * Please Place Description here.
 */
enum PLE_DLE_PORT_ID {
	PLE_DLE_PORT_ID_DISPATCH = 0,
	PLE_DLE_PORT_ID_MPDU = 1,
	PLE_DLE_PORT_ID_SEC = 2,
	PLE_DLE_PORT_ID_CMAC0 = 3,
	PLE_DLE_PORT_ID_CMAC1 = 4,
	PLE_DLE_PORT_ID_WDRLS = 5,
	PLE_DLE_PORT_ID_CPU_IO = 6,
	PLE_DLE_PORT_ID_PLRLS = 7,
	PLE_DLE_PORT_ID_END = 8
};

/**
 * @enum WDE_DLE_QUEID_PKTIN
 *
 * @brief WDE_DLE_QUEID_PKTIN
 *
 * @var WDE_DLE_QUEID_PKTIN::WDE_DLE_QUEID_AC0
 * Please Place Description here.
 * @var WDE_DLE_QUEID_PKTIN::WDE_DLE_QUEID_AC1
 * Please Place Description here.
 * @var WDE_DLE_QUEID_PKTIN::WDE_DLE_QUEID_AC2
 * Please Place Description here.
 * @var WDE_DLE_QUEID_PKTIN::WDE_DLE_QUEID_AC3
 * Please Place Description here.
 * @var WDE_DLE_QUEID_PKTIN::WDE_DLE_QUEID_MSIC
 * Please Place Description here.
 */
enum WDE_DLE_QUEID_PKTIN {
	WDE_DLE_QUEID_AC0 = 0x0,
	WDE_DLE_QUEID_AC1 = 0x1,
	WDE_DLE_QUEID_AC2 = 0x2,
	WDE_DLE_QUEID_AC3 = 0x3,
	WDE_DLE_QUEID_MSIC = 0x4
};

/**
 * @enum WDE_DLE_QUEID_CMAC
 *
 * @brief WDE_DLE_QUEID_CMAC
 *
 * @var WDE_DLE_QUEID_CMAC::WDE_DLE_QUEID_ACQ_BE
 * Please Place Description here.
 * @var WDE_DLE_QUEID_CMAC::WDE_DLE_QUEID_ACQ_BK
 * Please Place Description here.
 * @var WDE_DLE_QUEID_CMAC::WDE_DLE_QUEID_ACQ_VI
 * Please Place Description here.
 * @var WDE_DLE_QUEID_CMAC::WDE_DLE_QUEID_ACQ_VO
 * Please Place Description here.
 * @var WDE_DLE_QUEID_CMAC::WDE_DLE_QUEID_B0_BEACON
 * Please Place Description here.
 * @var WDE_DLE_QUEID_CMAC::WDE_DLE_QUEID_B0_HIGH
 * Please Place Description here.
 * @var WDE_DLE_QUEID_CMAC::WDE_DLE_QUEID_B0_MGN_NORMAL
 * Please Place Description here.
 * @var WDE_DLE_QUEID_CMAC::WDE_DLE_QUEID_B0_MGN_NO_POWER_SAVE
 * Please Place Description here.
 * @var WDE_DLE_QUEID_CMAC::WDE_DLE_QUEID_B0_MGN_FAST_EDCA
 * Please Place Description here.
 * @var WDE_DLE_QUEID_CMAC::WDE_DLE_QUEID_B1_BEACON
 * Please Place Description here.
 * @var WDE_DLE_QUEID_CMAC::WDE_DLE_QUEID_B1_HIGH
 * Please Place Description here.
 * @var WDE_DLE_QUEID_CMAC::WDE_DLE_QUEID_B1_MGN_NORMAL
 * Please Place Description here.
 * @var WDE_DLE_QUEID_CMAC::WDE_DLE_QUEID_B1_MGN_NO_POWER_SAVE
 * Please Place Description here.
 * @var WDE_DLE_QUEID_CMAC::WDE_DLE_QUEID_B1_MGN_FAST_EDCA
 * Please Place Description here.
 * @var WDE_DLE_QUEID_CMAC::WDE_DLE_QUEID_WMM0_F2P_VO
 * Please Place Description here.
 * @var WDE_DLE_QUEID_CMAC::WDE_DLE_QUEID_WMM0_F2P_VI
 * Please Place Description here.
 * @var WDE_DLE_QUEID_CMAC::WDE_DLE_QUEID_WMM0_F2P_BE
 * Please Place Description here.
 * @var WDE_DLE_QUEID_CMAC::WDE_DLE_QUEID_WMM0_F2P_BK
 * Please Place Description here.
 * @var WDE_DLE_QUEID_CMAC::WDE_DLE_QUEID_WMM1_F2P_VO
 * Please Place Description here.
 * @var WDE_DLE_QUEID_CMAC::WDE_DLE_QUEID_WMM1_F2P_VI
 * Please Place Description here.
 * @var WDE_DLE_QUEID_CMAC::WDE_DLE_QUEID_WMM1_F2P_BE
 * Please Place Description here.
 * @var WDE_DLE_QUEID_CMAC::WDE_DLE_QUEID_WMM1_F2P_BK
 * Please Place Description here.
 * @var WDE_DLE_QUEID_CMAC::WDE_DLE_QUEID_B0_ULQ
 * Please Place Description here.
 * @var WDE_DLE_QUEID_CMAC::WDE_DLE_QUEID_B0_TWTQ0
 * Please Place Description here.
 * @var WDE_DLE_QUEID_CMAC::WDE_DLE_QUEID_B0_TWTQ1
 * Please Place Description here.
 */
enum WDE_DLE_QUEID_CMAC {
	WDE_DLE_QUEID_ACQ_BE = 0,
	WDE_DLE_QUEID_ACQ_BK = 1,
	WDE_DLE_QUEID_ACQ_VI = 2,
	WDE_DLE_QUEID_ACQ_VO = 3,
	WDE_DLE_QUEID_B0_BEACON = 0x10,
	WDE_DLE_QUEID_B0_HIGH = 0x11,
	WDE_DLE_QUEID_B0_MGN_NORMAL = 0x12,
	WDE_DLE_QUEID_B0_MGN_NO_POWER_SAVE = 0x13,
	WDE_DLE_QUEID_B0_MGN_FAST_EDCA = 0x14,
	WDE_DLE_QUEID_B1_BEACON = 0x18,
	WDE_DLE_QUEID_B1_HIGH = 0x19,
	WDE_DLE_QUEID_B1_MGN_NORMAL = 0x1A,
	WDE_DLE_QUEID_B1_MGN_NO_POWER_SAVE = 0x1B,
	WDE_DLE_QUEID_B1_MGN_FAST_EDCA = 0x1C,
	WDE_DLE_QUEID_WMM0_F2P_VO = 0x20,
	WDE_DLE_QUEID_WMM0_F2P_VI = 0x21,
	WDE_DLE_QUEID_WMM0_F2P_BE = 0x22,
	WDE_DLE_QUEID_WMM0_F2P_BK = 0x23,
	WDE_DLE_QUEID_WMM1_F2P_VO = 0x24,
	WDE_DLE_QUEID_WMM1_F2P_VI = 0x25,
	WDE_DLE_QUEID_WMM1_F2P_BE = 0x26,
	WDE_DLE_QUEID_WMM1_F2P_BK = 0x27,
	WDE_DLE_QUEID_B0_ULQ = 0x30,
	WDE_DLE_QUEID_B0_TWTQ0 = 0x31,
	WDE_DLE_QUEID_B0_TWTQ1 = 0x32
};

/**
 * @enum WDE_DLE_QUEID_CPUIO
 *
 * @brief WDE_DLE_QUEID_CPUIO
 *
 * @var WDE_DLE_QUEID_CPUIO::WDE_DLE_QUEID_CPUIO_0
 * Please Place Description here.
 * @var WDE_DLE_QUEID_CPUIO::WDE_DLE_QUEID_CPUIO_1
 * Please Place Description here.
 */
enum WDE_DLE_QUEID_CPUIO {
	WDE_DLE_QUEID_CPUIO_0 = 0x0,
	WDE_DLE_QUEID_CPUIO_1 = 0x1
};

/**
 * @enum PLE_DLE_QUEID_CPUIO
 *
 * @brief PLE_DLE_QUEID_CPUIO
 *
 * @var PLE_DLE_QUEID_CPUIO::PLE_DLE_QUEID_CPUIO_0
 * Please Place Description here.
 * @var PLE_DLE_QUEID_CPUIO::PLE_DLE_QUEID_CPUIO_1
 * Please Place Description here.
 */
enum PLE_DLE_QUEID_CPUIO {
	PLE_DLE_QUEID_CPUIO_0 = 0x0,
	PLE_DLE_QUEID_CPUIO_1 = 0x1
};

/**
 * @enum WDE_DLE_QUEID_WDRLS
 *
 * @brief WDE_DLE_QUEID_WDRLS
 *
 * @var WDE_DLE_QUEID_WDRLS::WDE_DLE_QUEID_TXOK
 * Please Place Description here.
 * @var WDE_DLE_QUEID_WDRLS::WDE_DLE_QUEID_DROP_RETRY_LIMIT
 * Please Place Description here.
 * @var WDE_DLE_QUEID_WDRLS::WDE_DLE_QUEID_DROP_LIFETIME_TO
 * Please Place Description here.
 * @var WDE_DLE_QUEID_WDRLS::WDE_DLE_QUEID_DROP_MACID_DROP
 * Please Place Description here.
 * @var WDE_DLE_QUEID_WDRLS::WDE_DLE_QUEID_NO_REPORT
 * Please Place Description here.
 */
enum WDE_DLE_QUEID_WDRLS {
	WDE_DLE_QUEID_TXOK = 0x0,
	WDE_DLE_QUEID_DROP_RETRY_LIMIT = 0x1,
	WDE_DLE_QUEID_DROP_LIFETIME_TO = 0x2,
	WDE_DLE_QUEID_DROP_MACID_DROP = 0x3,
	WDE_DLE_QUEID_NO_REPORT = 0x4
};

/**
 * @enum PLE_DLE_QUEID_PLRLS
 *
 * @brief PLE_DLE_QUEID_PLRLS
 *
 * @var PLE_DLE_QUEID_PLRLS::PLE_DLE_QUEID_NO_REPORT
 * Please Place Description here.
 */
enum PLE_DLE_QUEID_PLRLS {
	PLE_DLE_QUEID_NO_REPORT = 0x0
};

/**
 * @enum WDE_DLE_QUOTA_ID
 *
 * @brief WDE_DLE_QUOTA_ID
 *
 * @var WDE_DLE_QUOTA_ID::WDE_DLE_QUOTA_ID_HOST_IF
 * Please Place Description here.
 * @var WDE_DLE_QUOTA_ID::WDE_DLE_QUOTA_ID_WLAN_CPU
 * Please Place Description here.
 * @var WDE_DLE_QUOTA_ID::WDE_DLE_QUOTA_ID_DATA_CPU
 * Please Place Description here.
 * @var WDE_DLE_QUOTA_ID::WDE_DLE_QUOTA_ID_PKTIN
 * Please Place Description here.
 * @var WDE_DLE_QUOTA_ID::WDE_DLE_QUOTA_ID_CPUIO
 * Please Place Description here.
 * @var WDE_DLE_QUOTA_ID::WDE_DLE_QUOTA_ID_END
 * Please Place Description here.
 */
enum WDE_DLE_QUOTA_ID {
	WDE_DLE_QUOTA_ID_HOST_IF = 0,
	WDE_DLE_QUOTA_ID_WLAN_CPU = 1,
	WDE_DLE_QUOTA_ID_DATA_CPU = 2,
	WDE_DLE_QUOTA_ID_PKTIN = 3,
	WDE_DLE_QUOTA_ID_CPUIO = 4,
	WDE_DLE_QUOTA_ID_END = 5
};

/**
 * @enum PLE_DLE_QUOTA_ID
 *
 * @brief PLE_DLE_QUOTA_ID
 *
 * @var PLE_DLE_QUOTA_ID::PLE_DLE_QUOTA_ID_BAND0_TXPL
 * Please Place Description here.
 * @var PLE_DLE_QUOTA_ID::PLE_DLE_QUOTA_ID_BAND1_TXPL
 * Please Place Description here.
 * @var PLE_DLE_QUOTA_ID::PLE_DLE_QUOTA_ID_C2H
 * Please Place Description here.
 * @var PLE_DLE_QUOTA_ID::PLE_DLE_QUOTA_ID_H2C
 * Please Place Description here.
 * @var PLE_DLE_QUOTA_ID::PLE_DLE_QUOTA_ID_WLAN_CPU
 * Please Place Description here.
 * @var PLE_DLE_QUOTA_ID::PLE_DLE_QUOTA_ID_MPDU
 * Please Place Description here.
 * @var PLE_DLE_QUOTA_ID::PLE_DLE_QUOTA_ID_CMAC0_RX
 * Please Place Description here.
 * @var PLE_DLE_QUOTA_ID::PLE_DLE_QUOTA_ID_CMAC1_RX
 * Please Place Description here.
 * @var PLE_DLE_QUOTA_ID::PLE_DLE_QUOTA_ID_CMAC1_BBRPT
 * Please Place Description here.
 * @var PLE_DLE_QUOTA_ID::PLE_DLE_QUOTA_ID_WDRLS_RPT
 * Please Place Description here.
 * @var PLE_DLE_QUOTA_ID::PLE_DLE_QUOTA_ID_CPUIO
 * Please Place Description here.
 * @var PLE_DLE_QUOTA_ID::PLE_DLE_QUOTA_ID_END
 * Please Place Description here.
 */
enum PLE_DLE_QUOTA_ID {
	PLE_DLE_QUOTA_ID_BAND0_TXPL = 0,
	PLE_DLE_QUOTA_ID_BAND1_TXPL = 1,
	PLE_DLE_QUOTA_ID_C2H = 2,
	PLE_DLE_QUOTA_ID_H2C = 3,
	PLE_DLE_QUOTA_ID_WLAN_CPU = 4,
	PLE_DLE_QUOTA_ID_MPDU = 5,
	PLE_DLE_QUOTA_ID_CMAC0_RX = 6,
	PLE_DLE_QUOTA_ID_CMAC1_RX = 7,
	PLE_DLE_QUOTA_ID_CMAC1_BBRPT = 8,
	PLE_DLE_QUOTA_ID_WDRLS_RPT = 9,
	PLE_DLE_QUOTA_ID_CPUIO = 10,
	PLE_DLE_QUOTA_ID_END = 11
};

/**
 * @enum CPUIO_CTRL_TYPE
 *
 * @brief CPUIO_CTRL_TYPE
 *
 * @var CPUIO_CTRL_TYPE::CPUIO_CTRL_TYPE_WD
 * Please Place Description here.
 * @var CPUIO_CTRL_TYPE::CPUIO_CTRL_TYPE_PLD
 * Please Place Description here.
 * @var CPUIO_CTRL_TYPE::CPUIO_CTRL_TYPE_NUM
 * Please Place Description here.
 */
enum CPUIO_CTRL_TYPE {
	CPUIO_CTRL_TYPE_WD = 0,
	CPUIO_CTRL_TYPE_PLD = 1,
	CPUIO_CTRL_TYPE_NUM = 2
};

/**
 * @enum CPUIO_OP_COMMAND_TYPE
 *
 * @brief CPUIO_OP_COMMAND_TYPE
 *
 * @var CPUIO_OP_COMMAND_TYPE::CPUIO_OP_CMD_GET_1ST_PID
 * Please Place Description here.
 * @var CPUIO_OP_COMMAND_TYPE::CPUIO_OP_CMD_GET_NEXT_PID
 * Please Place Description here.
 * @var CPUIO_OP_COMMAND_TYPE::CPUIO_OP_CMD_ENQ_TO_TAIL
 * Please Place Description here.
 * @var CPUIO_OP_COMMAND_TYPE::CPUIO_OP_CMD_ENQ_TO_HEAD
 * Please Place Description here.
 * @var CPUIO_OP_COMMAND_TYPE::CPUIO_OP_CMD_DEQ
 * Please Place Description here.
 * @var CPUIO_OP_COMMAND_TYPE::CPUIO_OP_CMD_DEQ_ENQ_ALL
 * Please Place Description here.
 * @var CPUIO_OP_COMMAND_TYPE::CPUIO_OP_CMD_DEQ_ENQ_TO_TAIL
 * Please Place Description here.
 */
enum CPUIO_OP_COMMAND_TYPE {
	CPUIO_OP_CMD_GET_1ST_PID = 0,
	CPUIO_OP_CMD_GET_NEXT_PID = 1,
	CPUIO_OP_CMD_ENQ_TO_TAIL = 4,
	CPUIO_OP_CMD_ENQ_TO_HEAD = 5,
	CPUIO_OP_CMD_DEQ = 8,
	CPUIO_OP_CMD_DEQ_ENQ_ALL = 9,
	CPUIO_OP_CMD_DEQ_ENQ_TO_TAIL = 12
};

/**
 * @enum pkt_drop_ac
 *
 * @brief pkt_drop_ac
 *
 * @var pkt_drop_ac::PKT_DROP_BE
 * Please Place Description here.
 * @var pkt_drop_ac::PKT_DROP_BK
 * Please Place Description here.
 * @var pkt_drop_ac::PKT_DROP_VI
 * Please Place Description here.
 * @var pkt_drop_ac::PKT_DROP_VO
 * Please Place Description here.
 * @var pkt_drop_ac::PKT_DROP_AC_LAST
 * Please Place Description here.
 * @var pkt_drop_ac::PKT_DROP_AC_MAX
 * Please Place Description here.
 * @var pkt_drop_ac::PKT_DROP_AC_INVALID
 * Please Place Description here.
 */
enum pkt_drop_ac {
	PKT_DROP_BE = WDE_DLE_QID_BE,
	PKT_DROP_BK = WDE_DLE_QID_BK,
	PKT_DROP_VI = WDE_DLE_QID_VI,
	PKT_DROP_VO = WDE_DLE_QID_VO,

	/* keep last */
	PKT_DROP_AC_LAST,
	PKT_DROP_AC_MAX = PKT_DROP_AC_LAST,
	PKT_DROP_AC_INVALID = PKT_DROP_AC_LAST,
};

/*--------------------Define MACRO----------------------------------*/
#define VIRTUAL_ADDRESS_MAPPING(eng_sel, pkt_id, offset)	\
	(0x00000000 | (((eng_sel) & 0x1) << 27) |		\
	(((pkt_id) & 0xFFF) << 15) | ((offset) & 0xEFFF))

#define GET_VIRTUAL_ADDRESS_WD(pkt_id, offset)\
	((((pkt_id) & 0xFFF) << 15) | ((offset) & 0xEFFF))

/*--------------------Define Struct----------------------------------*/

/**
 * @struct cpuio_buf_req_t
 * @brief cpuio_buf_req_t
 *
 * @var cpuio_buf_req_t::len
 * Please Place Description here.
 * @var cpuio_buf_req_t::pktid
 * Please Place Description here.
 */
struct cpuio_buf_req_t {
	// input
	u16 len;
	// output
	u16 pktid;
};

/**
 * @struct cpuio_ctrl_t
 * @brief cpuio_ctrl_t
 *
 * @var cpuio_ctrl_t::pkt_num
 * Please Place Description here.
 * @var cpuio_ctrl_t::start_pktid
 * Please Place Description here.
 * @var cpuio_ctrl_t::end_pktid
 * Please Place Description here.
 * @var cpuio_ctrl_t::cmd_type
 * Please Place Description here.
 * @var cpuio_ctrl_t::macid
 * Please Place Description here.
 * @var cpuio_ctrl_t::src_pid
 * Please Place Description here.
 * @var cpuio_ctrl_t::src_qid
 * Please Place Description here.
 * @var cpuio_ctrl_t::dst_pid
 * Please Place Description here.
 * @var cpuio_ctrl_t::dst_qid
 * Please Place Description here.
 * @var cpuio_ctrl_t::pktid
 * Please Place Description here.
 */
struct cpuio_ctrl_t {
	// input
	u16 pkt_num;
	u16 start_pktid;
	u16 end_pktid;
	u8 cmd_type;
	u8 macid;
	u8 src_pid;
	u8 src_qid;
	u8 dst_pid;
	u8 dst_qid;
	// output
	u16 pktid;
};

/**
 * @struct deq_enq_info
 * @brief deq_enq_info
 *
 * @var deq_enq_info::macid
 * Please Place Description here.
 * @var deq_enq_info::src_pid
 * Please Place Description here.
 * @var deq_enq_info::src_qid
 * Please Place Description here.
 * @var deq_enq_info::dst_pid
 * Please Place Description here.
 * @var deq_enq_info::dst_qid
 * Please Place Description here.
 * @var deq_enq_info::pktid
 * Please Place Description here.
 */
struct deq_enq_info {
	u8 macid;
	u8 src_pid;
	u8 src_qid;
	u8 dst_pid;
	u8 dst_qid;
	u16 pktid;
};

/**
 * @struct first_pid_info
 * @brief first_pid_info
 *
 * @var first_pid_info::macid
 * Please Place Description here.
 * @var first_pid_info::src_pid
 * Please Place Description here.
 * @var first_pid_info::src_qid
 * Please Place Description here.
 * @var first_pid_info::pktid
 * Please Place Description here.
 */
struct first_pid_info {
	u8 macid;
	u8 src_pid;
	u8 src_qid;
	u16 pktid;
};

/**
 * @struct next_pid_info
 * @brief next_pid_info
 *
 * @var next_pid_info::macid
 * Please Place Description here.
 * @var next_pid_info::src_pid
 * Please Place Description here.
 * @var next_pid_info::src_qid
 * Please Place Description here.
 * @var next_pid_info::start_pktid
 * Please Place Description here.
 * @var next_pid_info::pktid
 * Please Place Description here.
 */
struct next_pid_info {
	u8 macid;
	u8 src_pid;
	u8 src_qid;
	u16 start_pktid;
	u16 pktid;
};

/*--------------------Export global variable----------------------------*/

/*--------------------Function declaration-----------------------------*/

/**
 * @addtogroup Firmware
 * @{
 * @addtogroup CPU_IO
 * @{
 */

/**
 * @brief deq_enq_all
 *
 * @param *adapter
 * @param *info
 * @return Please Place Description here.
 * @retval u32
 */
u32 deq_enq_all(struct mac_ax_adapter *adapter, struct deq_enq_info *info);
/**
 * @}
 * @}
 */

/**
 * @addtogroup Firmware
 * @{
 * @addtogroup CPU_IO
 * @{
 */

/**
 * @brief mac_dle_buf_req_wd
 *
 * @param *adapter
 * @param *buf_req_p
 * @return Please Place Description here.
 * @retval u32
 */
u32 mac_dle_buf_req_wd(struct mac_ax_adapter *adapter,
		       struct cpuio_buf_req_t *buf_req_p);
/**
 * @}
 * @}
 */

/**
 * @addtogroup Firmware
 * @{
 * @addtogroup CPU_IO
 * @{
 */

/**
 * @brief mac_dle_buf_req_pl
 *
 * @param *adapter
 * @param *buf_req_p
 * @return Please Place Description here.
 * @retval u32
 */
u32 mac_dle_buf_req_pl(struct mac_ax_adapter *adapter,
		       struct cpuio_buf_req_t *buf_req_p);
/**
 * @}
 * @}
 */

/**
 * @addtogroup Firmware
 * @{
 * @addtogroup CPU_IO
 * @{
 */

/**
 * @brief mac_set_cpuio_wd
 *
 * @param *adapter
 * @param *ctrl_para_p
 * @return Please Place Description here.
 * @retval u32
 */
u32 mac_set_cpuio_wd(struct mac_ax_adapter *adapter,
		     struct cpuio_ctrl_t *ctrl_para_p);
/**
 * @}
 * @}
 */

/**
 * @addtogroup Firmware
 * @{
 * @addtogroup CPU_IO
 * @{
 */

/**
 * @brief mac_set_cpuio_pl
 *
 * @param *adapter
 * @param *ctrl_para_p
 * @return Please Place Description here.
 * @retval u32
 */
u32 mac_set_cpuio_pl(struct mac_ax_adapter *adapter,
		     struct cpuio_ctrl_t *ctrl_para_p);
/**
 * @}
 * @}
 */

/**
 * @addtogroup Firmware
 * @{
 * @addtogroup CPU_IO
 * @{
 */

/**
 * @brief mac_wde_pkt_drop
 *
 * @param *adapter
 * @param *info
 * @return Please Place Description here.
 * @retval u32
 */
u32 mac_wde_pkt_drop(struct mac_ax_adapter *adapter,
		     struct mac_ax_pkt_drop_info *info);
/**
 * @}
 * @}
 */

#endif

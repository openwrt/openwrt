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

 #ifndef _MAC_AX_DBG_CMD_H_
#define _MAC_AX_DBG_CMD_H_

#include "../type.h"
#include "../mac_def.h"
#include "../mac_ax.h"
#include "tblupd.h"
#if MAC_AX_8852A_SUPPORT
#include "../fw_ax/rtl8852a/hal8852a_fw_log.h"
#endif
#if MAC_AX_8852B_SUPPORT
#include "../fw_ax/rtl8852b/hal8852b_fw_log.h"
#endif
#if MAC_AX_8852C_SUPPORT
#include "../fw_ax/rtl8852c/hal8852c_fw_log.h"
#endif
#if MAC_AX_8192XB_SUPPORT
#include "../fw_ax/rtl8192xb/hal8192xb_fw_log.h"
#endif
#if MAC_AX_8851B_SUPPORT
#include "../fw_ax/rtl8851b/hal8851b_fw_log.h"
#endif
#if MAC_AX_8851E_SUPPORT
#include "../fw_ax/rtl8851e/hal8851e_fw_log.h"
#endif
#if MAC_AX_8852D_SUPPORT
#include "../fw_ax/rtl8852d/hal8852d_fw_log.h"
#endif

#define FWDGB_CFG_OP_SET 0
#define FWDGB_CFG_OP_CLR 1
#define FWDGB_CFG_OP_INFO 2

#define FWDGB_CFG_TYPE_LEVEL 0
#define FWDGB_CFG_TYPE_OUTPUT 1
#define FWDGB_CFG_TYPE_COMP 2
#define FWDGB_CFG_TYPE_COMP_EXT 3

#define LEN_PKT_HDR 8
#define LEN_PKTCNT 4
#define LEN_OPCODE 4
#define LEN_LENGTH 4
#define FWSTATUS_OPCODE_MASK 0xFFFF

#define FW_CURTCB_AX	0xf5fc
#define FW_CURTCB_BE	0xf5fc
#define FW_TCB_ADDR_MASK_AX	0x3FFFF
#define FW_TCB_ADDR_MASK_BE	0x3FFFF
#define FW_CURTCB_SP_START_OFFSET 0x30
#define FW_CURTCB_TASK_NAME_OFFSET 0x34
#define FW_MAX_TASK_NAME_LEN 16

#define eMallocMAX	23
#define eMallocTypeMAX	3

#define OPCODE_HDR_LEN 2
#define MAX_FWSTATSUS_PKT_LEN 12
//STR_BUF_SIZE , -128 for driver stack size warning
#define STR_BUF_SIZE (1024 - 128)

#define FWQC_SLOW_HEAP_TH 0
#define FWQC_FAST_HEAP_TH 0

#define FWCISR_OFFSET 0x08
#define PORT_CFG_OFFSET 0x40

#define GET_FIELD_OPCODE(opcode) ((opcode) & (FWSTATUS_OPCODE_MASK))
#define	MAC_DBG_MSG(max_buff_len, used_len, buff_addr, remain_len, fmt, ...)\
	do {									\
		u32 *used_len_tmp = &(used_len);				\
		if (*used_len_tmp < max_buff_len)				\
			*used_len_tmp += PLTFM_SNPRINTF(buff_addr, remain_len, fmt, ##__VA_ARGS__);\
	} while (0)

/**
 * @enum mac_hal_cmd_id
 *
 * @brief mac_hal_cmd_id
 *
 * @var mac_hal_cmd_id::MAC_HAL_HELP
 * Please Place Description here.
 * @var mac_hal_cmd_id::MAC_MAC_DD_DBG
 * Please Place Description here.
 * @var mac_hal_cmd_id::MAC_MAC_REG_DUMP
 * Please Place Description here.
 * @var mac_hal_cmd_id::MAC_MAC_FW_DBG
 * Please Place Description here.
 * @var mac_hal_cmd_id::MAC_MAC_FW_LOG
 * Please Place Description here.
 * @var mac_hal_cmd_id::MAC_MAC_FW_CURTCB
 * Please Place Description here.
 * @var mac_hal_cmd_id::MAC_MAC_FW_INFO
 * Please Place Description here.
 * @var mac_hal_cmd_id::MAC_MAC_QC_START
 * Please Place Description here.
 * @var mac_hal_cmd_id::MAC_MAC_QC_END
 * Please Place Description here.
 */
enum mac_hal_cmd_id {
	MAC_HAL_HELP = 0,
	MAC_MAC_DD_DBG,
	MAC_MAC_REG_DUMP,
	MAC_MAC_FW_DBG,
	MAC_MAC_FW_LOG,
	MAC_MAC_FW_CURTCB,
	MAC_MAC_FW_INFO,
	MAC_MAC_DBG_TX,
	MAC_MAC_DBG_RX,
	MAC_MAC_DBG_DMAC,
	MAC_MAC_DBG_CMAC,
	MAC_MAC_DBG_BCN,
	MAC_MAC_ERROR_DUMP,
	MAC_MAC_SER_CNT_DUMP,
	MAC_MAC_SET_SER_LVL,
	MAC_MAC_GET_SER_LVL,
	MAC_MAC_DL_SYM,
	MAC_MAC_QC_START,
	MAC_MAC_QC_END,
	MAC_MAC_REQ_PWR_ST,
	MAC_MAC_REQ_PWR_LVL,
	MAC_MAC_CHSW,
	MAC_MAC_CHSW_RET,
};

/**
 * @enum mac_ax_fw_status
 *
 * @brief mac_ax_fw_status
 *
 * @var mac_ax_fw_status::FW_STATUS_TASKINFO
 * Please Place Description here.
 * @var mac_ax_fw_status::FW_STATUS_FLASHINFO
 * Please Place Description here.
 * @var mac_ax_fw_status::FW_STATUS_HEAPINFO
 * Please Place Description here.
 * @var mac_ax_fw_status::FW_STATUS_MEMINFO_FAST
 * Please Place Description here.
 * @var mac_ax_fw_status::FW_STATUS_MEMINFO_SLOW
 * Please Place Description here.
 * @var mac_ax_fw_status::FW_STATUS_PSINFO
 * Please Place Description here.
 * @var mac_ax_fw_status::FW_STATUS_H2C_C2HINFO
 * Please Place Description here.
 * @var mac_ax_fw_status::FW_STATUS_ISRINFO
 * Please Place Description here.
 * @var mac_ax_fw_status::FW_STATUS_MAX
 * Please Place Description here.
 */
enum mac_ax_fw_status {
	FW_STATUS_TASKINFO,
	FW_STATUS_FLASHINFO,
	FW_STATUS_HEAPINFO,
	FW_STATUS_MEMINFO_FAST,
	FW_STATUS_MEMINFO_SLOW,
	FW_STATUS_PSINFO,
	FW_STATUS_H2C_C2HINFO,
	FW_STATUS_ISRINFO,
	FW_STATUS_DBGINFO_REG,
	FW_STATUS_SERINFO,
	FW_STATUS_CHSW_TIMING,
	FW_STATUS_MAX
};

/**
 * @enum ISRStatistic
 *
 * @brief ISRStatistic
 *
 * @var ISRStatistic::ISRStatistic_WDT
 * Please Place Description here.
 * @var ISRStatistic::ISRStatistic_CMAC0
 * Please Place Description here.
 * @var ISRStatistic::ISRStatistic_CMAC1
 * Please Place Description here.
 * @var ISRStatistic::ISRStatistic_CMAC2
 * Please Place Description here.
 * @var ISRStatistic::ISRStatistic_CMAC3
 * Please Place Description here.
 * @var ISRStatistic::ISRStatistic_CMAC4
 * Please Place Description here.
 * @var ISRStatistic::ISRStatistic_CMAC5
 * Please Place Description here.
 * @var ISRStatistic::ISRStatistic_CMAC6
 * Please Place Description here.
 * @var ISRStatistic::ISRStatistic_CMAC7
 * Please Place Description here.
 * @var ISRStatistic::ISRStatistic_CMAC8
 * Please Place Description here.
 * @var ISRStatistic::ISRStatistic_CMAC9
 * Please Place Description here.
 * @var ISRStatistic::ISRStatistic_CMAC10
 * Please Place Description here.
 * @var ISRStatistic::ISRStatistic_DMAC0
 * Please Place Description here.
 * @var ISRStatistic::ISRStatistic_DMAC1
 * Please Place Description here.
 * @var ISRStatistic::ISRStatistic_DMAC2
 * Please Place Description here.
 * @var ISRStatistic::ISRStatistic_SYS0
 * Please Place Description here.
 * @var ISRStatistic::ISRStatistic_IPSec
 * Please Place Description here.
 * @var ISRStatistic::ISRStatistic_AXIDMA
 * Please Place Description here.
 * @var ISRStatistic::ISRStatistic_IDDMA
 * Please Place Description here.
 * @var ISRStatistic::ISRStatistic_MACErr
 * Please Place Description here.
 * @var ISRStatistic::ISRStatistic_UART
 * Please Place Description here.
 * @var ISRStatistic::ISRStatistic_RXI300
 * Please Place Description here.
 * @var ISRStatistic::ISRStatistic_DMAC0_GT0
 * Please Place Description here.
 * @var ISRStatistic::ISRStatistic_DMAC0_GT1
 * Please Place Description here.
 * @var ISRStatistic::ISRStatistic_DMAC0_GT2_3
 * Please Place Description here.
 * @var ISRStatistic::ISRStatistic_DMAC0_H2C
 * Please Place Description here.
 * @var ISRStatistic::ISRStatistic_SYS0_HALT_H2C
 * Please Place Description here.
 * @var ISRStatistic::ISRStatistic_SYS0_GPIO
 * Please Place Description here.
 * @var ISRStatistic::ISRStatistic_MACErr_DMAC
 * Please Place Description here.
 * @var ISRStatistic::ISRStatistic_MACErr_CMAC0
 * Please Place Description here.
 * @var ISRStatistic::ISRStatistic_MACErr_CMAC1
 * Please Place Description here.
 * @var ISRStatistic::ISRStatistic_MAX
 * Please Place Description here.
 */
enum ISRStatistic {
	/* Fisrt layer */
	// WDTISRROM8852A
	ISRStatistic_WDT = 0,	// no use currently
	// CMACISRROM8852A
	ISRStatistic_CMAC0 = 1,
	ISRStatistic_CMAC1 = 2,
	ISRStatistic_CMAC2 = 3,
	ISRStatistic_CMAC3 = 4,
	ISRStatistic_CMAC4 = 5,
	ISRStatistic_CMAC5 = 6,
	ISRStatistic_CMAC6 = 7,
	ISRStatistic_CMAC7 = 8,
	ISRStatistic_CMAC8 = 9,
	ISRStatistic_CMAC9 = 10,
	ISRStatistic_CMAC10 = 11,
	// DMACISRROM8852A
	ISRStatistic_DMAC0 = 12,
	ISRStatistic_DMAC1 = 13,
	ISRStatistic_DMAC2 = 14,
	// SysISRROM8852A
	ISRStatistic_SYS0 = 15,
	// DataHDLISRROM8852A
	ISRStatistic_IPSec = 16,
	ISRStatistic_AXIDMA = 17,
	ISRStatistic_IDDMA = 18,
	// PlatISRROM8852A
	ISRStatistic_MACErr = 19,
	ISRStatistic_UART = 20,
	ISRStatistic_RXI300 = 21,
	// Second layer
	ISRStatistic_DMAC0_GT0 = 22,
	ISRStatistic_DMAC0_GT1 = 23,
	ISRStatistic_DMAC0_GT2_3 = 24,
	ISRStatistic_DMAC0_H2C = 25,
	ISRStatistic_SYS0_HALT_H2C = 26,
	ISRStatistic_SYS0_GPIO = 27,
	ISRStatistic_MACErr_DMAC = 28,
	ISRStatistic_MACErr_CMAC0 = 29,
	ISRStatistic_MACErr_CMAC1 = 30,
	ISRStatistic_MAX = 31
};

/**
 * @struct mac_hal_cmd_info
 * @brief mac_hal_cmd_info
 *
 * @var mac_hal_cmd_info::name
 * Please Place Description here.
 * @var mac_hal_cmd_info::id
 * Please Place Description here.
 */
struct mac_hal_cmd_info {
	char name[16];
	u16 id;
	u32 (*handler)(struct mac_ax_adapter *adapter, char input[][MAC_MAX_ARGV],
		       u32 input_num, char *output, u32 out_len, u32 *used);
};

/**
 * @struct fw_status_pkt
 * @brief fw_status_pkt
 *
 * @var fw_status_pkt::op_code
 * Please Place Description here.
 * @var fw_status_pkt::length
 * Please Place Description here.
 * @var fw_status_pkt::data
 * Please Place Description here.
 */
struct fw_status_pkt {
	u32 op_code;
	u32 length;
	u8 *data;
};

/**
 * @struct flash_info
 * @brief flash_info
 *
 * @var flash_info::b1initdone
 * Please Place Description here.
 * @var flash_info::b7rsvd
 * Please Place Description here.
 * @var flash_info::pid
 * Please Place Description here.
 */
struct flash_info {
	u8 b1initdone: 1;
	u8 b7rsvd: 7;
	u8 pid[3];
	u8 u1flashtype;
	u8 u1dienum;
	u8 dieindex;
	u8 pageindex;
	u8 bsupportps; //PS is Page Switch. Page size is 16MB for 3-Bytes address mode.
	u8 u1addressmode;
	u8 u1dieunitsize; // unit is "Byte"
	u8 rsvd1;
	u32 u4dieunitnum; // unit number of die
	u32 u4flashsize; // flash total size = u4DieUnitNum * DieNum * FlashUnit.
};

/**
 * @struct mem_info
 * @brief mem_info
 *
 * @var mem_info::owner_id
 * Please Place Description here.
 * @var mem_info::owner_type
 * Please Place Description here.
 * @var mem_info::total_size
 * Please Place Description here.
 */
struct mem_info {
	u8 owner_id;
	u8 owner_type;
	u16 total_size;
};

/**
 * @struct isr_info
 * @brief isr_info
 *
 * @var isr_info::ISRCnt
 * Please Place Description here.
 * @var isr_info::ISRExecTimeMax_hi
 * Please Place Description here.
 * @var isr_info::ISRExecTimeMax_lo
 * Please Place Description here.
 */
struct isr_info {
	u32 ISRCnt[ISRStatistic_MAX];
	u32 ISRExecTimeMax_hi[ISRStatistic_MAX];
	u32 ISRExecTimeMax_lo[ISRStatistic_MAX];
};

/**
 * @struct chswofld_timing_info
 * @brief chswofld_timing_info
 *
 * @var chswofld_timing_info::mac
 * Please Place Description here.
 * @var chswofld_timing_info::bb
 * Please Place Description here.
 * @var chswofld_timing_info::rf
 * Please Place Description here.
 * @var chswofld_timing_info::rfReload
 * Please Place Description here.
 * @var chswofld_timing_info::total
 * Please Place Description here.

 */
struct chswofld_timing_info {
	u32 mac;
	u32 bb;
	u32 rf;
	u32 rf_reld;
	u32 total;
};

/**
 * @struct fw_status_proc_class
 * @brief fw_status_proc_class
 *
 * @var flash_info::b1initdone
 * Please Place Description here.
 * @var flash_info::b7rsvd
 * Please Place Description here.
 * @var flash_info::pid
 * Please Place Description here.
 */
struct halcmd_proc_class {
	u16 id;
	u32 (*handler)(struct mac_ax_adapter *adapter, char input[][MAC_MAX_ARGV]);
};

/**
 * @brief mac_fw_status_parser
 *
 * @param *adapter
 * @param *input
 * @param *input_num
 * @return Please Place Description here.
 * @retval void
 */
void mac_fw_status_parser(struct mac_ax_adapter *adapter, char input[][MAC_MAX_ARGV],
			  u32 input_num);

/**
 * @brief mac_halmac_cmd
 *
 * @param *adapter
 * @param *input
 * @param *output
 * @param *out_len
 * @return Please Place Description here.
 * @retval s32
 */
s32 mac_halmac_cmd(struct mac_ax_adapter *adapter, char *input, char *output, u32 out_len);

/**
 * @brief mac_halmac_cmd_parser
 *
 * @param *adapter
 * @param *input
 * @param *input_num
 * @param *output
 * @param *out_len
 * @return Please Place Description here.
 * @retval void
 */
void mac_halmac_cmd_parser(struct mac_ax_adapter *adapter,
			   char input[][MAC_MAX_ARGV], u32 input_num,
			   char *output, u32 out_len);

/**
 * @brief c2h_fw_status
 *
 * @param *adapter
 * @param *buf
 * @param *len
 * @param *info
 * @return Please Place Description here.
 * @retval void
 */
u32 c2h_fw_status(struct mac_ax_adapter *adapter, u8 *buf, u32 len,
		  struct rtw_c2h_info *info);

/**
 * @brief fw_status_taskinfo_handler
 *
 * @param *adapter
 * @param *buf
 * @param *len
 * @return Please Place Description here.
 * @retval void
 */
u32 fw_status_taskinfo_handler(struct mac_ax_adapter *adapter, u8 *buf, u32 len);

/**
 * @brief fw_status_flashinfo_handler
 *
 * @param *adapter
 * @param *buf
 * @param *len
 * @return Please Place Description here.
 * @retval void
 */
u32 fw_status_flashinfo_handler(struct mac_ax_adapter *adapter, u8 *buf, u32 len);

/**
 * @brief fw_status_heapinfo_handler
 *
 * @param *adapter
 * @param *buf
 * @param *len
 * @return Please Place Description here.
 * @retval void
 */
u32 fw_status_heapinfo_handler(struct mac_ax_adapter *adapter, u8 *buf, u32 len);

/**
 * @brief fw_status_meminfo_fast_handler
 *
 * @param *adapter
 * @param *buf
 * @param *len
 * @return Please Place Description here.
 * @retval void
 */
u32 fw_status_meminfo_fast_handler(struct mac_ax_adapter *adapter, u8 *buf, u32 len);

/**
 * @brief fw_status_meminfo_slow_handler
 *
 * @param *adapter
 * @param *buf
 * @param *len
 * @return Please Place Description here.
 * @retval void
 */
u32 fw_status_meminfo_slow_handler(struct mac_ax_adapter *adapter, u8 *buf, u32 len);

/**
 * @brief fw_status_psinfo_handler
 *
 * @param *adapter
 * @param *buf
 * @param *len
 * @return Please Place Description here.
 * @retval void
 */
u32 fw_status_psinfo_handler(struct mac_ax_adapter *adapter, u8 *buf, u32 len);

/**
 * @brief fw_status_h2c_c2hinfo_handler
 *
 * @param *adapter
 * @param *buf
 * @param *len
 * @return Please Place Description here.
 * @retval void
 */
u32 fw_status_h2c_c2hinfo_handler(struct mac_ax_adapter *adapter, u8 *buf, u32 len);

/**
 * @brief fw_status_isrinfo_handler
 *
 * @param *adapter
 * @param *buf
 * @param *len
 * @return Please Place Description here.
 * @retval void
 */
u32 fw_status_isrinfo_handler(struct mac_ax_adapter *adapter, u8 *buf, u32 len);

/**
 * @brief fw_status_chsw_handler
 *
 * @param *adapter
 * @param *buf
 * @param *len
 * @return Please Place Description here.
 * @retval void
 */
u32 fw_status_chsw_handler(struct mac_ax_adapter *adapter, u8 *buf, u32 len);

/**
 * @brief cmd_mac_help
 *
 * @param *adapter
 * @param *input
 * @param *input_num
 * @return Please Place Description here.
 * @retval void
 */
u32 cmd_mac_help(struct mac_ax_adapter *adapter, char input[][MAC_MAX_ARGV], u32 input_num,
		 char *output, u32 out_len, u32 *used);

/**
 * @brief cmd_mac_dbg_dump
 *
 * @param *adapter
 * @param *input
 * @param *input_num
 * @return Please Place Description here.
 * @retval void
 */
u32 cmd_mac_dbg_dump(struct mac_ax_adapter *adapter, char input[][MAC_MAX_ARGV], u32 input_num,
		     char *output, u32 out_len, u32 *used);

/**
 * @brief cmd_mac_reg_dump
 *
 * @param *adapter
 * @param *input
 * @param *input_num
 * @return Please Place Description here.
 * @retval void
 */
u32 cmd_mac_reg_dump(struct mac_ax_adapter *adapter, char input[][MAC_MAX_ARGV], u32 input_num,
		     char *output, u32 out_len, u32 *used);

/**
 * @brief cmd_mac_fw_dump
 *
 * @param *adapter
 * @param *input
 * @param *input_num
 * @return Please Place Description here.
 * @retval void
 */
u32 cmd_mac_fw_dump(struct mac_ax_adapter *adapter,  char input[][MAC_MAX_ARGV], u32 input_num,
		    char *output, u32 out_len, u32 *used);

/**
 * @brief cmd_mac_fw_log_cfg
 *
 * @param *adapter
 * @param *input
 * @param *input_num
 * @return Please Place Description here.
 * @retval void
 */
u32 cmd_mac_fw_log_cfg(struct mac_ax_adapter *adapter, char input[][MAC_MAX_ARGV], u32 input_num,
		       char *output, u32 out_len, u32 *used);
/**
 * @brief cmd_mac_fw_curtcb
 *
 * @param *adapter
 * @param *input
 * @param *input_num
 * @return Please Place Description here.
 * @retval void
 */
u32 cmd_mac_fw_curtcb(struct mac_ax_adapter *adapter,
		      char input[][MAC_MAX_ARGV],
		      u32 input_num,
		      char *output, u32 out_len, u32 *used);

/**
 * @brief cmd_mac_fw_status_parser
 *
 * @param *adapter
 * @param *input
 * @param *input_num
 * @return Please Place Description here.
 * @retval void
 */
u32 cmd_mac_fw_status_parser(struct mac_ax_adapter *adapter, char input[][MAC_MAX_ARGV],
			     u32 input_num, char *output, u32 out_len, u32 *used);

u32 cmd_mac_dl_sym(struct mac_ax_adapter *adapter, char input[][MAC_MAX_ARGV],
		   u32 input_num, char *output, u32 out_len, u32 *used);

/**
 * @brief cmd_mac_error_dump
 *
 * @param *adapter
 * @param *input
 * @param *input_num
 * @return Please Place Description here.
 * @retval void
 */
u32 cmd_mac_error_dump(struct mac_ax_adapter *adapter, char input[][MAC_MAX_ARGV], u32 input_num,
		       char *output, u32 out_len, u32 *used);

/**
 * @brief cmd_mac_dbg_tx_dump
 *
 * @param *adapter
 * @param *input
 * @param *input_num
 * @return Please Place Description here.
 * @retval void
 */
u32 cmd_mac_dbg_tx_dump(struct mac_ax_adapter *adapter,  char input[][MAC_MAX_ARGV],
			u32 input_num, char *output, u32 out_len, u32 *used);

/**
 * @brief cmd_mac_dbg_rx_dump
 *
 * @param *adapter
 * @param *input
 * @param *input_num
 * @return Please Place Description here.
 * @retval void
 */
u32 cmd_mac_dbg_rx_dump(struct mac_ax_adapter *adapter,  char input[][MAC_MAX_ARGV],
			u32 input_num, char *output, u32 out_len, u32 *used);

/**
 * @brief cmd_mac_dbg_dmac
 *
 * @param *adapter
 * @param *input
 * @param *input_num
 * @return Please Place Description here.
 * @retval void
 */
u32 cmd_mac_dbg_dmac(struct mac_ax_adapter *adapter,  char input[][MAC_MAX_ARGV], u32 input_num,
		     char *output, u32 out_len, u32 *used);

/**
 * @brief cmd_mac_dbg_cmac
 *
 * @param *adapter
 * @param *input
 * @param *input_num
 * @return Please Place Description here.
 * @retval void
 */
u32 cmd_mac_dbg_cmac(struct mac_ax_adapter *adapter,  char input[][MAC_MAX_ARGV], u32 input_num,
		     char *output, u32 out_len, u32 *used);

/**
 * @brief cmd_mac_dbg_bcn
 *
 * @param *adapter
 * @param *input
 * @param *input_num
 * @return Please Place Description here.
 * @retval void
 */
u32 cmd_mac_dbg_bcn(struct mac_ax_adapter *adapter,  char input[][MAC_MAX_ARGV], u32 input_num,
		    char *output, u32 out_len, u32 *used);

/**
 * @brief cmd_mac_ser_cnt_dump
 *
 * @param *adapter
 * @param *input
 * @param *input_num
 * @return Please Place Description here.
 * @retval void
 */
u32 cmd_mac_ser_cnt_dump(struct mac_ax_adapter *adapter,  char input[][MAC_MAX_ARGV], u32 input_num,
			 char *output, u32 out_len, u32 *used);

/**
 * @brief cmd_mac_qc_start
 *
 * @param *adapter
 * @param *input
 * @param *input_num
 * @return Please Place Description here.
 * @retval void
 */
u32 cmd_mac_qc_start(struct mac_ax_adapter *adapter,
		     char input[][MAC_MAX_ARGV],
		     u32 input_num,
		     char *output, u32 out_len, u32 *used);

/**
 * @brief cmd_mac_qc_end
 *
 * @param *adapter
 * @param *input
 * @param *input_num
 * @return Please Place Description here.
 * @retval void
 */
u32 cmd_mac_qc_end(struct mac_ax_adapter *adapter,
		   char input[][MAC_MAX_ARGV],
		   u32 input_num,
		   char *output, u32 out_len, u32 *used);

/**
 * @brief cmd_mac_req_pwr_st
 *
 * @param *adapter
 * @param *input
 * @param *input_num
 * @return Please Place Description here.
 * @retval void
 */
u32 cmd_mac_req_pwr_st(struct mac_ax_adapter *adapter,
		       char input[][MAC_MAX_ARGV],
		       u32 input_num,
		       char *output, u32 out_len, u32 *used);

/**
 * @brief cmd_mac_req_pwr_lvl
 *
 * @param *adapter
 * @param *input
 * @param *input_num
 * @return Please Place Description here.
 * @retval void
 */
u32 cmd_mac_req_pwr_lvl(struct mac_ax_adapter *adapter,
			char input[][MAC_MAX_ARGV],
			u32 input_num,
			char *output, u32 out_len, u32 *used);

/**
 * @brief fw_log_int_dump
 *
 * @param *adapter
 * @param *buf
 * @param *msgno
 * @param *para_num
 * @return Please Place Description here.
 * @retval void
 */
static void fw_log_int_dump(struct mac_ax_adapter *adapter, u32 *buf, u32 msgno, u8 para_num,
			    char *str_buf);

/**
 * @brief fw_log_scan_array
 *
 * @param *adapter
 * @param *msgno
 * @return Please Place Description here.
 * @retval u32
 */
u32 fw_log_scan_array(struct mac_ax_adapter *adapter, u32 msgno);

/**
 * @brief fw_log_set_array
 *
 * @param *adapter
 * @return Please Place Description here.
 * @retval u32
 */
void fw_log_set_array(struct mac_ax_adapter *adapter);

/**
 * @brief fw_log_dump
 *
 * @param *adapter
 * @param *buf
 * @param *len
 * @param *info
 * @return Please Place Description here.
 * @retval void
 */
void fw_log_dump(struct mac_ax_adapter *adapter, u8 *buf, u32 len,
		 struct rtw_c2h_info *info);

/**
 * @brief fw_log_skip_trailing_zero
 *
 * @param *s_ptr
 * @param *end_ptr
 * @return Please Place Description here.
 * @retval u8*
 */
u8 *fw_log_skip_trailing_zero(u8 *s_ptr, u8 *end_ptr);

/**
 * @brief fw_log_check_integrity
 *
 * @param *symbol_tab
 * @param *end_ptr
 * @return Please Place Description here.
 * @retval u32
 */
u32 fw_log_check_integrity(u8 *symbol_tab, u8 *end_ptr);

/**
 * @brief fw_log_create_array
 *
 * @param *array_ptr
 * @param *symbol_tab
 * @param *end_ptr
 * @param *enum_count
 * @param *enum_list
 * @return Please Place Description here.
 * @retval u32
 */
u32 fw_log_create_array(struct mac_ax_adapter *adapter, struct mac_fw_msg *array_ptr,
			u8 *symbol_tab, u8 *end_ptr, u32 enum_count, u8 *enum_list);

/**
 * @brief mac_fw_log_set_array
 *
 * @param *adapter
 * @param *symbol_ptr
 * @param *file_size
 * @return Please Place Description here.
 * @retval u32
 */
u32 mac_fw_log_set_array(struct mac_ax_adapter *adapter, void *symbol_ptr, u32 file_size);

/**
 * @brief mac_fw_log_unset_array
 *
 * @param *adapter
 * @return Please Place Description here.
 * @retval u32
 */
u32 mac_fw_log_unset_array(struct mac_ax_adapter *adapter);

/**
 * @brief cmd_mac_ser_level_dump
 *
 * @param *adapter
 * @return Please Place Description here.
 * @retval u32
 */
u32 cmd_mac_ser_level_dump(struct mac_ax_adapter *adapter,  char input[][MAC_MAX_ARGV],
			   u32 input_num, char *output, u32 out_len, u32 *used);

/**
 * @brief cmd_mac_fw_chsw
 *
 * @param *adapter
 * @return Please Place Description here.
 * @retval u32
 */
u32 cmd_mac_fw_chsw(struct mac_ax_adapter *adapter, char input[][MAC_MAX_ARGV], u32 input_num,
		    char *output, u32 out_len, u32 *used);

/**
 * @brief cmd_mac_fw_chsw_ret
 *
 * @param *adapter
 * @return Please Place Description here.
 * @retval u32
 */
u32 cmd_mac_fw_chsw_ret(struct mac_ax_adapter *adapter, char input[][MAC_MAX_ARGV], u32 input_num,
			char *output, u32 out_len, u32 *used);

/**
 * @brief cmd_mac_bcn_stats
 *
 * @param *adapter
 * @return Please Place Description here.
 * @retval u32
 */
u32 cmd_mac_bcn_stats(struct mac_ax_adapter *adapter, char input[][MAC_MAX_ARGV], u32 input_num,
		      char *output, u32 out_len, u32 *used);

#endif

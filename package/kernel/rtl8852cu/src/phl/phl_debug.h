/******************************************************************************
 *
 * Copyright(c) 2019 - 2021 Realtek Corporation.
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
 *****************************************************************************/
#ifndef _PHL_DEBUG_H_
#define _PHL_DEBUG_H_

/* phl log level */
enum {
	_PHL_NONE_ = 0,
	_PHL_ALWAYS_ = 1,
	_PHL_ERR_ = 2,
	_PHL_WARNING_ = 3,
	_PHL_INFO_ = 4,
	_PHL_DEBUG_ = 5,
	_PHL_MAX_ = 6
};

#define PHL_PREFIX "PHL: "
#define HALPS_PREFIX "HALPS:"

#define	PHL_DBG_OUTBUF(max_buff_len, used_len, buff_addr, remain_len, fmt, ...)\
	do {									\
		u32 *used_len_tmp = &(used_len);				\
		if (*used_len_tmp < max_buff_len)				\
			*used_len_tmp += _os_snprintf(buff_addr, remain_len, fmt, ##__VA_ARGS__);\
	} while (0)

#ifdef CONFIG_RTW_DEBUG

/*Define the tracing components*/
#define COMP_PHL_DBG BIT0		/* For function call tracing. */
#define COMP_PHL_RECV BIT1
#define COMP_PHL_XMIT BIT2
#define COMP_PHL_MAC BIT3
#define COMP_PHL_SOUND BIT4
#define COMP_PHL_WOW BIT5
#define COMP_PHL_TRIG BIT6
#define COMP_PHL_PKTOFLD BIT7
#define COMP_PHL_FSM BIT8
#define COMP_PHL_PS BIT9
#define COMP_PHL_PSTS BIT10
#define COMP_PHL_BB BIT11
#define COMP_PHL_RF BIT12
#define COMP_PHL_LED BIT13
#define COMP_PHL_MCC BIT14
#define COMP_PHL_P2PPS BIT15
#define COMP_PHL_ECSA BIT16
#define COMP_PHL_CMDDISP BIT17
#define COMP_PHL_BTC BIT18
#define COMP_PHL_TWT BIT19
#define COMP_PHL_OFDMA BIT20
#define COMP_PHL_GROUP BIT21
#define COMP_PHL_MR_COEX BIT22
#define COMP_PHL_CHINFO BIT23


enum PHL_DBG_LEVEL {
	_PHL_DBG = 0,
	_PHL_RECV,
	_PHL_XMIT,
	_PHL_MAC,
	_PHL_SOUND,
	_PHL_WOW,
	_PHL_TRIG,
	_PHL_PKTOFLD,
	_PHL_FSM,
	_PHL_PS,
	_PHL_PSTS,
	_PHL_TWT,
	_PHL_OFDMA,
	_PHL_GROUP,
};

struct phl_dbg_comp_list {
	char *name;
	enum PHL_DBG_LEVEL level;
	u32	level_comp;
};

extern u32 phl_log_components;
extern u8 phl_log_level;
#define DEBUG_MAX_NAME_LEN 50

struct dbg_alloc_buf {
	_os_list list;
	u8 file_name[DEBUG_MAX_NAME_LEN];
	u8 func_name[DEBUG_MAX_NAME_LEN];
	u32 line_num;
	u32 buf_size;
	u8 *buf_ptr;
};

struct dbg_mem_ctx {
	_os_list alloc_buf_list;
	_os_lock alloc_buf_list_lock;
	u32 alloc_buf_cnt;
};


#undef PHL_TRACE
#define PHL_TRACE(comp, level, fmt, ...)     \
	do {\
		if(((comp) & phl_log_components) && (level <= phl_log_level)) {\
			_os_dbgdump(PHL_PREFIX fmt, ##__VA_ARGS__);\
		} \
	} while (0)

#undef PHL_TRACE_LMT
#define PHL_TRACE_LMT(comp, level, fmt, ...)     \
	do {\
		if(((comp) & phl_log_components) && (level <= phl_log_level)) {\
			_os_dbgdump_lmt(PHL_PREFIX fmt, ##__VA_ARGS__);\
		} \
	} while (0)

#undef PHL_DATA
#define PHL_DATA(comp, level, fmt, ...)     \
	do {\
		if(((comp) & phl_log_components) && (level <= phl_log_level)) {\
			_os_dbgdump_c(fmt, ##__VA_ARGS__);\
		} \
	} while (0)

#undef PHL_DATA_LMT
#define PHL_DATA_LMT(comp, level, fmt, ...)     \
	do {\
		if(((comp) & phl_log_components) && (level <= phl_log_level)) {\
			_os_dbgdump_c_lmt(fmt, ##__VA_ARGS__);\
		} \
	} while (0)

#undef PHL_ASSERT
#define PHL_ASSERT(fmt, ...) \
	do {\
			PHL_TRACE(COMP_PHL_DBG, _PHL_ERR_, "ERROR " fmt, ##__VA_ARGS__);\
			_os_assert(true); \
	} while (0)


#undef PHL_PRINT
#define PHL_PRINT(fmt, ...)     \
	do {\
			PHL_TRACE(COMP_PHL_DBG, _PHL_ALWAYS_, fmt, ##__VA_ARGS__);\
	} while (0)

#undef PHL_PRINT_LMT
#define PHL_PRINT_LMT(fmt, ...)     \
	do {\
			PHL_TRACE_LMT(COMP_PHL_DBG, _PHL_ALWAYS_, fmt, ##__VA_ARGS__);\
	} while (0)

#undef PHL_ERR
#define PHL_ERR(fmt, ...)     \
	do {\
			PHL_TRACE(COMP_PHL_DBG, _PHL_ERR_, "ERROR " fmt, ##__VA_ARGS__);\
	} while (0)

#undef PHL_ERR_LMT
#define PHL_ERR_LMT(fmt, ...)     \
	do {\
			PHL_TRACE_LMT(COMP_PHL_DBG, _PHL_ERR_, "ERROR " fmt, ##__VA_ARGS__);\
	} while (0)

#undef PHL_WARN
#define PHL_WARN(fmt, ...)     \
	do {\
			PHL_TRACE(COMP_PHL_DBG, _PHL_WARNING_, "WARN " fmt, ##__VA_ARGS__);\
	} while (0)

#undef PHL_WARN_LMT
#define PHL_WARN_LMT(fmt, ...)     \
	do {\
			PHL_TRACE_LMT(COMP_PHL_DBG, _PHL_WARNING_, "WARN " fmt, ##__VA_ARGS__);\
	} while (0)

#undef PHL_INFO
#define PHL_INFO(fmt, ...)     \
	do {\
			PHL_TRACE(COMP_PHL_DBG, _PHL_INFO_, fmt, ##__VA_ARGS__);\
	} while (0)

#undef PHL_INFO_LMT
#define PHL_INFO_LMT(fmt, ...)     \
	do {\
			PHL_TRACE_LMT(COMP_PHL_DBG, _PHL_INFO_, fmt, ##__VA_ARGS__);\
	} while (0)

#undef PHL_DBG
#define PHL_DBG(fmt, ...)     \
	do {\
			PHL_TRACE(COMP_PHL_DBG, _PHL_DEBUG_, fmt, ##__VA_ARGS__);\
	} while (0)

#undef PHL_DBG_LMT
#define PHL_DBG_LMT(fmt, ...)     \
	do {\
			PHL_TRACE_LMT(COMP_PHL_DBG, _PHL_DEBUG_, fmt, ##__VA_ARGS__);\
	} while (0)

#define FUNCIN() PHL_TRACE(COMP_PHL_DBG, _PHL_DEBUG_, "Enter %s\n", __FUNCTION__)
#define FUNCOUT() PHL_TRACE(COMP_PHL_DBG, _PHL_DEBUG_, "Leave %s\n", __FUNCTION__)
#define FUNCIN_WSTS(_sts) PHL_TRACE(COMP_PHL_DBG, _PHL_DEBUG_, "Enter with 0x%08X %s\n", _sts, __FUNCTION__)
#define FUNCOUT_WSTS(_sts) PHL_TRACE(COMP_PHL_DBG, _PHL_DEBUG_, "Leave with 0x%08X %s\n", _sts, __FUNCTION__)

void debug_dump_buf(u8 *buf, u16 buf_len, const char *prefix);
void debug_dump_data(u8 *buf, u32 buf_len, const char *prefix);
void rt_alloc_dbg_buf(void *phl, u8 *buf_ptr, u32 buf_size,
		const u8 *file_name, u32 line_num, const u8 *func_name);
void rt_free_dbg_buf(void *phl, u8 *buf_ptr, u32 buf_size,
		const u8 *file_name, u32 line_num, const u8 *func_name);
void rt_mem_dbg_init(void *phl);
void rt_mem_dbg_deinit(void *phl);
u32 rtw_phl_dbg_ctrl_comp(u8 ctrl, u32 para_dbg);
void debug_dump_mac_address(u8 *mac_addr);

#define phl_ops_error_msg(ops_fun)	  \
	PHL_ERR("### %s - Please hook phl_hci_ops.%s ###\n", __func__, ops_fun)

#define hal_error_msg(ops_fun)		\
	PHL_PRINT("### %s - Error : Please hook hal_ops.%s ###\n", __FUNCTION__, ops_fun)

#else  /* CONFIG_RTW_DEBUG */

#define PHL_TRACE(comp, level, fmt, ...)
#define PHL_TRACE_LMT(comp, level, fmt, ...)
#define PHL_PRINT(fmt, ...)
#define PHL_PRINT_LMT(fmt, ...)
#define PHL_ERR(fmt, ...)
#define PHL_ERR_LMT(fmt, ...)
#define PHL_WARN(fmt, ...)
#define PHL_WARN_LMT(fmt, ...)
#define PHL_INFO(fmt, ...)
#define PHL_INFO_LMT(fmt, ...)
#define PHL_DBG(fmt, ...)
#define PHL_DBG_LMT(fmt, ...)
#define PHL_DATA(fmt, ...)
#define PHL_ASSERT(fmt, ...)
#define FUNCIN()
#define FUNCOUT()
#define FUNCIN_WSTS(_sts)
#define FUNCOUT_WSTS(_sts)
#define debug_dump_buf(_buf, _buf_len, _prefix)
#define debug_dump_data(_buf, _buf_len, _prefix)
#define debug_dump_mac_address(_mac_addr)
#define rt_alloc_dbg_buf(_phl, _buf, _buf_size, _file_name, _line_num, \
			_func_name)
#define rt_free_dbg_buf(_phl, _buf, _buf_size, _file_name, _line_num, \
			_func_name)
#define rt_mem_dbg_init(_phl)
#define rt_mem_dbg_deinit(_phl)

#define rtw_phl_dbg_ctrl_comp(_ctrl, _comp_bit) 0
#define phl_ops_error_msg(ops_fun) do {} while (0)
#define hal_error_msg(ops_fun) do {} while (0)

#endif  /* CONFIG_RTW_DEBUG */

#ifdef CONFIG_RTW_MIRROR_DUMP
enum {
	_PHL_MIR_CAT_H2C = 0,
	_PHL_MIR_CAT_C2H,
	_PHL_MIR_CAT_PKT,
	_PHL_MIR_CAT_TXD,
	_PHL_MIR_CAT_RXD,
	_PHL_MIR_CAT_MAX
};

enum {
	PHL_MIR_CFG_H2C = 0,
	PHL_MIR_CFG_C2H,
	PHL_MIR_CFG_TXD,
	PHL_MIR_CFG_RXD,
	PHL_MIR_CFG_TX,
	PHL_MIR_CFG_RX
};

void phl_mirror_dump_h2c(void *phl, struct rtw_h2c_pkt *pkt);
void phl_mirror_dump_c2h(void *phl, struct rtw_pkt_buf_list *pkt);
void phl_mirror_dump_wd(void *phl, u8 txch, u8 *wd, u32 wd_len);
void phl_mirror_dump_rxd(void *phl, u8 *rxd, u32 rxd_len);
#endif
#endif	/* _PHL_DEBUG_H_ */

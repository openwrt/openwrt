/******************************************************************************
 *
 * Copyright(c) 2007 - 2022 Realtek Corporation.
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
#ifndef __RTW_DEBUG_H__
#define __RTW_DEBUG_H__

/* driver log level*/
enum {
	_DRV_NONE_ = 0,
	_DRV_ALWAYS_ = 1,
	_DRV_ERR_ = 2,
	_DRV_WARNING_ = 3,
	_DRV_INFO_ = 4,
	_DRV_DEBUG_ = 5,
	_DRV_MAX_ = 6
};

#define DRIVER_PREFIX "RTW: "

#define RTW_PRINT(x, ...) do {} while (0)
#define RTW_ERR(x, ...) do {} while (0)
#define RTW_WARN(x,...) do {} while (0)
#define RTW_INFO(x,...) do {} while (0)
#define RTW_DBG(x,...) do {} while (0)
#define RTW_PRINT_SEL(x,...) do {} while (0)
#define _RTW_PRINT(x, ...) do {} while (0)
#define _RTW_ERR(x, ...) do {} while (0)
#define _RTW_WARN(x,...) do {} while (0)
#define _RTW_INFO(x,...) do {} while (0)
#define _RTW_DBG(x,...) do {} while (0)
#define _RTW_PRINT_SEL(x,...) do {} while (0)

#define RTW_INFO_DUMP(_TitleString, _HexData, _HexDataLen) do {} while (0)
#define RTW_DBG_DUMP(_TitleString, _HexData, _HexDataLen) do {} while (0)
#define RTW_PRINT_DUMP(_TitleString, _HexData, _HexDataLen) do {} while (0)

#define RTW_DBG_EXPR(EXPR) do {} while (0)

#define RTW_DBGDUMP 0 /* 'stream' for _dbgdump */



#undef _dbgdump
#undef _seqdump

#if defined(PLATFORM_LINUX)
	#ifdef DBG_THREAD_PID
	#define T_PID_FMT	"(%5u) "
	#define T_PID_ARG	current->pid
	#else /* !DBG_THREAD_PID */
	#define T_PID_FMT	"%s"
	#define T_PID_ARG	""
	#endif /* !DBG_THREAD_PID */

	#ifdef DBG_CPU_INFO
	#define CPU_INFO_FMT	"[%u] "
	#define CPU_INFO_ARG	task_cpu(current)
	#else /* !DBG_CPU_INFO */
	#define CPU_INFO_FMT	"%s"
	#define CPU_INFO_ARG	""
	#endif /* !DBG_CPU_INFO */

	/* Extra information in prefix */
	#define EX_INFO_FMT	T_PID_FMT CPU_INFO_FMT
	#define EX_INFO_ARG	T_PID_ARG, CPU_INFO_ARG

	#define _dbgdump(fmt, arg...)	printk(EX_INFO_FMT fmt, EX_INFO_ARG, ##arg)
	#define _dbgdump_lmt(fmt, arg...)	printk_ratelimited(EX_INFO_FMT fmt, EX_INFO_ARG, ##arg)

	#if (LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 24))
	#define KERN_CONT
	#endif
	/*
	 * _dbgdump_c() default use KERN_CONT flag, so it would not print
	 * messages with a new line. Usually use it if you want to continue
	 * a line.
	 */
	#define _dbgdump_c(fmt, arg...)	printk(KERN_CONT fmt, ##arg)
	#define _dbgdump_c_lmt(fmt, arg...)	printk_ratelimited(KERN_CONT fmt, ##arg)
	#define _seqdump seq_printf
#elif defined(PLATFORM_FREEBSD)
	#define _dbgdump printf
	#if (LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 24))
	#define KERN_CONT
	#endif
	#define _dbgdump_c(fmt, arg...)	printf(KERN_CONT fmt, ##arg)
	#define _seqdump(sel, fmt, arg...) _dbgdump(fmt, ##arg)
#endif

void RTW_BUF_DUMP_SEL(uint _loglevel, void *sel, u8 *_titlestring,
			bool _idx_show, const u8 *_hexdata, int _hexdatalen);
void RTW_BUF_DUMP_SEL_ALWAYS(void *sel, u8 *_titlestring,
			bool _idx_show, const u8 *_hexdata, int _hexdatalen);

#ifdef CONFIG_RTW_DEBUG

/*#ifndef _OS_INTFS_C_*/
extern uint rtw_drv_log_level;
/*#endif*/

#if defined(_dbgdump)

/* with driver-defined prefix */
#undef RTW_PRINT
#define RTW_PRINT(fmt, arg...)     \
	do {\
		if (_DRV_ALWAYS_ <= rtw_drv_log_level) {\
			_dbgdump(DRIVER_PREFIX fmt, ##arg);\
		} \
	} while (0)

#undef RTW_PRINT_LMT
#define RTW_PRINT_LMT(fmt, arg...)     \
	do {\
		if (_DRV_ALWAYS_ <= rtw_drv_log_level) {\
			_dbgdump_lmt(DRIVER_PREFIX fmt, ##arg);\
		} \
	} while (0)

#undef RTW_ERR
#define RTW_ERR(fmt, arg...)     \
	do {\
		if (_DRV_ERR_ <= rtw_drv_log_level) {\
			_dbgdump(DRIVER_PREFIX "ERROR " fmt, ##arg);\
		} \
	} while (0)

#undef RTW_ERR_LMT
#define RTW_ERR_LMT(fmt, arg...)     \
	do {\
		if (_DRV_ERR_ <= rtw_drv_log_level) {\
			_dbgdump_lmt(DRIVER_PREFIX "ERROR " fmt, ##arg);\
		} \
	} while (0)

#undef RTW_WARN
#define RTW_WARN(fmt, arg...)     \
	do {\
		if (_DRV_WARNING_ <= rtw_drv_log_level) {\
			_dbgdump(DRIVER_PREFIX "WARN " fmt, ##arg);\
		} \
	} while (0)

#undef RTW_WARN_LMT
#define RTW_WARN_LMT(fmt, arg...)     \
	do {\
		if (_DRV_WARNING_ <= rtw_drv_log_level) {\
			_dbgdump_lmt(DRIVER_PREFIX "WARN " fmt, ##arg);\
		} \
	} while (0)

#undef RTW_INFO
#define RTW_INFO(fmt, arg...)     \
	do {\
		if (_DRV_INFO_ <= rtw_drv_log_level) {\
			_dbgdump(DRIVER_PREFIX fmt, ##arg);\
		} \
	} while (0)

#undef RTW_INFO_LMT
#define RTW_INFO_LMT(fmt, arg...)     \
	do {\
		if (_DRV_INFO_ <= rtw_drv_log_level) {\
			_dbgdump_lmt(DRIVER_PREFIX fmt, ##arg);\
		} \
	} while (0)

#undef RTW_DBG
#define RTW_DBG(fmt, arg...)     \
	do {\
		if (_DRV_DEBUG_ <= rtw_drv_log_level) {\
			_dbgdump(DRIVER_PREFIX fmt, ##arg);\
		} \
	} while (0)

#undef RTW_DBG_LMT
#define RTW_DBG_LMT(fmt, arg...)     \
	do {\
		if (_DRV_DEBUG_ <= rtw_drv_log_level) {\
			_dbgdump_lmt(DRIVER_PREFIX fmt, ##arg);\
		} \
	} while (0)

#undef RTW_INFO_DUMP
#define RTW_INFO_DUMP(_TitleString, _HexData, _HexDataLen)	\
	RTW_BUF_DUMP_SEL(_DRV_INFO_, RTW_DBGDUMP, _TitleString, _FALSE, _HexData, _HexDataLen)

#undef RTW_DBG_DUMP
#define RTW_DBG_DUMP(_TitleString, _HexData, _HexDataLen)	\
	RTW_BUF_DUMP_SEL(_DRV_DEBUG_, RTW_DBGDUMP, _TitleString, _FALSE, _HexData, _HexDataLen)


#undef RTW_PRINT_DUMP
#define RTW_PRINT_DUMP(_TitleString, _HexData, _HexDataLen)	\
	RTW_BUF_DUMP_SEL(_DRV_ALWAYS_, RTW_DBGDUMP, _TitleString, _FALSE, _HexData, _HexDataLen)

/* without driver-defined prefix */
#undef _RTW_PRINT
#define _RTW_PRINT(fmt, arg...)     \
	do {\
		if (_DRV_ALWAYS_ <= rtw_drv_log_level) {\
			_dbgdump_c(fmt, ##arg);\
		} \
	} while (0)

#undef _RTW_PRINT_LMT
#define _RTW_PRINT_LMT(fmt, arg...)     \
	do {\
		if (_DRV_ALWAYS_ <= rtw_drv_log_level) {\
			_dbgdump_c_lmt(fmt, ##arg);\
		} \
	} while (0)

#undef _RTW_ERR
#define _RTW_ERR(fmt, arg...)     \
	do {\
		if (_DRV_ERR_ <= rtw_drv_log_level) {\
			_dbgdump_c(fmt, ##arg);\
		} \
	} while (0)

#undef _RTW_ERR_LMT
#define _RTW_ERR_LMT(fmt, arg...)     \
	do {\
		if (_DRV_ERR_ <= rtw_drv_log_level) {\
			_dbgdump_c_lmt(fmt, ##arg);\
		} \
	} while (0)

#undef _RTW_WARN
#define _RTW_WARN(fmt, arg...)     \
	do {\
		if (_DRV_WARNING_ <= rtw_drv_log_level) {\
			_dbgdump_c(fmt, ##arg);\
		} \
	} while (0)

#undef _RTW_WARN_LMT
#define _RTW_WARN_LMT(fmt, arg...)     \
	do {\
		if (_DRV_WARNING_ <= rtw_drv_log_level) {\
			_dbgdump_c_lmt(fmt, ##arg);\
		} \
	} while (0)

#undef _RTW_INFO
#define _RTW_INFO(fmt, arg...)     \
	do {\
		if (_DRV_INFO_ <= rtw_drv_log_level) {\
			_dbgdump_c(fmt, ##arg);\
		} \
	} while (0)

#undef _RTW_INFO_LMT
#define _RTW_INFO_LMT(fmt, arg...)     \
	do {\
		if (_DRV_INFO_ <= rtw_drv_log_level) {\
			_dbgdump_c_lmt(fmt, ##arg);\
		} \
	} while (0)

#undef _RTW_DBG
#define _RTW_DBG(fmt, arg...)     \
	do {\
		if (_DRV_DEBUG_ <= rtw_drv_log_level) {\
			_dbgdump_c(fmt, ##arg);\
		} \
	} while (0)

#undef _RTW_DBG_LMT
#define _RTW_DBG_LMT(fmt, arg...)     \
	do {\
		if (_DRV_DEBUG_ <= rtw_drv_log_level) {\
			_dbgdump_c_lmt(fmt, ##arg);\
		} \
	} while (0)


/* other debug APIs */
#undef RTW_DBG_EXPR
#define RTW_DBG_EXPR(EXPR) do { if (_DRV_DEBUG_ <= rtw_drv_log_level) EXPR; } while (0)

#endif /* defined(_dbgdump) */
#else
#undef _RTW_DBG_LMT
#define _RTW_DBG_LMT(fmt, arg...)     \
	do {\
	} while (0)

#undef RTW_WARN_LMT
#define RTW_WARN_LMT(fmt, arg...)     \
	do {\
	} while (0)
#endif /* CONFIG_RTW_DEBUG */

#if defined(_seqdump)
/* dump message to selected 'stream' with driver-defined prefix */
#undef RTW_PRINT_SEL
#define RTW_PRINT_SEL(sel, fmt, arg...) \
	do {\
		if (sel == RTW_DBGDUMP)\
			RTW_PRINT(fmt, ##arg); \
		else {\
			_seqdump(sel, fmt, ##arg) /*rtw_warn_on(1)*/; \
		} \
	} while (0)

/* dump message to selected 'stream' */
#undef _RTW_PRINT_SEL
#define _RTW_PRINT_SEL(sel, fmt, arg...) \
	do {\
		if (sel == RTW_DBGDUMP)\
			_RTW_PRINT(fmt, ##arg); \
		else {\
			_seqdump(sel, fmt, ##arg) /*rtw_warn_on(1)*/; \
		} \
	} while (0)

/* dump message to selected 'stream' */
#undef RTW_DUMP_SEL
#define RTW_DUMP_SEL(sel, _HexData, _HexDataLen) \
	RTW_BUF_DUMP_SEL(_DRV_ALWAYS_, sel, NULL, _FALSE, _HexData, _HexDataLen)

#define RTW_MAP_DUMP_SEL(sel, _TitleString, _HexData, _HexDataLen) \
	RTW_BUF_DUMP_SEL(_DRV_ALWAYS_, sel, _TitleString, _TRUE, _HexData, _HexDataLen)

#define RTW_MAP_DUMP_SEL_ALWAYS(sel, _TitleString, _HexData, _HexDataLen) \
	RTW_BUF_DUMP_SEL_ALWAYS(sel, _TitleString, _TRUE, _HexData, _HexDataLen)

#endif /* defined(_seqdump) */


#ifdef CONFIG_DBG_COUNTER
	#define DBG_COUNTER(counter) counter++
#else
	#define DBG_COUNTER(counter)
#endif

void dump_drv_version(void *sel);
void dump_log_level(void *sel);
void dump_drv_cfg(void *sel);

void rtw_sink_rtp_seq_dbg(_adapter *adapter, u8 *ehdr_pos);

#ifdef CONFIG_RECV_REORDERING_CTRL
struct sta_info;
void sta_rx_reorder_ctl_dump(void *sel, struct sta_info *sta);
#endif
struct dvobj_priv;
void dump_tx_rate_bmp(void *sel, struct dvobj_priv *dvobj);
void dump_adapters_status(void *sel, struct dvobj_priv *dvobj);

struct sec_cam_ent;
void dump_sec_cam_ent(void *sel, struct sec_cam_ent *ent, int id);
void dump_sec_cam_ent_title(void *sel, u8 has_id);
void dump_sec_cam(void *sel, _adapter *adapter);
void dump_sec_cam_cache(void *sel, _adapter *adapter);

bool rtw_del_rx_ampdu_test_trigger_no_tx_fail(void);
u32 rtw_get_wait_hiq_empty_ms(void);
void rtw_sta_linking_test_set_start(void);
bool rtw_sta_linking_test_wait_done(void);
bool rtw_sta_linking_test_force_fail(void);
#ifdef CONFIG_AP_MODE
u16 rtw_ap_linking_test_force_auth_fail(void);
u16 rtw_ap_linking_test_force_asoc_fail(void);
#endif

#ifdef CONFIG_PROC_DEBUG
ssize_t proc_set_write_reg(struct file *file, const char __user *buffer, size_t count, loff_t *pos, void *data);
int proc_get_read_reg(struct seq_file *m, void *v);
ssize_t proc_set_read_reg(struct file *file, const char __user *buffer, size_t count, loff_t *pos, void *data);

#ifdef CONFIG_IGNORE_GO_AND_LOW_RSSI_IN_SCAN_LIST
int proc_get_ignore_go_and_low_rssi_in_scan(struct seq_file *m, void *v);
ssize_t proc_set_ignore_go_and_low_rssi_in_scan(struct file *file,
	const char __user *buffer, size_t count, loff_t *pos, void *data);
#endif /*CONFIG_IGNORE_GO_AND_LOW_RSSI_IN_SCAN_LIST*/

ssize_t proc_set_mac_dbg_status_dump(struct file *file, const char __user *buffer, size_t count, loff_t *pos, void *data);

int proc_get_fwstate(struct seq_file *m, void *v);
int proc_get_sec_info(struct seq_file *m, void *v);
int proc_get_mlmext_state(struct seq_file *m, void *v);
#ifdef CONFIG_LAYER2_ROAMING
int proc_get_roam_flags(struct seq_file *m, void *v);
ssize_t proc_set_roam_flags(struct file *file, const char __user *buffer, size_t count, loff_t *pos, void *data);
int proc_get_roam_param(struct seq_file *m, void *v);
ssize_t proc_set_roam_param(struct file *file, const char __user *buffer, size_t count, loff_t *pos, void *data);
ssize_t proc_set_roam_tgt_addr(struct file *file, const char __user *buffer, size_t count, loff_t *pos, void *data);
#endif /* CONFIG_LAYER2_ROAMING */
int proc_get_qos_option(struct seq_file *m, void *v);
int proc_get_ht_option(struct seq_file *m, void *v);
int proc_get_rf_info(struct seq_file *m, void *v);
int proc_get_scan_param(struct seq_file *m, void *v);
ssize_t proc_set_scan_param(struct file *file, const char __user *buffer, size_t count, loff_t *pos, void *data);
int proc_get_scan_abort(struct seq_file *m, void *v);
ssize_t proc_set_scan_abort(struct file *file, const char __user *buffer, size_t count, loff_t *pos, void *data);

int proc_get_survey_info(struct seq_file *m, void *v);
ssize_t proc_set_survey_info(struct file *file, const char __user *buffer, size_t count, loff_t *pos, void *data);
int proc_get_ap_info(struct seq_file *m, void *v);
#ifdef PRIVATE_R
int proc_get_infra_ap(struct seq_file *m, void *v);
#endif /* PRIVATE_R */
ssize_t proc_reset_trx_info(struct file *file, const char __user *buffer, size_t count, loff_t *pos, void *data);
int proc_get_trx_info(struct seq_file *m, void *v);
ssize_t proc_set_tx_power_offset(struct file *file, const char __user *buffer, size_t count, loff_t *pos, void *data);
int proc_get_tx_power_offset(struct seq_file *m, void *v);
int proc_get_rate_ctl(struct seq_file *m, void *v);
int proc_get_wifi_spec(struct seq_file *m, void *v);
ssize_t proc_set_rate_ctl(struct file *file, const char __user *buffer, size_t count, loff_t *pos, void *data);
int proc_get_bw_ctl(struct seq_file *m, void *v);
ssize_t proc_set_bw_ctl(struct file *file, const char __user *buffer, size_t count, loff_t *pos, void *data);
#ifdef DBG_RX_COUNTER_DUMP
int proc_get_rx_cnt_dump(struct seq_file *m, void *v);
ssize_t proc_set_rx_cnt_dump(struct file *file, const char __user *buffer, size_t count, loff_t *pos, void *data);
#endif

#ifdef CONFIG_AP_MODE
int proc_get_bmc_tx_rate(struct seq_file *m, void *v);
ssize_t proc_set_bmc_tx_rate(struct file *file, const char __user *buffer, size_t count, loff_t *pos, void *data);
#endif /*CONFIG_AP_MODE*/

int proc_get_ps_dbg_info(struct seq_file *m, void *v);
ssize_t proc_set_ps_dbg_info(struct file *file, const char __user *buffer, size_t count, loff_t *pos, void *data);

ssize_t proc_set_del_rx_ampdu_test_case(struct file *file, const char __user *buffer, size_t count, loff_t *pos, void *data);
ssize_t proc_set_wait_hiq_empty(struct file *file, const char __user *buffer, size_t count, loff_t *pos, void *data);
ssize_t proc_set_sta_linking_test(struct file *file, const char __user *buffer, size_t count, loff_t *pos, void *data);
#ifdef CONFIG_AP_MODE
ssize_t proc_set_ap_linking_test(struct file *file, const char __user *buffer, size_t count, loff_t *pos, void *data);
#endif

struct sta_info;
int update_sta_stats(_adapter *adapter, struct sta_info *psta);
int proc_get_rx_stat(struct seq_file *m, void *v);
int proc_get_tx_stat(struct seq_file *m, void *v);
int proc_get_sta_tx_stat(struct seq_file *m, void *v);
ssize_t proc_set_sta_tx_stat(struct file *file, const char __user *buffer, size_t count, loff_t *pos, void *data);
#ifdef CONFIG_AP_MODE
int proc_get_sta_active_time(struct seq_file *m, void *v);
int proc_get_all_sta_info(struct seq_file *m, void *v);
#endif /* CONFIG_AP_MODE */

#ifdef DBG_MEMORY_LEAK
int proc_get_malloc_cnt(struct seq_file *m, void *v);
#endif /* DBG_MEMORY_LEAK */

#ifdef CONFIG_FIND_BEST_CHANNEL
int proc_get_best_channel(struct seq_file *m, void *v);
ssize_t proc_set_best_channel(struct file *file, const char __user *buffer, size_t count, loff_t *pos, void *data);
#endif /* CONFIG_FIND_BEST_CHANNEL */

int proc_get_trx_info_debug(struct seq_file *m, void *v);

#ifdef CONFIG_HUAWEI_PROC
int proc_get_huawei_trx_info(struct seq_file *m, void *v);
#endif

int proc_get_rx_signal(struct seq_file *m, void *v);
ssize_t proc_set_rx_signal(struct file *file, const char __user *buffer, size_t count, loff_t *pos, void *data);
int proc_get_hw_status(struct seq_file *m, void *v);
ssize_t proc_set_hw_status(struct file *file, const char __user *buffer, size_t count, loff_t *pos, void *data);
int proc_get_mac_rptbuf(struct seq_file *m, void *v);

#ifdef CONFIG_80211N_HT
int proc_get_ht_enable(struct seq_file *m, void *v);
ssize_t proc_set_ht_enable(struct file *file, const char __user *buffer, size_t count, loff_t *pos, void *data);

int proc_get_bw_mode(struct seq_file *m, void *v);
ssize_t proc_set_bw_mode(struct file *file, const char __user *buffer, size_t count, loff_t *pos, void *data);

int proc_get_ampdu_enable(struct seq_file *m, void *v);
ssize_t proc_set_ampdu_enable(struct file *file, const char __user *buffer, size_t count, loff_t *pos, void *data);

void dump_regsty_rx_ampdu_size_limit(void *sel, _adapter *adapter);
int proc_get_rx_ampdu(struct seq_file *m, void *v);
ssize_t proc_set_rx_ampdu(struct file *file, const char __user *buffer, size_t count, loff_t *pos, void *data);

void rtw_dump_dft_phy_cap(void *sel, _adapter *adapter);
void rtw_get_dft_phy_cap(void *sel, _adapter *adapter);
void rtw_dump_drv_phy_cap(void *sel, _adapter *adapter);
void rtw_dump_macaddr(void *sel, _adapter *adapter);

int proc_get_rx_stbc(struct seq_file *m, void *v);
ssize_t proc_set_rx_stbc(struct file *file, const char __user *buffer, size_t count, loff_t *pos, void *data);
int proc_get_stbc_cap(struct seq_file *m, void *v);
ssize_t proc_set_stbc_cap(struct file *file, const char __user *buffer, size_t count, loff_t *pos, void *data);
int proc_get_ldpc_cap(struct seq_file *m, void *v);
ssize_t proc_set_ldpc_cap(struct file *file, const char __user *buffer, size_t count, loff_t *pos, void *data);
#ifdef CONFIG_BEAMFORMING
int proc_get_txbf_cap(struct seq_file *m, void *v);
ssize_t proc_set_txbf_cap(struct file *file, const char __user *buffer, size_t count, loff_t *pos, void *data);
#endif
int proc_get_rx_ampdu_factor(struct seq_file *m, void *v);
ssize_t proc_set_rx_ampdu_factor(struct file *file, const char __user *buffer, size_t count, loff_t *pos, void *data);

int proc_get_tx_ampdu_num(struct seq_file *m, void *v);
ssize_t proc_set_tx_ampdu_num(struct file *file, const char __user *buffer, size_t count, loff_t *pos, void *data);

int proc_get_rx_ampdu_density(struct seq_file *m, void *v);
ssize_t proc_set_rx_ampdu_density(struct file *file, const char __user *buffer, size_t count, loff_t *pos, void *data);

int proc_get_tx_ampdu_density(struct seq_file *m, void *v);
ssize_t proc_set_tx_ampdu_density(struct file *file, const char __user *buffer, size_t count, loff_t *pos, void *data);

int proc_get_tx_quick_addba_req(struct seq_file *m, void *v);
ssize_t proc_set_tx_quick_addba_req(struct file *file, const char __user *buffer, size_t count, loff_t *pos, void *data);
#ifdef CONFIG_TX_AMSDU
int proc_get_tx_amsdu(struct seq_file *m, void *v);
ssize_t proc_set_tx_amsdu(struct file *file, const char __user *buffer, size_t count, loff_t *pos, void *data);
int proc_get_tx_amsdu_rate(struct seq_file *m, void *v);
ssize_t proc_set_tx_amsdu_rate(struct file *file, const char __user *buffer, size_t count, loff_t *pos, void *data);
#endif
#endif /* CONFIG_80211N_HT */

#ifdef CONFIG_80211AC_VHT
int proc_get_vht_24g_enable(struct seq_file *m, void *v);
ssize_t proc_set_vht_24g_enable(struct file *file, const char __user *buffer,
				size_t count, loff_t *pos, void *data);
#endif

ssize_t proc_set_dyn_rrsr(struct file *file, const char __user *buffer, size_t count, loff_t *pos, void *data);
int proc_get_dyn_rrsr(struct seq_file *m, void *v);

#if 0
int proc_get_two_path_rssi(struct seq_file *m, void *v);
int proc_get_rssi_disp(struct seq_file *m, void *v);
ssize_t proc_set_rssi_disp(struct file *file, const char __user *buffer, size_t count, loff_t *pos, void *data);
#endif

#if defined(DBG_CONFIG_ERROR_DETECT)
int proc_get_sreset(struct seq_file *m, void *v);
ssize_t proc_set_sreset(struct file *file, const char __user *buffer, size_t count, loff_t *pos, void *data);
#endif /* DBG_CONFIG_ERROR_DETECT */

int proc_get_phy_adaptivity(struct seq_file *m, void *v);

#ifdef CONFIG_DBG_COUNTER
int proc_get_rx_logs(struct seq_file *m, void *v);
int proc_get_tx_logs(struct seq_file *m, void *v);
int proc_get_int_logs(struct seq_file *m, void *v);
#endif

#ifdef CONFIG_PCI_HCI
int proc_get_rx_ring(struct seq_file *m, void *v);
int proc_get_tx_ring(struct seq_file *m, void *v);
int proc_get_pci_aspm(struct seq_file *m, void *v);
int proc_get_pci_conf_space(struct seq_file *m, void *v);
ssize_t proc_set_pci_conf_space(struct file *file, const char __user *buffer, size_t count, loff_t *pos, void *data);

int proc_get_pci_bridge_conf_space(struct seq_file *m, void *v);
ssize_t proc_set_pci_bridge_conf_space(struct file *file, const char __user *buffer, size_t count, loff_t *pos, void *data);


#ifdef DBG_TXBD_DESC_DUMP
int proc_get_tx_ring_ext(struct seq_file *m, void *v);
ssize_t proc_set_tx_ring_ext(struct file *file, const char __user *buffer, size_t count, loff_t *pos, void *data);
#endif
#endif

#ifdef CONFIG_WOWLAN
int proc_get_wow_enable(struct seq_file *m, void *v);
ssize_t proc_set_wow_enable(struct file *file, const char __user *buffer,
		size_t count, loff_t *pos, void *data);
int proc_get_pattern_info(struct seq_file *m, void *v);
ssize_t proc_set_pattern_info(struct file *file, const char __user *buffer,
		size_t count, loff_t *pos, void *data);
int proc_get_wakeup_event(struct seq_file *m, void *v);
ssize_t proc_set_wakeup_event(struct file *file, const char __user *buffer,
		size_t count, loff_t *pos, void *data);
int proc_get_wakeup_reason(struct seq_file *m, void *v);
int proc_get_wake_indication(struct seq_file *m, void *v);
#ifdef CONFIG_GPIO_WAKEUP
int proc_get_wowlan_gpio_info(struct seq_file *m, void *v);
ssize_t proc_set_wowlan_gpio_info(struct file *file, const char __user *buffer,
		size_t count, loff_t *pos, void *data);
ssize_t proc_set_wow_gpio_duration(struct file *file, const char __user *buffer,
		size_t count, loff_t *pos, void *data);
int proc_get_wow_gpio_duration(struct seq_file *m, void *v);
#endif /*CONFIG_GPIO_WAKEUP*/
#endif /* CONFIG_WOWLAN */

#ifdef CONFIG_P2P_WOWLAN
int proc_get_p2p_wowlan_info(struct seq_file *m, void *v);
#endif /* CONFIG_P2P_WOWLAN */

#ifdef CONFIG_POWER_SAVE
ssize_t proc_set_ps_info(struct file *file, const char __user *buffer, size_t count, loff_t *pos, void *data);
#endif /* CONFIG_POWER_SAVE */

int proc_get_new_bcn_max(struct seq_file *m, void *v);
ssize_t proc_set_new_bcn_max(struct file *file, const char __user *buffer, size_t count, loff_t *pos, void *data);

#ifdef CONFIG_TDLS
int proc_get_tdls_enable(struct seq_file *m, void *v);
ssize_t proc_set_tdls_enable(struct file *file, const char __user *buffer, size_t count, loff_t *pos, void *data);
int proc_get_tdls_info(struct seq_file *m, void *v);
#endif

int proc_get_monitor(struct seq_file *m, void *v);
ssize_t proc_set_monitor(struct file *file, const char __user *buffer, size_t count, loff_t *pos, void *data);

#ifdef DBG_XMIT_BLOCK
int proc_get_xmit_block(struct seq_file *m, void *v);
ssize_t proc_set_xmit_block(struct file *file, const char __user *buffer, size_t count, loff_t *pos, void *data);
#endif

#ifdef CONFIG_RTKM
int proc_get_rtkm_info(struct seq_file *m, void *v);
ssize_t proc_set_rtkm_info(struct file *file, const char __user *buffer,
			   size_t count, loff_t *pos, void *data);
#endif /* CONFIG_RTKM */

#ifdef CONFIG_IEEE80211W
ssize_t proc_set_tx_sa_query(struct file *file, const char __user *buffer, size_t count, loff_t *pos, void *data);
int proc_get_tx_sa_query(struct seq_file *m, void *v);
ssize_t proc_set_tx_deauth(struct file *file, const char __user *buffer, size_t count, loff_t *pos, void *data);
int proc_get_tx_deauth(struct seq_file *m, void *v);
ssize_t proc_set_tx_auth(struct file *file, const char __user *buffer, size_t count, loff_t *pos, void *data);
int proc_get_tx_auth(struct seq_file *m, void *v);
#endif /* CONFIG_IEEE80211W */

#endif /* CONFIG_PROC_DEBUG */

int proc_get_efuse_map(struct seq_file *m, void *v);
ssize_t proc_set_efuse_map(struct file *file, const char __user *buffer, size_t count, loff_t *pos, void *data);

int proc_get_ack_timeout(struct seq_file *m, void *v);
ssize_t proc_set_ack_timeout(struct file *file, const char __user *buffer, size_t count, loff_t *pos, void *data);

int proc_get_fw_info(struct seq_file *m, void *v);

int proc_get_fw_offload(struct seq_file *m, void *v);
ssize_t proc_set_fw_offload(struct file *file, const char __user *buffer, size_t count, loff_t *pos, void *data);


#ifdef CONFIG_DBG_RF_CAL
int proc_get_iqk_info(struct seq_file *m, void *v);
ssize_t proc_set_iqk(struct file *file, const char __user *buffer, size_t count, loff_t *pos, void *data);
int proc_get_lck_info(struct seq_file *m, void *v);
ssize_t proc_set_lck(struct file *file, const char __user *buffer, size_t count, loff_t *pos, void *data);
#endif /*CONFIG_DBG_RF_CAL*/

#ifdef CONFIG_CTRL_TXSS_BY_TP
ssize_t proc_set_txss_tp(struct file *file, const char __user *buffer, size_t count, loff_t *pos, void *data);
int proc_get_txss_tp(struct seq_file *m, void *v);
#ifdef DBG_CTRL_TXSS
ssize_t proc_set_txss_ctrl(struct file *file, const char __user *buffer, size_t count, loff_t *pos, void *data);
int proc_get_txss_ctrl(struct seq_file *m, void *v);
#endif
#endif

#ifdef CONFIG_SUPPORT_STATIC_SMPS
ssize_t proc_set_smps(struct file *file, const char __user *buffer, size_t count, loff_t *pos, void *data);
int proc_get_smps(struct seq_file *m, void *v);
#endif

int proc_get_defs_param(struct seq_file *m, void *v);
ssize_t proc_set_defs_param(struct file *file, const char __user *buffer, size_t count, loff_t *pos, void *data);

#ifdef RTW_DETECT_HANG
ssize_t proc_set_hang_info(struct file *file, const char __user *buffer,
			   size_t count, loff_t *pos, void *data);
int proc_get_hang_info(struct seq_file *m, void *v);
#endif

int proc_get_disconnect_info(struct seq_file *m, void *v);
ssize_t proc_set_disconnect_info(struct file *file, const char __user *buffer,
				 size_t count, loff_t *pos, void *data);

int proc_get_chan(struct seq_file *m, void *v);
ssize_t proc_set_chan(struct file *file, const char __user *buffer,
				size_t count, loff_t *pos, void *data);

int proc_get_mr_test(struct seq_file *m, void *v);
ssize_t proc_set_mr_test(struct file *file, const char __user *buffer, size_t count, loff_t *pos, void *data);
int proc_get_deny_legacy(struct seq_file *m, void *v);
ssize_t proc_set_deny_legacy(struct file *file, const char __user *buffer, size_t count, loff_t *pos, void *data);
int proc_get_tx_ul_mu_disable(struct seq_file *m, void *v);
ssize_t proc_set_tx_ul_mu_disable(struct file *file, const char __user *buffer, size_t count, loff_t *pos, void *data);

#define _drv_always_		1
#define _drv_emerg_			2
#define _drv_alert_			3
#define _drv_crit_			4
#define _drv_err_			5
#define _drv_warning_		6
#define _drv_notice_		7
#define _drv_info_			8
#define _drv_dump_			9
#define _drv_debug_			10

#define _module_rtl871x_xmit_c_		BIT(0)
#define _module_xmit_osdep_c_		BIT(1)
#define _module_rtl871x_recv_c_		BIT(2)
#define _module_recv_osdep_c_		BIT(3)
#define _module_rtl871x_mlme_c_		BIT(4)
#define _module_mlme_osdep_c_		BIT(5)
#define _module_rtl871x_sta_mgt_c_		BIT(6)
#define _module_rtl871x_cmd_c_			BIT(7)
#define _module_cmd_osdep_c_		BIT(8)
#define _module_rtl871x_io_c_				BIT(9)
#define _module_io_osdep_c_		BIT(10)
#define _module_os_intfs_c_			BIT(11)
#define _module_rtl871x_security_c_		BIT(12)
#define _module_rtl871x_eeprom_c_			BIT(13)
#define _module_hal_init_c_		BIT(14)
#define _module_hci_hal_init_c_		BIT(15)
#define _module_rtl871x_ioctl_c_		BIT(16)
#define _module_rtl871x_ioctl_set_c_		BIT(17)
#define _module_rtl871x_ioctl_query_c_	BIT(18)
#define _module_rtl871x_pwrctrl_c_			BIT(19)
#define _module_hci_intfs_c_			BIT(20)
#define _module_hci_ops_c_			BIT(21)
#define _module_osdep_service_c_			BIT(22)
#define _module_mp_			BIT(23)
#define _module_hci_ops_os_c_			BIT(24)
#define _module_rtl871x_ioctl_os_c		BIT(25)
#define _module_rtl8712_cmd_c_		BIT(26)
/* #define _module_efuse_			BIT(27) */
#define	_module_rtl8192c_xmit_c_ BIT(28)
#define _module_hal_xmit_c_	BIT(28)
#define _module_efuse_			BIT(29)
#define _module_rtl8712_recv_c_		BIT(30)
#define _module_rtl8712_led_c_		BIT(31)

#endif /* __RTW_DEBUG_H__ */

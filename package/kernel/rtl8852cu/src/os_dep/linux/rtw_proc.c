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

#include <linux/ctype.h>	/* tolower() */
#include <drv_types.h>
#include "rtw_proc.h"
#include <rtw_btc.h>
#include "_hal_rate.h"

#ifdef CONFIG_PROC_DEBUG

static struct proc_dir_entry *rtw_proc = NULL;

inline struct proc_dir_entry *get_rtw_drv_proc(void)
{
	return rtw_proc;
}

#define RTW_PROC_NAME DRV_NAME

#if (LINUX_VERSION_CODE < KERNEL_VERSION(3, 9, 0))
#define file_inode(file) ((file)->f_dentry->d_inode)
#endif

#if (LINUX_VERSION_CODE < KERNEL_VERSION(3, 10, 0))
#define PDE_DATA(inode) PDE((inode))->data
#define proc_get_parent_data(inode) PDE((inode))->parent->data
#elif (LINUX_VERSION_CODE >= KERNEL_VERSION(5, 17, 0))
#define PDE_DATA pde_data
#endif

#if (LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 24))
#define get_proc_net proc_net
#else
#define get_proc_net init_net.proc_net
#endif

#if (LINUX_VERSION_CODE < KERNEL_VERSION(3, 10, 0))
int single_open_size(struct file *file, int (*show)(struct seq_file *, void *),
		void *data, size_t size)
{
	char *buf = kmalloc(size, GFP_KERNEL);
	int ret;
	if (!buf)
		return -ENOMEM;
	ret = single_open(file, show, data);
	if (ret) {
		kfree(buf);
		return ret;
	}
	((struct seq_file *)file->private_data)->buf = buf;
	((struct seq_file *)file->private_data)->size = size;
	return 0;
}
#endif

inline struct proc_dir_entry *rtw_proc_create_dir(const char *name, struct proc_dir_entry *parent, void *data)
{
	struct proc_dir_entry *entry;

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3, 10, 0))
	entry = proc_mkdir_data(name, S_IRUGO | S_IXUGO, parent, data);
#else
	/* entry = proc_mkdir_mode(name, S_IRUGO|S_IXUGO, parent); */
	entry = proc_mkdir(name, parent);
	if (entry)
		entry->data = data;
#endif

	return entry;
}

inline struct proc_dir_entry *rtw_proc_create_entry(const char *name, struct proc_dir_entry *parent,
	const struct rtw_proc_ops *fops, void * data)
{
	struct proc_dir_entry *entry;

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 26))
	entry = proc_create_data(name,  S_IFREG | S_IRUGO | S_IWUGO, parent, fops, data);
#else
	entry = create_proc_entry(name, S_IFREG | S_IRUGO | S_IWUGO, parent);
	if (entry) {
		entry->data = data;
		entry->proc_fops = fops;
	}
#endif

	return entry;
}

static int proc_get_dummy(struct seq_file *m, void *v)
{
	return 0;
}

static int proc_get_drv_version(struct seq_file *m, void *v)
{
	dump_drv_version(m);
	return 0;
}

static int proc_get_log_level(struct seq_file *m, void *v)
{
	dump_log_level(m);
	return 0;
}

static int proc_get_drv_cfg(struct seq_file *m, void *v)
{
	dump_drv_cfg(m);
	return 0;
}

static ssize_t proc_set_log_level(struct file *file, const char __user *buffer, size_t count, loff_t *pos, void *data)
{
	char tmp[32];
	int log_level;

	if (count < 1)
		return -EINVAL;

	if (count > sizeof(tmp)) {
		rtw_warn_on(1);
		return -EFAULT;
	}

#ifdef CONFIG_RTW_DEBUG
	if (buffer && !copy_from_user(tmp, buffer, count)) {

		int num = sscanf(tmp, "%d ", &log_level);

		if (num == 1 &&
		    log_level >= _DRV_NONE_ && log_level <= _DRV_MAX_) {
			rtw_drv_log_level = log_level;
			printk("rtw_drv_log_level:%d\n", rtw_drv_log_level);
		}
	} else
		return -EFAULT;
#else
	printk("CONFIG_RTW_DEBUG is disabled\n");
#endif

	return count;
}

#ifdef DBG_MEM_ALLOC
static int proc_get_mstat(struct seq_file *m, void *v)
{
	rtw_mstat_dump(m);
	return 0;
}
#endif /* DBG_MEM_ALLOC */

static bool regd_info;
static int proc_get_country_chplan_map(struct seq_file *m, void *v)
{
	dump_country_chplan_map(m, regd_info);
	return 0;
}

static ssize_t proc_set_country_chplan_map(struct file *file, const char __user *buffer, size_t count, loff_t *pos, void *data)
{
	char tmp[32];
	int regd_info_val;

	if (count < 1)
		return -EINVAL;

	if (count > sizeof(tmp)) {
		rtw_warn_on(1);
		return -EFAULT;
	}

	if (buffer && !copy_from_user(tmp, buffer, count)) {
		int num = sscanf(tmp, "%d", &regd_info_val);

		if (num >= 1)
			regd_info = regd_info_val ? 1 : 0;
	} else
		return -EFAULT;

	return count;
}

static int proc_get_country_list(struct seq_file *m, void *v)
{
	dump_country_list(m);
	return 0;
}

static int proc_get_chplan_id_list(struct seq_file *m, void *v)
{
	dump_chplan_id_list(m);
	return 0;
}

static int proc_get_chplan_country_list(struct seq_file *m, void *v)
{
	dump_chplan_country_list(m);
	return 0;
}

#if CONFIG_IEEE80211_BAND_6GHZ
static int proc_get_chplan_6g_id_list(struct seq_file *m, void *v)
{
	dump_chplan_6g_id_list(m);
	return 0;
}

static int proc_get_chplan_6g_country_list(struct seq_file *m, void *v)
{
	dump_chplan_6g_country_list(m);
	return 0;
}
#endif

#ifdef CONFIG_RTW_DEBUG
static int proc_get_chplan_test(struct seq_file *m, void *v)
{
	dump_chplan_test(m);
	return 0;
}
#endif

static int proc_get_chplan_ver(struct seq_file *m, void *v)
{
	dump_chplan_ver(m);
	return 0;
}

static int proc_get_global_op_class(struct seq_file *m, void *v)
{
	dump_global_op_class(m);
	return 0;
}

extern void rtw_hal_get_version(char *str, u32 len);

static int proc_get_hal_info(struct seq_file *m, void *v)
{
	char ver[30] = {0};

	rtw_hal_get_version(ver, 30);

	RTW_PRINT_SEL(m, "version: %s\n", ver);

	return 0;
}


/*
* rtw_drv_proc:
* init/deinit when register/unregister driver
*/
const struct rtw_proc_hdl drv_proc_hdls[] = {
	RTW_PROC_HDL_SSEQ("ver_info", proc_get_drv_version, NULL),
	RTW_PROC_HDL_SSEQ("log_level", proc_get_log_level, proc_set_log_level),
	RTW_PROC_HDL_SSEQ("drv_cfg", proc_get_drv_cfg, NULL),
#ifdef DBG_MEM_ALLOC
	RTW_PROC_HDL_SSEQ("mstat", proc_get_mstat, NULL),
#endif /* DBG_MEM_ALLOC */
	RTW_PROC_HDL_SSEQ("country_chplan_map", proc_get_country_chplan_map, proc_set_country_chplan_map),
	RTW_PROC_HDL_SSEQ("country_list", proc_get_country_list, NULL),
	RTW_PROC_HDL_SSEQ("chplan_id_list", proc_get_chplan_id_list, NULL),
	RTW_PROC_HDL_SSEQ("chplan_country_list", proc_get_chplan_country_list, NULL),
#if CONFIG_IEEE80211_BAND_6GHZ
	RTW_PROC_HDL_SSEQ("chplan_6g_id_list", proc_get_chplan_6g_id_list, NULL),
	RTW_PROC_HDL_SSEQ("chplan_6g_country_list", proc_get_chplan_6g_country_list, NULL),
#endif
#ifdef CONFIG_RTW_DEBUG
	RTW_PROC_HDL_SSEQ("chplan_test", proc_get_chplan_test, NULL),
#endif
	RTW_PROC_HDL_SSEQ("chplan_ver", proc_get_chplan_ver, NULL),
	RTW_PROC_HDL_SSEQ("global_op_class", proc_get_global_op_class, NULL),
	RTW_PROC_HDL_SSEQ("hal_info", proc_get_hal_info, NULL),
#ifdef CONFIG_RTKM
	RTW_PROC_HDL_SSEQ("rtkm", proc_get_rtkm_info, proc_set_rtkm_info),
#endif /* CONFIG_RTKM */
};

const int drv_proc_hdls_num = sizeof(drv_proc_hdls) / sizeof(struct rtw_proc_hdl);

static int rtw_drv_proc_open(struct inode *inode, struct file *file)
{
	/* struct net_device *dev = proc_get_parent_data(inode); */
	ssize_t index = (ssize_t)PDE_DATA(inode);
	const struct rtw_proc_hdl *hdl = drv_proc_hdls + index;
	void *private = NULL;

	if (hdl->type == RTW_PROC_HDL_TYPE_SEQ) {
		int res = seq_open(file, hdl->u.seq_op);

		if (res == 0)
			((struct seq_file *)file->private_data)->private = private;

		return res;
	} else if (hdl->type == RTW_PROC_HDL_TYPE_SSEQ) {
		int (*show)(struct seq_file *, void *) = hdl->u.show ? hdl->u.show : proc_get_dummy;

		return single_open(file, show, private);
	} else if (hdl->type == RTW_PROC_HDL_TYPE_SZSEQ) {
		int (*show)(struct seq_file *, void *) = hdl->u.sz.show ? hdl->u.sz.show : proc_get_dummy;
		#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3, 10, 0))
		return single_open_size(file, show, private, hdl->u.sz.size);
		#else
		return single_open(file, show, private);
		#endif
	} else {
		return -EROFS;
	}
}

static ssize_t rtw_drv_proc_write(struct file *file, const char __user *buffer, size_t count, loff_t *pos)
{
	ssize_t index = (ssize_t)PDE_DATA(file_inode(file));
	const struct rtw_proc_hdl *hdl = drv_proc_hdls + index;
	ssize_t (*write)(struct file *, const char __user *, size_t, loff_t *, void *) = hdl->write;

	if (write)
		return write(file, buffer, count, pos, NULL);

	return -EROFS;
}

static const struct rtw_proc_ops rtw_drv_proc_seq_fops = {
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(5, 6, 0))
	.proc_open = rtw_drv_proc_open,
	.proc_read = seq_read,
	.proc_lseek = seq_lseek,
	.proc_release = seq_release,
	.proc_write = rtw_drv_proc_write,
#else
	.owner = THIS_MODULE,
	.open = rtw_drv_proc_open,
	.read = seq_read,
	.llseek = seq_lseek,
	.release = seq_release,
	.write = rtw_drv_proc_write,
#endif
};

static const struct rtw_proc_ops rtw_drv_proc_sseq_fops = {
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(5, 6, 0))
	.proc_open = rtw_drv_proc_open,
	.proc_read = seq_read,
	.proc_lseek = seq_lseek,
	.proc_release = single_release,
	.proc_write = rtw_drv_proc_write,
#else
	.owner = THIS_MODULE,
	.open = rtw_drv_proc_open,
	.read = seq_read,
	.llseek = seq_lseek,
	.release = single_release,
	.write = rtw_drv_proc_write,
#endif
};

int rtw_drv_proc_init(void)
{
	int ret = _FAIL;
	ssize_t i;
	struct proc_dir_entry *entry = NULL;

	if (rtw_proc != NULL) {
		rtw_warn_on(1);
		goto exit;
	}

	rtw_proc = rtw_proc_create_dir(RTW_PROC_NAME, get_proc_net, NULL);

	if (rtw_proc == NULL) {
		rtw_warn_on(1);
		goto exit;
	}

	for (i = 0; i < drv_proc_hdls_num; i++) {
		if (drv_proc_hdls[i].type == RTW_PROC_HDL_TYPE_SEQ)
			entry = rtw_proc_create_entry(drv_proc_hdls[i].name, rtw_proc, &rtw_drv_proc_seq_fops, (void *)i);
		else if (drv_proc_hdls[i].type == RTW_PROC_HDL_TYPE_SSEQ ||
			drv_proc_hdls[i].type == RTW_PROC_HDL_TYPE_SZSEQ)
			entry = rtw_proc_create_entry(drv_proc_hdls[i].name, rtw_proc, &rtw_drv_proc_sseq_fops, (void *)i);
		else
			entry = NULL;

		if (!entry) {
			rtw_warn_on(1);
			goto exit;
		}
	}

	ret = _SUCCESS;

exit:
	return ret;
}

void rtw_drv_proc_deinit(void)
{
	int i;

	if (rtw_proc == NULL)
		return;

	for (i = 0; i < drv_proc_hdls_num; i++)
		remove_proc_entry(drv_proc_hdls[i].name, rtw_proc);

	remove_proc_entry(RTW_PROC_NAME, get_proc_net);
	rtw_proc = NULL;
}

#ifndef RTW_SEQ_FILE_TEST
#define RTW_SEQ_FILE_TEST 0
#endif

#if RTW_SEQ_FILE_TEST
#define RTW_SEQ_FILE_TEST_SHOW_LIMIT 300
static void *proc_start_seq_file_test(struct seq_file *m, loff_t *pos)
{
	struct net_device *dev = m->private;
	_adapter *adapter = (_adapter *)rtw_netdev_priv(dev);

	RTW_PRINT(FUNC_ADPT_FMT"\n", FUNC_ADPT_ARG(adapter));
	if (*pos >= RTW_SEQ_FILE_TEST_SHOW_LIMIT) {
		RTW_PRINT(FUNC_ADPT_FMT" pos:%llu, out of range return\n", FUNC_ADPT_ARG(adapter), *pos);
		return NULL;
	}

	RTW_PRINT(FUNC_ADPT_FMT" return pos:%lld\n", FUNC_ADPT_ARG(adapter), *pos);
	return pos;
}
void proc_stop_seq_file_test(struct seq_file *m, void *v)
{
	struct net_device *dev = m->private;
	_adapter *adapter = (_adapter *)rtw_netdev_priv(dev);

	RTW_PRINT(FUNC_ADPT_FMT"\n", FUNC_ADPT_ARG(adapter));
}

void *proc_next_seq_file_test(struct seq_file *m, void *v, loff_t *pos)
{
	struct net_device *dev = m->private;
	_adapter *adapter = (_adapter *)rtw_netdev_priv(dev);

	(*pos)++;
	if (*pos >= RTW_SEQ_FILE_TEST_SHOW_LIMIT) {
		RTW_PRINT(FUNC_ADPT_FMT" pos:%lld, out of range return\n", FUNC_ADPT_ARG(adapter), *pos);
		return NULL;
	}

	RTW_PRINT(FUNC_ADPT_FMT" return pos:%lld\n", FUNC_ADPT_ARG(adapter), *pos);
	return pos;
}

static int proc_get_seq_file_test(struct seq_file *m, void *v)
{
	struct net_device *dev = m->private;
	_adapter *adapter = (_adapter *)rtw_netdev_priv(dev);

	u32 pos = *((loff_t *)(v));
	RTW_PRINT(FUNC_ADPT_FMT" pos:%d\n", FUNC_ADPT_ARG(adapter), pos);
	RTW_PRINT_SEL(m, FUNC_ADPT_FMT" pos:%d\n", FUNC_ADPT_ARG(adapter), pos);
	return 0;
}

struct seq_operations seq_file_test = {
	.start = proc_start_seq_file_test,
	.stop  = proc_stop_seq_file_test,
	.next  = proc_next_seq_file_test,
	.show  = proc_get_seq_file_test,
};
#endif /* RTW_SEQ_FILE_TEST */

#ifdef CONFIG_SDIO_HCI
#include "rtw_sdio.h"
#define CCCR_LEN	0x100
static void sd_f0_reg_dump(void *sel, struct _ADAPTER *a)
{
	struct dvobj_priv *d = adapter_to_dvobj(a);
	u8 *cccr = NULL, *ptr, val;
	u8 ret;
	int i;


	cccr = rtw_vmalloc(CCCR_LEN);
	if (cccr) {
		ret = rtw_sdio_f0_read(d, 0, cccr, CCCR_LEN);
		if (ret == _FAIL) {
			RTW_PRINT_SEL(sel, "Fail to read F0 register!");
			goto exit;
		}
	} else {
		RTW_WARN(FUNC_ADPT_FMT ": alloc memory FAIL!\n",
			 FUNC_ADPT_ARG(a));
		ptr = &val;
	}

	for (i = 0x0; i < CCCR_LEN; i++) {
		if (cccr) {
			ptr = &cccr[i];
		} else {
			ret = rtw_sdio_f0_read(d, i, &val, 1);
			if (ret == _FAIL) {
				RTW_ERR(FUNC_ADPT_FMT ": Fail to read F0 "
					"register 0x%02x!\n",
					FUNC_ADPT_ARG(a), i);
				RTW_PRINT_SEL(sel, "... error!\n");
				goto exit;
			}
		}

		if (i % 16 == 0)
			RTW_PRINT_SEL(sel, "0x%02x ", i);

		_RTW_PRINT_SEL(sel, "%02x ", *ptr);

		if (i % 16 == 15)
			_RTW_PRINT_SEL(sel, "\n");
		else if (i % 8 == 7)
			_RTW_PRINT_SEL(sel, "\t");
	}

exit:
	if (cccr)
		rtw_vmfree(cccr, CCCR_LEN);
}

static int proc_get_sd_f0_reg_dump(struct seq_file *m, void *v)
{
	struct net_device *dev = m->private;
	_adapter *adapter = (_adapter *)rtw_netdev_priv(dev);
//	struct dvobj_priv *d = adapter_to_dvobj(adapter);

//	rtw_hal_sd_f0_reg_dump(m, GET_PHL_INFO(d));
	sd_f0_reg_dump(m, adapter);

	return 0;
}

static int proc_get_sdio_local_reg_dump(struct seq_file *m, void *v)
{
	struct net_device *dev = m->private;
	_adapter *adapter = (_adapter *)rtw_netdev_priv(dev);
	struct dvobj_priv *d = adapter_to_dvobj(adapter);

	rtw_hal_sdio_local_reg_dump(m, GET_PHL_INFO(d));

	return 0;
}
static int proc_get_sdio_card_info(struct seq_file *m, void *v)
{
	struct net_device *dev = m->private;
	_adapter *adapter = (_adapter *)rtw_netdev_priv(dev);

	dump_sdio_card_info(m, adapter_to_dvobj(adapter));

	return 0;
}

#ifdef DBG_SDIO
static int proc_get_sdio_dbg(struct seq_file *m, void *v)
{
	struct net_device *dev;
	_adapter *a;
	struct dvobj_priv *d;
	struct sdio_data *sdio;


	dev = m->private;
	a = (_adapter *)rtw_netdev_priv(dev);
	d = adapter_to_dvobj(a);
	sdio = dvobj_to_sdio(d);

	dump_sdio_card_info(m, d);

	RTW_PRINT_SEL(m, "CMD52 error cnt: %d\n", sdio->cmd52_err_cnt);
	RTW_PRINT_SEL(m, "CMD53 error cnt: %d\n", sdio->cmd53_err_cnt);

#if (DBG_SDIO >= 3)
	RTW_PRINT_SEL(m, "dbg: %s\n", sdio->dbg_enable?"enable":"disable");
	RTW_PRINT_SEL(m, "err_stop: %s\n", sdio->err_stop?"enable":"disable");
	RTW_PRINT_SEL(m, "err_test: %s\n", sdio->err_test?"enable":"disable");
	RTW_PRINT_SEL(m, "err_test_triggered: %s\n",
		      sdio->err_test_triggered?"yes":"no");
#endif /* DBG_SDIO >= 3 */

#if (DBG_SDIO >= 2)
	RTW_PRINT_SEL(m, "I/O error dump mark: %d\n", sdio->reg_dump_mark);
	if (sdio->reg_dump_mark) {
		if (sdio->dbg_msg)
			RTW_PRINT_SEL(m, "debug messages: %s\n", sdio->dbg_msg);
		if (sdio->reg_mac)
			RTW_BUF_DUMP_SEL(_DRV_ALWAYS_, m, "MAC register:",
					 _TRUE, sdio->reg_mac, 0x800);
		if (sdio->reg_mac_ext)
			RTW_BUF_DUMP_SEL(_DRV_ALWAYS_, m, "MAC EXT register:",
					 _TRUE, sdio->reg_mac_ext, 0x800);
		if (sdio->reg_local)
			RTW_BUF_DUMP_SEL(_DRV_ALWAYS_, m, "SDIO Local register:",
					 _TRUE, sdio->reg_local, 0x100);
		if (sdio->reg_cia)
			RTW_BUF_DUMP_SEL(_DRV_ALWAYS_, m, "SDIO CIA register:",
					 _TRUE, sdio->reg_cia, 0x200);
	}
#endif /* DBG_SDIO >= 2 */

	return 0;
}

#if (DBG_SDIO >= 2)
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(4, 0, 0))
#define strnicmp	strncasecmp
#endif /* Linux kernel >= 4.0.0 */
void rtw_sdio_dbg_reg_free(struct dvobj_priv *d);
#endif /* DBG_SDIO >= 2 */

ssize_t proc_set_sdio_dbg(struct file *file, const char __user *buffer,
			  size_t count, loff_t *pos, void *data)
{
#if (DBG_SDIO >= 2)
	struct net_device *dev = data;
	struct dvobj_priv *d;
	_adapter *a;
	struct sdio_data *sdio;
	char tmp[32], cmd[32] = {0};
	int num;


	if (count < 1)
		return -EFAULT;

	if (count > sizeof(tmp)) {
		rtw_warn_on(1);
		return -EFAULT;
	}

	a = (_adapter *)rtw_netdev_priv(dev);
	d = adapter_to_dvobj(a);
	sdio = dvobj_to_sdio(d);

	if (buffer && !copy_from_user(tmp, buffer, count)) {
		num = sscanf(tmp, "%s", cmd);

		if (num >= 1) {
			if (strnicmp(cmd, "reg_reset", 10) == 0) {
				sdio->reg_dump_mark = 0;
				goto exit;
			}
			if (strnicmp(cmd, "reg_free", 9) == 0) {
				rtw_sdio_dbg_reg_free(d);
				sdio->reg_dump_mark = 0;
				goto exit;
			}
#if (DBG_SDIO >= 3)
			if (strnicmp(cmd, "dbg_enable", 11) == 0) {
				sdio->dbg_enable = 1;
				goto exit;
			}
			if (strnicmp(cmd, "dbg_disable", 12) == 0) {
				sdio->dbg_enable = 0;
				goto exit;
			}
			if (strnicmp(cmd, "err_stop", 9) == 0) {
				sdio->err_stop = 1;
				goto exit;
			}
			if (strnicmp(cmd, "err_stop_disable", 16) == 0) {
				sdio->err_stop = 0;
				goto exit;
			}
			if (strnicmp(cmd, "err_test", 9) == 0) {
				sdio->err_test_triggered = 0;
				sdio->err_test = 1;
				goto exit;
			}
#endif /* DBG_SDIO >= 3 */
		}

		return -EINVAL;
	}

exit:
#endif /* DBG_SDIO >= 2 */
	return count;
}
#endif /* DBG_SDIO */
#endif /* CONFIG_SDIO_HCI */

static int proc_get_mac_reg_dump(struct seq_file *m, void *v)
{
	struct net_device *dev = m->private;
	_adapter *adapter = (_adapter *)rtw_netdev_priv(dev);
	struct dvobj_priv *dvobj = adapter_to_dvobj(adapter);

	rtw_phl_mac_reg_dump(m, GET_PHL_INFO(dvobj));

	return 0;
}

static int proc_get_bb_reg_dump(struct seq_file *m, void *v)
{
	struct net_device *dev = m->private;
	_adapter *adapter = (_adapter *)rtw_netdev_priv(dev);
	struct dvobj_priv *dvobj = adapter_to_dvobj(adapter);

	rtw_phl_bb_reg_dump(m, GET_PHL_INFO(dvobj));

	return 0;
}

static int proc_get_bb_reg_dump_ex(struct seq_file *m, void *v)
{
	struct net_device *dev = m->private;
	_adapter *adapter = (_adapter *)rtw_netdev_priv(dev);
	struct dvobj_priv *dvobj = adapter_to_dvobj(adapter);

	rtw_phl_bb_reg_dump_ex(m, GET_PHL_INFO(dvobj));

	return 0;
}

static int proc_get_rf_reg_dump(struct seq_file *m, void *v)
{
	struct net_device *dev = m->private;
	_adapter *adapter = (_adapter *)rtw_netdev_priv(dev);
	struct dvobj_priv *dvobj = adapter_to_dvobj(adapter);

	rtw_phl_rf_reg_dump(m, GET_PHL_INFO(dvobj));

	return 0;
}

#if 0
int proc_get_led_config(struct seq_file *m, void *v)
{
	struct net_device *dev = m->private;
	_adapter *adapter = (_adapter *)rtw_netdev_priv(dev);

	dump_led_config(m, adapter);

	return 0;
}

ssize_t proc_set_led_config(struct file *file, const char __user *buffer, size_t count, loff_t *pos, void *data)
{
	struct net_device *dev = data;
	_adapter *adapter = (_adapter *)rtw_netdev_priv(dev);

	char tmp[32];
	u8 strategy;
	u8 iface_en_mask;

	if (count < 1)
		return -EFAULT;

	if (count > sizeof(tmp)) {
		rtw_warn_on(1);
		return -EFAULT;
	}

	if (buffer && !copy_from_user(tmp, buffer, count)) {

		int num = sscanf(tmp, "%hhu %hhx", &strategy, &iface_en_mask);

		if (num >= 1)
			rtw_led_set_strategy(adapter, strategy);
		if (num >= 2)
			rtw_led_set_iface_en_mask(adapter, iface_en_mask);
	}

	return count;
}
#endif /* CONFIG_RTW_LED */

#ifdef CONFIG_AP_MODE
int proc_get_aid_status(struct seq_file *m, void *v)
{
	struct net_device *dev = m->private;
	_adapter *adapter = (_adapter *)rtw_netdev_priv(dev);

	dump_aid_status(m, adapter);

	return 0;
}

ssize_t proc_set_aid_status(struct file *file, const char __user *buffer, size_t count, loff_t *pos, void *data)
{
	struct net_device *dev = data;
	_adapter *adapter = (_adapter *)rtw_netdev_priv(dev);
	struct sta_priv *stapriv = &adapter->stapriv;

	char tmp[32];
	u8 rr;
	u16 started_aid;

	if (count < 1)
		return -EFAULT;

	if (count > sizeof(tmp)) {
		rtw_warn_on(1);
		return -EFAULT;
	}

	if (buffer && !copy_from_user(tmp, buffer, count)) {

		int num = sscanf(tmp, "%hhu %hu", &rr, &started_aid);

		if (num >= 1)
			stapriv->rr_aid = rr ? 1 : 0;
		if (num >= 2) {
			started_aid = started_aid % (stapriv->max_aid + 1);
			stapriv->started_aid = started_aid ? started_aid : 1;
		}
	}

	return count;
}

#if CONFIG_RTW_AP_DATA_BMC_TO_UC
static int proc_get_ap_b2u_flags(struct seq_file *m, void *v)
{
	struct net_device *dev = m->private;
	_adapter *adapter = rtw_netdev_priv(dev);

	if (MLME_IS_AP(adapter))
		dump_ap_b2u_flags(m, adapter);

	return 0;
}

static ssize_t proc_set_ap_b2u_flags(struct file *file, const char __user *buffer, size_t count, loff_t *pos, void *data)
{
	struct net_device *dev = data;
	_adapter *adapter = rtw_netdev_priv(dev);
	char tmp[32];

	if (count < 1)
		return -EFAULT;

	if (count > sizeof(tmp)) {
		rtw_warn_on(1);
		return -EFAULT;
	}

	if (buffer && !copy_from_user(tmp, buffer, count)) {
		u8 src, fwd;
		int num = sscanf(tmp, "%hhx %hhx", &src, &fwd);

		if (num >= 1)
			adapter->b2u_flags_ap_src = src;
		if (num >= 2)
			adapter->b2u_flags_ap_fwd = fwd;
	}

	return count;
}
#endif /* CONFIG_RTW_AP_DATA_BMC_TO_UC */
#endif /* CONFIG_AP_MODE */

static int proc_get_dump_tx_rate_bmp(struct seq_file *m, void *v)
{
	struct net_device *dev = m->private;
	_adapter *adapter = (_adapter *)rtw_netdev_priv(dev);

	dump_tx_rate_bmp(m, adapter_to_dvobj(adapter));

	return 0;
}

static int proc_get_dump_adapters_status(struct seq_file *m, void *v)
{
	struct net_device *dev = m->private;
	_adapter *adapter = (_adapter *)rtw_netdev_priv(dev);

	dump_adapters_status(m, adapter_to_dvobj(adapter));

	return 0;
}

#ifdef CONFIG_RTW_CUSTOMER_STR
static int proc_get_customer_str(struct seq_file *m, void *v)
{
	struct net_device *dev = m->private;
	_adapter *adapter = (_adapter *)rtw_netdev_priv(dev);
	u8 cstr[RTW_CUSTOMER_STR_LEN];

	if (rtw_hal_customer_str_read(adapter, cstr) != _SUCCESS)
		goto exit;

	RTW_PRINT_SEL(m, RTW_CUSTOMER_STR_FMT"\n", RTW_CUSTOMER_STR_ARG(cstr));

exit:
	return 0;
}
#endif /* CONFIG_RTW_CUSTOMER_STR */

#ifdef CONFIG_SCAN_BACKOP
static int proc_get_backop_flags_sta(struct seq_file *m, void *v)
{
	struct net_device *dev = m->private;
	_adapter *adapter = (_adapter *)rtw_netdev_priv(dev);
	struct mlme_ext_priv *mlmeext = &adapter->mlmeextpriv;

	RTW_PRINT_SEL(m, "0x%02x\n", mlmeext_scan_backop_flags_sta(mlmeext));

	return 0;
}

static ssize_t proc_set_backop_flags_sta(struct file *file, const char __user *buffer, size_t count, loff_t *pos, void *data)
{
	struct net_device *dev = data;
	_adapter *adapter = (_adapter *)rtw_netdev_priv(dev);
	struct mlme_ext_priv *mlmeext = &adapter->mlmeextpriv;

	char tmp[32];
	u8 flags;

	if (count < 1)
		return -EFAULT;

	if (count > sizeof(tmp)) {
		rtw_warn_on(1);
		return -EFAULT;
	}

	if (buffer && !copy_from_user(tmp, buffer, count)) {

		int num = sscanf(tmp, "%hhx", &flags);

		if (num == 1)
			mlmeext_assign_scan_backop_flags_sta(mlmeext, flags);
	}

	return count;
}

#ifdef CONFIG_AP_MODE
static int proc_get_backop_flags_ap(struct seq_file *m, void *v)
{
	struct net_device *dev = m->private;
	_adapter *adapter = (_adapter *)rtw_netdev_priv(dev);
	struct mlme_ext_priv *mlmeext = &adapter->mlmeextpriv;

	RTW_PRINT_SEL(m, "0x%02x\n", mlmeext_scan_backop_flags_ap(mlmeext));

	return 0;
}

static ssize_t proc_set_backop_flags_ap(struct file *file, const char __user *buffer, size_t count, loff_t *pos, void *data)
{
	struct net_device *dev = data;
	_adapter *adapter = (_adapter *)rtw_netdev_priv(dev);
	struct mlme_ext_priv *mlmeext = &adapter->mlmeextpriv;

	char tmp[32];
	u8 flags;

	if (count < 1)
		return -EFAULT;

	if (count > sizeof(tmp)) {
		rtw_warn_on(1);
		return -EFAULT;
	}

	if (buffer && !copy_from_user(tmp, buffer, count)) {

		int num = sscanf(tmp, "%hhx", &flags);

		if (num == 1)
			mlmeext_assign_scan_backop_flags_ap(mlmeext, flags);
	}

	return count;
}
#endif /* CONFIG_AP_MODE */

#ifdef CONFIG_RTW_MESH
static int proc_get_backop_flags_mesh(struct seq_file *m, void *v)
{
	struct net_device *dev = m->private;
	_adapter *adapter = (_adapter *)rtw_netdev_priv(dev);
	struct mlme_ext_priv *mlmeext = &adapter->mlmeextpriv;

	RTW_PRINT_SEL(m, "0x%02x\n", mlmeext_scan_backop_flags_mesh(mlmeext));

	return 0;
}

static ssize_t proc_set_backop_flags_mesh(struct file *file, const char __user *buffer, size_t count, loff_t *pos, void *data)
{
	struct net_device *dev = data;
	_adapter *adapter = (_adapter *)rtw_netdev_priv(dev);
	struct mlme_ext_priv *mlmeext = &adapter->mlmeextpriv;

	char tmp[32];
	u8 flags;

	if (count < 1)
		return -EFAULT;

	if (count > sizeof(tmp)) {
		rtw_warn_on(1);
		return -EFAULT;
	}

	if (buffer && !copy_from_user(tmp, buffer, count)) {

		int num = sscanf(tmp, "%hhx", &flags);

		if (num == 1)
			mlmeext_assign_scan_backop_flags_mesh(mlmeext, flags);
	}

	return count;
}
#endif /* CONFIG_RTW_MESH */

#endif /* CONFIG_SCAN_BACKOP */

/* gpio setting */
#ifdef CONFIG_GPIO_API
static ssize_t proc_set_config_gpio(struct file *file, const char __user *buffer, size_t count, loff_t *pos, void *data)
{
	struct net_device *dev = data;
	_adapter *padapter = (_adapter *)rtw_netdev_priv(dev);
	char tmp[32] = {0};
	int num = 0, gpio_pin = 0, gpio_mode = 0; /* gpio_mode:0 input  1:output; */

	if (count < 2)
		return -EFAULT;

	if (count > sizeof(tmp)) {
		rtw_warn_on(1);
		return -EFAULT;
	}

	if (buffer && !copy_from_user(tmp, buffer, count)) {
		num	= sscanf(tmp, "%d %d", &gpio_pin, &gpio_mode);
		RTW_INFO("num=%d gpio_pin=%d mode=%d\n", num, gpio_pin, gpio_mode);
		padapter->pre_gpio_pin = gpio_pin;

		if (gpio_mode == 0 || gpio_mode == 1)
			rtw_hal_config_gpio(padapter, gpio_pin, gpio_mode);
	}
	return count;

}
static ssize_t proc_set_gpio_output_value(struct file *file, const char __user *buffer, size_t count, loff_t *pos, void *data)
{
	struct net_device *dev = data;
	_adapter *padapter = (_adapter *)rtw_netdev_priv(dev);
	char tmp[32] = {0};
	int num = 0, gpio_pin = 0, pin_mode = 0; /* pin_mode: 1 high         0:low */

	if (count < 2)
		return -EFAULT;

	if (count > sizeof(tmp)) {
		rtw_warn_on(1);
		return -EFAULT;
	}

	if (buffer && !copy_from_user(tmp, buffer, count)) {
		num	= sscanf(tmp, "%d %d", &gpio_pin, &pin_mode);
		RTW_INFO("num=%d gpio_pin=%d pin_high=%d\n", num, gpio_pin, pin_mode);
		padapter->pre_gpio_pin = gpio_pin;

		if (pin_mode == 0 || pin_mode == 1)
			rtw_hal_set_gpio_output_value(padapter, gpio_pin, pin_mode);
	}
	return count;
}
static int proc_get_gpio(struct seq_file *m, void *v)
{
	u8 gpioreturnvalue = 0;
	struct net_device *dev = m->private;

	_adapter *padapter = (_adapter *)rtw_netdev_priv(dev);
	if (!padapter)
		return -EFAULT;
	gpioreturnvalue = rtw_hal_get_gpio(padapter, padapter->pre_gpio_pin);
	RTW_PRINT_SEL(m, "get_gpio %d:%d\n", padapter->pre_gpio_pin, gpioreturnvalue);

	return 0;

}
static ssize_t proc_set_gpio(struct file *file, const char __user *buffer, size_t count, loff_t *pos, void *data)
{
	struct net_device *dev = data;
	_adapter *padapter = (_adapter *)rtw_netdev_priv(dev);
	char tmp[32] = {0};
	int num = 0, gpio_pin = 0;

	if (count < 1)
		return -EFAULT;

	if (count > sizeof(tmp)) {
		rtw_warn_on(1);
		return -EFAULT;
	}

	if (buffer && !copy_from_user(tmp, buffer, count)) {
		num	= sscanf(tmp, "%d", &gpio_pin);
		RTW_INFO("num=%d gpio_pin=%d\n", num, gpio_pin);
		padapter->pre_gpio_pin = gpio_pin;

	}
	return count;
}
#endif

static ssize_t proc_set_rx_info_msg(struct file *file, const char __user *buffer, size_t count, loff_t *pos, void *data)
{

	struct net_device *dev = data;
	_adapter *padapter = (_adapter *)rtw_netdev_priv(dev);
	struct recv_info *recvinfo = &padapter->recvinfo;
	char tmp[32] = {0};
	int phy_info_flag = 0;

	if (!padapter)
		return -EFAULT;

	if (count < 1) {
		RTW_INFO("argument size is less than 1\n");
		return -EFAULT;
	}

	if (count > sizeof(tmp)) {
		rtw_warn_on(1);
		return -EFAULT;
	}

	if (buffer && !copy_from_user(tmp, buffer, count)) {
		int num = sscanf(tmp, "%d", &phy_info_flag);

		if (num == 1)
			recvinfo->store_law_data_flag = (boolean) phy_info_flag;
	}
	return count;
}
static int proc_get_rx_info_msg(struct seq_file *m, void *v)
{
	struct net_device *dev = m->private;
	_adapter *padapter = (_adapter *)rtw_netdev_priv(dev);

	/*rtw_hal_set_phydm_var(padapter, HAL_PHYDM_RX_DATA_INFO, m, _FALSE);*/
	return 0;
}
static int proc_get_tx_info_msg(struct seq_file *m, void *v)
{
	struct net_device *dev = m->private;
	_adapter *adapter = (_adapter *)rtw_netdev_priv(dev);
	struct sta_info *psta;
	u8 bc_addr[ETH_ALEN] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
	u8 null_addr[ETH_ALEN] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
	_adapter *padapter = (_adapter *)rtw_netdev_priv(dev);
	struct sta_priv *pstapriv = &padapter->stapriv;
	int i;
	_list	*plist, *phead;
	u16 current_rate_id = 0, current_sgi = 0;

	char *BW, *status;

	_rtw_spinlock_bh(&pstapriv->sta_hash_lock);

	if (MLME_IS_STA(padapter))
		status = "station mode";
	else if (MLME_IS_AP(padapter))
		status = "AP mode";
	else if (MLME_IS_MESH(padapter))
		status = "mesh mode";
	else
		status = " ";
	_RTW_PRINT_SEL(m, "status=%s\n", status);
	for (i = 0; i < NUM_STA; i++) {
		phead = &(pstapriv->sta_hash[i]);
		plist = get_next(phead);

		while ((rtw_end_of_queue_search(phead, plist)) == _FALSE) {

			psta = LIST_CONTAINOR(plist, struct sta_info, hash_list);

			plist = get_next(plist);

			if ((_rtw_memcmp(psta->phl_sta->mac_addr, bc_addr, ETH_ALEN)  !=  _TRUE)
				&& (_rtw_memcmp(psta->phl_sta->mac_addr, null_addr, ETH_ALEN) != _TRUE)
				&& (_rtw_memcmp(psta->phl_sta->mac_addr, psta->padapter_link->mac_addr, ETH_ALEN) != _TRUE)) {

				switch (psta->phl_sta->chandef.bw) {

				case CHANNEL_WIDTH_20:
					BW = "20M";
					break;

				case CHANNEL_WIDTH_40:
					BW = "40M";
					break;

				case CHANNEL_WIDTH_80:
					BW = "80M";
					break;

				case CHANNEL_WIDTH_160:
					BW = "160M";
					break;

				default:
					BW = "";
					break;
				}
				current_rate_id = rtw_get_current_tx_rate(adapter, psta);
				current_sgi = rtw_get_current_tx_sgi(adapter, psta);

				RTW_PRINT_SEL(m, "==============================\n");
				_RTW_PRINT_SEL(m, "macaddr=" MAC_FMT"\n", MAC_ARG(psta->phl_sta->mac_addr));
				_RTW_PRINT_SEL(m, "Tx_Data_Rate=0x%x\n", current_rate_id);
				_RTW_PRINT_SEL(m, "BW=%s,sgi=%u\n", BW, current_sgi);

			}
		}
	}

	_rtw_spinunlock_bh(&pstapriv->sta_hash_lock);

	return 0;

}

static int proc_get_false_alarm_accumulated(struct seq_file *m, void *v)
{
	struct net_device *dev = m->private;
	_adapter *padapter = (_adapter *)rtw_netdev_priv(dev);
	struct dvobj_priv *dvobj = adapter_to_dvobj(padapter);
	/* ToDo CONFIG_RTW_MLD: [currently primary link only] */
	struct _ADAPTER_LINK *alink = GET_PRIMARY_LINK(padapter);
	struct rtw_wifi_role_link_t *wrlink = alink->wrlink;
	enum phl_band_idx hw_band = wrlink->hw_band;

	_RTW_PRINT_SEL(m , "Accumulated False Alarm:%u\n",
		       ATOMIC_READ((ATOMIC_T *)&dvobj->fa_cnt_acc[hw_band]));

	return 0;
}

static ssize_t proc_set_false_alarm_accumulated(struct file *file,
						const char __user *buffer,
						size_t count, loff_t *pos,
						void *data)
{
	struct net_device *dev = data;
	_adapter *padapter = (_adapter *)rtw_netdev_priv(dev);
	struct dvobj_priv *dvobj = adapter_to_dvobj(padapter);
	/* ToDo CONFIG_RTW_MLD: [currently primary link only] */
	struct _ADAPTER_LINK *alink = GET_PRIMARY_LINK(padapter);
	struct rtw_wifi_role_link_t *wrlink = alink->wrlink;
	enum phl_band_idx hw_band = wrlink->hw_band;
	char tmp[32];
	u32 false_clr;

	if (count < 1)
		return -EINVAL;

	if (count > sizeof(tmp)) {
		rtw_warn_on(1);
		return -EFAULT;
	}

	if (buffer && !copy_from_user(tmp, buffer, count)) {
		u32 num = sscanf(tmp, "%u ", &false_clr);
		ATOMIC_SET((ATOMIC_T *)&dvobj->fa_cnt_acc[hw_band],
			   (int)false_clr);
	} else {
		return -EFAULT;
	}

	return count;
}

#ifdef PRIVATE_R
static int proc_get_roku_trx_info_msg(struct seq_file *m, void *v)
{
	struct net_device *dev = m->private;
	_adapter *adapter = (_adapter *)rtw_netdev_priv(dev);
	struct _ADAPTER_LINK *padapter_link = GET_PRIMARY_LINK(adapter);
	struct dvobj_priv *devob = adapter_to_dvobj(adapter);
	_list	*plist, *phead;
	struct sta_info *psta = NULL;
	u8 sta_mac[NUM_STA][ETH_ALEN] = {{0}};
	uint mac_id[NUM_STA];
	struct stainfo_stats *pstats = NULL;
	struct sta_priv	*pstapriv = &(adapter->stapriv);
	u32 i, j, macid_rec_idx = 0;
	u8 bc_addr[ETH_ALEN] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
	u8 null_addr[ETH_ALEN] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
	struct mlme_priv *pmlmepriv = &(adapter->mlmepriv);
	struct xmit_priv *pxmitpriv = &(adapter->xmitpriv);
	struct recv_info *recvinfo = &(adapter->recvinfo);
	struct sta_recv_dframe_info *psta_dframe_info;
	char cnt_str[168] = {0};
	char tmp_str[21] = {0};
	u8 rx_nss_num = get_phy_rx_nss(adapter, padapter_link);
	char *BW;
	u8 isCCKrate = 0, rf_path = 0;
	u16 current_rate_id = 0;
	u32 tx_retry_cnt[PHL_AC_QUEUE_TOTAL] = {0};
	u32 tx_fail_cnt[PHL_AC_QUEUE_TOTAL] = {0};
	u32 tx_ok_cnt[PHL_AC_QUEUE_TOTAL] = {0};

	psta = rtw_get_stainfo(pstapriv, get_bssid(pmlmepriv));
	if (psta) {
		RTW_PRINT_SEL(m, "Interface Name: %s\n", ADPT_ARG(adapter));

		RTW_PRINT_SEL(m, "===== TX and RX Count Statistics =====\n");
		RTW_PRINT_SEL(m , "TX packets:%llu\n", pxmitpriv->tx_pkts);
		RTW_PRINT_SEL(m , "RX packets:%llu\n", recvinfo->rx_pkts);

		RTW_PRINT_SEL(m, "===== TX Drop Statistics =====\n");
		RTW_PRINT_SEL(m, "[Driver] BK packets drop count: %llu\n", pxmitpriv->tx_bk_drop_cnt);
		RTW_PRINT_SEL(m, "[Driver] BE packets drop count: %llu\n", pxmitpriv->tx_be_drop_cnt);
		RTW_PRINT_SEL(m, "[Driver] VI packets drop count: %llu\n", pxmitpriv->tx_vi_drop_cnt);
		RTW_PRINT_SEL(m, "[Driver] VO packets drop count: %llu\n", pxmitpriv->tx_vo_drop_cnt);

		RTW_PRINT_SEL(m, "===== RX VO packets Statistics =====\n");

		for(i = DESC_RATE1M ; i <= DESC_RATE11M ; i++) {
			snprintf(tmp_str, sizeof(tmp_str), "%llu ", recvinfo->rx_vo_pkt_count_per_data_rate[i]);
			strncat(cnt_str, tmp_str, strlen(tmp_str));
		}
		RTW_PRINT_SEL(m, "CCK (1M, 2M, 5.5M, 11M): [ %s]\n", cnt_str);
		_rtw_memset(&cnt_str, '\0', sizeof(cnt_str));

		for(i = DESC_RATE6M ; i <= DESC_RATE54M ; i++) {
			snprintf(tmp_str, sizeof(tmp_str), "%llu ", recvinfo->rx_vo_pkt_count_per_data_rate[i]);
			strncat(cnt_str, tmp_str, strlen(tmp_str));
		}
		RTW_PRINT_SEL(m, "OFDM (6M, 9M, 12M, 18M, 24M, 36M, 48M, 54M): [ %s]\n", cnt_str);
		_rtw_memset(&cnt_str, '\0', sizeof(cnt_str));

		for(i = 0 ; i < rx_nss_num ; i++) {
			for(j = DESC_RATEMCS0 ; j <= DESC_RATEMCS7 ; j++) {
				snprintf(tmp_str, sizeof(tmp_str), "%llu ", recvinfo->rx_vo_pkt_count_per_data_rate[j + (i<<3)]);
				strncat(cnt_str, tmp_str, strlen(tmp_str));
			}
			RTW_PRINT_SEL(m, "HT_MCS (%d-%d): [ %s]\n", (i<<3), (i<<3) + 7, cnt_str);
			_rtw_memset(&cnt_str, '\0', sizeof(cnt_str));
		}

		if (rx_nss_num == 1)
			RTW_PRINT_SEL(m, "HT_MCS (8-15): [ 0 0 0 0 0 0 0 0 ]\n");

		for(i = 0 ; i < rx_nss_num ; i++) {
			for(j = DESC_RATEVHTSS1MCS0 ; j <= DESC_RATEVHTSS1MCS9 ; j++) {
				snprintf(tmp_str, sizeof(tmp_str), "%llu ", recvinfo->rx_vo_pkt_count_per_data_rate[j + (i * 10)]);
				strncat(cnt_str, tmp_str, strlen(tmp_str));
			}
			RTW_PRINT_SEL(m, "VHT_%dSS_MCS (0-9): [ %s]\n", (i + 1), cnt_str);
			_rtw_memset(&cnt_str, '\0', sizeof(cnt_str));
		}

		if (rx_nss_num == 1)
			RTW_PRINT_SEL(m, "VHT_2SS_MCS (0-9): [ 0 0 0 0 0 0 0 0 0 0 ]\n");

		RTW_PRINT_SEL(m, "RX counter for VO packets with retry bit set: %llu\n\n", recvinfo->rx_vo_pkt_retry_count);
	}

	_rtw_spinlock_bh(&pstapriv->sta_hash_lock);
	for (i = 0; i < NUM_STA; i++) {
		phead = &(pstapriv->sta_hash[i]);
		plist = get_next(phead);
		while ((rtw_end_of_queue_search(phead, plist)) == _FALSE) {
			psta = LIST_CONTAINOR(plist, struct sta_info, hash_list);
			plist = get_next(plist);
			if ((_rtw_memcmp(psta->phl_sta->mac_addr, bc_addr, 6) !=  _TRUE)
				&& (_rtw_memcmp(psta->phl_sta->mac_addr, null_addr, 6) != _TRUE)
				&& (_rtw_memcmp(psta->phl_sta->mac_addr, psta->padapter_link->mac_addr, 6) != _TRUE)) {
				_rtw_memcpy(&sta_mac[macid_rec_idx][0], psta->phl_sta->mac_addr, ETH_ALEN);
				mac_id[macid_rec_idx] = psta->phl_sta->macid;
				macid_rec_idx++;
			}
		}
	}
	_rtw_spinunlock_bh(&pstapriv->sta_hash_lock);

	for (i = 0; i < macid_rec_idx; i++) {
		psta = rtw_get_stainfo(pstapriv, &sta_mac[i][0]);

		if (psta) {
			RTW_PRINT_SEL(m, "========= STA("MAC_FMT") =========\n", MAC_ARG(psta->phl_sta->mac_addr));
			RTW_PRINT_SEL(m, "oper_ch=%d, oper_bw=%d, oper_ch_offet=%d\n",
						rtw_get_oper_ch(adapter, padapter_link),
						rtw_get_oper_bw(adapter, padapter_link),
						rtw_get_oper_choffset(adapter, padapter_link));
#ifdef DBG_RX_DFRAME_RAW_DATA
			psta_dframe_info = &psta->sta_dframe_info_bmc;
			isCCKrate = (psta_dframe_info->sta_data_rate <= DESC_RATE11M) ? _TRUE : _FALSE;

			switch (psta_dframe_info->sta_bw_mode) {
				case CHANNEL_WIDTH_20:
					BW = "20M";
					break;
				case CHANNEL_WIDTH_40:
					BW = "40M";
					break;
				case CHANNEL_WIDTH_80:
					BW = "80M";
					break;
				case CHANNEL_WIDTH_160:
					BW = "160M";
					break;
				default:
					BW = "";
					break;
			}

			RTW_PRINT_SEL(m, "BW=%s, sgi =%d\n", BW, psta_dframe_info->sta_sgi);
			RTW_PRINT_SEL(m, "Rx_Data_Rate = %s\n", HDATA_RATE(psta_dframe_info->sta_data_rate));
			RTW_PRINT_SEL(m, "Rx_Data_Rate_Int = %d\n", (psta_dframe_info->sta_data_rate));

			for (rf_path = 0; rf_path < GET_HAL_RFPATH_NUM(devob); rf_path++) {
				if (!isCCKrate) {
					RTW_PRINT_SEL(m , "RF_PATH_%d RSSI:%d(dBm)", rf_path, psta_dframe_info->sta_RxPwr[rf_path]);
					RTW_PRINT_SEL(m , "\nrx_ofdm_snr:%d(dB)\n", psta_dframe_info->sta_ofdm_snr[rf_path]);
				} else
					RTW_PRINT_SEL(m , "RF_PATH_%d RSSI:%d(dBm)\n", rf_path,
						rtw_phl_rssi_to_dbm(psta_dframe_info->sta_mimo_signal_strength[rf_path]));
			}

			if (GET_HAL_RFPATH_NUM(devob) == 1) {
				if (!isCCKrate) {
					RTW_PRINT_SEL(m , "RF_PATH_1 RSSI:%d(dBm)", 0);
					RTW_PRINT_SEL(m , "\nrx_ofdm_snr:%d(dB)\n", 0);
				} else
					RTW_PRINT_SEL(m , "RF_PATH_1 RSSI:%d(dBm)\n", 0);
			}
#endif
			RTW_PRINT_SEL(m , "Total False Alarm:%d / Cnt_CCA_all:%d\n",
			rtw_phl_get_phy_stat_info(GET_PHL_INFO(devob), HW_BAND_0, STAT_INFO_FA_ALL),
			rtw_phl_get_phy_stat_info(GET_PHL_INFO(devob), HW_BAND_0, STAT_INFO_CCA_ALL));

			rtw_get_sta_tx_stat(adapter, psta);
			pstats = &psta->sta_stats;

			RTW_PRINT_SEL(m, "MAC: " MAC_FMT " sent: %u fail: %u retry: %u\n",
			MAC_ARG(&sta_mac[i][0]), pstats->tx_ok_cnt, pstats->tx_fail_cnt, pstats->tx_retry_cnt);
			RTW_PRINT_SEL(m, "total_tx_retry: %llu, rx_retry: %u\n", pstats->total_tx_retry_cnt,
						ATOMIC_READ((ATOMIC_T *)&pstats->rx_retry_cnt));
						ATOMIC_SET((ATOMIC_T *)&pstats->rx_retry_cnt, 0);

			RTW_PRINT_SEL(m, "[HW] BK packets drop count: %u\n", tx_fail_cnt[PHL_BE_QUEUE_SEL]);
			RTW_PRINT_SEL(m, "[HW] BE packets drop count: %u\n", tx_fail_cnt[PHL_BK_QUEUE_SEL]);
			RTW_PRINT_SEL(m, "[HW] VI packets drop count: %u\n", tx_fail_cnt[PHL_VI_QUEUE_SEL]);
			RTW_PRINT_SEL(m, "[HW] VO packets drop count: %u\n", tx_fail_cnt[PHL_VO_QUEUE_SEL]);
		} else {
			RTW_PRINT_SEL(m, "STA is gone\n");
		}
	}

	return 0;
}
#endif /* PRIVATE_R */


static int proc_get_linked_info_dump(struct seq_file *m, void *v)
{
	struct net_device *dev = m->private;
	_adapter *padapter = (_adapter *)rtw_netdev_priv(dev);
	if (padapter)
		RTW_PRINT_SEL(m, "rtw_hal_linked_info_dump :%s\n", (padapter->bLinkInfoDump) ? "enable" : "disable");

	return 0;
}


static ssize_t proc_set_linked_info_dump(struct file *file, const char __user *buffer, size_t count, loff_t *pos, void *data)
{
	struct net_device *dev = data;
	_adapter *padapter = (_adapter *)rtw_netdev_priv(dev);

	char tmp[32] = {0};
	int mode = 0, pre_mode = 0;
	int num = 0;

	if (count < 1)
		return -EFAULT;

	if (count > sizeof(tmp)) {
		rtw_warn_on(1);
		return -EFAULT;
	}

	pre_mode = padapter->bLinkInfoDump;
	RTW_INFO("pre_mode=%d\n", pre_mode);

	if (buffer && !copy_from_user(tmp, buffer, count)) {

		num	= sscanf(tmp, "%d ", &mode);
		RTW_INFO("num=%d mode=%d\n", num, mode);

		if (num != 1) {
			RTW_INFO("argument number is wrong\n");
			return -EFAULT;
		}

		if (mode == 1 || (mode == 0 && pre_mode == 1)) /* not consider pwr_saving 0: */
			padapter->bLinkInfoDump = mode;

		else if ((mode == 2) || (mode == 0 && pre_mode == 2)) { /* consider power_saving */
			/* RTW_INFO("rtw_hal_linked_info_dump =%s\n", (padapter->bLinkInfoDump)?"enable":"disable") */
			rtw_hal_linked_info_dump(padapter, mode);
		}
	}
	return count;
}


static int proc_get_sta_tp_dump(struct seq_file *m, void *v)
{
	struct net_device *dev = m->private;
	_adapter *padapter = (_adapter *)rtw_netdev_priv(dev);

	if (padapter)
		RTW_PRINT_SEL(m, "sta_tp_dump :%s\n", (padapter->bsta_tp_dump) ? "enable" : "disable");

	return 0;
}

static ssize_t proc_set_sta_tp_dump(struct file *file, const char __user *buffer, size_t count, loff_t *pos, void *data)
{
	struct net_device *dev = data;
	_adapter *padapter = (_adapter *)rtw_netdev_priv(dev);

	char tmp[32] = {0};
	int mode = 0;
	int num = 0;

	if (count < 1)
		return -EFAULT;

	if (count > sizeof(tmp)) {
		rtw_warn_on(1);
		return -EFAULT;
	}

	if (buffer && !copy_from_user(tmp, buffer, count)) {

		num	= sscanf(tmp, "%d ", &mode);

		if (num != 1) {
			RTW_INFO("argument number is wrong\n");
			return -EFAULT;
		}
		if (padapter)
			padapter->bsta_tp_dump = mode;
	}
	return count;
}

static int proc_get_sta_tp_info(struct seq_file *m, void *v)
{
	struct net_device *dev = m->private;
	_adapter *padapter = (_adapter *)rtw_netdev_priv(dev);

	if (padapter)
		rtw_sta_traffic_info(m, padapter);

	return 0;
}

static int proc_get_turboedca_ctrl(struct seq_file *m, void *v)
{
	struct net_device *dev = m->private;
	_adapter *padapter = (_adapter *)rtw_netdev_priv(dev);

	if (padapter->dis_turboedca == EN_TURBO)
		RTW_PRINT_SEL(m, "Turbo-EDCA : %s\n", "Enable");
	else if (padapter->dis_turboedca == DIS_TURBO)
		RTW_PRINT_SEL(m, "Turbo-EDCA : %s\n", "Disable");
	else
		RTW_PRINT_SEL(m, "Turbo-EDCA : %s, mode=%d, edca_param_mode=0x%x\n",
			"Disable", padapter->dis_turboedca, padapter->edca_param_mode);

	_RTW_PRINT_SEL(m, "PARAM_BE:0x%x\n", padapter->edca_param_mode);

	return 0;
}

static ssize_t proc_set_turboedca_ctrl(struct file *file, const char __user *buffer, size_t count, loff_t *pos, void *data)
{
	struct net_device *dev = data;
	_adapter *padapter = (_adapter *)rtw_netdev_priv(dev);
	char tmp[32] = {0};
	int mode = 0, num = 0;
	u32 param_mode = 0;

	if (count < 1)
		return -EFAULT;

	if (count > sizeof(tmp))
		return -EFAULT;

	if (buffer && !copy_from_user(tmp, buffer, count)) {

		num = sscanf(tmp, "%d %x", &mode, &param_mode);

		if (num < 1 || num > 2) {
			RTW_INFO("argument number is wrong\n");
			return -EFAULT;
		}

		/*  0: enable turboedca,
			1: disable turboedca,
			2: disable turboedca and setting EDCA parameter based on the input parameter
			> 2 : currently reset to 0 */

		if (mode > 2)
			mode = 0;

		padapter->dis_turboedca = mode;

		padapter->edca_param_mode = 0; /* init. value */

		RTW_INFO("dis_turboedca mode = 0x%x\n", padapter->dis_turboedca);

		if (num == 2) {

			padapter->edca_param_mode = param_mode;

			RTW_INFO("param_mode = 0x%x\n", param_mode);
		}

	}
	return count;
}

static int proc_get_mac_qinfo(struct seq_file *m, void *v)
{
	struct net_device *dev = m->private;
	_adapter *adapter = (_adapter *)rtw_netdev_priv(dev);

	rtw_hal_get_hwreg(adapter, HW_VAR_DUMP_MAC_QUEUE_INFO, (u8 *)m);

	return 0;
}

int proc_get_wifi_spec(struct seq_file *m, void *v)
{
	struct net_device *dev = m->private;
	_adapter *padapter = (_adapter *)rtw_netdev_priv(dev);
	struct registry_priv	*pregpriv = &padapter->registrypriv;

	RTW_PRINT_SEL(m, "wifi_spec=%d\n", pregpriv->wifi_spec);
	return 0;
}

static int proc_get_chan_plan(struct seq_file *m, void *v)
{
	struct net_device *dev = m->private;
	_adapter *adapter = (_adapter *)rtw_netdev_priv(dev);

	dump_cur_chplan(m, adapter_to_rfctl(adapter));

	return 0;
}

static ssize_t proc_set_chan_plan(struct file *file, const char __user *buffer, size_t count, loff_t *pos, void *data)
{
	struct net_device *dev = data;
	_adapter *padapter = (_adapter *)rtw_netdev_priv(dev);
	char tmp[32];
	u16 chplan = RTW_CHPLAN_UNSPECIFIED;
	u16 chplan_6g = RTW_CHPLAN_6G_UNSPECIFIED;

	if (!padapter)
		return -EFAULT;

	if (count < 1) {
		RTW_INFO("argument size is less than 1\n");
		return -EFAULT;
	}

	if (count > sizeof(tmp)) {
		rtw_warn_on(1);
		return -EFAULT;
	}

	if (buffer && !copy_from_user(tmp, buffer, count)) {
		int num = sscanf(tmp, "%hx %hx", &chplan, &chplan_6g);
		if (num < 1)
			return count;

		rtw_chplan_ioctl_input_mapping(&chplan, &chplan_6g);
	}

	rtw_set_channel_plan(padapter, chplan, chplan_6g, RTW_REGD_SET_BY_USER);

	return count;
}

static int proc_get_country_code(struct seq_file *m, void *v)
{
	struct net_device *dev = m->private;
	_adapter *adapter = (_adapter *)rtw_netdev_priv(dev);
	struct rf_ctl_t *rfctl = adapter_to_rfctl(adapter);

	dump_cur_country(m, rfctl);

	return 0;
}

static ssize_t proc_set_country_code(struct file *file, const char __user *buffer, size_t count, loff_t *pos, void *data)
{
	struct net_device *dev = data;
	_adapter *padapter = (_adapter *)rtw_netdev_priv(dev);
	char tmp[32];
	char alpha2[2];
	int num;

	if (count < 1)
		return -EFAULT;

	if (count > sizeof(tmp)) {
		rtw_warn_on(1);
		return -EFAULT;
	}

	if (!buffer || copy_from_user(tmp, buffer, count))
		goto exit;

	num = sscanf(tmp, "%c%c", &alpha2[0], &alpha2[1]);
	if (num !=	2)
		return count;

	rtw_set_country(padapter, alpha2, RTW_REGD_SET_BY_USER);

exit:
	return count;
}

static int proc_get_chplan_confs(struct seq_file *m, void *v)
{
	struct net_device *dev = m->private;
	_adapter *adapter = (_adapter *)rtw_netdev_priv(dev);
	struct rf_ctl_t *rfctl = adapter_to_rfctl(adapter);

	dump_cur_chplan_confs(m, rfctl);

	return 0;
}

static ssize_t proc_set_chplan_confs(struct file *file, const char __user *buffer, size_t count, loff_t *pos, void *data)
{
	struct net_device *dev = data;
	_adapter *adapter = rtw_netdev_priv(dev);
	char *tmp;
	struct chplan_confs confs;

	if (!buffer || count < 1)
		return -EFAULT;

	tmp = rtw_vmalloc(count + 1);
	if (!tmp)
		return -ENOMEM;

	if (copy_from_user(tmp, buffer, count))
		goto exit;
	tmp[count] = '\0';

	if (rtw_parse_chplan_confs_cmd_str(&confs, tmp))
		rtw_set_chplan_confs_cmd(adapter, RTW_CMDF_WAIT_ACK, &confs);

exit:
	rtw_vmfree(tmp, count + 1);

	return count;
}

static int cap_spt_op_class_ch_detail = 0;

static int proc_get_cap_spt_op_class_ch(struct seq_file *m, void *v)
{
	struct net_device *dev = m->private;
	_adapter *adapter = (_adapter *)rtw_netdev_priv(dev);

	dump_cap_spt_op_class_ch(m , adapter_to_rfctl(adapter), cap_spt_op_class_ch_detail);
	return 0;
}

static ssize_t proc_set_cap_spt_op_class_ch(struct file *file, const char __user *buffer, size_t count, loff_t *pos, void *data)
{
	struct net_device *dev = data;
	_adapter *padapter = (_adapter *)rtw_netdev_priv(dev);
	char tmp[32];

	if (count < 1)
		return -EFAULT;

	if (count > sizeof(tmp)) {
		rtw_warn_on(1);
		return -EFAULT;
	}

	if (!buffer || copy_from_user(tmp, buffer, count))
		goto exit;

	sscanf(tmp, "%d", &cap_spt_op_class_ch_detail);

exit:
	return count;
}

static int reg_spt_op_class_ch_detail = 0;

static int proc_get_reg_spt_op_class_ch(struct seq_file *m, void *v)
{
	struct net_device *dev = m->private;
	_adapter *adapter = (_adapter *)rtw_netdev_priv(dev);

	dump_reg_spt_op_class_ch(m , adapter_to_rfctl(adapter), reg_spt_op_class_ch_detail);
	return 0;
}

static ssize_t proc_set_reg_spt_op_class_ch(struct file *file, const char __user *buffer, size_t count, loff_t *pos, void *data)
{
	struct net_device *dev = data;
	_adapter *padapter = (_adapter *)rtw_netdev_priv(dev);
	char tmp[32];

	if (count < 1)
		return -EFAULT;

	if (count > sizeof(tmp)) {
		rtw_warn_on(1);
		return -EFAULT;
	}

	if (!buffer || copy_from_user(tmp, buffer, count))
		goto exit;

	sscanf(tmp, "%d", &reg_spt_op_class_ch_detail);

exit:
	return count;
}

static int cur_spt_op_class_ch_detail = 0;

static int proc_get_cur_spt_op_class_ch(struct seq_file *m, void *v)
{
	struct net_device *dev = m->private;
	_adapter *adapter = (_adapter *)rtw_netdev_priv(dev);

	dump_cur_spt_op_class_ch(m , adapter_to_rfctl(adapter), cur_spt_op_class_ch_detail);
	return 0;
}

static ssize_t proc_set_cur_spt_op_class_ch(struct file *file, const char __user *buffer, size_t count, loff_t *pos, void *data)
{
	struct net_device *dev = data;
	_adapter *padapter = (_adapter *)rtw_netdev_priv(dev);
	char tmp[32];

	if (count < 1)
		return -EFAULT;

	if (count > sizeof(tmp)) {
		rtw_warn_on(1);
		return -EFAULT;
	}

	if (!buffer || copy_from_user(tmp, buffer, count))
		goto exit;

	sscanf(tmp, "%d", &cur_spt_op_class_ch_detail);

exit:
	return count;
}

#if CONFIG_RTW_MACADDR_ACL
static int proc_get_macaddr_acl(struct seq_file *m, void *v)
{
	struct net_device *dev = m->private;
	_adapter *adapter = (_adapter *)rtw_netdev_priv(dev);

	dump_macaddr_acl(m, adapter);
	return 0;
}

ssize_t proc_set_macaddr_acl(struct file *file, const char __user *buffer, size_t count, loff_t *pos, void *data)
{
	struct net_device *dev = data;
	_adapter *adapter = (_adapter *)rtw_netdev_priv(dev);
	char tmp[17 * NUM_ACL + 32] = {0};
	u8 period;
	char cmd[32];
	u8 mode;
	u8 addr[ETH_ALEN];

#define MAC_ACL_CMD_MODE	0
#define MAC_ACL_CMD_ADD		1
#define MAC_ACL_CMD_DEL		2
#define MAC_ACL_CMD_CLR		3
#define MAC_ACL_CMD_NUM		4

	static const char * const mac_acl_cmd_str[] = {
		"mode",
		"add",
		"del",
		"clr",
	};
	u8 cmd_id = MAC_ACL_CMD_NUM;

	if (count < 1)
		return -EFAULT;

	if (count > sizeof(tmp)) {
		rtw_warn_on(1);
		return -EFAULT;
	}

	if (buffer && !copy_from_user(tmp, buffer, count)) {
		/*
		* <period> mode <mode> <macaddr> [<macaddr>]
		* <period> mode <mode>
		* <period> add <macaddr> [<macaddr>]
		* <period> del <macaddr> [<macaddr>]
		* <period> clr
		*/
		char *c, *next;
		int i;
		u8 is_bcast;

		next = tmp;
		c = strsep(&next, " \t");
		if (!c || sscanf(c, "%hhu", &period) != 1)
			goto exit;

		if (period >= RTW_ACL_PERIOD_NUM) {
			RTW_WARN(FUNC_ADPT_FMT" invalid period:%u", FUNC_ADPT_ARG(adapter), period);
			goto exit;
		}

		c = strsep(&next, " \t");
		if (!c || sscanf(c, "%s", cmd) != 1)
			goto exit;

		for (i = 0; i < MAC_ACL_CMD_NUM; i++)
			if (strcmp(mac_acl_cmd_str[i], cmd) == 0)
				cmd_id = i;

		switch (cmd_id) {
		case MAC_ACL_CMD_MODE:
			c = strsep(&next, " \t");
			if (!c || sscanf(c, "%hhu", &mode) != 1)
				goto exit;

			if (mode >= RTW_ACL_MODE_MAX) {
				RTW_WARN(FUNC_ADPT_FMT" invalid mode:%u", FUNC_ADPT_ARG(adapter), mode);
				goto exit;
			}
			break;

		case MAC_ACL_CMD_ADD:
		case MAC_ACL_CMD_DEL:
			break;

		case MAC_ACL_CMD_CLR:
			/* clear settings */
			rtw_macaddr_acl_clear(adapter, period);
			goto exit;

		default:
			RTW_WARN(FUNC_ADPT_FMT" invalid cmd:\"%s\"", FUNC_ADPT_ARG(adapter), cmd);
			goto exit;
		}

		/* check for macaddr list */
		c = strsep(&next, " \t");
		if (!c && cmd_id == MAC_ACL_CMD_MODE) {
			/* set mode only  */
			rtw_set_macaddr_acl(adapter, period, mode);
			goto exit;
		}

		if (cmd_id == MAC_ACL_CMD_MODE) {
			/* set mode and entire macaddr list */
			rtw_macaddr_acl_clear(adapter, period);
			rtw_set_macaddr_acl(adapter, period, mode);
		}

		while (c != NULL) {
			if (sscanf(c, MAC_SFMT, MAC_SARG(addr)) != 6)
				break;

			is_bcast = is_broadcast_mac_addr(addr);
			if (is_bcast
				|| rtw_check_invalid_mac_address(addr, 0) == _FALSE
			) {
				if (cmd_id == MAC_ACL_CMD_DEL) {
					rtw_acl_remove_sta(adapter, period, addr);
					if (is_bcast)
						break;
				 } else if (!is_bcast)
					rtw_acl_add_sta(adapter, period, addr);
			}

			c = strsep(&next, " \t");
		}
	}

exit:
	return count;
}
#endif /* CONFIG_RTW_MACADDR_ACL */

#if CONFIG_RTW_PRE_LINK_STA
static int proc_get_pre_link_sta(struct seq_file *m, void *v)
{
	struct net_device *dev = m->private;
	_adapter *adapter = (_adapter *)rtw_netdev_priv(dev);

	dump_pre_link_sta_ctl(m, &adapter->stapriv);
	return 0;
}

ssize_t proc_set_pre_link_sta(struct file *file, const char __user *buffer, size_t count, loff_t *pos, void *data)
{
	struct net_device *dev = data;
	_adapter *adapter = (_adapter *)rtw_netdev_priv(dev);
	struct mlme_priv *mlme = &adapter->mlmepriv;
	struct mlme_ext_priv *mlmeext = &adapter->mlmeextpriv;
	char tmp[17 * RTW_PRE_LINK_STA_NUM + 32] = {0};
	char arg0[16] = {0};
	u8 addr[ETH_ALEN];

#define PRE_LINK_STA_CMD_RESET	0
#define PRE_LINK_STA_CMD_ADD	1
#define PRE_LINK_STA_CMD_DEL	2
#define PRE_LINK_STA_CMD_NUM	3

	static const char * const pre_link_sta_cmd_str[] = {
		"reset",
		"add",
		"del"
	};
	u8 cmd_id = PRE_LINK_STA_CMD_NUM;

	if (count < 1)
		return -EFAULT;

	if (count > sizeof(tmp)) {
		rtw_warn_on(1);
		return -EFAULT;
	}

	if (buffer && !copy_from_user(tmp, buffer, count)) {
		/* cmd [<macaddr>] */
		char *c, *next;
		int i;

		next = tmp;
		c = strsep(&next, " \t");

		if (sscanf(c, "%s", arg0) != 1)
			goto exit;

		for (i = 0; i < PRE_LINK_STA_CMD_NUM; i++)
			if (strcmp(pre_link_sta_cmd_str[i], arg0) == 0)
				cmd_id = i;

		switch (cmd_id) {
		case PRE_LINK_STA_CMD_RESET:
			rtw_pre_link_sta_ctl_reset(&adapter->stapriv);
			goto exit;
		case PRE_LINK_STA_CMD_ADD:
		case PRE_LINK_STA_CMD_DEL:
			break;
		default:
			goto exit;
		}

		/* macaddr list */
		c = strsep(&next, " \t");
		while (c != NULL) {
			if (sscanf(c, MAC_SFMT, MAC_SARG(addr)) != 6)
				break;

			if (rtw_check_invalid_mac_address(addr, 0) == _FALSE) {
				if (cmd_id == PRE_LINK_STA_CMD_ADD)
					rtw_pre_link_sta_add(&adapter->stapriv, addr);
				else
					rtw_pre_link_sta_del(&adapter->stapriv, addr);
			}

			c = strsep(&next, " \t");
		}
	}

exit:
	return count;
}
#endif /* CONFIG_RTW_PRE_LINK_STA */

static int proc_get_ch_sel_policy(struct seq_file *m, void *v)
{
	struct net_device *dev = m->private;
	_adapter *adapter = (_adapter *)rtw_netdev_priv(dev);
	struct rf_ctl_t *rfctl = adapter_to_rfctl(adapter);

	RTW_PRINT_SEL(m, "%-16s\n", "within_same_band");

	RTW_PRINT_SEL(m, "%16d\n", rfctl->ch_sel_within_same_band);

	return 0;
}

static ssize_t proc_set_ch_sel_policy(struct file *file, const char __user *buffer, size_t count, loff_t *pos, void *data)
{
	struct net_device *dev = data;
	_adapter *adapter = (_adapter *)rtw_netdev_priv(dev);
	struct rf_ctl_t *rfctl = adapter_to_rfctl(adapter);
	char tmp[32];
	u8 within_sb;
	int num;

	if (count < 1)
		return -EFAULT;

	if (count > sizeof(tmp)) {
		rtw_warn_on(1);
		return -EFAULT;
	}

	if (!buffer || copy_from_user(tmp, buffer, count))
		goto exit;

	num = sscanf(tmp, "%hhu", &within_sb);
	if (num >=	1)
		rfctl->ch_sel_within_same_band = within_sb ? 1 : 0;

exit:
	return count;
}

#ifdef CONFIG_80211D
static int proc_get_country_ie_slave_records(struct seq_file *m, void *v)
{
	struct net_device *dev = m->private;
	_adapter *adapter = (_adapter *)rtw_netdev_priv(dev);
	struct rf_ctl_t *rfctl = adapter_to_rfctl(adapter);

	dump_country_ie_slave_records(m, rfctl, 0);

	return 0;
}
#endif /* CONFIG_80211D */

#ifdef CONFIG_DFS_MASTER
static int proc_get_dfs_test_case(struct seq_file *m, void *v)
{
	struct net_device *dev = m->private;
	_adapter *adapter = (_adapter *)rtw_netdev_priv(dev);
	struct rf_ctl_t *rfctl = adapter_to_rfctl(adapter);

	RTW_PRINT_SEL(m, "%-24s %-19s\n", "radar_detect_trigger_non", "choose_dfs_ch_first");
	RTW_PRINT_SEL(m, "%24hhu %19hhu\n"
		, rfctl->dbg_dfs_radar_detect_trigger_non
		, rfctl->dbg_dfs_choose_dfs_ch_first
	);

	return 0;
}

static ssize_t proc_set_dfs_test_case(struct file *file, const char __user *buffer, size_t count, loff_t *pos, void *data)
{
	struct net_device *dev = data;
	_adapter *adapter = (_adapter *)rtw_netdev_priv(dev);
	struct rf_ctl_t *rfctl = adapter_to_rfctl(adapter);
	char tmp[32];
	u8 radar_detect_trigger_non;
	u8 choose_dfs_ch_first;

	if (count < 1)
		return -EFAULT;

	if (count > sizeof(tmp)) {
		rtw_warn_on(1);
		return -EFAULT;
	}

	if (buffer && !copy_from_user(tmp, buffer, count)) {
		int num = sscanf(tmp, "%hhu %hhu", &radar_detect_trigger_non, &choose_dfs_ch_first);

		if (num >= 1)
			rfctl->dbg_dfs_radar_detect_trigger_non = radar_detect_trigger_non;
		if (num >= 2)
			rfctl->dbg_dfs_choose_dfs_ch_first = choose_dfs_ch_first;
	}

	return count;
}

ssize_t proc_set_update_non_ocp(struct file *file, const char __user *buffer, size_t count, loff_t *pos, void *data)
{
	struct net_device *dev = data;
	_adapter *adapter = (_adapter *)rtw_netdev_priv(dev);
	char tmp[32];
	u8 band, ch, bw = CHANNEL_WIDTH_20, offset = CHAN_OFFSET_NO_EXT;
	int ms = -1;

	if (count < 1)
		return -EFAULT;

	if (count > sizeof(tmp)) {
		rtw_warn_on(1);
		return -EFAULT;
	}

	if (buffer && !copy_from_user(tmp, buffer, count)) {

		int num = sscanf(tmp, "%hhu %hhu %hhu %hhu %d", &band, &ch, &bw, &offset, &ms);

		if (num < 1 || (bw != CHANNEL_WIDTH_20 && num < 4))
			goto exit;

		rtw_rfctl_force_update_non_ocp_ms(adapter_to_rfctl(adapter), band, ch, bw, offset, ms);
	}

exit:
	return count;
}

ssize_t proc_set_radar_detect(struct file *file, const char __user *buffer, size_t count, loff_t *pos, void *data)
{
	struct net_device *dev = data;
	_adapter *adapter = (_adapter *)rtw_netdev_priv(dev);
	struct rf_ctl_t *rfctl = adapter_to_rfctl(adapter);
	char tmp[32];
	u8 fake_radar_detect_cnt = 0;

	if (count < 1)
		return -EFAULT;

	if (count > sizeof(tmp)) {
		rtw_warn_on(1);
		return -EFAULT;
	}

	if (buffer && !copy_from_user(tmp, buffer, count)) {

		int num = sscanf(tmp, "%hhu", &fake_radar_detect_cnt);

		if (num < 1)
			goto exit;

		rfctl->dbg_dfs_fake_radar_detect_cnt = fake_radar_detect_cnt;
	}

exit:
	return count;
}

static int proc_get_dfs_ch_sel_e_flags(struct seq_file *m, void *v)
{
	struct net_device *dev = m->private;
	_adapter *adapter = (_adapter *)rtw_netdev_priv(dev);
	struct rf_ctl_t *rfctl = adapter_to_rfctl(adapter);

	RTW_PRINT_SEL(m, "0x%02x\n", rfctl->dfs_ch_sel_e_flags);

	return 0;
}

static ssize_t proc_set_dfs_ch_sel_e_flags(struct file *file, const char __user *buffer, size_t count, loff_t *pos, void *data)
{
	struct net_device *dev = data;
	_adapter *adapter = (_adapter *)rtw_netdev_priv(dev);
	struct rf_ctl_t *rfctl = adapter_to_rfctl(adapter);
	char tmp[32];
	u8 e_flags;
	int num;

	if (count < 1)
		return -EFAULT;

	if (count > sizeof(tmp)) {
		rtw_warn_on(1);
		return -EFAULT;
	}

	if (!buffer || copy_from_user(tmp, buffer, count))
		goto exit;

	num = sscanf(tmp, "%hhx", &e_flags);
	if (num != 1)
		goto exit;

	rfctl->dfs_ch_sel_e_flags = e_flags;

exit:
	return count;
}

static int proc_get_dfs_ch_sel_d_flags(struct seq_file *m, void *v)
{
	struct net_device *dev = m->private;
	_adapter *adapter = (_adapter *)rtw_netdev_priv(dev);
	struct rf_ctl_t *rfctl = adapter_to_rfctl(adapter);

	RTW_PRINT_SEL(m, "0x%02x\n", rfctl->dfs_ch_sel_d_flags);

	return 0;
}

static ssize_t proc_set_dfs_ch_sel_d_flags(struct file *file, const char __user *buffer, size_t count, loff_t *pos, void *data)
{
	struct net_device *dev = data;
	_adapter *adapter = (_adapter *)rtw_netdev_priv(dev);
	struct rf_ctl_t *rfctl = adapter_to_rfctl(adapter);
	char tmp[32];
	u8 d_flags;
	int num;

	if (count < 1)
		return -EFAULT;

	if (count > sizeof(tmp)) {
		rtw_warn_on(1);
		return -EFAULT;
	}

	if (!buffer || copy_from_user(tmp, buffer, count))
		goto exit;

	num = sscanf(tmp, "%hhx", &d_flags);
	if (num != 1)
		goto exit;

	rfctl->dfs_ch_sel_d_flags = d_flags;

exit:
	return count;
}

#if CONFIG_DFS_SLAVE_WITH_RADAR_DETECT
static int proc_get_dfs_slave_with_rd(struct seq_file *m, void *v)
{
	struct net_device *dev = m->private;
	_adapter *adapter = (_adapter *)rtw_netdev_priv(dev);
	struct rf_ctl_t *rfctl = adapter_to_rfctl(adapter);

	RTW_PRINT_SEL(m, "%u\n", rfctl->dfs_slave_with_rd);

	return 0;
}

static ssize_t proc_set_dfs_slave_with_rd(struct file *file, const char __user *buffer, size_t count, loff_t *pos, void *data)
{
	struct net_device *dev = data;
	_adapter *adapter = (_adapter *)rtw_netdev_priv(dev);
	struct rf_ctl_t *rfctl = adapter_to_rfctl(adapter);
	char tmp[32];
	u8 rd;
	int num;

	if (count < 1)
		return -EFAULT;

	if (count > sizeof(tmp)) {
		rtw_warn_on(1);
		return -EFAULT;
	}

	if (!buffer || copy_from_user(tmp, buffer, count))
		goto exit;

	num = sscanf(tmp, "%hhu", &rd);
	if (num != 1)
		goto exit;

	rd = rd ? 1 : 0;

	if (rfctl->dfs_slave_with_rd != rd) {
		int i;

		rfctl->dfs_slave_with_rd = rd;
		for (i = HW_BAND_0; i < HW_BAND_MAX; i++)
			rtw_dfs_rd_en_decision_cmd(adapter_to_dvobj(adapter), i);
	}

exit:
	return count;
}
#endif /* CONFIG_DFS_SLAVE_WITH_RADAR_DETECT */
#endif /* CONFIG_DFS_MASTER */

#ifdef CONFIG_80211N_HT
int proc_get_rx_ampdu_size_limit(struct seq_file *m, void *v)
{
	struct net_device *dev = m->private;
	_adapter *adapter = (_adapter *)rtw_netdev_priv(dev);

	dump_regsty_rx_ampdu_size_limit(m, adapter);

	return 0;
}

ssize_t proc_set_rx_ampdu_size_limit(struct file *file, const char __user *buffer, size_t count, loff_t *pos, void *data)
{
	struct net_device *dev = data;
	_adapter *adapter = (_adapter *)rtw_netdev_priv(dev);
	struct registry_priv *regsty = adapter_to_regsty(adapter);
	char tmp[32];
	u8 nss;
	u8 limit_by_bw[4] = {0xFF};

	if (count < 1)
		return -EFAULT;

	if (count > sizeof(tmp)) {
		rtw_warn_on(1);
		return -EFAULT;
	}

	if (buffer && !copy_from_user(tmp, buffer, count)) {
		int i;
		int num = sscanf(tmp, "%hhu %hhu %hhu %hhu %hhu"
			, &nss, &limit_by_bw[0], &limit_by_bw[1], &limit_by_bw[2], &limit_by_bw[3]);

		if (num < 2)
			goto exit;
		if (nss == 0 || nss > 4)
			goto exit;

		for (i = 0; i < num - 1; i++)
			regsty->rx_ampdu_sz_limit_by_nss_bw[nss - 1][i] = limit_by_bw[i];
		rtw_rx_ampdu_apply(adapter);
	}

exit:
	return count;
}
#endif /* CONFIG_80211N_HT */

static int proc_get_rx_chk_limit(struct seq_file *m, void *v)
{
	struct net_device *dev = m->private;
	_adapter *padapter = (_adapter *)rtw_netdev_priv(dev);

	RTW_PRINT_SEL(m, "Rx chk limit : %d\n", rtw_get_rx_chk_limit(padapter));

	return 0;
}

static ssize_t proc_set_rx_chk_limit(struct file *file, const char __user *buffer, size_t count, loff_t *pos, void *data)
{
	char tmp[32];
	struct net_device *dev = data;
	_adapter *padapter = (_adapter *)rtw_netdev_priv(dev);
	int rx_chk_limit;

	if (count < 1) {
		RTW_INFO("argument size is less than 1\n");
		return -EFAULT;
	}

	if (count > sizeof(tmp)) {
		rtw_warn_on(1);
		return -EFAULT;
	}

	if (buffer && !copy_from_user(tmp, buffer, count)) {
		int num = sscanf(tmp, "%d", &rx_chk_limit);

		rtw_set_rx_chk_limit(padapter, rx_chk_limit);
	}

	return count;
}

static int proc_get_udpport(struct seq_file *m, void *v)
{
	struct net_device *dev = m->private;
	_adapter *padapter = (_adapter *)rtw_netdev_priv(dev);
	struct recv_info *precvinfo = &(padapter->recvinfo);

	RTW_PRINT_SEL(m, "%d\n", precvinfo->sink_udpport);
	return 0;
}
static ssize_t proc_set_udpport(struct file *file, const char __user *buffer, size_t count, loff_t *pos, void *data)
{
	struct net_device *dev = data;
	_adapter *padapter = (_adapter *)rtw_netdev_priv(dev);
	struct recv_info *precvinfo = &(padapter->recvinfo);
	int sink_udpport = 0;
	char tmp[32];


	if (!padapter)
		return -EFAULT;

	if (count < 1) {
		RTW_INFO("argument size is less than 1\n");
		return -EFAULT;
	}

	if (count > sizeof(tmp)) {
		rtw_warn_on(1);
		return -EFAULT;
	}

	if (buffer && !copy_from_user(tmp, buffer, count)) {

		int num = sscanf(tmp, "%d", &sink_udpport);

		if (num !=  1) {
			RTW_INFO("invalid input parameter number!\n");
			return count;
		}

	}
	precvinfo->sink_udpport = sink_udpport;

	return count;

}

#if 0
static int proc_get_mi_ap_bc_info(struct seq_file *m, void *v)
{
	struct net_device *dev = m->private;
	_adapter *adapter = (_adapter *)rtw_netdev_priv(dev);
	struct dvobj_priv *dvobj = adapter_to_dvobj(adapter);
	struct macid_ctl_t *macid_ctl = dvobj_to_macidctl(dvobj);
	u8 i;

	for (i = 0; i < dvobj->iface_nums; i++)
		RTW_PRINT_SEL(m, "iface_id:%d, mac_id && sec_cam_id = %d\n", i, macid_ctl->iface_bmc[i]);

	return 0;
}
#endif
#if 0
static int proc_get_macid_info(struct seq_file *m, void *v)
{
	struct net_device *dev = m->private;
	_adapter *adapter = (_adapter *)rtw_netdev_priv(dev);
	struct _ADAPTER_LINK *adapter_link = GET_PRIMARY_LINK(adapter);
	struct dvobj_priv *dvobj = adapter_to_dvobj(adapter);
	struct macid_ctl_t *macid_ctl = dvobj_to_macidctl(dvobj);
	u8 i;
	u8 null_addr[ETH_ALEN] = {0};
	u8 *macaddr;

	RTW_PRINT_SEL(m, "max_num:%u\n", macid_ctl->num);
	RTW_PRINT_SEL(m, "\n");

	RTW_PRINT_SEL(m, "used:\n");
	dump_macid_map(m, &macid_ctl->used, macid_ctl->num);
	RTW_PRINT_SEL(m, "\n");

	RTW_PRINT_SEL(m, "%-3s %-3s %-5s %-4s %-17s %-6s %-3s"
		, "id", "bmc", "ifbmp", "ch_g", "macaddr", "bw", "vht");

	if (get_phy_rx_nss(adapter, adapter_link) > 2)
		_RTW_PRINT_SEL(m, " %-10s", "rate_bmp1");

	_RTW_PRINT_SEL(m, " %-10s %s\n", "rate_bmp0", "status");

	for (i = 0; i < macid_ctl->num; i++) {
		if (rtw_macid_is_used(macid_ctl, i)
			|| macid_ctl->h2c_msr[i]
		) {
			if (macid_ctl->sta[i])
				macaddr = macid_ctl->sta[i]->phl_sta->mac_addr;
			else
				macaddr = null_addr;

			RTW_PRINT_SEL(m, "%3u %3u  0x%02x %4d "MAC_FMT" %6s %3u"
				, i
				, rtw_macid_is_bmc(macid_ctl, i)
				, rtw_macid_get_iface_bmp(macid_ctl, i)
				, rtw_macid_get_ch_g(macid_ctl, i)
				, MAC_ARG(macaddr)
				, ch_width_str(macid_ctl->bw[i])
				, macid_ctl->vht_en[i]
			);

			if (get_phy_rx_nss(adapter, adapter_link) > 2)
				_RTW_PRINT_SEL(m, " 0x%08X", macid_ctl->rate_bmp1[i]);
#ifdef DBG_MACID_MSR_INFO
			_RTW_PRINT_SEL(m, " 0x%08X "H2C_MSR_FMT" %s\n"
				, macid_ctl->rate_bmp0[i]
				, H2C_MSR_ARG(&macid_ctl->h2c_msr[i])
				, rtw_macid_is_used(macid_ctl, i) ? "" : "[unused]"
			);
#endif
		}
	}

	return 0;
}
#endif
static int proc_get_sec_cam(struct seq_file *m, void *v)
{
	struct net_device *dev = m->private;
	_adapter *adapter = (_adapter *)rtw_netdev_priv(dev);
	struct dvobj_priv *dvobj = adapter_to_dvobj(adapter);
	struct cam_ctl_t *cam_ctl = &dvobj->cam_ctl;

	RTW_PRINT_SEL(m, "sec_cap:0x%02x\n", cam_ctl->sec_cap);
	RTW_PRINT_SEL(m, "flags:0x%08x\n", cam_ctl->flags);
	RTW_PRINT_SEL(m, "\n");

	RTW_PRINT_SEL(m, "max_num:%u\n", cam_ctl->num);
	RTW_PRINT_SEL(m, "used:\n");
	dump_sec_cam_map(m, &cam_ctl->used, cam_ctl->num);
	RTW_PRINT_SEL(m, "\n");

#if 0 /*GEORGIA_TODO_REDEFINE_IO*/
	RTW_PRINT_SEL(m, "reg_scr:0x%04x\n", rtw_read16(adapter, 0x680));
#endif
	RTW_PRINT_SEL(m, "\n");

	dump_sec_cam(m, adapter);

	return 0;
}

static ssize_t proc_set_sec_cam(struct file *file, const char __user *buffer, size_t count, loff_t *pos, void *data)
{
	struct net_device *dev = data;
	_adapter *adapter = (_adapter *)rtw_netdev_priv(dev);
	struct dvobj_priv *dvobj = adapter_to_dvobj(adapter);
	struct cam_ctl_t *cam_ctl = &dvobj->cam_ctl;
	char tmp[32] = {0};
	char cmd[4];
	u8 id_1 = 0, id_2 = 0;

	if (count > sizeof(tmp)) {
		rtw_warn_on(1);
		return -EFAULT;
	}

	if (buffer && !copy_from_user(tmp, buffer, count)) {

		/* c <id_1>: clear specific cam entry */
		/* wfc <id_1>: write specific cam entry from cam cache */
		/* sw <id_1> <id_2>: sec_cam 1/2 swap */

		int num = sscanf(tmp, "%s %hhu %hhu", cmd, &id_1, &id_2);

		if (num < 2)
			return count;

		if ((id_1 >= cam_ctl->num) || (id_2 >= cam_ctl->num)) {
			RTW_ERR(FUNC_ADPT_FMT" invalid id_1:%u id_2:%u\n", FUNC_ADPT_ARG(adapter), id_1, id_2);
			return count;
		}

		if (strcmp("c", cmd) == 0) {
			_clear_cam_entry(adapter, id_1);
			adapter->securitypriv.hw_decrypted = _FALSE; /* temporarily set this for TX path to use SW enc */
		} else if (strcmp("wfc", cmd) == 0)
			write_cam_from_cache(adapter, id_1);
		else if (strcmp("sw", cmd) == 0)
			rtw_sec_cam_swap(adapter, id_1, id_2);
		else if (strcmp("cdk", cmd) == 0)
			rtw_clean_dk_section(adapter);
#ifdef DBG_SEC_CAM_MOVE
		else if (strcmp("sgd", cmd) == 0)
			rtw_hal_move_sta_gk_to_dk(adapter);
		else if (strcmp("rsd", cmd) == 0)
			rtw_hal_read_sta_dk_key(adapter, id_1);
#endif
	}

	return count;
}

static int proc_get_sec_cam_cache(struct seq_file *m, void *v)
{
	struct net_device *dev = m->private;
	_adapter *adapter = (_adapter *)rtw_netdev_priv(dev);

	dump_sec_cam_cache(m, adapter);
	return 0;
}

#ifdef CONFIG_DBG_AX_CAM

static int proc_get_ax_address_cam(struct seq_file *m, void *v) {
	struct net_device *dev = m->private;
	struct _ADAPTER *adapter = (struct _ADAPTER *)rtw_netdev_priv(dev);

	get_ax_address_cam(m, adapter);
	return 0;
}

static int proc_get_ax_sec_cam(struct seq_file *m, void *v) {
	struct net_device *dev = m->private;
	struct _ADAPTER *adapter = (struct _ADAPTER *)rtw_netdev_priv(dev);

	get_ax_sec_cam(m, adapter);
	return 0;
}

static int proc_get_ax_valid_key(struct seq_file *m, void *v) {
	struct net_device *dev = m->private;
	struct _ADAPTER *adapter = (struct _ADAPTER *)rtw_netdev_priv(dev);

	get_ax_valid_key(m, adapter);
	return 0;
}

#endif

static ssize_t proc_set_change_bss_chbw(struct file *file, const char __user *buffer, size_t count, loff_t *pos, void *data)
{
	struct net_device *dev = data;
	_adapter *adapter = (_adapter *)rtw_netdev_priv(dev);
	struct dvobj_priv *dvobj = adapter_to_dvobj(adapter);
	int i;
	char tmp[32];
	s16 ch;
	s8 bw = REQ_BW_NONE, offset = REQ_OFFSET_NONE, band = REQ_BAND_NONE;
	u8 ifbmp = 0;

	if (count < 1)
		return -EFAULT;

	if (count > sizeof(tmp)) {
		rtw_warn_on(1);
		return -EFAULT;
	}

	if (buffer && !copy_from_user(tmp, buffer, count)) {

		int num = sscanf(tmp, "%hd %hhd %hhd %hhx", &ch, &bw, &offset, &ifbmp);

		if (num < 1 || (bw != CHANNEL_WIDTH_20 && num < 3))
			goto exit;

		if (num < 4)
			ifbmp = BIT(adapter->iface_id);
		else
			ifbmp &= (1 << dvobj->iface_nums) - 1;

		for (i = 0; i < dvobj->iface_nums; i++) {
			if (!(ifbmp & BIT(i)) || !dvobj->padapters[i])
				continue;

			if (!CHK_MLME_STATE(dvobj->padapters[i], WIFI_AP_STATE | WIFI_MESH_STATE)
				|| !MLME_IS_ASOC(dvobj->padapters[i]))
				ifbmp &= ~BIT(i);
		}

		if (ifbmp)
			rtw_change_bss_bchbw_cmd(adapter, RTW_CMDF_WAIT_ACK, ifbmp, 0, band, ch, bw, offset);
	}

exit:
	return count;
}

static int proc_get_tx_bw_mode(struct seq_file *m, void *v)
{
	struct net_device *dev = m->private;
	_adapter *adapter = (_adapter *)rtw_netdev_priv(dev);

	RTW_PRINT_SEL(m, "0x%02x\n", adapter->driver_tx_bw_mode);
	RTW_PRINT_SEL(m, "2.4G:%s\n", ch_width_str(ADAPTER_TX_BW_2G(adapter)));
	RTW_PRINT_SEL(m, "5G:%s\n", ch_width_str(ADAPTER_TX_BW_5G(adapter)));

	return 0;
}

static void rtw_set_tx_bw_mode(_adapter *adapter, u8 bw_mode)
{
	/* ToDo CONFIG_RTW_MLD: [currently primary link only] */
	struct _ADAPTER_LINK *adapter_link = GET_PRIMARY_LINK(adapter);
	struct link_mlme_ext_priv *mlmeext = &(adapter_link->mlmeextpriv);
	/* macid_ctl move to phl */
	/* struct macid_ctl_t *macid_ctl = &adapter->dvobj->macid_ctl; */
	u8 update = _FALSE;

	if ((MLME_STATE(adapter) & WIFI_ASOC_STATE)
		&& ((mlmeext->chandef.chan <= 14 && BW_MODE_2G(bw_mode) != ADAPTER_TX_BW_2G(adapter))
			|| (mlmeext->chandef.chan >= 36 && BW_MODE_5G(bw_mode) != ADAPTER_TX_BW_5G(adapter)))
	) {
		/* RA mask update needed */
		update = _TRUE;
	}
	adapter->driver_tx_bw_mode = bw_mode;

#if 0
	if (update == _TRUE) {
		struct sta_info *sta;
		int i;

		/*for (i = 0; i < MACID_NUM_SW_LIMIT; i++) {*/
			//sta = macid_ctl->sta[i];

			sta = rtw_get_stainfo(&adapter->stapriv, get_bssid(&adapter->mlmepriv));
			if (sta && !is_broadcast_mac_addr(sta->phl_sta->mac_addr)) {
				rtw_phl_cmd_change_stainfo(adapter_to_dvobj(adapter)->phl,
					   sta->phl_sta,
					   STA_CHG_RAMASK,
					   NULL,
					   0,
					   PHL_CMD_NO_WAIT, /*PHL_CMD_DIRECTLY,PHL_CMD_WAIT*/
					   0);
			}
		}
	/*}*/
#endif
}

static ssize_t proc_set_tx_bw_mode(struct file *file, const char __user *buffer, size_t count, loff_t *pos, void *data)
{
	struct net_device *dev = data;
	_adapter *adapter = (_adapter *)rtw_netdev_priv(dev);
	char tmp[32];
	u8 bw_mode;

	if (count < 1)
		return -EFAULT;

	if (count > sizeof(tmp)) {
		rtw_warn_on(1);
		return -EFAULT;
	}

	if (buffer && !copy_from_user(tmp, buffer, count)) {

		int num = sscanf(tmp, "%hhx", &bw_mode);

		if (num < 1 || bw_mode == adapter->driver_tx_bw_mode)
			goto exit;

		rtw_set_tx_bw_mode(adapter, bw_mode);
	}

exit:
	return count;
}

#ifdef CONFIG_ECSA_PHL
static int proc_get_ecsa_allow_reason(struct seq_file *m, void *v)
{
	struct net_device *dev = m->private;
	_adapter *a = (_adapter *)rtw_netdev_priv(dev);
	struct core_ecsa_info *ecsa_info = &(a->ecsa_info);

	RTW_PRINT_SEL(m, "ECSA allow reason : 0x%02x\n", ecsa_info->ecsa_allow_case);
	RTW_PRINT_SEL(m, "bit 0 : ECSA_START_MCC_24G_TO_24G\n");
	RTW_PRINT_SEL(m, "bit 1 : ECSA_START_MCC_5G_TO_5G\n");
	RTW_PRINT_SEL(m, "bit 2 : ECSA_START_MCC_24G_TO_5G\n");
	RTW_PRINT_SEL(m, "bit 3 : ECSA_START_MCC_5G_TO_24G\n");
	RTW_PRINT_SEL(m, "bit 4 : ECSA_START_CORE_TRIGGER\n");

	return 0;
}

static ssize_t proc_set_ecsa_allow_reason(struct file *file,
		const char __user *buffer, size_t count, loff_t *pos, void *data)
{
	struct net_device *dev = data;
	_adapter *a = (_adapter *)rtw_netdev_priv(dev);
	struct core_ecsa_info *ecsa_info = &(a->ecsa_info);
	char tmp[12];
	u8 reason;

	if (count < 1)
		return -EFAULT;

	if (count > sizeof(tmp)) {
		rtw_warn_on(1);
		return -EFAULT;
	}

	if (buffer && !copy_from_user(tmp, buffer, count)) {
		int num = sscanf(tmp, "%hhx", &reason);

		if (num < 1 || reason >= ECSA_START_UNKNOWN)
			goto exit;

		ecsa_info->ecsa_allow_case = reason;
	}
exit:
	return count;
}
#endif

static int proc_get_hal_txpwr_info(struct seq_file *m, void *v)
{
#ifdef CONFIG_TXPWR_PG_WITH_PWR_IDX
	struct net_device *dev = m->private;
	_adapter *adapter = (_adapter *)rtw_netdev_priv(dev);
	struct dvobj_priv *devob = adapter_to_dvobj(adapter);
	HAL_DATA_TYPE *hal_data = GET_PHL_COM(devob);
	struct hal_spec_t *hal_spec = GET_HAL_SPEC(devob);

	if (hal_data->txpwr_pg_mode == TXPWR_PG_WITH_PWR_IDX) {
		if (rtw_hw_is_band_support(devob, BAND_ON_24G))
			dump_hal_txpwr_info_2g(m, adapter, hal_spec->rfpath_num_2g, hal_data->max_tx_cnt);

		#if CONFIG_IEEE80211_BAND_5GHZ
		if (rtw_hw_is_band_support(devob, BAND_ON_5G))
			dump_hal_txpwr_info_5g(m, adapter, hal_spec->rfpath_num_5g, hal_data->max_tx_cnt);
		#endif
	}
#endif

	return 0;
}

static int proc_get_target_tx_power(struct seq_file *m, void *v)
{
	struct net_device *dev = m->private;
	_adapter *adapter = (_adapter *)rtw_netdev_priv(dev);

	rtw_hal_dump_target_tx_power(m, adapter);

	return 0;
}

static int proc_get_tx_power_by_rate(struct seq_file *m, void *v)
{
	struct net_device *dev = m->private;
	_adapter *adapter = (_adapter *)rtw_netdev_priv(dev);

	rtw_hal_dump_tx_power_by_rate(m, adapter);

	return 0;
}

#if CONFIG_TXPWR_LIMIT
static int proc_get_tx_power_limit(struct seq_file *m, void *v)
{
	struct net_device *dev = m->private;
	_adapter *adapter = (_adapter *)rtw_netdev_priv(dev);

	dump_txpwr_lmt(m, adapter);

	return 0;
}
#endif /* CONFIG_TXPWR_LIMIT */

static int proc_get_tpc_settings(struct seq_file *m, void *v)
{
	struct net_device *dev = m->private;
	_adapter *adapter = (_adapter *)rtw_netdev_priv(dev);

	dump_txpwr_tpc_settings(m, adapter_to_dvobj(adapter));

	return 0;
}

static ssize_t proc_set_tpc_settings(struct file *file, const char __user *buffer, size_t count, loff_t *pos, void *data)
{
	struct net_device *dev = data;
	_adapter *adapter = (_adapter *)rtw_netdev_priv(dev);
	struct dvobj_priv *dvobj = adapter_to_dvobj(adapter);
	struct rf_ctl_t *rfctl = dvobj_to_rfctl(dvobj);

	char tmp[32] = {0};
	u8 mode;
	u16 m_constraint;

	if (count > sizeof(tmp)) {
		rtw_warn_on(1);
		return -EFAULT;
	}

	if (buffer && !copy_from_user(tmp, buffer, count)) {

		int num = sscanf(tmp, "%hhu %hu", &mode, &m_constraint);

		if (num < 1)
			return count;

		if (mode >= TPC_MODE_INVALID)
			return count;

		if (mode == TPC_MODE_MANUAL && num >= 2)
			rfctl->tpc_manual_constraint = rtw_min(m_constraint, TPC_MANUAL_CONSTRAINT_MAX);
		rfctl->tpc_mode = mode;

		if (rtw_hw_is_init_completed(dvobj))
			rtw_run_in_thread_cmd_wait(adapter, (void *)rtw_update_txpwr_level_all_hwband, dvobj, 2000);
	}

	return count;
}

static int proc_get_tx_power_ext_info(struct seq_file *m, void *v)
{
	struct net_device *dev = m->private;
	_adapter *adapter = (_adapter *)rtw_netdev_priv(dev);

	dump_tx_power_ext_info(m, adapter_to_dvobj(adapter));

	return 0;
}

static ssize_t proc_set_tx_power_ext_info(struct file *file, const char __user *buffer, size_t count, loff_t *pos, void *data)
{
	struct net_device *dev = data;
	_adapter *adapter = (_adapter *)rtw_netdev_priv(dev);

	char tmp[32] = {0};
	char cmd[16] = {0};

	if (count > sizeof(tmp)) {
		rtw_warn_on(1);
		return -EFAULT;
	}

	if (buffer && !copy_from_user(tmp, buffer, count)) {

		int num = sscanf(tmp, "%s", cmd);

		if (num < 1)
			return count;

#if 0 /* TODO */
		#ifdef CONFIG_LOAD_PHY_PARA_FROM_FILE
		phy_free_filebuf_mask(adapter, LOAD_BB_PG_PARA_FILE | LOAD_RF_TXPWR_LMT_PARA_FILE);
		#endif

		if (strcmp("default", cmd) == 0)
			rtw_run_in_thread_cmd(adapter, ((void *)(phy_reload_default_tx_power_ext_info)), adapter);
		else
			rtw_run_in_thread_cmd(adapter, ((void *)(phy_reload_tx_power_ext_info)), adapter);

		rtw_run_in_thread_cmd_wait(adapter, ((void *)(rtw_hal_update_txpwr_level)), adapter, 2000);
#endif
	}

	return count;
}

#ifdef GEORGIA_TODO_TX_PWR
static void *proc_start_tx_power_idx(struct seq_file *m, loff_t *pos)
{
	u8 path = ((*pos) & 0xFF00) >> 8;

	if (path >= RF_PATH_MAX)
		return NULL;

	return pos;
}
static void proc_stop_tx_power_idx(struct seq_file *m, void *v)
{
}

static void *proc_next_tx_power_idx(struct seq_file *m, void *v, loff_t *pos)
{
	u8 path = ((*pos) & 0xFF00) >> 8;
	u8 rs = *pos & 0xFF;

	rs++;
	if (rs >= RATE_SECTION_NUM) {
		rs = 0;
		path++;
	}

	if (path >= RF_PATH_MAX)
		return NULL;

	*pos = (path << 8) | rs;

	return pos;
}

static int proc_get_tx_power_idx(struct seq_file *m, void *v)
{
	struct net_device *dev = m->private;
	_adapter *adapter = (_adapter *)rtw_netdev_priv(dev);
	u32 pos = *((loff_t *)(v));
	u8 path = (pos & 0xFF00) >> 8;
	u8 rs = pos & 0xFF;

	if (0)
		RTW_INFO("%s path=%u, rs=%u\n", __func__, path, rs);

	if (path == RF_PATH_A && rs == CCK)
		dump_tx_power_idx_title(m, adapter);
	dump_tx_power_idx_by_path_rs(m, adapter, path, rs);

	return 0;
}

static struct seq_operations seq_ops_tx_power_idx = {
	.start = proc_start_tx_power_idx,
	.stop  = proc_stop_tx_power_idx,
	.next  = proc_next_tx_power_idx,
	.show  = proc_get_tx_power_idx,
};

static void *proc_start_txpwr_total_dbm(struct seq_file *m, loff_t *pos)
{
	u8 rs = *pos;

	if (rs >= RATE_SECTION_NUM)
		return NULL;

	return pos;
}

static void proc_stop_txpwr_total_dbm(struct seq_file *m, void *v)
{
}

static void *proc_next_txpwr_total_dbm(struct seq_file *m, void *v, loff_t *pos)
{
	u8 rs = *pos;

	rs++;
	if (rs >= RATE_SECTION_NUM)
		return NULL;

	*pos = rs;

	return pos;
}

static int proc_get_txpwr_total_dbm(struct seq_file *m, void *v)
{
	struct net_device *dev = m->private;
	_adapter *adapter = (_adapter *)rtw_netdev_priv(dev);
	u32 pos = *((loff_t *)(v));
	u8 rs = pos;

	if (rs == CCK)
		dump_txpwr_total_dbm_title(m, adapter);
	dump_txpwr_total_dbm_by_rs(m, adapter, rs);

	return 0;
}

static struct seq_operations seq_ops_txpwr_total_dbm = {
	.start = proc_start_txpwr_total_dbm,
	.stop  = proc_stop_txpwr_total_dbm,
	.next  = proc_next_txpwr_total_dbm,
	.show  = proc_get_txpwr_total_dbm,
};
#endif

int proc_get_mac_addr(struct seq_file *m, void *v)
{
	struct net_device *dev = m->private;
	_adapter *adapter = (_adapter *)rtw_netdev_priv(dev);

	rtw_dump_macaddr(m, adapter);
	return 0;
}

static int proc_get_skip_band(struct seq_file *m, void *v)
{
	struct net_device *dev = m->private;
	_adapter *adapter = (_adapter *)rtw_netdev_priv(dev);
	int bandskip;

	bandskip = RTW_GET_SCAN_BAND_SKIP(adapter);
	RTW_PRINT_SEL(m, "bandskip:0x%02x\n", bandskip);
	return 0;
}

static ssize_t proc_set_skip_band(struct file *file, const char __user *buffer, size_t count, loff_t *pos, void *data)
{
	struct net_device *dev = data;
	_adapter *padapter = (_adapter *)rtw_netdev_priv(dev);
	char tmp[6];
	u8 skip_band;

	if (count < 1)
		return -EFAULT;

	if (count > sizeof(tmp)) {
		rtw_warn_on(1);
		return -EFAULT;
	}
	if (buffer && !copy_from_user(tmp, buffer, count)) {

		int num = sscanf(tmp, "%hhu", &skip_band);

		if (num < 1)
			return -EINVAL;

		if (1 == skip_band)
			RTW_SET_SCAN_BAND_SKIP(padapter, BAND_24G);
		else if (2 == skip_band)
			RTW_SET_SCAN_BAND_SKIP(padapter, BAND_5G);
		else if (3 == skip_band)
			RTW_CLR_SCAN_BAND_SKIP(padapter, BAND_24G);
		else if (4 == skip_band)
			RTW_CLR_SCAN_BAND_SKIP(padapter, BAND_5G);
	}
	return count;

}

#ifdef CONFIG_RTW_ACS
static void rtw_acs_chan_info_dump(struct seq_file *m, _adapter *a)
{
	struct net_device *dev = m->private;
	_adapter *adapter = (_adapter *)rtw_netdev_priv(dev);
	struct dvobj_priv *d = adapter_to_dvobj(adapter);
	struct rtw_acs_info_parm rpt;
	struct rtw_chset *chset = adapter_to_chset(adapter);
	int i;

	RTW_PRINT_SEL(m, "%5s  %3s  %5s  %4s(%%)  %4s(%%)\n",
						"Index", "CH", "Band", "NHM", "CLM");

	for (i = 0; i < MAX_CHANNEL_NUM && chset->chs[i].ChannelNum != 0; i++){
		_rtw_memset(&rpt, 0, sizeof(struct rtw_acs_info_parm));
		rtw_acs_get_report(adapter, chset->chs[i].band, chset->chs[i].ChannelNum, &rpt);
		RTW_PRINT_SEL(m, "%5d  %3d  %5s  %4d  %7d\n",
			i, chset->chs[i].ChannelNum ,band_str(chset->chs[i].band), rpt.rpt.nhm_ratio, rpt.rpt.clm_ratio);
	}
}

static int proc_get_chan_info(struct seq_file *m, void *v)
{
	struct net_device *dev = m->private;
	_adapter *adapter = (_adapter *)rtw_netdev_priv(dev);

	rtw_acs_chan_info_dump(m, adapter);
	return 0;
}

#ifdef WKARD_ACS
static void rtw_acs_info_dump(struct seq_file *m, _adapter *a)
{
}
static int proc_get_best_chan(struct seq_file *m, void *v)
{
	struct net_device *dev = m->private;
	_adapter *adapter = (_adapter *)rtw_netdev_priv(dev);

	if (IS_ACS_ENABLE(adapter))
		rtw_acs_info_dump(m, adapter);
	else
		_RTW_PRINT_SEL(m,"ACS disabled\n");
	return 0;
}

static ssize_t proc_set_acs(struct file *file, const char __user *buffer, size_t count, loff_t *pos, void *data)
{
#ifdef CONFIG_RTW_ACS_DBG
	struct net_device *dev = data;
	_adapter *padapter = (_adapter *)rtw_netdev_priv(dev);
	char tmp[32];
	u8 acs_state = 0;
	u16 scan_ch_ms= 0, acs_scan_ch_ms = 0;
	u8 scan_type = SCAN_ACTIVE, igi= 0, bw = 0;
	u8 acs_scan_type = SCAN_ACTIVE, acs_igi= 0, acs_bw = 0;

	if (count < 1)
		return -EFAULT;

	if (count > sizeof(tmp)) {
		rtw_warn_on(1);
		return -EFAULT;
	}
	if (buffer && !copy_from_user(tmp, buffer, count)) {

		int num = sscanf(tmp, "%hhu %hhu %hu %hhx %hhu",
			&acs_state, &scan_type, &scan_ch_ms, &igi, &bw);

		if (num < 1)
			return -EINVAL;

		if (acs_state)
			rtw_acs_start(padapter);
		else
			rtw_acs_stop(padapter);
		num = num -1;

		if(num) {
			if (num-- > 0)
				acs_scan_type = scan_type;
			if (num-- > 0)
				acs_scan_ch_ms = scan_ch_ms;
			if (num-- > 0)
				acs_igi = igi;
			if (num-- > 0)
				acs_bw = bw;
			rtw_acs_adv_setting(padapter, acs_scan_type, acs_scan_ch_ms, acs_igi, acs_bw);
		}
	}
#endif /*CONFIG_RTW_ACS_DBG*/
	return count;
}
#endif /* WKARD_ACS */
#endif /*CONFIG_RTW_ACS*/

static int proc_get_env_rpt(struct seq_file *m, void *v)
{
	struct net_device *dev = m->private;
	_adapter *adapter = (_adapter *)rtw_netdev_priv(dev);

	rtw_dump_env_rpt(adapter, m);

	return 0;
}

static int proc_get_hal_spec(struct seq_file *m, void *v)
{
	struct net_device *dev = m->private;
	_adapter *adapter = (_adapter *)rtw_netdev_priv(dev);

	rtw_hw_dump_hal_spec(m, adapter_to_dvobj(adapter));
	return 0;
}

static int proc_get_hal_trx_mode(struct seq_file *m, void *v)
{
	struct net_device *dev = m->private;
	_adapter *adapter = rtw_netdev_priv(dev);

	rtw_hal_dump_trx_mode(m, adapter);
	return 0;
}

static int proc_get_phy_cap(struct seq_file *m, void *v)
{
	struct net_device *dev = m->private;
	_adapter *adapter = (_adapter *)rtw_netdev_priv(dev);

#ifdef CONFIG_80211N_HT
	rtw_dump_drv_phy_cap(m, adapter);
	rtw_get_dft_phy_cap(m, adapter);
#endif /* CONFIG_80211N_HT */
	return 0;
}

static int proc_dump_rsvd_page(struct seq_file *m, void *v)
{
	struct net_device *dev = m->private;
	_adapter *adapter = (_adapter *)rtw_netdev_priv(dev);

	rtw_hal_dump_rsvd_page(m, adapter, adapter->rsvd_page_offset, adapter->rsvd_page_num);
	return 0;
}
static ssize_t proc_set_rsvd_page_info(struct file *file, const char __user *buffer, size_t count, loff_t *pos, void *data)
{
	struct net_device *dev = data;
	_adapter *padapter = (_adapter *)rtw_netdev_priv(dev);
	char tmp[32];
	u8 page_offset, page_num;

	if (count < 2)
		return -EFAULT;

	if (count > sizeof(tmp)) {
		rtw_warn_on(1);
		return -EFAULT;
	}
	if (buffer && !copy_from_user(tmp, buffer, count)) {

		int num = sscanf(tmp, "%hhu %hhu", &page_offset, &page_num);

		if (num < 2)
			return -EINVAL;
		padapter->rsvd_page_offset = page_offset;
		padapter->rsvd_page_num = page_num;
	}
	return count;
}

#ifdef CONFIG_SUPPORT_FIFO_DUMP
static int proc_dump_fifo(struct seq_file *m, void *v)
{
	struct net_device *dev = m->private;
	_adapter *adapter = (_adapter *)rtw_netdev_priv(dev);

	rtw_dump_fifo(m, adapter, adapter->fifo_sel, adapter->fifo_addr, adapter->fifo_size);
	return 0;
}
static ssize_t proc_set_fifo_info(struct file *file, const char __user *buffer, size_t count, loff_t *pos, void *data)
{
	struct net_device *dev = data;
	_adapter *padapter = (_adapter *)rtw_netdev_priv(dev);
	char tmp[32];
	u8 fifo_sel = 0;
	u32 fifo_addr = 0;
	u32 fifo_size = 0;

	if (count < 3)
		return -EFAULT;

	if (count > sizeof(tmp)) {
		rtw_warn_on(1);
		return -EFAULT;
	}
	if (buffer && !copy_from_user(tmp, buffer, count)) {

		int num = sscanf(tmp, "%hhu %x %d", &fifo_sel, &fifo_addr, &fifo_size);

		if (num < 3)
			return -EINVAL;

		padapter->fifo_sel = fifo_sel;
		padapter->fifo_addr = fifo_addr;
		padapter->fifo_size = fifo_size;
	}
	return count;
}
#endif


#ifdef CONFIG_WOWLAN
int proc_dump_pattern_cam(struct seq_file *m, void *v)
{
	struct net_device *dev = m->private;
	_adapter *padapter = (_adapter *)rtw_netdev_priv(dev);
	struct pwrctrl_priv *pwrpriv = adapter_to_pwrctl(padapter);
	int i;
	#if 0 /* WOW_ToDo */
	struct  rtl_wow_pattern context;

	for (i = 0 ; i < pwrpriv->wowlan_pattern_idx; i++) {
		rtw_wow_pattern_read_cam_ent(padapter, i, &context);
		rtw_dump_wow_pattern(m, &context, i);
	}
	#endif
	return 0;
}
#endif


static int proc_get_napi_info(struct seq_file *m, void *v)
{
	struct net_device *dev = m->private;
	_adapter *adapter = (_adapter *)rtw_netdev_priv(dev);
	struct registry_priv *pregistrypriv = &adapter->registrypriv;
	u8 napi = 0, gro = 0;
	u32 weight = 0;
	struct dvobj_priv *d;
	d = adapter_to_dvobj(adapter);


#ifdef CONFIG_RTW_NAPI
	if (pregistrypriv->en_napi) {
		napi = 1;
		weight = RTL_NAPI_WEIGHT;
	}

#ifdef CONFIG_RTW_GRO
	if (pregistrypriv->en_gro)
		gro = 1;
#endif /* CONFIG_RTW_GRO */
#endif /* CONFIG_RTW_NAPI */

	if (napi) {
		RTW_PRINT_SEL(m, "NAPI enable, weight=%d\n", weight);
#ifdef CONFIG_RTW_NAPI_DYNAMIC
		RTW_PRINT_SEL(m, "Dynamaic NAPI mechanism is on, current NAPI %s\n",
			      d->en_napi_dynamic ? "enable" : "disable");
		RTW_PRINT_SEL(m, "Dynamaic NAPI info:\n"
				 "\ttcp_rx_threshold = %d Mbps\n"
				 "\tcur_rx_tp = %d Mbps\n",
			      pregistrypriv->napi_threshold,
			      d->traffic_stat.cur_rx_tp);
#endif /* CONFIG_RTW_NAPI_DYNAMIC */
	} else {
		RTW_PRINT_SEL(m, "NAPI disable\n");
	}
	RTW_PRINT_SEL(m, "GRO %s\n", gro?"enable":"disable");

	return 0;

}

#ifdef CONFIG_RTW_NAPI_DYNAMIC
static ssize_t proc_set_napi_th(struct file *file, const char __user *buffer, size_t count, loff_t *pos, void *data)
{
	struct net_device *dev = data;
	_adapter *adapter = (_adapter *)rtw_netdev_priv(dev);
	struct registry_priv *registry = &adapter->registrypriv;
	struct dvobj_priv *dvobj = adapter_to_dvobj(adapter);
	_adapter *iface = NULL;
	char tmp[32] = {0};
	int thrshld = 0;
	int num = 0, i = 0;


	if (count < 1)
		return -EFAULT;

	if (count > sizeof(tmp)) {
		rtw_warn_on(1);
		return -EFAULT;
	}

	RTW_INFO("%s: Last threshold = %d Mbps\n", __FUNCTION__, registry->napi_threshold);


	for (i = 0; i < dvobj->iface_nums; i++) {
		iface = dvobj->padapters[i];
		if (iface) {
			if (buffer && !copy_from_user(tmp, buffer, count)) {
				registry = &iface->registrypriv;
				num = sscanf(tmp, "%d", &thrshld);
				if (num > 0) {
					if (thrshld > 0)
						registry->napi_threshold = thrshld;
				}
			}
		}
	}
	RTW_INFO("%s: New threshold = %d Mbps\n", __FUNCTION__, registry->napi_threshold);
	RTW_INFO("%s: Current RX throughput = %d Mbps\n",
		 __FUNCTION__, adapter_to_dvobj(adapter)->traffic_stat.cur_rx_tp);

	return count;
}
#endif /* CONFIG_RTW_NAPI_DYNAMIC */


ssize_t proc_set_dynamic_agg_enable(struct file *file, const char __user *buffer, size_t count, loff_t *pos, void *data)
{
	struct net_device *dev = data;
	_adapter *padapter = (_adapter *)rtw_netdev_priv(dev);
	char tmp[32];
	int enable = 0, i = 0;

	if (count > sizeof(tmp)) {
		rtw_warn_on(1);
		return -EFAULT;
	}

	if (buffer && !copy_from_user(tmp, buffer, count)) {

		struct dvobj_priv *dvobj = adapter_to_dvobj(padapter);
		_adapter *iface = NULL;
		int num = sscanf(tmp, "%d", &enable);

		if (num !=  1) {
			RTW_INFO("invalid parameter!\n");
			return count;
		}

		RTW_INFO("dynamic_agg_enable:%d\n", enable);

		for (i = 0; i < dvobj->iface_nums; i++) {
			iface = dvobj->padapters[i];
			if (iface)
				iface->registrypriv.dynamic_agg_enable = enable;
		}

	}

	return count;

}

static int proc_get_dynamic_agg_enable(struct seq_file *m, void *v)
{
	struct net_device *dev = m->private;
	_adapter *adapter = (_adapter *)rtw_netdev_priv(dev);
	struct registry_priv *pregistrypriv = &adapter->registrypriv;

	RTW_PRINT_SEL(m, "dynamic_agg_enable:%d\n", pregistrypriv->dynamic_agg_enable);

	return 0;
}

#ifdef CONFIG_RTW_WDS
static int proc_get_wds_en(struct seq_file *m, void *v)
{
	struct net_device *dev = m->private;
	_adapter *adapter = rtw_netdev_priv(dev);

	if (MLME_STATE(adapter) & (WIFI_AP_STATE | WIFI_STATION_STATE))
		RTW_PRINT_SEL(m, "%d\n", adapter_use_wds(adapter));

	return 0;
}

static ssize_t proc_set_wds_en(struct file *file, const char __user *buffer, size_t count, loff_t *pos, void *data)
{
	struct net_device *dev = data;
	_adapter *adapter = rtw_netdev_priv(dev);
	char tmp[32];

	if (!(MLME_STATE(adapter) & (WIFI_AP_STATE | WIFI_STATION_STATE)))
		return -EFAULT;

	if (count < 1)
		return -EFAULT;

	if (count > sizeof(tmp)) {
		rtw_warn_on(1);
		return -EFAULT;
	}

	if (buffer && !copy_from_user(tmp, buffer, count)) {
		u8 enable;
		int num = sscanf(tmp, "%hhu", &enable);

		if (num >= 1)
			adapter_set_use_wds(adapter, enable);
	}

	return count;
}

static ssize_t proc_set_sta_wds_en(struct file *file, const char __user *buffer, size_t count, loff_t *pos, void *data)
{
	struct net_device *dev = data;
	_adapter *adapter = rtw_netdev_priv(dev);
	char tmp[32];

	if (count < 1)
		return -EFAULT;

	if (count > sizeof(tmp)) {
		rtw_warn_on(1);
		return -EFAULT;
	}

	if (buffer && !copy_from_user(tmp, buffer, count)) {
		u8 enable;
		u8 addr[ETH_ALEN];
		struct sta_info *sta;
		int num = sscanf(tmp, "%hhu "MAC_SFMT, &enable, MAC_SARG(addr));

		if (num != 7)
			return -EINVAL;

		if (IS_MCAST(addr) || _rtw_memcmp(adapter_mac_addr(adapter), addr, ETH_ALEN))
			return -EINVAL;

		sta = rtw_get_stainfo(&adapter->stapriv, addr);
		if (!sta)
			return -EINVAL;

		if (enable)
			sta->flags |= WLAN_STA_WDS;
		else
			sta->flags &= ~WLAN_STA_WDS;
	}

	return count;
}

static int proc_get_wds_gptr(struct seq_file *m, void *v)
{
	struct net_device *dev = m->private;
	_adapter *adapter = rtw_netdev_priv(dev);

	if (MLME_IS_STA(adapter) && MLME_IS_ASOC(adapter))
		dump_wgptr(m, adapter);

	return 0;
}

#ifdef CONFIG_AP_MODE
static int proc_get_wds_path(struct seq_file *m, void *v)
{
	struct net_device *dev = m->private;
	_adapter *adapter = rtw_netdev_priv(dev);

	if (MLME_IS_AP(adapter) && MLME_IS_ASOC(adapter))
		dump_wpath(m, adapter);

	return 0;
}
#endif /* CONFIG_AP_MODE */
#endif /* CONFIG_RTW_WDS */

#ifdef CONFIG_RTW_MULTI_AP
static int proc_get_multi_ap_opmode(struct seq_file *m, void *v)
{
	struct net_device *dev = m->private;
	_adapter *adapter = rtw_netdev_priv(dev);

	if (MLME_STATE(adapter) & (WIFI_AP_STATE | WIFI_STATION_STATE))
		RTW_PRINT_SEL(m, "0x%02x\n", adapter->multi_ap);

	return 0;
}

static ssize_t proc_set_multi_ap_opmode(struct file *file, const char __user *buffer, size_t count, loff_t *pos, void *data)
{
	struct net_device *dev = data;
	_adapter *adapter = rtw_netdev_priv(dev);
	char tmp[32];

	if (!(MLME_STATE(adapter) & (WIFI_AP_STATE | WIFI_STATION_STATE)))
		return -EFAULT;

	if (count < 1)
		return -EFAULT;

	if (count > sizeof(tmp)) {
		rtw_warn_on(1);
		return -EFAULT;
	}

	if (buffer && !copy_from_user(tmp, buffer, count)) {
		u8 mode;
		int num = sscanf(tmp, "%hhx", &mode);

		if (num >= 1) {
			if (MLME_IS_AP(adapter))
				adapter->multi_ap = mode & (MULTI_AP_FRONTHAUL_BSS | MULTI_AP_BACKHAUL_BSS);
			else
				adapter->multi_ap = mode & MULTI_AP_BACKHAUL_STA;
			if (adapter->multi_ap & (MULTI_AP_BACKHAUL_BSS | MULTI_AP_BACKHAUL_STA))
				adapter_set_use_wds(adapter, 1);
		}
	}

	return count;
}

static int proc_get_unassoc_sta(struct seq_file *m, void *v)
{
	struct net_device *dev = m->private;
	_adapter *adapter = GET_PRIMARY_ADAPTER(rtw_netdev_priv(dev));

	dump_unassoc_sta(m, adapter);
	return 0;
}

ssize_t proc_set_unassoc_sta(struct file *file, const char __user *buffer, size_t count, loff_t *pos, void *data)
{
	struct net_device *dev = data;
	_adapter *adapter = GET_PRIMARY_ADAPTER(rtw_netdev_priv(dev));
	char tmp[17 * 10 + 32] = {0};
	char cmd[32];
	u8 mode;
	u8 stype = 0;
	u8 addr[ETH_ALEN];

#define UNASOC_STA_CMD_MODE	0
#define UNASOC_STA_CMD_ADD	1
#define UNASOC_STA_CMD_DEL	2
#define UNASOC_STA_CMD_CLR	3
#define UNASOC_STA_CMD_UNINT	4
#define UNASOC_STA_CMD_NUM	5

	static const char * const unasoc_sta_cmd_str[] = {
		"mode",
		"add",
		"del",
		"clr",
		"uninterest",
	};
	u8 cmd_id = UNASOC_STA_CMD_NUM;

	if (count < 1)
		return -EFAULT;

	if (count > sizeof(tmp)) {
		RTW_WARN(FUNC_ADPT_FMT" input string too long\n", FUNC_ADPT_ARG(adapter));
		rtw_warn_on(1);
		return -EFAULT;
	}

	if (buffer && !copy_from_user(tmp, buffer, count)) {
		/*
		* mode <stype>,<mode>
		* add <macaddr> [<macaddr>]
		* del <macaddr> [<macaddr>]
		* clr
		*/
		char *c, *next;
		int i;
		u8 is_bcast;

		next = tmp;
		c = strsep(&next, " \t");
		if (!c || sscanf(c, "%s", cmd) != 1)
			goto exit;

		for (i = 0; i < UNASOC_STA_CMD_NUM; i++)
			if (strcmp(unasoc_sta_cmd_str[i], cmd) == 0)
				cmd_id = i;

		switch (cmd_id) {
		case UNASOC_STA_CMD_MODE:
			c = strsep(&next, " \t");
			if (!c || sscanf(c, "%hhu,%hhu", &stype, &mode) != 2) {
				RTW_WARN(FUNC_ADPT_FMT" invalid arguments of mode cmd\n", FUNC_ADPT_ARG(adapter));
				goto exit;
			}

			if (stype >= UNASOC_STA_SRC_NUM) {
				RTW_WARN(FUNC_ADPT_FMT" invalid stype:%u\n", FUNC_ADPT_ARG(adapter), stype);
				goto exit;
			}

			if (mode >= UNASOC_STA_MODE_NUM) {
				RTW_WARN(FUNC_ADPT_FMT" invalid mode:%u\n", FUNC_ADPT_ARG(adapter), mode);
				goto exit;
			}

			rtw_unassoc_sta_set_mode(adapter, stype, mode);
			break;

		case UNASOC_STA_CMD_ADD:
		case UNASOC_STA_CMD_DEL:
		case UNASOC_STA_CMD_UNINT:
			/* check for macaddr list */
			c = strsep(&next, " \t");
			while (c != NULL) {
				if (sscanf(c, MAC_SFMT, MAC_SARG(addr)) != 6)
					break;

				is_bcast = is_broadcast_mac_addr(addr);
				if (is_bcast
					|| rtw_check_invalid_mac_address(addr, 0) == _FALSE
				) {
					if (cmd_id == UNASOC_STA_CMD_DEL) {
						if (is_bcast) {
							rtw_del_unassoc_sta_queue(adapter);
							break;
						} else
							rtw_del_unassoc_sta(adapter, addr);
					} else if (cmd_id == UNASOC_STA_CMD_UNINT) {
						if (is_bcast) {
							rtw_undo_all_interested_unassoc_sta(adapter);
							break;
						} else
							rtw_undo_interested_unassoc_sta(adapter, addr);
					} else if (!is_bcast)
						rtw_add_interested_unassoc_sta(adapter, addr);
				}

				c = strsep(&next, " \t");
			}
			break;

		case UNASOC_STA_CMD_CLR:
			/* clear sta list */
			rtw_del_unassoc_sta_queue(adapter);
			goto exit;

		default:
			RTW_WARN(FUNC_ADPT_FMT" invalid cmd:\"%s\"\n", FUNC_ADPT_ARG(adapter), cmd);
			goto exit;
		}
	}

exit:
	return count;
}

#ifdef CONFIG_IOCTL_CFG80211
static u8 assoc_req_mac_addr[6];
int proc_get_sta_assoc_req_frame_body(struct seq_file *m, void *v)
{
	struct net_device *dev = m->private;
	_adapter *adapter = (_adapter *)rtw_netdev_priv(dev);

	if (MLME_IS_AP(adapter)) {
		struct sta_info *psta;
		u8 *passoc_req = NULL;
		u32 assoc_req_len = 0;

		psta = rtw_get_stainfo(&adapter->stapriv, assoc_req_mac_addr);
		if (psta == NULL) {
			RTW_PRINT(FUNC_ADPT_FMT" sta("MAC_FMT") not found\n",
				  FUNC_ADPT_ARG(adapter), MAC_ARG(assoc_req_mac_addr));
			return 0;
		}

		RTW_PRINT(FUNC_ADPT_FMT" sta("MAC_FMT") found\n",
			  FUNC_ADPT_ARG(adapter), MAC_ARG(assoc_req_mac_addr));
		_rtw_spinlock_bh(&psta->lock);
		if (psta->passoc_req && psta->assoc_req_len > 0) {
			passoc_req = rtw_zmalloc(psta->assoc_req_len);
			if (passoc_req) {
				assoc_req_len = psta->assoc_req_len;
				_rtw_memcpy(passoc_req, psta->passoc_req, assoc_req_len);
			}
		}

		_rtw_spinunlock_bh(&psta->lock);
		if (passoc_req && assoc_req_len > IEEE80211_3ADDR_LEN) {
			u8 *body = passoc_req + IEEE80211_3ADDR_LEN;
			u32 body_len = assoc_req_len - IEEE80211_3ADDR_LEN;
			u16 i;

			for (i = 0; i < body_len; i++)
				_RTW_PRINT_SEL(m, "%02X", body[i]);
			_RTW_PRINT_SEL(m, "\n");
		}

		if (passoc_req && assoc_req_len > 0)
			rtw_mfree(passoc_req, assoc_req_len);
	}

	return 0;
}

ssize_t proc_set_sta_assoc_req_frame_body(struct file *file, const char __user *buffer, size_t count, loff_t *pos, void *data)
{
	struct net_device *dev = data;
	_adapter *adapter = (_adapter *)rtw_netdev_priv(dev);
	char tmp[18] = {0};

	if (count < 1)
		return -EFAULT;

	if (count > sizeof(tmp)) {
		rtw_warn_on(1);
		return -EFAULT;
	}

	if (buffer && !copy_from_user(tmp, buffer, count)) {
		if (sscanf(tmp, MAC_SFMT, MAC_SARG(assoc_req_mac_addr)) != 6) {
			_rtw_memset(assoc_req_mac_addr, 0, 6);
			RTW_PRINT(FUNC_ADPT_FMT" Invalid format\n",
				  FUNC_ADPT_ARG(adapter));
		}

	}

	return count;
}
#endif /* CONFIG_IOCTL_CFG80211 */

static int proc_get_ch_util_threshold(struct seq_file *m, void *v)
{
	struct net_device *dev = m->private;
	_adapter *adapter = GET_PRIMARY_ADAPTER(rtw_netdev_priv(dev));

	RTW_PRINT_SEL(m, "%hhu\n", adapter->ch_util_threshold);

	return 0;
}

static ssize_t proc_set_ch_util_threshold(struct file *file, const char __user *buffer, size_t count, loff_t *pos, void *data)
{
	struct net_device *dev = data;
	_adapter *adapter = GET_PRIMARY_ADAPTER(rtw_netdev_priv(dev));
	char tmp[4];

	if (count < 1)
		return -EFAULT;

	if (count > sizeof(tmp)) {
		rtw_warn_on(1);
		return -EFAULT;
	}

	if (buffer && !copy_from_user(tmp, buffer, count)) {
		u8 threshold;
		int num = sscanf(tmp, "%hhu", &threshold);

		if (num == 1)
			adapter->ch_util_threshold = threshold;
	}

	return count;
}

static int proc_get_ch_utilization(struct seq_file *m, void *v)
{
	struct net_device *dev = m->private;
	_adapter *adapter = (_adapter *)rtw_netdev_priv(dev);

	RTW_PRINT_SEL(m, "%hhu\n", rtw_get_ch_utilization(adapter));

	return 0;
}
#endif /* CONFIG_RTW_MULTI_AP */

#ifdef CONFIG_RTW_MESH
static int proc_get_mesh_peer_sel_policy(struct seq_file *m, void *v)
{
	struct net_device *dev = m->private;
	_adapter *adapter = (_adapter *)rtw_netdev_priv(dev);

	dump_mesh_peer_sel_policy(m, adapter);

	return 0;
}

#if CONFIG_RTW_MESH_ACNODE_PREVENT
static int proc_get_mesh_acnode_prevent(struct seq_file *m, void *v)
{
	struct net_device *dev = m->private;
	_adapter *adapter = (_adapter *)rtw_netdev_priv(dev);

	if (MLME_IS_MESH(adapter))
		dump_mesh_acnode_prevent_settings(m, adapter);

	return 0;
}

static ssize_t proc_set_mesh_acnode_prevent(struct file *file, const char __user *buffer, size_t count, loff_t *pos, void *data)
{
	struct net_device *dev = data;
	_adapter *adapter = (_adapter *)rtw_netdev_priv(dev);
	char tmp[32];

	if (count < 1)
		return -EFAULT;

	if (count > sizeof(tmp)) {
		rtw_warn_on(1);
		return -EFAULT;
	}

	if (buffer && !copy_from_user(tmp, buffer, count)) {
		struct mesh_peer_sel_policy *peer_sel_policy = &adapter->mesh_cfg.peer_sel_policy;
		u8 enable;
		u32 conf_timeout_ms;
		u32 notify_timeout_ms;
		int num = sscanf(tmp, "%hhu %u %u", &enable, &conf_timeout_ms, &notify_timeout_ms);

		if (num >= 1)
			peer_sel_policy->acnode_prevent = enable;
		if (num >= 2)
			peer_sel_policy->acnode_conf_timeout_ms = conf_timeout_ms;
		if (num >= 3)
			peer_sel_policy->acnode_notify_timeout_ms = notify_timeout_ms;
	}

	return count;
}
#endif /* CONFIG_RTW_MESH_ACNODE_PREVENT */

#if CONFIG_RTW_MESH_OFFCH_CAND
static int proc_get_mesh_offch_cand(struct seq_file *m, void *v)
{
	struct net_device *dev = m->private;
	_adapter *adapter = (_adapter *)rtw_netdev_priv(dev);

	if (MLME_IS_MESH(adapter))
		dump_mesh_offch_cand_settings(m, adapter);

	return 0;
}

static ssize_t proc_set_mesh_offch_cand(struct file *file, const char __user *buffer, size_t count, loff_t *pos, void *data)
{
	struct net_device *dev = data;
	_adapter *adapter = (_adapter *)rtw_netdev_priv(dev);
	char tmp[32];

	if (count < 1)
		return -EFAULT;

	if (count > sizeof(tmp)) {
		rtw_warn_on(1);
		return -EFAULT;
	}

	if (buffer && !copy_from_user(tmp, buffer, count)) {
		struct mesh_peer_sel_policy *peer_sel_policy = &adapter->mesh_cfg.peer_sel_policy;
		u8 enable;
		u32 find_int_ms;
		int num = sscanf(tmp, "%hhu %u", &enable, &find_int_ms);

		if (num >= 1)
			peer_sel_policy->offch_cand = enable;
		if (num >= 2)
			peer_sel_policy->offch_find_int_ms = find_int_ms;
	}

	return count;
}
#endif /* CONFIG_RTW_MESH_OFFCH_CAND */

#if CONFIG_RTW_MESH_PEER_BLACKLIST
static int proc_get_mesh_peer_blacklist(struct seq_file *m, void *v)
{
	struct net_device *dev = m->private;
	_adapter *adapter = (_adapter *)rtw_netdev_priv(dev);

	if (MLME_IS_MESH(adapter)) {
		dump_mesh_peer_blacklist_settings(m, adapter);
		if (MLME_IS_ASOC(adapter))
			dump_mesh_peer_blacklist(m, adapter);
	}

	return 0;
}

static ssize_t proc_set_mesh_peer_blacklist(struct file *file, const char __user *buffer, size_t count, loff_t *pos, void *data)
{
	struct net_device *dev = data;
	_adapter *adapter = (_adapter *)rtw_netdev_priv(dev);
	char tmp[32];

	if (count < 1)
		return -EFAULT;

	if (count > sizeof(tmp)) {
		rtw_warn_on(1);
		return -EFAULT;
	}

	if (buffer && !copy_from_user(tmp, buffer, count)) {
		struct mesh_peer_sel_policy *peer_sel_policy = &adapter->mesh_cfg.peer_sel_policy;
		u32 conf_timeout_ms;
		u32 blacklist_timeout_ms;
		int num = sscanf(tmp, "%u %u", &conf_timeout_ms, &blacklist_timeout_ms);

		if (num >= 1)
			peer_sel_policy->peer_conf_timeout_ms = conf_timeout_ms;
		if (num >= 2)
			peer_sel_policy->peer_blacklist_timeout_ms = blacklist_timeout_ms;
	}

	return count;
}
#endif /* CONFIG_RTW_MESH_PEER_BLACKLIST */

#if CONFIG_RTW_MESH_CTO_MGATE_BLACKLIST
static int proc_get_mesh_cto_mgate_require(struct seq_file *m, void *v)
{
	struct net_device *dev = m->private;
	_adapter *adapter = (_adapter *)rtw_netdev_priv(dev);

	if (MLME_IS_MESH(adapter))
		RTW_PRINT_SEL(m, "%u\n", adapter->mesh_cfg.peer_sel_policy.cto_mgate_require);

	return 0;
}

static ssize_t proc_set_mesh_cto_mgate_require(struct file *file, const char __user *buffer, size_t count, loff_t *pos, void *data)
{
	struct net_device *dev = data;
	_adapter *adapter = (_adapter *)rtw_netdev_priv(dev);
	char tmp[32];

	if (count < 1)
		return -EFAULT;

	if (count > sizeof(tmp)) {
		rtw_warn_on(1);
		return -EFAULT;
	}

	if (buffer && !copy_from_user(tmp, buffer, count)) {
		struct mesh_peer_sel_policy *peer_sel_policy = &adapter->mesh_cfg.peer_sel_policy;
		u8 require;
		int num = sscanf(tmp, "%hhu", &require);

		if (num >= 1) {
			peer_sel_policy->cto_mgate_require = require;
			#if CONFIG_RTW_MESH_CTO_MGATE_CARRIER
			if (rtw_mesh_cto_mgate_required(adapter))
				rtw_netif_carrier_off(adapter->pnetdev);
			else
				rtw_netif_carrier_on(adapter->pnetdev);
			#endif
		}
	}

	return count;
}

static int proc_get_mesh_cto_mgate_blacklist(struct seq_file *m, void *v)
{
	struct net_device *dev = m->private;
	_adapter *adapter = (_adapter *)rtw_netdev_priv(dev);

	if (MLME_IS_MESH(adapter)) {
		dump_mesh_cto_mgate_blacklist_settings(m, adapter);
		if (MLME_IS_ASOC(adapter))
			dump_mesh_cto_mgate_blacklist(m, adapter);
	}

	return 0;
}

static ssize_t proc_set_mesh_cto_mgate_blacklist(struct file *file, const char __user *buffer, size_t count, loff_t *pos, void *data)
{
	struct net_device *dev = data;
	_adapter *adapter = (_adapter *)rtw_netdev_priv(dev);
	char tmp[32];

	if (count < 1)
		return -EFAULT;

	if (count > sizeof(tmp)) {
		rtw_warn_on(1);
		return -EFAULT;
	}

	if (buffer && !copy_from_user(tmp, buffer, count)) {
		struct mesh_peer_sel_policy *peer_sel_policy = &adapter->mesh_cfg.peer_sel_policy;
		u32 conf_timeout_ms;
		u32 blacklist_timeout_ms;
		int num = sscanf(tmp, "%u %u", &conf_timeout_ms, &blacklist_timeout_ms);

		if (num >= 1)
			peer_sel_policy->cto_mgate_conf_timeout_ms = conf_timeout_ms;
		if (num >= 2)
			peer_sel_policy->cto_mgate_blacklist_timeout_ms = blacklist_timeout_ms;
	}

	return count;
}
#endif /* CONFIG_RTW_MESH_CTO_MGATE_BLACKLIST */

static int proc_get_mesh_networks(struct seq_file *m, void *v)
{
	struct net_device *dev = m->private;
	_adapter *adapter = (_adapter *)rtw_netdev_priv(dev);

	dump_mesh_networks(m, adapter);

	return 0;
}

static int proc_get_mesh_plink_ctl(struct seq_file *m, void *v)
{
	struct net_device *dev = m->private;
	_adapter *adapter = (_adapter *)rtw_netdev_priv(dev);

	if (MLME_IS_MESH(adapter))
		dump_mesh_plink_ctl(m, adapter);

	return 0;
}

static int proc_get_mesh_mpath(struct seq_file *m, void *v)
{
	struct net_device *dev = m->private;
	_adapter *adapter = (_adapter *)rtw_netdev_priv(dev);

	if (MLME_IS_MESH(adapter) && MLME_IS_ASOC(adapter))
		dump_mpath(m, adapter);

	return 0;
}

static int proc_get_mesh_mpp(struct seq_file *m, void *v)
{
	struct net_device *dev = m->private;
	_adapter *adapter = (_adapter *)rtw_netdev_priv(dev);

	if (MLME_IS_MESH(adapter) && MLME_IS_ASOC(adapter))
		dump_mpp(m, adapter);

	return 0;
}

static int proc_get_mesh_known_gates(struct seq_file *m, void *v)
{
	struct net_device *dev = m->private;
	_adapter *adapter = (_adapter *)rtw_netdev_priv(dev);

	if (MLME_IS_MESH(adapter))
		dump_known_gates(m, adapter);

	return 0;
}

#if CONFIG_RTW_MESH_DATA_BMC_TO_UC
static int proc_get_mesh_b2u_flags(struct seq_file *m, void *v)
{
	struct net_device *dev = m->private;
	_adapter *adapter = (_adapter *)rtw_netdev_priv(dev);

	if (MLME_IS_MESH(adapter))
		dump_mesh_b2u_flags(m, adapter);

	return 0;
}

static ssize_t proc_set_mesh_b2u_flags(struct file *file, const char __user *buffer, size_t count, loff_t *pos, void *data)
{
	struct net_device *dev = data;
	_adapter *adapter = (_adapter *)rtw_netdev_priv(dev);
	char tmp[32];

	if (count < 1)
		return -EFAULT;

	if (count > sizeof(tmp)) {
		rtw_warn_on(1);
		return -EFAULT;
	}

	if (buffer && !copy_from_user(tmp, buffer, count)) {
		struct rtw_mesh_cfg *mcfg = &adapter->mesh_cfg;
		u8 msrc, mfwd;
		int num = sscanf(tmp, "%hhx %hhx", &msrc, &mfwd);

		if (num >= 1)
			mcfg->b2u_flags_msrc = msrc;
		if (num >= 2)
			mcfg->b2u_flags_mfwd = mfwd;
	}

	return count;
}
#endif /* CONFIG_RTW_MESH_DATA_BMC_TO_UC */

static int proc_get_mesh_stats(struct seq_file *m, void *v)
{
	struct net_device *dev = m->private;
	_adapter *adapter = (_adapter *)rtw_netdev_priv(dev);

	if (MLME_IS_MESH(adapter))
		dump_mesh_stats(m, adapter);

	return 0;
}

static int proc_get_mesh_gate_timeout(struct seq_file *m, void *v)
{
	struct net_device *dev = m->private;
	_adapter *adapter = (_adapter *)rtw_netdev_priv(dev);

	if (MLME_IS_MESH(adapter))
		RTW_PRINT_SEL(m, "%u factor\n",
			       adapter->mesh_cfg.path_gate_timeout_factor);

	return 0;
}

static ssize_t proc_set_mesh_gate_timeout(struct file *file, const char __user *buffer, size_t count, loff_t *pos, void *data)
{
	struct net_device *dev = data;
	_adapter *adapter = (_adapter *)rtw_netdev_priv(dev);
	char tmp[32];

	if (count < 1)
		return -EFAULT;

	if (count > sizeof(tmp)) {
		rtw_warn_on(1);
		return -EFAULT;
	}

	if (buffer && !copy_from_user(tmp, buffer, count)) {
		struct rtw_mesh_cfg *mcfg = &adapter->mesh_cfg;
		u32 timeout;
		int num = sscanf(tmp, "%u", &timeout);

		if (num < 1)
			goto exit;

		mcfg->path_gate_timeout_factor = timeout;
	}

exit:
	return count;
}

static int proc_get_mesh_gate_state(struct seq_file *m, void *v)
{
	struct net_device *dev = m->private;
	_adapter *adapter = (_adapter *)rtw_netdev_priv(dev);
	struct rtw_mesh_cfg *mcfg = &adapter->mesh_cfg;
	u8 cto_mgate = 0;

	if (MLME_IS_MESH(adapter)) {
		if (rtw_mesh_is_primary_gate(adapter))
			RTW_PRINT_SEL(m, "PG\n");
		else if (mcfg->dot11MeshGateAnnouncementProtocol)
			RTW_PRINT_SEL(m, "G\n");
		else if (rtw_mesh_gate_num(adapter))
			RTW_PRINT_SEL(m, "C\n");
		else
			RTW_PRINT_SEL(m, "N\n");
	}

	return 0;
}

static int proc_get_peer_alive_based_preq(struct seq_file *m, void *v)
{
	struct net_device *dev = m->private;
	_adapter *adapter= (_adapter *)rtw_netdev_priv(dev);
	struct registry_priv  *rp = &adapter->registrypriv;

	RTW_PRINT_SEL(m, "peer_alive_based_preq = %u\n",
		      rp->peer_alive_based_preq);

	return 0;
}

static ssize_t
proc_set_peer_alive_based_preq(struct file *file, const char __user *buffer,
			       size_t count, loff_t *pos, void *data)
{
	struct net_device *dev = data;
	_adapter *adapter = (_adapter *)rtw_netdev_priv(dev);
	struct registry_priv  *rp = &adapter->registrypriv;
	char tmp[8];
	int num = 0;
	u8 enable = 0;

	if (count > sizeof(tmp)) {
		rtw_warn_on(1);
		return -EFAULT;
	}

	if (!buffer || copy_from_user(tmp, buffer, count))
		goto exit;

	num = sscanf(tmp, "%hhu", &enable);
	if (num !=  1) {
		RTW_ERR("%s: invalid parameter!\n", __FUNCTION__);
		goto exit;
	}

	if (enable > 1) {
		RTW_ERR("%s: invalid value!\n", __FUNCTION__);
		goto exit;
	}
	rp->peer_alive_based_preq = enable;

exit:
	return count;
}

#endif /* CONFIG_RTW_MESH */

#ifdef RTW_BUSY_DENY_SCAN
static int proc_get_scan_interval_thr(struct seq_file *m, void *v)
{
	struct net_device *dev = m->private;
	_adapter *adapter= (_adapter *)rtw_netdev_priv(dev);
	struct registry_priv *rp = &adapter->registrypriv;


	RTW_PRINT_SEL(m, "scan interval threshold = %u ms\n",
		      rp->scan_interval_thr);

	return 0;
}

static ssize_t proc_set_scan_interval_thr(struct file *file,
				          const char __user *buffer,
				          size_t count, loff_t *pos, void *data)
{
	struct net_device *dev = data;
	_adapter *adapter= (_adapter *)rtw_netdev_priv(dev);
	struct registry_priv *rp = &adapter->registrypriv;
	char tmp[12];
	int num = 0;
	u32 thr = 0;


	if (count > sizeof(tmp)) {
		rtw_warn_on(1);
		return -EFAULT;
	}

	if (!buffer || copy_from_user(tmp, buffer, count))
		goto exit;

	num = sscanf(tmp, "%u", &thr);
	if (num != 1) {
		RTW_ERR("%s: invalid parameter!\n", __FUNCTION__);
		goto exit;
	}

	rp->scan_interval_thr = thr;

	RTW_PRINT("%s: scan interval threshold = %u ms\n",
		  __FUNCTION__, rp->scan_interval_thr);

exit:
	return count;
}

#endif /* RTW_BUSY_DENY_SCAN */

static int proc_get_scan_deny(struct seq_file *m, void *v)
{
	struct net_device *dev = m->private;
	_adapter *adapter= (_adapter *)rtw_netdev_priv(dev);
	struct dvobj_priv *dvobj = adapter_to_dvobj(adapter);

	RTW_PRINT_SEL(m, "scan_deny is %s\n", (dvobj->scan_deny == _TRUE) ? "enable":"disable");

	return 0;
}

static ssize_t proc_set_scan_deny(struct file *file, const char __user *buffer,
				size_t count, loff_t *pos, void *data)
{
	struct net_device *dev = data;
	_adapter *adapter = (_adapter *)rtw_netdev_priv(dev);
	struct dvobj_priv *dvobj = adapter_to_dvobj(adapter);
	char tmp[8];
	int num = 0;
	int enable = 0;

	if (count > sizeof(tmp)) {
		rtw_warn_on(1);
		return -EFAULT;
	}

	if (!buffer || copy_from_user(tmp, buffer, count))
		goto exit;

	num = sscanf(tmp, "%d", &enable);
	if (num !=  1) {
		RTW_ERR("%s: invalid parameter!\n", __FUNCTION__);
		goto exit;
	}

	dvobj->scan_deny = enable ? _TRUE : _FALSE;

	RTW_PRINT("%s: scan_deny is %s\n",
		  __FUNCTION__, (dvobj->scan_deny == _TRUE) ? "enable":"disable");

exit:
	return count;
}

int proc_get_cur_beacon_keys(struct seq_file *m, void *v)
{
	struct net_device *dev = m->private;
	_adapter *adapter = rtw_netdev_priv(dev);
	/* ToDo CONFIG_RTW_MLD: [currently primary link only] */
	struct _ADAPTER_LINK *adapter_link = GET_PRIMARY_LINK(adapter);
	struct link_mlme_priv *mlme = &adapter_link->mlmepriv;

	rtw_dump_bcn_keys(m, &mlme->cur_beacon_keys);

	return 0;
}

#if defined(PRIVATE_R) && defined(CONFIG_P2P)
static int proc_get_go_hidden_ssid_mode(struct seq_file *m, void *v)
{
	struct net_device *dev = m->private;
	_adapter *padapter = (_adapter *)rtw_netdev_priv(dev);
	struct registry_priv	*pregpriv = &padapter->registrypriv;

	if (pregpriv)
		RTW_PRINT_SEL(m, "0x%lx\n", pregpriv->go_hidden_ssid_mode);

	return 0;
}

static ssize_t proc_set_go_hidden_ssid_mode(struct file *file, const char __user *buffer, size_t count, loff_t *pos, void *data)
{

	struct net_device *dev = data;
	_adapter *padapter = (_adapter *)rtw_netdev_priv(dev);
	struct registry_priv	*pregpriv = &padapter->registrypriv;
	struct mlme_ext_priv *pmlmeext = &padapter->mlmeextpriv;
	char tmp[32];
	int go_hidden_ssid_mode, feature_bit;

	if (!padapter)
		return -EFAULT;

	if (count < 1) {
		RTW_INFO("argument size is less than 1\n");
		return -EFAULT;
	}

	if (count > sizeof(tmp)) {
		rtw_warn_on(1);
		return -EFAULT;
	}

	if (buffer && !copy_from_user(tmp, buffer, count)) {

		int num = sscanf(tmp, "%d %d",&feature_bit,  &go_hidden_ssid_mode);

		if (num < 1)
			return -EINVAL;

		/* Cancel hide_ssid_timer and reset flag when unhide SSID by proc */
		if (!feature_bit && !go_hidden_ssid_mode) {
			ATOMIC_SET(&pregpriv->set_hide_ssid_timer, 0);
			_cancel_timer_ex(&pmlmeext->hide_ssid_timer);
		}
		if (go_hidden_ssid_mode == 1)
			rtw_set_bit(feature_bit, &pregpriv->go_hidden_ssid_mode);
		else if (!go_hidden_ssid_mode)
			rtw_clear_bit(feature_bit, &pregpriv->go_hidden_ssid_mode);
		else
			return -EFAULT;

		RTW_INFO("go_hidden_ssid_mode:0x%lx\n", pregpriv->go_hidden_ssid_mode);
	} else
		return -EFAULT;

	issue_beacon(padapter, 0);

	return count;
}

#endif

static int proc_get_amsdu_mode(struct seq_file *m, void *v)
{
	struct net_device *dev = m->private;
	_adapter *padapter = (_adapter *)rtw_netdev_priv(dev);
	struct registry_priv	*pregpriv = &padapter->registrypriv;

	if (pregpriv) {
		if (pregpriv->amsdu_mode == RTW_AMSDU_MODE_NON_SPP)
			RTW_PRINT_SEL(m, "amsdu mode: NON-SPP\n");
		else if (pregpriv->amsdu_mode == RTW_AMSDU_MODE_SPP)
			RTW_PRINT_SEL(m, "amsdu mode: SPP\n");
		else if (pregpriv->amsdu_mode == RTW_AMSDU_MODE_ALL_DROP)
			RTW_PRINT_SEL(m, "amsdu mode: ALL DROP\n");
		else
			RTW_PRINT_SEL(m, "unexpected amsdu mode\n");
	}

	return 0;
}

static ssize_t proc_set_amsdu_mode(struct file *file, const char __user *buffer, size_t count, loff_t *pos, void *data)
{
	struct net_device *dev = data;
	_adapter *padapter = (_adapter *)rtw_netdev_priv(dev);
	struct registry_priv	*pregpriv = &padapter->registrypriv;
	char tmp[32];
	u32 mode;
	u8 bw_2g;
	u8 bw_5g;

	if (count < 1)
		return -EFAULT;

	if (count > sizeof(tmp)) {
		rtw_warn_on(1);
		return -EFAULT;
	}

	if (buffer && !copy_from_user(tmp, buffer, count)) {

		int num = sscanf(tmp, "%d", &mode);

		if (mode == RTW_AMSDU_MODE_NON_SPP
			|| mode == RTW_AMSDU_MODE_SPP
			|| mode == RTW_AMSDU_MODE_ALL_DROP) {
			pregpriv->amsdu_mode = mode;
			RTW_INFO("amsdu mode=%u\n", mode);
		} else {
			RTW_INFO("set unexpected mode = %d, won't apply\n", mode);
		}
	}

	return count;

}

#ifdef PRIVATE_R
static int proc_get_vendor_ie_filter(struct seq_file *m, void *v)
{
	struct net_device *dev = m->private;
	_adapter *adapter = (_adapter *)rtw_netdev_priv(dev);
	struct mlme_priv *mlmepriv = &adapter->mlmepriv;
	int i, j;

	if (!mlmepriv)
		return -EFAULT;

	RTW_PRINT_SEL(m, "enable: %u\n", mlmepriv->vendor_ie_filter_enable);
	RTW_PRINT_SEL(m, "index\tcontent\n");

	for (i = 0; i < MAX_VENDOR_IE_NUM; i++) {
		RTW_PRINT_SEL(m, "%d\t", i);
		if (mlmepriv->vendor_ie_len[i])
			for (j = 0; j < mlmepriv->vendor_ie_len[i] + 2; j++)
				RTW_PRINT_SEL(m, "%02x", mlmepriv->vendor_ie_filter[i][j]);
		RTW_PRINT_SEL(m, "\n");
	}

	return 0;
}

static ssize_t proc_set_vendor_ie_filter(struct file *file, const char __user *buffer, size_t count, loff_t *pos, void *data)
{
	struct net_device *dev = data;
	_adapter *adapter = (_adapter *)rtw_netdev_priv(dev);
	struct mlme_priv *mlmepriv = &adapter->mlmepriv;
	char tmp[MAX_VENDOR_IE_PARAM_LEN];
	u8 vendor_ie_num = 0;
	u8 *vendor_ie;
	u8 enable;
	int num, i, len = 0;

	vendor_ie = rtw_malloc(MAX_VENDOR_IE_LEN);

	if (!vendor_ie || !adapter)
		return -EFAULT;

	if (count < 1) {
		RTW_INFO("argument size is less than 1\n");
		return -EFAULT;
	}

	if (count > sizeof(tmp)) {
		rtw_warn_on(1);
		return -EFAULT;
	}

	if (buffer && !copy_from_user(tmp, buffer, count)) {
		if (count > 2) {
			num = sscanf(tmp, "%hhu %s", &vendor_ie_num, vendor_ie);

			if (num < 1)
				return -EINVAL;

			if (vendor_ie_num > MAX_VENDOR_IE_NUM - 1)
				return -EINVAL;

			if (_rtw_memcmp(vendor_ie, "0", 1)) {
				_rtw_memset(mlmepriv->vendor_ie_filter[vendor_ie_num] , 0 , MAX_VENDOR_IE_LEN);
				mlmepriv->vendor_ie_len[vendor_ie_num] = 0;
			}

			for (i = 0; i < strlen(vendor_ie); i += 2) {
				mlmepriv->vendor_ie_filter[vendor_ie_num][len] = key_2char2num(vendor_ie[i], vendor_ie[i+1]);
				if (len == 1)
					mlmepriv->vendor_ie_len[vendor_ie_num] = mlmepriv->vendor_ie_filter[vendor_ie_num][len];
				len++;
			}
			rtw_mfree(vendor_ie, MAX_VENDOR_IE_LEN);
		} else if (count == 2) {
			num = sscanf(tmp, "%hhu", &mlmepriv->vendor_ie_filter_enable);
		} else {
			RTW_INFO("argument size is wrong\n");
			return -EINVAL;
		}
	} else {
		return -EFAULT;
	}

	return count;
}
#endif

#ifdef PRIVATE_R
static ssize_t proc_set_enable_dfs(struct file *file, const char __user *buffer, size_t count, loff_t *pos, void *data)
{
	struct net_device *dev = data;
	_adapter *adapter = rtw_netdev_priv(dev);
	char *alpha2;
	char tmp[32];
	u8 enable_dfs = 0;

	if (count < 1) {
		RTW_INFO("argument size is less than 1\n");
		return -EFAULT;
	}

	if (count > sizeof(tmp)) {
		rtw_warn_on(1);
		return -EFAULT;
	}

	if (buffer && !copy_from_user(tmp, buffer, count)) {
		int num = sscanf(tmp, "%hhx", &enable_dfs);

		if (num >= 1) {
			struct get_chplan_resp *chplan;

			if (rtw_get_chplan_cmd(adapter, RTW_CMDF_WAIT_ACK, &chplan) == _SUCCESS) {
				if (!!enable_dfs != !(chplan->confs.dis_ch_flags & RTW_CHF_DFS)) {
					struct chplan_confs confs = {.set_types = BIT(CHPLAN_CONFS_DIS_CH_FLAGS)};

					if (enable_dfs)
						confs.dis_ch_flags = chplan->confs.dis_ch_flags & ~RTW_CHF_DFS;
					else
						confs.dis_ch_flags = chplan->confs.dis_ch_flags | RTW_CHF_DFS;

					rtw_set_chplan_confs_cmd(adapter, RTW_CMDF_WAIT_ACK, &confs);
				}
				rtw_free_get_chplan_resp(chplan);
			}
		}
	}

	return count;
}

static int proc_get_enable_dfs(struct seq_file *m, void *v)
{
	struct net_device *dev = m->private;
	_adapter *adapter = rtw_netdev_priv(dev);
	struct get_chplan_resp *chplan;

	if (rtw_get_chplan_cmd(adapter, RTW_CMDF_WAIT_ACK, &chplan) == _FAIL)
		goto exit;

	RTW_PRINT_SEL(m, "dfs enabled: %d\n", !(chplan->confs.dis_ch_flags & RTW_CHF_DFS));

	rtw_free_get_chplan_resp(chplan);

exit:
	return 0;
}
#endif


/*
* rtw_adapter_proc:
* init/deinit when register/unregister net_device
*/
const struct rtw_proc_hdl adapter_proc_hdls[] = {
#if RTW_SEQ_FILE_TEST
	RTW_PROC_HDL_SEQ("seq_file_test", &seq_file_test, NULL),
#endif
	RTW_PROC_HDL_SSEQ("write_reg", NULL, proc_set_write_reg),
	RTW_PROC_HDL_SSEQ("read_reg", proc_get_read_reg, proc_set_read_reg),

	RTW_PROC_HDL_SSEQ("mac_dbg_status_dump", NULL, proc_set_mac_dbg_status_dump),

	RTW_PROC_HDL_SSEQ("tx_rate_bmp", proc_get_dump_tx_rate_bmp, NULL),
	RTW_PROC_HDL_SSEQ("adapters_status", proc_get_dump_adapters_status, NULL),
#ifdef CONFIG_RTW_CUSTOMER_STR
	RTW_PROC_HDL_SSEQ("customer_str", proc_get_customer_str, NULL),
#endif
	RTW_PROC_HDL_SSEQ("fwstate", proc_get_fwstate, NULL),
	RTW_PROC_HDL_SSEQ("sec_info", proc_get_sec_info, NULL),
	RTW_PROC_HDL_SSEQ("mlmext_state", proc_get_mlmext_state, NULL),
	RTW_PROC_HDL_SSEQ("qos_option", proc_get_qos_option, NULL),
	RTW_PROC_HDL_SSEQ("ht_option", proc_get_ht_option, NULL),
	RTW_PROC_HDL_SSEQ("rf_info", proc_get_rf_info, NULL),
	RTW_PROC_HDL_SSEQ("scan_param", proc_get_scan_param, proc_set_scan_param),
	RTW_PROC_HDL_SSEQ("scan_abort", proc_get_scan_abort, proc_set_scan_abort),
#ifdef CONFIG_SCAN_BACKOP
	RTW_PROC_HDL_SSEQ("backop_flags_sta", proc_get_backop_flags_sta, proc_set_backop_flags_sta),
	#ifdef CONFIG_AP_MODE
	RTW_PROC_HDL_SSEQ("backop_flags_ap", proc_get_backop_flags_ap, proc_set_backop_flags_ap),
	#endif
	#ifdef CONFIG_RTW_MESH
	RTW_PROC_HDL_SSEQ("backop_flags_mesh", proc_get_backop_flags_mesh, proc_set_backop_flags_mesh),
	#endif
#endif
	RTW_PROC_HDL_SSEQ("survey_info", proc_get_survey_info, proc_set_survey_info),
	RTW_PROC_HDL_SSEQ("ap_info", proc_get_ap_info, NULL),
#ifdef PRIVATE_R
	RTW_PROC_HDL_SSEQ("infra_ap", proc_get_infra_ap, NULL),
#endif /* PRIVATE_R */
	RTW_PROC_HDL_SSEQ("trx_info", proc_get_trx_info, proc_reset_trx_info),
	RTW_PROC_HDL_SSEQ("tx_power_offset", proc_get_tx_power_offset, proc_set_tx_power_offset),
	RTW_PROC_HDL_SSEQ("rate_ctl", proc_get_rate_ctl, proc_set_rate_ctl),
	RTW_PROC_HDL_SSEQ("bw_ctl", proc_get_bw_ctl, proc_set_bw_ctl),
	RTW_PROC_HDL_SSEQ("mac_qinfo", proc_get_mac_qinfo, NULL),
	/*RTW_PROC_HDL_SSEQ("macid_info", proc_get_macid_info, NULL), */
	/* RTW_PROC_HDL_SSEQ("bcmc_info", proc_get_mi_ap_bc_info, NULL), */
	RTW_PROC_HDL_SSEQ("sec_cam", proc_get_sec_cam, proc_set_sec_cam),
	RTW_PROC_HDL_SSEQ("sec_cam_cache", proc_get_sec_cam_cache, NULL),
#ifdef CONFIG_DBG_AX_CAM
	RTW_PROC_HDL_SSEQ("dump_ax_valid_key", proc_get_ax_valid_key, NULL),
	RTW_PROC_HDL_SSEQ("dump_ax_address_cam", proc_get_ax_address_cam, NULL),
	RTW_PROC_HDL_SSEQ("dump_ax_security_cam", proc_get_ax_sec_cam, NULL),
#endif

	RTW_PROC_HDL_SSEQ("ps_dbg_info", proc_get_ps_dbg_info, proc_set_ps_dbg_info),
	RTW_PROC_HDL_SSEQ("wifi_spec", proc_get_wifi_spec, NULL),
#ifdef CONFIG_LAYER2_ROAMING
	RTW_PROC_HDL_SSEQ("roam_flags", proc_get_roam_flags, proc_set_roam_flags),
	RTW_PROC_HDL_SSEQ("roam_param", proc_get_roam_param, proc_set_roam_param),
	RTW_PROC_HDL_SSEQ("roam_tgt_addr", NULL, proc_set_roam_tgt_addr),
#endif /* CONFIG_LAYER2_ROAMING */
#ifdef CONFIG_RTW_MBO
	RTW_PROC_HDL_SSEQ("non_pref_ch", rtw_mbo_proc_non_pref_chans_get, rtw_mbo_proc_non_pref_chans_set),
	RTW_PROC_HDL_SSEQ("cell_data", rtw_mbo_proc_cell_data_get, rtw_mbo_proc_cell_data_set),
	RTW_PROC_HDL_SSEQ("mbo_attr", rtw_mbo_proc_attr_get, rtw_mbo_proc_attr_set),
#endif
#ifdef CONFIG_RTW_80211R
	RTW_PROC_HDL_SSEQ("ft_flags", rtw_ft_proc_flags_get, rtw_ft_proc_flags_set),
#endif
	RTW_PROC_HDL_SSEQ("defs_param", proc_get_defs_param, proc_set_defs_param),
#ifdef CONFIG_SDIO_HCI
	RTW_PROC_HDL_SSEQ("sd_f0_reg_dump", proc_get_sd_f0_reg_dump, NULL),
	RTW_PROC_HDL_SSEQ("sdio_local_reg_dump", proc_get_sdio_local_reg_dump, NULL),
	RTW_PROC_HDL_SSEQ("sdio_card_info", proc_get_sdio_card_info, NULL),
#ifdef DBG_SDIO
	RTW_PROC_HDL_SSEQ("sdio_dbg", proc_get_sdio_dbg, proc_set_sdio_dbg),
#endif /* DBG_SDIO */
#endif /* CONFIG_SDIO_HCI */

	RTW_PROC_HDL_SSEQ("del_rx_ampdu_test_case", NULL, proc_set_del_rx_ampdu_test_case),
	RTW_PROC_HDL_SSEQ("wait_hiq_empty", NULL, proc_set_wait_hiq_empty),
	RTW_PROC_HDL_SSEQ("sta_linking_test", NULL, proc_set_sta_linking_test),
#ifdef CONFIG_AP_MODE
	RTW_PROC_HDL_SSEQ("ap_linking_test", NULL, proc_set_ap_linking_test),
#endif

	RTW_PROC_HDL_SSEQ("mac_reg_dump", proc_get_mac_reg_dump, NULL),
	RTW_PROC_HDL_SSEQ("bb_reg_dump", proc_get_bb_reg_dump, NULL),
	RTW_PROC_HDL_SSEQ("bb_reg_dump_ex", proc_get_bb_reg_dump_ex, NULL),
	RTW_PROC_HDL_SSEQ("rf_reg_dump", proc_get_rf_reg_dump, NULL),

#if 0
	RTW_PROC_HDL_SSEQ("led_config", proc_get_led_config, proc_set_led_config),
#endif

#ifdef CONFIG_POWER_SAVE
	RTW_PROC_HDL_SSEQ("ps_info", NULL, proc_set_ps_info),
#endif /* CONFIG_POWER_SAVE */

#ifdef CONFIG_AP_MODE
	RTW_PROC_HDL_SSEQ("aid_status", proc_get_aid_status, proc_set_aid_status),
	RTW_PROC_HDL_SSEQ("sta_active_time", proc_get_sta_active_time, NULL),
	RTW_PROC_HDL_SSEQ("all_sta_info", proc_get_all_sta_info, NULL),
	RTW_PROC_HDL_SSEQ("bmc_tx_rate", proc_get_bmc_tx_rate, proc_set_bmc_tx_rate),
	#if CONFIG_RTW_AP_DATA_BMC_TO_UC
	RTW_PROC_HDL_SSEQ("ap_b2u_flags", proc_get_ap_b2u_flags, proc_set_ap_b2u_flags),
	#endif
#endif /* CONFIG_AP_MODE */

#ifdef DBG_MEMORY_LEAK
	RTW_PROC_HDL_SSEQ("_malloc_cnt", proc_get_malloc_cnt, NULL),
#endif /* DBG_MEMORY_LEAK */

#ifdef CONFIG_FIND_BEST_CHANNEL
	RTW_PROC_HDL_SSEQ("best_channel", proc_get_best_channel, proc_set_best_channel),
#endif

	RTW_PROC_HDL_SSEQ("rx_signal", proc_get_rx_signal, proc_set_rx_signal),
	RTW_PROC_HDL_SSEQ("rx_chk_limit", proc_get_rx_chk_limit, proc_set_rx_chk_limit),
	RTW_PROC_HDL_SSEQ("hw_info", proc_get_hw_status, proc_set_hw_status),
	RTW_PROC_HDL_SSEQ("mac_rptbuf", proc_get_mac_rptbuf, NULL),
#ifdef CONFIG_80211N_HT
	RTW_PROC_HDL_SSEQ("ht_enable", proc_get_ht_enable, proc_set_ht_enable),
	RTW_PROC_HDL_SSEQ("bw_mode", proc_get_bw_mode, proc_set_bw_mode),
	RTW_PROC_HDL_SSEQ("ampdu_enable", proc_get_ampdu_enable, proc_set_ampdu_enable),
	RTW_PROC_HDL_SSEQ("rx_ampdu", proc_get_rx_ampdu, proc_set_rx_ampdu),
	RTW_PROC_HDL_SSEQ("rx_ampdu_size_limit", proc_get_rx_ampdu_size_limit, proc_set_rx_ampdu_size_limit),
	RTW_PROC_HDL_SSEQ("rx_ampdu_factor", proc_get_rx_ampdu_factor, proc_set_rx_ampdu_factor),
	RTW_PROC_HDL_SSEQ("rx_ampdu_density", proc_get_rx_ampdu_density, proc_set_rx_ampdu_density),
	RTW_PROC_HDL_SSEQ("tx_ampdu_density", proc_get_tx_ampdu_density, proc_set_tx_ampdu_density),
	RTW_PROC_HDL_SSEQ("tx_ampdu_num", proc_get_tx_ampdu_num, proc_set_tx_ampdu_num),
	RTW_PROC_HDL_SSEQ("tx_quick_addba_req", proc_get_tx_quick_addba_req, proc_set_tx_quick_addba_req),
#ifdef CONFIG_TX_AMSDU
	RTW_PROC_HDL_SSEQ("tx_amsdu", proc_get_tx_amsdu, proc_set_tx_amsdu),
	RTW_PROC_HDL_SSEQ("tx_amsdu_rate", proc_get_tx_amsdu_rate, proc_set_tx_amsdu_rate),
#endif
#endif /* CONFIG_80211N_HT */

#ifdef CONFIG_80211AC_VHT
	RTW_PROC_HDL_SSEQ("vht_24g_enable", proc_get_vht_24g_enable, proc_set_vht_24g_enable),
#endif /* CONFIG_80211AC_VHT */

	RTW_PROC_HDL_SSEQ("dynamic_rrsr", proc_get_dyn_rrsr, proc_set_dyn_rrsr),

	/* RTW_PROC_HDL_SSEQ("path_rssi", proc_get_two_path_rssi, NULL),
	* 	RTW_PROC_HDL_SSEQ("rssi_disp",proc_get_rssi_disp, proc_set_rssi_disp), */

#if defined(DBG_CONFIG_ERROR_DETECT)
	RTW_PROC_HDL_SSEQ("sreset", proc_get_sreset, proc_set_sreset),
#endif /* DBG_CONFIG_ERROR_DETECT */
	RTW_PROC_HDL_SSEQ("trx_info_debug", proc_get_trx_info_debug, NULL),

#ifdef CONFIG_HUAWEI_PROC
	RTW_PROC_HDL_SSEQ("huawei_trx_info", proc_get_huawei_trx_info, NULL),
#endif
	RTW_PROC_HDL_SSEQ("rtw_hal_linked_info_dump", proc_get_linked_info_dump, proc_set_linked_info_dump),
	RTW_PROC_HDL_SSEQ("sta_tp_dump", proc_get_sta_tp_dump, proc_set_sta_tp_dump),
	RTW_PROC_HDL_SSEQ("sta_tp_info", proc_get_sta_tp_info, NULL),
	RTW_PROC_HDL_SSEQ("dis_turboedca", proc_get_turboedca_ctrl, proc_set_turboedca_ctrl),
	RTW_PROC_HDL_SSEQ("tx_info_msg", proc_get_tx_info_msg, NULL),
	RTW_PROC_HDL_SSEQ("rx_info_msg", proc_get_rx_info_msg, proc_set_rx_info_msg),
	RTW_PROC_HDL_SSEQ("false_alarm_accumulated",
			  proc_get_false_alarm_accumulated,
			  proc_set_false_alarm_accumulated),
#ifdef PRIVATE_R
	RTW_PROC_HDL_SSEQ("roku_trx_info_msg", proc_get_roku_trx_info_msg, NULL),
#endif

#ifdef CONFIG_GPIO_API
	RTW_PROC_HDL_SSEQ("gpio_info", proc_get_gpio, proc_set_gpio),
	RTW_PROC_HDL_SSEQ("gpio_set_output_value", NULL, proc_set_gpio_output_value),
	RTW_PROC_HDL_SSEQ("gpio_set_direction", NULL, proc_set_config_gpio),
#endif

#ifdef CONFIG_DBG_COUNTER
	RTW_PROC_HDL_SSEQ("rx_logs", proc_get_rx_logs, NULL),
	RTW_PROC_HDL_SSEQ("tx_logs", proc_get_tx_logs, NULL),
	RTW_PROC_HDL_SSEQ("int_logs", proc_get_int_logs, NULL),
#endif

#ifdef CONFIG_DBG_RF_CAL
	RTW_PROC_HDL_SSEQ("iqk", proc_get_iqk_info, proc_set_iqk),
	RTW_PROC_HDL_SSEQ("lck", proc_get_lck_info, proc_set_lck),
#endif

#ifdef CONFIG_PCI_HCI
	RTW_PROC_HDL_SSEQ("rx_ring", proc_get_rx_ring, NULL),
	RTW_PROC_HDL_SSEQ("tx_ring", proc_get_tx_ring, NULL),
#ifdef DBG_TXBD_DESC_DUMP
	RTW_PROC_HDL_SSEQ("tx_ring_ext", proc_get_tx_ring_ext, proc_set_tx_ring_ext),
#endif
	RTW_PROC_HDL_SSEQ("pci_aspm", proc_get_pci_aspm, NULL),

	RTW_PROC_HDL_SSEQ("pci_conf_space", proc_get_pci_conf_space, proc_set_pci_conf_space),

	RTW_PROC_HDL_SSEQ("pci_bridge_conf_space", proc_get_pci_bridge_conf_space, proc_set_pci_bridge_conf_space),

#endif

#ifdef CONFIG_WOWLAN
	RTW_PROC_HDL_SSEQ("wow_enable", proc_get_wow_enable, proc_set_wow_enable),
	RTW_PROC_HDL_SSEQ("wow_pattern_info", proc_get_pattern_info, proc_set_pattern_info),
	RTW_PROC_HDL_SSEQ("wow_wakeup_event", proc_get_wakeup_event,
			  proc_set_wakeup_event),
	RTW_PROC_HDL_SSEQ("wowlan_last_wake_reason", proc_get_wakeup_reason, NULL),
	RTW_PROC_HDL_SSEQ("wow_pattern_cam", proc_dump_pattern_cam, NULL),
	RTW_PROC_HDL_SSEQ("wow_wake_indication", proc_get_wake_indication, NULL),
#ifdef CONFIG_GPIO_WAKEUP
	RTW_PROC_HDL_SSEQ("wowlan_gpio_info", proc_get_wowlan_gpio_info, proc_set_wowlan_gpio_info),
	RTW_PROC_HDL_SSEQ("wow_gpio_duration", proc_get_wow_gpio_duration,
			proc_set_wow_gpio_duration),
#endif
#endif

#ifdef CONFIG_P2P_WOWLAN
	RTW_PROC_HDL_SSEQ("p2p_wowlan_info", proc_get_p2p_wowlan_info, NULL),
#endif
	RTW_PROC_HDL_SSEQ("country_code", proc_get_country_code, proc_set_country_code),
	RTW_PROC_HDL_SSEQ("chan_plan", proc_get_chan_plan, proc_set_chan_plan),
	RTW_PROC_HDL_SSEQ("chplan_confs", proc_get_chplan_confs, proc_set_chplan_confs),
	RTW_PROC_HDL_SSEQ("cap_spt_op_class_ch", proc_get_cap_spt_op_class_ch, proc_set_cap_spt_op_class_ch),
	RTW_PROC_HDL_SSEQ("reg_spt_op_class_ch", proc_get_reg_spt_op_class_ch, proc_set_reg_spt_op_class_ch),
	RTW_PROC_HDL_SSEQ("cur_spt_op_class_ch", proc_get_cur_spt_op_class_ch, proc_set_cur_spt_op_class_ch),
#if CONFIG_RTW_MACADDR_ACL
	RTW_PROC_HDL_SSEQ("macaddr_acl", proc_get_macaddr_acl, proc_set_macaddr_acl),
#endif
#if CONFIG_RTW_PRE_LINK_STA
	RTW_PROC_HDL_SSEQ("pre_link_sta", proc_get_pre_link_sta, proc_set_pre_link_sta),
#endif
	RTW_PROC_HDL_SSEQ("ch_sel_policy", proc_get_ch_sel_policy, proc_set_ch_sel_policy),
#ifdef CONFIG_80211D
	RTW_PROC_HDL_SSEQ("country_ie_slave_records", proc_get_country_ie_slave_records, NULL),
#endif
#ifdef CONFIG_DFS_MASTER
	RTW_PROC_HDL_SSEQ("dfs_test_case", proc_get_dfs_test_case, proc_set_dfs_test_case),
	RTW_PROC_HDL_SSEQ("update_non_ocp", NULL, proc_set_update_non_ocp),
	RTW_PROC_HDL_SSEQ("radar_detect", NULL, proc_set_radar_detect),
	RTW_PROC_HDL_SSEQ("dfs_ch_sel_e_flags", proc_get_dfs_ch_sel_e_flags, proc_set_dfs_ch_sel_e_flags),
	RTW_PROC_HDL_SSEQ("dfs_ch_sel_d_flags", proc_get_dfs_ch_sel_d_flags, proc_set_dfs_ch_sel_d_flags),
	#if CONFIG_DFS_SLAVE_WITH_RADAR_DETECT
	RTW_PROC_HDL_SSEQ("dfs_slave_with_rd", proc_get_dfs_slave_with_rd, proc_set_dfs_slave_with_rd),
	#endif
#endif
	RTW_PROC_HDL_SSEQ("sink_udpport", proc_get_udpport, proc_set_udpport),
#ifdef DBG_RX_COUNTER_DUMP
	RTW_PROC_HDL_SSEQ("dump_rx_cnt_mode", proc_get_rx_cnt_dump, proc_set_rx_cnt_dump),
#endif
	RTW_PROC_HDL_SSEQ("change_bss_chbw", NULL, proc_set_change_bss_chbw),
	RTW_PROC_HDL_SSEQ("tx_bw_mode", proc_get_tx_bw_mode, proc_set_tx_bw_mode),

#ifdef CONFIG_ECSA_PHL
	RTW_PROC_HDL_SSEQ("ecsa_allow_reason", proc_get_ecsa_allow_reason, proc_set_ecsa_allow_reason),
#endif

	RTW_PROC_HDL_SSEQ("target_tx_power", proc_get_target_tx_power, NULL),
	RTW_PROC_HDL_SSEQ("tx_power_by_rate", proc_get_tx_power_by_rate, NULL),
#if CONFIG_TXPWR_LIMIT
	RTW_PROC_HDL_SSEQ("tx_power_limit", proc_get_tx_power_limit, NULL),
#endif
	RTW_PROC_HDL_SSEQ("tpc_settings", proc_get_tpc_settings, proc_set_tpc_settings),
	RTW_PROC_HDL_SSEQ("tx_power_ext_info", proc_get_tx_power_ext_info, proc_set_tx_power_ext_info),
#ifdef GEORGIA_TODO_TX_PWR
	RTW_PROC_HDL_SEQ("tx_power_idx", &seq_ops_tx_power_idx, NULL),
	RTW_PROC_HDL_SEQ("txpwr_total_dbm", &seq_ops_txpwr_total_dbm, NULL),
#endif
#ifdef CONFIG_TDLS
	RTW_PROC_HDL_SSEQ("tdls_info", proc_get_tdls_info, NULL),
	RTW_PROC_HDL_SSEQ("tdls_enable", proc_get_tdls_enable, proc_set_tdls_enable),
#endif
	RTW_PROC_HDL_SSEQ("monitor", proc_get_monitor, proc_set_monitor),

#ifdef CONFIG_RTW_ACS
	RTW_PROC_HDL_SSEQ("chan_info", proc_get_chan_info, NULL),
#ifdef WKARD_ACS
	RTW_PROC_HDL_SSEQ("acs", proc_get_best_chan, proc_set_acs),
#endif
#endif
	RTW_PROC_HDL_SSEQ("env_info", proc_get_env_rpt, NULL),
	RTW_PROC_HDL_SSEQ("hal_spec", proc_get_hal_spec, NULL),
	RTW_PROC_HDL_SSEQ("hal_trx_mode", proc_get_hal_trx_mode, NULL),
	RTW_PROC_HDL_SSEQ("hal_txpwr_info", proc_get_hal_txpwr_info, NULL),

	RTW_PROC_HDL_SSEQ("efuse_map", proc_get_efuse_map, NULL),
#ifdef CONFIG_IEEE80211W
	RTW_PROC_HDL_SSEQ("11w_tx_sa_query", proc_get_tx_sa_query, proc_set_tx_sa_query),
	RTW_PROC_HDL_SSEQ("11w_tx_deauth", proc_get_tx_deauth, proc_set_tx_deauth),
	RTW_PROC_HDL_SSEQ("11w_tx_auth", proc_get_tx_auth, proc_set_tx_auth),
#endif /* CONFIG_IEEE80211W */

	RTW_PROC_HDL_SSEQ("mac_addr", proc_get_mac_addr, NULL),
	RTW_PROC_HDL_SSEQ("skip_band", proc_get_skip_band, proc_set_skip_band),

	RTW_PROC_HDL_SSEQ("rx_stat", proc_get_rx_stat, NULL),

	RTW_PROC_HDL_SSEQ("tx_stat", proc_get_tx_stat, NULL),
	RTW_PROC_HDL_SSEQ("sta_tx_stat", proc_get_sta_tx_stat, proc_set_sta_tx_stat),
	/**** PHY Capability ****/
	RTW_PROC_HDL_SSEQ("phy_cap", proc_get_phy_cap, NULL),
#ifdef CONFIG_80211N_HT
	RTW_PROC_HDL_SSEQ("stbc_cap", proc_get_stbc_cap, proc_set_stbc_cap),
	RTW_PROC_HDL_SSEQ("ldpc_cap", proc_get_ldpc_cap, proc_set_ldpc_cap),
#endif /* CONFIG_80211N_HT */
#ifdef CONFIG_BEAMFORMING
	RTW_PROC_HDL_SSEQ("txbf_cap", proc_get_txbf_cap, proc_set_txbf_cap),
#endif

	RTW_PROC_HDL_SSEQ("napi_info", proc_get_napi_info, NULL),
#ifdef CONFIG_RTW_NAPI_DYNAMIC
	RTW_PROC_HDL_SSEQ("napi_th", proc_get_napi_info, proc_set_napi_th),
#endif /* CONFIG_RTW_NAPI_DYNAMIC */

	RTW_PROC_HDL_SSEQ("rsvd_page", proc_dump_rsvd_page, proc_set_rsvd_page_info),

#ifdef CONFIG_SUPPORT_FIFO_DUMP
	RTW_PROC_HDL_SSEQ("fifo_dump", proc_dump_fifo, proc_set_fifo_info),
#endif

#ifdef CONFIG_IGNORE_GO_AND_LOW_RSSI_IN_SCAN_LIST
	RTW_PROC_HDL_SSEQ("ignore_go_and_low_rssi_in_scan",
		proc_get_ignore_go_and_low_rssi_in_scan,
		proc_set_ignore_go_and_low_rssi_in_scan),
#endif /*CONFIG_IGNORE_GO_AND_LOW_RSSI_IN_SCAN_LIST*/

#ifdef DBG_XMIT_BLOCK
	RTW_PROC_HDL_SSEQ("xmit_block", proc_get_xmit_block, proc_set_xmit_block),
#endif

	RTW_PROC_HDL_SSEQ("ack_timeout", proc_get_ack_timeout, proc_set_ack_timeout),

	RTW_PROC_HDL_SSEQ("dynamic_agg_enable", proc_get_dynamic_agg_enable, proc_set_dynamic_agg_enable),

	RTW_PROC_HDL_SSEQ("fw_info", proc_get_fw_info, NULL),
	RTW_PROC_HDL_SSEQ("fw_offload", proc_get_fw_offload, proc_set_fw_offload),

#ifdef CONFIG_RTW_WDS
	RTW_PROC_HDL_SSEQ("wds_en", proc_get_wds_en, proc_set_wds_en),
	RTW_PROC_HDL_SSEQ("sta_wds_en", NULL, proc_set_sta_wds_en),
	RTW_PROC_HDL_SSEQ("wds_gptr", proc_get_wds_gptr, NULL),
	#ifdef CONFIG_AP_MODE
	RTW_PROC_HDL_SSEQ("wds_path", proc_get_wds_path, NULL),
	#endif
#endif

#ifdef CONFIG_RTW_MULTI_AP
	RTW_PROC_HDL_SSEQ("multi_ap_opmode", proc_get_multi_ap_opmode, proc_set_multi_ap_opmode),
	RTW_PROC_HDL_SSEQ("unassoc_sta", proc_get_unassoc_sta, proc_set_unassoc_sta),
#ifdef CONFIG_IOCTL_CFG80211
	RTW_PROC_HDL_SSEQ("sta_assoc_req_frame_body", proc_get_sta_assoc_req_frame_body, proc_set_sta_assoc_req_frame_body),
#endif
	RTW_PROC_HDL_SSEQ("ch_util_threshold", proc_get_ch_util_threshold, proc_set_ch_util_threshold),
	RTW_PROC_HDL_SSEQ("ch_utilization", proc_get_ch_utilization, NULL),
#endif

#ifdef CONFIG_RTW_MESH
	#if CONFIG_RTW_MESH_ACNODE_PREVENT
	RTW_PROC_HDL_SSEQ("mesh_acnode_prevent", proc_get_mesh_acnode_prevent, proc_set_mesh_acnode_prevent),
	#endif
	#if CONFIG_RTW_MESH_OFFCH_CAND
	RTW_PROC_HDL_SSEQ("mesh_offch_cand", proc_get_mesh_offch_cand, proc_set_mesh_offch_cand),
	#endif
	#if CONFIG_RTW_MESH_PEER_BLACKLIST
	RTW_PROC_HDL_SSEQ("mesh_peer_blacklist", proc_get_mesh_peer_blacklist, proc_set_mesh_peer_blacklist),
	#endif
	#if CONFIG_RTW_MESH_CTO_MGATE_BLACKLIST
	RTW_PROC_HDL_SSEQ("mesh_cto_mgate_require", proc_get_mesh_cto_mgate_require, proc_set_mesh_cto_mgate_require),
	RTW_PROC_HDL_SSEQ("mesh_cto_mgate_blacklist", proc_get_mesh_cto_mgate_blacklist, proc_set_mesh_cto_mgate_blacklist),
	#endif
	RTW_PROC_HDL_SSEQ("mesh_peer_sel_policy", proc_get_mesh_peer_sel_policy, NULL),
	RTW_PROC_HDL_SSEQ("mesh_networks", proc_get_mesh_networks, NULL),
	RTW_PROC_HDL_SSEQ("mesh_plink_ctl", proc_get_mesh_plink_ctl, NULL),
	RTW_PROC_HDL_SSEQ("mesh_mpath", proc_get_mesh_mpath, NULL),
	RTW_PROC_HDL_SSEQ("mesh_mpp", proc_get_mesh_mpp, NULL),
	RTW_PROC_HDL_SSEQ("mesh_known_gates", proc_get_mesh_known_gates, NULL),
	#if CONFIG_RTW_MESH_DATA_BMC_TO_UC
	RTW_PROC_HDL_SSEQ("mesh_b2u_flags", proc_get_mesh_b2u_flags, proc_set_mesh_b2u_flags),
	#endif
	RTW_PROC_HDL_SSEQ("mesh_stats", proc_get_mesh_stats, NULL),
	RTW_PROC_HDL_SSEQ("mesh_gate_timeout_factor", proc_get_mesh_gate_timeout, proc_set_mesh_gate_timeout),
	RTW_PROC_HDL_SSEQ("mesh_gate_state", proc_get_mesh_gate_state, NULL),
	RTW_PROC_HDL_SSEQ("mesh_peer_alive_based_preq", proc_get_peer_alive_based_preq, proc_set_peer_alive_based_preq),
#endif

#ifdef CONFIG_SUPPORT_STATIC_SMPS
	RTW_PROC_HDL_SSEQ("smps", proc_get_smps, proc_set_smps),
#endif

#ifdef RTW_BUSY_DENY_SCAN
	RTW_PROC_HDL_SSEQ("scan_interval_thr", proc_get_scan_interval_thr, \
			  proc_set_scan_interval_thr),
#endif
	RTW_PROC_HDL_SSEQ("scan_deny", proc_get_scan_deny, proc_set_scan_deny),

#ifdef CONFIG_CTRL_TXSS_BY_TP
	RTW_PROC_HDL_SSEQ("txss_tp", proc_get_txss_tp, proc_set_txss_tp),
	#ifdef DBG_CTRL_TXSS
	RTW_PROC_HDL_SSEQ("txss_ctrl", proc_get_txss_ctrl, proc_set_txss_ctrl),
	#endif
#endif

	RTW_PROC_HDL_SSEQ("cur_beacon_keys", proc_get_cur_beacon_keys, NULL),

	RTW_PROC_HDL_SSEQ("chan", proc_get_chan, proc_set_chan),

	RTW_PROC_HDL_SSEQ("mr_test", proc_get_mr_test, proc_set_mr_test),

	RTW_PROC_HDL_SSEQ("deny_legacy", proc_get_deny_legacy, proc_set_deny_legacy),
#if defined(PRIVATE_R) && defined(CONFIG_P2P)
	RTW_PROC_HDL_SSEQ("go_hidden_ssid_mode", proc_get_go_hidden_ssid_mode, proc_set_go_hidden_ssid_mode),
#endif
	RTW_PROC_HDL_SSEQ("rtw_amsdu_mode", proc_get_amsdu_mode, proc_set_amsdu_mode),
#ifdef CONFIG_80211AX_HE
	RTW_PROC_HDL_SSEQ("tx_ul_mu_disable", proc_get_tx_ul_mu_disable, proc_set_tx_ul_mu_disable),
#endif
#ifdef PRIVATE_R
	RTW_PROC_HDL_SSEQ("vendor_ie_filter", proc_get_vendor_ie_filter, proc_set_vendor_ie_filter),
#endif
#ifdef RTW_DETECT_HANG
	RTW_PROC_HDL_SSEQ("hang_info", proc_get_hang_info, proc_set_hang_info),
#endif
	RTW_PROC_HDL_SSEQ("disconnect_info", proc_get_disconnect_info,
			  proc_set_disconnect_info),
#ifdef PRIVATE_R
	RTW_PROC_HDL_SSEQ("enable_dfs", proc_get_enable_dfs, proc_set_enable_dfs),
#endif
};

const int adapter_proc_hdls_num = sizeof(adapter_proc_hdls) / sizeof(struct rtw_proc_hdl);

static int rtw_adapter_proc_open(struct inode *inode, struct file *file)
{
	ssize_t index = (ssize_t)PDE_DATA(inode);
	const struct rtw_proc_hdl *hdl = adapter_proc_hdls + index;
	void *private = proc_get_parent_data(inode);

	if (hdl->type == RTW_PROC_HDL_TYPE_SEQ) {
		int res = seq_open(file, hdl->u.seq_op);

		if (res == 0)
			((struct seq_file *)file->private_data)->private = private;

		return res;
	} else if (hdl->type == RTW_PROC_HDL_TYPE_SSEQ) {
		int (*show)(struct seq_file *, void *) = hdl->u.show ? hdl->u.show : proc_get_dummy;

		return single_open(file, show, private);
	} else if (hdl->type == RTW_PROC_HDL_TYPE_SZSEQ) {
		int (*show)(struct seq_file *, void *) = hdl->u.sz.show ? hdl->u.sz.show : proc_get_dummy;

		#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3, 10, 0))
		return single_open_size(file, show, private, hdl->u.sz.size);
		#else
		return single_open(file, show, private);
		#endif
	} else {
		return -EROFS;
	}
}

static ssize_t rtw_adapter_proc_write(struct file *file, const char __user *buffer, size_t count, loff_t *pos)
{
	ssize_t index = (ssize_t)PDE_DATA(file_inode(file));
	const struct rtw_proc_hdl *hdl = adapter_proc_hdls + index;
	ssize_t (*write)(struct file *, const char __user *, size_t, loff_t *, void *) = hdl->write;

	if (write)
		return write(file, buffer, count, pos, ((struct seq_file *)file->private_data)->private);

	return -EROFS;
}

static const struct rtw_proc_ops rtw_adapter_proc_seq_fops = {
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(5, 6, 0))
	.proc_open = rtw_adapter_proc_open,
	.proc_read = seq_read,
	.proc_lseek = seq_lseek,
	.proc_release = seq_release,
	.proc_write = rtw_adapter_proc_write,
#else
	.owner = THIS_MODULE,
	.open = rtw_adapter_proc_open,
	.read = seq_read,
	.llseek = seq_lseek,
	.release = seq_release,
	.write = rtw_adapter_proc_write,
#endif
};

static const struct rtw_proc_ops rtw_adapter_proc_sseq_fops = {
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(5, 6, 0))
	.proc_open = rtw_adapter_proc_open,
	.proc_read = seq_read,
	.proc_lseek = seq_lseek,
	.proc_release = seq_release,
	.proc_write = rtw_adapter_proc_write,
#else
	.owner = THIS_MODULE,
	.open = rtw_adapter_proc_open,
	.read = seq_read,
	.llseek = seq_lseek,
	.release = single_release,
	.write = rtw_adapter_proc_write,
#endif
};

int proc_get_phy_adaptivity(struct seq_file *m, void *v)
{
	struct net_device *dev = m->private;
	_adapter *padapter = (_adapter *)rtw_netdev_priv(dev);

	rtw_cfg_adaptivity_config_msg(m, padapter);
	rtw_hal_phy_adaptivity_parm_msg(m, padapter);

	return 0;
}

static char *phydm_msg = NULL;
#define PHYDM_MSG_LEN	80*24*4

static int proc_get_phydm_cmd(struct seq_file *m, void *v)
{
	struct net_device *netdev = m->private;
	_adapter *padapter = (_adapter *)rtw_netdev_priv(netdev);

	if (NULL == phydm_msg) {
		_RTW_PRINT_SEL(m, "(Nothing to output)\n");
		return 0;
#if 0
		phydm_msg = rtw_zmalloc(PHYDM_MSG_LEN);
		if (NULL == phydm_msg)
			return -ENOMEM;

		phydm_cmd(phydm, NULL, 0, 0, phydm_msg, PHYDM_MSG_LEN);
#endif
	}

	_RTW_PRINT_SEL(m, "%s\n", phydm_msg);

	rtw_mfree(phydm_msg, PHYDM_MSG_LEN);
	phydm_msg = NULL;

	return 0;
}

static int proc_get_phl_cmd(struct seq_file *m, void *v)
{
	return proc_get_phydm_cmd(m, v);
}

static ssize_t proc_set_phydm_cmd(struct file *file, char *buffer, size_t count,
				  loff_t *pos, void *data,
				  enum rtw_proc_cmd_type type)
{
	struct net_device *netdev = (struct net_device *)data;
	_adapter *padapter = (_adapter *)rtw_netdev_priv(netdev);
	struct dvobj_priv *dvobj = adapter_to_dvobj(padapter);
	struct rtw_proc_cmd cmd;

	if (count < 1)
		return -EFAULT;

	if (buffer) {
		if (NULL == phydm_msg) {
			phydm_msg = rtw_zmalloc(PHYDM_MSG_LEN);
			if (NULL == phydm_msg)
				return -ENOMEM;
		} else
			_rtw_memset(phydm_msg, 0, PHYDM_MSG_LEN);

		cmd.in_type = RTW_ARG_TYPE_BUF;
		cmd.in_cnt_len = count;
		cmd.in.buf = buffer;

		rtw_phl_proc_cmd(GET_PHL_INFO(dvobj), type, &cmd, phydm_msg, PHYDM_MSG_LEN);

		if (strlen(phydm_msg) == 0) {
			rtw_mfree(phydm_msg, PHYDM_MSG_LEN);
			phydm_msg = NULL;
		}
	}

	return count;
}

static ssize_t proc_set_phl_cmd(struct file *file, const char __user *buffer, size_t count, loff_t *pos, void *data)
{
	char *buf, *p;
	enum rtw_proc_cmd_type type;
	ssize_t ret, len;
	int type_len;

	buf = _rtw_malloc(count + 1);
	if (!buf)
		return -ENOMEM;

	if (copy_from_user(buf, buffer, count)) {
		ret = -EFAULT;
		goto err;
	}

	if (strncmp(buf, "bb ", 3) == 0) {
		type = RTW_PROC_CMD_BB;
		type_len = 3;
	} else if (strncmp(buf, "rf ", 3) == 0) {
		type = RTW_PROC_CMD_RF;
		type_len = 3;
	} else if (strncmp(buf, "mac ", 4) == 0) {
		type = RTW_PROC_CMD_MAC;
		type_len = 4;
	} else if (strncmp(buf, "phl ", 4) == 0) {
		type = RTW_PROC_CMD_PHL;
		type_len = 4;
	} else if (strncmp(buf, "core ", 5) == 0) {
		printk("%s(%d)\n",__func__,__LINE__);
		type = RTW_PROC_CMD_CORE;
		type_len = 5;
	} else if (strncmp(buf, "btc ", 4) == 0) {
		printk("%s(%d)\n",__func__,__LINE__);
		type = RTW_PROC_CMD_BTC;
		type_len = 4;
	} else if (strncmp(buf, "efuse ", 6) == 0) {
		type = RTW_PROC_CMD_EFUSE;
		type_len = 6;
	} else {
		ret = -EINVAL;
		goto err;
	}

	/* skip first type token, like 'bb ', 'rf ' */
	p = buf + type_len;
	len = count - type_len;

	/* remove trailing newline character, because BB/RF parser uses
	 * " ," as delimiter that leads the last token contains '\n' if
	 * we use 'echo' command without '-n' argument.
	 */
	if (p[len - 1] == '\n')
		len--;
	p[len++] = '\0';

	ret = proc_set_phydm_cmd(file, p, len, pos, data, type);

err:
	_rtw_mfree(buf, count + 1);

	if (ret >= 0)
		return count;

	return ret;
}

/*
* rtw_odm_proc:
* init/deinit when register/unregister net_device, along with rtw_adapter_proc
*/
const struct rtw_proc_hdl odm_proc_hdls[] = {
	RTW_PROC_HDL_SSEQ("adaptivity", proc_get_phy_adaptivity, NULL),
	RTW_PROC_HDL_SZSEQ("phl_cmd", proc_get_phl_cmd, proc_set_phl_cmd, PHYDM_MSG_LEN),
};

const int odm_proc_hdls_num = sizeof(odm_proc_hdls) / sizeof(struct rtw_proc_hdl);

static int rtw_odm_proc_open(struct inode *inode, struct file *file)
{
	ssize_t index = (ssize_t)PDE_DATA(inode);
	const struct rtw_proc_hdl *hdl = odm_proc_hdls + index;
	void *private = proc_get_parent_data(inode);

	if (hdl->type == RTW_PROC_HDL_TYPE_SEQ) {
		int res = seq_open(file, hdl->u.seq_op);

		if (res == 0)
			((struct seq_file *)file->private_data)->private = private;

		return res;
	} else if (hdl->type == RTW_PROC_HDL_TYPE_SSEQ) {
		int (*show)(struct seq_file *, void *) = hdl->u.show ? hdl->u.show : proc_get_dummy;

		return single_open(file, show, private);
	} else if (hdl->type == RTW_PROC_HDL_TYPE_SZSEQ) {
		int (*show)(struct seq_file *, void *) = hdl->u.sz.show ? hdl->u.sz.show : proc_get_dummy;

		#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3, 10, 0))
		return single_open_size(file, show, private, hdl->u.sz.size);
		#else
		return single_open(file, show, private);
		#endif
	} else {
		return -EROFS;
	}
}

static ssize_t rtw_odm_proc_write(struct file *file, const char __user *buffer, size_t count, loff_t *pos)
{
	ssize_t index = (ssize_t)PDE_DATA(file_inode(file));
	const struct rtw_proc_hdl *hdl = odm_proc_hdls + index;
	ssize_t (*write)(struct file *, const char __user *, size_t, loff_t *, void *) = hdl->write;

	if (write)
		return write(file, buffer, count, pos, ((struct seq_file *)file->private_data)->private);

	return -EROFS;
}

static const struct rtw_proc_ops rtw_odm_proc_seq_fops = {
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(5, 6, 0))
	.proc_open = rtw_odm_proc_open,
	.proc_read = seq_read,
	.proc_lseek = seq_lseek,
	.proc_release = seq_release,
	.proc_write = rtw_odm_proc_write,
#else
	.owner = THIS_MODULE,
	.open = rtw_odm_proc_open,
	.read = seq_read,
	.llseek = seq_lseek,
	.release = seq_release,
	.write = rtw_odm_proc_write,
#endif
};

static const struct rtw_proc_ops rtw_odm_proc_sseq_fops = {
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(5, 6, 0))
	.proc_open = rtw_odm_proc_open,
	.proc_read = seq_read,
	.proc_lseek = seq_lseek,
	.proc_release = single_release,
	.proc_write = rtw_odm_proc_write,
#else
	.owner = THIS_MODULE,
	.open = rtw_odm_proc_open,
	.read = seq_read,
	.llseek = seq_lseek,
	.release = single_release,
	.write = rtw_odm_proc_write,
#endif
};

struct proc_dir_entry *rtw_odm_proc_init(struct net_device *dev)
{
	struct proc_dir_entry *dir_odm = NULL;
	struct proc_dir_entry *entry = NULL;
	_adapter	*adapter = rtw_netdev_priv(dev);
	ssize_t i;

	if (adapter->dir_dev == NULL) {
		rtw_warn_on(1);
		goto exit;
	}

	if (adapter->dir_odm != NULL) {
		rtw_warn_on(1);
		goto exit;
	}

	dir_odm = rtw_proc_create_dir("odm", adapter->dir_dev, dev);
	if (dir_odm == NULL) {
		rtw_warn_on(1);
		goto exit;
	}

	adapter->dir_odm = dir_odm;

	for (i = 0; i < odm_proc_hdls_num; i++) {
		if (odm_proc_hdls[i].type == RTW_PROC_HDL_TYPE_SEQ)
			entry = rtw_proc_create_entry(odm_proc_hdls[i].name, dir_odm, &rtw_odm_proc_seq_fops, (void *)i);
		else if (odm_proc_hdls[i].type == RTW_PROC_HDL_TYPE_SSEQ ||
			 odm_proc_hdls[i].type == RTW_PROC_HDL_TYPE_SZSEQ)
			entry = rtw_proc_create_entry(odm_proc_hdls[i].name, dir_odm, &rtw_odm_proc_sseq_fops, (void *)i);
		else
			entry = NULL;

		if (!entry) {
			rtw_warn_on(1);
			goto exit;
		}
	}

exit:
	return dir_odm;
}

void rtw_odm_proc_deinit(_adapter  *adapter)
{
	struct proc_dir_entry *dir_odm = NULL;
	int i;

	dir_odm = adapter->dir_odm;

	if (dir_odm == NULL) {
		rtw_warn_on(1);
		return;
	}

	for (i = 0; i < odm_proc_hdls_num; i++)
		remove_proc_entry(odm_proc_hdls[i].name, dir_odm);

	remove_proc_entry("odm", adapter->dir_dev);

	adapter->dir_odm = NULL;

	if (phydm_msg) {
		rtw_mfree(phydm_msg, PHYDM_MSG_LEN);
		phydm_msg = NULL;
	}
}

struct proc_dir_entry *rtw_adapter_proc_init(struct net_device *dev)
{
	struct proc_dir_entry *drv_proc = get_rtw_drv_proc();
	struct proc_dir_entry *dir_dev = NULL;
	struct proc_dir_entry *entry = NULL;
	_adapter *adapter = rtw_netdev_priv(dev);
	ssize_t i;

	if (drv_proc == NULL) {
		rtw_warn_on(1);
		goto exit;
	}

	if (adapter->dir_dev != NULL) {
		rtw_warn_on(1);
		goto exit;
	}

	dir_dev = rtw_proc_create_dir(dev->name, drv_proc, dev);
	if (dir_dev == NULL) {
		rtw_warn_on(1);
		goto exit;
	}

	adapter->dir_dev = dir_dev;

	for (i = 0; i < adapter_proc_hdls_num; i++) {
		if (adapter_proc_hdls[i].type == RTW_PROC_HDL_TYPE_SEQ)
			entry = rtw_proc_create_entry(adapter_proc_hdls[i].name, dir_dev, &rtw_adapter_proc_seq_fops, (void *)i);
		else if (adapter_proc_hdls[i].type == RTW_PROC_HDL_TYPE_SSEQ ||
			 adapter_proc_hdls[i].type == RTW_PROC_HDL_TYPE_SZSEQ)
			entry = rtw_proc_create_entry(adapter_proc_hdls[i].name, dir_dev, &rtw_adapter_proc_sseq_fops, (void *)i);
		else
			entry = NULL;

		if (!entry) {
			rtw_warn_on(1);
			goto exit;
		}
	}

	rtw_odm_proc_init(dev);
exit:
	return dir_dev;
}

void rtw_adapter_proc_deinit(struct net_device *dev)
{
	struct proc_dir_entry *drv_proc = get_rtw_drv_proc();
	struct proc_dir_entry *dir_dev = NULL;
	_adapter *adapter = rtw_netdev_priv(dev);
	int i;

	dir_dev = adapter->dir_dev;

	if (dir_dev == NULL) {
		rtw_warn_on(1);
		return;
	}

	for (i = 0; i < adapter_proc_hdls_num; i++)
		remove_proc_entry(adapter_proc_hdls[i].name, dir_dev);

	rtw_odm_proc_deinit(adapter);
	remove_proc_entry(dev->name, drv_proc);

	adapter->dir_dev = NULL;
}

void rtw_adapter_proc_replace(struct net_device *dev)
{
	struct proc_dir_entry *drv_proc = get_rtw_drv_proc();
	struct proc_dir_entry *dir_dev = NULL;
	_adapter *adapter = rtw_netdev_priv(dev);
	int i;

	dir_dev = adapter->dir_dev;

	if (dir_dev == NULL) {
		rtw_warn_on(1);
		return;
	}

	for (i = 0; i < adapter_proc_hdls_num; i++)
		remove_proc_entry(adapter_proc_hdls[i].name, dir_dev);

	rtw_odm_proc_deinit(adapter);

	remove_proc_entry(adapter->old_ifname, drv_proc);

	adapter->dir_dev = NULL;

	rtw_adapter_proc_init(dev);

}

#endif /* CONFIG_PROC_DEBUG */

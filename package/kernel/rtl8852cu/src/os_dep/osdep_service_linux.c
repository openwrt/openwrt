/******************************************************************************
 *
 * Copyright(c) 2019 Realtek Corporation.
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
#define _OSDEP_SERVICE_LINUX_C_
#include <drv_types.h>

#ifdef DBG_MEMORY_LEAK
ATOMIC_T _malloc_cnt = ATOMIC_INIT(0);
ATOMIC_T _malloc_size = ATOMIC_INIT(0);
#endif /* DBG_MEMORY_LEAK */

/*
* Translate the OS dependent @param error_code to OS independent RTW_STATUS_CODE
* @return: one of RTW_STATUS_CODE
*/
inline int RTW_STATUS_CODE(int error_code)
{
	if (error_code >= 0)
		return _SUCCESS;

	switch (error_code) {
	/* case -ETIMEDOUT: */
	/*	return RTW_STATUS_TIMEDOUT; */
	default:
		return _FAIL;
	}
}

void _rtw_skb_queue_purge(struct sk_buff_head *list)
{
	struct sk_buff *skb;

	while ((skb = skb_dequeue(list)) != NULL)
		_rtw_skb_free(skb);
}

void _rtw_memcpy(void *dst, const void *src, u32 sz)
{
	memcpy(dst, src, sz);
}

inline void _rtw_memmove(void *dst, const void *src, u32 sz)
{
	memmove(dst, src, sz);
}

int _rtw_memcmp(const void *dst, const void *src, u32 sz)
{
	/* under Linux/GNU/GLibc, the return value of memcmp for two same mem. chunk is 0 */
	if (!(memcmp(dst, src, sz)))
		return _TRUE;
	else
		return _FALSE;
}

void _rtw_memset(void *pbuf, int c, u32 sz)
{
	memset(pbuf, c, sz);
}

void _rtw_init_listhead(_list *list)
{
	INIT_LIST_HEAD(list);
}
/*
For the following list_xxx operations,
caller must guarantee the atomic context.
Otherwise, there will be racing condition.
*/
u32 rtw_is_list_empty(_list *phead)
{
	if (list_empty(phead))
		return _TRUE;
	else
		return _FALSE;
}

void rtw_list_insert_head(_list *plist, _list *phead)
{
	list_add(plist, phead);
}

void rtw_list_insert_tail(_list *plist, _list *phead)
{
	list_add_tail(plist, phead);
}

inline void rtw_list_splice(_list *list, _list *head)
{
	list_splice(list, head);
}

inline void rtw_list_splice_init(_list *list, _list *head)
{
	list_splice_init(list, head);
}

inline void rtw_list_splice_tail(_list *list, _list *head)
{
	#if (LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 27))
	if (!list_empty(list))
		__list_splice(list, head);
	#else
	list_splice_tail(list, head);
	#endif
}

inline void rtw_hlist_head_init(rtw_hlist_head *h)
{
	INIT_HLIST_HEAD(h);
}

inline void rtw_hlist_add_head(rtw_hlist_node *n, rtw_hlist_head *h)
{
	hlist_add_head(n, h);
}

inline void rtw_hlist_del(rtw_hlist_node *n)
{
	hlist_del(n);
}

inline void rtw_hlist_add_head_rcu(rtw_hlist_node *n, rtw_hlist_head *h)
{
	hlist_add_head_rcu(n, h);
}

inline void rtw_hlist_del_rcu(rtw_hlist_node *n)
{
	hlist_del_rcu(n);
}

void rtw_init_timer(_timer *ptimer, void *pfunc, void *ctx)
{
	_init_timer(ptimer, pfunc, ctx);
}

systime _rtw_get_current_time(void)
{
	return jiffies;
}

inline u32 _rtw_systime_to_ms(systime stime)
{
	return jiffies_to_msecs(stime);
}

inline u32 _rtw_systime_to_us(systime stime)
{
	return jiffies_to_usecs(stime);
}

inline systime _rtw_ms_to_systime(u32 ms)
{
	return msecs_to_jiffies(ms);
}

inline systime _rtw_us_to_systime(u32 us)
{
	return usecs_to_jiffies(us);
}

inline bool _rtw_time_after(systime a, systime b)
{
	return time_after(a, b);
}

inline bool _rtw_time_after_eq(systime a, systime b)
{
	return time_after_eq(a, b);
}

void rtw_sleep_schedulable(int ms)
{
	u32 delta;

	delta = (ms * HZ) / 1000; /* (ms) */
	if (delta == 0) {
		delta = 1;/* 1 ms */
	}
	set_current_state(TASK_INTERRUPTIBLE);
        schedule_timeout(delta);
	return;
}

void rtw_msleep_os(int ms)
{
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 36))
	if (ms < 20) {
		unsigned long us = ms * 1000UL;
		usleep_range(us, us + 1000UL);
	} else
#endif
		msleep((unsigned int)ms);

}
void rtw_usleep_os(int us)
{
	/* msleep((unsigned int)us); */
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 36))
	usleep_range(us, us + 1);
#else
	if (1 < (us / 1000))
		msleep(1);
	else
		msleep((us / 1000) + 1);
#endif
}


#ifdef DBG_DELAY_OS
void _rtw_mdelay_os(int ms, const char *func, const int line)
{
	RTW_INFO("%s:%d %s(%d)\n", func, line, __FUNCTION__, ms);
	mdelay((unsigned long)ms);
}
void _rtw_udelay_os(int us, const char *func, const int line)
{
	RTW_INFO("%s:%d %s(%d)\n", func, line, __FUNCTION__, us);
	udelay((unsigned long)us);
}
#else
void rtw_mdelay_os(int ms)
{
	mdelay((unsigned long)ms);
}
void rtw_udelay_os(int us)
{
	udelay((unsigned long)us);
}
#endif

void rtw_yield_os(void)
{
	yield();
}


#define RTW_SUSPEND_LOCK_NAME "rtw_wifi"
#define RTW_SUSPEND_TRAFFIC_LOCK_NAME "rtw_wifi_traffic"
#define RTW_SUSPEND_RESUME_LOCK_NAME "rtw_wifi_resume"

#ifdef CONFIG_WAKELOCK
static struct wake_lock rtw_suspend_lock;
static struct wake_lock rtw_suspend_traffic_lock;
static struct wake_lock rtw_suspend_resume_lock;
#elif defined(CONFIG_ANDROID_POWER)
static android_suspend_lock_t rtw_suspend_lock = {
	.name = RTW_SUSPEND_LOCK_NAME
};
static android_suspend_lock_t rtw_suspend_traffic_lock = {
	.name = RTW_SUSPEND_TRAFFIC_LOCK_NAME
};
static android_suspend_lock_t rtw_suspend_resume_lock = {
	.name = RTW_SUSPEND_RESUME_LOCK_NAME
};
#endif

inline void rtw_suspend_lock_init(void)
{
#ifdef CONFIG_WAKELOCK
	wake_lock_init(&rtw_suspend_lock, WAKE_LOCK_SUSPEND, RTW_SUSPEND_LOCK_NAME);
	wake_lock_init(&rtw_suspend_traffic_lock, WAKE_LOCK_SUSPEND, RTW_SUSPEND_TRAFFIC_LOCK_NAME);
	wake_lock_init(&rtw_suspend_resume_lock, WAKE_LOCK_SUSPEND, RTW_SUSPEND_RESUME_LOCK_NAME);
#elif defined(CONFIG_ANDROID_POWER)
	android_init_suspend_lock(&rtw_suspend_lock);
	android_init_suspend_lock(&rtw_suspend_traffic_lock);
	android_init_suspend_lock(&rtw_suspend_resume_lock);
#endif
}

inline void rtw_suspend_lock_uninit(void)
{
#ifdef CONFIG_WAKELOCK
	wake_lock_destroy(&rtw_suspend_lock);
	wake_lock_destroy(&rtw_suspend_traffic_lock);
	wake_lock_destroy(&rtw_suspend_resume_lock);
#elif defined(CONFIG_ANDROID_POWER)
	android_uninit_suspend_lock(&rtw_suspend_lock);
	android_uninit_suspend_lock(&rtw_suspend_traffic_lock);
	android_uninit_suspend_lock(&rtw_suspend_resume_lock);
#endif
}

inline void rtw_lock_suspend(void)
{
#ifdef CONFIG_WAKELOCK
	wake_lock(&rtw_suspend_lock);
#elif defined(CONFIG_ANDROID_POWER)
	android_lock_suspend(&rtw_suspend_lock);
#endif

#if  defined(CONFIG_WAKELOCK) || defined(CONFIG_ANDROID_POWER)
	/* RTW_INFO("####%s: suspend_lock_count:%d####\n", __FUNCTION__, rtw_suspend_lock.stat.count); */
#endif
}

inline void rtw_unlock_suspend(void)
{
#ifdef CONFIG_WAKELOCK
	wake_unlock(&rtw_suspend_lock);
#elif defined(CONFIG_ANDROID_POWER)
	android_unlock_suspend(&rtw_suspend_lock);
#endif

#if  defined(CONFIG_WAKELOCK) || defined(CONFIG_ANDROID_POWER)
	/* RTW_INFO("####%s: suspend_lock_count:%d####\n", __FUNCTION__, rtw_suspend_lock.stat.count); */
#endif
}

inline void rtw_resume_lock_suspend(void)
{
#ifdef CONFIG_WAKELOCK
	wake_lock(&rtw_suspend_resume_lock);
#elif defined(CONFIG_ANDROID_POWER)
	android_lock_suspend(&rtw_suspend_resume_lock);
#endif

#if  defined(CONFIG_WAKELOCK) || defined(CONFIG_ANDROID_POWER)
	/* RTW_INFO("####%s: suspend_lock_count:%d####\n", __FUNCTION__, rtw_suspend_lock.stat.count); */
#endif
}

inline void rtw_resume_unlock_suspend(void)
{
#ifdef CONFIG_WAKELOCK
	wake_unlock(&rtw_suspend_resume_lock);
#elif defined(CONFIG_ANDROID_POWER)
	android_unlock_suspend(&rtw_suspend_resume_lock);
#endif

#if  defined(CONFIG_WAKELOCK) || defined(CONFIG_ANDROID_POWER)
	/* RTW_INFO("####%s: suspend_lock_count:%d####\n", __FUNCTION__, rtw_suspend_lock.stat.count); */
#endif
}

inline void rtw_lock_suspend_timeout(u32 timeout_ms)
{
#ifdef CONFIG_WAKELOCK
	wake_lock_timeout(&rtw_suspend_lock, rtw_ms_to_systime(timeout_ms));
#elif defined(CONFIG_ANDROID_POWER)
	android_lock_suspend_auto_expire(&rtw_suspend_lock, rtw_ms_to_systime(timeout_ms));
#endif
}

inline void rtw_lock_traffic_suspend_timeout(u32 timeout_ms)
{
#ifdef CONFIG_WAKELOCK
	wake_lock_timeout(&rtw_suspend_traffic_lock, rtw_ms_to_systime(timeout_ms));
#elif defined(CONFIG_ANDROID_POWER)
	android_lock_suspend_auto_expire(&rtw_suspend_traffic_lock, rtw_ms_to_systime(timeout_ms));
#endif
	/* RTW_INFO("traffic lock timeout:%d\n", timeout_ms); */
}

inline void rtw_set_bit(int nr, unsigned long *addr)
{
	set_bit(nr, addr);
}

inline void rtw_clear_bit(int nr, unsigned long *addr)
{
	clear_bit(nr, addr);
}

inline int rtw_test_and_clear_bit(int nr, unsigned long *addr)
{
	return test_and_clear_bit(nr, addr);
}
inline int rtw_test_and_set_bit(int nr, unsigned long *addr)
{
	return test_and_set_bit(nr, addr);
}

#if !defined(CONFIG_RTW_ANDROID_GKI)
/*
* Open a file with the specific @param path, @param flag, @param mode
* @param fpp the pointer of struct file pointer to get struct file pointer while file opening is success
* @param path the path of the file to open
* @param flag file operation flags, please refer to linux document
* @param mode please refer to linux document
* @return Linux specific error code
*/
static int openFile(struct file **fpp, const char *path, int flag, int mode)
{
	struct file *fp;

	fp = filp_open(path, flag, mode);
	if (IS_ERR(fp)) {
		*fpp = NULL;
		return PTR_ERR(fp);
	} else {
		*fpp = fp;
		return 0;
	}
}

/*
* Close the file with the specific @param fp
* @param fp the pointer of struct file to close
* @return always 0
*/
static int closeFile(struct file *fp)
{
	filp_close(fp, NULL);
	return 0;
}

static int readFile(struct file *fp, char *buf, int len)
{
	int rlen = 0, sum = 0;

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(4, 1, 0))
	if (!(fp->f_mode & FMODE_CAN_READ))
#else
	if (!fp->f_op || !fp->f_op->read)
#endif
		return -EPERM;

	while (sum < len) {
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(4, 14, 0))
		rlen = kernel_read(fp, buf + sum, len - sum, &fp->f_pos);
#elif (LINUX_VERSION_CODE >= KERNEL_VERSION(4, 1, 0))
		rlen = __vfs_read(fp, buf + sum, len - sum, &fp->f_pos);
#else
		rlen = fp->f_op->read(fp, buf + sum, len - sum, &fp->f_pos);
#endif
		if (rlen > 0)
			sum += rlen;
		else if (0 != rlen)
			return rlen;
		else
			break;
	}

	return  sum;

}

static int writeFile(struct file *fp, char *buf, int len)
{
	int wlen = 0, sum = 0;

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(4, 1, 0))
	if (!(fp->f_mode & FMODE_CAN_WRITE))
#else
	if (!fp->f_op || !fp->f_op->write)
#endif
		return -EPERM;

	while (sum < len) {
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(4, 14, 0))
		wlen = kernel_write(fp, buf + sum, len - sum, &fp->f_pos);
#elif (LINUX_VERSION_CODE >= KERNEL_VERSION(4, 1, 0))
		wlen = __vfs_write(fp, buf + sum, len - sum, &fp->f_pos);
#else
		wlen = fp->f_op->write(fp, buf + sum, len - sum, &fp->f_pos);
#endif
		if (wlen > 0)
			sum += wlen;
		else if (0 != wlen)
			return wlen;
		else
			break;
	}

	return sum;

}

/*
* Test if the specifi @param pathname is a direct and readable
* If readable, @param sz is not used
* @param pathname the name of the path to test
* @return Linux specific error code
*/
static int isDirReadable(const char *pathname, u32 *sz)
{
	struct path path;
	int error = 0;

	return kern_path(pathname, LOOKUP_FOLLOW, &path);
}

/*
* Test if the specifi @param path is a file and readable
* If readable, @param sz is got
* @param path the path of the file to test
* @return Linux specific error code
*/
static int isFileReadable(const char *path, u32 *sz)
{
	struct file *fp;
	int ret = 0;
	#if (LINUX_VERSION_CODE < KERNEL_VERSION(5, 10, 0))
	mm_segment_t oldfs;
	#endif
	char buf;

	fp = filp_open(path, O_RDONLY, 0);
	if (IS_ERR(fp))
		ret = PTR_ERR(fp);
	else {
		#if (LINUX_VERSION_CODE < KERNEL_VERSION(5, 10, 0))
		oldfs = get_fs();
		#if (LINUX_VERSION_CODE >= KERNEL_VERSION(5, 1, 0))
		set_fs(KERNEL_DS);
		#else
		set_fs(get_ds());
		#endif
		#endif

		if (1 != readFile(fp, &buf, 1))
			ret = PTR_ERR(fp);

		if (ret == 0 && sz) {
			#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3, 19, 0))
			*sz = i_size_read(fp->f_path.dentry->d_inode);
			#else
			*sz = i_size_read(fp->f_dentry->d_inode);
			#endif
		}

		#if (LINUX_VERSION_CODE < KERNEL_VERSION(5, 10, 0))
		set_fs(oldfs);
		#endif
		filp_close(fp, NULL);
	}
	return ret;
}
#endif /* !defined(CONFIG_RTW_ANDROID_GKI)*/

/*
* Open the file with @param path and retrive the file content into memory starting from @param buf for @param sz at most
* @param path the path of the file to open and read
* @param buf the starting address of the buffer to store file content
* @param sz how many bytes to read at most
* @return the byte we've read, or Linux specific error code
*/
static int retriveFromFile(const char *path, u8 *buf, u32 sz)
{
#if defined(CONFIG_RTW_ANDROID_GKI)
	int ret = -EINVAL;
	const struct firmware *fw = NULL;
	char* const delim = "/";
	char *name, *token, *cur, *path_tmp = NULL;


	if (path == NULL || buf == NULL) {
		RTW_ERR("%s() NULL pointer\n", __func__);
		goto err;
	}

	path_tmp = kstrdup(path, GFP_KERNEL);
	if (path_tmp == NULL) {
		RTW_ERR("%s() cannot copy path for parsing file name\n", __func__);
		goto err;
	}

	/* parsing file name from path */
	cur = path_tmp;
	token = strsep(&cur, delim);
	while (token != NULL) {
		token = strsep(&cur, delim);
		if(token)
			name = token;
	}

	if (name == NULL) {
		RTW_ERR("%s() parsing file name fail\n", __func__);
		goto err;
	}

	/* request_firmware() will find file in /vendor/firmware but not in path */
	ret = request_firmware(&fw, name, NULL);
	if (ret == 0) {
		RTW_INFO("%s() Success. retrieve file : %s, file size : %zu\n", __func__, name, fw->size);

		if ((u32)fw->size < sz) {
			_rtw_memcpy(buf, fw->data, (u32)fw->size);
			ret = (u32)fw->size;
			goto exit;
		} else {
			RTW_ERR("%s() file size : %zu exceed buf size : %u\n", __func__, fw->size, sz);
			ret = -EFBIG;
			goto err;
		}
	} else {
		RTW_ERR("%s() Fail. retrieve file : %s, error : %d\n", __func__, name, ret);
		goto err;
	}



err:
	RTW_ERR("%s() Fail. retrieve file : %s, error : %d\n", __func__, path, ret);
exit:
	if (path_tmp)
		kfree(path_tmp);
	if (fw)
		release_firmware(fw);
	return ret;
#else /* !defined(CONFIG_RTW_ANDROID_GKI) */
	int ret = -1;
	#if (LINUX_VERSION_CODE < KERNEL_VERSION(5, 10, 0))
	mm_segment_t oldfs;
	#endif
	struct file *fp;

	if (path && buf) {
		ret = openFile(&fp, path, O_RDONLY, 0);
		if (0 == ret) {
			RTW_INFO("%s openFile path:%s fp=%p\n", __FUNCTION__, path , fp);

			#if (LINUX_VERSION_CODE < KERNEL_VERSION(5, 10, 0))
			oldfs = get_fs();
			#if (LINUX_VERSION_CODE >= KERNEL_VERSION(5, 1, 0))
			set_fs(KERNEL_DS);
			#else
			set_fs(get_ds());
			#endif
			#endif

			ret = readFile(fp, buf, sz);

			#if (LINUX_VERSION_CODE < KERNEL_VERSION(5, 10, 0))
			set_fs(oldfs);
			#endif
			closeFile(fp);

			RTW_INFO("%s readFile, ret:%d\n", __FUNCTION__, ret);

		} else
			RTW_INFO("%s openFile path:%s Fail, ret:%d\n", __FUNCTION__, path, ret);
	} else {
		RTW_INFO("%s NULL pointer\n", __FUNCTION__);
		ret =  -EINVAL;
	}
	return ret;
#endif /* defined(CONFIG_RTW_ANDROID_GKI) */
}

#if !defined(CONFIG_RTW_ANDROID_GKI)
/*
* Open the file with @param path and wirte @param sz byte of data starting from @param buf into the file
* @param path the path of the file to open and write
* @param buf the starting address of the data to write into file
* @param sz how many bytes to write at most
* @return the byte we've written, or Linux specific error code
*/
static int storeToFile(const char *path, u8 *buf, u32 sz)
{
	int ret = 0;
	#if (LINUX_VERSION_CODE < KERNEL_VERSION(5, 10, 0))
	mm_segment_t oldfs;
	#endif
	struct file *fp;

	if (path && buf) {
		ret = openFile(&fp, path, O_CREAT | O_WRONLY, 0666);
		if (0 == ret) {
			RTW_INFO("%s openFile path:%s fp=%p\n", __FUNCTION__, path , fp);

			#if (LINUX_VERSION_CODE < KERNEL_VERSION(5, 10, 0))
			oldfs = get_fs();
			#if (LINUX_VERSION_CODE >= KERNEL_VERSION(5, 1, 0))
			set_fs(KERNEL_DS);
			#else
			set_fs(get_ds());
			#endif
			#endif

			ret = writeFile(fp, buf, sz);

			#if (LINUX_VERSION_CODE < KERNEL_VERSION(5, 10, 0))
			set_fs(oldfs);
			#endif
			closeFile(fp);

			RTW_INFO("%s writeFile, ret:%d\n", __FUNCTION__, ret);

		} else
			RTW_INFO("%s openFile path:%s Fail, ret:%d\n", __FUNCTION__, path, ret);
	} else {
		RTW_INFO("%s NULL pointer\n", __FUNCTION__);
		ret =  -EINVAL;
	}
	return ret;
}

/*
* Test if the specifi @param path is a direct and readable
* @param path the path of the direct to test
* @return _TRUE or _FALSE
*/
int rtw_is_dir_readable(const char *path)
{
	if (isDirReadable(path, NULL) == 0)
		return _TRUE;
	else
		return _FALSE;
}
#endif /* !defined(CONFIG_RTW_ANDROID_GKI)*/

/*
* Test if the specifi @param path is a file and readable
* @param path the path of the file to test
* @return _TRUE or _FALSE
*/
int rtw_is_file_readable(const char *path)
{
#if !defined(CONFIG_RTW_ANDROID_GKI)
	if (isFileReadable(path, NULL) == 0)
		return _TRUE;
	else
		return _FALSE;
#else
	RTW_INFO("%s() Android GKI prohibbit kernel_read, return _TRUE\n", __func__);
	return  _TRUE;
#endif /* !defined(CONFIG_RTW_ANDROID_GKI) */
}

/*
* Test if the specifi @param path is a file and readable.
* If readable, @param sz is got
* @param path the path of the file to test
* @return _TRUE or _FALSE
*/
int rtw_is_file_readable_with_size(const char *path, u32 *sz)
{
#if !defined(CONFIG_RTW_ANDROID_GKI)
	if (isFileReadable(path, sz) == 0)
		return _TRUE;
	else
		return _FALSE;
#else
	RTW_INFO("%s() Android GKI prohibbit kernel_read, return _TRUE\n", __func__);
	*sz = 0;
	return  _TRUE;
#endif /* !defined(CONFIG_RTW_ANDROID_GKI) */
}


/*
* Open the file with @param path and retrive the file content into memory starting from @param buf for @param sz at most
* @param path the path of the file to open and read
* @param buf the starting address of the buffer to store file content
* @param sz how many bytes to read at most
* @return the byte we've read
*/
int rtw_retrieve_from_file(const char *path, u8 *buf, u32 sz)
{
	int ret = retriveFromFile(path, buf, sz);
	return ret >= 0 ? ret : 0;
}

#if !defined(CONFIG_RTW_ANDROID_GKI)
/*
* Open the file with @param path and wirte @param sz byte of data starting from @param buf into the file
* @param path the path of the file to open and write
* @param buf the starting address of the data to write into file
* @param sz how many bytes to write at most
* @return the byte we've written
*/
int rtw_store_to_file(const char *path, u8 *buf, u32 sz)
{
	int ret = storeToFile(path, buf, sz);
	return ret >= 0 ? ret : 0;
}
#endif /* !defined(CONFIG_RTW_ANDROID_GKI) */

struct net_device *rtw_alloc_etherdev_with_old_priv(int sizeof_priv, void *old_priv)
{
	struct net_device *pnetdev;
	struct rtw_netdev_priv_indicator *pnpi;

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 35))
	pnetdev = alloc_etherdev_mq(sizeof(struct rtw_netdev_priv_indicator), 4);
#else
	pnetdev = alloc_etherdev(sizeof(struct rtw_netdev_priv_indicator));
#endif
	if (!pnetdev)
		goto RETURN;

	pnpi = netdev_priv(pnetdev);
	pnpi->priv = old_priv;
	pnpi->sizeof_priv = sizeof_priv;

RETURN:
	return pnetdev;
}

struct net_device *rtw_alloc_etherdev(int sizeof_priv)
{
	struct net_device *pnetdev;
	struct rtw_netdev_priv_indicator *pnpi;

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 35))
	pnetdev = alloc_etherdev_mq(sizeof(struct rtw_netdev_priv_indicator), 4);
#else
	pnetdev = alloc_etherdev(sizeof(struct rtw_netdev_priv_indicator));
#endif
	if (!pnetdev)
		goto RETURN;

	pnpi = netdev_priv(pnetdev);

	pnpi->priv = rtw_zvmalloc(sizeof_priv);
	if (!pnpi->priv) {
		free_netdev(pnetdev);
		pnetdev = NULL;
		goto RETURN;
	}

	pnpi->sizeof_priv = sizeof_priv;
RETURN:
	return pnetdev;
}

void rtw_free_netdev(struct net_device *netdev)
{
	struct rtw_netdev_priv_indicator *pnpi;

	if (!netdev)
		goto RETURN;

	pnpi = netdev_priv(netdev);

	if (!pnpi->priv)
		goto RETURN;

	free_netdev(netdev);

RETURN:
	return;
}

int rtw_change_ifname(_adapter *padapter, const char *ifname)
{
	struct dvobj_priv *dvobj;
	struct net_device *pnetdev;
	struct net_device *cur_pnetdev;
	struct rereg_nd_name_data *rereg_priv;
	int ret;
	u8 rtnl_lock_needed;

	if (!padapter)
		goto error;

	dvobj = adapter_to_dvobj(padapter);
	cur_pnetdev = padapter->pnetdev;
	rereg_priv = &padapter->rereg_nd_name_priv;

	/* free the old_pnetdev */
	if (rereg_priv->old_pnetdev) {
		free_netdev(rereg_priv->old_pnetdev);
		rereg_priv->old_pnetdev = NULL;
	}

	rtnl_lock_needed = rtw_rtnl_lock_needed(dvobj);

	if (rtnl_lock_needed)
		unregister_netdev(cur_pnetdev);
	else
		unregister_netdevice(cur_pnetdev);

	rereg_priv->old_pnetdev = cur_pnetdev;

	pnetdev = rtw_init_netdev(padapter);
	if (!pnetdev)  {
		ret = -1;
		goto error;
	}

	SET_NETDEV_DEV(pnetdev, dvobj_to_dev(adapter_to_dvobj(padapter)));

	rtw_init_netdev_name(pnetdev, ifname);

	dev_addr_mod(pnetdev, 0, adapter_mac_addr(padapter), ETH_ALEN);

	if (rtnl_lock_needed)
		ret = register_netdev(pnetdev);
	else
		ret = register_netdevice(pnetdev);

	if (ret != 0) {
		goto error;
	}

	return 0;

error:

	return -1;

}

#ifdef CONFIG_PLATFORM_SPRD
#ifdef do_div
	#undef do_div
#endif
	#include <asm-generic/div64.h>
#endif

u64 rtw_modular64(u64 x, u64 y)
{
	return do_div(x, y);
}

u64 rtw_division64(u64 x, u64 y)
{
	do_div(x, y);
	return x;
}

inline u32 rtw_random32(void)
{
	#if (LINUX_VERSION_CODE >= KERNEL_VERSION(6,1,0))
	return get_random_u32();
	#elif (LINUX_VERSION_CODE >= KERNEL_VERSION(3,8,0))
	return prandom_u32();
	#elif (LINUX_VERSION_CODE <= KERNEL_VERSION(2,6,18))
	u32 random_int;
	get_random_bytes( &random_int , 4 );
	return random_int;
	#else
	return random32();
	#endif
}

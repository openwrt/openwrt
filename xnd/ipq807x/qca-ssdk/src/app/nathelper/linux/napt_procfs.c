/*
 * Copyright (c) 2012, 2015, The Linux Foundation. All rights reserved.
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
 *  napt_procfs.c -  create files in /proc
 *
 */
#include <linux/version.h>
#ifdef KVER32
#include <generated/autoconf.h>
#else
#include <linux/autoconf.h>
#include <linux/kernel.h>
#endif
#include <linux/kconfig.h>
#include <linux/proc_fs.h>
#include <linux/sysfs.h>
#include <linux/if_ether.h>
#include <asm/uaccess.h>    /* for copy_from_user */
#include "aos_types.h"

#ifdef AUTO_UPDATE_PPPOE_INFO
#define NF_PROCFS_PERM  0444
#else
#define NF_PROCFS_PERM  0644
#endif

#define ATHRS17_MAC_LEN         13      // 12+1
#define ATHRS17_IP_LEN     9       // 8+1
#define ATHRS17_CHAR_MAX_LEN ATHRS17_MAC_LEN

#define NF_PROCFS_DIR                        "qca_switch"

#define NF_ATHRS17_HNAT_NAME                 "nf_athrs17_hnat"
#define NF_ATHRS17_HNAT_WAN_TYPE_NAME        "nf_athrs17_hnat_wan_type"
#define NF_ATHRS17_HNAT_PPP_ID_NAME          "nf_athrs17_hnat_ppp_id"
#define NF_ATHRS17_HNAT_UDP_THRESH_NAME      "nf_athrs17_hnat_udp_thresh"
#define NF_ATHRS17_HNAT_WAN_IP_NAME          "nf_athrs17_hnat_wan_ip"
#define NF_ATHRS17_HNAT_PPP_PEER_IP_NAME     "nf_athrs17_hnat_ppp_peer_ip"
#define NF_ATHRS17_HNAT_PPP_PEER_MAC_NAME    "nf_athrs17_hnat_ppp_peer_mac"
#define NF_ATHRS17_HNAT_WAN_MAC_NAME         "nf_athrs17_hnat_wan_mac"

#define NF_ATHRS17_HNAT_PPP_ID2_NAME          "nf_athrs17_hnat_ppp_id2"
#define NF_ATHRS17_HNAT_PPP_PEER_MAC2_NAME    "nf_athrs17_hnat_ppp_peer_mac2"

/* for PPPoE */
int nf_athrs17_hnat = 1;
int nf_athrs17_hnat_wan_type = 0;
int nf_athrs17_hnat_ppp_id = 0;
int nf_athrs17_hnat_udp_thresh = 0;
a_uint32_t nf_athrs17_hnat_wan_ip = 0;
a_uint32_t nf_athrs17_hnat_ppp_peer_ip = 0;
unsigned char nf_athrs17_hnat_ppp_peer_mac[ETH_ALEN] = {0};
unsigned char nf_athrs17_hnat_wan_mac[ETH_ALEN] = {0};
extern int nf_athrs17_hnat_sync_counter_en;
extern char  hnat_log_en;
extern int scan_period;
extern int scan_enable;
extern int napt_need_clean;
extern int wan_switch;
extern char nat_wan_port;
extern a_uint32_t packet_thres_base;
extern a_uint32_t polling_quota;
extern a_bool_t napt_add_bypass_check;
extern void napt_wan_switch_prehandle(void);
/* for IPv6 over PPPoE (only for S17c)*/
int nf_athrs17_hnat_ppp_id2 = 0;
unsigned char nf_athrs17_hnat_ppp_peer_mac2[ETH_ALEN] = {0};

#if 0
static void setup_proc_entry(void)
{
    nf_athrs17_hnat = 1;
    nf_athrs17_hnat_wan_type = 0;
    nf_athrs17_hnat_ppp_id = 0;
    memset(&nf_athrs17_hnat_ppp_peer_mac, 0, ETH_ALEN);
    memset(&nf_athrs17_hnat_wan_mac, 0, ETH_ALEN);
    nf_athrs17_hnat_ppp_peer_ip = 0;
    nf_athrs17_hnat_wan_ip = 0;

    nf_athrs17_hnat_ppp_id2 = 0;
    memset(&nf_athrs17_hnat_ppp_peer_mac2, 0, ETH_ALEN);
	nf_athrs17_hnat_sync_counter_en = 0;
}

/**
 * This structure hold information about the /proc file
 *
 */
static struct proc_dir_entry *qca_switch_dir;

static struct proc_dir_entry *nf_athrs17_hnat_file;
static struct proc_dir_entry *nf_athrs17_hnat_wan_type_file;
static struct proc_dir_entry *nf_athrs17_hnat_ppp_id_file;
static struct proc_dir_entry *nf_athrs17_hnat_udp_thresh_file;
static struct proc_dir_entry *nf_athrs17_hnat_wan_ip_file;
static struct proc_dir_entry *nf_athrs17_hnat_ppp_peer_ip_file;
static struct proc_dir_entry *nf_athrs17_hnat_ppp_peer_mac_file;
static struct proc_dir_entry *nf_athrs17_hnat_wan_mac_file;

static struct proc_dir_entry *nf_athrs17_hnat_ppp_id2_file;
static struct proc_dir_entry *nf_athrs17_hnat_ppp_peer_mac2_file;

/**
 * This function is called then the /proc file is read
 *
 */
static int procfile_read_int(char *page, char **start, off_t off, int count,  int *eof, void *data)
{
    int ret;
    int *prv_data = (int *)data;

	ret = snprintf(page, sizeof(int), "%d\n", *prv_data);

    return ret;
}

static int procfile_read_ip(char *page, char **start, off_t off, int count, int *eof, void *data)
{
	int ret;
	unsigned char *prv_data = (unsigned char *)data;

	ret = snprintf(page, sizeof(a_uint32_t), "%d.%d.%d.%d\n", prv_data[0], prv_data[1], prv_data[2], prv_data[3]);

	return ret;
}

static int procfile_read_mac(char *page, char **start, off_t off, int count, int *eof, void *data)
{
	int ret;
	unsigned char *prv_data = (unsigned char *)data;
	unsigned long long *ptr_ull;

	ret = snprintf(page, sizeof(unsigned char)*ETH_ALEN, "%.2x-%.2x-%.2x-%.2x-%.2x-%.2x\n",
				prv_data[0], prv_data[1], prv_data[2], prv_data[3], prv_data[4], prv_data[5]);

	ptr_ull = (unsigned long long *)prv_data;

	return ret;
}

/**
 * This function is called with the /proc file is written
 *
 */
#ifdef AUTO_UPDATE_PPPOE_INFO
#define procfile_write_int NULL
#define procfile_write_ip NULL
#define procfile_write_mac NULL
#else
static int procfile_write_int(struct file *file, const char *buffer, unsigned long count, void *data)
{
    int len;
    uint8_t tmp_buf[9] = {'0', '0', '0', '0', '0', '0', '0', '0', '0'};
    unsigned int *prv_data = (unsigned int *)data;
	int res = 0;
    
    if(count > sizeof(tmp_buf))
        len = sizeof(tmp_buf);
    else
        len = count;

    if(copy_from_user(tmp_buf, buffer, len))
        return -EFAULT;

	tmp_buf[len-1] = '\0';
	res = kstrtol((const char *)tmp_buf, 10, prv_data);
	if(res < 0)
		return res;

    // printk("[write] prv_data 0x%p -> 0x%08x\n", prv_data, *prv_data);

    return len;
}

static int procfile_write_ip(struct file *file, const char *buffer, unsigned long count, void *data)
{
    int ret;
    int len;
    unsigned char tmp_buf[ATHRS17_IP_LEN];
    unsigned long *prv_data = (unsigned long *)data;
	int res = 0;

    if(count > ATHRS17_IP_LEN)
        len = ATHRS17_IP_LEN;
    else
        len = count;
		
    if(copy_from_user(tmp_buf, buffer, len))
		return -EFAULT;

    tmp_buf[len-1] = '\0';

	res = kstrtol((const char *)tmp_buf, 16, prv_data);
	if(res < 0)
		return res;

    return ret;
}

static int procfile_write_mac(struct file *file, const char *buffer, unsigned long count, void *data)
{
    int ret;
    int len;
    unsigned char tmp_buf[ATHRS17_MAC_LEN];
    unsigned char *ptr_char;
    unsigned long long *prv_data = (unsigned long long *)data;
	int res = 0;

    if(count > ATHRS17_MAC_LEN)
        len = ATHRS17_MAC_LEN;
    else
        len = count;
		
    if(copy_from_user((void *)tmp_buf, buffer, len))
		return -EFAULT;

    tmp_buf[len-1] = 't';

	res = kstrtoll((const char *)tmp_buf, 16, prv_data);
	if(res < 0)
		return res;
    *prv_data = cpu_to_be64p(prv_data);
    ptr_char = (unsigned char *)prv_data;
    ptr_char[0] = ptr_char[2];
    ptr_char[1] = ptr_char[3];
    ptr_char[2] = ptr_char[4];
    ptr_char[3] = ptr_char[5];
    ptr_char[4] = ptr_char[6];
    ptr_char[5] = ptr_char[7];

    return ret;
}
#endif // ifdef AUTO_UPDATE_PPPOE_INFO


int napt_procfs_init(void)
{
    int ret = 0;

    setup_proc_entry();

    /* create directory */
    qca_switch_dir = proc_mkdir(NF_PROCFS_DIR, NULL);
    if(qca_switch_dir == NULL)
    {
        ret = -ENOMEM;
        goto err_out;
    }

    /* create the /proc file */
    nf_athrs17_hnat_file = create_proc_entry(NF_ATHRS17_HNAT_NAME, 0644, qca_switch_dir);
    if (NULL == nf_athrs17_hnat_file)
    {
        printk("Error: Can not create /proc/%s/%s\n", NF_PROCFS_DIR, NF_ATHRS17_HNAT_NAME);
        goto no_athrs17_hnat;
    }
    nf_athrs17_hnat_file->data = &nf_athrs17_hnat;
    nf_athrs17_hnat_file->read_proc  = procfile_read_int;
    nf_athrs17_hnat_file->write_proc = procfile_write_int;
    printk("/proc/%s/%s is created\n", NF_PROCFS_DIR, NF_ATHRS17_HNAT_NAME);

    nf_athrs17_hnat_wan_type_file = create_proc_entry(NF_ATHRS17_HNAT_WAN_TYPE_NAME, NF_PROCFS_PERM, qca_switch_dir);
    if (NULL == nf_athrs17_hnat_wan_type_file)
    {
        printk("Error: Can not create /proc/%s/%s\n", NF_PROCFS_DIR, NF_ATHRS17_HNAT_WAN_TYPE_NAME);
        goto no_athrs17_hnat_wan_type;
    }
    nf_athrs17_hnat_wan_type_file->data = &nf_athrs17_hnat_wan_type;
    nf_athrs17_hnat_wan_type_file->read_proc  = procfile_read_int;
    nf_athrs17_hnat_wan_type_file->write_proc = procfile_write_int;
    printk("/proc/%s/%s is created\n", NF_PROCFS_DIR, NF_ATHRS17_HNAT_WAN_TYPE_NAME);

    nf_athrs17_hnat_ppp_id_file = create_proc_entry(NF_ATHRS17_HNAT_PPP_ID_NAME, NF_PROCFS_PERM, qca_switch_dir);
    if (NULL == nf_athrs17_hnat_ppp_id_file)
    {
        printk("Error: Can not create /proc/%s/%s\n", NF_PROCFS_DIR, NF_ATHRS17_HNAT_PPP_ID_NAME);
        goto no_athrs17_hnat_ppp_id;
    }
    nf_athrs17_hnat_ppp_id_file->data = &nf_athrs17_hnat_ppp_id;
    nf_athrs17_hnat_ppp_id_file->read_proc  = procfile_read_int;
    nf_athrs17_hnat_ppp_id_file->write_proc = procfile_write_int;
    printk("/proc/%s/%s is created\n", NF_PROCFS_DIR, NF_ATHRS17_HNAT_PPP_ID_NAME);

    nf_athrs17_hnat_udp_thresh_file = create_proc_entry(NF_ATHRS17_HNAT_UDP_THRESH_NAME, NF_PROCFS_PERM, qca_switch_dir);
    if (NULL == nf_athrs17_hnat_udp_thresh_file)
    {
        printk("Error: Can not create /proc/%s/%s\n", NF_PROCFS_DIR, NF_ATHRS17_HNAT_UDP_THRESH_NAME);
        goto no_athrs17_hnat_udp_thresh;
    }
    nf_athrs17_hnat_udp_thresh_file->data = &nf_athrs17_hnat_udp_thresh;
    nf_athrs17_hnat_udp_thresh_file->read_proc  = procfile_read_int;
    nf_athrs17_hnat_udp_thresh_file->write_proc = procfile_write_int;
    printk("/proc/%s/%s is created\n", NF_PROCFS_DIR, NF_ATHRS17_HNAT_UDP_THRESH_NAME);

    nf_athrs17_hnat_wan_ip_file = create_proc_entry(NF_ATHRS17_HNAT_WAN_IP_NAME, NF_PROCFS_PERM, qca_switch_dir);
    if (NULL == nf_athrs17_hnat_wan_ip_file)
    {
        printk("Error: Can not create /proc/%s/%s\n", NF_PROCFS_DIR, NF_ATHRS17_HNAT_WAN_IP_NAME);
        goto no_athrs17_hnat_wan_ip;
    }
    nf_athrs17_hnat_wan_ip_file->data = &nf_athrs17_hnat_wan_ip;
    nf_athrs17_hnat_wan_ip_file->read_proc  = procfile_read_ip;
    nf_athrs17_hnat_wan_ip_file->write_proc = procfile_write_ip;
    printk("/proc/%s/%s is created\n", NF_PROCFS_DIR, NF_ATHRS17_HNAT_WAN_IP_NAME);

    nf_athrs17_hnat_ppp_peer_ip_file = create_proc_entry(NF_ATHRS17_HNAT_PPP_PEER_IP_NAME, NF_PROCFS_PERM, qca_switch_dir);
    if (NULL == nf_athrs17_hnat_ppp_peer_ip_file)
    {
        printk("Error: Can not create /proc/%s/%s\n", NF_PROCFS_DIR, NF_ATHRS17_HNAT_PPP_PEER_IP_NAME);
        goto no_athrs17_hnat_ppp_peer_ip;
    }
    nf_athrs17_hnat_ppp_peer_ip_file->data = &nf_athrs17_hnat_ppp_peer_ip;
    nf_athrs17_hnat_ppp_peer_ip_file->read_proc  = procfile_read_ip;
    nf_athrs17_hnat_ppp_peer_ip_file->write_proc = procfile_write_ip;
    printk("/proc/%s/%s is created\n", NF_PROCFS_DIR, NF_ATHRS17_HNAT_PPP_PEER_IP_NAME);

    nf_athrs17_hnat_ppp_peer_mac_file = create_proc_entry(NF_ATHRS17_HNAT_PPP_PEER_MAC_NAME, NF_PROCFS_PERM, qca_switch_dir);
    if (NULL == nf_athrs17_hnat_ppp_peer_mac_file)
    {
        printk("Error: Can not create /proc/%s/%s\n", NF_PROCFS_DIR, NF_ATHRS17_HNAT_PPP_PEER_MAC_NAME);
        goto no_athrs17_hnat_ppp_peer_mac;
    }
    nf_athrs17_hnat_ppp_peer_mac_file->data = &nf_athrs17_hnat_ppp_peer_mac;
    nf_athrs17_hnat_ppp_peer_mac_file->read_proc  = procfile_read_mac;
    nf_athrs17_hnat_ppp_peer_mac_file->write_proc = procfile_write_mac;
    printk("/proc/%s/%s is created\n", NF_PROCFS_DIR, NF_ATHRS17_HNAT_PPP_PEER_MAC_NAME);

    nf_athrs17_hnat_wan_mac_file = create_proc_entry(NF_ATHRS17_HNAT_WAN_MAC_NAME, NF_PROCFS_PERM, qca_switch_dir);
    if (NULL == nf_athrs17_hnat_wan_mac_file)
    {
        printk("Error: Can not create /proc/%s/%s\n", NF_PROCFS_DIR, NF_ATHRS17_HNAT_WAN_MAC_NAME);
        goto no_athrs17_hnat_wan_mac;
    }
    nf_athrs17_hnat_wan_mac_file->data = &nf_athrs17_hnat_wan_mac;
    nf_athrs17_hnat_wan_mac_file->read_proc  = procfile_read_mac;
    nf_athrs17_hnat_wan_mac_file->write_proc = procfile_write_mac;
    printk("/proc/%s/%s is created\n", NF_PROCFS_DIR, NF_ATHRS17_HNAT_WAN_MAC_NAME);

    nf_athrs17_hnat_ppp_id2_file = create_proc_entry(NF_ATHRS17_HNAT_PPP_ID2_NAME, NF_PROCFS_PERM, qca_switch_dir);
    if (NULL == nf_athrs17_hnat_ppp_id2_file)
    {
        printk("Error: Can not create /proc/%s/%s\n", NF_PROCFS_DIR, NF_ATHRS17_HNAT_PPP_ID2_NAME);
        goto no_athrs17_hnat_ppp_id;
    }
    nf_athrs17_hnat_ppp_id2_file->data = &nf_athrs17_hnat_ppp_id2;
    nf_athrs17_hnat_ppp_id2_file->read_proc  = procfile_read_int;
    nf_athrs17_hnat_ppp_id2_file->write_proc = procfile_write_int;
    printk("/proc/%s/%s is created\n", NF_PROCFS_DIR, NF_ATHRS17_HNAT_PPP_ID2_NAME);

    nf_athrs17_hnat_ppp_peer_mac2_file = create_proc_entry(NF_ATHRS17_HNAT_PPP_PEER_MAC2_NAME, NF_PROCFS_PERM, qca_switch_dir);
    if (NULL == nf_athrs17_hnat_ppp_peer_mac2_file)
    {
        printk("Error: Can not create /proc/%s/%s\n", NF_PROCFS_DIR, NF_ATHRS17_HNAT_PPP_PEER_MAC2_NAME);
        goto no_athrs17_hnat_ppp_peer_mac;
    }
    nf_athrs17_hnat_ppp_peer_mac2_file->data = &nf_athrs17_hnat_ppp_peer_mac2;
    nf_athrs17_hnat_ppp_peer_mac2_file->read_proc  = procfile_read_mac;
    nf_athrs17_hnat_ppp_peer_mac2_file->write_proc = procfile_write_mac;
    printk("/proc/%s/%s is created\n", NF_PROCFS_DIR, NF_ATHRS17_HNAT_PPP_PEER_MAC_NAME);

    return 0;

no_athrs17_hnat_wan_mac:
    remove_proc_entry(NF_ATHRS17_HNAT_PPP_PEER_MAC_NAME, qca_switch_dir);
no_athrs17_hnat_ppp_peer_mac:
    remove_proc_entry(NF_ATHRS17_HNAT_PPP_PEER_IP_NAME, qca_switch_dir);
no_athrs17_hnat_ppp_peer_ip:
    remove_proc_entry(NF_ATHRS17_HNAT_WAN_IP_NAME, qca_switch_dir);
no_athrs17_hnat_wan_ip:
    remove_proc_entry(NF_ATHRS17_HNAT_UDP_THRESH_NAME, qca_switch_dir);
no_athrs17_hnat_udp_thresh:
    remove_proc_entry(NF_ATHRS17_HNAT_PPP_ID_NAME, qca_switch_dir);
no_athrs17_hnat_ppp_id:
    remove_proc_entry(NF_ATHRS17_HNAT_WAN_TYPE_NAME, qca_switch_dir);
no_athrs17_hnat_wan_type:
    remove_proc_entry(NF_ATHRS17_HNAT_NAME, qca_switch_dir);
no_athrs17_hnat:
    remove_proc_entry(NF_PROCFS_DIR, NULL);
err_out:
    return ret;
}

void napt_procfs_exit(void)
{
    remove_proc_entry(NF_ATHRS17_HNAT_NAME, qca_switch_dir);
    remove_proc_entry(NF_ATHRS17_HNAT_WAN_TYPE_NAME, qca_switch_dir);
    remove_proc_entry(NF_ATHRS17_HNAT_PPP_ID_NAME, qca_switch_dir);
    remove_proc_entry(NF_ATHRS17_HNAT_UDP_THRESH_NAME, qca_switch_dir);
    remove_proc_entry(NF_ATHRS17_HNAT_WAN_IP_NAME, qca_switch_dir);
    remove_proc_entry(NF_ATHRS17_HNAT_PPP_PEER_IP_NAME, qca_switch_dir);
    remove_proc_entry(NF_ATHRS17_HNAT_PPP_PEER_MAC_NAME, qca_switch_dir);
    remove_proc_entry(NF_ATHRS17_HNAT_WAN_MAC_NAME, qca_switch_dir);
    remove_proc_entry(NF_ATHRS17_HNAT_PPP_ID2_NAME, qca_switch_dir);
    remove_proc_entry(NF_ATHRS17_HNAT_PPP_PEER_MAC2_NAME, qca_switch_dir);
    remove_proc_entry(NF_PROCFS_DIR, NULL);
    printk(KERN_INFO "/proc/%s/%s removed\n", NF_PROCFS_DIR, NF_ATHRS17_HNAT_NAME);
}
#else
static ssize_t napt_hnat_get(struct device *dev,
		  struct device_attribute *attr,
		  char *buf)
{
	ssize_t count;
	a_uint32_t num;

	num = (a_uint32_t)nf_athrs17_hnat;

	count = snprintf(buf, (ssize_t)PAGE_SIZE, "%u", num);
	return count;
}

static ssize_t napt_hnat_set(struct device *dev,
		  struct device_attribute *attr,
		  const char *buf, size_t count)
{
	char num_buf[12];
	a_uint32_t num;


	if (count >= sizeof(num_buf)) return 0;
	memcpy(num_buf, buf, count);
	num_buf[count] = '\0';
	sscanf(num_buf, "%u", &num);

	nf_athrs17_hnat = num;

	return count;
}

static ssize_t napt_wan_type_get(struct device *dev,
		  struct device_attribute *attr,
		  char *buf)
{
	ssize_t count;
	a_uint32_t num;

	num = (a_uint32_t)nf_athrs17_hnat_wan_type;

	count = snprintf(buf, (ssize_t)PAGE_SIZE, "%u", num);
	return count;
}

static ssize_t napt_wan_type_set(struct device *dev,
		  struct device_attribute *attr,
		  const char *buf, size_t count)
{
	char num_buf[12];
	a_uint32_t num;


	if (count >= sizeof(num_buf)) return 0;
	memcpy(num_buf, buf, count);
	num_buf[count] = '\0';
	sscanf(num_buf, "%u", &num);

	nf_athrs17_hnat_wan_type = num;

	return count;
}

static ssize_t napt_ppp_id_get(struct device *dev,
		  struct device_attribute *attr,
		  char *buf)
{
	ssize_t count;
	a_uint32_t num;

	num = (a_uint32_t)nf_athrs17_hnat_ppp_id;

	count = snprintf(buf, (ssize_t)PAGE_SIZE, "%u", num);
	return count;
}

static ssize_t napt_ppp_id_set(struct device *dev,
		  struct device_attribute *attr,
		  const char *buf, size_t count)
{
	char num_buf[12];
	a_uint32_t num;


	if (count >= sizeof(num_buf)) return 0;
	memcpy(num_buf, buf, count);
	num_buf[count] = '\0';
	sscanf(num_buf, "%u", &num);

	nf_athrs17_hnat_ppp_id = num;

	return count;
}

static ssize_t napt_udp_thresh_get(struct device *dev,
		  struct device_attribute *attr,
		  char *buf)
{
	ssize_t count;
	a_uint32_t num;

	num = (a_uint32_t)nf_athrs17_hnat_udp_thresh;

	count = snprintf(buf, (ssize_t)PAGE_SIZE, "%u", num);
	return count;
}

static ssize_t napt_udp_thresh_set(struct device *dev,
		  struct device_attribute *attr,
		  const char *buf, size_t count)
{
	char num_buf[12];
	a_uint32_t num;


	if (count >= sizeof(num_buf)) return 0;
	memcpy(num_buf, buf, count);
	num_buf[count] = '\0';
	sscanf(num_buf, "%u", &num);

	nf_athrs17_hnat_udp_thresh = num;

	return count;
}

static ssize_t napt_wan_ip_get(struct device *dev,
		  struct device_attribute *attr,
		  char *buf)
{
	ssize_t count;
	unsigned char* data;

	data = (unsigned char*)&nf_athrs17_hnat_wan_ip;

	count = snprintf(buf, (ssize_t)PAGE_SIZE, "%d.%d.%d.%d",
					data[0], data[1], data[2], data[3]);
	return count;
}

static ssize_t napt_wan_ip_set(struct device *dev,
		  struct device_attribute *attr,
		  const char *buf, size_t count)
{
	char num_buf[12];
	a_uint32_t num;


	if (count >= sizeof(num_buf)) return 0;
	memcpy(num_buf, buf, count);
	num_buf[count] = '\0';
	sscanf(num_buf, "%u", &num);

	nf_athrs17_hnat_wan_ip = num;

	return count;
}

static ssize_t napt_ppp_peer_ip_get(struct device *dev,
		  struct device_attribute *attr,
		  char *buf)
{
	ssize_t count;
	unsigned char* data;

	data = (unsigned char*)&nf_athrs17_hnat_ppp_peer_ip;

	count = snprintf(buf, (ssize_t)PAGE_SIZE, "%d.%d.%d.%d",
					data[0], data[1], data[2], data[3]);
	return count;
}

static ssize_t napt_ppp_peer_ip_set(struct device *dev,
		  struct device_attribute *attr,
		  const char *buf, size_t count)
{
	char num_buf[12];
	a_uint32_t num;


	if (count >= sizeof(num_buf)) return 0;
	memcpy(num_buf, buf, count);
	num_buf[count] = '\0';
	sscanf(num_buf, "%u", &num);

	nf_athrs17_hnat_ppp_peer_ip = num;

	return count;
}

static ssize_t napt_peer_mac_get(struct device *dev,
		  struct device_attribute *attr,
		  char *buf)
{
	ssize_t count;
	unsigned char* data;

	data = (unsigned char*)&nf_athrs17_hnat_ppp_peer_mac;

	count = snprintf(buf, (ssize_t)PAGE_SIZE, "%.2x-%.2x-%.2x-%.2x-%.2x-%.2x",
					data[0], data[1], data[2], data[3], data[4], data[5]);
	return count;
}

static ssize_t napt_peer_mac_set(struct device *dev,
		  struct device_attribute *attr,
		  const char *buf, size_t count)
{
	char num_buf[32];
	unsigned long long prv_data;
	unsigned char *ptr_char;
	int res = 0;

	if (count >= sizeof(num_buf)) return 0;
	memcpy(num_buf, buf, count);
	num_buf[count] = '\0';
	res = kstrtoll((const char *)num_buf, 16, &prv_data);
	if(res < 0)
		return res;

	prv_data = cpu_to_be64p(&prv_data);
	ptr_char = (unsigned char *)&prv_data;
	nf_athrs17_hnat_ppp_peer_mac[0] = ptr_char[2];
    nf_athrs17_hnat_ppp_peer_mac[1] = ptr_char[3];
    nf_athrs17_hnat_ppp_peer_mac[2] = ptr_char[4];
    nf_athrs17_hnat_ppp_peer_mac[3] = ptr_char[5];
    nf_athrs17_hnat_ppp_peer_mac[4] = ptr_char[6];
    nf_athrs17_hnat_ppp_peer_mac[5] = ptr_char[7];

	return count;
}

static ssize_t napt_wan_mac_get(struct device *dev,
		  struct device_attribute *attr,
		  char *buf)
{
	ssize_t count;
	unsigned char* data;

	data = (unsigned char*)&nf_athrs17_hnat_wan_mac;

	count = snprintf(buf, (ssize_t)PAGE_SIZE, "%.2x-%.2x-%.2x-%.2x-%.2x-%.2x",
					data[0], data[1], data[2], data[3], data[4], data[5]);
	return count;
}

static ssize_t napt_wan_mac_set(struct device *dev,
		  struct device_attribute *attr,
		  const char *buf, size_t count)
{
	char num_buf[32];
	unsigned long long prv_data;
	unsigned char *ptr_char;
	int res = 0;

	if (count >= sizeof(num_buf)) return 0;
	memcpy(num_buf, buf, count);
	num_buf[count] = '\0';
	res = kstrtoll((const char *)num_buf, 16, &prv_data);
	if(res < 0)
		return res;

	prv_data = cpu_to_be64p(&prv_data);
	ptr_char = (unsigned char *)&prv_data;
	nf_athrs17_hnat_wan_mac[0] = ptr_char[2];
    nf_athrs17_hnat_wan_mac[1] = ptr_char[3];
    nf_athrs17_hnat_wan_mac[2] = ptr_char[4];
    nf_athrs17_hnat_wan_mac[3] = ptr_char[5];
    nf_athrs17_hnat_wan_mac[4] = ptr_char[6];
    nf_athrs17_hnat_wan_mac[5] = ptr_char[7];

	return count;
}

static ssize_t napt_peer_mac2_get(struct device *dev,
		  struct device_attribute *attr,
		  char *buf)
{
	ssize_t count;
	unsigned char* data;

	data = (unsigned char*)&nf_athrs17_hnat_ppp_peer_mac2;

	count = snprintf(buf, (ssize_t)PAGE_SIZE, "%.2x-%.2x-%.2x-%.2x-%.2x-%.2x",
					data[0], data[1], data[2], data[3], data[4], data[5]);
	return count;
}

static ssize_t napt_peer_mac2_set(struct device *dev,
		  struct device_attribute *attr,
		  const char *buf, size_t count)
{
	char num_buf[32];
	unsigned long long prv_data;
	unsigned char *ptr_char;
	int res = 0;

	if (count >= sizeof(num_buf)) return 0;
	memcpy(num_buf, buf, count);
	num_buf[count] = '\0';
	res = kstrtoll((const char *)num_buf, 16, &prv_data);
	if(res < 0)
		return res;

	prv_data = cpu_to_be64p(&prv_data);
	ptr_char = (unsigned char *)&prv_data;
	nf_athrs17_hnat_ppp_peer_mac2[0] = ptr_char[2];
    nf_athrs17_hnat_ppp_peer_mac2[1] = ptr_char[3];
    nf_athrs17_hnat_ppp_peer_mac2[2] = ptr_char[4];
    nf_athrs17_hnat_ppp_peer_mac2[3] = ptr_char[5];
    nf_athrs17_hnat_ppp_peer_mac2[4] = ptr_char[6];
    nf_athrs17_hnat_ppp_peer_mac2[5] = ptr_char[7];

	return count;
}

static ssize_t napt_ppp_id2_get(struct device *dev,
		  struct device_attribute *attr,
		  char *buf)
{
	ssize_t count;
	a_uint32_t num;

	num = (a_uint32_t)nf_athrs17_hnat_ppp_id2;

	count = snprintf(buf, (ssize_t)PAGE_SIZE, "%u", num);
	return count;
}

static ssize_t napt_ppp_id2_set(struct device *dev,
		  struct device_attribute *attr,
		  const char *buf, size_t count)
{
	char num_buf[12];
	a_uint32_t num;


	if (count >= sizeof(num_buf)) return 0;
	memcpy(num_buf, buf, count);
	num_buf[count] = '\0';
	sscanf(num_buf, "%u", &num);

	nf_athrs17_hnat_ppp_id2 = num;

	return count;
}

static ssize_t napt_sync_counter_en_get(struct device *dev,
		  struct device_attribute *attr,
		  char *buf)
{
	ssize_t count;
	a_uint32_t num;

	num = (a_uint32_t)nf_athrs17_hnat_sync_counter_en;

	count = snprintf(buf, (ssize_t)PAGE_SIZE, "%u", num);
	return count;
}

static ssize_t napt_sync_counter_en_set(struct device *dev,
		  struct device_attribute *attr,
		  const char *buf, size_t count)
{
	char num_buf[12];
	a_uint32_t num;


	if (count >= sizeof(num_buf)) return 0;
	memcpy(num_buf, buf, count);
	num_buf[count] = '\0';
	sscanf(num_buf, "%u", &num);

	nf_athrs17_hnat_sync_counter_en = num;

	return count;
}

static ssize_t napt_log_en_get(struct device *dev,
		  struct device_attribute *attr,
		  char *buf)
{
	ssize_t count;
	a_uint32_t num;

	num = (a_uint32_t)hnat_log_en;

	count = snprintf(buf, (ssize_t)PAGE_SIZE, "%u", num);
	return count;
}

static ssize_t napt_log_en_set(struct device *dev,
		  struct device_attribute *attr,
		  const char *buf, size_t count)
{
	char num_buf[12];
	a_uint32_t num;


	if (count >= sizeof(num_buf)) return 0;
	memcpy(num_buf, buf, count);
	num_buf[count] = '\0';
	sscanf(num_buf, "%u", &num);

	hnat_log_en = num;

	return count;
}

static ssize_t napt_scan_period_get(struct device *dev,
		  struct device_attribute *attr,
		  char *buf)
{
	ssize_t count;
	a_uint32_t num;

	num = (a_uint32_t)scan_period;

	count = snprintf(buf, (ssize_t)PAGE_SIZE, "%u", num);
	return count;
}

static ssize_t napt_scan_period_set(struct device *dev,
		  struct device_attribute *attr,
		  const char *buf, size_t count)
{
	char num_buf[12];
	a_uint32_t num;


	if (count >= sizeof(num_buf)) return 0;
	memcpy(num_buf, buf, count);
	num_buf[count] = '\0';
	sscanf(num_buf, "%u", &num);

	scan_period = num;

	return count;
}

static ssize_t napt_scan_enable_get(struct device *dev,
		  struct device_attribute *attr,
		  char *buf)
{
	ssize_t count;
	a_uint32_t num;

	num = (a_uint32_t)scan_enable;

	count = snprintf(buf, (ssize_t)PAGE_SIZE, "%u", num);
	return count;
}

static ssize_t napt_scan_enable_set(struct device *dev,
		  struct device_attribute *attr,
		  const char *buf, size_t count)
{
	char num_buf[12];
	a_uint32_t num;


	if (count >= sizeof(num_buf)) return 0;
	memcpy(num_buf, buf, count);
	num_buf[count] = '\0';
	sscanf(num_buf, "%u", &num);

	scan_enable = num;

	return count;
}

static ssize_t napt_need_clean_get(struct device *dev,
		  struct device_attribute *attr,
		  char *buf)
{
	ssize_t count;
	a_uint32_t num;

	num = (a_uint32_t)napt_need_clean;

	count = snprintf(buf, (ssize_t)PAGE_SIZE, "%u", num);
	return count;
}

static ssize_t napt_need_clean_set(struct device *dev,
		  struct device_attribute *attr,
		  const char *buf, size_t count)
{
	char num_buf[12];
	a_uint32_t num;


	if (count >= sizeof(num_buf)) return 0;
	memcpy(num_buf, buf, count);
	num_buf[count] = '\0';
	sscanf(num_buf, "%u", &num);

	napt_need_clean = num;

	return count;
}

static ssize_t napt_wan_switch_get(struct device *dev,
		  struct device_attribute *attr,
		  char *buf)
{
	ssize_t count;
	a_uint32_t num;

	num = (a_uint32_t)wan_switch;

	count = snprintf(buf, (ssize_t)PAGE_SIZE, "%u", num);
	return count;
}

static ssize_t napt_wan_switch_set(struct device *dev,
		  struct device_attribute *attr,
		  const char *buf, size_t count)
{
	char num_buf[12];
	a_uint32_t num;


	if (count >= sizeof(num_buf)) return 0;
	memcpy(num_buf, buf, count);
	num_buf[count] = '\0';
	sscanf(num_buf, "%u", &num);

	wan_switch = num;
	napt_wan_switch_prehandle();

	return count;
}

static ssize_t napt_wan_port_get(struct device *dev,
		  struct device_attribute *attr,
		  char *buf)
{
	ssize_t count;
	a_uint32_t num;

	num = (a_uint32_t)nat_wan_port;

	count = snprintf(buf, (ssize_t)PAGE_SIZE, "%u", num);
	return count;
}

static ssize_t napt_wan_port_set(struct device *dev,
		  struct device_attribute *attr,
		  const char *buf, size_t count)
{
	char num_buf[12];
	a_uint32_t num;


	if (count >= sizeof(num_buf)) return 0;
	memcpy(num_buf, buf, count);
	num_buf[count] = '\0';
	sscanf(num_buf, "%u", &num);

	nat_wan_port = num;

	return count;
}

static ssize_t napt_thresh_base_get(struct device *dev,
		  struct device_attribute *attr,
		  char *buf)
{
	ssize_t count;
	a_uint32_t num;

	num = packet_thres_base;

	count = snprintf(buf, (ssize_t)PAGE_SIZE, "%u", num);
	return count;
}

static ssize_t napt_thresh_base_set(struct device *dev,
		  struct device_attribute *attr,
		  const char *buf, size_t count)
{
	char num_buf[12];
	a_uint32_t num;


	if (count >= sizeof(num_buf)) return 0;
	memcpy(num_buf, buf, count);
	num_buf[count] = '\0';
	sscanf(num_buf, "%u", &num);

	packet_thres_base = num;

	return count;
}

static ssize_t napt_polling_quota_get(struct device *dev,
		  struct device_attribute *attr,
		  char *buf)
{
	ssize_t count;
	a_uint32_t num;

	num = polling_quota;

	count = snprintf(buf, (ssize_t)PAGE_SIZE, "%u", num);
	return count;
}

static ssize_t napt_polling_quota_set(struct device *dev,
		  struct device_attribute *attr,
		  const char *buf, size_t count)
{
	char num_buf[12];
	a_uint32_t num;


	if (count >= sizeof(num_buf)) return 0;
	memcpy(num_buf, buf, count);
	num_buf[count] = '\0';
	sscanf(num_buf, "%u", &num);

	polling_quota = num;

	return count;
}

static ssize_t napt_bypass_check_get(struct device *dev,
		  struct device_attribute *attr,
		  char *buf)
{
	ssize_t count;
	a_uint32_t num;

	num = napt_add_bypass_check;

	count = snprintf(buf, (ssize_t)PAGE_SIZE, "%u", num);
	return count;
}

static ssize_t napt_bypass_check_set(struct device *dev,
		  struct device_attribute *attr,
		  const char *buf, size_t count)
{
	char num_buf[12];
	a_uint32_t num;


	if (count >= sizeof(num_buf)) return 0;
	memcpy(num_buf, buf, count);
	num_buf[count] = '\0';
	sscanf(num_buf, "%u", &num);

	napt_add_bypass_check = num;

	return count;
}

extern void napt_helper_show(void);
static ssize_t napt_log_show_get(struct device *dev,
		  struct device_attribute *attr,
		  char *buf)
{
	napt_helper_show();
	return 0;
}

struct kobject *napt_sys = NULL;
static const struct device_attribute napt_hnat_attr =
	__ATTR(hnat, 0660, napt_hnat_get, napt_hnat_set);
static const struct device_attribute napt_wan_type_attr =
	__ATTR(wan_type, 0660, napt_wan_type_get, napt_wan_type_set);
static const struct device_attribute napt_ppp_id_attr =
	__ATTR(ppp_id, 0660, napt_ppp_id_get, napt_ppp_id_set);
static const struct device_attribute napt_udp_thresh_attr =
	__ATTR(udp_thresh, 0660, napt_udp_thresh_get, napt_udp_thresh_set);
static const struct device_attribute napt_wan_ip_attr =
	__ATTR(wan_ip, 0660, napt_wan_ip_get, napt_wan_ip_set);
static const struct device_attribute napt_ppp_peer_ip_attr =
	__ATTR(peer_ip, 0660, napt_ppp_peer_ip_get, napt_ppp_peer_ip_set);
static const struct device_attribute napt_ppp_peer_mac_attr =
	__ATTR(peer_mac, 0660, napt_peer_mac_get, napt_peer_mac_set);
static const struct device_attribute napt_wan_mac_attr =
	__ATTR(wan_mac, 0660, napt_wan_mac_get, napt_wan_mac_set);
static const struct device_attribute napt_ppp_id2_attr =
	__ATTR(ppp_id2, 0660, napt_ppp_id2_get, napt_ppp_id2_set);
static const struct device_attribute napt_ppp_peer_mac2_attr =
	__ATTR(peer_mac2, 0660, napt_peer_mac2_get, napt_peer_mac2_set);
static const struct device_attribute napt_sync_counter_en_attr =
	__ATTR(sync_counter_en, 0660, napt_sync_counter_en_get, napt_sync_counter_en_set);
static const struct device_attribute napt_log_en_attr =
	__ATTR(log_en, 0660, napt_log_en_get, napt_log_en_set);
static const struct device_attribute napt_log_show_attr =
	__ATTR(log_show, 0660, napt_log_show_get, NULL);
static const struct device_attribute napt_scan_period_attr =
	__ATTR(speriod, 0660, napt_scan_period_get, napt_scan_period_set);
static const struct device_attribute napt_scan_enable_attr =
	__ATTR(scan_en, 0660, napt_scan_enable_get, napt_scan_enable_set);
static const struct device_attribute napt_need_clean_attr =
	__ATTR(napt_clean, 0660, napt_need_clean_get, napt_need_clean_set);
static const struct device_attribute napt_wan_switch_attr =
	__ATTR(napt_switch, 0660, napt_wan_switch_get, napt_wan_switch_set);
static const struct device_attribute napt_wan_port_attr =
	__ATTR(wan_port, 0660, napt_wan_port_get, napt_wan_port_set);
static const struct device_attribute napt_thresh_base_attr =
	__ATTR(thresh_base, 0660, napt_thresh_base_get, napt_thresh_base_set);
static const struct device_attribute napt_polling_quota_attr =
	__ATTR(ct_quota, 0660, napt_polling_quota_get, napt_polling_quota_set);
static const struct device_attribute napt_bypass_check_attr =
	__ATTR(bypass_check, 0660, napt_bypass_check_get, napt_bypass_check_set);

int napt_procfs_init(void)
{
	int ret = 0;

	napt_sys = kobject_create_and_add("ssdk_napt", NULL);
	if (!napt_sys) {
		printk("napt failed to register sysfs\n ");
		return ret;
	}



	ret = sysfs_create_file(napt_sys, &napt_hnat_attr.attr);
	if (ret) {
		printk("Failed to register hnat SysFS file\n");
		goto CLEANUP_1;
	}
	ret = sysfs_create_file(napt_sys, &napt_wan_type_attr.attr);
	if (ret) {
		printk("Failed to register wan type SysFS file\n");
		goto CLEANUP_2;
	}
	ret = sysfs_create_file(napt_sys, &napt_ppp_id_attr.attr);
	if (ret) {
		printk("Failed to register ppp id SysFS file\n");
		goto CLEANUP_3;
	}
	ret = sysfs_create_file(napt_sys, &napt_udp_thresh_attr.attr);
	if (ret) {
		printk("Failed to register udp thresh SysFS file\n");
		goto CLEANUP_4;
	}
	ret = sysfs_create_file(napt_sys, &napt_wan_ip_attr.attr);
	if (ret) {
		printk("Failed to register wan ip SysFS file\n");
		goto CLEANUP_5;
	}
	ret = sysfs_create_file(napt_sys, &napt_ppp_peer_ip_attr.attr);
	if (ret) {
		printk("Failed to register ppp peer ip SysFS file\n");
		goto CLEANUP_6;
	}
	ret = sysfs_create_file(napt_sys, &napt_ppp_peer_mac_attr.attr);
	if (ret) {
		printk("Failed to register ppp peer mac SysFS file\n");
		goto CLEANUP_7;
	}
	ret = sysfs_create_file(napt_sys, &napt_wan_mac_attr.attr);
	if (ret) {
		printk("Failed to register wan mac SysFS file\n");
		goto CLEANUP_8;
	}
	ret = sysfs_create_file(napt_sys, &napt_ppp_id2_attr.attr);
	if (ret) {
		printk("Failed to register ppp id2 SysFS file\n");
		goto CLEANUP_9;
	}
	ret = sysfs_create_file(napt_sys, &napt_ppp_peer_mac2_attr.attr);
	if (ret) {
		printk("Failed to register ppp peer mac2 SysFS file\n");
		goto CLEANUP_10;
	}
	ret = sysfs_create_file(napt_sys, &napt_sync_counter_en_attr.attr);
	if (ret) {
		printk("Failed to register sync counter en SysFS file\n");
		goto CLEANUP_11;
	}
	ret = sysfs_create_file(napt_sys, &napt_log_en_attr.attr);
	if (ret) {
		printk("Failed to register log en SysFS file\n");
		goto CLEANUP_12;
	}
	ret = sysfs_create_file(napt_sys, &napt_log_show_attr.attr);
	if (ret) {
		printk("Failed to register log show SysFS file\n");
		goto CLEANUP_13;
	}
	ret = sysfs_create_file(napt_sys, &napt_scan_period_attr.attr);
	if (ret) {
		printk("Failed to register scan period SysFS file\n");
		goto CLEANUP_14;
	}
	ret = sysfs_create_file(napt_sys, &napt_scan_enable_attr.attr);
	if (ret) {
		printk("Failed to register scan enable SysFS file\n");
		goto CLEANUP_15;
	}
	ret = sysfs_create_file(napt_sys, &napt_need_clean_attr.attr);
	if (ret) {
		printk("Failed to register napt clean SysFS file\n");
		goto CLEANUP_16;
	}
	ret = sysfs_create_file(napt_sys, &napt_wan_switch_attr.attr);
	if (ret) {
		printk("Failed to register napt wan switch SysFS file\n");
		goto CLEANUP_17;
	}
	ret = sysfs_create_file(napt_sys, &napt_wan_port_attr.attr);
	if (ret) {
		printk("Failed to register napt wan port SysFS file\n");
		goto CLEANUP_18;
	}
	ret = sysfs_create_file(napt_sys, &napt_thresh_base_attr.attr);
	if (ret) {
		printk("Failed to register napt thresh base SysFS file\n");
		goto CLEANUP_19;
	}
	ret = sysfs_create_file(napt_sys, &napt_polling_quota_attr.attr);
	if (ret) {
		printk("Failed to register napt polling quota SysFS file\n");
		goto CLEANUP_20;
	}
	ret = sysfs_create_file(napt_sys, &napt_bypass_check_attr.attr);
	if (ret) {
		printk("Failed to register napt add check bypass SysFS file\n");
		goto CLEANUP_21;
	}
	return 0;
CLEANUP_21:
	sysfs_remove_file(napt_sys, &napt_polling_quota_attr.attr);
CLEANUP_20:
	sysfs_remove_file(napt_sys, &napt_thresh_base_attr.attr);
CLEANUP_19:
	sysfs_remove_file(napt_sys, &napt_wan_port_attr.attr);
CLEANUP_18:
	sysfs_remove_file(napt_sys, &napt_wan_switch_attr.attr);
CLEANUP_17:
	sysfs_remove_file(napt_sys, &napt_need_clean_attr.attr);
CLEANUP_16:
	sysfs_remove_file(napt_sys, &napt_scan_enable_attr.attr);
CLEANUP_15:
	sysfs_remove_file(napt_sys, &napt_scan_period_attr.attr);
CLEANUP_14:
	sysfs_remove_file(napt_sys, &napt_log_show_attr.attr);
CLEANUP_13:
	sysfs_remove_file(napt_sys, &napt_log_en_attr.attr);
CLEANUP_12:
	sysfs_remove_file(napt_sys, &napt_sync_counter_en_attr.attr);
CLEANUP_11:
	sysfs_remove_file(napt_sys, &napt_ppp_peer_mac2_attr.attr);
CLEANUP_10:
	sysfs_remove_file(napt_sys, &napt_ppp_id2_attr.attr);
CLEANUP_9:
	sysfs_remove_file(napt_sys, &napt_wan_mac_attr.attr);
CLEANUP_8:
	sysfs_remove_file(napt_sys, &napt_ppp_peer_mac_attr.attr);
CLEANUP_7:
	sysfs_remove_file(napt_sys, &napt_ppp_peer_ip_attr.attr);
CLEANUP_6:
	sysfs_remove_file(napt_sys, &napt_wan_ip_attr.attr);
CLEANUP_5:
	sysfs_remove_file(napt_sys, &napt_udp_thresh_attr.attr);
CLEANUP_4:
	sysfs_remove_file(napt_sys, &napt_ppp_id_attr.attr);
CLEANUP_3:
	sysfs_remove_file(napt_sys, &napt_wan_type_attr.attr);
CLEANUP_2:
	sysfs_remove_file(napt_sys, &napt_hnat_attr.attr);
CLEANUP_1:
	kobject_put(napt_sys);

	return ret;
}

void napt_procfs_exit(void)
{
	printk("napt procfs exit\n");

	sysfs_remove_file(napt_sys, &napt_bypass_check_attr.attr);
	sysfs_remove_file(napt_sys, &napt_polling_quota_attr.attr);
	sysfs_remove_file(napt_sys, &napt_thresh_base_attr.attr);
	sysfs_remove_file(napt_sys, &napt_wan_port_attr.attr);
	sysfs_remove_file(napt_sys, &napt_wan_switch_attr.attr);
	sysfs_remove_file(napt_sys, &napt_need_clean_attr.attr);
	sysfs_remove_file(napt_sys, &napt_scan_enable_attr.attr);
	sysfs_remove_file(napt_sys, &napt_scan_period_attr.attr);
	sysfs_remove_file(napt_sys, &napt_log_show_attr.attr);
	sysfs_remove_file(napt_sys, &napt_log_en_attr.attr);
	sysfs_remove_file(napt_sys, &napt_sync_counter_en_attr.attr);
	sysfs_remove_file(napt_sys, &napt_ppp_peer_mac2_attr.attr);
	sysfs_remove_file(napt_sys, &napt_ppp_id2_attr.attr);
	sysfs_remove_file(napt_sys, &napt_wan_mac_attr.attr);
	sysfs_remove_file(napt_sys, &napt_ppp_peer_mac_attr.attr);
	sysfs_remove_file(napt_sys, &napt_ppp_peer_ip_attr.attr);
	sysfs_remove_file(napt_sys, &napt_wan_ip_attr.attr);
	sysfs_remove_file(napt_sys, &napt_udp_thresh_attr.attr);
	sysfs_remove_file(napt_sys, &napt_ppp_id_attr.attr);
	sysfs_remove_file(napt_sys, &napt_wan_type_attr.attr);
	sysfs_remove_file(napt_sys, &napt_hnat_attr.attr);

	kobject_put(napt_sys);
}


#endif


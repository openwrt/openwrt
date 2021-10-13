/*
 **************************************************************************
 * Copyright (c) 2014-2018, 2020, The Linux Foundation. All rights reserved.
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
 **************************************************************************
 */
/*
 * nss_log.c
 *	NSS FW debug logger retrieval from DDR (memory)
 *
 */
#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/errno.h>
#include <linux/mm.h>
#include <linux/fs.h>
#include <linux/miscdevice.h>
#include <linux/posix-timers.h>
#include <linux/interrupt.h>
#include <linux/time.h>
#include <linux/platform_device.h>
#include <linux/device.h>
#include <nss_hal.h>
#include "nss_core.h"
#include "nss_log.h"

/*
 * Private data for each device file open instance
 */
struct nss_log_data {
	void *load_mem;		/* Pointer to struct nss_log_descriptor - descriptor data */
	dma_addr_t dma_addr;	/* Handle to DMA */
	uint32_t last_entry;	/* Last known sampled entry (or index) */
	uint32_t nentries;	/* Caches the total number of entries of log buffer */
	int nss_id;		/* NSS Core id being used */
	struct nss_ctx_instance *nss_ctx;
				/* NSS ctx instance */
};

struct nss_log_ring_buffer_addr nss_rbe[NSS_MAX_CORES];

static DEFINE_MUTEX(nss_log_mutex);
static wait_queue_head_t nss_log_wq;
static nss_log_msg_callback_t nss_debug_interface_cb;
static void *nss_debug_interface_app_data = NULL;

static wait_queue_head_t msg_wq;
enum nss_cmn_response msg_response;
static bool msg_event;

/*
 * nss_log_llseek()
 *	Seek operation.
 */
static loff_t nss_log_llseek(struct file *file, loff_t offset, int origin)
{
	struct nss_log_data *data = file->private_data;

	switch (origin) {
	case SEEK_SET:
		break;
	case SEEK_CUR:
		offset += file->f_pos;
		break;
	case SEEK_END:
		offset = ((data->nentries * sizeof(struct nss_log_entry)) + sizeof(struct nss_log_descriptor)) - offset;
		break;
	default:
		return -EINVAL;
	}

	return (offset >= 0) ? (file->f_pos = offset) : -EINVAL;
}

/*
 * nss_log_open()
 *	Open operation for our device. We let as many instance run together
 */
static int nss_log_open(struct inode *inode, struct file *filp)
{
	struct nss_log_data *data = NULL;
	struct nss_top_instance *nss_top;
	struct nss_ctx_instance *nss_ctx;
	int nss_id;

	/*
	 * i_private is passed to us by debug_fs_create()
	 */
	nss_id = (int)(nss_ptr_t)inode->i_private;
	if (nss_id < 0 || nss_id >= nss_top_main.num_nss) {
		nss_warning("nss_id is not valid :%d\n", nss_id);
		return -ENODEV;
	}

	nss_top = &nss_top_main;
	nss_ctx = &nss_top->nss[nss_id];

	data = kzalloc(sizeof(struct nss_log_data), GFP_KERNEL);
	if (!data) {
		nss_warning("%px: Failed to allocate memory for log_data", nss_ctx);
		return -ENOMEM;
	}

	mutex_lock(&nss_log_mutex);
	if (!nss_rbe[nss_id].addr) {
		mutex_unlock(&nss_log_mutex);
		kfree(data);
		nss_warning("%px: Ring buffer not configured yet for nss_id:%d", nss_ctx, nss_id);
		return -EIO;
	}

	/*
	 * Actual ring buffer.
	 */
	data->load_mem = nss_rbe[nss_id].addr;
	data->last_entry = 0;
	data->nentries = nss_rbe[nss_id].nentries;
	data->dma_addr = nss_rbe[nss_id].dma_addr;
	data->nss_ctx = nss_ctx;

	/*
	 * Increment the reference count so that we don't free
	 * the memory
	 */
	nss_rbe[nss_id].ref_cnt++;
	data->nss_id = nss_id;
	filp->private_data = data;
	mutex_unlock(&nss_log_mutex);

	return 0;
}

/*
 * nss_log_release()
 *	release gets called when close() is called on the file
 *	descriptor. We unmap the IO region.
 */
static int nss_log_release(struct inode *inode, struct file *filp)
{
	struct nss_log_data *data = filp->private_data;

	if (!data) {
		return -EINVAL;
	}

	mutex_lock(&nss_log_mutex);
	nss_rbe[data->nss_id].ref_cnt--;
	BUG_ON(nss_rbe[data->nss_id].ref_cnt < 0);
	if (!nss_rbe[data->nss_id].ref_cnt) {
		wake_up(&nss_log_wq);
	}
	mutex_unlock(&nss_log_mutex);
	kfree(data);
	return 0;
}

/*
 * nss_log_current_entry()
 *	Reads current entry index from NSS log descriptor.
 */
static uint32_t nss_log_current_entry(struct nss_log_descriptor *desc)
{
	rmb();
	return desc->current_entry;
}

/*
 * nss_log_read()
 *	Read operation lets command like cat and tail read our memory log buffer data.
 */
static ssize_t nss_log_read(struct file *filp, char __user *buf, size_t size, loff_t *ppos)
{
	struct nss_log_data *data = filp->private_data;
	struct nss_log_descriptor *desc;
	size_t bytes = 0;
	size_t b;
	struct nss_log_entry *rb;
	uint32_t entry;
	uint32_t offset, index;
	char msg[NSS_LOG_OUTPUT_LINE_SIZE];

	if (!data) {
		return -EINVAL;
	}

	desc = data->load_mem;
	if (!desc) {
		nss_warning("%px: load_mem is NULL", data);
		return -EINVAL;
	}

	/*
	 * If buffer is too small to fit even one entry.
	 */
	if (size < NSS_LOG_OUTPUT_LINE_SIZE) {
		return 0;
	}

	/*
	 * Get the current index
	 */
	dma_sync_single_for_cpu(data->nss_ctx->dev, data->dma_addr, sizeof(struct nss_log_descriptor), DMA_FROM_DEVICE);

	entry = nss_log_current_entry(desc);

	/*
	 * If the current and last sampled indexes are same then bail out.
	 */
	if (unlikely(data->last_entry == entry)) {
		return 0;
	}

	/*
	 * If this is the first read (after open) on our device file.
	 */
	if (unlikely(!(*ppos))) {
		/*
		 * If log buffer has rolled over. Almost all the time
		 * it will be true.
		 */
		if (likely(entry > data->nentries)) {
			/*
			 * Determine how much we can stuff in one
			 * buffer passed to us and accordingly
			 * reduce our index.
			 */
			data->last_entry = entry - data->nentries;
		} else {
			data->last_entry = 0;
		}
	} else if (unlikely(entry > data->nentries && ((entry - data->nentries) > data->last_entry))) {
		/*
		 * If FW is producing debug buffer at a pace faster than
		 * we can consume, then we restrict our iteration.
		 */
		data->last_entry = entry - data->nentries;
	}

	/*
	 * Iterate over indexes.
	 */
	while (entry > data->last_entry) {
		index = offset = (data->last_entry % data->nentries);
		offset = (offset * sizeof(struct nss_log_entry))
			 + offsetof(struct nss_log_descriptor, log_ring_buffer);

		dma_sync_single_for_cpu(data->nss_ctx->dev, data->dma_addr + offset,
			sizeof(struct nss_log_entry), DMA_FROM_DEVICE);
		rb = &desc->log_ring_buffer[index];

		b = scnprintf(msg, sizeof(msg), NSS_LOG_LINE_FORMAT,
			rb->thread_num, rb->timestamp, rb->message);

		data->last_entry++;

		/*
		 * Copy to user buffer and if we fail then we return
		 * failure.
		 */
		if (copy_to_user(buf + bytes, msg, b)) {
			return -EFAULT;
		}

		bytes += b;

		/*
		 * If we ran out of space in the buffer.
		 */
		if ((bytes + NSS_LOG_OUTPUT_LINE_SIZE) >= size)
			break;
	}

	if (bytes > 0)
		*ppos =  bytes;

	return bytes;
}

struct file_operations nss_logs_core_ops = {
	.owner = THIS_MODULE,
	.open = nss_log_open,
	.read = nss_log_read,
	.release = nss_log_release,
	.llseek = nss_log_llseek,
};

/*
 * nss_debug_interface_set_callback()
 *	Sets the callback
 */
void nss_debug_interface_set_callback(nss_log_msg_callback_t cb, void *app_data)
{
	nss_debug_interface_cb = cb;
	nss_debug_interface_app_data = app_data;
}

/*
 * nss_debug_interface_event()
 *	Received an event from NSS FW
 */
static void nss_debug_interface_event(void *app_data, struct nss_log_debug_interface_msg *nim)
{
	struct nss_cmn_msg *ncm = (struct nss_cmn_msg *)nim;

	msg_response = ncm->response;
	msg_event = true;
	wake_up(&msg_wq);
}

/*
 * nss_debug_interface_handler()
 *	handle NSS -> HLOS messages for debug interfaces
 */
static void nss_debug_interface_handler(struct nss_ctx_instance *nss_ctx, struct nss_cmn_msg *ncm, __attribute__((unused))void *app_data)
{
	struct nss_log_debug_interface_msg *ntm = (struct nss_log_debug_interface_msg *)ncm;
	nss_log_msg_callback_t cb;

	BUG_ON(ncm->interface != NSS_DEBUG_INTERFACE);

	/*
	 * Is this a valid request/response packet?
	 */
	if (ncm->type > NSS_DEBUG_INTERFACE_TYPE_MAX) {
		nss_warning("%px: received invalid message %d for CAPWAP interface", nss_ctx, ncm->type);
		return;
	}

	if (nss_cmn_get_msg_len(ncm) > sizeof(struct nss_log_debug_interface_msg)) {
		nss_warning("%px: Length of message is greater than required: %d", nss_ctx, nss_cmn_get_msg_len(ncm));
		return;
	}

	nss_core_log_msg_failures(nss_ctx, ncm);

	/*
	 * Update the callback and app_data for NOTIFY messages.
	 */
	if (ncm->response == NSS_CMN_RESPONSE_NOTIFY) {
		ncm->cb = (nss_ptr_t)nss_debug_interface_cb;
		ncm->app_data = (nss_ptr_t)nss_debug_interface_app_data;
	}

	/*
	 * Do we have a callback
	 */
	if (!ncm->cb) {
		nss_trace("%px: cb is null for interface %d", nss_ctx, ncm->interface);
		return;
	}

	cb = (nss_log_msg_callback_t)ncm->cb;
	cb((void *)ncm->app_data, ntm);
}

/*
 * nss_debug_interface_tx()
 *	Transmit a debug interface message to NSS FW
 */
static nss_tx_status_t nss_debug_interface_tx(struct nss_ctx_instance *nss_ctx, struct nss_log_debug_interface_msg *msg)
{
	struct nss_cmn_msg *ncm = &msg->cm;

	/*
	 * Sanity check the message
	 */
	if (ncm->interface != NSS_DEBUG_INTERFACE) {
		nss_warning("%px: tx request for another interface: %d", nss_ctx, ncm->interface);
		return NSS_TX_FAILURE;
	}

	if (ncm->type > NSS_DEBUG_INTERFACE_TYPE_MAX) {
		nss_warning("%px: message type out of range: %d", nss_ctx, ncm->type);
		return NSS_TX_FAILURE;
	}

	return nss_core_send_cmd(nss_ctx, msg, sizeof(*msg), NSS_NBUF_PAYLOAD_SIZE);
}

/*
 * nss_debug_log_buffer_alloc()
 *	Allocates and Initializes log buffer for the use in NSS FW (logging)
 */
bool nss_debug_log_buffer_alloc(uint8_t nss_id, uint32_t nentry)
{
	struct nss_log_debug_interface_msg msg;
	struct nss_log_debug_memory_msg *dbg;
	struct nss_top_instance *nss_top;
	struct nss_ctx_instance *nss_ctx;
	dma_addr_t dma_addr;
	uint32_t size;
	void *addr = NULL;
	nss_tx_status_t status;

	if (nss_id >= nss_top_main.num_nss) {
		return false;
	}

	nss_top = &nss_top_main;
	nss_ctx = &nss_top->nss[nss_id];

	if (nss_ctx->state != NSS_CORE_STATE_INITIALIZED) {
		nss_warning("%px: NSS Core:%d is not initialized yet\n", nss_ctx, nss_id);
		return false;
	}

	size = sizeof(struct nss_log_descriptor) + (sizeof(struct nss_log_entry) * nentry);
	addr = kmalloc(size, GFP_ATOMIC);
	if (!addr) {
		nss_warning("%px: Failed to allocate memory for logging (size:%d)\n", nss_ctx, size);
		return false;
	}

	memset(addr, 0, size);
	dma_addr = (uint32_t)dma_map_single(nss_ctx->dev, addr, size, DMA_FROM_DEVICE);
	if (unlikely(dma_mapping_error(nss_ctx->dev, dma_addr))) {
		nss_warning("%px: Failed to map address in DMA", nss_ctx);
		kfree(addr);
		return false;
	}

	/*
	 * If we already have ring buffer associated with nss_id, then
	 * we must wait before we attach a new ring buffer.
	 */
	mutex_lock(&nss_log_mutex);
	if (nss_rbe[nss_id].addr) {
		mutex_unlock(&nss_log_mutex);

		/*
		 * Someone is using the current logbuffer. Wait until ref count become 0.
		 * We have to return mutex here, because the current user requires it to
		 * release the reference.
		 */
		if (!wait_event_timeout(nss_log_wq, !nss_rbe[nss_id].ref_cnt, 5 * HZ)) {
			nss_warning("%px: Timeout waiting for refcnt to become 0\n", nss_ctx);
			goto fail;
		}

		mutex_lock(&nss_log_mutex);
		if (!nss_rbe[nss_id].addr) {
			mutex_unlock(&nss_log_mutex);
			goto fail;
		}
		if (nss_rbe[nss_id].ref_cnt > 0) {
			mutex_unlock(&nss_log_mutex);
			nss_warning("%px: Some other thread is contending..opting out\n", nss_ctx);
			goto fail;
		}
	}

	memset(&msg, 0, sizeof(struct nss_log_debug_interface_msg));
	nss_cmn_msg_init(&msg.cm, NSS_DEBUG_INTERFACE, NSS_DEBUG_INTERFACE_TYPE_LOG_BUF_INIT,
		sizeof(struct nss_log_debug_memory_msg), nss_debug_interface_event, NULL);

	dbg = &msg.msg.addr;
	dbg->nentry = nentry;
	dbg->version = NSS_DEBUG_LOG_VERSION;
	dbg->phy_addr = dma_addr;

	msg_event = false;
	status = nss_debug_interface_tx(nss_ctx, &msg);
	if (status != NSS_TX_SUCCESS) {
		mutex_unlock(&nss_log_mutex);
		nss_warning("%px: Failed to send message to debug interface:%d\n", nss_ctx, status);
		goto fail;
	}

	/*
	 * Wait for 5 seconds since this is a critical operation.
	 * Mutex is not unlocked here because we do not want someone to acquire the mutex and use the logbuffer
	 * while we are waiting message from NSS.
	 */
	if (!wait_event_timeout(msg_wq, msg_event, 5 * HZ)) {
		mutex_unlock(&nss_log_mutex);
		nss_warning("%px: Timeout send message to debug interface\n", nss_ctx);
		goto fail;
	}

	if (msg_response != NSS_CMN_RESPONSE_ACK) {
		mutex_unlock(&nss_log_mutex);
		nss_warning("%px: Response error for send message to debug interface:%d\n", nss_ctx, msg_response);
		goto fail;
	}

	/*
	 * If we had to free the previous allocation for ring buffer.
	 */
	if (nss_rbe[nss_id].addr) {
		uint32_t old_size;
		old_size = sizeof(struct nss_log_descriptor) +
			(sizeof(struct nss_log_entry) * nss_rbe[nss_id].nentries);
		dma_unmap_single(nss_ctx->dev, nss_rbe[nss_id].dma_addr, old_size, DMA_FROM_DEVICE);
		kfree(nss_rbe[nss_id].addr);
	}

	nss_rbe[nss_id].addr = addr;
	nss_rbe[nss_id].nentries = nentry;
	nss_rbe[nss_id].ref_cnt = 0;
	nss_rbe[nss_id].dma_addr = dma_addr;
	mutex_unlock(&nss_log_mutex);
	wake_up(&nss_log_wq);
	return true;

fail:
	dma_unmap_single(nss_ctx->dev, dma_addr, size, DMA_FROM_DEVICE);
	kfree(addr);
	wake_up(&nss_log_wq);
	return false;
}

/*
 * nss_logbuffer_handler()
 *	Enable NSS debug output
 */
int nss_logbuffer_handler(struct ctl_table *ctl, int write, void __user *buffer, size_t *lenp, loff_t *ppos)
{
	int ret;
	int core_status;
	int i;

	ret = proc_dointvec(ctl, write, buffer, lenp, ppos);
	if (ret)  {
		return ret;
	}

	if (!write) {
		return ret;
	}

	if (nss_ctl_logbuf < 32) {
		nss_warning("Invalid NSS FW logbuffer size:%d (must be > 32)\n", nss_ctl_logbuf);
		nss_ctl_logbuf = 0;
		return ret;
	}

	for (i = 0; i < nss_top_main.num_nss; i++) {
		/*
		 * Register the callback handler and allocate the debug log buffers
		 */
		core_status = nss_core_register_handler(&nss_top_main.nss[i], NSS_DEBUG_INTERFACE, nss_debug_interface_handler, NULL);
		if (core_status != NSS_CORE_STATUS_SUCCESS) {
			nss_warning("NSS logbuffer init failed with register handler:%d\n", core_status);
		}

		if (!nss_debug_log_buffer_alloc(i, nss_ctl_logbuf)) {
			nss_warning("%d: Failed to set debug log buffer on NSS core", i);
		}
	}

	return ret;
}

/*
 * nss_log_init()
 *	Initializes NSS FW logs retrieval logic from /sys
 */
void nss_log_init(void)
{
	int i;
	struct dentry *logs_dentry;
	struct dentry *core_log_dentry;

	memset(nss_rbe, 0, sizeof(nss_rbe));
	init_waitqueue_head(&nss_log_wq);
	init_waitqueue_head(&msg_wq);

	/*
	 * Create directory for obtaining NSS FW logs from each core
	 */
	logs_dentry = debugfs_create_dir("logs", nss_top_main.top_dentry);
	if (unlikely(!logs_dentry)) {
		nss_warning("Failed to create qca-nss-drv/logs directory in debugfs");
		return;
	}

	for (i = 0; i < nss_top_main.num_nss; i++) {
		char file[16];
		extern struct file_operations nss_logs_core_ops;

		snprintf(file, sizeof(file), "core%d", i);
		core_log_dentry = debugfs_create_file(file, 0400,
						logs_dentry, (void *)(nss_ptr_t)i, &nss_logs_core_ops);
		if (unlikely(!core_log_dentry)) {
			nss_warning("Failed to create qca-nss-drv/logs/%s file in debugfs", file);
			return;
		}
	}

	nss_debug_interface_set_callback(nss_debug_interface_event, NULL);
}

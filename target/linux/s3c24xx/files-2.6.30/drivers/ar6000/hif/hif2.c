/*
 * hif2.c - HIF layer re-implementation for the Linux SDIO stack
 *
 * Copyright (C) 2008, 2009 by OpenMoko, Inc.
 * Written by Werner Almesberger <werner@openmoko.org>
 * All Rights Reserved
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * Based on:
 *
 * @abstract: HIF layer reference implementation for Atheros SDIO stack
 * @notice: Copyright (c) 2004-2006 Atheros Communications Inc.
 */


#include <linux/kernel.h>
#include <linux/kthread.h>
#include <linux/list.h>
#include <linux/wait.h>
#include <linux/spinlock.h>
#include <linux/mutex.h>
#include <linux/sched.h>
#include <linux/mmc/sdio_func.h>
#include <linux/mmc/sdio.h>
#include <linux/mmc/sdio_ids.h>

#include "athdefs.h"
#include "a_types.h"
#include "hif.h"


/* @@@ Hack - this wants cleaning up */

#ifdef CONFIG_MACH_NEO1973_GTA02

#include <mach/gta02-pm-wlan.h>

#else /* CONFIG_MACH_NEO1973_GTA02 */

#define	gta02_wlan_query_rfkill_lock()  1
#define	gta02_wlan_set_rfkill_cb(cb, hif) ((void) cb)
#define	gta02_wlan_query_rfkill_unlock()
#define	gta02_wlan_clear_rfkill_cb()

#endif /* !CONFIG_MACH_NEO1973_GTA02 */


/*
 * KNOWN BUGS:
 *
 * - HIF_DEVICE_IRQ_ASYNC_SYNC doesn't work yet (gets MMC errors)
 * - latency can reach hundreds of ms, probably because of scheduling delays
 * - packets go through about three queues before finally hitting the network
 */

/*
 * Differences from Atheros' HIFs:
 *
 * - synchronous and asynchronous requests may get reordered with respect to
 *   each other, e.g., if HIFReadWrite returns for an asynchronous request and
 *   then HIFReadWrite is called for a synchronous request, the synchronous
 *   request may be executed before the asynchronous request.
 *
 * - request queue locking seems unnecessarily complex in the Atheros HIFs.
 *
 * - Atheros mask interrupts by calling sdio_claim_irq/sdio_release_irq, which
 *   can cause quite a bit of overhead. This HIF has its own light-weight
 *   interrupt masking.
 *
 * - Atheros call deviceInsertedHandler from a thread spawned off the probe or
 *   device insertion function. The original explanation for the Atheros SDIO
 *   stack said that this is done because a delay is needed to let the chip
 *   complete initialization. There is indeed a one second delay in the thread.
 *
 *   The Atheros Linux SDIO HIF removes the delay and only retains the thread.
 *   Experimentally removing the thread didn't show any conflicts, so let's get
 *   rid of it for good.
 *
 * - The Atheros SDIO stack with Samuel's driver sets SDIO_CCCR_POWER in
 *   SDIO_POWER_EMPC. Atheros' Linux SDIO code apparently doesn't. We don't
 *   either, and this seems to work fine.
 *   @@@ Need to check this with Atheros.
 */


#define MBOXES			4

#define HIF_MBOX_BLOCK_SIZE	128
#define	HIF_MBOX_BASE_ADDR	0x800
#define	HIF_MBOX_WIDTH		0x800
#define	HIF_MBOX_START_ADDR(mbox) \
    (HIF_MBOX_BASE_ADDR+(mbox)*HIF_MBOX_WIDTH)


struct hif_device {
	void *htc_handle;
	struct sdio_func *func;

	/*
	 * @@@ our sweet little bit of bogosity - the mechanism that lets us
	 * use the SDIO stack from softirqs. This really wants to use skbs.
	 */
	struct list_head queue;
	spinlock_t queue_lock;
	struct task_struct *io_task;
	wait_queue_head_t wait;

	/*
	 * activate_lock protects "active" and the activation/deactivation
	 * process itself.
	 *
	 * Relation to other locks: The SDIO function can be claimed while
	 * activate_lock is being held, but trying to acquire activate_lock
	 * while having ownership of the SDIO function could cause a deadlock.
	 */
	int active;
	struct mutex activate_lock;
};

struct hif_request {
	struct list_head list;
	struct sdio_func *func;
	int (*read)(struct sdio_func *func,
	    void *dst, unsigned int addr, int count);
	int (*write)(struct sdio_func *func,
	    unsigned int addr, void *src, int count);
	void *buf;
	unsigned long addr;
	int len;
	A_STATUS (*completion)(void *context, A_STATUS status);
	void *context;
};


static HTC_CALLBACKS htcCallbacks;

/*
 * shutdown_lock prevents recursion through HIFShutDownDevice
 */
static DEFINE_MUTEX(shutdown_lock);


/* ----- Request processing ------------------------------------------------ */


static A_STATUS process_request(struct hif_request *req)
{
	int ret;
	A_STATUS status;

	dev_dbg(&req->func->dev, "process_request(req %p)\n", req);
	sdio_claim_host(req->func);
	if (req->read) {
		ret = req->read(req->func, req->buf, req->addr, req->len);
	} else {
		ret = req->write(req->func, req->addr, req->buf, req->len);
	}
	sdio_release_host(req->func);
	status = ret ? A_ERROR : A_OK;
	if (req->completion)
		req->completion(req->context, status);
	kfree(req);
	return status;
}


static void enqueue_request(struct hif_device *hif, struct hif_request *req)
{
	unsigned long flags;

	dev_dbg(&req->func->dev, "enqueue_request(req %p)\n", req);
	spin_lock_irqsave(&hif->queue_lock, flags);
	list_add_tail(&req->list, &hif->queue);
	spin_unlock_irqrestore(&hif->queue_lock, flags);
	wake_up(&hif->wait);
}


static struct hif_request *dequeue_request(struct hif_device *hif)
{
	struct hif_request *req;
	unsigned long flags;

	spin_lock_irqsave(&hif->queue_lock, flags);
	if (list_empty(&hif->queue))
		req = NULL;
	else {
		req = list_first_entry(&hif->queue,
		    struct hif_request, list);
		list_del(&req->list);
	}
	spin_unlock_irqrestore(&hif->queue_lock, flags);
	return req;
}


static void wait_queue_empty(struct hif_device *hif)
{
	unsigned long flags;
	int empty;

	while (1) {
		spin_lock_irqsave(&hif->queue_lock, flags);
		empty = list_empty(&hif->queue);
		spin_unlock_irqrestore(&hif->queue_lock, flags);
		if (empty)
			break;
		else
			yield();
	}
}


static int io(void *data)
{
	struct hif_device *hif = data;
	struct sched_param param = { .sched_priority = 2 };
		/* one priority level slower than ksdioirqd (which is at 1) */
	DEFINE_WAIT(wait);
	struct hif_request *req;

	sched_setscheduler(current, SCHED_FIFO, &param);

	while (1) {
		while (1) {
			/*
			 * Since we never use signals here, one might think
			 * that this ought to be TASK_UNINTERRUPTIBLE. However,
			 * such a task would increase the load average and,
			 * worse, it would trigger the softlockup check.
			 */
			prepare_to_wait(&hif->wait, &wait, TASK_INTERRUPTIBLE);
			if (kthread_should_stop()) {
				finish_wait(&hif->wait, &wait);
				return 0;
			}
			req = dequeue_request(hif);
			if (req)
				break;
			schedule();
		}
		finish_wait(&hif->wait, &wait);

		(void) process_request(req);
	}
	return 0;
}


A_STATUS HIFReadWrite(HIF_DEVICE *hif, A_UINT32 address, A_UCHAR *buffer,
    A_UINT32 length, A_UINT32 request, void *context)
{
	struct device *dev = HIFGetOSDevice(hif);
	struct hif_request *req;

	dev_dbg(dev, "HIFReadWrite(device %p, address 0x%x, buffer %p, "
	    "length %d, request 0x%x, context %p)\n",
	    hif, address, buffer, length, request, context);

	BUG_ON(!(request & (HIF_SYNCHRONOUS | HIF_ASYNCHRONOUS)));
	BUG_ON(!(request & (HIF_BYTE_BASIS | HIF_BLOCK_BASIS)));
	BUG_ON(!(request & (HIF_READ | HIF_WRITE)));
	BUG_ON(!(request & HIF_EXTENDED_IO));

	if (address >= HIF_MBOX_START_ADDR(0) &&
	    address < HIF_MBOX_START_ADDR(MBOXES+1)) {
		BUG_ON(length > HIF_MBOX_WIDTH);
		/* Adjust the address so that the last byte falls on the EOM
		   address. */
		address += HIF_MBOX_WIDTH-length;
	}

	req = kzalloc(sizeof(*req), GFP_ATOMIC);
	if (!req) {
		if (request & HIF_ASYNCHRONOUS)
			htcCallbacks.rwCompletionHandler(context, A_ERROR);
		return A_ERROR;
	}

	req->func = hif->func;
	req->addr = address;
	req->buf = buffer;
	req->len = length;

	if (request & HIF_READ) {
		if (request & HIF_FIXED_ADDRESS)
			req->read = sdio_readsb;
		else
			req->read = sdio_memcpy_fromio;
	} else {
		if (request & HIF_FIXED_ADDRESS)
			req->write = sdio_writesb;
		else
			req->write = sdio_memcpy_toio;
	}

	if (!(request & HIF_ASYNCHRONOUS))
		return process_request(req);

	req->completion = htcCallbacks.rwCompletionHandler;
	req->context = context;
	enqueue_request(hif, req);

	return A_OK;
}


/* ----- Interrupt handling ------------------------------------------------ */

/*
 * Volatile ought to be good enough to make gcc do the right thing on S3C24xx.
 * No need to use atomic or put barriers, keeping the code more readable.
 *
 * Warning: this story changes if going SMP/SMT.
 */

static volatile int masked = 1;
static volatile int pending;
static volatile int in_interrupt;


static void ar6000_do_irq(struct sdio_func *func)
{
	HIF_DEVICE *hif = sdio_get_drvdata(func);
	struct device *dev = HIFGetOSDevice(hif);
	A_STATUS status;

	dev_dbg(dev, "ar6000_do_irq -> %p\n", htcCallbacks.dsrHandler);

	status = htcCallbacks.dsrHandler(hif->htc_handle);
	BUG_ON(status != A_OK);
}


static void sdio_ar6000_irq(struct sdio_func *func)
{
	HIF_DEVICE *hif = sdio_get_drvdata(func);
	struct device *dev = HIFGetOSDevice(hif);

	dev_dbg(dev, "sdio_ar6000_irq\n");

	in_interrupt = 1;
	if (masked) {
		in_interrupt = 0;
		pending++;
		return;
	}
	/*
	 * @@@ This is ugly. If we don't drop the lock, we'll deadlock when
	 * the handler tries to do SDIO. So there are four choices:
	 *
	 * 1) Break the call chain by calling the callback from a workqueue.
	 *    Ugh.
	 * 2) Make process_request aware that we already have the lock.
	 * 3) Drop the lock. Which is ugly but should be safe as long as we're
	 *    making sure the device doesn't go away.
	 * 4) Change the AR6k driver such that it only issues asynchronous
	 *    quests when called from an interrupt.
	 *
	 * Solution 2) is probably the best for now. Will try it later.
	 */
	sdio_release_host(func);
	ar6000_do_irq(func);
	sdio_claim_host(func);
	in_interrupt = 0;
}


void HIFAckInterrupt(HIF_DEVICE *hif)
{
	struct device *dev = HIFGetOSDevice(hif);

	dev_dbg(dev, "HIFAckInterrupt\n");
	/* do nothing */
}


void HIFUnMaskInterrupt(HIF_DEVICE *hif)
{
	struct device *dev = HIFGetOSDevice(hif);

	dev_dbg(dev, "HIFUnMaskInterrupt\n");
	do {
		masked = 1;
		if (pending) {
			pending = 0;
			ar6000_do_irq(hif->func);
			/* We may take an interrupt before unmasking and thus
			   get it pending. In this case, we just loop back. */
		}
		masked = 0;
	}
	while (pending);
}


void HIFMaskInterrupt(HIF_DEVICE *hif)
{
	struct device *dev = HIFGetOSDevice(hif);

	dev_dbg(dev, "HIFMaskInterrupt\n");
	/*
	 * Since sdio_ar6000_irq can also be called from a process context, we
	 * may conceivably end up racing with it. Thus, we need to wait until
	 * we can be sure that no concurrent interrupt processing is going on
	 * before we return.
	 *
	 * Note: this may be a bit on the paranoid side - the callers may
	 * actually be nice enough to disable scheduling. Check later.
	 */
	masked = 1;
	while (in_interrupt)
		yield();
}


/* ----- HIF API glue functions -------------------------------------------- */


struct device *HIFGetOSDevice(HIF_DEVICE *hif)
{
	return &hif->func->dev;
}


void HIFSetHandle(void *hif_handle, void *handle)
{
	HIF_DEVICE *hif = (HIF_DEVICE *) hif_handle;

	hif->htc_handle = handle;
}


/* ----- Device configuration (HIF side) ----------------------------------- */


A_STATUS HIFConfigureDevice(HIF_DEVICE *hif,
    HIF_DEVICE_CONFIG_OPCODE opcode, void *config, A_UINT32 configLen)
{
	struct device *dev = HIFGetOSDevice(hif);
	HIF_DEVICE_IRQ_PROCESSING_MODE *ipm_cfg = config;
	A_UINT32 *mbs_cfg = config;
	int i;

	dev_dbg(dev, "HIFConfigureDevice\n");

	switch (opcode) {
	case HIF_DEVICE_GET_MBOX_BLOCK_SIZE:
		for (i = 0; i != MBOXES; i++)
			mbs_cfg[i] = HIF_MBOX_BLOCK_SIZE;
		break;
	case HIF_DEVICE_GET_MBOX_ADDR:
		for (i = 0; i != MBOXES; i++)
			mbs_cfg[i] = HIF_MBOX_START_ADDR(i);
		break;
	case HIF_DEVICE_GET_IRQ_PROC_MODE:
		*ipm_cfg = HIF_DEVICE_IRQ_SYNC_ONLY;
//		*ipm_cfg = HIF_DEVICE_IRQ_ASYNC_SYNC;
		break;
	default:
		return A_ERROR;
	}
	return A_OK;
}


/* ----- Device probe and removal (Linux side) ----------------------------- */


static int ar6000_do_activate(struct hif_device *hif)
{
	struct sdio_func *func = hif->func;
	struct device *dev = &func->dev;
	int ret;

	dev_dbg(dev, "ar6000_do_activate\n");

	sdio_claim_host(func);
	sdio_enable_func(func);

	INIT_LIST_HEAD(&hif->queue);
	init_waitqueue_head(&hif->wait);
	spin_lock_init(&hif->queue_lock);

	ret = sdio_set_block_size(func, HIF_MBOX_BLOCK_SIZE);
	if (ret < 0) {
		dev_err(dev, "sdio_set_block_size returns %d\n", ret);
		goto out_enabled;
	}
	ret = sdio_claim_irq(func, sdio_ar6000_irq);
	if (ret) {
		dev_err(dev, "sdio_claim_irq returns %d\n", ret);
		goto out_enabled;
	}
	/* Set SDIO_BUS_CD_DISABLE in SDIO_CCCR_IF ? */
#if 0
	sdio_f0_writeb(func, SDIO_CCCR_CAP_E4MI, SDIO_CCCR_CAPS, &ret);
	if (ret) {
		dev_err(dev, "sdio_f0_writeb(SDIO_CCCR_CAPS) returns %d\n",
		    ret);
		goto out_got_irq;
	}
#else
	if (0) /* avoid warning */
		goto out_got_irq;
#endif

	sdio_release_host(func);

	hif->io_task = kthread_run(io, hif, "ar6000_io");
	ret = IS_ERR(hif->io_task);
	if (ret) {
		dev_err(dev, "kthread_run(ar6000_io): %d\n", ret);
		goto out_func_ready;
	}

	ret = htcCallbacks.deviceInsertedHandler(hif);
	if (ret == A_OK)
		return 0;

	dev_err(dev, "deviceInsertedHandler: %d\n", ret);

	ret = kthread_stop(hif->io_task);
	if (ret)
		dev_err(dev, "kthread_stop (ar6000_io): %d\n", ret);

out_func_ready:
	sdio_claim_host(func);

out_got_irq:
	sdio_release_irq(func);

out_enabled:
	sdio_disable_func(func);
	sdio_release_host(func);

	return ret;
}


static void ar6000_do_deactivate(struct hif_device *hif)
{
	struct sdio_func *func = hif->func;
	struct device *dev = &func->dev;
	int ret;

	dev_dbg(dev, "ar6000_do_deactivate\n");
	if (!hif->active)
		return;

	if (mutex_trylock(&shutdown_lock)) {
		/*
		 * Funny, Atheros' HIF does this call, but this just puts us in
		 * a recursion through HTCShutDown/HIFShutDown if unloading the
		 * module.
		 *
		 * However, we need it for suspend/resume. See the comment at
		 * HIFShutDown, below.
		 */
		ret = htcCallbacks.deviceRemovedHandler(hif->htc_handle, A_OK);
		if (ret != A_OK)
			dev_err(dev, "deviceRemovedHandler: %d\n", ret);
		mutex_unlock(&shutdown_lock);
	}
	wait_queue_empty(hif);
	ret = kthread_stop(hif->io_task);
	if (ret)
		dev_err(dev, "kthread_stop (ar6000_io): %d\n", ret);
	sdio_claim_host(func);
	sdio_release_irq(func);
	sdio_disable_func(func);
	sdio_release_host(func);
}


static int ar6000_activate(struct hif_device *hif)
{
	int ret = 0;

	dev_dbg(&hif->func->dev, "ar6000_activate\n");
	mutex_lock(&hif->activate_lock);
	if (!hif->active) {
		ret = ar6000_do_activate(hif);
		if (ret) {
			printk(KERN_ERR "%s: Failed to activate %d\n",
				__func__, ret);
			goto out;
		}
		hif->active = 1;
	}
out:
	mutex_unlock(&hif->activate_lock);
	return ret;
}


static void ar6000_deactivate(struct hif_device *hif)
{
	dev_dbg(&hif->func->dev, "ar6000_deactivate\n");
	mutex_lock(&hif->activate_lock);
	if (hif->active) {
		ar6000_do_deactivate(hif);
		hif->active = 0;
	}
	mutex_unlock(&hif->activate_lock);
}


static int ar6000_rfkill_cb(void *data, int on)
{
	struct hif_device *hif = data;
	struct sdio_func *func = hif->func;
	struct device *dev = &func->dev;

	dev_dbg(dev, "ar6000_rfkill_cb: on %d\n", on);
	if (on)
		return ar6000_activate(hif);
	ar6000_deactivate(hif);
	return 0;
}


static int sdio_ar6000_probe(struct sdio_func *func,
    const struct sdio_device_id *id)
{
	struct device *dev = &func->dev;
	struct hif_device *hif;
	int ret = 0;

	dev_dbg(dev, "sdio_ar6000_probe\n");
	BUG_ON(!htcCallbacks.deviceInsertedHandler);

	hif = kzalloc(sizeof(*hif), GFP_KERNEL);
	if (!hif)
		return -ENOMEM;

	sdio_set_drvdata(func, hif);
	hif->func = func;
	mutex_init(&hif->activate_lock);
	hif->active = 0;

	if (gta02_wlan_query_rfkill_lock())
		ret = ar6000_activate(hif);
	if (!ret) {
		gta02_wlan_set_rfkill_cb(ar6000_rfkill_cb, hif);
		return 0;
	}
	gta02_wlan_query_rfkill_unlock();
	sdio_set_drvdata(func, NULL);
	kfree(hif);
	return ret;
}


static void sdio_ar6000_remove(struct sdio_func *func)
{
	struct device *dev = &func->dev;
	HIF_DEVICE *hif = sdio_get_drvdata(func);

	dev_dbg(dev, "sdio_ar6000_remove\n");
	gta02_wlan_clear_rfkill_cb();
	ar6000_deactivate(hif);
	sdio_set_drvdata(func, NULL);
	kfree(hif);
}


/* ----- Device registration/unregistration (called by HIF) ---------------- */


#define ATHEROS_SDIO_DEVICE(id, offset) \
    SDIO_DEVICE(SDIO_VENDOR_ID_ATHEROS, SDIO_DEVICE_ID_ATHEROS_##id | (offset))

static const struct sdio_device_id sdio_ar6000_ids[] = {
	{ ATHEROS_SDIO_DEVICE(AR6002, 0)	},
	{ ATHEROS_SDIO_DEVICE(AR6002, 0x1)	},
	{ ATHEROS_SDIO_DEVICE(AR6001, 0x8)	},
	{ ATHEROS_SDIO_DEVICE(AR6001, 0x9)	},
	{ ATHEROS_SDIO_DEVICE(AR6001, 0xa)	},
	{ ATHEROS_SDIO_DEVICE(AR6001, 0xb)	},
	{ /* end: all zeroes */			},
};

MODULE_DEVICE_TABLE(sdio, sdio_ar6000_ids);


static struct sdio_driver sdio_ar6000_driver = {
	.probe		= sdio_ar6000_probe,
	.remove		= sdio_ar6000_remove,
	.name		= "sdio_ar6000",
	.id_table	= sdio_ar6000_ids,
};


int HIFInit(HTC_CALLBACKS *callbacks)
{
	int ret;

	BUG_ON(!callbacks);

	printk(KERN_DEBUG "HIFInit\n");
	htcCallbacks = *callbacks;

	ret = sdio_register_driver(&sdio_ar6000_driver);
	if (ret) {
		printk(KERN_ERR
		    "sdio_register_driver(sdio_ar6000_driver): %d\n", ret);
		return A_ERROR;
	}

	return 0;
}


/*
 * We have four possible call chains here:
 *
 * System shutdown/reboot:
 *
 *   kernel_restart_prepare ...> device_shutdown ... > s3cmci_shutdown ->
 *     mmc_remove_host ..> sdio_bus_remove -> sdio_ar6000_remove ->
 *     ar6000_deactivate -> ar6000_do_deactivate ->
 *     deviceRemovedHandler (HTCTargetRemovedHandler) -> HIFShutDownDevice
 *
 *   This is roughly the same sequence as suspend, described below.
 *
 * Module removal:
 *
 *   sys_delete_module -> ar6000_cleanup_module -> HTCShutDown ->
 *     HIFShutDownDevice -> sdio_unregister_driver ...> sdio_bus_remove ->
 *     sdio_ar6000_remove -> ar6000_deactivate -> ar6000_do_deactivate
 *
 *   In this case, HIFShutDownDevice must call sdio_unregister_driver to
 *   notify the driver about its removal. ar6000_do_deactivate must not call
 *   deviceRemovedHandler, because that would loop back into HIFShutDownDevice.
 *
 * Suspend:
 *
 *   device_suspend ...> s3cmci_suspend ...> sdio_bus_remove ->
 *     sdio_ar6000_remove -> ar6000_deactivate -> ar6000_do_deactivate ->
 *     deviceRemovedHandler (HTCTargetRemovedHandler) -> HIFShutDownDevice
 *
 *   We must call deviceRemovedHandler to inform the ar6k stack that the device
 *   has been removed. Since HTCTargetRemovedHandler calls back into
 *   HIFShutDownDevice, we must also prevent the call to
 *   sdio_unregister_driver, or we'd end up recursing into the SDIO stack,
 *   eventually deadlocking somewhere.
 *
 * rfkill:
 *
 *   rfkill_state_store -> rfkill_toggle_radio -> gta02_wlan_toggle_radio ->
 *   ar6000_rfkill_cb -> ar6000_deactivate -> ar6000_do_deactivate ->
 *     deviceRemovedHandler (HTCTargetRemovedHandler) -> HIFShutDownDevice
 *
 *   This is similar to suspend - only the entry point changes.
 */

void HIFShutDownDevice(HIF_DEVICE *hif)
{
	/* Beware, HTCShutDown calls us with hif == NULL ! */
	if (mutex_trylock(&shutdown_lock)) {
		sdio_unregister_driver(&sdio_ar6000_driver);
		mutex_unlock(&shutdown_lock);
	}
}

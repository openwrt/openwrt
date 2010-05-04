#ifndef _BSD_COMPAT_H_
#define _BSD_COMPAT_H_ 1
/****************************************************************************/
/*
 * Provide compat routines for older linux kernels and BSD kernels
 *
 * Written by David McCullough <david_mccullough@mcafee.com>
 * Copyright (C) 2010 David McCullough <david_mccullough@mcafee.com>
 *
 * LICENSE TERMS
 *
 * The free distribution and use of this software in both source and binary
 * form is allowed (with or without changes) provided that:
 *
 *   1. distributions of this source code include the above copyright
 *      notice, this list of conditions and the following disclaimer;
 *
 *   2. distributions in binary form include the above copyright
 *      notice, this list of conditions and the following disclaimer
 *      in the documentation and/or other associated materials;
 *
 *   3. the copyright holder's name is not used to endorse products
 *      built using this software without specific written permission.
 *
 * ALTERNATIVELY, provided that this notice is retained in full, this file
 * may be distributed under the terms of the GNU General Public License (GPL),
 * in which case the provisions of the GPL apply INSTEAD OF those given above.
 *
 * DISCLAIMER
 *
 * This software is provided 'as is' with no explicit or implied warranties
 * in respect of its properties, including, but not limited to, correctness
 * and/or fitness for purpose.
 */
/****************************************************************************/
#ifdef __KERNEL__
/*
 * fake some BSD driver interface stuff specifically for OCF use
 */

typedef struct ocf_device *device_t;

typedef struct {
	int (*cryptodev_newsession)(device_t dev, u_int32_t *sidp, struct cryptoini *cri);
	int (*cryptodev_freesession)(device_t dev, u_int64_t tid);
	int (*cryptodev_process)(device_t dev, struct cryptop *crp, int hint);
	int (*cryptodev_kprocess)(device_t dev, struct cryptkop *krp, int hint);
} device_method_t;
#define DEVMETHOD(id, func)	id: func

struct ocf_device {
	char name[32];		/* the driver name */
	char nameunit[32];	/* the driver name + HW instance */
	int  unit;
	device_method_t	methods;
	void *softc;
};

#define CRYPTODEV_NEWSESSION(dev, sid, cri) \
	((*(dev)->methods.cryptodev_newsession)(dev,sid,cri))
#define CRYPTODEV_FREESESSION(dev, sid) \
	((*(dev)->methods.cryptodev_freesession)(dev, sid))
#define CRYPTODEV_PROCESS(dev, crp, hint) \
	((*(dev)->methods.cryptodev_process)(dev, crp, hint))
#define CRYPTODEV_KPROCESS(dev, krp, hint) \
	((*(dev)->methods.cryptodev_kprocess)(dev, krp, hint))

#define device_get_name(dev)	((dev)->name)
#define device_get_nameunit(dev)	((dev)->nameunit)
#define device_get_unit(dev)	((dev)->unit)
#define device_get_softc(dev)	((dev)->softc)

#define	softc_device_decl \
		struct ocf_device _device; \
		device_t

#define	softc_device_init(_sc, _name, _unit, _methods) \
	if (1) {\
	strncpy((_sc)->_device.name, _name, sizeof((_sc)->_device.name) - 1); \
	snprintf((_sc)->_device.nameunit, sizeof((_sc)->_device.name), "%s%d", _name, _unit); \
	(_sc)->_device.unit = _unit; \
	(_sc)->_device.methods = _methods; \
	(_sc)->_device.softc = (void *) _sc; \
	*(device_t *)((softc_get_device(_sc))+1) = &(_sc)->_device; \
	} else

#define	softc_get_device(_sc)	(&(_sc)->_device)

/*
 * iomem support for 2.4 and 2.6 kernels
 */
#include <linux/version.h>
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,0)
#define ocf_iomem_t	unsigned long

/*
 * implement simple workqueue like support for older kernels
 */

#include <linux/tqueue.h>

#define work_struct tq_struct

#define INIT_WORK(wp, fp, ap) \
	do { \
		(wp)->sync = 0; \
		(wp)->routine = (fp); \
		(wp)->data = (ap); \
	} while (0)

#define schedule_work(wp) \
	do { \
		queue_task((wp), &tq_immediate); \
		mark_bh(IMMEDIATE_BH); \
	} while (0)

#define flush_scheduled_work()	run_task_queue(&tq_immediate)

#else
#define ocf_iomem_t	void __iomem *

#include <linux/workqueue.h>

#endif

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,26)
#include <linux/fdtable.h>
#elif LINUX_VERSION_CODE < KERNEL_VERSION(2,6,11)
#define files_fdtable(files)	(files)
#endif

#ifdef MODULE_PARM
#undef module_param	/* just in case */
#define	module_param(a,b,c)		MODULE_PARM(a,"i")
#endif

#define bzero(s,l)		memset(s,0,l)
#define bcopy(s,d,l)	memcpy(d,s,l)
#define bcmp(x, y, l)	memcmp(x,y,l)

#define MIN(x,y)	((x) < (y) ? (x) : (y))

#define device_printf(dev, a...) ({ \
				printk("%s: ", device_get_nameunit(dev)); printk(a); \
			})

#undef printf
#define printf(fmt...)	printk(fmt)

#define KASSERT(c,p)	if (!(c)) { printk p ; } else

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,0)
#define ocf_daemonize(str) \
	daemonize(); \
	spin_lock_irq(&current->sigmask_lock); \
	sigemptyset(&current->blocked); \
	recalc_sigpending(current); \
	spin_unlock_irq(&current->sigmask_lock); \
	sprintf(current->comm, str);
#else
#define ocf_daemonize(str) daemonize(str);
#endif

#define	TAILQ_INSERT_TAIL(q,d,m) list_add_tail(&(d)->m, (q))
#define	TAILQ_EMPTY(q)	list_empty(q)
#define	TAILQ_FOREACH(v, q, m) list_for_each_entry(v, q, m)

#define read_random(p,l) get_random_bytes(p,l)

#define DELAY(x)	((x) > 2000 ? mdelay((x)/1000) : udelay(x))
#define strtoul simple_strtoul

#define pci_get_vendor(dev)	((dev)->vendor)
#define pci_get_device(dev)	((dev)->device)

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,0)
#define pci_set_consistent_dma_mask(dev, mask) (0)
#endif
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,10)
#define pci_dma_sync_single_for_cpu pci_dma_sync_single
#endif

#ifndef DMA_32BIT_MASK
#define DMA_32BIT_MASK  0x00000000ffffffffULL
#endif

#ifndef htole32
#define htole32(x)	cpu_to_le32(x)
#endif
#ifndef htobe32
#define htobe32(x)	cpu_to_be32(x)
#endif
#ifndef htole16
#define htole16(x)	cpu_to_le16(x)
#endif
#ifndef htobe16
#define htobe16(x)	cpu_to_be16(x)
#endif

/* older kernels don't have these */

#include <asm/irq.h>
#if !defined(IRQ_NONE) && !defined(IRQ_RETVAL)
#define IRQ_NONE
#define IRQ_HANDLED
#define IRQ_WAKE_THREAD
#define IRQ_RETVAL
#define irqreturn_t void
typedef irqreturn_t (*irq_handler_t)(int irq, void *arg, struct pt_regs *regs);
#endif
#ifndef IRQF_SHARED
#define IRQF_SHARED	SA_SHIRQ
#endif

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,5,0)
# define strlcpy(dest,src,len) \
		({strncpy(dest,src,(len)-1); ((char *)dest)[(len)-1] = '\0'; })
#endif

#ifndef MAX_ERRNO
#define MAX_ERRNO	4095
#endif
#ifndef IS_ERR_VALUE
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,5,5)
#include <linux/err.h>
#endif
#ifndef IS_ERR_VALUE
#define IS_ERR_VALUE(x) ((unsigned long)(x) >= (unsigned long)-MAX_ERRNO)
#endif
#endif

/*
 * common debug for all
 */
#if 1
#define dprintk(a...)	do { if (debug) printk(a); } while(0)
#else
#define dprintk(a...)
#endif

#ifndef SLAB_ATOMIC
/* Changed in 2.6.20, must use GFP_ATOMIC now */
#define	SLAB_ATOMIC	GFP_ATOMIC
#endif

/*
 * need some additional support for older kernels */
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,2)
#define pci_register_driver_compat(driver, rc) \
	do { \
		if ((rc) > 0) { \
			(rc) = 0; \
		} else if (rc == 0) { \
			(rc) = -ENODEV; \
		} else { \
			pci_unregister_driver(driver); \
		} \
	} while (0)
#elif LINUX_VERSION_CODE < KERNEL_VERSION(2,6,10)
#define pci_register_driver_compat(driver,rc) ((rc) = (rc) < 0 ? (rc) : 0)
#else
#define pci_register_driver_compat(driver,rc)
#endif

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,24)

#include <linux/mm.h>
#include <asm/scatterlist.h>

static inline void sg_set_page(struct scatterlist *sg,  struct page *page,
			       unsigned int len, unsigned int offset)
{
	sg->page = page;
	sg->offset = offset;
	sg->length = len;
}

static inline void *sg_virt(struct scatterlist *sg)
{
	return page_address(sg->page) + sg->offset;
}

#define sg_init_table(sg, n)

#endif

#ifndef late_initcall
#define late_initcall(init) module_init(init)
#endif

#endif /* __KERNEL__ */

/****************************************************************************/
#endif /* _BSD_COMPAT_H_ */

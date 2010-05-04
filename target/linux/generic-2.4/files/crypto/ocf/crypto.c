/*-
 * Linux port done by David McCullough <david_mccullough@mcafee.com>
 * Copyright (C) 2006-2010 David McCullough
 * Copyright (C) 2004-2005 Intel Corporation.
 * The license and original author are listed below.
 *
 * Redistribution and use in source and binary forms, with or without
 * Copyright (c) 2002-2006 Sam Leffler.  All rights reserved.
 *
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#if 0
#include <sys/cdefs.h>
__FBSDID("$FreeBSD: src/sys/opencrypto/crypto.c,v 1.27 2007/03/21 03:42:51 sam Exp $");
#endif

/*
 * Cryptographic Subsystem.
 *
 * This code is derived from the Openbsd Cryptographic Framework (OCF)
 * that has the copyright shown below.  Very little of the original
 * code remains.
 */
/*-
 * The author of this code is Angelos D. Keromytis (angelos@cis.upenn.edu)
 *
 * This code was written by Angelos D. Keromytis in Athens, Greece, in
 * February 2000. Network Security Technologies Inc. (NSTI) kindly
 * supported the development of this code.
 *
 * Copyright (c) 2000, 2001 Angelos D. Keromytis
 *
 * Permission to use, copy, and modify this software with or without fee
 * is hereby granted, provided that this entire notice is included in
 * all source code copies of any software which is or includes a copy or
 * modification of this software.
 *
 * THIS SOFTWARE IS BEING PROVIDED "AS IS", WITHOUT ANY EXPRESS OR
 * IMPLIED WARRANTY. IN PARTICULAR, NONE OF THE AUTHORS MAKES ANY
 * REPRESENTATION OR WARRANTY OF ANY KIND CONCERNING THE
 * MERCHANTABILITY OF THIS SOFTWARE OR ITS FITNESS FOR ANY PARTICULAR
 * PURPOSE.
 *
__FBSDID("$FreeBSD: src/sys/opencrypto/crypto.c,v 1.16 2005/01/07 02:29:16 imp Exp $");
 */


#ifndef AUTOCONF_INCLUDED
#include <linux/config.h>
#endif
#include <linux/module.h>
#include <linux/init.h>
#include <linux/list.h>
#include <linux/slab.h>
#include <linux/wait.h>
#include <linux/sched.h>
#include <linux/spinlock.h>
#include <linux/version.h>
#include <cryptodev.h>

/*
 * keep track of whether or not we have been initialised, a big
 * issue if we are linked into the kernel and a driver gets started before
 * us
 */
static int crypto_initted = 0;

/*
 * Crypto drivers register themselves by allocating a slot in the
 * crypto_drivers table with crypto_get_driverid() and then registering
 * each algorithm they support with crypto_register() and crypto_kregister().
 */

/*
 * lock on driver table
 * we track its state as spin_is_locked does not do anything on non-SMP boxes
 */
static spinlock_t	crypto_drivers_lock;
static int			crypto_drivers_locked;		/* for non-SMP boxes */

#define	CRYPTO_DRIVER_LOCK() \
			({ \
				spin_lock_irqsave(&crypto_drivers_lock, d_flags); \
			 	crypto_drivers_locked = 1; \
				dprintk("%s,%d: DRIVER_LOCK()\n", __FILE__, __LINE__); \
			 })
#define	CRYPTO_DRIVER_UNLOCK() \
			({ \
			 	dprintk("%s,%d: DRIVER_UNLOCK()\n", __FILE__, __LINE__); \
			 	crypto_drivers_locked = 0; \
				spin_unlock_irqrestore(&crypto_drivers_lock, d_flags); \
			 })
#define	CRYPTO_DRIVER_ASSERT() \
			({ \
			 	if (!crypto_drivers_locked) { \
					dprintk("%s,%d: DRIVER_ASSERT!\n", __FILE__, __LINE__); \
			 	} \
			 })

/*
 * Crypto device/driver capabilities structure.
 *
 * Synchronization:
 * (d) - protected by CRYPTO_DRIVER_LOCK()
 * (q) - protected by CRYPTO_Q_LOCK()
 * Not tagged fields are read-only.
 */
struct cryptocap {
	device_t	cc_dev;			/* (d) device/driver */
	u_int32_t	cc_sessions;		/* (d) # of sessions */
	u_int32_t	cc_koperations;		/* (d) # os asym operations */
	/*
	 * Largest possible operator length (in bits) for each type of
	 * encryption algorithm. XXX not used
	 */
	u_int16_t	cc_max_op_len[CRYPTO_ALGORITHM_MAX + 1];
	u_int8_t	cc_alg[CRYPTO_ALGORITHM_MAX + 1];
	u_int8_t	cc_kalg[CRK_ALGORITHM_MAX + 1];

	int		cc_flags;		/* (d) flags */
#define CRYPTOCAP_F_CLEANUP	0x80000000	/* needs resource cleanup */
	int		cc_qblocked;		/* (q) symmetric q blocked */
	int		cc_kqblocked;		/* (q) asymmetric q blocked */

	int		cc_unqblocked;		/* (q) symmetric q blocked */
	int		cc_unkqblocked;		/* (q) asymmetric q blocked */
};
static struct cryptocap *crypto_drivers = NULL;
static int crypto_drivers_num = 0;

/*
 * There are two queues for crypto requests; one for symmetric (e.g.
 * cipher) operations and one for asymmetric (e.g. MOD)operations.
 * A single mutex is used to lock access to both queues.  We could
 * have one per-queue but having one simplifies handling of block/unblock
 * operations.
 */
static	int crp_sleep = 0;
static LIST_HEAD(crp_q);		/* request queues */
static LIST_HEAD(crp_kq);

static spinlock_t crypto_q_lock;

int crypto_all_qblocked = 0;  /* protect with Q_LOCK */
module_param(crypto_all_qblocked, int, 0444);
MODULE_PARM_DESC(crypto_all_qblocked, "Are all crypto queues blocked");

int crypto_all_kqblocked = 0; /* protect with Q_LOCK */
module_param(crypto_all_kqblocked, int, 0444);
MODULE_PARM_DESC(crypto_all_kqblocked, "Are all asym crypto queues blocked");

#define	CRYPTO_Q_LOCK() \
			({ \
				spin_lock_irqsave(&crypto_q_lock, q_flags); \
			 	dprintk("%s,%d: Q_LOCK()\n", __FILE__, __LINE__); \
			 })
#define	CRYPTO_Q_UNLOCK() \
			({ \
			 	dprintk("%s,%d: Q_UNLOCK()\n", __FILE__, __LINE__); \
				spin_unlock_irqrestore(&crypto_q_lock, q_flags); \
			 })

/*
 * There are two queues for processing completed crypto requests; one
 * for the symmetric and one for the asymmetric ops.  We only need one
 * but have two to avoid type futzing (cryptop vs. cryptkop).  A single
 * mutex is used to lock access to both queues.  Note that this lock
 * must be separate from the lock on request queues to insure driver
 * callbacks don't generate lock order reversals.
 */
static LIST_HEAD(crp_ret_q);		/* callback queues */
static LIST_HEAD(crp_ret_kq);

static spinlock_t crypto_ret_q_lock;
#define	CRYPTO_RETQ_LOCK() \
			({ \
				spin_lock_irqsave(&crypto_ret_q_lock, r_flags); \
				dprintk("%s,%d: RETQ_LOCK\n", __FILE__, __LINE__); \
			 })
#define	CRYPTO_RETQ_UNLOCK() \
			({ \
			 	dprintk("%s,%d: RETQ_UNLOCK\n", __FILE__, __LINE__); \
				spin_unlock_irqrestore(&crypto_ret_q_lock, r_flags); \
			 })
#define	CRYPTO_RETQ_EMPTY()	(list_empty(&crp_ret_q) && list_empty(&crp_ret_kq))

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,20)
static kmem_cache_t *cryptop_zone;
static kmem_cache_t *cryptodesc_zone;
#else
static struct kmem_cache *cryptop_zone;
static struct kmem_cache *cryptodesc_zone;
#endif

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,27)
#include <linux/sched.h>
#define	kill_proc(p,s,v)	send_sig(s,find_task_by_vpid(p),0)
#endif

#define debug crypto_debug
int crypto_debug = 0;
module_param(crypto_debug, int, 0644);
MODULE_PARM_DESC(crypto_debug, "Enable debug");
EXPORT_SYMBOL(crypto_debug);

/*
 * Maximum number of outstanding crypto requests before we start
 * failing requests.  We need this to prevent DOS when too many
 * requests are arriving for us to keep up.  Otherwise we will
 * run the system out of memory.  Since crypto is slow,  we are
 * usually the bottleneck that needs to say, enough is enough.
 *
 * We cannot print errors when this condition occurs,  we are already too
 * slow,  printing anything will just kill us
 */

static int crypto_q_cnt = 0;
module_param(crypto_q_cnt, int, 0444);
MODULE_PARM_DESC(crypto_q_cnt,
		"Current number of outstanding crypto requests");

static int crypto_q_max = 1000;
module_param(crypto_q_max, int, 0644);
MODULE_PARM_DESC(crypto_q_max,
		"Maximum number of outstanding crypto requests");

#define bootverbose crypto_verbose
static int crypto_verbose = 0;
module_param(crypto_verbose, int, 0644);
MODULE_PARM_DESC(crypto_verbose,
		"Enable verbose crypto startup");

int	crypto_usercrypto = 1;	/* userland may do crypto reqs */
module_param(crypto_usercrypto, int, 0644);
MODULE_PARM_DESC(crypto_usercrypto,
	   "Enable/disable user-mode access to crypto support");

int	crypto_userasymcrypto = 1;	/* userland may do asym crypto reqs */
module_param(crypto_userasymcrypto, int, 0644);
MODULE_PARM_DESC(crypto_userasymcrypto,
	   "Enable/disable user-mode access to asymmetric crypto support");

int	crypto_devallowsoft = 0;	/* only use hardware crypto */
module_param(crypto_devallowsoft, int, 0644);
MODULE_PARM_DESC(crypto_devallowsoft,
	   "Enable/disable use of software crypto support");

/*
 * This parameter controls the maximum number of crypto operations to 
 * do consecutively in the crypto kernel thread before scheduling to allow 
 * other processes to run. Without it, it is possible to get into a 
 * situation where the crypto thread never allows any other processes to run.
 * Default to 1000 which should be less than one second.
 */
static int crypto_max_loopcount = 1000;
module_param(crypto_max_loopcount, int, 0644);
MODULE_PARM_DESC(crypto_max_loopcount,
	   "Maximum number of crypto ops to do before yielding to other processes");

static pid_t	cryptoproc = (pid_t) -1;
static struct	completion cryptoproc_exited;
static DECLARE_WAIT_QUEUE_HEAD(cryptoproc_wait);
static pid_t	cryptoretproc = (pid_t) -1;
static struct	completion cryptoretproc_exited;
static DECLARE_WAIT_QUEUE_HEAD(cryptoretproc_wait);

static	int crypto_proc(void *arg);
static	int crypto_ret_proc(void *arg);
static	int crypto_invoke(struct cryptocap *cap, struct cryptop *crp, int hint);
static	int crypto_kinvoke(struct cryptkop *krp, int flags);
static	void crypto_exit(void);
static  int crypto_init(void);

static	struct cryptostats cryptostats;

static struct cryptocap *
crypto_checkdriver(u_int32_t hid)
{
	if (crypto_drivers == NULL)
		return NULL;
	return (hid >= crypto_drivers_num ? NULL : &crypto_drivers[hid]);
}

/*
 * Compare a driver's list of supported algorithms against another
 * list; return non-zero if all algorithms are supported.
 */
static int
driver_suitable(const struct cryptocap *cap, const struct cryptoini *cri)
{
	const struct cryptoini *cr;

	/* See if all the algorithms are supported. */
	for (cr = cri; cr; cr = cr->cri_next)
		if (cap->cc_alg[cr->cri_alg] == 0)
			return 0;
	return 1;
}

/*
 * Select a driver for a new session that supports the specified
 * algorithms and, optionally, is constrained according to the flags.
 * The algorithm we use here is pretty stupid; just use the
 * first driver that supports all the algorithms we need. If there
 * are multiple drivers we choose the driver with the fewest active
 * sessions.  We prefer hardware-backed drivers to software ones.
 *
 * XXX We need more smarts here (in real life too, but that's
 * XXX another story altogether).
 */
static struct cryptocap *
crypto_select_driver(const struct cryptoini *cri, int flags)
{
	struct cryptocap *cap, *best;
	int match, hid;

	CRYPTO_DRIVER_ASSERT();

	/*
	 * Look first for hardware crypto devices if permitted.
	 */
	if (flags & CRYPTOCAP_F_HARDWARE)
		match = CRYPTOCAP_F_HARDWARE;
	else
		match = CRYPTOCAP_F_SOFTWARE;
	best = NULL;
again:
	for (hid = 0; hid < crypto_drivers_num; hid++) {
		cap = &crypto_drivers[hid];
		/*
		 * If it's not initialized, is in the process of
		 * going away, or is not appropriate (hardware
		 * or software based on match), then skip.
		 */
		if (cap->cc_dev == NULL ||
		    (cap->cc_flags & CRYPTOCAP_F_CLEANUP) ||
		    (cap->cc_flags & match) == 0)
			continue;

		/* verify all the algorithms are supported. */
		if (driver_suitable(cap, cri)) {
			if (best == NULL ||
			    cap->cc_sessions < best->cc_sessions)
				best = cap;
		}
	}
	if (best != NULL)
		return best;
	if (match == CRYPTOCAP_F_HARDWARE && (flags & CRYPTOCAP_F_SOFTWARE)) {
		/* sort of an Algol 68-style for loop */
		match = CRYPTOCAP_F_SOFTWARE;
		goto again;
	}
	return best;
}

/*
 * Create a new session.  The crid argument specifies a crypto
 * driver to use or constraints on a driver to select (hardware
 * only, software only, either).  Whatever driver is selected
 * must be capable of the requested crypto algorithms.
 */
int
crypto_newsession(u_int64_t *sid, struct cryptoini *cri, int crid)
{
	struct cryptocap *cap;
	u_int32_t hid, lid;
	int err;
	unsigned long d_flags;

	CRYPTO_DRIVER_LOCK();
	if ((crid & (CRYPTOCAP_F_HARDWARE | CRYPTOCAP_F_SOFTWARE)) == 0) {
		/*
		 * Use specified driver; verify it is capable.
		 */
		cap = crypto_checkdriver(crid);
		if (cap != NULL && !driver_suitable(cap, cri))
			cap = NULL;
	} else {
		/*
		 * No requested driver; select based on crid flags.
		 */
		cap = crypto_select_driver(cri, crid);
		/*
		 * if NULL then can't do everything in one session.
		 * XXX Fix this. We need to inject a "virtual" session
		 * XXX layer right about here.
		 */
	}
	if (cap != NULL) {
		/* Call the driver initialization routine. */
		hid = cap - crypto_drivers;
		lid = hid;		/* Pass the driver ID. */
		cap->cc_sessions++;
		CRYPTO_DRIVER_UNLOCK();
		err = CRYPTODEV_NEWSESSION(cap->cc_dev, &lid, cri);
		CRYPTO_DRIVER_LOCK();
		if (err == 0) {
			(*sid) = (cap->cc_flags & 0xff000000)
			       | (hid & 0x00ffffff);
			(*sid) <<= 32;
			(*sid) |= (lid & 0xffffffff);
		} else
			cap->cc_sessions--;
	} else
		err = EINVAL;
	CRYPTO_DRIVER_UNLOCK();
	return err;
}

static void
crypto_remove(struct cryptocap *cap)
{
	CRYPTO_DRIVER_ASSERT();
	if (cap->cc_sessions == 0 && cap->cc_koperations == 0)
		bzero(cap, sizeof(*cap));
}

/*
 * Delete an existing session (or a reserved session on an unregistered
 * driver).
 */
int
crypto_freesession(u_int64_t sid)
{
	struct cryptocap *cap;
	u_int32_t hid;
	int err = 0;
	unsigned long d_flags;

	dprintk("%s()\n", __FUNCTION__);
	CRYPTO_DRIVER_LOCK();

	if (crypto_drivers == NULL) {
		err = EINVAL;
		goto done;
	}

	/* Determine two IDs. */
	hid = CRYPTO_SESID2HID(sid);

	if (hid >= crypto_drivers_num) {
		dprintk("%s - INVALID DRIVER NUM %d\n", __FUNCTION__, hid);
		err = ENOENT;
		goto done;
	}
	cap = &crypto_drivers[hid];

	if (cap->cc_dev) {
		CRYPTO_DRIVER_UNLOCK();
		/* Call the driver cleanup routine, if available, unlocked. */
		err = CRYPTODEV_FREESESSION(cap->cc_dev, sid);
		CRYPTO_DRIVER_LOCK();
	}

	if (cap->cc_sessions)
		cap->cc_sessions--;

	if (cap->cc_flags & CRYPTOCAP_F_CLEANUP)
		crypto_remove(cap);

done:
	CRYPTO_DRIVER_UNLOCK();
	return err;
}

/*
 * Return an unused driver id.  Used by drivers prior to registering
 * support for the algorithms they handle.
 */
int32_t
crypto_get_driverid(device_t dev, int flags)
{
	struct cryptocap *newdrv;
	int i;
	unsigned long d_flags;

	if ((flags & (CRYPTOCAP_F_HARDWARE | CRYPTOCAP_F_SOFTWARE)) == 0) {
		printf("%s: no flags specified when registering driver\n",
		    device_get_nameunit(dev));
		return -1;
	}

	CRYPTO_DRIVER_LOCK();

	for (i = 0; i < crypto_drivers_num; i++) {
		if (crypto_drivers[i].cc_dev == NULL &&
		    (crypto_drivers[i].cc_flags & CRYPTOCAP_F_CLEANUP) == 0) {
			break;
		}
	}

	/* Out of entries, allocate some more. */
	if (i == crypto_drivers_num) {
		/* Be careful about wrap-around. */
		if (2 * crypto_drivers_num <= crypto_drivers_num) {
			CRYPTO_DRIVER_UNLOCK();
			printk("crypto: driver count wraparound!\n");
			return -1;
		}

		newdrv = kmalloc(2 * crypto_drivers_num * sizeof(struct cryptocap),
				GFP_KERNEL);
		if (newdrv == NULL) {
			CRYPTO_DRIVER_UNLOCK();
			printk("crypto: no space to expand driver table!\n");
			return -1;
		}

		memcpy(newdrv, crypto_drivers,
				crypto_drivers_num * sizeof(struct cryptocap));
		memset(&newdrv[crypto_drivers_num], 0,
				crypto_drivers_num * sizeof(struct cryptocap));

		crypto_drivers_num *= 2;

		kfree(crypto_drivers);
		crypto_drivers = newdrv;
	}

	/* NB: state is zero'd on free */
	crypto_drivers[i].cc_sessions = 1;	/* Mark */
	crypto_drivers[i].cc_dev = dev;
	crypto_drivers[i].cc_flags = flags;
	if (bootverbose)
		printf("crypto: assign %s driver id %u, flags %u\n",
		    device_get_nameunit(dev), i, flags);

	CRYPTO_DRIVER_UNLOCK();

	return i;
}

/*
 * Lookup a driver by name.  We match against the full device
 * name and unit, and against just the name.  The latter gives
 * us a simple widlcarding by device name.  On success return the
 * driver/hardware identifier; otherwise return -1.
 */
int
crypto_find_driver(const char *match)
{
	int i, len = strlen(match);
	unsigned long d_flags;

	CRYPTO_DRIVER_LOCK();
	for (i = 0; i < crypto_drivers_num; i++) {
		device_t dev = crypto_drivers[i].cc_dev;
		if (dev == NULL ||
		    (crypto_drivers[i].cc_flags & CRYPTOCAP_F_CLEANUP))
			continue;
		if (strncmp(match, device_get_nameunit(dev), len) == 0 ||
		    strncmp(match, device_get_name(dev), len) == 0)
			break;
	}
	CRYPTO_DRIVER_UNLOCK();
	return i < crypto_drivers_num ? i : -1;
}

/*
 * Return the device_t for the specified driver or NULL
 * if the driver identifier is invalid.
 */
device_t
crypto_find_device_byhid(int hid)
{
	struct cryptocap *cap = crypto_checkdriver(hid);
	return cap != NULL ? cap->cc_dev : NULL;
}

/*
 * Return the device/driver capabilities.
 */
int
crypto_getcaps(int hid)
{
	struct cryptocap *cap = crypto_checkdriver(hid);
	return cap != NULL ? cap->cc_flags : 0;
}

/*
 * Register support for a key-related algorithm.  This routine
 * is called once for each algorithm supported a driver.
 */
int
crypto_kregister(u_int32_t driverid, int kalg, u_int32_t flags)
{
	struct cryptocap *cap;
	int err;
	unsigned long d_flags;

	dprintk("%s()\n", __FUNCTION__);
	CRYPTO_DRIVER_LOCK();

	cap = crypto_checkdriver(driverid);
	if (cap != NULL &&
	    (CRK_ALGORITM_MIN <= kalg && kalg <= CRK_ALGORITHM_MAX)) {
		/*
		 * XXX Do some performance testing to determine placing.
		 * XXX We probably need an auxiliary data structure that
		 * XXX describes relative performances.
		 */

		cap->cc_kalg[kalg] = flags | CRYPTO_ALG_FLAG_SUPPORTED;
		if (bootverbose)
			printf("crypto: %s registers key alg %u flags %u\n"
				, device_get_nameunit(cap->cc_dev)
				, kalg
				, flags
			);
		err = 0;
	} else
		err = EINVAL;

	CRYPTO_DRIVER_UNLOCK();
	return err;
}

/*
 * Register support for a non-key-related algorithm.  This routine
 * is called once for each such algorithm supported by a driver.
 */
int
crypto_register(u_int32_t driverid, int alg, u_int16_t maxoplen,
    u_int32_t flags)
{
	struct cryptocap *cap;
	int err;
	unsigned long d_flags;

	dprintk("%s(id=0x%x, alg=%d, maxoplen=%d, flags=0x%x)\n", __FUNCTION__,
			driverid, alg, maxoplen, flags);

	CRYPTO_DRIVER_LOCK();

	cap = crypto_checkdriver(driverid);
	/* NB: algorithms are in the range [1..max] */
	if (cap != NULL &&
	    (CRYPTO_ALGORITHM_MIN <= alg && alg <= CRYPTO_ALGORITHM_MAX)) {
		/*
		 * XXX Do some performance testing to determine placing.
		 * XXX We probably need an auxiliary data structure that
		 * XXX describes relative performances.
		 */

		cap->cc_alg[alg] = flags | CRYPTO_ALG_FLAG_SUPPORTED;
		cap->cc_max_op_len[alg] = maxoplen;
		if (bootverbose)
			printf("crypto: %s registers alg %u flags %u maxoplen %u\n"
				, device_get_nameunit(cap->cc_dev)
				, alg
				, flags
				, maxoplen
			);
		cap->cc_sessions = 0;		/* Unmark */
		err = 0;
	} else
		err = EINVAL;

	CRYPTO_DRIVER_UNLOCK();
	return err;
}

static void
driver_finis(struct cryptocap *cap)
{
	u_int32_t ses, kops;

	CRYPTO_DRIVER_ASSERT();

	ses = cap->cc_sessions;
	kops = cap->cc_koperations;
	bzero(cap, sizeof(*cap));
	if (ses != 0 || kops != 0) {
		/*
		 * If there are pending sessions,
		 * just mark as invalid.
		 */
		cap->cc_flags |= CRYPTOCAP_F_CLEANUP;
		cap->cc_sessions = ses;
		cap->cc_koperations = kops;
	}
}

/*
 * Unregister a crypto driver. If there are pending sessions using it,
 * leave enough information around so that subsequent calls using those
 * sessions will correctly detect the driver has been unregistered and
 * reroute requests.
 */
int
crypto_unregister(u_int32_t driverid, int alg)
{
	struct cryptocap *cap;
	int i, err;
	unsigned long d_flags;

	dprintk("%s()\n", __FUNCTION__);
	CRYPTO_DRIVER_LOCK();

	cap = crypto_checkdriver(driverid);
	if (cap != NULL &&
	    (CRYPTO_ALGORITHM_MIN <= alg && alg <= CRYPTO_ALGORITHM_MAX) &&
	    cap->cc_alg[alg] != 0) {
		cap->cc_alg[alg] = 0;
		cap->cc_max_op_len[alg] = 0;

		/* Was this the last algorithm ? */
		for (i = 1; i <= CRYPTO_ALGORITHM_MAX; i++)
			if (cap->cc_alg[i] != 0)
				break;

		if (i == CRYPTO_ALGORITHM_MAX + 1)
			driver_finis(cap);
		err = 0;
	} else
		err = EINVAL;
	CRYPTO_DRIVER_UNLOCK();
	return err;
}

/*
 * Unregister all algorithms associated with a crypto driver.
 * If there are pending sessions using it, leave enough information
 * around so that subsequent calls using those sessions will
 * correctly detect the driver has been unregistered and reroute
 * requests.
 */
int
crypto_unregister_all(u_int32_t driverid)
{
	struct cryptocap *cap;
	int err;
	unsigned long d_flags;

	dprintk("%s()\n", __FUNCTION__);
	CRYPTO_DRIVER_LOCK();
	cap = crypto_checkdriver(driverid);
	if (cap != NULL) {
		driver_finis(cap);
		err = 0;
	} else
		err = EINVAL;
	CRYPTO_DRIVER_UNLOCK();

	return err;
}

/*
 * Clear blockage on a driver.  The what parameter indicates whether
 * the driver is now ready for cryptop's and/or cryptokop's.
 */
int
crypto_unblock(u_int32_t driverid, int what)
{
	struct cryptocap *cap;
	int err;
	unsigned long q_flags;

	CRYPTO_Q_LOCK();
	cap = crypto_checkdriver(driverid);
	if (cap != NULL) {
		if (what & CRYPTO_SYMQ) {
			cap->cc_qblocked = 0;
			cap->cc_unqblocked = 0;
			crypto_all_qblocked = 0;
		}
		if (what & CRYPTO_ASYMQ) {
			cap->cc_kqblocked = 0;
			cap->cc_unkqblocked = 0;
			crypto_all_kqblocked = 0;
		}
		if (crp_sleep)
			wake_up_interruptible(&cryptoproc_wait);
		err = 0;
	} else
		err = EINVAL;
	CRYPTO_Q_UNLOCK(); //DAVIDM should this be a driver lock

	return err;
}

/*
 * Add a crypto request to a queue, to be processed by the kernel thread.
 */
int
crypto_dispatch(struct cryptop *crp)
{
	struct cryptocap *cap;
	int result = -1;
	unsigned long q_flags;

	dprintk("%s()\n", __FUNCTION__);

	cryptostats.cs_ops++;

	CRYPTO_Q_LOCK();
	if (crypto_q_cnt >= crypto_q_max) {
		CRYPTO_Q_UNLOCK();
		cryptostats.cs_drops++;
		return ENOMEM;
	}
	crypto_q_cnt++;

	/* make sure we are starting a fresh run on this crp. */
	crp->crp_flags &= ~CRYPTO_F_DONE;
	crp->crp_etype = 0;

	/*
	 * Caller marked the request to be processed immediately; dispatch
	 * it directly to the driver unless the driver is currently blocked.
	 */
	if ((crp->crp_flags & CRYPTO_F_BATCH) == 0) {
		int hid = CRYPTO_SESID2HID(crp->crp_sid);
		cap = crypto_checkdriver(hid);
		/* Driver cannot disappear when there is an active session. */
		KASSERT(cap != NULL, ("%s: Driver disappeared.", __func__));
		if (!cap->cc_qblocked) {
			crypto_all_qblocked = 0;
			crypto_drivers[hid].cc_unqblocked = 1;
			CRYPTO_Q_UNLOCK();
			result = crypto_invoke(cap, crp, 0);
			CRYPTO_Q_LOCK();
			if (result == ERESTART)
				if (crypto_drivers[hid].cc_unqblocked)
					crypto_drivers[hid].cc_qblocked = 1;
			crypto_drivers[hid].cc_unqblocked = 0;
		}
	}
	if (result == ERESTART) {
		/*
		 * The driver ran out of resources, mark the
		 * driver ``blocked'' for cryptop's and put
		 * the request back in the queue.  It would
		 * best to put the request back where we got
		 * it but that's hard so for now we put it
		 * at the front.  This should be ok; putting
		 * it at the end does not work.
		 */
		list_add(&crp->crp_next, &crp_q);
		cryptostats.cs_blocks++;
		result = 0;
	} else if (result == -1) {
		TAILQ_INSERT_TAIL(&crp_q, crp, crp_next);
		result = 0;
	}
	if (crp_sleep)
		wake_up_interruptible(&cryptoproc_wait);
	CRYPTO_Q_UNLOCK();
	return result;
}

/*
 * Add an asymetric crypto request to a queue,
 * to be processed by the kernel thread.
 */
int
crypto_kdispatch(struct cryptkop *krp)
{
	int error;
	unsigned long q_flags;

	cryptostats.cs_kops++;

	error = crypto_kinvoke(krp, krp->krp_crid);
	if (error == ERESTART) {
		CRYPTO_Q_LOCK();
		TAILQ_INSERT_TAIL(&crp_kq, krp, krp_next);
		if (crp_sleep)
			wake_up_interruptible(&cryptoproc_wait);
		CRYPTO_Q_UNLOCK();
		error = 0;
	}
	return error;
}

/*
 * Verify a driver is suitable for the specified operation.
 */
static __inline int
kdriver_suitable(const struct cryptocap *cap, const struct cryptkop *krp)
{
	return (cap->cc_kalg[krp->krp_op] & CRYPTO_ALG_FLAG_SUPPORTED) != 0;
}

/*
 * Select a driver for an asym operation.  The driver must
 * support the necessary algorithm.  The caller can constrain
 * which device is selected with the flags parameter.  The
 * algorithm we use here is pretty stupid; just use the first
 * driver that supports the algorithms we need. If there are
 * multiple suitable drivers we choose the driver with the
 * fewest active operations.  We prefer hardware-backed
 * drivers to software ones when either may be used.
 */
static struct cryptocap *
crypto_select_kdriver(const struct cryptkop *krp, int flags)
{
	struct cryptocap *cap, *best, *blocked;
	int match, hid;

	CRYPTO_DRIVER_ASSERT();

	/*
	 * Look first for hardware crypto devices if permitted.
	 */
	if (flags & CRYPTOCAP_F_HARDWARE)
		match = CRYPTOCAP_F_HARDWARE;
	else
		match = CRYPTOCAP_F_SOFTWARE;
	best = NULL;
	blocked = NULL;
again:
	for (hid = 0; hid < crypto_drivers_num; hid++) {
		cap = &crypto_drivers[hid];
		/*
		 * If it's not initialized, is in the process of
		 * going away, or is not appropriate (hardware
		 * or software based on match), then skip.
		 */
		if (cap->cc_dev == NULL ||
		    (cap->cc_flags & CRYPTOCAP_F_CLEANUP) ||
		    (cap->cc_flags & match) == 0)
			continue;

		/* verify all the algorithms are supported. */
		if (kdriver_suitable(cap, krp)) {
			if (best == NULL ||
			    cap->cc_koperations < best->cc_koperations)
				best = cap;
		}
	}
	if (best != NULL)
		return best;
	if (match == CRYPTOCAP_F_HARDWARE && (flags & CRYPTOCAP_F_SOFTWARE)) {
		/* sort of an Algol 68-style for loop */
		match = CRYPTOCAP_F_SOFTWARE;
		goto again;
	}
	return best;
}

/*
 * Dispatch an assymetric crypto request.
 */
static int
crypto_kinvoke(struct cryptkop *krp, int crid)
{
	struct cryptocap *cap = NULL;
	int error;
	unsigned long d_flags;

	KASSERT(krp != NULL, ("%s: krp == NULL", __func__));
	KASSERT(krp->krp_callback != NULL,
	    ("%s: krp->crp_callback == NULL", __func__));

	CRYPTO_DRIVER_LOCK();
	if ((crid & (CRYPTOCAP_F_HARDWARE | CRYPTOCAP_F_SOFTWARE)) == 0) {
		cap = crypto_checkdriver(crid);
		if (cap != NULL) {
			/*
			 * Driver present, it must support the necessary
			 * algorithm and, if s/w drivers are excluded,
			 * it must be registered as hardware-backed.
			 */
			if (!kdriver_suitable(cap, krp) ||
			    (!crypto_devallowsoft &&
			     (cap->cc_flags & CRYPTOCAP_F_HARDWARE) == 0))
				cap = NULL;
		}
	} else {
		/*
		 * No requested driver; select based on crid flags.
		 */
		if (!crypto_devallowsoft)	/* NB: disallow s/w drivers */
			crid &= ~CRYPTOCAP_F_SOFTWARE;
		cap = crypto_select_kdriver(krp, crid);
	}
	if (cap != NULL && !cap->cc_kqblocked) {
		krp->krp_hid = cap - crypto_drivers;
		cap->cc_koperations++;
		CRYPTO_DRIVER_UNLOCK();
		error = CRYPTODEV_KPROCESS(cap->cc_dev, krp, 0);
		CRYPTO_DRIVER_LOCK();
		if (error == ERESTART) {
			cap->cc_koperations--;
			CRYPTO_DRIVER_UNLOCK();
			return (error);
		}
		/* return the actual device used */
		krp->krp_crid = krp->krp_hid;
	} else {
		/*
		 * NB: cap is !NULL if device is blocked; in
		 *     that case return ERESTART so the operation
		 *     is resubmitted if possible.
		 */
		error = (cap == NULL) ? ENODEV : ERESTART;
	}
	CRYPTO_DRIVER_UNLOCK();

	if (error) {
		krp->krp_status = error;
		crypto_kdone(krp);
	}
	return 0;
}


/*
 * Dispatch a crypto request to the appropriate crypto devices.
 */
static int
crypto_invoke(struct cryptocap *cap, struct cryptop *crp, int hint)
{
	KASSERT(crp != NULL, ("%s: crp == NULL", __func__));
	KASSERT(crp->crp_callback != NULL,
	    ("%s: crp->crp_callback == NULL", __func__));
	KASSERT(crp->crp_desc != NULL, ("%s: crp->crp_desc == NULL", __func__));

	dprintk("%s()\n", __FUNCTION__);

#ifdef CRYPTO_TIMING
	if (crypto_timing)
		crypto_tstat(&cryptostats.cs_invoke, &crp->crp_tstamp);
#endif
	if (cap->cc_flags & CRYPTOCAP_F_CLEANUP) {
		struct cryptodesc *crd;
		u_int64_t nid;

		/*
		 * Driver has unregistered; migrate the session and return
		 * an error to the caller so they'll resubmit the op.
		 *
		 * XXX: What if there are more already queued requests for this
		 *      session?
		 */
		crypto_freesession(crp->crp_sid);

		for (crd = crp->crp_desc; crd->crd_next; crd = crd->crd_next)
			crd->CRD_INI.cri_next = &(crd->crd_next->CRD_INI);

		/* XXX propagate flags from initial session? */
		if (crypto_newsession(&nid, &(crp->crp_desc->CRD_INI),
		    CRYPTOCAP_F_HARDWARE | CRYPTOCAP_F_SOFTWARE) == 0)
			crp->crp_sid = nid;

		crp->crp_etype = EAGAIN;
		crypto_done(crp);
		return 0;
	} else {
		/*
		 * Invoke the driver to process the request.
		 */
		return CRYPTODEV_PROCESS(cap->cc_dev, crp, hint);
	}
}

/*
 * Release a set of crypto descriptors.
 */
void
crypto_freereq(struct cryptop *crp)
{
	struct cryptodesc *crd;

	if (crp == NULL)
		return;

#ifdef DIAGNOSTIC
	{
		struct cryptop *crp2;
		unsigned long q_flags;

		CRYPTO_Q_LOCK();
		TAILQ_FOREACH(crp2, &crp_q, crp_next) {
			KASSERT(crp2 != crp,
			    ("Freeing cryptop from the crypto queue (%p).",
			    crp));
		}
		CRYPTO_Q_UNLOCK();
		CRYPTO_RETQ_LOCK();
		TAILQ_FOREACH(crp2, &crp_ret_q, crp_next) {
			KASSERT(crp2 != crp,
			    ("Freeing cryptop from the return queue (%p).",
			    crp));
		}
		CRYPTO_RETQ_UNLOCK();
	}
#endif

	while ((crd = crp->crp_desc) != NULL) {
		crp->crp_desc = crd->crd_next;
		kmem_cache_free(cryptodesc_zone, crd);
	}
	kmem_cache_free(cryptop_zone, crp);
}

/*
 * Acquire a set of crypto descriptors.
 */
struct cryptop *
crypto_getreq(int num)
{
	struct cryptodesc *crd;
	struct cryptop *crp;

	crp = kmem_cache_alloc(cryptop_zone, SLAB_ATOMIC);
	if (crp != NULL) {
		memset(crp, 0, sizeof(*crp));
		INIT_LIST_HEAD(&crp->crp_next);
		init_waitqueue_head(&crp->crp_waitq);
		while (num--) {
			crd = kmem_cache_alloc(cryptodesc_zone, SLAB_ATOMIC);
			if (crd == NULL) {
				crypto_freereq(crp);
				return NULL;
			}
			memset(crd, 0, sizeof(*crd));
			crd->crd_next = crp->crp_desc;
			crp->crp_desc = crd;
		}
	}
	return crp;
}

/*
 * Invoke the callback on behalf of the driver.
 */
void
crypto_done(struct cryptop *crp)
{
	unsigned long q_flags;

	dprintk("%s()\n", __FUNCTION__);
	if ((crp->crp_flags & CRYPTO_F_DONE) == 0) {
		crp->crp_flags |= CRYPTO_F_DONE;
		CRYPTO_Q_LOCK();
		crypto_q_cnt--;
		CRYPTO_Q_UNLOCK();
	} else
		printk("crypto: crypto_done op already done, flags 0x%x",
				crp->crp_flags);
	if (crp->crp_etype != 0)
		cryptostats.cs_errs++;
	/*
	 * CBIMM means unconditionally do the callback immediately;
	 * CBIFSYNC means do the callback immediately only if the
	 * operation was done synchronously.  Both are used to avoid
	 * doing extraneous context switches; the latter is mostly
	 * used with the software crypto driver.
	 */
	if ((crp->crp_flags & CRYPTO_F_CBIMM) ||
	    ((crp->crp_flags & CRYPTO_F_CBIFSYNC) &&
	     (CRYPTO_SESID2CAPS(crp->crp_sid) & CRYPTOCAP_F_SYNC))) {
		/*
		 * Do the callback directly.  This is ok when the
		 * callback routine does very little (e.g. the
		 * /dev/crypto callback method just does a wakeup).
		 */
		crp->crp_callback(crp);
	} else {
		unsigned long r_flags;
		/*
		 * Normal case; queue the callback for the thread.
		 */
		CRYPTO_RETQ_LOCK();
		if (CRYPTO_RETQ_EMPTY())
			wake_up_interruptible(&cryptoretproc_wait);/* shared wait channel */
		TAILQ_INSERT_TAIL(&crp_ret_q, crp, crp_next);
		CRYPTO_RETQ_UNLOCK();
	}
}

/*
 * Invoke the callback on behalf of the driver.
 */
void
crypto_kdone(struct cryptkop *krp)
{
	struct cryptocap *cap;
	unsigned long d_flags;

	if ((krp->krp_flags & CRYPTO_KF_DONE) != 0)
		printk("crypto: crypto_kdone op already done, flags 0x%x",
				krp->krp_flags);
	krp->krp_flags |= CRYPTO_KF_DONE;
	if (krp->krp_status != 0)
		cryptostats.cs_kerrs++;

	CRYPTO_DRIVER_LOCK();
	/* XXX: What if driver is loaded in the meantime? */
	if (krp->krp_hid < crypto_drivers_num) {
		cap = &crypto_drivers[krp->krp_hid];
		cap->cc_koperations--;
		KASSERT(cap->cc_koperations >= 0, ("cc_koperations < 0"));
		if (cap->cc_flags & CRYPTOCAP_F_CLEANUP)
			crypto_remove(cap);
	}
	CRYPTO_DRIVER_UNLOCK();

	/*
	 * CBIMM means unconditionally do the callback immediately;
	 * This is used to avoid doing extraneous context switches
	 */
	if ((krp->krp_flags & CRYPTO_KF_CBIMM)) {
		/*
		 * Do the callback directly.  This is ok when the
		 * callback routine does very little (e.g. the
		 * /dev/crypto callback method just does a wakeup).
		 */
		krp->krp_callback(krp);
	} else {
		unsigned long r_flags;
		/*
		 * Normal case; queue the callback for the thread.
		 */
		CRYPTO_RETQ_LOCK();
		if (CRYPTO_RETQ_EMPTY())
			wake_up_interruptible(&cryptoretproc_wait);/* shared wait channel */
		TAILQ_INSERT_TAIL(&crp_ret_kq, krp, krp_next);
		CRYPTO_RETQ_UNLOCK();
	}
}

int
crypto_getfeat(int *featp)
{
	int hid, kalg, feat = 0;
	unsigned long d_flags;

	CRYPTO_DRIVER_LOCK();
	for (hid = 0; hid < crypto_drivers_num; hid++) {
		const struct cryptocap *cap = &crypto_drivers[hid];

		if ((cap->cc_flags & CRYPTOCAP_F_SOFTWARE) &&
		    !crypto_devallowsoft) {
			continue;
		}
		for (kalg = 0; kalg < CRK_ALGORITHM_MAX; kalg++)
			if (cap->cc_kalg[kalg] & CRYPTO_ALG_FLAG_SUPPORTED)
				feat |=  1 << kalg;
	}
	CRYPTO_DRIVER_UNLOCK();
	*featp = feat;
	return (0);
}

/*
 * Crypto thread, dispatches crypto requests.
 */
static int
crypto_proc(void *arg)
{
	struct cryptop *crp, *submit;
	struct cryptkop *krp, *krpp;
	struct cryptocap *cap;
	u_int32_t hid;
	int result, hint;
	unsigned long q_flags;
	int loopcount = 0;

	ocf_daemonize("crypto");

	CRYPTO_Q_LOCK();
	for (;;) {
		/*
		 * we need to make sure we don't get into a busy loop with nothing
		 * to do,  the two crypto_all_*blocked vars help us find out when
		 * we are all full and can do nothing on any driver or Q.  If so we
		 * wait for an unblock.
		 */
		crypto_all_qblocked  = !list_empty(&crp_q);

		/*
		 * Find the first element in the queue that can be
		 * processed and look-ahead to see if multiple ops
		 * are ready for the same driver.
		 */
		submit = NULL;
		hint = 0;
		list_for_each_entry(crp, &crp_q, crp_next) {
			hid = CRYPTO_SESID2HID(crp->crp_sid);
			cap = crypto_checkdriver(hid);
			/*
			 * Driver cannot disappear when there is an active
			 * session.
			 */
			KASSERT(cap != NULL, ("%s:%u Driver disappeared.",
			    __func__, __LINE__));
			if (cap == NULL || cap->cc_dev == NULL) {
				/* Op needs to be migrated, process it. */
				if (submit == NULL)
					submit = crp;
				break;
			}
			if (!cap->cc_qblocked) {
				if (submit != NULL) {
					/*
					 * We stop on finding another op,
					 * regardless whether its for the same
					 * driver or not.  We could keep
					 * searching the queue but it might be
					 * better to just use a per-driver
					 * queue instead.
					 */
					if (CRYPTO_SESID2HID(submit->crp_sid) == hid)
						hint = CRYPTO_HINT_MORE;
					break;
				} else {
					submit = crp;
					if ((submit->crp_flags & CRYPTO_F_BATCH) == 0)
						break;
					/* keep scanning for more are q'd */
				}
			}
		}
		if (submit != NULL) {
			hid = CRYPTO_SESID2HID(submit->crp_sid);
			crypto_all_qblocked = 0;
			list_del(&submit->crp_next);
			crypto_drivers[hid].cc_unqblocked = 1;
			cap = crypto_checkdriver(hid);
			CRYPTO_Q_UNLOCK();
			KASSERT(cap != NULL, ("%s:%u Driver disappeared.",
			    __func__, __LINE__));
			result = crypto_invoke(cap, submit, hint);
			CRYPTO_Q_LOCK();
			if (result == ERESTART) {
				/*
				 * The driver ran out of resources, mark the
				 * driver ``blocked'' for cryptop's and put
				 * the request back in the queue.  It would
				 * best to put the request back where we got
				 * it but that's hard so for now we put it
				 * at the front.  This should be ok; putting
				 * it at the end does not work.
				 */
				/* XXX validate sid again? */
				list_add(&submit->crp_next, &crp_q);
				cryptostats.cs_blocks++;
				if (crypto_drivers[hid].cc_unqblocked)
					crypto_drivers[hid].cc_qblocked=0;
				crypto_drivers[hid].cc_unqblocked=0;
			}
			crypto_drivers[hid].cc_unqblocked = 0;
		}

		crypto_all_kqblocked = !list_empty(&crp_kq);

		/* As above, but for key ops */
		krp = NULL;
		list_for_each_entry(krpp, &crp_kq, krp_next) {
			cap = crypto_checkdriver(krpp->krp_hid);
			if (cap == NULL || cap->cc_dev == NULL) {
				/*
				 * Operation needs to be migrated, invalidate
				 * the assigned device so it will reselect a
				 * new one below.  Propagate the original
				 * crid selection flags if supplied.
				 */
				krp->krp_hid = krp->krp_crid &
				    (CRYPTOCAP_F_SOFTWARE|CRYPTOCAP_F_HARDWARE);
				if (krp->krp_hid == 0)
					krp->krp_hid =
				    CRYPTOCAP_F_SOFTWARE|CRYPTOCAP_F_HARDWARE;
				break;
			}
			if (!cap->cc_kqblocked) {
				krp = krpp;
				break;
			}
		}
		if (krp != NULL) {
			crypto_all_kqblocked = 0;
			list_del(&krp->krp_next);
			crypto_drivers[krp->krp_hid].cc_kqblocked = 1;
			CRYPTO_Q_UNLOCK();
			result = crypto_kinvoke(krp, krp->krp_hid);
			CRYPTO_Q_LOCK();
			if (result == ERESTART) {
				/*
				 * The driver ran out of resources, mark the
				 * driver ``blocked'' for cryptkop's and put
				 * the request back in the queue.  It would
				 * best to put the request back where we got
				 * it but that's hard so for now we put it
				 * at the front.  This should be ok; putting
				 * it at the end does not work.
				 */
				/* XXX validate sid again? */
				list_add(&krp->krp_next, &crp_kq);
				cryptostats.cs_kblocks++;
			} else
				crypto_drivers[krp->krp_hid].cc_kqblocked = 0;
		}

		if (submit == NULL && krp == NULL) {
			/*
			 * Nothing more to be processed.  Sleep until we're
			 * woken because there are more ops to process.
			 * This happens either by submission or by a driver
			 * becoming unblocked and notifying us through
			 * crypto_unblock.  Note that when we wakeup we
			 * start processing each queue again from the
			 * front. It's not clear that it's important to
			 * preserve this ordering since ops may finish
			 * out of order if dispatched to different devices
			 * and some become blocked while others do not.
			 */
			dprintk("%s - sleeping (qe=%d qb=%d kqe=%d kqb=%d)\n",
					__FUNCTION__,
					list_empty(&crp_q), crypto_all_qblocked,
					list_empty(&crp_kq), crypto_all_kqblocked);
			loopcount = 0;
			CRYPTO_Q_UNLOCK();
			crp_sleep = 1;
			wait_event_interruptible(cryptoproc_wait,
					!(list_empty(&crp_q) || crypto_all_qblocked) ||
					!(list_empty(&crp_kq) || crypto_all_kqblocked) ||
					cryptoproc == (pid_t) -1);
			crp_sleep = 0;
			if (signal_pending (current)) {
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,0)
				spin_lock_irq(&current->sigmask_lock);
#endif
				flush_signals(current);
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,0)
				spin_unlock_irq(&current->sigmask_lock);
#endif
			}
			CRYPTO_Q_LOCK();
			dprintk("%s - awake\n", __FUNCTION__);
			if (cryptoproc == (pid_t) -1)
				break;
			cryptostats.cs_intrs++;
		} else if (loopcount > crypto_max_loopcount) {
			/*
			 * Give other processes a chance to run if we've 
			 * been using the CPU exclusively for a while.
			 */
			loopcount = 0;
			schedule();
		}
		loopcount++;
	}
	CRYPTO_Q_UNLOCK();
	complete_and_exit(&cryptoproc_exited, 0);
}

/*
 * Crypto returns thread, does callbacks for processed crypto requests.
 * Callbacks are done here, rather than in the crypto drivers, because
 * callbacks typically are expensive and would slow interrupt handling.
 */
static int
crypto_ret_proc(void *arg)
{
	struct cryptop *crpt;
	struct cryptkop *krpt;
	unsigned long  r_flags;

	ocf_daemonize("crypto_ret");

	CRYPTO_RETQ_LOCK();
	for (;;) {
		/* Harvest return q's for completed ops */
		crpt = NULL;
		if (!list_empty(&crp_ret_q))
			crpt = list_entry(crp_ret_q.next, typeof(*crpt), crp_next);
		if (crpt != NULL)
			list_del(&crpt->crp_next);

		krpt = NULL;
		if (!list_empty(&crp_ret_kq))
			krpt = list_entry(crp_ret_kq.next, typeof(*krpt), krp_next);
		if (krpt != NULL)
			list_del(&krpt->krp_next);

		if (crpt != NULL || krpt != NULL) {
			CRYPTO_RETQ_UNLOCK();
			/*
			 * Run callbacks unlocked.
			 */
			if (crpt != NULL)
				crpt->crp_callback(crpt);
			if (krpt != NULL)
				krpt->krp_callback(krpt);
			CRYPTO_RETQ_LOCK();
		} else {
			/*
			 * Nothing more to be processed.  Sleep until we're
			 * woken because there are more returns to process.
			 */
			dprintk("%s - sleeping\n", __FUNCTION__);
			CRYPTO_RETQ_UNLOCK();
			wait_event_interruptible(cryptoretproc_wait,
					cryptoretproc == (pid_t) -1 ||
					!list_empty(&crp_ret_q) ||
					!list_empty(&crp_ret_kq));
			if (signal_pending (current)) {
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,0)
				spin_lock_irq(&current->sigmask_lock);
#endif
				flush_signals(current);
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,0)
				spin_unlock_irq(&current->sigmask_lock);
#endif
			}
			CRYPTO_RETQ_LOCK();
			dprintk("%s - awake\n", __FUNCTION__);
			if (cryptoretproc == (pid_t) -1) {
				dprintk("%s - EXITING!\n", __FUNCTION__);
				break;
			}
			cryptostats.cs_rets++;
		}
	}
	CRYPTO_RETQ_UNLOCK();
	complete_and_exit(&cryptoretproc_exited, 0);
}


#if 0 /* should put this into /proc or something */
static void
db_show_drivers(void)
{
	int hid;

	db_printf("%12s %4s %4s %8s %2s %2s\n"
		, "Device"
		, "Ses"
		, "Kops"
		, "Flags"
		, "QB"
		, "KB"
	);
	for (hid = 0; hid < crypto_drivers_num; hid++) {
		const struct cryptocap *cap = &crypto_drivers[hid];
		if (cap->cc_dev == NULL)
			continue;
		db_printf("%-12s %4u %4u %08x %2u %2u\n"
		    , device_get_nameunit(cap->cc_dev)
		    , cap->cc_sessions
		    , cap->cc_koperations
		    , cap->cc_flags
		    , cap->cc_qblocked
		    , cap->cc_kqblocked
		);
	}
}

DB_SHOW_COMMAND(crypto, db_show_crypto)
{
	struct cryptop *crp;

	db_show_drivers();
	db_printf("\n");

	db_printf("%4s %8s %4s %4s %4s %4s %8s %8s\n",
	    "HID", "Caps", "Ilen", "Olen", "Etype", "Flags",
	    "Desc", "Callback");
	TAILQ_FOREACH(crp, &crp_q, crp_next) {
		db_printf("%4u %08x %4u %4u %4u %04x %8p %8p\n"
		    , (int) CRYPTO_SESID2HID(crp->crp_sid)
		    , (int) CRYPTO_SESID2CAPS(crp->crp_sid)
		    , crp->crp_ilen, crp->crp_olen
		    , crp->crp_etype
		    , crp->crp_flags
		    , crp->crp_desc
		    , crp->crp_callback
		);
	}
	if (!TAILQ_EMPTY(&crp_ret_q)) {
		db_printf("\n%4s %4s %4s %8s\n",
		    "HID", "Etype", "Flags", "Callback");
		TAILQ_FOREACH(crp, &crp_ret_q, crp_next) {
			db_printf("%4u %4u %04x %8p\n"
			    , (int) CRYPTO_SESID2HID(crp->crp_sid)
			    , crp->crp_etype
			    , crp->crp_flags
			    , crp->crp_callback
			);
		}
	}
}

DB_SHOW_COMMAND(kcrypto, db_show_kcrypto)
{
	struct cryptkop *krp;

	db_show_drivers();
	db_printf("\n");

	db_printf("%4s %5s %4s %4s %8s %4s %8s\n",
	    "Op", "Status", "#IP", "#OP", "CRID", "HID", "Callback");
	TAILQ_FOREACH(krp, &crp_kq, krp_next) {
		db_printf("%4u %5u %4u %4u %08x %4u %8p\n"
		    , krp->krp_op
		    , krp->krp_status
		    , krp->krp_iparams, krp->krp_oparams
		    , krp->krp_crid, krp->krp_hid
		    , krp->krp_callback
		);
	}
	if (!TAILQ_EMPTY(&crp_ret_q)) {
		db_printf("%4s %5s %8s %4s %8s\n",
		    "Op", "Status", "CRID", "HID", "Callback");
		TAILQ_FOREACH(krp, &crp_ret_kq, krp_next) {
			db_printf("%4u %5u %08x %4u %8p\n"
			    , krp->krp_op
			    , krp->krp_status
			    , krp->krp_crid, krp->krp_hid
			    , krp->krp_callback
			);
		}
	}
}
#endif


static int
crypto_init(void)
{
	int error;

	dprintk("%s(%p)\n", __FUNCTION__, (void *) crypto_init);

	if (crypto_initted)
		return 0;
	crypto_initted = 1;

	spin_lock_init(&crypto_drivers_lock);
	spin_lock_init(&crypto_q_lock);
	spin_lock_init(&crypto_ret_q_lock);

	cryptop_zone = kmem_cache_create("cryptop", sizeof(struct cryptop),
				       0, SLAB_HWCACHE_ALIGN, NULL
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,23)
				       , NULL
#endif
					);

	cryptodesc_zone = kmem_cache_create("cryptodesc", sizeof(struct cryptodesc),
				       0, SLAB_HWCACHE_ALIGN, NULL
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,23)
				       , NULL
#endif
					);

	if (cryptodesc_zone == NULL || cryptop_zone == NULL) {
		printk("crypto: crypto_init cannot setup crypto zones\n");
		error = ENOMEM;
		goto bad;
	}

	crypto_drivers_num = CRYPTO_DRIVERS_INITIAL;
	crypto_drivers = kmalloc(crypto_drivers_num * sizeof(struct cryptocap),
			GFP_KERNEL);
	if (crypto_drivers == NULL) {
		printk("crypto: crypto_init cannot setup crypto drivers\n");
		error = ENOMEM;
		goto bad;
	}

	memset(crypto_drivers, 0, crypto_drivers_num * sizeof(struct cryptocap));

	init_completion(&cryptoproc_exited);
	init_completion(&cryptoretproc_exited);

	cryptoproc = 0; /* to avoid race condition where proc runs first */
	cryptoproc = kernel_thread(crypto_proc, NULL, CLONE_FS|CLONE_FILES);
	if (cryptoproc < 0) {
		error = cryptoproc;
		printk("crypto: crypto_init cannot start crypto thread; error %d",
			error);
		goto bad;
	}

	cryptoretproc = 0; /* to avoid race condition where proc runs first */
	cryptoretproc = kernel_thread(crypto_ret_proc, NULL, CLONE_FS|CLONE_FILES);
	if (cryptoretproc < 0) {
		error = cryptoretproc;
		printk("crypto: crypto_init cannot start cryptoret thread; error %d",
				error);
		goto bad;
	}

	return 0;
bad:
	crypto_exit();
	return error;
}


static void
crypto_exit(void)
{
	pid_t p;
	unsigned long d_flags;

	dprintk("%s()\n", __FUNCTION__);

	/*
	 * Terminate any crypto threads.
	 */

	CRYPTO_DRIVER_LOCK();
	p = cryptoproc;
	cryptoproc = (pid_t) -1;
	kill_proc(p, SIGTERM, 1);
	wake_up_interruptible(&cryptoproc_wait);
	CRYPTO_DRIVER_UNLOCK();

	wait_for_completion(&cryptoproc_exited);

	CRYPTO_DRIVER_LOCK();
	p = cryptoretproc;
	cryptoretproc = (pid_t) -1;
	kill_proc(p, SIGTERM, 1);
	wake_up_interruptible(&cryptoretproc_wait);
	CRYPTO_DRIVER_UNLOCK();

	wait_for_completion(&cryptoretproc_exited);

	/* XXX flush queues??? */

	/* 
	 * Reclaim dynamically allocated resources.
	 */
	if (crypto_drivers != NULL)
		kfree(crypto_drivers);

	if (cryptodesc_zone != NULL)
		kmem_cache_destroy(cryptodesc_zone);
	if (cryptop_zone != NULL)
		kmem_cache_destroy(cryptop_zone);
}


EXPORT_SYMBOL(crypto_newsession);
EXPORT_SYMBOL(crypto_freesession);
EXPORT_SYMBOL(crypto_get_driverid);
EXPORT_SYMBOL(crypto_kregister);
EXPORT_SYMBOL(crypto_register);
EXPORT_SYMBOL(crypto_unregister);
EXPORT_SYMBOL(crypto_unregister_all);
EXPORT_SYMBOL(crypto_unblock);
EXPORT_SYMBOL(crypto_dispatch);
EXPORT_SYMBOL(crypto_kdispatch);
EXPORT_SYMBOL(crypto_freereq);
EXPORT_SYMBOL(crypto_getreq);
EXPORT_SYMBOL(crypto_done);
EXPORT_SYMBOL(crypto_kdone);
EXPORT_SYMBOL(crypto_getfeat);
EXPORT_SYMBOL(crypto_userasymcrypto);
EXPORT_SYMBOL(crypto_getcaps);
EXPORT_SYMBOL(crypto_find_driver);
EXPORT_SYMBOL(crypto_find_device_byhid);

module_init(crypto_init);
module_exit(crypto_exit);

MODULE_LICENSE("BSD");
MODULE_AUTHOR("David McCullough <david_mccullough@mcafee.com>");
MODULE_DESCRIPTION("OCF (OpenBSD Cryptographic Framework)");

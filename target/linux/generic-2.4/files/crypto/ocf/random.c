/*
 * A system independant way of adding entropy to the kernels pool
 * this way the drivers can focus on the real work and we can take
 * care of pushing it to the appropriate place in the kernel.
 *
 * This should be fast and callable from timers/interrupts
 *
 * Written by David McCullough <david_mccullough@mcafee.com>
 * Copyright (C) 2006-2010 David McCullough
 * Copyright (C) 2004-2005 Intel Corporation.
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
 * ALTERNATIVELY, provided that this notice is retained in full, this product
 * may be distributed under the terms of the GNU General Public License (GPL),
 * in which case the provisions of the GPL apply INSTEAD OF those given above.
 *
 * DISCLAIMER
 *
 * This software is provided 'as is' with no explicit or implied warranties
 * in respect of its properties, including, but not limited to, correctness
 * and/or fitness for purpose.
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
#include <linux/unistd.h>
#include <linux/poll.h>
#include <linux/random.h>
#include <cryptodev.h>

#ifdef CONFIG_OCF_FIPS
#include "rndtest.h"
#endif

#ifndef HAS_RANDOM_INPUT_WAIT
#error "Please do not enable OCF_RANDOMHARVEST unless you have applied patches"
#endif

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,27)
#include <linux/sched.h>
#define	kill_proc(p,s,v)	send_sig(s,find_task_by_vpid(p),0)
#endif

/*
 * a hack to access the debug levels from the crypto driver
 */
extern int crypto_debug;
#define debug crypto_debug

/*
 * a list of all registered random providers
 */
static LIST_HEAD(random_ops);
static int started = 0;
static int initted = 0;

struct random_op {
	struct list_head random_list;
	u_int32_t driverid;
	int (*read_random)(void *arg, u_int32_t *buf, int len);
	void *arg;
};

static int random_proc(void *arg);

static pid_t		randomproc = (pid_t) -1;
static spinlock_t	random_lock;

/*
 * just init the spin locks
 */
static int
crypto_random_init(void)
{
	spin_lock_init(&random_lock);
	initted = 1;
	return(0);
}

/*
 * Add the given random reader to our list (if not present)
 * and start the thread (if not already started)
 *
 * we have to assume that driver id is ok for now
 */
int
crypto_rregister(
	u_int32_t driverid,
	int (*read_random)(void *arg, u_int32_t *buf, int len),
	void *arg)
{
	unsigned long flags;
	int ret = 0;
	struct random_op	*rops, *tmp;

	dprintk("%s,%d: %s(0x%x, %p, %p)\n", __FILE__, __LINE__,
			__FUNCTION__, driverid, read_random, arg);

	if (!initted)
		crypto_random_init();

#if 0
	struct cryptocap	*cap;

	cap = crypto_checkdriver(driverid);
	if (!cap)
		return EINVAL;
#endif

	list_for_each_entry_safe(rops, tmp, &random_ops, random_list) {
		if (rops->driverid == driverid && rops->read_random == read_random)
			return EEXIST;
	}

	rops = (struct random_op *) kmalloc(sizeof(*rops), GFP_KERNEL);
	if (!rops)
		return ENOMEM;

	rops->driverid    = driverid;
	rops->read_random = read_random;
	rops->arg = arg;

	spin_lock_irqsave(&random_lock, flags);
	list_add_tail(&rops->random_list, &random_ops);
	if (!started) {
		randomproc = kernel_thread(random_proc, NULL, CLONE_FS|CLONE_FILES);
		if (randomproc < 0) {
			ret = randomproc;
			printk("crypto: crypto_rregister cannot start random thread; "
					"error %d", ret);
		} else
			started = 1;
	}
	spin_unlock_irqrestore(&random_lock, flags);

	return ret;
}
EXPORT_SYMBOL(crypto_rregister);

int
crypto_runregister_all(u_int32_t driverid)
{
	struct random_op *rops, *tmp;
	unsigned long flags;

	dprintk("%s,%d: %s(0x%x)\n", __FILE__, __LINE__, __FUNCTION__, driverid);

	list_for_each_entry_safe(rops, tmp, &random_ops, random_list) {
		if (rops->driverid == driverid) {
			list_del(&rops->random_list);
			kfree(rops);
		}
	}

	spin_lock_irqsave(&random_lock, flags);
	if (list_empty(&random_ops) && started)
		kill_proc(randomproc, SIGKILL, 1);
	spin_unlock_irqrestore(&random_lock, flags);
	return(0);
}
EXPORT_SYMBOL(crypto_runregister_all);

/*
 * while we can add entropy to random.c continue to read random data from
 * the drivers and push it to random.
 */
static int
random_proc(void *arg)
{
	int n;
	int wantcnt;
	int bufcnt = 0;
	int retval = 0;
	int *buf = NULL;

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,0)
	daemonize();
	spin_lock_irq(&current->sigmask_lock);
	sigemptyset(&current->blocked);
	recalc_sigpending(current);
	spin_unlock_irq(&current->sigmask_lock);
	sprintf(current->comm, "ocf-random");
#else
	daemonize("ocf-random");
	allow_signal(SIGKILL);
#endif

	(void) get_fs();
	set_fs(get_ds());

#ifdef CONFIG_OCF_FIPS
#define NUM_INT (RNDTEST_NBYTES/sizeof(int))
#else
#define NUM_INT 32
#endif

	/*
	 * some devices can transferr their RNG data direct into memory,
	 * so make sure it is device friendly
	 */
	buf = kmalloc(NUM_INT * sizeof(int), GFP_DMA);
	if (NULL == buf) {
		printk("crypto: RNG could not allocate memory\n");
		retval = -ENOMEM;
		goto bad_alloc;
	}

	wantcnt = NUM_INT;   /* start by adding some entropy */

	/*
	 * its possible due to errors or driver removal that we no longer
	 * have anything to do,  if so exit or we will consume all the CPU
	 * doing nothing
	 */
	while (!list_empty(&random_ops)) {
		struct random_op	*rops, *tmp;

#ifdef CONFIG_OCF_FIPS
		if (wantcnt)
			wantcnt = NUM_INT; /* FIPs mode can do 20000 bits or none */
#endif

		/* see if we can get enough entropy to make the world
		 * a better place.
		 */
		while (bufcnt < wantcnt && bufcnt < NUM_INT) {
			list_for_each_entry_safe(rops, tmp, &random_ops, random_list) {

				n = (*rops->read_random)(rops->arg, &buf[bufcnt],
							 NUM_INT - bufcnt);

				/* on failure remove the random number generator */
				if (n == -1) {
					list_del(&rops->random_list);
					printk("crypto: RNG (driverid=0x%x) failed, disabling\n",
							rops->driverid);
					kfree(rops);
				} else if (n > 0)
					bufcnt += n;
			}
			/* give up CPU for a bit, just in case as this is a loop */
			schedule();
		}


#ifdef CONFIG_OCF_FIPS
		if (bufcnt > 0 && rndtest_buf((unsigned char *) &buf[0])) {
			dprintk("crypto: buffer had fips errors, discarding\n");
			bufcnt = 0;
		}
#endif

		/*
		 * if we have a certified buffer,  we can send some data
		 * to /dev/random and move along
		 */
		if (bufcnt > 0) {
			/* add what we have */
			random_input_words(buf, bufcnt, bufcnt*sizeof(int)*8);
			bufcnt = 0;
		}

		/* give up CPU for a bit so we don't hog while filling */
		schedule();

		/* wait for needing more */
		wantcnt = random_input_wait();

		if (wantcnt <= 0)
			wantcnt = 0; /* try to get some info again */
		else
		 	/* round up to one word or we can loop forever */
			wantcnt = (wantcnt + (sizeof(int)*8)) / (sizeof(int)*8);
		if (wantcnt > NUM_INT) {
			wantcnt = NUM_INT;
		}

		if (signal_pending(current)) {
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,0)
			spin_lock_irq(&current->sigmask_lock);
#endif
			flush_signals(current);
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,0)
			spin_unlock_irq(&current->sigmask_lock);
#endif
		}
	}
	
	kfree(buf);

bad_alloc:
	spin_lock_irq(&random_lock);
	randomproc = (pid_t) -1;
	started = 0;
	spin_unlock_irq(&random_lock);

	return retval;
}


/*
 * An OCF module for determining the cost of crypto versus the cost of
 * IPSec processing outside of OCF.  This modules gives us the effect of
 * zero cost encryption,  of course you will need to run it at both ends
 * since it does no crypto at all.
 *
 * Written by David McCullough <david_mccullough@securecomputing.com>
 * Copyright (C) 2006-2007 David McCullough 
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
#include <linux/sched.h>
#include <linux/wait.h>
#include <linux/crypto.h>
#include <linux/interrupt.h>

#include <cryptodev.h>
#include <uio.h>

static int32_t			 null_id = -1;
static u_int32_t		 null_sesnum = 0;

static int null_process(device_t, struct cryptop *, int);
static int null_newsession(device_t, u_int32_t *, struct cryptoini *);
static int null_freesession(device_t, u_int64_t);

#define debug ocfnull_debug
int ocfnull_debug = 0;
module_param(ocfnull_debug, int, 0644);
MODULE_PARM_DESC(ocfnull_debug, "Enable debug");

/*
 * dummy device structure
 */

static struct {
	softc_device_decl	sc_dev;
} nulldev;

static device_method_t null_methods = {
	/* crypto device methods */
	DEVMETHOD(cryptodev_newsession,	null_newsession),
	DEVMETHOD(cryptodev_freesession,null_freesession),
	DEVMETHOD(cryptodev_process,	null_process),
};

/*
 * Generate a new software session.
 */
static int
null_newsession(device_t arg, u_int32_t *sid, struct cryptoini *cri)
{
	dprintk("%s()\n", __FUNCTION__);
	if (sid == NULL || cri == NULL) {
		dprintk("%s,%d - EINVAL\n", __FILE__, __LINE__);
		return EINVAL;
	}

	if (null_sesnum == 0)
		null_sesnum++;
	*sid = null_sesnum++;
	return 0;
}


/*
 * Free a session.
 */
static int
null_freesession(device_t arg, u_int64_t tid)
{
	u_int32_t sid = CRYPTO_SESID2LID(tid);

	dprintk("%s()\n", __FUNCTION__);
	if (sid > null_sesnum) {
		dprintk("%s,%d: EINVAL\n", __FILE__, __LINE__);
		return EINVAL;
	}

	/* Silently accept and return */
	if (sid == 0)
		return 0;
	return 0;
}


/*
 * Process a request.
 */
static int
null_process(device_t arg, struct cryptop *crp, int hint)
{
	unsigned int lid;

	dprintk("%s()\n", __FUNCTION__);

	/* Sanity check */
	if (crp == NULL) {
		dprintk("%s,%d: EINVAL\n", __FILE__, __LINE__);
		return EINVAL;
	}

	crp->crp_etype = 0;

	if (crp->crp_desc == NULL || crp->crp_buf == NULL) {
		dprintk("%s,%d: EINVAL\n", __FILE__, __LINE__);
		crp->crp_etype = EINVAL;
		goto done;
	}

	/*
	 * find the session we are using
	 */

	lid = crp->crp_sid & 0xffffffff;
	if (lid >= null_sesnum || lid == 0) {
		crp->crp_etype = ENOENT;
		dprintk("%s,%d: ENOENT\n", __FILE__, __LINE__);
		goto done;
	}

done:
	crypto_done(crp);
	return 0;
}


/*
 * our driver startup and shutdown routines
 */

static int
null_init(void)
{
	dprintk("%s(%p)\n", __FUNCTION__, null_init);

	memset(&nulldev, 0, sizeof(nulldev));
	softc_device_init(&nulldev, "ocfnull", 0, null_methods);

	null_id = crypto_get_driverid(softc_get_device(&nulldev),
				CRYPTOCAP_F_HARDWARE);
	if (null_id < 0)
		panic("ocfnull: crypto device cannot initialize!");

#define	REGISTER(alg) \
	crypto_register(null_id,alg,0,0)
	REGISTER(CRYPTO_DES_CBC);
	REGISTER(CRYPTO_3DES_CBC);
	REGISTER(CRYPTO_RIJNDAEL128_CBC);
	REGISTER(CRYPTO_MD5);
	REGISTER(CRYPTO_SHA1);
	REGISTER(CRYPTO_MD5_HMAC);
	REGISTER(CRYPTO_SHA1_HMAC);
#undef REGISTER

	return 0;
}

static void
null_exit(void)
{
	dprintk("%s()\n", __FUNCTION__);
	crypto_unregister_all(null_id);
	null_id = -1;
}

module_init(null_init);
module_exit(null_exit);

MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("David McCullough <david_mccullough@securecomputing.com>");
MODULE_DESCRIPTION("ocfnull - claims a lot but does nothing");

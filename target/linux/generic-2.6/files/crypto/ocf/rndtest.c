/*	$OpenBSD$	*/

/*
 * OCF/Linux port done by David McCullough <david_mccullough@securecomputing.com>
 * Copyright (C) 2006-2007 David McCullough
 * Copyright (C) 2004-2005 Intel Corporation.
 * The license and original author are listed below.
 *
 * Copyright (c) 2002 Jason L. Wright (jason@thought.net)
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *	This product includes software developed by Jason L. Wright
 * 4. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef AUTOCONF_INCLUDED
#include <linux/config.h>
#endif
#include <linux/module.h>
#include <linux/list.h>
#include <linux/wait.h>
#include <linux/time.h>
#include <linux/version.h>
#include <linux/unistd.h>
#include <linux/kernel.h>
#include <linux/string.h>
#include <linux/time.h>
#include <cryptodev.h>
#include "rndtest.h"

static struct rndtest_stats rndstats;

static	void rndtest_test(struct rndtest_state *);

/* The tests themselves */
static	int rndtest_monobit(struct rndtest_state *);
static	int rndtest_runs(struct rndtest_state *);
static	int rndtest_longruns(struct rndtest_state *);
static	int rndtest_chi_4(struct rndtest_state *);

static	int rndtest_runs_check(struct rndtest_state *, int, int *);
static	void rndtest_runs_record(struct rndtest_state *, int, int *);

static const struct rndtest_testfunc {
	int (*test)(struct rndtest_state *);
} rndtest_funcs[] = {
	{ rndtest_monobit },
	{ rndtest_runs },
	{ rndtest_chi_4 },
	{ rndtest_longruns },
};

#define	RNDTEST_NTESTS	(sizeof(rndtest_funcs)/sizeof(rndtest_funcs[0]))

static void
rndtest_test(struct rndtest_state *rsp)
{
	int i, rv = 0;

	rndstats.rst_tests++;
	for (i = 0; i < RNDTEST_NTESTS; i++)
		rv |= (*rndtest_funcs[i].test)(rsp);
	rsp->rs_discard = (rv != 0);
}


extern int crypto_debug;
#define rndtest_verbose 2
#define rndtest_report(rsp, failure, fmt, a...) \
	{ if (failure || crypto_debug) { printk("rng_test: " fmt "\n", a); } else; }

#define	RNDTEST_MONOBIT_MINONES	9725
#define	RNDTEST_MONOBIT_MAXONES	10275

static int
rndtest_monobit(struct rndtest_state *rsp)
{
	int i, ones = 0, j;
	u_int8_t r;

	for (i = 0; i < RNDTEST_NBYTES; i++) {
		r = rsp->rs_buf[i];
		for (j = 0; j < 8; j++, r <<= 1)
			if (r & 0x80)
				ones++;
	}
	if (ones > RNDTEST_MONOBIT_MINONES &&
	    ones < RNDTEST_MONOBIT_MAXONES) {
		if (rndtest_verbose > 1)
			rndtest_report(rsp, 0, "monobit pass (%d < %d < %d)",
			    RNDTEST_MONOBIT_MINONES, ones,
			    RNDTEST_MONOBIT_MAXONES);
		return (0);
	} else {
		if (rndtest_verbose)
			rndtest_report(rsp, 1,
			    "monobit failed (%d ones)", ones);
		rndstats.rst_monobit++;
		return (-1);
	}
}

#define	RNDTEST_RUNS_NINTERVAL	6

static const struct rndtest_runs_tabs {
	u_int16_t min, max;
} rndtest_runs_tab[] = {
	{ 2343, 2657 },
	{ 1135, 1365 },
	{ 542, 708 },
	{ 251, 373 },
	{ 111, 201 },
	{ 111, 201 },
};

static int
rndtest_runs(struct rndtest_state *rsp)
{
	int i, j, ones, zeros, rv = 0;
	int onei[RNDTEST_RUNS_NINTERVAL], zeroi[RNDTEST_RUNS_NINTERVAL];
	u_int8_t c;

	bzero(onei, sizeof(onei));
	bzero(zeroi, sizeof(zeroi));
	ones = zeros = 0;
	for (i = 0; i < RNDTEST_NBYTES; i++) {
		c = rsp->rs_buf[i];
		for (j = 0; j < 8; j++, c <<= 1) {
			if (c & 0x80) {
				ones++;
				rndtest_runs_record(rsp, zeros, zeroi);
				zeros = 0;
			} else {
				zeros++;
				rndtest_runs_record(rsp, ones, onei);
				ones = 0;
			}
		}
	}
	rndtest_runs_record(rsp, ones, onei);
	rndtest_runs_record(rsp, zeros, zeroi);

	rv |= rndtest_runs_check(rsp, 0, zeroi);
	rv |= rndtest_runs_check(rsp, 1, onei);

	if (rv)
		rndstats.rst_runs++;

	return (rv);
}

static void
rndtest_runs_record(struct rndtest_state *rsp, int len, int *intrv)
{
	if (len == 0)
		return;
	if (len > RNDTEST_RUNS_NINTERVAL)
		len = RNDTEST_RUNS_NINTERVAL;
	len -= 1;
	intrv[len]++;
}

static int
rndtest_runs_check(struct rndtest_state *rsp, int val, int *src)
{
	int i, rv = 0;

	for (i = 0; i < RNDTEST_RUNS_NINTERVAL; i++) {
		if (src[i] < rndtest_runs_tab[i].min ||
		    src[i] > rndtest_runs_tab[i].max) {
			rndtest_report(rsp, 1,
			    "%s interval %d failed (%d, %d-%d)",
			    val ? "ones" : "zeros",
			    i + 1, src[i], rndtest_runs_tab[i].min,
			    rndtest_runs_tab[i].max);
			rv = -1;
		} else {
			rndtest_report(rsp, 0,
			    "runs pass %s interval %d (%d < %d < %d)",
			    val ? "ones" : "zeros",
			    i + 1, rndtest_runs_tab[i].min, src[i],
			    rndtest_runs_tab[i].max);
		}
	}
	return (rv);
}

static int
rndtest_longruns(struct rndtest_state *rsp)
{
	int i, j, ones = 0, zeros = 0, maxones = 0, maxzeros = 0;
	u_int8_t c;

	for (i = 0; i < RNDTEST_NBYTES; i++) {
		c = rsp->rs_buf[i];
		for (j = 0; j < 8; j++, c <<= 1) {
			if (c & 0x80) {
				zeros = 0;
				ones++;
				if (ones > maxones)
					maxones = ones;
			} else {
				ones = 0;
				zeros++;
				if (zeros > maxzeros)
					maxzeros = zeros;
			}
		}
	}

	if (maxones < 26 && maxzeros < 26) {
		rndtest_report(rsp, 0, "longruns pass (%d ones, %d zeros)",
			maxones, maxzeros);
		return (0);
	} else {
		rndtest_report(rsp, 1, "longruns fail (%d ones, %d zeros)",
			maxones, maxzeros);
		rndstats.rst_longruns++;
		return (-1);
	}
}

/*
 * chi^2 test over 4 bits: (this is called the poker test in FIPS 140-2,
 * but it is really the chi^2 test over 4 bits (the poker test as described
 * by Knuth vol 2 is something different, and I take him as authoritative
 * on nomenclature over NIST).
 */
#define	RNDTEST_CHI4_K	16
#define	RNDTEST_CHI4_K_MASK	(RNDTEST_CHI4_K - 1)

/*
 * The unnormalized values are used so that we don't have to worry about
 * fractional precision.  The "real" value is found by:
 *	(V - 1562500) * (16 / 5000) = Vn   (where V is the unnormalized value)
 */
#define	RNDTEST_CHI4_VMIN	1563181		/* 2.1792 */
#define	RNDTEST_CHI4_VMAX	1576929		/* 46.1728 */

static int
rndtest_chi_4(struct rndtest_state *rsp)
{
	unsigned int freq[RNDTEST_CHI4_K], i, sum;

	for (i = 0; i < RNDTEST_CHI4_K; i++)
		freq[i] = 0;

	/* Get number of occurances of each 4 bit pattern */
	for (i = 0; i < RNDTEST_NBYTES; i++) {
		freq[(rsp->rs_buf[i] >> 4) & RNDTEST_CHI4_K_MASK]++;
		freq[(rsp->rs_buf[i] >> 0) & RNDTEST_CHI4_K_MASK]++;
	}

	for (i = 0, sum = 0; i < RNDTEST_CHI4_K; i++)
		sum += freq[i] * freq[i];

	if (sum >= 1563181 && sum <= 1576929) {
		rndtest_report(rsp, 0, "chi^2(4): pass (sum %u)", sum);
		return (0);
	} else {
		rndtest_report(rsp, 1, "chi^2(4): failed (sum %u)", sum);
		rndstats.rst_chi++;
		return (-1);
	}
}

int
rndtest_buf(unsigned char *buf)
{
	struct rndtest_state rsp;

	memset(&rsp, 0, sizeof(rsp));
	rsp.rs_buf = buf;
	rndtest_test(&rsp);
	return(rsp.rs_discard);
}


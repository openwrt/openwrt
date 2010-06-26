/*	$FreeBSD: src/sys/dev/rndtest/rndtest.h,v 1.1 2003/03/11 22:54:44 sam Exp $	*/
/*	$OpenBSD$	*/

/*
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


/* Some of the tests depend on these values */
#define	RNDTEST_NBYTES	2500
#define	RNDTEST_NBITS	(8 * RNDTEST_NBYTES)

struct rndtest_state {
	int		rs_discard;	/* discard/accept random data */
	u_int8_t	*rs_buf;
};

struct rndtest_stats {
	u_int32_t	rst_discard;	/* number of bytes discarded */
	u_int32_t	rst_tests;	/* number of test runs */
	u_int32_t	rst_monobit;	/* monobit test failures */
	u_int32_t	rst_runs;	/* 0/1 runs failures */
	u_int32_t	rst_longruns;	/* longruns failures */
	u_int32_t	rst_chi;	/* chi^2 failures */
};

extern int rndtest_buf(unsigned char *buf);

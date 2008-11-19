/*	$KAME: md5.c,v 1.5 2000/11/08 06:13:08 itojun Exp $	*/
/*
 * Copyright (C) 1995, 1996, 1997, and 1998 WIDE Project.
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
 * 3. Neither the name of the project nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE PROJECT AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE PROJECT OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#if 0
#include <sys/cdefs.h>
__FBSDID("$FreeBSD: src/sys/crypto/md5.c,v 1.9 2004/01/27 19:49:19 des Exp $");

#include <sys/types.h>
#include <sys/cdefs.h>
#include <sys/time.h>
#include <sys/systm.h>
#include <crypto/md5.h>
#endif

#define SHIFT(X, s) (((X) << (s)) | ((X) >> (32 - (s))))

#define F(X, Y, Z) (((X) & (Y)) | ((~X) & (Z)))
#define G(X, Y, Z) (((X) & (Z)) | ((Y) & (~Z)))
#define H(X, Y, Z) ((X) ^ (Y) ^ (Z))
#define I(X, Y, Z) ((Y) ^ ((X) | (~Z)))

#define ROUND1(a, b, c, d, k, s, i) { \
	(a) = (a) + F((b), (c), (d)) + X[(k)] + T[(i)]; \
	(a) = SHIFT((a), (s)); \
	(a) = (b) + (a); \
}

#define ROUND2(a, b, c, d, k, s, i) { \
	(a) = (a) + G((b), (c), (d)) + X[(k)] + T[(i)]; \
	(a) = SHIFT((a), (s)); \
	(a) = (b) + (a); \
}

#define ROUND3(a, b, c, d, k, s, i) { \
	(a) = (a) + H((b), (c), (d)) + X[(k)] + T[(i)]; \
	(a) = SHIFT((a), (s)); \
	(a) = (b) + (a); \
}

#define ROUND4(a, b, c, d, k, s, i) { \
	(a) = (a) + I((b), (c), (d)) + X[(k)] + T[(i)]; \
	(a) = SHIFT((a), (s)); \
	(a) = (b) + (a); \
}

#define Sa	 7
#define Sb	12
#define Sc	17
#define Sd	22

#define Se	 5
#define Sf	 9
#define Sg	14
#define Sh	20

#define Si	 4
#define Sj	11
#define Sk	16
#define Sl	23

#define Sm	 6
#define Sn	10
#define So	15
#define Sp	21

#define MD5_A0	0x67452301
#define MD5_B0	0xefcdab89
#define MD5_C0	0x98badcfe
#define MD5_D0	0x10325476

/* Integer part of 4294967296 times abs(sin(i)), where i is in radians. */
static const u_int32_t T[65] = {
	0,
	0xd76aa478, 	0xe8c7b756,	0x242070db,	0xc1bdceee,
	0xf57c0faf,	0x4787c62a, 	0xa8304613,	0xfd469501,
	0x698098d8,	0x8b44f7af,	0xffff5bb1,	0x895cd7be,
	0x6b901122, 	0xfd987193, 	0xa679438e,	0x49b40821,

	0xf61e2562,	0xc040b340, 	0x265e5a51, 	0xe9b6c7aa,
	0xd62f105d,	0x2441453,	0xd8a1e681,	0xe7d3fbc8,
	0x21e1cde6,	0xc33707d6, 	0xf4d50d87, 	0x455a14ed,
	0xa9e3e905,	0xfcefa3f8, 	0x676f02d9, 	0x8d2a4c8a,

	0xfffa3942,	0x8771f681, 	0x6d9d6122, 	0xfde5380c,
	0xa4beea44, 	0x4bdecfa9, 	0xf6bb4b60, 	0xbebfbc70,
	0x289b7ec6, 	0xeaa127fa, 	0xd4ef3085,	0x4881d05,
	0xd9d4d039, 	0xe6db99e5, 	0x1fa27cf8, 	0xc4ac5665,

	0xf4292244, 	0x432aff97, 	0xab9423a7, 	0xfc93a039,
	0x655b59c3, 	0x8f0ccc92, 	0xffeff47d, 	0x85845dd1,
	0x6fa87e4f, 	0xfe2ce6e0, 	0xa3014314, 	0x4e0811a1,
	0xf7537e82, 	0xbd3af235, 	0x2ad7d2bb, 	0xeb86d391,
};

static const u_int8_t md5_paddat[MD5_BUFLEN] = {
	0x80,	0,	0,	0,	0,	0,	0,	0,
	0,	0,	0,	0,	0,	0,	0,	0,
	0,	0,	0,	0,	0,	0,	0,	0,
	0,	0,	0,	0,	0,	0,	0,	0,
	0,	0,	0,	0,	0,	0,	0,	0,
	0,	0,	0,	0,	0,	0,	0,	0,
	0,	0,	0,	0,	0,	0,	0,	0,
	0,	0,	0,	0,	0,	0,	0,	0,	
};

static void md5_calc(u_int8_t *, md5_ctxt *);

void md5_init(ctxt)
	md5_ctxt *ctxt;
{
	ctxt->md5_n = 0;
	ctxt->md5_i = 0;
	ctxt->md5_sta = MD5_A0;
	ctxt->md5_stb = MD5_B0;
	ctxt->md5_stc = MD5_C0;
	ctxt->md5_std = MD5_D0;
	bzero(ctxt->md5_buf, sizeof(ctxt->md5_buf));
}

void md5_loop(ctxt, input, len)
	md5_ctxt *ctxt;
	u_int8_t *input;
	u_int len; /* number of bytes */
{
	u_int gap, i;

	ctxt->md5_n += len * 8; /* byte to bit */
	gap = MD5_BUFLEN - ctxt->md5_i;

	if (len >= gap) {
		bcopy((void *)input, (void *)(ctxt->md5_buf + ctxt->md5_i),
			gap);
		md5_calc(ctxt->md5_buf, ctxt);

		for (i = gap; i + MD5_BUFLEN <= len; i += MD5_BUFLEN) {
			md5_calc((u_int8_t *)(input + i), ctxt);
		}
		
		ctxt->md5_i = len - i;
		bcopy((void *)(input + i), (void *)ctxt->md5_buf, ctxt->md5_i);
	} else {
		bcopy((void *)input, (void *)(ctxt->md5_buf + ctxt->md5_i),
			len);
		ctxt->md5_i += len;
	}
}

void md5_pad(ctxt)
	md5_ctxt *ctxt;
{
	u_int gap;

	/* Don't count up padding. Keep md5_n. */	
	gap = MD5_BUFLEN - ctxt->md5_i;
	if (gap > 8) {
		bcopy(md5_paddat,
		      (void *)(ctxt->md5_buf + ctxt->md5_i),
		      gap - sizeof(ctxt->md5_n));
	} else {
		/* including gap == 8 */
		bcopy(md5_paddat, (void *)(ctxt->md5_buf + ctxt->md5_i),
			gap);
		md5_calc(ctxt->md5_buf, ctxt);
		bcopy((md5_paddat + gap),
		      (void *)ctxt->md5_buf,
		      MD5_BUFLEN - sizeof(ctxt->md5_n));
	}

	/* 8 byte word */	
#if BYTE_ORDER == LITTLE_ENDIAN
	bcopy(&ctxt->md5_n8[0], &ctxt->md5_buf[56], 8);
#endif
#if BYTE_ORDER == BIG_ENDIAN
	ctxt->md5_buf[56] = ctxt->md5_n8[7];
	ctxt->md5_buf[57] = ctxt->md5_n8[6];
	ctxt->md5_buf[58] = ctxt->md5_n8[5];
	ctxt->md5_buf[59] = ctxt->md5_n8[4];
	ctxt->md5_buf[60] = ctxt->md5_n8[3];
	ctxt->md5_buf[61] = ctxt->md5_n8[2];
	ctxt->md5_buf[62] = ctxt->md5_n8[1];
	ctxt->md5_buf[63] = ctxt->md5_n8[0];
#endif

	md5_calc(ctxt->md5_buf, ctxt);
}

void md5_result(digest, ctxt)
	u_int8_t *digest;
	md5_ctxt *ctxt;
{
	/* 4 byte words */
#if BYTE_ORDER == LITTLE_ENDIAN
	bcopy(&ctxt->md5_st8[0], digest, 16);
#endif
#if BYTE_ORDER == BIG_ENDIAN
	digest[ 0] = ctxt->md5_st8[ 3]; digest[ 1] = ctxt->md5_st8[ 2];
	digest[ 2] = ctxt->md5_st8[ 1]; digest[ 3] = ctxt->md5_st8[ 0];
	digest[ 4] = ctxt->md5_st8[ 7]; digest[ 5] = ctxt->md5_st8[ 6];
	digest[ 6] = ctxt->md5_st8[ 5]; digest[ 7] = ctxt->md5_st8[ 4];
	digest[ 8] = ctxt->md5_st8[11]; digest[ 9] = ctxt->md5_st8[10];
	digest[10] = ctxt->md5_st8[ 9]; digest[11] = ctxt->md5_st8[ 8];
	digest[12] = ctxt->md5_st8[15]; digest[13] = ctxt->md5_st8[14];
	digest[14] = ctxt->md5_st8[13]; digest[15] = ctxt->md5_st8[12];
#endif
}

static void md5_calc(b64, ctxt)
	u_int8_t *b64;
	md5_ctxt *ctxt;
{
	u_int32_t A = ctxt->md5_sta;
	u_int32_t B = ctxt->md5_stb;
	u_int32_t C = ctxt->md5_stc;
	u_int32_t D = ctxt->md5_std;
#if BYTE_ORDER == LITTLE_ENDIAN
	u_int32_t *X = (u_int32_t *)b64;
#endif	
#if BYTE_ORDER == BIG_ENDIAN
	/* 4 byte words */
	/* what a brute force but fast! */
	u_int32_t X[16];
	u_int8_t *y = (u_int8_t *)X;
	y[ 0] = b64[ 3]; y[ 1] = b64[ 2]; y[ 2] = b64[ 1]; y[ 3] = b64[ 0];
	y[ 4] = b64[ 7]; y[ 5] = b64[ 6]; y[ 6] = b64[ 5]; y[ 7] = b64[ 4];
	y[ 8] = b64[11]; y[ 9] = b64[10]; y[10] = b64[ 9]; y[11] = b64[ 8];
	y[12] = b64[15]; y[13] = b64[14]; y[14] = b64[13]; y[15] = b64[12];
	y[16] = b64[19]; y[17] = b64[18]; y[18] = b64[17]; y[19] = b64[16];
	y[20] = b64[23]; y[21] = b64[22]; y[22] = b64[21]; y[23] = b64[20];
	y[24] = b64[27]; y[25] = b64[26]; y[26] = b64[25]; y[27] = b64[24];
	y[28] = b64[31]; y[29] = b64[30]; y[30] = b64[29]; y[31] = b64[28];
	y[32] = b64[35]; y[33] = b64[34]; y[34] = b64[33]; y[35] = b64[32];
	y[36] = b64[39]; y[37] = b64[38]; y[38] = b64[37]; y[39] = b64[36];
	y[40] = b64[43]; y[41] = b64[42]; y[42] = b64[41]; y[43] = b64[40];
	y[44] = b64[47]; y[45] = b64[46]; y[46] = b64[45]; y[47] = b64[44];
	y[48] = b64[51]; y[49] = b64[50]; y[50] = b64[49]; y[51] = b64[48];
	y[52] = b64[55]; y[53] = b64[54]; y[54] = b64[53]; y[55] = b64[52];
	y[56] = b64[59]; y[57] = b64[58]; y[58] = b64[57]; y[59] = b64[56];
	y[60] = b64[63]; y[61] = b64[62]; y[62] = b64[61]; y[63] = b64[60];
#endif

	ROUND1(A, B, C, D,  0, Sa,  1); ROUND1(D, A, B, C,  1, Sb,  2);
	ROUND1(C, D, A, B,  2, Sc,  3); ROUND1(B, C, D, A,  3, Sd,  4);
	ROUND1(A, B, C, D,  4, Sa,  5); ROUND1(D, A, B, C,  5, Sb,  6);
	ROUND1(C, D, A, B,  6, Sc,  7); ROUND1(B, C, D, A,  7, Sd,  8);
	ROUND1(A, B, C, D,  8, Sa,  9); ROUND1(D, A, B, C,  9, Sb, 10);
	ROUND1(C, D, A, B, 10, Sc, 11); ROUND1(B, C, D, A, 11, Sd, 12);
	ROUND1(A, B, C, D, 12, Sa, 13); ROUND1(D, A, B, C, 13, Sb, 14);
	ROUND1(C, D, A, B, 14, Sc, 15); ROUND1(B, C, D, A, 15, Sd, 16);
	
	ROUND2(A, B, C, D,  1, Se, 17); ROUND2(D, A, B, C,  6, Sf, 18);
	ROUND2(C, D, A, B, 11, Sg, 19); ROUND2(B, C, D, A,  0, Sh, 20);
	ROUND2(A, B, C, D,  5, Se, 21); ROUND2(D, A, B, C, 10, Sf, 22);
	ROUND2(C, D, A, B, 15, Sg, 23); ROUND2(B, C, D, A,  4, Sh, 24);
	ROUND2(A, B, C, D,  9, Se, 25); ROUND2(D, A, B, C, 14, Sf, 26);
	ROUND2(C, D, A, B,  3, Sg, 27); ROUND2(B, C, D, A,  8, Sh, 28);
	ROUND2(A, B, C, D, 13, Se, 29); ROUND2(D, A, B, C,  2, Sf, 30);
	ROUND2(C, D, A, B,  7, Sg, 31); ROUND2(B, C, D, A, 12, Sh, 32);

	ROUND3(A, B, C, D,  5, Si, 33); ROUND3(D, A, B, C,  8, Sj, 34);
	ROUND3(C, D, A, B, 11, Sk, 35); ROUND3(B, C, D, A, 14, Sl, 36);
	ROUND3(A, B, C, D,  1, Si, 37); ROUND3(D, A, B, C,  4, Sj, 38);
	ROUND3(C, D, A, B,  7, Sk, 39); ROUND3(B, C, D, A, 10, Sl, 40);
	ROUND3(A, B, C, D, 13, Si, 41); ROUND3(D, A, B, C,  0, Sj, 42);
	ROUND3(C, D, A, B,  3, Sk, 43); ROUND3(B, C, D, A,  6, Sl, 44);
	ROUND3(A, B, C, D,  9, Si, 45); ROUND3(D, A, B, C, 12, Sj, 46);
	ROUND3(C, D, A, B, 15, Sk, 47); ROUND3(B, C, D, A,  2, Sl, 48);
	
	ROUND4(A, B, C, D,  0, Sm, 49); ROUND4(D, A, B, C,  7, Sn, 50);	
	ROUND4(C, D, A, B, 14, So, 51); ROUND4(B, C, D, A,  5, Sp, 52);	
	ROUND4(A, B, C, D, 12, Sm, 53); ROUND4(D, A, B, C,  3, Sn, 54);	
	ROUND4(C, D, A, B, 10, So, 55); ROUND4(B, C, D, A,  1, Sp, 56);	
	ROUND4(A, B, C, D,  8, Sm, 57); ROUND4(D, A, B, C, 15, Sn, 58);	
	ROUND4(C, D, A, B,  6, So, 59); ROUND4(B, C, D, A, 13, Sp, 60);	
	ROUND4(A, B, C, D,  4, Sm, 61); ROUND4(D, A, B, C, 11, Sn, 62);	
	ROUND4(C, D, A, B,  2, So, 63); ROUND4(B, C, D, A,  9, Sp, 64);

	ctxt->md5_sta += A;
	ctxt->md5_stb += B;
	ctxt->md5_stc += C;
	ctxt->md5_std += D;
}

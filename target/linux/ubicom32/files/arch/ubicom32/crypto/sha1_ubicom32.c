/*
 * arch/ubicom32/crypto/sha1_ubicom32.c
 *   Ubicom32 implementation of the SHA1 Secure Hash Algorithm.
 *
 * (C) Copyright 2009, Ubicom, Inc.
 *
 * This file is part of the Ubicom32 Linux Kernel Port.
 *
 * The Ubicom32 Linux Kernel Port is free software: you can redistribute
 * it and/or modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation, either version 2 of the
 * License, or (at your option) any later version.
 *
 * The Ubicom32 Linux Kernel Port is distributed in the hope that it
 * will be useful, but WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See
 * the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with the Ubicom32 Linux Kernel Port.  If not,
 * see <http://www.gnu.org/licenses/>.
 *
 * Ubicom32 implementation derived from (with many thanks):
 *   arch/m68knommu
 *   arch/blackfin
 *   arch/parisc
 */
#include <linux/init.h>
#include <linux/module.h>
#include <linux/crypto.h>
#include <crypto/sha.h>
#include <asm/linkage.h>

#include "crypto_ubicom32.h"
#define HASH_SECURITY_BLOCK_CONTROL_INIT_NO_ENCYPTION 2
#define HASH_SECURITY_BLOCK_CONTROL_INIT_SHA1 ((1 << 5) | HASH_SECURITY_BLOCK_CONTROL_INIT_NO_ENCYPTION)

struct ubicom32_sha1_ctx {
	u64 count;		/* message length */
	u32 state[5];
	u8 buf[2 * SHA1_BLOCK_SIZE];
};

static inline void sha1_clear_2ws(u8 *buf, int wc)
{
	asm volatile (
	"1:	move.4	(%0)4++, #0		\n\t"
	"	move.4	(%0)4++, #0		\n\t"
	"	sub.4	%1, #2, %1		\n\t"
	"	jmple.f	1b			\n\t"
		:
		: "a" (buf), "d" (wc)
		: "cc"
	);
}

/* only wipe out count, state, and 1st half of buf - 9 bytes at most */
#define sha1_wipe_out(sctx) sha1_clear_2ws((u8 *)sctx, 2 + 5 + 16 - 2)

static inline void sha1_init_digest(u32 *digest)
{
	hw_crypto_set_ctrl(HASH_SECURITY_BLOCK_CONTROL_INIT_SHA1);
	asm volatile (
	"	; move digests to hash_output regs	\n\t"
	"	move.4	0x70(%0), 0x0(%1)		\n\t"
	"	move.4	0x74(%0), 0x4(%1)		\n\t"
	"	move.4	0x78(%0), 0x8(%1)		\n\t"
	"	move.4	0x7c(%0), 0xc(%1)		\n\t"
	"	move.4	0x80(%0), 0x10(%1)		\n\t"
		:
		: "a" (SEC_BASE), "a" (digest)
	);
}

static inline void sha1_transform_feed(const u8 *in)
{
	asm volatile (
	"	; write the 1st 16 bytes	\n\t"
	"	move.4	0x30(%0), 0x0(%1)	\n\t"
	"	move.4	0x34(%0), 0x4(%1)	\n\t"
	"	move.4	0x38(%0), 0x8(%1)	\n\t"
	"	move.4	0x3c(%0), 0xc(%1)	\n\t"
	"	move.4	0x40(%0), %1		\n\t"
	"	; write the 2nd 16 bytes	\n\t"
	"	move.4	0x30(%0), 0x10(%1)	\n\t"
	"	move.4	0x34(%0), 0x14(%1)	\n\t"
	"	move.4	0x38(%0), 0x18(%1)	\n\t"
	"	move.4	0x3c(%0), 0x1c(%1)	\n\t"
	"	move.4	0x40(%0), %1		\n\t"
	"	; write the 3rd 16 bytes	\n\t"
	"	move.4	0x30(%0), 0x20(%1)	\n\t"
	"	move.4	0x34(%0), 0x24(%1)	\n\t"
	"	move.4	0x38(%0), 0x28(%1)	\n\t"
	"	move.4	0x3c(%0), 0x2c(%1)	\n\t"
	"	move.4	0x40(%0), %1		\n\t"
	"	; write the 4th 16 bytes	\n\t"
	"	move.4	0x30(%0), 0x30(%1)	\n\t"
	"	move.4	0x34(%0), 0x34(%1)	\n\t"
	"	move.4	0x38(%0), 0x38(%1)	\n\t"
	"	move.4	0x3c(%0), 0x3c(%1)	\n\t"
	"	move.4	0x40(%0), %1		\n\t"
	"	pipe_flush 0			\n\t"
		:
		: "a"(SEC_BASE), "a"(in)
	);
}

static inline void sha1_transform_wait(void)
{
	asm volatile (
	"	btst	0x04(%0), #0		\n\t"
	"	jmpne.f -4			\n\t"
		:
		: "a"(SEC_BASE)
		: "cc"
	);
}

static inline void sha1_output_digest(u32 *digest)
{
	asm volatile (
	"	move.4	0x0(%1), 0x70(%0)		\n\t"
	"	move.4	0x4(%1), 0x74(%0)		\n\t"
	"	move.4	0x8(%1), 0x78(%0)		\n\t"
	"	move.4	0xc(%1), 0x7c(%0)		\n\t"
	"	move.4	0x10(%1), 0x80(%0)		\n\t"
		:
		: "a" (SEC_BASE), "a" (digest)
	);
}

static __ocm_text void sha1_init(struct crypto_tfm *tfm)
{
	struct ubicom32_sha1_ctx *sctx = crypto_tfm_ctx(tfm);

	sctx->state[0] = SHA1_H0;
	sctx->state[1] = SHA1_H1;
	sctx->state[2] = SHA1_H2;
	sctx->state[3] = SHA1_H3;
	sctx->state[4] = SHA1_H4;
	sctx->count = 0;
}

static void __ocm_text sha1_update(struct crypto_tfm *tfm, const u8 *data,
			unsigned int len)
{
	struct ubicom32_sha1_ctx *sctx = crypto_tfm_ctx(tfm);
	int index, clen;

	/* how much is already in the buffer? */
	index = sctx->count & 0x3f;

	sctx->count += len;

	if (index + len < SHA1_BLOCK_SIZE) {
		goto store_only;
	}

	hw_crypto_lock();
	hw_crypto_check();

	/* init digest set ctrl register too */
	sha1_init_digest(sctx->state);

	if (unlikely(index == 0 && SEC_ALIGNED(data))) {
fast_process:
#if CRYPTO_UBICOM32_LOOP_ASM
		if (likely(len >= SHA1_BLOCK_SIZE)) {
			register unsigned int cnt = len >> 6; 	// loop = len / 64;
			sha1_transform_feed(data);
			data += SHA1_BLOCK_SIZE;

			/* cnt is pre-decremented in the loop */
			asm volatile (
			"; while (--loop):  work on 2nd block   \n\t"
			"1:	add.4	%2, #-1, %2		\n\t"
			"	jmpeq.f	5f			\n\t"
			"					\n\t"
			"	; write the 1st 16 bytes	\n\t"
			"	move.4	0x30(%1), (%0)4++	\n\t"
			"	move.4	0x34(%1), (%0)4++	\n\t"
			"	move.4	0x38(%1), (%0)4++	\n\t"
			"	move.4	0x3c(%1), (%0)4++	\n\t"
			"	; can not kick off hw before it \n\t"
			"	; is done with the prev block   \n\t"
			"					\n\t"
			"	btst	0x04(%1), #0		\n\t"
			"	jmpne.f	-4			\n\t"
			"					\n\t"
			"	; tell hw to load 1st 16 bytes  \n\t"
			"	move.4	0x40(%1), %2		\n\t"
			"					\n\t"
			"	; write the 2nd 16 bytes	\n\t"
			"	move.4	0x30(%1), (%0)4++	\n\t"
			"	move.4	0x34(%1), (%0)4++	\n\t"
			"	move.4	0x38(%1), (%0)4++	\n\t"
			"	move.4	0x3c(%1), (%0)4++	\n\t"
			"	move.4	0x40(%1), %2		\n\t"
			"					\n\t"
			"	; write the 3rd 16 bytes	\n\t"
			"	move.4	0x30(%1), (%0)4++	\n\t"
			"	move.4	0x34(%1), (%0)4++	\n\t"
			"	move.4	0x38(%1), (%0)4++	\n\t"
			"	move.4	0x3c(%1), (%0)4++	\n\t"
			"	move.4	0x40(%1), %2		\n\t"
			"					\n\t"
			"	; write the 4th 16 bytes	\n\t"
			"	move.4	0x30(%1), (%0)4++	\n\t"
			"	move.4	0x34(%1), (%0)4++	\n\t"
			"	move.4	0x38(%1), (%0)4++	\n\t"
			"	move.4	0x3c(%1), (%0)4++	\n\t"
			"	move.4	0x40(%1), %2		\n\t"
			"					\n\t"
			"; no need flush, enough insts		\n\t"
			"; before next hw wait			\n\t"
			"					\n\t"
			"; go back to loop			\n\t"
			"	jmpt 1b				\n\t"
			"					\n\t"
			"; wait hw for last block		\n\t"
			"5:	btst 0x04(%1), #0		\n\t"
			"	jmpne.f -4			\n\t"
			"					\n\t"
				: "+a" (data)
				: "a"( SEC_BASE), "d" (cnt)
				: "cc"
			);

			len = len & (64 - 1);
		}
#else
		while (likely(len >= SHA1_BLOCK_SIZE)) {
			sha1_transform_feed(data);
			data += SHA1_BLOCK_SIZE;
			len -= SHA1_BLOCK_SIZE;
			sha1_transform_wait();
		}
#endif
		goto store;
	}

	/* process one stored block */
	if (index) {
		clen = SHA1_BLOCK_SIZE - index;
		memcpy(sctx->buf + index, data, clen);
		sha1_transform_feed(sctx->buf);
		data += clen;
		len -= clen;
		index = 0;
		sha1_transform_wait();
	}

	if (likely(SEC_ALIGNED(data))) {
		goto fast_process;
	}

	/* process as many blocks as possible */
	if (likely(len >= SHA1_BLOCK_SIZE)) {
		memcpy(sctx->buf, data, SHA1_BLOCK_SIZE);
		do {
			sha1_transform_feed(sctx->buf);
			data += SHA1_BLOCK_SIZE;
			len -= SHA1_BLOCK_SIZE;
			if (likely(len >= SHA1_BLOCK_SIZE)) {
				memcpy(sctx->buf, data, SHA1_BLOCK_SIZE);
				sha1_transform_wait();
				continue;
			}
			/* it is the last block */
			sha1_transform_wait();
			break;
		} while (1);
	}

store:
	sha1_output_digest(sctx->state);
	hw_crypto_unlock();

store_only:
	/* anything left? */
	if (len)
		memcpy(sctx->buf + index , data, len);
}

/* Add padding and return the message digest. */
static void __ocm_text sha1_final(struct crypto_tfm *tfm, u8 *out)
{
	struct ubicom32_sha1_ctx *sctx = crypto_tfm_ctx(tfm);
	u64 bits;
	unsigned int index, end;

	/* must perform manual padding */
	index = sctx->count & 0x3f;
	end =  (index < 56) ? SHA1_BLOCK_SIZE : (2 * SHA1_BLOCK_SIZE);

	/* start pad with 1 */
	sctx->buf[index] = 0x80;

	/* pad with zeros */
	index++;
	memset(sctx->buf + index, 0x00, end - index - 8);

	/* append message length */
	bits = sctx->count << 3 ;
	SEC_COPY_2W(sctx->buf + end - 8, &bits);

	/* force to use the sctx->buf and ignore the partial buf */
	sctx->count = sctx->count & ~0x3f;
	sha1_update(tfm, sctx->buf, end);

	/* copy digest to out */
	SEC_COPY_5W(out, sctx->state);

	/* wipe context */
	sha1_wipe_out(sctx);
}

static struct crypto_alg alg = {
	.cra_name	=	"sha1",
	.cra_driver_name=	"sha1-ubicom32",
	.cra_priority	=	CRYPTO_UBICOM32_PRIORITY,
	.cra_flags	=	CRYPTO_ALG_TYPE_DIGEST,
	.cra_blocksize	=	SHA1_BLOCK_SIZE,
	.cra_ctxsize	=	sizeof(struct ubicom32_sha1_ctx),
	.cra_module	=	THIS_MODULE,
	.cra_list	=	LIST_HEAD_INIT(alg.cra_list),
	.cra_u	  = {
		.digest = {
			.dia_digestsize =       SHA1_DIGEST_SIZE,
			.dia_init       =       sha1_init,
			.dia_update     =       sha1_update,
			.dia_final      =       sha1_final,
		}
	}
};

static int __init init(void)
{
	hw_crypto_init();
	return crypto_register_alg(&alg);
}

static void __exit fini(void)
{
	crypto_unregister_alg(&alg);
}

module_init(init);
module_exit(fini);

MODULE_ALIAS("sha1");

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("SHA1 Secure Hash Algorithm");

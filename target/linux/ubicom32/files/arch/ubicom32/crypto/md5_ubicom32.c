/*
 * arch/ubicom32/crypto/md5_ubicom32.c
 *   Ubicom32 implementation of the MD5 Secure Hash Algorithm
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

#include "crypto_ubicom32.h"

#define MD5_DIGEST_SIZE	16
#define MD5_BLOCK_SIZE	64
#define MD5_HASH_WORDS	4

extern void _md5_ip5k_init_digest(u32_t *digest);
extern void _md5_ip5k_transform(u32_t *data_input);
extern void _md5_ip5k_get_digest(u32_t *digest);

struct ubicom32_md5_ctx {
	u64 count;		/* message length */
	u32 state[MD5_HASH_WORDS];
	u8 buf[2 * MD5_BLOCK_SIZE];
};

static void md5_init(struct crypto_tfm *tfm)
{
	struct ubicom32_md5_ctx *mctx = crypto_tfm_ctx(tfm);
	mctx->state[0] = 0x01234567;
	mctx->state[1] = 0x89abcdef;
	mctx->state[2] = 0xfedcba98;
	mctx->state[3] = 0x76543210;

	mctx->count = 0;
}

static inline void _md5_process(u32 *digest, const u8 *data)
{
	_md5_ip5k_transform((u32 *)data);
}

static void md5_update(struct crypto_tfm *tfm, const u8 *data,
			unsigned int len)
{
	struct ubicom32_md5_ctx *mctx = crypto_tfm_ctx(tfm);
	int index, clen;

	/* how much is already in the buffer? */
	index = mctx->count & 0x3f;

	mctx->count += len;

	if (index + len < MD5_BLOCK_SIZE) {
		goto store_only;
	}

	hw_crypto_lock();
	hw_crypto_check();

	/* init digest set ctrl register too */
	_md5_ip5k_init_digest(mctx->state);

	if (unlikely(index == 0 && SEC_ALIGNED(data))) {
fast_process:
		while (len >= MD5_BLOCK_SIZE) {
			_md5_process(mctx->state, data);
			data += MD5_BLOCK_SIZE;
			len -= MD5_BLOCK_SIZE;
		}
		goto store;
	}

	/* process one stored block */
	if (index) {
		clen = MD5_BLOCK_SIZE - index;
		memcpy(mctx->buf + index, data, clen);
		_md5_process(mctx->state, mctx->buf);
		data += clen;
		len -= clen;
		index = 0;
	}

	if (likely(SEC_ALIGNED(data))) {
		goto fast_process;
	}

	/* process as many blocks as possible */
	while (len >= MD5_BLOCK_SIZE) {
		memcpy(mctx->buf, data, MD5_BLOCK_SIZE);
		_md5_process(mctx->state, mctx->buf);
		data += MD5_BLOCK_SIZE;
		len -= MD5_BLOCK_SIZE;
	}

store:
	_md5_ip5k_get_digest(mctx->state);
	hw_crypto_unlock();

store_only:
	/* anything left? */
	if (len)
		memcpy(mctx->buf + index , data, len);
}

/* Add padding and return the message digest. */
static void md5_final(struct crypto_tfm *tfm, u8 *out)
{
	struct ubicom32_md5_ctx *mctx = crypto_tfm_ctx(tfm);
	u32 bits[2];
	unsigned int index, end;

	/* must perform manual padding */
	index = mctx->count & 0x3f;
	end =  (index < 56) ? MD5_BLOCK_SIZE : (2 * MD5_BLOCK_SIZE);

	/* start pad with 1 */
	mctx->buf[index] = 0x80;

	/* pad with zeros */
	index++;
	memset(mctx->buf + index, 0x00, end - index - 8);

	/* append message length */
	bits[0] = mctx->count << 3;
	bits[1] = mctx->count >> 29;
	__cpu_to_le32s(bits);
	__cpu_to_le32s(bits + 1);

	memcpy(mctx->buf + end - 8, &bits, sizeof(bits));

	/* force to use the mctx->buf and ignore the partial buf */
	mctx->count = mctx->count & ~0x3f;
	md5_update(tfm, mctx->buf, end);

	/* copy digest to out */
	memcpy(out, mctx->state, MD5_DIGEST_SIZE);

	/* wipe context */
	memset(mctx, 0, sizeof *mctx);
}

static struct crypto_alg alg = {
	.cra_name	=	"md5",
	.cra_driver_name=	"md5-ubicom32",
	.cra_priority	=	CRYPTO_UBICOM32_PRIORITY,
	.cra_flags	=	CRYPTO_ALG_TYPE_DIGEST,
	.cra_blocksize	=	MD5_BLOCK_SIZE,
	.cra_ctxsize	=	sizeof(struct ubicom32_md5_ctx),
	.cra_module	=	THIS_MODULE,
	.cra_list	=	LIST_HEAD_INIT(alg.cra_list),
	.cra_u = {
		.digest = {
			.dia_digestsize =       MD5_DIGEST_SIZE,
			.dia_init       =       md5_init,
			.dia_update     =       md5_update,
			.dia_final      =       md5_final,
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

MODULE_ALIAS("md5");

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("MD5 Secure Hash Algorithm");

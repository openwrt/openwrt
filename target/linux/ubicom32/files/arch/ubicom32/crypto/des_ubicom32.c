/*
 * arch/ubicom32/crypto/des_ubicom32.c
 *   Ubicom32 implementation of the DES Cipher Algorithm.
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
#include <crypto/algapi.h>
#include <linux/init.h>
#include <linux/module.h>

#include "crypto_ubicom32.h"
extern int crypto_des_check_key(const u8 *key, unsigned int keylen, u32 *flags);

#define DES_BLOCK_SIZE 8
#define DES_KEY_SIZE 8

#define DES3_192_KEY_SIZE	(3 * DES_KEY_SIZE)
#define DES3_192_BLOCK_SIZE	DES_BLOCK_SIZE

#define DES3_SUB_KEY(key, i) 	(((u8 *)key) + (i * DES_KEY_SIZE))

enum des_ops {
	DES_ENCRYPT,
	DES_DECRYPT,

	DES3_EDE_ENCRYPT,
	DES3_EDE_DECRYPT,

#ifdef DES3_EEE
	DES3_EEE_ENCRYPT,
	DES3_EEE_DECRYPT,
#endif
};

struct ubicom32_des_ctx {
	u8 key[3 * DES_KEY_SIZE];
	u32 ctrl;
	int key_len;
};

static inline void des_hw_set_key(const u8 *key, u8 key_len)
{
	/*
	 * HW 3DES is not tested yet, use DES just as ipOS
	 */
	DES_SET_KEY(key);
}

static inline void des_hw_cipher(u8 *out, const u8 *in)
{
	SEC_SET_INPUT_2W(in);

	asm volatile (
	"	; start DES by writing 0x38(SECURITY_BASE)	\n\t"
	"	move.4 0x38(%0), #0x01				\n\t"
	"	pipe_flush 0					\n\t"
	"							\n\t"
	"	; wait for the module to calculate the output	\n\t"
	"	btst 0x04(%0), #0				\n\t"
	"	jmpne.f .-4					\n\t"
		:
		: "a" (SEC_BASE)
		: "cc"
	);

	SEC_GET_OUTPUT_2W(out);
}


static void inline des3_hw_ede_encrypt(u8 *keys, u8 *out, const u8 *in)
{
	hw_crypto_set_ctrl(SEC_ALG_DES | SEC_DIR_ENCRYPT);
	des_hw_set_key(DES3_SUB_KEY(keys, 0), DES_KEY_SIZE);
	des_hw_cipher(out, in);

	hw_crypto_set_ctrl(SEC_ALG_DES | SEC_DIR_DECRYPT);
	des_hw_set_key(DES3_SUB_KEY(keys, 1), DES_KEY_SIZE);
	des_hw_cipher(out, out);

	hw_crypto_set_ctrl(SEC_ALG_DES | SEC_DIR_ENCRYPT);
	des_hw_set_key(DES3_SUB_KEY(keys, 2), DES_KEY_SIZE);
	des_hw_cipher(out, out);
}

static void inline des3_hw_ede_decrypt(u8 *keys, u8 *out, const u8 *in)
{
	hw_crypto_set_ctrl(SEC_ALG_DES | SEC_DIR_DECRYPT);
	des_hw_set_key(DES3_SUB_KEY(keys, 2), DES_KEY_SIZE);
	des_hw_cipher(out, in);

	hw_crypto_set_ctrl(SEC_ALG_DES | SEC_DIR_ENCRYPT);
	des_hw_set_key(DES3_SUB_KEY(keys, 1), DES_KEY_SIZE);
	des_hw_cipher(out, out);

	hw_crypto_set_ctrl(SEC_ALG_DES | SEC_DIR_DECRYPT);
	des_hw_set_key(DES3_SUB_KEY(keys, 0), DES_KEY_SIZE);
	des_hw_cipher(out, out);
}

#ifdef DES3_EEE
static void inline des3_hw_eee_encrypt(u8 *keys, u8 *out, const u8 *in)
{
	hw_crypto_set_ctrl(SEC_ALG_DES | SEC_DIR_ENCRYPT);
	des_hw_set_key(DES3_SUB_KEY(keys, 0), 2);
	des_hw_cipher(out, in);

	hw_crypto_set_ctrl(SEC_ALG_DES | SEC_DIR_ENCRYPT);
	des_hw_set_key(DES3_SUB_KEY(keys, 1), 2);
	des_hw_cipher(out, out);

	hw_crypto_set_ctrl(SEC_ALG_DES | SEC_DIR_ENCRYPT);
	des_hw_set_key(DES3_SUB_KEY(keys, 2), 2);
	des_hw_cipher(out, out);
}

static void inline des3_hw_eee_decrypt(u8 *keys, u8 *out, const u8 *in)
{
	hw_crypto_set_ctrl(SEC_ALG_DES | SEC_DIR_DECRYPT);
	des_hw_set_key(DES3_SUB_KEY(keys, 2), 2);
	des_hw_cipher(out, in);

	hw_crypto_set_ctrl(SEC_ALG_DES | SEC_DIR_DECRYPT);
	des_hw_set_key(DES3_SUB_KEY(keys, 1), 2);
	des_hw_cipher(out, out);

	hw_crypto_set_ctrl(SEC_ALG_DES | SEC_DIR_DECRYPT);
	des_hw_set_key(DES3_SUB_KEY(keys, 0), 2);
	des_hw_cipher(out, out);
}
#endif

static int des_setkey(struct crypto_tfm *tfm, const u8 *key,
		      unsigned int keylen)
{
	struct ubicom32_des_ctx *dctx = crypto_tfm_ctx(tfm);
	u32 *flags = &tfm->crt_flags;
	int ret;

	/* test if key is valid (not a weak key) */
	ret = crypto_des_check_key(key, keylen, flags);
	if (ret == 0) {
		memcpy(dctx->key, key, keylen);
		dctx->key_len = keylen;
		//dctx->ctrl = (keylen == DES_KEY_SIZE) ? SEC_ALG_DES : SEC_ALG_3DES
		/* 2DES and 3DES are both implemented with DES hw function */
		dctx->ctrl = SEC_ALG_DES;
	}
	return ret;
}

static inline void des_cipher_1b(struct crypto_tfm *tfm, u8 *out, const u8 *in, u32 extra_flags)
{
	const struct ubicom32_des_ctx *uctx = crypto_tfm_ctx(tfm);

	hw_crypto_lock();
	hw_crypto_check();
	hw_crypto_set_ctrl(uctx->ctrl | extra_flags);

	des_hw_set_key(uctx->key, uctx->key_len);
	des_hw_cipher(out, in);

	hw_crypto_unlock();
}

static void des_encrypt(struct crypto_tfm *tfm, u8 *out, const u8 *in)
{
	des_cipher_1b(tfm, out, in, SEC_DIR_ENCRYPT);
}

static void des_decrypt(struct crypto_tfm *tfm, u8 *out, const u8 *in)
{
	des_cipher_1b(tfm, out, in, SEC_DIR_DECRYPT);
}

static struct crypto_alg des_alg = {
	.cra_name		=	"des",
	.cra_driver_name	=	"des-ubicom32",
	.cra_priority		=	CRYPTO_UBICOM32_PRIORITY,
	.cra_flags		=	CRYPTO_ALG_TYPE_CIPHER,
	.cra_blocksize		=	DES_BLOCK_SIZE,
	.cra_ctxsize		=	sizeof(struct ubicom32_des_ctx),
	.cra_alignmask		=	CRYPTO_UBICOM32_ALIGNMENT - 1,
	.cra_alignmask		=	CRYPTO_UBICOM32_ALIGNMENT - 1,
	.cra_module		=	THIS_MODULE,
	.cra_list		=	LIST_HEAD_INIT(des_alg.cra_list),
	.cra_u			= {
		.cipher = {
			.cia_min_keysize	=	DES_KEY_SIZE,
			.cia_max_keysize	=	DES_KEY_SIZE,
			.cia_setkey		=	des_setkey,
			.cia_encrypt		=	des_encrypt,
			.cia_decrypt		=	des_decrypt,
		}
	}
};

static void ecb_des_ciper_loop(u8 *out, u8 *in, unsigned int n)
{
	while (likely(n)) {
		des_hw_cipher(out, in);
		out += DES_BLOCK_SIZE;
		in += DES_BLOCK_SIZE;
		n -= DES_BLOCK_SIZE;
	}
}

static void ecb_des3_ede_encrypt_loop(u8 *keys, u8 *out, u8 *in, unsigned int n)
{
	while (likely(n)) {
		des3_hw_ede_encrypt(keys, out, in);

		out += DES_BLOCK_SIZE;
		in += DES_BLOCK_SIZE;
		n -= DES_BLOCK_SIZE;
	}
}

static void ecb_des3_ede_decrypt_loop(u8 *keys, u8 *out, u8 *in, unsigned int n)
{
	while (likely(n)) {
		des3_hw_ede_decrypt(keys, out, in);

		out += DES_BLOCK_SIZE;
		in += DES_BLOCK_SIZE;
		n -= DES_BLOCK_SIZE;
	}
}

#ifdef DES3_EEE
static void ecb_des3_eee_encrypt_loop(u8 *keys, u8 *out, u8 *in, unsigned int n)
{
	while (likely(n)) {
		des3_hw_eee_encrypt(keys, out, in);

		out += DES_BLOCK_SIZE;
		in += DES_BLOCK_SIZE;
		n -= DES_BLOCK_SIZE;
	}
}

static void ecb_des3_eee_decrypt_loop(u8 *keys, u8 *out, u8 *in, unsigned int n)
{
	while (likely(n)) {
		des3_hw_eee_decrypt(keys, out, in);

		out += DES_BLOCK_SIZE;
		in += DES_BLOCK_SIZE;
		n -= DES_BLOCK_SIZE;
	}
}
#endif

static inline void ecb_des_cipher_n(struct ubicom32_des_ctx *uctx, enum des_ops op, u8 *out, u8 *in, unsigned int n)
{
	switch (op) {
	case DES_ENCRYPT:
	case DES_DECRYPT:
		/* set the right algo, direction and key once */
		hw_crypto_set_ctrl(SEC_ALG_DES | (op == DES_ENCRYPT ? SEC_DIR_ENCRYPT : 0));
		des_hw_set_key(uctx->key, uctx->key_len);
		ecb_des_ciper_loop(out, in, n);
		break;

	case DES3_EDE_ENCRYPT:
		ecb_des3_ede_encrypt_loop(uctx->key, out, in, n);
		break;

	case DES3_EDE_DECRYPT:
		ecb_des3_ede_decrypt_loop(uctx->key, out, in, n);
		break;

#ifdef DES3_EEE
	case DES3_EEE_ENCRYPT:
		ecb_des3_eee_encrypt_loop(uctx->key, out, in, n);
		break;

	case DES3_EEE_DECRYPT:
		ecb_des3_eee_decrypt_loop(uctx->key, out, in, n);
		break;
#endif
	}
}

static inline void des_xor_2w(u32 *data, u32 *iv)
{
	data[0] ^= iv[0];
	data[1] ^= iv[1];
}

static void cbc_des_encrypt_loop(u8 *out, u8 *in, u8 *iv, unsigned int n)
{
	while (likely(n)) {
		des_xor_2w((u32 *)in, (u32 *)iv);
		des_hw_cipher(out, in);
		SEC_COPY_2W(iv, out);
		out += DES_BLOCK_SIZE;
		in += DES_BLOCK_SIZE;
		n -= DES_BLOCK_SIZE;
	}
}

static void cbc_des_decrypt_loop(u8 *out, u8 *in, u8 *iv, unsigned int n)
{
	u8 next_iv[DES_BLOCK_SIZE];
	while (likely(n)) {
		SEC_COPY_2W(next_iv, in);
		des_hw_cipher(out, in);
		des_xor_2w((u32 *)out, (u32 *)iv);
		SEC_COPY_2W(iv, next_iv);

		out += DES_BLOCK_SIZE;
		in += DES_BLOCK_SIZE;
		n -= DES_BLOCK_SIZE;
	}
}

static void cbc_des3_ede_encrypt_loop(u8 *keys, u8 *out, u8 *in, u8 *iv, unsigned int n)
{
	while (likely(n)) {
		des_xor_2w((u32 *)in, (u32 *)iv);
		des3_hw_ede_encrypt(keys, out, in);
		SEC_COPY_2W(iv, out);

		out += DES_BLOCK_SIZE;
		in += DES_BLOCK_SIZE;
		n -= DES_BLOCK_SIZE;
	}
}

static void cbc_des3_ede_decrypt_loop(u8 *keys, u8 *out, u8 *in, u8 *iv, unsigned int n)
{
	u8 next_iv[DES_BLOCK_SIZE];
	while (likely(n)) {
		SEC_COPY_2W(next_iv, in);
		des3_hw_ede_decrypt(keys, out, in);
		des_xor_2w((u32 *)out, (u32 *)iv);
		SEC_COPY_2W(iv, next_iv);

		out += DES_BLOCK_SIZE;
		in += DES_BLOCK_SIZE;
		n -= DES_BLOCK_SIZE;
	}
}

#ifdef DES3_EEE
static void cbc_des3_eee_encrypt_loop(u8 *keys, u8 *out, u8 *in, u8 *iv, unsigned int n)
{
	while (likely(n)) {
		des_xor_2w((u32 *)in, (u32 *)iv);
		des3_hw_eee_encrypt(keys, out, in);
		SEC_COPY_2W(iv, out);

		out += DES_BLOCK_SIZE;
		in += DES_BLOCK_SIZE;
		n -= DES_BLOCK_SIZE;
	}
}

static void cbc_des3_eee_decrypt_loop(u8 *keys, u8 *out, u8 *in, u8 *iv, unsigned int n)
{
	u8 next_iv[DES_BLOCK_SIZE];
	while (likely(n)) {
		SEC_COPY_2W(next_iv, in);
		des3_hw_eee_decrypt(keys, out, in);
		des_xor_2w((u32 *)out, (u32 *)iv);
		SEC_COPY_2W(iv, next_iv);

		out += DES_BLOCK_SIZE;
		in += DES_BLOCK_SIZE;
		n -= DES_BLOCK_SIZE;
	}
}
#endif

static inline void cbc_des_cipher_n(struct ubicom32_des_ctx *uctx, enum des_ops op, u8 *out, u8 *in, u8 *iv, unsigned int n)
{
	switch (op) {
	case DES_ENCRYPT:
		hw_crypto_set_ctrl(SEC_ALG_DES | SEC_DIR_ENCRYPT);
		des_hw_set_key(uctx->key, uctx->key_len);
		cbc_des_encrypt_loop(out, in, iv, n);
		break;

	case DES_DECRYPT:
		/* set the right algo, direction and key once */
		hw_crypto_set_ctrl(SEC_ALG_DES | SEC_DIR_DECRYPT);
		des_hw_set_key(uctx->key, uctx->key_len);
		cbc_des_decrypt_loop(out, in, iv, n);
		break;

	case DES3_EDE_ENCRYPT:
		cbc_des3_ede_encrypt_loop(uctx->key, out, in, iv, n);
		break;

	case DES3_EDE_DECRYPT:
		cbc_des3_ede_decrypt_loop(uctx->key, out, in, iv, n);
		break;

#ifdef DES3_EEE
	case DES3_EEE_ENCRYPT:
		cbc_des3_eee_encrypt_loop(uctx->key, out, in, iv, n);
		break;

	case DES3_EEE_DECRYPT:
		cbc_des3_eee_decrypt_loop(uctx->key, out, in, iv, n);
		break;
#endif
	}
}

static int des_cipher(struct blkcipher_desc *desc, struct scatterlist *dst,
		      struct scatterlist *src, unsigned int nbytes, u32 extra_flags, enum des_ops op)
{
	struct ubicom32_des_ctx *uctx = crypto_blkcipher_ctx(desc->tfm);
	int ret;

	struct blkcipher_walk walk;
	blkcipher_walk_init(&walk, dst, src, nbytes);
	ret = blkcipher_walk_virt(desc, &walk);
	if (ret) {
		return ret;
	}

	hw_crypto_lock();
	hw_crypto_check();

	while ((nbytes = walk.nbytes)) {
		/* only use complete blocks */
		unsigned int n = nbytes & ~(DES_BLOCK_SIZE - 1);
		u8 *out = walk.dst.virt.addr;
		u8 *in = walk.src.virt.addr;

		/* finish n/16 blocks */
		if (extra_flags & SEC_CBC_SET) {
			cbc_des_cipher_n(uctx, op, out, in, walk.iv, n);
		} else {
			ecb_des_cipher_n(uctx, op, out, in, n);
		}

		nbytes &= DES_BLOCK_SIZE - 1;
		ret = blkcipher_walk_done(desc, &walk, nbytes);
	}

	hw_crypto_unlock();
	return ret;
}

static int ecb_des_encrypt(struct blkcipher_desc *desc,
			   struct scatterlist *dst, struct scatterlist *src,
			   unsigned int nbytes)
{
	return des_cipher(desc, dst, src, nbytes, SEC_CBC_NONE, DES_ENCRYPT);
}

static int ecb_des_decrypt(struct blkcipher_desc *desc,
			   struct scatterlist *dst, struct scatterlist *src,
			   unsigned int nbytes)
{
	return des_cipher(desc, dst, src, nbytes, SEC_CBC_NONE, DES_DECRYPT);
}

static struct crypto_alg ecb_des_alg = {
	.cra_name		=	"ecb(des)",
	.cra_driver_name	=	"ecb-des-ubicom32",
	.cra_priority		=	CRYPTO_UBICOM32_COMPOSITE_PRIORITY,
	.cra_flags		=	CRYPTO_ALG_TYPE_BLKCIPHER,
	.cra_blocksize		=	DES_BLOCK_SIZE,
	.cra_ctxsize		=	sizeof(struct ubicom32_des_ctx),
	.cra_alignmask		=	CRYPTO_UBICOM32_ALIGNMENT - 1,
	.cra_type		=	&crypto_blkcipher_type,
	.cra_module		=	THIS_MODULE,
	.cra_list		=	LIST_HEAD_INIT(ecb_des_alg.cra_list),
	.cra_u			= {
		.blkcipher = {
			.min_keysize		=	DES_KEY_SIZE,
			.max_keysize		=	DES_KEY_SIZE,
			.setkey			=	des_setkey,
			.encrypt		=	ecb_des_encrypt,
			.decrypt		=	ecb_des_decrypt,
		}
	}
};

static int cbc_des_encrypt(struct blkcipher_desc *desc,
			   struct scatterlist *dst, struct scatterlist *src,
			   unsigned int nbytes)
{
	return des_cipher(desc, dst, src, nbytes, SEC_CBC_SET, DES_ENCRYPT);
}

static int cbc_des_decrypt(struct blkcipher_desc *desc,
			   struct scatterlist *dst, struct scatterlist *src,
			   unsigned int nbytes)
{
	return des_cipher(desc, dst, src, nbytes, SEC_CBC_SET, DES_DECRYPT);
}

static struct crypto_alg cbc_des_alg = {
	.cra_name		=	"cbc(des)",
	.cra_driver_name	=	"cbc-des-ubicom32",
	.cra_priority		=	CRYPTO_UBICOM32_COMPOSITE_PRIORITY,
	.cra_flags		=	CRYPTO_ALG_TYPE_BLKCIPHER,
	.cra_blocksize		=	DES_BLOCK_SIZE,
	.cra_ctxsize		=	sizeof(struct ubicom32_des_ctx),
	.cra_alignmask		=	CRYPTO_UBICOM32_ALIGNMENT - 1,
	.cra_type		=	&crypto_blkcipher_type,
	.cra_module		=	THIS_MODULE,
	.cra_list		=	LIST_HEAD_INIT(cbc_des_alg.cra_list),
	.cra_u			= {
		.blkcipher = {
			.min_keysize		=	DES_KEY_SIZE,
			.max_keysize		=	DES_KEY_SIZE,
			.ivsize			=	DES_BLOCK_SIZE,
			.setkey			=	des_setkey,
			.encrypt		=	cbc_des_encrypt,
			.decrypt		=	cbc_des_decrypt,
		}
	}
};

/*
 * RFC2451:
 *
 *   For DES-EDE3, there is no known need to reject weak or
 *   complementation keys.  Any weakness is obviated by the use of
 *   multiple keys.
 *
 *   However, if the first two or last two independent 64-bit keys are
 *   equal (k1 == k2 or k2 == k3), then the DES3 operation is simply the
 *   same as DES.  Implementers MUST reject keys that exhibit this
 *   property.
 *
 */
static int des3_192_setkey(struct crypto_tfm *tfm, const u8 *key,
			   unsigned int keylen)
{
	int i, ret;
	struct ubicom32_des_ctx *dctx = crypto_tfm_ctx(tfm);
	const u8 *temp_key = key;
	u32 *flags = &tfm->crt_flags;

	if (!(memcmp(key, &key[DES_KEY_SIZE], DES_KEY_SIZE) &&
	    memcmp(&key[DES_KEY_SIZE], &key[DES_KEY_SIZE * 2],
		   DES_KEY_SIZE))) {

		*flags |= CRYPTO_TFM_RES_BAD_KEY_SCHED;
		return -EINVAL;
	}
	for (i = 0; i < 3; i++, temp_key += DES_KEY_SIZE) {
		ret = crypto_des_check_key(temp_key, DES_KEY_SIZE, flags);
		if (ret < 0)
			return ret;
	}
	memcpy(dctx->key, key, keylen);
	dctx->ctrl = SEC_ALG_DES;	//hw 3DES not working yet
	dctx->key_len = keylen;
	return 0;
}

static void des3_192_encrypt(struct crypto_tfm *tfm, u8 *dst, const u8 *src)
{
	struct ubicom32_des_ctx *uctx = crypto_tfm_ctx(tfm);

	hw_crypto_lock();
	hw_crypto_check();

	des3_hw_ede_encrypt(uctx->key, dst, src);

	hw_crypto_unlock();
}

static void des3_192_decrypt(struct crypto_tfm *tfm, u8 *dst, const u8 *src)
{
	struct ubicom32_des_ctx *uctx = crypto_tfm_ctx(tfm);

	hw_crypto_lock();
	hw_crypto_check();

	des3_hw_ede_decrypt(uctx->key, dst, src);

	hw_crypto_unlock();
}

static struct crypto_alg des3_192_alg = {
	.cra_name		=	"des3_ede",
	.cra_driver_name	=	"des3_ede-ubicom32",
	.cra_priority		=	CRYPTO_UBICOM32_PRIORITY,
	.cra_flags		=	CRYPTO_ALG_TYPE_CIPHER,
	.cra_blocksize		=	DES3_192_BLOCK_SIZE,
	.cra_ctxsize		=	sizeof(struct ubicom32_des_ctx),
	.cra_alignmask		=	CRYPTO_UBICOM32_ALIGNMENT - 1,
	.cra_module		=	THIS_MODULE,
	.cra_list		=	LIST_HEAD_INIT(des3_192_alg.cra_list),
	.cra_u			= {
		.cipher = {
			.cia_min_keysize	=	DES3_192_KEY_SIZE,
			.cia_max_keysize	=	DES3_192_KEY_SIZE,
			.cia_setkey		=	des3_192_setkey,
			.cia_encrypt		=	des3_192_encrypt,
			.cia_decrypt		=	des3_192_decrypt,
		}
	}
};

static int ecb_des3_192_encrypt(struct blkcipher_desc *desc,
				struct scatterlist *dst,
				struct scatterlist *src, unsigned int nbytes)
{
	return des_cipher(desc, dst, src, nbytes, SEC_CBC_NONE, DES3_EDE_ENCRYPT);
}

static int ecb_des3_192_decrypt(struct blkcipher_desc *desc,
				struct scatterlist *dst,
				struct scatterlist *src, unsigned int nbytes)
{
	return des_cipher(desc, dst, src, nbytes, SEC_CBC_NONE, DES3_EDE_DECRYPT);
}

static struct crypto_alg ecb_des3_192_alg = {
	.cra_name		=	"ecb(des3_ede)",
	.cra_driver_name	=	"ecb-des3_ede-ubicom32",
	.cra_priority		=	CRYPTO_UBICOM32_COMPOSITE_PRIORITY,
	.cra_flags		=	CRYPTO_ALG_TYPE_BLKCIPHER,
	.cra_blocksize		=	DES3_192_BLOCK_SIZE,
	.cra_ctxsize		=	sizeof(struct ubicom32_des_ctx),
	.cra_alignmask		=	CRYPTO_UBICOM32_ALIGNMENT - 1,
	.cra_type		=	&crypto_blkcipher_type,
	.cra_module		=	THIS_MODULE,
	.cra_list		=	LIST_HEAD_INIT(
						ecb_des3_192_alg.cra_list),
	.cra_u			= {
		.blkcipher = {
			.min_keysize		=	DES3_192_KEY_SIZE,
			.max_keysize		=	DES3_192_KEY_SIZE,
			.setkey			=	des3_192_setkey,
			.encrypt		=	ecb_des3_192_encrypt,
			.decrypt		=	ecb_des3_192_decrypt,
		}
	}
};

static int cbc_des3_192_encrypt(struct blkcipher_desc *desc,
				struct scatterlist *dst,
				struct scatterlist *src, unsigned int nbytes)
{
	return des_cipher(desc, dst, src, nbytes, SEC_CBC_SET, DES3_EDE_ENCRYPT);
}

static int cbc_des3_192_decrypt(struct blkcipher_desc *desc,
				struct scatterlist *dst,
				struct scatterlist *src, unsigned int nbytes)
{
	return des_cipher(desc, dst, src, nbytes, SEC_CBC_SET, DES3_EDE_DECRYPT);
}

static struct crypto_alg cbc_des3_192_alg = {
	.cra_name		=	"cbc(des3_ede)",
	.cra_driver_name	=	"cbc-des3_ede-ubicom32",
	.cra_priority		=	CRYPTO_UBICOM32_COMPOSITE_PRIORITY,
	.cra_flags		=	CRYPTO_ALG_TYPE_BLKCIPHER,
	.cra_blocksize		=	DES3_192_BLOCK_SIZE,
	.cra_ctxsize		=	sizeof(struct ubicom32_des_ctx),
	.cra_alignmask		=	CRYPTO_UBICOM32_ALIGNMENT - 1,
	.cra_type		=	&crypto_blkcipher_type,
	.cra_module		=	THIS_MODULE,
	.cra_list		=	LIST_HEAD_INIT(
						cbc_des3_192_alg.cra_list),
	.cra_u			= {
		.blkcipher = {
			.min_keysize		=	DES3_192_KEY_SIZE,
			.max_keysize		=	DES3_192_KEY_SIZE,
			.ivsize			=	DES3_192_BLOCK_SIZE,
			.setkey			=	des3_192_setkey,
			.encrypt		=	cbc_des3_192_encrypt,
			.decrypt		=	cbc_des3_192_decrypt,
		}
	}
};

static int init(void)
{
	int ret = 0;

	hw_crypto_init();

	ret = crypto_register_alg(&des_alg);
	if (ret)
		goto des_err;
	ret = crypto_register_alg(&ecb_des_alg);
	if (ret)
		goto ecb_des_err;
	ret = crypto_register_alg(&cbc_des_alg);
	if (ret)
		goto cbc_des_err;

	ret = crypto_register_alg(&des3_192_alg);
	if (ret)
		goto des3_192_err;
	ret = crypto_register_alg(&ecb_des3_192_alg);
	if (ret)
		goto ecb_des3_192_err;
	ret = crypto_register_alg(&cbc_des3_192_alg);
	if (ret)
		goto cbc_des3_192_err;

out:
	return ret;

cbc_des3_192_err:
	crypto_unregister_alg(&ecb_des3_192_alg);
ecb_des3_192_err:
	crypto_unregister_alg(&des3_192_alg);
des3_192_err:
	crypto_unregister_alg(&cbc_des_alg);
cbc_des_err:
	crypto_unregister_alg(&ecb_des_alg);
ecb_des_err:
	crypto_unregister_alg(&des_alg);
des_err:
	goto out;
}

static void __exit fini(void)
{
	crypto_unregister_alg(&cbc_des3_192_alg);
	crypto_unregister_alg(&ecb_des3_192_alg);
	crypto_unregister_alg(&des3_192_alg);
	crypto_unregister_alg(&cbc_des_alg);
	crypto_unregister_alg(&ecb_des_alg);
	crypto_unregister_alg(&des_alg);
}

module_init(init);
module_exit(fini);

MODULE_ALIAS("des");
MODULE_ALIAS("des3_ede");

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("DES & Triple DES EDE Cipher Algorithms");

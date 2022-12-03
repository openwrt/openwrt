// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2019 - 2021
 *
 * Richard van Schagen <vschagen@icloud.com>
 */

#if IS_ENABLED(CONFIG_CRYPTO_DEV_EIP93_AES)
#include <crypto/aes.h>
#include <crypto/ctr.h>
#endif
#if IS_ENABLED(CONFIG_CRYPTO_DEV_EIP93_DES)
#include <crypto/internal/des.h>
#endif
#include <linux/dma-mapping.h>

#include "eip93-cipher.h"
#include "eip93-common.h"
#include "eip93-regs.h"

void mtk_skcipher_handle_result(struct crypto_async_request *async, int err)
{
	struct mtk_crypto_ctx *ctx = crypto_tfm_ctx(async->tfm);
	struct mtk_device *mtk = ctx->mtk;
	struct skcipher_request *req = skcipher_request_cast(async);
	struct mtk_cipher_reqctx *rctx = skcipher_request_ctx(req);

	mtk_unmap_dma(mtk, rctx, req->src, req->dst);
	mtk_handle_result(mtk, rctx, req->iv);

	skcipher_request_complete(req, err);
}

static int mtk_skcipher_send_req(struct crypto_async_request *async)
{
	struct skcipher_request *req = skcipher_request_cast(async);
	struct mtk_cipher_reqctx *rctx = skcipher_request_ctx(req);
#if IS_ENABLED(CONFIG_CRYPTO_DEV_EIP93_POLL)
	struct mtk_crypto_ctx *ctx = crypto_tfm_ctx(async->tfm);
	struct mtk_device *mtk = ctx->mtk;
#endif
	int err;

	err = check_valid_request(rctx);

	if (err) {
		skcipher_request_complete(req, err);
		return err;
	}
	err = mtk_send_req(async, req->iv, rctx);
	if (err != -EINPROGRESS)
		return err;

#if IS_ENABLED(CONFIG_CRYPTO_DEV_EIP93_POLL)
	mtk_handle_result_polling(mtk);
#endif
	return -EINPROGRESS;
}

/* Crypto skcipher API functions */
static int mtk_skcipher_cra_init(struct crypto_tfm *tfm)
{
	struct mtk_crypto_ctx *ctx = crypto_tfm_ctx(tfm);
	struct mtk_alg_template *tmpl = container_of(tfm->__crt_alg,
				struct mtk_alg_template, alg.skcipher.base);

	crypto_skcipher_set_reqsize(__crypto_skcipher_cast(tfm),
					sizeof(struct mtk_cipher_reqctx));

	memset(ctx, 0, sizeof(*ctx));

	ctx->mtk = tmpl->mtk;

	ctx->sa_in = kzalloc(sizeof(struct saRecord_s), GFP_KERNEL);
	if (!ctx->sa_in)
		return -ENOMEM;

	ctx->sa_base_in = dma_map_single(ctx->mtk->dev, ctx->sa_in,
				sizeof(struct saRecord_s), DMA_TO_DEVICE);

	ctx->sa_out = kzalloc(sizeof(struct saRecord_s), GFP_KERNEL);
	if (!ctx->sa_out)
		return -ENOMEM;

	ctx->sa_base_out = dma_map_single(ctx->mtk->dev, ctx->sa_out,
				sizeof(struct saRecord_s), DMA_TO_DEVICE);
	return 0;
}

static void mtk_skcipher_cra_exit(struct crypto_tfm *tfm)
{
	struct mtk_crypto_ctx *ctx = crypto_tfm_ctx(tfm);

	dma_unmap_single(ctx->mtk->dev, ctx->sa_base_in,
			sizeof(struct saRecord_s), DMA_TO_DEVICE);
	dma_unmap_single(ctx->mtk->dev, ctx->sa_base_out,
			sizeof(struct saRecord_s), DMA_TO_DEVICE);
	kfree(ctx->sa_in);
	kfree(ctx->sa_out);
}

static int mtk_skcipher_setkey(struct crypto_skcipher *ctfm, const u8 *key,
				 unsigned int len)
{
	struct crypto_tfm *tfm = crypto_skcipher_tfm(ctfm);
	struct mtk_crypto_ctx *ctx = crypto_tfm_ctx(tfm);
	struct mtk_alg_template *tmpl = container_of(tfm->__crt_alg,
				struct mtk_alg_template, alg.skcipher.base);
	u32 flags = tmpl->flags;
	struct saRecord_s *saRecord = ctx->sa_out;
	u32 nonce = 0;
	unsigned int keylen = len;
	int sa_size = sizeof(struct saRecord_s);
	int err = -EINVAL;

	if (!key || !keylen)
		return err;

#if IS_ENABLED(CONFIG_CRYPTO_DEV_EIP93_AES)
	if (IS_RFC3686(flags)) {
		if (len < CTR_RFC3686_NONCE_SIZE)
			return err;

		keylen = len - CTR_RFC3686_NONCE_SIZE;
		memcpy(&nonce, key + keylen, CTR_RFC3686_NONCE_SIZE);
	}
#endif

#if IS_ENABLED(CONFIG_CRYPTO_DEV_EIP93_DES)
	if (flags & MTK_ALG_DES) {
		ctx->blksize = DES_BLOCK_SIZE;
		err = verify_skcipher_des_key(ctfm, key);
	}
	if (flags & MTK_ALG_3DES) {
		ctx->blksize = DES3_EDE_BLOCK_SIZE;
		err = verify_skcipher_des3_key(ctfm, key);
	}
#endif
#if IS_ENABLED(CONFIG_CRYPTO_DEV_EIP93_AES)
	if (flags & MTK_ALG_AES) {
		struct crypto_aes_ctx aes;

		ctx->blksize = AES_BLOCK_SIZE;
		err = aes_expandkey(&aes, key, keylen);
	}
#endif
	if (err)
		return err;

	dma_unmap_single(ctx->mtk->dev, ctx->sa_base_in, sa_size,
								DMA_TO_DEVICE);

	dma_unmap_single(ctx->mtk->dev, ctx->sa_base_out, sa_size,
								DMA_TO_DEVICE);

	mtk_set_saRecord(saRecord, keylen, flags);

	memcpy(saRecord->saKey, key, keylen);
	ctx->saNonce = nonce;
	saRecord->saNonce = nonce;
	saRecord->saCmd0.bits.direction = 0;

	memcpy(ctx->sa_in, saRecord, sa_size);
	ctx->sa_in->saCmd0.bits.direction = 1;

	ctx->sa_base_out = dma_map_single(ctx->mtk->dev, ctx->sa_out, sa_size,
								DMA_TO_DEVICE);

	ctx->sa_base_in = dma_map_single(ctx->mtk->dev, ctx->sa_in, sa_size,
								DMA_TO_DEVICE);
	return err;
}

static int mtk_skcipher_crypt(struct skcipher_request *req)
{
	struct mtk_cipher_reqctx *rctx = skcipher_request_ctx(req);
	struct crypto_async_request *async = &req->base;
	struct mtk_crypto_ctx *ctx = crypto_tfm_ctx(req->base.tfm);
	struct crypto_skcipher *skcipher = crypto_skcipher_reqtfm(req);

	if (!req->cryptlen)
		return 0;

	rctx->assoclen = 0;
	rctx->textsize = req->cryptlen;
	rctx->authsize = 0;
	rctx->sg_src = req->src;
	rctx->sg_dst = req->dst;
	rctx->ivsize = crypto_skcipher_ivsize(skcipher);
	rctx->blksize = ctx->blksize;
	rctx->flags |= MTK_DESC_SKCIPHER;
	if (!IS_ECB(rctx->flags))
		rctx->flags |= MTK_DESC_DMA_IV;

	return mtk_skcipher_send_req(async);
}

static int mtk_skcipher_encrypt(struct skcipher_request *req)
{
	struct mtk_crypto_ctx *ctx = crypto_tfm_ctx(req->base.tfm);
	struct mtk_cipher_reqctx *rctx = skcipher_request_ctx(req);
	struct mtk_alg_template *tmpl = container_of(req->base.tfm->__crt_alg,
				struct mtk_alg_template, alg.skcipher.base);

	rctx->flags = tmpl->flags;
	rctx->flags |= MTK_ENCRYPT;
	rctx->saRecord_base = ctx->sa_base_out;

	return mtk_skcipher_crypt(req);
}

static int mtk_skcipher_decrypt(struct skcipher_request *req)
{
	struct mtk_crypto_ctx *ctx = crypto_tfm_ctx(req->base.tfm);
	struct mtk_cipher_reqctx *rctx = skcipher_request_ctx(req);
	struct mtk_alg_template *tmpl = container_of(req->base.tfm->__crt_alg,
				struct mtk_alg_template, alg.skcipher.base);

	rctx->flags = tmpl->flags;
	rctx->flags |= MTK_DECRYPT;
	rctx->saRecord_base = ctx->sa_base_in;

	return mtk_skcipher_crypt(req);
}

/* Available algorithms in this module */
#if IS_ENABLED(CONFIG_CRYPTO_DEV_EIP93_AES)
struct mtk_alg_template mtk_alg_ecb_aes = {
	.type = MTK_ALG_TYPE_SKCIPHER,
	.flags = MTK_MODE_ECB | MTK_ALG_AES,
	.alg.skcipher = {
		.setkey = mtk_skcipher_setkey,
		.encrypt = mtk_skcipher_encrypt,
		.decrypt = mtk_skcipher_decrypt,
		.min_keysize = AES_MIN_KEY_SIZE,
		.max_keysize = AES_MAX_KEY_SIZE,
		.ivsize	= 0,
		.base = {
			.cra_name = "ecb(aes)",
			.cra_driver_name = "ecb(aes-eip93)",
			.cra_priority = MTK_CRA_PRIORITY,
			.cra_flags = CRYPTO_ALG_ASYNC |
					CRYPTO_ALG_NEED_FALLBACK |
					CRYPTO_ALG_KERN_DRIVER_ONLY,
			.cra_blocksize = AES_BLOCK_SIZE,
			.cra_ctxsize = sizeof(struct mtk_crypto_ctx),
			.cra_alignmask = 0xf,
			.cra_init = mtk_skcipher_cra_init,
			.cra_exit = mtk_skcipher_cra_exit,
			.cra_module = THIS_MODULE,
		},
	},
};

struct mtk_alg_template mtk_alg_cbc_aes = {
	.type = MTK_ALG_TYPE_SKCIPHER,
	.flags = MTK_MODE_CBC | MTK_ALG_AES,
	.alg.skcipher = {
		.setkey = mtk_skcipher_setkey,
		.encrypt = mtk_skcipher_encrypt,
		.decrypt = mtk_skcipher_decrypt,
		.min_keysize = AES_MIN_KEY_SIZE,
		.max_keysize = AES_MAX_KEY_SIZE,
		.ivsize	= AES_BLOCK_SIZE,
		.base = {
			.cra_name = "cbc(aes)",
			.cra_driver_name = "cbc(aes-eip93)",
			.cra_priority = MTK_CRA_PRIORITY,
			.cra_flags = CRYPTO_ALG_ASYNC |
					CRYPTO_ALG_NEED_FALLBACK |
					CRYPTO_ALG_KERN_DRIVER_ONLY,
			.cra_blocksize = AES_BLOCK_SIZE,
			.cra_ctxsize = sizeof(struct mtk_crypto_ctx),
			.cra_alignmask = 0xf,
			.cra_init = mtk_skcipher_cra_init,
			.cra_exit = mtk_skcipher_cra_exit,
			.cra_module = THIS_MODULE,
		},
	},
};

struct mtk_alg_template mtk_alg_ctr_aes = {
	.type = MTK_ALG_TYPE_SKCIPHER,
	.flags = MTK_MODE_CTR | MTK_ALG_AES,
	.alg.skcipher = {
		.setkey = mtk_skcipher_setkey,
		.encrypt = mtk_skcipher_encrypt,
		.decrypt = mtk_skcipher_decrypt,
		.min_keysize = AES_MIN_KEY_SIZE,
		.max_keysize = AES_MAX_KEY_SIZE,
		.ivsize	= AES_BLOCK_SIZE,
		.base = {
			.cra_name = "ctr(aes)",
			.cra_driver_name = "ctr(aes-eip93)",
			.cra_priority = MTK_CRA_PRIORITY,
			.cra_flags = CRYPTO_ALG_ASYNC |
				     CRYPTO_ALG_NEED_FALLBACK |
				     CRYPTO_ALG_KERN_DRIVER_ONLY,
			.cra_blocksize = 1,
			.cra_ctxsize = sizeof(struct mtk_crypto_ctx),
			.cra_alignmask = 0xf,
			.cra_init = mtk_skcipher_cra_init,
			.cra_exit = mtk_skcipher_cra_exit,
			.cra_module = THIS_MODULE,
		},
	},
};

struct mtk_alg_template mtk_alg_rfc3686_aes = {
	.type = MTK_ALG_TYPE_SKCIPHER,
	.flags = MTK_MODE_CTR | MTK_MODE_RFC3686 | MTK_ALG_AES,
	.alg.skcipher = {
		.setkey = mtk_skcipher_setkey,
		.encrypt = mtk_skcipher_encrypt,
		.decrypt = mtk_skcipher_decrypt,
		.min_keysize = AES_MIN_KEY_SIZE + CTR_RFC3686_NONCE_SIZE,
		.max_keysize = AES_MAX_KEY_SIZE + CTR_RFC3686_NONCE_SIZE,
		.ivsize	= CTR_RFC3686_IV_SIZE,
		.base = {
			.cra_name = "rfc3686(ctr(aes))",
			.cra_driver_name = "rfc3686(ctr(aes-eip93))",
			.cra_priority = MTK_CRA_PRIORITY,
			.cra_flags = CRYPTO_ALG_ASYNC |
					CRYPTO_ALG_NEED_FALLBACK |
					CRYPTO_ALG_KERN_DRIVER_ONLY,
			.cra_blocksize = 1,
			.cra_ctxsize = sizeof(struct mtk_crypto_ctx),
			.cra_alignmask = 0xf,
			.cra_init = mtk_skcipher_cra_init,
			.cra_exit = mtk_skcipher_cra_exit,
			.cra_module = THIS_MODULE,
		},
	},
};
#endif
#if IS_ENABLED(CONFIG_CRYPTO_DEV_EIP93_DES)
struct mtk_alg_template mtk_alg_ecb_des = {
	.type = MTK_ALG_TYPE_SKCIPHER,
	.flags = MTK_MODE_ECB | MTK_ALG_DES,
	.alg.skcipher = {
		.setkey = mtk_skcipher_setkey,
		.encrypt = mtk_skcipher_encrypt,
		.decrypt = mtk_skcipher_decrypt,
		.min_keysize = DES_KEY_SIZE,
		.max_keysize = DES_KEY_SIZE,
		.ivsize	= 0,
		.base = {
			.cra_name = "ecb(des)",
			.cra_driver_name = "ebc(des-eip93)",
			.cra_priority = MTK_CRA_PRIORITY,
			.cra_flags = CRYPTO_ALG_ASYNC |
					CRYPTO_ALG_KERN_DRIVER_ONLY,
			.cra_blocksize = DES_BLOCK_SIZE,
			.cra_ctxsize = sizeof(struct mtk_crypto_ctx),
			.cra_alignmask = 0,
			.cra_init = mtk_skcipher_cra_init,
			.cra_exit = mtk_skcipher_cra_exit,
			.cra_module = THIS_MODULE,
		},
	},
};

struct mtk_alg_template mtk_alg_cbc_des = {
	.type = MTK_ALG_TYPE_SKCIPHER,
	.flags = MTK_MODE_CBC | MTK_ALG_DES,
	.alg.skcipher = {
		.setkey = mtk_skcipher_setkey,
		.encrypt = mtk_skcipher_encrypt,
		.decrypt = mtk_skcipher_decrypt,
		.min_keysize = DES_KEY_SIZE,
		.max_keysize = DES_KEY_SIZE,
		.ivsize	= DES_BLOCK_SIZE,
		.base = {
			.cra_name = "cbc(des)",
			.cra_driver_name = "cbc(des-eip93)",
			.cra_priority = MTK_CRA_PRIORITY,
			.cra_flags = CRYPTO_ALG_ASYNC |
					CRYPTO_ALG_KERN_DRIVER_ONLY,
			.cra_blocksize = DES_BLOCK_SIZE,
			.cra_ctxsize = sizeof(struct mtk_crypto_ctx),
			.cra_alignmask = 0,
			.cra_init = mtk_skcipher_cra_init,
			.cra_exit = mtk_skcipher_cra_exit,
			.cra_module = THIS_MODULE,
		},
	},
};

struct mtk_alg_template mtk_alg_ecb_des3_ede = {
	.type = MTK_ALG_TYPE_SKCIPHER,
	.flags = MTK_MODE_ECB | MTK_ALG_3DES,
	.alg.skcipher = {
		.setkey = mtk_skcipher_setkey,
		.encrypt = mtk_skcipher_encrypt,
		.decrypt = mtk_skcipher_decrypt,
		.min_keysize = DES3_EDE_KEY_SIZE,
		.max_keysize = DES3_EDE_KEY_SIZE,
		.ivsize	= 0,
		.base = {
			.cra_name = "ecb(des3_ede)",
			.cra_driver_name = "ecb(des3_ede-eip93)",
			.cra_priority = MTK_CRA_PRIORITY,
			.cra_flags = CRYPTO_ALG_ASYNC |
					CRYPTO_ALG_KERN_DRIVER_ONLY,
			.cra_blocksize = DES3_EDE_BLOCK_SIZE,
			.cra_ctxsize = sizeof(struct mtk_crypto_ctx),
			.cra_alignmask = 0,
			.cra_init = mtk_skcipher_cra_init,
			.cra_exit = mtk_skcipher_cra_exit,
			.cra_module = THIS_MODULE,
		},
	},
};

struct mtk_alg_template mtk_alg_cbc_des3_ede = {
	.type = MTK_ALG_TYPE_SKCIPHER,
	.flags = MTK_MODE_CBC | MTK_ALG_3DES,
	.alg.skcipher = {
		.setkey = mtk_skcipher_setkey,
		.encrypt = mtk_skcipher_encrypt,
		.decrypt = mtk_skcipher_decrypt,
		.min_keysize = DES3_EDE_KEY_SIZE,
		.max_keysize = DES3_EDE_KEY_SIZE,
		.ivsize	= DES3_EDE_BLOCK_SIZE,
		.base = {
			.cra_name = "cbc(des3_ede)",
			.cra_driver_name = "cbc(des3_ede-eip93)",
			.cra_priority = MTK_CRA_PRIORITY,
			.cra_flags = CRYPTO_ALG_ASYNC |
					CRYPTO_ALG_KERN_DRIVER_ONLY,
			.cra_blocksize = DES3_EDE_BLOCK_SIZE,
			.cra_ctxsize = sizeof(struct mtk_crypto_ctx),
			.cra_alignmask = 0,
			.cra_init = mtk_skcipher_cra_init,
			.cra_exit = mtk_skcipher_cra_exit,
			.cra_module = THIS_MODULE,
		},
	},
};
#endif

// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2019 - 2021
 *
 * Richard van Schagen <vschagen@icloud.com>
 */

#include <crypto/aead.h>
#include <crypto/aes.h>
#include <crypto/authenc.h>
#include <crypto/ctr.h>
#include <crypto/hmac.h>
#include <crypto/internal/aead.h>
#include <crypto/md5.h>
#include <crypto/null.h>
#include <crypto/sha.h>

#if IS_ENABLED(CONFIG_CRYPTO_DEV_EIP93_DES)
#include <crypto/internal/des.h>
#endif

#include <linux/crypto.h>
#include <linux/dma-mapping.h>

#include "eip93-aead.h"
#include "eip93-cipher.h"
#include "eip93-regs.h"
#include "eip93-common.h"

void mtk_aead_handle_result(struct crypto_async_request *async, int err)
{
	struct mtk_crypto_ctx *ctx = crypto_tfm_ctx(async->tfm);
	struct mtk_device *mtk = ctx->mtk;
	struct aead_request *req = aead_request_cast(async);
	struct mtk_cipher_reqctx *rctx = aead_request_ctx(req);

	mtk_unmap_dma(mtk, rctx, req->src, req->dst);
	mtk_handle_result(mtk, rctx, req->iv);

	if (err == 1)
		err = -EBADMSG;
	if (err == 4)
		err = 0; //SPI error

	aead_request_complete(req, err);
}

static int mtk_aead_send_req(struct crypto_async_request *async)
{
	struct aead_request *req = aead_request_cast(async);
	struct mtk_cipher_reqctx *rctx = aead_request_ctx(req);
	int err;
#if IS_ENABLED(CONFIG_CRYPTO_DEV_EIP93_POLL)
	struct mtk_crypto_ctx *ctx = crypto_tfm_ctx(req->base.tfm);
	struct mtk_device *mtk = ctx->mtk;
#endif

	err = check_valid_request(rctx);
	if (err) {
		aead_request_complete(req, err);
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

/* Crypto aead API functions */
static int mtk_aead_cra_init(struct crypto_tfm *tfm)
{
	struct mtk_crypto_ctx *ctx = crypto_tfm_ctx(tfm);
	struct mtk_alg_template *tmpl = container_of(tfm->__crt_alg,
				struct mtk_alg_template, alg.aead.base);
	u32 flags = tmpl->flags;
	char *alg_base;

	memset(ctx, 0, sizeof(*ctx));

	crypto_aead_set_reqsize(__crypto_aead_cast(tfm),
			sizeof(struct mtk_cipher_reqctx));

	ctx->mtk = tmpl->mtk;
	ctx->in_first = true;
	ctx->out_first = true;

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

	/* software workaround for now */
	if (IS_HASH_MD5(flags))
		alg_base = "md5";
	if (IS_HASH_SHA1(flags))
		alg_base = "sha1";
	if (IS_HASH_SHA224(flags))
		alg_base = "sha224";
	if (IS_HASH_SHA256(flags))
		alg_base = "sha256";

	ctx->shash = crypto_alloc_shash(alg_base, 0, CRYPTO_ALG_NEED_FALLBACK);

	if (IS_ERR(ctx->shash)) {
		dev_err(ctx->mtk->dev, "base driver %s could not be loaded.\n",
				alg_base);
		return PTR_ERR(ctx->shash);
	}

	return 0;
}

static void mtk_aead_cra_exit(struct crypto_tfm *tfm)
{
	struct mtk_crypto_ctx *ctx = crypto_tfm_ctx(tfm);

	if (ctx->shash)
		crypto_free_shash(ctx->shash);

	dma_unmap_single(ctx->mtk->dev, ctx->sa_base_in,
			sizeof(struct saRecord_s), DMA_TO_DEVICE);
	dma_unmap_single(ctx->mtk->dev, ctx->sa_base_out,
			sizeof(struct saRecord_s), DMA_TO_DEVICE);
	kfree(ctx->sa_in);
	kfree(ctx->sa_out);
}

static int mtk_aead_setkey(struct crypto_aead *ctfm, const u8 *key,
			unsigned int len)
{
	struct crypto_tfm *tfm = crypto_aead_tfm(ctfm);
	struct mtk_crypto_ctx *ctx = crypto_tfm_ctx(tfm);
	struct mtk_alg_template *tmpl = container_of(tfm->__crt_alg,
				struct mtk_alg_template, alg.skcipher.base);
	u32 flags = tmpl->flags;
	u32 nonce = 0;
	struct crypto_authenc_keys keys;
	struct crypto_aes_ctx aes;
	struct saRecord_s *saRecord = ctx->sa_out;
	int sa_size = sizeof(struct saRecord_s);
	int err = -EINVAL;

	if (crypto_authenc_extractkeys(&keys, key, len))
		return err;

	if (IS_RFC3686(flags)) {
		if (keys.enckeylen < CTR_RFC3686_NONCE_SIZE)
			return err;

		keys.enckeylen -= CTR_RFC3686_NONCE_SIZE;
		memcpy(&nonce, keys.enckey + keys.enckeylen,
						CTR_RFC3686_NONCE_SIZE);
	}

	switch ((flags & MTK_ALG_MASK)) {
#if IS_ENABLED(CONFIG_CRYPTO_DEV_EIP93_DES)
	case MTK_ALG_DES:
		err = verify_aead_des_key(ctfm, keys.enckey, keys.enckeylen);
		break;
	case MTK_ALG_3DES:
		if (keys.enckeylen != DES3_EDE_KEY_SIZE)
			return -EINVAL;

		err = verify_aead_des3_key(ctfm, keys.enckey, keys.enckeylen);
		break;
#endif
	case MTK_ALG_AES:
		err = aes_expandkey(&aes, keys.enckey, keys.enckeylen);
	}
	if (err)
		return err;

	ctx->blksize = crypto_aead_blocksize(ctfm);
	dma_unmap_single(ctx->mtk->dev, ctx->sa_base_in, sa_size,
								DMA_TO_DEVICE);

	dma_unmap_single(ctx->mtk->dev, ctx->sa_base_out, sa_size,
								DMA_TO_DEVICE);
	/* Encryption key */
	mtk_set_saRecord(saRecord, keys.enckeylen, flags);
	saRecord->saCmd0.bits.opCode = 1;
	saRecord->saCmd0.bits.digestLength = ctx->authsize >> 2;

	memcpy(saRecord->saKey, keys.enckey, keys.enckeylen);
	ctx->saNonce = nonce;
	saRecord->saNonce = nonce;

	/* authentication key */
	err = mtk_authenc_setkey(ctx->shash, saRecord, keys.authkey,
							keys.authkeylen);

	saRecord->saCmd0.bits.direction = 0;
	memcpy(ctx->sa_in, saRecord, sa_size);
	ctx->sa_in->saCmd0.bits.direction = 1;
	ctx->sa_in->saCmd1.bits.copyDigest = 0;

	ctx->sa_base_out = dma_map_single(ctx->mtk->dev, ctx->sa_out, sa_size,
								DMA_TO_DEVICE);
	ctx->sa_base_in = dma_map_single(ctx->mtk->dev, ctx->sa_in, sa_size,
								DMA_TO_DEVICE);
	ctx->in_first = true;
	ctx->out_first = true;

	return err;
}

static int mtk_aead_setauthsize(struct crypto_aead *ctfm,
				unsigned int authsize)
{
	struct crypto_tfm *tfm = crypto_aead_tfm(ctfm);
	struct mtk_crypto_ctx *ctx = crypto_tfm_ctx(tfm);

	dma_unmap_single(ctx->mtk->dev, ctx->sa_base_in,
				sizeof(struct saRecord_s), DMA_TO_DEVICE);

	dma_unmap_single(ctx->mtk->dev, ctx->sa_base_out,
				sizeof(struct saRecord_s), DMA_TO_DEVICE);

	ctx->authsize = authsize;
	ctx->sa_in->saCmd0.bits.digestLength = ctx->authsize >> 2;
	ctx->sa_out->saCmd0.bits.digestLength = ctx->authsize >> 2;

	ctx->sa_base_out = dma_map_single(ctx->mtk->dev, ctx->sa_out,
			sizeof(struct saRecord_s), DMA_TO_DEVICE);
	ctx->sa_base_in = dma_map_single(ctx->mtk->dev, ctx->sa_in,
				sizeof(struct saRecord_s), DMA_TO_DEVICE);
	return 0;
}

static void mtk_aead_setassoc(struct mtk_crypto_ctx *ctx,
			struct aead_request *req, bool in)
{
	struct saRecord_s *saRecord;

	if (in) {
		dma_unmap_single(ctx->mtk->dev, ctx->sa_base_in,
				sizeof(struct saRecord_s), DMA_TO_DEVICE);
		saRecord = ctx->sa_in;
		saRecord->saCmd1.bits.hashCryptOffset = req->assoclen >> 2;

		ctx->sa_base_in = dma_map_single(ctx->mtk->dev, ctx->sa_in,
				sizeof(struct saRecord_s), DMA_TO_DEVICE);
		ctx->assoclen_in = req->assoclen;
	} else {
		dma_unmap_single(ctx->mtk->dev, ctx->sa_base_out,
				sizeof(struct saRecord_s), DMA_TO_DEVICE);
		saRecord = ctx->sa_out;
		saRecord->saCmd1.bits.hashCryptOffset = req->assoclen >> 2;

		ctx->sa_base_out = dma_map_single(ctx->mtk->dev, ctx->sa_out,
			sizeof(struct saRecord_s), DMA_TO_DEVICE);
		ctx->assoclen_out = req->assoclen;
	}
}

static int mtk_aead_crypt(struct aead_request *req)
{
	struct mtk_cipher_reqctx *rctx = aead_request_ctx(req);
	struct crypto_async_request *async = &req->base;
	struct mtk_crypto_ctx *ctx = crypto_tfm_ctx(req->base.tfm);
	struct crypto_aead *aead = crypto_aead_reqtfm(req);

	rctx->textsize = req->cryptlen;
	rctx->blksize = ctx->blksize;
	rctx->assoclen = req->assoclen;
	rctx->authsize = ctx->authsize;
	rctx->sg_src = req->src;
	rctx->sg_dst = req->dst;
	rctx->ivsize = crypto_aead_ivsize(aead);
	rctx->flags |= MTK_DESC_AEAD;

	if IS_DECRYPT(rctx->flags)
		rctx->textsize -= rctx->authsize;

	return mtk_aead_send_req(async);
}

static int mtk_aead_encrypt(struct aead_request *req)
{
	struct mtk_crypto_ctx *ctx = crypto_tfm_ctx(req->base.tfm);
	struct mtk_cipher_reqctx *rctx = aead_request_ctx(req);
	struct mtk_alg_template *tmpl = container_of(req->base.tfm->__crt_alg,
				struct mtk_alg_template, alg.aead.base);

	rctx->flags = tmpl->flags;
	rctx->flags |= MTK_ENCRYPT;
	if (ctx->out_first) {
		mtk_aead_setassoc(ctx, req, false);
		ctx->out_first = false;
	}

	if (req->assoclen != ctx->assoclen_out) {
		dev_err(ctx->mtk->dev, "Request AAD length error\n");
		return -EINVAL;
	}

	rctx->saRecord_base = ctx->sa_base_out;

	return mtk_aead_crypt(req);
}

static int mtk_aead_decrypt(struct aead_request *req)
{
	struct mtk_crypto_ctx *ctx = crypto_tfm_ctx(req->base.tfm);
	struct mtk_cipher_reqctx *rctx = aead_request_ctx(req);
	struct mtk_alg_template *tmpl = container_of(req->base.tfm->__crt_alg,
				struct mtk_alg_template, alg.aead.base);

	rctx->flags = tmpl->flags;
	rctx->flags |= MTK_DECRYPT;
	if (ctx->in_first) {
		mtk_aead_setassoc(ctx, req, true);
		ctx->in_first = false;
	}

	if (req->assoclen != ctx->assoclen_in) {
		dev_err(ctx->mtk->dev, "Request AAD length error\n");
		return -EINVAL;
	}

	rctx->saRecord_base = ctx->sa_base_in;

	return mtk_aead_crypt(req);
}

/* Available authenc algorithms in this module */
#if IS_ENABLED(CONFIG_CRYPTO_DEV_EIP93_AES)
struct mtk_alg_template mtk_alg_authenc_hmac_md5_cbc_aes = {
	.type = MTK_ALG_TYPE_AEAD,
	.flags = MTK_HASH_HMAC | MTK_HASH_MD5 | MTK_MODE_CBC | MTK_ALG_AES,
	.alg.aead = {
		.setkey = mtk_aead_setkey,
		.encrypt = mtk_aead_encrypt,
		.decrypt = mtk_aead_decrypt,
		.ivsize	= AES_BLOCK_SIZE,
		.setauthsize = mtk_aead_setauthsize,
		.maxauthsize = MD5_DIGEST_SIZE,
		.base = {
			.cra_name = "authenc(hmac(md5),cbc(aes))",
			.cra_driver_name =
				"authenc(hmac(md5-eip93), cbc(aes-eip93))",
			.cra_priority = MTK_CRA_PRIORITY,
			.cra_flags = CRYPTO_ALG_ASYNC |
					CRYPTO_ALG_KERN_DRIVER_ONLY,
			.cra_blocksize = AES_BLOCK_SIZE,
			.cra_ctxsize = sizeof(struct mtk_crypto_ctx),
			.cra_alignmask = 0,
			.cra_init = mtk_aead_cra_init,
			.cra_exit = mtk_aead_cra_exit,
			.cra_module = THIS_MODULE,
		},
	},
};

struct mtk_alg_template mtk_alg_authenc_hmac_sha1_cbc_aes = {
	.type = MTK_ALG_TYPE_AEAD,
	.flags = MTK_HASH_HMAC | MTK_HASH_SHA1 | MTK_MODE_CBC | MTK_ALG_AES,
	.alg.aead = {
		.setkey = mtk_aead_setkey,
		.encrypt = mtk_aead_encrypt,
		.decrypt = mtk_aead_decrypt,
		.ivsize	= AES_BLOCK_SIZE,
		.setauthsize = mtk_aead_setauthsize,
		.maxauthsize = SHA1_DIGEST_SIZE,
		.base = {
			.cra_name = "authenc(hmac(sha1),cbc(aes))",
			.cra_driver_name =
				"authenc(hmac(sha1-eip93),cbc(aes-eip93))",
			.cra_priority = MTK_CRA_PRIORITY,
			.cra_flags = CRYPTO_ALG_ASYNC |
					CRYPTO_ALG_KERN_DRIVER_ONLY,
			.cra_blocksize = AES_BLOCK_SIZE,
			.cra_ctxsize = sizeof(struct mtk_crypto_ctx),
			.cra_alignmask = 0,
			.cra_init = mtk_aead_cra_init,
			.cra_exit = mtk_aead_cra_exit,
			.cra_module = THIS_MODULE,
		},
	},
};

struct mtk_alg_template mtk_alg_authenc_hmac_sha224_cbc_aes = {
	.type = MTK_ALG_TYPE_AEAD,
	.flags = MTK_HASH_HMAC | MTK_HASH_SHA224 | MTK_MODE_CBC | MTK_ALG_AES,
	.alg.aead = {
		.setkey = mtk_aead_setkey,
		.encrypt = mtk_aead_encrypt,
		.decrypt = mtk_aead_decrypt,
		.ivsize	= AES_BLOCK_SIZE,
		.setauthsize = mtk_aead_setauthsize,
		.maxauthsize = SHA224_DIGEST_SIZE,
		.base = {
			.cra_name = "authenc(hmac(sha224),cbc(aes))",
			.cra_driver_name =
				"authenc(hmac(sha224-eip93),cbc(aes-eip93))",
			.cra_priority = MTK_CRA_PRIORITY,
			.cra_flags = CRYPTO_ALG_ASYNC |
					CRYPTO_ALG_KERN_DRIVER_ONLY,
			.cra_blocksize = AES_BLOCK_SIZE,
			.cra_ctxsize = sizeof(struct mtk_crypto_ctx),
			.cra_alignmask = 0,
			.cra_init = mtk_aead_cra_init,
			.cra_exit = mtk_aead_cra_exit,
			.cra_module = THIS_MODULE,
		},
	},
};

struct mtk_alg_template mtk_alg_authenc_hmac_sha256_cbc_aes = {
	.type = MTK_ALG_TYPE_AEAD,
	.flags = MTK_HASH_HMAC | MTK_HASH_SHA256 | MTK_MODE_CBC | MTK_ALG_AES,
	.alg.aead = {
		.setkey = mtk_aead_setkey,
		.encrypt = mtk_aead_encrypt,
		.decrypt = mtk_aead_decrypt,
		.ivsize	= AES_BLOCK_SIZE,
		.setauthsize = mtk_aead_setauthsize,
		.maxauthsize = SHA256_DIGEST_SIZE,
		.base = {
			.cra_name = "authenc(hmac(sha256),cbc(aes))",
			.cra_driver_name =
				"authenc(hmac(sha256-eip93),cbc(aes-eip93))",
			.cra_priority = MTK_CRA_PRIORITY,
			.cra_flags = CRYPTO_ALG_ASYNC |
					CRYPTO_ALG_KERN_DRIVER_ONLY,
			.cra_blocksize = AES_BLOCK_SIZE,
			.cra_ctxsize = sizeof(struct mtk_crypto_ctx),
			.cra_alignmask = 0,
			.cra_init = mtk_aead_cra_init,
			.cra_exit = mtk_aead_cra_exit,
			.cra_module = THIS_MODULE,
		},
	},
};

struct mtk_alg_template mtk_alg_authenc_hmac_md5_rfc3686_aes = {
	.type = MTK_ALG_TYPE_AEAD,
	.flags = MTK_HASH_HMAC | MTK_HASH_MD5 |
			MTK_MODE_CTR | MTK_MODE_RFC3686 | MTK_ALG_AES,
	.alg.aead = {
		.setkey = mtk_aead_setkey,
		.encrypt = mtk_aead_encrypt,
		.decrypt = mtk_aead_decrypt,
		.ivsize	= CTR_RFC3686_IV_SIZE,
		.setauthsize = mtk_aead_setauthsize,
		.maxauthsize = MD5_DIGEST_SIZE,
		.base = {
			.cra_name = "authenc(hmac(md5),rfc3686(ctr(aes)))",
			.cra_driver_name =
			"authenc(hmac(md5-eip93),rfc3686(ctr(aes-eip93)))",
			.cra_priority = MTK_CRA_PRIORITY,
			.cra_flags = CRYPTO_ALG_ASYNC |
					CRYPTO_ALG_KERN_DRIVER_ONLY,
			.cra_blocksize = 1,
			.cra_ctxsize = sizeof(struct mtk_crypto_ctx),
			.cra_alignmask = 0,
			.cra_init = mtk_aead_cra_init,
			.cra_exit = mtk_aead_cra_exit,
			.cra_module = THIS_MODULE,
		},
	},
};

struct mtk_alg_template mtk_alg_authenc_hmac_sha1_rfc3686_aes = {
	.type = MTK_ALG_TYPE_AEAD,
	.flags = MTK_HASH_HMAC | MTK_HASH_SHA1 |
			MTK_MODE_CTR | MTK_MODE_RFC3686 | MTK_ALG_AES,
	.alg.aead = {
		.setkey = mtk_aead_setkey,
		.encrypt = mtk_aead_encrypt,
		.decrypt = mtk_aead_decrypt,
		.ivsize	= CTR_RFC3686_IV_SIZE,
		.setauthsize = mtk_aead_setauthsize,
		.maxauthsize = SHA1_DIGEST_SIZE,
		.base = {
			.cra_name = "authenc(hmac(sha1),rfc3686(ctr(aes)))",
			.cra_driver_name =
			"authenc(hmac(sha1-eip93),rfc3686(ctr(aes-eip93)))",
			.cra_priority = MTK_CRA_PRIORITY,
			.cra_flags = CRYPTO_ALG_ASYNC |
					CRYPTO_ALG_KERN_DRIVER_ONLY,
			.cra_blocksize = 1,
			.cra_ctxsize = sizeof(struct mtk_crypto_ctx),
			.cra_alignmask = 0,
			.cra_init = mtk_aead_cra_init,
			.cra_exit = mtk_aead_cra_exit,
			.cra_module = THIS_MODULE,
		},
	},
};

struct mtk_alg_template mtk_alg_authenc_hmac_sha224_rfc3686_aes = {
	.type = MTK_ALG_TYPE_AEAD,
	.flags = MTK_HASH_HMAC | MTK_HASH_SHA224 |
			MTK_MODE_CTR | MTK_MODE_RFC3686 | MTK_ALG_AES,
	.alg.aead = {
		.setkey = mtk_aead_setkey,
		.encrypt = mtk_aead_encrypt,
		.decrypt = mtk_aead_decrypt,
		.ivsize	= CTR_RFC3686_IV_SIZE,
		.setauthsize = mtk_aead_setauthsize,
		.maxauthsize = SHA224_DIGEST_SIZE,
		.base = {
			.cra_name = "authenc(hmac(sha224),rfc3686(ctr(aes)))",
			.cra_driver_name =
			"authenc(hmac(sha224-eip93),rfc3686(ctr(aes-eip93)))",
			.cra_priority = MTK_CRA_PRIORITY,
			.cra_flags = CRYPTO_ALG_ASYNC |
					CRYPTO_ALG_KERN_DRIVER_ONLY,
			.cra_blocksize = 1,
			.cra_ctxsize = sizeof(struct mtk_crypto_ctx),
			.cra_alignmask = 0,
			.cra_init = mtk_aead_cra_init,
			.cra_exit = mtk_aead_cra_exit,
			.cra_module = THIS_MODULE,
		},
	},
};

struct mtk_alg_template mtk_alg_authenc_hmac_sha256_rfc3686_aes = {
	.type = MTK_ALG_TYPE_AEAD,
	.flags = MTK_HASH_HMAC | MTK_HASH_SHA256 |
			MTK_MODE_CTR | MTK_MODE_RFC3686 | MTK_ALG_AES,
	.alg.aead = {
		.setkey = mtk_aead_setkey,
		.encrypt = mtk_aead_encrypt,
		.decrypt = mtk_aead_decrypt,
		.ivsize	= CTR_RFC3686_IV_SIZE,
		.setauthsize = mtk_aead_setauthsize,
		.maxauthsize = SHA256_DIGEST_SIZE,
		.base = {
			.cra_name = "authenc(hmac(sha256),rfc3686(ctr(aes)))",
			.cra_driver_name =
			"authenc(hmac(sha256-eip93),rfc3686(ctr(aes-eip93)))",
			.cra_priority = MTK_CRA_PRIORITY,
			.cra_flags = CRYPTO_ALG_ASYNC |
					CRYPTO_ALG_KERN_DRIVER_ONLY,
			.cra_blocksize = 1,
			.cra_ctxsize = sizeof(struct mtk_crypto_ctx),
			.cra_alignmask = 0,
			.cra_init = mtk_aead_cra_init,
			.cra_exit = mtk_aead_cra_exit,
			.cra_module = THIS_MODULE,
		},
	},
};
#endif
#if IS_ENABLED(CONFIG_CRYPTO_DEV_EIP93_DES)
struct mtk_alg_template mtk_alg_authenc_hmac_md5_cbc_des = {
	.type = MTK_ALG_TYPE_AEAD,
	.flags = MTK_HASH_HMAC | MTK_HASH_MD5 | MTK_MODE_CBC | MTK_ALG_DES,
	.alg.aead = {
		.setkey = mtk_aead_setkey,
		.encrypt = mtk_aead_encrypt,
		.decrypt = mtk_aead_decrypt,
		.ivsize	= DES_BLOCK_SIZE,
		.setauthsize = mtk_aead_setauthsize,
		.maxauthsize = MD5_DIGEST_SIZE,
		.base = {
			.cra_name = "authenc(hmac(md5),cbc(des))",
			.cra_driver_name =
				"authenc(hmac(md5-eip93),cbc(des-eip93))",
			.cra_priority = MTK_CRA_PRIORITY,
			.cra_flags = CRYPTO_ALG_ASYNC |
					CRYPTO_ALG_KERN_DRIVER_ONLY,
			.cra_blocksize = DES_BLOCK_SIZE,
			.cra_ctxsize = sizeof(struct mtk_crypto_ctx),
			.cra_alignmask = 0,
			.cra_init = mtk_aead_cra_init,
			.cra_exit = mtk_aead_cra_exit,
			.cra_module = THIS_MODULE,
		},
	},
};

struct mtk_alg_template mtk_alg_authenc_hmac_sha1_cbc_des = {
	.type = MTK_ALG_TYPE_AEAD,
	.flags = MTK_HASH_HMAC | MTK_HASH_SHA1 | MTK_MODE_CBC | MTK_ALG_DES,
	.alg.aead = {
		.setkey = mtk_aead_setkey,
		.encrypt = mtk_aead_encrypt,
		.decrypt = mtk_aead_decrypt,
		.ivsize	= DES_BLOCK_SIZE,
		.setauthsize = mtk_aead_setauthsize,
		.maxauthsize = SHA1_DIGEST_SIZE,
		.base = {
			.cra_name = "authenc(hmac(sha1),cbc(des))",
			.cra_driver_name =
				"authenc(hmac(sha1-eip93),cbc(des-eip93))",
			.cra_priority = MTK_CRA_PRIORITY,
			.cra_flags = CRYPTO_ALG_ASYNC |
					CRYPTO_ALG_KERN_DRIVER_ONLY,
			.cra_blocksize = DES_BLOCK_SIZE,
			.cra_ctxsize = sizeof(struct mtk_crypto_ctx),
			.cra_alignmask = 0,
			.cra_init = mtk_aead_cra_init,
			.cra_exit = mtk_aead_cra_exit,
			.cra_module = THIS_MODULE,
		},
	},
};

struct mtk_alg_template mtk_alg_authenc_hmac_sha224_cbc_des = {
	.type = MTK_ALG_TYPE_AEAD,
	.flags = MTK_HASH_HMAC | MTK_HASH_SHA224 | MTK_MODE_CBC | MTK_ALG_DES,
	.alg.aead = {
		.setkey = mtk_aead_setkey,
		.encrypt = mtk_aead_encrypt,
		.decrypt = mtk_aead_decrypt,
		.ivsize	= DES_BLOCK_SIZE,
		.setauthsize = mtk_aead_setauthsize,
		.maxauthsize = SHA224_DIGEST_SIZE,
		.base = {
			.cra_name = "authenc(hmac(sha224),cbc(des))",
			.cra_driver_name =
				"authenc(hmac(sha224-eip93),cbc(des-eip93))",
			.cra_priority = MTK_CRA_PRIORITY,
			.cra_flags = CRYPTO_ALG_ASYNC |
					CRYPTO_ALG_KERN_DRIVER_ONLY,
			.cra_blocksize = DES_BLOCK_SIZE,
			.cra_ctxsize = sizeof(struct mtk_crypto_ctx),
			.cra_alignmask = 0,
			.cra_init = mtk_aead_cra_init,
			.cra_exit = mtk_aead_cra_exit,
			.cra_module = THIS_MODULE,
		},
	},
};

struct mtk_alg_template mtk_alg_authenc_hmac_sha256_cbc_des = {
	.type = MTK_ALG_TYPE_AEAD,
	.flags = MTK_HASH_HMAC | MTK_HASH_SHA256 | MTK_MODE_CBC | MTK_ALG_DES,
	.alg.aead = {
		.setkey = mtk_aead_setkey,
		.encrypt = mtk_aead_encrypt,
		.decrypt = mtk_aead_decrypt,
		.ivsize	= DES_BLOCK_SIZE,
		.setauthsize = mtk_aead_setauthsize,
		.maxauthsize = SHA256_DIGEST_SIZE,
		.base = {
			.cra_name = "authenc(hmac(sha256),cbc(des))",
			.cra_driver_name =
				"authenc(hmac(sha256-eip93),cbc(des-eip93))",
			.cra_priority = MTK_CRA_PRIORITY,
			.cra_flags = CRYPTO_ALG_ASYNC |
					CRYPTO_ALG_KERN_DRIVER_ONLY,
			.cra_blocksize = DES_BLOCK_SIZE,
			.cra_ctxsize = sizeof(struct mtk_crypto_ctx),
			.cra_alignmask = 0,
			.cra_init = mtk_aead_cra_init,
			.cra_exit = mtk_aead_cra_exit,
			.cra_module = THIS_MODULE,
		},
	},
};

struct mtk_alg_template mtk_alg_authenc_hmac_md5_cbc_des3_ede = {
	.type = MTK_ALG_TYPE_AEAD,
	.flags = MTK_HASH_HMAC | MTK_HASH_MD5 | MTK_MODE_CBC | MTK_ALG_3DES,
	.alg.aead = {
		.setkey = mtk_aead_setkey,
		.encrypt = mtk_aead_encrypt,
		.decrypt = mtk_aead_decrypt,
		.ivsize	= DES3_EDE_BLOCK_SIZE,
		.setauthsize = mtk_aead_setauthsize,
		.maxauthsize = MD5_DIGEST_SIZE,
		.base = {
			.cra_name = "authenc(hmac(md5),cbc(des3_ede))",
			.cra_driver_name =
				"authenc(hmac(md5-eip93),cbc(des3_ede-eip93))",
			.cra_priority = MTK_CRA_PRIORITY,
			.cra_flags = CRYPTO_ALG_ASYNC |
					CRYPTO_ALG_KERN_DRIVER_ONLY,
			.cra_blocksize = DES3_EDE_BLOCK_SIZE,
			.cra_ctxsize = sizeof(struct mtk_crypto_ctx),
			.cra_alignmask = 0x0,
			.cra_init = mtk_aead_cra_init,
			.cra_exit = mtk_aead_cra_exit,
			.cra_module = THIS_MODULE,
		},
	},
};

struct mtk_alg_template mtk_alg_authenc_hmac_sha1_cbc_des3_ede = {
	.type = MTK_ALG_TYPE_AEAD,
	.flags = MTK_HASH_HMAC | MTK_HASH_SHA1 | MTK_MODE_CBC | MTK_ALG_3DES,
	.alg.aead = {
		.setkey = mtk_aead_setkey,
		.encrypt = mtk_aead_encrypt,
		.decrypt = mtk_aead_decrypt,
		.ivsize	= DES3_EDE_BLOCK_SIZE,
		.setauthsize = mtk_aead_setauthsize,
		.maxauthsize = SHA1_DIGEST_SIZE,
		.base = {
			.cra_name = "authenc(hmac(sha1),cbc(des3_ede))",
			.cra_driver_name =
				"authenc(hmac(sha1-eip93),cbc(des3_ede-eip93))",
			.cra_priority = MTK_CRA_PRIORITY,
			.cra_flags = CRYPTO_ALG_ASYNC |
					CRYPTO_ALG_KERN_DRIVER_ONLY,
			.cra_blocksize = DES3_EDE_BLOCK_SIZE,
			.cra_ctxsize = sizeof(struct mtk_crypto_ctx),
			.cra_alignmask = 0x0,
			.cra_init = mtk_aead_cra_init,
			.cra_exit = mtk_aead_cra_exit,
			.cra_module = THIS_MODULE,
		},
	},
};

struct mtk_alg_template mtk_alg_authenc_hmac_sha224_cbc_des3_ede = {
	.type = MTK_ALG_TYPE_AEAD,
	.flags = MTK_HASH_HMAC | MTK_HASH_SHA224 | MTK_MODE_CBC | MTK_ALG_3DES,
	.alg.aead = {
		.setkey = mtk_aead_setkey,
		.encrypt = mtk_aead_encrypt,
		.decrypt = mtk_aead_decrypt,
		.ivsize	= DES3_EDE_BLOCK_SIZE,
		.setauthsize = mtk_aead_setauthsize,
		.maxauthsize = SHA224_DIGEST_SIZE,
		.base = {
			.cra_name = "authenc(hmac(sha224),cbc(des3_ede))",
			.cra_driver_name =
			"authenc(hmac(sha224-eip93),cbc(des3_ede-eip93))",
			.cra_priority = MTK_CRA_PRIORITY,
			.cra_flags = CRYPTO_ALG_ASYNC |
					CRYPTO_ALG_KERN_DRIVER_ONLY,
			.cra_blocksize = DES3_EDE_BLOCK_SIZE,
			.cra_ctxsize = sizeof(struct mtk_crypto_ctx),
			.cra_alignmask = 0x0,
			.cra_init = mtk_aead_cra_init,
			.cra_exit = mtk_aead_cra_exit,
			.cra_module = THIS_MODULE,
		},
	},
};

struct mtk_alg_template mtk_alg_authenc_hmac_sha256_cbc_des3_ede = {
	.type = MTK_ALG_TYPE_AEAD,
	.flags = MTK_HASH_HMAC | MTK_HASH_SHA256 | MTK_MODE_CBC | MTK_ALG_3DES,
	.alg.aead = {
		.setkey = mtk_aead_setkey,
		.encrypt = mtk_aead_encrypt,
		.decrypt = mtk_aead_decrypt,
		.ivsize	= DES3_EDE_BLOCK_SIZE,
		.setauthsize = mtk_aead_setauthsize,
		.maxauthsize = SHA256_DIGEST_SIZE,
		.base = {
			.cra_name = "authenc(hmac(sha256),cbc(des3_ede))",
			.cra_driver_name =
			"authenc(hmac(sha256-eip93),cbc(des3_ede-eip93))",
			.cra_priority = MTK_CRA_PRIORITY,
			.cra_flags = CRYPTO_ALG_ASYNC |
					CRYPTO_ALG_KERN_DRIVER_ONLY,
			.cra_blocksize = DES3_EDE_BLOCK_SIZE,
			.cra_ctxsize = sizeof(struct mtk_crypto_ctx),
			.cra_alignmask = 0x0,
			.cra_init = mtk_aead_cra_init,
			.cra_exit = mtk_aead_cra_exit,
			.cra_module = THIS_MODULE,
		},
	},
};
#endif

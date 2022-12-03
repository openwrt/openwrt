/* SPDX-License-Identifier: GPL-2.0
 *
 * Copyright (C) 2019 - 2021
 *
 * Richard van Schagen <vschagen@icloud.com>
 */

#ifndef _SHA_H_
#define _SHA_H_

#include <crypto/sha.h>

#include "eip93-main.h"

extern struct mtk_alg_template mtk_alg_sha1;
extern struct mtk_alg_template mtk_alg_sha224;
extern struct mtk_alg_template mtk_alg_sha256;
extern struct mtk_alg_template mtk_alg_hmac_sha1;
extern struct mtk_alg_template mtk_alg_hmac_sha224;
extern struct mtk_alg_template mtk_alg_hmac_sha256;

struct mtk_ahash_ctx {
	struct mtk_device	*mtk;
	struct saRecord_s	*sa_in;
	dma_addr_t		sa_base_in;
	u32			init_state[SHA256_DIGEST_SIZE / sizeof(u32)];
	struct crypto_shash	*shash;
};

struct mtk_ahash_reqctx {
	struct mtk_device	*mtk;
	struct saState_s	*saState;
	dma_addr_t		saState_base;
	u32			saState_idx;
	struct saRecord_s	*saRecord;
	dma_addr_t		saRecord_base;
	u32			flags;

	int		nents;
	dma_addr_t	result_dma;

	u64		len;
	u64		processed;

	u8		cache[SHA256_BLOCK_SIZE] __aligned(sizeof(u32));
	dma_addr_t	cache_dma;
	unsigned int	cache_sz;

	u8		cache_next[SHA256_BLOCK_SIZE] __aligned(sizeof(u32));
};

struct mtk_ahash_export_state {
	u64		len;
	u64		processed;
	u32		flags;

	u32		saIDigest[8];
	u32		stateByteCnt[2];
	u8		cache[SHA256_BLOCK_SIZE];
};

int mtk_ahash_handle_result(struct mtk_device *mtk,
				  struct crypto_async_request *async,
				  int err);

int mtk_ahash_send_req(struct mtk_device *mtk,
				struct crypto_async_request *async);
#endif /* _SHA_H_ */

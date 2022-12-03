/* SPDX-License-Identifier: GPL-2.0
 *
 * Copyright (C) 2019 - 2021
 *
 * Richard van Schagen <vschagen@icloud.com>
 */
#ifndef _EIP93_CIPHER_H_
#define _EIP93_CIPHER_H_

#include "eip93-main.h"

struct mtk_crypto_ctx {
	struct mtk_device	*mtk;
	struct saRecord_s	*sa_in;
	dma_addr_t		sa_base_in;
	struct saRecord_s	*sa_out;
	dma_addr_t		sa_base_out;
	u32			saNonce;
	u32			blksize;
	/* AEAD specific */
	u32			authsize;
	u32			assoclen_in;
	u32			assoclen_out;
	bool			in_first;
	bool			out_first;
	struct crypto_shash	*shash;
};

struct mtk_cipher_reqctx {
	u32				flags;
	u32				blksize;
	u32				ivsize;
	u32				textsize;
	u32				assoclen;
	u32				authsize;
	dma_addr_t			saRecord_base;
	struct saState_s		*saState;
	dma_addr_t			saState_base;
	u32				saState_idx;
	struct eip93_descriptor_s	*cdesc;
	struct scatterlist		*sg_src;
	struct scatterlist		*sg_dst;
	int				src_nents;
	int				dst_nents;
	struct saState_s		*saState_ctr;
	dma_addr_t			saState_base_ctr;
	u32				saState_ctr_idx;
};

int check_valid_request(struct mtk_cipher_reqctx *rctx);

void mtk_unmap_dma(struct mtk_device *mtk, struct mtk_cipher_reqctx *rctx,
			struct scatterlist *reqsrc, struct scatterlist *reqdst);

void mtk_skcipher_handle_result(struct crypto_async_request *async, int err);

int mtk_send_req(struct crypto_async_request *async,
			const u8 *reqiv, struct mtk_cipher_reqctx *rctx);

void mtk_handle_result(struct mtk_device *mtk, struct mtk_cipher_reqctx *rctx,
			u8 *reqiv);

#endif /* _EIP93_CIPHER_H_ */

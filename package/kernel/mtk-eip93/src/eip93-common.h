/* SPDX-License-Identifier: GPL-2.0
 *
 * Copyright (C) 2019 - 2021
 *
 * Richard van Schagen <vschagen@icloud.com>
 */

#ifndef _EIP93_COMMON_H_
#define _EIP93_COMMON_H_

#include "eip93-cipher.h"

inline void *mtk_ring_next_wptr(struct mtk_device *mtk,
						struct mtk_desc_ring *ring);

inline void *mtk_ring_next_rptr(struct mtk_device *mtk,
						struct mtk_desc_ring *ring);

inline int mtk_put_descriptor(struct mtk_device *mtk,
					struct eip93_descriptor_s *desc);

inline void *mtk_get_descriptor(struct mtk_device *mtk);

inline int mtk_get_free_saState(struct mtk_device *mtk);

void mtk_set_saRecord(struct saRecord_s *saRecord, const unsigned int keylen,
				const unsigned long flags);

#if IS_ENABLED(CONFIG_CRYPTO_DEV_EIP93_HMAC)
int mtk_authenc_setkey(struct crypto_shash *cshash, struct saRecord_s *sa,
			const u8 *authkey, unsigned int authkeylen);
#endif

#endif /* _EIP93_COMMON_H_ */

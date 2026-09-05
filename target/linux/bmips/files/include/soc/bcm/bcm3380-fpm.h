/* SPDX-License-Identifier: GPL-2.0-only */
#ifndef __SOC_BCM3380_FPM_H
#define __SOC_BCM3380_FPM_H

#include <linux/bits.h>
#include <linux/device.h>
#include <linux/dma-mapping.h>
#include <linux/types.h>

struct bcm3380_fpm_pool;

#define BCM3380_FPM_TOKEN_SIZE_MASK	0x00000fff
#define BCM3380_FPM_TOKEN_INDEX_MASK	0x0003ffff
#define BCM3380_FPM_TOKEN_VALID		BIT(31)

static inline u32 fpm_token_size(u32 token)
{
	return token & BCM3380_FPM_TOKEN_SIZE_MASK;
}

static inline u32 fpm_token_index(u32 token)
{
	return (token >> 12) & BCM3380_FPM_TOKEN_INDEX_MASK;
}

static inline bool fpm_token_valid(u32 token)
{
	return token & BCM3380_FPM_TOKEN_VALID;
}

int fpm_pool_get(struct device *consumer, struct bcm3380_fpm_pool **pool);
void fpm_pool_put(struct bcm3380_fpm_pool *pool);

dma_addr_t fpm_buffer_base_dma(struct bcm3380_fpm_pool *pool);

// See enum FPM_CTRL_FP_BUF_SIZE
u32 fpm_buffer_size_code(struct bcm3380_fpm_pool *pool);

u32 fpm_alloc_free_bus_addr(struct bcm3380_fpm_pool *pool);
u32 fpm_alloc_free_bus_addr_for_size(struct bcm3380_fpm_pool *pool,
				     size_t size);

u32 fpm_borrow_token(struct bcm3380_fpm_pool *pool);
void fpm_return_token(struct bcm3380_fpm_pool *pool, u32 token);
void *fpm_token_to_virt(struct bcm3380_fpm_pool *pool, u32 token);
u32 fpm_tokens_available(struct bcm3380_fpm_pool *pool);

#endif /* __SOC_BCM3380_FPM_H */

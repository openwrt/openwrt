/*
 * drivers/crypto/al_crypto.h
 *
 * Annapurna Labs Crypto driver - header file
 *
 * Copyright (C) 2012 Annapurna Labs Ltd.
 *
 * Chained scatter/gather lists handling based on caam driver.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
#ifndef __AL_CRYPTO_H__
#define __AL_CRYPTO_H__

#include <linux/init.h>
#include <linux/cache.h>
#include <linux/circ_buf.h>
#include <linux/pci_ids.h>
#include <linux/semaphore.h>
#include <linux/pci.h>
#include <linux/interrupt.h>
#include <linux/scatterlist.h>
#include <linux/crypto.h>
#include <crypto/aes.h>
#include <crypto/des.h>
#include <crypto/algapi.h>

#include <mach/al_hal_ssm_crypto.h>
#include <mach/al_hal_ssm_crc_memcpy.h>

#define AL_CRYPTO_VERSION  "0.01"

#ifndef CONFIG_ALPINE_VP_WA
#define AL_CRYPTO_TX_CDESC_SIZE			8
#define AL_CRYPTO_RX_CDESC_SIZE			8
#else
/* Currently in VP it is always 16 bytes */
#define AL_CRYPTO_TX_CDESC_SIZE			16
#define AL_CRYPTO_RX_CDESC_SIZE			16
#endif

#define AL_CRYPTO_DMA_MAX_CHANNELS		4

/* 4 interrupts for the 4 queues and 1 for group D */
#define AL_CRYPTO_MSIX_INTERRUPTS		AL_CRYPTO_DMA_MAX_CHANNELS + 1

#define AL_CRYPTO_SW_RING_MIN_ORDER		4
#define AL_CRYPTO_SW_RING_MAX_ORDER		16

/*
 * tx: 31(supported by HW) - 1(metadata) - 1(sa_in) -
 *			1(enc_iv_in|auth_iv_in) - 1(auth_sign_in) = 27
 * rx: 31(supported by HW) - 1(sa_out) - 1(enc_iv_out|auth_iv_out) -
 *			1(next_enc_iv_out) - 1(auth_sign_out) = 27
 */
#define AL_CRYPTO_OP_MAX_BUFS			27
#define AL_CRYPTO_HASH_HMAC_IPAD		0x36
#define AL_CRYPTO_HASH_HMAC_OPAD		0x5c

#define AL_CRYPTO_MAX_IV_LENGTH	16 /* max of AES_BLOCK_SIZE,
							DES3_EDE_BLOCK_SIZE */

#ifdef CONFIG_CRYPTO_DEV_AL_CRYPTO_STATS
#define AL_CRYPTO_STATS_INC(var, incval)	(var) += (incval)
#define AL_CRYPTO_STATS_DEC(var, decval)	(var) -= (decval)
#define AL_CRYPTO_STATS_SET(var, val)		(var) = (val)
#define AL_CRYPTO_STATS_LOCK(lock)		\
	spin_lock_bh(lock)
#define AL_CRYPTO_STATS_UNLOCK(lock)	\
	spin_unlock_bh(lock)
#define AL_CRYPTO_STATS_INIT_LOCK(lock)	\
	spin_lock_init(lock)
#else
#define AL_CRYPTO_STATS_INC(var, incval)
#define AL_CRYPTO_STATS_DEC(var, decval)
#define AL_CRYPTO_STATS_SET(var, val)
#define AL_CRYPTO_STATS_LOCK(lock)
#define AL_CRYPTO_STATS_LOCK(lock)
#define AL_CRYPTO_STATS_UNLOCK(lock)
#define AL_CRYPTO_STATS_INIT_LOCK(lock)
#endif

#define AL_CRYPTO_IRQNAME_SIZE			40

#define AL_CRYPTO_INT_MODER_RES			1

#define CHKSUM_BLOCK_SIZE       1
#define CHKSUM_DIGEST_SIZE      4

#define MAX_CACHE_ENTRIES_PER_CHANNEL 	CACHED_SAD_SIZE

enum al_crypto_req_type {
	AL_CRYPTO_REQ_SKCIPHER,
	AL_CRYPTO_REQ_AEAD,
	AL_CRYPTO_REQ_AHASH,
	AL_CRYPTO_REQ_CRC,
};

/* software descriptor structure
 */
struct al_crypto_sw_desc {
	union {
		struct al_crypto_transaction	hal_xaction;
		struct al_crc_transaction	hal_crc_xaction;
	};

	struct al_buf	src_bufs[AL_SSM_MAX_SRC_DESCS];
	struct al_buf	dst_bufs[AL_SSM_MAX_DST_DESCS];

	void	*req;
	int	req_type;
	int	src_nents;
	int	dst_nents;
};

/**
 * cache entry in lru list
 */
struct al_crypto_cache_lru_entry {
	struct list_head		list;
	struct al_crypto_cache_state	*ctx;
	u32				cache_idx;
};

#ifdef CONFIG_CRYPTO_DEV_AL_CRYPTO_STATS
/**
 * struct al_crypto_chan_stats_gen - Crypto DMA channel statistics - general
 * @skcipher_tfms - active skcipher tfms
 * @skcipher_tfms - active aead tfms
 * @skcipher_tfms - active ahash tfms
 */
struct al_crypto_chan_stats_gen {
	uint64_t skcipher_tfms;
	uint64_t aead_tfms;
	uint64_t ahash_tfms;
	uint64_t crc_tfms;
};

/**
 * struct al_crypto_chan_stats_prep - Crypto DMA channel statistics -
 * 								preparation
 * @skcipher_encrypt_reqs - skcipher encrypt requests
 * @skcipher_encrypt_bytes - skcipher encrypted bytes
 * @skcipher_decrypt_reqs - skcipher decrypt requests
 * @skcipher_decrypt_bytes - skcipher decrypted bytes
 * @aead_encrypt_hash_reqs - aead combined encrypt+hash requests
 * @aead_encrypt_bytes - aead encrypted bytes
 * @aead_hash_bytes - aead hashed bytes
 * @aead_decrypt_validate_reqs - aead combined decrypt+validate requests
 * @aead_decrypt_bytes - aead decrypted bytes
 * @aead_validate_bytes - aead validate bytes
 * @ahash_reqs - ahash requests
 * @ahash_bytes - ahash hashed bytes
 * @cache_misses - SA cache misses
 * @skcipher_reqs_le512 - skcipher requests up to 512 bytes
 * @skcipher_reqs_512_2048 - skcipher requests between 512 and 2048 bytes
 * @skcipher_reqs_2048_4096 - skcipher requests between 2048 and 4096 bytes
 * @skcipher_reqs_gt4096 - skcipher requests greater than 4096 bytes
 * @aead_reqs_le512 - aead requests up to 512 bytes
 * @aead_reqs_512_2048 - aead requests between 512 and 2048 bytes
 * @aead_reqs_2048_4096 - aead requests between 2048 and 4096 bytes
 * @aead_reqs_gt4096 - aead requests greater than 4096 bytes
 * @ahash_reqs_le512 - ahash requests up to 512 bytes
 * @ahash_reqs_512_2048 - ahash requests between 512 and 2048 bytes
 * @ahash_reqs_2048_4096 - ahash requests between 2048 and 4096 bytes
 * @ahash_reqs_gt4096 - ahash requests greater than 4096 bytes
 */
struct al_crypto_chan_stats_prep {
	uint64_t skcipher_encrypt_reqs;
	uint64_t skcipher_encrypt_bytes;
	uint64_t skcipher_decrypt_reqs;
	uint64_t skcipher_decrypt_bytes;
	uint64_t aead_encrypt_hash_reqs;
	uint64_t aead_encrypt_bytes;
	uint64_t aead_hash_bytes;
	uint64_t aead_decrypt_validate_reqs;
	uint64_t aead_decrypt_bytes;
	uint64_t aead_validate_bytes;
	uint64_t ahash_reqs;
	uint64_t ahash_bytes;
	uint64_t crc_reqs;
	uint64_t crc_bytes;
	uint64_t cache_misses;
	uint64_t skcipher_reqs_le512;
	uint64_t skcipher_reqs_512_2048;
	uint64_t skcipher_reqs_2048_4096;
	uint64_t skcipher_reqs_gt4096;
	uint64_t aead_reqs_le512;
	uint64_t aead_reqs_512_2048;
	uint64_t aead_reqs_2048_4096;
	uint64_t aead_reqs_gt4096;
	uint64_t ahash_reqs_le512;
	uint64_t ahash_reqs_512_2048;
	uint64_t ahash_reqs_2048_4096;
	uint64_t ahash_reqs_gt4096;
	uint64_t crc_reqs_le512;
	uint64_t crc_reqs_512_2048;
	uint64_t crc_reqs_2048_4096;
	uint64_t crc_reqs_gt4096;
};

/**
 * struct al_crypto_chan_stats_comp - Crypto DMA channel statistics -
 * 								completion
 * @redundant_int_cnt - redundant interrupts (interrupts without completions)
 */
struct al_crypto_chan_stats_comp {
	uint64_t redundant_int_cnt;
	uint64_t max_active_descs;
};
#endif

/* internal representation of a DMA channel
 */
struct al_crypto_chan {
	struct al_ssm_dma	*hal_crypto;

	int	idx;
	enum al_ssm_q_type type;
	cpumask_t      affinity_hint_mask;

	/* Tx UDMA hw ring */
	int tx_descs_num;	/* number of descriptors in Tx queue */
	void *tx_dma_desc_virt; /* Tx descriptors ring */
	dma_addr_t tx_dma_desc;

	/* Rx UDMA hw ring */
	int rx_descs_num;	/* number of descriptors in Rx queue */
	void *rx_dma_desc_virt; /* Rx descriptors ring */
	dma_addr_t rx_dma_desc;
	void *rx_dma_cdesc_virt; /* Rx completion descriptors ring */
	dma_addr_t rx_dma_cdesc;

	/* SW descriptors ring */
	u16 alloc_order;
	struct al_crypto_sw_desc **sw_ring;
#ifdef CONFIG_CRYPTO_DEV_AL_CRYPTO_STATS
	struct al_crypto_chan_stats_gen stats_gen;
	spinlock_t stats_gen_lock; /* locked during access of general stats */
#endif

	/* Frequently accessed prep */
	spinlock_t prep_lock ____cacheline_aligned;	/* locked during
						xaction preparation and
						cache management changes */
	u16 head;
	int sw_desc_num_locked; /* num of sw descriptors locked during xaction
						preparation */
	u32 tx_desc_produced;	/* num of hw descriptors generated by HAL */
	struct crypto_queue sw_queue; /* sw queue for backlog */
#ifdef CONFIG_CRYPTO_DEV_AL_CRYPTO_STATS
	struct al_crypto_chan_stats_prep stats_prep;
#endif

	/* LRU cache management */
	int cache_entries_num;
	struct list_head cache_lru_list;
	int cache_lru_count;
	struct al_crypto_cache_lru_entry cache_lru_entries[
					MAX_CACHE_ENTRIES_PER_CHANNEL];

	/* Frequently accessed cleanup */
	spinlock_t cleanup_lock ____cacheline_aligned_in_smp; /* locked during
								cleanup */
	u16 tail;
#ifdef CONFIG_CRYPTO_DEV_AL_CRYPTO_STATS
	struct al_crypto_chan_stats_comp stats_comp;
#endif

	struct al_crypto_device *device;
	struct tasklet_struct cleanup_task;
	struct kobject kobj;
};

#define to_dev(al_crypto_chan) (&(al_crypto_chan)->device->pdev->dev)

/* internal structure for AL Crypto IRQ
 */
struct al_crypto_irq {
	char name[AL_CRYPTO_IRQNAME_SIZE];
};

/* internal structure for AL Crypto device
 */
struct al_crypto_device {
	struct pci_dev		*pdev;

	struct al_ssm_dma_params	ssm_dma_params;
	void __iomem	*udma_regs_base;
	void __iomem	*crypto_regs_base;

	struct al_ssm_dma	hal_crypto;

	struct msix_entry	msix_entries[AL_CRYPTO_MSIX_INTERRUPTS];
	struct al_crypto_irq	irq_tbl[AL_CRYPTO_MSIX_INTERRUPTS];
	struct al_crypto_chan	*channels[AL_CRYPTO_DMA_MAX_CHANNELS];
	int			num_channels;
	int			max_channels;
	int			crc_channels;
	struct kset 		*channels_kset;
	struct tasklet_struct	cleanup_task;
	int			int_moderation;
	int			num_irq_used;

	struct kmem_cache	*cache;	   /* descriptors cache */
	atomic_t		tfm_count; /* used to allocate the dma
						channel for current tfm */
	atomic_t		crc_tfm_count; /* used to allocate the dma
						channel for current crc tfm */
	struct list_head skcipher_list;	/* list of registered skcipher algorithms */
	struct list_head aead_list;	/* list of registered aead algorithms */
	struct list_head hash_list;	/* list of registered hash algorithms */
	struct list_head crc_list;	/* list of registered crc/csum algorithms */
};

struct al_crypto_cache_state {
	bool			cached;
	int			idx;
};

/* context structure
 */
struct al_crypto_ctx {
	struct al_crypto_chan	*chan;
	struct al_crypto_cache_state	cache_state;
	struct al_crypto_sa	sa;
	struct al_crypto_hw_sa	*hw_sa;
	dma_addr_t		hw_sa_dma_addr;
	struct crypto_shash	*sw_hash;	/* for HMAC key hashing */
	u8 			*iv;
	dma_addr_t		iv_dma_addr;
	u8			*hmac_pads;
	struct crypto_aes_ctx	aes_key;
};

/* DMA ring management inline functions */
static inline u16 al_crypto_ring_size(struct al_crypto_chan *chan)
{
	return 1 << chan->alloc_order;
}

/* count of transactions in flight with the engine */
static inline u16 al_crypto_ring_active(struct al_crypto_chan *chan)
{
	return CIRC_CNT(chan->head, chan->tail, al_crypto_ring_size(chan));
}
static inline u16 al_crypto_ring_space(struct al_crypto_chan *chan)
{
	return CIRC_SPACE(chan->head, chan->tail, al_crypto_ring_size(chan));
}

static inline struct al_crypto_sw_desc  *
al_crypto_get_ring_ent(struct al_crypto_chan *chan, u16 idx)
{
	return chan->sw_ring[idx & (al_crypto_ring_size(chan) - 1)];
}

int al_crypto_get_sw_desc(struct al_crypto_chan *chan, int num);

void al_crypto_tx_submit(struct al_crypto_chan *chan);

#ifdef DEBUG
#define set_desc_id(desc, i) ((desc)->id = (i))
#define desc_id(desc) ((desc)->id)
#else
#define set_desc_id(desc, i)
#define desc_id(desc) (0)
#endif

static inline void sg_map_to_xaction_buffers(struct scatterlist *sg_in,
					     struct al_buf* bufs,
					     unsigned int length,
					     int *buf_idx)
{
	struct scatterlist *sg = sg_in, *next_sg;

	if (!length)
		return;

	bufs[*buf_idx].addr = sg_dma_address(sg);
	bufs[*buf_idx].len = 0;

	while (length > sg_dma_len(sg)) {
		bufs[*buf_idx].len += sg_dma_len(sg);

		length -= sg_dma_len(sg);
		next_sg = sg_next(sg);

		BUG_ON(!next_sg);

		if (sg_dma_address(sg) + sg_dma_len(sg) != sg_dma_address(next_sg)) {
			(*buf_idx)++;
			bufs[*buf_idx].addr = sg_dma_address(next_sg);
			bufs[*buf_idx].len = 0;
		}

		sg = next_sg;
	}

	/* last sg */
	bufs[*buf_idx].len += length;
	(*buf_idx)++;
}

/* SA cache management using LRU */
void al_crypto_cache_update_lru(struct al_crypto_chan *chan,
				struct al_crypto_cache_state *ctx);

u32 al_crypto_cache_replace_lru(struct al_crypto_chan *chan,
				struct al_crypto_cache_state *ctx,
				struct al_crypto_cache_state **old_ctx);

void al_crypto_cache_remove_lru(struct al_crypto_chan *chan,
				struct al_crypto_cache_state *ctx);

/* Core APIs */
int al_crypto_core_init(
	struct al_crypto_device	*device,
	void __iomem		*iobase_udma,
	void __iomem		*iobase_app);

int al_crypto_core_terminate(
	struct al_crypto_device	*device);

int al_crypto_cleanup_fn(
	struct al_crypto_chan	*chan,
	int			from_tasklet);

void al_crypto_set_int_moderation(
	struct al_crypto_device	*device,
	int			usec);

int al_crypto_get_int_moderation(
	struct al_crypto_device *device);

int al_crypto_keylen_to_sa_aes_ksize(unsigned int keylen,
				     enum al_crypto_sa_aes_ksize *ksize);

int al_crypto_sa_aes_ksize_to_keylen(enum al_crypto_sa_aes_ksize ksize,
				     unsigned int *keylen);

void al_crypto_hexdump_sgl(const char *level, struct scatterlist *sgl,
			   const char *name, off_t skip, int len, gfp_t gfp_flags);

/* aead related functions */
void al_crypto_cleanup_single_aead(
	struct al_crypto_chan		*chan,
	struct al_crypto_sw_desc	*desc,
	u32				comp_status);

/* ahash related functions */
void al_crypto_cleanup_single_ahash(
	struct al_crypto_chan		*chan,
	struct al_crypto_sw_desc	*desc,
	u32				comp_status);

/* crc related functions */
void al_crypto_cleanup_single_crc(
	struct al_crypto_chan		*chan,
	struct al_crypto_sw_desc	*desc,
	u32				comp_status);

int hmac_setkey(struct al_crypto_ctx *ctx,
	const u8			*key,
	unsigned int			keylen,
	unsigned int			sw_hash_interm_offset,
	unsigned int			sw_hash_interm_size);

/* sysfs */
void al_crypto_free_channel(struct al_crypto_chan *chan);

int al_crypto_sysfs_init(struct al_crypto_device *device);

void al_crypto_sysfs_terminate(struct al_crypto_device *device);

/* al_crypto_skcipher APIs */
int al_crypto_skcipher_init(struct al_crypto_device *device);

void al_crypto_skcipher_terminate(struct al_crypto_device *device);

void al_crypto_skcipher_cleanup_single(
	struct al_crypto_chan		*chan,
	struct al_crypto_sw_desc	*desc,
	u32				comp_status);

int al_crypto_skcipher_process_queue(struct al_crypto_chan *chan);

/* al_crypto_aead APIs */
int al_crypto_aead_init(struct al_crypto_device *device);

void al_crypto_aead_terminate(struct al_crypto_device *device);

void al_crypto_aead_cleanup_single(
	struct al_crypto_chan		*chan,
	struct al_crypto_sw_desc	*desc,
	u32				comp_status);

/* al_crypto_hash APIs */
int al_crypto_hash_init(struct al_crypto_device *device);

void al_crypto_hash_terminate(struct al_crypto_device *device);

/* al_crypto_crc APIs */
int al_crypto_crc_init(struct al_crypto_device *device);

void al_crypto_crc_terminate(struct al_crypto_device *device);

#endif	/* __AL_CRYPTO_H__ */

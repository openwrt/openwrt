/*
 * Annapurna Labs DMA Linux driver - Operation completion cleanup
 * Copyright(c) 2011 Annapurna Labs.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU General Public License,
 * version 2, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin St - Fifth Floor, Boston, MA 02110-1301 USA.
 *
 * The full GNU General Public License is included in this distribution in
 * the file called "COPYING".
 *
 */

#include <mach/al_hal_udma_iofic.h>
#include "../dmaengine.h"
#include "al_dma.h"

#define smp_read_barrier_depends() do {} while(0)

static void al_dma_cleanup_single(
	struct al_dma_chan	*chan,
	struct al_dma_sw_desc	*desc,
	uint32_t		comp_status);

static inline void al_dma_cleanup_single_memcpy(
	struct al_dma_chan	*chan,
	struct al_dma_sw_desc	*desc);

static inline void al_dma_cleanup_single_xor(
	struct al_dma_chan	*chan,
	struct al_dma_sw_desc	*desc);

static inline void al_dma_cleanup_single_pq_val(
	struct al_dma_chan	*chan,
	struct al_dma_sw_desc	*desc,
	uint32_t		comp_status);

static inline void al_dma_cleanup_single_xor_val(
	struct al_dma_chan	*chan,
	struct al_dma_sw_desc	*desc,
	uint32_t		comp_status);


static void al_dma_cleanup_unmap(
	struct al_dma_chan	*chan,
	struct al_dma_sw_desc	*desc);

/******************************************************************************
 *****************************************************************************/
int al_dma_cleanup_fn(
	struct al_dma_chan	*chan,
	int			from_tasklet)
{
	struct al_dma_sw_desc *desc;
	uint32_t comp_status;
	u16 active;
	int idx, i;
	uint32_t rc;

	dev_dbg(to_dev(chan), "%s: head: %#x tail: %#x\n",
		__func__, chan->head, chan->tail);

	spin_lock_bh(&chan->cleanup_lock);

	idx = chan->tail;

	active = al_dma_ring_active(chan);
	for (i = 0; i < active; i++) {
		rc = al_raid_dma_completion(chan->hal_raid, chan->idx,
					    &comp_status);

		/* if no completed transaction found -> exit */
		if (rc == 0) {
			dev_dbg(to_dev(chan), "%s: No completion\n",
			__func__);

			break;
		}

		dev_dbg(
			to_dev(chan),
			"%s: completion status: %u\n",
			__func__,
			comp_status);

		/* This will instruct the CPU to make sure the index is up to
		   date before reading the new item */
		smp_read_barrier_depends();

		desc = al_dma_get_ring_ent(chan, idx + i);

		al_dma_cleanup_single(chan, desc, comp_status);
	}

	/* This will make sure the CPU has finished reading the item
	   before it writes the new tail pointer, which will erase the item */
	smp_mb();

	chan->tail = idx + i;

	AL_DMA_STATS_INC(chan->stats_comp.matching_cpu,
			i * ((chan->idx == smp_processor_id())));
	AL_DMA_STATS_INC(chan->stats_comp.mismatching_cpu,
			i * (!(chan->idx == smp_processor_id())));

	/* Keep track of redundant interrupts - interrupts that doesn't
	   yield completions */
	if (unlikely(from_tasklet && (!i))) {
		AL_DMA_STATS_INC(chan->stats_comp.redundant_int_cnt, 1);
	}

	spin_unlock_bh(&chan->cleanup_lock);

	return i;
};

/******************************************************************************
 *****************************************************************************/
static inline void al_dma_cleanup_single(
	struct al_dma_chan	*chan,
	struct al_dma_sw_desc	*desc,
	uint32_t		comp_status)
{
	struct dma_async_tx_descriptor *tx;

	if (desc->last_is_memcpy) {
		desc->last_is_memcpy = 0;
		al_dma_cleanup_single_memcpy(chan, desc);
	} else if (desc->last_is_xor) {
		desc->last_is_xor = 0;
		al_dma_cleanup_single_xor(chan, desc);
	} else if (desc->last_is_pq_val) {
		desc->last_is_pq_val = 0;
		al_dma_cleanup_single_pq_val(chan, desc, comp_status);
	} else if (desc->last_is_xor_val) {
		desc->last_is_xor_val = 0;
		al_dma_cleanup_single_xor_val(chan, desc, comp_status);
	}

	tx = &desc->txd;
	if (tx->cookie) {
		dma_cookie_complete(tx);
		al_dma_cleanup_unmap(chan, desc);
		if (tx->callback) {
			tx->callback(tx->callback_param);
			tx->callback = NULL;
		}
	}
};

/******************************************************************************
 *****************************************************************************/
static inline void al_dma_cleanup_unmap(
	struct al_dma_chan	*chan,
	struct al_dma_sw_desc	*desc)
{
	struct pci_dev *pdev = chan->device->pdev;
	int i;

	for (i = 0; i < desc->umap_ent_cnt; i++) {
		struct al_dma_unmap_info_ent *ent = &desc->unmap_info[i];

		switch (ent->type) {
		case AL_UNMAP_SINGLE:
			dma_unmap_single(
				&pdev->dev, ent->handle, ent->size, ent->dir);
			break;
		case AL_UNMAP_PAGE:
			dma_unmap_page(&pdev->dev, ent->handle, ent->size, ent->dir);
			break;
		}
	}
}

/******************************************************************************
 *****************************************************************************/
static inline void al_dma_cleanup_single_memcpy(
	struct al_dma_chan	*chan,
	struct al_dma_sw_desc	*desc)
{
#ifdef AL_DMA_MEMCPY_VALIDATION
	if (memcmp(desc->memcpy_dest, desc->memcpy_src, desc->memcpy_len)) {
		dev_err(
			to_dev(chan),
			"%s: memcpy (%p, %p, %d) failed!\n",
			__func__,
			desc->memcpy_dest,
			desc->memcpy_src,
			desc->memcpy_len);
	} else
		dev_dbg(
			to_dev(chan),
			"%s: memcpy (%p, %p, %d) ok!\n",
			__func__,
			desc->memcpy_dest,
			desc->memcpy_src,
			desc->memcpy_len);
#endif
}

/******************************************************************************
 *****************************************************************************/
static inline void al_dma_cleanup_single_pq_val(
	struct al_dma_chan	*chan,
	struct al_dma_sw_desc	*desc,
	uint32_t		comp_status)
{
	if (unlikely(comp_status & AL_RAID_P_VAL_ERROR)) {
		dev_dbg(
			to_dev(chan),
			"%s: pq_val failed P!\n",
			__func__);

		(*desc->pq_val_res) |= SUM_CHECK_P_RESULT;
	}

	if (unlikely(comp_status & AL_RAID_Q_VAL_ERROR)) {
		dev_dbg(
			to_dev(chan),
			"%s: pq_val failed Q!\n",
			__func__);

		(*desc->pq_val_res) |= SUM_CHECK_Q_RESULT;
	}
}

/******************************************************************************
 *****************************************************************************/
static inline void al_dma_cleanup_single_xor(
	struct al_dma_chan	*chan,
	struct al_dma_sw_desc	*desc)
{
#ifdef AL_DMA_XOR_VALIDATION
	int src_index;
	int dest_index;

	for (
		dest_index = 0;
		dest_index < desc->xor_len;
		dest_index++) {
		uint8_t byte_val =
			((uint8_t *)desc->xor_dest)[dest_index];

		for (
			src_index = 0;
			src_index < desc->xor_src_cnt;
			src_index++) {
			byte_val ^=
				((uint8_t *)desc->xor_src[
					src_index])[dest_index];
		}

		if (byte_val) {
			if (desc->xoddr_src_cnt != 3)
				dev_err(
					to_dev(chan),
					"%s: xor(%p, [%d srcs, %d) failed!\n",
					__func__,
					desc->xor_dest,
					desc->xor_src_cnt,
					desc->xor_len);
			else
				dev_err(
					to_dev(chan),
					"%s: xor(%p, %p, %p, %p, %d) failed!\n",
					__func__,
					desc->xor_dest,
					desc->xor_src[0],
					desc->xor_src[1],
					desc->xor_src[2],
					desc->xor_len);

			break;
		}
	}

	if (dest_index == desc->xor_len) {
		dev_dbg(
			to_dev(chan),
			"%s: xor (%p, %d, %d) ok!\n",
			__func__,
			desc->xor_dest,
			desc->xor_len,
			desc->xor_src_cnt);
	}
#endif
}

/******************************************************************************
 *****************************************************************************/
static inline void al_dma_cleanup_single_xor_val(
	struct al_dma_chan	*chan,
	struct al_dma_sw_desc	*desc,
	uint32_t		comp_status)
{
	if (unlikely(comp_status & AL_RAID_P_VAL_ERROR)) {
		dev_dbg(
			to_dev(chan),
			"%s: xor_val failed P!\n",
			__func__);

		(*desc->xor_val_res) |= SUM_CHECK_P_RESULT;
	}
}


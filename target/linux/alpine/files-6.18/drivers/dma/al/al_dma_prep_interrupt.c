/*
 * Annapurna Labs DMA Linux driver - Interrupt preparation
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

#include "al_dma.h"
#include "al_dma_prep.h"

/******************************************************************************
 *****************************************************************************/
struct dma_async_tx_descriptor *
al_dma_prep_interrupt_lock(struct dma_chan *c, unsigned long flags) {
  struct al_dma_chan *chan = to_al_dma_chan(c);
  struct dma_async_tx_descriptor *txd = NULL;
  int idx;
  int32_t rc;
  struct al_dma_sw_desc *desc;
  struct al_raid_transaction *xaction;

  dev_dbg(chan->device->common.dev, "%s: chan->idx = %d, flags = %08x\n",
          __func__, chan->idx, (unsigned int)flags);

  if (likely(al_dma_get_sw_desc_lock(chan, 1) == 0))
    idx = chan->head;
  else {
    dev_dbg(chan->device->common.dev, "%s: al_dma_get_sw_desc_lock failed!\n",
            __func__);

    return NULL;
  }

  chan->sw_desc_num_locked = 1;

  desc = al_dma_get_ring_ent(chan, idx);

  desc->umap_ent_cnt = 0;

  txd = &desc->txd;

  desc->txd.flags = flags;

  /* prepare hal transaction */
  xaction = &desc->hal_xaction;
  memset(xaction, 0, sizeof(struct al_raid_transaction));
  xaction->op = AL_RAID_OP_NOP;
  xaction->flags |= AL_SSM_INTERRUPT;
  if (flags & DMA_PREP_FENCE)
    xaction->flags |= AL_SSM_BARRIER;

  if (flags & (~(DMA_PREP_INTERRUPT | DMA_PREP_FENCE)))
    dev_err(chan->device->common.dev, "%s: flags = %08x\n", __func__,
            (unsigned int)flags);

  xaction->num_of_srcs = 0;
  xaction->total_src_bufs = 0;

  xaction->num_of_dsts = 0;
  xaction->total_dst_bufs = 0;

  dev_dbg(chan->device->common.dev, "%s: xaction->flags = %08x\n", __func__,
          xaction->flags);

  /* send raid transaction to engine */
  rc = al_raid_dma_prepare(chan->hal_raid, chan->idx, &desc->hal_xaction);
  if (unlikely(rc)) {
    dev_err(chan->device->common.dev, "%s: al_raid_dma_prepare failed!\n",
            __func__);
    spin_unlock_bh(&chan->prep_lock);
    return NULL;
  }

  chan->tx_desc_produced += desc->hal_xaction.tx_descs_count;

  AL_DMA_STATS_UPDATE(chan, chan->stats_prep.int_num, 1,
                      chan->stats_prep.int_num, /* dummy */
                      0);

  al_dma_tx_submit_sw_cond_unlock(chan, txd);

  return txd;
}

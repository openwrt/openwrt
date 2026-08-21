/*
 * Annapurna Labs DMA Linux driver - PQ preparation
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

#define MAX_SIZE AL_DMA_MAX_SIZE_PQ

/******************************************************************************
 *****************************************************************************/
struct dma_async_tx_descriptor *
al_dma_prep_pq_lock(struct dma_chan *c, dma_addr_t *dst, dma_addr_t *src,
                    unsigned int src_cnt, const unsigned char *scf, size_t len,
                    unsigned long flags) {
  struct al_dma_chan *chan = to_al_dma_chan(c);
  struct dma_async_tx_descriptor *txd = NULL;
  int idx;
  int32_t rc;
  int sw_desc_num_req = ALIGN(len, MAX_SIZE) / MAX_SIZE;
  int sw_desc_num_req_orig = sw_desc_num_req;
  size_t len_orig = len;
  dma_addr_t src_off = 0;
  dma_addr_t dst_off = 0;
  int i;

  dev_dbg(chan->device->common.dev,
          "%s: dest_p = %X, dest_q = %X, src = %X, cnt = %d, len = %d,"
          " flags = %08x\n",
          __func__, (unsigned int)dst[0], (unsigned int)dst[1],
          (unsigned int)src, src_cnt, len, (unsigned int)flags);

  if (unlikely(src_cnt > AL_DMA_MAX_XOR)) {
    BUG();
    return NULL;
  }

  if (unlikely((flags & DMA_PREP_PQ_DISABLE_P) &&
               (flags & DMA_PREP_PQ_DISABLE_Q))) {
    BUG();
    return NULL;
  }

  if (likely(al_dma_get_sw_desc_lock(chan, sw_desc_num_req) == 0))
    idx = chan->head;
  else {
    dev_dbg(chan->device->common.dev, "%s: al_dma_get_sw_desc_lock failed!\n",
            __func__);

    return NULL;
  }

  chan->sw_desc_num_locked = sw_desc_num_req;

  if (unlikely(sw_desc_num_req > 1))
    dev_dbg(chan->device->common.dev,
            "%s: splitting transaction to %d sub-transactions\n\n", __func__,
            sw_desc_num_req);

  while (sw_desc_num_req) {
    int cur_len = (len > MAX_SIZE) ? MAX_SIZE : len;

    struct al_dma_sw_desc *desc = al_dma_get_ring_ent(chan, idx);

    struct al_raid_transaction *xaction;

    if (1 == sw_desc_num_req)
      txd = &desc->txd;

    if (1 == sw_desc_num_req) {
      int umap_ent_cnt = 0;
      dma_descriptor_unmap(txd);

      desc->umap_ent_cnt = umap_ent_cnt;
    } else
      desc->umap_ent_cnt = 0;

    desc->txd.flags = flags;
    desc->len = cur_len;
    /* prepare hal transaction */
    xaction = &desc->hal_xaction;
    memset(xaction, 0, sizeof(struct al_raid_transaction));
    xaction->op = AL_RAID_OP_PQ_CALC;
    if (flags & DMA_PREP_PQ_DISABLE_P)
      xaction->op = AL_RAID_OP_Q_CALC;
    if (flags & DMA_PREP_PQ_DISABLE_Q)
      xaction->op = AL_RAID_OP_P_CALC;
    if ((flags & DMA_PREP_INTERRUPT) && (1 == sw_desc_num_req))
      xaction->flags |= AL_SSM_INTERRUPT;
    if ((flags & DMA_PREP_FENCE) && (1 == sw_desc_num_req))
      xaction->flags |= AL_SSM_BARRIER;

    /* use bufs[0] and block[i] for source buffers/blocks */
    for (i = 0; i < src_cnt; i++) {
      desc->bufs[i].addr = src[i] + src_off;
      desc->bufs[i].len = cur_len;
      desc->blocks[i].bufs = &desc->bufs[i];
      desc->blocks[i].num = 1;
    }

    xaction->srcs_blocks = &desc->blocks[0];
    xaction->num_of_srcs = src_cnt;
    xaction->total_src_bufs = src_cnt;

    /* use bufs[1] and block[1] for destination buffers/blocks */
    if (flags & DMA_PREP_PQ_DISABLE_P) {
      desc->bufs[src_cnt].addr = dst[1] + dst_off;
      desc->bufs[src_cnt].len = cur_len;
      desc->blocks[src_cnt].bufs = &desc->bufs[src_cnt];
      desc->blocks[src_cnt].num = 1;

      xaction->num_of_dsts = 1;
      xaction->total_dst_bufs = 1;
    } else if (flags & DMA_PREP_PQ_DISABLE_Q) {
      desc->bufs[src_cnt].addr = dst[0] + dst_off;
      desc->bufs[src_cnt].len = cur_len;
      desc->blocks[src_cnt].bufs = &desc->bufs[src_cnt];
      desc->blocks[src_cnt].num = 1;

      xaction->num_of_dsts = 1;
      xaction->total_dst_bufs = 1;
    } else {
      desc->bufs[src_cnt].addr = dst[0] + dst_off;
      desc->bufs[src_cnt].len = cur_len;
      desc->blocks[src_cnt].bufs = &desc->bufs[src_cnt];
      desc->blocks[src_cnt].num = 1;

      desc->bufs[src_cnt + 1].addr = dst[1] + dst_off;
      desc->bufs[src_cnt + 1].len = cur_len;
      desc->blocks[src_cnt + 1].bufs = &desc->bufs[src_cnt + 1];
      desc->blocks[src_cnt + 1].num = 1;

      xaction->num_of_dsts = 2;
      xaction->total_dst_bufs = 2;
    }

    xaction->dsts_blocks = &desc->blocks[src_cnt];

    xaction->coefs = (uint8_t *)scf;

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
    BUG_ON(desc->hal_xaction.tx_descs_count > AL_DMA_MAX_XOR);

    idx++;
    sw_desc_num_req--;
    len -= MAX_SIZE;
    src_off += MAX_SIZE;
    dst_off += MAX_SIZE;
  }

  AL_DMA_STATS_UPDATE(chan, chan->stats_prep.pq_num, sw_desc_num_req_orig,
                      chan->stats_prep.pq_size, len_orig);

  al_dma_tx_submit_sw_cond_unlock(chan, txd);

  return txd;
}

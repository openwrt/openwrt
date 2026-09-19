/*
 * drivers/crypto/al/al_dma_module_params.c
 *
 * Annapurna Labs DMA driver - module params
 *
 * Copyright (C) 2013 Annapurna Labs Ltd.
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
#include "al_dma_module_params.h"
#include "al_dma.h"
#include <linux/module.h>

static int op_support_interrupt = 1;
module_param(op_support_interrupt, int, 0444);
MODULE_PARM_DESC(op_support_interrupt,
                 "DMA_INTERRUPT capability (default: 1 - enabled)");

static int op_support_memcpy = 1;
module_param(op_support_memcpy, int, 0444);
MODULE_PARM_DESC(op_support_memcpy,
                 "DMA_MEMCPY capability (default: 1 - enabled)");

static int op_support_memset = 1;
module_param(op_support_memset, int, 0444);
MODULE_PARM_DESC(op_support_memset,
                 "DMA_MEMSET capability (default: 1 - enabled)");

static int op_support_xor = 1;
module_param(op_support_xor, int, 0444);
MODULE_PARM_DESC(op_support_xor, "DMA_XOR capability (default: 1 - enabled)");

static int op_support_xor_val = 1;
module_param(op_support_xor_val, int, 0444);
MODULE_PARM_DESC(op_support_xor_val,
                 "DMA_XOR_VAL capability (default: 1 - enabled)");

static int op_support_pq = 1;
module_param(op_support_pq, int, 0444);
MODULE_PARM_DESC(op_support_pq, "DMA_PQ capability (default: 1 - enabled)");

static int op_support_pq_val = 1;
module_param(op_support_pq_val, int, 0444);
MODULE_PARM_DESC(op_support_pq_val,
                 "DMA_PQ_VAL capability (default: 1 - enabled)");

static int max_channels = AL_DMA_MAX_CHANNELS;
module_param(max_channels, int, 0644);
MODULE_PARM_DESC(max_channels,
                 "maximum number of channels (queues) to enable (default: 4)");

static int ring_alloc_order = 10;
module_param(ring_alloc_order, int, 0644);
MODULE_PARM_DESC(ring_alloc_order, "allocate 2^n descriptors per channel"
                                   " (default: 8 max: 16)");

static int tx_descs_order = 15;
module_param(tx_descs_order, int, 0644);
MODULE_PARM_DESC(tx_descs_order,
                 "allocate 2^n of descriptors in Tx queue (default: 15)");

static int rx_descs_order = 15;
module_param(rx_descs_order, int, 0644);
MODULE_PARM_DESC(rx_descs_order,
                 "allocate 2^n of descriptors in Rx queue (default: 15)");

int al_dma_get_op_support_interrupt(void) { return op_support_interrupt; }

int al_dma_get_op_support_memcpy(void) { return op_support_memcpy; }

int al_dma_get_op_support_memset(void) { return op_support_memset; }

int al_dma_get_op_support_xor(void) { return op_support_xor; }

int al_dma_get_op_support_xor_val(void) { return op_support_xor_val; }

int al_dma_get_op_support_pq(void) { return op_support_pq; }

int al_dma_get_op_support_pq_val(void) { return op_support_pq_val; }

int al_dma_get_max_channels(void) { return max_channels; }

int al_dma_get_ring_alloc_order(void) { return ring_alloc_order; }

int al_dma_get_tx_descs_order(void) { return tx_descs_order; }

int al_dma_get_rx_descs_order(void) { return rx_descs_order; }

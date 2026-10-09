/*
 * Annapurna Labs DMA Linux driver - operation preparation declarations
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

#ifndef __AL_DMA_PREP_H__
#define __AL_DMA_PREP_H__

#include "al_dma.h"

struct dma_async_tx_descriptor *al_dma_prep_interrupt_lock(
	struct dma_chan *c,
	unsigned long flags);

struct dma_async_tx_descriptor *al_dma_prep_memcpy_lock(
	struct dma_chan	*c,
	dma_addr_t		dest,
	dma_addr_t		src,
	size_t			len,
	unsigned long		flags);

struct dma_async_tx_descriptor *al_dma_prep_memset_lock(
	struct dma_chan *c,
	dma_addr_t	dest,
	int		value,
	size_t		len,
	unsigned long	flags);

struct dma_async_tx_descriptor *al_dma_prep_xor_lock(
	struct dma_chan	*c,
	dma_addr_t		dest,
	dma_addr_t		*src,
	unsigned int		src_cnt,
	size_t			len,
	unsigned long		flags);

struct dma_async_tx_descriptor *al_dma_prep_pq_lock(
	struct dma_chan	*c,
	dma_addr_t		*dst,
	dma_addr_t		*src,
	unsigned int		src_cnt,
	const unsigned char	*scf,
	size_t			len,
	unsigned long		flags);

struct dma_async_tx_descriptor *al_dma_prep_pq_val_lock(
	struct dma_chan	*c,
	dma_addr_t		*pq,
	dma_addr_t		*src,
	unsigned int		src_cnt,
	const unsigned char	*scf,
	size_t			len,
	enum sum_check_flags	*pqres,
	unsigned long		flags);

struct dma_async_tx_descriptor *al_dma_prep_xor_val_lock(
	struct dma_chan	*c,
	dma_addr_t		*src,
	unsigned int		src_cnt,
	size_t			len,
	enum sum_check_flags	*result,
	unsigned long		flags);

#endif


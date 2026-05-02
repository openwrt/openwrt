/*******************************************************************************
Copyright (C) 2013 Annapurna Labs Ltd.

This file may be licensed under the terms of the Annapurna Labs Commercial
License Agreement.

Alternatively, this file can be distributed under the terms of the GNU General
Public License V2 as published by the Free Software Foundation and can be
found at http://www.gnu.org/licenses/gpl-2.0.html

Alternatively, redistribution and use in source and binary forms, with or
without modification, are permitted provided that the following conditions are
met:

    *     Redistributions of source code must retain the above copyright notice,
	  this list of conditions and the following disclaimer.

    *     Redistributions in binary form must reproduce the above copyright
	  notice, this list of conditions and the following disclaimer in
	  the documentation and/or other materials provided with the
	  distribution.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
 ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*******************************************************************************/

/**
 *  @{
 * @file   al_hal_ssm_raid.c
 *
 */

#include <mach/al_hal_ssm.h>
#include "al_hal_ssm_raid.h"
#include "al_hal_ssm_raid_regs.h"

#define RX_COMP_STATUS_MASK	\
	(AL_RAID_P_VAL_ERROR | AL_RAID_Q_VAL_ERROR |\
	 AL_RAID_BUS_PARITY_ERROR | AL_RAID_SOURCE_LEN_ERROR |\
	 AL_RAID_CMD_DECODE_ERROR | AL_RAID_INTERNAL_ERROR |\
	 AL_RAID_REDIRECTED_TRANSACTION |\
	 AL_RAID_REDIRECTED_SRC_UDMA |\
	 AL_RAID_REDIRECTED_SRC_QUEUE)

/** operation attributes */
struct al_op_attr {
	uint32_t opcode; /* hw opcode */
	uint32_t meta_descs; /* number of meta descs needed for the operation */
	al_bool have_g_coef;
	al_bool have_p_coef;
};

static const struct al_op_attr op_attr_table[] = {
	/*	opcode			meta	g coef	    p coef */
	{AL_RAID_MEM_CPY_OPCODE,	0,	AL_FALSE, AL_FALSE},
	{AL_RAID_MEM_SET_OPCODE,	1,	AL_FALSE, AL_FALSE},
	{AL_RAID_MEM_SCRUB_OPCODE,	0,	AL_FALSE, AL_FALSE},
	{AL_RAID_MEM_CMP_OPCODE,	4,	AL_FALSE, AL_FALSE},
	{AL_RAID_NOP_OPCODE,		1,	AL_FALSE, AL_FALSE},
	{AL_RAID_P_CALC_OPCODE,		0,	AL_FALSE, AL_TRUE},
	{AL_RAID_Q_CALC_OPCODE,		0,	AL_TRUE, AL_FALSE},
	{AL_RAID_PQ_CALC_OPCODE,	0,	AL_TRUE, AL_TRUE},
	{AL_RAID_P_VAL_OPCODE,		0,	AL_FALSE, AL_TRUE},
	{AL_RAID_Q_VAL_OPCODE,		0,	AL_TRUE, AL_FALSE},
	{AL_RAID_PQ_VAL_OPCODE,		0,	AL_TRUE, AL_TRUE},
};

#define GF_SIZE		256

static const uint8_t gflog[GF_SIZE] = {
	0xff, 0x00, 0x01, 0x19, 0x02, 0x32, 0x1a, 0xc6,
	0x03, 0xdf, 0x33, 0xee, 0x1b, 0x68, 0xc7, 0x4b,
	0x04, 0x64, 0xe0, 0x0e, 0x34, 0x8d, 0xef, 0x81,
	0x1c, 0xc1, 0x69, 0xf8, 0xc8, 0x08, 0x4c, 0x71,
	0x05, 0x8a, 0x65, 0x2f, 0xe1, 0x24, 0x0f, 0x21,
	0x35, 0x93, 0x8e, 0xda, 0xf0, 0x12, 0x82, 0x45,
	0x1d, 0xb5, 0xc2, 0x7d, 0x6a, 0x27, 0xf9, 0xb9,
	0xc9, 0x9a, 0x09, 0x78, 0x4d, 0xe4, 0x72, 0xa6,
	0x06, 0xbf, 0x8b, 0x62, 0x66, 0xdd, 0x30, 0xfd,
	0xe2, 0x98, 0x25, 0xb3, 0x10, 0x91, 0x22, 0x88,
	0x36, 0xd0, 0x94, 0xce, 0x8f, 0x96, 0xdb, 0xbd,
	0xf1, 0xd2, 0x13, 0x5c, 0x83, 0x38, 0x46, 0x40,
	0x1e, 0x42, 0xb6, 0xa3, 0xc3, 0x48, 0x7e, 0x6e,
	0x6b, 0x3a, 0x28, 0x54, 0xfa, 0x85, 0xba, 0x3d,
	0xca, 0x5e, 0x9b, 0x9f, 0x0a, 0x15, 0x79, 0x2b,
	0x4e, 0xd4, 0xe5, 0xac, 0x73, 0xf3, 0xa7, 0x57,
	0x07, 0x70, 0xc0, 0xf7, 0x8c, 0x80, 0x63, 0x0d,
	0x67, 0x4a, 0xde, 0xed, 0x31, 0xc5, 0xfe, 0x18,
	0xe3, 0xa5, 0x99, 0x77, 0x26, 0xb8, 0xb4, 0x7c,
	0x11, 0x44, 0x92, 0xd9, 0x23, 0x20, 0x89, 0x2e,
	0x37, 0x3f, 0xd1, 0x5b, 0x95, 0xbc, 0xcf, 0xcd,
	0x90, 0x87, 0x97, 0xb2, 0xdc, 0xfc, 0xbe, 0x61,
	0xf2, 0x56, 0xd3, 0xab, 0x14, 0x2a, 0x5d, 0x9e,
	0x84, 0x3c, 0x39, 0x53, 0x47, 0x6d, 0x41, 0xa2,
	0x1f, 0x2d, 0x43, 0xd8, 0xb7, 0x7b, 0xa4, 0x76,
	0xc4, 0x17, 0x49, 0xec, 0x7f, 0x0c, 0x6f, 0xf6,
	0x6c, 0xa1, 0x3b, 0x52, 0x29, 0x9d, 0x55, 0xaa,
	0xfb, 0x60, 0x86, 0xb1, 0xbb, 0xcc, 0x3e, 0x5a,
	0xcb, 0x59, 0x5f, 0xb0, 0x9c, 0xa9, 0xa0, 0x51,
	0x0b, 0xf5, 0x16, 0xeb, 0x7a, 0x75, 0x2c, 0xd7,
	0x4f, 0xae, 0xd5, 0xe9, 0xe6, 0xe7, 0xad, 0xe8,
	0x74, 0xd6, 0xf4, 0xea, 0xa8, 0x50, 0x58, 0xaf
};

static const uint8_t gfilog[GF_SIZE] = {
	0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80,
	0x1d, 0x3a, 0x74, 0xe8, 0xcd, 0x87, 0x13, 0x26,
	0x4c, 0x98, 0x2d, 0x5a, 0xb4, 0x75, 0xea, 0xc9,
	0x8f, 0x03, 0x06, 0x0c, 0x18, 0x30, 0x60, 0xc0,
	0x9d, 0x27, 0x4e, 0x9c, 0x25, 0x4a, 0x94, 0x35,
	0x6a, 0xd4, 0xb5, 0x77, 0xee, 0xc1, 0x9f, 0x23,
	0x46, 0x8c, 0x05, 0x0a, 0x14, 0x28, 0x50, 0xa0,
	0x5d, 0xba, 0x69, 0xd2, 0xb9, 0x6f, 0xde, 0xa1,
	0x5f, 0xbe, 0x61, 0xc2, 0x99, 0x2f, 0x5e, 0xbc,
	0x65, 0xca, 0x89, 0x0f, 0x1e, 0x3c, 0x78, 0xf0,
	0xfd, 0xe7, 0xd3, 0xbb, 0x6b, 0xd6, 0xb1, 0x7f,
	0xfe, 0xe1, 0xdf, 0xa3, 0x5b, 0xb6, 0x71, 0xe2,
	0xd9, 0xaf, 0x43, 0x86, 0x11, 0x22, 0x44, 0x88,
	0x0d, 0x1a, 0x34, 0x68, 0xd0, 0xbd, 0x67, 0xce,
	0x81, 0x1f, 0x3e, 0x7c, 0xf8, 0xed, 0xc7, 0x93,
	0x3b, 0x76, 0xec, 0xc5, 0x97, 0x33, 0x66, 0xcc,
	0x85, 0x17, 0x2e, 0x5c, 0xb8, 0x6d, 0xda, 0xa9,
	0x4f, 0x9e, 0x21, 0x42, 0x84, 0x15, 0x2a, 0x54,
	0xa8, 0x4d, 0x9a, 0x29, 0x52, 0xa4, 0x55, 0xaa,
	0x49, 0x92, 0x39, 0x72, 0xe4, 0xd5, 0xb7, 0x73,
	0xe6, 0xd1, 0xbf, 0x63, 0xc6, 0x91, 0x3f, 0x7e,
	0xfc, 0xe5, 0xd7, 0xb3, 0x7b, 0xf6, 0xf1, 0xff,
	0xe3, 0xdb, 0xab, 0x4b, 0x96, 0x31, 0x62, 0xc4,
	0x95, 0x37, 0x6e, 0xdc, 0xa5, 0x57, 0xae, 0x41,
	0x82, 0x19, 0x32, 0x64, 0xc8, 0x8d, 0x07, 0x0e,
	0x1c, 0x38, 0x70, 0xe0, 0xdd, 0xa7, 0x53, 0xa6,
	0x51, 0xa2, 0x59, 0xb2, 0x79, 0xf2, 0xf9, 0xef,
	0xc3, 0x9b, 0x2b, 0x56, 0xac, 0x45, 0x8a, 0x09,
	0x12, 0x24, 0x48, 0x90, 0x3d, 0x7a, 0xf4, 0xf5,
	0xf7, 0xf3, 0xfb, 0xeb, 0xcb, 0x8b, 0x0b, 0x16,
	0x2c, 0x58, 0xb0, 0x7d, 0xfa, 0xe9, 0xcf, 0x83,
	0x1b, 0x36, 0x6c, 0xd8, 0xad, 0x47, 0x8e, 0x00
};

static void _al_raid_load_table(
	struct raid_accelerator_regs __iomem	*app_regs,
	int					gflog,
	const uint8_t				*table)
{
	uint32_t *base_reg;
	int	i;

	if (gflog)
		base_reg = &app_regs->gflog_table[0].w0_raw;
	else
		base_reg = &app_regs->gfilog_table[0].w0_r;

	for (i = 0; i < GF_SIZE/4; i++) {
		int	table_idx = i << 2; /* *4 */
		uint32_t reg = (table[table_idx + 3] << 24) |
			(table[table_idx + 2] << 16) |
			(table[table_idx + 1] << 8) |
			table[table_idx];

		al_reg_write32(base_reg + i, reg);
	}
}

/**
 * get number of rx submission descriptors needed to the transaction
 *
 * we need rx descriptor for each destination buffer.
 * if the transaction doesn't have destination buffers, then one*
 * descriptor is needed
 *
 * @param xaction transaction context
 *
 * @return number of rx submission descriptors
 */
INLINE uint32_t _al_raid_xaction_rx_descs_count(
	struct al_raid_transaction *xaction)
{
	return xaction->total_dst_bufs ? xaction->total_dst_bufs : 1;
}

/**
 * get number of tx submission descriptors needed to the transaction
 *
 * we need tx descriptor for each source buffer.
 * MEM_SET needs 1 Meta descriptor, MEM_CMP needs 4
 * and we need at least one descriptor for the opcode
 *
 * @param xaction transaction context
 * @param meta number of meta descriptors
 *
 * @return number of tx submission descriptors
 */
INLINE uint32_t _al_raid_xaction_tx_descs_count(
	struct al_raid_transaction	*xaction,
	uint32_t					meta)
{
	uint32_t count = xaction->total_src_bufs + meta;
	return count ? count : 1;
}

/**
 * prepare the rx submission descriptors
 * this function writes the contents of the rx submission descriptors
 * @param rx_udma_q rx udma handle
 * @param xaction transaction context
 */
static void _al_raid_set_rx_descs(
	struct al_udma_q			*rx_udma_q,
	struct al_raid_transaction	*xaction)
{
	uint32_t flags = 0;
	union al_udma_desc *rx_desc;
	struct al_block *block;
	uint32_t blk_idx;

	if (xaction->flags & AL_SSM_INTERRUPT)
		flags = AL_M2S_DESC_INT_EN;
	if (xaction->flags & AL_SSM_DEST_NO_SNOOP)
		flags |= AL_M2S_DESC_NO_SNOOP_H;

	/* if the xaction doesn't have destination buffers, allocate single
	   Meta descriptor */
	if (xaction->total_dst_bufs == 0) {
		rx_desc = al_udma_desc_get(rx_udma_q);
		flags |= al_udma_ring_id_get(rx_udma_q) <<
			AL_M2S_DESC_RING_ID_SHIFT;
		flags |= AL_RAID_RX_DESC_META;
		/* write back flags */
		rx_desc->rx.len_ctrl = swap32_to_le(flags);
		return;
	}

	/* for each desc set buffer length, address */
	block = xaction->dsts_blocks;
	for (blk_idx = 0; blk_idx < xaction->num_of_dsts; blk_idx++) {
		struct al_buf *buf = block->bufs;
		unsigned int buf_idx = 0;
		for (; buf_idx < block->num; buf_idx++) {
			uint64_t vmid =	((uint64_t)block->vmid) <<
				AL_UDMA_DESC_VMID_SHIFT;
			uint32_t flags_len = flags;
			uint32_t ring_id;

			rx_desc =  al_udma_desc_get(rx_udma_q);
			/* get ring id */
			ring_id = al_udma_ring_id_get(rx_udma_q)
				<< AL_M2S_DESC_RING_ID_SHIFT;

			flags_len |= ring_id;

			flags_len |= buf->len & AL_M2S_DESC_LEN_MASK;
			rx_desc->rx.len_ctrl = swap32_to_le(flags_len);
			rx_desc->rx.buf1_ptr = swap64_to_le(buf->addr | vmid);
			buf++;
		}
		block++;
	}
}

/**
 * calculate the total length of rx buffers
 *
 * @param xaction transaction context
 */
static uint32_t _al_raid_rx_get_len(struct al_raid_transaction *xaction)
{
	uint32_t total_len = 0;
	uint32_t blk_idx;
	struct al_block *block = xaction->dsts_blocks;

	for (blk_idx = 0; blk_idx < xaction->num_of_dsts; blk_idx++) {
		struct al_buf *buf = block->bufs;
		unsigned int buf_idx = 0;
		for (; buf_idx < block->num; buf_idx++) {
			total_len += buf->len;
			buf++;
		}
	}
	return total_len;
}

/**
 * fill the tx submission descriptors
 * this function writes the contents of the tx submission descriptors
 * @param tx_udma_q tx udma handle
 * @param xaction transaction context
 * @param meta number of meta descriptors used by this xaction
 * @param op_attr operation attributes
 */
static void _al_raid_set_tx_descs(
	struct al_udma_q				*tx_udma_q,
	  struct al_raid_transaction	*xaction,
	uint32_t						meta,
	const struct al_op_attr		*op_attr)
{
	union al_udma_desc *tx_desc;
	uint32_t flags = AL_M2S_DESC_FIRST;
	struct al_block *block;
	uint32_t blk_idx;


	for (blk_idx = 0; blk_idx < meta; blk_idx++) {
		uint32_t flags_len = flags;
		uint32_t ring_id;

		/* clear first flags */
		flags = 0;

		/* get next descriptor */
		tx_desc =  al_udma_desc_get(tx_udma_q);
		/* get ring id */
		ring_id = al_udma_ring_id_get(tx_udma_q) <<
			AL_M2S_DESC_RING_ID_SHIFT;

		flags_len |= ring_id;

		/* write descriptor's flags */
		flags_len |= AL_M2S_DESC_META_DATA;

		/* set LAST flag */
		if ((blk_idx == (meta - 1)) && (xaction->num_of_srcs == 0)) {
			flags_len |= AL_M2S_DESC_LAST;
			if (xaction->flags & AL_SSM_BARRIER)
				flags_len |= AL_M2S_DESC_DMB;
		}

		if (blk_idx == 0) {
			uint32_t attr = op_attr->opcode;

			if (xaction->op == AL_RAID_OP_MEM_SET)
				attr |= xaction->mem_set_flags;

			/* write opcode in first descriptor */
			tx_desc->tx.meta_ctrl = swap32_to_le(attr);
		}
		/* write meta data */
		if (xaction->op == AL_RAID_OP_MEM_SET) {
			/* memset needs length in meta desc */
			uint32_t total_len = _al_raid_rx_get_len(xaction);
			flags_len |= total_len & AL_M2S_DESC_LEN_MASK;
			tx_desc->tx_meta.meta1 = xaction->data[1];
			tx_desc->tx_meta.meta2 = xaction->data[0];
		} else if (xaction->op == AL_RAID_OP_MEM_CMP) {
			tx_desc->tx_meta.meta1 = xaction->pattern_data[blk_idx];
			tx_desc->tx_meta.meta2 = xaction->pattern_mask[blk_idx];
		}
		tx_desc->tx.len_ctrl = swap32_to_le(flags_len);
	}

	if (xaction->flags & AL_SSM_SRC_NO_SNOOP)
		flags |= AL_M2S_DESC_NO_SNOOP_H;
	if (xaction->flags & AL_SSM_BARRIER)
		flags |= AL_M2S_DESC_DMB;

	/* for each desc set buffer length, address */
	block = xaction->srcs_blocks;
	for (blk_idx = 0; blk_idx < xaction->num_of_srcs; blk_idx++) {
		uint32_t attr = op_attr->opcode;
		struct al_buf *buf = block->bufs;
		unsigned int buf_idx = 0;

		attr &= ~0xFFFFF;
		if (op_attr->have_g_coef == AL_TRUE)
			attr |= xaction->coefs[blk_idx] & 0xFF;
		if (blk_idx == 0)
			attr |= AL_RAID_TX_DESC_META_FIRST_SOURCE;
		if (blk_idx == (xaction->num_of_srcs - 1))
			attr |= AL_RAID_TX_DESC_META_LAST_SOURCE;
		if (op_attr->have_p_coef == AL_TRUE)
			if ((xaction->op != AL_RAID_OP_PQ_VAL) ||
				(blk_idx != xaction->q_index))
				attr |= AL_RAID_TX_DESC_META_P_ENABLE;

		for (buf_idx = 0; buf_idx < block->num; buf_idx++) {
			uint64_t vmid =	((uint64_t)block->vmid) <<
				AL_UDMA_DESC_VMID_SHIFT;
			uint32_t flags_len = flags;
			uint32_t ring_id;

			/* clear first and DMB flags, keep no snoop hint flag */
			flags &= AL_M2S_DESC_NO_SNOOP_H;

			tx_desc =  al_udma_desc_get(tx_udma_q);
			/* get ring id, and clear FIRST and Int flags */
			ring_id = al_udma_ring_id_get(tx_udma_q) <<
				AL_M2S_DESC_RING_ID_SHIFT;

			flags_len |= ring_id;
			/* set LAST flag if last descriptor */
			if ((blk_idx == (xaction->num_of_srcs - 1)) &&
			    (buf_idx == (block->num - 1)))
				flags_len |= AL_M2S_DESC_LAST;

			flags_len |= buf->len & AL_M2S_DESC_LEN_MASK;

			if (buf_idx == 0) {
				/* write attributes for descriptors */
				/* that start new source */
				tx_desc->tx.meta_ctrl = swap32_to_le(attr);
			} else {
				flags_len |= AL_M2S_DESC_CONCAT;
			}
			tx_desc->tx.len_ctrl = swap32_to_le(flags_len);
			tx_desc->tx.buf_ptr = swap64_to_le(buf->addr | vmid);
			/* move to next buffer/descriptor */
			buf++;
		}
		block++;
	}
}

/****************************** API functions *********************************/
void al_raid_init(void __iomem *app_regs)
{
	/* initialize the GFLOG and GFILOG tables of the hw */
	_al_raid_load_table(app_regs, 1, gflog);
	_al_raid_load_table(app_regs, 0, gfilog);
}

/**
 * prepare raid transaction
 *
 * @param raid_dma raid DMA handle
 * @param qid queue index
 * @param xaction transaction context
 *
 * @return 0 if no error found.
 *	   -ENOSPC if no space available.
 */
int al_raid_dma_prepare(
	struct al_ssm_dma			*raid_dma,
	uint32_t					qid,
	struct al_raid_transaction	*xaction)
{
	uint32_t rx_descs;
	uint32_t tx_descs, meta;
	struct al_udma_q *rx_udma_q;
	struct al_udma_q *tx_udma_q;
	const struct al_op_attr *op_attr;
	int rc;

	/* assert valid opcode */
	al_assert(xaction->op <
		(sizeof(op_attr_table)/sizeof(op_attr_table[0])));

	op_attr = &(op_attr_table[xaction->op]);

	/* calc rx (S2M) descriptors */
	rx_descs = _al_raid_xaction_rx_descs_count(xaction);
	al_assert(rx_descs <= AL_SSM_MAX_DST_DESCS);
	rc =  al_udma_q_handle_get(&raid_dma->m2m_udma.rx_udma, qid, &rx_udma_q);

	al_assert(rc == 0); /* assert valid rx q handle */

	if (unlikely(al_udma_available_get(rx_udma_q) < rx_descs)) {
		al_dbg("raid [%s]: rx q has no enough free descriptor",
			 raid_dma->m2m_udma.name);
		return -ENOSPC;
	}

	/* calc tx (M2S) descriptors */
	meta = op_attr->meta_descs;
	tx_descs = _al_raid_xaction_tx_descs_count(xaction, meta);
	al_assert(tx_descs <= AL_SSM_MAX_SRC_DESCS);
	rc =  al_udma_q_handle_get(&raid_dma->m2m_udma.tx_udma, qid, &tx_udma_q);
	al_assert(rc == 0); /* assert valid tx q handle */
	if (unlikely(al_udma_available_get(tx_udma_q) < tx_descs)) {
		al_dbg("raid [%s]: tx q has no enough free descriptor",
			 raid_dma->m2m_udma.name);
		return -ENOSPC;
	}

	/* prepare rx descs */
	_al_raid_set_rx_descs(rx_udma_q, xaction);
	/* add rx descriptors */
	al_udma_desc_action_add(rx_udma_q, rx_descs);

	/* prepare tx descriptors */
	_al_raid_set_tx_descs(tx_udma_q, xaction, meta, op_attr);
	/* add tx descriptors */
	xaction->tx_descs_count = tx_descs;
	/* union al_udma_desc_action_add(tx_udma_q, tx_descs); */

	return 0;
}
EXPORT_SYMBOL(al_raid_dma_prepare);

/**
 * add previously prepared transaction to hw engine
 *
 * @param raid_dma raid DMA handle
 * @param qid queue index
 * @param tx_descs number of tx descriptors to be processed by the engine
 *
 * @return 0 if no error found.
 *	   -ENOSPC if no space available.
 */
int al_raid_dma_action(
	struct al_ssm_dma	*raid_dma,
	uint32_t			qid,
	uint32_t			tx_descs)
{
	struct al_udma_q *tx_udma_q;
	int rc;

	rc =  al_udma_q_handle_get(&raid_dma->m2m_udma.tx_udma, qid, &tx_udma_q);
	al_assert(rc == 0); /* assert valid tx q handle */

	al_udma_desc_action_add(tx_udma_q, tx_descs);

	return 0;
}
EXPORT_SYMBOL(al_raid_dma_action);

/**
 * check and cleanup completed transaction
 *
 * @param raid_dma raid DMA handle
 * @param qid queue index
 * @param comp_status status reported by rx completion descriptor
 *
 * @return 1 if a transaction was completed. 0 otherwise
 */
int al_raid_dma_completion(
	struct al_ssm_dma	*raid_dma,
	uint32_t qid,
	uint32_t *comp_status)
{
	struct al_udma_q *rx_udma_q;
	struct al_udma_q *tx_udma_q;
	volatile union al_udma_cdesc *cdesc;
	int rc;
	uint32_t cdesc_count;

	rc =  al_udma_q_handle_get(&raid_dma->m2m_udma.rx_udma, qid, &rx_udma_q);
	al_assert(rc == 0); /* assert valid rx q handle */

	cdesc_count = al_udma_cdesc_packet_get(rx_udma_q, &cdesc);
	if (cdesc_count == 0)
		return 0;

	/* if we have multiple completion descriptors, then last one will have
	   the valid status */
	if (unlikely(cdesc_count > 1))
		cdesc = al_cdesc_next(rx_udma_q, cdesc, cdesc_count - 1);

	*comp_status = swap32_from_le(cdesc->al_desc_comp_rx.ctrl_meta) &
		RX_COMP_STATUS_MASK;

	al_dbg(
		"raid [%s %d]: packet completed. "
		"count %d status desc %p meta %x\n",
		raid_dma->m2m_udma.name, qid, cdesc_count, cdesc,
		cdesc->al_desc_comp_rx.ctrl_meta);

	al_udma_cdesc_ack(rx_udma_q, cdesc_count);

	/* cleanup tx completion queue */
	rc =  al_udma_q_handle_get(&raid_dma->m2m_udma.tx_udma, qid, &tx_udma_q);
	al_assert(rc == 0); /* assert valid tx q handle */

	cdesc_count = al_udma_cdesc_get_all(tx_udma_q, NULL);
	if (cdesc_count != 0)
		al_udma_cdesc_ack(tx_udma_q, cdesc_count);

	return 1;
}
EXPORT_SYMBOL(al_raid_dma_completion);
/** @} end of RAID group */

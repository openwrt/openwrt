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
 * @defgroup group_udma_fast UDMA Fast API
 *
 ** @{
 * The UDMA Fast API can be used to perform simple operations by directly modifying
 * the UDMA descriptors instead of passing via HAL SW structures and functions.
 * This allows to achieve optimal performance for those operations.
 *
 * Currently RAID controller and MEMCOPY and PARALLEL MEMCOPY operations are
 * supported.
 *
 * @file   al_hal_udma_fast.h
 *
 * @brief Header file for UDMA Fast API
 *
 */

#ifndef __AL_HAL_UDMA_FAST_H__
#define __AL_HAL_UDMA_FAST_H__

#include <mach/al_hal_common.h>
#include <mach/al_hal_ssm_crc_memcpy.h>
#include <mach/al_hal_udma.h>

/* *INDENT-OFF* */
#ifdef __cplusplus
extern "C" {
#endif
/* *INDENT-ON* */

/**
 * Prepare MEMCPY udma queues to work in fast mode - init all the descriptors
 * according to opcode and flags passed in xaction
 *
 * @param udma_txq udma tx queue handle
 * @param udma_rxq udma rx queue handle
 * @param xaction transaction context
 *
 * @return 0 if no error found
 */
int al_udma_fast_memcpy_q_prepare(struct al_udma_q *udma_txq,
		struct al_udma_q *udma_rxq,
		struct al_memcpy_transaction *xaction);

/**
 * Get udma descriptor by index in queue
 *
 * @param udma_q udma queue handle
 * @param index descriptor index
 *
 * @return udma descriptor handle
 */
static INLINE union al_udma_desc *al_udma_fast_desc_get_by_idx(struct al_udma_q *udma_q,
		uint32_t index)
{
	union al_udma_desc *desc;

	al_assert(udma_q);
	al_assert(index < udma_q->size);

	desc = udma_q->desc_base_ptr + index;
	return desc;
}

/* Work with tx desc structures as buf_ptr, flags and len fields are in same
 * location for tx and rx descs
 */
/**
 * Set udma descriptor buffer address
 *
 * @param desc udma descriptor handle
 * @param buf_ptr buffer adddress
 * @param vmid virtual machine id
 */
static inline void al_udma_fast_desc_buf_set(union al_udma_desc *desc,
		al_phys_addr_t buf_ptr, uint16_t vmid)
{
	al_assert(desc);

	desc->tx.buf_ptr = swap64_to_le(buf_ptr | vmid);
}

/**
 * Set udma descriptor flags specified by flags param and mask, while keeping
 * flags that are not specified by the mask
 *
 * @param desc udma descriptor handle
 * @param flags flags
 * @param mask flags mask
 */
static inline void al_udma_fast_desc_flags_set(union al_udma_desc *desc,
		uint32_t flags, uint32_t mask)
{
	uint32_t flags_len;

	al_assert(desc);

	flags_len = swap32_from_le(desc->tx.len_ctrl);
	mask &= ~AL_M2S_DESC_LEN_MASK;
	flags_len &= ~mask;
	flags_len |= flags;
	desc->tx.len_ctrl = swap32_to_le(flags_len);
}

/**
 * Set udma descriptor ring id
 *
 * @param desc udma descriptor handle
 * @param ring_id ring id
 */
static inline void al_udma_fast_desc_ring_id_set(union al_udma_desc *desc,
		uint32_t ring_id)
{
	uint32_t flags_len;

	al_assert(desc);

	flags_len = swap32_from_le(desc->tx.len_ctrl);
	flags_len &= ~AL_M2S_DESC_RING_ID_MASK;
	flags_len |= ring_id << AL_M2S_DESC_RING_ID_SHIFT;
	desc->tx.len_ctrl = swap32_to_le(flags_len);
}

/**
 * Set udma descriptor buffer length
 *
 * @param desc udma descriptor handle
 * @param len buffer length
 */
static inline void al_udma_fast_desc_len_set(union al_udma_desc *desc,
		uint16_t len)
{
	uint32_t flags_len;

	al_assert(desc);

	flags_len = swap32_from_le(desc->tx.len_ctrl);
	flags_len &= ~AL_M2S_DESC_LEN_MASK;
	flags_len |= len;
	desc->tx.len_ctrl = swap32_to_le(flags_len);
}

/**
 * Get up to desc_to_complete completed descriptors
 *
 * If use_head is set to AL TRUE head register is used to determine number of
 * completed descriptors and reg read is performed on every poll operation.
 * Otherwise each completion descriptor is read to determine whether it is
 * completed. If HW cache coherency is used there's no cache miss until the
 * descriptor is completed.
 *
 * @param udma_rxq udma rx queue handle
 * @param descs_to_complete max number of completed descriptors to get
 * @param use_head poll head register instead of completion descriptors
 */
static inline int al_udma_fast_completion(struct al_udma_q *udma_rxq,
		uint32_t descs_to_complete, al_bool use_head)
{
	volatile union al_udma_cdesc *curr;
	uint32_t cdesc_count = 0;

	if (!use_head) {
		curr = udma_rxq->comp_head_ptr;
		while (descs_to_complete != 0) {
			uint32_t comp_flags;

			comp_flags = swap32_from_le(curr->al_desc_comp_tx.ctrl_meta);
			if (al_udma_new_cdesc(udma_rxq, comp_flags) == AL_FALSE)
				break;

			cdesc_count++;
			descs_to_complete--;
			curr = al_cdesc_next_update(udma_rxq, curr);
		}

		udma_rxq->comp_head_ptr = curr;
	} else {
		cdesc_count = al_udma_cdesc_get_all(udma_rxq, NULL);

		if (cdesc_count > descs_to_complete)
			cdesc_count = descs_to_complete;
	}

	if (cdesc_count)
		al_udma_cdesc_ack(udma_rxq, cdesc_count);

	return cdesc_count;
}

/* *INDENT-OFF* */
#ifdef __cplusplus
}
#endif
/* *INDENT-ON* */
/** @} end of UDMA Fast group */
#endif /* __AL_HAL_UDMA_FAST_H__ */

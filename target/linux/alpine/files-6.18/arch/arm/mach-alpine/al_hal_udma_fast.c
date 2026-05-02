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

#include <mach/al_hal_udma_fast.h>

int al_udma_fast_memcpy_q_prepare(struct al_udma_q *udma_txq,
		struct al_udma_q *udma_rxq,
		struct al_memcpy_transaction *xaction)
{
	union al_udma_desc *desc;
	uint32_t i;
	uint32_t attr = 0;
	uint32_t tx_flags = 0;
	uint32_t rx_flags = 0;

	attr |= AL_CRC_CHECKSUM << TX_DESC_META_OP_SHIFT;
	attr |= AL_CRC_CHECKSUM_NULL << TX_DESC_META_CRC_OP_TYPE_SHIFT;
	attr |= TX_DESC_META_CRC_SEND_ORIG;
	attr |= RX_DESC_META_CRC_FIRST_BUF;
	attr |= RX_DESC_META_CRC_LAST_BUF;

	if (xaction->flags & AL_SSM_BARRIER)
		tx_flags |= AL_M2S_DESC_DMB;
	if (xaction->flags & AL_SSM_SRC_NO_SNOOP)
		tx_flags |= AL_M2S_DESC_NO_SNOOP_H;
	tx_flags |= AL_M2S_DESC_FIRST | AL_M2S_DESC_LAST;

	if (xaction->flags & AL_SSM_INTERRUPT)
		rx_flags |= AL_M2S_DESC_INT_EN;
	if (xaction->flags & AL_SSM_DEST_NO_SNOOP)
		rx_flags |= AL_M2S_DESC_NO_SNOOP_H;

	for (i = 0; i < udma_txq->size; i++) {
		desc = udma_txq->desc_base_ptr + i;
		desc->tx.meta_ctrl = swap32_to_le(attr);
		desc->tx.len_ctrl = swap32_to_le(tx_flags);
	}

	for (i = 0; i < udma_rxq->size; i++) {
		desc = udma_rxq->desc_base_ptr + i;
		desc->rx.len_ctrl = swap32_to_le(rx_flags);
	}

	return 0;
}

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
 * @file   al_hal_m2m_udma.c
 *
 * @brief  HAL driver for DMA that is a compound of M2S and S2M UDMAs.
 *
 */

#include <mach/al_hal_m2m_udma.h>
#include <mach/al_hal_udma_config.h>

/* Init M2M UDMA */
/*
 * initialize M2M UDMA
 */
int al_m2m_udma_init(struct al_m2m_udma *m2m_udma,
					struct al_m2m_udma_params *params)
{
	struct al_udma_params dma_params;
	int rc;

	al_dbg("raid [%s]: Initialize unit\n", params->name);

	m2m_udma->name = params->name;
	m2m_udma->m2s_regs_base = params->m2s_regs_base;
	m2m_udma->s2m_regs_base = params->s2m_regs_base;
	m2m_udma->num_of_queues = params->num_of_queues;

	/* initialize tx udma component */
	dma_params.udma_reg =
			(union udma_regs __iomem *)m2m_udma->m2s_regs_base;
	dma_params.type = UDMA_TX;
	dma_params.num_of_queues = m2m_udma->num_of_queues;
	dma_params.name = "tx dma";

	rc = al_udma_init(&m2m_udma->tx_udma, &dma_params);
	if (rc != 0) {
		al_err("failed to initialize %s, error %d\n",
			 dma_params.name, rc);
		return rc;
	}

	al_udma_m2s_max_descs_set(&m2m_udma->tx_udma, params->max_m2s_descs_per_pkt);

	/* initialize rx udma component */
	dma_params.udma_reg =
			(union udma_regs __iomem *)m2m_udma->s2m_regs_base;
	dma_params.type = UDMA_RX;
	dma_params.num_of_queues = m2m_udma->num_of_queues;
	dma_params.name = "rx dma";

	rc = al_udma_init(&m2m_udma->rx_udma, &dma_params);
	if (rc != 0) {
		al_err("failed to initialize %s, error %d\n",
			 dma_params.name, rc);
		return rc;
	}

	al_udma_s2m_max_descs_set(&m2m_udma->rx_udma, params->max_s2m_descs_per_pkt);

	return 0;
}

/*
 * initialize the m2s(tx) and s2m(tx) udmas of the queue
 */
int al_m2m_udma_q_init(struct al_m2m_udma *m2m_udma, uint32_t qid,
			   struct al_udma_q_params *tx_params,
			   struct al_udma_q_params *rx_params)
{
	int rc;

	al_dbg("udma [%s]: Initialize queue %d\n", m2m_udma->name,
		 qid);

	rc = al_udma_q_init(&m2m_udma->tx_udma, qid, tx_params);
	if (rc != 0) {
		al_err("[%s]: failed to initialize tx q %d, error %d\n",
			 m2m_udma->name, qid, rc);
		return rc;
	}

	rc = al_udma_q_init(&m2m_udma->rx_udma, qid, rx_params);
	if (rc != 0) {
		al_err("[%s]: failed to initialize rx q %d, error %d\n",
			 m2m_udma->name, qid, rc);
		return rc;
	}
	return 0;

}

/*
 * Change the M2M UDMA state
 */
int al_m2m_udma_state_set(struct al_m2m_udma *m2m_udma,
					enum al_udma_state udma_state)
{
	int rc;

	rc = al_udma_state_set(&m2m_udma->tx_udma, udma_state);
	if (rc != 0) {
		al_err("[%s]: failed to change state, error %d\n",
			 m2m_udma->name, rc);
		return rc;
	}

	rc = al_udma_state_set(&m2m_udma->rx_udma, udma_state);
	if (rc != 0) {
		al_err("[%s]: failed to change state, error %d\n",
			 m2m_udma->name, rc);
		return rc;
	}

	return 0;
}

/*
 * Get udma handle of the tx or rx udma, this handle can be used to call misc
 * configuration functions defined at al_udma_config.h
 */
int al_m2m_udma_handle_get(struct al_m2m_udma *m2m_udma,
						enum al_udma_type type,
						struct al_udma **udma)
{
	if (type == UDMA_TX)
		*udma = &m2m_udma->tx_udma;
	else
		*udma = &m2m_udma->rx_udma;
	return 0;
}

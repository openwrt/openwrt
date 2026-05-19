/*******************************************************************************
Copyright (C) 2014 Annapurna Labs Ltd.

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
 * @file   al_hal_ssm.c
 *
 */

#include <mach/al_hal_ssm.h>

/**
 * Initialize acceleration DMA for RAID/Crypto usage
 *
 * @param ssm_dma ssm dma handle
 * @param params parameters from upper layer
 *
 * @return 0 on success.
 */
int al_ssm_dma_init(
	struct al_ssm_dma		*ssm_dma,
	struct al_ssm_dma_params	*params)
{
	struct al_m2m_udma_params m2m_params;
	struct unit_regs __iomem *unit_regs;
	int rc;

	al_dbg("ssm [%s]: Initialize unit\n", params->name);

	ssm_dma->dev_id = params->dev_id;
	ssm_dma->rev_id = params->rev_id;

	m2m_params.name = params->name;
	unit_regs = (struct unit_regs __iomem *)params->udma_regs_base;
	m2m_params.m2s_regs_base = &unit_regs->m2s;
	m2m_params.s2m_regs_base = &unit_regs->s2m;
	m2m_params.num_of_queues = params->num_of_queues;
	m2m_params.max_m2s_descs_per_pkt = AL_SSM_MAX_SRC_DESCS;
	m2m_params.max_s2m_descs_per_pkt = AL_SSM_MAX_DST_DESCS;

	/* initialize the udma  */
	rc =  al_m2m_udma_init(&ssm_dma->m2m_udma, &m2m_params);
	if (rc != 0)
		al_err("failed to initialize udma, error %d\n", rc);
	return rc;
}

/**
 * Initialize the m2s(tx) and s2m(rx) components of the queue
 *
 * @param ssm_dma ssm dma handle
 * @param qid queue index
 * @param tx_params TX UDMA params
 * @param rx_params RX UDMA params
 * @param q_type indicate q type (crc/csum/memcpy, crypto, raid)
 *
 * @return 0 if no error found.
 *	   -EINVAL if the qid is out of range
 *	   -EIO if queue was already initialized
 */
int al_ssm_dma_q_init(struct al_ssm_dma		*ssm_dma,
		      uint32_t			qid,
		      struct al_udma_q_params	*tx_params,
		      struct al_udma_q_params	*rx_params,
		      enum al_ssm_q_type	q_type)
{
	int rc;

	al_dbg("ssm [%s]: Initialize queue %d\n",
		 ssm_dma->m2m_udma.name, qid);

	tx_params->dev_id = ssm_dma->dev_id;
	tx_params->rev_id = ssm_dma->rev_id;
	rx_params->dev_id = ssm_dma->dev_id;
	rx_params->rev_id = ssm_dma->rev_id;

	rc = al_m2m_udma_q_init(&ssm_dma->m2m_udma, qid, tx_params, rx_params);
	if (rc != 0)
		al_err("ssm [%s]: failed to initialize tx q %d, error %d\n",
			 ssm_dma->m2m_udma.name, qid, rc);
	else
		ssm_dma->q_types[qid] = q_type;

	return rc;
}

/**
 * Change the DMA state
 *
 * @param ssm_dma ssm DMA handle
 * @param dma_state the new state
 *
 * @return 0
 */
int al_ssm_dma_state_set(
	struct al_ssm_dma	*ssm_dma,
	enum al_udma_state	dma_state)
{
	int rc;

	rc =  al_m2m_udma_state_set(&ssm_dma->m2m_udma, dma_state);
	if (rc != 0)
		al_err("ssm [%s]: failed to change state, error %d\n",
			 ssm_dma->m2m_udma.name, rc);
	return rc;
}

/**
 * Get udma handle of the tx or rx udma, this handle can be used to call misc
 * configuration functions defined at al_udma_config.h
 *
 * @param ssm_dma ssm DMA handle
 * @param type tx or rx udma
 * @param udma the requested udma handle written to this pointer
 *
 * @return 0
 */
int al_ssm_dma_handle_get(
	struct al_ssm_dma	*ssm_dma,
	enum al_udma_type	type,
	struct al_udma		**udma)
{
	return al_m2m_udma_handle_get(&ssm_dma->m2m_udma, type, udma);
}

/******************************************************************************
 ******************************************************************************/
struct al_udma *al_ssm_dma_tx_udma_handle_get(
	struct al_ssm_dma *ssm_dma)
{
	struct al_udma *udma;
	int err;

	err = al_m2m_udma_handle_get(&ssm_dma->m2m_udma, UDMA_TX, &udma);
	if (err)
		return NULL;

	return udma;
}

/******************************************************************************
 ******************************************************************************/
struct al_udma_q *al_ssm_dma_tx_queue_handle_get(
	struct al_ssm_dma	*ssm_dma,
	unsigned int		qid)
{
	struct al_udma *udma;
	int err;

	err = al_m2m_udma_handle_get(&ssm_dma->m2m_udma, UDMA_TX, &udma);
	if (err)
		return NULL;

	return &udma->udma_q[qid];
}

/******************************************************************************
 ******************************************************************************/
struct al_udma *al_ssm_dma_rx_udma_handle_get(
	struct al_ssm_dma *ssm_dma)
{
	struct al_udma *udma;
	int err;

	err = al_m2m_udma_handle_get(&ssm_dma->m2m_udma, UDMA_RX, &udma);
	if (err)
		return NULL;

	return udma;
}

/******************************************************************************
 ******************************************************************************/
struct al_udma_q *al_ssm_dma_rx_queue_handle_get(
	struct al_ssm_dma	*ssm_dma,
	unsigned int		qid)
{
	struct al_udma *udma;
	int err;

	err = al_m2m_udma_handle_get(&ssm_dma->m2m_udma, UDMA_RX, &udma);
	if (err)
		return NULL;

	return &udma->udma_q[qid];
}

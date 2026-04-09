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
 * @defgroup group_m2m_udma M2M UDMA
 * @ingroup group_udma_api
 *  M2M UDMA
 *  @{
 * @file   al_hal_m2m_udma.h
 *
 * @brief Header file for HAL driver for DMA that compound of M2S and S2M.
 *
 * The M2M UDMA is a software concept that defines a DMA that is consisted from
 * M2S and S2M UDMAs, this concept is used to share common functionality
 * between different DMA's that use M2S and S2M UDMAs, this is the case for the
 * RAID and Crypto Acceleration DMAs.
 * The M2M UDMA is built on top of the UDMA driver, while the later manages
 * either S2M or M2S UDMA, the M2M instantiates two UDMA engines, and uses the
 * UDMA driver to manage and provide the following functionalities:
 * - S2M and M2S UDMA initialization.
 * - S2M and M2S UDMA Queues initialization.
 * - manages the state of the two GMAs.
 * Other functionalities will be provided directly by the UDMA driver.
 */

#ifndef __AL_HAL_M2M_UDMA_H__
#define __AL_HAL_M2M_UDMA_H__

#include <mach/al_hal_common.h>
#include <mach/al_hal_udma.h>

/* *INDENT-OFF* */
#ifdef __cplusplus
extern "C" {
#endif
/* *INDENT-ON* */

/** M2M UDMA private data structure */
struct al_m2m_udma {
	char *name;
	void __iomem *m2s_regs_base;
	void __iomem *s2m_regs_base;
	uint8_t num_of_queues;
	struct al_udma	tx_udma;	/** the m2s component of the M2M UDMA */
	struct al_udma	rx_udma;	/** the s2m component of the M2M UDMA */
};

/** M2M UDMA parameters from upper layer */
struct al_m2m_udma_params {
	void __iomem *m2s_regs_base;
	void __iomem *s2m_regs_base;
	char *name; /** the upper layer must keep the string area */
	uint8_t num_of_queues;/** number of queues */
	uint8_t max_m2s_descs_per_pkt; /** maximum descriptors per m2s packet */
	uint8_t max_s2m_descs_per_pkt; /** maximum descriptors per s2m packet */
};

/**
 * initialize M2M UDMA
 *
 * @param m2m_udma m2m udma handle
 * @param params m2m udma parameters from upper layer
 *
 * @return 0 on success. -EINVAL otherwise.
 */
int al_m2m_udma_init(struct al_m2m_udma *m2m_udma,
		     struct al_m2m_udma_params *params);

/**
 * initialize the m2s(tx) and s2m(tx) udmas of the queue
 *
 * @param m2m_udma m2m udma handle
 * @param qid queue index
 * @param tx_params udma queue pramas fot the tx udma queue
 * @param rx_params udma queue pramas fot the rx udma queue
 *
 * @return 0 if no error found.
 *	   -EINVAL if the qid is out of range
 *	   -EIO if queue was already initialized
 */
int al_m2m_udma_q_init(struct al_m2m_udma *m2m_udma, uint32_t qid,
		       struct al_udma_q_params *tx_params,
		       struct al_udma_q_params *rx_params);
/**
 * Change the M2M UDMA state
 *
 * @param dma m2m udma handle
 * @param udma_state the target state
 *
 * @return 0
 */
int al_m2m_udma_state_set(struct al_m2m_udma *dma,
			  enum al_udma_state udma_state);

/**
 * Get udma handle of the tx or rx udma, this handle can be used to call misc
 * configuration functions defined at al_udma_config.h
 *
 * @param m2m_udma m2m udma handle
 * @param type tx or rx udma
 * @param udma the requested udma handle written to this pointer
 *
 * @return 0
 */
int al_m2m_udma_handle_get(struct al_m2m_udma *m2m_udma,
			enum al_udma_type type,
			struct al_udma **udma);

/* *INDENT-OFF* */
#ifdef __cplusplus
}
#endif
/* *INDENT-ON* */
#endif		/* __AL_HAL_M2M_UDMA_H__ */
/** @} end of M2M UDMA group */

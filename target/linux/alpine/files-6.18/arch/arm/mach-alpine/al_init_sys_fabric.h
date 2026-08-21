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
 * The fabric consists of CCU, SMMU(*2), GIC(*2), and NB registers.
 *
 * The fabric hal provides simple api for fabric initialization, and not a
 * complete coverage of the unit's functionality.
 * main objective: to keep consistent fabric initialization between
 * different environments. Only functions used by all environments added.
 *
 * GIC and SMMU are only handled when initializing the secure context.
 * Both internal and external gic are initialized in that aspect.
 */

#ifndef __AL_HAL_SYS_FABRIC_H__
#define __AL_HAL_SYS_FABRIC_H__

#include <mach/al_hal_common.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Initialize NB service regs
 *
 * @param nb_regs_address
 *	address of nb service registers
 * @param dev_ord_relax
 *	optimization: relax ordering between device-access reads and
 *	writes to different addresses.
 */
void al_nbservice_init(void __iomem *nb_regs_address,
		al_bool dev_ord_relax);

/**
 * Initialize CCU
 *
 * @param ccu_address
 *	address of ccu registers
 * @param iocc
 *	enable I/O cache coherency
 */
void al_ccu_init(void __iomem *ccu_address, al_bool iocc);

/**
 * Clear NB service regs settings
 *
 * @param nb_regs_address
 *	address of nb service registers
 */
void al_nbservice_clear_settings(void __iomem *nb_regs_address);

/**
 * Clear ccu settings
 *
 * @param ccu_address
 *	address of ccu registers
 */
void al_ccu_clear_settings(void __iomem *ccu_address);


#ifdef __cplusplus
}
#endif

#endif /* AL_HAL_SYS_FABRIC_H_ */

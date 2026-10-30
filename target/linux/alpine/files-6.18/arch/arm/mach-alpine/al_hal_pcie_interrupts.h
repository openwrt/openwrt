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

#ifndef _AL_HAL_PCIE_INTERRUPTS_H_
#define _AL_HAL_PCIE_INTERRUPTS_H_

#include <mach/al_hal_common.h>
#include <mach/al_hal_pcie.h>
#include <mach/al_hal_iofic.h>

/**
 * @defgroup group_pcie_interrupts PCIe interrupts
 * @ingroup grouppcie
 *  @{
 *  The PCIe interrupts HAL can be used to control PCIe unit interrupts.
 *  There are 3 groups of interrupts: app group A, app group B and AXI.
 *
 * @file   al_hal_pcie_interrupts.h
 *
 */

/** App group A interrupts mask - don't change */
enum al_pcie_app_int_grp_a {
	AL_PCIE_APP_INT_DEASSERT_INTD = AL_BIT(0),
	AL_PCIE_APP_INT_DEASSERT_INTC = AL_BIT(1),
	AL_PCIE_APP_INT_DEASSERT_INTB = AL_BIT(2),
	AL_PCIE_APP_INT_DEASSERT_INTA = AL_BIT(3),
	AL_PCIE_APP_INT_ASSERT_INTD = AL_BIT(4),
	AL_PCIE_APP_INT_ASSERT_INTC = AL_BIT(5),
	AL_PCIE_APP_INT_ASSERT_INTB = AL_BIT(6),
	AL_PCIE_APP_INT_ASSERT_INTA = AL_BIT(7),
	AL_PCIE_APP_INT_MSI_CNTR_RCV_INT = AL_BIT(8),
	AL_PCIE_APP_INT_MSI_TRNS_GNT = AL_BIT(9),
	AL_PCIE_APP_INT_SYS_ERR_RC = AL_BIT(10),
	AL_PCIE_APP_INT_FLR_PF_ACTIVE = AL_BIT(11),
	AL_PCIE_APP_INT_AER_RC_ERR = AL_BIT(12),
	AL_PCIE_APP_INT_AER_RC_ERR_MSI = AL_BIT(13),
	AL_PCIE_APP_INT_WAKE = AL_BIT(14),
	AL_PCIE_APP_INT_PME_INT = AL_BIT(15),
	AL_PCIE_APP_INT_PME_MSI = AL_BIT(16),
	AL_PCIE_APP_INT_HP_PME = AL_BIT(17),
	AL_PCIE_APP_INT_HP_INT = AL_BIT(18),
	AL_PCIE_APP_INT_HP_MSI = AL_BIT(19),
	AL_PCIE_APP_INT_VPD_INT = AL_BIT(20),
	AL_PCIE_APP_INT_LINK_DOWN = AL_BIT(21),
	AL_PCIE_APP_INT_PM_XTLH_BLOCK_TLP = AL_BIT(22),
	AL_PCIE_APP_INT_XMLH_LINK_UP = AL_BIT(23),
	AL_PCIE_APP_INT_RDLH_LINK_UP = AL_BIT(24),
	AL_PCIE_APP_INT_LTSSM_RCVRY_STATE = AL_BIT(25),
	AL_PCIE_APP_INT_CFG_WR = AL_BIT(26),
	AL_PCIE_APP_INT_CFG_EMUL = AL_BIT(31),
};

/**
 * @brief   Initialize and configure PCIe controller interrupts
 *
 * @param   pcie_port pcie port handle
 *
 * @return  0 if no error found
 */
int al_pcie_ints_config(struct al_pcie_port *pcie_port);

/**
 * Unmask PCIe app group a interrupts
 *
 * @param	pcie_port pcie port handle
 * @param	int_mask interrupt mask.
 */
void al_pcie_app_int_grp_a_unmask(struct al_pcie_port *pcie_port,
		uint32_t int_mask);

/**
 * Mask PCIe app group a interrupts
 *
 * @param	pcie_port pcie port handle
 * @param	int_mask interrupt mask.
 */
void al_pcie_app_int_grp_a_mask(struct al_pcie_port *pcie_port,
		uint32_t int_mask);

/**
 * Unmask PCIe app group b interrupts
 *
 * @param	pcie_port pcie port handle
 * @param	int_mask interrupt mask.
 */
void al_pcie_app_int_grp_b_unmask(struct al_pcie_port *pcie_port,
		uint32_t int_mask);

/**
 * Mask PCIe app group b interrupts
 *
 * @param	pcie_port pcie port handle
 * @param	int_mask interrupt mask.
 */
void al_pcie_app_int_grp_b_mask(struct al_pcie_port *pcie_port,
		uint32_t int_mask);

/**
 * Clear the PCIe app group a interrupt cause
 *
 * @param	pcie_port pcie port handle
 * @param	int_cause interrupt cause register bits to clear
 */
static INLINE void  al_pcie_app_int_grp_a_cause_clear(
		struct al_pcie_port *pcie_port,
		uint32_t int_cause)
{
	al_iofic_clear_cause(pcie_port->app_int_grp_a_base, 0, int_cause);
}

/**
 * Read PCIe app group a interrupt cause
 *
 * @param	pcie_port pcie port handle
 * @return	interrupt cause mask
 */
static INLINE uint32_t  al_pcie_app_int_grp_a_cause_read(
		struct al_pcie_port *pcie_port)
{
	return al_iofic_read_cause(pcie_port->app_int_grp_a_base, 0);
}

#endif
/** @} end of group_pcie_interrupts group */

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
 * @addtogroup groupddr
 *
 *  @{
 * @file   al_hal_ddr_utils.h
 *
 */

#ifndef __AL_HAL_DDR_UTILS_H__
#define __AL_HAL_DDR_UTILS_H__

#include <mach/al_hal_common.h>
#include <mach/al_hal_reg_utils.h>
#include "al_hal_ddr_ctrl_regs.h"
#include "al_hal_ddr_phy_regs.h"

/* *INDENT-OFF* */
#ifdef __cplusplus
extern "C" {
#endif
/* *INDENT-ON* */

/* Default timeout for register polling operations */
#define DEFAULT_TIMEOUT		5000

#if 0	/* Check if masked required */
#define _al_reg_write32_masked(reg, mask, data) \
	__al_reg_write32_masked(__LINE__, reg, mask, data)

#define ___al_reg_write32_masked(reg, mask, data) \
	____al_reg_write32_masked(__LINE__, reg, mask, data)

void __al_reg_write32_masked(
	int line,
	uint32_t *reg,
	uint32_t mask,
	uint32_t data)
{
	if (!(al_reg_read32(reg) & ~mask))
		al_info("%d can be non masked!\n", line);
	al_reg_write32_masked(reg, mask, data);
}

void ____al_reg_write32_masked(
	int line,
	uint32_t *reg,
	uint32_t mask,
	uint32_t data)
{
	if (al_reg_read32(reg) & ~mask)
		al_info("%d can not be non masked!\n", line);
	al_reg_write32_masked(reg, mask, data);
}
#else
#define _al_reg_write32_masked(reg, mask, data) \
	al_reg_write32_masked(reg, mask, data)

static inline void ___al_reg_write32_masked(
	uint32_t *reg,
	uint32_t mask __attribute__((__unused__)),
	uint32_t data)
{
	al_reg_write32(reg, data);
}
#endif

/*******************************************************************************
 ******************************************************************************/
static int al_ddr_reg_poll32(
	uint32_t __iomem	*reg,
	uint32_t		mask,
	uint32_t		data,
	unsigned int		timeout)
{
	while ((al_reg_read32(reg) & mask) != data) {
		if (timeout) {
			al_udelay(1);
			timeout--;
		} else {
			return -ETIME;
		}
	}

	return 0;
}

/*******************************************************************************
 ******************************************************************************/
/* Wait for controller normal operating mode */
static int al_ddr_ctrl_wait_for_normal_operating_mode(
	struct al_dwc_ddr_umctl2_regs __iomem *ctrl_regs)
{
	int err;

	err = al_ddr_reg_poll32(
		&ctrl_regs->stat,
		DWC_DDR_UMCTL2_REGS_STAT_OPERATING_MODE_MASK,
		DWC_DDR_UMCTL2_REGS_STAT_OPERATING_MODE_NORMAL,
		DEFAULT_TIMEOUT);

	if (err) {
		al_err("%s: al_ddr_reg_poll32 failed!\n", __func__);
		return err;
	}

	return 0;
}

/*******************************************************************************
 ******************************************************************************/
/**
 * Disabling VT calculation
 * VP calculation must be disabled during writes to the delay line registers
 */
static int al_ddr_phy_vt_calc_disable(
	struct al_ddr_phy_regs __iomem *phy_regs)
{
	int err;

	_al_reg_write32_masked(
		&phy_regs->PGCR[1],
		DWC_DDR_PHY_REGS_PGCR1_INHVT,
		DWC_DDR_PHY_REGS_PGCR1_INHVT);

	err = al_ddr_reg_poll32(
		&phy_regs->PGSR[1],
		DWC_DDR_PHY_REGS_PGSR1_VTSTOP,
		DWC_DDR_PHY_REGS_PGSR1_VTSTOP,
		DEFAULT_TIMEOUT);

	if (err) {
		al_err("%s: al_ddr_reg_poll32 failed!\n", __func__);
		return err;
	}

	return 0;
}

/*******************************************************************************
 ******************************************************************************/
/* Enabling VT calculation */
static void al_ddr_phy_vt_calc_enable(
	struct al_ddr_phy_regs __iomem *phy_regs)
{
	_al_reg_write32_masked(
		&phy_regs->PGCR[1],
		DWC_DDR_PHY_REGS_PGCR1_INHVT,
		0);
}

/*******************************************************************************
 ******************************************************************************/
/* Stop DDR controller access to the PHY*/
static inline void al_ddr_ctrl_stop(
	void __iomem		*ddr_ctrl_regs_base)
{
	struct al_dwc_ddr_umctl2_regs __iomem *ctrl_regs =
		&((struct al_ddr_ctrl_regs __iomem *)
		ddr_ctrl_regs_base)->umctl2_regs;

	___al_reg_write32_masked(
		&ctrl_regs->dfimisc,
		DWC_DDR_UMCTL2_REGS_DFIMISC_DFI_INIT_COMPLETE_EN,
		0);

	/* Stop controller refresh and ZQ calibration commands */
	_al_reg_write32_masked(
		&ctrl_regs->rfshctl3,
		DWC_DDR_UMCTL2_REGS_RFSHCTL3_DIS_AUTO_REFRESH,
		DWC_DDR_UMCTL2_REGS_RFSHCTL3_DIS_AUTO_REFRESH);

	_al_reg_write32_masked(
		&ctrl_regs->zqctl0,
		DWC_DDR_UMCTL2_REGS_ZQCTL0_DIS_AUTO_ZQ,
		DWC_DDR_UMCTL2_REGS_ZQCTL0_DIS_AUTO_ZQ);

	al_data_memory_barrier();
}

/*******************************************************************************
 ******************************************************************************/
/* Resume DDR controller access to the PHY*/
static inline void al_ddr_ctrl_resume(
	void __iomem		*ddr_ctrl_regs_base)
{
	struct al_dwc_ddr_umctl2_regs __iomem *ctrl_regs =
		&((struct al_ddr_ctrl_regs __iomem *)
		ddr_ctrl_regs_base)->umctl2_regs;

	___al_reg_write32_masked(
		&ctrl_regs->dfimisc,
		DWC_DDR_UMCTL2_REGS_DFIMISC_DFI_INIT_COMPLETE_EN,
		DWC_DDR_UMCTL2_REGS_DFIMISC_DFI_INIT_COMPLETE_EN);

	/* Resume controller refresh and ZQ calibration commands */
	_al_reg_write32_masked(
		&ctrl_regs->rfshctl3,
		DWC_DDR_UMCTL2_REGS_RFSHCTL3_DIS_AUTO_REFRESH,
		0);

	_al_reg_write32_masked(
		&ctrl_regs->zqctl0,
		DWC_DDR_UMCTL2_REGS_ZQCTL0_DIS_AUTO_ZQ,
		0);

	al_data_memory_barrier();

	al_ddr_ctrl_wait_for_normal_operating_mode(ctrl_regs);
}

/* *INDENT-OFF* */
#ifdef __cplusplus
}
#endif
/* *INDENT-ON* */
/** @} end of DDR group */
#endif


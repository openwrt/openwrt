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
 * @file   al_hal_ddr.c
 *
 * @brief  DDR controller & PHY HAL driver
 *
 */

#include <mach/al_hal_nb_regs.h>
#include "al_hal_ddr.h"
#include "al_hal_ddr_ctrl_regs.h"
#include "al_hal_ddr_phy_regs.h"
#include "al_hal_ddr_utils.h"

/* Wait for PHY BIST to be done */
static int al_ddr_phy_wait_for_bist_done(
	struct al_ddr_phy_regs __iomem *phy_regs);

/*******************************************************************************
 ******************************************************************************/
int al_ddr_phy_datx_bist(
	void __iomem			*ddr_ctrl_regs_base,
	void __iomem			*ddr_phy_regs_base,
	struct al_ddr_bist_params	*params)
{
	int i;
	int err;

	struct al_ddr_phy_regs __iomem *phy_regs =
		(struct al_ddr_phy_regs __iomem *)ddr_phy_regs_base;

	uint32_t mode =
		(params->mode == AL_DDR_BIST_MODE_LOOPBACK) ?
		DWC_DDR_PHY_REGS_BISTRR_BMODE_LOOPBACK :
		DWC_DDR_PHY_REGS_BISTRR_BMODE_DRAM;

	uint32_t pattern =
		(params->pat == AL_DDR_BIST_PATTERN_WALK_0) ?
		DWC_DDR_PHY_REGS_BISTRR_BDPAT_WALKING_0 :
		(params->pat == AL_DDR_BIST_PATTERN_WALK_1) ?
		DWC_DDR_PHY_REGS_BISTRR_BDPAT_WALKING_1 :
		(params->pat == AL_DDR_BIST_PATTERN_LFSR) ?
		DWC_DDR_PHY_REGS_BISTRR_BDPAT_LFSR :
		DWC_DDR_PHY_REGS_BISTRR_BDPAT_USER;

	al_ddr_phy_vt_calc_disable(phy_regs);

	al_ddr_ctrl_stop(ddr_ctrl_regs_base);

	/**
	 * Init BIST mode of operation
	 */

	/* BISTUDPR */
	_al_reg_write32_masked(
		&phy_regs->BISTUDPR,
		DWC_DDR_PHY_REGS_BISTUDPR_BUDP0_MASK |
		DWC_DDR_PHY_REGS_BISTUDPR_BUDP1_MASK,
		DWC_DDR_PHY_REGS_BISTUDPR_BUDP0(params->user_pat_even) |
		DWC_DDR_PHY_REGS_BISTUDPR_BUDP1(params->user_pat_odd));

	/* BISTWCR */
	_al_reg_write32_masked(
		&phy_regs->BISTWCR,
		DWC_DDR_PHY_REGS_BISTWCR_BWCNT_MASK,
		DWC_DDR_PHY_REGS_BISTWCR_BWCNT(params->wc));

	/* BISTAR0 */
	_al_reg_write32_masked(
		&phy_regs->BISTAR[0],
		DWC_DDR_PHY_REGS_BISTAR0_BCOL_MASK |
		DWC_DDR_PHY_REGS_BISTAR0_BROW_MASK |
		DWC_DDR_PHY_REGS_BISTAR0_BBANK_MASK,
		DWC_DDR_PHY_REGS_BISTAR0_BCOL(params->col_min) |
		DWC_DDR_PHY_REGS_BISTAR0_BROW(params->row_min) |
		DWC_DDR_PHY_REGS_BISTAR0_BBANK(params->bank_min));

	/* BISTAR1 */
	_al_reg_write32_masked(
		&phy_regs->BISTAR[1],
		DWC_DDR_PHY_REGS_BISTAR1_BRANK_MASK |
		DWC_DDR_PHY_REGS_BISTAR1_BMRANK_MASK |
		DWC_DDR_PHY_REGS_BISTAR1_BAINC_MASK,
		DWC_DDR_PHY_REGS_BISTAR1_BRANK(params->rank_min) |
		DWC_DDR_PHY_REGS_BISTAR1_BMRANK(params->rank_max) |
		DWC_DDR_PHY_REGS_BISTAR1_BAINC(params->inc));

	/* BISTAR2 */
	_al_reg_write32_masked(
		&phy_regs->BISTAR[2],
		DWC_DDR_PHY_REGS_BISTAR2_BMCOL_MASK |
		DWC_DDR_PHY_REGS_BISTAR2_BMROW_MASK |
		DWC_DDR_PHY_REGS_BISTAR2_BMBANK_MASK,
		DWC_DDR_PHY_REGS_BISTAR2_BMCOL(params->col_max) |
		DWC_DDR_PHY_REGS_BISTAR2_BMROW(params->row_max) |
		DWC_DDR_PHY_REGS_BISTAR2_BMBANK(params->bank_max));

	/* Run DATX8 BIST */
	for (i = 0; i < AL_DDR_PHY_NUM_BYTE_LANES; i++) {
		if (!params->active_byte_lanes[i])
			continue;

		/* Reset status */
		_al_reg_write32_masked(
			&phy_regs->BISTRR,
			DWC_DDR_PHY_REGS_BISTRR_BINST_MASK,
			DWC_DDR_PHY_REGS_BISTRR_BINST_RESET);

		/* Run BIST */
		_al_reg_write32_masked(
			&phy_regs->BISTRR,
			DWC_DDR_PHY_REGS_BISTRR_BINST_MASK |
			DWC_DDR_PHY_REGS_BISTRR_BMODE_MASK |
			DWC_DDR_PHY_REGS_BISTRR_BDXEN |
			DWC_DDR_PHY_REGS_BISTRR_BACEN |
			DWC_DDR_PHY_REGS_BISTRR_BDPAT_MASK |
			DWC_DDR_PHY_REGS_BISTRR_BDXSEL_MASK,
			DWC_DDR_PHY_REGS_BISTRR_BINST_RUN |
			mode |
			DWC_DDR_PHY_REGS_BISTRR_BDXEN |
			pattern |
			DWC_DDR_PHY_REGS_BISTRR_BDXSEL(i));

		al_data_memory_barrier();

		/* Read BISTGSR for BIST done */
		err = al_ddr_phy_wait_for_bist_done(phy_regs);
		if (err) {
			al_err(
				"%s:%d: al_ddr_phy_wait_for_bist_done failed "
				"(byte lane %d)!\n",
				__func__,
				__LINE__,
				i);
			return err;
		}
	}

	/* stop BIST */
	_al_reg_write32_masked(
		&phy_regs->BISTRR,
		DWC_DDR_PHY_REGS_BISTRR_BINST_MASK,
		DWC_DDR_PHY_REGS_BISTRR_BINST_NOP);

	/* PGCR3 - after BIST re-apply power down of unused DQs */
	_al_reg_write32_masked(
		&phy_regs->PGCR[3],
		DWC_DDR_PHY_REGS_PGCR3_GATEDXCTLCLK |
		DWC_DDR_PHY_REGS_PGCR3_GATEDXDDRCLK |
		DWC_DDR_PHY_REGS_PGCR3_GATEDXRDCLK,
		DWC_DDR_PHY_REGS_PGCR3_GATEDXCTLCLK |
		DWC_DDR_PHY_REGS_PGCR3_GATEDXDDRCLK |
		DWC_DDR_PHY_REGS_PGCR3_GATEDXRDCLK);

	al_ddr_phy_vt_calc_enable(phy_regs);

	al_ddr_ctrl_resume(ddr_ctrl_regs_base);

	return 0;
}

/*******************************************************************************
 ******************************************************************************/
int al_ddr_phy_ac_bist(
	void __iomem		*ddr_phy_regs_base,
	enum al_ddr_bist_pat	pat)
{
	int err;

	struct al_ddr_phy_regs __iomem *phy_regs =
		(struct al_ddr_phy_regs __iomem *)ddr_phy_regs_base;

	uint32_t pattern =
		(pat == AL_DDR_BIST_PATTERN_WALK_0) ?
		DWC_DDR_PHY_REGS_BISTRR_BDPAT_WALKING_0 :
		(pat == AL_DDR_BIST_PATTERN_WALK_1) ?
		DWC_DDR_PHY_REGS_BISTRR_BDPAT_WALKING_1 :
		DWC_DDR_PHY_REGS_BISTRR_BDPAT_LFSR;

	/* Run AC BIST */
	/* Reset status */
	_al_reg_write32_masked(
		&phy_regs->BISTRR,
		DWC_DDR_PHY_REGS_BISTRR_BINST_MASK,
		DWC_DDR_PHY_REGS_BISTRR_BINST_RESET);

	/* Power up I/O receivers */
	_al_reg_write32_masked(
		&phy_regs->ACIOCR[0],
		DWC_DDR_PHY_REGS_ACIOCR0_ACPDR |
		DWC_DDR_PHY_REGS_ACIOCR0_CKPDR_MASK |
		DWC_DDR_PHY_REGS_ACIOCR0_RANKPDR_MASK |
		DWC_DDR_PHY_REGS_ACIOCR0_RSTPDR,
		0);

	/* Loopback before buffer in I/O */
	al_reg_write32_masked(
		&phy_regs->PGCR[1],
		DWC_DDR_PHY_REGS_PGCR1_IOLB,
		DWC_DDR_PHY_REGS_PGCR1_IOLB);

	/* Run BIST */
	_al_reg_write32_masked(
		&phy_regs->BISTRR,
		DWC_DDR_PHY_REGS_BISTRR_BINST_MASK |
		DWC_DDR_PHY_REGS_BISTRR_BMODE_MASK |
		DWC_DDR_PHY_REGS_BISTRR_BDXEN |
		DWC_DDR_PHY_REGS_BISTRR_BACEN |
		DWC_DDR_PHY_REGS_BISTRR_BDPAT_MASK,
		DWC_DDR_PHY_REGS_BISTRR_BINST_RUN |
		DWC_DDR_PHY_REGS_BISTRR_BMODE_LOOPBACK |
		DWC_DDR_PHY_REGS_BISTRR_BACEN |
		pattern);

	al_data_memory_barrier();

	/* Read BISTGSR for BIST done */
	err = al_ddr_phy_wait_for_bist_done(phy_regs);
	if (err) {
		al_err(
			"%s:%d: al_ddr_phy_wait_for_bist_done failed!\n",
			__func__,
			__LINE__);
		return err;
	}

	/* Power down I/O receivers */
	_al_reg_write32_masked(
		&phy_regs->ACIOCR[0],
		DWC_DDR_PHY_REGS_ACIOCR0_ACPDR |
		DWC_DDR_PHY_REGS_ACIOCR0_CKPDR_MASK |
		DWC_DDR_PHY_REGS_ACIOCR0_RANKPDR_MASK |
		DWC_DDR_PHY_REGS_ACIOCR0_RSTPDR,
		DWC_DDR_PHY_REGS_ACIOCR0_ACPDR |
		DWC_DDR_PHY_REGS_ACIOCR0_CKPDR_MASK |
		DWC_DDR_PHY_REGS_ACIOCR0_RANKPDR_MASK |
		DWC_DDR_PHY_REGS_ACIOCR0_RSTPDR);

	/* stop BIST */
	_al_reg_write32_masked(
		&phy_regs->BISTRR,
		DWC_DDR_PHY_REGS_BISTRR_BINST_MASK,
		DWC_DDR_PHY_REGS_BISTRR_BINST_NOP);

	return 0;
}

/*******************************************************************************
 ******************************************************************************/
int al_ddr_power_mode_set(
	void __iomem		*ddr_ctrl_regs_base,
	enum al_ddr_power_mode	power_mode,
	unsigned int		timer_x32)
{
	struct al_dwc_ddr_umctl2_regs __iomem *ctrl_regs =
		&((struct al_ddr_ctrl_regs __iomem *)
		ddr_ctrl_regs_base)->umctl2_regs;

	uint32_t mode =
		(power_mode == AL_DDR_POWERMODE_SELF_REFRESH) ?
		DWC_DDR_UMCTL2_REGS_PWRCTL_SELFREF_EN :
		(power_mode == AL_DDR_POWERMODE_POWER_DOWN) ?
		DWC_DDR_UMCTL2_REGS_PWRCTL_POWERDOWN_EN :
		0;

	/*
	 * Addressing RMN: 1037
	 *
	 * RMN description:
	 * In the current logic, it is possible for DRAM Read data and/or
	 * Write data to be active while/after one of the following occurs:
	 * Power Down Entry (PDE)
	 * Self Refresh Entry (SRE)
	 * This would violate the memory protocol DDR3 which require
	 * "no data bursts are in progress" when the above commands occur.
	 * Software flow:
	 * For violations related to PDE, issue can be avoided by ensuring
	 * that timer_x32>1. For violations related to SRE, ensure that
	 * AL_DDR_POWERMODE_SELF_REFRESH is set only after all Read data has
	 * been returned on your application interface and all write data has
	 * reached the DRAM.
	 */

	if (timer_x32 <= 1) {
		al_err(
			"%s:%d: power mode timer must be greater than 1!\n",
			__func__,
			__LINE__);
		return -EIO;
	}

	al_reg_write32(&ctrl_regs->pwrtmg,
		timer_x32 << DWC_DDR_UMCTL2_REGS_PWRTMG_POWERDOWN_TO_X32_SHIFT);

	al_reg_write32(&ctrl_regs->pwrctl, mode);

	return 0;
}

/*******************************************************************************
 ******************************************************************************/
enum al_ddr_operating_mode al_ddr_operating_mode_get(
	void __iomem			*ddr_ctrl_regs_base)
{
	struct al_dwc_ddr_umctl2_regs __iomem *ctrl_regs =
		&((struct al_ddr_ctrl_regs __iomem *)
		ddr_ctrl_regs_base)->umctl2_regs;
	uint32_t reg_val;
	enum al_ddr_operating_mode operating_mode;

	reg_val = al_reg_read32(&ctrl_regs->stat);
	reg_val &= DWC_DDR_UMCTL2_REGS_STAT_OPERATING_MODE_MASK;

	operating_mode =
		(reg_val == DWC_DDR_UMCTL2_REGS_STAT_OPERATING_MODE_INIT) ?
		AL_DDR_OPERATING_MODE_INIT :
		(reg_val == DWC_DDR_UMCTL2_REGS_STAT_OPERATING_MODE_NORMAL) ?
		AL_DDR_OPERATING_MODE_NORMAL :
		(reg_val == DWC_DDR_UMCTL2_REGS_STAT_OPERATING_MODE_POWER_DOWN) ?
		AL_DDR_OPERATING_MODE_POWER_DOWN :
		AL_DDR_OPERATING_MODE_SELF_REFRESH;

	return operating_mode;
}

/*******************************************************************************
 ******************************************************************************/
static int al_ddr_phy_wait_for_bist_done(
	struct al_ddr_phy_regs __iomem *phy_regs)
{
	int err;
	uint32_t reg_val;

	err = al_ddr_reg_poll32(
		&phy_regs->BISTGSR,
		DWC_DDR_PHY_REGS_BISTGSR_BDONE,
		DWC_DDR_PHY_REGS_BISTGSR_BDONE,
		DEFAULT_TIMEOUT);

	if (err) {
		al_err("%s: al_ddr_reg_poll32 failed!\n", __func__);
		return err;
	}

	reg_val = al_reg_read32(&phy_regs->BISTGSR);
	/* Make sure no bist errors */
	if (reg_val &
		(DWC_DDR_PHY_REGS_BISTGSR_BACERR |
		DWC_DDR_PHY_REGS_BISTGSR_BDXERR)) {
		al_err("%s: PHY bist error (BISTGSR = %X)!\n",
			__func__, reg_val);
		al_dbg("%s: (BISTWER = %X)!\n",
			__func__, al_reg_read32(&phy_regs->BISTWER));
		al_dbg("%s: (BISTBER2 = %X)!\n",
			__func__, al_reg_read32(&phy_regs->BISTBER[2]));
		al_dbg("%s: (BISTBER3 = %X)!\n",
			__func__, al_reg_read32(&phy_regs->BISTBER[3]));
		al_dbg("%s: (BISTWCSR = %X)!\n",
			__func__, al_reg_read32(&phy_regs->BISTWCSR));
		al_dbg("%s: (BISTFWR2 = %X)!\n",
			__func__, al_reg_read32(&phy_regs->BISTFWR[2]));
		return -EIO;
	}

	return 0;
}

/*******************************************************************************
 ******************************************************************************/
unsigned int al_ddr_active_ranks_get(
	void __iomem		*ddr_ctrl_regs_base)
{
	struct al_dwc_ddr_umctl2_regs __iomem *ctrl_regs =
		&((struct al_ddr_ctrl_regs __iomem *)
		ddr_ctrl_regs_base)->umctl2_regs;
	uint32_t mstr_val;
	unsigned int active_ranks = 0;

	mstr_val = al_reg_read32(&ctrl_regs->mstr);
	mstr_val &= DWC_DDR_UMCTL2_REGS_MSTR_ACTIVE_RANKS_MASK;
	mstr_val >>= DWC_DDR_UMCTL2_REGS_MSTR_ACTIVE_RANKS_SHIFT;

	/* each bit on mstr_val is corresponding to an available rank */
	while(mstr_val > 0) {
		active_ranks += 1;
		mstr_val >>= 1;
	}

	return active_ranks;
}

/*******************************************************************************
 ******************************************************************************/
int al_ddr_ecc_status_get(
	void __iomem			*ddr_ctrl_regs_base,
	struct al_ddr_ecc_status	*corr_status,
	struct al_ddr_ecc_status	*uncorr_status)
{
	uint32_t reg_val;

	struct al_dwc_ddr_umctl2_regs __iomem *ctrl_regs =
		&((struct al_ddr_ctrl_regs __iomem *)
		ddr_ctrl_regs_base)->umctl2_regs;

	/* Correctable status */
	if (corr_status) {
		reg_val = al_reg_read32(&ctrl_regs->eccstat);
		corr_status->ecc_corrected_bit_num = (reg_val &
			DWC_DDR_UMCTL2_REGS_ECCSTAT_ECC_CORRECTED_BIT_NUM_MASK)
			>> DWC_DDR_UMCTL2_REGS_ECCSTAT_ECC_CORRECTED_BIT_NUM_SHIFT;

		reg_val = al_reg_read32(&ctrl_regs->eccerrcnt);
		corr_status->err_cnt = (reg_val &
			DWC_DDR_UMCTL2_REGS_ECCERRCNT_ECC_CORR_ERR_CNT_MASK)
			>> DWC_DDR_UMCTL2_REGS_ECCERRCNT_ECC_CORR_ERR_CNT_SHIFT;

		reg_val = al_reg_read32(&ctrl_regs->ecccaddr0);
		corr_status->row = (reg_val &
			DWC_DDR_UMCTL2_REGS_ECCCADDR0_ECC_CORR_ROW_MASK)
			>> DWC_DDR_UMCTL2_REGS_ECCCADDR0_ECC_CORR_ROW_SHIFT;
		corr_status->rank = (reg_val &
			DWC_DDR_UMCTL2_REGS_ECCCADDR0_ECC_CORR_RANK_MASK)
			>> DWC_DDR_UMCTL2_REGS_ECCCADDR0_ECC_CORR_RANK_SHIFT;

		reg_val = al_reg_read32(&ctrl_regs->ecccaddr1);
		corr_status->bank = (reg_val &
			DWC_DDR_UMCTL2_REGS_ECCCADDR1_ECC_CORR_BANK_MASK)
			>> DWC_DDR_UMCTL2_REGS_ECCCADDR1_ECC_CORR_BANK_SHIFT;
		corr_status->col = (reg_val &
			DWC_DDR_UMCTL2_REGS_ECCCADDR1_ECC_CORR_COL_MASK)
			>> DWC_DDR_UMCTL2_REGS_ECCCADDR1_ECC_CORR_COL_SHIFT;

		corr_status->syndromes_31_0 = al_reg_read32(&ctrl_regs->ecccsyn0);
		corr_status->syndromes_63_32 = al_reg_read32(&ctrl_regs->ecccsyn1);
		reg_val = al_reg_read32(&ctrl_regs->ecccsyn2);
		corr_status->syndromes_ecc = (reg_val &
			DWC_DDR_UMCTL2_REGS_ECCCSYN2_ECC_CORR_SYNDROMES_71_64_MASK)
			>> DWC_DDR_UMCTL2_REGS_ECCCSYN2_ECC_CORR_SYNDROMES_71_64_SHIFT;

		corr_status->corr_bit_mask_31_0 =
			al_reg_read32(&ctrl_regs->eccbitmask0);
		corr_status->corr_bit_mask_63_32 =
			al_reg_read32(&ctrl_regs->eccbitmask1);
		reg_val = al_reg_read32(&ctrl_regs->eccbitmask2);
		corr_status->corr_bit_mask_ecc = (reg_val &
			DWC_DDR_UMCTL2_REGS_ECCBITMASK2_ECC_CORR_BIT_MASK_71_64_MASK) >>
			DWC_DDR_UMCTL2_REGS_ECCBITMASK2_ECC_CORR_BIT_MASK_71_64_SHIFT;
	}

	/* Uncorrectable status */
	if (uncorr_status) {
		reg_val = al_reg_read32(&ctrl_regs->eccerrcnt);
		uncorr_status->err_cnt = (reg_val &
			DWC_DDR_UMCTL2_REGS_ECCERRCNT_ECC_UNCORR_ERR_CNT_MASK)
			>> DWC_DDR_UMCTL2_REGS_ECCERRCNT_ECC_UNCORR_ERR_CNT_SHIFT;

		reg_val = al_reg_read32(&ctrl_regs->eccuaddr0);
		uncorr_status->row = (reg_val &
			DWC_DDR_UMCTL2_REGS_ECCUADDR0_ECC_UNCORR_ROW_MASK)
			>> DWC_DDR_UMCTL2_REGS_ECCUADDR0_ECC_UNCORR_ROW_SHIFT;
		uncorr_status->rank = (reg_val &
			DWC_DDR_UMCTL2_REGS_ECCUADDR0_ECC_UNCORR_RANK_MASK)
			>> DWC_DDR_UMCTL2_REGS_ECCUADDR0_ECC_UNCORR_RANK_SHIFT;

		reg_val = al_reg_read32(&ctrl_regs->eccuaddr1);
		uncorr_status->bank = (reg_val &
			DWC_DDR_UMCTL2_REGS_ECCUADDR1_ECC_UNCORR_BANK_MASK)
			>> DWC_DDR_UMCTL2_REGS_ECCUADDR1_ECC_UNCORR_BANK_SHIFT;
		uncorr_status->col = (reg_val &
			DWC_DDR_UMCTL2_REGS_ECCUADDR1_ECC_UNCORR_COL_MASK)
			>> DWC_DDR_UMCTL2_REGS_ECCUADDR1_ECC_UNCORR_COL_SHIFT;

		uncorr_status->syndromes_31_0 = al_reg_read32(&ctrl_regs->eccusyn0);
		uncorr_status->syndromes_63_32 = al_reg_read32(&ctrl_regs->eccusyn1);
		reg_val = al_reg_read32(&ctrl_regs->eccusyn2);
		uncorr_status->syndromes_ecc = (reg_val &
			DWC_DDR_UMCTL2_REGS_ECCUSYN2_ECC_UNCORR_SYNDROMES_71_64_MASK) >>
			DWC_DDR_UMCTL2_REGS_ECCUSYN2_ECC_UNCORR_SYNDROMES_71_64_SHIFT;
	}

	return 0;
}

/*******************************************************************************
 ******************************************************************************/
void al_ddr_ecc_cfg_get(
	void __iomem			*ddr_ctrl_regs_base,
	struct al_ddr_ecc_cfg		*cfg)
{
	struct al_dwc_ddr_umctl2_regs __iomem *ctrl_regs =
		&((struct al_ddr_ctrl_regs __iomem *)
		ddr_ctrl_regs_base)->umctl2_regs;
	uint32_t cfg_val;

	cfg_val = al_reg_read32(&ctrl_regs->ecccfg0);

	cfg->ecc_enabled =
		((cfg_val & DWC_DDR_UMCTL2_REGS_ECCCFG0_ECC_MODE_MASK) ==
		DWC_DDR_UMCTL2_REGS_ECCCFG0_ECC_MODE_DIS) ? AL_FALSE : AL_TRUE;

	/* dis_scrub is valid only when ecc mode is enabled */
	if (cfg->ecc_enabled)
		cfg->scrub_enabled =
			(cfg_val & DWC_DDR_UMCTL2_REGS_ECCCFG0_DIS_SCRUB) ?
			AL_FALSE : AL_TRUE;
}

/*******************************************************************************
 ******************************************************************************/
int al_ddr_ecc_corr_count_clear(
	void __iomem		*ddr_ctrl_regs_base)
{
	struct al_dwc_ddr_umctl2_regs __iomem *ctrl_regs =
		&((struct al_ddr_ctrl_regs __iomem *)
		ddr_ctrl_regs_base)->umctl2_regs;

	al_reg_write32(&ctrl_regs->eccclr,
			DWC_DDR_UMCTL2_REGS_ECCCLR_ECC_CLR_CORR_ERR_CNT);

	return 0;
}

/*******************************************************************************
 ******************************************************************************/
int al_ddr_ecc_corr_int_clear(
	void __iomem		*nb_regs_base,
	void __iomem		*ddr_ctrl_regs_base)
{
	struct al_dwc_ddr_umctl2_regs __iomem *ctrl_regs =
		&((struct al_ddr_ctrl_regs __iomem *)
		ddr_ctrl_regs_base)->umctl2_regs;

	al_reg_write32(&ctrl_regs->eccclr,
			DWC_DDR_UMCTL2_REGS_ECCCLR_ECC_CLR_CORR_ERR);

	if (nb_regs_base) {
		struct al_nb_regs __iomem *nb_regs;

		al_data_memory_barrier();

		nb_regs = (struct al_nb_regs __iomem *)nb_regs_base;
		al_reg_write32(&nb_regs->global.nb_int_cause,
				~NB_GLOBAL_NB_INT_CAUSE_MCTL_ECC_CORR_ERR);
	}

	return 0;
}

/*******************************************************************************
 ******************************************************************************/
int al_ddr_ecc_uncorr_count_clear(
	void __iomem		*ddr_ctrl_regs_base)
{
	struct al_dwc_ddr_umctl2_regs __iomem *ctrl_regs =
		&((struct al_ddr_ctrl_regs __iomem *)
		ddr_ctrl_regs_base)->umctl2_regs;

	al_reg_write32(&ctrl_regs->eccclr,
			DWC_DDR_UMCTL2_REGS_ECCCLR_ECC_CLR_UNCORR_ERR_CNT);

	return 0;
}

/*******************************************************************************
 ******************************************************************************/
int al_ddr_ecc_uncorr_int_clear(
	void __iomem		*nb_regs_base,
	void __iomem		*ddr_ctrl_regs_base)
{
	struct al_dwc_ddr_umctl2_regs __iomem *ctrl_regs =
		&((struct al_ddr_ctrl_regs __iomem *)
		ddr_ctrl_regs_base)->umctl2_regs;

	al_reg_write32(&ctrl_regs->eccclr,
			DWC_DDR_UMCTL2_REGS_ECCCLR_ECC_CLR_UNCORR_ERR);

	if (nb_regs_base) {
		struct al_nb_regs __iomem *nb_regs;

		al_data_memory_barrier();

		nb_regs = (struct al_nb_regs __iomem *)nb_regs_base;
		al_reg_write32(&nb_regs->global.nb_int_cause,
				~NB_GLOBAL_NB_INT_CAUSE_MCTL_ECC_UNCORR_ERR);
	}

	return 0;
}

/*******************************************************************************
 ******************************************************************************/
int al_ddr_ecc_data_poison_enable(
	void __iomem		*ddr_ctrl_regs_base,
	unsigned int		rank,
	unsigned int		bank,
	unsigned int		col,
	unsigned int		row)
{
	struct al_dwc_ddr_umctl2_regs __iomem *ctrl_regs =
		&((struct al_ddr_ctrl_regs __iomem *)
		ddr_ctrl_regs_base)->umctl2_regs;

	/* Set data poison address */
	al_reg_write32(&ctrl_regs->eccpoisonaddr0,
		(col <<
		 DWC_DDR_UMCTL2_REGS_ECCPOISONADDR0_ECC_POISON_COL_SHIFT) |
		(rank <<
		 DWC_DDR_UMCTL2_REGS_ECCPOISONADDR0_ECC_POISON_RANK_SHIFT));
	al_reg_write32(&ctrl_regs->eccpoisonaddr1,
		(row <<
		 DWC_DDR_UMCTL2_REGS_ECCPOISONADDR1_ECC_POISON_ROW_SHIFT) |
		(bank <<
		 DWC_DDR_UMCTL2_REGS_ECCPOISONADDR1_ECC_POISON_BANK_SHIFT));

	/* Enable data poisoning */
	al_reg_write32(&ctrl_regs->ecccfg1,
			DWC_DDR_UMCTL2_REGS_ECCCFG1_DATA_POISON);

	return 0;
}

/*******************************************************************************
 ******************************************************************************/
int al_ddr_ecc_data_poison_disable(
	void __iomem		*ddr_ctrl_regs_base)
{
	struct al_dwc_ddr_umctl2_regs __iomem *ctrl_regs =
		&((struct al_ddr_ctrl_regs __iomem *)
		ddr_ctrl_regs_base)->umctl2_regs;

	/* Disable data poisoning */
	al_reg_write32(&ctrl_regs->ecccfg1, 0);

	return 0;
}

/*******************************************************************************
 ******************************************************************************/
unsigned int al_ddr_parity_count_get(
	void __iomem		*ddr_ctrl_regs_base)
{
	uint32_t reg_val;
	unsigned int parity_count;

	struct al_dwc_ddr_umctl2_regs __iomem *ctrl_regs =
		&((struct al_ddr_ctrl_regs __iomem *)
		ddr_ctrl_regs_base)->umctl2_regs;

	reg_val = al_reg_read32(&ctrl_regs->parstat);
	parity_count = (reg_val &
		DWC_DDR_UMCTL2_REGS_PARSTAT_DFI_PARITY_ERR_CNT_MASK)
		>> DWC_DDR_UMCTL2_REGS_PARSTAT_DFI_PARITY_ERR_CNT_SHIFT;

	return parity_count;
}

/*******************************************************************************
 ******************************************************************************/
void al_ddr_parity_count_clear(
	void __iomem		*ddr_ctrl_regs_base)
{
	struct al_dwc_ddr_umctl2_regs __iomem *ctrl_regs =
		&((struct al_ddr_ctrl_regs __iomem *)
		ddr_ctrl_regs_base)->umctl2_regs;

	al_reg_write32_masked(&ctrl_regs->parctl,
			DWC_DDR_UMCTL2_REGS_PARCTL_DFI_PARITY_ERR_CNT_CLR,
			DWC_DDR_UMCTL2_REGS_PARCTL_DFI_PARITY_ERR_CNT_CLR);
}

/*******************************************************************************
 ******************************************************************************/
void al_ddr_parity_int_clear(
	void __iomem		*nb_regs_base,
	void __iomem		*ddr_ctrl_regs_base)
{
	struct al_dwc_ddr_umctl2_regs __iomem *ctrl_regs =
		&((struct al_ddr_ctrl_regs __iomem *)
		ddr_ctrl_regs_base)->umctl2_regs;

	struct al_nb_regs __iomem *nb_regs =
		(struct al_nb_regs __iomem *)nb_regs_base;

	al_reg_write32_masked(&ctrl_regs->parctl,
			DWC_DDR_UMCTL2_REGS_PARCTL_DFI_PARITY_ERR_INT_CLR,
			DWC_DDR_UMCTL2_REGS_PARCTL_DFI_PARITY_ERR_INT_CLR);

	al_data_memory_barrier();

	al_reg_write32(&nb_regs->global.nb_int_cause,
			~NB_GLOBAL_NB_INT_CAUSE_MCTL_PARITY_ERR);
}

/*******************************************************************************
 ******************************************************************************/
static int al_ddr_address_map_get(
	void __iomem			*ddr_ctrl_regs_base,
	struct al_ddr_addrmap	*addrmap)
{
	struct al_dwc_ddr_umctl2_regs __iomem *ctrl_regs =
		&((struct al_ddr_ctrl_regs __iomem *)
		ddr_ctrl_regs_base)->umctl2_regs;

	uint32_t reg_val;
	int addrmap_col_b2_11[10];
	int addrmap_bank_b0_2[3];
	int addrmap_row_b0_2_10[3];
	int addrmap_row_b11_15[5];
	int addrmap_cs_b0_1[2];

	unsigned int i;

	enum al_ddr_data_width data_width =
			al_ddr_data_width_get(ddr_ctrl_regs_base);

	/**
	 * CS address mapping
	 */
	reg_val = al_reg_read32(&ctrl_regs->addrmap0);
	addrmap_cs_b0_1[1] = (reg_val &
		DWC_DDR_UMCTL2_REGS_ADDRMAP0_ADDRMAP_CS_BIT1_MASK)
		>> DWC_DDR_UMCTL2_REGS_ADDRMAP0_ADDRMAP_CS_BIT1_SHIFT;
	addrmap_cs_b0_1[0] = (reg_val &
		DWC_DDR_UMCTL2_REGS_ADDRMAP0_ADDRMAP_CS_BIT0_MASK)
		>> DWC_DDR_UMCTL2_REGS_ADDRMAP0_ADDRMAP_CS_BIT0_SHIFT;

	/**
	 * Bank address mapping
	 */
	reg_val = al_reg_read32(&ctrl_regs->addrmap1);
	addrmap_bank_b0_2[2] = (reg_val &
		DWC_DDR_UMCTL2_REGS_ADDRMAP1_ADDRMAP_BANK_B2_MASK)
		>> DWC_DDR_UMCTL2_REGS_ADDRMAP1_ADDRMAP_BANK_B2_SHIFT;
	addrmap_bank_b0_2[1] = (reg_val &
		DWC_DDR_UMCTL2_REGS_ADDRMAP1_ADDRMAP_BANK_B1_MASK)
		>> DWC_DDR_UMCTL2_REGS_ADDRMAP1_ADDRMAP_BANK_B1_SHIFT;
	addrmap_bank_b0_2[0] = (reg_val &
		DWC_DDR_UMCTL2_REGS_ADDRMAP1_ADDRMAP_BANK_B0_MASK)
		>> DWC_DDR_UMCTL2_REGS_ADDRMAP1_ADDRMAP_BANK_B0_SHIFT;

	/**
	 * Column address mapping
	 */
	reg_val = al_reg_read32(&ctrl_regs->addrmap2);
	addrmap_col_b2_11[3] = (reg_val &
		DWC_DDR_UMCTL2_REGS_ADDRMAP2_ADDRMAP_COL_B5_MASK)
		>> DWC_DDR_UMCTL2_REGS_ADDRMAP2_ADDRMAP_COL_B5_SHIFT;
	addrmap_col_b2_11[2] = (reg_val &
		DWC_DDR_UMCTL2_REGS_ADDRMAP2_ADDRMAP_COL_B4_MASK)
		>> DWC_DDR_UMCTL2_REGS_ADDRMAP2_ADDRMAP_COL_B4_SHIFT;
	addrmap_col_b2_11[1] = (reg_val &
		DWC_DDR_UMCTL2_REGS_ADDRMAP2_ADDRMAP_COL_B3_MASK)
		>> DWC_DDR_UMCTL2_REGS_ADDRMAP2_ADDRMAP_COL_B3_SHIFT;
	addrmap_col_b2_11[0] = (reg_val &
		DWC_DDR_UMCTL2_REGS_ADDRMAP2_ADDRMAP_COL_B2_MASK)
		>> DWC_DDR_UMCTL2_REGS_ADDRMAP2_ADDRMAP_COL_B2_SHIFT;

	reg_val = al_reg_read32(&ctrl_regs->addrmap3);
	addrmap_col_b2_11[7] = (reg_val &
		DWC_DDR_UMCTL2_REGS_ADDRMAP3_ADDRMAP_COL_B9_MASK)
		>> DWC_DDR_UMCTL2_REGS_ADDRMAP3_ADDRMAP_COL_B9_SHIFT;
	addrmap_col_b2_11[6] = (reg_val &
		DWC_DDR_UMCTL2_REGS_ADDRMAP3_ADDRMAP_COL_B8_MASK)
		>> DWC_DDR_UMCTL2_REGS_ADDRMAP3_ADDRMAP_COL_B8_SHIFT;
	addrmap_col_b2_11[5] = (reg_val &
		DWC_DDR_UMCTL2_REGS_ADDRMAP3_ADDRMAP_COL_B7_MASK)
		>> DWC_DDR_UMCTL2_REGS_ADDRMAP3_ADDRMAP_COL_B7_SHIFT;
	addrmap_col_b2_11[4] = (reg_val &
		DWC_DDR_UMCTL2_REGS_ADDRMAP3_ADDRMAP_COL_B6_MASK)
		>> DWC_DDR_UMCTL2_REGS_ADDRMAP3_ADDRMAP_COL_B6_SHIFT;

	reg_val = al_reg_read32(&ctrl_regs->addrmap4);
	addrmap_col_b2_11[9] = (reg_val &
		DWC_DDR_UMCTL2_REGS_ADDRMAP4_ADDRMAP_COL_B11_MASK)
		>> DWC_DDR_UMCTL2_REGS_ADDRMAP4_ADDRMAP_COL_B11_SHIFT;
	addrmap_col_b2_11[8] = (reg_val &
		DWC_DDR_UMCTL2_REGS_ADDRMAP4_ADDRMAP_COL_B10_MASK)
		>> DWC_DDR_UMCTL2_REGS_ADDRMAP4_ADDRMAP_COL_B10_SHIFT;

	/**
	 * Row address mapping
	 */
	reg_val = al_reg_read32(&ctrl_regs->addrmap5);
	addrmap_row_b11_15[0] = (reg_val &
		DWC_DDR_UMCTL2_REGS_ADDRMAP5_ADDRMAP_ROW_B11_MASK)
		>> DWC_DDR_UMCTL2_REGS_ADDRMAP5_ADDRMAP_ROW_B11_SHIFT;
	addrmap_row_b0_2_10[2] = (reg_val &
		DWC_DDR_UMCTL2_REGS_ADDRMAP5_ADDRMAP_ROW_B2_10_MASK)
		>> DWC_DDR_UMCTL2_REGS_ADDRMAP5_ADDRMAP_ROW_B2_10_SHIFT;
	addrmap_row_b0_2_10[1] = (reg_val &
		DWC_DDR_UMCTL2_REGS_ADDRMAP5_ADDRMAP_ROW_B1_MASK)
		>> DWC_DDR_UMCTL2_REGS_ADDRMAP5_ADDRMAP_ROW_B1_SHIFT;
	addrmap_row_b0_2_10[0] = (reg_val &
		DWC_DDR_UMCTL2_REGS_ADDRMAP5_ADDRMAP_ROW_B0_MASK)
		>> DWC_DDR_UMCTL2_REGS_ADDRMAP5_ADDRMAP_ROW_B0_SHIFT;

	reg_val = al_reg_read32(&ctrl_regs->addrmap6);
	addrmap_row_b11_15[4] = (reg_val &
		DWC_DDR_UMCTL2_REGS_ADDRMAP6_ADDRMAP_ROW_B15_MASK)
		>> DWC_DDR_UMCTL2_REGS_ADDRMAP6_ADDRMAP_ROW_B15_SHIFT;
	addrmap_row_b11_15[3] = (reg_val &
		DWC_DDR_UMCTL2_REGS_ADDRMAP6_ADDRMAP_ROW_B14_MASK)
		>> DWC_DDR_UMCTL2_REGS_ADDRMAP6_ADDRMAP_ROW_B14_SHIFT;
	addrmap_row_b11_15[2] = (reg_val &
		DWC_DDR_UMCTL2_REGS_ADDRMAP6_ADDRMAP_ROW_B13_MASK)
		>> DWC_DDR_UMCTL2_REGS_ADDRMAP6_ADDRMAP_ROW_B13_SHIFT;
	addrmap_row_b11_15[1] = (reg_val &
		DWC_DDR_UMCTL2_REGS_ADDRMAP6_ADDRMAP_ROW_B12_MASK)
		>> DWC_DDR_UMCTL2_REGS_ADDRMAP6_ADDRMAP_ROW_B12_SHIFT;

	/* Column */
	for (i = 0; i < (AL_ARR_SIZE(addrmap->col_b3_9_b11_13) - 1); i++) {
		int user_val;

		user_val = addrmap_col_b2_11[i];

		if (data_width == AL_DDR_DATA_WIDTH_64_BITS)
			addrmap->col_b3_9_b11_13[i] =
				(user_val == AL_DDR_ADDR_MAP_COL_DISABLED) ?
				AL_DDR_ADDRMAP_NC :
				(user_val + AL_DDR_ADDR_MAP_OFFSET +
				AL_DDR_ADDR_MAP_COL_2_BASE + i);
		else
			addrmap->col_b3_9_b11_13[i + 1] =
				(user_val == AL_DDR_ADDR_MAP_COL_DISABLED) ?
				AL_DDR_ADDRMAP_NC :
				(user_val + AL_DDR_ADDR_MAP_OFFSET +
				AL_DDR_ADDR_MAP_COL_2_BASE + i);
	}

	if (data_width == AL_DDR_DATA_WIDTH_64_BITS)
		addrmap->col_b3_9_b11_13[i] = AL_DDR_ADDRMAP_NC;
	if (data_width == AL_DDR_DATA_WIDTH_32_BITS)
		addrmap->col_b3_9_b11_13[0] = 5;

	/* Bank */
	for (i = 0; i < AL_ARR_SIZE(addrmap->bank_b0_2); i++) {
		int user_val = addrmap_bank_b0_2[i];

		addrmap->bank_b0_2[i] =
			(user_val == AL_DDR_ADDR_MAP_BANK_DISABLED) ?
			AL_DDR_ADDRMAP_NC :
			(user_val + AL_DDR_ADDR_MAP_OFFSET +
			 AL_DDR_ADDR_MAP_BANK_0_BASE + i);
	}

	/* CS */
	for (i = 0; i < AL_ARR_SIZE(addrmap->cs_b0_1); i++) {
		int user_val = addrmap_cs_b0_1[i];

		addrmap->cs_b0_1[i] =
			(user_val == AL_DDR_ADDR_MAP_CS_DISABLED) ?
			AL_DDR_ADDRMAP_NC :
			(user_val + AL_DDR_ADDR_MAP_OFFSET +
			 AL_DDR_ADDR_MAP_CS_0_BASE + i);
	}

	/* Row */
	for (i = 0; i < AL_ARR_SIZE(addrmap->row_b0_2_10); i++) {
		int user_val = addrmap_row_b0_2_10[i];

		addrmap->row_b0_2_10[i] =
			(user_val == AL_DDR_ADDR_MAP_ROW_DISABLED) ?
			AL_DDR_ADDRMAP_NC :
			(user_val + AL_DDR_ADDR_MAP_OFFSET +
			 AL_DDR_ADDR_MAP_ROW_0_BASE + i);
	}

	for (i = 0; i < AL_ARR_SIZE(addrmap->row_b11_15); i++) {
		int user_val = addrmap_row_b11_15[i];

		addrmap->row_b11_15[i] =
			(user_val == AL_DDR_ADDR_MAP_ROW_DISABLED) ?
			AL_DDR_ADDRMAP_NC :
			(user_val + AL_DDR_ADDR_MAP_OFFSET +
			 AL_DDR_ADDR_MAP_ROW_11_BASE + i);
	}

	return 0;
}

/*******************************************************************************
 ******************************************************************************/
enum al_ddr_data_width al_ddr_data_width_get(
	void __iomem		*ddr_ctrl_regs_base)
{
	struct al_dwc_ddr_umctl2_regs __iomem *ctrl_regs =
		&((struct al_ddr_ctrl_regs __iomem *)
		ddr_ctrl_regs_base)->umctl2_regs;
	uint32_t reg_val;
	enum al_ddr_data_width data_width;

	reg_val = al_reg_read32(&ctrl_regs->mstr);
	reg_val &= DWC_DDR_UMCTL2_REGS_MSTR_DATA_BUS_WIDTH_MASK;

	data_width =
		(reg_val == DWC_DDR_UMCTL2_REGS_MSTR_DATA_BUS_WIDTH_64) ?
		AL_DDR_DATA_WIDTH_64_BITS :
		AL_DDR_DATA_WIDTH_32_BITS;

	return data_width;
}

/*******************************************************************************
 ******************************************************************************/
int al_ddr_address_translate_sys2dram(
	void __iomem			*ddr_ctrl_regs_base,
	al_phys_addr_t			sys_address,
	unsigned int			*rank,
	unsigned int			*bank,
	unsigned int			*col,
	unsigned int			*row)
{

	int i;
	unsigned int temp_rank = 0;
	unsigned int temp_bank = 0;
	unsigned int temp_col = 0;
	unsigned int temp_row = 0;
	struct al_ddr_addrmap addrmap;

	enum al_ddr_data_width data_width =
			al_ddr_data_width_get(ddr_ctrl_regs_base);

	al_ddr_address_map_get(ddr_ctrl_regs_base,&addrmap);

	if (data_width == AL_DDR_DATA_WIDTH_64_BITS)
		temp_col += ((sys_address >> 3) & 0x7);
	else
		temp_col += ((sys_address >> 2) & 0x7);

	for (i = 0; i < 7; i++)
		if (addrmap.col_b3_9_b11_13[i] != AL_DDR_ADDRMAP_NC){
			temp_col += (((sys_address >> addrmap.col_b3_9_b11_13[i]) & 0x1) << (i + 3));
		}
	if (addrmap.col_b3_9_b11_13[7] != AL_DDR_ADDRMAP_NC)
		temp_col += (((sys_address >> addrmap.col_b3_9_b11_13[7]) & 0x1) << 11);
	if (addrmap.col_b3_9_b11_13[8] != AL_DDR_ADDRMAP_NC)
		temp_col += (((sys_address >> addrmap.col_b3_9_b11_13[8]) & 0x1) << 13);

	for (i = 0; i < 3; i++)
		if (addrmap.bank_b0_2[i] != AL_DDR_ADDRMAP_NC){
			temp_bank += (((sys_address >> addrmap.bank_b0_2[i]) & 0x1) << i);
		}

	for (i = 0; i < 2; i++)
		if (addrmap.row_b0_2_10[i] != AL_DDR_ADDRMAP_NC){
			temp_row += (((sys_address >> addrmap.row_b0_2_10[i]) & 0x1) << i);
		}

	if (addrmap.row_b0_2_10[2] != AL_DDR_ADDRMAP_NC)
		for (i = 0; i < 9; i++){
			temp_row += (((sys_address >> (addrmap.row_b0_2_10[2] + i)) & 0x1) << (i + 2));
		}

	for (i = 0; i < 5; i++)
		if (addrmap.row_b11_15[i] != AL_DDR_ADDRMAP_NC){
			temp_row += (((sys_address >> addrmap.row_b11_15[i]) & 0x1) << (i + 11));
		}

	for (i = 0; i < 2; i++)
		if (addrmap.cs_b0_1[i] != AL_DDR_ADDRMAP_NC){
			temp_rank += (((sys_address >> addrmap.cs_b0_1[i]) & 0x1) << i);
		}

	*rank = temp_rank;
	*bank = temp_bank;
	*col = temp_col;
	*row = temp_row;

	return 0;
}

/*******************************************************************************
 ******************************************************************************/
int al_ddr_address_translate_dram2sys(
	void __iomem			*ddr_ctrl_regs_base,
	al_phys_addr_t			*sys_address,
	unsigned int			rank,
	unsigned int			bank,
	unsigned int			col,
	unsigned int			row)
{
	int i;
	struct al_ddr_addrmap addrmap;
	al_phys_addr_t address = 0;

	enum al_ddr_data_width data_width =
			al_ddr_data_width_get(ddr_ctrl_regs_base);
	al_ddr_address_map_get(ddr_ctrl_regs_base,&addrmap);

	if (data_width == AL_DDR_DATA_WIDTH_64_BITS)
		address += ((col & 0x7) << 3);
	else
		address += ((col & 0x7) << 2);

	for (i = 0; i < 7; i++)
		if (addrmap.col_b3_9_b11_13[i] != AL_DDR_ADDRMAP_NC){
			address += ((((al_phys_addr_t)col >> (i + 3)) & 0x1) << addrmap.col_b3_9_b11_13[i]);
		}
	if (addrmap.col_b3_9_b11_13[7] != AL_DDR_ADDRMAP_NC)
		address += ((((al_phys_addr_t)col >> 11) & 0x1) << addrmap.col_b3_9_b11_13[7]);
	if (addrmap.col_b3_9_b11_13[8] != AL_DDR_ADDRMAP_NC)
		address += ((((al_phys_addr_t)col >> 13) & 0x1) << addrmap.col_b3_9_b11_13[8]);

	for (i = 0; i < 3; i++)
		if (addrmap.bank_b0_2[i] != AL_DDR_ADDRMAP_NC){
			address += ((((al_phys_addr_t)bank >> (i)) & 0x1) << addrmap.bank_b0_2[i]);
		}

	for (i = 0; i < 2; i++)
		if (addrmap.row_b0_2_10[i] != AL_DDR_ADDRMAP_NC){
			address += ((((al_phys_addr_t)row >> (i)) & 0x1) << addrmap.row_b0_2_10[i]);
		}

	if (addrmap.row_b0_2_10[2] != AL_DDR_ADDRMAP_NC)
		for (i = 0; i < 9; i++){
			address += ((((al_phys_addr_t)row >> (i + 2)) & 0x1) << (addrmap.row_b0_2_10[2] + i));
		}


	for (i = 0; i < 5; i++)
		if (addrmap.row_b11_15[i] != AL_DDR_ADDRMAP_NC){
			address += ((((al_phys_addr_t)row >> (i + 11)) & 0x1) << addrmap.row_b11_15[i]);
		}

	for (i = 0; i < 2; i++)
		if (addrmap.cs_b0_1[i] != AL_DDR_ADDRMAP_NC){
			address += ((((al_phys_addr_t)rank >> (i)) & 0x1) << addrmap.cs_b0_1[i]);
		}

	*sys_address = address;

	return 0;
}

/*******************************************************************************
 ******************************************************************************/
unsigned int al_ddr_bits_per_rank_get(
	void __iomem			*ddr_ctrl_regs_base)
{
	int i, active_bits = 0;
	struct al_ddr_addrmap addrmap;
	enum al_ddr_data_width data_width =
			al_ddr_data_width_get(ddr_ctrl_regs_base);

	al_ddr_address_map_get(ddr_ctrl_regs_base,&addrmap);

	/* 64bit systems have a 6bit offset, 32bit systems have a 5bit offset */
	if (data_width == AL_DDR_DATA_WIDTH_64_BITS)
		active_bits += 6;
	else
		active_bits += 5;

	/* iterate over addrmap, count the amount of connected bits */
	for (i = 0; i < 9; i++)
		if (addrmap.col_b3_9_b11_13[i] != AL_DDR_ADDRMAP_NC)
			active_bits++;

	for (i = 0; i < 3; i++)
		if (addrmap.bank_b0_2[i] != AL_DDR_ADDRMAP_NC)
			active_bits++;

	for (i = 0; i < 2; i++)
		if (addrmap.row_b0_2_10[i] != AL_DDR_ADDRMAP_NC)
			active_bits++;

	if (addrmap.row_b0_2_10[2] != AL_DDR_ADDRMAP_NC)
		active_bits += 9;

	for (i = 0; i < 5; i++)
		if (addrmap.row_b11_15[i] != AL_DDR_ADDRMAP_NC)
			active_bits++;

	return active_bits;
}

/** @} end of DDR group */


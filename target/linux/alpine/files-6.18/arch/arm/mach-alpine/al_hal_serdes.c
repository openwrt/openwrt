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

#include <mach/al_hal_serdes.h>
#include <mach/al_hal_serdes_regs.h>
#include <mach/al_hal_serdes_internal_regs.h>

#define SRDS_CORE_REG_ADDR(page, type, offset)\
	(((page) << 13) | ((type) << 12) | (offset))

/* Link Training configuration */
#define AL_SERDES_TX_DEEMPH_SUM_MAX		0x1b

/* c configurations */
#define AL_SERDES_TX_DEEMPH_C_ZERO_MAX_VAL	0x1b
#define AL_SERDES_TX_DEEMPH_C_ZERO_MIN_VAL	0
#define AL_SERDES_TX_DEEMPH_C_ZERO_PRESET	AL_SERDES_TX_DEEMPH_C_ZERO_MAX_VAL

/* c(+1) configurations */
#define AL_SERDES_TX_DEEMPH_C_PLUS_MAX_VAL	0x9
#define AL_SERDES_TX_DEEMPH_C_PLUS_MIN_VAL	0
#define AL_SERDES_TX_DEEMPH_C_PLUS_PRESET	AL_SERDES_TX_DEEMPH_C_PLUS_MIN_VAL

/* c(-1) configurations */
#define AL_SERDES_TX_DEEMPH_C_MINUS_MAX_VAL	0x6
#define AL_SERDES_TX_DEEMPH_C_MINUS_MIN_VAL	0
#define AL_SERDES_TX_DEEMPH_C_MINUS_PRESET	AL_SERDES_TX_DEEMPH_C_MINUS_MIN_VAL

/* Rx equal total delay = MDELAY * TRIES */
#define AL_SERDES_RX_EQUAL_MDELAY		10
#define AL_SERDES_RX_EQUAL_TRIES		50

/* Rx eye calculation delay = MDELAY * TRIES */
#define AL_SERDES_RX_EYE_CAL_MDELAY		50
#define AL_SERDES_RX_EYE_CAL_TRIES		70


/**
 * SERDES core reg read
 */
static inline uint8_t al_serdes_grp_reg_read(
	struct al_serdes_group_info	*grp_info,
	enum al_serdes_reg_page		page,
	enum al_serdes_reg_type		type,
	uint16_t			offset);

/**
 * SERDES core reg write
 */
static inline void al_serdes_grp_reg_write(
	struct al_serdes_group_info	*grp_info,
	enum al_serdes_reg_page		page,
	enum al_serdes_reg_type		type,
	uint16_t			offset,
	uint8_t				data);

/**
 * SERDES core masked reg write
 */
static inline void al_serdes_grp_reg_masked_write(
	struct al_serdes_group_info	*grp_info,
	enum al_serdes_reg_page		page,
	enum al_serdes_reg_type		type,
	uint16_t			offset,
	uint8_t				mask,
	uint8_t				data);

/******************************************************************************/
/******************************************************************************/
int al_serdes_handle_init(
	void __iomem			*serdes_regs_base,
	struct al_serdes_obj		*obj)
{
	int i;

	al_dbg(
		"%s(%p, %p)\n",
		__func__,
		serdes_regs_base,
		obj);

	al_assert(serdes_regs_base);

	for (i = 0; i < AL_SRDS_NUM_GROUPS; i++) {
		obj->grp_info[i].pobj = obj;

		obj->grp_info[i].regs_base =
			&((struct al_serdes_regs *)serdes_regs_base)[i];
	}

	return 0;
}

/******************************************************************************/
/******************************************************************************/
int al_serdes_reg_read(
	struct al_serdes_obj	*obj,
	enum al_serdes_group	grp,
	enum al_serdes_reg_page	page,
	enum al_serdes_reg_type	type,
	uint16_t		offset,
	uint8_t			*data)
{
	int status = 0;

	al_dbg(
		"%s(%p, %d, %d, %d, %u)\n",
		__func__,
		obj,
		grp,
		page,
		type,
		offset);

	al_assert(obj);
	al_assert(data);
	al_assert(((int)grp) >= AL_SRDS_GRP_A);
	al_assert(((int)grp) <= AL_SRDS_GRP_D);
	al_assert(((int)page) >= AL_SRDS_REG_PAGE_0_LANE_0);
	al_assert(((int)page) <= AL_SRDS_REG_PAGE_4_COMMON);
	al_assert(((int)type) >= AL_SRDS_REG_TYPE_PMA);
	al_assert(((int)type) <= AL_SRDS_REG_TYPE_PCS);

	*data = al_serdes_grp_reg_read(
		&obj->grp_info[grp],
		page,
		type,
		offset);

	al_dbg(
		"%s: return(%u)\n",
		__func__,
		*data);

	return status;
}

/******************************************************************************/
/******************************************************************************/
int al_serdes_reg_write(
	struct al_serdes_obj	*obj,
	enum al_serdes_group	grp,
	enum al_serdes_reg_page	page,
	enum al_serdes_reg_type	type,
	uint16_t		offset,
	uint8_t			data)
{
	int status = 0;

	al_dbg(
		"%s(%p, %d, %d, %d, %u, %u)\n",
		__func__,
		obj,
		grp,
		page,
		type,
		offset,
		data);

	al_assert(obj);
	al_assert(((int)grp) >= AL_SRDS_GRP_A);
	al_assert(((int)grp) <= AL_SRDS_GRP_D);
	al_assert(((int)page) >= AL_SRDS_REG_PAGE_0_LANE_0);
	al_assert(((int)page) <= AL_SRDS_REG_PAGE_0123_LANES_0123);
	al_assert(((int)type) >= AL_SRDS_REG_TYPE_PMA);
	al_assert(((int)type) <= AL_SRDS_REG_TYPE_PCS);

	al_serdes_grp_reg_write(
		&obj->grp_info[grp],
		page,
		type,
		offset,
		data);

	return status;
}

/******************************************************************************/
/******************************************************************************/
#if (SERDES_IREG_FLD_PCSRX_DATAWIDTH_REG_NUM != SERDES_IREG_FLD_PCSTX_DATAWIDTH_REG_NUM)
#error "Wrong assumption!"
#endif
#if (SERDES_IREG_FLD_PCSRX_DIVRATE_REG_NUM != SERDES_IREG_FLD_PCSTX_DIVRATE_REG_NUM)
#error "Wrong assumption!"
#endif
#if (SERDES_IREG_FLD_CMNPCIEGEN3_LOCWREN_REG_NUM != SERDES_IREG_FLD_CMNPCS_LOCWREN_REG_NUM)
#error "Wrong assumption!"
#endif
#if (SERDES_IREG_FLD_CMNPCIEGEN3_LOCWREN_REG_NUM != SERDES_IREG_FLD_CMNPCSBIST_LOCWREN_REG_NUM)
#error "Wrong assumption!"
#endif
#if (SERDES_IREG_FLD_CMNPCIEGEN3_LOCWREN_REG_NUM != SERDES_IREG_FLD_CMNPCSPSTATE_LOCWREN_REG_NUM)
#error "Wrong assumption!"
#endif
#if (SERDES_IREG_FLD_LANEPCSPSTATE_LOCWREN_REG_NUM != SERDES_IREG_FLD_LB_LOCWREN_REG_NUM)
#error "Wrong assumption!"
#endif
#if (SERDES_IREG_FLD_LANEPCSPSTATE_LOCWREN_REG_NUM != SERDES_IREG_FLD_PCSRX_LOCWREN_REG_NUM)
#error "Wrong assumption!"
#endif
#if (SERDES_IREG_FLD_LANEPCSPSTATE_LOCWREN_REG_NUM != SERDES_IREG_FLD_PCSRXBIST_LOCWREN_REG_NUM)
#error "Wrong assumption!"
#endif
#if (SERDES_IREG_FLD_LANEPCSPSTATE_LOCWREN_REG_NUM != SERDES_IREG_FLD_PCSRXEQ_LOCWREN_REG_NUM)
#error "Wrong assumption!"
#endif
#if (SERDES_IREG_FLD_LANEPCSPSTATE_LOCWREN_REG_NUM != SERDES_IREG_FLD_PCSTX_LOCWREN_REG_NUM)
#error "Wrong assumption!"
#endif
void al_serdes_bist_overrides_enable(
	struct al_serdes_obj	*obj,
	enum al_serdes_group	grp,
	enum al_serdes_rate	rate)
{
	struct al_serdes_group_info *grp_info = &obj->grp_info[grp];
	int i;

	uint8_t rx_rate_val;
	uint8_t tx_rate_val;

	switch (rate) {
	case AL_SRDS_RATE_1_8:
		rx_rate_val = SERDES_IREG_FLD_PCSRX_DIVRATE_VAL_1_8;
		tx_rate_val = SERDES_IREG_FLD_PCSTX_DIVRATE_VAL_1_8;
		break;
	case AL_SRDS_RATE_1_4:
		rx_rate_val = SERDES_IREG_FLD_PCSRX_DIVRATE_VAL_1_4;
		tx_rate_val = SERDES_IREG_FLD_PCSTX_DIVRATE_VAL_1_4;
		break;
	case AL_SRDS_RATE_1_2:
		rx_rate_val = SERDES_IREG_FLD_PCSRX_DIVRATE_VAL_1_2;
		tx_rate_val = SERDES_IREG_FLD_PCSTX_DIVRATE_VAL_1_2;
		break;
	case AL_SRDS_RATE_FULL:
		rx_rate_val = SERDES_IREG_FLD_PCSRX_DIVRATE_VAL_1_1;
		tx_rate_val = SERDES_IREG_FLD_PCSTX_DIVRATE_VAL_1_1;
		break;
	default:
		al_err("%s: invalid rate (%d)\n",  __func__, rate);
		al_assert(0);
		rx_rate_val = SERDES_IREG_FLD_PCSRX_DIVRATE_VAL_1_1;
		tx_rate_val = SERDES_IREG_FLD_PCSTX_DIVRATE_VAL_1_1;
	}

	for (i = 0; i < AL_SRDS_NUM_LANES; i++) {
		al_serdes_grp_reg_masked_write(
			grp_info,
			(enum al_serdes_reg_page)i,
			AL_SRDS_REG_TYPE_PMA,
			SERDES_IREG_FLD_PCSRX_DATAWIDTH_REG_NUM,
			SERDES_IREG_FLD_PCSRX_DATAWIDTH_MASK |
			SERDES_IREG_FLD_PCSTX_DATAWIDTH_MASK,
			SERDES_IREG_FLD_PCSRX_DATAWIDTH_VAL_20 |
			SERDES_IREG_FLD_PCSTX_DATAWIDTH_VAL_20);

		al_serdes_grp_reg_masked_write(
			grp_info,
			(enum al_serdes_reg_page)i,
			AL_SRDS_REG_TYPE_PMA,
			SERDES_IREG_FLD_PCSRX_DIVRATE_REG_NUM,
			SERDES_IREG_FLD_PCSRX_DIVRATE_MASK |
			SERDES_IREG_FLD_PCSTX_DIVRATE_MASK,
			rx_rate_val | tx_rate_val);
	}

	al_serdes_grp_reg_masked_write(
		grp_info,
		AL_SRDS_REG_PAGE_4_COMMON,
		AL_SRDS_REG_TYPE_PMA,
		SERDES_IREG_FLD_CMNPCIEGEN3_LOCWREN_REG_NUM,
		SERDES_IREG_FLD_CMNPCIEGEN3_LOCWREN |
		SERDES_IREG_FLD_CMNPCS_LOCWREN |
		SERDES_IREG_FLD_CMNPCSBIST_LOCWREN |
		SERDES_IREG_FLD_CMNPCSPSTATE_LOCWREN,
		0);

	al_serdes_grp_reg_masked_write(
		grp_info,
		AL_SRDS_REG_PAGE_4_COMMON,
		AL_SRDS_REG_TYPE_PMA,
		SERDES_IREG_FLD_CMNPCIEGEN3_LOCWREN_REG_NUM,
		SERDES_IREG_FLD_CMNPCIEGEN3_LOCWREN |
		SERDES_IREG_FLD_CMNPCS_LOCWREN |
		SERDES_IREG_FLD_CMNPCSBIST_LOCWREN |
		SERDES_IREG_FLD_CMNPCSPSTATE_LOCWREN,
		0);

	al_serdes_grp_reg_masked_write(
		grp_info,
		AL_SRDS_REG_PAGE_4_COMMON,
		AL_SRDS_REG_TYPE_PMA,
		SERDES_IREG_FLD_PCS_LOCWREN_REG_NUM,
		SERDES_IREG_FLD_PCS_LOCWREN,
		0);

	al_serdes_grp_reg_masked_write(
		grp_info,
		AL_SRDS_REG_PAGE_4_COMMON,
		AL_SRDS_REG_TYPE_PMA,
		SERDES_IREG_FLD_CMNPCS_TXENABLE_REG_NUM,
		SERDES_IREG_FLD_CMNPCS_TXENABLE,
		SERDES_IREG_FLD_CMNPCS_TXENABLE);

	for (i = 0; i < AL_SRDS_NUM_LANES; i++) {
		al_serdes_grp_reg_masked_write(
			grp_info,
			(enum al_serdes_reg_page)i,
			AL_SRDS_REG_TYPE_PMA,
			SERDES_IREG_FLD_LANEPCSPSTATE_LOCWREN_REG_NUM,
			SERDES_IREG_FLD_LANEPCSPSTATE_LOCWREN |
			SERDES_IREG_FLD_LB_LOCWREN |
			SERDES_IREG_FLD_PCSRX_LOCWREN |
			SERDES_IREG_FLD_PCSRXBIST_LOCWREN |
			SERDES_IREG_FLD_PCSRXEQ_LOCWREN |
			SERDES_IREG_FLD_PCSTX_LOCWREN,
			0);

		al_serdes_grp_reg_masked_write(
			grp_info,
			(enum al_serdes_reg_page)i,
			AL_SRDS_REG_TYPE_PMA,
			SERDES_IREG_FLD_PCSTXBIST_LOCWREN_REG_NUM,
			SERDES_IREG_FLD_PCSTXBIST_LOCWREN,
			0);

		al_serdes_grp_reg_masked_write(
			grp_info,
			(enum al_serdes_reg_page)i,
			AL_SRDS_REG_TYPE_PMA,
			SERDES_IREG_FLD_TX_DRV_OVERRIDE_EN_REG_NUM,
			SERDES_IREG_FLD_TX_DRV_OVERRIDE_EN,
			0);

		al_serdes_grp_reg_masked_write(
			grp_info,
			(enum al_serdes_reg_page)i,
			AL_SRDS_REG_TYPE_PMA,
			SERDES_IREG_FLD_RXLOCK2REF_OVREN_REG_NUM,
			SERDES_IREG_FLD_RXLOCK2REF_OVREN,
			SERDES_IREG_FLD_RXLOCK2REF_OVREN);
	}
}

/******************************************************************************/
/******************************************************************************/
void al_serdes_group_pm_set(
	struct al_serdes_obj	*obj,
	enum al_serdes_group	grp,
	enum al_serdes_pm	pm)
{
	struct al_serdes_group_info *grp_info = &obj->grp_info[grp];

	uint8_t pm_val;

	switch (pm) {
	case AL_SRDS_PM_PD:
		pm_val = SERDES_IREG_FLD_CMNPCSPSTATE_SYNTH_VAL_PD;
		break;
	case AL_SRDS_PM_P2:
		pm_val = SERDES_IREG_FLD_CMNPCSPSTATE_SYNTH_VAL_P2;
		break;
	case AL_SRDS_PM_P1:
		pm_val = SERDES_IREG_FLD_CMNPCSPSTATE_SYNTH_VAL_P1;
		break;
	case AL_SRDS_PM_P0S:
		pm_val = SERDES_IREG_FLD_CMNPCSPSTATE_SYNTH_VAL_P0S;
		break;
	case AL_SRDS_PM_P0:
		pm_val = SERDES_IREG_FLD_CMNPCSPSTATE_SYNTH_VAL_P0;
		break;
	default:
		al_err("%s: invalid power mode (%d)\n",  __func__, pm);
		al_assert(0);
		pm_val = SERDES_IREG_FLD_CMNPCSPSTATE_SYNTH_VAL_P0;
	}

	al_serdes_grp_reg_masked_write(
		grp_info,
		AL_SRDS_REG_PAGE_4_COMMON,
		AL_SRDS_REG_TYPE_PMA,
		SERDES_IREG_FLD_CMNPCSPSTATE_SYNTH_REG_NUM,
		SERDES_IREG_FLD_CMNPCSPSTATE_SYNTH_MASK,
		pm_val);
}

/******************************************************************************/
/******************************************************************************/
void al_serdes_lane_pm_set(
	struct al_serdes_obj	*obj,
	enum al_serdes_group	grp,
	enum al_serdes_lane	lane,
	enum al_serdes_pm	rx_pm,
	enum al_serdes_pm	tx_pm)
{
	struct al_serdes_group_info *grp_info = &obj->grp_info[grp];

	uint8_t rx_pm_val;
	uint8_t tx_pm_val;

	switch (rx_pm) {
	case AL_SRDS_PM_PD:
		rx_pm_val = SERDES_IREG_FLD_LANEPCSPSTATE_RX_VAL_PD;
		break;
	case AL_SRDS_PM_P2:
		rx_pm_val = SERDES_IREG_FLD_LANEPCSPSTATE_RX_VAL_P2;
		break;
	case AL_SRDS_PM_P1:
		rx_pm_val = SERDES_IREG_FLD_LANEPCSPSTATE_RX_VAL_P1;
		break;
	case AL_SRDS_PM_P0S:
		rx_pm_val = SERDES_IREG_FLD_LANEPCSPSTATE_RX_VAL_P0S;
		break;
	case AL_SRDS_PM_P0:
		rx_pm_val = SERDES_IREG_FLD_LANEPCSPSTATE_RX_VAL_P0;
		break;
	default:
		al_err("%s: invalid rx power mode (%d)\n",  __func__, rx_pm);
		al_assert(0);
		rx_pm_val = SERDES_IREG_FLD_LANEPCSPSTATE_RX_VAL_P0;
	}

	switch (tx_pm) {
	case AL_SRDS_PM_PD:
		tx_pm_val = SERDES_IREG_FLD_LANEPCSPSTATE_TX_VAL_PD;
		break;
	case AL_SRDS_PM_P2:
		tx_pm_val = SERDES_IREG_FLD_LANEPCSPSTATE_TX_VAL_P2;
		break;
	case AL_SRDS_PM_P1:
		tx_pm_val = SERDES_IREG_FLD_LANEPCSPSTATE_TX_VAL_P1;
		break;
	case AL_SRDS_PM_P0S:
		tx_pm_val = SERDES_IREG_FLD_LANEPCSPSTATE_TX_VAL_P0S;
		break;
	case AL_SRDS_PM_P0:
		tx_pm_val = SERDES_IREG_FLD_LANEPCSPSTATE_TX_VAL_P0;
		break;
	default:
		al_err("%s: invalid tx power mode (%d)\n",  __func__, tx_pm);
		al_assert(0);
		tx_pm_val = SERDES_IREG_FLD_LANEPCSPSTATE_TX_VAL_P0;
	}

	al_serdes_grp_reg_masked_write(
		grp_info,
		(enum al_serdes_reg_page)lane,
		AL_SRDS_REG_TYPE_PMA,
		SERDES_IREG_FLD_LANEPCSPSTATE_RX_REG_NUM,
		SERDES_IREG_FLD_LANEPCSPSTATE_RX_MASK,
		rx_pm_val);

	al_serdes_grp_reg_masked_write(
		grp_info,
		(enum al_serdes_reg_page)lane,
		AL_SRDS_REG_TYPE_PMA,
		SERDES_IREG_FLD_LANEPCSPSTATE_TX_REG_NUM,
		SERDES_IREG_FLD_LANEPCSPSTATE_TX_MASK,
		tx_pm_val);
}

/******************************************************************************/
/******************************************************************************/
void al_serdes_pma_hard_reset_group(
	struct al_serdes_obj	*obj,
	enum al_serdes_group	grp,
	al_bool			enable)
{
	struct al_serdes_group_info *grp_info = &obj->grp_info[grp];

	/* Enable Hard Reset Override */
	al_serdes_grp_reg_masked_write(
		grp_info,
		AL_SRDS_REG_PAGE_4_COMMON,
		AL_SRDS_REG_TYPE_PMA,
		SERDES_IREG_FLD_CMNCTLPOR_HARDRSTBYPASSEN_SYNTH_REG_NUM,
		SERDES_IREG_FLD_CMNCTLPOR_HARDRSTBYPASSEN_SYNTH_MASK,
		SERDES_IREG_FLD_CMNCTLPOR_HARDRSTBYPASSEN_SYNTH_VAL_REGS);

	/* Assert/Deassert Hard Reset Override */
	al_serdes_grp_reg_masked_write(
		grp_info,
		AL_SRDS_REG_PAGE_4_COMMON,
		AL_SRDS_REG_TYPE_PMA,
		SERDES_IREG_FLD_CMNCTLPOR_HARDRSTBYPASS_SYNTH_REG_NUM,
		SERDES_IREG_FLD_CMNCTLPOR_HARDRSTBYPASS_SYNTH_MASK,
		enable ?
		SERDES_IREG_FLD_CMNCTLPOR_HARDRSTBYPASS_SYNTH_VAL_ASSERT :
		SERDES_IREG_FLD_CMNCTLPOR_HARDRSTBYPASS_SYNTH_VAL_DEASSERT);
}

/******************************************************************************/
/******************************************************************************/
void al_serdes_pma_hard_reset_lane(
	struct al_serdes_obj	*obj,
	enum al_serdes_group	grp,
	enum al_serdes_lane	lane,
	al_bool			enable)
{
	struct al_serdes_group_info *grp_info = &obj->grp_info[grp];

	/* Enable Hard Reset Override */
	al_serdes_grp_reg_masked_write(
		grp_info,
		(enum al_serdes_reg_page)lane,
		AL_SRDS_REG_TYPE_PMA,
		SERDES_IREG_FLD_CMNCTLPOR_HARDRSTBYPASSEN_REG_NUM,
		SERDES_IREG_FLD_CMNCTLPOR_HARDRSTBYPASSEN_MASK,
		SERDES_IREG_FLD_CMNCTLPOR_HARDRSTBYPASSEN_VAL_REGS);

	/* Assert/Deassert Hard Reset Override */
	al_serdes_grp_reg_masked_write(
		grp_info,
		(enum al_serdes_reg_page)lane,
		AL_SRDS_REG_TYPE_PMA,
		SERDES_IREG_FLD_CMNCTLPOR_HARDRSTBYPASS_REG_NUM,
		SERDES_IREG_FLD_CMNCTLPOR_HARDRSTBYPASS_MASK,
		enable ?
		SERDES_IREG_FLD_CMNCTLPOR_HARDRSTBYPASS_VAL_ASSERT :
		SERDES_IREG_FLD_CMNCTLPOR_HARDRSTBYPASS_VAL_DEASSERT);
}

/******************************************************************************/
/******************************************************************************/
#if	(SERDES_IREG_FLD_LB_RX2TXUNTIMEDEN_REG_NUM !=\
	SERDES_IREG_FLD_LB_TX2RXBUFTIMEDEN_REG_NUM) ||\
	(SERDES_IREG_FLD_LB_TX2RXBUFTIMEDEN_REG_NUM !=\
	SERDES_IREG_FLD_LB_TX2RXIOTIMEDEN_REG_NUM) ||\
	(SERDES_IREG_FLD_LB_TX2RXIOTIMEDEN_REG_NUM !=\
	 SERDES_IREG_FLD_LB_PARRX2TXTIMEDEN_REG_NUM) ||\
	(SERDES_IREG_FLD_LB_PARRX2TXTIMEDEN_REG_NUM !=\
	 SERDES_IREG_FLD_LB_CDRCLK2TXEN_REG_NUM)
#error Wrong assumption
#endif

void al_serdes_loopback_control(
	struct al_serdes_obj	*obj,
	enum al_serdes_group	grp,
	enum al_serdes_lane	lane,
	enum al_serdes_lb_mode	mode)
{
	struct al_serdes_group_info *grp_info = &obj->grp_info[grp];
	uint8_t val = 0;

	switch (mode) {
	case AL_SRDS_LB_MODE_OFF:
		break;
	case AL_SRDS_LB_MODE_PMA_IO_UN_TIMED_RX_TO_TX:
		val = SERDES_IREG_FLD_LB_RX2TXUNTIMEDEN;
		break;
	case AL_SRDS_LB_MODE_PMA_INTERNALLY_BUFFERED_SERIAL_TX_TO_RX:
		val = SERDES_IREG_FLD_LB_TX2RXBUFTIMEDEN;
		break;
	case AL_SRDS_LB_MODE_PMA_SERIAL_TX_IO_TO_RX_IO:
		val = SERDES_IREG_FLD_LB_TX2RXIOTIMEDEN;
		break;
	case AL_SRDS_LB_MODE_PMA_PARALLEL_RX_TO_TX:
		val = SERDES_IREG_FLD_LB_PARRX2TXTIMEDEN |
			SERDES_IREG_FLD_LB_CDRCLK2TXEN;
		break;
	default:
		al_err("%s: invalid mode (%d)\n",  __func__, mode);
		al_assert(0);
	}

	al_serdes_grp_reg_masked_write(
		grp_info,
		(enum al_serdes_reg_page)lane,
		AL_SRDS_REG_TYPE_PMA,
		SERDES_IREG_FLD_LB_RX2TXUNTIMEDEN_REG_NUM,
		SERDES_IREG_FLD_LB_RX2TXUNTIMEDEN |
		SERDES_IREG_FLD_LB_TX2RXBUFTIMEDEN |
		SERDES_IREG_FLD_LB_TX2RXIOTIMEDEN |
		SERDES_IREG_FLD_LB_PARRX2TXTIMEDEN |
		SERDES_IREG_FLD_LB_CDRCLK2TXEN,
		val);
}

/******************************************************************************/
/******************************************************************************/
void al_serdes_bist_pattern_select(
	struct al_serdes_obj		*obj,
	enum al_serdes_group		grp,
	enum al_serdes_bist_pattern	pattern,
	uint8_t				*user_data)
{
	struct al_serdes_group_info *grp_info = &obj->grp_info[grp];
	uint8_t val = 0;

	switch (pattern) {
	case AL_SRDS_BIST_PATTERN_USER:
		al_assert(user_data);
		al_err("%s: user pattern currently not supported!\n", __func__);
		al_assert(0);
		break;
	case AL_SRDS_BIST_PATTERN_PRBS7:
		val = SERDES_IREG_FLD_CMNPCSBIST_MODESEL_VAL_PRBS7;
		break;
	case AL_SRDS_BIST_PATTERN_PRBS23:
		val = SERDES_IREG_FLD_CMNPCSBIST_MODESEL_VAL_PRBS23;
		break;
	case AL_SRDS_BIST_PATTERN_PRBS31:
		val = SERDES_IREG_FLD_CMNPCSBIST_MODESEL_VAL_PRBS31;
		break;
	case AL_SRDS_BIST_PATTERN_CLK1010:
		val = SERDES_IREG_FLD_CMNPCSBIST_MODESEL_VAL_CLK1010;
		break;
	default:
		al_err("%s: invalid pattern (%d)\n", __func__, pattern);
		al_assert(0);
	}

	al_serdes_grp_reg_masked_write(
		grp_info,
		AL_SRDS_REG_PAGE_4_COMMON,
		AL_SRDS_REG_TYPE_PMA,
		SERDES_IREG_FLD_CMNPCSBIST_MODESEL_REG_NUM,
		SERDES_IREG_FLD_CMNPCSBIST_MODESEL_MASK,
		val);
}

/******************************************************************************/
/******************************************************************************/
void al_serdes_bist_tx_enable(
	struct al_serdes_obj	*obj,
	enum al_serdes_group	grp,
	enum al_serdes_lane	lane,
	al_bool			enable)
{
	struct al_serdes_group_info *grp_info = &obj->grp_info[grp];

	al_serdes_grp_reg_masked_write(
		grp_info,
		(enum al_serdes_reg_page)lane,
		AL_SRDS_REG_TYPE_PMA,
		SERDES_IREG_FLD_PCSTXBIST_EN_REG_NUM,
		SERDES_IREG_FLD_PCSTXBIST_EN,
		enable ? SERDES_IREG_FLD_PCSTXBIST_EN : 0);
}

/******************************************************************************/
/******************************************************************************/
void al_serdes_bist_tx_err_inject(
	struct al_serdes_obj	*obj,
	enum al_serdes_group	grp)
{
	struct al_serdes_group_info *grp_info = &obj->grp_info[grp];

	al_serdes_grp_reg_masked_write(
		grp_info,
		AL_SRDS_REG_PAGE_4_COMMON,
		AL_SRDS_REG_TYPE_PMA,
		SERDES_IREG_FLD_TXBIST_BITERROR_EN_REG_NUM,
		SERDES_IREG_FLD_TXBIST_BITERROR_EN,
		SERDES_IREG_FLD_TXBIST_BITERROR_EN);

	al_serdes_grp_reg_masked_write(
		grp_info,
		AL_SRDS_REG_PAGE_4_COMMON,
		AL_SRDS_REG_TYPE_PMA,
		SERDES_IREG_FLD_TXBIST_BITERROR_EN_REG_NUM,
		SERDES_IREG_FLD_TXBIST_BITERROR_EN,
		0);
}

/******************************************************************************/
/******************************************************************************/
void al_serdes_bist_rx_enable(
	struct al_serdes_obj	*obj,
	enum al_serdes_group	grp,
	enum al_serdes_lane	lane,
	al_bool			enable)
{
	struct al_serdes_group_info *grp_info = &obj->grp_info[grp];

	al_serdes_grp_reg_masked_write(
		grp_info,
		(enum al_serdes_reg_page)lane,
		AL_SRDS_REG_TYPE_PMA,
		SERDES_IREG_FLD_PCSRXBIST_EN_REG_NUM,
		SERDES_IREG_FLD_PCSRXBIST_EN,
		enable ? SERDES_IREG_FLD_PCSRXBIST_EN : 0);
}

/******************************************************************************/
/******************************************************************************/
#if	(SERDES_IREG_FLD_RXBIST_ERRCOUNT_OVERFLOW_REG_NUM !=\
	SERDES_IREG_FLD_RXBIST_RXLOCKED_REG_NUM)
#error Wrong assumption
#endif

void al_serdes_bist_rx_status(
	struct al_serdes_obj	*obj,
	enum al_serdes_group	grp,
	enum al_serdes_lane	lane,
	al_bool			*is_locked,
	al_bool			*err_cnt_overflow,
	uint16_t		*err_cnt)
{
	struct al_serdes_group_info *grp_info = &obj->grp_info[grp];
	uint8_t status_reg_val;
	uint16_t err_cnt_msb_reg_val;
	uint16_t err_cnt_lsb_reg_val;

	status_reg_val = al_serdes_grp_reg_read(
		grp_info,
		(enum al_serdes_reg_page)lane,
		AL_SRDS_REG_TYPE_PMA,
		SERDES_IREG_FLD_RXBIST_RXLOCKED_REG_NUM);

	err_cnt_msb_reg_val = al_serdes_grp_reg_read(
		grp_info,
		(enum al_serdes_reg_page)lane,
		AL_SRDS_REG_TYPE_PMA,
		SERDES_IREG_FLD_RXBIST_ERRCOUNT_MSB_REG_NUM);

	err_cnt_lsb_reg_val = al_serdes_grp_reg_read(
		grp_info,
		(enum al_serdes_reg_page)lane,
		AL_SRDS_REG_TYPE_PMA,
		SERDES_IREG_FLD_RXBIST_ERRCOUNT_LSB_REG_NUM);

	*is_locked =
		(status_reg_val & SERDES_IREG_FLD_RXBIST_RXLOCKED) ?
		AL_TRUE : AL_FALSE;

	*err_cnt_overflow =
		(status_reg_val & SERDES_IREG_FLD_RXBIST_ERRCOUNT_OVERFLOW) ?
		AL_TRUE : AL_FALSE;

	*err_cnt = (err_cnt_msb_reg_val << 8) + err_cnt_lsb_reg_val;
}

/******************************************************************************/
/******************************************************************************/
static inline uint8_t al_serdes_grp_reg_read(
	struct al_serdes_group_info	*grp_info,
	enum al_serdes_reg_page		page,
	enum al_serdes_reg_type		type,
	uint16_t			offset)
{
	al_reg_write32(
		&grp_info->regs_base->gen.reg_addr,
		SRDS_CORE_REG_ADDR(page, type, offset));

	return al_reg_read32(&grp_info->regs_base->gen.reg_data);
}

/******************************************************************************/
/******************************************************************************/
static inline void al_serdes_grp_reg_write(
	struct al_serdes_group_info	*grp_info,
	enum al_serdes_reg_page		page,
	enum al_serdes_reg_type		type,
	uint16_t			offset,
	uint8_t				data)
{
	al_reg_write32(
		&grp_info->regs_base->gen.reg_addr,
		SRDS_CORE_REG_ADDR(page, type, offset));

	al_reg_write32(&grp_info->regs_base->gen.reg_data, data);
}

/******************************************************************************/
/******************************************************************************/
static inline void al_serdes_grp_reg_masked_write(
	struct al_serdes_group_info	*grp_info,
	enum al_serdes_reg_page		page,
	enum al_serdes_reg_type		type,
	uint16_t			offset,
	uint8_t				mask,
	uint8_t				data)
{
	uint8_t val;
	enum al_serdes_reg_page	start_page = page;
	enum al_serdes_reg_page	end_page   = page;
	enum al_serdes_reg_page	iter_page;

	if (page == AL_SRDS_REG_PAGE_0123_LANES_0123) {
		start_page = AL_SRDS_REG_PAGE_0_LANE_0;
		end_page   = AL_SRDS_REG_PAGE_3_LANE_3;
	}

	for(iter_page = start_page; iter_page <= end_page; ++iter_page) {
		val = al_serdes_grp_reg_read(grp_info, iter_page, type, offset);
		val &= ~mask;
		val |= data;
		al_serdes_grp_reg_write(grp_info, iter_page, type, offset, val);
	}
}

/******************************************************************************/
/******************************************************************************/
int al_serdes_eye_measure_run(
		struct al_serdes_obj	*obj,
		enum al_serdes_group	grp,
		enum al_serdes_lane	lane,
		uint32_t		timeout,
		unsigned int		*value)
{
	uint32_t reg = 0;
	uint32_t i;
	struct serdes_lane *lane_regs;

	lane_regs = &obj->grp_info[grp].regs_base->lane[lane];

	al_reg_write32(&lane_regs->ictl_multi_rxeq,
		       SERDES_LANE_ICTL_MULTI_RXEQ_START_L_A);

	for (i = 0 ; i < timeout ; i++) {
		reg = al_reg_read32(&lane_regs->octl_multi);

		if (reg & SERDES_LANE_OCTL_MULTI_RXEQ_DONE_L_A)
			break;

		al_msleep(10);
	}

	if (i == timeout) {
		al_err("%s: measure eye failed on timeout\n", __func__);
		return -ETIMEDOUT;
	}

	*value = al_reg_read32(&lane_regs->odat_multi_rxeq);

	al_reg_write32(&lane_regs->ictl_multi_rxeq, 0);

	return 0;
}

/******************************************************************************/
/******************************************************************************/
int al_serdes_eye_diag_sample(
		struct al_serdes_obj	*obj,
		enum al_serdes_group	grp,
		enum al_serdes_lane	lane,
		unsigned int		x,
		int			y,
		unsigned int		timeout,
		unsigned int		*value)
{
	enum al_serdes_reg_page	page = (enum al_serdes_reg_page)lane;
	struct al_serdes_group_info *grp_info;
	uint32_t i;
	uint8_t sample_count_orig_msb;
	uint8_t sample_count_orig_lsb;

	al_assert(obj);
	al_assert(((int)grp) >= AL_SRDS_GRP_A);
	al_assert(((int)grp) <= AL_SRDS_GRP_D);
	al_assert(((int)page) >= AL_SRDS_REG_PAGE_0_LANE_0);
	al_assert(((int)page) <= AL_SRDS_REG_PAGE_0123_LANES_0123);

	grp_info = &obj->grp_info[grp];

	/* Obtain sample count by reading RXCALROAMEYEMEAS_COUNT */
	sample_count_orig_msb = al_serdes_grp_reg_read(grp_info,
		AL_SRDS_REG_PAGE_4_COMMON, AL_SRDS_REG_TYPE_PMA,
		SERDES_IREG_FLD_EYE_DIAG_SAMPLE_CNT_MSB_REG_NUM);
	sample_count_orig_lsb = al_serdes_grp_reg_read(grp_info,
		AL_SRDS_REG_PAGE_4_COMMON, AL_SRDS_REG_TYPE_PMA,
		SERDES_IREG_FLD_EYE_DIAG_SAMPLE_CNT_LSB_REG_NUM);

	/* Set sample count to ~100000 samples */
	al_serdes_grp_reg_write(grp_info, AL_SRDS_REG_PAGE_4_COMMON,
		AL_SRDS_REG_TYPE_PMA,
		SERDES_IREG_FLD_EYE_DIAG_SAMPLE_CNT_MSB_REG_NUM, 0x13);
	al_serdes_grp_reg_write(grp_info, AL_SRDS_REG_PAGE_4_COMMON,
		AL_SRDS_REG_TYPE_PMA,
		SERDES_IREG_FLD_EYE_DIAG_SAMPLE_CNT_LSB_REG_NUM, 0x88);

	/* BER Contour Overwrite */
	al_serdes_grp_reg_masked_write(grp_info, page, AL_SRDS_REG_TYPE_PMA,
		SERDES_IREG_FLD_RXCALROAMEYEMEASIN_LOCWREN_REG_NUM,
		SERDES_IREG_FLD_RXCALROAMEYEMEASIN_LOCWREN,
		0);
	al_serdes_grp_reg_masked_write(grp_info, page, AL_SRDS_REG_TYPE_PMA,
		SERDES_IREG_FLD_RXCALROAMXADJUST_LOCWREN_REG_NUM,
		SERDES_IREG_FLD_RXCALROAMXADJUST_LOCWREN,
		0);
	al_serdes_grp_reg_masked_write(grp_info, page, AL_SRDS_REG_TYPE_PMA,
		SERDES_IREG_FLD_RXCALROAMYADJUST_LOCWREN_REG_NUM,
		SERDES_IREG_FLD_RXCALROAMYADJUST_LOCWREN,
		0);

	/* RXROAM_XORBITSEL = 0x1 or 0x0 */
	al_serdes_grp_reg_masked_write(grp_info, page, AL_SRDS_REG_TYPE_PMA,
		SERDES_IREG_FLD_RXROAM_XORBITSEL_REG_NUM,
		SERDES_IREG_FLD_RXROAM_XORBITSEL,
		SERDES_IREG_FLD_RXROAM_XORBITSEL_2ND);

	/* Set X */
	al_serdes_grp_reg_write(grp_info, page, AL_SRDS_REG_TYPE_PMA,
		SERDES_IREG_FLD_RXCALROAMXADJUST_REG_NUM, x);

	/* Set Y */
	al_serdes_grp_reg_write(grp_info, page, AL_SRDS_REG_TYPE_PMA,
		SERDES_IREG_FLD_RXCALROAMYADJUST_REG_NUM,
		y < 32 ? 31 - y : y + 1);

	/* Start Measurement by setting RXCALROAMEYEMEASIN_CYCLEEN = 0x1 */
	al_serdes_grp_reg_masked_write(grp_info, page, AL_SRDS_REG_TYPE_PMA,
		SERDES_IREG_FLD_RXCALROAMEYEMEASIN_CYCLEEN_REG_NUM,
		SERDES_IREG_FLD_RXCALROAMEYEMEASIN_CYCLEEN_START,
		SERDES_IREG_FLD_RXCALROAMEYEMEASIN_CYCLEEN_START);

	/* Check RXCALROAMEYEMEASDONE Signal (Polling Until 0x1) */
	for (i = 0 ; i < timeout ; i++) {
		if (al_serdes_grp_reg_read(grp_info, page, AL_SRDS_REG_TYPE_PMA,
			SERDES_IREG_FLD_RXCALROAMEYEMEASDONE_REG_NUM) &
			SERDES_IREG_FLD_RXCALROAMEYEMEASDONE)
			break;
		al_udelay(1);
	}
	if (i == timeout) {
		al_err("%s: eye diagram sampling timed out!\n", __func__);
		return -ETIMEDOUT;
	}

	/* Stop Measurement by setting RXCALROAMEYEMEASIN_CYCLEEN = 0x0 */
	al_serdes_grp_reg_masked_write(grp_info, page, AL_SRDS_REG_TYPE_PMA,
		SERDES_IREG_FLD_RXCALROAMEYEMEASIN_CYCLEEN_REG_NUM,
		SERDES_IREG_FLD_RXCALROAMEYEMEASIN_CYCLEEN_START,
		0);

	/* Obtain Error Counts by reading RXCALROAMEYEMEAS_ACC */
	*value = ((unsigned int)al_serdes_grp_reg_read(grp_info, page,
		AL_SRDS_REG_TYPE_PMA,
		SERDES_IREG_FLD_RXCALROAMEYEMEAS_ACC_MSB_REG_NUM)) << 8 |
		al_serdes_grp_reg_read(grp_info, page, AL_SRDS_REG_TYPE_PMA,
		SERDES_IREG_FLD_RXCALROAMEYEMEAS_ACC_LSB_REG_NUM);

	/* BER Contour Overwrite */
	al_serdes_grp_reg_masked_write(grp_info, page, AL_SRDS_REG_TYPE_PMA,
		SERDES_IREG_FLD_RXCALROAMEYEMEASIN_LOCWREN_REG_NUM,
		SERDES_IREG_FLD_RXCALROAMEYEMEASIN_LOCWREN,
		SERDES_IREG_FLD_RXCALROAMEYEMEASIN_LOCWREN);
	al_serdes_grp_reg_masked_write(grp_info, page, AL_SRDS_REG_TYPE_PMA,
		SERDES_IREG_FLD_RXCALROAMXADJUST_LOCWREN_REG_NUM,
		SERDES_IREG_FLD_RXCALROAMXADJUST_LOCWREN,
		SERDES_IREG_FLD_RXCALROAMXADJUST_LOCWREN);
	al_serdes_grp_reg_masked_write(grp_info, page, AL_SRDS_REG_TYPE_PMA,
		SERDES_IREG_FLD_RXCALROAMYADJUST_LOCWREN_REG_NUM,
		SERDES_IREG_FLD_RXCALROAMYADJUST_LOCWREN,
		SERDES_IREG_FLD_RXCALROAMYADJUST_LOCWREN);

	/* Restore sample count */
	al_serdes_grp_reg_write(grp_info, AL_SRDS_REG_PAGE_4_COMMON,
		AL_SRDS_REG_TYPE_PMA,
		SERDES_IREG_FLD_EYE_DIAG_SAMPLE_CNT_MSB_REG_NUM,
		sample_count_orig_msb);
	al_serdes_grp_reg_write(grp_info, AL_SRDS_REG_PAGE_4_COMMON,
		AL_SRDS_REG_TYPE_PMA,
		SERDES_IREG_FLD_EYE_DIAG_SAMPLE_CNT_LSB_REG_NUM,
		sample_count_orig_lsb);

	return 0;
}

/******************************************************************************/
/******************************************************************************/
static void al_serdes_tx_deemph_set(
		struct al_serdes_obj	*obj,
		enum al_serdes_group	grp,
		enum al_serdes_lane	lane,
		uint32_t		c_zero,
		uint32_t		c_plus_1,
		uint32_t		c_minus_1)
{
	al_serdes_grp_reg_masked_write(
			&obj->grp_info[grp],
			(enum al_serdes_reg_page)lane,
			AL_SRDS_REG_TYPE_PMA,
			SERDES_IREG_TX_DRV_1_REG_NUM,
			SERDES_IREG_TX_DRV_1_LEVN_MASK,
			((c_zero + c_plus_1 + c_minus_1)
				<< SERDES_IREG_TX_DRV_1_LEVN_SHIFT));

	al_serdes_grp_reg_masked_write(
			&obj->grp_info[grp],
			(enum al_serdes_reg_page)lane,
			AL_SRDS_REG_TYPE_PMA,
			SERDES_IREG_TX_DRV_2_REG_NUM,
			SERDES_IREG_TX_DRV_2_LEVNM1_MASK,
			(c_plus_1 << SERDES_IREG_TX_DRV_2_LEVNM1_SHIFT));

	al_serdes_grp_reg_masked_write(
			&obj->grp_info[grp],
			(enum al_serdes_reg_page)lane,
			AL_SRDS_REG_TYPE_PMA,
			SERDES_IREG_TX_DRV_3_REG_NUM,
			SERDES_IREG_TX_DRV_3_LEVNP1_MASK,
			(c_minus_1 << SERDES_IREG_TX_DRV_3_LEVNP1_SHIFT));
}

static void al_serdes_tx_deemph_get(
		struct al_serdes_obj	*obj,
		enum al_serdes_group	grp,
		enum al_serdes_lane	lane,
		uint32_t		*c_zero,
		uint32_t		*c_plus_1,
		uint32_t		*c_minus_1)
{
	uint32_t reg = 0;

	reg = al_serdes_grp_reg_read(
			&obj->grp_info[grp],
			(enum al_serdes_reg_page)lane,
			AL_SRDS_REG_TYPE_PMA,
			SERDES_IREG_TX_DRV_2_REG_NUM);

	*c_plus_1 = ((reg & SERDES_IREG_TX_DRV_2_LEVNM1_MASK) >>
					SERDES_IREG_TX_DRV_2_LEVNM1_SHIFT);

	reg = al_serdes_grp_reg_read(
			&obj->grp_info[grp],
			(enum al_serdes_reg_page)lane,
			AL_SRDS_REG_TYPE_PMA,
			SERDES_IREG_TX_DRV_3_REG_NUM);

	*c_minus_1 = ((reg & SERDES_IREG_TX_DRV_3_LEVNP1_MASK) >>
					SERDES_IREG_TX_DRV_3_LEVNP1_SHIFT);

	reg = al_serdes_grp_reg_read(
			&obj->grp_info[grp],
			(enum al_serdes_reg_page)lane,
			AL_SRDS_REG_TYPE_PMA,
			SERDES_IREG_TX_DRV_1_REG_NUM);

	*c_zero = (((reg & SERDES_IREG_TX_DRV_1_LEVN_MASK) >>
		SERDES_IREG_TX_DRV_1_LEVN_SHIFT) - *c_plus_1 - *c_minus_1);
}

al_bool al_serdes_tx_deemph_inc(
		struct al_serdes_obj	*obj,
		enum al_serdes_group	grp,
		enum al_serdes_lane	lane,
		enum al_serdes_tx_deemph_param param)
{
	al_bool ret = AL_TRUE;
	uint32_t c0;
	uint32_t c1;
	uint32_t c_1;

	al_serdes_tx_deemph_get(obj, grp, lane, &c0, &c1, &c_1);

	al_dbg("%s: current txdeemph: c0 = 0x%x c1 = 0x%x c-1 = 0x%x\n",
		__func__, c0, c1, c_1);

	switch (param) {
	case AL_SERDES_TX_DEEMP_C_ZERO:

		if (c0 == AL_SERDES_TX_DEEMPH_C_ZERO_MAX_VAL)
			return AL_FALSE;

		c0++;

		break;
	case AL_SERDES_TX_DEEMP_C_PLUS:

		if (c1 == AL_SERDES_TX_DEEMPH_C_PLUS_MAX_VAL)
			return AL_FALSE;

		c1++;

		break;
	case AL_SERDES_TX_DEEMP_C_MINUS:

		if (c_1 == AL_SERDES_TX_DEEMPH_C_MINUS_MAX_VAL)
			return AL_FALSE;

		c_1++;

		break;
	}

	if ((c0 + c1 + c_1) > AL_SERDES_TX_DEEMPH_SUM_MAX) {
		al_dbg("%s: sum of all tx de-emphasis over the max limit\n",
			__func__);

		return AL_FALSE;
	}

	al_dbg("%s: new txdeemph: c0 = 0x%x c1 = 0x%x c-1 = 0x%x\n",
		__func__, c0, c1, c_1);

	al_serdes_tx_deemph_set(obj, grp, lane, c0, c1, c_1);

	return ret;
}

al_bool al_serdes_tx_deemph_dec(
		struct al_serdes_obj	*obj,
		enum al_serdes_group	grp,
		enum al_serdes_lane	lane,
		enum al_serdes_tx_deemph_param param)
{
	al_bool ret = AL_TRUE;
	uint32_t c0;
	uint32_t c1;
	uint32_t c_1;

	al_serdes_tx_deemph_get(obj, grp, lane, &c0, &c1, &c_1);

	al_dbg("%s: current txdeemph: c0 = 0x%x c1 = 0x%x c-1 = 0x%x\n",
		__func__, c0, c1, c_1);

	switch (param) {
	case AL_SERDES_TX_DEEMP_C_ZERO:

		if (c0 == AL_SERDES_TX_DEEMPH_C_ZERO_MIN_VAL)
			return AL_FALSE;

		c0--;

		break;
	case AL_SERDES_TX_DEEMP_C_PLUS:

		if (c1 == AL_SERDES_TX_DEEMPH_C_PLUS_MIN_VAL)
			return AL_FALSE;

		c1--;

		break;
	case AL_SERDES_TX_DEEMP_C_MINUS:

		if (c_1 == AL_SERDES_TX_DEEMPH_C_MINUS_MIN_VAL)
			return AL_FALSE;

		c_1--;

		break;
	}

	al_dbg("%s: new txdeemph: c0 = 0x%x c1 = 0x%x c-1 = 0x%x\n",
		__func__, c0, c1, c_1);

	al_serdes_tx_deemph_set(obj, grp, lane, c0, c1, c_1);

	return ret;
}

void al_serdes_tx_deemph_preset(
		struct al_serdes_obj	*obj,
		enum al_serdes_group	grp,
		enum al_serdes_lane	lane)
{
	uint32_t c0;
	uint32_t c1;
	uint32_t c_1;

	c0 = AL_SERDES_TX_DEEMPH_C_ZERO_PRESET;

	c1 = AL_SERDES_TX_DEEMPH_C_PLUS_PRESET;

	c_1 = AL_SERDES_TX_DEEMPH_C_MINUS_PRESET;

	al_dbg("preset: new txdeemph: c0 = 0x%x c1 = 0x%x c-1 = 0x%x\n",
		c0, c1, c_1);

	al_serdes_tx_deemph_set(obj, grp, lane, c0, c1, c_1);
}

al_bool al_serdes_signal_is_detected(
		struct al_serdes_obj	*obj,
		enum al_serdes_group	grp,
		enum al_serdes_lane	lane)
{
	uint32_t reg = 0;

	reg = al_serdes_grp_reg_read(
			&obj->grp_info[grp],
			(enum al_serdes_reg_page)lane,
			AL_SRDS_REG_TYPE_PMA,
			SERDES_IREG_FLD_RXRANDET_REG_NUM);

	return ((reg & SERDES_IREG_FLD_RXRANDET_STAT) ? AL_TRUE : AL_FALSE);
}

void al_serdes_tx_advanced_params_set(struct al_serdes_obj		*obj,
				      enum al_serdes_group		grp,
				      enum al_serdes_lane		lane,
				      struct al_serdes_adv_tx_params	*params)
{
	uint8_t reg = 0;

	if(!params->override) {
		al_serdes_grp_reg_masked_write(&obj->grp_info[grp],
			(enum al_serdes_reg_page)lane,
			AL_SRDS_REG_TYPE_PMA,
			SERDES_IREG_FLD_TX_DRV_OVERRIDE_EN_REG_NUM,
			SERDES_IREG_FLD_TX_DRV_OVERRIDE_EN,
			SERDES_IREG_FLD_TX_DRV_OVERRIDE_EN);

		return;
	}

	al_serdes_grp_reg_masked_write(&obj->grp_info[grp],
			(enum al_serdes_reg_page)lane,
			AL_SRDS_REG_TYPE_PMA,
			SERDES_IREG_FLD_TX_DRV_OVERRIDE_EN_REG_NUM,
			SERDES_IREG_FLD_TX_DRV_OVERRIDE_EN,
			0);

	AL_REG_FIELD_SET(reg,
			 SERDES_IREG_TX_DRV_1_HLEV_MASK,
			 SERDES_IREG_TX_DRV_1_HLEV_SHIFT,
			 params->amp);

	AL_REG_FIELD_SET(reg,
			 SERDES_IREG_TX_DRV_1_LEVN_MASK,
			 SERDES_IREG_TX_DRV_1_LEVN_SHIFT,
			 params->total_driver_units);

	al_serdes_grp_reg_write(&obj->grp_info[grp],
				(enum al_serdes_reg_page)lane,
				AL_SRDS_REG_TYPE_PMA,
				SERDES_IREG_TX_DRV_1_REG_NUM,
				reg);

	reg = 0;
	AL_REG_FIELD_SET(reg,
			 SERDES_IREG_TX_DRV_2_LEVNM1_MASK,
			 SERDES_IREG_TX_DRV_2_LEVNM1_SHIFT,
			 params->c_plus_1);

	AL_REG_FIELD_SET(reg,
			 SERDES_IREG_TX_DRV_2_LEVNM2_MASK,
			 SERDES_IREG_TX_DRV_2_LEVNM2_SHIFT,
			 params->c_plus_2);

	al_serdes_grp_reg_write(&obj->grp_info[grp],
				(enum al_serdes_reg_page)lane,
				AL_SRDS_REG_TYPE_PMA,
				SERDES_IREG_TX_DRV_2_REG_NUM,
				reg);

	reg = 0;
	AL_REG_FIELD_SET(reg,
			 SERDES_IREG_TX_DRV_3_LEVNP1_MASK,
			 SERDES_IREG_TX_DRV_3_LEVNP1_SHIFT,
			 params->c_minus_1);

	AL_REG_FIELD_SET(reg,
			 SERDES_IREG_TX_DRV_3_SLEW_MASK,
			 SERDES_IREG_TX_DRV_3_SLEW_SHIFT,
			 params->slew_rate);

	al_serdes_grp_reg_write(&obj->grp_info[grp],
				(enum al_serdes_reg_page)lane,
				AL_SRDS_REG_TYPE_PMA,
				SERDES_IREG_TX_DRV_3_REG_NUM,
				reg);

}

void al_serdes_tx_advanced_params_get(struct al_serdes_obj	     	*obj,
				      enum al_serdes_group	      	grp,
				      enum al_serdes_lane		lane,
				      struct al_serdes_adv_tx_params *tx_params)
{
	uint8_t reg_val = 0;

	al_serdes_reg_read(obj, grp, (enum al_serdes_reg_page)lane, AL_SRDS_REG_TYPE_PMA,
			SERDES_IREG_TX_DRV_1_REG_NUM,
			&reg_val);
	tx_params->amp = (reg_val & SERDES_IREG_TX_DRV_1_HLEV_MASK) >>
				SERDES_IREG_TX_DRV_1_HLEV_SHIFT;
	tx_params->total_driver_units = (reg_val &
					SERDES_IREG_TX_DRV_1_LEVN_MASK) >>
					SERDES_IREG_TX_DRV_1_LEVN_SHIFT;

	al_serdes_reg_read(obj, grp, (enum al_serdes_reg_page)lane, AL_SRDS_REG_TYPE_PMA,
			SERDES_IREG_TX_DRV_2_REG_NUM,
			&reg_val);
	tx_params->c_plus_1 = (reg_val & SERDES_IREG_TX_DRV_2_LEVNM1_MASK) >>
				SERDES_IREG_TX_DRV_2_LEVNM1_SHIFT;
	tx_params->c_plus_2 = (reg_val & SERDES_IREG_TX_DRV_2_LEVNM2_MASK) >>
				SERDES_IREG_TX_DRV_2_LEVNM2_SHIFT;

	al_serdes_reg_read(obj, grp, (enum al_serdes_reg_page)lane, AL_SRDS_REG_TYPE_PMA,
			SERDES_IREG_TX_DRV_3_REG_NUM,
			&reg_val);
	tx_params->c_minus_1 = (reg_val & SERDES_IREG_TX_DRV_3_LEVNP1_MASK) >>
				SERDES_IREG_TX_DRV_3_LEVNP1_SHIFT;
	tx_params->slew_rate = (reg_val & SERDES_IREG_TX_DRV_3_SLEW_MASK) >>
				SERDES_IREG_TX_DRV_3_SLEW_SHIFT;

	al_serdes_reg_read(obj, grp, (enum al_serdes_reg_page)lane, AL_SRDS_REG_TYPE_PMA,
			SERDES_IREG_FLD_TX_DRV_OVERRIDE_EN_REG_NUM,
			&reg_val);
	tx_params->override = ((reg_val & SERDES_IREG_FLD_TX_DRV_OVERRIDE_EN) == 0);
}


void al_serdes_rx_advanced_params_set(struct al_serdes_obj	      *obj,
				      enum al_serdes_group	      grp,
				      enum al_serdes_lane	      lane,
				      struct al_serdes_adv_rx_params  *params)
{
	uint8_t reg = 0;

	if(!params->override) {
		al_serdes_grp_reg_masked_write(&obj->grp_info[grp],
			(enum al_serdes_reg_page)lane,
			AL_SRDS_REG_TYPE_PMA,
			SERDES_IREG_FLD_RX_DRV_OVERRIDE_EN_REG_NUM,
			SERDES_IREG_FLD_RX_DRV_OVERRIDE_EN,
			SERDES_IREG_FLD_RX_DRV_OVERRIDE_EN);

		return;
	}

	al_serdes_grp_reg_masked_write(&obj->grp_info[grp],
			(enum al_serdes_reg_page)lane,
			AL_SRDS_REG_TYPE_PMA,
			SERDES_IREG_FLD_RX_DRV_OVERRIDE_EN_REG_NUM,
			SERDES_IREG_FLD_RX_DRV_OVERRIDE_EN,
			0);

	AL_REG_FIELD_SET(reg,
			 SERDES_IREG_RX_CALEQ_1_DCGAIN_MASK,
			 SERDES_IREG_RX_CALEQ_1_DCGAIN_SHIFT,
			 params->dcgain);

	AL_REG_FIELD_SET(reg,
			 SERDES_IREG_RX_CALEQ_1_DFEPSTAP3DB_MASK,
			 SERDES_IREG_RX_CALEQ_1_DFEPSTAP3DB_SHIFT,
			 params->dfe_3db_freq);

	al_serdes_grp_reg_write(&obj->grp_info[grp],
				(enum al_serdes_reg_page)lane,
				AL_SRDS_REG_TYPE_PMA,
				SERDES_IREG_RX_CALEQ_1_REG_NUM,
				reg);

	reg = 0;
	AL_REG_FIELD_SET(reg,
			 SERDES_IREG_RX_CALEQ_2_DFEPSTAPGAIN_MASK,
			 SERDES_IREG_RX_CALEQ_2_DFEPSTAPGAIN_SHIFT,
			 params->dfe_gain);

	AL_REG_FIELD_SET(reg,
			 SERDES_IREG_RX_CALEQ_2_DFETAP1GAIN_MASK,
			 SERDES_IREG_RX_CALEQ_2_DFETAP1GAIN_SHIFT,
			 params->dfe_first_tap_ctrl);

	al_serdes_grp_reg_write(&obj->grp_info[grp],
				(enum al_serdes_reg_page)lane,
				AL_SRDS_REG_TYPE_PMA,
				SERDES_IREG_RX_CALEQ_2_REG_NUM,
				reg);

	reg = 0;
	AL_REG_FIELD_SET(reg,
			 SERDES_IREG_RX_CALEQ_3_DFETAP2GAIN_MASK,
			 SERDES_IREG_RX_CALEQ_3_DFETAP2GAIN_SHIFT,
			 params->dfe_secound_tap_ctrl);

	AL_REG_FIELD_SET(reg,
			 SERDES_IREG_RX_CALEQ_3_DFETAP3GAIN_MASK,
			 SERDES_IREG_RX_CALEQ_3_DFETAP3GAIN_SHIFT,
			 params->dfe_third_tap_ctrl);

	al_serdes_grp_reg_write(&obj->grp_info[grp],
				(enum al_serdes_reg_page)lane,
				AL_SRDS_REG_TYPE_PMA,
				SERDES_IREG_RX_CALEQ_3_REG_NUM,
				reg);

	reg = 0;
	AL_REG_FIELD_SET(reg,
			 SERDES_IREG_RX_CALEQ_4_DFETAP4GAIN_MASK,
			 SERDES_IREG_RX_CALEQ_4_DFETAP4GAIN_SHIFT,
			 params->dfe_fourth_tap_ctrl);

	AL_REG_FIELD_SET(reg,
			 SERDES_IREG_RX_CALEQ_4_LOFREQAGCGAIN_MASK,
			 SERDES_IREG_RX_CALEQ_4_LOFREQAGCGAIN_SHIFT,
			 params->low_freq_agc_gain);

	al_serdes_grp_reg_write(&obj->grp_info[grp],
				(enum al_serdes_reg_page)lane,
				AL_SRDS_REG_TYPE_PMA,
				SERDES_IREG_RX_CALEQ_4_REG_NUM,
				reg);

	reg = 0;
	AL_REG_FIELD_SET(reg,
			 SERDES_IREG_RX_CALEQ_5_PRECAL_CODE_SEL_MASK,
			 SERDES_IREG_RX_CALEQ_5_PRECAL_CODE_SEL_SHIFT,
			 params->precal_code_sel);

	AL_REG_FIELD_SET(reg,
			 SERDES_IREG_RX_CALEQ_5_HIFREQAGCCAP_MASK,
			 SERDES_IREG_RX_CALEQ_5_HIFREQAGCCAP_SHIFT,
			 params->high_freq_agc_boost);

	al_serdes_grp_reg_write(&obj->grp_info[grp],
				(enum al_serdes_reg_page)lane,
				AL_SRDS_REG_TYPE_PMA,
				SERDES_IREG_RX_CALEQ_5_REG_NUM,
				reg);
}

static inline void al_serdes_ns_delay(int cnt)
{
	al_udelay((cnt + 999) / 1000);
}

static inline void al_serdes_common_cfg_eth(struct al_serdes_group_info *grp_info)
{
	al_serdes_grp_reg_masked_write(
				grp_info,
				AL_SRDS_REG_PAGE_4_COMMON,
				AL_SRDS_REG_TYPE_PMA,
				SERDES_IREG_FLD_RXEQ_LOOKUP_CODE_EN_REG_NUM,
				SERDES_IREG_FLD_RXEQ_LOOKUP_CODE_EN_MASK,
				(0x1 << SERDES_IREG_FLD_RXEQ_LOOKUP_CODE_EN_SHIFT));

	al_serdes_grp_reg_masked_write(
				grp_info,
				AL_SRDS_REG_PAGE_4_COMMON,
				AL_SRDS_REG_TYPE_PMA,
				SERDES_IREG_FLD_RXEQ_LOOKUP_LASTCODE_REG_NUM,
				SERDES_IREG_FLD_RXEQ_LOOKUP_LASTCODE_MASK,
				(0 << SERDES_IREG_FLD_RXEQ_LOOKUP_LASTCODE_SHIFT));

	al_serdes_grp_reg_masked_write(
				grp_info,
				AL_SRDS_REG_PAGE_4_COMMON,
				AL_SRDS_REG_TYPE_PMA,
				SERDES_IREG_FLD_RXEQ_COARSE_RUN1_MASK_REG_NUM,
				SERDES_IREG_FLD_RXEQ_COARSE_RUN1_MASK_MASK,
				(0x2 << SERDES_IREG_FLD_RXEQ_COARSE_RUN1_MASK_SHIFT));

	al_serdes_grp_reg_masked_write(
				grp_info,
				AL_SRDS_REG_PAGE_4_COMMON,
				AL_SRDS_REG_TYPE_PMA,
				SERDES_IREG_FLD_RXEQ_COARSE_RUN2_MASK_REG_NUM,
				SERDES_IREG_FLD_RXEQ_COARSE_RUN2_MASK_MASK,
				(0 << SERDES_IREG_FLD_RXEQ_COARSE_RUN2_MASK_SHIFT));

	al_serdes_grp_reg_masked_write(
				grp_info,
				AL_SRDS_REG_PAGE_4_COMMON,
				AL_SRDS_REG_TYPE_PMA,
				SERDES_IREG_FLD_RXEQ_COARSE_STEP_REG_NUM,
				SERDES_IREG_FLD_RXEQ_COARSE_STEP_MASK,
				(0x1 << SERDES_IREG_FLD_RXEQ_COARSE_STEP_SHIFT));

	al_serdes_grp_reg_masked_write(
				grp_info,
				AL_SRDS_REG_PAGE_4_COMMON,
				AL_SRDS_REG_TYPE_PMA,
				SERDES_IREG_FLD_RXEQ_COARSE_ITER_NUM_REG_NUM,
				SERDES_IREG_FLD_RXEQ_COARSE_ITER_NUM_MASK,
				(0x1 << SERDES_IREG_FLD_RXEQ_COARSE_ITER_NUM_SHIFT));

	al_serdes_grp_reg_masked_write(
				grp_info,
				AL_SRDS_REG_PAGE_4_COMMON,
				AL_SRDS_REG_TYPE_PMA,
				SERDES_IREG_FLD_RXEQ_FINE_RUN1_MASK_REG_NUM,
				SERDES_IREG_FLD_RXEQ_FINE_RUN1_MASK_MASK,
				(0xf0 << SERDES_IREG_FLD_RXEQ_FINE_RUN1_MASK_SHIFT));

	al_serdes_grp_reg_masked_write(
				grp_info,
				AL_SRDS_REG_PAGE_4_COMMON,
				AL_SRDS_REG_TYPE_PMA,
				SERDES_IREG_FLD_RXEQ_FINE_RUN2_MASK_REG_NUM,
				SERDES_IREG_FLD_RXEQ_FINE_RUN2_MASK_MASK,
				(0 << SERDES_IREG_FLD_RXEQ_FINE_RUN2_MASK_SHIFT));

	al_serdes_grp_reg_masked_write(
				grp_info,
				AL_SRDS_REG_PAGE_4_COMMON,
				AL_SRDS_REG_TYPE_PMA,
				SERDES_IREG_FLD_RXEQ_FINE_STEP_REG_NUM,
				SERDES_IREG_FLD_RXEQ_FINE_STEP_MASK,
				(1 << SERDES_IREG_FLD_RXEQ_FINE_STEP_SHIFT));

	al_serdes_grp_reg_masked_write(
				grp_info,
				AL_SRDS_REG_PAGE_4_COMMON,
				AL_SRDS_REG_TYPE_PMA,
				SERDES_IREG_FLD_RXEQ_FINE_ITER_NUM_REG_NUM,
				SERDES_IREG_FLD_RXEQ_FINE_ITER_NUM_MASK,
				(0x8 << SERDES_IREG_FLD_RXEQ_FINE_ITER_NUM_SHIFT));

	al_serdes_grp_reg_masked_write(
				grp_info,
				AL_SRDS_REG_PAGE_4_COMMON,
				AL_SRDS_REG_TYPE_PMA,
				SERDES_IREG_FLD_RXCALEYEDIAGFSM_BERTHRESHOLD1_REG_NUM,
				SERDES_IREG_FLD_RXCALEYEDIAGFSM_BERTHRESHOLD1_MASK,
				(0 << SERDES_IREG_FLD_RXCALEYEDIAGFSM_BERTHRESHOLD1_SHIFT));

	al_serdes_grp_reg_masked_write(
				grp_info,
				AL_SRDS_REG_PAGE_4_COMMON,
				AL_SRDS_REG_TYPE_PMA,
				SERDES_IREG_FLD_RXCALEYEDIAGFSM_BERTHRESHOLD2_REG_NUM,
				SERDES_IREG_FLD_RXCALEYEDIAGFSM_BERTHRESHOLD2_MASK,
				(0x64 << SERDES_IREG_FLD_RXCALEYEDIAGFSM_BERTHRESHOLD2_SHIFT));

	al_serdes_grp_reg_masked_write(
				grp_info,
				AL_SRDS_REG_PAGE_4_COMMON,
				AL_SRDS_REG_TYPE_PMA,
				SERDES_IREG_FLD_RXCALEYEDIAGFSM_XVALCOARSE_REG_NUM,
				SERDES_IREG_FLD_RXCALEYEDIAGFSM_XVALCOARSE_MASK,
				(0x3 << SERDES_IREG_FLD_RXCALEYEDIAGFSM_XVALCOARSE_SHIFT));

	al_serdes_grp_reg_masked_write(
				grp_info,
				AL_SRDS_REG_PAGE_4_COMMON,
				AL_SRDS_REG_TYPE_PMA,
				SERDES_IREG_FLD_RXCALEYEDIAGFSM_XVALFINE_REG_NUM,
				SERDES_IREG_FLD_RXCALEYEDIAGFSM_XVALFINE_MASK,
				(0x1 << SERDES_IREG_FLD_RXCALEYEDIAGFSM_XVALFINE_SHIFT));

	al_serdes_grp_reg_masked_write(
				grp_info,
				AL_SRDS_REG_PAGE_4_COMMON,
				AL_SRDS_REG_TYPE_PMA,
				SERDES_IREG_FLD_RXCALEYEDIAGFSM_YVALCOARSE_REG_NUM,
				SERDES_IREG_FLD_RXCALEYEDIAGFSM_YVALCOARSE_MASK,
				(3 << SERDES_IREG_FLD_RXCALEYEDIAGFSM_YVALCOARSE_SHIFT));

	al_serdes_grp_reg_masked_write(
				grp_info,
				AL_SRDS_REG_PAGE_4_COMMON,
				AL_SRDS_REG_TYPE_PMA,
				SERDES_IREG_FLD_RXCALEYEDIAGFSM_YVALFINE_REG_NUM,
				SERDES_IREG_FLD_RXCALEYEDIAGFSM_YVALFINE_MASK,
				(1 << SERDES_IREG_FLD_RXCALEYEDIAGFSM_YVALFINE_SHIFT));

	al_serdes_grp_reg_masked_write(
				grp_info,
				AL_SRDS_REG_PAGE_4_COMMON,
				AL_SRDS_REG_TYPE_PMA,
				SERDES_IREG_FLD_EYE_DIAG_SAMPLE_CNT_MSB_REG_NUM,
				SERDES_IREG_FLD_EYE_DIAG_SAMPLE_CNT_MSB_MASK,
				(0xc << SERDES_IREG_FLD_EYE_DIAG_SAMPLE_CNT_MSB_SHIFT));

	al_serdes_grp_reg_masked_write(
				grp_info,
				AL_SRDS_REG_PAGE_4_COMMON,
				AL_SRDS_REG_TYPE_PMA,
				SERDES_IREG_FLD_EYE_DIAG_SAMPLE_CNT_LSB_REG_NUM,
				SERDES_IREG_FLD_EYE_DIAG_SAMPLE_CNT_LSB_MASK,
				(0xcc << SERDES_IREG_FLD_EYE_DIAG_SAMPLE_CNT_LSB_SHIFT));
}

struct al_serdes_mode_rx_tx_inv_state {
	al_bool		restore;
	uint32_t	pipe_rst;
	uint32_t	ipd_multi[AL_SRDS_NUM_LANES];
	uint8_t		inv_value[AL_SRDS_NUM_LANES];
};

static void al_serdes_mode_rx_tx_inv_state_save(
	struct al_serdes_group_info		*grp_info,
	struct al_serdes_mode_rx_tx_inv_state	*state)
{
	if (al_reg_read32(&grp_info->regs_base->gen.irst) & SERDES_GEN_IRST_POR_B_A) {
		int i;

		state->restore = AL_TRUE;
		state->pipe_rst = al_reg_read32(&grp_info->regs_base->gen.irst);

		for (i = 0; i < AL_SRDS_NUM_LANES; i++) {
			state->inv_value[i] = al_serdes_grp_reg_read(
				grp_info,
				i,
				AL_SRDS_REG_TYPE_PMA,
				SERDES_IREG_FLD_POLARITY_RX_REG_NUM);
			state->ipd_multi[i] =
				al_reg_read32(&grp_info->regs_base->lane[i].ipd_multi);
		}
	} else {
		state->restore = AL_FALSE;
	}
}

static void al_serdes_mode_rx_tx_inv_state_restore(
	struct al_serdes_group_info		*grp_info,
	struct al_serdes_mode_rx_tx_inv_state	*state)
{
	if (state->restore) {
		int i;

		for (i = 0; i < AL_SRDS_NUM_LANES; i++) {
			al_serdes_grp_reg_write(
				grp_info,
				i,
				AL_SRDS_REG_TYPE_PMA,
				SERDES_IREG_FLD_POLARITY_RX_REG_NUM,
				state->inv_value[i]);
			al_reg_write32(
				&grp_info->regs_base->lane[i].ipd_multi, state->ipd_multi[i]);
			al_reg_write32_masked(
				&grp_info->regs_base->gen.irst,
				(SERDES_GEN_IRST_PIPE_RST_L0_B_A_SEL >> i) |
				(SERDES_GEN_IRST_PIPE_RST_L0_B_A >> i),
				state->pipe_rst);
		}
	}
}

void al_serdes_mode_set_sgmii(
	struct al_serdes_obj	*obj,
	enum al_serdes_group	grp)
{
	struct al_serdes_group_info *grp_info;
	struct al_serdes_mode_rx_tx_inv_state rx_tx_inv_state;

	al_assert(obj);
	al_assert(((int)grp) >= AL_SRDS_GRP_A);
	al_assert(((int)grp) <= AL_SRDS_GRP_D);

	grp_info = &obj->grp_info[grp];

	al_serdes_mode_rx_tx_inv_state_save(grp_info, &rx_tx_inv_state);

	al_reg_write32(&grp_info->regs_base->gen.irst, 0x000000);
	al_reg_write32(&grp_info->regs_base->lane[0].ictl_multi, 0x10110010);
	al_reg_write32(&grp_info->regs_base->lane[1].ictl_multi, 0x10110010);
	al_reg_write32(&grp_info->regs_base->lane[2].ictl_multi, 0x10110010);
	al_reg_write32(&grp_info->regs_base->lane[3].ictl_multi, 0x10110010);
	al_reg_write32(&grp_info->regs_base->gen.ipd_multi_synth , 0x0001);
	al_reg_write32(&grp_info->regs_base->lane[0].ipd_multi, 0x0003);
	al_reg_write32(&grp_info->regs_base->lane[1].ipd_multi, 0x0003);
	al_reg_write32(&grp_info->regs_base->lane[2].ipd_multi, 0x0003);
	al_reg_write32(&grp_info->regs_base->lane[3].ipd_multi, 0x0003);
	al_reg_write32(&grp_info->regs_base->gen.ictl_pcs , 0);
	al_reg_write32(&grp_info->regs_base->gen.irst, 0x001000);
	al_serdes_ns_delay(800);
	al_reg_write32(&grp_info->regs_base->gen.irst, 0x000000);
	al_serdes_ns_delay(500);
	al_reg_write32(&grp_info->regs_base->gen.irst, 0x001000);
	al_serdes_ns_delay(500);

	al_serdes_grp_reg_write(grp_info, AL_SRDS_REG_PAGE_0123_LANES_0123,
		AL_SRDS_REG_TYPE_PMA, 101, 183);
	al_serdes_grp_reg_write(grp_info, AL_SRDS_REG_PAGE_0123_LANES_0123,
		AL_SRDS_REG_TYPE_PMA, 102, 183);
	al_serdes_grp_reg_write(grp_info, AL_SRDS_REG_PAGE_0123_LANES_0123,
		AL_SRDS_REG_TYPE_PMA, 103, 12);
	al_serdes_grp_reg_write(grp_info, AL_SRDS_REG_PAGE_0123_LANES_0123,
		AL_SRDS_REG_TYPE_PMA, 104, 12);
	al_serdes_grp_reg_write(grp_info, AL_SRDS_REG_PAGE_0123_LANES_0123,
		AL_SRDS_REG_TYPE_PMA, 105, 26);
	al_serdes_grp_reg_write(grp_info, AL_SRDS_REG_PAGE_0123_LANES_0123,
		AL_SRDS_REG_TYPE_PMA, 106, 26);
	al_serdes_grp_reg_write(grp_info, AL_SRDS_REG_PAGE_0123_LANES_0123,
		AL_SRDS_REG_TYPE_PMA, 107, 2);
	al_serdes_grp_reg_write(grp_info, AL_SRDS_REG_PAGE_0123_LANES_0123,
		AL_SRDS_REG_TYPE_PMA, 108, 2);
	al_serdes_grp_reg_write(grp_info, AL_SRDS_REG_PAGE_0123_LANES_0123,
		AL_SRDS_REG_TYPE_PMA, 109, 17);
	al_serdes_grp_reg_write(grp_info, AL_SRDS_REG_PAGE_0123_LANES_0123,
		AL_SRDS_REG_TYPE_PMA, 110, 13);
	al_serdes_grp_reg_write(grp_info, AL_SRDS_REG_PAGE_4_COMMON,
		AL_SRDS_REG_TYPE_PMA, 101, 153);
	al_serdes_grp_reg_write(grp_info, AL_SRDS_REG_PAGE_4_COMMON,
		AL_SRDS_REG_TYPE_PMA, 102, 0);
	al_serdes_grp_reg_write(grp_info, AL_SRDS_REG_PAGE_4_COMMON,
		AL_SRDS_REG_TYPE_PMA, 103, 108);
	al_serdes_grp_reg_write(grp_info, AL_SRDS_REG_PAGE_4_COMMON,
		AL_SRDS_REG_TYPE_PMA, 104, 183);
	al_serdes_grp_reg_write(grp_info, AL_SRDS_REG_PAGE_4_COMMON,
		AL_SRDS_REG_TYPE_PMA, 105, 183);
	al_serdes_grp_reg_write(grp_info, AL_SRDS_REG_PAGE_4_COMMON,
		AL_SRDS_REG_TYPE_PMA, 106, 12);
	al_serdes_grp_reg_write(grp_info, AL_SRDS_REG_PAGE_4_COMMON,
		AL_SRDS_REG_TYPE_PMA, 107, 12);
	al_serdes_grp_reg_write(grp_info, AL_SRDS_REG_PAGE_4_COMMON,
		AL_SRDS_REG_TYPE_PMA, 108, 26);
	al_serdes_grp_reg_write(grp_info, AL_SRDS_REG_PAGE_4_COMMON,
		AL_SRDS_REG_TYPE_PMA, 109, 26);
	al_serdes_grp_reg_write(grp_info, AL_SRDS_REG_PAGE_4_COMMON,
		AL_SRDS_REG_TYPE_PMA, 110, 7);
	al_serdes_grp_reg_write(grp_info, AL_SRDS_REG_PAGE_4_COMMON,
		AL_SRDS_REG_TYPE_PMA, 111, 12);
	al_serdes_grp_reg_write(grp_info, AL_SRDS_REG_PAGE_4_COMMON,
		AL_SRDS_REG_TYPE_PMA, 112, 8);
	al_serdes_grp_reg_write(grp_info, AL_SRDS_REG_PAGE_4_COMMON,
		AL_SRDS_REG_TYPE_PMA, 113, 0);
	al_serdes_grp_reg_write(grp_info, AL_SRDS_REG_PAGE_4_COMMON,
		AL_SRDS_REG_TYPE_PMA, 114, 8);
	al_serdes_grp_reg_write(grp_info, AL_SRDS_REG_PAGE_4_COMMON,
		AL_SRDS_REG_TYPE_PMA, 115, 0);
	al_serdes_grp_reg_write(grp_info, AL_SRDS_REG_PAGE_4_COMMON,
		AL_SRDS_REG_TYPE_PMA, 116, 255);
	al_serdes_grp_reg_write(grp_info, AL_SRDS_REG_PAGE_4_COMMON,
		AL_SRDS_REG_TYPE_PMA, 117, 179);
	al_serdes_grp_reg_write(grp_info, AL_SRDS_REG_PAGE_4_COMMON,
		AL_SRDS_REG_TYPE_PMA, 118, 246);
	al_serdes_grp_reg_write(grp_info, AL_SRDS_REG_PAGE_4_COMMON,
		AL_SRDS_REG_TYPE_PMA, 119, 208);
	al_serdes_grp_reg_write(grp_info, AL_SRDS_REG_PAGE_4_COMMON,
		AL_SRDS_REG_TYPE_PMA, 120, 239);
	al_serdes_grp_reg_write(grp_info, AL_SRDS_REG_PAGE_4_COMMON,
		AL_SRDS_REG_TYPE_PMA, 121, 251);
	al_serdes_grp_reg_write(grp_info, AL_SRDS_REG_PAGE_4_COMMON,
		AL_SRDS_REG_TYPE_PMA, 122, 255);
	al_serdes_grp_reg_write(grp_info, AL_SRDS_REG_PAGE_4_COMMON,
		AL_SRDS_REG_TYPE_PMA, 123, 255);
	al_serdes_grp_reg_write(grp_info, AL_SRDS_REG_PAGE_4_COMMON,
		AL_SRDS_REG_TYPE_PMA, 124, 255);
	al_serdes_grp_reg_write(grp_info, AL_SRDS_REG_PAGE_4_COMMON,
		AL_SRDS_REG_TYPE_PMA, 125, 255);
	al_serdes_grp_reg_write(grp_info, AL_SRDS_REG_PAGE_4_COMMON,
		AL_SRDS_REG_TYPE_PMA, 126, 255);
	al_serdes_grp_reg_write(grp_info, AL_SRDS_REG_PAGE_4_COMMON,
		AL_SRDS_REG_TYPE_PMA, 127, 211);
	al_serdes_grp_reg_write(grp_info, AL_SRDS_REG_PAGE_4_COMMON,
		AL_SRDS_REG_TYPE_PMA, 128, 211);
	al_serdes_grp_reg_write(grp_info, AL_SRDS_REG_PAGE_4_COMMON,
		AL_SRDS_REG_TYPE_PMA, 129, 226);
	al_serdes_grp_reg_write(grp_info, AL_SRDS_REG_PAGE_4_COMMON,
		AL_SRDS_REG_TYPE_PMA, 130, 239);
	al_serdes_grp_reg_write(grp_info, AL_SRDS_REG_PAGE_4_COMMON,
		AL_SRDS_REG_TYPE_PMA, 131, 251);
	al_serdes_grp_reg_write(grp_info, AL_SRDS_REG_PAGE_4_COMMON,
		AL_SRDS_REG_TYPE_PMA, 132, 251);
	al_serdes_grp_reg_write(grp_info, AL_SRDS_REG_PAGE_4_COMMON,
		AL_SRDS_REG_TYPE_PMA, 133, 255);
	al_serdes_grp_reg_write(grp_info, AL_SRDS_REG_PAGE_4_COMMON,
		AL_SRDS_REG_TYPE_PMA, 134, 239);
	al_serdes_grp_reg_write(grp_info, AL_SRDS_REG_PAGE_4_COMMON,
		AL_SRDS_REG_TYPE_PMA, 135, 255);
	al_serdes_grp_reg_write(grp_info, AL_SRDS_REG_PAGE_4_COMMON,
		AL_SRDS_REG_TYPE_PMA, 136, 255);
	al_serdes_grp_reg_write(grp_info, AL_SRDS_REG_PAGE_4_COMMON,
		AL_SRDS_REG_TYPE_PMA, 137, 211);
	al_serdes_grp_reg_write(grp_info, AL_SRDS_REG_PAGE_4_COMMON,
		AL_SRDS_REG_TYPE_PMA, 138, 211);
	al_serdes_grp_reg_write(grp_info, AL_SRDS_REG_PAGE_4_COMMON,
		AL_SRDS_REG_TYPE_PMA, 139, 226);
	al_serdes_grp_reg_write(grp_info, AL_SRDS_REG_PAGE_4_COMMON,
		AL_SRDS_REG_TYPE_PMA, 140, 239);
	al_serdes_grp_reg_write(grp_info, AL_SRDS_REG_PAGE_4_COMMON,
		AL_SRDS_REG_TYPE_PMA, 141, 251);
	al_serdes_grp_reg_write(grp_info, AL_SRDS_REG_PAGE_4_COMMON,
		AL_SRDS_REG_TYPE_PMA, 142, 251);
	al_serdes_grp_reg_write(grp_info, AL_SRDS_REG_PAGE_4_COMMON,
		AL_SRDS_REG_TYPE_PMA, 143, 255);
	al_serdes_grp_reg_write(grp_info, AL_SRDS_REG_PAGE_4_COMMON,
		AL_SRDS_REG_TYPE_PMA, 144, 239);
	al_serdes_grp_reg_write(grp_info, AL_SRDS_REG_PAGE_4_COMMON,
		AL_SRDS_REG_TYPE_PMA, 145, 255);
	al_serdes_grp_reg_write(grp_info, AL_SRDS_REG_PAGE_4_COMMON,
		AL_SRDS_REG_TYPE_PMA, 146, 255);
	al_serdes_grp_reg_write(grp_info, AL_SRDS_REG_PAGE_4_COMMON,
		AL_SRDS_REG_TYPE_PMA, 147, 251);
	al_serdes_grp_reg_write(grp_info, AL_SRDS_REG_PAGE_4_COMMON,
		AL_SRDS_REG_TYPE_PMA, 148, 255);
	al_serdes_grp_reg_write(grp_info, AL_SRDS_REG_PAGE_4_COMMON,
		AL_SRDS_REG_TYPE_PMA, 149, 63);
	al_serdes_grp_reg_write(grp_info, AL_SRDS_REG_PAGE_4_COMMON,
		AL_SRDS_REG_TYPE_PMA, 150, 0);
	al_serdes_grp_reg_write(grp_info, AL_SRDS_REG_PAGE_4_COMMON,
		AL_SRDS_REG_TYPE_PMA, 151, 100);
	al_serdes_grp_reg_write(grp_info, AL_SRDS_REG_PAGE_4_COMMON,
		AL_SRDS_REG_TYPE_PMA, 152, 0);
	al_serdes_grp_reg_write(grp_info, AL_SRDS_REG_PAGE_4_COMMON,
		AL_SRDS_REG_TYPE_PMA, 153, 4);
	al_serdes_grp_reg_write(grp_info, AL_SRDS_REG_PAGE_4_COMMON,
		AL_SRDS_REG_TYPE_PMA, 154, 2);
	al_serdes_grp_reg_write(grp_info, AL_SRDS_REG_PAGE_4_COMMON,
		AL_SRDS_REG_TYPE_PMA, 155, 5);
	al_serdes_grp_reg_write(grp_info, AL_SRDS_REG_PAGE_4_COMMON,
		AL_SRDS_REG_TYPE_PMA, 156, 5);
	al_serdes_grp_reg_write(grp_info, AL_SRDS_REG_PAGE_4_COMMON,
		AL_SRDS_REG_TYPE_PMA, 157, 4);
	al_serdes_grp_reg_write(grp_info, AL_SRDS_REG_PAGE_4_COMMON,
		AL_SRDS_REG_TYPE_PMA, 158, 0);
	al_serdes_grp_reg_write(grp_info, AL_SRDS_REG_PAGE_4_COMMON,
		AL_SRDS_REG_TYPE_PMA, 159, 0);
	al_serdes_grp_reg_write(grp_info, AL_SRDS_REG_PAGE_4_COMMON,
		AL_SRDS_REG_TYPE_PMA, 160, 8);
	al_serdes_grp_reg_write(grp_info, AL_SRDS_REG_PAGE_4_COMMON,
		AL_SRDS_REG_TYPE_PMA, 161, 4);
	al_serdes_grp_reg_write(grp_info, AL_SRDS_REG_PAGE_4_COMMON,
		AL_SRDS_REG_TYPE_PMA, 162, 0);
	al_serdes_grp_reg_write(grp_info, AL_SRDS_REG_PAGE_4_COMMON,
		AL_SRDS_REG_TYPE_PMA, 163, 0);
	al_serdes_grp_reg_write(grp_info, AL_SRDS_REG_PAGE_4_COMMON,
		AL_SRDS_REG_TYPE_PMA, 164, 4);
	al_serdes_grp_reg_write(grp_info, AL_SRDS_REG_PAGE_0_LANE_0,
		AL_SRDS_REG_TYPE_PMA, 7, 0);
	al_serdes_grp_reg_write(grp_info, AL_SRDS_REG_PAGE_1_LANE_1,
		AL_SRDS_REG_TYPE_PMA, 7, 0);
	al_serdes_grp_reg_write(grp_info, AL_SRDS_REG_PAGE_2_LANE_2,
		AL_SRDS_REG_TYPE_PMA, 7, 0);
	al_serdes_grp_reg_write(grp_info, AL_SRDS_REG_PAGE_3_LANE_3,
		AL_SRDS_REG_TYPE_PMA, 7, 0);
	al_serdes_grp_reg_write(grp_info, AL_SRDS_REG_PAGE_4_COMMON,
		AL_SRDS_REG_TYPE_PMA, 13, 16);
	al_serdes_grp_reg_write(grp_info, AL_SRDS_REG_PAGE_4_COMMON,
		AL_SRDS_REG_TYPE_PMA, 48, 0);
	al_serdes_grp_reg_write(grp_info, AL_SRDS_REG_PAGE_4_COMMON,
		AL_SRDS_REG_TYPE_PMA, 49, 0);
	al_serdes_grp_reg_write(grp_info, AL_SRDS_REG_PAGE_4_COMMON,
		AL_SRDS_REG_TYPE_PMA, 54, 0);
	al_serdes_grp_reg_write(grp_info, AL_SRDS_REG_PAGE_4_COMMON,
		AL_SRDS_REG_TYPE_PMA, 55, 180);
	al_serdes_grp_reg_write(grp_info, AL_SRDS_REG_PAGE_4_COMMON,
		AL_SRDS_REG_TYPE_PMA, 93, 2);
	al_serdes_grp_reg_write(grp_info, AL_SRDS_REG_PAGE_4_COMMON,
		AL_SRDS_REG_TYPE_PMA, 165, 3);
	al_serdes_grp_reg_write(grp_info, AL_SRDS_REG_PAGE_0123_LANES_0123,
		AL_SRDS_REG_TYPE_PMA, 41, 6);
	al_serdes_grp_reg_write(grp_info, AL_SRDS_REG_PAGE_4_COMMON,
		AL_SRDS_REG_TYPE_PMA, 354, 3);
	al_serdes_grp_reg_write(grp_info, AL_SRDS_REG_PAGE_4_COMMON,
		AL_SRDS_REG_TYPE_PMA, 355, 58);
	al_serdes_grp_reg_write(grp_info, AL_SRDS_REG_PAGE_4_COMMON,
		AL_SRDS_REG_TYPE_PMA, 356, 9);
	al_serdes_grp_reg_write(grp_info, AL_SRDS_REG_PAGE_4_COMMON,
		AL_SRDS_REG_TYPE_PMA, 357, 3);
	al_serdes_grp_reg_write(grp_info, AL_SRDS_REG_PAGE_4_COMMON,
		AL_SRDS_REG_TYPE_PMA, 358, 62);
	al_serdes_grp_reg_write(grp_info, AL_SRDS_REG_PAGE_4_COMMON,
		AL_SRDS_REG_TYPE_PMA, 359, 12);
	al_serdes_grp_reg_write(grp_info, AL_SRDS_REG_PAGE_0123_LANES_0123,
		AL_SRDS_REG_TYPE_PMA, 701, 0);
	al_serdes_grp_reg_write(grp_info, AL_SRDS_REG_PAGE_0123_LANES_0123,
		AL_SRDS_REG_TYPE_PMA, 87, 0x1f);

	al_serdes_common_cfg_eth(grp_info);

	al_serdes_mode_rx_tx_inv_state_restore(grp_info, &rx_tx_inv_state);

	al_reg_write32(&grp_info->regs_base->gen.irst, 0x0011F0);
	al_serdes_ns_delay(500);
}

void al_serdes_mode_set_kr(
	struct al_serdes_obj	*obj,
	enum al_serdes_group	grp)
{
	struct al_serdes_group_info *grp_info;
	struct al_serdes_mode_rx_tx_inv_state rx_tx_inv_state;

	al_assert(obj);
	al_assert(((int)grp) >= AL_SRDS_GRP_A);
	al_assert(((int)grp) <= AL_SRDS_GRP_D);

	grp_info = &obj->grp_info[grp];

	al_serdes_mode_rx_tx_inv_state_save(grp_info, &rx_tx_inv_state);

	al_reg_write32(&grp_info->regs_base->gen.irst, 0x000000);
	al_reg_write32(&grp_info->regs_base->lane[0].ictl_multi, 0x30330030);
	al_reg_write32(&grp_info->regs_base->lane[1].ictl_multi, 0x30330030);
	al_reg_write32(&grp_info->regs_base->lane[2].ictl_multi, 0x30330030);
	al_reg_write32(&grp_info->regs_base->lane[3].ictl_multi, 0x30330030);
	al_reg_write32(&grp_info->regs_base->gen.ipd_multi_synth , 0x0001);
	al_reg_write32(&grp_info->regs_base->lane[0].ipd_multi, 0x0003);
	al_reg_write32(&grp_info->regs_base->lane[1].ipd_multi, 0x0003);
	al_reg_write32(&grp_info->regs_base->lane[2].ipd_multi, 0x0003);
	al_reg_write32(&grp_info->regs_base->lane[3].ipd_multi, 0x0003);
	al_reg_write32(&grp_info->regs_base->gen.ictl_pcs , 0);
	al_reg_write32(&grp_info->regs_base->gen.irst, 0x001000);
	al_serdes_ns_delay(800);
	al_reg_write32(&grp_info->regs_base->gen.irst, 0x000000);
	al_serdes_ns_delay(500);
	al_reg_write32(&grp_info->regs_base->gen.irst, 0x001000);
	al_serdes_ns_delay(500);

	al_serdes_grp_reg_write(grp_info, AL_SRDS_REG_PAGE_0123_LANES_0123,
		AL_SRDS_REG_TYPE_PMA, 101, 189);
	al_serdes_grp_reg_write(grp_info, AL_SRDS_REG_PAGE_0123_LANES_0123,
		AL_SRDS_REG_TYPE_PMA, 102, 189);
	al_serdes_grp_reg_write(grp_info, AL_SRDS_REG_PAGE_0123_LANES_0123,
		AL_SRDS_REG_TYPE_PMA, 103, 6);
	al_serdes_grp_reg_write(grp_info, AL_SRDS_REG_PAGE_0123_LANES_0123,
		AL_SRDS_REG_TYPE_PMA, 104, 6);
	al_serdes_grp_reg_write(grp_info, AL_SRDS_REG_PAGE_0123_LANES_0123,
		AL_SRDS_REG_TYPE_PMA, 105, 27);
	al_serdes_grp_reg_write(grp_info, AL_SRDS_REG_PAGE_0123_LANES_0123,
		AL_SRDS_REG_TYPE_PMA, 106, 27);
	al_serdes_grp_reg_write(grp_info, AL_SRDS_REG_PAGE_0123_LANES_0123,
		AL_SRDS_REG_TYPE_PMA, 107, 1);
	al_serdes_grp_reg_write(grp_info, AL_SRDS_REG_PAGE_0123_LANES_0123,
		AL_SRDS_REG_TYPE_PMA, 108, 1);
	al_serdes_grp_reg_write(grp_info, AL_SRDS_REG_PAGE_0123_LANES_0123,
		AL_SRDS_REG_TYPE_PMA, 109, 119);
	al_serdes_grp_reg_write(grp_info, AL_SRDS_REG_PAGE_0123_LANES_0123,
		AL_SRDS_REG_TYPE_PMA, 110, 5);
	al_serdes_grp_reg_write(grp_info, AL_SRDS_REG_PAGE_4_COMMON,
		AL_SRDS_REG_TYPE_PMA, 101, 170);
	al_serdes_grp_reg_write(grp_info, AL_SRDS_REG_PAGE_4_COMMON,
		AL_SRDS_REG_TYPE_PMA, 102, 0);
	al_serdes_grp_reg_write(grp_info, AL_SRDS_REG_PAGE_4_COMMON,
		AL_SRDS_REG_TYPE_PMA, 103, 108);
	al_serdes_grp_reg_write(grp_info, AL_SRDS_REG_PAGE_4_COMMON,
		AL_SRDS_REG_TYPE_PMA, 104, 189);
	al_serdes_grp_reg_write(grp_info, AL_SRDS_REG_PAGE_4_COMMON,
		AL_SRDS_REG_TYPE_PMA, 105, 189);
	al_serdes_grp_reg_write(grp_info, AL_SRDS_REG_PAGE_4_COMMON,
		AL_SRDS_REG_TYPE_PMA, 106, 6);
	al_serdes_grp_reg_write(grp_info, AL_SRDS_REG_PAGE_4_COMMON,
		AL_SRDS_REG_TYPE_PMA, 107, 6);
	al_serdes_grp_reg_write(grp_info, AL_SRDS_REG_PAGE_4_COMMON,
		AL_SRDS_REG_TYPE_PMA, 108, 27);
	al_serdes_grp_reg_write(grp_info, AL_SRDS_REG_PAGE_4_COMMON,
		AL_SRDS_REG_TYPE_PMA, 109, 27);
	al_serdes_grp_reg_write(grp_info, AL_SRDS_REG_PAGE_4_COMMON,
		AL_SRDS_REG_TYPE_PMA, 110, 7);
	al_serdes_grp_reg_write(grp_info, AL_SRDS_REG_PAGE_4_COMMON,
		AL_SRDS_REG_TYPE_PMA, 111, 12);
	al_serdes_grp_reg_write(grp_info, AL_SRDS_REG_PAGE_4_COMMON,
		AL_SRDS_REG_TYPE_PMA, 112, 16);
	al_serdes_grp_reg_write(grp_info, AL_SRDS_REG_PAGE_4_COMMON,
		AL_SRDS_REG_TYPE_PMA, 113, 0);
	al_serdes_grp_reg_write(grp_info, AL_SRDS_REG_PAGE_4_COMMON,
		AL_SRDS_REG_TYPE_PMA, 114, 16);
	al_serdes_grp_reg_write(grp_info, AL_SRDS_REG_PAGE_4_COMMON,
		AL_SRDS_REG_TYPE_PMA, 115, 0);
	al_serdes_grp_reg_write(grp_info, AL_SRDS_REG_PAGE_4_COMMON,
		AL_SRDS_REG_TYPE_PMA, 116, 255);
	al_serdes_grp_reg_write(grp_info, AL_SRDS_REG_PAGE_4_COMMON,
		AL_SRDS_REG_TYPE_PMA, 117, 179);
	al_serdes_grp_reg_write(grp_info, AL_SRDS_REG_PAGE_4_COMMON,
		AL_SRDS_REG_TYPE_PMA, 118, 246);
	al_serdes_grp_reg_write(grp_info, AL_SRDS_REG_PAGE_4_COMMON,
		AL_SRDS_REG_TYPE_PMA, 119, 208);
	al_serdes_grp_reg_write(grp_info, AL_SRDS_REG_PAGE_4_COMMON,
		AL_SRDS_REG_TYPE_PMA, 120, 239);
	al_serdes_grp_reg_write(grp_info, AL_SRDS_REG_PAGE_4_COMMON,
		AL_SRDS_REG_TYPE_PMA, 121, 251);
	al_serdes_grp_reg_write(grp_info, AL_SRDS_REG_PAGE_4_COMMON,
		AL_SRDS_REG_TYPE_PMA, 122, 255);
	al_serdes_grp_reg_write(grp_info, AL_SRDS_REG_PAGE_4_COMMON,
		AL_SRDS_REG_TYPE_PMA, 123, 255);
	al_serdes_grp_reg_write(grp_info, AL_SRDS_REG_PAGE_4_COMMON,
		AL_SRDS_REG_TYPE_PMA, 124, 255);
	al_serdes_grp_reg_write(grp_info, AL_SRDS_REG_PAGE_4_COMMON,
		AL_SRDS_REG_TYPE_PMA, 125, 255);
	al_serdes_grp_reg_write(grp_info, AL_SRDS_REG_PAGE_4_COMMON,
		AL_SRDS_REG_TYPE_PMA, 126, 255);
	al_serdes_grp_reg_write(grp_info, AL_SRDS_REG_PAGE_4_COMMON,
		AL_SRDS_REG_TYPE_PMA, 127, 211);
	al_serdes_grp_reg_write(grp_info, AL_SRDS_REG_PAGE_4_COMMON,
		AL_SRDS_REG_TYPE_PMA, 128, 211);
	al_serdes_grp_reg_write(grp_info, AL_SRDS_REG_PAGE_4_COMMON,
		AL_SRDS_REG_TYPE_PMA, 129, 226);
	al_serdes_grp_reg_write(grp_info, AL_SRDS_REG_PAGE_4_COMMON,
		AL_SRDS_REG_TYPE_PMA, 130, 239);
	al_serdes_grp_reg_write(grp_info, AL_SRDS_REG_PAGE_4_COMMON,
		AL_SRDS_REG_TYPE_PMA, 131, 251);
	al_serdes_grp_reg_write(grp_info, AL_SRDS_REG_PAGE_4_COMMON,
		AL_SRDS_REG_TYPE_PMA, 132, 251);
	al_serdes_grp_reg_write(grp_info, AL_SRDS_REG_PAGE_4_COMMON,
		AL_SRDS_REG_TYPE_PMA, 133, 255);
	al_serdes_grp_reg_write(grp_info, AL_SRDS_REG_PAGE_4_COMMON,
		AL_SRDS_REG_TYPE_PMA, 134, 239);
	al_serdes_grp_reg_write(grp_info, AL_SRDS_REG_PAGE_4_COMMON,
		AL_SRDS_REG_TYPE_PMA, 135, 255);
	al_serdes_grp_reg_write(grp_info, AL_SRDS_REG_PAGE_4_COMMON,
		AL_SRDS_REG_TYPE_PMA, 136, 255);
	al_serdes_grp_reg_write(grp_info, AL_SRDS_REG_PAGE_4_COMMON,
		AL_SRDS_REG_TYPE_PMA, 137, 211);
	al_serdes_grp_reg_write(grp_info, AL_SRDS_REG_PAGE_4_COMMON,
		AL_SRDS_REG_TYPE_PMA, 138, 211);
	al_serdes_grp_reg_write(grp_info, AL_SRDS_REG_PAGE_4_COMMON,
		AL_SRDS_REG_TYPE_PMA, 139, 226);
	al_serdes_grp_reg_write(grp_info, AL_SRDS_REG_PAGE_4_COMMON,
		AL_SRDS_REG_TYPE_PMA, 140, 239);
	al_serdes_grp_reg_write(grp_info, AL_SRDS_REG_PAGE_4_COMMON,
		AL_SRDS_REG_TYPE_PMA, 141, 251);
	al_serdes_grp_reg_write(grp_info, AL_SRDS_REG_PAGE_4_COMMON,
		AL_SRDS_REG_TYPE_PMA, 142, 251);
	al_serdes_grp_reg_write(grp_info, AL_SRDS_REG_PAGE_4_COMMON,
		AL_SRDS_REG_TYPE_PMA, 143, 255);
	al_serdes_grp_reg_write(grp_info, AL_SRDS_REG_PAGE_4_COMMON,
		AL_SRDS_REG_TYPE_PMA, 144, 239);
	al_serdes_grp_reg_write(grp_info, AL_SRDS_REG_PAGE_4_COMMON,
		AL_SRDS_REG_TYPE_PMA, 145, 255);
	al_serdes_grp_reg_write(grp_info, AL_SRDS_REG_PAGE_4_COMMON,
		AL_SRDS_REG_TYPE_PMA, 146, 255);
	al_serdes_grp_reg_write(grp_info, AL_SRDS_REG_PAGE_4_COMMON,
		AL_SRDS_REG_TYPE_PMA, 147, 251);
	al_serdes_grp_reg_write(grp_info, AL_SRDS_REG_PAGE_4_COMMON,
		AL_SRDS_REG_TYPE_PMA, 148, 255);
	al_serdes_grp_reg_write(grp_info, AL_SRDS_REG_PAGE_4_COMMON,
		AL_SRDS_REG_TYPE_PMA, 149, 63);
	al_serdes_grp_reg_write(grp_info, AL_SRDS_REG_PAGE_4_COMMON,
		AL_SRDS_REG_TYPE_PMA, 150, 0);
	al_serdes_grp_reg_write(grp_info, AL_SRDS_REG_PAGE_4_COMMON,
		AL_SRDS_REG_TYPE_PMA, 151, 50);
	al_serdes_grp_reg_write(grp_info, AL_SRDS_REG_PAGE_4_COMMON,
		AL_SRDS_REG_TYPE_PMA, 152, 17);
	al_serdes_grp_reg_write(grp_info, AL_SRDS_REG_PAGE_4_COMMON,
		AL_SRDS_REG_TYPE_PMA, 153, 2);
	al_serdes_grp_reg_write(grp_info, AL_SRDS_REG_PAGE_4_COMMON,
		AL_SRDS_REG_TYPE_PMA, 154, 1);
	al_serdes_grp_reg_write(grp_info, AL_SRDS_REG_PAGE_4_COMMON,
		AL_SRDS_REG_TYPE_PMA, 155, 0);
	al_serdes_grp_reg_write(grp_info, AL_SRDS_REG_PAGE_4_COMMON,
		AL_SRDS_REG_TYPE_PMA, 156, 0);
	al_serdes_grp_reg_write(grp_info, AL_SRDS_REG_PAGE_4_COMMON,
		AL_SRDS_REG_TYPE_PMA, 157, 4);
	al_serdes_grp_reg_write(grp_info, AL_SRDS_REG_PAGE_4_COMMON,
		AL_SRDS_REG_TYPE_PMA, 158, 0);
	al_serdes_grp_reg_write(grp_info, AL_SRDS_REG_PAGE_4_COMMON,
		AL_SRDS_REG_TYPE_PMA, 159, 0);
	al_serdes_grp_reg_write(grp_info, AL_SRDS_REG_PAGE_4_COMMON,
		AL_SRDS_REG_TYPE_PMA, 160, 8);
	al_serdes_grp_reg_write(grp_info, AL_SRDS_REG_PAGE_4_COMMON,
		AL_SRDS_REG_TYPE_PMA, 161, 4);
	al_serdes_grp_reg_write(grp_info, AL_SRDS_REG_PAGE_4_COMMON,
		AL_SRDS_REG_TYPE_PMA, 162, 0);
	al_serdes_grp_reg_write(grp_info, AL_SRDS_REG_PAGE_4_COMMON,
		AL_SRDS_REG_TYPE_PMA, 163, 0);
	al_serdes_grp_reg_write(grp_info, AL_SRDS_REG_PAGE_4_COMMON,
		AL_SRDS_REG_TYPE_PMA, 164, 4);
	al_serdes_grp_reg_write(grp_info, AL_SRDS_REG_PAGE_0_LANE_0,
		AL_SRDS_REG_TYPE_PMA, 7, 0);
	al_serdes_grp_reg_write(grp_info, AL_SRDS_REG_PAGE_1_LANE_1,
		AL_SRDS_REG_TYPE_PMA, 7, 0);
	al_serdes_grp_reg_write(grp_info, AL_SRDS_REG_PAGE_2_LANE_2,
		AL_SRDS_REG_TYPE_PMA, 7, 0);
	al_serdes_grp_reg_write(grp_info, AL_SRDS_REG_PAGE_3_LANE_3,
		AL_SRDS_REG_TYPE_PMA, 7, 0);
	al_serdes_grp_reg_write(grp_info, AL_SRDS_REG_PAGE_4_COMMON,
		AL_SRDS_REG_TYPE_PMA, 13, 16);
	al_serdes_grp_reg_write(grp_info, AL_SRDS_REG_PAGE_4_COMMON,
		AL_SRDS_REG_TYPE_PMA, 48, 0);
	al_serdes_grp_reg_write(grp_info, AL_SRDS_REG_PAGE_4_COMMON,
		AL_SRDS_REG_TYPE_PMA, 49, 0);
	al_serdes_grp_reg_write(grp_info, AL_SRDS_REG_PAGE_4_COMMON,
		AL_SRDS_REG_TYPE_PMA, 54, 0);
	al_serdes_grp_reg_write(grp_info, AL_SRDS_REG_PAGE_4_COMMON,
		AL_SRDS_REG_TYPE_PMA, 55, 149); /*Was 182*/
	al_serdes_grp_reg_write(grp_info, AL_SRDS_REG_PAGE_4_COMMON,
		AL_SRDS_REG_TYPE_PMA, 93, 2);
	al_serdes_grp_reg_write(grp_info, AL_SRDS_REG_PAGE_4_COMMON,
		AL_SRDS_REG_TYPE_PMA, 165, 3);
	al_serdes_grp_reg_write(grp_info, AL_SRDS_REG_PAGE_0123_LANES_0123,
		AL_SRDS_REG_TYPE_PMA, 41, 6);
	al_serdes_grp_reg_write(grp_info, AL_SRDS_REG_PAGE_4_COMMON,
		AL_SRDS_REG_TYPE_PMA, 354, 3);
	al_serdes_grp_reg_write(grp_info, AL_SRDS_REG_PAGE_4_COMMON,
		AL_SRDS_REG_TYPE_PMA, 355, 58);
	al_serdes_grp_reg_write(grp_info, AL_SRDS_REG_PAGE_4_COMMON,
		AL_SRDS_REG_TYPE_PMA, 356, 9);
	al_serdes_grp_reg_write(grp_info, AL_SRDS_REG_PAGE_4_COMMON,
		AL_SRDS_REG_TYPE_PMA, 357, 3);
	al_serdes_grp_reg_write(grp_info, AL_SRDS_REG_PAGE_4_COMMON,
		AL_SRDS_REG_TYPE_PMA, 358, 62);
	al_serdes_grp_reg_write(grp_info, AL_SRDS_REG_PAGE_4_COMMON,
		AL_SRDS_REG_TYPE_PMA, 359, 12);
	al_serdes_grp_reg_write(grp_info, AL_SRDS_REG_PAGE_0123_LANES_0123,
		AL_SRDS_REG_TYPE_PMA, 701, 0);
	al_serdes_grp_reg_write(grp_info, AL_SRDS_REG_PAGE_0123_LANES_0123,
		AL_SRDS_REG_TYPE_PMA, 87, 0x1f);

	al_serdes_common_cfg_eth(grp_info);

	al_serdes_mode_rx_tx_inv_state_restore(grp_info, &rx_tx_inv_state);

	al_reg_write32(&grp_info->regs_base->gen.irst, 0x0011F0);
	al_serdes_ns_delay(500);
}

void al_serdes_rx_advanced_params_get(struct al_serdes_obj           *obj,
				      enum al_serdes_group	      grp,
				      enum al_serdes_lane	      lane,
				      struct al_serdes_adv_rx_params* rx_params)
{
	uint8_t temp_val;

	al_serdes_reg_read(
			obj, grp, (enum al_serdes_reg_page)lane, AL_SRDS_REG_TYPE_PMA,
			SERDES_IREG_RX_CALEQ_1_REG_NUM,
			&temp_val);
	rx_params->dcgain = (temp_val & SERDES_IREG_RX_CALEQ_1_DCGAIN_MASK) >>
				SERDES_IREG_RX_CALEQ_1_DCGAIN_SHIFT;
	rx_params->dfe_3db_freq = (temp_val &
				SERDES_IREG_RX_CALEQ_1_DFEPSTAP3DB_MASK) >>
				SERDES_IREG_RX_CALEQ_1_DFEPSTAP3DB_SHIFT;

	al_serdes_reg_read(
			obj, grp, (enum al_serdes_reg_page)lane, AL_SRDS_REG_TYPE_PMA,
			SERDES_IREG_RX_CALEQ_2_REG_NUM,
			&temp_val);
	rx_params->dfe_gain = (temp_val &
				SERDES_IREG_RX_CALEQ_2_DFEPSTAPGAIN_MASK) >>
				SERDES_IREG_RX_CALEQ_2_DFEPSTAPGAIN_SHIFT;
	rx_params->dfe_first_tap_ctrl = (temp_val &
			SERDES_IREG_RX_CALEQ_2_DFETAP1GAIN_MASK) >>
			SERDES_IREG_RX_CALEQ_2_DFETAP1GAIN_SHIFT;

	al_serdes_reg_read(
			obj, grp, (enum al_serdes_reg_page)lane, AL_SRDS_REG_TYPE_PMA,
			SERDES_IREG_RX_CALEQ_3_REG_NUM,
			&temp_val);
	rx_params->dfe_secound_tap_ctrl = (temp_val &
			SERDES_IREG_RX_CALEQ_3_DFETAP2GAIN_MASK) >>
			SERDES_IREG_RX_CALEQ_3_DFETAP2GAIN_SHIFT;
	rx_params->dfe_third_tap_ctrl = (temp_val &
			SERDES_IREG_RX_CALEQ_3_DFETAP3GAIN_MASK) >>
			SERDES_IREG_RX_CALEQ_3_DFETAP3GAIN_SHIFT;

	al_serdes_reg_read(
			obj, grp, (enum al_serdes_reg_page)lane, AL_SRDS_REG_TYPE_PMA,
			SERDES_IREG_RX_CALEQ_4_REG_NUM,
			&temp_val);
	rx_params->dfe_fourth_tap_ctrl = (temp_val &
			SERDES_IREG_RX_CALEQ_4_DFETAP4GAIN_MASK) >>
			SERDES_IREG_RX_CALEQ_4_DFETAP4GAIN_SHIFT;
	rx_params->low_freq_agc_gain = (temp_val &
			SERDES_IREG_RX_CALEQ_4_LOFREQAGCGAIN_MASK) >>
			SERDES_IREG_RX_CALEQ_4_LOFREQAGCGAIN_SHIFT;

	al_serdes_reg_read(
			obj, grp, (enum al_serdes_reg_page)lane, AL_SRDS_REG_TYPE_PMA,
			SERDES_IREG_RX_CALEQ_5_REG_NUM,
			&temp_val);
	rx_params->precal_code_sel = (temp_val &
			SERDES_IREG_RX_CALEQ_5_PRECAL_CODE_SEL_MASK) >>
			SERDES_IREG_RX_CALEQ_5_PRECAL_CODE_SEL_SHIFT;
	rx_params->high_freq_agc_boost = (temp_val &
			SERDES_IREG_RX_CALEQ_5_HIFREQAGCCAP_MASK) >>
			SERDES_IREG_RX_CALEQ_5_HIFREQAGCCAP_SHIFT;

	al_serdes_reg_read(obj, grp, (enum al_serdes_reg_page)lane, AL_SRDS_REG_TYPE_PMA,
			SERDES_IREG_FLD_RX_DRV_OVERRIDE_EN_REG_NUM,
			&temp_val);
	rx_params->override = ((temp_val & SERDES_IREG_FLD_RX_DRV_OVERRIDE_EN) == 0);
}

#if (	SERDES_IREG_FLD_RXCALEYEDIAGFSMIN_LOCWREN_REG_NUM != \
		SERDES_IREG_FLD_RXCALROAMEYEMEASIN_LOCWREN_REG_NUM || \
	SERDES_IREG_FLD_RXCALEYEDIAGFSMIN_LOCWREN_REG_NUM != \
		SERDES_IREG_FLD_RXCALROAMXADJUST_LOCWREN_REG_NUM)
#error Wrong assumption
#endif
int al_serdes_rx_equalization(
		struct al_serdes_obj *obj,
		enum al_serdes_group grp,
		enum al_serdes_lane  lane)
{
	uint8_t serdes_ireg_fld_rxcalroamyadjust_locwren_val;
	uint8_t serdes_ireg_fld_rxroam_xorbitsel_val;
	uint8_t serdes_ireg_fld_pcsrxeq_locwren_val;
	uint8_t serdes_ireg_fld_rxcal_locwren_val;
	uint8_t temp_val;
	uint8_t done;

	int test_score;
	int i;

	/*
	 * Make sure Roam Eye mechanism is not overridden
	 * Lane SERDES_IREG_FLD_RXCALEYEDIAGFSMIN_LOCWREN = 1,
	 * 	so Rx 4-Point Eye process is not overridden
	 * Lane SERDES_IREG_FLD_RXCALROAMEYEMEASIN_LOCWREN = 1,
	 * 	so Eye Roam latch is not overridden
	 * Lane SERDES_IREG_FLD_RXCALROAMXADJUST_LOCWREN = 1,
	 * 	so Eye Roam latch 'X adjust' is not overridden
	 * Lane SERDES_IREG_FLD_RXCALROAMYADJUST_LOCWREN = 1,
	 * 	so Eye Roam latch 'Y adjust' is not overridden
	 * Lane SERDES_IREG_FLD_RXROAM_XORBITSEL = 0/1,
	 * 	so Eye Roamlatch works on the right Eye position (XORBITSEL)
	 * 	For most cases 0 is needed, but sometimes 1 is needed.
	 * 	I couldn't sort out why is this so the code uses a global
	 *      XORBITSELmode variable, set by the user (GUI). Default is 0.
	 * control must be internal. At the end we restore original setting
	 */

	/* save current values for restoring them later in the end */
	al_serdes_reg_read(
			obj, grp, (enum al_serdes_reg_page)lane, AL_SRDS_REG_TYPE_PMA,
			SERDES_IREG_FLD_RXCAL_LOCWREN_REG_NUM,
			&serdes_ireg_fld_rxcal_locwren_val);

	al_serdes_reg_read(
			obj, grp, (enum al_serdes_reg_page)lane, AL_SRDS_REG_TYPE_PMA,
			SERDES_IREG_FLD_RXCALROAMYADJUST_LOCWREN_REG_NUM,
			&serdes_ireg_fld_rxcalroamyadjust_locwren_val );
	al_serdes_reg_read(
			obj, grp, (enum al_serdes_reg_page)lane, AL_SRDS_REG_TYPE_PMA,
			SERDES_IREG_FLD_RXROAM_XORBITSEL_REG_NUM,
			&serdes_ireg_fld_rxroam_xorbitsel_val );
	al_serdes_reg_read(
			obj, grp, (enum al_serdes_reg_page)lane, AL_SRDS_REG_TYPE_PMA,
			SERDES_IREG_FLD_PCSRXEQ_LOCWREN_REG_NUM,
			&serdes_ireg_fld_pcsrxeq_locwren_val );

	/*
	 * Set Bits:
	 * SERDES_IREG_FLD_RXCALEYEDIAGFSMIN_LOCWREN
	 * SERDES_IREG_FLD_RXCALROAMEYEMEASIN_LOCWREN
	 * SERDES_IREG_FLD_RXCALROAMXADJUST_LOCWREN
	 * to return 4pt-RxEye and EyeRoam Latch to internal logic
	 *
	 * clear bit SERDES_IREG_FLD_RX_DRV_OVERRIDE_EN
	 * AGC/DFE controlled via PMA registers
	 */
	temp_val  = serdes_ireg_fld_rxcal_locwren_val;
	temp_val |= SERDES_IREG_FLD_RXCALEYEDIAGFSMIN_LOCWREN;
	temp_val |= SERDES_IREG_FLD_RXCALROAMEYEMEASIN_LOCWREN;
	temp_val |= SERDES_IREG_FLD_RXCALROAMXADJUST_LOCWREN;
	temp_val |= SERDES_IREG_FLD_RX_DRV_OVERRIDE_EN;

	al_serdes_reg_write(
			obj, grp, (enum al_serdes_reg_page)lane, AL_SRDS_REG_TYPE_PMA,
			SERDES_IREG_FLD_RXCAL_LOCWREN_REG_NUM,
			temp_val );

	/*
	 * Set bit SERDES_IREG_FLD_RXCALROAMYADJUST_LOCWREN
	 * to return EyeRoam Latch Y to internal logic
	 */
	temp_val = serdes_ireg_fld_rxcalroamyadjust_locwren_val |
			SERDES_IREG_FLD_RXCALROAMYADJUST_LOCWREN;
	al_serdes_reg_write(
			obj, grp, (enum al_serdes_reg_page)lane, AL_SRDS_REG_TYPE_PMA,
			SERDES_IREG_FLD_RXCALROAMYADJUST_LOCWREN_REG_NUM,
			temp_val );

	/*
	 * Clear Bit: SERDES_IREG_FLD_RXROAM_XORBITSEL
	 * so XORBITSEL=0, needed for the Eye mapping.
	 */
	temp_val = serdes_ireg_fld_rxroam_xorbitsel_val &
			~SERDES_IREG_FLD_RXROAM_XORBITSEL;
	al_serdes_reg_write(
			obj, grp, (enum al_serdes_reg_page)lane, AL_SRDS_REG_TYPE_PMA,
			SERDES_IREG_FLD_RXROAM_XORBITSEL_REG_NUM,
			temp_val );

	/*
	 * Take Control from int.pin over RxEQ process.
	 * Clear Bit SERDES_IREG_FLD_PCSRXEQ_LOCWREN
	 * to override RxEQ via PMA
	 */
	temp_val = serdes_ireg_fld_pcsrxeq_locwren_val &
			~SERDES_IREG_FLD_PCSRXEQ_LOCWREN;
	al_serdes_reg_write(
			obj, grp, (enum al_serdes_reg_page)lane, AL_SRDS_REG_TYPE_PMA,
			SERDES_IREG_FLD_PCSRXEQ_LOCWREN_REG_NUM,
			temp_val );


	/*
	 * Start/Stop RxEQ Cal is via PCSRXEQ_START: 1=START. 0=STOP.
	 * Clear Bit SERDES_IREG_FLD_PCSRXEQ_START
	 * to start fresh from Stop
	 */
	al_serdes_reg_read(
			obj, grp, (enum al_serdes_reg_page)lane, AL_SRDS_REG_TYPE_PMA,
			SERDES_IREG_FLD_PCSRXEQ_START_REG_NUM,
			&temp_val );
	temp_val &= ~SERDES_IREG_FLD_PCSRXEQ_START;
	al_serdes_reg_write(
			obj, grp, (enum al_serdes_reg_page)lane, AL_SRDS_REG_TYPE_PMA,
			SERDES_IREG_FLD_PCSRXEQ_START_REG_NUM,
			temp_val );

	/* Set Bit SERDES_IREG_FLD_PCSRXEQ_START
	 * to begin Rx Eq Cal */
	temp_val |= SERDES_IREG_FLD_PCSRXEQ_START;
	al_serdes_reg_write(
			obj, grp, (enum al_serdes_reg_page)lane, AL_SRDS_REG_TYPE_PMA,
			SERDES_IREG_FLD_PCSRXEQ_START_REG_NUM,
			temp_val );

	/* Poll on RxEq Cal completion. SERDES_IREG_FLD_RXEQ_DONE. 1=Done. */
	for( i = 0; i < AL_SERDES_RX_EQUAL_TRIES; ++i ) {
		al_serdes_reg_read(
				obj, grp, (enum al_serdes_reg_page)lane, AL_SRDS_REG_TYPE_PMA,
				SERDES_IREG_FLD_RXCALROAMEYEMEASDONE_REG_NUM,
				&done );
		done &= SERDES_IREG_FLD_RXEQ_DONE;

		/* Check if RxEQ Cal is done */
		if (done)
			break;
		al_msleep(AL_SERDES_RX_EQUAL_MDELAY);
	}

	if (!done) {
		al_err("%s: Timeout!\n", __func__);
		return -1;
	}

	/* Stop the RxEQ process. */
	temp_val &= ~SERDES_IREG_FLD_PCSRXEQ_START;
	al_serdes_reg_write(
			obj, grp, (enum al_serdes_reg_page)lane, AL_SRDS_REG_TYPE_PMA,
			SERDES_IREG_FLD_PCSRXEQ_START_REG_NUM,
			temp_val );
	/* Get score */
	al_serdes_reg_read(
			obj, grp, (enum al_serdes_reg_page)lane, AL_SRDS_REG_TYPE_PMA,
			SERDES_IREG_RXEQ_BEST_EYE_MSB_VAL_REG_NUM,
			&temp_val );
	test_score = (int)( (temp_val & 0xFF) << 6 );
	al_serdes_reg_read(
			obj, grp, (enum al_serdes_reg_page)lane, AL_SRDS_REG_TYPE_PMA,
			SERDES_IREG_RXEQ_BEST_EYE_LSB_VAL_REG_NUM,
			&temp_val );
	test_score += (int)(temp_val & SERDES_IREG_RXEQ_BEST_EYE_LSB_VAL_MASK);

	/* Restore start values */
	al_serdes_reg_write(
			obj, grp, (enum al_serdes_reg_page)lane, AL_SRDS_REG_TYPE_PMA,
			SERDES_IREG_FLD_RXCAL_LOCWREN_REG_NUM,
			serdes_ireg_fld_rxcal_locwren_val);
	al_serdes_reg_write(
			obj, grp, (enum al_serdes_reg_page)lane, AL_SRDS_REG_TYPE_PMA,
			SERDES_IREG_FLD_RXCALROAMYADJUST_LOCWREN_REG_NUM,
			serdes_ireg_fld_rxcalroamyadjust_locwren_val );
	al_serdes_reg_write(
			obj, grp, (enum al_serdes_reg_page)lane, AL_SRDS_REG_TYPE_PMA,
			SERDES_IREG_FLD_RXROAM_XORBITSEL_REG_NUM,
			serdes_ireg_fld_rxroam_xorbitsel_val );
	al_serdes_reg_write(
			obj, grp, (enum al_serdes_reg_page)lane, AL_SRDS_REG_TYPE_PMA,
			SERDES_IREG_FLD_PCSRXEQ_LOCWREN_REG_NUM,
			serdes_ireg_fld_pcsrxeq_locwren_val );

	return test_score;
}

#if (	SERDES_IREG_FLD_RXCAL_LOCWREN_REG_NUM != \
		SERDES_IREG_FLD_RXCALROAMEYEMEASIN_LOCWREN_REG_NUM || \
	SERDES_IREG_FLD_RXCAL_LOCWREN_REG_NUM != \
		SERDES_IREG_FLD_RXCALROAMXADJUST_LOCWREN_REG_NUM || \
	SERDES_IREG_FLD_RXCAL_LOCWREN_REG_NUM != \
		SERDES_IREG_FLD_RXCALEYEDIAGFSMIN_LOCWREN_REG_NUM)
#error Wrong assumption
#endif
int al_serdes_calc_eye_size(
		struct al_serdes_obj *obj,
		enum al_serdes_group grp,
		enum al_serdes_lane  lane,
		int*                 width,
		int*                 height)
{
	uint8_t rxcaleyediagfsm_x_y_valweight_val;
	uint8_t rxcaleyediagfsm_xvalcoarse_val;
	uint8_t rxcaleyediagfsm_xvalfine_val;
	uint8_t rxcaleyediagfsm_yvalcoarse_val;
	uint8_t rxcaleyediagfsm_yvalfine_val;
	uint8_t rxlock2ref_locwren_val;
	uint8_t rxcal_locwren_val;
	uint8_t rxcalroamyadjust_locwren_val;
	uint8_t rxlock2ref_ovren_val;

	int i;
	uint8_t status;
	uint8_t reg_value;

	/* Save Registers */
	al_serdes_reg_read(obj, grp, (enum al_serdes_reg_page)lane, AL_SRDS_REG_TYPE_PMA,
			SERDES_IREG_FLD_RXLOCK2REF_LOCWREN_REG_NUM,
			&rxlock2ref_locwren_val);
	al_serdes_reg_read(obj, grp, (enum al_serdes_reg_page)lane, AL_SRDS_REG_TYPE_PMA,
			SERDES_IREG_FLD_RXCAL_LOCWREN_REG_NUM,
			&rxcal_locwren_val);
	al_serdes_reg_read(obj, grp, (enum al_serdes_reg_page)lane, AL_SRDS_REG_TYPE_PMA,
			SERDES_IREG_FLD_RXCALROAMYADJUST_LOCWREN_REG_NUM,
			&rxcalroamyadjust_locwren_val);
	al_serdes_reg_read(obj, grp, (enum al_serdes_reg_page)lane, AL_SRDS_REG_TYPE_PMA,
			SERDES_IREG_FLD_RXLOCK2REF_OVREN_REG_NUM,
			&rxlock2ref_ovren_val);

	al_serdes_reg_read(obj, grp, AL_SRDS_REG_PAGE_4_COMMON,
			AL_SRDS_REG_TYPE_PMA,
			SERDES_IREG_FLD_RXCALEYEDIAGFSM_X_Y_VALWEIGHT_REG_NUM,
			&rxcaleyediagfsm_x_y_valweight_val);
	al_serdes_reg_read(obj, grp, AL_SRDS_REG_PAGE_4_COMMON,
			AL_SRDS_REG_TYPE_PMA,
			SERDES_IREG_FLD_RXCALEYEDIAGFSM_XVALCOARSE_REG_NUM,
			&rxcaleyediagfsm_xvalcoarse_val);
	al_serdes_reg_read(obj, grp, AL_SRDS_REG_PAGE_4_COMMON,
			AL_SRDS_REG_TYPE_PMA,
			SERDES_IREG_FLD_RXCALEYEDIAGFSM_XVALFINE_REG_NUM,
			&rxcaleyediagfsm_xvalfine_val);
	al_serdes_reg_read(obj, grp, AL_SRDS_REG_PAGE_4_COMMON,
			AL_SRDS_REG_TYPE_PMA,
			SERDES_IREG_FLD_RXCALEYEDIAGFSM_YVALCOARSE_REG_NUM,
			&rxcaleyediagfsm_yvalcoarse_val);
	al_serdes_reg_read(obj, grp, AL_SRDS_REG_PAGE_4_COMMON,
			AL_SRDS_REG_TYPE_PMA,
			SERDES_IREG_FLD_RXCALEYEDIAGFSM_YVALFINE_REG_NUM,
			&rxcaleyediagfsm_yvalfine_val);

	/*
	 * Clear Bit:
	 * 	SERDES_IREG_FLD_RXCALEYEDIAGFSMIN_LOCWREN
	 * 	to override RxEQ via PMA
	 * Set Bits:
	 * 	SERDES_IREG_FLD_RXCALROAMEYEMEASIN_LOCWREN,
	 * 	SERDES_IREG_FLD_RXCALROAMXADJUST_LOCWREN
	 * 	to keep Eye Diag Roam controlled internally
	 */
	al_serdes_grp_reg_masked_write(&obj->grp_info[grp],
			(enum al_serdes_reg_page)lane,
			AL_SRDS_REG_TYPE_PMA,
			SERDES_IREG_FLD_RXCAL_LOCWREN_REG_NUM,
			SERDES_IREG_FLD_RXCALEYEDIAGFSMIN_LOCWREN  |
			SERDES_IREG_FLD_RXCALROAMEYEMEASIN_LOCWREN |
			SERDES_IREG_FLD_RXCALROAMXADJUST_LOCWREN,
			SERDES_IREG_FLD_RXCALROAMEYEMEASIN_LOCWREN |
			SERDES_IREG_FLD_RXCALROAMXADJUST_LOCWREN);
	/*
	 * Set Bit:
	 * 	 SERDES_IREG_FLD_RXCALROAMYADJUST_LOCWREN
	 * 	 to keep Eye Diag Roam controlled internally
	 */
	al_serdes_grp_reg_masked_write(&obj->grp_info[grp],
			(enum al_serdes_reg_page)lane,
			AL_SRDS_REG_TYPE_PMA,
			SERDES_IREG_FLD_RXCALROAMYADJUST_LOCWREN_REG_NUM,
			SERDES_IREG_FLD_RXCALROAMYADJUST_LOCWREN,
			SERDES_IREG_FLD_RXCALROAMYADJUST_LOCWREN);

	/*
	 * Clear Bit:
	 * 	SERDES_IREG_FLD_RXROAM_XORBITSEL,
	 * 	so XORBITSEL=0, needed for the Eye mapping
	 *  Set Bit:
	 *  SERDES_IREG_FLD_RXLOCK2REF_OVREN,
	 *  so RXLOCK2REF_OVREN=1, keeping lock to data, preventing data hit
	 */
	al_serdes_grp_reg_masked_write(&obj->grp_info[grp],
			(enum al_serdes_reg_page)lane,
			AL_SRDS_REG_TYPE_PMA,
			SERDES_IREG_FLD_RXROAM_XORBITSEL_REG_NUM,
			SERDES_IREG_FLD_RXLOCK2REF_OVREN |
			SERDES_IREG_FLD_RXROAM_XORBITSEL,
			SERDES_IREG_FLD_RXLOCK2REF_OVREN);


	/*
	 * Clear Bit:
	 * 	SERDES_IREG_FLD_RXLOCK2REF_LOCWREN,
	 * 	so RXLOCK2REF_LOCWREN=0, to override control
	 */
	al_serdes_grp_reg_masked_write(&obj->grp_info[grp],
				(enum al_serdes_reg_page)lane,
				AL_SRDS_REG_TYPE_PMA,
				SERDES_IREG_FLD_RXLOCK2REF_LOCWREN_REG_NUM,
				SERDES_IREG_FLD_RXLOCK2REF_LOCWREN,
				0);

	/* Width Calculation */

	/* Return Value = 0*Y + 1*X */
	al_serdes_reg_write(obj, grp, AL_SRDS_REG_PAGE_4_COMMON,
			AL_SRDS_REG_TYPE_PMA,
			SERDES_IREG_FLD_RXCALEYEDIAGFSM_X_Y_VALWEIGHT_REG_NUM,
			0x01);
	/* X coarse scan step = 3 */
	al_serdes_reg_write(obj, grp, AL_SRDS_REG_PAGE_4_COMMON,
			AL_SRDS_REG_TYPE_PMA,
			SERDES_IREG_FLD_RXCALEYEDIAGFSM_XVALCOARSE_REG_NUM,
			0x03);
	/* X fine scan step = 1   */
	al_serdes_reg_write(obj, grp, AL_SRDS_REG_PAGE_4_COMMON,
			AL_SRDS_REG_TYPE_PMA,
			SERDES_IREG_FLD_RXCALEYEDIAGFSM_XVALFINE_REG_NUM,
			0x01);
	/* Y coarse scan step = 0 */
	al_serdes_reg_write(obj, grp, AL_SRDS_REG_PAGE_4_COMMON,
			AL_SRDS_REG_TYPE_PMA,
			SERDES_IREG_FLD_RXCALEYEDIAGFSM_YVALCOARSE_REG_NUM,
			0x00);
	/* Y fine scan step = 0   */
	al_serdes_reg_write(obj, grp, AL_SRDS_REG_PAGE_4_COMMON,
			AL_SRDS_REG_TYPE_PMA,
			SERDES_IREG_FLD_RXCALEYEDIAGFSM_YVALFINE_REG_NUM,
			0x00);

	/*
	 * Set Bit:
	 * 	SERDES_IREG_FLD_RXCALEYEDIAGFSMIN_START,
	 * 	to start Eye measurement
	 */
	al_serdes_grp_reg_masked_write(&obj->grp_info[grp],
				(enum al_serdes_reg_page)lane,
				AL_SRDS_REG_TYPE_PMA,
				SERDES_IREG_FLD_RXCALEYEDIAGFSMIN_START_REG_NUM,
				SERDES_IREG_FLD_RXCALEYEDIAGFSMIN_START,
				SERDES_IREG_FLD_RXCALEYEDIAGFSMIN_START);

	for( i = 0; i < AL_SERDES_RX_EYE_CAL_TRIES; ++i ) {
		/* Check if RxEQ Cal is done */
		al_serdes_reg_read(
				obj, grp, (enum al_serdes_reg_page)lane, AL_SRDS_REG_TYPE_PMA,
				SERDES_IREG_FLD_RXCALEYEDIAGFSM_DONE_REG_NUM,
				&status );
		if (status & SERDES_IREG_FLD_RXCALEYEDIAGFSM_DONE)
			break;
		al_msleep(AL_SERDES_RX_EYE_CAL_MDELAY);
	}

	if (status & SERDES_IREG_FLD_RXCALEYEDIAGFSM_ERR) {
		al_err("%s: eye measure error!\n", __func__);
		return -1;
	}

	if (!(status & SERDES_IREG_FLD_RXCALEYEDIAGFSM_DONE)) {
		al_err("%s: eye measure timeout!\n", __func__);
		return -1;
	}

	/*  Read Eye Opening Metrics, Bits:
	 * 	SERDES_IREG_FLD_RXCALEYEDIAGFSM_EYESUM_LSB,
	 * 	SERDES_IREG_FLD_RXCALEYEDIAGFSM_EYESUM_LSB
	 */
	al_serdes_reg_read(
			obj, grp, (enum al_serdes_reg_page)lane, AL_SRDS_REG_TYPE_PMA,
			SERDES_IREG_FLD_RXCALEYEDIAGFSM_EYESUM_MSB_REG_NUM,
			&reg_value );
	*width = reg_value << 6;
	al_serdes_reg_read(
			obj, grp, (enum al_serdes_reg_page)lane, AL_SRDS_REG_TYPE_PMA,
			SERDES_IREG_FLD_RXCALEYEDIAGFSM_EYESUM_LSB_REG_NUM,
			&reg_value );
	*width =+ reg_value & SERDES_IREG_FLD_RXCALEYEDIAGFSM_EYESUM_LSB_MAKE;

	/*
	 * Clear Bit:
	 * 	SERDES_IREG_FLD_RXCALEYEDIAGFSMIN_START,
	 * 	to stop Eye measurement
	 */
	al_serdes_grp_reg_masked_write(&obj->grp_info[grp],
				(enum al_serdes_reg_page)lane,
				AL_SRDS_REG_TYPE_PMA,
				SERDES_IREG_FLD_RXCALEYEDIAGFSMIN_START_REG_NUM,
				SERDES_IREG_FLD_RXCALEYEDIAGFSMIN_START,
				0);

	/* Height Calculation */

	/* Return Value = 1*Y + 0*X */
	al_serdes_reg_write(obj, grp, AL_SRDS_REG_PAGE_4_COMMON,
			AL_SRDS_REG_TYPE_PMA,
			SERDES_IREG_FLD_RXCALEYEDIAGFSM_X_Y_VALWEIGHT_REG_NUM,
			0x10);
	/* X coarse scan step = 0 */
	al_serdes_reg_write(obj, grp, AL_SRDS_REG_PAGE_4_COMMON,
			AL_SRDS_REG_TYPE_PMA,
			SERDES_IREG_FLD_RXCALEYEDIAGFSM_XVALCOARSE_REG_NUM,
			0x00);
	/* X fine scan step = 0   */
	al_serdes_reg_write(obj, grp, AL_SRDS_REG_PAGE_4_COMMON,
			AL_SRDS_REG_TYPE_PMA,
			SERDES_IREG_FLD_RXCALEYEDIAGFSM_XVALFINE_REG_NUM,
			0x00);
	/* Y coarse scan step = 3 */
	al_serdes_reg_write(obj, grp, AL_SRDS_REG_PAGE_4_COMMON,
			AL_SRDS_REG_TYPE_PMA,
			SERDES_IREG_FLD_RXCALEYEDIAGFSM_YVALCOARSE_REG_NUM,
			0x03);
	/* Y fine scan step = 1   */
	al_serdes_reg_write(obj, grp, AL_SRDS_REG_PAGE_4_COMMON,
			AL_SRDS_REG_TYPE_PMA,
			SERDES_IREG_FLD_RXCALEYEDIAGFSM_YVALFINE_REG_NUM,
			0x01);

	/*
	 * Set Bit:
	 * 	SERDES_IREG_FLD_RXCALEYEDIAGFSMIN_START,
	 * 	to start Eye measurement
	 */
	al_serdes_grp_reg_masked_write(&obj->grp_info[grp],
				(enum al_serdes_reg_page)lane,
				AL_SRDS_REG_TYPE_PMA,
				SERDES_IREG_FLD_RXCALEYEDIAGFSMIN_START_REG_NUM,
				SERDES_IREG_FLD_RXCALEYEDIAGFSMIN_START,
				SERDES_IREG_FLD_RXCALEYEDIAGFSMIN_START);

	for( i = 0; i < AL_SERDES_RX_EYE_CAL_TRIES; ++i ) {
		/* Check if RxEQ Cal is done */
		al_serdes_reg_read(
				obj, grp, (enum al_serdes_reg_page)lane, AL_SRDS_REG_TYPE_PMA,
				SERDES_IREG_FLD_RXCALEYEDIAGFSM_DONE_REG_NUM,
				&status );
		if (status & SERDES_IREG_FLD_RXCALEYEDIAGFSM_DONE)
			break;
		al_msleep(AL_SERDES_RX_EYE_CAL_MDELAY);
	}

	if (status & SERDES_IREG_FLD_RXCALEYEDIAGFSM_ERR) {
		al_err("%s: eye measure error!\n", __func__);
		return -1;
	}

	if (!(status & SERDES_IREG_FLD_RXCALEYEDIAGFSM_DONE)) {
		al_err("%s: eye measure timeout!\n", __func__);
		return -1;
	}

	/*  Read Eye Opening Metrics, Bits:
	 * 	SERDES_IREG_FLD_RXCALEYEDIAGFSM_EYESUM_LSB,
	 * 	SERDES_IREG_FLD_RXCALEYEDIAGFSM_EYESUM_LSB
	 */
	al_serdes_reg_read(
			obj, grp, (enum al_serdes_reg_page)lane, AL_SRDS_REG_TYPE_PMA,
			SERDES_IREG_FLD_RXCALEYEDIAGFSM_EYESUM_MSB_REG_NUM,
			&reg_value );
	*height = reg_value << 6;
	al_serdes_reg_read(
			obj, grp, (enum al_serdes_reg_page)lane, AL_SRDS_REG_TYPE_PMA,
			SERDES_IREG_FLD_RXCALEYEDIAGFSM_EYESUM_LSB_REG_NUM,
			&reg_value );
	*height =+ reg_value & SERDES_IREG_FLD_RXCALEYEDIAGFSM_EYESUM_LSB_MAKE;

	/*
	 * Clear Bit:
	 * 	SERDES_IREG_FLD_RXCALEYEDIAGFSMIN_START,
	 * 	to stop Eye measurement
	 */
	al_serdes_grp_reg_masked_write(&obj->grp_info[grp],
				(enum al_serdes_reg_page)lane,
				AL_SRDS_REG_TYPE_PMA,
				SERDES_IREG_FLD_RXCALEYEDIAGFSMIN_START_REG_NUM,
				SERDES_IREG_FLD_RXCALEYEDIAGFSMIN_START,
				0);

	/* Restore Registers */
	al_serdes_reg_write(obj, grp, AL_SRDS_REG_PAGE_4_COMMON,
			AL_SRDS_REG_TYPE_PMA,
			SERDES_IREG_FLD_RXCALEYEDIAGFSM_X_Y_VALWEIGHT_REG_NUM,
			rxcaleyediagfsm_x_y_valweight_val);
	al_serdes_reg_write(obj, grp, AL_SRDS_REG_PAGE_4_COMMON,
			AL_SRDS_REG_TYPE_PMA,
			SERDES_IREG_FLD_RXCALEYEDIAGFSM_XVALCOARSE_REG_NUM,
			rxcaleyediagfsm_xvalcoarse_val);
	al_serdes_reg_write(obj, grp, AL_SRDS_REG_PAGE_4_COMMON,
			AL_SRDS_REG_TYPE_PMA,
			SERDES_IREG_FLD_RXCALEYEDIAGFSM_XVALFINE_REG_NUM,
			rxcaleyediagfsm_xvalfine_val);
	al_serdes_reg_write(obj, grp, AL_SRDS_REG_PAGE_4_COMMON,
			AL_SRDS_REG_TYPE_PMA,
			SERDES_IREG_FLD_RXCALEYEDIAGFSM_YVALCOARSE_REG_NUM,
			rxcaleyediagfsm_yvalcoarse_val);
	al_serdes_reg_write(obj, grp, AL_SRDS_REG_PAGE_4_COMMON,
			AL_SRDS_REG_TYPE_PMA,
			SERDES_IREG_FLD_RXCALEYEDIAGFSM_YVALFINE_REG_NUM,
			rxcaleyediagfsm_yvalfine_val);

	al_serdes_reg_write(obj, grp, (enum al_serdes_reg_page)lane, AL_SRDS_REG_TYPE_PMA,
			SERDES_IREG_FLD_RXLOCK2REF_LOCWREN_REG_NUM,
			rxlock2ref_locwren_val);
	al_serdes_reg_write(obj, grp, (enum al_serdes_reg_page)lane, AL_SRDS_REG_TYPE_PMA,
			SERDES_IREG_FLD_RXCAL_LOCWREN_REG_NUM,
			rxcal_locwren_val);
	al_serdes_reg_write(obj, grp, (enum al_serdes_reg_page)lane, AL_SRDS_REG_TYPE_PMA,
			SERDES_IREG_FLD_RXCALROAMYADJUST_LOCWREN_REG_NUM,
			rxcalroamyadjust_locwren_val);
	al_serdes_reg_write(obj, grp, (enum al_serdes_reg_page)lane, AL_SRDS_REG_TYPE_PMA,
			SERDES_IREG_FLD_RXLOCK2REF_OVREN_REG_NUM,
			rxlock2ref_ovren_val);
	return 0;
}

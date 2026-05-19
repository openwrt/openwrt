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
 *  Ethernet
 *  @{
 * @file   al_hal_eth_kr.c
 *
 * @brief  KR HAL driver for main functions (auto-neg, Link Training)
 *
 */

#include "al_hal_eth_kr.h"
#include "al_hal_eth_mac_regs.h"

/*
 * AN(Auto Negotiation) registers
 * (read / write indirect with al_eth_an_reg_read/write)
 */
#define AL_ETH_KR_AN_CONTROL					0
#define AL_ETH_KR_AN_CONTROL_RESTART				AL_BIT(9)
#define AL_ETH_KR_AN_CONTROL_ENABLE				AL_BIT(12)

#define AL_ETH_KR_AN_STATUS					1
#define AL_ETH_KR_AN_STATUS_COMPLETED				AL_BIT(5)
#define AL_ETH_KR_AN_STATUS_BASE_PAGE_RECEIVED			AL_BIT(6)
#define AL_ETH_KR_AN_STATUS_CHECK_MASK				0xFF8A
#define AL_ETH_KR_AN_STATUS_CHECK_NO_ERROR			0x0008

/* AN local advertising */
#define AL_ETH_KR_AN_ADV0					16
#define AL_ETH_KR_AN_ADV1					17
#define AL_ETH_KR_AN_ADV2					18
/* AN remote advertising */
#define AL_ETH_KR_AN_REM_ADV0					19
#define AL_ETH_KR_AN_REM_ADV1					20
#define AL_ETH_KR_AN_REM_ADV2					21
/* AN advertising registers parsing */
/* register 1 */
#define AL_ETH_KR_AN_ADV1_SEL_FIELD_MASK			0x001f
#define AL_ETH_KR_AN_ADV1_SEL_FIELD_SHIFT			0
#define AL_ETH_KR_AN_ADV1_ECHOED_NONCE_MASK			0x03e0
#define AL_ETH_KR_AN_ADV1_ECHOED_NONCE_SHIFT			5
#define AL_ETH_KR_AN_ADV1_CAPABILITY_MASK			0x1c00
#define AL_ETH_KR_AN_ADV1_CAPABILITY_SHIFT			10
#define AL_ETH_KR_AN_ADV1_REM_FAULT_MASK			0x2000
#define AL_ETH_KR_AN_ADV1_REM_FAULT_SHIFT			13
#define AL_ETH_KR_AN_ADV1_ACK_MASK				0x4000
#define AL_ETH_KR_AN_ADV1_ACK_SHIFT				14
#define AL_ETH_KR_AN_ADV1_NEXT_PAGE_MASK			0x8000
#define AL_ETH_KR_AN_ADV1_NEXT_PAGE_SHIFT			15
/* register 2 */
#define AL_ETH_KR_AN_ADV2_TX_NONCE_MASK				0x001f
#define AL_ETH_KR_AN_ADV2_TX_NONCE_SHIFT			0
#define AL_ETH_KR_AN_ADV2_TECH_MASK				0xffe0
#define AL_ETH_KR_AN_ADV2_TECH_SHIFT				5
/* register 3 */
/* TECH field in the third register is extended to the field in the second
 * register and it is currently reserved (should be always 0) */
#define AL_ETH_KR_AN_ADV3_TECH_MASK				0x1fff
#define AL_ETH_KR_AN_ADV3_TECH_SHIFT				0
#define AL_ETH_KR_AN_ADV3_FEC_MASK				0xc000
#define AL_ETH_KR_AN_ADV3_FEC_SHIFT				14

/*
 * LT(Link Training) registers
 * (read / write indirect with al_eth_pma_reg_read/write)
 */
#define AL_ETH_KR_PMD_CONTROL					150
#define AL_ETH_KR_PMD_CONTROL_RESTART				0
#define AL_ETH_KR_PMD_CONTROL_ENABLE				1

#define AL_ETH_KR_PMD_STATUS					151
#define AL_ETH_KR_PMD_STATUS_RECEIVER_COMPLETED_SHIFT		0
#define AL_ETH_KR_PMD_STATUS_RECEIVER_FRAME_LOCK_SHIFT		1
#define AL_ETH_KR_PMD_STATUS_RECEIVER_START_UP_PROTO_PROG_SHIFT	2
#define AL_ETH_KR_PMD_STATUS_FAILURE_SHIFT			3

#define AL_ETH_KR_PMD_LP_COEF_UP				152
#define AL_ETH_KR_PMD_LP_COEF_UP_MINUS_MASK			0x0003
#define AL_ETH_KR_PMD_LP_COEF_UP_MINUS_SHIFT			0
#define AL_ETH_KR_PMD_LP_COEF_UP_ZERO_MASK			0x000C
#define AL_ETH_KR_PMD_LP_COEF_UP_ZERO_SHIFT			2
#define AL_ETH_KR_PMD_LP_COEF_UP_PLUS_MASK			0x0030
#define AL_ETH_KR_PMD_LP_COEF_UP_PLUS_SHIFT			4
#define AL_ETH_KR_PMD_LP_COEF_UP_INITIALIZE_SHIFT		12
#define AL_ETH_KR_PMD_LP_COEF_UP_PRESET_SHIFT			13

#define AL_ETH_KR_PMD_LP_STATUS_REPORT				153
#define AL_ETH_KR_PMD_LP_STATUS_REPORT_MINUS_MASK		0x0003
#define AL_ETH_KR_PMD_LP_STATUS_REPORT_MINUS_SHIFT		0
#define AL_ETH_KR_PMD_LP_STATUS_REPORT_ZERO_MASK		0x000C
#define AL_ETH_KR_PMD_LP_STATUS_REPORT_ZERO_SHIFT		2
#define AL_ETH_KR_PMD_LP_STATUS_REPORT_PLUS_MASK		0x0030
#define AL_ETH_KR_PMD_LP_STATUS_REPORT_PLUS_SHIFT		4
#define AL_ETH_KR_PMD_LP_STATUS_RECEIVER_READY_SHIFT		15

#define AL_ETH_KR_PMD_LD_COEF_UP				154
#define AL_ETH_KR_PMD_LD_COEF_UP_MINUS_MASK			0x0003
#define AL_ETH_KR_PMD_LD_COEF_UP_MINUS_SHIFT			0
#define AL_ETH_KR_PMD_LD_COEF_UP_ZERO_MASK			0x000C
#define AL_ETH_KR_PMD_LD_COEF_UP_ZERO_SHIFT			2
#define AL_ETH_KR_PMD_LD_COEF_UP_PLUS_MASK			0x0030
#define AL_ETH_KR_PMD_LD_COEF_UP_PLUS_SHIFT			4
#define AL_ETH_KR_PMD_LD_COEF_UP_INITIALIZE_SHIFT		12
#define AL_ETH_KR_PMD_LD_COEF_UP_PRESET_SHIFT			13

#define AL_ETH_KR_PMD_LD_STATUS_REPORT				155
#define AL_ETH_KR_PMD_LD_STATUS_REPORT_MINUS_MASK		0x0003
#define AL_ETH_KR_PMD_LD_STATUS_REPORT_MINUS_SHIFT		0
#define AL_ETH_KR_PMD_LD_STATUS_REPORT_ZERO_MASK		0x000C
#define AL_ETH_KR_PMD_LD_STATUS_REPORT_ZERO_SHIFT		2
#define AL_ETH_KR_PMD_LD_STATUS_REPORT_PLUS_MASK		0x0030
#define AL_ETH_KR_PMD_LD_STATUS_REPORT_PLUS_SHIFT		4
#define AL_ETH_KR_PMD_LD_STATUS_REPORT_RECEIVER_READY_SHIFT	15

/**
 * read pcs auto-negotiation registers (indirect access)
 *
 * @param adapter pointer to the private structure
 * @param reg_addr address in the an registers
 *
 * @return the register value
 */
static uint16_t al_eth_an_reg_read(
			struct al_hal_eth_adapter *adapter,
			uint16_t reg_addr)
{
	uint32_t mmd_data;

	/* indirect access */
	al_reg_write32(&adapter->mac_regs_base->kr.an_addr, reg_addr);
	mmd_data = al_reg_read32(&adapter->mac_regs_base->kr.an_data);
	mmd_data &= 0xFFFF;

	al_dbg("[%s]: %s - reg %d. val 0x%x",
	       adapter->name, __func__, reg_addr, mmd_data);

	return (uint16_t)mmd_data;
}

/**
 * write pcs auto-negotiation registers (indirect access)
 *
 * @param adapter pointer to the private structure
 * @param reg_addr address in the an registers
 * @param reg_data value to write to the register
 *
 */
static void al_eth_an_reg_write(
			struct al_hal_eth_adapter *adapter,
			uint16_t reg_addr,
			uint16_t reg_data)
{
	uint32_t mmd_data = reg_data;

	/* indirect access */
	al_reg_write32(&adapter->mac_regs_base->kr.an_addr, reg_addr);
	al_reg_write32(&adapter->mac_regs_base->kr.an_data, reg_data);

	al_dbg("[%s]: %s - reg %d. val 0x%x",
	       adapter->name, __func__, reg_addr, mmd_data);
}

/**
 * read pcs pma registers (indirect access)
 *
 * @param adapter pointer to the private structure
 * @param reg_addr address in the pma registers
 *
 * @return the register value
 */
static uint16_t al_eth_pma_reg_read(
			struct al_hal_eth_adapter *adapter,
			uint16_t reg_addr)
{
	uint32_t mmd_data;

	/* indirect access */
	al_reg_write32(&adapter->mac_regs_base->kr.pma_addr, reg_addr);
	mmd_data = al_reg_read32(&adapter->mac_regs_base->kr.pma_data);
	mmd_data &= 0xFFFF;

	al_dbg("[%s]: %s - reg %d. val 0x%x",
	       adapter->name, __func__, reg_addr, mmd_data);

	return (uint16_t)mmd_data;
}

/**
 * write pcs pma registers (indirect access)
 *
 * @param adapter pointer to the private structure
 * @param reg_addr address in the pma registers
 * @param reg_data value to write to the register
 *
 */
static void al_eth_pma_reg_write(
			struct al_hal_eth_adapter *adapter,
			uint16_t reg_addr,
			uint16_t reg_data)
{
	uint32_t mmd_data = reg_data;

	/* indirect access */
	al_reg_write32(&adapter->mac_regs_base->kr.pma_addr, reg_addr);
	al_reg_write32(&adapter->mac_regs_base->kr.pma_data, reg_data);

	al_dbg("[%s]: %s - reg %d. val 0x%x",
	       adapter->name, __func__, reg_addr, mmd_data);
}

void al_eth_lp_coeff_up_get(
			struct al_hal_eth_adapter *adapter,
			struct al_eth_kr_coef_up_data *lpcoeff)
{
	uint16_t reg;

	reg = al_eth_pma_reg_read(adapter, AL_ETH_KR_PMD_LP_COEF_UP);

	lpcoeff->preset =
		(AL_REG_BIT_GET(
			reg, AL_ETH_KR_PMD_LP_COEF_UP_PRESET_SHIFT) != 0);

	lpcoeff->initialize =
		(AL_REG_BIT_GET(
			reg, AL_ETH_KR_PMD_LP_COEF_UP_INITIALIZE_SHIFT) != 0);

	lpcoeff->c_minus = AL_REG_FIELD_GET(reg,
					AL_ETH_KR_PMD_LP_COEF_UP_MINUS_MASK,
					AL_ETH_KR_PMD_LP_COEF_UP_MINUS_SHIFT);

	lpcoeff->c_zero = AL_REG_FIELD_GET(reg,
					AL_ETH_KR_PMD_LP_COEF_UP_ZERO_MASK,
					AL_ETH_KR_PMD_LP_COEF_UP_ZERO_SHIFT);

	lpcoeff->c_plus = AL_REG_FIELD_GET(reg,
					AL_ETH_KR_PMD_LP_COEF_UP_PLUS_MASK,
					AL_ETH_KR_PMD_LP_COEF_UP_PLUS_SHIFT);
}

void al_eth_lp_status_report_get(
			struct al_hal_eth_adapter *adapter,
			struct al_eth_kr_status_report_data *status)
{
	uint16_t reg;

	reg = al_eth_pma_reg_read(adapter,
				  AL_ETH_KR_PMD_LP_STATUS_REPORT);

	status->c_minus = AL_REG_FIELD_GET(reg,
				    AL_ETH_KR_PMD_LP_STATUS_REPORT_MINUS_MASK,
				    AL_ETH_KR_PMD_LP_STATUS_REPORT_MINUS_SHIFT);

	status->c_zero = AL_REG_FIELD_GET(reg,
				   AL_ETH_KR_PMD_LP_STATUS_REPORT_ZERO_MASK,
				   AL_ETH_KR_PMD_LP_STATUS_REPORT_ZERO_SHIFT);

	status->c_plus = AL_REG_FIELD_GET(reg,
				   AL_ETH_KR_PMD_LP_STATUS_REPORT_PLUS_MASK,
				   AL_ETH_KR_PMD_LP_STATUS_REPORT_PLUS_SHIFT);

	status->receiver_ready =
		(AL_REG_BIT_GET(
			reg, AL_ETH_KR_PMD_LP_STATUS_RECEIVER_READY_SHIFT) != 0);

}

void al_eth_ld_coeff_up_set(
			struct al_hal_eth_adapter *adapter,
			struct al_eth_kr_coef_up_data *ldcoeff)
{
	uint16_t reg = 0;

	if (ldcoeff->preset)
		AL_REG_BIT_SET(reg, AL_ETH_KR_PMD_LD_COEF_UP_PRESET_SHIFT);

	if (ldcoeff->initialize)
		AL_REG_BIT_SET(reg, AL_ETH_KR_PMD_LD_COEF_UP_INITIALIZE_SHIFT);

	AL_REG_FIELD_SET(reg,
			 AL_ETH_KR_PMD_LD_COEF_UP_MINUS_MASK,
			 AL_ETH_KR_PMD_LD_COEF_UP_MINUS_SHIFT,
			 ldcoeff->c_minus);

	AL_REG_FIELD_SET(reg,
			 AL_ETH_KR_PMD_LD_COEF_UP_ZERO_MASK,
			 AL_ETH_KR_PMD_LD_COEF_UP_ZERO_SHIFT,
			 ldcoeff->c_zero);

	AL_REG_FIELD_SET(reg,
			 AL_ETH_KR_PMD_LD_COEF_UP_PLUS_MASK,
			 AL_ETH_KR_PMD_LD_COEF_UP_PLUS_SHIFT,
			 ldcoeff->c_plus);

	al_eth_pma_reg_write(adapter,
			     AL_ETH_KR_PMD_LD_COEF_UP,
			     reg);
}

void al_eth_ld_status_report_set(
			struct al_hal_eth_adapter *adapter,
			struct al_eth_kr_status_report_data *status)
{
	uint16_t reg = 0;

	AL_REG_FIELD_SET(reg,
			 AL_ETH_KR_PMD_LD_STATUS_REPORT_MINUS_MASK,
			 AL_ETH_KR_PMD_LD_STATUS_REPORT_MINUS_SHIFT,
			 status->c_minus);

	AL_REG_FIELD_SET(reg,
			 AL_ETH_KR_PMD_LD_STATUS_REPORT_ZERO_MASK,
			 AL_ETH_KR_PMD_LD_STATUS_REPORT_ZERO_SHIFT,
			 status->c_zero);

	AL_REG_FIELD_SET(reg,
			 AL_ETH_KR_PMD_LD_STATUS_REPORT_PLUS_MASK,
			 AL_ETH_KR_PMD_LD_STATUS_REPORT_PLUS_SHIFT,
			 status->c_plus);

	if (status->receiver_ready)
		AL_REG_BIT_SET(reg,
			AL_ETH_KR_PMD_LD_STATUS_REPORT_RECEIVER_READY_SHIFT);

	al_eth_pma_reg_write(adapter,
			     AL_ETH_KR_PMD_LD_STATUS_REPORT,
			     reg);
}

al_bool al_eth_kr_receiver_frame_lock_get(struct al_hal_eth_adapter *adapter)
{
	uint16_t reg;

	reg = al_eth_pma_reg_read(adapter, AL_ETH_KR_PMD_STATUS);

	return (AL_REG_BIT_GET(reg,
			AL_ETH_KR_PMD_STATUS_RECEIVER_FRAME_LOCK_SHIFT) != 0);
}

al_bool al_eth_kr_startup_proto_prog_get(struct al_hal_eth_adapter *adapter)
{
	uint16_t reg;

	reg = al_eth_pma_reg_read(adapter, AL_ETH_KR_PMD_STATUS);

	return (AL_REG_BIT_GET(
		reg, AL_ETH_KR_PMD_STATUS_RECEIVER_START_UP_PROTO_PROG_SHIFT) != 0);
}

al_bool al_eth_kr_training_status_fail_get(struct al_hal_eth_adapter *adapter)
{
	uint16_t reg;

	reg = al_eth_pma_reg_read(adapter, AL_ETH_KR_PMD_STATUS);

	return (AL_REG_BIT_GET(reg, AL_ETH_KR_PMD_STATUS_FAILURE_SHIFT) != 0);
}

void al_eth_receiver_ready_set(struct al_hal_eth_adapter *adapter)
{
	al_eth_pma_reg_write(adapter, AL_ETH_KR_PMD_STATUS, 1);
}

/*************************** auto negotiation *********************************/
static int al_eth_kr_an_validate_adv(struct al_hal_eth_adapter *adapter,
				     struct al_eth_an_adv *an_adv)
{
	al_assert(adapter);

	if (an_adv == NULL)
		return 0;

	if (an_adv->selector_field != 1) {
		al_err("[%s]: %s failed on selector_field (%d)\n",
			adapter->name, __func__, an_adv->selector_field);
		return -EINVAL;
	}

	if (an_adv->capability & AL_BIT(2)) {
		al_err("[%s]: %s failed on capability bit 2 (%d)\n",
			adapter->name, __func__, an_adv->capability);
		return -EINVAL;
	}

	if (an_adv->remote_fault) {
		al_err("[%s]: %s failed on remote_fault (%d)\n",
			adapter->name, __func__, an_adv->remote_fault);
		return -EINVAL;
	}

	if (an_adv->acknowledge) {
		al_err("[%s]: %s failed on acknowledge (%d)\n",
			adapter->name, __func__, an_adv->acknowledge);
		return -EINVAL;
	}

	if (an_adv->next_page) {
		al_err("[%s]: %s failed on next_page (%d)\n",
			adapter->name, __func__, an_adv->next_page);
		return -EINVAL;
	}

	if (an_adv->technology != AL_ETH_AN_TECH_10GBASE_KR) {
		al_err("[%s]: %s failed on technology (0x%x)\n",
			adapter->name, __func__, an_adv->technology);
		return -EINVAL;
	}
	return 0;
}

static int al_eth_kr_an_write_adv(struct al_hal_eth_adapter *adapter,
			       struct al_eth_an_adv *an_adv)
{
	uint16_t	reg;

	if(an_adv == NULL)
		return 0;

	reg = 0;
	AL_REG_FIELD_SET(reg, AL_ETH_KR_AN_ADV1_SEL_FIELD_MASK,
			 AL_ETH_KR_AN_ADV1_SEL_FIELD_SHIFT,
			 an_adv->selector_field);

	AL_REG_FIELD_SET(reg, AL_ETH_KR_AN_ADV1_ECHOED_NONCE_MASK,
			 AL_ETH_KR_AN_ADV1_ECHOED_NONCE_SHIFT,
			 an_adv->echoed_nonce);

	AL_REG_FIELD_SET(reg, AL_ETH_KR_AN_ADV1_CAPABILITY_MASK,
			 AL_ETH_KR_AN_ADV1_CAPABILITY_SHIFT,
			 an_adv->capability);

	AL_REG_BIT_VAL_SET(reg, AL_ETH_KR_AN_ADV1_REM_FAULT_SHIFT,
			   an_adv->remote_fault);

	AL_REG_BIT_VAL_SET(reg, AL_ETH_KR_AN_ADV1_ACK_SHIFT,
			   an_adv->acknowledge);

	AL_REG_BIT_VAL_SET(reg, AL_ETH_KR_AN_ADV1_NEXT_PAGE_SHIFT,
			   an_adv->next_page);

	al_eth_an_reg_write(adapter, AL_ETH_KR_AN_ADV0, reg);

	reg = 0;
	AL_REG_FIELD_SET(reg, AL_ETH_KR_AN_ADV2_TX_NONCE_MASK,
			 AL_ETH_KR_AN_ADV2_TX_NONCE_SHIFT,
			 an_adv->transmitted_nonce);

	AL_REG_FIELD_SET(reg, AL_ETH_KR_AN_ADV2_TECH_MASK,
			 AL_ETH_KR_AN_ADV2_TECH_SHIFT,
			 an_adv->technology);

	al_eth_an_reg_write(adapter, AL_ETH_KR_AN_ADV1, reg);

	reg = 0;
	AL_REG_FIELD_SET(reg, AL_ETH_KR_AN_ADV3_TECH_MASK,
			 AL_ETH_KR_AN_ADV3_TECH_SHIFT,
			 an_adv->technology >> 11);

	AL_REG_FIELD_SET(reg, AL_ETH_KR_AN_ADV3_FEC_MASK,
			 AL_ETH_KR_AN_ADV3_FEC_SHIFT,
			 an_adv->fec_capability);

	al_eth_an_reg_write(adapter, AL_ETH_KR_AN_ADV2, reg);

	return 0;
}

void al_eth_kr_an_read_adv(struct al_hal_eth_adapter *adapter,
			   struct al_eth_an_adv *an_adv)
{
	int16_t	reg;

	al_assert(an_adv != NULL);


	reg = al_eth_an_reg_read(adapter, AL_ETH_KR_AN_REM_ADV0);

	an_adv->selector_field = AL_REG_FIELD_GET(reg,
					AL_ETH_KR_AN_ADV1_SEL_FIELD_MASK,
					AL_ETH_KR_AN_ADV1_SEL_FIELD_SHIFT);

	an_adv->echoed_nonce = AL_REG_FIELD_GET(reg,
					AL_ETH_KR_AN_ADV1_ECHOED_NONCE_MASK,
					AL_ETH_KR_AN_ADV1_ECHOED_NONCE_SHIFT);

	an_adv->capability = AL_REG_FIELD_GET(reg,
					AL_ETH_KR_AN_ADV1_CAPABILITY_MASK,
					AL_ETH_KR_AN_ADV1_CAPABILITY_SHIFT);

	an_adv->remote_fault = AL_REG_BIT_GET(reg,
					AL_ETH_KR_AN_ADV1_REM_FAULT_SHIFT);

	an_adv->acknowledge = AL_REG_BIT_GET(reg,
					AL_ETH_KR_AN_ADV1_ACK_SHIFT);

	an_adv->next_page = AL_REG_BIT_GET(reg,
					AL_ETH_KR_AN_ADV1_NEXT_PAGE_SHIFT);


	reg = al_eth_an_reg_read(adapter, AL_ETH_KR_AN_REM_ADV1);

	an_adv->transmitted_nonce = AL_REG_FIELD_GET(reg,
					 AL_ETH_KR_AN_ADV2_TX_NONCE_MASK,
					 AL_ETH_KR_AN_ADV2_TX_NONCE_SHIFT);

	an_adv->technology = AL_REG_FIELD_GET(reg,
					 AL_ETH_KR_AN_ADV2_TECH_MASK,
					 AL_ETH_KR_AN_ADV2_TECH_SHIFT);


	reg = al_eth_an_reg_read(adapter, AL_ETH_KR_AN_REM_ADV2);

	an_adv->technology |= (AL_REG_FIELD_GET(reg,
					AL_ETH_KR_AN_ADV3_TECH_MASK,
					AL_ETH_KR_AN_ADV3_TECH_SHIFT)  << 11);

	an_adv->fec_capability = AL_REG_FIELD_GET(reg,
					 AL_ETH_KR_AN_ADV3_FEC_MASK,
					 AL_ETH_KR_AN_ADV3_FEC_SHIFT);
}


int al_eth_kr_an_init(struct al_hal_eth_adapter *adapter,
		      struct al_eth_an_adv *an_adv)
{
	int		rc;

	rc = al_eth_kr_an_validate_adv(adapter, an_adv);
	if (rc)
		return rc;

	rc = al_eth_kr_an_write_adv(adapter, an_adv);
	if (rc)
		return rc;

	/* clear status */
	al_eth_an_reg_read(adapter, AL_ETH_KR_AN_STATUS);

	al_dbg("[%s]: autonegotiation initialized successfully", adapter->name);
	return 0;
}

int al_eth_kr_an_start(struct al_hal_eth_adapter *adapter, al_bool lt_enable)
{
	al_dbg("Eth [%s]: enable autonegotiation. lt_en %s",
		adapter->name, (lt_enable == AL_TRUE) ? "yes" : "no");

	al_eth_pma_reg_write(adapter,
			     AL_ETH_KR_PMD_CONTROL,
			     AL_BIT(AL_ETH_KR_PMD_CONTROL_RESTART));

	al_eth_an_reg_write(adapter,
				AL_ETH_KR_AN_CONTROL,
				(AL_ETH_KR_AN_CONTROL_ENABLE |
				AL_ETH_KR_AN_CONTROL_RESTART));

	if (lt_enable == AL_TRUE) {
		al_eth_kr_lt_initialize(adapter);
	}

	return 0;
}

void al_eth_kr_an_stop(struct al_hal_eth_adapter *adapter)
{
	al_eth_an_reg_write(adapter, AL_ETH_KR_AN_CONTROL, 0);
}

void al_eth_kr_an_status_check(struct al_hal_eth_adapter *adapter,
			      al_bool *page_received,
			      al_bool *an_completed,
			      al_bool *error)
{
	uint16_t	reg;

	reg = al_eth_an_reg_read(adapter, AL_ETH_KR_AN_STATUS);

	if ((reg & AL_ETH_KR_AN_STATUS_CHECK_MASK) !=
				AL_ETH_KR_AN_STATUS_CHECK_NO_ERROR) {
		al_err("[%s]: %s AN_STATUS (0x%x) indicated error\n",
			adapter->name, __func__, reg);

		*error = AL_TRUE;
	}

	if (reg & AL_ETH_KR_AN_STATUS_BASE_PAGE_RECEIVED)
		*page_received = AL_TRUE;
	else
		*page_received = AL_FALSE;

	if (reg & AL_ETH_KR_AN_STATUS_COMPLETED)
		*an_completed = AL_TRUE;
	else
		*an_completed = AL_FALSE;
}


/****************************** KR Link Training *****************************/
void al_eth_kr_lt_restart(struct al_hal_eth_adapter *adapter)
{
	al_dbg("[%s]: KR LT Restart Link Training.\n", adapter->name);

	al_eth_pma_reg_write(adapter,
			     AL_ETH_KR_PMD_CONTROL,
			     (AL_BIT(AL_ETH_KR_PMD_CONTROL_ENABLE) |
			     AL_BIT(AL_ETH_KR_PMD_CONTROL_RESTART)));
}

void al_eth_kr_lt_stop(struct al_hal_eth_adapter *adapter)
{
	al_dbg("[%s]: KR LT Stop Link Training.\n", adapter->name);

	al_eth_pma_reg_write(adapter,
			     AL_ETH_KR_PMD_CONTROL,
			     AL_BIT(AL_ETH_KR_PMD_CONTROL_RESTART));
}

void al_eth_kr_lt_initialize(struct al_hal_eth_adapter *adapter)
{
	al_dbg("[%s]: KR LT Initialize.\n", adapter->name);

	/* Reset LT state machine */
	al_eth_kr_lt_stop(adapter);

	/* clear receiver status */
	al_eth_pma_reg_write(adapter, AL_ETH_KR_PMD_STATUS, 0);

	/* Coefficient Update to all zero (no command, hold) */
	al_eth_pma_reg_write(adapter, AL_ETH_KR_PMD_LD_COEF_UP, 0);
	/* Coefficient Status to all zero (not_updated) */
	al_eth_pma_reg_write(adapter, AL_ETH_KR_PMD_LD_STATUS_REPORT, 0);

	/* start */
	al_eth_kr_lt_restart(adapter);
}

al_bool al_eth_kr_lt_frame_lock_wait(struct al_hal_eth_adapter *adapter,
				     uint32_t timeout)
{
	uint32_t loop;
	uint16_t reg = 0;

	for (loop = 0; loop < timeout; loop++) {
		reg = al_eth_pma_reg_read(adapter, AL_ETH_KR_PMD_STATUS);

		if (AL_REG_BIT_GET(reg, AL_ETH_KR_PMD_STATUS_FAILURE_SHIFT)) {
			al_info("[%s]: Failed on Training Failure."
			       " loops %d PMD STATUS 0x%04x\n",
			       adapter->name, loop, reg);

			return AL_FALSE;
		}
		if (AL_REG_BIT_GET(reg,
			AL_ETH_KR_PMD_STATUS_RECEIVER_FRAME_LOCK_SHIFT)) {
			al_dbg("[%s]: Frame lock received."
			       " loops %d PMD STATUS 0x%04x\n",
			       adapter->name, loop, reg);

			return AL_TRUE;
		}
		al_udelay(1);
	}
	al_info("[%s]: Failed on timeout. PMD STATUS 0x%04x\n",
			adapter->name, reg);

	return AL_FALSE;
}

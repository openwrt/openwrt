/*
 * Copyright (c) 2016-2017, The Linux Foundation. All rights reserved.
 * Permission to use, copy, modify, and/or distribute this software for
 * any purpose with or without fee is hereby granted, provided that the
 * above copyright notice and this permission notice appear in all copies.
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT
 * OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */


/**
 * @defgroup
 * @{
 */
#include "sw.h"
#include "hsl.h"
#include "hppe_reg_access.h"
#include "hppe_mib_reg.h"
#include "hppe_mib.h"

sw_error_t
hppe_mac_mib_ctrl_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union mac_mib_ctrl_u *value)
{
	if (index >= MAC_MIB_CTRL_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				NSS_MAC_CSR_BASE_ADDR + MAC_MIB_CTRL_ADDRESS + \
				index * MAC_MIB_CTRL_INC,
				&value->val);
}

sw_error_t
hppe_mac_mib_ctrl_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union mac_mib_ctrl_u *value)
{
	return hppe_reg_set(
				dev_id,
				NSS_MAC_CSR_BASE_ADDR + MAC_MIB_CTRL_ADDRESS + \
				index * MAC_MIB_CTRL_INC,
				value->val);
}

sw_error_t
hppe_rxbroad_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rxbroad_u *value)
{
	if (index >= RXBROAD_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				NSS_MAC_CSR_BASE_ADDR + RXBROAD_ADDRESS + \
				index * RXBROAD_INC,
				&value->val);
}

sw_error_t
hppe_rxbroad_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rxbroad_u *value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_rxpause_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rxpause_u *value)
{
	if (index >= RXPAUSE_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				NSS_MAC_CSR_BASE_ADDR + RXPAUSE_ADDRESS + \
				index * RXPAUSE_INC,
				&value->val);
}

sw_error_t
hppe_rxpause_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rxpause_u *value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_rxmulti_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rxmulti_u *value)
{
	if (index >= RXMULTI_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				NSS_MAC_CSR_BASE_ADDR + RXMULTI_ADDRESS + \
				index * RXMULTI_INC,
				&value->val);
}

sw_error_t
hppe_rxmulti_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rxmulti_u *value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_rxfcserr_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rxfcserr_u *value)
{
	if (index >= RXFCSERR_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				NSS_MAC_CSR_BASE_ADDR + RXFCSERR_ADDRESS + \
				index * RXFCSERR_INC,
				&value->val);
}

sw_error_t
hppe_rxfcserr_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rxfcserr_u *value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_rxalignerr_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rxalignerr_u *value)
{
	if (index >= RXALIGNERR_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				NSS_MAC_CSR_BASE_ADDR + RXALIGNERR_ADDRESS + \
				index * RXALIGNERR_INC,
				&value->val);
}

sw_error_t
hppe_rxalignerr_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rxalignerr_u *value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_rxrunt_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rxrunt_u *value)
{
	if (index >= RXRUNT_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				NSS_MAC_CSR_BASE_ADDR + RXRUNT_ADDRESS + \
				index * RXRUNT_INC,
				&value->val);
}

sw_error_t
hppe_rxrunt_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rxrunt_u *value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_rxfrag_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rxfrag_u *value)
{
	if (index >= RXFRAG_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				NSS_MAC_CSR_BASE_ADDR + RXFRAG_ADDRESS + \
				index * RXFRAG_INC,
				&value->val);
}

sw_error_t
hppe_rxfrag_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rxfrag_u *value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_rxjumbofcserr_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rxjumbofcserr_u *value)
{
	if (index >= RXJUMBOFCSERR_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				NSS_MAC_CSR_BASE_ADDR + RXJUMBOFCSERR_ADDRESS + \
				index * RXJUMBOFCSERR_INC,
				&value->val);
}

sw_error_t
hppe_rxjumbofcserr_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rxjumbofcserr_u *value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_rxjumboalignerr_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rxjumboalignerr_u *value)
{
	if (index >= RXJUMBOALIGNERR_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				NSS_MAC_CSR_BASE_ADDR + RXJUMBOALIGNERR_ADDRESS + \
				index * RXJUMBOALIGNERR_INC,
				&value->val);
}

sw_error_t
hppe_rxjumboalignerr_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rxjumboalignerr_u *value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_rxpkt64_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rxpkt64_u *value)
{
	if (index >= RXPKT64_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				NSS_MAC_CSR_BASE_ADDR + RXPKT64_ADDRESS + \
				index * RXPKT64_INC,
				&value->val);
}

sw_error_t
hppe_rxpkt64_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rxpkt64_u *value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_rxpkt65to127_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rxpkt65to127_u *value)
{
	if (index >= RXPKT65TO127_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				NSS_MAC_CSR_BASE_ADDR + RXPKT65TO127_ADDRESS + \
				index * RXPKT65TO127_INC,
				&value->val);
}

sw_error_t
hppe_rxpkt65to127_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rxpkt65to127_u *value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_rxpkt128to255_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rxpkt128to255_u *value)
{
	if (index >= RXPKT128TO255_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				NSS_MAC_CSR_BASE_ADDR + RXPKT128TO255_ADDRESS + \
				index * RXPKT128TO255_INC,
				&value->val);
}

sw_error_t
hppe_rxpkt128to255_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rxpkt128to255_u *value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_rxpkt256to511_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rxpkt256to511_u *value)
{
	if (index >= RXPKT256TO511_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				NSS_MAC_CSR_BASE_ADDR + RXPKT256TO511_ADDRESS + \
				index * RXPKT256TO511_INC,
				&value->val);
}

sw_error_t
hppe_rxpkt256to511_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rxpkt256to511_u *value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_rxpkt512to1023_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rxpkt512to1023_u *value)
{
	if (index >= RXPKT512TO1023_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				NSS_MAC_CSR_BASE_ADDR + RXPKT512TO1023_ADDRESS + \
				index * RXPKT512TO1023_INC,
				&value->val);
}

sw_error_t
hppe_rxpkt512to1023_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rxpkt512to1023_u *value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_rxpkt1024to1518_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rxpkt1024to1518_u *value)
{
	if (index >= RXPKT1024TO1518_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				NSS_MAC_CSR_BASE_ADDR + RXPKT1024TO1518_ADDRESS + \
				index * RXPKT1024TO1518_INC,
				&value->val);
}

sw_error_t
hppe_rxpkt1024to1518_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rxpkt1024to1518_u *value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_rxpkt1519tox_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rxpkt1519tox_u *value)
{
	if (index >= RXPKT1519TOX_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				NSS_MAC_CSR_BASE_ADDR + RXPKT1519TOX_ADDRESS + \
				index * RXPKT1519TOX_INC,
				&value->val);
}

sw_error_t
hppe_rxpkt1519tox_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rxpkt1519tox_u *value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_rxtoolong_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rxtoolong_u *value)
{
	if (index >= RXTOOLONG_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				NSS_MAC_CSR_BASE_ADDR + RXTOOLONG_ADDRESS + \
				index * RXTOOLONG_INC,
				&value->val);
}

sw_error_t
hppe_rxtoolong_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rxtoolong_u *value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_rxgoodbyte_l_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rxgoodbyte_l_u *value)
{
	if (index >= RXGOODBYTE_L_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				NSS_MAC_CSR_BASE_ADDR + RXGOODBYTE_L_ADDRESS + \
				index * RXGOODBYTE_L_INC,
				&value->val);
}

sw_error_t
hppe_rxgoodbyte_l_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rxgoodbyte_l_u *value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_rxgoodbyte_h_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rxgoodbyte_h_u *value)
{
	if (index >= RXGOODBYTE_H_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				NSS_MAC_CSR_BASE_ADDR + RXGOODBYTE_H_ADDRESS + \
				index * RXGOODBYTE_H_INC,
				&value->val);
}

sw_error_t
hppe_rxgoodbyte_h_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rxgoodbyte_h_u *value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_rxbadbyte_l_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rxbadbyte_l_u *value)
{
	if (index >= RXBADBYTE_L_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				NSS_MAC_CSR_BASE_ADDR + RXBADBYTE_L_ADDRESS + \
				index * RXBADBYTE_L_INC,
				&value->val);
}

sw_error_t
hppe_rxbadbyte_l_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rxbadbyte_l_u *value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_rxbadbyte_h_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rxbadbyte_h_u *value)
{
	if (index >= RXBADBYTE_H_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				NSS_MAC_CSR_BASE_ADDR + RXBADBYTE_H_ADDRESS + \
				index * RXBADBYTE_H_INC,
				&value->val);
}

sw_error_t
hppe_rxbadbyte_h_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rxbadbyte_h_u *value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_rxuni_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rxuni_u *value)
{
	if (index >= RXUNI_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				NSS_MAC_CSR_BASE_ADDR + RXUNI_ADDRESS + \
				index * RXUNI_INC,
				&value->val);
}

sw_error_t
hppe_rxuni_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rxuni_u *value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_txbroad_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union txbroad_u *value)
{
	if (index >= TXBROAD_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				NSS_MAC_CSR_BASE_ADDR + TXBROAD_ADDRESS + \
				index * TXBROAD_INC,
				&value->val);
}

sw_error_t
hppe_txbroad_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union txbroad_u *value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_txpause_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union txpause_u *value)
{
	if (index >= TXPAUSE_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				NSS_MAC_CSR_BASE_ADDR + TXPAUSE_ADDRESS + \
				index * TXPAUSE_INC,
				&value->val);
}

sw_error_t
hppe_txpause_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union txpause_u *value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_txmulti_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union txmulti_u *value)
{
	if (index >= TXMULTI_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				NSS_MAC_CSR_BASE_ADDR + TXMULTI_ADDRESS + \
				index * TXMULTI_INC,
				&value->val);
}

sw_error_t
hppe_txmulti_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union txmulti_u *value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_txunderrun_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union txunderrun_u *value)
{
	if (index >= TXUNDERRUN_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				NSS_MAC_CSR_BASE_ADDR + TXUNDERRUN_ADDRESS + \
				index * TXUNDERRUN_INC,
				&value->val);
}

sw_error_t
hppe_txunderrun_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union txunderrun_u *value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_txpkt64_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union txpkt64_u *value)
{
	if (index >= TXPKT64_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				NSS_MAC_CSR_BASE_ADDR + TXPKT64_ADDRESS + \
				index * TXPKT64_INC,
				&value->val);
}

sw_error_t
hppe_txpkt64_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union txpkt64_u *value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_txpkt65to127_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union txpkt65to127_u *value)
{
	if (index >= TXPKT65TO127_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				NSS_MAC_CSR_BASE_ADDR + TXPKT65TO127_ADDRESS + \
				index * TXPKT65TO127_INC,
				&value->val);
}

sw_error_t
hppe_txpkt65to127_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union txpkt65to127_u *value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_txpkt128to255_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union txpkt128to255_u *value)
{
	if (index >= TXPKT128TO255_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				NSS_MAC_CSR_BASE_ADDR + TXPKT128TO255_ADDRESS + \
				index * TXPKT128TO255_INC,
				&value->val);
}

sw_error_t
hppe_txpkt128to255_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union txpkt128to255_u *value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_txpkt256to511_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union txpkt256to511_u *value)
{
	if (index >= TXPKT256TO511_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				NSS_MAC_CSR_BASE_ADDR + TXPKT256TO511_ADDRESS + \
				index * TXPKT256TO511_INC,
				&value->val);
}

sw_error_t
hppe_txpkt256to511_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union txpkt256to511_u *value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_txpkt512to1023_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union txpkt512to1023_u *value)
{
	if (index >= TXPKT512TO1023_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				NSS_MAC_CSR_BASE_ADDR + TXPKT512TO1023_ADDRESS + \
				index * TXPKT512TO1023_INC,
				&value->val);
}

sw_error_t
hppe_txpkt512to1023_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union txpkt512to1023_u *value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_txpkt1024to1518_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union txpkt1024to1518_u *value)
{
	if (index >= TXPKT1024TO1518_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				NSS_MAC_CSR_BASE_ADDR + TXPKT1024TO1518_ADDRESS + \
				index * TXPKT1024TO1518_INC,
				&value->val);
}

sw_error_t
hppe_txpkt1024to1518_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union txpkt1024to1518_u *value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_txpkt1519tox_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union txpkt1519tox_u *value)
{
	if (index >= TXPKT1519TOX_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				NSS_MAC_CSR_BASE_ADDR + TXPKT1519TOX_ADDRESS + \
				index * TXPKT1519TOX_INC,
				&value->val);
}

sw_error_t
hppe_txpkt1519tox_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union txpkt1519tox_u *value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_txbyte_l_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union txbyte_l_u *value)
{
	if (index >= TXBYTE_L_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				NSS_MAC_CSR_BASE_ADDR + TXBYTE_L_ADDRESS + \
				index * TXBYTE_L_INC,
				&value->val);
}

sw_error_t
hppe_txbyte_l_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union txbyte_l_u *value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_txbyte_h_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union txbyte_h_u *value)
{
	if (index >= TXBYTE_H_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				NSS_MAC_CSR_BASE_ADDR + TXBYTE_H_ADDRESS + \
				index * TXBYTE_H_INC,
				&value->val);
}

sw_error_t
hppe_txbyte_h_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union txbyte_h_u *value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_txcollisions_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union txcollisions_u *value)
{
	if (index >= TXCOLLISIONS_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				NSS_MAC_CSR_BASE_ADDR + TXCOLLISIONS_ADDRESS + \
				index * TXCOLLISIONS_INC,
				&value->val);
}

sw_error_t
hppe_txcollisions_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union txcollisions_u *value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_txabortcol_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union txabortcol_u *value)
{
	if (index >= TXABORTCOL_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				NSS_MAC_CSR_BASE_ADDR + TXABORTCOL_ADDRESS + \
				index * TXABORTCOL_INC,
				&value->val);
}

sw_error_t
hppe_txabortcol_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union txabortcol_u *value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_txmulticol_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union txmulticol_u *value)
{
	if (index >= TXMULTICOL_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				NSS_MAC_CSR_BASE_ADDR + TXMULTICOL_ADDRESS + \
				index * TXMULTICOL_INC,
				&value->val);
}

sw_error_t
hppe_txmulticol_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union txmulticol_u *value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_txsinglecol_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union txsinglecol_u *value)
{
	if (index >= TXSINGLECOL_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				NSS_MAC_CSR_BASE_ADDR + TXSINGLECOL_ADDRESS + \
				index * TXSINGLECOL_INC,
				&value->val);
}

sw_error_t
hppe_txsinglecol_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union txsinglecol_u *value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_txexcessivedefer_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union txexcessivedefer_u *value)
{
	if (index >= TXEXCESSIVEDEFER_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				NSS_MAC_CSR_BASE_ADDR + TXEXCESSIVEDEFER_ADDRESS + \
				index * TXEXCESSIVEDEFER_INC,
				&value->val);
}

sw_error_t
hppe_txexcessivedefer_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union txexcessivedefer_u *value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_txdefer_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union txdefer_u *value)
{
	if (index >= TXDEFER_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				NSS_MAC_CSR_BASE_ADDR + TXDEFER_ADDRESS + \
				index * TXDEFER_INC,
				&value->val);
}

sw_error_t
hppe_txdefer_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union txdefer_u *value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_txlatecol_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union txlatecol_u *value)
{
	if (index >= TXLATECOL_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				NSS_MAC_CSR_BASE_ADDR + TXLATECOL_ADDRESS + \
				index * TXLATECOL_INC,
				&value->val);
}

sw_error_t
hppe_txlatecol_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union txlatecol_u *value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_txuni_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union txuni_u *value)
{
	if (index >= TXUNI_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				NSS_MAC_CSR_BASE_ADDR + TXUNI_ADDRESS + \
				index * TXUNI_INC,
				&value->val);
}

sw_error_t
hppe_txuni_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union txuni_u *value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_mac_mib_ctrl_mib_reset_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mac_mib_ctrl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_mib_ctrl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.mib_reset;
	return ret;
}

sw_error_t
hppe_mac_mib_ctrl_mib_reset_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union mac_mib_ctrl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_mib_ctrl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.mib_reset = value;
	ret = hppe_mac_mib_ctrl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_mac_mib_ctrl_mib_en_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mac_mib_ctrl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_mib_ctrl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.mib_en;
	return ret;
}

sw_error_t
hppe_mac_mib_ctrl_mib_en_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union mac_mib_ctrl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_mib_ctrl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.mib_en = value;
	ret = hppe_mac_mib_ctrl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_mac_mib_ctrl_mib_rd_clr_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mac_mib_ctrl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_mib_ctrl_get(dev_id, index, &reg_val);
	*value = reg_val.bf.mib_rd_clr;
	return ret;
}

sw_error_t
hppe_mac_mib_ctrl_mib_rd_clr_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	union mac_mib_ctrl_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mac_mib_ctrl_get(dev_id, index, &reg_val);
	if (SW_OK != ret)
		return ret;
	reg_val.bf.mib_rd_clr = value;
	ret = hppe_mac_mib_ctrl_set(dev_id, index, &reg_val);
	return ret;
}

sw_error_t
hppe_rxbroad_rxbroad_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union rxbroad_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_rxbroad_get(dev_id, index, &reg_val);
	*value = reg_val.bf.rxbroad;
	return ret;
}

sw_error_t
hppe_rxbroad_rxbroad_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_rxpause_rxpause_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union rxpause_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_rxpause_get(dev_id, index, &reg_val);
	*value = reg_val.bf.rxpause;
	return ret;
}

sw_error_t
hppe_rxpause_rxpause_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_rxmulti_rxmulti_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union rxmulti_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_rxmulti_get(dev_id, index, &reg_val);
	*value = reg_val.bf.rxmulti;
	return ret;
}

sw_error_t
hppe_rxmulti_rxmulti_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_rxfcserr_rxfcserr_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union rxfcserr_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_rxfcserr_get(dev_id, index, &reg_val);
	*value = reg_val.bf.rxfcserr;
	return ret;
}

sw_error_t
hppe_rxfcserr_rxfcserr_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_rxalignerr_rxalignerr_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union rxalignerr_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_rxalignerr_get(dev_id, index, &reg_val);
	*value = reg_val.bf.rxalignerr;
	return ret;
}

sw_error_t
hppe_rxalignerr_rxalignerr_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_rxrunt_rxrunt_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union rxrunt_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_rxrunt_get(dev_id, index, &reg_val);
	*value = reg_val.bf.rxrunt;
	return ret;
}

sw_error_t
hppe_rxrunt_rxrunt_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_rxfrag_rxfrag_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union rxfrag_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_rxfrag_get(dev_id, index, &reg_val);
	*value = reg_val.bf.rxfrag;
	return ret;
}

sw_error_t
hppe_rxfrag_rxfrag_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_rxjumbofcserr_rxjumbofcserr_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union rxjumbofcserr_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_rxjumbofcserr_get(dev_id, index, &reg_val);
	*value = reg_val.bf.rxjumbofcserr;
	return ret;
}

sw_error_t
hppe_rxjumbofcserr_rxjumbofcserr_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_rxjumboalignerr_rxjumboalignerr_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union rxjumboalignerr_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_rxjumboalignerr_get(dev_id, index, &reg_val);
	*value = reg_val.bf.rxjumboalignerr;
	return ret;
}

sw_error_t
hppe_rxjumboalignerr_rxjumboalignerr_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_rxpkt64_rxpkt64_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union rxpkt64_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_rxpkt64_get(dev_id, index, &reg_val);
	*value = reg_val.bf.rxpkt64;
	return ret;
}

sw_error_t
hppe_rxpkt64_rxpkt64_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_rxpkt65to127_rxpkt65to127_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union rxpkt65to127_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_rxpkt65to127_get(dev_id, index, &reg_val);
	*value = reg_val.bf.rxpkt65to127;
	return ret;
}

sw_error_t
hppe_rxpkt65to127_rxpkt65to127_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_rxpkt128to255_rxpkt128to255_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union rxpkt128to255_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_rxpkt128to255_get(dev_id, index, &reg_val);
	*value = reg_val.bf.rxpkt128to255;
	return ret;
}

sw_error_t
hppe_rxpkt128to255_rxpkt128to255_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_rxpkt256to511_rxpkt256to511_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union rxpkt256to511_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_rxpkt256to511_get(dev_id, index, &reg_val);
	*value = reg_val.bf.rxpkt256to511;
	return ret;
}

sw_error_t
hppe_rxpkt256to511_rxpkt256to511_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_rxpkt512to1023_rxpkt512to1023_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union rxpkt512to1023_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_rxpkt512to1023_get(dev_id, index, &reg_val);
	*value = reg_val.bf.rxpkt512to1023;
	return ret;
}

sw_error_t
hppe_rxpkt512to1023_rxpkt512to1023_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_rxpkt1024to1518_rxpkt1024to1518_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union rxpkt1024to1518_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_rxpkt1024to1518_get(dev_id, index, &reg_val);
	*value = reg_val.bf.rxpkt1024to1518;
	return ret;
}

sw_error_t
hppe_rxpkt1024to1518_rxpkt1024to1518_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_rxpkt1519tox_rxpkt1519tox_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union rxpkt1519tox_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_rxpkt1519tox_get(dev_id, index, &reg_val);
	*value = reg_val.bf.rxpkt1519tox;
	return ret;
}

sw_error_t
hppe_rxpkt1519tox_rxpkt1519tox_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_rxtoolong_rxtoolong_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union rxtoolong_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_rxtoolong_get(dev_id, index, &reg_val);
	*value = reg_val.bf.rxtoolong;
	return ret;
}

sw_error_t
hppe_rxtoolong_rxtoolong_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_rxgoodbyte_l_rxgoodbyte_l_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union rxgoodbyte_l_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_rxgoodbyte_l_get(dev_id, index, &reg_val);
	*value = reg_val.bf.rxgoodbyte_l;
	return ret;
}

sw_error_t
hppe_rxgoodbyte_l_rxgoodbyte_l_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_rxgoodbyte_h_rxgoodbyte_h_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union rxgoodbyte_h_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_rxgoodbyte_h_get(dev_id, index, &reg_val);
	*value = reg_val.bf.rxgoodbyte_h;
	return ret;
}

sw_error_t
hppe_rxgoodbyte_h_rxgoodbyte_h_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_rxbadbyte_l_rxbadbyte_l_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union rxbadbyte_l_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_rxbadbyte_l_get(dev_id, index, &reg_val);
	*value = reg_val.bf.rxbadbyte_l;
	return ret;
}

sw_error_t
hppe_rxbadbyte_l_rxbadbyte_l_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_rxbadbyte_h_rxbadbyte_h_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union rxbadbyte_h_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_rxbadbyte_h_get(dev_id, index, &reg_val);
	*value = reg_val.bf.rxbadbyte_h;
	return ret;
}

sw_error_t
hppe_rxbadbyte_h_rxbadbyte_h_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_rxuni_rxuni_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union rxuni_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_rxuni_get(dev_id, index, &reg_val);
	*value = reg_val.bf.rxuni;
	return ret;
}

sw_error_t
hppe_rxuni_rxuni_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_txbroad_txbroad_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union txbroad_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_txbroad_get(dev_id, index, &reg_val);
	*value = reg_val.bf.txbroad;
	return ret;
}

sw_error_t
hppe_txbroad_txbroad_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_txpause_txpause_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union txpause_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_txpause_get(dev_id, index, &reg_val);
	*value = reg_val.bf.txpause;
	return ret;
}

sw_error_t
hppe_txpause_txpause_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_txmulti_txmulti_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union txmulti_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_txmulti_get(dev_id, index, &reg_val);
	*value = reg_val.bf.txmulti;
	return ret;
}

sw_error_t
hppe_txmulti_txmulti_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_txunderrun_txunderrun_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union txunderrun_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_txunderrun_get(dev_id, index, &reg_val);
	*value = reg_val.bf.txunderrun;
	return ret;
}

sw_error_t
hppe_txunderrun_txunderrun_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_txpkt64_txpkt64_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union txpkt64_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_txpkt64_get(dev_id, index, &reg_val);
	*value = reg_val.bf.txpkt64;
	return ret;
}

sw_error_t
hppe_txpkt64_txpkt64_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_txpkt65to127_txpkt65to127_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union txpkt65to127_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_txpkt65to127_get(dev_id, index, &reg_val);
	*value = reg_val.bf.txpkt65to127;
	return ret;
}

sw_error_t
hppe_txpkt65to127_txpkt65to127_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_txpkt128to255_txpkt128to255_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union txpkt128to255_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_txpkt128to255_get(dev_id, index, &reg_val);
	*value = reg_val.bf.txpkt128to255;
	return ret;
}

sw_error_t
hppe_txpkt128to255_txpkt128to255_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_txpkt256to511_txpkt256to511_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union txpkt256to511_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_txpkt256to511_get(dev_id, index, &reg_val);
	*value = reg_val.bf.txpkt256to511;
	return ret;
}

sw_error_t
hppe_txpkt256to511_txpkt256to511_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_txpkt512to1023_txpkt512to1023_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union txpkt512to1023_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_txpkt512to1023_get(dev_id, index, &reg_val);
	*value = reg_val.bf.txpkt512to1023;
	return ret;
}

sw_error_t
hppe_txpkt512to1023_txpkt512to1023_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_txpkt1024to1518_txpkt1024to1518_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union txpkt1024to1518_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_txpkt1024to1518_get(dev_id, index, &reg_val);
	*value = reg_val.bf.txpkt1024to1518;
	return ret;
}

sw_error_t
hppe_txpkt1024to1518_txpkt1024to1518_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_txpkt1519tox_txpkt1519tox_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union txpkt1519tox_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_txpkt1519tox_get(dev_id, index, &reg_val);
	*value = reg_val.bf.txpkt1519tox;
	return ret;
}

sw_error_t
hppe_txpkt1519tox_txpkt1519tox_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_txbyte_l_txbyte_l_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union txbyte_l_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_txbyte_l_get(dev_id, index, &reg_val);
	*value = reg_val.bf.txbyte_l;
	return ret;
}

sw_error_t
hppe_txbyte_l_txbyte_l_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_txbyte_h_txbyte_h_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union txbyte_h_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_txbyte_h_get(dev_id, index, &reg_val);
	*value = reg_val.bf.txbyte_h;
	return ret;
}

sw_error_t
hppe_txbyte_h_txbyte_h_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_txcollisions_txcollisions_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union txcollisions_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_txcollisions_get(dev_id, index, &reg_val);
	*value = reg_val.bf.txcollisions;
	return ret;
}

sw_error_t
hppe_txcollisions_txcollisions_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_txabortcol_txabortcol_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union txabortcol_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_txabortcol_get(dev_id, index, &reg_val);
	*value = reg_val.bf.txabortcol;
	return ret;
}

sw_error_t
hppe_txabortcol_txabortcol_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_txmulticol_txmulticol_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union txmulticol_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_txmulticol_get(dev_id, index, &reg_val);
	*value = reg_val.bf.txmulticol;
	return ret;
}

sw_error_t
hppe_txmulticol_txmulticol_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_txsinglecol_txsinglecol_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union txsinglecol_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_txsinglecol_get(dev_id, index, &reg_val);
	*value = reg_val.bf.txsinglecol;
	return ret;
}

sw_error_t
hppe_txsinglecol_txsinglecol_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_txexcessivedefer_txexcessivedefer_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union txexcessivedefer_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_txexcessivedefer_get(dev_id, index, &reg_val);
	*value = reg_val.bf.txexcessivedefer;
	return ret;
}

sw_error_t
hppe_txexcessivedefer_txexcessivedefer_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_txdefer_txdefer_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union txdefer_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_txdefer_get(dev_id, index, &reg_val);
	*value = reg_val.bf.txdefer;
	return ret;
}

sw_error_t
hppe_txdefer_txdefer_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_txlatecol_txlatecol_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union txlatecol_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_txlatecol_get(dev_id, index, &reg_val);
	*value = reg_val.bf.txlatecol;
	return ret;
}

sw_error_t
hppe_txlatecol_txlatecol_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_txuni_txuni_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union txuni_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_txuni_get(dev_id, index, &reg_val);
	*value = reg_val.bf.txuni;
	return ret;
}

sw_error_t
hppe_txuni_txuni_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}


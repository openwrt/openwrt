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
#include "hppe_xgmacmib_reg.h"
#include "hppe_xgmacmib.h"


sw_error_t
hppe_mmc_control_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union mmc_control_u *value)
{
	if (index >= MMC_CONTROL_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				NSS_XGMAC_CSR_BASE_ADDR + MMC_CONTROL_ADDRESS + \
				index * MMC_CONTROL_INC,
				&value->val);
}

sw_error_t
hppe_mmc_control_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union mmc_control_u *value)
{
	return hppe_reg_set(
				dev_id,
				NSS_XGMAC_CSR_BASE_ADDR + MMC_CONTROL_ADDRESS + \
				index * MMC_CONTROL_INC,
				value->val);
}

sw_error_t
hppe_tx_octet_count_good_bad_low_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union tx_octet_count_good_bad_low_u *value)
{
	if (index >= TX_OCTET_COUNT_GOOD_BAD_LOW_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				NSS_XGMAC_CSR_BASE_ADDR + TX_OCTET_COUNT_GOOD_BAD_LOW_ADDRESS + \
				index * TX_OCTET_COUNT_GOOD_BAD_LOW_INC,
				&value->val);
}

sw_error_t
hppe_tx_octet_count_good_bad_low_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union tx_octet_count_good_bad_low_u *value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_tx_octet_count_good_bad_high_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union tx_octet_count_good_bad_high_u *value)
{
	if (index >= TX_OCTET_COUNT_GOOD_BAD_HIGH_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				NSS_XGMAC_CSR_BASE_ADDR + TX_OCTET_COUNT_GOOD_BAD_HIGH_ADDRESS + \
				index * TX_OCTET_COUNT_GOOD_BAD_HIGH_INC,
				&value->val);
}

sw_error_t
hppe_tx_octet_count_good_bad_high_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union tx_octet_count_good_bad_high_u *value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_tx_frame_count_good_bad_low_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union tx_frame_count_good_bad_low_u *value)
{
	if (index >= TX_FRAME_COUNT_GOOD_BAD_LOW_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				NSS_XGMAC_CSR_BASE_ADDR + TX_FRAME_COUNT_GOOD_BAD_LOW_ADDRESS + \
				index * TX_FRAME_COUNT_GOOD_BAD_LOW_INC,
				&value->val);
}

sw_error_t
hppe_tx_frame_count_good_bad_low_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union tx_frame_count_good_bad_low_u *value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_tx_frame_count_good_bad_high_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union tx_frame_count_good_bad_high_u *value)
{
	if (index >= TX_FRAME_COUNT_GOOD_BAD_HIGH_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				NSS_XGMAC_CSR_BASE_ADDR + TX_FRAME_COUNT_GOOD_BAD_HIGH_ADDRESS + \
				index * TX_FRAME_COUNT_GOOD_BAD_HIGH_INC,
				&value->val);
}

sw_error_t
hppe_tx_frame_count_good_bad_high_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union tx_frame_count_good_bad_high_u *value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_tx_broadcast_frames_good_low_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union tx_broadcast_frames_good_low_u *value)
{
	if (index >= TX_BROADCAST_FRAMES_GOOD_LOW_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				NSS_XGMAC_CSR_BASE_ADDR + TX_BROADCAST_FRAMES_GOOD_LOW_ADDRESS + \
				index * TX_BROADCAST_FRAMES_GOOD_LOW_INC,
				&value->val);
}

sw_error_t
hppe_tx_broadcast_frames_good_low_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union tx_broadcast_frames_good_low_u *value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_tx_broadcast_frames_good_high_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union tx_broadcast_frames_good_high_u *value)
{
	if (index >= TX_BROADCAST_FRAMES_GOOD_HIGH_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				NSS_XGMAC_CSR_BASE_ADDR + TX_BROADCAST_FRAMES_GOOD_HIGH_ADDRESS + \
				index * TX_BROADCAST_FRAMES_GOOD_HIGH_INC,
				&value->val);
}

sw_error_t
hppe_tx_broadcast_frames_good_high_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union tx_broadcast_frames_good_high_u *value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_tx_multicast_frames_good_low_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union tx_multicast_frames_good_low_u *value)
{
	if (index >= TX_MULTICAST_FRAMES_GOOD_LOW_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				NSS_XGMAC_CSR_BASE_ADDR + TX_MULTICAST_FRAMES_GOOD_LOW_ADDRESS + \
				index * TX_MULTICAST_FRAMES_GOOD_LOW_INC,
				&value->val);
}

sw_error_t
hppe_tx_multicast_frames_good_low_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union tx_multicast_frames_good_low_u *value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_tx_multicast_frames_good_high_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union tx_multicast_frames_good_high_u *value)
{
	if (index >= TX_MULTICAST_FRAMES_GOOD_HIGH_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				NSS_XGMAC_CSR_BASE_ADDR + TX_MULTICAST_FRAMES_GOOD_HIGH_ADDRESS + \
				index * TX_MULTICAST_FRAMES_GOOD_HIGH_INC,
				&value->val);
}

sw_error_t
hppe_tx_multicast_frames_good_high_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union tx_multicast_frames_good_high_u *value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_tx_64octets_frames_good_bad_low_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union tx_64octets_frames_good_bad_low_u *value)
{
	if (index >= TX_64OCTETS_FRAMES_GOOD_BAD_LOW_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				NSS_XGMAC_CSR_BASE_ADDR + TX_64OCTETS_FRAMES_GOOD_BAD_LOW_ADDRESS + \
				index * TX_64OCTETS_FRAMES_GOOD_BAD_LOW_INC,
				&value->val);
}

sw_error_t
hppe_tx_64octets_frames_good_bad_low_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union tx_64octets_frames_good_bad_low_u *value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_tx_64octets_frames_good_bad_high_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union tx_64octets_frames_good_bad_high_u *value)
{
	if (index >= TX_64OCTETS_FRAMES_GOOD_BAD_HIGH_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				NSS_XGMAC_CSR_BASE_ADDR + TX_64OCTETS_FRAMES_GOOD_BAD_HIGH_ADDRESS + \
				index * TX_64OCTETS_FRAMES_GOOD_BAD_HIGH_INC,
				&value->val);
}

sw_error_t
hppe_tx_64octets_frames_good_bad_high_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union tx_64octets_frames_good_bad_high_u *value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_tx_65to127octets_frames_good_bad_low_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union tx_65to127octets_frames_good_bad_low_u *value)
{
	if (index >= TX_65TO127OCTETS_FRAMES_GOOD_BAD_LOW_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				NSS_XGMAC_CSR_BASE_ADDR + TX_65TO127OCTETS_FRAMES_GOOD_BAD_LOW_ADDRESS + \
				index * TX_65TO127OCTETS_FRAMES_GOOD_BAD_LOW_INC,
				&value->val);
}

sw_error_t
hppe_tx_65to127octets_frames_good_bad_low_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union tx_65to127octets_frames_good_bad_low_u *value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_tx_65to127octets_frames_good_bad_high_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union tx_65to127octets_frames_good_bad_high_u *value)
{
	if (index >= TX_65TO127OCTETS_FRAMES_GOOD_BAD_HIGH_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				NSS_XGMAC_CSR_BASE_ADDR + TX_65TO127OCTETS_FRAMES_GOOD_BAD_HIGH_ADDRESS + \
				index * TX_65TO127OCTETS_FRAMES_GOOD_BAD_HIGH_INC,
				&value->val);
}

sw_error_t
hppe_tx_65to127octets_frames_good_bad_high_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union tx_65to127octets_frames_good_bad_high_u *value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_tx_128to255octets_frames_good_bad_low_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union tx_128to255octets_frames_good_bad_low_u *value)
{
	if (index >= TX_128TO255OCTETS_FRAMES_GOOD_BAD_LOW_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				NSS_XGMAC_CSR_BASE_ADDR + TX_128TO255OCTETS_FRAMES_GOOD_BAD_LOW_ADDRESS + \
				index * TX_128TO255OCTETS_FRAMES_GOOD_BAD_LOW_INC,
				&value->val);
}

sw_error_t
hppe_tx_128to255octets_frames_good_bad_low_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union tx_128to255octets_frames_good_bad_low_u *value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_tx_128to255octets_frames_good_bad_high_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union tx_128to255octets_frames_good_bad_high_u *value)
{
	if (index >= TX_128TO255OCTETS_FRAMES_GOOD_BAD_HIGH_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				NSS_XGMAC_CSR_BASE_ADDR + TX_128TO255OCTETS_FRAMES_GOOD_BAD_HIGH_ADDRESS + \
				index * TX_128TO255OCTETS_FRAMES_GOOD_BAD_HIGH_INC,
				&value->val);
}

sw_error_t
hppe_tx_128to255octets_frames_good_bad_high_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union tx_128to255octets_frames_good_bad_high_u *value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_tx_256to511octets_frames_good_bad_low_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union tx_256to511octets_frames_good_bad_low_u *value)
{
	if (index >= TX_256TO511OCTETS_FRAMES_GOOD_BAD_LOW_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				NSS_XGMAC_CSR_BASE_ADDR + TX_256TO511OCTETS_FRAMES_GOOD_BAD_LOW_ADDRESS + \
				index * TX_256TO511OCTETS_FRAMES_GOOD_BAD_LOW_INC,
				&value->val);
}

sw_error_t
hppe_tx_256to511octets_frames_good_bad_low_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union tx_256to511octets_frames_good_bad_low_u *value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_tx_256to511octets_frames_good_bad_high_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union tx_256to511octets_frames_good_bad_high_u *value)
{
	if (index >= TX_256TO511OCTETS_FRAMES_GOOD_BAD_HIGH_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				NSS_XGMAC_CSR_BASE_ADDR + TX_256TO511OCTETS_FRAMES_GOOD_BAD_HIGH_ADDRESS + \
				index * TX_256TO511OCTETS_FRAMES_GOOD_BAD_HIGH_INC,
				&value->val);
}

sw_error_t
hppe_tx_256to511octets_frames_good_bad_high_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union tx_256to511octets_frames_good_bad_high_u *value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_tx_512to1023octets_frames_good_bad_low_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union tx_512to1023octets_frames_good_bad_low_u *value)
{
	if (index >= TX_512TO1023OCTETS_FRAMES_GOOD_BAD_LOW_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				NSS_XGMAC_CSR_BASE_ADDR + TX_512TO1023OCTETS_FRAMES_GOOD_BAD_LOW_ADDRESS + \
				index * TX_512TO1023OCTETS_FRAMES_GOOD_BAD_LOW_INC,
				&value->val);
}

sw_error_t
hppe_tx_512to1023octets_frames_good_bad_low_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union tx_512to1023octets_frames_good_bad_low_u *value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_tx_512to1023octets_frames_good_bad_high_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union tx_512to1023octets_frames_good_bad_high_u *value)
{
	if (index >= TX_512TO1023OCTETS_FRAMES_GOOD_BAD_HIGH_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				NSS_XGMAC_CSR_BASE_ADDR + TX_512TO1023OCTETS_FRAMES_GOOD_BAD_HIGH_ADDRESS + \
				index * TX_512TO1023OCTETS_FRAMES_GOOD_BAD_HIGH_INC,
				&value->val);
}

sw_error_t
hppe_tx_512to1023octets_frames_good_bad_high_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union tx_512to1023octets_frames_good_bad_high_u *value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_tx_1024tomaxoctets_frames_good_bad_low_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union tx_1024tomaxoctets_frames_good_bad_low_u *value)
{
	if (index >= TX_1024TOMAXOCTETS_FRAMES_GOOD_BAD_LOW_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				NSS_XGMAC_CSR_BASE_ADDR + TX_1024TOMAXOCTETS_FRAMES_GOOD_BAD_LOW_ADDRESS + \
				index * TX_1024TOMAXOCTETS_FRAMES_GOOD_BAD_LOW_INC,
				&value->val);
}

sw_error_t
hppe_tx_1024tomaxoctets_frames_good_bad_low_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union tx_1024tomaxoctets_frames_good_bad_low_u *value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_tx_1024tomaxoctets_frames_good_bad_high_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union tx_1024tomaxoctets_frames_good_bad_high_u *value)
{
	if (index >= TX_1024TOMAXOCTETS_FRAMES_GOOD_BAD_HIGH_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				NSS_XGMAC_CSR_BASE_ADDR + TX_1024TOMAXOCTETS_FRAMES_GOOD_BAD_HIGH_ADDRESS + \
				index * TX_1024TOMAXOCTETS_FRAMES_GOOD_BAD_HIGH_INC,
				&value->val);
}

sw_error_t
hppe_tx_1024tomaxoctets_frames_good_bad_high_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union tx_1024tomaxoctets_frames_good_bad_high_u *value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_tx_unicast_frames_good_bad_low_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union tx_unicast_frames_good_bad_low_u *value)
{
	if (index >= TX_UNICAST_FRAMES_GOOD_BAD_LOW_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				NSS_XGMAC_CSR_BASE_ADDR + TX_UNICAST_FRAMES_GOOD_BAD_LOW_ADDRESS + \
				index * TX_UNICAST_FRAMES_GOOD_BAD_LOW_INC,
				&value->val);
}

sw_error_t
hppe_tx_unicast_frames_good_bad_low_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union tx_unicast_frames_good_bad_low_u *value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_tx_unicast_frames_good_bad_high_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union tx_unicast_frames_good_bad_high_u *value)
{
	if (index >= TX_UNICAST_FRAMES_GOOD_BAD_HIGH_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				NSS_XGMAC_CSR_BASE_ADDR + TX_UNICAST_FRAMES_GOOD_BAD_HIGH_ADDRESS + \
				index * TX_UNICAST_FRAMES_GOOD_BAD_HIGH_INC,
				&value->val);
}

sw_error_t
hppe_tx_unicast_frames_good_bad_high_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union tx_unicast_frames_good_bad_high_u *value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_tx_multicast_frames_good_bad_low_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union tx_multicast_frames_good_bad_low_u *value)
{
	if (index >= TX_MULTICAST_FRAMES_GOOD_BAD_LOW_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				NSS_XGMAC_CSR_BASE_ADDR + TX_MULTICAST_FRAMES_GOOD_BAD_LOW_ADDRESS + \
				index * TX_MULTICAST_FRAMES_GOOD_BAD_LOW_INC,
				&value->val);
}

sw_error_t
hppe_tx_multicast_frames_good_bad_low_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union tx_multicast_frames_good_bad_low_u *value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_tx_multicast_frames_good_bad_high_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union tx_multicast_frames_good_bad_high_u *value)
{
	if (index >= TX_MULTICAST_FRAMES_GOOD_BAD_HIGH_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				NSS_XGMAC_CSR_BASE_ADDR + TX_MULTICAST_FRAMES_GOOD_BAD_HIGH_ADDRESS + \
				index * TX_MULTICAST_FRAMES_GOOD_BAD_HIGH_INC,
				&value->val);
}

sw_error_t
hppe_tx_multicast_frames_good_bad_high_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union tx_multicast_frames_good_bad_high_u *value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_tx_broadcast_frames_good_bad_low_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union tx_broadcast_frames_good_bad_low_u *value)
{
	if (index >= TX_BROADCAST_FRAMES_GOOD_BAD_LOW_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				NSS_XGMAC_CSR_BASE_ADDR + TX_BROADCAST_FRAMES_GOOD_BAD_LOW_ADDRESS + \
				index * TX_BROADCAST_FRAMES_GOOD_BAD_LOW_INC,
				&value->val);
}

sw_error_t
hppe_tx_broadcast_frames_good_bad_low_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union tx_broadcast_frames_good_bad_low_u *value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_tx_broadcast_frames_good_bad_high_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union tx_broadcast_frames_good_bad_high_u *value)
{
	if (index >= TX_BROADCAST_FRAMES_GOOD_BAD_HIGH_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				NSS_XGMAC_CSR_BASE_ADDR + TX_BROADCAST_FRAMES_GOOD_BAD_HIGH_ADDRESS + \
				index * TX_BROADCAST_FRAMES_GOOD_BAD_HIGH_INC,
				&value->val);
}

sw_error_t
hppe_tx_broadcast_frames_good_bad_high_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union tx_broadcast_frames_good_bad_high_u *value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_tx_underflow_error_frames_low_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union tx_underflow_error_frames_low_u *value)
{
	if (index >= TX_UNDERFLOW_ERROR_FRAMES_LOW_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				NSS_XGMAC_CSR_BASE_ADDR + TX_UNDERFLOW_ERROR_FRAMES_LOW_ADDRESS + \
				index * TX_UNDERFLOW_ERROR_FRAMES_LOW_INC,
				&value->val);
}

sw_error_t
hppe_tx_underflow_error_frames_low_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union tx_underflow_error_frames_low_u *value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_tx_underflow_error_frames_high_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union tx_underflow_error_frames_high_u *value)
{
	if (index >= TX_UNDERFLOW_ERROR_FRAMES_HIGH_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				NSS_XGMAC_CSR_BASE_ADDR + TX_UNDERFLOW_ERROR_FRAMES_HIGH_ADDRESS + \
				index * TX_UNDERFLOW_ERROR_FRAMES_HIGH_INC,
				&value->val);
}

sw_error_t
hppe_tx_underflow_error_frames_high_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union tx_underflow_error_frames_high_u *value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_tx_octet_count_good_low_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union tx_octet_count_good_low_u *value)
{
	if (index >= TX_OCTET_COUNT_GOOD_LOW_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				NSS_XGMAC_CSR_BASE_ADDR + TX_OCTET_COUNT_GOOD_LOW_ADDRESS + \
				index * TX_OCTET_COUNT_GOOD_LOW_INC,
				&value->val);
}

sw_error_t
hppe_tx_octet_count_good_low_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union tx_octet_count_good_low_u *value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_tx_octet_count_good_high_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union tx_octet_count_good_high_u *value)
{
	if (index >= TX_OCTET_COUNT_GOOD_HIGH_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				NSS_XGMAC_CSR_BASE_ADDR + TX_OCTET_COUNT_GOOD_HIGH_ADDRESS + \
				index * TX_OCTET_COUNT_GOOD_HIGH_INC,
				&value->val);
}

sw_error_t
hppe_tx_octet_count_good_high_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union tx_octet_count_good_high_u *value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_tx_frame_count_good_low_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union tx_frame_count_good_low_u *value)
{
	if (index >= TX_FRAME_COUNT_GOOD_LOW_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				NSS_XGMAC_CSR_BASE_ADDR + TX_FRAME_COUNT_GOOD_LOW_ADDRESS + \
				index * TX_FRAME_COUNT_GOOD_LOW_INC,
				&value->val);
}

sw_error_t
hppe_tx_frame_count_good_low_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union tx_frame_count_good_low_u *value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_tx_frame_count_good_high_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union tx_frame_count_good_high_u *value)
{
	if (index >= TX_FRAME_COUNT_GOOD_HIGH_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				NSS_XGMAC_CSR_BASE_ADDR + TX_FRAME_COUNT_GOOD_HIGH_ADDRESS + \
				index * TX_FRAME_COUNT_GOOD_HIGH_INC,
				&value->val);
}

sw_error_t
hppe_tx_frame_count_good_high_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union tx_frame_count_good_high_u *value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_tx_pause_frames_low_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union tx_pause_frames_low_u *value)
{
	if (index >= TX_PAUSE_FRAMES_LOW_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				NSS_XGMAC_CSR_BASE_ADDR + TX_PAUSE_FRAMES_LOW_ADDRESS + \
				index * TX_PAUSE_FRAMES_LOW_INC,
				&value->val);
}

sw_error_t
hppe_tx_pause_frames_low_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union tx_pause_frames_low_u *value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_tx_pause_frames_high_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union tx_pause_frames_high_u *value)
{
	if (index >= TX_PAUSE_FRAMES_HIGH_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				NSS_XGMAC_CSR_BASE_ADDR + TX_PAUSE_FRAMES_HIGH_ADDRESS + \
				index * TX_PAUSE_FRAMES_HIGH_INC,
				&value->val);
}

sw_error_t
hppe_tx_pause_frames_high_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union tx_pause_frames_high_u *value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_tx_vlan_frames_good_low_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union tx_vlan_frames_good_low_u *value)
{
	if (index >= TX_VLAN_FRAMES_GOOD_LOW_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				NSS_XGMAC_CSR_BASE_ADDR + TX_VLAN_FRAMES_GOOD_LOW_ADDRESS + \
				index * TX_VLAN_FRAMES_GOOD_LOW_INC,
				&value->val);
}

sw_error_t
hppe_tx_vlan_frames_good_low_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union tx_vlan_frames_good_low_u *value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_tx_vlan_frames_good_high_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union tx_vlan_frames_good_high_u *value)
{
	if (index >= TX_VLAN_FRAMES_GOOD_HIGH_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				NSS_XGMAC_CSR_BASE_ADDR + TX_VLAN_FRAMES_GOOD_HIGH_ADDRESS + \
				index * TX_VLAN_FRAMES_GOOD_HIGH_INC,
				&value->val);
}

sw_error_t
hppe_tx_vlan_frames_good_high_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union tx_vlan_frames_good_high_u *value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_tx_lpi_usec_cntr_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union tx_lpi_usec_cntr_u *value)
{
	if (index >= TX_LPI_USEC_CNTR_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				NSS_XGMAC_CSR_BASE_ADDR + TX_LPI_USEC_CNTR_ADDRESS + \
				index * TX_LPI_USEC_CNTR_INC,
				&value->val);
}

sw_error_t
hppe_tx_lpi_usec_cntr_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union tx_lpi_usec_cntr_u *value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_tx_lpi_tran_cntr_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union tx_lpi_tran_cntr_u *value)
{
	if (index >= TX_LPI_TRAN_CNTR_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				NSS_XGMAC_CSR_BASE_ADDR + TX_LPI_TRAN_CNTR_ADDRESS + \
				index * TX_LPI_TRAN_CNTR_INC,
				&value->val);
}

sw_error_t
hppe_tx_lpi_tran_cntr_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union tx_lpi_tran_cntr_u *value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_rx_frame_count_good_bad_low_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rx_frame_count_good_bad_low_u *value)
{
	if (index >= RX_FRAME_COUNT_GOOD_BAD_LOW_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				NSS_XGMAC_CSR_BASE_ADDR + RX_FRAME_COUNT_GOOD_BAD_LOW_ADDRESS + \
				index * RX_FRAME_COUNT_GOOD_BAD_LOW_INC,
				&value->val);
}

sw_error_t
hppe_rx_frame_count_good_bad_low_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rx_frame_count_good_bad_low_u *value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_rx_frame_count_good_bad_high_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rx_frame_count_good_bad_high_u *value)
{
	if (index >= RX_FRAME_COUNT_GOOD_BAD_HIGH_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				NSS_XGMAC_CSR_BASE_ADDR + RX_FRAME_COUNT_GOOD_BAD_HIGH_ADDRESS + \
				index * RX_FRAME_COUNT_GOOD_BAD_HIGH_INC,
				&value->val);
}

sw_error_t
hppe_rx_frame_count_good_bad_high_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rx_frame_count_good_bad_high_u *value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_rx_octet_count_good_bad_low_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rx_octet_count_good_bad_low_u *value)
{
	if (index >= RX_OCTET_COUNT_GOOD_BAD_LOW_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				NSS_XGMAC_CSR_BASE_ADDR + RX_OCTET_COUNT_GOOD_BAD_LOW_ADDRESS + \
				index * RX_OCTET_COUNT_GOOD_BAD_LOW_INC,
				&value->val);
}

sw_error_t
hppe_rx_octet_count_good_bad_low_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rx_octet_count_good_bad_low_u *value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_rx_octet_count_good_bad_high_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rx_octet_count_good_bad_high_u *value)
{
	if (index >= RX_OCTET_COUNT_GOOD_BAD_HIGH_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				NSS_XGMAC_CSR_BASE_ADDR + RX_OCTET_COUNT_GOOD_BAD_HIGH_ADDRESS + \
				index * RX_OCTET_COUNT_GOOD_BAD_HIGH_INC,
				&value->val);
}

sw_error_t
hppe_rx_octet_count_good_bad_high_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rx_octet_count_good_bad_high_u *value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_rx_octet_count_good_low_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rx_octet_count_good_low_u *value)
{
	if (index >= RX_OCTET_COUNT_GOOD_LOW_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				NSS_XGMAC_CSR_BASE_ADDR + RX_OCTET_COUNT_GOOD_LOW_ADDRESS + \
				index * RX_OCTET_COUNT_GOOD_LOW_INC,
				&value->val);
}

sw_error_t
hppe_rx_octet_count_good_low_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rx_octet_count_good_low_u *value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_rx_octet_count_good_high_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rx_octet_count_good_high_u *value)
{
	if (index >= RX_OCTET_COUNT_GOOD_HIGH_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				NSS_XGMAC_CSR_BASE_ADDR + RX_OCTET_COUNT_GOOD_HIGH_ADDRESS + \
				index * RX_OCTET_COUNT_GOOD_HIGH_INC,
				&value->val);
}

sw_error_t
hppe_rx_octet_count_good_high_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rx_octet_count_good_high_u *value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_rx_broadcast_frames_good_low_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rx_broadcast_frames_good_low_u *value)
{
	if (index >= RX_BROADCAST_FRAMES_GOOD_LOW_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				NSS_XGMAC_CSR_BASE_ADDR + RX_BROADCAST_FRAMES_GOOD_LOW_ADDRESS + \
				index * RX_BROADCAST_FRAMES_GOOD_LOW_INC,
				&value->val);
}

sw_error_t
hppe_rx_broadcast_frames_good_low_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rx_broadcast_frames_good_low_u *value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_rx_broadcast_frames_good_high_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rx_broadcast_frames_good_high_u *value)
{
	if (index >= RX_BROADCAST_FRAMES_GOOD_HIGH_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				NSS_XGMAC_CSR_BASE_ADDR + RX_BROADCAST_FRAMES_GOOD_HIGH_ADDRESS + \
				index * RX_BROADCAST_FRAMES_GOOD_HIGH_INC,
				&value->val);
}

sw_error_t
hppe_rx_broadcast_frames_good_high_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rx_broadcast_frames_good_high_u *value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_rx_multicast_frames_good_low_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rx_multicast_frames_good_low_u *value)
{
	if (index >= RX_MULTICAST_FRAMES_GOOD_LOW_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				NSS_XGMAC_CSR_BASE_ADDR + RX_MULTICAST_FRAMES_GOOD_LOW_ADDRESS + \
				index * RX_MULTICAST_FRAMES_GOOD_LOW_INC,
				&value->val);
}

sw_error_t
hppe_rx_multicast_frames_good_low_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rx_multicast_frames_good_low_u *value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_rx_multicast_frames_good_high_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rx_multicast_frames_good_high_u *value)
{
	if (index >= RX_MULTICAST_FRAMES_GOOD_HIGH_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				NSS_XGMAC_CSR_BASE_ADDR + RX_MULTICAST_FRAMES_GOOD_HIGH_ADDRESS + \
				index * RX_MULTICAST_FRAMES_GOOD_HIGH_INC,
				&value->val);
}

sw_error_t
hppe_rx_multicast_frames_good_high_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rx_multicast_frames_good_high_u *value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_rx_crc_error_frames_low_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rx_crc_error_frames_low_u *value)
{
	if (index >= RX_CRC_ERROR_FRAMES_LOW_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				NSS_XGMAC_CSR_BASE_ADDR + RX_CRC_ERROR_FRAMES_LOW_ADDRESS + \
				index * RX_CRC_ERROR_FRAMES_LOW_INC,
				&value->val);
}

sw_error_t
hppe_rx_crc_error_frames_low_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rx_crc_error_frames_low_u *value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_rx_crc_error_frames_high_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rx_crc_error_frames_high_u *value)
{
	if (index >= RX_CRC_ERROR_FRAMES_HIGH_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				NSS_XGMAC_CSR_BASE_ADDR + RX_CRC_ERROR_FRAMES_HIGH_ADDRESS + \
				index * RX_CRC_ERROR_FRAMES_HIGH_INC,
				&value->val);
}

sw_error_t
hppe_rx_crc_error_frames_high_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rx_crc_error_frames_high_u *value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_rx_runt_error_frames_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rx_runt_error_frames_u *value)
{
	if (index >= RX_RUNT_ERROR_FRAMES_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				NSS_XGMAC_CSR_BASE_ADDR + RX_RUNT_ERROR_FRAMES_ADDRESS + \
				index * RX_RUNT_ERROR_FRAMES_INC,
				&value->val);
}

sw_error_t
hppe_rx_runt_error_frames_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rx_runt_error_frames_u *value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_rx_jabber_error_frames_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rx_jabber_error_frames_u *value)
{
	if (index >= RX_JABBER_ERROR_FRAMES_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				NSS_XGMAC_CSR_BASE_ADDR + RX_JABBER_ERROR_FRAMES_ADDRESS + \
				index * RX_JABBER_ERROR_FRAMES_INC,
				&value->val);
}

sw_error_t
hppe_rx_jabber_error_frames_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rx_jabber_error_frames_u *value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_rx_undersize_frames_good_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rx_undersize_frames_good_u *value)
{
	if (index >= RX_UNDERSIZE_FRAMES_GOOD_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				NSS_XGMAC_CSR_BASE_ADDR + RX_UNDERSIZE_FRAMES_GOOD_ADDRESS + \
				index * RX_UNDERSIZE_FRAMES_GOOD_INC,
				&value->val);
}

sw_error_t
hppe_rx_undersize_frames_good_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rx_undersize_frames_good_u *value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_rx_oversize_frames_good_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rx_oversize_frames_good_u *value)
{
	if (index >= RX_OVERSIZE_FRAMES_GOOD_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				NSS_XGMAC_CSR_BASE_ADDR + RX_OVERSIZE_FRAMES_GOOD_ADDRESS + \
				index * RX_OVERSIZE_FRAMES_GOOD_INC,
				&value->val);
}

sw_error_t
hppe_rx_oversize_frames_good_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rx_oversize_frames_good_u *value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_rx_64octets_frames_good_bad_low_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rx_64octets_frames_good_bad_low_u *value)
{
	if (index >= RX_64OCTETS_FRAMES_GOOD_BAD_LOW_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				NSS_XGMAC_CSR_BASE_ADDR + RX_64OCTETS_FRAMES_GOOD_BAD_LOW_ADDRESS + \
				index * RX_64OCTETS_FRAMES_GOOD_BAD_LOW_INC,
				&value->val);
}

sw_error_t
hppe_rx_64octets_frames_good_bad_low_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rx_64octets_frames_good_bad_low_u *value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_rx_64octets_frames_good_bad_high_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rx_64octets_frames_good_bad_high_u *value)
{
	if (index >= RX_64OCTETS_FRAMES_GOOD_BAD_HIGH_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				NSS_XGMAC_CSR_BASE_ADDR + RX_64OCTETS_FRAMES_GOOD_BAD_HIGH_ADDRESS + \
				index * RX_64OCTETS_FRAMES_GOOD_BAD_HIGH_INC,
				&value->val);
}

sw_error_t
hppe_rx_64octets_frames_good_bad_high_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rx_64octets_frames_good_bad_high_u *value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_rx_65to127octets_frames_good_bad_low_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rx_65to127octets_frames_good_bad_low_u *value)
{
	if (index >= RX_65TO127OCTETS_FRAMES_GOOD_BAD_LOW_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				NSS_XGMAC_CSR_BASE_ADDR + RX_65TO127OCTETS_FRAMES_GOOD_BAD_LOW_ADDRESS + \
				index * RX_65TO127OCTETS_FRAMES_GOOD_BAD_LOW_INC,
				&value->val);
}

sw_error_t
hppe_rx_65to127octets_frames_good_bad_low_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rx_65to127octets_frames_good_bad_low_u *value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_rx_65to127octets_frames_good_bad_high_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rx_65to127octets_frames_good_bad_high_u *value)
{
	if (index >= RX_65TO127OCTETS_FRAMES_GOOD_BAD_HIGH_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				NSS_XGMAC_CSR_BASE_ADDR + RX_65TO127OCTETS_FRAMES_GOOD_BAD_HIGH_ADDRESS + \
				index * RX_65TO127OCTETS_FRAMES_GOOD_BAD_HIGH_INC,
				&value->val);
}

sw_error_t
hppe_rx_65to127octets_frames_good_bad_high_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rx_65to127octets_frames_good_bad_high_u *value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_rx_128to255octets_frames_good_bad_low_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rx_128to255octets_frames_good_bad_low_u *value)
{
	if (index >= RX_128TO255OCTETS_FRAMES_GOOD_BAD_LOW_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				NSS_XGMAC_CSR_BASE_ADDR + RX_128TO255OCTETS_FRAMES_GOOD_BAD_LOW_ADDRESS + \
				index * RX_128TO255OCTETS_FRAMES_GOOD_BAD_LOW_INC,
				&value->val);
}

sw_error_t
hppe_rx_128to255octets_frames_good_bad_low_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rx_128to255octets_frames_good_bad_low_u *value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_rx_128to255octets_frames_good_bad_high_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rx_128to255octets_frames_good_bad_high_u *value)
{
	if (index >= RX_128TO255OCTETS_FRAMES_GOOD_BAD_HIGH_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				NSS_XGMAC_CSR_BASE_ADDR + RX_128TO255OCTETS_FRAMES_GOOD_BAD_HIGH_ADDRESS + \
				index * RX_128TO255OCTETS_FRAMES_GOOD_BAD_HIGH_INC,
				&value->val);
}

sw_error_t
hppe_rx_128to255octets_frames_good_bad_high_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rx_128to255octets_frames_good_bad_high_u *value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_rx_256to511octets_frames_good_bad_low_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rx_256to511octets_frames_good_bad_low_u *value)
{
	if (index >= RX_256TO511OCTETS_FRAMES_GOOD_BAD_LOW_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				NSS_XGMAC_CSR_BASE_ADDR + RX_256TO511OCTETS_FRAMES_GOOD_BAD_LOW_ADDRESS + \
				index * RX_256TO511OCTETS_FRAMES_GOOD_BAD_LOW_INC,
				&value->val);
}

sw_error_t
hppe_rx_256to511octets_frames_good_bad_low_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rx_256to511octets_frames_good_bad_low_u *value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_rx_256to511octets_frames_good_bad_high_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rx_256to511octets_frames_good_bad_high_u *value)
{
	if (index >= RX_256TO511OCTETS_FRAMES_GOOD_BAD_HIGH_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				NSS_XGMAC_CSR_BASE_ADDR + RX_256TO511OCTETS_FRAMES_GOOD_BAD_HIGH_ADDRESS + \
				index * RX_256TO511OCTETS_FRAMES_GOOD_BAD_HIGH_INC,
				&value->val);
}

sw_error_t
hppe_rx_256to511octets_frames_good_bad_high_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rx_256to511octets_frames_good_bad_high_u *value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_rx_512to1023octets_frames_good_bad_low_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rx_512to1023octets_frames_good_bad_low_u *value)
{
	if (index >= RX_512TO1023OCTETS_FRAMES_GOOD_BAD_LOW_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				NSS_XGMAC_CSR_BASE_ADDR + RX_512TO1023OCTETS_FRAMES_GOOD_BAD_LOW_ADDRESS + \
				index * RX_512TO1023OCTETS_FRAMES_GOOD_BAD_LOW_INC,
				&value->val);
}

sw_error_t
hppe_rx_512to1023octets_frames_good_bad_low_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rx_512to1023octets_frames_good_bad_low_u *value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_rx_512to1023octets_frames_good_bad_high_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rx_512to1023octets_frames_good_bad_high_u *value)
{
	if (index >= RX_512TO1023OCTETS_FRAMES_GOOD_BAD_HIGH_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				NSS_XGMAC_CSR_BASE_ADDR + RX_512TO1023OCTETS_FRAMES_GOOD_BAD_HIGH_ADDRESS + \
				index * RX_512TO1023OCTETS_FRAMES_GOOD_BAD_HIGH_INC,
				&value->val);
}

sw_error_t
hppe_rx_512to1023octets_frames_good_bad_high_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rx_512to1023octets_frames_good_bad_high_u *value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_rx_1024tomaxoctets_frames_good_bad_low_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rx_1024tomaxoctets_frames_good_bad_low_u *value)
{
	if (index >= RX_1024TOMAXOCTETS_FRAMES_GOOD_BAD_LOW_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				NSS_XGMAC_CSR_BASE_ADDR + RX_1024TOMAXOCTETS_FRAMES_GOOD_BAD_LOW_ADDRESS + \
				index * RX_1024TOMAXOCTETS_FRAMES_GOOD_BAD_LOW_INC,
				&value->val);
}

sw_error_t
hppe_rx_1024tomaxoctets_frames_good_bad_low_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rx_1024tomaxoctets_frames_good_bad_low_u *value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_rx_1024tomaxoctets_frames_good_bad_high_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rx_1024tomaxoctets_frames_good_bad_high_u *value)
{
	if (index >= RX_1024TOMAXOCTETS_FRAMES_GOOD_BAD_HIGH_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				NSS_XGMAC_CSR_BASE_ADDR + RX_1024TOMAXOCTETS_FRAMES_GOOD_BAD_HIGH_ADDRESS + \
				index * RX_1024TOMAXOCTETS_FRAMES_GOOD_BAD_HIGH_INC,
				&value->val);
}

sw_error_t
hppe_rx_1024tomaxoctets_frames_good_bad_high_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rx_1024tomaxoctets_frames_good_bad_high_u *value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_rx_unicast_frames_good_low_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rx_unicast_frames_good_low_u *value)
{
	if (index >= RX_UNICAST_FRAMES_GOOD_LOW_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				NSS_XGMAC_CSR_BASE_ADDR + RX_UNICAST_FRAMES_GOOD_LOW_ADDRESS + \
				index * RX_UNICAST_FRAMES_GOOD_LOW_INC,
				&value->val);
}

sw_error_t
hppe_rx_unicast_frames_good_low_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rx_unicast_frames_good_low_u *value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_rx_unicast_frames_good_high_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rx_unicast_frames_good_high_u *value)
{
	if (index >= RX_UNICAST_FRAMES_GOOD_HIGH_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				NSS_XGMAC_CSR_BASE_ADDR + RX_UNICAST_FRAMES_GOOD_HIGH_ADDRESS + \
				index * RX_UNICAST_FRAMES_GOOD_HIGH_INC,
				&value->val);
}

sw_error_t
hppe_rx_unicast_frames_good_high_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rx_unicast_frames_good_high_u *value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_rx_length_error_frames_low_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rx_length_error_frames_low_u *value)
{
	if (index >= RX_LENGTH_ERROR_FRAMES_LOW_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				NSS_XGMAC_CSR_BASE_ADDR + RX_LENGTH_ERROR_FRAMES_LOW_ADDRESS + \
				index * RX_LENGTH_ERROR_FRAMES_LOW_INC,
				&value->val);
}

sw_error_t
hppe_rx_length_error_frames_low_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rx_length_error_frames_low_u *value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_rx_length_error_frames_high_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rx_length_error_frames_high_u *value)
{
	if (index >= RX_LENGTH_ERROR_FRAMES_HIGH_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				NSS_XGMAC_CSR_BASE_ADDR + RX_LENGTH_ERROR_FRAMES_HIGH_ADDRESS + \
				index * RX_LENGTH_ERROR_FRAMES_HIGH_INC,
				&value->val);
}

sw_error_t
hppe_rx_length_error_frames_high_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rx_length_error_frames_high_u *value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_rx_outofrange_frames_low_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rx_outofrange_frames_low_u *value)
{
	if (index >= RX_OUTOFRANGE_FRAMES_LOW_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				NSS_XGMAC_CSR_BASE_ADDR + RX_OUTOFRANGE_FRAMES_LOW_ADDRESS + \
				index * RX_OUTOFRANGE_FRAMES_LOW_INC,
				&value->val);
}

sw_error_t
hppe_rx_outofrange_frames_low_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rx_outofrange_frames_low_u *value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_rx_outofrange_frames_high_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rx_outofrange_frames_high_u *value)
{
	if (index >= RX_OUTOFRANGE_FRAMES_HIGH_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				NSS_XGMAC_CSR_BASE_ADDR + RX_OUTOFRANGE_FRAMES_HIGH_ADDRESS + \
				index * RX_OUTOFRANGE_FRAMES_HIGH_INC,
				&value->val);
}

sw_error_t
hppe_rx_outofrange_frames_high_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rx_outofrange_frames_high_u *value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_rx_pause_frames_low_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rx_pause_frames_low_u *value)
{
	if (index >= RX_PAUSE_FRAMES_LOW_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				NSS_XGMAC_CSR_BASE_ADDR + RX_PAUSE_FRAMES_LOW_ADDRESS + \
				index * RX_PAUSE_FRAMES_LOW_INC,
				&value->val);
}

sw_error_t
hppe_rx_pause_frames_low_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rx_pause_frames_low_u *value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_rx_pause_frames_high_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rx_pause_frames_high_u *value)
{
	if (index >= RX_PAUSE_FRAMES_HIGH_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				NSS_XGMAC_CSR_BASE_ADDR + RX_PAUSE_FRAMES_HIGH_ADDRESS + \
				index * RX_PAUSE_FRAMES_HIGH_INC,
				&value->val);
}

sw_error_t
hppe_rx_pause_frames_high_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rx_pause_frames_high_u *value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_rx_fifooverflow_frames_low_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rx_fifooverflow_frames_low_u *value)
{
	if (index >= RX_FIFOOVERFLOW_FRAMES_LOW_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				NSS_XGMAC_CSR_BASE_ADDR + RX_FIFOOVERFLOW_FRAMES_LOW_ADDRESS + \
				index * RX_FIFOOVERFLOW_FRAMES_LOW_INC,
				&value->val);
}

sw_error_t
hppe_rx_fifooverflow_frames_low_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rx_fifooverflow_frames_low_u *value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_rx_fifooverflow_frames_high_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rx_fifooverflow_frames_high_u *value)
{
	if (index >= RX_FIFOOVERFLOW_FRAMES_HIGH_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				NSS_XGMAC_CSR_BASE_ADDR + RX_FIFOOVERFLOW_FRAMES_HIGH_ADDRESS + \
				index * RX_FIFOOVERFLOW_FRAMES_HIGH_INC,
				&value->val);
}

sw_error_t
hppe_rx_fifooverflow_frames_high_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rx_fifooverflow_frames_high_u *value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_rx_vlan_frames_good_bad_low_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rx_vlan_frames_good_bad_low_u *value)
{
	if (index >= RX_VLAN_FRAMES_GOOD_BAD_LOW_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				NSS_XGMAC_CSR_BASE_ADDR + RX_VLAN_FRAMES_GOOD_BAD_LOW_ADDRESS + \
				index * RX_VLAN_FRAMES_GOOD_BAD_LOW_INC,
				&value->val);
}

sw_error_t
hppe_rx_vlan_frames_good_bad_low_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rx_vlan_frames_good_bad_low_u *value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_rx_vlan_frames_good_bad_high_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rx_vlan_frames_good_bad_high_u *value)
{
	if (index >= RX_VLAN_FRAMES_GOOD_BAD_HIGH_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				NSS_XGMAC_CSR_BASE_ADDR + RX_VLAN_FRAMES_GOOD_BAD_HIGH_ADDRESS + \
				index * RX_VLAN_FRAMES_GOOD_BAD_HIGH_INC,
				&value->val);
}

sw_error_t
hppe_rx_vlan_frames_good_bad_high_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rx_vlan_frames_good_bad_high_u *value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_rx_watchdog_error_frames_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rx_watchdog_error_frames_u *value)
{
	if (index >= RX_WATCHDOG_ERROR_FRAMES_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				NSS_XGMAC_CSR_BASE_ADDR + RX_WATCHDOG_ERROR_FRAMES_ADDRESS + \
				index * RX_WATCHDOG_ERROR_FRAMES_INC,
				&value->val);
}

sw_error_t
hppe_rx_watchdog_error_frames_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rx_watchdog_error_frames_u *value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_rx_lpi_usec_cntr_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rx_lpi_usec_cntr_u *value)
{
	if (index >= RX_LPI_USEC_CNTR_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				NSS_XGMAC_CSR_BASE_ADDR + RX_LPI_USEC_CNTR_ADDRESS + \
				index * RX_LPI_USEC_CNTR_INC,
				&value->val);
}

sw_error_t
hppe_rx_lpi_usec_cntr_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rx_lpi_usec_cntr_u *value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_rx_lpi_tran_cntr_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rx_lpi_tran_cntr_u *value)
{
	if (index >= RX_LPI_TRAN_CNTR_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				NSS_XGMAC_CSR_BASE_ADDR + RX_LPI_TRAN_CNTR_ADDRESS + \
				index * RX_LPI_TRAN_CNTR_INC,
				&value->val);
}

sw_error_t
hppe_rx_lpi_tran_cntr_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rx_lpi_tran_cntr_u *value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_rx_discard_frame_count_good_bad_low_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rx_discard_frame_count_good_bad_low_u *value)
{
	if (index >= RX_DISCARD_FRAME_COUNT_GOOD_BAD_LOW_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				NSS_XGMAC_CSR_BASE_ADDR + RX_DISCARD_FRAME_COUNT_GOOD_BAD_LOW_ADDRESS + \
				index * RX_DISCARD_FRAME_COUNT_GOOD_BAD_LOW_INC,
				&value->val);
}

sw_error_t
hppe_rx_discard_frame_count_good_bad_low_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rx_discard_frame_count_good_bad_low_u *value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_rx_discard_frame_count_good_bad_high_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rx_discard_frame_count_good_bad_high_u *value)
{
	if (index >= RX_DISCARD_FRAME_COUNT_GOOD_BAD_HIGH_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				NSS_XGMAC_CSR_BASE_ADDR + RX_DISCARD_FRAME_COUNT_GOOD_BAD_HIGH_ADDRESS + \
				index * RX_DISCARD_FRAME_COUNT_GOOD_BAD_HIGH_INC,
				&value->val);
}

sw_error_t
hppe_rx_discard_frame_count_good_bad_high_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rx_discard_frame_count_good_bad_high_u *value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_rx_discard_octet_count_good_bad_low_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rx_discard_octet_count_good_bad_low_u *value)
{
	if (index >= RX_DISCARD_OCTET_COUNT_GOOD_BAD_LOW_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				NSS_XGMAC_CSR_BASE_ADDR + RX_DISCARD_OCTET_COUNT_GOOD_BAD_LOW_ADDRESS + \
				index * RX_DISCARD_OCTET_COUNT_GOOD_BAD_LOW_INC,
				&value->val);
}

sw_error_t
hppe_rx_discard_octet_count_good_bad_low_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rx_discard_octet_count_good_bad_low_u *value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_rx_discard_octet_count_good_bad_high_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rx_discard_octet_count_good_bad_high_u *value)
{
	if (index >= RX_DISCARD_OCTET_COUNT_GOOD_BAD_HIGH_MAX_ENTRY)
		return SW_OUT_OF_RANGE;
	return hppe_reg_get(
				dev_id,
				NSS_XGMAC_CSR_BASE_ADDR + RX_DISCARD_OCTET_COUNT_GOOD_BAD_HIGH_ADDRESS + \
				index * RX_DISCARD_OCTET_COUNT_GOOD_BAD_HIGH_INC,
				&value->val);
}

sw_error_t
hppe_rx_discard_octet_count_good_bad_high_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		union rx_discard_octet_count_good_bad_high_u *value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_mmc_control_cntrst_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mmc_control_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mmc_control_get(dev_id, index, &reg_val);
	*value = reg_val.bf.cntrst;
	return ret;
}

sw_error_t
hppe_mmc_control_cntrst_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_mmc_control_rstonrd_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mmc_control_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mmc_control_get(dev_id, index, &reg_val);
	*value = reg_val.bf.rstonrd;
	return ret;
}

sw_error_t
hppe_mmc_control_rstonrd_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_mmc_control_cntstopro_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mmc_control_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mmc_control_get(dev_id, index, &reg_val);
	*value = reg_val.bf.cntstopro;
	return ret;
}

sw_error_t
hppe_mmc_control_cntstopro_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_mmc_control_mct_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mmc_control_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mmc_control_get(dev_id, index, &reg_val);
	*value = reg_val.bf.mct;
	return ret;
}

sw_error_t
hppe_mmc_control_mct_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_mmc_control_pr_mmc_sel_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mmc_control_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mmc_control_get(dev_id, index, &reg_val);
	*value = reg_val.bf.pr_mmc_sel;
	return ret;
}

sw_error_t
hppe_mmc_control_pr_mmc_sel_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_mmc_control_cntprst_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mmc_control_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mmc_control_get(dev_id, index, &reg_val);
	*value = reg_val.bf.cntprst;
	return ret;
}

sw_error_t
hppe_mmc_control_cntprst_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_mmc_control_mcf_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union mmc_control_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_mmc_control_get(dev_id, index, &reg_val);
	*value = reg_val.bf.mcf;
	return ret;
}

sw_error_t
hppe_mmc_control_mcf_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_tx_octet_count_good_bad_low_txoctgblo_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union tx_octet_count_good_bad_low_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_tx_octet_count_good_bad_low_get(dev_id, index, &reg_val);
	*value = reg_val.bf.txoctgblo;
	return ret;
}

sw_error_t
hppe_tx_octet_count_good_bad_low_txoctgblo_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_tx_octet_count_good_bad_high_txoctgbhi_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union tx_octet_count_good_bad_high_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_tx_octet_count_good_bad_high_get(dev_id, index, &reg_val);
	*value = reg_val.bf.txoctgbhi;
	return ret;
}

sw_error_t
hppe_tx_octet_count_good_bad_high_txoctgbhi_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_tx_frame_count_good_bad_low_txfrmgblo_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union tx_frame_count_good_bad_low_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_tx_frame_count_good_bad_low_get(dev_id, index, &reg_val);
	*value = reg_val.bf.txfrmgblo;
	return ret;
}

sw_error_t
hppe_tx_frame_count_good_bad_low_txfrmgblo_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_tx_frame_count_good_bad_high_txfrmgbhi_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union tx_frame_count_good_bad_high_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_tx_frame_count_good_bad_high_get(dev_id, index, &reg_val);
	*value = reg_val.bf.txfrmgbhi;
	return ret;
}

sw_error_t
hppe_tx_frame_count_good_bad_high_txfrmgbhi_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_tx_broadcast_frames_good_low_txbcastglo_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union tx_broadcast_frames_good_low_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_tx_broadcast_frames_good_low_get(dev_id, index, &reg_val);
	*value = reg_val.bf.txbcastglo;
	return ret;
}

sw_error_t
hppe_tx_broadcast_frames_good_low_txbcastglo_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_tx_broadcast_frames_good_high_txbcastghi_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union tx_broadcast_frames_good_high_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_tx_broadcast_frames_good_high_get(dev_id, index, &reg_val);
	*value = reg_val.bf.txbcastghi;
	return ret;
}

sw_error_t
hppe_tx_broadcast_frames_good_high_txbcastghi_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_tx_multicast_frames_good_low_txmcastglo_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union tx_multicast_frames_good_low_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_tx_multicast_frames_good_low_get(dev_id, index, &reg_val);
	*value = reg_val.bf.txmcastglo;
	return ret;
}

sw_error_t
hppe_tx_multicast_frames_good_low_txmcastglo_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_tx_multicast_frames_good_high_txmcastghi_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union tx_multicast_frames_good_high_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_tx_multicast_frames_good_high_get(dev_id, index, &reg_val);
	*value = reg_val.bf.txmcastghi;
	return ret;
}

sw_error_t
hppe_tx_multicast_frames_good_high_txmcastghi_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_tx_64octets_frames_good_bad_low_tx64octgblo_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union tx_64octets_frames_good_bad_low_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_tx_64octets_frames_good_bad_low_get(dev_id, index, &reg_val);
	*value = reg_val.bf.tx64octgblo;
	return ret;
}

sw_error_t
hppe_tx_64octets_frames_good_bad_low_tx64octgblo_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_tx_64octets_frames_good_bad_high_tx64octgbhi_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union tx_64octets_frames_good_bad_high_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_tx_64octets_frames_good_bad_high_get(dev_id, index, &reg_val);
	*value = reg_val.bf.tx64octgbhi;
	return ret;
}

sw_error_t
hppe_tx_64octets_frames_good_bad_high_tx64octgbhi_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_tx_65to127octets_frames_good_bad_low_tx65_127octgblo_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union tx_65to127octets_frames_good_bad_low_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_tx_65to127octets_frames_good_bad_low_get(dev_id, index, &reg_val);
	*value = reg_val.bf.tx65_127octgblo;
	return ret;
}

sw_error_t
hppe_tx_65to127octets_frames_good_bad_low_tx65_127octgblo_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_tx_65to127octets_frames_good_bad_high_tx65_127octgbhi_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union tx_65to127octets_frames_good_bad_high_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_tx_65to127octets_frames_good_bad_high_get(dev_id, index, &reg_val);
	*value = reg_val.bf.tx65_127octgbhi;
	return ret;
}

sw_error_t
hppe_tx_65to127octets_frames_good_bad_high_tx65_127octgbhi_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_tx_128to255octets_frames_good_bad_low_tx128_255octgblo_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union tx_128to255octets_frames_good_bad_low_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_tx_128to255octets_frames_good_bad_low_get(dev_id, index, &reg_val);
	*value = reg_val.bf.tx128_255octgblo;
	return ret;
}

sw_error_t
hppe_tx_128to255octets_frames_good_bad_low_tx128_255octgblo_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_tx_128to255octets_frames_good_bad_high_tx128_255octgbhi_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union tx_128to255octets_frames_good_bad_high_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_tx_128to255octets_frames_good_bad_high_get(dev_id, index, &reg_val);
	*value = reg_val.bf.tx128_255octgbhi;
	return ret;
}

sw_error_t
hppe_tx_128to255octets_frames_good_bad_high_tx128_255octgbhi_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_tx_256to511octets_frames_good_bad_low_tx256_511octgblo_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union tx_256to511octets_frames_good_bad_low_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_tx_256to511octets_frames_good_bad_low_get(dev_id, index, &reg_val);
	*value = reg_val.bf.tx256_511octgblo;
	return ret;
}

sw_error_t
hppe_tx_256to511octets_frames_good_bad_low_tx256_511octgblo_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_tx_256to511octets_frames_good_bad_high_tx256_511octgbhi_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union tx_256to511octets_frames_good_bad_high_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_tx_256to511octets_frames_good_bad_high_get(dev_id, index, &reg_val);
	*value = reg_val.bf.tx256_511octgbhi;
	return ret;
}

sw_error_t
hppe_tx_256to511octets_frames_good_bad_high_tx256_511octgbhi_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_tx_512to1023octets_frames_good_bad_low_tx512_1023octgblo_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union tx_512to1023octets_frames_good_bad_low_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_tx_512to1023octets_frames_good_bad_low_get(dev_id, index, &reg_val);
	*value = reg_val.bf.tx512_1023octgblo;
	return ret;
}

sw_error_t
hppe_tx_512to1023octets_frames_good_bad_low_tx512_1023octgblo_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_tx_512to1023octets_frames_good_bad_high_tx512_1023octgbhi_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union tx_512to1023octets_frames_good_bad_high_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_tx_512to1023octets_frames_good_bad_high_get(dev_id, index, &reg_val);
	*value = reg_val.bf.tx512_1023octgbhi;
	return ret;
}

sw_error_t
hppe_tx_512to1023octets_frames_good_bad_high_tx512_1023octgbhi_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_tx_1024tomaxoctets_frames_good_bad_low_tx1024_maxoctgblo_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union tx_1024tomaxoctets_frames_good_bad_low_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_tx_1024tomaxoctets_frames_good_bad_low_get(dev_id, index, &reg_val);
	*value = reg_val.bf.tx1024_maxoctgblo;
	return ret;
}

sw_error_t
hppe_tx_1024tomaxoctets_frames_good_bad_low_tx1024_maxoctgblo_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_tx_1024tomaxoctets_frames_good_bad_high_tx1024_maxoctgbhi_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union tx_1024tomaxoctets_frames_good_bad_high_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_tx_1024tomaxoctets_frames_good_bad_high_get(dev_id, index, &reg_val);
	*value = reg_val.bf.tx1024_maxoctgbhi;
	return ret;
}

sw_error_t
hppe_tx_1024tomaxoctets_frames_good_bad_high_tx1024_maxoctgbhi_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_tx_unicast_frames_good_bad_low_txucastgblo_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union tx_unicast_frames_good_bad_low_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_tx_unicast_frames_good_bad_low_get(dev_id, index, &reg_val);
	*value = reg_val.bf.txucastgblo;
	return ret;
}

sw_error_t
hppe_tx_unicast_frames_good_bad_low_txucastgblo_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_tx_unicast_frames_good_bad_high_txucastgbhi_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union tx_unicast_frames_good_bad_high_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_tx_unicast_frames_good_bad_high_get(dev_id, index, &reg_val);
	*value = reg_val.bf.txucastgbhi;
	return ret;
}

sw_error_t
hppe_tx_unicast_frames_good_bad_high_txucastgbhi_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_tx_multicast_frames_good_bad_low_txmcastgblo_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union tx_multicast_frames_good_bad_low_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_tx_multicast_frames_good_bad_low_get(dev_id, index, &reg_val);
	*value = reg_val.bf.txmcastgblo;
	return ret;
}

sw_error_t
hppe_tx_multicast_frames_good_bad_low_txmcastgblo_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_tx_multicast_frames_good_bad_high_txmcastgbhi_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union tx_multicast_frames_good_bad_high_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_tx_multicast_frames_good_bad_high_get(dev_id, index, &reg_val);
	*value = reg_val.bf.txmcastgbhi;
	return ret;
}

sw_error_t
hppe_tx_multicast_frames_good_bad_high_txmcastgbhi_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_tx_broadcast_frames_good_bad_low_txbcastgblo_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union tx_broadcast_frames_good_bad_low_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_tx_broadcast_frames_good_bad_low_get(dev_id, index, &reg_val);
	*value = reg_val.bf.txbcastgblo;
	return ret;
}

sw_error_t
hppe_tx_broadcast_frames_good_bad_low_txbcastgblo_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_tx_broadcast_frames_good_bad_high_txbcastgbhi_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union tx_broadcast_frames_good_bad_high_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_tx_broadcast_frames_good_bad_high_get(dev_id, index, &reg_val);
	*value = reg_val.bf.txbcastgbhi;
	return ret;
}

sw_error_t
hppe_tx_broadcast_frames_good_bad_high_txbcastgbhi_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_tx_underflow_error_frames_low_txundrflwlo_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union tx_underflow_error_frames_low_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_tx_underflow_error_frames_low_get(dev_id, index, &reg_val);
	*value = reg_val.bf.txundrflwlo;
	return ret;
}

sw_error_t
hppe_tx_underflow_error_frames_low_txundrflwlo_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_tx_underflow_error_frames_high_txundrflwhi_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union tx_underflow_error_frames_high_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_tx_underflow_error_frames_high_get(dev_id, index, &reg_val);
	*value = reg_val.bf.txundrflwhi;
	return ret;
}

sw_error_t
hppe_tx_underflow_error_frames_high_txundrflwhi_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_tx_octet_count_good_low_txoctglo_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union tx_octet_count_good_low_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_tx_octet_count_good_low_get(dev_id, index, &reg_val);
	*value = reg_val.bf.txoctglo;
	return ret;
}

sw_error_t
hppe_tx_octet_count_good_low_txoctglo_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_tx_octet_count_good_high_txoctghi_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union tx_octet_count_good_high_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_tx_octet_count_good_high_get(dev_id, index, &reg_val);
	*value = reg_val.bf.txoctghi;
	return ret;
}

sw_error_t
hppe_tx_octet_count_good_high_txoctghi_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_tx_frame_count_good_low_txfrmglo_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union tx_frame_count_good_low_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_tx_frame_count_good_low_get(dev_id, index, &reg_val);
	*value = reg_val.bf.txfrmglo;
	return ret;
}

sw_error_t
hppe_tx_frame_count_good_low_txfrmglo_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_tx_frame_count_good_high_txfrmghi_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union tx_frame_count_good_high_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_tx_frame_count_good_high_get(dev_id, index, &reg_val);
	*value = reg_val.bf.txfrmghi;
	return ret;
}

sw_error_t
hppe_tx_frame_count_good_high_txfrmghi_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_tx_pause_frames_low_txpauseglo_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union tx_pause_frames_low_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_tx_pause_frames_low_get(dev_id, index, &reg_val);
	*value = reg_val.bf.txpauseglo;
	return ret;
}

sw_error_t
hppe_tx_pause_frames_low_txpauseglo_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_tx_pause_frames_high_txpauseghi_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union tx_pause_frames_high_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_tx_pause_frames_high_get(dev_id, index, &reg_val);
	*value = reg_val.bf.txpauseghi;
	return ret;
}

sw_error_t
hppe_tx_pause_frames_high_txpauseghi_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_tx_vlan_frames_good_low_txvlanglo_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union tx_vlan_frames_good_low_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_tx_vlan_frames_good_low_get(dev_id, index, &reg_val);
	*value = reg_val.bf.txvlanglo;
	return ret;
}

sw_error_t
hppe_tx_vlan_frames_good_low_txvlanglo_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_tx_vlan_frames_good_high_txvlanghi_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union tx_vlan_frames_good_high_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_tx_vlan_frames_good_high_get(dev_id, index, &reg_val);
	*value = reg_val.bf.txvlanghi;
	return ret;
}

sw_error_t
hppe_tx_vlan_frames_good_high_txvlanghi_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_tx_lpi_usec_cntr_txlpiusc_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union tx_lpi_usec_cntr_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_tx_lpi_usec_cntr_get(dev_id, index, &reg_val);
	*value = reg_val.bf.txlpiusc;
	return ret;
}

sw_error_t
hppe_tx_lpi_usec_cntr_txlpiusc_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_tx_lpi_tran_cntr_txlpitrc_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union tx_lpi_tran_cntr_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_tx_lpi_tran_cntr_get(dev_id, index, &reg_val);
	*value = reg_val.bf.txlpitrc;
	return ret;
}

sw_error_t
hppe_tx_lpi_tran_cntr_txlpitrc_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_rx_frame_count_good_bad_low_rxfrmgblo_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union rx_frame_count_good_bad_low_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_rx_frame_count_good_bad_low_get(dev_id, index, &reg_val);
	*value = reg_val.bf.rxfrmgblo;
	return ret;
}

sw_error_t
hppe_rx_frame_count_good_bad_low_rxfrmgblo_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_rx_frame_count_good_bad_high_rxfrmgbhi_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union rx_frame_count_good_bad_high_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_rx_frame_count_good_bad_high_get(dev_id, index, &reg_val);
	*value = reg_val.bf.rxfrmgbhi;
	return ret;
}

sw_error_t
hppe_rx_frame_count_good_bad_high_rxfrmgbhi_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_rx_octet_count_good_bad_low_rxoctgblo_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union rx_octet_count_good_bad_low_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_rx_octet_count_good_bad_low_get(dev_id, index, &reg_val);
	*value = reg_val.bf.rxoctgblo;
	return ret;
}

sw_error_t
hppe_rx_octet_count_good_bad_low_rxoctgblo_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_rx_octet_count_good_bad_high_rxoctgbhi_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union rx_octet_count_good_bad_high_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_rx_octet_count_good_bad_high_get(dev_id, index, &reg_val);
	*value = reg_val.bf.rxoctgbhi;
	return ret;
}

sw_error_t
hppe_rx_octet_count_good_bad_high_rxoctgbhi_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_rx_octet_count_good_low_rxoctglo_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union rx_octet_count_good_low_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_rx_octet_count_good_low_get(dev_id, index, &reg_val);
	*value = reg_val.bf.rxoctglo;
	return ret;
}

sw_error_t
hppe_rx_octet_count_good_low_rxoctglo_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_rx_octet_count_good_high_rxoctghi_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union rx_octet_count_good_high_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_rx_octet_count_good_high_get(dev_id, index, &reg_val);
	*value = reg_val.bf.rxoctghi;
	return ret;
}

sw_error_t
hppe_rx_octet_count_good_high_rxoctghi_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_rx_broadcast_frames_good_low_rxbcastglo_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union rx_broadcast_frames_good_low_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_rx_broadcast_frames_good_low_get(dev_id, index, &reg_val);
	*value = reg_val.bf.rxbcastglo;
	return ret;
}

sw_error_t
hppe_rx_broadcast_frames_good_low_rxbcastglo_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_rx_broadcast_frames_good_high_rxbcastghi_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union rx_broadcast_frames_good_high_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_rx_broadcast_frames_good_high_get(dev_id, index, &reg_val);
	*value = reg_val.bf.rxbcastghi;
	return ret;
}

sw_error_t
hppe_rx_broadcast_frames_good_high_rxbcastghi_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_rx_multicast_frames_good_low_rxmcastglo_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union rx_multicast_frames_good_low_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_rx_multicast_frames_good_low_get(dev_id, index, &reg_val);
	*value = reg_val.bf.rxmcastglo;
	return ret;
}

sw_error_t
hppe_rx_multicast_frames_good_low_rxmcastglo_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_rx_multicast_frames_good_high_rxmcastghi_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union rx_multicast_frames_good_high_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_rx_multicast_frames_good_high_get(dev_id, index, &reg_val);
	*value = reg_val.bf.rxmcastghi;
	return ret;
}

sw_error_t
hppe_rx_multicast_frames_good_high_rxmcastghi_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_rx_crc_error_frames_low_rxcrcerlo_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union rx_crc_error_frames_low_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_rx_crc_error_frames_low_get(dev_id, index, &reg_val);
	*value = reg_val.bf.rxcrcerlo;
	return ret;
}

sw_error_t
hppe_rx_crc_error_frames_low_rxcrcerlo_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_rx_crc_error_frames_high_rxcrcerhi_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union rx_crc_error_frames_high_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_rx_crc_error_frames_high_get(dev_id, index, &reg_val);
	*value = reg_val.bf.rxcrcerhi;
	return ret;
}

sw_error_t
hppe_rx_crc_error_frames_high_rxcrcerhi_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_rx_runt_error_frames_rxrunter_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union rx_runt_error_frames_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_rx_runt_error_frames_get(dev_id, index, &reg_val);
	*value = reg_val.bf.rxrunter;
	return ret;
}

sw_error_t
hppe_rx_runt_error_frames_rxrunter_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_rx_jabber_error_frames_rxjaberer_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union rx_jabber_error_frames_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_rx_jabber_error_frames_get(dev_id, index, &reg_val);
	*value = reg_val.bf.rxjaberer;
	return ret;
}

sw_error_t
hppe_rx_jabber_error_frames_rxjaberer_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_rx_undersize_frames_good_rxusizeg_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union rx_undersize_frames_good_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_rx_undersize_frames_good_get(dev_id, index, &reg_val);
	*value = reg_val.bf.rxusizeg;
	return ret;
}

sw_error_t
hppe_rx_undersize_frames_good_rxusizeg_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_rx_oversize_frames_good_rxosizeg_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union rx_oversize_frames_good_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_rx_oversize_frames_good_get(dev_id, index, &reg_val);
	*value = reg_val.bf.rxosizeg;
	return ret;
}

sw_error_t
hppe_rx_oversize_frames_good_rxosizeg_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_rx_64octets_frames_good_bad_low_rx64octgblo_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union rx_64octets_frames_good_bad_low_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_rx_64octets_frames_good_bad_low_get(dev_id, index, &reg_val);
	*value = reg_val.bf.rx64octgblo;
	return ret;
}

sw_error_t
hppe_rx_64octets_frames_good_bad_low_rx64octgblo_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_rx_64octets_frames_good_bad_high_rx64octgbhi_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union rx_64octets_frames_good_bad_high_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_rx_64octets_frames_good_bad_high_get(dev_id, index, &reg_val);
	*value = reg_val.bf.rx64octgbhi;
	return ret;
}

sw_error_t
hppe_rx_64octets_frames_good_bad_high_rx64octgbhi_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_rx_65to127octets_frames_good_bad_low_rx65_127octgblo_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union rx_65to127octets_frames_good_bad_low_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_rx_65to127octets_frames_good_bad_low_get(dev_id, index, &reg_val);
	*value = reg_val.bf.rx65_127octgblo;
	return ret;
}

sw_error_t
hppe_rx_65to127octets_frames_good_bad_low_rx65_127octgblo_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_rx_65to127octets_frames_good_bad_high_rx65_127octgbhi_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union rx_65to127octets_frames_good_bad_high_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_rx_65to127octets_frames_good_bad_high_get(dev_id, index, &reg_val);
	*value = reg_val.bf.rx65_127octgbhi;
	return ret;
}

sw_error_t
hppe_rx_65to127octets_frames_good_bad_high_rx65_127octgbhi_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_rx_128to255octets_frames_good_bad_low_rx128_255octgblo_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union rx_128to255octets_frames_good_bad_low_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_rx_128to255octets_frames_good_bad_low_get(dev_id, index, &reg_val);
	*value = reg_val.bf.rx128_255octgblo;
	return ret;
}

sw_error_t
hppe_rx_128to255octets_frames_good_bad_low_rx128_255octgblo_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_rx_128to255octets_frames_good_bad_high_rx128_255octgbhi_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union rx_128to255octets_frames_good_bad_high_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_rx_128to255octets_frames_good_bad_high_get(dev_id, index, &reg_val);
	*value = reg_val.bf.rx128_255octgbhi;
	return ret;
}

sw_error_t
hppe_rx_128to255octets_frames_good_bad_high_rx128_255octgbhi_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_rx_256to511octets_frames_good_bad_low_rx256_511octgblo_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union rx_256to511octets_frames_good_bad_low_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_rx_256to511octets_frames_good_bad_low_get(dev_id, index, &reg_val);
	*value = reg_val.bf.rx256_511octgblo;
	return ret;
}

sw_error_t
hppe_rx_256to511octets_frames_good_bad_low_rx256_511octgblo_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_rx_256to511octets_frames_good_bad_high_rx256_511octgbhi_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union rx_256to511octets_frames_good_bad_high_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_rx_256to511octets_frames_good_bad_high_get(dev_id, index, &reg_val);
	*value = reg_val.bf.rx256_511octgbhi;
	return ret;
}

sw_error_t
hppe_rx_256to511octets_frames_good_bad_high_rx256_511octgbhi_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_rx_512to1023octets_frames_good_bad_low_rx512_1023octgblo_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union rx_512to1023octets_frames_good_bad_low_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_rx_512to1023octets_frames_good_bad_low_get(dev_id, index, &reg_val);
	*value = reg_val.bf.rx512_1023octgblo;
	return ret;
}

sw_error_t
hppe_rx_512to1023octets_frames_good_bad_low_rx512_1023octgblo_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_rx_512to1023octets_frames_good_bad_high_rx512_1023octgbhi_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union rx_512to1023octets_frames_good_bad_high_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_rx_512to1023octets_frames_good_bad_high_get(dev_id, index, &reg_val);
	*value = reg_val.bf.rx512_1023octgbhi;
	return ret;
}

sw_error_t
hppe_rx_512to1023octets_frames_good_bad_high_rx512_1023octgbhi_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_rx_1024tomaxoctets_frames_good_bad_low_rx1024_maxgboctlo_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union rx_1024tomaxoctets_frames_good_bad_low_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_rx_1024tomaxoctets_frames_good_bad_low_get(dev_id, index, &reg_val);
	*value = reg_val.bf.rx1024_maxgboctlo;
	return ret;
}

sw_error_t
hppe_rx_1024tomaxoctets_frames_good_bad_low_rx1024_maxgboctlo_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_rx_1024tomaxoctets_frames_good_bad_high_rx1024_maxgbocthi_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union rx_1024tomaxoctets_frames_good_bad_high_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_rx_1024tomaxoctets_frames_good_bad_high_get(dev_id, index, &reg_val);
	*value = reg_val.bf.rx1024_maxgbocthi;
	return ret;
}

sw_error_t
hppe_rx_1024tomaxoctets_frames_good_bad_high_rx1024_maxgbocthi_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_rx_unicast_frames_good_low_rxucastglo_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union rx_unicast_frames_good_low_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_rx_unicast_frames_good_low_get(dev_id, index, &reg_val);
	*value = reg_val.bf.rxucastglo;
	return ret;
}

sw_error_t
hppe_rx_unicast_frames_good_low_rxucastglo_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_rx_unicast_frames_good_high_rxucastghi_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union rx_unicast_frames_good_high_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_rx_unicast_frames_good_high_get(dev_id, index, &reg_val);
	*value = reg_val.bf.rxucastghi;
	return ret;
}

sw_error_t
hppe_rx_unicast_frames_good_high_rxucastghi_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_rx_length_error_frames_low_rxlenerrlo_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union rx_length_error_frames_low_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_rx_length_error_frames_low_get(dev_id, index, &reg_val);
	*value = reg_val.bf.rxlenerrlo;
	return ret;
}

sw_error_t
hppe_rx_length_error_frames_low_rxlenerrlo_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_rx_length_error_frames_high_rxlenerrhi_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union rx_length_error_frames_high_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_rx_length_error_frames_high_get(dev_id, index, &reg_val);
	*value = reg_val.bf.rxlenerrhi;
	return ret;
}

sw_error_t
hppe_rx_length_error_frames_high_rxlenerrhi_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_rx_outofrange_frames_low_rxorangelo_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union rx_outofrange_frames_low_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_rx_outofrange_frames_low_get(dev_id, index, &reg_val);
	*value = reg_val.bf.rxorangelo;
	return ret;
}

sw_error_t
hppe_rx_outofrange_frames_low_rxorangelo_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_rx_outofrange_frames_high_rxorangehi_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union rx_outofrange_frames_high_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_rx_outofrange_frames_high_get(dev_id, index, &reg_val);
	*value = reg_val.bf.rxorangehi;
	return ret;
}

sw_error_t
hppe_rx_outofrange_frames_high_rxorangehi_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_rx_pause_frames_low_rxpauselo_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union rx_pause_frames_low_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_rx_pause_frames_low_get(dev_id, index, &reg_val);
	*value = reg_val.bf.rxpauselo;
	return ret;
}

sw_error_t
hppe_rx_pause_frames_low_rxpauselo_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_rx_pause_frames_high_rxpausehi_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union rx_pause_frames_high_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_rx_pause_frames_high_get(dev_id, index, &reg_val);
	*value = reg_val.bf.rxpausehi;
	return ret;
}

sw_error_t
hppe_rx_pause_frames_high_rxpausehi_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_rx_fifooverflow_frames_low_rxfovflo_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union rx_fifooverflow_frames_low_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_rx_fifooverflow_frames_low_get(dev_id, index, &reg_val);
	*value = reg_val.bf.rxfovflo;
	return ret;
}

sw_error_t
hppe_rx_fifooverflow_frames_low_rxfovflo_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_rx_fifooverflow_frames_high_rxfovfhi_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union rx_fifooverflow_frames_high_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_rx_fifooverflow_frames_high_get(dev_id, index, &reg_val);
	*value = reg_val.bf.rxfovfhi;
	return ret;
}

sw_error_t
hppe_rx_fifooverflow_frames_high_rxfovfhi_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_rx_vlan_frames_good_bad_low_rxvlangblo_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union rx_vlan_frames_good_bad_low_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_rx_vlan_frames_good_bad_low_get(dev_id, index, &reg_val);
	*value = reg_val.bf.rxvlangblo;
	return ret;
}

sw_error_t
hppe_rx_vlan_frames_good_bad_low_rxvlangblo_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_rx_vlan_frames_good_bad_high_rxvlangbhi_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union rx_vlan_frames_good_bad_high_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_rx_vlan_frames_good_bad_high_get(dev_id, index, &reg_val);
	*value = reg_val.bf.rxvlangbhi;
	return ret;
}

sw_error_t
hppe_rx_vlan_frames_good_bad_high_rxvlangbhi_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_rx_watchdog_error_frames_rxwdogerr_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union rx_watchdog_error_frames_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_rx_watchdog_error_frames_get(dev_id, index, &reg_val);
	*value = reg_val.bf.rxwdogerr;
	return ret;
}

sw_error_t
hppe_rx_watchdog_error_frames_rxwdogerr_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_rx_lpi_usec_cntr_rxlpiusc_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union rx_lpi_usec_cntr_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_rx_lpi_usec_cntr_get(dev_id, index, &reg_val);
	*value = reg_val.bf.rxlpiusc;
	return ret;
}

sw_error_t
hppe_rx_lpi_usec_cntr_rxlpiusc_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_rx_lpi_tran_cntr_rxlpitrc_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union rx_lpi_tran_cntr_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_rx_lpi_tran_cntr_get(dev_id, index, &reg_val);
	*value = reg_val.bf.rxlpitrc;
	return ret;
}

sw_error_t
hppe_rx_lpi_tran_cntr_rxlpitrc_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_rx_discard_frame_count_good_bad_low_rxdfcntgblo_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union rx_discard_frame_count_good_bad_low_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_rx_discard_frame_count_good_bad_low_get(dev_id, index, &reg_val);
	*value = reg_val.bf.rxdfcntgblo;
	return ret;
}

sw_error_t
hppe_rx_discard_frame_count_good_bad_low_rxdfcntgblo_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_rx_discard_frame_count_good_bad_high_rxdfcntgbhi_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union rx_discard_frame_count_good_bad_high_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_rx_discard_frame_count_good_bad_high_get(dev_id, index, &reg_val);
	*value = reg_val.bf.rxdfcntgbhi;
	return ret;
}

sw_error_t
hppe_rx_discard_frame_count_good_bad_high_rxdfcntgbhi_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_rx_discard_octet_count_good_bad_low_rxdocntgblo_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union rx_discard_octet_count_good_bad_low_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_rx_discard_octet_count_good_bad_low_get(dev_id, index, &reg_val);
	*value = reg_val.bf.rxdocntgblo;
	return ret;
}

sw_error_t
hppe_rx_discard_octet_count_good_bad_low_rxdocntgblo_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}

sw_error_t
hppe_rx_discard_octet_count_good_bad_high_rxdocntgbhi_get(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t *value)
{
	union rx_discard_octet_count_good_bad_high_u reg_val;
	sw_error_t ret = SW_OK;

	ret = hppe_rx_discard_octet_count_good_bad_high_get(dev_id, index, &reg_val);
	*value = reg_val.bf.rxdocntgbhi;
	return ret;
}

sw_error_t
hppe_rx_discard_octet_count_good_bad_high_rxdocntgbhi_set(
		a_uint32_t dev_id,
		a_uint32_t index,
		a_uint32_t value)
{
	return SW_NOT_SUPPORTED;
}


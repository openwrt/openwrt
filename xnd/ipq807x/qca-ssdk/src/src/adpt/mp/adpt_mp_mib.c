/*
 * Copyright (c) 2020, The Linux Foundation. All rights reserved.
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include "sw.h"
#include "mp_mib_reg.h"
#include "mp_mib.h"
#include "adpt.h"
#include "adpt_mp.h"

sw_error_t
adpt_mp_mib_cpukeep_get(a_uint32_t dev_id, a_bool_t *enable)
{
	sw_error_t rv = SW_OK;
	union mmc_control_u mmc_control;

	ADPT_DEV_ID_CHECK(dev_id);
	ADPT_NULL_POINT_CHECK(enable);

	rv = mp_mmc_control_get(dev_id, MP_GMAC0, &mmc_control);
	SW_RTN_ON_ERROR(rv);
	if(mmc_control.bf.rstonrd)
	{
		*enable = A_FALSE;
	}
	else
	{
		*enable = A_TRUE;
	}

	return rv;
}

sw_error_t
adpt_mp_mib_cpukeep_set(a_uint32_t dev_id, a_bool_t enable)
{
	sw_error_t rv = SW_OK;
	a_uint32_t gmac_id = 0, status = 0;
	union mmc_control_u mmc_control;

	ADPT_DEV_ID_CHECK(dev_id);

	if (enable == A_TRUE)
	{
		status = A_FALSE;
	}
	else
	{
		status = A_TRUE;
	}
	for(gmac_id = MP_GMAC0; gmac_id <= MP_GMAC1; gmac_id++)
	{
		rv = mp_mmc_control_get(dev_id, gmac_id, &mmc_control);
		SW_RTN_ON_ERROR(rv);
		mmc_control.bf.rstonrd = status;
		rv = mp_mmc_control_set(dev_id, gmac_id, &mmc_control);
		SW_RTN_ON_ERROR(rv);
	}

	return rv;
}

sw_error_t
adpt_mp_mib_status_get(a_uint32_t dev_id, a_bool_t *enable)
{
	sw_error_t rv = SW_OK;
	union mmc_control_u mmc_control;

	ADPT_DEV_ID_CHECK(dev_id);
	ADPT_NULL_POINT_CHECK(enable);

	rv = mp_mmc_control_get(dev_id, MP_GMAC0, &mmc_control);
	SW_RTN_ON_ERROR(rv);
	if(mmc_control.bf.cntfreez)
	{
		*enable = A_FALSE;
	}
	else
	{
		*enable = A_TRUE;
	}

	return rv;
}

sw_error_t
adpt_mp_mib_status_set(a_uint32_t dev_id, a_bool_t enable)
{
	sw_error_t rv = SW_OK;
	a_uint32_t gmac_id = 0, status = 0;
	union mmc_control_u mmc_control;

	ADPT_DEV_ID_CHECK(dev_id);

	if (enable == A_TRUE)
	{
		status = A_FALSE;
	}
	else
	{
		status = A_TRUE;
	}
	for(gmac_id = MP_GMAC0; gmac_id <= MP_GMAC1; gmac_id++)
	{
		rv = mp_mmc_control_get(dev_id, gmac_id, &mmc_control);
		SW_RTN_ON_ERROR(rv);
		mmc_control.bf.cntfreez = status;
		rv = mp_mmc_control_set(dev_id, gmac_id, &mmc_control);
		SW_RTN_ON_ERROR(rv);
	}

	return rv;
}

sw_error_t
adpt_mp_mib_port_flush_counters(a_uint32_t dev_id, fal_port_t port_id)
{
	sw_error_t rv = SW_OK;
	a_uint32_t gmac_id = 0;
	union mmc_control_u mmc_control;

	ADPT_DEV_ID_CHECK(dev_id);
	MP_PORT_ID_CHECK(port_id);

	gmac_id = MP_PORT_TO_GMAC_ID(port_id);
	rv = mp_mmc_control_get(dev_id, gmac_id, &mmc_control);
	SW_RTN_ON_ERROR(rv);
	mmc_control.bf.cntrst = A_TRUE;
	rv = mp_mmc_control_set(dev_id, gmac_id, &mmc_control);

	return rv;
}

static sw_error_t
adpt_mp_get_mib_info(a_uint32_t dev_id, fal_port_t port_id,
	fal_mib_info_t * mib_info)
{
	a_uint32_t gmac_id = 0;

	ADPT_DEV_ID_CHECK(dev_id);
	ADPT_NULL_POINT_CHECK(mib_info);
	MP_PORT_ID_CHECK(port_id);
	memset(mib_info, 0, sizeof(fal_mib_info_t));

	gmac_id = MP_PORT_TO_GMAC_ID(port_id);
	mp_tx_broadcast_frames_good_get(dev_id, gmac_id,
		(union tx_broadcast_frames_good_u *)&mib_info->TxBroad);
	mp_tx_multicast_frames_good_get(dev_id, gmac_id,
		(union tx_multicast_frames_good_u *)&mib_info->TxMulti);
	mp_tx_64octets_frames_good_bad_get(dev_id, gmac_id,
		(union tx_64octets_frames_good_bad_u *)&mib_info->Tx64Byte);
	mp_tx_65to127octets_frames_good_bad_get(dev_id, gmac_id,
		(union tx_65to127octets_frames_good_bad_u *)&mib_info->Tx128Byte);
	mp_tx_128to255octets_frames_good_bad_get(dev_id, gmac_id,
		(union tx_128to255octets_frames_good_bad_u *)&mib_info->Tx256Byte);
	mp_tx_256to511octets_frames_good_bad_get(dev_id, gmac_id,
		(union tx_256to511octets_frames_good_bad_u *)&mib_info->Tx512Byte);
	mp_tx_512to1023octets_frames_good_bad_get(dev_id, gmac_id,
		(union tx_512to1023octets_frames_good_bad_u *)&mib_info->Tx1024Byte);
	mp_tx_1024tomaxoctets_frames_good_bad_get(dev_id, gmac_id,
		(union tx_1024tomaxoctets_frames_good_bad_u *)&mib_info->TxMaxByte);
	mp_tx_unicast_frames_good_bad_get(dev_id, gmac_id,
		(union tx_unicast_frames_good_bad_u *)&mib_info->TxUniCast);
	mp_tx_underflow_error_frames_get(dev_id, gmac_id,
		(union tx_underflow_error_frames_u *)&mib_info->TxUnderRun);
	mp_tx_single_col_good_frames_get(dev_id, gmac_id,
		(union tx_single_collision_good_frames_u *)&mib_info->TxSingalCol);
	mp_t_multi_col_good_frames_get(dev_id, gmac_id,
		(union tx_multiple_collision_good_frames_u *)&mib_info->TxMultiCol);
	mp_tx_defer_frames_get(dev_id, gmac_id,
		(union tx_deferred_frames_u *)&mib_info->TxDefer);
	mp_tx_late_col_frames_get(dev_id, gmac_id,
		(union tx_late_collision_frames_u *)&mib_info->TxLateCol);
	mp_tx_excessive_col_frames_get(dev_id, gmac_id,
		(union tx_excessive_collision_frames_u *)&mib_info->TxExcDefer);
	mp_tx_octet_count_good_get(dev_id, gmac_id,
		(union tx_octet_count_good_u *) &mib_info->TxByte_lo);
	mp_tx_pause_frames_get(dev_id, gmac_id,
		(union tx_pause_frames_u *)&mib_info->TxPause);
	mp_tx_osize_frames_good_get(dev_id, gmac_id,
		(union tx_osize_frames_good_u *)&mib_info->TxOverSize);

	mp_rx_octet_count_good_get(dev_id, gmac_id,
		(union rx_octet_count_good_u *)&mib_info->RxGoodByte_lo);
	mp_rx_broadcast_frames_good_get(dev_id, gmac_id,
		(union rx_broadcast_frames_good_u *)&mib_info->RxBroad);
	mp_rx_multicast_frames_good_get(dev_id, gmac_id,
		(union rx_multicast_frames_good_u *)&mib_info->RxMulti);
	mp_rx_crc_error_frames_get(dev_id, gmac_id,
		(union rx_crc_error_frames_u *)&mib_info->RxFcsErr);
	mp_rx_alignment_error_frames_get(dev_id, gmac_id,
		(union rx_crc_error_frames_u *)&mib_info->RxAllignErr);
	mp_rx_runt_error_frames_get(dev_id, gmac_id,
		(union rx_runt_error_frames_u *)&mib_info->RxFragment);
	mp_rx_jabber_error_frames_get(dev_id, gmac_id,
		(union rx_jabber_error_frames_u *)&mib_info->RxJumboFcsErr);
	mp_rx_undersize_frames_good_get(dev_id, gmac_id,
		(union rx_undersize_frames_good_u *)&mib_info->RxRunt);
	mp_rx_oversize_frames_good_get(dev_id, gmac_id,
		(union rx_oversize_frames_good_u *)&mib_info->RxTooLong);
	mp_rx_64octets_frames_good_bad_get(dev_id, gmac_id,
		(union rx_64octets_frames_good_bad_u *)&mib_info->Rx64Byte);
	mp_rx_65to127octets_frames_good_bad_get(dev_id,gmac_id,
		(union rx_65to127octets_frames_good_bad_u *)&mib_info->Rx128Byte);
	mp_rx_128to255octets_frames_good_bad_get(dev_id, gmac_id,
		(union rx_128to255octets_frames_good_bad_u *)&mib_info->Rx256Byte);
	mp_rx_256to511octets_frames_good_bad_get(dev_id, gmac_id,
		(union rx_256to511octets_frames_good_bad_u *)&mib_info->Rx512Byte);
	mp_rx_512to1023octets_frames_good_bad_get(dev_id, gmac_id,
		(union rx_512to1023octets_frames_good_bad_u *)&mib_info->Rx1024Byte);
	mp_rx_1024tomaxoctets_frames_good_bad_get(dev_id, gmac_id,
		(union rx_1024tomaxoctets_frames_good_bad_u *)&mib_info->RxMaxByte);
	mp_rx_unicast_frames_good_get(dev_id, gmac_id,
		(union rx_unicast_frames_good_u *)&mib_info->RxUniCast);
	mp_rx_pause_frames_get(dev_id, gmac_id,
		(union rx_pause_frames_u *)&mib_info->RxPause);
	mp_rx_fifo_over_flow_frames_get(dev_id, gmac_id,
		(union rx_fifo_over_flow_frames_u *)&mib_info->RxOverFlow);

	return SW_OK;
}

sw_error_t
adpt_mp_get_tx_mib_info(a_uint32_t dev_id, fal_port_t port_id,
	fal_mib_info_t * mib_info)
{
	a_uint32_t gmac_id = 0;

	ADPT_DEV_ID_CHECK(dev_id);
	ADPT_NULL_POINT_CHECK(mib_info);
	MP_PORT_ID_CHECK(port_id);
	memset(mib_info, 0, sizeof(fal_mib_info_t));

	gmac_id = MP_PORT_TO_GMAC_ID(port_id);
	mp_tx_broadcast_frames_good_get(dev_id, gmac_id,
		(union tx_broadcast_frames_good_u *)&mib_info->TxBroad);
	mp_tx_multicast_frames_good_get(dev_id, gmac_id,
		(union tx_multicast_frames_good_u *)&mib_info->TxMulti);
	mp_tx_64octets_frames_good_bad_get(dev_id, gmac_id,
		(union tx_64octets_frames_good_bad_u *)&mib_info->Tx64Byte);
	mp_tx_65to127octets_frames_good_bad_get(dev_id, gmac_id,
		(union tx_65to127octets_frames_good_bad_u *)&mib_info->Tx128Byte);
	mp_tx_128to255octets_frames_good_bad_get(dev_id, gmac_id,
		(union tx_128to255octets_frames_good_bad_u *)&mib_info->Tx256Byte);
	mp_tx_256to511octets_frames_good_bad_get(dev_id, gmac_id,
		(union tx_256to511octets_frames_good_bad_u *)&mib_info->Tx512Byte);
	mp_tx_512to1023octets_frames_good_bad_get(dev_id, gmac_id,
		(union tx_512to1023octets_frames_good_bad_u *)&mib_info->Tx1024Byte);
	mp_tx_1024tomaxoctets_frames_good_bad_get(dev_id, gmac_id,
		(union tx_1024tomaxoctets_frames_good_bad_u *)&mib_info->TxMaxByte);
	mp_tx_unicast_frames_good_bad_get(dev_id, gmac_id,
		(union tx_unicast_frames_good_bad_u *)&mib_info->TxUniCast);
	mp_tx_underflow_error_frames_get(dev_id, gmac_id,
		(union tx_underflow_error_frames_u *)&mib_info->TxUnderRun);
	mp_tx_single_col_good_frames_get(dev_id, gmac_id,
		(union tx_single_collision_good_frames_u *)&mib_info->TxSingalCol);
	mp_t_multi_col_good_frames_get(dev_id, gmac_id,
		(union tx_multiple_collision_good_frames_u *)&mib_info->TxMultiCol);
	mp_tx_defer_frames_get(dev_id, gmac_id,
		(union tx_deferred_frames_u *)&mib_info->TxDefer);
	mp_tx_late_col_frames_get(dev_id, gmac_id,
		(union tx_late_collision_frames_u *)&mib_info->TxLateCol);
	mp_tx_excessive_col_frames_get(dev_id, gmac_id,
		(union tx_excessive_collision_frames_u *)&mib_info->TxExcDefer);
	mp_tx_octet_count_good_get(dev_id, gmac_id,
		(union tx_octet_count_good_u *) &mib_info->TxByte_lo);
	mp_tx_pause_frames_get(dev_id, gmac_id,
		(union tx_pause_frames_u *)&mib_info->TxPause);
	mp_tx_osize_frames_good_get(dev_id, gmac_id,
		(union tx_osize_frames_good_u *)&mib_info->TxOverSize);

	return SW_OK;
}

sw_error_t
adpt_mp_get_rx_mib_info(a_uint32_t dev_id, fal_port_t port_id,
                     fal_mib_info_t * mib_info )
{
	a_uint32_t gmac_id = 0;

	ADPT_DEV_ID_CHECK(dev_id);
	ADPT_NULL_POINT_CHECK(mib_info);
	MP_PORT_ID_CHECK(port_id);
	memset(mib_info, 0, sizeof(fal_mib_info_t));

	gmac_id = MP_PORT_TO_GMAC_ID(port_id);
	mp_rx_octet_count_good_get(dev_id, gmac_id,
		(union rx_octet_count_good_u *)&mib_info->RxGoodByte_lo);
	mp_rx_broadcast_frames_good_get(dev_id, gmac_id,
		(union rx_broadcast_frames_good_u *)&mib_info->RxBroad);
	mp_rx_multicast_frames_good_get(dev_id, gmac_id,
		(union rx_multicast_frames_good_u *)&mib_info->RxMulti);
	mp_rx_crc_error_frames_get(dev_id, gmac_id,
		(union rx_crc_error_frames_u *)&mib_info->RxFcsErr);
	mp_rx_alignment_error_frames_get(dev_id, gmac_id,
		(union rx_crc_error_frames_u *)&mib_info->RxAllignErr);
	mp_rx_runt_error_frames_get(dev_id, gmac_id,
		(union rx_runt_error_frames_u *)&mib_info->RxFragment);
	mp_rx_jabber_error_frames_get(dev_id, gmac_id,
		(union rx_jabber_error_frames_u *)&mib_info->RxJumboFcsErr);
	mp_rx_undersize_frames_good_get(dev_id, gmac_id,
		(union rx_undersize_frames_good_u *)&mib_info->RxRunt);
	mp_rx_oversize_frames_good_get(dev_id, gmac_id,
		(union rx_oversize_frames_good_u *)&mib_info->RxTooLong);
	mp_rx_64octets_frames_good_bad_get(dev_id, gmac_id,
		(union rx_64octets_frames_good_bad_u *)&mib_info->Rx64Byte);
	mp_rx_65to127octets_frames_good_bad_get(dev_id,gmac_id,
		(union rx_65to127octets_frames_good_bad_u *)&mib_info->Rx128Byte);
	mp_rx_128to255octets_frames_good_bad_get(dev_id, gmac_id,
		(union rx_128to255octets_frames_good_bad_u *)&mib_info->Rx256Byte);
	mp_rx_256to511octets_frames_good_bad_get(dev_id, gmac_id,
		(union rx_256to511octets_frames_good_bad_u *)&mib_info->Rx512Byte);
	mp_rx_512to1023octets_frames_good_bad_get(dev_id, gmac_id,
		(union rx_512to1023octets_frames_good_bad_u *)&mib_info->Rx1024Byte);
	mp_rx_1024tomaxoctets_frames_good_bad_get(dev_id, gmac_id,
		(union rx_1024tomaxoctets_frames_good_bad_u *)&mib_info->RxMaxByte);
	mp_rx_unicast_frames_good_get(dev_id, gmac_id,
		(union rx_unicast_frames_good_u *)&mib_info->RxUniCast);
	mp_rx_pause_frames_get(dev_id, gmac_id,
		(union rx_pause_frames_u *)&mib_info->RxPause);
	mp_rx_fifo_over_flow_frames_get(dev_id, gmac_id,
		(union rx_fifo_over_flow_frames_u *)&mib_info->RxOverFlow);

	return SW_OK;
}

sw_error_t adpt_mp_mib_init(a_uint32_t dev_id)
{
	adpt_api_t *p_adpt_api = NULL;

	p_adpt_api = adpt_api_ptr_get(dev_id);

	SW_RTN_ON_NULL(p_adpt_api);

	p_adpt_api->adpt_mib_cpukeep_get = adpt_mp_mib_cpukeep_get;
	p_adpt_api->adpt_mib_cpukeep_set = adpt_mp_mib_cpukeep_set;
	p_adpt_api->adpt_mib_status_get = adpt_mp_mib_status_get;
	p_adpt_api->adpt_mib_status_set = adpt_mp_mib_status_set;
	p_adpt_api->adpt_mib_port_flush_counters = adpt_mp_mib_port_flush_counters;
	p_adpt_api->adpt_get_mib_info = adpt_mp_get_mib_info;
	p_adpt_api->adpt_get_tx_mib_info = adpt_mp_get_tx_mib_info;
	p_adpt_api->adpt_get_rx_mib_info = adpt_mp_get_rx_mib_info;

	return SW_OK;
}
/**
 * @}
 */

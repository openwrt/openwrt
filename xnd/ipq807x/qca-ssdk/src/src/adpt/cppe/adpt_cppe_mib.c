/*
 * Copyright (c) 2019, The Linux Foundation. All rights reserved.
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
#include "adpt.h"
#include "cppe_loopback_reg.h"
#include "cppe_loopback.h"
#include "hppe_init.h"

sw_error_t
adpt_cppe_lpbk_mib_cpukeep_get(a_uint32_t dev_id, a_uint32_t port_id,
	a_bool_t *enable)
{
	sw_error_t rv = SW_OK;
	union lpbk_mib_ctrl_u reg_value;

	ADPT_DEV_ID_CHECK(dev_id);
	ADPT_NULL_POINT_CHECK(enable);

	rv = cppe_lpbk_mib_ctrl_get(dev_id, port_id, &reg_value);
	SW_RTN_ON_ERROR(rv);

	if (reg_value.bf.mib_rd_clr == A_TRUE)
	{
		*enable = A_FALSE;
	}
	else
	{
		*enable = A_TRUE;
	}

	return SW_OK;
}

sw_error_t
adpt_cppe_lpbk_mib_cpukeep_set(a_uint32_t dev_id, a_uint32_t port_id,
	a_bool_t enable)
{
	union lpbk_mib_ctrl_u reg_value;
	sw_error_t rv = SW_OK;

	ADPT_DEV_ID_CHECK(dev_id);
	port_id = HPPE_TO_GMAC_PORT_ID(port_id);
	rv = cppe_lpbk_mib_ctrl_get(dev_id, port_id, &reg_value);
	SW_RTN_ON_ERROR(rv);
	if(!enable)
	{
		reg_value.bf.mib_rd_clr = A_TRUE;
	}
	else
	{
		reg_value.bf.mib_rd_clr = A_FALSE;
	}
	rv = cppe_lpbk_mib_ctrl_set(dev_id, port_id, &reg_value);

	return rv;
}

sw_error_t
adpt_hppe_lpbk_mib_status_get(a_uint32_t dev_id, a_uint32_t port_id,
	a_bool_t *enable)
{
	sw_error_t rv = SW_OK;
	union lpbk_mib_ctrl_u reg_value;

	ADPT_DEV_ID_CHECK(dev_id);
	ADPT_NULL_POINT_CHECK(enable);

	rv = cppe_lpbk_mib_ctrl_get(dev_id, port_id, &reg_value);
	SW_RTN_ON_ERROR(rv);
	*enable = reg_value.bf.mib_en;

	return rv;
}

sw_error_t
adpt_cppe_lpbk_mib_status_set(a_uint32_t dev_id, a_uint32_t port_id,
	a_bool_t enable)
{
	union lpbk_mib_ctrl_u reg_value;
	sw_error_t rv = SW_OK;

	ADPT_DEV_ID_CHECK(dev_id);

	port_id = HPPE_TO_GMAC_PORT_ID(port_id);
	rv = cppe_lpbk_mib_ctrl_get(dev_id, port_id, &reg_value);
	SW_RTN_ON_ERROR(rv);
	reg_value.bf.mib_en = enable;
	rv = cppe_lpbk_mib_ctrl_set(dev_id, port_id, &reg_value);

	return rv;
}

sw_error_t
adpt_cppe_lpbk_mib_flush_counters(a_uint32_t dev_id, fal_port_t port_id)
{
	union lpbk_mib_ctrl_u reg_value;
	sw_error_t rv = SW_OK;

	ADPT_DEV_ID_CHECK(dev_id);
	port_id = HPPE_TO_GMAC_PORT_ID(port_id);
	rv = cppe_lpbk_mib_ctrl_get(dev_id, port_id, &reg_value);
	SW_RTN_ON_ERROR(rv);
	reg_value.bf.mib_reset = A_TRUE;
	rv = cppe_lpbk_mib_ctrl_set(dev_id, port_id, &reg_value);
	SW_RTN_ON_ERROR(rv);
	reg_value.bf.mib_reset = A_FALSE;
	rv = cppe_lpbk_mib_ctrl_set(dev_id, port_id, &reg_value);

	return rv;
}

sw_error_t
adpt_cppe_lpbk_get_mib_info(a_uint32_t dev_id, fal_port_t port_id,
	fal_mib_info_t * mib_info )
{
	ADPT_DEV_ID_CHECK(dev_id);
	ADPT_NULL_POINT_CHECK(mib_info);
	memset(mib_info, 0, sizeof(*mib_info));

	port_id = HPPE_TO_GMAC_PORT_ID(port_id);
	cppe_lpbk_mib_uni_get(dev_id, (a_uint32_t)port_id,
		(union lpbkuni_u *)&mib_info->RxUniCast);
	cppe_lpbk_mib_multi_get(dev_id, (a_uint32_t)port_id,
		(union lpbkmulti_u *)&mib_info->RxMulti);
	cppe_lpbk_mib_broad_get(dev_id, (a_uint32_t)port_id,
		(union lpbkbroad_u *)&mib_info->RxBroad);
	cppe_lpbk_mib_pkt64_get(dev_id, (a_uint32_t)port_id,
		(union lpbkpkt64_u *)&mib_info->Rx64Byte);
	cppe_lpbk_mib_pkt65to127_get(dev_id, (a_uint32_t)port_id,
		(union lpbkpkt65to127_u *)&mib_info->Rx128Byte);
	cppe_lpbk_mib_pkt128to255_get(dev_id, (a_uint32_t)port_id,
		(union lpbkpkt128to255_u *)&mib_info->Rx256Byte);
	cppe_lpbk_mib_pkt256to511_get(dev_id, (a_uint32_t)port_id,
		(union lpbkpkt256to511_u *)&mib_info->Rx512Byte);
	cppe_lpbk_mib_pkt512to1023_get(dev_id, (a_uint32_t)port_id,
		(union lpbkpkt512to1023_u *)&mib_info->Rx1024Byte);
	cppe_lpbk_mib_pkt1024to1518_get(dev_id, (a_uint32_t)port_id,
		(union lpbkpkt1024to1518_u *)&mib_info->Rx1518Byte);
	cppe_lpbk_mib_pkt1519tox_get(dev_id, (a_uint32_t)port_id,
		(union lpbkpkt1519tox_u *)&mib_info->RxMaxByte);
	cppe_lpbk_mib_toolong_get(dev_id, (a_uint32_t)port_id,
		(union lpbkpkttoolong_u *)&mib_info->RxTooLong);
	cppe_lpbk_mib_byte_l_get(dev_id, (a_uint32_t)port_id,
		(union lpbkbyte_l_u *)&mib_info->RxGoodByte_lo);
	cppe_lpbk_mib_byte_h_get(dev_id, (a_uint32_t)port_id,
		(union lpbkbyte_h_u *)&mib_info->RxGoodByte_hi);
	cppe_lpbk_mib_drop_get(dev_id, (a_uint32_t)port_id,
		(union lpbkdropcounter_u *)&mib_info->Filtered);
	cppe_lpbk_mib_tooshort_get(dev_id, (a_uint32_t)port_id,
		(union lpbkpkttooshort_u *)&mib_info->RxRunt);
	cppe_lpbk_mib_pkt14to63_get(dev_id, (a_uint32_t)port_id,
		(union lpbkpkt14to63_u *)&mib_info->Rx14To63);
	cppe_lpbk_mib_toolongbyte_l_get(dev_id, (a_uint32_t)port_id,
		(union lpbktoolongbyte_l_u *)&mib_info->RxTooLongByte_lo);
	cppe_lpbk_mib_toolongbyte_h_get(dev_id, (a_uint32_t)port_id,
		(union lpbktoolongbyte_h_u *)&mib_info->RxTooLongByte_hi);
	cppe_lpbk_mib_tooshortbyte_l_get(dev_id, (a_uint32_t)port_id,
		(union lpbktooshortbyte_l_u *)&mib_info->RxRuntByte_lo);
	cppe_lpbk_mib_tooshortbyte_h_get(dev_id, (a_uint32_t)port_id,
		(union lpbktooshortbyte_h_u *)&mib_info->RxRuntByte_hi);

	return SW_OK;
}
/**
 * @}
 */

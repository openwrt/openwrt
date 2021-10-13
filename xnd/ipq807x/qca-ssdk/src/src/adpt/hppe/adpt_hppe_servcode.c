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
#include "hppe_servcode_reg.h"
#include "hppe_servcode.h"
#include "hppe_fdb_reg.h"
#include "hppe_fdb.h"
#include "adpt.h"

#define MAX_PHYSICAL_PORT 8

sw_error_t adpt_hppe_servcode_config_set(a_uint32_t dev_id, a_uint32_t servcode_index,
					fal_servcode_config_t *entry)
{
	union in_l2_service_tbl_u in_l2_service_tbl;
	union service_tbl_u service_tbl;
	union eg_service_tbl_u eg_service_tbl;

	ADPT_DEV_ID_CHECK(dev_id);
	ADPT_NULL_POINT_CHECK(entry);

	if (servcode_index >= IN_L2_SERVICE_TBL_MAX_ENTRY || entry->dest_port_id >= MAX_PHYSICAL_PORT)
		return SW_OUT_OF_RANGE;

	in_l2_service_tbl.bf.dst_port_id_valid = entry->dest_port_valid;
	in_l2_service_tbl.bf.dst_port_id = entry->dest_port_id;
	in_l2_service_tbl.bf.direction = entry->direction;
	in_l2_service_tbl.bf.bypass_bitmap = entry->bypass_bitmap[1];
	in_l2_service_tbl.bf.rx_cnt_en = (entry->bypass_bitmap[2] >> 1) & 0x1;
	in_l2_service_tbl.bf.tx_cnt_en = (entry->bypass_bitmap[2] >> 3) & 0x1;
	SW_RTN_ON_ERROR(hppe_in_l2_service_tbl_set(dev_id, servcode_index, &in_l2_service_tbl));

	service_tbl.bf.bypass_bitmap = entry->bypass_bitmap[0];
	service_tbl.bf.rx_counting_en = entry->bypass_bitmap[2] & 0x1;
	SW_RTN_ON_ERROR(hppe_service_tbl_set(dev_id, servcode_index, &service_tbl));

	eg_service_tbl.bf.field_update_action = entry->field_update_bitmap;
	eg_service_tbl.bf.next_service_code = entry->next_service_code;
	eg_service_tbl.bf.hw_services = entry->hw_services;
	eg_service_tbl.bf.offset_sel = entry->offset_sel;
	eg_service_tbl.bf.tx_counting_en = (entry->bypass_bitmap[2] >> 2) & 0x1;
	SW_RTN_ON_ERROR(hppe_eg_service_tbl_set(dev_id, servcode_index, &eg_service_tbl));

	return SW_OK;
}

sw_error_t adpt_hppe_servcode_config_get(a_uint32_t dev_id, a_uint32_t servcode_index,
					fal_servcode_config_t *entry)
{
	union in_l2_service_tbl_u in_l2_service_tbl;
	union service_tbl_u service_tbl;
	union eg_service_tbl_u eg_service_tbl;

	ADPT_DEV_ID_CHECK(dev_id);
	ADPT_NULL_POINT_CHECK(entry);

	if (servcode_index >= IN_L2_SERVICE_TBL_MAX_ENTRY)
		return SW_OUT_OF_RANGE;

	SW_RTN_ON_ERROR(hppe_in_l2_service_tbl_get(dev_id, servcode_index, &in_l2_service_tbl));
	entry->dest_port_valid = in_l2_service_tbl.bf.dst_port_id_valid;
	entry->dest_port_id = in_l2_service_tbl.bf.dst_port_id;
	entry->direction = in_l2_service_tbl.bf.direction;
	entry->bypass_bitmap[1] = in_l2_service_tbl.bf.bypass_bitmap;
	entry->bypass_bitmap[2] |= in_l2_service_tbl.bf.rx_cnt_en << 1;
	entry->bypass_bitmap[2] |= in_l2_service_tbl.bf.tx_cnt_en << 3;

	SW_RTN_ON_ERROR(hppe_service_tbl_get(dev_id, servcode_index, &service_tbl));
	entry->bypass_bitmap[0] = service_tbl.bf.bypass_bitmap;
	entry->bypass_bitmap[2] |= service_tbl.bf.rx_counting_en;

	SW_RTN_ON_ERROR(hppe_eg_service_tbl_get(dev_id, servcode_index, &eg_service_tbl));
	entry->field_update_bitmap = eg_service_tbl.bf.field_update_action;
	entry->next_service_code = eg_service_tbl.bf.next_service_code;
	entry->hw_services = eg_service_tbl.bf.hw_services;
	entry->offset_sel = eg_service_tbl.bf.offset_sel;
	entry->bypass_bitmap[2] |= eg_service_tbl.bf.tx_counting_en << 2;

	return SW_OK;
}

sw_error_t adpt_hppe_servcode_loopcheck_en(a_uint32_t dev_id, a_bool_t enable)
{
	ADPT_DEV_ID_CHECK(dev_id);
#ifndef IN_FDB_MINI
	SW_RTN_ON_ERROR(hppe_l2_global_conf_service_code_loop_set(dev_id, enable));
#endif
	return SW_OK;
}

sw_error_t adpt_hppe_servcode_loopcheck_status_get(a_uint32_t dev_id, a_bool_t *enable)
{
	ADPT_DEV_ID_CHECK(dev_id);
	ADPT_NULL_POINT_CHECK(enable);
#ifndef IN_FDB_MINI
	SW_RTN_ON_ERROR(hppe_l2_global_conf_service_code_loop_get(dev_id, enable));
#endif
	return SW_OK;
}

void adpt_hppe_servcode_func_bitmap_init(a_uint32_t dev_id)
{
	adpt_api_t *p_adpt_api = NULL;

	p_adpt_api = adpt_api_ptr_get(dev_id);

	if(p_adpt_api == NULL)
		return;

	p_adpt_api->adpt_servcode_func_bitmap = 0x0;

	return;
}

static void adpt_hppe_servcode_func_unregister(a_uint32_t dev_id, adpt_api_t *p_adpt_api)
{
	if(p_adpt_api == NULL)
		return;

	p_adpt_api->adpt_servcode_config_set = NULL;
	p_adpt_api->adpt_servcode_config_get = NULL;
	p_adpt_api->adpt_servcode_loopcheck_en = NULL;
	p_adpt_api->adpt_servcode_loopcheck_status_get = NULL;

	return;
}

sw_error_t adpt_hppe_servcode_init(a_uint32_t dev_id)
{
	adpt_api_t *p_adpt_api = NULL;

	p_adpt_api = adpt_api_ptr_get(dev_id);

	if(p_adpt_api == NULL)
		return SW_FAIL;

	adpt_hppe_servcode_func_unregister(dev_id, p_adpt_api);

	if(p_adpt_api->adpt_servcode_func_bitmap & (1<<FUNC_SERVCODE_CONFIG_SET))
		p_adpt_api->adpt_servcode_config_set = adpt_hppe_servcode_config_set;
	if(p_adpt_api->adpt_servcode_func_bitmap & (1<<FUNC_SERVCODE_CONFIG_GET))
		p_adpt_api->adpt_servcode_config_get = adpt_hppe_servcode_config_get;
	if(p_adpt_api->adpt_servcode_func_bitmap & (1<<FUNC_SERVCODE_LOOPCHECK_EN))
		p_adpt_api->adpt_servcode_loopcheck_en = adpt_hppe_servcode_loopcheck_en;
	if(p_adpt_api->adpt_servcode_func_bitmap & (1<<FUNC_SERVCODE_LOOPCHECK_STATUS_GET))
		p_adpt_api->adpt_servcode_loopcheck_status_get = adpt_hppe_servcode_loopcheck_status_get;

	return SW_OK;
}

/**
 * @}
 */



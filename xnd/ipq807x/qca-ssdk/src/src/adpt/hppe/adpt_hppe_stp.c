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
#include "hppe_stp_reg.h"
#include "hppe_stp.h"
#include "adpt.h"

sw_error_t
adpt_hppe_stp_port_state_get(a_uint32_t dev_id, a_uint32_t st_id,
                       fal_port_t port_id, fal_stp_state_t * state)
{
	union cst_state_u cst_state;

	memset(&cst_state, 0, sizeof(cst_state));
	ADPT_DEV_ID_CHECK(dev_id);
	ADPT_NULL_POINT_CHECK(state);
	if (FAL_SINGLE_STP_ID != st_id)
		return SW_BAD_PARAM;

	SW_RTN_ON_ERROR(hppe_cst_state_get(dev_id, port_id, &cst_state));

	if (cst_state.bf.port_state == 0)
		*state = FAL_STP_DISABLED;
	else if (cst_state.bf.port_state == 1)
		*state = FAL_STP_BLOKING;
	else if (cst_state.bf.port_state == 2)
		*state = FAL_STP_LEARNING;
	else if (cst_state.bf.port_state == 3)
		*state = FAL_STP_FARWARDING;
	else
		*state = FAL_STP_STATE_BUTT;

	return SW_OK;
}

sw_error_t
adpt_hppe_stp_port_state_set(a_uint32_t dev_id, a_uint32_t st_id,
                       fal_port_t port_id, fal_stp_state_t state)
{
	union cst_state_u cst_state;

	memset(&cst_state, 0, sizeof(cst_state));

	/* stp port_id just support physical port, not support trunk and virtual port */
	if (FAL_PORT_ID_TYPE(port_id) != 0)
		return SW_BAD_PARAM;

	if (port_id >= CST_STATE_MAX_ENTRY)
		return SW_OUT_OF_RANGE;

	ADPT_DEV_ID_CHECK(dev_id);
	if (FAL_SINGLE_STP_ID != st_id)
		return SW_BAD_PARAM;

	if (state == FAL_STP_DISABLED)
		cst_state.bf.port_state = 0;
	else if (state == FAL_STP_BLOKING || state == FAL_STP_LISTENING)
		cst_state.bf.port_state = 1;
	else if (state == FAL_STP_LEARNING)
		cst_state.bf.port_state = 2;
	else if (state == FAL_STP_FARWARDING)
		cst_state.bf.port_state = 3;

	SW_RTN_ON_ERROR(hppe_cst_state_set(dev_id, port_id, &cst_state));

	return SW_OK;
}

void adpt_hppe_stp_func_bitmap_init(a_uint32_t dev_id)
{
	adpt_api_t *p_adpt_api = NULL;

	p_adpt_api = adpt_api_ptr_get(dev_id);

	if(p_adpt_api == NULL)
		return;

	p_adpt_api->adpt_stp_func_bitmap = ((1 << FUNC_STP_PORT_STATE_SET) |
						(1 << FUNC_STP_PORT_STATE_GET));

	return;
}

static void adpt_hppe_stp_func_unregister(a_uint32_t dev_id, adpt_api_t *p_adpt_api)
{
	if(p_adpt_api == NULL)
		return;

	p_adpt_api->adpt_stp_port_state_get = NULL;
	p_adpt_api->adpt_stp_port_state_set = NULL;

	return;
}

sw_error_t
adpt_hppe_stp_init(a_uint32_t dev_id)
{
	adpt_api_t *p_adpt_api = NULL;

	p_adpt_api = adpt_api_ptr_get(dev_id);

	if(p_adpt_api == NULL)
		return SW_FAIL;

	adpt_hppe_stp_func_unregister(dev_id, p_adpt_api);

	if (p_adpt_api->adpt_stp_func_bitmap & (1 << FUNC_STP_PORT_STATE_GET))
		p_adpt_api->adpt_stp_port_state_get = adpt_hppe_stp_port_state_get;
	if (p_adpt_api->adpt_stp_func_bitmap & (1 << FUNC_STP_PORT_STATE_SET))
		p_adpt_api->adpt_stp_port_state_set = adpt_hppe_stp_port_state_set;


	return SW_OK;
}

/**
 * @}
 */
